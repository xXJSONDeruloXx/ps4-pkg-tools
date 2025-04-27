FROM ubuntu:22.04

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libfmt-dev \
    libcrypto++-dev \
    zlib1g-dev

# Set working directory
WORKDIR /app

# Create build directory with proper permissions
RUN mkdir -p build && chmod 777 build

# We will mount the source code at runtime
CMD cd build && \
    cmake -DBUILD_PKG_TOOL=ON .. && \
    make -j$(nproc) && \
    echo "Build complete! Binary is at: /app/build/ps4-pkg-tool"