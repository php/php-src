/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2005 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Michael Wallner <mike@php.net>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_HASH_WHIRLPOOL_H
#define PHP_HASH_WHIRLPOOL_H

#include "ext/standard/basic_functions.h"

#if defined(SIZEOF_LONG) && (SIZEOF_LONG >= 8)
#	define L64(n)	(n##L)
typedef unsigned long php_hash_uint64;
#else
#	ifdef PHP_WIN32
#		define L64(n)	(n##i64)
typedef unsigned __int64 php_hash_uint64;
#	else
#		define L64(n)	(n##LL)
typedef unsigned long long php_hash_uint64;
#	endif
#endif

/* WHIRLPOOL context */
typedef struct {
	php_hash_uint64 state[8];
	unsigned char bitlength[32];
	struct {
		int pos;
		int bits;
		unsigned char data[64];
	} buffer;
} PHP_WHIRLPOOL_CTX;

PHP_HASH_API void PHP_WHIRLPOOLInit(PHP_WHIRLPOOL_CTX *);
PHP_HASH_API void PHP_WHIRLPOOLUpdate(PHP_WHIRLPOOL_CTX *, const unsigned char *, uint);
PHP_HASH_API void PHP_WHIRLPOOLFinal(unsigned char[64], PHP_WHIRLPOOL_CTX *);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
