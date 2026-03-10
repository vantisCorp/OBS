/*
 * Unit Tests for Memory Pool
 * vantisCorp fork - Phase 3 Performance Improvements
 *
 * Tests the memory pool implementation for correctness.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Mock OBS functions for standalone testing */
#ifndef BZALLOC_DEFINED
#define BZALLOC_DEFINED
static void *bzalloc(size_t size)
{
	void *ptr = calloc(1, size);
	return ptr;
}

static void bfree(void *ptr)
{
	free(ptr);
}
#endif

/* Include the memory pool header */
#define C99DEFS_H
typedef unsigned char uint8_t;
typedef unsigned long size_t;

/* Inline the memory pool implementation for testing */
#include "memory-pool.h"

/* Test counters */
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) static void test_##name(void)
#define RUN_TEST(name) do { \
    printf("Running: %s... ", #name); \
    test_##name(); \
    printf("PASSED\n"); \
    tests_passed++; \
} while(0)

#define ASSERT(cond) do { \
    if (!(cond)) { \
        printf("FAILED at line %d: %s\n", __LINE__, #cond); \
        tests_failed++; \
        return; \
    } \
} while(0)

/* Test: Pool initialization */
TEST(pool_init)
{
	int result = mem_pool_init(0);
	ASSERT(result == 0);

	/* Cleanup for next test */
	mem_pool_cleanup();

	/* Re-init with custom size */
	result = mem_pool_init(2048);
	ASSERT(result == 0);

	mem_pool_cleanup();
}

/* Test: Pool creation */
TEST(pool_create)
{
	mem_pool_init(1024);

	struct mem_pool *pool = mem_pool_create(2048);
	ASSERT(pool != NULL);
	ASSERT(pool->base != NULL);
	ASSERT(pool->size == 2048);
	ASSERT(pool->used == 0);

	mem_pool_destroy(pool);
	mem_pool_cleanup();
}

/* Test: Pool allocation */
TEST(pool_alloc)
{
	mem_pool_init(0);

	struct mem_pool *pool = mem_pool_create(1024);
	ASSERT(pool != NULL);

	/* Allocate some memory */
	void *ptr1 = mem_pool_alloc(pool, 100);
	ASSERT(ptr1 != NULL);
	ASSERT(pool->used >= 100);

	/* Allocate more */
	void *ptr2 = mem_pool_alloc(pool, 200);
	ASSERT(ptr2 != NULL);
	ASSERT(ptr2 > ptr1); /* Should be after ptr1 */

	/* Check alignment (8-byte) */
	ASSERT(((size_t)ptr1 & 7) == 0);
	ASSERT(((size_t)ptr2 & 7) == 0);

	mem_pool_destroy(pool);
	mem_pool_cleanup();
}

/* Test: Pool exhaustion */
TEST(pool_exhaustion)
{
	mem_pool_init(0);

	struct mem_pool *pool = mem_pool_create(100);
	ASSERT(pool != NULL);

	/* Try to allocate more than pool size */
	void *ptr = mem_pool_alloc(pool, 200);
	ASSERT(ptr == NULL); /* Should fail */

	/* Allocate exactly pool size (with alignment padding) */
	ptr = mem_pool_alloc(pool, 96); /* 96 + alignment should fit in 100 */
	ASSERT(ptr != NULL);

	/* Pool should now be exhausted */
	void *ptr2 = mem_pool_alloc(pool, 1);
	ASSERT(ptr2 == NULL);

	mem_pool_destroy(pool);
	mem_pool_cleanup();
}

/* Test: Pool reset */
TEST(pool_reset)
{
	mem_pool_init(0);

	struct mem_pool *pool = mem_pool_create(1024);
	ASSERT(pool != NULL);

	/* Allocate some memory */
	void *ptr = mem_pool_alloc(pool, 100);
	ASSERT(ptr != NULL);
	ASSERT(pool->used > 0);

	size_t peak_before = pool->peak;

	/* Reset pool */
	mem_pool_reset(pool);
	ASSERT(pool->used == 0);
	ASSERT(pool->peak == peak_before); /* Peak should be preserved */

	/* Should be able to allocate again */
	ptr = mem_pool_alloc(pool, 100);
	ASSERT(ptr != NULL);

	mem_pool_destroy(pool);
	mem_pool_cleanup();
}

/* Test: Pool statistics */
TEST(pool_stats)
{
	mem_pool_init(0);

	struct mem_pool *pool = mem_pool_create(1024);
	ASSERT(pool != NULL);

	size_t total, used, peak;

	mem_pool_stats(pool, &total, &used, &peak);
	ASSERT(total == 1024);
	ASSERT(used == 0);
	ASSERT(peak == 0);

	mem_pool_alloc(pool, 100);
	mem_pool_stats(pool, &total, &used, &peak);
	ASSERT(used >= 100);
	ASSERT(peak >= 100);

	mem_pool_reset(pool);
	mem_pool_stats(pool, &total, &used, &peak);
	ASSERT(used == 0);
	ASSERT(peak >= 100); /* Peak preserved */

	mem_pool_destroy(pool);
	mem_pool_cleanup();
}

/* Test: Multiple pools */
TEST(multiple_pools)
{
	mem_pool_init(0);

	struct mem_pool *pool1 = mem_pool_create(1024);
	struct mem_pool *pool2 = mem_pool_create(2048);
	struct mem_pool *pool3 = mem_pool_create(4096);

	ASSERT(pool1 != NULL);
	ASSERT(pool2 != NULL);
	ASSERT(pool3 != NULL);

	/* Allocate from each */
	void *p1 = mem_pool_alloc(pool1, 100);
	void *p2 = mem_pool_alloc(pool2, 200);
	void *p3 = mem_pool_alloc(pool3, 300);

	ASSERT(p1 != NULL);
	ASSERT(p2 != NULL);
	ASSERT(p3 != NULL);

	/* Each should be in different memory regions */
	ASSERT(p1 != p2);
	ASSERT(p2 != p3);
	ASSERT(p1 != p3);

	mem_pool_cleanup();
}

/* Test: Global allocation */
TEST(global_alloc)
{
	mem_pool_init(0);

	/* Create a pool first */
	struct mem_pool *pool = mem_pool_create(1024);
	ASSERT(pool != NULL);

	/* Allocate globally */
	void *ptr = mem_pool_global_alloc(100);
	ASSERT(ptr != NULL);

	/* Another allocation */
	void *ptr2 = mem_pool_global_alloc(200);
	ASSERT(ptr2 != NULL);

	mem_pool_cleanup();
}

/* Test: Cleanup */
TEST(cleanup)
{
	mem_pool_init(0);

	/* Create multiple pools */
	mem_pool_create(1024);
	mem_pool_create(2048);
	mem_pool_create(4096);

	struct mem_pool_manager *mgr = mem_pool_get_manager();
	ASSERT(mgr->pool_count == 3);

	/* Cleanup all */
	mem_pool_cleanup();
	ASSERT(mgr->pool_count == 0);
}

int main(void)
{
	printf("\n=== Memory Pool Unit Tests ===\n\n");

	RUN_TEST(pool_init);
	RUN_TEST(pool_create);
	RUN_TEST(pool_alloc);
	RUN_TEST(pool_exhaustion);
	RUN_TEST(pool_reset);
	RUN_TEST(pool_stats);
	RUN_TEST(multiple_pools);
	RUN_TEST(global_alloc);
	RUN_TEST(cleanup);

	printf("\n=== Results ===\n");
	printf("Passed: %d\n", tests_passed);
	printf("Failed: %d\n", tests_failed);

	return tests_failed > 0 ? 1 : 0;
}
