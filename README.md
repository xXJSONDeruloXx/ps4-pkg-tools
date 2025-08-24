# PS4 PKG Tool

A lightweight command-line utility and GUI application for extracting and decrypting PlayStation 4 PKG files.

## Overview

ps4-pkg-tool is both a CLI (Command Line Interface) and GUI utility that provides the ability to:

- Extract PKG files (.pkg) used by the PlayStation 4
- Decrypt the PKG content (game files, sce_sys directory, etc.)
- Reconstruct the complete file hierarchy
- Process PFS (PlayStation File System) images within the PKG
- Batch process multiple PKG files with a single command
- User-friendly drag & drop GUI interface with toggle-based output directory control

This tool uses the core PKG extraction and decryption functionality from the shadPS4 emulator project, stripped down into simple command-line and GUI utilities.

## Usage

### GUI Application

The GUI provides an intuitive interface for PKG extraction:

```bash
# Launch the GUI
ps4-pkg-tool-gui
```

**GUI Features:**
- **Drag & Drop**: Simply drag PKG files onto the window to start extraction
- **Output Directory Toggle**: Easily control where files are extracted
  - Toggle ON (default): Extract to same folder as PKG file
  - Toggle OFF: Choose custom output directory
- **Single File Mode**: Select individual PKG files for extraction
- **Batch Mode**: Process entire directories of PKG files
- **Progress Tracking**: Real-time extraction progress display

### Command Line Interface

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
- C++23 compatible compiler (GCC 12+, Clang 15+, MSVC 2022+)
- Git (for cloning submodules)
- Qt6 (optional, for GUI support)

**Bundled Dependencies** (included as git submodules):
- zlib - Compression library
- cryptopp (Crypto++ library) - Cryptographic functions
- fmt - Modern C++ formatting library
- toml11 - TOML parser for configuration files
- tracy - Profiling library (included but not actively used)

### Build Instructions

#### Quick Start with Makefile

The easiest way to build the project is using the provided Makefile:

```bash
# Clone repository with submodules
git clone --recursive https://github.com/xXJSONDeruloXx/ps4-pkg-tools.git
cd ps4-pkg-tools

# Build everything (CLI + GUI)
make

# Or build specific components
make cli      # Build only command-line tool
make gui      # Build only GUI application
make help     # Show all available options
```

**Makefile Commands:**
- `make` or `make all` - Setup and build everything
- `make setup` - Initialize submodules and configure CMake
- `make build` - Build both CLI and GUI tools
- `make cli` - Build only the command-line tool
- `make gui` - Build only the GUI tool
- `make clean` - Clean build artifacts
- `make rebuild` - Quick rebuild
- `make reset` - Reset everything and rebuild from scratch
- `make run-gui` - Build and run the GUI
- `make status` - Show current build status
- `make package` - Create distribution package

#### Manual CMake Build

#### GitHub Actions Automated Builds

This repository uses GitHub Actions to automatically build the tool for macOS and Linux environments. You can:

1. Download the latest prebuilt binaries from the Actions tab (available after a successful workflow run)
2. Fork the repository and use the workflows in your own repository
3. Manually trigger the workflows from the Actions tab using the "workflow_dispatch" event

The available workflows are:
- `build.yml`: Builds both macOS and Linux binaries on GitHub's runners
- `docker-build.yml`: Builds a Linux binary using Docker

#### macOS

**Quick Method (Recommended):**
```bash
# Install dependencies
brew install cmake qt@6  # Qt6 optional for GUI

# Clone and build
git clone --recursive https://github.com/xXJSONDeruloXx/ps4-pkg-tools.git
cd ps4-pkg-tools
make
```

**Manual Method:**
```bash
# Install build dependencies with Homebrew
brew install cmake

# Optional: Install Qt6 for GUI support
brew install qt@6

# Clone repository with submodules
git clone --recursive https://github.com/xXJSONDeruloXx/ps4-pkg-tools.git
cd ps4-pkg-tools

# Initialize submodules (if not cloned recursively)
git submodule update --init --recursive

# Create and configure build directory
mkdir build && cd build
cmake ..

# Build the project (using all available CPU cores)
cmake --build . -j$(sysctl -n hw.ncpu)

# Optional: Install to local user directory
cmake --install . --prefix ~/.local

# Test the built tools
./ps4-pkg-tool --help
./ps4-pkg-tool-gui  # If Qt6 was available
```

**Note**: This build uses the bundled dependencies (zlib, cryptopp, fmt) from the git submodules, so you don't need to install them separately with Homebrew.

#### Linux

