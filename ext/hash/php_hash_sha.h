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
   | SHA1 Author: Stefan Esser <sesser@php.net>                           |
   | SHA256 Author: Sara Golemon <pollita@php.net>                        |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_SHA_H
#define PHP_HASH_SHA_H

#include "ext/standard/sha1.h"
#include "ext/standard/basic_functions.h"

/* SHA224 context. */
typedef struct {
	uint32_t state[8];		/* state */
	uint32_t count[2];		/* number of bits, modulo 2^64 */
	unsigned char buffer[64];	/* input buffer */
} PHP_SHA224_CTX;
#define PHP_SHA224_SPEC "l8l2b64."

PHP_HASH_API void PHP_SHA224Init(PHP_SHA224_CTX *);
PHP_HASH_API void PHP_SHA224Update(PHP_SHA224_CTX *, const unsigned char *, size_t);
PHP_HASH_API void PHP_SHA224Final(unsigned char[28], PHP_SHA224_CTX *);

/* SHA256 context. */
typedef struct {
	uint32_t state[8];		/* state */
	uint32_t count[2];		/* number of bits, modulo 2^64 */
	unsigned char buffer[64];	/* input buffer */
} PHP_SHA256_CTX;
#define PHP_SHA256_SPEC "l8l2b64."

PHP_HASH_API void PHP_SHA256Init(PHP_SHA256_CTX *);
PHP_HASH_API void PHP_SHA256Update(PHP_SHA256_CTX *, const unsigned char *, size_t);
PHP_HASH_API void PHP_SHA256Final(unsigned char[32], PHP_SHA256_CTX *);

/* SHA384 context */
typedef struct {
	uint64_t state[8];	/* state */
	uint64_t count[2];	/* number of bits, modulo 2^128 */
	unsigned char buffer[128];	/* input buffer */
} PHP_SHA384_CTX;
#define PHP_SHA384_SPEC "q8q2b128."

PHP_HASH_API void PHP_SHA384Init(PHP_SHA384_CTX *);
PHP_HASH_API void PHP_SHA384Update(PHP_SHA384_CTX *, const unsigned char *, size_t);
PHP_HASH_API void PHP_SHA384Final(unsigned char[48], PHP_SHA384_CTX *);

/* SHA512 context */
typedef struct {
	uint64_t state[8];	/* state */
	uint64_t count[2];	/* number of bits, modulo 2^128 */
	unsigned char buffer[128];	/* input buffer */
} PHP_SHA512_CTX;
#define PHP_SHA512_SPEC "q8q2b128."

PHP_HASH_API void PHP_SHA512Init(PHP_SHA512_CTX *);
PHP_HASH_API void PHP_SHA512Update(PHP_SHA512_CTX *, const unsigned char *, size_t);
PHP_HASH_API void PHP_SHA512Final(unsigned char[64], PHP_SHA512_CTX *);

PHP_HASH_API void PHP_SHA512_256Init(PHP_SHA512_CTX *);
#define PHP_SHA512_256Update PHP_SHA512Update
PHP_HASH_API void PHP_SHA512_256Final(unsigned char[32], PHP_SHA512_CTX *);

PHP_HASH_API void PHP_SHA512_224Init(PHP_SHA512_CTX *);
#define PHP_SHA512_224Update PHP_SHA512Update
PHP_HASH_API void PHP_SHA512_224Final(unsigned char[28], PHP_SHA512_CTX *);

#endif /* PHP_HASH_SHA_H */
