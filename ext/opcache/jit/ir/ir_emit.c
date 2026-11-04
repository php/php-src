/*
 * IR - Lightweight JIT Compilation Framework
 * (Native code generator based on DynAsm)
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
#ifndef _WIN32
# include <dlfcn.h>
#else
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <psapi.h>
#endif

#if defined(__linux__) || defined(__sun)
# include <alloca.h>
#endif

#define DASM_M_GROW(ctx, t, p, sz, need) \
  do { \
    size_t _sz = (sz), _need = (need); \
    if (_sz < _need) { \
      size_t _limit = sizeof(t) * DASM_SEC2POS(1); \
      if (_need > _limit) { \
        Dst_REF->status = DASM_S_NOMEM; \
        return; \
      } \
      if (_sz < 16) _sz = 16; \
      while (_sz < _need) _sz += _sz; \
      if (_sz > _limit) _sz = _limit; \
      (p) = (t *)ir_mem_realloc((p), _sz); \
      (sz) = _sz; \
    } \
  } while(0)

#define DASM_M_FREE(ctx, p, sz) ir_mem_free(p)

#ifdef IR_DEBUG
# define DASM_CHECKS
# define DASM_ABORT IR_ASSERT(0)
#endif

typedef struct _ir_copy {
	ir_type type;
	ir_reg  from;
	ir_reg  to;
} ir_copy;

#define IR_U32_HI IR_U64 /* type for dessa copy of the high 32-bit value of constant */

typedef struct _ir_dessa_copy {
	ir_type type;
	int32_t from; /* negative - constant ref, [0..IR_REG_NUM) - CPU reg, [IR_REG_NUM...) - memory slot */
	int32_t to;   /* [0..IR_REG_NUM) - CPU reg, [IR_REG_NUM...) - memory slot  */
} ir_dessa_copy;

const ir_proto_t *ir_call_proto(const ir_ctx *ctx, const ir_insn *insn)
{
	if (IR_IS_CONST_REF(insn->op2)) {
		const ir_insn *func = &ctx->ir_base[insn->op2];

		if (func->op == IR_FUNC || func->op == IR_FUNC_ADDR) {
			if (func->proto) {
				return (const ir_proto_t *)ir_get_str(ctx, func->proto);
			}
		}
	} else if (ctx->ir_base[insn->op2].op == IR_PROTO) {
		return (const ir_proto_t *)ir_get_str(ctx, ctx->ir_base[insn->op2].op2);
	}
	return NULL;
}

IR_ALWAYS_INLINE uint32_t ir_rule(const ir_ctx *ctx, ir_ref ref)
{
	IR_ASSERT(!IR_IS_CONST_REF(ref));
	return ctx->rules[ref];
}

IR_ALWAYS_INLINE bool ir_in_same_block(ir_ctx *ctx, ir_ref ref)
{
	return ref > ctx->bb_start;
}


static ir_reg ir_get_param_reg(const ir_ctx *ctx, ir_ref ref)
{
	ir_use_list *use_list = &ctx->use_lists[1];
	int i;
	ir_ref use, *p;
	ir_insn *insn;
	int int_param = 0;
	int fp_param = 0;
#if IR_SIMD && defined(IR_TARGET_X86)
	int vector_param = 0;
#endif
	const ir_call_conv_dsc *cc = ir_get_call_conv_dsc(ctx->flags);

	for (i = use_list->count, p = &ctx->use_edges[use_list->refs]; i > 0; p++, i--) {
		use = *p;
		insn = &ctx->ir_base[use];
		if (insn->op == IR_PARAM) {
			if (IR_IS_TYPE_INT(insn->type)) {
				if (use == ref) {
					if (ctx->value_params && ctx->value_params[insn->op3 - 1].align && cc->pass_struct_by_val) {
						/* struct passed by value on stack */
						return IR_REG_NONE;
					} else if (int_param < cc->int_param_regs_count) {
#if IR_X86_I64
						if (insn->type == IR_I64 || insn->type == IR_U64) {
							return IR_REG_NONE;
						}
#endif
						return cc->int_param_regs[int_param];
					} else {
						return IR_REG_NONE;
					}
				} else if (ctx->value_params && ctx->value_params[insn->op3 - 1].align && cc->pass_struct_by_val) {
					/* struct passed by value on stack */
					continue;
				}
				int_param++;
				if (cc->shadow_param_regs) {
					fp_param++;
				}
#if IR_X86_I64
				if (insn->type == IR_I64 || insn->type == IR_U64) {
					int_param++;
					if (cc->shadow_param_regs) {
						fp_param++;
					}
				}
#endif
#if IR_SIMD && defined(IR_TARGET_X86)
			} else if (IR_IS_TYPE_VECTOR(insn->type)) {
				if (use == ref) {
					if (vector_param < cc->vector_param_regs_count) {
						return cc->vector_param_regs[vector_param];
					} else {
						return IR_REG_NONE;
					}
				}
				vector_param++;
#endif
			} else {
				IR_ASSERT(IR_IS_TYPE_FP(insn->type) || IR_IS_TYPE_VECTOR(insn->type));
				if (use == ref) {
					if (fp_param < cc->fp_param_regs_count) {
						return cc->fp_param_regs[fp_param];
					} else {
						return IR_REG_NONE;
					}
				}
				fp_param++;
				if (cc->shadow_param_regs) {
					int_param++;
				}
			}
		}
	}
	return IR_REG_NONE;
}

/* This remaps spill slot of the VAR, where PARAM is stored, to the PARAMs slot.
 * This eliminates copying, but more important, this fixes va_start() compilation
 * on systems, where it's defined through address calculation macro (e.g. MSVC x86).
 */
static void ir_remap_param_spill_slot(ir_ctx *ctx, ir_ref param, int32_t spill_slot)
{
	ir_use_list *param_use_list = &ctx->use_lists[param];

	if (param_use_list->count >= 1) {
		ir_ref param_store = ctx->use_edges[param_use_list->refs];
		ir_insn *param_store_insn = &ctx->ir_base[param_store];

		if (param_store_insn->op == IR_VSTORE && (ctx->rules[param_store] & IR_SKIPPED)) {
			ir_ref var = param_store_insn->op2;

			IR_ASSERT(param_store_insn->op3 == param);
			IR_ASSERT(ctx->ir_base[var].op == IR_VAR);

			/* Remap all related VADDR and VLOAD */
			ir_use_list *var_use_list = &ctx->use_lists[var];
			ir_ref n = var_use_list->count;
			ir_ref *p = &ctx->use_edges[var_use_list->refs];
			int32_t old_slot = IR_VREG_TO_STACK_SLOT(ctx->vregs[var]);

			for (; n > 0; p++, n--) {
				ir_ref use = *p;
				ir_insn *insn = &ctx->ir_base[use];

				if (insn->op == IR_VADDR) {
					if (ctx->rules[use] == IR_STATIC_ALLOCA) {
						IR_ASSERT(ctx->vregs[use] == ctx->vregs[var]);
						ctx->vregs[use] = IR_STACK_SLOT_TO_VREG(spill_slot);
					}
				} else if (insn->op == IR_VLOAD || insn->op == IR_VLOAD_v) {
					if (ctx->vregs[use] > 0) {
						ir_live_interval *ival = ctx->live_intervals[ctx->vregs[use]];
						if (ival && ival->stack_spill_pos == old_slot) {
							ival->stack_spill_pos = spill_slot;
						}
					}
				} else if (insn->op == IR_VSTORE || insn->op == IR_VSTORE_v) {
					if (insn->op3 > 0 && ctx->vregs[insn->op3] > 0) {
						ir_live_interval *ival = ctx->live_intervals[ctx->vregs[insn->op3]];
						if (ival && ival->stack_spill_pos == old_slot) {
							ival->stack_spill_pos = spill_slot;
						}
					}
				}
			}

			/* Remap VAR itself */
			ctx->vregs[var] = IR_STACK_SLOT_TO_VREG(spill_slot);
		}
	}
}

