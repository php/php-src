#include "ZendAccelerator.h"
#include "zend_optimizer_internal.h"
#include "zend_inference.h"
#include "zend_func_info.h"
#include "zend_call_graph.h"

typedef struct _context {
	zend_script *script;
	zend_op_array *op_array;
	zend_ssa *ssa;
	int *block_shiftlist;
	int *shiftlist;
	uint32_t new_num_opcodes;
} context;

static inline uint32_t num_def_operands(zend_ssa_op *ssa_op) {
	return (ssa_op->result_def >= 0) + (ssa_op->op1_def >= 0) + (ssa_op->op2_def >= 0);
}

static void emit_type_check(
		zend_ssa *ssa, zend_op *opline, int var, int op_type, int ssa_var, uint32_t lineno) {
	zend_ssa_var_info *info = &ssa->var_info[ssa_var];
	uint32_t type = info->type;

	if (type & MAY_BE_CLASS) {
		/* No way to check this one at runtime */
		MAKE_NOP(opline);
		return;
	}

	opline->opcode = ZEND_CHECK_INFERENCE;
	opline->op1_type = op_type;
	opline->op1.var = var;
	opline->op2_type = IS_UNUSED;
	opline->op2.num = (uint32_t) INT32_MIN;
	opline->result_type = IS_UNUSED;
	opline->result.num = (uint32_t) INT32_MAX;
	opline->extended_value = type;
	opline->lineno = lineno;

	if (info->has_range) {
		if (!info->range.underflow && info->range.min > INT32_MIN && info->range.min < INT32_MAX) {
			opline->op2.num = (uint32_t) info->range.min;
		}
		if (!info->range.overflow && info->range.max > INT32_MIN && info->range.max < INT32_MAX) {
			opline->result.num = (uint32_t) info->range.max;
		}
	}
}

static zend_op *emit_type_checks(
		zend_ssa *ssa, zend_op *new_opline, zend_op *opline, zend_ssa_op *ssa_op) {
	if (ssa_op->result_def >= 0) {
		emit_type_check(ssa, new_opline,
			opline->result.var, opline->result_type, ssa_op->result_def, opline->lineno);
		new_opline++;
	}
	if (ssa_op->op1_def >= 0) {
		emit_type_check(ssa, new_opline,
			opline->op1.var, opline->op1_type, ssa_op->op1_def, opline->lineno);
		new_opline++;
	}
	if (ssa_op->op2_def >= 0) {
		emit_type_check(ssa, new_opline,
			opline->op2.var, opline->op2_type, ssa_op->op2_def, opline->lineno);
		new_opline++;
	}
	if (ssa_op->op1_def >= 0 && ssa_op->op2_def >= 0
			&& ssa->vars[ssa_op->op1_def].var == ssa->vars[ssa_op->op2_def].var) {
		/* Handle super special case where the same var is def'd twice */
		if (ssa_op->op1_def > ssa_op->op2_def) {
			MAKE_NOP(new_opline - 1);
		} else {
			MAKE_NOP(new_opline - 2);
		}
	}
	return new_opline;
}

static zend_bool should_skip(zend_op *opline, zend_op *end) {
	if (opline+1 != end && (opline+1)->opcode == ZEND_OP_DATA) {
		/* Can't insert instructions before OP_DATA */
		return 1;
	}
	switch (opline->opcode) {
		/* These don't return real strings */
		case ZEND_ROPE_INIT:
		case ZEND_ROPE_ADD:
		/* Branches with a result */
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_ASSERT_CHECK:
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
		/* The final result is only available after multiple instructions */
		case ZEND_FETCH_DIM_W:
		case ZEND_FETCH_DIM_RW:
		case ZEND_FETCH_DIM_FUNC_ARG:
		case ZEND_FETCH_DIM_UNSET:
		/* Can cause issues with delayed early binding.
		 * Also we don't check MAY_BE_CLASS anyway. */
		case ZEND_FETCH_CLASS:
		/* These may be skipped over */
		case ZEND_RECV:
		case ZEND_RECV_INIT:
			return 1;
	}
	return 0;
}

