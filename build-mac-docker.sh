#!/bin/bash
# build-mac-docker.sh - Build ps4-pkg-tools for macOS using Docker

set -e

echo "===== Building macOS Binary via Docker ====="

# Create output directories if they don't exist
mkdir -p build
mkdir -p mac-build

# Run the build in a Docker container with Ubuntu and install osxcross tools
docker run --rm -v "$(pwd):/app" -w /app ubuntu:22.04 bash -c '
    # Update and install dependencies
    apt-get update
    apt-get install -y cmake make git libz-dev libssl-dev gcc g++ clang llvm libtool libxml2-dev uuid-dev

    echo "Building macOS binary..."
    mkdir -p /app/build
    cd /app/build
    
    # Configure with standard flags since we do not have OSXCross tools
    # in this Ubuntu container, we are just building for the host system (Linux)
    # but creating properly labeled output files
    cmake -DBUILD_PKG_TOOL=ON ..
    
    # Build using all available cores
    make -j$(nproc)
    
    # Copy the binary to mac-build directory with explicit name
    mkdir -p /app/mac-build
    cp ps4-pkg-tool /app/mac-build/ps4-pkg-tool-linux
    
    # Create copies with platform naming
    cp ps4-pkg-tool /app/build/ps4-pkg-tool-linux-docker
    
    echo "Build via Docker completed (Note: This is a Linux binary built in Docker)"
    echo "To create a true macOS binary via Docker would require adding OSXCross"
    echo "which needs Apple SDK files that cannot be distributed automatically."
'

echo "===== Build Complete ====="
echo "Binary built in Docker (Linux) available at:"
echo "  - mac-build/ps4-pkg-tool-linux"
echo "  - build/ps4-pkg-tool-linux-docker"
echo ""
echo "Note: This is a Linux binary built inside Docker."
echo "To create a true macOS binary would require installing OSXCross with Apple SDK files."