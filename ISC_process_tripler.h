/***************************************************************************//**
 * \file ISC_process_tripler.h
 * \brief Module for converting monochrome images to 3-channel.
 *
 * ISC_process_clamp_tripler is a module designed to turn a 1-channel image
 * (monochrome) to a 3-channel image (RGB) so that it can be saved as a PPM or
 * JPEG image (these formats require 3-channel images).
*******************************************************************************/

#ifndef _ISC_PROCESS_TRIPLER_H_
#define _ISC_PROCESS_TRIPLER_H_

#include <stdint.h>
#include "ISC_util_imagecontext.h"
#include "ISC_util_rowqueue.h"

/**
 * \brief Process-Module for Image Colorspace Tripling.
 *
 * ISC_process_clamp_colorspace is a module designed to "triple" a monochrome
 * image.  Basically, it converts a 1-channel image to a 3-channel so it can be
 * saved properly in a variety of formats that have hard-coded 3-channel
 * requirements.
 */
typedef struct
{
	//---------------------------USER-EDITED STUFF------------------------------
    ISC_util_imagecontext theContext; //!< The input image context.
    //---------------------------INTERNAL STUFF---------------------------------
	ISC_util_rowqueue *rqueue; //!< ISCRowQueue for temporary storage of rows.
	ISC_util_imagecontext outputContext; //!< The output image context.
    uint16_t remainingTripleCount; //!< The number of rows left to triple.
	uint16_t width; //!< The width of the image.
	//uint16_t height; //!< The height of the image.
} ISC_process_tripler;

//--------------------------------PROTOTYPES------------------------------------
ISC_process_tripler *ISC_process_tripler_start( ISC_util_imagecontext );
void ISC_process_tripler_feed( ISC_process_tripler *, uint8_t * );
uint8_t *ISC_process_tripler_process( ISC_process_tripler * );
void ISC_process_tripler_end( ISC_process_tripler * );

ISC_util_imagecontext ISC_process_tripler_context( ISC_process_tripler * );
bool ISC_process_tripler_running( ISC_process_tripler * );

#endif
