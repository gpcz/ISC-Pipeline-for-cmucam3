/***************************************************************************//**
 * \file ISC_out_png.c
 * \brief Module for encoding images as PNG files.
 *
 * ISC_out_png.c contains the function code for saving processed images in the
 * PNG (Portable Network Graphics) format.
 *
 * Code inspired from the /projects/png-grab example.
*******************************************************************************/

#include <stdlib.h>
#include <png.h>
#include "ISC_out_png.h"

/**
 * \brief Creates an ISC_out_png module.
 *
 * This function creates a new ISC_out_png module.  Give it the image
 * context of the image expected to come in, and give it the file pointer to
 * dump the file to.  It will return an ISC_out_png state structure.
 *
 * \param context The intended context of the image to feed into the module.
 * \param fp The file pointer.  Make this stdout to send it over the serial port.
 * \return ISC_out_png state structure.
 */
__attribute__((gnu_inline)) inline ISC_out_png *ISC_out_png_start( ISC_util_imagecontext context, FILE *fp )
{
	ISC_out_png *ipw = malloc( sizeof ( ISC_out_png ) );

	ipw->filePointer = fp;
	ipw->theContext = context;

	// Create the png_out_ptr.
	ipw->png_out_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	// Create the png_out_info_ptr.
	ipw->png_out_info_ptr = png_create_info_struct( ipw->png_out_ptr );

	// memory usage (in bytes) = 2^(memlevel+9) + 2^(windowbits+2).
	// This configuration uses 24576 bytes of memory.
	png_set_compression_mem_level( ipw->png_out_ptr, 5 );
	png_set_compression_window_bits( ipw->png_out_ptr, 11 );

	png_init_io( ipw->png_out_ptr, ipw->filePointer );
	png_set_IHDR( ipw->png_out_ptr,
			ipw->png_out_info_ptr,
			ipw->theContext.frame.width,
			ipw->theContext.frame.height,
			8,
			PNG_COLOR_TYPE_RGB,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT );
	png_write_info( ipw->png_out_ptr, ipw->png_out_info_ptr );

	ipw->rowsLeft = ipw->theContext.frame.height;
	ipw->finished = false;

	return ipw;
}

/**
 * \brief Feed a scanline into the ISC_out_png module.
 *
 * This function is used to a feed a new scanline into the ISC_out_png
 * module.
 *
 * \param ipw The state structure of the ISC_out_png module in question.
 * \param row A pointer to a scanline of the image.
 */
__attribute__((gnu_inline)) inline void ISC_out_png_feed( ISC_out_png *ipw, uint8_t *row )
{
	if ( ipw->rowsLeft > 0 )
	{
		png_write_row( ipw->png_out_ptr, row );
		free(row);
		ipw->rowsLeft = ipw->rowsLeft - 1;

		if ( ipw->rowsLeft == 0 )
			ipw->finished = true;
	}
}

/**
 * \brief Ends an ISC_out_png module.
 *
 * This function is used to clean up and free up an ISC_out_png module,
 * including writing the proper endings for the PNG file and freeing
 * the memory used.
 *
 * \param ipw The state structure of the ISC_out_png module in question.
 */
__attribute__((gnu_inline)) inline void ISC_out_png_end( ISC_out_png *ipw )
{
	png_write_end( ipw->png_out_ptr, ipw->png_out_info_ptr );
	png_destroy_write_struct( &ipw->png_out_ptr, &ipw->png_out_info_ptr );
	free(ipw);
}

/**
 * \brief Returns whether ISC_out_png is running.
 *
 * This function returns the current running state of ISC_out_png.  If the
 * module is finished (no more lines to feed()), it will return false.  If there
 * is still work to do, it will return true.
 *
 * \param ipw The state structure of the ISC_out_png module.
 * \return TRUE if running, FALSE if finished.
 */
__attribute__((gnu_inline)) inline bool ISC_out_png_running( ISC_out_png *ipw )
{
	return !ipw->finished;
}

