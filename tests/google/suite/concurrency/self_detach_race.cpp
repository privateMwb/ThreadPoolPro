// PulseThreadPool self-shutdown test suite.
//
// Coverage:
// - A task calling shutdown() on its own pool from a worker thread
//   doesn't deadlock, and destroying the pool afterward completes cleanly

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <atomic>
#include <thread>

using namespace ThreadPoolPro;

// Verifies a task calling shutdown() on its own pool doesn't deadlock,
// and that destroying the pool afterward completes cleanly.
TEST(SelfDetachRaceTest, SelfShutdownDoesNotDeadlock) {
    ThreadPool pool(2);
    std::atomic<bool> shutdownReturned{false};

    pool.detach([&pool, &shutdownReturned]() {
        pool.shutdown(ThreadPool::ShutdownMode::FinishTasks);
        shutdownReturned.store(true);
    });

    while (!shutdownReturned.load())
        std::this_thread::yield();

    EXPECT_TRUE(pool.isStopped());
    // Pool destructs here. If the self-detach path (selfDetachRequested_)
    // were broken, this would hang trying to join the worker that just
    // shut the pool down from inside its own task.
}
