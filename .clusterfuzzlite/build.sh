#!/bin/bash -eu
# ============================================================
# .clusterfuzzlite/build.sh
#
# Builds fuzz_work_stealing_queue directly against the three .cpp
# files WorkStealingQueue actually needs (Buffer, Task,
# WorkStealingQueue itself) rather than going through the project's
# CMake build. This deliberately skips ThreadPool.cpp/ThreadMarket.cpp
# and the oneTBB dependency entirely — WorkStealingQueue doesn't need
# either, and pulling them in would only add build surface unrelated
# to what this harness fuzzes.
#
# Add more `${SRC}/ThreadPoolPro/fuzz/fuzz_*.cpp` harnesses here as
# they're added; each becomes its own $OUT binary.
# ============================================================

cd "${SRC}/ThreadPoolPro"

$CXX $CXXFLAGS -std=c++23 -pthread \
  -I"${SRC}/ThreadPoolPro/include" \
  -c src/ThreadPoolPro/Detail/Buffer.cpp \
  -o "${WORK}/Buffer.o"

$CXX $CXXFLAGS -std=c++23 -pthread \
  -I"${SRC}/ThreadPoolPro/include" \
  -c src/ThreadPoolPro/Detail/Task.cpp \
  -o "${WORK}/Task.o"

$CXX $CXXFLAGS -std=c++23 -pthread \
  -I"${SRC}/ThreadPoolPro/include" \
  -c src/ThreadPoolPro/Detail/WorkStealingQueue.cpp \
  -o "${WORK}/WorkStealingQueue.o"

$CXX $CXXFLAGS -std=c++23 -pthread \
  -I"${SRC}/ThreadPoolPro/include" \
  "${WORK}/Buffer.o" "${WORK}/Task.o" "${WORK}/WorkStealingQueue.o" \
  fuzz/fuzz_work_stealing_queue.cpp \
  $LIB_FUZZING_ENGINE \
  -o "${OUT}/fuzz_work_stealing_queue"
