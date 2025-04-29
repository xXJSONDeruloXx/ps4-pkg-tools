@echo off
setlocal enabledelayedexpansion

echo ====================== PS4 PKG Tools Windows Build Script ======================
echo This script will build PS4 PKG Tools for Windows

:: Check for required tools
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake not found in PATH. Please install CMake and try again.
    exit /b 1
)

where cl >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo WARNING: Visual C++ compiler not found in PATH.
    echo Make sure you run this script from a Visual Studio Developer Command Prompt.
    echo Or run: "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    echo    (adjust path according to your Visual Studio installation)
    
    set /p continue="Continue anyway? (y/n): "
    if /i not "!continue!"=="y" exit /b 1
)

:: Create build directory
if not exist build-windows mkdir build-windows

:: Navigate to build directory
cd build-windows

echo.
echo ===== Configuring PS4 PKG Tools for Windows =====
cmake -G "Visual Studio 17 2022" -A x64 -DBUILD_PKG_TOOL=ON ..

echo.
echo ===== Building PS4 PKG Tools for Windows =====
cmake --build . --config Release

:: Check if the build was successful
if not exist Release\ps4-pkg-tool.exe (
    echo.
    echo ERROR: Build failed. Check the logs above for errors.
    exit /b 1
)

:: Copy the executable to the parent directory for consistency
copy Release\ps4-pkg-tool.exe ps4-pkg-tool-windows.exe

echo.
echo ===== Build Summary =====
echo Build completed successfully!
echo Windows binary: build-windows\Release\ps4-pkg-tool.exe
echo Windows binary (copy): build-windows\ps4-pkg-tool-windows.exe

cd ..
endlocal