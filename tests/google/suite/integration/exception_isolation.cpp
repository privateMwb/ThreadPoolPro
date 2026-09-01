// PulseThreadPool exception isolation test suite.
//
// Coverage:
// - A throwing detach()ed task doesn't stop other queued tasks from
//   running, and is counted via exceptionCount()
// - A throwing enqueue()d task's exception is confined to its own
//   Future; the pool remains fully usable afterward

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <atomic>
#include <stdexcept>

using namespace ThreadPoolPro;

// Verifies a throwing task doesn't stop other tasks from running, and
// is counted via exceptionCount().
TEST(ExceptionIsolationTest, ThrowingTaskDoesNotAffectOthers) {
    ThreadPool pool(4);
    std::atomic<int> completed{0};
    constexpr int taskCount = 20;

    for (int i = 0; i < taskCount; ++i) {
        pool.detach([&completed, i]() {
            if (i % 3 == 0)
                throw std::runtime_error("boom");
            completed.fetch_add(1);
        });
    }

    pool.waitIdle();

    int expectedThrows = 0;
    for (int i = 0; i < taskCount; ++i)
        if (i % 3 == 0)
            ++expectedThrows;

    EXPECT_EQ(pool.exceptionCount(), static_cast<std::size_t>(expectedThrows));
    EXPECT_EQ(completed.load(), taskCount - expectedThrows);
}

// Verifies an enqueue()d task's exception stays confined to its own
// Future, and the pool remains fully usable afterward.
TEST(ExceptionIsolationTest, EnqueueExceptionDoesNotAffectPool) {
    ThreadPool pool(2);

    auto future = pool.enqueue([]() -> int { throw std::runtime_error("boom"); });
    EXPECT_THROW(future.get(), std::runtime_error);

    auto ok = pool.enqueue([]() { return 99; });
    EXPECT_EQ(ok.get(), 99);
}
