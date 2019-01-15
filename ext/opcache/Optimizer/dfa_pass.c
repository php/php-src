/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
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

#ifndef ZEND_DEBUG_DFA
# define ZEND_DEBUG_DFA ZEND_DEBUG
#endif

#if ZEND_DEBUG_DFA
# include "ssa_integrity.c"
#endif

int zend_dfa_analyze_op_array(zend_op_array *op_array, zend_optimizer_ctx *ctx, zend_ssa *ssa)
{
	uint32_t build_flags;

	if (op_array->last_try_catch) {
		/* TODO: we can't analyze functions with try/catch/finally ??? */
		return FAILURE;
	}

    /* Build SSA */
	memset(ssa, 0, sizeof(zend_ssa));

	if (zend_build_cfg(&ctx->arena, op_array, ZEND_CFG_NO_ENTRY_PREDECESSORS, &ssa->cfg) != SUCCESS) {
		return FAILURE;
	}

	if ((ssa->cfg.flags & ZEND_FUNC_INDIRECT_VAR_ACCESS)) {
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
	if (zend_cfg_identify_loops(op_array, &ssa->cfg) != SUCCESS) {
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
	if (zend_build_ssa(&ctx->arena, ctx->script, op_array, build_flags, ssa) != SUCCESS) {
		return FAILURE;
	}

	if (ctx->debug_level & ZEND_DUMP_DFA_SSA) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA, "dfa ssa", ssa);
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

	if (zend_ssa_inference(&ctx->arena, op_array, ctx->script, ssa, ctx->optimization_level) != SUCCESS) {
		return FAILURE;
	}

	if (zend_ssa_escape_analysis(ctx->script, op_array, ssa) != SUCCESS) {
		return FAILURE;
	}

	if (ctx->debug_level & ZEND_DUMP_DFA_SSA_VARS) {
		zend_dump_ssa_variables(op_array, ssa, 0);
	}

	return SUCCESS;
}

static void zend_ssa_remove_nops(zend_op_array *op_array, zend_ssa *ssa, zend_optimizer_ctx *ctx)
{
	zend_basic_block *blocks = ssa->cfg.blocks;
	zend_basic_block *end = blocks + ssa->cfg.blocks_count;
	zend_basic_block *b;
	zend_func_info *func_info;
	int j;
	uint32_t i = 0;
	uint32_t target = 0;
	uint32_t *shiftlist;
	ALLOCA_FLAG(use_heap);

	shiftlist = (uint32_t *)do_alloca(sizeof(uint32_t) * op_array->last, use_heap);
	memset(shiftlist, 0, sizeof(uint32_t) * op_array->last);
	/* remove empty callee_info */
	func_info = ZEND_FUNC_INFO(op_array);
	if (func_info) {
		zend_call_info **call_info = &func_info->callee_info;
		while ((*call_info)) {
			if ((*call_info)->caller_init_opline->opcode == ZEND_NOP) {
				*call_info = (*call_info)->next_callee;
			} else {
				call_info = &(*call_info)->next_callee;
			}
		}
	}

	for (b = blocks; b < end; b++) {
		if (b->flags & (ZEND_BB_REACHABLE|ZEND_BB_UNREACHABLE_FREE)) {
			uint32_t end;

			if (b->len) {
				while (i < b->start) {
					shiftlist[i] = i - target;
					i++;
				}

				if (b->flags & ZEND_BB_UNREACHABLE_FREE) {
					/* Only keep the FREE for the loop var */
					ZEND_ASSERT(op_array->opcodes[b->start].opcode == ZEND_FREE
							|| op_array->opcodes[b->start].opcode == ZEND_FE_FREE);
					b->len = 1;
				}

				end = b->start + b->len;
				b->start = target;
				while (i < end) {
					shiftlist[i] = i - target;
					if (EXPECTED(op_array->opcodes[i].opcode != ZEND_NOP) ||
					   /* Keep NOP to support ZEND_VM_SMART_BRANCH. Using "target-1" instead of
					    * "i-1" here to check the last non-NOP instruction. */
					   (target > 0 &&
					    i + 1 < op_array->last &&
					    (op_array->opcodes[i+1].opcode == ZEND_JMPZ ||
					     op_array->opcodes[i+1].opcode == ZEND_JMPNZ) &&
					    zend_is_smart_branch(op_array->opcodes + target - 1))) {
						if (i != target) {
							op_array->opcodes[target] = op_array->opcodes[i];
							ssa->ops[target] = ssa->ops[i];
							ssa->cfg.map[target] = b - blocks;
						}
						target++;
					}
					i++;
				}
				if (target != end) {
					zend_op *opline;
					zend_op *new_opline;

					b->len = target - b->start;
					opline = op_array->opcodes + end - 1;
					if (opline->opcode == ZEND_NOP) {
						continue;
					}

					new_opline = op_array->opcodes + target - 1;
					zend_optimizer_migrate_jump(op_array, new_opline, opline);
				}
			} else {
				b->start = target;
			}
		} else {
			b->start = target;
			b->len = 0;
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
				zend_optimizer_shift_jump(op_array, opline, shiftlist);
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
		if (op_array->fn_flags & ZEND_ACC_EARLY_BINDING) {
			uint32_t *opline_num = &ctx->script->first_early_binding_opline;

			ZEND_ASSERT(op_array == &ctx->script->main_op_array);
			do {
				*opline_num -= shiftlist[*opline_num];
				opline_num = &op_array->opcodes[*opline_num].result.opline_num;
			} while (*opline_num != (uint32_t)-1);
		}

		/* update call graph */
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

	if (use_info->type & MAY_BE_REF) {
		return 0;
	}

	/* A type is possible that is not in the allowed types */
	if ((use_info->type & (MAY_BE_ANY|MAY_BE_UNDEF)) & ~(def_info->type & MAY_BE_ANY)) {
		return 0;
	}

	if (ZEND_TYPE_CODE(info->type) == IS_CALLABLE) {
		return 0;
	}

	if (ZEND_TYPE_IS_CLASS(info->type)) {
		if (!use_info->ce || !def_info->ce || !instanceof_function(use_info->ce, def_info->ce)) {
			return 0;
		}
	}

	return 1;
}

static zend_bool opline_supports_assign_contraction(
		zend_ssa *ssa, zend_op *opline, int src_var, uint32_t cv_var) {
	if (opline->opcode == ZEND_NEW) {
		/* see Zend/tests/generators/aborted_yield_during_new.phpt */
		return 0;
	}

	if (opline->opcode == ZEND_DO_ICALL || opline->opcode == ZEND_DO_UCALL
			|| opline->opcode == ZEND_DO_FCALL || opline->opcode == ZEND_DO_FCALL_BY_NAME) {
		/* Function calls may dtor the return value after it has already been written -- allow
		 * direct assignment only for types where a double-dtor does not matter. */
		uint32_t type = ssa->var_info[src_var].type;
		uint32_t simple = MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE;
		return !((type & MAY_BE_ANY) & ~simple);
	}

	if (opline->opcode == ZEND_POST_INC || opline->opcode == ZEND_POST_DEC) {
		/* POST_INC/DEC write the result variable before performing the inc/dec. For $i = $i++
		 * eliding the temporary variable would thus yield an incorrect result. */
		return opline->op1_type != IS_CV || opline->op1.var != cv_var;
	}

	if (opline->opcode == ZEND_INIT_ARRAY) {
		/* INIT_ARRAY initializes the result array before reading key/value. */
		return (opline->op1_type != IS_CV || opline->op1.var != cv_var)
			&& (opline->op2_type != IS_CV || opline->op2.var != cv_var);
	}

	if (opline->opcode == ZEND_CAST
			&& (opline->extended_value == IS_ARRAY || opline->extended_value == IS_OBJECT)) {
		/* CAST to array/object may initialize the result to an empty array/object before
		 * reading the expression. */
		return opline->op1_type != IS_CV || opline->op1.var != cv_var;
	}

	return 1;
}

int zend_dfa_optimize_calls(zend_op_array *op_array, zend_ssa *ssa)
{
	zend_func_info *func_info = ZEND_FUNC_INFO(op_array);
	int removed_ops = 0;

	if (func_info->callee_info) {
		zend_call_info *call_info = func_info->callee_info;

		do {
			if (call_info->caller_call_opline->opcode == ZEND_DO_ICALL
			 && call_info->callee_func
			 && ZSTR_LEN(call_info->callee_func->common.function_name) == sizeof("in_array")-1
			 && memcmp(ZSTR_VAL(call_info->callee_func->common.function_name), "in_array", sizeof("in_array")-1) == 0
			 && (call_info->caller_init_opline->extended_value == 2
			  || (call_info->caller_init_opline->extended_value == 3
			   && (call_info->caller_call_opline - 1)->opcode == ZEND_SEND_VAL
			   && (call_info->caller_call_opline - 1)->op1_type == IS_CONST))) {

				zend_op *send_array;
				zend_op *send_needly;
				zend_bool strict = 0;

				if (call_info->caller_init_opline->extended_value == 2) {
					send_array = call_info->caller_call_opline - 1;
					send_needly = call_info->caller_call_opline - 2;
				} else {
					if (zend_is_true(CT_CONSTANT_EX(op_array, (call_info->caller_call_opline - 1)->op1.constant))) {
						strict = 1;
					}
					send_array = call_info->caller_call_opline - 2;
					send_needly = call_info->caller_call_opline - 3;
				}

				if (send_array->opcode == ZEND_SEND_VAL
				 && send_array->op1_type == IS_CONST
				 && Z_TYPE_P(CT_CONSTANT_EX(op_array, send_array->op1.constant)) == IS_ARRAY
				 && (send_needly->opcode == ZEND_SEND_VAL
				  || send_needly->opcode == ZEND_SEND_VAR)
			    ) {
					int ok = 1;

					HashTable *src = Z_ARRVAL_P(CT_CONSTANT_EX(op_array, send_array->op1.constant));
					HashTable *dst;
					zval *val, tmp;
					zend_ulong idx;

					ZVAL_TRUE(&tmp);
					dst = zend_new_array(zend_hash_num_elements(src));
					if (strict) {
						ZEND_HASH_FOREACH_VAL(src, val) {
							if (Z_TYPE_P(val) == IS_STRING) {
								zend_hash_add(dst, Z_STR_P(val), &tmp);
							} else if (Z_TYPE_P(val) == IS_LONG) {
								zend_hash_index_add(dst, Z_LVAL_P(val), &tmp);
							} else {
								zend_array_destroy(dst);
								ok = 0;
								break;
							}
						} ZEND_HASH_FOREACH_END();
					} else {
						ZEND_HASH_FOREACH_VAL(src, val) {
							if (Z_TYPE_P(val) != IS_STRING || ZEND_HANDLE_NUMERIC(Z_STR_P(val), idx)) {
								zend_array_destroy(dst);
								ok = 0;
								break;
							}
							zend_hash_add(dst, Z_STR_P(val), &tmp);
						} ZEND_HASH_FOREACH_END();
					}

					if (ok) {
						uint32_t op_num = send_needly - op_array->opcodes;
						zend_ssa_op *ssa_op = ssa->ops + op_num;

						if (ssa_op->op1_use >= 0) {
							/* Reconstruct SSA */
							int var_num = ssa_op->op1_use;
							zend_ssa_var *var = ssa->vars + var_num;

							ZEND_ASSERT(ssa_op->op1_def < 0);
							zend_ssa_unlink_use_chain(ssa, op_num, ssa_op->op1_use);
							ssa_op->op1_use = -1;
							ssa_op->op1_use_chain = -1;
							op_num = call_info->caller_call_opline - op_array->opcodes;
							ssa_op = ssa->ops + op_num;
							ssa_op->op1_use = var_num;
							ssa_op->op1_use_chain = var->use_chain;
							var->use_chain = op_num;
						}

						ZVAL_ARR(&tmp, dst);

						/* Update opcode */
						call_info->caller_call_opline->opcode = ZEND_IN_ARRAY;
						call_info->caller_call_opline->extended_value = strict;
						call_info->caller_call_opline->op1_type = send_needly->op1_type;
						call_info->caller_call_opline->op1.num = send_needly->op1.num;
						call_info->caller_call_opline->op2_type = IS_CONST;
						call_info->caller_call_opline->op2.constant = zend_optimizer_add_literal(op_array, &tmp);
						if (call_info->caller_init_opline->extended_value == 3) {
							MAKE_NOP(call_info->caller_call_opline - 1);
						}
						MAKE_NOP(call_info->caller_init_opline);
						MAKE_NOP(send_needly);
						MAKE_NOP(send_array);
						removed_ops++;

					}
				}
			}
			call_info = call_info->next_callee;
		} while (call_info);
	}

	return removed_ops;
}

static zend_always_inline void take_successor_0(zend_ssa *ssa, int block_num, zend_basic_block *block)
{
	if (block->successors_count == 2) {
		if (block->successors[1] != block->successors[0]) {
			zend_ssa_remove_predecessor(ssa, block_num, block->successors[1]);
		}
		block->successors_count = 1;
	}
}

static zend_always_inline void take_successor_1(zend_ssa *ssa, int block_num, zend_basic_block *block)
{
	if (block->successors_count == 2) {
		if (block->successors[1] != block->successors[0]) {
			zend_ssa_remove_predecessor(ssa, block_num, block->successors[0]);
			block->successors[0] = block->successors[1];
		}
		block->successors_count = 1;
	}
}

static void compress_block(zend_op_array *op_array, zend_basic_block *block)
{
	while (block->len > 0) {
		zend_op *opline = &op_array->opcodes[block->start + block->len - 1];

		if (opline->opcode == ZEND_NOP
				&& (block->len == 1 || !zend_is_smart_branch(opline - 1))) {
			block->len--;
		} else {
			break;
		}
	}
}

static void replace_predecessor(zend_ssa *ssa, int block_id, int old_pred, int new_pred) {
	zend_basic_block *block = &ssa->cfg.blocks[block_id];
	int *predecessors = &ssa->cfg.predecessors[block->predecessor_offset];
	zend_ssa_phi *phi;

	int i;
	int old_pred_idx = -1;
	int new_pred_idx = -1;
	for (i = 0; i < block->predecessors_count; i++) {
		if (predecessors[i] == old_pred) {
			old_pred_idx = i;
		}
		if (predecessors[i] == new_pred) {
			new_pred_idx = i;
		}
	}

	ZEND_ASSERT(old_pred_idx != -1);
	if (new_pred_idx == -1) {
		/* If the new predecessor doesn't exist yet, simply rewire the old one */
		predecessors[old_pred_idx] = new_pred;
	} else {
		/* Otherwise, rewiring the old predecessor would make the new predecessor appear
		 * twice, which violates our CFG invariants. Remove the old predecessor instead. */
		memmove(
			predecessors + old_pred_idx,
			predecessors + old_pred_idx + 1,
			sizeof(int) * (block->predecessors_count - old_pred_idx - 1)
		);

		/* Also remove the corresponding phi node entries */
		for (phi = ssa->blocks[block_id].phis; phi; phi = phi->next) {
			memmove(
				phi->sources + old_pred_idx,
				phi->sources + old_pred_idx + 1,
				sizeof(int) * (block->predecessors_count - old_pred_idx - 1)
			);
		}

		block->predecessors_count--;
	}
}

static void zend_ssa_replace_control_link(zend_op_array *op_array, zend_ssa *ssa, int from, int to, int new_to)
{
	zend_basic_block *src = &ssa->cfg.blocks[from];
	zend_basic_block *old = &ssa->cfg.blocks[to];
	zend_basic_block *dst = &ssa->cfg.blocks[new_to];
	int i;
	zend_op *opline;

	for (i = 0; i < src->successors_count; i++) {
		if (src->successors[i] == to) {
			src->successors[i] = new_to;
		}
	}

	if (src->len > 0) {
		opline = op_array->opcodes + src->start + src->len - 1;
		switch (opline->opcode) {
			case ZEND_JMP:
			case ZEND_FAST_CALL:
				ZEND_ASSERT(ZEND_OP1_JMP_ADDR(opline) == op_array->opcodes + old->start);
				ZEND_SET_OP_JMP_ADDR(opline, opline->op1, op_array->opcodes + dst->start);
				break;
			case ZEND_JMPZNZ:
				if (ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value) == old->start) {
					opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, dst->start);
				}
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
				if (ZEND_OP2_JMP_ADDR(opline) == op_array->opcodes + old->start) {
					ZEND_SET_OP_JMP_ADDR(opline, opline->op2, op_array->opcodes + dst->start);
				}
				break;
			case ZEND_CATCH:
				if (!(opline->extended_value & ZEND_LAST_CATCH)) {
					if (ZEND_OP2_JMP_ADDR(opline) == op_array->opcodes + old->start) {
						ZEND_SET_OP_JMP_ADDR(opline, opline->op2, op_array->opcodes + dst->start);
					}
				}
				break;
			case ZEND_DECLARE_ANON_CLASS:
			case ZEND_DECLARE_ANON_INHERITED_CLASS:
			case ZEND_FE_FETCH_R:
			case ZEND_FE_FETCH_RW:
				if (ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value) == old->start) {
					opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, dst->start);
				}
				break;
			case ZEND_SWITCH_LONG:
			case ZEND_SWITCH_STRING:
				{
					HashTable *jumptable = Z_ARRVAL(ZEND_OP2_LITERAL(opline));
					zval *zv;
					ZEND_HASH_FOREACH_VAL(jumptable, zv) {
						if (ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, Z_LVAL_P(zv)) == old->start) {
							Z_LVAL_P(zv) = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, dst->start);
						}
					} ZEND_HASH_FOREACH_END();
					if (ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value) == old->start) {
						opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, dst->start);
					}
					break;
				}
		}
	}

	replace_predecessor(ssa, new_to, to, from);
}

