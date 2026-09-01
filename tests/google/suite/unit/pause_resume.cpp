// PulseThreadPool pause()/resume() test suite.
//
// Coverage:
// - isPaused() reflects pause()/resume() calls
// - A task submitted while paused doesn't start until resume()

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <atomic>
#include <chrono>
#include <thread>

using namespace ThreadPoolPro;

// Verifies isPaused() reflects pause()/resume() calls.
TEST(PauseResumeTest, TogglesState) {
    ThreadPool pool(2);
    EXPECT_FALSE(pool.isPaused());
    pool.pause();
    EXPECT_TRUE(pool.isPaused());
    pool.resume();
    EXPECT_FALSE(pool.isPaused());
}

// Verifies a task submitted while paused doesn't start until resume().
TEST(PauseResumeTest, PausedTaskWaitsForResume) {
    ThreadPool pool(2);
    pool.pause();

    std::atomic<bool> ran{false};
    pool.detach([&ran]() { ran.store(true); });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(ran.load());

    pool.resume();
    pool.waitIdle();
    EXPECT_TRUE(ran.load());
}
