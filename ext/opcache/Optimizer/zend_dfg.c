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

int zend_build_dfg(const zend_op_array *op_array, const zend_cfg *cfg, zend_dfg *dfg) /* {{{ */
{
	int set_size;
	zend_basic_block *blocks = cfg->blocks;
	int blocks_count = cfg->blocks_count;
	zend_bitset tmp, gen, def, use, in, out;
	zend_op *opline;
	uint32_t k;
	int j;

	/* FIXME: can we use "gen" instead of "def" for flow analyzing? */
	set_size = dfg->size;
	tmp = dfg->tmp;
	gen = dfg->gen;
	def = dfg->def;
	use = dfg->use;
	in  = dfg->in;
	out = dfg->out;

	/* Collect "gen", "def" and "use" sets */
	for (j = 0; j < blocks_count; j++) {
		if ((blocks[j].flags & ZEND_BB_REACHABLE) == 0) {
			continue;
		}
		for (k = blocks[j].start; k <= blocks[j].end; k++) {
			opline = op_array->opcodes + k;
			if (opline->opcode != ZEND_OP_DATA) {
				zend_op *next = opline + 1;
				if (k < blocks[j].end &&
					next->opcode == ZEND_OP_DATA) {
					if (next->op1_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
						if (!DFG_ISSET(def, set_size, j, EX_VAR_TO_NUM(next->op1.var))) {
							DFG_SET(use, set_size, j, EX_VAR_TO_NUM(next->op1.var));
						}
					}
					if (next->op2_type == IS_CV) {
						if (!DFG_ISSET(def, set_size, j,EX_VAR_TO_NUM(next->op2.var))) {
							DFG_SET(use, set_size, j, EX_VAR_TO_NUM(next->op2.var));
						}
					} else if (next->op2_type == IS_VAR ||
							   next->op2_type == IS_TMP_VAR) {
						/* ZEND_ASSIGN_??? use the second operand
						   of the following OP_DATA instruction as
						   a temporary variable */
						switch (opline->opcode) {
							case ZEND_ASSIGN_DIM:
							case ZEND_ASSIGN_OBJ:
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
								break;
							default:
								if (!DFG_ISSET(def, set_size, j, EX_VAR_TO_NUM(next->op2.var))) {
									DFG_SET(use, set_size, j, EX_VAR_TO_NUM(next->op2.var));
								}
						}
					}
				}
				if (opline->op1_type == IS_CV) {
					switch (opline->opcode) {
					case ZEND_ASSIGN:
					case ZEND_ASSIGN_REF:
					case ZEND_BIND_GLOBAL:
					case ZEND_BIND_STATIC:
					case ZEND_SEND_VAR_EX:
					case ZEND_SEND_REF:
					case ZEND_SEND_VAR_NO_REF:
					case ZEND_FE_RESET_R:
					case ZEND_FE_RESET_RW:
					case ZEND_ADD_ARRAY_ELEMENT:
					case ZEND_INIT_ARRAY:
					case ZEND_BIND_LEXICAL:
						if (!DFG_ISSET(use, set_size, j, EX_VAR_TO_NUM(opline->op1.var))) {
							// FIXME: include into "use" to ...?
							DFG_SET(use, set_size, j, EX_VAR_TO_NUM(opline->op1.var));
							DFG_SET(def, set_size, j, EX_VAR_TO_NUM(opline->op1.var));
						}
						DFG_SET(gen, set_size, j, EX_VAR_TO_NUM(opline->op1.var));
						break;
					case ZEND_UNSET_VAR:
						ZEND_ASSERT(opline->extended_value & ZEND_QUICK_SET);
						/* break missing intentionally */
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
						DFG_SET(gen, set_size, j, EX_VAR_TO_NUM(opline->op1.var));
					default:
						if (!DFG_ISSET(def, set_size, j, EX_VAR_TO_NUM(opline->op1.var))) {
							DFG_SET(use, set_size, j, EX_VAR_TO_NUM(opline->op1.var));
						}
					}
				} else if (opline->op1_type == IS_VAR ||
						   opline->op1_type == IS_TMP_VAR) {
					if (!DFG_ISSET(def, set_size, j, EX_VAR_TO_NUM(opline->op1.var))) {
						DFG_SET(use, set_size, j, EX_VAR_TO_NUM(opline->op1.var));
					}
				}
				if (opline->op2_type == IS_CV) {
					switch (opline->opcode) {
						case ZEND_ASSIGN:
						case ZEND_ASSIGN_REF:
						case ZEND_FE_FETCH_R:
						case ZEND_FE_FETCH_RW:
							if (!DFG_ISSET(use, set_size, j, EX_VAR_TO_NUM(opline->op2.var))) {
								// FIXME: include into "use" to ...?
								DFG_SET(use, set_size, j, EX_VAR_TO_NUM(opline->op2.var));
								DFG_SET(def, set_size, j, EX_VAR_TO_NUM(opline->op2.var));
							}
							DFG_SET(gen, set_size, j, EX_VAR_TO_NUM(opline->op2.var));
							break;
						default:
							if (!DFG_ISSET(def, set_size, j, EX_VAR_TO_NUM(opline->op2.var))) {
								DFG_SET(use, set_size, j, EX_VAR_TO_NUM(opline->op2.var));
							}
							break;
					}
				} else if (opline->op2_type == IS_VAR ||
						   opline->op2_type == IS_TMP_VAR) {
					if (opline->opcode == ZEND_FE_FETCH_R || opline->opcode == ZEND_FE_FETCH_RW) {
						if (!DFG_ISSET(use, set_size, j, EX_VAR_TO_NUM(opline->op2.var))) {
							DFG_SET(def, set_size, j, EX_VAR_TO_NUM(opline->op2.var));
						}
						DFG_SET(gen, set_size, j, EX_VAR_TO_NUM(opline->op2.var));
					} else {
						if (!DFG_ISSET(def, set_size, j, EX_VAR_TO_NUM(opline->op2.var))) {
							DFG_SET(use, set_size, j, EX_VAR_TO_NUM(opline->op2.var));
						}
					}
				}
				if (opline->result_type == IS_CV) {
					if (!DFG_ISSET(use, set_size, j, EX_VAR_TO_NUM(opline->result.var))) {
						DFG_SET(def, set_size, j, EX_VAR_TO_NUM(opline->result.var));
					}
					DFG_SET(gen, set_size, j, EX_VAR_TO_NUM(opline->result.var));
				} else if (opline->result_type == IS_VAR ||
						   opline->result_type == IS_TMP_VAR) {
					if (!DFG_ISSET(use, set_size, j, EX_VAR_TO_NUM(opline->result.var))) {
						DFG_SET(def, set_size, j, EX_VAR_TO_NUM(opline->result.var));
					}
					DFG_SET(gen, set_size, j, EX_VAR_TO_NUM(opline->result.var));
				}
				if ((opline->opcode == ZEND_FE_FETCH_R || opline->opcode == ZEND_FE_FETCH_RW) && opline->result_type == IS_TMP_VAR) {
					if (!DFG_ISSET(use, set_size, j, EX_VAR_TO_NUM(next->result.var))) {
						DFG_SET(def, set_size, j, EX_VAR_TO_NUM(next->result.var));
					}
					DFG_SET(gen, set_size, j, EX_VAR_TO_NUM(next->result.var));
				}
			}
		}
	}

	/* Calculate "in" and "out" sets */
	{
		uint32_t worklist_len = zend_bitset_len(blocks_count);
		ALLOCA_FLAG(use_heap);
		zend_bitset worklist = ZEND_BITSET_ALLOCA(worklist_len, use_heap);
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
