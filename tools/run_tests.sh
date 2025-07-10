#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
# run_tests.sh: Ensure .venv, run formatting, linting, and tests

set -eo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
source "$SCRIPT_DIR/ensure_venv.sh"
PROJECT_ROOT="$SCRIPT_DIR"

print_header() {
  echo -e "\033[1;34m$1\033[0m"
}

run_black() {
  print_header "🧹 Checking code style with black..."
  black --check "$PROJECT_ROOT"
}

run_ruff() {
  print_header "🔎 Linting with ruff..."
  ruff check "$PROJECT_ROOT"
}

run_flake8() {
  print_header "🐍 (Optional) Linting with flake8..."
  flake8 "$PROJECT_ROOT"
}

run_tests() {
  print_header "🧪 Running tests..."
  if [[ -d "$PROJECT_ROOT/tests" ]]; then
    pytest "$PROJECT_ROOT/tests" "$@"
  else
    echo "⚠️  No tests directory found."
  fi
}

main() {
  run_black
  run_ruff

  # Uncomment if needed
  # run_flake8

  run_tests "$@"

  echo -e "\033[1;32m✅ All checks passed\033[0m"
}

main "$@"

