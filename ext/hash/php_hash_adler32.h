/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Michael Wallner <mike@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_ADLER32_H
#define PHP_HASH_ADLER32_H

#include "ext/standard/basic_functions.h"

typedef struct {
	uint32_t state;
} PHP_ADLER32_CTX;
#define PHP_ADLER32_SPEC "l."

PHP_HASH_API void PHP_ADLER32Init(PHP_ADLER32_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args);
PHP_HASH_API void PHP_ADLER32Update(PHP_ADLER32_CTX *context, const unsigned char *input, size_t len);
PHP_HASH_API void PHP_ADLER32Final(unsigned char digest[4], PHP_ADLER32_CTX *context);
PHP_HASH_API int PHP_ADLER32Copy(const php_hash_ops *ops, PHP_ADLER32_CTX *orig_context, PHP_ADLER32_CTX *copy_context);

#endif
