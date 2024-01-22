/*
 * IR - Lightweight JIT Compilation Framework
 * (RA - Register Allocation, Liveness, Coalescing, SSA Deconstruction)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 *
 * See: "Linear Scan Register Allocation on SSA Form", Christian Wimmer and
 * Michael Franz, CGO'10 (2010)
 * See: "Optimized Interval Splitting in a Linear Scan Register Allocator",
 * Christian Wimmer VEE'10 (2005)
 */

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include <stdlib.h>
#include "ir.h"

#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
# include "ir_x86.h"
#elif defined(IR_TARGET_AARCH64)
# include "ir_aarch64.h"
#else
# error "Unknown IR target"
#endif

#include "ir_private.h"

int ir_regs_number(void)
{
	return IR_REG_NUM;
}

bool ir_reg_is_int(int32_t reg)
{
	IR_ASSERT(reg >= 0 && reg < IR_REG_NUM);
	return reg >= IR_REG_GP_FIRST && reg <= IR_REG_GP_LAST;
}

static int ir_assign_virtual_registers_slow(ir_ctx *ctx)
{
	uint32_t *vregs;
	uint32_t vregs_count = 0;
	uint32_t b;
	ir_ref i, n;
	ir_block *bb;
	ir_insn *insn;
	uint32_t flags;

	/* Assign unique virtual register to each data node */
	vregs = ir_mem_calloc(ctx->insns_count, sizeof(ir_ref));
	n = 1;
	for (b = 1, bb = ctx->cfg_blocks + b; b <= ctx->cfg_blocks_count; b++, bb++) {
		IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));
		i = bb->start;

		/* skip first instruction */
		insn = ctx->ir_base + i;
		n = ir_insn_len(insn);
		i += n;
		insn += n;
		while (i < bb->end) {
			flags = ir_op_flags[insn->op];
			if (((flags & IR_OP_FLAG_DATA) && insn->op != IR_VAR && (insn->op != IR_PARAM || ctx->use_lists[i].count > 0))
			 || ((flags & IR_OP_FLAG_MEM) && ctx->use_lists[i].count > 1)) {
				if (!ctx->rules || !(ctx->rules[i] & (IR_FUSED|IR_SKIPPED))) {
					vregs[i] = ++vregs_count;
				}
			}
			n = ir_insn_len(insn);
			i += n;
			insn += n;
		}
	}
	ctx->vregs_count = vregs_count;
	ctx->vregs = vregs;

	return 1;
}

int ir_assign_virtual_registers(ir_ctx *ctx)
{
	uint32_t *vregs;
	uint32_t vregs_count = 0;
	ir_ref i;
	ir_insn *insn;

	if (!ctx->rules) {
		return ir_assign_virtual_registers_slow(ctx);
	}

	/* Assign unique virtual register to each rule that needs it */
	vregs = ir_mem_malloc(ctx->insns_count * sizeof(ir_ref));

	for (i = 1, insn = &ctx->ir_base[1]; i < ctx->insns_count; i++, insn++) {
		uint32_t v = 0;

		if (ctx->rules[i] && !(ctx->rules[i] & (IR_FUSED|IR_SKIPPED))) {
			uint32_t flags = ir_op_flags[insn->op];

			if ((flags & IR_OP_FLAG_DATA)
			 || ((flags & IR_OP_FLAG_MEM) && ctx->use_lists[i].count > 1)) {
				v = ++vregs_count;
			}
		}
		vregs[i] = v;
	}

	ctx->vregs_count = vregs_count;
	ctx->vregs = vregs;

	return 1;
}

/* Lifetime intervals construction */

static ir_live_interval *ir_new_live_range(ir_ctx *ctx, int v, ir_live_pos start, ir_live_pos end)
{
	ir_live_interval *ival = ir_arena_alloc(&ctx->arena, sizeof(ir_live_interval));

	ival->type = IR_VOID;
	ival->reg = IR_REG_NONE;
	ival->flags = 0;
	ival->vreg = v;
	ival->stack_spill_pos = -1; // not allocated
	ival->range.start = start;
	ival->range.end = ival->end = end;
	ival->range.next = NULL;
	ival->use_pos = NULL;
	ival->next = NULL;

	ctx->live_intervals[v] = ival;
	return ival;
}

static ir_live_interval *ir_add_live_range(ir_ctx *ctx, int v, ir_live_pos start, ir_live_pos end)
{
	ir_live_interval *ival = ctx->live_intervals[v];
	ir_live_range *p, *q;

	if (!ival) {
		return ir_new_live_range(ctx, v, start, end);
	}

	p = &ival->range;
	if (end >= p->start) {
		ir_live_range *prev = NULL;

		do {
			if (p->end >= start) {
				if (start < p->start) {
					p->start = start;
				}
				if (end > p->end) {
					/* merge with next */
					ir_live_range *next = p->next;

					p->end = end;
					while (next && p->end >= next->start) {
						if (next->end > p->end) {
							p->end = next->end;
						}
						p->next = next->next;
						/* remember in the "unused_ranges" list */
						next->next = ctx->unused_ranges;
						ctx->unused_ranges = next;
						next = p->next;
					}
					if (!p->next) {
						ival->end = p->end;
					}
				}
				return ival;
			}
			prev = p;
			p = prev->next;
		} while (p && end >= p->start);
		if (!p) {
			ival->end = end;
		}
		if (prev) {
			if (ctx->unused_ranges) {
				/* reuse */
				q = ctx->unused_ranges;
				ctx->unused_ranges = q->next;
			} else {
				q = ir_arena_alloc(&ctx->arena, sizeof(ir_live_range));
			}
			prev->next = q;
			q->start = start;
			q->end = end;
			q->next = p;
			return ival;
		}
	}

	if (ctx->unused_ranges) {
		/* reuse */
		q = ctx->unused_ranges;
		ctx->unused_ranges = q->next;
	} else {
		q = ir_arena_alloc(&ctx->arena, sizeof(ir_live_range));
	}
	q->start = p->start;
	q->end = p->end;
	q->next = p->next;
	p->start = start;
	p->end = end;
	p->next = q;
	return ival;
}

IR_ALWAYS_INLINE ir_live_interval *ir_add_prev_live_range(ir_ctx *ctx, int v, ir_live_pos start, ir_live_pos end)
{
	ir_live_interval *ival = ctx->live_intervals[v];

	if (ival && ival->range.start == end) {
		ival->range.start = start;
		return ival;
	}
	return ir_add_live_range(ctx, v, start, end);
}

static void ir_add_fixed_live_range(ir_ctx *ctx, ir_reg reg, ir_live_pos start, ir_live_pos end)
{
	int v = ctx->vregs_count + 1 + reg;
	ir_live_interval *ival = ctx->live_intervals[v];
	ir_live_range *q;

	if (!ival) {
		ival = ir_arena_alloc(&ctx->arena, sizeof(ir_live_interval));
		ival->type = IR_VOID;
		ival->reg = reg;
		ival->flags = IR_LIVE_INTERVAL_FIXED;
		ival->vreg = v;
		ival->stack_spill_pos = -1; // not allocated
		ival->range.start = start;
		ival->range.end = ival->end = end;
		ival->range.next = NULL;
		ival->use_pos = NULL;
		ival->next = NULL;

		ctx->live_intervals[v] = ival;
	} else if (EXPECTED(end < ival->range.start)) {
		if (ctx->unused_ranges) {
			/* reuse */
			q = ctx->unused_ranges;
			ctx->unused_ranges = q->next;
		} else {
			q = ir_arena_alloc(&ctx->arena, sizeof(ir_live_range));
		}

		q->start = ival->range.start;
		q->end = ival->range.end;
		q->next = ival->range.next;
		ival->range.start = start;
		ival->range.end = end;
		ival->range.next = q;
	} else if (end == ival->range.start) {
		ival->range.start = start;
	} else {
		ir_add_live_range(ctx, v, start, end);
	}
}

static void ir_add_tmp(ir_ctx *ctx, ir_ref ref, ir_ref tmp_ref, int32_t tmp_op_num, ir_tmp_reg tmp_reg)
{
	ir_live_interval *ival = ir_arena_alloc(&ctx->arena, sizeof(ir_live_interval));

	ival->type = tmp_reg.type;
	ival->reg = IR_REG_NONE;
	ival->flags = IR_LIVE_INTERVAL_TEMP;
	ival->tmp_ref = tmp_ref;
	ival->tmp_op_num = tmp_op_num;
	ival->range.start = IR_START_LIVE_POS_FROM_REF(ref) + tmp_reg.start;
	ival->range.end = ival->end = IR_START_LIVE_POS_FROM_REF(ref) + tmp_reg.end;
	ival->range.next = NULL;
	ival->use_pos = NULL;

	if (!ctx->live_intervals[0]) {
		ival->next = NULL;
		ctx->live_intervals[0] = ival;
	} else if (ival->range.start >= ctx->live_intervals[0]->range.start) {
		ir_live_interval *prev = ctx->live_intervals[0];

		while (prev->next && ival->range.start >= prev->next->range.start) {
			prev = prev->next;
		}
		ival->next = prev->next;
		prev->next = ival;
	} else {
		ir_live_interval *next = ctx->live_intervals[0];

		ival->next = next;
		ctx->live_intervals[0] = ival;
	}
	return;
}

static bool ir_has_tmp(ir_ctx *ctx, ir_ref ref, int32_t op_num)
{
	ir_live_interval *ival = ctx->live_intervals[0];

	if (ival) {
		while (ival && IR_LIVE_POS_TO_REF(ival->range.start) <= ref) {
			if (ival->tmp_ref == ref && ival->tmp_op_num == op_num) {
				return 1;
			}
			ival = ival->next;
		}
	}
	return 0;
}

static ir_live_interval *ir_fix_live_range(ir_ctx *ctx, int v, ir_live_pos old_start, ir_live_pos new_start)
{
	ir_live_interval *ival = ctx->live_intervals[v];
	ir_live_range *p = &ival->range;

#if 0
	while (p && p->start < old_start) {
		p = p->next;
	}
#endif
	IR_ASSERT(ival && p->start == old_start);
	p->start = new_start;
	return ival;
}

static void ir_add_use_pos(ir_ctx *ctx, ir_live_interval *ival, ir_use_pos *use_pos)
{
	ir_use_pos *p = ival->use_pos;

	if (EXPECTED(!p || p->pos > use_pos->pos)) {
		use_pos->next = p;
		ival->use_pos = use_pos;
	} else {
		ir_use_pos *prev;

		do {
			prev = p;
			p = p->next;
		} while (p && p->pos < use_pos->pos);

		use_pos->next = prev->next;
		prev->next = use_pos;
	}
}


IR_ALWAYS_INLINE void ir_add_use(ir_ctx *ctx, ir_live_interval *ival, int op_num, ir_live_pos pos, ir_reg hint, uint8_t use_flags, ir_ref hint_ref)
{
	ir_use_pos *use_pos;

	use_pos = ir_arena_alloc(&ctx->arena, sizeof(ir_use_pos));
	use_pos->op_num = op_num;
	use_pos->hint = hint;
	use_pos->flags = use_flags;
	use_pos->hint_ref = hint_ref;
	use_pos->pos = pos;

	if (hint != IR_REG_NONE) {
		ival->flags |= IR_LIVE_INTERVAL_HAS_HINT_REGS;
	}
	if (hint_ref > 0) {
		ival->flags |= IR_LIVE_INTERVAL_HAS_HINT_REFS;
	}

	ir_add_use_pos(ctx, ival, use_pos);
}

static void ir_add_phi_use(ir_ctx *ctx, ir_live_interval *ival, int op_num, ir_live_pos pos, ir_ref phi_ref)
{
	ir_use_pos *use_pos;

	IR_ASSERT(phi_ref > 0);
	use_pos = ir_arena_alloc(&ctx->arena, sizeof(ir_use_pos));
	use_pos->op_num = op_num;
	use_pos->hint = IR_REG_NONE;
	use_pos->flags = IR_PHI_USE | IR_USE_SHOULD_BE_IN_REG; // TODO: ???
	use_pos->hint_ref = -phi_ref;
	use_pos->pos = pos;

	ir_add_use_pos(ctx, ival, use_pos);
}

static void ir_add_hint(ir_ctx *ctx, ir_ref ref, ir_live_pos pos, ir_reg hint)
{
	ir_live_interval *ival = ctx->live_intervals[ctx->vregs[ref]];

	if (!(ival->flags & IR_LIVE_INTERVAL_HAS_HINT_REGS)) {
		ir_use_pos *use_pos = ival->use_pos;

		while (use_pos) {
			if (use_pos->pos == pos) {
				if (use_pos->hint == IR_REG_NONE) {
					use_pos->hint = hint;
					ival->flags |= IR_LIVE_INTERVAL_HAS_HINT_REGS;
				}
			}
			use_pos = use_pos->next;
		}
	}
}

static void ir_hint_propagation(ir_ctx *ctx)
{
	int i;
	ir_live_interval *ival;
	ir_use_pos *use_pos;
	ir_use_pos *hint_use_pos;

	for (i = ctx->vregs_count; i > 0; i--) {
		ival = ctx->live_intervals[i];
		if (ival
		 && (ival->flags & (IR_LIVE_INTERVAL_HAS_HINT_REGS|IR_LIVE_INTERVAL_HAS_HINT_REFS)) == (IR_LIVE_INTERVAL_HAS_HINT_REGS|IR_LIVE_INTERVAL_HAS_HINT_REFS)) {
			use_pos = ival->use_pos;
			hint_use_pos = NULL;
			while (use_pos) {
				if (use_pos->op_num == 0) {
					if (use_pos->hint_ref > 0) {
						hint_use_pos = use_pos;
					}
				} else if (use_pos->hint != IR_REG_NONE) {
					if (hint_use_pos) {
						ir_add_hint(ctx, hint_use_pos->hint_ref, hint_use_pos->pos, use_pos->hint);
						hint_use_pos = NULL;
					}
				}
				use_pos = use_pos->next;
			}
		}
	}
}

#ifdef IR_BITSET_LIVENESS
/* DFS + Loop-Forest livness for SSA using bitset(s) */
static void ir_add_osr_entry_loads(ir_ctx *ctx, ir_block *bb, ir_bitset live, uint32_t len, uint32_t b)
{
	bool ok = 1;
	int count = 0;
	ir_list *list = (ir_list*)ctx->osr_entry_loads;
	ir_ref i;

	IR_BITSET_FOREACH(live, len, i) {
		/* Skip live references from ENTRY to PARAM. TODO: duplicate PARAM in each ENTRY ??? */
		ir_use_pos *use_pos = ctx->live_intervals[i]->use_pos;
		ir_ref ref = (use_pos->hint_ref < 0) ? -use_pos->hint_ref : IR_LIVE_POS_TO_REF(use_pos->pos);

		if (use_pos->op_num) {
			ir_ref *ops = ctx->ir_base[ref].ops;
			ref = ops[use_pos->op_num];
		}

		if (ctx->ir_base[ref].op == IR_PARAM) {
			continue;
		}
		if (ctx->binding) {
			ir_ref var = ir_binding_find(ctx, ref);
			if (var < 0) {
				/* We may load the value at OSR entry-point */
				if (!count) {
					bb->flags &= ~IR_BB_EMPTY;
					bb->flags |= IR_BB_OSR_ENTRY_LOADS;
					if (!ctx->osr_entry_loads) {
						list = ctx->osr_entry_loads = ir_mem_malloc(sizeof(ir_list));
						ir_list_init(list, 16);
					}
					ir_list_push(list, b);
					ir_list_push(list, 0);
				}
				ir_list_push(list, ref);
				count++;
				continue;
			}
		}
		fprintf(stderr, "ENTRY %d (block %d start %d) - live var %d\n", ctx->ir_base[bb->start].op2, b, bb->start, ref);
		ok = 0;
	} IR_BITSET_FOREACH_END();

	if (!ok) {
		IR_ASSERT(0);
	}
	if (count) {
		ir_list_set(list, ir_list_len(ctx->osr_entry_loads) - (count + 1), count);

#if 0
		/* ENTRY "clobbers" all registers */
		ir_ref ref = ctx->ir_base[bb->start].op1;
		ir_add_fixed_live_range(ctx, IR_REG_ALL,
			IR_DEF_LIVE_POS_FROM_REF(ref),
			IR_SAVE_LIVE_POS_FROM_REF(ref));
#endif
	}
}

