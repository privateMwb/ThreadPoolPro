// PulseThreadPool pause/resume cycle test suite.
//
// Coverage:
// - Pausing mid-batch stops any new task from starting; resuming lets
//   the rest of the batch run to completion

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <atomic>
#include <chrono>
#include <thread>

using namespace ThreadPoolPro;

// Verifies pausing mid-batch blocks new tasks from starting, and
// resuming drains the rest of the batch.
TEST(PauseResumeCycleTest, PauseMidBatchThenResumeDrainsRest) {
    ThreadPool pool(1);
    std::atomic<int> completed{0};
    std::atomic<bool> release{false};
    std::atomic<bool> started{false};

    // Occupy the single worker so the rest of the batch queues up.
    pool.detach([&release, &started]() {
        started.store(true);
        while (!release.load())
            std::this_thread::yield();
    });

    while (!started.load())
        std::this_thread::yield();

    pool.pause();
    for (int i = 0; i < 10; ++i)
        pool.detach([&completed]() { completed.fetch_add(1); });

    release.store(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(completed.load(), 0); // still paused after the blocking task released

    pool.resume();
    pool.waitIdle();
    EXPECT_EQ(completed.load(), 10);
}
