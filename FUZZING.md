# Fuzzing

ThreadPoolPro is fuzzed via [ClusterFuzzLite](https://google.github.io/clusterfuzzlite/),
running on every pull request that touches the fuzzed files, plus a
longer scheduled batch run every night.

## What's covered

**`fuzz_work_stealing_queue.cpp`** exercises `WorkStealingQueue`'s
owner-thread API (`pushBottom()`/`popBottom()`) against a shadow model,
checking:

- `popBottom()` never returns a task that wasn't pushed
- tasks come back out in LIFO order
- each task's callable runs at most once
- the queue survives repeated forced growth (`Buffer::grow()`) at
  several starting capacities
- `size()` reaches exactly 0 once every pushed task is drained

Built and run under both AddressSanitizer and UndefinedBehaviorSanitizer.

## What's deliberately NOT covered yet

This harness is single-threaded on purpose. It does not exercise
`steal()`'s cross-thread CAS path — the actual lock-free contention
between an owner and a thief. That's a different class of bug
(concurrency races, not memory-safety violations) and needs a
different tool: a concurrent harness under **ThreadSanitizer**, not
ASan/UBSan. Bugs like the `pendingTasks_`/`activeTasks_` ordering race
fixed in `ThreadPool.cpp` are exactly the kind of thing a TSan-based
concurrent harness could catch automatically — that's the natural next
harness to add here, not a replacement for this one.

## Running locally

```bash
git clone --recursive https://github.com/google/oss-fuzz.git
cd oss-fuzz
python infra/helper.py build_fuzzers --sanitizer address ThreadPoolPro /path/to/ThreadPoolPro
python infra/helper.py run_fuzzer ThreadPoolPro fuzz_work_stealing_queue
```

Or, without OSS-Fuzz's tooling, directly with clang:

```bash
clang++ -std=c++23 -pthread -fsanitize=fuzzer,address \
  -Iinclude \
  src/ThreadPoolPro/Detail/Buffer.cpp \
  src/ThreadPoolPro/Detail/Task.cpp \
  src/ThreadPoolPro/Detail/WorkStealingQueue.cpp \
  fuzz/fuzz_work_stealing_queue.cpp \
  -o fuzz_work_stealing_queue

./fuzz_work_stealing_queue
```

Add `-fsanitize=fuzzer,undefined` instead to run under UBSan.

## Reproducing a crash

ClusterFuzzLite uploads the failing input as a workflow artifact when
a run fails. Download it, then:

```bash
./fuzz_work_stealing_queue path/to/crash-<hash>
```

This replays that exact byte sequence through
`LLVMFuzzerTestOneInput()` once, deterministically — no sanitizer flags
needed beyond however the binary was already built.

## Adding a new harness

1. Add `fuzz/fuzz_<target>.cpp` with an `extern "C" int
   LLVMFuzzerTestOneInput(const uint8_t*, size_t)` entry point.
2. Add the matching compile + link block to `.clusterfuzzlite/build.sh`.
3. No workflow changes needed — `cflite_pr.yml`/`cflite_batch.yml`
   build and run every binary `build.sh` produces in `$OUT`.
