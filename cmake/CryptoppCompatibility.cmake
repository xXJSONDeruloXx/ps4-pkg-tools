# SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later

# This file contains compatibility fixes for building CryptoPP on Windows
message(STATUS "Applying CryptoPP Windows compatibility fixes")

if(WIN32)
  # Handle missing headers and functions on Windows
  add_definitions(
    # Define these to prevent cryptopp from looking for unistd.h and pthread
    -DCRYPTOPP_NO_UNISTD_H
    -DCRYPTOPP_DISABLE_PTHREADS
    # Bypass the off64_t check
    -DCRYPTOPP_DISABLE_OFFTT
    # Add Windows-specific defines
    -D_WINDOWS
  )
  
  # Provide dummy definitions or workarounds for functions not available on Windows
  if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.19.0")
    cmake_language(DEFER CALL cryptopp_windows_fixes)
  endif()
  
  # Function to apply additional fixes after targets have been created
  function(cryptopp_windows_fixes)
    if(TARGET cryptopp OR TARGET cryptopp-static)
      message(STATUS "Applying CryptoPP Windows compatibility fixes")
      
      # Set Windows-specific compile definitions
      target_compile_definitions(cryptopp PRIVATE 
        CRYPTOPP_NO_UNISTD_H 
        CRYPTOPP_DISABLE_PTHREADS
      )
      
      # Force Windows-specific include paths 
      target_include_directories(cryptopp PRIVATE
        "${CMAKE_SOURCE_DIR}/externals/cryptopp/win_compat"
      )
    endif()
  endfunction()
endif()