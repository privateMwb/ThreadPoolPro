// PulseThreadPool waitIdle()-helps-drain test suite.
//
// Coverage:
// - A thread blocked in waitIdle() helps drain the pool itself
//   (via fetchTaskExternal()) instead of sitting idle while waiting

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <atomic>
#include <mutex>
#include <set>
#include <thread>

using namespace ThreadPoolPro;

// Verifies the thread calling waitIdle() actually helps execute queued
// tasks rather than just waiting on the single worker to finish them.
TEST(WaitIdleDrainTest, WaitingThreadHelpsDrain) {
    ThreadPool pool(1);
    constexpr int taskCount = 200;
    std::atomic<int> completed{0};
    std::mutex idsMutex;
    std::set<std::thread::id> idsSeen;

    for (int i = 0; i < taskCount; ++i) {
        pool.detach([&completed, &idsMutex, &idsSeen]() {
            {
                std::lock_guard<std::mutex> lock(idsMutex);
                idsSeen.insert(std::this_thread::get_id());
            }
            completed.fetch_add(1);
        });
    }

    pool.waitIdle();

    EXPECT_EQ(completed.load(), taskCount);
    EXPECT_EQ(idsSeen.count(std::this_thread::get_id()), 1u); // the waiting thread itself helped
}