static void ir_add_fusion_ranges(ir_ctx *ctx, ir_ref ref, ir_ref input, ir_block *bb, ir_bitset live)
{
	ir_ref stack[4];
	int stack_pos = 0;
	ir_target_constraints constraints;
	ir_insn *insn;
	uint32_t j, n, flags, def_flags;
	ir_ref *p, child;
	uint8_t use_flags;
	ir_reg reg;
	ir_live_pos use_pos;
	ir_live_interval *ival;

	while (1) {
		IR_ASSERT(input > 0 && ctx->rules[input] & IR_FUSED);

		if (!(ctx->rules[input] & IR_SIMPLE)) {
			def_flags = ir_get_target_constraints(ctx, input, &constraints);
			n = constraints.tmps_count;
			while (n > 0) {
				n--;
				if (constraints.tmp_regs[n].type) {
					ir_add_tmp(ctx, ref, input, constraints.tmp_regs[n].num, constraints.tmp_regs[n]);
				} else {
					/* CPU specific constraints */
					ir_add_fixed_live_range(ctx, constraints.tmp_regs[n].reg,
						IR_START_LIVE_POS_FROM_REF(ref) + constraints.tmp_regs[n].start,
						IR_START_LIVE_POS_FROM_REF(ref) + constraints.tmp_regs[n].end);
				}
			}
		} else {
			def_flags = IR_OP1_MUST_BE_IN_REG | IR_OP2_MUST_BE_IN_REG | IR_OP3_MUST_BE_IN_REG;
			constraints.hints_count = 0;
		}

		insn = &ctx->ir_base[input];
		flags = ir_op_flags[insn->op];
		n = IR_INPUT_EDGES_COUNT(flags);
		j = 1;
		p = insn->ops + j;
		if (flags & IR_OP_FLAG_CONTROL) {
			j++;
			p++;
		}
		for (; j <= n; j++, p++) {
			IR_ASSERT(IR_OPND_KIND(flags, j) == IR_OPND_DATA);
			child = *p;
			if (child > 0) {
				uint32_t v = ctx->vregs[child];

				if (v) {
					use_flags = IR_FUSED_USE | IR_USE_FLAGS(def_flags, j);
					reg = (j < constraints.hints_count) ? constraints.hints[j] : IR_REG_NONE;
					use_pos = IR_LOAD_LIVE_POS_FROM_REF(ref);
					if (EXPECTED(reg == IR_REG_NONE)) {
						use_pos += IR_USE_SUB_REF;
					}

					if (!ir_bitset_in(live, v)) {
						/* live.add(opd) */
						ir_bitset_incl(live, v);
						/* intervals[opd].addRange(b.from, op.id) */
						ival = ir_add_live_range(ctx, v,
							IR_START_LIVE_POS_FROM_REF(bb->start), use_pos);
					} else {
						ival = ctx->live_intervals[v];
					}
					ir_add_use(ctx, ival, j, use_pos, reg, use_flags, -input);
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

int ir_compute_live_ranges(ir_ctx *ctx)
{
	uint32_t b, i, j, k, n, succ, *p;
	ir_ref ref;
	uint32_t len;
	ir_insn *insn;
	ir_block *bb, *succ_bb;
#ifdef IR_DEBUG
	ir_bitset visited;
#endif
	ir_bitset live, bb_live;
	ir_bitset loops = NULL;
	ir_bitqueue queue;
	ir_live_interval *ival;

	if (!(ctx->flags2 & IR_LINEAR) || !ctx->vregs) {
		return 0;
	}

	if (ctx->rules) {
		ctx->regs = ir_mem_malloc(sizeof(ir_regs) * ctx->insns_count);
		memset(ctx->regs, IR_REG_NONE, sizeof(ir_regs) * ctx->insns_count);
	}

	/* Root of the list of IR_VARs */
	ctx->vars = IR_UNUSED;

	/* Compute Live Ranges */
	ctx->flags2 &= ~IR_LR_HAVE_DESSA_MOVES;
	len = ir_bitset_len(ctx->vregs_count + 1);
	bb_live = ir_mem_malloc((ctx->cfg_blocks_count + 1) * len * sizeof(ir_bitset_base_t));

	/* vregs + tmp + fixed + SRATCH + ALL */
	ctx->live_intervals = ir_mem_calloc(ctx->vregs_count + 1 + IR_REG_NUM + 2, sizeof(ir_live_interval*));

#ifdef IR_DEBUG
	visited = ir_bitset_malloc(ctx->cfg_blocks_count + 1);
#endif

    if (!ctx->arena) {
		ctx->arena = ir_arena_create(16 * 1024);
	}

	/* for each basic block in reverse order */
	for (b = ctx->cfg_blocks_count; b > 0; b--) {
		bb = &ctx->cfg_blocks[b];
		IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));
		/* for each successor of b */

#ifdef IR_DEBUG
		ir_bitset_incl(visited, b);
#endif
		live = bb_live + (len * b);
		n = bb->successors_count;
		if (n == 0) {
			ir_bitset_clear(live, len);
		} else {
			p = &ctx->cfg_edges[bb->successors];
			succ = *p;

#ifdef IR_DEBUG
			/* blocks must be ordered where all dominators of a block are before this block */
			IR_ASSERT(ir_bitset_in(visited, succ) || bb->loop_header == succ);
#endif

			/* live = union of successors.liveIn */
			if (EXPECTED(succ > b) && EXPECTED(!(ctx->cfg_blocks[succ].flags & IR_BB_ENTRY))) {
				ir_bitset_copy(live, bb_live + (len * succ), len);
			} else {
				IR_ASSERT(succ > b || (ctx->cfg_blocks[succ].flags & IR_BB_LOOP_HEADER));
				ir_bitset_clear(live, len);
			}
			if (n > 1) {
				for (p++, n--; n > 0; p++, n--) {
					succ = *p;
					if (EXPECTED(succ > b) && EXPECTED(!(ctx->cfg_blocks[succ].flags & IR_BB_ENTRY))) {
						ir_bitset_union(live, bb_live + (len * succ), len);
					} else {
						IR_ASSERT(succ > b || (ctx->cfg_blocks[succ].flags & IR_BB_LOOP_HEADER));
					}
				}
			}

			/* for each opd in live */
			IR_BITSET_FOREACH(live, len, i) {
				/* intervals[opd].addRange(b.from, b.to) */
				ir_add_prev_live_range(ctx, i,
					IR_START_LIVE_POS_FROM_REF(bb->start),
					IR_END_LIVE_POS_FROM_REF(bb->end));
			} IR_BITSET_FOREACH_END();
		}

		if (bb->successors_count == 1) {
			/* for each phi function phi of successor */
			succ = ctx->cfg_edges[bb->successors];
			succ_bb = &ctx->cfg_blocks[succ];
			if (succ_bb->flags & IR_BB_HAS_PHI) {
				ir_use_list *use_list = &ctx->use_lists[succ_bb->start];

				k = ir_phi_input_number(ctx, succ_bb, b);
				IR_ASSERT(k != 0);
				for (ref = 0; ref < use_list->count; ref++) {
					ir_ref use = ctx->use_edges[use_list->refs + ref];
					insn = &ctx->ir_base[use];
					if (insn->op == IR_PHI) {
						ir_ref input = ir_insn_op(insn, k);
						if (input > 0) {
							uint32_t v = ctx->vregs[input];

							/* live.add(phi.inputOf(b)) */
							IR_ASSERT(v);
							ir_bitset_incl(live, v);
							/* intervals[phi.inputOf(b)].addRange(b.from, b.to) */
							ival = ir_add_prev_live_range(ctx, v,
								IR_START_LIVE_POS_FROM_REF(bb->start),
								IR_END_LIVE_POS_FROM_REF(bb->end));
							ir_add_phi_use(ctx, ival, k, IR_DEF_LIVE_POS_FROM_REF(bb->end), use);
						}
					}
				}
			}
		}

		/* for each operation op of b in reverse order */
		ref = bb->end;
		insn = &ctx->ir_base[ref];
		if (insn->op == IR_END || insn->op == IR_LOOP_END) {
			ref = ctx->prev_ref[ref];
		}
		for (; ref > bb->start; ref = ctx->prev_ref[ref]) {
			uint32_t def_flags;
			uint32_t flags;
			ir_ref *p;
			ir_target_constraints constraints;
			uint32_t v;

			if (ctx->rules) {
				int n;

				if (ctx->rules[ref] & (IR_FUSED|IR_SKIPPED)) {
					if (ctx->rules[ref] == (IR_SKIPPED|IR_VAR) && ctx->use_lists[ref].count > 0) {
						insn = &ctx->ir_base[ref];
						insn->op3 = ctx->vars;
						ctx->vars = ref;
					}
					continue;
				}

				def_flags = ir_get_target_constraints(ctx, ref, &constraints);
				n = constraints.tmps_count;
				while (n > 0) {
					n--;
					if (constraints.tmp_regs[n].type) {
						ir_add_tmp(ctx, ref, ref, constraints.tmp_regs[n].num, constraints.tmp_regs[n]);
					} else {
						/* CPU specific constraints */
						ir_add_fixed_live_range(ctx, constraints.tmp_regs[n].reg,
							IR_START_LIVE_POS_FROM_REF(ref) + constraints.tmp_regs[n].start,
							IR_START_LIVE_POS_FROM_REF(ref) + constraints.tmp_regs[n].end);
					}
				}
			} else {
				def_flags = 0;
				constraints.def_reg = IR_REG_NONE;
				constraints.hints_count = 0;
			}

			insn = &ctx->ir_base[ref];
			v = ctx->vregs[ref];
			if (v) {
				IR_ASSERT(ir_bitset_in(live, v));

				if (insn->op != IR_PHI) {
					ir_live_pos def_pos;
					ir_ref hint_ref = 0;
					ir_reg reg = constraints.def_reg;

					if (reg != IR_REG_NONE) {
						def_pos = IR_SAVE_LIVE_POS_FROM_REF(ref);
						if (insn->op == IR_PARAM || insn->op == IR_RLOAD) {
							/* parameter register must be kept before it's copied */
							ir_add_fixed_live_range(ctx, reg, IR_START_LIVE_POS_FROM_REF(bb->start), def_pos);
						}
					} else if (def_flags & IR_DEF_REUSES_OP1_REG) {
						if (!IR_IS_CONST_REF(insn->op1) && ctx->vregs[insn->op1]) {
							hint_ref = insn->op1;
						}
						def_pos = IR_LOAD_LIVE_POS_FROM_REF(ref);
					} else if (def_flags & IR_DEF_CONFLICTS_WITH_INPUT_REGS) {
						def_pos = IR_LOAD_LIVE_POS_FROM_REF(ref);
					} else {
						if (insn->op == IR_PARAM) {
							/* We may reuse parameter stack slot for spilling */
							ctx->live_intervals[v]->flags |= IR_LIVE_INTERVAL_MEM_PARAM;
						} else if (insn->op == IR_VLOAD) {
							/* Load may be fused into the usage instruction */
							ctx->live_intervals[v]->flags |= IR_LIVE_INTERVAL_MEM_LOAD;
						}
						def_pos = IR_DEF_LIVE_POS_FROM_REF(ref);
					}
					/* live.remove(opd) */
					ir_bitset_excl(live, v);
					/* intervals[opd].setFrom(op.id) */
					ival = ir_fix_live_range(ctx, v,
						IR_START_LIVE_POS_FROM_REF(bb->start), def_pos);
					ival->type = insn->type;
					ir_add_use(ctx, ival, 0, def_pos, reg, def_flags, hint_ref);
				} else {
					/* live.remove(opd) */
					ir_bitset_excl(live, v);
					/* PHIs inputs must not be processed */
					ival = ctx->live_intervals[v];
					if (UNEXPECTED(!ival)) {
						/* Dead PHI */
						ival = ir_add_live_range(ctx, v, IR_DEF_LIVE_POS_FROM_REF(ref), IR_USE_LIVE_POS_FROM_REF(ref));
					}
					ival->type = insn->type;
					ir_add_use(ctx, ival, 0, IR_DEF_LIVE_POS_FROM_REF(ref), IR_REG_NONE, IR_USE_SHOULD_BE_IN_REG, 0);
					continue;
				}
			}

			IR_ASSERT(insn->op != IR_PHI && (!ctx->rules || !(ctx->rules[ref] & (IR_FUSED|IR_SKIPPED))));
			flags = ir_op_flags[insn->op];
			j = 1;
			p = insn->ops + 1;
			if (flags & (IR_OP_FLAG_CONTROL|IR_OP_FLAG_MEM|IR_OP_FLAG_PINNED)) {
				j++;
				p++;
			}
			for (; j <= insn->inputs_count; j++, p++) {
				ir_ref input = *p;
				ir_reg reg = (j < constraints.hints_count) ? constraints.hints[j] : IR_REG_NONE;
				ir_live_pos use_pos;
				ir_ref hint_ref = 0;
				uint32_t v;

				if (input > 0) {
					v = ctx->vregs[input];
					if (v) {
						use_pos = IR_USE_LIVE_POS_FROM_REF(ref);
						if (reg != IR_REG_NONE) {
							use_pos = IR_LOAD_LIVE_POS_FROM_REF(ref);
							ir_add_fixed_live_range(ctx, reg, use_pos, use_pos + IR_USE_SUB_REF);
						} else if (def_flags & IR_DEF_REUSES_OP1_REG) {
							if (j == 1) {
								use_pos = IR_LOAD_LIVE_POS_FROM_REF(ref);
								IR_ASSERT(ctx->vregs[ref]);
								hint_ref = ref;
							} else if (input == insn->op1) {
								/* Input is the same as "op1" */
								use_pos = IR_LOAD_LIVE_POS_FROM_REF(ref);
							}
						}
						if (!ir_bitset_in(live, v)) {
							/* live.add(opd) */
							ir_bitset_incl(live, v);
							/* intervals[opd].addRange(b.from, op.id) */
							ival = ir_add_live_range(ctx, v, IR_START_LIVE_POS_FROM_REF(bb->start), use_pos);
						} else {
							ival = ctx->live_intervals[v];
						}
						ir_add_use(ctx, ival, j, use_pos, reg, IR_USE_FLAGS(def_flags, j), hint_ref);
					} else if (ctx->rules) {
						if (ctx->rules[input] & IR_FUSED) {
						    ir_add_fusion_ranges(ctx, ref, input, bb, live);
						} else if (ctx->rules[input] == (IR_SKIPPED|IR_RLOAD)) {
							ir_set_alocated_reg(ctx, ref, j, ctx->ir_base[input].op2);
						}
					}
				} else if (reg != IR_REG_NONE) {
					use_pos = IR_LOAD_LIVE_POS_FROM_REF(ref);
					ir_add_fixed_live_range(ctx, reg, use_pos, use_pos + IR_USE_SUB_REF);
				}
			}
		}

		/* if b is loop header */
		if ((bb->flags & IR_BB_LOOP_HEADER)
		 && !ir_bitset_empty(live, len)) {
			/* variables live at loop header are alive at the whole loop body */
			uint32_t bb_set_len = ir_bitset_len(ctx->cfg_blocks_count + 1);
			uint32_t child;
			ir_block *child_bb;
			ir_bitset child_live_in;

			if (!loops) {
				loops = ir_bitset_malloc(ctx->cfg_blocks_count + 1);
				ir_bitqueue_init(&queue, ctx->cfg_blocks_count + 1);
			} else {
				ir_bitset_clear(loops, bb_set_len);
				ir_bitqueue_clear(&queue);
			}
			ir_bitset_incl(loops, b);
			child = b;
			do {
				child_bb = &ctx->cfg_blocks[child];
				child_live_in = bb_live + (len * child);

				IR_BITSET_FOREACH(live, len, i) {
					ir_bitset_incl(child_live_in, i);
					ir_add_live_range(ctx, i,
						IR_START_LIVE_POS_FROM_REF(child_bb->start),
						IR_END_LIVE_POS_FROM_REF(child_bb->end));
				} IR_BITSET_FOREACH_END();

				child = child_bb->dom_child;
				while (child) {
					child_bb = &ctx->cfg_blocks[child];
					if (child_bb->loop_header && ir_bitset_in(loops, child_bb->loop_header)) {
						ir_bitqueue_add(&queue, child);
						if (child_bb->flags & IR_BB_LOOP_HEADER) {
							ir_bitset_incl(loops, child);
						}
					}
					child = child_bb->dom_next_child;
				}
			} while ((child = ir_bitqueue_pop(&queue)) != (uint32_t)-1);
		}
	}

	if (ctx->entries) {
		for (i = 0; i < ctx->entries_count; i++) {
			b = ctx->entries[i];
			bb = &ctx->cfg_blocks[b];
			live = bb_live + (len * b);
			ir_add_osr_entry_loads(ctx, bb, live, len, b);
		}
		if (ctx->osr_entry_loads) {
			ir_list_push((ir_list*)ctx->osr_entry_loads, 0);
		}
	}

	if (loops) {
		ir_mem_free(loops);
		ir_bitqueue_free(&queue);
	}

	ir_mem_free(bb_live);
#ifdef IR_DEBUG
	ir_mem_free(visited);
#endif

	return 1;
}

#else
/* Path exploration by definition liveness for SSA using sets represented by linked lists */

#define IS_LIVE_IN_BLOCK(v, b) \
	(live_in_block[v] == b)
#define SET_LIVE_IN_BLOCK(v, b) do { \
		live_in_block[v] = b; \
	} while (0)

/* Returns the last virtual register alive at the end of the block (it is used as an already-visited marker) */
IR_ALWAYS_INLINE uint32_t ir_live_out_top(ir_ctx *ctx, uint32_t *live_outs, ir_list *live_lists, uint32_t b)
{
#if 0
	return live_outs[b];
#else
	if (!live_outs[b]) {
		return -1;
	}
	return ir_list_at(live_lists, live_outs[b]);
#endif
}

/* Remember a virtual register alive at the end of the block */
IR_ALWAYS_INLINE void ir_live_out_push(ir_ctx *ctx, uint32_t *live_outs, ir_list *live_lists, uint32_t b, uint32_t v)
{
#if 0
	ir_block *bb = &ctx->cfg_blocks[b];
	live_outs[b] = v;
	ir_add_prev_live_range(ctx, v,
		IR_START_LIVE_POS_FROM_REF(bb->start),
		IR_END_LIVE_POS_FROM_REF(bb->end));
#else
	if (live_lists->len >= live_lists->a.size) {
		ir_array_grow(&live_lists->a, live_lists->a.size + 1024);
	}
	/* Form a linked list of virtual register live at the end of the block */
	ir_list_push_unchecked(live_lists, live_outs[b]); /* push old root of the list (previous element of the list) */
	live_outs[b] = ir_list_len(live_lists);           /* remember the new root */
	ir_list_push_unchecked(live_lists, v);            /* push a virtual register */
#endif
}

/*
 * Computes live-out sets for each basic-block per variable using def-use chains.
 *
 * The implementation is based on algorithms 6 and 7 desriebed in
 * "Computing Liveness Sets for SSA-Form Programs", Florian Brandner, Benoit Boissinot.
 * Alain Darte, Benoit Dupont de Dinechin, Fabrice Rastello. TR Inria RR-7503, 2011
 */
static void ir_compute_live_sets(ir_ctx *ctx, uint32_t *live_outs, ir_list *live_lists)
{
	ir_list block_queue, fuse_queue;
	ir_ref i;

	ir_list_init(&fuse_queue, 16);
	ir_list_init(&block_queue, 256);

	/* For each virtual register explore paths from all uses to definition */
	for (i = ctx->insns_count - 1; i > 0; i--) {
		uint32_t v = ctx->vregs[i];

		if (v) {
			uint32_t def_block = ctx->cfg_map[i];
			ir_use_list *use_list = &ctx->use_lists[i];
			ir_ref *p, n = use_list->count;

			/* Collect all blocks where 'v' is used into a 'block_queue' */
			for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
				ir_ref use = *p;
				ir_insn *insn = &ctx->ir_base[use];

				if (UNEXPECTED(insn->op == IR_PHI)) {
					ir_ref n = insn->inputs_count - 1;
					ir_ref *p = insn->ops + 2; /* PHI data inputs */
					ir_ref *q = ctx->ir_base[insn->op1].ops + 1; /* MERGE inputs */

					for (;n > 0; p++, q++, n--) {
						if (*p == i) {
							uint32_t pred_block = ctx->cfg_map[*q];

							if (ir_live_out_top(ctx, live_outs, live_lists, pred_block) != v) {
								ir_live_out_push(ctx, live_outs, live_lists, pred_block, v);
								if (pred_block != def_block) {
									ir_list_push(&block_queue, pred_block);
								}
							}
						}
					}
				} else if (ctx->rules && UNEXPECTED(ctx->rules[use] & IR_FUSED)) {
					while (1) {
						ir_use_list *use_list = &ctx->use_lists[use];
						ir_ref *p, n = use_list->count;

						for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
							ir_ref use = *p;

							if (ctx->rules[use] & IR_FUSED) {
								ir_list_push(&fuse_queue, use);
							} else {
								uint32_t use_block = ctx->cfg_map[use];

								if (def_block != use_block && ir_live_out_top(ctx, live_outs, live_lists, use_block) != v) {
									ir_list_push(&block_queue, use_block);
								}
							}
						}
						if (!ir_list_len(&fuse_queue)) {
							break;
						}
						use = ir_list_pop(&fuse_queue);
					}
				} else {
					uint32_t use_block = ctx->cfg_map[use];

					/* Check if the virtual register is alive at the start of 'use_block' */
					if (def_block != use_block && ir_live_out_top(ctx, live_outs, live_lists, use_block) != v) {
						ir_list_push(&block_queue, use_block);
					}
				}
			}

			/* UP_AND_MARK: Traverse through predecessor blocks until we reach the block where 'v' is defined*/
			while (ir_list_len(&block_queue)) {
				uint32_t b = ir_list_pop(&block_queue);
				ir_block *bb = &ctx->cfg_blocks[b];
				uint32_t *p, n = bb->predecessors_count;

				if (bb->flags & IR_BB_ENTRY) {
					/* live_in_push(ENTRY, v) */
					ir_insn *insn = &ctx->ir_base[bb->start];

					IR_ASSERT(insn->op == IR_ENTRY);
					IR_ASSERT(insn->op3 >= 0 && insn->op3 < (ir_ref)ctx->entries_count);
					if (live_lists->len >= live_lists->a.size) {
						ir_array_grow(&live_lists->a, live_lists->a.size + 1024);
					}
					ir_list_push_unchecked(live_lists, live_outs[ctx->cfg_blocks_count + 1 + insn->op3]);
					ir_list_push_unchecked(live_lists, v);
					live_outs[ctx->cfg_blocks_count + 1 + insn->op3] = ir_list_len(live_lists) - 1;
					continue;
				}
				for (p = &ctx->cfg_edges[bb->predecessors]; n > 0; p++, n--) {
					uint32_t pred_block = *p;

					/* Check if 'pred_block' wasn't traversed before */
					if (ir_live_out_top(ctx, live_outs, live_lists, pred_block) != v) {
						/* Mark a virtual register 'v' alive at the end of 'pred_block' */
						ir_live_out_push(ctx, live_outs, live_lists, pred_block, v);
						if (pred_block != def_block) {
							ir_list_push(&block_queue, pred_block);
						}
					}
				}
			}
		}
	}

	ir_list_free(&block_queue);
	ir_list_free(&fuse_queue);
}

