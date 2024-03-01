/*
 * IR - Lightweight JIT Compilation Framework
 * (Native code generator based on DynAsm)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 */

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
      if (_sz < 16) _sz = 16; \
      while (_sz < _need) _sz += _sz; \
      (p) = (t *)ir_mem_realloc((p), _sz); \
      (sz) = _sz; \
    } \
  } while(0)

#define DASM_M_FREE(ctx, p, sz) ir_mem_free(p)

#if IR_DEBUG
# define DASM_CHECKS
#endif

typedef struct _ir_copy {
	ir_type type;
	ir_reg  from;
	ir_reg  to;
} ir_copy;

typedef struct _ir_dessa_copy {
	ir_type type;
	int32_t from; /* negative - constant ref, [0..IR_REG_NUM) - CPU reg, [IR_REG_NUM...) - virtual reg */
	int32_t to;   /* [0..IR_REG_NUM) - CPU reg, [IR_REG_NUM...) - virtual reg  */
} ir_dessa_copy;

#if IR_REG_INT_ARGS
static const int8_t _ir_int_reg_params[IR_REG_INT_ARGS];
#else
static const int8_t *_ir_int_reg_params;
#endif
#if IR_REG_FP_ARGS
static const int8_t _ir_fp_reg_params[IR_REG_FP_ARGS];
#else
static const int8_t *_ir_fp_reg_params;
#endif

static const ir_proto_t *ir_call_proto(const ir_ctx *ctx, ir_insn *insn)
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

#ifdef IR_HAVE_FASTCALL
static const int8_t _ir_int_fc_reg_params[IR_REG_INT_FCARGS];
static const int8_t *_ir_fp_fc_reg_params;

bool ir_is_fastcall(const ir_ctx *ctx, const ir_insn *insn)
{
	if (sizeof(void*) == 4) {
		if (IR_IS_CONST_REF(insn->op2)) {
			const ir_insn *func = &ctx->ir_base[insn->op2];

			if (func->op == IR_FUNC || func->op == IR_FUNC_ADDR) {
				if (func->proto) {
					const ir_proto_t *proto = (const ir_proto_t *)ir_get_str(ctx, func->proto);

					return (proto->flags & IR_FASTCALL_FUNC) != 0;
				}
			}
		} else if (ctx->ir_base[insn->op2].op == IR_PROTO) {
			const ir_proto_t *proto = (const ir_proto_t *)ir_get_str(ctx, ctx->ir_base[insn->op2].op2);

			return (proto->flags & IR_FASTCALL_FUNC) != 0;
		}
		return 0;
	}
	return 0;
}
#else
bool ir_is_fastcall(const ir_ctx *ctx, const ir_insn *insn)
{
	return 0;
}
#endif

bool ir_is_vararg(const ir_ctx *ctx, ir_insn *insn)
{
	const ir_proto_t *proto = ir_call_proto(ctx, insn);

	if (proto) {
		return (proto->flags & IR_VARARG_FUNC) != 0;
	}
	return 0;
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
	int int_reg_params_count = IR_REG_INT_ARGS;
	int fp_reg_params_count = IR_REG_FP_ARGS;
	const int8_t *int_reg_params = _ir_int_reg_params;
	const int8_t *fp_reg_params = _ir_fp_reg_params;

#ifdef IR_HAVE_FASTCALL
	if (sizeof(void*) == 4 && (ctx->flags & IR_FASTCALL_FUNC)) {
		int_reg_params_count = IR_REG_INT_FCARGS;
		fp_reg_params_count = IR_REG_FP_FCARGS;
		int_reg_params = _ir_int_fc_reg_params;
		fp_reg_params = _ir_fp_fc_reg_params;
	}
#endif

	for (i = 0, p = &ctx->use_edges[use_list->refs]; i < use_list->count; i++, p++) {
		use = *p;
		insn = &ctx->ir_base[use];
		if (insn->op == IR_PARAM) {
			if (IR_IS_TYPE_INT(insn->type)) {
				if (use == ref) {
					if (int_param < int_reg_params_count) {
						return int_reg_params[int_param];
					} else {
						return IR_REG_NONE;
					}
				}
				int_param++;
#ifdef _WIN64
				/* WIN64 calling convention use common couter for int and fp registers */
				fp_param++;
#endif
			} else {
				IR_ASSERT(IR_IS_TYPE_FP(insn->type));
				if (use == ref) {
					if (fp_param < fp_reg_params_count) {
						return fp_reg_params[fp_param];
					} else {
						return IR_REG_NONE;
					}
				}
				fp_param++;
#ifdef _WIN64
				/* WIN64 calling convention use common couter for int and fp registers */
				int_param++;
#endif
			}
		}
	}
	return IR_REG_NONE;
}

