/* auto-generated on Fri Jul 17 14:52:28 EDT 2020. Do not edit! */
/* begin file include/simdjson.h */
#ifndef SIMDJSON_H
#define SIMDJSON_H

/**
 * @mainpage
 *
 * Check the [README.md](https://github.com/lemire/simdjson/blob/master/README.md#simdjson--parsing-gigabytes-of-json-per-second).
 */

/* begin file include/simdjson/compiler_check.h */
#ifndef SIMDJSON_COMPILER_CHECK_H
#define SIMDJSON_COMPILER_CHECK_H

#ifndef __cplusplus
#error simdjson requires a C++ compiler
#endif

#ifndef SIMDJSON_CPLUSPLUS
#if defined(_MSVC_LANG) && !defined(__clang__)
#define SIMDJSON_CPLUSPLUS (_MSC_VER == 1900 ? 201103L : _MSVC_LANG)
#else
#define SIMDJSON_CPLUSPLUS __cplusplus
#endif
#endif

// C++ 17
#if !defined(SIMDJSON_CPLUSPLUS17) && (SIMDJSON_CPLUSPLUS >= 201703L)
#define SIMDJSON_CPLUSPLUS17 1
#endif

// C++ 14
#if !defined(SIMDJSON_CPLUSPLUS14) && (SIMDJSON_CPLUSPLUS >= 201402L)
#define SIMDJSON_CPLUSPLUS14 1
#endif

// C++ 11
#if !defined(SIMDJSON_CPLUSPLUS11) && (SIMDJSON_CPLUSPLUS >= 201103L)
#define SIMDJSON_CPLUSPLUS11 1
#endif

#ifndef SIMDJSON_CPLUSPLUS11
#error simdjson requires a compiler compliant with the C++11 standard
#endif

#endif // SIMDJSON_COMPILER_CHECK_H
/* end file include/simdjson/compiler_check.h */
/* begin file include/simdjson/common_defs.h */
#ifndef SIMDJSON_COMMON_DEFS_H
#define SIMDJSON_COMMON_DEFS_H

#include <cassert>
/* begin file include/simdjson/portability.h */
#ifndef SIMDJSON_PORTABILITY_H
#define SIMDJSON_PORTABILITY_H

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cfloat>
#include <cassert>

#ifdef _MSC_VER
#define SIMDJSON_VISUAL_STUDIO 1
/**
 * We want to differentiate carefully between
 * clang under visual studio and regular visual
 * studio.
 *
 * Under clang for Windows, we enable:
 *  * target pragmas so that part and only part of the
 *     code gets compiled for advanced instructions.
 *  * computed gotos.
 *
 */
#ifdef __clang__
// clang under visual studio
#define SIMDJSON_CLANG_VISUAL_STUDIO 1
#else
// just regular visual studio (best guess)
#define SIMDJSON_REGULAR_VISUAL_STUDIO 1
#endif // __clang__
#endif // _MSC_VER

#ifdef SIMDJSON_REGULAR_VISUAL_STUDIO
// https://en.wikipedia.org/wiki/C_alternative_tokens
// This header should have no effect, except maybe
// under Visual Studio.
#include <iso646.h>
#endif

#if defined(__x86_64__) || defined(_M_AMD64)
#define SIMDJSON_IS_X86_64 1
#elif defined(__aarch64__) || defined(_M_ARM64)
#define SIMDJSON_IS_ARM64 1
#else
#define SIMDJSON_IS_32BITS 1

// We do not support 32-bit platforms, but it can be
// handy to identify them.
#if defined(_M_IX86) || defined(__i386__)
#define SIMDJSON_IS_X86_32BITS 1
#elif defined(__arm__) || defined(_M_ARM)
#define SIMDJSON_IS_ARM_32BITS 1
#endif

#endif // defined(__x86_64__) || defined(_M_AMD64)

#ifdef SIMDJSON_IS_32BITS
#pragma message("The simdjson library is designed \
for 64-bit processors and it seems that you are not \
compiling for a known 64-bit platform. All fast kernels \
will be disabled and performance may be poor. Please \
use a 64-bit target such as x64 or 64-bit ARM.")
#endif // SIMDJSON_IS_32BITS

// this is almost standard?
#undef STRINGIFY_IMPLEMENTATION_
#undef STRINGIFY
#define STRINGIFY_IMPLEMENTATION_(a) #a
#define STRINGIFY(a) STRINGIFY_IMPLEMENTATION_(a)

#ifndef SIMDJSON_IMPLEMENTATION_FALLBACK
#define SIMDJSON_IMPLEMENTATION_FALLBACK 1
#endif

#if SIMDJSON_IS_ARM64
#ifndef SIMDJSON_IMPLEMENTATION_ARM64
#define SIMDJSON_IMPLEMENTATION_ARM64 1
#endif
#define SIMDJSON_IMPLEMENTATION_HASWELL 0
#define SIMDJSON_IMPLEMENTATION_WESTMERE 0
#endif // SIMDJSON_IS_ARM64

// Our fast kernels require 64-bit systems.
//
// On 32-bit x86, we lack 64-bit popcnt, lzcnt, blsr instructions.
// Furthermore, the number of SIMD registers is reduced.
//
// On 32-bit ARM, we would have smaller registers.
//
// The simdjson users should still have the fallback kernel. It is
// slower, but it should run everywhere.
#if SIMDJSON_IS_X86_64
#ifndef SIMDJSON_IMPLEMENTATION_HASWELL
#define SIMDJSON_IMPLEMENTATION_HASWELL 1
#endif
#ifndef SIMDJSON_IMPLEMENTATION_WESTMERE
#define SIMDJSON_IMPLEMENTATION_WESTMERE 1
#endif
#define SIMDJSON_IMPLEMENTATION_ARM64 0
#endif // SIMDJSON_IS_X86_64

// We are going to use runtime dispatch.
#ifdef SIMDJSON_IS_X86_64
#ifdef __clang__
// clang does not have GCC push pop
// warning: clang attribute push can't be used within a namespace in clang up
// til 8.0 so TARGET_REGION and UNTARGET_REGION must be *outside* of a
// namespace.
#define TARGET_REGION(T)                                                       \
  _Pragma(STRINGIFY(                                                           \
      clang attribute push(__attribute__((target(T))), apply_to = function)))
#define UNTARGET_REGION _Pragma("clang attribute pop")
#elif defined(__GNUC__)
// GCC is easier
#define TARGET_REGION(T)                                                       \
  _Pragma("GCC push_options") _Pragma(STRINGIFY(GCC target(T)))
#define UNTARGET_REGION _Pragma("GCC pop_options")
#endif // clang then gcc

#endif // x86

// Default target region macros don't do anything.
#ifndef TARGET_REGION
#define TARGET_REGION(T)
#define UNTARGET_REGION
#endif

// under GCC and CLANG, we use these two macros
#define TARGET_HASWELL TARGET_REGION("avx2,bmi,pclmul,lzcnt")
#define TARGET_WESTMERE TARGET_REGION("sse4.2,pclmul")
#define TARGET_ARM64

// Is threading enabled?
#if defined(BOOST_HAS_THREADS) || defined(_REENTRANT) || defined(_MT)
#ifndef SIMDJSON_THREADS_ENABLED
#define SIMDJSON_THREADS_ENABLED
#endif
#endif

// workaround for large stack sizes under -O0.
// https://github.com/simdjson/simdjson/issues/691
#ifdef __APPLE__
#ifndef __OPTIMIZE__
// Apple systems have small stack sizes in secondary threads.
// Lack of compiler optimization may generate high stack usage.
// Users may want to disable threads for safety, but only when
// in debug mode which we detect by the fact that the __OPTIMIZE__
// macro is not defined.
#undef SIMDJSON_THREADS_ENABLED
#endif
#endif


#if SIMDJSON_DO_NOT_USE_THREADS_NO_MATTER_WHAT
// No matter what happened, we undefine SIMDJSON_THREADS_ENABLED and so disable threads.
#undef SIMDJSON_THREADS_ENABLED
#endif


#if defined(__clang__)
#define NO_SANITIZE_UNDEFINED __attribute__((no_sanitize("undefined")))
#elif defined(__GNUC__)
#define NO_SANITIZE_UNDEFINED __attribute__((no_sanitize_undefined))
#else
#define NO_SANITIZE_UNDEFINED
#endif

#ifdef SIMDJSON_VISUAL_STUDIO
// This is one case where we do not distinguish between
// regular visual studio and clang under visual studio.
// clang under Windows has _stricmp (like visual studio) but not strcasecmp (as clang normally has)
#define simdjson_strcasecmp _stricmp
#define simdjson_strncasecmp _strnicmp
#else
// The strcasecmp, strncasecmp, and strcasestr functions do not work with multibyte strings (e.g. UTF-8).
// So they are only useful for ASCII in our context.
// https://www.gnu.org/software/libunistring/manual/libunistring.html#char-_002a-strings
#define simdjson_strcasecmp strcasecmp
#define simdjson_strncasecmp strncasecmp
#endif

namespace simdjson {
/** @private portable version of  posix_memalign */
static inline void *aligned_malloc(size_t alignment, size_t size) {
  void *p;
#ifdef SIMDJSON_VISUAL_STUDIO
  p = _aligned_malloc(size, alignment);
#elif defined(__MINGW32__) || defined(__MINGW64__)
  p = __mingw_aligned_malloc(size, alignment);
#else
  // somehow, if this is used before including "x86intrin.h", it creates an
  // implicit defined warning.
  if (posix_memalign(&p, alignment, size) != 0) {
    return nullptr;
  }
#endif
  return p;
}

/** @private */
static inline char *aligned_malloc_char(size_t alignment, size_t size) {
  return (char *)aligned_malloc(alignment, size);
}

/** @private */
static inline void aligned_free(void *mem_block) {
  if (mem_block == nullptr) {
    return;
  }
#ifdef SIMDJSON_VISUAL_STUDIO
  _aligned_free(mem_block);
#elif defined(__MINGW32__) || defined(__MINGW64__)
  __mingw_aligned_free(mem_block);
#else
  free(mem_block);
#endif
}

/** @private */
static inline void aligned_free_char(char *mem_block) {
  aligned_free((void *)mem_block);
}

#ifdef NDEBUG

#ifdef SIMDJSON_VISUAL_STUDIO
#define SIMDJSON_UNREACHABLE() __assume(0)
#define SIMDJSON_ASSUME(COND) __assume(COND)
#else
#define SIMDJSON_UNREACHABLE() __builtin_unreachable();
#define SIMDJSON_ASSUME(COND) do { if (!(COND)) __builtin_unreachable(); } while (0)
#endif

#else // NDEBUG

#define SIMDJSON_UNREACHABLE() assert(0);
#define SIMDJSON_ASSUME(COND) assert(COND)

#endif

} // namespace simdjson
#endif // SIMDJSON_PORTABILITY_H
/* end file include/simdjson/portability.h */

namespace simdjson {

#ifndef SIMDJSON_EXCEPTIONS
#if __cpp_exceptions
#define SIMDJSON_EXCEPTIONS 1
#else
#define SIMDJSON_EXCEPTIONS 0
#endif
#endif

/** The maximum document size supported by simdjson. */
constexpr size_t SIMDJSON_MAXSIZE_BYTES = 0xFFFFFFFF;

/**
 * The amount of padding needed in a buffer to parse JSON.
 *
 * the input buf should be readable up to buf + SIMDJSON_PADDING
 * this is a stopgap; there should be a better description of the
 * main loop and its behavior that abstracts over this
 * See https://github.com/lemire/simdjson/issues/174
 */
constexpr size_t SIMDJSON_PADDING = 32;

/**
 * By default, simdjson supports this many nested objects and arrays.
 *
 * This is the default for parser::max_depth().
 */
constexpr size_t DEFAULT_MAX_DEPTH = 1024;

} // namespace simdjson

#if defined(__GNUC__)
  // Marks a block with a name so that MCA analysis can see it.
  #define BEGIN_DEBUG_BLOCK(name) __asm volatile("# LLVM-MCA-BEGIN " #name);
  #define END_DEBUG_BLOCK(name) __asm volatile("# LLVM-MCA-END " #name);
  #define DEBUG_BLOCK(name, block) BEGIN_DEBUG_BLOCK(name); block; END_DEBUG_BLOCK(name);
#else
  #define BEGIN_DEBUG_BLOCK(name)
  #define END_DEBUG_BLOCK(name)
  #define DEBUG_BLOCK(name, block)
#endif

#if !defined(SIMDJSON_REGULAR_VISUAL_STUDIO) && !defined(SIMDJSON_NO_COMPUTED_GOTO)
  // We assume here that *only* regular visual studio
  // does not support computed gotos.
  // Implemented using Labels as Values which works in GCC and CLANG (and maybe
  // also in Intel's compiler), but won't work in MSVC.
  // Compute gotos are good for performance, enable them if you can.
  #define SIMDJSON_USE_COMPUTED_GOTO
#endif

// Align to N-byte boundary
#define ROUNDUP_N(a, n) (((a) + ((n)-1)) & ~((n)-1))
#define ROUNDDOWN_N(a, n) ((a) & ~((n)-1))

#define ISALIGNED_N(ptr, n) (((uintptr_t)(ptr) & ((n)-1)) == 0)

#if defined(SIMDJSON_REGULAR_VISUAL_STUDIO)

  #define really_inline __forceinline
  #define never_inline __declspec(noinline)

  #define UNUSED
  #define WARN_UNUSED

  #ifndef likely
  #define likely(x) x
  #endif
  #ifndef unlikely
  #define unlikely(x) x
  #endif

  #define SIMDJSON_PUSH_DISABLE_WARNINGS __pragma(warning( push ))
  #define SIMDJSON_PUSH_DISABLE_ALL_WARNINGS __pragma(warning( push, 0 ))
  #define SIMDJSON_DISABLE_VS_WARNING(WARNING_NUMBER) __pragma(warning( disable : WARNING_NUMBER ))
  // Get rid of Intellisense-only warnings (Code Analysis)
  // Though __has_include is C++17, it is supported in Visual Studio 2017 or better (_MSC_VER>=1910).
  #if defined(_MSC_VER) && (_MSC_VER>=1910)
  #if __has_include(<CppCoreCheck\Warnings.h>)
  #include <CppCoreCheck\Warnings.h>
  #define SIMDJSON_DISABLE_UNDESIRED_WARNINGS SIMDJSON_DISABLE_VS_WARNING(ALL_CPPCORECHECK_WARNINGS)
  #endif
  #endif

  #ifndef SIMDJSON_DISABLE_UNDESIRED_WARNINGS
  #define SIMDJSON_DISABLE_UNDESIRED_WARNINGS
  #endif

  #define SIMDJSON_DISABLE_DEPRECATED_WARNING SIMDJSON_DISABLE_VS_WARNING(4996)
  #define SIMDJSON_POP_DISABLE_WARNINGS __pragma(warning( pop ))

#else // SIMDJSON_REGULAR_VISUAL_STUDIO

  #define really_inline inline __attribute__((always_inline, unused))
  #define never_inline inline __attribute__((noinline, unused))

  #define UNUSED __attribute__((unused))
  #define WARN_UNUSED __attribute__((warn_unused_result))

  #ifndef likely
  #define likely(x) __builtin_expect(!!(x), 1)
  #endif
  #ifndef unlikely
  #define unlikely(x) __builtin_expect(!!(x), 0)
  #endif

  #define SIMDJSON_PUSH_DISABLE_WARNINGS _Pragma("GCC diagnostic push")
  // gcc doesn't seem to disable all warnings with all and extra, add warnings here as necessary
  #define SIMDJSON_PUSH_DISABLE_ALL_WARNINGS SIMDJSON_PUSH_DISABLE_WARNINGS \
    SIMDJSON_DISABLE_GCC_WARNING(-Weffc++) \
    SIMDJSON_DISABLE_GCC_WARNING(-Wall) \
    SIMDJSON_DISABLE_GCC_WARNING(-Wconversion) \
    SIMDJSON_DISABLE_GCC_WARNING(-Wextra) \
    SIMDJSON_DISABLE_GCC_WARNING(-Wattributes) \
    SIMDJSON_DISABLE_GCC_WARNING(-Wimplicit-fallthrough) \
    SIMDJSON_DISABLE_GCC_WARNING(-Wnon-virtual-dtor) \
    SIMDJSON_DISABLE_GCC_WARNING(-Wreturn-type) \
    SIMDJSON_DISABLE_GCC_WARNING(-Wshadow) \
    SIMDJSON_DISABLE_GCC_WARNING(-Wunused-parameter) \
    SIMDJSON_DISABLE_GCC_WARNING(-Wunused-variable)
  #define SIMDJSON_PRAGMA(P) _Pragma(#P)
  #define SIMDJSON_DISABLE_GCC_WARNING(WARNING) SIMDJSON_PRAGMA(GCC diagnostic ignored #WARNING)
  #if defined(SIMDJSON_CLANG_VISUAL_STUDIO)
  #define SIMDJSON_DISABLE_UNDESIRED_WARNINGS SIMDJSON_DISABLE_GCC_WARNING(-Wmicrosoft-include)
  #else
  #define SIMDJSON_DISABLE_UNDESIRED_WARNINGS
  #endif
  #define SIMDJSON_DISABLE_DEPRECATED_WARNING SIMDJSON_DISABLE_GCC_WARNING(-Wdeprecated-declarations)
  #define SIMDJSON_POP_DISABLE_WARNINGS _Pragma("GCC diagnostic pop")



#endif // MSC_VER

#if defined(SIMDJSON_VISUAL_STUDIO)
    /**
     * It does not matter here whether you are using
     * the regular visual studio or clang under visual
     * studio.
     */
    #if SIMDJSON_USING_LIBRARY
    #define SIMDJSON_DLLIMPORTEXPORT __declspec(dllimport)
    #else
    #define SIMDJSON_DLLIMPORTEXPORT __declspec(dllexport)
    #endif
#else
    #define SIMDJSON_DLLIMPORTEXPORT
#endif

// C++17 requires string_view.
#if SIMDJSON_CPLUSPLUS17
#define SIMDJSON_HAS_STRING_VIEW
#endif

// This macro (__cpp_lib_string_view) has to be defined
// for C++17 and better, but if it is otherwise defined,
// we are going to assume that string_view is available
// even if we do not have C++17 support.
#ifdef __cpp_lib_string_view
#define SIMDJSON_HAS_STRING_VIEW
#endif

// Some systems have string_view even if we do not have C++17 support,
// and even if __cpp_lib_string_view is undefined, it is the case
// with Apple clang version 11.
// We must handle it. *This is important.*
#ifndef SIMDJSON_HAS_STRING_VIEW
#if defined __has_include
// do not combine the next #if with the previous one (unsafe)
#if __has_include (<string_view>)
// now it is safe to trigger the include
#include <string_view> // though the file is there, it does not follow that we got the implementation
#if defined(_LIBCPP_STRING_VIEW)
// Ah! So we under libc++ which under its Library Fundamentals Technical Specification, which preceeded C++17,
// included string_view.
// This means that we have string_view *even though* we may not have C++17.
#define SIMDJSON_HAS_STRING_VIEW
#endif // _LIBCPP_STRING_VIEW
#endif // __has_include (<string_view>)
#endif // defined __has_include
#endif // def SIMDJSON_HAS_STRING_VIEW
// end of complicated but important routine to try to detect string_view.

//
// Backfill std::string_view using nonstd::string_view on systems where
// we expect that string_view is missing. Important: if we get this wrong,
// we will end up with two string_view definitions and potential trouble.
// That is why we work so hard above to avoid it.
//
#ifndef SIMDJSON_HAS_STRING_VIEW
SIMDJSON_PUSH_DISABLE_ALL_WARNINGS
/* begin file include/simdjson/nonstd/string_view.hpp */
// Copyright 2017-2019 by Martin Moene
//
// string-view lite, a C++17-like string_view for C++98 and later.
// For more information see https://github.com/martinmoene/string-view-lite
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#ifndef NONSTD_SV_LITE_H_INCLUDED
#define NONSTD_SV_LITE_H_INCLUDED

#define string_view_lite_MAJOR  1
#define string_view_lite_MINOR  4
#define string_view_lite_PATCH  0

#define string_view_lite_VERSION  nssv_STRINGIFY(string_view_lite_MAJOR) "." nssv_STRINGIFY(string_view_lite_MINOR) "." nssv_STRINGIFY(string_view_lite_PATCH)

#define nssv_STRINGIFY(  x )  nssv_STRINGIFY_( x )
#define nssv_STRINGIFY_( x )  #x

// string-view lite configuration:

#define nssv_STRING_VIEW_DEFAULT  0
#define nssv_STRING_VIEW_NONSTD   1
#define nssv_STRING_VIEW_STD      2

#if !defined( nssv_CONFIG_SELECT_STRING_VIEW )
# define nssv_CONFIG_SELECT_STRING_VIEW  ( nssv_HAVE_STD_STRING_VIEW ? nssv_STRING_VIEW_STD : nssv_STRING_VIEW_NONSTD )
#endif

#if defined( nssv_CONFIG_SELECT_STD_STRING_VIEW ) || defined( nssv_CONFIG_SELECT_NONSTD_STRING_VIEW )
# error nssv_CONFIG_SELECT_STD_STRING_VIEW and nssv_CONFIG_SELECT_NONSTD_STRING_VIEW are deprecated and removed, please use nssv_CONFIG_SELECT_STRING_VIEW=nssv_STRING_VIEW_...
#endif

#ifndef  nssv_CONFIG_STD_SV_OPERATOR
# define nssv_CONFIG_STD_SV_OPERATOR  0
#endif

#ifndef  nssv_CONFIG_USR_SV_OPERATOR
# define nssv_CONFIG_USR_SV_OPERATOR  1
#endif

#ifdef   nssv_CONFIG_CONVERSION_STD_STRING
# define nssv_CONFIG_CONVERSION_STD_STRING_CLASS_METHODS   nssv_CONFIG_CONVERSION_STD_STRING
# define nssv_CONFIG_CONVERSION_STD_STRING_FREE_FUNCTIONS  nssv_CONFIG_CONVERSION_STD_STRING
#endif

#ifndef  nssv_CONFIG_CONVERSION_STD_STRING_CLASS_METHODS
# define nssv_CONFIG_CONVERSION_STD_STRING_CLASS_METHODS  1
#endif

#ifndef  nssv_CONFIG_CONVERSION_STD_STRING_FREE_FUNCTIONS
# define nssv_CONFIG_CONVERSION_STD_STRING_FREE_FUNCTIONS  1
#endif

// Control presence of exception handling (try and auto discover):

#ifndef nssv_CONFIG_NO_EXCEPTIONS
# if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)
#  define nssv_CONFIG_NO_EXCEPTIONS  0
# else
#  define nssv_CONFIG_NO_EXCEPTIONS  1
# endif
#endif

// C++ language version detection (C++20 is speculative):
// Note: VC14.0/1900 (VS2015) lacks too much from C++14.

#ifndef   nssv_CPLUSPLUS
# if defined(_MSVC_LANG ) && !defined(__clang__)
#  define nssv_CPLUSPLUS  (_MSC_VER == 1900 ? 201103L : _MSVC_LANG )
# else
#  define nssv_CPLUSPLUS  __cplusplus
# endif
#endif

#define nssv_CPP98_OR_GREATER  ( nssv_CPLUSPLUS >= 199711L )
#define nssv_CPP11_OR_GREATER  ( nssv_CPLUSPLUS >= 201103L )
#define nssv_CPP11_OR_GREATER_ ( nssv_CPLUSPLUS >= 201103L )
#define nssv_CPP14_OR_GREATER  ( nssv_CPLUSPLUS >= 201402L )
#define nssv_CPP17_OR_GREATER  ( nssv_CPLUSPLUS >= 201703L )
#define nssv_CPP20_OR_GREATER  ( nssv_CPLUSPLUS >= 202000L )

// use C++17 std::string_view if available and requested:

#if nssv_CPP17_OR_GREATER && defined(__has_include )
# if __has_include( <string_view> )
#  define nssv_HAVE_STD_STRING_VIEW  1
# else
#  define nssv_HAVE_STD_STRING_VIEW  0
# endif
#else
# define  nssv_HAVE_STD_STRING_VIEW  0
#endif

#define  nssv_USES_STD_STRING_VIEW  ( (nssv_CONFIG_SELECT_STRING_VIEW == nssv_STRING_VIEW_STD) || ((nssv_CONFIG_SELECT_STRING_VIEW == nssv_STRING_VIEW_DEFAULT) && nssv_HAVE_STD_STRING_VIEW) )

#define nssv_HAVE_STARTS_WITH ( nssv_CPP20_OR_GREATER || !nssv_USES_STD_STRING_VIEW )
#define nssv_HAVE_ENDS_WITH     nssv_HAVE_STARTS_WITH

//
// Use C++17 std::string_view:
//

#if nssv_USES_STD_STRING_VIEW

#include <string_view>

// Extensions for std::string:

#if nssv_CONFIG_CONVERSION_STD_STRING_FREE_FUNCTIONS

namespace nonstd {

template< class CharT, class Traits, class Allocator = std::allocator<CharT> >
std::basic_string<CharT, Traits, Allocator>
to_string( std::basic_string_view<CharT, Traits> v, Allocator const & a = Allocator() )
{
    return std::basic_string<CharT,Traits, Allocator>( v.begin(), v.end(), a );
}

template< class CharT, class Traits, class Allocator >
std::basic_string_view<CharT, Traits>
to_string_view( std::basic_string<CharT, Traits, Allocator> const & s )
{
    return std::basic_string_view<CharT, Traits>( s.data(), s.size() );
}

// Literal operators sv and _sv:

#if nssv_CONFIG_STD_SV_OPERATOR

using namespace std::literals::string_view_literals;

#endif

#if nssv_CONFIG_USR_SV_OPERATOR

inline namespace literals {
inline namespace string_view_literals {


constexpr std::string_view operator "" _sv( const char* str, size_t len ) noexcept  // (1)
{
    return std::string_view{ str, len };
}

constexpr std::u16string_view operator "" _sv( const char16_t* str, size_t len ) noexcept  // (2)
{
    return std::u16string_view{ str, len };
}

constexpr std::u32string_view operator "" _sv( const char32_t* str, size_t len ) noexcept  // (3)
{
    return std::u32string_view{ str, len };
}

constexpr std::wstring_view operator "" _sv( const wchar_t* str, size_t len ) noexcept  // (4)
{
    return std::wstring_view{ str, len };
}

}} // namespace literals::string_view_literals

#endif // nssv_CONFIG_USR_SV_OPERATOR

} // namespace nonstd

#endif // nssv_CONFIG_CONVERSION_STD_STRING_FREE_FUNCTIONS

namespace nonstd {

using std::string_view;
using std::wstring_view;
using std::u16string_view;
using std::u32string_view;
using std::basic_string_view;

// literal "sv" and "_sv", see above

using std::operator==;
using std::operator!=;
using std::operator<;
using std::operator<=;
using std::operator>;
using std::operator>=;

using std::operator<<;

} // namespace nonstd

#else // nssv_HAVE_STD_STRING_VIEW

//
// Before C++17: use string_view lite:
//

// Compiler versions:
//
// MSVC++  6.0  _MSC_VER == 1200  nssv_COMPILER_MSVC_VERSION ==  60  (Visual Studio 6.0)
// MSVC++  7.0  _MSC_VER == 1300  nssv_COMPILER_MSVC_VERSION ==  70  (Visual Studio .NET 2002)
// MSVC++  7.1  _MSC_VER == 1310  nssv_COMPILER_MSVC_VERSION ==  71  (Visual Studio .NET 2003)
// MSVC++  8.0  _MSC_VER == 1400  nssv_COMPILER_MSVC_VERSION ==  80  (Visual Studio 2005)
// MSVC++  9.0  _MSC_VER == 1500  nssv_COMPILER_MSVC_VERSION ==  90  (Visual Studio 2008)
// MSVC++ 10.0  _MSC_VER == 1600  nssv_COMPILER_MSVC_VERSION == 100  (Visual Studio 2010)
// MSVC++ 11.0  _MSC_VER == 1700  nssv_COMPILER_MSVC_VERSION == 110  (Visual Studio 2012)
// MSVC++ 12.0  _MSC_VER == 1800  nssv_COMPILER_MSVC_VERSION == 120  (Visual Studio 2013)
// MSVC++ 14.0  _MSC_VER == 1900  nssv_COMPILER_MSVC_VERSION == 140  (Visual Studio 2015)
// MSVC++ 14.1  _MSC_VER >= 1910  nssv_COMPILER_MSVC_VERSION == 141  (Visual Studio 2017)
// MSVC++ 14.2  _MSC_VER >= 1920  nssv_COMPILER_MSVC_VERSION == 142  (Visual Studio 2019)

#if defined(_MSC_VER ) && !defined(__clang__)
# define nssv_COMPILER_MSVC_VER      (_MSC_VER )
# define nssv_COMPILER_MSVC_VERSION  (_MSC_VER / 10 - 10 * ( 5 + (_MSC_VER < 1900 ) ) )
#else
# define nssv_COMPILER_MSVC_VER      0
# define nssv_COMPILER_MSVC_VERSION  0
#endif

#define nssv_COMPILER_VERSION( major, minor, patch )  ( 10 * ( 10 * (major) + (minor) ) + (patch) )

#if defined(__clang__)
# define nssv_COMPILER_CLANG_VERSION  nssv_COMPILER_VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__)
#else
# define nssv_COMPILER_CLANG_VERSION    0
#endif

#if defined(__GNUC__) && !defined(__clang__)
# define nssv_COMPILER_GNUC_VERSION  nssv_COMPILER_VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#else
# define nssv_COMPILER_GNUC_VERSION    0
#endif

// half-open range [lo..hi):
#define nssv_BETWEEN( v, lo, hi ) ( (lo) <= (v) && (v) < (hi) )

// Presence of language and library features:

#ifdef _HAS_CPP0X
# define nssv_HAS_CPP0X  _HAS_CPP0X
#else
# define nssv_HAS_CPP0X  0
#endif

// Unless defined otherwise below, consider VC14 as C++11 for variant-lite:

#if nssv_COMPILER_MSVC_VER >= 1900
# undef  nssv_CPP11_OR_GREATER
# define nssv_CPP11_OR_GREATER  1
#endif

#define nssv_CPP11_90   (nssv_CPP11_OR_GREATER_ || nssv_COMPILER_MSVC_VER >= 1500)
#define nssv_CPP11_100  (nssv_CPP11_OR_GREATER_ || nssv_COMPILER_MSVC_VER >= 1600)
#define nssv_CPP11_110  (nssv_CPP11_OR_GREATER_ || nssv_COMPILER_MSVC_VER >= 1700)
#define nssv_CPP11_120  (nssv_CPP11_OR_GREATER_ || nssv_COMPILER_MSVC_VER >= 1800)
#define nssv_CPP11_140  (nssv_CPP11_OR_GREATER_ || nssv_COMPILER_MSVC_VER >= 1900)
#define nssv_CPP11_141  (nssv_CPP11_OR_GREATER_ || nssv_COMPILER_MSVC_VER >= 1910)

#define nssv_CPP14_000  (nssv_CPP14_OR_GREATER)
#define nssv_CPP17_000  (nssv_CPP17_OR_GREATER)

// Presence of C++11 language features:

#define nssv_HAVE_CONSTEXPR_11          nssv_CPP11_140
#define nssv_HAVE_EXPLICIT_CONVERSION   nssv_CPP11_140
#define nssv_HAVE_INLINE_NAMESPACE      nssv_CPP11_140
#define nssv_HAVE_NOEXCEPT              nssv_CPP11_140
#define nssv_HAVE_NULLPTR               nssv_CPP11_100
#define nssv_HAVE_REF_QUALIFIER         nssv_CPP11_140
#define nssv_HAVE_UNICODE_LITERALS      nssv_CPP11_140
#define nssv_HAVE_USER_DEFINED_LITERALS nssv_CPP11_140
#define nssv_HAVE_WCHAR16_T             nssv_CPP11_100
#define nssv_HAVE_WCHAR32_T             nssv_CPP11_100

