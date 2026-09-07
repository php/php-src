/*
   +----------------------------------------------------------------------+
   | Zend Engine, DFG - Data Flow Graph                                   |
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_DFG_H
#define ZEND_DFG_H

#include "zend_bitset.h"
#include "zend_cfg.h"

typedef struct _zend_dfg {
	int         vars;
	uint32_t    size;
	zend_bitset tmp;
	zend_bitset def;
	zend_bitset use;
	zend_bitset in;
	zend_bitset out;
} zend_dfg;

#define DFG_BITSET(set, set_size, block_num) \
	((set) + ((block_num) * (set_size)))

#define DFG_SET(set, set_size, block_num, var_num) \
	zend_bitset_incl(DFG_BITSET(set, set_size, block_num), (var_num))

#define DFG_ISSET(set, set_size, block_num, var_num) \
	zend_bitset_in(DFG_BITSET(set, set_size, block_num), (var_num))

BEGIN_EXTERN_C()

void zend_build_dfg(const zend_op_array *op_array, const zend_cfg *cfg, const zend_dfg *dfg, uint32_t build_flags);
ZEND_API void zend_dfg_add_use_def_op(const zend_op_array *op_array, const zend_op *opline, uint32_t build_flags, zend_bitset use, zend_bitset def);

END_EXTERN_C()

#endif /* ZEND_DFG_H */
