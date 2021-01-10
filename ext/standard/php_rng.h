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

#ifndef PHP_RNG_H
#define PHP_RNG_H

#include "php.h"

extern PHPAPI zend_class_entry *rng_ce_RNG_RNGInterface;
extern PHPAPI zend_class_entry *rng_ce_RNG_RNG64Interface;

typedef struct _php_rng php_rng;
typedef struct _php_rng {
	uint32_t (*next)(php_rng*);
	uint64_t (*next64)(php_rng*);
	zend_long (*range)(php_rng*); // optional for object defined range implementation.
	void *state;
	zend_object std;
} php_rng;

static inline php_rng *rng_from_obj(zend_object *obj) {
	return (php_rng*)((char*)(obj) - XtOffsetOf(php_rng, std));
}

#define Z_RNG_P(zval) rng_from_obj(Z_OBJ_P(zval))

PHP_MINIT_FUNCTION(rng);

#define RNG_NAMESPACE "RNG\\"

PHPAPI php_rng *php_rng_initialize(uint32_t (*next)(php_rng*), uint64_t (*next64)(php_rng*));
PHPAPI int php_rng_next(uint32_t*, zval*);
PHPAPI int php_rng_next64(uint64_t*, zval*);
PHPAPI zend_long php_rng_range(zval*, zend_long, zend_long);

#endif
