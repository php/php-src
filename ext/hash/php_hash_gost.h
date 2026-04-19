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

#ifndef PHP_HASH_GOST_H
#define PHP_HASH_GOST_H

/* GOST context */
typedef struct {
	uint32_t state[16];
	uint32_t count[2];
	unsigned char length;
	unsigned char buffer[32];
	const uint32_t (*tables)[4][256];
} PHP_GOST_CTX;
#define PHP_GOST_SPEC "l16l2bb32"

PHP_HASH_API void PHP_GOSTInit(PHP_GOST_CTX *, ZEND_ATTRIBUTE_UNUSED HashTable *args);
PHP_HASH_API void PHP_GOSTUpdate(PHP_GOST_CTX *, const unsigned char *, size_t);
PHP_HASH_API void PHP_GOSTFinal(unsigned char[32], PHP_GOST_CTX *);

#endif
