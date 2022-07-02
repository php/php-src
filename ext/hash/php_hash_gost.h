/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Michael Wallner <mike@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_HASH_GOST_H
#define PHP_HASH_GOST_H

#include "ext/standard/basic_functions.h"

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
