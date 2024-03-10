/*
   +----------------------------------------------------------------------+
   | Zend Engine, DFG - Data Flow Graph                                   |
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

#include "zend_compile.h"
#include "zend_dfg.h"

static zend_always_inline void _zend_dfg_add_use_def_op(const zend_op_array *op_array, const zend_op *opline, uint32_t build_flags, zend_bitset use, zend_bitset def) /* {{{ */
{
	uint32_t var_num;
	const zend_op *next;

	if (opline->op1_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
		var_num = EX_VAR_TO_NUM(opline->op1.var);
		if (!zend_bitset_in(def, var_num)) {
			zend_bitset_incl(use, var_num);
		}
	}
	if (((opline->op2_type & (IS_VAR|IS_TMP_VAR)) != 0
	  && opline->opcode != ZEND_FE_FETCH_R
	  && opline->opcode != ZEND_FE_FETCH_RW)
	 || (opline->op2_type == IS_CV)) {
		var_num = EX_VAR_TO_NUM(opline->op2.var);
		if (!zend_bitset_in(def, var_num)) {
			zend_bitset_incl(use, var_num);
		}
	}
	if ((build_flags & ZEND_SSA_USE_CV_RESULTS)
	 && opline->result_type == IS_CV
	 && opline->opcode != ZEND_RECV) {
		var_num = EX_VAR_TO_NUM(opline->result.var);
		if (!zend_bitset_in(def, var_num)) {
			zend_bitset_incl(use, var_num);
		}
	}

	switch (opline->opcode) {
		case ZEND_ASSIGN:
			if ((build_flags & ZEND_SSA_RC_INFERENCE) && opline->op2_type == IS_CV) {
				zend_bitset_incl(def, EX_VAR_TO_NUM(opline->op2.var));
			}
			if (opline->op1_type == IS_CV) {
add_op1_def:
				zend_bitset_incl(def, EX_VAR_TO_NUM(opline->op1.var));
			}
			break;
		case ZEND_ASSIGN_REF:
			if (opline->op2_type == IS_CV) {
				zend_bitset_incl(def, EX_VAR_TO_NUM(opline->op2.var));
			}
			if (opline->op1_type == IS_CV) {
				goto add_op1_def;
			}
			break;
		case ZEND_ASSIGN_DIM:
		case ZEND_ASSIGN_OBJ:
			next = opline + 1;
			if (next->op1_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
				var_num = EX_VAR_TO_NUM(next->op1.var);
				if (!zend_bitset_in(def, var_num)) {
					zend_bitset_incl(use, var_num);
				}
				if (build_flags & ZEND_SSA_RC_INFERENCE && next->op1_type == IS_CV) {
					zend_bitset_incl(def, var_num);
				}
			}
			if (opline->op1_type == IS_CV) {
				goto add_op1_def;
			}
			break;
		case ZEND_ASSIGN_OBJ_REF:
			next = opline + 1;
			if (next->op1_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
				var_num = EX_VAR_TO_NUM(next->op1.var);
				if (!zend_bitset_in(def, var_num)) {
					zend_bitset_incl(use, var_num);
				}
				if (next->op1_type == IS_CV) {
					zend_bitset_incl(def, var_num);
				}
			}
			if (opline->op1_type == IS_CV) {
				goto add_op1_def;
			}
			break;
		case ZEND_ASSIGN_STATIC_PROP:
			next = opline + 1;
			if (next->op1_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
				var_num = EX_VAR_TO_NUM(next->op1.var);
				if (!zend_bitset_in(def, var_num)) {
					zend_bitset_incl(use, var_num);
				}
				if ((build_flags & ZEND_SSA_RC_INFERENCE) && next->op1_type == IS_CV) {
					zend_bitset_incl(def, var_num);
				}
			}
			break;
		case ZEND_ASSIGN_STATIC_PROP_REF:
			next = opline + 1;
			if (next->op1_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
				var_num = EX_VAR_TO_NUM(next->op1.var);
				if (!zend_bitset_in(def, var_num)) {
					zend_bitset_incl(use, var_num);
				}
				if (next->op1_type == IS_CV) {
					zend_bitset_incl(def, var_num);
				}
			}
			break;
		case ZEND_ASSIGN_STATIC_PROP_OP:
		case ZEND_FRAMELESS_ICALL_3:
			next = opline + 1;
			if (next->op1_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
				var_num = EX_VAR_TO_NUM(next->op1.var);
				if (!zend_bitset_in(def, var_num)) {
					zend_bitset_incl(use, var_num);
				}
			}
			break;
		case ZEND_ASSIGN_DIM_OP:
		case ZEND_ASSIGN_OBJ_OP:
			next = opline + 1;
			if (next->op1_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
				var_num = EX_VAR_TO_NUM(next->op1.var);
				if (!zend_bitset_in(def, var_num)) {
					zend_bitset_incl(use, var_num);
				}
			}
			if (opline->op1_type == IS_CV) {
				goto add_op1_def;
			}
			break;
		case ZEND_ASSIGN_OP:
		case ZEND_PRE_INC:
		case ZEND_PRE_DEC:
		case ZEND_POST_INC:
		case ZEND_POST_DEC:
		case ZEND_BIND_GLOBAL:
		case ZEND_BIND_STATIC:
		case ZEND_BIND_INIT_STATIC_OR_JMP:
		case ZEND_SEND_VAR_NO_REF:
		case ZEND_SEND_VAR_NO_REF_EX:
		case ZEND_SEND_VAR_EX:
		case ZEND_SEND_FUNC_ARG:
		case ZEND_SEND_REF:
		case ZEND_SEND_UNPACK:
		case ZEND_FE_RESET_RW:
		case ZEND_MAKE_REF:
		case ZEND_PRE_INC_OBJ:
		case ZEND_PRE_DEC_OBJ:
		case ZEND_POST_INC_OBJ:
		case ZEND_POST_DEC_OBJ:
		case ZEND_UNSET_DIM:
		case ZEND_UNSET_OBJ:
		case ZEND_FETCH_DIM_W:
		case ZEND_FETCH_DIM_RW:
		case ZEND_FETCH_DIM_FUNC_ARG:
		case ZEND_FETCH_DIM_UNSET:
		case ZEND_FETCH_LIST_W:
			if (opline->op1_type == IS_CV) {
				goto add_op1_def;
			}
			break;
		case ZEND_SEND_VAR:
		case ZEND_CAST:
		case ZEND_QM_ASSIGN:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_FE_RESET_R:
			if ((build_flags & ZEND_SSA_RC_INFERENCE) && opline->op1_type == IS_CV) {
				goto add_op1_def;
			}
			break;
		case ZEND_ADD_ARRAY_UNPACK:
			var_num = EX_VAR_TO_NUM(opline->result.var);
			if (!zend_bitset_in(def, var_num)) {
				zend_bitset_incl(use, var_num);
			}
			break;
		case ZEND_ADD_ARRAY_ELEMENT:
			var_num = EX_VAR_TO_NUM(opline->result.var);
			if (!zend_bitset_in(def, var_num)) {
				zend_bitset_incl(use, var_num);
			}
			ZEND_FALLTHROUGH;
		case ZEND_INIT_ARRAY:
			if (((build_flags & ZEND_SSA_RC_INFERENCE)
						|| (opline->extended_value & ZEND_ARRAY_ELEMENT_REF))
					&& opline->op1_type == IS_CV) {
				goto add_op1_def;
			}
			break;
		case ZEND_YIELD:
			if (opline->op1_type == IS_CV
					&& ((op_array->fn_flags & ZEND_ACC_RETURN_REFERENCE)
						|| (build_flags & ZEND_SSA_RC_INFERENCE))) {
				goto add_op1_def;
			}
			break;
		case ZEND_UNSET_CV:
			goto add_op1_def;
		case ZEND_VERIFY_RETURN_TYPE:
			if (opline->op1_type & (IS_TMP_VAR|IS_VAR|IS_CV)) {
				goto add_op1_def;
			}
			break;
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
#if 0
			/* This special case was handled above the switch */
			if (opline->op2_type != IS_CV) {
				op2_use = -1; /* not used */
			}
#endif
			zend_bitset_incl(def, EX_VAR_TO_NUM(opline->op2.var));
			break;
		case ZEND_BIND_LEXICAL:
			if ((opline->extended_value & ZEND_BIND_REF) || (build_flags & ZEND_SSA_RC_INFERENCE)) {
				zend_bitset_incl(def, EX_VAR_TO_NUM(opline->op2.var));
			}
			break;
		default:
			break;
	}

	if (opline->result_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
		zend_bitset_incl(def, EX_VAR_TO_NUM(opline->result.var));
	}
}
/* }}} */

