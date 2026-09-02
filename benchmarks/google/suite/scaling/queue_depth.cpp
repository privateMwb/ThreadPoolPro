// PulseThreadPool Scaling Benchmark Suite
// Measures how pushBottom() cost changes as the queue's backlog grows,
// independent of the SMALL/MEDIUM/LARGE iteration tiers applied
// uniformly elsewhere in this suite — this is about the size of a
// single un-drained backlog, not how many times an operation repeats.
//
// Covers:
// - pushBottom() cost at increasing backlog depth, exercising
//   WorkStealingQueue's buffer-growth path (Buffer::grow())
//
// This is specific to this library's internal buffer-growth behavior —
// no oneTBB equivalent applies, so this is a standalone Google
// Benchmark case with no paired variant. Implemented as a parameter
// sweep via ->Arg(...) rather than a manual loop over backlog sizes.

#include <benchmark/benchmark.h>

#include <ThreadPoolPro/ThreadPool.h>

using namespace ThreadPoolPro::Detail;

// Measures pushBottom() cost once the queue already holds `backlog`
// un-drained tasks, forcing repeated buffer growth as backlog increases.
static void BM_push_at_backlog(benchmark::State& state) {
    const std::size_t backlog = static_cast<std::size_t>(state.range(0));
    WorkStealingQueue queue;

    for (std::size_t i = 0; i < backlog; ++i)
        queue.pushBottom(Task([] {}));

    for (auto _ : state) {
        queue.pushBottom(Task([] {}));
    }

    while (queue.popBottom().has_value()) {
    }
}
BENCHMARK(BM_push_at_backlog)
    ->Name("pushBottom at backlog")
    ->Arg(0)
    ->Arg(1024)
    ->Arg(65536)
    ->ArgName("backlog");