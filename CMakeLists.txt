# SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.24)
project(ps4-pkg-tool CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## Minimal build: no platform-specific extras or external find modules.

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


# fmt removed – using only standard library facilities now.

set(COMMON_FILES src/common/io_file.cpp)

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

#---------------------------------------------
# CLI tool
#---------------------------------------------
add_executable(ps4-pkg-tool src/cli/main.cpp)
target_link_libraries(ps4-pkg-tool PRIVATE ps4_pkg_tool_core)
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
