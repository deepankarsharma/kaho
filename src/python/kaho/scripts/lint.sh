#!/bin/bash
set -euxo pipefail

poetry run cruft check
poetry run mypy --ignore-missing-imports kaho/
poetry run isort --check --diff kaho/ tests/
poetry run black --check kaho/ tests/
poetry run flake8 kaho/ tests/
poetry run safety check -i 39462 -i 40291
poetry run bandit -r kaho/
