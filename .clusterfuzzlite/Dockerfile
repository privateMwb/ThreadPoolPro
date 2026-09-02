# ============================================================
# .clusterfuzzlite/Dockerfile
# Base image for building the fuzz harnesses under ClusterFuzzLite.
# ============================================================

FROM gcr.io/oss-fuzz-base/base-builder:latest

COPY . $SRC/ThreadPoolPro
COPY .clusterfuzzlite/build.sh $SRC/

WORKDIR $SRC/ThreadPoolPro
