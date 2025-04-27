# MSVCCompatibility.cmake
# Custom module to handle MSVC-specific compatibility fixes

if(MSVC)
  message(STATUS "Applying comprehensive MSVC compatibility fixes for CryptoPP")
  
  # Add our patch header directory to include paths (high priority)
  include_directories(BEFORE "${CMAKE_CURRENT_LIST_DIR}")
  
  # Force C++17 minimum for MSVC to ensure STL compatibility with CryptoPP
  # Needed for proper allocator support
  if(CMAKE_CXX_STANDARD LESS 17)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
  endif()
  
  # Add MSVC-specific compiler flags for better standards compliance
  # /Zc:__cplusplus - Ensures correct __cplusplus macro value for better C++ conformance
  # /permissive- - Enforces standards compliance
  # /bigobj - Supports large object files needed for template-heavy code
  # /FI"cryptopp_wrapper.h" - Forces inclusion of our wrapper header in every source file
  add_compile_options(/MP /bigobj /Zc:__cplusplus /permissive- /FI"${CMAKE_CURRENT_LIST_DIR}/cryptopp_wrapper.h")
  
  # Suppress CryptoPP-related warnings
  add_compile_options(/wd4100 /wd4505 /wd4127 /wd4189 /wd4996)
  
  # Add compile definitions for Windows 
  add_compile_definitions(
    NOMINMAX
    WIN32_LEAN_AND_MEAN
  )
  
  # We define almost nothing here now because our forced-included wrapper handles it
  message(STATUS "Using cryptopp_wrapper.h to intercept and fix CryptoPP headers")
endif()