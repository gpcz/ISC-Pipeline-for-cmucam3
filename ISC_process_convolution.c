/***************************************************************************//**
* \file ISC_process_convolution.c
* \brief Convolution module with support for kernels of arbitrary centering.
*******************************************************************************/

#include <stdlib.h>
#include <stdint.h>

#include "ISC_util_assert.h"
#include "ISC_util_common.h"
#include "ISC_process_convolution.h"
#include "ISC_util_imagecontext.h"

#include <cc3.h>

//-------------------------------KERNEL STUFF----------------------------------

/**
 *  \brief Figures out and sets the kernel's divisor.
 *
 *  This function exists for those who don't feel like writing a for-loop that
 *  adds all the numbers in the kernel up.
 */
__attribute__((gnu_inline)) inline void ISC_process_convolution_setKernelDivisor( ISC_process_convolution_kernel *kern )
{
    uint8_t x, y;
    
    // Kernel sanity check.
    if ( kern->kernelSize > ISC_KERNEL_MAXSIZE )
        ISC_util_assert_message( "FATAL: kernelSize > ISC_KERNEL_MAXSIZE!" );
        
    // Start the count at 0.
    kern->divisor = 0;
    
    // Go through each number and add it to the count.
    for ( y = 0; y < kern->kernelSize; y++ )
        for ( x = 0; x < kern->kernelSize; x++ )
            kern->divisor += kern->matrix[y][x];
}

//------------------------------CONVOLUTION STUFF------------------------------

// Prototype so the compiler doesn't complain.
void ISC_process_convolution_PlantPixel( uint8_t *pixLoc, int16_t sum, int16_t divisor, int16_t tooHigh );

/**
 *  \brief Starts up ISC_process_convolution.
 *
 *  Starts the ISC_process_convolution module.
 *
 *  \param context The ImageContext of the input image.
 *  \param kern The convolution kernel.
 *
 *  \return A pointer to the ISC_process_convolution state structure.
 */
__attribute__((gnu_inline)) inline ISC_process_convolution *ISC_process_convolution_start( ISC_util_imagecontext context, ISC_process_convolution_kernel kern )
{
    uint16_t y;
    uint8_t *newRow;
    ISC_process_convolution *conv = malloc( sizeof( ISC_process_convolution ) );
    
    // First order of business: Check the integrity of the kernel.
    if ( kern.kernelSize > ISC_KERNEL_MAXSIZE )
        ISC_util_assert_message( "FATAL: kernelSize > ISC_KERNEL_MAXSIZE!" );

    if ( kern.centerX >= ISC_KERNEL_MAXSIZE ||
         kern.centerY >= ISC_KERNEL_MAXSIZE    )
        ISC_util_assert_message( "FATAL: centerX or Y > ISC_KERNEL_MAXSIZE!" );
        
    // Set the image context.
    conv->theContext = context;
    conv->width = conv->theContext.frame.width;
    conv->height = conv->theContext.frame.height;

    // Set up the row queue.
    // MEMORY IS ALLOCATED HERE.
    conv->rqueue = ISC_util_rowqueue_start( kern.kernelSize );

    // Transfer the kernel.
    conv->kernel = kern;

    // Determine the divisor.
    ISC_process_convolution_setKernelDivisor( &conv->kernel );
    
    // Fill the rows above the center of the kernel with zeroes to start.
    for ( y = 0; y < conv->kernel.centerY-1; y++ )
    {
		newRow = MallocZeroRow( &conv->theContext );
		ISC_util_rowqueue_feed( conv->rqueue, newRow );
    }
    
    // Final sanity checks.
    if ( conv->rqueue->hardMax != conv->kernel.kernelSize )
        ISC_util_assert_message( "FATAL: rowQueue hardMax wrong size!" );
    
    // Set the remainingConvolveCount.
    conv->remainingConvolveCount = conv->height;
    conv->remainingLoadCount = conv->height;

    return conv;
}

/**
 *  \brief Feeds a row into the ISC_process_convolution module.
 *
 *  Feeds a row of pixel data into the ISC_process_convolution module.
 *
 *  \param conv A pointer to the target ISC_process_convolution state structure.
 *  \param row A pointer to the pixel row.
 *
 *  \return Nothing.
 */
__attribute__((gnu_inline)) inline void ISC_process_convolution_feed( ISC_process_convolution *conv, uint8_t *row )
{
    uint8_t *newRow;

    if ( row )
    {
		ISC_util_rowqueue_feed( conv->rqueue, row );
		conv->remainingLoadCount--;
    }
    else
    {
		if ( conv->remainingLoadCount == 0 )
		{
	    	newRow = MallocZeroRow( &conv->theContext );
	    	ISC_util_rowqueue_feed( conv->rqueue, newRow );
		}
    }
}

/**
 *  \brief Inner convolution loop operation.
 *
 *  This function should never be called straight-out by the user, but rather
 *  is used by ISC_process_convolution_process to write the convoluted pixel.
 *  There are a couple of different things that can happen depending on the
 *  sum or the divisor, and this handles all of them.  It's an inline function
 *  because it's the inner loop of this rather computationally-expensive
 *  operation, and so the added executable code is well worth the speed
 *  increase.
 *
 *  \param pixLoc The location of the pixel to plant.
 *  \param sum The summation of the kernel*pixel values.
 *  \param divisor The divisor.
 *  \param tooHigh The divisor * 255.  This is stored to avoid multiplying the same number over and over again.
 *
 *  \return Nothing.  The result is sent through pixLoc.
 */
