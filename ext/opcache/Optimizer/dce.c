/*
   +----------------------------------------------------------------------+
   | Zend Engine, DCE - Dead Code Elimination                             |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Nikita Popov <nikic@php.net>                                |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/zend_inference.h"
#include "Optimizer/zend_ssa.h"
#include "Optimizer/zend_func_info.h"
#include "Optimizer/zend_call_graph.h"
#include "zend_bitset.h"

/* This pass implements a form of dead code elimination (DCE). The algorithm optimistically assumes
 * that all instructions and phis are dead. Instructions with immediate side-effects are then marked
 * as live. We then recursively (using a worklist) propagate liveness to the instructions that def
 * the used operands.
 *
 * Notes:
 *  * This pass does not perform unreachable code elimination. This happens as part of the SCCP
 *    pass.
 *  * The DCE is performed without taking control-dependence into account, i.e. all conditional
 *    branches are assumed to be live. It's possible to take control-dependence into account using
 *    the DCE algorithm described by Cytron et al., however it requires the construction of a
 *    postdominator tree and of postdominance frontiers, which does not seem worthwhile at this
 *    point.
 *  * We separate intrinsic side-effects from potential side-effects in the form of notices thrown
 *    by the instruction (in case we want to make this configurable). See may_have_side_effects() and
 *    zend_may_throw().
 *  * We often cannot DCE assignments and unsets while guaranteeing that dtors run in the same
 *    order. There is an optimization option to allow reordering of dtor effects.
 *  * The algorithm is able to eliminate dead modifications of non-escaping arrays
 *    and objects as well as dead arrays and objects allocations.
 */

typedef struct {
	zend_ssa *ssa;
	zend_op_array *op_array;
	zend_bitset instr_dead;
	zend_bitset phi_dead;
	zend_bitset instr_worklist;
	zend_bitset phi_worklist;
	zend_bitset phi_worklist_no_val;
	uint32_t instr_worklist_len;
	uint32_t phi_worklist_len;
	unsigned reorder_dtor_effects : 1;
} context;

static inline zend_bool is_bad_mod(const zend_ssa *ssa, int use, int def) {
	if (def < 0) {
		/* This modification is not tracked by SSA, assume the worst */
		return 1;
	}
	if (ssa->var_info[use].type & MAY_BE_REF) {
		/* Modification of reference may have side-effect */
		return 1;
	}
	return 0;
}

