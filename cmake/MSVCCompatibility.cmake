# SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later

# This file contains compatibility fixes for building with MSVC on Windows
message(STATUS "Applying MSVC compatibility fixes")

# Only apply C/C++ specific options to C/C++ files
set(MSVC_C_CXX_FLAGS
    # Force MSVC to use UTF-8 encoding for source files
    /utf-8
    # Enable multi-processor compilation
    /MP
    # Increase object file section name length
    /bigobj
    # C++23 mode
    /std:c++latest
)

# These definitions can be applied to all files
add_compile_definitions(
    # Disable unsafe CRT function warnings (like fopen)
    _CRT_SECURE_NO_WARNINGS
    # Disable min/max macros from windows.h
    NOMINMAX
)

# Apply C/C++ specific options only to C/C++ files
foreach(flag ${MSVC_C_CXX_FLAGS})
    add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:${flag}>)
endforeach()

# Treat warnings as errors in CI builds
if(DEFINED ENV{CI})
  add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/WX>)
endif()

# Common Windows libraries that may be needed
if(WIN32)
  # Make sure we link against these Windows-specific libraries
  link_libraries(ws2_32.lib Shlwapi.lib)
endif()