static int ir_get_args_regs(const ir_ctx *ctx, const ir_insn *insn, const ir_call_conv_dsc *cc, int8_t *regs)
{
	int j, n;
	ir_type type;
	int int_param = 0;
	int fp_param = 0;
#if IR_SIMD && defined(IR_TARGET_X86)
	int vector_param = 0;
#endif
	int count = 0;

	n = insn->inputs_count;
	n = IR_MIN(n, IR_MAX_REG_ARGS + 2);
	for (j = 3; j <= n; j++) {
		ir_insn *arg = &ctx->ir_base[ir_insn_op(insn, j)];
		type = arg->type;
		if (IR_IS_TYPE_INT(type)) {
			if (int_param < cc->int_param_regs_count && arg->op != IR_ARGVAL) {
#if IR_X86_I64
				if (type == IR_I64 || type == IR_U64) {
					regs[j] = IR_REG_NONE;
					count = j + 1;
					int_param += 2;
					if (cc->shadow_param_regs) {
						fp_param += 2;
					}
					continue;
				}
#endif
				regs[j] = cc->int_param_regs[int_param];
				count = j + 1;
				int_param++;
				if (cc->shadow_param_regs) {
					fp_param++;
				}
			} else {
				regs[j] = IR_REG_NONE;
			}
#if IR_SIMD && defined(IR_TARGET_X86)
		} else if (IR_IS_TYPE_VECTOR(type)) {
			if (vector_param < cc->vector_param_regs_count) {
				regs[j] = cc->vector_param_regs[vector_param];
				count = j + 1;
				vector_param++;
			} else {
				regs[j] = IR_REG_NONE;
			}
#endif
		} else {
			IR_ASSERT(IR_IS_TYPE_FP(type) || IR_IS_TYPE_VECTOR(type));
			if (fp_param < cc->fp_param_regs_count) {
				regs[j] = cc->fp_param_regs[fp_param];
				count = j + 1;
				fp_param++;
				if (cc->shadow_param_regs) {
					int_param++;
				}
			} else {
				regs[j] = IR_REG_NONE;
			}
		}
	}
	return count;
}

static bool ir_is_same_mem_var(const ir_ctx *ctx, ir_ref r1, int32_t offset)
{
	ir_live_interval *ival1;
	int32_t o1;

	if (IR_IS_CONST_REF(r1)) {
		return 0;
	}

	IR_ASSERT(ctx->vregs[r1] > 0);
	ival1 = ctx->live_intervals[ctx->vregs[r1]];
	IR_ASSERT(ival1);
	o1 = ival1->stack_spill_pos;
	IR_ASSERT(o1 != -1);
	return o1 == offset;
}

void *ir_resolve_sym_name(const char *name)
{
	void *addr;

#ifndef _WIN32
	void *handle = NULL;
# ifdef RTLD_DEFAULT
	handle = RTLD_DEFAULT;
# endif
	addr = dlsym(handle, name);
#else
	HMODULE mods[256];
	DWORD cbNeeded;
	uint32_t i = 0;

	addr = NULL;

	EnumProcessModules(GetCurrentProcess(), mods, sizeof(mods), &cbNeeded);

	while(i < (cbNeeded / sizeof(HMODULE))) {
		addr = GetProcAddress(mods[i], name);
		if (addr) {
			return addr;
		}
		i++;
	}
#endif
	return addr;
}

#ifdef IR_SNAPSHOT_HANDLER_DCL
	IR_SNAPSHOT_HANDLER_DCL();
#endif

#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
static void* ir_sym_addr(ir_ctx *ctx, const ir_insn *addr_insn)
{
	void *addr;

	if (ctx->loader && ctx->loader->resolve_sym_name) {
		addr = ctx->loader->resolve_sym_name(ctx->loader, ctx, addr_insn->val.name, IR_RESOLVE_SYM_SILENT);
	} else {
		const char *name = ir_get_str(ctx, addr_insn->val.name);
		addr = ir_resolve_sym_name(name);
	}
	return addr;
}
#endif

static void* ir_sym_val(ir_ctx *ctx, const ir_insn *addr_insn)
{
	void *addr;

	if (ctx->loader && ctx->loader->resolve_sym_name) {
		addr = ctx->loader->resolve_sym_name(ctx->loader, ctx, addr_insn->val.name, addr_insn->op == IR_FUNC ? IR_RESOLVE_SYM_ADD_THUNK : 0);
	} else {
		const char *name = ir_get_str(ctx, addr_insn->val.name);
		addr = ir_resolve_sym_name(name);
	}
	IR_ASSERT(addr);
	return addr;
}

static void *ir_call_addr(ir_ctx *ctx, ir_insn *insn, ir_insn *addr_insn)
{
	void *addr;

	IR_ASSERT(addr_insn->type == IR_ADDR);
	if (addr_insn->op == IR_FUNC) {
		addr = ir_sym_val(ctx, addr_insn);
	} else {
		IR_ASSERT(addr_insn->op == IR_ADDR || addr_insn->op == IR_FUNC_ADDR);
		addr = (void*)addr_insn->val.addr;
	}
	return addr;
}

static void *ir_jmp_addr(ir_ctx *ctx, ir_insn *insn, ir_insn *addr_insn)
{
	void *addr = ir_call_addr(ctx, insn, addr_insn);

#ifdef IR_SNAPSHOT_HANDLER
	if (ctx->ir_base[insn->op1].op == IR_SNAPSHOT) {
		addr = IR_SNAPSHOT_HANDLER(ctx, insn->op1, &ctx->ir_base[insn->op1], addr);
	}
#endif
	return addr;
}

void ir_set_fused_reg(ir_ctx *ctx, ir_ref root, ir_ref ref, ir_ref op, int8_t reg)
{
	char key[10];
	ir_ref ref_and_op = ref * sizeof(ir_ref) + op;

	if (!ctx->fused_regs) {
		ctx->fused_regs = ir_mem_malloc(sizeof(ir_strtab));
		ir_strtab_init(ctx->fused_regs, 8, 128);
	}
	memcpy(key, &root, sizeof(ir_ref));
	memcpy(key + 4, &ref_and_op, sizeof(ir_ref));
	ir_strtab_set(ctx->fused_regs, key, 8, 0x10000000 | (uint8_t)reg);
}

static int8_t ir_get_fused_reg_ex(ir_ctx *ctx, ir_ref root, ir_ref ref_and_op)
{
	if (ctx->fused_regs) {
		char key[10];
		ir_ref val;

		memcpy(key, &root, sizeof(ir_ref));
		memcpy(key + 4, &ref_and_op, sizeof(ir_ref));

		val = ir_strtab_find(ctx->fused_regs, key, 8);
		if (val) {
			return val;
		}
	}
	return ((int8_t*)ctx->regs)[ref_and_op];
}