static inline zend_bool may_have_side_effects(
		zend_op_array *op_array, zend_ssa *ssa,
		const zend_op *opline, const zend_ssa_op *ssa_op,
		zend_bool reorder_dtor_effects) {
	switch (opline->opcode) {
		case ZEND_NOP:
		case ZEND_IS_IDENTICAL:
		case ZEND_IS_NOT_IDENTICAL:
		case ZEND_QM_ASSIGN:
		case ZEND_FREE:
		case ZEND_TYPE_CHECK:
		case ZEND_DEFINED:
		case ZEND_ADD:
		case ZEND_SUB:
		case ZEND_MUL:
		case ZEND_POW:
		case ZEND_BW_OR:
		case ZEND_BW_AND:
		case ZEND_BW_XOR:
		case ZEND_CONCAT:
		case ZEND_FAST_CONCAT:
		case ZEND_DIV:
		case ZEND_MOD:
		case ZEND_BOOL_XOR:
		case ZEND_BOOL:
		case ZEND_BOOL_NOT:
		case ZEND_BW_NOT:
		case ZEND_SL:
		case ZEND_SR:
		case ZEND_IS_EQUAL:
		case ZEND_IS_NOT_EQUAL:
		case ZEND_IS_SMALLER:
		case ZEND_IS_SMALLER_OR_EQUAL:
		case ZEND_CASE:
		case ZEND_CAST:
		case ZEND_ROPE_INIT:
		case ZEND_ROPE_ADD:
		case ZEND_INIT_ARRAY:
		case ZEND_ADD_ARRAY_ELEMENT:
		case ZEND_SPACESHIP:
		case ZEND_STRLEN:
		case ZEND_COUNT:
		case ZEND_GET_TYPE:
		case ZEND_ISSET_ISEMPTY_THIS:
		case ZEND_ISSET_ISEMPTY_DIM_OBJ:
		case ZEND_FETCH_DIM_IS:
		case ZEND_ISSET_ISEMPTY_CV:
		case ZEND_ISSET_ISEMPTY_VAR:
		case ZEND_FETCH_IS:
		case ZEND_IN_ARRAY:
		case ZEND_FUNC_NUM_ARGS:
		case ZEND_FUNC_GET_ARGS:
			/* No side effects */
			return 0;
		case ZEND_ROPE_END:
			/* TODO: Rope dce optimization, see #76446 */
			return 1;
		case ZEND_JMP:
		case ZEND_JMPZ:
		case ZEND_JMPNZ:
		case ZEND_JMPZNZ:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_ASSERT_CHECK:
			/* For our purposes a jumps and branches are side effects. */
			return 1;
		case ZEND_BEGIN_SILENCE:
		case ZEND_END_SILENCE:
		case ZEND_ECHO:
		case ZEND_INCLUDE_OR_EVAL:
		case ZEND_THROW:
		case ZEND_EXT_STMT:
		case ZEND_EXT_FCALL_BEGIN:
		case ZEND_EXT_FCALL_END:
		case ZEND_EXT_NOP:
		case ZEND_TICKS:
		case ZEND_YIELD:
		case ZEND_YIELD_FROM:
			/* Intrinsic side effects */
			return 1;
		case ZEND_DO_FCALL:
		case ZEND_DO_FCALL_BY_NAME:
		case ZEND_DO_ICALL:
		case ZEND_DO_UCALL:
			/* For now assume all calls have side effects */
			return 1;
		case ZEND_RECV:
		case ZEND_RECV_INIT:
			/* Even though RECV_INIT can be side-effect free, these cannot be simply dropped
			 * due to the prologue skipping code. */
			return 1;
		case ZEND_ASSIGN_REF:
			return 1;
		case ZEND_ASSIGN:
		{
			if (is_bad_mod(ssa, ssa_op->op1_use, ssa_op->op1_def)) {
				return 1;
			}
			if (!reorder_dtor_effects) {
				if (opline->op2_type != IS_CONST
					&& (OP2_INFO() & MAY_HAVE_DTOR)
					&& ssa->vars[ssa_op->op2_use].escape_state != ESCAPE_STATE_NO_ESCAPE) {
					/* DCE might shorten lifetime */
					return 1;
				}
			}
			return 0;
		}
		case ZEND_UNSET_VAR:
			return 1;
		case ZEND_UNSET_CV:
		{
			uint32_t t1 = OP1_INFO();
			if (t1 & MAY_BE_REF) {
				/* We don't consider uses as the LHS of an assignment as real uses during DCE, so
				 * an unset may be considered dead even if there is a later assignment to the
				 * variable. Removing the unset in this case would not be correct if the variable
				 * is a reference, because unset breaks references. */
				return 1;
			}
			return 0;
		}
		case ZEND_PRE_INC:
		case ZEND_POST_INC:
		case ZEND_PRE_DEC:
		case ZEND_POST_DEC:
			return is_bad_mod(ssa, ssa_op->op1_use, ssa_op->op1_def);
		case ZEND_ASSIGN_OP:
			return is_bad_mod(ssa, ssa_op->op1_use, ssa_op->op1_def)
				|| ssa->vars[ssa_op->op1_def].escape_state != ESCAPE_STATE_NO_ESCAPE;
		case ZEND_ASSIGN_DIM:
		case ZEND_ASSIGN_OBJ:
			if (is_bad_mod(ssa, ssa_op->op1_use, ssa_op->op1_def)
				|| ssa->vars[ssa_op->op1_def].escape_state != ESCAPE_STATE_NO_ESCAPE) {
				return 1;
			}
			if (!reorder_dtor_effects) {
				opline++;
				ssa_op++;
				if (opline->op1_type != IS_CONST
					&& (OP1_INFO() & MAY_HAVE_DTOR)) {
					/* DCE might shorten lifetime */
					return 1;
				}
			}
			return 0;
		case ZEND_PRE_INC_OBJ:
		case ZEND_PRE_DEC_OBJ:
		case ZEND_POST_INC_OBJ:
		case ZEND_POST_DEC_OBJ:
			if (is_bad_mod(ssa, ssa_op->op1_use, ssa_op->op1_def)
				|| ssa->vars[ssa_op->op1_def].escape_state != ESCAPE_STATE_NO_ESCAPE) {
				return 1;
			}
			return 0;
		case ZEND_BIND_STATIC:
			if (op_array->static_variables
			 && (opline->extended_value & ZEND_BIND_REF) != 0) {
				zval *value =
					(zval*)((char*)op_array->static_variables->arData +
						(opline->extended_value & ~ZEND_BIND_REF));
				if (Z_TYPE_P(value) == IS_CONSTANT_AST) {
					/* AST may contain undefined constants */
					return 1;
				}
			}
			return 0;
		default:
			/* For everything we didn't handle, assume a side-effect */
			return 1;
	}
}

