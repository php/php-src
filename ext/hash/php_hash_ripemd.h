/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sara Golemon <pollita@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_RIPEMD_H
#define PHP_HASH_RIPEMD_H
#include "ext/standard/basic_functions.h"

/* RIPEMD context. */
typedef struct {
	uint32_t state[4];		/* state (ABCD) */
	uint32_t count[2];		/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} PHP_RIPEMD128_CTX;
#define PHP_RIPEMD128_SPEC "l4l2b64."

typedef struct {
	uint32_t state[5];		/* state (ABCD) */
	uint32_t count[2];		/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} PHP_RIPEMD160_CTX;
#define PHP_RIPEMD160_SPEC "l5l2b64."

typedef struct {
	uint32_t state[8];		/* state (ABCD) */
	uint32_t count[2];		/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} PHP_RIPEMD256_CTX;
#define PHP_RIPEMD256_SPEC "l8l2b64."

typedef struct {
	uint32_t state[10];		/* state (ABCD) */
	uint32_t count[2];		/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} PHP_RIPEMD320_CTX;
#define PHP_RIPEMD320_SPEC "l10l2b64."

PHP_HASH_API void PHP_RIPEMD128Init(PHP_RIPEMD128_CTX *, ZEND_ATTRIBUTE_UNUSED HashTable *);
PHP_HASH_API void PHP_RIPEMD128Update(PHP_RIPEMD128_CTX *, const unsigned char *, size_t);
PHP_HASH_API void PHP_RIPEMD128Final(unsigned char[16], PHP_RIPEMD128_CTX *);

PHP_HASH_API void PHP_RIPEMD160Init(PHP_RIPEMD160_CTX *, ZEND_ATTRIBUTE_UNUSED HashTable *);
PHP_HASH_API void PHP_RIPEMD160Update(PHP_RIPEMD160_CTX *, const unsigned char *, size_t);
PHP_HASH_API void PHP_RIPEMD160Final(unsigned char[20], PHP_RIPEMD160_CTX *);

PHP_HASH_API void PHP_RIPEMD256Init(PHP_RIPEMD256_CTX *, ZEND_ATTRIBUTE_UNUSED HashTable *);
PHP_HASH_API void PHP_RIPEMD256Update(PHP_RIPEMD256_CTX *, const unsigned char *, size_t);
PHP_HASH_API void PHP_RIPEMD256Final(unsigned char[32], PHP_RIPEMD256_CTX *);

PHP_HASH_API void PHP_RIPEMD320Init(PHP_RIPEMD320_CTX *, ZEND_ATTRIBUTE_UNUSED HashTable *);
PHP_HASH_API void PHP_RIPEMD320Update(PHP_RIPEMD320_CTX *, const unsigned char *, size_t);
PHP_HASH_API void PHP_RIPEMD320Final(unsigned char[40], PHP_RIPEMD320_CTX *);

#endif /* PHP_HASH_RIPEMD_H */
