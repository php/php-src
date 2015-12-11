/*
   +----------------------------------------------------------------------+
   | Zend Engine, SSA - Static Single Assignment Form                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 The PHP Group                                |
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

static int needs_pi(zend_op_array *op_array, zend_cfg *cfg, zend_dfg *dfg, zend_ssa *ssa, int from, int to, int var) /* {{{ */
{
	if (from == to || cfg->blocks[to].predecessors_count != 1) {
		zend_ssa_phi *p = ssa->blocks[to].phis;
		while (p) {
			if (p->pi < 0 && p->var == var) {
				return 1;
			}
			p = p->next;
		}
		return 0;
	}
	return DFG_ISSET(dfg->in, dfg->size, to, var);
}
/* }}} */

static int add_pi(zend_arena **arena, zend_op_array *op_array, zend_cfg *cfg, zend_dfg *dfg, zend_ssa *ssa, int from, int to, int var, int min_var, int max_var, long min, long max, char underflow, char overflow, char negative) /* {{{ */
{
	if (needs_pi(op_array, cfg, dfg, ssa, from, to, var)) {
		zend_ssa_phi *phi = zend_arena_calloc(arena, 1,
			sizeof(zend_ssa_phi) +
			sizeof(int) * cfg->blocks[to].predecessors_count +
			sizeof(void*) * cfg->blocks[to].predecessors_count);

		if (!phi) {
			return FAILURE;
		}
		phi->sources = (int*)(((char*)phi) + sizeof(zend_ssa_phi));
		memset(phi->sources, 0xff, sizeof(int) * cfg->blocks[to].predecessors_count);
		phi->use_chains = (zend_ssa_phi**)(((char*)phi->sources) + sizeof(int) * cfg->blocks[to].predecessors_count);

		phi->pi = from;
		phi->constraint.min_var = min_var;
		phi->constraint.max_var = max_var;
		phi->constraint.min_ssa_var = -1;
		phi->constraint.max_ssa_var = -1;
		phi->constraint.range.min = min;
		phi->constraint.range.max = max;
		phi->constraint.range.underflow = underflow;
		phi->constraint.range.overflow = overflow;
		phi->constraint.negative = negative ? NEG_INIT : NEG_NONE;
		phi->var = var;
		phi->ssa_var = -1;
		phi->next = ssa->blocks[to].phis;
		ssa->blocks[to].phis = phi;
	}
	return SUCCESS;
}
/* }}} */

