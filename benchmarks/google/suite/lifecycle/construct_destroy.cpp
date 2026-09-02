// PulseThreadPool Lifecycle Benchmark Suite
// Measures pool startup/teardown cost against oneTBB.
//
// Covers:
// - ThreadPool(n) construction + destruction
// - the equivalent oneTBB cost: task_arena(n) + eager initialize()
//
// tbb::task_arena is lazy by default — it doesn't spin up worker
// threads until first used. initialize() is called explicitly here so
// both sides pay for eager worker-thread creation, matching what
// ThreadPool's constructor always does.

#include <benchmark/benchmark.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <tbb/task_arena.h>

using namespace ThreadPoolPro;

constexpr int kWorkers = 4;

// Measures full construction and destruction of a pool sized to kWorkers.
static void BM_construct_destroy_ptp(benchmark::State& state) {
    for (auto _ : state) {
        ThreadPool pool(kWorkers);
    }
}
BENCHMARK(BM_construct_destroy_ptp)->Name("construct + destroy (PulseThreadPool)")->UseRealTime();

static void BM_construct_destroy_otbb(benchmark::State& state) {
    for (auto _ : state) {
        tbb::task_arena arena(kWorkers);
        arena.initialize();
    }
}
BENCHMARK(BM_construct_destroy_otbb)->Name("construct + destroy (oneTBB)")->UseRealTime();