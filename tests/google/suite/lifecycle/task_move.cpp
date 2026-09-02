// PulseThreadPool Task move semantics test suite.
//
// Coverage:
// - Move construction transfers the callable exactly once
// - Move assignment transfers the callable exactly once
// - Move construction correctly transfers a heap-allocated callable

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <cstddef>

using namespace ThreadPoolPro::Detail;

namespace {

// A callable whose destructor counts how many times it runs, used to
// verify Task's move constructor/assignment transfer ownership exactly
// once rather than double-destroying or leaking the wrapped callable.
struct CountedCallable {
    int* destructions;

    explicit CountedCallable(int* d) noexcept : destructions{d} {}

    CountedCallable(CountedCallable&& other) noexcept : destructions{other.destructions} {
        other.destructions = nullptr;
    }

    CountedCallable(const CountedCallable&) = delete;

    ~CountedCallable() {
        if (destructions)
            ++(*destructions);
    }

    void operator()() const {}
};

// Padded past SboCapacity (48 bytes) so Task heap-allocates it instead
// of storing it inline.
struct LargeCountedCallable : CountedCallable {
    using CountedCallable::CountedCallable;
    std::byte padding[64]{};
};

} // namespace

// Verifies move construction transfers the callable exactly once.
TEST(TaskMoveTest, MoveConstructTransfersCallable) {
    int destructions = 0;
    {
        Task original{CountedCallable(&destructions)};
        Task moved(std::move(original));

        EXPECT_FALSE(static_cast<bool>(original));
        EXPECT_TRUE(static_cast<bool>(moved));

        moved();
    }
    EXPECT_EQ(destructions, 1);
}

// Verifies move assignment transfers the callable exactly once.
TEST(TaskMoveTest, MoveAssignTransfersCallable) {
    int destructions = 0;
    {
        Task original{CountedCallable(&destructions)};
        Task target;
        target = std::move(original);

        EXPECT_FALSE(static_cast<bool>(original));
        EXPECT_TRUE(static_cast<bool>(target));

        target();
    }
    EXPECT_EQ(destructions, 1);
}

// Verifies move construction correctly transfers a heap-allocated callable.
TEST(TaskMoveTest, MoveConstructTransfersHeapCallable) {
    int destructions = 0;
    {
        Task original{LargeCountedCallable(&destructions)};
        Task moved(std::move(original));

        EXPECT_FALSE(static_cast<bool>(original));
        EXPECT_TRUE(static_cast<bool>(moved));

        moved();
    }
    EXPECT_EQ(destructions, 1);
}