static int zend_ssa_rename(zend_op_array *op_array, zend_cfg *cfg, zend_ssa *ssa, int *var, int n) /* {{{ */
{
	zend_basic_block *blocks = cfg->blocks;
	zend_ssa_block *ssa_blocks = ssa->blocks;
	zend_ssa_op *ssa_ops = ssa->ops;
	int ssa_vars_count = ssa->vars_count;
	int i, j;
	uint32_t k;
	zend_op *opline;
	int *tmp = NULL;

	// FIXME: Can we optimize this copying out in some cases?
	if (blocks[n].next_child >= 0) {
		tmp = alloca(sizeof(int) * (op_array->last_var + op_array->T));
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

	for (k = blocks[n].start; k <= blocks[n].end; k++) {
		opline = op_array->opcodes + k;
		if (opline->opcode != ZEND_OP_DATA) {
			zend_op *next = opline + 1;
			if (k < blocks[n].end &&
			    next->opcode == ZEND_OP_DATA) {
				if (next->op1_type == IS_CV) {
					ssa_ops[k + 1].op1_use = var[EX_VAR_TO_NUM(next->op1.var)];
					//USE_SSA_VAR(next->op1.var);
				} else if (next->op1_type == IS_VAR ||
				           next->op1_type == IS_TMP_VAR) {
					ssa_ops[k + 1].op1_use = var[EX_VAR_TO_NUM(next->op1.var)];
					//USE_SSA_VAR(op_array->last_var + next->op1.var);
				}
				if (next->op2_type == IS_CV) {
					ssa_ops[k + 1].op2_use = var[EX_VAR_TO_NUM(next->op2.var)];
					//USE_SSA_VAR(next->op2.var);
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
							ssa_ops[k + 1].op2_use = var[EX_VAR_TO_NUM(next->op2.var)];
							//USE_SSA_VAR(op_array->last_var + next->op2.var);
					}
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
					if (opline->op2_type == IS_CV) {
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
					if (next->op1_type == IS_CV) {
						ssa_ops[k + 1].op1_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(next->op1.var)] = ssa_vars_count;
						ssa_vars_count++;
						//NEW_SSA_VAR(next->op1.var)
					}
					break;
				case ZEND_ADD_ARRAY_ELEMENT:
					ssa_ops[k].result_use = var[EX_VAR_TO_NUM(opline->result.var)];
				case ZEND_INIT_ARRAY:
					if (opline->op1_type == IS_CV) {
						ssa_ops[k].op1_def = ssa_vars_count;
						var[EX_VAR_TO_NUM(opline->op1.var)] = ssa_vars_count;
						ssa_vars_count++;
						//NEW_SSA_VAR(opline+->op1.var)
					}
					break;
				case ZEND_SEND_VAR_NO_REF:
				case ZEND_SEND_VAR_EX:
				case ZEND_SEND_REF:
				case ZEND_FE_RESET_R:
				case ZEND_FE_RESET_RW:
//TODO: ???
					if (opline->op1_type == IS_CV) {
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
						var[EX_VAR_TO_NUM(opline->op1.var)] = EX_VAR_TO_NUM(opline->op1.var);
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
				default:
					break;
			}
			if (opline->result_type == IS_CV) {
				ssa_ops[k].result_def = ssa_vars_count;
				var[EX_VAR_TO_NUM(opline->result.var)] = ssa_vars_count;
				ssa_vars_count++;
				//NEW_SSA_VAR(opline->result.var)
			} else if (opline->result_type == IS_VAR ||
			           opline->result_type == IS_TMP_VAR) {
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
					if (p->constraint.min_var >= 0) {
						p->constraint.min_ssa_var = var[p->constraint.min_var];
					}
					if (p->constraint.max_var >= 0) {
						p->constraint.max_ssa_var = var[p->constraint.max_var];
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
						if (cfg->predecessors[blocks[succ].predecessor_offset + j] == n) {
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
								if (cfg->predecessors[blocks[succ].predecessor_offset + j] == n) {
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
		if (zend_ssa_rename(op_array, cfg, ssa, var, j) != SUCCESS)
			return FAILURE;
		j = blocks[j].next_child;
	}

	return SUCCESS;
}
/* }}} */

int zend_build_ssa(zend_arena **arena, zend_op_array *op_array, zend_cfg *cfg, int rt_constants, zend_ssa *ssa, uint32_t *func_flags) /* {{{ */
{
	zend_basic_block *blocks = cfg->blocks;
	zend_ssa_block *ssa_blocks;
	int blocks_count = cfg->blocks_count;
	uint32_t set_size;
	zend_bitset tmp, gen, in;
	int *var = 0;
	int i, j, k, changed;
	zend_dfg dfg;

	ssa_blocks = zend_arena_calloc(arena, blocks_count, sizeof(zend_ssa_block));
	if (!ssa_blocks) {
		return FAILURE;
	}
	ssa->blocks = ssa_blocks;

	/* Compute Variable Liveness */
	dfg.vars = op_array->last_var + op_array->T;
	dfg.size = set_size = zend_bitset_len(dfg.vars);
	dfg.tmp = alloca((set_size * sizeof(zend_ulong)) * (blocks_count * 5 + 1));
	memset(dfg.tmp, 0, (set_size * sizeof(zend_ulong)) * (blocks_count * 5 + 1));
	dfg.gen = dfg.tmp + set_size;
	dfg.def = dfg.gen + set_size * blocks_count;
	dfg.use = dfg.def + set_size * blocks_count;
	dfg.in  = dfg.use + set_size * blocks_count;
	dfg.out = dfg.in  + set_size * blocks_count;

	if (zend_build_dfg(op_array, cfg, &dfg) != SUCCESS) {
		return FAILURE;
	}

	tmp = dfg.tmp;
	gen = dfg.gen;
	in  = dfg.in;

	/* SSA construction, Step 1: Propagate "gen" sets in merge points */
	do {
		changed = 0;
		for (j = 0; j < blocks_count; j++) {
			if ((blocks[j].flags & ZEND_BB_REACHABLE) == 0) {
				continue;
			}
			if (j >= 0 && (blocks[j].predecessors_count > 1 || j == 0)) {
				zend_bitset_copy(tmp, gen + (j * set_size), set_size);
				for (k = 0; k < blocks[j].predecessors_count; k++) {
					i = cfg->predecessors[blocks[j].predecessor_offset + k];
					while (i != blocks[j].idom) {
						zend_bitset_union_with_intersection(tmp, tmp, gen + (i * set_size), in + (j * set_size), set_size);
						i = blocks[i].idom;
					}
				}
				if (!zend_bitset_equal(gen + (j * set_size), tmp, set_size)) {
					zend_bitset_copy(gen + (j * set_size), tmp, set_size);
					changed = 1;
				}
			}
		}
	} while (changed);

	/* SSA construction, Step 2: Phi placement based on Dominance Frontiers */
	var = alloca(sizeof(int) * (op_array->last_var + op_array->T));
	if (!var) {
		return FAILURE;
	}
	zend_bitset_clear(tmp, set_size);

	for (j = 0; j < blocks_count; j++) {
		if ((blocks[j].flags & ZEND_BB_REACHABLE) == 0) {
			continue;
		}
		if (blocks[j].predecessors_count > 1) {
			zend_bitset_clear(tmp, set_size);
			if (blocks[j].flags & ZEND_BB_IRREDUCIBLE_LOOP) {
				/* Prevent any values from flowing into irreducible loops by
				   replacing all incoming values with explicit phis.  The
				   register allocator depends on this property.  */
				zend_bitset_copy(tmp, in + (j * set_size), set_size);
			} else {
				for (k = 0; k < blocks[j].predecessors_count; k++) {
					i = cfg->predecessors[blocks[j].predecessor_offset + k];
					while (i != blocks[j].idom) {
						zend_bitset_union_with_intersection(tmp, tmp, gen + (i * set_size), in + (j * set_size), set_size);
						i = blocks[i].idom;
					}
				}
			}

			if (!zend_bitset_empty(tmp, set_size)) {
				i = op_array->last_var + op_array->T;
				while (i > 0) {
					i--;
					if (zend_bitset_in(tmp, i)) {
						zend_ssa_phi *phi = zend_arena_calloc(arena, 1,
							sizeof(zend_ssa_phi) +
							sizeof(int) * blocks[j].predecessors_count +
							sizeof(void*) * blocks[j].predecessors_count);

						if (!phi)
							return FAILURE;
						phi->sources = (int*)(((char*)phi) + sizeof(zend_ssa_phi));
						memset(phi->sources, 0xff, sizeof(int) * blocks[j].predecessors_count);
						phi->use_chains = (zend_ssa_phi**)(((char*)phi->sources) + sizeof(int) * cfg->blocks[j].predecessors_count);

					    phi->pi = -1;
						phi->var = i;
						phi->ssa_var = -1;
						phi->next = ssa_blocks[j].phis;
						ssa_blocks[j].phis = phi;
					}
				}
			}
		}
	}

	/* e-SSA construction: Pi placement (Pi is actually a Phi with single
	 * source and constraint).
	 * Order of Phis is importent, Pis must be placed before Phis
	 */
	for (j = 0; j < blocks_count; j++) {
		zend_op *opline = op_array->opcodes + cfg->blocks[j].end;
		int bt; /* successor block number if a condition is true */
		int bf; /* successor block number if a condition is false */

		if ((blocks[j].flags & ZEND_BB_REACHABLE) == 0) {
			continue;
		}
		/* the last instruction of basic block is conditional branch,
		 * based on comparison of CV(s)
		 */
		switch (opline->opcode) {
			case ZEND_JMPZ:
				if (cfg->blocks[cfg->blocks[j].successors[0]].start == OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes) {
					bf = cfg->blocks[j].successors[0];
					bt = cfg->blocks[j].successors[1];
				} else {
					bt = cfg->blocks[j].successors[0];
					bf = cfg->blocks[j].successors[1];
				}
				break;
			case ZEND_JMPNZ:
				if (cfg->blocks[cfg->blocks[j].successors[0]].start == OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes) {
					bt = cfg->blocks[j].successors[0];
					bf = cfg->blocks[j].successors[1];
				} else {
					bf = cfg->blocks[j].successors[0];
					bt = cfg->blocks[j].successors[1];
				}
				break;
		    case ZEND_JMPZNZ:
				if (cfg->blocks[cfg->blocks[j].successors[0]].start == OP_JMP_ADDR(opline, opline->op2) - op_array->opcodes) {
					bf = cfg->blocks[j].successors[0];
					bt = cfg->blocks[j].successors[1];
				} else {
					bt = cfg->blocks[j].successors[0];
					bf = cfg->blocks[j].successors[1];
				}
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
			long val1 = 0;
			long val2 = 0;
//			long val = 0;

			if ((opline-1)->op1_type == IS_CV) {
				var1 = EX_VAR_TO_NUM((opline-1)->op1.var);
			} else if ((opline-1)->op1_type == IS_TMP_VAR) {
				zend_op *op = opline;
				while (op != op_array->opcodes) {
					op--;
					if (op->result_type == IS_TMP_VAR &&
					    op->result.var == (opline-1)->op1.var) {
					    if (op->opcode == ZEND_POST_DEC) {
							if (op->op1_type == IS_CV) {
								var1 = EX_VAR_TO_NUM(op->op1.var);
								val2--;
							}
					    } else if (op->opcode == ZEND_POST_INC) {
							if (op->op1_type == IS_CV) {
								var1 = EX_VAR_TO_NUM(op->op1.var);
								val2++;
							}
					    } else if (op->opcode == ZEND_ADD) {
							if (op->op1_type == IS_CV &&
							    op->op2_type == IS_CONST &&
							    Z_TYPE_P(CRT_CONSTANT(op->op2)) == IS_LONG) {
								var1 = EX_VAR_TO_NUM(op->op1.var);
								val2 -= Z_LVAL_P(CRT_CONSTANT(op->op2));
							} else if (op->op2_type == IS_CV &&
							           op->op1_type == IS_CONST &&
							    Z_TYPE_P(CRT_CONSTANT(op->op1)) == IS_LONG) {
								var1 = EX_VAR_TO_NUM(op->op2.var);
								val2 -= Z_LVAL_P(CRT_CONSTANT(op->op1));
							}
					    } else if (op->opcode == ZEND_SUB) {
							if (op->op1_type == IS_CV &&
							    op->op2_type == IS_CONST &&
							    Z_TYPE_P(CRT_CONSTANT(op->op2)) == IS_LONG) {
								var1 = EX_VAR_TO_NUM(op->op1.var);
								val2 += Z_LVAL_P(CRT_CONSTANT(op->op2));
							}
					    }
					    break;
					}
				}
			}

			if ((opline-1)->op2_type == IS_CV) {
				var2 = EX_VAR_TO_NUM((opline-1)->op2.var);
			} else if ((opline-1)->op2_type == IS_TMP_VAR) {
				zend_op *op = opline;
				while (op != op_array->opcodes) {
					op--;
					if (op->result_type == IS_TMP_VAR &&
					    op->result.var == (opline-1)->op2.var) {
					    if (op->opcode == ZEND_POST_DEC) {
							if (op->op1_type == IS_CV) {
								var2 = EX_VAR_TO_NUM(op->op1.var);
								val1--;
							}
					    } else if (op->opcode == ZEND_POST_INC) {
							if (op->op1_type == IS_CV) {
								var2 = EX_VAR_TO_NUM(op->op1.var);
								val1++;
							}
					    } else if (op->opcode == ZEND_ADD) {
							if (op->op1_type == IS_CV &&
							    op->op2_type == IS_CONST &&
							    Z_TYPE_P(CRT_CONSTANT(op->op2)) == IS_LONG) {
								var2 = EX_VAR_TO_NUM(op->op1.var);
								val1 -= Z_LVAL_P(CRT_CONSTANT(op->op2));
							} else if (op->op2_type == IS_CV &&
							           op->op1_type == IS_CONST &&
							    Z_TYPE_P(CRT_CONSTANT(op->op1)) == IS_LONG) {
								var2 = EX_VAR_TO_NUM(op->op2.var);
								val1 -= Z_LVAL_P(CRT_CONSTANT(op->op1));
							}
					    } else if (op->opcode == ZEND_SUB) {
							if (op->op1_type == IS_CV &&
							    op->op2_type == IS_CONST &&
							    Z_TYPE_P(CRT_CONSTANT(op->op2)) == IS_LONG) {
								var2 = EX_VAR_TO_NUM(op->op1.var);
								val1 += Z_LVAL_P(CRT_CONSTANT(op->op2));
							}
					    }
					    break;
					}
				}
			}

			if (var1 >= 0 && var2 >= 0) {
				int tmp = val1;
				val1 -= val2;
				val2 -= tmp;
			} else if (var1 >= 0 && var2 < 0) {
				if ((opline-1)->op2_type == IS_CONST &&
				    Z_TYPE_P(CRT_CONSTANT((opline-1)->op2)) == IS_LONG) {
					val2 += Z_LVAL_P(CRT_CONSTANT((opline-1)->op2));
				} else if ((opline-1)->op2_type == IS_CONST &&
				    Z_TYPE_P(CRT_CONSTANT((opline-1)->op2)) == IS_FALSE) {
					val2 += 0;
				} else if ((opline-1)->op2_type == IS_CONST &&
				    Z_TYPE_P(CRT_CONSTANT((opline-1)->op2)) == IS_TRUE) {
					val2 += 12;
				} else {
					var1 = -1;
				}
			} else if (var1 < 0 && var2 >= 0) {
				if ((opline-1)->op1_type == IS_CONST &&
				    Z_TYPE_P(CRT_CONSTANT((opline-1)->op1)) == IS_LONG) {
					val1 += Z_LVAL_P(CRT_CONSTANT((opline-1)->op1));
				} else if ((opline-1)->op1_type == IS_CONST &&
				    Z_TYPE_P(CRT_CONSTANT((opline-1)->op1)) == IS_FALSE) {
					val1 += 0;
				} else if ((opline-1)->op1_type == IS_CONST &&
				    Z_TYPE_P(CRT_CONSTANT((opline-1)->op1)) == IS_TRUE) {
					val1 += 1;
				} else {
					var2 = -1;
				}
			}

			if (var1 >= 0) {
				if ((opline-1)->opcode == ZEND_IS_EQUAL) {
					if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bt, var1, var2, var2, val2, val2, 0, 0, 0) != SUCCESS) {
						return FAILURE;
					}
					if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bf, var1, var2, var2, val2, val2, 0, 0, 1) != SUCCESS) {
						return FAILURE;
					}
				} else if ((opline-1)->opcode == ZEND_IS_NOT_EQUAL) {
					if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bf, var1, var2, var2, val2, val2, 0, 0, 0) != SUCCESS) {
						return FAILURE;
					}
					if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bt, var1, var2, var2, val2, val2, 0, 0, 1) != SUCCESS) {
						return FAILURE;
					}
				} else if ((opline-1)->opcode == ZEND_IS_SMALLER) {
					if (val2 > LONG_MIN) {
						if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bt, var1, -1, var2, LONG_MIN, val2-1, 1, 0, 0) != SUCCESS) {
							return FAILURE;
						}
					}
					if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bf, var1, var2, -1, val2, LONG_MAX, 0, 1, 0) != SUCCESS) {
						return FAILURE;
					}
				} else if ((opline-1)->opcode == ZEND_IS_SMALLER_OR_EQUAL) {
					if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bt, var1, -1, var2, LONG_MIN, val2, 1, 0, 0) != SUCCESS) {
						return FAILURE;
					}
					if (val2 < LONG_MAX) {
						if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bf, var1, var2, -1, val2+1, LONG_MAX, 0, 1, 0) != SUCCESS) {
							return FAILURE;
						}
					}
				}
			}
			if (var2 >= 0) {
				if((opline-1)->opcode == ZEND_IS_EQUAL) {
					if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bt, var2, var1, var1, val1, val1, 0, 0, 0) != SUCCESS) {
						return FAILURE;
					}
					if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bf, var2, var1, var1, val1, val1, 0, 0, 1) != SUCCESS) {
						return FAILURE;
					}
				} else if ((opline-1)->opcode == ZEND_IS_NOT_EQUAL) {
					if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bf, var2, var1, var1, val1, val1, 0, 0, 0) != SUCCESS) {
						return FAILURE;
					}
					if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bt, var2, var1, var1, val1, val1, 0, 0, 1) != SUCCESS) {
						return FAILURE;
					}
				} else if ((opline-1)->opcode == ZEND_IS_SMALLER) {
					if (val1 < LONG_MAX) {
						if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bt, var2, var1, -1, val1+1, LONG_MAX, 0, 1, 0) != SUCCESS) {
							return FAILURE;
						}
					}
					if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bf, var2, -1, var1, LONG_MIN, val1, 1, 0, 0) != SUCCESS) {
						return FAILURE;
					}
				} else if ((opline-1)->opcode == ZEND_IS_SMALLER_OR_EQUAL) {
					if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bt, var2, var1, -1, val1, LONG_MAX, 0 ,1, 0) != SUCCESS) {
						return FAILURE;
					}
					if (val1 > LONG_MIN) {
						if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bf, var2, -1, var1, LONG_MIN, val1-1, 1, 0, 0) != SUCCESS) {
							return FAILURE;
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
				if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bf, var, -1, -1, -1, -1, 0, 0, 0) != SUCCESS) {
					return FAILURE;
				}
				if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bt, var, -1, -1, -1, -1, 0, 0, 1) != SUCCESS) {
					return FAILURE;
				}
			} else if ((opline-1)->opcode == ZEND_POST_INC) {
				if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bf, var, -1, -1, 1, 1, 0, 0, 0) != SUCCESS) {
					return FAILURE;
				}
				if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bt, var, -1, -1, 1, 1, 0, 0, 1) != SUCCESS) {
					return FAILURE;
				}
			}
		} else if (opline->op1_type == IS_VAR &&
		           ((opline-1)->opcode == ZEND_PRE_INC ||
		            (opline-1)->opcode == ZEND_PRE_DEC) &&
		           opline->op1.var == (opline-1)->result.var &&
		           (opline-1)->op1_type == IS_CV) {
			int var = EX_VAR_TO_NUM((opline-1)->op1.var);

			if ((opline-1)->opcode == ZEND_PRE_DEC) {
				if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bf, var, -1, -1, 0, 0, 0, 0, 0) != SUCCESS) {
					return FAILURE;
				}
				/* speculative */
				if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bt, var, -1, -1, 0, 0, 0, 0, 1) != SUCCESS) {
					return FAILURE;
				}
			} else if ((opline-1)->opcode == ZEND_PRE_INC) {
				if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bf, var, -1, -1, 0, 0, 0, 0, 0) != SUCCESS) {
					return FAILURE;
				}
				/* speculative */
				if (add_pi(arena, op_array, cfg, &dfg, ssa, j, bt, var, -1, -1, 0, 0, 0, 0, 1) != SUCCESS) {
					return FAILURE;
				}
			}
		}
	}

	/* SSA construction, Step ?: Phi after Pi placement based on Dominance Frontiers */
	for (j = 0; j < blocks_count; j++) {
		if ((blocks[j].flags & ZEND_BB_REACHABLE) == 0) {
			continue;
		}
		if (blocks[j].predecessors_count > 1) {
			zend_bitset_clear(tmp, set_size);
			if (blocks[j].flags & ZEND_BB_IRREDUCIBLE_LOOP) {
				/* Prevent any values from flowing into irreducible loops by
				   replacing all incoming values with explicit phis.  The
				   register allocator depends on this property.  */
				zend_bitset_copy(tmp, in + (j * set_size), set_size);
			} else {
				for (k = 0; k < blocks[j].predecessors_count; k++) {
					i = cfg->predecessors[blocks[j].predecessor_offset + k];
					while (i != blocks[j].idom) {
						zend_ssa_phi *p = ssa_blocks[i].phis;
						while (p) {
							if (p) {
								if (p->pi >= 0) {
									if (zend_bitset_in(in + (j * set_size), p->var) &&
									    !zend_bitset_in(gen + (i * set_size), p->var)) {
										zend_bitset_incl(tmp, p->var);
									}
								} else {
									zend_bitset_excl(tmp, p->var);
								}
							}
							p = p->next;
						}
						i = blocks[i].idom;
					}
				}
			}

			if (!zend_bitset_empty(tmp, set_size)) {
				i = op_array->last_var + op_array->T;
				while (i > 0) {
					i--;
					if (zend_bitset_in(tmp, i)) {
						zend_ssa_phi **pp = &ssa_blocks[j].phis;
						while (*pp) {
							if ((*pp)->pi <= 0 && (*pp)->var == i) {
								break;
							}
							pp = &(*pp)->next;
						}
						if (*pp == NULL) {
							zend_ssa_phi *phi = zend_arena_calloc(arena, 1,
								sizeof(zend_ssa_phi) +
								sizeof(int) * blocks[j].predecessors_count +
								sizeof(void*) * blocks[j].predecessors_count);

							if (!phi)
								return FAILURE;
							phi->sources = (int*)(((char*)phi) + sizeof(zend_ssa_phi));
							memset(phi->sources, 0xff, sizeof(int) * blocks[j].predecessors_count);
							phi->use_chains = (zend_ssa_phi**)(((char*)phi->sources) + sizeof(int) * cfg->blocks[j].predecessors_count);

						    phi->pi = -1;
							phi->var = i;
							phi->ssa_var = -1;
							phi->next = NULL;
							*pp = phi;
						}
					}
				}
			}
		}
	}

//???D	if (ZCG(accel_directives).jit_debug & JIT_DEBUG_DUMP_PHI) {
//???D		zend_jit_dump(op_array, JIT_DUMP_PHI_PLACEMENT);
//???D	}

	/* SSA construction, Step 3: Renaming */
	ssa->ops = zend_arena_calloc(arena, op_array->last, sizeof(zend_ssa_op));
	memset(ssa->ops, 0xff, op_array->last * sizeof(zend_ssa_op));
	memset(var, 0xff, (op_array->last_var + op_array->T) * sizeof(int));
	/* Create uninitialized SSA variables for each CV */
	for (j = 0; j < op_array->last_var; j++) {
		var[j] = j;
	}
	ssa->vars_count = op_array->last_var;
	if (zend_ssa_rename(op_array, cfg, ssa, var, 0) != SUCCESS) {
		return FAILURE;
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
