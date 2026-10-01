# ============================================================
# .clusterfuzzlite/Dockerfile
# ============================================================

FROM gcr.io/oss-fuzz-base/base-builder:latest@sha256:e67da2b0f6156b29b62734305a8c74db61aaf3158c7ab96386a07974fb33cdc8

COPY . $SRC/ThreadPoolPro
COPY .clusterfuzzlite/build.sh $SRC/

WORKDIR $SRC/ThreadPoolPro