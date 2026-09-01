// PulseThreadPool enqueue() test suite.
//
// Coverage:
// - Returns the task's return value via Future::get()
// - Propagates an exception thrown by the task
// - Throws std::runtime_error once the pool has begun shutting down
// - Future is no longer valid() after get() consumes it

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <stdexcept>

using namespace ThreadPoolPro;

// Verifies a submitted task's return value is observable through get().
TEST(EnqueueTest, ReturnsResult) {
    ThreadPool pool(2);
    auto future = pool.enqueue([](int a, int b) { return a + b; }, 2, 3);
    EXPECT_EQ(future.get(), 5);
}

// Verifies an exception thrown by the task is rethrown from get().
TEST(EnqueueTest, PropagatesException) {
    ThreadPool pool(2);
    auto future = pool.enqueue([]() -> int { throw std::runtime_error("boom"); });
    EXPECT_THROW(future.get(), std::runtime_error);
}

// Verifies enqueue() rejects new work once shutdown() has been called.
TEST(EnqueueTest, AfterShutdownThrows) {
    ThreadPool pool(2);
    pool.shutdown();
    EXPECT_THROW((void)pool.enqueue([]() { return 1; }), std::runtime_error);
}

// Verifies the Future becomes invalid once get() has consumed it.
TEST(EnqueueTest, FutureInvalidAfterGet) {
    ThreadPool pool(2);
    auto future = pool.enqueue([]() { return 42; });
    EXPECT_TRUE(future.valid());
    (void)future.get();
    EXPECT_FALSE(future.valid());
}
