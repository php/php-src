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
   | Author: Stefan Esser <sesser@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef SHA1_H
#define SHA1_H

/* SHA1 context. */
typedef struct {
	uint32_t state[5];		/* state (ABCD) */
	uint32_t count[2];		/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} PHP_SHA1_CTX;
#define PHP_SHA1_SPEC "l5l2b64."

#define PHP_SHA1Init(ctx) PHP_SHA1InitArgs(ctx, NULL)
PHPAPI void PHP_SHA1InitArgs(PHP_SHA1_CTX *, ZEND_ATTRIBUTE_UNUSED HashTable *);
PHPAPI void PHP_SHA1Update(PHP_SHA1_CTX *, const unsigned char *, size_t);
PHPAPI void PHP_SHA1Final(unsigned char[20], PHP_SHA1_CTX *);
PHPAPI void make_sha1_digest(char *sha1str, const unsigned char *digest);

#endif
