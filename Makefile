# PS4 PKG Tools Makefile
# This Makefile provides convenient targets for building the PS4 PKG tools

# Default build directory
BUILD_DIR := build
CMAKE_BUILD_TYPE ?= Release

# Detect number of CPU cores for parallel compilation
NPROCS := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Default target
.PHONY: all
all: setup build

# Help target
.PHONY: help
help:
	@echo "PS4 PKG Tools - Available Make targets:"
	@echo ""
	@echo "  all          - Setup dependencies and build everything (default)"
	@echo "  setup        - Initialize git submodules and configure CMake"
	@echo "  build        - Build all targets (CLI and GUI)"
	@echo "  cli          - Build only the CLI tool"
	@echo "  gui          - Build only the GUI tool"
	@echo "  clean        - Clean build artifacts"
	@echo "  distclean    - Remove build directory completely"
	@echo "  reset        - Reset submodules and rebuild from scratch"
	@echo "  install      - Install binaries to system"
	@echo "  test         - Run tests (if available)"
	@echo "  debug        - Build with debug symbols"
	@echo "  release      - Build optimized release version"
	@echo ""
	@echo "Variables:"
	@echo "  BUILD_DIR          = $(BUILD_DIR)"
	@echo "  CMAKE_BUILD_TYPE   = $(CMAKE_BUILD_TYPE)"
	@echo "  NPROCS             = $(NPROCS)"

# Setup: initialize submodules and configure CMake
.PHONY: setup
setup: $(BUILD_DIR)/Makefile

$(BUILD_DIR)/Makefile:
	@echo "=== Setting up PS4 PKG Tools ==="
	@echo "Initializing git submodules..."
	git submodule update --init --recursive
	@echo "Creating build directory..."
	mkdir -p $(BUILD_DIR)
	@echo "Configuring CMake..."
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) \
		-DBUILD_PKG_QT_GUI=ON \
		..

# Build all targets
.PHONY: build
build: $(BUILD_DIR)/Makefile
	@echo "=== Building PS4 PKG Tools ==="
	cd $(BUILD_DIR) && $(MAKE) -j$(NPROCS)
	@echo "✅ Build complete!"
	@echo "   CLI tool: $(BUILD_DIR)/ps4-pkg-tool"
	@echo "   GUI tool: $(BUILD_DIR)/ps4-pkg-tool-gui"

# Build only CLI tool
.PHONY: cli
cli: $(BUILD_DIR)/Makefile
	@echo "=== Building CLI tool ==="
	cd $(BUILD_DIR) && $(MAKE) -j$(NPROCS) ps4-pkg-tool
	@echo "✅ CLI build complete: $(BUILD_DIR)/ps4-pkg-tool"

# Build only GUI tool
.PHONY: gui
gui: $(BUILD_DIR)/Makefile
	@echo "=== Building GUI tool ==="
	cd $(BUILD_DIR) && $(MAKE) -j$(NPROCS) ps4-pkg-tool-gui
	@echo "✅ GUI build complete: $(BUILD_DIR)/ps4-pkg-tool-gui"

# Debug build
.PHONY: debug
debug:
	@$(MAKE) CMAKE_BUILD_TYPE=Debug all

# Release build
.PHONY: release
release:
	@$(MAKE) CMAKE_BUILD_TYPE=Release all

# Clean build artifacts
.PHONY: clean
clean:
	@echo "=== Cleaning build artifacts ==="
	@if [ -d "$(BUILD_DIR)" ]; then \
		cd $(BUILD_DIR) && $(MAKE) clean 2>/dev/null || true; \
	fi
	@echo "✅ Clean complete"

# Remove build directory completely
.PHONY: distclean
distclean:
	@echo "=== Removing build directory ==="
	rm -rf $(BUILD_DIR)
	@echo "✅ Build directory removed"

# Reset everything and rebuild from scratch
.PHONY: reset
reset: distclean
	@echo "=== Resetting submodules ==="
	git submodule deinit -f --all || true
	git submodule update --init --recursive
	@$(MAKE) all

# Install binaries to system
.PHONY: install
install: build
	@echo "=== Installing PS4 PKG Tools ==="
	cd $(BUILD_DIR) && $(MAKE) install
	@echo "✅ Installation complete"

# Run tests (if available)
.PHONY: test
test: build
	@echo "=== Running tests ==="
	@if [ -d "$(BUILD_DIR)" ]; then \
		cd $(BUILD_DIR) && $(MAKE) test 2>/dev/null || echo "No tests configured"; \
	else \
		echo "Build directory not found. Run 'make build' first."; \
	fi

# Run the GUI tool
.PHONY: run-gui
run-gui: gui
	@echo "=== Running GUI tool ==="
	cd $(BUILD_DIR) && ./ps4-pkg-tool-gui

# Run the CLI tool with help
.PHONY: run-cli
run-cli: cli
	@echo "=== Running CLI tool ==="
	cd $(BUILD_DIR) && ./ps4-pkg-tool --help

# Check if we can build Qt GUI
.PHONY: check-qt
check-qt:
	@echo "=== Checking Qt6 availability ==="
	@if pkg-config --exists Qt6Widgets 2>/dev/null; then \
		echo "✅ Qt6 Widgets found - GUI can be built"; \
		pkg-config --modversion Qt6Widgets; \
	elif command -v qmake >/dev/null 2>&1; then \
		echo "✅ Qt found via qmake - GUI can be built"; \
		qmake -version; \
	else \
		echo "❌ Qt6 not found - only CLI will be available"; \
		echo "Install Qt6 development packages to build the GUI"; \
	fi

# Show current status
.PHONY: status
status:
	@echo "=== PS4 PKG Tools Status ==="
	@echo "Build directory: $(BUILD_DIR)"
	@echo "Build type: $(CMAKE_BUILD_TYPE)"
	@echo "CPU cores: $(NPROCS)"
	@echo ""
	@if [ -d "$(BUILD_DIR)" ]; then \
		echo "✅ Build directory exists"; \
		if [ -f "$(BUILD_DIR)/ps4-pkg-tool" ]; then \
			echo "✅ CLI tool built"; \
		else \
			echo "❌ CLI tool not built"; \
		fi; \
		if [ -f "$(BUILD_DIR)/ps4-pkg-tool-gui" ]; then \
			echo "✅ GUI tool built"; \
		else \
			echo "❌ GUI tool not built"; \
		fi; \
	else \
		echo "❌ Build directory not found"; \
	fi

# Development helpers
.PHONY: format
format:
	@echo "=== Formatting code ==="
	@find src -name "*.cpp" -o -name "*.h" | xargs clang-format -i 2>/dev/null || \
		echo "clang-format not available"

# Quick rebuild (without cleaning submodules)
.PHONY: rebuild
rebuild: clean build

# Package binaries
.PHONY: package
package: build
	@echo "=== Creating package ==="
	mkdir -p dist
	@if [ -f "$(BUILD_DIR)/ps4-pkg-tool" ]; then \
		cp $(BUILD_DIR)/ps4-pkg-tool dist/; \
		echo "✅ CLI tool packaged"; \
	fi
	@if [ -f "$(BUILD_DIR)/ps4-pkg-tool-gui" ]; then \
		cp $(BUILD_DIR)/ps4-pkg-tool-gui dist/; \
		echo "✅ GUI tool packaged"; \
	fi
	@echo "Package created in dist/"

.PHONY: clean-package
clean-package:
	rm -rf dist
