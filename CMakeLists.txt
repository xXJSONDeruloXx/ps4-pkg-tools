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
  
  # Include CryptoPP compatibility fixes for Windows
  if(EXISTS "${CMAKE_MODULE_PATH}/CryptoppCompatibility.cmake")
    include("${CMAKE_MODULE_PATH}/CryptoppCompatibility.cmake")
  endif()
endif()

# Define options to handle platform-specific issues
option(BUILD_WITHOUT_PTHREAD "Disable pthread support" OFF)
option(DISABLE_OFF64_T_CHECK "Disable off64_t size check" OFF)

# For Windows, we need to disable pthread and off64_t checks
if(WIN32)
  set(BUILD_WITHOUT_PTHREAD ON)
  set(DISABLE_OFF64_T_CHECK ON)
endif()

# Use zlib from submodule
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "Build zlib examples" FORCE)
set(ZLIB_BUILD_TESTS OFF CACHE BOOL "Build zlib tests" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/externals/zlib EXCLUDE_FROM_ALL)

# Configure cryptopp from submodule
set(CRYPTOPP_BUILD_TESTING OFF CACHE BOOL "Build cryptopp tests")
set(CRYPTOPP_BUILD_DOCUMENTATION OFF CACHE BOOL "Build cryptopp documentation") 
set(CRYPTOPP_INSTALL OFF CACHE BOOL "Install cryptopp library")
set(CRYPTOPP_SOURCES ${CMAKE_SOURCE_DIR}/externals/cryptopp)

# On Windows, disable warnings in cryptopp
if(WIN32)
  set(CRYPTOPP_WARNING_OPTIONS "/W0;/WX-" CACHE STRING "Warning options for cryptopp" FORCE)
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/externals/cryptopp-cmake EXCLUDE_FROM_ALL)

# Include paths for external libraries
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/externals/toml11/include
    ${CMAKE_SOURCE_DIR}/externals/tracy/public
    ${CMAKE_SOURCE_DIR}/externals/fmt/include
    ${CMAKE_SOURCE_DIR}/externals                # Add this line to include cryptopp directly
    ${CMAKE_SOURCE_DIR}/externals/zlib           # Include zlib headers
)

# On Windows, include our compatibility header directory
if(MSVC)
  include_directories(BEFORE "${CMAKE_SOURCE_DIR}/.github")
  
  # Create a win_compat directory if it doesn't exist
  if(NOT EXISTS "${CMAKE_SOURCE_DIR}/externals/cryptopp/win_compat")
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/externals/cryptopp/win_compat")
  endif()
endif()

# Build fmt from source
add_subdirectory(${CMAKE_SOURCE_DIR}/externals/fmt EXCLUDE_FROM_ALL)

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
    src/common/error.cpp
    src/common/path_util.cpp
    src/common/string_util.cpp
    src/common/config.cpp
    src/common/assert.cpp
    src/common/thread.cpp
    src/common/logging/backend.cpp
    src/common/logging/filter.cpp
    src/common/logging/text_formatter.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/scm_rev.cpp
)

# Add core sources needed for extraction
set(CORE_FILES
    src/core/crypto/crypto.cpp
    src/core/file_format/pkg.cpp
    src/core/file_format/pkg_type.cpp
    src/core/file_format/trp.cpp
)

# Build the CLI tool
add_executable(ps4-pkg-tool
    src/cli/main.cpp
    ${COMMON_FILES}
    ${CORE_FILES}
)

# Link dependencies - use zlib target instead of ZLIB::ZLIB
target_link_libraries(ps4-pkg-tool PRIVATE 
    fmt::fmt
    cryptopp::cryptopp
    zlibstatic              # Use zlibstatic instead of ZLIB::ZLIB
)

# On Windows, we need to add additional system libraries
if(MSVC)
  target_link_libraries(ps4-pkg-tool PRIVATE ws2_32 Shlwapi)
endif()

# Install target
install(TARGETS ps4-pkg-tool RUNTIME DESTINATION bin)
