#!/bin/bash

set -eufo pipefail

# Check if we are running no a mac, if not print error and exit 1
if [[ "$(uname -s)" != "Darwin" ]]; then
  echo "This script must be run on a mac"
  exit 1
fi

# Set default build type to Release
BUILD_TYPE=Release

# Check for -debug argument
for arg in "$@"; do
    if [[ $arg == "-debug" ]]; then
        BUILD_TYPE=Debug
        break
    fi
done

pushd superbuild || exit
  mkdir -p build
  pushd build || exit
    cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..
    make
  popd || exit # superbuild/build
popd || exit # superbuild
