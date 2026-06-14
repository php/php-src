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
   | Author: Anatol Belski <ab@php.net>                                   |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_MURMUR_H
#define PHP_HASH_MURMUR_H

typedef struct {
	uint32_t h;
	uint32_t carry;
	uint32_t len;
} PHP_MURMUR3A_CTX;
#define PHP_MURMUR3A_SPEC "lll"

PHP_HASH_API void PHP_MURMUR3AInit(PHP_MURMUR3A_CTX *ctx, HashTable *args);
PHP_HASH_API void PHP_MURMUR3AUpdate(PHP_MURMUR3A_CTX *ctx, const unsigned char *in, size_t len);
PHP_HASH_API void PHP_MURMUR3AFinal(unsigned char digest[4], PHP_MURMUR3A_CTX *ctx);
PHP_HASH_API zend_result PHP_MURMUR3ACopy(const php_hash_ops *ops, const PHP_MURMUR3A_CTX *orig_context, PHP_MURMUR3A_CTX *copy_context);

typedef struct {
	uint32_t h[4];
	uint32_t carry[4];
	uint32_t len;
} PHP_MURMUR3C_CTX;
#define PHP_MURMUR3C_SPEC "lllllllll"

PHP_HASH_API void PHP_MURMUR3CInit(PHP_MURMUR3C_CTX *ctx, HashTable *args);
PHP_HASH_API void PHP_MURMUR3CUpdate(PHP_MURMUR3C_CTX *ctx, const unsigned char *in, size_t len);
PHP_HASH_API void PHP_MURMUR3CFinal(unsigned char digest[16], PHP_MURMUR3C_CTX *ctx);
PHP_HASH_API zend_result PHP_MURMUR3CCopy(const php_hash_ops *ops, const PHP_MURMUR3C_CTX *orig_context, PHP_MURMUR3C_CTX *copy_context);

typedef struct {
	uint64_t h[2];
	uint64_t carry[2];
	uint32_t len;
} PHP_MURMUR3F_CTX;
#define PHP_MURMUR3F_SPEC "qqqql"

PHP_HASH_API void PHP_MURMUR3FInit(PHP_MURMUR3F_CTX *ctx, HashTable *args);
PHP_HASH_API void PHP_MURMUR3FUpdate(PHP_MURMUR3F_CTX *ctx, const unsigned char *in, size_t len);
PHP_HASH_API void PHP_MURMUR3FFinal(unsigned char digest[16], PHP_MURMUR3F_CTX *ctx);
PHP_HASH_API zend_result PHP_MURMUR3FCopy(const php_hash_ops *ops, const PHP_MURMUR3F_CTX *orig_context, PHP_MURMUR3F_CTX *copy_context);

#endif /* PHP_HASH_MURMUR_H */

