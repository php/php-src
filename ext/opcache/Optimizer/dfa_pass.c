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
#include "zend_call_graph.h"
#include "zend_inference.h"
#include "zend_dump.h"

int zend_dfa_analyze_op_array(zend_op_array *op_array, zend_optimizer_ctx *ctx, zend_ssa *ssa, uint32_t *flags)
{
	uint32_t build_flags;

	if (op_array->last_try_catch) {
		/* TODO: we can't analyze functions with try/catch/finally ??? */
		return FAILURE;
	}

    /* Build SSA */
	memset(ssa, 0, sizeof(zend_ssa));

	if (zend_build_cfg(&ctx->arena, op_array, 0, &ssa->cfg, flags) != SUCCESS) {
		return FAILURE;
	}

	if (*flags & ZEND_FUNC_INDIRECT_VAR_ACCESS) {
		/* TODO: we can't analyze functions with indirect variable access ??? */
		return FAILURE;
	}

	if (zend_cfg_build_predecessors(&ctx->arena, &ssa->cfg) != SUCCESS) {
		return FAILURE;
	}

	if (ctx->debug_level & ZEND_DUMP_DFA_CFG) {
		zend_dump_op_array(op_array, ZEND_DUMP_CFG, "dfa cfg", &ssa->cfg);
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
		zend_dump_op_array(op_array, ZEND_DUMP_SSA, "before dfa pass", ssa);
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
		zend_dump_ssa_variables(op_array, ssa, 0);
	}

	return SUCCESS;
}

