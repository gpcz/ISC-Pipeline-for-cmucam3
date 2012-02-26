/***************************************************************************//**
 * \file ISC_out_histogram.c
 * \brief Out-Module for Histogram output.
 *
 * ISC_out_histogram.c contains the functions used to perform histograms
 * on a CMUcam3.
*******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cc3.h>

#include "ISC_out_histogram.h"
#include "ISC_util_assert.h"
#include "ISC_util_common.h"
#include "ISC_util_imagecontext.h"

// Allocate the memory for the Histograms in SubAreas.
__attribute__((gnu_inline)) inline static void InitSubAreas( ISC_out_histogram *ihs )
{
	uint8_t areaCounter;

	for ( areaCounter = 0; areaCounter < ihs->Xsubdivisions; areaCounter++ )
	{
		// SizeOf uint32_t * pixels in channel * amount of color bins
		// -MEMORY IS ALLOCATED HERE-
		ihs->subdivisions[areaCounter].histogram = malloc( sizeof(uint32_t) * 3 * ihs->colorBins );
	}
}

// Clear up the SubArea so it can be used for accurate counts.
__attribute__((gnu_inline)) inline static void ClearSubAreas( ISC_out_histogram *ihs )
{
	uint8_t areaCounter, binCounter;

	for ( areaCounter = 0; areaCounter < ihs->Xsubdivisions; areaCounter++ )
	{
		for ( binCounter = 0; binCounter < 3*ihs->colorBins; binCounter++ )
		{
			ihs->subdivisions[areaCounter].histogram[binCounter] = 0;
		}
	}
}

/**
 * \brief Creates an ISC_out_histogram module.
 *
 * This function creates a new ISC_out_histogram module.  The purpose of
 * ISC_out_histogram is to find histograms of images, which is essentially
 * a count of how many times a certain color value is found.  This
 * implementation separates color values into equally-spaced color "bins"
 * of 255/number of bins in size.  So for instance, if there are 2 color bins,
 * you will get the number of pixels that were under 128 in value for each
 * color range, and you will get the number of pixels over or equal to 128.
 * You can also break the image into subdivisions for more or less precision.
 * If you're low on memory or processor speed (like you are on the CMUcam3),
 * you will probably want fewer subdivisions.
 *
 * \param context The intended context of the image to feed into the module.
 * \param xSub The number of X divisions to break the image into when doing the count.
 * \param ySub The number of Y divisions to break the image into when doing the count.
 * \param colorBins The number of bins to have pixel value counts in.  This needs to be a power of two because it uses rightward shifts to do division really fast.
 * \return ISC_out_histogram state structure.
 */
__attribute__((gnu_inline)) inline ISC_out_histogram *ISC_out_histogram_start( ISC_util_imagecontext context, uint8_t xSub, uint8_t ySub, uint8_t colorBins )
{
	// Check a couple things to maintain sanity.
	if ( context.frame.height % ySub != 0 )
		ISC_util_assert_message( "Fatal: Subdivisions are not equal segments!" );

	// Make the new ISCHistogramSystem. 
	// -MEMORY IS ALLOCATED HERE-
	ISC_out_histogram *ihs = malloc( sizeof( ISC_out_histogram ) );

	// Transfer the Image Context.
	ihs->theContext = context;

	// Set initial variables.
	ihs->Xsubdivisions = xSub;
	ihs->Ysubdivisions = ySub;
	ihs->width = ihs->theContext.frame.width;
	ihs->height = ihs->theContext.frame.height;
	ihs->colorBins = colorBins;
	ihs->XsubdivShiftFactor = PowerOfTwoDetect( ihs->Xsubdivisions );

	// First, figure out if colorBins is a power of two.
	ihs->binShiftFactor = PowerOfTwoDetect( ihs->colorBins );

	// If it isn't, then we can't go on.
	if ( ihs->binShiftFactor == 0 )
		ISC_util_assert_message( "Fatal: Number of color bins must be a factor of 256!" );

	// Until I fix the code, the Histogram system should not go on if the
	// current ImageContext has a monochrome colorspace.
	if ( context.colorspace == CC3_COLORSPACE_MONOCHROME )
		ISC_util_assert_message( "Fatal: Histogrammer does not support monochrome yet!" );

	// If it is, the shift factor is the number of bits of each colorspace
	// value (8 in this case) minus binShiftFactor, since we only care
	// about the most significant bits.
	ihs->binShiftFactor = 8 - ihs->binShiftFactor;

	// Make the array of SubAreas. 
	// -MEMORY IS ALLOCATED HERE-
	ihs->subdivisions = malloc( sizeof( ISC_out_histogram_subarea ) * ihs->Xsubdivisions );

	// Initialize the SubAreas.
	InitSubAreas( ihs );

	// Clear the SubAreas.
	ClearSubAreas( ihs );

	ihs->linesLeft = ihs->height/ihs->Ysubdivisions;
	ihs->subsLeft = ihs->Ysubdivisions;

	return ihs;
}

/**
 * \brief Feeds a row of pixels into the histogrammer.
 *
 * This function feeds a row of pixels into the histogram system.
 *
 * \param ihs The ISC_out_histogram state structure.
 * \param row A pointer to a row of pixels.
 */
