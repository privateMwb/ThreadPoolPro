// PulseThreadPool Utility Benchmark Suite
// Measures the cost of running a task that throws.
//
// Covers:
// - detach() on a task that throws uncaught, letting ThreadPool's own
//   catch-and-count path (exceptionCounter_) handle it
// - the same workload with matched semantics against oneTBB: each task
//   catches its own exception and increments a shared counter, so both
//   sides observably run all N tasks to completion
//
// The first case has no fair oneTBB comparison: tbb::task_group cancels
// remaining submitted tasks after the first uncaught exception it
// observes, while ThreadPool counts every one and keeps running the
// rest — timing "run N throwing tasks" against each other would
// silently compare two different workloads (TBB wouldn't actually run
// all N), so it is a standalone Google Benchmark case with no paired
// variant. The second case sidesteps that mismatch by catching inside
// the task itself on both sides, so neither library's
// cancellation/counting behavior is in play — what's left to measure
// is genuinely comparable: the cost of throwing and catching an
// exception once per task, at the language level, under each scheduler.

#include <benchmark/benchmark.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <tbb/task_arena.h>
#include <tbb/task_group.h>

#include <atomic>
#include <stdexcept>

using namespace ThreadPoolPro;

constexpr int kWorkers = 4;

// Measures detach() throughput when every task throws uncaught,
// exercising ThreadPool's own exceptionCounter_ path. No oneTBB
// counterpart — see the file-level comment.
static void BM_uncaught_exception(benchmark::State& state) {
    ThreadPool pool(kWorkers);

    for (auto _ : state) {
        pool.detach([] { throw std::runtime_error("bench"); });
    }

    pool.waitIdle();
    benchmark::DoNotOptimize(pool.exceptionCount());
}
BENCHMARK(BM_uncaught_exception)->Name("detach (uncaught exception)")->UseRealTime();

// Measures throw-and-catch cost per task, with both sides swallowing
// the exception inside the task itself so all N tasks genuinely run on
// both sides — a fair comparison against oneTBB.
static void BM_caught_exception_ptp(benchmark::State& state) {
    ThreadPool pool(kWorkers);
    std::atomic<long long> counter{0};

    for (auto _ : state) {
        pool.detach([&] {
            try {
                throw std::runtime_error("bench");
            } catch (...) {
                counter.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    pool.waitIdle();
    benchmark::DoNotOptimize(counter.load());
}
BENCHMARK(BM_caught_exception_ptp)
    ->Name("detach (caught exception) (PulseThreadPool)")
    ->UseRealTime();

static void BM_caught_exception_otbb(benchmark::State& state) {
    tbb::task_arena arena(kWorkers);
    arena.initialize();
    tbb::task_group tg;
    std::atomic<long long> counter{0};

    for (auto _ : state) {
        arena.execute([&] {
            tg.run([&] {
                try {
                    throw std::runtime_error("bench");
                } catch (...) {
                    counter.fetch_add(1, std::memory_order_relaxed);
                }
            });
        });
    }

    arena.execute([&] { tg.wait(); });
    benchmark::DoNotOptimize(counter.load());
}
BENCHMARK(BM_caught_exception_otbb)->Name("detach (caught exception) (oneTBB)")->UseRealTime();