/*
   +----------------------------------------------------------------------+
   | Zend Engine, SSA - Static Single Assignment Form                     |
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
#include "zend_ssa.h"
#include "zend_dump.h"
#include "zend_inference.h"

static zend_bool dominates(const zend_basic_block *blocks, int a, int b) {
	while (blocks[b].level > blocks[a].level) {
		b = blocks[b].idom;
	}
	return a == b;
}

static zend_bool dominates_other_predecessors(
		const zend_cfg *cfg, const zend_basic_block *block, int check, int exclude) {
	int i;
	for (i = 0; i < block->predecessors_count; i++) {
		int predecessor = cfg->predecessors[block->predecessor_offset + i];
		if (predecessor != exclude && !dominates(cfg->blocks, check, predecessor)) {
			return 0;
		}
	}
	return 1;
}

static zend_bool needs_pi(const zend_op_array *op_array, zend_dfg *dfg, zend_ssa *ssa, int from, int to, int var) /* {{{ */
{
	zend_basic_block *from_block, *to_block;
	int other_successor;

	if (!DFG_ISSET(dfg->in, dfg->size, to, var)) {
		/* Variable is not live, certainly won't benefit from pi */
		return 0;
	}

	to_block = &ssa->cfg.blocks[to];
	if (to_block->predecessors_count == 1) {
		/* Always place pi if one predecessor (an if branch) */
		return 1;
	}

	/* Check that the other successor of the from block does not dominate all other predecessors.
	 * If it does, we'd probably end up annihilating a positive+negative pi assertion. */
	from_block = &ssa->cfg.blocks[from];
	other_successor = from_block->successors[0] == to
		? from_block->successors[1] : from_block->successors[0];
	return !dominates_other_predecessors(&ssa->cfg, to_block, other_successor, from);
}
/* }}} */

static zend_ssa_phi *add_pi(
		zend_arena **arena, const zend_op_array *op_array, zend_dfg *dfg, zend_ssa *ssa,
		int from, int to, int var) /* {{{ */
{
	zend_ssa_phi *phi;
	if (!needs_pi(op_array, dfg, ssa, from, to, var)) {
		return NULL;
	}

	phi = zend_arena_calloc(arena, 1,
		sizeof(zend_ssa_phi) +
		sizeof(int) * ssa->cfg.blocks[to].predecessors_count +
		sizeof(void*) * ssa->cfg.blocks[to].predecessors_count);
	phi->sources = (int*)(((char*)phi) + sizeof(zend_ssa_phi));
	memset(phi->sources, 0xff, sizeof(int) * ssa->cfg.blocks[to].predecessors_count);
	phi->use_chains = (zend_ssa_phi**)(((char*)phi->sources) + sizeof(int) * ssa->cfg.blocks[to].predecessors_count);

	phi->pi = from;
	phi->var = var;
	phi->ssa_var = -1;
	phi->next = ssa->blocks[to].phis;
	ssa->blocks[to].phis = phi;

	/* Block "to" now defines "var" via the pi statement, so add it to the "def" set. Note that
	 * this is not entirely accurate, because the pi is actually placed along the edge from->to.
	 * If there is a back-edge to "to" this may result in non-minimal SSA form. */
	DFG_SET(dfg->def, dfg->size, to, var);

	/* If there are multiple predecessors in the target block, we need to place a phi there.
	 * However this can (generally) not be expressed in terms of dominance frontiers, so place it
	 * explicitly. dfg->use here really is dfg->phi, we're reusing the set. */
	if (ssa->cfg.blocks[to].predecessors_count > 1) {
		DFG_SET(dfg->use, dfg->size, to, var);
	}

	return phi;
}
/* }}} */

static void pi_range(
		zend_ssa_phi *phi, int min_var, int max_var, zend_long min, zend_long max,
		char underflow, char overflow, char negative) /* {{{ */
{
	zend_ssa_range_constraint *constraint = &phi->constraint.range;
	constraint->min_var = min_var;
	constraint->max_var = max_var;
	constraint->min_ssa_var = -1;
	constraint->max_ssa_var = -1;
	constraint->range.min = min;
	constraint->range.max = max;
	constraint->range.underflow = underflow;
	constraint->range.overflow = overflow;
	constraint->negative = negative ? NEG_INIT : NEG_NONE;
	phi->has_range_constraint = 1;
}
/* }}} */

static inline void pi_range_equals(zend_ssa_phi *phi, int var, zend_long val) {
	pi_range(phi, var, var, val, val, 0, 0, 0);
}
static inline void pi_range_not_equals(zend_ssa_phi *phi, int var, zend_long val) {
	pi_range(phi, var, var, val, val, 0, 0, 1);
}
static inline void pi_range_min(zend_ssa_phi *phi, int var, zend_long val) {
	pi_range(phi, var, -1, val, ZEND_LONG_MAX, 0, 1, 0);
}
static inline void pi_range_max(zend_ssa_phi *phi, int var, zend_long val) {
	pi_range(phi, -1, var, ZEND_LONG_MIN, val, 1, 0, 0);
}

static void pi_type_mask(zend_ssa_phi *phi, uint32_t type_mask) {
	phi->has_range_constraint = 0;
	phi->constraint.type.ce = NULL;
	phi->constraint.type.type_mask = MAY_BE_REF|MAY_BE_RC1|MAY_BE_RCN;
	phi->constraint.type.type_mask |= type_mask;
	if (type_mask & MAY_BE_NULL) {
		phi->constraint.type.type_mask |= MAY_BE_UNDEF;
	}
}
static inline void pi_not_type_mask(zend_ssa_phi *phi, uint32_t type_mask) {
	uint32_t relevant = MAY_BE_ANY|MAY_BE_ARRAY_KEY_ANY|MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_OF_REF;
	pi_type_mask(phi, ~type_mask & relevant);
}
static inline uint32_t mask_for_type_check(uint32_t type) {
	if (type == _IS_BOOL) {
		return MAY_BE_TRUE|MAY_BE_FALSE;
	} else if (type == IS_ARRAY) {
		return MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_ANY|MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_OF_REF;
	} else {
		return 1 << type;
	}
}

