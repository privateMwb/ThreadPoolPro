// PulseThreadPool Future/ResultState test suite.
//
// Coverage:
// - get() returns a value published via setValue()
// - get() rethrows an exception published via setException()
// - get() on an empty Future throws
// - the void specialization publishes completion without a value
// - get() actually blocks (spin -> yield -> park) until another thread
//   publishes, for both the value-carrying and void specializations
// - setException() on a non-default, non-void T (std::thread::id, the
//   type ThreadPool::enqueue() actually returns it as elsewhere in the
//   suite) is exercised at least once

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <chrono>
#include <exception>
#include <stdexcept>
#include <thread>

using namespace ThreadPoolPro::Detail;

// Verifies get() returns a value published via setValue().
TEST(FutureResultTest, GetReturnsPublishedValue) {
    auto* state = new ResultState<int>();
    state->setValue(42);
    state->release(); // simulates the task closure's share being released
    Future<int> future(state);
    EXPECT_EQ(future.get(), 42);
}

// Verifies get() rethrows an exception published via setException().
TEST(FutureResultTest, GetRethrowsPublishedException) {
    auto* state = new ResultState<int>();
    state->setException(std::make_exception_ptr(std::runtime_error("boom")));
    state->release();
    Future<int> future(state);
    EXPECT_THROW(future.get(), std::runtime_error);
}

// Verifies get() on a default-constructed (empty) Future throws.
TEST(FutureResultTest, GetOnEmptyFutureThrows) {
    Future<int> future;
    EXPECT_THROW(future.get(), std::logic_error);
}

// Verifies the void specialization publishes completion without a value.
TEST(FutureResultTest, VoidSpecializationCompletes) {
    auto* state = new ResultState<void>();
    state->setValue();
    state->release();
    Future<void> future(state);
    future.get(); // must not throw
    EXPECT_FALSE(future.valid());
}

// Verifies get() actually blocks until another thread publishes the
// value, rather than only being tested with the result already
// available. A short producer-side delay ensures wait() runs long
// enough to fall through its spin tier into the yield and park tiers,
// rather than always resolving on the very first ready_ check.
TEST(FutureResultTest, GetBlocksUntilPublishedFromAnotherThread) {
    auto* state = new ResultState<int>();

    std::thread producer([state]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        state->setValue(99);
        state->release();
    });

    Future<int> future(state);
    EXPECT_EQ(future.get(), 99);

    producer.join();
}

// Same as above, for the void specialization's wait() path.
TEST(FutureResultTest, VoidGetBlocksUntilPublishedFromAnotherThread) {
    auto* state = new ResultState<void>();

    std::thread producer([state]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        state->setValue();
        state->release();
    });

    Future<void> future(state);
    future.get(); // must not throw, must actually wait for the producer

    producer.join();
}

// Verifies setException()/get() works for a non-default, non-void T
// too (std::thread::id, the type actually returned by the pool-reuse
// enqueue() tests elsewhere in the suite) — those tests only ever
// exercise the success path, so this is the only place that publishes
// an exception through a ResultState<std::thread::id>.
TEST(FutureResultTest, GetRethrowsPublishedExceptionThreadId) {
    auto* state = new ResultState<std::thread::id>();
    state->setException(std::make_exception_ptr(std::runtime_error("boom")));
    state->release();
    Future<std::thread::id> future(state);
    EXPECT_THROW(future.get(), std::runtime_error);
}
