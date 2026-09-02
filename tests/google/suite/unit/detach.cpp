// PulseThreadPool detach() test suite.
//
// Coverage:
// - Submitted task actually executes
// - An exception thrown by the task is counted, not propagated
// - Throws std::runtime_error once the pool has begun shutting down

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <atomic>
#include <stdexcept>

using namespace ThreadPoolPro;

// Verifies a detached task actually executes.
TEST(DetachTest, RunsTask) {
    ThreadPool pool(2);
    std::atomic<bool> ran{false};
    pool.detach([&ran]() { ran.store(true); });
    pool.waitIdle();
    EXPECT_TRUE(ran.load());
}

// Verifies an exception thrown by a detached task is counted rather than propagated.
TEST(DetachTest, CountsException) {
    ThreadPool pool(2);
    pool.detach([]() { throw std::runtime_error("boom"); });
    pool.waitIdle();
    EXPECT_EQ(pool.exceptionCount(), 1u);
}

// Verifies detach() rejects new work once shutdown() has been called.
TEST(DetachTest, AfterShutdownThrows) {
    ThreadPool pool(2);
    pool.shutdown();
    EXPECT_THROW(pool.detach([]() {}), std::runtime_error);
}
