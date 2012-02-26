/***************************************************************************//**
 * \file ISC_out_histogram.h
 * \brief Out-Module for Histogram output.
 *
 * ISC_out_histogram.h contains the functions used to perform histograms
 * on a CMUcam3.
*******************************************************************************/

#ifndef _ISC_OUT_HISTOGRAM_H_
#define _ISC_OUT_HISTOGRAM_H_

#include <cc3.h>
#include <stdio.h>

#include "ISC_util_imagecontext.h"

typedef struct
{
	uint32_t *histogram; //!< The histogram.
} ISC_out_histogram_subarea;

typedef struct
{
	ISC_util_imagecontext theContext; /*!< The Image Context. */

	uint16_t width; //!< Width of the image.
	uint16_t height; //!< Height of the image.

	uint8_t Xsubdivisions; //!< Number of X subdivisions.
	uint8_t XsubdivShiftFactor; //!< Used for divide-optimization.  0 if no optimization is going (not a power of two.)
	uint8_t Ysubdivisions; //!< Number of Y subdivisions.
	ISC_out_histogram_subarea *subdivisions; //!< Storage of X-subdivisions.
	uint8_t subsLeft; //!< YSubdivisions left.
	uint8_t linesLeft; //!< Lines left in the subdivision.
	uint8_t binShiftFactor; //!< Used for divide-optimization.  0 if no optimization is used (not a power of two.)
	uint8_t colorBins; //!< The number of color bins.
} ISC_out_histogram;

ISC_out_histogram *ISC_out_histogram_start( ISC_util_imagecontext, uint8_t, uint8_t, uint8_t ); 
void ISC_out_histogram_feed( ISC_out_histogram *, uint8_t * );
void ISC_out_histogram_export_text( ISC_out_histogram *, FILE * );
//void ISC_out_histogram_export_binary( ISC_out_histogram *, FILE * );
void ISC_out_histogram_end( ISC_out_histogram * );
bool ISC_out_histogram_running( ISC_out_histogram * );
uint8_t *ISC_out_histogram_getmaxes( ISC_out_histogram *ihs, uint8_t subX );

#endif

