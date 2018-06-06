/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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

#ifndef PHP_HASH_CRC32_H
#define PHP_HASH_CRC32_H

#include "ext/standard/basic_functions.h"

typedef struct {
	uint32_t state;
} PHP_CRC32_CTX;

PHP_HASH_API void PHP_CRC32Init(PHP_CRC32_CTX *context);
PHP_HASH_API void PHP_CRC32Update(PHP_CRC32_CTX *context, const unsigned char *input, size_t len);
PHP_HASH_API void PHP_CRC32BUpdate(PHP_CRC32_CTX *context, const unsigned char *input, size_t len);
PHP_HASH_API void PHP_CRC32Final(unsigned char digest[4], PHP_CRC32_CTX *context);
PHP_HASH_API int PHP_CRC32Copy(const php_hash_ops *ops, PHP_CRC32_CTX *orig_context, PHP_CRC32_CTX *copy_context);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