/* We can interpret $a + 5 == 0 as $a = 0 - 5, i.e. shift the adjustment to the other operand.
 * This negated adjustment is what is written into the "adjustment" parameter. */
static int find_adjusted_tmp_var(const zend_op_array *op_array, uint32_t build_flags, zend_op *opline, uint32_t var_num, zend_long *adjustment) /* {{{ */
{
	zend_op *op = opline;
	while (op != op_array->opcodes) {
		op--;
		if (op->result_type != IS_TMP_VAR || op->result.var != var_num) {
			continue;
		}

		if (op->opcode == ZEND_POST_DEC) {
			if (op->op1_type == IS_CV) {
				*adjustment = -1;
				return EX_VAR_TO_NUM(op->op1.var);
			}
		} else if (op->opcode == ZEND_POST_INC) {
			if (op->op1_type == IS_CV) {
				*adjustment = 1;
				return EX_VAR_TO_NUM(op->op1.var);
			}
		} else if (op->opcode == ZEND_ADD) {
			if (op->op1_type == IS_CV &&
				op->op2_type == IS_CONST &&
				Z_TYPE_P(CRT_CONSTANT(op->op2)) == IS_LONG &&
				Z_LVAL_P(CRT_CONSTANT(op->op2)) != ZEND_LONG_MIN) {
				*adjustment = -Z_LVAL_P(CRT_CONSTANT(op->op2));
				return EX_VAR_TO_NUM(op->op1.var);
			} else if (op->op2_type == IS_CV &&
					   op->op1_type == IS_CONST &&
					   Z_TYPE_P(CRT_CONSTANT(op->op1)) == IS_LONG &&
					   Z_LVAL_P(CRT_CONSTANT(op->op1)) != ZEND_LONG_MIN) {
				*adjustment = -Z_LVAL_P(CRT_CONSTANT(op->op1));
				return EX_VAR_TO_NUM(op->op2.var);
			}
		} else if (op->opcode == ZEND_SUB) {
			if (op->op1_type == IS_CV &&
				op->op2_type == IS_CONST &&
				Z_TYPE_P(CRT_CONSTANT(op->op2)) == IS_LONG) {
				*adjustment = Z_LVAL_P(CRT_CONSTANT(op->op2));
				return EX_VAR_TO_NUM(op->op1.var);
			}
		}
		break;
	}
	return -1;
}
/* }}} */

static inline zend_bool add_will_overflow(zend_long a, zend_long b) {
	return (b > 0 && a > ZEND_LONG_MAX - b)
		|| (b < 0 && a < ZEND_LONG_MIN - b);
}
static inline zend_bool sub_will_overflow(zend_long a, zend_long b) {
	return (b > 0 && a < ZEND_LONG_MIN + b)
		|| (b < 0 && a > ZEND_LONG_MAX + b);
}

/* e-SSA construction: Pi placement (Pi is actually a Phi with single
 * source and constraint).
 * Order of Phis is importent, Pis must be placed before Phis
 */