static void zend_ssa_unlink_block(zend_op_array *op_array, zend_ssa *ssa, zend_basic_block *block, int block_num)
{
	if (block->predecessors_count == 1 && ssa->blocks[block_num].phis == NULL) {
		int *predecessors, i;

		ZEND_ASSERT(block->successors_count == 1);
		predecessors = &ssa->cfg.predecessors[block->predecessor_offset];
		for (i = 0; i < block->predecessors_count; i++) {
			zend_ssa_replace_control_link(op_array, ssa, predecessors[i], block_num, block->successors[0]);
		}
		zend_ssa_remove_block(op_array, ssa, block_num);
	}
}

static int zend_dfa_optimize_jmps(zend_op_array *op_array, zend_ssa *ssa)
{
	int removed_ops = 0;
	int block_num = 0;

	for (block_num = 1; block_num < ssa->cfg.blocks_count; block_num++) {
		zend_basic_block *block = &ssa->cfg.blocks[block_num];

		if (!(block->flags & ZEND_BB_REACHABLE)) {
			continue;
		}
		compress_block(op_array, block);
		if (block->len == 0) {
			zend_ssa_unlink_block(op_array, ssa, block, block_num);
		}
	}

	block_num = 0;
	while (block_num < ssa->cfg.blocks_count
		&& !(ssa->cfg.blocks[block_num].flags & ZEND_BB_REACHABLE)) {
		block_num++;
	}
	while (block_num < ssa->cfg.blocks_count) {
		int next_block_num = block_num + 1;
		zend_basic_block *block = &ssa->cfg.blocks[block_num];
		uint32_t op_num;
		zend_op *opline;
		zend_ssa_op *ssa_op;

		while (next_block_num < ssa->cfg.blocks_count
			&& !(ssa->cfg.blocks[next_block_num].flags & ZEND_BB_REACHABLE)) {
			next_block_num++;
		}

		if (block->len) {
			op_num = block->start + block->len - 1;
			opline = op_array->opcodes + op_num;
			ssa_op = ssa->ops + op_num;

			switch (opline->opcode) {
				case ZEND_JMP:
optimize_jmp:
					if (block->successors[0] == next_block_num) {
						MAKE_NOP(opline);
						removed_ops++;
						goto optimize_nop;
					}
					break;
				case ZEND_JMPZ:
optimize_jmpz:
					if (opline->op1_type == IS_CONST) {
						if (zend_is_true(CT_CONSTANT_EX(op_array, opline->op1.constant))) {
							MAKE_NOP(opline);
							removed_ops++;
							take_successor_1(ssa, block_num, block);
							goto optimize_nop;
						} else {
							opline->opcode = ZEND_JMP;
							COPY_NODE(opline->op1, opline->op2);
							take_successor_0(ssa, block_num, block);
							goto optimize_jmp;
						}
					} else {
						if (block->successors[0] == next_block_num) {
							take_successor_0(ssa, block_num, block);
							if (opline->op1_type == IS_CV && (OP1_INFO() & MAY_BE_UNDEF)) {
								opline->opcode = ZEND_CHECK_VAR;
								opline->op2.num = 0;
							} else if (opline->op1_type == IS_CV || !(OP1_INFO() & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {
								zend_ssa_remove_instr(ssa, opline, ssa_op);
								removed_ops++;
								goto optimize_nop;
							} else {
								opline->opcode = ZEND_FREE;
								opline->op2.num = 0;
							}
						}
					}
					break;
				case ZEND_JMPNZ:
optimize_jmpnz:
					if (opline->op1_type == IS_CONST) {
						if (zend_is_true(CT_CONSTANT_EX(op_array, opline->op1.constant))) {
							opline->opcode = ZEND_JMP;
							COPY_NODE(opline->op1, opline->op2);
							take_successor_0(ssa, block_num, block);
							goto optimize_jmp;
						} else {
							MAKE_NOP(opline);
							removed_ops++;
							take_successor_1(ssa, block_num, block);
							goto optimize_nop;
						}
					} else if (block->successors_count == 2) {
						if (block->successors[0] == next_block_num) {
							take_successor_0(ssa, block_num, block);
							if (opline->op1_type == IS_CV && (OP1_INFO() & MAY_BE_UNDEF)) {
								opline->opcode = ZEND_CHECK_VAR;
								opline->op2.num = 0;
							} else if (opline->op1_type == IS_CV || !(OP1_INFO() & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {
								zend_ssa_remove_instr(ssa, opline, ssa_op);
								removed_ops++;
								goto optimize_nop;
							} else {
								opline->opcode = ZEND_FREE;
								opline->op2.num = 0;
							}
						}
					}
					break;
				case ZEND_JMPZNZ:
					if (opline->op1_type == IS_CONST) {
						if (zend_is_true(CT_CONSTANT_EX(op_array, opline->op1.constant))) {
							zend_op *target_opline = ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value);
							ZEND_SET_OP_JMP_ADDR(opline, opline->op1, target_opline);
							take_successor_1(ssa, block_num, block);
						} else {
							zend_op *target_opline = ZEND_OP2_JMP_ADDR(opline);
							ZEND_SET_OP_JMP_ADDR(opline, opline->op1, target_opline);
							take_successor_0(ssa, block_num, block);
						}
						opline->op1_type = IS_UNUSED;
						opline->extended_value = 0;
						opline->opcode = ZEND_JMP;
						goto optimize_jmp;
					} else if (block->successors_count == 2) {
						if (block->successors[0] == block->successors[1]) {
							take_successor_0(ssa, block_num, block);
							if (block->successors[0] == next_block_num) {
								if (opline->op1_type == IS_CV && (OP1_INFO() & MAY_BE_UNDEF)) {
									opline->opcode = ZEND_CHECK_VAR;
									opline->op2.num = 0;
								} else if (opline->op1_type == IS_CV || !(OP1_INFO() & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {
									zend_ssa_remove_instr(ssa, opline, ssa_op);
									removed_ops++;
									goto optimize_nop;
								} else {
									opline->opcode = ZEND_FREE;
									opline->op2.num = 0;
								}
							} else if ((opline->op1_type == IS_CV && !(OP1_INFO() & MAY_BE_UNDEF)) || !(OP1_INFO() & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {
								ZEND_ASSERT(ssa_op->op1_use >= 0);
								zend_ssa_unlink_use_chain(ssa, op_num, ssa_op->op1_use);
								ssa_op->op1_use = -1;
								ssa_op->op1_use_chain = -1;
								opline->opcode = ZEND_JMP;
								opline->op1_type = IS_UNUSED;
								opline->op1.num = opline->op2.num;
								goto optimize_jmp;
							}
						}
					}
					break;
				case ZEND_JMPZ_EX:
					if (ssa->vars[ssa_op->result_def].use_chain < 0
							&& ssa->vars[ssa_op->result_def].phi_use_chain == NULL) {
						opline->opcode = ZEND_JMPZ;
						opline->result_type = IS_UNUSED;
						zend_ssa_remove_result_def(ssa, ssa_op);
						goto optimize_jmpz;
					} else if (opline->op1_type == IS_CONST) {
						if (zend_is_true(CT_CONSTANT_EX(op_array, opline->op1.constant))) {
							opline->opcode = ZEND_QM_ASSIGN;
							take_successor_1(ssa, block_num, block);
						}
					}
					break;
				case ZEND_JMPNZ_EX:
					if (ssa->vars[ssa_op->result_def].use_chain < 0
							&& ssa->vars[ssa_op->result_def].phi_use_chain == NULL) {
						opline->opcode = ZEND_JMPNZ;
						opline->result_type = IS_UNUSED;
						zend_ssa_remove_result_def(ssa, ssa_op);
						goto optimize_jmpnz;
					} else if (opline->op1_type == IS_CONST) {
						if (!zend_is_true(CT_CONSTANT_EX(op_array, opline->op1.constant))) {
							opline->opcode = ZEND_QM_ASSIGN;
							take_successor_1(ssa, block_num, block);
						}
					}
					break;
				case ZEND_JMP_SET:
					if (ssa->vars[ssa_op->result_def].use_chain < 0
							&& ssa->vars[ssa_op->result_def].phi_use_chain == NULL) {
						opline->opcode = ZEND_JMPNZ;
						opline->result_type = IS_UNUSED;
						zend_ssa_remove_result_def(ssa, ssa_op);
						goto optimize_jmpnz;
					} else if (opline->op1_type == IS_CONST) {
						if (!zend_is_true(CT_CONSTANT_EX(op_array, opline->op1.constant))) {
							MAKE_NOP(opline);
							removed_ops++;
							take_successor_1(ssa, block_num, block);
							goto optimize_nop;
						}
					}
					break;
				case ZEND_COALESCE:
				{
					zend_ssa_var *var = &ssa->vars[ssa_op->result_def];
					if (opline->op1_type == IS_CONST
							&& var->use_chain < 0 && var->phi_use_chain == NULL) {
						if (Z_TYPE_P(CT_CONSTANT_EX(op_array, opline->op1.constant)) == IS_NULL) {
							zend_ssa_remove_result_def(ssa, ssa_op);
							MAKE_NOP(opline);
							removed_ops++;
							take_successor_1(ssa, block_num, block);
							goto optimize_nop;
						} else {
							if (opline->result_type & (IS_TMP_VAR|IS_VAR)) {
								zend_optimizer_remove_live_range_ex(op_array, opline->result.var, var->definition);
							}
							opline->opcode = ZEND_JMP;
							opline->result_type = IS_UNUSED;
							zend_ssa_remove_result_def(ssa, ssa_op);
							COPY_NODE(opline->op1, opline->op2);
							take_successor_0(ssa, block_num, block);
							goto optimize_jmp;
						}
					}
					break;
				}
				case ZEND_NOP:
optimize_nop:
					compress_block(op_array, block);
					if (block->len == 0) {
						if (block_num > 0) {
							zend_ssa_unlink_block(op_array, ssa, block, block_num);
							/* backtrack to previous basic block */
							do {
								block_num--;
							} while (block_num >= 0
								&& !(ssa->cfg.blocks[block_num].flags & ZEND_BB_REACHABLE));
							if (block_num >= 0) {
								continue;
							}
						}
					}
					break;
				default:
					break;
			}
		}

		block_num = next_block_num;
	}

	return removed_ops;
}

void zend_dfa_optimize_op_array(zend_op_array *op_array, zend_optimizer_ctx *ctx, zend_ssa *ssa, zend_call_info **call_map)
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

#if ZEND_DEBUG_DFA
		ssa_verify_integrity(op_array, ssa, "before dfa");
#endif

		if (ZEND_OPTIMIZER_PASS_8 & ctx->optimization_level) {
			if (sccp_optimize_op_array(ctx, op_array, ssa, call_map)) {
				remove_nops = 1;
			}

			if (zend_dfa_optimize_jmps(op_array, ssa)) {
				remove_nops = 1;
			}

#if ZEND_DEBUG_DFA
			ssa_verify_integrity(op_array, ssa, "after sccp");
#endif
			if (ZEND_FUNC_INFO(op_array)) {
				if (zend_dfa_optimize_calls(op_array, ssa)) {
					remove_nops = 1;
				}
			}
			if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_8) {
				zend_dump_op_array(op_array, ZEND_DUMP_SSA, "after sccp pass", ssa);
			}
#if ZEND_DEBUG_DFA
			ssa_verify_integrity(op_array, ssa, "after calls");
#endif
		}

		if (ZEND_OPTIMIZER_PASS_14 & ctx->optimization_level) {
			if (dce_optimize_op_array(op_array, ssa, 0)) {
				remove_nops = 1;
			}
			if (zend_dfa_optimize_jmps(op_array, ssa)) {
				remove_nops = 1;
			}
			if (ctx->debug_level & ZEND_DUMP_AFTER_PASS_14) {
				zend_dump_op_array(op_array, ZEND_DUMP_SSA, "after dce pass", ssa);
			}
#if ZEND_DEBUG_DFA
			ssa_verify_integrity(op_array, ssa, "after dce");
#endif
		}

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
				} else if (opline->opcode == ZEND_CONCAT) {
					if (!(OP1_INFO() & MAY_BE_OBJECT)
					 && !(OP2_INFO() & MAY_BE_OBJECT)) {
						opline->opcode = ZEND_FAST_CONCAT;
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
					 && opline_supports_assign_contraction(
						 ssa, &op_array->opcodes[ssa->vars[src_var].definition],
						 src_var, opline->op1.var)
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

						if (ssa->ops[op_1].op1_use != ssa->ops[op_1].op2_use) {
							zend_ssa_unlink_use_chain(ssa, op_1, orig_var);
						} else {
							ssa->ops[op_1].op2_use_chain = ssa->ops[op_1].op1_use_chain;
						}

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
				if (zend_ssa_unlink_use_chain(ssa, op_1, orig_var)) {

					int ret = ssa->vars[v].use_chain;

					ssa->ops[ret].op1_use = orig_var;
					ssa->ops[ret].op1_use_chain = ssa->vars[orig_var].use_chain;
					ssa->vars[orig_var].use_chain = ret;

					ssa->vars[v].definition = -1;
					ssa->vars[v].use_chain = -1;

					ssa->ops[op_1].op1_def = -1;
					ssa->ops[op_1].op1_use = -1;

					MAKE_NOP(opline);
					remove_nops = 1;
				}

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

#if ZEND_DEBUG_DFA
		ssa_verify_integrity(op_array, ssa, "after dfa");
#endif

		if (remove_nops) {
			zend_ssa_remove_nops(op_array, ssa, ctx);
#if ZEND_DEBUG_DFA
			ssa_verify_integrity(op_array, ssa, "after nop");
#endif
		}
	}

	if (ctx->debug_level & ZEND_DUMP_AFTER_DFA_PASS) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA, "after dfa pass", ssa);
	}
}

void zend_optimize_dfa(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	void *checkpoint = zend_arena_checkpoint(ctx->arena);
	zend_ssa ssa;

	if (zend_dfa_analyze_op_array(op_array, ctx, &ssa) != SUCCESS) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	zend_dfa_optimize_op_array(op_array, ctx, &ssa, NULL);

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
