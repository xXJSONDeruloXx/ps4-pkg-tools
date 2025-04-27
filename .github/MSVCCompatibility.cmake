# MSVCCompatibility.cmake
# Custom module to handle MSVC-specific compatibility fixes

if(MSVC)
  message(STATUS "Applying MSVC compatibility fixes for CryptoPP")
  
  # Add our patch header directory to include paths
  include_directories(BEFORE "${CMAKE_CURRENT_LIST_DIR}")
  
  # Add compile definitions to fix allocator issues
  add_compile_definitions(
    CRYPTOPP_DISABLE_ASM
    CRYPTOPP_DISABLE_MIXED_ASM
    CRYPTOPP_DISABLE_SSE2
    CRYPTOPP_DISABLE_SSSE3
    CRYPTOPP_DISABLE_UNCAUGHT_EXCEPTION
    CRYPTOPP_DISABLE_INTEL_ASM
    CRYPTOPP_MAINTAIN_BACKWARDS_COMPATIBILITY_562
    CRYPTOPP_MANUALLY_INSTANTIATE_TEMPLATES
    CRYPTOPP_DISABLE_TEMPLATE_SPECIALIZATION
    CRYPTOPP_INHIBIT_INSTANTIATE_TEMPLATES=1
    CRYPTOPP_CRYPTLIB_H_NO_EXTERN_TEMPLATE_TEMPLATE
    CRYPTOPP_STRCIPHR_MANUALLY_INSTANTIATE=1
    _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
    _SILENCE_CXX20_CISO646_REMOVED_WARNING
    NOMINMAX
  )
  
  # Force C++17 minimum for MSVC (even though we're using C++23)
  # This ensures better STL compatibility with CryptoPP
  if(CMAKE_CXX_STANDARD LESS 17)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
  endif()
  
  # Add MSVC-specific compiler flags for better standards compliance
  add_compile_options(/MP /bigobj /Zc:__cplusplus /permissive-)
  
  # Disable specific warnings that might appear with CryptoPP
  add_compile_options(/wd4100 /wd4505 /wd4127 /wd4189 /wd4996)
endif()