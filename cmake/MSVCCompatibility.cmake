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

# Apply C/C++ specific options only to C/C++ files - fixing malformed path issue
foreach(flag ${MSVC_C_CXX_FLAGS})
    add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:${flag}>)
endforeach()

# Special flags for external dependencies to disable warning as errors
function(apply_external_library_fixes)
    message(STATUS "Applying warning level fixes for external libraries")
    
    # Create list of external targets to modify
    set(external_targets
        cryptopp
        cryptopp-static
        fmt
        zlibstatic
    )
    
    # Apply fixes to each target if it exists
    foreach(target ${external_targets})
        if(TARGET ${target})
            message(STATUS "Applying warning level fixes for ${target}")
            # Disable warnings as errors and reduce warning level
            target_compile_options(${target} PRIVATE 
                /WX- # Disable warnings as errors
                /W0  # Disable warnings
            )
        endif()
    endforeach()
endfunction()

# Register a callback to be called after all targets have been defined
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.19.0")
    cmake_language(DEFER CALL apply_external_library_fixes)
else()
    # For older CMake versions, use a variable watch
    variable_watch(CMAKE_CONFIGURATION_TYPES apply_external_library_fixes)
endif()

# Treat warnings as errors in CI builds, but only for our own code
if(DEFINED ENV{CI})
    # Create a function to apply warning settings to our code
    function(apply_own_code_warning_settings)
        # Get all targets in the project
        get_property(all_targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)
        
        # List of known external targets to exclude
        set(external_targets
            cryptopp
            cryptopp-static
            fmt
            zlibstatic
            zlib
        )
        
        # Apply warnings as errors to our targets only
        foreach(target ${all_targets})
            # Skip if target is in external list
            list(FIND external_targets ${target} target_idx)
            if(${target_idx} EQUAL -1)
                # Apply warnings as errors only to our code
                get_target_property(target_type ${target} TYPE)
                if(NOT "${target_type}" STREQUAL "INTERFACE_LIBRARY")
                    message(STATUS "Enabling warnings as errors for ${target}")
                    target_compile_options(${target} PRIVATE /WX)
                endif()
            endif()
        endforeach()
    endfunction()
    
    # Register this function to run after targets are created
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.19.0")
        cmake_language(DEFER CALL apply_own_code_warning_settings)
    endif()
endif()

# Common Windows libraries that may be needed
if(WIN32)
    # Make sure we link against these Windows-specific libraries
    link_libraries(ws2_32.lib Shlwapi.lib)
endif()