static zend_always_inline void add_to_worklists(context *ctx, int var_num, int check) {
	zend_ssa_var *var = &ctx->ssa->vars[var_num];
	if (var->definition >= 0) {
		if (!check || zend_bitset_in(ctx->instr_dead, var->definition)) {
			zend_bitset_incl(ctx->instr_worklist, var->definition);
		}
	} else if (var->definition_phi) {
		if (!check || zend_bitset_in(ctx->phi_dead, var_num)) {
			zend_bitset_incl(ctx->phi_worklist, var_num);
		}
	}
}

static inline void add_to_phi_worklist_no_val(context *ctx, int var_num) {
	zend_ssa_var *var = &ctx->ssa->vars[var_num];
	if (var->definition_phi && zend_bitset_in(ctx->phi_dead, var_num)) {
		zend_bitset_incl(ctx->phi_worklist_no_val, var_num);
	}
}

static zend_always_inline void add_operands_to_worklists(context *ctx, zend_op *opline, zend_ssa_op *ssa_op, zend_ssa *ssa, int check) {
	if (ssa_op->result_use >= 0) {
		add_to_worklists(ctx, ssa_op->result_use, check);
	}
	if (ssa_op->op1_use >= 0) {
		if (!zend_ssa_is_no_val_use(opline, ssa_op, ssa_op->op1_use)
		 || (opline->opcode == ZEND_ASSIGN
		  && (ssa->var_info[ssa_op->op1_use].type & MAY_BE_REF) != 0)) {
			add_to_worklists(ctx, ssa_op->op1_use, check);
		} else {
			add_to_phi_worklist_no_val(ctx, ssa_op->op1_use);
		}
	}
	if (ssa_op->op2_use >= 0) {
		if (!zend_ssa_is_no_val_use(opline, ssa_op, ssa_op->op2_use)
		 || (opline->opcode == ZEND_FE_FETCH_R
		  && (ssa->var_info[ssa_op->op2_use].type & MAY_BE_REF) != 0)) {
			add_to_worklists(ctx, ssa_op->op2_use, check);
		} else {
			add_to_phi_worklist_no_val(ctx, ssa_op->op2_use);
		}
	}
}

static zend_always_inline void add_phi_sources_to_worklists(context *ctx, zend_ssa_phi *phi, int check) {
	zend_ssa *ssa = ctx->ssa;
	int source;
	FOREACH_PHI_SOURCE(phi, source) {
		add_to_worklists(ctx, source, check);
	} FOREACH_PHI_SOURCE_END();
}

static inline zend_bool is_var_dead(context *ctx, int var_num) {
	zend_ssa_var *var = &ctx->ssa->vars[var_num];
	if (var->definition_phi) {
		return zend_bitset_in(ctx->phi_dead, var_num);
	} else if (var->definition >= 0) {
		return zend_bitset_in(ctx->instr_dead, var->definition);
	} else {
		/* Variable has no definition, so either the definition has already been removed (var is
		 * dead) or this is one of the implicit variables at the start of the function (for our
		 * purposes live) */
		return var_num >= ctx->op_array->last_var;
	}
}

