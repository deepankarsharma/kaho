#!/bin/bash

CURDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

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

DEBUG_FLAG=""
if [[ $BUILD_TYPE == "Debug" ]]; then
    DEBUG_FLAG="--debug"
fi

"$CURDIR"/ci_build.sh $DEBUG_FLAG
"$CURDIR/"