static void ir_add_osr_entry_loads(ir_ctx *ctx, ir_block *bb, uint32_t pos, ir_list *live_lists, uint32_t b)
{
	bool ok = 1;
	int count = 0;
	ir_list *list = (ir_list*)ctx->osr_entry_loads;
	ir_ref i;

	while (pos) {
		i = ir_list_at(live_lists, pos);
		pos = ir_list_at(live_lists, pos - 1);

		/* Skip live references from ENTRY to PARAM. TODO: duplicate PARAM in each ENTRY ??? */
		ir_use_pos *use_pos = ctx->live_intervals[i]->use_pos;
		ir_ref ref = (use_pos->hint_ref < 0) ? -use_pos->hint_ref : IR_LIVE_POS_TO_REF(use_pos->pos);

		if (use_pos->op_num) {
			ir_ref *ops = ctx->ir_base[ref].ops;
			ref = ops[use_pos->op_num];
		}

		if (ctx->ir_base[ref].op == IR_PARAM) {
			continue;
		}
		if (ctx->binding) {
			ir_ref var = ir_binding_find(ctx, ref);
			if (var < 0) {
				/* We may load the value at OSR entry-point */
				if (!count) {
					bb->flags &= ~IR_BB_EMPTY;
					bb->flags |= IR_BB_OSR_ENTRY_LOADS;
					if (!ctx->osr_entry_loads) {
						list = ctx->osr_entry_loads = ir_mem_malloc(sizeof(ir_list));
						ir_list_init(list, 16);
					}
					ir_list_push(list, b);
					ir_list_push(list, 0);
				}
				ir_list_push(list, ref);
				count++;
				continue;
			}
		}
		fprintf(stderr, "ENTRY %d (block %d start %d) - live var %d\n", ctx->ir_base[bb->start].op2, b, bb->start, ref);
		ok = 0;
	}

	if (!ok) {
		IR_ASSERT(0);
	}
	if (count) {
		ir_list_set(list, ir_list_len(ctx->osr_entry_loads) - (count + 1), count);

#if 0
		/* ENTRY "clobbers" all registers */
		ir_ref ref = ctx->ir_base[bb->start].op1;
		ir_add_fixed_live_range(ctx, IR_REG_ALL,
			IR_DEF_LIVE_POS_FROM_REF(ref),
			IR_SAVE_LIVE_POS_FROM_REF(ref));
#endif
	}
}

