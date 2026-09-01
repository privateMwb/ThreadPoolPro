// PulseThreadPool Access Benchmark Suite
// Measures the cost of reading already-running pool state.
//
// Covers:
// - activeTaskCount() / queuedTasks() under load
// - idleThreadCount()
// - isPaused() / isStopped() / empty()
//
// No oneTBB equivalent exists for most of these — tbb::task_arena
// exposes no per-arena queue depth, active-task count, or idle-thread
// introspection. These are standalone Google Benchmark cases with no
// paired oneTBB variant.

#include <benchmark/benchmark.h>

#include <ThreadPoolPro/ThreadPool.h>

using namespace ThreadPoolPro;

// Measures activeTaskCount() and queuedTasks() while the pool is
// continuously busy running long-lived tasks in the background.
static void BM_task_counts(benchmark::State& state) {
    ThreadPool pool(4);

    for (int i = 0; i < 4; ++i)
        pool.detach([] { std::this_thread::sleep_for(std::chrono::seconds(30)); });

    for (auto _ : state) {
        benchmark::DoNotOptimize(pool.activeTaskCount());
        benchmark::DoNotOptimize(pool.queuedTasks());
    }

    pool.shutdown(ThreadPool::ShutdownMode::DiscardTasks);
}
BENCHMARK(BM_task_counts)->Name("active/queued counts");

// Measures idleThreadCount() on a pool with no work queued.
static void BM_idle_count(benchmark::State& state) {
    ThreadPool pool(4);

    for (auto _ : state) {
        benchmark::DoNotOptimize(pool.idleThreadCount());
    }
}
BENCHMARK(BM_idle_count)->Name("idle thread count");

// Measures the cheap boolean state queries together.
static void BM_bool_state(benchmark::State& state) {
    ThreadPool pool(4);

    for (auto _ : state) {
        benchmark::DoNotOptimize(pool.isPaused());
        benchmark::DoNotOptimize(pool.isStopped());
        benchmark::DoNotOptimize(pool.empty());
    }
}
BENCHMARK(BM_bool_state)->Name("paused/stopped/empty");