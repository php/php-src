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
  | Author: Michael Maclean <mgdm@php.net>                               |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_FNV_H
#define PHP_HASH_FNV_H

#define PHP_FNV1_32_INIT ((uint32_t)0x811c9dc5)

#define PHP_FNV_32_PRIME ((uint32_t)0x01000193)

#define PHP_FNV1_64_INIT ((uint64_t)0xcbf29ce484222325ULL)

#define PHP_FNV_64_PRIME ((uint64_t)0x100000001b3ULL)


/*
 * hash types
 */
enum php_fnv_type {
	PHP_FNV_NONE  = 0,	/* invalid FNV hash type */
	PHP_FNV0_32   = 1,	/* FNV-0 32 bit hash */
	PHP_FNV1_32   = 2,	/* FNV-1 32 bit hash */
	PHP_FNV1a_32  = 3,	/* FNV-1a 32 bit hash */
	PHP_FNV0_64   = 4,	/* FNV-0 64 bit hash */
	PHP_FNV1_64   = 5,	/* FNV-1 64 bit hash */
	PHP_FNV1a_64  = 6,	/* FNV-1a 64 bit hash */
};

typedef struct {
	uint32_t state;
} PHP_FNV132_CTX;
#define PHP_FNV132_SPEC "l."

typedef struct {
	uint64_t state;
} PHP_FNV164_CTX;
#define PHP_FNV164_SPEC "q."


PHP_HASH_API void PHP_FNV132Init(PHP_FNV132_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args);
PHP_HASH_API void PHP_FNV132Update(PHP_FNV132_CTX *context, const unsigned char *input, size_t inputLen);
PHP_HASH_API void PHP_FNV1a32Update(PHP_FNV132_CTX *context, const unsigned char *input, size_t inputLen);
PHP_HASH_API void PHP_FNV132Final(unsigned char digest[4], PHP_FNV132_CTX * context);

PHP_HASH_API void PHP_FNV164Init(PHP_FNV164_CTX *context, ZEND_ATTRIBUTE_UNUSED HashTable *args);
PHP_HASH_API void PHP_FNV164Update(PHP_FNV164_CTX *context, const unsigned char *input, size_t inputLen);
PHP_HASH_API void PHP_FNV1a64Update(PHP_FNV164_CTX *context, const unsigned char *input, size_t inputLen);
PHP_HASH_API void PHP_FNV164Final(unsigned char digest[8], PHP_FNV164_CTX * context);

static uint32_t fnv_32_buf(void *buf, size_t len, uint32_t hval, int alternate);
static uint64_t fnv_64_buf(void *buf, size_t len, uint64_t hval, int alternate);

#endif
