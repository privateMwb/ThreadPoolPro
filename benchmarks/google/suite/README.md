# Google Benchmark Suite

This document describes the benchmark categories under `suite/` — what
each one measures, and the individual benchmarks it contains.

Every benchmark compares PulseThreadPool against oneTBB — `task_arena` +
`task_group`, the industry-standard C++ work-stealing scheduler
PulseThreadPool is modeled after.

Some benchmarks have no meaningful oneTBB equivalent — TBB exposes no
public type-erased callable wrapper comparable to `Task`, no public
work-stealing deque, no pause/resume concept, and no per-arena
introspection into queue depth or active/idle thread counts. Those are
standalone Google Benchmark cases (a single `BM_*` function) rather than
a matched pair, timing PulseThreadPool alone. Paired comparisons are two
`BM_*` functions — a `_ptp` and an `_otbb` variant — registered under a
shared `->Name(...)` so they sort together in the results table. Sweeps
across a varying parameter (worker count, backlog depth) use Google
Benchmark's built-in `->Arg(...)` / `->ArgName(...)` mechanism instead of
a hand-rolled loop, so each swept value shows up as its own row per
implementation.

All benchmarks build into a single binary and run through the standard
`BENCHMARK_MAIN()` entry point — no custom suite registration. Use
`--benchmark_filter=<regex>` to run a subset, and
`--benchmark_out=<file> --benchmark_out_format=json` to capture results
for comparison across runs (e.g. with `compare.py` from the Google
Benchmark tooling).

---

## Access

Benchmarks Task invocation cost and already-running pool state —
outside of, and independent from, actual task submission.

### Benchmarks

- `invoke.cpp` — `operator()` on a Task holding a small (SBO-resident)
  callable, `operator()` on a Task holding a large (heap-allocated)
  callable (standalone)
- `state_query.cpp` — `activeTaskCount()` / `queuedTasks()` under load,
  `idleThreadCount()`, `isPaused()` / `isStopped()` / `empty()`
  (standalone)

---

## Core

Benchmarks the fundamental, most frequently exercised operations —
submitting tasks and the work-stealing queue primitives underneath
them.

### Benchmarks

- `enqueue.cpp` — `enqueue()` + `future::get()` for a single task,
  against oneTBB's `task_group::run()` + captured-variable idiom
- `steal.cpp` — `pushBottom()` + `popBottom()` on the owning thread
  (uncontended), `pushBottom()` while a second thread continuously
  steals (contended) (standalone)
- `detach.cpp` — `detach()` for a single task, `detach()` for a batch
  of 64 tasks + drain, against oneTBB's `task_group::run()`

---

## Lifecycle

Benchmarks object lifetime operations — pool construction/destruction
and Task's move semantics. Task is deliberately non-copyable, so
there's no copy_semantics counterpart here.

### Benchmarks

- `construct_destroy.cpp` — `ThreadPool(n)` construction + destruction,
  against `tbb::task_arena(n)` with eager `initialize()`
- `move_semantics.cpp` — move-constructing a Task holding an
  SBO-resident callable, move-constructing a Task holding a
  heap-allocated callable (standalone)

---

## Scaling

Benchmarks how per-operation cost changes as backlog depth or worker
count grows, independent of any fixed-size repeat count — these sweep
a structural parameter via Google Benchmark's `->Arg(...)` rather than
iteration volume.

### Benchmarks

- `queue_depth.cpp` — `pushBottom()` cost at increasing backlog depth
  (0 / 1,024 / 65,536 entries), exercising `WorkStealingQueue`'s
  buffer-growth path via `Buffer::grow()` (standalone; swept via
  `->Arg(...)`, labeled `backlog`)
- `worker_count.cpp` — `detach()` / `task_group::run()` batch
  submit-and-drain throughput at 1, 2, 4, 8, 16, and 32 workers (swept
  via `->Arg(...)`, labeled `workers`)

---

## Utility

Benchmarks pool control and error-handling paths that don't belong to
any of the categories above — pausing/resuming, and running tasks that
throw.

### Benchmarks

- `pause_resume.cpp` — `pause()` + `resume()` cycle on an idle pool
  (standalone)
- `exception_path.cpp` — `detach()` on a task that throws uncaught,
  exercising ThreadPool's own `exceptionCounter_` path (standalone);
  `detach()` on a task that catches its own exception, matched against
  oneTBB with identical catch-inside-task semantics

---

## Conventions

- **Standalone case** — one `BM_<name>` function, registered with
  `BENCHMARK(BM_<name>)->Name("<display name>")`.
- **Paired case** — two functions, `BM_<name>_ptp` and
  `BM_<name>_otbb`, each registered with a shared display-name prefix
  plus a `(PulseThreadPool)` / `(oneTBB)` suffix.
- **Parameter sweep** — a single function reading `state.range(0)`,
  registered once with one `->Arg(...)` call per swept value and an
  `->ArgName(...)` label.
- **`->UseRealTime()`** is set on any benchmark that blocks waiting on
  other threads to finish work (`waitIdle()`, `tg.wait()`, thread
  join, `resume()` waking workers) — CPU time alone would undercount
  wall-clock latency in those cases.
- `benchmark::DoNotOptimize(...)` is used in place of the old
  `doNotOptimize(...)`. For anything larger than a register (arrays,
  structs, containers), pass a pointer (`.data()`, `&x`) rather than
  the object itself — passing the object by value/const-ref trips a
  deprecation warning in newer Google Benchmark versions and is a
  weaker optimization barrier.