static void place_essa_pis(
		zend_arena **arena, const zend_script *script, const zend_op_array *op_array,
		uint32_t build_flags, zend_ssa *ssa, zend_dfg *dfg) /* {{{ */ {
	zend_basic_block *blocks = ssa->cfg.blocks;
	int j, blocks_count = ssa->cfg.blocks_count;
	for (j = 0; j < blocks_count; j++) {
		zend_ssa_phi *pi;
		zend_op *opline = op_array->opcodes + blocks[j].start + blocks[j].len - 1;
		int bt; /* successor block number if a condition is true */
		int bf; /* successor block number if a condition is false */

		if ((blocks[j].flags & ZEND_BB_REACHABLE) == 0 || blocks[j].len == 0) {
			continue;
		}
		/* the last instruction of basic block is conditional branch,
		 * based on comparison of CV(s)
		 */
		switch (opline->opcode) {
			case ZEND_JMPZ:
			case ZEND_JMPZNZ:
				bf = blocks[j].successors[0];
				bt = blocks[j].successors[1];
				break;
			case ZEND_JMPNZ:
				bt = blocks[j].successors[0];
				bf = blocks[j].successors[1];
				break;
			default:
				continue;
		}
		if (opline->op1_type == IS_TMP_VAR &&
		    ((opline-1)->opcode == ZEND_IS_EQUAL ||
		     (opline-1)->opcode == ZEND_IS_NOT_EQUAL ||
		     (opline-1)->opcode == ZEND_IS_SMALLER ||
		     (opline-1)->opcode == ZEND_IS_SMALLER_OR_EQUAL) &&
		    opline->op1.var == (opline-1)->result.var) {
			int  var1 = -1;
			int  var2 = -1;
			zend_long val1 = 0;
			zend_long val2 = 0;
//			long val = 0;

			if ((opline-1)->op1_type == IS_CV) {
				var1 = EX_VAR_TO_NUM((opline-1)->op1.var);
			} else if ((opline-1)->op1_type == IS_TMP_VAR) {
				var1 = find_adjusted_tmp_var(
					op_array, build_flags, opline, (opline-1)->op1.var, &val2);
			}

			if ((opline-1)->op2_type == IS_CV) {
				var2 = EX_VAR_TO_NUM((opline-1)->op2.var);
			} else if ((opline-1)->op2_type == IS_TMP_VAR) {
				var2 = find_adjusted_tmp_var(
					op_array, build_flags, opline, (opline-1)->op2.var, &val1);
			}

			if (var1 >= 0 && var2 >= 0) {
				if (!sub_will_overflow(val1, val2) && !sub_will_overflow(val2, val1)) {
					zend_long tmp = val1;
					val1 -= val2;
					val2 -= tmp;
				} else {
					var1 = -1;
					var2 = -1;
				}
			} else if (var1 >= 0 && var2 < 0) {
				zend_long add_val2 = 0;
				if ((opline-1)->op2_type == IS_CONST &&
				    Z_TYPE_P(CRT_CONSTANT((opline-1)->op2)) == IS_LONG) {
					add_val2 = Z_LVAL_P(CRT_CONSTANT((opline-1)->op2));
				} else if ((opline-1)->op2_type == IS_CONST &&
				    Z_TYPE_P(CRT_CONSTANT((opline-1)->op2)) == IS_FALSE) {
					add_val2 = 0;
				} else if ((opline-1)->op2_type == IS_CONST &&
				    Z_TYPE_P(CRT_CONSTANT((opline-1)->op2)) == IS_TRUE) {
					add_val2 = 1;
				} else {
					var1 = -1;
				}
				if (!add_will_overflow(val2, add_val2)) {
					val2 += add_val2;
				} else {
					var1 = -1;
				}
			} else if (var1 < 0 && var2 >= 0) {
				zend_long add_val1 = 0;
				if ((opline-1)->op1_type == IS_CONST &&
				    Z_TYPE_P(CRT_CONSTANT((opline-1)->op1)) == IS_LONG) {
					add_val1 = Z_LVAL_P(CRT_CONSTANT((opline-1)->op1));
				} else if ((opline-1)->op1_type == IS_CONST &&
				    Z_TYPE_P(CRT_CONSTANT((opline-1)->op1)) == IS_FALSE) {
					add_val1 = 0;
				} else if ((opline-1)->op1_type == IS_CONST &&
				    Z_TYPE_P(CRT_CONSTANT((opline-1)->op1)) == IS_TRUE) {
					add_val1 = 1;
				} else {
					var2 = -1;
				}
				if (!add_will_overflow(val1, add_val1)) {
					val1 += add_val1;
				} else {
					var2 = -1;
				}
			}

			if (var1 >= 0) {
				if ((opline-1)->opcode == ZEND_IS_EQUAL) {
					if ((pi = add_pi(arena, op_array, dfg, ssa, j, bt, var1))) {
						pi_range_equals(pi, var2, val2);
					}
					if ((pi = add_pi(arena, op_array, dfg, ssa, j, bf, var1))) {
						pi_range_not_equals(pi, var2, val2);
					}
				} else if ((opline-1)->opcode == ZEND_IS_NOT_EQUAL) {
					if ((pi = add_pi(arena, op_array, dfg, ssa, j, bf, var1))) {
						pi_range_equals(pi, var2, val2);
					}
					if ((pi = add_pi(arena, op_array, dfg, ssa, j, bt, var1))) {
						pi_range_not_equals(pi, var2, val2);
					}
				} else if ((opline-1)->opcode == ZEND_IS_SMALLER) {
					if (val2 > ZEND_LONG_MIN) {
						if ((pi = add_pi(arena, op_array, dfg, ssa, j, bt, var1))) {
							pi_range_max(pi, var2, val2-1);
						}
					}
					if ((pi = add_pi(arena, op_array, dfg, ssa, j, bf, var1))) {
						pi_range_min(pi, var2, val2);
					}
				} else if ((opline-1)->opcode == ZEND_IS_SMALLER_OR_EQUAL) {
					if ((pi = add_pi(arena, op_array, dfg, ssa, j, bt, var1))) {
						pi_range_max(pi, var2, val2);
					}
					if (val2 < ZEND_LONG_MAX) {
						if ((pi = add_pi(arena, op_array, dfg, ssa, j, bf, var1))) {
							pi_range_min(pi, var2, val2+1);
						}
					}
				}
			}
			if (var2 >= 0) {
				if((opline-1)->opcode == ZEND_IS_EQUAL) {
					if ((pi = add_pi(arena, op_array, dfg, ssa, j, bt, var2))) {
						pi_range_equals(pi, var1, val1);
					}
					if ((pi = add_pi(arena, op_array, dfg, ssa, j, bf, var2))) {
						pi_range_not_equals(pi, var1, val1);
					}
				} else if ((opline-1)->opcode == ZEND_IS_NOT_EQUAL) {
					if ((pi = add_pi(arena, op_array, dfg, ssa, j, bf, var2))) {
						pi_range_equals(pi, var1, val1);
					}
					if ((pi = add_pi(arena, op_array, dfg, ssa, j, bt, var2))) {
						pi_range_not_equals(pi, var1, val1);
					}
				} else if ((opline-1)->opcode == ZEND_IS_SMALLER) {
					if (val1 < ZEND_LONG_MAX) {
						if ((pi = add_pi(arena, op_array, dfg, ssa, j, bt, var2))) {
							pi_range_min(pi, var1, val1+1);
						}
					}
					if ((pi = add_pi(arena, op_array, dfg, ssa, j, bf, var2))) {
						pi_range_max(pi, var1, val1);
					}
				} else if ((opline-1)->opcode == ZEND_IS_SMALLER_OR_EQUAL) {
					if ((pi = add_pi(arena, op_array, dfg, ssa, j, bt, var2))) {
						pi_range_min(pi, var1, val1);
					}
					if (val1 > ZEND_LONG_MIN) {
						if ((pi = add_pi(arena, op_array, dfg, ssa, j, bf, var2))) {
							pi_range_max(pi, var1, val1-1);
						}
					}
				}
			}
		} else if (opline->op1_type == IS_TMP_VAR &&
		           ((opline-1)->opcode == ZEND_POST_INC ||
		            (opline-1)->opcode == ZEND_POST_DEC) &&
		           opline->op1.var == (opline-1)->result.var &&
		           (opline-1)->op1_type == IS_CV) {
			int var = EX_VAR_TO_NUM((opline-1)->op1.var);

			if ((opline-1)->opcode == ZEND_POST_DEC) {
				if ((pi = add_pi(arena, op_array, dfg, ssa, j, bf, var))) {
					pi_range_equals(pi, -1, -1);
				}
				if ((pi = add_pi(arena, op_array, dfg, ssa, j, bt, var))) {
					pi_range_not_equals(pi, -1, -1);
				}
			} else if ((opline-1)->opcode == ZEND_POST_INC) {
				if ((pi = add_pi(arena, op_array, dfg, ssa, j, bf, var))) {
					pi_range_equals(pi, -1, 1);
				}
				if ((pi = add_pi(arena, op_array, dfg, ssa, j, bt, var))) {
					pi_range_not_equals(pi, -1, 1);
				}
			}
		} else if (opline->op1_type == IS_VAR &&
		           ((opline-1)->opcode == ZEND_PRE_INC ||
		            (opline-1)->opcode == ZEND_PRE_DEC) &&
		           opline->op1.var == (opline-1)->result.var &&
		           (opline-1)->op1_type == IS_CV) {
			int var = EX_VAR_TO_NUM((opline-1)->op1.var);

			if ((pi = add_pi(arena, op_array, dfg, ssa, j, bf, var))) {
				pi_range_equals(pi, -1, 0);
			}
			/* speculative */
			if ((pi = add_pi(arena, op_array, dfg, ssa, j, bt, var))) {
				pi_range_not_equals(pi, -1, 0);
			}
		} else if (opline->op1_type == IS_TMP_VAR && (opline-1)->opcode == ZEND_TYPE_CHECK &&
				   opline->op1.var == (opline-1)->result.var && (opline-1)->op1_type == IS_CV) {
			int var = EX_VAR_TO_NUM((opline-1)->op1.var);
			uint32_t type = (opline-1)->extended_value;
			if ((pi = add_pi(arena, op_array, dfg, ssa, j, bt, var))) {
				pi_type_mask(pi, mask_for_type_check(type));
			}
			if (type != IS_OBJECT && type != IS_RESOURCE) {
				/* is_object() and is_resource() may return false, even though the value is
				 * an object/resource. */
				if ((pi = add_pi(arena, op_array, dfg, ssa, j, bf, var))) {
					pi_not_type_mask(pi, mask_for_type_check(type));
				}
			}
		} else if (opline->op1_type == IS_TMP_VAR &&
				   ((opline-1)->opcode == ZEND_IS_IDENTICAL
					|| (opline-1)->opcode == ZEND_IS_NOT_IDENTICAL) &&
				   opline->op1.var == (opline-1)->result.var) {
			int var;
			zval *val;
			uint32_t type_mask;
			if ((opline-1)->op1_type == IS_CV && (opline-1)->op2_type == IS_CONST) {
				var = EX_VAR_TO_NUM((opline-1)->op1.var);
				val = CRT_CONSTANT((opline-1)->op2);
			} else if ((opline-1)->op1_type == IS_CONST && (opline-1)->op2_type == IS_CV) {
				var = EX_VAR_TO_NUM((opline-1)->op2.var);
				val = CRT_CONSTANT((opline-1)->op1);
			} else {
				continue;
			}

			/* We're interested in === null/true/false comparisons here, because they eliminate
			 * a type in the false-branch. Other === VAL comparisons are unlikely to be useful. */
			if (Z_TYPE_P(val) != IS_NULL && Z_TYPE_P(val) != IS_TRUE && Z_TYPE_P(val) != IS_FALSE) {
				continue;
			}

			type_mask = _const_op_type(val);
			if ((opline-1)->opcode == ZEND_IS_IDENTICAL) {
				if ((pi = add_pi(arena, op_array, dfg, ssa, j, bt, var))) {
					pi_type_mask(pi, type_mask);
				}
				if ((pi = add_pi(arena, op_array, dfg, ssa, j, bf, var))) {
					pi_not_type_mask(pi, type_mask);
				}
			} else {
				if ((pi = add_pi(arena, op_array, dfg, ssa, j, bf, var))) {
					pi_type_mask(pi, type_mask);
				}
				if ((pi = add_pi(arena, op_array, dfg, ssa, j, bt, var))) {
					pi_not_type_mask(pi, type_mask);
				}
			}
		} else if (opline->op1_type == IS_TMP_VAR && (opline-1)->opcode == ZEND_INSTANCEOF &&
				   opline->op1.var == (opline-1)->result.var && (opline-1)->op1_type == IS_CV &&
				   (opline-1)->op2_type == IS_CONST) {
			int var = EX_VAR_TO_NUM((opline-1)->op1.var);
			zend_string *lcname = Z_STR_P(CRT_CONSTANT((opline-1)->op2) + 1);
			zend_class_entry *ce = zend_hash_find_ptr(&script->class_table, lcname);
			if (!ce) {
				ce = zend_hash_find_ptr(CG(class_table), lcname);
				if (!ce || ce->type != ZEND_INTERNAL_CLASS) {
					continue;
				}
			}

			if ((pi = add_pi(arena, op_array, dfg, ssa, j, bt, var))) {
				pi_type_mask(pi, MAY_BE_OBJECT);
				pi->constraint.type.ce = ce;
			}
		}
	}
}
/* }}} */

