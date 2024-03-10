#!/bin/bash
set -euxo pipefail

poetry run isort kaho/ tests/
poetry run black kaho/ tests/
