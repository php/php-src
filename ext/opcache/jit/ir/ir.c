/*
 * IR - Lightweight JIT Compilation Framework
 * (IR construction, folding, utilities)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 *
 * The logical IR representation is based on Cliff Click's Sea of Nodes.
 * See: C. Click, M. Paleczny. "A Simple Graph-Based Intermediate
 * Representation" In ACM SIGPLAN Workshop on Intermediate Representations
 * (IR '95), pages 35-49, Jan. 1995.
 *
 * The physical IR representation is based on Mike Pall's LuaJIT IR.
 * See: M. Pall. "LuaJIT 2.0 intellectual property disclosure and research
 * opportunities" November 2009 http://lua-users.org/lists/lua-l/2009-11/msg00089.html
 */

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#ifndef _WIN32
# include <sys/mman.h>
# if defined(__linux__) || defined(__sun)
#  include <alloca.h>
# endif
# if defined(__APPLE__) && defined(__aarch64__)
#  include <libkern/OSCacheControl.h>
# endif
#else
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

#include "ir.h"
#include "ir_private.h"

#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_VALGRIND
# include <valgrind/valgrind.h>
#endif

#define IR_TYPE_FLAGS(name, type, field, flags) ((flags)|sizeof(type)),
#define IR_TYPE_NAME(name, type, field, flags)  #name,
#define IR_TYPE_CNAME(name, type, field, flags) #type,
#define IR_TYPE_SIZE(name, type, field, flags)  sizeof(type),
#define IR_OP_NAME(name, flags, op1, op2, op3)  #name,

const uint8_t ir_type_flags[IR_LAST_TYPE] = {
	0,
	IR_TYPES(IR_TYPE_FLAGS)
};

const char *ir_type_name[IR_LAST_TYPE] = {
	"void",
	IR_TYPES(IR_TYPE_NAME)
};

const uint8_t ir_type_size[IR_LAST_TYPE] = {
	0,
	IR_TYPES(IR_TYPE_SIZE)
};

const char *ir_type_cname[IR_LAST_TYPE] = {
	"void",
	IR_TYPES(IR_TYPE_CNAME)
};

const char *ir_op_name[IR_LAST_OP] = {
	IR_OPS(IR_OP_NAME)
#ifdef IR_PHP
	IR_PHP_OPS(IR_OP_NAME)
#endif
};

static void ir_print_escaped_str(const char *s, size_t len, FILE *f)
{
	char ch;

	while (len > 0) {
		ch = *s;
		switch (ch) {
			case '\\': fputs("\\\\", f); break;
			case '\'': fputs("'", f); break;
			case '\"': fputs("\\\"", f); break;
			case '\a': fputs("\\a", f); break;
			case '\b': fputs("\\b", f); break;
			case '\033': fputs("\\e", f); break;
			case '\f': fputs("\\f", f); break;
			case '\n': fputs("\\n", f); break;
			case '\r': fputs("\\r", f); break;
			case '\t': fputs("\\t", f); break;
			case '\v': fputs("\\v", f); break;
			case '\?': fputs("\\?", f); break;
			default:
				if (ch < 32) {
					fprintf(f, "\\%c%c%c",
						'0' + ((ch >> 3) % 8),
						'0' + ((ch >> 6) % 8),
						'0' + (ch % 8));
					break;
				} else {
					fputc(ch, f);
				}
		}
		s++;
		len--;
	}
}

void ir_print_const(const ir_ctx *ctx, const ir_insn *insn, FILE *f, bool quoted)
{
	char buf[128];

	if (insn->op == IR_FUNC || insn->op == IR_SYM) {
		fprintf(f, "%s", ir_get_str(ctx, insn->val.name));
		return;
	} else if (insn->op == IR_STR) {
		size_t len;
		const char *str = ir_get_strl(ctx, insn->val.str, &len);

		if (quoted) {
			fprintf(f, "\"");
			ir_print_escaped_str(str, len, f);
			fprintf(f, "\"");
		} else {
			ir_print_escaped_str(str, len, f);
		}
		return;
	}
	IR_ASSERT(IR_IS_CONST_OP(insn->op) || insn->op == IR_FUNC_ADDR);
	switch (insn->type) {
		case IR_BOOL:
			fprintf(f, "%u", insn->val.b);
			break;
		case IR_U8:
			fprintf(f, "%u", insn->val.u8);
			break;
		case IR_U16:
			fprintf(f, "%u", insn->val.u16);
			break;
		case IR_U32:
			fprintf(f, "%u", insn->val.u32);
			break;
		case IR_U64:
			fprintf(f, "%" PRIu64, insn->val.u64);
			break;
		case IR_ADDR:
			if (insn->val.addr) {
				fprintf(f, "0x%" PRIxPTR, insn->val.addr);
			} else {
				fprintf(f, "0");
			}
			break;
		case IR_CHAR:
			if (insn->val.c == '\\') {
				fprintf(f, "'\\\\'");
			} else if (insn->val.c >= ' ') {
				fprintf(f, "'%c'", insn->val.c);
			} else if (insn->val.c == '\t') {
				fprintf(f, "'\\t'");
			} else if (insn->val.c == '\r') {
				fprintf(f, "'\\r'");
			} else if (insn->val.c == '\n') {
				fprintf(f, "'\\n'");
			} else if (insn->val.c == '\0') {
				fprintf(f, "'\\0'");
			} else {
				fprintf(f, "%u", insn->val.c);
			}
			break;
		case IR_I8:
			fprintf(f, "%d", insn->val.i8);
			break;
		case IR_I16:
			fprintf(f, "%d", insn->val.i16);
			break;
		case IR_I32:
			fprintf(f, "%d", insn->val.i32);
			break;
		case IR_I64:
			fprintf(f, "%" PRIi64, insn->val.i64);
			break;
		case IR_DOUBLE:
			if (isnan(insn->val.d)) {
				fprintf(f, "nan");
			} else {
				snprintf(buf, sizeof(buf), "%g", insn->val.d);
				if (strtod(buf, NULL) != insn->val.d) {
					snprintf(buf, sizeof(buf), "%.53e", insn->val.d);
					if (strtod(buf, NULL) != insn->val.d) {
						IR_ASSERT(0 && "can't format double");
					}
				}
				fprintf(f, "%s", buf);
			}
			break;
		case IR_FLOAT:
			if (isnan(insn->val.f)) {
				fprintf(f, "nan");
			} else {
				snprintf(buf, sizeof(buf), "%g", insn->val.f);
				if (strtod(buf, NULL) != insn->val.f) {
					snprintf(buf, sizeof(buf), "%.24e", insn->val.f);
					if (strtod(buf, NULL) != insn->val.f) {
						IR_ASSERT(0 && "can't format float");
					}
				}
				fprintf(f, "%s", buf);
			}
			break;
		default:
			IR_ASSERT(0);
			break;
	}
}

#define ir_op_flag_v       0
#define ir_op_flag_v0X3    (0 | (3 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_d       IR_OP_FLAG_DATA
#define ir_op_flag_d0      ir_op_flag_d
#define ir_op_flag_d1      (ir_op_flag_d | 1 | (1 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_d1X1    (ir_op_flag_d | 1 | (2 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_d2      (ir_op_flag_d | 2 | (2 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_d2C     (ir_op_flag_d | IR_OP_FLAG_COMMUTATIVE | 2 | (2 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_d3      (ir_op_flag_d | 3 | (3 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_r       IR_OP_FLAG_DATA                                         // "d" and "r" are the same now
#define ir_op_flag_r0      ir_op_flag_r
#define ir_op_flag_p       (IR_OP_FLAG_DATA | IR_OP_FLAG_PINNED)
#define ir_op_flag_p1      (ir_op_flag_p | 1 | (1 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_p1X1    (ir_op_flag_p | 1 | (2 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_p1X2    (ir_op_flag_p | 1 | (3 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_p2      (ir_op_flag_p | 2 | (2 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_pN      (ir_op_flag_p | IR_OP_FLAG_VAR_INPUTS)
#define ir_op_flag_c       IR_OP_FLAG_CONTROL
#define ir_op_flag_c1X2    (ir_op_flag_c | 1 | (3 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_c3      (ir_op_flag_c | 3 | (3 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_S       (IR_OP_FLAG_CONTROL|IR_OP_FLAG_BB_START)
#define ir_op_flag_S0X1    (ir_op_flag_S | 0 | (1 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_S1      (ir_op_flag_S | 1 | (1 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_S1X1    (ir_op_flag_S | 1 | (2 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_S2      (ir_op_flag_S | 2 | (2 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_S2X1    (ir_op_flag_S | 2 | (3 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_SN      (ir_op_flag_S | IR_OP_FLAG_VAR_INPUTS)
#define ir_op_flag_E       (IR_OP_FLAG_CONTROL|IR_OP_FLAG_BB_END)
#define ir_op_flag_E1      (ir_op_flag_E | 1 | (1 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_E2      (ir_op_flag_E | 2 | (2 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_T       (IR_OP_FLAG_CONTROL|IR_OP_FLAG_BB_END|IR_OP_FLAG_TERMINATOR)
#define ir_op_flag_T2X1    (ir_op_flag_T | 2 | (3 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_T1X2    (ir_op_flag_T | 1 | (3 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_l       (IR_OP_FLAG_CONTROL|IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_LOAD)
#define ir_op_flag_l0      ir_op_flag_l
#define ir_op_flag_l1      (ir_op_flag_l | 1 | (1 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_l1X1    (ir_op_flag_l | 1 | (2 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_l1X2    (ir_op_flag_l | 1 | (3 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_l2      (ir_op_flag_l | 2 | (2 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_l3      (ir_op_flag_l | 3 | (3 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_s       (IR_OP_FLAG_CONTROL|IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_STORE)
#define ir_op_flag_s0      ir_op_flag_s
#define ir_op_flag_s1      (ir_op_flag_s | 1 | (1 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_s2      (ir_op_flag_s | 2 | (2 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_s2X1    (ir_op_flag_s | 2 | (3 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_s3      (ir_op_flag_s | 3 | (3 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_x1      (IR_OP_FLAG_CONTROL|IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_CALL | 1 | (1 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_x2      (IR_OP_FLAG_CONTROL|IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_CALL | 2 | (2 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_x3      (IR_OP_FLAG_CONTROL|IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_CALL | 3 | (3 << IR_OP_FLAG_OPERANDS_SHIFT))
#define ir_op_flag_xN      (IR_OP_FLAG_CONTROL|IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_CALL | IR_OP_FLAG_VAR_INPUTS)
#define ir_op_flag_a2      (IR_OP_FLAG_CONTROL|IR_OP_FLAG_MEM|IR_OP_FLAG_MEM_ALLOC | 2 | (2 << IR_OP_FLAG_OPERANDS_SHIFT))

#define ir_op_kind____     IR_OPND_UNUSED
#define ir_op_kind_def     IR_OPND_DATA
#define ir_op_kind_ref     IR_OPND_DATA
#define ir_op_kind_src     IR_OPND_CONTROL
#define ir_op_kind_reg     IR_OPND_CONTROL_DEP
#define ir_op_kind_ret     IR_OPND_CONTROL_REF
#define ir_op_kind_str     IR_OPND_STR
#define ir_op_kind_num     IR_OPND_NUM
#define ir_op_kind_fld     IR_OPND_STR
#define ir_op_kind_var     IR_OPND_DATA
#define ir_op_kind_prb     IR_OPND_PROB
#define ir_op_kind_opt     IR_OPND_PROB
#define ir_op_kind_pro     IR_OPND_PROTO

