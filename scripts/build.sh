#!/bin/bash
set -x

build_type="${1:-Release}"
mkdir -p build
pushd build
cmake ../ -DCMAKE_BUILD_TYPE=${build_type} .
make
popd
