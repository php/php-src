/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Dmitry Stogov <zeev@zend.com>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_PORTABILITY_H
#define ZEND_PORTABILITY_H

#ifdef __cplusplus
#define BEGIN_EXTERN_C() extern "C" {
#define END_EXTERN_C() }
#else
#define BEGIN_EXTERN_C()
#define END_EXTERN_C()
#endif

/*
 * general definitions
 */

#ifdef ZEND_WIN32
# include "zend_config.w32.h"
# define ZEND_PATHS_SEPARATOR		';'
#elif defined(NETWARE)
# include <zend_config.h>
# define ZEND_PATHS_SEPARATOR		';'
#elif defined(__riscos__)
# include <zend_config.h>
# define ZEND_PATHS_SEPARATOR		';'
#else
# include <zend_config.h>
# define ZEND_PATHS_SEPARATOR		':'
#endif

#include "../TSRM/TSRM.h"

#include <stdio.h>
#include <assert.h>

#ifdef HAVE_UNIX_H
# include <unix.h>
#endif

#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif

#ifdef HAVE_DLFCN_H
# include <dlfcn.h>
#endif

#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif

#if HAVE_ALLOCA_H && !defined(_ALLOCA_H)
# include <alloca.h>
#endif

#if defined(ZEND_WIN32)
#include <intrin.h>
#endif

/* Only use this macro if you know for sure that all of the switches values
   are covered by its case statements */
#if ZEND_DEBUG
# define EMPTY_SWITCH_DEFAULT_CASE() default: ZEND_ASSERT(0); break;
#elif defined(ZEND_WIN32)
# define EMPTY_SWITCH_DEFAULT_CASE() default: __assume(0); break;
#else
# define EMPTY_SWITCH_DEFAULT_CASE()
#endif

/* all HAVE_XXX test have to be after the include of zend_config above */

#if defined(HAVE_LIBDL) && !defined(ZEND_WIN32)

# ifndef RTLD_LAZY
#  define RTLD_LAZY 1    /* Solaris 1, FreeBSD's (2.1.7.1 and older) */
# endif

# ifndef RTLD_GLOBAL
#  define RTLD_GLOBAL 0
# endif

# if defined(RTLD_GROUP) && defined(RTLD_WORLD) && defined(RTLD_PARENT)
#  define DL_LOAD(libname)			dlopen(libname, RTLD_LAZY | RTLD_GLOBAL | RTLD_GROUP | RTLD_WORLD | RTLD_PARENT)
# elif defined(RTLD_DEEPBIND)
#  define DL_LOAD(libname)			dlopen(libname, RTLD_LAZY | RTLD_GLOBAL | RTLD_DEEPBIND)
# else
#  define DL_LOAD(libname)			dlopen(libname, RTLD_LAZY | RTLD_GLOBAL)
# endif
# define DL_UNLOAD					dlclose
# if defined(DLSYM_NEEDS_UNDERSCORE)
#  define DL_FETCH_SYMBOL(h,s)		dlsym((h), "_" s)
# else
#  define DL_FETCH_SYMBOL			dlsym
# endif
# define DL_ERROR					dlerror
# define DL_HANDLE					void *
# define ZEND_EXTENSIONS_SUPPORT	1
#elif defined(ZEND_WIN32)
# define DL_LOAD(libname)			LoadLibrary(libname)
# define DL_FETCH_SYMBOL			GetProcAddress
# define DL_UNLOAD					FreeLibrary
# define DL_HANDLE					HMODULE
# define ZEND_EXTENSIONS_SUPPORT	1
#else
# define DL_HANDLE					void *
# define ZEND_EXTENSIONS_SUPPORT	0
#endif

/* AIX requires this to be the first thing in the file.  */
#ifndef __GNUC__
# ifndef HAVE_ALLOCA_H
#  ifdef _AIX
#   pragma alloca
#  else
#   ifndef alloca /* predefined by HP cc +Olibcalls */
char *alloca();
#   endif
#  endif
# endif
#endif

