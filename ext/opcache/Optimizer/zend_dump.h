/*
   +----------------------------------------------------------------------+
   | Zend Engine, Bytecode Visualisation                                  |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_DUMP_H
#define ZEND_DUMP_H

#include "zend_ssa.h"
#include "zend_dfg.h"

#define ZEND_DUMP_HIDE_UNREACHABLE     (1<<0)
#define ZEND_DUMP_RC_INFERENCE         (1<<1)
#define ZEND_DUMP_CFG                  (1<<2)
#define ZEND_DUMP_SSA                  (1<<3)
#define ZEND_DUMP_RT_CONSTANTS         ZEND_RT_CONSTANTS

BEGIN_EXTERN_C()

void zend_dump_op_array(const zend_op_array *op_array, uint32_t dump_flags, const char *msg, const void *data);
void zend_dump_dominators(const zend_op_array *op_array, const zend_cfg *cfg);
void zend_dump_dfg(const zend_op_array *op_array, const zend_cfg *cfg, const zend_dfg *dfg);
void zend_dump_phi_placement(const zend_op_array *op_array, const zend_ssa *ssa);
void zend_dump_variables(const zend_op_array *op_array);
void zend_dump_ssa_variables(const zend_op_array *op_array, const zend_ssa *ssa, uint32_t dump_flags);
void zend_dump_var(const zend_op_array *op_array, zend_uchar var_type, int var_num);
void zend_dump_op_array_name(const zend_op_array *op_array);
void zend_dump_const(const zval *zv);
void zend_dump_ht(HashTable *ht);

END_EXTERN_C()

#endif /* ZEND_DUMP_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
