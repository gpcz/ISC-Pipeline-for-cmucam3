/***************************************************************************//**
 * \file ISC_process_tripler.c
 * \brief Module for converting monochrome images to 3-channel.
 *
 * ISC_process_clamp_tripler is a module designed to turn a 1-channel image
 * (monochrome) to a 3-channel image (RGB) so that it can be saved as a PPM or
 * JPEG image (these formats require 3-channel images).
*******************************************************************************/

#include <cc3.h>
#include <stdlib.h>

#include "ISC_util_common.h"
#include "ISC_util_assert.h"
#include "ISC_util_imagecontext.h"
#include "ISC_util_rowqueue.h"
#include "ISC_process_tripler.h"

/**
 *  \brief Starts an ISC_process_tripler module.
 *
 *  This function creates an ISC_process_tripler module, whhich converts a
 *  monochrome image into a fake RGB image for saving.
 *
 *  \param context The image context of the desired output image.
 *  \return A pointer to an ISC_process_tripler state structure.
 */
__attribute__((gnu_inline)) inline ISC_process_tripler *ISC_process_tripler_start( ISC_util_imagecontext context )
{
	ISC_process_tripler *ipcc;
	
	// Make the new state structure.
	// MEMORY IS ALLOCATED HERE.
	ipcc = malloc( sizeof( ISC_process_tripler ) );
	if ( !ipcc )
		ISC_util_assert_message( "FATAL: Not enough memory to allocate clamp_colorspace!" );
	
	// If the image doesn't have 3 channels and a channel of interest for
	// clamping, there's something very wrong going on, and we might as 
	// well just assert now.
	if ( context.frame.channels > 1 )
		ISC_util_assert_message( "FATAL: Attempting to triple a non-monochrome image!" );

	// Transfer the image context and set up the quick width/height variables.
	ipcc->theContext = context;
	ipcc->width = context.frame.width;
	//ipcc->height = context.frame.height;

	// Prepare the output context.
	ipcc->outputContext = ipcc->theContext;
	ipcc->outputContext.frame.channels = 3;
	ipcc->outputContext.colorspace = CC3_COLORSPACE_RGB;
	ipcc->outputContext.frame.coi = CC3_CHANNEL_ALL;
	
	// Make a rowqueue.
	ipcc->rqueue = ISC_util_rowqueue_start( 3 );
	
	// Store the amount of rows left to triple (all the ones in the image).
	ipcc->remainingTripleCount = context.frame.height;
	
	return ipcc;
}

/**
 *  \brief Feeds a row into ISC_process_tripler.
 *
 *  This function feeds a row into the ISC_process_tripler module.
 *
 *  \param ipcc The module state structure.
 *  \param row The image row.
 */
__attribute__((gnu_inline)) inline void ISC_process_tripler_feed( ISC_process_tripler *ipcc, uint8_t *row )
{
	// Pretty simple feeder.  Just put the new row into the rowqueue.
	ISC_util_rowqueue_feed( ipcc->rqueue, row );
}

/**
 *  \brief Processes a row from ISC_process_tripler.
 *
 *  This function processes a row from the ISC_process_tripler module.
 *
 *  \param ipcc The module state structure.
 *  \return The processed image row.
 */
__attribute__((gnu_inline)) inline uint8_t *ISC_process_tripler_process( ISC_process_tripler *ipcc )
{
	uint8_t *fullRow, *tempRow;
	uint8_t *newRow;
	uint16_t x;

	// Get the 1-channel row to triple.
	fullRow = ISC_util_rowqueue_process( ipcc->rqueue );

	// Allocate some row memory for the tripled row.
	newRow = MallocRow( &ipcc->outputContext );
	tempRow = newRow;

	for ( x = 0; x < ipcc->width; x++ )
	{
		// Triple the pixel.
		*newRow = *(fullRow+x);
		*(++newRow) = *(fullRow+x);
		*(++newRow) = *(fullRow+x);
		newRow++;
	}

	ipcc->remainingTripleCount--;
	
	// fullRow has been tripled, so its usefulness is now zero.
	free( fullRow );

	return tempRow;
}

/**
 *  \brief Ends ISC_process_tripler.
 *
 *  This function ends the ISC_process_tripler module.
 *
 *  \param ipcc The module state structure.
 *  \return Peace of mind.
 */
__attribute__((gnu_inline)) inline void ISC_process_tripler_end( ISC_process_tripler *ipcc )
{
	// Empty out and dellocate the rowqueue.
	ISC_util_rowqueue_end( ipcc->rqueue );
	
	// We're almost done...
	free( ipcc );
}

/**
 *  \brief Gets the image context.
 *
 *  This function gets the outbound image context for the ISC_util_imagecontext
 *  module.
 *
 *  \param ipcc The module state structure.
 *  \return The image context.
 */
__attribute__((gnu_inline)) inline ISC_util_imagecontext ISC_process_tripler_context( ISC_process_tripler *ipcc )
{
	ISC_util_imagecontext theContext;

	theContext = ipcc->outputContext;
	return theContext;
}

/**
 *  \brief Determines if the tripler is still running.
 *
 *  This function returns whether or not the tripler module is still doing work.
 *
 *  \param ipcc The module state structure.
 *  \return 0 if the tripler is done, 1 if it is still working.
 */
__attribute__((gnu_inline)) inline bool ISC_process_tripler_running( ISC_process_tripler *ipcc )
{
	if ( ipcc->remainingTripleCount > 0 )
		return true;
	else
		return false;
}
