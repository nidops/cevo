#!/bin/bash

# SPDX-License-Identifier: Apache-2.0

set -e

BUILD_TYPE=${BUILD_TYPE:-Test}
BUILD_DIR="build"

if [[ "$BUILD_TYPE" == "Test" ]]; then
    BUILD_TESTS=ON
else
    BUILD_TESTS=OFF
fi

function configure() {
    echo -e "\033[1;36m🛠️  [BUILD] Configuring project — Type: \033[1;33m$BUILD_TYPE\033[1;36m | Unit-tests: \033[1;32m$BUILD_TESTS\033[0m"
    cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DBUILD_TESTS=$BUILD_TESTS -DCMAKE_C_FLAGS="-g -O0"
}

function generate() {
    echo "🐍 Generating auto-generated API sources..."
    cmake --build "$BUILD_DIR" --target generate_apis
}

function build() {
    echo "🔨 Building project..."
    cmake --build "$BUILD_DIR" -- -j$(nproc)
}

if [[ "$1" == "configure" ]]; then
    configure
    echo "📁 Configuration complete. You can now run 'make -C $BUILD_DIR'"
    exit 0
fi

if [[ "$1" == "generate" ]]; then
    if [ ! -d "$BUILD_DIR" ]; then
        echo "Build directory not found. Running configure first."
        configure
    fi
    generate
    echo "✅ Generation complete."
    exit 0
fi

if [[ "$1" == "clean" ]]; then
    echo "🧹 Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    echo "✅ Clean complete."
    exit 0
fi

# Default behavior: configure, generate, re-configure, build
if [ ! -d "$BUILD_DIR" ]; then
    configure
fi

generate

build

echo "✅ Build complete."