#define _IR_OP_FLAGS(name, flags, op1, op2, op3) \
	IR_OP_FLAGS(ir_op_flag_ ## flags, ir_op_kind_ ## op1, ir_op_kind_ ## op2, ir_op_kind_ ## op3),

const uint32_t ir_op_flags[IR_LAST_OP] = {
	IR_OPS(_IR_OP_FLAGS)
#ifdef IR_PHP
	IR_PHP_OPS(_IR_OP_FLAGS)
#endif
};

static void ir_grow_bottom(ir_ctx *ctx)
{
	ir_insn *buf = ctx->ir_base - ctx->consts_limit;
	ir_ref old_consts_limit = ctx->consts_limit;

	if (ctx->consts_limit < 1024 * 4) {
		ctx->consts_limit *= 2;
	} else if (ctx->consts_limit < 1024 * 4 * 2) {
		ctx->consts_limit = 1024 * 4 * 2;
	} else {
		ctx->consts_limit += 1024 * 4;
	}
	buf = ir_mem_realloc(buf, (ctx->consts_limit + ctx->insns_limit) * sizeof(ir_insn));
	memmove(buf + (ctx->consts_limit - old_consts_limit),
		buf,
		(old_consts_limit + ctx->insns_count) * sizeof(ir_insn));
	ctx->ir_base = buf + ctx->consts_limit;
}

static ir_ref ir_next_const(ir_ctx *ctx)
{
	ir_ref ref = ctx->consts_count;

	if (UNEXPECTED(ref >= ctx->consts_limit)) {
		ir_grow_bottom(ctx);
	}
	ctx->consts_count = ref + 1;
	return -ref;
}

static void ir_grow_top(ir_ctx *ctx)
{
	ir_insn *buf = ctx->ir_base - ctx->consts_limit;

	if (ctx->insns_limit < 1024 * 4) {
		ctx->insns_limit *= 2;
	} else if (ctx->insns_limit < 1024 * 4 * 2) {
		ctx->insns_limit = 1024 * 4 * 2;
	} else {
		ctx->insns_limit += 1024 * 4;
	}
	buf = ir_mem_realloc(buf, (ctx->consts_limit + ctx->insns_limit) * sizeof(ir_insn));
	ctx->ir_base = buf + ctx->consts_limit;
}

static ir_ref ir_next_insn(ir_ctx *ctx)
{
	ir_ref ref = ctx->insns_count;

	if (UNEXPECTED(ref >= ctx->insns_limit)) {
		ir_grow_top(ctx);
	}
	ctx->insns_count = ref + 1;
	return ref;
}

void ir_truncate(ir_ctx *ctx)
{
	ir_insn *buf = ir_mem_malloc((ctx->consts_count + ctx->insns_count) * sizeof(ir_insn));

	memcpy(buf, ctx->ir_base - ctx->consts_count, (ctx->consts_count + ctx->insns_count) * sizeof(ir_insn));
	ir_mem_free(ctx->ir_base - ctx->consts_limit);
	ctx->insns_limit = ctx->insns_count;
	ctx->consts_limit = ctx->consts_count;
	ctx->ir_base = buf + ctx->consts_limit;
}

void ir_init(ir_ctx *ctx, uint32_t flags, ir_ref consts_limit, ir_ref insns_limit)
{
	ir_insn *buf;

	IR_ASSERT(consts_limit >= IR_CONSTS_LIMIT_MIN);
	IR_ASSERT(insns_limit >= IR_INSNS_LIMIT_MIN);

	memset(ctx, 0, sizeof(ir_ctx));

	ctx->insns_count = IR_UNUSED + 1;
	ctx->insns_limit = insns_limit;
	ctx->consts_count = -(IR_TRUE - 1);
	ctx->consts_limit = consts_limit;
	ctx->fold_cse_limit = IR_UNUSED + 1;
	ctx->flags = flags;

	ctx->spill_base = -1;
	ctx->fixed_stack_frame_size = -1;

	buf = ir_mem_malloc((consts_limit + insns_limit) * sizeof(ir_insn));
	ctx->ir_base = buf + consts_limit;

	ctx->ir_base[IR_UNUSED].optx = IR_NOP;
	ctx->ir_base[IR_NULL].optx = IR_OPT(IR_C_ADDR, IR_ADDR);
	ctx->ir_base[IR_NULL].val.u64 = 0;
	ctx->ir_base[IR_FALSE].optx = IR_OPT(IR_C_BOOL, IR_BOOL);
	ctx->ir_base[IR_FALSE].val.u64 = 0;
	ctx->ir_base[IR_TRUE].optx = IR_OPT(IR_C_BOOL, IR_BOOL);
	ctx->ir_base[IR_TRUE].val.u64 = 1;
}

void ir_free(ir_ctx *ctx)
{
	ir_insn *buf = ctx->ir_base - ctx->consts_limit;
	ir_mem_free(buf);
	if (ctx->strtab.data) {
		ir_strtab_free(&ctx->strtab);
	}
	if (ctx->binding) {
		ir_hashtab_free(ctx->binding);
		ir_mem_free(ctx->binding);
	}
	if (ctx->use_lists) {
		ir_mem_free(ctx->use_lists);
	}
	if (ctx->use_edges) {
		ir_mem_free(ctx->use_edges);
	}
	if (ctx->cfg_blocks) {
		ir_mem_free(ctx->cfg_blocks);
	}
	if (ctx->cfg_edges) {
		ir_mem_free(ctx->cfg_edges);
	}
	if (ctx->cfg_map) {
		ir_mem_free(ctx->cfg_map);
	}
	if (ctx->rules) {
		ir_mem_free(ctx->rules);
	}
	if (ctx->vregs) {
		ir_mem_free(ctx->vregs);
	}
	if (ctx->live_intervals) {
		ir_mem_free(ctx->live_intervals);
	}
	if (ctx->arena) {
		ir_arena_free(ctx->arena);
	}
	if (ctx->regs) {
		ir_mem_free(ctx->regs);
		if (ctx->fused_regs) {
			ir_strtab_free(ctx->fused_regs);
			ir_mem_free(ctx->fused_regs);
		}
	}
	if (ctx->prev_ref) {
		ir_mem_free(ctx->prev_ref);
	}
	if (ctx->entries) {
		ir_mem_free(ctx->entries);
	}
	if (ctx->osr_entry_loads) {
		ir_list_free((ir_list*)ctx->osr_entry_loads);
		ir_mem_free(ctx->osr_entry_loads);
	}
}

ir_ref ir_unique_const_addr(ir_ctx *ctx, uintptr_t addr)
{
	ir_ref ref = ir_next_const(ctx);
	ir_insn *insn = &ctx->ir_base[ref];

	insn->optx = IR_OPT(IR_ADDR, IR_ADDR);
	insn->val.u64 = addr;
	/* don't insert into constants chain */
	insn->prev_const = IR_UNUSED;
#if 0
	insn->prev_const = ctx->prev_const_chain[IR_ADDR];
	ctx->prev_const_chain[IR_ADDR] = ref;
#endif
#if 0
	ir_insn *prev_insn, *next_insn;
	ir_ref next;

	prev_insn = NULL;
	next = ctx->prev_const_chain[IR_ADDR];
	while (next) {
		next_insn = &ctx->ir_base[next];
		if (UNEXPECTED(next_insn->val.u64 >= addr)) {
			break;
		}
		prev_insn = next_insn;
		next = next_insn->prev_const;
	}

	if (prev_insn) {
		insn->prev_const = prev_insn->prev_const;
		prev_insn->prev_const = ref;
	} else {
		insn->prev_const = ctx->prev_const_chain[IR_ADDR];
		ctx->prev_const_chain[IR_ADDR] = ref;
	}
#endif

	return ref;
}

static IR_NEVER_INLINE ir_ref ir_const_ex(ir_ctx *ctx, ir_val val, uint8_t type, uint32_t optx)
{
	ir_insn *insn, *prev_insn;
	ir_ref ref, prev;

	if (type == IR_BOOL) {
		return val.u64 ? IR_TRUE : IR_FALSE;
	} else if (type == IR_ADDR && val.u64 == 0) {
		return IR_NULL;
	}
	prev_insn = NULL;
	ref = ctx->prev_const_chain[type];
	while (ref) {
		insn = &ctx->ir_base[ref];
		if (UNEXPECTED(insn->val.u64 >= val.u64)) {
			if (insn->val.u64 == val.u64) {
				if (insn->optx == optx) {
					return ref;
				}
			} else {
				break;
			}
		}
		prev_insn = insn;
		ref = insn->prev_const;
	}

	if (prev_insn) {
		prev = prev_insn->prev_const;
		prev_insn->prev_const = -ctx->consts_count;
	} else {
		prev = ctx->prev_const_chain[type];
		ctx->prev_const_chain[type] = -ctx->consts_count;
	}

	ref = ir_next_const(ctx);
	insn = &ctx->ir_base[ref];
	insn->prev_const = prev;

	insn->optx = optx;
	insn->val.u64 = val.u64;

	return ref;
}

ir_ref ir_const(ir_ctx *ctx, ir_val val, uint8_t type)
{
	return ir_const_ex(ctx, val, type, IR_OPT(type, type));
}

ir_ref ir_const_i8(ir_ctx *ctx, int8_t c)
{
	ir_val val;
	val.i64 = c;
	return ir_const(ctx, val, IR_I8);
}

ir_ref ir_const_i16(ir_ctx *ctx, int16_t c)
{
	ir_val val;
	val.i64 = c;
	return ir_const(ctx, val, IR_I16);
}

ir_ref ir_const_i32(ir_ctx *ctx, int32_t c)
{
	ir_val val;
	val.i64 = c;
	return ir_const(ctx, val, IR_I32);
}

ir_ref ir_const_i64(ir_ctx *ctx, int64_t c)
{
	ir_val val;
	val.i64 = c;
	return ir_const(ctx, val, IR_I64);
}

ir_ref ir_const_u8(ir_ctx *ctx, uint8_t c)
{
	ir_val val;
	val.u64 = c;
	return ir_const(ctx, val, IR_U8);
}

ir_ref ir_const_u16(ir_ctx *ctx, uint16_t c)
{
	ir_val val;
	val.u64 = c;
	return ir_const(ctx, val, IR_U16);
}

ir_ref ir_const_u32(ir_ctx *ctx, uint32_t c)
{
	ir_val val;
	val.u64 = c;
	return ir_const(ctx, val, IR_U32);
}

ir_ref ir_const_u64(ir_ctx *ctx, uint64_t c)
{
	ir_val val;
	val.u64 = c;
	return ir_const(ctx, val, IR_U64);
}

ir_ref ir_const_bool(ir_ctx *ctx, bool c)
{
	return (c) ? IR_TRUE : IR_FALSE;
}

ir_ref ir_const_char(ir_ctx *ctx, char c)
{
	ir_val val;
	val.i64 = c;
	return ir_const(ctx, val, IR_CHAR);
}

ir_ref ir_const_float(ir_ctx *ctx, float c)
{
	ir_val val;
	val.u32_hi = 0;
	val.f = c;
	return ir_const(ctx, val, IR_FLOAT);
}

ir_ref ir_const_double(ir_ctx *ctx, double c)
{
	ir_val val;
	val.d = c;
	return ir_const(ctx, val, IR_DOUBLE);
}

ir_ref ir_const_addr(ir_ctx *ctx, uintptr_t c)
{
	if (c == 0) {
		return IR_NULL;
	}
	ir_val val;
	val.u64 = c;
	return ir_const(ctx, val, IR_ADDR);
}

ir_ref ir_const_func_addr(ir_ctx *ctx, uintptr_t c, ir_ref proto)
{
	if (c == 0) {
		return IR_NULL;
	}
	ir_val val;
	val.u64 = c;
	IR_ASSERT(proto >= 0 && proto < 0xffff);
	return ir_const_ex(ctx, val, IR_ADDR, IR_OPTX(IR_FUNC_ADDR, IR_ADDR, proto));
}

ir_ref ir_const_func(ir_ctx *ctx, ir_ref str, ir_ref proto)
{
	ir_val val;
	val.u64 = str;
	IR_ASSERT(proto >= 0 && proto < 0xffff);
	return ir_const_ex(ctx, val, IR_ADDR, IR_OPTX(IR_FUNC, IR_ADDR, proto));
}

ir_ref ir_const_sym(ir_ctx *ctx, ir_ref str)
{
	ir_val val;
	val.u64 = str;
	return ir_const_ex(ctx, val, IR_ADDR, IR_OPTX(IR_SYM, IR_ADDR, 0));
}

ir_ref ir_const_str(ir_ctx *ctx, ir_ref str)
{
	ir_val val;
	val.u64 = str;
	return ir_const_ex(ctx, val, IR_ADDR, IR_OPTX(IR_STR, IR_ADDR, 0));
}

ir_ref ir_str(ir_ctx *ctx, const char *s)
{
	size_t len;

	if (!ctx->strtab.data) {
		ir_strtab_init(&ctx->strtab, 64, 4096);
	}
	len = strlen(s);
	IR_ASSERT(len <= 0xffffffff);
	return ir_strtab_lookup(&ctx->strtab, s, (uint32_t)len, ir_strtab_count(&ctx->strtab) + 1);
}

ir_ref ir_strl(ir_ctx *ctx, const char *s, size_t len)
{
	if (!ctx->strtab.data) {
		ir_strtab_init(&ctx->strtab, 64, 4096);
	}
	IR_ASSERT(len <= 0xffffffff);
	return ir_strtab_lookup(&ctx->strtab, s, (uint32_t)len, ir_strtab_count(&ctx->strtab) + 1);
}

const char *ir_get_str(const ir_ctx *ctx, ir_ref idx)
{
	IR_ASSERT(ctx->strtab.data);
	return ir_strtab_str(&ctx->strtab, idx - 1);
}

const char *ir_get_strl(const ir_ctx *ctx, ir_ref idx, size_t *len)
{
	IR_ASSERT(ctx->strtab.data);
	return ir_strtab_strl(&ctx->strtab, idx - 1, len);
}

ir_ref ir_proto_0(ir_ctx *ctx, uint8_t flags, ir_type ret_type)
{
	ir_proto_t proto;

	proto.flags = flags;
	proto.ret_type = ret_type;
	proto.params_count = 0;
	return ir_strl(ctx, (const char *)&proto, offsetof(ir_proto_t, param_types) + 0);
}

ir_ref ir_proto_1(ir_ctx *ctx, uint8_t flags, ir_type ret_type, ir_type t1)
{
	ir_proto_t proto;

	proto.flags = flags;
	proto.ret_type = ret_type;
	proto.params_count = 1;
	proto.param_types[0] = t1;
	return ir_strl(ctx, (const char *)&proto, offsetof(ir_proto_t, param_types) + 1);
}

ir_ref ir_proto_2(ir_ctx *ctx, uint8_t flags, ir_type ret_type, ir_type t1, ir_type t2)
{
	ir_proto_t proto;

	proto.flags = flags;
	proto.ret_type = ret_type;
	proto.params_count = 2;
	proto.param_types[0] = t1;
	proto.param_types[1] = t2;
	return ir_strl(ctx, (const char *)&proto, offsetof(ir_proto_t, param_types) + 2);
}

ir_ref ir_proto_3(ir_ctx *ctx, uint8_t flags, ir_type ret_type, ir_type t1, ir_type t2, ir_type t3)
{
	ir_proto_t proto;

	proto.flags = flags;
	proto.ret_type = ret_type;
	proto.params_count = 3;
	proto.param_types[0] = t1;
	proto.param_types[1] = t2;
	proto.param_types[2] = t3;
	return ir_strl(ctx, (const char *)&proto, offsetof(ir_proto_t, param_types) + 3);
}

ir_ref ir_proto_4(ir_ctx *ctx, uint8_t flags, ir_type ret_type, ir_type t1, ir_type t2, ir_type t3,
                                                                ir_type t4)
{
	ir_proto_t proto;

	proto.flags = flags;
	proto.ret_type = ret_type;
	proto.params_count = 4;
	proto.param_types[0] = t1;
	proto.param_types[1] = t2;
	proto.param_types[2] = t3;
	proto.param_types[3] = t4;
	return ir_strl(ctx, (const char *)&proto, offsetof(ir_proto_t, param_types) + 4);
}

ir_ref ir_proto_5(ir_ctx *ctx, uint8_t flags, ir_type ret_type, ir_type t1, ir_type t2, ir_type t3,
                                                                ir_type t4, ir_type t5)
{
	ir_proto_t proto;

	proto.flags = flags;
	proto.ret_type = ret_type;
	proto.params_count = 5;
	proto.param_types[0] = t1;
	proto.param_types[1] = t2;
	proto.param_types[2] = t3;
	proto.param_types[3] = t4;
	proto.param_types[4] = t5;
	return ir_strl(ctx, (const char *)&proto, offsetof(ir_proto_t, param_types) + 5);
}

ir_ref ir_proto(ir_ctx *ctx, uint8_t flags, ir_type ret_type, uint32_t params_count, uint8_t *param_types)
{
	ir_proto_t *proto = alloca(offsetof(ir_proto_t, param_types) + params_count);

	IR_ASSERT(params_count <= IR_MAX_PROTO_PARAMS);
	proto->flags = flags;
	proto->ret_type = ret_type;
	proto->params_count = params_count;
	memcpy(proto->param_types, param_types, params_count);
	return ir_strl(ctx, (const char *)proto, offsetof(ir_proto_t, param_types) + params_count);
}

/* IR construction */
ir_ref ir_emit(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2, ir_ref op3)
{
	ir_ref   ref = ir_next_insn(ctx);
	ir_insn *insn = &ctx->ir_base[ref];

	insn->optx = opt;
	insn->op1 = op1;
	insn->op2 = op2;
	insn->op3 = op3;

	return ref;
}

ir_ref ir_emit0(ir_ctx *ctx, uint32_t opt)
{
	return ir_emit(ctx, opt, IR_UNUSED, IR_UNUSED, IR_UNUSED);
}

ir_ref ir_emit1(ir_ctx *ctx, uint32_t opt, ir_ref op1)
{
	return ir_emit(ctx, opt, op1, IR_UNUSED, IR_UNUSED);
}

ir_ref ir_emit2(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2)
{
	return ir_emit(ctx, opt, op1, op2, IR_UNUSED);
}

ir_ref ir_emit3(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2, ir_ref op3)
{
	return ir_emit(ctx, opt, op1, op2, op3);
}

static ir_ref _ir_fold_cse(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2, ir_ref op3)
{
	ir_ref ref = ctx->prev_insn_chain[opt & IR_OPT_OP_MASK];
	ir_insn *insn;

	if (ref) {
		ir_ref limit = ctx->fold_cse_limit;

		if (op1 > limit) {
			limit = op1;
		}
		if (op2 > limit) {
			limit = op2;
		}
		if (op3 > limit) {
			limit = op3;
		}
		while (ref >= limit) {
			insn = &ctx->ir_base[ref];
			if (insn->opt == opt && insn->op1 == op1 && insn->op2 == op2 && insn->op3 == op3) {
				return ref;
			}
			if (!insn->prev_insn_offset) {
				break;
			}
			ref = ref - (ir_ref)(uint32_t)insn->prev_insn_offset;
		}
	}

	return IR_UNUSED;
}

#define IR_FOLD(X)        IR_FOLD1(X, __LINE__)
#define IR_FOLD1(X, Y)    IR_FOLD2(X, Y)
#define IR_FOLD2(X, Y)    case IR_RULE_ ## Y:

#define IR_FOLD_ERROR(msg) do { \
		IR_ASSERT(0 && (msg)); \
		goto ir_fold_emit; \
	} while (0)

