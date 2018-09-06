#!/usr/bin/env bash

set -euo pipefail

mkdir build || true

pushd build

cmake ..
make

popd

./build/retro-voxel-display $@
