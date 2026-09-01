// PulseThreadPool Utility Benchmark Suite
// Measures pause()/resume() overhead.
//
// Covers:
// - pause() on a pool with no active work
// - resume() (which must wake every idle worker)
//
// No oneTBB equivalent exists — tbb::task_arena has no pause/resume
// concept — so this is a standalone Google Benchmark case with no
// paired variant.

#include <benchmark/benchmark.h>

#include <ThreadPoolPro/ThreadPool.h>

using namespace ThreadPoolPro;

constexpr int kWorkers = 4;

// Measures a pause()/resume() cycle on an idle pool.
static void BM_pause_resume_cycle(benchmark::State& state) {
    ThreadPool pool(kWorkers);

    for (auto _ : state) {
        pool.pause();
        pool.resume();
    }
}
BENCHMARK(BM_pause_resume_cycle)->Name("pause + resume cycle")->UseRealTime();