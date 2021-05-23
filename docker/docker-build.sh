#!/usr/bin/env bash
set -e
cd "$(dirname -- "$(readlink -f -- "$0")")"

sudo docker build -t gpu-fan-meister-build .

cd ..
ROOT_DIR="$(pwd)"

USER_ID="$(id -u)"
GROUP_ID="$(id -g)"

sudo docker run \
    --rm -ti \
    -v"$ROOT_DIR:/gpu-fan-meister" \
    gpu-fan-meister-build \
    /gpu-fan-meister/build.sh