static int zend_ssa_rename(const zend_op_array *op_array, uint32_t build_flags, zend_ssa *ssa, int *var, int n) /* {{{ */
{
	zend_basic_block *blocks = ssa->cfg.blocks;
	zend_ssa_block *ssa_blocks = ssa->blocks;
	zend_ssa_op *ssa_ops = ssa->ops;
	int ssa_vars_count = ssa->vars_count;
	int i, j;
	zend_op *opline, *end;
	int *tmp = NULL;
	ALLOCA_FLAG(use_heap);

	// FIXME: Can we optimize this copying out in some cases?
	if (blocks[n].next_child >= 0) {
		tmp = do_alloca(sizeof(int) * (op_array->last_var + op_array->T), use_heap);
		memcpy(tmp, var, sizeof(int) * (op_array->last_var + op_array->T));
		var = tmp;
	}

	if (ssa_blocks[n].phis) {
		zend_ssa_phi *phi = ssa_blocks[n].phis;
		do {
			if (phi->ssa_var < 0) {
				phi->ssa_var = ssa_vars_count;
				var[phi->var] = ssa_vars_count;
				ssa_vars_count++;
			} else {
				var[phi->var] = phi->ssa_var;
			}
			phi = phi->next;
		} while (phi);
	}

	opline = op_array->opcodes + blocks[n].start;
	end = opline + blocks[n].len;
	for (; opline < end; opline++) {
		uint32_t k = opline - op_array->opcodes;
		if (opline->opcode != ZEND_OP_DATA) {
			zend_op *next = opline + 1;
			if (next < end && next->opcode == ZEND_OP_DATA) {
				if (next->op1_type == IS_CV) {
					ssa_ops[k + 1].op1_use = var[EX_VAR_TO_NUM(next->op1.var)];
					//USE_SSA_VAR(next->op1.var);
				} else if (next->op1_type & (IS_VAR|IS_TMP_VAR)) {
					ssa_ops[k + 1].op1_use = var[EX_VAR_TO_NUM(next->op1.var)];
					//USE_SSA_VAR(op_array->last_var + next->op1.var);
				}
				if (next->op2_type == IS_CV) {
					ssa_ops[k + 1].op2_use = var[EX_VAR_TO_NUM(next->op2.var)];
					//USE_SSA_VAR(next->op2.var);
				} else if (next->op2_type & (IS_VAR|IS_TMP_VAR)) {
					ssa_ops[k + 1].op2_use = var[EX_VAR_TO_NUM(next->op2.var)];
					//USE_SSA_VAR(op_array->last_var + next->op2.var);
				}
			}
			if (opline->op1_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
				ssa_ops[k].op1_use = var[EX_VAR_TO_NUM(opline->op1.var)];
				//USE_SSA_VAR(op_array->last_var + opline->op1.var)
			}
			if (opline->opcode == ZEND_FE_FETCH_R || opline->opcode == ZEND_FE_FETCH_RW) {
				if (opline->op2_type == IS_CV) {
					ssa_ops[k].op2_use = var[EX_VAR_TO_NUM(opline->op2.var)];
				}
				ssa_ops[k].op2_def = ssa_vars_count;
				var[EX_VAR_TO_NUM(opline->op2.var)] = ssa_vars_count;
				ssa_vars_count++;
				//NEW_SSA_VAR(opline->op2.var)
			} else if (opline->op2_type & (IS_CV|IS_VAR|IS_TMP_VAR)) {
				ssa_ops[k].op2_use = var[EX_VAR_TO_NUM(opline->op2.var)];
				//USE_SSA_VAR(op_array->last_var + opline->op2.var)
			}
			switch (opline->opcode) {
				case ZEND_ASSIGN:
					if (opline->op1_type == IS_CV) {
						ssa_ops[k].op1_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op1.var)] = ssa_vars_count;
						ssa_vars_count++;
						//NEW_SSA_VAR(opline->op1.var)
					}
					if ((build_flags & ZEND_SSA_RC_INFERENCE) && opline->op2_type == IS_CV) {
						ssa_ops[k].op2_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op2.var)] = ssa_vars_count;
						ssa_vars_count++;
						//NEW_SSA_VAR(opline->op2.var)
					}
					break;
				case ZEND_ASSIGN_REF:
