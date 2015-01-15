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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_CONFIG_NW_H
#define ZEND_CONFIG_NW_H


#include <string.h>
#include <float.h>

typedef unsigned long ulong;
typedef unsigned int uint;

#define HAVE_ALLOCA 1
#define HAVE_LIMITS_H 1
/* #include <malloc.h> */

#define HAVE_STRING_H 1
#define HAVE_SYS_SELECT_H 1
#define HAVE_STDLIB_H 1
#undef HAVE_KILL
#define HAVE_GETPID 1
/* #define HAVE_ALLOCA_H 1 */
#define HAVE_MEMCPY 1
#define HAVE_STRDUP 1
#define HAVE_SYS_TYPES_H 1
/* #define HAVE_STDIOSTR_H 1 */
#define HAVE_CLASS_ISTDIOSTREAM
#define istdiostream stdiostream
#define HAVE_STDARG_H	1
#define HAVE_DLFCN_H	1
/* #define HAVE_LIBDL 1 */
#define HAVE_SNPRINTF	1
#define HAVE_VSNPRINTF	1

/*
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define zend_isinf(a)	0
#define zend_finite(x)	_finite(x)
#define zend_isnan(x)	_isnan(x)
*/

#define zend_sprintf sprintf

/* This will cause the compilation process to be MUCH longer, but will generate
 * a much quicker PHP binary
 */
/*
#undef inline
#ifdef ZEND_WIN32_FORCE_INLINE
# define inline __forceinline
#else
# define inline
#endif
*/

/*
#define zend_finite(A) _finite(A)
#define zend_isnan(A) _isnan(A)
*/

#endif /* ZEND_CONFIG_NW_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
