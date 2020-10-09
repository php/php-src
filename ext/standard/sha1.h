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
   | Author: Stefan Esser <sesser@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef SHA1_H
#define SHA1_H

#include "ext/standard/basic_functions.h"

/* SHA1 context. */
typedef struct {
	uint32_t state[5];		/* state (ABCD) */
	uint32_t count[2];		/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} PHP_SHA1_CTX;
#define PHP_SHA1_SPEC "l5l2b64."

PHPAPI void PHP_SHA1Init(PHP_SHA1_CTX *);
PHPAPI void PHP_SHA1Update(PHP_SHA1_CTX *, const unsigned char *, size_t);
PHPAPI void PHP_SHA1Final(unsigned char[20], PHP_SHA1_CTX *);
PHPAPI void make_sha1_digest(char *sha1str, const unsigned char *digest);

#endif
