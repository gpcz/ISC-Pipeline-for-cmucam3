/***************************************************************************//**
 * \file ISC_out_png.h
 * \brief Module for encoding images as PNG files.
 *
 * ISC_out_png.h contains the prototypes and structs for saving processed images
 * in the PNG (Portable Network Graphics) format.
*******************************************************************************/

#ifndef _ISC_OUT_PNG_H_
#define _ISC_OUT_PNG_H_

#include <stdio.h>
#include <stdint.h>
#include <png.h>

#include "ISC_util_imagecontext.h"

/**
 * \brief Out-Module for PNG output.
 *
 * ISC_out_png is a module for exporting PNG images from the CMUcam3.  Direct
 * the file pointer to stdout if you want to send stuff over the serial port,
 * or use a file pointer obtained from the SD card's filesystem.
 */
typedef struct
{
    //----------------------------USER-DEFINED----------------------------------
	ISC_util_imagecontext theContext; /*!< The image context. */
    FILE *filePointer; /*!< The file pointer to the output PNG. */
    //----------------------------SYSTEM-HANDLED--------------------------------
    png_structp png_out_ptr; /*!< An internal libpng data structure. */
    png_infop png_out_info_ptr; /*!< An internal libpng data structure. */
    uint16_t rowsLeft; /*!< The number of rows left to process. */
    //-------------------------ISC_PIPELINE REQUIRED----------------------------
    bool finished; /*!< Is the module finished? */
} ISC_out_png;

ISC_out_png *ISC_out_png_start( ISC_util_imagecontext context, FILE *fp );
void ISC_out_png_feed( ISC_out_png *, uint8_t * );
void ISC_out_png_end( ISC_out_png * );
bool ISC_out_png_running( ISC_out_png *ipw );

#endif
