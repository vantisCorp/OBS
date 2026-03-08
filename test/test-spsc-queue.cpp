/******************************************************************************
    Copyright (C) 2026 vantisCorp

    Unit tests for lock-free SPSC queue.
******************************************************************************/

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include "util/spsc-queue.h"

class SpscQueueTest : public ::testing::Test {
protected:
    spsc_queue_t queue;
    
    void SetUp() override {
        memset(&queue, 0, sizeof(queue));
    }
    
    void TearDown() override {
        spsc_queue_free(&queue);
    }
};

// Test basic initialization
TEST_F(SpscQueueTest, Init) {
    EXPECT_TRUE(spsc_queue_init(&queue, sizeof(int), 16));
    EXPECT_EQ(16u, spsc_queue_capacity(&queue));
    EXPECT_TRUE(spsc_queue_empty(&queue));
    EXPECT_FALSE(spsc_queue_full(&queue));
}

// Test capacity rounding to power of 2
TEST_F(SpscQueueTest, CapacityRounding) {
    EXPECT_TRUE(spsc_queue_init(&queue, sizeof(int), 10));
    EXPECT_EQ(16u, spsc_queue_capacity(&queue));  // Rounded up to power of 2
    
    spsc_queue_free(&queue);
    
    EXPECT_TRUE(spsc_queue_init(&queue, sizeof(int), 100));
    EXPECT_EQ(128u, spsc_queue_capacity(&queue));  // Rounded up to power of 2
}

// Test push and pop
TEST_F(SpscQueueTest, PushPop) {
    EXPECT_TRUE(spsc_queue_init(&queue, sizeof(int), 16));
    
    int value = 42;
    EXPECT_TRUE(spsc_queue_push(&queue, &value));
    EXPECT_EQ(1u, spsc_queue_size(&queue));
    
    int result = 0;
    EXPECT_TRUE(spsc_queue_pop(&queue, &result));
    EXPECT_EQ(42, result);
    EXPECT_TRUE(spsc_queue_empty(&queue));
}

// Test queue full
TEST_F(SpscQueueTest, QueueFull) {
    EXPECT_TRUE(spsc_queue_init(&queue, sizeof(int), 4));
    
    for (int i = 0; i < 4; i++) {
        EXPECT_TRUE(spsc_queue_push(&queue, &i));
    }
    
    EXPECT_TRUE(spsc_queue_full(&queue));
    
    int extra = 999;
    EXPECT_FALSE(spsc_queue_push(&queue, &extra));  // Should fail, queue full
}

// Test queue empty
TEST_F(SpscQueueTest, QueueEmpty) {
    EXPECT_TRUE(spsc_queue_init(&queue, sizeof(int), 16));
    
    int result = 0;
    EXPECT_FALSE(spsc_queue_pop(&queue, &result));  // Should fail, queue empty
}

// Test peek
TEST_F(SpscQueueTest, Peek) {
    EXPECT_TRUE(spsc_queue_init(&queue, sizeof(int), 16));
    
    int value = 123;
    spsc_queue_push(&queue, &value);
    
    int result = 0;
    EXPECT_TRUE(spsc_queue_peek(&queue, &result));
    EXPECT_EQ(123, result);
    
    // Peek should not remove the element
    EXPECT_EQ(1u, spsc_queue_size(&queue));
    
    spsc_queue_pop(&queue, &result);
    EXPECT_EQ(123, result);
}

// Test wrap-around
TEST_F(SpscQueueTest, WrapAround) {
    EXPECT_TRUE(spsc_queue_init(&queue, sizeof(int), 4));
    
    // Fill and drain multiple times to test wrap-around
    for (int round = 0; round < 10; round++) {
        // Fill
        for (int i = 0; i < 4; i++) {
            int value = round * 10 + i;
            EXPECT_TRUE(spsc_queue_push(&queue, &value));
        }
        
        // Drain
        for (int i = 0; i < 4; i++) {
            int result = 0;
            EXPECT_TRUE(spsc_queue_pop(&queue, &result));
            EXPECT_EQ(round * 10 + i, result);
        }
    }
}

// Test multi-threaded producer-consumer
TEST_F(SpscQueueTest, MultiThreaded) {
    const size_t num_items = 10000;
    EXPECT_TRUE(spsc_queue_init(&queue, sizeof(int), 1024));
    
    std::atomic<int> produced_count{0};
    std::atomic<int> consumed_count{0};
    std::atomic<bool> done{false};
    
    // Producer thread
    std::thread producer([&]() {
        for (int i = 0; i < (int)num_items; i++) {
            while (!spsc_queue_push(&queue, &i)) {
                // Spin until successful
                std::this_thread::yield();
            }
            produced_count++;
        }
        done = true;
    });
    
    // Consumer thread
    std::thread consumer([&]() {
        int expected = 0;
        while (expected < (int)num_items) {
            int value = 0;
            if (spsc_queue_pop(&queue, &value)) {
                EXPECT_EQ(expected, value);
                expected++;
                consumed_count++;
            } else if (done && spsc_queue_empty(&queue)) {
                break;
            } else {
                std::this_thread::yield();
            }
        }
    });
    
    producer.join();
    consumer.join();
    
    EXPECT_EQ((int)num_items, produced_count.load());
    EXPECT_EQ((int)num_items, consumed_count.load());
}

// Test with larger elements
TEST_F(SpscQueueTest, LargeElements) {
    struct LargeElement {
        int values[64];
    };
    
    spsc_queue_free(&queue);
    EXPECT_TRUE(spsc_queue_init(&queue, sizeof(LargeElement), 16));
    
    LargeElement elem;
    for (int i = 0; i < 64; i++) {
        elem.values[i] = i * 100;
    }
    
    EXPECT_TRUE(spsc_queue_push(&queue, &elem));
    
    LargeElement result;
    EXPECT_TRUE(spsc_queue_pop(&queue, &result));
    
    for (int i = 0; i < 64; i++) {
        EXPECT_EQ(i * 100, result.values[i]);
    }
}

// Test NULL handling
TEST_F(SpscQueueTest, NullHandling) {
    EXPECT_FALSE(spsc_queue_init(NULL, sizeof(int), 16));
    EXPECT_FALSE(spsc_queue_init(&queue, 0, 16));
    EXPECT_FALSE(spsc_queue_init(&queue, sizeof(int), 0));
    
    EXPECT_TRUE(spsc_queue_init(&queue, sizeof(int), 16));
    
    int value = 42;
    EXPECT_FALSE(spsc_queue_push(NULL, &value));
    EXPECT_FALSE(spsc_queue_push(&queue, NULL));
    EXPECT_FALSE(spsc_queue_pop(NULL, &value));
    EXPECT_FALSE(spsc_queue_pop(&queue, NULL));
}

// Test clear
TEST_F(SpscQueueTest, Clear) {
    EXPECT_TRUE(spsc_queue_init(&queue, sizeof(int), 16));
    
    for (int i = 0; i < 10; i++) {
        spsc_queue_push(&queue, &i);
    }
    
    EXPECT_EQ(10u, spsc_queue_size(&queue));
    
    spsc_queue_clear(&queue);
    
    EXPECT_TRUE(spsc_queue_empty(&queue));
    EXPECT_EQ(0u, spsc_queue_size(&queue));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}