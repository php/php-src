#ifndef _HAD_ZIPCONF_H
#define _HAD_ZIPCONF_H

/*
   zipconf.h -- platform specific include file

   This file was generated automatically by CMake
   based on ../cmake-zipconf.h.in.
 */
#define LIBZIP_VERSION "1.1.2"
#define LIBZIP_VERSION_MAJOR 1
#define LIBZIP_VERSION_MINOR 1
#define LIBZIP_VERSION_MICRO 2

/* #undef HAVE_INTTYPES_H_LIBZIP */

#if defined(_WIN32)
# if _MSC_VER > 1500
#  define HAVE_STDINT_H_LIBZIP
# else
#  include "win32/php_stdint.h"
# endif
#else
# include <inttypes.h>
#endif
#define HAVE_SYS_TYPES_H_LIBZIP
#define HAVE___INT8_LIBZIP
#define HAVE_INT8_T_LIBZIP
#define HAVE_UINT8_T_LIBZIP
#define HAVE___INT16_LIBZIP
#define HAVE_INT16_T_LIBZIP
#define HAVE_UINT16_T_LIBZIP
#define HAVE___INT32_LIBZIP
#define HAVE_INT32_T_LIBZIP
#define HAVE_UINT32_T_LIBZIP
#define HAVE___INT64_LIBZIP
#define HAVE_INT64_T_LIBZIP
#define HAVE_UINT64_T_LIBZIP
#define SHORT_LIBZIP 2
#define INT_LIBZIP 4
#define LONG_LIBZIP 4
#define LONG_LONG_LIBZIP 8

#if defined(HAVE_STDINT_H_LIBZIP)
#include <stdint.h>
#elif defined(HAVE_INTTYPES_H_LIBZIP)
#include <inttypes.h>
#elif defined(HAVE_SYS_TYPES_H_LIBZIP)
#include <sys/types.h>
#endif

#if defined(HAVE_INT8_T_LIBZIP)
typedef int8_t zip_int8_t;
#elif defined(HAVE___INT8_LIBZIP)
typedef __int8 zip_int8_t;
#else
typedef signed char zip_int8_t;
#endif
#if defined(HAVE_UINT8_T_LIBZIP)
typedef uint8_t zip_uint8_t;
#elif defined(HAVE___INT8_LIBZIP)
typedef unsigned __int8 zip_uint8_t;
#else
typedef unsigned char zip_uint8_t;
#endif
#if defined(HAVE_INT16_T_LIBZIP)
typedef int16_t zip_int16_t;
#elif defined(HAVE___INT16_LIBZIP)
typedef __int16 zip_int16_t;
#elif defined(SHORT_LIBZIP) && SHORT_LIBZIP == 2
typedef signed short zip_int16_t;
#endif
#if defined(HAVE_UINT16_T_LIBZIP)
typedef uint16_t zip_uint16_t;
#elif defined(HAVE___INT16_LIBZIP)
typedef unsigned __int16 zip_uint16_t;
#elif defined(SHORT_LIBZIP) && SHORT_LIBZIP == 2
typedef unsigned short zip_uint16_t;
#endif
#if defined(HAVE_INT32_T_LIBZIP)
typedef int32_t zip_int32_t;
#elif defined(HAVE___INT32_LIBZIP)
typedef __int32 zip_int32_t;
#elif defined(INT_LIBZIP) && INT_LIBZIP == 4
typedef signed int zip_int32_t;
#elif defined(LONG_LIBZIP) && LONG_LIBZIP == 4
typedef signed long zip_int32_t;
#endif
#if defined(HAVE_UINT32_T_LIBZIP)
typedef uint32_t zip_uint32_t;
#elif defined(HAVE___INT32_LIBZIP)
typedef unsigned __int32 zip_uint32_t;
#elif defined(INT_LIBZIP) && INT_LIBZIP == 4
typedef unsigned int zip_uint32_t;
#elif defined(LONG_LIBZIP) && LONG_LIBZIP == 4
typedef unsigned long zip_uint32_t;
#endif
#if defined(HAVE_INT64_T_LIBZIP)
typedef int64_t zip_int64_t;
#elif defined(HAVE___INT64_LIBZIP)
typedef __int64 zip_int64_t;
#elif defined(LONG_LIBZIP) && LONG_LIBZIP == 8
typedef signed long zip_int64_t;
#elif defined(LONG_LONG_LIBZIP) && LONG_LONG_LIBZIP == 8
typedef signed long long zip_int64_t;
#endif
#if defined(HAVE_UINT64_T_LIBZIP)
typedef uint64_t zip_uint64_t;
#elif defined(HAVE___INT64_LIBZIP)
typedef unsigned __int64 zip_uint64_t;
#elif defined(LONG_LIBZIP) && LONG_LONG_LIBZIP == 8
typedef unsigned long zip_uint64_t;
#elif defined(LONG_LONG_LIBZIP) && LONG_LONG_LIBZIP == 8
typedef unsigned long long zip_uint64_t;
#endif

#define ZIP_INT8_MIN	-0x80
#define ZIP_INT8_MAX	 0x7f
#define ZIP_UINT8_MAX	 0xff

#define ZIP_INT16_MIN	-0x8000
#define ZIP_INT16_MAX	 0x7fff
#define ZIP_UINT16_MAX	 0xffff

#define ZIP_INT32_MIN	-0x80000000L
#define ZIP_INT32_MAX	 0x7fffffffL
#define ZIP_UINT32_MAX	 0xffffffffLU

#define ZIP_INT64_MIN	-0x8000000000000000LL
#define ZIP_INT64_MAX	 0x7fffffffffffffffLL
#define ZIP_UINT64_MAX	 0xffffffffffffffffULL

#endif /* zipconf.h */