//TODO: ???
					if (opline->op1_type == IS_CV) {
						ssa_ops[k].op1_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op1.var)] = ssa_vars_count;
						ssa_vars_count++;
						//NEW_SSA_VAR(opline->op1.var)
					}
					if (opline->op2_type == IS_CV) {
						ssa_ops[k].op2_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op2.var)] = ssa_vars_count;
						ssa_vars_count++;
						//NEW_SSA_VAR(opline->op2.var)
					}
					break;
				case ZEND_BIND_GLOBAL:
				case ZEND_BIND_STATIC:
					if (opline->op1_type == IS_CV) {
						ssa_ops[k].op1_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op1.var)] = ssa_vars_count;
						ssa_vars_count++;
						//NEW_SSA_VAR(opline->op1.var)
					}
					break;
				case ZEND_ASSIGN_DIM:
				case ZEND_ASSIGN_OBJ:
					if (opline->op1_type == IS_CV) {
						ssa_ops[k].op1_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op1.var)] = ssa_vars_count;
						ssa_vars_count++;
						//NEW_SSA_VAR(opline->op1.var)
					}
					if ((build_flags & ZEND_SSA_RC_INFERENCE) && next->op1_type == IS_CV) {
						ssa_ops[k + 1].op1_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(next->op1.var)] = ssa_vars_count;
						ssa_vars_count++;
						//NEW_SSA_VAR(next->op1.var)
					}
					break;
				case ZEND_ADD_ARRAY_ELEMENT:
					ssa_ops[k].result_use = var[EX_VAR_TO_NUM(opline->result.var)];
				case ZEND_INIT_ARRAY:
					if (((build_flags & ZEND_SSA_RC_INFERENCE)
								|| (opline->extended_value & ZEND_ARRAY_ELEMENT_REF))
							&& opline->op1_type == IS_CV) {
						ssa_ops[k].op1_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op1.var)] = ssa_vars_count;
						ssa_vars_count++;
						//NEW_SSA_VAR(opline+->op1.var)
					}
					break;
				case ZEND_SEND_VAR_NO_REF:
				case ZEND_SEND_VAR_NO_REF_EX:
				case ZEND_SEND_VAR_EX:
				case ZEND_SEND_REF:
				case ZEND_SEND_UNPACK:
				case ZEND_FE_RESET_RW:
