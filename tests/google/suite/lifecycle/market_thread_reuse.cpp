// PulseThreadPool worker-thread reuse across ThreadPool lifetimes.
//
// Coverage:
// - A worker OS thread returned to the market by one ThreadPool's
//   destructor is reused by the very next ThreadPool that leases one,
//   instead of a fresh OS thread being spawned.

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <thread>

using namespace ThreadPoolPro;

// Verifies a destroyed pool's worker thread is handed straight to the
// next pool that leases one, rather than spawning a new OS thread.
TEST(MarketThreadReuseTest, WorkerThreadIsReusedAcrossPools) {
    std::thread::id firstThreadId;

    {
        ThreadPool pool(1);
        auto future = pool.enqueue([]() { return std::this_thread::get_id(); });
        firstThreadId = future.get();
    }

    std::thread::id secondThreadId;
    {
        ThreadPool pool(1);
        auto future = pool.enqueue([]() { return std::this_thread::get_id(); });
        secondThreadId = future.get();
    }

    EXPECT_EQ(firstThreadId, secondThreadId);
}
