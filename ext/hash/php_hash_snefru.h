/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
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

#ifndef PHP_HASH_SNEFRU_H
#define PHP_HASH_SNEFRU_H

/* SNEFRU-2.5a with 8 passes and 256 bit hash output
 * AKA "Xerox Secure Hash Function" 
 */

#include "ext/standard/basic_functions.h"

/* SNEFRU context */
typedef struct {
	php_hash_uint32 state[16];
	php_hash_uint32 count[2];
	unsigned char length;
	unsigned char buffer[32];
} PHP_SNEFRU_CTX;

PHP_HASH_API void PHP_SNEFRUInit(PHP_SNEFRU_CTX *);
PHP_HASH_API void PHP_SNEFRUUpdate(PHP_SNEFRU_CTX *, const unsigned char *, size_t);
PHP_HASH_API void PHP_SNEFRUFinal(unsigned char[32], PHP_SNEFRU_CTX *);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