//TODO: ???
					if (opline->op1_type == IS_CV) {
						ssa_ops[k].op1_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op1.var)] = ssa_vars_count;
						ssa_vars_count++;
						//NEW_SSA_VAR(opline->op1.var)
					}
					break;
				case ZEND_FE_RESET_R:
					if ((build_flags & ZEND_SSA_RC_INFERENCE) && opline->op1_type == IS_CV) {
						ssa_ops[k].op1_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op1.var)] = ssa_vars_count;
						ssa_vars_count++;
						//NEW_SSA_VAR(opline->op1.var)
					}
					break;
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
					if (opline->op1_type == IS_CV) {
						ssa_ops[k].op1_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op1.var)] = ssa_vars_count;
						ssa_vars_count++;
						//NEW_SSA_VAR(opline->op1.var)
					}
					break;
				case ZEND_UNSET_VAR:
					if (opline->extended_value & ZEND_QUICK_SET) {
						ssa_ops[k].op1_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op1.var)] = ssa_vars_count;
						ssa_vars_count++;
					}
					break;
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
					if (opline->op1_type == IS_CV) {
						ssa_ops[k].op1_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op1.var)] = ssa_vars_count;
						ssa_vars_count++;
						//NEW_SSA_VAR(opline->op1.var)
					}
					break;
				case ZEND_BIND_LEXICAL:
					if (opline->extended_value || (build_flags & ZEND_SSA_RC_INFERENCE)) {
						ssa_ops[k].op2_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op2.var)] = ssa_vars_count;
						ssa_vars_count++;
					}
					break;
				case ZEND_YIELD:
					if (opline->op1_type == IS_CV
							&& ((op_array->fn_flags & ZEND_ACC_RETURN_REFERENCE)
								|| (build_flags & ZEND_SSA_RC_INFERENCE))) {
						ssa_ops[k].op1_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op1.var)] = ssa_vars_count;
						ssa_vars_count++;
					}
					break;
				case ZEND_VERIFY_RETURN_TYPE:
					if (opline->op1_type & (IS_TMP_VAR|IS_VAR|IS_CV)) {
						ssa_ops[k].op1_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op1.var)] = ssa_vars_count;
						ssa_vars_count++;
						//NEW_SSA_VAR(opline->op1.var)
					}
					break;
				default:
					break;
			}
			if (opline->result_type == IS_CV) {
				ssa_ops[k].result_def = ssa_vars_count;
				var[EX_VAR_TO_NUM(opline->result.var)] = ssa_vars_count;
				ssa_vars_count++;
				//NEW_SSA_VAR(opline->result.var)
			} else if (opline->result_type & (IS_VAR|IS_TMP_VAR)) {
				ssa_ops[k].result_def = ssa_vars_count;
				var[EX_VAR_TO_NUM(opline->result.var)] = ssa_vars_count;
				ssa_vars_count++;
				//NEW_SSA_VAR(op_array->last_var + opline->result.var)
			}
		}
	}

	for (i = 0; i < 2; i++) {
		int succ = blocks[n].successors[i];
		if (succ >= 0) {
			zend_ssa_phi *p;
			for (p = ssa_blocks[succ].phis; p; p = p->next) {
				if (p->pi == n) {
					/* e-SSA Pi */
					if (p->has_range_constraint) {
						if (p->constraint.range.min_var >= 0) {
							p->constraint.range.min_ssa_var = var[p->constraint.range.min_var];
						}
						if (p->constraint.range.max_var >= 0) {
							p->constraint.range.max_ssa_var = var[p->constraint.range.max_var];
						}
					}
					for (j = 0; j < blocks[succ].predecessors_count; j++) {
						p->sources[j] = var[p->var];
					}
					if (p->ssa_var < 0) {
						p->ssa_var = ssa_vars_count;
						ssa_vars_count++;
					}
				} else if (p->pi < 0) {
					/* Normal Phi */
					for (j = 0; j < blocks[succ].predecessors_count; j++)
						if (ssa->cfg.predecessors[blocks[succ].predecessor_offset + j] == n) {
							break;
						}
					ZEND_ASSERT(j < blocks[succ].predecessors_count);
					p->sources[j] = var[p->var];
				}
			}
			for (p = ssa_blocks[succ].phis; p && (p->pi >= 0); p = p->next) {
				if (p->pi == n) {
					zend_ssa_phi *q = p->next;
					while (q) {
						if (q->pi < 0 && q->var == p->var) {
							for (j = 0; j < blocks[succ].predecessors_count; j++) {
								if (ssa->cfg.predecessors[blocks[succ].predecessor_offset + j] == n) {
									break;
								}
							}
							ZEND_ASSERT(j < blocks[succ].predecessors_count);
							q->sources[j] = p->ssa_var;
						}
						q = q->next;
					}
				}
			}
		}
	}

	ssa->vars_count = ssa_vars_count;

	j = blocks[n].children;
	while (j >= 0) {
		// FIXME: Tail call optimization?
		if (zend_ssa_rename(op_array, build_flags, ssa, var, j) != SUCCESS)
			return FAILURE;
		j = blocks[j].next_child;
	}

	if (tmp) {
		free_alloca(tmp, use_heap);
	}

	return SUCCESS;
}
/* }}} */

