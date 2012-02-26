/***************************************************************************//**
 * \file ISC_util_assert.c
 * \brief Assert functions.
 *
 * ISC_util_assert.c describes several assert functions that can simultaneously
 * be used on the virtual-cam simulator and the CMUcam3 itself.  If there is an
 * uncorrectable error, modules will throw an assert, which will stop the camera
 * dead in its tracks.
 *
 * If no preprocessor variables are set, this file will assume CMUcam3 mode and
 * use the version of ISC_util_assert that goes in a while(1) loop.
*******************************************************************************/

#include <stdio.h>
#include "ISC_util_assert.h"

#ifdef SDL_TEST_ENVIRONMENT
//******************************LIBSDL TEST MODE********************************

#include <assert.h>

/**
 * \brief Assert.
 *
 * ISC_util_assert will go into an infinite loop and fail.  This function, in
 * an ideal world, will never be called in a production environment.
 */
void ISC_util_assert( void )
{
    assert(0); // Stick a fork in it, the program's cooked.   
}

/**
 * \brief Assert with message.
 *
 * ISC_util_assert_message will take down the CMUcam3, but print a message to
 * the serial port before doing so.  This gives developers an indication of
 * what went wrong while debugging.
 *
 * \param mess The message to display.
 */
void ISC_util_assert_message( const char *mess )
{
    printf( mess );
    assert(0); // Stick a fork in it, the program's cooked.   
}

#else
//********************************CMUCAM3 MODE**********************************

/**
 * \brief Assert.
 *
 * ISC_util_assert will go into an infinite loop and fail.  This function, in
 * an ideal world, will never be called in a production environment.
 */
void ISC_util_assert()
{
    while(1); // Stick a fork in it, the program's cooked.   
}

/**
 * \brief Assert with message.
 *
 * ISC_util_assert_message will take down the CMUcam3, but print a message to
 * the serial port before doing so.  This gives developers an indication of
 * what went wrong while debugging.
 *
 * \param mess The message to display.
 */
void ISC_util_assert_message( const char *mess )
{
    printf( mess );
    while(1); // Stick a fork in it, the program's cooked.   
}

#endif
