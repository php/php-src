/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Michael Wallner <mike@php.net>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_HASH_TYPES_H
#define PHP_HASH_TYPES_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifndef PHP_WIN32
#include "php_config.h"
#endif
#endif

#ifndef PHP_WIN32
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
#else
#error "Need a 64bit integer type"
#endif
#else
#define L64(x) x##i64
typedef unsigned __int64 php_hash_uint64;
typedef unsigned __int32 php_hash_uint32;
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