#if ! ( ( nssv_CPP11_OR_GREATER && nssv_COMPILER_CLANG_VERSION ) || nssv_BETWEEN( nssv_COMPILER_CLANG_VERSION, 300, 400 ) )
# define nssv_HAVE_STD_DEFINED_LITERALS  nssv_CPP11_140
#else
# define nssv_HAVE_STD_DEFINED_LITERALS  0
#endif

// Presence of C++14 language features:

#define nssv_HAVE_CONSTEXPR_14          nssv_CPP14_000

// Presence of C++17 language features:

#define nssv_HAVE_NODISCARD             nssv_CPP17_000

// Presence of C++ library features:

#define nssv_HAVE_STD_HASH              nssv_CPP11_120

// C++ feature usage:

#if nssv_HAVE_CONSTEXPR_11
# define nssv_constexpr  constexpr
#else
# define nssv_constexpr  /*constexpr*/
#endif

#if  nssv_HAVE_CONSTEXPR_14
# define nssv_constexpr14  constexpr
#else
# define nssv_constexpr14  /*constexpr*/
#endif

#if nssv_HAVE_EXPLICIT_CONVERSION
# define nssv_explicit  explicit
#else
# define nssv_explicit  /*explicit*/
#endif

#if nssv_HAVE_INLINE_NAMESPACE
# define nssv_inline_ns  inline
#else
# define nssv_inline_ns  /*inline*/
#endif

#if nssv_HAVE_NOEXCEPT
# define nssv_noexcept  noexcept
#else
# define nssv_noexcept  /*noexcept*/
#endif

//#if nssv_HAVE_REF_QUALIFIER
//# define nssv_ref_qual  &
//# define nssv_refref_qual  &&
//#else
//# define nssv_ref_qual  /*&*/
//# define nssv_refref_qual  /*&&*/
//#endif

#if nssv_HAVE_NULLPTR
# define nssv_nullptr  nullptr
#else
# define nssv_nullptr  NULL
#endif

#if nssv_HAVE_NODISCARD
# define nssv_nodiscard  [[nodiscard]]
#else
# define nssv_nodiscard  /*[[nodiscard]]*/
#endif

// Additional includes:

#include <algorithm>
#include <cassert>
#include <iterator>
#include <limits>
#include <ostream>
#include <string>   // std::char_traits<>

#if ! nssv_CONFIG_NO_EXCEPTIONS
# include <stdexcept>
#endif

#if nssv_CPP11_OR_GREATER
# include <type_traits>
#endif

// Clang, GNUC, MSVC warning suppression macros:

#if defined(__clang__)
# pragma clang diagnostic ignored "-Wreserved-user-defined-literal"
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wuser-defined-literals"
#elif defined(__GNUC__)
# pragma  GCC  diagnostic push
# pragma  GCC  diagnostic ignored "-Wliteral-suffix"
#endif // __clang__

#if nssv_COMPILER_MSVC_VERSION >= 140
# define nssv_SUPPRESS_MSGSL_WARNING(expr)        [[gsl::suppress(expr)]]
# define nssv_SUPPRESS_MSVC_WARNING(code, descr)  __pragma(warning(suppress: code) )
# define nssv_DISABLE_MSVC_WARNINGS(codes)        __pragma(warning(push))  __pragma(warning(disable: codes))
#else
# define nssv_SUPPRESS_MSGSL_WARNING(expr)
# define nssv_SUPPRESS_MSVC_WARNING(code, descr)
# define nssv_DISABLE_MSVC_WARNINGS(codes)
#endif

#if defined(__clang__)
# define nssv_RESTORE_WARNINGS()  _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
# define nssv_RESTORE_WARNINGS()  _Pragma("GCC diagnostic pop")
#elif nssv_COMPILER_MSVC_VERSION >= 140
# define nssv_RESTORE_WARNINGS()  __pragma(warning(pop ))
#else
# define nssv_RESTORE_WARNINGS()
#endif

// Suppress the following MSVC (GSL) warnings:
// - C4455, non-gsl   : 'operator ""sv': literal suffix identifiers that do not
//                      start with an underscore are reserved
// - C26472, gsl::t.1 : don't use a static_cast for arithmetic conversions;
//                      use brace initialization, gsl::narrow_cast or gsl::narow
// - C26481: gsl::b.1 : don't use pointer arithmetic. Use span instead

nssv_DISABLE_MSVC_WARNINGS( 4455 26481 26472 )
//nssv_DISABLE_CLANG_WARNINGS( "-Wuser-defined-literals" )
//nssv_DISABLE_GNUC_WARNINGS( -Wliteral-suffix )

namespace nonstd { namespace sv_lite {

#if nssv_CPP11_OR_GREATER

namespace detail {

#if nssv_CPP14_OR_GREATER

template< typename CharT >
inline constexpr std::size_t length( CharT * s, std::size_t result = 0 )
{
    CharT * v = s;
    std::size_t r = result;
    while ( *v != '\0' ) {
       ++v;
       ++r;
    }
    return r;
}

#else // nssv_CPP14_OR_GREATER

// Expect tail call optimization to make length() non-recursive:

template< typename CharT >
inline constexpr std::size_t length( CharT * s, std::size_t result = 0 )
{
    return *s == '\0' ? result : length( s + 1, result + 1 );
}

#endif // nssv_CPP14_OR_GREATER

} // namespace detail

#endif // nssv_CPP11_OR_GREATER

template
<
    class CharT,
    class Traits = std::char_traits<CharT>
>
class basic_string_view;

//
// basic_string_view:
//

template
<
    class CharT,
    class Traits /* = std::char_traits<CharT> */
>
class basic_string_view
{
public:
    // Member types:

    typedef Traits traits_type;
    typedef CharT  value_type;

    typedef CharT       * pointer;
    typedef CharT const * const_pointer;
    typedef CharT       & reference;
    typedef CharT const & const_reference;

    typedef const_pointer iterator;
    typedef const_pointer const_iterator;
    typedef std::reverse_iterator< const_iterator > reverse_iterator;
    typedef	std::reverse_iterator< const_iterator > const_reverse_iterator;

    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;

    // 24.4.2.1 Construction and assignment:

    nssv_constexpr basic_string_view() nssv_noexcept
        : data_( nssv_nullptr )
        , size_( 0 )
    {}

#if nssv_CPP11_OR_GREATER
    nssv_constexpr basic_string_view( basic_string_view const & other ) nssv_noexcept = default;
#else
    nssv_constexpr basic_string_view( basic_string_view const & other ) nssv_noexcept
        : data_( other.data_)
        , size_( other.size_)
    {}
#endif

    nssv_constexpr basic_string_view( CharT const * s, size_type count ) nssv_noexcept // non-standard noexcept
        : data_( s )
        , size_( count )
    {}

    nssv_constexpr basic_string_view( CharT const * s) nssv_noexcept // non-standard noexcept
        : data_( s )
#if nssv_CPP17_OR_GREATER
        , size_( Traits::length(s) )
#elif nssv_CPP11_OR_GREATER
        , size_( detail::length(s) )
#else
        , size_( Traits::length(s) )
#endif
    {}

    // Assignment:

#if nssv_CPP11_OR_GREATER
    nssv_constexpr14 basic_string_view & operator=( basic_string_view const & other ) nssv_noexcept = default;
#else
    nssv_constexpr14 basic_string_view & operator=( basic_string_view const & other ) nssv_noexcept
    {
        data_ = other.data_;
        size_ = other.size_;
        return *this;
    }
#endif

    // 24.4.2.2 Iterator support:

    nssv_constexpr const_iterator begin()  const nssv_noexcept { return data_;         }
    nssv_constexpr const_iterator end()    const nssv_noexcept { return data_ + size_; }

    nssv_constexpr const_iterator cbegin() const nssv_noexcept { return begin(); }
    nssv_constexpr const_iterator cend()   const nssv_noexcept { return end();   }

    nssv_constexpr const_reverse_iterator rbegin()  const nssv_noexcept { return const_reverse_iterator( end() );   }
    nssv_constexpr const_reverse_iterator rend()    const nssv_noexcept { return const_reverse_iterator( begin() ); }

    nssv_constexpr const_reverse_iterator crbegin() const nssv_noexcept { return rbegin(); }
    nssv_constexpr const_reverse_iterator crend()   const nssv_noexcept { return rend();   }

    // 24.4.2.3 Capacity:

    nssv_constexpr size_type size()     const nssv_noexcept { return size_; }
    nssv_constexpr size_type length()   const nssv_noexcept { return size_; }
    nssv_constexpr size_type max_size() const nssv_noexcept { return (std::numeric_limits< size_type >::max)(); }

    // since C++20
    nssv_nodiscard nssv_constexpr bool empty() const nssv_noexcept
    {
        return 0 == size_;
    }

    // 24.4.2.4 Element access:

    nssv_constexpr const_reference operator[]( size_type pos ) const
    {
        return data_at( pos );
    }

    nssv_constexpr14 const_reference at( size_type pos ) const
    {
#if nssv_CONFIG_NO_EXCEPTIONS
        assert( pos < size() );
#else
        if ( pos >= size() )
        {
            throw std::out_of_range("nonstd::string_view::at()");
        }
#endif
        return data_at( pos );
    }

    nssv_constexpr const_reference front() const { return data_at( 0 );          }
    nssv_constexpr const_reference back()  const { return data_at( size() - 1 ); }

    nssv_constexpr const_pointer   data()  const nssv_noexcept { return data_; }

    // 24.4.2.5 Modifiers:

    nssv_constexpr14 void remove_prefix( size_type n )
    {
        assert( n <= size() );
        data_ += n;
        size_ -= n;
    }

    nssv_constexpr14 void remove_suffix( size_type n )
    {
        assert( n <= size() );
        size_ -= n;
    }

    nssv_constexpr14 void swap( basic_string_view & other ) nssv_noexcept
    {
        using std::swap;
        swap( data_, other.data_ );
        swap( size_, other.size_ );
    }

    // 24.4.2.6 String operations:

    size_type copy( CharT * dest, size_type n, size_type pos = 0 ) const
    {
#if nssv_CONFIG_NO_EXCEPTIONS
        assert( pos <= size() );
#else
        if ( pos > size() )
        {
            throw std::out_of_range("nonstd::string_view::copy()");
        }
#endif
        const size_type rlen = (std::min)( n, size() - pos );

        (void) Traits::copy( dest, data() + pos, rlen );

        return rlen;
    }

    nssv_constexpr14 basic_string_view substr( size_type pos = 0, size_type n = npos ) const
    {
#if nssv_CONFIG_NO_EXCEPTIONS
        assert( pos <= size() );
#else
        if ( pos > size() )
        {
            throw std::out_of_range("nonstd::string_view::substr()");
        }
#endif
        return basic_string_view( data() + pos, (std::min)( n, size() - pos ) );
    }

    // compare(), 6x:

    nssv_constexpr14 int compare( basic_string_view other ) const nssv_noexcept // (1)
    {
        if ( const int result = Traits::compare( data(), other.data(), (std::min)( size(), other.size() ) ) )
        {
            return result;
        }

        return size() == other.size() ? 0 : size() < other.size() ? -1 : 1;
    }

    nssv_constexpr int compare( size_type pos1, size_type n1, basic_string_view other ) const // (2)
    {
        return substr( pos1, n1 ).compare( other );
    }

    nssv_constexpr int compare( size_type pos1, size_type n1, basic_string_view other, size_type pos2, size_type n2 ) const // (3)
    {
        return substr( pos1, n1 ).compare( other.substr( pos2, n2 ) );
    }

    nssv_constexpr int compare( CharT const * s ) const // (4)
    {
        return compare( basic_string_view( s ) );
    }

    nssv_constexpr int compare( size_type pos1, size_type n1, CharT const * s ) const // (5)
    {
        return substr( pos1, n1 ).compare( basic_string_view( s ) );
    }

    nssv_constexpr int compare( size_type pos1, size_type n1, CharT const * s, size_type n2 ) const // (6)
    {
        return substr( pos1, n1 ).compare( basic_string_view( s, n2 ) );
    }

    // 24.4.2.7 Searching:

    // starts_with(), 3x, since C++20:

    nssv_constexpr bool starts_with( basic_string_view v ) const nssv_noexcept  // (1)
    {
        return size() >= v.size() && compare( 0, v.size(), v ) == 0;
    }

    nssv_constexpr bool starts_with( CharT c ) const nssv_noexcept  // (2)
    {
        return starts_with( basic_string_view( &c, 1 ) );
    }

    nssv_constexpr bool starts_with( CharT const * s ) const  // (3)
    {
        return starts_with( basic_string_view( s ) );
    }

    // ends_with(), 3x, since C++20:

    nssv_constexpr bool ends_with( basic_string_view v ) const nssv_noexcept  // (1)
    {
        return size() >= v.size() && compare( size() - v.size(), npos, v ) == 0;
    }

    nssv_constexpr bool ends_with( CharT c ) const nssv_noexcept  // (2)
    {
        return ends_with( basic_string_view( &c, 1 ) );
    }

    nssv_constexpr bool ends_with( CharT const * s ) const  // (3)
    {
        return ends_with( basic_string_view( s ) );
    }

    // find(), 4x:

    nssv_constexpr14 size_type find( basic_string_view v, size_type pos = 0 ) const nssv_noexcept  // (1)
    {
        return assert( v.size() == 0 || v.data() != nssv_nullptr )
            , pos >= size()
            ? npos
            : to_pos( std::search( cbegin() + pos, cend(), v.cbegin(), v.cend(), Traits::eq ) );
    }

    nssv_constexpr14 size_type find( CharT c, size_type pos = 0 ) const nssv_noexcept  // (2)
    {
        return find( basic_string_view( &c, 1 ), pos );
    }

    nssv_constexpr14 size_type find( CharT const * s, size_type pos, size_type n ) const  // (3)
    {
        return find( basic_string_view( s, n ), pos );
    }

    nssv_constexpr14 size_type find( CharT const * s, size_type pos = 0 ) const  // (4)
    {
        return find( basic_string_view( s ), pos );
    }

    // rfind(), 4x:

    nssv_constexpr14 size_type rfind( basic_string_view v, size_type pos = npos ) const nssv_noexcept  // (1)
    {
        if ( size() < v.size() )
        {
            return npos;
        }

        if ( v.empty() )
        {
            return (std::min)( size(), pos );
        }

        const_iterator last   = cbegin() + (std::min)( size() - v.size(), pos ) + v.size();
        const_iterator result = std::find_end( cbegin(), last, v.cbegin(), v.cend(), Traits::eq );

        return result != last ? size_type( result - cbegin() ) : npos;
    }

    nssv_constexpr14 size_type rfind( CharT c, size_type pos = npos ) const nssv_noexcept  // (2)
    {
        return rfind( basic_string_view( &c, 1 ), pos );
    }

    nssv_constexpr14 size_type rfind( CharT const * s, size_type pos, size_type n ) const  // (3)
    {
        return rfind( basic_string_view( s, n ), pos );
    }

    nssv_constexpr14 size_type rfind( CharT const * s, size_type pos = npos ) const  // (4)
    {
        return rfind( basic_string_view( s ), pos );
    }

    // find_first_of(), 4x:

    nssv_constexpr size_type find_first_of( basic_string_view v, size_type pos = 0 ) const nssv_noexcept  // (1)
    {
        return pos >= size()
            ? npos
            : to_pos( std::find_first_of( cbegin() + pos, cend(), v.cbegin(), v.cend(), Traits::eq ) );
    }

    nssv_constexpr size_type find_first_of( CharT c, size_type pos = 0 ) const nssv_noexcept  // (2)
    {
        return find_first_of( basic_string_view( &c, 1 ), pos );
    }

    nssv_constexpr size_type find_first_of( CharT const * s, size_type pos, size_type n ) const  // (3)
    {
        return find_first_of( basic_string_view( s, n ), pos );
    }

    nssv_constexpr size_type find_first_of(  CharT const * s, size_type pos = 0 ) const  // (4)
    {
        return find_first_of( basic_string_view( s ), pos );
    }

    // find_last_of(), 4x:

    nssv_constexpr size_type find_last_of( basic_string_view v, size_type pos = npos ) const nssv_noexcept  // (1)
    {
        return empty()
            ? npos
            : pos >= size()
            ? find_last_of( v, size() - 1 )
            : to_pos( std::find_first_of( const_reverse_iterator( cbegin() + pos + 1 ), crend(), v.cbegin(), v.cend(), Traits::eq ) );
    }

    nssv_constexpr size_type find_last_of( CharT c, size_type pos = npos ) const nssv_noexcept  // (2)
    {
        return find_last_of( basic_string_view( &c, 1 ), pos );
    }

    nssv_constexpr size_type find_last_of( CharT const * s, size_type pos, size_type count ) const  // (3)
    {
        return find_last_of( basic_string_view( s, count ), pos );
    }

    nssv_constexpr size_type find_last_of( CharT const * s, size_type pos = npos ) const  // (4)
    {
        return find_last_of( basic_string_view( s ), pos );
    }

    // find_first_not_of(), 4x:

    nssv_constexpr size_type find_first_not_of( basic_string_view v, size_type pos = 0 ) const nssv_noexcept  // (1)
    {
        return pos >= size()
            ? npos
            : to_pos( std::find_if( cbegin() + pos, cend(), not_in_view( v ) ) );
    }

    nssv_constexpr size_type find_first_not_of( CharT c, size_type pos = 0 ) const nssv_noexcept  // (2)
    {
        return find_first_not_of( basic_string_view( &c, 1 ), pos );
    }

    nssv_constexpr size_type find_first_not_of( CharT const * s, size_type pos, size_type count ) const  // (3)
    {
        return find_first_not_of( basic_string_view( s, count ), pos );
    }

    nssv_constexpr size_type find_first_not_of( CharT const * s, size_type pos = 0 ) const  // (4)
    {
        return find_first_not_of( basic_string_view( s ), pos );
    }

    // find_last_not_of(), 4x:

    nssv_constexpr size_type find_last_not_of( basic_string_view v, size_type pos = npos ) const nssv_noexcept  // (1)
    {
        return empty()
            ? npos
            : pos >= size()
            ? find_last_not_of( v, size() - 1 )
            : to_pos( std::find_if( const_reverse_iterator( cbegin() + pos + 1 ), crend(), not_in_view( v ) ) );
    }

    nssv_constexpr size_type find_last_not_of( CharT c, size_type pos = npos ) const nssv_noexcept  // (2)
    {
        return find_last_not_of( basic_string_view( &c, 1 ), pos );
    }

    nssv_constexpr size_type find_last_not_of( CharT const * s, size_type pos, size_type count ) const  // (3)
    {
        return find_last_not_of( basic_string_view( s, count ), pos );
    }

    nssv_constexpr size_type find_last_not_of( CharT const * s, size_type pos = npos ) const  // (4)
    {
        return find_last_not_of( basic_string_view( s ), pos );
    }

    // Constants:

#if nssv_CPP17_OR_GREATER
    static nssv_constexpr size_type npos = size_type(-1);
#elif nssv_CPP11_OR_GREATER
    enum : size_type { npos = size_type(-1) };
#else
    enum { npos = size_type(-1) };
#endif

private:
    struct not_in_view
    {
        const basic_string_view v;

        nssv_constexpr explicit not_in_view( basic_string_view v ) : v( v ) {}

        nssv_constexpr bool operator()( CharT c ) const
        {
            return npos == v.find_first_of( c );
        }
    };

    nssv_constexpr size_type to_pos( const_iterator it ) const
    {
        return it == cend() ? npos : size_type( it - cbegin() );
    }

    nssv_constexpr size_type to_pos( const_reverse_iterator it ) const
    {
        return it == crend() ? npos : size_type( crend() - it - 1 );
    }

    nssv_constexpr const_reference data_at( size_type pos ) const
    {
#if nssv_BETWEEN( nssv_COMPILER_GNUC_VERSION, 1, 500 )
        return data_[pos];
#else
        return assert( pos < size() ), data_[pos];
#endif
    }

private:
    const_pointer data_;
    size_type     size_;

public:
#if nssv_CONFIG_CONVERSION_STD_STRING_CLASS_METHODS

    template< class Allocator >
    basic_string_view( std::basic_string<CharT, Traits, Allocator> const & s ) nssv_noexcept
        : data_( s.data() )
        , size_( s.size() )
    {}

#if nssv_HAVE_EXPLICIT_CONVERSION

    template< class Allocator >
    explicit operator std::basic_string<CharT, Traits, Allocator>() const
    {
        return to_string( Allocator() );
    }

#endif // nssv_HAVE_EXPLICIT_CONVERSION

#if nssv_CPP11_OR_GREATER

    template< class Allocator = std::allocator<CharT> >
    std::basic_string<CharT, Traits, Allocator>
    to_string( Allocator const & a = Allocator() ) const
    {
        return std::basic_string<CharT, Traits, Allocator>( begin(), end(), a );
    }

#else

    std::basic_string<CharT, Traits>
    to_string() const
    {
        return std::basic_string<CharT, Traits>( begin(), end() );
    }

