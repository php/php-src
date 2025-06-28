/*
   +----------------------------------------------------------------------+
   | Zend Engine, Bytecode Visualisation                                  |
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

#ifndef ZEND_DUMP_H
#define ZEND_DUMP_H

#include "zend_ssa.h"
#include "zend_dfg.h"

#include <stdint.h>

#define ZEND_DUMP_HIDE_UNREACHABLE     (1<<0)
#define ZEND_DUMP_RC_INFERENCE         (1<<1)
#define ZEND_DUMP_CFG                  (1<<2)
#define ZEND_DUMP_SSA                  (1<<3)
#define ZEND_DUMP_LIVE_RANGES          (1<<4)
#define ZEND_DUMP_LINE_NUMBERS         (1<<5)

BEGIN_EXTERN_C()

ZEND_API void zend_dump_op_array(const zend_op_array *op_array, uint32_t dump_flags, const char *msg, const void *data);
ZEND_API void zend_dump_op(const zend_op_array *op_array, const zend_basic_block *b, const zend_op *opline, uint32_t dump_flags, const zend_ssa *ssa, const zend_ssa_op *ssa_op);
ZEND_API void zend_dump_op_line(const zend_op_array *op_array, const zend_basic_block *b, const zend_op *opline, uint32_t dump_flags, const void *data);
void zend_dump_dominators(const zend_op_array *op_array, const zend_cfg *cfg);
void zend_dump_dfg(const zend_op_array *op_array, const zend_cfg *cfg, const zend_dfg *dfg);
void zend_dump_phi_placement(const zend_op_array *op_array, const zend_ssa *ssa);
void zend_dump_ssa_variables(const zend_op_array *op_array, const zend_ssa *ssa, uint32_t dump_flags);
ZEND_API void zend_dump_ssa_var(const zend_op_array *op_array, const zend_ssa *ssa, int ssa_var_num, uint8_t var_type, uint32_t var_num, uint32_t dump_flags);
ZEND_API void zend_dump_var(const zend_op_array *op_array, uint8_t var_type, uint32_t var_num);
void zend_dump_op_array_name(const zend_op_array *op_array);
void zend_dump_const(const zval *zv);
void zend_dump_ht(HashTable *ht);

END_EXTERN_C()

#endif /* ZEND_DUMP_H */
