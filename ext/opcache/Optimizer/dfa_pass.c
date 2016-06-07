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

	if (zend_build_cfg(&ctx->arena, op_array,
			ZEND_CFG_NO_ENTRY_PREDECESSORS, &ssa->cfg, flags) != SUCCESS) {
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
	if (zend_build_ssa(&ctx->arena, ctx->script, op_array, build_flags, ssa, flags) != SUCCESS) {
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
		if (b->flags & (ZEND_BB_REACHABLE|ZEND_BB_UNREACHABLE_FREE)) {
			uint32_t end;
			if (b->flags & ZEND_BB_UNREACHABLE_FREE) {
				/* Only keep the FREE for the loop var */
				ZEND_ASSERT(op_array->opcodes[b->start].opcode == ZEND_FREE
						|| op_array->opcodes[b->start].opcode == ZEND_FE_FREE);
				b->len = 1;
			}

			end = b->start + b->len;
			i = b->start;
			b->start = target;
			while (i < end) {
				shiftlist[i] = i - target;
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
					}
					target++;
				}
				i++;
			}
			if (target != end && b->len != 0) {
				zend_op *opline;
				zend_op *new_opline;

				opline = op_array->opcodes + end - 1;
				b->len = target - b->start;
				new_opline = op_array->opcodes + target - 1;
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
			if ((b->flags & ZEND_BB_REACHABLE) && b->len != 0) {
				zend_op *opline = op_array->opcodes + b->start + b->len - 1;

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

static inline zend_bool can_elide_return_type_check(
		zend_op_array *op_array, zend_ssa *ssa, zend_ssa_op *ssa_op) {
	zend_arg_info *info = &op_array->arg_info[-1];
	zend_ssa_var_info *use_info = &ssa->var_info[ssa_op->op1_use];
	zend_ssa_var_info *def_info = &ssa->var_info[ssa_op->op1_def];

	/* A type is possible that is not in the allowed types */
	if ((use_info->type & (MAY_BE_ANY|MAY_BE_UNDEF)) & ~(def_info->type & MAY_BE_ANY)) {
		return 0;
	}

	if (info->type_hint == IS_CALLABLE) {
		return 0;
	}

	if (info->class_name) {
		if (!use_info->ce || !def_info->ce || !instanceof_function(use_info->ce, def_info->ce)) {
			return 0;
		}
	}

	return 1;
}

void zend_dfa_optimize_op_array(zend_op_array *op_array, zend_optimizer_ctx *ctx, zend_ssa *ssa)
{
	if (ctx->debug_level & ZEND_DUMP_BEFORE_DFA_PASS) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA, "before dfa pass", ssa);
	}

	if (ssa->var_info) {
		int op_1;
		int v;
		int remove_nops = 0;
		zend_op *opline;
		zval tmp;

		for (v = op_array->last_var; v < ssa->vars_count; v++) {

			op_1 = ssa->vars[v].definition;

			if (op_1 < 0) {
				continue;
			}

			opline = op_array->opcodes + op_1;

			/* Convert LONG constants to DOUBLE */
			if (ssa->var_info[v].use_as_double) {
				if (opline->opcode == ZEND_ASSIGN
				 && opline->op2_type == IS_CONST
				 && ssa->ops[op_1].op1_def == v
				 && !RETURN_VALUE_USED(opline)
				) {

// op_1: ASSIGN ? -> #v [use_as_double], long(?) => ASSIGN ? -> #v, double(?)

					zval *zv = CT_CONSTANT_EX(op_array, opline->op2.constant);
					ZEND_ASSERT(Z_TYPE_INFO_P(zv) == IS_LONG);
					ZVAL_DOUBLE(&tmp, zval_get_double(zv));
					opline->op2.constant = zend_optimizer_add_literal(op_array, &tmp);

				} else if (opline->opcode == ZEND_QM_ASSIGN
				 && opline->op1_type == IS_CONST
				) {

// op_1: QM_ASSIGN #v [use_as_double], long(?) => QM_ASSIGN #v, double(?)

					zval *zv = CT_CONSTANT_EX(op_array, opline->op1.constant);
					ZEND_ASSERT(Z_TYPE_INFO_P(zv) == IS_LONG);
					ZVAL_DOUBLE(&tmp, zval_get_double(zv));
					opline->op1.constant = zend_optimizer_add_literal(op_array, &tmp);
				}

			} else {
				if (opline->opcode == ZEND_ADD
				 || opline->opcode == ZEND_SUB
				 || opline->opcode == ZEND_MUL
				 || opline->opcode == ZEND_IS_EQUAL
				 || opline->opcode == ZEND_IS_NOT_EQUAL
				 || opline->opcode == ZEND_IS_SMALLER
				 || opline->opcode == ZEND_IS_SMALLER_OR_EQUAL
				) {

					if (opline->op1_type == IS_CONST
					 && opline->op2_type != IS_CONST
					 && (OP2_INFO() & MAY_BE_ANY) == MAY_BE_DOUBLE
					 && Z_TYPE_INFO_P(CT_CONSTANT_EX(op_array, opline->op1.constant)) == IS_LONG
					) {

// op_1: #v.? = ADD long(?), #?.? [double] => #v.? = ADD double(?), #?.? [double]

						zval *zv = CT_CONSTANT_EX(op_array, opline->op1.constant);
						ZVAL_DOUBLE(&tmp, zval_get_double(zv));
						opline->op1.constant = zend_optimizer_add_literal(op_array, &tmp);

					} else if (opline->op1_type != IS_CONST
					 && opline->op2_type == IS_CONST
					 && (OP1_INFO() & MAY_BE_ANY) == MAY_BE_DOUBLE
					 && Z_TYPE_INFO_P(CT_CONSTANT_EX(op_array, opline->op2.constant)) == IS_LONG
					) {

// op_1: #v.? = ADD #?.? [double], long(?) => #v.? = ADD #?.? [double], double(?)

						zval *zv = CT_CONSTANT_EX(op_array, opline->op2.constant);
						ZVAL_DOUBLE(&tmp, zval_get_double(zv));
						opline->op2.constant = zend_optimizer_add_literal(op_array, &tmp);
					}
				}
			}

			if (ssa->vars[v].var >= op_array->last_var) {
				/* skip TMP and VAR */
				continue;
			}

			if (opline->opcode == ZEND_ASSIGN
			 && ssa->ops[op_1].op1_def == v
			 && !RETURN_VALUE_USED(opline)
			) {
				int orig_var = ssa->ops[op_1].op1_use;

				if (orig_var >= 0
				 && !(ssa->var_info[orig_var].type & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))
				) {

					int src_var = ssa->ops[op_1].op2_use;

					if ((opline->op2_type & (IS_TMP_VAR|IS_VAR))
					 && src_var >= 0
					 && !(ssa->var_info[src_var].type & MAY_BE_REF)
					 && ssa->vars[src_var].definition >= 0
					 && ssa->ops[ssa->vars[src_var].definition].result_def == src_var
					 && ssa->ops[ssa->vars[src_var].definition].result_use < 0
					 && ssa->vars[src_var].use_chain == op_1
					 && ssa->ops[op_1].op2_use_chain < 0
					 && !ssa->vars[src_var].phi_use_chain
					 && !ssa->vars[src_var].sym_use_chain
					 /* see Zend/tests/generators/aborted_yield_during_new.phpt */
					 && op_array->opcodes[ssa->vars[src_var].definition].opcode != ZEND_NEW
					) {

						int op_2 = ssa->vars[src_var].definition;

// op_2: #src_var.T = OP ...                                     => #v.CV = OP ...
// op_1: ASSIGN #orig_var.CV [undef,scalar] -> #v.CV, #src_var.T    NOP

						if (zend_ssa_unlink_use_chain(ssa, op_1, orig_var)) {
							/* Reconstruct SSA */
							ssa->vars[v].definition = op_2;
							ssa->ops[op_2].result_def = v;

							ssa->vars[src_var].definition = -1;
							ssa->vars[src_var].use_chain = -1;

							ssa->ops[op_1].op1_use = -1;
							ssa->ops[op_1].op2_use = -1;
							ssa->ops[op_1].op1_def = -1;
							ssa->ops[op_1].op1_use_chain = -1;

							/* Update opcodes */
							op_array->opcodes[op_2].result_type = opline->op1_type;
							op_array->opcodes[op_2].result.var = opline->op1.var;
							MAKE_NOP(opline);
							remove_nops = 1;
						}
					} else if (opline->op2_type == IS_CONST
					 || ((opline->op2_type & (IS_TMP_VAR|IS_VAR|IS_CV))
					     && ssa->ops[op_1].op2_use >= 0
					     && ssa->ops[op_1].op2_def < 0)
					) {

// op_1: ASSIGN #orig_var.CV [undef,scalar] -> #v.CV, CONST|TMPVAR => QM_ASSIGN v.CV, CONST|TMPVAR

						if (zend_ssa_unlink_use_chain(ssa, op_1, orig_var)) {
							/* Reconstruct SSA */
							ssa->ops[op_1].result_def = v;
							ssa->ops[op_1].op1_def = -1;
							ssa->ops[op_1].op1_use = ssa->ops[op_1].op2_use;
							ssa->ops[op_1].op1_use_chain = ssa->ops[op_1].op2_use_chain;
							ssa->ops[op_1].op2_use = -1;
							ssa->ops[op_1].op2_use_chain = -1;

							/* Update opcode */
							opline->result_type = opline->op1_type;
							opline->result.var = opline->op1.var;
							opline->op1_type = opline->op2_type;
							opline->op1.var = opline->op2.var;
							opline->op2_type = IS_UNUSED;
							opline->op2.var = 0;
							opline->opcode = ZEND_QM_ASSIGN;
						}
					}
				}

			} else if (opline->opcode == ZEND_ASSIGN_ADD
			 && opline->extended_value == 0
			 && ssa->ops[op_1].op1_def == v
			 && opline->op2_type == IS_CONST
			 && Z_TYPE_P(CT_CONSTANT_EX(op_array, opline->op2.constant)) == IS_LONG
			 && Z_LVAL_P(CT_CONSTANT_EX(op_array, opline->op2.constant)) == 1
			 && ssa->ops[op_1].op1_use >= 0
			 && !(ssa->var_info[ssa->ops[op_1].op1_use].type & (MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {

// op_1: ASSIGN_ADD #?.CV [undef,null,int,foat] ->#v.CV, int(1) => PRE_INC #?.CV ->#v.CV

				opline->opcode = ZEND_PRE_INC;
				SET_UNUSED(opline->op2);

			} else if (opline->opcode == ZEND_ASSIGN_SUB
			 && opline->extended_value == 0
			 && ssa->ops[op_1].op1_def == v
			 && opline->op2_type == IS_CONST
			 && Z_TYPE_P(CT_CONSTANT_EX(op_array, opline->op2.constant)) == IS_LONG
			 && Z_LVAL_P(CT_CONSTANT_EX(op_array, opline->op2.constant)) == 1
			 && ssa->ops[op_1].op1_use >= 0
			 && !(ssa->var_info[ssa->ops[op_1].op1_use].type & (MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {

// op_1: ASSIGN_SUB #?.CV [undef,null,int,foat] -> #v.CV, int(1) => PRE_DEC #?.CV ->#v.CV

				opline->opcode = ZEND_PRE_DEC;
				SET_UNUSED(opline->op2);

			} else if (opline->opcode == ZEND_VERIFY_RETURN_TYPE
			 && ssa->ops[op_1].op1_def == v
			 && ssa->ops[op_1].op1_use >= 0
			 && ssa->ops[op_1].op1_use_chain == -1
			 && ssa->vars[v].use_chain >= 0
			 && can_elide_return_type_check(op_array, ssa, &ssa->ops[op_1])) {

// op_1: VERIFY_RETURN_TYPE #orig_var.CV [T] -> #v.CV [T] => NOP

				int orig_var = ssa->ops[op_1].op1_use;
				int ret = ssa->vars[v].use_chain;

				ssa->vars[orig_var].use_chain = ret;
				ssa->ops[ret].op1_use = orig_var;

				ssa->vars[v].definition = -1;
				ssa->vars[v].use_chain = -1;

				ssa->ops[op_1].op1_def = -1;
				ssa->ops[op_1].op1_use = -1;

				MAKE_NOP(opline);
				remove_nops = 1;

			} else if (ssa->ops[op_1].op1_def == v
			 && !RETURN_VALUE_USED(opline)
			 && ssa->ops[op_1].op1_use >= 0
			 && !(ssa->var_info[ssa->ops[op_1].op1_use].type & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))
			 && (opline->opcode == ZEND_ASSIGN_ADD
			  || opline->opcode == ZEND_ASSIGN_SUB
			  || opline->opcode == ZEND_ASSIGN_MUL
			  || opline->opcode == ZEND_ASSIGN_DIV
			  || opline->opcode == ZEND_ASSIGN_MOD
			  || opline->opcode == ZEND_ASSIGN_SL
			  || opline->opcode == ZEND_ASSIGN_SR
			  || opline->opcode == ZEND_ASSIGN_BW_OR
			  || opline->opcode == ZEND_ASSIGN_BW_AND
			  || opline->opcode == ZEND_ASSIGN_BW_XOR)
			 && opline->extended_value == 0) {

// op_1: ASSIGN_ADD #orig_var.CV [undef,null,bool,int,double] -> #v.CV, ? => #v.CV = ADD #orig_var.CV, ?

				/* Reconstruct SSA */
				ssa->ops[op_1].result_def = ssa->ops[op_1].op1_def;
				ssa->ops[op_1].op1_def = -1;

				/* Update opcode */
				opline->opcode -= (ZEND_ASSIGN_ADD - ZEND_ADD);
				opline->result_type = opline->op1_type;
				opline->result.var = opline->op1.var;

			}
		}

		if (remove_nops) {
			zend_ssa_remove_nops(op_array, ssa);
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
