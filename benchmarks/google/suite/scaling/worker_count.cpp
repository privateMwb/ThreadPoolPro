// PulseThreadPool Scaling Benchmark Suite
// Measures how per-task submission cost changes as worker count grows,
// against oneTBB — the "varying worker counts" axis called for in the
// original performance review.
//
// Covers:
// - detach() / task_group::run() throughput at 1, 2, 4, 8, 16, and 32
//   workers, submitting a fixed-size batch and draining it each time
//
// Implemented as a Google Benchmark parameter sweep via ->Arg(...):
// each worker count becomes one row per implementation, so the results
// table lines the two implementations up at every worker count.

#include <benchmark/benchmark.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <tbb/task_arena.h>
#include <tbb/task_group.h>

using namespace ThreadPoolPro;

constexpr int kBatch = 256;

// Measures batch submission + drain at a given worker count.
static void BM_worker_count_ptp(benchmark::State& state) {
    const int workers = static_cast<int>(state.range(0));
    ThreadPool pool(static_cast<std::size_t>(workers));

    for (auto _ : state) {
        for (int i = 0; i < kBatch; ++i)
            pool.detach([] {});

        pool.waitIdle();
    }
}
BENCHMARK(BM_worker_count_ptp)
    ->Name("worker count sweep (PulseThreadPool)")
    ->Arg(1)
    ->Arg(2)
    ->Arg(4)
    ->Arg(8)
    ->Arg(16)
    ->Arg(32)
    ->ArgName("workers")
    ->UseRealTime();

static void BM_worker_count_otbb(benchmark::State& state) {
    const int workers = static_cast<int>(state.range(0));
    tbb::task_arena arena(workers);
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
BENCHMARK(BM_worker_count_otbb)
    ->Name("worker count sweep (oneTBB)")
    ->Arg(1)
    ->Arg(2)
    ->Arg(4)
    ->Arg(8)
    ->Arg(16)
    ->Arg(32)
    ->ArgName("workers")
    ->UseRealTime();