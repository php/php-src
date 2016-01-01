/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
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

/* $Id$ */

#ifndef PHP_HASH_HAVAL_H
#define PHP_HASH_HAVAL_H

#include "ext/standard/basic_functions.h"
/* HAVAL context. */
typedef struct {
	php_hash_uint32 state[8];
	php_hash_uint32 count[2];
	unsigned char buffer[128];

	char passes;
	short output;
	void (*Transform)(php_hash_uint32 state[8], const unsigned char block[128]);
} PHP_HAVAL_CTX;

#define PHP_HASH_HAVAL_INIT_DECL(p,b)	PHP_HASH_API void PHP_##p##HAVAL##b##Init(PHP_HAVAL_CTX *); \
										PHP_HASH_API void PHP_HAVAL##b##Final(unsigned char*, PHP_HAVAL_CTX *);

PHP_HASH_API void PHP_HAVALUpdate(PHP_HAVAL_CTX *, const unsigned char *, unsigned int);

PHP_HASH_HAVAL_INIT_DECL(3,128)
PHP_HASH_HAVAL_INIT_DECL(3,160)
PHP_HASH_HAVAL_INIT_DECL(3,192)
PHP_HASH_HAVAL_INIT_DECL(3,224)
PHP_HASH_HAVAL_INIT_DECL(3,256)

PHP_HASH_HAVAL_INIT_DECL(4,128)
PHP_HASH_HAVAL_INIT_DECL(4,160)
PHP_HASH_HAVAL_INIT_DECL(4,192)
PHP_HASH_HAVAL_INIT_DECL(4,224)
PHP_HASH_HAVAL_INIT_DECL(4,256)

PHP_HASH_HAVAL_INIT_DECL(5,128)
PHP_HASH_HAVAL_INIT_DECL(5,160)
PHP_HASH_HAVAL_INIT_DECL(5,192)
PHP_HASH_HAVAL_INIT_DECL(5,224)
PHP_HASH_HAVAL_INIT_DECL(5,256)

#endif
