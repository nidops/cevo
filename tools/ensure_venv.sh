#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
# ensure_venv.sh : Ensures .venv exists in the project root folder and installs required tools

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
VENV="$ROOT_DIR/.venv"

# Check if python3 exists
if ! command -v python3 &>/dev/null; then
  echo "❌ python3 not found in PATH. Please install Python 3."
  exit 1
fi

# Step 1: Create venv if missing
if [ ! -d "$VENV" ]; then
  echo "🔧 Creating virtual environment in project root..."
  python3 -m venv "$VENV"
fi

# Validate venv
if [ ! -f "$VENV/bin/activate" ]; then
  echo "❌ Failed to create a valid virtual environment."
  exit 1
fi

# Step 2: Activate venv
source "$VENV/bin/activate"

# Step 3: Upgrade pip quietly but warn if it fails
echo "⬆️  Ensuring latest pip..."
if ! pip install --upgrade pip >/dev/null 2>&1; then
  echo "⚠️ pip upgrade failed, continuing anyway"
fi

# Step 4: Ensure key packages are installed
REQUIRED_PKGS=(ruamel.yaml pytest black ruff flake8)
for pkg in "${REQUIRED_PKGS[@]}"; do
  if ! pip show "$pkg" &>/dev/null; then
    echo "📦 Installing $pkg..."
    pip install "$pkg" >/dev/null || {
      echo "❌ Failed to install $pkg"
      exit 1
    }
  else
    echo "✅ $pkg already installed"
  fi
done