static int8_t ir_get_fused_reg(ir_ctx *ctx, ir_ref root, ir_ref ref, ir_ref op)
{
	return ir_get_fused_reg_ex(ctx, root, ref * sizeof(ir_ref) + op);
}

#if defined(__GNUC__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Warray-bounds"
# pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif

#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
# include "dynasm/dasm_proto.h"
# include "dynasm/dasm_x86.h"
#elif defined(IR_TARGET_AARCH64)
# include "dynasm/dasm_proto.h"
static int ir_add_veneer(dasm_State *Dst, void *buffer, uint32_t ins, int *b, uint32_t *cp, ptrdiff_t offset);
# define DASM_ADD_VENEER ir_add_veneer
# include "dynasm/dasm_arm64.h"
#else
# error "Unknown IR target"
#endif

#if defined(__GNUC__)
# pragma GCC diagnostic pop
#endif

/* Forward Declarations */
static void ir_emit_osr_entry_loads(ir_ctx *ctx, int b, ir_block *bb);
static int ir_parallel_copy(ir_ctx *ctx, ir_copy *copies, int count, ir_reg tmp_reg, ir_reg tmp_fp_reg);
static void ir_emit_dessa_moves(ir_ctx *ctx, int b, ir_block *bb);

typedef struct _ir_common_backend_data {
    ir_reg_alloc_data  ra_data;
	dasm_State        *dasm_state;
	ir_bitset          emit_constants;
} ir_common_backend_data;

static int ir_get_const_label(ir_ctx *ctx, ir_ref ref)
{
	ir_common_backend_data *data = ctx->data;
	int label = ctx->cfg_blocks_count - ref;

	IR_ASSERT(IR_IS_CONST_REF(ref));
	ir_bitset_incl(data->emit_constants, -ref);
	return label;
}

#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
# include <ir_emit_x86.h>
#elif defined(IR_TARGET_AARCH64)
# include <ir_emit_aarch64.h>
#else
# error "Unknown IR target"
#endif

static IR_NEVER_INLINE void ir_emit_osr_entry_loads(ir_ctx *ctx, int b, ir_block *bb)
{
	ir_list *list = (ir_list*)ctx->osr_entry_loads;
	int pos = 0, count, i;
	ir_ref ref;

	IR_ASSERT(ctx->binding);
	IR_ASSERT(list);
	while (1) {
		i = ir_list_at(list, pos);
		if (b == i) {
			break;
		}
		IR_ASSERT(i != 0); /* end marker */
		pos++;
		count = ir_list_at(list, pos);
		pos += count + 1;
	}
	pos++;
	count = ir_list_at(list, pos);
	pos++;

	for (i = 0; i < count; i++, pos++) {
		ref = ir_list_at(list, pos);
		IR_ASSERT(ref >= 0 && ctx->vregs[ref] > 0 && ctx->live_intervals[ctx->vregs[ref]]);
		if (!(ctx->live_intervals[ctx->vregs[ref]]->flags & IR_LIVE_INTERVAL_SPILLED)) {
			/* not spilled */
			ir_reg reg = ctx->live_intervals[ctx->vregs[ref]]->reg;
			ir_type type = ctx->ir_base[ref].type;
			int32_t offset = -ir_binding_find(ctx, ref);

			IR_ASSERT(offset > 0);
			ir_emit_load_mem(ctx, type, reg, IR_MEM_BO(ctx->spill_base, offset));
		} else {
			IR_ASSERT(ctx->live_intervals[ctx->vregs[ref]]->flags & IR_LIVE_INTERVAL_SPILL_SPECIAL);
		}
	}
}

/*
 * Parallel copy sequentialization algorithm
 *
 * The implementation is based on algorithm 1 desriebed in
 * "Revisiting Out-of-SSA Translation for Correctness, Code Quality and Efficiency",
 * Benoit Boissinot, Alain Darte, Fabrice Rastello, Benoit Dupont de Dinechin, Christophe Guillon.
 * 2009 International Symposium on Code Generation and Optimization, Seattle, WA, USA, 2009,
 * pp. 114-125, doi: 10.1109/CGO.2009.19.
 */
static int ir_parallel_copy(ir_ctx *ctx, ir_copy *copies, int count, ir_reg tmp_reg, ir_reg tmp_fp_reg)
{
	int i;
	int8_t *pred, *loc, *types;
	ir_reg to, from;
	ir_type type;
	ir_regset todo, ready, srcs;

	if (count == 1) {
		to = copies[0].to;
		from = copies[0].from;
		IR_ASSERT(from != to);
		type = copies[0].type;
		if (IR_IS_TYPE_INT(type)) {
			ir_emit_mov(ctx, type, to, from);
		} else {
			ir_emit_fp_mov(ctx, type, to, from);
		}
		return 1;
	}

	loc = alloca(IR_REG_NUM * 3 * sizeof(int8_t));
	pred = loc + IR_REG_NUM;
	types = pred + IR_REG_NUM;
	todo = IR_REGSET_EMPTY;
	srcs = IR_REGSET_EMPTY;

	for (i = 0; i < count; i++) {
		from = copies[i].from;
		to = copies[i].to;
		IR_ASSERT(from != to);
		IR_REGSET_INCL(srcs, from);
		loc[from] = from;
		pred[to] = from;
		types[from] = copies[i].type;
		IR_ASSERT(!IR_REGSET_IN(todo, to));
		IR_REGSET_INCL(todo, to);
	}

	ready = IR_REGSET_DIFFERENCE(todo, srcs);

	if (ready == todo) {
		for (i = 0; i < count; i++) {
			from = copies[i].from;
			to = copies[i].to;
			IR_ASSERT(from != to);
			type = copies[i].type;
			if (IR_IS_TYPE_INT(type)) {
				ir_emit_mov(ctx, type, to, from);
			} else {
				ir_emit_fp_mov(ctx, type, to, from);
			}
		}
		return 1;
	}

	/* temporary registers can't be the same as some of the destinations */
	IR_ASSERT(tmp_reg == IR_REG_NONE || !IR_REGSET_IN(todo, tmp_reg));
	IR_ASSERT(tmp_fp_reg == IR_REG_NONE || !IR_REGSET_IN(todo, tmp_fp_reg));

	/* first we resolve all "windmill blades" - trees (this doesn't requre temporary registers) */
	while (ready != IR_REGSET_EMPTY) {
		ir_reg r;

		to = ir_regset_pop_first(&ready);
		from = pred[to];
		r = loc[from];
		type = types[from];
		if (IR_IS_TYPE_INT(type)) {
			ir_emit_mov_ext(ctx, type, to, r);
		} else {
			ir_emit_fp_mov(ctx, type, to, r);
		}
		IR_REGSET_EXCL(todo, to);
		loc[from] = to;
		if (from == r && IR_REGSET_IN(todo, from)) {
			IR_REGSET_INCL(ready, from);
		}
	}
	if (todo == IR_REGSET_EMPTY) {
		return 1;
	}

	/* at this point the sources that are the same as temoraries are already moved */
	IR_ASSERT(tmp_reg == IR_REG_NONE || !IR_REGSET_IN(srcs, tmp_reg) || pred[loc[tmp_reg]] == tmp_reg);
	IR_ASSERT(tmp_fp_reg == IR_REG_NONE || !IR_REGSET_IN(srcs, tmp_fp_reg) || pred[loc[tmp_fp_reg]] == tmp_fp_reg);

	/* now we resolve all "windmill axles" - cycles (this reuires temporary registers) */
	while (todo != IR_REGSET_EMPTY) {
		to = ir_regset_pop_first(&todo);
		from = pred[to];
		IR_ASSERT(to != loc[from]);
		type = types[from];
		if (IR_IS_TYPE_INT(type)) {
#ifdef IR_HAVE_SWAP_INT
			if (pred[from] == to) {
				if (ir_type_size[types[to]] > ir_type_size[type]) {
					type = types[to];
				}
				ir_emit_swap(ctx, type, to, from);
				IR_REGSET_EXCL(todo, from);
				loc[to] = from;
				loc[from] = to;
				continue;
			}
#endif
			IR_ASSERT(tmp_reg != IR_REG_NONE);
			IR_ASSERT(tmp_reg >= IR_REG_GP_FIRST && tmp_reg <= IR_REG_GP_LAST);
			ir_emit_mov(ctx, type, tmp_reg, to);
			loc[to] = tmp_reg;
		} else {
#ifdef IR_HAVE_SWAP_FP
			if (pred[from] == to && types[to] == type) {
				ir_emit_swap_fp(ctx, type, to, from);
				IR_REGSET_EXCL(todo, from);
				loc[to] = from;
				loc[from] = to;
				continue;
			}
#endif
			IR_ASSERT(tmp_fp_reg != IR_REG_NONE);
			IR_ASSERT(tmp_fp_reg >= IR_REG_FP_FIRST && tmp_fp_reg <= IR_REG_FP_LAST);
			ir_emit_fp_mov(ctx, type, tmp_fp_reg, to);
			loc[to] = tmp_fp_reg;
		}
		while (1) {
			ir_reg r;

			from = pred[to];
			r = loc[from];
			type = types[from];
			if (IR_IS_TYPE_INT(type)) {
				ir_emit_mov_ext(ctx, type, to, r);
			} else {
				ir_emit_fp_mov(ctx, type, to, r);
			}
			IR_REGSET_EXCL(todo, to);
			loc[from] = to;
			if (from == r && IR_REGSET_IN(todo, from)) {
				to = from;
			} else {
				break;
			}
		}
	}

	return 1;
}

