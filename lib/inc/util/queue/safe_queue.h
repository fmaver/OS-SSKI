#pragma once

#include <pthread.h>
#include <commons/collections/queue.h>

typedef struct SafeQueue
{

	// ! Private:

	/**
	 * @brief Thread safety ensurer.
	 * @private
	 */
	pthread_mutex_t _mtx;

	/**
	 * @brief queue implementation.
	 * @private
	 */
	t_queue *_queue;

	// ! Public:

} safe_queue_t;

/**
 * @brief Creates a safe queue instance
 *
 * @return a new safe queue instance
 */
safe_queue_t *
new_safe_queue(void);

/**
 * @brief Deallocates memory associated with a safe queue.
 *
 * @param queue to be deleted
 * @param destroyer method used to deleted queue elements
 */
void safe_queue_destroy(safe_queue_t *queue,
						void (*destroyer)(void *));

/**
 * @brief safely add an element at the last of the queue.
 *
 * @param queue the safe queue itself
 *
 * @param element the element in order to add to the safe queue
 *
 * @return void
 */
void safe_queue_push(safe_queue_t *queue, void *element);

/**
 * @brief safely remove the last element of the queue.
 *
 * @param queue the safe queue itself
 *
 * @return void* first element of the queue
 */
void *safe_queue_pop(safe_queue_t *queue);

/**
 * @brief Safe Peeks a queue of elements
 *
 * @param queue to be peeked
 * @return an element reference
 */
void *safe_queue_peek(safe_queue_t *queue);

/**
 * @brief Tells wether a queue is empty or not (thread safe)
 *
 * @param queue the queue itself
 * @return true when no elements are equeued, false when there are elements
 */
bool safe_queue_is_empty(safe_queue_t *queue);

/**
 * @brief Sorts a queue of elements
 *
 * @param queue  the queue itself
 * @param comparator the method used to compare elements: this should return true if the first element goes before the second one
 */
void safe_queue_sort(safe_queue_t *queue, bool (*comparator)(void *, void *));

/**
 * @brief Finds an element in a queue by a given criteria
 *
 * @param queue the queue to be searched
 * @param criteria the method used to find the element must return true
 */
void *safe_queue_find_by(safe_queue_t *queue, bool (*criteria)(void *));

/**
 * @brief Tells wether any elements in the queue match a given criteria
 *
 * @param queue the queue to be searched
 * @param matcher the criteria to  satisfy
 */
bool safe_queue_any(safe_queue_t *queue, bool (*matcher)(void *));

/**
 * @brief Removes an element from a queue
 *
 * @param queue the queue to be searched
 * @param matcher the criteria to  satisfy
 * @return the element removed or null if not found
 */
void *safe_queue_remove_by(safe_queue_t *queue, bool (*matcher)(void *));
