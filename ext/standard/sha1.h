/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stefan Esser <sesser@php.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef SHA1_H
#define SHA1_H

#include "ext/standard/basic_functions.h"

/* SHA1 context. */
typedef struct {
	php_uint32 state[5];		/* state (ABCD) */
	php_uint32 count[2];		/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} PHP_SHA1_CTX;

PHPAPI void PHP_SHA1Init(PHP_SHA1_CTX *);
PHPAPI void PHP_SHA1Update(PHP_SHA1_CTX *, const unsigned char *, unsigned int);
PHPAPI void PHP_SHA1Final(unsigned char[20], PHP_SHA1_CTX *);

PHP_FUNCTION(sha1);
PHP_FUNCTION(sha1_file);

#endif
