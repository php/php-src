/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef ZEND_CONFIG_W32_H
#define ZEND_CONFIG_W32_H

#define HAVE_ALLOCA 1
#define HAVE_LIMITS_H 1
#define _CRTDBG_MAP_ALLOC

#include <malloc.h>
#include <stdlib.h>
#include <crtdbg.h>

#include <string.h>
#include <windows.h>
#include <float.h>

typedef unsigned long ulong;
typedef unsigned int uint;

#undef HAVE_KILL
#define HAVE_GETPID 1
/* #define HAVE_ALLOCA_H 1 */
#define HAVE_MEMCPY 1
#define HAVE_STRDUP 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_STDIOSTR_H 1
#define HAVE_CLASS_ISTDIOSTREAM
#define istdiostream stdiostream
#define HAVE_STDARG_H	1
#define HAVE_SNPRINTF	1
#define HAVE_VSNPRINTF	1

#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define zend_isinf(a)	0
#define zend_finite(x)	_finite(x)
#define zend_isnan(x)	_isnan(x)

#define zend_sprintf sprintf

/* This will cause the compilation process to be MUCH longer, but will generate
 * a much quicker PHP binary
 */
#undef inline
#ifdef ZEND_WIN32_FORCE_INLINE
# define inline __forceinline
#else
# define inline
#endif

#define zend_finite(A) _finite(A)
#define zend_isnan(A) _isnan(A)

#ifdef LIBZEND_EXPORTS
#	define ZEND_API __declspec(dllexport)
#else
#	define ZEND_API __declspec(dllimport)
#endif

#define ZEND_DLEXPORT		__declspec(dllexport)
#define ZEND_DLIMPORT		__declspec(dllimport)

/* 0x00200000L is MB_SERVICE_NOTIFICATION, which is only supported under Windows NT 
 * (and requires _WIN32_WINNT to be defined, which prevents the resulting executable
 * from running under Windows 9x
 * Windows 9x should silently ignore it, so it's being used here directly
 */
#ifndef MB_SERVICE_NOTIFICATION
#define	MB_SERVICE_NOTIFICATION		0x00200000L
#endif

#define ZEND_SERVICE_MB_STYLE		(MB_TOPMOST|MB_SERVICE_NOTIFICATION)

#endif /* ZEND_CONFIG_W32_H */
