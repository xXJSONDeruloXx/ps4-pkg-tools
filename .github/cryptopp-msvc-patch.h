// This patch file addresses CryptoPP compatibility issues with MSVC
// It should be included before any CryptoPP headers

// Disable problematic features in CryptoPP for MSVC
#define CRYPTOPP_DISABLE_ASM
#define CRYPTOPP_DISABLE_MIXED_ASM
#define CRYPTOPP_DISABLE_SSE2 
#define CRYPTOPP_DISABLE_SSSE3
#define CRYPTOPP_DISABLE_UNCAUGHT_EXCEPTION
#define CRYPTOPP_DISABLE_INTEL_ASM

// Force backwards compatibility mode for allocators
#define CRYPTOPP_MAINTAIN_BACKWARDS_COMPATIBILITY_562

// Fix template and allocator issues
#define CRYPTOPP_MANUALLY_INSTANTIATE_TEMPLATES
#define CRYPTOPP_DISABLE_TEMPLATE_SPECIALIZATION 
#define CRYPTOPP_INHIBIT_INSTANTIATE_TEMPLATES 1

// Additional template fixes
#define CRYPTOPP_CRYPTLIB_H_NO_EXTERN_TEMPLATE_TEMPLATE

// Fix specific issues with strciphr.cpp
#define CRYPTOPP_STRCIPHR_MANUALLY_INSTANTIATE 1

// Tell STL to use std::allocator for CryptoPP types
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

// Ensure Windows-specific headers are included
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <io.h>
#define NOMINMAX  // Prevent Windows min/max macros from interfering

// Map POSIX functions to Windows equivalents where needed
#ifndef HAVE_UNISTD_H
  #define access _access
  #define strcasecmp _stricmp
  #define strncasecmp _strnicmp
  #define fileno _fileno
#endif

// Add allocator compatibility layer
#ifdef _MSC_VER
  // Prevent errors about undeclared vector specializations
  namespace std {
    template<class T> struct _Is_CryptoPP_allocator : std::false_type {};
    
    template<class T>
    struct _Is_CryptoPP_allocator<CryptoPP::AllocatorWithCleanup<T>> : std::true_type {};
  }
#endif