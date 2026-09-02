// PulseThreadPool Core Benchmark Suite
// Measures the Chase-Lev queue primitives directly, bypassing ThreadPool.
//
// Covers:
// - pushBottom() + popBottom() on the owning thread (uncontended)
// - steal() from a second thread while the owner keeps pushing/popping
//
// No oneTBB equivalent exists — TBB's internal work-stealing deque isn't
// public API, so there's nothing to compare against at this level. Both
// cases are standalone Google Benchmark cases with no paired variant.

#include <benchmark/benchmark.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <atomic>
#include <thread>

using namespace ThreadPoolPro::Detail;

// Measures a push immediately followed by a pop on the same (owning)
// thread — the uncontended fast path with no atomic RMW required.
static void BM_push_pop_uncontended(benchmark::State& state) {
    WorkStealingQueue queue;

    for (auto _ : state) {
        queue.pushBottom(Task([] {}));
        auto task = queue.popBottom();
        benchmark::DoNotOptimize(task.has_value());
    }
}
BENCHMARK(BM_push_pop_uncontended)->Name("pushBottom + popBottom (uncontended)");

// Measures pushBottom() while a second thread continuously steals —
// the contended cross-thread path.
static void BM_steal_contended(benchmark::State& state) {
    WorkStealingQueue queue;
    std::atomic<bool> stop{false};
    std::atomic<long long> stolen{0};

    std::thread thief([&] {
        while (!stop.load(std::memory_order_relaxed)) {
            if (auto task = queue.steal())
                stolen.fetch_add(1, std::memory_order_relaxed);
            else
                std::this_thread::yield();
        }
    });

    for (auto _ : state) {
        queue.pushBottom(Task([] {}));
    }

    stop.store(true, std::memory_order_relaxed);
    thief.join();
    benchmark::DoNotOptimize(stolen.load());
}
BENCHMARK(BM_steal_contended)->Name("pushBottom (contended by steal)")->UseRealTime();