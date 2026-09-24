# ============================================================
# .clusterfuzzlite/Dockerfile
# ============================================================

FROM gcr.io/oss-fuzz-base/base-builder:latest@sha256:d0d9fb8232fd746e487e6e8e6f8631060b4a391a64b19ad3d87bdf454d00da50

COPY . $SRC/ThreadPoolPro
COPY .clusterfuzzlite/build.sh $SRC/

WORKDIR $SRC/ThreadPoolPro