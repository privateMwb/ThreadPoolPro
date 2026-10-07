# ============================================================
# .clusterfuzzlite/Dockerfile
# ============================================================

FROM gcr.io/oss-fuzz-base/base-builder:latest@sha256:8a03650a9d44a382fb126c2239790ef078c82bcd39439625233e8ca74203eb53

COPY . $SRC/ThreadPoolPro
COPY .clusterfuzzlite/build.sh $SRC/

WORKDIR $SRC/ThreadPoolPro