// PulseThreadPool Task construction test suite.
//
// Coverage:
// - A small callable is constructed and invoked correctly (inline SBO path)
// - A large callable is constructed and invoked correctly (heap path)

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <cstddef>

using namespace ThreadPoolPro::Detail;

// Verifies a small callable is constructed and invoked correctly.
TEST(TaskConstructionTest, ConstructSmallCallableInvokes) {
    bool ran = false;
    Task task([&ran]() { ran = true; });
    Task moved(std::move(task));
    moved();
    EXPECT_TRUE(ran);
}

// Verifies a large callable is constructed and invoked correctly.
TEST(TaskConstructionTest, ConstructLargeCallableInvokes) {
    std::byte padding[128]{};
    bool ran = false;

    Task task([&ran, padding]() {
        (void)padding;
        ran = true;
    });

    task();
    EXPECT_TRUE(ran);
}