__attribute__((gnu_inline)) inline void ISC_process_convolution_PlantPixel( uint8_t *pixLoc, int16_t sum, int16_t divisor, int16_t tooHigh )
{
    if ( sum > tooHigh )
        *pixLoc = 255;
    else if ( sum >= 0 )
    {
        if ( divisor == 0 )
            *pixLoc = ((uint8_t)(sum+128));
        else
            *pixLoc = ((uint8_t)(sum / divisor));
    }
    else
        *pixLoc = 0;
}

/**
 *  \brief Computes and sends out a scanline of convoluted data.
 *
 *  This function is the Process function of the ISC_process_convolution
 *  module, and so it computes a convoluted scanline if the requisite
 *  data for doing so exists in the pipeline.
 *
 *  \param conv A pointer to the ISC_process_convolution state structure.
 *
 *  \return A pointer to a scanline of convoluted image data.
 */
__attribute__((gnu_inline)) inline uint8_t *ISC_process_convolution_process( ISC_process_convolution *conv )
{
    uint16_t col, kx, ky, col_plus_kx;
    uint8_t *pixel;
    int16_t temp;
    int16_t sumR, sumG, sumB;
    int16_t tooHigh = 255 * conv->kernel.divisor;
    uint8_t *tempRow;
    uint8_t **rows;  

    if ( conv->rqueue->currentSize == conv->rqueue->hardMax )
    {
	tempRow = MallocRow( &conv->theContext );
		    
	// Make the row array.
	// MEMORY IS ALLOCATED HERE.
	rows = malloc( sizeof(uint8_t*)*conv->rqueue->hardMax );
	ISC_util_rowqueue_make_rowarray( conv->rqueue, rows );

	// There are two situations that can happen: one-channel mode or three-
	// channel.  However, we don't want to keep checking which one we're
	// doing over and over again in inner loops, since that wastes cycles.
	// So instead, we check it once outside and have two instances of the
	// same code.
	//! \todo Add one-channel version of the convolution code to process.
	for ( col = 0; col < conv->width; col++ )
	{
	    // Don't want funky sum results...
	    sumR = 0;
	    sumG = 0;
	    sumB = 0;

	    for ( ky = 0; ky < conv->kernel.kernelSize; ky++ )
	    {
		for ( kx = 0; kx < conv->kernel.kernelSize; kx++ )
		{
		    
		    col_plus_kx = col + kx;
		    if ( col_plus_kx >= conv->kernel.centerX && col_plus_kx < conv->width )
		    {
			// Find a pointer to the exact pixel.
			pixel = rows[ky] + (col_plus_kx - conv->kernel.centerX)*3;

			// Add to R-sum.
			temp = *pixel;
			sumR += (temp * conv->kernel.matrix[ky][kx]);
				    
			// Add to G-sum.
			temp = *(++pixel);
			sumG += (temp * conv->kernel.matrix[ky][kx]);
				    
			// Add to B-sum.
			temp = *(++pixel);
			sumB += (temp * conv->kernel.matrix[ky][kx]);
		    }
		}   
	    }

	    // Apply divisor rule and place the new pixel into tempRow.
	    ISC_process_convolution_PlantPixel( tempRow + (col*3), sumR, conv->kernel.divisor, tooHigh );
	    ISC_process_convolution_PlantPixel( tempRow + (col*3)+1, sumG, conv->kernel.divisor, tooHigh );
	    ISC_process_convolution_PlantPixel( tempRow + (col*3)+2, sumB, conv->kernel.divisor, tooHigh );
	}

	free(rows);
	free(ISC_util_rowqueue_process( conv->rqueue ));

	conv->remainingConvolveCount--;
	return tempRow;
    }
    else
	return NULL;
}

/**
 *  \brief Ends the ISC_process_convolution module.
 *
 *  This function will clean up the ISC_process_convolution module, freeing
 *  all memory allocated.  If you don't call this, you will leak memory, and
 *  leaking memory on a CMUcam3 is going to end poorly.
 *
 *  \param conv A pointer to the state structure of the module to end.
 *
 *  \return Nothing.
 */
__attribute__((gnu_inline)) inline void ISC_process_convolution_end( ISC_process_convolution *conv )
{
    uint8_t y;
    
    // First order of business: Check to see if we're actually done.
    if ( conv->remainingConvolveCount > 0 )
        ISC_util_assert_message( "FATAL: cleanup before convolution done!" );
    
    // Do away with all remaining rows in the queue.
    for ( y = 0; y < conv->rqueue->currentSize; y++ )
	free(ISC_util_rowqueue_process(conv->rqueue));

    // End the rowqueue.
    ISC_util_rowqueue_end(conv->rqueue);

    // End the convolution module.
    free(conv);
}

/**
 * \brief Returns the module's Image Context.
 *
 * Returns the module's Image Context.
 *
 * \param conv The state structure.
 *
 * \return The image context.
 */
__attribute__((gnu_inline)) inline ISC_util_imagecontext ISC_process_convolution_context( ISC_process_convolution *conv )
{
    ISC_util_imagecontext out;
    out = conv->theContext;

    return out;
}

