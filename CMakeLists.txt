# SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.24)
project(ps4-pkg-tool CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add module path for custom Find scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Include MSVC compatibility fixes on Windows
if(MSVC)
  message(STATUS "Windows build detected - applying MSVC compatibility patches")
  if(EXISTS "${CMAKE_MODULE_PATH}/MSVCCompatibility.cmake")
    include("${CMAKE_MODULE_PATH}/MSVCCompatibility.cmake")
  elseif(EXISTS "${CMAKE_TOOLCHAIN_FILE}")
    include("${CMAKE_TOOLCHAIN_FILE}")
  endif()
  
  # Add Windows-specific compile definitions
  add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
endif()

## --- Minimal dependency setup (CLI + optional GUI only) ---
# Use zlib from submodule (needed for PFSC decompression)
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "Build zlib examples" FORCE)
set(ZLIB_BUILD_TESTS OFF CACHE BOOL "Build zlib tests" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/externals/zlib EXCLUDE_FROM_ALL)

# Configure cryptopp from submodule (RSA/AES/HMAC)
set(CRYPTOPP_BUILD_TESTING OFF CACHE BOOL "Build cryptopp tests")
set(CRYPTOPP_BUILD_DOCUMENTATION OFF CACHE BOOL "Build cryptopp documentation")
set(CRYPTOPP_INSTALL OFF CACHE BOOL "Install cryptopp library")
set(CRYPTOPP_SOURCES ${CMAKE_SOURCE_DIR}/externals/cryptopp)
add_subdirectory(${CMAKE_SOURCE_DIR}/externals/cryptopp-cmake EXCLUDE_FROM_ALL)

# Only include required source include paths (drop tracy, toml11, fmt, etc.)
include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/externals                # cryptopp headers
  ${CMAKE_SOURCE_DIR}/externals/zlib           # zlib headers
)

# On Windows, include our compatibility header directory
if(MSVC)
  include_directories(BEFORE "${CMAKE_SOURCE_DIR}/.github")
endif()

# fmt removed – using only standard library facilities now.

# Configure required version info for scm_rev.cpp
set(EMULATOR_VERSION_MAJOR "0")
set(EMULATOR_VERSION_MINOR "8")
set(EMULATOR_VERSION_PATCH "0")
set(APP_VERSION "${EMULATOR_VERSION_MAJOR}.${EMULATOR_VERSION_MINOR}.${EMULATOR_VERSION_PATCH}")
set(APP_IS_RELEASE true)

# Generate scm_rev.cpp file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/scm_rev.cpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/src/common/scm_rev.cpp"
    @ONLY
)

# Add common source files needed for the tool
set(COMMON_FILES
  src/common/io_file.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/src/common/scm_rev.cpp
)

# Add core sources needed for extraction
set(CORE_FILES
  src/core/crypto/crypto.cpp
  src/core/file_format/pkg.cpp
  src/core/file_format/pkg_type.cpp
)

#---------------------------------------------
# Shared library for PKG tool core (extraction logic)
#---------------------------------------------
add_library(ps4_pkg_tool_core STATIC
    ${COMMON_FILES}
    ${CORE_FILES}
    src/pkg_tool/lib.cpp
)
target_include_directories(ps4_pkg_tool_core PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(ps4_pkg_tool_core PUBLIC cryptopp::cryptopp zlibstatic)
if(MSVC)
  target_link_libraries(ps4_pkg_tool_core PUBLIC ws2_32)
endif()

#---------------------------------------------
# CLI tool
#---------------------------------------------
add_executable(ps4-pkg-tool src/cli/main.cpp)
target_link_libraries(ps4-pkg-tool PRIVATE ps4_pkg_tool_core)
if(MSVC)
  target_link_libraries(ps4-pkg-tool PRIVATE ws2_32)
endif()
install(TARGETS ps4-pkg-tool RUNTIME DESTINATION bin)

#---------------------------------------------
# Optional minimal Qt GUI frontend
#---------------------------------------------
option(BUILD_PKG_QT_GUI "Build minimal Qt GUI frontend for ps4-pkg-tool" ON)
if(BUILD_PKG_QT_GUI)
  find_package(Qt6 COMPONENTS Widgets QUIET)
  if(Qt6Widgets_FOUND)
    message(STATUS "Building ps4-pkg-tool-gui (Qt6)")
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    add_executable(ps4-pkg-tool-gui
        src/gui/pkg_tool_gui.cpp
    )
    target_link_libraries(ps4-pkg-tool-gui PRIVATE ps4_pkg_tool_core Qt6::Widgets)
    install(TARGETS ps4-pkg-tool-gui RUNTIME DESTINATION bin)
  else()
    message(WARNING "Qt6 Widgets not found; ps4-pkg-tool-gui will not be built.")
  endif()
endif()
