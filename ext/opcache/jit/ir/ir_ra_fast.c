/*
 * IR - Lightweight JIT Compilation Framework
 * Fast Local Register Allocation (used for -O0)
 * This file is part of the IR Project distributed under the MIT-style LICENSE.
 * Authors: Dmitry Stogov <dmitry@php.net>
 */

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include "ir.h"

#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
# include "ir_x86.h"
#elif defined(IR_TARGET_AARCH64)
# include "ir_aarch64.h"
#else
# error "Unknown IR target"
#endif

#include "ir_private.h"

typedef struct {
	ir_reg_alloc_data ra_data;
	ir_block *bb;
	int32_t  num;
	int32_t  tmp_num;
	ir_regset scratch;
	ir_regset preserved;
	ir_regset fixed;
	ir_regset allocated;
	ir_regset clobbered_args;
	ir_regset clobbered[IR_SUB_REFS_COUNT];
	struct {
		ir_ref def;
		ir_ref root;
		ir_ref use;
		ir_ref op;
	} state[IR_REG_NUM];
	struct {
		uint8_t type;
		int8_t  start;
		int8_t  end;
		int8_t  hint;
		int8_t  flags;
		ir_ref  root;
		ir_ref  ref;
		ir_ref  op;
		ir_ref  def;
	} regs[32], tmp_regs[32];
} ir_reg_alloc_fast_data;

#ifdef IR_DEBUG
# define IR_DEBUG_SPILLS 1
# if IR_X86_I64
#  define IR_REG_NAME_FMT              "%s%s%s"
#  define IR_REG_NAME_VAL(_reg, _type) (((_type) == IR_I64 || (_type) == IR_U64) ? \
                                        ir_reg_name(IR_REG_I64_LO(IR_REG_NUM(_reg)), IR_U32) : \
										ir_reg_name(IR_REG_NUM(_reg), _type)), \
                                       (((_type) == IR_I64 || (_type) == IR_U64) ? " and " : ""), \
                                       (((_type) == IR_I64 || (_type) == IR_U64) ? \
                                        ir_reg_name(IR_REG_I64_HI(IR_REG_NUM(_reg)), IR_U32) : "")
# else
#  define IR_REG_NAME_FMT              "%s"
#  define IR_REG_NAME_VAL(_reg, _type) ir_reg_name(IR_REG_NUM(_reg), _type)
# endif
# define IR_LOG(comment) do { \
		if (ctx->flags & IR_DEBUG_RA) { \
			fprintf(stderr, "---- %s\n", comment); \
		} \
	} while (0)
# define IR_LOG_ASSIGN(_ref, _pos, _x, _y, _reg) do { \
		if (ctx->flags & IR_DEBUG_RA) { \
			fprintf(stderr, "  ---- Assign R%d (d_%d) to " IR_REG_NAME_FMT "%s at %d (%d.%d)\n", \
				ctx->vregs[_ref], _ref, \
				IR_REG_NAME_VAL(_reg, ctx->ir_base[_ref].type), \
				(((_reg) & IR_REG_SPILL_LOAD) ? " (spill)" : ""), \
				_pos, _x, _y); \
		} \
	} while (0)
# define IR_LOG_ASSIGN_TMP(_type, _pos, _x, _y, _reg) do { \
		if (ctx->flags & IR_DEBUG_RA) { \
			fprintf(stderr, "  ---- Assign TMP to " IR_REG_NAME_FMT "%s at %d (%d.%d)\n", \
				IR_REG_NAME_VAL(_reg, _type), \
				(((_reg) & IR_REG_SPILL_LOAD) ? " (spill)" : ""), \
				_pos, _x, _y); \
		} \
	} while (0)
# define IR_LOG_ASSIGN_VAR(_var, _spill) do { \
		if (IR_DEBUG_SPILLS && (ctx->flags & IR_DEBUG_RA)) { \
			fprintf(stderr, "  ---- Spill VAR d_%d to 0x%x\n", \
				_var, _spill); \
		} \
	} while (0)
# define IR_LOG_ASSIGN_SPILL(_vreg, _spill) do { \
		if (IR_DEBUG_SPILLS && (ctx->flags & IR_DEBUG_RA)) { \
			fprintf(stderr, "  ---- Spill R%d to 0x%x\n", \
				_vreg, _spill); \
		} \
	} while (0)
# define IR_LOG_FREE(_ref, _reg) do { \
		if (ctx->flags & IR_DEBUG_RA) { \
			fprintf(stderr, "  ---- Free R%d(d_%d) assigned to " IR_REG_NAME_FMT "\n", \
				ctx->vregs[_ref], _ref, \
				IR_REG_NAME_VAL(_reg, ctx->ir_base[_ref].type)); \
		} \
	} while (0)
# define IR_LOG_EVICT(_ref, _reg) do { \
		if (ctx->flags & IR_DEBUG_RA) { \
			fprintf(stderr, "  ---- Evict R%d(d_%d) assigned to " IR_REG_NAME_FMT "\n", \
				ctx->vregs[_ref], _ref, \
				IR_REG_NAME_VAL(_reg, ctx->ir_base[_ref].type)); \
		} \
	} while (0)
# define IR_LOG_SPILL_LOAD(_ref, _pos, _x, _y, _reg) do { \
		if (ctx->flags & IR_DEBUG_RA) { \
			fprintf(stderr, "  ---- Add Spill Load R%d (d_%d) to " IR_REG_NAME_FMT " at %d (%d.%d)\n", \
				ctx->vregs[_ref], _ref, \
				IR_REG_NAME_VAL(_reg, ctx->ir_base[_ref].type), \
				_pos, _x, _y); \
		} \
	} while (0)
#else
# define IR_LOG(comment)
# define IR_LOG_ASSIGN(_ref, _pos, _x, _y, _reg)
# define IR_LOG_ASSIGN_TMP(_type, _pos, _x, _y, _reg)
# define IR_LOG_ASSIGN_VAR(_vreg, _spill)
# define IR_LOG_ASSIGN_SPILL(_vreg, _spill)
# define IR_LOG_FREE(_ref, _reg)
# define IR_LOG_EVICT(_ref, _reg)
# define IR_LOG_SPILL_LOAD(_ref, _pos, _x, _y, _reg)
#endif

static void _add_scratch(ir_reg_alloc_fast_data *data, ir_reg reg, int8_t start, int8_t end)
{
	int8_t j;

	if (start < 0) {
		/* ARGVAL support */
		if (reg >= IR_REG_NUM) {
			data->clobbered_args = IR_REGSET_UNION(data->clobbered_args, ir_scratch_regset[reg - IR_REG_NUM]);
		} else {
			IR_REGSET_INCL(data->clobbered_args, reg);
		}
		start = 0;
	}
	IR_ASSERT(start >= 0 && end <= IR_SUB_REFS_COUNT);
	if (reg >= IR_REG_NUM) {
		for (j = start; j < end; j++) {
			data->clobbered[j] = IR_REGSET_UNION(data->clobbered[j], ir_scratch_regset[reg - IR_REG_NUM]);
		}
	} else {
		for (j = start; j < end; j++) {
			IR_REGSET_INCL(data->clobbered[j], reg);
		}
	}
}

static void _add_tmp_reg(ir_reg_alloc_fast_data *data, ir_type type,
                     int8_t start, int8_t end, uint8_t flags,
                     ir_ref root, ir_ref ref, ir_ref op)
{
	uintptr_t n;

	IR_ASSERT(start >= 0 && end <= IR_SUB_REFS_COUNT && data->num < 32);
	n = data->tmp_num++;
	data->tmp_regs[n].type = type;
	data->tmp_regs[n].start = start;
	data->tmp_regs[n].end = end;
	data->tmp_regs[n].flags = flags;
	data->tmp_regs[n].root = root;
	data->tmp_regs[n].ref = ref;
	data->tmp_regs[n].op = op;
}

