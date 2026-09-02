// PulseThreadPool WorkStealingQueue test suite.
//
// Coverage:
// - popBottom() on an empty queue returns nullopt
// - pushBottom()/popBottom() round-trips a task correctly
// - popBottom() takes tasks in LIFO order from the owner end
// - steal() on an empty queue returns nullopt
// - steal() takes from the top (oldest-pushed) end
// - A stolen task is invocable and only delivered once
// - size() tracks pushBottom(), popBottom(), and steal()

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <vector>

using namespace ThreadPoolPro::Detail;

// Verifies popBottom() on an empty queue returns nullopt.
TEST(WorkStealingQueueTest, PopOnEmptyReturnsNullopt) {
    WorkStealingQueue queue;
    EXPECT_FALSE(queue.popBottom().has_value());
}

// Verifies pushBottom()/popBottom() round-trips a task correctly.
TEST(WorkStealingQueueTest, PushThenPopReturnsSameTask) {
    WorkStealingQueue queue;
    int value = 0;
    queue.pushBottom(Task([&value]() { value = 7; }));

    auto task = queue.popBottom();
    EXPECT_TRUE(task.has_value());
    (*task)();
    EXPECT_EQ(value, 7);
}

// Verifies popBottom() takes tasks in LIFO order from the owner end.
TEST(WorkStealingQueueTest, PopOrderIsLifo) {
    WorkStealingQueue queue;
    std::vector<int> order;

    queue.pushBottom(Task([&order]() { order.push_back(1); }));
    queue.pushBottom(Task([&order]() { order.push_back(2); }));
    queue.pushBottom(Task([&order]() { order.push_back(3); }));

    (*queue.popBottom())();
    (*queue.popBottom())();
    (*queue.popBottom())();

    EXPECT_EQ(order, (std::vector<int>{3, 2, 1}));
}

// Verifies steal() on an empty queue returns nullopt.
TEST(WorkStealingQueueTest, StealOnEmptyReturnsNullopt) {
    WorkStealingQueue queue;
    EXPECT_FALSE(queue.steal().has_value());
}

// Verifies steal() takes from the top (oldest-pushed) end.
TEST(WorkStealingQueueTest, StealTakesOldestTask) {
    WorkStealingQueue queue;
    std::vector<int> order;

    queue.pushBottom(Task([&order]() { order.push_back(1); }));
    queue.pushBottom(Task([&order]() { order.push_back(2); }));
    queue.pushBottom(Task([&order]() { order.push_back(3); }));

    (*queue.steal())();
    (*queue.steal())();

    EXPECT_EQ(order, (std::vector<int>{1, 2}));
}

// Verifies a stolen task is invocable and only delivered once.
TEST(WorkStealingQueueTest, StolenTaskRunsExactlyOnce) {
    WorkStealingQueue queue;
    int calls = 0;
    queue.pushBottom(Task([&calls]() { ++calls; }));

    auto stolen = queue.steal();
    EXPECT_TRUE(stolen.has_value());
    (*stolen)();
    EXPECT_EQ(calls, 1);
    EXPECT_EQ(queue.size(), 0u);
}

// Verifies a freshly constructed queue reports size 0.
TEST(WorkStealingQueueTest, FreshQueueIsEmpty) {
    WorkStealingQueue queue;
    EXPECT_EQ(queue.size(), 0u);
}

// Verifies size() grows with pushBottom() and shrinks with popBottom().
TEST(WorkStealingQueueTest, SizeTracksPushAndPop) {
    WorkStealingQueue queue;
    queue.pushBottom(Task([]() {}));
    queue.pushBottom(Task([]() {}));
    EXPECT_EQ(queue.size(), 2u);

    auto popped = queue.popBottom();
    EXPECT_EQ(queue.size(), 1u);
    (*popped)();

    auto remaining = queue.popBottom();
    (*remaining)();
}

// Verifies size() shrinks with steal() too.
TEST(WorkStealingQueueTest, SizeTracksSteal) {
    WorkStealingQueue queue;
    queue.pushBottom(Task([]() {}));
    queue.pushBottom(Task([]() {}));

    auto stolen = queue.steal();
    EXPECT_EQ(queue.size(), 1u);
    (*stolen)();

    auto remaining = queue.popBottom();
    (*remaining)();
}