static void zend_ssa_remove_nops(zend_op_array *op_array, zend_ssa *ssa)
{
	zend_basic_block *blocks = ssa->cfg.blocks;
	zend_basic_block *end = blocks + ssa->cfg.blocks_count;
	zend_basic_block *b;
	zend_func_info *func_info;
	int j;
	uint32_t i;
	uint32_t target = 0;
	uint32_t *shiftlist;
	ALLOCA_FLAG(use_heap);

	shiftlist = (uint32_t *)do_alloca(sizeof(uint32_t) * op_array->last, use_heap);
	memset(shiftlist, 0, sizeof(uint32_t) * op_array->last);
	for (b = blocks; b < end; b++) {
		if (b->flags & ZEND_BB_REACHABLE) {
			i = b->start;
			b->start = target;
			while (i <= b->end) {
				if (EXPECTED(op_array->opcodes[i].opcode != ZEND_NOP) ||
				   /*keep NOP to support ZEND_VM_SMART_BRANCH */
				   (i > 0 &&
				    i + 1 < op_array->last &&
				    (op_array->opcodes[i+1].opcode == ZEND_JMPZ ||
				     op_array->opcodes[i+1].opcode == ZEND_JMPNZ) &&
				    (op_array->opcodes[i-1].opcode == ZEND_IS_IDENTICAL ||
				     op_array->opcodes[i-1].opcode == ZEND_IS_NOT_IDENTICAL ||
				     op_array->opcodes[i-1].opcode == ZEND_IS_EQUAL ||
				     op_array->opcodes[i-1].opcode == ZEND_IS_NOT_EQUAL ||
				     op_array->opcodes[i-1].opcode == ZEND_IS_SMALLER ||
				     op_array->opcodes[i-1].opcode == ZEND_IS_SMALLER_OR_EQUAL ||
				     op_array->opcodes[i-1].opcode == ZEND_CASE ||
				     op_array->opcodes[i-1].opcode == ZEND_ISSET_ISEMPTY_VAR ||
				     op_array->opcodes[i-1].opcode == ZEND_ISSET_ISEMPTY_STATIC_PROP ||
				     op_array->opcodes[i-1].opcode == ZEND_ISSET_ISEMPTY_DIM_OBJ ||
				     op_array->opcodes[i-1].opcode == ZEND_ISSET_ISEMPTY_PROP_OBJ ||
				     op_array->opcodes[i-1].opcode == ZEND_INSTANCEOF ||
				     op_array->opcodes[i-1].opcode == ZEND_TYPE_CHECK ||
				     op_array->opcodes[i-1].opcode == ZEND_DEFINED))) {
					if (i != target) {
						op_array->opcodes[target] = op_array->opcodes[i];
						ssa->ops[target] = ssa->ops[i];
						shiftlist[i] = i - target;
					}
					target++;
				}
				i++;
			}
			if (b->end != target - 1) {
				zend_op *opline;
				zend_op *new_opline;

				opline = op_array->opcodes + b->end;
				b->end = target - 1;
				new_opline = op_array->opcodes + b->end;
				switch (new_opline->opcode) {
					case ZEND_JMP:
					case ZEND_FAST_CALL:
						ZEND_SET_OP_JMP_ADDR(new_opline, new_opline->op1, ZEND_OP1_JMP_ADDR(opline));
						break;
					case ZEND_JMPZNZ:
						new_opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, new_opline, ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value));
						/* break missing intentionally */
					case ZEND_JMPZ:
					case ZEND_JMPNZ:
					case ZEND_JMPZ_EX:
					case ZEND_JMPNZ_EX:
					case ZEND_FE_RESET_R:
					case ZEND_FE_RESET_RW:
					case ZEND_JMP_SET:
					case ZEND_COALESCE:
					case ZEND_ASSERT_CHECK:
						ZEND_SET_OP_JMP_ADDR(new_opline, new_opline->op2, ZEND_OP2_JMP_ADDR(opline));
						break;
					case ZEND_CATCH:
						if (!opline->result.num) {
							new_opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, new_opline, ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value));
						}
						break;
					case ZEND_DECLARE_ANON_CLASS:
					case ZEND_DECLARE_ANON_INHERITED_CLASS:
					case ZEND_FE_FETCH_R:
					case ZEND_FE_FETCH_RW:
						new_opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, new_opline, ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value));
						break;
				}
			}
		}
	}

	if (target != op_array->last) {
		/* reset rest opcodes */
		for (i = target; i < op_array->last; i++) {
			MAKE_NOP(op_array->opcodes + i);
		}

		/* update SSA variables */
		for (j = 0; j < ssa->vars_count; j++) {
			if (ssa->vars[j].definition >= 0) {
				ssa->vars[j].definition -= shiftlist[ssa->vars[j].definition];
			}
			if (ssa->vars[j].use_chain >= 0) {
				ssa->vars[j].use_chain -= shiftlist[ssa->vars[j].use_chain];
			}
		}
		for (i = 0; i < op_array->last; i++) {
			if (ssa->ops[i].op1_use_chain >= 0) {
				ssa->ops[i].op1_use_chain -= shiftlist[ssa->ops[i].op1_use_chain];
			}
			if (ssa->ops[i].op2_use_chain >= 0) {
				ssa->ops[i].op2_use_chain -= shiftlist[ssa->ops[i].op2_use_chain];
			}
			if (ssa->ops[i].res_use_chain >= 0) {
				ssa->ops[i].res_use_chain -= shiftlist[ssa->ops[i].res_use_chain];
			}
		}

		/* update branch targets */
		for (b = blocks; b < end; b++) {
			if (b->flags & ZEND_BB_REACHABLE) {
				zend_op *opline = op_array->opcodes + b->end;

				switch (opline->opcode) {
					case ZEND_JMP:
					case ZEND_FAST_CALL:
						ZEND_SET_OP_JMP_ADDR(opline, opline->op1, ZEND_OP1_JMP_ADDR(opline) - shiftlist[ZEND_OP1_JMP_ADDR(opline) - op_array->opcodes]);
						break;
					case ZEND_JMPZNZ:
						opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value) - shiftlist[ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value)]);
						/* break missing intentionally */
					case ZEND_JMPZ:
					case ZEND_JMPNZ:
					case ZEND_JMPZ_EX:
					case ZEND_JMPNZ_EX:
					case ZEND_FE_RESET_R:
					case ZEND_FE_RESET_RW:
					case ZEND_JMP_SET:
					case ZEND_COALESCE:
					case ZEND_ASSERT_CHECK:
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, ZEND_OP2_JMP_ADDR(opline) - shiftlist[ZEND_OP2_JMP_ADDR(opline) - op_array->opcodes]);
						break;
					case ZEND_DECLARE_ANON_CLASS:
					case ZEND_DECLARE_ANON_INHERITED_CLASS:
					case ZEND_FE_FETCH_R:
					case ZEND_FE_FETCH_RW:
					case ZEND_CATCH:
						opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value) - shiftlist[ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value)]);
						break;
				}
			}
		}

		/* update brk/cont array */
		for (j = 0; j < op_array->last_live_range; j++) {
			op_array->live_range[j].start -= shiftlist[op_array->live_range[j].start];
			op_array->live_range[j].end   -= shiftlist[op_array->live_range[j].end];
		}

		/* update try/catch array */
		for (j = 0; j < op_array->last_try_catch; j++) {
			op_array->try_catch_array[j].try_op -= shiftlist[op_array->try_catch_array[j].try_op];
			op_array->try_catch_array[j].catch_op -= shiftlist[op_array->try_catch_array[j].catch_op];
			if (op_array->try_catch_array[j].finally_op) {
				op_array->try_catch_array[j].finally_op -= shiftlist[op_array->try_catch_array[j].finally_op];
				op_array->try_catch_array[j].finally_end -= shiftlist[op_array->try_catch_array[j].finally_end];
			}
		}

		/* update early binding list */
		if (op_array->early_binding != (uint32_t)-1) {
			uint32_t *opline_num = &op_array->early_binding;

			do {
				*opline_num -= shiftlist[*opline_num];
				opline_num = &ZEND_RESULT(&op_array->opcodes[*opline_num]).opline_num;
			} while (*opline_num != (uint32_t)-1);
		}

		/* update call graph */
		func_info = ZEND_FUNC_INFO(op_array);
		if (func_info) {
			zend_call_info *call_info = func_info->callee_info;
			while (call_info) {
				call_info->caller_init_opline -=
					shiftlist[call_info->caller_init_opline - op_array->opcodes];
				call_info->caller_call_opline -=
					shiftlist[call_info->caller_call_opline - op_array->opcodes];
				call_info = call_info->next_callee;
			}
		}

		op_array->last = target;
	}
	free_alloca(shiftlist, use_heap);
}

