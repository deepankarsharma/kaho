#!/bin/bash

# Make sure you change Project version in CMakeLists.txt when you bump this
# For more details refer to bumping_versions playbook
export KAHO_VERSION="1.2.0"

CURDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

function kaho_base_dir() {
    echo "${CURDIR}/../"
}

function kaho_art_dir() {
    echo "${CURDIR}/../art/"
}

function kaho_mac_installer_art_dir() {
    echo "${CURDIR}/../art/mac_installer/"
}

function kaho_mac_third_party_dir() {
    echo "/opt/kaho"
}

function kaho_mac_third_party_bin_dir() {
    echo "/opt/kaho/bin"
}

function kaho_mac_releases_dir() {
    echo "/opt/kaho/mac_releases"
}

function ensure_dir() {
    local dir="$1"
    if [ ! -d "$dir" ]; then
        mkdir -p "$dir"
    fi
}