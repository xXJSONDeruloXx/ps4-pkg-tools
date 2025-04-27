#!/bin/bash
set -e

echo "====================== PS4 PKG Tools Unified Build Script ======================"
echo "This script will build both macOS and Linux binaries"

# Create output directories
mkdir -p build linux-build

echo ""
echo "===== PHASE 1: Building macOS Native Binary ====="

# Build macOS binary
echo "Building PS4 PKG Tools for macOS..."
(
    cd build
    cmake -DBUILD_PKG_TOOL=ON .. 
    make -j$(sysctl -n hw.ncpu)
)

if [ -f "build/ps4-pkg-tool" ]; then
    echo "✅ macOS build complete! Binary is at: build/ps4-pkg-tool"
    # Copy to a consistent location
    cp build/ps4-pkg-tool build/ps4-pkg-tool-macos
    chmod +x build/ps4-pkg-tool-macos
else
    echo "❌ macOS build failed."
    exit 1
fi

echo ""
echo "===== PHASE 2: Building Linux Binary via Docker ====="

# Build Linux binary using Docker - following the same approach as in build-linux.sh
echo "Building PS4 PKG Tools for Linux using Docker..."

# Run the build process directly in a pre-built Ubuntu image
docker run --rm -v "$(pwd):/app" ubuntu:22.04 bash -c '
    # Update and install required dependencies
    apt-get update && apt-get install -y \
        build-essential \
        cmake \
        git \
        libfmt-dev \
        libcrypto++-dev \
        zlib1g-dev

    # Navigate to the app directory
    cd /app

    # Configure the project - using same structure as build-linux.sh
    mkdir -p build
    cd build
    cmake -DBUILD_PKG_TOOL=ON ..
    
    # Build the project
    make -j$(nproc)
    
    # Verify build is successful
    if [ -f "ps4-pkg-tool" ]; then
        echo "Build successful! Binary is at: /app/build/ps4-pkg-tool"
        # Copy to linux-build directory to preserve the binary
        cp ps4-pkg-tool /app/linux-build/
        # Also copy to a consistent location
        cp ps4-pkg-tool /app/build/ps4-pkg-tool-linux
    else
        echo "Build failed. Binary not found."
        exit 1
    fi
'

if [ -f "linux-build/ps4-pkg-tool" ]; then
    echo "✅ Linux build complete! Binary is at: linux-build/ps4-pkg-tool"
    chmod +x linux-build/ps4-pkg-tool
    chmod +x build/ps4-pkg-tool-linux
else
    echo "❌ Linux build failed. Check the logs above for errors."
    exit 1
fi

echo ""
echo "===== Build Summary ====="
echo "✅ macOS binary: build/ps4-pkg-tool-macos"
echo "✅ Linux binary: build/ps4-pkg-tool-linux"
echo ""
echo "You can also find the binaries at their original locations:"
echo "- macOS: build/ps4-pkg-tool"
echo "- Linux: linux-build/ps4-pkg-tool"
echo ""
echo "Build completed successfully!"