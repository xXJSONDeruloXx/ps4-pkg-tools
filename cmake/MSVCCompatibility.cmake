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

# Special flags for external dependencies to disable warning as errors
function(apply_external_library_fixes)
    message(STATUS "Applying fixes for external libraries")
    
    # Turn off warnings as errors for external libraries
    # Use target_compile_options to ensure it only affects specific targets
    if(TARGET cryptopp)
        message(STATUS "Applying fixes for CryptoPP")
        target_compile_options(cryptopp PRIVATE 
            /WX- # Disable warnings as errors
            /W0  # Disable warnings
        )
    endif()
    
    if(TARGET cryptopp-static)
        message(STATUS "Applying fixes for CryptoPP-static")
        target_compile_options(cryptopp-static PRIVATE 
            /WX- # Disable warnings as errors
            /W0  # Disable warnings
        )
    endif()
    
    # Add any other external libraries that need special fixes here
endfunction()

# Register a callback to be called after all targets have been defined
# This ensures our function runs after the external targets are created
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.19.0")
    cmake_language(DEFER CALL apply_external_library_fixes)
else()
    # For older CMake versions, we may need to call this function manually
    # via a macro or in another part of the build
    variable_watch(CMAKE_CURRENT_LIST_DIR apply_external_library_fixes)
endif()

# Treat warnings as errors in CI builds, but only for our own code
# (already disabled for external libraries above)
if(DEFINED ENV{CI})
  # Use a regex pattern to exclude external libraries
  add_compile_options($<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<NOT:$<IN_LIST:$<TARGET_PROPERTY:NAME>,cryptopp;cryptopp-static>>>:/WX>)
endif()

# Common Windows libraries that may be needed
if(WIN32)
  # Make sure we link against these Windows-specific libraries
  link_libraries(ws2_32.lib Shlwapi.lib)
endif()