static void ir_emit_dessa_move(ir_ctx *ctx, ir_mem *mem_slots,
                               ir_type type, ir_ref to, ir_ref from,
                               ir_reg tmp_reg, ir_reg tmp_fp_reg)
{
	ir_mem mem_from, mem_to;

	IR_ASSERT(from != to);
	if (to < IR_REG_NUM) {
		if (IR_IS_CONST_REF(from)) {
			if (-from < ctx->consts_count) {
#if IR_X86_I64
				if (type == IR_U32_HI) {
					ir_emit_load_i64_hi(ctx, to, from);
				} else
#endif
				/* constant reference */
				ir_emit_load(ctx, type, to, from);
			} else {
				/* local variable address */
				ir_load_local_addr(ctx, to, -from - ctx->consts_count);
			}
		} else if (from < IR_REG_NUM) {
			if (IR_IS_TYPE_INT(type)) {
				ir_emit_mov(ctx, type, to, from);
			} else {
				ir_emit_fp_mov(ctx, type, to, from);
			}
		} else {
			mem_from = mem_slots[from - IR_REG_NUM];
			ir_emit_load_mem(ctx, type, to, mem_from);
		}
	} else {
		mem_to = mem_slots[to - IR_REG_NUM];
		if (IR_IS_CONST_REF(from)) {
			if (-from < ctx->consts_count) {
				/* constant reference */
#if IR_X86_I64
				if (type == IR_U32_HI) {
#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
					ir_emit_store_mem_imm(ctx, IR_U32, mem_to, ctx->ir_base[from].val.u32_hi);
#else
					IR_ASSERT(tmp_reg != IR_REG_NONE);
					ir_emit_load_i64_hi(ctx, tmp_reg, from);
					ir_emit_store_mem(ctx, IR_U32, mem_to, tmp_reg);
#endif
					return;
				} else
#endif
#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
				if (IR_IS_TYPE_INT(type)
				 && !IR_IS_SYM_CONST(ctx->ir_base[from].op)
				 && (ir_type_size[type] != 8 || IR_IS_SIGNED_32BIT(ctx->ir_base[from].val.i64))) {
					ir_emit_store_mem_imm(ctx, type, mem_to, ctx->ir_base[from].val.i32);
					return;
				}
#endif
				ir_reg tmp = IR_IS_TYPE_INT(type) ? tmp_reg : tmp_fp_reg;
				IR_ASSERT(tmp != IR_REG_NONE);
				ir_emit_load(ctx, type, tmp, from);
				ir_emit_store_mem(ctx, type, mem_to, tmp);
			} else {
				/* local variable address */
				IR_ASSERT(IR_IS_TYPE_INT(type));
				IR_ASSERT(tmp_reg != IR_REG_NONE);
				ir_load_local_addr(ctx, tmp_reg, -from - ctx->consts_count);
				ir_emit_store_mem(ctx, type, mem_to, tmp_reg);
			}
		} else if (from < IR_REG_NUM) {
			ir_emit_store_mem(ctx, type, mem_to, from);
		} else {
			mem_from = mem_slots[from - IR_REG_NUM];
			IR_ASSERT(IR_MEM_VAL(mem_to) != IR_MEM_VAL(mem_from));
			ir_reg tmp = IR_IS_TYPE_INT(type) ?  tmp_reg : tmp_fp_reg;
			IR_ASSERT(tmp != IR_REG_NONE);
			ir_emit_load_mem(ctx, type, tmp, mem_from);
			ir_emit_store_mem(ctx, type, mem_to, tmp);
		}
	}
}

