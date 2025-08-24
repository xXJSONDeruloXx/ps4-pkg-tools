# PS4 PKG Tools

A tool for extracting PlayStation 4 PKG files with both command-line and graphical interfaces.

## Features

- **CLI Tool** (`ps4-pkg-tool`): Command-line extraction with support for single files and batch directory processing
- **GUI Tool** (`ps4-pkg-tool-gui`): Minimal Qt-based graphical interface for easy file/directory selection

## Building

### Prerequisites

- CMake 3.24+
- C++23 compatible compiler
- Qt6 Widgets (optional, for GUI)

### Arch Linux / CachyOS

```bash
sudo pacman -S cmake make gcc qt6-base
```

### Build Steps

```bash
# Initialize submodules
git submodule update --init --recursive

# Configure and build
cmake -S . -B build
cmake --build build -j$(nproc)
```

### Build Options

- `BUILD_PKG_QT_GUI=ON/OFF` - Enable/disable Qt GUI build (default: ON if Qt6 found)

## Usage

### Command Line

```bash
# Extract single PKG file
./build/ps4-pkg-tool game.pkg [output_directory]

# Extract all PKG files in directory
./build/ps4-pkg-tool --dir /path/to/pkgs [output_directory]

# Show help
./build/ps4-pkg-tool --help
```

### GUI

```bash
./build/ps4-pkg-tool-gui
```

The GUI provides:
- File picker for individual PKG files
- Directory browser for batch processing
- Optional output directory selection
- Real-time metadata display (Title ID, size, flags, file count)
- Progress bar for extraction
- Sequential processing of multiple PKG files

## Output Structure

PKG files are extracted to subdirectories named after their Title ID:
```
output_directory/
  ├── CUSA12345/
  │   ├── extracted files...
  └── CUSA67890/
      ├── extracted files...
```

## License

GPL-2.0-or-later
