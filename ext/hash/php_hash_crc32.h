/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Michael Wallner <mike@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_CRC32_H
#define PHP_HASH_CRC32_H

#include "ext/standard/basic_functions.h"

typedef struct {
	uint32_t state;
} PHP_CRC32_CTX;
#define PHP_CRC32_SPEC "l."

PHP_HASH_API void PHP_CRC32Init(PHP_CRC32_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args);
PHP_HASH_API void PHP_CRC32Update(PHP_CRC32_CTX *context, const unsigned char *input, size_t len);
PHP_HASH_API void PHP_CRC32BUpdate(PHP_CRC32_CTX *context, const unsigned char *input, size_t len);
PHP_HASH_API void PHP_CRC32CUpdate(PHP_CRC32_CTX *context, const unsigned char *input, size_t len);
PHP_HASH_API void PHP_CRC32LEFinal(unsigned char digest[4], PHP_CRC32_CTX *context);
PHP_HASH_API void PHP_CRC32BEFinal(unsigned char digest[4], PHP_CRC32_CTX *context);
PHP_HASH_API zend_result PHP_CRC32Copy(const php_hash_ops *ops, const PHP_CRC32_CTX *orig_context, PHP_CRC32_CTX *copy_context);

#endif
