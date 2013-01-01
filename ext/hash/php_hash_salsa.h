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

#ifndef PHP_HASH_SALSA_H
#define PHP_HASH_SALSA_H

#include "ext/standard/basic_functions.h"

/* SALSA context */
typedef struct {
	php_hash_uint32 state[16];
	unsigned char init:1;
	unsigned char length:7;
	unsigned char buffer[64];
	void (*Transform)(php_hash_uint32 state[16], php_hash_uint32 data[16]);
} PHP_SALSA_CTX;

#define PHP_SALSAInit PHP_SALSA20Init
PHP_HASH_API void PHP_SALSA10Init(PHP_SALSA_CTX *);
PHP_HASH_API void PHP_SALSA20Init(PHP_SALSA_CTX *);

PHP_HASH_API void PHP_SALSAUpdate(PHP_SALSA_CTX *, const unsigned char *, size_t);
PHP_HASH_API void PHP_SALSAFinal(unsigned char[64], PHP_SALSA_CTX *);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
