/***************************************************************************//**
 * \file ISC_out_jpeg.c
 * \brief Module for encoding images as JPEG files.
 *
 * ISC_out_jpeg.c contains the functions for saving processed images
 * in the JPEG format.
*******************************************************************************/

#include "ISC_out_jpeg.h"
#include "ISC_util_assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

#include "ISC_util_imagecontext.h"

/**
 * \brief Creates an ISC_out_jpeg module.
 *
 * This function creates a new ISC_out_jpeg module.  Give it the image
 * context of the image expected to come in, and give it the file pointer to
 * dump the file to.  It will return an ISC_out_jpeg state structure.
 *
 * \param context The intended context of the image to feed into the module.
 * \param fp The file pointer.  Make this stdout to send it over the serial port.
 * \return ISC_out_jpeg state structure.
 */
__attribute__((gnu_inline)) inline ISC_out_jpeg *ISC_out_jpeg_start( ISC_util_imagecontext context, FILE *fp )
{
    ISC_out_jpeg *temp = (ISC_out_jpeg *)malloc( sizeof( ISC_out_jpeg ) );
    if ( !temp )
        ISC_util_assert_message( "Ran out of memory creating the JPEG compression schema." );

	temp->theContext = context;
	temp->filePointer = fp;

    temp->compressInfo.err = jpeg_std_error( &temp->jpegError );
    jpeg_create_compress( &temp->compressInfo );
    
    temp->compressInfo.image_width = temp->theContext.frame.width;
    temp->compressInfo.image_height = temp->theContext.frame.height;
    temp->compressInfo.input_components = 3;
    temp->compressInfo.in_color_space = JCS_RGB;
    
    jpeg_set_defaults( &temp->compressInfo );
    jpeg_set_quality( &temp->compressInfo, 85, 1 /*true*/ );
    
    jpeg_stdio_dest(&temp->compressInfo, temp->filePointer);

    jpeg_start_compress(&temp->compressInfo, TRUE);

	temp->rowsLeft = temp->theContext.frame.height;
	temp->finished = false;

    return temp;
}

/**
 * \brief Feed a row into an ISC_out_jpeg module.
 *
 * This module feeds a row into the ISC_out_jpeg module, which gives the JPEG
 * module the newest information.
 *
 * \param ijc The module's state structure.
 * \param row A pointer to a row of pixels.
 * \return Nothing.
 */
__attribute__((gnu_inline)) inline void ISC_out_jpeg_feed( ISC_out_jpeg *ijc, uint8_t *row )
{
    JSAMPROW jpegRow[1];
    
	if ( ijc->rowsLeft > 0 )
	{
    	jpegRow[0] = row;
    	jpeg_write_scanlines(&ijc->compressInfo, jpegRow, 1);
		free(row);
		ijc->rowsLeft--;

		if ( ijc->rowsLeft == 0 )
			ijc->finished = true;
	}
}

/**
 * \brief End an ISC_out_jpeg module.
 *
 * This function clears up the state structure for an ISC_out_jpeg module.
 *
 * \param ijc The module's state structure.
 * \return Nothing.
 */
__attribute__((gnu_inline)) inline void ISC_out_jpeg_end( ISC_out_jpeg *ijc )
{
    jpeg_finish_compress(&ijc->compressInfo);
    jpeg_destroy_compress(&ijc->compressInfo);
    free(ijc);
}

/**
 * \brief Check to see if a JPEG module is running.
 *
 * This function checks to see if the JPEG module still needs to do work.
 *
 * \param ijc The module's state structure.
 * \return Whether or not the ISC_out_jpeg module still needs to do work.
 */
__attribute__((gnu_inline)) inline bool ISC_out_jpeg_running( ISC_out_jpeg *ijc )
{
	return !ijc->finished;
}