static void _add_reg(ir_reg_alloc_fast_data *data, ir_type type,
                     int8_t start, int8_t end, ir_reg hint, int8_t flags,
                     ir_ref root, ir_ref ref, ir_ref op, ir_ref def)
{
	uintptr_t n;

	IR_ASSERT(start >= 0 && end <= IR_SUB_REFS_COUNT && data->num < 32);
	n = data->num++;
	data->regs[n].type = type;
	data->regs[n].start = start;
	data->regs[n].end = end;
	data->regs[n].hint = hint;
	data->regs[n].flags = flags;
	data->regs[n].root = root;
	data->regs[n].ref = ref;
	data->regs[n].op = op;
	data->regs[n].def = def;
}

static void _pick_spill(ir_ctx *ctx, ir_live_interval *ival)
{
	size_t size = ir_get_type_size(ival->type);

	IR_ASSERT(ival->stack_spill_pos == -1);
#if IR_SIMD
	if (IR_IS_TYPE_VECTOR(ival->type) && size < 4) {
		size = 4;
	}
#endif
	ival->stack_spill_pos = ir_allocate_spill_slot(ctx, size, 0);
	IR_LOG_ASSIGN_SPILL(ival->vreg, ival->stack_spill_pos);
}

static void _evict_reg(ir_ctx *ctx, ir_reg_alloc_fast_data *data, ir_reg reg)
{
	ir_live_interval *ival;
	ir_ref ref = data->state[reg].def;

	IR_ASSERT(ctx->vregs[ref] > 0);
	ival = ctx->live_intervals[ctx->vregs[ref]];

	/* allocate spill slot */
	if (ival->stack_spill_pos == -1) {
		_pick_spill(ctx, ival);
	}

	/* free reg */
#if IR_X86_I64
	if (ival->type == IR_I64 || ival->type == IR_U64) {
		if (ival->reg == IR_REG_NONE && ival->reg_hi == IR_REG_NONE) {
			/* already evicted */
			return;
		}

		IR_ASSERT(ival->reg != IR_REG_NONE && ival->reg_hi != IR_REG_NONE && (ival->reg == reg || ival->reg_hi == reg));

		ir_reg pair = IR_REG_I64_PAIR(ival->reg, ival->reg_hi);
		IR_LOG_EVICT(ref, pair);
		IR_REGSET_EXCL(data->allocated, ival->reg);
		IR_REGSET_EXCL(data->allocated, ival->reg_hi);
		ival->reg = ival->reg_hi = IR_REG_NONE;

		if (data->state[reg].use) {
			/* add spill load for the previous use */
			if (!data->state[reg].root
			 || ctx->ir_base[data->state[reg].use].op == IR_ARGVAL) {
				ir_set_alocated_reg(ctx, data->state[reg].use, data->state[reg].op, pair | IR_REG_SPILL_LOAD);
			} else {
				ctx->rules[data->state[reg].use] |= IR_FUSED_REG;
				ir_set_fused_reg(ctx, data->state[reg].root, data->state[reg].use, data->state[reg].op,
					pair | IR_REG_SPILL_LOAD);
			}
			IR_LOG_SPILL_LOAD(ref, data->state[reg].root, data->state[reg].use, data->state[reg].op, pair);
		}
		return;
	}
#endif
	IR_ASSERT(ival->reg == reg);
	IR_LOG_EVICT(ref, reg);
	IR_REGSET_EXCL(data->allocated, reg);
	ival->reg = IR_REG_NONE;

	if (data->state[reg].use) {
		/* add spill load for the previous use */
		if (!data->state[reg].root
		 || ctx->ir_base[data->state[reg].use].op == IR_ARGVAL) {
			ir_set_alocated_reg(ctx, data->state[reg].use, data->state[reg].op, reg | IR_REG_SPILL_LOAD);
		} else {
			ctx->rules[data->state[reg].use] |= IR_FUSED_REG;
			ir_set_fused_reg(ctx, data->state[reg].root, data->state[reg].use, data->state[reg].op,
				reg | IR_REG_SPILL_LOAD);
		}
		IR_LOG_SPILL_LOAD(ref, data->state[reg].root, data->state[reg].use, data->state[reg].op, reg);
	}
}

static ir_regset _preferred_regset(ir_ctx *ctx, ir_reg_alloc_fast_data *data, ir_ref use, ir_ref def)
{
	ir_ref ref;

	if (!use || def < data->bb->start || def >= use) {
		return data->scratch;
	}
	for (ref = ctx->prev_ref[use]; ref != def; ref = ctx->prev_ref[ref]) {
		if (ctx->ir_base[ref].op == IR_CALL) {
			return data->preserved;
		}
	}
	return data->scratch;
}

static ir_reg _pick_free_reg(ir_ctx *ctx, ir_reg_alloc_fast_data *data, ir_type type, ir_ref use, ir_ref def)
{
	ir_regset available;
	ir_ref reg;

	if (IR_IS_TYPE_INT(type)) {
		available = IR_REGSET_GP;
#if defined(IR_TARGET_X86)
		if (ir_type_size[type] == 1) {
			IR_REGSET_EXCL(available, IR_REG_RBP);
			IR_REGSET_EXCL(available, IR_REG_RSI);
			IR_REGSET_EXCL(available, IR_REG_RDI);
		}
#endif
	} else {
		IR_ASSERT(IR_IS_TYPE_FP(type) || IR_IS_TYPE_VECTOR(type));
		available = IR_REGSET_FP;
	}
	available = IR_REGSET_DIFFERENCE(available, data->allocated);
	if (!IR_REGSET_IS_EMPTY(available)) {
		ir_regset preferred = IR_REGSET_INTERSECTION(_preferred_regset(ctx, data, use, def), available);

		if (!IR_REGSET_IS_EMPTY(preferred)) {
			reg = IR_REGSET_FIRST(preferred);
			if (IR_REGSET_IN(data->preserved, reg)) {
				ir_regset tmp = (ir_regset)ctx->used_preserved_regs;
				IR_REGSET_INCL(tmp, reg);
				ctx->used_preserved_regs = tmp;
			}
			return reg;
		}
		reg = IR_REGSET_FIRST(available);
		if (IR_REGSET_IN(data->preserved, reg)) {
			ir_regset tmp = (ir_regset)ctx->used_preserved_regs;
			IR_REGSET_INCL(tmp, reg);
			ctx->used_preserved_regs = tmp;
		}
		return reg;
	}
	return IR_REG_NONE;
}

static ir_reg _pick_reg(ir_ctx *ctx, ir_reg_alloc_fast_data *data, ir_type type, ir_regset used, uint8_t flags, ir_ref use, ir_ref def)
{
	ir_regset available;
	ir_reg reg, best_reg;
	ir_ref best_cost;

	if (IR_IS_TYPE_INT(type)) {
		available = IR_REGSET_GP;
#if defined(IR_TARGET_X86)
		if (ir_type_size[type] == 1) {
			IR_REGSET_EXCL(available, IR_REG_RBP);
			IR_REGSET_EXCL(available, IR_REG_RSI);
			IR_REGSET_EXCL(available, IR_REG_RDI);
		}
#endif
	} else {
		IR_ASSERT(IR_IS_TYPE_FP(type) || IR_IS_TYPE_VECTOR(type));
		available = IR_REGSET_FP;
	}
	available = IR_REGSET_DIFFERENCE(available, used);
	if (!IR_REGSET_IS_EMPTY(available)) {
		ir_regset available_unused = IR_REGSET_DIFFERENCE(available, data->allocated);
		ir_regset preferred = IR_REGSET_INTERSECTION(_preferred_regset(ctx, data, use, def), available);

		if (!IR_REGSET_IS_EMPTY(available_unused)) {
			ir_regset available_preferred = IR_REGSET_INTERSECTION(available_unused, preferred);

			if (!IR_REGSET_IS_EMPTY(available_preferred)) {
				reg = IR_REGSET_FIRST(available_preferred);
				if (IR_REGSET_IN(data->preserved, reg)) {
					ir_regset tmp = (ir_regset)ctx->used_preserved_regs;
					IR_REGSET_INCL(tmp, reg);
					ctx->used_preserved_regs = tmp;
				}
				return reg;
			}
			reg = IR_REGSET_FIRST(available_unused);
			if (IR_REGSET_IN(data->preserved, reg)) {
				ir_regset tmp = (ir_regset)ctx->used_preserved_regs;
				IR_REGSET_INCL(tmp, reg);
				ctx->used_preserved_regs = tmp;
			}
			return reg;
		}

		if (!(flags & IR_USE_MUST_BE_IN_REG)) {
			return IR_REG_NONE;
		}

		/* select best reg to evict */
		available = IR_REGSET_INTERSECTION(available, data->allocated);
		best_cost = 0x7fffffff;
		best_reg = IR_REG_NONE;
		IR_REGSET_FOREACH(available, reg) {
			if (use) {
				/* skip registers allocated for the same instruction */
				if (data->state[reg].root) {
					if (data->state[reg].root == use) continue;
				} else {
					if (data->state[reg].use == use) continue;
				}
			}
			if (data->state[reg].def < best_cost) {
				best_cost = data->state[reg].def;
				best_reg = reg;
			}
		} IR_REGSET_FOREACH_END();

		_evict_reg(ctx, data, best_reg);
		return best_reg;
	}

	if (!(flags & IR_USE_MUST_BE_IN_REG)) {
		return IR_REG_NONE;
	}

	fprintf(stderr, "Internal Error: No registers available. Allocation is not possible\n");
	IR_ASSERT(0);
	exit(-1);
}

