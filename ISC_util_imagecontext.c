/***************************************************************************//**
 * \file ISC_util_imagecontext.c
 * \brief Structure for storing image information going into a module.
 *
 * ISC_util_imagecontext.c describes a data structure for storing information
 * about an image destined to enter/exit an ISC Pipeline module.  This allows
 * modules to have a context for which to do any processing necessary on the
 * image, hence it is called ImageContext.
*******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <cc3.h>
#include "ISC_util_imagecontext.h"

/**
 * \brief Set up an Image Context from current camera state.
 *
 * ISC_util_imagecontext_getfromcurrent() grabs as much of the current camera
 * state as possible from the internal CMUcam3 variables, and for everything
 * else it is supplied in the function parameters.  It will return a nice,
 * ready-to-use ISC_util_imagecontext structure, which can be used by any
 * module.
 */
ISC_util_imagecontext ISC_util_imagecontext_getfromcurrent( cc3_camera_resolution_t res, cc3_colorspace_t cs )
{
	ISC_util_imagecontext c;

	c.frame = cc3_g_pixbuf_frame;
	c.resolution = res;
	c.colorspace = cs;

	return c;
}