IR_ALWAYS_INLINE void ir_dessa_resolve_cycle(ir_ctx *ctx, ir_mem *mem_slots, int32_t *pred, int32_t *loc,
                                             int8_t *types, ir_bitset todo, int32_t root,
                                             ir_reg tmp_reg, ir_reg tmp_fp_reg)
{
	ir_ref from;
	ir_mem tmp_spill_slot;
	ir_type type;
	int32_t to = root;

	IR_MEM_VAL(tmp_spill_slot) = 0;
	IR_ASSERT(!IR_IS_CONST_REF(to));
	from = pred[to];
	type = types[from];
	IR_ASSERT(!IR_IS_CONST_REF(from));
	IR_ASSERT(from != to);
	IR_ASSERT(loc[from] == from);

	if (IR_IS_TYPE_INT(type)) {
#ifdef IR_HAVE_SWAP_INT
		if (pred[from] == to && to < IR_REG_NUM && from < IR_REG_NUM) {
			/* a simple cycle from 2 elements */
			if (ir_type_size[types[to]] > ir_type_size[type]) {
				type = types[to];
			}
			ir_emit_swap(ctx, type, to, from);
			ir_bitset_excl(todo, from);
			ir_bitset_excl(todo, to);
			loc[to] = from;
			loc[from] = to;
			return;
		}
#endif
		IR_ASSERT(tmp_reg != IR_REG_NONE);
		IR_ASSERT(tmp_reg >= IR_REG_GP_FIRST && tmp_reg <= IR_REG_GP_LAST);
		loc[to] = tmp_reg;
		if (to < IR_REG_NUM) {
			ir_emit_mov(ctx, type, tmp_reg, to);
		} else {
			ir_emit_load_mem_int(ctx, type, tmp_reg, mem_slots[to - IR_REG_NUM]);
		}
	} else {
#ifdef IR_HAVE_SWAP_FP
		if (pred[from] == to && to < IR_REG_NUM && from < IR_REG_NUM && types[to] == type) {
			/* a simple cycle from 2 elements */
			ir_emit_swap_fp(ctx, type, to, from);
			IR_REGSET_EXCL(todo, from);
			IR_REGSET_EXCL(todo, to);
			loc[to] = from;
			loc[from] = to;
			return;
		}
#endif
		IR_ASSERT(tmp_fp_reg != IR_REG_NONE);
		IR_ASSERT(tmp_fp_reg >= IR_REG_FP_FIRST && tmp_fp_reg <= IR_REG_FP_LAST);
		loc[to] = tmp_fp_reg;
		if (to < IR_REG_NUM) {
			ir_emit_fp_mov(ctx, type, tmp_fp_reg, to);
		} else {
			ir_emit_load_mem_fp(ctx, type, tmp_fp_reg, mem_slots[to - IR_REG_NUM]);
		}
	}

	while (1) {
		int32_t r;

		from = pred[to];
		r = loc[from];
		type = types[to];

		if (from == root) break;

		/* Memory to memory move inside an isolated or "blocked" cycle requres an additional temporary register */
		if (to >= IR_REG_NUM && r >= IR_REG_NUM) {
			ir_reg tmp = IR_IS_TYPE_INT(type) ?  tmp_reg : tmp_fp_reg;

			if (!IR_MEM_VAL(tmp_spill_slot)) {
				/* Free a register, saving it in a temporary spill slot */
				tmp_spill_slot = IR_MEM_BO(IR_REG_STACK_POINTER, -16);
				ir_emit_store_mem(ctx, type, tmp_spill_slot, tmp);
			}
			ir_emit_dessa_move(ctx, mem_slots, type, to, r, tmp_reg, tmp_fp_reg);
		} else {
			ir_emit_dessa_move(ctx, mem_slots, type, to, r, IR_REG_NONE, IR_REG_NONE);
		}
		ir_bitset_excl(todo, to);
		loc[from] = to;
		to = from;
	}

	type = types[to];
	if (IR_MEM_VAL(tmp_spill_slot)) {
		ir_emit_load_mem(ctx, type, IR_IS_TYPE_INT(type) ? tmp_reg : tmp_fp_reg, tmp_spill_slot);
	}
	ir_emit_dessa_move(ctx, mem_slots, type, to, loc[from], IR_REG_NONE, IR_REG_NONE);
	ir_bitset_excl(todo, to);
	loc[from] = to;
}

static int ir_dessa_parallel_copy(ir_ctx *ctx, ir_dessa_copy *copies, int count,
                                  ir_mem *mem_slots, int mem_slots_count,
                                  ir_reg tmp_reg, ir_reg tmp_fp_reg)
{
	int i;
	int32_t *pred, *loc, to, from;
	int8_t *types;
	ir_type type;
	uint32_t len;
	ir_bitset todo, ready, srcs, visited;

	if (count == 1) {
		to = copies[0].to;
		from = copies[0].from;
		IR_ASSERT(from != to);
		type = copies[0].type;
		ir_emit_dessa_move(ctx, mem_slots, type, to, from, tmp_reg, tmp_fp_reg);
		return 1;
	}

	len = IR_REG_NUM + mem_slots_count + 1;
	loc = ir_mem_malloc(len * 2 * sizeof(int32_t) + len * sizeof(int8_t));
	pred = loc + len;
	types = (int8_t*)(pred + len);

	len = ir_bitset_len(len);
	todo = ir_mem_malloc(len * IR_BITSET_BITS / 8 * 4);
	memset(todo, 0, len * IR_BITSET_BITS / 8 * 2);
	srcs = todo + len;
	ready = srcs + len;
	visited = ready + len;

	for (i = 0; i < count; i++) {
		from = copies[i].from;
		to = copies[i].to;
		IR_ASSERT(from != to);
		if (!IR_IS_CONST_REF(from)) {
			ir_bitset_incl(srcs, from);
			loc[from] = from;
		}
		pred[to] = from;
		types[to] = copies[i].type;
		IR_ASSERT(!ir_bitset_in(todo, to));
		ir_bitset_incl(todo, to);
	}

	/* temporary registers can't be the same as some of the sources */
	IR_ASSERT(tmp_reg == IR_REG_NONE || !ir_bitset_in(srcs, tmp_reg));
	IR_ASSERT(tmp_fp_reg == IR_REG_NONE || !ir_bitset_in(srcs, tmp_fp_reg));

	/* first we resolve all "windmill blades" - trees, that don't set temporary registers */
	ir_bitset_copy(ready, todo, len);
	ir_bitset_difference(ready, srcs, len);
	if (tmp_reg != IR_REG_NONE) {
		ir_bitset_excl(ready, tmp_reg);
	}
	if (tmp_fp_reg != IR_REG_NONE) {
		ir_bitset_excl(ready, tmp_fp_reg);
	}
	while ((to = ir_bitset_pop_first(ready, len)) >= 0) {
		ir_bitset_excl(todo, to);
		type = types[to];
		from = pred[to];
		if (IR_IS_CONST_REF(from)) {
			ir_emit_dessa_move(ctx, mem_slots, type, to, from, tmp_reg, tmp_fp_reg);
		} else {
			int32_t r = loc[from];
			ir_emit_dessa_move(ctx, mem_slots, type, to, r, tmp_reg, tmp_fp_reg);
			loc[from] = to;
			if (from == r && ir_bitset_in(todo, from) && from != tmp_reg && from != tmp_fp_reg) {
				ir_bitset_incl(ready, from);
			}
		}
	}

	/* then we resolve all "windmill axles" - cycles (this requres temporary registers) */
	ir_bitset_copy(ready, todo, len);
	ir_bitset_intersection(ready, srcs, len);
	while ((to = ir_bitset_first(ready, len)) >= 0) {
		ir_bitset_clear(visited, len);
		ir_bitset_incl(visited, to);
		to = pred[to];
		while (!IR_IS_CONST_REF(to) && ir_bitset_in(ready, to)) {
			to = pred[to];
			IR_ASSERT(!IR_IS_CONST_REF(to));
			if (ir_bitset_in(visited, to)) {
				/* We found a cycle. Resolve it. */
				ir_bitset_incl(visited, to);
				ir_dessa_resolve_cycle(ctx, mem_slots, pred, loc, types, todo, to, tmp_reg, tmp_fp_reg);
				break;
			}
			ir_bitset_incl(visited, to);
		}
		ir_bitset_difference(ready, visited, len);
	}

	/* finally we resolve remaining "windmill blades" - trees that set temporary registers */
	ir_bitset_copy(ready, todo, len);
	ir_bitset_difference(ready, srcs, len);
	while ((to = ir_bitset_pop_first(ready, len)) >= 0) {
		ir_bitset_excl(todo, to);
		type = types[to];
		from = pred[to];
#ifdef IR_DEBUG
		/* If destionation is set, it can't be used as temporary anymore */
		if (to == tmp_reg) {
			tmp_reg = IR_REG_NONE;
		}
		if (to == tmp_fp_reg) {
			tmp_fp_reg = IR_REG_NONE;
		}
#endif
		if (IR_IS_CONST_REF(from)) {
			ir_emit_dessa_move(ctx, mem_slots, type, to, from, tmp_reg, tmp_fp_reg);
		} else {
			int32_t r = loc[from];
			ir_emit_dessa_move(ctx, mem_slots, type, to, r, tmp_reg, tmp_fp_reg);
			loc[from] = to;
			if (from == r && ir_bitset_in(todo, from)) {
				ir_bitset_incl(ready, from);
			}
		}
	}

	IR_ASSERT(ir_bitset_empty(todo, len));

	ir_mem_free(todo);
	ir_mem_free(loc);
	return 1;
}

