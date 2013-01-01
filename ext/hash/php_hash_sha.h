/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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

/* $Id$ */

#ifndef PHP_HASH_SHA_H
#define PHP_HASH_SHA_H

/* When SHA is removed from Core,
	the ext/standard/sha1.c file can be removed
	and the ext/standard/sha1.h file can be reduced to:
		#define PHP_HASH_SHA1_NOT_IN_CORE
		#include "ext/hash/php_hash_sha.h"
	Don't forget to remove sha1() and sha1_file() from basic_functions.c
 */
#include "ext/standard/sha1.h"
#include "ext/standard/basic_functions.h"

#ifdef PHP_HASH_SHA1_NOT_IN_CORE

/* SHA1 context. */
typedef struct {
	php_hash_uint32 state[5];		/* state (ABCD) */
	php_hash_uint32 count[2];		/* number of bits, modulo 2^64 */
	unsigned char buffer[64];	/* input buffer */
} PHP_SHA1_CTX;

PHP_HASH_API void PHP_SHA1Init(PHP_SHA1_CTX *);
PHP_HASH_API void PHP_SHA1Update(PHP_SHA1_CTX *, const unsigned char *, unsigned int);
PHP_HASH_API void PHP_SHA1Final(unsigned char[20], PHP_SHA1_CTX *);

PHP_FUNCTION(sha1);
PHP_FUNCTION(sha1_file);

#endif /* PHP_HASH_SHA1_NOT_IN_CORE */

/* SHA224 context. */
typedef struct {
	php_hash_uint32 state[8];		/* state */
	php_hash_uint32 count[2];		/* number of bits, modulo 2^64 */
	unsigned char buffer[64];	/* input buffer */
} PHP_SHA224_CTX;

PHP_HASH_API void PHP_SHA224Init(PHP_SHA224_CTX *);
PHP_HASH_API void PHP_SHA224Update(PHP_SHA224_CTX *, const unsigned char *, unsigned int);
PHP_HASH_API void PHP_SHA224Final(unsigned char[28], PHP_SHA224_CTX *);

/* SHA256 context. */
typedef struct {
	php_hash_uint32 state[8];		/* state */
	php_hash_uint32 count[2];		/* number of bits, modulo 2^64 */
	unsigned char buffer[64];	/* input buffer */
} PHP_SHA256_CTX;

PHP_HASH_API void PHP_SHA256Init(PHP_SHA256_CTX *);
PHP_HASH_API void PHP_SHA256Update(PHP_SHA256_CTX *, const unsigned char *, unsigned int);
PHP_HASH_API void PHP_SHA256Final(unsigned char[32], PHP_SHA256_CTX *);

/* SHA384 context */
typedef struct {
	php_hash_uint64 state[8];	/* state */
	php_hash_uint64 count[2];	/* number of bits, modulo 2^128 */
	unsigned char buffer[128];	/* input buffer */
} PHP_SHA384_CTX;

PHP_HASH_API void PHP_SHA384Init(PHP_SHA384_CTX *);
PHP_HASH_API void PHP_SHA384Update(PHP_SHA384_CTX *, const unsigned char *, unsigned int);
PHP_HASH_API void PHP_SHA384Final(unsigned char[48], PHP_SHA384_CTX *);

/* SHA512 context */
typedef struct {
	php_hash_uint64 state[8];	/* state */
	php_hash_uint64 count[2];	/* number of bits, modulo 2^128 */
	unsigned char buffer[128];	/* input buffer */
} PHP_SHA512_CTX;

PHP_HASH_API void PHP_SHA512Init(PHP_SHA512_CTX *);
PHP_HASH_API void PHP_SHA512Update(PHP_SHA512_CTX *, const unsigned char *, unsigned int);
PHP_HASH_API void PHP_SHA512Final(unsigned char[64], PHP_SHA512_CTX *);

#endif /* PHP_HASH_SHA_H */