static void ir_add_fusion_ranges(ir_ctx *ctx, ir_ref ref, ir_ref input, ir_block *bb, uint32_t *live_in_block, uint32_t b)
{
	ir_ref stack[4];
	int stack_pos = 0;
	ir_target_constraints constraints;
	ir_insn *insn;
	uint32_t j, n, flags, def_flags;
	ir_ref *p, child;
	uint8_t use_flags;
	ir_reg reg;
	ir_live_pos pos = IR_START_LIVE_POS_FROM_REF(ref);
	ir_live_pos use_pos;
	ir_live_interval *ival;

	while (1) {
		IR_ASSERT(input > 0 && ctx->rules[input] & IR_FUSED);

		if (!(ctx->rules[input] & IR_SIMPLE)) {
			def_flags = ir_get_target_constraints(ctx, input, &constraints);
			n = constraints.tmps_count;
			while (n > 0) {
				n--;
				if (constraints.tmp_regs[n].type) {
					ir_add_tmp(ctx, ref, input, constraints.tmp_regs[n].num, constraints.tmp_regs[n]);
				} else {
					/* CPU specific constraints */
					ir_add_fixed_live_range(ctx, constraints.tmp_regs[n].reg,
						pos + constraints.tmp_regs[n].start,
						pos + constraints.tmp_regs[n].end);
				}
			}
		} else {
			def_flags = IR_OP1_MUST_BE_IN_REG | IR_OP2_MUST_BE_IN_REG | IR_OP3_MUST_BE_IN_REG;
			constraints.hints_count = 0;
		}

		insn = &ctx->ir_base[input];
		flags = ir_op_flags[insn->op];
		IR_ASSERT(!IR_OP_HAS_VAR_INPUTS(flags));
		n = IR_INPUT_EDGES_COUNT(flags);
		j = 1;
		p = insn->ops + j;
		if (flags & IR_OP_FLAG_CONTROL) {
			j++;
			p++;
		}
		for (; j <= n; j++, p++) {
			IR_ASSERT(IR_OPND_KIND(flags, j) == IR_OPND_DATA);
			child = *p;
			if (child > 0) {
				uint32_t v = ctx->vregs[child];

				if (v) {
					reg = (j < constraints.hints_count) ? constraints.hints[j] : IR_REG_NONE;
					use_pos = pos;
					if (EXPECTED(reg == IR_REG_NONE)) {
						use_pos += IR_USE_SUB_REF;
					}

					if (!IS_LIVE_IN_BLOCK(v, b)) {
						/* live.add(opd) */
						SET_LIVE_IN_BLOCK(v, b);
						/* intervals[opd].addRange(b.from, op.id) */
						ival = ir_add_live_range(ctx, v,
							IR_START_LIVE_POS_FROM_REF(bb->start), use_pos);
					} else {
						ival = ctx->live_intervals[v];
					}
					use_flags = IR_FUSED_USE | IR_USE_FLAGS(def_flags, j);
					ir_add_use(ctx, ival, j, use_pos, reg, use_flags, -input);
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

int ir_compute_live_ranges(ir_ctx *ctx)
{
	uint32_t b, i, j, k, n, succ;
	ir_ref ref;
	ir_insn *insn;
	ir_block *bb, *succ_bb;
	uint32_t *live_outs;
	uint32_t *live_in_block;
	ir_list live_lists;
	ir_live_interval *ival;

	if (!(ctx->flags2 & IR_LINEAR) || !ctx->vregs) {
		return 0;
	}

	if (ctx->rules) {
		ctx->regs = ir_mem_malloc(sizeof(ir_regs) * ctx->insns_count);
		memset(ctx->regs, IR_REG_NONE, sizeof(ir_regs) * ctx->insns_count);
	}

	/* Root of the list of IR_VARs */
	ctx->vars = IR_UNUSED;

	/* Compute Live Ranges */
	ctx->flags2 &= ~IR_LR_HAVE_DESSA_MOVES;

	/* vregs + tmp + fixed + SRATCH + ALL */
	ctx->live_intervals = ir_mem_calloc(ctx->vregs_count + 1 + IR_REG_NUM + 2, sizeof(ir_live_interval*));

    if (!ctx->arena) {
		ctx->arena = ir_arena_create(16 * 1024);
	}

	live_outs = ir_mem_calloc(ctx->cfg_blocks_count + 1 + ctx->entries_count, sizeof(uint32_t));
	ir_list_init(&live_lists, 1024);
	ir_compute_live_sets(ctx, live_outs, &live_lists);
	live_in_block = ir_mem_calloc(ctx->vregs_count + 1, sizeof(uint32_t));

	/* for each basic block in reverse order */
	for (b = ctx->cfg_blocks_count; b > 0; b--) {
		bb = &ctx->cfg_blocks[b];
		IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));

		/* For all virtual register alive at the end of the block */
		n = live_outs[b];
		while (n != 0) {
			i = ir_list_at(&live_lists, n);
			SET_LIVE_IN_BLOCK(i, b);
			ir_add_prev_live_range(ctx, i,
				IR_START_LIVE_POS_FROM_REF(bb->start),
				IR_END_LIVE_POS_FROM_REF(bb->end));
			n = ir_list_at(&live_lists, n - 1);
		}

		if (bb->successors_count == 1) {
			/* for each phi function of the successor */
			succ = ctx->cfg_edges[bb->successors];
			succ_bb = &ctx->cfg_blocks[succ];
			if (succ_bb->flags & IR_BB_HAS_PHI) {
				ir_use_list *use_list = &ctx->use_lists[succ_bb->start];
				ir_ref n, *p;

				k = ir_phi_input_number(ctx, succ_bb, b);
				IR_ASSERT(k != 0);
				n = use_list->count;
				for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
					ir_ref use = *p;
					insn = &ctx->ir_base[use];
					if (insn->op == IR_PHI) {
						ir_ref input = ir_insn_op(insn, k);
						if (input > 0) {
							uint32_t v = ctx->vregs[input];

							IR_ASSERT(v);
							ival = ctx->live_intervals[v];
							ir_add_phi_use(ctx, ival, k, IR_DEF_LIVE_POS_FROM_REF(bb->end), use);
						}
					}
				}
			}
		}

		/* for each operation of the block in reverse order */
		ref = bb->end;
		insn = &ctx->ir_base[ref];
		if (insn->op == IR_END || insn->op == IR_LOOP_END) {
			ref = ctx->prev_ref[ref];
		}
		for (; ref > bb->start; ref = ctx->prev_ref[ref]) {
			uint32_t def_flags;
			uint32_t flags;
			ir_ref *p;
			ir_target_constraints constraints;
			uint32_t v;

			if (ctx->rules) {
				int n;

				if (ctx->rules[ref] & (IR_FUSED|IR_SKIPPED)) {
					if (ctx->rules[ref] == (IR_SKIPPED|IR_VAR) && ctx->use_lists[ref].count > 0) {
						insn = &ctx->ir_base[ref];
						insn->op3 = ctx->vars;
						ctx->vars = ref;
					}
					continue;
				}

				def_flags = ir_get_target_constraints(ctx, ref, &constraints);
				n = constraints.tmps_count;
				while (n > 0) {
					n--;
					if (constraints.tmp_regs[n].type) {
						ir_add_tmp(ctx, ref, ref, constraints.tmp_regs[n].num, constraints.tmp_regs[n]);
					} else {
						/* CPU specific constraints */
						ir_add_fixed_live_range(ctx, constraints.tmp_regs[n].reg,
							IR_START_LIVE_POS_FROM_REF(ref) + constraints.tmp_regs[n].start,
							IR_START_LIVE_POS_FROM_REF(ref) + constraints.tmp_regs[n].end);
					}
				}
			} else {
				def_flags = 0;
				constraints.def_reg = IR_REG_NONE;
				constraints.hints_count = 0;
			}

			insn = &ctx->ir_base[ref];
			v = ctx->vregs[ref];
			if (v) {
				if (insn->op != IR_PHI) {
					ir_live_pos def_pos;
					ir_ref hint_ref = 0;
					ir_reg reg = constraints.def_reg;

					if (reg != IR_REG_NONE) {
						def_pos = IR_SAVE_LIVE_POS_FROM_REF(ref);
						if (insn->op == IR_PARAM || insn->op == IR_RLOAD) {
							/* parameter register must be kept before it's copied */
							ir_add_fixed_live_range(ctx, reg, IR_START_LIVE_POS_FROM_REF(bb->start), def_pos);
						}
					} else if (def_flags & IR_DEF_REUSES_OP1_REG) {
						if (!IR_IS_CONST_REF(insn->op1) && ctx->vregs[insn->op1]) {
							hint_ref = insn->op1;
						}
						if (def_flags & IR_DEF_CONFLICTS_WITH_INPUT_REGS) {
							def_pos = IR_USE_LIVE_POS_FROM_REF(ref);
						} else {
							def_pos = IR_LOAD_LIVE_POS_FROM_REF(ref);
						}
					} else if (def_flags & IR_DEF_CONFLICTS_WITH_INPUT_REGS) {
						def_pos = IR_LOAD_LIVE_POS_FROM_REF(ref);
					} else {
						if (insn->op == IR_PARAM) {
							/* We may reuse parameter stack slot for spilling */
							ctx->live_intervals[v]->flags |= IR_LIVE_INTERVAL_MEM_PARAM;
						} else if (insn->op == IR_VLOAD) {
							/* Load may be fused into the usage instruction */
							ctx->live_intervals[v]->flags |= IR_LIVE_INTERVAL_MEM_LOAD;
						}
						def_pos = IR_DEF_LIVE_POS_FROM_REF(ref);
					}
					/* intervals[opd].setFrom(op.id) */
					ival = ir_fix_live_range(ctx, v,
						IR_START_LIVE_POS_FROM_REF(bb->start), def_pos);
					ival->type = insn->type;
					ir_add_use(ctx, ival, 0, def_pos, reg, def_flags, hint_ref);
				} else {
					/* PHIs inputs must not be processed */
					ival = ctx->live_intervals[v];
					if (UNEXPECTED(!ival)) {
						/* Dead PHI */
						ival = ir_add_live_range(ctx, v, IR_DEF_LIVE_POS_FROM_REF(ref), IR_USE_LIVE_POS_FROM_REF(ref));
					}
					ival->type = insn->type;
					ir_add_use(ctx, ival, 0, IR_DEF_LIVE_POS_FROM_REF(ref), IR_REG_NONE, IR_USE_SHOULD_BE_IN_REG, 0);
					continue;
				}
			}

			IR_ASSERT(insn->op != IR_PHI && (!ctx->rules || !(ctx->rules[ref] & (IR_FUSED|IR_SKIPPED))));
			flags = ir_op_flags[insn->op];
			j = 1;
			p = insn->ops + 1;
			if (flags & (IR_OP_FLAG_CONTROL|IR_OP_FLAG_MEM|IR_OP_FLAG_PINNED)) {
				j++;
				p++;
			}
			for (; j <= insn->inputs_count; j++, p++) {
				ir_ref input = *p;
				ir_reg reg = (j < constraints.hints_count) ? constraints.hints[j] : IR_REG_NONE;
				ir_live_pos use_pos;
				ir_ref hint_ref = 0;
				uint32_t v;

				if (input > 0) {
					v = ctx->vregs[input];
					if (v) {
						use_pos = IR_USE_LIVE_POS_FROM_REF(ref);
						if (reg != IR_REG_NONE) {
							use_pos = IR_LOAD_LIVE_POS_FROM_REF(ref);
							ir_add_fixed_live_range(ctx, reg, use_pos, use_pos + IR_USE_SUB_REF);
						} else if (def_flags & IR_DEF_REUSES_OP1_REG) {
							if (j == 1) {
								if (def_flags & IR_DEF_CONFLICTS_WITH_INPUT_REGS) {
									use_pos = IR_USE_LIVE_POS_FROM_REF(ref);
								} else {
									use_pos = IR_LOAD_LIVE_POS_FROM_REF(ref);
								}
								IR_ASSERT(ctx->vregs[ref]);
								hint_ref = ref;
							} else if (input == insn->op1) {
								/* Input is the same as "op1" */
								use_pos = IR_LOAD_LIVE_POS_FROM_REF(ref);
							}
						}
						if (!IS_LIVE_IN_BLOCK(v, b)) {
							/* live.add(opd) */
							SET_LIVE_IN_BLOCK(v, b);
							/* intervals[opd].addRange(b.from, op.id) */
							ival = ir_add_live_range(ctx, v, IR_START_LIVE_POS_FROM_REF(bb->start), use_pos);
						} else {
							ival = ctx->live_intervals[v];
						}
						ir_add_use(ctx, ival, j, use_pos, reg, IR_USE_FLAGS(def_flags, j), hint_ref);
					} else if (ctx->rules) {
						if (ctx->rules[input] & IR_FUSED) {
						    ir_add_fusion_ranges(ctx, ref, input, bb, live_in_block, b);
						} else {
							if (ctx->rules[input] == (IR_SKIPPED|IR_RLOAD)) {
								ir_set_alocated_reg(ctx, ref, j, ctx->ir_base[input].op2);
							}
							if (reg != IR_REG_NONE) {
								use_pos = IR_LOAD_LIVE_POS_FROM_REF(ref);
								ir_add_fixed_live_range(ctx, reg, use_pos, use_pos + IR_USE_SUB_REF);
							}
						}
					}
				} else if (reg != IR_REG_NONE) {
					use_pos = IR_LOAD_LIVE_POS_FROM_REF(ref);
					ir_add_fixed_live_range(ctx, reg, use_pos, use_pos + IR_USE_SUB_REF);
				}
			}
		}
	}

	if (ctx->entries) {
		for (i = 0; i < ctx->entries_count; i++) {
			b = ctx->entries[i];
			bb = &ctx->cfg_blocks[b];
			IR_ASSERT(bb->predecessors_count == 1);
			ir_add_osr_entry_loads(ctx, bb, live_outs[ctx->cfg_blocks_count + 1 + i], &live_lists, b);
		}
		if (ctx->osr_entry_loads) {
			ir_list_push((ir_list*)ctx->osr_entry_loads, 0);
		}
	}

	ir_list_free(&live_lists);
	ir_mem_free(live_outs);
	ir_mem_free(live_in_block);

	return 1;
}

#endif

/* Live Ranges coalescing */

static ir_live_pos ir_ivals_overlap(ir_live_range *lrg1, ir_live_range *lrg2)
{
	while (1) {
		if (lrg2->start < lrg1->end) {
			if (lrg1->start < lrg2->end) {
				return IR_MAX(lrg1->start, lrg2->start);
			} else {
				lrg2 = lrg2->next;
				if (!lrg2) {
					return 0;
				}
			}
		} else {
			lrg1 = lrg1->next;
			if (!lrg1) {
				return 0;
			}
		}
	}
}

static ir_live_pos ir_vregs_overlap(ir_ctx *ctx, uint32_t r1, uint32_t r2)
{
	ir_live_interval *ival1 = ctx->live_intervals[r1];
	ir_live_interval *ival2 = ctx->live_intervals[r2];

#if 0
	if (ival2->range.start >= ival1->end
	 || ival1->range.start >= ival2->end) {
		return 0;
	}
#endif
	return ir_ivals_overlap(&ival1->range, &ival2->range);
}

static void ir_vregs_join(ir_ctx *ctx, uint32_t r1, uint32_t r2)
{
	ir_live_interval *ival = ctx->live_intervals[r2];
	ir_live_range *live_range = &ival->range;
	ir_live_range *next;
	ir_use_pos *use_pos, *next_pos, **prev;

#if 0
	fprintf(stderr, "COALESCE %d -> %d\n", r2, r1);
#endif

	ir_add_live_range(ctx, r1, live_range->start, live_range->end);
	live_range = live_range->next;
	while (live_range) {
		next = live_range->next;
		live_range->next = ctx->unused_ranges;
		ctx->unused_ranges = live_range;
		ir_add_live_range(ctx, r1, live_range->start, live_range->end);
		live_range = next;
	}

	/* merge sorted use_pos lists */
	prev = &ctx->live_intervals[r1]->use_pos;
	use_pos = ival->use_pos;
	while (use_pos) {
		if (use_pos->hint_ref > 0 && ctx->vregs[use_pos->hint_ref] == r1) {
			use_pos->hint_ref = 0;
		}
		while (*prev && ((*prev)->pos < use_pos->pos ||
			((*prev)->pos == use_pos->pos &&
				(use_pos->op_num == 0 || (*prev)->op_num < use_pos->op_num)))) {
			if ((*prev)->hint_ref > 0 && ctx->vregs[(*prev)->hint_ref] == r2) {
				(*prev)->hint_ref = 0;
			}
			prev = &(*prev)->next;
		}
		next_pos = use_pos->next;
		use_pos->next = *prev;
		*prev = use_pos;
		prev = &use_pos->next;
	    use_pos = next_pos;
	}
	use_pos = *prev;
	while (use_pos) {
		if (use_pos->hint_ref > 0 && ctx->vregs[use_pos->hint_ref] == r2) {
			use_pos->hint_ref = 0;
		}
		use_pos = use_pos->next;
	}

	ctx->live_intervals[r1]->flags |=
		IR_LIVE_INTERVAL_COALESCED | (ival->flags & (IR_LIVE_INTERVAL_HAS_HINT_REGS|IR_LIVE_INTERVAL_HAS_HINT_REFS));
	if (ctx->ir_base[IR_LIVE_POS_TO_REF(ctx->live_intervals[r1]->use_pos->pos)].op != IR_VLOAD) {
		ctx->live_intervals[r1]->flags &= ~IR_LIVE_INTERVAL_MEM_LOAD;
	}
	ctx->live_intervals[r2] = NULL;

	// TODO: remember to reuse ???
	//ir_mem_free(ival);
}

static bool ir_try_coalesce(ir_ctx *ctx, ir_ref from, ir_ref to)
{
	ir_ref i;
	uint32_t v1 = ctx->vregs[from];
	uint32_t v2 = ctx->vregs[to];

	if (v1 != v2 && !ir_vregs_overlap(ctx, v1, v2)) {
		uint16_t f1 = ctx->live_intervals[v1]->flags;
		uint16_t f2 = ctx->live_intervals[v2]->flags;

		if ((f1 & IR_LIVE_INTERVAL_COALESCED) && !(f2 & IR_LIVE_INTERVAL_COALESCED)) {
			ir_vregs_join(ctx, v1, v2);
			ctx->vregs[to] = v1;
		} else if ((f2 & IR_LIVE_INTERVAL_COALESCED) && !(f1 & IR_LIVE_INTERVAL_COALESCED)) {
			ir_vregs_join(ctx, v2, v1);
			ctx->vregs[from] = v2;
		} else if (from < to) {
			ir_vregs_join(ctx, v1, v2);
			if (f2 & IR_LIVE_INTERVAL_COALESCED) {
				for (i = 1; i < ctx->insns_count; i++) {
					if (ctx->vregs[i] == v2) {
						ctx->vregs[i] = v1;
					}
				}
			} else {
				ctx->vregs[to] = v1;
			}
		} else {
			ir_vregs_join(ctx, v2, v1);
			if (f1 & IR_LIVE_INTERVAL_COALESCED) {
				for (i = 1; i < ctx->insns_count; i++) {
					if (ctx->vregs[i] == v1) {
						ctx->vregs[i] = v2;
					}
				}
			} else {
				ctx->vregs[from] = v2;
			}
		}
		return 1;
	}
	return 0;
}

static void ir_add_phi_move(ir_ctx *ctx, uint32_t b, ir_ref from, ir_ref to)
{
	if (IR_IS_CONST_REF(from) || ctx->vregs[from] != ctx->vregs[to]) {
		ctx->cfg_blocks[b].flags &= ~IR_BB_EMPTY;
		ctx->cfg_blocks[b].flags |= IR_BB_DESSA_MOVES;
		ctx->flags2 |= IR_LR_HAVE_DESSA_MOVES;
#if 0
		fprintf(stderr, "BB%d: MOV %d -> %d\n", b, from, to);
#endif
	}
}

typedef struct _ir_coalesce_block {
	uint32_t b;
	uint32_t loop_depth;
} ir_coalesce_block;

static int ir_block_cmp(const void *b1, const void *b2)
{
	ir_coalesce_block *d1 = (ir_coalesce_block*)b1;
	ir_coalesce_block *d2 = (ir_coalesce_block*)b2;

	if (d1->loop_depth > d2->loop_depth) {
		return -1;
	} else if (d1->loop_depth == d2->loop_depth) {
		if (d1->b < d2->b) {
			return -1;
		} else {
			return 1;
		}
	} else {
		return 1;
	}
}

static void ir_swap_operands(ir_ctx *ctx, ir_ref i, ir_insn *insn)
{
	ir_live_pos pos = IR_USE_LIVE_POS_FROM_REF(i);
	ir_live_pos load_pos = IR_LOAD_LIVE_POS_FROM_REF(i);
	ir_live_interval *ival;
	ir_live_range *r;
	ir_use_pos *p, *p1 = NULL, *p2 = NULL;
	ir_ref tmp;

	tmp = insn->op1;
	insn->op1 = insn->op2;
	insn->op2 = tmp;

	ival = ctx->live_intervals[ctx->vregs[insn->op1]];
	p = ival->use_pos;
	while (p) {
		if (p->pos == pos) {
			p->pos = load_pos;
			p->op_num = 1;
			p1 = p;
			break;
		}
		p = p->next;
	}

	ival = ctx->live_intervals[ctx->vregs[i]];
	p = ival->use_pos;
	while (p) {
		if (p->pos == load_pos) {
			p->hint_ref = insn->op1;
			break;
		}
		p = p->next;
	}

	if (insn->op2 > 0 && ctx->vregs[insn->op2]) {
		ival = ctx->live_intervals[ctx->vregs[insn->op2]];
		r = &ival->range;
		while (r) {
			if (r->end == load_pos) {
				r->end = pos;
				if (!r->next) {
					ival->end = pos;
				}
				break;
			}
			r = r->next;
		}
		p = ival->use_pos;
		while (p) {
			if (p->pos == load_pos) {
				p->pos = pos;
				p->op_num = 2;
				p2 = p;
				break;
			}
			p = p->next;
		}
	}
	if (p1 && p2) {
		uint8_t tmp = p1->flags;
		p1->flags = p2->flags;
		p2->flags = tmp;
	}
}

static int ir_hint_conflict(ir_ctx *ctx, ir_ref ref, int use, int def)
{
	ir_use_pos *p;
	ir_reg r1 = IR_REG_NONE;
	ir_reg r2 = IR_REG_NONE;

	p = ctx->live_intervals[use]->use_pos;
	while (p) {
		if (IR_LIVE_POS_TO_REF(p->pos) == ref) {
			break;
		}
		if (p->hint != IR_REG_NONE) {
			r1 = p->hint;
		}
		p = p->next;
	}

	p = ctx->live_intervals[def]->use_pos;
	while (p) {
		if (IR_LIVE_POS_TO_REF(p->pos) > ref) {
			if (p->hint != IR_REG_NONE) {
				r2 = p->hint;
				break;
			}
		}
		p = p->next;
	}
	return r1 != r2 && r1 != IR_REG_NONE && r2 != IR_REG_NONE;
}

static int ir_try_swap_operands(ir_ctx *ctx, ir_ref i, ir_insn *insn)
{
	if (ctx->vregs[insn->op1]
	 && ctx->vregs[insn->op1] != ctx->vregs[i]
	 && !ir_vregs_overlap(ctx, ctx->vregs[insn->op1], ctx->vregs[i])
	 && !ir_hint_conflict(ctx, i, ctx->vregs[insn->op1], ctx->vregs[i])) {
		/* pass */
	} else {
		if (ctx->vregs[insn->op2] && ctx->vregs[insn->op2] != ctx->vregs[i]) {
			ir_live_pos pos = IR_USE_LIVE_POS_FROM_REF(i);
			ir_live_pos load_pos = IR_LOAD_LIVE_POS_FROM_REF(i);
			ir_live_interval *ival = ctx->live_intervals[ctx->vregs[insn->op2]];
			ir_live_range *r = &ival->range;

			if ((ival->flags & IR_LIVE_INTERVAL_MEM_PARAM) && ctx->use_lists[insn->op2].count == 1) {
				return 0;
			}
			while (r) {
				if (r->end == pos) {
					r->end = load_pos;
					if (!r->next) {
						ival->end = load_pos;
					}
					if (!ir_vregs_overlap(ctx, ctx->vregs[insn->op2], ctx->vregs[i])
					 && !ir_hint_conflict(ctx, i, ctx->vregs[insn->op2], ctx->vregs[i])) {
						ir_swap_operands(ctx, i, insn);
						return 1;
					} else {
						r->end = pos;
						if (!r->next) {
							ival->end = pos;
						}
					}
					break;
				}
				r = r->next;
			}
		}
	}
	return 0;
}

int ir_coalesce(ir_ctx *ctx)
{
	uint32_t b, n, succ, pred_b, count = 0;
	ir_ref *p, use, input, k, j;
	ir_block *bb, *succ_bb;
	ir_use_list *use_list;
	ir_insn *insn;
	ir_bitset visited;
	ir_coalesce_block *list;
	bool compact = 0;

	/* Collect a list of blocks which are predecossors to block with phi functions */
	list = ir_mem_malloc(sizeof(ir_coalesce_block) * ctx->cfg_blocks_count);
	visited = ir_bitset_malloc(ctx->cfg_blocks_count + 1);
	for (b = 1, bb = &ctx->cfg_blocks[1]; b <= ctx->cfg_blocks_count; b++, bb++) {
		IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));
		if (bb->flags & IR_BB_HAS_PHI) {
			k = bb->predecessors_count;
			if (k > 1) {
				use_list = &ctx->use_lists[bb->start];
				n = use_list->count;
				IR_ASSERT(k == ctx->ir_base[bb->start].inputs_count);
				for (p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
					use = *p;
					insn = &ctx->ir_base[use];
					if (insn->op == IR_PHI) {
						do {
							k--;
							pred_b = ctx->cfg_edges[bb->predecessors + k];
							if (!ir_bitset_in(visited, pred_b)) {
								ir_bitset_incl(visited, pred_b);
								list[count].b = pred_b;
								list[count].loop_depth = ctx->cfg_blocks[pred_b].loop_depth;
								count++;
							}
						} while (k > 0);
						break;
					}
				}
			}
		}
	}
	ir_mem_free(visited);

	/* Sort blocks according to their loop depth */
	qsort(list, count, sizeof(ir_coalesce_block), ir_block_cmp);

	while (count > 0) {
		uint32_t i;

		count--;
		b = list[count].b;
		bb = &ctx->cfg_blocks[b];
		IR_ASSERT(bb->successors_count == 1);
		succ = ctx->cfg_edges[bb->successors];
		succ_bb = &ctx->cfg_blocks[succ];
		IR_ASSERT(succ_bb->predecessors_count > 1);
		k = ir_phi_input_number(ctx, succ_bb, b);
		use_list = &ctx->use_lists[succ_bb->start];
		n = use_list->count;
		for (i = 0, p = &ctx->use_edges[use_list->refs]; i < n; i++, p++) {
			use = *p;
			insn = &ctx->ir_base[use];
			if (insn->op == IR_PHI) {
				input = ir_insn_op(insn, k);
				if (input > 0) {
					if (!ir_try_coalesce(ctx, input, use)) {
						ir_add_phi_move(ctx, b, input, use);
					} else {
						compact = 1;
					}
				} else {
					/* Move for constant input */
					ir_add_phi_move(ctx, b, input, use);
				}
			}
		}
	}
	ir_mem_free(list);

	ir_hint_propagation(ctx);

	if (ctx->rules) {
		/* try to swap operands of commutative instructions for better register allocation */
		for (b = 1, bb = &ctx->cfg_blocks[1]; b <= ctx->cfg_blocks_count; b++, bb++) {
			ir_ref i;

			IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));
			i = bb->end;

			/* skip last instruction */
			i = ctx->prev_ref[i];

			while (i != bb->start) {
				insn = &ctx->ir_base[i];
				if ((ir_op_flags[insn->op] & IR_OP_FLAG_COMMUTATIVE)
				 && ctx->vregs[i]
				 && ctx->live_intervals[ctx->vregs[i]]->use_pos
				 && (ctx->live_intervals[ctx->vregs[i]]->use_pos->flags & IR_DEF_REUSES_OP1_REG)
				 && insn->op2 > 0
				 && insn->op1 > 0
				 && insn->op1 != insn->op2) {
					ir_try_swap_operands(ctx, i, insn);
				}
				i = ctx->prev_ref[i];
			}
		}
	}

	if (compact) {
		ir_ref i, n;
		uint32_t *xlat = ir_mem_malloc((ctx->vregs_count + 1) * sizeof(uint32_t));

		for (i = 1, n = 1; i <= ctx->vregs_count; i++) {
			if (ctx->live_intervals[i]) {
				xlat[i] = n;
				if (i != n) {
					ctx->live_intervals[n] = ctx->live_intervals[i];
					ctx->live_intervals[n]->vreg = n;
				}
				n++;
			}
		}
		n--;
		if (n != ctx->vregs_count) {
			j = ctx->vregs_count - n;
			/* vregs + tmp + fixed + SRATCH + ALL */
			for (i = n + 1; i <= n + IR_REG_NUM + 2; i++) {
				ctx->live_intervals[i] = ctx->live_intervals[i + j];
				if (ctx->live_intervals[i]) {
					ctx->live_intervals[i]->vreg = i;
				}
			}
			for (j = 1; j < ctx->insns_count; j++) {
				if (ctx->vregs[j]) {
					ctx->vregs[j] = xlat[ctx->vregs[j]];
				}
			}
			ctx->vregs_count = n;
		}
		ir_mem_free(xlat);
	}

	return 1;
}

/* SSA Deconstruction */

