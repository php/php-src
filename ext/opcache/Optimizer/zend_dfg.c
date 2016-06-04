/*
   +----------------------------------------------------------------------+
   | Zend Engine, DFG - Data Flow Graph                                   |
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
#include "zend_compile.h"
#include "zend_dfg.h"

int zend_build_dfg(const zend_op_array *op_array, const zend_cfg *cfg, zend_dfg *dfg, uint32_t build_flags) /* {{{ */
{
	int set_size;
	zend_basic_block *blocks = cfg->blocks;
	int blocks_count = cfg->blocks_count;
	zend_bitset tmp, def, use, in, out;
	uint32_t k, var_num;
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
		if ((blocks[j].flags & ZEND_BB_REACHABLE) == 0) {
			continue;
		}

		opline = op_array->opcodes + blocks[j].start;
		end = opline + blocks[j].len;
		for (; opline < end; opline++) {
			if (opline->opcode != ZEND_OP_DATA) {
				zend_op *next = opline + 1;
				if (next < end && next->opcode == ZEND_OP_DATA) {
					if (next->op1_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
						var_num = EX_VAR_TO_NUM(next->op1.var);
						if (!DFG_ISSET(def, set_size, j, var_num)) {
							DFG_SET(use, set_size, j, var_num);
						}
					}
					if (next->op2_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
						var_num = EX_VAR_TO_NUM(next->op2.var);
						if (!DFG_ISSET(def, set_size, j, var_num)) {
							DFG_SET(use, set_size, j, var_num);
						}
					}
				}
				if (opline->op1_type == IS_CV) {
					var_num = EX_VAR_TO_NUM(opline->op1.var);
					switch (opline->opcode) {
					case ZEND_ADD_ARRAY_ELEMENT:
					case ZEND_INIT_ARRAY:
						if ((build_flags & ZEND_SSA_RC_INFERENCE)
								|| (opline->extended_value & ZEND_ARRAY_ELEMENT_REF)) {
							goto op1_def;
						}
						goto op1_use;
					case ZEND_FE_RESET_R:
						if (build_flags & ZEND_SSA_RC_INFERENCE) {
							goto op1_def;
						}
						goto op1_use;
					case ZEND_YIELD:
						if ((build_flags & ZEND_SSA_RC_INFERENCE)
								|| (op_array->fn_flags & ZEND_ACC_RETURN_REFERENCE)) {
							goto op1_def;
						}
						goto op1_use;
					case ZEND_UNSET_VAR:
						ZEND_ASSERT(opline->extended_value & ZEND_QUICK_SET);
						/* break missing intentionally */
					case ZEND_ASSIGN:
					case ZEND_ASSIGN_REF:
					case ZEND_BIND_GLOBAL:
					case ZEND_BIND_STATIC:
					case ZEND_SEND_VAR_EX:
					case ZEND_SEND_REF:
					case ZEND_SEND_VAR_NO_REF:
					case ZEND_SEND_VAR_NO_REF_EX:
					case ZEND_FE_RESET_RW:
					case ZEND_ASSIGN_ADD:
					case ZEND_ASSIGN_SUB:
					case ZEND_ASSIGN_MUL:
					case ZEND_ASSIGN_DIV:
					case ZEND_ASSIGN_MOD:
					case ZEND_ASSIGN_SL:
					case ZEND_ASSIGN_SR:
					case ZEND_ASSIGN_CONCAT:
					case ZEND_ASSIGN_BW_OR:
					case ZEND_ASSIGN_BW_AND:
					case ZEND_ASSIGN_BW_XOR:
					case ZEND_ASSIGN_POW:
					case ZEND_PRE_INC:
					case ZEND_PRE_DEC:
					case ZEND_POST_INC:
					case ZEND_POST_DEC:
					case ZEND_ASSIGN_DIM:
					case ZEND_ASSIGN_OBJ:
					case ZEND_UNSET_DIM:
					case ZEND_UNSET_OBJ:
					case ZEND_FETCH_DIM_W:
					case ZEND_FETCH_DIM_RW:
					case ZEND_FETCH_DIM_FUNC_ARG:
					case ZEND_FETCH_DIM_UNSET:
					case ZEND_FETCH_OBJ_W:
					case ZEND_FETCH_OBJ_RW:
					case ZEND_FETCH_OBJ_FUNC_ARG:
					case ZEND_FETCH_OBJ_UNSET:
					case ZEND_VERIFY_RETURN_TYPE:
op1_def:
						/* `def` always come along with dtor or separation,
						 * thus the origin var info might be also `use`d in the feature(CG) */
						DFG_SET(use, set_size, j, var_num);
						DFG_SET(def, set_size, j, var_num);
						break;
					default:
op1_use:
						if (!DFG_ISSET(def, set_size, j, var_num)) {
							DFG_SET(use, set_size, j, var_num);
						}
					}
				} else if (opline->op1_type & (IS_VAR|IS_TMP_VAR)) {
					var_num = EX_VAR_TO_NUM(opline->op1.var);
					if (opline->opcode == ZEND_VERIFY_RETURN_TYPE) {
						DFG_SET(use, set_size, j, var_num);
						DFG_SET(def, set_size, j, var_num);
					} else if (!DFG_ISSET(def, set_size, j, var_num)) {
						DFG_SET(use, set_size, j, var_num);
					}
				}
				if (opline->op2_type == IS_CV) {
					var_num = EX_VAR_TO_NUM(opline->op2.var);
					switch (opline->opcode) {
						case ZEND_ASSIGN:
							if (build_flags & ZEND_SSA_RC_INFERENCE) {
								goto op2_def;
							}
							goto op2_use;
						case ZEND_BIND_LEXICAL:
							if ((build_flags & ZEND_SSA_RC_INFERENCE) || opline->extended_value) {
								goto op2_def;
							}
							goto op2_use;
						case ZEND_ASSIGN_REF:
						case ZEND_FE_FETCH_R:
						case ZEND_FE_FETCH_RW:
op2_def:
							// FIXME: include into "use" too ...?
							DFG_SET(use, set_size, j, var_num);
							DFG_SET(def, set_size, j, var_num);
							break;
						default:
op2_use:
							if (!DFG_ISSET(def, set_size, j, var_num)) {
								DFG_SET(use, set_size, j, var_num);
							}
							break;
					}
				} else if (opline->op2_type & (IS_VAR|IS_TMP_VAR)) {
					var_num = EX_VAR_TO_NUM(opline->op2.var);
					if (opline->opcode == ZEND_FE_FETCH_R || opline->opcode == ZEND_FE_FETCH_RW) {
						DFG_SET(def, set_size, j, var_num);
					} else {
						if (!DFG_ISSET(def, set_size, j, var_num)) {
							DFG_SET(use, set_size, j, var_num);
						}
					}
				}
				if (opline->result_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
					var_num = EX_VAR_TO_NUM(opline->result.var);
					DFG_SET(def, set_size, j, var_num);
				}
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
			if (blocks[j].successors[0] >= 0) {
				zend_bitset_copy(DFG_BITSET(out, set_size, j), DFG_BITSET(in, set_size, blocks[j].successors[0]), set_size);
				if (blocks[j].successors[1] >= 0) {
					zend_bitset_union(DFG_BITSET(out, set_size, j), DFG_BITSET(in, set_size, blocks[j].successors[1]), set_size);
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

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
