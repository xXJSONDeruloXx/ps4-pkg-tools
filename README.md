# PS4 PKG Tool

A lightweight command-line utility for extracting and decrypting PlayStation 4 PKG files.

## Overview

ps4-pkg-tool is a CLI (Command Line Interface) utility that provides the ability to:

- Extract PKG files (.pkg) used by the PlayStation 4
- Decrypt the PKG content (game files, sce_sys directory, etc.)
- Reconstruct the complete file hierarchy
- Process PFS (PlayStation File System) images within the PKG
- Batch process multiple PKG files with a single command

This tool uses the core PKG extraction and decryption functionality from the shadPS4 emulator project, stripped down into a simple command-line utility.

## Usage

```bash
# For a single PKG file
ps4-pkg-tool <path/to/pkg> [path/to/output]

# For batch processing a directory of PKG files
ps4-pkg-tool --dir <directory/with/pkgs> [path/to/output]
```

### Examples

```bash
# Extract a game to the Desktop (explicit output path)
ps4-pkg-tool /path/to/Game-CUSAXXXXX.pkg ~/Desktop/GameExtracted

# Extract a DLC package (with automatic output path in same directory as PKG)
ps4-pkg-tool /path/to/DLC-CUSAXXXXX.pkg

# Extract all PKG files in a directory and its subdirectories
ps4-pkg-tool --dir ~/PS4Games ~/Extracted/AllGames

# Extract all PKG files in a directory using that directory for output
ps4-pkg-tool --dir ~/PS4Games
```

## Building from Source

### Prerequisites

- CMake (3.24 or later)
- C++23 compatible compiler (GCC 12+, Clang 15+)
- Dependencies:
  - zlib
  - cryptopp (Crypto++ library)
  - fmt

### Build Instructions

#### GitHub Actions Automated Builds

This repository uses GitHub Actions to automatically build the tool for macOS and Linux environments. You can:

1. Download the latest prebuilt binaries from the Actions tab (available after a successful workflow run)
2. Fork the repository and use the workflows in your own repository
3. Manually trigger the workflows from the Actions tab using the "workflow_dispatch" event

The available workflows are:
- `build.yml`: Builds both macOS and Linux binaries on GitHub's runners
- `docker-build.yml`: Builds a Linux binary using Docker

#### macOS

```bash
# Install dependencies with Homebrew
brew install cmake

# Clone repository with submodules
git clone --recursive https://github.com/xXJSONDeruloXx/ps4-pkg-tools.git
cd ps4-pkg-tools

# OR
git clone https://github.com/xXJSONDeruloXx/ps4-pkg-tools.git
git submodule update --init --recursive

# Configure and build
mkdir build && cd build
cmake -DCMAKE_CXX_STANDARD=23 ..
make
```

#### Linux

There are two ways to build on Linux: directly or using Docker.

##### Direct Build (on Linux systems)

```bash
# Install dependencies
sudo apt install cmake libfmt-dev libcrypto++-dev zlib1g-dev

# For Ubuntu/Debian, install a newer GCC for C++23 support
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install -y gcc-13 g++-13
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100

# Clone repository with submodules
git clone --recursive https://github.com/xXJSONDeruloXx/ps4-pkg-tools.git
cd ps4-pkg-tools

# Configure and build
mkdir build && cd build
cmake -DBUILD_PKG_TOOL=ON -DCMAKE_CXX_STANDARD=23 ..
make
```

##### Cross-compilation with Docker (from macOS or Linux)

```bash
# Clone repository with submodules
git clone --recursive https://github.com/xXJSONDeruloXx/ps4-pkg-tools.git
cd ps4-pkg-tools

# Option 1: Build only Linux binary
./build-linux.sh

# Option 2: Build both macOS and Linux binaries in one command
./build.sh
```

The resulting Linux binary will be placed in `linux-build/ps4-pkg-tool` and can be run on most modern Linux distributions.

## Technical Details

### PKG File Format

PlayStation 4 PKG files are encrypted containers that hold:
- Core executable content
- Game data and assets
- System metadata
- PFS (PlayStation File System) image

### Extraction Process

1. Parse the PKG header to validate the file structure
2. Decrypt the PKG table entries using RSA2048 and the derived key
3. Recover the PFS encryption key (EKPFS)
4. Generate data and tweak keys for AES-XTS decryption
5. Decrypt the PFS image and extract the compressed filesystem (PFSC)
6. Reconstruct the complete file tree and paths
7. Write decrypted files to the target location

### Components

- `PKG` class: Central PKG processing and extraction
- `Crypto` class: RSA, AES and other cryptographic operations
- `TRP` class: Trophy file processing

## License

This tool is released under the GPL-2.0-or-later license, the same license as the shadPS4 project.

## Credits

- Based on the file format handling code from the shadPS4 emulator project
- Thanks to all researchers and developers who reverse-engineered the PS4 PKG format

## Disclaimer

This tool is intended for legal use with your own purchased content. We do not condone piracy.