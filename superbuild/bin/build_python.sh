#!/bin/bash

set -eu

# Check if the Python version is provided as an argument
if [ $# -eq 0 ]; then
    echo "Usage: build_python.sh <SB_INSTALL_PREFIX> <PYTHON_VERSION>"
    exit 1
fi

# Set the Python version and download URL
SB_INSTALL_PREFIX=$1
BUILD_TYPE=$2
PYTHON_VERSION=$3


# Check the current system and install dependencies accordingly
if [[ "$(uname)" == "Darwin" ]]; then
    echo "Detected macOS. Installing dependencies using Homebrew..."

    # Update Homebrew
    #brew update
    # Install the required dependencies
    brew install openssl readline sqlite3 xz zlib tcl-tk

    export PYTHON_CONFIGURE_OPTS="--enable-framework"
    pyenv install -s $PYTHON_VERSION

    mkdir -p $SB_INSTALL_PREFIX/python
    cp -a ~/.pyenv/versions/$PYTHON_VERSION/Library/Frameworks/Python.framework $SB_INSTALL_PREFIX/python

elif [[ -f "/etc/debian_version" ]]; then
    if [ ! -d "$HOME/.pyenv" ]; then
        echo "Detected Debian-based system. Installing dependencies using apt..."
        curl https://pyenv.run | bash
        pyenv install -s $PYTHON_VERSION
    fi
elif [[ -f "/etc/redhat-release" || -f "/etc/fedora-release" ]]; then
    echo "Detected RPM-based system. Installing dependencies using dnf or yum..."

    # Check if dnf is available, otherwise use yum
    if command -v dnf &> /dev/null; then
        # Install the build dependencies for Python using dnf
        sudo dnf builddep python$PYTHON_VERSION
    else
        # Install the build dependencies for Python using yum
        sudo yum-builddep python$PYTHON_VERSION
    fi

else
    echo "Unsupported system. Please install the necessary dependencies manually."
    exit 1
fi

