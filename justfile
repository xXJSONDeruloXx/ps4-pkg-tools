# Default recipe to run when `just` is called without arguments
default:
    @just --list

# Set up the repository with all dependencies
setup-repo:
    git submodule update --init --recursive
    mkdir -p build && cd build && cmake ..

# Build the project
build:
    cd build && make

# One command to set up and build
all: setup-repo build
    @echo "Build completed successfully!"

# Clean build directory
clean:
    rm -rf build

# Reset and clean everything
clean-slate:
    rm -rf externals/*/
    rm -rf build
    git submodule deinit -f --all || true

# Rebuild from scratch
rebuild: clean all

# Extract a PKG file
extract PKG_FILE OUTPUT_DIR:
    ./build/ps4-pkg-tool "{{PKG_FILE}}" "{{OUTPUT_DIR}}"