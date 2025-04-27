#pragma once
// This wrapper intercepts all CryptoPP includes and ensures they're properly
// configured before the actual headers are included

// Prevent the Windows min/max macros from interfering
#define NOMINMAX

// Apply fixes before any CryptoPP headers
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#define CRYPTOPP_DISABLE_ASM 1
#define CRYPTOPP_DISABLE_MIXED_ASM 1
#define CRYPTOPP_DISABLE_SSE2 1
#define CRYPTOPP_DISABLE_SSSE3 1
#define CRYPTOPP_DISABLE_INTEL_ASM 1
#define CRYPTOPP_MAINTAIN_BACKWARDS_COMPATIBILITY_562 1

// Turn off template instantiation in headers
#define CRYPTOPP_MANUALLY_INSTANTIATE_TEMPLATES 1
#define CRYPTOPP_DISABLE_TEMPLATE_SPECIALIZATION 1

// Critical: Completely bypass the problematic strciphr.cpp template instantiations
// This will prevent duplicate template definitions
#define CRYPTOPP_EXTERN_TEMPLATE_EXCLUDE_STRCIPHR 1

// Silence deprecation warnings
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

// Include our specific fix for strciphr.cpp
#include "strciphr_fix.h"

// Standard includes needed for Windows
#include <string>
#include <vector>
#include <algorithm>
#include <memory>

// Make sure all std::allocator traits exist 
namespace std {
  template <typename T> struct allocator_traits;
}

// Now we can safely include the original headers
// Add this line wherever you need CryptoPP functionality
// #include <cryptopp/cryptlib.h>