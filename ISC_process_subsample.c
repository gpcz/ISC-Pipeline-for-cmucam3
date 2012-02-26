/***************************************************************************//**
 * \file ISC_process_subsample.c
 * \brief Subsampling module.
 *
 * ISC_process_subsample.c contains the data structures and function prototypes 
 * necessary for performing fast mean-based and random-based image downsampling 
 * on the CMUcam3, as the cc3 API has not finished this functionality yet as
 * of this writing.
*******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <cc3.h>

#include "ISC_process_subsample.h"

#include "ISC_util_assert.h"
#include "ISC_util_common.h"
#include "ISC_util_imagecontext.h"

/**
 * \brief Start ISC_process_subsample module.
 *
 * ISC_process_subsample_start starts the ISC Pipeline subsampling system.
 *
 * \param context The image context.
 * \param skipFactorX The factor to downscale the image in the horizontal axis.
 * \param skipFactorY The factor to downscale the image in the vertical axis.
 * \param subType The type of subsampling to perform.
 * \return The State Structure for a ISC_process_subsample module.
 */
__attribute__((gnu_inline)) inline ISC_process_subsample *ISC_process_subsample_start( ISC_util_imagecontext context, uint8_t skipFactorX, uint8_t skipFactorY, cc3_subsample_mode_t subType )
{
	ISC_process_subsample *ips;

	// Make the new data type.
	// MEMORY IS ALLOCATED HERE.
	ips = malloc( sizeof( ISC_process_subsample ) );

	// Set the user-defines.
	ips->theContext = context;
	ips->skipFactorX = skipFactorX;
	ips->skipFactorY = skipFactorY;
	ips->subType = subType;

	// A few sanity checks:
	if ( ips->theContext.frame.width % ips->skipFactorX != 0 )
		ISC_util_assert_message( "Subsample skipX factors must be a factor of the width!" );
	if ( ips->theContext.frame.height % ips->skipFactorY != 0 )
		ISC_util_assert_message( "Subsample skipY factors must be a factor of the height!" );

	// Set the System-Handleds and the ISC_pipeline requirements.	
	ips->skipShiftFactorX = PowerOfTwoDetect( ips->skipFactorX );
	ips->skipShiftFactorY = PowerOfTwoDetect( ips->skipFactorY );
	ips->endWidth = ips->theContext.frame.width / ips->skipFactorX;
	ips->endHeight = ips->theContext.frame.height / ips->skipFactorY;
	ips->linesLeft = ips->theContext.frame.height;

	// Set up the rowqueue.
	ips->rq = ISC_util_rowqueue_start( ips->skipFactorY );

	// Alright, let's do this.
	return ips;
}

/**
 * \brief ISC_process_subsample feed function.
 *
 * ISC_process_subsample_feed feeds a row into the subsample module.
 *
 * \param ips The State Structure of the module.
 * \param row The incoming row to be fed.
 */
__attribute__((gnu_inline)) inline void ISC_process_subsample_feed( ISC_process_subsample *ips, uint8_t *row )
{
	// Not very complicated -- just jam the newest row into the queue.
	// If the pipeline screws this up somehow, it will throw an assert in
	// rowQueue.
	if ( row )
		ISC_util_rowqueue_feed( ips->rq, row );
}

/**
 * \brief ISC_process_subsample process function.
 *
 * ISC_process_subsample_process returns the next row from the module.
 *
 * \param ips The State Structure of the module.
 * \return The subsampled row from the module.
 */
