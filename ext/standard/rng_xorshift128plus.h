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
   | Authors: Go Kudo <zeriyoshi@gmail.com>                               |
   +----------------------------------------------------------------------+
*/
#ifndef _RNG_XORSHIFT128PLUS_H
#define _RNG_XORSHIFT128PLUS_H

#include "php.h"
#include "php_rng.h"

extern PHPAPI zend_class_entry *rng_ce_RNG_XorShift128Plus;

#define XORSHIFT128PLUS_N 2

typedef struct _rng_xorshift128plus_state {
	uint64_t s[XORSHIFT128PLUS_N];
} rng_xorshift128plus_state;

PHP_MINIT_FUNCTION(rng_xorshift128plus);

#endif
