// PulseThreadPool concurrent-submission test suite.
//
// Coverage:
// - Many external threads calling detach() on the same pool
//   simultaneously all have their tasks executed exactly once

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <atomic>
#include <thread>
#include <vector>

using namespace ThreadPoolPro;

// Verifies many concurrent external producers submitting into the pool
// all have their tasks executed exactly once.
TEST(ConcurrentSubmitTest, RunsEveryTask) {
    ThreadPool pool(4);
    constexpr int producerCount = 8;
    constexpr int perProducer = 200;
    std::atomic<int> completed{0};

    std::vector<std::thread> producers;
    for (int p = 0; p < producerCount; ++p) {
        producers.emplace_back([&pool, &completed]() {
            for (int i = 0; i < perProducer; ++i)
                pool.detach([&completed]() { completed.fetch_add(1); });
        });
    }

    for (auto& producer : producers)
        producer.join();

    pool.waitIdle();
    EXPECT_EQ(completed.load(), producerCount * perProducer);
}
