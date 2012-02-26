/***************************************************************************//**
 * \file ISC_process_clamp_colorspace.h
 * \brief Module for removing extraneous colorspace info.
 *
 * ISC_process_clamp_colorspace.h removes extraneous color information from a
 * scanline if a channel of interest is specified.  This will make modules such 
 * as convolution run faster due to less color information needing to be 
 * processed.
*******************************************************************************/

#ifndef _ISC_PROCESS_CLAMP_COLORSPACE_H_
#define _ISC_PROCESS_CLAMP_COLORSPACE_H_

#include <stdint.h>
#include "ISC_util_imagecontext.h"
#include "ISC_util_rowqueue.h"

/**
 * \brief Process-Module for Image Colorspace Clamping.
 *
 * ISC_process_clamp_colorspace is a module designed to "clamp" a colorspace.
 * Basically, the Image Context can specify a color channel of interest, and
 * if you feed an RGB image into this module with a color channel of interest,
 * the module will discard all the other color information except for the
 * channel of interest.  This will give you a monochrome image, but it will
 * give you performance increases in certain computationally-intensive modules
 * such as convolution.
 */
typedef struct
{
	//---------------------------USER-EDITED STUFF------------------------------
    ISC_util_imagecontext theContext; //!< The input image context.
    //---------------------------INTERNAL STUFF---------------------------------
	ISC_util_rowqueue *rqueue; //!< ISCRowQueue for temporary storage of rows.
	ISC_util_imagecontext outputContext; //!< The output image context.
    uint16_t remainingClampCount; //!< The number of rows left to clamp.
	uint16_t width; //!< The width of the image.
	//uint16_t height; //!< The height of the image.
} ISC_process_clamp_colorspace;

//--------------------------------PROTOTYPES------------------------------------
ISC_process_clamp_colorspace *ISC_process_clamp_colorspace_start( ISC_util_imagecontext );
void ISC_process_clamp_colorspace_feed( ISC_process_clamp_colorspace *, uint8_t * );
uint8_t *ISC_process_clamp_colorspace_process( ISC_process_clamp_colorspace * );
void ISC_process_clamp_colorspace_end( ISC_process_clamp_colorspace * );

ISC_util_imagecontext ISC_process_clamp_colorspace_context( ISC_process_clamp_colorspace * );
bool ISC_process_clamp_colorspace_running( ISC_process_clamp_colorspace * );

#endif
