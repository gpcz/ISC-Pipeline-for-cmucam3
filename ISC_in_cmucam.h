/***************************************************************************//**
 * \file ISC_in_cmucam.h
 * \brief Module for obtaining images from the CMUcam3.
 *
 * ISC_in_cmucam.h describes a module for extracting image rows from the CMUcam3
 * smart-camera's FIFO pixbuf.  This is mainly a set of wrapper functions for
 * cc3 API functions, but it makes programs using ISC Pipeline look much
 * cleaner and more organized.
*******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "ISC_util_imagecontext.h"

#ifndef _ISC_IN_CMUCAM_H_
#define _ISC_IN_CMUCAM_H_

/**
 * \brief In-Module for CMUcam3.
 *
 * ISC_in_cmucam extracts image rows from the CMUcam3's FIFO pixbuf.
 */
typedef struct
{
	// USER-DEFINED
	ISC_util_imagecontext theContext; /*!< The Image Context of the camera's output. */

	// HANDLED BY FUNCTIONS
	//uint8_t *outRow; /*!< The latest row coming out of the camera. */
	uint16_t linesLeft; /*!< The amount of rows left to come out of the camera. */

	// ISC_PIPELINE REQUIREMENTS
	bool finished; /*!< Is the module done with the frame in question? */
} ISC_in_cmucam;

ISC_in_cmucam *ISC_in_cmucam_start( ISC_util_imagecontext context );
ISC_util_imagecontext ISC_in_cmucam_context( ISC_in_cmucam * );
uint8_t *ISC_in_cmucam_process( ISC_in_cmucam * );
void ISC_in_cmucam_end( ISC_in_cmucam * );
bool ISC_in_cmucam_running( ISC_in_cmucam * );

#endif

