# ============================================================
# .clusterfuzzlite/Dockerfile
# ============================================================

FROM gcr.io/oss-fuzz-base/base-builder:latest@sha256:1f9cf74bc3d7029b528494542f9d9851b256dceed38d3f420989434dc5ae0760

COPY . $SRC/ThreadPoolPro
COPY .clusterfuzzlite/build.sh $SRC/

WORKDIR $SRC/ThreadPoolPro