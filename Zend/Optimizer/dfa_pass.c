/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

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

zend_result zend_dfa_analyze_op_array(zend_op_array *op_array, zend_optimizer_ctx *ctx, zend_ssa *ssa)
{
	uint32_t build_flags;

	if (op_array->last_try_catch) {
		/* TODO: we can't analyze functions with try/catch/finally ??? */
		return FAILURE;
	}

    /* Build SSA */
	memset(ssa, 0, sizeof(zend_ssa));

	zend_build_cfg(&ctx->arena, op_array, ZEND_CFG_NO_ENTRY_PREDECESSORS, &ssa->cfg);

	if ((ssa->cfg.flags & ZEND_FUNC_INDIRECT_VAR_ACCESS)) {
		/* TODO: we can't analyze functions with indirect variable access ??? */
		return FAILURE;
	}

	zend_cfg_build_predecessors(&ctx->arena, &ssa->cfg);

	if (ctx->debug_level & ZEND_DUMP_DFA_CFG) {
		zend_dump_op_array(op_array, ZEND_DUMP_CFG, "dfa cfg", &ssa->cfg);
	}

	/* Compute Dominators Tree */
	zend_cfg_compute_dominators_tree(op_array, &ssa->cfg);

	/* Identify reducible and irreducible loops */
	zend_cfg_identify_loops(op_array, &ssa->cfg);

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
	if (zend_build_ssa(&ctx->arena, ctx->script, op_array, build_flags, ssa) == FAILURE) {
		return FAILURE;
	}

	if (ctx->debug_level & ZEND_DUMP_DFA_SSA) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA, "dfa ssa", ssa);
	}


	zend_ssa_compute_use_def_chains(&ctx->arena, op_array, ssa);

	zend_ssa_find_false_dependencies(op_array, ssa);

	zend_ssa_find_sccs(op_array, ssa);

	if (zend_ssa_inference(&ctx->arena, op_array, ctx->script, ssa, ctx->optimization_level) == FAILURE) {
		return FAILURE;
	}

	if (zend_ssa_escape_analysis(ctx->script, op_array, ssa) == FAILURE) {
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
	zend_basic_block *blocks_end = blocks + ssa->cfg.blocks_count;
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

	for (b = blocks; b < blocks_end; b++) {
		if (b->flags & (ZEND_BB_REACHABLE|ZEND_BB_UNREACHABLE_FREE)) {
			if (b->len) {
				uint32_t new_start, old_end;
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

				new_start = target;
				old_end = b->start + b->len;
				while (i < old_end) {
					shiftlist[i] = i - target;
					if (EXPECTED(op_array->opcodes[i].opcode != ZEND_NOP)) {
						if (i != target) {
							op_array->opcodes[target] = op_array->opcodes[i];
							ssa->ops[target] = ssa->ops[i];
							ssa->cfg.map[target] = b - blocks;
						}
						target++;
					}
					i++;
				}
				b->start = new_start;
				if (target != old_end) {
					zend_op *opline;
					zend_op *new_opline;

					b->len = target - b->start;
					opline = op_array->opcodes + old_end - 1;
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
		for (b = blocks; b < blocks_end; b++) {
			if ((b->flags & ZEND_BB_REACHABLE) && b->len != 0) {
				zend_op *opline = op_array->opcodes + b->start + b->len - 1;
				zend_optimizer_shift_jump(op_array, opline, shiftlist);
			}
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

		/* update call graph */
		if (func_info) {
			zend_call_info *call_info = func_info->callee_info;
			while (call_info) {
				call_info->caller_init_opline -=
					shiftlist[call_info->caller_init_opline - op_array->opcodes];
				if (call_info->caller_call_opline) {
					call_info->caller_call_opline -=
						shiftlist[call_info->caller_call_opline - op_array->opcodes];
				}
				call_info = call_info->next_callee;
			}
		}

		op_array->last = target;
	}
	free_alloca(shiftlist, use_heap);
}

static bool safe_instanceof(zend_class_entry *ce1, zend_class_entry *ce2) {
	if (ce1 == ce2) {
		return 1;
	}
	if (!(ce1->ce_flags & ZEND_ACC_LINKED)) {
		/* This case could be generalized, similarly to unlinked_instanceof */
		return 0;
	}
	return instanceof_function(ce1, ce2);
}

static inline bool can_elide_list_type(
	const zend_script *script, const zend_op_array *op_array,
	const zend_ssa_var_info *use_info, zend_type type)
{
	zend_type *single_type;
	/* For intersection: result==false is failure, default is success.
	 * For union: result==true is success, default is failure. */
	bool is_intersection = ZEND_TYPE_IS_INTERSECTION(type);
	ZEND_TYPE_FOREACH(type, single_type) {
		if (ZEND_TYPE_HAS_LIST(*single_type)) {
			ZEND_ASSERT(!is_intersection);
			return can_elide_list_type(script, op_array, use_info, *single_type);
		}
		if (ZEND_TYPE_HAS_NAME(*single_type)) {
			zend_string *lcname = zend_string_tolower(ZEND_TYPE_NAME(*single_type));
			zend_class_entry *ce = zend_optimizer_get_class_entry(script, op_array, lcname);
			zend_string_release(lcname);
			bool result = ce && safe_instanceof(use_info->ce, ce);
			if (result == !is_intersection) {
				return result;
			}
		}
	} ZEND_TYPE_FOREACH_END();
	return is_intersection;
}

static inline bool can_elide_return_type_check(
		const zend_script *script, zend_op_array *op_array, zend_ssa *ssa, zend_ssa_op *ssa_op) {
	zend_arg_info *arg_info = &op_array->arg_info[-1];
	zend_ssa_var_info *use_info = &ssa->var_info[ssa_op->op1_use];
	uint32_t use_type = use_info->type & (MAY_BE_ANY|MAY_BE_UNDEF);
	if (use_type & MAY_BE_REF) {
		return 0;
	}

	if (use_type & MAY_BE_UNDEF) {
		use_type &= ~MAY_BE_UNDEF;
		use_type |= MAY_BE_NULL;
	}

	uint32_t disallowed_types = use_type & ~ZEND_TYPE_PURE_MASK(arg_info->type);
	if (!disallowed_types) {
		/* Only contains allowed types. */
		return true;
	}

	if (disallowed_types == MAY_BE_OBJECT && use_info->ce && ZEND_TYPE_IS_COMPLEX(arg_info->type)) {
		return can_elide_list_type(script, op_array, use_info, arg_info->type);
	}

	return false;
}

static bool opline_supports_assign_contraction(
		zend_op_array *op_array, zend_ssa *ssa, zend_op *opline, int src_var, uint32_t cv_var) {
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

	if ((opline->opcode == ZEND_ASSIGN_OP
	  || opline->opcode == ZEND_ASSIGN_OBJ
	  || opline->opcode == ZEND_ASSIGN_DIM
	  || opline->opcode == ZEND_ASSIGN_OBJ_OP
	  || opline->opcode == ZEND_ASSIGN_DIM_OP)
	 && opline->op1_type == IS_CV
	 && opline->op1.var == cv_var
	 && zend_may_throw(opline, &ssa->ops[ssa->vars[src_var].definition], op_array, ssa)) {
		return 0;
	}

	return 1;
}

static bool variable_defined_or_used_in_range(zend_ssa *ssa, int var, int start, int end)
{
	while (start < end) {
		const zend_ssa_op *ssa_op = &ssa->ops[start];
		if ((ssa_op->op1_def >= 0 && ssa->vars[ssa_op->op1_def].var == var) ||
			(ssa_op->op2_def >= 0 && ssa->vars[ssa_op->op2_def].var == var) ||
			(ssa_op->result_def >= 0 && ssa->vars[ssa_op->result_def].var == var) ||
			(ssa_op->op1_use >= 0 && ssa->vars[ssa_op->op1_use].var == var) ||
			(ssa_op->op2_use >= 0 && ssa->vars[ssa_op->op2_use].var == var) ||
			(ssa_op->result_use >= 0 && ssa->vars[ssa_op->result_use].var == var)
		) {
			return 1;
		}
		start++;
	}
	return 0;
}

int zend_dfa_optimize_calls(zend_op_array *op_array, zend_ssa *ssa)
{
	zend_func_info *func_info = ZEND_FUNC_INFO(op_array);
	int removed_ops = 0;

	if (func_info->callee_info) {
		zend_call_info *call_info = func_info->callee_info;

		do {
			if (call_info->caller_call_opline
			 && call_info->caller_call_opline->opcode == ZEND_DO_ICALL
			 && call_info->callee_func
			 && zend_string_equals_literal(call_info->callee_func->common.function_name, "in_array")
			 && (call_info->caller_init_opline->extended_value == 2
			  || (call_info->caller_init_opline->extended_value == 3
			   && (call_info->caller_call_opline - 1)->opcode == ZEND_SEND_VAL
			   && (call_info->caller_call_opline - 1)->op1_type == IS_CONST))) {

				zend_op *send_array;
				zend_op *send_needly;
				bool strict = 0;
				ZEND_ASSERT(!call_info->is_prototype);

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
					bool ok = 1;

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

						op_num = call_info->caller_call_opline - op_array->opcodes;
						ssa_op = ssa->ops + op_num;
						if (ssa_op->result_def >= 0) {
							int var = ssa_op->result_def;
							int use = ssa->vars[var].use_chain;

							/* If the result is used only in a JMPZ/JMPNZ, replace result type with
							 * IS_TMP_VAR, which will enable use of smart branches. Don't do this
							 * in other cases, as not all opcodes support both VAR and TMP. */
							if (ssa->vars[var].phi_use_chain == NULL
								&& ssa->ops[use].op1_use == var
								&& ssa->ops[use].op1_use_chain == -1
								&& (op_array->opcodes[use].opcode == ZEND_JMPZ
									|| op_array->opcodes[use].opcode == ZEND_JMPNZ)) {
								call_info->caller_call_opline->result_type = IS_TMP_VAR;
								op_array->opcodes[use].op1_type = IS_TMP_VAR;
							}
						}
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

static zend_always_inline void take_successor_ex(zend_ssa *ssa, int block_num, zend_basic_block *block, int target_block)
{
	int i;

	for (i = 0; i < block->successors_count; i++) {
		if (block->successors[i] != target_block) {
			zend_ssa_remove_predecessor(ssa, block_num, block->successors[i]);
		}
	}
	block->successors[0] = target_block;
	block->successors_count = 1;
}

static void compress_block(zend_op_array *op_array, zend_basic_block *block)
{
	while (block->len > 0) {
		zend_op *opline = &op_array->opcodes[block->start + block->len - 1];

		if (opline->opcode == ZEND_NOP) {
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
			if (phi->pi >= 0) {
				if (phi->pi == old_pred || phi->pi == new_pred) {
					zend_ssa_rename_var_uses(
						ssa, phi->ssa_var, phi->sources[0], /* update_types */ 0);
					zend_ssa_remove_phi(ssa, phi);
				}
			} else {
				memmove(
					phi->sources + old_pred_idx,
					phi->sources + old_pred_idx + 1,
					sizeof(int) * (block->predecessors_count - old_pred_idx - 1)
				);
			}
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
			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
			case ZEND_FE_RESET_R:
			case ZEND_FE_RESET_RW:
			case ZEND_JMP_SET:
			case ZEND_COALESCE:
			case ZEND_ASSERT_CHECK:
			case ZEND_JMP_NULL:
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
			case ZEND_FE_FETCH_R:
			case ZEND_FE_FETCH_RW:
				if (ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value) == old->start) {
					opline->extended_value = ZEND_OPLINE_NUM_TO_OFFSET(op_array, opline, dst->start);
				}
				break;
			case ZEND_SWITCH_LONG:
			case ZEND_SWITCH_STRING:
			case ZEND_MATCH:
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
		zend_basic_block *fe_fetch_block = NULL;

		ZEND_ASSERT(block->successors_count == 1);
		predecessors = &ssa->cfg.predecessors[block->predecessor_offset];
		if (block->predecessors_count == 1 && (block->flags & ZEND_BB_FOLLOW)) {
			zend_basic_block *pred_block = &ssa->cfg.blocks[predecessors[0]];

			if (pred_block->len > 0 && (pred_block->flags & ZEND_BB_REACHABLE)) {
				if ((op_array->opcodes[pred_block->start + pred_block->len - 1].opcode == ZEND_FE_FETCH_R
				 || op_array->opcodes[pred_block->start + pred_block->len - 1].opcode == ZEND_FE_FETCH_RW)
				  && op_array->opcodes[pred_block->start + pred_block->len - 1].op2_type == IS_CV) {
					fe_fetch_block = pred_block;
			    }
			}
		}
		for (i = 0; i < block->predecessors_count; i++) {
			zend_ssa_replace_control_link(op_array, ssa, predecessors[i], block_num, block->successors[0]);
		}
		zend_ssa_remove_block(op_array, ssa, block_num);
		if (fe_fetch_block && fe_fetch_block->successors[0] == fe_fetch_block->successors[1]) {
			/* The body of "foreach" loop was removed */
			int ssa_var = ssa->ops[fe_fetch_block->start + fe_fetch_block->len - 1].op2_def;
			if (ssa_var >= 0) {
				zend_ssa_remove_uses_of_var(ssa, ssa_var);
			}
		}
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
		bool can_follow = 1;

		while (next_block_num < ssa->cfg.blocks_count
			&& !(ssa->cfg.blocks[next_block_num].flags & ZEND_BB_REACHABLE)) {
			if (ssa->cfg.blocks[next_block_num].flags & ZEND_BB_UNREACHABLE_FREE) {
				can_follow = 0;
			}
			next_block_num++;
		}

		if (block->len) {
			op_num = block->start + block->len - 1;
			opline = op_array->opcodes + op_num;
			ssa_op = ssa->ops + op_num;

			switch (opline->opcode) {
				case ZEND_JMP:
optimize_jmp:
					if (block->successors[0] == next_block_num && can_follow) {
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
						if (block->successors[0] == next_block_num && can_follow) {
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
						if (block->successors[0] == next_block_num && can_follow) {
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
							zend_ssa_remove_result_def(ssa, ssa_op);
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
				case ZEND_JMP_NULL:
				{
					zend_ssa_var *var = &ssa->vars[ssa_op->result_def];
					if (opline->op1_type == IS_CONST
							&& var->use_chain < 0 && var->phi_use_chain == NULL) {
						if (Z_TYPE_P(CT_CONSTANT_EX(op_array, opline->op1.constant)) == IS_NULL) {
							opline->opcode = ZEND_JMP;
							opline->result_type = IS_UNUSED;
							zend_ssa_remove_result_def(ssa, ssa_op);
							COPY_NODE(opline->op1, opline->op2);
							take_successor_0(ssa, block_num, block);
							goto optimize_jmp;
						} else {
							zend_ssa_remove_result_def(ssa, ssa_op);
							MAKE_NOP(opline);
							removed_ops++;
							take_successor_1(ssa, block_num, block);
							goto optimize_nop;
						}
					}
					break;
				}
				case ZEND_SWITCH_LONG:
				case ZEND_SWITCH_STRING:
				case ZEND_MATCH:
					if (opline->op1_type == IS_CONST) {
						zval *zv = CT_CONSTANT_EX(op_array, opline->op1.constant);
						zend_uchar type = Z_TYPE_P(zv);
						bool correct_type =
							(opline->opcode == ZEND_SWITCH_LONG && type == IS_LONG)
							|| (opline->opcode == ZEND_SWITCH_STRING && type == IS_STRING)
							|| (opline->opcode == ZEND_MATCH && (type == IS_LONG || type == IS_STRING));

						/* Switch statements have a fallback chain for loose comparison. In those
						 * cases the SWITCH_* instruction is a NOP. Match does strict comparison and
						 * thus jumps to the default branch on mismatched types, so we need to
						 * convert MATCH to a jmp. */
						if (!correct_type && opline->opcode != ZEND_MATCH) {
							removed_ops++;
							MAKE_NOP(opline);
							opline->extended_value = 0;
							take_successor_ex(ssa, block_num, block, block->successors[block->successors_count - 1]);
							goto optimize_nop;
						}

						uint32_t target;
						if (correct_type) {
							HashTable *jmptable = Z_ARRVAL_P(CT_CONSTANT_EX(op_array, opline->op2.constant));
							zval *jmp_zv = type == IS_LONG
								? zend_hash_index_find(jmptable, Z_LVAL_P(zv))
								: zend_hash_find(jmptable, Z_STR_P(zv));

							if (jmp_zv) {
								target = ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, Z_LVAL_P(jmp_zv));
							} else {
								target = ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value);
							}
						} else {
							ZEND_ASSERT(opline->opcode == ZEND_MATCH);
							target = ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, opline->extended_value);
						}
						opline->opcode = ZEND_JMP;
						opline->extended_value = 0;
						SET_UNUSED(opline->op1);
						ZEND_SET_OP_JMP_ADDR(opline, opline->op1, op_array->opcodes + target);
						SET_UNUSED(opline->op2);
						take_successor_ex(ssa, block_num, block, ssa->cfg.map[target]);
						goto optimize_jmp;
					}
					break;
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

static bool zend_dfa_try_to_replace_result(zend_op_array *op_array, zend_ssa *ssa, int def, int cv_var)
{
	int result_var = ssa->ops[def].result_def;
	int cv = EX_NUM_TO_VAR(ssa->vars[cv_var].var);

	if (result_var >= 0
	 && !(ssa->var_info[cv_var].type & MAY_BE_REF)
	 && ssa->vars[cv_var].alias == NO_ALIAS
	 && ssa->vars[result_var].phi_use_chain == NULL
	 && ssa->vars[result_var].sym_use_chain == NULL) {
		int use = ssa->vars[result_var].use_chain;

		if (use >= 0
		 && zend_ssa_next_use(ssa->ops, result_var, use) < 0
		 && op_array->opcodes[use].opcode != ZEND_FREE
		 && op_array->opcodes[use].opcode != ZEND_SEND_VAL
		 && op_array->opcodes[use].opcode != ZEND_SEND_VAL_EX
		 && op_array->opcodes[use].opcode != ZEND_VERIFY_RETURN_TYPE
		 && op_array->opcodes[use].opcode != ZEND_YIELD) {
			if (use > def) {
				int i = use;
				const zend_op *opline = &op_array->opcodes[use];

				while (i > def) {
					if ((opline->op1_type == IS_CV && opline->op1.var == cv)
					 || (opline->op2_type == IS_CV && opline->op2.var == cv)
					 || (opline->result_type == IS_CV && opline->result.var == cv)) {
						return 0;
					}
					opline--;
					i--;
				}

				/* Update opcodes and reconstruct SSA */
				ssa->vars[result_var].definition = -1;
				ssa->vars[result_var].use_chain = -1;
				ssa->ops[def].result_def = -1;

				op_array->opcodes[def].result_type = IS_UNUSED;
				op_array->opcodes[def].result.var = 0;

				if (ssa->ops[use].op1_use == result_var) {
					ssa->ops[use].op1_use = cv_var;
					ssa->ops[use].op1_use_chain = ssa->vars[cv_var].use_chain;
					ssa->vars[cv_var].use_chain = use;

					op_array->opcodes[use].op1_type = IS_CV;
					op_array->opcodes[use].op1.var = cv;
				} else if (ssa->ops[use].op2_use == result_var) {
					ssa->ops[use].op2_use = cv_var;
					ssa->ops[use].op2_use_chain = ssa->vars[cv_var].use_chain;
					ssa->vars[cv_var].use_chain = use;

					op_array->opcodes[use].op2_type = IS_CV;
					op_array->opcodes[use].op2.var = cv;
				} else if (ssa->ops[use].result_use == result_var) {
					ssa->ops[use].result_use = cv_var;
					ssa->ops[use].res_use_chain = ssa->vars[cv_var].use_chain;
					ssa->vars[cv_var].use_chain = use;

					op_array->opcodes[use].result_type = IS_CV;
					op_array->opcodes[use].result.var = cv;
				}

				return 1;
			}
		}
	}

	return 0;
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
		zend_ssa_op *ssa_op;
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
			if (dce_optimize_op_array(op_array, ctx, ssa, 0)) {
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
			ssa_op = &ssa->ops[op_1];

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

					if (opline->op1_type == IS_CONST && opline->op2_type != IS_CONST) {
						zval *zv = CT_CONSTANT_EX(op_array, opline->op1.constant);

						if ((OP2_INFO() & MAY_BE_ANY) == MAY_BE_DOUBLE
						 && Z_TYPE_INFO_P(zv) == IS_LONG) {

// op_1: #v.? = ADD long(?), #?.? [double] => #v.? = ADD double(?), #?.? [double]

							ZVAL_DOUBLE(&tmp, zval_get_double(zv));
							opline->op1.constant = zend_optimizer_add_literal(op_array, &tmp);
							zv = CT_CONSTANT_EX(op_array, opline->op1.constant);
						}
						if (opline->opcode == ZEND_ADD) {
							zv = CT_CONSTANT_EX(op_array, opline->op1.constant);

							if (((OP2_INFO() & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_LONG
							  && Z_TYPE_INFO_P(zv) == IS_LONG
							  && Z_LVAL_P(zv) == 0)
							 || ((OP2_INFO() & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_DOUBLE
							  && Z_TYPE_INFO_P(zv) == IS_DOUBLE
							  && Z_DVAL_P(zv) == 0.0)) {

// op_1: #v.? = ADD 0, #?.? [double,long] => #v.? = QM_ASSIGN #?.?

								opline->opcode = ZEND_QM_ASSIGN;
								opline->op1_type = opline->op2_type;
								opline->op1.var = opline->op2.var;
								opline->op2_type = IS_UNUSED;
								opline->op2.num = 0;
								ssa->ops[op_1].op1_use = ssa->ops[op_1].op2_use;
								ssa->ops[op_1].op1_use_chain = ssa->ops[op_1].op2_use_chain;
								ssa->ops[op_1].op2_use = -1;
								ssa->ops[op_1].op2_use_chain = -1;
							}
						} else if (opline->opcode == ZEND_MUL
						 && (OP2_INFO() & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) == 0) {
							zv = CT_CONSTANT_EX(op_array, opline->op1.constant);

							if ((Z_TYPE_INFO_P(zv) == IS_LONG
							  && Z_LVAL_P(zv) == 2)
							 || (Z_TYPE_INFO_P(zv) == IS_DOUBLE
							  && Z_DVAL_P(zv) == 2.0
							  && !(OP2_INFO() & MAY_BE_LONG))) {

// op_1: #v.? = MUL 2, #x.? [double,long] => #v.? = ADD #x.?, #x.?

								opline->opcode = ZEND_ADD;
								opline->op1_type = opline->op2_type;
								opline->op1.var = opline->op2.var;
								ssa->ops[op_1].op1_use = ssa->ops[op_1].op2_use;
								ssa->ops[op_1].op1_use_chain = ssa->ops[op_1].op2_use_chain;
							}
						}
					} else if (opline->op1_type != IS_CONST && opline->op2_type == IS_CONST) {
						zval *zv = CT_CONSTANT_EX(op_array, opline->op2.constant);

						if ((OP1_INFO() & MAY_BE_ANY) == MAY_BE_DOUBLE
						 && Z_TYPE_INFO_P(CT_CONSTANT_EX(op_array, opline->op2.constant)) == IS_LONG) {

// op_1: #v.? = ADD #?.? [double], long(?) => #v.? = ADD #?.? [double], double(?)

							ZVAL_DOUBLE(&tmp, zval_get_double(zv));
							opline->op2.constant = zend_optimizer_add_literal(op_array, &tmp);
							zv = CT_CONSTANT_EX(op_array, opline->op2.constant);
						}
						if (opline->opcode == ZEND_ADD || opline->opcode == ZEND_SUB) {
							if (((OP1_INFO() & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_LONG
							  && Z_TYPE_INFO_P(zv) == IS_LONG
							  && Z_LVAL_P(zv) == 0)
							 || ((OP1_INFO() & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_DOUBLE
							  && Z_TYPE_INFO_P(zv) == IS_DOUBLE
							  && Z_DVAL_P(zv) == 0.0)) {

// op_1: #v.? = ADD #?.? [double,long], 0 => #v.? = QM_ASSIGN #?.?

								opline->opcode = ZEND_QM_ASSIGN;
								opline->op2_type = IS_UNUSED;
								opline->op2.num = 0;
							}
						} else if (opline->opcode == ZEND_MUL
						 && (OP1_INFO() & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) == 0) {
							zv = CT_CONSTANT_EX(op_array, opline->op2.constant);

							if ((Z_TYPE_INFO_P(zv) == IS_LONG
							  && Z_LVAL_P(zv) == 2)
							 || (Z_TYPE_INFO_P(zv) == IS_DOUBLE
							  && Z_DVAL_P(zv) == 2.0
							  && !(OP1_INFO() & MAY_BE_LONG))) {

// op_1: #v.? = MUL #x.? [double,long], 2 => #v.? = ADD #x.?, #x.?

								opline->opcode = ZEND_ADD;
								opline->op2_type = opline->op1_type;
								opline->op2.var = opline->op1.var;
								ssa->ops[op_1].op2_use = ssa->ops[op_1].op1_use;
								ssa->ops[op_1].op2_use_chain = ssa->ops[op_1].op1_use_chain;
							}
						}
					}
				} else if (opline->opcode == ZEND_CONCAT) {
					if (!(OP1_INFO() & MAY_BE_OBJECT)
					 && !(OP2_INFO() & MAY_BE_OBJECT)) {
						opline->opcode = ZEND_FAST_CONCAT;
					}
				} else if (opline->opcode == ZEND_VERIFY_RETURN_TYPE
				 && opline->op1_type != IS_CONST
				 && ssa->ops[op_1].op1_def == v
				 && ssa->ops[op_1].op1_use >= 0) {
					int orig_var = ssa->ops[op_1].op1_use;
					int ret = ssa->vars[v].use_chain;

					if (ssa->ops[op_1].op1_use_chain == -1
					 && can_elide_return_type_check(ctx->script, op_array, ssa, &ssa->ops[op_1])) {

// op_1: VERIFY_RETURN_TYPE #orig_var.? [T] -> #v.? [T] => NOP

						zend_ssa_unlink_use_chain(ssa, op_1, orig_var);

						if (ret >= 0) {
							ssa->ops[ret].op1_use = orig_var;
							ssa->ops[ret].op1_use_chain = ssa->vars[orig_var].use_chain;
							ssa->vars[orig_var].use_chain = ret;
						}

						ssa->vars[v].definition = -1;
						ssa->vars[v].use_chain = -1;

						ssa->ops[op_1].op1_def = -1;
						ssa->ops[op_1].op1_use = -1;

						MAKE_NOP(opline);
						remove_nops = 1;
					} else if (ret >= 0
					 && ssa->ops[ret].op1_use == v
					 && ssa->ops[ret].op1_use_chain == -1
					 && can_elide_return_type_check(ctx->script, op_array, ssa, &ssa->ops[op_1])) {

// op_1: VERIFY_RETURN_TYPE #orig_var.? [T] -> #v.? [T] => NOP

						zend_ssa_replace_use_chain(ssa, op_1, ret, orig_var);

						ssa->ops[ret].op1_use = orig_var;
						ssa->ops[ret].op1_use_chain = ssa->ops[op_1].op1_use_chain;

						ssa->vars[v].definition = -1;
						ssa->vars[v].use_chain = -1;

						ssa->ops[op_1].op1_def = -1;
						ssa->ops[op_1].op1_use = -1;

						MAKE_NOP(opline);
						remove_nops = 1;
					}
				}
			}

			if (opline->opcode == ZEND_QM_ASSIGN
			 && ssa->ops[op_1].result_def == v
			 && opline->op1_type & (IS_TMP_VAR|IS_VAR)
			 && !(ssa->var_info[v].type & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))
			) {

				int src_var = ssa->ops[op_1].op1_use;

				if (src_var >= 0
				 && !(ssa->var_info[src_var].type & MAY_BE_REF)
				 && (ssa->var_info[src_var].type & (MAY_BE_UNDEF|MAY_BE_ANY))
				 && ssa->vars[src_var].definition >= 0
				 && ssa->ops[ssa->vars[src_var].definition].result_def == src_var
				 && ssa->ops[ssa->vars[src_var].definition].result_use < 0
				 && ssa->vars[src_var].use_chain == op_1
				 && ssa->ops[op_1].op1_use_chain < 0
				 && !ssa->vars[src_var].phi_use_chain
				 && !ssa->vars[src_var].sym_use_chain
				 && opline_supports_assign_contraction(
					 op_array, ssa, &op_array->opcodes[ssa->vars[src_var].definition],
					 src_var, opline->result.var)
				 && !variable_defined_or_used_in_range(ssa, EX_VAR_TO_NUM(opline->result.var),
						ssa->vars[src_var].definition+1, op_1)
				) {

					int orig_var = ssa->ops[op_1].result_use;
					int op_2 = ssa->vars[src_var].definition;

// op_2: #src_var.T = OP ...                                        => #v.CV = OP ...
// op_1: QM_ASSIGN #src_var.T #orig_var.CV [undef,scalar] -> #v.CV,    NOP

					if (orig_var >= 0) {
						zend_ssa_unlink_use_chain(ssa, op_1, orig_var);
					}

					/* Reconstruct SSA */
					ssa->vars[v].definition = op_2;
					ssa->ops[op_2].result_def = v;

					ssa->vars[src_var].definition = -1;
					ssa->vars[src_var].use_chain = -1;

					ssa->ops[op_1].op1_use = -1;
					ssa->ops[op_1].op1_def = -1;
					ssa->ops[op_1].op1_use_chain = -1;
					ssa->ops[op_1].result_use = -1;
					ssa->ops[op_1].result_def = -1;
					ssa->ops[op_1].res_use_chain = -1;

					/* Update opcodes */
					op_array->opcodes[op_2].result_type = opline->result_type;
					op_array->opcodes[op_2].result.var = opline->result.var;

					MAKE_NOP(opline);
					remove_nops = 1;

					if (op_array->opcodes[op_2].opcode == ZEND_SUB
					 && op_array->opcodes[op_2].op1_type == op_array->opcodes[op_2].result_type
					 && op_array->opcodes[op_2].op1.var == op_array->opcodes[op_2].result.var
					 && op_array->opcodes[op_2].op2_type == IS_CONST
					 && Z_TYPE_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op_2].op2.constant)) == IS_LONG
					 && Z_LVAL_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op_2].op2.constant)) == 1
					 && ssa->ops[op_2].op1_use >= 0
					 && !(ssa->var_info[ssa->ops[op_2].op1_use].type & (MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {

						op_array->opcodes[op_2].opcode = ZEND_PRE_DEC;
						SET_UNUSED(op_array->opcodes[op_2].op2);
						SET_UNUSED(op_array->opcodes[op_2].result);

						ssa->ops[op_2].result_def = -1;
						ssa->ops[op_2].op1_def = v;

					} else if (op_array->opcodes[op_2].opcode == ZEND_ADD
					 && op_array->opcodes[op_2].op1_type == op_array->opcodes[op_2].result_type
					 && op_array->opcodes[op_2].op1.var == op_array->opcodes[op_2].result.var
					 && op_array->opcodes[op_2].op2_type == IS_CONST
					 && Z_TYPE_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op_2].op2.constant)) == IS_LONG
					 && Z_LVAL_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op_2].op2.constant)) == 1
					 && ssa->ops[op_2].op1_use >= 0
					 && !(ssa->var_info[ssa->ops[op_2].op1_use].type & (MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {

						op_array->opcodes[op_2].opcode = ZEND_PRE_INC;
						SET_UNUSED(op_array->opcodes[op_2].op2);
						SET_UNUSED(op_array->opcodes[op_2].result);

						ssa->ops[op_2].result_def = -1;
						ssa->ops[op_2].op1_def = v;

					} else if (op_array->opcodes[op_2].opcode == ZEND_ADD
					 && op_array->opcodes[op_2].op2_type == op_array->opcodes[op_2].result_type
					 && op_array->opcodes[op_2].op2.var == op_array->opcodes[op_2].result.var
					 && op_array->opcodes[op_2].op1_type == IS_CONST
					 && Z_TYPE_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op_2].op1.constant)) == IS_LONG
					 && Z_LVAL_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op_2].op1.constant)) == 1
					 && ssa->ops[op_2].op2_use >= 0
					 && !(ssa->var_info[ssa->ops[op_2].op2_use].type & (MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {

						op_array->opcodes[op_2].opcode = ZEND_PRE_INC;
						op_array->opcodes[op_2].op1_type = op_array->opcodes[op_2].op2_type;
						op_array->opcodes[op_2].op1.var = op_array->opcodes[op_2].op2.var;
						SET_UNUSED(op_array->opcodes[op_2].op2);
						SET_UNUSED(op_array->opcodes[op_2].result);

						ssa->ops[op_2].result_def = -1;
						ssa->ops[op_2].op1_def = v;
						ssa->ops[op_2].op1_use = ssa->ops[op_2].op2_use;
						ssa->ops[op_2].op1_use_chain = ssa->ops[op_2].op2_use_chain;
						ssa->ops[op_2].op2_use = -1;
						ssa->ops[op_2].op2_use_chain = -1;
					}
				}
			}

			if (ssa->vars[v].var >= op_array->last_var) {
				/* skip TMP and VAR */
				continue;
			}

			if (ssa->ops[op_1].op1_def == v
			 && RETURN_VALUE_USED(opline)) {
				if (opline->opcode == ZEND_ASSIGN
				 || opline->opcode == ZEND_ASSIGN_OP
				 || opline->opcode == ZEND_PRE_INC
				 || opline->opcode == ZEND_PRE_DEC) {
					zend_dfa_try_to_replace_result(op_array, ssa, op_1, v);
				} else if (opline->opcode == ZEND_POST_INC) {
					int result_var = ssa->ops[op_1].result_def;

					if (result_var >= 0
					 && (ssa->var_info[result_var].type & ((MAY_BE_ANY|MAY_BE_REF|MAY_BE_UNDEF) - (MAY_BE_LONG|MAY_BE_DOUBLE))) == 0) {
						int use = ssa->vars[result_var].use_chain;

						if (use >= 0 && op_array->opcodes[use].opcode == ZEND_IS_SMALLER
						 && ssa->ops[use].op1_use == result_var
						 && zend_dfa_try_to_replace_result(op_array, ssa, op_1, v)) {
							opline->opcode = ZEND_PRE_INC;
							op_array->opcodes[use].opcode = ZEND_IS_SMALLER_OR_EQUAL;
						}
					}
				} else if (opline->opcode == ZEND_POST_DEC) {
					int result_var = ssa->ops[op_1].result_def;

					if (result_var >= 0
					 && (ssa->var_info[result_var].type & ((MAY_BE_ANY|MAY_BE_REF|MAY_BE_UNDEF) - (MAY_BE_LONG|MAY_BE_DOUBLE))) == 0) {
						int use = ssa->vars[result_var].use_chain;

						if (use >= 0 && op_array->opcodes[use].opcode == ZEND_IS_SMALLER
						 && ssa->ops[use].op2_use == result_var
						 && zend_dfa_try_to_replace_result(op_array, ssa, op_1, v)) {
							opline->opcode = ZEND_PRE_DEC;
							op_array->opcodes[use].opcode = ZEND_IS_SMALLER_OR_EQUAL;
						}
					}
				}
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
					 && (ssa->var_info[src_var].type & (MAY_BE_UNDEF|MAY_BE_ANY))
					 && ssa->vars[src_var].definition >= 0
					 && ssa->ops[ssa->vars[src_var].definition].result_def == src_var
					 && ssa->ops[ssa->vars[src_var].definition].result_use < 0
					 && ssa->vars[src_var].use_chain == op_1
					 && ssa->ops[op_1].op2_use_chain < 0
					 && !ssa->vars[src_var].phi_use_chain
					 && !ssa->vars[src_var].sym_use_chain
					 && opline_supports_assign_contraction(
						 op_array, ssa, &op_array->opcodes[ssa->vars[src_var].definition],
						 src_var, opline->op1.var)
					 && !variable_defined_or_used_in_range(ssa, EX_VAR_TO_NUM(opline->op1.var),
							ssa->vars[src_var].definition+1, op_1)
					) {

						int op_2 = ssa->vars[src_var].definition;

// op_2: #src_var.T = OP ...                                     => #v.CV = OP ...
// op_1: ASSIGN #orig_var.CV [undef,scalar] -> #v.CV, #src_var.T    NOP

						zend_ssa_unlink_use_chain(ssa, op_1, orig_var);
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

						if (op_array->opcodes[op_2].opcode == ZEND_SUB
						 && op_array->opcodes[op_2].op1_type == op_array->opcodes[op_2].result_type
						 && op_array->opcodes[op_2].op1.var == op_array->opcodes[op_2].result.var
						 && op_array->opcodes[op_2].op2_type == IS_CONST
						 && Z_TYPE_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op_2].op2.constant)) == IS_LONG
						 && Z_LVAL_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op_2].op2.constant)) == 1
						 && ssa->ops[op_2].op1_use >= 0
						 && !(ssa->var_info[ssa->ops[op_2].op1_use].type & (MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {

							op_array->opcodes[op_2].opcode = ZEND_PRE_DEC;
							SET_UNUSED(op_array->opcodes[op_2].op2);
							SET_UNUSED(op_array->opcodes[op_2].result);

							ssa->ops[op_2].result_def = -1;
							ssa->ops[op_2].op1_def = v;

						} else if (op_array->opcodes[op_2].opcode == ZEND_ADD
						 && op_array->opcodes[op_2].op1_type == op_array->opcodes[op_2].result_type
						 && op_array->opcodes[op_2].op1.var == op_array->opcodes[op_2].result.var
						 && op_array->opcodes[op_2].op2_type == IS_CONST
						 && Z_TYPE_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op_2].op2.constant)) == IS_LONG
						 && Z_LVAL_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op_2].op2.constant)) == 1
						 && ssa->ops[op_2].op1_use >= 0
						 && !(ssa->var_info[ssa->ops[op_2].op1_use].type & (MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {

							op_array->opcodes[op_2].opcode = ZEND_PRE_INC;
							SET_UNUSED(op_array->opcodes[op_2].op2);
							SET_UNUSED(op_array->opcodes[op_2].result);

							ssa->ops[op_2].result_def = -1;
							ssa->ops[op_2].op1_def = v;

						} else if (op_array->opcodes[op_2].opcode == ZEND_ADD
						 && op_array->opcodes[op_2].op2_type == op_array->opcodes[op_2].result_type
						 && op_array->opcodes[op_2].op2.var == op_array->opcodes[op_2].result.var
						 && op_array->opcodes[op_2].op1_type == IS_CONST
						 && Z_TYPE_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op_2].op1.constant)) == IS_LONG
						 && Z_LVAL_P(CT_CONSTANT_EX(op_array, op_array->opcodes[op_2].op1.constant)) == 1
						 && ssa->ops[op_2].op2_use >= 0
						 && !(ssa->var_info[ssa->ops[op_2].op2_use].type & (MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {

							op_array->opcodes[op_2].opcode = ZEND_PRE_INC;
							op_array->opcodes[op_2].op1_type = op_array->opcodes[op_2].op2_type;
							op_array->opcodes[op_2].op1.var = op_array->opcodes[op_2].op2.var;
							SET_UNUSED(op_array->opcodes[op_2].op2);
							SET_UNUSED(op_array->opcodes[op_2].result);

							ssa->ops[op_2].result_def = -1;
							ssa->ops[op_2].op1_def = v;
							ssa->ops[op_2].op1_use = ssa->ops[op_2].op2_use;
							ssa->ops[op_2].op1_use_chain = ssa->ops[op_2].op2_use_chain;
							ssa->ops[op_2].op2_use = -1;
							ssa->ops[op_2].op2_use_chain = -1;
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

			} else if (opline->opcode == ZEND_ASSIGN_OP
			 && opline->extended_value == ZEND_ADD
			 && ssa->ops[op_1].op1_def == v
			 && opline->op2_type == IS_CONST
			 && Z_TYPE_P(CT_CONSTANT_EX(op_array, opline->op2.constant)) == IS_LONG
			 && Z_LVAL_P(CT_CONSTANT_EX(op_array, opline->op2.constant)) == 1
			 && ssa->ops[op_1].op1_use >= 0
			 && !(ssa->var_info[ssa->ops[op_1].op1_use].type & (MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {

// op_1: ASSIGN_ADD #?.CV [undef,null,int,foat] ->#v.CV, int(1) => PRE_INC #?.CV ->#v.CV

				opline->opcode = ZEND_PRE_INC;
				opline->extended_value = 0;
				SET_UNUSED(opline->op2);

			} else if (opline->opcode == ZEND_ASSIGN_OP
			 && opline->extended_value == ZEND_SUB
			 && ssa->ops[op_1].op1_def == v
			 && opline->op2_type == IS_CONST
			 && Z_TYPE_P(CT_CONSTANT_EX(op_array, opline->op2.constant)) == IS_LONG
			 && Z_LVAL_P(CT_CONSTANT_EX(op_array, opline->op2.constant)) == 1
			 && ssa->ops[op_1].op1_use >= 0
			 && !(ssa->var_info[ssa->ops[op_1].op1_use].type & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {

// op_1: ASSIGN_SUB #?.CV [undef,null,int,foat] -> #v.CV, int(1) => PRE_DEC #?.CV ->#v.CV

				opline->opcode = ZEND_PRE_DEC;
				opline->extended_value = 0;
				SET_UNUSED(opline->op2);

			} else if (ssa->ops[op_1].op1_def == v
			 && !RETURN_VALUE_USED(opline)
			 && ssa->ops[op_1].op1_use >= 0
			 && !(ssa->var_info[ssa->ops[op_1].op1_use].type & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))
			 && opline->opcode == ZEND_ASSIGN_OP
			 && opline->extended_value != ZEND_CONCAT) {

// op_1: ASSIGN_OP #orig_var.CV [undef,null,bool,int,double] -> #v.CV, ? => #v.CV = ADD #orig_var.CV, ?

				/* Reconstruct SSA */
				ssa->ops[op_1].result_def = ssa->ops[op_1].op1_def;
				ssa->ops[op_1].op1_def = -1;

				/* Update opcode */
				opline->opcode = opline->extended_value;
				opline->extended_value = 0;
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

	if (zend_dfa_analyze_op_array(op_array, ctx, &ssa) == FAILURE) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	zend_dfa_optimize_op_array(op_array, ctx, &ssa, NULL);

	/* Destroy SSA */
	zend_arena_release(&ctx->arena, checkpoint);
}
