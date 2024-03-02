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

cmake ..
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DKAHO_MAC_SIGN_APP=ON ..
cmake --build . --parallel
