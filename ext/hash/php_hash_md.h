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
   | Original Author: Rasmus Lerdorf <rasmus@lerdorf.on.ca>               |
   | Modified for pHASH by: Sara Golemon <pollita@php.net>
   +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_MD_H
#define PHP_HASH_MD_H

#include "ext/standard/md5.h"

/* MD4 context */
typedef struct {
	uint32_t state[4];
	uint32_t count[2];
	unsigned char buffer[64];
} PHP_MD4_CTX;
#define PHP_MD4_SPEC "l4l2b64."

PHP_HASH_API void PHP_MD4Init(PHP_MD4_CTX *);
PHP_HASH_API void PHP_MD4Update(PHP_MD4_CTX *context, const unsigned char *, size_t);
PHP_HASH_API void PHP_MD4Final(unsigned char[16], PHP_MD4_CTX *);

/* MD2 context */
typedef struct {
	unsigned char state[48];
	unsigned char checksum[16];
	unsigned char buffer[16];
	unsigned char in_buffer;
} PHP_MD2_CTX;
#define PHP_MD2_SPEC "b48b16b16b."

PHP_HASH_API void PHP_MD2Init(PHP_MD2_CTX *context);
PHP_HASH_API void PHP_MD2Update(PHP_MD2_CTX *context, const unsigned char *, size_t);
PHP_HASH_API void PHP_MD2Final(unsigned char[16], PHP_MD2_CTX *);

#endif