__attribute__((gnu_inline)) inline uint8_t *ISC_process_subsample_process( ISC_process_subsample *ips )
{
	uint8_t **rowArray;
	uint8_t *finishedRow;
	uint16_t offset = ips->skipFactorX;
	uint16_t countX, countY, countMeta;
	uint16_t whichPixel = 0;
	uint32_t sumR, sumG, sumB;

	if ( ips->rq->currentSize == ips->skipFactorY )
	{
		finishedRow = malloc( sizeof(uint8_t)*3*ips->endWidth );

		// MEMORY IS ALLOCATED HERE.
		rowArray = malloc( sizeof(uint8_t*) * ips->skipFactorY );

		// This data structure is way more convenient to handle when
		// doing for-loops of rows like we're about to do.
		ISC_util_rowqueue_make_rowarray( ips->rq, rowArray );

		for ( countMeta = 0; countMeta < ips->endWidth; countMeta++ )
		{
			// Ok, we're trying to find the mean.  First step is to
			// zero some temporary R, G, and B variables so we can 
			// find a sum of all the pixels in the big rectangle of
			// pixels we're trying to represent with this mean.
			sumR = 0; sumG = 0; sumB = 0;

			// Add all the Rs, Gs, and Bs up in the rectangle.
			for ( countX = offset-ips->skipFactorX; countX < offset; countX++ )
			{
				for ( countY = 0; countY < ips->skipFactorY; countY++ )
				{
					sumR += rowArray[countY][countX*3];
					sumG += rowArray[countY][countX*3+1];
					sumB += rowArray[countY][countX*3+2];
				}
			}
	
			// Next, we want to divide by the number of horizontal
			// and vertical pixels.  However, we can speed this
			// process up by doing bit-shifts if the number of
			// horizontal/vertical pixels is a factor of 256.
			if ( ips->skipShiftFactorX == 0 )
			{
				// No speed boost :(
				sumR /= ips->skipFactorX;
				sumG /= ips->skipFactorX;
				sumB /= ips->skipFactorX;
			}
			else
			{
				// Speed boost!
				sumR = sumR >> ips->skipShiftFactorX;
				sumG = sumG >> ips->skipShiftFactorX;
				sumB = sumB >> ips->skipShiftFactorX;
			}

			if ( ips->skipShiftFactorY == 0 )
			{
				// No speed boost :(
				sumR /= ips->skipFactorY;
				sumG /= ips->skipFactorY;
				sumB /= ips->skipFactorY;	
			}
			else
			{
				// Speed boost!
				sumR = sumR >> ips->skipShiftFactorY;
				sumG = sumG >> ips->skipShiftFactorY;
				sumB = sumB >> ips->skipShiftFactorY;
			}

			// Okay, let's put this new subsampled pixel in the
			// finishedRow so we can send it off to the next
			// module.
			*(finishedRow + (whichPixel*3)) = sumR;
			*(finishedRow + (whichPixel*3) + 1) = sumG;
			*(finishedRow + (whichPixel*3) + 2) = sumB;

			// Increment/handle the variables we use to keep
			// counts of pixels.
			whichPixel++;
			offset += ips->skipFactorX;
		}

		// We don't need the rowArray anymore, so dellocate it.
		free( rowArray );

		// Destroying the rowArray didn't get rid of the rows in the
		// rowQueue, and we don't need 'em anymore, so let's get rid
		// of them.
		for ( countX = 0; countX < ips->skipFactorY; countX++ )
		{
			free(ISC_util_rowqueue_process( ips->rq ));
		}

		// This memory is going to be handled by the next module
		// in the pipeline.  We don't have to worry about it.
		return finishedRow;
	}
	else
	{
		return NULL;
	}
}

/**
 * \brief ISC_process_subsample context function.
 *
 * ISC_process_subsample_context returns the output Image Context of the
 * ISC_process_subsample module for the next module in the pipeline.
 *
 * \param ips The State Structure of the module.
 * \return The output Image Context of the module.
 */
__attribute__((gnu_inline)) inline ISC_util_imagecontext ISC_process_subsample_context( ISC_process_subsample *ips )
{
	ISC_util_imagecontext out;

	out = ips->theContext;
	out.frame.width = ips->endWidth;
	out.frame.height = ips->endHeight;

	return out;
}

/**
 * \brief ISC_process_subsample end function.
 *
 * ISC_process_subsample_end ends the ISC_process_subsample module.
 *
 * \param ips The State Structure of the module.
 */
__attribute__((gnu_inline)) inline void ISC_process_subsample_end( ISC_process_subsample *ips )
{
	ISC_util_rowqueue_end( ips->rq );
	free(ips);
}

/**
 * \brief ISC_process_subsample running function.
 *
 * ISC_process_subsample_running returns whether or not the module is still
 * running (aka, not finished processing the full image).
 *
 * \param ips The State Structure of the module.
 * \return TRUE if running, FALSE if finished.
 */
__attribute__((gnu_inline)) inline bool ISC_process_subsample_running( ISC_process_subsample *ips )
{
	return !ips->finished;
}

