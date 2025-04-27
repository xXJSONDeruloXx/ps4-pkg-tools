#pragma once
/*
 * This file specifically addresses template redefinition issues in strciphr.cpp
 * It forces template instantiations to be excluded from the problematic file
 * to avoid duplicate definition errors with MSVC
 */

// Prevent template instantiation in strciphr.cpp
#define CRYPTOPP_MANUALLY_INSTANTIATE_TEMPLATES 1

// Exclude common templates that cause redefinition errors
namespace CryptoPP {
    // Forward declarations of templates that cause redefinition errors
    template <class T> class AdditiveCipherTemplate;
    template <class T> class CFB_Mode_ExternalCipher;
    template <class T> class SymmetricCipherFinal;
    
    // Explicit "extern template" declarations to prevent duplicate instantiations
    #define CRYPTOPP_STRCIPHR_EXTERN_TEMPLATE(x) extern template x
    
    // These are the templates that cause the most issues
    CRYPTOPP_STRCIPHR_EXTERN_TEMPLATE(void AdditiveCipherTemplate<>::UncheckedSetKey(const byte*, unsigned int, const NameValuePairs&));
    CRYPTOPP_STRCIPHR_EXTERN_TEMPLATE(void AdditiveCipherTemplate<>::ProcessData(byte*, const byte*, size_t));
    CRYPTOPP_STRCIPHR_EXTERN_TEMPLATE(void AdditiveCipherTemplate<>::Seek(lword));
    CRYPTOPP_STRCIPHR_EXTERN_TEMPLATE(void AdditiveCipherTemplate<>::GenerateBlock(byte*, size_t));
    CRYPTOPP_STRCIPHR_EXTERN_TEMPLATE(void AdditiveCipherTemplate<>::ProcessString(byte*, size_t));
    CRYPTOPP_STRCIPHR_EXTERN_TEMPLATE(void AdditiveCipherTemplate<>::ProcessString(byte*, const byte*, size_t));
    CRYPTOPP_STRCIPHR_EXTERN_TEMPLATE(void AdditiveCipherTemplate<>::Resynchronize(const byte*, int));
}