// Sometimes we can mark the var as EXT_UNUSED
static zend_bool try_remove_var_def(context *ctx, int free_var, int use_chain, zend_op *opline) {
	if (use_chain >= 0) {
		return 0;
	}
	zend_ssa_var *var = &ctx->ssa->vars[free_var];
	int def = var->definition;

	if (def >= 0) {
		zend_ssa_op *def_op = &ctx->ssa->ops[def];

		if (def_op->result_def == free_var
				&& var->phi_use_chain == NULL
				&& var->use_chain == (opline - ctx->op_array->opcodes)) {
			zend_op *def_opline = &ctx->op_array->opcodes[def];

			switch (def_opline->opcode) {
				case ZEND_ASSIGN:
				case ZEND_ASSIGN_REF:
				case ZEND_ASSIGN_DIM:
				case ZEND_ASSIGN_OBJ:
				case ZEND_ASSIGN_OBJ_REF:
				case ZEND_ASSIGN_STATIC_PROP:
				case ZEND_ASSIGN_STATIC_PROP_REF:
				case ZEND_ASSIGN_OP:
				case ZEND_ASSIGN_DIM_OP:
				case ZEND_ASSIGN_OBJ_OP:
				case ZEND_ASSIGN_STATIC_PROP_OP:
				case ZEND_PRE_INC:
				case ZEND_PRE_DEC:
				case ZEND_PRE_INC_OBJ:
				case ZEND_POST_INC_OBJ:
				case ZEND_PRE_DEC_OBJ:
				case ZEND_POST_DEC_OBJ:
				case ZEND_DO_ICALL:
				case ZEND_DO_UCALL:
				case ZEND_DO_FCALL_BY_NAME:
				case ZEND_DO_FCALL:
				case ZEND_INCLUDE_OR_EVAL:
				case ZEND_YIELD:
				case ZEND_YIELD_FROM:
				case ZEND_ASSERT_CHECK:
					def_opline->result_type = IS_UNUSED;
					def_opline->result.var = 0;
					def_op->result_def = -1;
					var->definition = -1;
					return 1;
				default:
					break;
			}
		}
	}
	return 0;
}

/* Returns whether the instruction has been DCEd */
static zend_bool dce_instr(context *ctx, zend_op *opline, zend_ssa_op *ssa_op) {
	zend_ssa *ssa = ctx->ssa;
	int free_var = -1;
	zend_uchar free_var_type;

	if (opline->opcode == ZEND_NOP) {
		return 0;
	}

	/* We mark FREEs as dead, but they're only really dead if the destroyed var is dead */
	if (opline->opcode == ZEND_FREE
			&& (ssa->var_info[ssa_op->op1_use].type & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))
			&& !is_var_dead(ctx, ssa_op->op1_use)) {
		return 0;
	}

	if ((opline->op1_type & (IS_VAR|IS_TMP_VAR))&& !is_var_dead(ctx, ssa_op->op1_use)) {
		if (!try_remove_var_def(ctx, ssa_op->op1_use, ssa_op->op1_use_chain, opline)) {
			if (ssa->var_info[ssa_op->op1_use].type & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF)
				&& opline->opcode != ZEND_CASE) {
				free_var = ssa_op->op1_use;
				free_var_type = opline->op1_type;
			}
		}
	}
	if ((opline->op2_type & (IS_VAR|IS_TMP_VAR)) && !is_var_dead(ctx, ssa_op->op2_use)) {
		if (!try_remove_var_def(ctx, ssa_op->op2_use, ssa_op->op2_use_chain, opline)) {
			if (ssa->var_info[ssa_op->op2_use].type & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF)) {
				if (free_var >= 0) {
					// TODO: We can't free two vars. Keep instruction alive.
					zend_bitset_excl(ctx->instr_dead, opline - ctx->op_array->opcodes);
					return 0;
				}
				free_var = ssa_op->op2_use;
				free_var_type = opline->op2_type;
			}
		}
	}

	zend_ssa_rename_defs_of_instr(ctx->ssa, ssa_op);
	zend_ssa_remove_instr(ctx->ssa, opline, ssa_op);

	if (free_var >= 0) {
		opline->opcode = ZEND_FREE;
		opline->op1.var = (uintptr_t) ZEND_CALL_VAR_NUM(NULL, ssa->vars[free_var].var);
		opline->op1_type = free_var_type;

		ssa_op->op1_use = free_var;
		ssa_op->op1_use_chain = ssa->vars[free_var].use_chain;
		ssa->vars[free_var].use_chain = ssa_op - ssa->ops;
		return 0;
	}
	return 1;
}

static inline int get_common_phi_source(zend_ssa *ssa, zend_ssa_phi *phi) {
	int common_source = -1;
	int source;
	FOREACH_PHI_SOURCE(phi, source) {
		if (common_source == -1) {
			common_source = source;
		} else if (common_source != source && source != phi->ssa_var) {
			return -1;
		}
	} FOREACH_PHI_SOURCE_END();
	ZEND_ASSERT(common_source != -1);
	return common_source;
}