**Quick Method (Recommended):**
```bash
# Install dependencies
sudo apt install cmake build-essential qt6-base-dev  # Qt6 optional for GUI

# Clone and build
git clone --recursive https://github.com/xXJSONDeruloXx/ps4-pkg-tools.git
cd ps4-pkg-tools
make
```

There are also additional ways to build on Linux: directly or using Docker.

##### Direct Build (on Linux systems)

**Manual Method:**
```bash
# Install build dependencies
sudo apt install cmake build-essential git

# For Ubuntu/Debian, install a newer GCC for C++23 support (if needed)
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install -y gcc-13 g++-13
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100

# Optional: Install Qt6 for GUI support
sudo apt install qt6-base-dev

# Clone repository with submodules
git clone --recursive https://github.com/xXJSONDeruloXx/ps4-pkg-tools.git
cd ps4-pkg-tools

# Initialize submodules (if not cloned recursively)
git submodule update --init --recursive

# Create and configure build directory
mkdir build && cd build
cmake ..

# Build the project (using all available CPU cores)
cmake --build . -j$(nproc)

# Optional: Install to local user directory
cmake --install . --prefix ~/.local

# Test the built tools
./ps4-pkg-tool --help
./ps4-pkg-tool-gui  # If Qt6 was available
```

**Note**: This build uses the bundled dependencies (zlib, cryptopp, fmt) from the git submodules, so you don't need to install them separately. The submodules ensure compatible versions and avoid potential conflicts with system packages.

#### Build Options and Targets

The CMake build system provides several options and targets:

**Build Targets:**
- `ps4-pkg-tool` - The main CLI tool (always built)
- `ps4-pkg-tool-gui` - Optional Qt6-based GUI (built if Qt6 is found)
- `ps4_pkg_tool_core` - Static library containing the core extraction logic

**CMake Options:**
- `BUILD_PKG_QT_GUI=ON/OFF` - Enable/disable Qt GUI build (default: ON)

**Build Configurations:**
```bash
# Debug build (default)
cmake ..

# Release build with optimizations
cmake -DCMAKE_BUILD_TYPE=Release ..

# Disable GUI build
cmake -DBUILD_PKG_QT_GUI=OFF ..

# Custom install prefix
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
```

**Useful Make Commands:**
```bash
# Check if Qt6 is available for GUI building
make check-qt

# Build with debug symbols
make debug

# Build optimized release version
make release

# Clean and rebuild
make rebuild

# Show project status
make status

# Run the applications
make run-gui
make run-cli
```

**Useful CMake Commands:**
```bash
# Clean build directory
cmake --build . --target clean

# Build specific target only
cmake --build . --target ps4-pkg-tool

# Install with verbose output
cmake --install . --verbose

# Show build system information
cmake .. -LAH
```

##### Alternative: Using Convenience Build Scripts

The repository includes convenience scripts for automated builds:

```bash
# Clone repository with submodules
git clone --recursive https://github.com/xXJSONDeruloXx/ps4-pkg-tools.git
cd ps4-pkg-tools

# Option 1: Use the Makefile (recommended)
make

# Option 2: Build Linux binary using Docker (cross-platform)
./build-linux.sh

# Option 3: Build both macOS and Linux binaries (macOS only)
./build.sh
```

**Makefile**: Provides convenient targets for common build tasks and automatically detects system capabilities.

**build-linux.sh**: Uses Docker to create a Linux binary on any platform that supports Docker. The resulting binary will be in `linux-build/ps4-pkg-tool`.

**build.sh**: macOS-specific script that builds both native macOS and Linux (via Docker) binaries in one command.

#### Troubleshooting Build Issues

**Common Build Problems:**

1. **Submodules not initialized**: If you get errors about missing dependencies:
   ```bash
   git submodule update --init --recursive
   ```

2. **C++23 compiler not found**: Update your compiler or use a newer version:
   ```bash
   # Ubuntu/Debian
   sudo apt install gcc-13 g++-13
   
   # macOS
   xcode-select --install
   ```

3. **CMake version too old**: Update CMake to 3.24 or later:
   ```bash
   # Ubuntu/Debian
   sudo apt remove cmake
   sudo snap install cmake --classic
   
   # macOS
   brew upgrade cmake
   ```

4. **Qt6 not found** (affects GUI build only):
   ```bash
   # Check Qt6 availability
   make check-qt
   
   # Disable GUI build
   cmake -DBUILD_PKG_QT_GUI=OFF ..
   
   # Or install Qt6
   sudo apt install qt6-base-dev  # Linux
   brew install qt@6              # macOS
   ```

5. **Permission errors with Docker builds**: Ensure Docker is running and you have permissions:
   ```bash
   sudo usermod -aG docker $USER
   # Log out and back in
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