
/* $Id$ */

#ifndef PHP_HASH_TYPES_H
#define PHP_HASH_TYPES_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef _MSC_VER
#if SIZEOF_LONG == 8
#define L64(x) x
typedef unsigned long php_hash_uint64;
#if SIZEOF_INT == 4
typedef unsigned int php_hash_uint32;
#elif SIZEOF_SHORT == 4
typedef unsigned short php_hash_uint32;
#else
#error "Need a 32bit integer type"
#endif
#elif SIZEOF_LONG_LONG == 8
#define L64(x) x##LL
typedef unsigned long long php_hash_uint64;
#if SIZEOF_INT == 4
typedef unsigned int php_hash_uint32;
#elif SIZEOF_LONG == 4
typedef unsigned long php_hash_uint32;
#else
#error "Need a 32bit integer type"
#endif
#endif
#else
#define L64(x) x##i64
typedef unsigned __int64 php_hash_uint64;
typedef unsigned __int32 php_hash_uint32;
#endif

#endif

