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
   | Author: Andi Gutmans <andi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_BCMATH_H
#define PHP_BCMATH_H

#include "libbcmath/src/bcmath.h"
#include "zend_API.h"
#include "ext/standard/php_math_round_mode.h"

extern zend_module_entry bcmath_module_entry;
#define phpext_bcmath_ptr &bcmath_module_entry

#include "php_version.h"
#define PHP_BCMATH_VERSION PHP_VERSION

#define BC_ARENA_SIZE 256

ZEND_BEGIN_MODULE_GLOBALS(bcmath)
	bc_num _zero_;
	bc_num _one_;
	bc_num _two_;
	int bc_precision;
	char *arena;
	size_t arena_offset;
ZEND_END_MODULE_GLOBALS(bcmath)

#if defined(ZTS) && defined(COMPILE_DL_BCMATH)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

ZEND_EXTERN_MODULE_GLOBALS(bcmath)
#define BCG(v) ZEND_MODULE_GLOBALS_ACCESSOR(bcmath, v)

/* Maximum number of digits to extend when scale needs to be extended, such as in undivisible division */
#define BC_MATH_NUMBER_EXPAND_SCALE 10

typedef struct _bcmath_number_obj_t {
	zend_string *value;
	size_t scale;
	bc_num num;
	zend_object std;
} bcmath_number_obj_t;

#endif /* PHP_BCMATH_H */
