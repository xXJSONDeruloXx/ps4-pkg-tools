# SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later

# This file contains compatibility fixes for building with MSVC on Windows
message(STATUS "Applying MSVC compatibility fixes")

# Force MSVC to use UTF-8 encoding for source files
add_compile_options(/utf-8)

# Enable multi-processor compilation
add_compile_options(/MP)

# Disable unsafe CRT function warnings (like fopen)
add_compile_options(/D_CRT_SECURE_NO_WARNINGS)

# Disable min/max macros from windows.h
add_compile_options(/DNOMINMAX)

# Increase object file section name length
add_compile_options(/bigobj)

# C++23 mode (/std:c++latest)
add_compile_options(/std:c++latest)

# Treat warnings as errors in CI builds
if(DEFINED ENV{CI})
  add_compile_options(/WX)
endif()

# Common Windows libraries that may be needed
if(WIN32)
  # Make sure we link against these Windows-specific libraries
  link_libraries(ws2_32.lib Shlwapi.lib)
endif()