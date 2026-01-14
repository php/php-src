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

#define ZEND_PARTIAL_USES_VARIADIC_PLACEHOLDER (1<<0)
/* Whether the PFA is cacheable in SHM. This flag is set only when the op_array that declares the PFA is itself in SHM,
 * for two reasons: Avoids SHM churn, and ensures that the PFA cache key is unique. */
#define ZEND_PARTIAL_CACHEABLE_IN_SHM          (1<<1)
#define ZEND_PARTIAL_FLAGS                     (ZEND_PARTIAL_USES_VARIADIC_PLACEHOLDER|ZEND_PARTIAL_CACHEABLE_IN_SHM)

/* Create a partial application of 'function'
 * 'declaring_lineno_ptr' should be a pointer the zend_op.lineno or
 * zend_ast.lineno that declares the PFA. The address is used to build a cache
 * key. */
void zend_partial_create(zval *result, zval *this_ptr, zend_function *function,
		uint32_t argc, zval *argv, zend_array *extra_named_params,
		const zend_array *named_positions,
		zend_string *declaring_filename,
		const uint32_t *declaring_lineno_ptr, void **cache_slot,
		zend_string *pfa_name, uint32_t flags);

void zend_partial_op_array_dtor(zval *pDest);

END_EXTERN_C()

#endif