static void compute_shiftlist(context *ctx) {
	zend_ssa *ssa = ctx->ssa;
	zend_cfg *cfg = &ssa->cfg;
	int *block_shiftlist = ctx->block_shiftlist;
	int *shiftlist = ctx->shiftlist;
	zend_op *end = ctx->op_array->opcodes + ctx->op_array->last;
	int i, j, shift = 0;

	ctx->new_num_opcodes = 0;
	for (i = 0; i < cfg->blocks_count; i++) {
		const zend_basic_block *block = &cfg->blocks[i];
		block_shiftlist[i] = shift;

		if (!(block->flags & ZEND_BB_REACHABLE) || block->len == 0) {
			shift -= block->len;
			continue;
		}

		ctx->new_num_opcodes += block->len;
		for (j = block->start; j < block->start + block->len; j++) {
			zend_ssa_op *ssa_op = &ssa->ops[j];
			uint32_t num = num_def_operands(ssa_op);
			shiftlist[j] = shift;
			if (!should_skip(&ctx->op_array->opcodes[j], end)) {
				shift += num;
				ctx->new_num_opcodes += num;
			}
		}
	}
}

static void copy_instr(
		context *ctx, zend_op *new_opline, zend_op *old_opline, zend_op *new_opcodes) {
#define TO_NEW(block_num) \
		(new_opcodes + cfg->blocks[block_num].start + ctx->block_shiftlist[block_num])
	zend_op_array *op_array = ctx->op_array;
	const zend_cfg *cfg = &ctx->ssa->cfg;
	zend_basic_block *block = &cfg->blocks[cfg->map[old_opline - op_array->opcodes]];
	*new_opline = *old_opline;

	/* Adjust JMP offsets */
	switch (new_opline->opcode) {
		case ZEND_JMP:
		case ZEND_FAST_CALL:
			ZEND_SET_OP_JMP_ADDR(new_opline, new_opline->op1, TO_NEW(block->successors[0]));
			break;
		case ZEND_JMPZNZ:
			new_opline->extended_value = ZEND_OPLINE_TO_OFFSET(
				new_opline, TO_NEW(block->successors[1]));
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
			ZEND_SET_OP_JMP_ADDR(new_opline, new_opline->op2, TO_NEW(block->successors[0]));
			break;
		case ZEND_CATCH:
			if (!(new_opline->extended_value & ZEND_LAST_CATCH)) {
				ZEND_SET_OP_JMP_ADDR(new_opline, new_opline->op2, TO_NEW(block->successors[0]));
			}
			break;
		case ZEND_DECLARE_ANON_CLASS:
		case ZEND_DECLARE_ANON_INHERITED_CLASS:
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
			new_opline->extended_value = ZEND_OPLINE_TO_OFFSET(
				new_opline, TO_NEW(block->successors[0]));
			break;
		case ZEND_SWITCH_LONG:
		case ZEND_SWITCH_STRING:
		{
			HashTable *jumptable = Z_ARRVAL(ZEND_OP2_LITERAL(new_opline));
			zval *zv;
			int s = 0;
			ZEND_HASH_FOREACH_VAL(jumptable, zv) {
				Z_LVAL_P(zv) = ZEND_OPLINE_TO_OFFSET(new_opline, TO_NEW(block->successors[s]));
				s++;
			} ZEND_HASH_FOREACH_END();
			new_opline->extended_value = ZEND_OPLINE_TO_OFFSET(
				new_opline, TO_NEW(block->successors[s]));
			break;
		}
	} 
#undef TO_NEW
}

