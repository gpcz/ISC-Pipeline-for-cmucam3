/***************************************************************************//**
 * \file ISC_out_ppm.c
 * \brief Module for PPM image output.
 *
 * ISC_out_ppm.c describes data structures and functions necessary to save/send
 * PPM-formatted images using the CMUcam3.  This code highly emphasizes the
 * scanline-by-scanline approach used by the ISC_histogram and ISC_convolution
 * systems.
*******************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "ISC_out_ppm.h"

/**
 * \brief Creates an ISC_out_ppm module.
 *
 * This function creates a new ISC_out_ppm_start module.  Give it the image
 * context of the image expected to come in, and give it the file pointer to
 * dump the file to.  It will return an ISC_out_ppm state structure.
 *
 * \param context The intended context of the image to feed into the module.
 * \param fp The file pointer.  Make this stdout to send it over the serial port.
 * \return ISC_out_ppm state structure.
 */
__attribute__((gnu_inline)) inline ISC_out_ppm *ISC_out_ppm_start( ISC_util_imagecontext context, FILE *fp )
{
	// Make the new module state structure.
	ISC_out_ppm *ipw = malloc( sizeof( ISC_out_ppm ) );
	
	// Load it up with its initial values.
	ipw->filePointer = fp;
	ipw->finished = 0;

	ipw->theContext = context;
	ipw->width = ipw->theContext.frame.width;
	ipw->height = ipw->theContext.frame.height;
	ipw->rowsLeft = ipw->height;

	//Write the header of the PPM file, since enough stuff is now known
	//to do this.
	fprintf( ipw->filePointer, "P6\n%d %d\n255\n", ipw->width, ipw->height );

	return ipw;
}

/**
 * \brief Accepts rows of image data for PPM conversion.
 *
 * This function accepts a new row of image data into the ISC_out_ppm module.
 *
 * \param ipw The state structure of the ISC_out_ppm module.
 * \param row A pointer to the pixel row to feed in.
 * \return Nothing -- this is a data sink function.
 */
__attribute__((gnu_inline)) inline void ISC_out_ppm_feed( ISC_out_ppm *ipw, uint8_t *row )
{
	uint16_t x;

	if ( row && ipw->finished == 0 )
	{
		// Write out the row of pixels to the PPM file.
		for ( x = 0; x < ipw->width*3; x++ )
		{
			fputc( *(row+x), ipw->filePointer );
		}

		// One more row down, rowsLeft rows to go.  Unless rowsLeft is
		// 0, then we're done.
		ipw->rowsLeft--;
		if ( ipw->rowsLeft == 0 )
			ipw->finished = 1;

		// Feed functions are expected to free row pointers.
		free(row);
	}
}

/**
 * \brief Ends the ISC_out_ppm module.
 *
 * This function dellocates the memory used by ISC_out_ppm.  The user is still
 * expected to close the file pointer used, though.  This is because if the
 * module did this automatically, bad things could happen if it tried to close
 * stdout.
 *
 * \param ipw The state structure of the ISC_out_ppm module.
 * \return Nothing but the minty feeling of deallocated memory.
 */
__attribute__((gnu_inline)) inline void ISC_out_ppm_end( ISC_out_ppm *ipw )
{
	// Mmm...nothing like peace of mind.
	free(ipw);
}

/**
 * \brief Returns whether ISC_out_ppm is running.
 *
 * This function returns the current running state of ISC_out_ppm.  If the
 * module is finished (no more lines to feed()), it will return false.  If there
 * is still work to do, it will return true.
 *
 * \param ipw The state structure of the ISC_out_ppm module.
 * \return TRUE if running, FALSE if finished.
 */
__attribute__((gnu_inline)) inline bool ISC_out_ppm_running( ISC_out_ppm *ipw )
{
	return !ipw->finished;
}

