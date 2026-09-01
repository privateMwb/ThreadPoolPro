// PulseThreadPool Core Benchmark Suite
// Measures fire-and-forget task submission against oneTBB.
//
// Covers:
// - detach() for a single task, no result observed
// - the direct oneTBB equivalent: task_group::run()
//
// This is the cleanest apples-to-apples comparison in the suite: both
// detach() and task_group::run() are "submit and don't wait" by design.
// Each case is registered as a matched pair of Google Benchmark
// functions — one per implementation — sharing the same display name
// prefix so they line up in the results table.

#include <benchmark/benchmark.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <tbb/task_arena.h>
#include <tbb/task_group.h>

using namespace ThreadPoolPro;

constexpr int kWorkers = 4;
constexpr int kBatch = 64;

// Measures submitting a single no-op task without observing a result.
static void BM_detach_single_ptp(benchmark::State& state) {
    ThreadPool pool(kWorkers);

    for (auto _ : state) {
        pool.detach([] {});
    }
}
BENCHMARK(BM_detach_single_ptp)->Name("detach single task (PulseThreadPool)")->UseRealTime();

static void BM_detach_single_otbb(benchmark::State& state) {
    tbb::task_arena arena(kWorkers);
    arena.initialize();
    tbb::task_group tg;

    for (auto _ : state) {
        arena.execute([&] { tg.run([] {}); });
    }

    arena.execute([&] { tg.wait(); });
}
BENCHMARK(BM_detach_single_otbb)->Name("detach single task (oneTBB)")->UseRealTime();

// Measures submitting a batch of tasks, then waiting for the whole
// batch to finish — the realistic way detach()/run() are actually used.
static void BM_detach_batch_drain_ptp(benchmark::State& state) {
    ThreadPool pool(kWorkers);

    for (auto _ : state) {
        for (int i = 0; i < kBatch; ++i)
            pool.detach([] {});

        pool.waitIdle();
    }
}
BENCHMARK(BM_detach_batch_drain_ptp)
    ->Name("detach batch + drain (64 tasks) (PulseThreadPool)")
    ->UseRealTime();

static void BM_detach_batch_drain_otbb(benchmark::State& state) {
    tbb::task_arena arena(kWorkers);
    arena.initialize();
    tbb::task_group tg;

    for (auto _ : state) {
        arena.execute([&] {
            for (int i = 0; i < kBatch; ++i)
                tg.run([] {});
        });
        arena.execute([&] { tg.wait(); });
    }
}
BENCHMARK(BM_detach_batch_drain_otbb)
    ->Name("detach batch + drain (64 tasks) (oneTBB)")
    ->UseRealTime();