static ir_live_interval *_create_ival(ir_ctx *ctx, uint32_t vreg, ir_type type)
{
	ir_live_interval *ival = ir_arena_alloc(&ctx->arena, sizeof(ir_live_interval));

	memset(ival, 0, sizeof(ir_live_interval));
	ctx->live_intervals[vreg] = ival;
	ival->type = type;
	ival->reg = IR_REG_NONE;
	ival->vreg = vreg;
	ival->stack_spill_pos = -1;
	return ival;
}

IR_ALWAYS_INLINE ir_live_interval *_get_ival(ir_ctx *ctx, ir_ref ref)
{
	int32_t vreg = ctx->vregs[ref];
	ir_live_interval *ival;

	IR_ASSERT(vreg > 0);
	ival = ctx->live_intervals[vreg];
	if (!ival) {
		return _create_ival(ctx, vreg, ctx->ir_base[ref].type);
	}
	return ival;
}

static void ir_allocate_var_spill_slot(ir_ctx *ctx, ir_ref var, ir_insn *var_insn)
{
	size_t size = ir_get_type_size(var_insn->type);
	size_t align = var_insn->op3;
	int32_t offset;

#if IR_SIMD
	if (IR_IS_TYPE_VECTOR(var_insn->type) && size < 4) {
		size = 4;
		align = IR_MAX(align, 4);
	}
#endif

	offset = ir_allocate_spill_slot(ctx, size, align);
	ctx->vregs[var] = IR_STACK_SLOT_TO_VREG(offset);
	IR_LOG_ASSIGN_VAR(var, offset);
}

static bool _may_coalesce(ir_ctx *ctx, ir_ref input, ir_ref def)
{
	/* Coalescing of values connected trough PHI requires global live range analyzes */
	return 0;
}

static void ir_add_fusion_data(ir_ctx *ctx, ir_ref ref, ir_ref input, ir_reg_alloc_fast_data *data, ir_ref start)
{
	ir_ref stack[4];
	int stack_pos = 0;
	ir_target_constraints constraints;
	ir_insn *insn;
	uint32_t j, n, def_flags;
	ir_ref *p, child;

	while (1) {
		IR_ASSERT(input > 0 && ctx->rules[input] & IR_FUSED);

		if (!(ctx->rules[input] & IR_SIMPLE)) {
			def_flags = ir_get_target_constraints(ctx, input, &constraints);
			n = constraints.tmps_count;
			while (n > 0) {
				n--;
				if (constraints.tmp_regs[n].type) {
					uint8_t flags = 0;
					ir_ref op = constraints.tmp_regs[n].num;

					if (op > 0 && op <= ctx->ir_base[input].inputs_count) {
						ir_ref *ops = ctx->ir_base[input].ops;

						if (IR_IS_CONST_REF(ops[op])) {
							/* rematerialization */
							flags = IR_REG_SPILL_LOAD;
						} else if (ctx->rules[ops[op]] == IR_STATIC_ALLOCA) {
							/* local address rematerialization */
							flags = IR_REG_SPILL_LOAD;
						}
					}
					_add_tmp_reg(data, constraints.tmp_regs[n].type,
						constraints.tmp_regs[n].start, constraints.tmp_regs[n].end, flags,
						ref, input, op);
				} else {
					_add_scratch(data, constraints.tmp_regs[n].reg,
						constraints.tmp_regs[n].start, constraints.tmp_regs[n].end);
				}
			}
		} else {
			def_flags = IR_OP1_MUST_BE_IN_REG | IR_OP2_MUST_BE_IN_REG | IR_OP3_MUST_BE_IN_REG;
			constraints.hints_count = 0;
		}

		insn = &ctx->ir_base[input];
		n = insn->inputs_count;
		j = 1;
		p = insn->ops + j;
		if (ir_op_flags[insn->op] & (IR_OP_FLAG_CONTROL|IR_OP_FLAG_PINNED)) {
			j++;
			p++;
		}
		for (; j <= n; j++, p++) {
			IR_ASSERT(IR_OPND_KIND(ir_op_flags[insn->op], j) == IR_OPND_DATA);
			child = *p;
			if (child > 0) {
				if (ctx->vregs[child] > 0) {
					uint32_t use_flags = IR_USE_FLAGS(def_flags, j);
					ir_live_interval *ival = ctx->live_intervals[ctx->vregs[child]];

					if (!ival) {
						ival = _create_ival(ctx, ctx->vregs[child], ctx->ir_base[child].type);
					}
					if (child < start || child >= ref) {
						/* live before BB start - spill it */
						if (ival->stack_spill_pos == -1) {
							_pick_spill(ctx, ival);
						}
						if (!(use_flags & IR_USE_MUST_BE_IN_REG)) {
							ctx->rules[input] |= IR_FUSED_REG;
							continue;
						}
					}

					ir_reg reg = (j < constraints.hints_count) ? constraints.hints[j] : IR_REG_NONE;
					int8_t use_pos = EXPECTED(reg == IR_REG_NONE) ? IR_USE_SUB_REF : IR_LOAD_SUB_REF;

					_add_reg(data, ctx->ir_base[child].type, IR_LOAD_SUB_REF, use_pos, reg, use_flags,
						ref, input, j, child);
				} else if (ctx->rules[child] & IR_FUSED) {
					IR_ASSERT(stack_pos < (int)(sizeof(stack)/sizeof(stack_pos)));
					stack[stack_pos++] = child;
				} else if (ctx->rules[child] == (IR_SKIPPED|IR_RLOAD)) {
					ir_set_alocated_reg(ctx, input, j, ctx->ir_base[child].op2);
				}
			}
		}
		if (!stack_pos) {
			break;
		}
		input = stack[--stack_pos];
	}
}

