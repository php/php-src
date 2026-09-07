/*
   +----------------------------------------------------------------------+
   | Zend Engine, Removing unused variables                               |
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Nikita Popov <nikic@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "Optimizer/zend_optimizer_internal.h"
#include "zend_bitset.h"
#include "zend_observer.h"

/* Types a CV may have, excluding array kinds. */
#define CV_TYPE_KINDS (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)
/* Type kinds that are not refcounted. */
#define CV_TYPE_KINDS_NO_RC (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_BOOL|MAY_BE_LONG|MAY_BE_DOUBLE)

/* Fills in `rc_cvs` with the CV numbers that may be refcounted, based on SSA info (if available). */
static void compute_rc_cvs(
		const zend_op_array *op_array, const zend_ssa *ssa, zend_bitset rc_cvs)
{
	if (!ssa || !ssa->var_info) {
		/* No type information: every CV has to be assumed refcounted. */
		zend_bitset_fill(rc_cvs, zend_bitset_len(op_array->last_var));
		return;
	}

	zend_bitset_clear(rc_cvs, zend_bitset_len(op_array->last_var));

	/* TODO: Argument slots right now have to be always destroyed because
	 * when ZEND_RECV fails, they can be refcounted (e.g. passing an array to an int parameter).
	 * This can be improved by making the ZEND_RECV op destroy the argument iself. */
	for (uint32_t i = 0; i < MIN((uint32_t) op_array->last_var, op_array->num_args); i++) {
		zend_bitset_incl(rc_cvs, i);
	}
	for (int i = 0; i < ssa->vars_count; i++) {
		int cv = ssa->vars[i].var;

		if (cv < 0 || cv >= op_array->last_var || zend_bitset_in(rc_cvs, cv)) {
			continue;
		}
		if (ssa->var_info[i].type & CV_TYPE_KINDS & ~CV_TYPE_KINDS_NO_RC) {
			zend_bitset_incl(rc_cvs, cv);
		}
	}

	/* ZEND_RETURN_BY_REF turns its op1 into a reference without being recorded as
	 * such in the SSA. */
	/* TODO: should this be recorded by type inference and SSA construction via a new def? */
	if (op_array->fn_flags & ZEND_ACC_RETURN_REFERENCE) {
		for (uint32_t i = 0; i < op_array->last; i++) {
			const zend_op *opline = &op_array->opcodes[i];

			if (opline->opcode == ZEND_RETURN_BY_REF && opline->op1_type == IS_CV) {
				zend_bitset_incl(rc_cvs, EX_VAR_TO_NUM(opline->op1.var));
			}
		}
	}
}

/* This pass removes all CVs and temporaries that are completely unused. It does *not* merge any CVs or TMPs.
 *
 * It also sorts the refcounted CVs so they appear at the front, to fill in `op_array->last_var_to_free`.
 * This improves call frame cleanup performance by skipping (in bulk) the CVs that are not refcounted. */
void zend_optimizer_compact_vars(zend_op_array *op_array, const zend_ssa *ssa) {
	int i;

	ALLOCA_FLAG(use_heap1);
	ALLOCA_FLAG(use_heap2);
	ALLOCA_FLAG(use_heap3);
	uint32_t used_vars_len = zend_bitset_len(op_array->last_var + op_array->T);
	zend_bitset used_vars = ZEND_BITSET_ALLOCA(used_vars_len, use_heap1);
	uint32_t *vars_map = do_alloca((op_array->last_var + op_array->T) * sizeof(uint32_t), use_heap2);
	uint32_t rc_cvs_len = zend_bitset_len(op_array->last_var);
	zend_bitset rc_cvs = ZEND_BITSET_ALLOCA(rc_cvs_len, use_heap3);

	/* Determine which CVs are used */
	zend_bitset_clear(used_vars, used_vars_len);
	for (i = 0; i < op_array->last; i++) {
		const zend_op *opline = &op_array->opcodes[i];
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

	compute_rc_cvs(op_array, ssa, rc_cvs);

	uint32_t num_cvs = 0;
	uint32_t last_var_to_free = 0;
	for (i = 0; i < op_array->last_var; i++) {
		/* Parameters have a fixed position because other components depend on that ordering. */
		if (i < op_array->num_args) {
			ZEND_ASSERT(zend_bitset_in(used_vars, i)
				&& "A parameter CV is written by its RECV and cannot be unused");
		} else if (!zend_bitset_in(used_vars, i) || !zend_bitset_in(rc_cvs, i)) {
			vars_map[i] = (uint32_t) -1;
			continue;
		}
		vars_map[i] = num_cvs++;
		if (zend_bitset_in(rc_cvs, i)) {
			/* Parameters keep their slot even when they are not refcounted,
			 * so take the highest refcounted slot rather than the group size. */
			last_var_to_free = vars_map[i] + 1;
		}
	}

	/* The CVs that can never hold a refcounted value go last, after `last_var_to_free`. */
	for (i = op_array->num_args; i < op_array->last_var; i++) {
		if (vars_map[i] == (uint32_t) -1 && zend_bitset_in(used_vars, i)) {
			vars_map[i] = num_cvs++;
		}
	}
	ZEND_ASSERT(last_var_to_free <= num_cvs);

	uint32_t num_tmps = 0;
	for (i = op_array->last_var; i < op_array->last_var + op_array->T; i++) {
		if (zend_bitset_in(used_vars, i)) {
			vars_map[i] = num_cvs + num_tmps++;
		} else {
			vars_map[i] = (uint32_t) -1;
		}
	}

	free_alloca(used_vars, use_heap1);
	free_alloca(rc_cvs, use_heap3);

	op_array->last_var_to_free = last_var_to_free;

	/* Nothing was removed and no CV moved: the rewrite below would be a no-op.
	 * Check if anything got moved, as the number of vars could still be the same in that case. */
	if (num_cvs == op_array->last_var && num_tmps == op_array->T) {
		bool identity = true;
		for (i = 0; i < op_array->last_var; i++) {
			if (vars_map[i] != (uint32_t) i) {
				identity = false;
				break;
			}
		}
		if (identity) {
			free_alloca(vars_map, use_heap2);
			return;
		}
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

	/* Update CV name table, either because of CVs being removed or being moved. */
	{
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

	op_array->T = num_tmps + ZEND_OBSERVER_ENABLED; // reserve last temporary for observers if enabled

	free_alloca(vars_map, use_heap2);
}