#define IR_FOLD_CONST_U(_val) do { \
		val.u64 = (_val); \
		goto ir_fold_const; \
	} while (0)

#define IR_FOLD_CONST_I(_val) do { \
		val.i64 = (_val); \
		goto ir_fold_const; \
	} while (0)

#define IR_FOLD_CONST_D(_val) do { \
		val.d = (_val); \
		goto ir_fold_const; \
	} while (0)

#define IR_FOLD_CONST_F(_val) do { \
		val.f = (_val); \
		val.u32_hi = 0; \
		goto ir_fold_const; \
	} while (0)

#define IR_FOLD_COPY(op)  do { \
		ref = (op); \
		goto ir_fold_copy; \
	} while (0)

#define IR_FOLD_BOOL(cond) \
	IR_FOLD_COPY((cond) ? IR_TRUE : IR_FALSE)

#define IR_FOLD_NAMED(name)    ir_fold_ ## name:
#define IR_FOLD_DO_NAMED(name) goto ir_fold_ ## name
#define IR_FOLD_RESTART        goto ir_fold_restart
#define IR_FOLD_CSE            goto ir_fold_cse
#define IR_FOLD_EMIT           goto ir_fold_emit
#define IR_FOLD_NEXT           break

#include "ir_fold_hash.h"

#define IR_FOLD_RULE(x) ((x) >> 21)
#define IR_FOLD_KEY(x)  ((x) & 0x1fffff)

/*
 * key = insn->op | (insn->op1->op << 7) | (insn->op2->op << 14)
 *
 * ANY and UNUSED ops are represented by 0
 */

