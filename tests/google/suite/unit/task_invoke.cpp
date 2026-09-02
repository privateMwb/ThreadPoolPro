// PulseThreadPool Task invocation test suite.
//
// Coverage:
// - operator() invokes the wrapped callable
// - operator() on an empty Task throws
// - operator bool() reflects whether a callable is held

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <stdexcept>

using namespace ThreadPoolPro::Detail;

// Verifies operator() invokes the wrapped callable.
TEST(TaskInvokeTest, InvokeRunsCallable) {
    bool ran = false;
    Task task([&ran]() { ran = true; });
    Task moved(std::move(task));
    moved();
    EXPECT_TRUE(ran);
}

// Verifies operator() on an empty Task throws.
TEST(TaskInvokeTest, InvokeOnEmptyThrows) {
    Task task;
    EXPECT_THROW(task(), std::logic_error);
}

// Verifies operator bool() reports whether a callable is held.
TEST(TaskInvokeTest, BoolConversionReflectsState) {
    Task empty;
    EXPECT_FALSE(static_cast<bool>(empty));

    Task holding([]() {});
    EXPECT_TRUE(static_cast<bool>(holding));

    Task moved(std::move(holding));
    moved();
}
