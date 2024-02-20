#!/bin/bash

set -eux

mkdir build

pushd build
    cmake ..
    cmake --build . --parallel -- -j8
popd