ir_ref ir_folding(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2, ir_ref op3, ir_insn *op1_insn, ir_insn *op2_insn, ir_insn *op3_insn)
{
	uint8_t op;
	ir_ref ref;
	ir_val val;
	uint32_t key, any;
	(void) op3_insn;

restart:
	key = (opt & IR_OPT_OP_MASK) + ((uint32_t)op1_insn->op << 7) + ((uint32_t)op2_insn->op << 14);
	any = 0x1fffff;
	do {
		uint32_t k = key & any;
		uint32_t h = _ir_fold_hashkey(k);
		uint32_t fh = _ir_fold_hash[h];
		if (IR_FOLD_KEY(fh) == k /*|| (fh = _ir_fold_hash[h+1], (fh & 0x1fffff) == k)*/) {
			switch (IR_FOLD_RULE(fh)) {
#include "ir_fold.h"
				default:
					break;
			}
		}
		if (any == 0x7f) {
			/* All parrerns are checked. Pass on to CSE. */
			goto ir_fold_cse;
		}
		/* op2/op1/op  op2/_/op    _/op1/op    _/_/op
		 * 0x1fffff -> 0x1fc07f -> 0x003fff -> 0x00007f
		 * from masks to bis: 11 -> 10 -> 01 -> 00
		 *
		 * a b  => x y
		 * 1 1     1 0
		 * 1 0     0 1
		 * 0 1     0 0
		 *
		 * x = a & b; y = !b
		 */
		any = ((any & (any << 7)) & 0x1fc000) | (~any & 0x3f80) | 0x7f;
	} while (1);

ir_fold_restart:
	if (!(ctx->flags2 & IR_OPT_IN_SCCP)) {
		op1_insn = ctx->ir_base + op1;
		op2_insn = ctx->ir_base + op2;
		op3_insn = ctx->ir_base + op3;
		goto restart;
	} else {
		ctx->fold_insn.optx = opt;
		ctx->fold_insn.op1 = op1;
		ctx->fold_insn.op2 = op2;
		ctx->fold_insn.op3 = op3;
		return IR_FOLD_DO_RESTART;
	}
ir_fold_cse:
	if (!(ctx->flags2 & IR_OPT_IN_SCCP)) {
		/* Local CSE */
		ref = _ir_fold_cse(ctx, opt, op1, op2, op3);
		if (ref) {
			return ref;
		}

		ref = ir_emit(ctx, opt, op1, op2, op3);

		/* Update local CSE chain */
		op = opt & IR_OPT_OP_MASK;
		ir_ref prev = ctx->prev_insn_chain[op];
		ir_insn *insn = ctx->ir_base + ref;
		if (!prev || ref - prev > 0xffff) {
			/* can't fit into 16-bit */
			insn->prev_insn_offset = 0;
		} else {
			insn->prev_insn_offset = ref - prev;
		}
		ctx->prev_insn_chain[op] = ref;

		return ref;
	}
ir_fold_emit:
	if (!(ctx->flags2 & IR_OPT_IN_SCCP)) {
		return ir_emit(ctx, opt, op1, op2, op3);
	} else {
		ctx->fold_insn.optx = opt;
		ctx->fold_insn.op1 = op1;
		ctx->fold_insn.op2 = op2;
		ctx->fold_insn.op3 = op3;
		return IR_FOLD_DO_EMIT;
	}
ir_fold_copy:
	if (!(ctx->flags2 & IR_OPT_IN_SCCP)) {
		return ref;
	} else {
		ctx->fold_insn.op1 = ref;
		return IR_FOLD_DO_COPY;
	}
ir_fold_const:
	if (!(ctx->flags2 & IR_OPT_IN_SCCP)) {
		return ir_const(ctx, val, IR_OPT_TYPE(opt));
	} else {
		ctx->fold_insn.type = IR_OPT_TYPE(opt);
		ctx->fold_insn.val.u64 = val.u64;
		return IR_FOLD_DO_CONST;
	}
}

ir_ref ir_fold(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2, ir_ref op3)
{
	if (UNEXPECTED(!(ctx->flags & IR_OPT_FOLDING))) {
		if ((opt & IR_OPT_OP_MASK) == IR_PHI) {
			opt |= (3 << IR_OPT_INPUTS_SHIFT);
		}
		return ir_emit(ctx, opt, op1, op2, op3);
	}
	return ir_folding(ctx, opt, op1, op2, op3, ctx->ir_base + op1, ctx->ir_base + op2, ctx->ir_base + op3);
}

ir_ref ir_fold0(ir_ctx *ctx, uint32_t opt)
{
	return ir_fold(ctx, opt, IR_UNUSED, IR_UNUSED, IR_UNUSED);
}

ir_ref ir_fold1(ir_ctx *ctx, uint32_t opt, ir_ref op1)
{
	return ir_fold(ctx, opt, op1, IR_UNUSED, IR_UNUSED);
}

ir_ref ir_fold2(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2)
{
	return ir_fold(ctx, opt, op1, op2, IR_UNUSED);
}

ir_ref ir_fold3(ir_ctx *ctx, uint32_t opt, ir_ref op1, ir_ref op2, ir_ref op3)
{
	return ir_fold(ctx, opt, op1, op2, op3);
}

ir_ref ir_emit_N(ir_ctx *ctx, uint32_t opt, int32_t count)
{
	int i;
	ir_ref *p, ref = ctx->insns_count;
	ir_insn *insn;

	IR_ASSERT(count >= 0);
	while (UNEXPECTED(ref + count/4 >= ctx->insns_limit)) {
		ir_grow_top(ctx);
	}
	ctx->insns_count = ref + 1 + count/4;

	insn = &ctx->ir_base[ref];
	insn->optx = opt | (count << IR_OPT_INPUTS_SHIFT);
	for (i = 1, p = insn->ops + i; i <= (count|3); i++, p++) {
		*p = IR_UNUSED;
	}

	return ref;
}

void ir_set_op(ir_ctx *ctx, ir_ref ref, int32_t n, ir_ref val)
{
	ir_insn *insn = &ctx->ir_base[ref];

#ifdef IR_DEBUG
	if (n > 3) {
		int32_t count;

		IR_ASSERT(IR_OP_HAS_VAR_INPUTS(ir_op_flags[insn->op]));
		count = insn->inputs_count;
		IR_ASSERT(n <= count);
	}
#endif
	ir_insn_set_op(insn, n, val);
}

ir_ref ir_param(ir_ctx *ctx, ir_type type, ir_ref region, const char *name, int pos)
{
	return ir_emit(ctx, IR_OPT(IR_PARAM, type), region, ir_str(ctx, name), pos);
}

ir_ref ir_var(ir_ctx *ctx, ir_type type, ir_ref region, const char *name)
{
	return ir_emit(ctx, IR_OPT(IR_VAR, type), region, ir_str(ctx, name), IR_UNUSED);
}

ir_ref ir_bind(ir_ctx *ctx, ir_ref var, ir_ref def)
{
	if (IR_IS_CONST_REF(def)) {
		return def;
	}
	if (!ctx->binding) {
		ctx->binding = ir_mem_malloc(sizeof(ir_hashtab));;
		ir_hashtab_init(ctx->binding, 16);
	}
	/* Node may be bound to some special spill slot (using negative "var") */
	IR_ASSERT(var < 0);
	if (!ir_hashtab_add(ctx->binding, def, var)) {
		/* Add a copy with different binding */
		def = ir_emit2(ctx, IR_OPT(IR_COPY, ctx->ir_base[def].type), def, 1);
		ir_hashtab_add(ctx->binding, def, var);
	}
	return def;
}

/* Batch construction of def->use edges */
#if 0
void ir_build_def_use_lists(ir_ctx *ctx)
{
	ir_ref n, i, j, *p, def;
	ir_insn *insn;
	uint32_t edges_count;
	ir_use_list *lists = ir_mem_calloc(ctx->insns_count, sizeof(ir_use_list));
	ir_ref *edges;
	ir_use_list *use_list;

	for (i = IR_UNUSED + 1, insn = ctx->ir_base + i; i < ctx->insns_count;) {
		uint32_t flags = ir_op_flags[insn->op];

		if (UNEXPECTED(IR_OP_HAS_VAR_INPUTS(flags))) {
			n = insn->inputs_count;
		} else {
			n = insn->inputs_count = IR_INPUT_EDGES_COUNT(flags);
		}
		for (j = n, p = insn->ops + 1; j > 0; j--, p++) {
			def = *p;
			if (def > 0) {
				lists[def].count++;
			}
		}
		n = ir_insn_inputs_to_len(n);
		i += n;
		insn += n;
	}

	edges_count = 0;
	for (i = IR_UNUSED + 1, use_list = &lists[i]; i < ctx->insns_count; i++, use_list++) {
		use_list->refs = edges_count;
		edges_count += use_list->count;
		use_list->count = 0;
	}

	edges = ir_mem_malloc(edges_count * sizeof(ir_ref));
	for (i = IR_UNUSED + 1, insn = ctx->ir_base + i; i < ctx->insns_count;) {
		n = insn->inputs_count;
		for (j = n, p = insn->ops + 1; j > 0; j--, p++) {
			def = *p;
			if (def > 0) {
				use_list = &lists[def];
				edges[use_list->refs + use_list->count++] = i;
			}
		}
		n = ir_insn_inputs_to_len(n);
		i += n;
		insn += n;
	}

	ctx->use_edges = edges;
	ctx->use_edges_count = edges_count;
	ctx->use_lists = lists;
}
#else
void ir_build_def_use_lists(ir_ctx *ctx)
{
	ir_ref n, i, j, *p, def;
	ir_insn *insn;
	size_t linked_lists_size, linked_lists_top = 0, edges_count = 0;
	ir_use_list *lists = ir_mem_calloc(ctx->insns_count, sizeof(ir_use_list));
	ir_ref *edges;
	ir_use_list *use_list;
	ir_ref *linked_lists;

	linked_lists_size = IR_ALIGNED_SIZE(ctx->insns_count, 1024);
	linked_lists = ir_mem_malloc(linked_lists_size * sizeof(ir_ref));
	for (i = IR_UNUSED + 1, insn = ctx->ir_base + i; i < ctx->insns_count;) {
		uint32_t flags = ir_op_flags[insn->op];

		if (UNEXPECTED(IR_OP_HAS_VAR_INPUTS(flags))) {
			n = insn->inputs_count;
		} else {
			n = insn->inputs_count = IR_INPUT_EDGES_COUNT(flags);
		}
		for (j = n, p = insn->ops + 1; j > 0; j--, p++) {
			def = *p;
			if (def > 0) {
				use_list = &lists[def];
				edges_count++;
				if (!use_list->refs) {
					/* store a single "use" directly in "refs" using a positive number */
					use_list->refs = i;
					use_list->count = 1;
				} else {
					if (UNEXPECTED(linked_lists_top >= linked_lists_size)) {
						linked_lists_size += 1024;
						linked_lists = ir_mem_realloc(linked_lists, linked_lists_size * sizeof(ir_ref));
					}
					/* form a linked list of "uses" (like in binsort) */
					linked_lists[linked_lists_top] = i; /* store the "use" */
					linked_lists[linked_lists_top + 1] = use_list->refs; /* store list next */
					use_list->refs = -(linked_lists_top + 1); /* store a head of the list using a negative number */
					linked_lists_top += 2;
					use_list->count++;
				}
			}
		}
		n = ir_insn_inputs_to_len(n);
		i += n;
		insn += n;
	}

	ctx->use_edges_count = edges_count;
	edges = ir_mem_malloc(edges_count * sizeof(ir_ref));
	for (use_list = lists + ctx->insns_count - 1; use_list != lists; use_list--) {
		n = use_list->refs;
		if (n) {
			/* transform linked list to plain array */
			while (n < 0) {
				n = -n;
				edges[--edges_count] = linked_lists[n - 1];
				n = linked_lists[n];
			}
			IR_ASSERT(n > 0);
			edges[--edges_count] = n;
			use_list->refs = edges_count;
		}
	}

	ctx->use_edges = edges;
	ctx->use_lists = lists;
	ir_mem_free(linked_lists);
}
#endif

/* Helper Data Types */
void ir_array_grow(ir_array *a, uint32_t size)
{
	IR_ASSERT(size > a->size);
	a->refs = ir_mem_realloc(a->refs, size * sizeof(ir_ref));
	a->size = size;
}

void ir_array_insert(ir_array *a, uint32_t i, ir_ref val)
{
	IR_ASSERT(i < a->size);
	if (a->refs[a->size - 1]) {
		ir_array_grow(a, a->size + 1);
	}
	memmove(a->refs + i + 1, a->refs + i, (a->size - i - 1) * sizeof(ir_ref));
	a->refs[i] = val;
}

void ir_array_remove(ir_array *a, uint32_t i)
{
	IR_ASSERT(i < a->size);
	memmove(a->refs + i, a->refs + i + 1, (a->size - i - 1) * sizeof(ir_ref));
	a->refs[a->size - 1] = IR_UNUSED;
}

void ir_list_insert(ir_list *l, uint32_t i, ir_ref val)
{
	IR_ASSERT(i < l->len);
	if (l->len >= l->a.size) {
		ir_array_grow(&l->a, l->a.size + 1);
	}
	memmove(l->a.refs + i + 1, l->a.refs + i, (l->len - i) * sizeof(ir_ref));
	l->a.refs[i] = val;
	l->len++;
}

