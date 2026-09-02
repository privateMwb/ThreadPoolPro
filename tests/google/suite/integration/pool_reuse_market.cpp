// PulseThreadPool repeated pool-lifecycle test suite.
//
// Coverage:
// - Many successive construct/run/destroy cycles, each reusing worker
//   threads returned to ThreadMarket by the previous cycle, execute
//   correctly every time

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <atomic>

using namespace ThreadPoolPro;

// Verifies many successive pool construct/run/destroy cycles each
// execute their full batch of tasks correctly.
TEST(PoolReuseMarketTest, RepeatedCyclesExecuteCorrectly) {
    for (int cycle = 0; cycle < 50; ++cycle) {
        ThreadPool pool(4);
        std::atomic<int> completed{0};

        for (int i = 0; i < 20; ++i)
            pool.detach([&completed]() { completed.fetch_add(1); });

        pool.waitIdle();
        EXPECT_EQ(completed.load(), 20);
    }
}
