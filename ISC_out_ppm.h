/***************************************************************************//**
 * \file ISC_out_ppm.h
 * \brief Module for PPM image output.
 *
 * ISC_out_ppm.h describes data structures and functions necessary to save/send
 * PPM-formatted images using the CMUcam3.  This code highly emphasizes the
 * scanline-by-scanline approach used by the ISC_histogram and ISC_convolution
 * systems.
*******************************************************************************/

#ifndef _ISC_OUT_PPM_H_
#define _ISC_OUT_PPM_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#include "ISC_util_imagecontext.h"

/**
 * \brief Out-Module for PPM output.
 *
 * ISC_out_ppm is a module for exporting PPM images from the CMUcam3.  Direct
 * the file pointer to stdout if you want to send stuff over the serial port,
 * or use a file pointer obtained from the SD card's filesystem.
 */
typedef struct
{
    //----------------------------USER-DEFINED----------------------------------
    ISC_util_imagecontext theContext; /*!< The Image Context of the camera's output. */
    FILE *filePointer; /*!< The file pointer to send the PPM to. */
    //----------------------------SYSTEM-HANDLED--------------------------------
    uint16_t width; /*!< Width of the image. */
    uint16_t height; /*!< Height of the image. */
    uint16_t rowsLeft; /*!< The number of rows left to process. */
    //-------------------------ISC_PIPELINE REQUIRED----------------------------
    bool finished; /*!< Is the module finished? */
} ISC_out_ppm;

ISC_out_ppm *ISC_out_ppm_start( ISC_util_imagecontext context, FILE *fp );
void ISC_out_ppm_feed( ISC_out_ppm *, uint8_t * );
void ISC_out_ppm_end( ISC_out_ppm * );
bool ISC_out_ppm_running( ISC_out_ppm * );

#endif
