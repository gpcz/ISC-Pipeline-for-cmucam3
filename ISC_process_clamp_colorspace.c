/***************************************************************************//**
 * \file ISC_process_clamp_colorspace.c
 * \brief Module for removing extraneous colorspace info.
 *
 * ISC_process_clamp_colorspace.c removes extraneous color information from a
 * scanline if a region of interest is specified.  This will make modules such 
 * as convolution run faster due to less color information needing to be 
 * processed.
*******************************************************************************/

#include <cc3.h>
#include <stdlib.h>

#include "ISC_util_common.h"
#include "ISC_util_assert.h"
#include "ISC_util_imagecontext.h"
#include "ISC_util_rowqueue.h"
#include "ISC_process_clamp_colorspace.h"

/**
 *  \brief Starts an ISC_process_clamp_colorspace module.
 *
 *  This function creates an ISC_process_clamp_colorspace module, which
 *  removes color information from an image however the programmer asks.
 *
 *  \param context The image context of the desired output image.
 *  \return A pointer to an ISC_process_clamp_colorspace state structure.
 */
__attribute__((gnu_inline)) inline ISC_process_clamp_colorspace *ISC_process_clamp_colorspace_start( ISC_util_imagecontext context )
{
	ISC_process_clamp_colorspace *ipcc;
	
	// Make the new state structure.
	// MEMORY IS ALLOCATED HERE.
	ipcc = malloc( sizeof( ISC_process_clamp_colorspace ) );
	if ( !ipcc )
		ISC_util_assert_message( "FATAL: Not enough memory to allocate clamp_colorspace!" );
	
	// If the image doesn't have 3 channels and a channel of interest for
	// clamping, there's something very wrong going on, and we might as 
	// well just assert now.
	if ( context.frame.channels < 3 )
		ISC_util_assert_message( "FATAL: Attempt to clamp a monochrome image!" );
	if ( context.frame.coi == CC3_CHANNEL_ALL )
		ISC_util_assert_message( "FATAL: No specific channel of interest declared!" );

	// Transfer the image context and set up the quick width/height variables.
	ipcc->theContext = context;
	ipcc->width = context.frame.width;
	//ipcc->height = context.frame.height;

	// Prepare the output context.
	ipcc->outputContext = ipcc->theContext;
	ipcc->outputContext.frame.channels = 1;
	ipcc->outputContext.colorspace = CC3_COLORSPACE_MONOCHROME;
	ipcc->outputContext.frame.coi = CC3_CHANNEL_SINGLE;
	
	// Make a rowqueue.
	ipcc->rqueue = ISC_util_rowqueue_start( 3 );
	
	// Store the amount of rows left to clamp (all the ones in the image).
	ipcc->remainingClampCount = context.frame.height;
	
	return ipcc;
}

/**
 *  \brief Feeds a row of pixels into the colorspace clamper.
 *
 *  This function feeds a row of pixels into the colorspace clamper so it has
 *  data to work with when processing.
 *
 *  \param ipcc The module state structure.
 *  \param row The pixel row.
 */
__attribute__((gnu_inline)) inline void ISC_process_clamp_colorspace_feed( ISC_process_clamp_colorspace *ipcc, uint8_t *row )
{
	// Pretty simple feeder.  Just put the new row into the rowqueue.
	ISC_util_rowqueue_feed( ipcc->rqueue, row );
}

/**
 *  \brief Spits out a freshly-processed row of color-clamped pixels.
 *
 *  This function processes and returns a row of pixels from the image you fed
 *  it in, but clamped by colorspace like you asked.
 *
 *  \param ipcc The module state structure.
 *  \return The processed pixel row.
 */
__attribute__((gnu_inline)) inline uint8_t *ISC_process_clamp_colorspace_process( ISC_process_clamp_colorspace *ipcc )
{
	uint8_t *fullRow, *tempRow;
	uint8_t *newRow;
	uint16_t x;

	// Get the full 3-channel row to clamp.
	fullRow = ISC_util_rowqueue_process( ipcc->rqueue );
	tempRow = fullRow;

	// Allocate some row memory for the clamped row.
	newRow = MallocRow( &ipcc->outputContext );

	// What we're doing here is moving to the right color channel of interest
	// beforehand so we just add 3 to get to the next pixel in fullRow within 
	// our inner loop (beats adding coi within the for-loop over and over).
	// WARNING: This line will fail miserably if the cc3_channel_t enumeration
	// is modified in any way, since it relies on R=0, G=1, B=2, etc.  If they
	// change this, just do an array of if-statements that shift you to the
	// right place using pointer addition.
	fullRow += ipcc->theContext.frame.coi;

	for ( x = 0; x < ipcc->width; x++ )
	{
		// Transfer the pixel from fullRow to newRow.
		*(newRow+x) = *fullRow;
		// Go three spaces forward (next pixel).
		fullRow += 3;
	}

	ipcc->remainingClampCount--;
	
	// The row has been clamped, so its usefulness is now zero.
	free( tempRow );

	return newRow;
}

/**
 *  \brief Ends an ISC_process_clamp_colorspace module.
 *
 *  Cleans up the memory usage of an ISC_process_clamp_colorspace module
 *  for when it is no longer needed.
 *
 *  \param ipcc The module state structure.
 *  \return Peace of mind.
 */
__attribute__((gnu_inline)) inline void ISC_process_clamp_colorspace_end( ISC_process_clamp_colorspace *ipcc )
{
	// Empty out and dellocate the rowqueue.
	ISC_util_rowqueue_end( ipcc->rqueue );
	
	// We're almost done...
	free( ipcc );
}

/**
 *  \brief Returns the current image context.
 *
 *  Returns the image context.
 *
 *  \param ipcc The module state structure.
 *  \return Image context.
 */
__attribute__((gnu_inline)) inline ISC_util_imagecontext ISC_process_clamp_colorspace_context( ISC_process_clamp_colorspace *ipcc )
{
	ISC_util_imagecontext theContext;

	theContext = ipcc->outputContext;
	return theContext;
}

/**
 *  \brief Returns whether or not the module is still running.
 *
 *  Returns whether or not the module is still doing work.
 *
 *  \param ipcc The module state structure.
 *  \return Whether or not the module is still doing work.
 */
__attribute__((gnu_inline)) inline bool ISC_process_clamp_colorspace_running( ISC_process_clamp_colorspace *ipcc )
{
	if ( ipcc->remainingClampCount > 0 )
		return true;
	else
		return false;
}
