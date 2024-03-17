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
#include "php.h"
#include "ext/standard/php_math.h"

extern zend_module_entry bcmath_module_entry;
#define phpext_bcmath_ptr &bcmath_module_entry

#include "php_version.h"
#define PHP_BCMATH_VERSION PHP_VERSION

ZEND_BEGIN_MODULE_GLOBALS(bcmath)
	bc_num _zero_;
	bc_num _one_;
	bc_num _two_;
	int bc_precision;
ZEND_END_MODULE_GLOBALS(bcmath)

#if defined(ZTS) && defined(COMPILE_DL_BCMATH)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

ZEND_EXTERN_MODULE_GLOBALS(bcmath)
#define BCG(v) ZEND_MODULE_GLOBALS_ACCESSOR(bcmath, v)

typedef struct _bc_num_obj {
	zend_object std;
	bc_num bc_num;
} bc_num_obj;

typedef enum {
	BC_NUM_ADD,
	BC_NUM_SUB,
	BC_NUM_MUL,
	BC_NUM_DIV,
	BC_NUM_MOD,
	BC_NUM_POW
} bc_num_calculation_type;

#endif /* PHP_BCMATH_H */