/* Compatibility with non-clang compilers */
#ifndef __has_attribute
# define __has_attribute(x) 0
#endif

/* GCC x.y.z supplies __GNUC__ = x and __GNUC_MINOR__ = y */
#ifdef __GNUC__
# define ZEND_GCC_VERSION (__GNUC__ * 1000 + __GNUC_MINOR__)
#else
# define ZEND_GCC_VERSION 0
#endif

#if ZEND_GCC_VERSION >= 2096
# define ZEND_ATTRIBUTE_MALLOC __attribute__ ((__malloc__))
#else
# define ZEND_ATTRIBUTE_MALLOC
#endif

#if ZEND_GCC_VERSION >= 4003 || __has_attribute(alloc_size)
# define ZEND_ATTRIBUTE_ALLOC_SIZE(X) __attribute__ ((alloc_size(X)))
# define ZEND_ATTRIBUTE_ALLOC_SIZE2(X,Y) __attribute__ ((alloc_size(X,Y)))
#else
# define ZEND_ATTRIBUTE_ALLOC_SIZE(X)
# define ZEND_ATTRIBUTE_ALLOC_SIZE2(X,Y)
#endif

/* Format string checks are disabled by default, because we use custom format modifiers (like %p),
 * which cause a large amount of false positives. You can enable format checks by adding
 * -DZEND_CHECK_FORMAT_STRINGS to CFLAGS. */

#if defined(ZEND_CHECK_FORMAT_STRINGS) && (ZEND_GCC_VERSION >= 2007 || __has_attribute(format))
# define ZEND_ATTRIBUTE_FORMAT(type, idx, first) __attribute__ ((format(type, idx, first)))
#else
# define ZEND_ATTRIBUTE_FORMAT(type, idx, first)
#endif

#if defined(ZEND_CHECK_FORMAT_STRINGS) && ((ZEND_GCC_VERSION >= 3001 && !defined(__INTEL_COMPILER)) || __has_attribute(format))
# define ZEND_ATTRIBUTE_PTR_FORMAT(type, idx, first) __attribute__ ((format(type, idx, first)))
#else
# define ZEND_ATTRIBUTE_PTR_FORMAT(type, idx, first)
#endif

#if ZEND_GCC_VERSION >= 3001 || __has_attribute(deprecated)
# define ZEND_ATTRIBUTE_DEPRECATED  __attribute__((deprecated))
#elif defined(ZEND_WIN32) && defined(_MSC_VER) && _MSC_VER >= 1300
# define ZEND_ATTRIBUTE_DEPRECATED  __declspec(deprecated)
#else
# define ZEND_ATTRIBUTE_DEPRECATED
#endif

#if defined(__GNUC__) && ZEND_GCC_VERSION >= 4003
# define ZEND_ATTRIBUTE_UNUSED __attribute__((unused))
# define ZEND_ATTRIBUTE_UNUSED_LABEL __attribute__((cold, unused));
#else
# define ZEND_ATTRIBUTE_UNUSED
# define ZEND_ATTRIBUTE_UNUSED_LABEL
#endif

#if defined(__GNUC__) && ZEND_GCC_VERSION >= 3004 && defined(__i386__)
# define ZEND_FASTCALL __attribute__((fastcall))
#elif defined(_MSC_VER) && defined(_M_IX86) && _MSC_VER <= 1700
# define ZEND_FASTCALL __fastcall
#elif defined(_MSC_VER) && _MSC_VER >= 1800
# define ZEND_FASTCALL __vectorcall
#else
# define ZEND_FASTCALL
#endif

#if defined(__GNUC__) && ZEND_GCC_VERSION >= 3004
#else
# define __restrict__
#endif
#define restrict __restrict__

#if (defined(__GNUC__) && __GNUC__ >= 3 && !defined(__INTEL_COMPILER) && !defined(DARWIN) && !defined(__hpux) && !defined(_AIX) && !defined(__osf__)) || __has_attribute(noreturn)
# define HAVE_NORETURN
# define ZEND_NORETURN __attribute__((noreturn))
#elif defined(ZEND_WIN32)
# define HAVE_NORETURN
# define ZEND_NORETURN __declspec(noreturn)
#else
# define ZEND_NORETURN
#endif

