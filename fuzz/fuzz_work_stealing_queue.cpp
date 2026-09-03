// ============================================================
// fuzz/work_stealing_queue.cpp
//
// Fuzzes WorkStealingQueue's owner-thread API (pushBottom/popBottom)
// against a shadow model, single-threaded. Checks that:
//   - popBottom() never returns a task that wasn't pushed
//   - tasks come back out in LIFO order
//   - each task's callable runs at most once
//   - the queue survives many pushes that force Buffer::grow()
//     repeatedly, at several starting capacities
//   - size() reaches exactly 0 once every pushed task is drained
//
// Deliberately single-threaded: this targets memory-safety bugs in
// Buffer's growth/copy logic and the owner-thread free-list recycling
// (acquireNode/releaseNode), which is where ASan/UBSan add the most
// value. It does NOT exercise steal()'s cross-thread CAS path — that
// needs a concurrent harness under ThreadSanitizer instead, since
// races there are line-of-business, not memory-safety, bugs. See
// FUZZING.md for why that's a deliberately separate follow-up rather
// than bolted onto this harness.
// ============================================================

#include <ThreadPoolPro/Detail/WorkStealingQueue.h>

#include <cstdint>
#include <cstdlib>
#include <deque>
#include <vector>

using ThreadPoolPro::Detail::Task;
using ThreadPoolPro::Detail::WorkStealingQueue;

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    if (size == 0)
        return 0;

    // First byte selects a starting capacity, biased toward small
    // values so most runs are forced through Buffer::grow() at least
    // once rather than only exercising a comfortably pre-sized buffer.
    static constexpr std::size_t kCapacities[] = {1, 2, 4, 1024};
    const std::size_t capacity = kCapacities[data[0] % 4];
    ++data;
    --size;

    WorkStealingQueue queue(capacity);

    // Shadow model: ids currently believed queued, in push order.
    // back() is what popBottom() should hand back next.
    std::deque<int> shadow;
    std::vector<int> executionCounts;
    int nextId = 0;

    for (std::size_t i = 0; i < size; ++i) {
        // Bias roughly 2:1 toward push so runs actually accumulate
        // enough depth to hit growth, rather than oscillating near-empty.
        const bool doPush = (data[i] % 3) != 0;

        if (doPush) {
            const int id = nextId++;
            executionCounts.push_back(0);

            queue.pushBottom(Task([id, &executionCounts]() { ++executionCounts[static_cast<std::size_t>(id)]; }));
            shadow.push_back(id);
            continue;
        }

        auto task = queue.popBottom();

        if (shadow.empty()) {
            // Nothing queued — single-threaded, so popBottom() must agree.
            if (task.has_value())
                std::abort();
            continue;
        }

        if (!task.has_value()) {
            // No concurrent stealer exists in this harness, so a
            // non-empty shadow must never fail to pop.
            std::abort();
        }

        const int expected = shadow.back();
        shadow.pop_back();
        (*task)();

        if (executionCounts[static_cast<std::size_t>(expected)] != 1)
            std::abort(); // wrong task returned, or it ran more than once
    }

    // Drain whatever's left; every remaining shadow entry must come
    // back out, still in LIFO order, exactly once.
    while (!shadow.empty()) {
        auto task = queue.popBottom();
        if (!task.has_value())
            std::abort();

        const int expected = shadow.back();
        shadow.pop_back();
        (*task)();

        if (executionCounts[static_cast<std::size_t>(expected)] != 1)
            std::abort();
    }

    if (queue.size() != 0)
        std::abort();

    return 0;
}
