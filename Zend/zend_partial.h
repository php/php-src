/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Arnaud Le Blanc <arnaud.lb@gmail.com>                       |
   +----------------------------------------------------------------------+
*/
#ifndef ZEND_PARTIAL_H
#define ZEND_PARTIAL_H

#include "zend_compile.h"

BEGIN_EXTERN_C()

/* Create a partial application of 'function'
 * 'declaring_lineno_ptr' should be a pointer the zend_op.lineno or
 * zend_ast.lineno that declares the PFA. The address is used to build a cache
 * key. */
void zend_partial_create(zval *result, zval *this_ptr, zend_function *function,
		uint32_t argc, zval *argv, zend_array *extra_named_params,
		const zend_array *named_positions,
		const zend_op_array *declaring_op_array,
		const uint32_t *declaring_lineno_ptr, void **cache_slot,
		bool uses_variadic_placeholder);

void zend_partial_op_array_dtor(zval *pDest);

END_EXTERN_C()

#endif
