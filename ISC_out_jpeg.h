/***************************************************************************//**
 * \file ISC_out_jpeg.h
 * \brief Module for encoding images as JPEG files.
 *
 * ISC_out_jpeg.h contains the structs and prototypes for saving processed
 * images in the JPEG format.
*******************************************************************************/

#ifndef _ISC_OUT_JPEG_H_
#define _ISC_OUT_JPEG_H_

#include <stdio.h>
#include <stdint.h>
#include <jpeglib.h>

#include "ISC_util_imagecontext.h"

/**
 * \brief Out-Module for JPEG output.
 *
 * ISC_out_jpeg is a module for exporting JPEG images from the CMUcam3.  Direct
 * the file pointer to stdout if you want to send stuff over the serial port,
 * or use a file pointer obtained from the SD card's filesystem.
 */
typedef struct
{
    //----------------------------USER-DEFINED----------------------------------
    ISC_util_imagecontext theContext; /*!< The image context. */
	FILE *filePointer; /*!< The file pointer to send the JPEG to. */
    //----------------------------SYSTEM-HANDLED--------------------------------
    struct jpeg_compress_struct compressInfo; /*!< An internal data structure of libjpeg. */
    struct jpeg_error_mgr jpegError; /*!< A structure used by libjpeg for error handling. */
	uint16_t rowsLeft; /*!< Amount of rows left to process. */
	//-------------------------ISC_PIPELINE REQUIRED----------------------------
    bool finished; /*!< Is the module finished? */
} ISC_out_jpeg;

ISC_out_jpeg *ISC_out_jpeg_start( ISC_util_imagecontext context, FILE *fp );
void ISC_out_jpeg_feed( ISC_out_jpeg *ijc, uint8_t *row );
void ISC_out_jpeg_end( ISC_out_jpeg *ijc );
bool ISC_out_jpeg_running( ISC_out_jpeg *ijc );

#endif
