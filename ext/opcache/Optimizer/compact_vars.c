/*
   +----------------------------------------------------------------------+
   | Zend Engine, Removing unused variables                               |
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
   | Authors: Nikita Popov <nikic@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_bitset.h"

/* This pass removes all CVs and temporaries that are completely unused. It does *not* merge any CVs or TMPs.
 * This pass does not operate on SSA form anymore. */
void zend_optimizer_compact_vars(zend_op_array *op_array) {
	int i;

	ALLOCA_FLAG(use_heap1);
	ALLOCA_FLAG(use_heap2);
	uint32_t used_vars_len = zend_bitset_len(op_array->last_var + op_array->T);
	zend_bitset used_vars = ZEND_BITSET_ALLOCA(used_vars_len, use_heap1);
	uint32_t *vars_map = do_alloca((op_array->last_var + op_array->T) * sizeof(uint32_t), use_heap2);
	uint32_t num_cvs, num_tmps;

	/* Determine which CVs are used */
	zend_bitset_clear(used_vars, used_vars_len);
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		if (opline->op1_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
			zend_bitset_incl(used_vars, VAR_NUM(opline->op1.var));
		}
		if (opline->op2_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
			zend_bitset_incl(used_vars, VAR_NUM(opline->op2.var));
		}
		if (opline->result_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
			zend_bitset_incl(used_vars, VAR_NUM(opline->result.var));
			if (opline->opcode == ZEND_ROPE_INIT) {
				uint32_t num = ((opline->extended_value * sizeof(zend_string*)) + (sizeof(zval) - 1)) / sizeof(zval);
				while (num > 1) {
					num--;
					zend_bitset_incl(used_vars, VAR_NUM(opline->result.var) + num);
				}
			}
		}
	}

	num_cvs = 0;
	for (i = 0; i < op_array->last_var; i++) {
		if (zend_bitset_in(used_vars, i)) {
			vars_map[i] = num_cvs++;
		} else {
			vars_map[i] = (uint32_t) -1;
		}
	}

	num_tmps = 0;
	for (i = op_array->last_var; i < op_array->last_var + op_array->T; i++) {
		if (zend_bitset_in(used_vars, i)) {
			vars_map[i] = num_cvs + num_tmps++;
		} else {
			vars_map[i] = (uint32_t) -1;
		}
	}

	free_alloca(used_vars, use_heap1);
	if (num_cvs == op_array->last_var && num_tmps == op_array->T) {
		free_alloca(vars_map, use_heap2);
		return;
	}

	ZEND_ASSERT(num_cvs <= op_array->last_var);
	ZEND_ASSERT(num_tmps <= op_array->T);

	/* Update CV and TMP references in opcodes */
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		if (opline->op1_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
			opline->op1.var = NUM_VAR(vars_map[VAR_NUM(opline->op1.var)]);
		}
		if (opline->op2_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
			opline->op2.var = NUM_VAR(vars_map[VAR_NUM(opline->op2.var)]);
		}
		if (opline->result_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
			opline->result.var = NUM_VAR(vars_map[VAR_NUM(opline->result.var)]);
		}
	}

	/* Update TMP references in live ranges */
	if (op_array->live_range) {
		for (i = 0; i < op_array->last_live_range; i++) {
			op_array->live_range[i].var =
				(op_array->live_range[i].var & ZEND_LIVE_MASK) |
				NUM_VAR(vars_map[VAR_NUM(op_array->live_range[i].var & ~ZEND_LIVE_MASK)]);
		}
	}

	/* Update CV name table */
	if (num_cvs != op_array->last_var) {
		if (num_cvs) {
			zend_string **names = safe_emalloc(sizeof(zend_string *), num_cvs, 0);
			for (i = 0; i < op_array->last_var; i++) {
				if (vars_map[i] != (uint32_t) -1) {
					names[vars_map[i]] = op_array->vars[i];
				} else {
					zend_string_release_ex(op_array->vars[i], 0);
				}
			}
			efree(op_array->vars);
			op_array->vars = names;
		} else {
			for (i = 0; i < op_array->last_var; i++) {
				zend_string_release_ex(op_array->vars[i], 0);
			}
			efree(op_array->vars);
			op_array->vars = NULL;
		}
		op_array->last_var = num_cvs;
	}

	op_array->T = num_tmps;

	free_alloca(vars_map, use_heap2);
}
