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

#ifndef PHP_HASH_ADLER32_H
#define PHP_HASH_ADLER32_H

typedef struct {
	uint32_t state;
} PHP_ADLER32_CTX;
#define PHP_ADLER32_SPEC "l."

PHP_HASH_API void PHP_ADLER32Init(PHP_ADLER32_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args);
PHP_HASH_API void PHP_ADLER32Update(PHP_ADLER32_CTX *context, const unsigned char *input, size_t len);
PHP_HASH_API void PHP_ADLER32Final(unsigned char digest[4], PHP_ADLER32_CTX *context);
PHP_HASH_API zend_result PHP_ADLER32Copy(const php_hash_ops *ops, const PHP_ADLER32_CTX *orig_context, PHP_ADLER32_CTX *copy_context);

#endif