#if (defined(__GNUC__) && __GNUC__ >= 3 && !defined(__INTEL_COMPILER) && !defined(DARWIN) && !defined(__hpux) && !defined(_AIX) && !defined(__osf__))
# define HAVE_NORETURN_ALIAS
#endif

#if ZEND_DEBUG
# define zend_always_inline inline
# define zend_never_inline
#else
# if defined(__GNUC__)
#  if __GNUC__ >= 3
#   define zend_always_inline inline __attribute__((always_inline))
#   define zend_never_inline __attribute__((noinline))
#  else
#   define zend_always_inline inline
#   define zend_never_inline
#  endif
# elif defined(_MSC_VER)
#  define zend_always_inline __forceinline
#  define zend_never_inline
# else
#  if __has_attribute(always_inline)
#   define zend_always_inline inline __attribute__((always_inline))
#  else
#   define zend_always_inline inline
#  endif
#  if __has_attribute(noinline)
#   define zend_never_inline __attribute__((noinline))
#  else
#   define zend_never_inline
#  endif
# endif
#endif /* ZEND_DEBUG */

#if (defined (__GNUC__) && __GNUC__ > 2 ) && !defined(DARWIN) && !defined(__hpux) && !defined(_AIX)
# define EXPECTED(condition)   __builtin_expect(!(!(condition)), 1)
# define UNEXPECTED(condition) __builtin_expect(!(!(condition)), 0)
#else
# define EXPECTED(condition)   (condition)
# define UNEXPECTED(condition) (condition)
#endif

#ifndef XtOffsetOf
# if defined(CRAY) || (defined(__ARMCC_VERSION) && !defined(LINUX))
# ifdef __STDC__
# define XtOffset(p_type, field) _Offsetof(p_type, field)
# else
# ifdef CRAY2
# define XtOffset(p_type, field) \
    (sizeof(int)*((unsigned int)&(((p_type)NULL)->field)))

# else /* !CRAY2 */

# define XtOffset(p_type, field) ((unsigned int)&(((p_type)NULL)->field))

# endif /* !CRAY2 */
# endif /* __STDC__ */
# else /* ! (CRAY || __arm) */

# define XtOffset(p_type, field) \
    ((zend_long) (((char *) (&(((p_type)NULL)->field))) - ((char *) NULL)))

# endif /* !CRAY */

# ifdef offsetof
# define XtOffsetOf(s_type, field) offsetof(s_type, field)
# else
# define XtOffsetOf(s_type, field) XtOffset(s_type*, field)
# endif

#endif

#if (HAVE_ALLOCA || (defined (__GNUC__) && __GNUC__ >= 2)) && !(defined(ZTS) && defined(NETWARE)) && !(defined(ZTS) && defined(HPUX)) && !defined(DARWIN)
# define ZEND_ALLOCA_MAX_SIZE (32 * 1024)
# define ALLOCA_FLAG(name) \
	zend_bool name;
# define SET_ALLOCA_FLAG(name) \
	name = 1
# define do_alloca_ex(size, limit, use_heap) \
	((use_heap = (UNEXPECTED((size) > (limit)))) ? emalloc(size) : alloca(size))
# define do_alloca(size, use_heap) \
	do_alloca_ex(size, ZEND_ALLOCA_MAX_SIZE, use_heap)
# define free_alloca(p, use_heap) \
	do { if (UNEXPECTED(use_heap)) efree(p); } while (0)
#else
# define ALLOCA_FLAG(name)
# define SET_ALLOCA_FLAG(name)
# define do_alloca(p, use_heap)		emalloc(p)
# define free_alloca(p, use_heap)	efree(p)
#endif

#ifdef HAVE_SIGSETJMP
# define SETJMP(a) sigsetjmp(a, 0)
# define LONGJMP(a,b) siglongjmp(a, b)
# define JMP_BUF sigjmp_buf
#else
# define SETJMP(a) setjmp(a)
# define LONGJMP(a,b) longjmp(a, b)
# define JMP_BUF jmp_buf
#endif

