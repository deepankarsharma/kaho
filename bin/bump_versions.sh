#!/bin/bash

# First do this 
# 1. In CMakeLists.txt bump project version
# 2. In bin/base.sh bump KAHO_VERSION
# 3. Run this script like bump_versions.sh <build_dir>


# Check for a single argument
if [ $# -ne 1 ]; then
  echo "Usage: $0 <build_dir>"
  exit 1
fi
BUILD_DIR=$1

# Get the current directory
CURDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. $CURDIR/base.sh

pushd "$BUILD_DIR" || exit 1
    # Build the dmg
    "$CURDIR/mac_build_dmg.sh"
popd || exit 1

/opt/kaho/Release/sparkle/bin/generate_appcast --account kaho_sparkle /opt/kaho/mac_releases/
rclone sync /opt/kaho/mac_releases r2:kaho-mac-updates