static void try_remove_trivial_phi(context *ctx, zend_ssa_phi *phi) {
	zend_ssa *ssa = ctx->ssa;
	if (phi->pi < 0) {
		/* Phi assignment with identical source operands */
		int common_source = get_common_phi_source(ssa, phi);
		if (common_source >= 0) {
			zend_ssa_rename_var_uses(ssa, phi->ssa_var, common_source, 1);
			zend_ssa_remove_phi(ssa, phi);
		}
	} else {
		/* Pi assignment that is only used in Phi/Pi assignments */
		// TODO What if we want to rerun type inference after DCE? Maybe separate this?
		/*ZEND_ASSERT(phi->sources[0] != -1);
		if (ssa->vars[phi->ssa_var].use_chain < 0) {
			zend_ssa_rename_var_uses_keep_types(ssa, phi->ssa_var, phi->sources[0], 1);
			zend_ssa_remove_phi(ssa, phi);
		}*/
	}
}

static inline zend_bool may_break_varargs(const zend_op_array *op_array, const zend_ssa *ssa, const zend_ssa_op *ssa_op) {
	if (ssa_op->op1_def >= 0
			&& ssa->vars[ssa_op->op1_def].var < op_array->num_args) {
		return 1;
	}
	if (ssa_op->op2_def >= 0
			&& ssa->vars[ssa_op->op2_def].var < op_array->num_args) {
		return 1;
	}
	if (ssa_op->result_def >= 0
			&& ssa->vars[ssa_op->result_def].var < op_array->num_args) {
		return 1;
	}
	return 0;
}