void ir_list_remove(ir_list *l, uint32_t i)
{
	IR_ASSERT(i < l->len);
	memmove(l->a.refs + i, l->a.refs + i + 1, (l->len - i) * sizeof(ir_ref));
	l->len--;
}

bool ir_list_contains(const ir_list *l, ir_ref val)
{
	uint32_t i;

	for (i = 0; i < l->len; i++) {
		if (ir_array_at(&l->a, i) == val) {
			return 1;
		}
	}
	return 0;
}

static uint32_t ir_hashtab_hash_size(uint32_t size)
{
	size -= 1;
	size |= (size >> 1);
	size |= (size >> 2);
	size |= (size >> 4);
	size |= (size >> 8);
	size |= (size >> 16);
	return IR_MAX(size + 1, 4);
}

static void ir_hashtab_resize(ir_hashtab *tab)
{
	uint32_t old_hash_size = (uint32_t)(-(int32_t)tab->mask);
	char *old_data = tab->data;
	uint32_t size = tab->size * 2;
	uint32_t hash_size = ir_hashtab_hash_size(size);
	char *data = ir_mem_malloc(hash_size * sizeof(uint32_t) + size * sizeof(ir_hashtab_bucket));
	ir_hashtab_bucket *p;
	uint32_t pos, i;

	memset(data, -1, hash_size * sizeof(uint32_t));
	tab->data = data + (hash_size * sizeof(uint32_t));
	tab->mask = (uint32_t)(-(int32_t)hash_size);
	tab->size = size;

	memcpy(tab->data, old_data, tab->count * sizeof(ir_hashtab_bucket));
	ir_mem_free(old_data - (old_hash_size * sizeof(uint32_t)));

	i = tab->count;
	pos = 0;
	p = (ir_hashtab_bucket*)tab->data;
	do {
		uint32_t key = p->key | tab->mask;
		p->next = ((uint32_t*)tab->data)[(int32_t)key];
		((uint32_t*)tab->data)[(int32_t)key] = pos;
		pos += sizeof(ir_hashtab_bucket);
		p++;
	} while (--i);
}

void ir_hashtab_init(ir_hashtab *tab, uint32_t size)
{
	IR_ASSERT(size > 0);
	uint32_t hash_size = ir_hashtab_hash_size(size);
	char *data = ir_mem_malloc(hash_size * sizeof(uint32_t) + size * sizeof(ir_hashtab_bucket));
	memset(data, -1, hash_size * sizeof(uint32_t));
	tab->data = (data + (hash_size * sizeof(uint32_t)));
	tab->mask = (uint32_t)(-(int32_t)hash_size);
	tab->size = size;
	tab->count = 0;
	tab->pos = 0;
}

void ir_hashtab_free(ir_hashtab *tab)
{
	uint32_t hash_size = (uint32_t)(-(int32_t)tab->mask);
	char *data = (char*)tab->data - (hash_size * sizeof(uint32_t));
	ir_mem_free(data);
	tab->data = NULL;
}

ir_ref ir_hashtab_find(const ir_hashtab *tab, uint32_t key)
{
	const char *data = (const char*)tab->data;
	uint32_t pos = ((uint32_t*)data)[(int32_t)(key | tab->mask)];
	ir_hashtab_bucket *p;

	while (pos != IR_INVALID_IDX) {
		p = (ir_hashtab_bucket*)(data + pos);
		if (p->key == key) {
			return p->val;
		}
		pos = p->next;
	}
	return IR_INVALID_VAL;
}

bool ir_hashtab_add(ir_hashtab *tab, uint32_t key, ir_ref val)
{
	char *data = (char*)tab->data;
	uint32_t pos = ((uint32_t*)data)[(int32_t)(key | tab->mask)];
	ir_hashtab_bucket *p;

	while (pos != IR_INVALID_IDX) {
		p = (ir_hashtab_bucket*)(data + pos);
		if (p->key == key) {
			return p->val == val;
		}
		pos = p->next;
	}

	if (UNEXPECTED(tab->count >= tab->size)) {
		ir_hashtab_resize(tab);
		data = tab->data;
	}

	pos = tab->pos;
	tab->pos += sizeof(ir_hashtab_bucket);
	tab->count++;
	p = (ir_hashtab_bucket*)(data + pos);
	p->key = key;
	p->val = val;
	key |= tab->mask;
	p->next = ((uint32_t*)data)[(int32_t)key];
	((uint32_t*)data)[(int32_t)key] = pos;
	return 1;
}

static int ir_hashtab_key_cmp(const void *b1, const void *b2)
{
	return ((ir_hashtab_bucket*)b1)->key - ((ir_hashtab_bucket*)b2)->key;
}

void ir_hashtab_key_sort(ir_hashtab *tab)
{
	ir_hashtab_bucket *p;
	uint32_t hash_size, pos, i;

	if (!tab->count) {
		return;
	}

	qsort(tab->data, tab->count, sizeof(ir_hashtab_bucket), ir_hashtab_key_cmp);

	hash_size = ir_hashtab_hash_size(tab->size);
	memset((char*)tab->data - (hash_size * sizeof(uint32_t)), -1, hash_size * sizeof(uint32_t));

	i = tab->count;
	pos = 0;
	p = (ir_hashtab_bucket*)tab->data;
	do {
		uint32_t key = p->key | tab->mask;
		p->next = ((uint32_t*)tab->data)[(int32_t)key];
		((uint32_t*)tab->data)[(int32_t)key] = pos;
		pos += sizeof(ir_hashtab_bucket);
		p++;
	} while (--i);
}

static void ir_addrtab_resize(ir_hashtab *tab)
{
	uint32_t old_hash_size = (uint32_t)(-(int32_t)tab->mask);
	char *old_data = tab->data;
	uint32_t size = tab->size * 2;
	uint32_t hash_size = ir_hashtab_hash_size(size);
	char *data = ir_mem_malloc(hash_size * sizeof(uint32_t) + size * sizeof(ir_addrtab_bucket));
	ir_addrtab_bucket *p;
	uint32_t pos, i;

	memset(data, -1, hash_size * sizeof(uint32_t));
	tab->data = data + (hash_size * sizeof(uint32_t));
	tab->mask = (uint32_t)(-(int32_t)hash_size);
	tab->size = size;

	memcpy(tab->data, old_data, tab->count * sizeof(ir_addrtab_bucket));
	ir_mem_free(old_data - (old_hash_size * sizeof(uint32_t)));

	i = tab->count;
	pos = 0;
	p = (ir_addrtab_bucket*)tab->data;
	do {
		uint32_t key = (uint32_t)p->key | tab->mask;
		p->next = ((uint32_t*)tab->data)[(int32_t)key];
		((uint32_t*)tab->data)[(int32_t)key] = pos;
		pos += sizeof(ir_addrtab_bucket);
		p++;
	} while (--i);
}

void ir_addrtab_init(ir_hashtab *tab, uint32_t size)
{
	IR_ASSERT(size > 0);
	uint32_t hash_size = ir_hashtab_hash_size(size);
	char *data = ir_mem_malloc(hash_size * sizeof(uint32_t) + size * sizeof(ir_addrtab_bucket));
	memset(data, -1, hash_size * sizeof(uint32_t));
	tab->data = (data + (hash_size * sizeof(uint32_t)));
	tab->mask = (uint32_t)(-(int32_t)hash_size);
	tab->size = size;
	tab->count = 0;
	tab->pos = 0;
}

void ir_addrtab_free(ir_hashtab *tab)
{
	uint32_t hash_size = (uint32_t)(-(int32_t)tab->mask);
	char *data = (char*)tab->data - (hash_size * sizeof(uint32_t));
	ir_mem_free(data);
	tab->data = NULL;
}

ir_ref ir_addrtab_find(const ir_hashtab *tab, uint64_t key)
{
	const char *data = (const char*)tab->data;
	uint32_t pos = ((uint32_t*)data)[(int32_t)(key | tab->mask)];
	ir_addrtab_bucket *p;

	while (pos != IR_INVALID_IDX) {
		p = (ir_addrtab_bucket*)(data + pos);
		if (p->key == key) {
			return p->val;
		}
		pos = p->next;
	}
	return IR_INVALID_VAL;
}

bool ir_addrtab_add(ir_hashtab *tab, uint64_t key, ir_ref val)
{
	char *data = (char*)tab->data;
	uint32_t pos = ((uint32_t*)data)[(int32_t)(key | tab->mask)];
	ir_addrtab_bucket *p;

	while (pos != IR_INVALID_IDX) {
		p = (ir_addrtab_bucket*)(data + pos);
		if (p->key == key) {
			return p->val == val;
		}
		pos = p->next;
	}

	if (UNEXPECTED(tab->count >= tab->size)) {
		ir_addrtab_resize(tab);
		data = tab->data;
	}

	pos = tab->pos;
	tab->pos += sizeof(ir_addrtab_bucket);
	tab->count++;
	p = (ir_addrtab_bucket*)(data + pos);
	p->key = key;
	p->val = val;
	key |= tab->mask;
	p->next = ((uint32_t*)data)[(int32_t)key];
	((uint32_t*)data)[(int32_t)key] = pos;
	return 1;
}

/* Memory API */
#ifdef _WIN32
void *ir_mem_mmap(size_t size)
{
	void *ret;

#ifdef _M_X64
	DWORD size_hi = size >> 32, size_lo = size & 0xffffffff;
#else
	DWORD size_hi = 0, size_lo = size;
#endif

	HANDLE h = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_EXECUTE_READWRITE, size_hi, size_lo, NULL);

	ret = MapViewOfFile(h, FILE_MAP_READ | FILE_MAP_WRITE | FILE_MAP_EXECUTE, 0, 0, size);
	if (!ret) {
		CloseHandle(h);
	}

	return ret;
}

int ir_mem_unmap(void *ptr, size_t size)
{
	/* XXX file handle is leaked. */
	UnmapViewOfFile(ptr);
	return 1;
}

int ir_mem_protect(void *ptr, size_t size)
{
	return 1;
}

int ir_mem_unprotect(void *ptr, size_t size)
{
	return 1;
}