int ir_compute_dessa_moves(ir_ctx *ctx)
{
	uint32_t b, i, n;
	ir_ref j, k, *p, use;
	ir_block *bb;
	ir_use_list *use_list;
	ir_insn *insn;

	for (b = 1, bb = &ctx->cfg_blocks[1]; b <= ctx->cfg_blocks_count; b++, bb++) {
		IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));
		k = bb->predecessors_count;
		if (k > 1) {
			use_list = &ctx->use_lists[bb->start];
			n = use_list->count;
			if (n > 1) {
				IR_ASSERT(k == ctx->ir_base[bb->start].inputs_count);
				k++;
				for (i = 0, p = &ctx->use_edges[use_list->refs]; i < n; i++, p++) {
					use = *p;
					insn = &ctx->ir_base[use];
					if (insn->op == IR_PHI) {
						for (j = 2; j <= k; j++) {
							if (IR_IS_CONST_REF(ir_insn_op(insn, j)) || ctx->vregs[ir_insn_op(insn, j)] != ctx->vregs[use]) {
								int pred = ctx->cfg_edges[bb->predecessors + (j-2)];
								ctx->cfg_blocks[pred].flags &= ~IR_BB_EMPTY;
								ctx->cfg_blocks[pred].flags |= IR_BB_DESSA_MOVES;
								ctx->flags2 |= IR_LR_HAVE_DESSA_MOVES;
							}
						}
					}
				}
			}
		}
	}
	return 1;
}

int ir_gen_dessa_moves(ir_ctx *ctx, uint32_t b, emit_copy_t emit_copy)
{
	uint32_t succ, k, n = 0;
	ir_block *bb, *succ_bb;
	ir_use_list *use_list;
	ir_ref *loc, *pred, i, *p, ref, input;
	ir_insn *insn;
	uint32_t len;
	ir_bitset todo, ready;
	bool have_constants = 0;

	bb = &ctx->cfg_blocks[b];
	if (!(bb->flags & IR_BB_DESSA_MOVES)) {
		return 0;
	}
	IR_ASSERT(bb->successors_count == 1);
	succ = ctx->cfg_edges[bb->successors];
	succ_bb = &ctx->cfg_blocks[succ];
	IR_ASSERT(succ_bb->predecessors_count > 1);
	use_list = &ctx->use_lists[succ_bb->start];

	k = ir_phi_input_number(ctx, succ_bb, b);

	loc = ir_mem_malloc(ctx->insns_count * 2 * sizeof(ir_ref));
	pred = loc + ctx->insns_count;
	len = ir_bitset_len(ctx->insns_count);
	todo = ir_bitset_malloc(ctx->insns_count);

	for (i = 0, p = &ctx->use_edges[use_list->refs]; i < use_list->count; i++, p++) {
		ref = *p;
		insn = &ctx->ir_base[ref];
		if (insn->op == IR_PHI) {
			input = ir_insn_op(insn, k);
			if (IR_IS_CONST_REF(input)) {
				have_constants = 1;
			} else if (ctx->vregs[input] != ctx->vregs[ref]) {
				loc[ref] = pred[input] = 0;
				ir_bitset_incl(todo, ref);
				n++;
			}
		}
	}

	if (n > 0) {
		ready = ir_bitset_malloc(ctx->insns_count);
		IR_BITSET_FOREACH(todo, len, ref) {
			insn = &ctx->ir_base[ref];
			IR_ASSERT(insn->op == IR_PHI);
			input = ir_insn_op(insn, k);
			loc[input] = input;
			pred[ref] = input;
		} IR_BITSET_FOREACH_END();

		IR_BITSET_FOREACH(todo, len, i) {
			if (!loc[i]) {
				ir_bitset_incl(ready, i);
			}
		} IR_BITSET_FOREACH_END();

		while (1) {
			ir_ref a, b, c;

			while ((b = ir_bitset_pop_first(ready, len)) >= 0) {
				a = pred[b];
				c = loc[a];
				emit_copy(ctx, ctx->ir_base[b].type, c, b);
				ir_bitset_excl(todo, b);
				loc[a] = b;
				if (a == c && pred[a]) {
					ir_bitset_incl(ready, a);
				}
			}
			b = ir_bitset_pop_first(todo, len);
			if (b < 0) {
				break;
			}
			IR_ASSERT(b != loc[pred[b]]);
			emit_copy(ctx, ctx->ir_base[b].type, b, 0);
			loc[b] = 0;
			ir_bitset_incl(ready, b);
		}

		ir_mem_free(ready);
	}

	ir_mem_free(todo);
	ir_mem_free(loc);

	if (have_constants) {
		for (i = 0, p = &ctx->use_edges[use_list->refs]; i < use_list->count; i++, p++) {
			ref = *p;
			insn = &ctx->ir_base[ref];
			if (insn->op == IR_PHI) {
				input = ir_insn_op(insn, k);
				if (IR_IS_CONST_REF(input)) {
					emit_copy(ctx, insn->type, input, ref);
				}
			}
		}
	}

	return 1;
}

/* Linear Scan Register Allocation */

#ifdef IR_DEBUG
# define IR_LOG_LSRA(action, ival, comment) do { \
		if (ctx->flags & IR_DEBUG_RA) { \
			ir_live_interval *_ival = (ival); \
			ir_live_pos _start = _ival->range.start; \
			ir_live_pos _end = _ival->end; \
			fprintf(stderr, action " R%d [%d.%d...%d.%d)" comment "\n", \
				(_ival->flags & IR_LIVE_INTERVAL_TEMP) ? 0 : _ival->vreg, \
				IR_LIVE_POS_TO_REF(_start), IR_LIVE_POS_TO_SUB_REF(_start), \
				IR_LIVE_POS_TO_REF(_end), IR_LIVE_POS_TO_SUB_REF(_end)); \
		} \
	} while (0)
# define IR_LOG_LSRA_ASSIGN(action, ival, comment) do { \
		if (ctx->flags & IR_DEBUG_RA) { \
			ir_live_interval *_ival = (ival); \
			ir_live_pos _start = _ival->range.start; \
			ir_live_pos _end = _ival->end; \
			fprintf(stderr, action " R%d [%d.%d...%d.%d) to %s" comment "\n", \
				(_ival->flags & IR_LIVE_INTERVAL_TEMP) ? 0 : _ival->vreg, \
				IR_LIVE_POS_TO_REF(_start), IR_LIVE_POS_TO_SUB_REF(_start), \
				IR_LIVE_POS_TO_REF(_end), IR_LIVE_POS_TO_SUB_REF(_end), \
				ir_reg_name(_ival->reg, _ival->type)); \
		} \
	} while (0)
# define IR_LOG_LSRA_SPLIT(ival, pos) do { \
		if (ctx->flags & IR_DEBUG_RA) { \
			ir_live_interval *_ival = (ival); \
			ir_live_pos _start = _ival->range.start; \
			ir_live_pos _end = _ival->end; \
			ir_live_pos _pos = (pos); \
			fprintf(stderr, "      ---- Split R%d [%d.%d...%d.%d) at %d.%d\n", \
				(_ival->flags & IR_LIVE_INTERVAL_TEMP) ? 0 : _ival->vreg, \
				IR_LIVE_POS_TO_REF(_start), IR_LIVE_POS_TO_SUB_REF(_start), \
				IR_LIVE_POS_TO_REF(_end), IR_LIVE_POS_TO_SUB_REF(_end), \
				IR_LIVE_POS_TO_REF(_pos), IR_LIVE_POS_TO_SUB_REF(_pos)); \
		} \
	} while (0)
# define IR_LOG_LSRA_CONFLICT(action, ival, pos) do { \
		if (ctx->flags & IR_DEBUG_RA) { \
			ir_live_interval *_ival = (ival); \
			ir_live_pos _start = _ival->range.start; \
			ir_live_pos _end = _ival->end; \
			ir_live_pos _pos = (pos); \
			fprintf(stderr, action " R%d [%d.%d...%d.%d) assigned to %s at %d.%d\n", \
				(_ival->flags & IR_LIVE_INTERVAL_TEMP) ? 0 : _ival->vreg, \
				IR_LIVE_POS_TO_REF(_start), IR_LIVE_POS_TO_SUB_REF(_start), \
				IR_LIVE_POS_TO_REF(_end), IR_LIVE_POS_TO_SUB_REF(_end), \
				ir_reg_name(_ival->reg, _ival->type), \
				IR_LIVE_POS_TO_REF(_pos), IR_LIVE_POS_TO_SUB_REF(_pos)); \
		} \
	} while (0)
#else
# define IR_LOG_LSRA(action, ival, comment)
# define IR_LOG_LSRA_ASSIGN(action, ival, comment)
# define IR_LOG_LSRA_SPLIT(ival, pos)
# define IR_LOG_LSRA_CONFLICT(action, ival, pos);
#endif

static bool ir_ival_covers(ir_live_interval *ival, ir_live_pos position)
{
	ir_live_range *live_range = &ival->range;

	do {
		if (position < live_range->end) {
			return position >= live_range->start;
		}
		live_range = live_range->next;
	} while (live_range);

	return 0;
}

static bool ir_ival_has_hole_between(ir_live_interval *ival, ir_live_pos from, ir_live_pos to)
{
	ir_live_range *r = &ival->range;

	while (r) {
		if (from < r->start) {
			return 1;
		} else if (to <= r->end) {
			return 0;
		}
		r = r->next;
	}
	return 0;
}


static ir_live_pos ir_last_use_pos_before(ir_live_interval *ival, ir_live_pos pos, uint8_t flags)
{
	ir_live_pos ret = 0;
	ir_use_pos *p = ival->use_pos;

	while (p && p->pos <= pos) {
		if (p->flags & flags) {
			ret = p->pos;
		}
		p = p->next;
	}
	return ret;
}

static ir_live_pos ir_first_use_pos_after(ir_live_interval *ival, ir_live_pos pos, uint8_t flags)
{
	ir_use_pos *p = ival->use_pos;

	while (p && p->pos <= pos) {
		p = p->next;
	}
	while (p && !(p->flags & flags)) {
		p = p->next;
	}
	return p ? p->pos : 0x7fffffff;
}

static ir_live_pos ir_first_use_pos(ir_live_interval *ival, uint8_t flags)
{
	ir_use_pos *p = ival->use_pos;

	while (p && !(p->flags & flags)) {
		p = p->next;
	}
	return p ? p->pos : 0x7fffffff;
}

static ir_block *ir_block_from_live_pos(ir_ctx *ctx, ir_live_pos pos)
{
	ir_ref ref = IR_LIVE_POS_TO_REF(pos);
	uint32_t b = ctx->cfg_map[ref];

	while (!b) {
		ref--;
		IR_ASSERT(ref > 0);
		b = ctx->cfg_map[ref];
	}
	IR_ASSERT(b <= ctx->cfg_blocks_count);
	return &ctx->cfg_blocks[b];
}

static ir_live_pos ir_find_optimal_split_position(ir_ctx *ctx, ir_live_interval *ival, ir_live_pos min_pos, ir_live_pos max_pos, bool prefer_max)
{
	ir_block *min_bb, *max_bb;

	if (min_pos == max_pos) {
		return max_pos;
	}

	IR_ASSERT(min_pos < max_pos);
	IR_ASSERT(min_pos >= ival->range.start);
	IR_ASSERT(max_pos < ival->end);

	min_bb = ir_block_from_live_pos(ctx, min_pos);
	max_bb = ir_block_from_live_pos(ctx, max_pos);

	if (min_bb == max_bb
	 || ir_ival_has_hole_between(ival, min_pos, max_pos)) {  // TODO: ???
		return (prefer_max) ? max_pos : min_pos;
	}

	if (max_bb->loop_depth > 0) {
		/* Split at the end of the loop entry */
		do {
			ir_block *bb;

			if (max_bb->flags & IR_BB_LOOP_HEADER) {
				bb = max_bb;
			} else {
				IR_ASSERT(max_bb->loop_header);
				bb = &ctx->cfg_blocks[max_bb->loop_header];
			}
			bb = &ctx->cfg_blocks[bb->idom];
			if (IR_DEF_LIVE_POS_FROM_REF(bb->end) < min_pos) {
				break;
			}
			max_bb = bb;
		} while (max_bb->loop_depth > 0);

		if (IR_DEF_LIVE_POS_FROM_REF(max_bb->end) < max_pos) {
			return IR_DEF_LIVE_POS_FROM_REF(max_bb->end);
		}
	}

	if (IR_LOAD_LIVE_POS_FROM_REF(max_bb->start) > min_pos) {
		return IR_LOAD_LIVE_POS_FROM_REF(max_bb->start);
	} else {
		// TODO: "min_bb" is in a deeper loop than "max_bb" ???
		return max_pos;
	}
}

static ir_live_interval *ir_split_interval_at(ir_ctx *ctx, ir_live_interval *ival, ir_live_pos pos)
{
	ir_live_interval *child;
	ir_live_range *p, *prev;
	ir_use_pos *use_pos, *prev_use_pos;

	IR_LOG_LSRA_SPLIT(ival, pos);
	IR_ASSERT(pos > ival->range.start);
	ctx->flags2 |= IR_RA_HAVE_SPLITS;

	p = &ival->range;
	prev = NULL;
	while (p && pos >= p->end) {
		prev = p;
		p = prev->next;
	}
	IR_ASSERT(p);

	if (pos < p->start) {
		/* split between ranges */
		pos = p->start;
	}

	use_pos = ival->use_pos;
	prev_use_pos = NULL;

	ival->flags &= ~(IR_LIVE_INTERVAL_HAS_HINT_REGS|IR_LIVE_INTERVAL_HAS_HINT_REFS);
	if (p->start == pos) {
		while (use_pos && pos > use_pos->pos) {
			if (use_pos->hint != IR_REG_NONE) {
				ival->flags |= IR_LIVE_INTERVAL_HAS_HINT_REGS;
			}
			if (use_pos->hint_ref > 0) {
				ival->flags |= IR_LIVE_INTERVAL_HAS_HINT_REFS;
			}
			prev_use_pos = use_pos;
			use_pos = use_pos->next;
		}
	} else {
		while (use_pos && pos >= use_pos->pos) {
			if (use_pos->hint != IR_REG_NONE) {
				ival->flags |= IR_LIVE_INTERVAL_HAS_HINT_REGS;
			}
			if (use_pos->hint_ref > 0) {
				ival->flags |= IR_LIVE_INTERVAL_HAS_HINT_REFS;
			}
			prev_use_pos = use_pos;
			use_pos = use_pos->next;
		}
	}

	child = ir_arena_alloc(&ctx->arena, sizeof(ir_live_interval));
	child->type = ival->type;
	child->reg = IR_REG_NONE;
	child->flags = IR_LIVE_INTERVAL_SPLIT_CHILD;
	child->vreg = ival->vreg;
	child->stack_spill_pos = -1; // not allocated
	child->range.start = pos;
	child->range.end = p->end;
	child->range.next = p->next;
	child->end = ival->end;
	child->use_pos = prev_use_pos ? prev_use_pos->next : use_pos;

	child->next = ival->next;
	ival->next = child;

	if (pos == p->start) {
		prev->next = NULL;
		ival->end = prev->end;
		/* Cache to reuse */
		p->next = ctx->unused_ranges;
		ctx->unused_ranges = p;
	} else {
		p->end = ival->end = pos;
		p->next = NULL;
	}
	if (prev_use_pos) {
		prev_use_pos->next = NULL;
	} else {
		ival->use_pos = NULL;
	}

	use_pos = child->use_pos;
	while (use_pos) {
		if (use_pos->hint != IR_REG_NONE) {
			child->flags |= IR_LIVE_INTERVAL_HAS_HINT_REGS;
		}
		if (use_pos->hint_ref > 0) {
			child->flags |= IR_LIVE_INTERVAL_HAS_HINT_REFS;
		}
		use_pos = use_pos->next;
	}

	return child;
}

int32_t ir_allocate_spill_slot(ir_ctx *ctx, ir_type type, ir_reg_alloc_data *data)
{
	int32_t ret;
	uint8_t size = ir_type_size[type];

	IR_ASSERT(size == 1 || size == 2 || size == 4 || size == 8);
	if (data->handled && data->handled[size]) {
		ret = data->handled[size]->stack_spill_pos;
		data->handled[size] = data->handled[size]->list_next;
	} else if (size == 8) {
		ret = ctx->stack_frame_size;
		ctx->stack_frame_size += 8;
	} else if (size == 4) {
		if (data->unused_slot_4) {
			ret = data->unused_slot_4;
			data->unused_slot_4 = 0;
	    } else if (data->handled && data->handled[8]) {
			ret = data->handled[8]->stack_spill_pos;
			data->handled[8] = data->handled[8]->list_next;
			data->unused_slot_4 = ret + 4;
		} else {
			ret = ctx->stack_frame_size;
			if (sizeof(void*) == 8) {
				data->unused_slot_4 = ctx->stack_frame_size + 4;
				ctx->stack_frame_size += 8;
			} else {
				ctx->stack_frame_size += 4;
			}
		}
	} else if (size == 2) {
		if (data->unused_slot_2) {
			ret = data->unused_slot_2;
			data->unused_slot_2 = 0;
		} else if (data->unused_slot_4) {
			ret = data->unused_slot_4;
			data->unused_slot_2 = data->unused_slot_4 + 2;
			data->unused_slot_4 = 0;
	    } else if (data->handled && data->handled[4]) {
			ret = data->handled[4]->stack_spill_pos;
			data->handled[4] = data->handled[4]->list_next;
			data->unused_slot_2 = ret + 2;
	    } else if (data->handled && data->handled[8]) {
			ret = data->handled[8]->stack_spill_pos;
			data->handled[8] = data->handled[8]->list_next;
			data->unused_slot_2 = ret + 2;
			data->unused_slot_4 = ret + 4;
		} else {
			ret = ctx->stack_frame_size;
			data->unused_slot_2 = ctx->stack_frame_size + 2;
			if (sizeof(void*) == 8) {
				data->unused_slot_4 = ctx->stack_frame_size + 4;
				ctx->stack_frame_size += 8;
			} else {
				ctx->stack_frame_size += 4;
			}
		}
	} else {
		IR_ASSERT(size == 1);
		if (data->unused_slot_1) {
			ret = data->unused_slot_1;
			data->unused_slot_1 = 0;
		} else if (data->unused_slot_2) {
			ret = data->unused_slot_2;
			data->unused_slot_1 = data->unused_slot_2 + 1;
			data->unused_slot_2 = 0;
		} else if (data->unused_slot_4) {
			ret = data->unused_slot_4;
			data->unused_slot_1 = data->unused_slot_4 + 1;
			data->unused_slot_2 = data->unused_slot_4 + 2;
			data->unused_slot_4 = 0;
	    } else if (data->handled && data->handled[2]) {
			ret = data->handled[2]->stack_spill_pos;
			data->handled[2] = data->handled[2]->list_next;
			data->unused_slot_1 = ret + 1;
	    } else if (data->handled && data->handled[4]) {
			ret = data->handled[4]->stack_spill_pos;
			data->handled[4] = data->handled[4]->list_next;
			data->unused_slot_1 = ret + 1;
			data->unused_slot_2 = ret + 2;
	    } else if (data->handled && data->handled[8]) {
			ret = data->handled[8]->stack_spill_pos;
			data->handled[8] = data->handled[8]->list_next;
			data->unused_slot_1 = ret + 1;
			data->unused_slot_2 = ret + 2;
			data->unused_slot_4 = ret + 4;
		} else {
			ret = ctx->stack_frame_size;
			data->unused_slot_1 = ctx->stack_frame_size + 1;
			data->unused_slot_2 = ctx->stack_frame_size + 2;
			if (sizeof(void*) == 8) {
				data->unused_slot_4 = ctx->stack_frame_size + 4;
				ctx->stack_frame_size += 8;
			} else {
				ctx->stack_frame_size += 4;
			}
		}
	}
	return ret;
}

