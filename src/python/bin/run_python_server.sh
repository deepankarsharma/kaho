#!/bin/bash

CURDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

. $CURDIR/../kaho/.venv/bin/activate
python $CURDIR/../kaho/kaho/main.py

