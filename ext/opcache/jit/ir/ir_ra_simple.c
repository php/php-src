/*
 * IR - Lightweight JIT Compilation Framework
 * Simple Local Register Allocation (used for -O0)
 * This file is part of the IR Project distributed under the MIT-style LICENSE.
 * Authors: Dmitry Stogov <dmitry@php.net>
 *
 * Simple RA assign registers for each IR instruction separately and always pass
 * values between instrutios in stack spill slots.
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
	int32_t  num;
	ir_regset preserved_regs;
	ir_regset clobbered[IR_SUB_REFS_COUNT];
	struct {
		uint8_t type;
		int8_t  start;
		int8_t  end;
		int8_t  hint;
		int8_t  flags;
		ir_ref  root;
		ir_ref  ref;
		ir_ref  op;
	} regs[32];
} ir_reg_alloc_simple_data;

static void _add_scratch(ir_reg_alloc_simple_data *data, ir_reg reg, int8_t start, int8_t end)
{
	int8_t j;

	if (start < 0) start = 0; // TODO: ARGVAL support ???
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

static void _add_reg(ir_reg_alloc_simple_data *data, ir_type type,
                     int8_t start, int8_t end, ir_reg hint, int8_t flags,
                     ir_ref root, ir_ref ref, ir_ref op)
{
	IR_ASSERT(start >= 0 && end <= IR_SUB_REFS_COUNT && data->num < 32);
	data->regs[data->num].type = type;
	data->regs[data->num].start = start;
	data->regs[data->num].end = end;
	data->regs[data->num].hint = hint;
	data->regs[data->num].flags = flags;
	data->regs[data->num].root = root;
	data->regs[data->num].ref = ref;
	data->regs[data->num].op = op;
	data->num++;
}

static ir_reg _get_free_reg(ir_type type, ir_regset available)
{
	if (IR_IS_TYPE_INT(type)) {
		available = IR_REGSET_INTERSECTION(available, IR_REGSET_GP);
	} else {
		IR_ASSERT(IR_IS_TYPE_FP(type) || IR_IS_TYPE_VECTOR(type));
		available = IR_REGSET_INTERSECTION(available, IR_REGSET_FP);
	}
	if (!IR_REGSET_IS_EMPTY(available)) {
		return IR_REGSET_FIRST(available);
	} else {
		return IR_REG_NONE;
	}
}

static ir_reg _get_free_reg2(ir_ctx *ctx, ir_type type, ir_reg_alloc_simple_data *data, int j)
{
	int n;
	ir_regset available;
	ir_reg reg;

	if (IR_IS_TYPE_INT(type)) {
		available = IR_REGSET_GP;
		if (ctx->flags & IR_USE_FRAME_POINTER) {
			IR_REGSET_EXCL(available, IR_REG_FRAME_POINTER);
		}

#if defined(IR_TARGET_X86)
		if (ir_type_size[type] == 1) {
			/* TODO: if no registers avialivle, we may use of one this register for already allocated interval ??? */
			IR_REGSET_EXCL(available, IR_REG_RBP);
			IR_REGSET_EXCL(available, IR_REG_RSI);
			IR_REGSET_EXCL(available, IR_REG_RDI);
		}
#endif
	} else {
		IR_ASSERT(IR_IS_TYPE_FP(type) || IR_IS_TYPE_VECTOR(type));
		available = IR_REGSET_FP;
	}
	for (n = data->regs[j].start; n < data->regs[j].end; n++) {
		available = IR_REGSET_DIFFERENCE(available, data->clobbered[n]);
	}
	if (IR_REGSET_IS_EMPTY(available)) {
		fprintf(stderr, "Internal Error: No registers available. Allocation is not possible\n");
		IR_ASSERT(0);
		exit(-1);
	}

	reg = IR_REGSET_FIRST(available);
	if (IR_REGSET_IN(data->preserved_regs, reg)) {
		ir_regset tmp = (ir_regset)ctx->used_preserved_regs;
		IR_REGSET_INCL(tmp, reg);
		ctx->used_preserved_regs = tmp;
	}
	return reg;
}