static ir_reg ir_get_first_reg_hint(ir_ctx *ctx, ir_live_interval *ival, ir_regset available)
{
	ir_use_pos *use_pos;
	ir_reg reg;

	use_pos = ival->use_pos;
	while (use_pos) {
		reg = use_pos->hint;
		if (reg >= 0 && IR_REGSET_IN(available, reg)) {
			return reg;
		}
		use_pos = use_pos->next;
	}

	return IR_REG_NONE;
}

static ir_reg ir_try_allocate_preferred_reg(ir_ctx *ctx, ir_live_interval *ival, ir_regset available, ir_live_pos *freeUntilPos)
{
	ir_use_pos *use_pos;
	ir_reg reg;

	if (ival->flags & IR_LIVE_INTERVAL_HAS_HINT_REGS) {
		use_pos = ival->use_pos;
		while (use_pos) {
			reg = use_pos->hint;
			if (reg >= 0 && IR_REGSET_IN(available, reg)) {
				if (ival->end <= freeUntilPos[reg]) {
					/* register available for the whole interval */
					return reg;
				}
			}
			use_pos = use_pos->next;
		}
	}

	if (ival->flags & IR_LIVE_INTERVAL_HAS_HINT_REFS) {
		use_pos = ival->use_pos;
		while (use_pos) {
			if (use_pos->hint_ref > 0) {
				reg = ctx->live_intervals[ctx->vregs[use_pos->hint_ref]]->reg;
				if (reg >= 0 && IR_REGSET_IN(available, reg)) {
					if (ival->end <= freeUntilPos[reg]) {
						/* register available for the whole interval */
						return reg;
					}
				}
			}
			use_pos = use_pos->next;
		}
	}

	return IR_REG_NONE;
}

static ir_reg ir_get_preferred_reg(ir_ctx *ctx, ir_live_interval *ival, ir_regset available)
{
	ir_use_pos *use_pos;
	ir_reg reg;

	use_pos = ival->use_pos;
	while (use_pos) {
		reg = use_pos->hint;
		if (reg >= 0 && IR_REGSET_IN(available, reg)) {
			return reg;
		} else if (use_pos->hint_ref > 0) {
			reg = ctx->live_intervals[ctx->vregs[use_pos->hint_ref]]->reg;
			if (reg >= 0 && IR_REGSET_IN(available, reg)) {
				return reg;
			}
		}
		use_pos = use_pos->next;
	}

	return IR_REG_NONE;
}

static void ir_add_to_unhandled(ir_live_interval **unhandled, ir_live_interval *ival)
{
	ir_live_pos pos = ival->range.start;

	if (*unhandled == NULL
	 || pos < (*unhandled)->range.start
	 || (pos == (*unhandled)->range.start
	  && (ival->flags & (IR_LIVE_INTERVAL_HAS_HINT_REGS|IR_LIVE_INTERVAL_HAS_HINT_REFS))
	  && !((*unhandled)->flags & (IR_LIVE_INTERVAL_HAS_HINT_REGS|IR_LIVE_INTERVAL_HAS_HINT_REFS)))
	 || (pos == (*unhandled)->range.start
	  && ival->vreg > (*unhandled)->vreg)) {
		ival->list_next = *unhandled;
		*unhandled = ival;
	} else {
		ir_live_interval *prev = *unhandled;

		while (prev->list_next) {
			if (pos < prev->list_next->range.start
			 || (pos == prev->list_next->range.start
			  && (ival->flags & (IR_LIVE_INTERVAL_HAS_HINT_REGS|IR_LIVE_INTERVAL_HAS_HINT_REFS))
			  && !(prev->list_next->flags & (IR_LIVE_INTERVAL_HAS_HINT_REGS|IR_LIVE_INTERVAL_HAS_HINT_REFS)))
			 || (pos == prev->list_next->range.start
			  && ival->vreg > prev->list_next->vreg)) {
				break;
			}
			prev = prev->list_next;
		}
		ival->list_next = prev->list_next;
		prev->list_next = ival;
	}
}

/* merge sorted lists */
static void ir_merge_to_unhandled(ir_live_interval **unhandled, ir_live_interval *ival)
{
	ir_live_interval **prev;
	ir_live_pos pos;

	if (*unhandled == NULL) {
		*unhandled = ival;
		while (ival) {
			ival = ival->list_next = ival->next;
		}
	} else {
		prev = unhandled;
		while (ival) {
			pos = ival->range.start;
			while (*prev && pos >= (*prev)->range.start) {
				prev = &(*prev)->list_next;
			}
			ival->list_next = *prev;
			*prev = ival;
			prev = &ival->list_next;
			ival = ival->next;
		}
	}
#if IR_DEBUG
	ival = *unhandled;
	pos = 0;

	while (ival) {
		IR_ASSERT(ival->range.start >= pos);
		pos = ival->range.start;
		ival = ival->list_next;
	}
#endif
}

static void ir_add_to_unhandled_spill(ir_live_interval **unhandled, ir_live_interval *ival)
{
	ir_live_pos pos = ival->range.start;

	if (*unhandled == NULL
	 || pos <= (*unhandled)->range.start) {
		ival->list_next = *unhandled;
		*unhandled = ival;
	} else {
		ir_live_interval *prev = *unhandled;

		while (prev->list_next) {
			if (pos <= prev->list_next->range.start) {
				break;
			}
			prev = prev->list_next;
		}
		ival->list_next = prev->list_next;
		prev->list_next = ival;
	}
}

static ir_reg ir_try_allocate_free_reg(ir_ctx *ctx, ir_live_interval *ival, ir_live_interval **active, ir_live_interval *inactive, ir_live_interval **unhandled)
{
	ir_live_pos freeUntilPos[IR_REG_NUM];
	int i, reg;
	ir_live_pos pos, next;
	ir_live_interval *other;
	ir_regset available, overlapped, scratch;

	if (IR_IS_TYPE_FP(ival->type)) {
		available = IR_REGSET_FP;
		/* set freeUntilPos of all physical registers to maxInt */
		for (i = IR_REG_FP_FIRST; i <= IR_REG_FP_LAST; i++) {
			freeUntilPos[i] = 0x7fffffff;
		}
	} else {
		available = IR_REGSET_GP;
		if (ctx->flags & IR_USE_FRAME_POINTER) {
			IR_REGSET_EXCL(available, IR_REG_FRAME_POINTER);
		}
#if defined(IR_TARGET_X86)
		if (ir_type_size[ival->type] == 1) {
			/* TODO: if no registers avialivle, we may use of one this register for already allocated interval ??? */
			IR_REGSET_EXCL(available, IR_REG_RBP);
			IR_REGSET_EXCL(available, IR_REG_RSI);
			IR_REGSET_EXCL(available, IR_REG_RDI);
		}
#endif
		/* set freeUntilPos of all physical registers to maxInt */
		for (i = IR_REG_GP_FIRST; i <= IR_REG_GP_LAST; i++) {
			freeUntilPos[i] = 0x7fffffff;
		}
	}

	available = IR_REGSET_DIFFERENCE(available, (ir_regset)ctx->fixed_regset);

	/* for each interval it in active */
	other = *active;
	while (other) {
		/* freeUntilPos[it.reg] = 0 */
		reg = other->reg;
		IR_ASSERT(reg >= 0);
		if (reg >= IR_REG_SCRATCH) {
			if (reg == IR_REG_SCRATCH) {
				available = IR_REGSET_DIFFERENCE(available, IR_REGSET_SCRATCH);
			} else {
				IR_ASSERT(reg == IR_REG_ALL);
				available = IR_REGSET_EMPTY;
			}
		} else {
			IR_REGSET_EXCL(available, reg);
		}
		other = other->list_next;
	}

	/* for each interval it in inactive intersecting with current
	 *
	 * This loop is not necessary for program in SSA form (see LSRA on SSA fig. 6),
	 * but it is still necessary after coalescing and splitting
	 */
	overlapped = IR_REGSET_EMPTY;
	other = inactive;
	pos = ival->end;
	while (other) {
		/* freeUntilPos[it.reg] = next intersection of it with current */
		if (other->current_range->start < pos) {
			next = ir_ivals_overlap(&ival->range, other->current_range);
			if (next) {
				reg = other->reg;
				IR_ASSERT(reg >= 0);
				if (reg >= IR_REG_SCRATCH) {
					ir_regset regset;

					if (reg == IR_REG_SCRATCH) {
						regset = IR_REGSET_INTERSECTION(available, IR_REGSET_SCRATCH);
					} else {
						IR_ASSERT(reg == IR_REG_ALL);
						regset = available;
					}
					overlapped = IR_REGSET_UNION(overlapped, regset);
					IR_REGSET_FOREACH(regset, reg) {
						if (next < freeUntilPos[reg]) {
							freeUntilPos[reg] = next;
						}
					} IR_REGSET_FOREACH_END();
				} else if (IR_REGSET_IN(available, reg)) {
					IR_REGSET_INCL(overlapped, reg);
					if (next < freeUntilPos[reg]) {
						freeUntilPos[reg] = next;
					}
				}
			}
		}
		other = other->list_next;
	}

	available = IR_REGSET_DIFFERENCE(available, overlapped);
	if (available != IR_REGSET_EMPTY) {

		if (ival->flags & (IR_LIVE_INTERVAL_HAS_HINT_REGS|IR_LIVE_INTERVAL_HAS_HINT_REFS)) {
			/* Try to use hint */
			reg = ir_try_allocate_preferred_reg(ctx, ival, available, freeUntilPos);
			if (reg != IR_REG_NONE) {
				ival->reg = reg;
				IR_LOG_LSRA_ASSIGN("    ---- Assign", ival, " (hint available without spilling)");
				if (*unhandled && ival->end > (*unhandled)->range.start) {
					ival->list_next = *active;
					*active = ival;
				}
				return reg;
			}
		}

		if (ival->flags & IR_LIVE_INTERVAL_SPLIT_CHILD) {
			/* Try to reuse the register previously allocated for splited interval */
			reg = ctx->live_intervals[ival->vreg]->reg;
			if (reg >= 0 && IR_REGSET_IN(available, reg)) {
				ival->reg = reg;
				IR_LOG_LSRA_ASSIGN("    ---- Assign", ival, " (available without spilling)");
				if (*unhandled && ival->end > (*unhandled)->range.start) {
					ival->list_next = *active;
					*active = ival;
				}
				return reg;
			}
		}

		/* prefer caller-saved registers to avoid save/restore in prologue/epilogue */
		scratch = IR_REGSET_INTERSECTION(available, IR_REGSET_SCRATCH);
		if (scratch != IR_REGSET_EMPTY) {
			/* prefer registers that don't conflict with the hints for the following unhandled intervals */
			if (1) {
				ir_regset non_conflicting = scratch;

				other = *unhandled;
				while (other && other->range.start < ival->range.end) {
					if (other->flags & IR_LIVE_INTERVAL_HAS_HINT_REGS) {
						reg = ir_get_first_reg_hint(ctx, other, non_conflicting);

						if (reg >= 0) {
							IR_REGSET_EXCL(non_conflicting, reg);
							if (non_conflicting == IR_REGSET_EMPTY) {
								break;
							}
						}
					}
					other = other->list_next;
				}
				if (non_conflicting != IR_REGSET_EMPTY) {
					reg = IR_REGSET_FIRST(non_conflicting);
				} else {
					reg = IR_REGSET_FIRST(scratch);
				}
			} else {
				reg = IR_REGSET_FIRST(scratch);
			}
		} else {
			reg = IR_REGSET_FIRST(available);
		}
		ival->reg = reg;
		IR_LOG_LSRA_ASSIGN("    ---- Assign", ival, " (available without spilling)");
		if (*unhandled && ival->end > (*unhandled)->range.start) {
			ival->list_next = *active;
			*active = ival;
		}
		return reg;
	}

	/* reg = register with highest freeUntilPos */
	reg = IR_REG_NONE;
	pos = 0;
	IR_REGSET_FOREACH(overlapped, i) {
		if (freeUntilPos[i] > pos) {
			pos = freeUntilPos[i];
			reg = i;
		} else if (freeUntilPos[i] == pos
				&& !IR_REGSET_IN(IR_REGSET_SCRATCH, reg)
				&& IR_REGSET_IN(IR_REGSET_SCRATCH, i)) {
			/* prefer caller-saved registers to avoid save/restore in prologue/epilogue */
			pos = freeUntilPos[i];
			reg = i;
		}
	} IR_REGSET_FOREACH_END();

	if (pos > ival->range.start) {
		/* register available for the first part of the interval */
		/* split current before freeUntilPos[reg] */
		ir_live_pos split_pos = ir_last_use_pos_before(ival, pos,
			IR_USE_MUST_BE_IN_REG | IR_USE_SHOULD_BE_IN_REG);
		if (split_pos > ival->range.start) {
			split_pos = ir_find_optimal_split_position(ctx, ival, split_pos, pos, 0);
			other = ir_split_interval_at(ctx, ival, split_pos);
			if (ival->flags & (IR_LIVE_INTERVAL_HAS_HINT_REGS|IR_LIVE_INTERVAL_HAS_HINT_REFS)) {
				ir_reg pref_reg = ir_try_allocate_preferred_reg(ctx, ival, IR_REGSET_UNION(available, overlapped), freeUntilPos);

				if (pref_reg != IR_REG_NONE) {
					ival->reg = pref_reg;
				} else {
					ival->reg = reg;
				}
			} else {
				ival->reg = reg;
			}
			IR_LOG_LSRA_ASSIGN("    ---- Assign", ival, " (available without spilling for the first part)");
			if (*unhandled && ival->end > (*unhandled)->range.start) {
				ival->list_next = *active;
				*active = ival;
			}
			ir_add_to_unhandled(unhandled, other);
			IR_LOG_LSRA("      ---- Queue", other, "");
			return reg;
		}
	}
	return IR_REG_NONE;
}

