// PulseThreadPool observer test suite.
//
// Coverage:
// - threadCount() matches the constructor argument
// - A fresh pool reports empty, with no active or queued tasks
// - activeTaskCount() reflects a task blocked mid-execution
// - isStopped() reflects shutdown()

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <atomic>
#include <thread>

using namespace ThreadPoolPro;

// Verifies threadCount() reflects the constructor argument.
TEST(PoolObserversTest, ThreadCountMatchesConstructor) {
    ThreadPool pool(4);
    EXPECT_EQ(pool.threadCount(), 4);
}

// Verifies a freshly constructed pool reports empty and idle.
TEST(PoolObserversTest, FreshPoolIsEmpty) {
    ThreadPool pool(2);
    EXPECT_TRUE(pool.empty());
    EXPECT_EQ(pool.activeTaskCount(), 0);
    EXPECT_EQ(pool.queuedTasks(), 0);
}

// Verifies activeTaskCount() reflects a task blocked mid-execution.
TEST(PoolObserversTest, ActiveTaskCountDuringExecution) {
    ThreadPool pool(1);
    std::atomic<bool> release{false};
    std::atomic<bool> started{false};

    pool.detach([&]() {
        started.store(true);
        while (!release.load())
            std::this_thread::yield();
    });

    while (!started.load())
        std::this_thread::yield();

    EXPECT_EQ(pool.activeTaskCount(), 1);
    release.store(true);
    pool.waitIdle();
}

// Verifies isStopped() reflects shutdown().
TEST(PoolObserversTest, IsStoppedAfterShutdown) {
    ThreadPool pool(2);
    EXPECT_FALSE(pool.isStopped());
    pool.shutdown();
    EXPECT_TRUE(pool.isStopped());
}
