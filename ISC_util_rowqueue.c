/***************************************************************************//**
 * \file ISC_util_rowqueue.c
 * \brief Row-queue module.
 *
 * ISC_rowqueue.c contains functions for using RowQueue, a system
 * designed to make image convolution pipelines possible exclusively
 * in RAM (aka, fast).
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "ISC_util_rowqueue.h"
#include "ISC_util_assert.h"

/**
 * \brief Initialize a row queue.
 *
 * ISC_util_rowqueue_start is designed to initialize and allocate a RowQueue,
 * which is a very helpful data structure for temporarily storing rows in a
 * pipeline module.
 * 
 * \param theHardMax The hard maximum amount of rows in the queue, enforced by assert.
 * \return An allocated pointer to an ISC_util_rowqueue structure containing the module state.
 */
__attribute__((gnu_inline)) inline ISC_util_rowqueue *ISC_util_rowqueue_start( uint8_t theHardMax )
{
	// MEMORY IS ALLOCATED HERE.
	ISC_util_rowqueue *iur = (ISC_util_rowqueue*)malloc( sizeof( ISC_util_rowqueue ) );

    	iur->front = NULL;
    	iur->back = NULL;
    	iur->currentSize = 0;
	iur->hardMax = theHardMax;

	return iur;
}

/**
 * \brief Push into queue.
 *
 * ISC_util_rowqueue_feed plants a row into the rowqueue.  If you try to plant
 * a row into a queue larger or equal to the maximum size, the queue will
 * trigger an assert.  If this happens, it is usually indicative of a bigger
 * problem, like a bug in one of the other modules.
 * 
 * \param queue The ISC_util_rowqueue state structure, given by ISC_util_rowqueue_start.
 * \param row A row of pixel data.
 * \return Nothing -- this is a data sink.
 */
__attribute__((gnu_inline)) inline void ISC_util_rowqueue_feed( ISC_util_rowqueue *queue, uint8_t *row )
{
    ISC_util_rowqueue_rowpiece *temp;
    
    if ( queue->currentSize == queue->hardMax )
        ISC_util_assert_message( "FATAL: Hit hardMax on ISC_util_rowqueue." );
    
    // MEMORY IS ALLOCATED HERE.
    temp = (ISC_util_rowqueue_rowpiece*)malloc( sizeof( ISC_util_rowqueue_rowpiece ) );

    if ( !temp )
        ISC_util_assert_message( "FATAL: Ran out of mem making an ISC_util_rowqueue_rowpiece." );
    ;
    temp->row = row;
    temp->next = NULL;
    
    if ( queue->back )
    {
        queue->back->next = temp;
        queue->back = queue->back->next;
    }
    else
    {
        queue->front = temp;
        queue->back = temp;
    }
    
    queue->currentSize++;
}

/**
 * \brief Pop from queue.
 *
 * ISC_util_rowqueue_process pops a row from the queue.  If you try to pop from
 * an empty queue, the queue will trigger an assert.  If this happens, it means
 * that one of the other modules has a bug somewhere.
 * 
 * \param queue The ISC_util_rowqueue state structure given by ISC_util_rowqueue_start.
 * \return The row freshly-popped from the queue.  The module relinquishes ownership of that row's memory space when you do this.
 */
__attribute__((gnu_inline)) inline uint8_t *ISC_util_rowqueue_process( ISC_util_rowqueue *queue )
{
    uint8_t *result = NULL;
    ISC_util_rowqueue_rowpiece *piece;
    
    if ( queue->currentSize == 0 )
        ISC_util_assert_message( "FATAL: Tried to pull from empty rowQueue." );
        
    	if ( queue->front ) // Something is seriously wrong if this isn't the
	                    // case, since we just checked to see if the queue
			    // was empty two lines ago.
    	{
        	result = queue->front->row;
        	piece = queue->front;
        	queue->front = queue->front->next;
		// Deallocates memory made in feed().
        	free( piece );

		queue->currentSize--;
    		if ( queue->currentSize == 0 )
        		queue->back = NULL;
    	}
	return result;
    	
}

/**
 * \brief Create an array of row pointers from a row queue.
 *
 * ISC_util_rowqueue_make_rowarray creates an array of rows from the data in a
 * ISC_util_rowqueue.  This is highly useful for many functions including the
 * ISC_process_convolution module, which loads incoming rows into a rowqueue
 * but still requires random access to arbitrary rows in the queue.
 * 
 * \param rq The ISC_util_rowqueue state structure given by ISC_util_rowqueue_start.
 * \param rowarray An array of uint8_t pointers of length hardMax.  This needs to be initialized before you call this function.
 * \return The function returns its result through the rowarray parameter.
 */
__attribute__((gnu_inline)) inline void ISC_util_rowqueue_make_rowarray( ISC_util_rowqueue *rq, uint8_t **rowarray )
{
	uint8_t count;
	ISC_util_rowqueue_rowpiece *current = rq->front;

	for ( count = 0; count < rq->hardMax; count++ )
		rowarray[count] = NULL;

	count = 0;
	while ( current )
	{
		rowarray[count] = current->row;
		current = current->next;
		count++;
	}
}

/**
 * \brief Deallocate a rowqueue.
 *
 * ISC_util_rowqueue_end reverses what ISC_util_rowqueue_start began.  It will
 * clear and free all rows still in the queue, and then deallocate the state
 * structure of the queue, ending its existence.
 * 
 * \param queue The rowqueue state struct, given by the start function.
 * \return No return value -- the module has finished its duty.
 */
__attribute__((gnu_inline)) inline void ISC_util_rowqueue_end( ISC_util_rowqueue *queue )
{
	while ( queue->currentSize > 0 )
	{
		// Pop all rows from the rowqueue and free the rows as you get 
		// them.
		free(ISC_util_rowqueue_process( queue ));
	}
	// Free the queue itself.
	free( queue );	
}