static int ir_get_args_regs(const ir_ctx *ctx, const ir_insn *insn, int8_t *regs)
{
	int j, n;
	ir_type type;
	int int_param = 0;
	int fp_param = 0;
	int count = 0;
	int int_reg_params_count = IR_REG_INT_ARGS;
	int fp_reg_params_count = IR_REG_FP_ARGS;
	const int8_t *int_reg_params = _ir_int_reg_params;
	const int8_t *fp_reg_params = _ir_fp_reg_params;

#ifdef IR_HAVE_FASTCALL
	if (sizeof(void*) == 4 && ir_is_fastcall(ctx, insn)) {
		int_reg_params_count = IR_REG_INT_FCARGS;
		fp_reg_params_count = IR_REG_FP_FCARGS;
		int_reg_params = _ir_int_fc_reg_params;
		fp_reg_params = _ir_fp_fc_reg_params;
	}
#endif

	n = insn->inputs_count;
	n = IR_MIN(n, IR_MAX_REG_ARGS + 2);
	for (j = 3; j <= n; j++) {
		type = ctx->ir_base[ir_insn_op(insn, j)].type;
		if (IR_IS_TYPE_INT(type)) {
			if (int_param < int_reg_params_count) {
				regs[j] = int_reg_params[int_param];
				count = j + 1;
			} else {
				regs[j] = IR_REG_NONE;
			}
			int_param++;
#ifdef _WIN64
			/* WIN64 calling convention use common couter for int and fp registers */
			fp_param++;
#endif
		} else {
			IR_ASSERT(IR_IS_TYPE_FP(type));
			if (fp_param < fp_reg_params_count) {
				regs[j] = fp_reg_params[fp_param];
				count = j + 1;
			} else {
				regs[j] = IR_REG_NONE;
			}
			fp_param++;
#ifdef _WIN64
			/* WIN64 calling convention use common couter for int and fp registers */
			int_param++;
#endif
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

	IR_ASSERT(ctx->vregs[r1]);
	ival1 = ctx->live_intervals[ctx->vregs[r1]];
	IR_ASSERT(ival1);
	o1 = ival1->stack_spill_pos;
	IR_ASSERT(o1 != -1);
	return o1 == offset;
}

void *ir_resolve_sym_name(const char *name)
{
	void *handle = NULL;
	void *addr;

#ifndef _WIN32
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
	const char *name = ir_get_str(ctx, addr_insn->val.name);
	void *addr = (ctx->loader && ctx->loader->resolve_sym_name) ?
		ctx->loader->resolve_sym_name(ctx->loader, name, 0) :
		ir_resolve_sym_name(name);

	return addr;
}
#endif

static void* ir_sym_val(ir_ctx *ctx, const ir_insn *addr_insn)
{
	const char *name = ir_get_str(ctx, addr_insn->val.name);
	void *addr = (ctx->loader && ctx->loader->resolve_sym_name) ?
		ctx->loader->resolve_sym_name(ctx->loader, name, addr_insn->op == IR_FUNC) :
		ir_resolve_sym_name(name);

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

static int8_t ir_get_fused_reg(ir_ctx *ctx, ir_ref root, ir_ref ref_and_op)
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
	uint32_t           dessa_from_block;
	dasm_State        *dasm_state;
	ir_bitset          emit_constants;
} ir_common_backend_data;

static int ir_const_label(ir_ctx *ctx, ir_ref ref)
{
	ir_common_backend_data *data = ctx->data;
	int label = ctx->cfg_blocks_count - ref;

	IR_ASSERT(IR_IS_CONST_REF(ref));
	ir_bitset_incl(data->emit_constants, -ref);
	return label;
}

#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
# include "ir_emit_x86.h"
#elif defined(IR_TARGET_AARCH64)
# include "ir_emit_aarch64.h"
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
		IR_ASSERT(ref >= 0 && ctx->vregs[ref] && ctx->live_intervals[ctx->vregs[ref]]);
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
			if (pred[from] == to) {
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

static void ir_emit_dessa_move(ir_ctx *ctx, ir_type type, ir_ref to, ir_ref from, ir_reg tmp_reg, ir_reg tmp_fp_reg)
{
	ir_mem mem_from, mem_to;

	IR_ASSERT(from != to);
	if (to < IR_REG_NUM) {
		if (IR_IS_CONST_REF(from)) {
			ir_emit_load(ctx, type, to, from);
		} else if (from < IR_REG_NUM) {
			if (IR_IS_TYPE_INT(type)) {
				ir_emit_mov(ctx, type, to, from);
			} else {
				ir_emit_fp_mov(ctx, type, to, from);
			}
		} else {
			mem_from = ir_vreg_spill_slot(ctx, from - IR_REG_NUM);
			ir_emit_load_mem(ctx, type, to, mem_from);
		}
	} else {
		mem_to = ir_vreg_spill_slot(ctx, to - IR_REG_NUM);
		if (IR_IS_CONST_REF(from)) {
#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
			if (IR_IS_TYPE_INT(type)
			 && !IR_IS_SYM_CONST(ctx->ir_base[from].op)
			 && (ir_type_size[type] != 8 || IR_IS_SIGNED_32BIT(ctx->ir_base[from].val.i64))) {
				ir_emit_store_mem_imm(ctx, type, mem_to, ctx->ir_base[from].val.i32);
				return;
			}
#endif
			ir_reg tmp = IR_IS_TYPE_INT(type) ?  tmp_reg : tmp_fp_reg;
			IR_ASSERT(tmp != IR_REG_NONE);
			ir_emit_load(ctx, type, tmp, from);
			ir_emit_store_mem(ctx, type, mem_to, tmp);
		} else if (from < IR_REG_NUM) {
			ir_emit_store_mem(ctx, type, mem_to, from);
		} else {
			mem_from = ir_vreg_spill_slot(ctx, from - IR_REG_NUM);
			IR_ASSERT(IR_MEM_VAL(mem_to) != IR_MEM_VAL(mem_from));
			ir_reg tmp = IR_IS_TYPE_INT(type) ?  tmp_reg : tmp_fp_reg;
			IR_ASSERT(tmp != IR_REG_NONE);
			ir_emit_load_mem(ctx, type, tmp, mem_from);
			ir_emit_store_mem(ctx, type, mem_to, tmp);
		}
	}
}

IR_ALWAYS_INLINE void ir_dessa_resolve_cycle(ir_ctx *ctx, int32_t *pred, int32_t *loc, ir_bitset todo, ir_type type, int32_t to, ir_reg tmp_reg, ir_reg tmp_fp_reg)
{
	ir_ref from;
	ir_mem tmp_spill_slot;

	IR_MEM_VAL(tmp_spill_slot) = 0;
	IR_ASSERT(!IR_IS_CONST_REF(to));
	from = pred[to];
	IR_ASSERT(!IR_IS_CONST_REF(from));
	IR_ASSERT(from != to);
	IR_ASSERT(loc[from] == from);

	if (IR_IS_TYPE_INT(type)) {
#ifdef IR_HAVE_SWAP_INT
		if (pred[from] == to && to < IR_REG_NUM && from < IR_REG_NUM) {
			/* a simple cycle from 2 elements */
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
			ir_emit_load_mem_int(ctx, type, tmp_reg, ir_vreg_spill_slot(ctx, to - IR_REG_NUM));
		}
	} else {
#ifdef IR_HAVE_SWAP_FP
		if (pred[from] == to && to < IR_REG_NUM && from < IR_REG_NUM) {
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
			ir_emit_load_mem_fp(ctx, type, tmp_fp_reg, ir_vreg_spill_slot(ctx, to - IR_REG_NUM));
		}
	}

	while (1) {
		int32_t r;

		from = pred[to];
		r = loc[from];

		if (from == r && ir_bitset_in(todo, from)) {
			/* Memory to memory move inside an isolated or "blocked" cycle requres an additional temporary register */
			if (to >= IR_REG_NUM && r >= IR_REG_NUM) {
				ir_reg tmp = IR_IS_TYPE_INT(type) ?  tmp_reg : tmp_fp_reg;

				if (!IR_MEM_VAL(tmp_spill_slot)) {
					/* Free a register, saving it in a temporary spill slot */
					tmp_spill_slot = IR_MEM_BO(IR_REG_STACK_POINTER, -16);
					ir_emit_store_mem(ctx, type, tmp_spill_slot, tmp);
				}
				ir_emit_dessa_move(ctx, type, to, r, tmp_reg, tmp_fp_reg);
			} else {
				ir_emit_dessa_move(ctx, type, to, r, IR_REG_NONE, IR_REG_NONE);
			}
			ir_bitset_excl(todo, to);
			loc[from] = to;
			to = from;
		} else {
			break;
		}
	}
	if (IR_MEM_VAL(tmp_spill_slot)) {
		ir_emit_load_mem(ctx, type, IR_IS_TYPE_INT(type) ? tmp_reg : tmp_fp_reg, tmp_spill_slot);
	}
	ir_emit_dessa_move(ctx, type, to, loc[from], IR_REG_NONE, IR_REG_NONE);
	ir_bitset_excl(todo, to);
	loc[from] = to;
}

static int ir_dessa_parallel_copy(ir_ctx *ctx, ir_dessa_copy *copies, int count, ir_reg tmp_reg, ir_reg tmp_fp_reg)
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
		ir_emit_dessa_move(ctx, type, to, from, tmp_reg, tmp_fp_reg);
		return 1;
	}

	len = IR_REG_NUM + ctx->vregs_count + 1;
	todo = ir_bitset_malloc(len);
	srcs = ir_bitset_malloc(len);
	loc = ir_mem_malloc(len * 2 * sizeof(int32_t) + len * sizeof(int8_t));
	pred = loc + len;
	types = (int8_t*)(pred + len);

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
	ready = ir_bitset_malloc(len);
	ir_bitset_copy(ready, todo, ir_bitset_len(len));
	ir_bitset_difference(ready, srcs, ir_bitset_len(len));
	if (tmp_reg != IR_REG_NONE) {
		ir_bitset_excl(ready, tmp_reg);
	}
	if (tmp_fp_reg != IR_REG_NONE) {
		ir_bitset_excl(ready, tmp_fp_reg);
	}
	while ((to = ir_bitset_pop_first(ready, ir_bitset_len(len))) >= 0) {
		ir_bitset_excl(todo, to);
		type = types[to];
		from = pred[to];
		if (IR_IS_CONST_REF(from)) {
			ir_emit_dessa_move(ctx, type, to, from, tmp_reg, tmp_fp_reg);
		} else {
			int32_t r = loc[from];
			ir_emit_dessa_move(ctx, type, to, r, tmp_reg, tmp_fp_reg);
			loc[from] = to;
			if (from == r && ir_bitset_in(todo, from) && from != tmp_reg && from != tmp_fp_reg) {
				ir_bitset_incl(ready, from);
			}
		}
	}

	/* then we resolve all "windmill axles" - cycles (this requres temporary registers) */
	visited = ir_bitset_malloc(len);
	ir_bitset_copy(ready, todo, ir_bitset_len(len));
	ir_bitset_intersection(ready, srcs, ir_bitset_len(len));
	while ((to = ir_bitset_first(ready, ir_bitset_len(len))) >= 0) {
		ir_bitset_clear(visited, ir_bitset_len(len));
		ir_bitset_incl(visited, to);
		to = pred[to];
		while (!IR_IS_CONST_REF(to) && ir_bitset_in(ready, to)) {
			to = pred[to];
			if (!IR_IS_CONST_REF(to) && ir_bitset_in(visited, to)) {
				/* We found a cycle. Resolve it. */
				ir_bitset_incl(visited, to);
				type = types[to];
				ir_dessa_resolve_cycle(ctx, pred, loc, todo, type, to, tmp_reg, tmp_fp_reg);
				break;
			}
			ir_bitset_incl(visited, to);
		}
		ir_bitset_difference(ready, visited, ir_bitset_len(len));
	}

	/* finally we resolve remaining "windmill blades" - trees that set temporary registers */
	ir_bitset_copy(ready, todo, ir_bitset_len(len));
	ir_bitset_difference(ready, srcs, ir_bitset_len(len));
	while ((to = ir_bitset_pop_first(ready, ir_bitset_len(len))) >= 0) {
		ir_bitset_excl(todo, to);
		type = types[to];
		from = pred[to];
		if (IR_IS_CONST_REF(from)) {
			ir_emit_dessa_move(ctx, type, to, from, tmp_reg, tmp_fp_reg);
		} else {
			int32_t r = loc[from];
			ir_emit_dessa_move(ctx, type, to, r, tmp_reg, tmp_fp_reg);
			loc[from] = to;
			if (from == r && ir_bitset_in(todo, from)) {
				ir_bitset_incl(ready, from);
			}
		}
	}

	IR_ASSERT(ir_bitset_empty(todo, ir_bitset_len(len)));

	ir_mem_free(visited);
	ir_mem_free(ready);
	ir_mem_free(loc);
	ir_mem_free(srcs);
	ir_mem_free(todo);
	return 1;
}

static void ir_emit_dessa_moves(ir_ctx *ctx, int b, ir_block *bb)
{
	uint32_t succ, k, n = 0;
	ir_block *succ_bb;
	ir_use_list *use_list;
	ir_ref i, *p;
	ir_dessa_copy *copies;
	ir_reg tmp_reg = ctx->regs[bb->end][0];
	ir_reg tmp_fp_reg = ctx->regs[bb->end][1];

	IR_ASSERT(bb->successors_count == 1);
	succ = ctx->cfg_edges[bb->successors];
	succ_bb = &ctx->cfg_blocks[succ];
	IR_ASSERT(succ_bb->predecessors_count > 1);
	use_list = &ctx->use_lists[succ_bb->start];
	k = ir_phi_input_number(ctx, succ_bb, b);

	copies = alloca(use_list->count * sizeof(ir_dessa_copy));

	for (i = 0, p = &ctx->use_edges[use_list->refs]; i < use_list->count; i++, p++) {
		ir_ref ref = *p;
		ir_insn *insn = &ctx->ir_base[ref];

		if (insn->op == IR_PHI) {
			ir_ref input = ir_insn_op(insn, k);
			ir_reg src = ir_get_alocated_reg(ctx, ref, k);
			ir_reg dst = ctx->regs[ref][0];
			ir_ref from, to;

			IR_ASSERT(dst == IR_REG_NONE || !IR_REG_SPILLED(dst));
			if (IR_IS_CONST_REF(input)) {
				from = input;
			} else {
				from = (src != IR_REG_NONE && !IR_REG_SPILLED(src)) ?
					(ir_ref)src : (ir_ref)(IR_REG_NUM + ctx->vregs[input]);
			}
			to = (dst != IR_REG_NONE) ?
				(ir_ref)dst : (ir_ref)(IR_REG_NUM + ctx->vregs[ref]);
			if (to != from) {
				if (to >= IR_REG_NUM
				 && from >= IR_REG_NUM
				 && IR_MEM_VAL(ir_vreg_spill_slot(ctx, from - IR_REG_NUM)) ==
						IR_MEM_VAL(ir_vreg_spill_slot(ctx, to - IR_REG_NUM))) {
					/* It's possible that different virtual registers share the same special spill slot */
					// TODO: See ext/opcache/tests/jit/gh11917.phpt failure on Linux 32-bit
					continue;
				}
				copies[n].type = insn->type;
				copies[n].from = from;
				copies[n].to = to;
				n++;
			}
		}
	}

	if (n > 0) {
		ir_dessa_parallel_copy(ctx, copies, n, tmp_reg, tmp_fp_reg);
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
		ref = bb->end;
		if (bb->successors_count == 1) {
			insn = &ctx->ir_base[ref];
			if (insn->op == IR_END || insn->op == IR_LOOP_END) {
				ctx->rules[ref] = insn->op;
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

int32_t ir_get_spill_slot_offset(ir_ctx *ctx, ir_ref ref)
{
	int32_t offset;

	IR_ASSERT(ref >= 0 && ctx->vregs[ref] && ctx->live_intervals[ctx->vregs[ref]]);
	offset = ctx->live_intervals[ctx->vregs[ref]]->stack_spill_pos;
	IR_ASSERT(offset != -1);
	return IR_SPILL_POS_TO_OFFSET(offset);
}
