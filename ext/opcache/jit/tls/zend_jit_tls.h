/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
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

#ifndef ZEND_JIT_TLS_H
#define ZEND_JIT_TLS_H

#include "Zend/zend_types.h"

#include <stdint.h>
#include <stddef.h>

zend_result zend_jit_resolve_tsrm_ls_cache_offsets(
	size_t *tcb_offset,
	size_t *module_index,
	size_t *module_offset
);

/* Used for testing */
void *zend_jit_tsrm_ls_cache_address(
	size_t tcb_offset,
	size_t module_index,
	size_t module_offset
);

#endif /* ZEND_JIT_TLS_H */
