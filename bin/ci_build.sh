#!/bin/bash

set -eux

# Set default build type to Release
BUILD_TYPE=Release

# Check for -debug argument
for arg in "$@"; do
    if [[ $arg == "-debug" ]]; then
        BUILD_TYPE=Debug
        break
    fi
done

mkdir build-${BUILD_TYPE}
pushd build-${BUILD_TYPE}
    cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..
    cmake --build . --parallel
popd
