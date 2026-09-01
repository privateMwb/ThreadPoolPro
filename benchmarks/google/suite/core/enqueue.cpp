// PulseThreadPool Core Benchmark Suite
// Measures submitting a task and obtaining its result, against oneTBB.
//
// Covers:
// - enqueue() + future::get() for a single task
// - the equivalent oneTBB idiom: task_group::run() writing into a
//   captured variable, followed by task_group::wait()
//
// oneTBB's task_group has no per-task future — a result is obtained by
// writing into a captured variable and waiting on the group, which is
// the idiomatic way to get a result out of TBB. Both sides are pinned
// to the same worker count via task_arena / ThreadPool(n).

#include <benchmark/benchmark.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <tbb/task_arena.h>
#include <tbb/task_group.h>

using namespace ThreadPoolPro;

constexpr int kWorkers = 4;

// Measures submitting one task and reading back its computed result.
static void BM_enqueue_result_ptp(benchmark::State& state) {
    ThreadPool pool(kWorkers);

    for (auto _ : state) {
        auto future = pool.enqueue([] { return 21 * 2; });
        benchmark::DoNotOptimize(future.get());
    }
}
BENCHMARK(BM_enqueue_result_ptp)->Name("enqueue + result (PulseThreadPool)")->UseRealTime();

static void BM_enqueue_result_otbb(benchmark::State& state) {
    tbb::task_arena arena(kWorkers);
    arena.initialize(); // Force eager thread creation instead of TBB's default lazy init.
    tbb::task_group tg;

    for (auto _ : state) {
        int result = 0;
        arena.execute([&] { tg.run([&] { result = 21 * 2; }); });
        arena.execute([&] { tg.wait(); });
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_enqueue_result_otbb)->Name("enqueue + result (oneTBB)")->UseRealTime();