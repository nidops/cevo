# SPDX-License-Identifier: Apache-2.0

.PHONY: venv test lint format check

all: venv

venv:
	@echo "🐍 Ensuring Python virtual environment..."
	@./ensure_venv.sh

test: venv
	@echo "🧪 Running Python unit tests..."
	@./run_tests.sh

lint: venv
	@echo "🔍 Running linters: ruff + flake8..."
	@ruff check .
	@flake8 .

format: venv
	@echo "🎨 Formatting Python code with black..."
	@black .

check: lint test
