#!/bin/bash

CURDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"


export PATH=$CURDIR/../superbuild/third_party/bin:$PATH
cp $CURDIR/../superbuild/third_party/bin/server kaho.app/Contents/MacOS
codesign --deep --force --verify --verbose --timestamp --options runtime --sign "Developer ID Application: Deepankar Sharma (VTN5FB5JF7)" kaho.app
create-dmg --codesign "Apple Distribution: Deepankar Sharma (VTN5FB5JF7)" --notarize "kaho" kaho.dmg kaho.app 