int ir_reg_alloc_fast(ir_ctx *ctx)
{
	uint32_t b;
	ir_block *bb;
	ir_insn *insn;
	ir_ref i, n, j, *p;
	ir_target_constraints constraints;
	uint32_t def_flags;
	ir_reg reg;
	ir_regset param_regs;
	ir_reg_alloc_fast_data data;
	int32_t offset;

	ir_compute_dessa_moves(ctx);

	IR_LOG("Start Fast Local RA");

	memset(&data.ra_data, 0, sizeof(data.ra_data));
	data.ra_data.cc = ir_get_call_conv_dsc(ctx->flags);
	ctx->data = &data.ra_data;

	ctx->stack_frame_size = 0;
	ctx->call_stack_size = 0;
	ctx->used_preserved_regs = ctx->fixed_save_regset;

	data.fixed = (ir_regset)ctx->fixed_regset;
	if (ctx->flags & IR_USE_FRAME_POINTER) {
		IR_REGSET_INCL(data.fixed, IR_REG_FRAME_POINTER);
	}

	data.scratch = IR_REGSET_DIFFERENCE(ir_scratch_regset[data.ra_data.cc->scratch_reg - IR_REG_NUM], data.fixed);
	data.preserved = IR_REGSET_DIFFERENCE(data.ra_data.cc->preserved_regs, data.fixed);
	param_regs = IR_REGSET_EMPTY;

	ctx->regs = ir_mem_malloc(sizeof(ir_regs) * ctx->insns_count);
	memset(ctx->regs, IR_REG_NONE, sizeof(ir_regs) * ctx->insns_count);

	/* vregs + tmp + fixed + SRATCH + ALL */
	ctx->live_intervals = ir_mem_calloc(ctx->vregs_count + 1 + IR_REG_SET_NUM, sizeof(ir_live_interval*));

	if (!ctx->arena) {
		ctx->arena = ir_arena_create(16 * 1024);
	}

	/* For all basic blocks in reverse order */
	for (b = ctx->cfg_blocks_count, bb = ctx->cfg_blocks + b; b > 0; bb--, b--) {
		IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));

		if (bb->flags & IR_BB_EMPTY) {
			continue;
		}

		data.allocated = IR_REGSET_EMPTY;
		data.bb = bb;

		if (bb->flags & IR_BB_DESSA_MOVES) {
			ir_block *succ_bb;
			ir_use_list *use_list;
			ir_ref k, *p, n;
			bool needs_tmp_gp = 0;
			bool needs_tmp_fp = 0;

			/* For all PHI functions in the successor of this block */
			IR_ASSERT(bb->successors_count == 1);
			succ_bb = &ctx->cfg_blocks[ctx->cfg_edges[bb->successors]];
			k = ir_phi_input_number(ctx, succ_bb, b);
			use_list = &ctx->use_lists[succ_bb->start];
			n = use_list->count;
			for (p = ctx->use_edges + use_list->refs; n > 0; p++, n--) {
				ir_ref use = *p;
				ir_insn *use_insn = &ctx->ir_base[use];

				if (use_insn->op == IR_PHI && ctx->vregs[use] > 0) {
					ir_ref input = ir_insn_op(use_insn, k);

					/* Require tmp register for potential MEM->MEM DESSA move */
					if (use < bb->start || ctx->regs[use][0] == IR_REG_NONE) {
						if (IR_IS_TYPE_INT(use_insn->type)) {
							needs_tmp_gp = 1;
						} else {
							needs_tmp_fp = 1;
						}
					}

					if (input > 0 && ctx->vregs[input] > 0) {
						ir_live_interval *ival = ctx->live_intervals[ctx->vregs[input]];

						if (!ival) {
							ival = _create_ival(ctx, ctx->vregs[input], ctx->ir_base[input].type);
						}

						/* If input of PHI, coming from this block, is defined in this block
						 * (otherwise it's treated as "global" and spilled).
						 */
						if (input > bb->start && input < bb->end) {
#if IR_X86_I64
							if (use_insn->type == IR_I64 || use_insn->type == IR_U64) {
								ir_reg reg_hi, reg_lo;

								if (ival->reg != IR_REG_NONE) {
									/* reuse previously allocated registers */
									reg = IR_REG_I64_PAIR(ival->reg, ival->reg_hi);
									ir_set_alocated_reg(ctx, use, k, reg);
									IR_LOG_ASSIGN(input, bb->end, use, k, reg);
									continue;
								} else if (ctx->regs[use][0] != IR_REG_NONE
								 && !IR_REG_SPILLED(ctx->regs[use][0])
								 && !IR_REGSET_IN(data.allocated, IR_REG_I64_LO(ctx->regs[use][0]))
								 && !IR_REGSET_IN(data.allocated, IR_REG_I64_HI(ctx->regs[use][0]))) {
									/* lightweight register coalescing */
									reg = ctx->regs[use][0];
									reg_lo = IR_REG_I64_LO(reg);
									reg_hi = IR_REG_I64_HI(reg);
								} else {
									reg_lo = _pick_free_reg(ctx, &data, ival->type, bb->end, input);
									if (reg_lo == IR_REG_NONE) {
										reg = IR_REG_NONE;
									} else {
										IR_REGSET_INCL(data.allocated, reg_lo);
										reg_hi = _pick_free_reg(ctx, &data, ival->type, bb->end, input);
										if (reg_hi == IR_REG_NONE) {
											IR_REGSET_EXCL(data.allocated, reg_lo);
											reg = IR_REG_NONE;
										} else {
											IR_ASSERT(reg_lo != reg_hi);
											if (reg_lo > reg_hi) {
												SWAP_REGS(reg_lo, reg_hi);
											}
											reg = IR_REG_I64_PAIR(reg_lo, reg_hi);
										}
									}
								}
								if (reg != IR_REG_NONE) {
									data.state[reg_hi].def = input;
									data.state[reg_hi].root = IR_UNUSED;
									data.state[reg_hi].use = use;
									data.state[reg_hi].op = k;
									IR_REGSET_INCL(data.allocated, reg_hi);
									ival->reg_hi = reg_hi;
									data.state[reg_lo].def = input;
									data.state[reg_lo].root = IR_UNUSED;
									data.state[reg_lo].use = use;
									data.state[reg_lo].op = k;
									IR_REGSET_INCL(data.allocated, reg_lo);
									ival->reg = reg_lo;
									ir_set_alocated_reg(ctx, use, k, reg);
									IR_LOG_ASSIGN(input, bb->end, use, k, reg);
									continue;
								}
							} else
#endif
							{
								if (ival->reg != IR_REG_NONE) {
									/* reuse previously allocated register */
									reg = ival->reg;
									ir_set_alocated_reg(ctx, use, k, reg);
									IR_LOG_ASSIGN(input, bb->end, use, k, reg);
									continue;
								} else if (ctx->regs[use][0] != IR_REG_NONE
								 && !IR_REG_SPILLED(ctx->regs[use][0])
								 && !IR_REGSET_IN(data.allocated, ctx->regs[use][0])) {
									/* lightweight register coalescing */
									reg = ctx->regs[use][0];
								} else {
									reg = _pick_free_reg(ctx, &data, ival->type, bb->end, input);
								}
								if (reg != IR_REG_NONE) {
									data.state[reg].def = input;
									data.state[reg].root = IR_UNUSED;
									data.state[reg].use = use;
									data.state[reg].op = k;
									IR_REGSET_INCL(data.allocated, reg);
									ival->reg = reg;
									ir_set_alocated_reg(ctx, use, k, reg);
									IR_LOG_ASSIGN(input, bb->end, use, k, reg);
									continue;
								}
							}
						}

						if (ival->stack_spill_pos == -1) {
							if (ctx->vregs[use] > 0
							 && ctx->live_intervals[ctx->vregs[use]]
							 && ctx->live_intervals[ctx->vregs[use]]->stack_spill_pos != -1
							 && _may_coalesce(ctx, input, use)) {
								/* lightweight spill slot coalescing */
								ival->stack_spill_pos = ctx->live_intervals[ctx->vregs[use]]->stack_spill_pos;
								IR_LOG_ASSIGN_SPILL(ival->vreg, ival->stack_spill_pos);
							} else {
								_pick_spill(ctx, ival);
							}
						}
					}
				}
			}

			/* Add tmp registers for MEM->MEM DESSA moves */
			if (needs_tmp_gp) {
				ctx->regs[bb->end][0] = _pick_reg(ctx, &data, IR_I32, IR_REGSET_EMPTY, IR_USE_MUST_BE_IN_REG, 0, 0);
			}
			if (needs_tmp_fp) {
				ctx->regs[bb->end][1] = _pick_reg(ctx, &data, IR_DOUBLE, IR_REGSET_EMPTY, IR_USE_MUST_BE_IN_REG, 0, 0);
			}
		}

		/* For all basic block's instructions in reverse order */
		for (i = bb->end; i != bb->start; i = ctx->prev_ref[i]) {
			uint32_t rule = ctx->rules[i];

			if (rule & (IR_FUSED|IR_SKIPPED)) {
				if ((rule & IR_RULE_MASK) == IR_ALLOCA) {
					insn = ctx->ir_base + i;

					/* assign "static" spill slots for VARs and ALLOCAs */
					if (IR_VREG_IS_STACK_SLOT(ctx->vregs[i])) {
						/* spill slot already allocated */
					} else if (insn->op == IR_VAR) {
						if (ctx->use_lists[i].count > 0) {
							ir_allocate_var_spill_slot(ctx, i, insn);
						}
					} else if (insn->op == IR_ALLOCA) {
						if (ctx->use_lists[i].count > 0) {
							ir_insn *val = &ctx->ir_base[insn->op2];

							IR_ASSERT(IR_IS_CONST_REF(insn->op2));
							IR_ASSERT(IR_IS_TYPE_INT(val->type));
							IR_ASSERT(!IR_IS_SYM_CONST(val->op));
							IR_ASSERT(IR_IS_TYPE_UNSIGNED(val->type) || val->val.i64 >= 0);
							IR_ASSERT(val->val.i64 < 0x7fffffff);
							offset = ir_allocate_spill_slot(ctx, val->val.i32, insn->op3);
							ctx->vregs[i] = IR_STACK_SLOT_TO_VREG(offset);
							IR_LOG_ASSIGN_VAR(i, offset);
						}
					} else if (insn->op == IR_VADDR) {
						if (!IR_VREG_IS_STACK_SLOT(ctx->vregs[insn->op1])) {
							ir_allocate_var_spill_slot(ctx, insn->op1, &ctx->ir_base[insn->op1]);
						}
						ctx->vregs[i] = ctx->vregs[insn->op1];
					}
				}
				/* registers for fused instructions are allocated together with their "root"s */
				continue;
			}

			insn = ctx->ir_base + i;
			if (ctx->vregs[i] > 0) {
				ir_live_interval *ival = ctx->live_intervals[ctx->vregs[i]];

				if (!ival || (ival->reg == IR_REG_NONE && ival->stack_spill_pos == -1)) {
					if (IR_IS_FOLDABLE_OP(insn->op) || (ir_op_flags[insn->op] & IR_OP_FLAG_NO_SIDE_EFFECT)) {
						/* lightweigh dead code elimination */
						ctx->rules[i] = IR_SKIPPED | IR_NOP;
						ctx->vregs[i] = 0;
						continue;
					} else if (insn->op == IR_PARAM) {
						/* unused PARAM */
						ctx->rules[i] = IR_SKIPPED | IR_PARAM;
						ctx->vregs[i] = 0;
						continue;
					} else if (insn->op == IR_CALL) {
						/* unused CALL result */
						ctx->vregs[i] = 0;
					}

				/* Handle PHI specially (inputs come from other blocks) */
				} else if (insn->op == IR_PHI) {
					IR_ASSERT(ival);
					if (ival->reg != IR_REG_NONE) {
						/* use the previously allocated register */
#if IR_X86_I64
						if (ival->type == IR_I64 || ival->type == IR_U64) {
							IR_REGSET_EXCL(data.allocated, ival->reg);
							IR_REGSET_EXCL(data.allocated, ival->reg_hi);
							reg = IR_REG_I64_PAIR(ival->reg, ival->reg_hi);
							if (ival->stack_spill_pos != -1) {
#if 1
								/* Spilled PHI var is passed through memory */
								_evict_reg(ctx, &data, ival->reg);
								reg = IR_REG_NONE;
#else

								reg |= IR_REG_SPILL_STORE;
#endif
							}
							ival->reg = ival->reg_hi = IR_REG_NONE;
							ir_set_alocated_reg(ctx, i, 0, reg);
							if (reg != IR_REG_NONE) {
								IR_LOG_ASSIGN(i, i, i, 0, reg);
							}
							continue;
						}
#endif
						reg = ival->reg;
						IR_REGSET_EXCL(data.allocated, reg);
						if (ival->stack_spill_pos != -1) {
#if 1
							/* Spilled PHI var is passed through memory */
							_evict_reg(ctx, &data, reg);
							reg = IR_REG_NONE;
#else
							reg |= IR_REG_SPILL_STORE;
#endif
						}
						ival->reg = IR_REG_NONE;
						ir_set_alocated_reg(ctx, i, 0, reg);
						if (reg != IR_REG_NONE) {
							IR_LOG_ASSIGN(i, i, i, 0, reg);
						}
					} else {
						IR_ASSERT(ival->stack_spill_pos != -1);
					}
					continue;

				/* An attempt to swap instruction operands for better op1 register reuse */
				} else if ((rule & IR_MAY_SWAP)
				 && insn->op2 > 0
				 && ctx->vregs[insn->op2] > 0
				 && insn->op2 > bb->start
				 && (!ctx->live_intervals[ctx->vregs[insn->op2]]
				  || ctx->live_intervals[ctx->vregs[insn->op2]]->reg == IR_REG_NONE)
				 && !(insn->op1 > 0
				  && ctx->vregs[insn->op1] > 0
				  && insn->op1 > bb->start
				  && (!ctx->live_intervals[ctx->vregs[insn->op1]]
				   || ctx->live_intervals[ctx->vregs[insn->op1]]->reg == IR_REG_NONE))) {
					SWAP_REFS(insn->op1, insn->op2);
				}
			} else if (insn->op == IR_VSTORE || insn->op == IR_VSTORE_v) {
				if (!IR_VREG_IS_STACK_SLOT(ctx->vregs[insn->op2])) {
					ir_allocate_var_spill_slot(ctx, insn->op2, &ctx->ir_base[insn->op2]);
				}
				if (b == 1
				 && insn->op == IR_VSTORE
				 && !IR_IS_CONST_REF(insn->op3)
				 && ctx->ir_base[insn->op3].op == IR_PARAM
				 /* first use of VAR and PARAM is VSTORE(_, VAR, PARAM) */
				 && ctx->use_edges[ctx->use_lists[insn->op2].refs] == i
				 && ctx->use_edges[ctx->use_lists[insn->op3].refs] == i) {
					ir_live_interval *ival = ctx->live_intervals[ctx->vregs[insn->op3]];

					if (!ival) {
						ival = _create_ival(ctx, ctx->vregs[insn->op3], ctx->ir_base[insn->op3].type);
					}
					if (ival->stack_spill_pos == -1) {
						ival->stack_spill_pos = IR_VREG_TO_STACK_SLOT(ctx->vregs[insn->op2]);
						IR_LOG_ASSIGN_SPILL(ival->vreg, ival->stack_spill_pos);
						continue;
					}
				}
			}

			/* Reset information for the current instruction */
			data.num = data.tmp_num = 0;
			data.clobbered_args = data.fixed;
			for (j = 0; j < IR_SUB_REFS_COUNT; j++) {
				data.clobbered[j] = IR_REGSET_EMPTY;
			}

			/* Get infirmation about target constraints */
			def_flags = ir_get_target_constraints(ctx, i, &constraints);

			/* Collect information about scratch and temporary registers */
			n = constraints.tmps_count;
			while (n) {
				n--;
				IR_ASSERT(constraints.tmp_regs[n].start >= 0 && constraints.tmp_regs[n].end < IR_SUB_REFS_COUNT);
				if (constraints.tmp_regs[n].type) {
					ir_reg flags = 0;
					ir_ref op = constraints.tmp_regs[n].num;

					if (op > 0 && op <= insn->inputs_count) {
						ir_ref *ops = insn->ops;

						if (IR_IS_CONST_REF(ops[op])) {
							/* rematerialization */
							flags = IR_REG_SPILL_LOAD;
						} else if (ctx->rules[ops[op]] == IR_STATIC_ALLOCA) {
							/* local address rematerialization */
							flags = IR_REG_SPILL_LOAD;
						}
					}
					_add_tmp_reg(&data, constraints.tmp_regs[n].type,
						constraints.tmp_regs[n].start, constraints.tmp_regs[n].end, flags,
						IR_UNUSED, i, op);
				} else {
					_add_scratch(&data, constraints.tmp_regs[n].reg,
						constraints.tmp_regs[n].start, constraints.tmp_regs[n].end);
				}
			}

			/* Collect information about necessary input registers and related constaints */
			n = insn->inputs_count;
			j = 1;
			p = insn->ops + 1;
			if (ir_op_flags[insn->op] & (IR_OP_FLAG_CONTROL|IR_OP_FLAG_MEM|IR_OP_FLAG_PINNED)) {
				j++;
				p++;
			}
			for (; j <= n; j++, p++) {
				ir_ref input = *p;
				ir_reg reg = (j < constraints.hints_count) ? constraints.hints[j] : IR_REG_NONE;
				ir_live_pos use_pos;
				uint32_t use_flags = IR_USE_FLAGS(def_flags, j);

				if (reg != IR_REG_NONE) {
#if IR_X86_I64
					if (use_flags & IR_HINT_TWO_REGS) {
						IR_REGSET_INCL(data.clobbered[IR_LOAD_SUB_REF], IR_REG_I64_LO(reg));
						IR_REGSET_INCL(data.clobbered[IR_LOAD_SUB_REF], IR_REG_I64_HI(reg));
					} else
#endif
					IR_REGSET_INCL(data.clobbered[IR_LOAD_SUB_REF], reg);
				}

				if (input > 0) {
					if (ctx->vregs[input] > 0) {
						ir_live_interval *ival = ctx->live_intervals[ctx->vregs[input]];

						if (!ival) {
							ival = _create_ival(ctx, ctx->vregs[input], ctx->ir_base[input].type);
						}
						if (input < bb->start || input >= i) {
							/* live before BB start - spill it */
							if (ival->stack_spill_pos == -1) {
								_pick_spill(ctx, ival);
							}
							if (!(use_flags & IR_USE_MUST_BE_IN_REG)) {
								continue;
							}
						}

						use_pos = IR_USE_SUB_REF;
						if (reg != IR_REG_NONE) {
							use_pos = IR_LOAD_SUB_REF;
#if IR_X86_I64
							if (use_flags & IR_HINT_TWO_REGS) {
								IR_REGSET_INCL(data.clobbered[IR_LOAD_SUB_REF], IR_REG_I64_LO(reg));
								IR_REGSET_INCL(data.clobbered[IR_LOAD_SUB_REF], IR_REG_I64_HI(reg));
							} else
#endif
							IR_REGSET_INCL(data.clobbered[IR_LOAD_SUB_REF], reg);
						} else if (def_flags & IR_DEF_REUSES_OP1_REG) {
							if (j == 1) {
								if (def_flags & IR_DEF_CONFLICTS_WITH_INPUT_REGS) {
									use_pos = IR_USE_SUB_REF;
								} else {
									use_pos = IR_LOAD_SUB_REF;
								}
							} else if (input == insn->op1) {
								/* Input is the same as "op1" */
								use_pos = IR_LOAD_SUB_REF;
							}
						}
						_add_reg(&data, ctx->ir_base[input].type, IR_LOAD_SUB_REF, use_pos, reg, use_flags,
							IR_UNUSED, i, j, input);
					} else {
						if ((ctx->rules[input] & (IR_FUSED|IR_SKIPPED)) == IR_FUSED) {
							ir_add_fusion_data(ctx, i, input, &data, bb->start);
						} else if (ctx->rules[input] == (IR_SKIPPED|IR_RLOAD)) {
							ir_set_alocated_reg(ctx, i, j, ctx->ir_base[input].op2);
						}
					}
				}
			}

			/* allocate output register */
			if (ctx->vregs[i] > 0) {
				ir_live_interval *ival;
				ir_live_pos def_pos;
				ir_regset used;

				if (constraints.def_reg != IR_REG_NONE) {
					def_pos = IR_SAVE_SUB_REF;
					if (insn->op == IR_PARAM) {
						/* parameter register must not be used before its loaded */
						IR_REGSET_INCL(param_regs, constraints.def_reg);
					}
				} else if (def_flags & IR_DEF_REUSES_OP1_REG) {
					if (def_flags & IR_DEF_CONFLICTS_WITH_INPUT_REGS) {
						def_pos = IR_USE_SUB_REF;
					} else {
						def_pos = IR_LOAD_SUB_REF;
					}
				} else if (def_flags & IR_DEF_CONFLICTS_WITH_INPUT_REGS) {
					def_pos = IR_LOAD_SUB_REF;
				} else {
					if (insn->op == IR_PARAM) {
						/* We may reuse parameter stack slot for spilling */
						ir_live_interval *ival = ctx->live_intervals[ctx->vregs[i]];

						do {
							if (ctx->use_lists[i].count > 0) {
								ir_ref use = ctx->use_edges[ctx->use_lists[i].refs];
								if (use < bb->end) {
									ir_insn *use_insn = &ctx->ir_base[use];
									if (use_insn->op == IR_VSTORE
									 && !(ctx->rules[use] & IR_SKIPPED)
									 /* first use of VAR is VSTORE(_, VAR, PARAM) */
									 && ctx->use_edges[ctx->use_lists[use_insn->op2].refs] == use) {
										IR_ASSERT(use_insn->op3 == i);
										/* skip VSTORE (VAR is going to be remapped to PARAM) */
										ival->flags |= IR_LIVE_INTERVAL_VAR_MEM_PARAM;
										ival->use_pos = NULL;
										ctx->rules[use] = IR_SKIPPED | IR_NOP;
										if (ctx->use_lists[i].count != 1) {
											break;
										}
									}
								}
							}
							ival->flags |= IR_LIVE_INTERVAL_MEM_PARAM;
							ival->stack_spill_pos = -1; // ???
							IR_LOG_ASSIGN_SPILL(ival->vreg, ival->stack_spill_pos);
						} while (0);
					}
					def_pos = IR_DEF_SUB_REF;
				}

				used = data.fixed;
				for (n = def_pos; n <= IR_SAVE_SUB_REF; n++) {
					used = IR_REGSET_UNION(used, data.clobbered[n]);
				}

				ival = ctx->live_intervals[ctx->vregs[i]];
				if (!ival) {
					ival = _create_ival(ctx, ctx->vregs[i], insn->type);
				}


#if IR_X86_I64
				if (insn->type == IR_I64 || insn->type == IR_U64) {
					/* allocate output register pair */
					ir_reg reg_lo, reg_hi;

					do {
						if (ival->reg != IR_REG_NONE) {
							if (!IR_REGSET_IN(used, ival->reg) && !IR_REGSET_IN(used, ival->reg_hi)) {
								/* use the previously allocated register */
								reg_lo = ival->reg;
								reg_hi = ival->reg_hi;
								reg = IR_REG_I64_PAIR(ival->reg, ival->reg_hi);
								IR_REGSET_EXCL(data.allocated, ival->reg);
								IR_REGSET_EXCL(data.allocated, ival->reg_hi);
								ival->reg = ival->reg_hi = IR_REG_NONE;
								IR_LOG_FREE(i, reg);
								break;
							} else {
								_evict_reg(ctx, &data, ival->reg);
							}
						}

						if (def_flags & IR_USE_MUST_BE_IN_REG) {
							reg = constraints.def_reg;

							if (reg != IR_REG_NONE
							 && !IR_REGSET_IN(IR_REGSET_UNION(used, data.allocated), IR_REG_I64_LO(reg))
							 && !IR_REGSET_IN(IR_REGSET_UNION(used, data.allocated), IR_REG_I64_HI(reg))) {
								/* use fixed register hint */
								reg_hi = IR_REG_I64_HI(reg);
								reg_lo = IR_REG_I64_LO(reg);
							} else {
								reg_lo = _pick_reg(ctx, &data, insn->type, used, IR_USE_MUST_BE_IN_REG, 0, 0);
								IR_REGSET_INCL(used, reg_lo);
								reg_hi = _pick_reg(ctx, &data, insn->type, used, IR_USE_MUST_BE_IN_REG, 0, 0);
								reg = IR_REG_I64_PAIR(ival->reg, ival->reg_hi);
								IR_ASSERT(reg_lo != reg_hi);
								if (reg_lo > reg_hi) {
									SWAP_REGS(reg_lo, reg_hi);
								}
								reg = IR_REG_I64_PAIR(reg_lo, reg_hi);
							}
						} else {
							reg = IR_REG_NONE;
						}
					} while (0);

					if (reg != IR_REG_NONE) {
						for (n = def_pos; n <= IR_SAVE_SUB_REF; n++) {
							IR_REGSET_INCL(data.clobbered[n], reg_lo);
							IR_REGSET_INCL(data.clobbered[n], reg_hi);
						}
						if (ival && ival->stack_spill_pos != -1) {
							reg |= IR_REG_SPILL_STORE;
						}
						ir_set_alocated_reg(ctx, i, 0, reg);
						IR_LOG_ASSIGN(i, i, i, 0, reg);
					}
				} else
#endif
				do {
					/* allocate output register */
					do {
						if (ival->reg != IR_REG_NONE) {
							if (!IR_REGSET_IN(used, ival->reg)) {
								/* use the previously allocated register */
								reg = ival->reg;
								IR_REGSET_EXCL(data.allocated, reg);
								ival->reg = IR_REG_NONE;
								IR_LOG_FREE(i, reg);
								break;
							} else {
								_evict_reg(ctx, &data, ival->reg);
							}
						}
						if (def_flags & IR_USE_MUST_BE_IN_REG) {
							reg = constraints.def_reg;
							if (reg != IR_REG_NONE && !IR_REGSET_IN(IR_REGSET_UNION(used, data.allocated), reg)) {
								/* use fixed register hint */
							} else {
								reg = _pick_reg(ctx, &data, insn->type, used, IR_USE_MUST_BE_IN_REG, 0, 0);
							}
						} else {
							reg = IR_REG_NONE;
						}
					} while (0);

					if (reg != IR_REG_NONE) {
						for (n = def_pos; n <= IR_SAVE_SUB_REF; n++) {
							IR_REGSET_INCL(data.clobbered[n], reg);
						}
						if (ival && ival->stack_spill_pos != -1) {
							reg |= IR_REG_SPILL_STORE;
						}
						ir_set_alocated_reg(ctx, i, 0, reg);
						IR_LOG_ASSIGN(i, i, i, 0, reg);
					}
				} while (0);
			}

			/* evict clobbered registers */
			if (data.allocated != IR_REGSET_EMPTY) {
				ir_regset used = param_regs;

				for (n = 0; n < IR_SUB_REFS_COUNT; n++) {
					used = IR_REGSET_UNION(used, data.clobbered[n]);
				}
				used = IR_REGSET_INTERSECTION(used, data.allocated);
				if (used != IR_REGSET_EMPTY) {
					IR_REGSET_FOREACH(used, reg) {
						_evict_reg(ctx, &data, reg);
					} IR_REGSET_FOREACH_END();
				}
			}

			/* allocate temporary registers */
			for (j = 0; j < data.tmp_num; j++) {
				ir_regset used = data.fixed;

				for (n = data.tmp_regs[j].start; n < data.tmp_regs[j].end; n++) {
					used = IR_REGSET_UNION(used, data.clobbered[n]);
				}

				reg = _pick_reg(ctx, &data, data.tmp_regs[j].type, used, IR_USE_MUST_BE_IN_REG, 0, 0);
				for (n = data.tmp_regs[j].start; n < data.tmp_regs[j].end; n++) {
					IR_REGSET_INCL(data.clobbered[n], reg);
				}
#if IR_X86_I64
				if (data.tmp_regs[j].type == IR_I64 || data.tmp_regs[j].type == IR_U64) {
					ir_reg reg_hi;

					IR_REGSET_INCL(used, reg);
					reg_hi = _pick_reg(ctx, &data, data.tmp_regs[j].type, used, IR_USE_MUST_BE_IN_REG, 0, 0);
					for (n = data.tmp_regs[j].start; n < data.tmp_regs[j].end; n++) {
						IR_REGSET_INCL(data.clobbered[n], reg_hi);
					}
					IR_ASSERT(reg != reg_hi);
					if (reg > reg_hi) {
						SWAP_REGS(reg, reg_hi);
					}
					reg = IR_REG_I64_PAIR(reg, reg_hi);
				}
#endif
				reg = reg | data.tmp_regs[j].flags;
				if (data.tmp_regs[j].op == 4 && insn->inputs_count < 4) {
					if (!ctx->tmp_regs) {
						ctx->tmp_regs = ir_mem_malloc(ctx->insns_count);
						memset(ctx->tmp_regs, -1, ctx->insns_count);
					}
					ctx->tmp_regs[data.tmp_regs[j].ref] = reg;
				} else if (!data.tmp_regs[j].root
				 || ir_get_alocated_reg(ctx, data.tmp_regs[j].ref, data.tmp_regs[j].op) == IR_REG_NONE) {
					ir_set_alocated_reg(ctx, data.tmp_regs[j].ref, data.tmp_regs[j].op, reg);
				} else if (ir_get_alocated_reg(ctx, data.tmp_regs[j].ref, data.tmp_regs[j].op) != reg) {
					ctx->rules[data.tmp_regs[j].ref] |= IR_FUSED_REG;
					ir_set_fused_reg(ctx, data.tmp_regs[j].root, data.tmp_regs[j].ref, data.tmp_regs[j].op, reg);
				}
				IR_LOG_ASSIGN_TMP(data.tmp_regs[j].type, i, data.tmp_regs[j].ref, data.tmp_regs[j].op, reg);
			}

			/* allocate input registers */
			for (j = 0; j < data.num; j++) {
				ir_regset used = data.clobbered_args;
				ir_live_interval *ival;

				for (n = data.regs[j].start; n < data.regs[j].end; n++) {
					used = IR_REGSET_UNION(used, data.clobbered[n]);
				}

#if IR_X86_I64
				if (data.regs[j].type == IR_I64 || data.regs[j].type == IR_U64) {
					ir_reg reg_lo, reg_hi;

					/* allocate input register pair */
					do {
						IR_ASSERT(data.regs[j].def > 0 && ctx->vregs[data.regs[j].def] > 0);
						ival = ctx->live_intervals[ctx->vregs[data.regs[j].def]];
						reg_lo = ival->reg;
						reg_hi = ival->reg_hi;

						/* reuse previously allocated registers if possible */
						if (reg_lo != IR_REG_NONE && reg_hi != IR_REG_NONE) {
							IR_ASSERT(data.state[reg_lo].def == data.regs[j].def);
							IR_ASSERT(data.state[reg_hi].def == data.regs[j].def);
							if ((data.state[reg_lo].root == 0 && data.state[reg_lo].use == i)
							 || data.state[reg_lo].root == i) {
								/* keep previous operand of the same instraction as "last use" */
								break;
							} else {
								/* update "last use" */
								IR_ASSERT(!IR_REGSET_IN(used, reg_lo) && !IR_REGSET_IN(used, ival->reg_hi));
								data.state[reg_lo].root = data.regs[j].root;
								data.state[reg_lo].use = data.regs[j].ref;
								data.state[reg_lo].op = data.regs[j].op;
								data.state[reg_hi].root = data.regs[j].root;
								data.state[reg_hi].use = data.regs[j].ref;
								data.state[reg_hi].op = data.regs[j].op;
								reg = IR_REG_I64_PAIR(reg_lo, reg_hi);
								break;
							}
						}

						/* try fixed register hint */
						reg = data.regs[j].hint;
						reg_hi = IR_REG_NONE;
						reg_lo = IR_REG_NONE;
						if (reg != IR_REG_NONE) {
							if (data.regs[j].flags & IR_HINT_TWO_REGS) {
								reg_hi = IR_REG_I64_HI(reg);
								reg_lo = IR_REG_I64_LO(reg);
							} else {
								reg_lo = reg;
							}
						}

						if (reg_lo != IR_REG_NONE
						 && reg_hi != IR_REG_NONE
						 && !IR_REGSET_IN(IR_REGSET_UNION(used, data.allocated), reg_lo)
						 && !IR_REGSET_IN(IR_REGSET_UNION(used, data.allocated), reg_hi)) {
							/* use fixed register hint */
						} else if ((def_flags & IR_DEF_REUSES_OP1_REG)
						 && data.regs[j].op == 1
						 && data.regs[j].root == IR_UNUSED
						 && ctx->regs[i][0] != IR_REG_NONE
						 && (insn->type == IR_I64 || insn->type == IR_U64)
						 && !IR_REGSET_IN(IR_REGSET_UNION(used, data.allocated), IR_REG_I64_LO(IR_REG_NUM(ctx->regs[i][0])))
						 && !IR_REGSET_IN(IR_REGSET_UNION(used, data.allocated), IR_REG_I64_HI(IR_REG_NUM(ctx->regs[i][0])))
						 && (ival->reg == IR_REG_NONE && ival->stack_spill_pos == -1)) {
							/* reuse for op1 the register allocated for instruction result (lightweigh coalescing) */
							reg = IR_REG_NUM(ctx->regs[i][0]);
							reg_hi = IR_REG_I64_HI(reg);
							reg_lo = IR_REG_I64_LO(reg);
						} else {
							reg_lo = _pick_reg(ctx, &data, data.regs[j].type, used, data.regs[j].flags, i, data.regs[j].def);
							if (reg_lo == IR_REG_NONE) {
								reg = IR_REG_NONE;
								break;
							}
							IR_REGSET_INCL(used, reg_lo);
							reg_hi = _pick_reg(ctx, &data, data.regs[j].type, used, data.regs[j].flags, i, data.regs[j].def);
							IR_REGSET_EXCL(used, reg_lo);
							if (reg_hi == IR_REG_NONE) {
								reg = IR_REG_NONE;
								break;
							}
							IR_ASSERT(reg_lo != reg_hi);
							if (reg_lo > reg_hi) {
								SWAP_REGS(reg_lo, reg_hi);
							}
							reg = IR_REG_I64_PAIR(reg_lo, reg_hi);
						}

						data.state[reg_lo].def = data.regs[j].def;
						data.state[reg_lo].root = data.regs[j].root;
						data.state[reg_lo].use = data.regs[j].ref;
						data.state[reg_lo].op = data.regs[j].op;
						data.state[reg_hi].def = data.regs[j].def;
						data.state[reg_hi].root = data.regs[j].root;
						data.state[reg_hi].use = data.regs[j].ref;
						data.state[reg_hi].op = data.regs[j].op;
						IR_REGSET_INCL(data.allocated, reg_lo);
						IR_REGSET_INCL(data.allocated, reg_hi);
						for (n = data.regs[j].start; n < data.regs[j].end; n++) {
							IR_REGSET_INCL(data.clobbered[n], reg_lo);
							IR_REGSET_INCL(data.clobbered[n], reg_hi);
						}
					} while (0);

					if (reg != IR_REG_NONE) {
						ival->reg = reg_lo;
						ival->reg_hi = reg_hi;
						if (!data.regs[j].root
						 || (!(ctx->rules[data.regs[j].ref] & IR_FUSED_REG)
						  && ir_get_alocated_reg(ctx, data.regs[j].ref, data.regs[j].op) == IR_REG_NONE)) {
							ir_set_alocated_reg(ctx, data.regs[j].ref, data.regs[j].op, reg);
						} else if (ir_get_alocated_reg(ctx, data.regs[j].ref, data.regs[j].op) != reg) {
							ctx->rules[data.regs[j].ref] |= IR_FUSED_REG;
							ir_set_fused_reg(ctx, data.regs[j].root, data.regs[j].ref, data.regs[j].op, reg);
						}
						IR_LOG_ASSIGN(data.regs[j].def, i, data.regs[j].ref, data.regs[j].op, reg);
					} else if (ival->stack_spill_pos == -1) {
						_pick_spill(ctx, ival);
					}

					continue;
				}
#endif

				/* allocate input register */
				do {
					IR_ASSERT(data.regs[j].def > 0 && ctx->vregs[data.regs[j].def] > 0);
					ival = ctx->live_intervals[ctx->vregs[data.regs[j].def]];
					reg = ival->reg;

					/* reuse previously allocated registers if possible */
					if (reg != IR_REG_NONE) {
						IR_ASSERT(data.state[reg].def == data.regs[j].def);
						if ((data.state[reg].root == 0 && data.state[reg].use == i)
						 || data.state[reg].root == i) {
							/* keep previous operand of the same instraction as "last use" */
							break;
						} else {
							/* update "last use" */
							IR_ASSERT(!IR_REGSET_IN(used, reg));
							data.state[reg].root = data.regs[j].root;
							data.state[reg].use = data.regs[j].ref;
							data.state[reg].op = data.regs[j].op;
							break;
						}
					}

					reg = data.regs[j].hint;
					if (reg != IR_REG_NONE && !IR_REGSET_IN(IR_REGSET_UNION(used, data.allocated), reg)) {
						/* use fixed register hint */
					} else if ((def_flags & IR_DEF_REUSES_OP1_REG)
					 && data.regs[j].op == 1
					 && data.regs[j].root == IR_UNUSED
					 && ctx->regs[i][0] != IR_REG_NONE
					 && IR_IS_TYPE_INT(insn->type) == IR_IS_TYPE_INT(data.regs[j].type)
					 && (ival->reg == IR_REG_NONE && ival->stack_spill_pos == -1)) {
						/* reuse for op1 the register allocated for instruction result (lightweigh coalescing) */
						reg = IR_REG_NUM(ctx->regs[i][0]);
#if IR_X86_I64
						if (insn->type == IR_I64 || insn->type == IR_U64) {
							reg = IR_REG_I64_LO(reg);
						}
#endif
						if (IR_REGSET_IN(IR_REGSET_UNION(used, data.allocated), reg)) {
							goto pick;
						}
#if defined(IR_TARGET_X86)
						if (IR_IS_TYPE_INT(data.regs[j].type)
						  && ir_type_size[data.regs[j].type] == 1
						  && (reg == IR_REG_RBP || reg == IR_REG_RSI || reg == IR_REG_RDI)) {
							goto pick;
						}
#endif
					} else {
pick:
						reg = _pick_reg(ctx, &data, data.regs[j].type, used, data.regs[j].flags, i, data.regs[j].def);
						if (reg == IR_REG_NONE) {
							break;
						}
					}
					data.state[reg].def = data.regs[j].def;
					data.state[reg].root = data.regs[j].root;
					data.state[reg].use = data.regs[j].ref;
					data.state[reg].op = data.regs[j].op;
					IR_REGSET_INCL(data.allocated, reg);
					for (n = data.regs[j].start; n < data.regs[j].end; n++) {
						IR_REGSET_INCL(data.clobbered[n], reg);
					}
				} while (0);

				if (reg != IR_REG_NONE) {
					ival->reg = reg;
					if (!data.regs[j].root
					 || (!(ctx->rules[data.regs[j].ref] & IR_FUSED_REG)
					  && ir_get_alocated_reg(ctx, data.regs[j].ref, data.regs[j].op) == IR_REG_NONE)) {
						ir_set_alocated_reg(ctx, data.regs[j].ref, data.regs[j].op, reg);
					} else if (ir_get_alocated_reg(ctx, data.regs[j].ref, data.regs[j].op) != reg) {
						ctx->rules[data.regs[j].ref] |= IR_FUSED_REG;
						ir_set_fused_reg(ctx, data.regs[j].root, data.regs[j].ref, data.regs[j].op, reg);
					}
					IR_LOG_ASSIGN(data.regs[j].def, i, data.regs[j].ref, data.regs[j].op, reg);
				} else if (ival->stack_spill_pos == -1) {
					_pick_spill(ctx, ival);
				}
			}
		}

		if (data.allocated != IR_REGSET_EMPTY) {
			IR_REGSET_FOREACH(data.allocated, reg) {
				_evict_reg(ctx, &data, reg);
			} IR_REGSET_FOREACH_END();
		}
	}

#ifdef IR_TARGET_X86
	if (ctx->flags2 & IR_HAS_FP_RET_SLOT) {
		ctx->ret_slot = ir_allocate_spill_slot(ctx, ir_type_size[IR_DOUBLE], 0);
	} else if ((ctx->ret_type == IR_FLOAT || ctx->ret_type == IR_DOUBLE)
			&& data.ra_data.cc->fp_ret_reg == IR_REG_NONE) {
		ctx->ret_slot = ir_allocate_spill_slot(ctx, ir_type_size[ctx->ret_type], 0);
	} else {
		ctx->ret_slot = -1;
	}
#endif

	ctx->flags |= IR_NO_STACK_COMBINE;
	ir_fix_stack_frame(ctx);
	ctx->data = NULL;

	IR_LOG("Finish Fast Local RA");

	return 1;
}