    template< class Allocator >
    std::basic_string<CharT, Traits, Allocator>
    to_string( Allocator const & a ) const
    {
        return std::basic_string<CharT, Traits, Allocator>( begin(), end(), a );
    }

#endif // nssv_CPP11_OR_GREATER

#endif // nssv_CONFIG_CONVERSION_STD_STRING_CLASS_METHODS
};

//
// Non-member functions:
//

// 24.4.3 Non-member comparison functions:
// lexicographically compare two string views (function template):

template< class CharT, class Traits >
nssv_constexpr bool operator== (
    basic_string_view <CharT, Traits> lhs,
    basic_string_view <CharT, Traits> rhs ) nssv_noexcept
{ return lhs.compare( rhs ) == 0 ; }

template< class CharT, class Traits >
nssv_constexpr bool operator!= (
    basic_string_view <CharT, Traits> lhs,
    basic_string_view <CharT, Traits> rhs ) nssv_noexcept
{ return lhs.compare( rhs ) != 0 ; }

template< class CharT, class Traits >
nssv_constexpr bool operator< (
    basic_string_view <CharT, Traits> lhs,
    basic_string_view <CharT, Traits> rhs ) nssv_noexcept
{ return lhs.compare( rhs ) < 0 ; }

template< class CharT, class Traits >
nssv_constexpr bool operator<= (
    basic_string_view <CharT, Traits> lhs,
    basic_string_view <CharT, Traits> rhs ) nssv_noexcept
{ return lhs.compare( rhs ) <= 0 ; }

template< class CharT, class Traits >
nssv_constexpr bool operator> (
    basic_string_view <CharT, Traits> lhs,
    basic_string_view <CharT, Traits> rhs ) nssv_noexcept
{ return lhs.compare( rhs ) > 0 ; }

template< class CharT, class Traits >
nssv_constexpr bool operator>= (
    basic_string_view <CharT, Traits> lhs,
    basic_string_view <CharT, Traits> rhs ) nssv_noexcept
{ return lhs.compare( rhs ) >= 0 ; }

// Let S be basic_string_view<CharT, Traits>, and sv be an instance of S.
// Implementations shall provide sufficient additional overloads marked
// constexpr and noexcept so that an object t with an implicit conversion
// to S can be compared according to Table 67.

#if ! nssv_CPP11_OR_GREATER || nssv_BETWEEN( nssv_COMPILER_MSVC_VERSION, 100, 141 )

// accomodate for older compilers:

// ==

template< class CharT, class Traits>
nssv_constexpr bool operator==(
    basic_string_view<CharT, Traits> lhs,
    char const * rhs ) nssv_noexcept
{ return lhs.compare( rhs ) == 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator==(
    char const * lhs,
    basic_string_view<CharT, Traits> rhs ) nssv_noexcept
{ return rhs.compare( lhs ) == 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator==(
    basic_string_view<CharT, Traits> lhs,
    std::basic_string<CharT, Traits> rhs ) nssv_noexcept
{ return lhs.size() == rhs.size() && lhs.compare( rhs ) == 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator==(
    std::basic_string<CharT, Traits> rhs,
    basic_string_view<CharT, Traits> lhs ) nssv_noexcept
{ return lhs.size() == rhs.size() && lhs.compare( rhs ) == 0; }

// !=

template< class CharT, class Traits>
nssv_constexpr bool operator!=(
    basic_string_view<CharT, Traits> lhs,
    char const * rhs ) nssv_noexcept
{ return lhs.compare( rhs ) != 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator!=(
    char const * lhs,
    basic_string_view<CharT, Traits> rhs ) nssv_noexcept
{ return rhs.compare( lhs ) != 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator!=(
    basic_string_view<CharT, Traits> lhs,
    std::basic_string<CharT, Traits> rhs ) nssv_noexcept
{ return lhs.size() != rhs.size() && lhs.compare( rhs ) != 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator!=(
    std::basic_string<CharT, Traits> rhs,
    basic_string_view<CharT, Traits> lhs ) nssv_noexcept
{ return lhs.size() != rhs.size() || rhs.compare( lhs ) != 0; }

// <

template< class CharT, class Traits>
nssv_constexpr bool operator<(
    basic_string_view<CharT, Traits> lhs,
    char const * rhs ) nssv_noexcept
{ return lhs.compare( rhs ) < 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator<(
    char const * lhs,
    basic_string_view<CharT, Traits> rhs ) nssv_noexcept
{ return rhs.compare( lhs ) > 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator<(
    basic_string_view<CharT, Traits> lhs,
    std::basic_string<CharT, Traits> rhs ) nssv_noexcept
{ return lhs.compare( rhs ) < 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator<(
    std::basic_string<CharT, Traits> rhs,
    basic_string_view<CharT, Traits> lhs ) nssv_noexcept
{ return rhs.compare( lhs ) > 0; }

// <=

template< class CharT, class Traits>
nssv_constexpr bool operator<=(
    basic_string_view<CharT, Traits> lhs,
    char const * rhs ) nssv_noexcept
{ return lhs.compare( rhs ) <= 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator<=(
    char const * lhs,
    basic_string_view<CharT, Traits> rhs ) nssv_noexcept
{ return rhs.compare( lhs ) >= 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator<=(
    basic_string_view<CharT, Traits> lhs,
    std::basic_string<CharT, Traits> rhs ) nssv_noexcept
{ return lhs.compare( rhs ) <= 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator<=(
    std::basic_string<CharT, Traits> rhs,
    basic_string_view<CharT, Traits> lhs ) nssv_noexcept
{ return rhs.compare( lhs ) >= 0; }

// >

template< class CharT, class Traits>
nssv_constexpr bool operator>(
    basic_string_view<CharT, Traits> lhs,
    char const * rhs ) nssv_noexcept
{ return lhs.compare( rhs ) > 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator>(
    char const * lhs,
    basic_string_view<CharT, Traits> rhs ) nssv_noexcept
{ return rhs.compare( lhs ) < 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator>(
    basic_string_view<CharT, Traits> lhs,
    std::basic_string<CharT, Traits> rhs ) nssv_noexcept
{ return lhs.compare( rhs ) > 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator>(
    std::basic_string<CharT, Traits> rhs,
    basic_string_view<CharT, Traits> lhs ) nssv_noexcept
{ return rhs.compare( lhs ) < 0; }

// >=

template< class CharT, class Traits>
nssv_constexpr bool operator>=(
    basic_string_view<CharT, Traits> lhs,
    char const * rhs ) nssv_noexcept
{ return lhs.compare( rhs ) >= 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator>=(
    char const * lhs,
    basic_string_view<CharT, Traits> rhs ) nssv_noexcept
{ return rhs.compare( lhs ) <= 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator>=(
    basic_string_view<CharT, Traits> lhs,
    std::basic_string<CharT, Traits> rhs ) nssv_noexcept
{ return lhs.compare( rhs ) >= 0; }

template< class CharT, class Traits>
nssv_constexpr bool operator>=(
    std::basic_string<CharT, Traits> rhs,
    basic_string_view<CharT, Traits> lhs ) nssv_noexcept
{ return rhs.compare( lhs ) <= 0; }

#else // newer compilers:

#define nssv_BASIC_STRING_VIEW_I(T,U)  typename std::decay< basic_string_view<T,U> >::type

#if nssv_BETWEEN( nssv_COMPILER_MSVC_VERSION, 140, 150 )
# define nssv_MSVC_ORDER(x)  , int=x
#else
# define nssv_MSVC_ORDER(x)  /*, int=x*/
#endif

// ==

template< class CharT, class Traits  nssv_MSVC_ORDER(1) >
nssv_constexpr bool operator==(
         basic_string_view  <CharT, Traits> lhs,
    nssv_BASIC_STRING_VIEW_I(CharT, Traits) rhs ) nssv_noexcept
{ return lhs.compare( rhs ) == 0; }

template< class CharT, class Traits  nssv_MSVC_ORDER(2) >
nssv_constexpr bool operator==(
    nssv_BASIC_STRING_VIEW_I(CharT, Traits) lhs,
         basic_string_view  <CharT, Traits> rhs ) nssv_noexcept
{ return lhs.size() == rhs.size() && lhs.compare( rhs ) == 0; }

// !=

template< class CharT, class Traits  nssv_MSVC_ORDER(1) >
nssv_constexpr bool operator!= (
         basic_string_view  < CharT, Traits > lhs,
    nssv_BASIC_STRING_VIEW_I( CharT, Traits ) rhs ) nssv_noexcept
{ return lhs.size() != rhs.size() || lhs.compare( rhs ) != 0 ; }

template< class CharT, class Traits  nssv_MSVC_ORDER(2) >
nssv_constexpr bool operator!= (
    nssv_BASIC_STRING_VIEW_I( CharT, Traits ) lhs,
         basic_string_view  < CharT, Traits > rhs ) nssv_noexcept
{ return lhs.compare( rhs ) != 0 ; }

// <

template< class CharT, class Traits  nssv_MSVC_ORDER(1) >
nssv_constexpr bool operator< (
         basic_string_view  < CharT, Traits > lhs,
    nssv_BASIC_STRING_VIEW_I( CharT, Traits ) rhs ) nssv_noexcept
{ return lhs.compare( rhs ) < 0 ; }

template< class CharT, class Traits  nssv_MSVC_ORDER(2) >
nssv_constexpr bool operator< (
    nssv_BASIC_STRING_VIEW_I( CharT, Traits ) lhs,
         basic_string_view  < CharT, Traits > rhs ) nssv_noexcept
{ return lhs.compare( rhs ) < 0 ; }

// <=

template< class CharT, class Traits  nssv_MSVC_ORDER(1) >
nssv_constexpr bool operator<= (
         basic_string_view  < CharT, Traits > lhs,
    nssv_BASIC_STRING_VIEW_I( CharT, Traits ) rhs ) nssv_noexcept
{ return lhs.compare( rhs ) <= 0 ; }

template< class CharT, class Traits  nssv_MSVC_ORDER(2) >
nssv_constexpr bool operator<= (
    nssv_BASIC_STRING_VIEW_I( CharT, Traits ) lhs,
         basic_string_view  < CharT, Traits > rhs ) nssv_noexcept
{ return lhs.compare( rhs ) <= 0 ; }

// >

template< class CharT, class Traits  nssv_MSVC_ORDER(1) >
nssv_constexpr bool operator> (
         basic_string_view  < CharT, Traits > lhs,
    nssv_BASIC_STRING_VIEW_I( CharT, Traits ) rhs ) nssv_noexcept
{ return lhs.compare( rhs ) > 0 ; }

template< class CharT, class Traits  nssv_MSVC_ORDER(2) >
nssv_constexpr bool operator> (
    nssv_BASIC_STRING_VIEW_I( CharT, Traits ) lhs,
         basic_string_view  < CharT, Traits > rhs ) nssv_noexcept
{ return lhs.compare( rhs ) > 0 ; }

// >=

template< class CharT, class Traits  nssv_MSVC_ORDER(1) >
nssv_constexpr bool operator>= (
         basic_string_view  < CharT, Traits > lhs,
    nssv_BASIC_STRING_VIEW_I( CharT, Traits ) rhs ) nssv_noexcept
{ return lhs.compare( rhs ) >= 0 ; }

template< class CharT, class Traits  nssv_MSVC_ORDER(2) >
nssv_constexpr bool operator>= (
    nssv_BASIC_STRING_VIEW_I( CharT, Traits ) lhs,
         basic_string_view  < CharT, Traits > rhs ) nssv_noexcept
{ return lhs.compare( rhs ) >= 0 ; }

#undef nssv_MSVC_ORDER
#undef nssv_BASIC_STRING_VIEW_I

#endif // compiler-dependent approach to comparisons

// 24.4.4 Inserters and extractors:

namespace detail {

template< class Stream >
void write_padding( Stream & os, std::streamsize n )
{
    for ( std::streamsize i = 0; i < n; ++i )
        os.rdbuf()->sputc( os.fill() );
}

template< class Stream, class View >
Stream & write_to_stream( Stream & os, View const & sv )
{
    typename Stream::sentry sentry( os );

    if ( !os )
        return os;

    const std::streamsize length = static_cast<std::streamsize>( sv.length() );

    // Whether, and how, to pad:
    const bool      pad = ( length < os.width() );
    const bool left_pad = pad && ( os.flags() & std::ios_base::adjustfield ) == std::ios_base::right;

    if ( left_pad )
        write_padding( os, os.width() - length );

    // Write span characters:
    os.rdbuf()->sputn( sv.begin(), length );

    if ( pad && !left_pad )
        write_padding( os, os.width() - length );

    // Reset output stream width:
    os.width( 0 );

    return os;
}

} // namespace detail

template< class CharT, class Traits >
std::basic_ostream<CharT, Traits> &
operator<<(
    std::basic_ostream<CharT, Traits>& os,
    basic_string_view <CharT, Traits> sv )
{
    return detail::write_to_stream( os, sv );
}

// Several typedefs for common character types are provided:

typedef basic_string_view<char>      string_view;
typedef basic_string_view<wchar_t>   wstring_view;
#if nssv_HAVE_WCHAR16_T
typedef basic_string_view<char16_t>  u16string_view;
typedef basic_string_view<char32_t>  u32string_view;
#endif

}} // namespace nonstd::sv_lite

//
// 24.4.6 Suffix for basic_string_view literals:
//

#if nssv_HAVE_USER_DEFINED_LITERALS

namespace nonstd {
nssv_inline_ns namespace literals {
nssv_inline_ns namespace string_view_literals {

#if nssv_CONFIG_STD_SV_OPERATOR && nssv_HAVE_STD_DEFINED_LITERALS

nssv_constexpr nonstd::sv_lite::string_view operator "" sv( const char* str, size_t len ) nssv_noexcept  // (1)
{
    return nonstd::sv_lite::string_view{ str, len };
}

nssv_constexpr nonstd::sv_lite::u16string_view operator "" sv( const char16_t* str, size_t len ) nssv_noexcept  // (2)
{
    return nonstd::sv_lite::u16string_view{ str, len };
}

nssv_constexpr nonstd::sv_lite::u32string_view operator "" sv( const char32_t* str, size_t len ) nssv_noexcept  // (3)
{
    return nonstd::sv_lite::u32string_view{ str, len };
}

nssv_constexpr nonstd::sv_lite::wstring_view operator "" sv( const wchar_t* str, size_t len ) nssv_noexcept  // (4)
{
    return nonstd::sv_lite::wstring_view{ str, len };
}

#endif // nssv_CONFIG_STD_SV_OPERATOR && nssv_HAVE_STD_DEFINED_LITERALS

#if nssv_CONFIG_USR_SV_OPERATOR

nssv_constexpr nonstd::sv_lite::string_view operator "" _sv( const char* str, size_t len ) nssv_noexcept  // (1)
{
    return nonstd::sv_lite::string_view{ str, len };
}

nssv_constexpr nonstd::sv_lite::u16string_view operator "" _sv( const char16_t* str, size_t len ) nssv_noexcept  // (2)
{
    return nonstd::sv_lite::u16string_view{ str, len };
}

nssv_constexpr nonstd::sv_lite::u32string_view operator "" _sv( const char32_t* str, size_t len ) nssv_noexcept  // (3)
{
    return nonstd::sv_lite::u32string_view{ str, len };
}

nssv_constexpr nonstd::sv_lite::wstring_view operator "" _sv( const wchar_t* str, size_t len ) nssv_noexcept  // (4)
{
    return nonstd::sv_lite::wstring_view{ str, len };
}

#endif // nssv_CONFIG_USR_SV_OPERATOR

}}} // namespace nonstd::literals::string_view_literals

#endif

//
// Extensions for std::string:
//

#if nssv_CONFIG_CONVERSION_STD_STRING_FREE_FUNCTIONS

namespace nonstd {
namespace sv_lite {

// Exclude MSVC 14 (19.00): it yields ambiguous to_string():

#if nssv_CPP11_OR_GREATER && nssv_COMPILER_MSVC_VERSION != 140

template< class CharT, class Traits, class Allocator = std::allocator<CharT> >
std::basic_string<CharT, Traits, Allocator>
to_string( basic_string_view<CharT, Traits> v, Allocator const & a = Allocator() )
{
    return std::basic_string<CharT,Traits, Allocator>( v.begin(), v.end(), a );
}

#else

template< class CharT, class Traits >
std::basic_string<CharT, Traits>
to_string( basic_string_view<CharT, Traits> v )
{
    return std::basic_string<CharT, Traits>( v.begin(), v.end() );
}

template< class CharT, class Traits, class Allocator >
std::basic_string<CharT, Traits, Allocator>
to_string( basic_string_view<CharT, Traits> v, Allocator const & a )
{
    return std::basic_string<CharT, Traits, Allocator>( v.begin(), v.end(), a );
}

#endif // nssv_CPP11_OR_GREATER

template< class CharT, class Traits, class Allocator >
basic_string_view<CharT, Traits>
to_string_view( std::basic_string<CharT, Traits, Allocator> const & s )
{
    return basic_string_view<CharT, Traits>( s.data(), s.size() );
}

}} // namespace nonstd::sv_lite

#endif // nssv_CONFIG_CONVERSION_STD_STRING_FREE_FUNCTIONS

//
// make types and algorithms available in namespace nonstd:
//

namespace nonstd {

using sv_lite::basic_string_view;
using sv_lite::string_view;
using sv_lite::wstring_view;

#if nssv_HAVE_WCHAR16_T
using sv_lite::u16string_view;
#endif
#if nssv_HAVE_WCHAR32_T
using sv_lite::u32string_view;
#endif

// literal "sv"

using sv_lite::operator==;
using sv_lite::operator!=;
using sv_lite::operator<;
using sv_lite::operator<=;
using sv_lite::operator>;
using sv_lite::operator>=;

using sv_lite::operator<<;

#if nssv_CONFIG_CONVERSION_STD_STRING_FREE_FUNCTIONS
using sv_lite::to_string;
using sv_lite::to_string_view;
#endif

} // namespace nonstd

// 24.4.5 Hash support (C++11):

// Note: The hash value of a string view object is equal to the hash value of
// the corresponding string object.

#if nssv_HAVE_STD_HASH

#include <functional>

namespace std {

template<>
struct hash< nonstd::string_view >
{
public:
    std::size_t operator()( nonstd::string_view v ) const nssv_noexcept
    {
        return std::hash<std::string>()( std::string( v.data(), v.size() ) );
    }
};

template<>
struct hash< nonstd::wstring_view >
{
public:
    std::size_t operator()( nonstd::wstring_view v ) const nssv_noexcept
    {
        return std::hash<std::wstring>()( std::wstring( v.data(), v.size() ) );
    }
};

template<>
struct hash< nonstd::u16string_view >
{
public:
    std::size_t operator()( nonstd::u16string_view v ) const nssv_noexcept
    {
        return std::hash<std::u16string>()( std::u16string( v.data(), v.size() ) );
    }
};

template<>
struct hash< nonstd::u32string_view >
{
public:
    std::size_t operator()( nonstd::u32string_view v ) const nssv_noexcept
    {
        return std::hash<std::u32string>()( std::u32string( v.data(), v.size() ) );
    }
};

} // namespace std

#endif // nssv_HAVE_STD_HASH

nssv_RESTORE_WARNINGS()

#endif // nssv_HAVE_STD_STRING_VIEW
#endif // NONSTD_SV_LITE_H_INCLUDED
/* end file include/simdjson/nonstd/string_view.hpp */
SIMDJSON_POP_DISABLE_WARNINGS

namespace std {
  using string_view = nonstd::string_view;
}
#endif // SIMDJSON_HAS_STRING_VIEW
#undef SIMDJSON_HAS_STRING_VIEW // We are not going to need this macro anymore.

#endif // SIMDJSON_COMMON_DEFS_H
/* end file include/simdjson/nonstd/string_view.hpp */

SIMDJSON_PUSH_DISABLE_WARNINGS
SIMDJSON_DISABLE_UNDESIRED_WARNINGS

// Public API
/* begin file include/simdjson/simdjson_version.h */
// /include/simdjson/simdjson_version.h automatically generated by release.py,
// do not change by hand
#ifndef SIMDJSON_SIMDJSON_VERSION_H
#define SIMDJSON_SIMDJSON_VERSION_H

/** The version of simdjson being used (major.minor.revision) */
#define SIMDJSON_VERSION 0.4.7

namespace simdjson {
enum {
  /**
   * The major version (MAJOR.minor.revision) of simdjson being used.
   */
  SIMDJSON_VERSION_MAJOR = 0,
  /**
   * The minor version (major.MINOR.revision) of simdjson being used.
   */
  SIMDJSON_VERSION_MINOR = 4,
  /**
   * The revision (major.minor.REVISION) of simdjson being used.
   */
  SIMDJSON_VERSION_REVISION = 7
};
} // namespace simdjson

#endif // SIMDJSON_SIMDJSON_VERSION_H
/* end file include/simdjson/simdjson_version.h */
/* begin file include/simdjson/error.h */
#ifndef SIMDJSON_ERROR_H
#define SIMDJSON_ERROR_H

#include <string>

namespace simdjson {

/**
 * All possible errors returned by simdjson.
 */
enum error_code {
  SUCCESS = 0,              ///< No error
  CAPACITY,                 ///< This parser can't support a document that big
  MEMALLOC,                 ///< Error allocating memory, most likely out of memory
  TAPE_ERROR,               ///< Something went wrong while writing to the tape (stage 2), this is a generic error
  DEPTH_ERROR,              ///< Your document exceeds the user-specified depth limitation
  STRING_ERROR,             ///< Problem while parsing a string
  T_ATOM_ERROR,             ///< Problem while parsing an atom starting with the letter 't'
  F_ATOM_ERROR,             ///< Problem while parsing an atom starting with the letter 'f'
  N_ATOM_ERROR,             ///< Problem while parsing an atom starting with the letter 'n'
  NUMBER_ERROR,             ///< Problem while parsing a number
  UTF8_ERROR,               ///< the input is not valid UTF-8
  UNINITIALIZED,            ///< unknown error, or uninitialized document
  EMPTY,                    ///< no structural element found
  UNESCAPED_CHARS,          ///< found unescaped characters in a string.
  UNCLOSED_STRING,          ///< missing quote at the end
  UNSUPPORTED_ARCHITECTURE, ///< unsupported architecture
  INCORRECT_TYPE,           ///< JSON element has a different type than user expected
  NUMBER_OUT_OF_RANGE,      ///< JSON number does not fit in 64 bits
  INDEX_OUT_OF_BOUNDS,      ///< JSON array index too large
  NO_SUCH_FIELD,            ///< JSON field not found in object
  IO_ERROR,                 ///< Error reading a file
  INVALID_JSON_POINTER,     ///< Invalid JSON pointer reference
  INVALID_URI_FRAGMENT,     ///< Invalid URI fragment
  UNEXPECTED_ERROR,         ///< indicative of a bug in simdjson
  /** @private Number of error codes */
  NUM_ERROR_CODES
};

/**
 * Get the error message for the given error code.
 *
 *   dom::parser parser;
 *   dom::element doc;
 *   auto error = parser.parse("foo").get(doc);
 *   if (error) { printf("Error: %s\n", error_message(error)); }
 *
 * @return The error message.
 */
inline const char *error_message(error_code error) noexcept;

/**
 * Write the error message to the output stream
 */
inline std::ostream& operator<<(std::ostream& out, error_code error) noexcept;

/**
 * Exception thrown when an exception-supporting simdjson method is called
 */
struct simdjson_error : public std::exception {
  /**
   * Create an exception from a simdjson error code.
   * @param error The error code
   */
  simdjson_error(error_code error) noexcept : _error{error} { }
  /** The error message */
  const char *what() const noexcept { return error_message(error()); }
  /** The error code */
  error_code error() const noexcept { return _error; }
private:
  /** The error code that was used */
  error_code _error;
};

namespace internal {

/**
 * The result of a simdjson operation that could fail.
 *
 * Gives the option of reading error codes, or throwing an exception by casting to the desired result.
 *
 * This is a base class for implementations that want to add functions to the result type for
 * chaining.
 *
 * Override like:
 *
 *   struct simdjson_result<T> : public internal::simdjson_result_base<T> {
 *     simdjson_result() noexcept : internal::simdjson_result_base<T>() {}
 *     simdjson_result(error_code error) noexcept : internal::simdjson_result_base<T>(error) {}
 *     simdjson_result(T &&value) noexcept : internal::simdjson_result_base<T>(std::forward(value)) {}
 *     simdjson_result(T &&value, error_code error) noexcept : internal::simdjson_result_base<T>(value, error) {}
 *     // Your extra methods here
 *   }
 *
 * Then any method returning simdjson_result<T> will be chainable with your methods.
 */
template<typename T>
struct simdjson_result_base : public std::pair<T, error_code> {

  /**
   * Create a new empty result with error = UNINITIALIZED.
   */
  really_inline simdjson_result_base() noexcept;

  /**
   * Create a new error result.
   */
  really_inline simdjson_result_base(error_code error) noexcept;

  /**
   * Create a new successful result.
   */
  really_inline simdjson_result_base(T &&value) noexcept;

  /**
   * Create a new result with both things (use if you don't want to branch when creating the result).
   */
  really_inline simdjson_result_base(T &&value, error_code error) noexcept;

  /**
   * Move the value and the error to the provided variables.
   *
   * @param value The variable to assign the value to. May not be set if there is an error.
   * @param error The variable to assign the error to. Set to SUCCESS if there is no error.
   */
  really_inline void tie(T &value, error_code &error) && noexcept;

  /**
   * Move the value to the provided variable.
   *
   * @param value The variable to assign the value to. May not be set if there is an error.
   */
  really_inline error_code get(T &value) && noexcept;

  /**
   * The error.
   */
  really_inline error_code error() const noexcept;

#if SIMDJSON_EXCEPTIONS

  /**
   * Get the result value.
   *
   * @throw simdjson_error if there was an error.
   */
  really_inline T& value() noexcept(false);

  /**
   * Take the result value (move it).
   *
   * @throw simdjson_error if there was an error.
   */
  really_inline T&& take_value() && noexcept(false);

  /**
   * Cast to the value (will throw on error).
   *
   * @throw simdjson_error if there was an error.
   */
  really_inline operator T&&() && noexcept(false);

#endif // SIMDJSON_EXCEPTIONS
}; // struct simdjson_result_base

} // namespace internal

/**
 * The result of a simdjson operation that could fail.
 *
 * Gives the option of reading error codes, or throwing an exception by casting to the desired result.
 */
template<typename T>
struct simdjson_result : public internal::simdjson_result_base<T> {
  /**
   * @private Create a new empty result with error = UNINITIALIZED.
   */
  really_inline simdjson_result() noexcept;
  /**
   * @private Create a new error result.
   */
  really_inline simdjson_result(T &&value) noexcept;
  /**
   * @private Create a new successful result.
   */
  really_inline simdjson_result(error_code error_code) noexcept;
  /**
   * @private Create a new result with both things (use if you don't want to branch when creating the result).
   */
  really_inline simdjson_result(T &&value, error_code error) noexcept;

  /**
   * Move the value and the error to the provided variables.
   *
   * @param value The variable to assign the value to. May not be set if there is an error.
   * @param error The variable to assign the error to. Set to SUCCESS if there is no error.
   */
  really_inline void tie(T &value, error_code &error) && noexcept;

  /**
   * Move the value to the provided variable.
   *
   * @param value The variable to assign the value to. May not be set if there is an error.
   */
  WARN_UNUSED really_inline error_code get(T &value) && noexcept;

  /**
   * The error.
   */
  really_inline error_code error() const noexcept;

#if SIMDJSON_EXCEPTIONS

  /**
   * Get the result value.
   *
   * @throw simdjson_error if there was an error.
   */
  really_inline T& value() noexcept(false);

  /**
   * Take the result value (move it).
   *
   * @throw simdjson_error if there was an error.
   */
  really_inline T&& take_value() && noexcept(false);

  /**
   * Cast to the value (will throw on error).
   *
   * @throw simdjson_error if there was an error.
   */
  really_inline operator T&&() && noexcept(false);

#endif // SIMDJSON_EXCEPTIONS
}; // struct simdjson_result

/**
 * @deprecated This is an alias and will be removed, use error_code instead
 */
using ErrorValues [[deprecated("This is an alias and will be removed, use error_code instead")]] = error_code;

/**
 * @deprecated Error codes should be stored and returned as `error_code`, use `error_message()` instead.
 */
[[deprecated("Error codes should be stored and returned as `error_code`, use `error_message()` instead.")]]
inline const std::string &error_message(int error) noexcept;

} // namespace simdjson

#endif // SIMDJSON_ERROR_H
/* end file include/simdjson/error.h */
/* begin file include/simdjson/padded_string.h */
#ifndef SIMDJSON_PADDED_STRING_H
#define SIMDJSON_PADDED_STRING_H


#include <cstring>
#include <memory>
#include <string>
#include <ostream>

namespace simdjson {

/**
 * String with extra allocation for ease of use with parser::parse()
 *
 * This is a move-only class, it cannot be copied.
 */
struct padded_string final {

  /**
   * Create a new, empty padded string.
   */
  explicit inline padded_string() noexcept;
  /**
   * Create a new padded string buffer.
   *
   * @param length the size of the string.
   */
  explicit inline padded_string(size_t length) noexcept;
  /**
   * Create a new padded string by copying the given input.
   *
   * @param data the buffer to copy
   * @param length the number of bytes to copy
   */
  explicit inline padded_string(const char *data, size_t length) noexcept;
  /**
   * Create a new padded string by copying the given input.
   *
   * @param str_ the string to copy
   */
  inline padded_string(const std::string & str_ ) noexcept;
  /**
   * Create a new padded string by copying the given input.
   *
   * @param sv_ the string to copy
   */
  inline padded_string(std::string_view sv_) noexcept;
  /**
   * Move one padded string into another.
   *
   * The original padded string will be reduced to zero capacity.
   *
   * @param o the string to move.
   */
  inline padded_string(padded_string &&o) noexcept;
  /**
   * Move one padded string into another.
   *
   * The original padded string will be reduced to zero capacity.
   *
   * @param o the string to move.
   */
  inline padded_string &operator=(padded_string &&o) noexcept;
  inline void swap(padded_string &o) noexcept;
  ~padded_string() noexcept;

  /**
   * The length of the string.
   *
   * Does not include padding.
   */
  size_t size() const noexcept;

  /**
   * The length of the string.
   *
   * Does not include padding.
   */
  size_t length() const noexcept;

  /**
   * The string data.
   **/
  const char *data() const noexcept;

  /**
   * The string data.
   **/
  char *data() noexcept;

  /**
   * Create a std::string_view with the same content.
   */
  operator std::string_view() const;

  /**
   * Load this padded string from a file.
   *
   * @param path the path to the file.
   **/
  inline static simdjson_result<padded_string> load(const std::string &path) noexcept;

private:
  padded_string &operator=(const padded_string &o) = delete;
  padded_string(const padded_string &o) = delete;

  size_t viable_size{0};
  char *data_ptr{nullptr};

}; // padded_string

/**
 * Send padded_string instance to an output stream.
 *
 * @param out The output stream.
 * @param s The padded_string instance.
 * @throw if there is an error with the underlying output stream. simdjson itself will not throw.
 */
inline std::ostream& operator<<(std::ostream& out, const padded_string& s) { return out << s.data(); }

#if SIMDJSON_EXCEPTIONS
/**
 * Send padded_string instance to an output stream.
 *
 * @param out The output stream.
 * @param s The padded_string instance.
  * @throw simdjson_error if the result being printed has an error. If there is an error with the
 *        underlying output stream, that error will be propagated (simdjson_error will not be
 *        thrown).
 */
inline std::ostream& operator<<(std::ostream& out, simdjson_result<padded_string> &s) noexcept(false) { return out << s.value(); }
#endif

} // namespace simdjson

// This is deliberately outside of simdjson so that people get it without having to use the namespace
inline simdjson::padded_string operator "" _padded(const char *str, size_t len) {
  return simdjson::padded_string(str, len);
}

namespace simdjson {
namespace internal {

// low-level function to allocate memory with padding so we can read past the
// "length" bytes safely. if you must provide a pointer to some data, create it
// with this function: length is the max. size in bytes of the string caller is
// responsible to free the memory (free(...))
inline char *allocate_padded_buffer(size_t length) noexcept;

} // namespace internal
} // namespace simdjson

#endif // SIMDJSON_PADDED_STRING_H
/* end file include/simdjson/padded_string.h */
/* begin file include/simdjson/implementation.h */
#ifndef SIMDJSON_IMPLEMENTATION_H
#define SIMDJSON_IMPLEMENTATION_H

/* begin file include/simdjson/internal/dom_parser_implementation.h */
#ifndef SIMDJSON_INTERNAL_DOM_PARSER_IMPLEMENTATION_H
#define SIMDJSON_INTERNAL_DOM_PARSER_IMPLEMENTATION_H

#include <memory>

namespace simdjson {

namespace dom {
class document;
} // namespace dom

namespace internal {

/**
 * An implementation of simdjson's DOM parser for a particular CPU architecture.
 *
 * This class is expected to be accessed only by pointer, and never move in memory (though the
 * pointer can move).
 */
class dom_parser_implementation {
public:

  /**
   * @private For internal implementation use
   *
   * Run a full JSON parse on a single document (stage1 + stage2).
   *
   * Guaranteed only to be called when capacity > document length.
   *
   * Overridden by each implementation.
   *
   * @param buf The json document to parse. *MUST* be allocated up to len + SIMDJSON_PADDING bytes.
   * @param len The length of the json document.
   * @return The error code, or SUCCESS if there was no error.
   */
  WARN_UNUSED virtual error_code parse(const uint8_t *buf, size_t len, dom::document &doc) noexcept = 0;

  /**
   * @private For internal implementation use
   *
   * Stage 1 of the document parser.
   *
   * Guaranteed only to be called when capacity > document length.
   *
   * Overridden by each implementation.
   *
   * @param buf The json document to parse.
   * @param len The length of the json document.
   * @param streaming Whether this is being called by parser::parse_many.
   * @return The error code, or SUCCESS if there was no error.
   */
  WARN_UNUSED virtual error_code stage1(const uint8_t *buf, size_t len, bool streaming) noexcept = 0;

  /**
   * @private For internal implementation use
   *
   * Stage 2 of the document parser.
   *
   * Called after stage1().
   *
   * Overridden by each implementation.
   *
   * @param doc The document to output to.
   * @return The error code, or SUCCESS if there was no error.
   */
  WARN_UNUSED virtual error_code stage2(dom::document &doc) noexcept = 0;

  /**
   * @private For internal implementation use
   *
   * Stage 2 of the document parser for parser::parse_many.
   *
   * Guaranteed only to be called after stage1().
   * Overridden by each implementation.
   *
   * @param doc The document to output to.
   * @return The error code, SUCCESS if there was no error, or EMPTY if all documents have been parsed.
   */
  WARN_UNUSED virtual error_code stage2_next(dom::document &doc) noexcept = 0;

  /**
   * Change the capacity of this parser.
   *
   * Generally used for reallocation.
   *
   * @param capacity The new capacity.
   * @param max_depth The new max_depth.
   * @return The error code, or SUCCESS if there was no error.
   */
  virtual error_code set_capacity(size_t capacity) noexcept = 0;

  /**
   * Change the max depth of this parser.
   *
   * Generally used for reallocation.
   *
   * @param capacity The new capacity.
   * @param max_depth The new max_depth.
   * @return The error code, or SUCCESS if there was no error.
   */
  virtual error_code set_max_depth(size_t max_depth) noexcept = 0;

  /**
   * Deallocate this parser.
   */
  virtual ~dom_parser_implementation() = default;

  /** Number of structural indices passed from stage 1 to stage 2 */
  uint32_t n_structural_indexes{0};
  /** Structural indices passed from stage 1 to stage 2 */
  std::unique_ptr<uint32_t[]> structural_indexes{};
  /** Next structural index to parse */
  uint32_t next_structural_index{0};

  /**
   * The largest document this parser can support without reallocating.
   *
   * @return Current capacity, in bytes.
   */
  really_inline size_t capacity() const noexcept;

  /**
   * The maximum level of nested object and arrays supported by this parser.
   *
   * @return Maximum depth, in bytes.
   */
  really_inline size_t max_depth() const noexcept;

  /**
   * Ensure this parser has enough memory to process JSON documents up to `capacity` bytes in length
   * and `max_depth` depth.
   *
   * @param capacity The new capacity.
   * @param max_depth The new max_depth. Defaults to DEFAULT_MAX_DEPTH.
   * @return The error, if there is one.
   */
  WARN_UNUSED inline error_code allocate(size_t capacity, size_t max_depth) noexcept;

protected:
  /**
   * The maximum document length this parser supports.
   *
   * Buffers are large enough to handle any document up to this length.
   */
  size_t _capacity{0};

  /**
   * The maximum depth (number of nested objects and arrays) supported by this parser.
   *
   * Defaults to DEFAULT_MAX_DEPTH.
   */
  size_t _max_depth{0};
}; // class dom_parser_implementation

really_inline size_t dom_parser_implementation::capacity() const noexcept {
  return _capacity;
}

really_inline size_t dom_parser_implementation::max_depth() const noexcept {
  return _max_depth;
}

WARN_UNUSED
inline error_code dom_parser_implementation::allocate(size_t capacity, size_t max_depth) noexcept {
  if (this->max_depth() != max_depth) {
    error_code err = set_max_depth(max_depth);
    if (err) { return err; }
  }
  if (_capacity != capacity) {
    error_code err = set_capacity(capacity);
    if (err) { return err; }
  }
  return SUCCESS;
}

} // namespace internal
} // namespace simdjson

#endif // SIMDJSON_INTERNAL_DOM_PARSER_IMPLEMENTATION_H
/* end file include/simdjson/internal/dom_parser_implementation.h */
#include <string>
#include <atomic>
#include <vector>

namespace simdjson {

/**
 * Validate the UTF-8 string.
 *
 * @param buf the string to validate.
 * @param len the length of the string in bytes.
 * @return true if the string is valid UTF-8.
 */
WARN_UNUSED bool validate_utf8(const char * buf, size_t len) noexcept;


/**
 * Validate the UTF-8 string.
 *
 * @param sv the string_view to validate.
 * @return true if the string is valid UTF-8.
 */
really_inline WARN_UNUSED bool validate_utf8(const std::string_view sv) noexcept {
  return validate_utf8(sv.data(), sv.size());
}

/**
 * Validate the UTF-8 string.
 *
 * @param p the string to validate.
 * @return true if the string is valid UTF-8.
 */
really_inline WARN_UNUSED bool validate_utf8(const std::string& s) noexcept {
  return validate_utf8(s.data(), s.size());
}

namespace dom {
  class document;
} // namespace dom

/**
 * An implementation of simdjson for a particular CPU architecture.
 *
 * Also used to maintain the currently active implementation. The active implementation is
 * automatically initialized on first use to the most advanced implementation supported by the host.
 */
class implementation {
public:

  /**
   * The name of this implementation.
   *
   *     const implementation *impl = simdjson::active_implementation;
   *     cout << "simdjson is optimized for " << impl->name() << "(" << impl->description() << ")" << endl;
   *
   * @return the name of the implementation, e.g. "haswell", "westmere", "arm64"
   */
  virtual const std::string &name() const { return _name; }

  /**
   * The description of this implementation.
   *
   *     const implementation *impl = simdjson::active_implementation;
   *     cout << "simdjson is optimized for " << impl->name() << "(" << impl->description() << ")" << endl;
   *
   * @return the name of the implementation, e.g. "haswell", "westmere", "arm64"
   */
  virtual const std::string &description() const { return _description; }

  /**
   * @private For internal implementation use
   *
   * The instruction sets this implementation is compiled against.
   *
   * @return a mask of all required `instruction_set` values
   */
  virtual uint32_t required_instruction_sets() const { return _required_instruction_sets; };

  /**
   * @private For internal implementation use
   *
   *     const implementation *impl = simdjson::active_implementation;
   *     cout << "simdjson is optimized for " << impl->name() << "(" << impl->description() << ")" << endl;
   *
   * @param capacity The largest document that will be passed to the parser.
   * @param max_depth The maximum JSON object/array nesting this parser is expected to handle.
   * @param dst The place to put the resulting parser implementation.
   * @return the name of the implementation, e.g. "haswell", "westmere", "arm64"
   */
  virtual error_code create_dom_parser_implementation(
    size_t capacity,
    size_t max_depth,
    std::unique_ptr<internal::dom_parser_implementation> &dst
  ) const noexcept = 0;

  /**
   * @private For internal implementation use
   *
   * Minify the input string assuming that it represents a JSON string, does not parse or validate.
   *
   * Overridden by each implementation.
   *
   * @param buf the json document to minify.
   * @param len the length of the json document.
   * @param dst the buffer to write the minified document to. *MUST* be allocated up to len + SIMDJSON_PADDING bytes.
   * @param dst_len the number of bytes written. Output only.
   * @return the error code, or SUCCESS if there was no error.
   */
  WARN_UNUSED virtual error_code minify(const uint8_t *buf, size_t len, uint8_t *dst, size_t &dst_len) const noexcept = 0;


  /**
   * Validate the UTF-8 string.
   *
   * Overridden by each implementation.
   *
   * @param buf the string to validate.
   * @param len the length of the string in bytes.
   * @return true if and only if the string is valid UTF-8.
   */
  WARN_UNUSED virtual bool validate_utf8(const char *buf, size_t len) const noexcept = 0;

protected:
  /** @private Construct an implementation with the given name and description. For subclasses. */
  really_inline implementation(
    std::string_view name,
    std::string_view description,
    uint32_t required_instruction_sets
  ) :
    _name(name),
    _description(description),
    _required_instruction_sets(required_instruction_sets)
  {
  }
  virtual ~implementation()=default;

private:
  /**
   * The name of this implementation.
   */
  const std::string _name;

  /**
   * The description of this implementation.
   */
  const std::string _description;

  /**
   * Instruction sets required for this implementation.
   */
  const uint32_t _required_instruction_sets;
};

/** @private */
namespace internal {

/**
 * The list of available implementations compiled into simdjson.
 */
class available_implementation_list {
public:
  /** Get the list of available implementations compiled into simdjson */
  really_inline available_implementation_list() {}
  /** Number of implementations */
  size_t size() const noexcept;
  /** STL const begin() iterator */
  const implementation * const *begin() const noexcept;
  /** STL const end() iterator */
  const implementation * const *end() const noexcept;

  /**
   * Get the implementation with the given name.
   *
   * Case sensitive.
   *
   *     const implementation *impl = simdjson::available_implementations["westmere"];
   *     if (!impl) { exit(1); }
   *     simdjson::active_implementation = impl;
   *
   * @param name the implementation to find, e.g. "westmere", "haswell", "arm64"
   * @return the implementation, or nullptr if the parse failed.
   */
  const implementation * operator[](const std::string_view &name) const noexcept {
    for (const implementation * impl : *this) {
      if (impl->name() == name) { return impl; }
    }
    return nullptr;
  }

  /**
   * Detect the most advanced implementation supported by the current host.
   *
   * This is used to initialize the implementation on startup.
   *
   *     const implementation *impl = simdjson::available_implementation::detect_best_supported();
   *     simdjson::active_implementation = impl;
   *
   * @return the most advanced supported implementation for the current host, or an
   *         implementation that returns UNSUPPORTED_ARCHITECTURE if there is no supported
   *         implementation. Will never return nullptr.
   */
  const implementation *detect_best_supported() const noexcept;
};

template<typename T>
class atomic_ptr {
public:
  atomic_ptr(T *_ptr) : ptr{_ptr} {}

  operator const T*() const { return ptr.load(); }
  const T& operator*() const { return *ptr; }
  const T* operator->() const { return ptr.load(); }

  operator T*() { return ptr.load(); }
  T& operator*() { return *ptr; }
  T* operator->() { return ptr.load(); }
  atomic_ptr& operator=(T *_ptr) { ptr = _ptr; return *this; }

private:
  std::atomic<T*> ptr;
};

} // namespace internal

/**
 * The list of available implementations compiled into simdjson.
 */
extern SIMDJSON_DLLIMPORTEXPORT const internal::available_implementation_list available_implementations;

/**
  * The active implementation.
  *
  * Automatically initialized on first use to the most advanced implementation supported by this hardware.
  */
extern SIMDJSON_DLLIMPORTEXPORT internal::atomic_ptr<const implementation> active_implementation;

} // namespace simdjson

#endif // SIMDJSON_IMPLEMENTATION_H
/* end file include/simdjson/internal/dom_parser_implementation.h */
/* begin file include/simdjson/dom/array.h */
#ifndef SIMDJSON_DOM_ARRAY_H
#define SIMDJSON_DOM_ARRAY_H

/* begin file include/simdjson/internal/tape_ref.h */
#ifndef SIMDJSON_INTERNAL_TAPE_REF_H
#define SIMDJSON_INTERNAL_TAPE_REF_H

/* begin file include/simdjson/internal/tape_type.h */
#ifndef SIMDJSON_INTERNAL_TAPE_TYPE_H
#define SIMDJSON_INTERNAL_TAPE_TYPE_H

namespace simdjson {
namespace internal {

/**
 * The possible types in the tape.
 */
enum class tape_type {
  ROOT = 'r',
  START_ARRAY = '[',
  START_OBJECT = '{',
  END_ARRAY = ']',
  END_OBJECT = '}',
  STRING = '"',
  INT64 = 'l',
  UINT64 = 'u',
  DOUBLE = 'd',
  TRUE_VALUE = 't',
  FALSE_VALUE = 'f',
  NULL_VALUE = 'n'
}; // enum class tape_type

} // namespace internal
} // namespace simdjson

#endif // SIMDJSON_INTERNAL_TAPE_TYPE_H
/* end file include/simdjson/internal/tape_type.h */

namespace simdjson {

namespace dom {
  class document;
}

namespace internal {

constexpr const uint64_t JSON_VALUE_MASK = 0x00FFFFFFFFFFFFFF;
constexpr const uint32_t JSON_COUNT_MASK = 0xFFFFFF;

/**
 * A reference to an element on the tape. Internal only.
 */
class tape_ref {
public:
  really_inline tape_ref() noexcept;
  really_inline tape_ref(const dom::document *doc, size_t json_index) noexcept;
  inline size_t after_element() const noexcept;
  really_inline tape_type tape_ref_type() const noexcept;
  really_inline uint64_t tape_value() const noexcept;
  really_inline bool is_double() const noexcept;
  really_inline bool is_int64() const noexcept;
  really_inline bool is_uint64() const noexcept;
  really_inline bool is_false() const noexcept;
  really_inline bool is_true() const noexcept;
  really_inline bool is_null_on_tape() const noexcept;// different name to avoid clash with is_null.
  really_inline uint32_t matching_brace_index() const noexcept;
  really_inline uint32_t scope_count() const noexcept;
  template<typename T>
  really_inline T next_tape_value() const noexcept;
  really_inline uint32_t get_string_length() const noexcept;
  really_inline const char * get_c_str() const noexcept;
  inline std::string_view get_string_view() const noexcept;

  /** The document this element references. */
  const dom::document *doc;

  /** The index of this element on `doc.tape[]` */
  size_t json_index;
};

} // namespace internal
} // namespace simdjson

#endif // SIMDJSON_INTERNAL_TAPE_REF_H
/* end file include/simdjson/internal/tape_type.h */
/* begin file include/simdjson/minify.h */
#ifndef SIMDJSON_MINIFY_H
#define SIMDJSON_MINIFY_H

#include <string>
#include <ostream>
#include <sstream>

namespace simdjson {



/**
 *
 * Minify the input string assuming that it represents a JSON string, does not parse or validate.
 * This function is much faster than parsing a JSON string and then writing a minified version of it.
 * However, it does not validate the input.
 *
 *
 * @param buf the json document to minify.
 * @param len the length of the json document.
 * @param dst the buffer to write the minified document to. *MUST* be allocated up to len + SIMDJSON_PADDING bytes.
 * @param dst_len the number of bytes written. Output only.
 * @return the error code, or SUCCESS if there was no error.
 */
WARN_UNUSED error_code minify(const char *buf, size_t len, char *dst, size_t &dst_len) noexcept;

/**
 * Minifies a JSON element or document, printing the smallest possible valid JSON.
 *
 *   dom::parser parser;
 *   element doc = parser.parse("   [ 1 , 2 , 3 ] "_padded);
 *   cout << minify(doc) << endl; // prints [1,2,3]
 *
 */
template<typename T>
class minifier {
public:
  /**
   * Create a new minifier.
   *
   * @param _value The document or element to minify.
   */
  inline minifier(const T &_value) noexcept : value{_value} {}

  /**
   * Minify JSON to a string.
   */
  inline operator std::string() const noexcept { std::stringstream s; s << *this; return s.str(); }

  /**
   * Minify JSON to an output stream.
   */
  inline std::ostream& print(std::ostream& out);
private:
  const T &value;
};

template<typename T>
inline minifier<T> minify(const T &value) noexcept { return minifier<T>(value); }

/**
 * Minify JSON to an output stream.
 *
 * @param out The output stream.
 * @param formatter The minifier.
 * @throw if there is an error with the underlying output stream. simdjson itself will not throw.
 */
template<typename T>
inline std::ostream& operator<<(std::ostream& out, minifier<T> formatter) { return formatter.print(out); }

} // namespace simdjson

#endif // SIMDJSON_MINIFY_H
/* end file include/simdjson/minify.h */
#include <ostream>

namespace simdjson {
namespace dom {

class document;
class element;

/**
 * JSON array.
 */
class array {
public:
  /** Create a new, invalid array */
  really_inline array() noexcept;

  class iterator {
  public:
    /**
     * Get the actual value
     */
    inline element operator*() const noexcept;
    /**
     * Get the next value.
     *
     * Part of the std::iterator interface.
     *
     */
    inline iterator& operator++() noexcept;
    /**
     * Check if these values come from the same place in the JSON.
     *
     * Part of the std::iterator interface.
     */
    inline bool operator!=(const iterator& other) const noexcept;
  private:
    really_inline iterator(const internal::tape_ref &tape) noexcept;
    internal::tape_ref tape;
    friend class array;
  };

  /**
   * Return the first array element.
   *
   * Part of the std::iterable interface.
   */
  inline iterator begin() const noexcept;
  /**
   * One past the last array element.
   *
   * Part of the std::iterable interface.
   */
  inline iterator end() const noexcept;
  /**
   * Get the size of the array (number of immediate children).
   * It is a saturated value with a maximum of 0xFFFFFF: if the value
   * is 0xFFFFFF then the size is 0xFFFFFF or greater.
   */
  inline size_t size() const noexcept;
  /**
   * Get the value associated with the given JSON pointer.
   *
   *   dom::parser parser;
   *   array a = parser.parse(R"([ { "foo": { "a": [ 10, 20, 30 ] }} ])"_padded);
   *   a.at("0/foo/a/1") == 20
   *   a.at("0")["foo"]["a"].at(1) == 20
   *
   * @return The value associated with the given JSON pointer, or:
   *         - NO_SUCH_FIELD if a field does not exist in an object
   *         - INDEX_OUT_OF_BOUNDS if an array index is larger than an array length
   *         - INCORRECT_TYPE if a non-integer is used to access an array
   *         - INVALID_JSON_POINTER if the JSON pointer is invalid and cannot be parsed
   */
  inline simdjson_result<element> at(const std::string_view &json_pointer) const noexcept;

  /**
   * Get the value at the given index. This function has linear-time complexity and
   * is equivalent to the following:
   *
   *    size_t i=0;
   *    for (auto element : *this) {
   *      if (i == index) { return element; }
   *      i++;
   *    }
   *    return INDEX_OUT_OF_BOUNDS;
   *
   * Avoid calling the at() function repeatedly.
   *
   * @return The value at the given index, or:
   *         - INDEX_OUT_OF_BOUNDS if the array index is larger than an array length
   */
  inline simdjson_result<element> at(size_t index) const noexcept;

private:
  really_inline array(const internal::tape_ref &tape) noexcept;
  internal::tape_ref tape;
  friend class element;
  friend struct simdjson_result<element>;
  template<typename T>
  friend class simdjson::minifier;
};

/**
 * Print JSON to an output stream.
 *
 * By default, the value will be printed minified.
 *
 * @param out The output stream.
 * @param value The value to print.
 * @throw if there is an error with the underlying output stream. simdjson itself will not throw.
 */
inline std::ostream& operator<<(std::ostream& out, const array &value);

} // namespace dom

/** The result of a JSON conversion that may fail. */
template<>
struct simdjson_result<dom::array> : public internal::simdjson_result_base<dom::array> {
public:
  really_inline simdjson_result() noexcept; ///< @private
  really_inline simdjson_result(dom::array value) noexcept; ///< @private
  really_inline simdjson_result(error_code error) noexcept; ///< @private

  inline simdjson_result<dom::element> at(const std::string_view &json_pointer) const noexcept;
  inline simdjson_result<dom::element> at(size_t index) const noexcept;

#if SIMDJSON_EXCEPTIONS
  inline dom::array::iterator begin() const noexcept(false);
  inline dom::array::iterator end() const noexcept(false);
  inline size_t size() const noexcept(false);
#endif // SIMDJSON_EXCEPTIONS
};

#if SIMDJSON_EXCEPTIONS
/**
 * Print JSON to an output stream.
 *
 * By default, the value will be printed minified.
 *
 * @param out The output stream.
 * @param value The value to print.
 * @throw simdjson_error if the result being printed has an error. If there is an error with the
 *        underlying output stream, that error will be propagated (simdjson_error will not be
 *        thrown).
 */
inline std::ostream& operator<<(std::ostream& out, const simdjson_result<dom::array> &value) noexcept(false);
#endif

} // namespace simdjson

#endif // SIMDJSON_DOM_ARRAY_H
/* end file include/simdjson/minify.h */
/* begin file include/simdjson/dom/document_stream.h */
#ifndef SIMDJSON_DOCUMENT_STREAM_H
#define SIMDJSON_DOCUMENT_STREAM_H

/* begin file include/simdjson/dom/parser.h */
#ifndef SIMDJSON_DOM_PARSER_H
#define SIMDJSON_DOM_PARSER_H

/* begin file include/simdjson/dom/document.h */
#ifndef SIMDJSON_DOM_DOCUMENT_H
#define SIMDJSON_DOM_DOCUMENT_H

#include <memory>
#include <ostream>

namespace simdjson {
namespace dom {

class element;

/**
 * A parsed JSON document.
 *
 * This class cannot be copied, only moved, to avoid unintended allocations.
 */
class document {
public:
  /**
   * Create a document container with zero capacity.
   *
   * The parser will allocate capacity as needed.
   */
  document() noexcept = default;
  ~document() noexcept = default;

  /**
   * Take another document's buffers.
   *
   * @param other The document to take. Its capacity is zeroed and it is invalidated.
   */
  document(document &&other) noexcept = default;
  /** @private */
  document(const document &) = delete; // Disallow copying
  /**
   * Take another document's buffers.
   *
   * @param other The document to take. Its capacity is zeroed.
   */
  document &operator=(document &&other) noexcept = default;
  /** @private */
  document &operator=(const document &) = delete; // Disallow copying

  /**
   * Get the root element of this document as a JSON array.
   */
  element root() const noexcept;

  /**
   * @private Dump the raw tape for debugging.
   *
   * @param os the stream to output to.
   * @return false if the tape is likely wrong (e.g., you did not parse a valid JSON).
   */
  bool dump_raw_tape(std::ostream &os) const noexcept;

  /** @private Structural values. */
  std::unique_ptr<uint64_t[]> tape{};

  /** @private String values.
   *
   * Should be at least byte_capacity.
   */
  std::unique_ptr<uint8_t[]> string_buf{};

private:
  inline error_code allocate(size_t len) noexcept;
  template<typename T>
  friend class simdjson::minifier;
  friend class parser;
}; // class document

} // namespace dom
} // namespace simdjson

#endif // SIMDJSON_DOM_DOCUMENT_H
/* end file include/simdjson/dom/document.h */
#include <memory>
#include <ostream>
#include <string>

namespace simdjson {

namespace dom {

class document_stream;
class element;

/** The default batch size for parser.parse_many() and parser.load_many() */
static constexpr size_t DEFAULT_BATCH_SIZE = 1000000;

/**
  * A persistent document parser.
  *
  * The parser is designed to be reused, holding the internal buffers necessary to do parsing,
  * as well as memory for a single document. The parsed document is overwritten on each parse.
  *
  * This class cannot be copied, only moved, to avoid unintended allocations.
  *
  * @note This is not thread safe: one parser cannot produce two documents at the same time!
  */
class parser {
public:
  /**
  * Create a JSON parser.
  *
  * The new parser will have zero capacity.
  *
  * @param max_capacity The maximum document length the parser can automatically handle. The parser
  *    will allocate more capacity on an as needed basis (when it sees documents too big to handle)
  *    up to this amount. The parser still starts with zero capacity no matter what this number is:
  *    to allocate an initial capacity, call allocate() after constructing the parser.
  *    Defaults to SIMDJSON_MAXSIZE_BYTES (the largest single document simdjson can process).
  */
  really_inline explicit parser(size_t max_capacity = SIMDJSON_MAXSIZE_BYTES) noexcept;
  /**
   * Take another parser's buffers and state.
   *
   * @param other The parser to take. Its capacity is zeroed.
   */
  really_inline parser(parser &&other) noexcept;
  parser(const parser &) = delete; ///< @private Disallow copying
  /**
   * Take another parser's buffers and state.
   *
   * @param other The parser to take. Its capacity is zeroed.
   */
  really_inline parser &operator=(parser &&other) noexcept;
  parser &operator=(const parser &) = delete; ///< @private Disallow copying

  /** Deallocate the JSON parser. */
  ~parser()=default;

  /**
   * Load a JSON document from a file and return a reference to it.
   *
   *   dom::parser parser;
   *   const element doc = parser.load("jsonexamples/twitter.json");
   *
   * ### IMPORTANT: Document Lifetime
   *
   * The JSON document still lives in the parser: this is the most efficient way to parse JSON
   * documents because it reuses the same buffers, but you *must* use the document before you
   * destroy the parser or call parse() again.
   *
   * ### Parser Capacity
   *
   * If the parser's current capacity is less than the file length, it will allocate enough capacity
   * to handle it (up to max_capacity).
   *
   * @param path The path to load.
   * @return The document, or an error:
   *         - IO_ERROR if there was an error opening or reading the file.
   *         - MEMALLOC if the parser does not have enough capacity and memory allocation fails.
   *         - CAPACITY if the parser does not have enough capacity and len > max_capacity.
   *         - other json errors if parsing fails.
   */
  inline simdjson_result<element> load(const std::string &path) & noexcept;
  inline simdjson_result<element> load(const std::string &path) &&  = delete ;
  /**
   * Parse a JSON document and return a temporary reference to it.
   *
   *   dom::parser parser;
   *   element doc = parser.parse(buf, len);
   *
   * ### IMPORTANT: Document Lifetime
   *
   * The JSON document still lives in the parser: this is the most efficient way to parse JSON
   * documents because it reuses the same buffers, but you *must* use the document before you
   * destroy the parser or call parse() again.
   *
   * ### REQUIRED: Buffer Padding
   *
   * The buffer must have at least SIMDJSON_PADDING extra allocated bytes. It does not matter what
   * those bytes are initialized to, as long as they are allocated.
   *
   * If realloc_if_needed is true, it is assumed that the buffer does *not* have enough padding,
   * and it is copied into an enlarged temporary buffer before parsing.
   *
   * ### Parser Capacity
   *
   * If the parser's current capacity is less than len, it will allocate enough capacity
   * to handle it (up to max_capacity).
   *
   * @param buf The JSON to parse. Must have at least len + SIMDJSON_PADDING allocated bytes, unless
   *            realloc_if_needed is true.
   * @param len The length of the JSON.
   * @param realloc_if_needed Whether to reallocate and enlarge the JSON buffer to add padding.
   * @return The document, or an error:
   *         - MEMALLOC if realloc_if_needed is true or the parser does not have enough capacity,
   *           and memory allocation fails.
   *         - CAPACITY if the parser does not have enough capacity and len > max_capacity.
   *         - other json errors if parsing fails.
   */
  inline simdjson_result<element> parse(const uint8_t *buf, size_t len, bool realloc_if_needed = true) & noexcept;
  inline simdjson_result<element> parse(const uint8_t *buf, size_t len, bool realloc_if_needed = true) && =delete;
  /** @overload parse(const uint8_t *buf, size_t len, bool realloc_if_needed) */
  really_inline simdjson_result<element> parse(const char *buf, size_t len, bool realloc_if_needed = true) & noexcept;
  really_inline simdjson_result<element> parse(const char *buf, size_t len, bool realloc_if_needed = true) && =delete;
  /** @overload parse(const uint8_t *buf, size_t len, bool realloc_if_needed) */
  really_inline simdjson_result<element> parse(const std::string &s) & noexcept;
  really_inline simdjson_result<element> parse(const std::string &s) && =delete;
  /** @overload parse(const uint8_t *buf, size_t len, bool realloc_if_needed) */
  really_inline simdjson_result<element> parse(const padded_string &s) & noexcept;
  really_inline simdjson_result<element> parse(const padded_string &s) && =delete;

  /** @private We do not want to allow implicit conversion from C string to std::string. */
  really_inline simdjson_result<element> parse(const char *buf) noexcept = delete;

  /**
   * Load a file containing many JSON documents.
   *
   *   dom::parser parser;
   *   for (const element doc : parser.load_many(path)) {
   *     cout << std::string(doc["title"]) << endl;
   *   }
   *
   * ### Format
   *
   * The file must contain a series of one or more JSON documents, concatenated into a single
   * buffer, separated by whitespace. It effectively parses until it has a fully valid document,
   * then starts parsing the next document at that point. (It does this with more parallelism and
   * lookahead than you might think, though.)
   *
   * documents that consist of an object or array may omit the whitespace between them, concatenating
   * with no separator. documents that consist of a single primitive (i.e. documents that are not
   * arrays or objects) MUST be separated with whitespace.
   *
   * The documents must not exceed batch_size bytes (by default 1MB) or they will fail to parse.
   * Setting batch_size to excessively large or excesively small values may impact negatively the
   * performance.
   *
   * ### Error Handling
   *
   * All errors are returned during iteration: if there is a global error such as memory allocation,
   * it will be yielded as the first result. Iteration always stops after the first error.
   *
   * As with all other simdjson methods, non-exception error handling is readily available through
   * the same interface, requiring you to check the error before using the document:
   *
   *   dom::parser parser;
   *   dom::document_stream docs;
   *   auto error = parser.load_many(path).get(docs);
   *   if (error) { cerr << error << endl; exit(1); }
   *   for (auto doc : docs) {
   *     std::string_view title;
   *     if ((error = doc["title"].get(title)) { cerr << error << endl; exit(1); }
   *     cout << title << endl;
   *   }
   *
   * ### Threads
   *
   * When compiled with SIMDJSON_THREADS_ENABLED, this method will use a single thread under the
   * hood to do some lookahead.
   *
   * ### Parser Capacity
   *
   * If the parser's current capacity is less than batch_size, it will allocate enough capacity
   * to handle it (up to max_capacity).
   *
   * @param path File name pointing at the concatenated JSON to parse.
   * @param batch_size The batch size to use. MUST be larger than the largest document. The sweet
   *                   spot is cache-related: small enough to fit in cache, yet big enough to
   *                   parse as many documents as possible in one tight loop.
   *                   Defaults to 10MB, which has been a reasonable sweet spot in our tests.
   * @return The stream, or an error. An empty input will yield 0 documents rather than an EMPTY error. Errors:
   *         - IO_ERROR if there was an error opening or reading the file.
   *         - MEMALLOC if the parser does not have enough capacity and memory allocation fails.
   *         - CAPACITY if the parser does not have enough capacity and batch_size > max_capacity.
   *         - other json errors if parsing fails.
   */
  inline simdjson_result<document_stream> load_many(const std::string &path, size_t batch_size = DEFAULT_BATCH_SIZE) noexcept;

  /**
   * Parse a buffer containing many JSON documents.
   *
   *   dom::parser parser;
   *   for (element doc : parser.parse_many(buf, len)) {
   *     cout << std::string(doc["title"]) << endl;
   *   }
   *
   * ### Format
   *
   * The buffer must contain a series of one or more JSON documents, concatenated into a single
   * buffer, separated by whitespace. It effectively parses until it has a fully valid document,
   * then starts parsing the next document at that point. (It does this with more parallelism and
   * lookahead than you might think, though.)
   *
   * documents that consist of an object or array may omit the whitespace between them, concatenating
   * with no separator. documents that consist of a single primitive (i.e. documents that are not
   * arrays or objects) MUST be separated with whitespace.
   *
   * The documents must not exceed batch_size bytes (by default 1MB) or they will fail to parse.
   * Setting batch_size to excessively large or excesively small values may impact negatively the
   * performance.
   *
   * ### Error Handling
   *
   * All errors are returned during iteration: if there is a global error such as memory allocation,
   * it will be yielded as the first result. Iteration always stops after the first error.
   *
   * As with all other simdjson methods, non-exception error handling is readily available through
   * the same interface, requiring you to check the error before using the document:
   *
   *   dom::parser parser;
   *   dom::document_stream docs;
   *   auto error = parser.load_many(path).get(docs);
   *   if (error) { cerr << error << endl; exit(1); }
   *   for (auto doc : docs) {
   *     std::string_view title;
   *     if ((error = doc["title"].get(title)) { cerr << error << endl; exit(1); }
   *     cout << title << endl;
   *   }
   *
   * ### REQUIRED: Buffer Padding
   *
   * The buffer must have at least SIMDJSON_PADDING extra allocated bytes. It does not matter what
   * those bytes are initialized to, as long as they are allocated.
   *
   * ### Threads
   *
   * When compiled with SIMDJSON_THREADS_ENABLED, this method will use a single thread under the
   * hood to do some lookahead.
   *
   * ### Parser Capacity
   *
   * If the parser's current capacity is less than batch_size, it will allocate enough capacity
   * to handle it (up to max_capacity).
   *
   * @param buf The concatenated JSON to parse. Must have at least len + SIMDJSON_PADDING allocated bytes.
   * @param len The length of the concatenated JSON.
   * @param batch_size The batch size to use. MUST be larger than the largest document. The sweet
   *                   spot is cache-related: small enough to fit in cache, yet big enough to
   *                   parse as many documents as possible in one tight loop.
   *                   Defaults to 10MB, which has been a reasonable sweet spot in our tests.
   * @return The stream, or an error. An empty input will yield 0 documents rather than an EMPTY error. Errors:
   *         - MEMALLOC if the parser does not have enough capacity and memory allocation fails
   *         - CAPACITY if the parser does not have enough capacity and batch_size > max_capacity.
   *         - other json errors if parsing fails.
   */
  inline simdjson_result<document_stream> parse_many(const uint8_t *buf, size_t len, size_t batch_size = DEFAULT_BATCH_SIZE) noexcept;
  /** @overload parse_many(const uint8_t *buf, size_t len, size_t batch_size) */
  inline simdjson_result<document_stream> parse_many(const char *buf, size_t len, size_t batch_size = DEFAULT_BATCH_SIZE) noexcept;
  /** @overload parse_many(const uint8_t *buf, size_t len, size_t batch_size) */
  inline simdjson_result<document_stream> parse_many(const std::string &s, size_t batch_size = DEFAULT_BATCH_SIZE) noexcept;
  /** @overload parse_many(const uint8_t *buf, size_t len, size_t batch_size) */
  inline simdjson_result<document_stream> parse_many(const padded_string &s, size_t batch_size = DEFAULT_BATCH_SIZE) noexcept;

  /** @private We do not want to allow implicit conversion from C string to std::string. */
  simdjson_result<document_stream> parse_many(const char *buf, size_t batch_size = DEFAULT_BATCH_SIZE) noexcept = delete;

  /**
   * Ensure this parser has enough memory to process JSON documents up to `capacity` bytes in length
   * and `max_depth` depth.
   *
   * @param capacity The new capacity.
   * @param max_depth The new max_depth. Defaults to DEFAULT_MAX_DEPTH.
   * @return The error, if there is one.
   */
  WARN_UNUSED inline error_code allocate(size_t capacity, size_t max_depth = DEFAULT_MAX_DEPTH) noexcept;

  /**
   * @private deprecated because it returns bool instead of error_code, which is our standard for
   * failures. Use allocate() instead.
   *
   * Ensure this parser has enough memory to process JSON documents up to `capacity` bytes in length
   * and `max_depth` depth.
   *
   * @param capacity The new capacity.
   * @param max_depth The new max_depth. Defaults to DEFAULT_MAX_DEPTH.
   * @return true if successful, false if allocation failed.
   */
  [[deprecated("Use allocate() instead.")]]
  WARN_UNUSED inline bool allocate_capacity(size_t capacity, size_t max_depth = DEFAULT_MAX_DEPTH) noexcept;

  /**
   * The largest document this parser can support without reallocating.
   *
   * @return Current capacity, in bytes.
   */
  really_inline size_t capacity() const noexcept;

  /**
   * The largest document this parser can automatically support.
   *
   * The parser may reallocate internal buffers as needed up to this amount.
   *
   * @return Maximum capacity, in bytes.
   */
  really_inline size_t max_capacity() const noexcept;

  /**
   * The maximum level of nested object and arrays supported by this parser.
   *
   * @return Maximum depth, in bytes.
   */
  really_inline size_t max_depth() const noexcept;

  /**
   * Set max_capacity. This is the largest document this parser can automatically support.
   *
   * The parser may reallocate internal buffers as needed up to this amount as documents are passed
   * to it.
   *
   * This call will not allocate or deallocate, even if capacity is currently above max_capacity.
   *
   * @param max_capacity The new maximum capacity, in bytes.
   */
  really_inline void set_max_capacity(size_t max_capacity) noexcept;

  /** @private Use the new DOM API instead */
  class Iterator;
  /** @private Use simdjson_error instead */
  using InvalidJSON [[deprecated("Use simdjson_error instead")]] = simdjson_error;

  /** @private [for benchmarking access] The implementation to use */
  std::unique_ptr<internal::dom_parser_implementation> implementation{};

  /** @private Use `if (parser.parse(...).error())` instead */
  bool valid{false};
  /** @private Use `parser.parse(...).error()` instead */
  error_code error{UNINITIALIZED};

  /** @private Use `parser.parse(...).value()` instead */
  document doc{};

  /** @private returns true if the document parsed was valid */
  [[deprecated("Use the result of parser.parse() instead")]]
  inline bool is_valid() const noexcept;

  /**
   * @private return an error code corresponding to the last parsing attempt, see
   * simdjson.h will return UNITIALIZED if no parsing was attempted
   */
  [[deprecated("Use the result of parser.parse() instead")]]
  inline int get_error_code() const noexcept;

  /** @private return the string equivalent of "get_error_code" */
  [[deprecated("Use error_message() on the result of parser.parse() instead, or cout << error")]]
  inline std::string get_error_message() const noexcept;

  /** @private */
  [[deprecated("Use cout << on the result of parser.parse() instead")]]
  inline bool print_json(std::ostream &os) const noexcept;

  /** @private Private and deprecated: use `parser.parse(...).doc.dump_raw_tape()` instead */
  inline bool dump_raw_tape(std::ostream &os) const noexcept;

private:
  /**
   * The maximum document length this parser will automatically support.
   *
   * The parser will not be automatically allocated above this amount.
   */
  size_t _max_capacity;

  /**
   * The loaded buffer (reused each time load() is called)
   */
  #if defined(_MSC_VER) && _MSC_VER < 1910
  // older versions of Visual Studio lack proper support for unique_ptr.
  std::unique_ptr<char[]> loaded_bytes;
  #else
  std::unique_ptr<char[], decltype(&aligned_free_char)> loaded_bytes;
  #endif

  /** Capacity of loaded_bytes buffer. */
  size_t _loaded_bytes_capacity{0};

  // all nodes are stored on the doc.tape using a 64-bit word.
  //
  // strings, double and ints are stored as
  //  a 64-bit word with a pointer to the actual value
  //
  //
  //
  // for objects or arrays, store [ or {  at the beginning and } and ] at the
  // end. For the openings ([ or {), we annotate them with a reference to the
  // location on the doc.tape of the end, and for then closings (} and ]), we
  // annotate them with a reference to the location of the opening
  //
  //

  /**
   * Ensure we have enough capacity to handle at least desired_capacity bytes,
   * and auto-allocate if not.
   */
  inline error_code ensure_capacity(size_t desired_capacity) noexcept;

  /** Read the file into loaded_bytes */
  inline simdjson_result<size_t> read_file(const std::string &path) noexcept;

  friend class parser::Iterator;
  friend class document_stream;
}; // class parser

} // namespace dom
} // namespace simdjson

#endif // SIMDJSON_DOM_PARSER_H
/* end file include/simdjson/dom/document.h */
#ifdef SIMDJSON_THREADS_ENABLED
#include <thread>
#include <mutex>
#include <condition_variable>
#endif

namespace simdjson {
namespace dom {


#ifdef SIMDJSON_THREADS_ENABLED
/** @private Custom worker class **/
struct stage1_worker {
  stage1_worker() noexcept = default;
  stage1_worker(const stage1_worker&) = delete;
  stage1_worker(stage1_worker&&) = delete;
  stage1_worker operator=(const stage1_worker&) = delete;
  ~stage1_worker();
  /**
   * We only start the thread when it is needed, not at object construction, this may throw.
   * You should only call this once.
   **/
  void start_thread();
  /**
   * Start a stage 1 job. You should first call 'run', then 'finish'.
   * You must call start_thread once before.
   */
  void run(document_stream * ds, dom::parser * stage1, size_t next_batch_start);
  /** Wait for the run to finish (blocking). You should first call 'run', then 'finish'. **/
  void finish();

private:

  /**
   * Normally, we would never stop the thread. But we do in the destructor.
   * This function is only safe assuming that you are not waiting for results. You
   * should have called run, then finish, and be done.
   **/
  void stop_thread();

  std::thread thread{};
  /** These three variables define the work done by the thread. **/
  dom::parser * stage1_thread_parser{};
  size_t _next_batch_start{};
  document_stream * owner{};
  /**
   * We have two state variables. This could be streamlined to one variable in the future but
   * we use two for clarity.
   */
  bool has_work{false};
  bool can_work{true};

  /**
   * We lock using a mutex.
   */
  std::mutex locking_mutex{};
  std::condition_variable cond_var{};
};
#endif

/**
 * A forward-only stream of documents.
 *
 * Produced by parser::parse_many.
 *
 */
class document_stream {
public:
  /**
   * Construct an uninitialized document_stream.
   *
   *  ```c++
   *  document_stream docs;
   *  error = parser.parse_many(json).get(docs);
   *  ```
   */
  really_inline document_stream() noexcept;
  /** Move one document_stream to another. */
  really_inline document_stream(document_stream &&other) noexcept = default;
  /** Move one document_stream to another. */
  really_inline document_stream &operator=(document_stream &&other) noexcept = default;

  really_inline ~document_stream() noexcept;

  /**
   * An iterator through a forward-only stream of documents.
   */
  class iterator {
  public:
    /**
     * Get the current document (or error).
     */
    really_inline simdjson_result<element> operator*() noexcept;
    /**
     * Advance to the next document.
     */
    inline iterator& operator++() noexcept;
    /**
     * Check if we're at the end yet.
     * @param other the end iterator to compare to.
     */
    really_inline bool operator!=(const iterator &other) const noexcept;
    /**
     * @private
     *
     * Gives the current index in the input document in bytes.
     *
     *   document_stream stream = parser.parse_many(json,window);
     *   for(auto i = stream.begin(); i != stream.end(); ++i) {
     *      auto doc = *i;
     *      size_t index = i.current_index();
     *   }
     *
     * This function (current_index()) is experimental and the usage
     * may change in future versions of simdjson: we find the API somewhat
     * awkward and we would like to offer something friendlier.
     */
     really_inline size_t current_index() noexcept;
  private:
    really_inline iterator(document_stream &s, bool finished) noexcept;
    /** The document_stream we're iterating through. */
    document_stream& stream;
    /** Whether we're finished or not. */
    bool finished;
    friend class document_stream;
  };

  /**
   * Start iterating the documents in the stream.
   */
  really_inline iterator begin() noexcept;
  /**
   * The end of the stream, for iterator comparison purposes.
   */
  really_inline iterator end() noexcept;

private:

  document_stream &operator=(const document_stream &) = delete; // Disallow copying
  document_stream(const document_stream &other) = delete; // Disallow copying

  /**
   * Construct a document_stream. Does not allocate or parse anything until the iterator is
   * used.
   */
  really_inline document_stream(
    dom::parser &parser,
    const uint8_t *buf,
    size_t len,
    size_t batch_size
  ) noexcept;

  /**
   * Parse the first document in the buffer. Used by begin(), to handle allocation and
   * initialization.
   */
  inline void start() noexcept;

  /**
   * Parse the next document found in the buffer previously given to document_stream.
   *
   * The content should be a valid JSON document encoded as UTF-8. If there is a
   * UTF-8 BOM, the caller is responsible for omitting it, UTF-8 BOM are
   * discouraged.
   *
   * You do NOT need to pre-allocate a parser.  This function takes care of
   * pre-allocating a capacity defined by the batch_size defined when creating the
   * document_stream object.
   *
   * The function returns simdjson::EMPTY if there is no more data to be parsed.
   *
   * The function returns simdjson::SUCCESS (as integer = 0) in case of success
   * and indicates that the buffer has successfully been parsed to the end.
   * Every document it contained has been parsed without error.
   *
   * The function returns an error code from simdjson/simdjson.h in case of failure
   * such as simdjson::CAPACITY, simdjson::MEMALLOC, simdjson::DEPTH_ERROR and so forth;
   * the simdjson::error_message function converts these error codes into a string).
   *
   * You can also check validity by calling parser.is_valid(). The same parser can
   * and should be reused for the other documents in the buffer.
   */
  inline void next() noexcept;

  /**
   * Pass the next batch through stage 1 and return when finished.
   * When threads are enabled, this may wait for the stage 1 thread to finish.
   */
  inline void load_batch() noexcept;

  /** Get the next document index. */
  inline size_t next_batch_start() const noexcept;

  /** Pass the next batch through stage 1 with the given parser. */
  inline error_code run_stage1(dom::parser &p, size_t batch_start) noexcept;

  dom::parser *parser;
  const uint8_t *buf;
  size_t len;
  size_t batch_size;
  /** The error (or lack thereof) from the current document. */
  error_code error;
  size_t batch_start{0};
  size_t doc_index{};

#ifdef SIMDJSON_THREADS_ENABLED
  inline void load_from_stage1_thread() noexcept;

  /** Start a thread to run stage 1 on the next batch. */
  inline void start_stage1_thread() noexcept;

  /** Wait for the stage 1 thread to finish and capture the results. */
  inline void finish_stage1_thread() noexcept;

  /** The error returned from the stage 1 thread. */
  error_code stage1_thread_error{UNINITIALIZED};
  /** The thread used to run stage 1 against the next batch in the background. */
  friend struct stage1_worker;
  std::unique_ptr<stage1_worker> worker{new(std::nothrow) stage1_worker()};
  /**
   * The parser used to run stage 1 in the background. Will be swapped
   * with the regular parser when finished.
   */
  dom::parser stage1_thread_parser{};
#endif // SIMDJSON_THREADS_ENABLED

  friend class dom::parser;
  friend struct simdjson_result<dom::document_stream>;
  friend struct internal::simdjson_result_base<dom::document_stream>;

}; // class document_stream

} // namespace dom

template<>
struct simdjson_result<dom::document_stream> : public internal::simdjson_result_base<dom::document_stream> {
public:
  really_inline simdjson_result() noexcept; ///< @private
  really_inline simdjson_result(error_code error) noexcept; ///< @private
  really_inline simdjson_result(dom::document_stream &&value) noexcept; ///< @private

#if SIMDJSON_EXCEPTIONS
  really_inline dom::document_stream::iterator begin() noexcept(false);
  really_inline dom::document_stream::iterator end() noexcept(false);
#else // SIMDJSON_EXCEPTIONS
  [[deprecated("parse_many() and load_many() may return errors. Use document_stream stream; error = parser.parse_many().get(doc); instead.")]]
  really_inline dom::document_stream::iterator begin() noexcept;
  [[deprecated("parse_many() and load_many() may return errors. Use document_stream stream; error = parser.parse_many().get(doc); instead.")]]
  really_inline dom::document_stream::iterator end() noexcept;
#endif // SIMDJSON_EXCEPTIONS
}; // struct simdjson_result<dom::document_stream>

} // namespace simdjson

#endif // SIMDJSON_DOCUMENT_STREAM_H
/* end file include/simdjson/dom/document.h */
/* begin file include/simdjson/dom/element.h */
#ifndef SIMDJSON_DOM_ELEMENT_H
#define SIMDJSON_DOM_ELEMENT_H

#include <ostream>

namespace simdjson {
namespace dom {

class array;
class document;
class object;

/**
 * The actual concrete type of a JSON element
 * This is the type it is most easily cast to with get<>.
 */
enum class element_type {
  ARRAY = '[',     ///< dom::array
  OBJECT = '{',    ///< dom::object
  INT64 = 'l',     ///< int64_t
  UINT64 = 'u',    ///< uint64_t: any integer that fits in uint64_t but *not* int64_t
  DOUBLE = 'd',    ///< double: Any number with a "." or "e" that fits in double.
  STRING = '"',    ///< std::string_view
  BOOL = 't',      ///< bool
  NULL_VALUE = 'n' ///< null
};

/**
 * A JSON element.
 *
 * References an element in a JSON document, representing a JSON null, boolean, string, number,
 * array or object.
 */
class element {
public:
  /** Create a new, invalid element. */
  really_inline element() noexcept;

  /** The type of this element. */
  really_inline element_type type() const noexcept;

  /**
   * Cast this element to an array.
   *
   * Equivalent to get<array>().
   *
   * @returns An object that can be used to iterate the array, or:
   *          INCORRECT_TYPE if the JSON element is not an array.
   */
  inline simdjson_result<array> get_array() const noexcept;
  /**
   * Cast this element to an object.
   *
   * Equivalent to get<object>().
   *
   * @returns An object that can be used to look up or iterate the object's fields, or:
   *          INCORRECT_TYPE if the JSON element is not an object.
   */
  inline simdjson_result<object> get_object() const noexcept;
  /**
   * Cast this element to a null-terminated C string.
   *
   * The string is guaranteed to be valid UTF-8.
   *
   * The get_c_str() function is equivalent to get<const char *>().
   *
   * The length of the string is given by get_string_length(). Because JSON strings
   * may contain null characters, it may be incorrect to use strlen to determine the
   * string length.
   *
   * It is possible to get a single string_view instance which represents both the string
   * content and its length: see get_string().
   *
   * @returns A pointer to a null-terminated UTF-8 string. This string is stored in the parser and will
   *          be invalidated the next time it parses a document or when it is destroyed.
   *          Returns INCORRECT_TYPE if the JSON element is not a string.
   */
  inline simdjson_result<const char *> get_c_str() const noexcept;
  /**
   * Gives the length in bytes of the string.
   *
   * It is possible to get a single string_view instance which represents both the string
   * content and its length: see get_string().
   *
   * @returns A string length in bytes.
   *          Returns INCORRECT_TYPE if the JSON element is not a string.
   */
  inline simdjson_result<size_t> get_string_length() const noexcept;
  /**
   * Cast this element to a string.
   *
   * The string is guaranteed to be valid UTF-8.
   *
   * Equivalent to get<std::string_view>().
   *
   * @returns An UTF-8 string. The string is stored in the parser and will be invalidated the next time it
   *          parses a document or when it is destroyed.
   *          Returns INCORRECT_TYPE if the JSON element is not a string.
   */
  inline simdjson_result<std::string_view> get_string() const noexcept;
  /**
   * Cast this element to a signed integer.
   *
   * Equivalent to get<int64_t>().
   *
   * @returns A signed 64-bit integer.
   *          Returns INCORRECT_TYPE if the JSON element is not an integer, or NUMBER_OUT_OF_RANGE
   *          if it is negative.
   */
  inline simdjson_result<int64_t> get_int64() const noexcept;
  /**
   * Cast this element to an unsigned integer.
   *
   * Equivalent to get<uint64_t>().
   *
   * @returns An unsigned 64-bit integer.
   *          Returns INCORRECT_TYPE if the JSON element is not an integer, or NUMBER_OUT_OF_RANGE
   *          if it is too large.
   */
  inline simdjson_result<uint64_t> get_uint64() const noexcept;
  /**
   * Cast this element to an double floating-point.
   *
   * Equivalent to get<double>().
   *
   * @returns A double value.
   *          Returns INCORRECT_TYPE if the JSON element is not a number.
   */
  inline simdjson_result<double> get_double() const noexcept;
  /**
   * Cast this element to a bool.
   *
   * Equivalent to get<bool>().
   *
   * @returns A bool value.
   *          Returns INCORRECT_TYPE if the JSON element is not a boolean.
   */
  inline simdjson_result<bool> get_bool() const noexcept;

  /**
   * Whether this element is a json array.
   *
   * Equivalent to is<array>().
   */
  inline bool is_array() const noexcept;
  /**
   * Whether this element is a json object.
   *
   * Equivalent to is<object>().
   */
  inline bool is_object() const noexcept;
  /**
   * Whether this element is a json string.
   *
   * Equivalent to is<std::string_view>() or is<const char *>().
   */
  inline bool is_string() const noexcept;
  /**
   * Whether this element is a json number that fits in a signed 64-bit integer.
   *
   * Equivalent to is<int64_t>().
   */
  inline bool is_int64() const noexcept;
  /**
   * Whether this element is a json number that fits in an unsigned 64-bit integer.
   *
   * Equivalent to is<uint64_t>().
   */
  inline bool is_uint64() const noexcept;
  /**
   * Whether this element is a json number that fits in a double.
   *
   * Equivalent to is<double>().
   */
  inline bool is_double() const noexcept;
  /**
   * Whether this element is a json number.
   *
   * Both integers and floating points will return true.
   */
  inline bool is_number() const noexcept;
  /**
   * Whether this element is a json `true` or `false`.
   *
   * Equivalent to is<bool>().
   */
  inline bool is_bool() const noexcept;
  /**
   * Whether this element is a json `null`.
   */
  inline bool is_null() const noexcept;

  /**
   * Tell whether the value can be cast to provided type (T).
   *
   * Supported types:
   * - Boolean: bool
   * - Number: double, uint64_t, int64_t
   * - String: std::string_view, const char *
   * - Array: dom::array
   * - Object: dom::object
   *
   * @tparam T bool, double, uint64_t, int64_t, std::string_view, const char *, dom::array, dom::object
   */
  template<typename T>
  really_inline bool is() const noexcept;

  /**
   * Get the value as the provided type (T).
   *
   * Supported types:
   * - Boolean: bool
   * - Number: double, uint64_t, int64_t
   * - String: std::string_view, const char *
   * - Array: dom::array
   * - Object: dom::object
   *
   * @tparam T bool, double, uint64_t, int64_t, std::string_view, const char *, dom::array, dom::object
   *
   * @returns The value cast to the given type, or:
   *          INCORRECT_TYPE if the value cannot be cast to the given type.
   */
  template<typename T>
  inline simdjson_result<T> get() const noexcept;

  /**
   * Get the value as the provided type (T).
   *
   * Supported types:
   * - Boolean: bool
   * - Number: double, uint64_t, int64_t
   * - String: std::string_view, const char *
   * - Array: dom::array
   * - Object: dom::object
   *
   * @tparam T bool, double, uint64_t, int64_t, std::string_view, const char *, dom::array, dom::object
   *
   * @param value The variable to set to the value. May not be set if there is an error.
   *
   * @returns The error that occurred, or SUCCESS if there was no error.
   */
  template<typename T>
  WARN_UNUSED really_inline error_code get(T &value) const noexcept;

  /**
   * Get the value as the provided type (T), setting error if it's not the given type.
   *
   * Supported types:
   * - Boolean: bool
   * - Number: double, uint64_t, int64_t
   * - String: std::string_view, const char *
   * - Array: dom::array
   * - Object: dom::object
   *
   * @tparam T bool, double, uint64_t, int64_t, std::string_view, const char *, dom::array, dom::object
   *
   * @param value The variable to set to the given type. value is undefined if there is an error.
   * @param error The variable to store the error. error is set to error_code::SUCCEED if there is an error.
   */
  template<typename T>
  inline void tie(T &value, error_code &error) && noexcept;

#if SIMDJSON_EXCEPTIONS
  /**
   * Read this element as a boolean.
   *
   * @return The boolean value
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not a boolean.
   */
  inline operator bool() const noexcept(false);

  /**
   * Read this element as a null-terminated UTF-8 string.
   *
   * Be mindful that JSON allows strings to contain null characters.
   *
   * Does *not* convert other types to a string; requires that the JSON type of the element was
   * an actual string.
   *
   * @return The string value.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not a string.
   */
  inline explicit operator const char*() const noexcept(false);

  /**
   * Read this element as a null-terminated UTF-8 string.
   *
   * Does *not* convert other types to a string; requires that the JSON type of the element was
   * an actual string.
   *
   * @return The string value.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not a string.
   */
  inline operator std::string_view() const noexcept(false);

  /**
   * Read this element as an unsigned integer.
   *
   * @return The integer value.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not an integer
   * @exception simdjson_error(NUMBER_OUT_OF_RANGE) if the integer doesn't fit in 64 bits or is negative
   */
  inline operator uint64_t() const noexcept(false);
  /**
   * Read this element as an signed integer.
   *
   * @return The integer value.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not an integer
   * @exception simdjson_error(NUMBER_OUT_OF_RANGE) if the integer doesn't fit in 64 bits
   */
  inline operator int64_t() const noexcept(false);
  /**
   * Read this element as an double.
   *
   * @return The double value.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not a number
   * @exception simdjson_error(NUMBER_OUT_OF_RANGE) if the integer doesn't fit in 64 bits or is negative
   */
  inline operator double() const noexcept(false);
  /**
   * Read this element as a JSON array.
   *
   * @return The JSON array.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not an array
   */
  inline operator array() const noexcept(false);
  /**
   * Read this element as a JSON object (key/value pairs).
   *
   * @return The JSON object.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not an object
   */
  inline operator object() const noexcept(false);

  /**
   * Iterate over each element in this array.
   *
   * @return The beginning of the iteration.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not an array
   */
  inline dom::array::iterator begin() const noexcept(false);

  /**
   * Iterate over each element in this array.
   *
   * @return The end of the iteration.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON element is not an array
   */
  inline dom::array::iterator end() const noexcept(false);
#endif // SIMDJSON_EXCEPTIONS

  /**
   * Get the value associated with the given key.
   *
   * The key will be matched against **unescaped** JSON:
   *
   *   dom::parser parser;
   *   parser.parse(R"({ "a\n": 1 })"_padded)["a\n"].get<uint64_t>().first == 1
   *   parser.parse(R"({ "a\n": 1 })"_padded)["a\\n"].get<uint64_t>().error() == NO_SUCH_FIELD
   *
   * @return The value associated with this field, or:
   *         - NO_SUCH_FIELD if the field does not exist in the object
   *         - INCORRECT_TYPE if this is not an object
   */
  inline simdjson_result<element> operator[](const std::string_view &key) const noexcept;

  /**
   * Get the value associated with the given key.
   *
   * The key will be matched against **unescaped** JSON:
   *
   *   dom::parser parser;
   *   parser.parse(R"({ "a\n": 1 })"_padded)["a\n"].get<uint64_t>().first == 1
   *   parser.parse(R"({ "a\n": 1 })"_padded)["a\\n"].get<uint64_t>().error() == NO_SUCH_FIELD
   *
   * @return The value associated with this field, or:
   *         - NO_SUCH_FIELD if the field does not exist in the object
   *         - INCORRECT_TYPE if this is not an object
   */
  inline simdjson_result<element> operator[](const char *key) const noexcept;

  /**
   * Get the value associated with the given JSON pointer.
   *
   *   dom::parser parser;
   *   element doc = parser.parse(R"({ "foo": { "a": [ 10, 20, 30 ] }})"_padded);
   *   doc.at("/foo/a/1") == 20
   *   doc.at("/")["foo"]["a"].at(1) == 20
   *   doc.at("")["foo"]["a"].at(1) == 20
   *
   * @return The value associated with the given JSON pointer, or:
   *         - NO_SUCH_FIELD if a field does not exist in an object
   *         - INDEX_OUT_OF_BOUNDS if an array index is larger than an array length
   *         - INCORRECT_TYPE if a non-integer is used to access an array
   *         - INVALID_JSON_POINTER if the JSON pointer is invalid and cannot be parsed
   */
  inline simdjson_result<element> at(const std::string_view &json_pointer) const noexcept;

  /**
   * Get the value at the given index.
   *
   * @return The value at the given index, or:
   *         - INDEX_OUT_OF_BOUNDS if the array index is larger than an array length
   */
  inline simdjson_result<element> at(size_t index) const noexcept;

  /**
   * Get the value associated with the given key.
   *
   * The key will be matched against **unescaped** JSON:
   *
   *   dom::parser parser;
   *   parser.parse(R"({ "a\n": 1 })"_padded)["a\n"].get<uint64_t>().first == 1
   *   parser.parse(R"({ "a\n": 1 })"_padded)["a\\n"].get<uint64_t>().error() == NO_SUCH_FIELD
   *
   * @return The value associated with this field, or:
   *         - NO_SUCH_FIELD if the field does not exist in the object
   */
  inline simdjson_result<element> at_key(const std::string_view &key) const noexcept;

  /**
   * Get the value associated with the given key in a case-insensitive manner.
   *
   * Note: The key will be matched against **unescaped** JSON.
   *
   * @return The value associated with this field, or:
   *         - NO_SUCH_FIELD if the field does not exist in the object
   */
  inline simdjson_result<element> at_key_case_insensitive(const std::string_view &key) const noexcept;

  /** @private for debugging. Prints out the root element. */
  inline bool dump_raw_tape(std::ostream &out) const noexcept;

private:
  really_inline element(const internal::tape_ref &tape) noexcept;
  internal::tape_ref tape;
  friend class document;
  friend class object;
  friend class array;
  friend struct simdjson_result<element>;
  template<typename T>
  friend class simdjson::minifier;
};

/**
 * Print JSON to an output stream.
 *
 * By default, the value will be printed minified.
 *
 * @param out The output stream.
 * @param value The value to print.
 * @throw if there is an error with the underlying output stream. simdjson itself will not throw.
 */
inline std::ostream& operator<<(std::ostream& out, const element &value);

/**
 * Print element type to an output stream.
 *
 * @param out The output stream.
 * @param value The value to print.
 * @throw if there is an error with the underlying output stream. simdjson itself will not throw.
 */
inline std::ostream& operator<<(std::ostream& out, element_type type);

} // namespace dom

/** The result of a JSON navigation that may fail. */
template<>
struct simdjson_result<dom::element> : public internal::simdjson_result_base<dom::element> {
public:
  really_inline simdjson_result() noexcept; ///< @private
  really_inline simdjson_result(dom::element &&value) noexcept; ///< @private
  really_inline simdjson_result(error_code error) noexcept; ///< @private

  really_inline simdjson_result<dom::element_type> type() const noexcept;
  template<typename T>
  really_inline bool is() const noexcept;
  template<typename T>
  really_inline simdjson_result<T> get() const noexcept;
  template<typename T>
  WARN_UNUSED really_inline error_code get(T &value) const noexcept;

  really_inline simdjson_result<dom::array> get_array() const noexcept;
  really_inline simdjson_result<dom::object> get_object() const noexcept;
  really_inline simdjson_result<const char *> get_c_str() const noexcept;
  really_inline simdjson_result<size_t> get_string_length() const noexcept;
  really_inline simdjson_result<std::string_view> get_string() const noexcept;
  really_inline simdjson_result<int64_t> get_int64() const noexcept;
  really_inline simdjson_result<uint64_t> get_uint64() const noexcept;
  really_inline simdjson_result<double> get_double() const noexcept;
  really_inline simdjson_result<bool> get_bool() const noexcept;

  really_inline bool is_array() const noexcept;
  really_inline bool is_object() const noexcept;
  really_inline bool is_string() const noexcept;
  really_inline bool is_int64() const noexcept;
  really_inline bool is_uint64() const noexcept;
  really_inline bool is_double() const noexcept;
  really_inline bool is_bool() const noexcept;
  really_inline bool is_null() const noexcept;

  really_inline simdjson_result<dom::element> operator[](const std::string_view &key) const noexcept;
  really_inline simdjson_result<dom::element> operator[](const char *key) const noexcept;
  really_inline simdjson_result<dom::element> at(const std::string_view &json_pointer) const noexcept;
  really_inline simdjson_result<dom::element> at(size_t index) const noexcept;
  really_inline simdjson_result<dom::element> at_key(const std::string_view &key) const noexcept;
  really_inline simdjson_result<dom::element> at_key_case_insensitive(const std::string_view &key) const noexcept;

#if SIMDJSON_EXCEPTIONS
  really_inline operator bool() const noexcept(false);
  really_inline explicit operator const char*() const noexcept(false);
  really_inline operator std::string_view() const noexcept(false);
  really_inline operator uint64_t() const noexcept(false);
  really_inline operator int64_t() const noexcept(false);
  really_inline operator double() const noexcept(false);
  really_inline operator dom::array() const noexcept(false);
  really_inline operator dom::object() const noexcept(false);

  really_inline dom::array::iterator begin() const noexcept(false);
  really_inline dom::array::iterator end() const noexcept(false);
#endif // SIMDJSON_EXCEPTIONS
};

#if SIMDJSON_EXCEPTIONS
/**
 * Print JSON to an output stream.
 *
 * By default, the value will be printed minified.
 *
 * @param out The output stream.
 * @param value The value to print.
 * @throw simdjson_error if the result being printed has an error. If there is an error with the
 *        underlying output stream, that error will be propagated (simdjson_error will not be
 *        thrown).
 */
really_inline std::ostream& operator<<(std::ostream& out, const simdjson_result<dom::element> &value) noexcept(false);
#endif

} // namespace simdjson

#endif // SIMDJSON_DOM_DOCUMENT_H
/* end file include/simdjson/dom/element.h */
/* begin file include/simdjson/dom/object.h */
#ifndef SIMDJSON_DOM_OBJECT_H
#define SIMDJSON_DOM_OBJECT_H

#include <ostream>

namespace simdjson {
namespace dom {

class document;
class element;
class key_value_pair;

/**
 * JSON object.
 */
class object {
public:
  /** Create a new, invalid object */
  really_inline object() noexcept;

  class iterator {
  public:
    /**
     * Get the actual key/value pair
     */
    inline const key_value_pair operator*() const noexcept;
    /**
     * Get the next key/value pair.
     *
     * Part of the std::iterator interface.
     *
     */
    inline iterator& operator++() noexcept;
    /**
     * Check if these key value pairs come from the same place in the JSON.
     *
     * Part of the std::iterator interface.
     */
    inline bool operator!=(const iterator& other) const noexcept;
    /**
     * Get the key of this key/value pair.
     */
    inline std::string_view key() const noexcept;
    /**
     * Get the length (in bytes) of the key in this key/value pair.
     * You should expect this function to be faster than key().size().
     */
    inline uint32_t key_length() const noexcept;
    /**
     * Returns true if the key in this key/value pair is equal
     * to the provided string_view.
     */
    inline bool key_equals(const std::string_view & o) const noexcept;
    /**
     * Returns true if the key in this key/value pair is equal
     * to the provided string_view in a case-insensitive manner.
     * Case comparisons may only be handled correctly for ASCII strings.
     */
    inline bool key_equals_case_insensitive(const std::string_view & o) const noexcept;
    /**
     * Get the key of this key/value pair.
     */
    inline const char *key_c_str() const noexcept;
    /**
     * Get the value of this key/value pair.
     */
    inline element value() const noexcept;
  private:
    really_inline iterator(const internal::tape_ref &tape) noexcept;

    internal::tape_ref tape;

    friend class object;
  };

  /**
   * Return the first key/value pair.
   *
   * Part of the std::iterable interface.
   */
  inline iterator begin() const noexcept;
  /**
   * One past the last key/value pair.
   *
   * Part of the std::iterable interface.
   */
  inline iterator end() const noexcept;
  /**
   * Get the size of the object (number of keys).
   * It is a saturated value with a maximum of 0xFFFFFF: if the value
   * is 0xFFFFFF then the size is 0xFFFFFF or greater.
   */
  inline size_t size() const noexcept;
  /**
   * Get the value associated with the given key.
   *
   * The key will be matched against **unescaped** JSON:
   *
   *   dom::parser parser;
   *   parser.parse(R"({ "a\n": 1 })"_padded)["a\n"].get<uint64_t>().first == 1
   *   parser.parse(R"({ "a\n": 1 })"_padded)["a\\n"].get<uint64_t>().error() == NO_SUCH_FIELD
   *
   * This function has linear-time complexity: the keys are checked one by one.
   *
   * @return The value associated with this field, or:
   *         - NO_SUCH_FIELD if the field does not exist in the object
   *         - INCORRECT_TYPE if this is not an object
   */
  inline simdjson_result<element> operator[](const std::string_view &key) const noexcept;

  /**
   * Get the value associated with the given key.
   *
   * The key will be matched against **unescaped** JSON:
   *
   *   dom::parser parser;
   *   parser.parse(R"({ "a\n": 1 })"_padded)["a\n"].get<uint64_t>().first == 1
   *   parser.parse(R"({ "a\n": 1 })"_padded)["a\\n"].get<uint64_t>().error() == NO_SUCH_FIELD
   *
   * This function has linear-time complexity: the keys are checked one by one.
   *
   * @return The value associated with this field, or:
   *         - NO_SUCH_FIELD if the field does not exist in the object
   *         - INCORRECT_TYPE if this is not an object
   */
  inline simdjson_result<element> operator[](const char *key) const noexcept;

  /**
   * Get the value associated with the given JSON pointer.
   *
   *   dom::parser parser;
   *   object obj = parser.parse(R"({ "foo": { "a": [ 10, 20, 30 ] }})"_padded);
   *   obj.at("foo/a/1") == 20
   *   obj.at("foo")["a"].at(1) == 20
   *
   * @return The value associated with the given JSON pointer, or:
   *         - NO_SUCH_FIELD if a field does not exist in an object
   *         - INDEX_OUT_OF_BOUNDS if an array index is larger than an array length
   *         - INCORRECT_TYPE if a non-integer is used to access an array
   *         - INVALID_JSON_POINTER if the JSON pointer is invalid and cannot be parsed
   */
  inline simdjson_result<element> at(const std::string_view &json_pointer) const noexcept;

  /**
   * Get the value associated with the given key.
   *
   * The key will be matched against **unescaped** JSON:
   *
   *   dom::parser parser;
   *   parser.parse(R"({ "a\n": 1 })"_padded)["a\n"].get<uint64_t>().first == 1
   *   parser.parse(R"({ "a\n": 1 })"_padded)["a\\n"].get<uint64_t>().error() == NO_SUCH_FIELD
   *
   * This function has linear-time complexity: the keys are checked one by one.
   *
   * @return The value associated with this field, or:
   *         - NO_SUCH_FIELD if the field does not exist in the object
   */
  inline simdjson_result<element> at_key(const std::string_view &key) const noexcept;

  /**
   * Get the value associated with the given key in a case-insensitive manner.
   * It is only guaranteed to work over ASCII inputs.
   *
   * Note: The key will be matched against **unescaped** JSON.
   *
   * This function has linear-time complexity: the keys are checked one by one.
   *
   * @return The value associated with this field, or:
   *         - NO_SUCH_FIELD if the field does not exist in the object
   */
  inline simdjson_result<element> at_key_case_insensitive(const std::string_view &key) const noexcept;

private:
  really_inline object(const internal::tape_ref &tape) noexcept;

  internal::tape_ref tape;

  friend class element;
  friend struct simdjson_result<element>;
  template<typename T>
  friend class simdjson::minifier;
};

/**
 * Key/value pair in an object.
 */
class key_value_pair {
public:
  /** key in the key-value pair **/
  std::string_view key;
  /** value in the key-value pair **/
  element value;

private:
  really_inline key_value_pair(const std::string_view &_key, element _value) noexcept;
  friend class object;
};

/**
 * Print JSON to an output stream.
 *
 * By default, the value will be printed minified.
 *
 * @param out The output stream.
 * @param value The value to print.
 * @throw if there is an error with the underlying output stream. simdjson itself will not throw.
 */
inline std::ostream& operator<<(std::ostream& out, const object &value);
/**
 * Print JSON to an output stream.
 *
 * By default, the value will be printed minified.
 *
 * @param out The output stream.
 * @param value The value to print.
 * @throw if there is an error with the underlying output stream. simdjson itself will not throw.
 */
inline std::ostream& operator<<(std::ostream& out, const key_value_pair &value);

} // namespace dom

/** The result of a JSON conversion that may fail. */
template<>
struct simdjson_result<dom::object> : public internal::simdjson_result_base<dom::object> {
public:
  really_inline simdjson_result() noexcept; ///< @private
  really_inline simdjson_result(dom::object value) noexcept; ///< @private
  really_inline simdjson_result(error_code error) noexcept; ///< @private

  inline simdjson_result<dom::element> operator[](const std::string_view &key) const noexcept;
  inline simdjson_result<dom::element> operator[](const char *key) const noexcept;
  inline simdjson_result<dom::element> at(const std::string_view &json_pointer) const noexcept;
  inline simdjson_result<dom::element> at_key(const std::string_view &key) const noexcept;
  inline simdjson_result<dom::element> at_key_case_insensitive(const std::string_view &key) const noexcept;

#if SIMDJSON_EXCEPTIONS
  inline dom::object::iterator begin() const noexcept(false);
  inline dom::object::iterator end() const noexcept(false);
  inline size_t size() const noexcept(false);
#endif // SIMDJSON_EXCEPTIONS
};

#if SIMDJSON_EXCEPTIONS
/**
 * Print JSON to an output stream.
 *
 * By default, the value will be printed minified.
 *
 * @param out The output stream.
 * @param value The value to print.
 * @throw simdjson_error if the result being printed has an error. If there is an error with the
 *        underlying output stream, that error will be propagated (simdjson_error will not be
 *        thrown).
 */
inline std::ostream& operator<<(std::ostream& out, const simdjson_result<dom::object> &value) noexcept(false);
#endif // SIMDJSON_EXCEPTIONS

} // namespace simdjson

#endif // SIMDJSON_DOM_OBJECT_H
/* end file include/simdjson/dom/object.h */

// Deprecated API
/* begin file include/simdjson/dom/jsonparser.h */
// TODO Remove this -- deprecated API and files

#ifndef SIMDJSON_DOM_JSONPARSER_H
#define SIMDJSON_DOM_JSONPARSER_H

/* begin file include/simdjson/dom/parsedjson.h */
// TODO Remove this -- deprecated API and files

#ifndef SIMDJSON_DOM_PARSEDJSON_H
#define SIMDJSON_DOM_PARSEDJSON_H


namespace simdjson {

/**
 * @deprecated Use `dom::parser` instead.
 */
using ParsedJson [[deprecated("Use dom::parser instead")]] = dom::parser;

} // namespace simdjson

#endif // SIMDJSON_DOM_PARSEDJSON_H
/* end file include/simdjson/dom/parsedjson.h */
/* begin file include/simdjson/jsonioutil.h */
#ifndef SIMDJSON_JSONIOUTIL_H
#define SIMDJSON_JSONIOUTIL_H


namespace simdjson {

#if SIMDJSON_EXCEPTIONS

[[deprecated("Use padded_string::load() instead")]]
inline padded_string get_corpus(const char *path) {
  return padded_string::load(path);
}

#endif // SIMDJSON_EXCEPTIONS

} // namespace simdjson

#endif // SIMDJSON_JSONIOUTIL_H
/* end file include/simdjson/jsonioutil.h */

namespace simdjson {

//
// C API (json_parse and build_parsed_json) declarations
//

[[deprecated("Use parser.parse() instead")]]
inline int json_parse(const uint8_t *buf, size_t len, dom::parser &parser, bool realloc_if_needed = true) noexcept {
  error_code code = parser.parse(buf, len, realloc_if_needed).error();
  // The deprecated json_parse API is a signal that the user plans to *use* the error code / valid
  // bits in the parser instead of heeding the result code. The normal parser unsets those in
  // anticipation of making the error code ephemeral.
  // Here we put the code back into the parser, until we've removed this method.
  parser.valid = code == SUCCESS;
  parser.error = code;
  return code;
}
[[deprecated("Use parser.parse() instead")]]
inline int json_parse(const char *buf, size_t len, dom::parser &parser, bool realloc_if_needed = true) noexcept {
  error_code code = parser.parse(buf, len, realloc_if_needed).error();
  // The deprecated json_parse API is a signal that the user plans to *use* the error code / valid
  // bits in the parser instead of heeding the result code. The normal parser unsets those in
  // anticipation of making the error code ephemeral.
  // Here we put the code back into the parser, until we've removed this method.
  parser.valid = code == SUCCESS;
  parser.error = code;
  return code;
}
[[deprecated("Use parser.parse() instead")]]
inline int json_parse(const std::string &s, dom::parser &parser, bool realloc_if_needed = true) noexcept {
  error_code code = parser.parse(s.data(), s.length(), realloc_if_needed).error();
  // The deprecated json_parse API is a signal that the user plans to *use* the error code / valid
  // bits in the parser instead of heeding the result code. The normal parser unsets those in
  // anticipation of making the error code ephemeral.
  // Here we put the code back into the parser, until we've removed this method.
  parser.valid = code == SUCCESS;
  parser.error = code;
  return code;
}
[[deprecated("Use parser.parse() instead")]]
inline int json_parse(const padded_string &s, dom::parser &parser) noexcept {
  error_code code = parser.parse(s).error();
  // The deprecated json_parse API is a signal that the user plans to *use* the error code / valid
  // bits in the parser instead of heeding the result code. The normal parser unsets those in
  // anticipation of making the error code ephemeral.
  // Here we put the code back into the parser, until we've removed this method.
  parser.valid = code == SUCCESS;
  parser.error = code;
  return code;
}

[[deprecated("Use parser.parse() instead")]]
WARN_UNUSED inline dom::parser build_parsed_json(const uint8_t *buf, size_t len, bool realloc_if_needed = true) noexcept {
  dom::parser parser;
  error_code code = parser.parse(buf, len, realloc_if_needed).error();
  // The deprecated json_parse API is a signal that the user plans to *use* the error code / valid
  // bits in the parser instead of heeding the result code. The normal parser unsets those in
  // anticipation of making the error code ephemeral.
  // Here we put the code back into the parser, until we've removed this method.
  parser.valid = code == SUCCESS;
  parser.error = code;
  return parser;
}
[[deprecated("Use parser.parse() instead")]]
WARN_UNUSED inline dom::parser build_parsed_json(const char *buf, size_t len, bool realloc_if_needed = true) noexcept {
  dom::parser parser;
  error_code code = parser.parse(buf, len, realloc_if_needed).error();
  // The deprecated json_parse API is a signal that the user plans to *use* the error code / valid
  // bits in the parser instead of heeding the result code. The normal parser unsets those in
  // anticipation of making the error code ephemeral.
  // Here we put the code back into the parser, until we've removed this method.
  parser.valid = code == SUCCESS;
  parser.error = code;
  return parser;
}
[[deprecated("Use parser.parse() instead")]]
WARN_UNUSED inline dom::parser build_parsed_json(const std::string &s, bool realloc_if_needed = true) noexcept {
  dom::parser parser;
  error_code code = parser.parse(s.data(), s.length(), realloc_if_needed).error();
  // The deprecated json_parse API is a signal that the user plans to *use* the error code / valid
  // bits in the parser instead of heeding the result code. The normal parser unsets those in
  // anticipation of making the error code ephemeral.
  // Here we put the code back into the parser, until we've removed this method.
  parser.valid = code == SUCCESS;
  parser.error = code;
  return parser;
}
[[deprecated("Use parser.parse() instead")]]
WARN_UNUSED inline dom::parser build_parsed_json(const padded_string &s) noexcept {
  dom::parser parser;
  error_code code = parser.parse(s).error();
  // The deprecated json_parse API is a signal that the user plans to *use* the error code / valid
  // bits in the parser instead of heeding the result code. The normal parser unsets those in
  // anticipation of making the error code ephemeral.
  // Here we put the code back into the parser, until we've removed this method.
  parser.valid = code == SUCCESS;
  parser.error = code;
  return parser;
}

/** @private We do not want to allow implicit conversion from C string to std::string. */
int json_parse(const char *buf, dom::parser &parser) noexcept = delete;
/** @private We do not want to allow implicit conversion from C string to std::string. */
dom::parser build_parsed_json(const char *buf) noexcept = delete;

} // namespace simdjson

#endif // SIMDJSON_DOM_JSONPARSER_H
/* end file include/simdjson/jsonioutil.h */
/* begin file include/simdjson/dom/parsedjson_iterator.h */
// TODO Remove this -- deprecated API and files

#ifndef SIMDJSON_DOM_PARSEDJSON_ITERATOR_H
#define SIMDJSON_DOM_PARSEDJSON_ITERATOR_H

#include <cstring>
#include <string>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>

/* begin file include/simdjson/internal/jsonformatutils.h */
#ifndef SIMDJSON_INTERNAL_JSONFORMATUTILS_H
#define SIMDJSON_INTERNAL_JSONFORMATUTILS_H

#include <iomanip>
#include <iostream>
#include <sstream>

namespace simdjson {
namespace internal {

class escape_json_string;

inline std::ostream& operator<<(std::ostream& out, const escape_json_string &str);

class escape_json_string {
public:
  escape_json_string(std::string_view _str) noexcept : str{_str} {}
  operator std::string() const noexcept { std::stringstream s; s << *this; return s.str(); }
private:
  std::string_view str;
  friend std::ostream& operator<<(std::ostream& out, const escape_json_string &unescaped);
};

inline std::ostream& operator<<(std::ostream& out, const escape_json_string &unescaped) {
  for (size_t i=0; i<unescaped.str.length(); i++) {
    switch (unescaped.str[i]) {
    case '\b':
      out << "\\b";
      break;
    case '\f':
      out << "\\f";
      break;
    case '\n':
      out << "\\n";
      break;
    case '\r':
      out << "\\r";
      break;
    case '\"':
      out << "\\\"";
      break;
    case '\t':
      out << "\\t";
      break;
    case '\\':
      out << "\\\\";
      break;
    default:
      if ((unsigned char)unescaped.str[i] <= 0x1F) {
        // TODO can this be done once at the beginning, or will it mess up << char?
        std::ios::fmtflags f(out.flags());
        out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << int(unescaped.str[i]);
        out.flags(f);
      } else {
        out << unescaped.str[i];
      }
    }
  }
  return out;
}

} // namespace internal
} // namespace simdjson

#endif // SIMDJSON_INTERNAL_JSONFORMATUTILS_H
/* end file include/simdjson/internal/jsonformatutils.h */

namespace simdjson {

/** @private **/
class [[deprecated("Use the new DOM navigation API instead (see doc/basics.md)")]] dom::parser::Iterator {
public:
  inline Iterator(const dom::parser &parser) noexcept(false);
  inline Iterator(const Iterator &o) noexcept;
  inline ~Iterator() noexcept;

  inline Iterator& operator=(const Iterator&) = delete;

  inline bool is_ok() const;

  // useful for debugging purposes
  inline size_t get_tape_location() const;

  // useful for debugging purposes
  inline size_t get_tape_length() const;

  // returns the current depth (start at 1 with 0 reserved for the fictitious
  // root node)
  inline size_t get_depth() const;

  // A scope is a series of nodes at the same depth, typically it is either an
  // object ({) or an array ([). The root node has type 'r'.
  inline uint8_t get_scope_type() const;

  // move forward in document order
  inline bool move_forward();

  // retrieve the character code of what we're looking at:
  // [{"slutfn are the possibilities
  inline uint8_t get_type() const {
      return current_type; // short functions should be inlined!
  }

  // get the int64_t value at this node; valid only if get_type is "l"
  inline int64_t get_integer() const {
      if (location + 1 >= tape_length) {
      return 0; // default value in case of error
      }
      return static_cast<int64_t>(doc.tape[location + 1]);
  }

  // get the value as uint64; valid only if  if get_type is "u"
  inline uint64_t get_unsigned_integer() const {
      if (location + 1 >= tape_length) {
      return 0; // default value in case of error
      }
      return doc.tape[location + 1];
  }

  // get the string value at this node (NULL ended); valid only if get_type is "
  // note that tabs, and line endings are escaped in the returned value (see
  // print_with_escapes) return value is valid UTF-8, it may contain NULL chars
  // within the string: get_string_length determines the true string length.
  inline const char *get_string() const {
      return reinterpret_cast<const char *>(
          doc.string_buf.get() + (current_val & internal::JSON_VALUE_MASK) + sizeof(uint32_t));
  }

  // return the length of the string in bytes
  inline uint32_t get_string_length() const {
      uint32_t answer;
      memcpy(&answer,
          reinterpret_cast<const char *>(doc.string_buf.get() +
                                          (current_val & internal::JSON_VALUE_MASK)),
          sizeof(uint32_t));
      return answer;
  }

  // get the double value at this node; valid only if
  // get_type() is "d"
  inline double get_double() const {
      if (location + 1 >= tape_length) {
      return std::numeric_limits<double>::quiet_NaN(); // default value in
                                                      // case of error
      }
      double answer;
      memcpy(&answer, &doc.tape[location + 1], sizeof(answer));
      return answer;
  }

  inline bool is_object_or_array() const { return is_object() || is_array(); }

  inline bool is_object() const { return get_type() == '{'; }

  inline bool is_array() const { return get_type() == '['; }

  inline bool is_string() const { return get_type() == '"'; }

  // Returns true if the current type of the node is an signed integer.
  // You can get its value with `get_integer()`.
  inline bool is_integer() const { return get_type() == 'l'; }

  // Returns true if the current type of the node is an unsigned integer.
  // You can get its value with `get_unsigned_integer()`.
  //
  // NOTE:
  // Only a large value, which is out of range of a 64-bit signed integer, is
  // represented internally as an unsigned node. On the other hand, a typical
  // positive integer, such as 1, 42, or 1000000, is as a signed node.
  // Be aware this function returns false for a signed node.
  inline bool is_unsigned_integer() const { return get_type() == 'u'; }
  // Returns true if the current type of the node is a double floating-point number.
  inline bool is_double() const { return get_type() == 'd'; }
  // Returns true if the current type of the node is a number (integer or floating-point).
  inline bool is_number() const {
      return is_integer() || is_unsigned_integer() || is_double();
  }
  // Returns true if the current type of the node is a bool with true value.
  inline bool is_true() const { return get_type() == 't'; }
  // Returns true if the current type of the node is a bool with false value.
  inline bool is_false() const { return get_type() == 'f'; }
  // Returns true if the current type of the node is null.
  inline bool is_null() const { return get_type() == 'n'; }
  // Returns true if the type byte represents an object of an array
  static bool is_object_or_array(uint8_t type) {
      return ((type == '[') || (type == '{'));
  }

  // when at {, go one level deep, looking for a given key
  // if successful, we are left pointing at the value,
  // if not, we are still pointing at the object ({)
  // (in case of repeated keys, this only finds the first one).
  // We seek the key using C's strcmp so if your JSON strings contain
  // NULL chars, this would trigger a false positive: if you expect that
  // to be the case, take extra precautions.
  // Furthermore, we do the comparison character-by-character
  // without taking into account Unicode equivalence.
  inline bool move_to_key(const char *key);

  // as above, but case insensitive lookup (strcmpi instead of strcmp)
  inline bool move_to_key_insensitive(const char *key);

  // when at {, go one level deep, looking for a given key
  // if successful, we are left pointing at the value,
  // if not, we are still pointing at the object ({)
  // (in case of repeated keys, this only finds the first one).
  // The string we search for can contain NULL values.
  // Furthermore, we do the comparison character-by-character
  // without taking into account Unicode equivalence.
  inline bool move_to_key(const char *key, uint32_t length);

  // when at a key location within an object, this moves to the accompanying
  // value (located next to it). This is equivalent but much faster than
  // calling "next()".
  inline void move_to_value();

  // when at [, go one level deep, and advance to the given index.
  // if successful, we are left pointing at the value,
  // if not, we are still pointing at the array ([)
  inline bool move_to_index(uint32_t index);

  // Moves the iterator to the value corresponding to the json pointer.
  // Always search from the root of the document.
  // if successful, we are left pointing at the value,
  // if not, we are still pointing the same value we were pointing before the
  // call. The json pointer follows the rfc6901 standard's syntax:
  // https://tools.ietf.org/html/rfc6901 However, the standard says "If a
  // referenced member name is not unique in an object, the member that is
  // referenced is undefined, and evaluation fails". Here we just return the
  // first corresponding value. The length parameter is the length of the
  // jsonpointer string ('pointer').
  inline bool move_to(const char *pointer, uint32_t length);

  // Moves the iterator to the value corresponding to the json pointer.
  // Always search from the root of the document.
  // if successful, we are left pointing at the value,
  // if not, we are still pointing the same value we were pointing before the
  // call. The json pointer implementation follows the rfc6901 standard's
  // syntax: https://tools.ietf.org/html/rfc6901 However, the standard says
  // "If a referenced member name is not unique in an object, the member that
  // is referenced is undefined, and evaluation fails". Here we just return
  // the first corresponding value.
  inline bool move_to(const std::string &pointer) {
      return move_to(pointer.c_str(), uint32_t(pointer.length()));
  }

  private:
  // Almost the same as move_to(), except it searches from the current
  // position. The pointer's syntax is identical, though that case is not
  // handled by the rfc6901 standard. The '/' is still required at the
  // beginning. However, contrary to move_to(), the URI Fragment Identifier
  // Representation is not supported here. Also, in case of failure, we are
  // left pointing at the closest value it could reach. For these reasons it
  // is private. It exists because it is used by move_to().
  inline bool relative_move_to(const char *pointer, uint32_t length);

  public:
  // throughout return true if we can do the navigation, false
  // otherwise

  // Withing a given scope (series of nodes at the same depth within either an
  // array or an object), we move forward.
  // Thus, given [true, null, {"a":1}, [1,2]], we would visit true, null, {
  // and [. At the object ({) or at the array ([), you can issue a "down" to
  // visit their content. valid if we're not at the end of a scope (returns
  // true).
  inline bool next();

  // Within a given scope (series of nodes at the same depth within either an
  // array or an object), we move backward.
  // Thus, given [true, null, {"a":1}, [1,2]], we would visit ], }, null, true
  // when starting at the end of the scope. At the object ({) or at the array
  // ([), you can issue a "down" to visit their content.
  // Performance warning: This function is implemented by starting again
  // from the beginning of the scope and scanning forward. You should expect
  // it to be relatively slow.
  inline bool prev();

  // Moves back to either the containing array or object (type { or [) from
  // within a contained scope.
  // Valid unless we are at the first level of the document
  inline bool up();

  // Valid if we're at a [ or { and it starts a non-empty scope; moves us to
  // start of that deeper scope if it not empty. Thus, given [true, null,
  // {"a":1}, [1,2]], if we are at the { node, we would move to the "a" node.
  inline bool down();

  // move us to the start of our current scope,
  // a scope is a series of nodes at the same level
  inline void to_start_scope();

  inline void rewind() {
      while (up())
      ;
  }



  // print the node we are currently pointing at
  inline bool print(std::ostream &os, bool escape_strings = true) const;

  private:
  const document &doc;
  size_t max_depth{};
  size_t depth{};
  size_t location{}; // our current location on a tape
  size_t tape_length{};
  uint8_t current_type{};
  uint64_t current_val{};
  typedef struct {
      size_t start_of_scope;
      uint8_t scope_type;
  } scopeindex_t;

  scopeindex_t *depth_index{};
};

} // namespace simdjson

#endif // SIMDJSON_DOM_PARSEDJSON_ITERATOR_H
/* end file include/simdjson/internal/jsonformatutils.h */

// Inline functions
/* begin file include/simdjson/inline/array.h */
#ifndef SIMDJSON_INLINE_ARRAY_H
#define SIMDJSON_INLINE_ARRAY_H

// Inline implementations go in here.

#include <utility>

namespace simdjson {

//
// simdjson_result<dom::array> inline implementation
//
really_inline simdjson_result<dom::array>::simdjson_result() noexcept
    : internal::simdjson_result_base<dom::array>() {}
really_inline simdjson_result<dom::array>::simdjson_result(dom::array value) noexcept
    : internal::simdjson_result_base<dom::array>(std::forward<dom::array>(value)) {}
really_inline simdjson_result<dom::array>::simdjson_result(error_code error) noexcept
    : internal::simdjson_result_base<dom::array>(error) {}

#if SIMDJSON_EXCEPTIONS

inline dom::array::iterator simdjson_result<dom::array>::begin() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first.begin();
}
inline dom::array::iterator simdjson_result<dom::array>::end() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first.end();
}
inline size_t simdjson_result<dom::array>::size() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first.size();
}

#endif // SIMDJSON_EXCEPTIONS

inline simdjson_result<dom::element> simdjson_result<dom::array>::at(const std::string_view &json_pointer) const noexcept {
  if (error()) { return error(); }
  return first.at(json_pointer);
}
inline simdjson_result<dom::element> simdjson_result<dom::array>::at(size_t index) const noexcept {
  if (error()) { return error(); }
  return first.at(index);
}

namespace dom {

//
// array inline implementation
//
really_inline array::array() noexcept : tape{} {}
really_inline array::array(const internal::tape_ref &_tape) noexcept : tape{_tape} {}
inline array::iterator array::begin() const noexcept {
  return internal::tape_ref(tape.doc, tape.json_index + 1);
}
inline array::iterator array::end() const noexcept {
  return internal::tape_ref(tape.doc, tape.after_element() - 1);
}
inline size_t array::size() const noexcept {
  return tape.scope_count();
}
inline simdjson_result<element> array::at(const std::string_view &json_pointer) const noexcept {
  // - means "the append position" or "the element after the end of the array"
  // We don't support this, because we're returning a real element, not a position.
  if (json_pointer == "-") { return INDEX_OUT_OF_BOUNDS; }

  // Read the array index
  size_t array_index = 0;
  size_t i;
  for (i = 0; i < json_pointer.length() && json_pointer[i] != '/'; i++) {
    uint8_t digit = uint8_t(json_pointer[i] - '0');
    // Check for non-digit in array index. If it's there, we're trying to get a field in an object
    if (digit > 9) { return INCORRECT_TYPE; }
    array_index = array_index*10 + digit;
  }

  // 0 followed by other digits is invalid
  if (i > 1 && json_pointer[0] == '0') { return INVALID_JSON_POINTER; } // "JSON pointer array index has other characters after 0"

  // Empty string is invalid; so is a "/" with no digits before it
  if (i == 0) { return INVALID_JSON_POINTER; } // "Empty string in JSON pointer array index"

  // Get the child
  auto child = array(tape).at(array_index);
  // If there is a /, we're not done yet, call recursively.
  if (i < json_pointer.length()) {
    child = child.at(json_pointer.substr(i+1));
  }
  return child;
}
inline simdjson_result<element> array::at(size_t index) const noexcept {
  size_t i=0;
  for (auto element : *this) {
    if (i == index) { return element; }
    i++;
  }
  return INDEX_OUT_OF_BOUNDS;
}

//
// array::iterator inline implementation
//
really_inline array::iterator::iterator(const internal::tape_ref &_tape) noexcept : tape{_tape} { }
inline element array::iterator::operator*() const noexcept {
  return element(tape);
}
inline bool array::iterator::operator!=(const array::iterator& other) const noexcept {
  return tape.json_index != other.tape.json_index;
}
inline array::iterator& array::iterator::operator++() noexcept {
  tape.json_index = tape.after_element();
  return *this;
}

inline std::ostream& operator<<(std::ostream& out, const array &value) {
  return out << minify<array>(value);
}

} // namespace dom

template<>
inline std::ostream& minifier<dom::array>::print(std::ostream& out) {
  out << '[';
  auto iter = value.begin();
  auto end = value.end();
  if (iter != end) {
    out << minify<dom::element>(*iter);
    for (++iter; iter != end; ++iter) {
      out << "," << minify<dom::element>(*iter);
    }
  }
  return out << ']';
}

#if SIMDJSON_EXCEPTIONS

template<>
inline std::ostream& minifier<simdjson_result<dom::array>>::print(std::ostream& out) {
  if (value.error()) { throw simdjson_error(value.error()); }
  return out << minify<dom::array>(value.first);
}

inline std::ostream& operator<<(std::ostream& out, const simdjson_result<dom::array> &value) noexcept(false) {
  return out << minify<simdjson_result<dom::array>>(value);
}

#endif

} // namespace simdjson

#endif // SIMDJSON_INLINE_ARRAY_H
/* end file include/simdjson/inline/array.h */
/* begin file include/simdjson/inline/document_stream.h */
#ifndef SIMDJSON_INLINE_DOCUMENT_STREAM_H
#define SIMDJSON_INLINE_DOCUMENT_STREAM_H

#include <algorithm>
#include <limits>
#include <stdexcept>
namespace simdjson {
namespace dom {

#ifdef SIMDJSON_THREADS_ENABLED
inline void stage1_worker::finish() {
  std::unique_lock<std::mutex> lock(locking_mutex);
  cond_var.wait(lock, [this]{return has_work == false;});
}

inline stage1_worker::~stage1_worker() {
  stop_thread();
}

inline void stage1_worker::start_thread() {
  std::unique_lock<std::mutex> lock(locking_mutex);
  if(thread.joinable()) {
    return; // This should never happen but we never want to create more than one thread.
  }
  thread = std::thread([this]{
      while(can_work) {
        std::unique_lock<std::mutex> thread_lock(locking_mutex);
        cond_var.wait(thread_lock, [this]{return has_work || !can_work;});
        if(!can_work) {
          break;
        }
        this->owner->stage1_thread_error = this->owner->run_stage1(*this->stage1_thread_parser,
              this->_next_batch_start);
        this->has_work = false;
        thread_lock.unlock();
        cond_var.notify_one(); // will notify "finish"
      }
    }
  );
}


inline void stage1_worker::stop_thread() {
  std::unique_lock<std::mutex> lock(locking_mutex);
  // We have to make sure that all locks can be released.
  can_work = false;
  has_work = false;
  lock.unlock();
  cond_var.notify_all();
  if(thread.joinable()) {
    thread.join();
  }
}

inline void stage1_worker::run(document_stream * ds, dom::parser * stage1, size_t next_batch_start) {
  std::unique_lock<std::mutex> lock(locking_mutex);
  owner = ds;
  _next_batch_start = next_batch_start;
  stage1_thread_parser = stage1;
  has_work = true;
  lock.unlock();
  cond_var.notify_one();// will notify the thread lock
}
#endif

really_inline document_stream::document_stream(
  dom::parser &_parser,
  const uint8_t *_buf,
  size_t _len,
  size_t _batch_size
) noexcept
  : parser{&_parser},
    buf{_buf},
    len{_len},
    batch_size{_batch_size},
    error{SUCCESS}
{
#ifdef SIMDJSON_THREADS_ENABLED
  if(worker.get() == nullptr) {
    error = MEMALLOC;
  }
#endif
}

really_inline document_stream::document_stream() noexcept
  : parser{nullptr},
    buf{nullptr},
    len{0},
    batch_size{0},
    error{UNINITIALIZED} {
}

really_inline document_stream::~document_stream() noexcept {
}

really_inline document_stream::iterator document_stream::begin() noexcept {
  start();
  // If there are no documents, we're finished.
  return iterator(*this, error == EMPTY);
}

really_inline document_stream::iterator document_stream::end() noexcept {
  return iterator(*this, true);
}

really_inline document_stream::iterator::iterator(document_stream& _stream, bool is_end) noexcept
  : stream{_stream}, finished{is_end} {
}

really_inline simdjson_result<element> document_stream::iterator::operator*() noexcept {
  // Once we have yielded any errors, we're finished.
  if (stream.error) { finished = true; return stream.error; }
  return stream.parser->doc.root();
}

really_inline document_stream::iterator& document_stream::iterator::operator++() noexcept {
  stream.next();
  // If that was the last document, we're finished.
  if (stream.error == EMPTY) { finished = true; }
  return *this;
}

really_inline bool document_stream::iterator::operator!=(const document_stream::iterator &other) const noexcept {
  return finished != other.finished;
}

inline void document_stream::start() noexcept {
  if (error) { return; }

  error = parser->ensure_capacity(batch_size);
  if (error) { return; }

  // Always run the first stage 1 parse immediately
  batch_start = 0;
  error = run_stage1(*parser, batch_start);
  if (error) { return; }

#ifdef SIMDJSON_THREADS_ENABLED
  if (next_batch_start() < len) {
    // Kick off the first thread if needed
    error = stage1_thread_parser.ensure_capacity(batch_size);
    if (error) { return; }
    worker->start_thread();
    start_stage1_thread();
    if (error) { return; }
  }
#endif // SIMDJSON_THREADS_ENABLED

  next();
}

really_inline size_t document_stream::iterator::current_index() noexcept {
  return stream.doc_index;
}
inline void document_stream::next() noexcept {
  if (error) { return; }

  // Load the next document from the batch
  doc_index = batch_start + parser->implementation->structural_indexes[parser->implementation->next_structural_index];
  error = parser->implementation->stage2_next(parser->doc);
  // If that was the last document in the batch, load another batch (if available)
  while (error == EMPTY) {
    batch_start = next_batch_start();
    if (batch_start >= len) { break; }

#ifdef SIMDJSON_THREADS_ENABLED
    load_from_stage1_thread();
#else
    error = run_stage1(*parser, batch_start);
#endif
    if (error) { continue; } // If the error was EMPTY, we may want to load another batch.
    // Run stage 2 on the first document in the batch
    doc_index = batch_start + parser->implementation->structural_indexes[parser->implementation->next_structural_index];
    error = parser->implementation->stage2_next(parser->doc);
  }
}

inline size_t document_stream::next_batch_start() const noexcept {
  return batch_start + parser->implementation->structural_indexes[parser->implementation->n_structural_indexes];
}

inline error_code document_stream::run_stage1(dom::parser &p, size_t _batch_start) noexcept {
  // If this is the final batch, pass partial = false
  size_t remaining = len - _batch_start;
  if (remaining <= batch_size) {
    return p.implementation->stage1(&buf[_batch_start], remaining, false);
  } else {
    return p.implementation->stage1(&buf[_batch_start], batch_size, true);
  }
}

#ifdef SIMDJSON_THREADS_ENABLED

inline void document_stream::load_from_stage1_thread() noexcept {
  worker->finish();
  // Swap to the parser that was loaded up in the thread. Make sure the parser has
  // enough memory to swap to, as well.
  std::swap(*parser, stage1_thread_parser);
  error = stage1_thread_error;
  if (error) { return; }

  // If there's anything left, start the stage 1 thread!
  if (next_batch_start() < len) {
    start_stage1_thread();
  }
}

inline void document_stream::start_stage1_thread() noexcept {
  // we call the thread on a lambda that will update
  // this->stage1_thread_error
  // there is only one thread that may write to this value
  // TODO this is NOT exception-safe.
  this->stage1_thread_error = UNINITIALIZED; // In case something goes wrong, make sure it's an error
  size_t _next_batch_start = this->next_batch_start();

  worker->run(this, & this->stage1_thread_parser, _next_batch_start);
}

#endif // SIMDJSON_THREADS_ENABLED

} // namespace dom

really_inline simdjson_result<dom::document_stream>::simdjson_result() noexcept
  : simdjson_result_base() {
}
really_inline simdjson_result<dom::document_stream>::simdjson_result(error_code error) noexcept
  : simdjson_result_base(error) {
}
really_inline simdjson_result<dom::document_stream>::simdjson_result(dom::document_stream &&value) noexcept
  : simdjson_result_base(std::forward<dom::document_stream>(value)) {
}

#if SIMDJSON_EXCEPTIONS
really_inline dom::document_stream::iterator simdjson_result<dom::document_stream>::begin() noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first.begin();
}
really_inline dom::document_stream::iterator simdjson_result<dom::document_stream>::end() noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first.end();
}
#else // SIMDJSON_EXCEPTIONS
really_inline dom::document_stream::iterator simdjson_result<dom::document_stream>::begin() noexcept {
  first.error = error();
  return first.begin();
}
really_inline dom::document_stream::iterator simdjson_result<dom::document_stream>::end() noexcept {
  first.error = error();
  return first.end();
}
#endif // SIMDJSON_EXCEPTIONS

} // namespace simdjson
#endif // SIMDJSON_INLINE_DOCUMENT_STREAM_H
/* end file include/simdjson/inline/document_stream.h */
/* begin file include/simdjson/inline/document.h */
#ifndef SIMDJSON_INLINE_DOCUMENT_H
#define SIMDJSON_INLINE_DOCUMENT_H

// Inline implementations go in here.

#include <ostream>
#include <cstring>

namespace simdjson {
namespace dom {

//
// document inline implementation
//
inline element document::root() const noexcept {
  return element(internal::tape_ref(this, 1));
}

WARN_UNUSED
inline error_code document::allocate(size_t capacity) noexcept {
  if (capacity == 0) {
    string_buf.reset();
    tape.reset();
    return SUCCESS;
  }

  // a pathological input like "[[[[..." would generate len tape elements, so
  // need a capacity of at least len + 1, but it is also possible to do
  // worse with "[7,7,7,7,6,7,7,7,6,7,7,6,[7,7,7,7,6,7,7,7,6,7,7,6,7,7,7,7,7,7,6"
  //where len + 1 tape elements are
  // generated, see issue https://github.com/lemire/simdjson/issues/345
  size_t tape_capacity = ROUNDUP_N(capacity + 3, 64);
  // a document with only zero-length strings... could have len/3 string
  // and we would need len/3 * 5 bytes on the string buffer
  size_t string_capacity = ROUNDUP_N(5 * capacity / 3 + 32, 64);
  string_buf.reset( new (std::nothrow) uint8_t[string_capacity]);
  tape.reset(new (std::nothrow) uint64_t[tape_capacity]);
  return string_buf && tape ? SUCCESS : MEMALLOC;
}

inline bool document::dump_raw_tape(std::ostream &os) const noexcept {
  uint32_t string_length;
  size_t tape_idx = 0;
  uint64_t tape_val = tape[tape_idx];
  uint8_t type = uint8_t(tape_val >> 56);
  os << tape_idx << " : " << type;
  tape_idx++;
  size_t how_many = 0;
  if (type == 'r') {
    how_many = size_t(tape_val & internal::JSON_VALUE_MASK);
  } else {
    // Error: no starting root node?
    return false;
  }
  os << "\t// pointing to " << how_many << " (right after last node)\n";
  uint64_t payload;
  for (; tape_idx < how_many; tape_idx++) {
    os << tape_idx << " : ";
    tape_val = tape[tape_idx];
    payload = tape_val & internal::JSON_VALUE_MASK;
    type = uint8_t(tape_val >> 56);
    switch (type) {
    case '"': // we have a string
      os << "string \"";
      memcpy(&string_length, string_buf.get() + payload, sizeof(uint32_t));
      os << internal::escape_json_string(std::string_view(
        (const char *)(string_buf.get() + payload + sizeof(uint32_t)),
        string_length
      ));
      os << '"';
      os << '\n';
      break;
    case 'l': // we have a long int
      if (tape_idx + 1 >= how_many) {
        return false;
      }
      os << "integer " << static_cast<int64_t>(tape[++tape_idx]) << "\n";
      break;
    case 'u': // we have a long uint
      if (tape_idx + 1 >= how_many) {
        return false;
      }
      os << "unsigned integer " << tape[++tape_idx] << "\n";
      break;
    case 'd': // we have a double
      os << "float ";
      if (tape_idx + 1 >= how_many) {
        return false;
      }
      double answer;
      memcpy(&answer, &tape[++tape_idx], sizeof(answer));
      os << answer << '\n';
      break;
    case 'n': // we have a null
      os << "null\n";
      break;
    case 't': // we have a true
      os << "true\n";
      break;
    case 'f': // we have a false
      os << "false\n";
      break;
    case '{': // we have an object
      os << "{\t// pointing to next tape location " << payload
         << " (first node after the scope) \n";
      break;
    case '}': // we end an object
      os << "}\t// pointing to previous tape location " << payload
         << " (start of the scope) \n";
      break;
    case '[': // we start an array
      os << "[\t// pointing to next tape location " << payload
         << " (first node after the scope) \n";
      break;
    case ']': // we end an array
      os << "]\t// pointing to previous tape location " << payload
         << " (start of the scope) \n";
      break;
    case 'r': // we start and end with the root node
      // should we be hitting the root node?
      return false;
    default:
      return false;
    }
  }
  tape_val = tape[tape_idx];
  payload = tape_val & internal::JSON_VALUE_MASK;
  type = uint8_t(tape_val >> 56);
  os << tape_idx << " : " << type << "\t// pointing to " << payload
     << " (start root)\n";
  return true;
}

} // namespace dom
} // namespace simdjson

#endif // SIMDJSON_INLINE_DOCUMENT_H
/* end file include/simdjson/inline/document.h */
/* begin file include/simdjson/inline/element.h */
#ifndef SIMDJSON_INLINE_ELEMENT_H
#define SIMDJSON_INLINE_ELEMENT_H

#include <cstring>
#include <utility>

namespace simdjson {

//
// simdjson_result<dom::element> inline implementation
//
really_inline simdjson_result<dom::element>::simdjson_result() noexcept
    : internal::simdjson_result_base<dom::element>() {}
really_inline simdjson_result<dom::element>::simdjson_result(dom::element &&value) noexcept
    : internal::simdjson_result_base<dom::element>(std::forward<dom::element>(value)) {}
really_inline simdjson_result<dom::element>::simdjson_result(error_code error) noexcept
    : internal::simdjson_result_base<dom::element>(error) {}
inline simdjson_result<dom::element_type> simdjson_result<dom::element>::type() const noexcept {
  if (error()) { return error(); }
  return first.type();
}

template<typename T>
really_inline bool simdjson_result<dom::element>::is() const noexcept {
  return !error() && first.is<T>();
}
template<typename T>
really_inline simdjson_result<T> simdjson_result<dom::element>::get() const noexcept {
  if (error()) { return error(); }
  return first.get<T>();
}
template<typename T>
WARN_UNUSED really_inline error_code simdjson_result<dom::element>::get(T &value) const noexcept {
  if (error()) { return error(); }
  return first.get<T>(value);
}

really_inline simdjson_result<dom::array> simdjson_result<dom::element>::get_array() const noexcept {
  if (error()) { return error(); }
  return first.get_array();
}
really_inline simdjson_result<dom::object> simdjson_result<dom::element>::get_object() const noexcept {
  if (error()) { return error(); }
  return first.get_object();
}
really_inline simdjson_result<const char *> simdjson_result<dom::element>::get_c_str() const noexcept {
  if (error()) { return error(); }
  return first.get_c_str();
}
really_inline simdjson_result<size_t> simdjson_result<dom::element>::get_string_length() const noexcept {
  if (error()) { return error(); }
  return first.get_string_length();
}
really_inline simdjson_result<std::string_view> simdjson_result<dom::element>::get_string() const noexcept {
  if (error()) { return error(); }
  return first.get_string();
}
really_inline simdjson_result<int64_t> simdjson_result<dom::element>::get_int64() const noexcept {
  if (error()) { return error(); }
  return first.get_int64();
}
really_inline simdjson_result<uint64_t> simdjson_result<dom::element>::get_uint64() const noexcept {
  if (error()) { return error(); }
  return first.get_uint64();
}
really_inline simdjson_result<double> simdjson_result<dom::element>::get_double() const noexcept {
  if (error()) { return error(); }
  return first.get_double();
}
really_inline simdjson_result<bool> simdjson_result<dom::element>::get_bool() const noexcept {
  if (error()) { return error(); }
  return first.get_bool();
}

really_inline bool simdjson_result<dom::element>::is_array() const noexcept {
  return !error() && first.is_array();
}
really_inline bool simdjson_result<dom::element>::is_object() const noexcept {
  return !error() && first.is_object();
}
really_inline bool simdjson_result<dom::element>::is_string() const noexcept {
  return !error() && first.is_string();
}
really_inline bool simdjson_result<dom::element>::is_int64() const noexcept {
  return !error() && first.is_int64();
}
really_inline bool simdjson_result<dom::element>::is_uint64() const noexcept {
  return !error() && first.is_uint64();
}
really_inline bool simdjson_result<dom::element>::is_double() const noexcept {
  return !error() && first.is_double();
}
really_inline bool simdjson_result<dom::element>::is_bool() const noexcept {
  return !error() && first.is_bool();
}

really_inline bool simdjson_result<dom::element>::is_null() const noexcept {
  return !error() && first.is_null();
}

really_inline simdjson_result<dom::element> simdjson_result<dom::element>::operator[](const std::string_view &key) const noexcept {
  if (error()) { return error(); }
  return first[key];
}
really_inline simdjson_result<dom::element> simdjson_result<dom::element>::operator[](const char *key) const noexcept {
  if (error()) { return error(); }
  return first[key];
}
really_inline simdjson_result<dom::element> simdjson_result<dom::element>::at(const std::string_view &json_pointer) const noexcept {
  if (error()) { return error(); }
  return first.at(json_pointer);
}
really_inline simdjson_result<dom::element> simdjson_result<dom::element>::at(size_t index) const noexcept {
  if (error()) { return error(); }
  return first.at(index);
}
really_inline simdjson_result<dom::element> simdjson_result<dom::element>::at_key(const std::string_view &key) const noexcept {
  if (error()) { return error(); }
  return first.at_key(key);
}
really_inline simdjson_result<dom::element> simdjson_result<dom::element>::at_key_case_insensitive(const std::string_view &key) const noexcept {
  if (error()) { return error(); }
  return first.at_key_case_insensitive(key);
}

#if SIMDJSON_EXCEPTIONS

really_inline simdjson_result<dom::element>::operator bool() const noexcept(false) {
  return get<bool>();
}
really_inline simdjson_result<dom::element>::operator const char *() const noexcept(false) {
  return get<const char *>();
}
really_inline simdjson_result<dom::element>::operator std::string_view() const noexcept(false) {
  return get<std::string_view>();
}
really_inline simdjson_result<dom::element>::operator uint64_t() const noexcept(false) {
  return get<uint64_t>();
}
really_inline simdjson_result<dom::element>::operator int64_t() const noexcept(false) {
  return get<int64_t>();
}
really_inline simdjson_result<dom::element>::operator double() const noexcept(false) {
  return get<double>();
}
really_inline simdjson_result<dom::element>::operator dom::array() const noexcept(false) {
  return get<dom::array>();
}
really_inline simdjson_result<dom::element>::operator dom::object() const noexcept(false) {
  return get<dom::object>();
}

really_inline dom::array::iterator simdjson_result<dom::element>::begin() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first.begin();
}
really_inline dom::array::iterator simdjson_result<dom::element>::end() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first.end();
}

#endif // SIMDJSON_EXCEPTIONS

namespace dom {

//
// element inline implementation
//
really_inline element::element() noexcept : tape{} {}
really_inline element::element(const internal::tape_ref &_tape) noexcept : tape{_tape} { }

inline element_type element::type() const noexcept {
  auto tape_type = tape.tape_ref_type();
  return tape_type == internal::tape_type::FALSE_VALUE ? element_type::BOOL : static_cast<element_type>(tape_type);
}

inline simdjson_result<bool> element::get_bool() const noexcept {
  if(tape.is_true()) {
    return true;
  } else if(tape.is_false()) {
    return false;
  }
  return INCORRECT_TYPE;
}
inline simdjson_result<const char *> element::get_c_str() const noexcept {
  switch (tape.tape_ref_type()) {
    case internal::tape_type::STRING: {
      return tape.get_c_str();
    }
    default:
      return INCORRECT_TYPE;
  }
}
inline simdjson_result<size_t> element::get_string_length() const noexcept {
  switch (tape.tape_ref_type()) {
    case internal::tape_type::STRING: {
      return tape.get_string_length();
    }
    default:
      return INCORRECT_TYPE;
  }
}
inline simdjson_result<std::string_view> element::get_string() const noexcept {
  switch (tape.tape_ref_type()) {
    case internal::tape_type::STRING:
      return tape.get_string_view();
    default:
      return INCORRECT_TYPE;
  }
}
inline simdjson_result<uint64_t> element::get_uint64() const noexcept {
  if(unlikely(!tape.is_uint64())) { // branch rarely taken
    if(tape.is_int64()) {
      int64_t result = tape.next_tape_value<int64_t>();
      if (result < 0) {
        return NUMBER_OUT_OF_RANGE;
      }
      return uint64_t(result);
    }
    return INCORRECT_TYPE;
  }
  return tape.next_tape_value<int64_t>();
}
inline simdjson_result<int64_t> element::get_int64() const noexcept {
  if(unlikely(!tape.is_int64())) { // branch rarely taken
    if(tape.is_uint64()) {
      uint64_t result = tape.next_tape_value<uint64_t>();
      // Wrapping max in parens to handle Windows issue: https://stackoverflow.com/questions/11544073/how-do-i-deal-with-the-max-macro-in-windows-h-colliding-with-max-in-std
      if (result > uint64_t((std::numeric_limits<int64_t>::max)())) {
        return NUMBER_OUT_OF_RANGE;
      }
      return static_cast<int64_t>(result);
    }
    return INCORRECT_TYPE;
  }
  return tape.next_tape_value<int64_t>();
}
inline simdjson_result<double> element::get_double() const noexcept {
  // Performance considerations:
  // 1. Querying tape_ref_type() implies doing a shift, it is fast to just do a straight
  //   comparison.
  // 2. Using a switch-case relies on the compiler guessing what kind of code generation
  //    we want... But the compiler cannot know that we expect the type to be "double"
  //    most of the time.
  // We can expect get<double> to refer to a double type almost all the time.
  // It is important to craft the code accordingly so that the compiler can use this
  // information. (This could also be solved with profile-guided optimization.)
  if(unlikely(!tape.is_double())) { // branch rarely taken
    if(tape.is_uint64()) {
      return double(tape.next_tape_value<uint64_t>());
    } else if(tape.is_int64()) {
      return double(tape.next_tape_value<int64_t>());
    }
    return INCORRECT_TYPE;
  }
  // this is common:
  return tape.next_tape_value<double>();
}
inline simdjson_result<array> element::get_array() const noexcept {
  switch (tape.tape_ref_type()) {
    case internal::tape_type::START_ARRAY:
      return array(tape);
    default:
      return INCORRECT_TYPE;
  }
}
inline simdjson_result<object> element::get_object() const noexcept {
  switch (tape.tape_ref_type()) {
    case internal::tape_type::START_OBJECT:
      return object(tape);
    default:
      return INCORRECT_TYPE;
  }
}

template<typename T>
WARN_UNUSED really_inline error_code element::get(T &value) const noexcept {
  return get<T>().get(value);
}
// An element-specific version prevents recursion with simdjson_result::get<element>(value)
template<>
WARN_UNUSED really_inline error_code element::get<element>(element &value) const noexcept {
  value = element(tape);
  return SUCCESS;
}

template<typename T>
really_inline bool element::is() const noexcept {
  auto result = get<T>();
  return !result.error();
}

template<> inline simdjson_result<array> element::get<array>() const noexcept { return get_array(); }
template<> inline simdjson_result<object> element::get<object>() const noexcept { return get_object(); }
template<> inline simdjson_result<const char *> element::get<const char *>() const noexcept { return get_c_str(); }
template<> inline simdjson_result<std::string_view> element::get<std::string_view>() const noexcept { return get_string(); }
template<> inline simdjson_result<int64_t> element::get<int64_t>() const noexcept { return get_int64(); }
template<> inline simdjson_result<uint64_t> element::get<uint64_t>() const noexcept { return get_uint64(); }
template<> inline simdjson_result<double> element::get<double>() const noexcept { return get_double(); }
template<> inline simdjson_result<bool> element::get<bool>() const noexcept { return get_bool(); }

inline bool element::is_array() const noexcept { return is<array>(); }
inline bool element::is_object() const noexcept { return is<object>(); }
inline bool element::is_string() const noexcept { return is<std::string_view>(); }
inline bool element::is_int64() const noexcept { return is<int64_t>(); }
inline bool element::is_uint64() const noexcept { return is<uint64_t>(); }
inline bool element::is_double() const noexcept { return is<double>(); }
inline bool element::is_bool() const noexcept { return is<bool>(); }

inline bool element::is_null() const noexcept {
  return tape.is_null_on_tape();
}

#if SIMDJSON_EXCEPTIONS

inline element::operator bool() const noexcept(false) { return get<bool>(); }
inline element::operator const char*() const noexcept(false) { return get<const char *>(); }
inline element::operator std::string_view() const noexcept(false) { return get<std::string_view>(); }
inline element::operator uint64_t() const noexcept(false) { return get<uint64_t>(); }
inline element::operator int64_t() const noexcept(false) { return get<int64_t>(); }
inline element::operator double() const noexcept(false) { return get<double>(); }
inline element::operator array() const noexcept(false) { return get<array>(); }
inline element::operator object() const noexcept(false) { return get<object>(); }

inline array::iterator element::begin() const noexcept(false) {
  return get<array>().begin();
}
inline array::iterator element::end() const noexcept(false) {
  return get<array>().end();
}

#endif // SIMDJSON_EXCEPTIONS

inline simdjson_result<element> element::operator[](const std::string_view &key) const noexcept {
  return at_key(key);
}
inline simdjson_result<element> element::operator[](const char *key) const noexcept {
  return at_key(key);
}
inline simdjson_result<element> element::at(const std::string_view &json_pointer) const noexcept {
  switch (tape.tape_ref_type()) {
    case internal::tape_type::START_OBJECT:
      return object(tape).at(json_pointer);
    case internal::tape_type::START_ARRAY:
      return array(tape).at(json_pointer);
    default:
      return INCORRECT_TYPE;
  }
}
inline simdjson_result<element> element::at(size_t index) const noexcept {
  return get<array>().at(index);
}
inline simdjson_result<element> element::at_key(const std::string_view &key) const noexcept {
  return get<object>().at_key(key);
}
inline simdjson_result<element> element::at_key_case_insensitive(const std::string_view &key) const noexcept {
  return get<object>().at_key_case_insensitive(key);
}

inline bool element::dump_raw_tape(std::ostream &out) const noexcept {
  return tape.doc->dump_raw_tape(out);
}

inline std::ostream& operator<<(std::ostream& out, const element &value) {
  return out << minify<element>(value);
}

inline std::ostream& operator<<(std::ostream& out, element_type type) {
  switch (type) {
    case element_type::ARRAY:
      return out << "array";
    case element_type::OBJECT:
      return out << "object";
    case element_type::INT64:
      return out << "int64_t";
    case element_type::UINT64:
      return out << "uint64_t";
    case element_type::DOUBLE:
      return out << "double";
    case element_type::STRING:
      return out << "string";
    case element_type::BOOL:
      return out << "bool";
    case element_type::NULL_VALUE:
      return out << "null";
    default:
      return out << "unexpected content!!!"; // abort() usage is forbidden in the library
  }
}

} // namespace dom

template<>
inline std::ostream& minifier<dom::element>::print(std::ostream& out) {
  using tape_type=internal::tape_type;
  size_t depth = 0;
  constexpr size_t MAX_DEPTH = 16;
  bool is_object[MAX_DEPTH];
  is_object[0] = false;
  bool after_value = false;

  internal::tape_ref iter(value.tape);
  do {
    // print commas after each value
    if (after_value) {
      out << ",";
    }
    // If we are in an object, print the next key and :, and skip to the next value.
    if (is_object[depth]) {
      out << '"' << internal::escape_json_string(iter.get_string_view()) << "\":";
      iter.json_index++;
    }
    switch (iter.tape_ref_type()) {

    // Arrays
    case tape_type::START_ARRAY: {
      // If we're too deep, we need to recurse to go deeper.
      depth++;
      if (unlikely(depth >= MAX_DEPTH)) {
        out << minify<dom::array>(dom::array(iter));
        iter.json_index = iter.matching_brace_index() - 1; // Jump to the ]
        depth--;
        break;
      }

      // Output start [
      out << '[';
      iter.json_index++;

      // Handle empty [] (we don't want to come back around and print commas)
      if (iter.tape_ref_type() == tape_type::END_ARRAY) {
        out << ']';
        depth--;
        break;
      }

      is_object[depth] = false;
      after_value = false;
      continue;
    }

    // Objects
    case tape_type::START_OBJECT: {
      // If we're too deep, we need to recurse to go deeper.
      depth++;
      if (unlikely(depth >= MAX_DEPTH)) {
        out << minify<dom::object>(dom::object(iter));
        iter.json_index = iter.matching_brace_index() - 1; // Jump to the }
        depth--;
        break;
      }

      // Output start {
      out << '{';
      iter.json_index++;

      // Handle empty {} (we don't want to come back around and print commas)
      if (iter.tape_ref_type() == tape_type::END_OBJECT) {
        out << '}';
        depth--;
        break;
      }

      is_object[depth] = true;
      after_value = false;
      continue;
    }

    // Scalars
    case tape_type::STRING:
      out << '"' << internal::escape_json_string(iter.get_string_view()) << '"';
      break;
    case tape_type::INT64:
      out << iter.next_tape_value<int64_t>();
      iter.json_index++; // numbers take up 2 spots, so we need to increment extra
      break;
    case tape_type::UINT64:
      out << iter.next_tape_value<uint64_t>();
      iter.json_index++; // numbers take up 2 spots, so we need to increment extra
      break;
    case tape_type::DOUBLE:
      out << iter.next_tape_value<double>();
      iter.json_index++; // numbers take up 2 spots, so we need to increment extra
      break;
    case tape_type::TRUE_VALUE:
      out << "true";
      break;
    case tape_type::FALSE_VALUE:
      out << "false";
      break;
    case tape_type::NULL_VALUE:
      out << "null";
      break;

    // These are impossible
    case tape_type::END_ARRAY:
    case tape_type::END_OBJECT:
    case tape_type::ROOT:
      out << "unexpected content!!!"; // abort() usage is forbidden in the library
    }
    iter.json_index++;
    after_value = true;

    // Handle multiple ends in a row
    while (depth != 0 && (iter.tape_ref_type() == tape_type::END_ARRAY || iter.tape_ref_type() == tape_type::END_OBJECT)) {
      out << char(iter.tape_ref_type());
      depth--;
      iter.json_index++;
    }

    // Stop when we're at depth 0
  } while (depth != 0);

  return out;
}

#if SIMDJSON_EXCEPTIONS

template<>
really_inline std::ostream& minifier<simdjson_result<dom::element>>::print(std::ostream& out) {
  if (value.error()) { throw simdjson_error(value.error()); }
  return out << minify<dom::element>(value.first);
}

really_inline std::ostream& operator<<(std::ostream& out, const simdjson_result<dom::element> &value) noexcept(false) {
  return out << minify<simdjson_result<dom::element>>(value);
}
#endif

} // namespace simdjson

#endif // SIMDJSON_INLINE_ELEMENT_H
/* end file include/simdjson/inline/element.h */
/* begin file include/simdjson/inline/error.h */
#ifndef SIMDJSON_INLINE_ERROR_H
#define SIMDJSON_INLINE_ERROR_H

#include <cstring>
#include <string>
#include <utility>

namespace simdjson {
namespace internal {
  // We store the error code so we can validate the error message is associated with the right code
  struct error_code_info {
    error_code code;
    std::string message;
  };
  // These MUST match the codes in error_code. We check this constraint in basictests.
  extern SIMDJSON_DLLIMPORTEXPORT const error_code_info error_codes[];
} // namespace internal


inline const char *error_message(error_code error) noexcept {
  // If you're using error_code, we're trusting you got it from the enum.
  return internal::error_codes[int(error)].message.c_str();
}

inline const std::string &error_message(int error) noexcept {
  if (error < 0 || error >= error_code::NUM_ERROR_CODES) {
    return internal::error_codes[UNEXPECTED_ERROR].message;
  }
  return internal::error_codes[error].message;
}

inline std::ostream& operator<<(std::ostream& out, error_code error) noexcept {
  return out << error_message(error);
}

namespace internal {

//
// internal::simdjson_result_base<T> inline implementation
//

template<typename T>
really_inline void simdjson_result_base<T>::tie(T &value, error_code &error) && noexcept {
  // on the clang compiler that comes with current macOS (Apple clang version 11.0.0),
  // tie(width, error) = size["w"].get<uint64_t>();
  // fails with "error: no viable overloaded '='""
  error = this->second;
  if (!error) {
    value = std::forward<simdjson_result_base<T>>(*this).first;
  }
}

template<typename T>
WARN_UNUSED really_inline error_code simdjson_result_base<T>::get(T &value) && noexcept {
  error_code error;
  std::forward<simdjson_result_base<T>>(*this).tie(value, error);
  return error;
}

template<typename T>
really_inline error_code simdjson_result_base<T>::error() const noexcept {
  return this->second;
}

#if SIMDJSON_EXCEPTIONS

template<typename T>
really_inline T& simdjson_result_base<T>::value() noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return this->first;
}

template<typename T>
really_inline T&& simdjson_result_base<T>::take_value() && noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return std::forward<T>(this->first);
}

template<typename T>
really_inline simdjson_result_base<T>::operator T&&() && noexcept(false) {
  return std::forward<simdjson_result_base<T>>(*this).take_value();
}

#endif // SIMDJSON_EXCEPTIONS

template<typename T>
really_inline simdjson_result_base<T>::simdjson_result_base(T &&value, error_code error) noexcept
    : std::pair<T, error_code>(std::forward<T>(value), error) {}
template<typename T>
really_inline simdjson_result_base<T>::simdjson_result_base(error_code error) noexcept
    : simdjson_result_base(T{}, error) {}
template<typename T>
really_inline simdjson_result_base<T>::simdjson_result_base(T &&value) noexcept
    : simdjson_result_base(std::forward<T>(value), SUCCESS) {}
template<typename T>
really_inline simdjson_result_base<T>::simdjson_result_base() noexcept
    : simdjson_result_base(T{}, UNINITIALIZED) {}

} // namespace internal

///
/// simdjson_result<T> inline implementation
///

template<typename T>
really_inline void simdjson_result<T>::tie(T &value, error_code &error) && noexcept {
  std::forward<internal::simdjson_result_base<T>>(*this).tie(value, error);
}

template<typename T>
WARN_UNUSED really_inline error_code simdjson_result<T>::get(T &value) && noexcept {
  return std::forward<internal::simdjson_result_base<T>>(*this).get(value);
}

template<typename T>
really_inline error_code simdjson_result<T>::error() const noexcept {
  return internal::simdjson_result_base<T>::error();
}

#if SIMDJSON_EXCEPTIONS

template<typename T>
really_inline T& simdjson_result<T>::value() noexcept(false) {
  return internal::simdjson_result_base<T>::value();
}

template<typename T>
really_inline T&& simdjson_result<T>::take_value() && noexcept(false) {
  return std::forward<internal::simdjson_result_base<T>>(*this).take_value();
}

template<typename T>
really_inline simdjson_result<T>::operator T&&() && noexcept(false) {
  return std::forward<internal::simdjson_result_base<T>>(*this).take_value();
}

#endif // SIMDJSON_EXCEPTIONS

template<typename T>
really_inline simdjson_result<T>::simdjson_result(T &&value, error_code error) noexcept
    : internal::simdjson_result_base<T>(std::forward<T>(value), error) {}
template<typename T>
really_inline simdjson_result<T>::simdjson_result(error_code error) noexcept
    : internal::simdjson_result_base<T>(error) {}
template<typename T>
really_inline simdjson_result<T>::simdjson_result(T &&value) noexcept
    : internal::simdjson_result_base<T>(std::forward<T>(value)) {}
template<typename T>
really_inline simdjson_result<T>::simdjson_result() noexcept
    : internal::simdjson_result_base<T>() {}

} // namespace simdjson

#endif // SIMDJSON_INLINE_ERROR_H
/* end file include/simdjson/inline/error.h */
/* begin file include/simdjson/inline/object.h */
#ifndef SIMDJSON_INLINE_OBJECT_H
#define SIMDJSON_INLINE_OBJECT_H

#include <cstring>
#include <string>

namespace simdjson {

//
// simdjson_result<dom::object> inline implementation
//
really_inline simdjson_result<dom::object>::simdjson_result() noexcept
    : internal::simdjson_result_base<dom::object>() {}
really_inline simdjson_result<dom::object>::simdjson_result(dom::object value) noexcept
    : internal::simdjson_result_base<dom::object>(std::forward<dom::object>(value)) {}
really_inline simdjson_result<dom::object>::simdjson_result(error_code error) noexcept
    : internal::simdjson_result_base<dom::object>(error) {}

inline simdjson_result<dom::element> simdjson_result<dom::object>::operator[](const std::string_view &key) const noexcept {
  if (error()) { return error(); }
  return first[key];
}
inline simdjson_result<dom::element> simdjson_result<dom::object>::operator[](const char *key) const noexcept {
  if (error()) { return error(); }
  return first[key];
}
inline simdjson_result<dom::element> simdjson_result<dom::object>::at(const std::string_view &json_pointer) const noexcept {
  if (error()) { return error(); }
  return first.at(json_pointer);
}
inline simdjson_result<dom::element> simdjson_result<dom::object>::at_key(const std::string_view &key) const noexcept {
  if (error()) { return error(); }
  return first.at_key(key);
}
inline simdjson_result<dom::element> simdjson_result<dom::object>::at_key_case_insensitive(const std::string_view &key) const noexcept {
  if (error()) { return error(); }
  return first.at_key_case_insensitive(key);
}

#if SIMDJSON_EXCEPTIONS

inline dom::object::iterator simdjson_result<dom::object>::begin() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first.begin();
}
inline dom::object::iterator simdjson_result<dom::object>::end() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first.end();
}
inline size_t simdjson_result<dom::object>::size() const noexcept(false) {
  if (error()) { throw simdjson_error(error()); }
  return first.size();
}

#endif // SIMDJSON_EXCEPTIONS

namespace dom {

//
// object inline implementation
//
really_inline object::object() noexcept : tape{} {}
really_inline object::object(const internal::tape_ref &_tape) noexcept : tape{_tape} { }
inline object::iterator object::begin() const noexcept {
  return internal::tape_ref(tape.doc, tape.json_index + 1);
}
inline object::iterator object::end() const noexcept {
  return internal::tape_ref(tape.doc, tape.after_element() - 1);
}
inline size_t object::size() const noexcept {
  return tape.scope_count();
}

inline simdjson_result<element> object::operator[](const std::string_view &key) const noexcept {
  return at_key(key);
}
inline simdjson_result<element> object::operator[](const char *key) const noexcept {
  return at_key(key);
}
inline simdjson_result<element> object::at(const std::string_view &json_pointer) const noexcept {
  size_t slash = json_pointer.find('/');
  std::string_view key = json_pointer.substr(0, slash);

  // Grab the child with the given key
  simdjson_result<element> child;

  // If there is an escape character in the key, unescape it and then get the child.
  size_t escape = key.find('~');
  if (escape != std::string_view::npos) {
    // Unescape the key
    std::string unescaped(key);
    do {
      switch (unescaped[escape+1]) {
        case '0':
          unescaped.replace(escape, 2, "~");
          break;
        case '1':
          unescaped.replace(escape, 2, "/");
          break;
        default:
          return INVALID_JSON_POINTER; // "Unexpected ~ escape character in JSON pointer");
      }
      escape = unescaped.find('~', escape+1);
    } while (escape != std::string::npos);
    child = at_key(unescaped);
  } else {
    child = at_key(key);
  }

  // If there is a /, we have to recurse and look up more of the path
  if (slash != std::string_view::npos) {
    child = child.at(json_pointer.substr(slash+1));
  }

  return child;
}
inline simdjson_result<element> object::at_key(const std::string_view &key) const noexcept {
  iterator end_field = end();
  for (iterator field = begin(); field != end_field; ++field) {
    if (field.key_equals(key)) {
      return field.value();
    }
  }
  return NO_SUCH_FIELD;
}
// In case you wonder why we need this, please see
// https://github.com/simdjson/simdjson/issues/323
// People do seek keys in a case-insensitive manner.
inline simdjson_result<element> object::at_key_case_insensitive(const std::string_view &key) const noexcept {
  iterator end_field = end();
  for (iterator field = begin(); field != end_field; ++field) {
    if (field.key_equals_case_insensitive(key)) {
      return field.value();
    }
  }
  return NO_SUCH_FIELD;
}

//
// object::iterator inline implementation
//
really_inline object::iterator::iterator(const internal::tape_ref &_tape) noexcept : tape{_tape} { }
inline const key_value_pair object::iterator::operator*() const noexcept {
  return key_value_pair(key(), value());
}
inline bool object::iterator::operator!=(const object::iterator& other) const noexcept {
  return tape.json_index != other.tape.json_index;
}
inline object::iterator& object::iterator::operator++() noexcept {
  tape.json_index++;
  tape.json_index = tape.after_element();
  return *this;
}
inline std::string_view object::iterator::key() const noexcept {
  return tape.get_string_view();
}
inline uint32_t object::iterator::key_length() const noexcept {
  return tape.get_string_length();
}
inline const char* object::iterator::key_c_str() const noexcept {
  return reinterpret_cast<const char *>(&tape.doc->string_buf[size_t(tape.tape_value()) + sizeof(uint32_t)]);
}
inline element object::iterator::value() const noexcept {
  return element(internal::tape_ref(tape.doc, tape.json_index + 1));
}

/**
 * Design notes:
 * Instead of constructing a string_view and then comparing it with a
 * user-provided strings, it is probably more performant to have dedicated
 * functions taking as a parameter the string we want to compare against
 * and return true when they are equal. That avoids the creation of a temporary
 * std::string_view. Though it is possible for the compiler to avoid entirely
 * any overhead due to string_view, relying too much on compiler magic is
 * problematic: compiler magic sometimes fail, and then what do you do?
 * Also, enticing users to rely on high-performance function is probably better
 * on the long run.
 */

inline bool object::iterator::key_equals(const std::string_view & o) const noexcept {
  // We use the fact that the key length can be computed quickly
  // without access to the string buffer.
  const uint32_t len = key_length();
  if(o.size() == len) {
    // We avoid construction of a temporary string_view instance.
    return (memcmp(o.data(), key_c_str(), len) == 0);
  }
  return false;
}

inline bool object::iterator::key_equals_case_insensitive(const std::string_view & o) const noexcept {
  // We use the fact that the key length can be computed quickly
  // without access to the string buffer.
  const uint32_t len = key_length();
  if(o.size() == len) {
      // See For case-insensitive string comparisons, avoid char-by-char functions
      // https://lemire.me/blog/2020/04/30/for-case-insensitive-string-comparisons-avoid-char-by-char-functions/
      // Note that it might be worth rolling our own strncasecmp function, with vectorization.
      return (simdjson_strncasecmp(o.data(), key_c_str(), len) == 0);
  }
  return false;
}
//
// key_value_pair inline implementation
//
inline key_value_pair::key_value_pair(const std::string_view &_key, element _value) noexcept :
  key(_key), value(_value) {}

inline std::ostream& operator<<(std::ostream& out, const object &value) {
  return out << minify<object>(value);
}
inline std::ostream& operator<<(std::ostream& out, const key_value_pair &value) {
  return out << minify<key_value_pair>(value);
}

} // namespace dom

template<>
inline std::ostream& minifier<dom::object>::print(std::ostream& out) {
  out << '{';
  auto pair = value.begin();
  auto end = value.end();
  if (pair != end) {
    out << minify<dom::key_value_pair>(*pair);
    for (++pair; pair != end; ++pair) {
      out << "," << minify<dom::key_value_pair>(*pair);
    }
  }
  return out << '}';
}

template<>
inline std::ostream& minifier<dom::key_value_pair>::print(std::ostream& out) {
  return out << '"' << internal::escape_json_string(value.key) << "\":" << value.value;
}

#if SIMDJSON_EXCEPTIONS

template<>
inline std::ostream& minifier<simdjson_result<dom::object>>::print(std::ostream& out) {
  if (value.error()) { throw simdjson_error(value.error()); }
  return out << minify<dom::object>(value.first);
}

inline std::ostream& operator<<(std::ostream& out, const simdjson_result<dom::object> &value) noexcept(false) {
  return out << minify<simdjson_result<dom::object>>(value);
}
#endif // SIMDJSON_EXCEPTIONS

} // namespace simdjson

#endif // SIMDJSON_INLINE_OBJECT_H
/* end file include/simdjson/inline/object.h */
/* begin file include/simdjson/inline/padded_string.h */
#ifndef SIMDJSON_INLINE_PADDED_STRING_H
#define SIMDJSON_INLINE_PADDED_STRING_H


#include <climits>
#include <cstring>
#include <memory>
#include <string>

namespace simdjson {
namespace internal {

// low-level function to allocate memory with padding so we can read past the
// "length" bytes safely. if you must provide a pointer to some data, create it
// with this function: length is the max. size in bytes of the string caller is
// responsible to free the memory (free(...))
inline char *allocate_padded_buffer(size_t length) noexcept {
  // we could do a simple malloc
  // return (char *) malloc(length + SIMDJSON_PADDING);
  // However, we might as well align to cache lines...
  size_t totalpaddedlength = length + SIMDJSON_PADDING;
#if defined(_MSC_VER) && _MSC_VER < 1910
  // For legacy Visual Studio 2015 since it does not have proper C++11 support
  char *padded_buffer = new[totalpaddedlength];
#else
  char *padded_buffer = aligned_malloc_char(64, totalpaddedlength);
#endif
#ifndef NDEBUG
  if (padded_buffer == nullptr) {
    return nullptr;
  }
#endif // NDEBUG
  memset(padded_buffer + length, 0, totalpaddedlength - length);
  return padded_buffer;
} // allocate_padded_buffer()

} // namespace internal


inline padded_string::padded_string() noexcept {}
inline padded_string::padded_string(size_t length) noexcept
    : viable_size(length), data_ptr(internal::allocate_padded_buffer(length)) {
  if (data_ptr != nullptr)
    data_ptr[length] = '\0'; // easier when you need a c_str
}
inline padded_string::padded_string(const char *data, size_t length) noexcept
    : viable_size(length), data_ptr(internal::allocate_padded_buffer(length)) {
  if ((data != nullptr) and (data_ptr != nullptr)) {
    memcpy(data_ptr, data, length);
    data_ptr[length] = '\0'; // easier when you need a c_str
  }
}
// note: do not pass std::string arguments by value
inline padded_string::padded_string(const std::string & str_ ) noexcept
    : viable_size(str_.size()), data_ptr(internal::allocate_padded_buffer(str_.size())) {
  if (data_ptr != nullptr) {
    memcpy(data_ptr, str_.data(), str_.size());
    data_ptr[str_.size()] = '\0'; // easier when you need a c_str
  }
}
// note: do pass std::string_view arguments by value
inline padded_string::padded_string(std::string_view sv_) noexcept
    : viable_size(sv_.size()), data_ptr(internal::allocate_padded_buffer(sv_.size())) {
  if (data_ptr != nullptr) {
    memcpy(data_ptr, sv_.data(), sv_.size());
    data_ptr[sv_.size()] = '\0'; // easier when you need a c_str
  }
}
inline padded_string::padded_string(padded_string &&o) noexcept
    : viable_size(o.viable_size), data_ptr(o.data_ptr) {
  o.data_ptr = nullptr; // we take ownership
}

inline padded_string &padded_string::operator=(padded_string &&o) noexcept {
  aligned_free_char(data_ptr);
  data_ptr = o.data_ptr;
  viable_size = o.viable_size;
  o.data_ptr = nullptr; // we take ownership
  o.viable_size = 0;
  return *this;
}

inline void padded_string::swap(padded_string &o) noexcept {
  size_t tmp_viable_size = viable_size;
  char *tmp_data_ptr = data_ptr;
  viable_size = o.viable_size;
  data_ptr = o.data_ptr;
  o.data_ptr = tmp_data_ptr;
  o.viable_size = tmp_viable_size;
}

inline padded_string::~padded_string() noexcept {
  aligned_free_char(data_ptr);
}

inline size_t padded_string::size() const noexcept { return viable_size; }

inline size_t padded_string::length() const noexcept { return viable_size; }

inline const char *padded_string::data() const noexcept { return data_ptr; }

inline char *padded_string::data() noexcept { return data_ptr; }

inline padded_string::operator std::string_view() const { return std::string_view(data(), length()); }

inline simdjson_result<padded_string> padded_string::load(const std::string &filename) noexcept {
  // Open the file
  SIMDJSON_PUSH_DISABLE_WARNINGS
  SIMDJSON_DISABLE_DEPRECATED_WARNING // Disable CRT_SECURE warning on MSVC: manually verified this is safe
  std::FILE *fp = std::fopen(filename.c_str(), "rb");
  SIMDJSON_POP_DISABLE_WARNINGS

  if (fp == nullptr) {
    return IO_ERROR;
  }

  // Get the file size
  if(std::fseek(fp, 0, SEEK_END) < 0) {
    std::fclose(fp);
    return IO_ERROR;
  }
  long llen = std::ftell(fp);
  if((llen < 0) || (llen == LONG_MAX)) {
    std::fclose(fp);
    return IO_ERROR;
  }

  // Allocate the padded_string
  size_t len = (size_t) llen;
  padded_string s(len);
  if (s.data() == nullptr) {
    std::fclose(fp);
    return MEMALLOC;
  }

  // Read the padded_string
  std::rewind(fp);
  size_t bytes_read = std::fread(s.data(), 1, len, fp);
  if (std::fclose(fp) != 0 || bytes_read != len) {
    return IO_ERROR;
  }

  return s;
}

} // namespace simdjson

#endif // SIMDJSON_INLINE_PADDED_STRING_H
/* end file include/simdjson/inline/padded_string.h */
/* begin file include/simdjson/inline/parsedjson_iterator.h */
#ifndef SIMDJSON_INLINE_PARSEDJSON_ITERATOR_H
#define SIMDJSON_INLINE_PARSEDJSON_ITERATOR_H

#include <cstring>

namespace simdjson {

// VS2017 reports deprecated warnings when you define a deprecated class's methods.
SIMDJSON_PUSH_DISABLE_WARNINGS
SIMDJSON_DISABLE_DEPRECATED_WARNING

// Because of template weirdness, the actual class definition is inline in the document class

WARN_UNUSED bool dom::parser::Iterator::is_ok() const {
  return location < tape_length;
}

// useful for debugging purposes
size_t dom::parser::Iterator::get_tape_location() const {
  return location;
}

// useful for debugging purposes
size_t dom::parser::Iterator::get_tape_length() const {
  return tape_length;
}

// returns the current depth (start at 1 with 0 reserved for the fictitious root
// node)
size_t dom::parser::Iterator::get_depth() const {
  return depth;
}

// A scope is a series of nodes at the same depth, typically it is either an
// object ({) or an array ([). The root node has type 'r'.
uint8_t dom::parser::Iterator::get_scope_type() const {
  return depth_index[depth].scope_type;
}

bool dom::parser::Iterator::move_forward() {
  if (location + 1 >= tape_length) {
    return false; // we are at the end!
  }

  if ((current_type == '[') || (current_type == '{')) {
    // We are entering a new scope
    depth++;
    assert(depth < max_depth);
    depth_index[depth].start_of_scope = location;
    depth_index[depth].scope_type = current_type;
  } else if ((current_type == ']') || (current_type == '}')) {
    // Leaving a scope.
    depth--;
  } else if (is_number()) {
    // these types use 2 locations on the tape, not just one.
    location += 1;
  }

  location += 1;
  current_val = doc.tape[location];
  current_type = uint8_t(current_val >> 56);
  return true;
}

void dom::parser::Iterator::move_to_value() {
  // assume that we are on a key, so move by 1.
  location += 1;
  current_val = doc.tape[location];
  current_type = uint8_t(current_val >> 56);
}

bool dom::parser::Iterator::move_to_key(const char *key) {
    if (down()) {
      do {
        const bool right_key = (strcmp(get_string(), key) == 0);
        move_to_value();
        if (right_key) {
          return true;
        }
      } while (next());
      up();
    }
    return false;
}

bool dom::parser::Iterator::move_to_key_insensitive(
    const char *key) {
    if (down()) {
      do {
        const bool right_key = (simdjson_strcasecmp(get_string(), key) == 0);
        move_to_value();
        if (right_key) {
          return true;
        }
      } while (next());
      up();
    }
    return false;
}

bool dom::parser::Iterator::move_to_key(const char *key,
                                                       uint32_t length) {
  if (down()) {
    do {
      bool right_key = ((get_string_length() == length) &&
                        (memcmp(get_string(), key, length) == 0));
      move_to_value();
      if (right_key) {
        return true;
      }
    } while (next());
    up();
  }
  return false;
}

bool dom::parser::Iterator::move_to_index(uint32_t index) {
  if (down()) {
    uint32_t i = 0;
    for (; i < index; i++) {
      if (!next()) {
        break;
      }
    }
    if (i == index) {
      return true;
    }
    up();
  }
  return false;
}

bool dom::parser::Iterator::prev() {
  size_t target_location = location;
  to_start_scope();
  size_t npos = location;
  if (target_location == npos) {
    return false; // we were already at the start
  }
  size_t oldnpos;
  // we have that npos < target_location here
  do {
    oldnpos = npos;
    if ((current_type == '[') || (current_type == '{')) {
      // we need to jump
      npos = uint32_t(current_val);
    } else {
      npos = npos + ((current_type == 'd' || current_type == 'l') ? 2 : 1);
    }
  } while (npos < target_location);
  location = oldnpos;
  current_val = doc.tape[location];
  current_type = uint8_t(current_val >> 56);
  return true;
}

bool dom::parser::Iterator::up() {
  if (depth == 1) {
    return false; // don't allow moving back to root
  }
  to_start_scope();
  // next we just move to the previous value
  depth--;
  location -= 1;
  current_val = doc.tape[location];
  current_type = uint8_t(current_val >> 56);
  return true;
}

bool dom::parser::Iterator::down() {
  if (location + 1 >= tape_length) {
    return false;
  }
  if ((current_type == '[') || (current_type == '{')) {
    size_t npos = uint32_t(current_val);
    if (npos == location + 2) {
      return false; // we have an empty scope
    }
    depth++;
    assert(depth < max_depth);
    location = location + 1;
    depth_index[depth].start_of_scope = location;
    depth_index[depth].scope_type = current_type;
    current_val = doc.tape[location];
    current_type = uint8_t(current_val >> 56);
    return true;
  }
  return false;
}

void dom::parser::Iterator::to_start_scope() {
  location = depth_index[depth].start_of_scope;
  current_val = doc.tape[location];
  current_type = uint8_t(current_val >> 56);
}

bool dom::parser::Iterator::next() {
  size_t npos;
  if ((current_type == '[') || (current_type == '{')) {
    // we need to jump
    npos = uint32_t(current_val);
  } else {
    npos = location + (is_number() ? 2 : 1);
  }
  uint64_t next_val = doc.tape[npos];
  uint8_t next_type = uint8_t(next_val >> 56);
  if ((next_type == ']') || (next_type == '}')) {
    return false; // we reached the end of the scope
  }
  location = npos;
  current_val = next_val;
  current_type = next_type;
  return true;
}
dom::parser::Iterator::Iterator(const dom::parser &pj) noexcept(false)
    : doc(pj.doc)
{
#if SIMDJSON_EXCEPTIONS
  if (!pj.valid) { throw simdjson_error(pj.error); }
#else
  if (!pj.valid) { return; } //  abort() usage is forbidden in the library
#endif

  max_depth = pj.max_depth();
  depth_index = new scopeindex_t[max_depth + 1];
  depth_index[0].start_of_scope = location;
  current_val = doc.tape[location++];
  current_type = uint8_t(current_val >> 56);
  depth_index[0].scope_type = current_type;
  tape_length = size_t(current_val & internal::JSON_VALUE_MASK);
  if (location < tape_length) {
    // If we make it here, then depth_capacity must >=2, but the compiler
    // may not know this.
    current_val = doc.tape[location];
    current_type = uint8_t(current_val >> 56);
    depth++;
    assert(depth < max_depth);
    depth_index[depth].start_of_scope = location;
    depth_index[depth].scope_type = current_type;
  }
}
dom::parser::Iterator::Iterator(
    const dom::parser::Iterator &o) noexcept
    : doc(o.doc),
    max_depth(o.depth),
    depth(o.depth),
    location(o.location),
    tape_length(o.tape_length),
    current_type(o.current_type),
    current_val(o.current_val)
{
  depth_index = new scopeindex_t[max_depth+1];
  memcpy(depth_index, o.depth_index, (depth + 1) * sizeof(depth_index[0]));
}

dom::parser::Iterator::~Iterator() noexcept {
  if (depth_index) { delete[] depth_index; }
}

bool dom::parser::Iterator::print(std::ostream &os, bool escape_strings) const {
  if (!is_ok()) {
    return false;
  }
  switch (current_type) {
  case '"': // we have a string
    os << '"';
    if (escape_strings) {
      os << internal::escape_json_string(std::string_view(get_string(), get_string_length()));
    } else {
      // was: os << get_string();, but given that we can include null chars, we
      // have to do something crazier:
      std::copy(get_string(), get_string() + get_string_length(), std::ostream_iterator<char>(os));
    }
    os << '"';
    break;
  case 'l': // we have a long int
    os << get_integer();
    break;
  case 'u':
    os << get_unsigned_integer();
    break;
  case 'd':
    os << get_double();
    break;
  case 'n': // we have a null
    os << "null";
    break;
  case 't': // we have a true
    os << "true";
    break;
  case 'f': // we have a false
    os << "false";
    break;
  case '{': // we have an object
  case '}': // we end an object
  case '[': // we start an array
  case ']': // we end an array
    os << char(current_type);
    break;
  default:
    return false;
  }
  return true;
}

bool dom::parser::Iterator::move_to(const char *pointer,
                                                   uint32_t length) {
  char *new_pointer = nullptr;
  if (pointer[0] == '#') {
    // Converting fragment representation to string representation
    new_pointer = new char[length];
    uint32_t new_length = 0;
    for (uint32_t i = 1; i < length; i++) {
      if (pointer[i] == '%' && pointer[i + 1] == 'x') {
#if __cpp_exceptions
        try {
#endif
          int fragment =
              std::stoi(std::string(&pointer[i + 2], 2), nullptr, 16);
          if (fragment == '\\' || fragment == '"' || (fragment <= 0x1F)) {
            // escaping the character
            new_pointer[new_length] = '\\';
            new_length++;
          }
          new_pointer[new_length] = char(fragment);
          i += 3;
#if __cpp_exceptions
        } catch (std::invalid_argument &) {
          delete[] new_pointer;
          return false; // the fragment is invalid
        }
#endif
      } else {
        new_pointer[new_length] = pointer[i];
      }
      new_length++;
    }
    length = new_length;
    pointer = new_pointer;
  }

  // saving the current state
  size_t depth_s = depth;
  size_t location_s = location;
  uint8_t current_type_s = current_type;
  uint64_t current_val_s = current_val;

  rewind(); // The json pointer is used from the root of the document.

  bool found = relative_move_to(pointer, length);
  delete[] new_pointer;

  if (!found) {
    // since the pointer has found nothing, we get back to the original
    // position.
    depth = depth_s;
    location = location_s;
    current_type = current_type_s;
    current_val = current_val_s;
  }

  return found;
}

bool dom::parser::Iterator::relative_move_to(const char *pointer,
                                                            uint32_t length) {
  if (length == 0) {
    // returns the whole document
    return true;
  }

  if (pointer[0] != '/') {
    // '/' must be the first character
    return false;
  }

  // finding the key in an object or the index in an array
  std::string key_or_index;
  uint32_t offset = 1;

  // checking for the "-" case
  if (is_array() && pointer[1] == '-') {
    if (length != 2) {
      // the pointer must be exactly "/-"
      // there can't be anything more after '-' as an index
      return false;
    }
    key_or_index = '-';
    offset = length; // will skip the loop coming right after
  }

  // We either transform the first reference token to a valid json key
  // or we make sure it is a valid index in an array.
  for (; offset < length; offset++) {
    if (pointer[offset] == '/') {
      // beginning of the next key or index
      break;
    }
    if (is_array() && (pointer[offset] < '0' || pointer[offset] > '9')) {
      // the index of an array must be an integer
      // we also make sure std::stoi won't discard whitespaces later
      return false;
    }
    if (pointer[offset] == '~') {
      // "~1" represents "/"
      if (pointer[offset + 1] == '1') {
        key_or_index += '/';
        offset++;
        continue;
      }
      // "~0" represents "~"
      if (pointer[offset + 1] == '0') {
        key_or_index += '~';
        offset++;
        continue;
      }
    }
    if (pointer[offset] == '\\') {
      if (pointer[offset + 1] == '\\' || pointer[offset + 1] == '"' ||
          (pointer[offset + 1] <= 0x1F)) {
        key_or_index += pointer[offset + 1];
        offset++;
        continue;
      }
      return false; // invalid escaped character
    }
    if (pointer[offset] == '\"') {
      // unescaped quote character. this is an invalid case.
      // lets do nothing and assume most pointers will be valid.
      // it won't find any corresponding json key anyway.
      // return false;
    }
    key_or_index += pointer[offset];
  }

  bool found = false;
  if (is_object()) {
    if (move_to_key(key_or_index.c_str(), uint32_t(key_or_index.length()))) {
      found = relative_move_to(pointer + offset, length - offset);
    }
  } else if (is_array()) {
    if (key_or_index == "-") { // handling "-" case first
      if (down()) {
        while (next())
          ; // moving to the end of the array
        // moving to the nonexistent value right after...
        size_t npos;
        if ((current_type == '[') || (current_type == '{')) {
          // we need to jump
          npos = uint32_t(current_val);
        } else {
          npos =
              location + ((current_type == 'd' || current_type == 'l') ? 2 : 1);
        }
        location = npos;
        current_val = doc.tape[npos];
        current_type = uint8_t(current_val >> 56);
        return true; // how could it fail ?
      }
    } else { // regular numeric index
      // The index can't have a leading '0'
      if (key_or_index[0] == '0' && key_or_index.length() > 1) {
        return false;
      }
      // it cannot be empty
      if (key_or_index.length() == 0) {
        return false;
      }
      // we already checked the index contains only valid digits
      uint32_t index = std::stoi(key_or_index);
      if (move_to_index(index)) {
        found = relative_move_to(pointer + offset, length - offset);
      }
    }
  }

  return found;
}

SIMDJSON_POP_DISABLE_WARNINGS

} // namespace simdjson

#endif // SIMDJSON_INLINE_PARSEDJSON_ITERATOR_H
/* end file include/simdjson/inline/parsedjson_iterator.h */
/* begin file include/simdjson/inline/parser.h */
#ifndef SIMDJSON_INLINE_PARSER_H
#define SIMDJSON_INLINE_PARSER_H

#include <cstdio>
#include <climits>

namespace simdjson {
namespace dom {

//
// parser inline implementation
//
#if defined(_MSC_VER) && _MSC_VER < 1910
// older versions of Visual Studio lack proper support for unique_ptr.
really_inline parser::parser(size_t max_capacity) noexcept
  : _max_capacity{max_capacity},
    loaded_bytes(nullptr) {
}
#else
really_inline parser::parser(size_t max_capacity) noexcept
  : _max_capacity{max_capacity},
    loaded_bytes(nullptr, &aligned_free_char) {
}
#endif
really_inline parser::parser(parser &&other) noexcept = default;
really_inline parser &parser::operator=(parser &&other) noexcept = default;

inline bool parser::is_valid() const noexcept { return valid; }
inline int parser::get_error_code() const noexcept { return error; }
inline std::string parser::get_error_message() const noexcept { return error_message(error); }
inline bool parser::print_json(std::ostream &os) const noexcept {
  if (!valid) { return false; }
  os << doc.root();
  return true;
}
inline bool parser::dump_raw_tape(std::ostream &os) const noexcept {
  return valid ? doc.dump_raw_tape(os) : false;
}

inline simdjson_result<size_t> parser::read_file(const std::string &path) noexcept {
  // Open the file
  SIMDJSON_PUSH_DISABLE_WARNINGS
  SIMDJSON_DISABLE_DEPRECATED_WARNING // Disable CRT_SECURE warning on MSVC: manually verified this is safe
  std::FILE *fp = std::fopen(path.c_str(), "rb");
  SIMDJSON_POP_DISABLE_WARNINGS

  if (fp == nullptr) {
    return IO_ERROR;
  }

  // Get the file size
  if(std::fseek(fp, 0, SEEK_END) < 0) {
    std::fclose(fp);
    return IO_ERROR;
  }
  long len = std::ftell(fp);
  if((len < 0) || (len == LONG_MAX)) {
    std::fclose(fp);
    return IO_ERROR;
  }

  // Make sure we have enough capacity to load the file
  if (_loaded_bytes_capacity < size_t(len)) {
    loaded_bytes.reset( internal::allocate_padded_buffer(len) );
    if (!loaded_bytes) {
      std::fclose(fp);
      return MEMALLOC;
    }
    _loaded_bytes_capacity = len;
  }

  // Read the string
  std::rewind(fp);
  size_t bytes_read = std::fread(loaded_bytes.get(), 1, len, fp);
  if (std::fclose(fp) != 0 || bytes_read != size_t(len)) {
    return IO_ERROR;
  }

  return bytes_read;
}

inline simdjson_result<element> parser::load(const std::string &path) & noexcept {
  size_t len;
  auto _error = read_file(path).get(len);
  if (_error) { return _error; }
  return parse(loaded_bytes.get(), len, false);
}

inline simdjson_result<document_stream> parser::load_many(const std::string &path, size_t batch_size) noexcept {
  size_t len;
  auto _error = read_file(path).get(len);
  if (_error) { return _error; }
  return document_stream(*this, (const uint8_t*)loaded_bytes.get(), len, batch_size);
}

inline simdjson_result<element> parser::parse(const uint8_t *buf, size_t len, bool realloc_if_needed) & noexcept {
  error_code _error = ensure_capacity(len);
  if (_error) { return _error; }

  if (realloc_if_needed) {
    const uint8_t *tmp_buf = buf;
    buf = (uint8_t *)internal::allocate_padded_buffer(len);
    if (buf == nullptr)
      return MEMALLOC;
    memcpy((void *)buf, tmp_buf, len);
  }

  _error = implementation->parse(buf, len, doc);
  if (realloc_if_needed) {
    aligned_free((void *)buf); // must free before we exit
  }
  if (_error) { return _error; }

  return doc.root();
}
really_inline simdjson_result<element> parser::parse(const char *buf, size_t len, bool realloc_if_needed) & noexcept {
  return parse((const uint8_t *)buf, len, realloc_if_needed);
}
really_inline simdjson_result<element> parser::parse(const std::string &s) & noexcept {
  return parse(s.data(), s.length(), s.capacity() - s.length() < SIMDJSON_PADDING);
}
really_inline simdjson_result<element> parser::parse(const padded_string &s) & noexcept {
  return parse(s.data(), s.length(), false);
}

inline simdjson_result<document_stream> parser::parse_many(const uint8_t *buf, size_t len, size_t batch_size) noexcept {
  return document_stream(*this, buf, len, batch_size);
}
inline simdjson_result<document_stream> parser::parse_many(const char *buf, size_t len, size_t batch_size) noexcept {
  return parse_many((const uint8_t *)buf, len, batch_size);
}
inline simdjson_result<document_stream> parser::parse_many(const std::string &s, size_t batch_size) noexcept {
  return parse_many(s.data(), s.length(), batch_size);
}
inline simdjson_result<document_stream> parser::parse_many(const padded_string &s, size_t batch_size) noexcept {
  return parse_many(s.data(), s.length(), batch_size);
}

really_inline size_t parser::capacity() const noexcept {
  return implementation ? implementation->capacity() : 0;
}
really_inline size_t parser::max_capacity() const noexcept {
  return _max_capacity;
}
really_inline size_t parser::max_depth() const noexcept {
  return implementation ? implementation->max_depth() : DEFAULT_MAX_DEPTH;
}

WARN_UNUSED
inline error_code parser::allocate(size_t capacity, size_t max_depth) noexcept {
  //
  // Reallocate implementation and document if needed
  //
  error_code err;
  //
  // It is possible that we change max_depth without touching capacity, in
  // which case, we do not want to reallocate the document buffers.
  //
  bool need_doc_allocation{false};
  if (implementation) {
    need_doc_allocation = implementation->capacity() != capacity || !doc.tape;
    err = implementation->allocate(capacity, max_depth);
  } else {
    need_doc_allocation = true;
    err = simdjson::active_implementation->create_dom_parser_implementation(capacity, max_depth, implementation);
  }
  if (err) { return err; }
  if (need_doc_allocation) {
    err = doc.allocate(capacity);
    if (err) { return err; }
  }
  return SUCCESS;
}

WARN_UNUSED
inline bool parser::allocate_capacity(size_t capacity, size_t max_depth) noexcept {
  return !allocate(capacity, max_depth);
}

inline error_code parser::ensure_capacity(size_t desired_capacity) noexcept {
  // If we don't have enough capacity, (try to) automatically bump it.
  // If the document was taken, reallocate that too.
  // Both in one if statement to minimize unlikely branching.
  if (unlikely(capacity() < desired_capacity || !doc.tape)) {
    if (desired_capacity > max_capacity()) {
      return error = CAPACITY;
    }
    return allocate(desired_capacity, max_depth());
  }

  return SUCCESS;
}

really_inline void parser::set_max_capacity(size_t max_capacity) noexcept {
  _max_capacity = max_capacity;
}

} // namespace dom
} // namespace simdjson

#endif // SIMDJSON_INLINE_PARSER_H
/* end file include/simdjson/inline/parser.h */
/* begin file include/simdjson/inline/tape_ref.h */
#ifndef SIMDJSON_INLINE_TAPE_REF_H
#define SIMDJSON_INLINE_TAPE_REF_H

#include <cstring>

namespace simdjson {
namespace internal {

//
// tape_ref inline implementation
//
really_inline tape_ref::tape_ref() noexcept : doc{nullptr}, json_index{0} {}
really_inline tape_ref::tape_ref(const dom::document *_doc, size_t _json_index) noexcept : doc{_doc}, json_index{_json_index} {}

// Some value types have a specific on-tape word value. It can be faster
// to check the type by doing a word-to-word comparison instead of extracting the
// most significant 8 bits.

really_inline bool tape_ref::is_double() const noexcept {
  constexpr uint64_t tape_double = uint64_t(tape_type::DOUBLE)<<56;
  return doc->tape[json_index] == tape_double;
}
really_inline bool tape_ref::is_int64() const noexcept {
  constexpr uint64_t tape_int64 = uint64_t(tape_type::INT64)<<56;
  return doc->tape[json_index] == tape_int64;
}
really_inline bool tape_ref::is_uint64() const noexcept {
  constexpr uint64_t tape_uint64 = uint64_t(tape_type::UINT64)<<56;
  return doc->tape[json_index] == tape_uint64;
}
really_inline bool tape_ref::is_false() const noexcept {
  constexpr uint64_t tape_false = uint64_t(tape_type::FALSE_VALUE)<<56;
  return doc->tape[json_index] == tape_false;
}
really_inline bool tape_ref::is_true() const noexcept {
  constexpr uint64_t tape_true = uint64_t(tape_type::TRUE_VALUE)<<56;
  return doc->tape[json_index] == tape_true;
}
really_inline bool tape_ref::is_null_on_tape() const noexcept {
  constexpr uint64_t tape_null = uint64_t(tape_type::NULL_VALUE)<<56;
  return doc->tape[json_index] == tape_null;
}

inline size_t tape_ref::after_element() const noexcept {
  switch (tape_ref_type()) {
    case tape_type::START_ARRAY:
    case tape_type::START_OBJECT:
      return matching_brace_index();
    case tape_type::UINT64:
    case tape_type::INT64:
    case tape_type::DOUBLE:
      return json_index + 2;
    default:
      return json_index + 1;
  }
}
really_inline tape_type tape_ref::tape_ref_type() const noexcept {
  return static_cast<tape_type>(doc->tape[json_index] >> 56);
}
really_inline uint64_t internal::tape_ref::tape_value() const noexcept {
  return doc->tape[json_index] & internal::JSON_VALUE_MASK;
}
really_inline uint32_t internal::tape_ref::matching_brace_index() const noexcept {
  return uint32_t(doc->tape[json_index]);
}
really_inline uint32_t internal::tape_ref::scope_count() const noexcept {
  return uint32_t((doc->tape[json_index] >> 32) & internal::JSON_COUNT_MASK);
}

template<typename T>
really_inline T tape_ref::next_tape_value() const noexcept {
  static_assert(sizeof(T) == sizeof(uint64_t), "next_tape_value() template parameter must be 64-bit");
  // Though the following is tempting...
  //  return *reinterpret_cast<const T*>(&doc->tape[json_index + 1]);
  // It is not generally safe. It is safer, and often faster to rely
  // on memcpy. Yes, it is uglier, but it is also encapsulated.
  T x;
  memcpy(&x,&doc->tape[json_index + 1],sizeof(uint64_t));
  return x;
}

really_inline uint32_t internal::tape_ref::get_string_length() const noexcept {
  size_t string_buf_index = size_t(tape_value());
  uint32_t len;
  memcpy(&len, &doc->string_buf[string_buf_index], sizeof(len));
  return len;
}

really_inline const char * internal::tape_ref::get_c_str() const noexcept {
  size_t string_buf_index = size_t(tape_value());
  return reinterpret_cast<const char *>(&doc->string_buf[string_buf_index + sizeof(uint32_t)]);
}

inline std::string_view internal::tape_ref::get_string_view() const noexcept {
  return std::string_view(
      get_c_str(),
      get_string_length()
  );
}

} // namespace internal
} // namespace simdjson

#endif // SIMDJSON_INLINE_TAPE_REF_H
/* end file include/simdjson/inline/tape_ref.h */

SIMDJSON_POP_DISABLE_WARNINGS

#endif // SIMDJSON_H
/* end file include/simdjson/inline/tape_ref.h */