static ir_reg ir_allocate_blocked_reg(ir_ctx *ctx, ir_live_interval *ival, ir_live_interval **active, ir_live_interval **inactive, ir_live_interval **unhandled)
{
	ir_live_pos nextUsePos[IR_REG_NUM];
	ir_live_pos blockPos[IR_REG_NUM];
	int i, reg;
	ir_live_pos pos, next_use_pos;
	ir_live_interval *other, *prev;
	ir_use_pos *use_pos;
	ir_regset available, tmp_regset;

	if (!(ival->flags & IR_LIVE_INTERVAL_TEMP)) {
		use_pos = ival->use_pos;
		while (use_pos && !(use_pos->flags & IR_USE_MUST_BE_IN_REG)) {
			use_pos = use_pos->next;
		}
		if (!use_pos) {
			/* spill */
			IR_LOG_LSRA("    ---- Spill", ival, " (no use pos that must be in reg)");
			ctx->flags2 |= IR_RA_HAVE_SPILLS;
			return IR_REG_NONE;
		}
		next_use_pos = use_pos->pos;
	} else {
		next_use_pos = ival->range.end;
	}

	if (IR_IS_TYPE_FP(ival->type)) {
		available = IR_REGSET_FP;
		/* set nextUsePos of all physical registers to maxInt */
		for (i = IR_REG_FP_FIRST; i <= IR_REG_FP_LAST; i++) {
			nextUsePos[i] = 0x7fffffff;
			blockPos[i] = 0x7fffffff;
		}
	} else {
		available = IR_REGSET_GP;
		if (ctx->flags & IR_USE_FRAME_POINTER) {
			IR_REGSET_EXCL(available, IR_REG_FRAME_POINTER);
		}
#if defined(IR_TARGET_X86)
		if (ir_type_size[ival->type] == 1) {
			/* TODO: if no registers avialivle, we may use of one this register for already allocated interval ??? */
			IR_REGSET_EXCL(available, IR_REG_RBP);
			IR_REGSET_EXCL(available, IR_REG_RSI);
			IR_REGSET_EXCL(available, IR_REG_RDI);
		}
#endif
		/* set nextUsePos of all physical registers to maxInt */
		for (i = IR_REG_GP_FIRST; i <= IR_REG_GP_LAST; i++) {
			nextUsePos[i] = 0x7fffffff;
			blockPos[i] = 0x7fffffff;
		}
	}

	available = IR_REGSET_DIFFERENCE(available, (ir_regset)ctx->fixed_regset);

	if (IR_REGSET_IS_EMPTY(available)) {
		fprintf(stderr, "LSRA Internal Error: No registers available. Allocation is not possible\n");
		IR_ASSERT(0);
		exit(-1);
	}

	/* for each interval it in active */
	other = *active;
	while (other) {
		/* nextUsePos[it.reg] = next use of it after start of current */
		reg = other->reg;
		IR_ASSERT(reg >= 0);
		if (reg >= IR_REG_SCRATCH) {
			ir_regset regset;

			if (reg == IR_REG_SCRATCH) {
				regset = IR_REGSET_INTERSECTION(available, IR_REGSET_SCRATCH);
			} else {
				IR_ASSERT(reg == IR_REG_ALL);
				regset = available;
			}
			IR_REGSET_FOREACH(regset, reg) {
				blockPos[reg] = nextUsePos[reg] = 0;
			} IR_REGSET_FOREACH_END();
		} else if (IR_REGSET_IN(available, reg)) {
			if (other->flags & (IR_LIVE_INTERVAL_FIXED|IR_LIVE_INTERVAL_TEMP)) {
				blockPos[reg] = nextUsePos[reg] = 0;
			} else {
				pos = ir_first_use_pos_after(other, ival->range.start,
					IR_USE_MUST_BE_IN_REG | IR_USE_SHOULD_BE_IN_REG);
				if (pos < nextUsePos[reg]) {
					nextUsePos[reg] = pos;
				}
			}
		}
		other = other->list_next;
	}

	/* for each interval it in inactive intersecting with current */
	other = *inactive;
	while (other) {
		/* freeUntilPos[it.reg] = next intersection of it with current */
		reg = other->reg;
		IR_ASSERT(reg >= 0);
		if (reg >= IR_REG_SCRATCH) {
			ir_live_pos overlap = ir_ivals_overlap(&ival->range, other->current_range);

			if (overlap) {
				ir_regset regset;

				if (reg == IR_REG_SCRATCH) {
					regset = IR_REGSET_INTERSECTION(available, IR_REGSET_SCRATCH);
				} else {
					IR_ASSERT(reg == IR_REG_ALL);
					regset = available;
				}
				IR_REGSET_FOREACH(regset, reg) {
					if (overlap < nextUsePos[reg]) {
						nextUsePos[reg] = overlap;
					}
					if (overlap < blockPos[reg]) {
						blockPos[reg] = overlap;
					}
				} IR_REGSET_FOREACH_END();
			}
		} else if (IR_REGSET_IN(available, reg)) {
			ir_live_pos overlap = ir_ivals_overlap(&ival->range, other->current_range);

			if (overlap) {
				if (other->flags & (IR_LIVE_INTERVAL_FIXED|IR_LIVE_INTERVAL_TEMP)) {
					if (overlap < nextUsePos[reg]) {
						nextUsePos[reg] = overlap;
					}
					if (overlap < blockPos[reg]) {
						blockPos[reg] = overlap;
					}
				} else {
					pos = ir_first_use_pos_after(other, ival->range.start,
						IR_USE_MUST_BE_IN_REG | IR_USE_SHOULD_BE_IN_REG);
					if (pos < nextUsePos[reg]) {
						nextUsePos[reg] = pos;
					}
				}
			}
		}
		other = other->list_next;
	}

	/* register hinting */
	reg = IR_REG_NONE;
	if (ival->flags & (IR_LIVE_INTERVAL_HAS_HINT_REGS|IR_LIVE_INTERVAL_HAS_HINT_REFS)) {
		reg = ir_get_preferred_reg(ctx, ival, available);
	}
	if (reg == IR_REG_NONE) {
select_register:
		reg = IR_REGSET_FIRST(available);
	}

	/* reg = register with highest nextUsePos */
	pos = nextUsePos[reg];
	tmp_regset = available;
	IR_REGSET_EXCL(tmp_regset, reg);
	IR_REGSET_FOREACH(tmp_regset, i) {
		if (nextUsePos[i] > pos) {
			pos = nextUsePos[i];
			reg = i;
		}
	} IR_REGSET_FOREACH_END();

	/* if first usage of current is after nextUsePos[reg] then */
	if (next_use_pos > pos && !(ival->flags & IR_LIVE_INTERVAL_TEMP)) {
		/* all other intervals are used before current, so it is best to spill current itself */
		/* assign spill slot to current */
		/* split current before its first use position that requires a register */
		ir_live_pos split_pos;

spill_current:
		if (next_use_pos == ival->range.start) {
			IR_ASSERT(ival->use_pos && ival->use_pos->op_num == 0);
			/* split right after definition */
			split_pos = next_use_pos + 1;
		} else {
			split_pos = ir_find_optimal_split_position(ctx, ival, ival->range.start, next_use_pos - 1, 1);
		}

		if (split_pos > ival->range.start) {
			IR_LOG_LSRA("    ---- Conflict with others", ival, " (all others are used before)");
			other = ir_split_interval_at(ctx, ival, split_pos);
			IR_LOG_LSRA("    ---- Spill", ival, "");
			ir_add_to_unhandled(unhandled, other);
			IR_LOG_LSRA("    ---- Queue", other, "");
			return IR_REG_NONE;
		}
	}

	if (ival->end > blockPos[reg]) {
		/* spilling make a register free only for the first part of current */
		IR_LOG_LSRA("    ---- Conflict with others", ival, " (spilling make a register free only for the first part)");
		/* split current at optimal position before block_pos[reg] */
		ir_live_pos split_pos = ir_last_use_pos_before(ival,  blockPos[reg] + 1,
			IR_USE_MUST_BE_IN_REG | IR_USE_SHOULD_BE_IN_REG);
		if (split_pos == 0) {
			split_pos = ir_first_use_pos_after(ival, blockPos[reg],
				IR_USE_MUST_BE_IN_REG | IR_USE_SHOULD_BE_IN_REG) - 1;
			other = ir_split_interval_at(ctx, ival, split_pos);
			ir_add_to_unhandled(unhandled, other);
			IR_LOG_LSRA("      ---- Queue", other, "");
			return IR_REG_NONE;
		}
		if (split_pos >= blockPos[reg]) {
try_next_available_register:
			IR_REGSET_EXCL(available, reg);
			if (IR_REGSET_IS_EMPTY(available)) {
				fprintf(stderr, "LSRA Internal Error: Unsolvable conflict. Allocation is not possible\n");
				IR_ASSERT(0);
				exit(-1);
			}
			IR_LOG_LSRA("      ---- Restart", ival, "");
			goto select_register;
		}
		split_pos = ir_find_optimal_split_position(ctx, ival, split_pos, blockPos[reg], 1);
		other = ir_split_interval_at(ctx, ival, split_pos);
		ir_add_to_unhandled(unhandled, other);
		IR_LOG_LSRA("      ---- Queue", other, "");
	}

	/* spill intervals that currently block reg */
	prev = NULL;
	other = *active;
	while (other) {
		ir_live_pos split_pos;

		if (reg == other->reg) {
			/* split active interval for reg at position */
			ir_live_pos overlap = ir_ivals_overlap(&ival->range, other->current_range);

			if (overlap) {
				ir_live_interval *child, *child2;

				IR_ASSERT(other->type != IR_VOID);
				IR_LOG_LSRA_CONFLICT("      ---- Conflict with active", other, overlap);

				split_pos = ir_last_use_pos_before(other, ival->range.start, IR_USE_MUST_BE_IN_REG | IR_USE_SHOULD_BE_IN_REG);
				if (split_pos == 0) {
					split_pos = ival->range.start;
				}
				split_pos = ir_find_optimal_split_position(ctx, other, split_pos, ival->range.start, 1);
				if (split_pos > other->range.start) {
					child = ir_split_interval_at(ctx, other, split_pos);
					if (prev) {
						prev->list_next = other->list_next;
					} else {
						*active = other->list_next;
					}
					IR_LOG_LSRA("      ---- Finish", other, "");
				} else {
					if (ir_first_use_pos(other, IR_USE_MUST_BE_IN_REG) <= other->end) {
						if (!(ival->flags & IR_LIVE_INTERVAL_TEMP)) {
							next_use_pos = ir_first_use_pos(ival, IR_USE_MUST_BE_IN_REG);
							if (next_use_pos == ival->range.start) {
								IR_ASSERT(ival->use_pos && ival->use_pos->op_num == 0);
								/* split right after definition */
								split_pos = next_use_pos + 1;
							} else {
								split_pos = ir_find_optimal_split_position(ctx, ival, ival->range.start, next_use_pos - 1, 1);
							}

							if (split_pos > ival->range.start) {
								goto spill_current;
							}
						}
						goto try_next_available_register;
					}
					child = other;
					other->reg = IR_REG_NONE;
					if (prev) {
						prev->list_next = other->list_next;
					} else {
						*active = other->list_next;
					}
					IR_LOG_LSRA("      ---- Spill and Finish", other, " (it must not be in reg)");
				}

				split_pos = ir_first_use_pos_after(child, ival->range.start, IR_USE_MUST_BE_IN_REG | IR_USE_SHOULD_BE_IN_REG) - 1; // TODO: ???
				if (split_pos > child->range.start && split_pos < child->end) {
					ir_live_pos opt_split_pos = ir_find_optimal_split_position(ctx, child, ival->range.start, split_pos, 1);
					if (opt_split_pos > child->range.start) {
						split_pos = opt_split_pos;
					}
					child2 = ir_split_interval_at(ctx, child, split_pos);
					IR_LOG_LSRA("      ---- Spill", child, "");
					ir_add_to_unhandled(unhandled, child2);
					IR_LOG_LSRA("      ---- Queue", child2, "");
				} else if (child != other) {
					// TODO: this may cause endless loop
					ir_add_to_unhandled(unhandled, child);
					IR_LOG_LSRA("      ---- Queue", child, "");
				}
			}
			break;
		}
		prev = other;
		other = other->list_next;
	}

	/* split any inactive interval for reg at the end of its lifetime hole */
	other = *inactive;
	while (other) {
		/* freeUntilPos[it.reg] = next intersection of it with current */
		if (reg == other->reg) {
			ir_live_pos overlap = ir_ivals_overlap(&ival->range, other->current_range);

			if (overlap) {
				ir_live_interval *child;

				IR_ASSERT(other->type != IR_VOID);
				IR_LOG_LSRA_CONFLICT("      ---- Conflict with inactive", other, overlap);
				// TODO: optimal split position (this case is not tested)
				child = ir_split_interval_at(ctx, other, overlap);
				/* reset range cache */
				other->current_range = &other->range;
				ir_add_to_unhandled(unhandled, child);
				IR_LOG_LSRA("      ---- Queue", child, "");
			}
		}
		other = other->list_next;
	}

	/* current.reg = reg */
	ival->reg = reg;
	IR_LOG_LSRA_ASSIGN("    ---- Assign", ival, " (after splitting others)");

	if (*unhandled && ival->end > (*unhandled)->range.start) {
		ival->list_next = *active;
		*active = ival;
	}
	return reg;
}

static int ir_fix_dessa_tmps(ir_ctx *ctx, uint8_t type, ir_ref from, ir_ref to)
{
	ir_block *bb = ctx->data;
	ir_tmp_reg tmp_reg;

	if (to == 0) {
		if (IR_IS_TYPE_INT(type)) {
			tmp_reg.num = 0;
			tmp_reg.type = type;
			tmp_reg.start = IR_DEF_SUB_REF;
			tmp_reg.end = IR_SAVE_SUB_REF;
		} else {
			IR_ASSERT(IR_IS_TYPE_FP(type));
			tmp_reg.num = 1;
			tmp_reg.type = type;
			tmp_reg.start = IR_DEF_SUB_REF;
			tmp_reg.end = IR_SAVE_SUB_REF;
		}
	} else if (from != 0) {
		if (IR_IS_TYPE_INT(type)) {
			tmp_reg.num = 0;
			tmp_reg.type = type;
			tmp_reg.start = IR_DEF_SUB_REF;
			tmp_reg.end = IR_SAVE_SUB_REF;
		} else {
			IR_ASSERT(IR_IS_TYPE_FP(type));
			tmp_reg.num = 1;
			tmp_reg.type = type;
			tmp_reg.start = IR_DEF_SUB_REF;
			tmp_reg.end = IR_SAVE_SUB_REF;
		}
	} else {
		return 1;
	}
	if (!ir_has_tmp(ctx, bb->end, tmp_reg.num)) {
		ir_add_tmp(ctx, bb->end, bb->end, tmp_reg.num, tmp_reg);
	}
	return 1;
}

static bool ir_ival_spill_for_fuse_load(ir_ctx *ctx, ir_live_interval *ival, ir_reg_alloc_data *data)
{
	ir_use_pos *use_pos = ival->use_pos;
	ir_insn *insn;

	if (ival->flags & IR_LIVE_INTERVAL_MEM_PARAM) {
		IR_ASSERT(!ival->next && use_pos && use_pos->op_num == 0);
		insn = &ctx->ir_base[IR_LIVE_POS_TO_REF(use_pos->pos)];
		IR_ASSERT(insn->op == IR_PARAM);
		use_pos = use_pos->next;
		if (use_pos && (use_pos->next || (use_pos->flags & IR_USE_MUST_BE_IN_REG))) {
			return 0;
		}

		if (use_pos) {
			ir_block *bb = ir_block_from_live_pos(ctx, use_pos->pos);
			if (bb->loop_depth) {
				return 0;
			}
		}

		return 1;
	} else if (ival->flags & IR_LIVE_INTERVAL_MEM_LOAD) {
		insn = &ctx->ir_base[IR_LIVE_POS_TO_REF(use_pos->pos)];
		IR_ASSERT(insn->op == IR_VLOAD);
		IR_ASSERT(ctx->ir_base[insn->op2].op == IR_VAR);
		use_pos = use_pos->next;
		if (use_pos && (use_pos->next || (use_pos->flags & IR_USE_MUST_BE_IN_REG))) {
			return 0;
		}

		if (use_pos) {
			ir_block *bb = ir_block_from_live_pos(ctx, use_pos->pos);
			if (bb->loop_depth && bb != ir_block_from_live_pos(ctx, ival->use_pos->pos)) {
				return 0;
			}
			/* check if VAR may be clobbered between VLOAD and use */
			ir_use_list *use_list = &ctx->use_lists[insn->op2];
			ir_ref n = use_list->count;
			ir_ref *p = &ctx->use_edges[use_list->refs];
			for (; n > 0; p++, n--) {
				ir_ref use = *p;
				if (ctx->ir_base[use].op == IR_VSTORE) {
					if (use > IR_LIVE_POS_TO_REF(ival->use_pos->pos) && use < IR_LIVE_POS_TO_REF(use_pos->pos)) {
						return 0;
					}
				} else if (ctx->ir_base[use].op == IR_VADDR) {
					return 0;
				}
			}
		}
		ival->stack_spill_pos = ctx->ir_base[insn->op2].op3;

		return 1;
	}
	return 0;
}

static void ir_assign_bound_spill_slots(ir_ctx *ctx)
{
	ir_hashtab_bucket *b = ctx->binding->data;
	uint32_t n = ctx->binding->count;
	uint32_t v;
	ir_live_interval *ival;

	while (n > 0) {
		v = ctx->vregs[b->key];
		if (v) {
			ival = ctx->live_intervals[v];
			if (ival
			 && ival->stack_spill_pos == -1
			 && (ival->next || ival->reg == IR_REG_NONE)) {
				IR_ASSERT(b->val < 0);
				/* special spill slot */
				ival->stack_spill_pos = -b->val;
				ival->flags |= IR_LIVE_INTERVAL_SPILLED | IR_LIVE_INTERVAL_SPILL_SPECIAL;
			}
		}
		b++;
		n--;
	}
}

static int ir_linear_scan(ir_ctx *ctx)
{
	uint32_t b;
	ir_block *bb;
	ir_live_interval *unhandled = NULL;
	ir_live_interval *active = NULL;
	ir_live_interval *inactive = NULL;
	ir_live_interval *ival, *other, *prev;
	int j;
	ir_live_pos position;
	ir_reg reg;
	ir_reg_alloc_data data;
	ir_ref vars = ctx->vars;

	if (!ctx->live_intervals) {
		return 0;
	}

	if (ctx->flags2 & IR_LR_HAVE_DESSA_MOVES) {
		/* Add fixed intervals for temporary registers used for DESSA moves */
		for (b = 1, bb = &ctx->cfg_blocks[1]; b <= ctx->cfg_blocks_count; b++, bb++) {
			IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));
			if (bb->flags & IR_BB_DESSA_MOVES) {
				ctx->data = bb;
				ir_gen_dessa_moves(ctx, b, ir_fix_dessa_tmps);
			}
		}
	}

	ctx->data = &data;
	ctx->stack_frame_size = 0;
	data.unused_slot_4 = 0;
	data.unused_slot_2 = 0;
	data.unused_slot_1 = 0;
	data.handled = NULL;

	while (vars) {
		ir_insn *insn = &ctx->ir_base[vars];

		IR_ASSERT(insn->op == IR_VAR);
		vars = insn->op3; /* list next */

		insn->op3 = ir_allocate_spill_slot(ctx, insn->type, &data);
	}

	for (j = ctx->vregs_count; j != 0; j--) {
		ival = ctx->live_intervals[j];
		if (ival) {
			if (!(ival->flags & (IR_LIVE_INTERVAL_MEM_PARAM|IR_LIVE_INTERVAL_MEM_LOAD))
					|| !ir_ival_spill_for_fuse_load(ctx, ival, &data)) {
				ir_add_to_unhandled(&unhandled, ival);
			}
		}
	}

	ival = ctx->live_intervals[0];
	if (ival) {
		ir_merge_to_unhandled(&unhandled, ival);
	}

	/* vregs + tmp + fixed + SRATCH + ALL */
	for (j = ctx->vregs_count + 1; j <= ctx->vregs_count + IR_REG_NUM + 2; j++) {
		ival = ctx->live_intervals[j];
		if (ival) {
			ival->current_range = &ival->range;
			ival->list_next = inactive;
			inactive = ival;
		}
	}

	ctx->flags2 &= ~(IR_RA_HAVE_SPLITS|IR_RA_HAVE_SPILLS);

#ifdef IR_DEBUG
	if (ctx->flags & IR_DEBUG_RA) {
		fprintf(stderr, "----\n");
		ir_dump_live_ranges(ctx, stderr);
		fprintf(stderr, "---- Start LSRA\n");
	}