void zend_dfa_optimize_op_array(zend_op_array *op_array, zend_optimizer_ctx *ctx, zend_ssa *ssa)
{
	if (ctx->debug_level & ZEND_DUMP_BEFORE_DFA_PASS) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA, "before dfa pass", ssa);
	}

	if (ssa->var_info) {
		int i;
		int remove_nops = 0;

		// 1: #1.T  = OP_Y                                |  #3.CV = OP_Y
		// 2: ASSIGN #2.CV [undef,scalar] -> #3.CV, #1.T  |  NOP
		// --
		// 2: ASSIGN #2.CV [undef,scalar] -> #3.CV, X     |  3.CV = QM_ASSIGN X

		for (i = 0; i < ssa->vars_count; i++) {
			int op2 = ssa->vars[i].definition;

			if (op2 < 0) {
				continue;
			}

			if (op_array->opcodes[op2].opcode == ZEND_ASSIGN
			 && op_array->opcodes[op2].op1_type == IS_CV
			 && !RETURN_VALUE_USED(&op_array->opcodes[op2])
			) {
				int var2 = ssa->ops[op2].op1_use;

				if (var2 >= 0
				 && !(ssa->var_info[var2].type & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))
				) {

					if ((op_array->opcodes[op2].op2_type & (IS_TMP_VAR|IS_VAR))
					 && ssa->ops[op2].op2_use >= 0
					 && !(ssa->var_info[ssa->ops[op2].op2_use].type & MAY_BE_REF)
					 && ssa->vars[ssa->ops[op2].op2_use].definition >= 0
					 && ssa->ops[ssa->vars[ssa->ops[op2].op2_use].definition].result_def == ssa->ops[op2].op2_use
					 && ssa->ops[ssa->vars[ssa->ops[op2].op2_use].definition].result_use < 0
					 && ssa->vars[ssa->ops[op2].op2_use].use_chain == op2
					 && ssa->ops[op2].op2_use_chain < 0
					 && !ssa->vars[ssa->ops[op2].op2_use].phi_use_chain
					 && !ssa->vars[ssa->ops[op2].op2_use].sym_use_chain
					 /* see Zend/tests/generators/aborted_yield_during_new.phpt */
					 && op_array->opcodes[ssa->vars[ssa->ops[op2].op2_use].definition].opcode != ZEND_NEW
					) {
						int var1 = ssa->ops[op2].op2_use;
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
					} else if (op_array->opcodes[op2].op2_type == IS_CONST
					 || ((op_array->opcodes[op2].op2_type & (IS_TMP_VAR|IS_VAR|IS_CV))
					     && ssa->ops[op2].op2_use >= 0
					     && ssa->ops[op2].op2_def < 0)
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

			} else if (op_array->opcodes[op2].opcode == ZEND_ASSIGN_ADD
			 && op_array->opcodes[op2].extended_value == 0
			 && ssa->ops[op2].op1_def == i
			 && op_array->opcodes[op2].op2_type == IS_CONST
			 && Z_TYPE_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op2].op2.constant)) == IS_LONG
			 && Z_LVAL_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op2].op2.constant)) == 1
			 && ssa->ops[op2].op1_use >= 0
			 && !(ssa->var_info[ssa->ops[op2].op1_use].type & (MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {

				op_array->opcodes[op2].opcode = ZEND_PRE_INC;
				SET_UNUSED(op_array->opcodes[op2].op2);

			} else if (op_array->opcodes[op2].opcode == ZEND_ASSIGN_SUB
			 && op_array->opcodes[op2].extended_value == 0
			 && ssa->ops[op2].op1_def == i
			 && op_array->opcodes[op2].op2_type == IS_CONST
			 && Z_TYPE_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op2].op2.constant)) == IS_LONG
			 && Z_LVAL_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op2].op2.constant)) == 1
			 && ssa->ops[op2].op1_use >= 0
			 && !(ssa->var_info[ssa->ops[op2].op1_use].type & (MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {

				op_array->opcodes[op2].opcode = ZEND_PRE_DEC;
				SET_UNUSED(op_array->opcodes[op2].op2);

			} else if (ssa->ops[op2].op1_def == i
			 && !RETURN_VALUE_USED(&op_array->opcodes[op2])
			 && ssa->ops[op2].op1_use >= 0
			 && !(ssa->var_info[ssa->ops[op2].op1_use].type & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))
			 && (op_array->opcodes[op2].opcode == ZEND_ASSIGN_ADD
			  || op_array->opcodes[op2].opcode == ZEND_ASSIGN_SUB
			  || op_array->opcodes[op2].opcode == ZEND_ASSIGN_MUL
			  || op_array->opcodes[op2].opcode == ZEND_ASSIGN_DIV
			  || op_array->opcodes[op2].opcode == ZEND_ASSIGN_MOD
			  || op_array->opcodes[op2].opcode == ZEND_ASSIGN_SL
			  || op_array->opcodes[op2].opcode == ZEND_ASSIGN_SR
			  || op_array->opcodes[op2].opcode == ZEND_ASSIGN_BW_OR
			  || op_array->opcodes[op2].opcode == ZEND_ASSIGN_BW_AND
			  || op_array->opcodes[op2].opcode == ZEND_ASSIGN_BW_XOR)
			 && op_array->opcodes[op2].extended_value == 0) {

				/* Reconstruct SSA */
				ssa->ops[op2].result_def = ssa->ops[op2].op1_def;
				ssa->ops[op2].op1_def = -1;

				/* Update opcode */
				op_array->opcodes[op2].opcode -= (ZEND_ASSIGN_ADD - ZEND_ADD);
				op_array->opcodes[op2].result_type = op_array->opcodes[op2].op1_type;
				op_array->opcodes[op2].result.var = op_array->opcodes[op2].op1.var;

			}
		}
		if (remove_nops) {
			zend_ssa_remove_nops(op_array, ssa);
		}
	}

	if (ssa->var_info) {
		int i;
		zend_op *opline;
		zval tmp;

		/* Convert LONG constants to DOUBLE */
		for (i = 0; i < ssa->vars_count; i++) {
			if (ssa->vars[i].definition >= 0) {
				int op = ssa->vars[i].definition;

				opline = op_array->opcodes + op;
				if (ssa->var_info[i].use_as_double) {
					if (opline->opcode == ZEND_ASSIGN &&
				    	opline->op2_type == IS_CONST) {
						zval *zv = CT_CONSTANT_EX(op_array, opline->op2.constant);
						ZEND_ASSERT(Z_TYPE_INFO_P(zv) == IS_LONG);
						ZVAL_DOUBLE(&tmp, zval_get_double(zv));
						opline->op2.constant = zend_optimizer_add_literal(op_array, &tmp);
					} else if (opline->opcode == ZEND_QM_ASSIGN &&
					           opline->op1_type == IS_CONST) {
						zval *zv = CT_CONSTANT_EX(op_array, opline->op1.constant);
						ZEND_ASSERT(Z_TYPE_INFO_P(zv) == IS_LONG);
						ZVAL_DOUBLE(&tmp, zval_get_double(zv));
						opline->op1.constant = zend_optimizer_add_literal(op_array, &tmp);
					}
				} else {
					if (opline->opcode == ZEND_ADD ||
					    opline->opcode == ZEND_SUB ||
					    opline->opcode == ZEND_MUL ||
					    opline->opcode == ZEND_IS_EQUAL ||
					    opline->opcode == ZEND_IS_NOT_EQUAL ||
					    opline->opcode == ZEND_IS_SMALLER ||
					    opline->opcode == ZEND_IS_SMALLER_OR_EQUAL) {
						if (opline->op1_type == IS_CONST &&
						    opline->op2_type != IS_CONST &&
						    (OP2_INFO() & MAY_BE_ANY) == MAY_BE_DOUBLE &&
						    Z_TYPE_INFO_P(CT_CONSTANT_EX(op_array, opline->op1.constant)) == IS_LONG) {
							zval *zv = CT_CONSTANT_EX(op_array, opline->op1.constant);
							ZVAL_DOUBLE(&tmp, zval_get_double(zv));
							opline->op1.constant = zend_optimizer_add_literal(op_array, &tmp);
						} else if (opline->op1_type != IS_CONST &&
						           opline->op2_type == IS_CONST &&
						           (OP1_INFO() & MAY_BE_ANY) == MAY_BE_DOUBLE &&
						           Z_TYPE_INFO_P(CT_CONSTANT_EX(op_array, opline->op2.constant)) == IS_LONG) {
							zval *zv = CT_CONSTANT_EX(op_array, opline->op2.constant);
							ZVAL_DOUBLE(&tmp, zval_get_double(zv));
							opline->op2.constant = zend_optimizer_add_literal(op_array, &tmp);
						}

					}
				}
			}
		}
	}

	if (ctx->debug_level & ZEND_DUMP_AFTER_DFA_PASS) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA, "after dfa pass", ssa);
	}
}

void zend_optimize_dfa(zend_op_array *op_array, zend_optimizer_ctx *ctx)
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