static bool ir_load_may_reuse_var_slot(ir_ctx *ctx, ir_block *bb, ir_ref var, ir_ref load)
{
	ir_use_list *use_list = &ctx->use_lists[load];
	ir_ref *p, use, i, n = use_list->count;
	ir_ref last_use = IR_UNUSED;
	ir_insn *insn;
	int address_taken = -1; /* unknown */

	if (n) {
		/* Check if all uses of the result of VLOAD are in the same basic block (after VLOAD) */
		for (p = ctx->use_edges + use_list->refs; n > 0; p++, n--) {
			use = *p;
			if (use < load || use > bb->end) return 0;
			if (use > last_use) last_use = use;
		}
		/* Check if the result of VLOAD can not be clobbered between VLOAD and the last use */
		for (i = load + 1, insn = &ctx->ir_base[i]; i < last_use;) {
			if ((insn->op == IR_VSTORE || insn->op == IR_VSTORE_v) && insn->op2 == var) {
				return 0;
			} else if (address_taken
			 && (insn->op == IR_STORE
			  || insn->op == IR_STORE_v
			  || insn->op == IR_CALL)) {
				if (address_taken == -1) {
					ir_use_list *use_list = &ctx->use_lists[var];
					ir_ref *p, n = use_list->count;

					for (p = ctx->use_edges + use_list->refs; n > 0; p++, n--) {
						if (ctx->ir_base[*p].op == IR_VADDR) {
							return 0;
						}
					}
					address_taken = 0;
				}
			}
			n = ir_insn_len(insn);
			i += n;
			insn += n;
		}
	}
	return 1;
}

static bool ir_store_may_reuse_var_slot(ir_ctx *ctx, ir_block *bb, ir_ref var, ir_ref store, ir_ref val)
{
	ir_ref i, n;
	ir_insn *insn;
	int address_taken = -1; /* unknown */

	/* Check if the value stored by VSTORE is produced in the same basic block (before VSTORE) */
	if (val < bb->start || val > store || store > bb->end) return 0;

	/* Check if the instructions between the "producer" and VSTORE can not access the same VAR */
	for (i = val, insn = &ctx->ir_base[i]; i < store;) {
		if ((insn->op == IR_VLOAD || insn->op == IR_VLOAD_v || insn->op == IR_VSTORE || insn->op == IR_VSTORE_v)
		 && insn->op2 == var) {
			return 0;
		} else if (address_taken
		 && (insn->op == IR_LOAD
		  || insn->op == IR_LOAD_v
		  || insn->op == IR_STORE
		  || insn->op == IR_STORE_v
		  || insn->op == IR_CALL)) {
			if (address_taken == -1) {
				ir_use_list *use_list = &ctx->use_lists[var];
				ir_ref *p, n = use_list->count;

				for (p = ctx->use_edges + use_list->refs; n > 0; p++, n--) {
					if (ctx->ir_base[*p].op == IR_VADDR) {
						return 0;
					}
				}
				address_taken = 0;
			}
		}
		n = ir_insn_len(insn);
		i += n;
		insn += n;
	}
	return 1;
}