#if ZEND_DEBUG
# define ZEND_FILE_LINE_D				const char *__zend_filename, const uint __zend_lineno
# define ZEND_FILE_LINE_DC				, ZEND_FILE_LINE_D
# define ZEND_FILE_LINE_ORIG_D			const char *__zend_orig_filename, const uint __zend_orig_lineno
# define ZEND_FILE_LINE_ORIG_DC			, ZEND_FILE_LINE_ORIG_D
# define ZEND_FILE_LINE_RELAY_C			__zend_filename, __zend_lineno
# define ZEND_FILE_LINE_RELAY_CC		, ZEND_FILE_LINE_RELAY_C
# define ZEND_FILE_LINE_C				__FILE__, __LINE__
# define ZEND_FILE_LINE_CC				, ZEND_FILE_LINE_C
# define ZEND_FILE_LINE_EMPTY_C			NULL, 0
# define ZEND_FILE_LINE_EMPTY_CC		, ZEND_FILE_LINE_EMPTY_C
# define ZEND_FILE_LINE_ORIG_RELAY_C	__zend_orig_filename, __zend_orig_lineno
# define ZEND_FILE_LINE_ORIG_RELAY_CC	, ZEND_FILE_LINE_ORIG_RELAY_C
# define ZEND_ASSERT(c)					assert(c)
#else
# define ZEND_FILE_LINE_D
# define ZEND_FILE_LINE_DC
# define ZEND_FILE_LINE_ORIG_D
# define ZEND_FILE_LINE_ORIG_DC
# define ZEND_FILE_LINE_RELAY_C
# define ZEND_FILE_LINE_RELAY_CC
# define ZEND_FILE_LINE_C
# define ZEND_FILE_LINE_CC
# define ZEND_FILE_LINE_EMPTY_C
# define ZEND_FILE_LINE_EMPTY_CC
# define ZEND_FILE_LINE_ORIG_RELAY_C
# define ZEND_FILE_LINE_ORIG_RELAY_CC
# define ZEND_ASSERT(c)
#endif	/* ZEND_DEBUG */

#if ZEND_DEBUG
# define Z_DBG(expr)		(expr)
#else
# define Z_DBG(expr)
#endif

#ifdef ZTS
# define ZTS_V 1
#else
# define ZTS_V 0
#endif

#ifndef LONG_MAX
# define LONG_MAX 2147483647L
#endif

#ifndef LONG_MIN
# define LONG_MIN (- LONG_MAX - 1)
#endif

#define MAX_LENGTH_OF_DOUBLE 32

#undef MIN
#undef MAX
#define MAX(a, b)  (((a)>(b))?(a):(b))
#define MIN(a, b)  (((a)<(b))?(a):(b))
#define ZEND_STRL(str)		(str), (sizeof(str)-1)
#define ZEND_STRS(str)		(str), (sizeof(str))
#define ZEND_NORMALIZE_BOOL(n)			\
	((n) ? (((n)>0) ? 1 : -1) : 0)
#define ZEND_TRUTH(x)		((x) ? 1 : 0)
#define ZEND_LOG_XOR(a, b)		(ZEND_TRUTH(a) ^ ZEND_TRUTH(b))

#define ZEND_MAX_RESERVED_RESOURCES	4

/* excpt.h on Digital Unix 4.0 defines function_table */
#undef function_table

#ifdef ZEND_WIN32
#define ZEND_SECURE_ZERO(var, size) RtlSecureZeroMemory((var), (size))
#else
#define ZEND_SECURE_ZERO(var, size) memset((var), 0, (size))
#endif

/* This check should only be used on network socket, not file descriptors */
#ifdef ZEND_WIN32
#define ZEND_VALID_SOCKET(sock) (INVALID_SOCKET != (sock))
#else
#define ZEND_VALID_SOCKET(sock) ((sock) >= 0)
#endif

#endif /* ZEND_PORTABILITY_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
