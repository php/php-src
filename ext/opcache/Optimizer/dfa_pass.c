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

#ifndef ZEND_DEBUG_DFA
# define ZEND_DEBUG_DFA ZEND_DEBUG
#endif

#if ZEND_DEBUG_DFA
# include "ssa_integrity.c"
#endif

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
		if (op_array->early_binding != (uint32_t)-1) {
			uint32_t *opline_num = &op_array->early_binding;

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
					dst = emalloc(sizeof(HashTable));
					zend_hash_init(dst, zend_hash_num_elements(src), NULL, ZVAL_PTR_DTOR, 0);
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

static int zend_dfa_optimize_jmps(zend_op_array *op_array, zend_ssa *ssa)
{
	int removed_ops = 0;
	int block_num = 0;

	while (block_num < ssa->cfg.blocks_count
		&& !(ssa->cfg.blocks[block_num].flags & ZEND_BB_REACHABLE)) {
		block_num++;
	}
	while (block_num < ssa->cfg.blocks_count) {
		int next_block_num = block_num + 1;
		zend_basic_block *block = &ssa->cfg.blocks[block_num];
		uint32_t op_num;
		zend_op *opline;
		zend_ssa_op *op;

		while (next_block_num < ssa->cfg.blocks_count
			&& !(ssa->cfg.blocks[next_block_num].flags & ZEND_BB_REACHABLE)) {
			next_block_num++;
		}

		if (block->len) {
			if (block->successors_count == 2) {
				if (block->successors[0] == block->successors[1]) {
					op_num = block->start + block->len - 1;
					opline = op_array->opcodes + op_num;
					switch (opline->opcode) {
						case ZEND_JMPZ:
						case ZEND_JMPNZ:
						case ZEND_JMPZNZ:
							op = ssa->ops + op_num;
							if (block->successors[0] == next_block_num) {
								if (opline->op1_type & (IS_CV|IS_CONST)) {
									zend_ssa_remove_instr(ssa, opline, op);
									if (op->op1_use >= 0) {
										zend_ssa_unlink_use_chain(ssa, op_num, op->op1_use);
										op->op1_use = -1;
										op->op1_use_chain = -1;
									}
									MAKE_NOP(opline);
									removed_ops++;
								} else {
									opline->opcode = ZEND_FREE;
									opline->op2.num = 0;
								}
							} else {
								if (opline->op1_type & (IS_CV|IS_CONST)) {
									if (op->op1_use >= 0) {
										zend_ssa_unlink_use_chain(ssa, op_num, op->op1_use);
										op->op1_use = -1;
										op->op1_use_chain = -1;
									}
									opline->opcode = ZEND_JMP;
									opline->op1_type = IS_UNUSED;
									opline->op1.num = opline->op2.num;
								}
							}
							break;
						default:
							break;
					}
				}
			} else if (block->successors_count == 1 && block->successors[0] == next_block_num) {
				op_num = block->start + block->len - 1;
				opline = op_array->opcodes + op_num;
				if (opline->opcode == ZEND_JMP) {
					MAKE_NOP(opline);
					removed_ops++;
				}
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
			zend_ssa_remove_nops(op_array, ssa);
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
	uint32_t flags = 0;
	zend_ssa ssa;

	if (zend_dfa_analyze_op_array(op_array, ctx, &ssa, &flags) != SUCCESS) {
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
