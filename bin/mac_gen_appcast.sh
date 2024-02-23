#!/bin/bash

# First do this 
# 1. In CMakeLists.txt bump project version
# 2. In bin/base.sh bump KAHO_VERSION
# 3. Run this script like bump_versions.sh <build_dir>
/opt/kaho/Release/sparkle/bin/generate_appcast --account kaho_sparkle /opt/kaho/mac_releases/


