#!/bin/bash

set -eufo pipefail

# Check if we are running no a mac, if not print error and exit 1
if [[ "$(uname -s)" != "Darwin" ]]; then
  echo "This script must be run on a mac"
  exit 1
fi

pushd superbuild || exit
  mkdir -p build
  pushd build || exit
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
  popd || exit # superbuild/build
popd || exit # superbuild
