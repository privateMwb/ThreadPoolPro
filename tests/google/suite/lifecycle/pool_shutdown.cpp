// PulseThreadPool ThreadPool shutdown test suite.
//
// Coverage:
// - The destructor finishes already-queued tasks before returning
// - shutdown(FinishTasks) lets already-queued tasks finish
// - shutdown(DiscardTasks) drops tasks that hadn't started yet
// - Only the first shutdown() call's mode takes effect

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <atomic>

using namespace ThreadPoolPro;

// Verifies the destructor finishes already-queued tasks before returning.
TEST(PoolShutdownTest, DestructorFinishesQueuedTasks) {
    std::atomic<int> completed{0};

    {
        ThreadPool pool(1);
        pool.pause();
        for (int i = 0; i < 5; ++i)
            pool.detach([&completed]() { completed.fetch_add(1); });
        // Pool destructs here. pause() no longer applies once shutdown()
        // starts (runState_ leaves Running), so the default FinishTasks
        // destructor still runs all five before returning.
    }

    EXPECT_EQ(completed.load(), 5);
}

// Verifies shutdown(FinishTasks) lets already-queued tasks finish.
TEST(PoolShutdownTest, ShutdownFinishTasksCompletesQueue) {
    ThreadPool pool(1);
    std::atomic<int> completed{0};

    pool.pause();
    for (int i = 0; i < 5; ++i)
        pool.detach([&completed]() { completed.fetch_add(1); });

    pool.shutdown(ThreadPool::ShutdownMode::FinishTasks);
    EXPECT_EQ(completed.load(), 5);
}

// Verifies shutdown(DiscardTasks) drops tasks that hadn't started yet.
TEST(PoolShutdownTest, ShutdownDiscardTasksDropsQueue) {
    ThreadPool pool(1);
    std::atomic<int> completed{0};

    // Paused, so none of these can have started when shutdown() runs.
    pool.pause();
    for (int i = 0; i < 5; ++i)
        pool.detach([&completed]() { completed.fetch_add(1); });

    pool.shutdown(ThreadPool::ShutdownMode::DiscardTasks);
    EXPECT_EQ(completed.load(), 0);
}

// Verifies only the first shutdown() call's mode takes effect.
TEST(PoolShutdownTest, SecondShutdownCallIsNoop) {
    ThreadPool pool(1);
    std::atomic<int> completed{0};

    pool.pause();
    for (int i = 0; i < 5; ++i)
        pool.detach([&completed]() { completed.fetch_add(1); });

    pool.shutdown(ThreadPool::ShutdownMode::FinishTasks);
    pool.shutdown(ThreadPool::ShutdownMode::DiscardTasks); // no-op: first call already won

    EXPECT_EQ(completed.load(), 5);
}
