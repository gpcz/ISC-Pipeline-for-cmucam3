/***************************************************************************//**
 * \file ISC_util_common.c
 * \brief Random useful functions.
 *
 * ISC_util_common.c contains some common functions that a bunch of different
 * modules all use.
*******************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include "ISC_util_common.h"
#include "ISC_util_imagecontext.h"

/**
 * \brief Check to see if a number is a power of two.
 *
 * This function is useful to find out if a number "d" is a power of two. If
 * you're multiplying or dividing numbers frequently by powers of two, you can 
 * save some serious clock cycles by just doing shifts.  This code makes it 
 * really easy to figure out if you can do this.
 */
__attribute__((gnu_inline)) inline uint8_t PowerOfTwoDetect( uint8_t d )
{
    uint8_t answer = 1;
    uint8_t shiftCount = 0;
    
    // Find the power of two that is equal or immediately higher than d.
    while ( answer < d )
    {
        answer = answer << 1;
	shiftCount++;
    }

    // If answer and d are equal, then d is a power of two.  In this case,
    // tell the system what the power is.  If they're not equal, set the
    // number to 0 so the histogrammer uses division instead of shifts.
    if ( answer == d )
    	return shiftCount;
    else
	return 0;
}

/**
 * \brief Allocate proper row memory.
 *
 * This function allocates a row of memory consistent with the context that is
 * brought in.
 */
__attribute__((gnu_inline)) inline uint8_t *MallocRow( ISC_util_imagecontext *context )
{
	uint8_t *result;

	result = malloc( context->frame.width * context->frame.channels );
	return result;
}

/**
 * \brief Allocate proper row memory, fill with zeroes.
 *
 * This function allocates a row of memory consistent with the context that is
 * brought in, and then fills it with zeroes.  Helpful for the module 
 * ISC_process_convolution.
 */
__attribute__((gnu_inline)) inline uint8_t *MallocZeroRow( ISC_util_imagecontext *context )
{
	uint8_t *result = MallocRow( context );
	uint16_t x;

	for ( x = 0; x < context->frame.width * context->frame.channels; x++ )
	{
		*(result+x) = 0;
	}
	return result;
}

