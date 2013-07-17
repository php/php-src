/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
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

#ifndef ZEND_TYPES_H
#define ZEND_TYPES_H

typedef unsigned char zend_bool;
typedef unsigned char zend_uchar;
typedef unsigned int zend_uint;
typedef unsigned long zend_ulong;
typedef unsigned short zend_ushort;

#ifdef ZEND_USE_LEGACY_STRING_TYPES
# define zend_str_size_int int
# define zend_str_size_uint unsigned int
# define zend_str_size_size_t size_t
# define zend_str_size_long long
# define ZEND_SIZE_MAX_LONG LONG_MAX
# define ZEND_SIZE_MAX_INT INT_MAX
typedef int zend_str_size;
#else
# define zend_str_size_int zend_str_size
# define zend_str_size_uint zend_str_size
# define zend_str_size_size_t zend_str_size
# define zend_str_size_long zend_str_size
# ifdef PHP_WIN32
#  ifdef _WIN64
#   define ZEND_SIZE_MAX_LONG  _UI64_MAX
#   define ZEND_SIZE_MAX_INT  _UI64_MAX
#  else
#   define ZEND_SIZE_MAX_LONG  _UI32_MAX
#   define ZEND_SIZE_MAX_INT  _UI32_MAX
#  endif
# else
#  define ZEND_SIZE_MAX_LONG  SIZE_MAX
#  define ZEND_SIZE_MAX_INT  SIZE_MAX
# endif
typedef size_t zend_str_size;
#endif

#define HAVE_ZEND_LONG64
#ifdef ZEND_WIN32
typedef __int64 zend_long64;
typedef unsigned __int64 zend_ulong64;
#elif SIZEOF_LONG_LONG_INT == 8
typedef long long int zend_long64;
typedef unsigned long long int zend_ulong64;
#elif SIZEOF_LONG_LONG == 8
typedef long long zend_long64;
typedef unsigned long long zend_ulong64;
#else
# undef HAVE_ZEND_LONG64
#endif

#ifdef _WIN64
typedef __int64 zend_intptr_t;
typedef unsigned __int64 zend_uintptr_t;
#else
typedef long zend_intptr_t;
typedef unsigned long zend_uintptr_t;
#endif

typedef unsigned int zend_object_handle;
typedef struct _zend_object_handlers zend_object_handlers;
typedef struct _zval_struct zval;

typedef struct _zend_object_value {
	zend_object_handle handle;
	const zend_object_handlers *handlers;
} zend_object_value;

#ifdef ZEND_ENABLE_INT64
# ifdef ZEND_WIN32
#  ifdef _WIN64
typedef __int64 zend_int_t;
typedef unsigned __int64 zend_uint_t;
#   define ZEND_INT_MAX _I64_MAX
#   define ZEND_INT_MIN _I64_MIN
#   define ZEND_UINT_MAX _UI64_MAX
#  else
#   error Cant enable 64 bit integers on non 64 bit platform
#  endif
# else
#  if defined(__LP64__) || defined(_LP64)
typedef int64_t zend_int_t;
typedef uint64_t zend_uint_t;
#   define ZEND_INT_MAX INT64_MAX
#   define ZEND_INT_MIN INT64_MIN
#   define ZEND_UINT_MAX UINT64_MAX
#  else
#   error Cant enable 64 bit integers on non 64 bit platform
#  endif
# endif
# define SIZEOF_ZEND_INT 8
#else
typedef long zend_int_t;
typedef unsigned long zend_uint_t;
# define ZEND_INT_MAX LONG_MAX
# define ZEND_INT_MIN LONG_MIN
# define ZEND_UINT_MAX ULONG_MAX
# define SIZEOF_ZEND_INT SIZEOF_LONG
#endif

#endif /* ZEND_TYPES_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
