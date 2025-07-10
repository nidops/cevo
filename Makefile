# SPDX-License-Identifier: Apache-2.0

.PHONY: all clean build run_tests configure help check_env

BUILD_DIR := build
BUILD_SCRIPT := ./build.sh

all: check_env build

check_env:
	@$(MAKE) -C tools

build:
	@echo "🔧 Building project..."
	$(BUILD_SCRIPT)

clean:
	@echo "🧹 Cleaning build directory..."
	@if [ -d "$(BUILD_DIR)" ]; then rm -rf $(BUILD_DIR); else echo "Build directory does not exist"; fi

run_tests:
	@echo "🚀 Running unit tests..."
	@cd $(BUILD_DIR) && ctest --output-on-failure || { \
		echo '❌ Some tests failed. Check above for details.'; \
		exit 1; \
	}
	@echo "✅ All tests passed successfully!"

configure:
	@echo "🛠️   Running CMake configuration only..."
	$(BUILD_SCRIPT) configure

generate:
	@echo "🛠️   Generating apis only..."
	$(BUILD_SCRIPT) generate

help:
	@echo "📦 Available targets:"
	@echo "  make                → Build the project (Test by default)"
	@echo "  make configure      → Run CMake configuration only (no build)"
	@echo "  make clean          → Remove build directory"
	@echo "  make run_tests      → Run unit tests via CTest"
	@echo ""
	@echo "🔧 Advanced:"
	@echo "  BUILD_TYPE=Release make         → Build in Release mode"
	@echo "  ./build.sh configure            → Only configure (manual build via make -C build)"
	@echo "  make -C build                   → Manually build after configuration"
