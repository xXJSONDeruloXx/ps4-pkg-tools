#!/bin/bash
set -e

echo "====================== PS4 PKG Tools macOS Build ======================"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

# Detect macOS architecture
MACOS_ARCH="$(uname -m)"
echo "Building natively for macOS (${MACOS_ARCH})"

mkdir -p "${BUILD_DIR}"

echo ""
echo "===== Configuring ====="
cd "${BUILD_DIR}"
cmake -DCMAKE_BUILD_TYPE=Release \
	-DBUILD_PKG_QT_GUI=ON \
	-DCMAKE_OSX_ARCHITECTURES="${MACOS_ARCH}" \
	"${SCRIPT_DIR}"

echo ""
echo "===== Building ====="
make -j$(sysctl -n hw.ncpu)

echo ""
echo "===== Build Summary ====="
if [ -f "${BUILD_DIR}/ps4-pkg-tool" ]; then
	echo "✅ CLI binary: ${BUILD_DIR}/ps4-pkg-tool"
	file "${BUILD_DIR}/ps4-pkg-tool"
else
	echo "❌ CLI binary not found"
	exit 1
fi

if [ -f "${BUILD_DIR}/ps4-pkg-tool-gui" ]; then
	echo "✅ GUI binary: ${BUILD_DIR}/ps4-pkg-tool-gui"
	file "${BUILD_DIR}/ps4-pkg-tool-gui"
else
	echo "⚠️  GUI binary not found (Qt6 may not be installed)"
fi

echo ""
echo "macOS build complete!"
