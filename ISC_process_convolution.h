/***************************************************************************//**
* \file ISC_process_convolution.h
* \brief Convolution module with support for kernels of arbitrary centering.
*******************************************************************************/

#ifndef _ISC_PROCESS_CONVOLUTION_H_
#define _ISC_PROCESS_CONVOLUTION_H_

#include <stdio.h>
#include <stdint.h>

#include "ISC_util_rowqueue.h"
#include "ISC_util_imagecontext.h"

//*******************************KERNEL STUFF***********************************

/**
 * ISC_KERNEL_MAXSIZE is the maximum size, both horizontally and vertically,
 * that an image convolution kernel can be.  Be advised that large kernel sizes
 * can become extremely computationally-intensive, so you may want to keep this
 * at 3 unless they make a new CMUcam that has more processor power.
 */
#define ISC_KERNEL_MAXSIZE 3

/**
 *	\brief Kernel structure for ISC_process_convolution.
 *
 *	ISC_process_convolution_kernel is the data structure holding the kernel 
 *	for an image convolution.
 */
typedef struct
{
	//--------------------------USER-EDITED STUFF------------------------------
    int16_t matrix[ISC_KERNEL_MAXSIZE][ISC_KERNEL_MAXSIZE]; //!< The matrix.
	uint8_t kernelSize; //!< Kernel size.
    uint8_t centerX; //!< Kernel center X.
    uint8_t centerY; //!< Kernel center Y.
    //---------------------------INTERNAL STUFF--------------------------------
    int16_t divisor; //!< Kernel divisor (sum of its coefficients).
} ISC_process_convolution_kernel;

void ISC_process_convolution_setKernelDivisor( ISC_process_convolution_kernel *kern );

//******************************CONVOLUTION STUFF*******************************

/**
 *	\brief State structure for ISC_process_convolution.
 *
 *	This structure stores the current state of an ISC_process_convolution
 *	module, which is used to perform an image convolution on an image.  The
 *	kernel can contain an arbitrary center point and be of an arbitrary size.
 */
typedef struct
{
    //---------------------------USER-EDITED STUFF------------------------------
    ISC_process_convolution_kernel kernel; //!< The kernel to convolve with.
    //---------------------------INTERNAL STUFF---------------------------------
	ISC_util_imagecontext theContext; //!< The image context.
	ISC_util_rowqueue *rqueue; //!< Rowqueue for storing rows.
	uint16_t width; //!< The width of the image.
   	uint16_t height; //!< The height of the image.
    uint16_t remainingConvolveCount; //!< The number of rows left to convolve.
    uint16_t remainingLoadCount; //!< The number of rows left to load.
} ISC_process_convolution;

ISC_process_convolution *ISC_process_convolution_start( ISC_util_imagecontext, ISC_process_convolution_kernel );
void ISC_process_convolution_feed( ISC_process_convolution *conv, uint8_t * );
uint8_t *ISC_process_convolution_process( ISC_process_convolution *conv );
void ISC_process_convolution_end( ISC_process_convolution *conv );

//bool ISC_process_convolution_running( ISC_process_convolution *conv );
ISC_util_imagecontext ISC_process_convolution_context( ISC_process_convolution *conv );

#endif
