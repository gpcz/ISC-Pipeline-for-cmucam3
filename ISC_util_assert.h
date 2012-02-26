/***************************************************************************//**
 * \file ISC_util_assert.h
 * \brief Assert functions.
 *
 * ISC_util_assert.h describes several assert functions that can simultaneously
 * be used on the virtual-cam simulator and the CMUcam3 itself.  If there is an
 * uncorrectable error, modules will throw an assert, which will stop the camera
 * dead in its tracks.
 *
 * If no preprocessor variables are set, this file will assume CMUcam3 mode and
 * use the version of ISC_util_assert that goes in a while(1) loop.
*******************************************************************************/

#ifndef _ISC_UTIL_ASSERT_H_
#define _ISC_UTIL_ASSERT_H_

void ISC_util_assert( void );
void ISC_util_assert_message( const char *mess );

#endif
