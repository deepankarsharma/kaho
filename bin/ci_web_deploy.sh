#!/bin/bash

set -e 

CURDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

. "$CURDIR/base.sh"

sed "s/{KAHO_VERSION}/$KAHO_VERSION/g" $CURDIR/../web/index.html.in