static uint32_t _find_mem_slot(ir_mem *mem_slots, uint32_t *mem_slots_count, ir_mem mem)
{
	uint32_t j, n = *mem_slots_count;

	for (j = 0; j < n; j++) {
		if (IR_MEM_VAL(mem_slots[j]) == IR_MEM_VAL(mem)) return j;
	}
	mem_slots[n] = mem;
	*mem_slots_count = n + 1;
	return n;
}

static bool ir_dessa_needs_spill_load(ir_ctx *ctx, ir_use_list *use_list, ir_ref skip, int op_num, ir_reg reg)
{
	ir_ref *p, n, use;
	ir_insn *insn;

	for (n = use_list->count, p = &ctx->use_edges[use_list->refs]; n > 0; p++, n--) {
		use = *p;
		if (use == skip) continue;
		insn = &ctx->ir_base[use];
		if (insn->op == IR_PHI) {
			ir_reg src = ir_get_alocated_reg(ctx, use, op_num);

			if (IR_REG_NUM(src) == reg) {
				/* the same input register used twice */
				return 1;
			}
		}
	}
	return 0;
}

static void ir_emit_dessa_moves(ir_ctx *ctx, int b, ir_block *bb)
{
	uint32_t succ, k, n = 0;
	ir_block *succ_bb;
	ir_use_list *use_list;
	ir_ref i, *p;
	ir_dessa_copy *copies;
	ir_mem *mem_slots;
	uint32_t mem_slots_count = 0;
	ir_reg tmp_reg = ctx->regs[bb->end][0];
	ir_reg tmp_fp_reg = ctx->regs[bb->end][1];

	IR_ASSERT(bb->successors_count == 1);
	succ = ctx->cfg_edges[bb->successors];
	succ_bb = &ctx->cfg_blocks[succ];
	IR_ASSERT(succ_bb->predecessors_count > 1);
	use_list = &ctx->use_lists[succ_bb->start];
	k = ir_phi_input_number(ctx, succ_bb, b);

#if IR_X86_I64
	copies = alloca((use_list->count - 1) * 2 * sizeof(ir_dessa_copy));
	mem_slots = alloca((use_list->count - 1) * 2 * 2 * sizeof(ir_mem));
#else
	copies = alloca((use_list->count - 1) * sizeof(ir_dessa_copy));
	mem_slots = alloca((use_list->count - 1) * 2 * sizeof(ir_mem));
#endif

	for (i = use_list->count, p = &ctx->use_edges[use_list->refs]; i > 0; p++, i--) {
		ir_ref ref = *p;
		ir_insn *insn = &ctx->ir_base[ref];

		if (insn->op == IR_PHI && ctx->vregs[ref] > 0) {
			ir_ref input = ir_insn_op(insn, k);
			ir_reg src = ir_get_alocated_reg(ctx, ref, k);
			ir_reg dst = ctx->regs[ref][0];
			ir_ref from, to;
#if IR_X86_I64
			ir_reg src_hi = IR_REG_NONE, dst_hi = IR_REG_NONE;
#endif

			IR_ASSERT(dst == IR_REG_NONE || !IR_REG_SPILLED(dst));
			if (IR_IS_CONST_REF(input)) {
				from = input;
			} else if (ir_rule(ctx, input) == IR_STATIC_ALLOCA) {
				/* encode local variable address */
				from = -(ctx->consts_count + input);
			} else if (src != IR_REG_NONE
			 && (!IR_REG_SPILLED(src)
			  || ir_dessa_needs_spill_load(ctx, use_list, ref, k, IR_REG_NUM(src)))) {
				if (IR_REG_SPILLED(src)) {
					src = IR_REG_NUM(src);
#if IR_X86_I64
					if (insn->type == IR_I64 || insn->type == IR_U64) {
						ir_reg reg_hi = IR_REG_I64_HI(src);
						ir_reg reg_lo = IR_REG_I64_LO(src);
						ir_emit_load_i64_lo(ctx, reg_lo, input);
						ir_emit_load_i64_hi(ctx, reg_hi, input);
					} else
#endif
					ir_emit_load(ctx, insn->type, src, input);
				}
#if IR_X86_I64
				if (insn->type == IR_I64 || insn->type == IR_U64) {
					src_hi = IR_REG_I64_HI(src);
					src = IR_REG_I64_LO(src);
				}
#endif
				from = src;
			} else {
				ir_mem mem = ir_vreg_spill_slot(ctx, ctx->vregs[input]);

				from = IR_REG_NUM + _find_mem_slot(mem_slots, &mem_slots_count, mem);
			}
			if (dst != IR_REG_NONE) {
				IR_ASSERT(!IR_REG_SPILLED(dst));
#if IR_X86_I64
				if (insn->type == IR_I64 || insn->type == IR_U64) {
					dst_hi = IR_REG_I64_HI(dst);
					dst = IR_REG_I64_LO(dst);
				}
#endif
				to = dst;
			} else {
				ir_mem mem = ir_vreg_spill_slot(ctx, ctx->vregs[ref]);

				to = IR_REG_NUM + _find_mem_slot(mem_slots, &mem_slots_count, mem);
			}
#if IR_X86_I64
			if (insn->type == IR_I64 || insn->type == IR_U64) {
				if (from != to) {
					copies[n].type = IR_U32;
					copies[n].from = from;
					copies[n].to = to;
					n++;
				} else if (from >= IR_REG_NUM) {
					continue;
				}
				if (from < 0) {
					/* pass */
				} else if (from < IR_REG_NUM) {
					from = src_hi;
				} else {
					ir_mem mem = IR_MEM_I64_HI(mem_slots[from - IR_REG_NUM]);

					from = IR_REG_NUM + _find_mem_slot(mem_slots, &mem_slots_count, mem);
				}
				if (to < IR_REG_NUM) {
					to = dst_hi;
				} else {
					ir_mem mem = IR_MEM_I64_HI(mem_slots[to - IR_REG_NUM]);

					to = IR_REG_NUM + _find_mem_slot(mem_slots, &mem_slots_count, mem);
				}
				if (from != to) {
					copies[n].type = (from < 0) ? IR_U32_HI : IR_U32;
					copies[n].from = from;
					copies[n].to = to;
					n++;
				}
				continue;
			}
#endif
			if (to != from) {
				copies[n].type = insn->type;
				copies[n].from = from;
				copies[n].to = to;
				n++;
			}
		}
	}

	if (n > 0) {
		ir_dessa_parallel_copy(ctx, copies, n, mem_slots, mem_slots_count, tmp_reg, tmp_fp_reg);
	}
}

