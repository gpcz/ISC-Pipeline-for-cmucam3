/***************************************************************************//**
 * \file ISC_util_rowqueue.h
 * \brief Row-queue module header.
 *
 * ISC_util_rowqueue.h describes a neat, fast, and safe dynamic data structure
 * for storing rows of image data that can be reused for any module.
*******************************************************************************/

#ifndef _ISC_UTIL_ROWQUEUE_H_
#define _ISC_UTIL_ROWQUEUE_H_

#include <stdint.h>

/**
 * \brief Storage container for one row.
 *
 * rowpiece is the internal data structure for defining a
 * linked list of row pointers.  Users of ISC_util_rowqueue will most likely
 * never encounter or use this structure.
 */
struct rowpiece
{
    uint8_t *row; /*!< The image row to be contained. */
    struct rowpiece *next; /*!< The next ISC_util_rowqueue_rowpiece in the linked list. */
};

/**
 * \brief Storage container for one row, renamed.
 *
 * ISC_util_rowqueue_rowpiece is the internal data structure for defining a
 * linked list of row pointers, but renamed.  Users of ISC_util_rowqueue will
 * most likely never encounter or use this structure.
 */
typedef struct rowpiece ISC_util_rowqueue_rowpiece;

/**
 * \brief A general-purpose row queue for modules.
 *
 * ISC_util_rowqueue is a module designed to be used by other modules for
 * support functions.  It can reliably store rows in a convenient queue
 * ideal for pipeline-style loading and unloading, and it can create a quick
 * array of row pointers that can be used in the process modules.
 */
typedef struct
{
    //----------------------------USER-DEFINED----------------------------------
    uint8_t hardMax; /*!< Hard maximum amount of rows in the queue. */
    
    //---------------------------HANDLED BY SYSTEM------------------------------
    uint8_t currentSize; /*!< Current size of the queue. */
    ISC_util_rowqueue_rowpiece *front; /*!< Front of the queue. */
    ISC_util_rowqueue_rowpiece *back;  /*!< Back of the queue. */

} ISC_util_rowqueue;

//------------------------------Prototypes--------------------------------------

ISC_util_rowqueue *ISC_util_rowqueue_start( uint8_t );
void ISC_util_rowqueue_feed( ISC_util_rowqueue *, uint8_t * );
uint8_t *ISC_util_rowqueue_process( ISC_util_rowqueue * );
void ISC_util_rowqueue_make_rowarray( ISC_util_rowqueue *, uint8_t ** );
void ISC_util_rowqueue_end( ISC_util_rowqueue * );

#endif
