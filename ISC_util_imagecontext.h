/***************************************************************************//**
 * \file ISC_util_imagecontext.h
 * \brief Structure for storing image information going into a module.
 *
 * ISC_util_imagecontext.h describes a data structure for storing information
 * about an image destined to enter/exit an ISC Pipeline module.  This allows
 * modules to have a context for which to do any processing necessary on the
 * image, hence it is called ImageContext.
*******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <cc3.h>

#ifndef _ISC_UTIL_IMAGECONTEXT_H_
#define _ISC_UTIL_IMAGECONTEXT_H_

/**
 * \brief Information about an image going in/out a pipeline.
 *
 * ISC_util_imagecontext describes the image going into/out of a module.  This
 * includes size, region-of-interest, colorspace, channel, and much more
 * information that is pertinent.
 */
typedef struct
{
	cc3_frame_t frame; /*!< Information about the dimensions of the image.*/
	cc3_camera_resolution_t resolution; /*!< Resolution of the image. */
	cc3_colorspace_t colorspace; /*!< Colorspace of the image. */
} ISC_util_imagecontext;

ISC_util_imagecontext ISC_util_imagecontext_getfromcurrent( cc3_camera_resolution_t, cc3_colorspace_t );

#endif
