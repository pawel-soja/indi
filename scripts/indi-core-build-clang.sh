#!/bin/bash

set -e

command -v realpath >/dev/null 2>&1 || realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

SRCS=$(dirname $(realpath $0))/..

mkdir -p build/indi-core
pushd build/indi-core
CC=clang CXX=clang++ cmake \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DFIX_WARNINGS=ON \
    -DCMAKE_BUILD_TYPE=$1 \
    -DINDI_BUILD_UNITTESTS=ON \
    . $SRCS

make -j3
popd
