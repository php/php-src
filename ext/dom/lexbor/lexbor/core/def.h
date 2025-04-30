/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_DEF_H
#define LEXBOR_DEF_H

#define LEXBOR_STRINGIZE_HELPER(x) #x
#define LEXBOR_STRINGIZE(x) LEXBOR_STRINGIZE_HELPER(x)

/* Format */
#ifdef _WIN32
    #define LEXBOR_FORMAT_Z "%Iu"
#else
    #define LEXBOR_FORMAT_Z "%zu"
#endif

/* Deprecated */
#ifdef _MSC_VER
    #define LXB_DEPRECATED(func) __declspec(deprecated) func
#elif defined(__GNUC__) || defined(__INTEL_COMPILER)
    #define LXB_DEPRECATED(func) func __attribute__((deprecated))
#else
    #define LXB_DEPRECATED(func) func
#endif

/* Debug */
//#define LEXBOR_DEBUG(...) do {} while (0)
//#define LEXBOR_DEBUG_ERROR(...) do {} while (0)

#define LEXBOR_MEM_ALIGN_STEP sizeof(void *)

#ifndef LEXBOR_STATIC
    #ifdef _WIN32
        #ifdef LEXBOR_BUILDING
            #define LXB_API __declspec(dllexport)
        #else
            #define LXB_API __declspec(dllimport)
        #endif
    #elif (defined(__SUNPRO_C)  || defined(__SUNPRO_CC))
        #define LXB_API __global
    #else
        #if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__INTEL_COMPILER)
            #define LXB_API __attribute__ ((visibility("default")))
        #else
            #define LXB_API
        #endif
    #endif
#else
    #define LXB_API
#endif

#define LXB_EXTERN extern LXB_API

#endif /* LEXBOR_DEF_H */
