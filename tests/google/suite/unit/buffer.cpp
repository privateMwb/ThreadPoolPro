// PulseThreadPool Buffer test suite.
//
// Coverage:
// - at() returns a writable reference to the requested slot
// - at() wraps a logical index into range via the capacity mask
// - grow() doubles capacity
// - grow() copies the live [top, bottom) pointer range into the new buffer
// - grow() leaves the original buffer untouched

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <memory>

using namespace ThreadPoolPro::Detail;

// Verifies at() returns a writable reference to the requested slot.
TEST(BufferTest, AtReturnsWritableSlot) {
    Buffer buffer(8);
    Task* task = reinterpret_cast<Task*>(0x1);
    buffer.at(3) = task;
    EXPECT_EQ(buffer.at(3), task);
}

// Verifies at() wraps a logical index into range via the capacity mask.
TEST(BufferTest, AtWrapsViaMask) {
    Buffer buffer(8);
    Task* task = reinterpret_cast<Task*>(0x2);
    buffer.at(2) = task;
    EXPECT_EQ(buffer.at(10), task); // 10 & 7 == 2
}

// Verifies grow() doubles capacity.
TEST(BufferTest, GrowDoublesCapacity) {
    Buffer buffer(4);
    std::unique_ptr<Buffer> grown(buffer.grow(0, 0));
    EXPECT_EQ(grown->capacity_, 8);
}

// Verifies grow() copies the live [top, bottom) pointer range into the new buffer.
TEST(BufferTest, GrowCopiesLiveRange) {
    Buffer buffer(4);
    Task* a = reinterpret_cast<Task*>(0x1);
    Task* b = reinterpret_cast<Task*>(0x2);
    buffer.at(0) = a;
    buffer.at(1) = b;

    std::unique_ptr<Buffer> grown(buffer.grow(2, 0));
    EXPECT_EQ(grown->at(0), a);
    EXPECT_EQ(grown->at(1), b);
}

// Verifies grow() leaves the original buffer untouched.
TEST(BufferTest, GrowDoesNotMutateOriginal) {
    Buffer buffer(4);
    Task* a = reinterpret_cast<Task*>(0x3);
    buffer.at(0) = a;

    std::unique_ptr<Buffer> grown(buffer.grow(1, 0));
    EXPECT_EQ(buffer.at(0), a);
    EXPECT_EQ(buffer.capacity_, 4);
}
