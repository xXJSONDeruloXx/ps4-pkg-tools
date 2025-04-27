#!/bin/bash
set -e

echo "Building PS4 PKG Tools for Linux using Docker..."

# Create build and output directories if they don't exist
mkdir -p build linux-build

# Run the build process directly in a pre-built Ubuntu image
# This avoids using docker build which is causing permission issues
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

    # Configure the project
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
    else
        echo "Build failed. Binary not found."
        exit 1
    fi
'

if [ -f "linux-build/ps4-pkg-tool" ]; then
    echo "✅ Cross-compilation complete! Linux binary is at: linux-build/ps4-pkg-tool"
    
    # Make the binary executable
    chmod +x linux-build/ps4-pkg-tool
    
    echo "To test on a Linux system, copy this binary and run it."
else
    echo "❌ Cross-compilation failed. Check the logs above for errors."
    exit 1
fi