int ir_mem_flush(void *ptr, size_t size)
{
	return 1;
}
#else
void *ir_mem_mmap(size_t size)
{
        int prot_flags = PROT_EXEC;
#if defined(__NetBSD__)
	prot_flags |= PROT_MPROTECT(PROT_READ|PROT_WRITE);
#endif
	void *ret = mmap(NULL, size, prot_flags, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (ret == MAP_FAILED) {
		ret = NULL;
	}
	return ret;
}

int ir_mem_unmap(void *ptr, size_t size)
{
	munmap(ptr, size);
	return 1;
}

int ir_mem_protect(void *ptr, size_t size)
{
	if (mprotect(ptr, size, PROT_READ | PROT_EXEC) != 0) {
#ifdef IR_DEBUG
		fprintf(stderr, "mprotect() failed\n");
#endif
		return 0;
	}
	return 1;
}

int ir_mem_unprotect(void *ptr, size_t size)
{
	if (mprotect(ptr, size, PROT_READ | PROT_WRITE) != 0) {
#ifdef IR_DEBUG
		fprintf(stderr, "mprotect() failed\n");
#endif
		return 0;
	}
	return 1;
}

int ir_mem_flush(void *ptr, size_t size)
{
#if ((defined(__GNUC__) && ZEND_GCC_VERSION >= 4003) || __has_builtin(__builtin___clear_cache))
	__builtin___clear_cache((char*)(ptr), (char*)(ptr) + size);
#endif
#if defined(__APPLE__) && defined(__aarch64__)
	sys_icache_invalidate(ptr, size);
#endif
#ifdef HAVE_VALGRIND
	VALGRIND_DISCARD_TRANSLATIONS(ptr, size);
#endif
	return 1;
}
#endif

/* Alias Analyses */
typedef enum _ir_alias {
	IR_MAY_ALIAS  = -1,
	IR_NO_ALIAS   =  0,
	IR_MUST_ALIAS =  1,
} ir_alias;

#if 0
static ir_alias ir_check_aliasing(ir_ctx *ctx, ir_ref addr1, ir_ref addr2)
{
	ir_insn *insn1, *insn2;

	if (addr1 == addr2) {
		return IR_MUST_ALIAS;
	}

	insn1 = &ctx->ir_base[addr1];
	insn2 = &ctx->ir_base[addr2];
	if (insn1->op == IR_ADD && IR_IS_CONST_REF(insn1->op2)) {
		if (insn1->op1 == addr2) {
			uintptr_t offset1 = ctx->ir_base[insn1->op2].val.u64;
			return (offset1 != 0) ? IR_MUST_ALIAS : IR_NO_ALIAS;
		} else if (insn2->op == IR_ADD && IR_IS_CONST_REF(insn1->op2) && insn1->op1 == insn2->op1) {
			if (insn1->op2 == insn2->op2) {
				return IR_MUST_ALIAS;
			} else if (IR_IS_CONST_REF(insn1->op2) && IR_IS_CONST_REF(insn2->op2)) {
				uintptr_t offset1 = ctx->ir_base[insn1->op2].val.u64;
				uintptr_t offset2 = ctx->ir_base[insn2->op2].val.u64;

				return (offset1 == offset2) ? IR_MUST_ALIAS : IR_NO_ALIAS;
			}
		}
	} else if (insn2->op == IR_ADD && IR_IS_CONST_REF(insn2->op2)) {
		if (insn2->op1 == addr1) {
			uintptr_t offset2 = ctx->ir_base[insn2->op2].val.u64;

			return (offset2 != 0) ? IR_MUST_ALIAS : IR_NO_ALIAS;
		}
	}
	return IR_MAY_ALIAS;
}
#endif

static ir_alias ir_check_partial_aliasing(const ir_ctx *ctx, ir_ref addr1, ir_ref addr2, ir_type type1, ir_type type2)
{
	ir_insn *insn1, *insn2;

	/* this must be already check */
	IR_ASSERT(addr1 != addr2);

	insn1 = &ctx->ir_base[addr1];
	insn2 = &ctx->ir_base[addr2];
	if (insn1->op == IR_ADD && IR_IS_CONST_REF(insn1->op2)) {
		if (insn1->op1 == addr2) {
			uintptr_t offset1 = ctx->ir_base[insn1->op2].val.addr;
			uintptr_t size2 = ir_type_size[type2];

			return (offset1 < size2) ? IR_MUST_ALIAS : IR_NO_ALIAS;
		} else if (insn2->op == IR_ADD && IR_IS_CONST_REF(insn1->op2) && insn1->op1 == insn2->op1) {
			if (insn1->op2 == insn2->op2) {
				return IR_MUST_ALIAS;
			} else if (IR_IS_CONST_REF(insn1->op2) && IR_IS_CONST_REF(insn2->op2)) {
				uintptr_t offset1 = ctx->ir_base[insn1->op2].val.addr;
				uintptr_t offset2 = ctx->ir_base[insn2->op2].val.addr;

				if (offset1 == offset2) {
					return IR_MUST_ALIAS;
				} else if (type1 == type2) {
					return IR_NO_ALIAS;
				} else {
					/* check for partail intersection */
					uintptr_t size1 = ir_type_size[type1];
					uintptr_t size2 = ir_type_size[type2];

					if (offset1	> offset2) {
						return offset1 < offset2 + size2 ? IR_MUST_ALIAS : IR_NO_ALIAS;
					} else {
						return offset2 < offset1 + size1 ? IR_MUST_ALIAS : IR_NO_ALIAS;
					}
				}
			}
		}
	} else if (insn2->op == IR_ADD && IR_IS_CONST_REF(insn2->op2)) {
		if (insn2->op1 == addr1) {
			uintptr_t offset2 = ctx->ir_base[insn2->op2].val.addr;
			uintptr_t size1 = ir_type_size[type1];

			return (offset2 < size1) ? IR_MUST_ALIAS : IR_NO_ALIAS;
		}
	}
	return IR_MAY_ALIAS;
}

static ir_ref ir_find_aliasing_load(ir_ctx *ctx, ir_ref ref, ir_type type, ir_ref addr)
{
	ir_ref limit = (addr > 0) ? addr : 1;
	ir_insn *insn;
	uint32_t modified_regset = 0;

	while (ref > limit) {
		insn = &ctx->ir_base[ref];
		if (insn->op == IR_LOAD) {
			if (insn->type == type && insn->op2 == addr) {
				return ref; /* load forwarding (L2L) */
			}
		} else if (insn->op == IR_STORE) {
			ir_type type2 = ctx->ir_base[insn->op3].type;

			if (insn->op2 == addr) {
				if (type2 == type) {
					ref = insn->op3;
					insn = &ctx->ir_base[ref];
					if (insn->op == IR_RLOAD && (modified_regset & (1 << insn->op2))) {
						/* anti-dependency */
						return IR_UNUSED;
					}
					return ref; /* store forwarding (S2L) */
				} else if (IR_IS_TYPE_INT(type) && ir_type_size[type2] > ir_type_size[type]) {
					return ir_fold1(ctx, IR_OPT(IR_TRUNC, type), insn->op3); /* partial store forwarding (S2L) */
				} else {
					return IR_UNUSED;
				}
			} else if (ir_check_partial_aliasing(ctx, addr, insn->op2, type, type2) != IR_NO_ALIAS) {
				return IR_UNUSED;
			}
		} else if (insn->op == IR_RSTORE) {
			modified_regset |= (1 << insn->op3);
		} else if (insn->op == IR_MERGE || insn->op == IR_LOOP_BEGIN || insn->op == IR_CALL || insn->op == IR_VSTORE) {
			return IR_UNUSED;
		}
		ref = insn->op1;
	}
	return IR_UNUSED;
}

/* IR Construction API */

ir_ref _ir_PARAM(ir_ctx *ctx, ir_type type, const char* name, ir_ref num)
{
	IR_ASSERT(ctx->control);
	IR_ASSERT(ctx->ir_base[ctx->control].op == IR_START);
	IR_ASSERT(ctx->insns_count == num + 1);
	return ir_param(ctx, type, ctx->control, name, num);
}

ir_ref _ir_VAR(ir_ctx *ctx, ir_type type, const char* name)
{
//	IR_ASSERT(ctx->control);
//	IR_ASSERT(IR_IS_BB_START(ctx->ir_base[ctx->control].op));
//	TODO: VAR may be insterted after some "memory" instruction
	ir_ref ref = ctx->control;

	while (1) {
		IR_ASSERT(ctx->control);
		if (IR_IS_BB_START(ctx->ir_base[ref].op)) {
			break;
		}
		ref = ctx->ir_base[ref].op1;
	}
	return ir_var(ctx, type, ref, name);
}

ir_ref _ir_PHI_2(ir_ctx *ctx, ir_type type, ir_ref src1, ir_ref src2)
{
	IR_ASSERT(ctx->control);
	IR_ASSERT(ctx->ir_base[ctx->control].op == IR_MERGE || ctx->ir_base[ctx->control].op == IR_LOOP_BEGIN);
	if (src1 == src2 && src1 != IR_UNUSED) {
		return src1;
	}
	return ir_emit3(ctx, IR_OPTX(IR_PHI, type, 3), ctx->control, src1, src2);
}

ir_ref _ir_PHI_N(ir_ctx *ctx, ir_type type, ir_ref n, ir_ref *inputs)
{
	IR_ASSERT(ctx->control);
	IR_ASSERT(n > 0);
	if (n == 1) {
		return inputs[0];
	} else {
	    ir_ref i;
		ir_ref ref = inputs[0];

		IR_ASSERT(ctx->ir_base[ctx->control].op == IR_MERGE || ctx->ir_base[ctx->control].op == IR_LOOP_BEGIN);
		if (ref != IR_UNUSED) {
			for (i = 1; i < n; i++) {
				if (inputs[i] != ref) {
					break;
				}
			}
			if (i == n) {
				/* all the same */
				return ref;
			}
		}

		ref = ir_emit_N(ctx, IR_OPT(IR_PHI, type), n + 1);
		ir_set_op(ctx, ref, 1, ctx->control);
		for (i = 0; i < n; i++) {
			ir_set_op(ctx, ref, i + 2, inputs[i]);
		}
		return ref;
	}
}

void _ir_PHI_SET_OP(ir_ctx *ctx, ir_ref phi, ir_ref pos, ir_ref src)
{
	ir_insn *insn = &ctx->ir_base[phi];
	ir_ref *ops = insn->ops;

	IR_ASSERT(insn->op == IR_PHI);
	IR_ASSERT(ctx->ir_base[insn->op1].op == IR_MERGE || ctx->ir_base[insn->op1].op == IR_LOOP_BEGIN);
	IR_ASSERT(pos > 0 && pos < insn->inputs_count);
	pos++; /* op1 is used for control */
	ops[pos] = src;
}

void _ir_START(ir_ctx *ctx)
{
	IR_ASSERT(!ctx->control);
	IR_ASSERT(ctx->insns_count == 1);
	ctx->control = ir_emit0(ctx, IR_START);
}

void _ir_ENTRY(ir_ctx *ctx, ir_ref src, ir_ref num)
{
	IR_ASSERT(!ctx->control);
	/* fake control edge */
	IR_ASSERT((ir_op_flags[ctx->ir_base[src].op] & IR_OP_FLAG_TERMINATOR)
		|| ctx->ir_base[src].op == IR_END
		|| ctx->ir_base[src].op == IR_LOOP_END); /* return from a recursive call */
	ctx->control = ir_emit2(ctx, IR_ENTRY, src, num);
}

void _ir_BEGIN(ir_ctx *ctx, ir_ref src)
{
	IR_ASSERT(!ctx->control);
	if (src
	 && src + 1 == ctx->insns_count
	 && ctx->ir_base[src].op == IR_END) {
		/* merge with the last END */
		ctx->control = ctx->ir_base[src].op1;
		ctx->insns_count--;
	} else {
		ctx->control = ir_emit1(ctx, IR_BEGIN, src);
	}
}

ir_ref _ir_IF(ir_ctx *ctx, ir_ref condition)
{
	ir_ref if_ref;

	IR_ASSERT(ctx->control);
	if_ref = ir_emit2(ctx, IR_IF, ctx->control, condition);
	ctx->control = IR_UNUSED;
	return if_ref;
}

void _ir_IF_TRUE(ir_ctx *ctx, ir_ref if_ref)
{
	IR_ASSERT(!ctx->control);
	IR_ASSERT(if_ref);
	IR_ASSERT(ctx->ir_base[if_ref].op == IR_IF);
	ctx->control = ir_emit1(ctx, IR_IF_TRUE, if_ref);
}

void _ir_IF_TRUE_cold(ir_ctx *ctx, ir_ref if_ref)
{
	IR_ASSERT(!ctx->control);
	IR_ASSERT(if_ref);
	IR_ASSERT(ctx->ir_base[if_ref].op == IR_IF);
	/* op2 is used as an indicator of low-probability branch */
	ctx->control = ir_emit2(ctx, IR_IF_TRUE, if_ref, 1);
}

void _ir_IF_FALSE(ir_ctx *ctx, ir_ref if_ref)
{
	IR_ASSERT(!ctx->control);
	IR_ASSERT(if_ref);
	IR_ASSERT(ctx->ir_base[if_ref].op == IR_IF);
	ctx->control = ir_emit1(ctx, IR_IF_FALSE, if_ref);
}

void _ir_IF_FALSE_cold(ir_ctx *ctx, ir_ref if_ref)
{
	IR_ASSERT(!ctx->control);
	IR_ASSERT(if_ref);
	IR_ASSERT(ctx->ir_base[if_ref].op == IR_IF);
	/* op2 is used as an indicator of low-probability branch */
	ctx->control = ir_emit2(ctx, IR_IF_FALSE, if_ref, 1);
}

ir_ref _ir_END(ir_ctx *ctx)
{
	ir_ref ref;

	IR_ASSERT(ctx->control);
	ref = ir_emit1(ctx, IR_END, ctx->control);
	ctx->control = IR_UNUSED;
	return ref;
}

void _ir_MERGE_2(ir_ctx *ctx, ir_ref src1, ir_ref src2)
{
	IR_ASSERT(!ctx->control);
	ctx->control = ir_emit2(ctx, IR_OPTX(IR_MERGE, IR_VOID, 2), src1, src2);
}

void _ir_MERGE_N(ir_ctx *ctx, ir_ref n, ir_ref *inputs)
{
	IR_ASSERT(!ctx->control);
	IR_ASSERT(n > 0);
	if (n == 1) {
		_ir_BEGIN(ctx, inputs[0]);
	} else {
		ir_ref *ops;

		ctx->control = ir_emit_N(ctx, IR_MERGE, n);
		ops = ctx->ir_base[ctx->control].ops;
		while (n) {
			n--;
			ops[n + 1] = inputs[n];
		}
	}
}

void _ir_MERGE_SET_OP(ir_ctx *ctx, ir_ref merge, ir_ref pos, ir_ref src)
{
	ir_insn *insn = &ctx->ir_base[merge];
	ir_ref *ops = insn->ops;

	IR_ASSERT(insn->op == IR_MERGE || insn->op == IR_LOOP_BEGIN);
	IR_ASSERT(pos > 0 && pos <= insn->inputs_count);
	ops[pos] = src;
}

ir_ref _ir_END_LIST(ir_ctx *ctx, ir_ref list)
{
	ir_ref ref;

	IR_ASSERT(ctx->control);
	IR_ASSERT(!list || ctx->ir_base[list].op == IR_END);
	/* create a liked list of END nodes with the same destination through END.op2 */
	ref = ir_emit2(ctx, IR_END, ctx->control, list);
	ctx->control = IR_UNUSED;
	return ref;
}

void _ir_MERGE_LIST(ir_ctx *ctx, ir_ref list)
{
	ir_ref ref = list;

	if (list != IR_UNUSED) {
		uint32_t n = 0;

		IR_ASSERT(!ctx->control);

		/* count inputs count */
		do {
			ir_insn *insn = &ctx->ir_base[ref];

			IR_ASSERT(insn->op == IR_END);
			ref = insn->op2;
			n++;
		} while (ref != IR_UNUSED);


		/* create MERGE node */
		IR_ASSERT(n > 0);
		if (n == 1) {
			ctx->ir_base[list].op2 = IR_UNUSED;
			_ir_BEGIN(ctx, list);
		} else {
			ctx->control = ir_emit_N(ctx, IR_MERGE, n);
			ref = list;
			while (n) {
				ir_insn *insn = &ctx->ir_base[ref];

				ir_set_op(ctx, ctx->control, n, ref);
				ref = insn->op2;
				insn->op2 = IR_UNUSED;
				n--;
			}
		}
	}
}

ir_ref _ir_LOOP_BEGIN(ir_ctx *ctx, ir_ref src1)
{
	IR_ASSERT(!ctx->control);
	ctx->control = ir_emit2(ctx, IR_OPTX(IR_LOOP_BEGIN, IR_VOID, 2), src1, IR_UNUSED);
	return ctx->control;
}

ir_ref _ir_LOOP_END(ir_ctx *ctx)
{
	ir_ref ref;

	IR_ASSERT(ctx->control);
	ref = ir_emit1(ctx, IR_LOOP_END, ctx->control);
	ctx->control = IR_UNUSED;
	return ref;
}

ir_ref _ir_CALL(ir_ctx *ctx, ir_type type, ir_ref func)
{
	IR_ASSERT(ctx->control);
	return ctx->control = ir_emit2(ctx, IR_OPTX(IR_CALL, type, 2), ctx->control, func);
}

ir_ref _ir_CALL_1(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1)
{
	IR_ASSERT(ctx->control);
	return ctx->control = ir_emit3(ctx, IR_OPTX(IR_CALL, type, 3), ctx->control, func, arg1);
}

ir_ref _ir_CALL_2(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2)
{
	ir_ref call;

	IR_ASSERT(ctx->control);
	call = ir_emit_N(ctx, IR_OPT(IR_CALL, type), 4);
	ir_set_op(ctx, call, 1, ctx->control);
	ir_set_op(ctx, call, 2, func);
	ir_set_op(ctx, call, 3, arg1);
	ir_set_op(ctx, call, 4, arg2);
	ctx->control = call;
	return call;
}

ir_ref _ir_CALL_3(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2, ir_ref arg3)
{
	ir_ref call;

	IR_ASSERT(ctx->control);
	call = ir_emit_N(ctx, IR_OPT(IR_CALL, type), 5);
	ir_set_op(ctx, call, 1, ctx->control);
	ir_set_op(ctx, call, 2, func);
	ir_set_op(ctx, call, 3, arg1);
	ir_set_op(ctx, call, 4, arg2);
	ir_set_op(ctx, call, 5, arg3);
	ctx->control = call;
	return call;
}

ir_ref _ir_CALL_4(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2, ir_ref arg3, ir_ref arg4)
{
	ir_ref call;

	IR_ASSERT(ctx->control);
	call = ir_emit_N(ctx, IR_OPT(IR_CALL, type), 6);
	ir_set_op(ctx, call, 1, ctx->control);
	ir_set_op(ctx, call, 2, func);
	ir_set_op(ctx, call, 3, arg1);
	ir_set_op(ctx, call, 4, arg2);
	ir_set_op(ctx, call, 5, arg3);
	ir_set_op(ctx, call, 6, arg4);
	ctx->control = call;
	return call;
}

ir_ref _ir_CALL_5(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2, ir_ref arg3, ir_ref arg4, ir_ref arg5)
{
	ir_ref call;

	IR_ASSERT(ctx->control);
	call = ir_emit_N(ctx, IR_OPT(IR_CALL, type), 7);
	ir_set_op(ctx, call, 1, ctx->control);
	ir_set_op(ctx, call, 2, func);
	ir_set_op(ctx, call, 3, arg1);
	ir_set_op(ctx, call, 4, arg2);
	ir_set_op(ctx, call, 5, arg3);
	ir_set_op(ctx, call, 6, arg4);
	ir_set_op(ctx, call, 7, arg5);
	ctx->control = call;
	return call;
}

ir_ref _ir_CALL_N(ir_ctx *ctx, ir_type type, ir_ref func, uint32_t count, ir_ref *args)
{
	ir_ref call;
	uint32_t i;

	IR_ASSERT(ctx->control);
	call = ir_emit_N(ctx, IR_OPT(IR_CALL, type), count + 2);
	ir_set_op(ctx, call, 1, ctx->control);
	ir_set_op(ctx, call, 2, func);
	for (i = 0; i < count; i++) {
		ir_set_op(ctx, call, i + 3, args[i]);
	}
	ctx->control = call;
	return call;
}

void _ir_UNREACHABLE(ir_ctx *ctx)
{
	IR_ASSERT(ctx->control);
	ctx->control = ir_emit3(ctx, IR_UNREACHABLE, ctx->control, IR_UNUSED, ctx->ir_base[1].op1);
	ctx->ir_base[1].op1 = ctx->control;
	ctx->control = IR_UNUSED;
}

void _ir_TAILCALL(ir_ctx *ctx, ir_type type, ir_ref func)
{
	IR_ASSERT(ctx->control);
	if (ctx->ret_type == (ir_type)-1) {
		ctx->ret_type = type;
	}
	IR_ASSERT(ctx->ret_type == type && "conflicting return type");
	ctx->control = ir_emit2(ctx, IR_OPTX(IR_TAILCALL, type, 2), ctx->control, func);
	_ir_UNREACHABLE(ctx);
}

void _ir_TAILCALL_1(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1)
{
	IR_ASSERT(ctx->control);
	if (ctx->ret_type == (ir_type)-1) {
		ctx->ret_type = type;
	}
	IR_ASSERT(ctx->ret_type == type && "conflicting return type");
	ctx->control = ir_emit3(ctx, IR_OPTX(IR_TAILCALL, type, 3), ctx->control, func, arg1);
	_ir_UNREACHABLE(ctx);
}

void _ir_TAILCALL_2(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2)
{
	ir_ref call;

	IR_ASSERT(ctx->control);
	if (ctx->ret_type == (ir_type)-1) {
		ctx->ret_type = type;
	}
	IR_ASSERT(ctx->ret_type == type && "conflicting return type");
	call = ir_emit_N(ctx, IR_OPT(IR_TAILCALL, type), 4);
	ir_set_op(ctx, call, 1, ctx->control);
	ir_set_op(ctx, call, 2, func);
	ir_set_op(ctx, call, 3, arg1);
	ir_set_op(ctx, call, 4, arg2);
	ctx->control = call;
	_ir_UNREACHABLE(ctx);
}

void _ir_TAILCALL_3(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2, ir_ref arg3)
{
	ir_ref call;

	IR_ASSERT(ctx->control);
	if (ctx->ret_type == (ir_type)-1) {
		ctx->ret_type = type;
	}
	IR_ASSERT(ctx->ret_type == type && "conflicting return type");
	call = ir_emit_N(ctx, IR_OPT(IR_TAILCALL, type), 5);
	ir_set_op(ctx, call, 1, ctx->control);
	ir_set_op(ctx, call, 2, func);
	ir_set_op(ctx, call, 3, arg1);
	ir_set_op(ctx, call, 4, arg2);
	ir_set_op(ctx, call, 5, arg3);
	ctx->control = call;
	_ir_UNREACHABLE(ctx);
}

void _ir_TAILCALL_4(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2, ir_ref arg3, ir_ref arg4)
{
	ir_ref call;

	IR_ASSERT(ctx->control);
	if (ctx->ret_type == (ir_type)-1) {
		ctx->ret_type = type;
	}
	IR_ASSERT(ctx->ret_type == type && "conflicting return type");
	call = ir_emit_N(ctx, IR_OPT(IR_TAILCALL, type), 6);
	ir_set_op(ctx, call, 1, ctx->control);
	ir_set_op(ctx, call, 2, func);
	ir_set_op(ctx, call, 3, arg1);
	ir_set_op(ctx, call, 4, arg2);
	ir_set_op(ctx, call, 5, arg3);
	ir_set_op(ctx, call, 6, arg4);
	ctx->control = call;
	_ir_UNREACHABLE(ctx);
}

void _ir_TAILCALL_5(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2, ir_ref arg3, ir_ref arg4, ir_ref arg5)
{
	ir_ref call;

	IR_ASSERT(ctx->control);
	if (ctx->ret_type == (ir_type)-1) {
		ctx->ret_type = type;
	}
	IR_ASSERT(ctx->ret_type == type && "conflicting return type");
	call = ir_emit_N(ctx, IR_OPT(IR_TAILCALL, type), 7);
	ir_set_op(ctx, call, 1, ctx->control);
	ir_set_op(ctx, call, 2, func);
	ir_set_op(ctx, call, 3, arg1);
	ir_set_op(ctx, call, 4, arg2);
	ir_set_op(ctx, call, 5, arg3);
	ir_set_op(ctx, call, 6, arg4);
	ir_set_op(ctx, call, 7, arg5);
	ctx->control = call;
	_ir_UNREACHABLE(ctx);
}

void _ir_TAILCALL_N(ir_ctx *ctx, ir_type type, ir_ref func, uint32_t count, ir_ref *args)
{
	ir_ref call;
	uint32_t i;

	IR_ASSERT(ctx->control);
	if (ctx->ret_type == (ir_type)-1) {
		ctx->ret_type = type;
	}
	IR_ASSERT(ctx->ret_type == type && "conflicting return type");
	call = ir_emit_N(ctx, IR_OPT(IR_TAILCALL, type), count + 2);
	ir_set_op(ctx, call, 1, ctx->control);
	ir_set_op(ctx, call, 2, func);
	for (i = 0; i < count; i++) {
		ir_set_op(ctx, call, i + 3, args[i]);
	}
	ctx->control = call;
	_ir_UNREACHABLE(ctx);
}

ir_ref _ir_SWITCH(ir_ctx *ctx, ir_ref val)
{
	ir_ref ref;

	IR_ASSERT(ctx->control);
	ref = ir_emit2(ctx, IR_SWITCH, ctx->control, val);
	ctx->control = IR_UNUSED;
	return ref;
}

void _ir_CASE_VAL(ir_ctx *ctx, ir_ref switch_ref, ir_ref val)
{
	IR_ASSERT(!ctx->control);
	ctx->control = ir_emit2(ctx, IR_CASE_VAL, switch_ref, val);
}

void _ir_CASE_DEFAULT(ir_ctx *ctx, ir_ref switch_ref)
{
	IR_ASSERT(!ctx->control);
	ctx->control = ir_emit1(ctx, IR_CASE_DEFAULT, switch_ref);
}

void _ir_RETURN(ir_ctx *ctx, ir_ref val)
{
	ir_type type = (val != IR_UNUSED) ? ctx->ir_base[val].type : IR_VOID;

	IR_ASSERT(ctx->control);
	if (ctx->ret_type == (ir_type)-1) {
		ctx->ret_type = type;
	}
	IR_ASSERT(ctx->ret_type == type && "conflicting return type");
	ctx->control = ir_emit3(ctx, IR_RETURN, ctx->control, val, ctx->ir_base[1].op1);
	ctx->ir_base[1].op1 = ctx->control;
	ctx->control = IR_UNUSED;
}

void _ir_IJMP(ir_ctx *ctx, ir_ref addr)
{
	IR_ASSERT(ctx->control);
	ctx->control = ir_emit3(ctx, IR_IJMP, ctx->control, addr, ctx->ir_base[1].op1);
	ctx->ir_base[1].op1 = ctx->control;
	ctx->control = IR_UNUSED;
}

ir_ref _ir_ADD_OFFSET(ir_ctx *ctx, ir_ref addr, uintptr_t offset)
{
	if (offset) {
		addr = ir_fold2(ctx, IR_OPT(IR_ADD, IR_ADDR), addr, ir_const_addr(ctx, offset));
	}
	return addr;
}

void _ir_GUARD(ir_ctx *ctx, ir_ref condition, ir_ref addr)
{
	IR_ASSERT(ctx->control);
	if (condition == IR_TRUE) {
		return;
	} else {
		ir_ref ref = ctx->control;
		ir_insn *insn;

		while (ref > condition) {
			insn = &ctx->ir_base[ref];
			if (insn->op == IR_GUARD) {
				if (insn->op2 == condition) {
					return;
				}
			} else if (insn->op == IR_GUARD_NOT) {
				if (insn->op2 == condition) {
					condition = IR_FALSE;
					break;
				}
			} else if (insn->op == IR_START || insn->op == IR_MERGE || insn->op == IR_LOOP_BEGIN) {
				break;
			}
			ref = insn->op1;
		}
	}
	if (ctx->snapshot_create) {
		ctx->snapshot_create(ctx, addr);
	}
	ctx->control = ir_emit3(ctx, IR_GUARD, ctx->control, condition, addr);
}

void _ir_GUARD_NOT(ir_ctx *ctx, ir_ref condition, ir_ref addr)
{
	IR_ASSERT(ctx->control);
	if (condition == IR_FALSE) {
		return;
	} else {
		ir_ref ref = ctx->control;
		ir_insn *insn;

		while (ref > condition) {
			insn = &ctx->ir_base[ref];
			if (insn->op == IR_GUARD_NOT) {
				if (insn->op2 == condition) {
					return;
				}
			} else if (insn->op == IR_GUARD) {
				if (insn->op2 == condition) {
					condition = IR_TRUE;
					break;
				}
			} else if (insn->op == IR_START || insn->op == IR_MERGE || insn->op == IR_LOOP_BEGIN) {
				break;
			}
			ref = insn->op1;
		}
	}
	if (ctx->snapshot_create) {
		ctx->snapshot_create(ctx, addr);
	}
	ctx->control = ir_emit3(ctx, IR_GUARD_NOT, ctx->control, condition, addr);
}

ir_ref _ir_SNAPSHOT(ir_ctx *ctx, ir_ref n)
{
	ir_ref snapshot;

	IR_ASSERT(ctx->control);
	snapshot = ir_emit_N(ctx, IR_SNAPSHOT, 1 + n); /* op1 is used for control */
	ctx->ir_base[snapshot].op1 = ctx->control;
	ctx->control = snapshot;
	return snapshot;
}

void _ir_SNAPSHOT_SET_OP(ir_ctx *ctx, ir_ref snapshot, ir_ref pos, ir_ref val)
{
	ir_insn *insn = &ctx->ir_base[snapshot];
	ir_ref *ops = insn->ops;

	IR_ASSERT(val < snapshot);
	IR_ASSERT(insn->op == IR_SNAPSHOT);
	pos++; /* op1 is used for control */
	IR_ASSERT(pos > 1 && pos <= insn->inputs_count);
	ops[pos] = val;
}

ir_ref _ir_EXITCALL(ir_ctx *ctx, ir_ref func)
{
	IR_ASSERT(ctx->control);
	return ctx->control = ir_emit2(ctx, IR_OPT(IR_EXITCALL, IR_I32), ctx->control, func);
}

ir_ref _ir_ALLOCA(ir_ctx *ctx, ir_ref size)
{
	IR_ASSERT(ctx->control);
	return ctx->control = ir_emit2(ctx, IR_OPT(IR_ALLOCA, IR_ADDR), ctx->control, size);
}

void _ir_AFREE(ir_ctx *ctx, ir_ref size)
{
	IR_ASSERT(ctx->control);
	ctx->control = ir_emit2(ctx, IR_AFREE, ctx->control, size);
}

ir_ref _ir_VLOAD(ir_ctx *ctx, ir_type type, ir_ref var)
{
	IR_ASSERT(ctx->control);
	return ctx->control = ir_emit2(ctx, IR_OPT(IR_VLOAD, type), ctx->control, var);
}

void _ir_VSTORE(ir_ctx *ctx, ir_ref var, ir_ref val)
{
	IR_ASSERT(ctx->control);
	ctx->control = ir_emit3(ctx, IR_VSTORE, ctx->control, var, val);
}

ir_ref _ir_TLS(ir_ctx *ctx, ir_ref index, ir_ref offset)
{
	IR_ASSERT(ctx->control);
	return ctx->control = ir_emit3(ctx, IR_OPT(IR_TLS, IR_ADDR), ctx->control, index, offset);
}

ir_ref _ir_RLOAD(ir_ctx *ctx, ir_type type, ir_ref reg)
{
	IR_ASSERT(ctx->control);
	return ctx->control = ir_emit2(ctx, IR_OPT(IR_RLOAD, type), ctx->control, reg);
}

void _ir_RSTORE(ir_ctx *ctx, ir_ref reg, ir_ref val)
{
	IR_ASSERT(ctx->control);
	ctx->control = ir_emit3(ctx, IR_RSTORE, ctx->control, val, reg);
}

ir_ref _ir_LOAD(ir_ctx *ctx, ir_type type, ir_ref addr)
{
	ir_ref ref = ir_find_aliasing_load(ctx, ctx->control, type, addr);

	IR_ASSERT(ctx->control);
	if (!ref) {
		ctx->control = ref = ir_emit2(ctx, IR_OPT(IR_LOAD, type), ctx->control, addr);
	}
	return ref;
}

void _ir_STORE(ir_ctx *ctx, ir_ref addr, ir_ref val)
{
	ir_ref limit = (addr > 0) ? addr : 1;
	ir_ref ref = ctx->control;
	ir_ref prev = IR_UNUSED;
	ir_insn *insn;
	ir_type type = ctx->ir_base[val].type;
	ir_type type2;
	bool guarded = 0;

	IR_ASSERT(ctx->control);
	while (ref > limit) {
		insn = &ctx->ir_base[ref];
		if (insn->op == IR_STORE) {
			if (insn->op2 == addr) {
				if (ctx->ir_base[insn->op3].type == type) {
					if (insn->op3 == val) {
						return;
					} else {
						if (!guarded) {
							if (prev) {
								ctx->ir_base[prev].op1 = insn->op1;
							} else {
								ctx->control = insn->op1;
							}
							insn->optx = IR_NOP;
							insn->op1 = IR_NOP;
							insn->op2 = IR_NOP;
							insn->op3 = IR_NOP;
						}
						break;
					}
				} else {
					break;
				}
			} else {
				type2 = ctx->ir_base[insn->op3].type;
				goto check_aliasing;
			}
		} else if (insn->op == IR_LOAD) {
			if (insn->op2 == addr) {
				break;
			}
			type2 = insn->type;
check_aliasing:
			if (ir_check_partial_aliasing(ctx, addr, insn->op2, type, type2) != IR_NO_ALIAS) {
				break;
			}
		} else if (insn->op == IR_GUARD || insn->op == IR_GUARD_NOT) {
			guarded = 1;
		} else if (insn->op >= IR_START || insn->op == IR_CALL) {
			break;
		}
		prev = ref;
		ref = insn->op1;
	}
	ctx->control = ir_emit3(ctx, IR_STORE, ctx->control, addr, val);
}

void _ir_VA_START(ir_ctx *ctx, ir_ref list)
{
	IR_ASSERT(ctx->control);
	ctx->control = ir_emit2(ctx, IR_VA_START, ctx->control, list);
}

void _ir_VA_END(ir_ctx *ctx, ir_ref list)
{
	IR_ASSERT(ctx->control);
	ctx->control = ir_emit2(ctx, IR_VA_END, ctx->control, list);
}

void _ir_VA_COPY(ir_ctx *ctx, ir_ref dst, ir_ref src)
{
	IR_ASSERT(ctx->control);
	ctx->control = ir_emit3(ctx, IR_VA_COPY, ctx->control, dst, src);
}

ir_ref _ir_VA_ARG(ir_ctx *ctx, ir_type type, ir_ref list)
{
	IR_ASSERT(ctx->control);
	return ctx->control = ir_emit2(ctx, IR_OPT(IR_VA_ARG, type), ctx->control, list);
}
