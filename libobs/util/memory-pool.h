/*
 * Memory Pool for High-Performance Allocations
 * vantisCorp fork - Phase 3 Performance Improvements
 * 
 * FIXED VERSION: Removed static global from header to prevent 
 * multiple definition errors during linking.
 *
 * Provides pre-allocated memory pools to avoid allocation overhead
 * in hot paths like video/audio processing.
 */

#pragma once

#include "c99defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Pool configuration */
#define MEM_POOL_DEFAULT_SIZE (64 * 1024)  /* 64KB default */
#define MEM_POOL_MAX_POOLS 16

/* Memory pool structure */
struct mem_pool {
    uint8_t *base;          /* Base address of pool memory */
    size_t size;            /* Total pool size */
    size_t used;            /* Bytes currently used */
    size_t peak;            /* Peak usage */
    struct mem_pool *next;  /* For pool chaining */
};

/* Pool manager for multiple pools */
struct mem_pool_manager {
    struct mem_pool *pools[MEM_POOL_MAX_POOLS];
    size_t pool_count;
    size_t default_pool_size;
};

/**
 * Get the global pool manager instance
 * @return Pointer to global pool manager
 */
static inline struct mem_pool_manager *mem_pool_get_manager(void)
{
    static struct mem_pool_manager instance = {0};
    return &instance;
}

/**
 * Initialize the memory pool system
 * @param default_pool_size Default size for new pools (0 for default)
 * @return 0 on success, negative on error
 */
static inline int mem_pool_init(size_t default_pool_size)
{
    struct mem_pool_manager *mgr = mem_pool_get_manager();
    
    if (mgr->pool_count > 0)
        return -1;  /* Already initialized */

    mgr->default_pool_size = default_pool_size > 0 
        ? default_pool_size 
        : MEM_POOL_DEFAULT_SIZE;
    mgr->pool_count = 0;

    return 0;
}

/**
 * Create a new memory pool
 * @param size Size of the pool in bytes (0 for default)
 * @return Pointer to pool or NULL on error
 */
static inline struct mem_pool *mem_pool_create(size_t size)
{
    struct mem_pool_manager *mgr = mem_pool_get_manager();
    
    if (mgr->pool_count >= MEM_POOL_MAX_POOLS)
        return NULL;

    struct mem_pool *pool = (struct mem_pool *)bzalloc(sizeof(struct mem_pool));
    if (!pool)
        return NULL;

    pool->size = size > 0 ? size : mgr->default_pool_size;
    pool->base = (uint8_t *)bzalloc(pool->size);
    
    if (!pool->base) {
        bfree(pool);
        return NULL;
    }

    pool->used = 0;
    pool->peak = 0;
    pool->next = NULL;

    mgr->pools[mgr->pool_count++] = pool;
    
    return pool;
}

/**
 * Allocate memory from a pool (fast path)
 * @param pool Pool to allocate from
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory or NULL if pool exhausted
 */
static inline void *mem_pool_alloc(struct mem_pool *pool, size_t size)
{
    if (!pool || !pool->base)
        return NULL;

    /* Align to 8-byte boundary */
    size = (size + 7) & ~7;

    if (pool->used + size > pool->size)
        return NULL;  /* Pool exhausted */

    void *ptr = pool->base + pool->used;
    pool->used += size;

    if (pool->used > pool->peak)
        pool->peak = pool->used;

    return ptr;
}

/**
 * Reset pool (free all allocations, keep memory)
 * @param pool Pool to reset
 */
static inline void mem_pool_reset(struct mem_pool *pool)
{
    if (!pool)
        return;

    pool->used = 0;
}

/**
 * Free a pool and its memory
 * @param pool Pool to free
 */
static inline void mem_pool_destroy(struct mem_pool *pool)
{
    if (!pool)
        return;

    struct mem_pool_manager *mgr = mem_pool_get_manager();
    
    /* Remove from manager */
    for (size_t i = 0; i < mgr->pool_count; i++) {
        if (mgr->pools[i] == pool) {
            mgr->pools[i] = mgr->pools[--mgr->pool_count];
            break;
        }
    }

    bfree(pool->base);
    bfree(pool);
}

/**
 * Get pool statistics
 * @param pool Pool to query
 * @param total Output: total pool size
 * @param used Output: bytes used
 * @param peak Output: peak usage
 */
static inline void mem_pool_stats(struct mem_pool *pool, 
                                   size_t *total, size_t *used, size_t *peak)
{
    if (!pool)
        return;

    if (total) *total = pool->size;
    if (used)  *used = pool->used;
    if (peak)  *peak = pool->peak;
}

/**
 * Allocate from global pools (convenience function)
 * @param size Bytes to allocate
 * @return Pointer or NULL
 */
static inline void *mem_pool_global_alloc(size_t size)
{
    struct mem_pool_manager *mgr = mem_pool_get_manager();
    
    /* Try each pool in order */
    for (size_t i = 0; i < mgr->pool_count; i++) {
        void *ptr = mem_pool_alloc(mgr->pools[i], size);
        if (ptr)
            return ptr;
    }
    
    /* No pool has space, create new one */
    struct mem_pool *new_pool = mem_pool_create(
        size > mgr->default_pool_size ? size : mgr->default_pool_size
    );
    
    if (!new_pool)
        return NULL;
    
    return mem_pool_alloc(new_pool, size);
}

/**
 * Cleanup all pools
 */
static inline void mem_pool_cleanup(void)
{
    struct mem_pool_manager *mgr = mem_pool_get_manager();
    
    while (mgr->pool_count > 0) {
        mem_pool_destroy(mgr->pools[0]);
    }
}

#ifdef __cplusplus
}
#endif