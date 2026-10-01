# ============================================================
# .clusterfuzzlite/Dockerfile
# ============================================================

FROM gcr.io/oss-fuzz-base/base-builder:latest@sha256:eb77f5af37831bde58f992e15783ff23f04ee81af67962b7e0aaa7e1e26a7662

COPY . $SRC/ThreadPoolPro
COPY .clusterfuzzlite/build.sh $SRC/

WORKDIR $SRC/ThreadPoolPro