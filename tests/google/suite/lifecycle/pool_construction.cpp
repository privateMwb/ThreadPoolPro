// PulseThreadPool ThreadPool construction test suite.
//
// Coverage:
// - A requested thread count of 0 is clamped to 1

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

using namespace ThreadPoolPro;

// Verifies a requested thread count of 0 is clamped to 1.
TEST(PoolConstructionTest, ZeroThreadCountClampedToOne) {
    ThreadPool pool(0);
    EXPECT_EQ(pool.threadCount(), 1);
}
