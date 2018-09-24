#!/usr/bin/env bash

set -euo pipefail

mkdir build || true

pushd build

cmake ..
make

popd

BINARY_NAME="$(cat configuration/binary_name.var)"

./build/${BINARY_NAME} $@
