# PS4 PKG Tool

A lightweight command-line utility for extracting and decrypting PlayStation 4 PKG files.

## Overview

ps4-pkg-tool is a CLI (Command Line Interface) utility that provides the ability to:

- Extract PKG files (.pkg) used by the PlayStation 4
- Decrypt the PKG content (game files, sce_sys directory, etc.)
- Reconstruct the complete file hierarchy
- Process PFS (PlayStation File System) images within the PKG

This tool uses the core PKG extraction and decryption functionality from the shadPS4 emulator project, stripped down into a simple command-line utility.

## Usage

```bash
ps4-pkg-tool <path/to/pkg> <path/to/output>
```

### Examples

```bash
# Extract a game to the Desktop
ps4-pkg-tool /path/to/Game-CUSAXXXXX.pkg ~/Desktop/GameExtracted

# Extract a DLC package
ps4-pkg-tool /path/to/DLC-CUSAXXXXX.pkg ~/DLC_Extracted
```

## Building from Source

### Prerequisites

- CMake (3.24 or later)
- C++23 compatible compiler (GCC 12+, Clang 15+, MSVC 2022+)
- Dependencies:
  - zlib
  - cryptopp (Crypto++ library)
  - fmt

### Build Instructions

#### macOS

```bash
# Install dependencies with Homebrew
brew install cmake fmt cryptopp

# Clone repository with submodules
git clone --recursive https://github.com/yourusername/ps4-pkg-tools.git
cd ps4-pkg-tools

# Configure and build
mkdir build && cd build
cmake ..
make
```

#### Linux

```bash
# Install dependencies
sudo apt install cmake libfmt-dev libcrypto++-dev zlib1g-dev

# Clone repository with submodules
git clone --recursive https://github.com/yourusername/ps4-pkg-tools.git
cd ps4-pkg-tools

# Configure and build
mkdir build && cd build
cmake -DBUILD_PKG_TOOL=ON ..
make
```

#### Windows

```bash
# Clone repository with submodules
git clone --recursive https://github.com/yourusername/ps4-pkg-tools.git
cd ps4-pkg-tools

# Configure and build (Visual Studio Developer Command Prompt)
mkdir build && cd build
cmake -DBUILD_PKG_TOOL=ON ..
cmake --build . --config Release
```

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