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
    -DCRYPTOPP_DISABLE_OFF64T
    # Disable assembly code that's causing issues
    -DCRYPTOPP_DISABLE_ASM
    # Add Windows-specific defines
    -D_WINDOWS
    -D_USE_MATH_DEFINES
  )
  
  # Provide dummy definitions or workarounds for functions not available on Windows
  if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.19.0")
    cmake_language(DEFER CALL cryptopp_windows_fixes)
  endif()
  
  # Function to apply additional fixes after targets have been created
  function(cryptopp_windows_fixes)
    if(TARGET cryptopp)
      message(STATUS "Applying CryptoPP Windows compatibility fixes to target")
      
      # Set Windows-specific compile definitions
      target_compile_definitions(cryptopp PRIVATE 
        CRYPTOPP_NO_UNISTD_H 
        CRYPTOPP_DISABLE_PTHREADS
        CRYPTOPP_DISABLE_ASM
        _USE_MATH_DEFINES
      )
      
      # Force Windows-specific include paths if needed
      file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/externals/cryptopp/win_compat")
      target_include_directories(cryptopp PRIVATE
        "${CMAKE_SOURCE_DIR}/externals/cryptopp/win_compat"
      )
      
      # Disable specific source files that cause issues
      get_target_property(CRYPTOPP_SOURCES cryptopp SOURCES)
      if(CRYPTOPP_SOURCES)
        list(FILTER CRYPTOPP_SOURCES EXCLUDE REGEX ".*\\.asm$")
        list(FILTER CRYPTOPP_SOURCES EXCLUDE REGEX ".*\\.S$")
        set_target_properties(cryptopp PROPERTIES SOURCES "${CRYPTOPP_SOURCES}")
      endif()
    endif()
    
    if(TARGET cryptopp-static)
      message(STATUS "Applying CryptoPP Windows compatibility fixes to static target")
      
      # Set Windows-specific compile definitions
      target_compile_definitions(cryptopp-static PRIVATE 
        CRYPTOPP_NO_UNISTD_H 
        CRYPTOPP_DISABLE_PTHREADS
        CRYPTOPP_DISABLE_ASM
        _USE_MATH_DEFINES
      )
      
      # Force Windows-specific include paths if needed
      target_include_directories(cryptopp-static PRIVATE
        "${CMAKE_SOURCE_DIR}/externals/cryptopp/win_compat"
      )
      
      # Disable specific source files that cause issues
      get_target_property(CRYPTOPP_STATIC_SOURCES cryptopp-static SOURCES)
      if(CRYPTOPP_STATIC_SOURCES)
        list(FILTER CRYPTOPP_STATIC_SOURCES EXCLUDE REGEX ".*\\.asm$")
        list(FILTER CRYPTOPP_STATIC_SOURCES EXCLUDE REGEX ".*\\.S$")
        set_target_properties(cryptopp-static PROPERTIES SOURCES "${CRYPTOPP_STATIC_SOURCES}")
      endif()
    endif()
  endfunction()
  
  # Create dummy unistd.h header for Windows builds
  file(WRITE "${CMAKE_SOURCE_DIR}/externals/cryptopp/win_compat/unistd.h" "// Empty unistd.h for Windows compatibility\n")
endif()