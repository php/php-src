/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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

#ifndef PHP_HASH_SHA3_H
#define PHP_HASH_SHA3_H

#include "php.h"

typedef struct {
#ifdef HAVE_SLOW_HASH3
	unsigned char state[200]; // 5 * 5 * sizeof(uint64)
	uint32_t pos;
#else
	void *hashinstance;
#endif
} PHP_SHA3_CTX;

typedef PHP_SHA3_CTX PHP_SHA3_224_CTX;
typedef PHP_SHA3_CTX PHP_SHA3_256_CTX;
typedef PHP_SHA3_CTX PHP_SHA3_384_CTX;
typedef PHP_SHA3_CTX PHP_SHA3_512_CTX;

PHP_HASH_API void PHP_SHA3224Init(PHP_SHA3_224_CTX*);
PHP_HASH_API void PHP_SHA3224Update(PHP_SHA3_224_CTX*, const unsigned char*, size_t);
PHP_HASH_API void PHP_SAH3224Final(unsigned char[32], PHP_SHA3_224_CTX*);

PHP_HASH_API void PHP_SHA3256Init(PHP_SHA3_256_CTX*);
PHP_HASH_API void PHP_SHA3256Update(PHP_SHA3_256_CTX*, const unsigned char*, size_t);
PHP_HASH_API void PHP_SAH3256Final(unsigned char[32], PHP_SHA3_256_CTX*);

PHP_HASH_API void PHP_SHA3384Init(PHP_SHA3_384_CTX*);
PHP_HASH_API void PHP_SHA3384Update(PHP_SHA3_384_CTX*, const unsigned char*, size_t);
PHP_HASH_API void PHP_SAH3384Final(unsigned char[32], PHP_SHA3_384_CTX*);

PHP_HASH_API void PHP_SHA3512Init(PHP_SHA3_512_CTX*);
PHP_HASH_API void PHP_SHA3512Update(PHP_SHA3_512_CTX*, const unsigned char*, size_t);
PHP_HASH_API void PHP_SAH3512Final(unsigned char[32], PHP_SHA3_512_CTX*);

#endif