#endif

	while (unhandled) {
		ival = unhandled;
		ival->current_range = &ival->range;
		unhandled = ival->list_next;
		position = ival->range.start;

		IR_LOG_LSRA("  ---- Processing", ival, "...");

		/* for each interval i in active */
		other = active;
		prev = NULL;
		while (other) {
			ir_live_range *r = other->current_range;

			IR_ASSERT(r);
			if (r->end <= position) {
				do {
					r = r->next;
				} while (r && r->end <= position);
				if (!r) {
					/* move i from active to handled */
					other = other->list_next;
					if (prev) {
						prev->list_next = other;
					} else {
						active = other;
					}
					continue;
				}
				other->current_range = r;
			}
			if (position < r->start) {
				/* move i from active to inactive */
				if (prev) {
					prev->list_next = other->list_next;
				} else {
					active = other->list_next;
				}
				other->list_next = inactive;
				inactive = other;
			} else {
				prev = other;
			}
			other = prev ? prev->list_next : active;
		}

		/* for each interval i in inactive */
		other = inactive;
		prev = NULL;
		while (other) {
			ir_live_range *r = other->current_range;

			IR_ASSERT(r);
			if (r->end <= position) {
				do {
					r = r->next;
				} while (r && r->end <= position);
				if (!r) {
					/* move i from inactive to handled */
					other = other->list_next;
					if (prev) {
						prev->list_next = other;
					} else {
						inactive = other;
					}
					continue;
				}
				other->current_range = r;
			}
			if (position >= r->start) {
				/* move i from inactive to active */
				if (prev) {
					prev->list_next = other->list_next;
				} else {
					inactive = other->list_next;
				}
				other->list_next = active;
				active = other;
			} else {
				prev = other;
			}
			other = prev ? prev->list_next : inactive;
		}

		reg = ir_try_allocate_free_reg(ctx, ival, &active, inactive, &unhandled);
		if (reg == IR_REG_NONE) {
			reg = ir_allocate_blocked_reg(ctx, ival, &active, &inactive, &unhandled);
		}
	}

#if 0 //def IR_DEBUG
	/* all intervals must be processed */
	ival = active;
	while (ival) {
		IR_ASSERT(!ival->next);
		ival = ival->list_next;
	}
	ival = inactive;
	while (ival) {
		IR_ASSERT(!ival->next);
		ival = ival->list_next;
	}
#endif

	if (ctx->flags2 & (IR_RA_HAVE_SPLITS|IR_RA_HAVE_SPILLS)) {

		if (ctx->binding) {
			ir_assign_bound_spill_slots(ctx);
		}

		/* Use simple linear-scan (without holes) to allocate and reuse spill slots */
		unhandled = NULL;
		for (j = ctx->vregs_count; j != 0; j--) {
			ival = ctx->live_intervals[j];
			if (ival
			 && (ival->next || ival->reg == IR_REG_NONE)
			 && ival->stack_spill_pos == -1) {
				ival->flags |= IR_LIVE_INTERVAL_SPILLED;
				if (!(ival->flags & IR_LIVE_INTERVAL_MEM_PARAM)) {
					ir_live_range *r;

					other = ival;
					while (other->next) {
						other = other->next;
					}
					r = &other->range;
					while (r->next) {
						r = r->next;
					}
					ival->end = r->end;
					ir_add_to_unhandled_spill(&unhandled, ival);
				}
			}
		}

		if (unhandled) {
			uint8_t size;
			ir_live_interval *handled[9] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
			ir_live_interval *old;

			data.handled = handled;
			active = NULL;
			while (unhandled) {
				ival = unhandled;
				ival->current_range = &ival->range;
				unhandled = ival->list_next;
				position = ival->range.start;

				/* for each interval i in active */
				other = active;
				prev = NULL;
				while (other) {
					if (other->end <= position) {
						/* move i from active to handled */
						if (prev) {
							prev->list_next = other->list_next;
						} else {
							active = other->list_next;
						}
						size = ir_type_size[other->type];
						IR_ASSERT(size == 1 || size == 2 || size == 4 || size == 8);
						old = handled[size];
						while (old) {
							if (old->stack_spill_pos == other->stack_spill_pos) {
								break;
							}
							old = old->list_next;
						}
						if (!old) {
							other->list_next = handled[size];
							handled[size] = other;
						}
					} else {
						prev = other;
					}
					other = prev ? prev->list_next : active;
				}

				ival->stack_spill_pos = ir_allocate_spill_slot(ctx, ival->type, &data);
				if (unhandled && ival->end > unhandled->range.start) {
					ival->list_next = active;
					active = ival;
				} else {
					size = ir_type_size[ival->type];
					IR_ASSERT(size == 1 || size == 2 || size == 4 || size == 8);
					old = handled[size];
					while (old) {
						if (old->stack_spill_pos == ival->stack_spill_pos) {
							break;
						}
						old = old->list_next;
					}
					if (!old) {
						ival->list_next = handled[size];
						handled[size] = ival;
					}
				}
			}
			data.handled = NULL;
		}
	}

#ifdef IR_TARGET_X86
	if (ctx->flags2 & IR_HAS_FP_RET_SLOT) {
		ctx->ret_slot = ir_allocate_spill_slot(ctx, IR_DOUBLE, &data);
	} else if (ctx->ret_type == IR_FLOAT || ctx->ret_type == IR_DOUBLE) {
		ctx->ret_slot = ir_allocate_spill_slot(ctx, ctx->ret_type, &data);
	} else {
		ctx->ret_slot = -1;
	}
#endif

#ifdef IR_DEBUG
	if (ctx->flags & IR_DEBUG_RA) {
		fprintf(stderr, "---- Finish LSRA\n");
		ir_dump_live_ranges(ctx, stderr);
		fprintf(stderr, "----\n");
	}
#endif

	return 1;
}

static bool needs_spill_reload(ir_ctx *ctx, ir_live_interval *ival, uint32_t b0, ir_bitset available)
{
    ir_worklist worklist;
	ir_block *bb;
	uint32_t b, *p, n;

	ir_worklist_init(&worklist, ctx->cfg_blocks_count + 1);
	ir_worklist_push(&worklist, b0);
	while (ir_worklist_len(&worklist) != 0) {
		b = ir_worklist_pop(&worklist);
        bb = &ctx->cfg_blocks[b];
		if (bb->flags & (IR_BB_ENTRY|IR_BB_START)) {
			ir_worklist_free(&worklist);
			return 1;
		}
		n = bb->predecessors_count;
		for (p = &ctx->cfg_edges[bb->predecessors]; n > 0; p++, n--) {
			b = *p;
			bb = &ctx->cfg_blocks[b];

			if (!ir_ival_covers(ival, IR_SAVE_LIVE_POS_FROM_REF(bb->end))) {
				ir_worklist_free(&worklist);
				return 1;
			} else if (!ir_bitset_in(available, b)) {
				ir_worklist_push(&worklist, b);
			}
		}
	}
	ir_worklist_free(&worklist);
	return 0;
}

static bool needs_spill_load(ir_ctx *ctx, ir_live_interval *ival, ir_use_pos *use_pos)
{
	if (use_pos->next
	 && use_pos->op_num == 1
	 && use_pos->next->pos == use_pos->pos
	 && !(use_pos->next->flags & IR_USE_MUST_BE_IN_REG)) {
		/* Support for R2 = ADD(R1, R1) */
		use_pos = use_pos->next;
	}
	return use_pos->next && use_pos->next->op_num != 0;
}

static void ir_set_fused_reg(ir_ctx *ctx, ir_ref root, ir_ref ref, uint8_t op_num, int8_t reg)
{
	char key[10];

	IR_ASSERT(reg != IR_REG_NONE);
	if (!ctx->fused_regs) {
		ctx->fused_regs = ir_mem_malloc(sizeof(ir_strtab));
		ir_strtab_init(ctx->fused_regs, 8, 128);
	}
	memcpy(key, &root, sizeof(ir_ref));
	memcpy(key + 4, &ref, sizeof(ir_ref));
	memcpy(key + 8, &op_num, sizeof(uint8_t));
	ir_strtab_lookup(ctx->fused_regs, key, 9, 0x10000000 | reg);
}

static void assign_regs(ir_ctx *ctx)
{
	ir_ref i;
	ir_live_interval *ival, *top_ival;
	ir_use_pos *use_pos;
	int8_t reg, old_reg;
	ir_ref ref;
	ir_regset used_regs = 0;

	if (!ctx->regs) {
		ctx->regs = ir_mem_malloc(sizeof(ir_regs) * ctx->insns_count);
		memset(ctx->regs, IR_REG_NONE, sizeof(ir_regs) * ctx->insns_count);
	}

	if (!(ctx->flags2 & (IR_RA_HAVE_SPLITS|IR_RA_HAVE_SPILLS))) {
		for (i = 1; i <= ctx->vregs_count; i++) {
			ival = ctx->live_intervals[i];
			if (ival) {
				do {
					if (ival->reg != IR_REG_NONE) {
						reg = ival->reg;
						IR_REGSET_INCL(used_regs, reg);
						use_pos = ival->use_pos;
						while (use_pos) {
							ref = (use_pos->hint_ref < 0) ? -use_pos->hint_ref : IR_LIVE_POS_TO_REF(use_pos->pos);
							ir_set_alocated_reg(ctx, ref, use_pos->op_num, reg);
							use_pos = use_pos->next;
						}
					}
					ival = ival->next;
				} while (ival);
			}
		}
	} else {
		ir_bitset available = ir_bitset_malloc(ctx->cfg_blocks_count + 1);

		for (i = 1; i <= ctx->vregs_count; i++) {
			top_ival = ival = ctx->live_intervals[i];
			if (ival) {
				if (!(ival->flags & IR_LIVE_INTERVAL_SPILLED)) {
					do {
						if (ival->reg != IR_REG_NONE) {
							IR_REGSET_INCL(used_regs, ival->reg);
							use_pos = ival->use_pos;
							while (use_pos) {
								reg = ival->reg;
								ref = IR_LIVE_POS_TO_REF(use_pos->pos);
								if (use_pos->hint_ref < 0) {
									ref = -use_pos->hint_ref;
								}
								ir_set_alocated_reg(ctx, ref, use_pos->op_num, reg);

								use_pos = use_pos->next;
							}
						}
						ival = ival->next;
					} while (ival);
				} else {
					do {
						if (ival->reg != IR_REG_NONE) {
							ir_ref prev_use_ref = IR_UNUSED;

							ir_bitset_clear(available, ir_bitset_len(ctx->cfg_blocks_count + 1));
							IR_REGSET_INCL(used_regs, ival->reg);
							use_pos = ival->use_pos;
							while (use_pos) {
								reg = ival->reg;
								ref = IR_LIVE_POS_TO_REF(use_pos->pos);
								// TODO: Insert spill loads and stores in optimal positions (resolution)
								if (use_pos->op_num == 0) {
									ir_bitset_clear(available, ir_bitset_len(ctx->cfg_blocks_count + 1));
									if (ctx->ir_base[ref].op == IR_PHI) {
										/* Spilled PHI var is passed through memory */
										reg = IR_REG_NONE;
										prev_use_ref = IR_UNUSED;
									} else if (ctx->ir_base[ref].op == IR_PARAM
									 && (ival->flags & IR_LIVE_INTERVAL_MEM_PARAM)) {
										/* Stack PARAM var is passed through memory */
										reg = IR_REG_NONE;
									} else {
										uint32_t use_b = ctx->cfg_map[ref];

										if (ir_ival_covers(ival, IR_SAVE_LIVE_POS_FROM_REF(ctx->cfg_blocks[use_b].end))) {
											ir_bitset_incl(available, use_b);
										}
										if (top_ival->flags & IR_LIVE_INTERVAL_SPILL_SPECIAL) {
											reg |= IR_REG_SPILL_SPECIAL;
										} else {
											reg |= IR_REG_SPILL_STORE;
										}
										prev_use_ref = ref;
									}
								} else if ((!prev_use_ref || ctx->cfg_map[prev_use_ref] != ctx->cfg_map[ref])
								 && needs_spill_reload(ctx, ival, ctx->cfg_map[ref], available)) {
									if (!(use_pos->flags & IR_USE_MUST_BE_IN_REG)
									 && use_pos->hint != reg
//									 && ctx->ir_base[ref].op != IR_CALL
//									 && ctx->ir_base[ref].op != IR_TAILCALL) {
									 && ctx->ir_base[ref].op != IR_SNAPSHOT
									 && !needs_spill_load(ctx, ival, use_pos)) {
										/* fuse spill load (valid only when register is not reused) */
										reg = IR_REG_NONE;
										if (use_pos->next
										 && use_pos->op_num == 1
										 && use_pos->next->pos == use_pos->pos
										 && !(use_pos->next->flags & IR_USE_MUST_BE_IN_REG)) {
											/* Support for R2 = BINOP(R1, R1) */
											if (use_pos->hint_ref < 0) {
												ref = -use_pos->hint_ref;
											}
											ir_set_alocated_reg(ctx, ref, use_pos->op_num, reg);
											use_pos = use_pos->next;
										}
									} else {
										if (top_ival->flags & IR_LIVE_INTERVAL_SPILL_SPECIAL) {
											reg |= IR_REG_SPILL_SPECIAL;
										} else {
											reg |= IR_REG_SPILL_LOAD;
										}
										if (ctx->ir_base[ref].op != IR_SNAPSHOT) {
											uint32_t use_b = ctx->cfg_map[ref];

											if (ir_ival_covers(ival, IR_SAVE_LIVE_POS_FROM_REF(ctx->cfg_blocks[use_b].end))) {
												ir_bitset_incl(available, use_b);
											}
											prev_use_ref = ref;
										}
									}
									if (use_pos->hint_ref < 0
									 && ctx->use_lists[-use_pos->hint_ref].count > 1
									 && (old_reg = ir_get_alocated_reg(ctx, -use_pos->hint_ref, use_pos->op_num)) != IR_REG_NONE) {
										if (top_ival->flags & IR_LIVE_INTERVAL_SPILL_SPECIAL) {
											reg |= IR_REG_SPILL_SPECIAL;
										} else {
											reg |= IR_REG_SPILL_LOAD;
										}
										if (reg != old_reg) {
											IR_ASSERT(ctx->rules[-use_pos->hint_ref] & IR_FUSED);
											ctx->rules[-use_pos->hint_ref] |= IR_FUSED_REG;
											ir_set_fused_reg(ctx, ref, -use_pos->hint_ref, use_pos->op_num, reg);
											use_pos = use_pos->next;
											continue;
										}
									}
								} else if (use_pos->flags & IR_PHI_USE) {
									IR_ASSERT(use_pos->hint_ref < 0);
									IR_ASSERT(ctx->vregs[-use_pos->hint_ref]);
									IR_ASSERT(ctx->live_intervals[ctx->vregs[-use_pos->hint_ref]]);
									if (ctx->live_intervals[ctx->vregs[-use_pos->hint_ref]]->flags & IR_LIVE_INTERVAL_SPILLED) {
										/* Spilled PHI var is passed through memory */
										reg = IR_REG_NONE;
									}
								} else if (use_pos->hint_ref < 0
										&& ctx->use_lists[-use_pos->hint_ref].count > 1
										&& (old_reg = ir_get_alocated_reg(ctx, -use_pos->hint_ref, use_pos->op_num)) != IR_REG_NONE) {
									if (reg != old_reg) {
										IR_ASSERT(ctx->rules[-use_pos->hint_ref] & IR_FUSED);
										ctx->rules[-use_pos->hint_ref] |= IR_FUSED_REG;
										ir_set_fused_reg(ctx, ref, -use_pos->hint_ref, use_pos->op_num, reg);
										use_pos = use_pos->next;
										continue;
									}
								} else {
									/* reuse register without spill load */
								}
								if (use_pos->hint_ref < 0) {
									ref = -use_pos->hint_ref;
								}
								ir_set_alocated_reg(ctx, ref, use_pos->op_num, reg);

								use_pos = use_pos->next;
							}
						} else if (!(top_ival->flags & IR_LIVE_INTERVAL_SPILL_SPECIAL)) {
							use_pos = ival->use_pos;
							while (use_pos) {
								ref = IR_LIVE_POS_TO_REF(use_pos->pos);
								if (ctx->ir_base[ref].op == IR_SNAPSHOT) {
									IR_ASSERT(use_pos->hint_ref >= 0);
									/* A reference to a CPU spill slot */
									reg = IR_REG_SPILL_STORE | IR_REG_STACK_POINTER;
									ir_set_alocated_reg(ctx, ref, use_pos->op_num, reg);
								}
								use_pos = use_pos->next;
							}
						}
						ival = ival->next;
					} while (ival);
				}
			}
		}
		ir_mem_free(available);
	}

	/* Temporary registers */
	ival = ctx->live_intervals[0];
	if (ival) {
		do {
			IR_ASSERT(ival->reg != IR_REG_NONE);
			IR_REGSET_INCL(used_regs, ival->reg);
			ir_set_alocated_reg(ctx, ival->tmp_ref, ival->tmp_op_num, ival->reg);
			ival = ival->next;
		} while (ival);
	}

	if (ctx->fixed_stack_frame_size != -1) {
		ctx->used_preserved_regs = (ir_regset)ctx->fixed_save_regset;
		if (IR_REGSET_DIFFERENCE(IR_REGSET_INTERSECTION(used_regs, IR_REGSET_PRESERVED),
			ctx->used_preserved_regs)) {
			// TODO: Preserved reg and fixed frame conflict ???
			// IR_ASSERT(0 && "Preserved reg and fixed frame conflict");
		}
	} else {
		ctx->used_preserved_regs = IR_REGSET_UNION((ir_regset)ctx->fixed_save_regset,
			IR_REGSET_DIFFERENCE(IR_REGSET_INTERSECTION(used_regs, IR_REGSET_PRESERVED),
				(ctx->flags & IR_FUNCTION) ? (ir_regset)ctx->fixed_regset : IR_REGSET_PRESERVED));
	}

	ir_fix_stack_frame(ctx);
}

int ir_reg_alloc(ir_ctx *ctx)
{
	if (ir_linear_scan(ctx)) {
		assign_regs(ctx);
		return 1;
	}
	return 0;
}
