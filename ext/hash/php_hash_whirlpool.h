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
   | Author: Michael Wallner <mike@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_WHIRLPOOL_H
#define PHP_HASH_WHIRLPOOL_H

/* WHIRLPOOL context */
typedef struct {
	uint64_t state[8];
	unsigned char bitlength[32];
	struct {
		int pos;
		int bits;
		unsigned char data[64];
	} buffer;
} PHP_WHIRLPOOL_CTX;
#define PHP_WHIRLPOOL_SPEC "q8b32iib64."

PHP_HASH_API void PHP_WHIRLPOOLInit(PHP_WHIRLPOOL_CTX *, ZEND_ATTRIBUTE_UNUSED HashTable *);
PHP_HASH_API void PHP_WHIRLPOOLUpdate(PHP_WHIRLPOOL_CTX *, const unsigned char *, size_t);
PHP_HASH_API void PHP_WHIRLPOOLFinal(unsigned char[64], PHP_WHIRLPOOL_CTX *);

#endif