/* TAILCALL optimization */
static bool ir_may_be_local_addr(ir_ctx *ctx, ir_insn *insn)
{
	if (insn->op == IR_PARAM) return 0;

	return 1;
}

static bool ir_try_tailcall(ir_ctx *ctx, ir_ref ref, ir_insn *insn)
{
	ir_ref proto_ref = IR_UNUSED;
	const ir_proto_t *proto = NULL;
	const ir_call_conv_dsc *cc;
	int32_t params_stack_size, copy_stack;

	if (IR_IS_CONST_REF(insn->op2)) {
		const ir_insn *func = &ctx->ir_base[insn->op2];

		if (func->op == IR_FUNC && func->proto) {
			uint32_t rule = ir_match_builtin_call(ctx, func);

			if (rule) {
				ctx->rules[ref] = rule;
				return 0;
			}
			proto_ref = func->proto;
		} else if (func->op == IR_FUNC_ADDR) {
			proto_ref = func->proto;
		}
	} else if (ctx->ir_base[insn->op2].op == IR_PROTO) {
		proto_ref = ctx->ir_base[insn->op2].op2;
	}

	if (!proto_ref) return 0;
	proto = (const ir_proto_t *)ir_get_str(ctx, proto_ref);

	if ((proto->flags & IR_CALL_CONV_MASK) != (ctx->flags & IR_CALL_CONV_MASK)) return 0;

	cc = ir_get_call_conv_dsc(proto ? proto->flags : IR_CC_DEFAULT);
	copy_stack = 0;
	params_stack_size = ir_call_used_stack(ctx, insn, cc, &copy_stack);
	if (cc->shadow_store_size && params_stack_size == cc->shadow_store_size) {
		params_stack_size = 0;
	}

	// TODO: "params_stack_size" must match the "args_stack_size"
	if (params_stack_size) return 0;

	/* check for passing addresses of local variable */
	uint32_t n = insn->inputs_count;
	for (uint32_t i = 3; i <= n; i++) {
		ir_ref input = ir_insn_op(insn, i);
		if (!IR_IS_CONST_REF(input) && ctx->ir_base[input].type == IR_ADDR) {
			/* Passing addrss of local varible to TAILCALL is disallowd */
			if (ir_may_be_local_addr(ctx, &ctx->ir_base[input])) {
				return 0;
			}
		}
	}

#if defined(IR_TARGET_X64) || defined(IR_TARGET_X86)
	if (!IR_IS_CONST_REF(insn->op2)) {
		if (ctx->ir_base[insn->op2].op == IR_PROTO) {
			if (IR_IS_CONST_REF(ctx->ir_base[insn->op2].op1)) {
				ctx->rules[insn->op2] = IR_FUSED | IR_SIMPLE | IR_PROTO;
			} else {
				ir_match_fuse_load(ctx, ctx->ir_base[insn->op2].op1, ref);
				if (ctx->rules[ctx->ir_base[insn->op2].op1] & IR_FUSED) {
					ctx->rules[insn->op2] = IR_FUSED | IR_SIMPLE | IR_PROTO;
				}
		   }
		} else {
			ir_match_fuse_load(ctx, insn->op2, ref);
		}
	}
#endif

	ctx->rules[ref] = IR_TAILCALL | IR_NO_REG;

	return 1;
}

#if 0
static bool ir_try_tailcalls(ir_ctx *ctx, ir_insn *merge_insn)
{
	ir_ref count = 0, n = merge_insn->inputs_count;
	ir_ref end, ref, *p = merge_insn->ops + 1;
	ir_insn *insn;

	do {
		end = *p;
		insn = &ctx->ir_base[end];
		IR_ASSERT(insn->op == IR_END);
		ref = insn->op1;
		insn = &ctx->ir_base[ref];
		if (insn->op == IR_CALL) {
			if (ir_try_tailcall(ctx, ref, insn)) {
				ctx->rules[end] = IR_SKIPPED | IR_NOP;
				count++;
			}
		} else if (insn->op == IR_MERGE) {
			if (ir_try_tailcalls(ctx, insn)) {
				ctx->rules[end] = IR_SKIPPED | IR_NOP;
				ctx->rules[ref] = IR_SKIPPED | IR_NOP;
				count++;
			}
		}
		p++;
	} while (--n != 0);

	return count == merge_insn->inputs_count;
}
#endif

static size_t ir_calc_args_stack(const ir_ctx *ctx)
{
	ir_use_list *use_list = &ctx->use_lists[1];
	ir_insn *insn;
	ir_ref i, n, *p, use;
	int int_param_num = 0;
	int fp_param_num = 0;
#if IR_SIMD && defined(IR_TARGET_X86)
	int vector_param_num = 0;
#endif
	ir_reg src_reg;
	const ir_call_conv_dsc *cc = ir_get_call_conv_dsc(ctx->flags);
	int32_t stack_offset = 0;

	n = use_list->count;
	for (i = 0, p = &ctx->use_edges[use_list->refs]; i < n; i++, p++) {
		use = *p;
		insn = &ctx->ir_base[use];
		if (insn->op == IR_PARAM) {
			if (IR_IS_TYPE_INT(insn->type)) {
				if (ctx->value_params && ctx->value_params[insn->op3 - 1].align) {
					/* struct passed by value on stack */
					uint32_t align = ctx->value_params[insn->op3 - 1].align;

					align = IR_MAX(sizeof(void*), align);
					stack_offset = IR_ALIGNED_SIZE(stack_offset, align);
					stack_offset += ctx->value_params[insn->op3 - 1].size;
					stack_offset = IR_ALIGNED_SIZE(stack_offset, sizeof(void*));
					continue;
				} else if (int_param_num < cc->int_param_regs_count) {
					src_reg = cc->int_param_regs[int_param_num];
#if IR_X86_I64
					if (src_reg != IR_REG_NONE && (insn->type == IR_I64 || insn->type == IR_U64)) {
						if (int_param_num + 1 < cc->int_param_regs_count) {
							int_param_num++;
							if (cc->shadow_param_regs) {
								fp_param_num++;
							}
						}
						src_reg = IR_REG_NONE;
					}
#endif
				} else {
					src_reg = IR_REG_NONE;
				}
				int_param_num++;
				if (cc->shadow_param_regs) {
					fp_param_num++;
				}
#if IR_SIMD && defined(IR_TARGET_X86)
			} else if (IR_IS_TYPE_VECTOR(insn->type)) {
				if (vector_param_num < cc->vector_param_regs_count) {
					src_reg = cc->vector_param_regs[vector_param_num];
				} else {
					src_reg = IR_REG_NONE;
				}
				vector_param_num++;
#endif
			} else {
				IR_ASSERT(IR_IS_TYPE_FP(insn->type) || IR_IS_TYPE_VECTOR(insn->type));
				if (fp_param_num < cc->fp_param_regs_count) {
					src_reg = cc->fp_param_regs[fp_param_num];
				} else {
					src_reg = IR_REG_NONE;
				}
				fp_param_num++;
				if (cc->shadow_param_regs) {
					int_param_num++;
				}
			}
			if (src_reg == IR_REG_NONE) {
				if (sizeof(void*) == 8) {
					stack_offset += sizeof(void*);
				} else {
					stack_offset += IR_MAX(sizeof(void*), ir_get_type_size(insn->type));
				}
			}
		}
	}

	return stack_offset;
}

