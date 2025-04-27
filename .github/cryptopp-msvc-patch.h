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

// Fix allocator issues
#define CRYPTOPP_MANUALLY_INSTANTIATE_TEMPLATES
#define CRYPTOPP_DISABLE_TEMPLATE_SPECIALIZATION

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