__attribute__((gnu_inline)) inline void ISC_out_histogram_feed( ISC_out_histogram *ihs, uint8_t *row )
{
	uint16_t counter, subCount, high=0, subSize = ihs->width/ihs->Xsubdivisions;
	uint8_t tempR, tempG, tempB;

	//printf( "feed.\n" );

	if ( row )
	{
		if ( ihs->linesLeft == 0 )
		{
			//printf("subbing.\n");
			ClearSubAreas( ihs );
			ihs->linesLeft = ihs->height / ihs->Ysubdivisions;
			ihs->subsLeft--;
			//printf("subbed.\n");
		}

		for ( subCount = 0; subCount < ihs->Xsubdivisions; subCount++ )
		{
			high += subSize;
			for ( counter = high-subSize; counter < high; counter++ )
			{
				tempR = (*(row+(counter*3))>>ihs->binShiftFactor)*3;
				tempG = (*(row+(counter*3)+1)>>ihs->binShiftFactor)*3+1;
				tempB = (*(row+(counter*3)+2)>>ihs->binShiftFactor)*3+2;
				//printf("go");
				ihs->subdivisions[subCount].histogram[tempR]++;
				ihs->subdivisions[subCount].histogram[tempG]++;
				ihs->subdivisions[subCount].histogram[tempB]++;
			}
		}
		//printf( "interesting.\n");
		ihs->linesLeft--;
		free(row);
	}
}

/**
 * \brief Exports the histogram results as text to a file or serial port.
 *
 * This function prints the results of the histogram to any file pointer
 * (including stdout) as human-readable text.
 *
 * \param ihs The ISC_out_histogram state structure.
 * \param fp The file pointer to write the data to.
 */
__attribute__((gnu_inline)) inline void ISC_out_histogram_export_text( ISC_out_histogram *ihs, FILE *fp )
{
	uint8_t xSubCount, bins;

	fprintf( fp, "Y-SUBDIVISION %u:\n", ihs->Ysubdivisions-ihs->subsLeft );
	for ( xSubCount = 0; xSubCount < ihs->Xsubdivisions; xSubCount++ )
	{
		fprintf( fp, "\tX-SUBDIVISION %u:\n", xSubCount );
		for ( bins = 0; bins < ihs->colorBins; bins++ )
		{
			fprintf( fp, "\t\tRBIN %u: %lu, GBIN %u: %lu, BBIN %u: %lu\n", bins, ihs->subdivisions[xSubCount].histogram[bins*3], bins, ihs->subdivisions[xSubCount].histogram[bins*3+1], bins, ihs->subdivisions[xSubCount].histogram[bins*3+2] );
		}
	}
}

/*void ISC_out_histogram_export_binary( ISC_out_histogram *ihs, FILE *fp )
{
	printf( "Not implemented yet!" );
}*/

/**
 * \brief Cleans up and ends the ISC_out_histogram system.
 *
 * This function cleans up the memory used by the ISC_out_histogram system,
 * and then ends the module.
 *
 * \param ihs The ISC_out_histogram state structure.
 */
__attribute__((gnu_inline)) inline void ISC_out_histogram_end( ISC_out_histogram *ihs )
{
	uint8_t areaCounter;

	for ( areaCounter = 0; areaCounter < ihs->Xsubdivisions; areaCounter++ )
	{
		// SizeOf uint32_t * pixels in channel * amount of color bins
		// -MEMORY IS FREED HERE-
		free(ihs->subdivisions[areaCounter].histogram);
	}
	free( ihs->subdivisions );
	free( ihs );
	ihs = NULL;
}

/**
 * \brief Sends back whether the histogrammer is still running.
 *
 * This function sends back whether or not the histogrammer is still
 * running.
 *
 * \param ihs The ISC_out_histogram state structure.
 * \return Whether or not the module is still running.
 */
__attribute__((gnu_inline)) inline bool ISC_out_histogram_running( ISC_out_histogram *ihs )
{
	if ( ihs->subsLeft == 1 && ihs->linesLeft == 0 )
		return false;
	else
		return true;
}

/**
 * \brief Gets the maximum value from the given color bin in a histogrammed row.
 *
 * This function will determine which color bin contains the most pixels from a
 * X-subdivision you specify.  You should check to see if linesLeft in the 
 * state structure is equal to 0 before calling or you will get incomplete 
 * results.
 *
 * \param ihs The ISC_out_histogram state structure.
 * \param subX The X-subdivision to find the maximum for.
 * \return The color bin in the X-subdivision of your choice that contains the maximum amount of pixels.
 */
__attribute__((gnu_inline)) inline uint8_t *ISC_out_histogram_getmaxes( ISC_out_histogram *ihs, uint8_t subX )
{
	//! \todo Getmax must support more colorspaces.
	uint8_t *maxes;
	uint8_t binCount;
        uint16_t rMax, gMax, bMax;

	maxes = malloc( sizeof(uint8_t)*3 );

	maxes[0] = 0; maxes[1] = 0; maxes[2] = 0;
	rMax = ihs->subdivisions[subX].histogram[0];
	gMax = ihs->subdivisions[subX].histogram[1];
	bMax = ihs->subdivisions[subX].histogram[2];
	// Data order is R0,G0,B0,R1,G1,B1,etc...
	for ( binCount = 1; binCount < ihs->colorBins; binCount++ )
	{
		if ( ihs->subdivisions[subX].histogram[binCount*3] > rMax )
		{
			rMax = ihs->subdivisions[subX].histogram[binCount*3];
			maxes[0] = binCount;
		}
		if ( ihs->subdivisions[subX].histogram[binCount*3+1] > gMax )
		{
			gMax = ihs->subdivisions[subX].histogram[binCount*3+1];
			maxes[1] = binCount;
		}
		if ( ihs->subdivisions[subX].histogram[binCount*3+2] > bMax )
		{
			bMax = ihs->subdivisions[subX].histogram[binCount*3+2];
			maxes[2] = binCount;
		}
	}
	
	return maxes;
}
