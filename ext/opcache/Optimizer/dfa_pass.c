/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_vm.h"
#include "zend_bitset.h"
#include "zend_cfg.h"
#include "zend_ssa.h"
#include "zend_func_info.h"
#include "zend_inference.h"
#include "zend_dump.h"

int zend_dfa_analyze_op_array(zend_op_array *op_array, zend_optimizer_ctx *ctx, zend_ssa *ssa, uint32_t *flags)
{
	uint32_t build_flags;

    /* Build SSA */
	memset(ssa, 0, sizeof(zend_ssa));

	if (zend_build_cfg(&ctx->arena, op_array, 0, &ssa->cfg, flags) != SUCCESS) {
		return FAILURE;
	}

	if (*flags & ZEND_FUNC_TOO_DYNAMIC) {
		return FAILURE;
	}

	if (zend_cfg_build_predecessors(&ctx->arena, &ssa->cfg) != SUCCESS) {
		return FAILURE;
	}

	if (ctx->debug_level & ZEND_DUMP_DFA_CFG) {
		zend_dump_op_array(op_array, ZEND_DUMP_CFG | ZEND_DUMP_HIDE_UNUSED_VARS, "dfa cfg", &ssa->cfg);
	}

	/* Compute Dominators Tree */
	if (zend_cfg_compute_dominators_tree(op_array, &ssa->cfg) != SUCCESS) {
		return FAILURE;
	}

	/* Identify reducible and irreducible loops */
	if (zend_cfg_identify_loops(op_array, &ssa->cfg, flags) != SUCCESS) {
		return FAILURE;
	}

	if (ctx->debug_level & ZEND_DUMP_DFA_DOMINATORS) {
		zend_dump_dominators(op_array, &ssa->cfg);
	}

	build_flags = 0;
	if (ctx->debug_level & ZEND_DUMP_DFA_LIVENESS) {
		build_flags |= ZEND_SSA_DEBUG_LIVENESS;
	}
	if (ctx->debug_level & ZEND_DUMP_DFA_PHI) {
		build_flags |= ZEND_SSA_DEBUG_PHI_PLACEMENT;
	}
	if (zend_build_ssa(&ctx->arena, op_array, build_flags, ssa, flags) != SUCCESS) {
		return FAILURE;
	}

	if (ctx->debug_level & ZEND_DUMP_DFA_SSA) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA | ZEND_DUMP_HIDE_UNUSED_VARS, "before dfa pass", ssa);
	}


	if (zend_ssa_compute_use_def_chains(&ctx->arena, op_array, ssa) != SUCCESS){
		return FAILURE;
	}

	if (zend_ssa_find_false_dependencies(op_array, ssa) != SUCCESS) {
		return FAILURE;
	}

	if (zend_ssa_find_sccs(op_array, ssa) != SUCCESS){
		return FAILURE;
	}

	if (zend_ssa_inference(&ctx->arena, op_array, ctx->script, ssa) != SUCCESS) {
		return FAILURE;
	}

	if (ctx->debug_level & ZEND_DUMP_DFA_SSA_VARS) {
		zend_dump_ssa_variables(op_array, ssa);
	}

	return SUCCESS;
}

