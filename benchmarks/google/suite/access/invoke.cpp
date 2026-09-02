// PulseThreadPool Access Benchmark Suite
// Measures Task invocation overhead in isolation, outside of any pool.
//
// Covers:
// - operator() on a Task holding a small (SBO-resident) callable
// - operator() on a Task holding a large (heap-allocated) callable
//
// No oneTBB equivalent exists at this granularity — TBB exposes no
// public type-erased callable wrapper comparable to Task, so these are
// standalone Google Benchmark cases with no paired "_tbb" variant.

#include <benchmark/benchmark.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <array>

using namespace ThreadPoolPro::Detail;

// Measures operator() on a Task whose callable fits inline storage.
static void BM_invoke_small(benchmark::State& state) {
    int counter = 0;
    Task task([&counter] { ++counter; });

    for (auto _ : state) {
        task();
    }

    benchmark::DoNotOptimize(counter);
}
BENCHMARK(BM_invoke_small)->Name("invoke small (SBO)");

// Measures operator() on a Task whose callable is too large for inline
// storage and was heap-allocated at construction.
static void BM_invoke_large(benchmark::State& state) {
    std::array<int, 16> payload{};
    int counter = 0;
    Task task([&counter, payload] {
        ++counter;
        benchmark::DoNotOptimize(payload.data());
    });

    for (auto _ : state) {
        task();
    }

    benchmark::DoNotOptimize(counter);
}
BENCHMARK(BM_invoke_large)->Name("invoke large (heap)");