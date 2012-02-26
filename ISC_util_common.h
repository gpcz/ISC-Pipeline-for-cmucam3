/***************************************************************************//**
 * \file ISC_util_common.h
 * \brief Random useful functions.
 *
 * ISC_util_common.h contains some common functions that a bunch of different
 * modules all use.
*******************************************************************************/

#ifndef _ISC_UTIL_COMMON_H_
#define _ISC_UTIL_COMMON_H_

#include <stdint.h>
#include "ISC_util_imagecontext.h"

uint8_t PowerOfTwoDetect( uint8_t d );
uint8_t *MallocRow( ISC_util_imagecontext * );
uint8_t *MallocZeroRow( ISC_util_imagecontext * );

#endif