int zend_build_ssa(zend_arena **arena, const zend_script *script, const zend_op_array *op_array, uint32_t build_flags, zend_ssa *ssa, uint32_t *func_flags) /* {{{ */
{
	zend_basic_block *blocks = ssa->cfg.blocks;
	zend_ssa_block *ssa_blocks;
	int blocks_count = ssa->cfg.blocks_count;
	uint32_t set_size;
	zend_bitset def, in, phi;
	int *var = NULL;
	int i, j, k, changed;
	zend_dfg dfg;
	ALLOCA_FLAG(dfg_use_heap)
	ALLOCA_FLAG(var_use_heap)

	ssa->rt_constants = (build_flags & ZEND_RT_CONSTANTS);
	ssa_blocks = zend_arena_calloc(arena, blocks_count, sizeof(zend_ssa_block));
	if (!ssa_blocks) {
		return FAILURE;
	}
	ssa->blocks = ssa_blocks;

	/* Compute Variable Liveness */
	dfg.vars = op_array->last_var + op_array->T;
	dfg.size = set_size = zend_bitset_len(dfg.vars);
	dfg.tmp = do_alloca((set_size * sizeof(zend_ulong)) * (blocks_count * 4 + 1), dfg_use_heap);
	memset(dfg.tmp, 0, (set_size * sizeof(zend_ulong)) * (blocks_count * 4 + 1));
	dfg.def = dfg.tmp + set_size;
	dfg.use = dfg.def + set_size * blocks_count;
	dfg.in  = dfg.use + set_size * blocks_count;
	dfg.out = dfg.in  + set_size * blocks_count;

	if (zend_build_dfg(op_array, &ssa->cfg, &dfg, build_flags) != SUCCESS) {
		free_alloca(dfg.tmp, dfg_use_heap);
		return FAILURE;
	}

	if (build_flags & ZEND_SSA_DEBUG_LIVENESS) {
		zend_dump_dfg(op_array, &ssa->cfg, &dfg);
	}

	def = dfg.def;
	in  = dfg.in;

	/* Reuse the "use" set, as we no longer need it */
	phi = dfg.use;
	zend_bitset_clear(phi, set_size * blocks_count);

	/* Place e-SSA pis. This will add additional "def" points, so it must
	 * happen before def propagation. */
	place_essa_pis(arena, script, op_array, build_flags, ssa, &dfg);

	/* SSA construction, Step 1: Propagate "def" sets in merge points */
	do {
		changed = 0;
		for (j = 0; j < blocks_count; j++) {
			zend_bitset def_j = def + j * set_size, phi_j = phi + j * set_size;
			if ((blocks[j].flags & ZEND_BB_REACHABLE) == 0) {
				continue;
			}
			if (blocks[j].predecessors_count > 1) {
				if (blocks[j].flags & ZEND_BB_IRREDUCIBLE_LOOP) {
					/* Prevent any values from flowing into irreducible loops by
					   replacing all incoming values with explicit phis.  The
					   register allocator depends on this property.  */
					zend_bitset_union(phi_j, in + (j * set_size), set_size);
				} else {
					for (k = 0; k < blocks[j].predecessors_count; k++) {
						i = ssa->cfg.predecessors[blocks[j].predecessor_offset + k];
						while (i != -1 && i != blocks[j].idom) {
							zend_bitset_union_with_intersection(
								phi_j, phi_j, def + (i * set_size), in + (j * set_size), set_size);
							i = blocks[i].idom;
						}
					}
				}
				if (!zend_bitset_subset(phi_j, def_j, set_size)) {
					zend_bitset_union(def_j, phi_j, set_size);
					changed = 1;
				}
			}
		}
	} while (changed);

	/* SSA construction, Step 2: Phi placement based on Dominance Frontiers */
	var = do_alloca(sizeof(int) * (op_array->last_var + op_array->T), var_use_heap);
	if (!var) {
		free_alloca(dfg.tmp, dfg_use_heap);
		return FAILURE;
	}

	for (j = 0; j < blocks_count; j++) {
		if ((blocks[j].flags & ZEND_BB_REACHABLE) == 0) {
			continue;
		}
		if (!zend_bitset_empty(phi + j * set_size, set_size)) {
			ZEND_BITSET_REVERSE_FOREACH(phi + j * set_size, set_size, i) {
				zend_ssa_phi *phi = zend_arena_calloc(arena, 1,
					sizeof(zend_ssa_phi) +
					sizeof(int) * blocks[j].predecessors_count +
					sizeof(void*) * blocks[j].predecessors_count);

				phi->sources = (int*)(((char*)phi) + sizeof(zend_ssa_phi));
				memset(phi->sources, 0xff, sizeof(int) * blocks[j].predecessors_count);
				phi->use_chains = (zend_ssa_phi**)(((char*)phi->sources) + sizeof(int) * ssa->cfg.blocks[j].predecessors_count);

				phi->pi = -1;
				phi->var = i;
				phi->ssa_var = -1;

				/* Place phis after pis */
				{
					zend_ssa_phi **pp = &ssa_blocks[j].phis;
					while (*pp) {
						if ((*pp)->pi < 0) {
							break;
						}
						pp = &(*pp)->next;
					}
					phi->next = *pp;
					*pp = phi;
				}
			} ZEND_BITSET_FOREACH_END();
		}
	}

	if (build_flags & ZEND_SSA_DEBUG_PHI_PLACEMENT) {
		zend_dump_phi_placement(op_array, ssa);
	}

	/* SSA construction, Step 3: Renaming */
	ssa->ops = zend_arena_calloc(arena, op_array->last, sizeof(zend_ssa_op));
	memset(ssa->ops, 0xff, op_array->last * sizeof(zend_ssa_op));
	memset(var + op_array->last_var, 0xff, op_array->T * sizeof(int));
	/* Create uninitialized SSA variables for each CV */
	for (j = 0; j < op_array->last_var; j++) {
		var[j] = j;
	}
	ssa->vars_count = op_array->last_var;
	if (zend_ssa_rename(op_array, build_flags, ssa, var, 0) != SUCCESS) {
		free_alloca(var, var_use_heap);
		free_alloca(dfg.tmp, dfg_use_heap);
		return FAILURE;
	}

	free_alloca(var, var_use_heap);
	free_alloca(dfg.tmp, dfg_use_heap);

	return SUCCESS;
}
/* }}} */

