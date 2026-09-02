// PulseThreadPool Lifecycle Benchmark Suite
// Measures Task's move construction cost, in isolation from any pool.
//
// Covers:
// - move-constructing a Task holding an SBO-resident callable
// - move-constructing a Task holding a heap-allocated callable
//
// Task is deliberately non-copyable, so there's no copy_semantics
// counterpart here. No oneTBB equivalent exists — TBB has no comparable
// type-erased, relocatable callable wrapper — so both cases are
// standalone Google Benchmark cases with no paired variant.

#include <benchmark/benchmark.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <array>

using namespace ThreadPoolPro::Detail;

// Measures move-constructing a Task whose callable fits inline storage.
static void BM_move_small(benchmark::State& state) {
    for (auto _ : state) {
        Task source([] {});
        Task moved(std::move(source));
    }
}
BENCHMARK(BM_move_small)->Name("move construct (SBO)");

// Measures move-constructing a Task whose callable was heap-allocated.
static void BM_move_large(benchmark::State& state) {
    std::array<int, 16> payload{};

    for (auto _ : state) {
        Task source([payload] { benchmark::DoNotOptimize(payload.data()); });
        Task moved(std::move(source));
    }
}
BENCHMARK(BM_move_large)->Name("move construct (heap)");