ZEND_API void zend_dfg_add_use_def_op(const zend_op_array *op_array, const zend_op *opline, uint32_t build_flags, zend_bitset use, zend_bitset def) /* {{{ */
{
	_zend_dfg_add_use_def_op(op_array, opline, build_flags, use, def);
}
/* }}} */

void zend_build_dfg(const zend_op_array *op_array, const zend_cfg *cfg, zend_dfg *dfg, uint32_t build_flags) /* {{{ */
{
	int set_size;
	zend_basic_block *blocks = cfg->blocks;
	int blocks_count = cfg->blocks_count;
	zend_bitset tmp, def, use, in, out;
	int k;
	int j;

	set_size = dfg->size;
	tmp = dfg->tmp;
	def = dfg->def;
	use = dfg->use;
	in  = dfg->in;
	out = dfg->out;

	/* Collect "def" and "use" sets */
	for (j = 0; j < blocks_count; j++) {
		zend_op *opline, *end;
		zend_bitset b_use, b_def;

		if ((blocks[j].flags & ZEND_BB_REACHABLE) == 0) {
			continue;
		}

		opline = op_array->opcodes + blocks[j].start;
		end = opline + blocks[j].len;
		b_use = DFG_BITSET(use, set_size, j);
		b_def = DFG_BITSET(def, set_size, j);
		for (; opline < end; opline++) {
			if (opline->opcode != ZEND_OP_DATA) {
				_zend_dfg_add_use_def_op(op_array, opline, build_flags, b_use, b_def);
			}
		}
	}

	/* Calculate "in" and "out" sets */
	{
		uint32_t worklist_len = zend_bitset_len(blocks_count);
		zend_bitset worklist;
		ALLOCA_FLAG(use_heap);
		worklist = ZEND_BITSET_ALLOCA(worklist_len, use_heap);
		memset(worklist, 0, worklist_len * ZEND_BITSET_ELM_SIZE);
		for (j = 0; j < blocks_count; j++) {
			zend_bitset_incl(worklist, j);
		}
		while (!zend_bitset_empty(worklist, worklist_len)) {
			/* We use the last block on the worklist, because predecessors tend to be located
			 * before the succeeding block, so this converges faster. */
			j = zend_bitset_last(worklist, worklist_len);
			zend_bitset_excl(worklist, j);

			if ((blocks[j].flags & ZEND_BB_REACHABLE) == 0) {
				continue;
			}
			if (blocks[j].successors_count != 0) {
				zend_bitset_copy(DFG_BITSET(out, set_size, j), DFG_BITSET(in, set_size, blocks[j].successors[0]), set_size);
				for (k = 1; k < blocks[j].successors_count; k++) {
					zend_bitset_union(DFG_BITSET(out, set_size, j), DFG_BITSET(in, set_size, blocks[j].successors[k]), set_size);
				}
			} else {
				zend_bitset_clear(DFG_BITSET(out, set_size, j), set_size);
			}
			zend_bitset_union_with_difference(tmp, DFG_BITSET(use, set_size, j), DFG_BITSET(out, set_size, j), DFG_BITSET(def, set_size, j), set_size);
			if (!zend_bitset_equal(DFG_BITSET(in, set_size, j), tmp, set_size)) {
				zend_bitset_copy(DFG_BITSET(in, set_size, j), tmp, set_size);

				/* Add predecessors of changed block to worklist */
				{
					int *predecessors = &cfg->predecessors[blocks[j].predecessor_offset];
					for (k = 0; k < blocks[j].predecessors_count; k++) {
						zend_bitset_incl(worklist, predecessors[k]);
					}
				}
			}
		}

		free_alloca(worklist, use_heap);
	}
}
/* }}} */
