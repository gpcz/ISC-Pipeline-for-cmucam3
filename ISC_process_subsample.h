/***************************************************************************//**
 * \file ISC_process_subsample.h
 * \brief Subsampling module.
 *
 * ISC_process_subsample.h contains the data structures and function prototypes 
 * necessary for performing fast mean-based and random-based image downsampling 
 * on the CMUcam3, as the cc3 API has not finished this functionality yet as
 * of this writing.
*******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "ISC_util_rowqueue.h"
#include "ISC_util_imagecontext.h"

/**
 * \brief Process-Module for Image Subsampling.
 *
 * ISC_process_subsample is a module designed for performing fast image
 * subsampling with mean-based and randon-pixel-based approaches.  Once the
 * cc3 API provides native support for these, this module will become
 * redundant.
 */
typedef struct
{
	//---------------------------USER-DEFINED-------------------------------
	ISC_util_imagecontext theContext; /*!< The Image Context. */
	uint8_t skipFactorX; //!< The horizontal skip factor of the subsample.
       	uint8_t skipFactorY; //!< The vertical skip factor of the subsample.
	cc3_subsample_mode_t subType; //!< The type of subsampling to do.
	//---------------------------SYSTEM-HANDLED-----------------------------
	uint8_t skipShiftFactorX; //!< Right-shifts needed to perform subsample.
	uint8_t skipShiftFactorY; //!< Left-shifts needed to perform subsample.
	uint16_t endWidth; //!< The width after subsampling.
	uint16_t endHeight; //!< The height after subsampling.
	uint16_t linesLeft; //!< The amount of lines left to subsample.

	ISC_util_rowqueue *rq; //!< The rowqueue for storing rows.
	
	//----------------------ISC_PIPELINE REQUIREMENT------------------------
	bool finished; //!< Is the module finished?
} ISC_process_subsample;

ISC_process_subsample *ISC_process_subsample_start( ISC_util_imagecontext, uint8_t, uint8_t, cc3_subsample_mode_t );
void ISC_process_subsample_feed( ISC_process_subsample *, uint8_t * );
uint8_t *ISC_process_subsample_process( ISC_process_subsample * );
ISC_util_imagecontext ISC_process_subsample_context( ISC_process_subsample * );
void ISC_process_subsample_end( ISC_process_subsample * );
bool ISC_process_subsample_running( ISC_process_subsample * );