int zend_ssa_compute_use_def_chains(zend_arena **arena, const zend_op_array *op_array, zend_ssa *ssa) /* {{{ */
{
	zend_ssa_var *ssa_vars;
	int i;

	if (!ssa->vars) {
		ssa->vars = zend_arena_calloc(arena, ssa->vars_count, sizeof(zend_ssa_var));
	}
	ssa_vars = ssa->vars;

	for (i = 0; i < op_array->last_var; i++) {
		ssa_vars[i].var = i;
		ssa_vars[i].scc = -1;
		ssa_vars[i].definition = -1;
		ssa_vars[i].use_chain = -1;
	}
	for (i = op_array->last_var; i < ssa->vars_count; i++) {
		ssa_vars[i].var = -1;
		ssa_vars[i].scc = -1;
		ssa_vars[i].definition = -1;
		ssa_vars[i].use_chain = -1;
	}

	for (i = op_array->last - 1; i >= 0; i--) {
		zend_ssa_op *op = ssa->ops + i;

		if (op->op1_use >= 0) {
			op->op1_use_chain = ssa_vars[op->op1_use].use_chain;
			ssa_vars[op->op1_use].use_chain = i;
		}
		if (op->op2_use >= 0 && op->op2_use != op->op1_use) {
			op->op2_use_chain = ssa_vars[op->op2_use].use_chain;
			ssa_vars[op->op2_use].use_chain = i;
		}
		if (op->result_use >= 0) {
			op->res_use_chain = ssa_vars[op->result_use].use_chain;
			ssa_vars[op->result_use].use_chain = i;
		}
		if (op->op1_def >= 0) {
			ssa_vars[op->op1_def].var = EX_VAR_TO_NUM(op_array->opcodes[i].op1.var);
			ssa_vars[op->op1_def].definition = i;
		}
		if (op->op2_def >= 0) {
			ssa_vars[op->op2_def].var = EX_VAR_TO_NUM(op_array->opcodes[i].op2.var);
			ssa_vars[op->op2_def].definition = i;
		}
		if (op->result_def >= 0) {
			ssa_vars[op->result_def].var = EX_VAR_TO_NUM(op_array->opcodes[i].result.var);
			ssa_vars[op->result_def].definition = i;
		}
	}

	for (i = 0; i < ssa->cfg.blocks_count; i++) {
		zend_ssa_phi *phi = ssa->blocks[i].phis;
		while (phi) {
			phi->block = i;
			ssa_vars[phi->ssa_var].var = phi->var;
			ssa_vars[phi->ssa_var].definition_phi = phi;
			if (phi->pi >= 0) {
				if (phi->sources[0] >= 0) {
					zend_ssa_phi *p = ssa_vars[phi->sources[0]].phi_use_chain;
					while (p && p != phi) {
						p = zend_ssa_next_use_phi(ssa, phi->sources[0], p);
					}
					if (!p) {
						phi->use_chains[0] = ssa_vars[phi->sources[0]].phi_use_chain;
						ssa_vars[phi->sources[0]].phi_use_chain = phi;
					}
				}
				if (phi->has_range_constraint) {
					/* min and max variables can't be used together */
					zend_ssa_range_constraint *constraint = &phi->constraint.range;
					if (constraint->min_ssa_var >= 0) {
						phi->sym_use_chain = ssa_vars[constraint->min_ssa_var].sym_use_chain;
						ssa_vars[constraint->min_ssa_var].sym_use_chain = phi;
					} else if (constraint->max_ssa_var >= 0) {
						phi->sym_use_chain = ssa_vars[constraint->max_ssa_var].sym_use_chain;
						ssa_vars[constraint->max_ssa_var].sym_use_chain = phi;
					}
				}
			} else {
				int j;

				for (j = 0; j < ssa->cfg.blocks[i].predecessors_count; j++) {
					if (phi->sources[j] >= 0) {
						zend_ssa_phi *p = ssa_vars[phi->sources[j]].phi_use_chain;
						while (p && p != phi) {
							p = zend_ssa_next_use_phi(ssa, phi->sources[j], p);
						}
						if (!p) {
							phi->use_chains[j] = ssa_vars[phi->sources[j]].phi_use_chain;
							ssa_vars[phi->sources[j]].phi_use_chain = phi;
						}
					}
				}
			}
			phi = phi->next;
		}
	}

	return SUCCESS;
}
/* }}} */

int zend_ssa_unlink_use_chain(zend_ssa *ssa, int op, int var) /* {{{ */
{
	if (ssa->vars[var].use_chain == op) {
		ssa->vars[var].use_chain = zend_ssa_next_use(ssa->ops, var, op);
		return 1;
	} else {
		int use = ssa->vars[var].use_chain;

		while (use >= 0) {
			if (ssa->ops[use].result_use == var) {
				if (ssa->ops[use].res_use_chain == op) {
					ssa->ops[use].res_use_chain = zend_ssa_next_use(ssa->ops, var, op);
					return 1;
				} else {
					use = ssa->ops[use].res_use_chain;
				}
			} else if (ssa->ops[use].op1_use == var) {
				if (ssa->ops[use].op1_use_chain == op) {
					ssa->ops[use].op1_use_chain = zend_ssa_next_use(ssa->ops, var, op);
					return 1;
				} else {
					use = ssa->ops[use].op1_use_chain;
				}
			} else if (ssa->ops[use].op2_use == var) {
				if (ssa->ops[use].op2_use_chain == op) {
					ssa->ops[use].op2_use_chain = zend_ssa_next_use(ssa->ops, var, op);
					return 1;
				} else {
					use = ssa->ops[use].op2_use_chain;
				}
			} else {
				break;
			}
		}
		/* something wrong */
		ZEND_ASSERT(0);
		return 0;
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