static void ir_match_tailcalls(ir_ctx *ctx)
{
	ir_ref ref;
	ir_insn *insn;
	size_t args_stack_size = (size_t)-1;

	ref = ctx->ir_base[1].op1;
	while (ref) {
		insn = &ctx->ir_base[ref];
		if (insn->op == IR_RETURN) {
			if (insn->op1 == insn->op2) {
				if (ctx->ir_base[insn->op1].op == IR_CALL) {
					if (args_stack_size == (size_t)-1) {
						args_stack_size = ir_calc_args_stack(ctx);
						// TODO: "args_stack_size" must match the "params_stack_size"
						if (args_stack_size) return;
					}
					if (ir_try_tailcall(ctx, insn->op1, &ctx->ir_base[insn->op1])) {
						ctx->rules[ref] = IR_SKIPPED | IR_NOP;
					}
				}
			} else if (insn->op2 == IR_UNUSED) {
				if (ctx->ir_base[insn->op1].op == IR_CALL) {
					if (args_stack_size == (size_t)-1) {
						args_stack_size = ir_calc_args_stack(ctx);
						// TODO: "args_stack_size" must match the "params_stack_size"
						if (args_stack_size) return;
					}
					if (ir_try_tailcall(ctx, insn->op1, &ctx->ir_base[insn->op1])) {
						ctx->rules[ref] = IR_SKIPPED | IR_NOP;
					}
#if 0
				} else if (ctx->ir_base[insn->op1].op == IR_MERGE) {
					if (args_stack_size == (size_t)-1) {
						args_stack_size = ir_calc_args_stack(ctx);
						// TODO: "args_stack_size" must match the "params_stack_size"
						if (args_stack_size) return;
					}
					if (ir_try_tailcalls(ctx, &ctx->ir_base[insn->op1])) {
						ctx->rules[insn->op1] = IR_SKIPPED | IR_NOP;
						ctx->rules[ref] = IR_SKIPPED | IR_NOP;
					}
#endif
				}
			}
		}
		ref = insn->op3;
	}
}

int ir_match(ir_ctx *ctx)
{
	uint32_t b;
	ir_ref start, ref, *prev_ref;
	ir_block *bb;
	ir_insn *insn;
	uint32_t entries_count = 0;

	ctx->rules = ir_mem_calloc(ctx->insns_count, sizeof(uint32_t));

	prev_ref = ctx->prev_ref;
	if (!prev_ref) {
		ir_build_prev_refs(ctx);
		prev_ref = ctx->prev_ref;
	}

	if (ctx->entries_count) {
		ctx->entries = ir_mem_malloc(ctx->entries_count * sizeof(ir_ref));
	}

	if ((ctx->flags & IR_OPT_TAILCALL)
	 && (ctx->flags & IR_FUNCTION)
	 && !(ctx->flags & IR_VARARG_FUNC)) {
		ir_match_tailcalls(ctx);
	}

	for (b = ctx->cfg_blocks_count, bb = ctx->cfg_blocks + b; b > 0; b--, bb--) {
		IR_ASSERT(!(bb->flags & IR_BB_UNREACHABLE));
		start = bb->start;
		if (UNEXPECTED(bb->flags & IR_BB_ENTRY)) {
			IR_ASSERT(entries_count < ctx->entries_count);
			insn = &ctx->ir_base[start];
			IR_ASSERT(insn->op == IR_ENTRY);
			insn->op3 = entries_count;
			ctx->entries[entries_count] = b;
			entries_count++;
		}
		ctx->rules[start] = IR_SKIPPED | IR_NOP;
		if (ctx->ir_base[start].op == IR_BEGIN && ctx->ir_base[start].op2) {
			ctx->flags2 |= IR_HAS_BLOCK_ADDR;
		}
		ref = bb->end;
		if (bb->successors_count == 1) {
			insn = &ctx->ir_base[ref];
			if (insn->op == IR_END || insn->op == IR_LOOP_END) {
				if (!ctx->rules[ref]) {
					ctx->rules[ref] = insn->op;
				}
				ref = prev_ref[ref];
				if (ref == start && ctx->cfg_edges[bb->successors] != b) {
					if (EXPECTED(!(bb->flags & IR_BB_ENTRY))) {
						bb->flags |= IR_BB_EMPTY;
					} else if (ctx->flags & IR_MERGE_EMPTY_ENTRIES) {
						bb->flags |= IR_BB_EMPTY;
						if (ctx->cfg_edges[bb->successors] == b + 1) {
							(bb + 1)->flags |= IR_BB_PREV_EMPTY_ENTRY;
						}
					}
					continue;
				}
			}
		}

		ctx->bb_start = start; /* bb_start is used by matcher to avoid fusion of insns from different blocks */

		while (ref != start) {
			uint32_t rule = ctx->rules[ref];

			if (!rule) {
				ctx->rules[ref] = rule = ir_match_insn(ctx, ref);
			}
			ir_match_insn2(ctx, ref, rule);
			ref = prev_ref[ref];
		}
	}

	if (ctx->entries_count) {
		ctx->entries_count = entries_count;
		if (!entries_count) {
			ir_mem_free(ctx->entries);
			ctx->entries = NULL;
		}
	}

	return 1;
}

int32_t ir_get_spill_slot_offset(const ir_ctx *ctx, ir_ref ref)
{
	int32_t offset;

	IR_ASSERT(ref >= 0 && ctx->vregs[ref] > 0 && ctx->live_intervals[ctx->vregs[ref]]);
	offset = ctx->live_intervals[ctx->vregs[ref]]->stack_spill_pos;
	IR_ASSERT(offset != -1);
	return IR_SPILL_POS_TO_OFFSET(offset);
}

const ir_call_conv_dsc *ir_get_call_conv_dsc(uint32_t flags)
{
#ifdef IR_TARGET_X86
	if ((flags & IR_CALL_CONV_MASK) == IR_CC_FASTCALL) {
		return &ir_call_conv_x86_fastcall;
	}
#elif defined(IR_TARGET_X64)
	switch (flags & IR_CALL_CONV_MASK) {
		case IR_CC_DEFAULT:              return &ir_call_conv_default;
		case IR_CC_FASTCALL:             return &ir_call_conv_default;
		case IR_CC_PRESERVE_NONE:        return &ir_call_conv_x86_64_preserve_none;
		case IR_CC_X86_64_SYSV:          return &ir_call_conv_x86_64_sysv;
		case IR_CC_X86_64_MS:            return &ir_call_conv_x86_64_ms;
		default: break;
	}
#elif defined(IR_TARGET_AARCH64)
	switch (flags & IR_CALL_CONV_MASK) {
		case IR_CC_DEFAULT:              return &ir_call_conv_default;
		case IR_CC_FASTCALL:             return &ir_call_conv_default;
		case IR_CC_PRESERVE_NONE:        return &ir_call_conv_aarch64_preserve_none;
		case IR_CC_AARCH64_SYSV:         return &ir_call_conv_aarch64_sysv;
		case IR_CC_AARCH64_DARWIN:       return &ir_call_conv_aarch64_darwin;
		default: break;
	}
#endif
	IR_ASSERT((flags & IR_CALL_CONV_MASK) == IR_CC_DEFAULT || (flags & IR_CALL_CONV_MASK) == IR_CC_BUILTIN);
	return &ir_call_conv_default;
}