void zend_dfa_optimize_op_array(zend_op_array *op_array, zend_optimizer_ctx *ctx, zend_ssa *ssa)
{
	if (ctx->debug_level & ZEND_DUMP_BEFORE_DFA_PASS) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA | ZEND_DUMP_HIDE_UNUSED_VARS, "before dfa pass", ssa);
	}

	if (ssa->var_info) {
		int i;
		int remove_nops = 0;

		// 1: #1.T  = OP_Y                                |  #3.CV = OP_Y
		// 2: ASSIGN #2.CV [undef,scalar] -> #3.CV, #1.T  |  NOP
		// --
		// 2: ASSIGN #2.CV [undef,scalar] -> #3.CV, C     |  3.CV = QM_ASSIGN C
		// --
		// 2: ASSIGN #2.CV [undef,scalar] -> #3.CV, #1.CV |  3.CV = QM_ASSIGN #1.CV

		for (i = 0; i < ssa->vars_count; i++) {
			int op2 = ssa->vars[i].definition;

			if (op2 >= 0
			 && op_array->opcodes[op2].opcode == ZEND_ASSIGN
			 && op_array->opcodes[op2].op1_type == IS_CV
			 && !RETURN_VALUE_USED(&op_array->opcodes[op2])
			) {
				int var2 = ssa->ops[op2].op1_use;

				if (var2 >= 0
				 && !(ssa->var_info[var2].type & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))
				) {

					if (op_array->opcodes[op2].op2_type & (IS_TMP_VAR|IS_VAR)) {
						int var1 = ssa->ops[op2].op2_use;

						if (var1 >= 0
						 && !(ssa->var_info[var1].type & MAY_BE_REF)
						 && ssa->vars[var1].definition >= 0
						 && ssa->ops[ssa->vars[var1].definition].result_def == var1
						 && ssa->ops[ssa->vars[var1].definition].result_use < 0
						 && ssa->vars[var1].use_chain == op2
						 && ssa->ops[op2].op2_use_chain < 0
						 && !ssa->vars[var1].phi_use_chain
						 && !ssa->vars[var1].sym_use_chain
						) {
							int op1 = ssa->vars[var1].definition;
							int var3 = i;

							if (zend_ssa_unlink_use_chain(ssa, op2, var2)) {
								/* Reconstruct SSA */
								ssa->vars[var3].definition = op1;
								ssa->ops[op1].result_def = var3;

								ssa->vars[var1].definition = -1;
								ssa->vars[var1].use_chain = -1;

								ssa->ops[op2].op1_use = -1;
								ssa->ops[op2].op2_use = -1;
								ssa->ops[op2].op1_def = -1;
								ssa->ops[op2].op1_use_chain = -1;

								/* Update opcodes */
								op_array->opcodes[op1].result_type = op_array->opcodes[op2].op1_type;
								op_array->opcodes[op1].result.var = op_array->opcodes[op2].op1.var;
								MAKE_NOP(&op_array->opcodes[op2]);
								remove_nops = 1;
							}
						}
					} else if (op_array->opcodes[op2].op2_type == IS_CONST
					 || (op_array->opcodes[op2].op2_type == IS_CV
					     && ssa->ops[op2].op2_use >= 0
					     && ssa->ops[op2].op2_def < 0
					     && !(ssa->var_info[ssa->ops[op2].op2_use].type & MAY_BE_REF))
					) {
						int var3 = i;

						if (zend_ssa_unlink_use_chain(ssa, op2, var2)) {
							/* Reconstruct SSA */
							ssa->ops[op2].result_def = var3;
							ssa->ops[op2].op1_def = -1;
							ssa->ops[op2].op1_use = ssa->ops[op2].op2_use;
							ssa->ops[op2].op1_use_chain = ssa->ops[op2].op2_use_chain;
							ssa->ops[op2].op2_use = -1;
							ssa->ops[op2].op2_use_chain = -1;

							/* Update opcode */
							op_array->opcodes[op2].result_type = op_array->opcodes[op2].op1_type;
							op_array->opcodes[op2].result.var = op_array->opcodes[op2].op1.var;
							op_array->opcodes[op2].op1_type = op_array->opcodes[op2].op2_type;
							op_array->opcodes[op2].op1.var = op_array->opcodes[op2].op2.var;
							op_array->opcodes[op2].op2_type = IS_UNUSED;
							op_array->opcodes[op2].op2.var = 0;
							op_array->opcodes[op2].opcode = ZEND_QM_ASSIGN;
						}
					}
				}
			}
		}
		if (remove_nops) {
			// TODO: remove nop???
		}
	}

	if (ctx->debug_level & ZEND_DUMP_AFTER_DFA_PASS) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA | ZEND_DUMP_HIDE_UNUSED_VARS, "after dfa pass", ssa);
	}
}

void optimize_dfa(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	void *checkpoint = zend_arena_checkpoint(ctx->arena);
	uint32_t flags = 0;
	zend_ssa ssa;

	if (zend_dfa_analyze_op_array(op_array, ctx, &ssa, &flags) != SUCCESS) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	zend_dfa_optimize_op_array(op_array, ctx, &ssa);

	/* Destroy SSA */
	zend_arena_release(&ctx->arena, checkpoint);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
