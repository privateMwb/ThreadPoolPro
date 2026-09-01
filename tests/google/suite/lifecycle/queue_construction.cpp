// PulseThreadPool WorkStealingQueue lifecycle test suite.
//
// Coverage:
// - Destroying an empty queue is safe
// - The destructor releases any task still queued at destruction time
// - Pushing past the initial capacity grows the buffer without losing
//   or corrupting already-queued tasks

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <vector>

using namespace ThreadPoolPro::Detail;

namespace {

// A callable whose destructor counts how many times it runs, used to
// verify the queue's destructor actually destroys any Task it still
// holds rather than leaking it.
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

} // namespace

// Verifies destroying an empty queue is safe.
TEST(QueueConstructionTest, DestroyingEmptyQueueIsSafe) {
    WorkStealingQueue queue;
    EXPECT_EQ(queue.size(), 0u);
}

// Verifies the destructor releases any task still queued at destruction.
TEST(QueueConstructionTest, DestructorReleasesQueuedTask) {
    int destructions = 0;
    {
        WorkStealingQueue queue(4);
        queue.pushBottom(Task(CountedCallable(&destructions)));
    }
    EXPECT_EQ(destructions, 1);
}

// Verifies pushing past the initial capacity grows the buffer without
// losing or corrupting already-queued tasks.
TEST(QueueConstructionTest, GrowthPreservesQueuedTasks) {
    WorkStealingQueue queue(4);
    std::vector<int> observed;

    for (int i = 0; i < 20; ++i)
        queue.pushBottom(Task([i, &observed]() { observed.push_back(i); }));

    EXPECT_EQ(queue.size(), 20u);

    while (auto task = queue.popBottom())
        (*task)();

    std::vector<int> expected;
    for (int i = 19; i >= 0; --i)
        expected.push_back(i);

    EXPECT_EQ(observed, expected);
}
