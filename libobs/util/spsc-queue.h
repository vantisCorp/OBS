/******************************************************************************
    Copyright (C) 2026 vantisCorp

    Lock-free Single Producer Single Consumer (SPSC) Queue.
    
    This implementation uses atomic operations for thread-safe
    communication between a single producer and single consumer
    without locks, reducing latency in audio pipelines.
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdbool.h>
#include "bmem.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Lock-free SPSC (Single Producer Single Consumer) Queue
 * 
 * This queue is designed for scenarios where one thread produces
 * data and another thread consumes it, without needing locks.
 * 
 * Memory ordering uses acquire/release semantics for correct
 * synchronization across architectures.
 */

struct spsc_queue {
    uint8_t *buffer;           /* Ring buffer storage */
    size_t capacity;           /* Buffer capacity (must be power of 2) */
    size_t element_size;       /* Size of each element */
    
    /* Atomic head/tail for lock-free operation */
    atomic_size_t head;        /* Write position (producer) */
    atomic_size_t tail;        /* Read position (consumer) */
};

typedef struct spsc_queue spsc_queue_t;

/**
 * Initialize an SPSC queue.
 * 
 * @param queue Queue to initialize
 * @param element_size Size of each element in bytes
 * @param capacity Number of elements (will be rounded up to power of 2)
 * @return true on success, false on allocation failure
 */
static inline bool spsc_queue_init(spsc_queue_t *queue, size_t element_size, size_t capacity)
{
    if (!queue || element_size == 0 || capacity == 0)
        return false;
    
    /* Round capacity up to power of 2 for efficient modulo */
    size_t cap = 1;
    while (cap < capacity)
        cap *= 2;
    
    queue->buffer = (uint8_t *)bzalloc(cap * element_size);
    if (!queue->buffer)
        return false;
    
    queue->capacity = cap;
    queue->element_size = element_size;
    atomic_init(&queue->head, 0);
    atomic_init(&queue->tail, 0);
    
    return true;
}

/**
 * Free an SPSC queue.
 * 
 * @param queue Queue to free
 */
static inline void spsc_queue_free(spsc_queue_t *queue)
{
    if (!queue)
        return;
    
    bfree(queue->buffer);
    queue->buffer = NULL;
    queue->capacity = 0;
    queue->element_size = 0;
    atomic_store(&queue->head, 0);
    atomic_store(&queue->tail, 0);
}

/**
 * Get the mask for circular buffer indexing.
 */
static inline size_t spsc_queue_mask(const spsc_queue_t *queue, size_t index)
{
    return index & (queue->capacity - 1);
}

/**
 * Push an element to the queue (producer thread only).
 * 
 * @param queue Queue to push to
 * @param element Element to push
 * @return true on success, false if queue is full
 */
static inline bool spsc_queue_push(spsc_queue_t *queue, const void *element)
{
    if (!queue || !element)
        return false;
    
    size_t head = atomic_load_explicit(&queue->head, memory_order_relaxed);
    size_t tail = atomic_load_explicit(&queue->tail, memory_order_acquire);
    
    /* Check if queue is full */
    if ((head - tail) >= queue->capacity)
        return false;
    
    /* Copy element to buffer */
    size_t index = spsc_queue_mask(queue, head);
    memcpy(queue->buffer + index * queue->element_size, element, queue->element_size);
    
    /* Publish the new head */
    atomic_store_explicit(&queue->head, head + 1, memory_order_release);
    
    return true;
}

/**
 * Pop an element from the queue (consumer thread only).
 * 
 * @param queue Queue to pop from
 * @param element Buffer to store popped element
 * @return true on success, false if queue is empty
 */
static inline bool spsc_queue_pop(spsc_queue_t *queue, void *element)
{
    if (!queue || !element)
        return false;
    
    size_t tail = atomic_load_explicit(&queue->tail, memory_order_relaxed);
    size_t head = atomic_load_explicit(&queue->head, memory_order_acquire);
    
    /* Check if queue is empty */
    if (head == tail)
        return false;
    
    /* Copy element from buffer */
    size_t index = spsc_queue_mask(queue, tail);
    memcpy(element, queue->buffer + index * queue->element_size, queue->element_size);
    
    /* Publish the new tail */
    atomic_store_explicit(&queue->tail, tail + 1, memory_order_release);
    
    return true;
}

/**
 * Peek at the front element without removing it (consumer thread only).
 * 
 * @param queue Queue to peek
 * @param element Buffer to store peeked element
 * @return true on success, false if queue is empty
 */
static inline bool spsc_queue_peek(const spsc_queue_t *queue, void *element)
{
    if (!queue || !element)
        return false;
    
    size_t tail = atomic_load_explicit(&queue->tail, memory_order_relaxed);
    size_t head = atomic_load_explicit(&queue->head, memory_order_acquire);
    
    if (head == tail)
        return false;
    
    size_t index = spsc_queue_mask(queue, tail);
    memcpy(element, queue->buffer + index * queue->element_size, queue->element_size);
    
    return true;
}

/**
 * Get the number of elements in the queue.
 * 
 * @param queue Queue to check
 * @return Number of elements
 */
static inline size_t spsc_queue_size(const spsc_queue_t *queue)
{
    if (!queue)
        return 0;
    
    size_t head = atomic_load_explicit(&queue->head, memory_order_acquire);
    size_t tail = atomic_load_explicit(&queue->tail, memory_order_acquire);
    
    return head - tail;
}

/**
 * Check if the queue is empty.
 * 
 * @param queue Queue to check
 * @return true if empty, false otherwise
 */
static inline bool spsc_queue_empty(const spsc_queue_t *queue)
{
    return spsc_queue_size(queue) == 0;
}

/**
 * Check if the queue is full.
 * 
 * @param queue Queue to check
 * @return true if full, false otherwise
 */
static inline bool spsc_queue_full(const spsc_queue_t *queue)
{
    if (!queue)
        return true;
    
    size_t head = atomic_load_explicit(&queue->head, memory_order_acquire);
    size_t tail = atomic_load_explicit(&queue->tail, memory_order_acquire);
    
    return (head - tail) >= queue->capacity;
}

/**
 * Get the capacity of the queue.
 * 
 * @param queue Queue to check
 * @return Capacity in number of elements
 */
static inline size_t spsc_queue_capacity(const spsc_queue_t *queue)
{
    return queue ? queue->capacity : 0;
}

/**
 * Clear all elements from the queue (not thread-safe).
 * 
 * @param queue Queue to clear
 */
static inline void spsc_queue_clear(spsc_queue_t *queue)
{
    if (!queue)
        return;
    
    atomic_store(&queue->head, 0);
    atomic_store(&queue->tail, 0);
}

#ifdef __cplusplus
}
#endif