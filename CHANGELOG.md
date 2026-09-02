# Changelog

All notable changes to ThreadPoolPro are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Nothing yet.

## [1.0.0] - 2026-07-26

The first stable release of ThreadPoolPro, a work-stealing C++ thread
pool for modern C++23.

### Added
- Lock-free, per-worker Chase-Lev work-stealing deques for local task
  storage.
- Type-erased task storage with small-buffer optimization — no
  allocation for small, nothrow-movable callables.
- Sharded, mutex-guarded injection queues for submissions from
  non-worker threads.
- Lightweight `Future`/`ResultState` replacing `std::packaged_task` +
  `std::future`.
- Process-wide `ThreadMarket` of leased, persistent OS threads — cheap
  pool construction/destruction after warm-up.
- Atomic wake-token park/wait for worker synchronization instead of
  `condition_variable` + mutex.
- `enqueue()` (with result) and `detach()` (fire-and-forget) submission
  APIs.
- Pause/resume support for in-flight task execution.
- Graceful and immediate shutdown modes (`FinishTasks` /
  `DiscardTasks`).
- `waitIdle()` — blocks until the pool drains, helping steal/execute
  tasks itself rather than sitting idle.
- Runtime statistics: active tasks, queued tasks, exception count, idle
  thread count.
- Move-only, exception-safe `Task` wrapper with explicit heap/inline
  storage paths.

### Performance
- Local push/pop costs no atomic read-modify-write in the uncontended
  case.
- Randomized steal-victim offsets spread contention across workers
  instead of serializing on the same low-index victim.
- Sharded injection queues avoid a single shared lock becoming the
  dominant contention point as worker count grows.
- Small-buffer-optimized `Task` storage avoids a heap allocation per
  submitted callable in the common case.
- `ResultState` is a single, purpose-sized heap allocation per
  `enqueue()`, replacing `std::packaged_task`'s larger general-purpose
  shared state.
- Spin → yield → park backoff avoids paying park/wake syscall latency
  for short-lived bursts of work.
- `ThreadMarket` reuses already-running OS threads across `ThreadPool`
  lifetimes, avoiding thread-creation cost on repeated
  construction/destruction.
- Benchmarked against oneTBB at 10K / 100K / 1M iterations (and
  1k–100k for worker-count scaling); ThreadPoolPro currently trails
  oneTBB — a mature, heavily-tuned production library — on nearly
  every apples-to-apples throughput comparison, sometimes
  substantially so at higher worker counts and batch sizes. The one
  exception measured is the caught-exception path under `detach()`,
  where ThreadPoolPro is modestly faster. Full results in
  `benchmarks/results/v1_0_0.md`.

### Testing
- Comprehensive test suite covering unit, integration, lifecycle,
  regression, and concurrency tests; move semantics; exception safety;
  task submission and retrieval (local queue, stealing, injection
  shards); pause/resume behavior; shutdown modes (`FinishTasks` /
  `DiscardTasks`), including self-detach from within a running task;
  Future/ResultState lifecycle, including error paths; and VTable
  operation round-trips (invoke, move, destroy, heap-delete).
- 93.3% line coverage (513/550 lines) and 96.9% function coverage
  (377/389 functions), excluding test infrastructure.

### CI
- Automated builds and tests across GCC, Clang, MSVC, and AppleClang,
  each in Debug and Release configurations.

[Unreleased]: https://github.com/privateMwb/ThreadPoolPro/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/privateMwb/ThreadPoolPro/releases/tag/v1.0.0
