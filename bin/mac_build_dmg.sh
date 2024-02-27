#!/bin/bash

CURDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. "$CURDIR/base.sh"

# tiffutil -cathidpicheck background.png background@2x.png -out background.tiff

# --codesign "Apple Distribution: Deepankar Sharma (VTN5FB5JF7)"
# --notarize "kaho" \
create-dmg \
    --volname "kaho" \
    --volicon "$(kaho_mac_installer_art_dir)/logo.icns" \
    --background "$(kaho_mac_installer_art_dir)/background.tiff" \
    --window-pos 200 120 \
    --window-size 800 400 \
    --icon-size 100 \
    --icon "kaho.app" 200 190 \
    --hide-extension "kaho.app" \
    --app-drop-link 600 185 \
    kaho-${KAHO_VERSION}.dmg kaho.app

ensure_dir "$(kaho_mac_releases_dir)"
cp "kaho-${KAHO_VERSION}.dmg" "$(kaho_mac_releases_dir)/"
mkdir -p /tmp/dmg
cp "kaho-${KAHO_VERSION}.dmg" "/tmp/dmg/kaho.dmg"