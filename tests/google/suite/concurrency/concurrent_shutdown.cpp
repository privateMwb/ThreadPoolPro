// PulseThreadPool concurrent-shutdown test suite.
//
// Coverage:
// - Two threads calling shutdown() concurrently with different modes
//   resolve to exactly one mode, cleanly — never a corrupted in-between
//   state where some but not all queued tasks ran

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

using namespace ThreadPoolPro;

// Verifies two threads calling shutdown() concurrently with different
// modes don't corrupt pool state — exactly one mode wins, and it wins
// cleanly (all queued tasks finish, or none of them start).
TEST(ConcurrentShutdownTest, ResolvesToOneMode) {
    ThreadPool pool(1);
    std::atomic<int> completed{0};

    pool.pause();
    for (int i = 0; i < 20; ++i)
        pool.detach([&completed]() { completed.fetch_add(1); });

    std::thread finisher([&pool]() { pool.shutdown(ThreadPool::ShutdownMode::FinishTasks); });
    std::thread discarder([&pool]() { pool.shutdown(ThreadPool::ShutdownMode::DiscardTasks); });

    finisher.join();
    discarder.join();

    EXPECT_TRUE(pool.isStopped());

    int result = completed.load();
    EXPECT_TRUE(result == 0 || result == 20);
}
