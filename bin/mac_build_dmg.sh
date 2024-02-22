#!/bin/bash

CURDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. "$CURDIR/base.sh"

create-dmg --codesign "Apple Distribution: Deepankar Sharma (VTN5FB5JF7)" \
    --volname "kaho" \
    --volicon "$(kaho_mac_installer_art_dir)/logo.icns" \
    --background "$(kaho_mac_installer_art_dir)/background.png" \
    --window-pos 200 120 \
    --window-size 400 400 \
    --icon-size 100 \
    --icon "kaho.app" 200 190 \
    --hide-extension "kaho.app" \
    --app-drop-link 600 185 \
    kaho-${KAHO_VERSION}.dmg kaho.app

ensure_dir "$(kaho_mac_releases_dir)"
cp "kaho-${KAHO_VERSION}.dmg" "$(kaho_mac_releases_dir)/"

