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
  | Author: Michael Maclean <mgdm@php.net>                               |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_HASH_FNV_H
#define PHP_HASH_FNV_H

#define PHP_FNV1_32_INIT ((php_hash_uint32)0x811c9dc5)
#define PHP_FNV1_32A_INIT PHP_FNV1_32_INIT

#define PHP_FNV_32_PRIME ((php_hash_uint32)0x01000193)

#define PHP_FNV1_64_INIT ((php_hash_uint64)0xcbf29ce484222325ULL)
#define PHP_FNV1A_64_INIT FNV1_64_INIT

#define PHP_FNV_64_PRIME ((php_hash_uint64)0x100000001b3ULL)


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
	php_hash_uint32 state;
} PHP_FNV132_CTX;

typedef struct {
	php_hash_uint64 state;
} PHP_FNV164_CTX;


PHP_HASH_API void PHP_FNV132Init(PHP_FNV132_CTX *context);
PHP_HASH_API void PHP_FNV132Update(PHP_FNV132_CTX *context, const unsigned char *input, unsigned int inputLen);
PHP_HASH_API void PHP_FNV1a32Update(PHP_FNV132_CTX *context, const unsigned char *input, unsigned int inputLen);
PHP_HASH_API void PHP_FNV132Final(unsigned char digest[16], PHP_FNV132_CTX * context);

PHP_HASH_API void PHP_FNV164Init(PHP_FNV164_CTX *context);
PHP_HASH_API void PHP_FNV164Update(PHP_FNV164_CTX *context, const unsigned char *input, unsigned int inputLen);
PHP_HASH_API void PHP_FNV1a64Update(PHP_FNV164_CTX *context, const unsigned char *input, unsigned int inputLen);
PHP_HASH_API void PHP_FNV164Final(unsigned char digest[16], PHP_FNV164_CTX * context);

static php_hash_uint32 fnv_32_buf(void *buf, size_t len, php_hash_uint32 hval, int alternate);
static php_hash_uint64 fnv_64_buf(void *buf, size_t len, php_hash_uint64 hval, int alternate);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