static void insert_type_checks(context *ctx) {
	zend_op_array *op_array = ctx->op_array;
	zend_func_info *func_info = ZEND_FUNC_INFO(op_array);
	zend_ssa *ssa = ctx->ssa;
	zend_cfg *cfg = &ssa->cfg;
	const int *shiftlist = ctx->shiftlist;
	zend_op *end = op_array->opcodes + op_array->last;
	zend_op *old_opcodes = op_array->opcodes;
	zend_op *new_opcodes = ecalloc(sizeof(zend_op), ctx->new_num_opcodes);
	zend_op *new_opline = new_opcodes;
	zend_ssa_op *new_ssa_ops = calloc(sizeof(zend_ssa_op), ctx->new_num_opcodes);
	zend_ssa_op *new_ssa_op = new_ssa_ops;
	int i, j;

	// TODO phis
	for (i = 0; i < cfg->blocks_count; i++) {
		const zend_basic_block *block = &cfg->blocks[i];

		if (!(block->flags & ZEND_BB_REACHABLE) || block->len == 0) {
			continue;
		}

		for (j = block->start; j < block->start + block->len; j++) {
			zend_op *opline = &op_array->opcodes[j];
			zend_ssa_op *ssa_op = &ssa->ops[j];
			copy_instr(ctx, new_opline++, opline, new_opcodes);
			*new_ssa_op++ = *ssa_op;

			/* Need to honor use_as_double if we're adding type checks */
			if (opline->opcode == ZEND_ASSIGN && opline->op2_type == IS_CONST
					&& ssa_op->op1_def >= 0 && ssa->var_info[ssa_op->op1_def].use_as_double) {
				zval *op2 = CT_CONSTANT_EX(op_array, opline->op2.constant);
				convert_to_double(op2);
			}
			if (!should_skip(&ctx->op_array->opcodes[j], end)) {
				zend_op *orig_new_opline = new_opline;
				new_opline = emit_type_checks(ssa, new_opline, opline, ssa_op);
				new_ssa_op += new_opline - orig_new_opline;
			}
		}
	}

	ssa->ops = new_ssa_ops;

	efree(op_array->opcodes);
	op_array->opcodes = new_opcodes;
	op_array->last = ctx->new_num_opcodes;

	/* update SSA variables */
	for (j = 0; j < ssa->vars_count; j++) {
		if (ssa->vars[j].definition >= 0) {
			ssa->vars[j].definition += shiftlist[ssa->vars[j].definition];
		}
		if (ssa->vars[j].use_chain >= 0) {
			ssa->vars[j].use_chain += shiftlist[ssa->vars[j].use_chain];
		}
	}
	for (i = 0; i < op_array->last; i++) {
		if (ssa->ops[i].op1_use_chain >= 0) {
			ssa->ops[i].op1_use_chain += shiftlist[ssa->ops[i].op1_use_chain];
		}
		if (ssa->ops[i].op2_use_chain >= 0) {
			ssa->ops[i].op2_use_chain += shiftlist[ssa->ops[i].op2_use_chain];
		}
		if (ssa->ops[i].res_use_chain >= 0) {
			ssa->ops[i].res_use_chain += shiftlist[ssa->ops[i].res_use_chain];
		}
	}

	/* Update live ranges */
	if (op_array->last_live_range) {
		for (i = 0; i < op_array->last_live_range; i++) {
			zend_live_range *range = &op_array->live_range[i];
			range->start += shiftlist[range->start];
			range->end += shiftlist[range->end];
		}
	}

	/* Update early binding chain */
	if (op_array->fn_flags & ZEND_ACC_EARLY_BINDING) {
		uint32_t *opline_num = &ctx->script->first_early_binding_opline;

		ZEND_ASSERT(op_array == &ctx->script->main_op_array);
		do {
			*opline_num += shiftlist[*opline_num];
			opline_num = &op_array->opcodes[*opline_num].result.opline_num;
		} while (*opline_num != (uint32_t)-1);
	}

	/* update call graph */
	if (func_info) {
		zend_call_info *call_info = func_info->callee_info;
		while (call_info) {
			call_info->caller_init_opline += new_opcodes - old_opcodes
				+ shiftlist[call_info->caller_init_opline - old_opcodes];
			call_info->caller_call_opline += new_opcodes - old_opcodes
				+ shiftlist[call_info->caller_call_opline - old_opcodes];
			call_info = call_info->next_callee;
		}
	}
}

void zend_ssa_verify_inference(
		zend_optimizer_ctx *opt_ctx, zend_op_array *op_array, zend_ssa *ssa) {
	context ctx;
	ctx.script = opt_ctx->script;
	ctx.op_array = op_array;
	ctx.ssa = ssa;
	ctx.block_shiftlist = zend_arena_calloc(&opt_ctx->arena, ssa->cfg.blocks_count, sizeof(int));
	ctx.shiftlist = zend_arena_calloc(&opt_ctx->arena, op_array->last, sizeof(int));
	compute_shiftlist(&ctx);
	insert_type_checks(&ctx);
}
