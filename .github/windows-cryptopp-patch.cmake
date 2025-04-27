# This file contains patches for CryptoPP compatibility with MSVC
# It will be included in the Windows build workflow

# Disable problematic features in CryptoPP for Windows builds
add_definitions(-DCRYPTOPP_DISABLE_ASM)
add_definitions(-DCRYPTOPP_DISABLE_MIXED_ASM)
add_definitions(-DCRYPTOPP_DISABLE_SSE2)
add_definitions(-DCRYPTOPP_DISABLE_SSSE3)
add_definitions(-DCRYPTOPP_DISABLE_UNCAUGHT_EXCEPTION)

# Force use of standard allocators for vectors
add_definitions(-DCRYPTOPP_MAINTAIN_BACKWARDS_COMPATIBILITY_562)

# Prevent inline assembly usage which causes issues with MSVC
add_definitions(-DCRYPTOPP_DISABLE_INTEL_ASM)

message(STATUS "Applied Windows-specific CryptoPP compatibility patches")