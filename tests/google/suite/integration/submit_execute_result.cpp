// PulseThreadPool end-to-end enqueue() result test suite.
//
// Coverage:
// - A task submitted via enqueue() is picked up by a worker, executed,
//   and its result observed through the returned Future
// - Many concurrently enqueued tasks each deliver their own correct,
//   independent result

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <vector>

using namespace ThreadPoolPro;

// Verifies a task submitted via enqueue() is picked up by a worker,
// executed, and its result observed through the returned Future.
TEST(SubmitExecuteResultTest, EnqueueResultFlowsEndToEnd) {
    ThreadPool pool(4);
    auto future = pool.enqueue([](int a, int b) { return a * b; }, 6, 7);
    EXPECT_EQ(future.get(), 42);
}

// Verifies multiple concurrently enqueued tasks each deliver their own
// correct, independent result.
TEST(SubmitExecuteResultTest, MultipleEnqueuedResultsAreIndependent) {
    ThreadPool pool(4);
    std::vector<Detail::Future<int>> futures;

    for (int i = 0; i < 50; ++i)
        futures.push_back(pool.enqueue([](int value) { return value * value; }, i));

    for (int i = 0; i < 50; ++i)
        EXPECT_EQ(futures[static_cast<std::size_t>(i)].get(), i * i);
}
