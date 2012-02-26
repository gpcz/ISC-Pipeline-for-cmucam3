/***************************************************************************//**
 * \file ISC_in_cmucam.c
 * \brief Module for obtaining images from the CMUcam3.
 *
 * ISC_in_cmucam.c contains the functions necessary to obtain scanlines from the
 * CMUcam3's pixbuf.  Although these are basically just wrapper functions, it 
 * helps to have them because it keeps the pipeline loop code clean-looking.
*******************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <cc3.h>

#include "ISC_in_cmucam.h"
#include "ISC_util_assert.h"
#include "ISC_util_imagecontext.h"

/**
 * \brief Creates a new ISC_in_cmucam module.
 *
 * ISC_in_cmucam_start creates a new state structure for an ISC_in_cmucam_start
 * module.
 *
 * \param context The Image Context of the image coming from the CMUcam3.
 * \return An allocated memory structure containing the current state of the ISC_in_cmucam module.
 */
__attribute__((gnu_inline)) inline ISC_in_cmucam *ISC_in_cmucam_start(ISC_util_imagecontext context)
{
	// Create the new structure.
	// MEMORY IS ALLOCATED HERE.
	ISC_in_cmucam *iic = malloc( sizeof( ISC_in_cmucam ) );
	if ( !iic )
		ISC_util_assert_message( "FATAL: Not enough memory to allocate an ISC_in_cmucam!\n" );

	// Create the outRow for the process function.
	// MEMORY IS ALLOCATED HERE.
	//iic->outRow = cc3_malloc_rows(1);
	/*if ( !iic->outRow )
		ISC_util_assert_message( "FATAL: Not enough memory to allocate an ISC_in_cmucam outRow!\n" );*/

	// Set up initial variables.
	iic->linesLeft = cc3_g_pixbuf_frame.height;
	iic->finished = 0;

	iic->theContext = context;

	// Return the new data structure, ready for processing.
	return iic;
}

/**
 * \brief Returns the ISC_in_cmucam module's Image Context.
 *
 * ISC_in_cmucam_context returns the ISC_in_cmucam module's Image Context so
 * it may be used as input for the next module in the pipeline.
 *
 * \param iic The ISC_in_cmucam state structure.
 * \return The ISC_in_cmucam's Image Context.
 */
__attribute__((gnu_inline)) inline ISC_util_imagecontext ISC_in_cmucam_context( ISC_in_cmucam *iic )
{
	return iic->theContext;
}

/**
 * \brief Gets a new row of data from the CMUcam3.
 *
 * ISC_in_cmucam_process grabs a new row from the CMUcam3's FIFO queue.  The
 * memory coming out of this function is assumed to be owned by whatever
 * module accepts it next -- this module makes no attempt to free what it
 * created, as per the design spec.
 *
 * \param iic The ISC_in_cmucam state structure.
 * \return The next freshly-allocated image row.
 */
__attribute__((gnu_inline)) inline uint8_t *ISC_in_cmucam_process( ISC_in_cmucam *iic )
{
	uint8_t *outRow;
	// Do we even have to do anything?
	if ( iic->finished )
		return NULL;
	else // It appears we do.
	{
		// Get the memory for the new row.
		// MEMORY IS ALLOCATED HERE, ASSUMED TO BE HANDLED EXTERNALLY.
		outRow = cc3_malloc_rows(1);

		// Get the row we need.
		cc3_pixbuf_read_rows( outRow, 1 );

		// One more row done.
		iic->linesLeft--;
		// If we don't have to do anything more, let the module know.
		if ( iic->linesLeft == 0 )
			iic->finished = 1;

		// Bring back what we've gotten from the 'cam.
		return outRow;
	}
}

/**
 * \brief Cleans up the spent ISC_in_cmucam module.
 *
 * ISC_in_cmucam_end deallocates the memory used by the ISC_in_cmucam module
 * created by ISC_in_cmucam_start.  This should be called after the pipeline
 * is done so as to avoid memory leaks.
 *
 * \param iic The ISC_in_cmucam state structure.
 * \return Nothing but piece of mind that memory is deallocated right.
 */
__attribute__((gnu_inline)) inline void ISC_in_cmucam_end( ISC_in_cmucam *iic )
{
	// Clean up after ISC_in_cmucam_start.
	//free( iic->outRow );
	free( iic );
}

/**
 * \brief Returns whether the module is still running or is finished.
 *
 * ISC_in_cmucam_running returns whether or not the module is currently
 * running.  Running is defined to be whether or not there are lines left
 * to process().
 *
 * \param iic The ISC_in_cmucam state structure.
 * \return TRUE if the module is still running, FALSE if the module is finished.
 */
__attribute__((gnu_inline)) inline bool ISC_in_cmucam_running( ISC_in_cmucam *iic )
{
	return !iic->finished;
}