int dce_optimize_op_array(zend_op_array *op_array, zend_ssa *ssa, zend_bool reorder_dtor_effects) {
	int i;
	zend_ssa_phi *phi;
	int removed_ops = 0;

	/* DCE of CV operations that changes arguments may affect vararg functions. */
	zend_bool has_varargs = (ssa->cfg.flags & ZEND_FUNC_VARARG) != 0;

	context ctx;
	ctx.ssa = ssa;
	ctx.op_array = op_array;
	ctx.reorder_dtor_effects = reorder_dtor_effects;

	/* We have no dedicated phi vector, so we use the whole ssa var vector instead */
	ctx.instr_worklist_len = zend_bitset_len(op_array->last);
	ctx.instr_worklist = alloca(sizeof(zend_ulong) * ctx.instr_worklist_len);
	memset(ctx.instr_worklist, 0, sizeof(zend_ulong) * ctx.instr_worklist_len);
	ctx.phi_worklist_len = zend_bitset_len(ssa->vars_count);
	ctx.phi_worklist = alloca(sizeof(zend_ulong) * ctx.phi_worklist_len);
	memset(ctx.phi_worklist, 0, sizeof(zend_ulong) * ctx.phi_worklist_len);
	ctx.phi_worklist_no_val = alloca(sizeof(zend_ulong) * ctx.phi_worklist_len);
	memset(ctx.phi_worklist_no_val, 0, sizeof(zend_ulong) * ctx.phi_worklist_len);

	/* Optimistically assume all instructions and phis to be dead */
	ctx.instr_dead = alloca(sizeof(zend_ulong) * ctx.instr_worklist_len);
	memset(ctx.instr_dead, 0, sizeof(zend_ulong) * ctx.instr_worklist_len);
	ctx.phi_dead = alloca(sizeof(zend_ulong) * ctx.phi_worklist_len);
	memset(ctx.phi_dead, 0xff, sizeof(zend_ulong) * ctx.phi_worklist_len);

	/* Mark reacable instruction without side effects as dead */
	int b = ssa->cfg.blocks_count;
	while (b > 0) {
		int	op_data = -1;

		b--;
		zend_basic_block *block = &ssa->cfg.blocks[b];
		if (!(block->flags & ZEND_BB_REACHABLE)) {
			continue;
		}
		i = block->start + block->len;
		while (i > block->start) {
			i--;

			if (op_array->opcodes[i].opcode == ZEND_OP_DATA) {
				op_data = i;
				continue;
			}

			if (zend_bitset_in(ctx.instr_worklist, i)) {
				zend_bitset_excl(ctx.instr_worklist, i);
				add_operands_to_worklists(&ctx, &op_array->opcodes[i], &ssa->ops[i], ssa, 0);
				if (op_data >= 0) {
					add_operands_to_worklists(&ctx, &op_array->opcodes[op_data], &ssa->ops[op_data], ssa, 0);
				}
			} else if (may_have_side_effects(op_array, ssa, &op_array->opcodes[i], &ssa->ops[i], ctx.reorder_dtor_effects)
					|| zend_may_throw(&op_array->opcodes[i], op_array, ssa)
					|| (has_varargs && may_break_varargs(op_array, ssa, &ssa->ops[i]))) {
				if (op_array->opcodes[i].opcode == ZEND_NEW
						&& op_array->opcodes[i+1].opcode == ZEND_DO_FCALL
						&& ssa->ops[i].result_def >= 0
						&& ssa->vars[ssa->ops[i].result_def].escape_state == ESCAPE_STATE_NO_ESCAPE) {
					zend_bitset_incl(ctx.instr_dead, i);
					zend_bitset_incl(ctx.instr_dead, i+1);
				} else {
					add_operands_to_worklists(&ctx, &op_array->opcodes[i], &ssa->ops[i], ssa, 0);
					if (op_data >= 0) {
						add_operands_to_worklists(&ctx, &op_array->opcodes[op_data], &ssa->ops[op_data], ssa, 0);
					}
				}
			} else {
				zend_bitset_incl(ctx.instr_dead, i);
				if (op_data >= 0) {
					zend_bitset_incl(ctx.instr_dead, op_data);
				}
			}
			op_data = -1;
		}
	}

	/* Propagate liveness backwards to all definitions of used vars */
	while (!zend_bitset_empty(ctx.instr_worklist, ctx.instr_worklist_len)
			|| !zend_bitset_empty(ctx.phi_worklist, ctx.phi_worklist_len)) {
		while ((i = zend_bitset_pop_first(ctx.instr_worklist, ctx.instr_worklist_len)) >= 0) {
			zend_bitset_excl(ctx.instr_dead, i);
			add_operands_to_worklists(&ctx, &op_array->opcodes[i], &ssa->ops[i], ssa, 1);
			if (i < op_array->last && op_array->opcodes[i+1].opcode == ZEND_OP_DATA) {
				zend_bitset_excl(ctx.instr_dead, i+1);
				add_operands_to_worklists(&ctx, &op_array->opcodes[i+1], &ssa->ops[i+1], ssa, 1);
			}
		}
		while ((i = zend_bitset_pop_first(ctx.phi_worklist, ctx.phi_worklist_len)) >= 0) {
			zend_bitset_excl(ctx.phi_dead, i);
			zend_bitset_excl(ctx.phi_worklist_no_val, i);
			add_phi_sources_to_worklists(&ctx, ssa->vars[i].definition_phi, 1);
		}
	}

	/* Eliminate dead instructions */
	ZEND_BITSET_FOREACH(ctx.instr_dead, ctx.instr_worklist_len, i) {
		removed_ops += dce_instr(&ctx, &op_array->opcodes[i], &ssa->ops[i]);
	} ZEND_BITSET_FOREACH_END();

	/* Improper uses don't count as "uses" for the purpose of instruction elimination,
	 * but we have to retain phis defining them.
	 * Propagate this information backwards, marking any phi with an improperly used
	 * target as non-dead. */
	while ((i = zend_bitset_pop_first(ctx.phi_worklist_no_val, ctx.phi_worklist_len)) >= 0) {
		zend_ssa_phi *phi = ssa->vars[i].definition_phi;
		int source;
		zend_bitset_excl(ctx.phi_dead, i);
		FOREACH_PHI_SOURCE(phi, source) {
			add_to_phi_worklist_no_val(&ctx, source);
		} FOREACH_PHI_SOURCE_END();
	}

	/* Now collect the actually dead phis */
	FOREACH_PHI(phi) {
		if (zend_bitset_in(ctx.phi_dead, phi->ssa_var)) {
			zend_ssa_remove_uses_of_var(ssa, phi->ssa_var);
			zend_ssa_remove_phi(ssa, phi);
		} else {
			/* Remove trivial phis (phis with identical source operands) */
			try_remove_trivial_phi(&ctx, phi);
		}
	} FOREACH_PHI_END();

	return removed_ops;
}
