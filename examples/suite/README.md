# Example Suite

This document describes the example categories under `suite/` — what each one
demonstrates, and the individual example files it contains.

| Category | Focus |
|---|---|
| [Advanced](#advanced) | Deeper pool mechanics — exceptions, stats, shutdown modes, work stealing |
| [Integration](#integration) | Interoperability with the rest of a codebase |
| [Misuse](#misuse) | Common mistakes and the exceptions they lead to |
| [Patterns](#patterns) | Common usage idioms built on the core API |
| [Quickstart](#quickstart) | Fundamental, everyday usage |

Unlike the test suite, an example doesn't assert correctness — it
demonstrates real usage of the library, including deliberate misuse where
instructive (see Misuse), so the reader sees both the correct pattern and the
mistake it guards against.

Every example file ends with `REGISTER_EXAMPLE_SUITE()`, which derives the
suite's category from its containing directory and assigns it a sequential id
within that category. This applies uniformly across every category below.

---

## Advanced

Demonstrates deeper mechanics of the pool — exception propagation versus
swallowing, the runtime stats counters, the difference between the two
shutdown modes, and the observable effect of work stealing.

### Examples

| File | What it covers |
|---|---|
| `discard_shutdown.cpp` | `shutdown(DiscardTasks)`: in-flight tasks still finish, queued ones don't; only the first `shutdown()` call's mode takes effect |
| `exception_handling.cpp` | `get()` rethrowing a task's exact exception type, catching by base class, and `detach()` swallowing exceptions into `exceptionCount()` instead |
| `runtime_stats.cpp` | `threadCount()`, `activeTaskCount()`, `queuedTasks()`, `idleThreadCount()`, and `exceptionCount()` as the pool runs, drains, and accumulates |
| `worker_stealing.cpp` | A single task fanning children onto its own worker's queue, and the wall-clock gap between four workers and one |

---

## Integration

Demonstrates interoperability with the rest of a codebase — building
parallel algorithm primitives on top of `enqueue()`/`detach()`, chaining task
results into pipelines, and a producer/consumer pattern.

### Examples

| File | What it covers |
|---|---|
| `parallel_for.cpp` | A `parallelFor()` built from `enqueue()`, splitting a range into per-thread chunks and joining on their futures |
| `pipeline_stages.cpp` | Chaining a task's result into the next stage, both sequentially and inside a single task, then several pipelines run as phases |
| `producer_consumer.cpp` | `detach()` as a lightweight consumer per produced item, with `waitIdle()` as the join point |
| `stl_algorithms.cpp` | Pool-backed `parallelTransform()` and `parallelCountIf()`, checked against their serial `std::` equivalents |

---

## Misuse

Demonstrates common mistakes and the exceptions they lead to, alongside the
correct pattern — including the one call (`waitIdle()` from a pool's own
worker) that's unsafe enough to be shown but not exercised.

### Examples

| File | What it covers |
|---|---|
| `double_get.cpp` | A second `get()` on the same Future throwing `std::logic_error`; `valid()` as the guard, and what moving actually transfers |
| `empty_future_get.cpp` | `get()` on a default-constructed or moved-from Future throwing `std::logic_error` |
| `submit_after_shutdown.cpp` | `enqueue()`/`detach()` throwing `std::runtime_error` once `shutdown()` has run; `isStopped()` as the check to make first |
| `worker_self_join.cpp` | `shutdown()` called safely from a worker's own task, and why `waitIdle()` from a worker is the one call to avoid |

---

## Patterns

Demonstrates common usage idioms built on top of the core API — submitting
and collecting a batch, pausing and resuming work, waiting for the pool to
drain, and a running task submitting further work.

### Examples

| File | What it covers |
|---|---|
| `batch_enqueue.cpp` | Submitting a batch before reading any result, then collecting futures back in order |
| `nested_submit.cpp` | A running task detaching, enqueueing, and fanning out further work onto its own pool |
| `pause_resume.cpp` | `pause()` holding new tasks back, `isPaused()`, and `resume()` releasing the queue |
| `wait_idle.cpp` | `queuedTasks()`/`activeTaskCount()` while busy, `waitIdle()` blocking until drained, `empty()` as a non-blocking check |

---

## Quickstart

Demonstrates fundamental, everyday usage — construction, submitting work
with and without a result, and shutting the pool down.

### Examples

| File | What it covers |
|---|---|
| `basic_usage.cpp` | Construction, `enqueue()`/`detach()`, `waitIdle()`, `threadCount()`, `shutdown()` |
| `detach_task.cpp` | Fire-and-forget submission, capturing state, and `exceptionCount()` instead of a thrown exception |
| `enqueue_future.cpp` | Future's `get()`, `valid()`, collecting several futures, and exception propagation |
| `pool_shutdown.cpp` | `FinishTasks` vs `DiscardTasks`, `isStopped()`, and the destructor's implicit `shutdown()` |
