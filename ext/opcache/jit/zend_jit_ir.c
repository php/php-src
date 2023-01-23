/*
 *  +----------------------------------------------------------------------+
 *  | Zend JIT                                                             |
 *  +----------------------------------------------------------------------+
 *  | Copyright (c) The PHP Group                                          |
 *  +----------------------------------------------------------------------+
 *  | This source file is subject to version 3.01 of the PHP license,      |
 *  | that is bundled with this package in the file LICENSE, and is        |
 *  | available through the world-wide-web at the following url:           |
 *  | https://www.php.net/license/3_01.txt                                 |
 *  | If you did not receive a copy of the PHP license and are unable to   |
 *  | obtain it through the world-wide-web, please send a note to          |
 *  | license@php.net so we can mail you a copy immediately.               |
 *  +----------------------------------------------------------------------+
 *  | Authors: Dmitry Stogov <dmitry@php.net>                              |
 *  +----------------------------------------------------------------------+
 */

#include "jit/ir/ir.h"

#if defined(IR_TARGET_X86)
# define IR_REG_SP       4 /* IR_REG_RSP */
# define ZREG_FP         6 /* IR_REG_RSI */
# define ZREG_IP         7 /* IR_REG_RDI */
# define ZREG_FIRST_FPR  8
#elif defined(IR_TARGET_X64)
# define IR_REG_SP       4 /* IR_REG_RSP */
# define ZREG_FP        14 /* IR_REG_R14 */
# define ZREG_IP        15 /* IR_REG_R15 */
# define ZREG_FIRST_FPR 16
#elif defined(IR_TARGET_AARCH64)
# define IR_REG_SP      31 /* IR_REG_RSP */
# define ZREG_FP        27 /* IR_REG_X27 */
# define ZREG_IP        28 /* IR_REG_X28 */
# define ZREG_FIRST_FPR 32
#else
# error "Unknown IR target"
#endif

#define ZREG_RX ZREG_IP

#ifdef ZEND_ENABLE_ZVAL_LONG64
# define IR_PHP_LONG       IR_I64
# define ir_const_php_long ir_const_i64
#else
# define IR_PHP_LONG       IR_I32
# define ir_const_php_long ir_const_i32
#endif

#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
# define IR_REGSET_PHP_FIXED \
	((1<<ZREG_FP) | (1<<ZREG_IP) | (1<<5)) /* prevent %rbp (%r5) usage */
#else
# define IR_REGSET_PHP_FIXED \
	((1<<ZREG_FP) | (1<<ZREG_IP))
#endif

static size_t zend_jit_trace_prologue_size = (size_t)-1;
static uint32_t allowed_opt_flags = 0;
static bool delayed_call_chain = 0; // TODO: remove this var (use jit->delayed_call_level) ???

#ifdef ZTS
static size_t tsrm_ls_cache_tcb_offset = 0;
static size_t tsrm_tls_index = 0;
static size_t tsrm_tls_offset = 0;

# define EG_TLS_OFFSET(field) \
	(executor_globals_offset + offsetof(zend_executor_globals, field))

# define CG_TLS_OFFSET(field) \
	(compiler_globals_offset + offsetof(zend_compiler_globals, field))

# define ZEND_JIT_EG_ADDR(_field) \
	zend_jit_addr_offset(jit, zend_jit_tls(jit), EG_TLS_OFFSET(_field))

# define ZEND_JIT_CG_ADDR(_field) \
	zend_jit_addr_offset(jit, zend_jit_tls(jit), CG_TLS_OFFSET(_field))

#else

# define ZEND_JIT_EG_ADDR(_field) \
	zend_jit_const_addr(jit, (uintptr_t)&EG(_field))

# define ZEND_JIT_CG_ADDR(_field) \
	zend_jit_const_addr(jit, (uintptr_t)&CG(_field))

#endif

#define JIT_STUBS(_) \
	_(exception_handler,              SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(exception_handler_undef,        SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(exception_handler_free_op2,     SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(exception_handler_free_op1_op2, SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(interrupt_handler,              SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(leave_function_handler,         SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(negative_shift,                 SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(mod_by_zero,                    SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(invalid_this,                   SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(undefined_function,             SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(throw_cannot_pass_by_ref,       SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(icall_throw,                    SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(leave_throw,                    SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(hybrid_runtime_jit,             SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE | IR_START_BR_TARGET) \
	_(hybrid_profile_jit,             SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE | IR_START_BR_TARGET) \
	_(hybrid_func_hot_counter,        SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE | IR_START_BR_TARGET) \
	_(hybrid_loop_hot_counter,        SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE | IR_START_BR_TARGET) \
	_(hybrid_func_trace_counter,      SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE | IR_START_BR_TARGET) \
	_(hybrid_ret_trace_counter,       SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE | IR_START_BR_TARGET) \
	_(hybrid_loop_trace_counter,      SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE | IR_START_BR_TARGET) \
	_(trace_halt,                     SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(trace_escape,                   SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(trace_exit,                     SP_ADJ_JIT,  SP_ADJ_VM, IR_SKIP_PROLOGUE) \
	_(assign_const,                   SP_ADJ_RET,  SP_ADJ_ASSIGN, IR_FUNCTION | IR_FASTCALL_FUNC) \
	_(assign_tmp,                     SP_ADJ_RET,  SP_ADJ_ASSIGN, IR_FUNCTION | IR_FASTCALL_FUNC) \
	_(assign_var,                     SP_ADJ_RET,  SP_ADJ_ASSIGN, IR_FUNCTION | IR_FASTCALL_FUNC) \
	_(assign_cv_noref,                SP_ADJ_RET,  SP_ADJ_ASSIGN, IR_FUNCTION | IR_FASTCALL_FUNC) \
	_(assign_cv,                      SP_ADJ_RET,  SP_ADJ_ASSIGN, IR_FUNCTION | IR_FASTCALL_FUNC) \

#define JIT_STUB_ID(name, offset, adjustment, flags) \
	jit_stub_ ## name,

#define JIT_STUB_FORWARD(name, offset, adjustment, flags) \
	static int zend_jit_ ## name ## _stub(zend_jit_ctx *jit);

#define JIT_STUB(name, offset, adjustment, flags) \
	{JIT_STUB_PREFIX #name, zend_jit_ ## name ## _stub, flags, offset, adjustment},

typedef enum _jit_stub_id {
	JIT_STUBS(JIT_STUB_ID)
	jit_last_stub
} jit_stub_id;

typedef struct _zend_jit_reg_var {
	ir_ref   ref;
	uint32_t flags;
} zend_jit_reg_var;

typedef struct _zend_jit_ctx {
	ir_ctx               ctx;
	const zend_op       *last_valid_opline;
	bool                 use_last_valid_opline;
	bool                 track_last_valid_opline;
	bool                 reuse_ip;
	uint32_t             delayed_call_level;
	int                  b;           /* current basic block number or -1 */
	ir_ref               control;
#ifdef ZTS
	ir_ref               tls;
#endif
	ir_ref               fp;
	ir_ref               trace_loop_ref;
	ir_ref               return_inputs;
	const zend_op_array *op_array;
	zend_ssa            *ssa;
	zend_string         *name;
	ir_ref              *bb_start_ref;      /* PHP BB -> IR ref mapping */
	ir_ref              *bb_predecessors;   /* PHP BB -> index in bb_edges -> IR refs of predessors */
	ir_ref              *bb_edges;
	zend_jit_trace_info *trace;
	zend_jit_reg_var    *ra;
	int                  delay_var;
	ir_ref              *delay_buf;
	int                  delay_count;
	ir_ref               eg_exception_addr;
	HashTable            addr_hash;
	ir_ref               stub_addr[jit_last_stub];
} zend_jit_ctx;

typedef int8_t zend_reg;

typedef struct _zend_jit_registers_buf {
#if defined(IR_TARGET_X64)
	uint64_t gpr[16]; /* general purpose integer register */
	double   fpr[16]; /* floating point registers */
#elif defined(IR_TARGET_X86)
	uint32_t gpr[8]; /* general purpose integer register */
	double   fpr[8]; /* floating point registers */
#elif defined (IR_TARGET_AARCH64)
	uint64_t gpr[32]; /* general purpose integer register */
	double   fpr[32]; /* floating point registers */
#else
# error "Unknown IR target"
#endif
} zend_jit_registers_buf;

/* Keep 32 exit points in a single code block */
#define ZEND_JIT_EXIT_POINTS_SPACING   4  // push byte + short jmp = bytes
#define ZEND_JIT_EXIT_POINTS_PER_GROUP 32 // number of continuous exit points

static ir_ref zend_jit_addr_offset(zend_jit_ctx *jit, ir_ref addr, uintptr_t offset); // for ZTS build (fix this ???)

static uint32_t zend_jit_exit_point_by_addr(void *addr);
int ZEND_FASTCALL zend_jit_trace_exit(uint32_t exit_num, zend_jit_registers_buf *regs);

static int zend_jit_assign_to_variable(zend_jit_ctx   *jit,
                                       const zend_op  *opline,
                                       zend_jit_addr   var_use_addr,
                                       zend_jit_addr   var_addr,
                                       uint32_t        var_info,
                                       uint32_t        var_def_info,
                                       zend_uchar      val_type,
                                       zend_jit_addr   val_addr,
                                       uint32_t        val_info,
                                       zend_jit_addr   res_addr,
                                       zend_jit_addr   ref_addr,
                                       bool       check_exception);

typedef struct _zend_jit_stub {
	const char *name;
	int (*stub)(zend_jit_ctx *jit);
	uint32_t flags;
	uint32_t offset;
	uint32_t adjustment;
} zend_jit_stub;

JIT_STUBS(JIT_STUB_FORWARD)

static const zend_jit_stub zend_jit_stubs[] = {
	JIT_STUBS(JIT_STUB)
};

static void* zend_jit_stub_handlers[sizeof(zend_jit_stubs) / sizeof(zend_jit_stubs[0])];

#ifdef ZTS
static ir_ref zend_jit_tls(zend_jit_ctx *jit)
{
	ZEND_ASSERT(jit->control);
	if (jit->tls) {
		/* Emit "TLS" once for basic block ??? */
		ir_insn *insn;
		ir_ref ref = jit->control;

		while (1) {
			if (ref == jit->tls) {
				return jit->tls;
			}
			insn = &jit->ctx.ir_base[ref];
			if (insn->op == IR_START
			 || insn->op == IR_BEGIN
			 || insn->op == IR_IF_TRUE
			 || insn->op == IR_IF_FALSE
			 || insn->op == IR_CASE_VAL
			 || insn->op == IR_CASE_DEFAULT
			 || insn->op == IR_MERGE
			 || insn->op == IR_LOOP_BEGIN
			 || insn->op == IR_ENTRY
			 || insn->op == IR_CALL) {
				break;
			}
			ref = insn->op1;
		}
	}
	jit->control = jit->tls = ir_emit3(&jit->ctx, IR_OPT(IR_TLS, IR_ADDR), jit->control,
		tsrm_ls_cache_tcb_offset ? tsrm_ls_cache_tcb_offset : tsrm_tls_index,
		tsrm_ls_cache_tcb_offset ? 0 : tsrm_tls_offset);
	return jit->tls;
}
#endif

static ir_ref zend_jit_const_addr(zend_jit_ctx *jit, uintptr_t addr)
{
	ir_ref ref;
	zval *zv;

	if (addr == 0) {
		return IR_NULL;
	}
	zv = zend_hash_index_lookup(&jit->addr_hash, addr);
	if (Z_TYPE_P(zv) == IS_LONG) {
		ref = Z_LVAL_P(zv);
		ZEND_ASSERT(jit->ctx.ir_base[ref].opt == IR_OPT(IR_ADDR, IR_ADDR));
	} else {
		ref = ir_unique_const_addr(&jit->ctx, addr);
		ZVAL_LONG(zv, ref);
	}
	return ref;
}

static ir_ref zend_jit_const_func_addr(zend_jit_ctx *jit, uintptr_t addr, uint16_t flags)
{
	ir_ref ref;
	ir_insn *insn;
	zval *zv;

	ZEND_ASSERT(addr != 0);
	zv = zend_hash_index_lookup(&jit->addr_hash, addr);
	if (Z_TYPE_P(zv) == IS_LONG) {
		ref = Z_LVAL_P(zv);
		ZEND_ASSERT(jit->ctx.ir_base[ref].opt == IR_OPT(IR_FUNC_ADDR, IR_ADDR) && jit->ctx.ir_base[ref].const_flags == flags);
	} else {
		ref = ir_unique_const_addr(&jit->ctx, addr);
		insn = &jit->ctx.ir_base[ref];
		insn->optx = IR_OPT(IR_FUNC_ADDR, IR_ADDR);
		insn->const_flags = flags;
		ZVAL_LONG(zv, ref);
	}
	return ref;
}

static ir_ref zend_jit_eg_exception_addr(zend_jit_ctx *jit)
{
#ifdef ZTS
	return ZEND_JIT_EG_ADDR(exception);
#else
	ir_ref ref = jit->eg_exception_addr;

	if (UNEXPECTED(!ref)) {
		ref = ir_unique_const_addr(&jit->ctx, (uintptr_t)&EG(exception));
		jit->eg_exception_addr = ref;
	}
	return ref;
#endif
}

static ir_ref zend_jit_stub_addr(zend_jit_ctx *jit, jit_stub_id id)
{
	ir_ref ref = jit->stub_addr[id];

	if (UNEXPECTED(!ref)) {
		ref = ir_unique_const_addr(&jit->ctx, (uintptr_t)zend_jit_stub_handlers[id]);
		jit->stub_addr[id] = ref;
	}
	return ref;
}

static ir_ref zend_jit_stub_func_addr(zend_jit_ctx *jit, jit_stub_id id, uint16_t flags)
{
	ir_ref ref = jit->stub_addr[id];
	ir_insn *insn;

	if (UNEXPECTED(!ref)) {
		ref = ir_unique_const_addr(&jit->ctx, (uintptr_t)zend_jit_stub_handlers[id]);
		insn = &jit->ctx.ir_base[ref];
		insn->optx = IR_OPT(IR_FUNC_ADDR, IR_ADDR);
		insn->const_flags = flags;
		jit->stub_addr[id] = ref;
	}
	return ref;
}

/* Alias Analyses */
// TODO: move to IR ???
typedef enum _ir_alias {
	IR_MAY_ALIAS  = -1,
	IR_NO_ALIAS   =  0,
	IR_MUST_ALIAS =  1,
} ir_alias;

extern const uint8_t ir_type_size[IR_LAST_TYPE];

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

static ir_alias ir_check_partial_aliasing(ir_ctx *ctx, ir_ref addr1, ir_ref addr2, ir_type type1, ir_type type2)
{
	ir_insn *insn1, *insn2;

	/* this must be already check */
	ZEND_ASSERT(addr1 != addr2);

	insn1 = &ctx->ir_base[addr1];
	insn2 = &ctx->ir_base[addr2];
	if (insn1->op == IR_ADD && IR_IS_CONST_REF(insn1->op2)) {
		if (insn1->op1 == addr2) {
			uintptr_t offset1 = ctx->ir_base[insn1->op2].val.u64;
			uintptr_t size2 = ir_type_size[type2];

			return (offset1 < size2) ? IR_MUST_ALIAS : IR_NO_ALIAS;
		} else if (insn2->op == IR_ADD && IR_IS_CONST_REF(insn1->op2) && insn1->op1 == insn2->op1) {
			if (insn1->op2 == insn2->op2) {
				return IR_MUST_ALIAS;
			} else if (IR_IS_CONST_REF(insn1->op2) && IR_IS_CONST_REF(insn2->op2)) {
				uintptr_t offset1 = ctx->ir_base[insn1->op2].val.u64;
				uintptr_t offset2 = ctx->ir_base[insn2->op2].val.u64;

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
			uintptr_t offset2 = ctx->ir_base[insn2->op2].val.u64;
			uintptr_t size1 = ir_type_size[type1];

			return (offset2 < size1) ? IR_MUST_ALIAS : IR_NO_ALIAS;
		}
	}
	return IR_MAY_ALIAS;
}

static ir_ref ir_find_aliasing_load(ir_ctx *ctx, ir_ref ref, ir_type type, ir_ref addr)
{
	ir_insn *insn;
	uint32_t modified_regset = 0;

	while (1) {
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
		} else if (insn->op == IR_START
			 || insn->op == IR_BEGIN
			 || insn->op == IR_IF_TRUE
			 || insn->op == IR_IF_FALSE
			 || insn->op == IR_CASE_VAL
			 || insn->op == IR_CASE_DEFAULT
			 || insn->op == IR_MERGE
			 || insn->op == IR_LOOP_BEGIN
			 || insn->op == IR_ENTRY
			 || insn->op == IR_CALL) {
			return IR_UNUSED;
		}
		ref = insn->op1;
	}
}

/* IR helpers */
static int zend_jit_param(zend_jit_ctx *jit, ir_type type, const char* name, ir_ref num)
{
	ZEND_ASSERT(jit->control);
	ZEND_ASSERT(jit->ctx.ir_base[jit->control].op == IR_START);
	ZEND_ASSERT(jit->ctx.insns_count == num + 1);
	return ir_param(&jit->ctx, type, jit->control, name, num);
}

static int zend_jit_entry(zend_jit_ctx *jit, unsigned int label)
{
	jit->ctx.ir_base[1].op2 = jit->control = ir_emit2(&jit->ctx, IR_ENTRY, label, jit->ctx.ir_base[1].op2);
	return 1;
}

static ir_ref zend_jit_rload(zend_jit_ctx *jit, ir_type type, ir_ref reg)
{
	ZEND_ASSERT(jit->control);
	return jit->control = ir_emit2(&jit->ctx, IR_OPT(IR_RLOAD, type), jit->control, reg);
}

static void zend_jit_rstore(zend_jit_ctx *jit, ir_ref val, ir_ref reg)
{
	ZEND_ASSERT(jit->control);
	jit->control = ir_emit3(&jit->ctx, IR_RSTORE, jit->control, val, reg);
}

static ir_ref zend_jit_load(zend_jit_ctx *jit, ir_type type, ir_ref addr)
{
	ir_ref ref = ir_find_aliasing_load(&jit->ctx, jit->control, type, addr);

	ZEND_ASSERT(jit->control);
	if (!ref) {
		jit->control = ref = ir_emit2(&jit->ctx, IR_OPT(IR_LOAD, type), jit->control, addr);
	}
	return ref;
}

static void zend_jit_store(zend_jit_ctx *jit, ir_ref addr, ir_ref val)
{
	ir_ref ref = jit->control;
	ir_ref prev = IR_UNUSED;
	ir_insn *insn;
	ir_type type = jit->ctx.ir_base[val].type;
	ir_type type2;

	ZEND_ASSERT(jit->control);
	while (1) {
		insn = &jit->ctx.ir_base[ref];
		if (insn->op == IR_STORE) {
			if (insn->op2 == addr) {
				if (jit->ctx.ir_base[insn->op3].type == type) {
					if (insn->op3 == val) {
						return;
					} else {
						if (prev) {
							jit->ctx.ir_base[prev].op1 = insn->op1;
						} else {
							jit->control = insn->op1;
						}
						insn->optx = IR_NOP;
						insn->op1 = IR_NOP;
						insn->op2 = IR_NOP;
						insn->op3 = IR_NOP;
						break;
					}
				} else {
					break;
				}
			} else {
				type2 = jit->ctx.ir_base[insn->op3].type;
				goto check_aliasing;
			}
		} else if (insn->op == IR_LOAD) {
			if (insn->op2 == addr) {
				break;
			}
			type2 = insn->type;
check_aliasing:
			if (ir_check_partial_aliasing(&jit->ctx, addr, insn->op2, type, type2) != IR_NO_ALIAS) {
				break;
			}
		} else if (insn->op == IR_START
			 || insn->op == IR_BEGIN
			 || insn->op == IR_IF_TRUE
			 || insn->op == IR_IF_FALSE
			 || insn->op == IR_CASE_VAL
			 || insn->op == IR_CASE_DEFAULT
			 || insn->op == IR_MERGE
			 || insn->op == IR_LOOP_BEGIN
			 || insn->op == IR_ENTRY
			 || insn->op == IR_CALL) {
			break;
		}
		prev = ref;
		ref = insn->op1;
	}
	jit->control = ir_emit3(&jit->ctx, IR_STORE, jit->control, addr, val);
}

static ir_ref zend_jit_call_0(zend_jit_ctx *jit, ir_type type, ir_ref func)
{
	jit->control = ir_emit2(&jit->ctx, IR_OPT(IR_CALL, type), jit->control, func);
	ZEND_ASSERT(jit->control);
	return jit->control;
}

static ir_ref zend_jit_call_1(zend_jit_ctx *jit, ir_type type, ir_ref func, ir_ref arg1)
{
	ir_ref call = ir_emit_N(&jit->ctx, IR_OPT(IR_CALL, type), 3);
	ZEND_ASSERT(jit->control);
	ir_set_op(&jit->ctx, call, 1, jit->control);
	ir_set_op(&jit->ctx, call, 2, func);
	ir_set_op(&jit->ctx, call, 3, arg1);
	jit->control = call;
	return call;
}

static ir_ref zend_jit_call_2(zend_jit_ctx *jit, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2)
{
	ir_ref call = ir_emit_N(&jit->ctx, IR_OPT(IR_CALL, type), 4);
	ZEND_ASSERT(jit->control);
	ir_set_op(&jit->ctx, call, 1, jit->control);
	ir_set_op(&jit->ctx, call, 2, func);
	ir_set_op(&jit->ctx, call, 3, arg1);
	ir_set_op(&jit->ctx, call, 4, arg2);
	jit->control = call;
	return call;
}

static ir_ref zend_jit_call_3(zend_jit_ctx *jit, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2, ir_ref arg3)
{
	ir_ref call = ir_emit_N(&jit->ctx, IR_OPT(IR_CALL, type), 5);
	ZEND_ASSERT(jit->control);
	ir_set_op(&jit->ctx, call, 1, jit->control);
	ir_set_op(&jit->ctx, call, 2, func);
	ir_set_op(&jit->ctx, call, 3, arg1);
	ir_set_op(&jit->ctx, call, 4, arg2);
	ir_set_op(&jit->ctx, call, 5, arg3);
	jit->control = call;
	return call;
}

static ir_ref zend_jit_call_4(zend_jit_ctx *jit, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2, ir_ref arg3, ir_ref arg4)
{
	ir_ref call = ir_emit_N(&jit->ctx, IR_OPT(IR_CALL, type), 6);
	ZEND_ASSERT(jit->control);
	ir_set_op(&jit->ctx, call, 1, jit->control);
	ir_set_op(&jit->ctx, call, 2, func);
	ir_set_op(&jit->ctx, call, 3, arg1);
	ir_set_op(&jit->ctx, call, 4, arg2);
	ir_set_op(&jit->ctx, call, 5, arg3);
	ir_set_op(&jit->ctx, call, 6, arg4);
	jit->control = call;
	return call;
}

static ir_ref zend_jit_call_5(zend_jit_ctx *jit, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2, ir_ref arg3, ir_ref arg4, ir_ref arg5)
{
	ir_ref call = ir_emit_N(&jit->ctx, IR_OPT(IR_CALL, type), 7);
	ZEND_ASSERT(jit->control);
	ir_set_op(&jit->ctx, call, 1, jit->control);
	ir_set_op(&jit->ctx, call, 2, func);
	ir_set_op(&jit->ctx, call, 3, arg1);
	ir_set_op(&jit->ctx, call, 4, arg2);
	ir_set_op(&jit->ctx, call, 5, arg3);
	ir_set_op(&jit->ctx, call, 6, arg4);
	ir_set_op(&jit->ctx, call, 7, arg5);
	jit->control = call;
	return call;
}

static void zend_jit_ret(zend_jit_ctx *jit, ir_ref ref)
{
	ZEND_ASSERT(jit->control);
	jit->control = ir_emit3(&jit->ctx, IR_RETURN, jit->control, ref, jit->ctx.ir_base[1].op1);
	jit->ctx.ir_base[1].op1 = jit->control;
}

static void zend_jit_tailcall_0(zend_jit_ctx *jit, ir_ref func)
{
	ZEND_ASSERT(jit->control);
	jit->control = ir_emit2(&jit->ctx, IR_TAILCALL, jit->control, func);
	jit->control = ir_emit3(&jit->ctx, IR_UNREACHABLE, jit->control, IR_UNUSED, jit->ctx.ir_base[1].op1);
	jit->ctx.ir_base[1].op1 = jit->control;
}

static void zend_jit_tailcall_1(zend_jit_ctx *jit, ir_ref func, ir_ref arg1)
{
	ZEND_ASSERT(jit->control);
	jit->control = ir_emit3(&jit->ctx, IR_TAILCALL, jit->control, func, arg1);
	jit->ctx.ir_base[jit->control].inputs_count = 3; //???
	jit->control = ir_emit3(&jit->ctx, IR_UNREACHABLE, jit->control, IR_UNUSED, jit->ctx.ir_base[1].op1);
	jit->ctx.ir_base[1].op1 = jit->control;
}

static void zend_jit_ijmp(zend_jit_ctx *jit, ir_ref addr)
{
	ZEND_ASSERT(jit->control);
	jit->control = ir_emit3(&jit->ctx, IR_IJMP, jit->control, addr, jit->ctx.ir_base[1].op1);
	jit->ctx.ir_base[1].op1 = jit->control;
	jit->control = 0;
}

static ir_ref zend_jit_end(zend_jit_ctx *jit)
{
	ZEND_ASSERT(jit->control);
	ir_ref ref = ir_emit1(&jit->ctx, IR_END, jit->control);
	jit->control = 0;
	return ref;
}

static ir_ref zend_jit_loop_end(zend_jit_ctx *jit, ir_ref loop)
{
	ZEND_ASSERT(jit->control);
	ir_ref ref = ir_emit2(&jit->ctx, IR_LOOP_END, jit->control, loop);
	jit->control = 0;
	return ref;
}

static void zend_jit_begin(zend_jit_ctx *jit, ir_ref in)
{
	if (in && in + 1 == jit->ctx.insns_count && jit->ctx.ir_base[in].op == IR_END) {
		jit->control = jit->ctx.ir_base[in].op1;
		jit->ctx.insns_count--;
	} else {
		jit->control = ir_emit1(&jit->ctx, IR_BEGIN, in);
	}
}

static ir_ref zend_jit_loop_begin(zend_jit_ctx *jit, ir_ref in1)
{
	return jit->control = ir_emit2(&jit->ctx, IR_LOOP_BEGIN, in1, IR_UNUSED);
}

static void zend_jit_merge_2(zend_jit_ctx *jit, ir_ref in1, ir_ref in2)
{
	jit->control = ir_emit2(&jit->ctx, IR_MERGE, in1, in2);
}

static void zend_jit_merge_3(zend_jit_ctx *jit, ir_ref in1, ir_ref in2, ir_ref in3)
{
	jit->control = ir_emit3(&jit->ctx, IR_MERGE, in1, in2, in3);
	jit->ctx.ir_base[jit->control].inputs_count = 3; //???
}

static void zend_jit_merge_N(zend_jit_ctx *jit, uint32_t n, ir_ref *inputs)
{
	ZEND_ASSERT(n > 0);
	if (n == 1) {
		zend_jit_begin(jit, inputs[0]);
	} else {
		jit->control = ir_emit_N(&jit->ctx, IR_MERGE, n);
		while (n) {
			n--;
			ir_set_op(&jit->ctx, jit->control, n + 1, inputs[n]);
		}
	}
}

#define zend_jit_array_add(__array, __ref) do { \
		ZEND_ASSERT(__array ## _count < sizeof(__array)/sizeof(__array[0])); \
		__array[__array ## _count++] = __ref; \
	} while (0)

static ir_ref zend_jit_list_add(zend_jit_ctx *jit, ir_ref list, ir_ref ref)
{
	ir_insn *insn = &jit->ctx.ir_base[ref];

	ZEND_ASSERT(insn->op == IR_END);
	insn->op2 = list;
	return ref;
}

static ir_ref zend_jit_list_add_N(zend_jit_ctx *jit, ir_ref list, uint32_t n, ir_ref *inputs)
{
	uint32_t i;

	ZEND_ASSERT(n > 0);
	for (i = 0; i < n; i++) {
		ir_ref ref = inputs[i];
		ir_insn *insn = &jit->ctx.ir_base[ref];

		ZEND_ASSERT(insn->op == IR_END);
		insn->op2 = list;
		list = ref;
	}
	return list;
}

static void zend_jit_merge_list(zend_jit_ctx *jit, ir_ref list)
{
	ir_ref ref = list;
	uint32_t n = 0;

	/* count inputs count */
	ZEND_ASSERT(list != IR_UNUSED);
	do {
		ir_insn *insn = &jit->ctx.ir_base[ref];

		ZEND_ASSERT(insn->op == IR_END);
		ref = insn->op2;
		n++;
	} while (ref != IR_UNUSED);

	/* create MERGE node */
	ZEND_ASSERT(n > 0);
	if (n == 1) {
		jit->ctx.ir_base[list].op2 = IR_UNUSED;
		zend_jit_begin(jit, list);
	} else {
		jit->control = ir_emit_N(&jit->ctx, IR_MERGE, n);
		ref = list;
		while (n) {
			ir_insn *insn = &jit->ctx.ir_base[ref];

			ir_set_op(&jit->ctx, jit->control, n, ref);
			ref = insn->op2;
			insn->op2 = IR_UNUSED;
			n--;
		}
	}
}

static ir_ref zend_jit_phi_2(zend_jit_ctx *jit, ir_ref in1, ir_ref in2)
{
	ir_type type = jit->ctx.ir_base[in1].type;
	ZEND_ASSERT(jit->control);
	return ir_emit3(&jit->ctx, IR_OPT(IR_PHI, type), jit->control, in1, in2);
}

static ir_ref zend_jit_phi_3(zend_jit_ctx *jit, ir_ref in1, ir_ref in2, ir_ref in3)
{
	ir_type type = jit->ctx.ir_base[in1].type;
	ir_ref phi = ir_emit_N(&jit->ctx, IR_OPT(IR_PHI, type), 4);
	ZEND_ASSERT(jit->control);
	ir_set_op(&jit->ctx, phi, 1, jit->control);
	ir_set_op(&jit->ctx, phi, 2, in1);
	ir_set_op(&jit->ctx, phi, 3, in2);
	ir_set_op(&jit->ctx, phi, 4, in3);
	return phi;
}

static ir_ref zend_jit_phi_N(zend_jit_ctx *jit, uint32_t n, ir_ref *inputs)
{
	ZEND_ASSERT(jit->control);
	ZEND_ASSERT(n > 0);
	if (n == 1) {
		return inputs[0];
	} else {
	    uint32_t i;
		ir_ref ref = inputs[0];

		for (i = 1; i < n; i++) {
			if (inputs[i] != ref) {
				break;
			}
		}
		if (i == n) {
			/* all the same */
			return ref;
		}

		ref = ir_emit_N(&jit->ctx, IR_OPT(IR_PHI, jit->ctx.ir_base[inputs[0]].type), n + 1);
		ir_set_op(&jit->ctx, ref, 1, jit->control);
		for (i = 0; i < n; i++) {
			ir_set_op(&jit->ctx, ref, i + 2, inputs[i]);
		}
		return ref;
	}
}

static ir_ref zend_jit_if(zend_jit_ctx *jit, ir_ref condition)
{
	ZEND_ASSERT(jit->control);
	return ir_emit2(&jit->ctx, IR_IF, jit->control, condition);
}

static void zend_jit_if_true(zend_jit_ctx *jit, ir_ref if_ref)
{
	ZEND_ASSERT(if_ref);
	jit->control = ir_emit1(&jit->ctx, IR_IF_TRUE, if_ref);
}

static void zend_jit_if_true_cold(zend_jit_ctx *jit, ir_ref if_ref)
{
	ZEND_ASSERT(if_ref);
	/* op2 is used as an indicator of low-probability branch */
	jit->control = ir_emit2(&jit->ctx, IR_IF_TRUE, if_ref, 1);
}

static void zend_jit_if_false(zend_jit_ctx *jit, ir_ref if_ref)
{
	ZEND_ASSERT(if_ref);
	jit->control = ir_emit1(&jit->ctx, IR_IF_FALSE, if_ref);
}

static void zend_jit_if_false_cold(zend_jit_ctx *jit, ir_ref if_ref)
{
	ZEND_ASSERT(if_ref);
	/* op2 is used as an indicator of low-probability branch */
	jit->control = ir_emit2(&jit->ctx, IR_IF_FALSE, if_ref, 1);
}

static ir_ref zend_jit_alloca(zend_jit_ctx *jit, ir_ref size_ref)
{
	ZEND_ASSERT(jit->control);
	return jit->control = ir_emit2(&jit->ctx, IR_OPT(IR_ALLOCA, IR_ADDR), jit->control, size_ref);
}

static void zend_jit_afree(zend_jit_ctx *jit, ir_ref size_ref)
{
	ZEND_ASSERT(jit->control);
	jit->control = ir_emit2(&jit->ctx, IR_AFREE, jit->control, size_ref);
}

static const char* zend_reg_name(int8_t reg)
{
	return ir_reg_name(reg, ir_reg_is_int(reg) ? IR_PHP_LONG : IR_DOUBLE);
}

static void zend_jit_snapshot(zend_jit_ctx *jit, ir_ref addr)
{
	if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE
	 && JIT_G(current_frame)) {
		const zend_op_array *op_array = &JIT_G(current_frame)->func->op_array;
		uint32_t stack_size = op_array->last_var + op_array->T;

		if (stack_size) {
			zend_jit_trace_stack *stack = JIT_G(current_frame)->stack;
			uint32_t snapshot_size, i;

			/* Check if we need snapshot entries for polymorphic method call */
			zend_jit_trace_info *t = jit->trace;
			uint32_t exit_point = 0, n = 0;

			if (addr < 0) {
				if (t->exit_count > 0
				 && jit->ctx.ir_base[addr].val.u64 == (uintptr_t)zend_jit_trace_get_exit_addr(t->exit_count - 1)) {
					exit_point = t->exit_count - 1;
					if (t->exit_info[exit_point].flags & ZEND_JIT_EXIT_METHOD_CALL) {
						n = 2;
					}
				}
			}

			/* TODO: Use sparse snapshots ??? */
			snapshot_size = stack_size;
			while (snapshot_size > 0 && !STACK_REF(stack, snapshot_size - 1)) {
				snapshot_size--;
			}
			if (snapshot_size || n) {
				ir_ref snapshot;

				snapshot = ir_emit_N(&jit->ctx, IR_SNAPSHOT, snapshot_size + 1 + n);
				ir_set_op(&jit->ctx, snapshot, 1, jit->control);
				for (i = 0; i < snapshot_size; i++) {
					ir_ref ref = STACK_REF(stack, i);

					ZEND_ASSERT(STACK_REF(stack, i) != IR_NULL);
					ir_set_op(&jit->ctx, snapshot, 2 + i, ref);
				}
				if (n) {
					ir_set_op(&jit->ctx, snapshot, 1 + snapshot_size + 1, t->exit_info[exit_point].poly_func_ref);
					ir_set_op(&jit->ctx, snapshot, 1 + snapshot_size + 2, t->exit_info[exit_point].poly_this_ref);
				}
				jit->control = snapshot;
			}
		}
	}
}

static int8_t zend_jit_add_trace_const(zend_jit_trace_info *t, int64_t val)
{
	int32_t i;

	for (i = 0; i < t->consts_count; i++) {
		if (t->constants[i].i == val) {
			return i;
		}
	}
	ZEND_ASSERT(i < 127); // TODO: extend possible number of constants ???
	t->consts_count = i + 1;
	t->constants = erealloc(t->constants, (i + 1) * sizeof(zend_jit_exit_const));
	t->constants[i].i = val;
	return i;
}

void *zend_jit_snapshot_handler(ir_ctx *ctx, ir_ref snapshot_ref, ir_insn *snapshot, void *addr)
{
	zend_jit_trace_info *t = ((zend_jit_ctx*)ctx)->trace;
	uint32_t exit_point;
	ir_ref n = snapshot->inputs_count;
	ir_ref i;

	if (addr == zend_jit_stub_handlers[jit_stub_exception_handler]
     || addr == zend_jit_stub_handlers[jit_stub_exception_handler_undef]
     || addr == zend_jit_stub_handlers[jit_stub_exception_handler_free_op1_op2]
     || addr == zend_jit_stub_handlers[jit_stub_exception_handler_free_op2]
     || addr == zend_jit_stub_handlers[jit_stub_interrupt_handler]
     || addr == zend_jit_stub_handlers[jit_stub_leave_function_handler]
     || addr == zend_jit_stub_handlers[jit_stub_negative_shift]
     || addr == zend_jit_stub_handlers[jit_stub_mod_by_zero]
     || addr == zend_jit_stub_handlers[jit_stub_invalid_this]
     || addr == zend_jit_stub_handlers[jit_stub_undefined_function]
     || addr == zend_jit_stub_handlers[jit_stub_throw_cannot_pass_by_ref]
     || addr == zend_jit_stub_handlers[jit_stub_icall_throw]
     || addr == zend_jit_stub_handlers[jit_stub_leave_throw]
     || addr == zend_jit_stub_handlers[jit_stub_trace_halt]
     || addr == zend_jit_stub_handlers[jit_stub_trace_escape]) {
		// TODO: ???
		return addr;
	}
	exit_point = zend_jit_exit_point_by_addr(addr);
	ZEND_ASSERT(exit_point < t->exit_count);

	if (t->exit_info[exit_point].flags & ZEND_JIT_EXIT_METHOD_CALL) {
		ZEND_ASSERT(ctx->regs[snapshot_ref][n - 1] != -1 && ctx->regs[snapshot_ref][n] != -1);
		t->exit_info[exit_point].poly_func_reg = ctx->regs[snapshot_ref][n - 1];
		t->exit_info[exit_point].poly_this_reg = ctx->regs[snapshot_ref][n];
		n -= 2;
	}

	for (i = 2; i <= n; i++) {
		ir_ref ref = ir_insn_op(snapshot, i);

		if (ref) {
			int8_t *reg_ops = ctx->regs[snapshot_ref];
			int8_t reg = reg_ops[i];
			ir_ref var = i - 2; // TODO: Use sparse snapshots ???

			ZEND_ASSERT(var < t->exit_info[exit_point].stack_size);
			if (t->stack_map[t->exit_info[exit_point].stack_offset + var].flags == ZREG_ZVAL_COPY) {
				ZEND_ASSERT(reg != ZREG_NONE);
				t->stack_map[t->exit_info[exit_point].stack_offset + var].reg = reg & 0x3f; // TODO: remove magic mask ???
			} else {
				ZEND_ASSERT(t->stack_map[t->exit_info[exit_point].stack_offset + var].type == IS_LONG ||
					t->stack_map[t->exit_info[exit_point].stack_offset + var].type == IS_DOUBLE);

				if (ref > 0) {
					if (reg != ZREG_NONE) {
						t->stack_map[t->exit_info[exit_point].stack_offset + var].reg = reg & 0x3f; // TODO: remove magic mask ???
					} else {
						t->stack_map[t->exit_info[exit_point].stack_offset + var].flags = ZREG_TYPE_ONLY;
					}
				} else {
					int8_t idx = zend_jit_add_trace_const(t, ctx->ir_base[ref].val.i64);
					t->stack_map[t->exit_info[exit_point].stack_offset + var].flags = ZREG_CONST;
					t->stack_map[t->exit_info[exit_point].stack_offset + var].reg = idx;
				}
			}
		}
	}
	return addr;
}

static void zend_jit_side_exit(zend_jit_ctx *jit, ir_ref addr)
{
	zend_jit_snapshot(jit, addr);
	zend_jit_ijmp(jit, addr);
}

static void zend_jit_guard(zend_jit_ctx *jit, ir_ref condition, ir_ref addr)
{
	ZEND_ASSERT(jit->control);
	if (condition == IR_TRUE) {
		return;
	} else {
		ir_ref ref = jit->control;
		ir_insn *insn;

		while (ref > condition) {
			insn = &jit->ctx.ir_base[ref];
			if (insn->op == IR_GUARD) {
				if (insn->op2 == condition) {
					return;
				}
			} else if (insn->op == IR_GUARD_NOT) {
				if (insn->op2 == condition) {
					condition = IR_FALSE;
					return;
				}
			} else if (insn->op == IR_START
				 || insn->op == IR_BEGIN
				 || insn->op == IR_IF_TRUE
				 || insn->op == IR_IF_FALSE
				 || insn->op == IR_CASE_VAL
				 || insn->op == IR_CASE_DEFAULT
				 || insn->op == IR_MERGE
				 || insn->op == IR_LOOP_BEGIN
				 || insn->op == IR_ENTRY) {
				break;
			}
			ref = insn->op1;
		}
	}
	zend_jit_snapshot(jit, addr);
	jit->control = ir_emit3(&jit->ctx, IR_GUARD, jit->control, condition, addr);
}

static void zend_jit_guard_not(zend_jit_ctx *jit, ir_ref condition, ir_ref addr)
{
	ZEND_ASSERT(jit->control);
	if (condition == IR_FALSE) {
		return;
	} else {
		ir_ref ref = jit->control;
		ir_insn *insn;

		while (ref > condition) {
			insn = &jit->ctx.ir_base[ref];
			if (insn->op == IR_GUARD_NOT) {
				if (insn->op2 == condition) {
					return;
				}
			} else if (insn->op == IR_GUARD) {
				if (insn->op2 == condition) {
					condition = IR_TRUE;
					return;
				}
			} else if (insn->op == IR_START
				 || insn->op == IR_BEGIN
				 || insn->op == IR_IF_TRUE
				 || insn->op == IR_IF_FALSE
				 || insn->op == IR_CASE_VAL
				 || insn->op == IR_CASE_DEFAULT
				 || insn->op == IR_MERGE
				 || insn->op == IR_LOOP_BEGIN
				 || insn->op == IR_ENTRY) {
				break;
			}
			ref = insn->op1;
		}
	}
	zend_jit_snapshot(jit, addr);
	jit->control = ir_emit3(&jit->ctx, IR_GUARD_NOT, jit->control, condition, addr);
}

static ir_ref zend_jit_addr_offset(zend_jit_ctx *jit, ir_ref addr, uintptr_t offset)
{
	if (offset) {
		addr = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
			addr,
			zend_jit_const_addr(jit, offset));
	}
	return addr;
}

static ir_ref zend_jit_load_at_offset(zend_jit_ctx *jit, ir_type type, ir_ref addr, uintptr_t offset)
{
	return zend_jit_load(jit, type,
		zend_jit_addr_offset(jit, addr, offset));
}

static void zend_jit_store_at_offset(zend_jit_ctx *jit, ir_ref addr, uintptr_t offset, ir_ref val)
{
	zend_jit_store(jit,
		zend_jit_addr_offset(jit, addr, offset),
		val);
}

/* PHP helpers */

static ir_ref zend_jit_emalloc(zend_jit_ctx *jit, size_t size, const zend_op_array *op_array, const zend_op *opline)
{
#if ZEND_DEBUG
	return zend_jit_call_5(jit, IR_ADDR,
		zend_jit_const_func_addr(jit, (uintptr_t)_emalloc, IR_CONST_FASTCALL_FUNC),
		zend_jit_const_addr(jit, size),
		op_array->filename ? zend_jit_const_addr(jit, (uintptr_t)op_array->filename->val) : IR_NULL,
		ir_const_u32(&jit->ctx, opline ? opline->lineno : 0),
		IR_NULL,
		ir_const_u32(&jit->ctx, 0));
#elif defined(HAVE_BUILTIN_CONSTANT_P)
	if (size > 24 && size <= 32) {
		return zend_jit_call_0(jit, IR_ADDR,
			zend_jit_const_func_addr(jit, (uintptr_t)_emalloc_32, IR_CONST_FASTCALL_FUNC));
	} else {
		return zend_jit_call_1(jit, IR_ADDR,
			zend_jit_const_func_addr(jit, (uintptr_t)_emalloc, IR_CONST_FASTCALL_FUNC),
			zend_jit_const_addr(jit, size));
	}
#else
	return zend_jit_call_1(jit, IR_ADDR,
		zend_jit_const_func_addr(jit, (uintptr_t)_emalloc, IR_CONST_FASTCALL_FUNC),
		zend_jit_const_addr(jit, size));
#endif
}

static ir_ref zend_jit_efree(zend_jit_ctx *jit, ir_ref ptr, size_t size, const zend_op_array *op_array, const zend_op *opline)
{
#if ZEND_DEBUG
	return zend_jit_call_5(jit, IR_ADDR,
		zend_jit_const_func_addr(jit, (uintptr_t)_efree, IR_CONST_FASTCALL_FUNC),
		ptr,
		op_array->filename ? zend_jit_const_addr(jit, (uintptr_t)op_array->filename->val) : IR_NULL,
		ir_const_u32(&jit->ctx, opline ? opline->lineno : 0),
		IR_NULL,
		ir_const_u32(&jit->ctx, 0));
#elif defined(HAVE_BUILTIN_CONSTANT_P)
	if (size > 24 && size <= 32) {
		return zend_jit_call_1(jit, IR_ADDR,
			zend_jit_const_func_addr(jit, (uintptr_t)_efree_32, IR_CONST_FASTCALL_FUNC),
			ptr);
	} else {
		return zend_jit_call_1(jit, IR_ADDR,
			zend_jit_const_func_addr(jit, (uintptr_t)_efree, IR_CONST_FASTCALL_FUNC),
			ptr);
	}
#else
	return zend_jit_call_1(jit, IR_ADDR,
		zend_jit_const_func_addr(jit, (uintptr_t)_efree, IR_CONST_FASTCALL_FUNC),
		ptr);
#endif
}

static ir_ref zend_jit_fp(zend_jit_ctx *jit)
{
	ZEND_ASSERT(jit->control);
	if (jit->fp == IR_UNUSED) {
		/* Emit "RLOAD FP" once for basic block ??? */
		jit->fp = zend_jit_rload(jit, IR_ADDR, ZREG_FP);
	} else {
		ir_insn *insn;
		ir_ref ref = jit->control;

		while (1) {
			if (ref == jit->fp) {
				break;
			}
			insn = &jit->ctx.ir_base[ref];
			if (insn->op == IR_START
			 || insn->op == IR_BEGIN
			 || insn->op == IR_IF_TRUE
			 || insn->op == IR_IF_FALSE
			 || insn->op == IR_CASE_VAL
			 || insn->op == IR_CASE_DEFAULT
			 || insn->op == IR_MERGE
			 || insn->op == IR_LOOP_BEGIN
			 || insn->op == IR_ENTRY
			 || insn->op == IR_CALL) {
				jit->fp = zend_jit_rload(jit, IR_ADDR, ZREG_FP);
				break;
			}
			ref = insn->op1;
		}
	}
	return jit->fp;
}

static void zend_jit_store_fp(zend_jit_ctx *jit, ir_ref ref)
{
	zend_jit_rstore(jit, ref, ZREG_FP);
	jit->fp = IR_UNUSED;
}

static ir_ref zend_jit_ip(zend_jit_ctx *jit)
{
	return zend_jit_rload(jit, IR_ADDR, ZREG_IP);
}

static void zend_jit_store_ip(zend_jit_ctx *jit, ir_ref ref)
{
	zend_jit_rstore(jit, ref, ZREG_IP);
}

static ir_ref zend_jit_ip32(zend_jit_ctx *jit)
{
	return zend_jit_rload(jit, IR_U32, ZREG_IP);
}

static void zend_jit_track_last_valid_opline(zend_jit_ctx *jit)
{
	jit->use_last_valid_opline = 0;
	jit->track_last_valid_opline = 1;
}

static void zend_jit_use_last_valid_opline(zend_jit_ctx *jit)
{
	if (jit->track_last_valid_opline) {
		jit->use_last_valid_opline = 1;
		jit->track_last_valid_opline = 0;
	}
}

static bool zend_jit_trace_uses_initial_ip(zend_jit_ctx *jit)
{
	return jit->use_last_valid_opline;
}

static void zend_jit_set_last_valid_opline(zend_jit_ctx *jit, const zend_op *opline)
{
	if (!jit->reuse_ip) {
		jit->track_last_valid_opline = 1;
		jit->last_valid_opline = opline;
	}
}

static void zend_jit_reset_last_valid_opline(zend_jit_ctx *jit)
{
	jit->track_last_valid_opline = 0;
	jit->last_valid_opline = NULL;
}

static void zend_jit_start_reuse_ip(zend_jit_ctx *jit)
{
	zend_jit_reset_last_valid_opline(jit);
	jit->reuse_ip = 1;
}

static int zend_jit_reuse_ip(zend_jit_ctx *jit)
{
	if (!jit->reuse_ip) {
		zend_jit_start_reuse_ip(jit);
		// RX = EX(call);
		zend_jit_store_ip(jit,
			zend_jit_load_at_offset(jit, IR_ADDR,
				zend_jit_fp(jit), offsetof(zend_execute_data, call)));
	}
	return 1;
}

static void zend_jit_stop_reuse_ip(zend_jit_ctx *jit)
{
	jit->reuse_ip = 0;
}

static int zend_jit_save_call_chain(zend_jit_ctx *jit, uint32_t call_level)
{
	ir_ref rx, call;

	if (call_level == 1) {
		// JIT: call = NULL;
		call = IR_NULL;
	} else {
		// JIT: call = EX(call);
		call = zend_jit_load_at_offset(jit, IR_ADDR,
			zend_jit_fp(jit), offsetof(zend_execute_data, call));
	}

	rx = zend_jit_ip(jit);

	// JIT: call->prev_execute_data = call;
	zend_jit_store_at_offset(jit,
		rx, offsetof(zend_execute_data, prev_execute_data),
		call);

	// JIT: EX(call) = call;
	zend_jit_store_at_offset(jit,
		zend_jit_fp(jit), offsetof(zend_execute_data, call),
		rx);

	jit->delayed_call_level = 0;
	delayed_call_chain = 0; // TODO: remove ???

	return 1;
}

static int zend_jit_ex_opline_addr(zend_jit_ctx *jit)
{
	return zend_jit_addr_offset(jit, zend_jit_fp(jit), offsetof(zend_execute_data, opline));
}

static void zend_jit_load_ip(zend_jit_ctx *jit, ir_ref ref)
{
	if (GCC_GLOBAL_REGS) {
		zend_jit_store_ip(jit, ref);
	} else {
		zend_jit_store(jit,
			zend_jit_ex_opline_addr(jit),
			ref);
	}
}

static void zend_jit_load_ip_addr(zend_jit_ctx *jit, const zend_op *target)
{
	zend_jit_load_ip(jit,
		zend_jit_const_addr(jit, (uintptr_t)target));
}

static int zend_jit_set_ip(zend_jit_ctx *jit, const zend_op *target)
{
	ir_ref ref;
	ir_ref addr = IR_UNUSED;

	if (jit->delayed_call_level) {
		if (!zend_jit_save_call_chain(jit, jit->delayed_call_level)) {
			return 0;
		}
	}

	if (jit->last_valid_opline) {
		zend_jit_use_last_valid_opline(jit);
		if (jit->last_valid_opline != target) {
			if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
				ref = zend_jit_ip(jit);
			} else {
				addr = zend_jit_ex_opline_addr(jit);
				ref = zend_jit_load(jit, IR_ADDR, addr);
			}
			if (target > jit->last_valid_opline) {
				ref = zend_jit_addr_offset(jit, ref,
					(uintptr_t)target - (uintptr_t)jit->last_valid_opline);
			} else {
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_ADDR), ref,
					zend_jit_const_addr(jit, (uintptr_t)jit->last_valid_opline - (uintptr_t)target));
			}
			if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
				zend_jit_store_ip(jit, ref);
			} else {
				zend_jit_store(jit, addr, ref);
			}
		}
	} else {
		if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
			zend_jit_store_ip(jit,
				zend_jit_const_addr(jit, (uintptr_t)target));
		} else {
			zend_jit_store(jit,
				zend_jit_ex_opline_addr(jit),
				zend_jit_const_addr(jit, (uintptr_t)target));
		}
	}
	jit->reuse_ip = 0;
	zend_jit_set_last_valid_opline(jit, target);
	return 1;
}

static int zend_jit_set_ip_ex(zend_jit_ctx *jit, const zend_op *target, bool set_ip_reg)
{
	//???
	return zend_jit_set_ip(jit, target);
}

static void zend_jit_set_ex_opline(zend_jit_ctx *jit, const zend_op *target)
{
	if (jit->last_valid_opline == target) {
		zend_jit_use_last_valid_opline(jit);
		if (GCC_GLOBAL_REGS) {
			// EX(opline) = opline
			zend_jit_store(jit,
				zend_jit_ex_opline_addr(jit),
				zend_jit_ip(jit));
		}
	} else {
		zend_jit_store(jit,
			zend_jit_ex_opline_addr(jit),
			zend_jit_const_addr(jit, (uintptr_t)target));
		if (!GCC_GLOBAL_REGS) {
			zend_jit_reset_last_valid_opline(jit);
		}
	}
}

static ir_ref zend_jit_mem_zval_addr(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_REF_ZVAL) {
		return Z_IR_REF(addr);
	} else {
		ir_ref reg;

		ZEND_ASSERT(Z_MODE(addr) == IS_MEM_ZVAL);
		if (Z_REG(addr) == ZREG_FP) {
			reg = zend_jit_fp(jit);
		} else if (Z_REG(addr) == ZREG_RX) {
			reg = zend_jit_ip(jit);
		} else {
			ZEND_ASSERT(0);
		}
		return zend_jit_addr_offset(jit, reg, Z_OFFSET(addr));
	}
}

static ir_ref zend_jit_zval_addr(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_CONST_ZVAL) {
		return zend_jit_const_addr(jit, (uintptr_t)Z_ZV(addr));
	} else {
		return zend_jit_mem_zval_addr(jit, addr);
	}
}

static ir_ref zend_jit_zval_ref_type(zend_jit_ctx *jit, ir_ref ref)
{
	return zend_jit_load_at_offset(jit, IR_U8,
		ref, offsetof(zval, u1.v.type));
}

static ir_ref zend_jit_zval_type(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_CONST_ZVAL) {
		return ir_const_u8(&jit->ctx, Z_TYPE_P(Z_ZV(addr)));
	} else if (Z_MODE(addr) == IS_MEM_ZVAL) {
		ir_ref reg;

		ZEND_ASSERT(Z_MODE(addr) == IS_MEM_ZVAL);
		if (Z_REG(addr) == ZREG_FP) {
			reg = zend_jit_fp(jit);
		} else if (Z_REG(addr) == ZREG_RX) {
			reg = zend_jit_ip(jit);
		} else {
			ZEND_ASSERT(0);
		}
		return zend_jit_load_at_offset(jit, IR_U8,
			reg, Z_OFFSET(addr) + offsetof(zval, u1.v.type));
	} else {
		return zend_jit_zval_ref_type(jit, zend_jit_mem_zval_addr(jit, addr));
	}
}

static ir_ref zend_jit_zval_ref_type_flags(zend_jit_ctx *jit, ir_ref ref)
{
	return zend_jit_load_at_offset(jit, IR_U8,
		ref, offsetof(zval, u1.v.type_flags));
}

static ir_ref zend_jit_zval_type_flags(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_CONST_ZVAL) {
		return ir_const_u8(&jit->ctx, Z_TYPE_FLAGS_P(Z_ZV(addr)));
	} else if (Z_MODE(addr) == IS_MEM_ZVAL) {
		ir_ref reg;

		ZEND_ASSERT(Z_MODE(addr) == IS_MEM_ZVAL);
		if (Z_REG(addr) == ZREG_FP) {
			reg = zend_jit_fp(jit);
		} else if (Z_REG(addr) == ZREG_RX) {
			reg = zend_jit_ip(jit);
		} else {
			ZEND_ASSERT(0);
		}
		return zend_jit_load_at_offset(jit, IR_U8,
			reg, Z_OFFSET(addr) + offsetof(zval, u1.v.type_flags));
	} else {
		return zend_jit_zval_ref_type_flags(jit, zend_jit_mem_zval_addr(jit, addr));
	}
}

static ir_ref zend_jit_zval_ref_type_info(zend_jit_ctx *jit, ir_ref ref)
{
	return zend_jit_load_at_offset(jit, IR_U32,
		ref, offsetof(zval, u1.type_info));
}

static ir_ref zend_jit_zval_type_info(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_CONST_ZVAL) {
		return ir_const_u32(&jit->ctx, Z_TYPE_INFO_P(Z_ZV(addr)));
	} else if (Z_MODE(addr) == IS_MEM_ZVAL) {
		ir_ref reg;

		ZEND_ASSERT(Z_MODE(addr) == IS_MEM_ZVAL);
		if (Z_REG(addr) == ZREG_FP) {
			reg = zend_jit_fp(jit);
		} else if (Z_REG(addr) == ZREG_RX) {
			reg = zend_jit_ip(jit);
		} else {
			ZEND_ASSERT(0);
		}
		return zend_jit_load_at_offset(jit, IR_U32,
			reg, Z_OFFSET(addr) + offsetof(zval, u1.type_info));
	} else {
		return zend_jit_zval_ref_type_info(jit, zend_jit_mem_zval_addr(jit, addr));
	}
}

static void zend_jit_zval_ref_set_type_info(zend_jit_ctx *jit, ir_ref ref, ir_ref type_info)
{
	zend_jit_store_at_offset(jit,
		ref, offsetof(zval, u1.type_info),
		type_info);
}

static void zend_jit_zval_set_type_info_ref(zend_jit_ctx *jit, zend_jit_addr addr, ir_ref type_info)
{
	if (Z_MODE(addr) == IS_MEM_ZVAL) {
		ir_ref reg;

		ZEND_ASSERT(Z_MODE(addr) == IS_MEM_ZVAL);
		if (Z_REG(addr) == ZREG_FP) {
			reg = zend_jit_fp(jit);
		} else if (Z_REG(addr) == ZREG_RX) {
			reg = zend_jit_ip(jit);
		} else {
			ZEND_ASSERT(0);
		}
		zend_jit_store_at_offset(jit,
			reg, Z_OFFSET(addr) + offsetof(zval, u1.type_info),
			type_info);
	} else {
		zend_jit_zval_ref_set_type_info(jit,
			zend_jit_mem_zval_addr(jit, addr),
			type_info);
	}
}

static void zend_jit_zval_set_type_info(zend_jit_ctx *jit, zend_jit_addr addr, uint32_t type_info)
{
	if (type_info < IS_STRING
	 && Z_MODE(addr) == IS_MEM_ZVAL
	 && Z_REG(addr) == ZREG_FP
	 && JIT_G(current_frame)
	 && STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(Z_OFFSET(addr))) == type_info) {
		/* type is already set */
		return;
	}
	zend_jit_zval_set_type_info_ref(jit, addr, ir_const_u32(&jit->ctx, type_info)); 
}

static ir_ref zend_jit_if_zval_ref_type(zend_jit_ctx *jit, ir_ref ref, ir_ref type)
{
	return zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
			zend_jit_zval_ref_type(jit, ref), type));
}

static ir_ref zend_jit_if_zval_type(zend_jit_ctx *jit, zend_jit_addr addr, uint8_t type)
{
	return zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
			zend_jit_zval_type(jit, addr),
			ir_const_u8(&jit->ctx, type)));
}

static ir_ref zend_jit_if_not_zval_type(zend_jit_ctx *jit, zend_jit_addr addr, uint8_t type)
{
	ir_ref ref = zend_jit_zval_type(jit, addr);

	if (type != IS_UNDEF) {
		ref = ir_fold2(&jit->ctx, IR_OPT(IR_NE, IR_BOOL),
			ref,
			ir_const_u8(&jit->ctx, type));
	}
	return zend_jit_if(jit, ref);
}

static void zend_jit_guard_zval_type(zend_jit_ctx *jit, zend_jit_addr addr, uint8_t type, const void *exit_addr)
{
	zend_jit_guard(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
			zend_jit_zval_type(jit, addr),
			ir_const_u8(&jit->ctx, type)),
		zend_jit_const_addr(jit, (uintptr_t)exit_addr));
}

static void zend_jit_guard_not_zval_type(zend_jit_ctx *jit, zend_jit_addr addr, uint8_t type, const void *exit_addr)
{
	zend_jit_guard(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_NE, IR_BOOL),
			zend_jit_zval_type(jit, addr),
			ir_const_u8(&jit->ctx, type)),
		zend_jit_const_addr(jit, (uintptr_t)exit_addr));
}

static ir_ref zend_jit_if_zval_refcounted(zend_jit_ctx *jit, zend_jit_addr addr)
{
	return zend_jit_if(jit, zend_jit_zval_type_flags(jit, addr));
}

static ir_ref zend_jit_if_zval_ref_collectable(zend_jit_ctx *jit, ir_ref addr_ref)
{
	return zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U8),
			zend_jit_zval_ref_type_flags(jit, addr_ref),
			ir_const_u8(&jit->ctx, IS_TYPE_COLLECTABLE)));
}

static ir_ref zend_jit_zval_ref_lval(zend_jit_ctx *jit, ir_ref ref)
{
	return zend_jit_load(jit, IR_PHP_LONG, ref);
}

static ir_ref zend_jit_zval_ref_dval(zend_jit_ctx *jit, ir_ref ref)
{
	return zend_jit_load(jit, IR_DOUBLE, ref);
}

static void zend_jit_def_reg(zend_jit_ctx *jit, zend_jit_addr addr, ir_ref val)
{
	int var;

	ZEND_ASSERT(Z_MODE(addr) == IS_REG);
	var = Z_SSA_VAR(addr);
	if (var == jit->delay_var) {
		jit->delay_buf[jit->delay_count++] = val;
		return;
	}
	ZEND_ASSERT(jit->ra && jit->ra[var].ref == IR_NULL);
	/* Negative "var" has special meaning for IR */
	val = ir_bind(&jit->ctx, -EX_NUM_TO_VAR(jit->ssa->vars[var].var), val);
	jit->ra[var].ref = val;

	if (jit->ra[var].flags & ZREG_FORWARD) {
		zend_ssa_phi *phi = jit->ssa->vars[var].phi_use_chain;
		zend_basic_block *bb;
		int n, j, *p;
		ir_ref *q;

		jit->ra[var].flags &= ~ZREG_FORWARD;
		while (phi != NULL) {
			zend_ssa_phi *dst_phi = phi;
			int src_var = var;

			if (dst_phi->pi >= 0) {
				jit->ra[src_var].ref = val;
				src_var = dst_phi->ssa_var;
				if (!(jit->ra[src_var].flags & ZREG_FORWARD)) {
					phi = zend_ssa_next_use_phi(jit->ssa, var, phi);
					continue;
				}
				dst_phi = jit->ssa->vars[src_var].phi_use_chain;
				ZEND_ASSERT(dst_phi != NULL && "reg forwarding");
				ZEND_ASSERT(!zend_ssa_next_use_phi(jit->ssa, src_var, dst_phi) && "reg forwarding");
				jit->ra[src_var].flags &= ~ZREG_FORWARD;
			}

			if (jit->ra[dst_phi->ssa_var].ref > 0) {
				ir_insn *phi_insn = &jit->ctx.ir_base[jit->ra[dst_phi->ssa_var].ref];
				ZEND_ASSERT(phi_insn->op == IR_PHI);
//				ZEND_ASSERT(ir_operands_count(ctx, phi_insn) == n + 1);
				bb = &jit->ssa->cfg.blocks[dst_phi->block];
				n = bb->predecessors_count;
				for (j = 0, p = &dst_phi->sources[0], q = phi_insn->ops + 2; j < n; j++, p++, q++) {
					if (*p == src_var) {
						*q = val;
					}
				}
			}

			phi = zend_ssa_next_use_phi(jit->ssa, var, phi);
		}
	}
}

static ir_ref zend_jit_use_reg(zend_jit_ctx *jit, zend_jit_addr addr)
{
	int var = Z_SSA_VAR(addr);

	ZEND_ASSERT(Z_MODE(addr) == IS_REG);
	ZEND_ASSERT(jit->ra && jit->ra[var].ref);
	if (jit->ra[var].ref == IR_NULL) {
		zend_jit_addr mem_addr;
		ir_ref ref;

		ZEND_ASSERT(jit->ra[var].flags & ZREG_LOAD);
		mem_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, EX_NUM_TO_VAR(jit->ssa->vars[var].var));
		if ((jit->ssa->var_info[var].type & MAY_BE_ANY) == MAY_BE_LONG) {
			ref = zend_jit_zval_ref_lval(jit, zend_jit_mem_zval_addr(jit, mem_addr));
		} else if ((jit->ssa->var_info[var].type & MAY_BE_ANY) == MAY_BE_DOUBLE) {
			ref = zend_jit_zval_ref_dval(jit, zend_jit_mem_zval_addr(jit, mem_addr));
		} else {
			ZEND_ASSERT(0);
		}
		zend_jit_def_reg(jit, addr, ref);
		return ref;
	}
	return jit->ra[Z_SSA_VAR(addr)].ref;
}

static void zend_jit_gen_pi(zend_jit_ctx *jit, zend_ssa_phi *phi)
{
	int src_var = phi->sources[0];
	int dst_var = phi->ssa_var;

	ZEND_ASSERT(phi->pi >= 0);
	ZEND_ASSERT(!(jit->ra[dst_var].flags & ZREG_LOAD));
	ZEND_ASSERT(jit->ra[src_var].ref);

	if (jit->ra[src_var].ref == IR_NULL) {
		/* Not defined yet */
		if (jit->ssa->vars[dst_var].use_chain < 0
		 && jit->ssa->vars[dst_var].phi_use_chain) {
			zend_ssa_phi *phi = jit->ssa->vars[dst_var].phi_use_chain;
			if (!zend_ssa_next_use_phi(jit->ssa, dst_var, phi)) {
				/* This is a Pi forwarded to Phi */
				jit->ra[src_var].flags |= ZREG_FORWARD;
				return;
			}
		}
		ZEND_ASSERT(0 && "Not defined Pi source");
	}
	/* Reuse register */
	zend_jit_def_reg(jit, ZEND_ADDR_REG(dst_var),
		zend_jit_use_reg(jit, ZEND_ADDR_REG(src_var)));
}

static void zend_jit_gen_phi(zend_jit_ctx *jit, zend_ssa_phi *phi)
{
	int dst_var = phi->ssa_var;
	zend_basic_block *bb = &jit->ssa->cfg.blocks[phi->block];
	int n = bb->predecessors_count;
	int i;
	ir_type type = (jit->ssa->var_info[phi->ssa_var].type & MAY_BE_LONG) ? IR_PHP_LONG : IR_DOUBLE;
	ir_ref merge = jit->bb_start_ref[phi->block];
	ir_ref ref;

	ZEND_ASSERT(phi->pi < 0);
	ZEND_ASSERT(!(jit->ra[dst_var].flags & ZREG_LOAD));
	ZEND_ASSERT(merge);
	ZEND_ASSERT(jit->ctx.ir_base[merge].op == IR_MERGE || jit->ctx.ir_base[merge].op == IR_LOOP_BEGIN);
	ZEND_ASSERT(n == (jit->ctx.ir_base[merge].inputs_count ? jit->ctx.ir_base[merge].inputs_count : 2));

	ref = ir_emit_N(&jit->ctx, IR_OPT(IR_PHI, type), n + 1);
	ir_set_op(&jit->ctx, ref, 1, merge);

	for (i = 0; i < n; i++) {
		int src_var = phi->sources[i];

		ZEND_ASSERT(jit->ra[src_var].ref);
		if (jit->ra[src_var].ref == IR_NULL) {
			jit->ra[src_var].flags |= ZREG_FORWARD;
		} else {
			ir_set_op(&jit->ctx, ref, i + 2,
				zend_jit_use_reg(jit, ZEND_ADDR_REG(src_var)));
		}
	}

	zend_jit_def_reg(jit, ZEND_ADDR_REG(dst_var), ref);
}

static ir_ref zend_jit_zval_lval(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_CONST_ZVAL) {
		return ir_const_php_long(&jit->ctx, Z_LVAL_P(Z_ZV(addr)));
	} else if (Z_MODE(addr) == IS_REG) {
		return zend_jit_use_reg(jit, addr);
	} else {
		return zend_jit_zval_ref_lval(jit, zend_jit_mem_zval_addr(jit, addr));
	}
}

static void zend_jit_zval_ref_set_lval(zend_jit_ctx *jit, ir_ref ref, ir_ref lval)
{
	zend_jit_store(jit, ref, lval);
}

static void zend_jit_zval_set_lval(zend_jit_ctx *jit, zend_jit_addr addr, ir_ref lval)
{
	if (Z_MODE(addr) == IS_REG) {
		zend_jit_def_reg(jit, addr, lval);
	} else {
		zend_jit_zval_ref_set_lval(jit,
			zend_jit_mem_zval_addr(jit, addr),
			lval);
	}
}

#if SIZEOF_ZEND_LONG == 4
static ir_ref zend_jit_zval_ref_w2(zend_jit_ctx *jit, ir_ref ref)
{
	return zend_jit_load_at_offset(jit, IR_PHP_LONG,
		ref, offsetof(zval, value.ww.w2));
}

static ir_ref zend_jit_zval_w2(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_CONST_ZVAL) {
		return ir_const_u32(&jit->ctx, (Z_ZV(addr))->value.ww.w2);
	} else {
		return zend_jit_zval_ref_w2(jit, zend_jit_mem_zval_addr(jit, addr));
	}
}

static void zend_jit_zval_ref_set_w2(zend_jit_ctx *jit, ir_ref ref, ir_ref lval)
{
	zend_jit_store_at_offset(jit,
		ref, offsetof(zval, value.ww.w2),
		lval);
}

static void zend_jit_zval_set_w2(zend_jit_ctx *jit, zend_jit_addr addr, ir_ref lval)
{
	zend_jit_zval_ref_set_w2(jit,
		zend_jit_mem_zval_addr(jit, addr),
		lval);
}
#endif

static ir_ref zend_jit_zval_dval(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_CONST_ZVAL) {
		return ir_const_double(&jit->ctx, Z_DVAL_P(Z_ZV(addr)));
	} else if (Z_MODE(addr) == IS_REG) {
		return zend_jit_use_reg(jit, addr);
	} else {
		return zend_jit_zval_ref_dval(jit, zend_jit_mem_zval_addr(jit, addr));
	}
}

static void zend_jit_zval_ref_set_dval(zend_jit_ctx *jit, ir_ref ref, ir_ref dval)
{
	zend_jit_store(jit, ref, dval);
}

static void zend_jit_zval_set_dval(zend_jit_ctx *jit, zend_jit_addr addr, ir_ref dval)
{
	if (Z_MODE(addr) == IS_REG) {
		zend_jit_def_reg(jit, addr, dval);
	} else {
		zend_jit_zval_ref_set_dval(jit,
			zend_jit_mem_zval_addr(jit, addr),
			dval);
	}
}

static ir_ref zend_jit_zval_ref_ptr(zend_jit_ctx *jit, ir_ref ref)
{
	return zend_jit_load(jit, IR_ADDR, ref);
}

static ir_ref zend_jit_zval_ptr(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_CONST_ZVAL) {
		return zend_jit_const_addr(jit, (uintptr_t)Z_PTR_P(Z_ZV(addr)));
	} else {
		return zend_jit_zval_ref_ptr(jit, zend_jit_mem_zval_addr(jit, addr));
	}
}

static void zend_jit_zval_ref_set_ptr(zend_jit_ctx *jit, ir_ref ref, ir_ref ptr)
{
	zend_jit_store(jit, ref, ptr);
}

static void zend_jit_zval_set_ptr(zend_jit_ctx *jit, zend_jit_addr addr, ir_ref ptr)
{
	zend_jit_zval_ref_set_ptr(jit,
		zend_jit_mem_zval_addr(jit, addr),
		ptr);
}

static void zend_jit_copy_zval_const(zend_jit_ctx *jit, zend_jit_addr dst, uint32_t dst_info, uint32_t dst_def_info, zval *zv, bool addref)
{
	ir_ref ref = IR_UNUSED;

	if (Z_TYPE_P(zv) > IS_TRUE) {
		if (Z_TYPE_P(zv) == IS_DOUBLE) {
			zend_jit_zval_set_dval(jit, dst,
				ir_const_double(&jit->ctx, Z_DVAL_P(zv)));
		} else if (Z_TYPE_P(zv) == IS_LONG && dst_def_info == MAY_BE_DOUBLE) {
			zend_jit_zval_set_dval(jit, dst,
				ir_const_double(&jit->ctx, (double)Z_LVAL_P(zv)));
		} else if (Z_TYPE_P(zv) == IS_LONG) {
			zend_jit_zval_set_lval(jit, dst,
				ir_const_php_long(&jit->ctx, Z_LVAL_P(zv)));
		} else {
			ir_ref counter;
			ref = zend_jit_const_addr(jit, (uintptr_t)Z_PTR_P(zv));
			zend_jit_zval_set_ptr(jit, dst, ref);
			if (addref && Z_REFCOUNTED_P(zv)) {
				counter = zend_jit_load(jit, IR_U32, ref);
				zend_jit_store(jit, ref,
					ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32), counter,
						ir_const_u32(&jit->ctx, 1)));
			}
		}
	}
	if (Z_MODE(dst) != IS_REG) {
		if (dst_def_info == MAY_BE_DOUBLE) {
			if ((dst_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_GUARD)) != MAY_BE_DOUBLE) {
				zend_jit_zval_set_type_info(jit, dst, IS_DOUBLE);
			}
		} else if (((dst_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_GUARD)) != (1<<Z_TYPE_P(zv))) || (dst_info & (MAY_BE_STRING|MAY_BE_ARRAY)) != 0) {
			zend_jit_zval_set_type_info(jit, dst, Z_TYPE_INFO_P(zv));
		}
	}
}

static ir_ref zend_jit_gc_refcount(zend_jit_ctx *jit, ir_ref ref)
{
	return zend_jit_load(jit, IR_U32, ref);
}

static void zend_jit_gc_set_refcount(zend_jit_ctx *jit, ir_ref ref, uint32_t refcount)
{
	zend_jit_store(jit, ref, ir_const_u32(&jit->ctx, refcount));
}

static void zend_jit_gc_addref(zend_jit_ctx *jit, ir_ref ref)
{
	ir_ref counter = zend_jit_load(jit, IR_U32, ref);
	zend_jit_store(jit, ref,
		ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32), counter,
			ir_const_u32(&jit->ctx, 1)));
}

static void zend_jit_gc_addref2(zend_jit_ctx *jit, ir_ref ref)
{
	ir_ref counter = zend_jit_load(jit, IR_U32, ref);
	zend_jit_store(jit, ref,
		ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32), counter,
			ir_const_u32(&jit->ctx, 2)));
}

static ir_ref zend_jit_gc_delref(zend_jit_ctx *jit, ir_ref ref)
{
	ir_ref counter = zend_jit_load(jit, IR_U32, ref);
	counter = ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_U32), counter, ir_const_u32(&jit->ctx, 1));
	zend_jit_store(jit, ref, counter);
	return counter;
}

static ir_ref zend_jit_if_gc_may_not_leak(zend_jit_ctx *jit, ir_ref ref)
{
	return zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
			zend_jit_load_at_offset(jit, IR_U32,
				ref, offsetof(zend_refcounted, gc.u.type_info)),
			ir_const_u32(&jit->ctx, GC_INFO_MASK | (GC_NOT_COLLECTABLE << GC_FLAGS_SHIFT))));
}

static ir_ref zend_jit_if_typed_ref(zend_jit_ctx *jit, ir_ref ref)
{
	return zend_jit_if(jit,
		zend_jit_load_at_offset(jit, IR_ADDR,
			ref, offsetof(zend_reference, sources.ptr)));
}

static void zend_jit_copy_zval(zend_jit_ctx *jit, zend_jit_addr dst, uint32_t dst_info, zend_jit_addr src, uint32_t src_info, bool addref)
{
	ir_ref ref = IR_UNUSED;

	if (src_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE))) {
		if ((src_info & (MAY_BE_ANY|MAY_BE_GUARD)) == MAY_BE_LONG) {
			ref = zend_jit_zval_lval(jit, src);
			zend_jit_zval_set_lval(jit, dst, ref);
		} else if ((src_info & (MAY_BE_ANY|MAY_BE_GUARD)) == MAY_BE_DOUBLE) {
			ref = zend_jit_zval_dval(jit, src);
			zend_jit_zval_set_dval(jit, dst, ref);
		} else {
#if SIZEOF_ZEND_LONG == 4
			if (src_info & (MAY_BE_DOUBLE|MAY_BE_GUARD)) {
				ref = zend_jit_zval_w2(jit, src);
				zend_jit_zval_set_w2(jit, dst, ref);
			}
#endif
			ref = zend_jit_zval_ptr(jit, src);
			zend_jit_zval_set_ptr(jit, dst, ref);
		}
	}
	if (has_concrete_type(src_info & MAY_BE_ANY)
	 && (src_info & (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE))
     && !(src_info & MAY_BE_GUARD)) {
		if (Z_MODE(dst) != IS_REG
		 && (dst_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_GUARD)) != (src_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_GUARD))) {
			zend_uchar type = concrete_type(src_info);
			zend_jit_zval_set_type_info(jit, dst, type);
		}
	} else {
		ir_ref type = zend_jit_zval_type_info(jit, src);
		zend_jit_zval_set_type_info_ref(jit, dst, type);
		if (addref) {
			if (src_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
				ir_ref if_refcounted = IR_UNUSED;

				if (src_info & (MAY_BE_ANY-(MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
					if_refcounted = zend_jit_if(jit,
						ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32), type, ir_const_u32(&jit->ctx, 0xff00)));
					zend_jit_if_true(jit, if_refcounted);
				}

				zend_jit_gc_addref(jit, ref);

				if (src_info & (MAY_BE_ANY-(MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
					ir_ref ref = zend_jit_end(jit);
					zend_jit_if_false(jit, if_refcounted);
					zend_jit_merge_2(jit, ref, zend_jit_end(jit));
				}
			}
		}
	}
}

static void zend_jit_copy_zval2(zend_jit_ctx *jit, zend_jit_addr dst2, zend_jit_addr dst, uint32_t dst_info, zend_jit_addr src, uint32_t src_info, int addref)
{
	ir_ref ref = IR_UNUSED;

	if (src_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE))) {
		if ((src_info & (MAY_BE_ANY|MAY_BE_GUARD)) == MAY_BE_LONG) {
			ref = zend_jit_zval_lval(jit, src);
			zend_jit_zval_set_lval(jit, dst, ref);
			zend_jit_zval_set_lval(jit, dst2, ref);
		} else if ((src_info & (MAY_BE_ANY|MAY_BE_GUARD)) == MAY_BE_DOUBLE) {
			ref = zend_jit_zval_dval(jit, src);
			zend_jit_zval_set_dval(jit, dst, ref);
			zend_jit_zval_set_dval(jit, dst2, ref);
		} else {
#if SIZEOF_ZEND_LONG == 4
			if (src_info & (MAY_BE_DOUBLE|MAY_BE_GUARD)) {
				ref = zend_jit_zval_w2(jit, src);
				zend_jit_zval_set_w2(jit, dst, ref);
				zend_jit_zval_set_w2(jit, dst2, ref);
			}
#endif
			ref = zend_jit_zval_ptr(jit, src);
			zend_jit_zval_set_ptr(jit, dst, ref);
			zend_jit_zval_set_ptr(jit, dst2, ref);
		}
	}
	if (has_concrete_type(src_info & MAY_BE_ANY)
	 && (src_info & (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE))
     && !(src_info & MAY_BE_GUARD)) {
		zend_uchar type = concrete_type(src_info);
		ir_ref type_ref = ir_const_u32(&jit->ctx, type);

		if (Z_MODE(dst) != IS_REG
		 && (dst_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_GUARD)) != (src_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_GUARD))) {
			zend_jit_zval_set_type_info_ref(jit, dst, type_ref);
		}
		if (Z_MODE(dst2) != IS_REG) {
			zend_jit_zval_set_type_info_ref(jit, dst2, type_ref);
		}
	} else {
		ir_ref type = zend_jit_zval_type_info(jit, src);
		zend_jit_zval_set_type_info_ref(jit, dst, type);
		zend_jit_zval_set_type_info_ref(jit, dst2, type);
		if (addref) {
			if (src_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
				ir_ref if_refcounted = IR_UNUSED;

				if (src_info & (MAY_BE_ANY-(MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
					if_refcounted = zend_jit_if(jit,
						ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32), type, ir_const_u32(&jit->ctx, 0xff00)));
					zend_jit_if_true(jit, if_refcounted);
				}

				if (addref == 2) {
					zend_jit_gc_addref2(jit, ref);
				} else {
					zend_jit_gc_addref(jit, ref);
				}

				if (src_info & (MAY_BE_ANY-(MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
					ir_ref ref = zend_jit_end(jit);
					zend_jit_if_false(jit, if_refcounted);
					zend_jit_merge_2(jit, ref, zend_jit_end(jit));
				}
			}
		}
	}
}

static void zend_jit_zval_dtor(zend_jit_ctx *jit, ir_ref ref, uint32_t op_info, const zend_op *opline)
{
	if (!((op_info) & MAY_BE_GUARD)
	 && has_concrete_type((op_info) & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
		zend_uchar type = concrete_type((op_info) & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE));
		if (type == IS_STRING && !ZEND_DEBUG) {
				zend_jit_call_1(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)_efree, IR_CONST_FASTCALL_FUNC),
					ref);
				return;
		} else if (type == IS_ARRAY) {
			if ((op_info) & (MAY_BE_ARRAY_KEY_STRING|MAY_BE_ARRAY_OF_STRING|MAY_BE_ARRAY_OF_ARRAY|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_REF)) {
				if (opline && ((op_info) & (MAY_BE_ARRAY_OF_ARRAY|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_REF))) {
					zend_jit_set_ex_opline(jit, opline);
				}
				zend_jit_call_1(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_array_destroy, IR_CONST_FASTCALL_FUNC),
					ref);
			} else {
				zend_jit_call_1(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_array_free, IR_CONST_FASTCALL_FUNC),
					ref);
			}
			return;
		} else if (type == IS_OBJECT) {
			if (opline) {
				zend_jit_set_ex_opline(jit, opline);
			}
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_objects_store_del, IR_CONST_FASTCALL_FUNC),
				ref);
			return;
		}
	}
	if (opline) {
		zend_jit_set_ex_opline(jit, opline);
	}
	zend_jit_call_1(jit, IR_VOID,
		zend_jit_const_func_addr(jit, (uintptr_t)rc_dtor_func, IR_CONST_FASTCALL_FUNC),
		ref);
}

static void zend_jit_zval_ptr_dtor(zend_jit_ctx  *jit,
                                   zend_jit_addr  addr,
                                   uint32_t       op_info,
                                   bool           gc,
                                   const zend_op *opline)
{
    ir_ref ref, ref2;
	ir_ref if_refcounted = IR_UNUSED;
	ir_ref if_not_zero = IR_UNUSED;
	ir_ref end_inputs[5];
	uint32_t end_inputs_count = 0;

	if (op_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF|MAY_BE_GUARD)) {
		if ((op_info) & ((MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_INDIRECT|MAY_BE_GUARD)-(MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
			if_refcounted = zend_jit_if_zval_refcounted(jit, addr);
			zend_jit_if_false(jit, if_refcounted);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_if_true(jit, if_refcounted);
		}
		ref = zend_jit_zval_ptr(jit, addr);
		ref2 = zend_jit_gc_delref(jit, ref);

		if (((op_info) & MAY_BE_GUARD) || RC_MAY_BE_1(op_info)) {
			if (((op_info) & MAY_BE_GUARD) || RC_MAY_BE_N(op_info)) {
				if_not_zero = zend_jit_if(jit, ref2);
				zend_jit_if_false(jit, if_not_zero);
			}
			// zval_dtor_func(r);
			zend_jit_zval_dtor(jit, ref, op_info, opline);
			if (if_not_zero) {
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_not_zero);
			}
		}
		if (gc && (((op_info) & MAY_BE_GUARD) || (RC_MAY_BE_N(op_info) && ((op_info) & (MAY_BE_REF|MAY_BE_ARRAY|MAY_BE_OBJECT)) != 0))) {
			ir_ref if_may_not_leak;

			if ((op_info) & (MAY_BE_REF|MAY_BE_GUARD)) {
				ir_ref if_ref, ref_path, if_collectable;

				if_ref = zend_jit_if_zval_type(jit, addr, IS_REFERENCE);
				zend_jit_if_true(jit, if_ref);


				ref2 = zend_jit_addr_offset(jit, ref, offsetof(zend_reference, val));

				if_collectable = zend_jit_if_zval_ref_collectable(jit, ref2);
				zend_jit_if_false(jit, if_collectable);
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_collectable);

				ref2 = zend_jit_zval_ref_ptr(jit, ref2);

				ref_path = zend_jit_end(jit);
				zend_jit_if_false(jit, if_ref);
				zend_jit_merge_2(jit, zend_jit_end(jit), ref_path);
				ref = zend_jit_phi_2(jit, ref, ref2);
			}

			if_may_not_leak = zend_jit_if_gc_may_not_leak(jit, ref);
			zend_jit_if_true(jit, if_may_not_leak);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_if_false(jit, if_may_not_leak);

			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)gc_possible_root, IR_CONST_FASTCALL_FUNC),
				ref);
		}

		if (end_inputs_count) {
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_merge_N(jit, end_inputs_count, end_inputs);
		}
	}
}

static void zend_jit_free_op(zend_jit_ctx  *jit,
                             zend_uchar     op_type,
                             znode_op       op,
                             uint32_t       op_info,
                             const zend_op *opline)
{
	if (op_type & (IS_VAR|IS_TMP_VAR)) {
		zend_jit_zval_ptr_dtor(jit,
			ZEND_ADDR_MEM_ZVAL(ZREG_FP, op.var),
			op_info, 0, opline);
	}
}

static void zend_jit_obj_release(zend_jit_ctx  *jit, ir_ref ref)
{
    ir_ref ref2, if_not_zero, if_may_not_leak;
	ir_ref end_inputs[3];
	uint32_t end_inputs_count = 0;

	// JIT: if (GC_DELREF(obj) == 0) {
	ref2 = zend_jit_gc_delref(jit, ref);
	if_not_zero = zend_jit_if(jit, ref2);
	zend_jit_if_false(jit, if_not_zero);

	// JIT: zend_objects_store_del(obj)
	zend_jit_call_1(jit, IR_VOID,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_objects_store_del, IR_CONST_FASTCALL_FUNC),
		ref);

	zend_jit_array_add(end_inputs, zend_jit_end(jit));
	zend_jit_if_true(jit, if_not_zero);

	if_may_not_leak = zend_jit_if_gc_may_not_leak(jit, ref);
	zend_jit_if_true(jit, if_may_not_leak);
	zend_jit_array_add(end_inputs, zend_jit_end(jit));
	zend_jit_if_false(jit, if_may_not_leak);

	zend_jit_call_1(jit, IR_VOID,
		zend_jit_const_func_addr(jit, (uintptr_t)gc_possible_root, IR_CONST_FASTCALL_FUNC),
		ref);

	zend_jit_array_add(end_inputs, zend_jit_end(jit));
	zend_jit_merge_N(jit, end_inputs_count, end_inputs);
}

static ir_ref zend_jit_uninitialized_zval_addr(zend_jit_ctx *jit)
{
	return ZEND_JIT_EG_ADDR(uninitialized_zval);
}

static int zend_jit_check_timeout(zend_jit_ctx *jit, const zend_op *opline, const void *exit_addr)
{
	ir_ref ref;

	ref = ZEND_JIT_EG_ADDR(vm_interrupt);
	ref = zend_jit_load(jit, IR_U8, ref);
	if (exit_addr) {
#if 1
		/* use IF instead of GUARD to allow jmp reordering */
		ref = zend_jit_if(jit, ref);
		zend_jit_if_true_cold(jit, ref);
		zend_jit_side_exit(jit,
			zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		zend_jit_if_false(jit, ref);
#else
		zend_jit_guard_not(jit, ref,
			zend_jit_const_addr(jit, (uintptr_t)exit_addr));
#endif
	} else if (!opline || jit->last_valid_opline == opline) {
		zend_jit_guard_not(jit, ref,
			zend_jit_stub_addr(jit, jit_stub_interrupt_handler));
	} else {
		ir_ref if_timeout;

		if_timeout = zend_jit_if(jit, ref);
		zend_jit_if_true_cold(jit, if_timeout);
		zend_jit_load_ip_addr(jit, opline);
		zend_jit_ijmp(jit,
			zend_jit_stub_addr(jit, jit_stub_interrupt_handler));
		zend_jit_if_false(jit, if_timeout);
	}
	return 1;
}

static void zend_jit_check_exception(zend_jit_ctx *jit)
{
	ir_ref ref;

	ref = zend_jit_eg_exception_addr(jit);
	ref = zend_jit_load(jit, IR_ADDR, ref);
	zend_jit_guard_not(jit, ref,
		zend_jit_stub_addr(jit, jit_stub_exception_handler));
}

static void zend_jit_check_exception_undef_result(zend_jit_ctx *jit, const zend_op *opline)
{
	ir_ref ref;

	ref = zend_jit_eg_exception_addr(jit);
	ref = zend_jit_load(jit, IR_ADDR, ref);
	zend_jit_guard_not(jit, ref,
		zend_jit_stub_addr(jit,
			(opline->result_type & (IS_TMP_VAR|IS_VAR)) ? jit_stub_exception_handler_undef : jit_stub_exception_handler));
}

static void zend_jit_type_check_undef(zend_jit_ctx  *jit,
                                      ir_ref         type,
                                      uint32_t       var,
                                      const zend_op *opline,
                                      bool           check_exception)
{
	ir_ref if_def, err_path;

	if_def = zend_jit_if(jit, type);
	zend_jit_if_false_cold(jit, if_def);

	if (opline) {
		zend_jit_set_ex_opline(jit, opline);
	}

	zend_jit_call_1(jit, IR_VOID,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_undefined_op_helper, IR_CONST_FASTCALL_FUNC),
		ir_const_u32(&jit->ctx, var));

	if (check_exception) {
		zend_jit_check_exception(jit);
	}

	err_path = zend_jit_end(jit);

	zend_jit_if_true(jit, if_def);
	zend_jit_merge_2(jit, err_path, zend_jit_end(jit));
}

static ir_ref zend_jit_zval_check_undef(zend_jit_ctx  *jit,
                                        ir_ref         ref,
                                        uint32_t       var,
                                        const zend_op *opline,
                                        bool           check_exception)
{
	ir_ref if_undef, err_path, ref2;

	if_undef = zend_jit_if_zval_ref_type(jit, ref, ir_const_u8(&jit->ctx, IS_UNDEF));
	zend_jit_if_true_cold(jit, if_undef);

	if (opline) {
		zend_jit_set_ex_opline(jit, opline);
	}

	zend_jit_call_1(jit, IR_VOID,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_undefined_op_helper, IR_CONST_FASTCALL_FUNC),
		ir_const_u32(&jit->ctx, var));

	if (check_exception) {
		zend_jit_check_exception(jit);
	}

	ref2 = zend_jit_uninitialized_zval_addr(jit);
	err_path = zend_jit_end(jit);

	zend_jit_if_false(jit, if_undef);
	zend_jit_merge_2(jit, err_path, zend_jit_end(jit));

	return zend_jit_phi_2(jit, ref2, ref);
}

/* stubs */

static int zend_jit_exception_handler_stub(zend_jit_ctx *jit)
{
	const void *handler;

	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		handler = zend_get_opcode_handler_func(EG(exception_op));

		zend_jit_call_0(jit, IR_VOID, zend_jit_const_func_addr(jit, (uintptr_t)handler, 0));
		zend_jit_tailcall_0(jit,
			zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));
	} else {
		handler = EG(exception_op)->handler;

		if (GCC_GLOBAL_REGS) {
			zend_jit_tailcall_0(jit, zend_jit_const_func_addr(jit, (uintptr_t)handler, 0));
		} else {
			ir_ref ref, if_negative, negative_path;

			ref = zend_jit_call_1(jit, IR_I32,
				zend_jit_const_func_addr(jit, (uintptr_t)handler, IR_CONST_FASTCALL_FUNC),
				zend_jit_fp(jit));

			if_negative = zend_jit_if(jit,
				ir_fold2(&jit->ctx, IR_OPT(IR_LT, IR_BOOL),
					ref,
					ir_const_i32(&jit->ctx, 0)));
			zend_jit_if_true(jit, if_negative);
			negative_path = zend_jit_end(jit);
			zend_jit_if_false(jit, if_negative);
			zend_jit_merge_2(jit, negative_path, zend_jit_end(jit));
			ref = zend_jit_phi_2(jit,
				ref,
				ir_const_i32(&jit->ctx, 1));

			zend_jit_ret(jit, ref);
		}
	}
	return 1;
}

static int zend_jit_exception_handler_undef_stub(zend_jit_ctx *jit)
{
	ir_ref ref, result_type, if_result_used;

	ref = ZEND_JIT_EG_ADDR(opline_before_exception);
	result_type = zend_jit_load_at_offset(jit, IR_U8,
		zend_jit_load(jit, IR_ADDR, ref), offsetof(zend_op, result_type));

	if_result_used = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U8),
			result_type,
			ir_const_u8(&jit->ctx, IS_TMP_VAR|IS_VAR)));
	zend_jit_if_true(jit, if_result_used);

	ref = zend_jit_load_at_offset(jit, IR_U32,
		zend_jit_load(jit, IR_ADDR, ref), offsetof(zend_op, result.var));
	if (sizeof(void*) == 8) {
		ref = ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_ADDR), ref);
	}

	zend_jit_store_at_offset(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
			zend_jit_fp(jit),
			ref),
		offsetof(zval, u1.type_info),
		ir_const_u32(&jit->ctx, IS_UNDEF));

	ref = zend_jit_end(jit);

	zend_jit_if_false(jit, if_result_used);
	zend_jit_merge_2(jit, ref, zend_jit_end(jit));

	zend_jit_ijmp(jit,
		zend_jit_stub_addr(jit, jit_stub_exception_handler));

	return 1;
}

static int zend_jit_exception_handler_free_op1_op2_stub(zend_jit_ctx *jit)
{
	ir_ref ref, if_dtor, dtor_path;
	zend_jit_addr var_addr;

	ref = zend_jit_load(jit, IR_ADDR, ZEND_JIT_EG_ADDR(opline_before_exception));
	if_dtor = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U8),
			zend_jit_load_at_offset(jit, IR_U8,
				ref, offsetof(zend_op, op1_type)),
			ir_const_u8(&jit->ctx, IS_TMP_VAR|IS_VAR)));
	zend_jit_if_true(jit, if_dtor);
	ref = zend_jit_load_at_offset(jit, IR_U32,
		ref, offsetof(zend_op, op1.var));
	if (sizeof(void*) == 8) {
		ref = ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_ADDR), ref);
	}
	ref = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
		zend_jit_fp(jit),
		ref);
	var_addr = ZEND_ADDR_REF_ZVAL(ref);
	zend_jit_zval_ptr_dtor(jit, var_addr, MAY_BE_ANY|MAY_BE_RC1|MAY_BE_RCN|MAY_BE_REF, 0, NULL);

	dtor_path = zend_jit_end(jit);
	zend_jit_if_false(jit, if_dtor);
	zend_jit_merge_2(jit, dtor_path, zend_jit_end(jit));

	zend_jit_ijmp(jit,
		zend_jit_stub_addr(jit, jit_stub_exception_handler_free_op2));

	return 1;
}

static int zend_jit_exception_handler_free_op2_stub(zend_jit_ctx *jit)
{
	ir_ref ref, if_dtor, dtor_path;
	zend_jit_addr var_addr;

	ref = zend_jit_load(jit, IR_ADDR, ZEND_JIT_EG_ADDR(opline_before_exception));
	if_dtor = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U8),
			zend_jit_load_at_offset(jit, IR_U8,
				ref, offsetof(zend_op, op2_type)),
			ir_const_u8(&jit->ctx, IS_TMP_VAR|IS_VAR)));
	zend_jit_if_true(jit, if_dtor);
	ref = zend_jit_load_at_offset(jit, IR_U32,
		ref, offsetof(zend_op, op2.var));
	if (sizeof(void*) == 8) {
		ref = ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_ADDR), ref);
	}
	ref = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
		zend_jit_fp(jit),
		ref);
	var_addr = ZEND_ADDR_REF_ZVAL(ref);
	zend_jit_zval_ptr_dtor(jit, var_addr, MAY_BE_ANY|MAY_BE_RC1|MAY_BE_RCN|MAY_BE_REF, 0, NULL);

	dtor_path = zend_jit_end(jit);
	zend_jit_if_false(jit, if_dtor);
	zend_jit_merge_2(jit, dtor_path, zend_jit_end(jit));

	zend_jit_ijmp(jit,
		zend_jit_stub_addr(jit, jit_stub_exception_handler_undef));

	return 1;
}

static int zend_jit_interrupt_handler_stub(zend_jit_ctx *jit)
{
	ir_ref ref, if_timeout, timeout_path, if_exception, exception_path;

	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		// EX(opline) = opline
		zend_jit_store(jit,
			zend_jit_ex_opline_addr(jit),
			zend_jit_ip(jit));
	}

	zend_jit_store(jit,
		ZEND_JIT_EG_ADDR(vm_interrupt),
		ir_const_u8(&jit->ctx, 0));
	ref = zend_jit_load(jit, IR_U8,
		ZEND_JIT_EG_ADDR(timed_out));
	if_timeout = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL), ref, ir_const_u8(&jit->ctx, 0)));
	zend_jit_if_false(jit, if_timeout);
	zend_jit_call_0(jit, IR_VOID, zend_jit_const_func_addr(jit, (uintptr_t)zend_timeout, 0));
	timeout_path = zend_jit_end(jit);
	zend_jit_if_true(jit, if_timeout);
	zend_jit_merge_2(jit, timeout_path, zend_jit_end(jit));

	if (zend_interrupt_function) {
		zend_jit_call_1(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_interrupt_function, 0),
			zend_jit_fp(jit));
		ref = zend_jit_load(jit, IR_ADDR, ZEND_JIT_EG_ADDR(exception));
		if_exception = zend_jit_if(jit, ref);
		zend_jit_if_true(jit, if_exception);
		zend_jit_call_0(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_exception_in_interrupt_handler_helper, 0));
		exception_path = zend_jit_end(jit);
		zend_jit_if_false(jit, if_exception);
		zend_jit_merge_2(jit, exception_path, zend_jit_end(jit));

		ref = ZEND_JIT_EG_ADDR(current_execute_data);
		zend_jit_store_fp(jit, zend_jit_load(jit, IR_ADDR, ref));
		ref = zend_jit_load(jit, IR_ADDR,
			zend_jit_ex_opline_addr(jit));
		zend_jit_store_ip(jit, ref);
	}

	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		zend_jit_tailcall_0(jit,
			zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));
	} else if (GCC_GLOBAL_REGS) {
		zend_jit_tailcall_0(jit,
			zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));
	} else {
		zend_jit_ret(jit, ir_const_i32(&jit->ctx, 1));
	}
	return 1;
}

static int zend_jit_leave_function_handler_stub(zend_jit_ctx *jit)
{
	ir_ref call_info = zend_jit_load_at_offset(jit, IR_U32,
		zend_jit_fp(jit), offsetof(zend_execute_data, This.u1.type_info));

	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		ir_ref if_top = zend_jit_if(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
				call_info,
				ir_const_u32(&jit->ctx, ZEND_CALL_TOP)));

		zend_jit_if_false(jit, if_top);
		zend_jit_call_1(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_leave_nested_func_helper, IR_CONST_FASTCALL_FUNC),
			call_info);
		zend_jit_store_ip(jit,
			zend_jit_load(jit, IR_ADDR,
				zend_jit_ex_opline_addr(jit)));
		zend_jit_tailcall_0(jit,
			zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));

		zend_jit_if_true(jit, if_top);
		zend_jit_call_1(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_leave_top_func_helper, IR_CONST_FASTCALL_FUNC),
			call_info);
		zend_jit_tailcall_0(jit,
			zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));
	} else {
ZEND_ASSERT(0);
		if (GCC_GLOBAL_REGS) {
//???			|	add r4, SPAD
		} else {
//???			|	mov FCARG2a, FP
//???			|	mov FP, aword T2 // restore FP
//???			|	mov RX, aword T3 // restore IP
//???			|	add r4, NR_SPAD
		}
//???		|	test FCARG1d, ZEND_CALL_TOP
//???		|	jnz >1
//???		|	EXT_JMP zend_jit_leave_nested_func_helper, r0
//???		|1:
//???		|	EXT_JMP zend_jit_leave_top_func_helper, r0
	}

	return 1;
}

static int zend_jit_negative_shift_stub(zend_jit_ctx *jit)
{
	zend_jit_call_2(jit, IR_VOID,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_throw_error, 0),
		zend_jit_const_addr(jit, (uintptr_t)zend_ce_arithmetic_error),
		zend_jit_const_addr(jit, (uintptr_t)"Bit shift by negative number"));
	zend_jit_ijmp(jit,
		zend_jit_stub_addr(jit, jit_stub_exception_handler_free_op1_op2));
	return 1;
}

static int zend_jit_mod_by_zero_stub(zend_jit_ctx *jit)
{
	zend_jit_call_2(jit, IR_VOID,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_throw_error, 0),
		zend_jit_const_addr(jit, (uintptr_t)zend_ce_division_by_zero_error),
		zend_jit_const_addr(jit, (uintptr_t)"Modulo by zero"));
	zend_jit_ijmp(jit,
		zend_jit_stub_addr(jit, jit_stub_exception_handler_free_op1_op2));
	return 1;
}

static int zend_jit_invalid_this_stub(zend_jit_ctx *jit)
{
	zend_jit_call_2(jit, IR_VOID,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_throw_error, 0),
		IR_NULL,
		zend_jit_const_addr(jit, (uintptr_t)"Using $this when not in object context"));
	zend_jit_ijmp(jit,
		zend_jit_stub_addr(jit, jit_stub_exception_handler_undef));
	return 1;
}

static int zend_jit_undefined_function_stub(zend_jit_ctx *jit)
{
	// JIT: load EX(opline)
	ir_ref ref = zend_jit_load(jit, IR_ADDR, zend_jit_fp(jit));
	ir_ref arg3 = zend_jit_load_at_offset(jit, IR_U32,
		ref, offsetof(zend_op, op2.constant));

	if (sizeof(void*) == 8) {
		arg3 = zend_jit_load(jit, IR_ADDR,
			ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
				ref,
				ir_fold1(&jit->ctx, IR_OPT(IR_SEXT, IR_ADDR), arg3)));
	} else {
		arg3 = zend_jit_load(jit, IR_ADDR, arg3);
	}
	arg3 = zend_jit_addr_offset(jit, arg3, offsetof(zend_string, val));

	zend_jit_call_3(jit, IR_VOID,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_throw_error, 0),
		IR_NULL,
		zend_jit_const_addr(jit, (uintptr_t)"Call to undefined function %s()"),
		arg3);

	zend_jit_ijmp(jit,
		zend_jit_stub_addr(jit, jit_stub_exception_handler));

	return 1;
}

static int zend_jit_throw_cannot_pass_by_ref_stub(zend_jit_ctx *jit)
{
	ir_ref opline, ref, rx, if_eq, if_tmp;

	// JIT: opline = EX(opline)
	opline = zend_jit_load(jit, IR_ADDR, zend_jit_fp(jit));

	// JIT: ZVAL_UNDEF(ZEND_CALL_VAR(RX, opline->result.var))
	ref = zend_jit_load_at_offset(jit, IR_U32,
		opline, offsetof(zend_op, result.var));
	if (sizeof(void*) == 8) {
		ref = ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_ADDR), ref);
	}
	rx = zend_jit_ip(jit);
	zend_jit_zval_ref_set_type_info(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR), rx, ref),
		ir_const_u32(&jit->ctx, IS_UNDEF));

	// last EX(call) frame may be delayed
	// JIT: if (EX(call) == RX)
	ref = zend_jit_load_at_offset(jit, IR_ADDR,
		zend_jit_fp(jit), offsetof(zend_execute_data, call));
	if_eq = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL), rx, ref));
	zend_jit_if_false(jit, if_eq);

	// JIT: RX->prev_execute_data == EX(call)
	zend_jit_store_at_offset(jit,
		rx, offsetof(zend_execute_data, prev_execute_data),
		ref);

	// JIT: EX(call) = RX
	zend_jit_store_at_offset(jit,
		zend_jit_fp(jit), offsetof(zend_execute_data, call),
		rx);

	ref = zend_jit_end(jit);
	zend_jit_if_true(jit, if_eq);
	zend_jit_merge_2(jit, ref, zend_jit_end(jit));

	// JIT: IP = opline
	zend_jit_store_ip(jit, opline);

	// JIT: zend_cannot_pass_by_reference(opline->op2.num)
	zend_jit_call_1(jit, IR_VOID,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_cannot_pass_by_reference, IR_CONST_FASTCALL_FUNC),
		zend_jit_load_at_offset(jit, IR_U32,
			opline, offsetof(zend_op, op2.num)));

	// JIT: if (IP->op1_type == IS_TMP_VAR)
	ref = zend_jit_load_at_offset(jit, IR_U8,
		zend_jit_ip(jit), offsetof(zend_op, op1_type));
	if_tmp = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
			ref,
			ir_const_u8(&jit->ctx, IS_TMP_VAR)));
	zend_jit_if_true(jit, if_tmp);

	// JIT: zval_ptr_dtor(EX_VAR(IP->op1.var))
	ref = zend_jit_load_at_offset(jit, IR_U32,
		zend_jit_ip(jit), offsetof(zend_op, op1.var));
	if (sizeof(void*) == 8) {
		ref = ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_ADDR), ref);
	}
	ref = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR), zend_jit_fp(jit), ref);
	zend_jit_zval_ptr_dtor(jit,
		ZEND_ADDR_REF_ZVAL(ref),
		MAY_BE_ANY|MAY_BE_RC1|MAY_BE_RCN|MAY_BE_REF, 0, NULL);

	ref = zend_jit_end(jit);
	zend_jit_if_false(jit, if_tmp);
	zend_jit_merge_2(jit, ref, zend_jit_end(jit));

	zend_jit_ijmp(jit,
		zend_jit_stub_addr(jit, jit_stub_exception_handler));

	return 1;
}

static int zend_jit_icall_throw_stub(zend_jit_ctx *jit)
{
	ir_ref ip, cond, if_set, long_path;

	// JIT: zend_rethrow_exception(zend_execute_data *execute_data)
	// JIT: if (EX(opline)->opcode != ZEND_HANDLE_EXCEPTION) {
	zend_jit_store_ip(jit,
		zend_jit_load(jit, IR_ADDR,
			zend_jit_ex_opline_addr(jit)));
	ip = zend_jit_ip(jit);
	cond = ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
		zend_jit_load_at_offset(jit, IR_U8,
			ip, offsetof(zend_op, opcode)),
		ir_const_u8(&jit->ctx, ZEND_HANDLE_EXCEPTION));
	if_set = zend_jit_if(jit, cond);
	zend_jit_if_false(jit, if_set);

	// JIT: EG(opline_before_exception) = opline;
	zend_jit_store(jit,
		ZEND_JIT_EG_ADDR(opline_before_exception),
		ip);

	long_path = zend_jit_end(jit);
	zend_jit_if_true(jit, if_set);
	zend_jit_merge_2(jit, long_path, zend_jit_end(jit));

	// JIT: opline = EG(exception_op);
	zend_jit_store_ip(jit,
		ZEND_JIT_EG_ADDR(exception_op));

	if (GCC_GLOBAL_REGS) {
		ip = zend_jit_ip(jit);
		zend_jit_store(jit,
			zend_jit_ex_opline_addr(jit),
			ip);
	}

	zend_jit_ijmp(jit,
		zend_jit_stub_addr(jit, jit_stub_exception_handler));

	return 1;
}

static int zend_jit_leave_throw_stub(zend_jit_ctx *jit)
{
	ir_ref ip, cond, if_set, long_path;

	// JIT: if (opline->opcode != ZEND_HANDLE_EXCEPTION) {
	if (GCC_GLOBAL_REGS) {
		zend_jit_store_ip(jit,
			zend_jit_load(jit, IR_ADDR,
				zend_jit_ex_opline_addr(jit)));
		ip = zend_jit_ip(jit);
		cond = ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
			zend_jit_load_at_offset(jit, IR_U8,
				ip, offsetof(zend_op, opcode)),
			ir_const_u8(&jit->ctx, ZEND_HANDLE_EXCEPTION));
		if_set = zend_jit_if(jit, cond);
		zend_jit_if_false(jit, if_set);

		// JIT: EG(opline_before_exception) = opline;
		zend_jit_store(jit,
			ZEND_JIT_EG_ADDR(opline_before_exception),
			ip);

		long_path = zend_jit_end(jit);
		zend_jit_if_true(jit, if_set);
		zend_jit_merge_2(jit, long_path, zend_jit_end(jit));

		// JIT: opline = EG(exception_op);
		zend_jit_store_ip(jit,
			ZEND_JIT_EG_ADDR(exception_op));

		if (GCC_GLOBAL_REGS) {
			ip = zend_jit_ip(jit);
			zend_jit_store(jit,
				zend_jit_ex_opline_addr(jit),
				ip);
		}

		// JIT: HANDLE_EXCEPTION()
		zend_jit_ijmp(jit,
			zend_jit_stub_addr(jit, jit_stub_exception_handler));
	} else {
ZEND_ASSERT(0);
//???		|	GET_IP FCARG1a
//???		|	cmp byte OP:FCARG1a->opcode, ZEND_HANDLE_EXCEPTION
//???		|	je >5
//???		|	// EG(opline_before_exception) = opline;
//???		|	MEM_STORE_ZTS aword, executor_globals, opline_before_exception, FCARG1a, r0
//???		|5:
//???		|	// opline = EG(exception_op);
//???		|	LOAD_IP_ADDR_ZTS executor_globals, exception_op
//???		|	mov FP, aword T2 // restore FP
//???		|	mov RX, aword T3 // restore IP
//???		|	add r4, NR_SPAD // stack alignment
//???		|	mov r0, 2 // ZEND_VM_LEAVE
//???		|	ret
	}

	return 1;
}

static int zend_jit_hybrid_runtime_jit_stub(zend_jit_ctx *jit)
{
	if (zend_jit_vm_kind != ZEND_VM_KIND_HYBRID) {
		return 0;
	}

	zend_jit_call_0(jit, IR_VOID,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_runtime_jit, IR_CONST_FASTCALL_FUNC));
	zend_jit_ijmp(jit,
		zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));
	return 1;
}

static int zend_jit_hybrid_profile_jit_stub(zend_jit_ctx *jit)
{
	ir_ref addr, func, run_time_cache, jit_extension;

	if (zend_jit_vm_kind != ZEND_VM_KIND_HYBRID) {
		return 0;
	}

	addr = zend_jit_const_addr(jit, (uintptr_t)&zend_jit_profile_counter),
	zend_jit_store(jit, addr,
		ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_PHP_LONG),
			zend_jit_load(jit, IR_PHP_LONG, addr),
			ir_const_php_long(&jit->ctx, 1)));

	func = zend_jit_load_at_offset(jit, IR_ADDR,
		zend_jit_fp(jit), offsetof(zend_execute_data, func));
	run_time_cache = zend_jit_load_at_offset(jit, IR_ADDR,
		zend_jit_fp(jit), offsetof(zend_execute_data, run_time_cache));
	jit_extension = zend_jit_load_at_offset(jit, IR_ADDR,
		func, offsetof(zend_op_array, reserved[zend_func_info_rid]));

	if (zend_jit_profile_counter_rid) {
		addr = zend_jit_addr_offset(jit, run_time_cache, zend_jit_profile_counter_rid * sizeof(void*));
	} else {
		addr = run_time_cache;
	}
	zend_jit_store(jit, addr,
		ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_PHP_LONG),
			zend_jit_load(jit, IR_PHP_LONG, addr),
			ir_const_php_long(&jit->ctx, 1)));

	addr = zend_jit_addr_offset(jit, jit_extension, offsetof(zend_jit_op_array_extension, orig_handler));
	zend_jit_ijmp(jit,
		zend_jit_load(jit, IR_ADDR, addr));

	return 1;
}

static int _zend_jit_hybrid_hot_counter_stub(zend_jit_ctx *jit, uint32_t cost)
{
	ir_ref func, jit_extension, addr, ref, if_overflow;

	func = zend_jit_load_at_offset(jit, IR_ADDR,
		zend_jit_fp(jit), offsetof(zend_execute_data, func));
	jit_extension = zend_jit_load_at_offset(jit, IR_ADDR,
		func, offsetof(zend_op_array, reserved[zend_func_info_rid]));
	addr = zend_jit_load_at_offset(jit, IR_ADDR,
		jit_extension, offsetof(zend_jit_op_array_hot_extension, counter));
	ref = ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_I16),
			zend_jit_load(jit, IR_I16, addr),
			ir_const_i16(&jit->ctx, cost));
	zend_jit_store(jit, addr, ref);
	if_overflow = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_LE, IR_BOOL), ref, ir_const_i16(&jit->ctx, 0)));

	zend_jit_if_true_cold(jit, if_overflow);
	zend_jit_store(jit, addr, ir_const_i16(&jit->ctx, ZEND_JIT_COUNTER_INIT));
	zend_jit_call_2(jit, IR_VOID,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_hot_func, IR_CONST_FASTCALL_FUNC),
		zend_jit_fp(jit),
		zend_jit_ip(jit));
	zend_jit_ijmp(jit,
		zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));

	zend_jit_if_false(jit, if_overflow);
	ref = ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_ADDR),
		zend_jit_ip(jit),
		zend_jit_load_at_offset(jit, IR_ADDR,
			func, offsetof(zend_op_array, opcodes)));
	ref = ir_fold2(&jit->ctx, IR_OPT(IR_DIV, IR_ADDR),
		ref,
		zend_jit_const_addr(jit, sizeof(zend_op) / sizeof(void*)));

	addr = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
		zend_jit_addr_offset(jit, jit_extension, offsetof(zend_jit_op_array_hot_extension, orig_handlers)),
		ref);
	zend_jit_ijmp(jit,
		zend_jit_load(jit, IR_ADDR, addr));

	return 1;
}

static int zend_jit_hybrid_func_hot_counter_stub(zend_jit_ctx *jit)
{
	if (zend_jit_vm_kind != ZEND_VM_KIND_HYBRID || !JIT_G(hot_func)) {
		return 0;
	}

	return _zend_jit_hybrid_hot_counter_stub(jit,
		((ZEND_JIT_COUNTER_INIT + JIT_G(hot_func) - 1) / JIT_G(hot_func)));
}

static int zend_jit_hybrid_loop_hot_counter_stub(zend_jit_ctx *jit)
{
	if (zend_jit_vm_kind != ZEND_VM_KIND_HYBRID || !JIT_G(hot_loop)) {
		return 0;
	}

	return _zend_jit_hybrid_hot_counter_stub(jit,
		((ZEND_JIT_COUNTER_INIT + JIT_G(hot_loop) - 1) / JIT_G(hot_loop)));
}

static ir_ref _zend_jit_orig_opline_handler(zend_jit_ctx *jit, ir_ref offset)
{
	ir_ref addr;

	if (GCC_GLOBAL_REGS) {
		addr = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
			offset,
			zend_jit_ip(jit));
	} else {
		addr = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
			offset,
			zend_jit_load(jit, IR_ADDR,
				zend_jit_ex_opline_addr(jit)));
	}

	return zend_jit_load(jit, IR_ADDR, addr);
}

static ir_ref zend_jit_orig_opline_handler(zend_jit_ctx *jit)
{
	ir_ref func, jit_extension, offset;

	func = zend_jit_load_at_offset(jit, IR_ADDR,
		zend_jit_fp(jit), offsetof(zend_execute_data, func));
	jit_extension = zend_jit_load_at_offset(jit, IR_ADDR,
		func, offsetof(zend_op_array, reserved[zend_func_info_rid]));
	offset = zend_jit_load_at_offset(jit, IR_ADDR,
		jit_extension, offsetof(zend_jit_op_array_trace_extension, offset));
	return _zend_jit_orig_opline_handler(jit, offset);
}

static int _zend_jit_hybrid_trace_counter_stub(zend_jit_ctx *jit, uint32_t cost)
{
	ir_ref func, jit_extension, offset, addr, ref, if_overflow, ret, if_halt;

	func = zend_jit_load_at_offset(jit, IR_ADDR,
		zend_jit_fp(jit), offsetof(zend_execute_data, func));
	jit_extension = zend_jit_load_at_offset(jit, IR_ADDR,
		func, offsetof(zend_op_array, reserved[zend_func_info_rid]));
	offset = zend_jit_load_at_offset(jit, IR_ADDR,
		jit_extension, offsetof(zend_jit_op_array_trace_extension, offset));
	addr = zend_jit_load_at_offset(jit, IR_ADDR,
		ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
			offset,
			zend_jit_ip(jit)),
		offsetof(zend_op_trace_info, counter));
	ref = ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_I16),
			zend_jit_load(jit, IR_I16, addr),
			ir_const_i16(&jit->ctx, cost));
	zend_jit_store(jit, addr, ref);
	if_overflow = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_LE, IR_BOOL), ref, ir_const_i16(&jit->ctx, 0)));

	zend_jit_if_true_cold(jit, if_overflow);
	zend_jit_store(jit, addr, ir_const_i16(&jit->ctx, ZEND_JIT_COUNTER_INIT));
	ret = zend_jit_call_2(jit, IR_I32,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_trace_hot_root, IR_CONST_FASTCALL_FUNC),
		zend_jit_fp(jit),
		zend_jit_ip(jit));
	if_halt = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_LT, IR_BOOL), ret, ir_const_i32(&jit->ctx, 0)));
	zend_jit_if_false(jit, if_halt);

	ref = ZEND_JIT_EG_ADDR(current_execute_data);
	zend_jit_store_fp(jit, zend_jit_load(jit, IR_ADDR, ref));
	ref = zend_jit_load(jit, IR_ADDR,
		zend_jit_ex_opline_addr(jit));
	zend_jit_store_ip(jit, ref);
	zend_jit_ijmp(jit,
		zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));

	zend_jit_if_false(jit, if_overflow);
	zend_jit_ijmp(jit,
		_zend_jit_orig_opline_handler(jit, offset));

	zend_jit_if_true(jit, if_halt);
	zend_jit_ijmp(jit,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_halt_op->handler, IR_CONST_FASTCALL_FUNC));

	return 1;
}

static int zend_jit_hybrid_func_trace_counter_stub(zend_jit_ctx *jit)
{
	if (zend_jit_vm_kind != ZEND_VM_KIND_HYBRID || !JIT_G(hot_func)) {
		return 0;
	}

	return _zend_jit_hybrid_trace_counter_stub(jit,
		((ZEND_JIT_COUNTER_INIT + JIT_G(hot_func) - 1) / JIT_G(hot_func)));
}

static int zend_jit_hybrid_ret_trace_counter_stub(zend_jit_ctx *jit)
{
	if (zend_jit_vm_kind != ZEND_VM_KIND_HYBRID || !JIT_G(hot_return)) {
		return 0;
	}

	return _zend_jit_hybrid_trace_counter_stub(jit,
		((ZEND_JIT_COUNTER_INIT + JIT_G(hot_return) - 1) / JIT_G(hot_return)));
}

static int zend_jit_hybrid_loop_trace_counter_stub(zend_jit_ctx *jit)
{
	if (zend_jit_vm_kind != ZEND_VM_KIND_HYBRID || !JIT_G(hot_loop)) {
		return 0;
	}

	return _zend_jit_hybrid_trace_counter_stub(jit,
		((ZEND_JIT_COUNTER_INIT + JIT_G(hot_loop) - 1) / JIT_G(hot_loop)));
}

static int zend_jit_trace_halt_stub(zend_jit_ctx *jit)
{
	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		zend_jit_tailcall_0(jit,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_halt_op->handler, IR_CONST_FASTCALL_FUNC));
	} else if (GCC_GLOBAL_REGS) {
		ZEND_ASSERT(0 && "NIY");
//???		|	add r4, SPAD // stack alignment
//???		|	xor IP, IP // PC must be zero
//???		|	ret
	} else {
		zend_jit_ret(jit, ir_const_i32(&jit->ctx, -1)); // ZEND_VM_RETURN
	}
	return 1;
}

static int zend_jit_trace_escape_stub(zend_jit_ctx *jit)
{
	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		zend_jit_tailcall_0(jit,
			zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));
	} else if (GCC_GLOBAL_REGS) {
		ZEND_ASSERT(0 && "NIY");
//???		|	add r4, SPAD // stack alignment
//???		|	JMP_IP
	} else {
		zend_jit_ret(jit, ir_const_i32(&jit->ctx, 1)); // ZEND_VM_ENTER
	}

	return 1;
}

static int zend_jit_trace_exit_stub(zend_jit_ctx *jit)
{
	ir_ref ref, ret, if_zero, addr;

	if (GCC_GLOBAL_REGS) {
		// EX(opline) = opline
		zend_jit_store(jit,
			zend_jit_ex_opline_addr(jit),
			zend_jit_ip(jit));
	}

	ret = jit->control = ir_emit2(&jit->ctx, IR_OPT(IR_EXITCALL, IR_I32), jit->control,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_trace_exit, IR_CONST_FASTCALL_FUNC));

	if_zero = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_I32), ret, ir_const_i32(&jit->ctx, 0)));

	zend_jit_if_true(jit, if_zero);

	if (GCC_GLOBAL_REGS) {
		ref = ZEND_JIT_EG_ADDR(current_execute_data);
		zend_jit_store_fp(jit, zend_jit_load(jit, IR_ADDR, ref));
		ref = zend_jit_load(jit, IR_ADDR,
			zend_jit_ex_opline_addr(jit));
		zend_jit_store_ip(jit, ref);
	}

	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		zend_jit_tailcall_0(jit,
			zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));
	} else if (GCC_GLOBAL_REGS) {
		ZEND_ASSERT(0 && "NIY");
//		|	add r4, SPAD // stack alignment
//		|	JMP_IP
	} else {
		zend_jit_ret(jit, ir_const_i32(&jit->ctx, 1)); // ZEND_VM_ENTER
	}

	zend_jit_if_false(jit, if_zero);

	zend_jit_guard(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_GE, IR_I32), ret, ir_const_i32(&jit->ctx, 0)),
		zend_jit_stub_addr(jit, jit_stub_trace_halt));

	ref = ZEND_JIT_EG_ADDR(current_execute_data);
	zend_jit_store_fp(jit, zend_jit_load(jit, IR_ADDR, ref));

	if (GCC_GLOBAL_REGS) {
		ref = zend_jit_load(jit, IR_ADDR,
			zend_jit_ex_opline_addr(jit));
		zend_jit_store_ip(jit, ref);
	}

	// check for interrupt (try to avoid this ???)
	if (!zend_jit_check_timeout(jit, NULL, NULL)) {
		return 0;
	}

	addr = zend_jit_orig_opline_handler(jit);
	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID || GCC_GLOBAL_REGS) {
		zend_jit_tailcall_0(jit, addr);
	} else {
#if defined(IR_TARGET_X86)
		addr =  ir_fold2(&jit->ctx, IR_OPT(IR_BITCAST, IR_ADDR),
			addr, IR_CONST_FASTCALL_FUNC), // Hack to use fastcall calling convention ???
#endif
		ref = zend_jit_call_1(jit, IR_I32, addr, zend_jit_fp(jit));
		zend_jit_guard(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_GE, IR_BOOL),
				ref, ir_const_i32(&jit->ctx, 0)),
				zend_jit_stub_addr(jit, jit_stub_trace_halt));
		zend_jit_ret(jit, ir_const_i32(&jit->ctx, 1)); // ZEND_VM_ENTER
	}

	return 1;
}

static int zend_jit_assign_const_stub(zend_jit_ctx *jit)
{
	ir_ref var = zend_jit_param(jit, IR_ADDR, "var", 1);
	ir_ref val = zend_jit_param(jit, IR_ADDR, "val", 2);

	zend_jit_addr var_addr = ZEND_ADDR_REF_ZVAL(var);
	zend_jit_addr val_addr = ZEND_ADDR_REF_ZVAL(val);
	uint32_t val_info = MAY_BE_ANY|MAY_BE_RC1|MAY_BE_RCN;

	if (!zend_jit_assign_to_variable(
			jit, NULL,
			var_addr, var_addr, -1, -1,
			IS_CONST, val_addr, val_info,
			0, 0, 0)) {
		return 0;
	}
	zend_jit_ret(jit, IR_UNUSED);
	return 1;
}

static int zend_jit_assign_tmp_stub(zend_jit_ctx *jit)
{
	ir_ref var = zend_jit_param(jit, IR_ADDR, "var", 1);
	ir_ref val = zend_jit_param(jit, IR_ADDR, "val", 2);

	zend_jit_addr var_addr = ZEND_ADDR_REF_ZVAL(var);
	zend_jit_addr val_addr = ZEND_ADDR_REF_ZVAL(val);
	uint32_t val_info = MAY_BE_ANY|MAY_BE_RC1|MAY_BE_RCN;

	if (!zend_jit_assign_to_variable(
			jit, NULL,
			var_addr, var_addr, -1, -1,
			IS_TMP_VAR, val_addr, val_info,
			0, 0, 0)) {
		return 0;
	}
	zend_jit_ret(jit, IR_UNUSED);
	return 1;
}

static int zend_jit_assign_var_stub(zend_jit_ctx *jit)
{
	ir_ref var = zend_jit_param(jit, IR_ADDR, "var", 1);
	ir_ref val = zend_jit_param(jit, IR_ADDR, "val", 2);

	zend_jit_addr var_addr = ZEND_ADDR_REF_ZVAL(var);
	zend_jit_addr val_addr = ZEND_ADDR_REF_ZVAL(val);
	uint32_t val_info = MAY_BE_ANY|MAY_BE_RC1|MAY_BE_RCN|MAY_BE_REF;

	if (!zend_jit_assign_to_variable(
			jit, NULL,
			var_addr, var_addr, -1, -1,
			IS_VAR, val_addr, val_info,
			0, 0, 0)) {
		return 0;
	}
	zend_jit_ret(jit, IR_UNUSED);
	return 1;
}

static int zend_jit_assign_cv_noref_stub(zend_jit_ctx *jit)
{
	ir_ref var = zend_jit_param(jit, IR_ADDR, "var", 1);
	ir_ref val = zend_jit_param(jit, IR_ADDR, "val", 2);

	zend_jit_addr var_addr = ZEND_ADDR_REF_ZVAL(var);
	zend_jit_addr val_addr = ZEND_ADDR_REF_ZVAL(val);
	uint32_t val_info = MAY_BE_ANY|MAY_BE_RC1|MAY_BE_RCN/*|MAY_BE_UNDEF*/;

	if (!zend_jit_assign_to_variable(
			jit, NULL,
			var_addr, var_addr, -1, -1,
			IS_CV, val_addr, val_info,
			0, 0, 0)) {
		return 0;
	}
	zend_jit_ret(jit, IR_UNUSED);
	return 1;
}

static int zend_jit_assign_cv_stub(zend_jit_ctx *jit)
{
	ir_ref var = zend_jit_param(jit, IR_ADDR, "var", 1);
	ir_ref val = zend_jit_param(jit, IR_ADDR, "val", 2);

	zend_jit_addr var_addr = ZEND_ADDR_REF_ZVAL(var);
	zend_jit_addr val_addr = ZEND_ADDR_REF_ZVAL(val);
	uint32_t val_info = MAY_BE_ANY|MAY_BE_RC1|MAY_BE_RCN|MAY_BE_REF/*|MAY_BE_UNDEF*/;

	if (!zend_jit_assign_to_variable(
			jit, NULL,
			var_addr, var_addr, -1, -1,
			IS_CV, val_addr, val_info,
			0, 0, 0)) {
		return 0;
	}
	zend_jit_ret(jit, IR_UNUSED);
	return 1;
}

static void zend_jit_init_ctx(zend_jit_ctx *jit, uint32_t flags)
{
	ir_init(&jit->ctx, 256, 1024);
#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
	if (JIT_G(opt_flags) & allowed_opt_flags & ZEND_JIT_CPU_AVX) {
		flags |= IR_AVX;
	}
#endif
#if defined (__CET__) && (__CET__ & 1) != 0
	flags |= IR_GEN_ENDBR;
#endif
	jit->ctx.flags |= flags | IR_OPT_FOLDING | IR_OPT_CFG | IR_OPT_CODEGEN | IR_HAS_CALLS;

	if (!(flags & IR_FUNCTION)) {
		jit->ctx.flags |= IR_NO_STACK_COMBINE;
		if (zend_jit_vm_kind == ZEND_VM_KIND_CALL) {
			jit->ctx.flags |= IR_FUNCTION;
			jit->ctx.fixed_stack_frame_size = sizeof(void*) * 63; /* TODO: reduce stack size ??? */
#if defined(IR_TARGET_X86)
			jit->ctx.fixed_save_regset = (1<<3) | (1<<5) | (1<<6) | (1<<7); /* all preserved registers ??? */
#elif defined(IR_TARGET_X64)
			jit->ctx.fixed_save_regset = (1<<3) | (1<<5) | (1<<12) | (1<<13) | (1<<14) | (1<<15); /* all preserved registers ??? */
#elif defined(IR_TARGET_AARCH64)
			jit->ctx.fixed_save_regset = (1<<19) | (1<<20) | (1<<21) | (1<<22) | (1<<23) | (1<<24)
				| (1<<25) | (1<<26) | (1<<27) | (1<<27) | (1<<28) | (1<<29) | (1<<30); /* all preserved registers ??? */
#else
			ZEND_ASSERT(0 && "NIY");
#endif
		} else {
#ifdef ZEND_VM_HYBRID_JIT_RED_ZONE_SIZE
			jit->ctx.fixed_stack_red_zone = ZEND_VM_HYBRID_JIT_RED_ZONE_SIZE;
			jit->ctx.fixed_stack_frame_size = 0;
#else
			jit->ctx.fixed_stack_red_zone = 0;
			jit->ctx.fixed_stack_frame_size = 0;
#endif
			if (!GCC_GLOBAL_REGS) {
				jit->ctx.fixed_save_regset = 1 << ZREG_FP;
			}
		}
	}
	jit->ctx.fixed_regset = IR_REGSET_PHP_FIXED;
	jit->op_array = NULL;
	jit->ssa = NULL;
	jit->name = NULL;
	jit->last_valid_opline = NULL;
	jit->use_last_valid_opline = 0;
	jit->track_last_valid_opline = 0;
	jit->reuse_ip = 0;
	jit->delayed_call_level = 0;
	delayed_call_chain = 0; // TODO: remove ???
	jit->b = -1;
	jit->control = ir_emit0(&jit->ctx, IR_START);
#ifdef ZTS
	jit->tls = IR_UNUSED;
#endif
	jit->fp = IR_UNUSED;
	jit->trace_loop_ref = IR_UNUSED;
	jit->return_inputs = IR_UNUSED;
	jit->op_array = NULL;
	jit->ssa = NULL;
	jit->bb_start_ref = NULL;
	jit->bb_predecessors = NULL;
	jit->bb_edges = NULL;
	jit->trace = NULL;
	jit->ra = NULL;
	jit->delay_var = -1;
	jit->delay_buf = NULL;
	jit->delay_count = 0;
	jit->eg_exception_addr = 0;
	zend_hash_init(&jit->addr_hash, 64, NULL, NULL, 0);
	memset(jit->stub_addr, 0, sizeof(jit->stub_addr));
}

static int zend_jit_free_ctx(zend_jit_ctx *jit)
{
	if (jit->name) {
		zend_string_release(jit->name);
	}
	zend_hash_destroy(&jit->addr_hash);
	ir_free(&jit->ctx);
	return 1;
}

static void *zend_jit_ir_compile(ir_ctx *ctx, size_t *size, const char *name)
{
	void *entry;

	if (JIT_G(debug) & ZEND_JIT_DEBUG_IR_SRC) {
		if (name) fprintf(stderr, "%s: ; after folding\n", name);
		ir_save(ctx, stderr);
	}

#if ZEND_DEBUG
	ir_check(ctx);
#endif

	ir_build_def_use_lists(ctx);

#if ZEND_DEBUG
	ir_check(ctx);
#endif

#if 1
	ir_sccp(ctx);
#endif

	if (JIT_G(debug) & ZEND_JIT_DEBUG_IR_AFTER_SCCP) {
		if (name) fprintf(stderr, "%s: ; after SCCP\n", name);
		ir_save(ctx, stderr);
	}

	ir_build_cfg(ctx);
#if 0
	ir_remove_unreachable_blocks(ctx);
#endif
	ir_build_dominators_tree(ctx);
	ir_find_loops(ctx);

	if (JIT_G(debug) & ZEND_JIT_DEBUG_IR_AFTER_SCCP) {
		if (JIT_G(debug) & ZEND_JIT_DEBUG_IR_CFG) {
			ir_dump_cfg(ctx, stderr);
		}
	}

	ir_gcm(ctx);
	ir_schedule(ctx);

	if (JIT_G(debug) & ZEND_JIT_DEBUG_IR_AFTER_SCHEDULE) {
		if (name) fprintf(stderr, "%s: ; after schedule\n", name);
		ir_save(ctx, stderr);
		if (JIT_G(debug) & ZEND_JIT_DEBUG_IR_CFG) {
			ir_dump_cfg(ctx, stderr);
		}
	}

	ir_match(ctx);
	ir_assign_virtual_registers(ctx);
	ir_compute_live_ranges(ctx);
	ir_coalesce(ctx);
	ir_reg_alloc(ctx);

	if (JIT_G(debug) & ZEND_JIT_DEBUG_IR_AFTER_REGS) {
		if (name) fprintf(stderr, "%s: ; after register allocation\n", name);
		ir_save(ctx, stderr);
		if (JIT_G(debug) & ZEND_JIT_DEBUG_IR_CFG) {
			ir_dump_cfg(ctx, stderr);
		}
		if (JIT_G(debug) & ZEND_JIT_DEBUG_IR_REGS) {
			ir_dump_live_ranges(ctx, stderr);
		}
	}

	ir_schedule_blocks(ctx);

	if (JIT_G(debug) & ZEND_JIT_DEBUG_IR_FINAL) {
		if (name) fprintf(stderr, "%s: ; final\n", name);
		ir_save(ctx, stderr);
		if (JIT_G(debug) & ZEND_JIT_DEBUG_IR_CFG) {
			ir_dump_cfg(ctx, stderr);
		}
		if (JIT_G(debug) & ZEND_JIT_DEBUG_IR_REGS) {
			ir_dump_live_ranges(ctx, stderr);
		}
	}

#if ZEND_DEBUG
	ir_check(ctx);
#endif

	ctx->code_buffer = *dasm_ptr;
	ctx->code_buffer_size = (char*)dasm_end - (char*)*dasm_ptr;

	entry = ir_emit_code(ctx, size);
	if (entry) {
		*dasm_ptr = (char*)entry + ZEND_MM_ALIGNED_SIZE_EX(*size, 16);
	}

	return entry;
}

static int zend_jit_setup_stubs(void)
{
	zend_jit_ctx jit;
	void *entry;
	size_t size;
	uint32_t i;

	for (i = 0; i < sizeof(zend_jit_stubs)/sizeof(zend_jit_stubs[0]); i++) {
		zend_jit_init_ctx(&jit, zend_jit_stubs[i].flags);

		if (!zend_jit_stubs[i].stub(&jit)) {
			zend_jit_free_ctx(&jit);
			zend_jit_stub_handlers[i] = NULL;
			continue;
		}

		entry = zend_jit_ir_compile(&jit.ctx, &size, zend_jit_stubs[i].name);
		if (!entry) {
			zend_jit_free_ctx(&jit);
			return 0;
		}

		zend_jit_stub_handlers[i] = entry;

		if (JIT_G(debug) & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_ASM_STUBS|ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF|ZEND_JIT_DEBUG_PERF_DUMP)) {
			if (JIT_G(debug) & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_ASM_STUBS)) {
				ir_disasm_add_symbol(zend_jit_stubs[i].name, (uintptr_t)entry, size);
			}
			if (JIT_G(debug) & ZEND_JIT_DEBUG_ASM_STUBS) {
				ir_disasm(zend_jit_stubs[i].name,
					entry, size, (JIT_G(debug) & ZEND_JIT_DEBUG_ASM_ADDR) != 0, &jit.ctx, stderr);
			}

			if (JIT_G(debug) & ZEND_JIT_DEBUG_GDB) {
//				ir_mem_unprotect(entry, size);
				ir_gdb_register(zend_jit_stubs[i].name, entry, size, 0, 0);
//				ir_mem_protect(entry, size);
			}

			if (JIT_G(debug) & (ZEND_JIT_DEBUG_PERF|ZEND_JIT_DEBUG_PERF_DUMP)) {
				ir_perf_map_register(zend_jit_stubs[i].name, entry, size);
				if (JIT_G(debug) & ZEND_JIT_DEBUG_PERF_DUMP) {
					ir_perf_jitdump_register(zend_jit_stubs[i].name, entry, size);
				}
			}
		}
		zend_jit_free_ctx(&jit);
	}
	return 1;
}

static void zend_jit_setup_disasm(void)
{
	ir_disasm_init();

	if (zend_vm_kind() == ZEND_VM_KIND_HYBRID) {
		zend_op opline;

		memset(&opline, 0, sizeof(opline));

		opline.opcode = ZEND_DO_UCALL;
		opline.result_type = IS_UNUSED;
		zend_vm_set_opcode_handler(&opline);
		ir_disasm_add_symbol("ZEND_DO_UCALL_SPEC_RETVAL_UNUSED_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_DO_UCALL;
		opline.result_type = IS_VAR;
		zend_vm_set_opcode_handler(&opline);
		ir_disasm_add_symbol("ZEND_DO_UCALL_SPEC_RETVAL_USED_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_DO_FCALL_BY_NAME;
		opline.result_type = IS_UNUSED;
		zend_vm_set_opcode_handler(&opline);
		ir_disasm_add_symbol("ZEND_DO_FCALL_BY_NAME_SPEC_RETVAL_UNUSED_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_DO_FCALL_BY_NAME;
		opline.result_type = IS_VAR;
		zend_vm_set_opcode_handler(&opline);
		ir_disasm_add_symbol("ZEND_DO_FCALL_BY_NAME_SPEC_RETVAL_USED_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_DO_FCALL;
		opline.result_type = IS_UNUSED;
		zend_vm_set_opcode_handler(&opline);
		ir_disasm_add_symbol("ZEND_DO_FCALL_SPEC_RETVAL_UNUSED_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_DO_FCALL;
		opline.result_type = IS_VAR;
		zend_vm_set_opcode_handler(&opline);
		ir_disasm_add_symbol("ZEND_DO_FCALL_SPEC_RETVAL_USED_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_RETURN;
		opline.op1_type = IS_CONST;
		zend_vm_set_opcode_handler(&opline);
		ir_disasm_add_symbol("ZEND_RETURN_SPEC_CONST_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_RETURN;
		opline.op1_type = IS_TMP_VAR;
		zend_vm_set_opcode_handler(&opline);
		ir_disasm_add_symbol("ZEND_RETURN_SPEC_TMP_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_RETURN;
		opline.op1_type = IS_VAR;
		zend_vm_set_opcode_handler(&opline);
		ir_disasm_add_symbol("ZEND_RETURN_SPEC_VAR_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		opline.opcode = ZEND_RETURN;
		opline.op1_type = IS_CV;
		zend_vm_set_opcode_handler(&opline);
		ir_disasm_add_symbol("ZEND_RETURN_SPEC_CV_LABEL", (uint64_t)(uintptr_t)opline.handler, sizeof(void*));

		ir_disasm_add_symbol("ZEND_HYBRID_HALT_LABEL", (uint64_t)(uintptr_t)zend_jit_halt_op->handler, sizeof(void*));

		ir_disasm_add_symbol("zend_jit_profile_counter",            (uint64_t)(uintptr_t)&zend_jit_profile_counter, sizeof(zend_jit_profile_counter));

		ir_disasm_add_symbol("zend_runtime_jit",                    (uint64_t)(uintptr_t)zend_runtime_jit, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_hot_func",                   (uint64_t)(uintptr_t)zend_jit_hot_func, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_trace_hot_root",             (uint64_t)(uintptr_t)zend_jit_trace_hot_root, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_trace_exit",                 (uint64_t)(uintptr_t)zend_jit_trace_exit, sizeof(void*));

		ir_disasm_add_symbol("zend_jit_array_free",                 (uint64_t)(uintptr_t)zend_jit_array_free, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_undefined_op_helper",        (uint64_t)(uintptr_t)zend_jit_undefined_op_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_pre_inc_typed_ref",          (uint64_t)(uintptr_t)zend_jit_pre_inc_typed_ref, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_pre_dec_typed_ref",          (uint64_t)(uintptr_t)zend_jit_pre_dec_typed_ref, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_post_inc_typed_ref",         (uint64_t)(uintptr_t)zend_jit_post_inc_typed_ref, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_post_dec_typed_ref",         (uint64_t)(uintptr_t)zend_jit_post_dec_typed_ref, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_pre_inc",                    (uint64_t)(uintptr_t)zend_jit_pre_inc, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_pre_dec",                    (uint64_t)(uintptr_t)zend_jit_pre_dec, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_add_arrays_helper",          (uint64_t)(uintptr_t)zend_jit_add_arrays_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fast_assign_concat_helper",  (uint64_t)(uintptr_t)zend_jit_fast_assign_concat_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fast_concat_helper",         (uint64_t)(uintptr_t)zend_jit_fast_concat_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fast_concat_tmp_helper",     (uint64_t)(uintptr_t)zend_jit_fast_concat_tmp_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_assign_op_to_typed_ref_tmp", (uint64_t)(uintptr_t)zend_jit_assign_op_to_typed_ref_tmp, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_assign_op_to_typed_ref",     (uint64_t)(uintptr_t)zend_jit_assign_op_to_typed_ref, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_assign_const_to_typed_ref",  (uint64_t)(uintptr_t)zend_jit_assign_const_to_typed_ref, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_assign_tmp_to_typed_ref",    (uint64_t)(uintptr_t)zend_jit_assign_tmp_to_typed_ref, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_assign_var_to_typed_ref",    (uint64_t)(uintptr_t)zend_jit_assign_var_to_typed_ref, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_assign_cv_to_typed_ref",     (uint64_t)(uintptr_t)zend_jit_assign_cv_to_typed_ref, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_check_constant",             (uint64_t)(uintptr_t)zend_jit_check_constant, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_get_constant",               (uint64_t)(uintptr_t)zend_jit_get_constant, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_int_extend_stack_helper",    (uint64_t)(uintptr_t)zend_jit_int_extend_stack_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_extend_stack_helper",        (uint64_t)(uintptr_t)zend_jit_extend_stack_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_init_func_run_time_cache_helper", (uint64_t)(uintptr_t)zend_jit_init_func_run_time_cache_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_find_func_helper",           (uint64_t)(uintptr_t)zend_jit_find_func_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_find_ns_func_helper",        (uint64_t)(uintptr_t)zend_jit_find_ns_func_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_unref_helper",               (uint64_t)(uintptr_t)zend_jit_unref_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_invalid_method_call",        (uint64_t)(uintptr_t)zend_jit_invalid_method_call, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_invalid_method_call_tmp",    (uint64_t)(uintptr_t)zend_jit_invalid_method_call_tmp, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_find_method_helper",         (uint64_t)(uintptr_t)zend_jit_find_method_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_find_method_tmp_helper",     (uint64_t)(uintptr_t)zend_jit_find_method_tmp_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_push_static_metod_call_frame", (uint64_t)(uintptr_t)zend_jit_push_static_metod_call_frame, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_push_static_metod_call_frame_tmp", (uint64_t)(uintptr_t)zend_jit_push_static_metod_call_frame_tmp, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_free_trampoline_helper",     (uint64_t)(uintptr_t)zend_jit_free_trampoline_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_verify_return_slow",         (uint64_t)(uintptr_t)zend_jit_verify_return_slow, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_deprecated_helper",          (uint64_t)(uintptr_t)zend_jit_deprecated_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_copy_extra_args_helper",     (uint64_t)(uintptr_t)zend_jit_copy_extra_args_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_vm_stack_free_args_helper",  (uint64_t)(uintptr_t)zend_jit_vm_stack_free_args_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_free_extra_named_params",        (uint64_t)(uintptr_t)zend_free_extra_named_params, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_free_call_frame",            (uint64_t)(uintptr_t)zend_jit_free_call_frame, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_exception_in_interrupt_handler_helper", (uint64_t)(uintptr_t)zend_jit_exception_in_interrupt_handler_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_verify_arg_slow",            (uint64_t)(uintptr_t)zend_jit_verify_arg_slow, sizeof(void*));
		ir_disasm_add_symbol("zend_missing_arg_error",              (uint64_t)(uintptr_t)zend_missing_arg_error, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_leave_func_helper",          (uint64_t)(uintptr_t)zend_jit_leave_func_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_leave_nested_func_helper",   (uint64_t)(uintptr_t)zend_jit_leave_nested_func_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_leave_top_func_helper",      (uint64_t)(uintptr_t)zend_jit_leave_top_func_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_global_helper",        (uint64_t)(uintptr_t)zend_jit_fetch_global_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_hash_index_lookup_rw_no_packed", (uint64_t)(uintptr_t)zend_jit_hash_index_lookup_rw_no_packed, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_hash_index_lookup_rw",       (uint64_t)(uintptr_t)zend_jit_hash_index_lookup_rw, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_hash_lookup_rw",             (uint64_t)(uintptr_t)zend_jit_hash_lookup_rw, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_symtable_find",              (uint64_t)(uintptr_t)zend_jit_symtable_find, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_symtable_lookup_w",          (uint64_t)(uintptr_t)zend_jit_symtable_lookup_w, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_symtable_lookup_rw",         (uint64_t)(uintptr_t)zend_jit_symtable_lookup_rw, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_dim_r_helper",         (uint64_t)(uintptr_t)zend_jit_fetch_dim_r_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_dim_is_helper",        (uint64_t)(uintptr_t)zend_jit_fetch_dim_is_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_dim_isset_helper",     (uint64_t)(uintptr_t)zend_jit_fetch_dim_isset_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_dim_rw_helper",        (uint64_t)(uintptr_t)zend_jit_fetch_dim_rw_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_dim_w_helper",         (uint64_t)(uintptr_t)zend_jit_fetch_dim_w_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_dim_str_offset_r_helper", (uint64_t)(uintptr_t)zend_jit_fetch_dim_str_offset_r_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_dim_str_r_helper",     (uint64_t)(uintptr_t)zend_jit_fetch_dim_str_r_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_dim_str_is_helper",    (uint64_t)(uintptr_t)zend_jit_fetch_dim_str_is_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_dim_obj_r_helper",     (uint64_t)(uintptr_t)zend_jit_fetch_dim_obj_r_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_dim_obj_is_helper",    (uint64_t)(uintptr_t)zend_jit_fetch_dim_obj_is_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_invalid_array_access",       (uint64_t)(uintptr_t)zend_jit_invalid_array_access, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_undefined_offset",           (uint64_t)(uintptr_t)zend_jit_undefined_offset, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_undefined_key",              (uint64_t)(uintptr_t)zend_jit_undefined_key, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_zval_array_dup",             (uint64_t)(uintptr_t)zend_jit_zval_array_dup, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_prepare_assign_dim_ref",     (uint64_t)(uintptr_t)zend_jit_prepare_assign_dim_ref, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_cannot_add_element",         (uint64_t)(uintptr_t)zend_jit_cannot_add_element, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_dim_obj_w_helper",     (uint64_t)(uintptr_t)zend_jit_fetch_dim_obj_w_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_dim_obj_rw_helper",    (uint64_t)(uintptr_t)zend_jit_fetch_dim_obj_rw_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_isset_dim_helper",           (uint64_t)(uintptr_t)zend_jit_isset_dim_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_assign_dim_helper",          (uint64_t)(uintptr_t)zend_jit_assign_dim_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_assign_dim_op_helper",       (uint64_t)(uintptr_t)zend_jit_assign_dim_op_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_obj_w_slow",           (uint64_t)(uintptr_t)zend_jit_fetch_obj_w_slow, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_obj_r_slow",           (uint64_t)(uintptr_t)zend_jit_fetch_obj_r_slow, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_obj_is_slow",          (uint64_t)(uintptr_t)zend_jit_fetch_obj_is_slow, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_obj_r_dynamic",        (uint64_t)(uintptr_t)zend_jit_fetch_obj_r_dynamic, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_fetch_obj_is_dynamic",       (uint64_t)(uintptr_t)zend_jit_fetch_obj_is_dynamic, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_check_array_promotion",      (uint64_t)(uintptr_t)zend_jit_check_array_promotion, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_create_typed_ref",           (uint64_t)(uintptr_t)zend_jit_create_typed_ref, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_invalid_property_write",     (uint64_t)(uintptr_t)zend_jit_invalid_property_write, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_invalid_property_read",      (uint64_t)(uintptr_t)zend_jit_invalid_property_read, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_extract_helper",             (uint64_t)(uintptr_t)zend_jit_extract_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_invalid_property_assign",    (uint64_t)(uintptr_t)zend_jit_invalid_property_assign, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_assign_to_typed_prop",       (uint64_t)(uintptr_t)zend_jit_assign_to_typed_prop, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_assign_obj_helper",          (uint64_t)(uintptr_t)zend_jit_assign_obj_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_invalid_property_assign_op", (uint64_t)(uintptr_t)zend_jit_invalid_property_assign_op, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_assign_op_to_typed_prop",    (uint64_t)(uintptr_t)zend_jit_assign_op_to_typed_prop, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_assign_obj_op_helper",       (uint64_t)(uintptr_t)zend_jit_assign_obj_op_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_invalid_property_incdec",    (uint64_t)(uintptr_t)zend_jit_invalid_property_incdec, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_inc_typed_prop",             (uint64_t)(uintptr_t)zend_jit_inc_typed_prop, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_dec_typed_prop",             (uint64_t)(uintptr_t)zend_jit_dec_typed_prop, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_pre_inc_typed_prop",         (uint64_t)(uintptr_t)zend_jit_pre_inc_typed_prop, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_post_inc_typed_prop",        (uint64_t)(uintptr_t)zend_jit_post_inc_typed_prop, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_pre_dec_typed_prop",         (uint64_t)(uintptr_t)zend_jit_pre_dec_typed_prop, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_post_dec_typed_prop",        (uint64_t)(uintptr_t)zend_jit_post_dec_typed_prop, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_pre_inc_obj_helper",         (uint64_t)(uintptr_t)zend_jit_pre_inc_obj_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_post_inc_obj_helper",        (uint64_t)(uintptr_t)zend_jit_post_inc_obj_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_pre_dec_obj_helper",         (uint64_t)(uintptr_t)zend_jit_pre_dec_obj_helper, sizeof(void*));
		ir_disasm_add_symbol("zend_jit_post_dec_obj_helper",        (uint64_t)(uintptr_t)zend_jit_post_dec_obj_helper, sizeof(void*));
	}

#ifndef ZTS
	ir_disasm_add_symbol("EG(current_execute_data)", (uint64_t)(uintptr_t)&EG(current_execute_data), sizeof(EG(current_execute_data)));
	ir_disasm_add_symbol("EG(exception)", (uint64_t)(uintptr_t)&EG(exception), sizeof(EG(exception)));
	ir_disasm_add_symbol("EG(opline_before_exception)", (uint64_t)(uintptr_t)&EG(opline_before_exception), sizeof(EG(opline_before_exception)));
	ir_disasm_add_symbol("EG(vm_interrupt)", (uint64_t)(uintptr_t)&EG(vm_interrupt), sizeof(EG(vm_interrupt)));
	ir_disasm_add_symbol("EG(timed_out)", (uint64_t)(uintptr_t)&EG(timed_out), sizeof(EG(timed_out)));
	ir_disasm_add_symbol("EG(uninitialized_zval)", (uint64_t)(uintptr_t)&EG(uninitialized_zval), sizeof(EG(uninitialized_zval)));
	ir_disasm_add_symbol("EG(zend_constants)", (uint64_t)(uintptr_t)&EG(zend_constants), sizeof(EG(zend_constants)));
	ir_disasm_add_symbol("EG(jit_trace_num)", (uint64_t)(uintptr_t)&EG(jit_trace_num), sizeof(EG(jit_trace_num)));
	ir_disasm_add_symbol("EG(vm_stack_top)", (uint64_t)(uintptr_t)&EG(vm_stack_top), sizeof(EG(vm_stack_top)));
	ir_disasm_add_symbol("EG(vm_stack_end)", (uint64_t)(uintptr_t)&EG(vm_stack_end), sizeof(EG(vm_stack_end)));
	ir_disasm_add_symbol("EG(exception_op)", (uint64_t)(uintptr_t)&EG(exception_op), sizeof(EG(exception_op)));
	ir_disasm_add_symbol("EG(symbol_table)", (uint64_t)(uintptr_t)&EG(symbol_table), sizeof(EG(symbol_table)));

	ir_disasm_add_symbol("CG(map_ptr_base)", (uint64_t)(uintptr_t)&CG(map_ptr_base), sizeof(CG(map_ptr_base)));
#endif
}

static int zend_jit_calc_trace_prologue_size(void)
{
	zend_jit_ctx jit;
	void *entry;
	size_t size;

	zend_jit_init_ctx(&jit, (zend_jit_vm_kind == ZEND_VM_KIND_CALL) ? IR_FUNCTION : 0);

	if (!GCC_GLOBAL_REGS) {
		ir_ref ref;
		ZEND_ASSERT(jit.control);
		ZEND_ASSERT(jit.ctx.ir_base[jit.control].op == IR_START);
		ref = zend_jit_param(&jit, IR_ADDR, "execute_data", 1);
		zend_jit_store_fp(&jit, ref);
		jit.ctx.flags |= IR_FASTCALL_FUNC;
	}

	jit.ctx.flags |= IR_START_BR_TARGET;

	jit.control = ir_emit3(&jit.ctx, IR_UNREACHABLE, jit.control, IR_UNUSED, jit.ctx.ir_base[1].op1);
	jit.ctx.ir_base[1].op1 = jit.control;

	entry = zend_jit_ir_compile(&jit.ctx, &size, "JIT$trace_prologue");
	zend_jit_free_ctx(&jit);

	if (!entry) {
		return 0;
	}

	zend_jit_trace_prologue_size = size;
	return 1;
}

static int zend_jit_setup(void)
{
#if defined(IR_TARGET_X86)
	if (!zend_cpu_supports_sse2()) {
		zend_error(E_CORE_ERROR, "CPU doesn't support SSE2");
		return FAILURE;
	}
#endif
#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
	allowed_opt_flags = 0;
	if (zend_cpu_supports_avx()) {
		allowed_opt_flags |= ZEND_JIT_CPU_AVX;
	}
#endif
#ifdef ZTS
#if defined(IR_TARGET_AARCH64)
	tsrm_ls_cache_tcb_offset = tsrm_get_ls_cache_tcb_offset();
	ZEND_ASSERT(tsrm_ls_cache_tcb_offset != 0);
# elif defined(_WIN64)
	tsrm_tls_index  = _tls_index * sizeof(void*);

	/* To find offset of "_tsrm_ls_cache" in TLS segment we perform a linear scan of local TLS memory */
	/* Probably, it might be better solution */
	do {
		void ***tls_mem = ((void****)__readgsqword(0x58))[_tls_index];
		void *val = _tsrm_ls_cache;
		size_t offset = 0;
		size_t size = (char*)&_tls_end - (char*)&_tls_start;

		while (offset < size) {
			if (*tls_mem == val) {
				tsrm_tls_offset = offset;
				break;
			}
			tls_mem++;
			offset += sizeof(void*);
		}
		if (offset >= size) {
			// TODO: error message ???
			return FAILURE;
		}
	} while(0);
# elif ZEND_WIN32
	tsrm_tls_index  = _tls_index * sizeof(void*);

	/* To find offset of "_tsrm_ls_cache" in TLS segment we perform a linear scan of local TLS memory */
	/* Probably, it might be better solution */
	do {
		void ***tls_mem = ((void****)__readfsdword(0x2c))[_tls_index];
		void *val = _tsrm_ls_cache;
		size_t offset = 0;
		size_t size = (char*)&_tls_end - (char*)&_tls_start;

		while (offset < size) {
			if (*tls_mem == val) {
				tsrm_tls_offset = offset;
				break;
			}
			tls_mem++;
			offset += sizeof(void*);
		}
		if (offset >= size) {
			// TODO: error message ???
			return FAILURE;
		}
	} while(0);
# elif defined(__APPLE__) && defined(__x86_64__)
	tsrm_ls_cache_tcb_offset = tsrm_get_ls_cache_tcb_offset();
	if (tsrm_ls_cache_tcb_offset == 0) {
		size_t *ti;
		__asm__(
			"leaq __tsrm_ls_cache(%%rip),%0"
			: "=r" (ti));
		tsrm_tls_offset = ti[2];
		tsrm_tls_index = ti[1] * 8;
	}
# elif defined(__GNUC__) && defined(__x86_64__)
	tsrm_ls_cache_tcb_offset = tsrm_get_ls_cache_tcb_offset();
	if (tsrm_ls_cache_tcb_offset == 0) {
#if defined(__has_attribute) && __has_attribute(tls_model) && !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__MUSL__)
		size_t ret;

		asm ("movq _tsrm_ls_cache@gottpoff(%%rip),%0"
			: "=r" (ret));
		tsrm_ls_cache_tcb_offset = ret;
#else
		size_t *ti;

		__asm__(
			"leaq _tsrm_ls_cache@tlsgd(%%rip), %0\n"
			: "=a" (ti));
		tsrm_tls_offset = ti[1];
		tsrm_tls_index = ti[0] * 16;
#endif
	}
# elif defined(__GNUC__) && defined(__i386__)
	tsrm_ls_cache_tcb_offset = tsrm_get_ls_cache_tcb_offset();
	if (tsrm_ls_cache_tcb_offset == 0) {
#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__MUSL__)
		size_t ret;

		asm ("leal _tsrm_ls_cache@ntpoff,%0\n"
			: "=a" (ret));
		tsrm_ls_cache_tcb_offset = ret;
#else
		size_t *ti, _ebx, _ecx, _edx;

		__asm__(
			"call 1f\n"
			".subsection 1\n"
			"1:\tmovl (%%esp), %%ebx\n\t"
			"ret\n"
			".previous\n\t"
			"addl $_GLOBAL_OFFSET_TABLE_, %%ebx\n\t"
			"leal _tsrm_ls_cache@tlsldm(%%ebx), %0\n\t"
			"call ___tls_get_addr@plt\n\t"
			"leal _tsrm_ls_cache@tlsldm(%%ebx), %0\n"
			: "=a" (ti), "=&b" (_ebx), "=&c" (_ecx), "=&d" (_edx));
		tsrm_tls_offset = ti[1];
		tsrm_tls_index = ti[0] * 8;
#endif
	}
# endif
#endif

#if 0//???
    memset(sp_adj, 0, sizeof(sp_adj));
#ifdef HAVE_GDB
	sp_adj[SP_ADJ_RET] = sizeof(void*);
	|.if X64WIN
	||	sp_adj[SP_ADJ_ASSIGN] = sp_adj[SP_ADJ_RET] + 0x28;       // sub r4, 0x28
	|.elif X64
	||	sp_adj[SP_ADJ_ASSIGN] = sp_adj[SP_ADJ_RET] + 8;          // sub r4, 8
	|.else
	||	sp_adj[SP_ADJ_ASSIGN] = sp_adj[SP_ADJ_RET] + 12;         // sub r4, 12
	|.endif
	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		zend_jit_set_sp_adj_vm();                                // set sp_adj[SP_ADJ_VM]
#ifndef ZEND_VM_HYBRID_JIT_RED_ZONE_SIZE
		|| sp_adj[SP_ADJ_JIT] = sp_adj[SP_ADJ_VM] + HYBRID_SPAD; // sub r4, HYBRID_SPAD
#else
		|| sp_adj[SP_ADJ_JIT] = sp_adj[SP_ADJ_VM];
#endif
	} else if (GCC_GLOBAL_REGS) {
		|| sp_adj[SP_ADJ_JIT] = sp_adj[SP_ADJ_RET] + SPAD;       // sub r4, SPAD
	} else {
		|| sp_adj[SP_ADJ_JIT] = sp_adj[SP_ADJ_RET] + NR_SPAD;    // sub r4, NR_SPAD
	}
#endif
#endif

    if (JIT_G(debug) & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_ASM_STUBS)) {
		zend_jit_setup_disasm();
	}
	if (JIT_G(debug) & ZEND_JIT_DEBUG_PERF_DUMP) {
		ir_perf_jitdump_open();
	}
	if (!zend_jit_calc_trace_prologue_size()) {
		return FAILURE;
	}
	if (!zend_jit_setup_stubs()) {
		return FAILURE;
	}

	return SUCCESS;
}

static void zend_jit_shutdown_ir(void)
{
	if (JIT_G(debug) & ZEND_JIT_DEBUG_PERF_DUMP) {
		ir_perf_jitdump_close();
	}
	if (JIT_G(debug) & ZEND_JIT_DEBUG_GDB) {
		ir_gdb_unregister_all();
	}
    if (JIT_G(debug) & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_ASM_STUBS)) {
		ir_disasm_free();
	}
}

/* PHP control flow reconstruction helpers */
static ir_ref zend_jit_if_ex(zend_jit_ctx *jit, ir_ref condition, ir_ref true_block)
{
	ZEND_ASSERT(jit->control);
	/* op3 is used as a temporary storage for PHP BB number to reconstruct PHP control flow */
	return jit->control = ir_emit3(&jit->ctx, IR_IF, jit->control, condition, true_block);
}

static void zend_jit_if_true_false_ex(zend_jit_ctx *jit, ir_ref if_ref, ir_ref true_block)
{
	ZEND_ASSERT(JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE);
	ZEND_ASSERT(if_ref);
	ZEND_ASSERT(jit->ctx.ir_base[if_ref].op == IR_IF);
	ZEND_ASSERT(jit->ctx.ir_base[if_ref].op3);
	if (jit->ctx.ir_base[if_ref].op3 == true_block) {
		zend_jit_if_true(jit, if_ref);
	} else {
		zend_jit_if_false(jit, if_ref);
	}
}

static void _zend_jit_add_predecessor_ref(zend_jit_ctx *jit, int b, int pred, ir_ref ref)
{
	int i, *p;
	zend_basic_block *bb;
	ir_ref *r, header;

	ZEND_ASSERT(JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE);
	bb = &jit->ssa->cfg.blocks[b];
	p = &jit->ssa->cfg.predecessors[bb->predecessor_offset];
	r = &jit->bb_edges[jit->bb_predecessors[b]];
	for (i = 0; i < bb->predecessors_count; i++, p++, r++) {
		if (*p == pred) {
			ZEND_ASSERT(*r == IR_UNUSED || *r == ref);
			header = jit->bb_start_ref[b];
			if (header) {
				/* this is back edge */
				ZEND_ASSERT(jit->ctx.ir_base[header].op == IR_LOOP_BEGIN);
				if (jit->ctx.ir_base[ref].op == IR_END) {
					jit->ctx.ir_base[ref].op = IR_LOOP_END;
					jit->ctx.ir_base[ref].op2 = header;
				} else if (jit->ctx.ir_base[ref].op == IR_IF) {
					zend_jit_if_true_false_ex(jit, ref, b);
					ref = zend_jit_loop_end(jit, header);
				} else if (jit->ctx.ir_base[ref].op == IR_UNREACHABLE) {
					zend_jit_begin(jit, ref);
					ref = zend_jit_loop_end(jit, header);
				} else {
					ZEND_ASSERT(0);
				}
				ir_set_op(&jit->ctx, header, i + 1, ref);
			}
			*r = ref;
			return;
		}
	}
	ZEND_ASSERT(0);
}

static void _zend_jit_merge_smart_branch_inputs(zend_jit_ctx *jit,
                                                uint32_t      true_label,
                                                uint32_t      false_label,
                                                ir_ref       *true_inputs,
                                                ir_ref        true_inputs_count,
                                                ir_ref       *false_inputs,
                                                ir_ref        false_inputs_count)
{
	ir_ref true_path = IR_UNUSED, false_path = IR_UNUSED;

	ZEND_ASSERT(JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE);
	if (true_inputs_count == 1) {
		true_path = true_inputs[0];
	} else if (true_inputs_count > 1) {
		zend_jit_merge_N(jit, true_inputs_count, true_inputs);
		true_path = zend_jit_end(jit);
	}
	if (false_inputs_count == 1) {
		ZEND_ASSERT(jit->ctx.ir_base[false_inputs[0]].op == IR_END);
		false_path = false_inputs[0];
	} else if (false_inputs_count > 1) {
		zend_jit_merge_N(jit, false_inputs_count, false_inputs);
		false_path = zend_jit_end(jit);
	}

	if (true_label == false_label && true_path && false_path) {
		zend_jit_merge_2(jit, true_path, false_path);
		_zend_jit_add_predecessor_ref(jit, true_label, jit->b, zend_jit_end(jit));
	} else {
		if (true_path) {
			_zend_jit_add_predecessor_ref(jit, true_label, jit->b, true_path);
		}
		if (false_path) {
			_zend_jit_add_predecessor_ref(jit, false_label, jit->b, false_path);
		}
	}

	jit->control = IR_UNUSED;
	jit->b = -1;
}

static void _zend_jit_fix_merges(zend_jit_ctx *jit)
{
	int i, count;
	ir_ref j, k, n, *p, *q, *r;
	ir_ref ref;
	ir_insn *insn, *phi;

	ZEND_ASSERT(JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE);
	count = jit->ssa->cfg.blocks_count;
	for (i = 0, p = jit->bb_start_ref; i < count; i++, p++) {
		ref = *p;
		if (ref) {
			insn = &jit->ctx.ir_base[ref];
			if (insn->op == IR_MERGE || insn->op == IR_LOOP_BEGIN) {
				n = insn->inputs_count != 0 ? insn->inputs_count : 2;
				/* Remove IS_UNUSED inputs */
				for (j = k = 0, q = r = insn->ops + 1; j < n; j++, q++) {
					if (*q) {
						if (q != r) {
							*r = *q;
							phi = insn + 1 + (n >> 2);
							while (phi->op == IR_PI) {
								phi++;
							}
							while (phi->op == IR_PHI) {
								ir_insn_set_op(phi, k + 1, ir_insn_op(phi, j + 1));
								phi += 1 + ((n + 1) >> 2);
							}
						}
						k++;
						r++;
					}
				}
				if (k != n) {
					ir_ref n2, k2;

					if (k <= 1) {
						insn->op = IR_BEGIN;
						insn->inputs_count = 0;
					} else {
						insn->inputs_count = (k == 2) ? 0 : k;
					}
					n2 = 1 + (n >> 2);
					k2 = 1 + (k >> 2);
					while (k2 != n2) {
						(insn+k2)->optx = IR_NOP;
						k2++;
					}
					phi = insn + 1 + (n >> 2);
					while (phi->op == IR_PI) {
						phi++;
					}
					while (phi->op == IR_PHI) {
						if (k <= 1) {
							phi->op = IR_COPY;
							phi->op1 = phi->op2;
							phi->op2 = 1;
						}
						n2 = 1 + ((n + 1) >> 2);
						k2 = 1 + ((k + 1) >> 2);
						while (k2 != n2) {
							(insn+k2)->optx = IR_NOP;
							k2++;
						}
						phi += 1 + ((n + 1) >> 2);
					}
				}
			}
		}
	}
}

static void zend_jit_case_start(zend_jit_ctx *jit, int switch_b, int case_b, ir_ref switch_ref)
{
	zend_basic_block *bb = &jit->ssa->cfg.blocks[switch_b];
	const zend_op *opline = &jit->op_array->opcodes[bb->start + bb->len - 1];

	if (opline->opcode == ZEND_SWITCH_LONG
	 || opline->opcode == ZEND_SWITCH_STRING
	 || opline->opcode == ZEND_MATCH) {
		HashTable *jumptable = Z_ARRVAL_P(RT_CONSTANT(opline, opline->op2));
		const zend_op *default_opline = ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value);
		int default_b = jit->ssa->cfg.map[default_opline - jit->op_array->opcodes];
		zval *zv;
		ir_ref ref = IR_UNUSED, list = IR_UNUSED, idx;

		ZEND_HASH_FOREACH_VAL(jumptable, zv) {
			const zend_op *target = ZEND_OFFSET_TO_OPLINE(opline, Z_LVAL_P(zv));
			int b = jit->ssa->cfg.map[target - jit->op_array->opcodes];

			if (b == case_b) {
				if (ref) {
					list = ir_emit2(&jit->ctx, IR_END, ref, list);
				}
				if (HT_IS_PACKED(jumptable)) {
					idx = ir_const_php_long(&jit->ctx, zv - jumptable->arPacked);
				} else {
					idx = ir_const_php_long(&jit->ctx, (Bucket*)zv - jumptable->arData);
				}
				ref = ir_emit2(&jit->ctx, IR_CASE_VAL, switch_ref, idx);
			}
		} ZEND_HASH_FOREACH_END();
		if (default_b == case_b) {
			if (ref) {
				list = ir_emit2(&jit->ctx, IR_END, ref, list);
			}
			if (jit->ctx.ir_base[switch_ref].op3) {
				/* op3 may contain a list of additional "default" path inputs for MATCH */
				ref = jit->ctx.ir_base[switch_ref].op3;
				jit->ctx.ir_base[switch_ref].op3 = IS_UNDEF;
				ZEND_ASSERT(jit->ctx.ir_base[ref].op == IR_END);
				ir_ref end = ref;
				while (jit->ctx.ir_base[end].op2) {
					ZEND_ASSERT(jit->ctx.ir_base[end].op == IR_END);
					end = jit->ctx.ir_base[end].op2;
				}
				jit->ctx.ir_base[end].op2 = list;
				list = ref;
			}
			ref = ir_emit1(&jit->ctx, IR_CASE_DEFAULT, switch_ref);
		}
		if (list) {
			list = ir_emit2(&jit->ctx, IR_END, ref, list);
			zend_jit_merge_list(jit, list);
		} else {
			jit->control = ref;
		}
	} else {
		ZEND_ASSERT(0);
	}
}

static int zend_jit_bb_start(zend_jit_ctx *jit, int b)
{
	zend_basic_block *bb;
	int i, n, *p, pred;
	ir_ref ref, bb_start;

	ZEND_ASSERT(JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE);
	ZEND_ASSERT(b < jit->ssa->cfg.blocks_count);
	bb = &jit->ssa->cfg.blocks[b];
	ZEND_ASSERT((bb->flags & ZEND_BB_REACHABLE) != 0);
	n = bb->predecessors_count;

	if (jit->control) {
		if (n != 0) {
			ZEND_ASSERT(jit->ctx.ir_base[jit->control].op == IR_ENTRY);
			if (!GCC_GLOBAL_REGS) {
				/* 2 is hardcoded reference to IR_PARAM */
				ZEND_ASSERT(jit->ctx.ir_base[2].op == IR_PARAM);
				ZEND_ASSERT(jit->ctx.ir_base[2].op3 == 1);
				zend_jit_store_fp(jit, 2);
			}
		} else {
			ZEND_ASSERT(jit->ctx.ir_base[jit->control].op == IR_START);
		}
	}

	if (n == 0) {
		/* pass */
		ZEND_ASSERT(jit->control);
		ZEND_ASSERT(jit->ctx.ir_base[jit->control].op == IR_START);
		bb_start = jit->control;
		if (!GCC_GLOBAL_REGS) {
			ir_ref ref = zend_jit_param(jit, IR_ADDR, "execute_data", 1);
			zend_jit_store_fp(jit, ref);
			jit->ctx.flags |= IR_FASTCALL_FUNC;
		}
	} else if (n == 1) {
		pred = jit->ssa->cfg.predecessors[bb->predecessor_offset];
		ref = jit->bb_edges[jit->bb_predecessors[b]];
		if (ref == IR_UNUSED) {
			if (!jit->control) {
				zend_jit_begin(jit, IR_UNUSED); /* unreachable block */
			}
		} else if (jit->ctx.ir_base[ref].op == IR_IF) {
			if (!jit->control) {
				zend_jit_if_true_false_ex(jit, ref, b);
			} else {
				ir_ref entry_path = zend_jit_end(jit);
				zend_jit_if_true_false_ex(jit, ref, b);
				zend_jit_merge_2(jit, zend_jit_end(jit), entry_path);
			}
		} else if (jit->ctx.ir_base[ref].op == IR_SWITCH) {
			zend_jit_case_start(jit, pred, b, ref);
		} else {
			if (!jit->control) {
				ZEND_ASSERT(jit->ctx.ir_base[ref].op == IR_END || jit->ctx.ir_base[ref].op == IR_UNREACHABLE);
				zend_jit_begin(jit, ref);
			} else {
				zend_jit_merge_2(jit, ref, zend_jit_end(jit));
			}
		}
		bb_start = jit->control;
	} else {
		int forward_edges_count = 0;
		int back_edges_count = 0;
		ir_ref *pred_refs;
		ir_ref entry_path = IR_UNUSED;
		ALLOCA_FLAG(use_heap);

		if (jit->control) {
			entry_path = zend_jit_end(jit);
		}
		pred_refs = (ir_ref *)do_alloca(sizeof(ir_ref) * n, use_heap);
		for (i = 0, p = jit->ssa->cfg.predecessors + bb->predecessor_offset; i < n; p++, i++) {
			pred = *p;
			if (jit->bb_start_ref[pred]) {
				/* forward edge */
				forward_edges_count++;
				ref = jit->bb_edges[jit->bb_predecessors[b] + i];
				if (ref == IR_UNUSED) {
					/* dead edge */
					pred_refs[i] = IR_UNUSED;
				} else {
					ir_op op = jit->ctx.ir_base[ref].op;

					if (op == IR_IF) {
						zend_jit_if_true_false_ex(jit, ref, b);
						pred_refs[i] = zend_jit_end(jit);
					} else if (op == IR_SWITCH) {
						zend_jit_case_start(jit, pred, b, ref);
						pred_refs[i] = zend_jit_end(jit);
					} else {
						ZEND_ASSERT(op == IR_END || op == IR_UNREACHABLE);
						pred_refs[i] = ref;
					}
				}
			} else {
				/* backward edge */
				back_edges_count++;
				pred_refs[i] = IR_UNUSED;
			}
		}

		if (bb->flags & ZEND_BB_LOOP_HEADER) {
			ZEND_ASSERT(back_edges_count != 0);
			ZEND_ASSERT(forward_edges_count != 0);
			zend_jit_merge_N(jit, n, pred_refs);
			jit->ctx.ir_base[jit->control].op = IR_LOOP_BEGIN;
			bb_start = jit->control;
			if (entry_path) {
				zend_jit_merge_2(jit, zend_jit_end(jit), entry_path);
			}
		} else {
//			ZEND_ASSERT(back_edges_count != 0);
			/* edges from exceptional blocks may be counted as back edges */
			zend_jit_merge_N(jit, n, pred_refs);
			bb_start = jit->control;
			if (entry_path) {
				zend_jit_merge_2(jit, zend_jit_end(jit), entry_path);
			}
		}
		free_alloca(pred_refs, use_heap);
	}
	jit->b = b;
	jit->bb_start_ref[b] = bb_start;

	return 1;
}

static int zend_jit_bb_end(zend_jit_ctx *jit, int b)
{
	int succ;
	zend_basic_block *bb;

	ZEND_ASSERT(JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE);
	if (jit->b != b) {
		return 1;
	}

	bb = &jit->ssa->cfg.blocks[b];
	ZEND_ASSERT(bb->successors_count != 0);
	if (bb->successors_count == 1) {
		succ = bb->successors[0];
	} else {
		const zend_op *opline = &jit->op_array->opcodes[bb->start + bb->len - 1];

		/* Use only the following successor of SWITCH and FE_RESET_R */
		ZEND_ASSERT(opline->opcode == ZEND_SWITCH_LONG
		 || opline->opcode == ZEND_SWITCH_STRING
		 || opline->opcode == ZEND_MATCH
		 || opline->opcode == ZEND_FE_RESET_R);
		succ = b + 1;
	}
	_zend_jit_add_predecessor_ref(jit, succ, b, zend_jit_end(jit));
	jit->control = IR_UNUSED;
	jit->b = -1;
	return 1;
}

static int zend_jit_cmp_ip(zend_jit_ctx *jit, ir_op op, const zend_op *next_opline)
{
	ir_ref ref;

#if 1
	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		ref = zend_jit_ip32(jit);
	} else {
		ref = zend_jit_ex_opline_addr(jit);
		ref = zend_jit_load(jit, IR_U32, ref);
	}
	ref = ir_fold2(&jit->ctx, IR_OPT(op, IR_BOOL), ref, ir_const_u32(&jit->ctx, (uint32_t)(uintptr_t)next_opline));
#else
	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		ref = zend_jit_ip(jit);
	} else {
		ref = zend_jit_ex_opline_addr(jit);
		ref = zend_jit_load(jit, IR_ADDR, ref);
	}
	ref = ir_fold2(&jit->ctx, IR_OPT(op, IR_BOOL), ref, zend_jit_const_addr(jit, (uintptr_t)next_opline));
#endif
	return ref;
}

static int zend_jit_cond_jmp(zend_jit_ctx *jit, const zend_op *next_opline, int target_block)
{
	ir_ref ref;
	zend_basic_block *bb;

	ZEND_ASSERT(jit->b >= 0);
	bb = &jit->ssa->cfg.blocks[jit->b];

	ZEND_ASSERT(bb->successors_count == 2);
	if (bb->successors[0] == bb->successors[1]) {
		_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, zend_jit_end(jit));
		jit->control = IR_UNUSED;
		jit->b = -1;
		zend_jit_set_last_valid_opline(jit, next_opline);
		return 1;
	}

	ref = zend_jit_cmp_ip(jit, IR_NE, next_opline);
	ref = zend_jit_if_ex(jit, ref, target_block);

	_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, ref);
	_zend_jit_add_predecessor_ref(jit, bb->successors[1], jit->b, ref);

	jit->control = IR_UNUSED;
	jit->b = -1;
	zend_jit_set_last_valid_opline(jit, next_opline);

	return 1;
}

static int zend_jit_set_cond(zend_jit_ctx *jit, const zend_op *next_opline, uint32_t var)
{
	ir_ref ref;

	ref = zend_jit_cmp_ip(jit, IR_EQ, next_opline);
	ref = ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref);
	ref = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32), ref, ir_const_u32(&jit->ctx, IS_FALSE));

	// EX_VAR(var) = ...
	zend_jit_store_at_offset(jit,
		zend_jit_fp(jit), var + offsetof(zval, u1.type_info),
		ref);

	zend_jit_reset_last_valid_opline(jit);
	return zend_jit_set_ip(jit, next_opline - 1);
}

static int zend_jit_handler(zend_jit_ctx *jit, const zend_op *opline, int may_throw)
{
	ir_ref ref;
	const void *handler;

	zend_jit_set_ip(jit, opline);
	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		handler = zend_get_opcode_handler_func(opline);
	} else {
		handler = opline->handler;
	}
	if (GCC_GLOBAL_REGS) {
		zend_jit_call_0(jit, IR_VOID, zend_jit_const_func_addr(jit, (uintptr_t)handler, 0));
	} else {
		ref = zend_jit_fp(jit);
		ref = zend_jit_call_1(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)handler, IR_CONST_FASTCALL_FUNC), ref);
	}
	if (may_throw) {
		zend_jit_check_exception(jit);
	}
	/* Skip the following OP_DATA */
	switch (opline->opcode) {
		case ZEND_ASSIGN_DIM:
		case ZEND_ASSIGN_OBJ:
		case ZEND_ASSIGN_STATIC_PROP:
		case ZEND_ASSIGN_DIM_OP:
		case ZEND_ASSIGN_OBJ_OP:
		case ZEND_ASSIGN_STATIC_PROP_OP:
		case ZEND_ASSIGN_STATIC_PROP_REF:
		case ZEND_ASSIGN_OBJ_REF:
			zend_jit_set_last_valid_opline(jit, opline + 2);
			break;
		default:
			zend_jit_set_last_valid_opline(jit, opline + 1);
			break;
	}
	return 1;
}

static int zend_jit_tail_handler(zend_jit_ctx *jit, const zend_op *opline)
{
	const void *handler;
	ir_ref ref;
	zend_basic_block *bb;

	zend_jit_set_ip(jit, opline);
	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		if (opline->opcode == ZEND_DO_UCALL ||
		    opline->opcode == ZEND_DO_FCALL_BY_NAME ||
		    opline->opcode == ZEND_DO_FCALL ||
		    opline->opcode == ZEND_RETURN) {

			/* Use inlined HYBRID VM handler */
			handler = opline->handler;
			zend_jit_tailcall_0(jit, zend_jit_const_func_addr(jit, (uintptr_t)handler, 0));
		} else {
			handler = zend_get_opcode_handler_func(opline);
			zend_jit_call_0(jit, IR_VOID, zend_jit_const_func_addr(jit, (uintptr_t)handler, 0));
			ref = zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit));
			zend_jit_tailcall_0(jit, ref);
		}
	} else {
		handler = opline->handler;
		if (GCC_GLOBAL_REGS) {
			zend_jit_tailcall_0(jit, zend_jit_const_func_addr(jit, (uintptr_t)handler, 0));
		} else {
			ref = zend_jit_fp(jit);
			zend_jit_tailcall_1(jit, zend_jit_const_func_addr(jit, (uintptr_t)handler, IR_CONST_FASTCALL_FUNC), ref);
		}
	}
	if (jit->b >= 0) {
		bb = &jit->ssa->cfg.blocks[jit->b];
		if (bb->successors_count > 0
		 && opline->opcode != ZEND_MATCH_ERROR) {
			int succ;

			if (bb->successors_count == 1) {
				succ = bb->successors[0];
			} else {
				/* Use only the following successor of FAST_CALL */
				ZEND_ASSERT(opline->opcode == ZEND_FAST_CALL);
				succ = jit->b + 1;
			}
			_zend_jit_add_predecessor_ref(jit, succ, jit->b, jit->control);
		}
		jit->control = IR_UNUSED;
		jit->b = -1;
		zend_jit_reset_last_valid_opline(jit);
	}
	return 1;
}

static int zend_jit_call(zend_jit_ctx *jit, const zend_op *opline, unsigned int next_block)
{
	return zend_jit_tail_handler(jit, opline);
}

static int zend_jit_spill_store(zend_jit_ctx *jit, zend_jit_addr src, zend_jit_addr dst, uint32_t info, bool set_type)
{
	ZEND_ASSERT(Z_MODE(src) == IS_REG);
	ZEND_ASSERT(Z_MODE(dst) == IS_MEM_ZVAL);

	if ((info & MAY_BE_ANY) == MAY_BE_LONG) {
		zend_jit_zval_set_lval(jit, dst, zend_jit_use_reg(jit, src));
		if (set_type &&
		    (Z_REG(dst) != ZREG_FP ||
		     !JIT_G(current_frame) ||
		     STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(Z_OFFSET(dst))) != IS_LONG)) {
			zend_jit_zval_set_type_info(jit, dst, IS_LONG);
		}
	} else if ((info & MAY_BE_ANY) == MAY_BE_DOUBLE) {
		zend_jit_zval_set_dval(jit, dst, zend_jit_use_reg(jit, src));
		if (set_type &&
		    (Z_REG(dst) != ZREG_FP ||
		     !JIT_G(current_frame) ||
		     STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(Z_OFFSET(dst))) != IS_DOUBLE)) {
			zend_jit_zval_set_type_info(jit, dst, IS_DOUBLE);
		}
	} else {
		ZEND_UNREACHABLE();
	}
	return 1;
}

static int zend_jit_load_reg(zend_jit_ctx *jit, zend_jit_addr src, zend_jit_addr dst, uint32_t info)
{
	ZEND_ASSERT(Z_MODE(src) == IS_MEM_ZVAL);
	ZEND_ASSERT(Z_MODE(dst) == IS_REG);

	if ((info & MAY_BE_ANY) == MAY_BE_LONG) {
		zend_jit_def_reg(jit, dst,
			zend_jit_zval_lval(jit, src));
	} else if ((info & MAY_BE_ANY) == MAY_BE_DOUBLE) {
		zend_jit_def_reg(jit, dst,
			zend_jit_zval_dval(jit, src));
	} else {
		ZEND_UNREACHABLE();
	}
	return 1;
}

static int zend_jit_store_var(zend_jit_ctx *jit, uint32_t info, int var, int ssa_var, bool set_type)
{
	zend_jit_addr src = ZEND_ADDR_REG(ssa_var);
	zend_jit_addr dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, EX_NUM_TO_VAR(var));

	return zend_jit_spill_store(jit, src, dst, info, set_type);
}

static int zend_jit_store_ref(zend_jit_ctx *jit, uint32_t info, int var, int32_t src, bool set_type)
{
	zend_jit_addr dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, EX_NUM_TO_VAR(var));

	if ((info & MAY_BE_ANY) == MAY_BE_LONG) {
		zend_jit_zval_set_lval(jit, dst, src);
		if (set_type &&
		    (Z_REG(dst) != ZREG_FP ||
		     !JIT_G(current_frame) ||
		     STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(Z_OFFSET(dst))) != IS_LONG)) {
			zend_jit_zval_set_type_info(jit, dst, IS_LONG);
		}
	} else if ((info & MAY_BE_ANY) == MAY_BE_DOUBLE) {
		zend_jit_zval_set_dval(jit, dst, src);
		if (set_type &&
		    (Z_REG(dst) != ZREG_FP ||
		     !JIT_G(current_frame) ||
		     STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(Z_OFFSET(dst))) != IS_DOUBLE)) {
			zend_jit_zval_set_type_info(jit, dst, IS_DOUBLE);
		}
	} else {
		ZEND_UNREACHABLE();
	}
	return 1;
}

static ir_ref zend_jit_deopt_rload(zend_jit_ctx *jit, ir_type type, int32_t reg)
{
	ir_ref ref = jit->control;
	ir_insn *insn;

	while (1) {
		insn = &jit->ctx.ir_base[ref];
		if (insn->op == IR_RLOAD && insn->op2 == reg) {
			ZEND_ASSERT(insn->type == type);
			return ref;
		} else if (insn->op == IR_START) {
			break;
		}
		ref = insn->op1;
	}
	return zend_jit_rload(jit, type, reg);
}

static int zend_jit_store_type(zend_jit_ctx *jit, int var, uint8_t type)
{
	zend_jit_addr dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, EX_NUM_TO_VAR(var));

	ZEND_ASSERT(type <= IS_DOUBLE);
	zend_jit_zval_set_type_info(jit, dst, type);
	return 1;
}

static int zend_jit_store_reg(zend_jit_ctx *jit, uint32_t info, int var, int8_t reg, bool set_type)
{
	zend_jit_addr src;
	zend_jit_addr dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, EX_NUM_TO_VAR(var));
	ir_type type;

	if ((info & MAY_BE_ANY) == MAY_BE_LONG) {
		type = IR_PHP_LONG;
		src = zend_jit_deopt_rload(jit, type, reg);
		if (jit->ra && jit->ra[var].ref == IR_NULL) {
			zend_jit_def_reg(jit, ZEND_ADDR_REG(var), src);
		} else {
			zend_jit_zval_set_lval(jit, dst, src);
			if (set_type &&
			    (Z_REG(dst) != ZREG_FP ||
			     !JIT_G(current_frame) ||
			     STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(Z_OFFSET(dst))) != IS_LONG)) {
				zend_jit_zval_set_type_info(jit, dst, IS_LONG);
			}
		}
	} else if ((info & MAY_BE_ANY) == MAY_BE_DOUBLE) {
		type = IR_DOUBLE;
		src = zend_jit_deopt_rload(jit, type, reg);
		if (jit->ra && jit->ra[var].ref == IR_NULL) {
			zend_jit_def_reg(jit, ZEND_ADDR_REG(var), src);
		} else {
			zend_jit_zval_set_dval(jit, dst, src);
			if (set_type &&
			    (Z_REG(dst) != ZREG_FP ||
			     !JIT_G(current_frame) ||
			     STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(Z_OFFSET(dst))) != IS_DOUBLE)) {
				zend_jit_zval_set_type_info(jit, dst, IS_DOUBLE);
			}
		}
	} else {
		ZEND_UNREACHABLE();
	}
	return 1;
}

static int zend_jit_store_var_type(zend_jit_ctx *jit, int var, uint32_t type)
{
	zend_jit_addr dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, EX_NUM_TO_VAR(var));

	zend_jit_zval_set_type_info(jit, dst, type);
	return 1;
}

static int zend_jit_zval_try_addref(zend_jit_ctx *jit, zend_jit_addr var_addr)
{
	ir_ref if_refcounted, end1;

	if_refcounted = zend_jit_if_zval_refcounted(jit, var_addr);
	zend_jit_if_false(jit, if_refcounted);
	end1 = zend_jit_end(jit);
	zend_jit_if_true(jit, if_refcounted);
	zend_jit_gc_addref(jit, zend_jit_zval_ptr(jit, var_addr));
	zend_jit_merge_2(jit, end1, zend_jit_end(jit));
	return 1;
}

static int zend_jit_store_var_if_necessary(zend_jit_ctx *jit, int var, zend_jit_addr src, uint32_t info)
{
	if (Z_MODE(src) == IS_REG && Z_STORE(src)) {
		zend_jit_addr dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, var);
		return zend_jit_spill_store(jit, src, dst, info, 1);
	}
	return 1;
}

static int zend_jit_store_var_if_necessary_ex(zend_jit_ctx *jit, int var, zend_jit_addr src, uint32_t info, zend_jit_addr old, uint32_t old_info)
{
	if (Z_MODE(src) == IS_REG && Z_STORE(src)) {
		zend_jit_addr dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, var);
		bool set_type = 1;

		if ((info & (MAY_BE_ANY|MAY_BE_REF|MAY_BE_UNDEF)) ==
		    (old_info & (MAY_BE_ANY|MAY_BE_REF|MAY_BE_UNDEF))) {
			if (Z_MODE(old) != IS_REG || Z_LOAD(old) || Z_STORE(old)) {
				set_type = 0;
			}
		}
		return zend_jit_spill_store(jit, src, dst, info, set_type);
	}
	return 1;
}

static int zend_jit_load_var(zend_jit_ctx *jit, uint32_t info, int var, int ssa_var)
{
	zend_jit_addr src = ZEND_ADDR_MEM_ZVAL(ZREG_FP, EX_NUM_TO_VAR(var));
	zend_jit_addr dst = ZEND_ADDR_REG(ssa_var);

	return zend_jit_load_reg(jit, src, dst, info);
}

static int zend_jit_invalidate_var_if_necessary(zend_jit_ctx *jit, zend_uchar op_type, zend_jit_addr addr, znode_op op)
{
	if ((op_type & (IS_TMP_VAR|IS_VAR)) && Z_MODE(addr) == IS_REG && !Z_LOAD(addr) && !Z_STORE(addr)) {
		zend_jit_addr dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, op.var);
		zend_jit_zval_set_type_info(jit, dst, IS_UNDEF);
	}
	return 1;
}

static int zend_jit_update_regs(zend_jit_ctx *jit, uint32_t var, zend_jit_addr src, zend_jit_addr dst, uint32_t info)
{
	if (!zend_jit_same_addr(src, dst)) {
		if (Z_MODE(src) == IS_REG) {
			if (Z_MODE(dst) == IS_REG) {
				zend_jit_def_reg(jit, dst, zend_jit_use_reg(jit, src));
				if (!Z_LOAD(src) && !Z_STORE(src) && Z_STORE(dst)) {
					zend_jit_addr var_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, var);

					if (!zend_jit_spill_store(jit, dst, var_addr, info,
							JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE ||
							JIT_G(current_frame) == NULL ||
							STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(var)) == IS_UNKNOWN ||
							(1 << STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(var))) != (info & MAY_BE_ANY)
					)) {
						return 0;
					}
				}
			} else if (Z_MODE(dst) == IS_MEM_ZVAL) {
				if (!Z_LOAD(src) && !Z_STORE(src)) {
					if (!zend_jit_spill_store(jit, src, dst, info,
							JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE ||
							JIT_G(current_frame) == NULL ||
							STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(var)) == IS_UNKNOWN ||
							(1 << STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(var))) != (info & MAY_BE_ANY)
					)) {
						return 0;
					}
				}
			} else {
				ZEND_UNREACHABLE();
			}
		} else if (Z_MODE(src) == IS_MEM_ZVAL) {
			if (Z_MODE(dst) == IS_REG) {
				if (!zend_jit_load_reg(jit, src, dst, info)) {
					return 0;
				}
			} else {
				ZEND_UNREACHABLE();
			}
		} else {
			ZEND_UNREACHABLE();
		}
	} else if (Z_MODE(dst) == IS_REG && Z_STORE(dst)) {
		dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, var);
		if (!zend_jit_spill_store(jit, src, dst, info,
				JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE ||
				JIT_G(current_frame) == NULL ||
				STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(var)) == IS_UNKNOWN ||
				(1 << STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(var))) != (info & MAY_BE_ANY)
		)) {
			return 0;
		}
	}
	return 1;
}

static int zend_jit_inc_dec(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, uint32_t op1_def_info, zend_jit_addr op1_def_addr, uint32_t res_use_info, uint32_t res_info, zend_jit_addr res_addr, int may_overflow, int may_throw)
{
	ir_ref if_long = IR_UNUSED;
	ir_ref typed_ref_path = IR_UNUSED;
	ir_ref ref;
	ir_op op;

	if (op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY)-MAY_BE_LONG)) {
		if_long = zend_jit_if_zval_type(jit, op1_addr, IS_LONG);
		zend_jit_if_true(jit, if_long);
	}
	if (opline->opcode == ZEND_POST_INC || opline->opcode == ZEND_POST_DEC) {
		zend_jit_zval_set_lval(jit, res_addr,
			zend_jit_zval_lval(jit, op1_addr));
		if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
			zend_jit_zval_set_type_info(jit, res_addr, IS_LONG);
		}
	}
	if (Z_MODE(op1_def_addr) == IS_MEM_ZVAL
	 && Z_MODE(op1_addr) == IS_REG
	 && !Z_LOAD(op1_addr)
	 && !Z_STORE(op1_addr)) {
		zend_jit_zval_set_type_info(jit, op1_def_addr, IS_LONG);
	}
	if (opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_POST_INC) {
		op = may_overflow ? IR_ADD_OV : IR_ADD;
	} else {
		op = may_overflow ? IR_SUB_OV : IR_SUB;
	}
	ref = ir_fold2(&jit->ctx, IR_OPT(op, IR_PHP_LONG),
		zend_jit_zval_lval(jit, op1_addr),
		ir_const_php_long(&jit->ctx, 1));
	if (op1_def_info & MAY_BE_LONG) {
		zend_jit_zval_set_lval(jit, op1_def_addr, ref);
	}
	if (may_overflow &&
	    (((op1_def_info & (MAY_BE_ANY|MAY_BE_GUARD)) == (MAY_BE_LONG|MAY_BE_GUARD)) ||
	     ((opline->result_type != IS_UNUSED && (res_info & (MAY_BE_ANY|MAY_BE_GUARD)) == (MAY_BE_LONG|MAY_BE_GUARD))))) {
		int32_t exit_point;
		const void *exit_addr;
		zend_jit_trace_stack *stack;
		uint32_t old_op1_info, old_res_info = 0;

		stack = JIT_G(current_frame)->stack;
		old_op1_info = STACK_INFO(stack, EX_VAR_TO_NUM(opline->op1.var));
		SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->op1.var), IS_DOUBLE, 0);
		if (opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_POST_INC) {
			SET_STACK_REF(stack, EX_VAR_TO_NUM(opline->op1.var), ir_const_double(&jit->ctx, ZEND_LONG_MAX + 1.0));
		} else {
			SET_STACK_REF(stack, EX_VAR_TO_NUM(opline->op1.var), ir_const_double(&jit->ctx, ZEND_LONG_MIN - 1.0));
		}
		if (opline->result_type != IS_UNUSED) {
			old_res_info = STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var));
			if (opline->opcode == ZEND_PRE_INC) {
				SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_DOUBLE, 0);
				SET_STACK_REF(stack, EX_VAR_TO_NUM(opline->result.var), ir_const_double(&jit->ctx, ZEND_LONG_MAX + 1.0));
			} else if (opline->opcode == ZEND_PRE_DEC) {
				SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_DOUBLE, 0);
				SET_STACK_REF(stack, EX_VAR_TO_NUM(opline->result.var), ir_const_double(&jit->ctx, ZEND_LONG_MIN - 1.0));
			} else if (opline->opcode == ZEND_POST_INC) {
				SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_LONG, 0);
				SET_STACK_REF(stack, EX_VAR_TO_NUM(opline->result.var), ir_const_php_long(&jit->ctx, ZEND_LONG_MAX));
			} else if (opline->opcode == ZEND_POST_DEC) {
				SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_LONG, 0);
				SET_STACK_REF(stack, EX_VAR_TO_NUM(opline->result.var), ir_const_php_long(&jit->ctx, ZEND_LONG_MIN));
			}
		}

		exit_point = zend_jit_trace_get_exit_point(opline + 1, 0);
		exit_addr = zend_jit_trace_get_exit_addr(exit_point);
		zend_jit_guard_not(jit,
			ir_fold1(&jit->ctx, IR_OPT(IR_OVERFLOW, IR_BOOL), ref),
			zend_jit_const_addr(jit, (uintptr_t)exit_addr));

		if ((opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_PRE_DEC) &&
		    opline->result_type != IS_UNUSED) {
			zend_jit_zval_set_lval(jit, res_addr, ref);
			if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
				zend_jit_zval_set_type_info(jit, res_addr, IS_LONG);
			}
		}

		SET_STACK_INFO(stack, EX_VAR_TO_NUM(opline->op1.var), old_op1_info);
		if (opline->result_type != IS_UNUSED) {
			SET_STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var), old_res_info);
		}
	} else if (may_overflow) {
		ir_ref if_overflow, fast_path = IR_UNUSED;

		if (((op1_def_info & (MAY_BE_ANY|MAY_BE_GUARD)) == (MAY_BE_DOUBLE|MAY_BE_GUARD))
		 || (opline->result_type != IS_UNUSED && (res_info & (MAY_BE_ANY|MAY_BE_GUARD)) ==  (MAY_BE_DOUBLE|MAY_BE_GUARD))) {
			int32_t exit_point;
			const void *exit_addr;

			exit_point = zend_jit_trace_get_exit_point(opline, 0);
			exit_addr = zend_jit_trace_get_exit_addr(exit_point);
			zend_jit_guard(jit,
				ir_fold1(&jit->ctx, IR_OPT(IR_OVERFLOW, IR_BOOL), ref),
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		} else {
			if_overflow = zend_jit_if(jit,
				ir_fold1(&jit->ctx, IR_OPT(IR_OVERFLOW, IR_BOOL), ref));
			zend_jit_if_false(jit, if_overflow);
			if ((opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_PRE_DEC) &&
			    opline->result_type != IS_UNUSED) {
				zend_jit_zval_set_lval(jit, res_addr, ref);
				if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
					zend_jit_zval_set_type_info(jit, res_addr, IS_LONG);
				}
			}
			fast_path = zend_jit_end(jit);

			/* overflow => cold path */
			zend_jit_if_true_cold(jit, if_overflow);
		}

		if (opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_POST_INC) {
			if (Z_MODE(op1_def_addr) == IS_REG) {
				zend_jit_zval_set_dval(jit, op1_def_addr,
					ir_const_double(&jit->ctx, (double)ZEND_LONG_MAX + 1.0));
			} else {
#if SIZEOF_ZEND_LONG == 4
				zend_jit_zval_set_lval(jit, op1_def_addr,
					ir_const_php_long(&jit->ctx, 0));
				zend_jit_zval_set_w2(jit, op1_def_addr,
					ir_const_u32(&jit->ctx, 0x41e00000));
#else
				zend_jit_zval_set_lval(jit, op1_def_addr,
					ir_const_php_long(&jit->ctx, 0x43e0000000000000));
#endif
				zend_jit_zval_set_type_info(jit, op1_def_addr, IS_DOUBLE);
			}
		} else {
			if (Z_MODE(op1_def_addr) == IS_REG) {
				zend_jit_zval_set_dval(jit, op1_def_addr,
					ir_const_double(&jit->ctx, (double)ZEND_LONG_MIN - 1.0));
			} else {
#if SIZEOF_ZEND_LONG == 4
				zend_jit_zval_set_lval(jit, op1_def_addr,
					ir_const_php_long(&jit->ctx, 0x00200000));
				zend_jit_zval_set_w2(jit, op1_def_addr,
					ir_const_u32(&jit->ctx, 0xc1e00000));
#else
				zend_jit_zval_set_lval(jit, op1_def_addr,
					ir_const_php_long(&jit->ctx, 0xc3e0000000000000));
#endif
				zend_jit_zval_set_type_info(jit, op1_def_addr, IS_DOUBLE);
			}
		}
		if ((opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_PRE_DEC) &&
		    opline->result_type != IS_UNUSED) {
			if (opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_POST_INC) {
				if (Z_MODE(res_addr) == IS_REG) {
					zend_jit_zval_set_dval(jit, res_addr,
						ir_const_double(&jit->ctx, (double)ZEND_LONG_MAX + 1.0));
				} else {
#if SIZEOF_ZEND_LONG == 4
					zend_jit_zval_set_lval(jit, res_addr,
						ir_const_php_long(&jit->ctx, 0));
					zend_jit_zval_set_w2(jit, res_addr,
						ir_const_u32(&jit->ctx, 0x41e00000));
#else
					zend_jit_zval_set_lval(jit, res_addr,
						ir_const_php_long(&jit->ctx, 0x43e0000000000000));
#endif
					zend_jit_zval_set_type_info(jit, res_addr, IS_DOUBLE);
				}
			} else {
				if (Z_MODE(res_addr) == IS_REG) {
					zend_jit_zval_set_dval(jit, res_addr,
						ir_const_double(&jit->ctx, (double)ZEND_LONG_MIN - 1.0));
				} else {
#if SIZEOF_ZEND_LONG == 4
					zend_jit_zval_set_lval(jit, res_addr,
						ir_const_php_long(&jit->ctx, 0x00200000));
					zend_jit_zval_set_w2(jit, res_addr,
						ir_const_u32(&jit->ctx, 0xc1e00000));
#else
					zend_jit_zval_set_lval(jit, res_addr,
						ir_const_php_long(&jit->ctx, 0xc3e0000000000000));
#endif
					zend_jit_zval_set_type_info(jit, res_addr, IS_DOUBLE);
				}
			}
		}

		if (fast_path) {
			zend_jit_merge_2(jit, fast_path, zend_jit_end(jit));
		}
	} else {
		if ((opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_PRE_DEC) &&
		    opline->result_type != IS_UNUSED) {
			zend_jit_zval_set_lval(jit, res_addr, ref);
			if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
				zend_jit_zval_set_type_info(jit, res_addr, IS_LONG);
			}
		}
	}
	if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG)) {
		ir_ref fast_path = zend_jit_end(jit);
		ir_ref err_path = IR_UNUSED;

		/* !is_long => cold path */
		zend_jit_if_false_cold(jit, if_long);
		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) {
			zend_jit_set_ex_opline(jit, opline);
			if (op1_info & MAY_BE_UNDEF) {
				ir_ref if_def;

				if_def = zend_jit_if_not_zval_type(jit, op1_addr, IS_UNDEF);
				zend_jit_if_false_cold(jit, if_def);

				// zend_error(E_WARNING, "Undefined variable $%s", ZSTR_VAL(CV_DEF_OF(EX_VAR_TO_NUM(opline->op1.var))));
				zend_jit_call_1(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_undefined_op_helper, IR_CONST_FASTCALL_FUNC),
					ir_const_u32(&jit->ctx, opline->op1.var));

				zend_jit_zval_set_type_info(jit, op1_def_addr, IS_NULL);
				err_path = zend_jit_end(jit);
				zend_jit_if_true(jit, if_def);
				zend_jit_merge_2(jit, err_path, zend_jit_end(jit)); // wrong merge

				op1_info |= MAY_BE_NULL;
			}

			ref = zend_jit_zval_addr(jit, op1_addr);

			if (op1_info & MAY_BE_REF) {
				ir_ref if_ref, if_typed, ref_path, func, ref2, arg2;

				if_ref = zend_jit_if_zval_ref_type(jit, ref, ir_const_u8(&jit->ctx, IS_REFERENCE));
				zend_jit_if_true(jit, if_ref);
				ref2 = zend_jit_zval_ref_ptr(jit, ref);

				if_typed = zend_jit_if_typed_ref(jit, ref2);
				zend_jit_if_true(jit, if_typed);

				if (RETURN_VALUE_USED(opline)) {
//????					if (Z_MODE(res_addr) == IS_REG) {
//????						arg2 = zend_jit_zval_addr(jit, ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var));
//????					} else {
						arg2 = zend_jit_zval_addr(jit, res_addr);
//????					}
				} else {
					arg2 = IR_NULL;
				}
				if (opline->opcode == ZEND_PRE_INC) {
					func = zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_pre_inc_typed_ref, IR_CONST_FASTCALL_FUNC);
				} else if (opline->opcode == ZEND_PRE_DEC) {
					func = zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_pre_dec_typed_ref, IR_CONST_FASTCALL_FUNC);
				} else if (opline->opcode == ZEND_POST_INC) {
					func = zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_post_inc_typed_ref, IR_CONST_FASTCALL_FUNC);
				} else if (opline->opcode == ZEND_POST_DEC) {
					func = zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_post_dec_typed_ref, IR_CONST_FASTCALL_FUNC);
				} else {
					ZEND_UNREACHABLE();
				}
				// TODO: check guard ???
//????				if (RETURN_VALUE_USED(opline) && Z_MODE(res_addr) == IS_REG) {
//????					zend_jit_addr real_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var);
//????					if (!zend_jit_load_reg(jit, real_addr, res_addr, res_info)) {
//????						return 0;
//????					}
//????				}
				zend_jit_call_2(jit, IR_VOID, func, ref2, arg2);
				zend_jit_check_exception(jit);
				typed_ref_path = zend_jit_end(jit);

				zend_jit_if_false(jit, if_typed);
				ref2 = zend_jit_addr_offset(jit, ref2, offsetof(zend_reference, val));
				ref_path = zend_jit_end(jit);
				zend_jit_if_false(jit, if_ref);
				zend_jit_merge_2(jit, ref_path, zend_jit_end(jit));
				ref = zend_jit_phi_2(jit, ref2, ref);
			}

			if (opline->opcode == ZEND_POST_INC || opline->opcode == ZEND_POST_DEC) {
				zend_jit_copy_zval(jit,
					res_addr,
					res_use_info,
					ZEND_ADDR_REF_ZVAL(ref), op1_info, 1);
			}
			if (opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_POST_INC) {
				if (opline->opcode == ZEND_PRE_INC && opline->result_type != IS_UNUSED) {
					ir_ref arg2 = zend_jit_zval_addr(jit, res_addr);
					zend_jit_call_2(jit, IR_VOID,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_pre_inc, IR_CONST_FASTCALL_FUNC),
						ref, arg2);
				} else {
					zend_jit_call_1(jit, IR_VOID,
						zend_jit_const_func_addr(jit, (uintptr_t)increment_function, IR_CONST_FASTCALL_FUNC),
						ref);
				}
			} else {
				if (opline->opcode == ZEND_PRE_DEC && opline->result_type != IS_UNUSED) {
					ir_ref arg2 = zend_jit_zval_addr(jit, res_addr);
					zend_jit_call_2(jit, IR_VOID,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_pre_dec, IR_CONST_FASTCALL_FUNC),
						ref, arg2);
				} else {
					zend_jit_call_1(jit, IR_VOID,
						zend_jit_const_func_addr(jit, (uintptr_t)decrement_function, IR_CONST_FASTCALL_FUNC),
						ref);
				}
			}
			if (may_throw) {
				zend_jit_check_exception(jit);
			}
		} else {
			ref = zend_jit_zval_dval(jit, op1_addr);
			if (opline->opcode == ZEND_POST_INC || opline->opcode == ZEND_POST_DEC) {
				zend_jit_zval_set_dval(jit, res_addr, ref);
				zend_jit_zval_set_type_info(jit, res_addr, IS_DOUBLE);
			}
			if (opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_POST_INC) {
				op = IR_ADD;
			} else {
				op = IR_SUB;
			}
			ref = ir_fold2(&jit->ctx, IR_OPT(op, IR_DOUBLE), ref,
				ir_const_double(&jit->ctx, 1.0));
			zend_jit_zval_set_dval(jit, op1_def_addr, ref);
			if ((opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_PRE_DEC) &&
			    opline->result_type != IS_UNUSED) {
				zend_jit_zval_set_dval(jit, res_addr, ref);
				zend_jit_zval_set_type_info(jit, res_addr, IS_DOUBLE);
			}
		}
		if (typed_ref_path) {
			zend_jit_merge_3(jit, fast_path, zend_jit_end(jit), typed_ref_path);
		} else {
			zend_jit_merge_2(jit, fast_path, zend_jit_end(jit));
		}
	}
	if (!zend_jit_store_var_if_necessary_ex(jit, opline->op1.var, op1_def_addr, op1_def_info, op1_addr, op1_info)) {
		return 0;
	}
	if (opline->result_type != IS_UNUSED) {
		if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, res_info)) {
			return 0;
		}
	}
	return 1;
}

static int zend_jit_math_long_long(zend_jit_ctx   *jit,
                                   const zend_op  *opline,
                                   zend_uchar      opcode,
                                   zend_jit_addr   op1_addr,
                                   zend_jit_addr   op2_addr,
                                   zend_jit_addr   res_addr,
                                   uint32_t        res_info,
                                   uint32_t        res_use_info,
                                   int             may_overflow)
{
	bool same_ops = zend_jit_same_addr(op1_addr, op2_addr);
	ir_op op;
	ir_ref op1, op2, ref, if_overflow = IR_UNUSED;

	if (opcode == ZEND_ADD) {
		op = may_overflow ? IR_ADD_OV : IR_ADD;
	} else if (opcode == ZEND_SUB) {
		op = may_overflow ? IR_SUB_OV : IR_SUB;
	} else if (opcode == ZEND_MUL) {
		op = may_overflow ? IR_MUL_OV : IR_MUL;
	} else {
		ZEND_ASSERT(0);
	}
	op1 = zend_jit_zval_lval(jit, op1_addr);
	op2 = (same_ops) ? op1 : zend_jit_zval_lval(jit, op2_addr);
	ref = ir_fold2(&jit->ctx, IR_OPT(op, IR_PHP_LONG), op1, op2);

	if (may_overflow) {
		if (res_info & MAY_BE_GUARD) {
			int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);
			const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

			if (!exit_addr) {
				return 0;
			}
			if ((res_info & MAY_BE_ANY) == MAY_BE_LONG) {
				zend_jit_guard_not(jit,
					ir_fold1(&jit->ctx, IR_OPT(IR_OVERFLOW, IR_BOOL), ref),
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				may_overflow = 0;
			} else if ((res_info & MAY_BE_ANY) == MAY_BE_DOUBLE) {
				zend_jit_guard(jit,
					ir_fold1(&jit->ctx, IR_OPT(IR_OVERFLOW, IR_BOOL), ref),
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				ZEND_UNREACHABLE();
			}
		} else {
			if_overflow = zend_jit_if(jit, ir_fold1(&jit->ctx, IR_OPT(IR_OVERFLOW, IR_BOOL), ref));
			zend_jit_if_false(jit, if_overflow);
		}
	}

	if ((res_info & MAY_BE_ANY) != MAY_BE_DOUBLE) {
		zend_jit_zval_set_lval(jit, res_addr, ref);

		if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
			if (Z_MODE(op1_addr) != IS_MEM_ZVAL || !zend_jit_same_addr(op1_addr, res_addr)) {
				if ((res_use_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF|MAY_BE_GUARD)) != MAY_BE_LONG) {
					zend_jit_zval_set_type_info(jit, res_addr, IS_LONG);
				}
			}
		}
	}

	if (may_overflow) {
		ir_ref fast_path = IR_UNUSED;

		if ((res_info & MAY_BE_ANY) != MAY_BE_DOUBLE) {
			fast_path = zend_jit_end(jit);
			zend_jit_if_true_cold(jit, if_overflow);
		}
		if (opcode == ZEND_ADD) {
			if (Z_MODE(op2_addr) == IS_CONST_ZVAL && Z_LVAL_P(Z_ZV(op2_addr)) == 1) {
				if (Z_MODE(res_addr) == IS_REG) {
					zend_jit_zval_set_dval(jit, res_addr,
						ir_const_double(&jit->ctx, (double)ZEND_LONG_MAX + 1.0));
				} else {
#if SIZEOF_ZEND_LONG == 4
					zend_jit_zval_set_lval(jit, res_addr,
						ir_const_php_long(&jit->ctx, 0));
					zend_jit_zval_set_w2(jit, res_addr,
						ir_const_u32(&jit->ctx, 0x41e00000));
#else
					zend_jit_zval_set_lval(jit, res_addr,
						ir_const_php_long(&jit->ctx, 0x43e0000000000000));
#endif
					zend_jit_zval_set_type_info(jit, res_addr, IS_DOUBLE);
				}
				if ((res_info & MAY_BE_ANY) != MAY_BE_DOUBLE) {
					zend_jit_merge_2(jit, fast_path, zend_jit_end(jit));
				}
				return 1;
			}
			op = IR_ADD;
		} else if (opcode == ZEND_SUB) {
			if (Z_MODE(op2_addr) == IS_CONST_ZVAL && Z_LVAL_P(Z_ZV(op2_addr)) == 1) {
				if (Z_MODE(res_addr) == IS_REG) {
					zend_jit_zval_set_dval(jit, res_addr,
						ir_const_double(&jit->ctx, (double)ZEND_LONG_MIN - 1.0));
				} else {
#if SIZEOF_ZEND_LONG == 4
					zend_jit_zval_set_lval(jit, res_addr,
						ir_const_php_long(&jit->ctx, 0x00200000));
					zend_jit_zval_set_w2(jit, res_addr,
						ir_const_u32(&jit->ctx, 0xc1e00000));
#else
					zend_jit_zval_set_lval(jit, res_addr,
						ir_const_php_long(&jit->ctx, 0xc3e0000000000000));
#endif
					zend_jit_zval_set_type_info(jit, res_addr, IS_DOUBLE);
				}
				if ((res_info & MAY_BE_ANY) != MAY_BE_DOUBLE) {
					zend_jit_merge_2(jit, fast_path, zend_jit_end(jit));
				}
				return 1;
			}
			op = IR_SUB;
		} else if (opcode == ZEND_MUL) {
			op = IR_MUL;
		} else {
			ZEND_ASSERT(0);
		}
#if 1
		/* reload */
		op1 = zend_jit_zval_lval(jit, op1_addr);
		op2 = (same_ops) ? op1 : zend_jit_zval_lval(jit, op2_addr);
#endif
		ref = ir_fold2(&jit->ctx, IR_OPT(op, IR_DOUBLE),
			ir_fold1(&jit->ctx, IR_OPT(IR_INT2FP, IR_DOUBLE), op1),
			ir_fold1(&jit->ctx, IR_OPT(IR_INT2FP, IR_DOUBLE), op2));
		zend_jit_zval_set_dval(jit, res_addr, ref);
		if (Z_MODE(res_addr) != IS_REG) {
			zend_jit_zval_set_type_info(jit, res_addr, IS_DOUBLE);
		}
		if ((res_info & MAY_BE_ANY) != MAY_BE_DOUBLE) {
			zend_jit_merge_2(jit, fast_path, zend_jit_end(jit));
		}
	}

#if 0
//???
	zend_reg result_reg;
	zend_reg tmp_reg = ZREG_R0;

	if (Z_MODE(res_addr) == IS_REG && (res_info & MAY_BE_LONG)) {
		if (may_overflow && (res_info & MAY_BE_GUARD)
		 && JIT_G(current_frame)
		 && zend_jit_opline_uses_reg(opline, Z_REG(res_addr))) {
			result_reg = ZREG_R0;
		} else {
			result_reg = Z_REG(res_addr);
		}
	} else if (Z_MODE(op1_addr) == IS_REG && Z_LAST_USE(op1_addr) && !may_overflow) {
		result_reg = Z_REG(op1_addr);
	} else if (Z_REG(res_addr) != ZREG_R0) {
		result_reg = ZREG_R0;
	} else {
		/* ASSIGN_DIM_OP */
		result_reg = ZREG_FCARG1;
		tmp_reg = ZREG_FCARG1;
	}

	if (opcode == ZEND_MUL &&
			Z_MODE(op2_addr) == IS_CONST_ZVAL &&
			Z_LVAL_P(Z_ZV(op2_addr)) == 2) {
		if (Z_MODE(op1_addr) == IS_REG && !may_overflow) {
			|	lea Ra(result_reg), [Ra(Z_REG(op1_addr))+Ra(Z_REG(op1_addr))]
		} else {
			|	GET_ZVAL_LVAL result_reg, op1_addr
			|	add Ra(result_reg), Ra(result_reg)
		}
	} else if (opcode == ZEND_MUL &&
			Z_MODE(op2_addr) == IS_CONST_ZVAL &&
			!may_overflow &&
			Z_LVAL_P(Z_ZV(op2_addr)) > 0 &&
			zend_long_is_power_of_two(Z_LVAL_P(Z_ZV(op2_addr)))) {
		|	GET_ZVAL_LVAL result_reg, op1_addr
		|	shl Ra(result_reg), zend_long_floor_log2(Z_LVAL_P(Z_ZV(op2_addr)))
	} else if (opcode == ZEND_MUL &&
			Z_MODE(op1_addr) == IS_CONST_ZVAL &&
			Z_LVAL_P(Z_ZV(op1_addr)) == 2) {
		if (Z_MODE(op2_addr) == IS_REG && !may_overflow) {
			|	lea Ra(result_reg), [Ra(Z_REG(op2_addr))+Ra(Z_REG(op2_addr))]
		} else {
			|	GET_ZVAL_LVAL result_reg, op2_addr
			|	add Ra(result_reg), Ra(result_reg)
		}
	} else if (opcode == ZEND_MUL &&
			Z_MODE(op1_addr) == IS_CONST_ZVAL &&
			!may_overflow &&
			Z_LVAL_P(Z_ZV(op1_addr)) > 0 &&
			zend_long_is_power_of_two(Z_LVAL_P(Z_ZV(op1_addr)))) {
		|	GET_ZVAL_LVAL result_reg, op2_addr
		|	shl Ra(result_reg), zend_long_floor_log2(Z_LVAL_P(Z_ZV(op1_addr)))
	} else if (opcode == ZEND_DIV &&
			(Z_MODE(op2_addr) == IS_CONST_ZVAL &&
			zend_long_is_power_of_two(Z_LVAL_P(Z_ZV(op2_addr))))) {
		|	GET_ZVAL_LVAL result_reg, op1_addr
		|	shr Ra(result_reg), zend_long_floor_log2(Z_LVAL_P(Z_ZV(op2_addr)))
	} else if (opcode == ZEND_ADD &&
			!may_overflow &&
			Z_MODE(op1_addr) == IS_REG &&
			Z_MODE(op2_addr) == IS_CONST_ZVAL &&
			IS_SIGNED_32BIT(Z_LVAL_P(Z_ZV(op2_addr)))) {
		|	lea Ra(result_reg), [Ra(Z_REG(op1_addr))+Z_LVAL_P(Z_ZV(op2_addr))]
	} else if (opcode == ZEND_ADD &&
			!may_overflow &&
			Z_MODE(op2_addr) == IS_REG &&
			Z_MODE(op1_addr) == IS_CONST_ZVAL &&
			IS_SIGNED_32BIT(Z_LVAL_P(Z_ZV(op1_addr)))) {
		|	lea Ra(result_reg), [Ra(Z_REG(op2_addr))+Z_LVAL_P(Z_ZV(op1_addr))]
	} else if (opcode == ZEND_SUB &&
			!may_overflow &&
			Z_MODE(op1_addr) == IS_REG &&
			Z_MODE(op2_addr) == IS_CONST_ZVAL &&
			IS_SIGNED_32BIT(-Z_LVAL_P(Z_ZV(op2_addr)))) {
		|	lea Ra(result_reg), [Ra(Z_REG(op1_addr))-Z_LVAL_P(Z_ZV(op2_addr))]
	} else {
		|	GET_ZVAL_LVAL result_reg, op1_addr
		if ((opcode == ZEND_ADD || opcode == ZEND_SUB)
		 && Z_MODE(op2_addr) == IS_CONST_ZVAL
		 && Z_LVAL_P(Z_ZV(op2_addr)) == 0) {
			/* +/- 0 */
			may_overflow = 0;
		} else if (same_ops && opcode != ZEND_DIV) {
			|	LONG_MATH_REG opcode, Ra(result_reg), Ra(result_reg)
		} else {
			zend_reg tmp_reg;

			if (Z_MODE(res_addr) == IS_MEM_ZVAL && Z_REG(res_addr) == ZREG_R0) {
				tmp_reg = ZREG_R1;
			} else if (result_reg != ZREG_R0) {
				tmp_reg = ZREG_R0;
			} else {
				tmp_reg = ZREG_R1;
			}
			|	LONG_MATH opcode, result_reg, op2_addr, tmp_reg
			(void)tmp_reg;
		}
	}
	if (may_overflow) {
		if (res_info & MAY_BE_GUARD) {
			int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);
			const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);
			if ((res_info & MAY_BE_ANY) == MAY_BE_LONG) {
				|	jo &exit_addr
				if (Z_MODE(res_addr) == IS_REG && result_reg != Z_REG(res_addr)) {
					|	mov Ra(Z_REG(res_addr)), Ra(result_reg)
				}
			} else if ((res_info & MAY_BE_ANY) == MAY_BE_DOUBLE) {
				|	jno &exit_addr
			} else {
				ZEND_UNREACHABLE();
			}
		} else {
			if (res_info & MAY_BE_LONG) {
				|	jo >1
			} else {
				|	jno >1
			}
		}
	}

	if (Z_MODE(res_addr) == IS_MEM_ZVAL && (res_info & MAY_BE_LONG)) {
		|	SET_ZVAL_LVAL res_addr, Ra(result_reg)
		if (Z_MODE(op1_addr) != IS_MEM_ZVAL || Z_REG(op1_addr) != Z_REG(res_addr) || Z_OFFSET(op1_addr) != Z_OFFSET(res_addr)) {
			if ((res_use_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF|MAY_BE_GUARD)) != MAY_BE_LONG) {
				|	SET_ZVAL_TYPE_INFO res_addr, IS_LONG
			}
		}
	}

	if (may_overflow && (!(res_info & MAY_BE_GUARD) || (res_info & MAY_BE_ANY) == MAY_BE_DOUBLE)) {
		zend_reg tmp_reg1 = ZREG_XMM0;
		zend_reg tmp_reg2 = ZREG_XMM1;

		if (res_info & MAY_BE_LONG) {
			|.cold_code
			|1:
		}

		do {
			if ((sizeof(void*) == 8 || Z_MODE(res_addr) != IS_REG) &&
			    ((Z_MODE(op1_addr) == IS_CONST_ZVAL && Z_LVAL_P(Z_ZV(op1_addr)) == 1) ||
			     (Z_MODE(op2_addr) == IS_CONST_ZVAL && Z_LVAL_P(Z_ZV(op2_addr)) == 1))) {
				if (opcode == ZEND_ADD) {
					|.if X64
						|	mov64 Ra(tmp_reg), 0x43e0000000000000
						if (Z_MODE(res_addr) == IS_REG) {
							|	movd xmm(Z_REG(res_addr)-ZREG_XMM0), Ra(tmp_reg)
						} else {
							|	SET_ZVAL_LVAL res_addr, Ra(tmp_reg)
						}
					|.else
						|	SET_ZVAL_LVAL res_addr, 0
						|	SET_ZVAL_W2 res_addr, 0x41e00000
					|.endif
					break;
				} else if (opcode == ZEND_SUB) {
					|.if X64
						|	mov64 Ra(tmp_reg), 0xc3e0000000000000
						if (Z_MODE(res_addr) == IS_REG) {
							|	movd xmm(Z_REG(res_addr)-ZREG_XMM0), Ra(tmp_reg)
						} else {
							|	SET_ZVAL_LVAL res_addr, Ra(tmp_reg)
						}
					|.else
						|	SET_ZVAL_LVAL res_addr, 0x00200000
						|	SET_ZVAL_W2 res_addr, 0xc1e00000
					|.endif
					break;
				}
			}

			|	DOUBLE_GET_ZVAL_LVAL tmp_reg1, op1_addr, tmp_reg
			|	DOUBLE_GET_ZVAL_LVAL tmp_reg2, op2_addr, tmp_reg
			if (CAN_USE_AVX()) {
				|	AVX_MATH_REG opcode, tmp_reg1, tmp_reg1, tmp_reg2
			} else {
				|	SSE_MATH_REG opcode, tmp_reg1, tmp_reg2
			}
			|	DOUBLE_SET_ZVAL_DVAL res_addr, tmp_reg1
		} while (0);

		if (Z_MODE(res_addr) == IS_MEM_ZVAL
		 && (res_use_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF|MAY_BE_GUARD)) != MAY_BE_DOUBLE) {
			|	SET_ZVAL_TYPE_INFO res_addr, IS_DOUBLE
		}
		if (res_info & MAY_BE_LONG) {
			|	jmp >2
			|.code
		}
		|2:
	}
#endif
	return 1;
}

static int zend_jit_math_long_double(zend_jit_ctx   *jit,
                                     zend_uchar      opcode,
                                     zend_jit_addr   op1_addr,
                                     zend_jit_addr   op2_addr,
                                     zend_jit_addr   res_addr,
                                     uint32_t        res_use_info)
{
	ir_op op;
	ir_ref op1, op2, ref;

	if (opcode == ZEND_ADD) {
		op = IR_ADD;
	} else if (opcode == ZEND_SUB) {
		op = IR_SUB;
	} else if (opcode == ZEND_MUL) {
		op = IR_MUL;
	} else if (opcode == ZEND_DIV) {
		op = IR_DIV;
	} else {
		ZEND_ASSERT(0);
	}
	op1 = zend_jit_zval_lval(jit, op1_addr);
	op2 = zend_jit_zval_dval(jit, op2_addr);
	ref = ir_fold2(&jit->ctx, IR_OPT(op, IR_DOUBLE),
		ir_fold1(&jit->ctx, IR_OPT(IR_INT2FP, IR_DOUBLE), op1), op2);
	zend_jit_zval_set_dval(jit, res_addr, ref);

	if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
		if ((res_use_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF|MAY_BE_GUARD)) != MAY_BE_DOUBLE) {
			zend_jit_zval_set_type_info(jit, res_addr, IS_DOUBLE);
		}
	}
	return 1;
}

static int zend_jit_math_double_long(zend_jit_ctx   *jit,
                                     zend_uchar      opcode,
                                     zend_jit_addr   op1_addr,
                                     zend_jit_addr   op2_addr,
                                     zend_jit_addr   res_addr,
                                     uint32_t        res_use_info)
{
	ir_op op;
	ir_ref op1, op2, ref;

	if (opcode == ZEND_ADD) {
		op = IR_ADD;
	} else if (opcode == ZEND_SUB) {
		op = IR_SUB;
	} else if (opcode == ZEND_MUL) {
		op = IR_MUL;
	} else if (opcode == ZEND_DIV) {
		op = IR_DIV;
	} else {
		ZEND_ASSERT(0);
	}
	op1 = zend_jit_zval_dval(jit, op1_addr);
	op2 = zend_jit_zval_lval(jit, op2_addr);
	ref = ir_fold2(&jit->ctx, IR_OPT(op, IR_DOUBLE), op1,
		ir_fold1(&jit->ctx, IR_OPT(IR_INT2FP, IR_DOUBLE), op2));
	zend_jit_zval_set_dval(jit, res_addr, ref);

	if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
		if (Z_MODE(op1_addr) != IS_MEM_ZVAL || !zend_jit_same_addr(op1_addr, res_addr)) {
			if ((res_use_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF|MAY_BE_GUARD)) != MAY_BE_DOUBLE) {
				zend_jit_zval_set_type_info(jit, res_addr, IS_DOUBLE);
			}
		}
	}
	return 1;
}

static int zend_jit_math_double_double(zend_jit_ctx   *jit,
                                       zend_uchar      opcode,
                                       zend_jit_addr   op1_addr,
                                       zend_jit_addr   op2_addr,
                                       zend_jit_addr   res_addr,
                                       uint32_t        res_use_info)
{
	bool same_ops = zend_jit_same_addr(op1_addr, op2_addr);
	ir_op op;
	ir_ref op1, op2, ref;

	if (opcode == ZEND_ADD) {
		op = IR_ADD;
	} else if (opcode == ZEND_SUB) {
		op = IR_SUB;
	} else if (opcode == ZEND_MUL) {
		op = IR_MUL;
	} else if (opcode == ZEND_DIV) {
		op = IR_DIV;
	} else {
		ZEND_ASSERT(0);
	}
	op1 = zend_jit_zval_dval(jit, op1_addr);
	op2 = (same_ops) ? op1 : zend_jit_zval_dval(jit, op2_addr);
	ref = ir_fold2(&jit->ctx, IR_OPT(op, IR_DOUBLE), op1, op2);
	zend_jit_zval_set_dval(jit, res_addr, ref);

	if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
		if (Z_MODE(op1_addr) != IS_MEM_ZVAL || !zend_jit_same_addr(op1_addr, res_addr)) {
			if ((res_use_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF|MAY_BE_GUARD)) != MAY_BE_DOUBLE) {
				zend_jit_zval_set_type_info(jit, res_addr, IS_DOUBLE);
			}
		}
	}
	return 1;
}

static int zend_jit_math_helper(zend_jit_ctx   *jit,
                                const zend_op  *opline,
                                zend_uchar      opcode,
                                zend_uchar      op1_type,
                                znode_op        op1,
                                zend_jit_addr   op1_addr,
                                uint32_t        op1_info,
                                zend_uchar      op2_type,
                                znode_op        op2,
                                zend_jit_addr   op2_addr,
                                uint32_t        op2_info,
                                uint32_t        res_var,
                                zend_jit_addr   res_addr,
                                uint32_t        res_info,
                                uint32_t        res_use_info,
                                int             may_overflow,
                                int             may_throw)
{
	ir_ref if_op1_long = IR_UNUSED;
	ir_ref if_op1_double = IR_UNUSED;
	ir_ref if_op2_double = IR_UNUSED;
	ir_ref if_op1_long_op2_long = IR_UNUSED;
	ir_ref if_op1_long_op2_double = IR_UNUSED;
	ir_ref if_op1_double_op2_double = IR_UNUSED;
	ir_ref if_op1_double_op2_long = IR_UNUSED;
	ir_ref end_inputs[5], slow_inputs[4], res_inputs[5];
	uint32_t slow_inputs_count = 0;
	uint32_t end_inputs_count = 0;
	bool same_ops = zend_jit_same_addr(op1_addr, op2_addr);

	if (Z_MODE(res_addr) == IS_REG) {
		jit->delay_var = Z_SSA_VAR(res_addr);
		jit->delay_buf = res_inputs;
		jit->delay_count = 0;
	}

	if ((res_info & MAY_BE_GUARD) && (res_info & MAY_BE_LONG) && (op1_info & MAY_BE_LONG) && (op2_info & MAY_BE_LONG)) {
		if (op1_info & (MAY_BE_ANY-MAY_BE_LONG)) {
			if_op1_long = zend_jit_if_zval_type(jit, op1_addr, IS_LONG);
			zend_jit_if_true(jit, if_op1_long);
		}
		if (!same_ops && (op2_info & (MAY_BE_ANY-MAY_BE_LONG))) {
			if_op1_long_op2_long = zend_jit_if_zval_type(jit, op2_addr, IS_LONG);
			zend_jit_if_true(jit, if_op1_long_op2_long);
		}
		if (!zend_jit_math_long_long(jit, opline, opcode, op1_addr, op2_addr, res_addr, res_info, res_use_info, may_overflow)) {
			return 0;
		}
		zend_jit_array_add(end_inputs, zend_jit_end(jit));
		if (if_op1_long) {
			zend_jit_if_false_cold(jit, if_op1_long);
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));
		}
		if (if_op1_long_op2_long) {
			zend_jit_if_false_cold(jit, if_op1_long_op2_long);
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));
		}
	} else if ((op1_info & MAY_BE_LONG) && (op2_info & MAY_BE_LONG) && (res_info & (MAY_BE_LONG|MAY_BE_DOUBLE))) {
		if (op1_info & (MAY_BE_ANY-MAY_BE_LONG)) {
			if_op1_long = zend_jit_if_zval_type(jit, op1_addr, IS_LONG);
			zend_jit_if_true(jit, if_op1_long);
		}
		if (!same_ops && (op2_info & (MAY_BE_ANY-MAY_BE_LONG))) {
			if_op1_long_op2_long = zend_jit_if_zval_type(jit, op2_addr, IS_LONG);
			zend_jit_if_false_cold(jit, if_op1_long_op2_long);
			if (op2_info & MAY_BE_DOUBLE) {
				if (op2_info & (MAY_BE_ANY-(MAY_BE_LONG|MAY_BE_DOUBLE))) {
					if_op1_long_op2_double = zend_jit_if_zval_type(jit, op2_addr, IS_DOUBLE);
					zend_jit_if_false_cold(jit, if_op1_long_op2_double);
					zend_jit_array_add(slow_inputs, zend_jit_end(jit));
					zend_jit_if_true(jit, if_op1_long_op2_double);
				}
				if (!zend_jit_math_long_double(jit, opcode, op1_addr, op2_addr, res_addr, res_use_info)) {
					return 0;
				}
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			} else {
				zend_jit_array_add(slow_inputs, zend_jit_end(jit));
			}
			zend_jit_if_true(jit, if_op1_long_op2_long);
		}
		if (!zend_jit_math_long_long(jit, opline, opcode, op1_addr, op2_addr, res_addr, res_info, res_use_info, may_overflow)) {
			return 0;
		}
		zend_jit_array_add(end_inputs, zend_jit_end(jit));

		if (if_op1_long) {
			zend_jit_if_false_cold(jit, if_op1_long);
		}

		if (op1_info & MAY_BE_DOUBLE) {
			if (op1_info & (MAY_BE_ANY-(MAY_BE_LONG|MAY_BE_DOUBLE))) {
				if_op1_double = zend_jit_if_zval_type(jit, op1_addr, IS_DOUBLE);
				zend_jit_if_false_cold(jit, if_op1_double);
				zend_jit_array_add(slow_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_op1_double);
			}
			if (op2_info & MAY_BE_DOUBLE) {
				if (!same_ops && (op2_info & (MAY_BE_ANY-MAY_BE_DOUBLE))) {
					if_op1_double_op2_double = zend_jit_if_zval_type(jit, op2_addr, IS_DOUBLE);
					zend_jit_if_true(jit, if_op1_double_op2_double);
				}
				if (!zend_jit_math_double_double(jit, opcode, op1_addr, op2_addr, res_addr, res_use_info)) {
					return 0;
				}
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
				if (if_op1_double_op2_double) {
					zend_jit_if_false_cold(jit, if_op1_double_op2_double);
				}
			}
			if (!same_ops) {
				if (op2_info & (MAY_BE_ANY-(MAY_BE_LONG|MAY_BE_DOUBLE))) {
					if_op1_double_op2_long = zend_jit_if_zval_type(jit, op2_addr, IS_LONG);
					zend_jit_if_false_cold(jit, if_op1_double_op2_long);
					zend_jit_array_add(slow_inputs, zend_jit_end(jit));
					zend_jit_if_true(jit, if_op1_double_op2_long);
				}
				if (!zend_jit_math_double_long(jit, opcode, op1_addr, op2_addr, res_addr, res_use_info)) {
					return 0;
				}
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			} else if (if_op1_double_op2_double) {
				zend_jit_array_add(slow_inputs, zend_jit_end(jit));
			}
		} else if (if_op1_long) {
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));
		}
	} else if ((op1_info & MAY_BE_DOUBLE) &&
	           !(op1_info & MAY_BE_LONG) &&
	           (op2_info & (MAY_BE_LONG|MAY_BE_DOUBLE)) &&
	           (res_info & MAY_BE_DOUBLE)) {
		if (op1_info & (MAY_BE_ANY-MAY_BE_DOUBLE)) {
			if_op1_double = zend_jit_if_zval_type(jit, op1_addr, IS_DOUBLE);
			zend_jit_if_false_cold(jit, if_op1_double);
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));
			zend_jit_if_true(jit, if_op1_double);
		}
		if (op2_info & MAY_BE_DOUBLE) {
			if (!same_ops && (op2_info & (MAY_BE_ANY-MAY_BE_DOUBLE))) {
				if_op1_double_op2_double = zend_jit_if_zval_type(jit, op2_addr, IS_DOUBLE);
				zend_jit_if_true(jit, if_op1_double_op2_double);
			}
			if (!zend_jit_math_double_double(jit, opcode, op1_addr, op2_addr, res_addr, res_use_info)) {
				return 0;
			}
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			if (if_op1_double_op2_double) {
				zend_jit_if_false_cold(jit, if_op1_double_op2_double);
			}
		}
		if (!same_ops && (op2_info & MAY_BE_LONG)) {
			if (op2_info & (MAY_BE_ANY-(MAY_BE_DOUBLE|MAY_BE_LONG))) {
				if_op1_double_op2_long = zend_jit_if_zval_type(jit, op2_addr, IS_LONG);
				zend_jit_if_false_cold(jit, if_op1_double_op2_long);
				zend_jit_array_add(slow_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_op1_double_op2_long);
			}
			if (!zend_jit_math_double_long(jit, opcode, op1_addr, op2_addr, res_addr, res_use_info)) {
				return 0;
			}
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		} else if (if_op1_double_op2_double) {
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));
		}
	} else if ((op2_info & MAY_BE_DOUBLE) &&
	           !(op2_info & MAY_BE_LONG) &&
	           (op1_info & (MAY_BE_LONG|MAY_BE_DOUBLE)) &&
	           (res_info & MAY_BE_DOUBLE)) {
		if (op2_info & (MAY_BE_ANY-MAY_BE_DOUBLE)) {
			if_op2_double = zend_jit_if_zval_type(jit, op2_addr, IS_DOUBLE);
			zend_jit_if_false_cold(jit, if_op2_double);
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));
			zend_jit_if_true(jit, if_op2_double);
		}
		if (op1_info & MAY_BE_DOUBLE) {
			if (!same_ops && (op1_info & (MAY_BE_ANY-MAY_BE_DOUBLE))) {
				if_op1_double_op2_double = zend_jit_if_zval_type(jit, op1_addr, IS_DOUBLE);
				zend_jit_if_true(jit, if_op1_double_op2_double);
			}
			if (!zend_jit_math_double_double(jit, opcode, op1_addr, op2_addr, res_addr, res_use_info)) {
				return 0;
			}
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			if (if_op1_double_op2_double) {
				zend_jit_if_false_cold(jit, if_op1_double_op2_double);
			}
		}
		if (!same_ops && (op1_info & MAY_BE_LONG)) {
			if (op1_info & (MAY_BE_ANY-(MAY_BE_DOUBLE|MAY_BE_LONG))) {
				if_op1_long_op2_double = zend_jit_if_zval_type(jit, op1_addr, IS_LONG);
				zend_jit_if_false_cold(jit, if_op1_long_op2_double);
				zend_jit_array_add(slow_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_op1_long_op2_double);
			}
			if (!zend_jit_math_long_double(jit, opcode, op1_addr, op2_addr, res_addr, res_use_info)) {
				return 0;
			}
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		} else if (if_op1_double_op2_double) {
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));
		}
	}

	if ((op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) ||
		(op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE)))) {
		ir_ref func, arg1, arg2, arg3;

		if (slow_inputs_count) {
			zend_jit_merge_N(jit, slow_inputs_count, slow_inputs);
		}

		if (Z_MODE(op1_addr) == IS_REG) {
			zend_jit_addr real_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, op1.var);
			if (!zend_jit_spill_store(jit, op1_addr, real_addr, op1_info, 1)) {
				return 0;
			}
			op1_addr = real_addr;
		}
		if (Z_MODE(op2_addr) == IS_REG) {
			zend_jit_addr real_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, op2.var);
			if (!zend_jit_spill_store(jit, op2_addr, real_addr, op2_info, 1)) {
				return 0;
			}
			op2_addr = real_addr;
		}
		if (Z_MODE(res_addr) == IS_REG) {
			arg1 = zend_jit_zval_addr(jit, ZEND_ADDR_MEM_ZVAL(ZREG_FP, res_var));
		} else {
			arg1 = zend_jit_zval_addr(jit, res_addr);
		}
		arg2 = zend_jit_zval_addr(jit, op1_addr);
		arg3 = zend_jit_zval_addr(jit, op2_addr);
		zend_jit_set_ex_opline(jit, opline);
		if (opcode == ZEND_ADD) {
			func = zend_jit_const_func_addr(jit, (uintptr_t)add_function, IR_CONST_FASTCALL_FUNC);
		} else if (opcode == ZEND_SUB) {
			func = zend_jit_const_func_addr(jit, (uintptr_t)sub_function, IR_CONST_FASTCALL_FUNC);
		} else if (opcode == ZEND_MUL) {
			func = zend_jit_const_func_addr(jit, (uintptr_t)mul_function, IR_CONST_FASTCALL_FUNC);
		} else if (opcode == ZEND_DIV) {
			func = zend_jit_const_func_addr(jit, (uintptr_t)div_function, IR_CONST_FASTCALL_FUNC);
		} else {
			ZEND_UNREACHABLE();
		}
		zend_jit_call_3(jit, IR_VOID, func, arg1, arg2, arg3);

		zend_jit_free_op(jit, op1_type, op1, op1_info, NULL);
		zend_jit_free_op(jit, op2_type, op2, op2_info, NULL);

		if (may_throw) {
			if (opline->opcode == ZEND_ASSIGN_DIM_OP && (opline->op2_type & (IS_VAR|IS_TMP_VAR))) {
				zend_jit_guard_not(jit,
					zend_jit_load(jit, IR_ADDR,
						zend_jit_eg_exception_addr(jit)),
					zend_jit_stub_addr(jit, jit_stub_exception_handler_free_op2));
			} else if (Z_MODE(res_addr) == IS_MEM_ZVAL && Z_REG(res_addr) == ZREG_RX) {
				zend_jit_check_exception_undef_result(jit, opline);
			} else {
				zend_jit_check_exception(jit);
			}
		}
		if (Z_MODE(res_addr) == IS_REG) {
			zend_jit_addr real_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, res_var);
			if (!zend_jit_load_reg(jit, real_addr, res_addr, res_info)) {
				return 0;
			}
		}
		zend_jit_array_add(end_inputs, zend_jit_end(jit));
	}

	if (end_inputs_count) {
		zend_jit_merge_N(jit, end_inputs_count, end_inputs);
	}

	if (Z_MODE(res_addr) == IS_REG) {
		int count = jit->delay_count;

		jit->delay_var = -1;
		jit->delay_buf = NULL;
		jit->delay_count = 0;
		ZEND_ASSERT(end_inputs_count == count);
		if (count == 1) {
			zend_jit_def_reg(jit, res_addr, res_inputs[0]);
		} else {
			ir_ref phi = zend_jit_phi_N(jit, count, res_inputs);
			zend_jit_def_reg(jit, res_addr, phi);
		}
	}

	return 1;
}

static int zend_jit_math(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, uint32_t op2_info, zend_jit_addr op2_addr, uint32_t res_use_info, uint32_t res_info, zend_jit_addr res_addr, int may_overflow, int may_throw)
{
	ZEND_ASSERT(!(op1_info & MAY_BE_UNDEF) && !(op2_info & MAY_BE_UNDEF));

	if (!zend_jit_math_helper(jit, opline, opline->opcode, opline->op1_type, opline->op1, op1_addr, op1_info, opline->op2_type, opline->op2, op2_addr, op2_info, opline->result.var, res_addr, res_info, res_use_info, may_overflow, may_throw)) {
		return 0;
	}
	if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, res_info)) {
		return 0;
	}
	return 1;
}

static int zend_jit_add_arrays(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, uint32_t op2_info, zend_jit_addr op2_addr, zend_jit_addr res_addr)
{
	ir_ref ref;
	ir_ref arg1 = zend_jit_zval_ptr(jit, op1_addr);
	ir_ref arg2 = zend_jit_zval_ptr(jit, op2_addr);

	ref = zend_jit_call_2(jit, IR_ADDR,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_add_arrays_helper, IR_CONST_FASTCALL_FUNC),
		arg1, arg2);
	zend_jit_zval_set_ptr(jit, res_addr, ref);
	zend_jit_zval_set_type_info(jit, res_addr, IS_ARRAY_EX);
	zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);
	zend_jit_free_op(jit, opline->op2_type, opline->op2, op2_info, opline);
	return 1;
}

static int zend_jit_long_math_helper(zend_jit_ctx   *jit,
                                     const zend_op  *opline,
                                     zend_uchar      opcode,
                                     zend_uchar      op1_type,
                                     znode_op        op1,
                                     zend_jit_addr   op1_addr,
                                     uint32_t        op1_info,
                                     zend_ssa_range *op1_range,
                                     zend_uchar      op2_type,
                                     znode_op        op2,
                                     zend_jit_addr   op2_addr,
                                     uint32_t        op2_info,
                                     zend_ssa_range *op2_range,
                                     uint32_t        res_var,
                                     zend_jit_addr   res_addr,
                                     uint32_t        res_info,
                                     uint32_t        res_use_info,
                                     int             may_throw)
{
	ir_ref ref = IR_UNUSED;
	ir_ref res_inputs[2];
	ir_ref if_long1 = IR_UNUSED;
	ir_ref if_long2 = IR_UNUSED;
	bool same_ops = zend_jit_same_addr(op1_addr, op2_addr);

	if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG)) {
		if_long1 = zend_jit_if_zval_type(jit, op1_addr, IS_LONG);
		zend_jit_if_true(jit, if_long1);
	}
	if (!same_ops && (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG))) {
		if_long2 = zend_jit_if_zval_type(jit, op2_addr, IS_LONG);
		zend_jit_if_true(jit, if_long2);
	}

	if (opcode == ZEND_SL) {
		if (Z_MODE(op2_addr) == IS_CONST_ZVAL) {
			zend_long op2_lval = Z_LVAL_P(Z_ZV(op2_addr));

			if (UNEXPECTED((zend_ulong)op2_lval >= SIZEOF_ZEND_LONG * 8)) {
				if (EXPECTED(op2_lval > 0)) {
					ref = ir_const_php_long(&jit->ctx, 0);
				} else {
					zend_jit_invalidate_var_if_necessary(jit, op1_type, op1_addr, op1);
					zend_jit_invalidate_var_if_necessary(jit, op2_type, op2_addr, op2);
					zend_jit_set_ex_opline(jit, opline);
					zend_jit_guard(jit, IR_FALSE,
						zend_jit_stub_addr(jit, jit_stub_negative_shift));
					ref = ir_const_php_long(&jit->ctx, 0); // dead code ???
				}
			} else {
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_SHL, IR_PHP_LONG),
					zend_jit_zval_lval(jit, op1_addr),
					ir_const_php_long(&jit->ctx, op2_lval));
			}
		} else {
			ref = zend_jit_zval_lval(jit, op2_addr);
			if (!op2_range ||
			     op2_range->min < 0 ||
			     op2_range->max >= SIZEOF_ZEND_LONG * 8) {

				ir_ref if_wrong, cold_path, ref2, if_ok;

				if_wrong = zend_jit_if(jit,
					ir_fold2(&jit->ctx, IR_OPT(IR_UGT, IR_BOOL), ref,
						ir_const_php_long(&jit->ctx, (SIZEOF_ZEND_LONG * 8) - 1)));
				zend_jit_if_true_cold(jit, if_wrong);
				if_ok = zend_jit_if(jit,
					ir_fold2(&jit->ctx, IR_OPT(IR_GE, IR_BOOL), ref,
						ir_const_php_long(&jit->ctx, 0)));
				zend_jit_if_false(jit, if_ok);
				zend_jit_set_ex_opline(jit, opline);
				zend_jit_invalidate_var_if_necessary(jit, op1_type, op1_addr, op1);
				zend_jit_invalidate_var_if_necessary(jit, op2_type, op2_addr, op2);
				zend_jit_ijmp(jit,
					zend_jit_stub_addr(jit, jit_stub_negative_shift));
				zend_jit_if_true(jit, if_ok);
				ref2 = ir_const_php_long(&jit->ctx, 0);
				cold_path = zend_jit_end(jit);
				zend_jit_if_false(jit, if_wrong);
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_SHL, IR_PHP_LONG),
					zend_jit_zval_lval(jit, op1_addr),
					ref);
				zend_jit_merge_2(jit, zend_jit_end(jit), cold_path);
				ref = zend_jit_phi_2(jit, ref, ref2);
			} else {
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_SHL, IR_PHP_LONG),
					zend_jit_zval_lval(jit, op1_addr),
					ref);
			}
		}
	} else if (opcode == ZEND_SR) {
		if (Z_MODE(op2_addr) == IS_CONST_ZVAL) {
			zend_long op2_lval = Z_LVAL_P(Z_ZV(op2_addr));

			if (UNEXPECTED((zend_ulong)op2_lval >= SIZEOF_ZEND_LONG * 8)) {
				if (EXPECTED(op2_lval > 0)) {
					ref = ir_fold2(&jit->ctx, IR_OPT(IR_SAR, IR_PHP_LONG),
						zend_jit_zval_lval(jit, op1_addr),
						ir_const_php_long(&jit->ctx, (SIZEOF_ZEND_LONG * 8) - 1));
				} else {
					zend_jit_invalidate_var_if_necessary(jit, op1_type, op1_addr, op1);
					zend_jit_invalidate_var_if_necessary(jit, op2_type, op2_addr, op2);
					zend_jit_set_ex_opline(jit, opline);
					zend_jit_guard(jit, IR_FALSE,
						zend_jit_stub_addr(jit, jit_stub_negative_shift));
					ref = ir_const_php_long(&jit->ctx, 0); // dead code ???
				}
			} else {
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_SAR, IR_PHP_LONG),
					zend_jit_zval_lval(jit, op1_addr),
					ir_const_php_long(&jit->ctx, op2_lval));
			}
		} else {
			ref = zend_jit_zval_lval(jit, op2_addr);
			if (!op2_range ||
			     op2_range->min < 0 ||
			     op2_range->max >= SIZEOF_ZEND_LONG * 8) {

				ir_ref if_wrong, cold_path, ref2, if_ok;

				if_wrong = zend_jit_if(jit,
					ir_fold2(&jit->ctx, IR_OPT(IR_UGT, IR_BOOL), ref,
						ir_const_php_long(&jit->ctx, (SIZEOF_ZEND_LONG * 8) - 1)));
				zend_jit_if_true_cold(jit, if_wrong);
				if_ok = zend_jit_if(jit,
					ir_fold2(&jit->ctx, IR_OPT(IR_GE, IR_BOOL), ref,
						ir_const_php_long(&jit->ctx, 0)));
				zend_jit_if_false(jit, if_ok);
				zend_jit_set_ex_opline(jit, opline);
				zend_jit_invalidate_var_if_necessary(jit, op1_type, op1_addr, op1);
				zend_jit_invalidate_var_if_necessary(jit, op2_type, op2_addr, op2);
				zend_jit_ijmp(jit,
					zend_jit_stub_addr(jit, jit_stub_negative_shift));
				zend_jit_if_true(jit, if_ok);
				ref2 = ir_const_php_long(&jit->ctx, (SIZEOF_ZEND_LONG * 8) - 1);
				cold_path = zend_jit_end(jit);
				zend_jit_if_false(jit, if_wrong);
				zend_jit_merge_2(jit, zend_jit_end(jit), cold_path);
				ref = zend_jit_phi_2(jit, ref, ref2);
			}
			ref = ir_fold2(&jit->ctx, IR_OPT(IR_SAR, IR_PHP_LONG),
				zend_jit_zval_lval(jit, op1_addr),
				ref);
		}
	} else if (opcode == ZEND_MOD) {
		if (Z_MODE(op2_addr) == IS_CONST_ZVAL) {
			zend_long op2_lval = Z_LVAL_P(Z_ZV(op2_addr));

			if (op2_lval == 0) {
				zend_jit_invalidate_var_if_necessary(jit, op1_type, op1_addr, op1);
				zend_jit_invalidate_var_if_necessary(jit, op2_type, op2_addr, op2);
				zend_jit_set_ex_opline(jit, opline);
				zend_jit_guard(jit, IR_FALSE,
					zend_jit_stub_addr(jit, jit_stub_mod_by_zero));
				ref = ir_const_php_long(&jit->ctx, 0); // dead code ???
			} else if (zend_long_is_power_of_two(op2_lval) && op1_range && op1_range->min >= 0) {
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_PHP_LONG),
					zend_jit_zval_lval(jit, op1_addr),
					ir_const_php_long(&jit->ctx, op2_lval - 1));
			} else {
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_MOD, IR_PHP_LONG),
					zend_jit_zval_lval(jit, op1_addr),
					ir_const_php_long(&jit->ctx, op2_lval));
			}
		} else {
			ir_ref zero_path = 0;

			ref = zend_jit_zval_lval(jit, op2_addr);
			if (!op2_range || (op2_range->min <= 0 && op2_range->max >= 0)) {
				ir_ref if_ok = zend_jit_if(jit, ref);
				zend_jit_if_false(jit, if_ok);
				zend_jit_set_ex_opline(jit, opline);
				zend_jit_invalidate_var_if_necessary(jit, op1_type, op1_addr, op1);
				zend_jit_invalidate_var_if_necessary(jit, op2_type, op2_addr, op2);
				zend_jit_ijmp(jit,
					zend_jit_stub_addr(jit, jit_stub_mod_by_zero));
				zend_jit_if_true(jit, if_ok);
			}

			/* Prevent overflow error/crash if op1 == LONG_MIN and op2 == -1 */
			if (!op2_range || (op2_range->min <= -1 && op2_range->max >= -1)) {
				ir_ref if_minus_one = zend_jit_if(jit,
					ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
						ref,
						ir_const_php_long(&jit->ctx, -1)));
				zend_jit_if_true_cold(jit, if_minus_one);
				zero_path = zend_jit_end(jit);
				zend_jit_if_false(jit, if_minus_one);
			}
			ref = ir_fold2(&jit->ctx, IR_OPT(IR_MOD, IR_PHP_LONG),
				zend_jit_zval_lval(jit, op1_addr),
				ref);

			if (zero_path) {
				zend_jit_merge_2(jit, zend_jit_end(jit), zero_path);
				ref = zend_jit_phi_2(jit,
					ref, ir_const_php_long(&jit->ctx, 0));
			}
		}
	} else {
		ir_op op;
		ir_ref op1, op2;

		if (opcode == ZEND_BW_OR) {
			op = IR_OR;
		} else if (opcode == ZEND_BW_AND) {
			op = IR_AND;
		} else if (opcode == ZEND_BW_XOR) {
			op = IR_XOR;
		} else {
			ZEND_ASSERT(0);
		}
		op1 = zend_jit_zval_lval(jit, op1_addr);
		op2 = (same_ops) ? op1 : zend_jit_zval_lval(jit, op2_addr);
		ref = ir_fold2(&jit->ctx, IR_OPT(op, IR_PHP_LONG), op1, op2);
	}

	if (ref) {
		if (Z_MODE(res_addr) == IS_REG
		 && ((op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG))
		  || (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG)))) {
			jit->delay_var = Z_SSA_VAR(res_addr);
			jit->delay_buf = res_inputs;
			jit->delay_count = 0;
		}
		zend_jit_zval_set_lval(jit, res_addr, ref);
		if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
			if (Z_MODE(op1_addr) != IS_MEM_ZVAL || !zend_jit_same_addr(op1_addr, res_addr)) {
				if ((res_use_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF|MAY_BE_GUARD)) != MAY_BE_LONG) {
					zend_jit_zval_set_type_info(jit, res_addr, IS_LONG);
				}
			}
		}
	}

	if ((op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG)) ||
		(op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG))) {
		ir_ref fast_path = zend_jit_end(jit);
		ir_ref func, arg1, arg2, arg3;

		if (if_long2 && if_long1) {
			ir_ref ref;
			zend_jit_if_false_cold(jit, if_long2);
			ref = zend_jit_end(jit);
			zend_jit_if_false_cold(jit, if_long1);
			zend_jit_merge_2(jit, ref, zend_jit_end(jit));
		} else if (if_long1) {
			zend_jit_if_false_cold(jit, if_long1);
		} else if (if_long2) {
			zend_jit_if_false_cold(jit, if_long2);
		}

		if (op1_info & MAY_BE_UNDEF) {
			ir_ref if_def, err_path;

			if_def = zend_jit_if_not_zval_type(jit, op1_addr, IS_UNDEF);
			zend_jit_if_false_cold(jit, if_def);

			// zend_error(E_WARNING, "Undefined variable $%s", ZSTR_VAL(CV_DEF_OF(EX_VAR_TO_NUM(opline->op1.var))));
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_undefined_op_helper, IR_CONST_FASTCALL_FUNC),
				ir_const_u32(&jit->ctx, opline->op1.var));

			zend_jit_zval_set_type_info(jit, op1_addr, IS_NULL);
			err_path = zend_jit_end(jit);
			zend_jit_if_true(jit, if_def);
			zend_jit_merge_2(jit, err_path, zend_jit_end(jit)); // wrong merge
		}

		if (op2_info & MAY_BE_UNDEF) {
			ir_ref if_def, err_path;

			if_def = zend_jit_if_not_zval_type(jit, op2_addr, IS_UNDEF);
			zend_jit_if_false_cold(jit, if_def);

			// zend_error(E_WARNING, "Undefined variable $%s", ZSTR_VAL(CV_DEF_OF(EX_VAR_TO_NUM(opline->op2.var))));
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_undefined_op_helper, IR_CONST_FASTCALL_FUNC),
				ir_const_u32(&jit->ctx, opline->op2.var));

			zend_jit_zval_set_type_info(jit, op2_addr, IS_NULL);
			err_path = zend_jit_end(jit);
			zend_jit_if_true(jit, if_def);
			zend_jit_merge_2(jit, err_path, zend_jit_end(jit)); // wrong merge
		}

		if (Z_MODE(op1_addr) == IS_REG) {
			zend_jit_addr real_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, op1.var);
			if (!zend_jit_spill_store(jit, op1_addr, real_addr, op1_info, 1)) {
				return 0;
			}
			op1_addr = real_addr;
		}
		if (Z_MODE(op2_addr) == IS_REG) {
			zend_jit_addr real_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, op2.var);
			if (!zend_jit_spill_store(jit, op2_addr, real_addr, op2_info, 1)) {
				return 0;
			}
			op2_addr = real_addr;
		}
		if (Z_MODE(res_addr) == IS_REG) {
			arg1 = zend_jit_zval_addr(jit, ZEND_ADDR_MEM_ZVAL(ZREG_FP, res_var));
		} else {
			arg1 = zend_jit_zval_addr(jit, res_addr);
		}
		arg2 = zend_jit_zval_addr(jit, op1_addr);
		arg3 = zend_jit_zval_addr(jit, op2_addr);
		zend_jit_set_ex_opline(jit, opline);
		if (opcode == ZEND_BW_OR) {
			func = zend_jit_const_func_addr(jit, (uintptr_t)bitwise_or_function, IR_CONST_FASTCALL_FUNC);
		} else if (opcode == ZEND_BW_AND) {
			func = zend_jit_const_func_addr(jit, (uintptr_t)bitwise_and_function, IR_CONST_FASTCALL_FUNC);
		} else if (opcode == ZEND_BW_XOR) {
			func = zend_jit_const_func_addr(jit, (uintptr_t)bitwise_xor_function, IR_CONST_FASTCALL_FUNC);
		} else if (opcode == ZEND_SL) {
			func = zend_jit_const_func_addr(jit, (uintptr_t)shift_left_function, IR_CONST_FASTCALL_FUNC);
		} else if (opcode == ZEND_SR) {
			func = zend_jit_const_func_addr(jit, (uintptr_t)shift_right_function, IR_CONST_FASTCALL_FUNC);
		} else if (opcode == ZEND_MOD) {
			func = zend_jit_const_func_addr(jit, (uintptr_t)mod_function, IR_CONST_FASTCALL_FUNC);
		} else {
			ZEND_UNREACHABLE();
		}
		zend_jit_call_3(jit, IR_VOID, func, arg1, arg2, arg3);

		if (op1_addr == res_addr && (op2_info & MAY_BE_RCN)) {
			/* compound assignment may decrement "op2" refcount */
			op2_info |= MAY_BE_RC1;
		}

		zend_jit_free_op(jit, op1_type, op1, op1_info, NULL);
		zend_jit_free_op(jit, op2_type, op2, op2_info, NULL);

		if (may_throw) {
			if (opline->opcode == ZEND_ASSIGN_DIM_OP && (opline->op2_type & (IS_VAR|IS_TMP_VAR))) {
				zend_jit_guard_not(jit,
					zend_jit_load(jit, IR_ADDR,
						zend_jit_eg_exception_addr(jit)),
					zend_jit_stub_addr(jit, jit_stub_exception_handler_free_op2));
			} else if (Z_MODE(res_addr) == IS_MEM_ZVAL && Z_REG(res_addr) == ZREG_RX) {
				zend_jit_check_exception_undef_result(jit, opline);
			} else {
				zend_jit_check_exception(jit);
			}
		}

		if (Z_MODE(res_addr) == IS_REG) {
			zend_jit_addr real_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, res_var);
			if (!zend_jit_load_reg(jit, real_addr, res_addr, res_info)) {
				return 0;
			}
		}

		zend_jit_merge_2(jit, fast_path, zend_jit_end(jit));

		if (Z_MODE(res_addr) == IS_REG) {
			int count = jit->delay_count;

			jit->delay_var = -1;
			jit->delay_buf = NULL;
			jit->delay_count = 0;
			ZEND_ASSERT(count == 2);
			if (count == 1) {
				zend_jit_def_reg(jit, res_addr, res_inputs[0]);
			} else {
				ir_ref phi = zend_jit_phi_N(jit, count, res_inputs);
				zend_jit_def_reg(jit, res_addr, phi);
			}
		}
	}

	return 1;
}

static int zend_jit_long_math(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_ssa_range *op1_range, zend_jit_addr op1_addr, uint32_t op2_info, zend_ssa_range *op2_range, zend_jit_addr op2_addr, uint32_t res_use_info, uint32_t res_info, zend_jit_addr res_addr, int may_throw)
{
	ZEND_ASSERT((op1_info & MAY_BE_LONG) && (op2_info & MAY_BE_LONG));

	if (!zend_jit_long_math_helper(jit, opline, opline->opcode,
			opline->op1_type, opline->op1, op1_addr, op1_info, op1_range,
			opline->op2_type, opline->op2, op2_addr, op2_info, op2_range,
			opline->result.var, res_addr, res_info, res_use_info, may_throw)) {
		return 0;
	}
	if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, res_info)) {
		return 0;
	}
	return 1;
}

static int zend_jit_concat_helper(zend_jit_ctx   *jit,
                                  const zend_op  *opline,
                                  zend_uchar      op1_type,
                                  znode_op        op1,
                                  zend_jit_addr   op1_addr,
                                  uint32_t        op1_info,
                                  zend_uchar      op2_type,
                                  znode_op        op2,
                                  zend_jit_addr   op2_addr,
                                  uint32_t        op2_info,
                                  zend_jit_addr   res_addr,
                                  int             may_throw)
{
	ir_ref if_op1_string = IR_UNUSED;
	ir_ref if_op2_string = IR_UNUSED;
	ir_ref fast_path = IR_UNUSED;

	if ((op1_info & MAY_BE_STRING) && (op2_info & MAY_BE_STRING)) {
		if (op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF) - MAY_BE_STRING)) {
			if_op1_string = zend_jit_if_zval_type(jit, op1_addr, IS_STRING);
			zend_jit_if_true(jit, if_op1_string);
		}
		if (op2_info & ((MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF) - MAY_BE_STRING)) {
			if_op2_string = zend_jit_if_zval_type(jit, op2_addr, IS_STRING);
			zend_jit_if_true(jit, if_op2_string);
		}
		if (zend_jit_same_addr(op1_addr, res_addr)) {
			ir_ref arg1 = zend_jit_zval_addr(jit, res_addr);
			ir_ref arg2 = zend_jit_zval_addr(jit, op2_addr);

			zend_jit_call_2(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fast_assign_concat_helper, IR_CONST_FASTCALL_FUNC),
				arg1, arg2);
			/* concatenation with itself may reduce refcount */
			op2_info |= MAY_BE_RC1;
		} else {
			ir_ref arg1 = zend_jit_zval_addr(jit, res_addr);
			ir_ref arg2 = zend_jit_zval_addr(jit, op1_addr);
			ir_ref arg3 = zend_jit_zval_addr(jit, op2_addr);

			if (op1_type == IS_CV || op1_type == IS_CONST) {
				zend_jit_call_3(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fast_concat_helper, IR_CONST_FASTCALL_FUNC),
					arg1, arg2, arg3);
			} else {
				zend_jit_call_3(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fast_concat_tmp_helper, IR_CONST_FASTCALL_FUNC),
					arg1, arg2, arg3);
			}
		}
		/* concatenation with empty string may increase refcount */
		op2_info |= MAY_BE_RCN;
		zend_jit_free_op(jit, op2_type, op2, op2_info, opline);
		if (if_op1_string || if_op2_string) {
			fast_path = zend_jit_end(jit);
		}
	}
	if ((op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF) - MAY_BE_STRING)) ||
	    (op2_info & ((MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF) - MAY_BE_STRING))) {
		if ((op1_info & MAY_BE_STRING) && (op2_info & MAY_BE_STRING)) {
			if (if_op1_string && if_op2_string) {
				ir_ref end1;

				zend_jit_if_false(jit, if_op1_string);
				end1 = zend_jit_end(jit);
				zend_jit_if_false(jit, if_op2_string);
				zend_jit_merge_2(jit, end1, zend_jit_end(jit));
			} else if (if_op1_string) {
				zend_jit_if_false_cold(jit, if_op1_string);
			} else if (if_op2_string) {
				zend_jit_if_false_cold(jit, if_op2_string);
			}
		}
		ir_ref arg1 = zend_jit_zval_addr(jit, res_addr);
		ir_ref arg2 = zend_jit_zval_addr(jit, op1_addr);
		ir_ref arg3 = zend_jit_zval_addr(jit, op2_addr);

		zend_jit_set_ex_opline(jit, opline);
		zend_jit_call_3(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)concat_function, IR_CONST_FASTCALL_FUNC),
			arg1, arg2, arg3);
		/* concatenation with empty string may increase refcount */
		op1_info |= MAY_BE_RCN;
		op2_info |= MAY_BE_RCN;
		zend_jit_free_op(jit, op1_type, op1, op1_info, NULL);
		zend_jit_free_op(jit, op2_type, op2, op2_info, NULL);
		if (may_throw) {
			if (opline->opcode == ZEND_ASSIGN_DIM_OP && (opline->op2_type & (IS_VAR|IS_TMP_VAR))) {
				zend_jit_guard_not(jit,
					zend_jit_load(jit, IR_ADDR,
						zend_jit_eg_exception_addr(jit)),
					zend_jit_stub_addr(jit, jit_stub_exception_handler_free_op2));
			} else if (Z_MODE(res_addr) == IS_MEM_ZVAL && Z_REG(res_addr) == ZREG_RX) {
				zend_jit_check_exception_undef_result(jit, opline);
			} else {
				zend_jit_check_exception(jit);
			}
		}
		if ((op1_info & MAY_BE_STRING) && (op2_info & MAY_BE_STRING)) {
			zend_jit_merge_2(jit,
				fast_path,
				zend_jit_end(jit));
		}
	}
	return 1;
}

static int zend_jit_concat(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, uint32_t op2_info, zend_jit_addr res_addr, int may_throw)
{
	zend_jit_addr op1_addr, op2_addr;

	ZEND_ASSERT(!(op1_info & MAY_BE_UNDEF) && !(op2_info & MAY_BE_UNDEF));
	ZEND_ASSERT((op1_info & MAY_BE_STRING) && (op2_info & MAY_BE_STRING));

	op1_addr = OP1_ADDR();
	op2_addr = OP2_ADDR();

	return zend_jit_concat_helper(jit, opline, opline->op1_type, opline->op1, op1_addr, op1_info, opline->op2_type, opline->op2, op2_addr, op2_info, res_addr, may_throw);
}

static int zend_jit_assign_op(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, uint32_t op1_def_info, zend_ssa_range *op1_range, uint32_t op2_info, zend_ssa_range *op2_range, int may_overflow, int may_throw)
{
	int result = 1;
	zend_jit_addr op1_addr, op2_addr;
	ir_ref slow_path = IR_UNUSED;


	ZEND_ASSERT(opline->op1_type == IS_CV && opline->result_type == IS_UNUSED);
	ZEND_ASSERT(!(op1_info & MAY_BE_UNDEF) && !(op2_info & MAY_BE_UNDEF));

	op1_addr = OP1_ADDR();
	op2_addr = OP2_ADDR();

	if (op1_info & MAY_BE_REF) {
		ir_ref ref, ref2, arg2, op1_noref_path;
		ir_ref if_op1_ref = IR_UNUSED;
		ir_ref if_op1_typed = IR_UNUSED;
		binary_op_type binary_op = get_binary_op(opline->extended_value);

		ref = zend_jit_zval_addr(jit, op1_addr);
		if_op1_ref = zend_jit_if_zval_ref_type(jit, ref, ir_const_u8(&jit->ctx, IS_REFERENCE));
		zend_jit_if_false(jit, if_op1_ref);
		op1_noref_path = zend_jit_end(jit);
		zend_jit_if_true(jit, if_op1_ref);
		ref2 = zend_jit_zval_ref_ptr(jit, ref);

		if_op1_typed = zend_jit_if_typed_ref(jit, ref2);
		zend_jit_if_true_cold(jit, if_op1_typed);

		arg2 = zend_jit_zval_addr(jit, op2_addr);
		zend_jit_set_ex_opline(jit, opline);
		if ((opline->op2_type & (IS_TMP_VAR|IS_VAR))
		 && (op2_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
			zend_jit_call_3(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_assign_op_to_typed_ref_tmp, IR_CONST_FASTCALL_FUNC),
				ref2, arg2, zend_jit_const_func_addr(jit, (uintptr_t)binary_op, IR_CONST_FASTCALL_FUNC));
		} else {
			zend_jit_call_3(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_assign_op_to_typed_ref, IR_CONST_FASTCALL_FUNC),
				ref2, arg2, zend_jit_const_func_addr(jit, (uintptr_t)binary_op, IR_CONST_FASTCALL_FUNC));
		}
		zend_jit_check_exception(jit);
		slow_path = zend_jit_end(jit);

		zend_jit_if_false(jit, if_op1_typed);
		ref2 = zend_jit_addr_offset(jit, ref2, offsetof(zend_reference, val));

		zend_jit_merge_2(jit, op1_noref_path, zend_jit_end(jit));
		ref = zend_jit_phi_2(jit, ref, ref2);
		op1_addr = ZEND_ADDR_REF_ZVAL(ref);
	}

	switch (opline->extended_value) {
		case ZEND_ADD:
		case ZEND_SUB:
		case ZEND_MUL:
		case ZEND_DIV:
			result = zend_jit_math_helper(jit, opline, opline->extended_value, opline->op1_type, opline->op1, op1_addr, op1_info, opline->op2_type, opline->op2, op2_addr, op2_info, opline->op1.var, op1_addr, op1_def_info, op1_info, may_overflow, may_throw);
			break;
		case ZEND_BW_OR:
		case ZEND_BW_AND:
		case ZEND_BW_XOR:
		case ZEND_SL:
		case ZEND_SR:
		case ZEND_MOD:
			result = zend_jit_long_math_helper(jit, opline, opline->extended_value,
				opline->op1_type, opline->op1, op1_addr, op1_info, op1_range,
				opline->op2_type, opline->op2, op2_addr, op2_info, op2_range,
				opline->op1.var, op1_addr, op1_def_info, op1_info, may_throw);
			break;
		case ZEND_CONCAT:
			result = zend_jit_concat_helper(jit, opline, opline->op1_type, opline->op1, op1_addr, op1_info, opline->op2_type, opline->op2, op2_addr, op2_info, op1_addr, may_throw);
			break;
		default:
			ZEND_UNREACHABLE();
	}

	if (op1_info & MAY_BE_REF) {
		zend_jit_merge_2(jit, slow_path,
			zend_jit_end(jit));
	}

	return result;
}

static ir_ref zend_jit_zval_ref_deref(zend_jit_ctx *jit, ir_ref ref)
{
	ir_ref if_ref, ref2, ref_path;
	ir_ref type = zend_jit_zval_ref_type(jit, ref);

	if_ref = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL), type, ir_const_u8(&jit->ctx, IS_REFERENCE)));

	zend_jit_if_true(jit, if_ref);
	ref2 = zend_jit_zval_ref_ptr(jit, ref);
	ref2 = zend_jit_addr_offset(jit, ref2, offsetof(zend_reference, val));
	ref_path = zend_jit_end(jit);

	zend_jit_if_false(jit, if_ref);
	zend_jit_merge_2(jit, zend_jit_end(jit), ref_path);
	ref = zend_jit_phi_2(jit, ref, ref2);

	return ref;
}

static zend_jit_addr zend_jit_zval_deref(zend_jit_ctx *jit, zend_jit_addr addr)
{
	ir_ref ref = zend_jit_zval_addr(jit, addr);
	ref = zend_jit_zval_ref_deref(jit, ref);
	return ZEND_ADDR_REF_ZVAL(ref);
}

static ir_ref zend_jit_zval_ref_indirect_deref(zend_jit_ctx *jit, ir_ref ref)
{
	ir_ref if_ref, ref2, ref_path;
	ir_ref type = zend_jit_zval_ref_type(jit, ref);

	if_ref = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL), type, ir_const_u8(&jit->ctx, IS_INDIRECT)));

	zend_jit_if_true(jit, if_ref);
	ref2 = zend_jit_zval_ref_ptr(jit, ref);
	ref_path = zend_jit_end(jit);

	zend_jit_if_false(jit, if_ref);
	zend_jit_merge_2(jit, zend_jit_end(jit), ref_path);
	ref = zend_jit_phi_2(jit, ref, ref2);

	return ref;
}

static zend_jit_addr zend_jit_zval_indirect_deref(zend_jit_ctx *jit, zend_jit_addr addr)
{
	ir_ref ref = zend_jit_zval_addr(jit, addr);
	ref = zend_jit_zval_ref_indirect_deref(jit, ref);
	return ZEND_ADDR_REF_ZVAL(ref);
}

static int zend_jit_simple_assign(zend_jit_ctx   *jit,
                                  const zend_op  *opline,
                                  zend_jit_addr   var_addr,
                                  uint32_t        var_info,
                                  uint32_t        var_def_info,
                                  zend_uchar      val_type,
                                  zend_jit_addr   val_addr,
                                  uint32_t        val_info,
                                  zend_jit_addr   res_addr,
                                  bool            check_exception)
{
	ir_ref end_inputs[5];
	uint32_t end_inputs_count = 0;

	if (Z_MODE(val_addr) == IS_CONST_ZVAL) {
		zval *zv = Z_ZV(val_addr);

		if (!res_addr) {
			zend_jit_copy_zval_const(jit,
				var_addr,
				var_info, var_def_info,
				zv, 1);
		} else {
			zend_jit_copy_zval_const(jit,
				var_addr,
				var_info, var_def_info,
				zv, 1);
			zend_jit_copy_zval_const(jit,
				res_addr,
				-1, var_def_info,
				zv, 1);
		}
	} else {
		if (val_info & MAY_BE_UNDEF) {
			ir_ref if_def, ret;

			if_def = zend_jit_if_not_zval_type(jit, val_addr, IS_UNDEF);
			zend_jit_if_false_cold(jit, if_def);

			zend_jit_zval_set_type_info(jit, var_addr, IS_NULL);
			if (res_addr) {
				zend_jit_zval_set_type_info(jit, res_addr, IS_NULL);
			}
			zend_jit_set_ex_opline(jit, opline);

			ZEND_ASSERT(Z_MODE(val_addr) == IS_MEM_ZVAL);
			// zend_error(E_WARNING, "Undefined variable $%s", ZSTR_VAL(CV_DEF_OF(EX_VAR_TO_NUM(opline->op1.var))));
			ret = zend_jit_call_1(jit, IR_I32,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_undefined_op_helper, IR_CONST_FASTCALL_FUNC),
				ir_const_u32(&jit->ctx, Z_OFFSET(val_addr)));

			if (check_exception) {
				zend_jit_guard(jit, ret,
					zend_jit_stub_addr(jit, jit_stub_exception_handler_undef));
			}

			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_if_true(jit, if_def);
		}
		if (val_info & MAY_BE_REF) {
			if (val_type == IS_CV) {
				ir_ref ref = zend_jit_zval_addr(jit, val_addr);
				ref = zend_jit_zval_ref_deref(jit, ref);
				val_addr = ZEND_ADDR_REF_ZVAL(ref);
			} else {
				ir_ref ref, type, if_ref, ref2, refcount, if_not_zero;

				ref = zend_jit_zval_addr(jit, val_addr);
				type = zend_jit_zval_ref_type(jit, ref);
				if_ref = zend_jit_if(jit,
					ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL), type, ir_const_u8(&jit->ctx, IS_REFERENCE)));

				zend_jit_if_true_cold(jit, if_ref);
				ref = zend_jit_zval_ref_ptr(jit, ref);
				ref2 = zend_jit_addr_offset(jit, ref, offsetof(zend_reference, val));
				if (!res_addr) {
					zend_jit_copy_zval(jit,
						var_addr,
						var_info,
						ZEND_ADDR_REF_ZVAL(ref2), val_info, 1);
				} else {
					zend_jit_copy_zval2(jit,
						res_addr,
						var_addr,
						var_info,
						ZEND_ADDR_REF_ZVAL(ref2), val_info, 2);
				}

				refcount = zend_jit_gc_delref(jit, ref);
				if_not_zero = zend_jit_if(jit, refcount);
				zend_jit_if_false(jit, if_not_zero);
				zend_jit_zval_dtor(jit, ref, val_info, opline);
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_not_zero);
				zend_jit_array_add(end_inputs, zend_jit_end(jit));

				zend_jit_if_false(jit, if_ref);

#if 0
//???
				zend_jit_addr ref_addr;
				zend_reg type_reg = tmp_reg;

				|	IF_ZVAL_TYPE val_addr, IS_REFERENCE, >1
				|.cold_code
				|1:
				|	// zend_refcounted *ref = Z_COUNTED_P(retval_ptr);
				|	GET_ZVAL_PTR r2, val_addr
				|	GC_DELREF r2
				|	// ZVAL_COPY_VALUE(return_value, &ref->value);
				ref_addr = ZEND_ADDR_MEM_ZVAL(ZREG_R2, 8);
				if (!res_addr) {
					|	ZVAL_COPY_VALUE var_addr, var_info, ref_addr, val_info, type_reg, tmp_reg
				} else {
					|	ZVAL_COPY_VALUE_2 var_addr, var_info, res_addr, ref_addr, val_info, type_reg, tmp_reg
				}
				|	je >2
				if (tmp_reg == ZREG_R0) {
					|	IF_NOT_REFCOUNTED ah, >3
				} else {
					|	IF_NOT_FLAGS Rd(tmp_reg), (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT), >3
				}
				|	GET_ZVAL_PTR Ra(tmp_reg), var_addr

				if (!res_addr) {
					|	GC_ADDREF Ra(tmp_reg)
				} else {
					|	add dword [Ra(tmp_reg)], 2
				}
				|	jmp >3
				|2:
				if (res_addr) {
					if (tmp_reg == ZREG_R0) {
						|	IF_NOT_REFCOUNTED ah, >2
					} else {
						|	IF_NOT_FLAGS Rd(tmp_reg), (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT), >2
					}
					|	GET_ZVAL_PTR Ra(tmp_reg), var_addr
					|	GC_ADDREF Ra(tmp_reg)
					|2:
				}
				|	EFREE_REFERENCE r2
				|	jmp >3
				|.code
#endif
			}
		}

		if (!res_addr) {
			zend_jit_copy_zval(jit,
				var_addr,
				var_info,
				val_addr, val_info, val_type == IS_CV);
		} else {
			zend_jit_copy_zval2(jit,
				res_addr,
				var_addr,
				var_info,
				val_addr, val_info, val_type == IS_CV ? 2 : 1);
		}
	}

	if (end_inputs_count) {
		zend_jit_array_add(end_inputs, zend_jit_end(jit));
		zend_jit_merge_N(jit, end_inputs_count, end_inputs);
	}

	return 1;
}

static int zend_jit_assign_to_variable_call(zend_jit_ctx   *jit,
                                            const zend_op  *opline,
                                            zend_jit_addr   __var_use_addr,
                                            zend_jit_addr   var_addr,
                                            uint32_t        __var_info,
                                            uint32_t        __var_def_info,
                                            zend_uchar      val_type,
                                            zend_jit_addr   val_addr,
                                            uint32_t        val_info,
                                            zend_jit_addr   __res_addr,
                                            bool       __check_exception)
{
	jit_stub_id func;
	ir_ref undef_path = IR_UNUSED;

	if (val_info & MAY_BE_UNDEF) {
		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
			int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
			const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

			if (!exit_addr) {
				return 0;
			}

			zend_jit_guard_not_zval_type(jit, val_addr, IS_UNDEF, exit_addr);
		} else {
			ir_ref if_def;

			ZEND_ASSERT(Z_MODE(val_addr) == IS_MEM_ZVAL && Z_REG(val_addr) == ZREG_FP);
			if_def = zend_jit_if(jit, zend_jit_zval_type(jit, val_addr));
			zend_jit_if_false_cold(jit, if_def);
			zend_jit_set_ex_opline(jit, opline);
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_undefined_op_helper, IR_CONST_FASTCALL_FUNC),
				ir_const_u32(&jit->ctx, Z_OFFSET(val_addr)));

			zend_jit_call_2(jit, IR_VOID,
				zend_jit_stub_func_addr(jit, jit_stub_assign_const, IR_CONST_FASTCALL_FUNC),
				zend_jit_zval_addr(jit, var_addr),
				ZEND_JIT_EG_ADDR(uninitialized_zval));

			undef_path = zend_jit_end(jit);
			zend_jit_if_true(jit, if_def);
		}
	}

	if (!(val_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF))) {
		func = jit_stub_assign_tmp;
	} else if (val_type == IS_CONST) {
		func = jit_stub_assign_const;
	} else if (val_type == IS_TMP_VAR) {
		func = jit_stub_assign_tmp;
	} else if (val_type == IS_VAR) {
		if (!(val_info & MAY_BE_REF)) {
			func = jit_stub_assign_tmp;
		} else {
			func = jit_stub_assign_var;
		}
	} else if (val_type == IS_CV) {
		if (!(val_info & MAY_BE_REF)) {
			func = jit_stub_assign_cv_noref;
		} else {
			func = jit_stub_assign_cv;
		}
	} else {
		ZEND_UNREACHABLE();
	}

	if (opline) {
		zend_jit_set_ex_opline(jit, opline);
	}

	zend_jit_call_2(jit, IR_VOID,
		zend_jit_stub_func_addr(jit, func, IR_CONST_FASTCALL_FUNC),
		zend_jit_zval_addr(jit, var_addr),
		zend_jit_zval_addr(jit, val_addr));

	if (undef_path) {
		zend_jit_merge_2(jit, undef_path, zend_jit_end(jit));
	}

	return 1;
}

static int zend_jit_assign_to_variable(zend_jit_ctx   *jit,
                                       const zend_op  *opline,
                                       zend_jit_addr   var_use_addr,
                                       zend_jit_addr   var_addr,
                                       uint32_t        var_info,
                                       uint32_t        var_def_info,
                                       zend_uchar      val_type,
                                       zend_jit_addr   val_addr,
                                       uint32_t        val_info,
                                       zend_jit_addr   res_addr,
                                       zend_jit_addr   ref_addr,
                                       bool       check_exception)
{
	ir_ref if_refcounted = IR_UNUSED;
	ir_ref end_inputs[5], simple_inputs[5], res_inputs[5];
	uint32_t end_inputs_count = 0;
	uint32_t simple_inputs_count = 0;
	bool done = 0;
	zend_jit_addr real_res_addr = 0;

	if (Z_MODE(val_addr) == IS_REG && jit->ra[Z_SSA_VAR(val_addr)].ref == IR_NULL) {
		/* Force load */
		zend_jit_use_reg(jit, val_addr);
	}

	if (Z_MODE(var_addr) == IS_REG) {
		jit->delay_var = Z_SSA_VAR(var_addr);
		jit->delay_buf = res_inputs;
		jit->delay_count = 0;
		if (Z_MODE(res_addr) == IS_REG) {
			real_res_addr = res_addr;
			res_addr = 0;
		}
	} else if (Z_MODE(res_addr) == IS_REG) {
		jit->delay_var = Z_SSA_VAR(res_addr);
		jit->delay_buf = res_inputs;
		jit->delay_count = 0;
	}

	if ((var_info & MAY_BE_REF) || ref_addr) {
		ir_ref ref = 0, if_ref = 0, ref2, arg2, ret, if_typed, non_ref_path;
		uintptr_t func;

		if (!ref_addr) {
			ref = zend_jit_zval_addr(jit, var_use_addr);
			if_ref = zend_jit_if_zval_ref_type(jit, ref, ir_const_u8(&jit->ctx, IS_REFERENCE));
			zend_jit_if_true(jit, if_ref);
			ref2 = zend_jit_zval_ref_ptr(jit, ref);
		} else {
			ref2 = zend_jit_mem_zval_addr(jit, ref_addr);
		}
		if_typed = zend_jit_if_typed_ref(jit, ref2);
		zend_jit_if_true_cold(jit, if_typed);
		zend_jit_set_ex_opline(jit, opline);
		if (val_type == IS_CONST) {
			func = (uintptr_t)zend_jit_assign_const_to_typed_ref;
		} else if (val_type == IS_TMP_VAR) {
			func = (uintptr_t)zend_jit_assign_tmp_to_typed_ref;
		} else if (val_type == IS_VAR) {
			func = (uintptr_t)zend_jit_assign_var_to_typed_ref;
		} else if (val_type == IS_CV) {
			func = (uintptr_t)zend_jit_assign_cv_to_typed_ref;
		} else {
			ZEND_UNREACHABLE();
		}
		if (Z_MODE(val_addr) == IS_REG) {
			ZEND_ASSERT(opline->opcode == ZEND_ASSIGN);
			zend_jit_addr real_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->op2.var);
			if (!zend_jit_spill_store(jit, val_addr, real_addr, val_info, 1)) {
				return 0;
			}
			arg2 = zend_jit_zval_addr(jit, real_addr);
		} else {
			arg2 = zend_jit_zval_addr(jit, val_addr);
		}
		ret = zend_jit_call_2(jit, IR_ADDR,
			zend_jit_const_func_addr(jit, func, IR_CONST_FASTCALL_FUNC),
			ref2, arg2);
		if (res_addr) {
			zend_jit_copy_zval(jit,
				res_addr,
				-1,
				ZEND_ADDR_REF_ZVAL(ret), -1, 1);
		}
		if (check_exception) {
			zend_jit_check_exception(jit);
		}
		zend_jit_array_add(end_inputs, zend_jit_end(jit));

		if (!ref_addr) {
			zend_jit_if_false(jit, if_ref);
			non_ref_path = zend_jit_end(jit);
			zend_jit_if_false(jit, if_typed);
			ref2 = zend_jit_addr_offset(jit, ref2, offsetof(zend_reference, val));
			zend_jit_merge_2(jit, non_ref_path, zend_jit_end(jit));
			ref = zend_jit_phi_2(jit, ref, ref2);
			var_addr = var_use_addr = ZEND_ADDR_REF_ZVAL(ref);
		} else {
			zend_jit_if_false(jit, if_typed);
		}
	}

	if (var_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
		ir_ref ref, counter, if_not_zero;

		if (var_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
			if_refcounted = zend_jit_if_zval_refcounted(jit, var_use_addr);
			zend_jit_if_false(jit, if_refcounted);
			zend_jit_array_add(simple_inputs, zend_jit_end(jit));
			zend_jit_if_true_cold(jit, if_refcounted);
		} else if (RC_MAY_BE_1(var_info)) {
			done = 1;
		}
		ref = zend_jit_zval_ptr(jit, var_use_addr);
		if (RC_MAY_BE_1(var_info)) {
			if (!zend_jit_simple_assign(jit, opline, var_addr, var_info, var_def_info, val_type, val_addr, val_info, res_addr, 0)) {
				return 0;
			}
			counter = zend_jit_gc_delref(jit, ref);

			if_not_zero = zend_jit_if(jit, counter);
			zend_jit_if_false(jit, if_not_zero);
			zend_jit_zval_dtor(jit, ref, var_info, opline);
			if (check_exception) {
				zend_jit_check_exception(jit);
			}
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_if_true(jit, if_not_zero);
			if (RC_MAY_BE_N(var_info) && (var_info & (MAY_BE_ARRAY|MAY_BE_OBJECT)) != 0) {
				ir_ref if_may_leak = zend_jit_if_gc_may_not_leak(jit, ref);
				zend_jit_if_false(jit, if_may_leak);
				zend_jit_call_1(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)gc_possible_root, IR_CONST_FASTCALL_FUNC),
					ref);

				if (Z_MODE(var_addr) == IS_REG || Z_MODE(res_addr) == IS_REG) {
					jit->delay_buf[jit->delay_count] = jit->delay_buf[jit->delay_count - 1];
					jit->delay_count++;
				}
				if (check_exception && (val_info & MAY_BE_UNDEF)) {
					zend_jit_check_exception(jit);
				}
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_may_leak);
			}
			if (Z_MODE(var_addr) == IS_REG || Z_MODE(res_addr) == IS_REG) {
				jit->delay_buf[jit->delay_count] = jit->delay_buf[jit->delay_count - 1];
				jit->delay_count++;
			}
			if (check_exception && (val_info & MAY_BE_UNDEF)) {
				zend_jit_check_exception(jit);
			}
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		} else /* if (RC_MAY_BE_N(var_info)) */ {
			zend_jit_gc_delref(jit, ref);
			if (var_info & (MAY_BE_ARRAY|MAY_BE_OBJECT)) {
				ir_ref if_may_leak = zend_jit_if_gc_may_not_leak(jit, ref);
				zend_jit_if_false(jit, if_may_leak);
				zend_jit_call_1(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)gc_possible_root, IR_CONST_FASTCALL_FUNC),
					ref);
				zend_jit_array_add(simple_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_may_leak);
			}
			zend_jit_array_add(simple_inputs, zend_jit_end(jit));
		}
	}

	if (simple_inputs_count) {
		zend_jit_merge_N(jit, simple_inputs_count, simple_inputs);
	}

	if (!done) {
		if (!zend_jit_simple_assign(jit, opline, var_addr, var_info, var_def_info, val_type, val_addr, val_info, res_addr, check_exception)) {
			return 0;
		}
		if (end_inputs_count) {
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	}

	if (end_inputs_count) {
		zend_jit_merge_N(jit, end_inputs_count, end_inputs);
	}

	if (Z_MODE(var_addr) == IS_REG || Z_MODE(res_addr) == IS_REG) {
		int count = jit->delay_count;
		ir_ref phi;

		jit->delay_var = -1;
		jit->delay_buf = NULL;
		jit->delay_count = 0;
		ZEND_ASSERT(end_inputs_count == count || (end_inputs_count == 0 && count == 1));
		if (count == 1) {
			phi = res_inputs[0];
		} else {
			phi = zend_jit_phi_N(jit, count, res_inputs);
		}
		if (Z_MODE(var_addr) == IS_REG) {
			zend_jit_def_reg(jit, var_addr, phi);
			if (real_res_addr) {
				if (var_def_info & MAY_BE_LONG) {
					zend_jit_zval_set_lval(jit, real_res_addr,
						zend_jit_zval_lval(jit, var_addr));
				} else {
					zend_jit_zval_set_dval(jit, real_res_addr,
						zend_jit_zval_dval(jit, var_addr));
				}
			}
		} else {
			zend_jit_def_reg(jit, res_addr, phi);
		}
	}

	return 1;
}

static int zend_jit_qm_assign(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, zend_jit_addr op1_def_addr, uint32_t res_use_info, uint32_t res_info, zend_jit_addr res_addr)
{
	if (op1_addr != op1_def_addr) {
		if (!zend_jit_update_regs(jit, opline->op1.var, op1_addr, op1_def_addr, op1_info)) {
			return 0;
		}
		if (Z_MODE(op1_def_addr) == IS_REG && Z_MODE(op1_addr) != IS_REG) {
			op1_addr = op1_def_addr;
		}
	}

	if (!zend_jit_simple_assign(jit, opline, res_addr, res_use_info, res_info, opline->op1_type, op1_addr, op1_info, 0, 1)) {
		return 0;
	}
	if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, res_info)) {
		return 0;
	}
	return 1;
}

static int zend_jit_assign(zend_jit_ctx  *jit,
                           const zend_op *opline,
                           uint32_t       op1_info,
                           zend_jit_addr  op1_use_addr,
                           uint32_t       op1_def_info,
                           zend_jit_addr  op1_addr,
                           uint32_t       op2_info,
                           zend_jit_addr  op2_addr,
                           zend_jit_addr  op2_def_addr,
                           uint32_t       res_info,
                           zend_jit_addr  res_addr,
                           zend_jit_addr  ref_addr,
                           int            may_throw)
{
	ZEND_ASSERT(opline->op1_type == IS_CV);

	if (op2_addr != op2_def_addr) {
		if (!zend_jit_update_regs(jit, opline->op2.var, op2_addr, op2_def_addr, op2_info)) {
			return 0;
		}
		if (Z_MODE(op2_def_addr) == IS_REG && Z_MODE(op2_addr) != IS_REG) {
			op2_addr = op2_def_addr;
		}
	}

	if (Z_MODE(op1_addr) != IS_REG
	 && Z_MODE(op1_use_addr) == IS_REG
	 && !Z_LOAD(op1_use_addr)
	 && !Z_STORE(op1_use_addr)) {
		/* Force type update */
		op1_info |= MAY_BE_UNDEF;
	}
	if (!zend_jit_assign_to_variable(jit, opline, op1_use_addr, op1_addr, op1_info, op1_def_info,
			opline->op2_type, op2_addr, op2_info, res_addr, ref_addr, may_throw)) {
		return 0;
	}
	if (!zend_jit_store_var_if_necessary_ex(jit, opline->op1.var, op1_addr, op1_def_info, op1_use_addr, op1_info)) {
		return 0;
	}
	if (opline->result_type != IS_UNUSED) {
		if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, res_info)) {
			return 0;
		}
	}

	return 1;
}

static ir_op zend_jit_cmp_op(const zend_op *opline, zend_uchar smart_branch_opcode, const void *exit_addr)
{
	ir_op op;

	switch (opline->opcode) {
		case ZEND_IS_EQUAL:
		case ZEND_IS_IDENTICAL:
		case ZEND_CASE:
		case ZEND_CASE_STRICT:
			op = IR_EQ;
			break;
		case ZEND_IS_NOT_EQUAL:
			op = IR_NE;
			break;
		case ZEND_IS_NOT_IDENTICAL:
			op = (exit_addr) ? IR_EQ : IR_NE;
			break;
		case ZEND_IS_SMALLER:
			op = IR_LT;
			break;
		case ZEND_IS_SMALLER_OR_EQUAL:
			op = IR_LE;
			break;
		default:
			ZEND_UNREACHABLE();
	}
	return op;
}

static int zend_jit_cmp_long_long(zend_jit_ctx   *jit,
                                  const zend_op  *opline,
                                  zend_ssa_range *op1_range,
                                  zend_jit_addr   op1_addr,
                                  zend_ssa_range *op2_range,
                                  zend_jit_addr   op2_addr,
                                  zend_jit_addr   res_addr,
                                  zend_uchar      smart_branch_opcode,
                                  uint32_t        target_label,
                                  uint32_t        target_label2,
                                  const void     *exit_addr,
                                  bool       skip_comparison)
{
//	bool swap = 0;
//	bool result;
	ir_op op;
	ir_ref op1, op2, ref;

#if 0
	if (zend_jit_is_constant_cmp_long_long(opline, op1_range, op1_addr, op2_range, op2_addr, &result)) {
		if (!smart_branch_opcode ||
		    smart_branch_opcode == ZEND_JMPZ_EX ||
		    smart_branch_opcode == ZEND_JMPNZ_EX) {
			zend_jit_zval_set_type_info(jit, res_addr,
				ir_const_u32(&jit->ctx, result ? IS_TRUE : IS_FALSE));
		}
		if (smart_branch_opcode && !exit_addr) {
			ZEND_ASSERT(jit->b >= 0);
			jit->bb_end_ref[jit->b] = zend_jit_end(jit);
			jit->control = IR_UNUSED;
			jit->b = -1;
#if 0
			if (smart_branch_opcode == ZEND_JMPZ ||
			    smart_branch_opcode == ZEND_JMPZ_EX) {
				if (!result) {
//???					| jmp => target_label
				}
			} else if (smart_branch_opcode == ZEND_JMPNZ ||
			           smart_branch_opcode == ZEND_JMPNZ_EX) {
				if (result) {
//???					| jmp => target_label
				}
			} else {
				ZEND_UNREACHABLE();
			}
#endif
		}
		return 1;
	}
#endif

    op = zend_jit_cmp_op(opline, smart_branch_opcode, exit_addr);
	op1 = zend_jit_zval_lval(jit, op1_addr);
	op2 = zend_jit_zval_lval(jit, op2_addr);
	ref = ir_fold2(&jit->ctx, IR_OPT(op, IR_BOOL), op1, op2);

	if (!smart_branch_opcode || smart_branch_opcode == ZEND_JMPNZ_EX || smart_branch_opcode == ZEND_JMPZ_EX) {
		zend_jit_zval_ref_set_type_info(jit,
			zend_jit_mem_zval_addr(jit, res_addr),
			ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
				ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref),
				ir_const_u32(&jit->ctx, IS_FALSE)));
	}
	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) {
			zend_jit_guard(jit, ref,
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		} else {
			zend_jit_guard_not(jit, ref,
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		}
	} else if (smart_branch_opcode) {
		zend_jit_if_ex(jit, ref,
			(smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ? target_label2 : target_label);
	}

	return 1;
}

static int zend_jit_cmp_long_double(zend_jit_ctx *jit, const zend_op *opline, zend_jit_addr op1_addr, zend_jit_addr op2_addr, zend_jit_addr res_addr, zend_uchar smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr)
{
	ir_op op;
	ir_ref op1, op2, ref;

    op = zend_jit_cmp_op(opline, smart_branch_opcode, exit_addr);
	op1 = zend_jit_zval_lval(jit, op1_addr);
	op2 = zend_jit_zval_dval(jit, op2_addr);
	ref = ir_fold2(&jit->ctx, IR_OPT(op, IR_BOOL), ir_fold1(&jit->ctx, IR_OPT(IR_INT2FP, IR_DOUBLE), op1), op2);

	if (!smart_branch_opcode || smart_branch_opcode == ZEND_JMPNZ_EX || smart_branch_opcode == ZEND_JMPZ_EX) {
		zend_jit_zval_ref_set_type_info(jit,
			zend_jit_mem_zval_addr(jit, res_addr),
			ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
				ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref),
				ir_const_u32(&jit->ctx, IS_FALSE)));
	}
	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) {
			zend_jit_guard(jit, ref,
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		} else {
			zend_jit_guard_not(jit, ref,
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		}
	} else if (smart_branch_opcode) {
		zend_jit_if_ex(jit, ref,
			(smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ? target_label2 : target_label);
	}
	return 1;
}

static int zend_jit_cmp_double_long(zend_jit_ctx *jit, const zend_op *opline, zend_jit_addr op1_addr, zend_jit_addr op2_addr, zend_jit_addr res_addr, zend_uchar smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr)
{
	ir_op op;
	ir_ref op1, op2, ref;

    op = zend_jit_cmp_op(opline, smart_branch_opcode, exit_addr);
	op1 = zend_jit_zval_dval(jit, op1_addr);
	op2 = zend_jit_zval_lval(jit, op2_addr);
	ref = ir_fold2(&jit->ctx, IR_OPT(op, IR_BOOL), op1, ir_fold1(&jit->ctx, IR_OPT(IR_INT2FP, IR_DOUBLE), op2));

	if (!smart_branch_opcode || smart_branch_opcode == ZEND_JMPNZ_EX || smart_branch_opcode == ZEND_JMPZ_EX) {
		zend_jit_zval_ref_set_type_info(jit,
			zend_jit_mem_zval_addr(jit, res_addr),
			ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
				ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref),
				ir_const_u32(&jit->ctx, IS_FALSE)));
	}
	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) {
			zend_jit_guard(jit, ref,
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		} else {
			zend_jit_guard_not(jit, ref,
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		}
	} else if (smart_branch_opcode) {
		zend_jit_if_ex(jit, ref,
			(smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ? target_label2 : target_label);
	}
	return 1;
}

static int zend_jit_cmp_double_double(zend_jit_ctx *jit, const zend_op *opline, zend_jit_addr op1_addr, zend_jit_addr op2_addr, zend_jit_addr res_addr, zend_uchar smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr)
{
	ir_op op;
	ir_ref op1, op2, ref;

    op = zend_jit_cmp_op(opline, smart_branch_opcode, exit_addr);
	op1 = zend_jit_zval_dval(jit, op1_addr);
	op2 = zend_jit_zval_dval(jit, op2_addr);
	ref = ir_fold2(&jit->ctx, IR_OPT(op, IR_BOOL), op1, op2);

	if (!smart_branch_opcode || smart_branch_opcode == ZEND_JMPNZ_EX || smart_branch_opcode == ZEND_JMPZ_EX) {
		zend_jit_zval_ref_set_type_info(jit,
			zend_jit_mem_zval_addr(jit, res_addr),
			ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
				ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref),
				ir_const_u32(&jit->ctx, IS_FALSE)));
	}
	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) {
			zend_jit_guard(jit, ref,
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		} else {
			zend_jit_guard_not(jit, ref,
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		}
	} else if (smart_branch_opcode) {
		zend_jit_if_ex(jit, ref,
			(smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ? target_label2 : target_label);
	}
	return 1;
}

static int zend_jit_cmp_slow(zend_jit_ctx *jit, ir_ref ref, const zend_op *opline, zend_jit_addr res_addr, zend_uchar smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr)
{
	ir_op op;

    op = zend_jit_cmp_op(opline, smart_branch_opcode, exit_addr);
	ref = ir_fold2(&jit->ctx, IR_OPT(op, IR_BOOL), ref, ir_const_i32(&jit->ctx, 0));

	if (!smart_branch_opcode || smart_branch_opcode == ZEND_JMPNZ_EX || smart_branch_opcode == ZEND_JMPZ_EX) {
		zend_jit_zval_ref_set_type_info(jit,
			zend_jit_mem_zval_addr(jit, res_addr),
			ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
				ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref),
				ir_const_u32(&jit->ctx, IS_FALSE)));
	}
	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) {
			zend_jit_guard(jit, ref,
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		} else {
			zend_jit_guard_not(jit, ref,
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		}
	} else if (smart_branch_opcode) {
		zend_jit_if_ex(jit, ref,
			(smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ? target_label2 : target_label);
	}

	return 1;
}

static int zend_jit_cmp(zend_jit_ctx   *jit,
                        const zend_op  *opline,
                        uint32_t        op1_info,
                        zend_ssa_range *op1_range,
                        zend_jit_addr   op1_addr,
                        uint32_t        op2_info,
                        zend_ssa_range *op2_range,
                        zend_jit_addr   op2_addr,
                        zend_jit_addr   res_addr,
                        int             may_throw,
                        zend_uchar      smart_branch_opcode,
                        uint32_t        target_label,
                        uint32_t        target_label2,
                        const void     *exit_addr,
                        bool       skip_comparison)
{
	ir_ref if_op1_long = IR_UNUSED;
	ir_ref if_op1_double = IR_UNUSED;
	ir_ref if_op2_double = IR_UNUSED;
	ir_ref if_op1_long_op2_long = IR_UNUSED;
	ir_ref if_op1_long_op2_double = IR_UNUSED;
	ir_ref if_op1_double_op2_double = IR_UNUSED;
	ir_ref if_op1_double_op2_long = IR_UNUSED;
	ir_ref end_inputs[5], slow_inputs[4];
	uint32_t slow_inputs_count = 0;
	uint32_t end_inputs_count = 0;
	bool same_ops = zend_jit_same_addr(op1_addr, op2_addr);
	bool has_slow =
		(op1_info & (MAY_BE_LONG|MAY_BE_DOUBLE)) &&
		(op2_info & (MAY_BE_LONG|MAY_BE_DOUBLE)) &&
		((op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) ||
		 (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))));

	if ((op1_info & MAY_BE_LONG) && (op2_info & MAY_BE_LONG)) {
		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG)) {
			if_op1_long = zend_jit_if_zval_type(jit, op1_addr, IS_LONG);
			zend_jit_if_true(jit, if_op1_long);
		}
		if (!same_ops && (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG))) {
			if_op1_long_op2_long = zend_jit_if_zval_type(jit, op2_addr, IS_LONG);
			zend_jit_if_false_cold(jit, if_op1_long_op2_long);
			if (op2_info & MAY_BE_DOUBLE) {
				if (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) {
					if_op1_long_op2_double = zend_jit_if_zval_type(jit, op2_addr, IS_DOUBLE);
					zend_jit_if_false_cold(jit, if_op1_long_op2_double);
					zend_jit_array_add(slow_inputs, zend_jit_end(jit));
					zend_jit_if_true(jit, if_op1_long_op2_double);
				}
				if (!zend_jit_cmp_long_double(jit, opline, op1_addr, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
					return 0;
				}
				zend_jit_array_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
					jit->control : zend_jit_end(jit));
			} else {
				zend_jit_array_add(slow_inputs, zend_jit_end(jit));
			}
			zend_jit_if_true(jit, if_op1_long_op2_long);
		}
		if (!zend_jit_cmp_long_long(jit, opline, op1_range, op1_addr, op2_range, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr, skip_comparison)) {
			return 0;
		}
		zend_jit_array_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
			jit->control : zend_jit_end(jit));

		if (if_op1_long) {
			zend_jit_if_false_cold(jit, if_op1_long);
		}
		if (op1_info & MAY_BE_DOUBLE) {
			if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) {
				if_op1_double = zend_jit_if_zval_type(jit, op1_addr, IS_DOUBLE);
				zend_jit_if_false_cold(jit, if_op1_double);
				zend_jit_array_add(slow_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_op1_double);
			}
			if (op2_info & MAY_BE_DOUBLE) {
				if (!same_ops && (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_DOUBLE))) {
					if_op1_double_op2_double = zend_jit_if_zval_type(jit, op2_addr, IS_DOUBLE);
					zend_jit_if_true(jit, if_op1_double_op2_double);
				}
				if (!zend_jit_cmp_double_double(jit, opline, op1_addr, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
					return 0;
				}
				zend_jit_array_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
					jit->control : zend_jit_end(jit));
				if (if_op1_double_op2_double) {
					zend_jit_if_false_cold(jit, if_op1_double_op2_double);
				}
			}
			if (!same_ops) {
				if (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) {
					if_op1_double_op2_long = zend_jit_if_zval_type(jit, op2_addr, IS_LONG);
					zend_jit_if_false_cold(jit, if_op1_double_op2_long);
					zend_jit_array_add(slow_inputs, zend_jit_end(jit));
					zend_jit_if_true(jit, if_op1_double_op2_long);
				}
				if (!zend_jit_cmp_double_long(jit, opline, op1_addr, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
					return 0;
				}
				zend_jit_array_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
					jit->control : zend_jit_end(jit));
			} else if (if_op1_double_op2_double) {
				zend_jit_array_add(slow_inputs, zend_jit_end(jit));
			}
		} else if (if_op1_long) {
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));
		}
	} else if ((op1_info & MAY_BE_DOUBLE) &&
	           !(op1_info & MAY_BE_LONG) &&
	           (op2_info & (MAY_BE_LONG|MAY_BE_DOUBLE))) {
		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_DOUBLE)) {
			if_op1_double = zend_jit_if_zval_type(jit, op1_addr, IS_DOUBLE);
			zend_jit_if_false_cold(jit, if_op1_double);
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));
			zend_jit_if_true(jit, if_op1_double);
		}
		if (op2_info & MAY_BE_DOUBLE) {
			if (!same_ops && (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_DOUBLE))) {
				if_op1_double_op2_double = zend_jit_if_zval_type(jit, op2_addr, IS_DOUBLE);
				zend_jit_if_true(jit, if_op1_double_op2_double);
			}
			if (!zend_jit_cmp_double_double(jit, opline, op1_addr, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
				return 0;
			}
			zend_jit_array_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
				jit->control : zend_jit_end(jit));
			if (if_op1_double_op2_double) {
				zend_jit_if_false_cold(jit, if_op1_double_op2_double);
			}
		}
		if (!same_ops && (op2_info & MAY_BE_LONG)) {
			if (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_DOUBLE|MAY_BE_LONG))) {
				if_op1_double_op2_long = zend_jit_if_zval_type(jit, op2_addr, IS_LONG);
				zend_jit_if_false_cold(jit, if_op1_double_op2_long);
				zend_jit_array_add(slow_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_op1_double_op2_long);
			}
			if (!zend_jit_cmp_double_long(jit, opline, op1_addr, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
				return 0;
			}
			zend_jit_array_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
				jit->control : zend_jit_end(jit));
		} else if (if_op1_double_op2_double) {
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));
		}
	} else if ((op2_info & MAY_BE_DOUBLE) &&
	           !(op2_info & MAY_BE_LONG) &&
	           (op1_info & (MAY_BE_LONG|MAY_BE_DOUBLE))) {
		if (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_DOUBLE)) {
			if_op2_double = zend_jit_if_zval_type(jit, op2_addr, IS_DOUBLE);
			zend_jit_if_false_cold(jit, if_op2_double);
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));
			zend_jit_if_true(jit, if_op2_double);
		}
		if (op1_info & MAY_BE_DOUBLE) {
			if (!same_ops && (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_DOUBLE))) {
				if_op1_double_op2_double = zend_jit_if_zval_type(jit, op1_addr, IS_DOUBLE);
				zend_jit_if_true(jit, if_op1_double_op2_double);
			}
			if (!zend_jit_cmp_double_double(jit, opline, op1_addr, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
				return 0;
			}
			zend_jit_array_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
				jit->control : zend_jit_end(jit));
			if (if_op1_double_op2_double) {
				zend_jit_if_false_cold(jit, if_op1_double_op2_double);
			}
		}
		if (!same_ops && (op1_info & MAY_BE_LONG)) {
			if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_DOUBLE|MAY_BE_LONG))) {
				if_op1_long_op2_double = zend_jit_if_zval_type(jit, op1_addr, IS_LONG);
				zend_jit_if_false_cold(jit, if_op1_long_op2_double);
				zend_jit_array_add(slow_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_op1_long_op2_double);
			}
			if (!zend_jit_cmp_long_double(jit, opline, op1_addr, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
				return 0;
			}
			zend_jit_array_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
				jit->control : zend_jit_end(jit));
		} else if (if_op1_double_op2_double) {
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));
		}
	}

	if (has_slow ||
	    (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) ||
	    (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE)))) {
	    ir_ref op1, op2, ref;

		if (slow_inputs_count) {
			zend_jit_merge_N(jit, slow_inputs_count, slow_inputs);
		}
		zend_jit_set_ex_opline(jit, opline);

		if (Z_MODE(op1_addr) == IS_REG) {
			zend_jit_addr real_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->op1.var);
			if (!zend_jit_spill_store(jit, op1_addr, real_addr, op1_info, 1)) {
				return 0;
			}
			op1_addr = real_addr;
		}
		if (Z_MODE(op2_addr) == IS_REG) {
			zend_jit_addr real_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->op2.var);
			if (!zend_jit_spill_store(jit, op2_addr, real_addr, op2_info, 1)) {
				return 0;
			}
			op2_addr = real_addr;
		}

		op1 = zend_jit_zval_addr(jit, op1_addr);
		if (opline->op1_type == IS_CV && (op1_info & MAY_BE_UNDEF)) {
			op1 = zend_jit_zval_check_undef(jit, op1, opline->op1.var, NULL, 0);
		}
		op2 = zend_jit_zval_addr(jit, op2_addr);
		if (opline->op2_type == IS_CV && (op2_info & MAY_BE_UNDEF)) {
			op2 = zend_jit_zval_check_undef(jit, op2, opline->op2.var, NULL, 0);
		}
		ref = zend_jit_call_2(jit, IR_I32,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_compare, IR_CONST_FASTCALL_FUNC),
			op1, op2);
		if (opline->opcode != ZEND_CASE) {
			zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, NULL);
		}
		zend_jit_free_op(jit, opline->op2_type, opline->op2, op2_info, NULL);
		if (may_throw) {
			zend_jit_check_exception_undef_result(jit, opline);
		}

		if (!zend_jit_cmp_slow(jit, ref, opline, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
			return 0;
		}

		zend_jit_array_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
			jit->control : zend_jit_end(jit));
	}

	if (end_inputs_count) {
		uint32_t n = end_inputs_count;

		if (smart_branch_opcode && !exit_addr) {
			zend_basic_block *bb;
			ir_ref ref;
			uint32_t label = (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ?
				target_label2 : target_label;
			uint32_t label2 = (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ?
				target_label : target_label2;

			ZEND_ASSERT(jit->b >= 0);
			bb = &jit->ssa->cfg.blocks[jit->b];
			ZEND_ASSERT(bb->successors_count == 2);

			if (UNEXPECTED(bb->successors[0] == bb->successors[1])) {
				ir_ref merge_inputs[10];
				uint32_t merge_inputs_count = 0;

				while (n) {
					n--;
					zend_jit_if_true(jit, end_inputs[n]);
					zend_jit_array_add(merge_inputs, zend_jit_end(jit));
					zend_jit_if_false(jit, end_inputs[n]);
					zend_jit_array_add(merge_inputs, zend_jit_end(jit));
				}
				zend_jit_merge_N(jit, merge_inputs_count, merge_inputs);
				_zend_jit_add_predecessor_ref(jit, label, jit->b, zend_jit_end(jit));
			} else if (n == 1) {
				ref = end_inputs[0];
				_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, ref);
				_zend_jit_add_predecessor_ref(jit, bb->successors[1], jit->b, ref);
			} else {
				ir_ref true_inputs[5], false_inputs[5];

				while (n) {
					n--;
					zend_jit_if_true(jit, end_inputs[n]);
					true_inputs[n] = zend_jit_end(jit);
					zend_jit_if_false(jit, end_inputs[n]);
					false_inputs[n] = zend_jit_end(jit);
				}
				n = end_inputs_count;
				zend_jit_merge_N(jit, n, true_inputs);
				_zend_jit_add_predecessor_ref(jit, label, jit->b, zend_jit_end(jit));
				zend_jit_merge_N(jit, n, false_inputs);
				_zend_jit_add_predecessor_ref(jit, label2, jit->b, zend_jit_end(jit));
			}
			jit->control = IR_UNUSED;
			jit->b = -1;
		} else {
			zend_jit_merge_N(jit, n, end_inputs);
		}
	}

	return 1;
}

static int zend_jit_identical(zend_jit_ctx   *jit,
                              const zend_op  *opline,
                              uint32_t        op1_info,
                              zend_ssa_range *op1_range,
                              zend_jit_addr   op1_addr,
                              uint32_t        op2_info,
                              zend_ssa_range *op2_range,
                              zend_jit_addr   op2_addr,
                              zend_jit_addr   res_addr,
                              int             may_throw,
                              zend_uchar      smart_branch_opcode,
                              uint32_t        target_label,
                              uint32_t        target_label2,
                              const void     *exit_addr,
                              bool       skip_comparison)
{
	bool always_false = 0, always_true = 0;
	ir_ref ref;

	if (opline->op1_type == IS_CV && (op1_info & MAY_BE_UNDEF)) {
		ir_ref op1 = zend_jit_zval_addr(jit, op1_addr);
		op1 = zend_jit_zval_check_undef(jit, op1, opline->op1.var, NULL, 0);
		op1_info |= MAY_BE_NULL;
		op1_addr = ZEND_ADDR_REF_ZVAL(op1);
	}
	if (opline->op2_type == IS_CV && (op2_info & MAY_BE_UNDEF)) {
		ir_ref op2 = zend_jit_zval_addr(jit, op2_addr);
		op2 = zend_jit_zval_check_undef(jit, op2, opline->op2.var, NULL, 0);
		op2_info |= MAY_BE_NULL;
		op2_addr = ZEND_ADDR_REF_ZVAL(op2);
	}

	if ((op1_info & op2_info & MAY_BE_ANY) == 0) {
		always_false = 1;
	} else if (has_concrete_type(op1_info)
	 && has_concrete_type(op2_info)
	 && concrete_type(op1_info) == concrete_type(op2_info)
	 && concrete_type(op1_info) <= IS_TRUE) {
		always_true = 1;
	} else if (Z_MODE(op1_addr) == IS_CONST_ZVAL && Z_MODE(op2_addr) == IS_CONST_ZVAL) {
		if (zend_is_identical(Z_ZV(op1_addr), Z_ZV(op2_addr))) {
			always_true = 1;
		} else {
			always_false = 1;
		}
	}

	if (always_true) {
		if (opline->opcode != ZEND_CASE_STRICT) {
			zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);
		}
		zend_jit_free_op(jit, opline->op2_type, opline->op2, op2_info, opline);
		if (!smart_branch_opcode
		 || smart_branch_opcode == ZEND_JMPZ_EX
		 || smart_branch_opcode == ZEND_JMPNZ_EX) {
			zend_jit_zval_set_type_info(jit, res_addr, opline->opcode != ZEND_IS_NOT_IDENTICAL ? IS_TRUE : IS_FALSE);
		}
		if (may_throw) {
			zend_jit_check_exception(jit);
		}
		if (exit_addr) {
			if (smart_branch_opcode == ZEND_JMPNZ) {
				zend_jit_side_exit(jit,
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			}
		} else if (smart_branch_opcode) {
			uint32_t label;

			if (opline->opcode == ZEND_IS_NOT_IDENTICAL) {
				label = (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ?
					target_label : target_label2;
			} else {
				label = (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ?
					target_label2 : target_label;
			}
			_zend_jit_add_predecessor_ref(jit, label, jit->b, zend_jit_end(jit));
			jit->control = IR_UNUSED;
			jit->b = -1;
		}
		return 1;
	} else if (always_false) {
		if (opline->opcode != ZEND_CASE_STRICT) {
			zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);
		}
		zend_jit_free_op(jit, opline->op2_type, opline->op2, op2_info, opline);
		if (!smart_branch_opcode
		 || smart_branch_opcode == ZEND_JMPZ_EX
		 || smart_branch_opcode == ZEND_JMPNZ_EX) {
			zend_jit_zval_set_type_info(jit, res_addr, opline->opcode != ZEND_IS_NOT_IDENTICAL ? IS_FALSE : IS_TRUE);
		}
		if (may_throw) {
			zend_jit_check_exception(jit);
		}
		if (exit_addr) {
			if (smart_branch_opcode == ZEND_JMPZ) {
				zend_jit_side_exit(jit,
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			}
		} else if (smart_branch_opcode) {
			uint32_t label;

			if (opline->opcode == ZEND_IS_NOT_IDENTICAL) {
				label = (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ?
					target_label2 : target_label;
			} else {
				label = (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ?
					target_label : target_label2;
			}
			_zend_jit_add_predecessor_ref(jit, label, jit->b, zend_jit_end(jit));
			jit->control = IR_UNUSED;
			jit->b = -1;
		}
		return 1;
	}

	if ((opline->op1_type & (IS_CV|IS_VAR)) && (op1_info & MAY_BE_REF)) {
		ref = zend_jit_zval_addr(jit, op1_addr);
		ref = zend_jit_zval_ref_deref(jit, ref);
		op1_addr = ZEND_ADDR_REF_ZVAL(ref);
	}
	if ((opline->op2_type & (IS_CV|IS_VAR)) && (op2_info & MAY_BE_REF)) {
		ref = zend_jit_zval_addr(jit, op2_addr);
		ref = zend_jit_zval_ref_deref(jit, ref);
		op2_addr = ZEND_ADDR_REF_ZVAL(ref);
	}

	if ((op1_info & (MAY_BE_REF|MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_LONG &&
	    (op2_info & (MAY_BE_REF|MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_LONG) {
		if (!zend_jit_cmp_long_long(jit, opline, op1_range, op1_addr, op2_range, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr, skip_comparison)) {
			return 0;
		}
	} else if ((op1_info & (MAY_BE_REF|MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_DOUBLE &&
	           (op2_info & (MAY_BE_REF|MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_DOUBLE) {
		if (!zend_jit_cmp_double_double(jit, opline, op1_addr, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
			return 0;
		}
	} else {
		if (opline->op1_type != IS_CONST) {
			if (Z_MODE(op1_addr) == IS_REG) {
				zend_jit_addr real_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->op1.var);
				if (!zend_jit_spill_store(jit, op1_addr, real_addr, op1_info, 1)) {
					return 0;
				}
				op1_addr = real_addr;
			}
		}
		if (opline->op2_type != IS_CONST) {
			if (Z_MODE(op2_addr) == IS_REG) {
				zend_jit_addr real_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->op2.var);
				if (!zend_jit_spill_store(jit, op2_addr, real_addr, op2_info, 1)) {
					return 0;
				}
			}
		}

		if (Z_MODE(op1_addr) == IS_CONST_ZVAL && Z_TYPE_P(Z_ZV(op1_addr)) <= IS_TRUE) {
			zval *val = Z_ZV(op1_addr);

			ref = ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
				zend_jit_zval_type(jit, op2_addr),
				ir_const_u8(&jit->ctx, Z_TYPE_P(val)));
		} else if (Z_MODE(op2_addr) == IS_CONST_ZVAL && Z_TYPE_P(Z_ZV(op2_addr)) <= IS_TRUE) {
			zval *val = Z_ZV(op2_addr);

			ref = ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
				zend_jit_zval_type(jit, op1_addr),
				ir_const_u8(&jit->ctx, Z_TYPE_P(val)));
		} else {
			ir_ref op1 = zend_jit_zval_addr(jit, op1_addr);
			ir_ref op2 = zend_jit_zval_addr(jit, op2_addr);

			ref = zend_jit_call_2(jit, IR_BOOL,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_is_identical, IR_CONST_FASTCALL_FUNC),
				op1, op2);
		}

		if (!smart_branch_opcode || smart_branch_opcode == ZEND_JMPNZ_EX || smart_branch_opcode == ZEND_JMPZ_EX) {
			if (opline->opcode == ZEND_IS_NOT_IDENTICAL) {
				zend_jit_zval_ref_set_type_info(jit,
					zend_jit_mem_zval_addr(jit, res_addr),
					ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_U32),
						ir_const_u32(&jit->ctx, IS_TRUE),
						ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref)));
			} else {
				zend_jit_zval_ref_set_type_info(jit,
					zend_jit_mem_zval_addr(jit, res_addr),
					ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
						ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref),
						ir_const_u32(&jit->ctx, IS_FALSE)));
			}
		}
		if (opline->opcode != ZEND_CASE_STRICT) {
			zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, NULL);
		}
		zend_jit_free_op(jit, opline->op2_type, opline->op2, op2_info, NULL);
		if (may_throw) {
			zend_jit_check_exception_undef_result(jit, opline);
		}
		if (exit_addr) {
			if (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) {
				zend_jit_guard(jit, ref,
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				zend_jit_guard_not(jit, ref,
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			}
		} else if (smart_branch_opcode) {
			if (opline->opcode == ZEND_IS_NOT_IDENTICAL) {
				/* swap labels */
				uint32_t tmp = target_label;
				target_label = target_label2;
				target_label2 = tmp;
			}
			zend_jit_if_ex(jit, ref,
				(smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ? target_label2 : target_label);
		}
	}

	if (exit_addr) {
		/* pass */
	} else if (smart_branch_opcode) {
		zend_basic_block *bb;
		ir_ref ref;

		ZEND_ASSERT(jit->b >= 0);
		bb = &jit->ssa->cfg.blocks[jit->b];
		ZEND_ASSERT(bb->successors_count == 2);

		ref = jit->control;
		if (bb->successors_count == 2 && bb->successors[0] == bb->successors[1]) {
			ir_ref true_path;
			zend_jit_if_true(jit, ref);
			true_path = zend_jit_end(jit);
			zend_jit_if_false(jit, ref);
			zend_jit_merge_2(jit, true_path, zend_jit_end(jit));
			_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, zend_jit_end(jit));
		} else {
			ZEND_ASSERT(bb->successors_count == 2);
			_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, ref);
			_zend_jit_add_predecessor_ref(jit, bb->successors[1], jit->b, ref);
		}
		jit->control = IR_UNUSED;
		jit->b = -1;
	}

	return 1;
}

static int zend_jit_bool_jmpznz(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, zend_jit_addr res_addr, uint32_t target_label, uint32_t target_label2, int may_throw, zend_uchar branch_opcode, const void *exit_addr)
{
	uint32_t true_label = -1;
	uint32_t false_label = -1;
	bool set_bool = 0;
	bool set_bool_not = 0;
	bool always_true = 0, always_false = 0;
	ir_ref ref, end_inputs[5], true_inputs[5], false_inputs[5];
	uint32_t end_inputs_count = 0, true_inputs_count = 0, false_inputs_count = 0;
	ir_type type = IR_UNUSED;

	if (branch_opcode == ZEND_BOOL) {
		set_bool = 1;
	} else if (branch_opcode == ZEND_BOOL_NOT) {
		set_bool = 1;
		set_bool_not = 1;
	} else if (branch_opcode == ZEND_JMPZ) {
		true_label = target_label2;
		false_label = target_label;
	} else if (branch_opcode == ZEND_JMPNZ) {
		true_label = target_label;
		false_label = target_label2;
	} else if (branch_opcode == ZEND_JMPZ_EX) {
		set_bool = 1;
		true_label = target_label2;
		false_label = target_label;
	} else if (branch_opcode == ZEND_JMPNZ_EX) {
		set_bool = 1;
		true_label = target_label;
		false_label = target_label2;
	} else {
		ZEND_UNREACHABLE();
	}

	if (opline->op1_type == IS_CV && (op1_info & MAY_BE_REF)) {
		ref = zend_jit_zval_addr(jit, op1_addr);
		ref = zend_jit_zval_ref_deref(jit, ref);
		op1_addr = ZEND_ADDR_REF_ZVAL(ref);
	}

	if (Z_MODE(op1_addr) == IS_CONST_ZVAL) {
		if (zend_is_true(Z_ZV(op1_addr))) {
			always_true = 1;
		} else {
			always_false = 1;
		}
	} else if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE)) {
		if (!(op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY)-MAY_BE_TRUE))) {
			always_true = 1;
		} else if (!(op1_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_FALSE)))) {
			if (opline->op1_type == IS_CV && (op1_info & MAY_BE_UNDEF)) {
				ref = zend_jit_zval_addr(jit, op1_addr);
				zend_jit_zval_check_undef(jit, ref, opline->op1.var, opline, 0);
			}
			always_false = 1;
		}
	}

	if (always_true) {
		if (set_bool) {
			zend_jit_zval_set_type_info(jit, res_addr, set_bool_not ? IS_FALSE : IS_TRUE);
		}
		zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);
		if (may_throw) {
			zend_jit_check_exception(jit);
		}
		if (true_label != (uint32_t)-1) {
			ZEND_ASSERT(exit_addr == NULL);
			_zend_jit_add_predecessor_ref(jit, true_label, jit->b, zend_jit_end(jit));
			jit->control = IR_UNUSED;
			jit->b = -1;
		}
		return 1;
	} else if (always_false) {
		if (set_bool) {
			zend_jit_zval_set_type_info(jit, res_addr, set_bool_not ? IS_TRUE : IS_FALSE);
		}
		zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);
		if (may_throw) {
			zend_jit_check_exception(jit);
		}
		if (false_label != (uint32_t)-1) {
			ZEND_ASSERT(exit_addr == NULL);
			_zend_jit_add_predecessor_ref(jit, false_label, jit->b, zend_jit_end(jit));
			jit->control = IR_UNUSED;
			jit->b = -1;
		}
		return 1;
	}

	if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE)) {
		type = zend_jit_zval_type(jit, op1_addr);
		if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_FALSE)) {
			ir_ref if_type = zend_jit_if(jit,
				ir_fold2(&jit->ctx, IR_OPT(IR_LT, IR_BOOL),
					type,
					ir_const_u8(&jit->ctx, IS_TRUE)));

			zend_jit_if_true_cold(jit, if_type);

			if (op1_info & MAY_BE_UNDEF) {
				zend_jit_type_check_undef(jit,
					type,
					opline->op1.var,
					opline, 1);
			}
			if (set_bool) {
				zend_jit_zval_set_type_info(jit, res_addr, set_bool_not ? IS_TRUE : IS_FALSE);
			}
			if (exit_addr) {
				if (branch_opcode == ZEND_JMPNZ) {
					zend_jit_array_add(end_inputs, zend_jit_end(jit));
				} else {
					zend_jit_side_exit(jit,
						zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				}
			} else if (false_label != (uint32_t)-1) {
				zend_jit_array_add(false_inputs, zend_jit_end(jit));
			} else {
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}
			zend_jit_if_false(jit, if_type);
		}

		if (op1_info & MAY_BE_TRUE) {
			ir_ref if_type = IR_UNUSED;

			if (op1_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE))) {
				if_type = zend_jit_if(jit,
					ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
						type,
						ir_const_u8(&jit->ctx, IS_TRUE)));

				zend_jit_if_true(jit, if_type);
			}
			if (set_bool) {
				zend_jit_zval_set_type_info(jit, res_addr, set_bool_not ? IS_FALSE : IS_TRUE);
			}
			if (exit_addr) {
				if (branch_opcode == ZEND_JMPZ) {
					zend_jit_array_add(end_inputs, zend_jit_end(jit));
				} else {
					zend_jit_side_exit(jit,
						zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				}
			} else if (true_label != (uint32_t)-1) {
				zend_jit_array_add(true_inputs, zend_jit_end(jit));
			} else {
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}
			if (if_type) {
				zend_jit_if_false(jit, if_type);
			}
		}
	}

	if (op1_info & MAY_BE_LONG) {
		ir_ref if_long = IR_UNUSED;
		ir_ref ref;

		if (op1_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG))) {
			if (!type) {
				type = zend_jit_zval_type(jit, op1_addr);
			}
			if_long = zend_jit_if(jit,
				ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
					type,
					ir_const_u8(&jit->ctx, IS_LONG)));
			zend_jit_if_true(jit, if_long);
		}
		ref = zend_jit_zval_lval(jit, op1_addr);
		if (branch_opcode == ZEND_BOOL || branch_opcode == ZEND_BOOL_NOT) {
			ref = ir_fold2(&jit->ctx, IR_OPT(IR_NE, IR_BOOL), ref,
				ir_const_php_long(&jit->ctx, 0));
			if (set_bool_not) {
				zend_jit_zval_ref_set_type_info(jit,
					zend_jit_mem_zval_addr(jit, res_addr),
					ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_U32),
						ir_const_u32(&jit->ctx, IS_TRUE),
						ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref)));
			} else {
				zend_jit_zval_ref_set_type_info(jit,
					zend_jit_mem_zval_addr(jit, res_addr),
					ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
						ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref),
						ir_const_u32(&jit->ctx, IS_FALSE)));
			}
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		} else if (exit_addr) {
			if (set_bool) {
				zend_jit_zval_ref_set_type_info(jit,
					zend_jit_mem_zval_addr(jit, res_addr),
					ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
						ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32),
							ir_fold2(&jit->ctx, IR_OPT(IR_NE, IR_BOOL), ref,
								ir_const_php_long(&jit->ctx, 0))),
						ir_const_u32(&jit->ctx, IS_FALSE)));
			}
			if (branch_opcode == ZEND_JMPZ || branch_opcode == ZEND_JMPZ_EX) {
				zend_jit_guard(jit, ref,
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				zend_jit_guard_not(jit, ref,
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			}
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		} else {
			ir_ref if_val = zend_jit_if(jit, ref);
			zend_jit_if_true(jit, if_val);
			if (set_bool) {
				zend_jit_zval_set_type_info(jit, res_addr, set_bool_not ? IS_FALSE : IS_TRUE);
			}
			zend_jit_array_add(true_inputs, zend_jit_end(jit));
			zend_jit_if_false(jit, if_val);
			if (set_bool) {
				zend_jit_zval_set_type_info(jit, res_addr, set_bool_not ? IS_TRUE : IS_FALSE);
			}
			zend_jit_array_add(false_inputs, zend_jit_end(jit));
		}
		if (if_long) {
			zend_jit_if_false(jit, if_long);
		}
	}

	if (op1_info & MAY_BE_DOUBLE) {
		ir_ref if_double = IR_UNUSED;
		ir_ref ref;

		if (op1_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE))) {
			if (!type) {
				type = zend_jit_zval_type(jit, op1_addr);
			}
			if_double = zend_jit_if(jit,
				ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
					type,
					ir_const_u8(&jit->ctx, IS_DOUBLE)));
			zend_jit_if_true(jit, if_double);
		}
		ref = ir_fold2(&jit->ctx, IR_OPT(IR_NE, IR_BOOL),
			zend_jit_zval_dval(jit, op1_addr),
			ir_const_double(&jit->ctx, 0.0));
		if (branch_opcode == ZEND_BOOL || branch_opcode == ZEND_BOOL_NOT) {
			if (set_bool_not) {
				zend_jit_zval_ref_set_type_info(jit,
					zend_jit_mem_zval_addr(jit, res_addr),
					ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_U32),
						ir_const_u32(&jit->ctx, IS_TRUE),
						ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref)));
			} else {
				zend_jit_zval_ref_set_type_info(jit,
					zend_jit_mem_zval_addr(jit, res_addr),
					ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
						ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref),
						ir_const_u32(&jit->ctx, IS_FALSE)));
			}
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		} else if (exit_addr) {
		    if (set_bool) {
				zend_jit_zval_ref_set_type_info(jit,
					zend_jit_mem_zval_addr(jit, res_addr),
					ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
						ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref),
						ir_const_u32(&jit->ctx, IS_FALSE)));
		    }
			if (branch_opcode == ZEND_JMPZ || branch_opcode == ZEND_JMPZ_EX) {
				zend_jit_guard(jit, ref,
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				zend_jit_guard_not(jit, ref,
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			}
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		} else {
			ir_ref if_val = zend_jit_if(jit, ref);
			zend_jit_if_true(jit, if_val);
			if (set_bool) {
				zend_jit_zval_set_type_info(jit, res_addr, set_bool_not ? IS_FALSE : IS_TRUE);
			}
			zend_jit_array_add(true_inputs, zend_jit_end(jit));
			zend_jit_if_false(jit, if_val);
			if (set_bool) {
				zend_jit_zval_set_type_info(jit, res_addr, set_bool_not ? IS_TRUE : IS_FALSE);
			}
			zend_jit_array_add(false_inputs, zend_jit_end(jit));
		}
		if (if_double) {
			zend_jit_if_false(jit, if_double);
		}
	}

	if (op1_info & (MAY_BE_ANY - (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE))) {
		zend_jit_set_ex_opline(jit, opline);
		ref = zend_jit_call_1(jit, IR_BOOL,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_is_true, IR_CONST_FASTCALL_FUNC),
			zend_jit_zval_addr(jit, op1_addr));
		zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, NULL);
		if (may_throw) {
			zend_jit_check_exception_undef_result(jit, opline);
		}
		if (branch_opcode == ZEND_BOOL || branch_opcode == ZEND_BOOL_NOT) {
			if (set_bool_not) {
				zend_jit_zval_ref_set_type_info(jit,
					zend_jit_mem_zval_addr(jit, res_addr),
					ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_U32),
						ir_const_u32(&jit->ctx, IS_TRUE),
						ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref)));
			} else {
				zend_jit_zval_ref_set_type_info(jit,
					zend_jit_mem_zval_addr(jit, res_addr),
					ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
						ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref),
						ir_const_u32(&jit->ctx, IS_FALSE)));
			}
			if (end_inputs_count) {
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}
		} else if (exit_addr) {
			if (set_bool) {
				zend_jit_zval_ref_set_type_info(jit,
					zend_jit_mem_zval_addr(jit, res_addr),
					ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
						ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref),
						ir_const_u32(&jit->ctx, IS_FALSE)));
			}
			if (branch_opcode == ZEND_JMPZ || branch_opcode == ZEND_JMPZ_EX) {
				zend_jit_guard(jit, ref,
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				zend_jit_guard_not(jit, ref,
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			}
			if (end_inputs_count) {
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}
		} else {
			ir_ref if_val = zend_jit_if(jit, ref);
			zend_jit_if_true(jit, if_val);
			if (set_bool) {
				zend_jit_zval_set_type_info(jit, res_addr, set_bool_not ? IS_FALSE : IS_TRUE);
			}
			zend_jit_array_add(true_inputs, zend_jit_end(jit));
			zend_jit_if_false(jit, if_val);
			if (set_bool) {
				zend_jit_zval_set_type_info(jit, res_addr, set_bool_not ? IS_TRUE : IS_FALSE);
			}
			zend_jit_array_add(false_inputs, zend_jit_end(jit));
		}
	}

	if (branch_opcode == ZEND_BOOL || branch_opcode == ZEND_BOOL_NOT || exit_addr) {
		if (end_inputs_count) {
			zend_jit_merge_N(jit, end_inputs_count, end_inputs);
		}
	} else {
		_zend_jit_merge_smart_branch_inputs(jit, true_label, false_label,
			true_inputs, true_inputs_count, false_inputs, false_inputs_count);
	}

	return 1;
}

static int zend_jit_defined(zend_jit_ctx *jit, const zend_op *opline, zend_uchar smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr)
{
	uint32_t defined_label = (uint32_t)-1;
	uint32_t undefined_label = (uint32_t)-1;
	zval *zv = RT_CONSTANT(opline, opline->op1);
	zend_jit_addr res_addr;
	ir_ref ref, ref2, if_set, if_zero, if_set2;
	ir_ref end_inputs[5], true_inputs[5], false_inputs[5];
	ir_ref end_inputs_count = 0, true_inputs_count = 0, false_inputs_count = 0;

	if (smart_branch_opcode && !exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ) {
			defined_label = target_label2;
			undefined_label = target_label;
		} else if (smart_branch_opcode == ZEND_JMPNZ) {
			defined_label = target_label;
			undefined_label = target_label2;
		} else {
			ZEND_UNREACHABLE();
		}
	} else {
		res_addr = RES_ADDR();
	}

	// if (CACHED_PTR(opline->extended_value)) {
	ref = zend_jit_load_at_offset(jit, IR_ADDR,
		zend_jit_load_at_offset(jit, IR_ADDR,
			zend_jit_fp(jit), offsetof(zend_execute_data, run_time_cache)),
		opline->extended_value);

	if_set = zend_jit_if(jit, ref);

	zend_jit_if_false_cold(jit, if_set);
	if_zero = zend_jit_end(jit);

	zend_jit_if_true(jit, if_set);
	if_set2 = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_ADDR), ref,
			zend_jit_const_addr(jit, CACHE_SPECIAL)));
	zend_jit_if_false(jit, if_set2);

	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPNZ) {
			zend_jit_side_exit(jit,
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		} else {
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	} else if (smart_branch_opcode) {
		zend_jit_array_add(true_inputs, zend_jit_end(jit));
	} else {
		zend_jit_zval_set_type_info(jit, res_addr, IS_TRUE);
		zend_jit_array_add(end_inputs, zend_jit_end(jit));
	}

	zend_jit_if_true_cold(jit, if_set2);

	ref2 = ZEND_JIT_EG_ADDR(zend_constants);
	ref = ir_fold2(&jit->ctx, IR_OPT(IR_SHR, IR_ADDR),
		ref,
		zend_jit_const_addr(jit, 1));
	if (sizeof(void*) == 8) {
		ref = ir_fold1(&jit->ctx, IR_OPT(IR_TRUNC, IR_U32), ref);
	}
	ref2 = ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
		zend_jit_load_at_offset(jit, IR_U32,
			zend_jit_load(jit, IR_ADDR, ref2), offsetof(HashTable, nNumOfElements)),
		ref);
	ref2 = zend_jit_if(jit, ref2);
	zend_jit_if_true(jit, ref2);

	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ) {
			zend_jit_side_exit(jit,
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		} else {
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	} else if (smart_branch_opcode) {
		zend_jit_array_add(false_inputs, zend_jit_end(jit));
	} else {
		zend_jit_zval_set_type_info(jit, res_addr, IS_FALSE);
		zend_jit_array_add(end_inputs, zend_jit_end(jit));
	}

	zend_jit_if_false(jit, ref2);
	zend_jit_merge_2(jit, if_zero, zend_jit_end(jit));

	zend_jit_set_ex_opline(jit, opline);
	ref2 = ir_fold2(&jit->ctx, IR_OPT(IR_NE, IR_BOOL),
		zend_jit_call_1(jit, IR_ADDR,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_check_constant, IR_CONST_FASTCALL_FUNC),
			zend_jit_const_addr(jit, (uintptr_t)zv)),
		IR_NULL);
	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ) {
			zend_jit_guard(jit, ref2,
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		} else {
			zend_jit_guard_not(jit, ref2,
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		}
		zend_jit_array_add(end_inputs, zend_jit_end(jit));
	} else if (smart_branch_opcode) {
		ref2 = zend_jit_if(jit, ref2);
		zend_jit_if_true(jit, ref2);
		zend_jit_array_add(true_inputs, zend_jit_end(jit));
		zend_jit_if_false(jit, ref2);
		zend_jit_array_add(false_inputs, zend_jit_end(jit));
	} else {
		zend_jit_zval_ref_set_type_info(jit,
			zend_jit_mem_zval_addr(jit, res_addr),
			ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
				ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref2),
				ir_const_u32(&jit->ctx, IS_FALSE)));
		zend_jit_array_add(end_inputs, zend_jit_end(jit));
	}

	if (!smart_branch_opcode || exit_addr) {
		if (end_inputs_count) {
			zend_jit_merge_N(jit, end_inputs_count, end_inputs);
		}
	} else {
		_zend_jit_merge_smart_branch_inputs(jit, defined_label, undefined_label,
			true_inputs, true_inputs_count, false_inputs, false_inputs_count);
	}

	return 1;
}

static int zend_jit_escape_if_undef(zend_jit_ctx *jit, int var, uint32_t flags, const zend_op *opline, int8_t reg)
{
#if 0 //???
	zend_jit_addr val_addr = ZEND_ADDR_MEM_ZVAL(ZREG_R0, 0);

	|	IF_NOT_ZVAL_TYPE val_addr, IS_UNDEF, >1

	if (flags & ZEND_JIT_EXIT_RESTORE_CALL) {
		if (!zend_jit_save_call_chain(Dst, -1)) {
			return 0;
		}
	}

	ZEND_ASSERT(opline);

	if ((opline-1)->opcode != ZEND_FETCH_CONSTANT
	 && (opline-1)->opcode != ZEND_FETCH_LIST_R
	 && ((opline-1)->op1_type & (IS_VAR|IS_TMP_VAR))
	 && !(flags & ZEND_JIT_EXIT_FREE_OP1)) {
		val_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, (opline-1)->op1.var);

		|	IF_NOT_ZVAL_REFCOUNTED val_addr, >2
		|	GET_ZVAL_PTR r0, val_addr
		|	GC_ADDREF r0
		|2:
	}

	|	LOAD_IP_ADDR (opline - 1)
	|	jmp ->trace_escape
	|1:
#endif
	return 1;
}

static int zend_jit_restore_zval(zend_jit_ctx *jit, int var, int8_t reg)
{
	zend_jit_addr var_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, var);
	zend_jit_addr reg_addr = ZEND_ADDR_REF_ZVAL(zend_jit_deopt_rload(jit, IR_ADDR, reg));

	// JIT: ZVAL_COPY_OR_DUP(EX_VAR(opline->result.var), &c->value); (no dup)
	zend_jit_copy_zval(jit, var_addr, MAY_BE_ANY, reg_addr, MAY_BE_ANY, 1);
	return 1;
}

static zend_jit_addr zend_jit_guard_fetch_result_type(zend_jit_ctx         *jit,
                                                      const zend_op        *opline,
                                                      zend_jit_addr         val_addr,
                                                      uint8_t               type,
                                                      bool                  deref,
                                                      uint32_t              flags,
                                                      bool                  op1_avoid_refcounting)
{
	zend_jit_trace_stack *stack = JIT_G(current_frame)->stack;
	int32_t exit_point;
	const void *res_exit_addr = NULL;
	ir_ref end1 = IR_UNUSED, ref1 = IR_UNUSED;
	ir_ref ref = zend_jit_zval_addr(jit, val_addr);
	uint32_t old_op1_info = 0;
	uint32_t old_info;
	ir_ref old_ref;

	if (op1_avoid_refcounting) {
		old_op1_info = STACK_INFO(stack, EX_VAR_TO_NUM(opline->op1.var));
		SET_STACK_REG(stack, EX_VAR_TO_NUM(opline->op1.var), ZREG_NONE);
	}
	old_info = STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var));
	old_ref = STACK_REF(stack, EX_VAR_TO_NUM(opline->result.var));
	CLEAR_STACK_REF(stack, EX_VAR_TO_NUM(opline->result.var));
	SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_UNKNOWN, 1);

	if (deref) {
		ir_ref if_type = zend_jit_if_zval_type(jit, val_addr, type);

		zend_jit_if_true(jit, if_type);
		end1 = zend_jit_end(jit);
		ref1 = ref;
		zend_jit_if_false_cold(jit, if_type);

		SET_STACK_REF_EX(stack, EX_VAR_TO_NUM(opline->result.var), ref, ZREG_ZVAL_COPY);
		exit_point = zend_jit_trace_get_exit_point(opline+1, flags);
		res_exit_addr = zend_jit_trace_get_exit_addr(exit_point);
		if (!res_exit_addr) {
			return 0;
		}

		zend_jit_guard_zval_type(jit, val_addr, IS_REFERENCE, res_exit_addr);
		ref = zend_jit_addr_offset(jit, zend_jit_zval_ptr(jit, val_addr), offsetof(zend_reference, val));
		val_addr = ZEND_ADDR_REF_ZVAL(ref);
	}

	SET_STACK_REF_EX(stack, EX_VAR_TO_NUM(opline->result.var), ref, ZREG_ZVAL_COPY);
	exit_point = zend_jit_trace_get_exit_point(opline+1, flags);
	res_exit_addr = zend_jit_trace_get_exit_addr(exit_point);
	if (!res_exit_addr) {
		return 0;
	}

	zend_jit_guard_zval_type(jit, val_addr, type, res_exit_addr);

	if (deref) {
		zend_jit_merge_2(jit, end1, zend_jit_end(jit));
		ref = zend_jit_phi_2(jit, ref1, ref);
	}

	val_addr = ZEND_ADDR_REF_ZVAL(ref);

	SET_STACK_REF(stack, EX_VAR_TO_NUM(opline->result.var), old_ref);
	SET_STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var), old_info);
	if (op1_avoid_refcounting) {
		SET_STACK_INFO(stack, EX_VAR_TO_NUM(opline->op1.var), old_op1_info);
	}

	return val_addr;
}

static int zend_jit_fetch_constant(zend_jit_ctx         *jit,
                                   const zend_op        *opline,
                                   const zend_op_array  *op_array,
                                   zend_ssa             *ssa,
                                   const zend_ssa_op    *ssa_op,
                                   zend_jit_addr         res_addr)
{
	zval *zv = RT_CONSTANT(opline, opline->op2) + 1;
	uint32_t res_info = RES_INFO();
	ir_ref ref, ref2, if_set, if_special, not_set_path, special_path, fast_path;

	// JIT: c = CACHED_PTR(opline->extended_value);
	ref = zend_jit_load_at_offset(jit, IR_ADDR,
		zend_jit_load_at_offset(jit, IR_ADDR,
			zend_jit_fp(jit), offsetof(zend_execute_data, run_time_cache)),
		opline->extended_value);


	// JIT: if (c != NULL)
	if_set = zend_jit_if(jit, ref);

	if (!zend_jit_is_persistent_constant(zv, opline->op1.num)) {
		// JIT: if (!IS_SPECIAL_CACHE_VAL(c))
		zend_jit_if_false_cold(jit, if_set);
		not_set_path = zend_jit_end(jit);
		zend_jit_if_true(jit, if_set);
		if_special = zend_jit_if(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_ADDR), ref,
				zend_jit_const_addr(jit, CACHE_SPECIAL)));
		zend_jit_if_true_cold(jit, if_special);
		special_path = zend_jit_end(jit);
		zend_jit_if_false(jit, if_special);
		fast_path = zend_jit_end(jit);
		zend_jit_merge_2(jit, not_set_path, special_path);
	} else {
		zend_jit_if_true(jit, if_set);
		fast_path = zend_jit_end(jit);
		zend_jit_if_false_cold(jit, if_set);
	}

	// JIT: zend_jit_get_constant(RT_CONSTANT(opline, opline->op2) + 1, opline->op1.num);
	zend_jit_set_ex_opline(jit, opline);
	ref2 = zend_jit_call_2(jit, IR_ADDR,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_get_constant, IR_CONST_FASTCALL_FUNC),
		zend_jit_const_addr(jit,(uintptr_t)zv),
		ir_const_u32(&jit->ctx, opline->op1.num));
	zend_jit_guard(jit, ref2,
		zend_jit_stub_addr(jit, jit_stub_exception_handler));

	zend_jit_merge_2(jit, fast_path, zend_jit_end(jit));
	ref = zend_jit_phi_2(jit, ref, ref2);

	if ((res_info & MAY_BE_GUARD) && JIT_G(current_frame)) {
		zend_uchar type = concrete_type(res_info);
		zend_jit_addr const_addr = ZEND_ADDR_REF_ZVAL(ref);

		const_addr = zend_jit_guard_fetch_result_type(jit, opline, const_addr, type, 0, 0, 0);
		if (!const_addr) {
			return 0;
		}

		res_info &= ~MAY_BE_GUARD;
		ssa->var_info[ssa_op->result_def].type &= ~MAY_BE_GUARD;

		// JIT: ZVAL_COPY_OR_DUP(EX_VAR(opline->result.var), &c->value); (no dup)
		zend_jit_copy_zval(jit, res_addr, MAY_BE_ANY, const_addr, res_info, 1);
		if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, res_info)) {
			return 0;
		}
	} else {
		ir_ref const_addr = ZEND_ADDR_REF_ZVAL(ref);

		// JIT: ZVAL_COPY_OR_DUP(EX_VAR(opline->result.var), &c->value); (no dup)
		zend_jit_copy_zval(jit, res_addr, MAY_BE_ANY, const_addr, MAY_BE_ANY, 1);
	}


	return 1;
}

static int zend_jit_type_check(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_uchar smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr)
{
	uint32_t  mask;
	zend_jit_addr op1_addr = OP1_ADDR();
	zend_jit_addr res_addr = 0;
	uint32_t true_label = -1, false_label = -1;
	ir_ref end_inputs[5], true_inputs[5], false_inputs[5];
	ir_ref end_inputs_count = 0, true_inputs_count = 0, false_inputs_count = 0;

	// TODO: support for is_resource() ???
	ZEND_ASSERT(opline->extended_value != MAY_BE_RESOURCE);

	if (smart_branch_opcode && !exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ) {
			true_label = target_label2;
			false_label = target_label;
		} else if (smart_branch_opcode == ZEND_JMPNZ) {
			true_label = target_label;
			false_label = target_label2;
		} else {
			ZEND_UNREACHABLE();
		}
	} else {
		res_addr = RES_ADDR();
	}

	if (op1_info & MAY_BE_UNDEF) {
		ir_ref if_def = IR_UNUSED;

		if (op1_info & (MAY_BE_ANY|MAY_BE_REF)) {
			if_def = zend_jit_if_not_zval_type(jit, op1_addr, IS_UNDEF);
			zend_jit_if_false_cold(jit, if_def);
		}

		zend_jit_set_ex_opline(jit, opline);
		zend_jit_call_1(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_undefined_op_helper, IR_CONST_FASTCALL_FUNC),
			ir_const_u32(&jit->ctx, opline->op1.var));
		zend_jit_check_exception_undef_result(jit, opline);
		if (opline->extended_value & MAY_BE_NULL) {
			if (exit_addr) {
				if (smart_branch_opcode == ZEND_JMPNZ) {
					zend_jit_side_exit(jit,
						zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				} else {
					zend_jit_array_add(end_inputs, zend_jit_end(jit));
				}
			} else if (smart_branch_opcode) {
				zend_jit_array_add(true_inputs, zend_jit_end(jit));
			} else {
				zend_jit_zval_set_type_info(jit, res_addr, IS_TRUE);
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}
		} else {
			if (exit_addr) {
				if (smart_branch_opcode == ZEND_JMPZ) {
					zend_jit_side_exit(jit,
						zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				} else {
					zend_jit_array_add(end_inputs, zend_jit_end(jit));
				}
			} else if (smart_branch_opcode) {
				zend_jit_array_add(false_inputs, zend_jit_end(jit));
			} else {
				zend_jit_zval_set_type_info(jit, res_addr, IS_FALSE);
				if (if_def) {
					zend_jit_array_add(end_inputs, zend_jit_end(jit));
				}
			}
		}

		if (if_def) {
			zend_jit_if_true(jit, if_def);
			op1_info |= MAY_BE_NULL;
		}
	}

	if (op1_info & (MAY_BE_ANY|MAY_BE_REF)) {
		mask = opline->extended_value;
		if (!(op1_info & MAY_BE_GUARD) && !(op1_info & (MAY_BE_ANY - mask))) {			
			zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);
			if (exit_addr) {
				if (smart_branch_opcode == ZEND_JMPNZ) {
					zend_jit_side_exit(jit,
						zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				} else if (end_inputs_count) {
					zend_jit_array_add(end_inputs, zend_jit_end(jit));
				}
			} else if (smart_branch_opcode) {
				zend_jit_array_add(true_inputs, zend_jit_end(jit));
			} else {
				zend_jit_zval_set_type_info(jit, res_addr, IS_TRUE);
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}
	    } else if (!(op1_info & MAY_BE_GUARD) && !(op1_info & mask)) {
			zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);
			if (exit_addr) {
				if (smart_branch_opcode == ZEND_JMPZ) {
					zend_jit_side_exit(jit,
						zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				} else if (end_inputs_count) {
					zend_jit_array_add(end_inputs, zend_jit_end(jit));
				}
			} else if (smart_branch_opcode) {
				zend_jit_array_add(false_inputs, zend_jit_end(jit));
			} else {
				zend_jit_zval_set_type_info(jit, res_addr, IS_FALSE);
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}
		} else {
			ir_ref ref;
			bool invert = 0;
			zend_uchar type;

			switch (mask) {
				case MAY_BE_NULL:   type = IS_NULL;   break;
				case MAY_BE_FALSE:  type = IS_FALSE;  break;
				case MAY_BE_TRUE:   type = IS_TRUE;   break;
				case MAY_BE_LONG:   type = IS_LONG;   break;
				case MAY_BE_DOUBLE: type = IS_DOUBLE; break;
				case MAY_BE_STRING: type = IS_STRING; break;
				case MAY_BE_ARRAY:  type = IS_ARRAY;  break;
				case MAY_BE_OBJECT: type = IS_OBJECT; break;
				case MAY_BE_ANY - MAY_BE_NULL:     type = IS_NULL;   invert = 1; break;
				case MAY_BE_ANY - MAY_BE_FALSE:    type = IS_FALSE;  invert = 1; break;
				case MAY_BE_ANY - MAY_BE_TRUE:     type = IS_TRUE;   invert = 1; break;
				case MAY_BE_ANY - MAY_BE_LONG:     type = IS_LONG;   invert = 1; break;
				case MAY_BE_ANY - MAY_BE_DOUBLE:   type = IS_DOUBLE; invert = 1; break;
				case MAY_BE_ANY - MAY_BE_STRING:   type = IS_STRING; invert = 1; break;
				case MAY_BE_ANY - MAY_BE_ARRAY:    type = IS_ARRAY;  invert = 1; break;
				case MAY_BE_ANY - MAY_BE_OBJECT:   type = IS_OBJECT; invert = 1; break;
				case MAY_BE_ANY - MAY_BE_RESOURCE: type = IS_OBJECT; invert = 1; break;
				default:
					type = 0;
			}

			if (op1_info & MAY_BE_REF) {
				ir_ref ref = zend_jit_zval_addr(jit, op1_addr);
				ref = zend_jit_zval_ref_deref(jit, ref);
				op1_addr = ZEND_ADDR_REF_ZVAL(ref);
			}
			if (type == 0) {
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
						ir_fold2(&jit->ctx, IR_OPT(IR_SHL, IR_U32),
							ir_const_u32(&jit->ctx, 1),
							zend_jit_zval_type(jit, op1_addr)),
						ir_const_u32(&jit->ctx, mask));
				if (!smart_branch_opcode) {
					ref = ir_fold2(&jit->ctx, IR_OPT(IR_NE, IR_BOOL), ref,
						ir_const_u32(&jit->ctx, 0));
				}
			} else if (invert) {
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_NE, IR_BOOL),
					zend_jit_zval_type(jit, op1_addr),
					ir_const_u8(&jit->ctx, type));
			} else {
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
					zend_jit_zval_type(jit, op1_addr),
					ir_const_u8(&jit->ctx, type));
			}

			zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);

			if (exit_addr) {
				if (smart_branch_opcode == ZEND_JMPZ) {
					zend_jit_guard(jit, ref,
						zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				} else {
					zend_jit_guard_not(jit, ref,
						zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				}
				if (end_inputs_count) {
					zend_jit_array_add(end_inputs, zend_jit_end(jit));
				}
			} else if (smart_branch_opcode) {
				ir_ref if_val = zend_jit_if(jit, ref);
				zend_jit_if_true(jit, if_val);
				zend_jit_array_add(true_inputs, zend_jit_end(jit));
				zend_jit_if_false(jit, if_val);
				zend_jit_array_add(false_inputs, zend_jit_end(jit));
			} else {
				zend_jit_zval_ref_set_type_info(jit,
					zend_jit_mem_zval_addr(jit, res_addr),
					ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
						ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref),
						ir_const_u32(&jit->ctx, IS_FALSE)));
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}
	    }
	}

	if (!smart_branch_opcode || exit_addr) {
		if (end_inputs_count) {
			zend_jit_merge_N(jit, end_inputs_count, end_inputs);
		}
	} else {
		_zend_jit_merge_smart_branch_inputs(jit, true_label, false_label,
			true_inputs, true_inputs_count, false_inputs, false_inputs_count);
	}

	return 1;
}

static int zend_jit_isset_isempty_cv(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr,  zend_uchar smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr)
{
	zend_jit_addr res_addr = RES_ADDR();
	uint32_t true_label = -1, false_label = -1;
	ir_ref end_inputs[5], true_inputs[5], false_inputs[5];
	ir_ref end_inputs_count = 0, true_inputs_count = 0, false_inputs_count = 0;

	// TODO: support for empty() ???
	ZEND_ASSERT(opline->extended_value != MAY_BE_RESOURCE);

	if (smart_branch_opcode && !exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ) {
			true_label = target_label2;
			false_label = target_label;
		} else if (smart_branch_opcode == ZEND_JMPNZ) {
			true_label = target_label;
			false_label = target_label2;
		} else {
			ZEND_UNREACHABLE();
		}
	} else {
		res_addr = RES_ADDR();
	}

	if (op1_info & MAY_BE_REF) {
		ir_ref ref = zend_jit_zval_addr(jit, op1_addr);
		ref = zend_jit_zval_ref_deref(jit, ref);
		op1_addr = ZEND_ADDR_REF_ZVAL(ref);
	}

	if (!(op1_info & (MAY_BE_UNDEF|MAY_BE_NULL))) {
		if (exit_addr) {
			ZEND_ASSERT(smart_branch_opcode == ZEND_JMPZ);
		} else if (smart_branch_opcode) {
			zend_jit_array_add(true_inputs, zend_jit_end(jit));
		} else {
			zend_jit_zval_set_type_info(jit, res_addr, IS_TRUE);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	} else if (!(op1_info & (MAY_BE_ANY - MAY_BE_NULL))) {
		if (exit_addr) {
			ZEND_ASSERT(smart_branch_opcode == ZEND_JMPNZ);
		} else if (smart_branch_opcode) {
			zend_jit_array_add(false_inputs, zend_jit_end(jit));
		} else {
			zend_jit_zval_set_type_info(jit, res_addr, IS_FALSE);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	} else {
		ir_ref ref = ir_fold2(&jit->ctx, IR_OPT(IR_GT, IR_U8),
			zend_jit_zval_type(jit, op1_addr),
			ir_const_u8(&jit->ctx, IS_NULL));
		if (exit_addr) {
			if (smart_branch_opcode == ZEND_JMPNZ) {
				zend_jit_guard_not(jit, ref,
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				zend_jit_guard(jit, ref,
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			}
			if (end_inputs_count) {
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}
		} else if (smart_branch_opcode) {
			ir_ref if_val = zend_jit_if(jit, ref);
			zend_jit_if_true(jit, if_val);
			zend_jit_array_add(true_inputs, zend_jit_end(jit));
			zend_jit_if_false(jit, if_val);
			zend_jit_array_add(false_inputs, zend_jit_end(jit));
		} else {
			zend_jit_zval_ref_set_type_info(jit,
				zend_jit_mem_zval_addr(jit, res_addr),
				ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
					ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32), ref),
					ir_const_u32(&jit->ctx, IS_FALSE)));
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	}

	if (!smart_branch_opcode || exit_addr) {
		if (end_inputs_count) {
			zend_jit_merge_N(jit, end_inputs_count, end_inputs);
		}
	} else {
		_zend_jit_merge_smart_branch_inputs(jit, true_label, false_label,
			true_inputs, true_inputs_count, false_inputs, false_inputs_count);
	}

	return 1;
}

/* copy of hidden zend_closure */
typedef struct _zend_closure {
	zend_object       std;
	zend_function     func;
	zval              this_ptr;
	zend_class_entry *called_scope;
	zif_handler       orig_internal_handler;
} zend_closure;

static int zend_jit_stack_check(zend_jit_ctx *jit, const zend_op *opline, uint32_t used_stack)
{
	int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
	const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

	if (!exit_addr) {
		return 0;
	}

	// JIT: if (EG(vm_stack_end) - EG(vm_stack_top) < used_stack)
	zend_jit_guard(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_UGE, IR_BOOL),
			ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_ADDR),
				zend_jit_load(jit, IR_ADDR, ZEND_JIT_EG_ADDR(vm_stack_end)),
				zend_jit_load(jit, IR_ADDR, ZEND_JIT_EG_ADDR(vm_stack_top))),
			zend_jit_const_addr(jit, used_stack)),
		zend_jit_const_addr(jit, (uintptr_t)exit_addr));

	return 1;
}

static int zend_jit_free_trampoline(zend_jit_ctx *jit, int8_t func_reg)
{
//???	ir_ref func_ref = zend_jit_deopt_rload(jit, IR_ADDR, func_reg);

	// JIT: if (UNEXPECTED(func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE))
ZEND_ASSERT(0);
//???	|	test dword [r0 + offsetof(zend_function, common.fn_flags)], ZEND_ACC_CALL_VIA_TRAMPOLINE
//???	|	jz >1
//???	|	mov FCARG1a, r0
//???	|	EXT_CALL zend_jit_free_trampoline_helper, r0
//???	|1:
	return 1;
}

static int zend_jit_push_call_frame(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, zend_function *func, bool is_closure, bool delayed_fetch_this, int checked_stack, ir_ref func_ref, ir_ref this_ref)
{
	uint32_t used_stack;
	ir_ref used_stack_ref = IR_UNUSED;
	bool stack_check = 1;
	ir_ref rx, ref, if_enough_stack, cold_path = IR_UNUSED;

	ZEND_ASSERT(func_ref != IR_NULL);
	if (func) {
		used_stack = zend_vm_calc_used_stack(opline->extended_value, func);
		if ((int)used_stack <= checked_stack) {
			stack_check = 0;
		}
		used_stack_ref = zend_jit_const_addr(jit, used_stack);
	} else {
		ir_ref num_args_ref;
		ir_ref if_internal_func = IR_UNUSED;

		used_stack = (ZEND_CALL_FRAME_SLOT + opline->extended_value + ZEND_OBSERVER_ENABLED) * sizeof(zval);
		used_stack_ref = zend_jit_const_addr(jit, used_stack);

		if (!is_closure) {
// TODO: used_stack_ref = ir_emit2(&jit->ctx, IR_OPT(IR_COPY, IR_ADDR), used_stack_ref, 0); /* load constant once ??? */

			// JIT: if (EXPECTED(ZEND_USER_CODE(func->type))) {
			ir_ref tmp = zend_jit_load_at_offset(jit, IR_U8,
				func_ref, offsetof(zend_function, type));
			if_internal_func = zend_jit_if(jit,
				ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U8),
					tmp,
					ir_const_u8(&jit->ctx, 1)));
			zend_jit_if_false(jit, if_internal_func);
		}

		// JIT: used_stack += (func->op_array.last_var + func->op_array.T - MIN(func->op_array.num_args, num_args)) * sizeof(zval);
		num_args_ref = ir_const_u32(&jit->ctx, opline->extended_value);
		if (!is_closure) {
			ref = ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_U32),
				ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_U32),
					ir_fold2(&jit->ctx, IR_OPT(IR_MIN, IR_U32),
						num_args_ref,
						zend_jit_load_at_offset(jit, IR_U32,
							func_ref, offsetof(zend_function, op_array.num_args))),
					zend_jit_load_at_offset(jit, IR_U32,
						func_ref, offsetof(zend_function, op_array.last_var))),
				zend_jit_load_at_offset(jit, IR_U32,
					func_ref, offsetof(zend_function, op_array.T)));
		} else {
			ref = ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_U32),
				ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_U32),
					ir_fold2(&jit->ctx, IR_OPT(IR_MIN, IR_U32),
						num_args_ref,
						zend_jit_load_at_offset(jit, IR_U32,
							func_ref, offsetof(zend_closure, func.op_array.num_args))),
					zend_jit_load_at_offset(jit, IR_U32,
						func_ref, offsetof(zend_closure, func.op_array.last_var))),
				zend_jit_load_at_offset(jit, IR_U32,
					func_ref, offsetof(zend_closure, func.op_array.T)));
		}
		ref = ir_fold2(&jit->ctx, IR_OPT(IR_MUL, IR_U32), ref, ir_const_u32(&jit->ctx, sizeof(zval)));
		if (sizeof(void*) == 8) {
			ref = ir_fold1(&jit->ctx, IR_OPT(IR_SEXT, IR_ADDR), ref);
		}
		ref = ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_ADDR), used_stack_ref, ref);

		if (is_closure) {
			used_stack_ref = ref;
		} else {
			ir_ref long_path = zend_jit_end(jit);
			zend_jit_if_true(jit, if_internal_func);
			zend_jit_merge_2(jit, long_path, zend_jit_end(jit));
			used_stack_ref = zend_jit_phi_2(jit, ref, used_stack_ref);
		}
	}

	zend_jit_start_reuse_ip(jit);

	// JIT: if (UNEXPECTED(used_stack > (size_t)(((char*)EG(vm_stack_end)) - (char*)call))) {
	zend_jit_store_ip(jit,
		zend_jit_load(jit, IR_ADDR, ZEND_JIT_EG_ADDR(vm_stack_top)));

	if (stack_check) {
		// JIT: Check Stack Overflow
		ref = ir_fold2(&jit->ctx, IR_OPT(IR_UGE, IR_ADDR),
			ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_ADDR),
				zend_jit_load(jit, IR_ADDR, ZEND_JIT_EG_ADDR(vm_stack_end)),
				zend_jit_ip(jit)),
			used_stack_ref);

		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
			int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
			const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

			if (!exit_addr) {
				return 0;
			}

			zend_jit_guard(jit, ref, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		} else {
			if_enough_stack = zend_jit_if(jit, ref);
			zend_jit_if_false_cold(jit, if_enough_stack);

#ifdef _WIN32
			if (0) {
#else
			if (opline->opcode == ZEND_INIT_FCALL && func && func->type == ZEND_INTERNAL_FUNCTION) {
#endif
				zend_jit_set_ex_opline(jit, opline);
				ref = zend_jit_call_1(jit, IR_ADDR,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_int_extend_stack_helper, IR_CONST_FASTCALL_FUNC),
					used_stack_ref);
			} else {
				if (!is_closure) {
					ref = func_ref;
				} else {
					ref = zend_jit_addr_offset(jit, func_ref, offsetof(zend_closure, func));
				}
				zend_jit_set_ex_opline(jit, opline);
				ref = zend_jit_call_2(jit, IR_ADDR,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_extend_stack_helper, IR_CONST_FASTCALL_FUNC),
					used_stack_ref, ref);
			}
			zend_jit_store_ip(jit, ref);

			cold_path = zend_jit_end(jit);
			zend_jit_if_true(jit, if_enough_stack);
		}
	}

	// JIT: EG(vm_stack_top) += used_stack;
	// JIT: EG(vm_stack_top) = (zval*)((char*)call + used_stack); // TODO: try this ???
	ref = ZEND_JIT_EG_ADDR(vm_stack_top);
	zend_jit_store(jit, ref,
		ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
			/* zend_jit_load() makes load forwarding and doesn't allow fusion on x86 ??? */
			(JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) ? zend_jit_ip(jit) : zend_jit_load(jit, IR_ADDR, ref),
			used_stack_ref));

	// JIT: zend_vm_init_call_frame(call, call_info, func, num_args, called_scope, object);
	rx = zend_jit_ip(jit);
	if (JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE || opline->opcode != ZEND_INIT_METHOD_CALL) {
		// JIT: ZEND_SET_CALL_INFO(call, 0, call_info);
		zend_jit_store_at_offset(jit,
			rx, offsetof(zend_execute_data, This.u1.type_info),
			ir_const_u32(&jit->ctx, IS_UNDEF | ZEND_CALL_NESTED_FUNCTION));
	}
#ifdef _WIN32
	if (0) {
#else
	if (opline->opcode == ZEND_INIT_FCALL && func && func->type == ZEND_INTERNAL_FUNCTION) {
#endif
		if (cold_path) {
			zend_jit_merge_2(jit, cold_path, zend_jit_end(jit));
			rx = zend_jit_ip(jit);
		}

		// JIT: call->func = func;
		zend_jit_store_at_offset(jit,
			rx, offsetof(zend_execute_data, func),
			func_ref);
	} else {
		if (!is_closure) {
			// JIT: call->func = func;
			zend_jit_store_at_offset(jit,
				rx, offsetof(zend_execute_data, func),
				func_ref);
		} else {
			// JIT: call->func = &closure->func;
			zend_jit_store_at_offset(jit,
				rx, offsetof(zend_execute_data, func),
				zend_jit_addr_offset(jit, func_ref, offsetof(zend_closure, func)));
		}
		if (cold_path) {
			zend_jit_merge_2(jit, cold_path, zend_jit_end(jit));
			rx = zend_jit_ip(jit);
		}
	}
	if (opline->opcode == ZEND_INIT_METHOD_CALL) {
		// JIT: Z_PTR(call->This) = obj;
		ZEND_ASSERT(this_ref != IR_NULL);
		zend_jit_store_at_offset(jit,
			rx, offsetof(zend_execute_data, This.value.ptr),
			this_ref);
	    if (opline->op1_type == IS_UNUSED || delayed_fetch_this) {
			// JIT: call->call_info |= ZEND_CALL_HAS_THIS;
			ref = zend_jit_addr_offset(jit, rx, offsetof(zend_execute_data, This.u1.type_info));
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				zend_jit_store(jit,
					ref,
					ir_const_u32(&jit->ctx,  ZEND_CALL_HAS_THIS));
			} else {
				zend_jit_store(jit,
					ref,
					ir_fold2(&jit->ctx, IR_OPT(IR_OR, IR_U32),
						zend_jit_load(jit, IR_U32, ref),
						ir_const_u32(&jit->ctx,  ZEND_CALL_HAS_THIS)));
			}
	    } else {
			if (opline->op1_type == IS_CV) {
				// JIT: GC_ADDREF(obj);
				zend_jit_gc_addref(jit, this_ref);
			}

			// JIT: call->call_info |= ZEND_CALL_HAS_THIS | ZEND_CALL_RELEASE_THIS;
			ref = zend_jit_addr_offset(jit, rx, offsetof(zend_execute_data, This.u1.type_info));
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				zend_jit_store(jit,
					ref,
					ir_const_u32(&jit->ctx,  ZEND_CALL_HAS_THIS | ZEND_CALL_RELEASE_THIS));
			} else {
				zend_jit_store(jit,
					ref,
					ir_fold2(&jit->ctx, IR_OPT(IR_OR, IR_U32),
						zend_jit_load(jit, IR_U32, ref),
						ir_const_u32(&jit->ctx,  ZEND_CALL_HAS_THIS | ZEND_CALL_RELEASE_THIS)));
			}
	    }
	} else if (!is_closure) {
		// JIT: Z_CE(call->This) = called_scope;
		zend_jit_store_at_offset(jit,
			rx, offsetof(zend_execute_data, This),
			IR_NULL);
	} else {
		ir_ref object_or_called_scope, call_info, call_info2, object, cond, if_cond, long_path;

		if (opline->op2_type == IS_CV) {
			// JIT: GC_ADDREF(closure);
			zend_jit_gc_addref(jit, func_ref);
		}

		// JIT: RX(object_or_called_scope) = closure->called_scope;
		object_or_called_scope = zend_jit_load_at_offset(jit, IR_ADDR,
			func_ref, offsetof(zend_closure, called_scope));

		// JIT: call_info = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_DYNAMIC | ZEND_CALL_CLOSURE |
		//      (closure->func->common.fn_flags & ZEND_ACC_FAKE_CLOSURE);
		call_info = ir_fold2(&jit->ctx, IR_OPT(IR_OR, IR_U32),
			ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
				zend_jit_load_at_offset(jit, IR_U32,
					func_ref, offsetof(zend_closure, func.common.fn_flags)),
				ir_const_u32(&jit->ctx, ZEND_ACC_FAKE_CLOSURE)),
			ir_const_u32(&jit->ctx, ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_DYNAMIC | ZEND_CALL_CLOSURE));
		// JIT: if (Z_TYPE(closure->this_ptr) != IS_UNDEF) {
		cond = zend_jit_load_at_offset(jit, IR_U8,
			func_ref, offsetof(zend_closure, this_ptr.u1.v.type));
		if_cond = zend_jit_if(jit, cond);
		zend_jit_if_true(jit, if_cond);

		// JIT: call_info |= ZEND_CALL_HAS_THIS;
		call_info2 = ir_fold2(&jit->ctx, IR_OPT(IR_OR, IR_U32), call_info, ir_const_u32(&jit->ctx, ZEND_CALL_HAS_THIS));

		// JIT: object_or_called_scope = Z_OBJ(closure->this_ptr);
		object = zend_jit_load_at_offset(jit, IR_ADDR,
			func_ref, offsetof(zend_closure, this_ptr.value.ptr));

		long_path = zend_jit_end(jit);
		zend_jit_if_false(jit, if_cond);
		zend_jit_merge_2(jit, zend_jit_end(jit), long_path);
		call_info = zend_jit_phi_2(jit, call_info, call_info2);
		object_or_called_scope = zend_jit_phi_2(jit, object_or_called_scope, object);

		// JIT: ZEND_SET_CALL_INFO(call, 0, call_info);
		ref = zend_jit_addr_offset(jit, rx, offsetof(zend_execute_data, This.u1.type_info));
		zend_jit_store(jit,
			ref,
			ir_fold2(&jit->ctx, IR_OPT(IR_OR, IR_U32),
				zend_jit_load(jit, IR_U32, ref),
				call_info));

		// JIT: Z_PTR(call->This) = object_or_called_scope;
		zend_jit_store_at_offset(jit,
			rx, offsetof(zend_execute_data, This.value.ptr),
			object_or_called_scope);

		// JIT: if (closure->func.op_array.run_time_cache__ptr)
		cond = zend_jit_load_at_offset(jit, IR_ADDR,
			func_ref, offsetof(zend_closure, func.op_array.run_time_cache__ptr));
		if_cond = zend_jit_if(jit, cond);
		zend_jit_if_false(jit, if_cond);

		// JIT: zend_jit_init_func_run_time_cache_helper(closure->func);
		zend_jit_call_1(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_init_func_run_time_cache_helper, IR_CONST_FASTCALL_FUNC),
			zend_jit_addr_offset(jit, func_ref, offsetof(zend_closure, func)));

		long_path = zend_jit_end(jit);
		zend_jit_if_true(jit, if_cond);
		zend_jit_merge_2(jit, zend_jit_end(jit), long_path);
	}

	// JIT: ZEND_CALL_NUM_ARGS(call) = num_args;
	zend_jit_store_at_offset(jit,
		rx, offsetof(zend_execute_data, This.u2.num_args),
		ir_const_u32(&jit->ctx, opline->extended_value));

	return 1;
}

static int zend_jit_init_fcall_guard(zend_jit_ctx *jit, uint32_t level, const zend_function *func, const zend_op *to_opline)
{
	int32_t exit_point;
	const void *exit_addr;
	ir_ref call;

	if (func->type == ZEND_INTERNAL_FUNCTION) {
#ifdef ZEND_WIN32
		// TODO: ASLR may cause different addresses in different workers ???
		return 0;
#endif
	} else if (func->type == ZEND_USER_FUNCTION) {
		if (!zend_accel_in_shm(func->op_array.opcodes)) {
			/* op_array and op_array->opcodes are not persistent. We can't link. */
			return 0;
		}
	} else {
		ZEND_UNREACHABLE();
		return 0;
	}

	exit_point = zend_jit_trace_get_exit_point(to_opline, ZEND_JIT_EXIT_POLYMORPHISM);
	exit_addr = zend_jit_trace_get_exit_addr(exit_point);
	if (!exit_addr) {
		return 0;
	}

	// call = EX(call);
	call = zend_jit_load_at_offset(jit, IR_ADDR,
		zend_jit_fp(jit), offsetof(zend_execute_data, call));
	while (level > 0) {
		// call = call->prev_execute_data
		call = zend_jit_load_at_offset(jit, IR_ADDR,
			call, offsetof(zend_execute_data, prev_execute_data));
		level--;
	}

	if (func->type == ZEND_USER_FUNCTION &&
	    (!(func->common.fn_flags & ZEND_ACC_IMMUTABLE) ||
	     (func->common.fn_flags & ZEND_ACC_CLOSURE) ||
	     !func->common.function_name)) {
		const zend_op *opcodes = func->op_array.opcodes;

		// JIT: if (call->func.op_array.opcodes != opcodes) goto exit_addr;
		zend_jit_guard(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
				zend_jit_load_at_offset(jit, IR_ADDR,
					zend_jit_load_at_offset(jit, IR_ADDR,
						call, offsetof(zend_execute_data, func)),
					offsetof(zend_op_array, opcodes)),
				zend_jit_const_addr(jit, (uintptr_t)opcodes)),
			zend_jit_const_addr(jit, (uintptr_t)exit_addr));
	} else {
		// JIT: if (call->func != func) goto exit_addr;
		zend_jit_guard(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
				zend_jit_load_at_offset(jit, IR_ADDR,
					call, offsetof(zend_execute_data, func)),
				zend_jit_const_addr(jit, (uintptr_t)func)),
			zend_jit_const_addr(jit, (uintptr_t)exit_addr));
	}

	return 1;
}

static int zend_jit_init_fcall(zend_jit_ctx *jit, const zend_op *opline, uint32_t b, const zend_op_array *op_array, zend_ssa *ssa, const zend_ssa_op *ssa_op, int call_level, zend_jit_trace_rec *trace, int checked_stack)
{
	zend_func_info *info = ZEND_FUNC_INFO(op_array);
	zend_call_info *call_info = NULL;
	zend_function *func = NULL;
	ir_ref func_ref = IR_UNUSED;

	if (jit->delayed_call_level) {
		if (!zend_jit_save_call_chain(jit, jit->delayed_call_level)) {
			return 0;
		}
	}

	if (info) {
		call_info = info->callee_info;
		while (call_info && call_info->caller_init_opline != opline) {
			call_info = call_info->next_callee;
		}
		if (call_info && call_info->callee_func && !call_info->is_prototype) {
			func = call_info->callee_func;
		}
	}

	if (!func
	 && trace
	 && trace->op == ZEND_JIT_TRACE_INIT_CALL) {
#ifdef _WIN32
		/* ASLR */
		if (trace->func->type != ZEND_INTERNAL_FUNCTION) {
			func = (zend_function*)trace->func;
		}
#else
		func = (zend_function*)trace->func;
#endif
	}

#ifdef _WIN32
	if (0) {
#else
	if (opline->opcode == ZEND_INIT_FCALL
	 && func
	 && func->type == ZEND_INTERNAL_FUNCTION) {
#endif
		/* load constant address later */
		func_ref = zend_jit_const_addr(jit, (uintptr_t)func);
	} else if (func && op_array == &func->op_array) {
		/* recursive call */
		if (!(func->op_array.fn_flags & ZEND_ACC_IMMUTABLE)) {
//???		    || (sizeof(void*) == 8 && !IS_SIGNED_32BIT(func))) {

			func_ref = zend_jit_load_at_offset(jit, IR_ADDR,
				zend_jit_fp(jit), offsetof(zend_execute_data, func));
		} else {
			func_ref = zend_jit_const_addr(jit, (uintptr_t)func);
		}
	} else {
		ir_ref if_func, cold_path, cache_slot_ref, ref;

		// JIT: if (CACHED_PTR(opline->result.num))
		cache_slot_ref = zend_jit_addr_offset(jit,
			zend_jit_load_at_offset(jit, IR_ADDR,
				zend_jit_fp(jit), offsetof(zend_execute_data, run_time_cache)),
			opline->result.num);
		func_ref = zend_jit_load(jit, IR_ADDR, cache_slot_ref);
		if_func = zend_jit_if(jit, func_ref);
		zend_jit_if_false_cold(jit, if_func);
		if (opline->opcode == ZEND_INIT_FCALL
		 && func
		 && func->type == ZEND_USER_FUNCTION
		 && (func->op_array.fn_flags & ZEND_ACC_IMMUTABLE)) {
			ref = zend_jit_const_addr(jit, (uintptr_t)func);
		    zend_jit_store(jit, cache_slot_ref, ref);
			ref = zend_jit_call_1(jit, IR_ADDR,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_init_func_run_time_cache_helper, IR_CONST_FASTCALL_FUNC),
				ref);
		} else {
			zval *zv = RT_CONSTANT(opline, opline->op2);

			if (opline->opcode == ZEND_INIT_FCALL) {
				ref = zend_jit_call_2(jit, IR_ADDR,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_find_func_helper, IR_CONST_FASTCALL_FUNC),
					zend_jit_const_addr(jit, (uintptr_t)Z_STR_P(zv)),
					cache_slot_ref);
			} else if (opline->opcode == ZEND_INIT_FCALL_BY_NAME) {
				ref = zend_jit_call_2(jit, IR_ADDR,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_find_func_helper, IR_CONST_FASTCALL_FUNC),
					zend_jit_const_addr(jit, (uintptr_t)Z_STR_P(zv + 1)),
					cache_slot_ref);
			} else if (opline->opcode == ZEND_INIT_NS_FCALL_BY_NAME) {
				ref = zend_jit_call_2(jit, IR_ADDR,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_find_ns_func_helper, IR_CONST_FASTCALL_FUNC),
					zend_jit_const_addr(jit, (uintptr_t)zv),
					cache_slot_ref);
			} else {
				ZEND_UNREACHABLE();
			}
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline,
					func && (func->common.fn_flags & ZEND_ACC_IMMUTABLE) ? ZEND_JIT_EXIT_INVALIDATE : 0);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				if (!func || opline->opcode == ZEND_INIT_FCALL) {
					zend_jit_guard(jit, ref, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				} else if (func->type == ZEND_USER_FUNCTION
					 && !(func->common.fn_flags & ZEND_ACC_IMMUTABLE)) {
					const zend_op *opcodes = func->op_array.opcodes;

					zend_jit_guard(jit,
						ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
							zend_jit_load_at_offset(jit, IR_ADDR,
								ref, offsetof(zend_op_array, opcodes)),
							zend_jit_const_addr(jit, (uintptr_t)opcodes)),
						zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				} else {
					zend_jit_guard(jit,
						ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
							ref,
							zend_jit_const_addr(jit, (uintptr_t)func)),
						zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				}
			} else {
zend_jit_set_ex_opline(jit, opline);
				zend_jit_guard(jit, ref,
					zend_jit_stub_addr(jit, jit_stub_undefined_function));
			}
		}
		cold_path = zend_jit_end(jit);
		zend_jit_if_true(jit, if_func);
		zend_jit_merge_2(jit, cold_path, zend_jit_end(jit));
		func_ref = zend_jit_phi_2(jit, ref, func_ref);
	}

	if (!zend_jit_push_call_frame(jit, opline, op_array, func, 0, 0, checked_stack, func_ref, IR_UNUSED)) {
		return 0;
	}

	if (zend_jit_needs_call_chain(call_info, b, op_array, ssa, ssa_op, opline, call_level, trace)) {
		if (!zend_jit_save_call_chain(jit, call_level)) {
			return 0;
		}
	} else {
		ZEND_ASSERT(call_level > 0); //??? delayed_call_chain = 1;
		jit->delayed_call_level = call_level;
		delayed_call_chain = 1; // TODO: remove ???
	}

	return 1;
}

static int zend_jit_init_method_call(zend_jit_ctx         *jit,
                                     const zend_op        *opline,
                                     uint32_t              b,
                                     const zend_op_array  *op_array,
                                     zend_ssa             *ssa,
                                     const zend_ssa_op    *ssa_op,
                                     int                   call_level,
                                     uint32_t              op1_info,
                                     zend_jit_addr         op1_addr,
                                     zend_class_entry     *ce,
                                     bool                  ce_is_instanceof,
                                     bool                  on_this,
                                     bool                  delayed_fetch_this,
                                     zend_class_entry     *trace_ce,
                                     zend_jit_trace_rec   *trace,
                                     int                   checked_stack,
                                     int8_t                func_reg,
                                     int8_t                this_reg,
                                     bool                  polymorphic_side_trace)
{
	zend_func_info *info = ZEND_FUNC_INFO(op_array);
	zend_call_info *call_info = NULL;
	zend_function *func = NULL;
	zval *function_name;
	ir_ref cond, if_static = IR_UNUSED, cold_path, this_ref = IR_NULL, func_ref = IR_NULL;

	ZEND_ASSERT(opline->op2_type == IS_CONST);
	ZEND_ASSERT(op1_info & MAY_BE_OBJECT);

	function_name = RT_CONSTANT(opline, opline->op2);

	if (info) {
		call_info = info->callee_info;
		while (call_info && call_info->caller_init_opline != opline) {
			call_info = call_info->next_callee;
		}
		if (call_info && call_info->callee_func && !call_info->is_prototype) {
			func = call_info->callee_func;
		}
	}

	if (polymorphic_side_trace) {
		/* function is passed in r0 from parent_trace */
		ZEND_ASSERT(func_reg >= 0 && this_reg >= 0);
		func_ref = zend_jit_deopt_rload(jit, IR_ADDR, func_reg);
		this_ref = zend_jit_deopt_rload(jit, IR_ADDR, this_reg);
	} else {
		ir_ref ref, ref2, if_found, fast_path, run_time_cache, this_ref2;

		if (on_this) {
			zend_jit_addr this_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, offsetof(zend_execute_data, This));
			this_ref = zend_jit_zval_ptr(jit, this_addr);
		} else {
		    if (op1_info & MAY_BE_REF) {
				if (opline->op1_type == IS_CV) {
					// JIT: ZVAL_DEREF(op1)
					ir_ref ref = zend_jit_zval_addr(jit, op1_addr);
					ref = zend_jit_zval_ref_deref(jit, ref);
					op1_addr = ZEND_ADDR_REF_ZVAL(ref);
				} else {
					ir_ref if_ref, long_path;

					/* Hack: Convert reference to regular value to simplify JIT code */
					ZEND_ASSERT(Z_REG(op1_addr) == ZREG_FP);

					if_ref = zend_jit_if_zval_type(jit, op1_addr, IS_REFERENCE);
					zend_jit_if_true(jit, if_ref);
					zend_jit_call_1(jit, IR_VOID,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_unref_helper, IR_CONST_FASTCALL_FUNC),
						zend_jit_zval_addr(jit, op1_addr));

					long_path = zend_jit_end(jit);
					zend_jit_if_false(jit, if_ref);
					zend_jit_merge_2(jit, long_path, zend_jit_end(jit));
				}
			}
			if (op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY)- MAY_BE_OBJECT)) {
				if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
					int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
					const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

					if (!exit_addr) {
						return 0;
					}
					zend_jit_guard(jit,
						ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
							zend_jit_zval_type(jit, op1_addr),
							ir_const_u8(&jit->ctx, IS_OBJECT)),
						zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				} else {
					ir_ref if_object = zend_jit_if_zval_type(jit, op1_addr, IS_OBJECT);

					zend_jit_if_false_cold(jit, if_object);

					zend_jit_set_ex_opline(jit, opline);
					if ((opline->op1_type & (IS_VAR|IS_TMP_VAR)) && !delayed_fetch_this) {
						zend_jit_call_1(jit, IR_VOID,
							zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_invalid_method_call_tmp, IR_CONST_FASTCALL_FUNC),
							zend_jit_zval_addr(jit, op1_addr));
					} else {
						zend_jit_call_1(jit, IR_VOID,
							zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_invalid_method_call, IR_CONST_FASTCALL_FUNC),
							zend_jit_zval_addr(jit, op1_addr));
					}
					zend_jit_ijmp(jit, zend_jit_stub_addr(jit, jit_stub_exception_handler));
					zend_jit_if_true(jit, if_object);
				}
			}

			this_ref = zend_jit_zval_ptr(jit, op1_addr);
		}

		if (delayed_call_chain) {
			if (!zend_jit_save_call_chain(jit, jit->delayed_call_level)) {
				return 0;
			}
		}

		if (func) {
			// JIT: fbc = CACHED_PTR(opline->result.num + sizeof(void*));
			ref = zend_jit_load_at_offset(jit, IR_ADDR,
				zend_jit_load_at_offset(jit, IR_ADDR,
					zend_jit_fp(jit), offsetof(zend_execute_data, run_time_cache)),
				opline->result.num + sizeof(void*));

			if_found = zend_jit_if(jit, ref);
			zend_jit_if_true(jit, if_found);
			fast_path = zend_jit_end(jit);
		} else {
			// JIT: if (CACHED_PTR(opline->result.num) == obj->ce)) {
			run_time_cache = zend_jit_load_at_offset(jit, IR_ADDR,
				zend_jit_fp(jit), offsetof(zend_execute_data, run_time_cache));
			ref = ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
				zend_jit_load_at_offset(jit, IR_ADDR,
					run_time_cache, opline->result.num),
				zend_jit_load_at_offset(jit, IR_ADDR,
					this_ref, offsetof(zend_object, ce)));
			if_found = zend_jit_if(jit, ref);
			zend_jit_if_true(jit, if_found);

			// JIT: fbc = CACHED_PTR(opline->result.num + sizeof(void*));
			ref = zend_jit_load_at_offset(jit, IR_ADDR,
				run_time_cache, opline->result.num + sizeof(void*));
			fast_path = zend_jit_end(jit);

		}

		zend_jit_if_false_cold(jit, if_found);
		zend_jit_set_ex_opline(jit, opline);

		// JIT: alloca(sizeof(void*));
		this_ref2 = zend_jit_alloca(jit, zend_jit_const_addr(jit, 0x10));
		zend_jit_store(jit, this_ref2, this_ref);

		if ((opline->op1_type & (IS_VAR|IS_TMP_VAR)) && !delayed_fetch_this) {
			ref2 = zend_jit_call_3(jit, IR_ADDR,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_find_method_tmp_helper, IR_CONST_FASTCALL_FUNC),
					this_ref,
					zend_jit_const_addr(jit, (uintptr_t)function_name),
					this_ref2);
		} else {
			ref2 = zend_jit_call_3(jit, IR_ADDR,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_find_method_helper, IR_CONST_FASTCALL_FUNC),
					this_ref,
					zend_jit_const_addr(jit, (uintptr_t)function_name),
					this_ref2);
		}

		// JIT: revert alloca
		this_ref2 = zend_jit_rload(jit, IR_ADDR, IR_REG_SP);
		this_ref2 = zend_jit_load(jit, IR_ADDR, this_ref2);
		zend_jit_afree(jit, zend_jit_const_addr(jit, 0x10));

		zend_jit_guard(jit, ref2, zend_jit_stub_addr(jit, jit_stub_exception_handler));

		zend_jit_merge_2(jit, fast_path, zend_jit_end(jit));
		func_ref = zend_jit_phi_2(jit, ref, ref2);
		this_ref = zend_jit_phi_2(jit, this_ref, this_ref2);
	}

	if ((!func || zend_jit_may_be_modified(func, op_array))
	 && trace
	 && trace->op == ZEND_JIT_TRACE_INIT_CALL
	 && trace->func
#ifdef _WIN32
	 && trace->func->type != ZEND_INTERNAL_FUNCTION
#endif
	) {
		int32_t exit_point;
		const void *exit_addr;

		exit_point = zend_jit_trace_get_exit_point(opline, func ? ZEND_JIT_EXIT_INVALIDATE : ZEND_JIT_EXIT_METHOD_CALL);
		exit_addr = zend_jit_trace_get_exit_addr(exit_point);
		if (!exit_addr) {
			return 0;
		}

		jit->trace->exit_info[exit_point].poly_func_ref = func_ref;
		jit->trace->exit_info[exit_point].poly_this_ref = this_ref;

		func = (zend_function*)trace->func;

		if (func->type == ZEND_USER_FUNCTION &&
		    (!(func->common.fn_flags & ZEND_ACC_IMMUTABLE) ||
		     (func->common.fn_flags & ZEND_ACC_CLOSURE) ||
		     !func->common.function_name)) {
			const zend_op *opcodes = func->op_array.opcodes;

			zend_jit_guard(jit,
				ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
					zend_jit_load_at_offset(jit, IR_ADDR,
						func_ref, offsetof(zend_op_array, opcodes)),
					zend_jit_const_addr(jit, (uintptr_t)opcodes)),
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		} else {
			zend_jit_guard(jit,
				ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
					func_ref,
					zend_jit_const_addr(jit, (uintptr_t)func)),
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		}
	}

	if (!func) {
		// JIT: if (fbc->common.fn_flags & ZEND_ACC_STATIC) {
		cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
			zend_jit_load_at_offset(jit, IR_U32,
				func_ref, offsetof(zend_function, common.fn_flags)),
			ir_const_u32(&jit->ctx, ZEND_ACC_STATIC));
		if_static = zend_jit_if(jit, cond);
		zend_jit_if_true_cold(jit, if_static);
	}

	if (!func || (func->common.fn_flags & ZEND_ACC_STATIC) != 0) {
		ir_ref ret;

		if ((opline->op1_type & (IS_VAR|IS_TMP_VAR)) && !delayed_fetch_this) {
			ret = zend_jit_call_3(jit, IR_ADDR,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_push_static_metod_call_frame_tmp, IR_CONST_FASTCALL_FUNC),
					this_ref,
					func_ref,
					ir_const_u32(&jit->ctx, opline->extended_value));
		} else {
			ret = zend_jit_call_3(jit, IR_ADDR,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_push_static_metod_call_frame, IR_CONST_FASTCALL_FUNC),
					this_ref,
					func_ref,
					ir_const_u32(&jit->ctx, opline->extended_value));
		}

		if ((opline->op1_type & (IS_VAR|IS_TMP_VAR) && !delayed_fetch_this)) {
			zend_jit_guard(jit, ret, zend_jit_stub_addr(jit, jit_stub_exception_handler));
		}
		zend_jit_store_ip(jit, ret);
	}

	if (!func) {
		cold_path = zend_jit_end(jit);
		zend_jit_if_false(jit, if_static);
	}

	if (!func || (func->common.fn_flags & ZEND_ACC_STATIC) == 0) {
		if (!zend_jit_push_call_frame(jit, opline, NULL, func, 0, delayed_fetch_this, checked_stack, func_ref, this_ref)) {
			return 0;
		}
	}

	if (!func) {
		zend_jit_merge_2(jit, cold_path, zend_jit_end(jit));
	}
	zend_jit_start_reuse_ip(jit);

	if (zend_jit_needs_call_chain(call_info, b, op_array, ssa, ssa_op, opline, call_level, trace)) {
		if (!zend_jit_save_call_chain(jit, call_level)) {
			return 0;
		}
	} else {
		delayed_call_chain = 1; // TODO: remove ???
		jit->delayed_call_level = call_level;
	}

	return 1;
}

static int zend_jit_init_closure_call(zend_jit_ctx         *jit,
                                      const zend_op        *opline,
                                      uint32_t              b,
                                      const zend_op_array  *op_array,
                                      zend_ssa             *ssa,
                                      const zend_ssa_op    *ssa_op,
                                      int                   call_level,
                                      zend_jit_trace_rec   *trace,
                                      int                   checked_stack)
{
	zend_function *func = NULL;
	zend_jit_addr op2_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->op2.var);
	ir_ref ref;

	ref = zend_jit_zval_ptr(jit, op2_addr);

	if (ssa->var_info[ssa_op->op2_use].ce != zend_ce_closure
	 && !(ssa->var_info[ssa_op->op2_use].type & MAY_BE_CLASS_GUARD)) {
		int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
		const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

		if (!exit_addr) {
			return 0;
		}

		zend_jit_guard(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
				zend_jit_load_at_offset(jit, IR_ADDR,
					ref, offsetof(zend_object, ce)),
				zend_jit_const_addr(jit, (uintptr_t)zend_ce_closure)),
			zend_jit_const_addr(jit, (uintptr_t)exit_addr));

		if (ssa->var_info && ssa_op->op2_use >= 0) {
			ssa->var_info[ssa_op->op2_use].type |= MAY_BE_CLASS_GUARD;
			ssa->var_info[ssa_op->op2_use].ce = zend_ce_closure;
			ssa->var_info[ssa_op->op2_use].is_instanceof = 0;
		}
	}

	if (trace
	 && trace->op == ZEND_JIT_TRACE_INIT_CALL
	 && trace->func
	 && trace->func->type == ZEND_USER_FUNCTION) {
		const zend_op *opcodes;
		int32_t exit_point;
		const void *exit_addr;

		func = (zend_function*)trace->func;
		opcodes = func->op_array.opcodes;
		exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_CLOSURE_CALL);
		exit_addr = zend_jit_trace_get_exit_addr(exit_point);
		if (!exit_addr) {
			return 0;
		}

		zend_jit_guard(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
				zend_jit_load_at_offset(jit, IR_ADDR,
					ref, offsetof(zend_closure, func.op_array.opcodes)),
				zend_jit_const_addr(jit, (uintptr_t)opcodes)),
			zend_jit_const_addr(jit, (uintptr_t)exit_addr));
	}

	if (delayed_call_chain) {
		if (!zend_jit_save_call_chain(jit, jit->delayed_call_level)) {
			return 0;
		}
	}

	if (!zend_jit_push_call_frame(jit, opline, NULL, func, 1, 0, checked_stack, ref, IR_UNUSED)) {
		return 0;
	}

	if (zend_jit_needs_call_chain(NULL, b, op_array, ssa, ssa_op, opline, call_level, trace)) {
		if (!zend_jit_save_call_chain(jit, call_level)) {
			return 0;
		}
	} else {
		delayed_call_chain = 1; // TODO: remove ???
		jit->delayed_call_level = call_level;
	}

	if (trace
	 && trace->op == ZEND_JIT_TRACE_END
	 && trace->stop == ZEND_JIT_TRACE_STOP_INTERPRETER) {
		if (!zend_jit_set_ip(jit, opline + 1)) {
			return 0;
		}
	}

	return 1;
}

static int zend_jit_send_val(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr)
{
	uint32_t arg_num = opline->op2.num;
	zend_jit_addr arg_addr;

	ZEND_ASSERT(opline->opcode == ZEND_SEND_VAL || arg_num <= MAX_ARG_FLAG_NUM);

	if (!zend_jit_reuse_ip(jit)) {
		return 0;
	}

	if (opline->opcode == ZEND_SEND_VAL_EX) {
		uint32_t mask = ZEND_SEND_BY_REF << ((arg_num + 3) * 2);

		ZEND_ASSERT(arg_num <= MAX_ARG_FLAG_NUM);

		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE
		 && JIT_G(current_frame)
		 && JIT_G(current_frame)->call
		 && JIT_G(current_frame)->call->func) {
			if (ARG_MUST_BE_SENT_BY_REF(JIT_G(current_frame)->call->func, arg_num)) {
				/* Don't generate code that always throws exception */
				return 0;
			}
		} else {
			ir_ref cond;

			cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
				zend_jit_load_at_offset(jit, IR_U32,
					zend_jit_load_at_offset(jit, IR_ADDR,
						zend_jit_ip(jit), offsetof(zend_execute_data, func)),
					offsetof(zend_function, quick_arg_flags)),
				ir_const_u32(&jit->ctx, mask));

			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);
				if (!exit_addr) {
					return 0;
				}
				zend_jit_guard_not(jit, cond, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				ir_ref if_pass_by_ref;

				if_pass_by_ref = zend_jit_if(jit, cond);

				zend_jit_if_true_cold(jit, if_pass_by_ref);
				if (Z_MODE(op1_addr) == IS_REG) {
					/* set type to avoid zval_ptr_dtor() on uninitialized value */
					zend_jit_addr addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->op1.var);
					zend_jit_zval_set_type_info(jit, addr, IS_UNDEF);
				}
				zend_jit_set_ex_opline(jit, opline);
				zend_jit_ijmp(jit,
					zend_jit_stub_addr(jit, jit_stub_throw_cannot_pass_by_ref));

				zend_jit_if_false(jit, if_pass_by_ref);
			}
		}
	}

	arg_addr = ZEND_ADDR_MEM_ZVAL(ZREG_RX, opline->result.var);

	if (opline->op1_type == IS_CONST) {
		zval *zv = RT_CONSTANT(opline, opline->op1);

		zend_jit_copy_zval_const(jit,
			arg_addr,
			MAY_BE_ANY, MAY_BE_ANY,
			zv, 1);
	} else {
		zend_jit_copy_zval(jit,
			arg_addr,
			MAY_BE_ANY,
			op1_addr, op1_info, 0);
	}

	return 1;
}

static int zend_jit_send_ref(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, uint32_t op1_info, int cold)
{
	zend_jit_addr op1_addr, arg_addr, ref_addr;
	ir_ref ref_path = IR_UNUSED;

	op1_addr = OP1_ADDR();
	arg_addr = ZEND_ADDR_MEM_ZVAL(ZREG_RX, opline->result.var);

	if (!zend_jit_reuse_ip(jit)) {
		return 0;
	}

	if (opline->op1_type == IS_VAR) {
		if (op1_info & MAY_BE_INDIRECT) {
			op1_addr = zend_jit_zval_indirect_deref(jit, op1_addr);
		}
	} else if (opline->op1_type == IS_CV) {
		if (op1_info & MAY_BE_UNDEF) {
			if (op1_info & (MAY_BE_ANY|MAY_BE_REF)) {
				ir_ref if_undef, long_path;

				// JIT: if (Z_TYPE_P(op1) == IS_UNDEF)
				if_undef = zend_jit_if_zval_type(jit, op1_addr, IS_UNDEF);
				zend_jit_if_true(jit, if_undef);
				// JIT: ZVAL_NULL(op1)
				zend_jit_zval_set_type_info(jit,op1_addr, IS_NULL);
				long_path = zend_jit_end(jit);
				zend_jit_if_false(jit, if_undef);
				zend_jit_merge_2(jit, long_path, zend_jit_end(jit));
			}
			op1_info &= ~MAY_BE_UNDEF;
			op1_info |= MAY_BE_NULL;
		}
	} else {
		ZEND_UNREACHABLE();
	}

	if (op1_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) {
		ir_ref ref, ref2;

		if (op1_info & MAY_BE_REF) {
			ir_ref if_ref;

			// JIT: if (Z_TYPE_P(op1) == IS_UNDEF)
			if_ref = zend_jit_if_zval_type(jit, op1_addr, IS_REFERENCE);
			zend_jit_if_true(jit, if_ref);
			// JIT: ref = Z_PTR_P(op1)
			ref = zend_jit_zval_ptr(jit, op1_addr);
			// JIT: GC_ADDREF(ref)
			zend_jit_gc_addref(jit, ref);
			// JIT: ZVAL_REFERENCE(arg, ref)
			zend_jit_zval_set_ptr(jit, arg_addr, ref);
			zend_jit_zval_set_type_info(jit, arg_addr, IS_REFERENCE_EX);
			ref_path = zend_jit_end(jit);
			zend_jit_if_false(jit, if_ref);
		}

		// JIT: ZVAL_NEW_REF(arg, varptr);
		// JIT: ref = emalloc(sizeof(zend_reference));
		ref = zend_jit_emalloc(jit, sizeof(zend_reference), op_array, opline);
		// JIT: GC_REFERENCE(ref) = 2
		zend_jit_gc_set_refcount(jit, ref, 2);
		// JIT: GC_TYPE(ref) = GC_REFERENCE
		zend_jit_store_at_offset(jit,
			ref, offsetof(zend_reference, gc.u.type_info),
			ir_const_u32(&jit->ctx, GC_REFERENCE));
		zend_jit_store_at_offset(jit,
			ref, offsetof(zend_reference, sources.ptr),
			IR_NULL);
		ref2 = zend_jit_addr_offset(jit, ref, offsetof(zend_reference, val));
		ref_addr = ZEND_ADDR_REF_ZVAL(ref2);

        // JIT: ZVAL_COPY_VALUE(&ref->val, op1)
		zend_jit_copy_zval(jit,
			ref_addr,
			MAY_BE_ANY,
			op1_addr, op1_info, 0);

		// JIT: ZVAL_REFERENCE(arg, ref)
		zend_jit_zval_set_ptr(jit, op1_addr, ref);
		zend_jit_zval_set_type_info(jit, op1_addr, IS_REFERENCE_EX);

		// JIT: ZVAL_REFERENCE(arg, ref)
		zend_jit_zval_set_ptr(jit, arg_addr, ref);
		zend_jit_zval_set_type_info(jit, arg_addr, IS_REFERENCE_EX);
	}

	if (ref_path) {
		zend_jit_merge_2(jit, ref_path, zend_jit_end(jit));
	}

	zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);

	return 1;
}

static int zend_jit_send_var(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, uint32_t op1_info, zend_jit_addr op1_addr, zend_jit_addr op1_def_addr)
{
	uint32_t arg_num = opline->op2.num;
	zend_jit_addr arg_addr;
	ir_ref end_inputs[8];
	uint32_t end_inputs_count = 0;

	ZEND_ASSERT((opline->opcode != ZEND_SEND_VAR_EX &&
	     opline->opcode != ZEND_SEND_VAR_NO_REF_EX) ||
	    arg_num <= MAX_ARG_FLAG_NUM);

	arg_addr = ZEND_ADDR_MEM_ZVAL(ZREG_RX, opline->result.var);

	if (!zend_jit_reuse_ip(jit)) {
		return 0;
	}

	if (opline->opcode == ZEND_SEND_VAR_EX) {
		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE
		 && JIT_G(current_frame)
		 && JIT_G(current_frame)->call
		 && JIT_G(current_frame)->call->func) {
			if (ARG_SHOULD_BE_SENT_BY_REF(JIT_G(current_frame)->call->func, arg_num)) {
				if (!zend_jit_send_ref(jit, opline, op_array, op1_info, 0)) {
					return 0;
				}
				return 1;
			}
		} else {
			uint32_t mask = (ZEND_SEND_BY_REF|ZEND_SEND_PREFER_REF) << ((arg_num + 3) * 2);
			ir_ref cond, if_send_by_ref;

			// JIT: if (RX->func->quick_arg_flags & mask)
			cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
				zend_jit_load_at_offset(jit, IR_U32,
					zend_jit_load_at_offset(jit, IR_ADDR,
						zend_jit_ip(jit), offsetof(zend_execute_data, func)),
					offsetof(zend_function, quick_arg_flags)),
				ir_const_u32(&jit->ctx, mask));
			if_send_by_ref = zend_jit_if(jit, cond);
			zend_jit_if_true_cold(jit, if_send_by_ref);

			if (!zend_jit_send_ref(jit, opline, op_array, op1_info, 1)) {
				return 0;
			}

			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_if_false(jit, if_send_by_ref);
		}
	} else if (opline->opcode == ZEND_SEND_VAR_NO_REF_EX) {
		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE
		 && JIT_G(current_frame)
		 && JIT_G(current_frame)->call
		 && JIT_G(current_frame)->call->func) {
			if (ARG_SHOULD_BE_SENT_BY_REF(JIT_G(current_frame)->call->func, arg_num)) {

		        // JIT: ZVAL_COPY_VALUE(arg, op1)
				zend_jit_copy_zval(jit,
					arg_addr,
					MAY_BE_ANY,
					op1_addr, op1_info, 0);

				if (!ARG_MAY_BE_SENT_BY_REF(JIT_G(current_frame)->call->func, arg_num)) {
					if (!(op1_info & MAY_BE_REF)) {
						/* Don't generate code that always throws exception */
						return 0;
					} else {
						int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
						const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);
						if (!exit_addr) {
							return 0;
						}

						// JIT: if (Z_TYPE_P(op1) != IS_REFERENCE)
						zend_jit_guard(jit,
							ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
								zend_jit_zval_type(jit, op1_addr),
								ir_const_u32(&jit->ctx, IS_REFERENCE)),
							zend_jit_const_addr(jit, (uintptr_t)exit_addr));
					}
				}
				return 1;
			}
		} else {
			uint32_t mask = (ZEND_SEND_BY_REF|ZEND_SEND_PREFER_REF) << ((arg_num + 3) * 2);
			ir_ref cond, func, if_send_by_ref, if_prefer_ref;

			// JIT: if (RX->func->quick_arg_flags & mask)
			func = zend_jit_load_at_offset(jit, IR_ADDR,
				zend_jit_ip(jit), offsetof(zend_execute_data, func));
			cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
				zend_jit_load_at_offset(jit, IR_U32,
					func, offsetof(zend_function, quick_arg_flags)),
				ir_const_u32(&jit->ctx, mask));
			if_send_by_ref = zend_jit_if(jit, cond);
			zend_jit_if_true_cold(jit, if_send_by_ref);

			mask = ZEND_SEND_PREFER_REF << ((arg_num + 3) * 2);

	        // JIT: ZVAL_COPY_VALUE(arg, op1)
			zend_jit_copy_zval(jit,
				arg_addr,
				MAY_BE_ANY,
				op1_addr, op1_info, 0);

			if (op1_info & MAY_BE_REF) {
				ir_ref if_ref = zend_jit_if_zval_type(jit, arg_addr, IS_REFERENCE);
				zend_jit_if_true(jit, if_ref);
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
				zend_jit_if_false(jit, if_ref);
			}

			// JIT: if (RX->func->quick_arg_flags & mask)
			cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
				zend_jit_load_at_offset(jit, IR_U32,
					func, offsetof(zend_function, quick_arg_flags)),
				ir_const_u32(&jit->ctx, mask));
			if_prefer_ref = zend_jit_if(jit, cond);
			zend_jit_if_true(jit, if_prefer_ref);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_if_false(jit, if_prefer_ref);

			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);
				if (!exit_addr) {
					return 0;
				}
				zend_jit_side_exit(jit, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				zend_jit_set_ex_opline(jit, opline);
				zend_jit_call_1(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_only_vars_by_reference, IR_CONST_FASTCALL_FUNC),
					zend_jit_zval_addr(jit, arg_addr));
				zend_jit_check_exception(jit);
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}

			zend_jit_if_false(jit, if_send_by_ref);
		}
	} else if (opline->opcode == ZEND_SEND_FUNC_ARG) {
		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE
		 && JIT_G(current_frame)
		 && JIT_G(current_frame)->call
		 && JIT_G(current_frame)->call->func) {
			if (ARG_SHOULD_BE_SENT_BY_REF(JIT_G(current_frame)->call->func, arg_num)) {
				if (!zend_jit_send_ref(jit, opline, op_array, op1_info, 0)) {
					return 0;
				}
				return 1;
			}
		} else {
			ir_ref cond, if_send_by_ref;

			// JIT: if (RX->This.u1.type_info & ZEND_CALL_SEND_ARG_BY_REF)
			cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
				zend_jit_load_at_offset(jit, IR_U32,
					zend_jit_ip(jit), offsetof(zend_execute_data, This.u1.type_info)),
				ir_const_u32(&jit->ctx, ZEND_CALL_SEND_ARG_BY_REF));
			if_send_by_ref = zend_jit_if(jit, cond);
			zend_jit_if_true_cold(jit, if_send_by_ref);

			if (!zend_jit_send_ref(jit, opline, op_array, op1_info, 1)) {
				return 0;
			}

			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_if_false(jit, if_send_by_ref);
		}
	}

	if (op1_info & MAY_BE_UNDEF) {
		ir_ref ref, if_undef = IR_UNUSED;

		if (op1_info & (MAY_BE_ANY|MAY_BE_REF)) {
			if_undef = zend_jit_if_zval_type(jit, op1_addr, IS_UNDEF);
			zend_jit_if_true_cold(jit, if_undef);
		}

		// JIT: zend_jit_undefined_op_helper(opline->op1.var)
		zend_jit_set_ex_opline(jit, opline);
		ref = zend_jit_call_1(jit, IR_I32,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_undefined_op_helper, IR_CONST_FASTCALL_FUNC),
			ir_const_u32(&jit->ctx, opline->op1.var));

		// JIT: ZVAL_NULL(arg)
		zend_jit_zval_set_type_info(jit, arg_addr, IS_NULL);

		// JIT: check_exception
		zend_jit_guard(jit, ref, zend_jit_stub_addr(jit, jit_stub_exception_handler));

		if (op1_info & (MAY_BE_ANY|MAY_BE_REF)) {
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_if_false(jit, if_undef);
		} else {
			if (end_inputs_count) {
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
				zend_jit_merge_N(jit, end_inputs_count, end_inputs);
			}
			return 1;
		}
	}

	if (opline->opcode == ZEND_SEND_VAR_NO_REF) {
        // JIT: ZVAL_COPY_VALUE(arg, op1)
		zend_jit_copy_zval(jit,
			arg_addr,
			MAY_BE_ANY,
			op1_addr, op1_info, 0);
		if (op1_info & MAY_BE_REF) {
				// JIT: if (Z_TYPE_P(arg) == IS_REFERENCE)
				ir_ref if_ref = zend_jit_if_zval_type(jit, arg_addr, IS_REFERENCE);
				zend_jit_if_true(jit, if_ref);
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
				zend_jit_if_false(jit, if_ref);
		}
		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
			int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
			const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);
			if (!exit_addr) {
				return 0;
			}
			zend_jit_guard(jit, IR_FALSE, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		} else {
			zend_jit_set_ex_opline(jit, opline);
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_only_vars_by_reference, IR_CONST_FASTCALL_FUNC),
				zend_jit_zval_addr(jit, arg_addr));
			zend_jit_check_exception(jit);
		}
	} else {
		if (op1_info & MAY_BE_REF) {
			if (opline->op1_type == IS_CV) {
				ir_ref ref;

				// JIT: ZVAL_DEREF(op1)
				ref = zend_jit_zval_addr(jit, op1_addr);
				ref = zend_jit_zval_ref_deref(jit, ref);
				op1_addr = ZEND_ADDR_REF_ZVAL(ref);

		        // JIT: ZVAL_COPY(arg, op1)
				zend_jit_copy_zval(jit,
					arg_addr,
					MAY_BE_ANY,
					op1_addr, op1_info, 1);
			} else {
				ir_ref if_ref, ref, ref2, refcount, if_not_zero, if_refcounted;
				zend_jit_addr ref_addr;

				// JIT: if (Z_TYPE_P(op1) == IS_REFERENCE)
				if_ref = zend_jit_if_zval_type(jit, op1_addr, IS_REFERENCE);
				zend_jit_if_true_cold(jit, if_ref);

				// JIT: ref = Z_COUNTED_P(op1);
				ref = zend_jit_zval_ptr(jit, op1_addr);
				ref2 = zend_jit_addr_offset(jit, ref, offsetof(zend_reference, val));
				ref_addr = ZEND_ADDR_REF_ZVAL(ref2);

				// JIT: ZVAL_COPY_VALUE(arg, op1);
				zend_jit_copy_zval(jit,
					arg_addr,
					MAY_BE_ANY,
					ref_addr, op1_info, 0);

				// JIT: if (GC_DELREF(ref) != 0)
				refcount = zend_jit_gc_delref(jit, ref);
				if_not_zero = zend_jit_if(jit, refcount);
				zend_jit_if_true(jit, if_not_zero);

                // JIT: if (Z_REFCOUNTED_P(arg)
				if_refcounted = zend_jit_if_zval_refcounted(jit, arg_addr);
				zend_jit_if_true(jit, if_refcounted);
				// JIT: Z_ADDREF_P(arg)
				zend_jit_gc_addref(jit, zend_jit_zval_ptr(jit, arg_addr));
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
				zend_jit_if_false(jit, if_refcounted);
				zend_jit_array_add(end_inputs, zend_jit_end(jit));


				zend_jit_if_false(jit, if_not_zero);

				// JIT: efree(ref)
				zend_jit_efree(jit, ref, sizeof(zend_reference), op_array, opline);
				zend_jit_array_add(end_inputs, zend_jit_end(jit));

				zend_jit_if_false(jit, if_ref);

				// JIT: ZVAL_COPY_VALUE(arg, op1);
				zend_jit_copy_zval(jit,
					arg_addr,
					MAY_BE_ANY,
					op1_addr, op1_info, 0);
			}
		} else {
			if (op1_addr != op1_def_addr) {
				if (!zend_jit_update_regs(jit, opline->op1.var, op1_addr, op1_def_addr, op1_info)) {
					return 0;
				}
				if (Z_MODE(op1_def_addr) == IS_REG && Z_MODE(op1_addr) != IS_REG) {
					op1_addr = op1_def_addr;
				}
			}

	        // JIT: ZVAL_COPY_VALUE(arg, op1)
			zend_jit_copy_zval(jit,
				arg_addr,
				MAY_BE_ANY,
				op1_addr, op1_info, opline->op1_type == IS_CV);
		}
	}

	if (end_inputs_count) {
		zend_jit_array_add(end_inputs, zend_jit_end(jit));
		zend_jit_merge_N(jit, end_inputs_count, end_inputs);
	}

	return 1;
}

static int zend_jit_check_func_arg(zend_jit_ctx *jit, const zend_op *opline)
{
	uint32_t arg_num = opline->op2.num;
	ir_ref ref;

	if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE
	 && JIT_G(current_frame)
	 && JIT_G(current_frame)->call
	 && JIT_G(current_frame)->call->func) {
		if (ARG_SHOULD_BE_SENT_BY_REF(JIT_G(current_frame)->call->func, arg_num)) {
			if (!TRACE_FRAME_IS_LAST_SEND_BY_REF(JIT_G(current_frame)->call)) {
				TRACE_FRAME_SET_LAST_SEND_BY_REF(JIT_G(current_frame)->call);
				// JIT: ZEND_ADD_CALL_FLAG(EX(call), ZEND_CALL_SEND_ARG_BY_REF);
				if (jit->reuse_ip) {
					ref = zend_jit_ip(jit);
				} else {
					ref = zend_jit_load_at_offset(jit, IR_ADDR,
						zend_jit_fp(jit), offsetof(zend_execute_data, call));
				}
				ref = zend_jit_addr_offset(jit, ref, offsetof(zend_execute_data, This.u1.type_info));
				zend_jit_store(jit,
					ref,
					ir_fold2(&jit->ctx, IR_OPT(IR_OR, IR_U32),
						zend_jit_load(jit, IR_U32, ref),
						ir_const_u32(&jit->ctx, ZEND_CALL_SEND_ARG_BY_REF)));
			}
		} else {
			if (!TRACE_FRAME_IS_LAST_SEND_BY_VAL(JIT_G(current_frame)->call)) {
				TRACE_FRAME_SET_LAST_SEND_BY_VAL(JIT_G(current_frame)->call);
				// JIT: ZEND_DEL_CALL_FLAG(EX(call), ZEND_CALL_SEND_ARG_BY_REF);
				if (jit->reuse_ip) {
					ref = zend_jit_ip(jit);
				} else {
					ref = zend_jit_load_at_offset(jit, IR_ADDR,
						zend_jit_fp(jit), offsetof(zend_execute_data, call));
				}
				ref = zend_jit_addr_offset(jit, ref, offsetof(zend_execute_data, This.u1.type_info));
				zend_jit_store(jit,
					ref,
					ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
						zend_jit_load(jit, IR_U32, ref),
						ir_const_u32(&jit->ctx, ~ZEND_CALL_SEND_ARG_BY_REF)));
			}
		}
	} else {
		// JIT: if (QUICK_ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
		uint32_t mask = (ZEND_SEND_BY_REF|ZEND_SEND_PREFER_REF) << ((arg_num + 3) * 2);
		ir_ref rx, if_ref, cold_path;

		if (!zend_jit_reuse_ip(jit)) {
			return 0;
		}

		rx = zend_jit_ip(jit);

		ref = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
			zend_jit_load_at_offset(jit, IR_U32,
				zend_jit_load_at_offset(jit, IR_ADDR,
					rx, offsetof(zend_execute_data, func)),
				offsetof(zend_function, quick_arg_flags)),
			ir_const_u32(&jit->ctx, mask));
		if_ref = zend_jit_if(jit, ref);
		zend_jit_if_true_cold(jit, if_ref);

		// JIT: ZEND_ADD_CALL_FLAG(EX(call), ZEND_CALL_SEND_ARG_BY_REF);
		ref = zend_jit_addr_offset(jit, rx, offsetof(zend_execute_data, This.u1.type_info));
		zend_jit_store(jit,
			ref,
			ir_fold2(&jit->ctx, IR_OPT(IR_OR, IR_U32),
				zend_jit_load(jit, IR_U32, ref),
				ir_const_u32(&jit->ctx, ZEND_CALL_SEND_ARG_BY_REF)));

		cold_path = zend_jit_end(jit);
		zend_jit_if_false(jit, if_ref);

		// JIT: ZEND_DEL_CALL_FLAG(EX(call), ZEND_CALL_SEND_ARG_BY_REF);
		ref = zend_jit_addr_offset(jit, rx, offsetof(zend_execute_data, This.u1.type_info));
		zend_jit_store(jit,
			ref,
			ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
				zend_jit_load(jit, IR_U32, ref),
				ir_const_u32(&jit->ctx, ~ZEND_CALL_SEND_ARG_BY_REF)));

		zend_jit_merge_2(jit, cold_path, zend_jit_end(jit));
	}

	return 1;
}

static int zend_jit_check_undef_args(zend_jit_ctx *jit, const zend_op *opline)
{
	ir_ref call, cond, if_may_have_undef, ret, cold_path;

	if (jit->reuse_ip) {
		call = zend_jit_ip(jit);
	} else {
		call = zend_jit_load_at_offset(jit, IR_ADDR,
			zend_jit_fp(jit), offsetof(zend_execute_data, call));
	}

	cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U8),
		zend_jit_load_at_offset(jit, IR_U8,
			call, offsetof(zend_execute_data, This.u1.type_info) + 3),
		ir_const_u8(&jit->ctx, ZEND_CALL_MAY_HAVE_UNDEF >> 24));
	if_may_have_undef = zend_jit_if(jit, cond);

	zend_jit_if_true_cold(jit, if_may_have_undef);
	zend_jit_set_ex_opline(jit, opline);
	ret = zend_jit_call_1(jit, IR_I32,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_handle_undef_args, IR_CONST_FASTCALL_FUNC),
		call);
	zend_jit_guard_not(jit, ret,
		zend_jit_stub_addr(jit, jit_stub_exception_handler));
	cold_path = zend_jit_end(jit);

	zend_jit_if_false(jit, if_may_have_undef);
	zend_jit_merge_2(jit, cold_path, zend_jit_end(jit));

	return 1;
}

static int zend_jit_do_fcall(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, zend_ssa *ssa, int call_level, unsigned int next_block, zend_jit_trace_rec *trace)
{
	zend_func_info *info = ZEND_FUNC_INFO(op_array);
	zend_call_info *call_info = NULL;
	const zend_function *func = NULL;
	uint32_t i;
	uint32_t call_num_args = 0;
	bool unknown_num_args = 0;
	const void *exit_addr = NULL;
	const zend_op *prev_opline;
	ir_ref rx, func_ref = IR_UNUSED, if_user = IR_UNUSED, user_path = IR_UNUSED;

	prev_opline = opline - 1;
	while (prev_opline->opcode == ZEND_EXT_FCALL_BEGIN || prev_opline->opcode == ZEND_TICKS) {
		prev_opline--;
	}
	if (prev_opline->opcode == ZEND_SEND_UNPACK || prev_opline->opcode == ZEND_SEND_ARRAY ||
			prev_opline->opcode == ZEND_CHECK_UNDEF_ARGS) {
		unknown_num_args = 1;
	}

	if (info) {
		call_info = info->callee_info;
		while (call_info && call_info->caller_call_opline != opline) {
			call_info = call_info->next_callee;
		}
		if (call_info && call_info->callee_func && !call_info->is_prototype) {
			func = call_info->callee_func;
		}
		if ((op_array->fn_flags & ZEND_ACC_TRAIT_CLONE)
		 && JIT_G(current_frame)
		 && JIT_G(current_frame)->call
		 && !JIT_G(current_frame)->call->func) {
			call_info = NULL; func = NULL; /* megamorphic call from trait */
		}
	}
	if (!func) {
		/* resolve function at run time */
	} else if (func->type == ZEND_USER_FUNCTION) {
		ZEND_ASSERT(opline->opcode != ZEND_DO_ICALL);
		call_num_args = call_info->num_args;
	} else if (func->type == ZEND_INTERNAL_FUNCTION) {
		ZEND_ASSERT(opline->opcode != ZEND_DO_UCALL);
		call_num_args = call_info->num_args;
	} else {
		ZEND_UNREACHABLE();
	}

	if (trace && !func) {
		if (trace->op == ZEND_JIT_TRACE_DO_ICALL) {
			ZEND_ASSERT(trace->func->type == ZEND_INTERNAL_FUNCTION);
#ifndef ZEND_WIN32
			// TODO: ASLR may cause different addresses in different workers ???
			func = trace->func;
			if (JIT_G(current_frame) &&
			    JIT_G(current_frame)->call &&
			    TRACE_FRAME_NUM_ARGS(JIT_G(current_frame)->call) >= 0) {
				call_num_args = TRACE_FRAME_NUM_ARGS(JIT_G(current_frame)->call);
			} else {
				unknown_num_args = 1;
			}
#endif
		} else if (trace->op == ZEND_JIT_TRACE_ENTER) {
			ZEND_ASSERT(trace->func->type == ZEND_USER_FUNCTION);
			if (zend_accel_in_shm(trace->func->op_array.opcodes)) {
				func = trace->func;
				if (JIT_G(current_frame) &&
				    JIT_G(current_frame)->call &&
				    TRACE_FRAME_NUM_ARGS(JIT_G(current_frame)->call) >= 0) {
					call_num_args = TRACE_FRAME_NUM_ARGS(JIT_G(current_frame)->call);
				} else {
					unknown_num_args = 1;
				}
			}
		}
	}

	bool may_have_extra_named_params =
		opline->extended_value == ZEND_FCALL_MAY_HAVE_EXTRA_NAMED_PARAMS &&
		(!func || func->common.fn_flags & ZEND_ACC_VARIADIC);

	if (!jit->reuse_ip) {
		zend_jit_start_reuse_ip(jit);
		// JIT: call = EX(call);
		zend_jit_store_ip(jit,
			zend_jit_load_at_offset(jit, IR_ADDR,
				zend_jit_fp(jit), offsetof(zend_execute_data, call)));
	}
	rx = zend_jit_ip(jit);
	zend_jit_stop_reuse_ip(jit);

	zend_jit_set_ex_opline(jit, opline);

	if (opline->opcode == ZEND_DO_FCALL) {
		if (!func) {
			if (trace) {
				uint32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);

				exit_addr = zend_jit_trace_get_exit_addr(exit_point);
				if (!exit_addr) {
					return 0;
				}

				func_ref = zend_jit_load_at_offset(jit, IR_ADDR,
					rx, offsetof(zend_execute_data, func));
				zend_jit_guard_not(jit,
					ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
						zend_jit_load_at_offset(jit, IR_U32,
							func_ref, offsetof(zend_op_array, fn_flags)),
						ir_const_u32(&jit->ctx, ZEND_ACC_DEPRECATED)),
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			}
		}
	}

	if (!delayed_call_chain) {
		// JIT: EX(call) = call->prev_execute_data;
		zend_jit_store_at_offset(jit,
			zend_jit_fp(jit), offsetof(zend_execute_data, call),
			(call_level == 1) ?
				IR_NULL :
				zend_jit_load_at_offset(jit, IR_ADDR,
					rx, offsetof(zend_execute_data, prev_execute_data)));
	}
	delayed_call_chain = 0;
	jit->delayed_call_level = 0;

	// JIT: call->prev_execute_data = execute_data;
	zend_jit_store_at_offset(jit,
		rx, offsetof(zend_execute_data, prev_execute_data),
		zend_jit_fp(jit));

	if (!func) {
		if (!func_ref) {
			func_ref = zend_jit_load_at_offset(jit, IR_ADDR,
				rx, offsetof(zend_execute_data, func));
		}
	}

	if (opline->opcode == ZEND_DO_FCALL) {
		if (!func) {
			if (!trace) {
				ir_ref cond, if_deprecated, cold_path, ret;

				cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
						zend_jit_load_at_offset(jit, IR_U32,
							func_ref, offsetof(zend_op_array, fn_flags)),
						ir_const_u32(&jit->ctx, ZEND_ACC_DEPRECATED));
				if_deprecated = zend_jit_if(jit, cond);
				zend_jit_if_true_cold(jit, if_deprecated);

				if (GCC_GLOBAL_REGS) {
					ret = zend_jit_call_0(jit, IR_BOOL,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_deprecated_helper, IR_CONST_FASTCALL_FUNC));
				} else {
					ret = zend_jit_call_1(jit, IR_BOOL,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_deprecated_helper, IR_CONST_FASTCALL_FUNC),
						rx);
				}
				zend_jit_guard(jit, ret, zend_jit_stub_addr(jit, jit_stub_exception_handler));

				cold_path = zend_jit_end(jit);
				zend_jit_if_false(jit, if_deprecated);
				zend_jit_merge_2(jit, cold_path, zend_jit_end(jit));
			}
		} else if (func->common.fn_flags & ZEND_ACC_DEPRECATED) {
			ir_ref ret;

			if (GCC_GLOBAL_REGS) {
				ret = zend_jit_call_0(jit, IR_BOOL,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_deprecated_helper, IR_CONST_FASTCALL_FUNC));
			} else {
				ret = zend_jit_call_1(jit, IR_BOOL,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_deprecated_helper, IR_CONST_FASTCALL_FUNC),
					rx);
			}
			zend_jit_guard(jit, ret, zend_jit_stub_addr(jit, jit_stub_exception_handler));
		}
	}

	if (!func
	 && opline->opcode != ZEND_DO_UCALL
	 && opline->opcode != ZEND_DO_ICALL) {
		ir_ref type_ref = zend_jit_load_at_offset(jit, IR_U8,
			func_ref, offsetof(zend_function, type));
		if_user = zend_jit_if(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
				type_ref,
				ir_const_u8(&jit->ctx, ZEND_USER_FUNCTION)));
		zend_jit_if_true(jit, if_user);
	}

	if ((!func || func->type == ZEND_USER_FUNCTION)
	 && opline->opcode != ZEND_DO_ICALL) {
		bool recursive_call_through_jmp = 0;

		// JIT: EX(call) = NULL;
		zend_jit_store_at_offset(jit,
			rx, offsetof(zend_execute_data, call),
			IR_NULL);

		// JIT: EX(return_value) = RETURN_VALUE_USED(opline) ? EX_VAR(opline->result.var) : 0;
		zend_jit_store_at_offset(jit,
			rx, offsetof(zend_execute_data, return_value),
			RETURN_VALUE_USED(opline) ?
				zend_jit_mem_zval_addr(jit, ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var)) :
				IR_NULL);

		// JIT: EX_LOAD_RUN_TIME_CACHE(op_array);
		if (!func || func->op_array.cache_size) {
			ir_ref run_time_cache;

			if (func && op_array == &func->op_array) {
				/* recursive call */
				run_time_cache = zend_jit_load_at_offset(jit, IR_ADDR,
					zend_jit_fp(jit), offsetof(zend_execute_data, run_time_cache));
			} else if (func
			 && !(func->op_array.fn_flags & ZEND_ACC_CLOSURE)
			 && ZEND_MAP_PTR_IS_OFFSET(func->op_array.run_time_cache)) {
				run_time_cache = zend_jit_load_at_offset(jit, IR_ADDR,
					zend_jit_load(jit, IR_ADDR, ZEND_JIT_CG_ADDR(map_ptr_base)),
					(uintptr_t)ZEND_MAP_PTR(func->op_array.run_time_cache));
			} else if ((func && (func->op_array.fn_flags & ZEND_ACC_CLOSURE)) ||
					(JIT_G(current_frame) &&
					 JIT_G(current_frame)->call &&
					 TRACE_FRAME_IS_CLOSURE_CALL(JIT_G(current_frame)->call))) {
				/* Closures always use direct pointers */
				ir_ref local_func_ref = func_ref ? func_ref :
					zend_jit_load_at_offset(jit, IR_ADDR,
						rx, offsetof(zend_execute_data, func));

				run_time_cache = zend_jit_load_at_offset(jit, IR_ADDR,
					local_func_ref, offsetof(zend_op_array, run_time_cache__ptr));
			} else {
				ir_ref if_odd, long_path, run_time_cache2;

				/* TODO: try avoiding this run-time load ??? */
				ir_ref local_func_ref = func_ref ? func_ref :
					zend_jit_load_at_offset(jit, IR_ADDR,
						rx, offsetof(zend_execute_data, func));

				run_time_cache = zend_jit_load_at_offset(jit, IR_ADDR,
					local_func_ref, offsetof(zend_op_array, run_time_cache__ptr));
				if_odd = zend_jit_if(jit,
					ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_ADDR),
						run_time_cache,
						zend_jit_const_addr(jit, 1)));
				zend_jit_if_true(jit, if_odd);

				run_time_cache2 = zend_jit_load(jit, IR_ADDR,
					ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
						run_time_cache,
						zend_jit_load(jit, IR_ADDR, ZEND_JIT_CG_ADDR(map_ptr_base))));

				long_path = zend_jit_end(jit);
				zend_jit_if_false(jit, if_odd);
				zend_jit_merge_2(jit, long_path, zend_jit_end(jit));
				run_time_cache = zend_jit_phi_2(jit, run_time_cache2, run_time_cache);
			}

			zend_jit_store_at_offset(jit,
				rx, offsetof(zend_execute_data, run_time_cache),
				run_time_cache);
		}

		// JIT: EG(current_execute_data) = execute_data = call;
		zend_jit_store(jit,
			ZEND_JIT_EG_ADDR(current_execute_data),
			rx);
		zend_jit_store_fp(jit, rx);

		// JIT: opline = op_array->opcodes;
		if (func && !unknown_num_args) {

			for (i = call_num_args; i < func->op_array.last_var; i++) {
				uint32_t n = EX_NUM_TO_VAR(i);
				zend_jit_addr var_addr = ZEND_ADDR_MEM_ZVAL(ZREG_RX, n);

				zend_jit_zval_set_type_info(jit, var_addr, IS_UNDEF);
			}

			if (call_num_args <= func->op_array.num_args) {
				if (!trace || (trace->op == ZEND_JIT_TRACE_END
				 && trace->stop == ZEND_JIT_TRACE_STOP_INTERPRETER)) {
					uint32_t num_args;

					if ((func->op_array.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) != 0) {
						if (trace) {
							num_args = 0;
						} else if (call_info) {
							num_args = skip_valid_arguments(op_array, ssa, call_info);
						} else {
							num_args = call_num_args;
						}
					} else {
						num_args = call_num_args;
					}
					if (zend_accel_in_shm(func->op_array.opcodes)) {
						zend_jit_load_ip_addr(jit, func->op_array.opcodes + num_args);
					} else {
						if (!func_ref) {
							func_ref = zend_jit_load_at_offset(jit, IR_ADDR,
								rx, offsetof(zend_execute_data, func));
						}
						ir_ref ip = zend_jit_load_at_offset(jit, IR_ADDR,
							func_ref, offsetof(zend_op_array, opcodes));
						if (num_args) {
							ip = zend_jit_addr_offset(jit, ip, num_args * sizeof(zend_op));
						}
						zend_jit_load_ip(jit, ip);
					}

					if (GCC_GLOBAL_REGS && !trace && op_array == &func->op_array
							&& num_args >= op_array->required_num_args) {
						/* recursive call */
						recursive_call_through_jmp = 1;
					}
				}
			} else {
				if (!trace || (trace->op == ZEND_JIT_TRACE_END
				 && trace->stop == ZEND_JIT_TRACE_STOP_INTERPRETER)) {
					ir_ref ip;

					if (func && zend_accel_in_shm(func->op_array.opcodes)) {
						ip = zend_jit_const_addr(jit, (uintptr_t)func->op_array.opcodes);
					} else {
						if (!func_ref) {
							func_ref = zend_jit_load_at_offset(jit, IR_ADDR,
								rx, offsetof(zend_execute_data, func));
						}
						ip = zend_jit_load_at_offset(jit, IR_ADDR,
							func_ref, offsetof(zend_op_array, opcodes));
					}
					zend_jit_load_ip(jit, ip);
				}
				if (GCC_GLOBAL_REGS) {
					zend_jit_call_0(jit, IR_VOID,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_copy_extra_args_helper, IR_CONST_FASTCALL_FUNC));
				} else {
					zend_jit_call_1(jit, IR_VOID,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_copy_extra_args_helper, IR_CONST_FASTCALL_FUNC),
						zend_jit_fp(jit));
				}
			}
		} else {
			ir_ref ip;

			// JIT: opline = op_array->opcodes
			if (func && zend_accel_in_shm(func->op_array.opcodes)) {
				ip = zend_jit_const_addr(jit, (uintptr_t)func->op_array.opcodes);
			} else {
				if (!func_ref) {
					func_ref = zend_jit_load_at_offset(jit, IR_ADDR,
						rx, offsetof(zend_execute_data, func));
				}
				ip = zend_jit_load_at_offset(jit, IR_ADDR,
					func_ref, offsetof(zend_op_array, opcodes));
			}
			zend_jit_load_ip(jit, ip);

			// JIT: num_args = EX_NUM_ARGS();
			ir_ref num_args, first_extra_arg;

			num_args = zend_jit_load_at_offset(jit, IR_U32,
				zend_jit_fp(jit), offsetof(zend_execute_data, This.u2.num_args));
			if (func) {
				first_extra_arg = ir_const_u32(&jit->ctx, func->op_array.num_args);
			} else {
				// JIT: first_extra_arg = op_array->num_args;
				ZEND_ASSERT(func_ref);
				first_extra_arg = zend_jit_load_at_offset(jit, IR_U32,
					func_ref, offsetof(zend_op_array, num_args));
			}

			// JIT: if (UNEXPECTED(num_args > first_extra_arg))
			ir_ref if_extra_args = zend_jit_if(jit,
				ir_fold2(&jit->ctx, IR_OPT(IR_GT, IR_BOOL), num_args, first_extra_arg));
			zend_jit_if_true_cold(jit, if_extra_args);
			if (GCC_GLOBAL_REGS) {
				zend_jit_call_0(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_copy_extra_args_helper, IR_CONST_FASTCALL_FUNC));
			} else {
				zend_jit_call_1(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_copy_extra_args_helper, IR_CONST_FASTCALL_FUNC),
					zend_jit_fp(jit));
			}
			ir_ref extra_args_path = zend_jit_end(jit);
			zend_jit_if_false(jit, if_extra_args);

			ir_ref type_hints_path = IR_UNUSED;
			if (!func || (func->op_array.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) == 0) {
				if (!func) {
					// JIT: if (EXPECTED((op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS) == 0))
					ir_ref cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
						zend_jit_load_at_offset(jit, IR_U32,
							func_ref, offsetof(zend_op_array, fn_flags)),
						ir_const_u32(&jit->ctx, ZEND_ACC_HAS_TYPE_HINTS));
					ir_ref if_has_type_hints = zend_jit_if(jit, cond);
					zend_jit_if_true(jit, if_has_type_hints);
					type_hints_path = zend_jit_end(jit);
					zend_jit_if_false(jit, if_has_type_hints);
				}
				// JIT: opline += num_args;

				ir_ref ref = ir_fold2(&jit->ctx, IR_OPT(IR_MUL, IR_U32),
					num_args,
					ir_const_i32(&jit->ctx, sizeof(zend_op)));

				if (sizeof(void*) == 8) {
					ref = ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_ADDR), ref);
				}

				if (GCC_GLOBAL_REGS) {
					zend_jit_store_ip(jit,
						ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
							zend_jit_ip(jit),
							ref));
				} else {
					ir_ref addr = zend_jit_ex_opline_addr(jit);

					zend_jit_store(jit,
						addr,
						ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
							zend_jit_load(jit, IR_ADDR, addr),
							ref));
				}
			}

			if (type_hints_path) {
				zend_jit_merge_3(jit, extra_args_path, zend_jit_end(jit), type_hints_path);
			} else {
				zend_jit_merge_2(jit, extra_args_path, zend_jit_end(jit));
			}

			// JIT: if (EXPECTED((int)num_args < op_array->last_var)) {
			ir_ref last_var;

			if (func) {
				last_var = ir_const_u32(&jit->ctx, func->op_array.last_var);
			} else {
				ZEND_ASSERT(func_ref);
				last_var = zend_jit_load_at_offset(jit, IR_U32,
					func_ref, offsetof(zend_op_array, last_var));
			}

			ir_ref if_need = zend_jit_if(jit,
				ir_fold2(&jit->ctx, IR_OPT(IR_LT, IR_U32), num_args, last_var));
			zend_jit_if_true(jit, if_need);

			ir_ref idx = ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_U32), last_var, num_args);

			// JIT: zval *var = EX_VAR_NUM(num_args);
			if (sizeof(void*) == 8) {
				num_args = ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_ADDR), num_args);
			}
			ir_ref var_ref = zend_jit_addr_offset(jit,
				ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
						zend_jit_fp(jit),
					ir_fold2(&jit->ctx, IR_OPT(IR_MUL, IR_ADDR),
						num_args,
						zend_jit_const_addr(jit, sizeof(zval)))),
				(ZEND_CALL_FRAME_SLOT * sizeof(zval)) + offsetof(zval, u1.type_info));

			ir_ref loop = zend_jit_loop_begin(jit, zend_jit_end(jit));
			var_ref = zend_jit_phi_2(jit, var_ref, IR_UNUSED);
			idx = zend_jit_phi_2(jit, idx, IR_UNUSED);
			zend_jit_store(jit,
				var_ref,
				ir_const_i32(&jit->ctx, IS_UNDEF));
			ir_set_op(&jit->ctx, var_ref, 3,
				zend_jit_addr_offset(jit, var_ref, sizeof(zval)));
			ir_ref idx2 = ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_U32),
				idx,
				ir_const_u32(&jit->ctx, 1));
			ir_set_op(&jit->ctx, idx, 3, idx2);
			ir_ref if_not_zero = zend_jit_if(jit, idx2);
			zend_jit_if_true(jit, if_not_zero);
			ir_set_op(&jit->ctx, loop, 2, zend_jit_loop_end(jit, loop));
			zend_jit_if_false(jit, if_not_zero);

			ir_ref long_path = zend_jit_end(jit);
			zend_jit_if_false(jit, if_need);
			zend_jit_merge_2(jit, long_path, zend_jit_end(jit));
		}

		if (ZEND_OBSERVER_ENABLED) {
			if (GCC_GLOBAL_REGS) {
				// EX(opline) = opline
				zend_jit_store(jit,
					zend_jit_ex_opline_addr(jit),
					zend_jit_ip(jit));
			}
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_observer_fcall_begin, IR_CONST_FASTCALL_FUNC),
				zend_jit_fp(jit));
		}

		if (trace) {
			if (!func && (opline->opcode != ZEND_DO_UCALL)) {
				user_path = zend_jit_end(jit);
			}
		} else {
			if (GCC_GLOBAL_REGS) {
				if (recursive_call_through_jmp) {
					// TODO: use direct jmp ???
					zend_jit_tailcall_0(jit,
						zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));
//???+++				|	jmp =>num_args
				} else {
					zend_jit_tailcall_0(jit,
						zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));
				}
			} else {
				zend_jit_ret(jit, ir_const_i32(&jit->ctx, 1));
			}
			if (func || (opline->opcode == ZEND_DO_UCALL)) {
				if (jit->b >= 0) {
					zend_basic_block *bb = &jit->ssa->cfg.blocks[jit->b];

					if (bb->successors_count > 0) {
						int succ;

						ZEND_ASSERT(bb->successors_count == 1);
						succ = bb->successors[0];
						_zend_jit_add_predecessor_ref(jit, succ, jit->b, jit->control);
					}
					jit->control = IR_UNUSED;
					jit->b = -1;
				}
			}
		}
	}

	if ((!func || func->type == ZEND_INTERNAL_FUNCTION)
	 && (opline->opcode != ZEND_DO_UCALL)) {
		if (!func && (opline->opcode != ZEND_DO_ICALL)) {
			zend_jit_if_false(jit, if_user);
		}
		if (opline->opcode == ZEND_DO_FCALL_BY_NAME) {
			if (!func) {
				if (trace) {
					uint32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);

					exit_addr = zend_jit_trace_get_exit_addr(exit_point);
					if (!exit_addr) {
						return 0;
					}
					ZEND_ASSERT(func_ref);
					zend_jit_guard_not(jit,
						ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
							zend_jit_load_at_offset(jit, IR_U32,
								func_ref, offsetof(zend_op_array, fn_flags)),
							ir_const_u32(&jit->ctx, ZEND_ACC_DEPRECATED)),
						zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				} else {
					ir_ref cond, if_deprecated, cold_path, ret;

					cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
						zend_jit_load_at_offset(jit, IR_U32,
							func_ref, offsetof(zend_op_array, fn_flags)),
						ir_const_u32(&jit->ctx, ZEND_ACC_DEPRECATED));
					if_deprecated = zend_jit_if(jit, cond);
					zend_jit_if_true_cold(jit, if_deprecated);

					if (GCC_GLOBAL_REGS) {
						ret = zend_jit_call_0(jit, IR_BOOL,
							zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_deprecated_helper, IR_CONST_FASTCALL_FUNC));
					} else {
						ret = zend_jit_call_1(jit, IR_BOOL,
							zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_deprecated_helper, IR_CONST_FASTCALL_FUNC),
							rx);
					}
					zend_jit_guard(jit, ret, zend_jit_stub_addr(jit, jit_stub_exception_handler));

					cold_path = zend_jit_end(jit);
					zend_jit_if_false(jit, if_deprecated);
					zend_jit_merge_2(jit, cold_path, zend_jit_end(jit));
				}
			} else if (func->common.fn_flags & ZEND_ACC_DEPRECATED) {
				ir_ref ret;

				if (GCC_GLOBAL_REGS) {
					ret = zend_jit_call_0(jit, IR_BOOL,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_deprecated_helper, IR_CONST_FASTCALL_FUNC));
				} else {
					ret = zend_jit_call_1(jit, IR_BOOL,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_deprecated_helper, IR_CONST_FASTCALL_FUNC),
						rx);
				}
				zend_jit_guard(jit, ret, zend_jit_stub_addr(jit, jit_stub_exception_handler));
			}
		}

		// JIT: EG(current_execute_data) = execute_data;
		zend_jit_store(jit,
			ZEND_JIT_EG_ADDR(current_execute_data),
			rx);

		if (ZEND_OBSERVER_ENABLED) {
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_observer_fcall_begin, IR_CONST_FASTCALL_FUNC),
				rx);
		}

		// JIT: ZVAL_NULL(EX_VAR(opline->result.var));
		ir_ref res_addr = IR_UNUSED, func_ptr;

		if (RETURN_VALUE_USED(opline)) {
			res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var);
		} else {
			/* CPU stack allocated temporary zval */
			// JIT: alloca(sizeof(void*));
			ir_ref ptr = zend_jit_alloca(jit, zend_jit_const_addr(jit, sizeof(zval)));
			res_addr = ZEND_ADDR_REF_ZVAL(ptr);
		}

		zend_jit_zval_set_type_info(jit, res_addr, IS_NULL);

		zend_jit_reset_last_valid_opline(jit);

		// JIT: fbc->internal_function.handler(call, ret);
		if (func) {
			func_ptr = zend_jit_const_func_addr(jit, (uintptr_t)func->internal_function.handler, IR_CONST_FASTCALL_FUNC);
		} else {
			func_ptr = zend_jit_load_at_offset(jit, IR_ADDR,
				func_ref, offsetof(zend_internal_function, handler));
#if defined(IR_TARGET_X86)
			func_ptr =  ir_fold2(&jit->ctx, IR_OPT(IR_BITCAST, IR_ADDR),
				func_ptr, IR_CONST_FASTCALL_FUNC); // Hack to use fastcall calling convention ???
#endif
		}
		zend_jit_call_2(jit, IR_VOID, func_ptr, rx, zend_jit_zval_addr(jit, res_addr));

		if (ZEND_OBSERVER_ENABLED) {
			zend_jit_call_2(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_observer_fcall_end, IR_CONST_FASTCALL_FUNC),
				rx,
				zend_jit_zval_addr(jit, res_addr));
		}

		// JIT: EG(current_execute_data) = execute_data;
		zend_jit_store(jit,
			ZEND_JIT_EG_ADDR(current_execute_data),
			zend_jit_fp(jit));

		// JIT: zend_vm_stack_free_args(call);
		if (func && !unknown_num_args) {
			for (i = 0; i < call_num_args; i++ ) {
				if (zend_jit_needs_arg_dtor(func, i, call_info)) {
					uint32_t offset = EX_NUM_TO_VAR(i);
					zend_jit_addr var_addr = ZEND_ADDR_MEM_ZVAL(ZREG_RX, offset);

					zend_jit_zval_ptr_dtor(jit, var_addr, MAY_BE_ANY|MAY_BE_RC1|MAY_BE_RCN, 0, opline);
				}
			}
		} else {
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_vm_stack_free_args_helper, IR_CONST_FASTCALL_FUNC),
				rx);
		}

		if (may_have_extra_named_params) {
			ir_ref cond, if_has_named, named_path;

			// JIT: if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS))
			cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U8),
				zend_jit_load_at_offset(jit, IR_U8,
					rx, offsetof(zend_execute_data, This.u1.type_info) + 3),
				ir_const_u8(&jit->ctx, ZEND_CALL_HAS_EXTRA_NAMED_PARAMS >> 24));
			if_has_named = zend_jit_if(jit, cond);
			zend_jit_if_true_cold(jit, if_has_named);

			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_free_extra_named_params, IR_CONST_FASTCALL_FUNC),
				zend_jit_load_at_offset(jit, IR_ADDR,
					rx, offsetof(zend_execute_data, extra_named_params)));

			named_path = zend_jit_end(jit);
			zend_jit_if_false(jit, if_has_named);
			zend_jit_merge_2(jit, named_path, zend_jit_end(jit));
		}

		if (opline->opcode == ZEND_DO_FCALL) {
			ir_ref cond, if_release_this, release_path;

			// TODO: optimize ???
			// JIT: if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_RELEASE_THIS))
			cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U8),
				zend_jit_load_at_offset(jit, IR_U8,
					rx, offsetof(zend_execute_data, This.u1.type_info) + 2),
				ir_const_u8(&jit->ctx, ZEND_CALL_RELEASE_THIS >> 16));
			if_release_this = zend_jit_if(jit, cond);
			zend_jit_if_true_cold(jit, if_release_this);

			// JIT: OBJ_RELEASE(Z_OBJ(RX->This));
			zend_jit_obj_release(jit,
				zend_jit_zval_ref_ptr(jit,
					zend_jit_addr_offset(jit, rx, offsetof(zend_execute_data, This))));

			release_path = zend_jit_end(jit);
			zend_jit_if_false(jit, if_release_this);
			zend_jit_merge_2(jit, release_path, zend_jit_end(jit));
		}


		ir_ref allocated_path = IR_UNUSED;

		if (JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE ||
		    !JIT_G(current_frame) ||
		    !JIT_G(current_frame)->call ||
		    !TRACE_FRAME_IS_NESTED(JIT_G(current_frame)->call) ||
		    prev_opline->opcode == ZEND_SEND_UNPACK ||
		    prev_opline->opcode == ZEND_SEND_ARRAY ||
			prev_opline->opcode == ZEND_CHECK_UNDEF_ARGS) {

			ir_ref cond, if_allocated;

			// JIT: zend_vm_stack_free_call_frame(call);
			// JIT: if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_ALLOCATED))
			cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U8),
				zend_jit_load_at_offset(jit, IR_U8,
					rx, offsetof(zend_execute_data, This.u1.type_info) + 2),
				ir_const_u8(&jit->ctx, ZEND_CALL_ALLOCATED >> 16));
			if_allocated = zend_jit_if(jit, cond);
			zend_jit_if_true_cold(jit, if_allocated);

			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_free_call_frame, IR_CONST_FASTCALL_FUNC),
				rx);

			allocated_path = zend_jit_end(jit);
			zend_jit_if_false(jit, if_allocated);
		}

		zend_jit_store(jit,
			ZEND_JIT_EG_ADDR(vm_stack_top),
			rx);

		if (allocated_path) {
			zend_jit_merge_2(jit, allocated_path, zend_jit_end(jit));
		}

		if (!RETURN_VALUE_USED(opline)) {
			zend_class_entry *ce;
			bool ce_is_instanceof;
			uint32_t func_info = call_info ?
				zend_get_func_info(call_info, ssa, &ce, &ce_is_instanceof) :
				(MAY_BE_ANY|MAY_BE_REF|MAY_BE_RC1|MAY_BE_RCN);

			/* If an exception is thrown, the return_value may stay at the
			 * original value of null. */
			func_info |= MAY_BE_NULL;

			if (func_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF)) {
				ir_ref sp = zend_jit_rload(jit, IR_ADDR, IR_REG_SP);
				res_addr = ZEND_ADDR_REF_ZVAL(sp);
				zend_jit_zval_ptr_dtor(jit, res_addr, func_info, 1, opline);
			}
			// JIT: revert alloca
			zend_jit_afree(jit, zend_jit_const_addr(jit, sizeof(zval)));
		}

		// JIT: if (UNEXPECTED(EG(exception) != NULL)) {
		zend_jit_guard_not(jit,
			zend_jit_load(jit, IR_ADDR,
				zend_jit_eg_exception_addr(jit)),
			zend_jit_stub_addr(jit, jit_stub_icall_throw));

		// TODO: Can we avoid checking for interrupts after each call ???
		if (trace && jit->last_valid_opline != opline) {
			int32_t exit_point = zend_jit_trace_get_exit_point(opline + 1, ZEND_JIT_EXIT_TO_VM);

			exit_addr = zend_jit_trace_get_exit_addr(exit_point);
			if (!exit_addr) {
				return 0;
			}
		} else {
			exit_addr = NULL;
		}

		if (!zend_jit_check_timeout(jit, opline + 1, exit_addr)) {
			return 0;
		}

		if ((!trace || !func) && opline->opcode != ZEND_DO_ICALL) {
			zend_jit_load_ip_addr(jit, opline + 1);
		} else if (trace
		 && trace->op == ZEND_JIT_TRACE_END
		 && trace->stop == ZEND_JIT_TRACE_STOP_INTERPRETER) {
			zend_jit_load_ip_addr(jit, opline + 1);
		}
	}

	if (user_path) {
		zend_jit_merge_2(jit, user_path, zend_jit_end(jit));
	}

	return 1;
}

static int zend_jit_constructor(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, zend_ssa *ssa, int call_level, int next_block)
{
	ir_ref if_skip_constructor = zend_jit_if_ex(jit, zend_jit_cmp_ip(jit, IR_NE, opline), next_block);

	zend_jit_if_false(jit, if_skip_constructor);

	if (JIT_G(opt_level) < ZEND_JIT_LEVEL_INLINE) {
		if (!zend_jit_tail_handler(jit, opline)) {
			return 0;
		}
	} else {
		if (!zend_jit_do_fcall(jit, opline, op_array, ssa, call_level, next_block, NULL)) {
			return 0;
		}
	}

    /* override predecessors of the next block */
	ZEND_ASSERT(jit->ssa->cfg.blocks[next_block].predecessors_count == 1);
    if (!jit->control) {
		jit->bb_edges[jit->bb_predecessors[next_block]] = if_skip_constructor;
	} else {
		/* merge current control path with the true branch of constructor skip condition */
		ir_ref end = zend_jit_end(jit);

		zend_jit_if_true(jit, if_skip_constructor);
		zend_jit_merge_2(jit, end, zend_jit_end(jit));
		jit->bb_edges[jit->bb_predecessors[next_block]] = zend_jit_end(jit);

		jit->control = IR_UNUSED;
		jit->b = -1;
		zend_jit_reset_last_valid_opline(jit);
	}

	return 1;
}

static int zend_jit_verify_arg_type(zend_jit_ctx *jit, const zend_op *opline, zend_arg_info *arg_info, bool check_exception)
{
	zend_jit_addr res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var);
	uint32_t type_mask = ZEND_TYPE_PURE_MASK(arg_info->type) & MAY_BE_ANY;
	ir_ref ref, fast_path = IR_UNUSED;

	ref = zend_jit_zval_addr(jit, res_addr);
	if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE
	 && JIT_G(current_frame)
	 && JIT_G(current_frame)->prev) {
		zend_jit_trace_stack *stack = JIT_G(current_frame)->stack;
		uint8_t type = STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var));

		if (type != IS_UNKNOWN && (type_mask & (1u << type))) {
			return 1;
		}
	}

	if (ZEND_ARG_SEND_MODE(arg_info)) {
		if (opline->opcode == ZEND_RECV_INIT) {
			ref = zend_jit_zval_ref_deref(jit, ref);
		} else {
			ref = zend_jit_zval_ref_ptr(jit, ref);
			ref = zend_jit_addr_offset(jit, ref, offsetof(zend_reference, val));
		}
	}

	if (type_mask != 0) {
		if (is_power_of_two(type_mask)) {
			uint32_t type_code = concrete_type(type_mask);
			ir_ref if_ok = zend_jit_if_zval_ref_type(jit, ref, ir_const_u8(&jit->ctx, type_code));
			zend_jit_if_true(jit, if_ok);
			fast_path = zend_jit_end(jit);
			zend_jit_if_false_cold(jit, if_ok);
		} else {
			ir_ref cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
				ir_fold2(&jit->ctx, IR_OPT(IR_SHL, IR_U32),
					ir_const_u32(&jit->ctx, 1),
					zend_jit_zval_ref_type(jit, ref)),
				ir_const_u32(&jit->ctx, type_mask));
			ir_ref if_ok = zend_jit_if(jit, cond);
			zend_jit_if_true(jit, if_ok);
			fast_path = zend_jit_end(jit);
			zend_jit_if_false_cold(jit, if_ok);
		}
	}

//	if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
		zend_jit_set_ex_opline(jit, opline);
//	} else {
//???		|	ADDR_STORE aword EX->opline, opline, r0
//	}
	ref = zend_jit_call_2(jit, IR_BOOL,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_verify_arg_slow, IR_CONST_FASTCALL_FUNC),
		ref,
		zend_jit_const_addr(jit, (uintptr_t)arg_info));

	if (check_exception) {
		zend_jit_guard(jit, ref, zend_jit_stub_addr(jit, jit_stub_exception_handler));
	}

	if (fast_path) {
		zend_jit_merge_2(jit, fast_path, zend_jit_end(jit));
	}

	return 1;
}

static int zend_jit_recv(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array)
{
	uint32_t arg_num = opline->op1.num;
	zend_arg_info *arg_info = NULL;

	if (op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS) {
		if (EXPECTED(arg_num <= op_array->num_args)) {
			arg_info = &op_array->arg_info[arg_num-1];
		} else if (UNEXPECTED(op_array->fn_flags & ZEND_ACC_VARIADIC)) {
			arg_info = &op_array->arg_info[op_array->num_args];
		}
		if (arg_info && !ZEND_TYPE_IS_SET(arg_info->type)) {
			arg_info = NULL;
		}
	}

	if (arg_info || (opline+1)->opcode != ZEND_RECV) {
		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
			if (!JIT_G(current_frame) ||
			    TRACE_FRAME_NUM_ARGS(JIT_G(current_frame)) < 0 ||
			    arg_num > TRACE_FRAME_NUM_ARGS(JIT_G(current_frame))) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				zend_jit_guard(jit,
					ir_fold2(&jit->ctx, IR_OPT(IR_GE, IR_BOOL),
						zend_jit_load_at_offset(jit, IR_U32,
							zend_jit_fp(jit), offsetof(zend_execute_data, This.u2.num_args)),
						ir_const_u32(&jit->ctx, arg_num)),
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			}
		} else {
			ir_ref cond, if_ok;

			cond = ir_fold2(&jit->ctx, IR_OPT(IR_GE, IR_BOOL),
				zend_jit_load_at_offset(jit, IR_U32,
					zend_jit_fp(jit), offsetof(zend_execute_data, This.u2.num_args)),
				ir_const_u32(&jit->ctx, arg_num));
			if_ok = zend_jit_if(jit, cond);
			zend_jit_if_false_cold(jit, if_ok);

//			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				zend_jit_set_ex_opline(jit, opline);
//			} else {
//???				|	ADDR_STORE aword EX->opline, opline, r0
//			}
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_missing_arg_error, IR_CONST_FASTCALL_FUNC),
				zend_jit_fp(jit));
			zend_jit_ijmp(jit, zend_jit_stub_addr(jit, jit_stub_exception_handler));
			zend_jit_if_true(jit, if_ok);
		}
	}

	if (arg_info) {
		if (!zend_jit_verify_arg_type(jit, opline, arg_info, 1)) {
			return 0;
		}
	}

	if (JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE) {
		if ((opline+1)->opcode != ZEND_RECV && (opline+1)->opcode != ZEND_RECV_INIT) {
			zend_jit_load_ip_addr(jit, opline + 1);
			zend_jit_set_last_valid_opline(jit, opline + 1);
		}
	}

	return 1;
}

static int zend_jit_recv_init(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, bool is_last, int may_throw)
{
	uint32_t arg_num = opline->op1.num;
	zval *zv = RT_CONSTANT(opline, opline->op2);
	zend_jit_addr res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var);
	ir_ref ref, if_fail, skip_path = IR_UNUSED;

	if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE
	 && JIT_G(current_frame)
	 && TRACE_FRAME_NUM_ARGS(JIT_G(current_frame)) >= 0) {
		if (arg_num > TRACE_FRAME_NUM_ARGS(JIT_G(current_frame))) {
			zend_jit_copy_zval_const(jit,
				res_addr,
				-1, -1,
				zv, 1);
		}
	} else {
		if (JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE ||
		    (op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
			ir_ref cond, if_skip;

			cond = ir_fold2(&jit->ctx, IR_OPT(IR_GE, IR_BOOL),
				zend_jit_load_at_offset(jit, IR_U32,
					zend_jit_fp(jit), offsetof(zend_execute_data, This.u2.num_args)),
				ir_const_u32(&jit->ctx, arg_num));
			if_skip = zend_jit_if(jit, cond);
			zend_jit_if_true(jit, if_skip);
			skip_path = zend_jit_end(jit);
			zend_jit_if_false(jit, if_skip);
		}
			zend_jit_copy_zval_const(jit,
				res_addr,
				-1, -1,
				zv, 1);
	}

	if (Z_CONSTANT_P(zv)) {
//		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
			zend_jit_set_ex_opline(jit, opline);
//???-			|	SET_EX_OPLINE opline, r0
//		} else {
//???			|	ADDR_STORE aword EX->opline, opline, r0
//		}

		ref = zend_jit_call_2(jit, IR_I32,
			zend_jit_const_func_addr(jit, (uintptr_t)zval_update_constant_ex, IR_CONST_FASTCALL_FUNC),
			zend_jit_zval_addr(jit, res_addr),
			zend_jit_load_at_offset(jit, IR_ADDR,
				zend_jit_load_at_offset(jit, IR_ADDR,
					zend_jit_fp(jit), offsetof(zend_execute_data, func)),
				offsetof(zend_op_array, scope)));

		if_fail = zend_jit_if(jit, ref);
		zend_jit_if_true_cold(jit, if_fail);
		zend_jit_zval_ptr_dtor(jit, res_addr, MAY_BE_ANY|MAY_BE_RC1|MAY_BE_RCN, 1, opline);
		zend_jit_ijmp(jit, zend_jit_stub_addr(jit, jit_stub_exception_handler));
		zend_jit_if_false(jit, if_fail);
	}

	if (skip_path) {
		zend_jit_merge_2(jit, skip_path, zend_jit_end(jit));
	}

	if (op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS) {
		do {
			zend_arg_info *arg_info;

			if (arg_num <= op_array->num_args) {
				arg_info = &op_array->arg_info[arg_num-1];
			} else if (op_array->fn_flags & ZEND_ACC_VARIADIC) {
				arg_info = &op_array->arg_info[op_array->num_args];
			} else {
				break;
			}
			if (!ZEND_TYPE_IS_SET(arg_info->type)) {
				break;
			}
			if (!zend_jit_verify_arg_type(jit, opline, arg_info, may_throw)) {
				return 0;
			}
		} while (0);
	}

	if (JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE) {
		if (is_last) {
			zend_jit_load_ip_addr(jit, opline + 1);
			zend_jit_set_last_valid_opline(jit, opline + 1);
		}
	}

	return 1;
}

static bool zend_jit_verify_return_type(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, uint32_t op1_info)
{
	zend_arg_info *arg_info = &op_array->arg_info[-1];
	ZEND_ASSERT(ZEND_TYPE_IS_SET(arg_info->type));
	zend_jit_addr op1_addr = OP1_ADDR();
	bool needs_slow_check = 1;
	uint32_t type_mask = ZEND_TYPE_PURE_MASK(arg_info->type) & MAY_BE_ANY;
	ir_ref fast_path = IR_UNUSED;

	if (type_mask != 0) {
		if (((op1_info & MAY_BE_ANY) & type_mask) == 0) {
			/* pass */
		} else if (((op1_info & MAY_BE_ANY) | type_mask) == type_mask) {
			needs_slow_check = 0;
		} else if (is_power_of_two(type_mask)) {
			uint32_t type_code = concrete_type(type_mask);
			ir_ref if_ok = zend_jit_if_zval_type(jit, op1_addr, type_code);

			zend_jit_if_true(jit, if_ok);
			fast_path = zend_jit_end(jit);
			zend_jit_if_false_cold(jit, if_ok);
		} else {
			ir_ref cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
				ir_fold2(&jit->ctx, IR_OPT(IR_SHL, IR_U32),
					ir_const_u32(&jit->ctx, 1),
					zend_jit_zval_type(jit, op1_addr)),
				ir_const_u32(&jit->ctx, type_mask));
			ir_ref if_ok = zend_jit_if(jit, cond);

			zend_jit_if_true(jit, if_ok);
			fast_path = zend_jit_end(jit);
			zend_jit_if_false_cold(jit, if_ok);
		}
	}
	if (needs_slow_check) {
		ir_ref ref;

		zend_jit_set_ex_opline(jit, opline);
		ref = zend_jit_zval_addr(jit, op1_addr);
		if (op1_info & MAY_BE_UNDEF) {
			ref = zend_jit_zval_check_undef(jit, ref, opline->op1.var, NULL, 1);
		}

		zend_jit_call_4(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_verify_return_slow, IR_CONST_FASTCALL_FUNC),
			ref,
			zend_jit_load_at_offset(jit, IR_ADDR,
				zend_jit_fp(jit), offsetof(zend_execute_data, func)),
			zend_jit_const_addr(jit, (uintptr_t)arg_info),
			zend_jit_addr_offset(jit,
				zend_jit_load_at_offset(jit, IR_ADDR,
					zend_jit_fp(jit), offsetof(zend_execute_data, run_time_cache)),
				opline->op2.num));

		zend_jit_check_exception(jit);

		if (fast_path) {
			zend_jit_merge_2(jit, fast_path, zend_jit_end(jit));
		}
	}

	return 1;
}

static int zend_jit_leave_frame(zend_jit_ctx *jit)
{
	// JIT: EG(current_execute_data) = EX(prev_execute_data);
	zend_jit_store(jit,
		ZEND_JIT_EG_ADDR(current_execute_data),
		zend_jit_load_at_offset(jit, IR_ADDR,
			zend_jit_fp(jit), offsetof(zend_execute_data, prev_execute_data)));
	return 1;
}

static int zend_jit_free_cvs(zend_jit_ctx *jit)
{
	// JIT: EG(current_execute_data) = EX(prev_execute_data);
	zend_jit_store(jit,
		ZEND_JIT_EG_ADDR(current_execute_data),
		zend_jit_load_at_offset(jit, IR_ADDR,
			zend_jit_fp(jit), offsetof(zend_execute_data, prev_execute_data)));

	// JIT: zend_free_compiled_variables(execute_data);
	zend_jit_call_1(jit, IR_VOID,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_free_compiled_variables, IR_CONST_FASTCALL_FUNC),
		zend_jit_fp(jit));
	return 1;
}

static int zend_jit_free_cv(zend_jit_ctx *jit, uint32_t info, uint32_t var)
{
	if (info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF)) {
		zend_jit_addr var_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, EX_NUM_TO_VAR(var));

		zend_jit_zval_ptr_dtor(jit, var_addr, info, 1, NULL);
	}
	return 1;
}

static int zend_jit_leave_func(zend_jit_ctx         *jit,
                               const zend_op_array  *op_array,
                               const zend_op        *opline,
                               uint32_t              op1_info,
                               bool             left_frame,
                               zend_jit_trace_rec   *trace,
                               zend_jit_trace_info  *trace_info,
                               int                   indirect_var_access,
                               int                   may_throw)
{
	bool may_be_top_frame =
		JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE ||
		!JIT_G(current_frame) ||
		!TRACE_FRAME_IS_NESTED(JIT_G(current_frame));
	bool may_need_call_helper =
		indirect_var_access || /* may have symbol table */
		!op_array->function_name || /* may have symbol table */
		may_be_top_frame ||
		(op_array->fn_flags & ZEND_ACC_VARIADIC) || /* may have extra named args */
		JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE ||
		!JIT_G(current_frame) ||
		TRACE_FRAME_NUM_ARGS(JIT_G(current_frame)) == -1 || /* unknown number of args */
		(uint32_t)TRACE_FRAME_NUM_ARGS(JIT_G(current_frame)) > op_array->num_args; /* extra args */
	bool may_need_release_this =
		!(op_array->fn_flags & ZEND_ACC_CLOSURE) &&
		op_array->scope &&
		!(op_array->fn_flags & ZEND_ACC_STATIC) &&
		(JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE ||
		 !JIT_G(current_frame) ||
		 !TRACE_FRAME_NO_NEED_RELEASE_THIS(JIT_G(current_frame)));
	ir_ref call_info, ref, cold_path = IR_UNUSED;

	call_info = zend_jit_load_at_offset(jit, IR_U32,
		zend_jit_fp(jit), offsetof(zend_execute_data, This.u1.type_info));
	if (may_need_call_helper) {
		if (!left_frame) {
			left_frame = 1;
		    if (!zend_jit_leave_frame(jit)) {
				return 0;
		    }
		}
		/* ZEND_CALL_FAKE_CLOSURE handled on slow path to eliminate check for ZEND_CALL_CLOSURE on fast path */
		ref = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
			call_info,
			ir_const_u32(&jit->ctx, ZEND_CALL_TOP|ZEND_CALL_HAS_SYMBOL_TABLE|ZEND_CALL_FREE_EXTRA_ARGS|ZEND_CALL_ALLOCATED|ZEND_CALL_HAS_EXTRA_NAMED_PARAMS|ZEND_CALL_FAKE_CLOSURE));
		if (trace && trace->op != ZEND_JIT_TRACE_END) {
			ir_ref if_slow = zend_jit_if(jit, ref);

			zend_jit_if_true_cold(jit, if_slow);
			if (!GCC_GLOBAL_REGS) {
				ref = zend_jit_call_1(jit, IR_I32,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_leave_func_helper, IR_CONST_FASTCALL_FUNC),
					zend_jit_fp(jit));
			} else {
				zend_jit_call_0(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_leave_func_helper, IR_CONST_FASTCALL_FUNC));
			}

			if (may_be_top_frame) {
				// TODO: try to avoid this check ???
				if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
#if 0
					/* this check should be handled by the following OPLINE guard */
					|	cmp IP, zend_jit_halt_op
					|	je ->trace_halt
#endif
				} else if (GCC_GLOBAL_REGS) {
					zend_jit_guard(jit,
						zend_jit_ip(jit),
						zend_jit_stub_addr(jit, jit_stub_trace_halt));
				} else {
					zend_jit_guard(jit,
						ir_fold2(&jit->ctx, IR_OPT(IR_GE, IR_BOOL),
							ref,
							ir_const_i32(&jit->ctx, 0)),
						zend_jit_stub_addr(jit, jit_stub_trace_halt));
				}
			}

			if (!GCC_GLOBAL_REGS) {
				// execute_data = EG(current_execute_data)
				zend_jit_store_fp(jit,
					zend_jit_load(jit, IR_ADDR,
						ZEND_JIT_EG_ADDR(current_execute_data)));
			}
			cold_path = zend_jit_end(jit);
			zend_jit_if_false(jit, if_slow);
		} else {
			zend_jit_guard_not(jit, ref,
				zend_jit_stub_addr(jit, jit_stub_leave_function_handler));
		}
	}

	if (op_array->fn_flags & ZEND_ACC_CLOSURE) {
		if (!left_frame) {
			left_frame = 1;
		    if (!zend_jit_leave_frame(jit)) {
				return 0;
		    }
		}
		// JIT: OBJ_RELEASE(ZEND_CLOSURE_OBJECT(EX(func)));
		zend_jit_obj_release(jit,
			zend_jit_addr_offset(jit,
				zend_jit_load_at_offset(jit, IR_ADDR,
					zend_jit_fp(jit), offsetof(zend_execute_data, func)),
				-sizeof(zend_object)));
	} else if (may_need_release_this) {
		ir_ref if_release, fast_path = IR_UNUSED;

		if (!left_frame) {
			left_frame = 1;
		    if (!zend_jit_leave_frame(jit)) {
				return 0;
		    }
		}
		if (!JIT_G(current_frame) || !TRACE_FRAME_ALWAYS_RELEASE_THIS(JIT_G(current_frame))) {
			// JIT: if (call_info & ZEND_CALL_RELEASE_THIS)
			if_release = zend_jit_if(jit,
				ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
					call_info,
					ir_const_u32(&jit->ctx, ZEND_CALL_RELEASE_THIS)));
			zend_jit_if_false(jit, if_release);
			fast_path = zend_jit_end(jit);
			zend_jit_if_true(jit, if_release);
		}
		// JIT: OBJ_RELEASE(execute_data->This))
		zend_jit_obj_release(jit,
			zend_jit_load_at_offset(jit, IR_ADDR,
				zend_jit_fp(jit), offsetof(zend_execute_data, This.value.obj)));
		if (fast_path) {
			zend_jit_merge_2(jit, fast_path, zend_jit_end(jit));
		}
		// TODO: avoid EG(excption) check for $this->foo() calls
		may_throw = 1;
	}

	// JIT: EG(vm_stack_top) = (zval*)execute_data
	zend_jit_store(jit,
		ZEND_JIT_EG_ADDR(vm_stack_top),
		zend_jit_fp(jit));

	// JITL execute_data = EX(prev_execute_data)
	zend_jit_store_fp(jit,
		zend_jit_load_at_offset(jit, IR_ADDR,
			zend_jit_fp(jit), offsetof(zend_execute_data, prev_execute_data)));

	if (!left_frame) {
		// JIT: EG(current_execute_data) = execute_data
		zend_jit_store(jit,
			ZEND_JIT_EG_ADDR(current_execute_data),
			zend_jit_fp(jit));
	}

	if (trace) {
		if (trace->op != ZEND_JIT_TRACE_END
		 && (JIT_G(current_frame) && !TRACE_FRAME_IS_UNKNOWN_RETURN(JIT_G(current_frame)))) {
			zend_jit_reset_last_valid_opline(jit);
		} else {
			if (GCC_GLOBAL_REGS) {
#if 1
				zend_jit_store_ip(jit,
					zend_jit_load(jit, IR_ADDR,
						zend_jit_ex_opline_addr(jit)));
				zend_jit_store_ip(jit,
					zend_jit_addr_offset(jit, zend_jit_ip(jit), sizeof(zend_op)));
#else
				// TODO: this IR produces worse code on x86, becuase of missing fusion ???
				zend_jit_store_ip(jit,
					zend_jit_addr_offset(jit,
						zend_jit_load(jit, IR_ADDR,
							zend_jit_ex_opline_addr(jit)),
						sizeof(zend_op)));
#endif
			} else {
				ir_ref ref = zend_jit_ex_opline_addr(jit);

				zend_jit_store(jit,
					ref,
					zend_jit_addr_offset(jit, zend_jit_load(jit, IR_ADDR, ref), sizeof(zend_op)));
			}
		}

		if (cold_path) {
			zend_jit_merge_2(jit, zend_jit_end(jit), cold_path);
		}

		if (trace->op == ZEND_JIT_TRACE_BACK
		 && (!JIT_G(current_frame) || TRACE_FRAME_IS_UNKNOWN_RETURN(JIT_G(current_frame)))) {
			const zend_op *next_opline = trace->opline;

			if ((opline->op1_type & (IS_VAR|IS_TMP_VAR))
			 && (op1_info & MAY_BE_RC1)
			 && (op1_info & (MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_ARRAY))) {
				/* exception might be thrown during destruction of unused return value */
				// JIT: if (EG(exception))
				zend_jit_guard_not(jit,
					zend_jit_load(jit, IR_ADDR, ZEND_JIT_EG_ADDR(exception)),
					zend_jit_stub_addr(jit, jit_stub_leave_throw));
			}
			do {
				trace++;
			} while (trace->op == ZEND_JIT_TRACE_INIT_CALL);
			ZEND_ASSERT(trace->op == ZEND_JIT_TRACE_VM || trace->op == ZEND_JIT_TRACE_END);
			next_opline = trace->opline;
			ZEND_ASSERT(next_opline != NULL);

			if (trace->op == ZEND_JIT_TRACE_END
			 && trace->stop == ZEND_JIT_TRACE_STOP_RECURSIVE_RET) {
				trace_info->flags |= ZEND_JIT_TRACE_LOOP;
					ir_ref ref, if_eq;

				ref = zend_jit_cmp_ip(jit, IR_EQ, next_opline);
				if_eq = zend_jit_if(jit, ref);
				zend_jit_if_true(jit, if_eq);
				ZEND_ASSERT(jit->trace_loop_ref);
				ZEND_ASSERT(jit->ctx.ir_base[jit->trace_loop_ref].op2 == IR_UNUSED);
				ir_set_op(&jit->ctx, jit->trace_loop_ref, 2, zend_jit_end(jit));
				zend_jit_if_false(jit, if_eq);

#ifdef ZEND_VM_HYBRID_JIT_RED_ZONE_SIZE
				zend_jit_tailcall_0(jit,
					zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));
#else
				zend_jit_ijmp(jit, zend_jit_stub_addr(jit, jit_stub_trace_escape));
#endif
			} else {
				zend_jit_guard(jit,
					zend_jit_cmp_ip(jit, IR_EQ, next_opline),
					zend_jit_stub_addr(jit, jit_stub_trace_escape));
			}

			zend_jit_set_last_valid_opline(jit, trace->opline);

			return 1;
		} else if (may_throw ||
				(((opline->op1_type & (IS_VAR|IS_TMP_VAR))
				  && (op1_info & MAY_BE_RC1)
				  && (op1_info & (MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_ARRAY)))
				 && (!JIT_G(current_frame) || TRACE_FRAME_IS_RETURN_VALUE_UNUSED(JIT_G(current_frame))))) {
			// JIT: if (EG(exception))
			zend_jit_guard_not(jit,
				zend_jit_load(jit, IR_ADDR, ZEND_JIT_EG_ADDR(exception)),
				zend_jit_stub_addr(jit, jit_stub_leave_throw));
		}

		return 1;
	} else {
		// JIT: if (EG(exception))
		zend_jit_guard_not(jit,
			zend_jit_load(jit, IR_ADDR, ZEND_JIT_EG_ADDR(exception)),
			zend_jit_stub_addr(jit, jit_stub_leave_throw));
		// JIT: opline = EX(opline) + 1
		if (GCC_GLOBAL_REGS) {
			zend_jit_store_ip(jit,
				zend_jit_load(jit, IR_ADDR,
					zend_jit_ex_opline_addr(jit)));
			zend_jit_store_ip(jit,
				zend_jit_addr_offset(jit, zend_jit_ip(jit), sizeof(zend_op)));
		} else {
			ir_ref ref = zend_jit_ex_opline_addr(jit);

			zend_jit_store(jit,
				ref,
				zend_jit_addr_offset(jit, zend_jit_load(jit, IR_ADDR, ref), sizeof(zend_op)));
		}
	}

	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		zend_jit_tailcall_0(jit,
			zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));
	} else if (GCC_GLOBAL_REGS) {
		zend_jit_tailcall_0(jit,
			zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));
	} else {
ZEND_ASSERT(0);
//???		|	mov FP, aword T2 // restore FP
//???		|	mov RX, aword T3 // restore IP
//???		|	add r4, NR_SPAD // stack alignment
//???		|	mov r0, 2 // ZEND_VM_LEAVE
//???		|	ret
	}

	jit->control = IR_UNUSED;
	jit->b = -1;

	return 1;
}

static int zend_jit_return(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, uint32_t op1_info, zend_jit_addr op1_addr)
{
	zend_jit_addr ret_addr;
	int8_t return_value_used;
	ir_ref return_value = IR_UNUSED, ref, refcount, if_return_value_used = IR_UNUSED;
	ir_ref end_inputs[8];
	uint32_t end_inputs_count = 0;

	ZEND_ASSERT(op_array->type != ZEND_EVAL_CODE && op_array->function_name);
	ZEND_ASSERT(!(op1_info & MAY_BE_UNDEF));

	if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && JIT_G(current_frame)) {
		if (TRACE_FRAME_IS_RETURN_VALUE_USED(JIT_G(current_frame))) {
			return_value_used = 1;
		} else if (TRACE_FRAME_IS_RETURN_VALUE_UNUSED(JIT_G(current_frame))) {
			return_value_used = 0;
		} else {
			return_value_used = -1;
		}
	} else {
		return_value_used = -1;
	}

	if (ZEND_OBSERVER_ENABLED) {
		if (Z_MODE(op1_addr) == IS_REG) {
			zend_jit_addr dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->op1.var);

			if (!zend_jit_spill_store(jit, op1_addr, dst, op1_info, 1)) {
				return 0;
			}
			op1_addr = dst;
		}
		zend_jit_call_2(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_observer_fcall_end, IR_CONST_FASTCALL_FUNC),
			zend_jit_fp(jit),
			zend_jit_zval_addr(jit, op1_addr));
	}

	// JIT: if (!EX(return_value))
	return_value = zend_jit_load_at_offset(jit, IR_ADDR,
		zend_jit_fp(jit), offsetof(zend_execute_data, return_value));
	ret_addr = ZEND_ADDR_REF_ZVAL(return_value);
	if ((opline->op1_type & (IS_VAR|IS_TMP_VAR)) &&
	    (op1_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
		if (return_value_used == -1) {
			if_return_value_used = zend_jit_if(jit, return_value);
			zend_jit_if_false_cold(jit, if_return_value_used);
		}
		if (return_value_used != 1) {
			if (op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)-(MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
				ir_ref if_refcounted = zend_jit_if_zval_refcounted(jit, op1_addr);
				zend_jit_if_false(jit, if_refcounted);
				end_inputs[end_inputs_count++]  = zend_jit_end(jit);
				zend_jit_if_true(jit, if_refcounted);
			}
			ref = zend_jit_zval_ptr(jit, op1_addr);
			refcount = zend_jit_gc_delref(jit, ref);

			if (RC_MAY_BE_1(op1_info)) {
				if (RC_MAY_BE_N(op1_info)) {
					ir_ref if_non_zero = zend_jit_if(jit, refcount);
					zend_jit_if_true(jit, if_non_zero);
					zend_jit_array_add(end_inputs, zend_jit_end(jit));
					zend_jit_if_false(jit, if_non_zero);
				}
				zend_jit_zval_dtor(jit, ref, op1_info, opline);
			}
			if (return_value_used == -1) {
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}
		}
	} else if (return_value_used == -1) {
		if_return_value_used = zend_jit_if(jit, return_value);
		zend_jit_if_false_cold(jit, if_return_value_used);
		zend_jit_array_add(end_inputs, zend_jit_end(jit));
	}

	if (if_return_value_used) {
		zend_jit_if_true(jit, if_return_value_used);
	}

	if (return_value_used == 0) {
		if (end_inputs_count) {
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_merge_N(jit, end_inputs_count, end_inputs);
		}
		return 1;
	}

	if (opline->op1_type == IS_CONST) {
		zval *zv = RT_CONSTANT(opline, opline->op1);

		zend_jit_copy_zval_const(jit, ret_addr, MAY_BE_ANY, MAY_BE_ANY, zv, 1);
	} else if (opline->op1_type == IS_TMP_VAR) {
		zend_jit_copy_zval(jit, ret_addr, MAY_BE_ANY, op1_addr, op1_info, 0);
	} else if (opline->op1_type == IS_CV) {
		if (op1_info & MAY_BE_REF) {
			ref = zend_jit_zval_addr(jit, op1_addr);
			ref = zend_jit_zval_ref_deref(jit, ref);
			op1_addr = ZEND_ADDR_REF_ZVAL(ref);
		}

		if (op1_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
			if (JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE ||
			    (op1_info & (MAY_BE_REF|MAY_BE_OBJECT)) ||
			    !op_array->function_name) {
				zend_jit_copy_zval(jit, ret_addr, MAY_BE_ANY, op1_addr, op1_info, 1);
			} else if (return_value_used != 1) {
				zend_jit_copy_zval(jit, ret_addr, MAY_BE_ANY, op1_addr, op1_info, 0);
				// JIT: if (EXPECTED(!(EX_CALL_INFO() & ZEND_CALL_CODE))) ZVAL_NULL(retval_ptr);
				zend_jit_zval_set_type_info(jit, op1_addr, IS_NULL);
			} else {
				zend_jit_copy_zval(jit, ret_addr, MAY_BE_ANY, op1_addr, op1_info, 0);
			}
		} else {
			zend_jit_copy_zval(jit, ret_addr, MAY_BE_ANY, op1_addr, op1_info, 0);
		}
	} else {
		if (op1_info & MAY_BE_REF) {
			ir_ref if_ref, ref2, if_non_zero;
			zend_jit_addr ref_addr;

			if_ref = zend_jit_if_zval_type(jit, op1_addr, IS_REFERENCE);
			zend_jit_if_true_cold(jit, if_ref);

			// JIT: zend_refcounted *ref = Z_COUNTED_P(retval_ptr)
			ref = zend_jit_zval_ptr(jit, op1_addr);

			// JIT: ZVAL_COPY_VALUE(return_value, &ref->value)
			ref2 = zend_jit_addr_offset(jit, ref, offsetof(zend_reference, val));
			ref_addr = ZEND_ADDR_REF_ZVAL(ref2);
			zend_jit_copy_zval(jit, ret_addr, MAY_BE_ANY, ref_addr, op1_info, 0);
			ref2 = zend_jit_gc_delref(jit, ref);
			if_non_zero = zend_jit_if(jit, ref2);
			zend_jit_if_true(jit, if_non_zero);

			// JIT: if (IS_REFCOUNTED())
			ir_ref if_refcounted = zend_jit_if_zval_refcounted(jit, ret_addr);
			zend_jit_if_false(jit, if_refcounted);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_if_true(jit, if_refcounted);

			// JIT: ADDREF
			ref2 = zend_jit_zval_ptr(jit, ret_addr);
			zend_jit_gc_addref(jit, ref2);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));

			zend_jit_if_false(jit, if_non_zero);

			zend_jit_efree(jit, ref, sizeof(zend_reference), op_array, opline);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));

			zend_jit_if_false(jit, if_ref);
		}
		zend_jit_copy_zval(jit, ret_addr, MAY_BE_ANY, op1_addr, op1_info, 0);
	}

	if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
		if (end_inputs_count) {
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_merge_N(jit, end_inputs_count, end_inputs);
		}
	} else {
		zend_jit_array_add(end_inputs, zend_jit_end(jit));
		jit->return_inputs = zend_jit_list_add_N(jit, jit->return_inputs, end_inputs_count, end_inputs);

		jit->control = IR_UNUSED;
		jit->b = -1;
	}

	return 1;
}

static int zend_jit_bind_global(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info)
{
	zend_jit_addr op1_addr = OP1_ADDR();
	zend_string *varname = Z_STR_P(RT_CONSTANT(opline, opline->op2));
	ir_ref cache_slot_ref, idx_ref, num_used_ref, bucket_ref, ref, ref2;
	ir_ref cond, if_fit, if_reference, if_same_key, fast_path;
	ir_ref slow_inputs[3], end_inputs[4];
	uint32_t slow_inputs_count = 0;
	uint32_t end_inputs_count = 0;

	// JIT: idx = (uintptr_t)CACHED_PTR(opline->extended_value) - 1;
	cache_slot_ref = zend_jit_addr_offset(jit,
		zend_jit_load_at_offset(jit, IR_ADDR,
			zend_jit_fp(jit), offsetof(zend_execute_data, run_time_cache)),
		opline->extended_value);
	idx_ref = ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_ADDR),
		zend_jit_load(jit, IR_ADDR, cache_slot_ref),
		zend_jit_const_addr(jit, 1));

	// JIT: if (EXPECTED(idx < EG(symbol_table).nNumUsed * sizeof(Bucket)))
	num_used_ref = ir_fold2(&jit->ctx, IR_OPT(IR_MUL, IR_U32),
		zend_jit_load(jit, IR_U32,
			ZEND_JIT_EG_ADDR(symbol_table.nNumUsed)),
		ir_const_u32(&jit->ctx, sizeof(Bucket)));
	if (sizeof(void*) == 8) {
		num_used_ref = ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_ADDR), num_used_ref);
	}
	cond = ir_fold2(&jit->ctx, IR_OPT(IR_ULT, IR_BOOL),
		idx_ref,
		num_used_ref);
	if_fit = zend_jit_if(jit, cond);
	zend_jit_if_false_cold(jit, if_fit);
	zend_jit_array_add(slow_inputs, zend_jit_end(jit));
	zend_jit_if_true(jit, if_fit);

	// JIT: Bucket *p = (Bucket*)((char*)EG(symbol_table).arData + idx);
	bucket_ref = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
		zend_jit_load(jit, IR_ADDR,
			ZEND_JIT_EG_ADDR(symbol_table.arData)),
		idx_ref);
	if_reference = zend_jit_if_zval_ref_type(jit, bucket_ref, ir_const_u8(&jit->ctx, IS_REFERENCE));
	zend_jit_if_false_cold(jit, if_reference);
	zend_jit_array_add(slow_inputs, zend_jit_end(jit));
	zend_jit_if_true(jit, if_reference);

	// JIT: (EXPECTED(p->key == varname))
	cond = ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_ADDR),
		zend_jit_load_at_offset(jit, IR_ADDR,
			bucket_ref, offsetof(Bucket, key)),
		zend_jit_const_addr(jit, (uintptr_t)varname));
	if_same_key = zend_jit_if(jit, cond);
	zend_jit_if_false_cold(jit, if_same_key);
	zend_jit_array_add(slow_inputs, zend_jit_end(jit));
	zend_jit_if_true(jit, if_same_key);

	// JIT: GC_ADDREF(Z_PTR(p->val))
	ref = zend_jit_zval_ref_ptr(jit, bucket_ref);
	zend_jit_gc_addref(jit, ref);

	fast_path = zend_jit_end(jit);
	zend_jit_merge_N(jit, slow_inputs_count, slow_inputs);

	ref2 = zend_jit_call_2(jit, IR_ADDR,
		zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_global_helper, IR_CONST_FASTCALL_FUNC),
		zend_jit_const_addr(jit, (uintptr_t)varname),
		cache_slot_ref);

	zend_jit_merge_2(jit, fast_path, zend_jit_end(jit));
	ref = zend_jit_phi_2(jit, ref, ref2);

	if (op1_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF)) {
		ir_ref if_refcounted = IR_UNUSED, refcount, if_non_zero, if_may_not_leak;

		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
			// JIT: if (UNEXPECTED(Z_REFCOUNTED_P(variable_ptr)))
			if_refcounted = zend_jit_if_zval_refcounted(jit, op1_addr);
			zend_jit_if_true_cold(jit, if_refcounted);
		}

		// JIT:zend_refcounted *garbage = Z_COUNTED_P(variable_ptr);
		ref2 = zend_jit_zval_ptr(jit, op1_addr);

		// JIT: ZVAL_REF(variable_ptr, ref)
		zend_jit_zval_set_ptr(jit, op1_addr, ref);
		zend_jit_zval_set_type_info(jit, op1_addr, IS_REFERENCE_EX);

		// JIT: if (GC_DELREF(garbage) == 0)
		refcount = zend_jit_gc_delref(jit, ref2);
		if_non_zero = zend_jit_if(jit, refcount);
		if (!(op1_info & (MAY_BE_REF|MAY_BE_ARRAY|MAY_BE_OBJECT))) {
			zend_jit_if_true(jit, if_non_zero);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
		zend_jit_if_false(jit, if_non_zero);

		zend_jit_zval_dtor(jit, ref2, op1_info, opline);
		if (op1_info & (MAY_BE_REF|MAY_BE_ARRAY|MAY_BE_OBJECT)) {
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_if_true(jit, if_non_zero);

			// JIT: GC_ZVAL_CHECK_POSSIBLE_ROOT(variable_ptr)
			if_may_not_leak = zend_jit_if_gc_may_not_leak(jit, ref2);
			zend_jit_if_true(jit, if_may_not_leak);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_if_false(jit, if_may_not_leak);
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)gc_possible_root, IR_CONST_FASTCALL_FUNC),
				ref2);
		}
		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_if_false(jit, if_refcounted);
		}
	}

	if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
		// JIT: ZVAL_REF(variable_ptr, ref)
		zend_jit_zval_set_ptr(jit, op1_addr, ref);
		zend_jit_zval_set_type_info(jit, op1_addr, IS_REFERENCE_EX);
	}

	if (end_inputs_count) {
		zend_jit_array_add(end_inputs, zend_jit_end(jit));
		zend_jit_merge_N(jit, end_inputs_count, end_inputs);
	}

	return 1;
}

static int zend_jit_free(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, int may_throw)
{
	zend_jit_addr op1_addr = OP1_ADDR();

	if (op1_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF)) {
		if (may_throw) {
			zend_jit_set_ex_opline(jit, opline);
		}
		if (opline->opcode == ZEND_FE_FREE && (op1_info & (MAY_BE_OBJECT|MAY_BE_REF))) {
			ir_ref ref, if_array, if_exists, merge[3];
			uint32_t merge_count = 0;

			if (op1_info & MAY_BE_ARRAY) {
				if_array = zend_jit_if_zval_type(jit, op1_addr, IS_ARRAY);
				zend_jit_if_true(jit, if_array);
				zend_jit_array_add(merge, zend_jit_end(jit));
				zend_jit_if_false(jit, if_array);
			}
			ref = zend_jit_load_at_offset(jit, IR_U32,
				zend_jit_fp(jit), opline->op1.var + offsetof(zval, u2.fe_iter_idx));
			if_exists = zend_jit_if(jit,
				ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL), ref, ir_const_u32(&jit->ctx, -1)));
			zend_jit_if_true(jit, if_exists);
			zend_jit_array_add(merge, zend_jit_end(jit));
			zend_jit_if_false(jit, if_exists);

			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_iterator_del, IR_CONST_FASTCALL_FUNC),
				ref);

			zend_jit_array_add(merge, zend_jit_end(jit));
			zend_jit_merge_N(jit, merge_count, merge);
		}

		zend_jit_zval_ptr_dtor(jit, op1_addr, op1_info, 0, opline);

		if (may_throw) {
			zend_jit_check_exception(jit);
		}
	}

	return 1;
}

static int zend_jit_echo(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info)
{
	if (opline->op1_type == IS_CONST) {
		zval *zv;
		size_t len;

		zv = RT_CONSTANT(opline, opline->op1);
		ZEND_ASSERT(Z_TYPE_P(zv) == IS_STRING);
		len = Z_STRLEN_P(zv);

		if (len > 0) {
			const char *str = Z_STRVAL_P(zv);

			zend_jit_set_ex_opline(jit, opline);
			zend_jit_call_2(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_write, 0),
				zend_jit_const_addr(jit, (uintptr_t)str),
				zend_jit_const_addr(jit, len));

			zend_jit_check_exception(jit);
		}
	} else {
		zend_jit_addr op1_addr = OP1_ADDR();
		ir_ref ref;

		ZEND_ASSERT((op1_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) == MAY_BE_STRING);

		zend_jit_set_ex_opline(jit, opline);

		ref = zend_jit_zval_ptr(jit, op1_addr);
		zend_jit_call_2(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_write, 0),
			zend_jit_addr_offset(jit, ref, offsetof(zend_string, val)),
			zend_jit_load_at_offset(jit, IR_ADDR, ref, offsetof(zend_string, len)));

		if (opline->op1_type & (IS_VAR|IS_TMP_VAR)) {
			zend_jit_zval_ptr_dtor(jit, op1_addr, op1_info, 0, opline);
		}

		zend_jit_check_exception(jit);
	}
	return 1;
}

static int zend_jit_strlen(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, zend_jit_addr res_addr)
{
	if (opline->op1_type == IS_CONST) {
		zval *zv;
		size_t len;

		zv = RT_CONSTANT(opline, opline->op1);
		ZEND_ASSERT(Z_TYPE_P(zv) == IS_STRING);
		len = Z_STRLEN_P(zv);

		zend_jit_zval_set_lval(jit, res_addr, ir_const_php_long(&jit->ctx, len));
		if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
			zend_jit_zval_set_type_info(jit, res_addr, IS_LONG);
		} else if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, MAY_BE_LONG)) {
			return 0;
		}
	} else {
		ir_ref ref;

		ZEND_ASSERT((op1_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) == MAY_BE_STRING);

		ref = zend_jit_zval_ptr(jit, op1_addr);
		ref = zend_jit_load_at_offset(jit, IR_PHP_LONG,
			ref, offsetof(zend_string, len));
		zend_jit_zval_set_lval(jit, res_addr, ref);

		if (Z_MODE(res_addr) == IS_REG) {
			if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, MAY_BE_LONG)) {
				return 0;
			}
		} else {
			zend_jit_zval_set_type_info(jit, res_addr, IS_LONG);
		}
		zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);
	}
	return 1;
}

static int zend_jit_count(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, zend_jit_addr res_addr, int may_throw)
{
	if (opline->op1_type == IS_CONST) {
		zval *zv;
		zend_long count;

		zv = RT_CONSTANT(opline, opline->op1);
		ZEND_ASSERT(Z_TYPE_P(zv) == IS_ARRAY);
		count = zend_hash_num_elements(Z_ARRVAL_P(zv));

		zend_jit_zval_set_lval(jit, res_addr, ir_const_php_long(&jit->ctx, count));
		if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
			zend_jit_zval_set_type_info(jit, res_addr, IS_LONG);
		} else if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, MAY_BE_LONG)) {
			return 0;
		}
	} else {
		ir_ref ref;

		ZEND_ASSERT((op1_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) == MAY_BE_ARRAY);
		// Note: See the implementation of ZEND_COUNT in Zend/zend_vm_def.h - arrays do not contain IS_UNDEF starting in php 8.1+.

		ref = zend_jit_zval_ptr(jit, op1_addr);
		if (sizeof(void*) == 8) {
			ref = zend_jit_load_at_offset(jit, IR_U32,
				ref, offsetof(HashTable, nNumOfElements));
			ref = ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_PHP_LONG), ref);
		} else {
			ref = zend_jit_load_at_offset(jit, IR_PHP_LONG,
				ref, offsetof(HashTable, nNumOfElements));
		}
		zend_jit_zval_set_lval(jit, res_addr, ref);

		if (Z_MODE(res_addr) == IS_REG) {
			if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, MAY_BE_LONG)) {
				return 0;
			}
		} else {
			zend_jit_zval_set_type_info(jit, res_addr, IS_LONG);
		}
		zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);
	}

	if (may_throw) {
		zend_jit_check_exception(jit);
	}
	return 1;
}

static int zend_jit_in_array(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr,  zend_uchar smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr)
{
	HashTable *ht = Z_ARRVAL_P(RT_CONSTANT(opline, opline->op2));
	zend_jit_addr res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var);
	ir_ref ref;

	ZEND_ASSERT(opline->op1_type != IS_VAR && opline->op1_type != IS_TMP_VAR);
	ZEND_ASSERT((op1_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF)) == MAY_BE_STRING);

	// JIT: result = zend_hash_find_ex(ht, Z_STR_P(op1), OP1_TYPE == IS_CONST);
	if (opline->op1_type != IS_CONST) {
		ref = zend_jit_call_2(jit, IR_ADDR,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_find, IR_CONST_FASTCALL_FUNC),
			zend_jit_const_addr(jit, (uintptr_t)ht),
			zend_jit_zval_ptr(jit, op1_addr));
	} else {
		zend_string *str = Z_STR_P(RT_CONSTANT(opline, opline->op1));

		ref = zend_jit_call_2(jit, IR_ADDR,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_find_known_hash, IR_CONST_FASTCALL_FUNC),
			zend_jit_const_addr(jit, (uintptr_t)ht),
			zend_jit_const_addr(jit, (uintptr_t)str));
	}

	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ) {
			zend_jit_guard(jit, ref, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		} else {
			zend_jit_guard_not(jit, ref, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		}
	} else if (smart_branch_opcode) {
		zend_basic_block *bb;

		ZEND_ASSERT(jit->b >= 0);
		bb = &jit->ssa->cfg.blocks[jit->b];
		ZEND_ASSERT(bb->successors_count == 2);
		ref = zend_jit_if_ex(jit, ref,
			(smart_branch_opcode == ZEND_JMPZ) ? target_label2 : target_label);
		_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, ref);
		_zend_jit_add_predecessor_ref(jit, bb->successors[1], jit->b, ref);
		jit->control = IR_UNUSED;
		jit->b = -1;
	} else {
		zend_jit_zval_set_type_info_ref(jit, res_addr,
			ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
				ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_U32),
					ir_fold2(&jit->ctx, IR_OPT(IR_NE, IR_BOOL), ref, IR_NULL)),
				ir_const_u32(&jit->ctx, IS_FALSE)));
	}

	return 1;
}

static int zend_jit_rope(zend_jit_ctx *jit, const zend_op *opline, uint32_t op2_info)
{
	uint32_t offset;

	offset = (opline->opcode == ZEND_ROPE_INIT) ?
		opline->result.var :
		opline->op1.var + opline->extended_value * sizeof(zend_string*);

	if (opline->op2_type == IS_CONST) {
		zval *zv = RT_CONSTANT(opline, opline->op2);
		zend_string *str;

		ZEND_ASSERT(Z_TYPE_P(zv) == IS_STRING);
		str = Z_STR_P(zv);

		zend_jit_store_at_offset(jit,
			zend_jit_fp(jit), offset,
			zend_jit_const_addr(jit, (uintptr_t)str));
	} else {
		zend_jit_addr op2_addr = OP2_ADDR();
		ir_ref ref;

		ZEND_ASSERT((op2_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) == MAY_BE_STRING);

		ref = zend_jit_zval_ptr(jit, op2_addr);
		zend_jit_store_at_offset(jit,
			zend_jit_fp(jit), offset,
			ref);
		if (opline->op2_type == IS_CV) {
			ir_ref if_refcounted, long_path;

			if_refcounted = zend_jit_if_zval_refcounted(jit, op2_addr);
			zend_jit_if_true(jit, if_refcounted);
			zend_jit_gc_addref(jit, ref);
			long_path = zend_jit_end(jit);

			zend_jit_if_false(jit, if_refcounted);
			zend_jit_merge_2(jit, long_path, zend_jit_end(jit));
		}
	}

	if (opline->opcode == ZEND_ROPE_END) {
		zend_jit_addr res_addr = RES_ADDR();
		ir_ref ref;

		ref = zend_jit_call_2(jit, IR_ADDR,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_rope_end, IR_CONST_FASTCALL_FUNC),
			zend_jit_addr_offset(jit, zend_jit_fp(jit), opline->op1.var),
			ir_const_u32(&jit->ctx, opline->extended_value));

		zend_jit_zval_set_ptr(jit, res_addr, ref);
		zend_jit_zval_set_type_info(jit, res_addr, IS_STRING_EX);
	}

	return 1;
}

static int zend_jit_zval_copy_deref(zend_jit_ctx *jit, zend_jit_addr res_addr, zend_jit_addr val_addr, ir_ref val_type_info)
{
	ir_ref if_refcounted, if_reference, ptr, merge_inputs[4], val_type_info_inputs[4], ptr_inputs[4];
	ir_ref ref2, ptr2, val_type_info2;
#if SIZEOF_ZEND_LONG == 4
	ir_ref ref, val_addr_inputs[4];
#endif
	uint32_t merge_inputs_count = 0;

	// JIT: ptr = Z_PTR_P(val);
	ptr = zend_jit_zval_ptr(jit, val_addr);

	// JIT: if (Z_OPT_REFCOUNTED_P(val)) {
	if_refcounted = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
			val_type_info,
			ir_const_u32(&jit->ctx, (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT))));
	zend_jit_if_false(jit, if_refcounted);
	merge_inputs[merge_inputs_count] = zend_jit_end(jit);
#if SIZEOF_ZEND_LONG == 4
	ref = zend_jit_zval_addr(jit, val_addr);
	val_addr_inputs[merge_inputs_count] = ref;
#endif
	val_type_info_inputs[merge_inputs_count] = val_type_info;
	ptr_inputs[merge_inputs_count] = ptr;
	merge_inputs_count++;

	zend_jit_if_true(jit, if_refcounted);

	// JIT: if (UNEXPECTED(Z_OPT_ISREF_P(val))) {
	if_reference = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_U8),
			ir_fold1(&jit->ctx, IR_OPT(IR_TRUNC, IR_U8), val_type_info),
			ir_const_u8(&jit->ctx, IS_REFERENCE)));
	zend_jit_if_true(jit, if_reference);

	// JIT:	val = Z_REFVAL_P(val);
	ref2 = zend_jit_addr_offset(jit, ptr, offsetof(zend_reference, val));
	ptr2 = zend_jit_zval_ref_ptr(jit, ref2);
	val_type_info2 = zend_jit_zval_ref_type_info(jit, ref2);

	// JIT:	if (Z_OPT_REFCOUNTED_P(val)) {
	if_refcounted = zend_jit_if(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
			val_type_info2,
			ir_const_u32(&jit->ctx, (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT))));
	zend_jit_if_false(jit, if_refcounted);
	merge_inputs[merge_inputs_count] = zend_jit_end(jit);
#if SIZEOF_ZEND_LONG == 4
	val_addr_inputs[merge_inputs_count] = ref2;
#endif
	val_type_info_inputs[merge_inputs_count] = val_type_info2;
	ptr_inputs[merge_inputs_count] = ptr2;
	merge_inputs_count++;

	zend_jit_if_true(jit, if_refcounted);

	// JIT:	Z_ADDREF_P(val);
	zend_jit_gc_addref(jit, ptr2);
	merge_inputs[merge_inputs_count] = zend_jit_end(jit);
#if SIZEOF_ZEND_LONG == 4
	val_addr_inputs[merge_inputs_count] = ref2;
#endif
	val_type_info_inputs[merge_inputs_count] = val_type_info2;
	ptr_inputs[merge_inputs_count] = ptr2;
	merge_inputs_count++;

	zend_jit_if_false(jit, if_reference);
	// JIT:	Z_ADDREF_P(val);
	zend_jit_gc_addref(jit, ptr);
	merge_inputs[merge_inputs_count] = zend_jit_end(jit);
#if SIZEOF_ZEND_LONG == 4
	val_addr_inputs[merge_inputs_count] = ref;
#endif
	val_type_info_inputs[merge_inputs_count] = val_type_info;
	ptr_inputs[merge_inputs_count] = ptr;
	merge_inputs_count++;

	zend_jit_merge_N(jit, merge_inputs_count, merge_inputs);
#if SIZEOF_ZEND_LONG == 4
	ref = zend_jit_phi_N(jit, merge_inputs_count, val_addr_inputs);
	val_addr = ZEND_ADDR_REF_ZVAL(ref);
#endif
	val_type_info = zend_jit_phi_N(jit, merge_inputs_count, val_type_info_inputs);
	ptr = zend_jit_phi_N(jit, merge_inputs_count, ptr_inputs);

	// JIT: Z_PTR_P(res) = ptr;
	zend_jit_zval_set_ptr(jit, res_addr, ptr);
#if SIZEOF_ZEND_LONG == 4
	zend_jit_zval_set_w2(jit, res_addr,
		zend_jit_zval_w2(jit, val_addr));
#endif
	zend_jit_zval_set_type_info_ref(jit, res_addr, val_type_info);

	return 1;
}

static int zend_jit_fetch_dimension_address_inner(zend_jit_ctx  *jit,
                                                  const zend_op *opline,
                                                  uint32_t       type,
                                                  uint32_t       op1_info,
                                                  uint32_t       op2_info,
                                                  uint8_t        dim_type,
                                                  const void    *found_exit_addr,
                                                  const void    *not_found_exit_addr,
                                                  const void    *exit_addr,
                                                  bool           result_type_guard,
                                                  ir_ref         ht_ref,
                                                  uint32_t      *found_inputs_count,
                                                  ir_ref        *found_inputs,
                                                  ir_ref        *found_vals,
                                                  uint32_t      *end_inputs_count,
                                                  ir_ref        *end_inputs)
{
	zend_jit_addr op2_addr = OP2_ADDR();
	zend_jit_addr res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var);
	ir_ref ref = IR_UNUSED, cond, if_found;
	ir_ref if_type = IS_UNUSED;
	ir_ref test_zval_inputs[4], test_zval_values[4];
	uint32_t test_zval_inputs_count = 0;

	if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE
	 && type == BP_VAR_R
	 && !exit_addr) {
		int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
		exit_addr = zend_jit_trace_get_exit_addr(exit_point);
		if (!exit_addr) {
			return 0;
		}
	}

	if (op2_info & MAY_BE_LONG) {
		bool op2_loaded = 0;
		bool packed_loaded = 0;
		bool bad_packed_key = 0;
		ir_ref if_packed = IS_UNDEF;
		ir_ref h = IR_UNUSED;
		ir_ref idx_not_found_inputs[5];
		uint32_t idx_not_found_inputs_count = 0;

		if (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - MAY_BE_LONG)) {
			// JIT: if (EXPECTED(Z_TYPE_P(dim) == IS_LONG))
			if_type = zend_jit_if_zval_type(jit, op2_addr, IS_LONG);
			zend_jit_if_true(jit, if_type);
		}
		if (op1_info & MAY_BE_PACKED_GUARD) {
			int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_PACKED_GUARD);
			const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

			if (!exit_addr) {
				return 0;
			}
			cond = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
				zend_jit_load_at_offset(jit, IR_U32,
					ht_ref, offsetof(zend_array, u.flags)),
				ir_const_u32(&jit->ctx, HASH_FLAG_PACKED));
			if (op1_info & MAY_BE_ARRAY_PACKED) {
				zend_jit_guard(jit, cond, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				zend_jit_guard_not(jit, cond, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			}
		}
		if (type == BP_VAR_W) {
			// JIT: hval = Z_LVAL_P(dim);
			h = zend_jit_zval_lval(jit, op2_addr);
			op2_loaded = 1;
		}
		if (op1_info & MAY_BE_ARRAY_PACKED) {
			zend_long val = -1;

			if (Z_MODE(op2_addr) == IS_CONST_ZVAL) {
				val = Z_LVAL_P(Z_ZV(op2_addr));
				if (val >= 0 && val < HT_MAX_SIZE) {
					packed_loaded = 1;
				} else {
					bad_packed_key = 1;
					if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && type == BP_VAR_R) {
						zend_jit_side_exit(jit, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
					}
				}
				h = ir_const_php_long(&jit->ctx, val);
			} else {
				if (!op2_loaded) {
					// JIT: hval = Z_LVAL_P(dim);
					h = zend_jit_zval_lval(jit, op2_addr);
					op2_loaded = 1;
				}
				packed_loaded = 1;
			}

			if (dim_type == IS_UNDEF && type == BP_VAR_W && packed_loaded) {
				/* don't generate "fast" code for packed array */
				packed_loaded = 0;
			}

			if (packed_loaded) {
				// JIT: ZEND_HASH_INDEX_FIND(ht, hval, retval, num_undef);
				if (op1_info & MAY_BE_ARRAY_NUMERIC_HASH) {
					if_packed = zend_jit_if(jit,
						ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
							zend_jit_load_at_offset(jit, IR_U32,
								ht_ref, offsetof(zend_array, u.flags)),
							ir_const_u32(&jit->ctx, HASH_FLAG_PACKED)));
					zend_jit_if_true(jit, if_packed);
				}
				// JIT: if (EXPECTED((zend_ulong)(_h) < (zend_ulong)(_ht)->nNumUsed))
				ref = zend_jit_load_at_offset(jit, IR_U32,
					ht_ref, offsetof(zend_array, nNumUsed));
#if SIZEOF_ZEND_LONG == 8
				ref = ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_PHP_LONG), ref);
#endif
				cond = ir_fold2(&jit->ctx, IR_OPT(IR_ULT, IR_BOOL), h, ref);
				if (type == BP_JIT_IS) {
					if (not_found_exit_addr) {
						zend_jit_guard(jit, cond, zend_jit_const_addr(jit, (uintptr_t)not_found_exit_addr));
					} else {
						ir_ref if_fit = zend_jit_if(jit, cond);
						zend_jit_if_false(jit, if_fit);
						end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
						zend_jit_if_true(jit, if_fit);
					}
				} else if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && type == BP_VAR_R) {
					zend_jit_guard(jit, cond, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				} else if (type == BP_VAR_IS && not_found_exit_addr) {
					zend_jit_guard(jit, cond, zend_jit_const_addr(jit, (uintptr_t)not_found_exit_addr));
				} else if (type == BP_VAR_RW && not_found_exit_addr) {
					zend_jit_guard(jit, cond, zend_jit_const_addr(jit, (uintptr_t)not_found_exit_addr));
//???+			} else if (type == BP_VAR_IS && result_type_guard) {
//???+				|	jbe >7 // NOT_FOUND
				} else {
					ir_ref if_fit = zend_jit_if(jit, cond);
					zend_jit_if_false(jit, if_fit);
					zend_jit_array_add(idx_not_found_inputs, zend_jit_end(jit));
					zend_jit_if_true(jit, if_fit);
				}
				// JIT: _ret = &_ht->arPacked[h];
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_MUL, IR_PHP_LONG),
						h,
						ir_const_php_long(&jit->ctx, sizeof(zval)));
				ref = ir_fold1(&jit->ctx, IR_OPT(IR_BITCAST, IR_ADDR), ref);
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
					zend_jit_load_at_offset(jit, IR_ADDR,
						ht_ref, offsetof(zend_array, arPacked)),
					ref);
				if (type == BP_JIT_IS) {
					test_zval_values[test_zval_inputs_count] = ref;
					zend_jit_array_add(test_zval_inputs, zend_jit_end(jit));
				}
			}
		}
		switch (type) {
			case BP_JIT_IS:
				if (op1_info & MAY_BE_ARRAY_NUMERIC_HASH) {
					if (if_packed) {
						zend_jit_if_false(jit, if_packed);
						if_packed = IR_UNUSED;
					}
					if (!op2_loaded) {
						// JIT: hval = Z_LVAL_P(dim);
						h = zend_jit_zval_lval(jit, op2_addr);
					}
					if (packed_loaded) {
						ref = zend_jit_call_2(jit, IR_ADDR,
							zend_jit_const_func_addr(jit, (uintptr_t)_zend_hash_index_find, IR_CONST_FASTCALL_FUNC),
							ht_ref,
							h);
					} else {
						ref = zend_jit_call_2(jit, IR_ADDR,
							zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_index_find, IR_CONST_FASTCALL_FUNC),
							ht_ref,
							h);
					}
					if (not_found_exit_addr) {
						zend_jit_guard(jit, ref, zend_jit_const_addr(jit, (uintptr_t)not_found_exit_addr));
					} else {
						if_found = zend_jit_if(jit, ref);
						zend_jit_if_false(jit, if_found);
						end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
						zend_jit_if_true(jit, if_found);
					}
					test_zval_values[test_zval_inputs_count] = ref;
					zend_jit_array_add(test_zval_inputs, zend_jit_end(jit));
				} else if (not_found_exit_addr) {
//???+					zend_jit_side_exit(jit, zend_jit_const_addr(jit, (uintptr_t)not_found_exit_addr));
//???+					|	jmp &not_found_exit_addr
				} else if (!packed_loaded) {
					end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
				}
				break;
			case BP_VAR_R:
			case BP_VAR_IS:
			case BP_VAR_UNSET:
				if (packed_loaded) {
					ir_ref type_ref = zend_jit_zval_ref_type(jit, ref);

					if (op1_info & MAY_BE_ARRAY_NUMERIC_HASH) {
						ir_ref if_def = zend_jit_if(jit, type_ref);
						zend_jit_if_true(jit, if_def);
						found_inputs[*found_inputs_count] = zend_jit_end(jit);
						found_vals[*found_inputs_count] = ref;
						(*found_inputs_count)++;
						zend_jit_if_false(jit, if_def);
					} else if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && type == BP_VAR_R) {
						/* perform IS_UNDEF check only after result type guard (during deoptimization) */
						if (!result_type_guard || (op1_info & MAY_BE_ARRAY_NUMERIC_HASH)) {
							zend_jit_guard(jit, type_ref, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
						}
					} else if (type == BP_VAR_IS && not_found_exit_addr) {
						zend_jit_guard(jit, type_ref, zend_jit_const_addr(jit, (uintptr_t)not_found_exit_addr));
//???+				} else if (type == BP_VAR_IS && result_type_guard) {
//???+					|	IF_Z_TYPE r0, IS_UNDEF, >7 // NOT_FOUND
					} else {
						ir_ref if_def = zend_jit_if(jit, type_ref);
						zend_jit_if_false(jit, if_def);
						zend_jit_array_add(idx_not_found_inputs, zend_jit_end(jit));
						zend_jit_if_true(jit, if_def);
					}
				}
				if (!(op1_info & MAY_BE_ARRAY_KEY_LONG) || (packed_loaded && (op1_info & MAY_BE_ARRAY_NUMERIC_HASH))) {
					if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && type == BP_VAR_R) {
						zend_jit_side_exit(jit, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
					} else if (type == BP_VAR_IS && not_found_exit_addr) {
						zend_jit_side_exit(jit, zend_jit_const_addr(jit, (uintptr_t)not_found_exit_addr));
//???+				} else if (type == BP_VAR_IS && result_type_guard) {
//???+					|	jmp >7 // NOT_FOUND
					} else {
						zend_jit_array_add(idx_not_found_inputs, zend_jit_end(jit));
					}
				}
				if (/*!packed_loaded ||*/ (op1_info & MAY_BE_ARRAY_NUMERIC_HASH)) {
					if (if_packed) {
						zend_jit_if_false(jit, if_packed);
						if_packed = IR_UNUSED;
					}
					if (!op2_loaded) {
						// JIT: hval = Z_LVAL_P(dim);
						h = zend_jit_zval_lval(jit, op2_addr);
					}
					if (packed_loaded) {
						ref = zend_jit_call_2(jit, IR_ADDR,
							zend_jit_const_func_addr(jit, (uintptr_t)_zend_hash_index_find, IR_CONST_FASTCALL_FUNC),
							ht_ref,
							h);
					} else {
						ref = zend_jit_call_2(jit, IR_ADDR,
							zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_index_find, IR_CONST_FASTCALL_FUNC),
							ht_ref,
							h);
					}
					if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && type == BP_VAR_R) {
						zend_jit_guard(jit, ref, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
					} else if (type == BP_VAR_IS && not_found_exit_addr) {
						zend_jit_guard(jit, ref, zend_jit_const_addr(jit, (uintptr_t)not_found_exit_addr));
//???+				} else if (type == BP_VAR_IS && result_type_guard) {
//???+					|	jz >7 // NOT_FOUND
					} else {
						if_found = zend_jit_if(jit, ref);
						zend_jit_if_false(jit, if_found);
						zend_jit_array_add(idx_not_found_inputs, zend_jit_end(jit));
						zend_jit_if_true(jit, if_found);
					}
					found_inputs[*found_inputs_count] = zend_jit_end(jit);
					found_vals[*found_inputs_count] = ref;
					(*found_inputs_count)++;
				} else if (packed_loaded) {
					found_inputs[*found_inputs_count] = zend_jit_end(jit);
					found_vals[*found_inputs_count] = ref;
					(*found_inputs_count)++;
				}

				if (idx_not_found_inputs_count) {
					zend_jit_merge_N(jit, idx_not_found_inputs_count, idx_not_found_inputs);
					switch (type) {
						case BP_VAR_R:
							ZEND_ASSERT(JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE);
							// JIT: zend_error(E_WARNING,"Undefined array key " ZEND_LONG_FMT, hval);
							// JIT: retval = &EG(uninitialized_zval);
							zend_jit_set_ex_opline(jit, opline);
							zend_jit_call_1(jit, IR_VOID,
								zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_undefined_offset, IR_CONST_FASTCALL_FUNC),
								zend_jit_fp(jit));
							end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
							break;
						case BP_VAR_IS:
						case BP_VAR_UNSET:
							if (!not_found_exit_addr /*&& !result_type_guard???*/) {
								// JIT: retval = &EG(uninitialized_zval);
								zend_jit_zval_set_type_info(jit, res_addr, IS_NULL);
								end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
							}
							break;
						default:
							ZEND_UNREACHABLE();
					}
                }
				break;
			case BP_VAR_RW:
				if (packed_loaded) {
					if (not_found_exit_addr) {
						found_inputs[*found_inputs_count] = zend_jit_end(jit);
						found_vals[*found_inputs_count] = ref;
						(*found_inputs_count)++;
					} else {
						ir_ref if_def = zend_jit_if(jit, zend_jit_zval_ref_type(jit, ref));
						zend_jit_if_true(jit, if_def);
						found_inputs[*found_inputs_count] = zend_jit_end(jit);
						found_vals[*found_inputs_count] = ref;
						(*found_inputs_count)++;
						zend_jit_if_false_cold(jit, if_def);
						zend_jit_array_add(idx_not_found_inputs, zend_jit_end(jit));
					}
				}
				if (!packed_loaded ||
						!not_found_exit_addr ||
						(op1_info & MAY_BE_ARRAY_NUMERIC_HASH)) {
					if (if_packed) {
						zend_jit_if_false(jit, if_packed);
						if_packed = IR_UNUSED;
						zend_jit_array_add(idx_not_found_inputs, zend_jit_end(jit));
					} if (!packed_loaded) {
						zend_jit_array_add(idx_not_found_inputs, zend_jit_end(jit));
					}

					zend_jit_merge_N(jit, idx_not_found_inputs_count, idx_not_found_inputs);
					if (!op2_loaded) {
						// JIT: hval = Z_LVAL_P(dim);
						h = zend_jit_zval_lval(jit, op2_addr);
					}
					if (packed_loaded) {
						ref = zend_jit_call_2(jit, IR_ADDR,
							zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_hash_index_lookup_rw_no_packed, IR_CONST_FASTCALL_FUNC),
							ht_ref,
							h);
					} else {
						ref = zend_jit_call_2(jit, IR_ADDR,
							zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_hash_index_lookup_rw, IR_CONST_FASTCALL_FUNC),
							ht_ref,
							h);
					}
					if (not_found_exit_addr) {
						if (packed_loaded) {
							zend_jit_guard(jit, ref, zend_jit_const_addr(jit, (uintptr_t)not_found_exit_addr));
						} else {
							zend_jit_guard(jit, ref, zend_jit_const_addr(jit, (uintptr_t)not_found_exit_addr));
						}
					} else {
						if_found = zend_jit_if(jit, ref);
						zend_jit_if_false(jit, if_found);
						end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
						zend_jit_if_true(jit, if_found);
					}
					found_inputs[*found_inputs_count] = zend_jit_end(jit);
					found_vals[*found_inputs_count] = ref;
					(*found_inputs_count)++;
				}
				break;
			case BP_VAR_W:
				if (packed_loaded) {
					ir_ref if_def = zend_jit_if(jit, zend_jit_zval_ref_type(jit, ref));
					zend_jit_if_true(jit, if_def);
					found_inputs[*found_inputs_count] = zend_jit_end(jit);
					found_vals[*found_inputs_count] = ref;
					(*found_inputs_count)++;
					zend_jit_if_false(jit, if_def);
					zend_jit_array_add(idx_not_found_inputs, zend_jit_end(jit));
				}
				if (!(op1_info & MAY_BE_ARRAY_KEY_LONG) || (op1_info & MAY_BE_ARRAY_NUMERIC_HASH) || packed_loaded || bad_packed_key || dim_type == IS_UNDEF) {
					if (if_packed) {
						zend_jit_if_false(jit, if_packed);
						if_packed = IR_UNUSED;
						zend_jit_array_add(idx_not_found_inputs, zend_jit_end(jit));
					} else if (!packed_loaded) {
						zend_jit_array_add(idx_not_found_inputs, zend_jit_end(jit));
					}
					zend_jit_merge_N(jit, idx_not_found_inputs_count, idx_not_found_inputs);
					if (!op2_loaded) {
						// JIT: hval = Z_LVAL_P(dim);
						h = zend_jit_zval_lval(jit, op2_addr);
					}
					ref = zend_jit_call_2(jit, IR_ADDR,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_index_lookup, IR_CONST_FASTCALL_FUNC),
						ht_ref,
						h);
					found_inputs[*found_inputs_count] = zend_jit_end(jit);
					found_vals[*found_inputs_count] = ref;
					(*found_inputs_count)++;
				}
				break;
			default:
				ZEND_UNREACHABLE();
		}
	}

	if (op2_info & MAY_BE_STRING) {
		ir_ref key;

		if (if_type) {
			zend_jit_if_false(jit, if_type);
			if_type = IS_UNUSED;
		}

		if (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_LONG|MAY_BE_STRING))) {
			// JIT: if (EXPECTED(Z_TYPE_P(dim) == IS_STRING))
			if_type = zend_jit_if_zval_type(jit, op2_addr, IS_STRING);
			zend_jit_if_true(jit, if_type);
		}

		// JIT: offset_key = Z_STR_P(dim);
		key = zend_jit_zval_ptr(jit, op2_addr);

		// JIT: retval = zend_hash_find(ht, offset_key);
		switch (type) {
			case BP_JIT_IS:
				if (opline->op2_type != IS_CONST) {
					ir_ref if_num, end1, ref2;

					if_num = zend_jit_if(jit,
						ir_fold2(&jit->ctx, IR_OPT(IR_ULE, IR_BOOL),
							zend_jit_load_at_offset(jit, IR_CHAR,
								key, offsetof(zend_string, val)),
							ir_const_char(&jit->ctx, '9')));
					zend_jit_if_true_cold(jit, if_num);
					ref = zend_jit_call_2(jit, IR_ADDR,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_symtable_find, IR_CONST_FASTCALL_FUNC),
						ht_ref,
						key);
					end1 = zend_jit_end(jit);
					zend_jit_if_false(jit, if_num);
					ref2 = zend_jit_call_2(jit, IR_ADDR,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_find, IR_CONST_FASTCALL_FUNC),
						ht_ref,
						key);
					zend_jit_merge_2(jit, end1, zend_jit_end(jit));
					ref = zend_jit_phi_2(jit, ref, ref2);
				} else {
					ref = zend_jit_call_2(jit, IR_ADDR,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_find_known_hash, IR_CONST_FASTCALL_FUNC),
						ht_ref,
						key);
				}
				if (not_found_exit_addr) {
					zend_jit_guard(jit, ref, zend_jit_const_addr(jit, (uintptr_t)not_found_exit_addr));
				} else {
					if_found = zend_jit_if(jit, ref);
					zend_jit_if_false(jit, if_found);
					end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
					zend_jit_if_true(jit, if_found);
				}
				test_zval_values[test_zval_inputs_count] = ref;
				zend_jit_array_add(test_zval_inputs, zend_jit_end(jit));
				break;
			case BP_VAR_R:
			case BP_VAR_IS:
			case BP_VAR_UNSET:
				if (opline->op2_type != IS_CONST) {
					ir_ref if_num, end1, ref2;

					if_num = zend_jit_if(jit,
						ir_fold2(&jit->ctx, IR_OPT(IR_ULE, IR_BOOL),
							zend_jit_load_at_offset(jit, IR_CHAR,
								key, offsetof(zend_string, val)),
							ir_const_char(&jit->ctx, '9')));
					zend_jit_if_true_cold(jit, if_num);
					ref = zend_jit_call_2(jit, IR_ADDR,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_symtable_find, IR_CONST_FASTCALL_FUNC),
						ht_ref,
						key);
					end1 = zend_jit_end(jit);
					zend_jit_if_false(jit, if_num);
					ref2 = zend_jit_call_2(jit, IR_ADDR,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_find, IR_CONST_FASTCALL_FUNC),
						ht_ref,
						key);
					zend_jit_merge_2(jit, end1, zend_jit_end(jit));
					ref = zend_jit_phi_2(jit, ref, ref2);
				} else {
					ref = zend_jit_call_2(jit, IR_ADDR,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_find_known_hash, IR_CONST_FASTCALL_FUNC),
						ht_ref,
						key);
				}
				if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && type == BP_VAR_R) {
					zend_jit_guard(jit, ref, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				} else if (type == BP_VAR_IS && not_found_exit_addr) {
					zend_jit_guard(jit, ref, zend_jit_const_addr(jit, (uintptr_t)not_found_exit_addr));
//???+			} else if (type == BP_VAR_IS && result_type_guard) {
//???+				|	jz >7 // NOT_FOUND
				} else {
					if_found = zend_jit_if(jit, ref);
					switch (type) {
						case BP_VAR_R:
							zend_jit_if_false_cold(jit, if_found);
							// JIT: zend_error(E_WARNING, "Undefined array key \"%s\"", ZSTR_VAL(offset_key));
							zend_jit_set_ex_opline(jit, opline);
							zend_jit_call_1(jit, IR_VOID,
								zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_undefined_key, IR_CONST_FASTCALL_FUNC),
								zend_jit_fp(jit));
							end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
							break;
						case BP_VAR_IS:
						case BP_VAR_UNSET:
							zend_jit_if_false(jit, if_found);
							// JIT: retval = &EG(uninitialized_zval);
							zend_jit_zval_set_type_info(jit, res_addr, IS_NULL);
							end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
							break;
						default:
							ZEND_UNREACHABLE();
					}
					zend_jit_if_true(jit, if_found);
				}
				found_inputs[*found_inputs_count] = zend_jit_end(jit);
				found_vals[*found_inputs_count] = ref;
				(*found_inputs_count)++;
				break;
			case BP_VAR_RW:
				if (opline->op2_type != IS_CONST) {
					ref = zend_jit_call_2(jit, IR_ADDR,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_symtable_lookup_rw, IR_CONST_FASTCALL_FUNC),
						ht_ref,
						key);
				} else {
					ref = zend_jit_call_2(jit, IR_ADDR,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_hash_lookup_rw, IR_CONST_FASTCALL_FUNC),
						ht_ref,
						key);
				}
				if (not_found_exit_addr) {
					zend_jit_guard(jit, ref, zend_jit_const_addr(jit, (uintptr_t)not_found_exit_addr));
				} else {
					if_found = zend_jit_if(jit, ref);
					zend_jit_if_false(jit, if_found);
					end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
					zend_jit_if_true(jit, if_found);
				}
				found_inputs[*found_inputs_count] = zend_jit_end(jit);
				found_vals[*found_inputs_count] = ref;
				(*found_inputs_count)++;
				break;
			case BP_VAR_W:
				if (opline->op2_type != IS_CONST) {
					ref = zend_jit_call_2(jit, IR_ADDR,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_symtable_lookup_w, IR_CONST_FASTCALL_FUNC),
						ht_ref,
						key);
				} else {
					ref = zend_jit_call_2(jit, IR_ADDR,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_lookup, IR_CONST_FASTCALL_FUNC),
						ht_ref,
						key);
				}
				found_inputs[*found_inputs_count] = zend_jit_end(jit);
				found_vals[*found_inputs_count] = ref;
				(*found_inputs_count)++;
				break;
			default:
				ZEND_UNREACHABLE();
		}
	}

	if (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_LONG|MAY_BE_STRING))) {
	    if (if_type) {
			zend_jit_if_false_cold(jit, if_type);
			if_type = IS_UNDEF;
		}
		if (type != BP_VAR_RW) {
			zend_jit_set_ex_opline(jit, opline);
		}
		ref = zend_jit_zval_addr(jit, op2_addr);
		switch (type) {
			case BP_VAR_R:
				zend_jit_call_3(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_dim_r_helper, IR_CONST_FASTCALL_FUNC),
					ht_ref,
					ref,
					zend_jit_zval_addr(jit, res_addr));
				end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
				break;
			case BP_JIT_IS:
				ref = zend_jit_call_2(jit, IR_I32,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_dim_isset_helper, IR_CONST_FASTCALL_FUNC),
					ht_ref,
					ref);
				if (not_found_exit_addr) {
					zend_jit_guard(jit, ref, zend_jit_const_addr(jit, (uintptr_t)not_found_exit_addr));
					found_inputs[*found_inputs_count] = zend_jit_end(jit);
					(*found_inputs_count)++;
				} else if (found_exit_addr) {
					zend_jit_guard_not(jit, ref, zend_jit_const_addr(jit, (uintptr_t)found_exit_addr));
					end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
				} else {
					if_found = zend_jit_if(jit, ref);
					zend_jit_if_true(jit, if_found);
					found_inputs[*found_inputs_count] = zend_jit_end(jit);
					(*found_inputs_count)++;
					zend_jit_if_false(jit, if_found);
					end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
				}
				break;
			case BP_VAR_IS:
			case BP_VAR_UNSET:
				zend_jit_call_3(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_dim_is_helper, IR_CONST_FASTCALL_FUNC),
					ht_ref,
					ref,
					zend_jit_zval_addr(jit, res_addr));
				end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
				break;
			case BP_VAR_RW:
				ref = zend_jit_call_2(jit, IR_ADDR,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_dim_rw_helper, IR_CONST_FASTCALL_FUNC),
					ht_ref,
					ref);
				if_found = zend_jit_if(jit, ref);
				zend_jit_if_true(jit, if_found);
				found_inputs[*found_inputs_count] = zend_jit_end(jit);
				found_vals[*found_inputs_count] = ref;
				(*found_inputs_count)++;
				zend_jit_if_false(jit, if_found);
				end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
				break;
			case BP_VAR_W:
				ref = zend_jit_call_2(jit, IR_ADDR,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_dim_w_helper, IR_CONST_FASTCALL_FUNC),
					ht_ref,
					ref);
				if_found = zend_jit_if(jit, ref);
				zend_jit_if_true(jit, if_found);
				found_inputs[*found_inputs_count] = zend_jit_end(jit);
				found_vals[*found_inputs_count] = ref;
				(*found_inputs_count)++;
				zend_jit_if_false(jit, if_found);
				end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
				break;
			default:
				ZEND_UNREACHABLE();
		}
	}

	if (type == BP_JIT_IS && (op1_info & MAY_BE_ARRAY) && (op2_info & (MAY_BE_LONG|MAY_BE_STRING))) {
		zend_jit_merge_N(jit, test_zval_inputs_count, test_zval_inputs);
		ref = zend_jit_phi_N(jit, test_zval_inputs_count, test_zval_values);

		if (op1_info & MAY_BE_ARRAY_OF_REF) {
			ref = zend_jit_zval_ref_deref(jit, ref);
		}
		cond = ir_fold2(&jit->ctx, IR_OPT(IR_GT, IR_BOOL), zend_jit_zval_ref_type(jit, ref), ir_const_u8(&jit->ctx, IS_NULL));
		if (not_found_exit_addr) {
			zend_jit_guard(jit, cond, zend_jit_const_addr(jit, (uintptr_t)not_found_exit_addr));
			found_inputs[(*found_inputs_count)++] = zend_jit_end(jit);
		} else if (found_exit_addr) {
			zend_jit_guard_not(jit, cond, zend_jit_const_addr(jit, (uintptr_t)found_exit_addr));
			end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
		} else {
			ir_ref if_set = zend_jit_if(jit, cond);
			zend_jit_if_false(jit, if_set);
			end_inputs[(*end_inputs_count)++] = zend_jit_end(jit);
			zend_jit_if_true(jit, if_set);
			found_inputs[(*found_inputs_count)++] = zend_jit_end(jit);
		}
	}

	return 1;
}

static int zend_jit_fetch_dim_read(zend_jit_ctx       *jit,
                                   const zend_op      *opline,
                                   zend_ssa           *ssa,
                                   const zend_ssa_op  *ssa_op,
                                   uint32_t            op1_info,
                                   zend_jit_addr       op1_addr,
                                   bool           op1_avoid_refcounting,
                                   uint32_t            op2_info,
                                   uint32_t            res_info,
                                   zend_jit_addr       res_addr,
                                   uint8_t             dim_type)
{
	zend_jit_addr orig_op1_addr, op2_addr;
	const void *exit_addr = NULL;
	const void *not_found_exit_addr = NULL;
	bool result_type_guard = 0;
	bool result_avoid_refcounting = 0;
	uint32_t may_be_string = (opline->opcode != ZEND_FETCH_LIST_R) ? MAY_BE_STRING : 0;
	int may_throw = 0;
	ir_ref if_type = IR_UNUSED;
	ir_ref end_inputs[10];
	uint32_t end_inputs_count = 0;

	orig_op1_addr = OP1_ADDR();
	op2_addr = OP2_ADDR();

	if (opline->opcode != ZEND_FETCH_DIM_IS
	 && JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
		int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
		exit_addr = zend_jit_trace_get_exit_addr(exit_point);
		if (!exit_addr) {
			return 0;
		}
	}

	if ((res_info & MAY_BE_GUARD)
	 && JIT_G(current_frame)
	 && (op1_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_ARRAY) {

		if (!(op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF) - (MAY_BE_STRING|MAY_BE_LONG)))) {
			result_type_guard = 1;
			res_info &= ~MAY_BE_GUARD;
			ssa->var_info[ssa_op->result_def].type &= ~MAY_BE_GUARD;
		}

		if ((opline->result_type & (IS_VAR|IS_TMP_VAR))
		 && (opline->opcode == ZEND_FETCH_LIST_R
		  || !(opline->op1_type & (IS_VAR|IS_TMP_VAR))
		  || op1_avoid_refcounting)
		 && (res_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))
		 && (ssa_op+1)->op1_use == ssa_op->result_def
		 && !(op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF) - (MAY_BE_STRING|MAY_BE_LONG)))
		 && zend_jit_may_avoid_refcounting(opline+1, res_info)) {
			result_avoid_refcounting = 1;
			ssa->var_info[ssa_op->result_def].avoid_refcounting = 1;
		}

		if (opline->opcode == ZEND_FETCH_DIM_IS
		 && !(res_info & MAY_BE_NULL)) {
			uint32_t flags = 0;
			uint32_t old_op1_info = 0;
			uint32_t old_info;
			zend_jit_trace_stack *stack = JIT_G(current_frame)->stack;
			int32_t exit_point;

			if (opline->opcode != ZEND_FETCH_LIST_R
			 && (opline->op1_type & (IS_VAR|IS_TMP_VAR))
			 && !op1_avoid_refcounting) {
				flags |= ZEND_JIT_EXIT_FREE_OP1;
			}
			if ((opline->op2_type & (IS_VAR|IS_TMP_VAR))
			 && (op2_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
				flags |= ZEND_JIT_EXIT_FREE_OP2;
			}

			if (op1_avoid_refcounting) {
				old_op1_info = STACK_INFO(stack, EX_VAR_TO_NUM(opline->op1.var));
				SET_STACK_REG(stack, EX_VAR_TO_NUM(opline->op1.var), ZREG_NONE);
			}

			old_info = STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var));
			SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_NULL, 0);
			SET_STACK_REG_EX(stack, EX_VAR_TO_NUM(opline->result.var), ZREG_NONE, ZREG_TYPE_ONLY);
			exit_point = zend_jit_trace_get_exit_point(opline+1, flags);
			SET_STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var), old_info);
			not_found_exit_addr = zend_jit_trace_get_exit_addr(exit_point);
			if (!not_found_exit_addr) {
				return 0;
			}

			if (op1_avoid_refcounting) {
				SET_STACK_INFO(stack, EX_VAR_TO_NUM(opline->op1.var), old_op1_info);
			}
		}
	}

	if (op1_info & MAY_BE_REF) {
		ir_ref ref = zend_jit_zval_addr(jit, op1_addr);
		ref = zend_jit_zval_ref_deref(jit, ref);
		op1_addr = ZEND_ADDR_REF_ZVAL(ref);
	}

	if (op1_info & MAY_BE_ARRAY) {
		ir_ref ht_ref, ref;
		zend_jit_addr val_addr;
		ir_ref found_inputs[10], found_vals[10];
		uint32_t found_inputs_count = 0;

		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - MAY_BE_ARRAY)) {
			if (exit_addr && !(op1_info & (MAY_BE_OBJECT|may_be_string))) {
				zend_jit_guard_zval_type(jit, op1_addr, IS_ARRAY, exit_addr);
			} else {
				if_type = zend_jit_if_zval_type(jit, op1_addr, IS_ARRAY);
				zend_jit_if_true(jit, if_type);
			}
		}

		ht_ref = zend_jit_zval_ptr(jit, op1_addr);

		if ((op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_LONG|MAY_BE_STRING))) ||
		    (opline->opcode != ZEND_FETCH_DIM_IS && JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE)) {
			may_throw = 1;
		}

		if (!zend_jit_fetch_dimension_address_inner(jit, opline,
				(opline->opcode != ZEND_FETCH_DIM_IS) ? BP_VAR_R : BP_VAR_IS,
				op1_info, op2_info, dim_type, NULL, not_found_exit_addr, exit_addr,
				result_type_guard, ht_ref, &found_inputs_count, found_inputs, found_vals, &end_inputs_count, end_inputs)) {
			return 0;
		}

		if (found_inputs_count) {
			zend_jit_merge_N(jit, found_inputs_count, found_inputs);
			ref = zend_jit_phi_N(jit, found_inputs_count, found_vals);
			val_addr = ZEND_ADDR_REF_ZVAL(ref);

			if (result_type_guard) {
				zend_uchar type = concrete_type(res_info);
				uint32_t flags = 0;

				if (opline->opcode != ZEND_FETCH_LIST_R
				 && (opline->op1_type & (IS_VAR|IS_TMP_VAR))
				 && !op1_avoid_refcounting) {
					flags |= ZEND_JIT_EXIT_FREE_OP1;
				}
				if ((opline->op2_type & (IS_VAR|IS_TMP_VAR))
				 && (op2_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
					flags |= ZEND_JIT_EXIT_FREE_OP2;
				}

				val_addr = zend_jit_guard_fetch_result_type(jit, opline, val_addr, type,
					(op1_info & MAY_BE_ARRAY_OF_REF) != 0, flags, op1_avoid_refcounting);
				if (!val_addr) {
					return 0;
				}

				// ZVAL_COPY
				zend_jit_copy_zval(jit, res_addr, -1, val_addr, res_info, !result_avoid_refcounting);
				if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
				} else if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, res_info)) {
					return 0;
				}
			} else if (op1_info & MAY_BE_ARRAY_OF_REF) {
				// ZVAL_COPY_DEREF
				ir_ref type_info = zend_jit_zval_type_info(jit, val_addr);
				if (!zend_jit_zval_copy_deref(jit, res_addr, val_addr, type_info)) {
					return 0;
				}
			} else  {
				// ZVAL_COPY
				zend_jit_copy_zval(jit, res_addr, -1, val_addr, res_info, 1);
			}

			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	}

	if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_ARRAY)) {
		if (if_type) {
			zend_jit_if_false_cold(jit, if_type);
			if_type = IS_UNDEF;
		}

		if (opline->opcode != ZEND_FETCH_LIST_R && (op1_info & MAY_BE_STRING)) {
			ir_ref str_ref;

			may_throw = 1;
			if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_ARRAY|MAY_BE_STRING))) {
				if (exit_addr && !(op1_info & MAY_BE_OBJECT)) {
					zend_jit_guard_zval_type(jit, op1_addr, IS_STRING, exit_addr);
				} else {
					if_type = zend_jit_if_zval_type(jit, op1_addr, IS_STRING);
					zend_jit_if_true(jit, if_type);
				}
			}
			zend_jit_set_ex_opline(jit, opline);
			str_ref = zend_jit_zval_ptr(jit, op1_addr);
			if (opline->opcode != ZEND_FETCH_DIM_IS) {
				ir_ref ref;

				if ((op2_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_GUARD)) == MAY_BE_LONG) {
					ref = zend_jit_call_2(jit, IR_ADDR,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_dim_str_offset_r_helper, IR_CONST_FASTCALL_FUNC),
						str_ref,
						zend_jit_zval_lval(jit, op2_addr));
				} else {
					ref = zend_jit_call_2(jit, IR_ADDR,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_dim_str_r_helper, IR_CONST_FASTCALL_FUNC),
						str_ref,
						zend_jit_zval_addr(jit, op2_addr));
				}
				zend_jit_zval_set_ptr(jit, res_addr, ref);
				zend_jit_zval_set_type_info(jit, res_addr, IS_STRING);
			} else {
				zend_jit_call_3(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_dim_str_is_helper, IR_CONST_FASTCALL_FUNC),
					str_ref,
					zend_jit_zval_addr(jit, op2_addr),
					zend_jit_zval_addr(jit, res_addr));
			}
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}

		if (op1_info & MAY_BE_OBJECT) {
			ir_ref arg2;

			if (if_type) {
				zend_jit_if_false_cold(jit, if_type);
				if_type = IS_UNDEF;
			}

			may_throw = 1;
			if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_ARRAY|MAY_BE_OBJECT|may_be_string))) {
				if (exit_addr) {
					zend_jit_guard_zval_type(jit, op1_addr, IS_OBJECT, exit_addr);
				} else {
					if_type = zend_jit_if_zval_type(jit, op1_addr, IS_OBJECT);
					zend_jit_if_true(jit, if_type);
				}
			}

			zend_jit_set_ex_opline(jit, opline);
			if (opline->op2_type == IS_CONST && Z_EXTRA_P(RT_CONSTANT(opline, opline->op2)) == ZEND_EXTRA_VALUE) {
				ZEND_ASSERT(Z_MODE(op2_addr) == IS_CONST_ZVAL);
				arg2 = zend_jit_const_addr(jit, (uintptr_t)(Z_ZV(op2_addr)+1));
			} else {
				arg2 = zend_jit_zval_addr(jit, op2_addr);
			}

			if (opline->opcode != ZEND_FETCH_DIM_IS) {
				zend_jit_call_3(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_dim_obj_r_helper, IR_CONST_FASTCALL_FUNC),
					zend_jit_zval_addr(jit, op1_addr),
					arg2,
					zend_jit_zval_addr(jit, res_addr));
			} else {
				zend_jit_call_3(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_dim_obj_is_helper, IR_CONST_FASTCALL_FUNC),
					zend_jit_zval_addr(jit, op1_addr),
					arg2,
					zend_jit_zval_addr(jit, res_addr));
			}

			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}

		if ((op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_ARRAY|MAY_BE_OBJECT|may_be_string)))
		 && (!exit_addr || !(op1_info & (MAY_BE_ARRAY|MAY_BE_OBJECT|may_be_string)))) {

			if (if_type) {
				zend_jit_if_false_cold(jit, if_type);
				if_type = IS_UNDEF;
			}

			if ((opline->opcode != ZEND_FETCH_DIM_IS && (op1_info & MAY_BE_UNDEF)) || (op2_info & MAY_BE_UNDEF)) {
				zend_jit_set_ex_opline(jit, opline);
				if (opline->opcode != ZEND_FETCH_DIM_IS && (op1_info & MAY_BE_UNDEF)) {
					may_throw = 1;
					zend_jit_type_check_undef(jit, zend_jit_zval_type(jit, op1_addr), opline->op1.var, NULL, 0);
				}

				if (op2_info & MAY_BE_UNDEF) {
					may_throw = 1;
					zend_jit_type_check_undef(jit, zend_jit_zval_type(jit, op2_addr), opline->op2.var, NULL, 0);
				}
			}

			if (opline->opcode != ZEND_FETCH_DIM_IS && opline->opcode != ZEND_FETCH_LIST_R) {
				ir_ref ref;

				may_throw = 1;
				if ((op1_info & MAY_BE_UNDEF) || (op2_info & MAY_BE_UNDEF)) {
					ref = zend_jit_zval_addr(jit, orig_op1_addr);
				} else {
					zend_jit_set_ex_opline(jit, opline);
					ref = zend_jit_zval_addr(jit, op1_addr);
				}
				zend_jit_call_1(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_invalid_array_access, IR_CONST_FASTCALL_FUNC),
					ref);
			}

			zend_jit_zval_set_type_info(jit, res_addr, IS_NULL);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	}

	if (end_inputs_count) {
		zend_jit_merge_N(jit, end_inputs_count, end_inputs);

#ifdef ZEND_JIT_USE_RC_INFERENCE
		if ((opline->op2_type & (IS_TMP_VAR|IS_VAR)) && (op1_info & MAY_BE_OBJECT)) {
			/* Magic offsetGet() may increase refcount of the key */
			op2_info |= MAY_BE_RCN;
	}
#endif

		if (opline->op2_type & (IS_TMP_VAR|IS_VAR)) {
			if ((op2_info & MAY_HAVE_DTOR) && (op2_info & MAY_BE_RC1)) {
				may_throw = 1;
			}
			zend_jit_free_op(jit,  opline->op2_type, opline->op2, op2_info, opline);
		}
		if (opline->opcode != ZEND_FETCH_LIST_R && !op1_avoid_refcounting) {
			if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
				if ((op1_info & MAY_HAVE_DTOR) && (op1_info & MAY_BE_RC1)) {
					may_throw = 1;
				}
				zend_jit_free_op(jit,  opline->op1_type, opline->op1, op1_info, opline);
			}
		}

		if (may_throw) {
			zend_jit_check_exception(jit);
		}
	} else if (op1_info & (MAY_BE_ANY|MAY_BE_UNDEF)) {
		zend_jit_begin(jit, IR_UNUSED); /* unreachable tail */
	}

	return 1;
}

static int zend_jit_separate_array(zend_jit_ctx *jit,
                                   zend_jit_addr var_addr,
                                   uint32_t      var_info)
{
	if (RC_MAY_BE_N(var_info)) {
		ir_ref ref = zend_jit_zval_ptr(jit, var_addr);

//???+		if (Z_MODE(addr) != IS_MEM_ZVAL || Z_REG(addr) != ZREG_FP) {
			ir_ref end1 = IR_UNUSED;

			if (RC_MAY_BE_1(var_info)) {
				ir_ref if_refcount_1 = zend_jit_if(jit,
					ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
						zend_jit_gc_refcount(jit, ref),
						ir_const_u32(&jit->ctx, 1)));
				zend_jit_if_true(jit, if_refcount_1);
				end1 = zend_jit_end(jit);
				zend_jit_if_false(jit, if_refcount_1);
			}
			zend_jit_call_1(jit, IR_VOID, //??? IR_ADDR,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_zval_array_dup, IR_CONST_FASTCALL_FUNC),
				zend_jit_zval_addr(jit, var_addr));
			if (end1) {
				zend_jit_merge_2(jit, end1, zend_jit_end(jit));
			}
#if 0 //???+
		} else {
			if (RC_MAY_BE_1(var_info)) {
|				cmp dword [FCARG1a], 1 // if (GC_REFCOUNT() > 1)
				if (cold) {
|					ja >1
|.cold_code
|1:
				} else {
|					jbe >2
				}
			}
|			IF_NOT_ZVAL_REFCOUNTED addr, >1
|			GC_DELREF FCARG1a
|1:
|			EXT_CALL zend_array_dup, r0
|			SET_ZVAL_PTR addr, r0
|			SET_ZVAL_TYPE_INFO addr, IS_ARRAY_EX
|			mov FCARG1a, r0
			if (RC_MAY_BE_1(var_info)) {
				if (cold) {
|					jmp >2
|.code
				}
			}
|2:
		}
#endif
	}
	return 1;
}


static zend_jit_addr zend_jit_prepare_array_update(zend_jit_ctx   *jit,
                                                   const zend_op  *opline,
                                                   uint32_t        op1_info,
                                                   zend_jit_addr   op1_addr,
                                                   ir_ref         *if_type,
                                                   int            *may_throw)
{
	ir_ref array_inputs[4];
	uint32_t array_inputs_count = 0;
	ir_ref ref;

	ref = zend_jit_zval_addr(jit, op1_addr);
	if (op1_info & MAY_BE_REF) {
		ir_ref if_reference, if_array, ref2, ref3, end1, array_ref_path;

		*may_throw = 1;
		if_reference = zend_jit_if_zval_type(jit, op1_addr, IS_REFERENCE);
		zend_jit_if_false(jit, if_reference);
		end1 = zend_jit_end(jit);
		zend_jit_if_true_cold(jit, if_reference);
		ref2 = zend_jit_addr_offset(jit,
			zend_jit_zval_ptr(jit, op1_addr),
			offsetof(zend_reference, val));
		if_array = zend_jit_if_zval_ref_type(jit, ref2, ir_const_u8(&jit->ctx, IS_ARRAY));
		zend_jit_if_true(jit, if_array);
		array_ref_path = zend_jit_end(jit);
		zend_jit_if_false_cold(jit, if_array);
		if (opline->opcode != ZEND_FETCH_DIM_RW && opline->opcode != ZEND_ASSIGN_DIM_OP) {
			zend_jit_set_ex_opline(jit, opline);
		}
		ref3 = zend_jit_call_1(jit, IR_ADDR,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_prepare_assign_dim_ref, IR_CONST_FASTCALL_FUNC),
			ref);
		zend_jit_guard(jit, ref3, zend_jit_stub_addr(jit, jit_stub_exception_handler_undef));

		// TODO: avoid double check for IS_ARRAy for array_ref_path ???
		zend_jit_merge_3(jit, end1, array_ref_path, zend_jit_end(jit));
		ref = zend_jit_phi_3(jit, ref, ref2, ref3);
		op1_addr = ZEND_ADDR_REF_ZVAL(ref);
	}

	if (op1_info & MAY_BE_ARRAY) {
		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - MAY_BE_ARRAY)) {
			*if_type = zend_jit_if_zval_type(jit, op1_addr, IS_ARRAY);
			zend_jit_if_true(jit, *if_type);
		}
		zend_jit_separate_array(jit, op1_addr, op1_info);
		if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL)) {
			zend_jit_array_add(array_inputs, zend_jit_end(jit));
		}
	}

	if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL)) {
		if (*if_type) {
			zend_jit_if_false(jit, *if_type);
			*if_type = IR_UNUSED;
		}
		if (op1_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_ARRAY))) {
			*if_type = zend_jit_if(jit,
				ir_fold2(&jit->ctx, IR_OPT(IR_LE, IR_BOOL),
					zend_jit_zval_type(jit, op1_addr),
					ir_const_u8(&jit->ctx, IS_NULL)));
			zend_jit_if_true(jit, *if_type);
		}
		if ((op1_info & MAY_BE_UNDEF)
		 && (opline->opcode == ZEND_FETCH_DIM_RW || opline->opcode == ZEND_ASSIGN_DIM_OP)) {
			ir_ref end1 = IR_UNUSED;

			*may_throw = 1;
			if (op1_info & MAY_BE_NULL) {
				ir_ref if_def = zend_jit_if(jit, zend_jit_zval_type(jit, op1_addr));
				zend_jit_if_true(jit, if_def);
				end1 = zend_jit_end(jit);
				zend_jit_if_false(jit, if_def);
			}
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_undefined_op_helper, IR_CONST_FASTCALL_FUNC),
				ir_const_u32(&jit->ctx, opline->op1.var));
			if (end1) {
				zend_jit_merge_2(jit, end1, zend_jit_end(jit));
			}
		}
		// JIT: ZVAL_ARR(container, zend_new_array(8));
		ref = zend_jit_call_0(jit, IR_ADDR,
			zend_jit_const_func_addr(jit, (uintptr_t)_zend_new_array_0, IR_CONST_FASTCALL_FUNC));
		zend_jit_zval_set_ptr(jit, op1_addr, ref);
		zend_jit_zval_set_type_info(jit, op1_addr, IS_ARRAY_EX);
		if (array_inputs_count) {
			zend_jit_array_add(array_inputs, zend_jit_end(jit));
		}
	}

	if (array_inputs_count) {
		zend_jit_merge_N(jit, array_inputs_count, array_inputs);
	}

	return op1_addr;
}

static int zend_jit_fetch_dim(zend_jit_ctx   *jit,
                              const zend_op  *opline,
                              uint32_t        op1_info,
                              zend_jit_addr   op1_addr,
                              uint32_t        op2_info,
                              zend_jit_addr   res_addr,
                              uint8_t         dim_type)
{
	zend_jit_addr op2_addr;
	int may_throw = 0;
	ir_ref end_inputs[10];
	uint32_t end_inputs_count = 0;
	ir_ref ref, if_type = IR_UNUSED;

	op2_addr = (opline->op2_type != IS_UNUSED) ? OP2_ADDR() : 0;

	if (opline->opcode == ZEND_FETCH_DIM_RW) {
		zend_jit_set_ex_opline(jit, opline);
	}

	op1_addr = zend_jit_prepare_array_update(jit, opline, op1_info, op1_addr, &if_type, &may_throw);

	if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_ARRAY)) {
		ir_ref found_inputs[8], found_vals[8], ht_ref;
		uint32_t found_inputs_count = 0;

		ht_ref = zend_jit_zval_ptr(jit, op1_addr);

		if (opline->op2_type == IS_UNUSED) {
			ir_ref if_ok;

			may_throw = 1;
			// JIT:var_ptr = zend_hash_next_index_insert(Z_ARRVAL_P(container), &EG(uninitialized_zval));
			ref = zend_jit_call_2(jit, IR_ADDR,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_next_index_insert, IR_CONST_FASTCALL_FUNC),
				ht_ref,
				ZEND_JIT_EG_ADDR(uninitialized_zval));

			// JIT: if (UNEXPECTED(!var_ptr)) {
			if_ok = zend_jit_if(jit, ref);
			zend_jit_if_false_cold(jit, if_ok);
			if (opline->opcode != ZEND_FETCH_DIM_RW) {
				zend_jit_set_ex_opline(jit, opline);
			}
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_cannot_add_element, IR_CONST_FASTCALL_FUNC),
				zend_jit_fp(jit));
			zend_jit_array_add(end_inputs, zend_jit_end(jit));

			zend_jit_if_true(jit, if_ok);
			zend_jit_zval_set_ptr(jit, res_addr, ref);
			zend_jit_zval_set_type_info(jit, res_addr, IS_INDIRECT);

			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		} else {
			uint32_t type;

			switch (opline->opcode) {
				case ZEND_FETCH_DIM_W:
				case ZEND_FETCH_LIST_W:
					type = BP_VAR_W;
					break;
				case ZEND_FETCH_DIM_RW:
					may_throw = 1;
					type = BP_VAR_RW;
					break;
				case ZEND_FETCH_DIM_UNSET:
					type = BP_VAR_UNSET;
					break;
				default:
					ZEND_UNREACHABLE();
			}

			if (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_LONG|MAY_BE_STRING))) {
				may_throw = 1;
			}
			if (!zend_jit_fetch_dimension_address_inner(jit, opline, type, op1_info, op2_info, dim_type, NULL, NULL, NULL,
					0, ht_ref, &found_inputs_count, found_inputs, found_vals, &end_inputs_count, end_inputs)) {
				return 0;
			}

			if (type == BP_VAR_RW || (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_LONG|MAY_BE_STRING)))) {
				if (end_inputs_count) {
					zend_jit_merge_N(jit, end_inputs_count, end_inputs);
					zend_jit_zval_set_type_info(jit, res_addr, IS_NULL);
					end_inputs_count = 0;
					zend_jit_array_add(end_inputs, zend_jit_end(jit));
				}
			} else {
				ZEND_ASSERT(end_inputs_count == 0);
			}

			if (found_inputs_count) {
				zend_jit_merge_N(jit, found_inputs_count, found_inputs);
				ref = zend_jit_phi_N(jit, found_inputs_count, found_vals);
				zend_jit_zval_set_ptr(jit, res_addr, ref);
				zend_jit_zval_set_type_info(jit, res_addr, IS_INDIRECT);
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}

		}
	}

	if (op1_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_ARRAY))) {
		ir_ref arg2;

		may_throw = 1;

		if (if_type) {
			zend_jit_if_false(jit, if_type);
			if_type = IR_UNUSED;
		}

		if (opline->opcode != ZEND_FETCH_DIM_RW) {
			zend_jit_set_ex_opline(jit, opline);
		}

	    if (opline->op2_type == IS_UNUSED) {
			arg2 = IR_NULL;
		} else if (opline->op2_type == IS_CONST && Z_EXTRA_P(RT_CONSTANT(opline, opline->op2)) == ZEND_EXTRA_VALUE) {
			ZEND_ASSERT(Z_MODE(op2_addr) == IS_CONST_ZVAL);
			arg2 = zend_jit_const_addr(jit, (uintptr_t)(Z_ZV(op2_addr) + 1));
		} else {
			arg2 = zend_jit_zval_addr(jit, op2_addr);
		}

		switch (opline->opcode) {
			case ZEND_FETCH_DIM_W:
			case ZEND_FETCH_LIST_W:
				zend_jit_call_3(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_dim_obj_w_helper, IR_CONST_FASTCALL_FUNC),
					zend_jit_zval_addr(jit, op1_addr),
					arg2,
					zend_jit_zval_addr(jit, res_addr));
				break;
			case ZEND_FETCH_DIM_RW:
				zend_jit_call_3(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_dim_obj_rw_helper, IR_CONST_FASTCALL_FUNC),
					zend_jit_zval_addr(jit, op1_addr),
					arg2,
					zend_jit_zval_addr(jit, res_addr));
				break;
//			case ZEND_FETCH_DIM_UNSET:
//				|	EXT_CALL zend_jit_fetch_dim_obj_unset_helper, r0
//				break;
			default:
				ZEND_UNREACHABLE();
			}

		if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_ARRAY)) {
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	}

#ifdef ZEND_JIT_USE_RC_INFERENCE
	if ((opline->op2_type & (IS_TMP_VAR|IS_VAR)) && (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_ARRAY|MAY_BE_OBJECT))) {
		/* ASSIGN_DIM may increase refcount of the key */
		op2_info |= MAY_BE_RCN;
	}
#endif

	if ((opline->op2_type & (IS_TMP_VAR|IS_VAR))
	 && (op2_info & MAY_HAVE_DTOR)
	 && (op2_info & MAY_BE_RC1)) {
		may_throw = 1;
	}

	if (end_inputs_count) {
		zend_jit_merge_N(jit, end_inputs_count, end_inputs);
	}

	zend_jit_free_op(jit, opline->op2_type, opline->op2, op2_info, opline);

	if (may_throw) {
		zend_jit_check_exception(jit);
	}

	return 1;
}

static int zend_jit_isset_isempty_dim(zend_jit_ctx   *jit,
                                      const zend_op  *opline,
                                      uint32_t        op1_info,
                                      zend_jit_addr   op1_addr,
                                      bool       op1_avoid_refcounting,
                                      uint32_t        op2_info,
                                      uint8_t         dim_type,
                                      int             may_throw,
                                      zend_uchar      smart_branch_opcode,
                                      uint32_t        target_label,
                                      uint32_t        target_label2,
                                      const void     *exit_addr)
{
	zend_jit_addr op2_addr, res_addr;
	ir_ref if_type = IR_UNUSED;
	ir_ref false_inputs[8], true_inputs[8], end_inputs[4];
	uint32_t false_inputs_count = 0;
	uint32_t true_inputs_count = 0;
	uint32_t end_inputs_count = 0;

	// TODO: support for empty() ???
	ZEND_ASSERT(!(opline->extended_value & ZEND_ISEMPTY));

	op2_addr = OP2_ADDR();
	res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var);

	if (op1_info & MAY_BE_REF) {
		ir_ref ref = zend_jit_zval_addr(jit, op1_addr);
		ref = zend_jit_zval_ref_deref(jit, ref);
		op1_addr = ZEND_ADDR_REF_ZVAL(ref);
	}

	if (op1_info & MAY_BE_ARRAY) {
		const void *found_exit_addr = NULL;
		const void *not_found_exit_addr = NULL;
		ir_ref ht_ref;

		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - MAY_BE_ARRAY)) {
			if_type = zend_jit_if_zval_type(jit, op1_addr, IS_ARRAY);
			zend_jit_if_true(jit, if_type);
		}

		ht_ref = zend_jit_zval_ptr(jit, op1_addr);

		if (exit_addr
		 && !(op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_ARRAY))
		 && !may_throw
		 && (!(opline->op1_type & (IS_TMP_VAR|IS_VAR)) || op1_avoid_refcounting)
		 && (!(opline->op2_type & (IS_TMP_VAR|IS_VAR)) || !(op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG)))) {
			if (smart_branch_opcode == ZEND_JMPNZ) {
				found_exit_addr = exit_addr;
			} else {
				not_found_exit_addr = exit_addr;
			}
		}
		if (!zend_jit_fetch_dimension_address_inner(jit, opline, BP_JIT_IS, op1_info, op2_info, dim_type, found_exit_addr, not_found_exit_addr, NULL,
				0, ht_ref, &true_inputs_count, true_inputs, NULL, &false_inputs_count, false_inputs)) {
			return 0;
		}

		if (found_exit_addr) {
			zend_jit_merge_N(jit, false_inputs_count, false_inputs);
			return 1;
		} else if (not_found_exit_addr) {
			zend_jit_merge_N(jit, true_inputs_count, true_inputs);
			return 1;
		}
	}

	if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_ARRAY)) {
		if (if_type) {
			zend_jit_if_false(jit, if_type);
			if_type = IR_UNUSED;
		}

		if (op1_info & (MAY_BE_STRING|MAY_BE_OBJECT)) {
			ir_ref ref, arg1, arg2, if_true;

			zend_jit_set_ex_opline(jit, opline);
			arg1 = zend_jit_zval_addr(jit, op1_addr);
			if (opline->op2_type == IS_CONST && Z_EXTRA_P(RT_CONSTANT(opline, opline->op2)) == ZEND_EXTRA_VALUE) {
				ZEND_ASSERT(Z_MODE(op2_addr) == IS_CONST_ZVAL);
				arg2 = zend_jit_const_addr(jit, (uintptr_t)(Z_ZV(op2_addr)+1));
			} else {
				arg2 = zend_jit_zval_addr(jit, op2_addr);
			}
			ref = zend_jit_call_2(jit, IR_I32,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_isset_dim_helper, IR_CONST_FASTCALL_FUNC),
				arg1, arg2);
			if_true = zend_jit_if(jit, ref);
			zend_jit_if_true(jit, if_true);
			zend_jit_array_add(true_inputs, zend_jit_end(jit));
			zend_jit_if_false(jit, if_true);
			zend_jit_array_add(false_inputs, zend_jit_end(jit));
		} else {
			if (op2_info & MAY_BE_UNDEF) {
				ir_ref end1 = IR_UNUSED;

				if (op2_info & MAY_BE_ANY) {
					ir_ref if_def = zend_jit_if(jit, zend_jit_zval_type(jit, op2_addr));
					zend_jit_if_true(jit, if_def);
					end1 = zend_jit_end(jit);
					zend_jit_if_false(jit, if_def);
				}
				zend_jit_set_ex_opline(jit, opline);
				zend_jit_call_1(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_undefined_op_helper, IR_CONST_FASTCALL_FUNC),
					ir_const_u32(&jit->ctx, opline->op2.var));
				if (end1) {
					zend_jit_merge_2(jit, end1, zend_jit_end(jit));
				}
			}
			zend_jit_array_add(false_inputs, zend_jit_end(jit));
		}
	}

#ifdef ZEND_JIT_USE_RC_INFERENCE
	if ((opline->op2_type & (IS_TMP_VAR|IS_VAR)) && (op1_info & MAY_BE_OBJECT)) {
		/* Magic offsetExists() may increase refcount of the key */
		op2_info |= MAY_BE_RCN;
	}
#endif

	if (op1_info & (MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_OBJECT)) {
		zend_jit_merge_N(jit, true_inputs_count, true_inputs);

		zend_jit_free_op(jit, opline->op2_type, opline->op2, op2_info, opline);
		if (!op1_avoid_refcounting) {
			zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);
		}
		if (may_throw) {
			zend_jit_check_exception_undef_result(jit, opline);
		}
		if (!(opline->extended_value & ZEND_ISEMPTY)) {
			if (exit_addr) {
				if (smart_branch_opcode == ZEND_JMPNZ) {
					zend_jit_side_exit(jit, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
				} else {
					zend_jit_array_add(end_inputs, zend_jit_end(jit));
				}
			} else if (smart_branch_opcode) {
				if (smart_branch_opcode == ZEND_JMPZ) {
					_zend_jit_add_predecessor_ref(jit, target_label2, jit->b, zend_jit_end(jit));
				} else if (smart_branch_opcode == ZEND_JMPNZ) {
					_zend_jit_add_predecessor_ref(jit, target_label, jit->b, zend_jit_end(jit));
				} else {
					ZEND_UNREACHABLE();
				}
			} else {
				zend_jit_zval_set_type_info(jit, res_addr, IS_TRUE);

				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}
		} else {
			ZEND_ASSERT(0); // TODO: support for empty()
		}
	}

	zend_jit_merge_N(jit, false_inputs_count, false_inputs);
	zend_jit_free_op(jit, opline->op2_type, opline->op2, op2_info, opline);
	if (!op1_avoid_refcounting) {
		zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);
	}
	if (may_throw) {
		zend_jit_check_exception_undef_result(jit, opline);
	}
	if (!(opline->extended_value & ZEND_ISEMPTY)) {
		if (exit_addr) {
			if (smart_branch_opcode == ZEND_JMPZ) {
				zend_jit_side_exit(jit, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}
		} else if (smart_branch_opcode) {
			if (smart_branch_opcode == ZEND_JMPZ) {
				_zend_jit_add_predecessor_ref(jit, target_label, jit->b, zend_jit_end(jit));
			} else if (smart_branch_opcode == ZEND_JMPNZ) {
				_zend_jit_add_predecessor_ref(jit, target_label2, jit->b, zend_jit_end(jit));
			} else {
				ZEND_UNREACHABLE();
			}
		} else {
			zend_jit_zval_set_type_info(jit, res_addr, IS_FALSE);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	} else {
		ZEND_ASSERT(0); // TODO: support for empty()
	}

    if (!exit_addr && smart_branch_opcode) {
		jit->control = IR_UNUSED;
		jit->b = -1;
    } else {
		zend_jit_merge_N(jit, end_inputs_count, end_inputs);
    }

	return 1;
}

static int zend_jit_assign_dim(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, uint32_t op2_info, uint32_t val_info, uint8_t dim_type, int may_throw)
{
	zend_jit_addr op2_addr, op3_addr, res_addr;
	ir_ref if_type = IR_UNUSED;
	ir_ref end_inputs[10];
	uint32_t end_inputs_count = 0;

	op2_addr = (opline->op2_type != IS_UNUSED) ? OP2_ADDR() : 0;
	op3_addr = OP1_DATA_ADDR();
	if (opline->result_type == IS_UNUSED) {
		res_addr = 0;
	} else {
		res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var);
	}

	if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && (val_info & MAY_BE_UNDEF)) {
		int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
		const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

		if (!exit_addr) {
			return 0;
		}

		zend_jit_guard_not_zval_type(jit, op3_addr, IS_UNDEF, exit_addr);

		val_info &= ~MAY_BE_UNDEF;
	}

	op1_addr = zend_jit_prepare_array_update(jit, opline, op1_info, op1_addr, &if_type, &may_throw);

	if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_ARRAY)) {
		ir_ref ht_ref = zend_jit_zval_ptr(jit, op1_addr);

		if (opline->op2_type == IS_UNUSED) {
			uint32_t var_info = MAY_BE_NULL;
			ir_ref if_ok, ref;
			zend_jit_addr var_addr;

			// JIT: var_ptr = zend_hash_next_index_insert(Z_ARRVAL_P(container), &EG(uninitialized_zval));
			ref = zend_jit_call_2(jit, IR_ADDR,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_next_index_insert, IR_CONST_FASTCALL_FUNC),
				ht_ref,
				ZEND_JIT_EG_ADDR(uninitialized_zval));

			// JIT: if (UNEXPECTED(!var_ptr)) {
			if_ok = zend_jit_if(jit, ref);
			zend_jit_if_false_cold(jit, if_ok);

			// JIT: zend_throw_error(NULL, "Cannot add element to the array as the next element is already occupied");
			zend_jit_set_ex_opline(jit, opline);
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_cannot_add_element, IR_CONST_FASTCALL_FUNC),
				zend_jit_fp(jit));

			zend_jit_array_add(end_inputs, zend_jit_end(jit));

			zend_jit_if_true(jit, if_ok);
			var_addr = ZEND_ADDR_REF_ZVAL(ref);
			if (!zend_jit_simple_assign(jit, opline, var_addr, var_info, -1, (opline+1)->op1_type, op3_addr, val_info, res_addr, 0)) {
				return 0;
			}
		} else {
			uint32_t var_info = zend_array_element_type(op1_info, opline->op1_type, 0, 0);
			zend_jit_addr var_addr;
			ir_ref found_inputs[8], found_values[8];
			uint32_t found_inputs_count = 0;
			ir_ref ref;

			if (!zend_jit_fetch_dimension_address_inner(jit, opline, BP_VAR_W, op1_info, op2_info, dim_type, NULL, NULL, NULL,
					0, ht_ref, &found_inputs_count, found_inputs, found_values, &end_inputs_count, end_inputs)) {
				return 0;
			}

			if (op1_info & (MAY_BE_ARRAY_OF_REF|MAY_BE_OBJECT)) {
				var_info |= MAY_BE_REF;
			}
			if (var_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
				var_info |= MAY_BE_RC1;
			}

			zend_jit_merge_N(jit, found_inputs_count, found_inputs);
			ref = zend_jit_phi_N(jit, found_inputs_count, found_values);
			var_addr = ZEND_ADDR_REF_ZVAL(ref);

			// JIT: value = zend_assign_to_variable(variable_ptr, value, OP_DATA_TYPE);
			if (opline->op1_type == IS_VAR) {
				ZEND_ASSERT(opline->result_type == IS_UNUSED);
				if (!zend_jit_assign_to_variable_call(jit, opline, var_addr, var_addr, var_info, -1, (opline+1)->op1_type, op3_addr, val_info, res_addr, 0)) {
					return 0;
				}
			} else {
				if (!zend_jit_assign_to_variable(jit, opline, var_addr, var_addr, var_info, -1, (opline+1)->op1_type, op3_addr, val_info, res_addr, 0, 0)) {
					return 0;
				}
			}
		}

		zend_jit_array_add(end_inputs, zend_jit_end(jit));
	}

	if (op1_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_ARRAY))) {
		ir_ref arg2, arg4;

		if (if_type) {
			zend_jit_if_false_cold(jit, if_type);
			if_type = IR_UNUSED;
		}

		zend_jit_set_ex_opline(jit, opline);

	    if (opline->op2_type == IS_UNUSED) {
			arg2 = IR_NULL;
		} else if (opline->op2_type == IS_CONST && Z_EXTRA_P(RT_CONSTANT(opline, opline->op2)) == ZEND_EXTRA_VALUE) {
				ZEND_ASSERT(Z_MODE(op2_addr) == IS_CONST_ZVAL);
			arg2 = zend_jit_const_addr(jit, (uintptr_t)(Z_ZV(op2_addr) + 1));
		} else {
			arg2 = zend_jit_zval_addr(jit, op2_addr);
		}

		if (opline->result_type == IS_UNUSED) {
			arg4 = IR_NULL;
		} else {
			arg4 = zend_jit_zval_addr(jit, res_addr);
		}
		zend_jit_call_4(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_assign_dim_helper, IR_CONST_FASTCALL_FUNC),
			zend_jit_zval_addr(jit, op1_addr),
			arg2,
			zend_jit_zval_addr(jit, op3_addr),
			arg4);

#ifdef ZEND_JIT_USE_RC_INFERENCE
		if (((opline+1)->op1_type & (IS_TMP_VAR|IS_VAR)) && (val_info & MAY_BE_RC1)) {
			/* ASSIGN_DIM may increase refcount of the value */
			val_info |= MAY_BE_RCN;
		}
#endif

		zend_jit_free_op(jit, (opline+1)->op1_type, (opline+1)->op1, val_info, NULL);

		zend_jit_array_add(end_inputs, zend_jit_end(jit));
	}

#ifdef ZEND_JIT_USE_RC_INFERENCE
	if ((opline->op2_type & (IS_TMP_VAR|IS_VAR)) && (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_ARRAY|MAY_BE_OBJECT))) {
		/* ASSIGN_DIM may increase refcount of the key */
		op2_info |= MAY_BE_RCN;
	}
#endif

	zend_jit_merge_N(jit, end_inputs_count, end_inputs);
	zend_jit_free_op(jit, opline->op2_type, opline->op2, op2_info, opline);

	if (may_throw) {
		zend_jit_check_exception(jit);
	}

	return 1;
}

static int zend_jit_assign_dim_op(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, uint32_t op1_def_info, zend_jit_addr op1_addr, uint32_t op2_info, uint32_t op1_data_info, zend_ssa_range *op1_data_range, uint8_t dim_type, int may_throw)
{
	zend_jit_addr op2_addr, op3_addr, var_addr = IS_UNUSED;
	const void *not_found_exit_addr = NULL;
	uint32_t var_info = MAY_BE_NULL;
	ir_ref if_type = IS_UNUSED;
	ir_ref end_inputs[10];
	uint32_t end_inputs_count = 0;
	bool emit_fast_path = 1;

	ZEND_ASSERT(opline->result_type == IS_UNUSED);

	op2_addr = (opline->op2_type != IS_UNUSED) ? OP2_ADDR() : 0;
	op3_addr = OP1_DATA_ADDR();

	zend_jit_set_ex_opline(jit, opline);

	op1_addr = zend_jit_prepare_array_update(jit, opline, op1_info, op1_addr, &if_type, &may_throw);

	if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_ARRAY)) {
		uint32_t var_def_info = zend_array_element_type(op1_def_info, opline->op1_type, 1, 0);
		ir_ref ht_ref = zend_jit_zval_ptr(jit, op1_addr);

		if (opline->op2_type == IS_UNUSED) {
			var_info = MAY_BE_NULL;
			ir_ref if_ok, ref;

			// JIT: var_ptr = zend_hash_next_index_insert(Z_ARRVAL_P(container), &EG(uninitialized_zval));
			ref = zend_jit_call_2(jit, IR_ADDR,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_next_index_insert, IR_CONST_FASTCALL_FUNC),
				ht_ref,
				ZEND_JIT_EG_ADDR(uninitialized_zval));

			// JIT: if (UNEXPECTED(!var_ptr)) {
			if_ok = zend_jit_if(jit, ref);
			zend_jit_if_false_cold(jit, if_ok);

			// JIT: zend_throw_error(NULL, "Cannot add element to the array as the next element is already occupied");
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_cannot_add_element, IR_CONST_FASTCALL_FUNC),
				zend_jit_fp(jit));

			zend_jit_array_add(end_inputs, zend_jit_end(jit));

			zend_jit_if_true(jit, if_ok);
			var_addr = ZEND_ADDR_REF_ZVAL(ref);
		} else {
			ir_ref found_inputs[8], found_values[8];
			uint32_t found_inputs_count = 0;
			ir_ref ref;

			var_info = zend_array_element_type(op1_info, opline->op1_type, 0, 0);
			if (op1_info & (MAY_BE_ARRAY_OF_REF|MAY_BE_OBJECT)) {
				var_info |= MAY_BE_REF;
			}
			if (var_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
				var_info |= MAY_BE_RC1;
			}

			if (dim_type != IS_UNKNOWN
			 && dim_type != IS_UNDEF
			 && (op1_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_ARRAY
			 && (op2_info & (MAY_BE_LONG|MAY_BE_STRING))
			 && !(op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_LONG|MAY_BE_STRING)))) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);
				not_found_exit_addr = zend_jit_trace_get_exit_addr(exit_point);
				if (!not_found_exit_addr) {
					return 0;
				}
			}

			if (!zend_jit_fetch_dimension_address_inner(jit, opline, BP_VAR_RW, op1_info, op2_info, dim_type, NULL, not_found_exit_addr, NULL,
					0, ht_ref, &found_inputs_count, found_inputs, found_values, &end_inputs_count, end_inputs)) {
				return 0;
			}

			if (found_inputs_count) {
				zend_jit_merge_N(jit, found_inputs_count, found_inputs);
				ref = zend_jit_phi_N(jit, found_inputs_count, found_values);
				var_addr = ZEND_ADDR_REF_ZVAL(ref);

				if (not_found_exit_addr && dim_type != IS_REFERENCE) {
					zend_jit_guard_zval_type(jit, var_addr, dim_type, not_found_exit_addr);
					var_info = (1 << dim_type) | (var_info & ~(MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF));
				}
				if (var_info & MAY_BE_REF) {
					binary_op_type binary_op = get_binary_op(opline->extended_value);
					ir_ref if_ref, if_typed, noref_path, ref_path, ref, reference, ref2, arg2;

					ref = zend_jit_zval_addr(jit, var_addr);
					if_ref = zend_jit_if_zval_type(jit, var_addr, IS_REFERENCE);
					zend_jit_if_false(jit, if_ref);
					noref_path = zend_jit_end(jit);
					zend_jit_if_true(jit, if_ref);

					reference = zend_jit_zval_ref_ptr(jit, ref);
					ref2 = zend_jit_addr_offset(jit, reference, offsetof(zend_reference, val));
					if_typed = zend_jit_if_typed_ref(jit, reference);
					zend_jit_if_false(jit, if_typed);
					ref_path = zend_jit_end(jit);
					zend_jit_if_true_cold(jit, if_typed);

					arg2 = zend_jit_zval_addr(jit, op3_addr);
					zend_jit_call_3(jit, IR_VOID,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_assign_op_to_typed_ref, IR_CONST_FASTCALL_FUNC),
						reference, arg2, zend_jit_const_func_addr(jit, (uintptr_t)binary_op, IR_CONST_FASTCALL_FUNC));

					zend_jit_array_add(end_inputs, zend_jit_end(jit));

					zend_jit_merge_2(jit, noref_path, ref_path);
					ref = zend_jit_phi_2(jit, ref, ref2);
					var_addr = ZEND_ADDR_REF_ZVAL(ref);
				}
			} else {
				emit_fast_path = 0;
			}
		}

		if (emit_fast_path) {
			uint8_t val_op_type = (opline+1)->op1_type;

			if (val_op_type & (IS_TMP_VAR|IS_VAR)) {
				/* prevent FREE_OP in the helpers */
				val_op_type = IS_CV;
			}

			switch (opline->extended_value) {
				case ZEND_ADD:
				case ZEND_SUB:
				case ZEND_MUL:
				case ZEND_DIV:
					if (!zend_jit_math_helper(jit, opline, opline->extended_value, IS_CV, opline->op1, var_addr, var_info, val_op_type, (opline+1)->op1, op3_addr, op1_data_info, 0, var_addr, var_def_info, var_info,
							1 /* may overflow */, may_throw)) {
						return 0;
					}
					break;
				case ZEND_BW_OR:
				case ZEND_BW_AND:
				case ZEND_BW_XOR:
				case ZEND_SL:
				case ZEND_SR:
				case ZEND_MOD:
					if (!zend_jit_long_math_helper(jit, opline, opline->extended_value,
							IS_CV, opline->op1, var_addr, var_info, NULL,
							val_op_type, (opline+1)->op1, op3_addr, op1_data_info,
							op1_data_range,
							0, var_addr, var_def_info, var_info, may_throw)) {
						return 0;
					}
					break;
				case ZEND_CONCAT:
					if (!zend_jit_concat_helper(jit, opline, IS_CV, opline->op1, var_addr, var_info, val_op_type, (opline+1)->op1, op3_addr, op1_data_info, var_addr,
							may_throw)) {
						return 0;
					}
					break;
				default:
					ZEND_UNREACHABLE();
			}

			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	}

	if (op1_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_ARRAY))) {
		binary_op_type binary_op;
		ir_ref arg2;

		if (if_type) {
			zend_jit_if_false_cold(jit, if_type);
			if_type = IS_UNUSED;
		}

	    if (opline->op2_type == IS_UNUSED) {
			arg2 = IR_NULL;
		} else if (opline->op2_type == IS_CONST && Z_EXTRA_P(RT_CONSTANT(opline, opline->op2)) == ZEND_EXTRA_VALUE) {
			ZEND_ASSERT(Z_MODE(op2_addr) == IS_CONST_ZVAL);
			arg2 = zend_jit_const_addr(jit, (uintptr_t)(Z_ZV(op2_addr) + 1));
		} else {
			arg2 = zend_jit_zval_addr(jit, op2_addr);
		}
		binary_op = get_binary_op(opline->extended_value);
		zend_jit_call_4(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_assign_dim_op_helper, IR_CONST_FASTCALL_FUNC),
			zend_jit_zval_addr(jit, op1_addr),
			arg2,
			zend_jit_zval_addr(jit, op3_addr),
			zend_jit_const_func_addr(jit, (uintptr_t)binary_op, IR_CONST_FASTCALL_FUNC));
		zend_jit_array_add(end_inputs, zend_jit_end(jit));
	}

	if (end_inputs_count) {
		zend_jit_merge_N(jit, end_inputs_count, end_inputs);
	}

	zend_jit_free_op(jit, (opline+1)->op1_type, (opline+1)->op1, op1_data_info, NULL);
	zend_jit_free_op(jit, opline->op2_type, opline->op2, op2_info, NULL);
	if (may_throw) {
		zend_jit_check_exception(jit);
	}

	return 1;
}

static int zend_jit_fe_reset(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info)
{
	zend_jit_addr res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var);

	// JIT: ZVAL_COPY(res, value);
	if (opline->op1_type == IS_CONST) {
		zval *zv = RT_CONSTANT(opline, opline->op1);

		zend_jit_copy_zval_const(jit, res_addr, MAY_BE_ANY, MAY_BE_ANY, zv, 1);
	} else {
		zend_jit_addr op1_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->op1.var);

		zend_jit_copy_zval(jit, res_addr, -1, op1_addr, op1_info, opline->op1_type == IS_CV);
	}

	// JIT: Z_FE_POS_P(res) = 0;
	zend_jit_store_at_offset(jit,
		zend_jit_fp(jit), opline->result.var + offsetof(zval, u2.fe_pos),
		ir_const_u32(&jit->ctx, 0));

	return 1;
}

static int zend_jit_packed_guard(zend_jit_ctx *jit, const zend_op *opline, uint32_t var, uint32_t op_info)
{
	int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_PACKED_GUARD);
	const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);
	zend_jit_addr addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, var);
	ir_ref ref;

	if (!exit_addr) {
		return 0;
	}

	ref = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
		zend_jit_load_at_offset(jit, IR_U32,
			zend_jit_zval_ptr(jit, addr), offsetof(zend_array, u.flags)),
		ir_const_u32(&jit->ctx, HASH_FLAG_PACKED));
	if (op_info & MAY_BE_ARRAY_PACKED) {
		zend_jit_guard(jit, ref,
			zend_jit_const_addr(jit, (uintptr_t)exit_addr));
	} else {
		zend_jit_guard_not(jit, ref,
			zend_jit_const_addr(jit, (uintptr_t)exit_addr));
	}

	return 1;
}

static int zend_jit_fe_fetch(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, uint32_t op2_info, unsigned int target_label, zend_uchar exit_opcode, const void *exit_addr)
{
	zend_jit_addr op1_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->op1.var);
	ir_ref ref, ht_ref, hash_pos_ref, packed_pos_ref, hash_p_ref = IR_UNUSED, packed_p_ref = IR_UNUSED, if_packed = IR_UNUSED;
	ir_ref if_undef_hash = IR_UNUSED, if_undef_packed = IR_UNUSED;
	ir_ref exit_inputs[4];
	uint32_t exit_inputs_count = 0;

	if (!MAY_BE_HASH(op1_info) && !MAY_BE_PACKED(op1_info)) {
		/* empty array */
		if (exit_addr) {
			if (exit_opcode == ZEND_JMP) {
				zend_jit_side_exit(jit,  zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			}
		} else {
			zend_basic_block *bb;

			ZEND_ASSERT(jit->b >= 0);
			bb = &jit->ssa->cfg.blocks[jit->b];
			_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, zend_jit_end(jit));
			jit->control = IR_UNUSED;
			jit->b = -1;
		}
		return 1;
	}

	// JIT: array = EX_VAR(opline->op1.var);
	// JIT: fe_ht = Z_ARRVAL_P(array);
	ht_ref = zend_jit_zval_ptr(jit, op1_addr);

	if (op1_info & MAY_BE_PACKED_GUARD) {
		if (!zend_jit_packed_guard(jit, opline, opline->op1.var, op1_info)) {
			return 0;
		}
	}

	// JIT: pos = Z_FE_POS_P(array);
	hash_pos_ref = packed_pos_ref = zend_jit_load_at_offset(jit, IR_U32,
		zend_jit_fp(jit), opline->op1.var + offsetof(zval, u2.fe_pos));

	if (MAY_BE_HASH(op1_info)) {
		ir_ref loop_ref, pos2_ref, p2_ref;

		if (MAY_BE_PACKED(op1_info)) {
			ref = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
				zend_jit_load_at_offset(jit, IR_U32,
					ht_ref, offsetof(zend_array, u.flags)),
				ir_const_u32(&jit->ctx, HASH_FLAG_PACKED));
			if_packed = zend_jit_if(jit, ref);
			zend_jit_if_false(jit, if_packed);
		}

		// JIT: p = fe_ht->arData + pos;
		if (sizeof(void*) == 8) {
			ref = ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_ADDR), hash_pos_ref);
		} else {
			ref = ir_fold1(&jit->ctx, IR_OPT(IR_BITCAST, IR_ADDR), hash_pos_ref);
		}
		hash_p_ref = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
			ir_fold2(&jit->ctx, IR_OPT(IR_MUL, IR_ADDR),
				ref,
				zend_jit_const_addr(jit, sizeof(Bucket))),
			zend_jit_load_at_offset(jit, IR_ADDR,
				ht_ref, offsetof(zend_array, arData)));

		loop_ref = zend_jit_loop_begin(jit, zend_jit_end(jit));
		hash_pos_ref = zend_jit_phi_2(jit, hash_pos_ref, IR_UNUSED);
		hash_p_ref = zend_jit_phi_2(jit, hash_p_ref, IR_UNUSED);

		// JIT: if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
		ref = ir_fold2(&jit->ctx, IR_OPT(IR_ULT, IR_BOOL),
			hash_pos_ref,
			zend_jit_load_at_offset(jit, IR_U32,
				ht_ref, offsetof(zend_array, nNumUsed)));

		// JIT: ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
		// JIT: ZEND_VM_CONTINUE();

		if (exit_addr) {
			if (exit_opcode == ZEND_JMP) {
				zend_jit_guard(jit, ref, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				ir_ref if_fit = zend_jit_if(jit, ref);
				zend_jit_if_false(jit, if_fit);
				zend_jit_array_add(exit_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_fit);
			}
		} else {
			ir_ref if_fit = zend_jit_if(jit, ref);
			zend_jit_if_false(jit, if_fit);
			zend_jit_array_add(exit_inputs, zend_jit_end(jit));
			zend_jit_if_true(jit, if_fit);
		}

		// JIT: pos++;
		pos2_ref = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
			hash_pos_ref,
			ir_const_u32(&jit->ctx, 1));

		// JIT: value_type = Z_TYPE_INFO_P(value);
		// JIT: if (EXPECTED(value_type != IS_UNDEF)) {
		if (!exit_addr || exit_opcode == ZEND_JMP) {
			if_undef_hash = zend_jit_if_zval_ref_type(jit, hash_p_ref, ir_const_u8(&jit->ctx, IS_UNDEF));
			zend_jit_if_true(jit, if_undef_hash);
		} else {
			zend_jit_guard_not(jit,
				zend_jit_zval_ref_type(jit, hash_p_ref),
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		}

		// JIT: p++;
		p2_ref = zend_jit_addr_offset(jit, hash_p_ref, sizeof(Bucket));

		ir_set_op(&jit->ctx, loop_ref, 2, zend_jit_loop_end(jit, loop_ref));
		ir_set_op(&jit->ctx, hash_pos_ref, 3, pos2_ref);
		ir_set_op(&jit->ctx, hash_p_ref, 3, p2_ref);

		if (MAY_BE_PACKED(op1_info)) {
			zend_jit_if_true(jit, if_packed);
		}
	}
	if (MAY_BE_PACKED(op1_info)) {
		ir_ref loop_ref, pos2_ref, p2_ref;

		// JIT: p = fe_ht->arPacked + pos;
		if (sizeof(void*) == 8) {
			ref = ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_ADDR), packed_pos_ref);
		} else {
			ref = ir_fold1(&jit->ctx, IR_OPT(IR_BITCAST, IR_ADDR), packed_pos_ref);
		}
		packed_p_ref = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR),
			ir_fold2(&jit->ctx, IR_OPT(IR_MUL, IR_ADDR),
				ref,
				zend_jit_const_addr(jit, sizeof(zval))),
			zend_jit_load_at_offset(jit, IR_ADDR,
				ht_ref, offsetof(zend_array, arPacked)));

		loop_ref = zend_jit_loop_begin(jit, zend_jit_end(jit));
		packed_pos_ref = zend_jit_phi_2(jit, packed_pos_ref, IR_UNUSED);
		packed_p_ref = zend_jit_phi_2(jit, packed_p_ref, IR_UNUSED);

		// JIT: if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
		ref = ir_fold2(&jit->ctx, IR_OPT(IR_ULT, IR_BOOL),
			packed_pos_ref,
			zend_jit_load_at_offset(jit, IR_U32,
				ht_ref, offsetof(zend_array, nNumUsed)));

		// JIT: ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
		// JIT: ZEND_VM_CONTINUE();
		if (exit_addr) {
			if (exit_opcode == ZEND_JMP) {
				zend_jit_guard(jit, ref, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				ir_ref if_fit = zend_jit_if(jit, ref);
				zend_jit_if_false(jit, if_fit);
				zend_jit_array_add(exit_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_fit);
			}
		} else {
			ir_ref if_fit = zend_jit_if(jit, ref);
			zend_jit_if_false(jit, if_fit);
			zend_jit_array_add(exit_inputs, zend_jit_end(jit));
			zend_jit_if_true(jit, if_fit);
		}

		// JIT: pos++;
		pos2_ref = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
			packed_pos_ref,
			ir_const_u32(&jit->ctx, 1));

		// JIT: value_type = Z_TYPE_INFO_P(value);
		// JIT: if (EXPECTED(value_type != IS_UNDEF)) {
		if (!exit_addr || exit_opcode == ZEND_JMP) {
			if_undef_packed = zend_jit_if_zval_ref_type(jit, packed_p_ref, ir_const_u8(&jit->ctx, IS_UNDEF));
			zend_jit_if_true(jit, if_undef_packed);
		} else {
			zend_jit_guard_not(jit,
				zend_jit_zval_ref_type(jit, packed_p_ref),
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		}

		// JIT: p++;
		p2_ref = zend_jit_addr_offset(jit, packed_p_ref, sizeof(zval));

		ir_set_op(&jit->ctx, loop_ref, 2, zend_jit_loop_end(jit, loop_ref));
		ir_set_op(&jit->ctx, packed_pos_ref, 3, pos2_ref);
		ir_set_op(&jit->ctx, packed_p_ref, 3, p2_ref);
	}

	if (!exit_addr || exit_opcode == ZEND_JMP) {
		zend_jit_addr val_addr;
		zend_jit_addr var_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->op2.var);
		uint32_t val_info;
		ir_ref p_ref = IR_UNUSED, hash_path = IR_UNUSED;

		if (RETURN_VALUE_USED(opline)) {
			zend_jit_addr res_addr = RES_ADDR();

			if (MAY_BE_HASH(op1_info)) {
				ir_ref key_ref = IR_UNUSED, if_key = IR_UNUSED, key_path = IR_UNUSED;

				ZEND_ASSERT(if_undef_hash);
				zend_jit_if_false(jit, if_undef_hash);

				// JIT: Z_FE_POS_P(array) = pos + 1;
				zend_jit_store_at_offset(jit,
					zend_jit_fp(jit), opline->op1.var + offsetof(zval, u2.fe_pos),
					ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
						hash_pos_ref,
						ir_const_u32(&jit->ctx, 1)));

				if (op1_info & MAY_BE_ARRAY_KEY_STRING) {
					key_ref = zend_jit_load_at_offset(jit, IR_ADDR,
						hash_p_ref, offsetof(Bucket, key));
				}
				if ((op1_info & MAY_BE_ARRAY_KEY_LONG)
				 && (op1_info & MAY_BE_ARRAY_KEY_STRING)) {
					// JIT: if (!p->key) {
					if_key = zend_jit_if(jit, key_ref);
					zend_jit_if_true(jit, if_key);
				}
				if (op1_info & MAY_BE_ARRAY_KEY_STRING) {
					ir_ref if_interned, interned_path;

					// JIT: ZVAL_STR_COPY(EX_VAR(opline->result.var), p->key);
					zend_jit_zval_set_ptr(jit, res_addr, key_ref);
					ref = ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
						zend_jit_load_at_offset(jit, IR_U32,
							key_ref, offsetof(zend_refcounted, gc.u.type_info)),
						ir_const_u32(&jit->ctx, IS_STR_INTERNED));
					if_interned = zend_jit_if(jit, ref);
					zend_jit_if_true(jit, if_interned);

					zend_jit_zval_set_type_info(jit, res_addr, IS_STRING);

					interned_path = zend_jit_end(jit);
					zend_jit_if_false(jit, if_interned);

					zend_jit_gc_addref(jit, key_ref);
					zend_jit_zval_set_type_info(jit, res_addr, IS_STRING_EX);

					zend_jit_merge_2(jit, interned_path, zend_jit_end(jit));

					if (op1_info & MAY_BE_ARRAY_KEY_LONG) {
						key_path = zend_jit_end(jit);
					}
				}
				if (op1_info & MAY_BE_ARRAY_KEY_LONG) {
					if (op1_info & MAY_BE_ARRAY_KEY_STRING) {
						zend_jit_if_false(jit, if_key);
					}
					// JIT: ZVAL_LONG(EX_VAR(opline->result.var), p->h);
					ref = zend_jit_load_at_offset(jit, IR_PHP_LONG,
						hash_p_ref, offsetof(Bucket, h));
					zend_jit_zval_set_lval(jit, res_addr, ref);
					zend_jit_zval_set_type_info(jit, res_addr, IS_LONG);

					if (op1_info & MAY_BE_ARRAY_KEY_STRING) {
						zend_jit_merge_2(jit, key_path, zend_jit_end(jit));
					}
				}
				if (MAY_BE_PACKED(op1_info)) {
					hash_path = zend_jit_end(jit);
				} else {
					p_ref = hash_p_ref;
				}
			}
			if (MAY_BE_PACKED(op1_info)) {
				ZEND_ASSERT(if_undef_packed);
				zend_jit_if_false(jit, if_undef_packed);

				// JIT: Z_FE_POS_P(array) = pos + 1;
				zend_jit_store_at_offset(jit,
					zend_jit_fp(jit), opline->op1.var + offsetof(zval, u2.fe_pos),
					ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
						packed_pos_ref,
						ir_const_u32(&jit->ctx, 1)));

				// JIT: ZVAL_LONG(EX_VAR(opline->result.var), pos);
				if (sizeof(zend_long) == 8) {
					packed_pos_ref = ir_fold1(&jit->ctx, IR_OPT(IR_ZEXT, IR_PHP_LONG), packed_pos_ref);
				} else {
					packed_pos_ref = ir_fold1(&jit->ctx, IR_OPT(IR_BITCAST, IR_PHP_LONG), packed_pos_ref);
				}
				zend_jit_zval_set_lval(jit, res_addr, packed_pos_ref);
				zend_jit_zval_set_type_info(jit, res_addr, IS_LONG);

				if (MAY_BE_HASH(op1_info)) {
					zend_jit_merge_2(jit, hash_path, zend_jit_end(jit));
					p_ref = zend_jit_phi_2(jit, hash_p_ref, packed_p_ref);
				} else {
					p_ref = packed_p_ref;
				}
			}
		} else {
			ir_ref pos_ref = IR_UNUSED;

			if (if_undef_hash && if_undef_packed) {
				ir_ref hash_path;

				zend_jit_if_false(jit, if_undef_hash);
				hash_path = zend_jit_end(jit);
				zend_jit_if_false(jit, if_undef_packed);
				zend_jit_merge_2(jit, hash_path, zend_jit_end(jit));
				pos_ref = zend_jit_phi_2(jit, hash_pos_ref, packed_pos_ref);
				p_ref = zend_jit_phi_2(jit, hash_p_ref, packed_p_ref);
			} else if (if_undef_hash) {
				zend_jit_if_false(jit, if_undef_hash);
				pos_ref = hash_pos_ref;
				p_ref = hash_p_ref;
			} else if (if_undef_packed) {
				zend_jit_if_false(jit, if_undef_packed);
				pos_ref = packed_pos_ref;
				p_ref = packed_p_ref;
			} else {
				ZEND_ASSERT(0);
			}

			// JIT: Z_FE_POS_P(array) = pos + 1;
			zend_jit_store_at_offset(jit,
				zend_jit_fp(jit), opline->op1.var + offsetof(zval, u2.fe_pos),
				ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_U32),
					pos_ref,
					ir_const_u32(&jit->ctx, 1)));
		}

		val_info = ((op1_info & MAY_BE_ARRAY_OF_ANY) >> MAY_BE_ARRAY_SHIFT);
		if (val_info & MAY_BE_ARRAY) {
			val_info |= MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF;
		}
		if (op1_info & MAY_BE_ARRAY_OF_REF) {
			val_info |= MAY_BE_REF | MAY_BE_RC1 | MAY_BE_RCN | MAY_BE_ANY |
				MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF;
		} else if (val_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
			val_info |= MAY_BE_RC1 | MAY_BE_RCN;
		}

		val_addr = ZEND_ADDR_REF_ZVAL(p_ref);
		if (opline->op2_type == IS_CV) {
			// JIT: zend_assign_to_variable(variable_ptr, value, IS_CV, EX_USES_STRICT_TYPES());
			if (!zend_jit_assign_to_variable(jit, opline, var_addr, var_addr, op2_info, -1, IS_CV, val_addr, val_info, 0, 0, 1)) {
				return 0;
			}
		} else {
			// JIT: ZVAL_COPY(res, value);
			zend_jit_copy_zval(jit, var_addr, -1, val_addr, val_info, 1);
		}

		if (!exit_addr) {
			zend_basic_block *bb;

			ZEND_ASSERT(jit->b >= 0);
			bb = &jit->ssa->cfg.blocks[jit->b];
			_zend_jit_add_predecessor_ref(jit, bb->successors[1], jit->b, zend_jit_end(jit));
			ZEND_ASSERT(exit_inputs_count > 0);
			if (exit_inputs_count == 1) {
				ref = exit_inputs[0];
			} else {
				zend_jit_merge_N(jit, exit_inputs_count, exit_inputs);
				ref = zend_jit_end(jit);
			}
			_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, ref);
			jit->control = IR_UNUSED;
			jit->b = -1;
		}
	} else {
		ZEND_ASSERT(exit_inputs_count);
		zend_jit_merge_N(jit, exit_inputs_count, exit_inputs);
	}

	return 1;
}

static int zend_jit_load_this(zend_jit_ctx *jit, uint32_t var)
{
	zend_jit_addr this_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, offsetof(zend_execute_data, This));
	zend_jit_addr var_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, var);
	ir_ref ref = zend_jit_zval_ptr(jit, this_addr);

	zend_jit_zval_set_ptr(jit, var_addr, ref);
	zend_jit_zval_set_type_info(jit, var_addr, IS_OBJECT_EX);
	zend_jit_gc_addref(jit, ref);

	return 1;
}

static int zend_jit_fetch_this(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, bool check_only)
{
	if (!op_array->scope || (op_array->fn_flags & ZEND_ACC_STATIC)) {
		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
			if (!JIT_G(current_frame) ||
			    !TRACE_FRAME_IS_THIS_CHECKED(JIT_G(current_frame))) {

				zend_jit_addr this_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, offsetof(zend_execute_data, This));
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}

				zend_jit_guard_zval_type(jit, this_addr, IS_OBJECT, exit_addr);

				if (JIT_G(current_frame)) {
					TRACE_FRAME_SET_THIS_CHECKED(JIT_G(current_frame));
				}
			}
		} else {
			zend_jit_addr this_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, offsetof(zend_execute_data, This));
			ir_ref if_object = zend_jit_if_zval_type(jit, this_addr, IS_OBJECT);

			zend_jit_if_false_cold(jit, if_object);
			zend_jit_set_ex_opline(jit, opline);
			zend_jit_ijmp(jit, zend_jit_stub_addr(jit, jit_stub_invalid_this));

			zend_jit_if_true(jit, if_object);
		}
	}

	if (!check_only) {
		if (!zend_jit_load_this(jit, opline->result.var)) {
			return 0;
		}
	}

	return 1;
}

static int zend_jit_class_guard(zend_jit_ctx *jit, const zend_op *opline, ir_ref obj_ref, zend_class_entry *ce)
{
	int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);
	const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

	if (!exit_addr) {
		return 0;
	}

	zend_jit_guard(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
			zend_jit_load_at_offset(jit, IR_ADDR, obj_ref, offsetof(zend_object, ce)),
			zend_jit_const_addr(jit, (uintptr_t)ce)),
		zend_jit_const_addr(jit, (uintptr_t)exit_addr));

	return 1;
}

static int zend_jit_fetch_obj(zend_jit_ctx         *jit,
                              const zend_op        *opline,
                              const zend_op_array  *op_array,
                              zend_ssa             *ssa,
                              const zend_ssa_op    *ssa_op,
                              uint32_t              op1_info,
                              zend_jit_addr         op1_addr,
                              bool                  op1_indirect,
                              zend_class_entry     *ce,
                              bool                  ce_is_instanceof,
                              bool                  on_this,
                              bool                  delayed_fetch_this,
                              bool                  op1_avoid_refcounting,
                              zend_class_entry     *trace_ce,
                              uint8_t               prop_type,
                              int                   may_throw)
{
	zval *member;
	zend_property_info *prop_info;
	bool may_be_dynamic = 1;
	zend_jit_addr res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var);
	zend_jit_addr prop_addr;
	uint32_t res_info = RES_INFO();
	ir_ref prop_type_ref = IR_UNUSED;
	ir_ref obj_ref = IR_UNUSED;
	ir_ref prop_ref = IR_UNUSED;
	ir_ref end_inputs[8];
	uint32_t end_inputs_count = 0;
	ir_ref slow_inputs[8];
	uint32_t slow_inputs_count = 0;

	ZEND_ASSERT(opline->op2_type == IS_CONST);
	ZEND_ASSERT(op1_info & MAY_BE_OBJECT);

	member = RT_CONSTANT(opline, opline->op2);
	ZEND_ASSERT(Z_TYPE_P(member) == IS_STRING && Z_STRVAL_P(member)[0] != '\0');
	prop_info = zend_get_known_property_info(op_array, ce, Z_STR_P(member), on_this, op_array->filename);

	if (on_this) {
		zend_jit_addr this_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, offsetof(zend_execute_data, This));
		obj_ref = zend_jit_zval_ptr(jit, this_addr);
	} else {
		if (opline->op1_type == IS_VAR
		 && opline->opcode == ZEND_FETCH_OBJ_W
		 && (op1_info & MAY_BE_INDIRECT)
		 && Z_REG(op1_addr) == ZREG_FP) {
			op1_addr = zend_jit_zval_indirect_deref(jit, op1_addr);
		}
		if (op1_info & MAY_BE_REF) {
			op1_addr = zend_jit_zval_deref(jit, op1_addr);
		}
		if (op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY)- MAY_BE_OBJECT)) {
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				zend_jit_guard_zval_type(jit, op1_addr, IS_OBJECT, exit_addr);
			} else {
				ir_ref if_obj = zend_jit_if_zval_type(jit, op1_addr, IS_OBJECT);

				zend_jit_if_false_cold(jit, if_obj);
				if (opline->opcode != ZEND_FETCH_OBJ_IS) {
					ir_ref op1_ref = IR_UNUSED;

					zend_jit_set_ex_opline(jit, opline);
					if (opline->opcode != ZEND_FETCH_OBJ_W && (op1_info & MAY_BE_UNDEF)) {
						zend_jit_addr orig_op1_addr = OP1_ADDR();
						ir_ref fast_path = IR_UNUSED;

						if (op1_info & MAY_BE_ANY) {
							ir_ref if_def = zend_jit_if(jit, zend_jit_zval_type(jit, op1_addr));
							zend_jit_if_true(jit, if_def);
							fast_path = zend_jit_end(jit);
							zend_jit_if_false_cold(jit, if_def);
						}
						zend_jit_call_1(jit, IR_VOID,
							zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_undefined_op_helper, IR_CONST_FASTCALL_FUNC),
							ir_const_u32(&jit->ctx, opline->op1.var));
						if (fast_path) {
							zend_jit_merge_2(jit, fast_path, zend_jit_end(jit));
						}
						op1_ref = zend_jit_zval_addr(jit, orig_op1_addr);
					} else {
						op1_ref = zend_jit_zval_addr(jit, op1_addr);
					}
					if (opline->opcode == ZEND_FETCH_OBJ_W) {
						zend_jit_call_2(jit, IR_VOID,
							zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_invalid_property_write, IR_CONST_FASTCALL_FUNC),
							op1_ref,
							zend_jit_const_addr(jit, (uintptr_t)Z_STRVAL_P(member)));
						zend_jit_zval_set_type_info(jit, res_addr, _IS_ERROR);
					} else {
						zend_jit_call_2(jit, IR_VOID,
							zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_invalid_property_read, IR_CONST_FASTCALL_FUNC),
							op1_ref,
							zend_jit_const_addr(jit, (uintptr_t)Z_STRVAL_P(member)));
						zend_jit_zval_set_type_info(jit, res_addr, IS_NULL);
					}
				} else {
					zend_jit_zval_set_type_info(jit, res_addr, IS_NULL);
				}
				zend_jit_array_add(end_inputs, zend_jit_end(jit));

				zend_jit_if_true(jit, if_obj);
			}
		}
		obj_ref = zend_jit_zval_ptr(jit, op1_addr);
	}

	ZEND_ASSERT(obj_ref);
	if (!prop_info && trace_ce && (trace_ce->ce_flags & ZEND_ACC_IMMUTABLE)) {
		prop_info = zend_get_known_property_info(op_array, trace_ce, Z_STR_P(member), on_this, op_array->filename);
		if (prop_info) {
			ce = trace_ce;
			ce_is_instanceof = 0;
			if (!(op1_info & MAY_BE_CLASS_GUARD)) {
				if (on_this && JIT_G(current_frame)
				 && TRACE_FRAME_IS_THIS_CLASS_CHECKED(JIT_G(current_frame))) {
					ZEND_ASSERT(JIT_G(current_frame)->ce == ce);
				} else if (zend_jit_class_guard(jit, opline, obj_ref, ce)) {
					if (on_this && JIT_G(current_frame)) {
						JIT_G(current_frame)->ce = ce;
						TRACE_FRAME_SET_THIS_CLASS_CHECKED(JIT_G(current_frame));
					}
				} else {
					return 0;
				}
				if (ssa->var_info && ssa_op->op1_use >= 0) {
					ssa->var_info[ssa_op->op1_use].type |= MAY_BE_CLASS_GUARD;
					ssa->var_info[ssa_op->op1_use].ce = ce;
					ssa->var_info[ssa_op->op1_use].is_instanceof = ce_is_instanceof;
				}
			}
		}
	}

	if (!prop_info) {
		ir_ref run_time_cache = zend_jit_load_at_offset(jit, IR_ADDR,
			zend_jit_fp(jit),
			offsetof(zend_execute_data, run_time_cache));
		ir_ref ref = zend_jit_load_at_offset(jit, IR_ADDR,
			run_time_cache,
			opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS);
		ir_ref if_same = zend_jit_if(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_ADDR),
				ref,
				zend_jit_load_at_offset(jit, IR_ADDR,
					obj_ref,
					offsetof(zend_object, ce))));

		zend_jit_if_false_cold(jit, if_same);
		zend_jit_array_add(slow_inputs, zend_jit_end(jit));

		zend_jit_if_true(jit, if_same);
		ir_ref offset_ref = zend_jit_load_at_offset(jit, IR_ADDR,
			run_time_cache,
			(opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*));

		may_be_dynamic = zend_may_be_dynamic_property(ce, Z_STR_P(member), opline->op1_type == IS_UNUSED, op_array->filename);
		if (may_be_dynamic) {
			ir_ref if_dynamic = zend_jit_if(jit,
				ir_fold2(&jit->ctx, IR_OPT(IR_LT, IR_BOOL),
					offset_ref,
					IR_NULL));
			if (opline->opcode == ZEND_FETCH_OBJ_W) {
				zend_jit_if_true_cold(jit, if_dynamic);
				zend_jit_array_add(slow_inputs, zend_jit_end(jit));
			} else {
				zend_jit_if_true_cold(jit, if_dynamic);
				zend_jit_set_ex_opline(jit, opline);

				if (opline->opcode != ZEND_FETCH_OBJ_IS) {
					zend_jit_call_2(jit, IR_VOID,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_obj_r_dynamic, IR_CONST_FASTCALL_FUNC),
						obj_ref,
						offset_ref);
				} else {
					zend_jit_call_2(jit, IR_VOID,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_obj_is_dynamic, IR_CONST_FASTCALL_FUNC),
						obj_ref,
						offset_ref);
				}
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}
			zend_jit_if_false(jit, if_dynamic);
		}
		prop_ref = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR), obj_ref, offset_ref);
		prop_type_ref = zend_jit_zval_ref_type(jit, prop_ref);
		ir_ref if_def = zend_jit_if(jit, prop_type_ref);
		zend_jit_if_false_cold(jit, if_def);
		zend_jit_array_add(slow_inputs, zend_jit_end(jit));
		zend_jit_if_true(jit, if_def);
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);
		if (opline->opcode == ZEND_FETCH_OBJ_W
		 && (!ce ||	ce_is_instanceof || (ce->ce_flags & ZEND_ACC_HAS_TYPE_HINTS))) {
			uint32_t flags = opline->extended_value & ZEND_FETCH_OBJ_FLAGS;

			ir_ref prop_info_ref = zend_jit_load_at_offset(jit, IR_ADDR,
				run_time_cache,
				(opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*) * 2);
			ir_ref if_has_prop_info = zend_jit_if(jit, prop_info_ref);

			zend_jit_if_true_cold(jit, if_has_prop_info);

			ir_ref if_readonly = zend_jit_if(jit,
				ir_fold2(&jit->ctx, IR_OPT(IR_AND, IR_U32),
					zend_jit_load_at_offset(jit, IR_U32, prop_info_ref, offsetof(zend_property_info, flags)),
					ir_const_u32(&jit->ctx, ZEND_ACC_READONLY)));
			zend_jit_if_true(jit, if_readonly);

			ir_ref if_prop_obj = zend_jit_if_zval_type(jit, prop_addr, IS_OBJECT);
			zend_jit_if_true(jit, if_prop_obj);
			ref = zend_jit_zval_ptr(jit, prop_addr);
			zend_jit_gc_addref(jit, ref);
			zend_jit_zval_set_ptr(jit, res_addr, ref);
			zend_jit_zval_set_type_info(jit, res_addr, IS_OBJECT_EX);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));

			zend_jit_if_false_cold(jit, if_prop_obj);
			zend_jit_set_ex_opline(jit, opline);
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_readonly_property_modification_error, IR_CONST_FASTCALL_FUNC),
				prop_info_ref);
			zend_jit_zval_set_type_info(jit, res_addr, _IS_ERROR);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));

			if (flags == ZEND_FETCH_DIM_WRITE) {
				zend_jit_if_false_cold(jit, if_readonly);
				zend_jit_set_ex_opline(jit, opline);
				zend_jit_call_2(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_check_array_promotion, IR_CONST_FASTCALL_FUNC),
					prop_ref,
					prop_info_ref);
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
				zend_jit_if_false(jit, if_has_prop_info);
			} else if (flags == ZEND_FETCH_REF) {
				zend_jit_if_false_cold(jit, if_readonly);
				zend_jit_call_3(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_create_typed_ref, IR_CONST_FASTCALL_FUNC),
					prop_ref,
					prop_info_ref,
					zend_jit_zval_addr(jit, res_addr));
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
				zend_jit_if_false(jit, if_has_prop_info);
			} else {
				ZEND_ASSERT(flags == 0);
				zend_jit_if_false(jit, if_has_prop_info);
				ir_ref path1 = zend_jit_end(jit);
				zend_jit_if_false(jit, if_readonly);
				zend_jit_merge_2(jit, path1, zend_jit_end(jit));
			}
		}
	} else {
		prop_ref = zend_jit_addr_offset(jit, obj_ref, prop_info->offset);
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);
		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
			if (opline->opcode == ZEND_FETCH_OBJ_W || !(res_info & MAY_BE_GUARD) || !JIT_G(current_frame)) {
				/* perform IS_UNDEF check only after result type guard (during deoptimization) */
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				prop_type_ref = zend_jit_zval_type_info(jit, prop_addr);
				zend_jit_guard(jit, prop_type_ref, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			}
		} else {
			prop_type_ref = zend_jit_zval_type_info(jit, prop_addr);
			ir_ref if_def = zend_jit_if(jit, prop_type_ref);
			zend_jit_if_false_cold(jit, if_def);
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));
			zend_jit_if_true(jit, if_def);
		}
		if (opline->opcode == ZEND_FETCH_OBJ_W && (prop_info->flags & ZEND_ACC_READONLY)) {
			if (!prop_type_ref) {
				prop_type_ref = zend_jit_zval_type_info(jit, prop_addr);
			}
			ir_ref if_prop_obj = zend_jit_if_zval_type(jit, prop_addr, IS_OBJECT);
			zend_jit_if_true(jit, if_prop_obj);
			ir_ref ref = zend_jit_zval_ptr(jit, prop_addr);
			zend_jit_gc_addref(jit, ref);
			zend_jit_zval_set_ptr(jit, res_addr, ref);
			zend_jit_zval_set_type_info(jit, res_addr, IS_OBJECT_EX);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));

			zend_jit_if_false_cold(jit, if_prop_obj);
			zend_jit_set_ex_opline(jit, opline);
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_readonly_property_modification_error, IR_CONST_FASTCALL_FUNC),
				zend_jit_const_addr(jit, (uintptr_t)prop_info));
			zend_jit_zval_set_type_info(jit, res_addr, _IS_ERROR);
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		} else if (opline->opcode == ZEND_FETCH_OBJ_W
		 && (opline->extended_value & ZEND_FETCH_OBJ_FLAGS)
		 && ZEND_TYPE_IS_SET(prop_info->type)) {
			uint32_t flags = opline->extended_value & ZEND_FETCH_OBJ_FLAGS;

			if (flags == ZEND_FETCH_DIM_WRITE) {
				if ((ZEND_TYPE_FULL_MASK(prop_info->type) & MAY_BE_ARRAY) == 0) {
					if (!prop_type_ref) {
						prop_type_ref = zend_jit_zval_type_info(jit, prop_addr);
					}
					ir_ref if_null_or_flase = zend_jit_if(jit,
						ir_fold2(&jit->ctx, IR_OPT(IR_LE, IR_BOOL),
							prop_type_ref,
							ir_const_u32(&jit->ctx, IR_FALSE)));
					zend_jit_if_true_cold(jit, if_null_or_flase);
					zend_jit_set_ex_opline(jit, opline);
					zend_jit_call_2(jit, IR_VOID,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_check_array_promotion, IR_CONST_FASTCALL_FUNC),
						prop_ref,
						zend_jit_const_addr(jit, (uintptr_t)prop_info));
					zend_jit_array_add(end_inputs, zend_jit_end(jit));
					zend_jit_if_false(jit, if_null_or_flase);
				}
			} else if (flags == ZEND_FETCH_REF) {
				ir_ref ref;

				if (!prop_type_ref) {
					prop_type_ref = zend_jit_zval_type_info(jit, prop_addr);
				}

				ir_ref if_reference = zend_jit_if(jit,
					ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL),
						prop_type_ref,
						ir_const_u32(&jit->ctx, IS_REFERENCE_EX)));
				zend_jit_if_false(jit, if_reference);
				if (ce && ce->ce_flags & ZEND_ACC_IMMUTABLE) {
					ref = zend_jit_const_addr(jit, (uintptr_t)prop_info);
				} else {
					int prop_info_offset =
						(((prop_info->offset - (sizeof(zend_object) - sizeof(zval))) / sizeof(zval)) * sizeof(void*));

					ref = zend_jit_load_at_offset(jit, IR_ADDR, obj_ref, offsetof(zend_object, ce));
					ref = zend_jit_load_at_offset(jit, IR_ADDR, ref, offsetof(zend_class_entry, properties_info_table));
					ref = zend_jit_load_at_offset(jit, IR_ADDR, ref, prop_info_offset);
				}
				zend_jit_call_3(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_create_typed_ref, IR_CONST_FASTCALL_FUNC),
					prop_ref,
					ref,
					zend_jit_zval_addr(jit, res_addr));
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_reference);
			} else {
				ZEND_UNREACHABLE();
			}
		}
	}

	if (opline->opcode == ZEND_FETCH_OBJ_W) {
		if (!prop_info || !(prop_info->flags & ZEND_ACC_READONLY)) {
			ZEND_ASSERT(prop_ref);
			zend_jit_zval_set_ptr(jit, res_addr, prop_ref);
			zend_jit_zval_set_type_info(jit, res_addr, IS_INDIRECT);
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && prop_info) {
				ssa->var_info[ssa_op->result_def].indirect_reference = 1;
			}
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	} else {
		bool result_avoid_refcounting = 0;

		if ((res_info & MAY_BE_GUARD) && JIT_G(current_frame) && prop_info) {
			zend_uchar type = concrete_type(res_info);
			uint32_t flags = 0;
			zend_jit_addr val_addr = prop_addr;

			if ((opline->op1_type & (IS_VAR|IS_TMP_VAR))
			 && !delayed_fetch_this
			 && !op1_avoid_refcounting) {
				flags = ZEND_JIT_EXIT_FREE_OP1;
			}

			if ((opline->result_type & (IS_VAR|IS_TMP_VAR))
			 && !(flags & ZEND_JIT_EXIT_FREE_OP1)
			 && (res_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))
			 && (ssa_op+1)->op1_use == ssa_op->result_def
			 && zend_jit_may_avoid_refcounting(opline+1, res_info)) {
				result_avoid_refcounting = 1;
				ssa->var_info[ssa_op->result_def].avoid_refcounting = 1;
			}

			val_addr = zend_jit_guard_fetch_result_type(jit, opline, val_addr, type,
				1, flags, op1_avoid_refcounting);
			if (!val_addr) {
				return 0;
			}

			res_info &= ~MAY_BE_GUARD;
			ssa->var_info[ssa_op->result_def].type &= ~MAY_BE_GUARD;

			// ZVAL_COPY
			zend_jit_copy_zval(jit, res_addr, -1, val_addr, res_info, !result_avoid_refcounting);
		} else {
			prop_type_ref = zend_jit_zval_type_info(jit, prop_addr);

			if (!zend_jit_zval_copy_deref(jit, res_addr, prop_addr, prop_type_ref)) {
				return 0;
			}
		}
		zend_jit_array_add(end_inputs, zend_jit_end(jit));
	}

	if (JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE || !prop_info) {
		zend_jit_merge_N(jit, slow_inputs_count, slow_inputs);
		zend_jit_set_ex_opline(jit, opline);

		if (opline->opcode == ZEND_FETCH_OBJ_W) {
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_obj_w_slow, IR_CONST_FASTCALL_FUNC),
				obj_ref);
		} else if (opline->opcode != ZEND_FETCH_OBJ_IS) {
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_obj_r_slow, IR_CONST_FASTCALL_FUNC),
				obj_ref);
		} else {
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_fetch_obj_is_slow, IR_CONST_FASTCALL_FUNC),
				obj_ref);
		}
		zend_jit_array_add(end_inputs, zend_jit_end(jit));
	}

	zend_jit_merge_N(jit, end_inputs_count, end_inputs);

	if (opline->op1_type != IS_UNUSED && !delayed_fetch_this && !op1_indirect) {
		if (opline->op1_type == IS_VAR
		 && opline->opcode == ZEND_FETCH_OBJ_W
		 && (op1_info & MAY_BE_RC1)) {
			zend_jit_addr orig_op1_addr = OP1_ADDR();
		    ir_ref if_refcounted, ptr, refcount, if_non_zero, path1, path2;

			if_refcounted = zend_jit_if_zval_refcounted(jit, orig_op1_addr);
			zend_jit_if_false(jit,  if_refcounted);
			path1 = zend_jit_end(jit);
			zend_jit_if_true(jit,  if_refcounted);
			ptr = zend_jit_zval_ptr(jit, orig_op1_addr);
			refcount = zend_jit_gc_delref(jit, ptr);
			if_non_zero = zend_jit_if(jit, refcount);
			zend_jit_if_true(jit,  if_non_zero);
			path2 = zend_jit_end(jit);
			zend_jit_if_false(jit,  if_non_zero);
			zend_jit_set_ex_opline(jit, opline);
			zend_jit_call_1(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_extract_helper, IR_CONST_FASTCALL_FUNC),
				ptr);
			zend_jit_merge_3(jit, zend_jit_end(jit), path1, path2);
		} else if (!op1_avoid_refcounting) {
			if (on_this) {
				op1_info &= ~MAY_BE_RC1;
			}
			zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);
		}
	}

	if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE
	 && prop_info
	 && (opline->opcode != ZEND_FETCH_OBJ_W ||
	     !(opline->extended_value & ZEND_FETCH_OBJ_FLAGS) ||
	     !ZEND_TYPE_IS_SET(prop_info->type))
	 && (!(opline->op1_type & (IS_VAR|IS_TMP_VAR)) || on_this || op1_indirect)) {
		may_throw = 0;
	}

	if (may_throw) {
		zend_jit_check_exception(jit);
	}

	return 1;
}

static int zend_jit_assign_obj(zend_jit_ctx         *jit,
                               const zend_op        *opline,
                               const zend_op_array  *op_array,
                               zend_ssa             *ssa,
                               const zend_ssa_op    *ssa_op,
                               uint32_t              op1_info,
                               zend_jit_addr         op1_addr,
                               uint32_t              val_info,
                               bool                  op1_indirect,
                               zend_class_entry     *ce,
                               bool                  ce_is_instanceof,
                               bool                  on_this,
                               bool                  delayed_fetch_this,
                               zend_class_entry     *trace_ce,
                               uint8_t               prop_type,
                               int                   may_throw)
{
	zval *member;
	zend_string *name;
	zend_property_info *prop_info;
	zend_jit_addr val_addr = OP1_DATA_ADDR();
	zend_jit_addr res_addr = 0;
	zend_jit_addr prop_addr;
	ir_ref obj_ref = IR_UNUSED;
	ir_ref prop_ref = IR_UNUSED;
	ir_ref delayed_end_input = IR_UNUSED;
	ir_ref end_inputs[8];
	uint32_t end_inputs_count = 0;
	ir_ref slow_inputs[8];
	uint32_t slow_inputs_count = 0;

	if (RETURN_VALUE_USED(opline)) {
		res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var);
	}

	ZEND_ASSERT(opline->op2_type == IS_CONST);
	ZEND_ASSERT(op1_info & MAY_BE_OBJECT);

	member = RT_CONSTANT(opline, opline->op2);
	ZEND_ASSERT(Z_TYPE_P(member) == IS_STRING && Z_STRVAL_P(member)[0] != '\0');
	name = Z_STR_P(member);
	prop_info = zend_get_known_property_info(op_array, ce, name, on_this, op_array->filename);

	if (on_this) {
		zend_jit_addr this_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, offsetof(zend_execute_data, This));
		obj_ref = zend_jit_zval_ptr(jit, this_addr);
	} else {
		if (opline->op1_type == IS_VAR
		 && (op1_info & MAY_BE_INDIRECT)
		 && Z_REG(op1_addr) == ZREG_FP) {
			op1_addr = zend_jit_zval_indirect_deref(jit, op1_addr);
		}
		if (op1_info & MAY_BE_REF) {
			op1_addr = zend_jit_zval_deref(jit, op1_addr);
		}
		if (op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY)- MAY_BE_OBJECT)) {
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				zend_jit_guard_zval_type(jit, op1_addr, IS_OBJECT, exit_addr);
			} else {
				ir_ref if_obj = zend_jit_if_zval_type(jit, op1_addr, IS_OBJECT);
				zend_jit_if_false_cold(jit, if_obj);

				zend_jit_set_ex_opline(jit, opline);
				zend_jit_call_2(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_invalid_property_assign, IR_CONST_FASTCALL_FUNC),
					zend_jit_zval_addr(jit, op1_addr),
					zend_jit_const_addr(jit, (uintptr_t)ZSTR_VAL(name)));

				if (RETURN_VALUE_USED(opline)) {
					zend_jit_zval_set_type_info(jit, res_addr, IS_NULL);
				}

				zend_jit_array_add(end_inputs, zend_jit_end(jit));

				zend_jit_if_true(jit, if_obj);
			}
		}
		obj_ref = zend_jit_zval_ptr(jit, op1_addr);
	}

	ZEND_ASSERT(obj_ref);
	if (!prop_info && trace_ce && (trace_ce->ce_flags & ZEND_ACC_IMMUTABLE)) {
		prop_info = zend_get_known_property_info(op_array, trace_ce, name, on_this, op_array->filename);
		if (prop_info) {
			ce = trace_ce;
			ce_is_instanceof = 0;
			if (!(op1_info & MAY_BE_CLASS_GUARD)) {
				if (on_this && JIT_G(current_frame)
				 && TRACE_FRAME_IS_THIS_CLASS_CHECKED(JIT_G(current_frame))) {
					ZEND_ASSERT(JIT_G(current_frame)->ce == ce);
				} else if (zend_jit_class_guard(jit, opline, obj_ref, ce)) {
					if (on_this && JIT_G(current_frame)) {
						JIT_G(current_frame)->ce = ce;
						TRACE_FRAME_SET_THIS_CLASS_CHECKED(JIT_G(current_frame));
					}
				} else {
					return 0;
				}
				if (ssa->var_info && ssa_op->op1_use >= 0) {
					ssa->var_info[ssa_op->op1_use].type |= MAY_BE_CLASS_GUARD;
					ssa->var_info[ssa_op->op1_use].ce = ce;
					ssa->var_info[ssa_op->op1_use].is_instanceof = ce_is_instanceof;
				}
				if (ssa->var_info && ssa_op->op1_def >= 0) {
					ssa->var_info[ssa_op->op1_def].type |= MAY_BE_CLASS_GUARD;
					ssa->var_info[ssa_op->op1_def].ce = ce;
					ssa->var_info[ssa_op->op1_def].is_instanceof = ce_is_instanceof;
				}
			}
		}
	}

	if (!prop_info) {
		ir_ref run_time_cache = zend_jit_load_at_offset(jit, IR_ADDR,
			zend_jit_fp(jit),
			offsetof(zend_execute_data, run_time_cache));
		ir_ref ref = zend_jit_load_at_offset(jit, IR_ADDR,
			run_time_cache,
			opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS);
		ir_ref if_same = zend_jit_if(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_ADDR),
				ref,
				zend_jit_load_at_offset(jit, IR_ADDR,
					obj_ref,
					offsetof(zend_object, ce))));

		zend_jit_if_false_cold(jit, if_same);
		zend_jit_array_add(slow_inputs, zend_jit_end(jit));

		zend_jit_if_true(jit, if_same);
		ir_ref offset_ref = zend_jit_load_at_offset(jit, IR_ADDR,
			run_time_cache,
			(opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*));

		ir_ref if_dynamic = zend_jit_if(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_LT, IR_BOOL),
				offset_ref,
				IR_NULL));
		zend_jit_if_true_cold(jit, if_dynamic);
		zend_jit_array_add(slow_inputs, zend_jit_end(jit));

		zend_jit_if_false(jit, if_dynamic);
		prop_ref = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR), obj_ref, offset_ref);
		ir_ref if_def = zend_jit_if(jit, zend_jit_zval_ref_type(jit, prop_ref));
		zend_jit_if_false_cold(jit, if_def);
		zend_jit_array_add(slow_inputs, zend_jit_end(jit));

		zend_jit_if_true(jit, if_def);
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);

		if (!ce || ce_is_instanceof || (ce->ce_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
			ir_ref prop_info_ref = zend_jit_load_at_offset(jit, IR_ADDR,
				run_time_cache,
				(opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*) * 2);
			ir_ref if_has_prop_info = zend_jit_if(jit, prop_info_ref);
			zend_jit_if_true_cold(jit, if_has_prop_info);

			// JIT: value = zend_assign_to_typed_prop(prop_info, property_val, value EXECUTE_DATA_CC);
			zend_jit_set_ex_opline(jit, opline);
			zend_jit_call_4(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_assign_to_typed_prop, IR_CONST_FASTCALL_FUNC),
				prop_ref,
				prop_info_ref,
				zend_jit_zval_addr(jit, val_addr),
				RETURN_VALUE_USED(opline) ? zend_jit_zval_addr(jit, res_addr) : IR_NULL);

			if ((opline+1)->op1_type == IS_CONST) {
				// TODO: ???
				// if (Z_TYPE_P(value) == orig_type) {
				// CACHE_PTR_EX(cache_slot + 2, NULL);
			}

			zend_jit_array_add(end_inputs, zend_jit_end(jit));
			zend_jit_if_false(jit, if_has_prop_info);
		}
	} else {
		prop_ref = zend_jit_addr_offset(jit, obj_ref, prop_info->offset);
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);
		if (!ce || ce_is_instanceof || !(ce->ce_flags & ZEND_ACC_IMMUTABLE) || ce->__get || ce->__set || (prop_info->flags & ZEND_ACC_READONLY)) {
			// Undefined property with magic __get()/__set()
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM); // TODO: use slow path ???
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				zend_jit_guard(jit,
					zend_jit_zval_type_info(jit, prop_addr),
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				ir_ref if_def = zend_jit_if(jit, zend_jit_zval_type_info(jit, prop_addr));
				zend_jit_if_false_cold(jit, if_def);
				zend_jit_array_add(slow_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_def);
			}
		}
		if (ZEND_TYPE_IS_SET(prop_info->type)) {
			ir_ref ref;

			// JIT: value = zend_assign_to_typed_prop(prop_info, property_val, value EXECUTE_DATA_CC);
			zend_jit_set_ex_opline(jit, opline);
			if (ce && ce->ce_flags & ZEND_ACC_IMMUTABLE) {
				ref = zend_jit_const_addr(jit, (uintptr_t)prop_info);
			} else {
				int prop_info_offset =
					(((prop_info->offset - (sizeof(zend_object) - sizeof(zval))) / sizeof(zval)) * sizeof(void*));

				ref = zend_jit_load_at_offset(jit, IR_ADDR, obj_ref, offsetof(zend_object, ce));
				ref = zend_jit_load_at_offset(jit, IR_ADDR, ref, offsetof(zend_class_entry, properties_info_table));
				ref = zend_jit_load_at_offset(jit, IR_ADDR, ref, prop_info_offset);
			}
			zend_jit_call_4(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_assign_to_typed_prop, IR_CONST_FASTCALL_FUNC),
				prop_ref,
				ref,
				zend_jit_zval_addr(jit, val_addr),
				RETURN_VALUE_USED(opline) ? zend_jit_zval_addr(jit, res_addr) : IR_NULL);

			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	}

	if (!prop_info || !ZEND_TYPE_IS_SET(prop_info->type)) {
		if (opline->result_type == IS_UNUSED) {
			if (!zend_jit_assign_to_variable_call(jit, opline, prop_addr, prop_addr, -1, -1, (opline+1)->op1_type, val_addr, val_info, res_addr, 0)) {
				return 0;
			}
		} else {
			if (!zend_jit_assign_to_variable(jit, opline, prop_addr, prop_addr, -1, -1, (opline+1)->op1_type, val_addr, val_info, res_addr, 0, 0)) {
				return 0;
			}
		}
		if (end_inputs_count || slow_inputs_count) {
			if (((opline+1)->op1_type & (IS_VAR|IS_TMP_VAR))
			 && (val_info & (MAY_BE_REF|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
				/* skip FREE_OP_DATA() */
				delayed_end_input = zend_jit_end(jit);
			} else {
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}
		}
	}

	if (slow_inputs_count) {
		zend_jit_merge_N(jit, slow_inputs_count, slow_inputs);
		zend_jit_set_ex_opline(jit, opline);

		// JIT: value = zobj->handlers->write_property(zobj, name, value, CACHE_ADDR(opline->extended_value));
		ir_ref run_time_cache = zend_jit_load_at_offset(jit, IR_ADDR,
			zend_jit_fp(jit),
			offsetof(zend_execute_data, run_time_cache));
		zend_jit_call_5(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_assign_obj_helper, IR_CONST_FASTCALL_FUNC),
			obj_ref,
			zend_jit_const_addr(jit, (uintptr_t)name),
			zend_jit_zval_addr(jit, val_addr),
			zend_jit_addr_offset(jit, run_time_cache, opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS),
			RETURN_VALUE_USED(opline) ? zend_jit_zval_addr(jit, res_addr) : IR_NULL);

		zend_jit_array_add(end_inputs, zend_jit_end(jit));
	}

	if (end_inputs_count) {
		zend_jit_merge_N(jit, end_inputs_count, end_inputs);

		if (val_info & (MAY_BE_REF|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
			val_info |= MAY_BE_RC1|MAY_BE_RCN;
		}
		zend_jit_free_op(jit, (opline+1)->op1_type, (opline+1)->op1, val_info, opline);

		if (delayed_end_input) {
			zend_jit_merge_2(jit, zend_jit_end(jit), delayed_end_input);
		}
	}

	if (opline->op1_type != IS_UNUSED && !delayed_fetch_this && !op1_indirect) {
		zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);
	}

	if (may_throw) {
		zend_jit_check_exception(jit);
	}

	return 1;
}

static int zend_jit_assign_obj_op(zend_jit_ctx         *jit,
                                  const zend_op        *opline,
                                  const zend_op_array  *op_array,
                                  zend_ssa             *ssa,
                                  const zend_ssa_op    *ssa_op,
                                  uint32_t              op1_info,
                                  zend_jit_addr         op1_addr,
                                  uint32_t              val_info,
                                  zend_ssa_range       *val_range,
                                  bool                  op1_indirect,
                                  zend_class_entry     *ce,
                                  bool                  ce_is_instanceof,
                                  bool                  on_this,
                                  bool                  delayed_fetch_this,
                                  zend_class_entry     *trace_ce,
                                  uint8_t               prop_type)
{
	zval *member;
	zend_string *name;
	zend_property_info *prop_info;
	zend_jit_addr val_addr = OP1_DATA_ADDR();
	zend_jit_addr prop_addr;
	bool use_prop_guard = 0;
	bool may_throw = 0;
	binary_op_type binary_op = get_binary_op(opline->extended_value);
	ir_ref obj_ref = IR_UNUSED;
	ir_ref prop_ref = IR_UNUSED;
	ir_ref end_inputs[8];
	uint32_t end_inputs_count = 0;
	ir_ref slow_inputs[8];
	uint32_t slow_inputs_count = 0;

	ZEND_ASSERT(opline->op2_type == IS_CONST);
	ZEND_ASSERT(op1_info & MAY_BE_OBJECT);
	ZEND_ASSERT(opline->result_type == IS_UNUSED);

	member = RT_CONSTANT(opline, opline->op2);
	ZEND_ASSERT(Z_TYPE_P(member) == IS_STRING && Z_STRVAL_P(member)[0] != '\0');
	name = Z_STR_P(member);
	prop_info = zend_get_known_property_info(op_array, ce, name, on_this, op_array->filename);

	if (on_this) {
		zend_jit_addr this_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, offsetof(zend_execute_data, This));
		obj_ref = zend_jit_zval_ptr(jit, this_addr);
	} else {
		if (opline->op1_type == IS_VAR
		 && (op1_info & MAY_BE_INDIRECT)
		 && Z_REG(op1_addr) == ZREG_FP) {
			op1_addr = zend_jit_zval_indirect_deref(jit, op1_addr);
		}
		if (op1_info & MAY_BE_REF) {
			op1_addr = zend_jit_zval_deref(jit, op1_addr);
		}
		if (op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY)- MAY_BE_OBJECT)) {
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				zend_jit_guard_zval_type(jit, op1_addr, IS_OBJECT, exit_addr);
			} else {
				ir_ref if_obj = zend_jit_if_zval_type(jit, op1_addr, IS_OBJECT);
				zend_jit_if_false_cold(jit, if_obj);

				zend_jit_set_ex_opline(jit, opline);
				zend_jit_call_2(jit, IR_VOID,
					(op1_info & MAY_BE_UNDEF) ?
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_invalid_property_assign_op, IR_CONST_FASTCALL_FUNC) :
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_invalid_property_assign, IR_CONST_FASTCALL_FUNC),
					zend_jit_zval_addr(jit, op1_addr),
					zend_jit_const_addr(jit, (uintptr_t)ZSTR_VAL(name)));

				may_throw = 1;

				zend_jit_array_add(end_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_obj);
			}
		}
		obj_ref = zend_jit_zval_ptr(jit, op1_addr);
	}

	ZEND_ASSERT(obj_ref);
	if (!prop_info && trace_ce && (trace_ce->ce_flags & ZEND_ACC_IMMUTABLE)) {
		prop_info = zend_get_known_property_info(op_array, trace_ce, name, on_this, op_array->filename);
		if (prop_info) {
			ce = trace_ce;
			ce_is_instanceof = 0;
			if (!(op1_info & MAY_BE_CLASS_GUARD)) {
				if (on_this && JIT_G(current_frame)
				 && TRACE_FRAME_IS_THIS_CLASS_CHECKED(JIT_G(current_frame))) {
					ZEND_ASSERT(JIT_G(current_frame)->ce == ce);
				} else if (zend_jit_class_guard(jit, opline, obj_ref, ce)) {
					if (on_this && JIT_G(current_frame)) {
						JIT_G(current_frame)->ce = ce;
						TRACE_FRAME_SET_THIS_CLASS_CHECKED(JIT_G(current_frame));
					}
				} else {
					return 0;
				}
				if (ssa->var_info && ssa_op->op1_use >= 0) {
					ssa->var_info[ssa_op->op1_use].type |= MAY_BE_CLASS_GUARD;
					ssa->var_info[ssa_op->op1_use].ce = ce;
					ssa->var_info[ssa_op->op1_use].is_instanceof = ce_is_instanceof;
				}
				if (ssa->var_info && ssa_op->op1_def >= 0) {
					ssa->var_info[ssa_op->op1_def].type |= MAY_BE_CLASS_GUARD;
					ssa->var_info[ssa_op->op1_def].ce = ce;
					ssa->var_info[ssa_op->op1_def].is_instanceof = ce_is_instanceof;
				}
			}
		}
	}

	use_prop_guard = (prop_type != IS_UNKNOWN
		&& prop_type != IS_UNDEF
		&& prop_type != IS_REFERENCE
		&& (op1_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_OBJECT);

	if (!prop_info) {
		ir_ref run_time_cache = zend_jit_load_at_offset(jit, IR_ADDR,
			zend_jit_fp(jit),
			offsetof(zend_execute_data, run_time_cache));
		ir_ref ref = zend_jit_load_at_offset(jit, IR_ADDR,
			run_time_cache,
			(opline+1)->extended_value & ~ZEND_FETCH_OBJ_FLAGS);
		ir_ref if_same = zend_jit_if(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_ADDR),
				ref,
				zend_jit_load_at_offset(jit, IR_ADDR,
					obj_ref,
					offsetof(zend_object, ce))));

		zend_jit_if_false_cold(jit, if_same);
		zend_jit_array_add(slow_inputs, zend_jit_end(jit));

		zend_jit_if_true(jit, if_same);
		if (!ce || ce_is_instanceof || (ce->ce_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
			ir_ref prop_info_ref = zend_jit_load_at_offset(jit, IR_ADDR,
				run_time_cache,
				((opline+1)->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*) * 2);
			ir_ref if_has_prop_info = zend_jit_if(jit, prop_info_ref);
			zend_jit_if_true_cold(jit, if_has_prop_info);
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));

			zend_jit_if_false(jit, if_has_prop_info);
		}
		ir_ref offset_ref = zend_jit_load_at_offset(jit, IR_ADDR,
			run_time_cache,
			((opline+1)->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*));

		ir_ref if_dynamic = zend_jit_if(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_LT, IR_BOOL),
				offset_ref,
				IR_NULL));
		zend_jit_if_true_cold(jit, if_dynamic);
		zend_jit_array_add(slow_inputs, zend_jit_end(jit));

		zend_jit_if_false(jit, if_dynamic);

		prop_ref = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR), obj_ref, offset_ref);
		if (!use_prop_guard) {
			ir_ref if_def = zend_jit_if(jit, zend_jit_zval_ref_type(jit, prop_ref));
			zend_jit_if_false_cold(jit, if_def);
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));

			zend_jit_if_true(jit, if_def);
		}
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);
	} else {
		prop_ref = zend_jit_addr_offset(jit, obj_ref, prop_info->offset);
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);

		if (ZEND_TYPE_IS_SET(prop_info->type) || !use_prop_guard) {
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				zend_jit_guard(jit,
					zend_jit_zval_type_info(jit, prop_addr),
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				ir_ref if_def = zend_jit_if(jit,
					zend_jit_zval_type_info(jit, prop_addr));
				zend_jit_if_false_cold(jit, if_def);
				zend_jit_array_add(slow_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_def);
			}
		}
		if (ZEND_TYPE_IS_SET(prop_info->type)) {
			ir_ref if_ref, if_typed, noref_path, ref_path, reference, ref;

			may_throw = 1;

			zend_jit_set_ex_opline(jit, opline);

			if_ref = zend_jit_if_zval_type(jit, prop_addr, IS_REFERENCE);
			zend_jit_if_false(jit, if_ref);
			noref_path = zend_jit_end(jit);
			zend_jit_if_true(jit, if_ref);

			reference = zend_jit_zval_ptr(jit, prop_addr);
			ref = zend_jit_addr_offset(jit, reference, offsetof(zend_reference, val));
			if_typed = zend_jit_if_typed_ref(jit, reference);
			zend_jit_if_false(jit, if_typed);
			ref_path = zend_jit_end(jit);
			zend_jit_if_true_cold(jit, if_typed);

			zend_jit_call_3(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_assign_op_to_typed_ref, IR_CONST_FASTCALL_FUNC),
				reference,
				zend_jit_zval_addr(jit, val_addr),
				zend_jit_const_func_addr(jit, (uintptr_t)binary_op, IR_CONST_FASTCALL_FUNC));

			zend_jit_array_add(end_inputs, zend_jit_end(jit));

			zend_jit_merge_2(jit, noref_path, ref_path);
			prop_ref = zend_jit_phi_2(jit, prop_ref, ref);
			prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);

			// JIT: value = zend_assign_to_typed_prop(prop_info, property_val, value EXECUTE_DATA_CC);
			if (ce && ce->ce_flags & ZEND_ACC_IMMUTABLE) {
				ref = zend_jit_const_addr(jit, (uintptr_t)prop_info);
			} else {
				int prop_info_offset =
					(((prop_info->offset - (sizeof(zend_object) - sizeof(zval))) / sizeof(zval)) * sizeof(void*));

				ref = zend_jit_load_at_offset(jit, IR_ADDR, obj_ref, offsetof(zend_object, ce));
				ref = zend_jit_load_at_offset(jit, IR_ADDR, ref, offsetof(zend_class_entry, properties_info_table));
				ref = zend_jit_load_at_offset(jit, IR_ADDR, ref, prop_info_offset);
			}

			zend_jit_call_4(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_assign_op_to_typed_prop, IR_CONST_FASTCALL_FUNC),
				prop_ref,
				ref,
				zend_jit_zval_addr(jit, val_addr),
				zend_jit_const_func_addr(jit, (uintptr_t)binary_op, IR_CONST_FASTCALL_FUNC));

			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	}

	if (!prop_info || !ZEND_TYPE_IS_SET(prop_info->type)) {
		zend_jit_addr var_addr = prop_addr;
		uint32_t var_info = MAY_BE_ANY|MAY_BE_REF|MAY_BE_RC1|MAY_BE_RCN;
		uint32_t var_def_info = MAY_BE_ANY|MAY_BE_REF|MAY_BE_RC1|MAY_BE_RCN;

		if (use_prop_guard) {
			int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);
			const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);
			if (!exit_addr) {
				return 0;
			}

			zend_jit_guard_zval_type(jit, prop_addr, prop_type, exit_addr);
			var_info = (1 << prop_type) | (var_info & ~(MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF));
		}

		if (var_info & MAY_BE_REF) {
			ir_ref if_ref, if_typed, noref_path, ref_path, reference, ref;

			may_throw = 1;

			if_ref = zend_jit_if_zval_type(jit, prop_addr, IS_REFERENCE);
			zend_jit_if_false(jit, if_ref);
			noref_path = zend_jit_end(jit);
			zend_jit_if_true(jit, if_ref);

			reference = zend_jit_zval_ptr(jit, var_addr);
			ref = zend_jit_addr_offset(jit, reference, offsetof(zend_reference, val));
			if_typed = zend_jit_if_typed_ref(jit, reference);
			zend_jit_if_false(jit, if_typed);
			ref_path = zend_jit_end(jit);
			zend_jit_if_true_cold(jit, if_typed);

			zend_jit_set_ex_opline(jit, opline);

			zend_jit_call_3(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_assign_op_to_typed_ref, IR_CONST_FASTCALL_FUNC),
				reference,
				zend_jit_zval_addr(jit, val_addr),
				zend_jit_const_func_addr(jit, (uintptr_t)binary_op, IR_CONST_FASTCALL_FUNC));

			zend_jit_array_add(end_inputs, zend_jit_end(jit));

			zend_jit_merge_2(jit, noref_path, ref_path);
			prop_ref = zend_jit_phi_2(jit, prop_ref, ref);
			var_addr = ZEND_ADDR_REF_ZVAL(prop_ref);

			var_info &= ~MAY_BE_REF;
		}

		uint8_t val_op_type = (opline+1)->op1_type;
		if (val_op_type & (IS_TMP_VAR|IS_VAR)) {
			/* prevent FREE_OP in the helpers */
			val_op_type = IS_CV;
		}

		switch (opline->extended_value) {
			case ZEND_ADD:
			case ZEND_SUB:
			case ZEND_MUL:
				if ((var_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) ||
				    (val_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
					if (opline->extended_value != ZEND_ADD ||
					    (var_info & MAY_BE_ANY) != MAY_BE_ARRAY ||
					    (val_info & MAY_BE_ANY) == MAY_BE_ARRAY) {
						may_throw = 1;
					}
				}
				if (!zend_jit_math_helper(jit, opline, opline->extended_value, IS_CV, opline->op1, var_addr, var_info, val_op_type, (opline+1)->op1, val_addr, val_info, 0, var_addr, var_def_info, var_info,
						1 /* may overflow */, 0)) {
					return 0;
				}
				break;
			case ZEND_BW_OR:
			case ZEND_BW_AND:
			case ZEND_BW_XOR:
				may_throw = 1;
				if ((var_info & (MAY_BE_STRING|MAY_BE_DOUBLE|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) ||
				    (val_info & (MAY_BE_STRING|MAY_BE_DOUBLE|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
					if ((var_info & MAY_BE_ANY) != MAY_BE_STRING ||
					    (val_info & MAY_BE_ANY) != MAY_BE_STRING) {
						may_throw = 1;
					}
				}
				goto long_math;
			case ZEND_SL:
			case ZEND_SR:
				if ((var_info & (MAY_BE_STRING|MAY_BE_DOUBLE|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) ||
				    (val_info & (MAY_BE_STRING|MAY_BE_DOUBLE|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
					may_throw = 1;
				}
				if (val_op_type != IS_CONST ||
				    Z_TYPE_P(RT_CONSTANT((opline+1), (opline+1)->op1)) != IS_LONG ||
				    Z_LVAL_P(RT_CONSTANT((opline+1), (opline+1)->op1)) < 0) {
					may_throw = 1;
				}
				goto long_math;
			case ZEND_MOD:
				if ((var_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) ||
				    (val_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
					if (opline->extended_value != ZEND_ADD ||
					    (var_info & MAY_BE_ANY) != MAY_BE_ARRAY ||
					    (val_info & MAY_BE_ANY) == MAY_BE_ARRAY) {
						may_throw = 1;
					}
				}
				if (val_op_type != IS_CONST ||
				    Z_TYPE_P(RT_CONSTANT((opline+1), (opline+1)->op1)) != IS_LONG ||
				    Z_LVAL_P(RT_CONSTANT((opline+1), (opline+1)->op1)) == 0) {
					may_throw = 1;
				}
long_math:
				if (!zend_jit_long_math_helper(jit, opline, opline->extended_value,
						IS_CV, opline->op1, var_addr, var_info, NULL,
						val_op_type, (opline+1)->op1, val_addr, val_info,
						val_range,
						0, var_addr, var_def_info, var_info, /* may throw */ 1)) {
					return 0;
				}
				break;
			case ZEND_CONCAT:
				may_throw = 1;
				if (!zend_jit_concat_helper(jit, opline, IS_CV, opline->op1, var_addr, var_info, val_op_type, (opline+1)->op1, val_addr, val_info, var_addr,
						0)) {
					return 0;
				}
				break;
			default:
				ZEND_UNREACHABLE();
		}
		if (end_inputs_count || slow_inputs_count) {
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	}

	if (slow_inputs_count) {
		zend_jit_merge_N(jit, slow_inputs_count, slow_inputs);

		may_throw = 1;

		zend_jit_set_ex_opline(jit, opline);
		ir_ref run_time_cache = zend_jit_load_at_offset(jit, IR_ADDR,
			zend_jit_fp(jit),
			offsetof(zend_execute_data, run_time_cache));
		zend_jit_call_5(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_assign_obj_op_helper, IR_CONST_FASTCALL_FUNC),
			obj_ref,
			zend_jit_const_addr(jit, (uintptr_t)name),
			zend_jit_zval_addr(jit, val_addr),
			zend_jit_addr_offset(jit, run_time_cache, (opline+1)->extended_value & ~ZEND_FETCH_OBJ_FLAGS),
			zend_jit_const_func_addr(jit, (uintptr_t)binary_op, IR_CONST_FASTCALL_FUNC));

		zend_jit_array_add(end_inputs, zend_jit_end(jit));
	}

	if (end_inputs_count) {
		zend_jit_merge_N(jit, end_inputs_count, end_inputs);
	}

	if (val_info & (MAY_BE_REF|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
		val_info |= MAY_BE_RC1|MAY_BE_RCN;
	}

	// JIT: FREE_OP_DATA();
	zend_jit_free_op(jit, (opline+1)->op1_type, (opline+1)->op1, val_info, opline);

	if (opline->op1_type != IS_UNUSED && !delayed_fetch_this && !op1_indirect) {
		if ((op1_info & MAY_HAVE_DTOR) && (op1_info & MAY_BE_RC1)) {
			may_throw = 1;
		}
		zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);
	}

	if (may_throw) {
		zend_jit_check_exception(jit);
	}

	return 1;
}

static int zend_jit_incdec_obj(zend_jit_ctx         *jit,
                               const zend_op        *opline,
                               const zend_op_array  *op_array,
                               zend_ssa             *ssa,
                               const zend_ssa_op    *ssa_op,
                               uint32_t              op1_info,
                               zend_jit_addr         op1_addr,
                               bool                  op1_indirect,
                               zend_class_entry     *ce,
                               bool                  ce_is_instanceof,
                               bool                  on_this,
                               bool                  delayed_fetch_this,
                               zend_class_entry     *trace_ce,
                               uint8_t               prop_type)
{
	zval *member;
	zend_string *name;
	zend_property_info *prop_info;
	zend_jit_addr res_addr = 0;
	zend_jit_addr prop_addr;
	bool use_prop_guard = 0;
	bool may_throw = 0;
	uint32_t res_info = (opline->result_type != IS_UNDEF) ? RES_INFO() : 0;
	ir_ref obj_ref = IR_UNUSED;
	ir_ref prop_ref = IR_UNUSED;
	ir_ref end_inputs[8];
	uint32_t end_inputs_count = 0;
	ir_ref slow_inputs[8];
	uint32_t slow_inputs_count = 0;

	ZEND_ASSERT(opline->op2_type == IS_CONST);
	ZEND_ASSERT(op1_info & MAY_BE_OBJECT);

	if (opline->result_type != IS_UNUSED) {
		res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var);
	}

	member = RT_CONSTANT(opline, opline->op2);
	ZEND_ASSERT(Z_TYPE_P(member) == IS_STRING && Z_STRVAL_P(member)[0] != '\0');
	name = Z_STR_P(member);
	prop_info = zend_get_known_property_info(op_array, ce, name, on_this, op_array->filename);

	if (on_this) {
		zend_jit_addr this_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, offsetof(zend_execute_data, This));
		obj_ref = zend_jit_zval_ptr(jit, this_addr);
	} else {
		if (opline->op1_type == IS_VAR
		 && (op1_info & MAY_BE_INDIRECT)
		 && Z_REG(op1_addr) == ZREG_FP) {
			op1_addr = zend_jit_zval_indirect_deref(jit, op1_addr);
		}
		if (op1_info & MAY_BE_REF) {
			op1_addr = zend_jit_zval_deref(jit, op1_addr);
		}
		if (op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY)- MAY_BE_OBJECT)) {
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				zend_jit_guard_zval_type(jit, op1_addr, IS_OBJECT, exit_addr);
			} else {
				ir_ref if_obj = zend_jit_if_zval_type(jit, op1_addr, IS_OBJECT);
				zend_jit_if_false_cold(jit, if_obj);

				zend_jit_set_ex_opline(jit, opline);
				zend_jit_call_2(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_invalid_property_incdec, IR_CONST_FASTCALL_FUNC),
					zend_jit_zval_addr(jit, op1_addr),
					zend_jit_const_addr(jit, (uintptr_t)ZSTR_VAL(name)));

				may_throw = 1;

				if ((opline->op1_type & (IS_VAR|IS_TMP_VAR)) && !delayed_fetch_this && !op1_indirect) {
					zend_jit_array_add(end_inputs, zend_jit_end(jit));
				} else {
					zend_jit_ijmp(jit, zend_jit_stub_addr(jit, jit_stub_exception_handler));
				}
				zend_jit_if_true(jit, if_obj);
			}
		}
		obj_ref = zend_jit_zval_ptr(jit, op1_addr);
	}

	ZEND_ASSERT(obj_ref);
	if (!prop_info && trace_ce && (trace_ce->ce_flags & ZEND_ACC_IMMUTABLE)) {
		prop_info = zend_get_known_property_info(op_array, trace_ce, name, on_this, op_array->filename);
		if (prop_info) {
			ce = trace_ce;
			ce_is_instanceof = 0;
			if (!(op1_info & MAY_BE_CLASS_GUARD)) {
				if (on_this && JIT_G(current_frame)
				 && TRACE_FRAME_IS_THIS_CLASS_CHECKED(JIT_G(current_frame))) {
					ZEND_ASSERT(JIT_G(current_frame)->ce == ce);
				} else if (zend_jit_class_guard(jit, opline, obj_ref, ce)) {
					if (on_this && JIT_G(current_frame)) {
						JIT_G(current_frame)->ce = ce;
						TRACE_FRAME_SET_THIS_CLASS_CHECKED(JIT_G(current_frame));
					}
				} else {
					return 0;
				}
				if (ssa->var_info && ssa_op->op1_use >= 0) {
					ssa->var_info[ssa_op->op1_use].type |= MAY_BE_CLASS_GUARD;
					ssa->var_info[ssa_op->op1_use].ce = ce;
					ssa->var_info[ssa_op->op1_use].is_instanceof = ce_is_instanceof;
				}
				if (ssa->var_info && ssa_op->op1_def >= 0) {
					ssa->var_info[ssa_op->op1_def].type |= MAY_BE_CLASS_GUARD;
					ssa->var_info[ssa_op->op1_def].ce = ce;
					ssa->var_info[ssa_op->op1_def].is_instanceof = ce_is_instanceof;
				}
			}
		}
	}

	use_prop_guard = (prop_type != IS_UNKNOWN
		&& prop_type != IS_UNDEF
		&& prop_type != IS_REFERENCE
		&& (op1_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_OBJECT);

	if (!prop_info) {
		ir_ref run_time_cache = zend_jit_load_at_offset(jit, IR_ADDR,
			zend_jit_fp(jit),
			offsetof(zend_execute_data, run_time_cache));
		ir_ref ref = zend_jit_load_at_offset(jit, IR_ADDR,
			run_time_cache,
			opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS);
		ir_ref if_same = zend_jit_if(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_ADDR),
				ref,
				zend_jit_load_at_offset(jit, IR_ADDR,
					obj_ref,
					offsetof(zend_object, ce))));

		zend_jit_if_false_cold(jit, if_same);
		zend_jit_array_add(slow_inputs, zend_jit_end(jit));

		zend_jit_if_true(jit, if_same);
		if (!ce || ce_is_instanceof || (ce->ce_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
			ir_ref prop_info_ref = zend_jit_load_at_offset(jit, IR_ADDR,
				run_time_cache,
				(opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*) * 2);
			ir_ref if_has_prop_info = zend_jit_if(jit, prop_info_ref);
			zend_jit_if_true_cold(jit, if_has_prop_info);
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));

			zend_jit_if_false(jit, if_has_prop_info);
		}
		ir_ref offset_ref = zend_jit_load_at_offset(jit, IR_ADDR,
			run_time_cache,
			(opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*));

		ir_ref if_dynamic = zend_jit_if(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_LT, IR_BOOL),
				offset_ref,
				IR_NULL));
		zend_jit_if_true_cold(jit, if_dynamic);
		zend_jit_array_add(slow_inputs, zend_jit_end(jit));

		zend_jit_if_false(jit, if_dynamic);

		prop_ref = ir_fold2(&jit->ctx, IR_OPT(IR_ADD, IR_ADDR), obj_ref, offset_ref);
		if (!use_prop_guard) {
			ir_ref if_def = zend_jit_if(jit, zend_jit_zval_ref_type(jit, prop_ref));
			zend_jit_if_false_cold(jit, if_def);
			zend_jit_array_add(slow_inputs, zend_jit_end(jit));

			zend_jit_if_true(jit, if_def);
		}
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);
	} else {
		prop_ref = zend_jit_addr_offset(jit, obj_ref, prop_info->offset);
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);

		if (ZEND_TYPE_IS_SET(prop_info->type) || !use_prop_guard) {
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				zend_jit_guard(jit,
					zend_jit_zval_type_info(jit, prop_addr),
					zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				ir_ref if_def = zend_jit_if(jit,
					zend_jit_zval_type_info(jit, prop_addr));
				zend_jit_if_false_cold(jit, if_def);
				zend_jit_array_add(slow_inputs, zend_jit_end(jit));
				zend_jit_if_true(jit, if_def);
			}
		}

		if (ZEND_TYPE_IS_SET(prop_info->type)) {
			const void *func;
			ir_ref ref;

			may_throw = 1;
			zend_jit_set_ex_opline(jit, opline);

			if (ce && ce->ce_flags & ZEND_ACC_IMMUTABLE) {
				ref = zend_jit_const_addr(jit, (uintptr_t)prop_info);
			} else {
				int prop_info_offset =
					(((prop_info->offset - (sizeof(zend_object) - sizeof(zval))) / sizeof(zval)) * sizeof(void*));

				ref = zend_jit_load_at_offset(jit, IR_ADDR, obj_ref, offsetof(zend_object, ce));
				ref = zend_jit_load_at_offset(jit, IR_ADDR, ref, offsetof(zend_class_entry, properties_info_table));
				ref = zend_jit_load_at_offset(jit, IR_ADDR, ref, prop_info_offset);
			}

			if (opline->result_type == IS_UNUSED) {
				switch (opline->opcode) {
					case ZEND_PRE_INC_OBJ:
					case ZEND_POST_INC_OBJ:
						func = zend_jit_inc_typed_prop;
						break;
					case ZEND_PRE_DEC_OBJ:
					case ZEND_POST_DEC_OBJ:
						func = zend_jit_dec_typed_prop;
						break;
					default:
						ZEND_UNREACHABLE();
				}

				zend_jit_call_2(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)func, IR_CONST_FASTCALL_FUNC),
					prop_ref,
					ref);
			} else {
				switch (opline->opcode) {
					case ZEND_PRE_INC_OBJ:
						func = zend_jit_pre_inc_typed_prop;
						break;
					case ZEND_PRE_DEC_OBJ:
						func = zend_jit_pre_dec_typed_prop;
						break;
					case ZEND_POST_INC_OBJ:
						func = zend_jit_post_inc_typed_prop;
						break;
					case ZEND_POST_DEC_OBJ:
						func = zend_jit_post_dec_typed_prop;
						break;
					default:
						ZEND_UNREACHABLE();
				}
				zend_jit_call_3(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)func, IR_CONST_FASTCALL_FUNC),
					prop_ref,
					ref,
					zend_jit_zval_addr(jit, res_addr));
			}
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
	}

	if (!prop_info || !ZEND_TYPE_IS_SET(prop_info->type)) {
		uint32_t var_info = MAY_BE_ANY|MAY_BE_REF|MAY_BE_RC1|MAY_BE_RCN;
		zend_jit_addr var_addr = prop_addr;
		ir_ref if_long = IR_UNUSED;
		ir_ref if_overflow = IR_UNUSED;

		if (use_prop_guard) {
			int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);
			const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);
			if (!exit_addr) {
				return 0;
			}

			zend_jit_guard_zval_type(jit, prop_addr, prop_type, exit_addr);
			var_info = (1 << prop_type) | (var_info & ~(MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF));
		}

		if (var_info & MAY_BE_REF) {
			const void *func;
			ir_ref if_ref, if_typed, noref_path, ref_path, reference, ref;

			if_ref = zend_jit_if_zval_type(jit, prop_addr, IS_REFERENCE);
			zend_jit_if_false(jit, if_ref);
			noref_path = zend_jit_end(jit);
			zend_jit_if_true(jit, if_ref);

			reference = zend_jit_zval_ptr(jit, var_addr);
			ref = zend_jit_addr_offset(jit, reference, offsetof(zend_reference, val));
			if_typed = zend_jit_if_typed_ref(jit, reference);
			zend_jit_if_false(jit, if_typed);
			ref_path = zend_jit_end(jit);
			zend_jit_if_true_cold(jit, if_typed);

			switch (opline->opcode) {
				case ZEND_PRE_INC_OBJ:
					func = zend_jit_pre_inc_typed_ref;
					break;
				case ZEND_PRE_DEC_OBJ:
					func = zend_jit_pre_dec_typed_ref;
					break;
				case ZEND_POST_INC_OBJ:
					func = zend_jit_post_inc_typed_ref;
					break;
				case ZEND_POST_DEC_OBJ:
					func = zend_jit_post_dec_typed_ref;
					break;
				default:
					ZEND_UNREACHABLE();
			}

			may_throw = 1;
			zend_jit_set_ex_opline(jit, opline);
			zend_jit_call_2(jit, IR_VOID,
				zend_jit_const_func_addr(jit, (uintptr_t)func, IR_CONST_FASTCALL_FUNC),
				reference,
				(opline->result_type == IS_UNUSED) ? IR_NULL : zend_jit_zval_addr(jit, res_addr));

			zend_jit_array_add(end_inputs, zend_jit_end(jit));

			zend_jit_merge_2(jit, noref_path, ref_path);
			prop_ref = zend_jit_phi_2(jit, prop_ref, ref);
			var_addr = ZEND_ADDR_REF_ZVAL(prop_ref);

			var_info &= ~MAY_BE_REF;
		}

		if (var_info & MAY_BE_LONG) {
			ir_ref addr, ref;

			if (var_info & (MAY_BE_ANY - MAY_BE_LONG)) {
				if_long = zend_jit_if_zval_type(jit, var_addr, IS_LONG);
				zend_jit_if_true(jit, if_long);
			}

			addr = zend_jit_zval_addr(jit, var_addr);
			ref = zend_jit_load(jit, IR_PHP_LONG, addr);
			if (opline->opcode == ZEND_POST_INC_OBJ || opline->opcode == ZEND_POST_DEC_OBJ) {
				if (opline->result_type != IS_UNUSED) {
					zend_jit_zval_set_lval(jit, res_addr, ref);
					zend_jit_zval_set_type_info(jit, res_addr, IS_LONG);
				}
			}
			if (opline->opcode == ZEND_PRE_INC_OBJ || opline->opcode == ZEND_POST_INC_OBJ) {
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_ADD_OV, IR_PHP_LONG),
					ref,
					ir_const_php_long(&jit->ctx, 1));
			} else {
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_SUB_OV, IR_PHP_LONG),
					ref,
					ir_const_php_long(&jit->ctx, 1));
			}

			zend_jit_store(jit, addr, ref);
			if_overflow = zend_jit_if(jit,
				ir_fold1(&jit->ctx, IR_OPT(IR_OVERFLOW, IR_BOOL), ref));
			zend_jit_if_false(jit, if_overflow);

			if (opline->opcode == ZEND_PRE_INC_OBJ || opline->opcode == ZEND_PRE_DEC_OBJ) {
				if (opline->result_type != IS_UNUSED) {
					zend_jit_zval_set_lval(jit, res_addr, ref);
					zend_jit_zval_set_type_info(jit, res_addr, IS_LONG);
				}
			}
			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}

		if (var_info & (MAY_BE_ANY - MAY_BE_LONG)) {
			if (var_info & (MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
				may_throw = 1;
			}
			if (if_long) {
				zend_jit_if_false_cold(jit, if_long);
			}
			if (opline->opcode == ZEND_POST_INC_OBJ || opline->opcode == ZEND_POST_DEC_OBJ) {
				zend_jit_copy_zval(jit, res_addr, -1, var_addr, var_info, 1);
			}
			if (opline->opcode == ZEND_PRE_INC_OBJ || opline->opcode == ZEND_POST_INC_OBJ) {
				if (opline->opcode == ZEND_PRE_INC_OBJ && opline->result_type != IS_UNUSED) {
					zend_jit_call_2(jit, IR_VOID,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_pre_inc, IR_CONST_FASTCALL_FUNC),
						zend_jit_zval_addr(jit, var_addr),
						zend_jit_zval_addr(jit, res_addr));
				} else {
					zend_jit_call_1(jit, IR_VOID,
						zend_jit_const_func_addr(jit, (uintptr_t)increment_function, IR_CONST_FASTCALL_FUNC),
						zend_jit_zval_addr(jit, var_addr));
				}
			} else {
				if (opline->opcode == ZEND_PRE_DEC_OBJ && opline->result_type != IS_UNUSED) {
					zend_jit_call_2(jit, IR_VOID,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_pre_dec, IR_CONST_FASTCALL_FUNC),
						zend_jit_zval_addr(jit, var_addr),
						zend_jit_zval_addr(jit, res_addr));
				} else {
					zend_jit_call_1(jit, IR_VOID,
						zend_jit_const_func_addr(jit, (uintptr_t)decrement_function, IR_CONST_FASTCALL_FUNC),
						zend_jit_zval_addr(jit, var_addr));
				}
			}

			zend_jit_array_add(end_inputs, zend_jit_end(jit));
		}
		if (var_info & MAY_BE_LONG) {
			zend_jit_if_true_cold(jit, if_overflow);
			if (opline->opcode == ZEND_PRE_INC_OBJ || opline->opcode == ZEND_POST_INC_OBJ) {
#if SIZEOF_ZEND_LONG == 4
				zend_jit_zval_set_lval(jit, var_addr,
					ir_const_php_long(&jit->ctx, 0));
				zend_jit_zval_set_w2(jit, var_addr,
					ir_const_u32(&jit->ctx, 0x41e00000));
#else
				zend_jit_zval_set_lval(jit, var_addr,
					ir_const_php_long(&jit->ctx, 0x43e0000000000000));
#endif
				zend_jit_zval_set_type_info(jit, var_addr, IS_DOUBLE);
				if (opline->opcode == ZEND_PRE_INC_OBJ && opline->result_type != IS_UNUSED) {
#if SIZEOF_ZEND_LONG == 4
					zend_jit_zval_set_lval(jit, res_addr,
						ir_const_php_long(&jit->ctx, 0));
					zend_jit_zval_set_w2(jit, res_addr,
						ir_const_u32(&jit->ctx, 0x41e00000));
#else
					zend_jit_zval_set_lval(jit, res_addr,
						ir_const_php_long(&jit->ctx, 0x43e0000000000000));
#endif
					zend_jit_zval_set_type_info(jit, res_addr, IS_DOUBLE);
				}
			} else {
#if SIZEOF_ZEND_LONG == 4
				zend_jit_zval_set_lval(jit, var_addr,
					ir_const_php_long(&jit->ctx, 0x00200000));
				zend_jit_zval_set_w2(jit, var_addr,
					ir_const_u32(&jit->ctx, 0xc1e00000));
#else
				zend_jit_zval_set_lval(jit, var_addr,
					ir_const_php_long(&jit->ctx, 0xc3e0000000000000));
#endif
				zend_jit_zval_set_type_info(jit, var_addr, IS_DOUBLE);
				if (opline->opcode == ZEND_PRE_DEC_OBJ && opline->result_type != IS_UNUSED) {
#if SIZEOF_ZEND_LONG == 4
					zend_jit_zval_set_lval(jit, res_addr,
						ir_const_php_long(&jit->ctx, 0x00200000));
					zend_jit_zval_set_w2(jit, res_addr,
						ir_const_u32(&jit->ctx, 0xc1e00000));
#else
					zend_jit_zval_set_lval(jit, res_addr,
						ir_const_php_long(&jit->ctx, 0xc3e0000000000000));
#endif
					zend_jit_zval_set_type_info(jit, res_addr, IS_DOUBLE);
				}
			}
			if (opline->result_type != IS_UNUSED
			 && (opline->opcode == ZEND_PRE_INC_OBJ || opline->opcode == ZEND_PRE_DEC_OBJ)
			 && prop_info
			 && !ZEND_TYPE_IS_SET(prop_info->type)
			 && (res_info & MAY_BE_GUARD)
			 && (res_info & MAY_BE_LONG)) {
				zend_jit_trace_stack *stack = JIT_G(current_frame)->stack;
				uint32_t old_res_info = STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var));
				int32_t exit_point;
				const void *exit_addr;

				SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_DOUBLE, 0);
				exit_point = zend_jit_trace_get_exit_point(opline + 1, 0);
				exit_addr = zend_jit_trace_get_exit_addr(exit_point);
				if (!exit_addr) {
					return 0;
				}
				SET_STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var), old_res_info);
				ssa->var_info[ssa_op->result_def].type = res_info & ~MAY_BE_GUARD;
				zend_jit_side_exit(jit, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
			} else {
				zend_jit_array_add(end_inputs, zend_jit_end(jit));
			}
		}
	}

	if (slow_inputs_count) {
		const void *func;

		zend_jit_merge_N(jit, slow_inputs_count, slow_inputs);

		// JIT: zend_jit_pre_inc_obj_helper(zobj, name, CACHE_ADDR(opline->extended_value), result);
		switch (opline->opcode) {
			case ZEND_PRE_INC_OBJ:
				func = zend_jit_pre_inc_obj_helper;
				break;
			case ZEND_PRE_DEC_OBJ:
				func = zend_jit_pre_dec_obj_helper;
				break;
			case ZEND_POST_INC_OBJ:
				func = zend_jit_post_inc_obj_helper;
				break;
			case ZEND_POST_DEC_OBJ:
				func = zend_jit_post_dec_obj_helper;
				break;
			default:
				ZEND_UNREACHABLE();
		}

		may_throw = 1;
		zend_jit_set_ex_opline(jit, opline);
		ir_ref run_time_cache = zend_jit_load_at_offset(jit, IR_ADDR,
			zend_jit_fp(jit),
			offsetof(zend_execute_data, run_time_cache));
		zend_jit_call_4(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)func, IR_CONST_FASTCALL_FUNC),
			obj_ref,
			zend_jit_const_addr(jit, (uintptr_t)name),
			zend_jit_addr_offset(jit, run_time_cache, opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS),
			(opline->result_type == IS_UNUSED) ? IR_NULL : zend_jit_zval_addr(jit, res_addr));

		zend_jit_array_add(end_inputs, zend_jit_end(jit));
	}

	if (end_inputs_count) {
		zend_jit_merge_N(jit, end_inputs_count, end_inputs);
	}

	if ((opline->op1_type & (IS_VAR|IS_TMP_VAR)) && !delayed_fetch_this && !op1_indirect) {
		if ((op1_info & MAY_HAVE_DTOR) && (op1_info & MAY_BE_RC1)) {
			may_throw = 1;
		}
		zend_jit_free_op(jit, opline->op1_type, opline->op1, op1_info, opline);
	}

	if (may_throw) {
		zend_jit_check_exception(jit);
	}

	return 1;
}

static int zend_jit_switch(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, zend_ssa *ssa, zend_jit_trace_rec *trace, zend_jit_trace_info *trace_info)
{
	HashTable *jumptable = Z_ARRVAL_P(RT_CONSTANT(opline, opline->op2));
	const zend_op *next_opline = NULL;
	ir_ref slow_inputs[8];
	uint32_t slow_inputs_count = 0;

	if (trace) {
		ZEND_ASSERT(trace->op == ZEND_JIT_TRACE_VM || trace->op == ZEND_JIT_TRACE_END);
		ZEND_ASSERT(trace->opline != NULL);
		next_opline = trace->opline;
	}

	if (opline->op1_type == IS_CONST) {
		zval *zv = RT_CONSTANT(opline, opline->op1);
		zval *jump_zv = NULL;
		int b;

		if (opline->opcode == ZEND_SWITCH_LONG) {
			if (Z_TYPE_P(zv) == IS_LONG) {
				jump_zv = zend_hash_index_find(jumptable, Z_LVAL_P(zv));
			}
		} else if (opline->opcode == ZEND_SWITCH_STRING) {
			if (Z_TYPE_P(zv) == IS_STRING) {
				jump_zv = zend_hash_find_known_hash(jumptable, Z_STR_P(zv));
			}
		} else if (opline->opcode == ZEND_MATCH) {
			if (Z_TYPE_P(zv) == IS_LONG) {
				jump_zv = zend_hash_index_find(jumptable, Z_LVAL_P(zv));
			} else if (Z_TYPE_P(zv) == IS_STRING) {
				jump_zv = zend_hash_find_known_hash(jumptable, Z_STR_P(zv));
			}
		} else {
			ZEND_UNREACHABLE();
		}
		if (next_opline) {
			const zend_op *target;

			if (jump_zv != NULL) {
				target = ZEND_OFFSET_TO_OPLINE(opline, Z_LVAL_P(jump_zv));
			} else {
				target = ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value);
			}
			ZEND_ASSERT(target == next_opline);
		} else {
			if (jump_zv != NULL) {
				b = ssa->cfg.map[ZEND_OFFSET_TO_OPLINE(opline, Z_LVAL_P(jump_zv)) - op_array->opcodes];
			} else {
				b = ssa->cfg.map[ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value) - op_array->opcodes];
			}
			_zend_jit_add_predecessor_ref(jit, b, jit->b, zend_jit_end(jit));
			jit->control = IR_UNUSED;
			jit->b = -1;
		}
	} else {
		zend_ssa_op *ssa_op = &ssa->ops[opline - op_array->opcodes];
		uint32_t op1_info = OP1_INFO();
		zend_jit_addr op1_addr = OP1_ADDR();
		const zend_op *default_opline = ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value);
		const zend_op *target;
		int default_b = next_opline ? -1 : ssa->cfg.map[default_opline - op_array->opcodes];
		int b;
		int32_t exit_point;
		const void *exit_addr;
		const void *fallback_label = NULL;
		const void *default_label = NULL;
		zval *zv;

		if (next_opline) {
			if (opline->opcode != ZEND_MATCH && next_opline != opline + 1) {
				exit_point = zend_jit_trace_get_exit_point(opline + 1, 0);
				fallback_label = zend_jit_trace_get_exit_addr(exit_point);
				if (!fallback_label) {
					return 0;
				}
			}
			if (next_opline != default_opline) {
				exit_point = zend_jit_trace_get_exit_point(default_opline, 0);
				default_label = zend_jit_trace_get_exit_addr(exit_point);
				if (!default_label) {
					return 0;
				}
			}
		}

		if (opline->opcode == ZEND_SWITCH_LONG) {
			if (op1_info & MAY_BE_LONG) {
				if (op1_info & MAY_BE_REF) {
					ir_ref ref, if_long, fast_path, ref2;

					ref = zend_jit_zval_addr(jit, op1_addr);
					if_long = zend_jit_if_zval_type(jit, op1_addr, IS_LONG);
					zend_jit_if_true(jit, if_long);
					fast_path = zend_jit_end(jit);
					zend_jit_if_false_cold(jit, if_long);

					// JIT: ZVAL_DEREF(op)
					if (fallback_label) {
						zend_jit_guard_zval_type(jit, op1_addr, IS_REFERENCE, fallback_label);
					} else {
						ir_ref if_ref = zend_jit_if_zval_type(jit, op1_addr, IS_REFERENCE);
						zend_jit_if_false_cold(jit, if_ref);
						zend_jit_array_add(slow_inputs, zend_jit_end(jit));
						zend_jit_if_true(jit, if_ref);
					}

					ref2 = zend_jit_addr_offset(jit,
						zend_jit_zval_ptr(jit, op1_addr), offsetof(zend_reference, val));
					op1_addr = ZEND_ADDR_REF_ZVAL(ref2);

					if (fallback_label) {
						zend_jit_guard_zval_type(jit, op1_addr, IS_LONG, fallback_label);
					} else {
						if_long = zend_jit_if_zval_type(jit, op1_addr, IS_LONG);
						zend_jit_if_false_cold(jit, if_long);
						zend_jit_array_add(slow_inputs, zend_jit_end(jit));
						zend_jit_if_true(jit, if_long);
					}

					zend_jit_merge_2(jit, fast_path, zend_jit_end(jit));
					ref = zend_jit_phi_2(jit, ref, ref2);
					op1_addr = ZEND_ADDR_REF_ZVAL(ref);
				} else if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG)) {
					if (fallback_label) {
						zend_jit_guard_zval_type(jit, op1_addr, IS_LONG, fallback_label);
					} else {
						ir_ref if_long = zend_jit_if_zval_type(jit, op1_addr, IS_LONG);
						zend_jit_if_false_cold(jit, if_long);
						zend_jit_array_add(slow_inputs, zend_jit_end(jit));
						zend_jit_if_true(jit, if_long);
					}
				}
				ir_ref ref = zend_jit_zval_lval(jit, op1_addr);

				if (!HT_IS_PACKED(jumptable)) {
					ref = zend_jit_call_2(jit, IR_PHP_LONG,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_index_find, IR_CONST_FASTCALL_FUNC),
						zend_jit_const_addr(jit, (uintptr_t)jumptable),
						ref);
					ref = ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_PHP_LONG),
						ref,
						ir_const_php_long(&jit->ctx, (uintptr_t)jumptable->arData));
					ref = ir_fold2(&jit->ctx, IR_OPT(IR_DIV, IR_PHP_LONG),
						ref,
						ir_const_php_long(&jit->ctx, sizeof(Bucket)));
				}
				ref = ir_emit2(&jit->ctx, IR_SWITCH, jit->control, ref);

				if (next_opline) {
					ir_ref continue_list = IR_UNUSED;

					ZEND_HASH_FOREACH_VAL(jumptable, zv) {
						ir_ref idx;
						target = ZEND_OFFSET_TO_OPLINE(opline, Z_LVAL_P(zv));

						if (HT_IS_PACKED(jumptable)) {
							idx = ir_const_php_long(&jit->ctx, zv - jumptable->arPacked);
						} else {
							idx = ir_const_php_long(&jit->ctx, (Bucket*)zv - jumptable->arData);
						}
						jit->control = ir_emit2(&jit->ctx, IR_CASE_VAL, ref, idx);
						if (target == next_opline) {
							continue_list = zend_jit_list_add(jit, continue_list, zend_jit_end(jit));
						} else {
							exit_point = zend_jit_trace_get_exit_point(target, 0);
							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								return 0;
							}
							zend_jit_side_exit(jit, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
						}
					} ZEND_HASH_FOREACH_END();

					jit->control = ir_emit1(&jit->ctx, IR_CASE_DEFAULT, ref);
					if (next_opline == default_opline) {
						continue_list = zend_jit_list_add(jit, continue_list, zend_jit_end(jit));
					} else {
						zend_jit_side_exit(jit, zend_jit_const_addr(jit, (uintptr_t)default_label));
					}
					if (continue_list) {
						zend_jit_merge_list(jit, continue_list);
					} else {
						ZEND_ASSERT(slow_inputs_count);
						zend_jit_merge_N(jit, slow_inputs_count, slow_inputs);
					}
				} else {
					ZEND_HASH_FOREACH_VAL(jumptable, zv) {
						target = ZEND_OFFSET_TO_OPLINE(opline, Z_LVAL_P(zv));
						b = ssa->cfg.map[target - op_array->opcodes];
						_zend_jit_add_predecessor_ref(jit, b, jit->b, ref);
					} ZEND_HASH_FOREACH_END();

					_zend_jit_add_predecessor_ref(jit, default_b, jit->b, ref);
					if (slow_inputs_count) {
						zend_jit_merge_N(jit, slow_inputs_count, slow_inputs);
						_zend_jit_add_predecessor_ref(jit, jit->b + 1, jit->b, zend_jit_end(jit));
					}
					jit->control = IR_UNUSED;
					jit->b = -1;
				}
			} else {
				ZEND_ASSERT(!next_opline);
				_zend_jit_add_predecessor_ref(jit, jit->b + 1, jit->b, zend_jit_end(jit));
				jit->control = IR_UNUSED;
				jit->b = -1;
			}
		} else if (opline->opcode == ZEND_SWITCH_STRING) {
			if (op1_info & MAY_BE_STRING) {
				if (op1_info & MAY_BE_REF) {
					ir_ref ref, if_string, fast_path, ref2;

					ref = zend_jit_zval_addr(jit, op1_addr);
					if_string = zend_jit_if_zval_type(jit, op1_addr, IS_STRING);
					zend_jit_if_true(jit, if_string);
					fast_path = zend_jit_end(jit);
					zend_jit_if_false_cold(jit, if_string);

					// JIT: ZVAL_DEREF(op)
					if (fallback_label) {
						zend_jit_guard_zval_type(jit, op1_addr, IS_REFERENCE, fallback_label);
					} else {
						ir_ref if_ref = zend_jit_if_zval_type(jit, op1_addr, IS_STRING);
						zend_jit_if_false_cold(jit, if_ref);
						zend_jit_array_add(slow_inputs, zend_jit_end(jit));
						zend_jit_if_true(jit, if_ref);
					}

					ref2 = zend_jit_addr_offset(jit,
						zend_jit_zval_ptr(jit, op1_addr), offsetof(zend_reference, val));
					op1_addr = ZEND_ADDR_REF_ZVAL(ref2);

					if (fallback_label) {
						zend_jit_guard_zval_type(jit, op1_addr, IS_LONG, fallback_label);
					} else {
						if_string = zend_jit_if_zval_type(jit, op1_addr, IS_STRING);
						zend_jit_if_false_cold(jit, if_string);
						zend_jit_array_add(slow_inputs, zend_jit_end(jit));
						zend_jit_if_true(jit, if_string);
					}

					zend_jit_merge_2(jit, fast_path, zend_jit_end(jit));
					ref = zend_jit_phi_2(jit, ref, ref2);
					op1_addr = ZEND_ADDR_REF_ZVAL(ref);
				} else if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_STRING)) {
					if (fallback_label) {
						zend_jit_guard_zval_type(jit, op1_addr, IS_STRING, fallback_label);
					} else {
						ir_ref if_string = zend_jit_if_zval_type(jit, op1_addr, IS_STRING);
						zend_jit_if_false_cold(jit, if_string);
						zend_jit_array_add(slow_inputs, zend_jit_end(jit));
						zend_jit_if_true(jit, if_string);
					}
				}

				ir_ref ref = zend_jit_zval_ptr(jit, op1_addr);
				ref = zend_jit_call_2(jit, IR_PHP_LONG,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_find, IR_CONST_FASTCALL_FUNC),
					zend_jit_const_addr(jit, (uintptr_t)jumptable),
					ref);
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_PHP_LONG),
					ref,
					ir_const_php_long(&jit->ctx, (uintptr_t)jumptable->arData));
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_DIV, IR_PHP_LONG),
					ref,
					ir_const_php_long(&jit->ctx, sizeof(Bucket)));
				ref = ir_emit2(&jit->ctx, IR_SWITCH, jit->control, ref);

				if (next_opline) {
					ir_ref continue_list = IR_UNUSED;

					ZEND_HASH_FOREACH_VAL(jumptable, zv) {
						ir_ref idx;
						target = ZEND_OFFSET_TO_OPLINE(opline, Z_LVAL_P(zv));

						if (HT_IS_PACKED(jumptable)) {
							idx = ir_const_php_long(&jit->ctx, zv - jumptable->arPacked);
						} else {
							idx = ir_const_php_long(&jit->ctx, (Bucket*)zv - jumptable->arData);
						}
						jit->control = ir_emit2(&jit->ctx, IR_CASE_VAL, ref, idx);
						if (target == next_opline) {
							continue_list = zend_jit_list_add(jit, continue_list, zend_jit_end(jit));
						} else {
							exit_point = zend_jit_trace_get_exit_point(target, 0);
							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								return 0;
							}
							zend_jit_side_exit(jit, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
						}
					} ZEND_HASH_FOREACH_END();

					jit->control = ir_emit1(&jit->ctx, IR_CASE_DEFAULT, ref);
					if (next_opline == default_opline) {
						continue_list = zend_jit_list_add(jit, continue_list, zend_jit_end(jit));
					} else {
						zend_jit_side_exit(jit, zend_jit_const_addr(jit, (uintptr_t)default_label));
					}
					if (continue_list) {
						zend_jit_merge_list(jit, continue_list);
					} else {
						ZEND_ASSERT(slow_inputs_count);
						zend_jit_merge_N(jit, slow_inputs_count, slow_inputs);
					}
				} else {
					ZEND_HASH_FOREACH_VAL(jumptable, zv) {
						target = ZEND_OFFSET_TO_OPLINE(opline, Z_LVAL_P(zv));
						b = ssa->cfg.map[target - op_array->opcodes];
						_zend_jit_add_predecessor_ref(jit, b, jit->b, ref);
					} ZEND_HASH_FOREACH_END();
					_zend_jit_add_predecessor_ref(jit, default_b, jit->b, ref);
					if (slow_inputs_count) {
						zend_jit_merge_N(jit, slow_inputs_count, slow_inputs);
						_zend_jit_add_predecessor_ref(jit, jit->b + 1, jit->b, zend_jit_end(jit));
					}
					jit->control = IR_UNUSED;
					jit->b = -1;
				}
			} else {
				ZEND_ASSERT(!next_opline);
				_zend_jit_add_predecessor_ref(jit, jit->b + 1, jit->b, zend_jit_end(jit));
				jit->control = IR_UNUSED;
				jit->b = -1;
			}
		} else if (opline->opcode == ZEND_MATCH) {
			ir_ref if_type = IR_UNUSED, default_input_list = IR_UNUSED, ref = IR_UNUSED;
			ir_ref continue_list = IR_UNUSED;

			if (op1_info & (MAY_BE_LONG|MAY_BE_STRING)) {
				ir_ref long_path = IR_UNUSED;

				if (op1_info & MAY_BE_REF) {
					op1_addr = zend_jit_zval_deref(jit, op1_addr);
				}
				if (op1_info & MAY_BE_LONG) {
					if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG)) {
						if (op1_info & (MAY_BE_STRING|MAY_BE_UNDEF)) {
							if_type = zend_jit_if_zval_type(jit, op1_addr, IS_LONG);
							zend_jit_if_true(jit, if_type);
						} else if (default_label) {
							zend_jit_guard_zval_type(jit, op1_addr, IS_LONG, default_label);
						} else if (next_opline) {
							ir_ref if_type = zend_jit_if_zval_type(jit, op1_addr, IS_LONG);
							zend_jit_if_false(jit, if_type);
							continue_list = zend_jit_list_add(jit, continue_list, zend_jit_end(jit));
							zend_jit_if_true(jit, if_type);
						} else {
							ir_ref if_type = zend_jit_if_zval_type(jit, op1_addr, IS_LONG);
							zend_jit_if_false(jit, if_type);
							default_input_list = zend_jit_list_add(jit, default_input_list, zend_jit_end(jit));
							zend_jit_if_true(jit, if_type);
						}
					}
					ref = zend_jit_zval_lval(jit, op1_addr);
					ref = zend_jit_call_2(jit, IR_PHP_LONG,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_index_find, IR_CONST_FASTCALL_FUNC),
						zend_jit_const_addr(jit, (uintptr_t)jumptable),
						ref);
					if (op1_info & MAY_BE_STRING) {
						long_path = zend_jit_end(jit);
					}
				}
				if (op1_info & MAY_BE_STRING) {
					if (if_type) {
						zend_jit_if_false(jit, if_type);
						if_type = IS_UNUSED;
					}
					if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_STRING))) {
						if (op1_info & MAY_BE_UNDEF) {
							if_type = zend_jit_if_zval_type(jit, op1_addr, IS_STRING);
							zend_jit_if_true(jit, if_type);
						} else if (default_label) {
							zend_jit_guard_zval_type(jit, op1_addr, IS_STRING, default_label);
						} else if (next_opline) {
							ir_ref if_type = zend_jit_if_zval_type(jit, op1_addr, IS_STRING);
							zend_jit_if_false(jit, if_type);
							continue_list = zend_jit_list_add(jit, continue_list, zend_jit_end(jit));
							zend_jit_if_true(jit, if_type);
						} else {
							ir_ref if_type = zend_jit_if_zval_type(jit, op1_addr, IS_STRING);
							zend_jit_if_false(jit, if_type);
							default_input_list = zend_jit_list_add(jit, default_input_list, zend_jit_end(jit));
							zend_jit_if_true(jit, if_type);
						}
					}
					ir_ref ref2 = zend_jit_zval_ptr(jit, op1_addr);
					ref2 = zend_jit_call_2(jit, IR_PHP_LONG,
						zend_jit_const_func_addr(jit, (uintptr_t)zend_hash_find, IR_CONST_FASTCALL_FUNC),
						zend_jit_const_addr(jit, (uintptr_t)jumptable),
						ref2);
					if (op1_info & MAY_BE_LONG) {
						zend_jit_merge_2(jit, long_path, zend_jit_end(jit));
						ref = zend_jit_phi_2(jit, ref, ref2);
					} else {
						ref = ref2;
					}
				}

				ref = ir_fold2(&jit->ctx, IR_OPT(IR_SUB, IR_PHP_LONG),
					ref,
					ir_const_php_long(&jit->ctx, (uintptr_t)jumptable->arData));
				ref = ir_fold2(&jit->ctx, IR_OPT(IR_DIV, IR_PHP_LONG),
					ref,
					ir_const_php_long(&jit->ctx, HT_IS_PACKED(jumptable) ? sizeof(zval) : sizeof(Bucket)));
				ref = ir_emit2(&jit->ctx, IR_SWITCH, jit->control, ref);

				if (next_opline) {
					ZEND_HASH_FOREACH_VAL(jumptable, zv) {
						ir_ref idx;
						target = ZEND_OFFSET_TO_OPLINE(opline, Z_LVAL_P(zv));

						if (HT_IS_PACKED(jumptable)) {
							idx = ir_const_php_long(&jit->ctx, zv - jumptable->arPacked);
						} else {
							idx = ir_const_php_long(&jit->ctx, (Bucket*)zv - jumptable->arData);
						}
						jit->control = ir_emit2(&jit->ctx, IR_CASE_VAL, ref, idx);
						if (target == next_opline) {
							continue_list = zend_jit_list_add(jit, continue_list, zend_jit_end(jit));
						} else {
							exit_point = zend_jit_trace_get_exit_point(target, 0);
							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								return 0;
							}
							zend_jit_side_exit(jit, zend_jit_const_addr(jit, (uintptr_t)exit_addr));
						}
					} ZEND_HASH_FOREACH_END();

					jit->control = ir_emit1(&jit->ctx, IR_CASE_DEFAULT, ref);
					if (next_opline == default_opline) {
						continue_list = zend_jit_list_add(jit, continue_list, zend_jit_end(jit));
					} else {
						zend_jit_side_exit(jit, zend_jit_const_addr(jit, (uintptr_t)default_label));
					}
				} else {
					ZEND_HASH_FOREACH_VAL(jumptable, zv) {
						target = ZEND_OFFSET_TO_OPLINE(opline, Z_LVAL_P(zv));
						b = ssa->cfg.map[target - op_array->opcodes];
						_zend_jit_add_predecessor_ref(jit, b, jit->b, ref);
					} ZEND_HASH_FOREACH_END();
					_zend_jit_add_predecessor_ref(jit, default_b, jit->b, ref);
				}
			}

			if (op1_info & MAY_BE_UNDEF) {
				if (if_type) {
					zend_jit_if_false(jit, if_type);
					if_type = IS_UNUSED;
				}
				if (op1_info & (MAY_BE_ANY-(MAY_BE_LONG|MAY_BE_STRING))) {
					if (default_label) {
						zend_jit_guard_zval_type(jit, op1_addr, IS_UNDEF, default_label);
					} else if (next_opline) {
						ir_ref if_def = zend_jit_if(jit, zend_jit_zval_type(jit, op1_addr));
						zend_jit_if_true(jit, if_def);
						continue_list = zend_jit_list_add(jit, continue_list, zend_jit_end(jit));
						zend_jit_if_false_cold(jit, if_def);
					} else {
						ir_ref if_def = zend_jit_if(jit, zend_jit_zval_type(jit, op1_addr));
						zend_jit_if_true(jit, if_def);
						default_input_list = zend_jit_list_add(jit, default_input_list, zend_jit_end(jit));
						zend_jit_if_false_cold(jit, if_def);
					}
				}

				zend_jit_set_ex_opline(jit, opline);
				zend_jit_call_1(jit, IR_VOID,
					zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_undefined_op_helper, IR_CONST_FASTCALL_FUNC),
					ir_const_u32(&jit->ctx, opline->op1.var));
				zend_jit_check_exception_undef_result(jit, opline);
				if (default_label) {
					zend_jit_side_exit(jit, zend_jit_const_addr(jit, (uintptr_t)default_label));
				} else if (next_opline) {
					continue_list = zend_jit_list_add(jit, continue_list, zend_jit_end(jit));
				} else {
					default_input_list = zend_jit_list_add(jit, default_input_list, zend_jit_end(jit));
				}
			}
			if (next_opline) {
				ZEND_ASSERT(continue_list);
				zend_jit_merge_list(jit, continue_list);
			} else {
				if (default_input_list) {
					ZEND_ASSERT(jit->ctx.ir_base[ref].op == IR_SWITCH);
					ZEND_ASSERT(jit->ctx.ir_base[ref].op3 == IR_UNUSED);
					jit->ctx.ir_base[ref].op3 = default_input_list;
				}
				jit->control = IR_UNUSED;
				jit->b = -1;
			}
		} else {
			ZEND_UNREACHABLE();
		}
	}
	return 1;
}

static int zend_jit_start(zend_jit_ctx *jit, const zend_op_array *op_array, zend_ssa *ssa)
{
	int i, count;
	zend_basic_block *bb;

	zend_jit_init_ctx(jit, (zend_jit_vm_kind == ZEND_VM_KIND_CALL) ? 0 : (IR_START_BR_TARGET|IR_ENTRY_BR_TARGET));

	jit->ctx.spill_base = ZREG_FP;

	jit->op_array = op_array;
	jit->ssa = ssa;
	jit->bb_start_ref = zend_arena_calloc(&CG(arena), ssa->cfg.blocks_count * 2, sizeof(ir_ref));
	jit->bb_predecessors = jit->bb_start_ref + ssa->cfg.blocks_count;

	count = 0;
	for (i = 0, bb = ssa->cfg.blocks; i < ssa->cfg.blocks_count; i++, bb++) {
		jit->bb_predecessors[i] = count;
		count += bb->predecessors_count;
	}
	jit->bb_edges = zend_arena_calloc(&CG(arena), count, sizeof(ir_ref));

	return 1;
}

static void *zend_jit_finish(zend_jit_ctx *jit)
{
	void *entry;
	size_t size;
	zend_string *str = NULL;

	if (JIT_G(debug) & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF|ZEND_JIT_DEBUG_PERF_DUMP|
			ZEND_JIT_DEBUG_IR_SRC|ZEND_JIT_DEBUG_IR_AFTER_SCCP|ZEND_JIT_DEBUG_IR_AFTER_SCCP|
			ZEND_JIT_DEBUG_IR_AFTER_SCHEDULE|ZEND_JIT_DEBUG_IR_AFTER_REGS|ZEND_JIT_DEBUG_IR_FINAL)) {
		if (jit->name) {
			str = zend_string_copy(jit->name);
		} else {
			str = zend_jit_func_name(jit->op_array);
		}
	}

	if (jit->op_array) {
		/* Only for function JIT */
		_zend_jit_fix_merges(jit);
	}

	entry = zend_jit_ir_compile(&jit->ctx, &size, str ? ZSTR_VAL(str) : NULL);
	if (entry) {
		if (JIT_G(debug) & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF|ZEND_JIT_DEBUG_PERF_DUMP)) {
			if (JIT_G(debug) & ZEND_JIT_DEBUG_ASM) {
				if (str) {
					ir_disasm_add_symbol(ZSTR_VAL(str), (uintptr_t)entry, size);
				}
				ir_disasm(str ? ZSTR_VAL(str) : "unknown",
					entry, size,
					(JIT_G(debug) & ZEND_JIT_DEBUG_ASM_ADDR) != 0,
					&jit->ctx, stderr);
			}

			if (str) {
				if (JIT_G(debug) & ZEND_JIT_DEBUG_GDB) {
//					ir_mem_unprotect(entry, size);
					ir_gdb_register(ZSTR_VAL(str), entry, size, 0, 0);
//					ir_mem_protect(entry, size);
				}

				if (JIT_G(debug) & (ZEND_JIT_DEBUG_PERF|ZEND_JIT_DEBUG_PERF_DUMP)) {
					ir_perf_map_register(ZSTR_VAL(str), entry, size);
					if (JIT_G(debug) & ZEND_JIT_DEBUG_PERF_DUMP) {
						ir_perf_jitdump_register(ZSTR_VAL(str), entry, size);
					}
				}
			}
		}

		if (jit->op_array) {
			/* Only for function JIT */
			zend_op *opline = (zend_op*)jit->op_array->opcodes;

			while (opline->opcode == ZEND_RECV) {
				opline++;
			}
			opline->handler = entry;

			ir_ref ref = jit->ctx.ir_base[1].op2;
			while (ref) {
				jit->op_array->opcodes[jit->ctx.ir_base[ref].op1].handler = (char*)entry + jit->ctx.ir_base[ref].op3;
				ref = jit->ctx.ir_base[ref].op2;
			}
		} else {
			/* Only for tracing JIT */
			zend_jit_trace_add_code(entry, size);
		}
	}

	if (str) {
		zend_string_release(str);
	}

	return entry;
}

static const void *zend_jit_trace_allocate_exit_group(uint32_t n)
{
	const void *entry;
	size_t size;

	entry = ir_emit_exitgroup(n, ZEND_JIT_EXIT_POINTS_PER_GROUP, zend_jit_stub_handlers[jit_stub_trace_exit],
		*dasm_ptr, (char*)dasm_end - (char*)*dasm_ptr, &size);

	if (entry) {
		*dasm_ptr = (char*)entry + ZEND_MM_ALIGNED_SIZE_EX(size, 16);
		if (JIT_G(debug) & ZEND_JIT_DEBUG_ASM) {
			uint32_t i;
			char name[32];

			for (i = 0; i < ZEND_JIT_EXIT_POINTS_PER_GROUP; i++) {
				sprintf(name, "jit$$trace_exit_%d", n + i);
				ir_disasm_add_symbol(name, (uintptr_t)entry + (i * ZEND_JIT_EXIT_POINTS_SPACING), ZEND_JIT_EXIT_POINTS_SPACING);
			}
		}
	}

	return entry;
}

static int zend_jit_type_guard(zend_jit_ctx *jit, const zend_op *opline, uint32_t var, uint8_t type)
{
	int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);
	const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);
	zend_jit_addr addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, var);
	ir_ref ref;

	if (!exit_addr) {
		return 0;
	}
	ref = zend_jit_zval_type(jit, addr);
	zend_jit_guard(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL), ref, ir_const_u8(&jit->ctx, type)),
		zend_jit_const_addr(jit, (uintptr_t)exit_addr));

	return 1;
}

static int zend_jit_scalar_type_guard(zend_jit_ctx *jit, const zend_op *opline, uint32_t var)
{
	int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);
	const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);
	zend_jit_addr addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, var);
	ir_ref ref;

	if (!exit_addr) {
		return 0;
	}
	ref = zend_jit_zval_type(jit, addr);
	zend_jit_guard(jit,
		ir_fold2(&jit->ctx, IR_OPT(IR_LT, IR_BOOL), ref, ir_const_u8(&jit->ctx, IS_STRING)),
		zend_jit_const_addr(jit, (uintptr_t)exit_addr));

	return 1;
}

static bool zend_jit_noref_guard(zend_jit_ctx *jit, const zend_op *opline, zend_jit_addr var_addr)
{
	ZEND_ASSERT(0 && "NIY");
#if 0
	int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);
	const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

	if (!exit_addr) {
		return 0;
	}
	|	IF_ZVAL_TYPE var_addr, IS_REFERENCE, &exit_addr
#endif

	return 1;
}

static int zend_jit_trace_opline_guard(zend_jit_ctx *jit, const zend_op *opline)
{
	uint32_t exit_point = zend_jit_trace_get_exit_point(NULL, 0);
	const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);
	ir_ref ref;

	if (!exit_addr) {
		return 0;
	}

    ref = zend_jit_cmp_ip(jit, IR_EQ, opline);
	zend_jit_guard(jit, ref,
		zend_jit_const_addr(jit, (uintptr_t)exit_addr));

	zend_jit_set_last_valid_opline(jit, opline);

	return 1;
}

static bool zend_jit_fetch_reference(zend_jit_ctx  *jit,
                                     const zend_op *opline,
                                     uint8_t        var_type,
                                     uint32_t      *var_info_ptr,
                                     zend_jit_addr *var_addr_ptr,
                                     zend_jit_addr *ref_addr_ptr,
                                     bool           add_ref_guard,
                                     bool           add_type_guard)
{
	zend_jit_addr var_addr = *var_addr_ptr;
	uint32_t var_info = *var_info_ptr;
	const void *exit_addr = NULL;
	ir_ref ref;

	if (add_ref_guard || add_type_guard) {
		int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);

		exit_addr = zend_jit_trace_get_exit_addr(exit_point);
		if (!exit_addr) {
			return 0;
		}
	}

	if (add_ref_guard) {
		ref = zend_jit_zval_type(jit, var_addr);
		zend_jit_guard(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL), ref, ir_const_u8(&jit->ctx, IS_REFERENCE)),
			zend_jit_const_addr(jit, (uintptr_t)exit_addr));
	}
	if (opline->opcode == ZEND_INIT_METHOD_CALL && opline->op1_type == IS_VAR) {
		/* Hack: Convert reference to regular value to simplify JIT code for INIT_METHOD_CALL */
		zend_jit_call_1(jit, IR_VOID,
			zend_jit_const_func_addr(jit, (uintptr_t)zend_jit_unref_helper, IR_CONST_FASTCALL_FUNC),
			zend_jit_zval_addr(jit, var_addr));
		*var_addr_ptr = var_addr;
	} else {
		ref = zend_jit_zval_ptr(jit, var_addr);
		if (ref_addr_ptr) {
			*ref_addr_ptr = ZEND_ADDR_REF_ZVAL(ref);
		}
		ref = zend_jit_addr_offset(jit, ref, offsetof(zend_reference, val));
		var_addr = ZEND_ADDR_REF_ZVAL(ref);
		*var_addr_ptr = var_addr;
	}

	if (var_type != IS_UNKNOWN) {
		var_type &= ~(IS_TRACE_REFERENCE|IS_TRACE_INDIRECT|IS_TRACE_PACKED);
	}
	if (add_type_guard
	 && var_type != IS_UNKNOWN
	 && (var_info & (MAY_BE_ANY|MAY_BE_UNDEF)) != (1 << var_type)) {
		ref = zend_jit_zval_type(jit, var_addr);
		zend_jit_guard(jit,
			ir_fold2(&jit->ctx, IR_OPT(IR_EQ, IR_BOOL), ref, ir_const_u8(&jit->ctx, var_type)),
			zend_jit_const_addr(jit, (uintptr_t)exit_addr));

		ZEND_ASSERT(var_info & (1 << var_type));
		if (var_type < IS_STRING) {
			var_info = (1 << var_type);
		} else if (var_type != IS_ARRAY) {
			var_info = (1 << var_type) | (var_info & (MAY_BE_RC1|MAY_BE_RCN));
		} else {
			var_info = MAY_BE_ARRAY | (var_info & (MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_OF_REF|MAY_BE_ARRAY_KEY_ANY|MAY_BE_RC1|MAY_BE_RCN));
		}

		*var_info_ptr = var_info;
	} else {
		var_info &= ~MAY_BE_REF;
		*var_info_ptr = var_info;
	}
	*var_info_ptr |= MAY_BE_GUARD; /* prevent generation of specialized zval dtor */

	return 1;
}

static bool zend_jit_fetch_indirect_var(zend_jit_ctx *jit, const zend_op *opline, uint8_t var_type, uint32_t *var_info_ptr, zend_jit_addr *var_addr_ptr, bool add_indirect_guard)
{
	zend_jit_addr var_addr = *var_addr_ptr;
	uint32_t var_info = *var_info_ptr;
	int32_t exit_point;
	const void *exit_addr;
	ir_ref ref = IR_UNUSED;

	if (add_indirect_guard) {
		int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);
		const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

		if (!exit_addr) {
			return 0;
		}
		zend_jit_guard_zval_type(jit, var_addr, IS_INDIRECT, exit_addr);
		ref = zend_jit_zval_ptr(jit, var_addr);
	} else {
		// TODO: avoid reloading ???
		ref = zend_jit_zval_ptr(jit, var_addr);
	}
	*var_info_ptr &= ~MAY_BE_INDIRECT;
	var_addr = ZEND_ADDR_REF_ZVAL(ref);
	*var_addr_ptr = var_addr;

	if (var_type != IS_UNKNOWN) {
		var_type &= ~(IS_TRACE_INDIRECT|IS_TRACE_PACKED);
	}
	if (!(var_type & IS_TRACE_REFERENCE)
	 && var_type != IS_UNKNOWN
	 && (var_info & (MAY_BE_ANY|MAY_BE_UNDEF)) != (1 << var_type)) {
		exit_point = zend_jit_trace_get_exit_point(opline, 0);
		exit_addr = zend_jit_trace_get_exit_addr(exit_point);

		if (!exit_addr) {
			return 0;
		}

		zend_jit_guard_zval_type(jit, var_addr, var_type, exit_addr);

		//var_info = zend_jit_trace_type_to_info_ex(var_type, var_info);
		ZEND_ASSERT(var_info & (1 << var_type));
		if (var_type < IS_STRING) {
			var_info = (1 << var_type);
		} else if (var_type != IS_ARRAY) {
			var_info = (1 << var_type) | (var_info & (MAY_BE_RC1|MAY_BE_RCN));
		} else {
			var_info = MAY_BE_ARRAY | (var_info & (MAY_BE_ARRAY_OF_ANY|MAY_BE_ARRAY_OF_REF|MAY_BE_ARRAY_KEY_ANY|MAY_BE_RC1|MAY_BE_RCN));
		}

		*var_info_ptr = var_info;
	}

	return 1;
}

static int zend_jit_trace_handler(zend_jit_ctx *jit, const zend_op_array *op_array, const zend_op *opline, int may_throw, zend_jit_trace_rec *trace)
{
	zend_jit_op_array_trace_extension *jit_extension =
		(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(op_array);
	size_t offset = jit_extension->offset;
	const void *handler =
		(zend_vm_opcode_handler_t)ZEND_OP_TRACE_INFO(opline, offset)->call_handler;
	ir_ref ref;

	zend_jit_set_ip(jit, opline);
	if (GCC_GLOBAL_REGS) {
		zend_jit_call_0(jit, IR_VOID, zend_jit_const_func_addr(jit, (uintptr_t)handler, 0));
	} else {
		ref = zend_jit_fp(jit);
		ref = zend_jit_call_1(jit, IR_I32,
			zend_jit_const_func_addr(jit, (uintptr_t)handler, IR_CONST_FASTCALL_FUNC), ref);
	}
	if (may_throw
	 && opline->opcode != ZEND_RETURN
	 && opline->opcode != ZEND_RETURN_BY_REF) {
		zend_jit_check_exception(jit);
	}

	while (trace->op != ZEND_JIT_TRACE_VM && trace->op != ZEND_JIT_TRACE_END) {
		trace++;
	}

	if (!GCC_GLOBAL_REGS
	 && (trace->op != ZEND_JIT_TRACE_END || trace->stop != ZEND_JIT_TRACE_STOP_RETURN)) {
		if (opline->opcode == ZEND_RETURN ||
		    opline->opcode == ZEND_RETURN_BY_REF ||
		    opline->opcode == ZEND_DO_UCALL ||
		    opline->opcode == ZEND_DO_FCALL_BY_NAME ||
		    opline->opcode == ZEND_DO_FCALL ||
		    opline->opcode == ZEND_GENERATOR_CREATE) {

			ir_ref addr = ZEND_JIT_EG_ADDR(current_execute_data);

			zend_jit_store_fp(jit, zend_jit_load(jit, IR_ADDR, addr));
		}
	}

	if (zend_jit_trace_may_exit(op_array, opline)) {
		if (opline->opcode == ZEND_RETURN ||
		    opline->opcode == ZEND_RETURN_BY_REF ||
		    opline->opcode == ZEND_GENERATOR_CREATE) {

			if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
				if (trace->op != ZEND_JIT_TRACE_END ||
				    (trace->stop != ZEND_JIT_TRACE_STOP_RETURN &&
				     trace->stop != ZEND_JIT_TRACE_STOP_INTERPRETER)) {
					/* this check may be handled by the following OPLINE guard or jmp [IP] */
					zend_jit_guard(jit,
						ir_fold2(&jit->ctx, IR_OPT(IR_NE, IR_BOOL),
							zend_jit_ip(jit),
							zend_jit_const_addr(jit, (uintptr_t)zend_jit_halt_op)),
						zend_jit_stub_addr(jit, jit_stub_trace_halt));
				}
			} else if (GCC_GLOBAL_REGS) {
				ZEND_ASSERT(0 && "NIY");
//???				|	test IP, IP
//???				|	je ->trace_halt
			} else {
				zend_jit_guard(jit,
					ir_fold2(&jit->ctx, IR_OPT(IR_GE, IR_BOOL),
						ref, ir_const_i32(&jit->ctx, 0)),
					zend_jit_stub_addr(jit, jit_stub_trace_halt));
			}
		} else if (opline->opcode == ZEND_EXIT ||
		           opline->opcode == ZEND_GENERATOR_RETURN ||
		           opline->opcode == ZEND_YIELD ||
		           opline->opcode == ZEND_YIELD_FROM) {
			zend_jit_ijmp(jit,
				zend_jit_stub_addr(jit, jit_stub_trace_halt));
			zend_jit_begin(jit, IR_UNUSED); /* unreachable block */
		}
		if (trace->op != ZEND_JIT_TRACE_END ||
		    (trace->stop != ZEND_JIT_TRACE_STOP_RETURN &&
		     trace->stop != ZEND_JIT_TRACE_STOP_INTERPRETER)) {

			const zend_op *next_opline = trace->opline;
			const zend_op *exit_opline = NULL;
			uint32_t exit_point;
			const void *exit_addr;
			uint32_t old_info = 0;
			uint32_t old_res_info = 0;
			zend_jit_trace_stack *stack = JIT_G(current_frame)->stack;
			ir_ref ref;

			if (zend_is_smart_branch(opline)) {
				bool exit_if_true = 0;
				exit_opline = zend_jit_trace_get_exit_opline(trace, opline + 1, &exit_if_true);
			} else {
				switch (opline->opcode) {
					case ZEND_JMPZ:
					case ZEND_JMPNZ:
					case ZEND_JMPZ_EX:
					case ZEND_JMPNZ_EX:
					case ZEND_JMP_SET:
					case ZEND_COALESCE:
					case ZEND_JMP_NULL:
					case ZEND_FE_RESET_R:
					case ZEND_FE_RESET_RW:
						exit_opline = (trace->opline == opline + 1) ?
							OP_JMP_ADDR(opline, opline->op2) :
							opline + 1;
						break;
					case ZEND_FE_FETCH_R:
					case ZEND_FE_FETCH_RW:
						exit_opline = (trace->opline == opline + 1) ?
							ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value) :
							opline + 1;
						break;

				}
			}

			switch (opline->opcode) {
				case ZEND_FE_FETCH_R:
				case ZEND_FE_FETCH_RW:
					if (opline->op2_type != IS_UNUSED) {
						old_info = STACK_INFO(stack, EX_VAR_TO_NUM(opline->op2.var));
						SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->op2.var), IS_UNKNOWN, 1);
					}
					break;
			}
			if (opline->result_type == IS_VAR || opline->result_type == IS_TMP_VAR) {
				old_res_info = STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var));
				SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_UNKNOWN, 1);
			}
			exit_point = zend_jit_trace_get_exit_point(exit_opline, 0);
			exit_addr = zend_jit_trace_get_exit_addr(exit_point);

			if (opline->result_type == IS_VAR || opline->result_type == IS_TMP_VAR) {
				SET_STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var), old_res_info);
			}
			switch (opline->opcode) {
				case ZEND_FE_FETCH_R:
				case ZEND_FE_FETCH_RW:
					if (opline->op2_type != IS_UNUSED) {
						SET_STACK_INFO(stack, EX_VAR_TO_NUM(opline->op2.var), old_info);
					}
					break;
			}

			if (!exit_addr) {
				return 0;
			}
			ref = zend_jit_cmp_ip(jit, IR_EQ, next_opline);
			zend_jit_guard(jit, ref,
				zend_jit_const_addr(jit, (uintptr_t)exit_addr));
		}
	}

	zend_jit_set_last_valid_opline(jit, trace->opline);

	return 1;
}

static int zend_jit_deoptimizer_start(zend_jit_ctx        *jit,
                                      zend_string         *name,
                                      uint32_t             trace_num,
                                      uint32_t             exit_num)
{
	zend_jit_init_ctx(jit, (zend_jit_vm_kind == ZEND_VM_KIND_CALL) ? IR_FUNCTION : 0);

	jit->ctx.spill_base = ZREG_FP;

	jit->op_array = NULL;//op_array;
	jit->ssa = NULL;//ssa;
	jit->name = zend_string_copy(name);

	jit->ctx.flags |= IR_SKIP_PROLOGUE;

	return 1;
}

static int zend_jit_trace_start(zend_jit_ctx        *jit,
                                const zend_op_array *op_array,
                                zend_ssa            *ssa,
                                zend_string         *name,
                                uint32_t             trace_num,
                                zend_jit_trace_info *parent,
                                uint32_t             exit_num)
{
	ir_ref ref;

	zend_jit_init_ctx(jit, (zend_jit_vm_kind == ZEND_VM_KIND_CALL) ? IR_FUNCTION : 0);

	jit->ctx.spill_base = ZREG_FP;

	jit->op_array = NULL;//op_array;
	jit->ssa = ssa;
	jit->name = zend_string_copy(name);

	if (!GCC_GLOBAL_REGS) {
		if (!parent) {
			ir_ref ref;
			ZEND_ASSERT(jit->control);
			ZEND_ASSERT(jit->ctx.ir_base[jit->control].op == IR_START);
			ref = zend_jit_param(jit, IR_ADDR, "execute_data", 1);
			zend_jit_store_fp(jit, ref);
			jit->ctx.flags |= IR_FASTCALL_FUNC;
		}
	}

	if (parent) {
		jit->ctx.flags |= IR_SKIP_PROLOGUE;
	} else {
		jit->ctx.flags |= IR_START_BR_TARGET;
	}

	if (parent) {
		int i;
		int parent_vars_count = parent->exit_info[exit_num].stack_size;
		zend_jit_trace_stack *parent_stack =
			parent->stack_map +
			parent->exit_info[exit_num].stack_offset;

		/* prevent clobbering of registers used for deoptimization */
		for (i = 0; i < parent_vars_count; i++) {
			if (STACK_FLAGS(parent_stack, i) != ZREG_CONST
			 && STACK_REG(parent_stack, i) != ZREG_NONE) {
				int32_t reg = STACK_REG(parent_stack, i);
				ir_type type;

				if (STACK_FLAGS(parent_stack, i) == ZREG_ZVAL_COPY) {
					type = IR_ADDR;
				} else if (STACK_TYPE(parent_stack, i) == IS_LONG) {
					type = IR_PHP_LONG;
				} else if (STACK_TYPE(parent_stack, i) == IS_DOUBLE) {
					type = IR_DOUBLE;
				} else {
					ZEND_ASSERT(0);
				}
				if (ssa && ssa->vars[i].no_val) {
					/* pass */
				} else {
					zend_jit_rload(jit, type, reg);
				}
//???			} else if (STACK_REG(parent_stack, i) == ZREG_ZVAL_COPY_GPR0) {
//???				ZEND_REGSET_EXCL(regset, ZREG_R0);
			}
		}
	}

	if (parent && parent->exit_info[exit_num].flags & ZEND_JIT_EXIT_METHOD_CALL) {
		ZEND_ASSERT(parent->exit_info[exit_num].poly_func_reg >= 0 && parent->exit_info[exit_num].poly_this_reg >= 0);
		zend_jit_rload(jit, IR_ADDR, parent->exit_info[exit_num].poly_func_reg);
		zend_jit_rload(jit, IR_ADDR, parent->exit_info[exit_num].poly_this_reg);
	}

	ref = ZEND_JIT_EG_ADDR(jit_trace_num);
	zend_jit_store(jit, ref, ir_const_u32(&jit->ctx, trace_num));

	return 1;
}

static int zend_jit_trace_begin_loop(zend_jit_ctx *jit)
{
	return zend_jit_loop_begin(jit, zend_jit_end(jit));
}

static void zend_jit_trace_gen_phi(zend_jit_ctx *jit, zend_ssa_phi *phi)
{
	int dst_var = phi->ssa_var;
	int src_var = phi->sources[0];
	ir_ref ref;

	ZEND_ASSERT(!(jit->ra[dst_var].flags & ZREG_LOAD));
	ZEND_ASSERT(jit->ra[src_var].ref != IR_UNUSED && jit->ra[src_var].ref != IR_NULL);

	ref = zend_jit_phi_2(jit,
		zend_jit_use_reg(jit, ZEND_ADDR_REG(src_var)), IR_UNUSED);

	src_var = phi->sources[1];
	ZEND_ASSERT(jit->ra[src_var].ref == IR_NULL);
	jit->ra[src_var].flags |= ZREG_FORWARD;

	zend_jit_def_reg(jit, ZEND_ADDR_REG(dst_var), ref);
}

static int zend_jit_trace_end_loop(zend_jit_ctx *jit, int loop_ref, const void *timeout_exit_addr)
{
	if (timeout_exit_addr) {
		zend_jit_check_timeout(jit, NULL, timeout_exit_addr);
	}
	ZEND_ASSERT(jit->ctx.ir_base[loop_ref].op2 == IR_UNUSED);
	ir_set_op(&jit->ctx, loop_ref, 2, zend_jit_loop_end(jit, loop_ref));
	return 1;
}

static int zend_jit_trace_return(zend_jit_ctx *jit, bool original_handler, const zend_op *opline)
{
	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID || GCC_GLOBAL_REGS) {
		if (!original_handler) {
			zend_jit_tailcall_0(jit,
				zend_jit_load(jit, IR_ADDR, zend_jit_ip(jit)));
		} else {
			zend_jit_tailcall_0(jit,
				zend_jit_orig_opline_handler(jit));
		}
	} else {
		if (original_handler) {
			ZEND_ASSERT(0 && "NIY");
#if 0
//???
			|	mov FCARG1a, FP
			|	mov r0, EX->func
			|	mov r0, aword [r0 + offsetof(zend_op_array, reserved[zend_func_info_rid])]
			|	mov r0, aword [r0 + offsetof(zend_jit_op_array_trace_extension, offset)]
			|	call aword [IP + r0]
			if (opline &&
			    (opline->opcode == ZEND_RETURN
			  || opline->opcode == ZEND_RETURN_BY_REF
			  || opline->opcode == ZEND_GENERATOR_RETURN
			  || opline->opcode == ZEND_GENERATOR_CREATE
			  || opline->opcode == ZEND_YIELD
			  || opline->opcode == ZEND_YIELD_FROM)) {
				zend_jit_ret(jit, ref);
			}
#endif
		}
		zend_jit_ret(jit, ir_const_i32(&jit->ctx, 2)); // ZEND_VM_LEAVE
	}
	return 1;
}

static int zend_jit_link_side_trace(const void *code, size_t size, uint32_t jmp_table_size, uint32_t exit_num, const void *addr)
{
	return ir_patch(code, size, jmp_table_size, zend_jit_trace_get_exit_addr(exit_num), addr);
}

static int zend_jit_trace_link_to_root(zend_jit_ctx *jit, zend_jit_trace_info *t, const void *timeout_exit_addr)
{
	const void *link_addr;

	/* Skip prologue. */
	ZEND_ASSERT(zend_jit_trace_prologue_size != (size_t)-1);
	link_addr = (const void*)((const char*)t->code_start + zend_jit_trace_prologue_size);

	if (timeout_exit_addr) {
		zend_jit_check_timeout(jit, NULL, timeout_exit_addr);
	}
	zend_jit_ijmp(jit, zend_jit_const_addr(jit, (uintptr_t)link_addr));

	return 1;
}

static bool zend_jit_opline_supports_reg(const zend_op_array *op_array, zend_ssa *ssa, const zend_op *opline, const zend_ssa_op *ssa_op, zend_jit_trace_rec *trace)
{
	uint32_t op1_info, op2_info;

	switch (opline->opcode) {
		case ZEND_SEND_VAR:
		case ZEND_SEND_VAL:
		case ZEND_SEND_VAL_EX:
			return (opline->op2_type != IS_CONST) && (opline->opcode != ZEND_SEND_VAL_EX || opline->op2.num <= MAX_ARG_FLAG_NUM);
		case ZEND_QM_ASSIGN:
		case ZEND_IS_SMALLER:
		case ZEND_IS_SMALLER_OR_EQUAL:
		case ZEND_IS_EQUAL:
		case ZEND_IS_NOT_EQUAL:
		case ZEND_IS_IDENTICAL:
		case ZEND_IS_NOT_IDENTICAL:
		case ZEND_CASE:
			return 1;
		case ZEND_RETURN:
			return (op_array->type != ZEND_EVAL_CODE && op_array->function_name);
		case ZEND_ASSIGN:
			return (opline->op1_type == IS_CV);
		case ZEND_ADD:
		case ZEND_SUB:
		case ZEND_MUL:
			op1_info = OP1_INFO();
			op2_info = OP2_INFO();
			return !(op1_info & MAY_BE_UNDEF)
				&& !(op2_info & MAY_BE_UNDEF)
				&& (op1_info & (MAY_BE_LONG|MAY_BE_DOUBLE))
				&& (op2_info & (MAY_BE_LONG|MAY_BE_DOUBLE));
		case ZEND_BW_OR:
		case ZEND_BW_AND:
		case ZEND_BW_XOR:
		case ZEND_SL:
		case ZEND_SR:
		case ZEND_MOD:
			op1_info = OP1_INFO();
			op2_info = OP2_INFO();
			return (op1_info & MAY_BE_LONG)
				&& (op2_info & MAY_BE_LONG);
		case ZEND_PRE_INC:
		case ZEND_PRE_DEC:
		case ZEND_POST_INC:
		case ZEND_POST_DEC:
			op1_info = OP1_INFO();
			return opline->op1_type == IS_CV
				&& (op1_info & MAY_BE_LONG)
				&& !(op1_info & MAY_BE_REF);
		case ZEND_STRLEN:
			op1_info = OP1_INFO();
			return (opline->op1_type & (IS_CV|IS_CONST))
				&& (op1_info & (MAY_BE_ANY|MAY_BE_REF|MAY_BE_UNDEF)) == MAY_BE_STRING;
		case ZEND_COUNT:
			op1_info = OP1_INFO();
			return (opline->op1_type & (IS_CV|IS_CONST))
				&& (op1_info & (MAY_BE_ANY|MAY_BE_REF|MAY_BE_UNDEF)) == MAY_BE_ARRAY;
		case ZEND_JMPZ:
		case ZEND_JMPNZ:
			if (JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE) {
				if (!ssa->cfg.map) {
					return 0;
				}
				if (opline > op_array->opcodes + ssa->cfg.blocks[ssa->cfg.map[opline-op_array->opcodes]].start &&
				    ((opline-1)->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
					return 0;
				}
			}
			ZEND_FALLTHROUGH;
		case ZEND_BOOL:
		case ZEND_BOOL_NOT:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
			return 1;
		case ZEND_FETCH_CONSTANT:
			return 1;
		case ZEND_FETCH_DIM_R:
			op1_info = OP1_INFO();
			op2_info = OP2_INFO();
			if (trace
			 && trace->op1_type != IS_UNKNOWN
			 && (trace->op1_type & ~(IS_TRACE_REFERENCE|IS_TRACE_INDIRECT|IS_TRACE_PACKED)) == IS_ARRAY) {
				op1_info &= ~((MAY_BE_ANY|MAY_BE_UNDEF) - MAY_BE_ARRAY);
			}
			return ((op1_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_ARRAY) &&
				(!(opline->op1_type & (IS_TMP_VAR|IS_VAR)) || !(op1_info & MAY_BE_RC1)) &&
					(((op2_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_LONG) ||
					 (((op2_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_STRING) &&
						 (!(opline->op2_type & (IS_TMP_VAR|IS_VAR)) || !(op2_info & MAY_BE_RC1))));
	}
	return 0;
}

static bool zend_jit_var_supports_reg(zend_ssa *ssa, int var)
{
	if (ssa->vars[var].no_val) {
		/* we don't need the value */
		return 0;
	}

	if (!(JIT_G(opt_flags) & ZEND_JIT_REG_ALLOC_GLOBAL)) {
		/* Disable global register allocation,
		 * register allocation for SSA variables connected through Phi functions
		 */
		if (ssa->vars[var].definition_phi) {
			return 0;
		}
		if (ssa->vars[var].phi_use_chain) {
			zend_ssa_phi *phi = ssa->vars[var].phi_use_chain;
			do {
				if (!ssa->vars[phi->ssa_var].no_val) {
					return 0;
				}
				phi = zend_ssa_next_use_phi(ssa, var, phi);
			} while (phi);
		}
	}

	if (((ssa->var_info[var].type & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF)) != MAY_BE_DOUBLE) &&
	    ((ssa->var_info[var].type & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF)) != MAY_BE_LONG)) {
	    /* bad type */
		return 0;
	}

	return 1;
}

static bool zend_jit_may_be_in_reg(const zend_op_array *op_array, zend_ssa *ssa, int var)
{
	if (!zend_jit_var_supports_reg(ssa, var)) {
		return 0;
	}

	if (ssa->vars[var].definition >= 0) {
		uint32_t def = ssa->vars[var].definition;
		if (!zend_jit_opline_supports_reg(op_array, ssa, op_array->opcodes + def, ssa->ops + def, NULL)) {
			return 0;
		}
	}

	if (ssa->vars[var].use_chain >= 0) {
		int use = ssa->vars[var].use_chain;

		do {
			if (!zend_ssa_is_no_val_use(op_array->opcodes + use, ssa->ops + use, var) &&
			    !zend_jit_opline_supports_reg(op_array, ssa, op_array->opcodes + use, ssa->ops + use, NULL)) {
				return 0;
			}
			use = zend_ssa_next_use(ssa->ops, var, use);
		} while (use >= 0);
	}

	if (JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE) {
		int def_block, use_block, use, j;
		zend_basic_block *bb;
		zend_ssa_phi *p;

		/* Check if live range is split by ENTRY block */
		if (ssa->vars[var].definition >= 0) {
			def_block =ssa->cfg.map[ssa->vars[var].definition];
		} else {
			ZEND_ASSERT(ssa->vars[var].definition_phi);
			def_block = ssa->vars[var].definition_phi->block;
		}

		if (ssa->vars[var].use_chain >= 0) {
			use = ssa->vars[var].use_chain;
			do {
				use_block = ssa->cfg.map[use];
				while (use_block != def_block) {
					bb = &ssa->cfg.blocks[use_block];
					if (bb->flags & (ZEND_BB_ENTRY|ZEND_BB_RECV_ENTRY)) {
						return 0;
					}
					use_block = bb->idom;
					if (use_block < 0) {
						break;
					}
				}
				use = zend_ssa_next_use(ssa->ops, var, use);
			} while (use >= 0);
		}

		p = ssa->vars[var].phi_use_chain;
		while (p) {
			use_block = p->block;
			if (use_block != def_block) {
				bb = &ssa->cfg.blocks[use_block];
				for (j = 0; j < bb->predecessors_count; j++) {
					if (p->sources[j] == var) {
						use_block = ssa->cfg.predecessors[bb->predecessor_offset + j];
						while (use_block != def_block) {
							bb = &ssa->cfg.blocks[use_block];
							if (bb->flags & (ZEND_BB_ENTRY|ZEND_BB_RECV_ENTRY)) {
								return 0;
							}
							use_block = bb->idom;
							if (use_block < 0) {
								break;
							}
						}
						break;
					}
				}
			}
			p = zend_ssa_next_use_phi(ssa, var, p);
		}
	}

	return 1;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