static void ir_add_fusion_data(ir_ctx *ctx, ir_ref ref, ir_ref input, ir_reg_alloc_simple_data *data)
{
	ir_ref stack[4];
	int stack_pos = 0;
	ir_target_constraints constraints;
	ir_insn *insn;
	uint32_t j, n, flags, def_flags;
	ir_ref *p, child;

	while (1) {
		IR_ASSERT(input > 0 && ctx->rules[input] & IR_FUSED);

		if (!(ctx->rules[input] & IR_SIMPLE)) {
			def_flags = ir_get_target_constraints(ctx, input, &constraints);
			n = constraints.tmps_count;
			while (n > 0) {
				n--;
				if (constraints.tmp_regs[n].type) {
					ir_reg flags = 0;
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
					_add_reg(data, constraints.tmp_regs[n].type,
						constraints.tmp_regs[n].start, constraints.tmp_regs[n].end, IR_REG_NONE, flags,
						IR_UNUSED, input, op);
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
		flags = ir_op_flags[insn->op];
		n = IR_INPUT_EDGES_COUNT(flags);
		j = 1;
		p = insn->ops + j;
		if (flags & (IR_OP_FLAG_CONTROL|IR_OP_FLAG_PINNED)) {
			j++;
			p++;
		}
		for (; j <= n; j++, p++) {
			IR_ASSERT(IR_OPND_KIND(flags, j) == IR_OPND_DATA);
			child = *p;
			if (child > 0) {
				if (ctx->vregs[child] > 0) {
					if (IR_USE_FLAGS(def_flags, j) & IR_USE_MUST_BE_IN_REG) {
						ir_reg reg = (j < constraints.hints_count) ? constraints.hints[j] : IR_REG_NONE;
						int8_t use_pos = EXPECTED(reg == IR_REG_NONE) ? IR_USE_SUB_REF : IR_LOAD_SUB_REF;

						_add_reg(data, ctx->ir_base[child].type, IR_LOAD_SUB_REF, use_pos, reg, IR_REG_SPILL_LOAD,
							ref, input, j);
					}
				} else if (ctx->rules[child] & IR_FUSED) {
					IR_ASSERT(stack_pos < (int)(sizeof(stack)/sizeof(stack_pos)));
					stack[stack_pos++] = child;
				} else if (ctx->rules[child] == (IR_SKIPPED|IR_RLOAD)) {
					ctx->regs[input][j] = ctx->ir_base[child].op2;
				}
			}
		}
		if (!stack_pos) {
			break;
		}
		input = stack[--stack_pos];
	}
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
}

int ir_reg_alloc_simple(ir_ctx *ctx)
{
	uint32_t b;
	ir_block *bb;
	ir_insn *insn;
	ir_ref i, n, j, *p;
	uint32_t *rule, insn_flags;
	ir_target_constraints constraints;
	uint32_t def_flags;
	ir_reg reg;
	ir_regset scratch;
	ir_reg_alloc_simple_data data;
	int32_t offset;

	ir_compute_dessa_moves(ctx);

	memset(&data.ra_data, 0, sizeof(data.ra_data));
	data.ra_data.cc = ir_get_call_conv_dsc(ctx->flags);
	ctx->data = &data.ra_data;

	ctx->stack_frame_size = 0;
	ctx->call_stack_size = 0;
	ctx->used_preserved_regs = 0;
	ctx->used_preserved_regs = ctx->fixed_save_regset;

	scratch = ir_scratch_regset[data.ra_data.cc->scratch_reg - IR_REG_NUM];
	data.preserved_regs = IR_REGSET_DIFFERENCE(data.ra_data.cc->preserved_regs, ctx->fixed_save_regset);

	ctx->regs = ir_mem_malloc(sizeof(ir_regs) * ctx->insns_count);
	memset(ctx->regs, IR_REG_NONE, sizeof(ir_regs) * ctx->insns_count);

	/* vregs + tmp + fixed + SRATCH + ALL */
	ctx->live_intervals = ir_mem_calloc(ctx->vregs_count + 1 + IR_REG_SET_NUM, sizeof(ir_live_interval*));

	if (!ctx->arena) {
		ctx->arena = ir_arena_create(16 * 1024);
	}

	for (b = 1, bb = ctx->cfg_blocks + b; b <= ctx->cfg_blocks_count; b++, bb++) {
		IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));
		for (i = bb->start, insn = ctx->ir_base + i, rule = ctx->rules + i; i <= bb->end;) {
			if (*rule & (IR_FUSED|IR_SKIPPED)) {
				if ((*rule & IR_RULE_MASK) == IR_ALLOCA) {
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
						}
					} else if (insn->op == IR_VADDR) {
						if (!IR_VREG_IS_STACK_SLOT(ctx->vregs[insn->op1])) {
							ir_allocate_var_spill_slot(ctx, insn->op1, &ctx->ir_base[insn->op1]);
						}
						ctx->vregs[i] = ctx->vregs[insn->op1];
					}
				}
			} else {
				data.num = 0;
				for (j = 0; j < IR_SUB_REFS_COUNT; j++) {
					data.clobbered[j] = IR_REGSET_EMPTY;
				}

				def_flags = ir_get_target_constraints(ctx, i, &constraints);
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
						_add_reg(&data, constraints.tmp_regs[n].type,
							constraints.tmp_regs[n].start, constraints.tmp_regs[n].end, IR_REG_NONE, flags,
							IR_UNUSED, i, op);
					} else {
						_add_scratch(&data, constraints.tmp_regs[n].reg,
							constraints.tmp_regs[n].start, constraints.tmp_regs[n].end);
					}
				}

				if (ctx->vregs[i] > 0) {
					reg = constraints.def_reg;
					if (!ctx->live_intervals[ctx->vregs[i]]) {
						do {
							ir_live_interval *ival = ir_arena_alloc(&ctx->arena, sizeof(ir_live_interval));
							memset(ival, 0, sizeof(ir_live_interval));
							ctx->live_intervals[ctx->vregs[i]] = ival;
							ival->type = insn->type;
							ival->reg = IR_REG_NONE;
							ival->vreg = ctx->vregs[i];
							ival->stack_spill_pos = -1;

							if (insn->op == IR_VLOAD || insn->op == IR_VLOAD_v) {
								if (!IR_VREG_IS_STACK_SLOT(ctx->vregs[insn->op2])) {
									ir_allocate_var_spill_slot(ctx, insn->op2, &ctx->ir_base[insn->op2]);
								}
								if (ir_load_may_reuse_var_slot(ctx, bb, insn->op2, i)) {
									ival->stack_spill_pos = IR_VREG_TO_STACK_SLOT(ctx->vregs[insn->op2]);
									reg = IR_REG_NONE;
									def_flags &= ~IR_USE_MUST_BE_IN_REG;
									break;
								}
							} else if (insn->op == IR_PARAM && reg == IR_REG_NONE) {
								/* We may reuse parameter stack slot for spilling */
								ival->flags |= IR_LIVE_INTERVAL_MEM_PARAM;
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
										}
									}
								}
								break;
							}

							if (ctx->use_lists[i].count == 1) {
								ir_ref use = ctx->use_edges[ctx->use_lists[i].refs];
								ir_insn *use_insn = &ctx->ir_base[use];

								if ((use_insn->op == IR_VSTORE || use_insn->op == IR_VSTORE_v)
								 && use_insn->op3 == i
								 && ir_store_may_reuse_var_slot(ctx, bb, use_insn->op2, use, i)) {
									if (!IR_VREG_IS_STACK_SLOT(ctx->vregs[use_insn->op2])) {
										ir_allocate_var_spill_slot(ctx, use_insn->op2, &ctx->ir_base[use_insn->op2]);
									}
									ival->stack_spill_pos = IR_VREG_TO_STACK_SLOT(ctx->vregs[use_insn->op2]);
									break;
								}
							}

							size_t size = ir_get_type_size(ival->type);
#if IR_SIMD
							if (IR_IS_TYPE_VECTOR(ival->type) && size < 4) {
								size = 4;
							}
#endif
							ival->stack_spill_pos = ir_allocate_spill_slot(ctx, size, 0);
						} while (0);
					}

					if (def_flags & IR_USE_MUST_BE_IN_REG) {
						ir_live_pos def_pos;

						if (reg != IR_REG_NONE) {
							def_pos = IR_SAVE_SUB_REF;
						} else if (def_flags & IR_DEF_REUSES_OP1_REG) {
							if (def_flags & IR_DEF_CONFLICTS_WITH_INPUT_REGS) {
								def_pos = IR_USE_SUB_REF;
							} else {
								def_pos = IR_LOAD_SUB_REF;
							}
						} else if (def_flags & IR_DEF_CONFLICTS_WITH_INPUT_REGS) {
							def_pos = IR_LOAD_SUB_REF;
						} else {
							def_pos = IR_DEF_SUB_REF;
						}

						_add_reg(&data, insn->type, def_pos, IR_SUB_REFS_COUNT, reg, IR_REG_SPILL_STORE,
							IR_UNUSED, i, 0);
					}
				} else if (insn->op == IR_VSTORE || insn->op == IR_VSTORE_v) {
					if (!IR_VREG_IS_STACK_SLOT(ctx->vregs[insn->op2])) {
						ir_allocate_var_spill_slot(ctx, insn->op2, &ctx->ir_base[insn->op2]);
					}
				}

				n = insn->inputs_count;
				insn_flags = ir_op_flags[insn->op];
				j = 1;
				p = insn->ops + 1;
				if (insn_flags & (IR_OP_FLAG_CONTROL|IR_OP_FLAG_MEM|IR_OP_FLAG_PINNED)) {
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
							use_pos = IR_USE_SUB_REF;
							if (reg != IR_REG_NONE) {
								use_pos = IR_LOAD_SUB_REF;
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
							if (use_flags & IR_USE_MUST_BE_IN_REG) {
								_add_reg(&data, ctx->ir_base[input].type, IR_LOAD_SUB_REF, use_pos, reg, IR_REG_SPILL_LOAD,
									IR_UNUSED, i, j);
							}
						} else {
							if ((ctx->rules[input] & (IR_FUSED|IR_SKIPPED)) == IR_FUSED) {
								ir_add_fusion_data(ctx, i, input, &data);
							} else if (ctx->rules[input] == (IR_SKIPPED|IR_RLOAD)) {
								ctx->regs[i][j] = ctx->ir_base[input].op2;
							}
						}
					}
				}

				for (j = 0; j < data.num; j++) {
					ir_regset available = scratch;
#if IR_X86_I64
					ir_reg reg2;
#endif

					for (n = data.regs[j].start; n < data.regs[j].end; n++) {
						available = IR_REGSET_DIFFERENCE(available, data.clobbered[n]);
					}
					reg = data.regs[j].hint;
#if IR_X86_I64
					reg2 = IR_REG_NONE;
					if (reg != IR_REG_NONE && (data.regs[j].type == IR_I64 || data.regs[j].type == IR_U64)) {
						reg2 = IR_REG_I64_HI(reg);
						reg = IR_REG_I64_LO(reg);
					}
#endif
					if (reg == IR_REG_NONE || !IR_REGSET_IN(available, reg)) {
						reg = _get_free_reg(data.regs[j].type, available);
						if (UNEXPECTED(reg == IR_REG_NONE)) {
							reg = _get_free_reg2(ctx, data.regs[j].type, &data, j);
						}
					}
					for (n = data.regs[j].start; n < data.regs[j].end; n++) {
						IR_REGSET_INCL(data.clobbered[n], reg);
					}
#if IR_X86_I64
					if (data.regs[j].type == IR_I64 || data.regs[j].type == IR_U64) {
						IR_REGSET_EXCL(available, reg);
						if (reg2 == IR_REG_NONE || !IR_REGSET_IN(available, reg2)) {
							reg2 = _get_free_reg(data.regs[j].type, available);
							if (UNEXPECTED(reg2 == IR_REG_NONE)) {
								reg2 = _get_free_reg2(ctx, data.regs[j].type, &data, j);
							}
						}
						for (n = data.regs[j].start; n < data.regs[j].end; n++) {
							IR_REGSET_INCL(data.clobbered[n], reg2);
						}
						if (reg > reg2) {
							SWAP_REGS(reg, reg2);
						}
						reg = IR_REG_I64_PAIR(reg, reg2);
					}
#endif
					reg = reg | data.regs[j].flags;
					if (data.regs[j].op == 4 && insn->inputs_count < 4) {
						if (!ctx->tmp_regs) {
							ctx->tmp_regs = ir_mem_malloc(ctx->insns_count);
							memset(ctx->tmp_regs, -1, ctx->insns_count);
						}
						ctx->tmp_regs[data.regs[j].ref] = reg;
					} else if (!data.regs[j].root || ctx->regs[data.regs[j].ref][data.regs[j].op] == IR_REG_NONE) {
						ctx->regs[data.regs[j].ref][data.regs[j].op] = reg;
					} else if (ctx->regs[data.regs[j].ref][data.regs[j].op] != reg) {
						ctx->rules[data.regs[j].ref] |= IR_FUSED_REG;
						ir_set_fused_reg(ctx, data.regs[j].root, data.regs[j].ref, data.regs[j].op, reg);
					}
				}
			}

			n = ir_insn_len(insn);
			i += n;
			insn += n;
			rule += n;
		}
		if (bb->flags & IR_BB_DESSA_MOVES) {
#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
			ctx->regs[bb->end][0] = IR_REG_RAX;
			ctx->regs[bb->end][1] = IR_REG_XMM0;
#elif defined(IR_TARGET_AARCH64)
			ctx->regs[bb->end][0] = IR_REG_X0;
			ctx->regs[bb->end][1] = IR_REG_V0;
#else
# error "Unknown IR target"
#endif
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

	return 1;
}
