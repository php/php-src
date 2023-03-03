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
#include "jit/ir/ir_builder.h"

#if defined(IR_TARGET_X86)
# define IR_REG_SP            4 /* IR_REG_RSP */
# define ZREG_FP              6 /* IR_REG_RSI */
# define ZREG_IP              7 /* IR_REG_RDI */
# define ZREG_FIRST_FPR       8
# define IR_REGSET_PRESERVED ((1<<3) | (1<<5) | (1<<6) | (1<<7)) /* all preserved registers */
#elif defined(IR_TARGET_X64)
# define IR_REG_SP            4 /* IR_REG_RSP */
# define ZREG_FP             14 /* IR_REG_R14 */
# define ZREG_IP             15 /* IR_REG_R15 */
# define ZREG_FIRST_FPR      16
# if defined(_WIN64)
#  define IR_REGSET_PRESERVED ((1<<3) | (1<<5) | (1<<6) | (1<<7) | (1<<12) | (1<<13) | (1<<14) | (1<<15))
/*
#  define IR_REGSET_PRESERVED ((1<<3) | (1<<5) | (1<<6) | (1<<7) | (1<<12) | (1<<13) | (1<<14) | (1<<15) | \
                               (1<<(16+6)) | (1<<(16+7)) | (1<<(16+8)) | (1<<(16+9)) | (1<<(16+10)) | \
                               (1<<(16+11)) | (1<<(16+12)) | (1<<(16+13)) | (1<<(16+14)) | (1<<(16+15)))
*/
# else
#  define IR_REGSET_PRESERVED ((1<<3) | (1<<5) | (1<<12) | (1<<13) | (1<<14) | (1<<15)) /* all preserved registers */
# endif
#elif defined(IR_TARGET_AARCH64)
# define IR_REG_SP           31 /* IR_REG_RSP */
# define ZREG_FP             27 /* IR_REG_X27 */
# define ZREG_IP             28 /* IR_REG_X28 */
# define ZREG_FIRST_FPR      32
# define IR_REGSET_PRESERVED ((1<<19) | (1<<20) | (1<<21) | (1<<22) | (1<<23) | \
                              (1<<24) | (1<<25) | (1<<26) | (1<<27) | (1<<28)) /* all preserved registers */
#else
# error "Unknown IR target"
#endif

#define ZREG_RX ZREG_IP

#define OPTIMIZE_FOR_SIZE 0

/* IR builder defines */
#undef  _ir_CTX
#define _ir_CTX                 (&jit->ctx)

#undef  ir_CONST_ADDR
#define ir_CONST_ADDR(_addr)    jit_CONST_ADDR(jit, (uintptr_t)(_addr))
#define ir_CONST_FUNC(_addr)    jit_CONST_FUNC(jit, (uintptr_t)(_addr), 0)
#define ir_CONST_FC_FUNC(_addr) jit_CONST_FUNC(jit, (uintptr_t)(_addr), IR_CONST_FASTCALL_FUNC)
#define ir_CAST_FC_FUNC(_addr)  ir_fold2(_ir_CTX, IR_OPT(IR_BITCAST, IR_ADDR), (_addr), IR_CONST_FASTCALL_FUNC)

#undef  ir_ADD_OFFSET
#define ir_ADD_OFFSET(_addr, _offset) \
	jit_ADD_OFFSET(jit, _addr, _offset)

#ifdef ZEND_ENABLE_ZVAL_LONG64
# define IR_LONG           IR_I64
# define ir_CONST_LONG     ir_CONST_I64
# define ir_UNARY_OP_L     ir_UNARY_OP_I64
# define ir_BINARY_OP_L    ir_BINARY_OP_I64
# define ir_ADD_L          ir_ADD_I64
# define ir_SUB_L          ir_SUB_I64
# define ir_MUL_L          ir_MUL_I64
# define ir_DIV_L          ir_DIV_I64
# define ir_MOD_L          ir_MOD_I64
# define ir_NEG_L          ir_NEG_I64
# define ir_ABS_L          ir_ABS_I64
# define ir_SEXT_L         ir_SEXT_I64
# define ir_ZEXT_L         ir_ZEXT_I64
# define ir_TRUNC_L        ir_TRUNC_I64
# define ir_BITCAST_L      ir_BITCAST_I64
# define ir_FP2L           ir_FP2I64
# define ir_ADD_OV_L       ir_ADD_OV_I64
# define ir_SUB_OV_L       ir_SUB_OV_I64
# define ir_MUL_OV_L       ir_MUL_OV_I64
# define ir_NOT_L          ir_NOT_I64
# define ir_OR_L           ir_OR_I64
# define ir_AND_L          ir_AND_I64
# define ir_XOR_L          ir_XOR_I64
# define ir_SHL_L          ir_SHL_I64
# define ir_SHR_L          ir_SHR_I64
# define ir_SAR_L          ir_SAR_I64
# define ir_ROL_L          ir_ROL_I64
# define ir_ROR_L          ir_ROR_I64
# define ir_MIN_L          ir_MIN_I64
# define ir_MAX_L          ir_MAX_I64
# define ir_LOAD_L         ir_LOAD_I64
#else
# define IR_LONG           IR_I32
# define ir_CONST_LONG     ir_CONST_I32
# define ir_UNARY_OP_L     ir_UNARY_OP_I32
# define ir_BINARY_OP_L    ir_BINARY_OP_I32
# define ir_ADD_L          ir_ADD_I32
# define ir_SUB_L          ir_SUB_I32
# define ir_MUL_L          ir_MUL_I32
# define ir_DIV_L          ir_DIV_I32
# define ir_MOD_L          ir_MOD_I32
# define ir_NEG_L          ir_NEG_I32
# define ir_ABS_L          ir_ABS_I32
# define ir_SEXT_L         ir_SEXT_I32
# define ir_ZEXT_L         ir_ZEXT_I32
# define ir_TRUNC_L        ir_TRUNC_I32
# define ir_BITCAST_L      ir_BITCAST_I32
# define ir_FP2L           ir_FP2I32
# define ir_ADD_OV_L       ir_ADD_OV_I32
# define ir_SUB_OV_L       ir_SUB_OV_I32
# define ir_MUL_OV_L       ir_MUL_OV_I32
# define ir_NOT_L          ir_NOT_I32
# define ir_OR_L           ir_OR_I32
# define ir_AND_L          ir_AND_I32
# define ir_XOR_L          ir_XOR_I32
# define ir_SHL_L          ir_SHL_I32
# define ir_SHR_L          ir_SHR_I32
# define ir_SAR_L          ir_SAR_I32
# define ir_ROL_L          ir_ROL_I32
# define ir_ROR_L          ir_ROR_I32
# define ir_MIN_L          ir_MIN_I32
# define ir_MAX_L          ir_MAX_I32
# define ir_LOAD_L         ir_LOAD_I32
#endif

/* A helper structure to collect IT rers for the following use in (MERGE/PHI)_N */
typedef struct _ir_refs {
  uint32_t count;
  uint32_t limit;
  ir_ref   refs[0];
} ir_refs;

#define ir_refs_size(_n)          (offsetof(ir_refs, refs) + sizeof(ir_ref) * (_n))
#define ir_refs_init(_name, _n)   _name = alloca(ir_refs_size(_n)); \
                                  do {_name->count = 0; _name->limit = (_n);} while (0)

static void ir_refs_add(ir_refs *refs, ir_ref ref)
{
	ir_ref *ptr;

	ZEND_ASSERT(refs->count < refs->limit);
	ptr = refs->refs;
	ptr[refs->count++] = ref;
}

static size_t zend_jit_trace_prologue_size = (size_t)-1;
#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
static uint32_t allowed_opt_flags = 0;
#endif
static bool delayed_call_chain = 0; // TODO: remove this var (use jit->delayed_call_level) ???

#ifdef ZTS
static size_t tsrm_ls_cache_tcb_offset = 0;
static size_t tsrm_tls_index = 0;
static size_t tsrm_tls_offset = 0;

# define EG_TLS_OFFSET(field) \
	(executor_globals_offset + offsetof(zend_executor_globals, field))

# define CG_TLS_OFFSET(field) \
	(compiler_globals_offset + offsetof(zend_compiler_globals, field))

# define jit_EG(_field) \
	ir_ADD_OFFSET(jit_TLS(jit), EG_TLS_OFFSET(_field))

# define jit_CG(_field) \
	ir_ADD_OFFSET(jit_TLS(jit), CG_TLS_OFFSET(_field))

#else

# define jit_EG(_field) \
	ir_CONST_ADDR(&EG(_field))

# define jit_CG(_field) \
	ir_CONST_ADDR(&CG(_field))

#endif

#define jit_CALL(_call, _field) \
	ir_ADD_OFFSET(_call, offsetof(zend_execute_data, _field))

#define jit_EX(_field) \
	jit_CALL(jit_FP(jit), _field)

#define jit_RX(_field) \
	jit_CALL(jit_IP(jit), _field)

#define JIT_STUBS(_) \
	_(exception_handler,              IR_SKIP_PROLOGUE) \
	_(exception_handler_undef,        IR_SKIP_PROLOGUE) \
	_(exception_handler_free_op2,     IR_SKIP_PROLOGUE) \
	_(exception_handler_free_op1_op2, IR_SKIP_PROLOGUE) \
	_(interrupt_handler,              IR_SKIP_PROLOGUE) \
	_(leave_function_handler,         IR_SKIP_PROLOGUE) \
	_(negative_shift,                 IR_SKIP_PROLOGUE) \
	_(mod_by_zero,                    IR_SKIP_PROLOGUE) \
	_(invalid_this,                   IR_SKIP_PROLOGUE) \
	_(undefined_function,             IR_SKIP_PROLOGUE) \
	_(throw_cannot_pass_by_ref,       IR_SKIP_PROLOGUE) \
	_(icall_throw,                    IR_SKIP_PROLOGUE) \
	_(leave_throw,                    IR_SKIP_PROLOGUE) \
	_(hybrid_runtime_jit,             IR_SKIP_PROLOGUE | IR_START_BR_TARGET) \
	_(hybrid_profile_jit,             IR_SKIP_PROLOGUE | IR_START_BR_TARGET) \
	_(hybrid_func_hot_counter,        IR_SKIP_PROLOGUE | IR_START_BR_TARGET) \
	_(hybrid_loop_hot_counter,        IR_SKIP_PROLOGUE | IR_START_BR_TARGET) \
	_(hybrid_func_trace_counter,      IR_SKIP_PROLOGUE | IR_START_BR_TARGET) \
	_(hybrid_ret_trace_counter,       IR_SKIP_PROLOGUE | IR_START_BR_TARGET) \
	_(hybrid_loop_trace_counter,      IR_SKIP_PROLOGUE | IR_START_BR_TARGET) \
	_(trace_halt,                     IR_SKIP_PROLOGUE) \
	_(trace_escape,                   IR_SKIP_PROLOGUE) \
	_(trace_exit,                     IR_SKIP_PROLOGUE) \
	_(undefined_offset,               IR_FUNCTION | IR_FASTCALL_FUNC) \
	_(undefined_key,                  IR_FUNCTION | IR_FASTCALL_FUNC) \
	_(cannot_add_element,             IR_FUNCTION | IR_FASTCALL_FUNC) \
	_(assign_const,                   IR_FUNCTION | IR_FASTCALL_FUNC) \
	_(assign_tmp,                     IR_FUNCTION | IR_FASTCALL_FUNC) \
	_(assign_var,                     IR_FUNCTION | IR_FASTCALL_FUNC) \
	_(assign_cv_noref,                IR_FUNCTION | IR_FASTCALL_FUNC) \
	_(assign_cv,                      IR_FUNCTION | IR_FASTCALL_FUNC) \
	_(new_array,                      IR_FUNCTION | IR_FASTCALL_FUNC) \

#define JIT_STUB_ID(name, flags) \
	jit_stub_ ## name,

#define JIT_STUB_FORWARD(name, flags) \
	static int zend_jit_ ## name ## _stub(zend_jit_ctx *jit);

#define JIT_STUB(name, flags) \
	{JIT_STUB_PREFIX #name, zend_jit_ ## name ## _stub, flags},

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
	ir_refs             *delay_refs;
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
} zend_jit_stub;

JIT_STUBS(JIT_STUB_FORWARD)

static const zend_jit_stub zend_jit_stubs[] = {
	JIT_STUBS(JIT_STUB)
};

static void* zend_jit_stub_handlers[sizeof(zend_jit_stubs) / sizeof(zend_jit_stubs[0])];

static bool zend_jit_prefer_const_addr_load(zend_jit_ctx *jit, uintptr_t addr)
{
#if defined(IR_TARGET_X86)
	return 0; /* always use immediate value */
#elif defined(IR_TARGET_X64)
	return addr > 0xffffffff; /* prefer loading long constant from memery */
#elif defined(IR_TARGET_AARCH64)
	return addr > 0xffff;
#else
# error "Unknown IR target"
#endif
}

static const char* zend_reg_name(int8_t reg)
{
	return ir_reg_name(reg, ir_reg_is_int(reg) ? IR_LONG : IR_DOUBLE);
}

/* IR helpers */

#ifdef ZTS
static ir_ref jit_TLS(zend_jit_ctx *jit)
{
	ZEND_ASSERT(jit->ctx.control);
	if (jit->tls) {
		/* Emit "TLS" once for basic block */
		ir_insn *insn;
		ir_ref ref = jit->ctx.control;

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
	jit->tls = ir_TLS(
		tsrm_ls_cache_tcb_offset ? tsrm_ls_cache_tcb_offset : tsrm_tls_index,
		tsrm_ls_cache_tcb_offset ? 0 : tsrm_tls_offset);
	return jit->tls;
}
#endif

static ir_ref jit_CONST_ADDR(zend_jit_ctx *jit, uintptr_t addr)
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

static ir_ref jit_CONST_FUNC(zend_jit_ctx *jit, uintptr_t addr, uint16_t flags)
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

static ir_ref jit_ADD_OFFSET(zend_jit_ctx *jit, ir_ref addr, uintptr_t offset)
{
	if (offset) {
		addr = ir_ADD_A(addr, ir_CONST_ADDR(offset));
	}
	return addr;
}

static ir_ref jit_EG_exception(zend_jit_ctx *jit)
{
#ifdef ZTS
	return jit_EG(exception);
#else
	ir_ref ref = jit->eg_exception_addr;

	if (UNEXPECTED(!ref)) {
		ref = ir_unique_const_addr(&jit->ctx, (uintptr_t)&EG(exception));
		jit->eg_exception_addr = ref;
	}
	return ref;
#endif
}

static ir_ref jit_STUB_ADDR(zend_jit_ctx *jit, jit_stub_id id)
{
	ir_ref ref = jit->stub_addr[id];

	if (UNEXPECTED(!ref)) {
		ref = ir_unique_const_addr(&jit->ctx, (uintptr_t)zend_jit_stub_handlers[id]);
		jit->stub_addr[id] = ref;
	}
	return ref;
}

static ir_ref jit_STUB_FUNC_ADDR(zend_jit_ctx *jit, jit_stub_id id, uint16_t flags)
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

static void jit_SNAPSHOT(zend_jit_ctx *jit, ir_ref addr)
{
	if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && JIT_G(current_frame)) {
		const void *ptr = (const void*)jit->ctx.ir_base[addr].val.addr;
		const zend_op_array *op_array = &JIT_G(current_frame)->func->op_array;
		uint32_t stack_size = op_array->last_var + op_array->T;

		if (ptr == zend_jit_stub_handlers[jit_stub_exception_handler]
	     || ptr == zend_jit_stub_handlers[jit_stub_exception_handler_undef]
	     || ptr == zend_jit_stub_handlers[jit_stub_exception_handler_free_op1_op2]
	     || ptr == zend_jit_stub_handlers[jit_stub_exception_handler_free_op2]
	     || ptr == zend_jit_stub_handlers[jit_stub_interrupt_handler]
	     || ptr == zend_jit_stub_handlers[jit_stub_leave_function_handler]
	     || ptr == zend_jit_stub_handlers[jit_stub_negative_shift]
	     || ptr == zend_jit_stub_handlers[jit_stub_mod_by_zero]
	     || ptr == zend_jit_stub_handlers[jit_stub_invalid_this]
	     || ptr == zend_jit_stub_handlers[jit_stub_undefined_function]
	     || ptr == zend_jit_stub_handlers[jit_stub_throw_cannot_pass_by_ref]
	     || ptr == zend_jit_stub_handlers[jit_stub_icall_throw]
	     || ptr == zend_jit_stub_handlers[jit_stub_leave_throw]
	     || ptr == zend_jit_stub_handlers[jit_stub_trace_halt]
	     || ptr == zend_jit_stub_handlers[jit_stub_trace_escape]) {
			/* This is a GUARD that trigger exit through a stub code (without deoptimization) */
			return;
		}

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

			snapshot_size = stack_size;
			while (snapshot_size > 0 && !STACK_REF(stack, snapshot_size - 1)) {
				snapshot_size--;
			}
			if (snapshot_size || n) {
				ir_ref snapshot;

				snapshot = ir_SNAPSHOT(snapshot_size + n);
				for (i = 0; i < snapshot_size; i++) {
					ir_ref ref = STACK_REF(stack, i);

					ZEND_ASSERT(STACK_REF(stack, i) != IR_NULL);
					ir_SNAPSHOT_SET_OP(snapshot, i + 1, ref);
				}
				if (n) {
					ir_SNAPSHOT_SET_OP(snapshot, snapshot_size + 1, t->exit_info[exit_point].poly_func_ref);
					ir_SNAPSHOT_SET_OP(snapshot, snapshot_size + 2, t->exit_info[exit_point].poly_this_ref);
				}
			}
		}
	}
}

static int32_t _add_trace_const(zend_jit_trace_info *t, int64_t val)
{
	int32_t i;

	for (i = 0; i < t->consts_count; i++) {
		if (t->constants[i].i == val) {
			return i;
		}
	}
	ZEND_ASSERT(i < 0x7fffffff);
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

	exit_point = zend_jit_exit_point_by_addr(addr);
	ZEND_ASSERT(exit_point < t->exit_count);

	if (t->exit_info[exit_point].flags & ZEND_JIT_EXIT_METHOD_CALL) {
		int8_t *reg_ops = ctx->regs[snapshot_ref];

		ZEND_ASSERT(reg_ops[n - 1] != -1 && reg_ops[n] != -1);
		t->exit_info[exit_point].poly_func_reg = reg_ops[n - 1];
		t->exit_info[exit_point].poly_this_reg = reg_ops[n];
		n -= 2;
	}

	for (i = 2; i <= n; i++) {
		ir_ref ref = ir_insn_op(snapshot, i);

		if (ref) {
			int8_t *reg_ops = ctx->regs[snapshot_ref];
			int8_t reg = reg_ops[i];
			ir_ref var = i - 2;

			ZEND_ASSERT(var < t->exit_info[exit_point].stack_size);
			if (t->stack_map[t->exit_info[exit_point].stack_offset + var].flags == ZREG_ZVAL_COPY) {
				ZEND_ASSERT(reg != ZREG_NONE);
				t->stack_map[t->exit_info[exit_point].stack_offset + var].reg = IR_REG_NUM(reg);
			} else if (t->stack_map[t->exit_info[exit_point].stack_offset + var].flags != ZREG_CONST) {
				ZEND_ASSERT(t->stack_map[t->exit_info[exit_point].stack_offset + var].type == IS_LONG ||
					t->stack_map[t->exit_info[exit_point].stack_offset + var].type == IS_DOUBLE);

				if (ref > 0) {
					if (reg != ZREG_NONE) {
						t->stack_map[t->exit_info[exit_point].stack_offset + var].reg = IR_REG_NUM(reg);
						if (reg & IR_REG_SPILL_LOAD) {
							t->stack_map[t->exit_info[exit_point].stack_offset + var].flags |= ZREG_LOAD;
						}
					} else {
						t->stack_map[t->exit_info[exit_point].stack_offset + var].flags = ZREG_TYPE_ONLY;
					}
				} else {
					int32_t idx = _add_trace_const(t, ctx->ir_base[ref].val.i64);
					t->stack_map[t->exit_info[exit_point].stack_offset + var].flags = ZREG_CONST;
					t->stack_map[t->exit_info[exit_point].stack_offset + var].ref = idx;
				}
			}
		}
	}
	return addr;
}

static void jit_SIDE_EXIT(zend_jit_ctx *jit, ir_ref addr)
{
	jit_SNAPSHOT(jit, addr);
	ir_IJMP(addr);
}

/* PHP JIT helpers */

static ir_ref jit_EMALLOC(zend_jit_ctx *jit, size_t size, const zend_op_array *op_array, const zend_op *opline)
{
#if ZEND_DEBUG
	return ir_CALL_5(IR_ADDR, ir_CONST_FC_FUNC(_emalloc),
		ir_CONST_ADDR(size),
		op_array->filename ? ir_CONST_ADDR(op_array->filename->val) : IR_NULL,
		ir_CONST_U32(opline ? opline->lineno : 0),
		IR_NULL,
		ir_CONST_U32(0));
#elif defined(HAVE_BUILTIN_CONSTANT_P)
	if (size > 24 && size <= 32) {
		return ir_CALL(IR_ADDR, ir_CONST_FC_FUNC(_emalloc_32));
	} else {
		return ir_CALL_1(IR_ADDR, ir_CONST_FC_FUNC(_emalloc), ir_CONST_ADDR(size));
	}
#else
	return ir_CALL_1(IR_ADDR, ir_CONST_FC_FUNC(_emalloc), ir_CONST_ADDR(size));
#endif
}

static ir_ref jit_EFREE(zend_jit_ctx *jit, ir_ref ptr, size_t size, const zend_op_array *op_array, const zend_op *opline)
{
#if ZEND_DEBUG
	return ir_CALL_5(IR_ADDR, ir_CONST_FC_FUNC(_efree),
		ptr,
		op_array && op_array->filename ? ir_CONST_ADDR(op_array->filename->val) : IR_NULL,
		ir_CONST_U32(opline ? opline->lineno : 0),
		IR_NULL,
		ir_CONST_U32(0));
#elif defined(HAVE_BUILTIN_CONSTANT_P)
	if (size > 24 && size <= 32) {
		return ir_CALL_1(IR_ADDR, ir_CONST_FC_FUNC(_efree_32), ptr);
	} else {
		return ir_CALL_1(IR_ADDR, ir_CONST_FC_FUNC(_efree), ptr);
	}
#else
	return ir_CALL_1(IR_ADDR, ir_CONST_FC_FUNC(_efree), ptr);
#endif
}

static ir_ref jit_FP(zend_jit_ctx *jit)
{
	ZEND_ASSERT(jit->ctx.control);
	if (jit->fp == IR_UNUSED) {
		/* Emit "RLOAD FP" once for basic block */
		jit->fp = ir_RLOAD_A(ZREG_FP);
	} else {
		ir_insn *insn;
		ir_ref ref = jit->ctx.control;

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
				jit->fp = ir_RLOAD_A(ZREG_FP);
				break;
			}
			ref = insn->op1;
		}
	}
	return jit->fp;
}

static void jit_STORE_FP(zend_jit_ctx *jit, ir_ref ref)
{
	ir_RSTORE(ZREG_FP, ref);
	jit->fp = IR_UNUSED;
}

static ir_ref jit_IP(zend_jit_ctx *jit)
{
	return ir_RLOAD_A(ZREG_IP);
}

static void jit_STORE_IP(zend_jit_ctx *jit, ir_ref ref)
{
	ir_RSTORE(ZREG_IP, ref);
}

static ir_ref jit_IP32(zend_jit_ctx *jit)
{
	return ir_RLOAD_U32(ZREG_IP);
}

static void jit_LOAD_IP(zend_jit_ctx *jit, ir_ref ref)
{
	if (GCC_GLOBAL_REGS) {
		jit_STORE_IP(jit, ref);
	} else {
		ir_STORE(jit_EX(opline), ref);
	}
}

static void jit_LOAD_IP_ADDR(zend_jit_ctx *jit, const zend_op *target)
{
	jit_LOAD_IP(jit, ir_CONST_ADDR(target));
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
		jit_STORE_IP(jit, ir_LOAD_A(jit_EX(call)));
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
		call = ir_LOAD_A(jit_EX(call));
	}

	rx = jit_IP(jit);

	// JIT: call->prev_execute_data = call;
	ir_STORE(jit_CALL(rx, prev_execute_data), call);

	// JIT: EX(call) = call;
	ir_STORE(jit_EX(call), rx);

	jit->delayed_call_level = 0;
	delayed_call_chain = 0;

	return 1;
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
			if (GCC_GLOBAL_REGS) {
				ref = jit_IP(jit);
			} else {
				addr = jit_EX(opline);
				ref = ir_LOAD_A(addr);
			}
			if (target > jit->last_valid_opline) {
				ref = ir_ADD_OFFSET(ref, (uintptr_t)target - (uintptr_t)jit->last_valid_opline);
			} else {
				ref = ir_SUB_A(ref, ir_CONST_ADDR((uintptr_t)jit->last_valid_opline - (uintptr_t)target));
			}
			if (GCC_GLOBAL_REGS) {
				jit_STORE_IP(jit, ref);
			} else {
				ir_STORE(addr, ref);
			}
		}
	} else {
		if (GCC_GLOBAL_REGS) {
			jit_STORE_IP(jit, ir_CONST_ADDR(target));
		} else {
			ir_STORE(jit_EX(opline), ir_CONST_ADDR(target));
		}
	}
	jit->reuse_ip = 0;
	zend_jit_set_last_valid_opline(jit, target);
	return 1;
}

static int zend_jit_set_ip_ex(zend_jit_ctx *jit, const zend_op *target, bool set_ip_reg)
{
	if (!GCC_GLOBAL_REGS && set_ip_reg && !jit->last_valid_opline) {
		/* Optimization to avoid duplicate constant load */
		ir_STORE(jit_EX(opline), ir_HARD_COPY_A(ir_CONST_ADDR(target)));
		return 1;
	}
	return zend_jit_set_ip(jit, target);
}

static void jit_SET_EX_OPLINE(zend_jit_ctx *jit, const zend_op *target)
{
	if (jit->last_valid_opline == target) {
		zend_jit_use_last_valid_opline(jit);
		if (GCC_GLOBAL_REGS) {
			// EX(opline) = opline
			ir_STORE(jit_EX(opline), jit_IP(jit));
		}
	} else {
		ir_STORE(jit_EX(opline), ir_CONST_ADDR(target));
		if (!GCC_GLOBAL_REGS) {
			zend_jit_reset_last_valid_opline(jit);
		}
	}
}

static ir_ref jit_ZVAL_ADDR(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_MEM_ZVAL) {
		ir_ref reg;

		if (Z_REG(addr) == ZREG_FP) {
			reg = jit_FP(jit);
		} else if (Z_REG(addr) == ZREG_RX) {
			reg = jit_IP(jit);
		} else {
			ZEND_ASSERT(0);
		}
		return ir_ADD_OFFSET(reg, Z_OFFSET(addr));
	} else if (Z_MODE(addr) == IS_REF_ZVAL) {
		return Z_IR_REF(addr);
	} else {
		ZEND_ASSERT(Z_MODE(addr) == IS_CONST_ZVAL);
		return ir_CONST_ADDR(Z_ZV(addr));
	}
}

static ir_ref jit_Z_TYPE_ref(zend_jit_ctx *jit, ir_ref ref)
{
	return ir_LOAD_U8(ir_ADD_OFFSET(ref, offsetof(zval, u1.v.type)));
}

static ir_ref jit_Z_TYPE(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_CONST_ZVAL) {
		return ir_CONST_U8(Z_TYPE_P(Z_ZV(addr)));
	} else if (Z_MODE(addr) == IS_MEM_ZVAL) {
		ir_ref reg;

		ZEND_ASSERT(Z_MODE(addr) == IS_MEM_ZVAL);
		if (Z_REG(addr) == ZREG_FP) {
			reg = jit_FP(jit);
		} else if (Z_REG(addr) == ZREG_RX) {
			reg = jit_IP(jit);
		} else {
			ZEND_ASSERT(0);
		}
		return ir_LOAD_U8(ir_ADD_OFFSET(reg, Z_OFFSET(addr) + offsetof(zval, u1.v.type)));
	} else {
		return jit_Z_TYPE_ref(jit, jit_ZVAL_ADDR(jit, addr));
	}
}

static ir_ref jit_Z_TYPE_FLAGS_ref(zend_jit_ctx *jit, ir_ref ref)
{
	return ir_LOAD_U8(ir_ADD_OFFSET(ref, offsetof(zval, u1.v.type_flags)));
}

static ir_ref jit_Z_TYPE_FLAGS(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_CONST_ZVAL) {
		return ir_CONST_U8(Z_TYPE_FLAGS_P(Z_ZV(addr)));
	} else if (Z_MODE(addr) == IS_MEM_ZVAL) {
		ir_ref reg;

		ZEND_ASSERT(Z_MODE(addr) == IS_MEM_ZVAL);
		if (Z_REG(addr) == ZREG_FP) {
			reg = jit_FP(jit);
		} else if (Z_REG(addr) == ZREG_RX) {
			reg = jit_IP(jit);
		} else {
			ZEND_ASSERT(0);
		}
		return ir_LOAD_U8(ir_ADD_OFFSET(reg, Z_OFFSET(addr) + offsetof(zval, u1.v.type_flags)));
	} else {
		return jit_Z_TYPE_FLAGS_ref(jit, jit_ZVAL_ADDR(jit, addr));
	}
}

static ir_ref jit_Z_TYPE_INFO_ref(zend_jit_ctx *jit, ir_ref ref)
{
	return ir_LOAD_U32(ir_ADD_OFFSET(ref, offsetof(zval, u1.type_info)));
}

static ir_ref jit_Z_TYPE_INFO(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_CONST_ZVAL) {
		return ir_CONST_U32(Z_TYPE_INFO_P(Z_ZV(addr)));
	} else if (Z_MODE(addr) == IS_MEM_ZVAL) {
		ir_ref reg;

		ZEND_ASSERT(Z_MODE(addr) == IS_MEM_ZVAL);
		if (Z_REG(addr) == ZREG_FP) {
			reg = jit_FP(jit);
		} else if (Z_REG(addr) == ZREG_RX) {
			reg = jit_IP(jit);
		} else {
			ZEND_ASSERT(0);
		}
		return ir_LOAD_U32(ir_ADD_OFFSET(reg, Z_OFFSET(addr) + offsetof(zval, u1.type_info)));
	} else {
		return jit_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, addr));
	}
}

static void jit_set_Z_TYPE_INFO_ref(zend_jit_ctx *jit, ir_ref ref, ir_ref type_info)
{
	ir_STORE(ir_ADD_OFFSET(ref, offsetof(zval, u1.type_info)), type_info);
}

static void jit_set_Z_TYPE_INFO_ex(zend_jit_ctx *jit, zend_jit_addr addr, ir_ref type_info)
{
	if (Z_MODE(addr) == IS_MEM_ZVAL) {
		ir_ref reg;

		ZEND_ASSERT(Z_MODE(addr) == IS_MEM_ZVAL);
		if (Z_REG(addr) == ZREG_FP) {
			reg = jit_FP(jit);
		} else if (Z_REG(addr) == ZREG_RX) {
			reg = jit_IP(jit);
		} else {
			ZEND_ASSERT(0);
		}
		ir_STORE(ir_ADD_OFFSET(reg, Z_OFFSET(addr) + offsetof(zval, u1.type_info)), type_info);
	} else {
		jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, addr), type_info);
	}
}

static void jit_set_Z_TYPE_INFO(zend_jit_ctx *jit, zend_jit_addr addr, uint32_t type_info)
{
	if (type_info < IS_STRING
	 && Z_MODE(addr) == IS_MEM_ZVAL
	 && Z_REG(addr) == ZREG_FP
	 && JIT_G(current_frame)
	 && STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(Z_OFFSET(addr))) == type_info) {
		/* type is already set */
		return;
	}
	jit_set_Z_TYPE_INFO_ex(jit, addr, ir_CONST_U32(type_info));
}

static ir_ref jit_if_Z_TYPE_ref(zend_jit_ctx *jit, ir_ref ref, ir_ref type)
{
	return ir_IF(ir_EQ(jit_Z_TYPE_ref(jit, ref), type));
}

static ir_ref jit_if_Z_TYPE(zend_jit_ctx *jit, zend_jit_addr addr, uint8_t type)
{
	ZEND_ASSERT(type != IS_UNDEF);
	return ir_IF(ir_EQ(jit_Z_TYPE(jit, addr), ir_CONST_U8(type)));
}

static ir_ref jit_if_not_Z_TYPE(zend_jit_ctx *jit, zend_jit_addr addr, uint8_t type)
{
	ir_ref ref = jit_Z_TYPE(jit, addr);

	if (type != IS_UNDEF) {
		ref = ir_NE(ref, ir_CONST_U8(type));
	}
	return ir_IF(ref);
}

static void jit_guard_Z_TYPE(zend_jit_ctx *jit, zend_jit_addr addr, uint8_t type, const void *exit_addr)
{
	ir_ref ref = jit_Z_TYPE(jit, addr);

	if (type != IS_UNDEF) {
		ir_GUARD(ir_EQ(ref, ir_CONST_U8(type)), ir_CONST_ADDR(exit_addr));
	} else {
		ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
	}
}

static void jit_guard_not_Z_TYPE(zend_jit_ctx *jit, zend_jit_addr addr, uint8_t type, const void *exit_addr)
{
	ir_ref ref = jit_Z_TYPE(jit, addr);

	if (type != IS_UNDEF) {
		ref = ir_NE(ref, ir_CONST_U8(type));
	}
	ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
}

static ir_ref jit_if_REFCOUNTED(zend_jit_ctx *jit, zend_jit_addr addr)
{
	return ir_IF(jit_Z_TYPE_FLAGS(jit, addr));
}

static ir_ref jit_if_COLLECTABLE_ref(zend_jit_ctx *jit, ir_ref addr_ref)
{
	return ir_IF(ir_AND_U8(jit_Z_TYPE_FLAGS_ref(jit, addr_ref), ir_CONST_U8(IS_TYPE_COLLECTABLE)));
}

static ir_ref jit_Z_LVAL_ref(zend_jit_ctx *jit, ir_ref ref)
{
	return ir_LOAD_L(ref);
}

static ir_ref jit_Z_DVAL_ref(zend_jit_ctx *jit, ir_ref ref)
{
	return ir_LOAD_D(ref);
}

static void zend_jit_def_reg(zend_jit_ctx *jit, zend_jit_addr addr, ir_ref val)
{
	int var;

	ZEND_ASSERT(Z_MODE(addr) == IS_REG);
	var = Z_SSA_VAR(addr);
	if (var == jit->delay_var) {
		ir_refs_add(jit->delay_refs, val);
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
			ref = jit_Z_LVAL_ref(jit, jit_ZVAL_ADDR(jit, mem_addr));
		} else if ((jit->ssa->var_info[var].type & MAY_BE_ANY) == MAY_BE_DOUBLE) {
			ref = jit_Z_DVAL_ref(jit, jit_ZVAL_ADDR(jit, mem_addr));
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
	ir_type type = (jit->ssa->var_info[phi->ssa_var].type & MAY_BE_LONG) ? IR_LONG : IR_DOUBLE;
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
			ir_set_op(&jit->ctx, ref, i + 2, zend_jit_use_reg(jit, ZEND_ADDR_REG(src_var)));
		}
	}

	zend_jit_def_reg(jit, ZEND_ADDR_REG(dst_var), ref);
}

static ir_ref jit_Z_LVAL(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_CONST_ZVAL) {
		return ir_CONST_LONG(Z_LVAL_P(Z_ZV(addr)));
	} else if (Z_MODE(addr) == IS_REG) {
		return zend_jit_use_reg(jit, addr);
	} else {
		return jit_Z_LVAL_ref(jit, jit_ZVAL_ADDR(jit, addr));
	}
}

static void jit_set_Z_LVAL(zend_jit_ctx *jit, zend_jit_addr addr, ir_ref lval)
{
	if (Z_MODE(addr) == IS_REG) {
		zend_jit_def_reg(jit, addr, lval);
	} else {
		ir_STORE(jit_ZVAL_ADDR(jit, addr), lval);
	}
}

#if SIZEOF_ZEND_LONG == 4
static ir_ref jit_Z_W2(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_CONST_ZVAL) {
		return ir_CONST_U32((Z_ZV(addr))->value.ww.w2);
	} else {
		return ir_LOAD_L(ir_ADD_OFFSET(jit_ZVAL_ADDR(jit, addr), offsetof(zval, value.ww.w2)));
	}
}

static void jit_set_Z_W2(zend_jit_ctx *jit, zend_jit_addr addr, ir_ref lval)
{
	ir_STORE(ir_ADD_OFFSET(jit_ZVAL_ADDR(jit, addr), offsetof(zval, value.ww.w2)), lval);
}
#endif

static ir_ref jit_Z_DVAL(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_CONST_ZVAL) {
		return ir_CONST_DOUBLE(Z_DVAL_P(Z_ZV(addr)));
	} else if (Z_MODE(addr) == IS_REG) {
		return zend_jit_use_reg(jit, addr);
	} else {
		return jit_Z_DVAL_ref(jit, jit_ZVAL_ADDR(jit, addr));
	}
}

static void jit_set_Z_DVAL(zend_jit_ctx *jit, zend_jit_addr addr, ir_ref dval)
{
	if (Z_MODE(addr) == IS_REG) {
		zend_jit_def_reg(jit, addr, dval);
	} else {
		ir_STORE(jit_ZVAL_ADDR(jit, addr), dval);
	}
}

static ir_ref jit_Z_PTR_ref(zend_jit_ctx *jit, ir_ref ref)
{
	return ir_LOAD_A(ref);
}

static ir_ref jit_Z_PTR(zend_jit_ctx *jit, zend_jit_addr addr)
{
	if (Z_MODE(addr) == IS_CONST_ZVAL) {
		return ir_CONST_ADDR(Z_PTR_P(Z_ZV(addr)));
	} else {
		return jit_Z_PTR_ref(jit, jit_ZVAL_ADDR(jit, addr));
	}
}

static void jit_set_Z_PTR(zend_jit_ctx *jit, zend_jit_addr addr, ir_ref ptr)
{
	ir_STORE(jit_ZVAL_ADDR(jit, addr), ptr);
}

static ir_ref jit_GC_REFCOUNT(zend_jit_ctx *jit, ir_ref ref)
{
	return ir_LOAD_U32(ref);
}

static void jit_set_GC_REFCOUNT(zend_jit_ctx *jit, ir_ref ref, uint32_t refcount)
{
	ir_STORE(ref, ir_CONST_U32(refcount));
}

static void jit_GC_ADDREF(zend_jit_ctx *jit, ir_ref ref)
{
	ir_STORE(ref, ir_ADD_U32(ir_LOAD_U32(ref), ir_CONST_U32(1)));
}

static void jit_GC_ADDREF2(zend_jit_ctx *jit, ir_ref ref)
{
	ir_ref counter = ir_LOAD_U32(ref);
	ir_STORE(ref, ir_ADD_U32(counter, ir_CONST_U32(2)));
}

static ir_ref jit_GC_DELREF(zend_jit_ctx *jit, ir_ref ref)
{
	ir_ref counter = ir_LOAD_U32(ref);
	counter = ir_SUB_U32(counter, ir_CONST_U32(1));
	ir_STORE(ref, counter);
	return counter;
}

static ir_ref jit_if_GC_MAY_NOT_LEAK(zend_jit_ctx *jit, ir_ref ref)
{
	return ir_IF(
		ir_AND_U32(
			ir_LOAD_U32(ir_ADD_OFFSET(ref, offsetof(zend_refcounted, gc.u.type_info))),
			ir_CONST_U32(GC_INFO_MASK | (GC_NOT_COLLECTABLE << GC_FLAGS_SHIFT))));
}

static void jit_ZVAL_COPY_CONST(zend_jit_ctx *jit, zend_jit_addr dst, uint32_t dst_info, uint32_t dst_def_info, zval *zv, bool addref)
{
	ir_ref ref = IR_UNUSED;

	if (Z_TYPE_P(zv) > IS_TRUE) {
		if (Z_TYPE_P(zv) == IS_DOUBLE) {
			jit_set_Z_DVAL(jit, dst, ir_CONST_DOUBLE(Z_DVAL_P(zv)));
		} else if (Z_TYPE_P(zv) == IS_LONG && dst_def_info == MAY_BE_DOUBLE) {
			jit_set_Z_DVAL(jit, dst, ir_CONST_DOUBLE((double)Z_LVAL_P(zv)));
		} else if (Z_TYPE_P(zv) == IS_LONG) {
			jit_set_Z_LVAL(jit, dst, ir_CONST_LONG(Z_LVAL_P(zv)));
		} else {
			ref = ir_CONST_ADDR(Z_PTR_P(zv));
			jit_set_Z_PTR(jit, dst, ref);
			if (addref && Z_REFCOUNTED_P(zv)) {
				jit_GC_ADDREF(jit, ref);
			}
		}
	}
	if (Z_MODE(dst) != IS_REG) {
		if (dst_def_info == MAY_BE_DOUBLE) {
			if ((dst_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_GUARD)) != MAY_BE_DOUBLE) {
				jit_set_Z_TYPE_INFO(jit, dst, IS_DOUBLE);
			}
		} else if (((dst_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_GUARD)) != (1<<Z_TYPE_P(zv))) || (dst_info & (MAY_BE_STRING|MAY_BE_ARRAY)) != 0) {
			jit_set_Z_TYPE_INFO(jit, dst, Z_TYPE_INFO_P(zv));
		}
	}
}

static ir_ref jit_if_TYPED_REF(zend_jit_ctx *jit, ir_ref ref)
{
	return ir_IF(ir_LOAD_A(ir_ADD_OFFSET(ref, offsetof(zend_reference, sources.ptr))));
}

static void jit_ZVAL_COPY(zend_jit_ctx *jit, zend_jit_addr dst, uint32_t dst_info, zend_jit_addr src, uint32_t src_info, bool addref)
{
	ir_ref ref = IR_UNUSED;

	if (src_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE))) {
		if ((src_info & (MAY_BE_ANY|MAY_BE_GUARD)) == MAY_BE_LONG) {
			jit_set_Z_LVAL(jit, dst, jit_Z_LVAL(jit, src));
		} else if ((src_info & (MAY_BE_ANY|MAY_BE_GUARD)) == MAY_BE_DOUBLE) {
			jit_set_Z_DVAL(jit, dst, jit_Z_DVAL(jit, src));
		} else {
#if SIZEOF_ZEND_LONG == 4
			if (src_info & (MAY_BE_DOUBLE|MAY_BE_GUARD)) {
				jit_set_Z_W2(jit, dst, jit_Z_W2(jit, src));
			}
#endif
			ref = jit_Z_PTR(jit, src);
			jit_set_Z_PTR(jit, dst, ref);
		}
	}
	if (has_concrete_type(src_info & MAY_BE_ANY)
	 && (src_info & (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE))
     && !(src_info & MAY_BE_GUARD)) {
		if (Z_MODE(dst) != IS_REG
		 && (dst_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_GUARD)) != (src_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_GUARD))) {
			zend_uchar type = concrete_type(src_info);
			jit_set_Z_TYPE_INFO(jit, dst, type);
		}
	} else {
		ir_ref type = jit_Z_TYPE_INFO(jit, src);
		jit_set_Z_TYPE_INFO_ex(jit, dst, type);
		if (addref) {
			if (src_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
				ir_ref if_refcounted = IR_UNUSED;

				if (src_info & (MAY_BE_ANY-(MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
					if_refcounted = ir_IF(ir_AND_U32(type, ir_CONST_U32(0xff00)));
					ir_IF_TRUE(if_refcounted);
				}

				jit_GC_ADDREF(jit, ref);

				if (src_info & (MAY_BE_ANY-(MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
					ir_MERGE_WITH_EMPTY_FALSE(if_refcounted);
				}
			}
		}
	}
}

static void jit_ZVAL_COPY_2(zend_jit_ctx *jit, zend_jit_addr dst2, zend_jit_addr dst, uint32_t dst_info, zend_jit_addr src, uint32_t src_info, int addref)
{
	ir_ref ref = IR_UNUSED;

	if (src_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE))) {
		if ((src_info & (MAY_BE_ANY|MAY_BE_GUARD)) == MAY_BE_LONG) {
			ref = jit_Z_LVAL(jit, src);
			jit_set_Z_LVAL(jit, dst, ref);
			jit_set_Z_LVAL(jit, dst2, ref);
		} else if ((src_info & (MAY_BE_ANY|MAY_BE_GUARD)) == MAY_BE_DOUBLE) {
			ref = jit_Z_DVAL(jit, src);
			jit_set_Z_DVAL(jit, dst, ref);
			jit_set_Z_DVAL(jit, dst2, ref);
		} else {
#if SIZEOF_ZEND_LONG == 4
			if (src_info & (MAY_BE_DOUBLE|MAY_BE_GUARD)) {
				ref = jit_Z_W2(jit, src);
				jit_set_Z_W2(jit, dst, ref);
				jit_set_Z_W2(jit, dst2, ref);
			}
#endif
			ref = jit_Z_PTR(jit, src);
			jit_set_Z_PTR(jit, dst, ref);
			jit_set_Z_PTR(jit, dst2, ref);
		}
	}
	if (has_concrete_type(src_info & MAY_BE_ANY)
	 && (src_info & (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE))
     && !(src_info & MAY_BE_GUARD)) {
		zend_uchar type = concrete_type(src_info);
		ir_ref type_ref = ir_CONST_U32(type);

		if (Z_MODE(dst) != IS_REG
		 && (dst_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_GUARD)) != (src_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_GUARD))) {
			jit_set_Z_TYPE_INFO_ex(jit, dst, type_ref);
		}
		if (Z_MODE(dst2) != IS_REG) {
			jit_set_Z_TYPE_INFO_ex(jit, dst2, type_ref);
		}
	} else {
		ir_ref type = jit_Z_TYPE_INFO(jit, src);
		jit_set_Z_TYPE_INFO_ex(jit, dst, type);
		jit_set_Z_TYPE_INFO_ex(jit, dst2, type);
		if (addref) {
			if (src_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
				ir_ref if_refcounted = IR_UNUSED;

				if (src_info & (MAY_BE_ANY-(MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
					if_refcounted = ir_IF(ir_AND_U32(type, ir_CONST_U32(0xff00)));
					ir_IF_TRUE(if_refcounted);
				}

				if (addref == 2) {
					jit_GC_ADDREF2(jit, ref);
				} else {
					jit_GC_ADDREF(jit, ref);
				}

				if (src_info & (MAY_BE_ANY-(MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
					ir_MERGE_WITH_EMPTY_FALSE(if_refcounted);
				}
			}
		}
	}
}

static void jit_ZVAL_DTOR(zend_jit_ctx *jit, ir_ref ref, uint32_t op_info, const zend_op *opline)
{
	if (!((op_info) & MAY_BE_GUARD)
	 && has_concrete_type((op_info) & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
		zend_uchar type = concrete_type((op_info) & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE));
		if (type == IS_STRING && !ZEND_DEBUG) {
				ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(_efree), ref);
				return;
		} else if (type == IS_ARRAY) {
			if ((op_info) & (MAY_BE_ARRAY_KEY_STRING|MAY_BE_ARRAY_OF_STRING|MAY_BE_ARRAY_OF_ARRAY|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_REF)) {
				if (opline && ((op_info) & (MAY_BE_ARRAY_OF_ARRAY|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_REF))) {
					jit_SET_EX_OPLINE(jit, opline);
				}
				ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_array_destroy), ref);
			} else {
				ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_array_free), ref);
			}
			return;
		} else if (type == IS_OBJECT) {
			if (opline) {
				jit_SET_EX_OPLINE(jit, opline);
			}
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_objects_store_del), ref);
			return;
		}
	}
	if (opline) {
		jit_SET_EX_OPLINE(jit, opline);
	}
	ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(rc_dtor_func), ref);
}

static void jit_ZVAL_PTR_DTOR(zend_jit_ctx  *jit,
                              zend_jit_addr  addr,
                              uint32_t       op_info,
                              bool           gc,
                              const zend_op *opline)
{
    ir_ref ref, ref2;
	ir_ref if_refcounted = IR_UNUSED;
	ir_ref if_not_zero = IR_UNUSED;
	ir_ref end_inputs = IR_UNUSED;

	if (op_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF|MAY_BE_GUARD)) {
		if ((op_info) & ((MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_INDIRECT|MAY_BE_GUARD)-(MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
			if_refcounted = jit_if_REFCOUNTED(jit, addr);
			ir_IF_FALSE(if_refcounted);
			ir_END_list(end_inputs);
			ir_IF_TRUE(if_refcounted);
		}
		ref = jit_Z_PTR(jit, addr);
		ref2 = jit_GC_DELREF(jit, ref);

		if (((op_info) & MAY_BE_GUARD) || RC_MAY_BE_1(op_info)) {
			if (((op_info) & MAY_BE_GUARD) || RC_MAY_BE_N(op_info)) {
				if_not_zero = ir_IF(ref2);
				ir_IF_FALSE(if_not_zero);
			}
			// zval_dtor_func(r);
			jit_ZVAL_DTOR(jit, ref, op_info, opline);
			if (if_not_zero) {
				ir_END_list(end_inputs);
				ir_IF_TRUE(if_not_zero);
			}
		}
		if (gc && (((op_info) & MAY_BE_GUARD) || (RC_MAY_BE_N(op_info) && ((op_info) & (MAY_BE_REF|MAY_BE_ARRAY|MAY_BE_OBJECT)) != 0))) {
			ir_ref if_may_not_leak;

			if ((op_info) & (MAY_BE_REF|MAY_BE_GUARD)) {
				ir_ref if_ref, if_collectable;

				if_ref = jit_if_Z_TYPE(jit, addr, IS_REFERENCE);
				ir_IF_TRUE(if_ref);

				ref2 = ir_ADD_OFFSET(ref, offsetof(zend_reference, val));

				if_collectable = jit_if_COLLECTABLE_ref(jit, ref2);
				ir_IF_FALSE(if_collectable);
				ir_END_list(end_inputs);
				ir_IF_TRUE(if_collectable);

				ref2 = jit_Z_PTR_ref(jit, ref2);

				ir_MERGE_WITH_EMPTY_FALSE(if_ref);
				ref = ir_PHI_2(ref2, ref);
			}

			if_may_not_leak = jit_if_GC_MAY_NOT_LEAK(jit, ref);
			ir_IF_TRUE(if_may_not_leak);
			ir_END_list(end_inputs);
			ir_IF_FALSE(if_may_not_leak);

			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(gc_possible_root), ref);
		}

		if (end_inputs) {
			ir_END_list(end_inputs);
			ir_MERGE_list(end_inputs);
		}
	}
}

static void jit_FREE_OP(zend_jit_ctx  *jit,
                        zend_uchar     op_type,
                        znode_op       op,
                        uint32_t       op_info,
                        const zend_op *opline)
{
	if (op_type & (IS_VAR|IS_TMP_VAR)) {
		jit_ZVAL_PTR_DTOR(jit,
			ZEND_ADDR_MEM_ZVAL(ZREG_FP, op.var),
			op_info, 0, opline);
	}
}

static void jit_OBJ_RELEASE(zend_jit_ctx  *jit, ir_ref ref)
{
	ir_ref end_inputs = IS_UNDEF;
    ir_ref if_not_zero, if_may_not_leak;

	// JIT: if (GC_DELREF(obj) == 0) {
	if_not_zero = ir_IF(jit_GC_DELREF(jit, ref));
	ir_IF_FALSE(if_not_zero);

	// JIT: zend_objects_store_del(obj)
	ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_objects_store_del), ref);
	ir_END_list(end_inputs);

	ir_IF_TRUE(if_not_zero);
	if_may_not_leak = jit_if_GC_MAY_NOT_LEAK(jit, ref);

	ir_IF_TRUE(if_may_not_leak);
	ir_END_list(end_inputs);

	ir_IF_FALSE(if_may_not_leak);
	ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(gc_possible_root), ref);
	ir_END_list(end_inputs);

	ir_MERGE_list(end_inputs);
}

static int zend_jit_check_timeout(zend_jit_ctx *jit, const zend_op *opline, const void *exit_addr)
{
	ir_ref ref = ir_LOAD_U8(jit_EG(vm_interrupt));

	if (exit_addr) {
		ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
	} else if (!opline || jit->last_valid_opline == opline) {
		ir_GUARD_NOT(ref, jit_STUB_ADDR(jit, jit_stub_interrupt_handler));
	} else {
		ir_ref if_timeout = ir_IF(ref);

		ir_IF_TRUE_cold(if_timeout);
		jit_LOAD_IP_ADDR(jit, opline);
		ir_IJMP(jit_STUB_ADDR(jit, jit_stub_interrupt_handler));
		ir_IF_FALSE(if_timeout);
	}
	return 1;
}

/* stubs */

static int zend_jit_exception_handler_stub(zend_jit_ctx *jit)
{
	const void *handler;

	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		handler = zend_get_opcode_handler_func(EG(exception_op));

		ir_CALL(IR_VOID, ir_CONST_FUNC(handler));
		ir_TAILCALL(ir_LOAD_A(jit_IP(jit)));
	} else {
		handler = EG(exception_op)->handler;

		if (GCC_GLOBAL_REGS) {
			ir_TAILCALL(ir_CONST_FUNC(handler));
		} else {
			ir_ref ref, if_negative;

			ref = ir_CALL_1(IR_I32, ir_CONST_FC_FUNC(handler), jit_FP(jit));
			if_negative = ir_IF(ir_LT(ref, ir_CONST_U32(0)));
			ir_IF_TRUE(if_negative);
			ir_MERGE_WITH_EMPTY_FALSE(if_negative);
			ref = ir_PHI_2(ref, ir_CONST_I32(1));
			ir_RETURN(ref);
		}
	}
	return 1;
}

static int zend_jit_exception_handler_undef_stub(zend_jit_ctx *jit)
{
	ir_ref ref, result_type, if_result_used;

	ref = jit_EG(opline_before_exception);
	result_type = ir_LOAD_U8(ir_ADD_OFFSET(ir_LOAD_A(ref), offsetof(zend_op, result_type)));

	if_result_used = ir_IF(ir_AND_U8(result_type, ir_CONST_U8(IS_TMP_VAR|IS_VAR)));
	ir_IF_TRUE(if_result_used);

	ref = ir_LOAD_U32(ir_ADD_OFFSET(ir_LOAD_A(ref), offsetof(zend_op, result.var)));
	if (sizeof(void*) == 8) {
		ref = ir_ZEXT_A(ref);
	}
	ir_STORE(ir_ADD_OFFSET(ir_ADD_A(jit_FP(jit), ref), offsetof(zval, u1.type_info)), ir_CONST_U32(IS_UNDEF));
	ir_MERGE_WITH_EMPTY_FALSE(if_result_used);

	ir_IJMP(jit_STUB_ADDR(jit, jit_stub_exception_handler));

	return 1;
}

static int zend_jit_exception_handler_free_op1_op2_stub(zend_jit_ctx *jit)
{
	ir_ref ref, if_dtor;
	zend_jit_addr var_addr;

	ref = ir_LOAD_A(jit_EG(opline_before_exception));
	if_dtor = ir_IF(ir_AND_U8(ir_LOAD_U8(ir_ADD_OFFSET(ref, offsetof(zend_op, op1_type))),
		ir_CONST_U8(IS_TMP_VAR|IS_VAR)));
	ir_IF_TRUE(if_dtor);
	ref = ir_LOAD_U32(ir_ADD_OFFSET(ref, offsetof(zend_op, op1.var)));
	if (sizeof(void*) == 8) {
		ref = ir_ZEXT_A(ref);
	}
	ref = ir_ADD_A(jit_FP(jit), ref);
	var_addr = ZEND_ADDR_REF_ZVAL(ref);
	jit_ZVAL_PTR_DTOR(jit, var_addr, MAY_BE_ANY|MAY_BE_RC1|MAY_BE_RCN|MAY_BE_REF, 0, NULL);
	ir_MERGE_WITH_EMPTY_FALSE(if_dtor);

	ir_IJMP(jit_STUB_ADDR(jit, jit_stub_exception_handler_free_op2));

	return 1;
}

static int zend_jit_exception_handler_free_op2_stub(zend_jit_ctx *jit)
{
	ir_ref ref, if_dtor;
	zend_jit_addr var_addr;

	ref = ir_LOAD_A(jit_EG(opline_before_exception));
	if_dtor = ir_IF(ir_AND_U8(ir_LOAD_U8(ir_ADD_OFFSET(ref, offsetof(zend_op, op2_type))),
		ir_CONST_U8(IS_TMP_VAR|IS_VAR)));
	ir_IF_TRUE(if_dtor);
	ref = ir_LOAD_U32(ir_ADD_OFFSET(ref, offsetof(zend_op, op2.var)));
	if (sizeof(void*) == 8) {
		ref = ir_ZEXT_A(ref);
	}
	ref = ir_ADD_A(jit_FP(jit), ref);
	var_addr = ZEND_ADDR_REF_ZVAL(ref);
	jit_ZVAL_PTR_DTOR(jit, var_addr, MAY_BE_ANY|MAY_BE_RC1|MAY_BE_RCN|MAY_BE_REF, 0, NULL);
	ir_MERGE_WITH_EMPTY_FALSE(if_dtor);

	ir_IJMP(jit_STUB_ADDR(jit, jit_stub_exception_handler_undef));

	return 1;
}

static int zend_jit_interrupt_handler_stub(zend_jit_ctx *jit)
{
	ir_ref if_timeout, if_exception;

	if (GCC_GLOBAL_REGS) {
		// EX(opline) = opline
		ir_STORE(jit_EX(opline), jit_IP(jit));
	}

	ir_STORE(jit_EG(vm_interrupt), ir_CONST_U8(0));
	if_timeout = ir_IF(ir_EQ(ir_LOAD_U8(jit_EG(timed_out)), ir_CONST_U8(0)));
	ir_IF_FALSE(if_timeout);
	ir_CALL(IR_VOID, ir_CONST_FUNC(zend_timeout));
	ir_MERGE_WITH_EMPTY_TRUE(if_timeout);

	if (zend_interrupt_function) {
		ir_CALL_1(IR_VOID, ir_CONST_FUNC(zend_interrupt_function), jit_FP(jit));
		if_exception = ir_IF(ir_LOAD_A(jit_EG(exception)));
		ir_IF_TRUE(if_exception);
		ir_CALL(IR_VOID, ir_CONST_FUNC(zend_jit_exception_in_interrupt_handler_helper));
		ir_MERGE_WITH_EMPTY_FALSE(if_exception);

		jit_STORE_FP(jit, ir_LOAD_A(jit_EG(current_execute_data)));
		jit_STORE_IP(jit, ir_LOAD_A(jit_EX(opline)));
	}

	if (GCC_GLOBAL_REGS) {
		ir_TAILCALL(ir_LOAD_A(jit_IP(jit)));
	} else {
		ir_RETURN(ir_CONST_I32(1));
	}
	return 1;
}

static int zend_jit_leave_function_handler_stub(zend_jit_ctx *jit)
{
	ir_ref call_info = ir_LOAD_U32(jit_EX(This.u1.type_info));
	ir_ref if_top = ir_IF(ir_AND_U32(call_info, ir_CONST_U32(ZEND_CALL_TOP)));

	ir_IF_FALSE(if_top);

	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_leave_nested_func_helper), call_info);
		jit_STORE_IP(jit,
			ir_LOAD_A(jit_EX(opline)));
		ir_TAILCALL(ir_LOAD_A(jit_IP(jit)));
	} else if (GCC_GLOBAL_REGS) {
		ir_TAILCALL_1(ir_CONST_FC_FUNC(zend_jit_leave_nested_func_helper), call_info);
	} else {
		ir_TAILCALL_2(ir_CONST_FC_FUNC(zend_jit_leave_nested_func_helper), call_info, jit_FP(jit));
	}

	ir_IF_TRUE(if_top);

	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_leave_top_func_helper), call_info);
		ir_TAILCALL(ir_LOAD_A(jit_IP(jit)));
	} else if (GCC_GLOBAL_REGS) {
		ir_TAILCALL_1(ir_CONST_FC_FUNC(zend_jit_leave_top_func_helper), call_info);
	} else {
		ir_TAILCALL_2(ir_CONST_FC_FUNC(zend_jit_leave_top_func_helper), call_info, jit_FP(jit));
	}

	return 1;
}

static int zend_jit_negative_shift_stub(zend_jit_ctx *jit)
{
	ir_CALL_2(IR_VOID, ir_CONST_FUNC(zend_throw_error),
		ir_CONST_ADDR(zend_ce_arithmetic_error),
		ir_CONST_ADDR("Bit shift by negative number"));
	ir_IJMP(jit_STUB_ADDR(jit, jit_stub_exception_handler_free_op1_op2));
	return 1;
}

static int zend_jit_mod_by_zero_stub(zend_jit_ctx *jit)
{
	ir_CALL_2(IR_VOID, ir_CONST_FUNC(zend_throw_error),
		ir_CONST_ADDR(zend_ce_division_by_zero_error),
		ir_CONST_ADDR("Modulo by zero"));
	ir_IJMP(jit_STUB_ADDR(jit, jit_stub_exception_handler_free_op1_op2));
	return 1;
}

static int zend_jit_invalid_this_stub(zend_jit_ctx *jit)
{
	ir_CALL_2(IR_VOID, ir_CONST_FUNC(zend_throw_error),
		IR_NULL,
		ir_CONST_ADDR("Using $this when not in object context"));
	ir_IJMP(jit_STUB_ADDR(jit, jit_stub_exception_handler_undef));
	return 1;
}

static int zend_jit_undefined_function_stub(zend_jit_ctx *jit)
{
	// JIT: load EX(opline)
	ir_ref ref = ir_LOAD_A(jit_FP(jit));
	ir_ref arg3 = ir_LOAD_U32(ir_ADD_OFFSET(ref, offsetof(zend_op, op2.constant)));

	if (sizeof(void*) == 8) {
		arg3 = ir_LOAD_A(ir_ADD_A(ref, ir_SEXT_A(arg3)));
	} else {
		arg3 = ir_LOAD_A(arg3);
	}
	arg3 = ir_ADD_OFFSET(arg3, offsetof(zend_string, val));

	ir_CALL_3(IR_VOID, ir_CONST_FUNC(zend_throw_error),
		IR_NULL,
		ir_CONST_ADDR("Call to undefined function %s()"),
		arg3);

	ir_IJMP(jit_STUB_ADDR(jit, jit_stub_exception_handler));

	return 1;
}

static int zend_jit_throw_cannot_pass_by_ref_stub(zend_jit_ctx *jit)
{
	ir_ref opline, ref, rx, if_eq, if_tmp;

	// JIT: opline = EX(opline)
	opline = ir_LOAD_A(jit_FP(jit));

	// JIT: ZVAL_UNDEF(ZEND_CALL_VAR(RX, opline->result.var))
	ref = ir_LOAD_U32(ir_ADD_OFFSET(opline, offsetof(zend_op, result.var)));
	if (sizeof(void*) == 8) {
		ref = ir_ZEXT_A(ref);
	}
	rx = jit_IP(jit);
	jit_set_Z_TYPE_INFO_ref(jit, ir_ADD_A(rx, ref), ir_CONST_U32(IS_UNDEF));

	// last EX(call) frame may be delayed
	// JIT: if (EX(call) == RX)
	ref = ir_LOAD_A(jit_EX(call));
	if_eq = ir_IF(ir_EQ(rx, ref));
	ir_IF_FALSE(if_eq);

	// JIT: RX->prev_execute_data == EX(call)
	ir_STORE(jit_CALL(rx, prev_execute_data), ref);

	// JIT: EX(call) = RX
	ir_STORE(jit_EX(call), rx);
	ir_MERGE_WITH_EMPTY_TRUE(if_eq);

	// JIT: IP = opline
	jit_STORE_IP(jit, opline);

	// JIT: zend_cannot_pass_by_reference(opline->op2.num)
	ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_cannot_pass_by_reference),
		ir_LOAD_U32(ir_ADD_OFFSET(opline, offsetof(zend_op, op2.num))));

	// JIT: if (IP->op1_type == IS_TMP_VAR)
	ref = ir_LOAD_U8(ir_ADD_OFFSET(jit_IP(jit), offsetof(zend_op, op1_type)));
	if_tmp = ir_IF(ir_EQ(ref, ir_CONST_U8(IS_TMP_VAR)));
	ir_IF_TRUE(if_tmp);

	// JIT: zval_ptr_dtor(EX_VAR(IP->op1.var))
	ref = ir_LOAD_U32(ir_ADD_OFFSET(jit_IP(jit), offsetof(zend_op, op1.var)));
	if (sizeof(void*) == 8) {
		ref = ir_ZEXT_A(ref);
	}
	ref = ir_ADD_A(jit_FP(jit), ref);
	jit_ZVAL_PTR_DTOR(jit,
		ZEND_ADDR_REF_ZVAL(ref),
		MAY_BE_ANY|MAY_BE_RC1|MAY_BE_RCN|MAY_BE_REF, 0, NULL);
	ir_MERGE_WITH_EMPTY_FALSE(if_tmp);

	ir_IJMP(jit_STUB_ADDR(jit, jit_stub_exception_handler));

	return 1;
}

static int zend_jit_icall_throw_stub(zend_jit_ctx *jit)
{
	ir_ref ip, if_set;

	// JIT: zend_rethrow_exception(zend_execute_data *execute_data)
	// JIT: if (EX(opline)->opcode != ZEND_HANDLE_EXCEPTION) {
	jit_STORE_IP(jit, ir_LOAD_A(jit_EX(opline)));
	ip = jit_IP(jit);
	if_set = ir_IF(ir_EQ(ir_LOAD_U8(ir_ADD_OFFSET(ip, offsetof(zend_op, opcode))),
		ir_CONST_U8(ZEND_HANDLE_EXCEPTION)));
	ir_IF_FALSE(if_set);

	// JIT: EG(opline_before_exception) = opline;
	ir_STORE(jit_EG(opline_before_exception), ip);
	ir_MERGE_WITH_EMPTY_TRUE(if_set);

	// JIT: opline = EG(exception_op);
	jit_STORE_IP(jit, jit_EG(exception_op));

	if (GCC_GLOBAL_REGS) {
		ir_STORE(jit_EX(opline), jit_IP(jit));
	}

	ir_IJMP(jit_STUB_ADDR(jit, jit_stub_exception_handler));

	return 1;
}

static int zend_jit_leave_throw_stub(zend_jit_ctx *jit)
{
	ir_ref ip, if_set;

	// JIT: if (opline->opcode != ZEND_HANDLE_EXCEPTION) {
	jit_STORE_IP(jit, ir_LOAD_A(jit_EX(opline)));
	ip = jit_IP(jit);
	if_set = ir_IF(ir_EQ(ir_LOAD_U8(ir_ADD_OFFSET(ip, offsetof(zend_op, opcode))),
		ir_CONST_U8(ZEND_HANDLE_EXCEPTION)));
	ir_IF_FALSE(if_set);

	// JIT: EG(opline_before_exception) = opline;
	ir_STORE(jit_EG(opline_before_exception), ip);
	ir_MERGE_WITH_EMPTY_TRUE(if_set);

	// JIT: opline = EG(exception_op);
	jit_LOAD_IP(jit, jit_EG(exception_op));

	if (GCC_GLOBAL_REGS) {
		ir_STORE(jit_EX(opline), jit_IP(jit));

		// JIT: HANDLE_EXCEPTION()
		ir_IJMP(jit_STUB_ADDR(jit, jit_stub_exception_handler));
	} else {
		ir_RETURN(ir_CONST_I32(2)); // ZEND_VM_LEAVE
	}

	return 1;
}

static int zend_jit_hybrid_runtime_jit_stub(zend_jit_ctx *jit)
{
	if (zend_jit_vm_kind != ZEND_VM_KIND_HYBRID) {
		return 0;
	}

	ir_CALL(IR_VOID, ir_CONST_FC_FUNC(zend_runtime_jit));
	ir_IJMP(ir_LOAD_A(jit_IP(jit)));
	return 1;
}

static int zend_jit_hybrid_profile_jit_stub(zend_jit_ctx *jit)
{
	ir_ref addr, func, run_time_cache, jit_extension;

	if (zend_jit_vm_kind != ZEND_VM_KIND_HYBRID) {
		return 0;
	}

	addr = ir_CONST_ADDR(&zend_jit_profile_counter),
	ir_STORE(addr, ir_ADD_L(ir_LOAD_L(addr), ir_CONST_LONG(1)));

	func = ir_LOAD_A(jit_EX(func));
	run_time_cache = ir_LOAD_A(jit_EX(run_time_cache));
	jit_extension = ir_LOAD_A(ir_ADD_OFFSET(func, offsetof(zend_op_array, reserved[zend_func_info_rid])));

	if (zend_jit_profile_counter_rid) {
		addr = ir_ADD_OFFSET(run_time_cache, zend_jit_profile_counter_rid * sizeof(void*));
	} else {
		addr = run_time_cache;
	}
	ir_STORE(addr, ir_ADD_L(ir_LOAD_L(addr), ir_CONST_LONG(1)));

	addr = ir_ADD_OFFSET(jit_extension, offsetof(zend_jit_op_array_extension, orig_handler));
	ir_IJMP(ir_LOAD_A(addr));

	return 1;
}

static int _zend_jit_hybrid_hot_counter_stub(zend_jit_ctx *jit, uint32_t cost)
{
	ir_ref func, jit_extension, addr, ref, if_overflow;

	func = ir_LOAD_A(jit_EX(func));
	jit_extension = ir_LOAD_A(ir_ADD_OFFSET(func, offsetof(zend_op_array, reserved[zend_func_info_rid])));
	addr = ir_LOAD_A(ir_ADD_OFFSET(jit_extension, offsetof(zend_jit_op_array_hot_extension, counter)));
	ref = ir_SUB_I16(ir_LOAD_I16(addr), ir_CONST_I16(cost));
	ir_STORE(addr, ref);
	if_overflow = ir_IF(ir_LE(ref, ir_CONST_I16(0)));

	ir_IF_TRUE_cold(if_overflow);
	ir_STORE(addr, ir_CONST_I16(ZEND_JIT_COUNTER_INIT));
	ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_hot_func),
		jit_FP(jit),
		jit_IP(jit));
	ir_IJMP(ir_LOAD_A(jit_IP(jit)));

	ir_IF_FALSE(if_overflow);
	ref = ir_SUB_A(jit_IP(jit),
		ir_LOAD_A(ir_ADD_OFFSET(func, offsetof(zend_op_array, opcodes))));
	ref = ir_DIV_A(ref, ir_CONST_ADDR(sizeof(zend_op) / sizeof(void*)));

	addr = ir_ADD_A(ir_ADD_OFFSET(jit_extension, offsetof(zend_jit_op_array_hot_extension, orig_handlers)),
		ref);
	ir_IJMP(ir_LOAD_A(addr));

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
		addr = ir_ADD_A(offset, jit_IP(jit));
	} else {
		addr = ir_ADD_A(offset, ir_LOAD_A(jit_EX(opline)));
	}

	return ir_LOAD_A(addr);
}

static ir_ref zend_jit_orig_opline_handler(zend_jit_ctx *jit)
{
	ir_ref func, jit_extension, offset;

	func = ir_LOAD_A(jit_EX(func));
	jit_extension = ir_LOAD_A(ir_ADD_OFFSET(func, offsetof(zend_op_array, reserved[zend_func_info_rid])));
	offset = ir_LOAD_A(ir_ADD_OFFSET(jit_extension, offsetof(zend_jit_op_array_trace_extension, offset)));
	return _zend_jit_orig_opline_handler(jit, offset);
}

static int _zend_jit_hybrid_trace_counter_stub(zend_jit_ctx *jit, uint32_t cost)
{
	ir_ref func, jit_extension, offset, addr, ref, if_overflow, ret, if_halt;

	func = ir_LOAD_A(jit_EX(func));
	jit_extension = ir_LOAD_A(ir_ADD_OFFSET(func, offsetof(zend_op_array, reserved[zend_func_info_rid])));
	offset = ir_LOAD_A(ir_ADD_OFFSET(jit_extension, offsetof(zend_jit_op_array_trace_extension, offset)));
	addr = ir_LOAD_A(ir_ADD_OFFSET(ir_ADD_A(offset, jit_IP(jit)), offsetof(zend_op_trace_info, counter)));
	ref = ir_SUB_I16(ir_LOAD_I16(addr), ir_CONST_I16(cost));
	ir_STORE(addr, ref);
	if_overflow = ir_IF(ir_LE(ref, ir_CONST_I16(0)));

	ir_IF_TRUE_cold(if_overflow);
	ir_STORE(addr, ir_CONST_I16(ZEND_JIT_COUNTER_INIT));
	ret = ir_CALL_2(IR_I32, ir_CONST_FC_FUNC(zend_jit_trace_hot_root),
		jit_FP(jit),
		jit_IP(jit));
	if_halt = ir_IF(ir_LT(ret, ir_CONST_I32(0)));
	ir_IF_FALSE(if_halt);

	ref = jit_EG(current_execute_data);
	jit_STORE_FP(jit, ir_LOAD_A(ref));
	ref = ir_LOAD_A(jit_EX(opline));
	jit_STORE_IP(jit, ref);
	ir_IJMP(ir_LOAD_A(jit_IP(jit)));

	ir_IF_FALSE(if_overflow);
	ir_IJMP(_zend_jit_orig_opline_handler(jit, offset));

	ir_IF_TRUE(if_halt);
	ir_IJMP(ir_CONST_FC_FUNC(zend_jit_halt_op->handler));

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
		ir_TAILCALL(ir_CONST_FC_FUNC(zend_jit_halt_op->handler));
	} else if (GCC_GLOBAL_REGS) {
		jit_STORE_IP(jit, IR_NULL);
		ir_RETURN(IR_VOID);
	} else {
		ir_RETURN(ir_CONST_I32(-1)); // ZEND_VM_RETURN
	}
	return 1;
}

static int zend_jit_trace_escape_stub(zend_jit_ctx *jit)
{
	if (GCC_GLOBAL_REGS) {
		ir_TAILCALL(ir_LOAD_A(jit_IP(jit)));
	} else {
		ir_RETURN(ir_CONST_I32(1)); // ZEND_VM_ENTER
	}

	return 1;
}

static int zend_jit_trace_exit_stub(zend_jit_ctx *jit)
{
	ir_ref ref, ret, if_zero, addr;

	if (GCC_GLOBAL_REGS) {
		// EX(opline) = opline
		ir_STORE(jit_EX(opline), jit_IP(jit));
	}

	ret = ir_EXITCALL(ir_CONST_FC_FUNC(zend_jit_trace_exit));

	if_zero = ir_IF(ir_EQ(ret, ir_CONST_I32(0)));

	ir_IF_TRUE(if_zero);

	if (GCC_GLOBAL_REGS) {
		ref = jit_EG(current_execute_data);
		jit_STORE_FP(jit, ir_LOAD_A(ref));
		ref = ir_LOAD_A(jit_EX(opline));
		jit_STORE_IP(jit, ref);
		ir_TAILCALL(ir_LOAD_A(jit_IP(jit)));
	} else {
		ir_RETURN(ir_CONST_I32(1)); // ZEND_VM_ENTER
	}

	ir_IF_FALSE(if_zero);

	ir_GUARD(ir_GE(ret, ir_CONST_I32(0)), jit_STUB_ADDR(jit, jit_stub_trace_halt));

	ref = jit_EG(current_execute_data);
	jit_STORE_FP(jit, ir_LOAD_A(ref));

	if (GCC_GLOBAL_REGS) {
		ref = ir_LOAD_A(jit_EX(opline));
		jit_STORE_IP(jit, ref);
	}

	// check for interrupt (try to avoid this ???)
	if (!zend_jit_check_timeout(jit, NULL, NULL)) {
		return 0;
	}

	addr = zend_jit_orig_opline_handler(jit);
	if (GCC_GLOBAL_REGS) {
		ir_TAILCALL(addr);
	} else {
#if defined(IR_TARGET_X86)
		addr = ir_CAST_FC_FUNC(addr);
#endif
		ref = ir_CALL_1(IR_I32, addr, jit_FP(jit));
		ir_GUARD(ir_GE(ref, ir_CONST_I32(0)), jit_STUB_ADDR(jit, jit_stub_trace_halt));
		ir_RETURN(ir_CONST_I32(1)); // ZEND_VM_ENTER
	}

	return 1;
}

static int zend_jit_undefined_offset_stub(zend_jit_ctx *jit)
{
	ir_ref opline = ir_LOAD_A(jit_EX(opline));
	ir_ref ref = ir_LOAD_U32(ir_ADD_OFFSET(opline, offsetof(zend_op, result.var)));
	ir_ref if_const, end1, ref1;

	if (sizeof(void*) == 8) {
		ref = ir_ZEXT_A(ref);
	}
	jit_set_Z_TYPE_INFO_ref(jit, ir_ADD_A(jit_FP(jit), ref), ir_CONST_U32(IS_NULL));

	if_const = ir_IF(ir_EQ(ir_LOAD_U8(ir_ADD_OFFSET(opline, offsetof(zend_op, op2_type))), ir_CONST_U8(IS_CONST)));

	ir_IF_TRUE(if_const);
#if ZEND_USE_ABS_CONST_ADDR
	ref1 = ir_LOAD_A(ir_ADD_OFFSET(opline, offsetof(zend_op, op2.zv)));
#else
	ref = ir_LOAD_U32(ir_ADD_OFFSET(opline, offsetof(zend_op, op2.constant)));
	if (sizeof(void*) == 8) {
		ref = ir_SEXT_A(ref);
	}
	ref1 = ir_ADD_A(ref, opline);
#endif

	end1 = ir_END();

	ir_IF_FALSE(if_const);
	ref = ir_LOAD_U32(ir_ADD_OFFSET(opline, offsetof(zend_op, op2.var)));
	if (sizeof(void*) == 8) {
		ref = ir_ZEXT_A(ref);
	}
	ref = ir_ADD_A(jit_FP(jit), ref);

	ir_MERGE_WITH(end1);
	ref = ir_PHI_2(ref, ref1);

	ref = jit_Z_LVAL_ref(jit, ref);
	ir_CALL_3(IR_VOID, ir_CONST_FUNC(zend_error),
		ir_CONST_U8(E_WARNING),
		ir_CONST_ADDR("Undefined array key " ZEND_LONG_FMT),
		ref);
	ir_RETURN(IR_VOID);

	return 1;
}

static int zend_jit_undefined_key_stub(zend_jit_ctx *jit)
{
	ir_ref opline = ir_LOAD_A(jit_EX(opline));
	ir_ref ref = ir_LOAD_U32(ir_ADD_OFFSET(opline, offsetof(zend_op, result.var)));
	ir_ref if_const, end1, ref1;

	if (sizeof(void*) == 8) {
		ref = ir_ZEXT_A(ref);
	}
	jit_set_Z_TYPE_INFO_ref(jit, ir_ADD_A(jit_FP(jit), ref), ir_CONST_U32(IS_NULL));

	if_const = ir_IF(ir_EQ(ir_LOAD_U8(ir_ADD_OFFSET(opline, offsetof(zend_op, op2_type))), ir_CONST_U8(IS_CONST)));

	ir_IF_TRUE(if_const);
#if ZEND_USE_ABS_CONST_ADDR
	ref1 = ir_LOAD_A(ir_ADD_OFFSET(opline, offsetof(zend_op, op2.zv)));
#else
	ref = ir_LOAD_U32(ir_ADD_OFFSET(opline, offsetof(zend_op, op2.constant)));
	if (sizeof(void*) == 8) {
		ref = ir_SEXT_A(ref);
	}
	ref1 = ir_ADD_A(ref, opline);
#endif

	end1 = ir_END();

	ir_IF_FALSE(if_const);
	ref = ir_LOAD_U32(ir_ADD_OFFSET(opline, offsetof(zend_op, op2.var)));
	if (sizeof(void*) == 8) {
		ref = ir_ZEXT_A(ref);
	}
	ref = ir_ADD_A(jit_FP(jit), ref);

	ir_MERGE_WITH(end1);
	ref = ir_PHI_2(ref, ref1);

	ref = ir_ADD_OFFSET(jit_Z_PTR_ref(jit, ref), offsetof(zend_string, val));
	ir_CALL_3(IR_VOID, ir_CONST_FUNC(zend_error),
		ir_CONST_U8(E_WARNING),
		ir_CONST_ADDR("Undefined array key \"%s\""),
		ref);
	ir_RETURN(IR_VOID);

	return 1;
}

static int zend_jit_cannot_add_element_stub(zend_jit_ctx *jit)
{
	ir_ref opline = ir_LOAD_A(jit_EX(opline));
	ir_ref ref, if_result_used;

	if_result_used = ir_IF(ir_AND_U8(
		ir_LOAD_U8(ir_ADD_OFFSET(opline, offsetof(zend_op, result_type))),
		ir_CONST_U8(IS_TMP_VAR|IS_VAR)));
	ir_IF_TRUE(if_result_used);

	ref = ir_LOAD_U32(ir_ADD_OFFSET(opline, offsetof(zend_op, result.var)));
	if (sizeof(void*) == 8) {
		ref = ir_ZEXT_A(ref);
	}
	jit_set_Z_TYPE_INFO_ref(jit, ir_ADD_A(jit_FP(jit), ref), ir_CONST_U32(IS_UNDEF));
	ir_MERGE_WITH_EMPTY_FALSE(if_result_used);

	ir_CALL_2(IR_VOID, ir_CONST_FUNC(zend_throw_error),
		IR_NULL,
		ir_CONST_ADDR("Cannot add element to the array as the next element is already occupied"));
	ir_RETURN(IR_VOID);

	return 1;
}

static int zend_jit_assign_const_stub(zend_jit_ctx *jit)
{
	ir_ref var = ir_PARAM(IR_ADDR, "var", 1);
	ir_ref val = ir_PARAM(IR_ADDR, "val", 2);

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
	ir_RETURN(IR_VOID);
	return 1;
}

static int zend_jit_assign_tmp_stub(zend_jit_ctx *jit)
{
	ir_ref var = ir_PARAM(IR_ADDR, "var", 1);
	ir_ref val = ir_PARAM(IR_ADDR, "val", 2);

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
	ir_RETURN(IR_VOID);
	return 1;
}

static int zend_jit_assign_var_stub(zend_jit_ctx *jit)
{
	ir_ref var = ir_PARAM(IR_ADDR, "var", 1);
	ir_ref val = ir_PARAM(IR_ADDR, "val", 2);

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
	ir_RETURN(IR_VOID);
	return 1;
}

static int zend_jit_assign_cv_noref_stub(zend_jit_ctx *jit)
{
	ir_ref var = ir_PARAM(IR_ADDR, "var", 1);
	ir_ref val = ir_PARAM(IR_ADDR, "val", 2);

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
	ir_RETURN(IR_VOID);
	return 1;
}

static int zend_jit_new_array_stub(zend_jit_ctx *jit)
{
	ir_ref var = ir_PARAM(IR_ADDR, "var", 1);
	zend_jit_addr var_addr = ZEND_ADDR_REF_ZVAL(var);
	ir_ref ref = ir_CALL(IR_ADDR, ir_CONST_FC_FUNC(_zend_new_array_0));

	jit_set_Z_PTR(jit, var_addr, ref);
	jit_set_Z_TYPE_INFO(jit, var_addr, IS_ARRAY_EX);
	ir_RETURN(ref);
	return 1;
}

static int zend_jit_assign_cv_stub(zend_jit_ctx *jit)
{
	ir_ref var = ir_PARAM(IR_ADDR, "var", 1);
	ir_ref val = ir_PARAM(IR_ADDR, "val", 2);

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
	ir_RETURN(IR_VOID);
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

	jit->ctx.fixed_regset = (1<<ZREG_FP) | (1<<ZREG_IP);
	if (!(flags & IR_FUNCTION)) {
		jit->ctx.flags |= IR_NO_STACK_COMBINE;
		if (zend_jit_vm_kind == ZEND_VM_KIND_CALL) {
			jit->ctx.flags |= IR_FUNCTION;
			/* Stack must be 16 byte aligned */
			/* TODO: select stack size ??? */
#if defined(IR_TARGET_AARCH64) || defined(_WIN64)
			jit->ctx.fixed_stack_frame_size = sizeof(void*) * 15;
#else
			jit->ctx.fixed_stack_frame_size = sizeof(void*) * 7;
#endif
			if (GCC_GLOBAL_REGS) {
				jit->ctx.fixed_save_regset = IR_REGSET_PRESERVED & ~((1<<ZREG_FP) | (1<<ZREG_IP));
			} else {
				jit->ctx.fixed_save_regset = IR_REGSET_PRESERVED;
//#ifdef _WIN64
//				jit->ctx.fixed_save_regset &= 0xffff; // TODO: don't save FP registers ???
//#endif
			}
		} else {
#ifdef ZEND_VM_HYBRID_JIT_RED_ZONE_SIZE
			jit->ctx.fixed_stack_red_zone = ZEND_VM_HYBRID_JIT_RED_ZONE_SIZE;
			if (jit->ctx.fixed_stack_red_zone > 16) {
				jit->ctx.fixed_stack_frame_size = jit->ctx.fixed_stack_red_zone - 16;
				jit->ctx.fixed_call_stack_size = 16;
			}
			jit->ctx.flags |= IR_MERGE_EMPTY_ENTRIES;
#else
			jit->ctx.fixed_stack_red_zone = 0;
			jit->ctx.fixed_stack_frame_size = 16;
			jit->ctx.fixed_call_stack_size = 16;
#endif
#if defined(IR_TARGET_X86) || defined(IR_TARGET_X64)
			jit->ctx.fixed_regset |= (1<<5); /* prevent %rbp (%r5) usage */
#endif
		}
	}

	jit->ctx.snapshot_create = (ir_snapshot_create_t)jit_SNAPSHOT;

	jit->op_array = NULL;
	jit->ssa = NULL;
	jit->name = NULL;
	jit->last_valid_opline = NULL;
	jit->use_last_valid_opline = 0;
	jit->track_last_valid_opline = 0;
	jit->reuse_ip = 0;
	jit->delayed_call_level = 0;
	delayed_call_chain = 0;
	jit->b = -1;
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
	jit->delay_refs = NULL;
	jit->eg_exception_addr = 0;
	zend_hash_init(&jit->addr_hash, 64, NULL, NULL, 0);
	memset(jit->stub_addr, 0, sizeof(jit->stub_addr));

	ir_START();
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
	ctx->flags &= ~IR_USE_FRAME_POINTER; /* don't use FRAME_POINTER even with ALLOCA, TODO: cleanup this ??? */
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

#ifndef _WIN32
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
#endif
		}
		zend_jit_free_ctx(&jit);
	}
	return 1;
}

#define REGISTER_HELPER(n)  \
	ir_disasm_add_symbol(#n, (uint64_t)(uintptr_t)n, sizeof(void*));
#define REGISTER_DATA(n)  \
	ir_disasm_add_symbol(#n, (uint64_t)(uintptr_t)&n, sizeof(n));

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
	}

	REGISTER_DATA(zend_jit_profile_counter);

	REGISTER_HELPER(zend_runtime_jit);
	REGISTER_HELPER(zend_jit_hot_func);
	REGISTER_HELPER(zend_jit_trace_hot_root);
	REGISTER_HELPER(zend_jit_trace_exit);

	REGISTER_HELPER(zend_jit_array_free);
	REGISTER_HELPER(zend_jit_undefined_op_helper);
	REGISTER_HELPER(zend_jit_pre_inc_typed_ref);
	REGISTER_HELPER(zend_jit_pre_dec_typed_ref);
	REGISTER_HELPER(zend_jit_post_inc_typed_ref);
	REGISTER_HELPER(zend_jit_post_dec_typed_ref);
	REGISTER_HELPER(zend_jit_pre_inc);
	REGISTER_HELPER(zend_jit_pre_dec);
	REGISTER_HELPER(zend_jit_add_arrays_helper);
	REGISTER_HELPER(zend_jit_fast_assign_concat_helper);
	REGISTER_HELPER(zend_jit_fast_concat_helper);
	REGISTER_HELPER(zend_jit_fast_concat_tmp_helper);
	REGISTER_HELPER(zend_jit_assign_op_to_typed_ref_tmp);
	REGISTER_HELPER(zend_jit_assign_op_to_typed_ref);
	REGISTER_HELPER(zend_jit_assign_const_to_typed_ref);
	REGISTER_HELPER(zend_jit_assign_tmp_to_typed_ref);
	REGISTER_HELPER(zend_jit_assign_var_to_typed_ref);
	REGISTER_HELPER(zend_jit_assign_cv_to_typed_ref);
	REGISTER_HELPER(zend_jit_check_constant);
	REGISTER_HELPER(zend_jit_get_constant);
	REGISTER_HELPER(zend_jit_int_extend_stack_helper);
	REGISTER_HELPER(zend_jit_extend_stack_helper);
	REGISTER_HELPER(zend_jit_init_func_run_time_cache_helper);
	REGISTER_HELPER(zend_jit_find_func_helper);
	REGISTER_HELPER(zend_jit_find_ns_func_helper);
	REGISTER_HELPER(zend_jit_unref_helper);
	REGISTER_HELPER(zend_jit_invalid_method_call);
	REGISTER_HELPER(zend_jit_invalid_method_call_tmp);
	REGISTER_HELPER(zend_jit_find_method_helper);
	REGISTER_HELPER(zend_jit_find_method_tmp_helper);
	REGISTER_HELPER(zend_jit_push_static_metod_call_frame);
	REGISTER_HELPER(zend_jit_push_static_metod_call_frame_tmp);
	REGISTER_HELPER(zend_jit_free_trampoline_helper);
	REGISTER_HELPER(zend_jit_verify_return_slow);
	REGISTER_HELPER(zend_jit_deprecated_helper);
	REGISTER_HELPER(zend_jit_copy_extra_args_helper);
	REGISTER_HELPER(zend_jit_vm_stack_free_args_helper);
	REGISTER_HELPER(zend_free_extra_named_params);
	REGISTER_HELPER(zend_jit_free_call_frame);
	REGISTER_HELPER(zend_jit_exception_in_interrupt_handler_helper);
	REGISTER_HELPER(zend_jit_verify_arg_slow);
	REGISTER_HELPER(zend_missing_arg_error);
	REGISTER_HELPER(zend_jit_only_vars_by_reference);
	REGISTER_HELPER(zend_jit_leave_func_helper);
	REGISTER_HELPER(zend_jit_leave_nested_func_helper);
	REGISTER_HELPER(zend_jit_leave_top_func_helper);
	REGISTER_HELPER(zend_jit_fetch_global_helper);
	REGISTER_HELPER(zend_jit_hash_index_lookup_rw_no_packed);
	REGISTER_HELPER(zend_jit_hash_index_lookup_rw);
	REGISTER_HELPER(zend_jit_hash_lookup_rw);
	REGISTER_HELPER(zend_jit_symtable_find);
	REGISTER_HELPER(zend_jit_symtable_lookup_w);
	REGISTER_HELPER(zend_jit_symtable_lookup_rw);
	REGISTER_HELPER(zend_jit_fetch_dim_r_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_is_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_isset_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_rw_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_w_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_str_offset_r_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_str_r_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_str_is_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_obj_r_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_obj_is_helper);
	REGISTER_HELPER(zend_jit_invalid_array_access);
	REGISTER_HELPER(zend_jit_zval_array_dup);
	REGISTER_HELPER(zend_jit_prepare_assign_dim_ref);
	REGISTER_HELPER(zend_jit_fetch_dim_obj_w_helper);
	REGISTER_HELPER(zend_jit_fetch_dim_obj_rw_helper);
	REGISTER_HELPER(zend_jit_isset_dim_helper);
	REGISTER_HELPER(zend_jit_assign_dim_helper);
	REGISTER_HELPER(zend_jit_assign_dim_op_helper);
	REGISTER_HELPER(zend_jit_fetch_obj_w_slow);
	REGISTER_HELPER(zend_jit_fetch_obj_r_slow);
	REGISTER_HELPER(zend_jit_fetch_obj_is_slow);
	REGISTER_HELPER(zend_jit_fetch_obj_r_dynamic);
	REGISTER_HELPER(zend_jit_fetch_obj_is_dynamic);
	REGISTER_HELPER(zend_jit_check_array_promotion);
	REGISTER_HELPER(zend_jit_create_typed_ref);
	REGISTER_HELPER(zend_jit_invalid_property_write);
	REGISTER_HELPER(zend_jit_invalid_property_read);
	REGISTER_HELPER(zend_jit_extract_helper);
	REGISTER_HELPER(zend_jit_invalid_property_assign);
	REGISTER_HELPER(zend_jit_assign_to_typed_prop);
	REGISTER_HELPER(zend_jit_assign_obj_helper);
	REGISTER_HELPER(zend_jit_invalid_property_assign_op);
	REGISTER_HELPER(zend_jit_assign_op_to_typed_prop);
	REGISTER_HELPER(zend_jit_assign_obj_op_helper);
	REGISTER_HELPER(zend_jit_invalid_property_incdec);
	REGISTER_HELPER(zend_jit_inc_typed_prop);
	REGISTER_HELPER(zend_jit_dec_typed_prop);
	REGISTER_HELPER(zend_jit_pre_inc_typed_prop);
	REGISTER_HELPER(zend_jit_post_inc_typed_prop);
	REGISTER_HELPER(zend_jit_pre_dec_typed_prop);
	REGISTER_HELPER(zend_jit_post_dec_typed_prop);
	REGISTER_HELPER(zend_jit_pre_inc_obj_helper);
	REGISTER_HELPER(zend_jit_post_inc_obj_helper);
	REGISTER_HELPER(zend_jit_pre_dec_obj_helper);
	REGISTER_HELPER(zend_jit_post_dec_obj_helper);
	REGISTER_HELPER(zend_jit_rope_end);

#ifndef ZTS
	REGISTER_DATA(EG(current_execute_data));
	REGISTER_DATA(EG(exception));
	REGISTER_DATA(EG(opline_before_exception));
	REGISTER_DATA(EG(vm_interrupt));
	REGISTER_DATA(EG(timed_out));
	REGISTER_DATA(EG(uninitialized_zval));
	REGISTER_DATA(EG(zend_constants));
	REGISTER_DATA(EG(jit_trace_num));
	REGISTER_DATA(EG(vm_stack_top));
	REGISTER_DATA(EG(vm_stack_end));
	REGISTER_DATA(EG(exception_op));
	REGISTER_DATA(EG(symbol_table));

	REGISTER_DATA(CG(map_ptr_base));
#endif
}

static int zend_jit_calc_trace_prologue_size(void)
{
	zend_jit_ctx jit_ctx;
	zend_jit_ctx *jit = &jit_ctx;
	void *entry;
	size_t size;

	zend_jit_init_ctx(jit, (zend_jit_vm_kind == ZEND_VM_KIND_CALL) ? 0 : IR_START_BR_TARGET);

	if (!GCC_GLOBAL_REGS) {
		ir_ref ref = ir_PARAM(IR_ADDR, "execute_data", 1);
		jit_STORE_FP(jit, ref);
		jit->ctx.flags |= IR_FASTCALL_FUNC;
	}

	ir_UNREACHABLE();

	entry = zend_jit_ir_compile(&jit->ctx, &size, "JIT$trace_prologue");
	zend_jit_free_ctx(jit);

	if (!entry) {
		return 0;
	}

	zend_jit_trace_prologue_size = size;
	return 1;
}

#if !ZEND_WIN32 && !defined(IR_TARGET_AARCH64)
static uintptr_t zend_jit_hybrid_vm_sp_adj = 0;

typedef struct _Unwind_Context _Unwind_Context;
typedef int (*_Unwind_Trace_Fn)(_Unwind_Context *, void *);
extern int _Unwind_Backtrace(_Unwind_Trace_Fn, void *);
extern uintptr_t _Unwind_GetCFA(_Unwind_Context *);

typedef struct _zend_jit_unwind_arg {
	int cnt;
	uintptr_t cfa[3];
} zend_jit_unwind_arg;

static int zend_jit_unwind_cb(_Unwind_Context *ctx, void *a)
{
	zend_jit_unwind_arg *arg = (zend_jit_unwind_arg*)a;
	arg->cfa[arg->cnt] = _Unwind_GetCFA(ctx);
	arg->cnt++;
	if (arg->cnt == 3) {
		return 5; // _URC_END_OF_STACK
	}
	return 0; // _URC_NO_REASON;
}

static void ZEND_FASTCALL zend_jit_touch_vm_stack_data(void *vm_stack_data)
{
	zend_jit_unwind_arg arg;

	memset(&arg, 0, sizeof(arg));
	_Unwind_Backtrace(zend_jit_unwind_cb, &arg);
	if (arg.cnt == 3) {
		zend_jit_hybrid_vm_sp_adj = arg.cfa[2] - arg.cfa[1];
	}
}

extern void (ZEND_FASTCALL *zend_touch_vm_stack_data)(void *vm_stack_data);

static zend_never_inline void zend_jit_set_sp_adj_vm(void)
{
	void (ZEND_FASTCALL *orig_zend_touch_vm_stack_data)(void *);

	orig_zend_touch_vm_stack_data = zend_touch_vm_stack_data;
	zend_touch_vm_stack_data = zend_jit_touch_vm_stack_data;
	execute_ex(NULL);                                        // set sp_adj[SP_ADJ_VM]
	zend_touch_vm_stack_data = orig_zend_touch_vm_stack_data;
}
#endif

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

#if !ZEND_WIN32 && !defined(IR_TARGET_AARCH64)
	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		zend_jit_set_sp_adj_vm(); // set zend_jit_hybrid_vm_sp_adj
	}
#endif

	if (JIT_G(debug) & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_ASM_STUBS)) {
		zend_jit_setup_disasm();
	}

#ifndef _WIN32
	if (JIT_G(debug) & ZEND_JIT_DEBUG_PERF_DUMP) {
		ir_perf_jitdump_open();
	}

#endif
	zend_long debug = JIT_G(debug);
	if (!(debug & ZEND_JIT_DEBUG_ASM_STUBS)) {
		JIT_G(debug) &= ~(ZEND_JIT_DEBUG_IR_SRC|ZEND_JIT_DEBUG_IR_FINAL|ZEND_JIT_DEBUG_IR_CFG|ZEND_JIT_DEBUG_IR_REGS|
			ZEND_JIT_DEBUG_IR_AFTER_SCCP|ZEND_JIT_DEBUG_IR_AFTER_SCHEDULE|ZEND_JIT_DEBUG_IR_AFTER_REGS);
	}

	if (!zend_jit_calc_trace_prologue_size()) {
	    JIT_G(debug) = debug;
		return FAILURE;
	}
	if (!zend_jit_setup_stubs()) {
	    JIT_G(debug) = debug;
		return FAILURE;
	}
	JIT_G(debug) = debug;

	return SUCCESS;
}

static void zend_jit_shutdown_ir(void)
{
#ifndef _WIN32
	if (JIT_G(debug) & ZEND_JIT_DEBUG_PERF_DUMP) {
		ir_perf_jitdump_close();
	}
	if (JIT_G(debug) & ZEND_JIT_DEBUG_GDB) {
		ir_gdb_unregister_all();
	}
#endif
	if (JIT_G(debug) & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_ASM_STUBS)) {
		ir_disasm_free();
	}
}

/* PHP control flow reconstruction helpers */
static ir_ref jit_IF_ex(zend_jit_ctx *jit, ir_ref condition, ir_ref true_block)
{
	ir_ref ref;

	ZEND_ASSERT(jit->ctx.control);
	ref = ir_IF(condition);
	/* op3 is used as a temporary storage for PHP BB number to reconstruct PHP control flow.
	 *
	 * It's used in jit_IF_TRUE_FALSE_ex() to select IF_TRUE or IF_FALSE instructions
	 * to start target block
	 */
	ir_set_op(&jit->ctx, ref, 3, true_block);
	jit->ctx.control = ref;
	return ref;
}

static void jit_IF_TRUE_FALSE_ex(zend_jit_ctx *jit, ir_ref if_ref, ir_ref true_block)
{
	ZEND_ASSERT(JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE);
	ZEND_ASSERT(if_ref);
	ZEND_ASSERT(jit->ctx.ir_base[if_ref].op == IR_IF);
	ZEND_ASSERT(jit->ctx.ir_base[if_ref].op3);
	if (jit->ctx.ir_base[if_ref].op3 == true_block) {
		ir_IF_TRUE(if_ref);
	} else {
		ir_IF_FALSE(if_ref);
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
					jit_IF_TRUE_FALSE_ex(jit, ref, b);
					ref = ir_LOOP_END(header);
				} else if (jit->ctx.ir_base[ref].op == IR_UNREACHABLE) {
					ir_BEGIN(ref);
					ref = ir_LOOP_END(header);
				} else {
					ZEND_ASSERT(0);
				}
				ir_MERGE_SET_OP(header, i + 1, ref);
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
                                                ir_ref        true_inputs,
                                                ir_ref        false_inputs)
{
	ir_ref true_path = IR_UNUSED, false_path = IR_UNUSED;

	ZEND_ASSERT(JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE);
	if (true_inputs) {
		ZEND_ASSERT(jit->ctx.ir_base[true_inputs].op == IR_END);
		if (!jit->ctx.ir_base[true_inputs].op2) {
			true_path = true_inputs;
		} else {
			ir_MERGE_list(true_inputs);
			true_path = ir_END();
		}
	}
	if (false_inputs) {
		ZEND_ASSERT(jit->ctx.ir_base[false_inputs].op == IR_END);
		if (!jit->ctx.ir_base[false_inputs].op2) {
			false_path = false_inputs;
		} else {
			ir_MERGE_list(false_inputs);
			false_path = ir_END();
		}
	}

	if (true_label == false_label && true_path && false_path) {
		ir_MERGE_2(true_path, false_path);
		_zend_jit_add_predecessor_ref(jit, true_label, jit->b, ir_END());
	} else {
		if (true_path) {
			_zend_jit_add_predecessor_ref(jit, true_label, jit->b, true_path);
		}
		if (false_path) {
			_zend_jit_add_predecessor_ref(jit, false_label, jit->b, false_path);
		}
	}

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
								ir_insn_set_op(phi, k + 2, ir_insn_op(phi, j + 2));
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
		ir_ref list = IR_UNUSED, idx;
		bool first = 1;

		ZEND_HASH_FOREACH_VAL(jumptable, zv) {
			const zend_op *target = ZEND_OFFSET_TO_OPLINE(opline, Z_LVAL_P(zv));
			int b = jit->ssa->cfg.map[target - jit->op_array->opcodes];

			if (b == case_b) {
				if (!first) {
					ir_END_list(list);
				}
				if (HT_IS_PACKED(jumptable)) {
					idx = ir_CONST_LONG(zv - jumptable->arPacked);
				} else {
					idx = ir_CONST_LONG((Bucket*)zv - jumptable->arData);
				}
				ir_CASE_VAL(switch_ref, idx);
				first = 0;
			}
		} ZEND_HASH_FOREACH_END();
		if (default_b == case_b) {
			if (!first) {
				ir_END_list(list);
			}
			if (jit->ctx.ir_base[switch_ref].op3) {
				/* op3 may contain a list of additional "default" path inputs for MATCH */
				ir_ref ref = jit->ctx.ir_base[switch_ref].op3;
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
			ir_CASE_DEFAULT(switch_ref);
		}
		if (list) {
			ir_END_list(list);
			ir_MERGE_list(list);
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

	if (jit->ctx.control) {
		if (n != 0) {
			ZEND_ASSERT(jit->ctx.ir_base[jit->ctx.control].op == IR_ENTRY);
			if (!GCC_GLOBAL_REGS) {
				/* 2 is hardcoded reference to IR_PARAM */
				ZEND_ASSERT(jit->ctx.ir_base[2].op == IR_PARAM);
				ZEND_ASSERT(jit->ctx.ir_base[2].op3 == 1);
				jit_STORE_FP(jit, 2);
			}
		} else {
			ZEND_ASSERT(jit->ctx.ir_base[jit->ctx.control].op == IR_START);
		}
	}

	if (n == 0) {
		/* pass */
		ZEND_ASSERT(jit->ctx.control);
		ZEND_ASSERT(jit->ctx.ir_base[jit->ctx.control].op == IR_START);
		bb_start = jit->ctx.control;
		if (!GCC_GLOBAL_REGS) {
			ir_ref ref = ir_PARAM(IR_ADDR, "execute_data", 1);
			jit_STORE_FP(jit, ref);
			jit->ctx.flags |= IR_FASTCALL_FUNC;
		}
	} else if (n == 1) {
		pred = jit->ssa->cfg.predecessors[bb->predecessor_offset];
		ref = jit->bb_edges[jit->bb_predecessors[b]];
		if (ref == IR_UNUSED) {
			if (!jit->ctx.control) {
				ir_BEGIN(IR_UNUSED); /* unreachable block */
			}
		} else {
			ir_op op = jit->ctx.ir_base[ref].op;

			if (op == IR_IF) {
				if (!jit->ctx.control) {
					jit_IF_TRUE_FALSE_ex(jit, ref, b);
				} else {
					ir_ref entry_path = ir_END();
					jit_IF_TRUE_FALSE_ex(jit, ref, b);
					ir_MERGE_WITH(entry_path);
				}
			} else if (op == IR_SWITCH) {
				zend_jit_case_start(jit, pred, b, ref);
			} else {
				if (!jit->ctx.control) {
					ZEND_ASSERT(op == IR_END || op == IR_UNREACHABLE || op == IR_RETURN);
					ir_BEGIN(ref);
				} else {
					ir_MERGE_WITH(ref);
				}
			}
		}
		bb_start = jit->ctx.control;
	} else {
		int forward_edges_count = 0;
		int back_edges_count = 0;
		ir_ref *pred_refs;
		ir_ref entry_path = IR_UNUSED;
		ALLOCA_FLAG(use_heap);

		if (jit->ctx.control) {
			entry_path = ir_END();
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
						jit_IF_TRUE_FALSE_ex(jit, ref, b);
						pred_refs[i] = ir_END();
					} else if (op == IR_SWITCH) {
						zend_jit_case_start(jit, pred, b, ref);
						pred_refs[i] = ir_END();
					} else {
						ZEND_ASSERT(op == IR_END || op == IR_UNREACHABLE || op == IR_RETURN);
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
			ir_MERGE_N(n, pred_refs);
			jit->ctx.ir_base[jit->ctx.control].op = IR_LOOP_BEGIN;
			bb_start = jit->ctx.control;
			if (entry_path) {
				ir_MERGE_WITH(entry_path);
			}
		} else {
//			ZEND_ASSERT(back_edges_count != 0);
			/* edges from exceptional blocks may be counted as back edges */
			ir_MERGE_N(n, pred_refs);
			bb_start = jit->ctx.control;
			if (entry_path) {
				ir_MERGE_WITH(entry_path);
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
	_zend_jit_add_predecessor_ref(jit, succ, b, ir_END());
	jit->ctx.control = IR_UNUSED;
	jit->b = -1;
	return 1;
}

static int jit_CMP_IP(zend_jit_ctx *jit, ir_op op, const zend_op *next_opline)
{
	ir_ref ref;

#if 1
	if (GCC_GLOBAL_REGS) {
		ref = jit_IP32(jit);
	} else {
		ref = ir_LOAD_U32(jit_EX(opline));
	}
	ref = ir_CMP_OP(op, ref, ir_CONST_U32((uint32_t)(uintptr_t)next_opline));
#else
	if (GCC_GLOBAL_REGS) {
		ref = jit_IP(jit);
	} else {
		ref = ir_LOAD_A(jit_EX(opline));
	}
	ref = ir_CMP_OP(op, ref, ir_CONST_ADDR(next_opline));
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
		_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, ir_END());
		jit->ctx.control = IR_UNUSED;
		jit->b = -1;
		zend_jit_set_last_valid_opline(jit, next_opline);
		return 1;
	}

	ref = jit_IF_ex(jit, jit_CMP_IP(jit, IR_NE, next_opline), target_block);

	_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, ref);
	_zend_jit_add_predecessor_ref(jit, bb->successors[1], jit->b, ref);

	jit->ctx.control = IR_UNUSED;
	jit->b = -1;
	zend_jit_set_last_valid_opline(jit, next_opline);

	return 1;
}

static int zend_jit_set_cond(zend_jit_ctx *jit, const zend_op *next_opline, uint32_t var)
{
	ir_ref ref;

	ref = ir_ADD_U32(ir_ZEXT_U32(jit_CMP_IP(jit, IR_EQ, next_opline)), ir_CONST_U32(IS_FALSE));

	// EX_VAR(var) = ...
	ir_STORE(ir_ADD_OFFSET(jit_FP(jit), var + offsetof(zval, u1.type_info)), ref);

	zend_jit_reset_last_valid_opline(jit);
	return zend_jit_set_ip(jit, next_opline - 1);
}

/* PHP JIT handlers */
static void zend_jit_check_exception(zend_jit_ctx *jit)
{
	ir_GUARD_NOT(ir_LOAD_A(jit_EG_exception(jit)),
		jit_STUB_ADDR(jit, jit_stub_exception_handler));
}

static void zend_jit_check_exception_undef_result(zend_jit_ctx *jit, const zend_op *opline)
{
	ir_GUARD_NOT(ir_LOAD_A(jit_EG_exception(jit)),
		jit_STUB_ADDR(jit,
			(opline->result_type & (IS_TMP_VAR|IS_VAR)) ? jit_stub_exception_handler_undef : jit_stub_exception_handler));
}

static void zend_jit_type_check_undef(zend_jit_ctx  *jit,
                                      ir_ref         type,
                                      uint32_t       var,
                                      const zend_op *opline,
                                      bool           check_exception,
                                      bool           in_cold_path)
{
	ir_ref if_def = ir_IF(type);

	if (!in_cold_path) {
		ir_IF_FALSE_cold(if_def);
	} else {
		ir_IF_FALSE(if_def);
	}
	if (opline) {
		jit_SET_EX_OPLINE(jit, opline);
	}
	ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_undefined_op_helper), ir_CONST_U32(var));
	if (check_exception) {
		zend_jit_check_exception(jit);
	}
	ir_MERGE_WITH_EMPTY_TRUE(if_def);
}

static ir_ref zend_jit_zval_check_undef(zend_jit_ctx  *jit,
                                        ir_ref         ref,
                                        uint32_t       var,
                                        const zend_op *opline,
                                        bool           check_exception)
{
	ir_ref if_def, ref2;

	if_def = ir_IF(jit_Z_TYPE_ref(jit, ref));
	ir_IF_FALSE_cold(if_def);

	if (opline) {
		jit_SET_EX_OPLINE(jit, opline);
	}

	ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_undefined_op_helper), ir_CONST_U32(var));

	if (check_exception) {
		zend_jit_check_exception(jit);
	}

	ref2 = jit_EG(uninitialized_zval);

	ir_MERGE_WITH_EMPTY_TRUE(if_def);

	return ir_PHI_2(ref2, ref);
}

static int zend_jit_entry(zend_jit_ctx *jit, unsigned int label)
{
	ir_ENTRY(label);
	return 1;
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
		ir_CALL(IR_VOID, ir_CONST_FUNC(handler));
	} else {
		ref = jit_FP(jit);
		ref = ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(handler), ref);
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
			ir_TAILCALL(ir_CONST_FUNC(handler));
		} else {
			handler = zend_get_opcode_handler_func(opline);
			ir_CALL(IR_VOID, ir_CONST_FUNC(handler));
			ref = ir_LOAD_A(jit_IP(jit));
			ir_TAILCALL(ref);
		}
	} else {
		handler = opline->handler;
		if (GCC_GLOBAL_REGS) {
			ir_TAILCALL(ir_CONST_FUNC(handler));
		} else {
			ref = jit_FP(jit);
			ir_TAILCALL_1(ir_CONST_FC_FUNC(handler), ref);
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
			_zend_jit_add_predecessor_ref(jit, succ, jit->b, jit->ctx.control);
		}
		jit->ctx.control = IR_UNUSED;
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
		jit_set_Z_LVAL(jit, dst, zend_jit_use_reg(jit, src));
		if (set_type &&
		    (Z_REG(dst) != ZREG_FP ||
		     !JIT_G(current_frame) ||
		     STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(Z_OFFSET(dst))) != IS_LONG)) {
			jit_set_Z_TYPE_INFO(jit, dst, IS_LONG);
		}
	} else if ((info & MAY_BE_ANY) == MAY_BE_DOUBLE) {
		jit_set_Z_DVAL(jit, dst, zend_jit_use_reg(jit, src));
		if (set_type &&
		    (Z_REG(dst) != ZREG_FP ||
		     !JIT_G(current_frame) ||
		     STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(Z_OFFSET(dst))) != IS_DOUBLE)) {
			jit_set_Z_TYPE_INFO(jit, dst, IS_DOUBLE);
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
		zend_jit_def_reg(jit, dst, jit_Z_LVAL(jit, src));
	} else if ((info & MAY_BE_ANY) == MAY_BE_DOUBLE) {
		zend_jit_def_reg(jit, dst, jit_Z_DVAL(jit, src));
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
		jit_set_Z_LVAL(jit, dst, src);
		if (set_type &&
		    (Z_REG(dst) != ZREG_FP ||
		     !JIT_G(current_frame) ||
		     STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(Z_OFFSET(dst))) != IS_LONG)) {
			jit_set_Z_TYPE_INFO(jit, dst, IS_LONG);
		}
	} else if ((info & MAY_BE_ANY) == MAY_BE_DOUBLE) {
		jit_set_Z_DVAL(jit, dst, src);
		if (set_type &&
		    (Z_REG(dst) != ZREG_FP ||
		     !JIT_G(current_frame) ||
		     STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(Z_OFFSET(dst))) != IS_DOUBLE)) {
			jit_set_Z_TYPE_INFO(jit, dst, IS_DOUBLE);
		}
	} else {
		ZEND_UNREACHABLE();
	}
	return 1;
}

static ir_ref zend_jit_deopt_rload(zend_jit_ctx *jit, ir_type type, int32_t reg)
{
	ir_ref ref = jit->ctx.control;
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
	return ir_RLOAD(type, reg);
}

static int zend_jit_store_const_long(zend_jit_ctx *jit, int var, zend_long val)
{
	zend_jit_addr dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, EX_NUM_TO_VAR(var));
	ir_ref src = ir_CONST_LONG(val);

	if (jit->ra && jit->ra[var].ref == IR_NULL) {
		zend_jit_def_reg(jit, ZEND_ADDR_REG(var), src);
	}
	jit_set_Z_LVAL(jit, dst, src);
	jit_set_Z_TYPE_INFO(jit, dst, IS_LONG);
	return 1;
}

static int zend_jit_store_const_double(zend_jit_ctx *jit, int var, double val)
{
	zend_jit_addr dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, EX_NUM_TO_VAR(var));
	ir_ref src = ir_CONST_DOUBLE(val);

	if (jit->ra && jit->ra[var].ref == IR_NULL) {
		zend_jit_def_reg(jit, ZEND_ADDR_REG(var), src);
	}
	jit_set_Z_DVAL(jit, dst, src);
	jit_set_Z_TYPE_INFO(jit, dst, IS_DOUBLE);
	return 1;
}

static int zend_jit_store_type(zend_jit_ctx *jit, int var, uint8_t type)
{
	zend_jit_addr dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, EX_NUM_TO_VAR(var));

	ZEND_ASSERT(type <= IS_DOUBLE);
	jit_set_Z_TYPE_INFO(jit, dst, type);
	return 1;
}

static int zend_jit_store_reg(zend_jit_ctx *jit, uint32_t info, int var, int8_t reg, bool in_mem, bool set_type)
{
	zend_jit_addr src;
	zend_jit_addr dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, EX_NUM_TO_VAR(var));
	ir_type type;

	if ((info & MAY_BE_ANY) == MAY_BE_LONG) {
		type = IR_LONG;
		src = zend_jit_deopt_rload(jit, type, reg);
		if (jit->ra && jit->ra[var].ref == IR_NULL) {
			zend_jit_def_reg(jit, ZEND_ADDR_REG(var), src);
		} else if (!in_mem) {
			jit_set_Z_LVAL(jit, dst, src);
			if (set_type &&
			    (Z_REG(dst) != ZREG_FP ||
			     !JIT_G(current_frame) ||
			     STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(Z_OFFSET(dst))) != IS_LONG)) {
				jit_set_Z_TYPE_INFO(jit, dst, IS_LONG);
			}
		}
	} else if ((info & MAY_BE_ANY) == MAY_BE_DOUBLE) {
		type = IR_DOUBLE;
		src = zend_jit_deopt_rload(jit, type, reg);
		if (jit->ra && jit->ra[var].ref == IR_NULL) {
			zend_jit_def_reg(jit, ZEND_ADDR_REG(var), src);
		} else if (!in_mem) {
			jit_set_Z_DVAL(jit, dst, src);
			if (set_type &&
			    (Z_REG(dst) != ZREG_FP ||
			     !JIT_G(current_frame) ||
			     STACK_MEM_TYPE(JIT_G(current_frame)->stack, EX_VAR_TO_NUM(Z_OFFSET(dst))) != IS_DOUBLE)) {
				jit_set_Z_TYPE_INFO(jit, dst, IS_DOUBLE);
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

	jit_set_Z_TYPE_INFO(jit, dst, type);
	return 1;
}

static int zend_jit_zval_try_addref(zend_jit_ctx *jit, zend_jit_addr var_addr)
{
	ir_ref if_refcounted, end1;

	if_refcounted = jit_if_REFCOUNTED(jit, var_addr);
	ir_IF_FALSE(if_refcounted);
	end1 = ir_END();
	ir_IF_TRUE(if_refcounted);
	jit_GC_ADDREF(jit, jit_Z_PTR(jit, var_addr));
	ir_MERGE_WITH(end1);
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
		/* Invalidate operand type to prevent incorrect destuction by exception_handler_free_op1_op2() */
		zend_jit_addr dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, op.var);
		jit_set_Z_TYPE_INFO(jit, dst, IS_UNDEF);
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
	ir_ref op1_lval_ref = IR_UNUSED;
	ir_ref ref;
	ir_op op;

	if (op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY)-MAY_BE_LONG)) {
		if_long = jit_if_Z_TYPE(jit, op1_addr, IS_LONG);
		ir_IF_TRUE(if_long);
	}
	if (opline->opcode == ZEND_POST_INC || opline->opcode == ZEND_POST_DEC) {
		op1_lval_ref = jit_Z_LVAL(jit, op1_addr);
		jit_set_Z_LVAL(jit, res_addr, op1_lval_ref);
		if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
		}
	}
	if (Z_MODE(op1_def_addr) == IS_MEM_ZVAL
	 && Z_MODE(op1_addr) == IS_REG
	 && !Z_LOAD(op1_addr)
	 && !Z_STORE(op1_addr)) {
		jit_set_Z_TYPE_INFO(jit, op1_def_addr, IS_LONG);
	}
	if (opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_POST_INC) {
		op = may_overflow ? IR_ADD_OV : IR_ADD;
	} else {
		op = may_overflow ? IR_SUB_OV : IR_SUB;
	}
	if (!op1_lval_ref) {
		op1_lval_ref = jit_Z_LVAL(jit, op1_addr);
	}
	ref = ir_BINARY_OP_L(op, op1_lval_ref, ir_CONST_LONG(1));
	if (op1_def_info & MAY_BE_LONG) {
		jit_set_Z_LVAL(jit, op1_def_addr, ref);
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
			SET_STACK_REF(stack, EX_VAR_TO_NUM(opline->op1.var), ir_CONST_DOUBLE(ZEND_LONG_MAX + 1.0));
		} else {
			SET_STACK_REF(stack, EX_VAR_TO_NUM(opline->op1.var), ir_CONST_DOUBLE(ZEND_LONG_MIN - 1.0));
		}
		if (opline->result_type != IS_UNUSED) {
			old_res_info = STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var));
			if (opline->opcode == ZEND_PRE_INC) {
				SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_DOUBLE, 0);
				SET_STACK_REF(stack, EX_VAR_TO_NUM(opline->result.var), ir_CONST_DOUBLE(ZEND_LONG_MAX + 1.0));
			} else if (opline->opcode == ZEND_PRE_DEC) {
				SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_DOUBLE, 0);
				SET_STACK_REF(stack, EX_VAR_TO_NUM(opline->result.var), ir_CONST_DOUBLE(ZEND_LONG_MIN - 1.0));
			} else if (opline->opcode == ZEND_POST_INC) {
				SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_LONG, 0);
				SET_STACK_REF(stack, EX_VAR_TO_NUM(opline->result.var), ir_CONST_LONG(ZEND_LONG_MAX));
			} else if (opline->opcode == ZEND_POST_DEC) {
				SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_LONG, 0);
				SET_STACK_REF(stack, EX_VAR_TO_NUM(opline->result.var), ir_CONST_LONG(ZEND_LONG_MIN));
			}
		}

		exit_point = zend_jit_trace_get_exit_point(opline + 1, 0);
		exit_addr = zend_jit_trace_get_exit_addr(exit_point);
		ir_GUARD_NOT(ir_OVERFLOW(ref), ir_CONST_ADDR(exit_addr));

		if ((opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_PRE_DEC) &&
		    opline->result_type != IS_UNUSED) {
			jit_set_Z_LVAL(jit, res_addr, ref);
			if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
				jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
			}
		}

		SET_STACK_INFO(stack, EX_VAR_TO_NUM(opline->op1.var), old_op1_info);
		if (opline->result_type != IS_UNUSED) {
			SET_STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var), old_res_info);
		}
	} else if (may_overflow) {
		ir_ref if_overflow;
		ir_ref merge_inputs = IR_UNUSED;

		if (((op1_def_info & (MAY_BE_ANY|MAY_BE_GUARD)) == (MAY_BE_DOUBLE|MAY_BE_GUARD))
		 || (opline->result_type != IS_UNUSED && (res_info & (MAY_BE_ANY|MAY_BE_GUARD)) ==  (MAY_BE_DOUBLE|MAY_BE_GUARD))) {
			int32_t exit_point;
			const void *exit_addr;

			exit_point = zend_jit_trace_get_exit_point(opline, 0);
			exit_addr = zend_jit_trace_get_exit_addr(exit_point);
			ir_GUARD(ir_OVERFLOW(ref), ir_CONST_ADDR(exit_addr));
		} else {
			if_overflow = ir_IF(ir_OVERFLOW(ref));
			ir_IF_FALSE(if_overflow);
			if ((opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_PRE_DEC) &&
			    opline->result_type != IS_UNUSED) {
				jit_set_Z_LVAL(jit, res_addr, ref);
				if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
				}
			}
			ir_END_list(merge_inputs);

			/* overflow => cold path */
			ir_IF_TRUE_cold(if_overflow);
		}

		if (opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_POST_INC) {
			if (Z_MODE(op1_def_addr) == IS_REG) {
				jit_set_Z_DVAL(jit, op1_def_addr, ir_CONST_DOUBLE((double)ZEND_LONG_MAX + 1.0));
			} else {
#if SIZEOF_ZEND_LONG == 4
				jit_set_Z_LVAL(jit, op1_def_addr, ir_CONST_LONG(0));
				jit_set_Z_W2(jit, op1_def_addr, ir_CONST_U32(0x41e00000));
#else
				jit_set_Z_LVAL(jit, op1_def_addr, ir_CONST_LONG(0x43e0000000000000));
#endif
				jit_set_Z_TYPE_INFO(jit, op1_def_addr, IS_DOUBLE);
			}
		} else {
			if (Z_MODE(op1_def_addr) == IS_REG) {
				jit_set_Z_DVAL(jit, op1_def_addr, ir_CONST_DOUBLE((double)ZEND_LONG_MIN - 1.0));
			} else {
#if SIZEOF_ZEND_LONG == 4
				jit_set_Z_LVAL(jit, op1_def_addr, ir_CONST_LONG(0x00200000));
				jit_set_Z_W2(jit, op1_def_addr, ir_CONST_U32(0xc1e00000));
#else
				jit_set_Z_LVAL(jit, op1_def_addr, ir_CONST_LONG(0xc3e0000000000000));
#endif
				jit_set_Z_TYPE_INFO(jit, op1_def_addr, IS_DOUBLE);
			}
		}
		if ((opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_PRE_DEC) &&
		    opline->result_type != IS_UNUSED) {
			if (opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_POST_INC) {
				if (Z_MODE(res_addr) == IS_REG) {
					jit_set_Z_DVAL(jit, res_addr, ir_CONST_DOUBLE((double)ZEND_LONG_MAX + 1.0));
				} else {
#if SIZEOF_ZEND_LONG == 4
					jit_set_Z_LVAL(jit, res_addr, ir_CONST_LONG(0));
					jit_set_Z_W2(jit, res_addr, ir_CONST_U32(0x41e00000));
#else
					jit_set_Z_LVAL(jit, res_addr, ir_CONST_LONG(0x43e0000000000000));
#endif
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_DOUBLE);
				}
			} else {
				if (Z_MODE(res_addr) == IS_REG) {
					jit_set_Z_DVAL(jit, res_addr, ir_CONST_DOUBLE((double)ZEND_LONG_MIN - 1.0));
				} else {
#if SIZEOF_ZEND_LONG == 4
					jit_set_Z_LVAL(jit, res_addr, ir_CONST_LONG(0x00200000));
					jit_set_Z_W2(jit, res_addr, ir_CONST_U32(0xc1e00000));
#else
					jit_set_Z_LVAL(jit, res_addr, ir_CONST_LONG(0xc3e0000000000000));
#endif
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_DOUBLE);
				}
			}
		}

		if (merge_inputs) {
			ir_END_list(merge_inputs);
			ir_MERGE_list(merge_inputs);
		}
	} else {
		if ((opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_PRE_DEC) &&
		    opline->result_type != IS_UNUSED) {
			jit_set_Z_LVAL(jit, res_addr, ref);
			if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
				jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
			}
		}
	}
	if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG)) {
		ir_ref merge_inputs = ir_END();

		/* !is_long => cold path */
		ir_IF_FALSE_cold(if_long);
		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) {
			jit_SET_EX_OPLINE(jit, opline);
			if (op1_info & MAY_BE_UNDEF) {
				ir_ref if_def;

				if_def = jit_if_not_Z_TYPE(jit, op1_addr, IS_UNDEF);
				ir_IF_FALSE_cold(if_def);

				// zend_error(E_WARNING, "Undefined variable $%s", ZSTR_VAL(CV_DEF_OF(EX_VAR_TO_NUM(opline->op1.var))));
				ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_undefined_op_helper), ir_CONST_U32(opline->op1.var));

				jit_set_Z_TYPE_INFO(jit, op1_def_addr, IS_NULL);
				ir_MERGE_WITH_EMPTY_TRUE(if_def);

				op1_info |= MAY_BE_NULL;
			}

			ref = jit_ZVAL_ADDR(jit, op1_addr);

			if (op1_info & MAY_BE_REF) {
				ir_ref if_ref, if_typed, func, ref2, arg2;

				if_ref = jit_if_Z_TYPE_ref(jit, ref, ir_CONST_U8(IS_REFERENCE));
				ir_IF_TRUE(if_ref);
				ref2 = jit_Z_PTR_ref(jit, ref);

				if_typed = jit_if_TYPED_REF(jit, ref2);
				ir_IF_TRUE(if_typed);

				if (RETURN_VALUE_USED(opline)) {
					ZEND_ASSERT(Z_MODE(res_addr) != IS_REG);
					arg2 = jit_ZVAL_ADDR(jit, res_addr);
				} else {
					arg2 = IR_NULL;
				}
				if (opline->opcode == ZEND_PRE_INC) {
					func = ir_CONST_FC_FUNC(zend_jit_pre_inc_typed_ref);
				} else if (opline->opcode == ZEND_PRE_DEC) {
					func = ir_CONST_FC_FUNC(zend_jit_pre_dec_typed_ref);
				} else if (opline->opcode == ZEND_POST_INC) {
					func = ir_CONST_FC_FUNC(zend_jit_post_inc_typed_ref);
				} else if (opline->opcode == ZEND_POST_DEC) {
					func = ir_CONST_FC_FUNC(zend_jit_post_dec_typed_ref);
				} else {
					ZEND_UNREACHABLE();
				}

				ir_CALL_2(IR_VOID, func, ref2, arg2);
				zend_jit_check_exception(jit);
				ir_END_list(merge_inputs);

				ir_IF_FALSE(if_typed);
				ref2 = ir_ADD_OFFSET(ref2, offsetof(zend_reference, val));
				ir_MERGE_WITH_EMPTY_FALSE(if_ref);
				ref = ir_PHI_2(ref2, ref);
			}

			if (opline->opcode == ZEND_POST_INC || opline->opcode == ZEND_POST_DEC) {
				jit_ZVAL_COPY(jit,
					res_addr,
					res_use_info,
					ZEND_ADDR_REF_ZVAL(ref), op1_info, 1);
			}
			if (opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_POST_INC) {
				if (opline->opcode == ZEND_PRE_INC && opline->result_type != IS_UNUSED) {
					ir_ref arg2 = jit_ZVAL_ADDR(jit, res_addr);
					ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_pre_inc), ref, arg2);
				} else {
					ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(increment_function), ref);
				}
			} else {
				if (opline->opcode == ZEND_PRE_DEC && opline->result_type != IS_UNUSED) {
					ir_ref arg2 = jit_ZVAL_ADDR(jit, res_addr);
					ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_pre_dec), ref, arg2);
				} else {
					ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(decrement_function), ref);
				}
			}
			if (may_throw) {
				zend_jit_check_exception(jit);
			}
		} else {
			ref = jit_Z_DVAL(jit, op1_addr);
			if (opline->opcode == ZEND_POST_INC || opline->opcode == ZEND_POST_DEC) {
				jit_set_Z_DVAL(jit, res_addr, ref);
				jit_set_Z_TYPE_INFO(jit, res_addr, IS_DOUBLE);
			}
			if (opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_POST_INC) {
				op = IR_ADD;
			} else {
				op = IR_SUB;
			}
			ref = ir_BINARY_OP_D(op, ref, ir_CONST_DOUBLE(1.0));
			jit_set_Z_DVAL(jit, op1_def_addr, ref);
			if ((opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_PRE_DEC) &&
			    opline->result_type != IS_UNUSED) {
				jit_set_Z_DVAL(jit, res_addr, ref);
				jit_set_Z_TYPE_INFO(jit, res_addr, IS_DOUBLE);
			}
		}
		ir_END_list(merge_inputs);
		ir_MERGE_list(merge_inputs);
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
	op1 = jit_Z_LVAL(jit, op1_addr);
	op2 = (same_ops) ? op1 : jit_Z_LVAL(jit, op2_addr);
	ref = ir_BINARY_OP_L(op, op1, op2);

	if (may_overflow) {
		if (res_info & MAY_BE_GUARD) {
			if ((res_info & MAY_BE_ANY) == MAY_BE_LONG) {
				zend_jit_trace_stack *stack = JIT_G(current_frame)->stack;
				uint32_t old_res_info;
				int32_t exit_point;
				const void *exit_addr;

				if (opline->opcode == ZEND_ADD
				 && Z_MODE(op2_addr) == IS_CONST_ZVAL && Z_LVAL_P(Z_ZV(op2_addr)) == 1) {
					old_res_info = STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var));
					SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_DOUBLE, 0);
					SET_STACK_REF(stack, EX_VAR_TO_NUM(opline->result.var), ir_CONST_DOUBLE(ZEND_LONG_MAX + 1.0));
					exit_point = zend_jit_trace_get_exit_point(opline + 1, 0);
					SET_STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var), old_res_info);
				} else if (opline->opcode == ZEND_SUB
				 && Z_MODE(op2_addr) == IS_CONST_ZVAL && Z_LVAL_P(Z_ZV(op2_addr)) == 1) {
					old_res_info = STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var));
					SET_STACK_TYPE(stack, EX_VAR_TO_NUM(opline->result.var), IS_DOUBLE, 0);
					SET_STACK_REF(stack, EX_VAR_TO_NUM(opline->result.var), ir_CONST_DOUBLE(ZEND_LONG_MIN - 1.0));
					exit_point = zend_jit_trace_get_exit_point(opline + 1, 0);
					SET_STACK_INFO(stack, EX_VAR_TO_NUM(opline->result.var), old_res_info);
				} else {
					exit_point = zend_jit_trace_get_exit_point(opline, 0);
				}

				exit_addr = zend_jit_trace_get_exit_addr(exit_point);
				if (!exit_addr) {
					return 0;
				}
				ir_GUARD_NOT(ir_OVERFLOW(ref), ir_CONST_ADDR(exit_addr));
				may_overflow = 0;
			} else if ((res_info & MAY_BE_ANY) == MAY_BE_DOUBLE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				ir_GUARD(ir_OVERFLOW(ref), ir_CONST_ADDR(exit_addr));
			} else {
				ZEND_UNREACHABLE();
			}
		} else {
			if_overflow = ir_IF(ir_OVERFLOW(ref));
			ir_IF_FALSE(if_overflow);
		}
	}

	if ((res_info & MAY_BE_ANY) != MAY_BE_DOUBLE) {
		jit_set_Z_LVAL(jit, res_addr, ref);

		if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
			if (Z_MODE(op1_addr) != IS_MEM_ZVAL || !zend_jit_same_addr(op1_addr, res_addr)) {
				if ((res_use_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF|MAY_BE_GUARD)) != MAY_BE_LONG) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
				}
			}
		}
	}

	if (may_overflow) {
		ir_ref fast_path = IR_UNUSED;

		if ((res_info & MAY_BE_ANY) != MAY_BE_DOUBLE) {
			fast_path = ir_END();
			ir_IF_TRUE_cold(if_overflow);
		}
		if (opcode == ZEND_ADD) {
			if (Z_MODE(op2_addr) == IS_CONST_ZVAL && Z_LVAL_P(Z_ZV(op2_addr)) == 1) {
				if (Z_MODE(res_addr) == IS_REG) {
					jit_set_Z_DVAL(jit, res_addr, ir_CONST_DOUBLE((double)ZEND_LONG_MAX + 1.0));
				} else {
#if SIZEOF_ZEND_LONG == 4
					jit_set_Z_LVAL(jit, res_addr, ir_CONST_LONG(0));
					jit_set_Z_W2(jit, res_addr, ir_CONST_U32(0x41e00000));
#else
					jit_set_Z_LVAL(jit, res_addr, ir_CONST_LONG(0x43e0000000000000));
#endif
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_DOUBLE);
				}
				if ((res_info & MAY_BE_ANY) != MAY_BE_DOUBLE) {
					ir_MERGE_WITH(fast_path);
				}
				return 1;
			}
			op = IR_ADD;
		} else if (opcode == ZEND_SUB) {
			if (Z_MODE(op2_addr) == IS_CONST_ZVAL && Z_LVAL_P(Z_ZV(op2_addr)) == 1) {
				if (Z_MODE(res_addr) == IS_REG) {
					jit_set_Z_DVAL(jit, res_addr, ir_CONST_DOUBLE((double)ZEND_LONG_MIN - 1.0));
				} else {
#if SIZEOF_ZEND_LONG == 4
					jit_set_Z_LVAL(jit, res_addr, ir_CONST_LONG(0x00200000));
					jit_set_Z_W2(jit, res_addr, ir_CONST_U32(0xc1e00000));
#else
					jit_set_Z_LVAL(jit, res_addr, ir_CONST_LONG(0xc3e0000000000000));
#endif
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_DOUBLE);
				}
				if ((res_info & MAY_BE_ANY) != MAY_BE_DOUBLE) {
					ir_MERGE_WITH(fast_path);
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
		op1 = jit_Z_LVAL(jit, op1_addr);
		op2 = (same_ops) ? op1 : jit_Z_LVAL(jit, op2_addr);
#endif
		ref = ir_BINARY_OP_D(op, ir_INT2D(op1), ir_INT2D(op2));
		jit_set_Z_DVAL(jit, res_addr, ref);
		if (Z_MODE(res_addr) != IS_REG) {
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_DOUBLE);
		}
		if ((res_info & MAY_BE_ANY) != MAY_BE_DOUBLE) {
			ir_MERGE_WITH(fast_path);
		}
	}

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
	op1 = jit_Z_LVAL(jit, op1_addr);
	op2 = jit_Z_DVAL(jit, op2_addr);
	ref = ir_BINARY_OP_D(op, ir_INT2D(op1), op2);
	jit_set_Z_DVAL(jit, res_addr, ref);

	if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
		if ((res_use_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF|MAY_BE_GUARD)) != MAY_BE_DOUBLE) {
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_DOUBLE);
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
	op1 = jit_Z_DVAL(jit, op1_addr);
	op2 = jit_Z_LVAL(jit, op2_addr);
	ref = ir_BINARY_OP_D(op, op1, ir_INT2D(op2));
	jit_set_Z_DVAL(jit, res_addr, ref);

	if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
		if (Z_MODE(op1_addr) != IS_MEM_ZVAL || !zend_jit_same_addr(op1_addr, res_addr)) {
			if ((res_use_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF|MAY_BE_GUARD)) != MAY_BE_DOUBLE) {
				jit_set_Z_TYPE_INFO(jit, res_addr, IS_DOUBLE);
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
	op1 = jit_Z_DVAL(jit, op1_addr);
	op2 = (same_ops) ? op1 : jit_Z_DVAL(jit, op2_addr);
	ref = ir_BINARY_OP_D(op, op1, op2);
	jit_set_Z_DVAL(jit, res_addr, ref);

	if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
		if (Z_MODE(op1_addr) != IS_MEM_ZVAL || !zend_jit_same_addr(op1_addr, res_addr)) {
			if ((res_use_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF|MAY_BE_GUARD)) != MAY_BE_DOUBLE) {
				jit_set_Z_TYPE_INFO(jit, res_addr, IS_DOUBLE);
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
	ir_ref slow_inputs = IR_UNUSED;
	bool same_ops = zend_jit_same_addr(op1_addr, op2_addr);
	ir_refs *end_inputs;
	ir_refs *res_inputs;

	ir_refs_init(end_inputs, 6);
	ir_refs_init(res_inputs, 6);

	if (Z_MODE(res_addr) == IS_REG) {
		jit->delay_var = Z_SSA_VAR(res_addr);
		jit->delay_refs = res_inputs;
	}

	if ((res_info & MAY_BE_GUARD) && (res_info & MAY_BE_LONG) && (op1_info & MAY_BE_LONG) && (op2_info & MAY_BE_LONG)) {
		if (op1_info & (MAY_BE_ANY-MAY_BE_LONG)) {
			if_op1_long = jit_if_Z_TYPE(jit, op1_addr, IS_LONG);
			ir_IF_TRUE(if_op1_long);
		}
		if (!same_ops && (op2_info & (MAY_BE_ANY-MAY_BE_LONG))) {
			if_op1_long_op2_long = jit_if_Z_TYPE(jit, op2_addr, IS_LONG);
			ir_IF_TRUE(if_op1_long_op2_long);
		}
		if (!zend_jit_math_long_long(jit, opline, opcode, op1_addr, op2_addr, res_addr, res_info, res_use_info, may_overflow)) {
			return 0;
		}
		ir_refs_add(end_inputs, ir_END());
		if (if_op1_long) {
			ir_IF_FALSE_cold(if_op1_long);
			ir_END_list(slow_inputs);
		}
		if (if_op1_long_op2_long) {
			ir_IF_FALSE_cold(if_op1_long_op2_long);
			ir_END_list(slow_inputs);
		}
	} else if ((op1_info & MAY_BE_LONG) && (op2_info & MAY_BE_LONG) && (res_info & (MAY_BE_LONG|MAY_BE_DOUBLE))) {
		if (op1_info & (MAY_BE_ANY-MAY_BE_LONG)) {
			if_op1_long = jit_if_Z_TYPE(jit, op1_addr, IS_LONG);
			ir_IF_TRUE(if_op1_long);
		}
		if (!same_ops && (op2_info & (MAY_BE_ANY-MAY_BE_LONG))) {
			if_op1_long_op2_long = jit_if_Z_TYPE(jit, op2_addr, IS_LONG);
			ir_IF_FALSE_cold(if_op1_long_op2_long);
			if (op2_info & MAY_BE_DOUBLE) {
				if (op2_info & (MAY_BE_ANY-(MAY_BE_LONG|MAY_BE_DOUBLE))) {
					if_op1_long_op2_double = jit_if_Z_TYPE(jit, op2_addr, IS_DOUBLE);
					ir_IF_FALSE_cold(if_op1_long_op2_double);
					ir_END_list(slow_inputs);
					ir_IF_TRUE(if_op1_long_op2_double);
				}
				if (!zend_jit_math_long_double(jit, opcode, op1_addr, op2_addr, res_addr, res_use_info)) {
					return 0;
				}
				ir_refs_add(end_inputs, ir_END());
			} else {
				ir_END_list(slow_inputs);
			}
			ir_IF_TRUE(if_op1_long_op2_long);
		}
		if (!zend_jit_math_long_long(jit, opline, opcode, op1_addr, op2_addr, res_addr, res_info, res_use_info, may_overflow)) {
			return 0;
		}
		ir_refs_add(end_inputs, ir_END());

		if (if_op1_long) {
			ir_IF_FALSE_cold(if_op1_long);
		}

		if (op1_info & MAY_BE_DOUBLE) {
			if (op1_info & (MAY_BE_ANY-(MAY_BE_LONG|MAY_BE_DOUBLE))) {
				if_op1_double = jit_if_Z_TYPE(jit, op1_addr, IS_DOUBLE);
				ir_IF_FALSE_cold(if_op1_double);
				ir_END_list(slow_inputs);
				ir_IF_TRUE(if_op1_double);
			}
			if (op2_info & MAY_BE_DOUBLE) {
				if (!same_ops && (op2_info & (MAY_BE_ANY-MAY_BE_DOUBLE))) {
					if_op1_double_op2_double = jit_if_Z_TYPE(jit, op2_addr, IS_DOUBLE);
					ir_IF_TRUE(if_op1_double_op2_double);
				}
				if (!zend_jit_math_double_double(jit, opcode, op1_addr, op2_addr, res_addr, res_use_info)) {
					return 0;
				}
				ir_refs_add(end_inputs, ir_END());
				if (if_op1_double_op2_double) {
					ir_IF_FALSE_cold(if_op1_double_op2_double);
				}
			}
			if (!same_ops) {
				if (op2_info & (MAY_BE_ANY-(MAY_BE_LONG|MAY_BE_DOUBLE))) {
					if_op1_double_op2_long = jit_if_Z_TYPE(jit, op2_addr, IS_LONG);
					ir_IF_FALSE_cold(if_op1_double_op2_long);
					ir_END_list(slow_inputs);
					ir_IF_TRUE(if_op1_double_op2_long);
				}
				if (!zend_jit_math_double_long(jit, opcode, op1_addr, op2_addr, res_addr, res_use_info)) {
					return 0;
				}
				ir_refs_add(end_inputs, ir_END());
			} else if (if_op1_double_op2_double) {
				ir_END_list(slow_inputs);
			}
		} else if (if_op1_long) {
			ir_END_list(slow_inputs);
		}
	} else if ((op1_info & MAY_BE_DOUBLE) &&
	           !(op1_info & MAY_BE_LONG) &&
	           (op2_info & (MAY_BE_LONG|MAY_BE_DOUBLE)) &&
	           (res_info & MAY_BE_DOUBLE)) {
		if (op1_info & (MAY_BE_ANY-MAY_BE_DOUBLE)) {
			if_op1_double = jit_if_Z_TYPE(jit, op1_addr, IS_DOUBLE);
			ir_IF_FALSE_cold(if_op1_double);
			ir_END_list(slow_inputs);
			ir_IF_TRUE(if_op1_double);
		}
		if (op2_info & MAY_BE_DOUBLE) {
			if (!same_ops && (op2_info & (MAY_BE_ANY-MAY_BE_DOUBLE))) {
				if_op1_double_op2_double = jit_if_Z_TYPE(jit, op2_addr, IS_DOUBLE);
				ir_IF_TRUE(if_op1_double_op2_double);
			}
			if (!zend_jit_math_double_double(jit, opcode, op1_addr, op2_addr, res_addr, res_use_info)) {
				return 0;
			}
			ir_refs_add(end_inputs, ir_END());
			if (if_op1_double_op2_double) {
				ir_IF_FALSE_cold(if_op1_double_op2_double);
			}
		}
		if (!same_ops && (op2_info & MAY_BE_LONG)) {
			if (op2_info & (MAY_BE_ANY-(MAY_BE_DOUBLE|MAY_BE_LONG))) {
				if_op1_double_op2_long = jit_if_Z_TYPE(jit, op2_addr, IS_LONG);
				ir_IF_FALSE_cold(if_op1_double_op2_long);
				ir_END_list(slow_inputs);
				ir_IF_TRUE(if_op1_double_op2_long);
			}
			if (!zend_jit_math_double_long(jit, opcode, op1_addr, op2_addr, res_addr, res_use_info)) {
				return 0;
			}
			ir_refs_add(end_inputs, ir_END());
		} else if (if_op1_double_op2_double) {
			ir_END_list(slow_inputs);
		}
	} else if ((op2_info & MAY_BE_DOUBLE) &&
	           !(op2_info & MAY_BE_LONG) &&
	           (op1_info & (MAY_BE_LONG|MAY_BE_DOUBLE)) &&
	           (res_info & MAY_BE_DOUBLE)) {
		if (op2_info & (MAY_BE_ANY-MAY_BE_DOUBLE)) {
			if_op2_double = jit_if_Z_TYPE(jit, op2_addr, IS_DOUBLE);
			ir_IF_FALSE_cold(if_op2_double);
			ir_END_list(slow_inputs);
			ir_IF_TRUE(if_op2_double);
		}
		if (op1_info & MAY_BE_DOUBLE) {
			if (!same_ops && (op1_info & (MAY_BE_ANY-MAY_BE_DOUBLE))) {
				if_op1_double_op2_double = jit_if_Z_TYPE(jit, op1_addr, IS_DOUBLE);
				ir_IF_TRUE(if_op1_double_op2_double);
			}
			if (!zend_jit_math_double_double(jit, opcode, op1_addr, op2_addr, res_addr, res_use_info)) {
				return 0;
			}
			ir_refs_add(end_inputs, ir_END());
			if (if_op1_double_op2_double) {
				ir_IF_FALSE_cold(if_op1_double_op2_double);
			}
		}
		if (!same_ops && (op1_info & MAY_BE_LONG)) {
			if (op1_info & (MAY_BE_ANY-(MAY_BE_DOUBLE|MAY_BE_LONG))) {
				if_op1_long_op2_double = jit_if_Z_TYPE(jit, op1_addr, IS_LONG);
				ir_IF_FALSE_cold(if_op1_long_op2_double);
				ir_END_list(slow_inputs);
				ir_IF_TRUE(if_op1_long_op2_double);
			}
			if (!zend_jit_math_long_double(jit, opcode, op1_addr, op2_addr, res_addr, res_use_info)) {
				return 0;
			}
			ir_refs_add(end_inputs, ir_END());
		} else if (if_op1_double_op2_double) {
			ir_END_list(slow_inputs);
		}
	}

	if ((op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) ||
		(op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE)))) {
		ir_ref func, arg1, arg2, arg3;

		if (slow_inputs) {
			ir_MERGE_list(slow_inputs);
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
			arg1 = jit_ZVAL_ADDR(jit, ZEND_ADDR_MEM_ZVAL(ZREG_FP, res_var));
		} else {
			arg1 = jit_ZVAL_ADDR(jit, res_addr);
		}
		arg2 = jit_ZVAL_ADDR(jit, op1_addr);
		arg3 = jit_ZVAL_ADDR(jit, op2_addr);
		jit_SET_EX_OPLINE(jit, opline);
		if (opcode == ZEND_ADD) {
			func = ir_CONST_FC_FUNC(add_function);
		} else if (opcode == ZEND_SUB) {
			func = ir_CONST_FC_FUNC(sub_function);
		} else if (opcode == ZEND_MUL) {
			func = ir_CONST_FC_FUNC(mul_function);
		} else if (opcode == ZEND_DIV) {
			func = ir_CONST_FC_FUNC(div_function);
		} else {
			ZEND_UNREACHABLE();
		}
		ir_CALL_3(IR_VOID, func, arg1, arg2, arg3);

		jit_FREE_OP(jit, op1_type, op1, op1_info, NULL);
		jit_FREE_OP(jit, op2_type, op2, op2_info, NULL);

		if (may_throw) {
			if (opline->opcode == ZEND_ASSIGN_DIM_OP && (opline->op2_type & (IS_VAR|IS_TMP_VAR))) {
				ir_GUARD_NOT(ir_LOAD_A(jit_EG_exception(jit)),
					jit_STUB_ADDR(jit, jit_stub_exception_handler_free_op2));
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
		ir_refs_add(end_inputs, ir_END());
	}

	if (end_inputs->count) {
		ir_MERGE_N(end_inputs->count, end_inputs->refs);
	}

	if (Z_MODE(res_addr) == IS_REG) {
		ZEND_ASSERT(jit->delay_refs == res_inputs);
		ZEND_ASSERT(end_inputs->count == res_inputs->count);
		jit->delay_var = -1;
		jit->delay_refs = NULL;
		if (res_inputs->count == 1) {
			zend_jit_def_reg(jit, res_addr, res_inputs->refs[0]);
		} else {
			ir_ref phi = ir_PHI_N(res_inputs->count, res_inputs->refs);
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
	ir_ref arg1 = jit_Z_PTR(jit, op1_addr);
	ir_ref arg2 = jit_Z_PTR(jit, op2_addr);

	ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_add_arrays_helper), arg1, arg2);
	jit_set_Z_PTR(jit, res_addr, ref);
	jit_set_Z_TYPE_INFO(jit, res_addr, IS_ARRAY_EX);
	jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
	jit_FREE_OP(jit, opline->op2_type, opline->op2, op2_info, opline);
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
	ir_ref if_long1 = IR_UNUSED;
	ir_ref if_long2 = IR_UNUSED;
	bool same_ops = zend_jit_same_addr(op1_addr, op2_addr);
	ir_refs *res_inputs;

	ir_refs_init(res_inputs, 2);

	if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG)) {
		if_long1 = jit_if_Z_TYPE(jit, op1_addr, IS_LONG);
		ir_IF_TRUE(if_long1);
	}
	if (!same_ops && (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG))) {
		if_long2 = jit_if_Z_TYPE(jit, op2_addr, IS_LONG);
		ir_IF_TRUE(if_long2);
	}

	if (opcode == ZEND_SL) {
		if (Z_MODE(op2_addr) == IS_CONST_ZVAL) {
			zend_long op2_lval = Z_LVAL_P(Z_ZV(op2_addr));

			if (UNEXPECTED((zend_ulong)op2_lval >= SIZEOF_ZEND_LONG * 8)) {
				if (EXPECTED(op2_lval > 0)) {
					ref = ir_CONST_LONG(0);
				} else {
					zend_jit_invalidate_var_if_necessary(jit, op1_type, op1_addr, op1);
					zend_jit_invalidate_var_if_necessary(jit, op2_type, op2_addr, op2);
					jit_SET_EX_OPLINE(jit, opline);
					ir_GUARD(IR_FALSE, jit_STUB_ADDR(jit, jit_stub_negative_shift));
					if (Z_MODE(res_addr) == IS_REG) {
						zend_jit_def_reg(jit, res_addr, ir_CONST_LONG(0)); // dead code
					}
				}
			} else {
				ref = ir_SHL_L(jit_Z_LVAL(jit, op1_addr), ir_CONST_LONG(op2_lval));
			}
		} else {
			ref = jit_Z_LVAL(jit, op2_addr);
			if (!op2_range ||
			     op2_range->min < 0 ||
			     op2_range->max >= SIZEOF_ZEND_LONG * 8) {

				ir_ref if_wrong, cold_path, ref2, if_ok;

				if_wrong = ir_IF(ir_UGT(ref, ir_CONST_LONG((SIZEOF_ZEND_LONG * 8) - 1)));
				ir_IF_TRUE_cold(if_wrong);
				if_ok = ir_IF(ir_GE(ref, ir_CONST_LONG(0)));
				ir_IF_FALSE(if_ok);
				jit_SET_EX_OPLINE(jit, opline);
				zend_jit_invalidate_var_if_necessary(jit, op1_type, op1_addr, op1);
				zend_jit_invalidate_var_if_necessary(jit, op2_type, op2_addr, op2);
				ir_IJMP(jit_STUB_ADDR(jit, jit_stub_negative_shift));
				ir_IF_TRUE(if_ok);
				ref2 = ir_CONST_LONG(0);
				cold_path = ir_END();
				ir_IF_FALSE(if_wrong);
				ref = ir_SHL_L(jit_Z_LVAL(jit, op1_addr), ref);
				ir_MERGE_WITH(cold_path);
				ref = ir_PHI_2(ref, ref2);
			} else {
				ref = ir_SHL_L(jit_Z_LVAL(jit, op1_addr), ref);
			}
		}
	} else if (opcode == ZEND_SR) {
		if (Z_MODE(op2_addr) == IS_CONST_ZVAL) {
			zend_long op2_lval = Z_LVAL_P(Z_ZV(op2_addr));

			if (UNEXPECTED((zend_ulong)op2_lval >= SIZEOF_ZEND_LONG * 8)) {
				if (EXPECTED(op2_lval > 0)) {
					ref = ir_SAR_L(
						jit_Z_LVAL(jit, op1_addr),
						ir_CONST_LONG((SIZEOF_ZEND_LONG * 8) - 1));
				} else {
					zend_jit_invalidate_var_if_necessary(jit, op1_type, op1_addr, op1);
					zend_jit_invalidate_var_if_necessary(jit, op2_type, op2_addr, op2);
					jit_SET_EX_OPLINE(jit, opline);
					ir_GUARD(IR_FALSE, jit_STUB_ADDR(jit, jit_stub_negative_shift));
					if (Z_MODE(res_addr) == IS_REG) {
						zend_jit_def_reg(jit, res_addr, ir_CONST_LONG(0)); // dead code
					}
				}
			} else {
				ref = ir_SAR_L(jit_Z_LVAL(jit, op1_addr), ir_CONST_LONG(op2_lval));
			}
		} else {
			ref = jit_Z_LVAL(jit, op2_addr);
			if (!op2_range ||
			     op2_range->min < 0 ||
			     op2_range->max >= SIZEOF_ZEND_LONG * 8) {

				ir_ref if_wrong, cold_path, ref2, if_ok;

				if_wrong = ir_IF(ir_UGT(ref, ir_CONST_LONG((SIZEOF_ZEND_LONG * 8) - 1)));
				ir_IF_TRUE_cold(if_wrong);
				if_ok = ir_IF(ir_GE(ref, ir_CONST_LONG(0)));
				ir_IF_FALSE(if_ok);
				jit_SET_EX_OPLINE(jit, opline);
				zend_jit_invalidate_var_if_necessary(jit, op1_type, op1_addr, op1);
				zend_jit_invalidate_var_if_necessary(jit, op2_type, op2_addr, op2);
				ir_IJMP(jit_STUB_ADDR(jit, jit_stub_negative_shift));
				ir_IF_TRUE(if_ok);
				ref2 = ir_CONST_LONG((SIZEOF_ZEND_LONG * 8) - 1);
				cold_path = ir_END();
				ir_IF_FALSE(if_wrong);
				ir_MERGE_WITH(cold_path);
				ref = ir_PHI_2(ref, ref2);
			}
			ref = ir_SAR_L(jit_Z_LVAL(jit, op1_addr), ref);
		}
	} else if (opcode == ZEND_MOD) {
		if (Z_MODE(op2_addr) == IS_CONST_ZVAL) {
			zend_long op2_lval = Z_LVAL_P(Z_ZV(op2_addr));

			if (op2_lval == 0) {
				zend_jit_invalidate_var_if_necessary(jit, op1_type, op1_addr, op1);
				zend_jit_invalidate_var_if_necessary(jit, op2_type, op2_addr, op2);
				jit_SET_EX_OPLINE(jit, opline);
				ir_GUARD(IR_FALSE,	jit_STUB_ADDR(jit, jit_stub_mod_by_zero));
				if (Z_MODE(res_addr) == IS_REG) {
					zend_jit_def_reg(jit, res_addr, ir_CONST_LONG(0)); // dead code
				}
			} else if (zend_long_is_power_of_two(op2_lval) && op1_range && op1_range->min >= 0) {
				ref = ir_AND_L(jit_Z_LVAL(jit, op1_addr), ir_CONST_LONG(op2_lval - 1));
			} else {
				ref = ir_MOD_L(jit_Z_LVAL(jit, op1_addr), ir_CONST_LONG(op2_lval));
			}
		} else {
			ir_ref zero_path = 0;

			ref = jit_Z_LVAL(jit, op2_addr);
			if (!op2_range || (op2_range->min <= 0 && op2_range->max >= 0)) {
				ir_ref if_ok = ir_IF(ref);
				ir_IF_FALSE(if_ok);
				jit_SET_EX_OPLINE(jit, opline);
				zend_jit_invalidate_var_if_necessary(jit, op1_type, op1_addr, op1);
				zend_jit_invalidate_var_if_necessary(jit, op2_type, op2_addr, op2);
				ir_IJMP(jit_STUB_ADDR(jit, jit_stub_mod_by_zero));
				ir_IF_TRUE(if_ok);
			}

			/* Prevent overflow error/crash if op1 == LONG_MIN and op2 == -1 */
			if (!op2_range || (op2_range->min <= -1 && op2_range->max >= -1)) {
				ir_ref if_minus_one = ir_IF(ir_EQ(ref, ir_CONST_LONG(-1)));
				ir_IF_TRUE_cold(if_minus_one);
				zero_path = ir_END();
				ir_IF_FALSE(if_minus_one);
			}
			ref = ir_MOD_L(jit_Z_LVAL(jit, op1_addr), ref);

			if (zero_path) {
				ir_MERGE_WITH(zero_path);
				ref = ir_PHI_2(ref, ir_CONST_LONG(0));
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
		op1 = jit_Z_LVAL(jit, op1_addr);
		op2 = (same_ops) ? op1 : jit_Z_LVAL(jit, op2_addr);
		ref = ir_BINARY_OP_L(op, op1, op2);
	}

	if (ref) {
		if (Z_MODE(res_addr) == IS_REG
		 && ((op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG))
		  || (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG)))) {
			jit->delay_var = Z_SSA_VAR(res_addr);
			jit->delay_refs = res_inputs;
		}
		jit_set_Z_LVAL(jit, res_addr, ref);
		if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
			if (Z_MODE(op1_addr) != IS_MEM_ZVAL || !zend_jit_same_addr(op1_addr, res_addr)) {
				if ((res_use_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF|MAY_BE_GUARD)) != MAY_BE_LONG) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
				}
			}
		}
	}

	if ((op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG)) ||
		(op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG))) {
		ir_ref fast_path = ir_END();
		ir_ref func, arg1, arg2, arg3;

		if (if_long2 && if_long1) {
			ir_ref ref;
			ir_IF_FALSE_cold(if_long2);
			ref = ir_END();
			ir_IF_FALSE_cold(if_long1);
			ir_MERGE_2(ref, ir_END());
		} else if (if_long1) {
			ir_IF_FALSE_cold(if_long1);
		} else if (if_long2) {
			ir_IF_FALSE_cold(if_long2);
		}

		if (op1_info & MAY_BE_UNDEF) {
			ir_ref if_def;

			if_def = jit_if_not_Z_TYPE(jit, op1_addr, IS_UNDEF);
			ir_IF_FALSE_cold(if_def);

			// zend_error(E_WARNING, "Undefined variable $%s", ZSTR_VAL(CV_DEF_OF(EX_VAR_TO_NUM(opline->op1.var))));
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_undefined_op_helper), ir_CONST_U32(opline->op1.var));

			jit_set_Z_TYPE_INFO(jit, op1_addr, IS_NULL);
			ir_MERGE_WITH_EMPTY_TRUE(if_def);
		}

		if (op2_info & MAY_BE_UNDEF) {
			ir_ref if_def;

			if_def = jit_if_not_Z_TYPE(jit, op2_addr, IS_UNDEF);
			ir_IF_FALSE_cold(if_def);

			// zend_error(E_WARNING, "Undefined variable $%s", ZSTR_VAL(CV_DEF_OF(EX_VAR_TO_NUM(opline->op2.var))));
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_undefined_op_helper), ir_CONST_U32(opline->op2.var));

			jit_set_Z_TYPE_INFO(jit, op2_addr, IS_NULL);
			ir_MERGE_WITH_EMPTY_TRUE(if_def);
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
			arg1 = jit_ZVAL_ADDR(jit, ZEND_ADDR_MEM_ZVAL(ZREG_FP, res_var));
		} else {
			arg1 = jit_ZVAL_ADDR(jit, res_addr);
		}
		arg2 = jit_ZVAL_ADDR(jit, op1_addr);
		arg3 = jit_ZVAL_ADDR(jit, op2_addr);
		jit_SET_EX_OPLINE(jit, opline);
		if (opcode == ZEND_BW_OR) {
			func = ir_CONST_FC_FUNC(bitwise_or_function);
		} else if (opcode == ZEND_BW_AND) {
			func = ir_CONST_FC_FUNC(bitwise_and_function);
		} else if (opcode == ZEND_BW_XOR) {
			func = ir_CONST_FC_FUNC(bitwise_xor_function);
		} else if (opcode == ZEND_SL) {
			func = ir_CONST_FC_FUNC(shift_left_function);
		} else if (opcode == ZEND_SR) {
			func = ir_CONST_FC_FUNC(shift_right_function);
		} else if (opcode == ZEND_MOD) {
			func = ir_CONST_FC_FUNC(mod_function);
		} else {
			ZEND_UNREACHABLE();
		}
		ir_CALL_3(IR_VOID, func, arg1, arg2, arg3);

		if (op1_addr == res_addr && (op2_info & MAY_BE_RCN)) {
			/* compound assignment may decrement "op2" refcount */
			op2_info |= MAY_BE_RC1;
		}

		jit_FREE_OP(jit, op1_type, op1, op1_info, NULL);
		jit_FREE_OP(jit, op2_type, op2, op2_info, NULL);

		if (may_throw) {
			if (opline->opcode == ZEND_ASSIGN_DIM_OP && (opline->op2_type & (IS_VAR|IS_TMP_VAR))) {
				ir_GUARD_NOT(ir_LOAD_A(jit_EG_exception(jit)),
					jit_STUB_ADDR(jit, jit_stub_exception_handler_free_op2));
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

		ir_MERGE_2(fast_path, ir_END());

		if (Z_MODE(res_addr) == IS_REG) {
			ZEND_ASSERT(jit->delay_refs == res_inputs);
			ZEND_ASSERT(res_inputs->count == 2);
			jit->delay_var = -1;
			jit->delay_refs = NULL;
			if (res_inputs->count == 1) {
				zend_jit_def_reg(jit, res_addr, res_inputs->refs[0]);
			} else {
				ir_ref phi = ir_PHI_N(res_inputs->count, res_inputs->refs);
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
			if_op1_string = jit_if_Z_TYPE(jit, op1_addr, IS_STRING);
			ir_IF_TRUE(if_op1_string);
		}
		if (op2_info & ((MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF) - MAY_BE_STRING)) {
			if_op2_string = jit_if_Z_TYPE(jit, op2_addr, IS_STRING);
			ir_IF_TRUE(if_op2_string);
		}
		if (zend_jit_same_addr(op1_addr, res_addr)) {
			ir_ref arg1 = jit_ZVAL_ADDR(jit, res_addr);
			ir_ref arg2 = jit_ZVAL_ADDR(jit, op2_addr);

			ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_fast_assign_concat_helper), arg1, arg2);
			/* concatenation with itself may reduce refcount */
			op2_info |= MAY_BE_RC1;
		} else {
			ir_ref arg1 = jit_ZVAL_ADDR(jit, res_addr);
			ir_ref arg2 = jit_ZVAL_ADDR(jit, op1_addr);
			ir_ref arg3 = jit_ZVAL_ADDR(jit, op2_addr);

			if (op1_type == IS_CV || op1_type == IS_CONST) {
				ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_fast_concat_helper), arg1, arg2, arg3);
			} else {
				ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_fast_concat_tmp_helper), arg1, arg2, arg3);
			}
		}
		/* concatenation with empty string may increase refcount */
		op2_info |= MAY_BE_RCN;
		jit_FREE_OP(jit, op2_type, op2, op2_info, opline);
		if (if_op1_string || if_op2_string) {
			fast_path = ir_END();
		}
	}
	if ((op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF) - MAY_BE_STRING)) ||
	    (op2_info & ((MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF) - MAY_BE_STRING))) {
		if ((op1_info & MAY_BE_STRING) && (op2_info & MAY_BE_STRING)) {
			if (if_op1_string && if_op2_string) {
				ir_IF_FALSE(if_op1_string);
				ir_MERGE_WITH_EMPTY_FALSE(if_op2_string);
			} else if (if_op1_string) {
				ir_IF_FALSE_cold(if_op1_string);
			} else if (if_op2_string) {
				ir_IF_FALSE_cold(if_op2_string);
			}
		}
		ir_ref arg1 = jit_ZVAL_ADDR(jit, res_addr);
		ir_ref arg2 = jit_ZVAL_ADDR(jit, op1_addr);
		ir_ref arg3 = jit_ZVAL_ADDR(jit, op2_addr);

		jit_SET_EX_OPLINE(jit, opline);
		ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(concat_function), arg1, arg2, arg3);
		/* concatenation with empty string may increase refcount */
		op1_info |= MAY_BE_RCN;
		op2_info |= MAY_BE_RCN;
		jit_FREE_OP(jit, op1_type, op1, op1_info, NULL);
		jit_FREE_OP(jit, op2_type, op2, op2_info, NULL);
		if (may_throw) {
			if (opline->opcode == ZEND_ASSIGN_DIM_OP && (opline->op2_type & (IS_VAR|IS_TMP_VAR))) {
				ir_GUARD_NOT(ir_LOAD_A(jit_EG_exception(jit)),
					jit_STUB_ADDR(jit, jit_stub_exception_handler_free_op2));
			} else if (Z_MODE(res_addr) == IS_MEM_ZVAL && Z_REG(res_addr) == ZREG_RX) {
				zend_jit_check_exception_undef_result(jit, opline);
			} else {
				zend_jit_check_exception(jit);
			}
		}
		if ((op1_info & MAY_BE_STRING) && (op2_info & MAY_BE_STRING)) {
			ir_MERGE_WITH(fast_path);
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

		ref = jit_ZVAL_ADDR(jit, op1_addr);
		if_op1_ref = jit_if_Z_TYPE_ref(jit, ref, ir_CONST_U8(IS_REFERENCE));
		ir_IF_FALSE(if_op1_ref);
		op1_noref_path = ir_END();
		ir_IF_TRUE(if_op1_ref);
		ref2 = jit_Z_PTR_ref(jit, ref);

		if_op1_typed = jit_if_TYPED_REF(jit, ref2);
		ir_IF_TRUE_cold(if_op1_typed);

		arg2 = jit_ZVAL_ADDR(jit, op2_addr);
		jit_SET_EX_OPLINE(jit, opline);
		if ((opline->op2_type & (IS_TMP_VAR|IS_VAR))
		 && (op2_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
			ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_assign_op_to_typed_ref_tmp),
				ref2, arg2, ir_CONST_FC_FUNC(binary_op));
		} else {
			ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_assign_op_to_typed_ref),
				ref2, arg2, ir_CONST_FC_FUNC(binary_op));
		}
		zend_jit_check_exception(jit);
		slow_path = ir_END();

		ir_IF_FALSE(if_op1_typed);
		ref2 = ir_ADD_OFFSET(ref2, offsetof(zend_reference, val));

		ir_MERGE_WITH(op1_noref_path);
		ref = ir_PHI_2(ref2, ref);
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
		ir_MERGE_WITH(slow_path);
	}

	return result;
}

static ir_ref jit_ZVAL_DEREF_ref(zend_jit_ctx *jit, ir_ref ref)
{
	ir_ref if_ref, ref2;

	if_ref = ir_IF(ir_EQ(jit_Z_TYPE_ref(jit, ref), ir_CONST_U8(IS_REFERENCE)));
	ir_IF_TRUE(if_ref);
	ref2 = ir_ADD_OFFSET(jit_Z_PTR_ref(jit, ref), offsetof(zend_reference, val));
	ir_MERGE_WITH_EMPTY_FALSE(if_ref);
	return ir_PHI_2(ref2, ref);
}

static zend_jit_addr jit_ZVAL_DEREF(zend_jit_ctx *jit, zend_jit_addr addr)
{
	ir_ref ref = jit_ZVAL_ADDR(jit, addr);
	ref = jit_ZVAL_DEREF_ref(jit, ref);
	return ZEND_ADDR_REF_ZVAL(ref);
}

static ir_ref jit_ZVAL_INDIRECT_DEREF_ref(zend_jit_ctx *jit, ir_ref ref)
{
	ir_ref if_ref, ref2;

	if_ref = ir_IF(ir_EQ(jit_Z_TYPE_ref(jit, ref), ir_CONST_U8(IS_INDIRECT)));
	ir_IF_TRUE(if_ref);
	ref2 = jit_Z_PTR_ref(jit, ref);
	ir_MERGE_WITH_EMPTY_FALSE(if_ref);
	return ir_PHI_2(ref2, ref);
}

static zend_jit_addr jit_ZVAL_INDIRECT_DEREF(zend_jit_ctx *jit, zend_jit_addr addr)
{
	ir_ref ref = jit_ZVAL_ADDR(jit, addr);
	ref = jit_ZVAL_INDIRECT_DEREF_ref(jit, ref);
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
	ir_ref end_inputs = IR_UNUSED;

	if (Z_MODE(val_addr) == IS_CONST_ZVAL) {
		zval *zv = Z_ZV(val_addr);

		if (!res_addr) {
			jit_ZVAL_COPY_CONST(jit,
				var_addr,
				var_info, var_def_info,
				zv, 1);
		} else {
			jit_ZVAL_COPY_CONST(jit,
				var_addr,
				var_info, var_def_info,
				zv, 1);
			jit_ZVAL_COPY_CONST(jit,
				res_addr,
				-1, var_def_info,
				zv, 1);
		}
	} else {
		if (val_info & MAY_BE_UNDEF) {
			ir_ref if_def, ret;

			if_def = jit_if_not_Z_TYPE(jit, val_addr, IS_UNDEF);
			ir_IF_FALSE_cold(if_def);

			jit_set_Z_TYPE_INFO(jit, var_addr, IS_NULL);
			if (res_addr) {
				jit_set_Z_TYPE_INFO(jit, res_addr, IS_NULL);
			}
			jit_SET_EX_OPLINE(jit, opline);

			ZEND_ASSERT(Z_MODE(val_addr) == IS_MEM_ZVAL);
			// zend_error(E_WARNING, "Undefined variable $%s", ZSTR_VAL(CV_DEF_OF(EX_VAR_TO_NUM(opline->op1.var))));
			ret = ir_CALL_1(IR_I32, ir_CONST_FC_FUNC(zend_jit_undefined_op_helper), ir_CONST_U32(Z_OFFSET(val_addr)));

			if (check_exception) {
				ir_GUARD(ret, jit_STUB_ADDR(jit, jit_stub_exception_handler_undef));
			}

			ir_END_list(end_inputs);
			ir_IF_TRUE(if_def);
		}
		if (val_info & MAY_BE_REF) {
			if (val_type == IS_CV) {
				ir_ref ref = jit_ZVAL_ADDR(jit, val_addr);
				ref = jit_ZVAL_DEREF_ref(jit, ref);
				val_addr = ZEND_ADDR_REF_ZVAL(ref);
			} else {
				ir_ref ref, type, if_ref, ref2, refcount, if_not_zero;

				ref = jit_ZVAL_ADDR(jit, val_addr);
				type = jit_Z_TYPE_ref(jit, ref);
				if_ref = ir_IF(ir_EQ(type, ir_CONST_U8(IS_REFERENCE)));

				ir_IF_TRUE_cold(if_ref);
				ref = jit_Z_PTR_ref(jit, ref);
				ref2 = ir_ADD_OFFSET(ref, offsetof(zend_reference, val));
				if (!res_addr) {
					jit_ZVAL_COPY(jit,
						var_addr,
						var_info,
						ZEND_ADDR_REF_ZVAL(ref2), val_info, 1);
				} else {
					jit_ZVAL_COPY_2(jit,
						res_addr,
						var_addr,
						var_info,
						ZEND_ADDR_REF_ZVAL(ref2), val_info, 2);
				}

				refcount = jit_GC_DELREF(jit, ref);
				if_not_zero = ir_IF(refcount);
				ir_IF_FALSE(if_not_zero);
				// TODO: instead of dtor() call and ADDREF above, we may call efree() and move addref at "true" path ???
				// This is related to GH-10168 (keep this before GH-10168 is completely closed)
				// jit_EFREE(jit, ref, sizeof(zend_reference), NULL, NULL);
				jit_ZVAL_DTOR(jit, ref, val_info, opline);
				ir_END_list(end_inputs);
				ir_IF_TRUE(if_not_zero);
				ir_END_list(end_inputs);

				ir_IF_FALSE(if_ref);
			}
		}

		if (!res_addr) {
			jit_ZVAL_COPY(jit,
				var_addr,
				var_info,
				val_addr, val_info, val_type == IS_CV);
		} else {
			jit_ZVAL_COPY_2(jit,
				res_addr,
				var_addr,
				var_info,
				val_addr, val_info, val_type == IS_CV ? 2 : 1);
		}
	}

	if (end_inputs) {
		ir_END_list(end_inputs);
		ir_MERGE_list(end_inputs);
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

			jit_guard_not_Z_TYPE(jit, val_addr, IS_UNDEF, exit_addr);
		} else {
			ir_ref if_def;

			ZEND_ASSERT(Z_MODE(val_addr) == IS_MEM_ZVAL && Z_REG(val_addr) == ZREG_FP);
			if_def = ir_IF(jit_Z_TYPE(jit, val_addr));
			ir_IF_FALSE_cold(if_def);
			jit_SET_EX_OPLINE(jit, opline);
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_undefined_op_helper), ir_CONST_U32(Z_OFFSET(val_addr)));

			ir_CALL_2(IR_VOID, jit_STUB_FUNC_ADDR(jit, jit_stub_assign_const, IR_CONST_FASTCALL_FUNC),
				jit_ZVAL_ADDR(jit, var_addr),
				jit_EG(uninitialized_zval));

			undef_path = ir_END();
			ir_IF_TRUE(if_def);
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
		jit_SET_EX_OPLINE(jit, opline);
	}

	ir_CALL_2(IR_VOID, jit_STUB_FUNC_ADDR(jit, func, IR_CONST_FASTCALL_FUNC),
		jit_ZVAL_ADDR(jit, var_addr),
		jit_ZVAL_ADDR(jit, val_addr));

	if (undef_path) {
		ir_MERGE_WITH(undef_path);
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
	ir_ref simple_inputs = IR_UNUSED;
	bool done = 0;
	zend_jit_addr real_res_addr = 0;
	ir_refs *end_inputs;
	ir_refs *res_inputs;

	ir_refs_init(end_inputs, 6);
	ir_refs_init(res_inputs, 6);

	if (Z_MODE(val_addr) == IS_REG && jit->ra[Z_SSA_VAR(val_addr)].ref == IR_NULL) {
		/* Force load */
		zend_jit_use_reg(jit, val_addr);
	}

	if (Z_MODE(var_addr) == IS_REG) {
		jit->delay_var = Z_SSA_VAR(var_addr);
		jit->delay_refs = res_inputs;
		if (Z_MODE(res_addr) == IS_REG) {
			real_res_addr = res_addr;
			res_addr = 0;
		}
	} else if (Z_MODE(res_addr) == IS_REG) {
		jit->delay_var = Z_SSA_VAR(res_addr);
		jit->delay_refs = res_inputs;
	}

	if ((var_info & MAY_BE_REF) || ref_addr) {
		ir_ref ref = 0, if_ref = 0, ref2, arg2, ret, if_typed, non_ref_path;
		uintptr_t func;

		if (!ref_addr) {
			ref = jit_ZVAL_ADDR(jit, var_use_addr);
			if_ref = jit_if_Z_TYPE_ref(jit, ref, ir_CONST_U8(IS_REFERENCE));
			ir_IF_TRUE(if_ref);
			ref2 = jit_Z_PTR_ref(jit, ref);
		} else {
			ref2 = jit_ZVAL_ADDR(jit, ref_addr);
		}
		if_typed = jit_if_TYPED_REF(jit, ref2);
		ir_IF_TRUE_cold(if_typed);
		jit_SET_EX_OPLINE(jit, opline);
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
			arg2 = jit_ZVAL_ADDR(jit, real_addr);
		} else {
			arg2 = jit_ZVAL_ADDR(jit, val_addr);
		}
		ret = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(func), ref2, arg2);
		if (res_addr) {
			jit_ZVAL_COPY(jit,
				res_addr,
				-1,
				ZEND_ADDR_REF_ZVAL(ret), -1, 1);
		}
		if (check_exception) {
			zend_jit_check_exception(jit);
		}
		ir_refs_add(end_inputs, ir_END());

		if (!ref_addr) {
			ir_IF_FALSE(if_ref);
			non_ref_path = ir_END();
			ir_IF_FALSE(if_typed);
			ref2 = ir_ADD_OFFSET(ref2, offsetof(zend_reference, val));
			ir_MERGE_WITH(non_ref_path);
			ref = ir_PHI_2(ref2, ref);
			var_addr = var_use_addr = ZEND_ADDR_REF_ZVAL(ref);
		} else {
			ir_IF_FALSE(if_typed);
		}
	}

	if (var_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
		ir_ref ref, counter, if_not_zero;

		if (var_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
			if_refcounted = jit_if_REFCOUNTED(jit, var_use_addr);
			ir_IF_FALSE(if_refcounted);
			ir_END_list(simple_inputs);
			ir_IF_TRUE_cold(if_refcounted);
		} else if (RC_MAY_BE_1(var_info)) {
			done = 1;
		}
		ref = jit_Z_PTR(jit, var_use_addr);
		if (RC_MAY_BE_1(var_info)) {
			if (!zend_jit_simple_assign(jit, opline, var_addr, var_info, var_def_info, val_type, val_addr, val_info, res_addr, 0)) {
				return 0;
			}
			counter = jit_GC_DELREF(jit, ref);

			if_not_zero = ir_IF(counter);
			ir_IF_FALSE(if_not_zero);
			jit_ZVAL_DTOR(jit, ref, var_info, opline);
			if (check_exception) {
				zend_jit_check_exception(jit);
			}
			ir_refs_add(end_inputs, ir_END());
			ir_IF_TRUE(if_not_zero);
			if (RC_MAY_BE_N(var_info) && (var_info & (MAY_BE_ARRAY|MAY_BE_OBJECT)) != 0) {
				ir_ref if_may_leak = jit_if_GC_MAY_NOT_LEAK(jit, ref);
				ir_IF_FALSE(if_may_leak);
				ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(gc_possible_root), ref);

				if (Z_MODE(var_addr) == IS_REG || Z_MODE(res_addr) == IS_REG) {
					ZEND_ASSERT(jit->delay_refs == res_inputs);
					ZEND_ASSERT(res_inputs->count > 0);
					ir_refs_add(res_inputs, res_inputs->refs[res_inputs->count - 1]);
				}
				if (check_exception && (val_info & MAY_BE_UNDEF)) {
					zend_jit_check_exception(jit);
				}
				ir_refs_add(end_inputs, ir_END());
				ir_IF_TRUE(if_may_leak);
			}
			if (Z_MODE(var_addr) == IS_REG || Z_MODE(res_addr) == IS_REG) {
				ZEND_ASSERT(jit->delay_refs == res_inputs);
				ZEND_ASSERT(res_inputs->count > 0);
				ir_refs_add(res_inputs, res_inputs->refs[res_inputs->count - 1]);
			}
			if (check_exception && (val_info & MAY_BE_UNDEF)) {
				zend_jit_check_exception(jit);
			}
			ir_refs_add(end_inputs, ir_END());
		} else /* if (RC_MAY_BE_N(var_info)) */ {
			jit_GC_DELREF(jit, ref);
			if (var_info & (MAY_BE_ARRAY|MAY_BE_OBJECT)) {
				ir_ref if_may_leak = jit_if_GC_MAY_NOT_LEAK(jit, ref);
				ir_IF_FALSE(if_may_leak);
				ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(gc_possible_root), ref);
				ir_END_list(simple_inputs);
				ir_IF_TRUE(if_may_leak);
			}
			ir_END_list(simple_inputs);
		}
	}

	if (simple_inputs) {
		ir_MERGE_list(simple_inputs);
	}

	if (!done) {
		if (!zend_jit_simple_assign(jit, opline, var_addr, var_info, var_def_info, val_type, val_addr, val_info, res_addr, check_exception)) {
			return 0;
		}
		if (end_inputs->count) {
			ir_refs_add(end_inputs, ir_END());
		}
	}

	if (end_inputs->count) {
		ir_MERGE_N(end_inputs->count, end_inputs->refs);
	}

	if (Z_MODE(var_addr) == IS_REG || Z_MODE(res_addr) == IS_REG) {
		ir_ref phi;

		ZEND_ASSERT(jit->delay_refs == res_inputs);
		ZEND_ASSERT(end_inputs->count == res_inputs->count || (end_inputs->count == 0 && res_inputs->count == 1));
		jit->delay_var = -1;
		jit->delay_refs = NULL;
		if (res_inputs->count == 1) {
			phi = res_inputs->refs[0];
		} else {
			phi = ir_PHI_N(res_inputs->count, res_inputs->refs);
		}
		if (Z_MODE(var_addr) == IS_REG) {
			zend_jit_def_reg(jit, var_addr, phi);
			if (real_res_addr) {
				if (var_def_info & MAY_BE_LONG) {
					jit_set_Z_LVAL(jit, real_res_addr, jit_Z_LVAL(jit, var_addr));
				} else {
					jit_set_Z_DVAL(jit, real_res_addr, jit_Z_DVAL(jit, var_addr));
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

static ir_op zend_jit_cmp_op(const zend_op *opline)
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
		case ZEND_IS_NOT_IDENTICAL:
			op = IR_NE;
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
	ir_ref ref;
	bool result;

	if (zend_jit_is_constant_cmp_long_long(opline, op1_range, op1_addr, op2_range, op2_addr, &result)) {
		if (!smart_branch_opcode ||
		    smart_branch_opcode == ZEND_JMPZ_EX ||
		    smart_branch_opcode == ZEND_JMPNZ_EX) {
			jit_set_Z_TYPE_INFO(jit, res_addr, result ? IS_TRUE : IS_FALSE);
		}
		if (smart_branch_opcode && !exit_addr) {
			if (smart_branch_opcode == ZEND_JMPZ ||
			    smart_branch_opcode == ZEND_JMPZ_EX) {
				jit_IF_ex(jit, IR_TRUE, result ? target_label2 : target_label);
			} else if (smart_branch_opcode == ZEND_JMPNZ ||
			           smart_branch_opcode == ZEND_JMPNZ_EX) {
				jit_IF_ex(jit, IR_TRUE, result ? target_label : target_label2);
			} else {
				ZEND_UNREACHABLE();
			}
		}
		return 1;
	}

	ref = ir_CMP_OP(zend_jit_cmp_op(opline), jit_Z_LVAL(jit, op1_addr), jit_Z_LVAL(jit, op2_addr));

	if (!smart_branch_opcode || smart_branch_opcode == ZEND_JMPNZ_EX || smart_branch_opcode == ZEND_JMPZ_EX) {
		jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
			ir_ADD_U32(ir_ZEXT_U32(ref), ir_CONST_U32(IS_FALSE)));
	}
	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) {
			if (opline->opcode != ZEND_IS_NOT_IDENTICAL) {
				ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
			} else {
				ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
			}
		} else {
			if (opline->opcode != ZEND_IS_NOT_IDENTICAL) {
				ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
			} else {
				ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
			}
		}
	} else if (smart_branch_opcode) {
		jit_IF_ex(jit, ref,
			(smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ? target_label2 : target_label);
	}

	return 1;
}

static int zend_jit_cmp_long_double(zend_jit_ctx *jit, const zend_op *opline, zend_jit_addr op1_addr, zend_jit_addr op2_addr, zend_jit_addr res_addr, zend_uchar smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr)
{
	ir_ref ref = ir_CMP_OP(zend_jit_cmp_op(opline), ir_INT2D(jit_Z_LVAL(jit, op1_addr)), jit_Z_DVAL(jit, op2_addr));

	if (!smart_branch_opcode || smart_branch_opcode == ZEND_JMPNZ_EX || smart_branch_opcode == ZEND_JMPZ_EX) {
		jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
			ir_ADD_U32(ir_ZEXT_U32(ref), ir_CONST_U32(IS_FALSE)));
	}
	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) {
			ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
		} else {
			ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
		}
	} else if (smart_branch_opcode) {
		jit_IF_ex(jit, ref,
			(smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ? target_label2 : target_label);
	}
	return 1;
}

static int zend_jit_cmp_double_long(zend_jit_ctx *jit, const zend_op *opline, zend_jit_addr op1_addr, zend_jit_addr op2_addr, zend_jit_addr res_addr, zend_uchar smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr)
{
	ir_ref ref = ir_CMP_OP(zend_jit_cmp_op(opline), jit_Z_DVAL(jit, op1_addr), ir_INT2D(jit_Z_LVAL(jit, op2_addr)));

	if (!smart_branch_opcode || smart_branch_opcode == ZEND_JMPNZ_EX || smart_branch_opcode == ZEND_JMPZ_EX) {
		jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
			ir_ADD_U32(ir_ZEXT_U32(ref), ir_CONST_U32(IS_FALSE)));
	}
	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) {
			ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
		} else {
			ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
		}
	} else if (smart_branch_opcode) {
		jit_IF_ex(jit, ref,
			(smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ? target_label2 : target_label);
	}
	return 1;
}

static int zend_jit_cmp_double_double(zend_jit_ctx *jit, const zend_op *opline, zend_jit_addr op1_addr, zend_jit_addr op2_addr, zend_jit_addr res_addr, zend_uchar smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr)
{
	ir_ref ref = ir_CMP_OP(zend_jit_cmp_op(opline), jit_Z_DVAL(jit, op1_addr), jit_Z_DVAL(jit, op2_addr));

	if (!smart_branch_opcode || smart_branch_opcode == ZEND_JMPNZ_EX || smart_branch_opcode == ZEND_JMPZ_EX) {
		jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
			ir_ADD_U32(ir_ZEXT_U32(ref), ir_CONST_U32(IS_FALSE)));
	}
	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) {
			if (opline->opcode != ZEND_IS_NOT_IDENTICAL) {
				ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
			} else {
				ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
			}
		} else {
			if (opline->opcode != ZEND_IS_NOT_IDENTICAL) {
				ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
			} else {
				ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
			}
		}
	} else if (smart_branch_opcode) {
		jit_IF_ex(jit, ref,
			(smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) ? target_label2 : target_label);
	}
	return 1;
}

static int zend_jit_cmp_slow(zend_jit_ctx *jit, ir_ref ref, const zend_op *opline, zend_jit_addr res_addr, zend_uchar smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr)
{
	ref = ir_CMP_OP(zend_jit_cmp_op(opline), ref, ir_CONST_I32(0));

	if (!smart_branch_opcode || smart_branch_opcode == ZEND_JMPNZ_EX || smart_branch_opcode == ZEND_JMPZ_EX) {
		jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
			ir_ADD_U32(ir_ZEXT_U32(ref), ir_CONST_U32(IS_FALSE)));
	}
	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) {
			ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
		} else {
			ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
		}
	} else if (smart_branch_opcode) {
		jit_IF_ex(jit, ref,
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
	ir_ref slow_inputs = IR_UNUSED;
	bool same_ops = zend_jit_same_addr(op1_addr, op2_addr);
	bool has_slow =
		(op1_info & (MAY_BE_LONG|MAY_BE_DOUBLE)) &&
		(op2_info & (MAY_BE_LONG|MAY_BE_DOUBLE)) &&
		((op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) ||
		 (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))));
	ir_refs *end_inputs;

	ir_refs_init(end_inputs, 8);

	if ((op1_info & MAY_BE_LONG) && (op2_info & MAY_BE_LONG)) {
		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG)) {
			if_op1_long = jit_if_Z_TYPE(jit, op1_addr, IS_LONG);
			ir_IF_TRUE(if_op1_long);
		}
		if (!same_ops && (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG))) {
			if_op1_long_op2_long = jit_if_Z_TYPE(jit, op2_addr, IS_LONG);
			ir_IF_FALSE_cold(if_op1_long_op2_long);
			if (op2_info & MAY_BE_DOUBLE) {
				if (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) {
					if_op1_long_op2_double = jit_if_Z_TYPE(jit, op2_addr, IS_DOUBLE);
					ir_IF_FALSE_cold(if_op1_long_op2_double);
					ir_END_list(slow_inputs);
					ir_IF_TRUE(if_op1_long_op2_double);
				}
				if (!zend_jit_cmp_long_double(jit, opline, op1_addr, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
					return 0;
				}
				ir_refs_add(end_inputs, (smart_branch_opcode && !exit_addr) ? jit->ctx.control : ir_END());
			} else {
				ir_END_list(slow_inputs);
			}
			ir_IF_TRUE(if_op1_long_op2_long);
		}
		if (!zend_jit_cmp_long_long(jit, opline, op1_range, op1_addr, op2_range, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr, skip_comparison)) {
			return 0;
		}
		ir_refs_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
			jit->ctx.control : ir_END());

		if (if_op1_long) {
			ir_IF_FALSE_cold(if_op1_long);
		}
		if (op1_info & MAY_BE_DOUBLE) {
			if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) {
				if_op1_double = jit_if_Z_TYPE(jit, op1_addr, IS_DOUBLE);
				ir_IF_FALSE_cold(if_op1_double);
				ir_END_list(slow_inputs);
				ir_IF_TRUE(if_op1_double);
			}
			if (op2_info & MAY_BE_DOUBLE) {
				if (!same_ops && (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_DOUBLE))) {
					if_op1_double_op2_double = jit_if_Z_TYPE(jit, op2_addr, IS_DOUBLE);
					ir_IF_TRUE(if_op1_double_op2_double);
				}
				if (!zend_jit_cmp_double_double(jit, opline, op1_addr, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
					return 0;
				}
				ir_refs_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
					jit->ctx.control : ir_END());
				if (if_op1_double_op2_double) {
					ir_IF_FALSE_cold(if_op1_double_op2_double);
				}
			}
			if (!same_ops) {
				if (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) {
					if_op1_double_op2_long = jit_if_Z_TYPE(jit, op2_addr, IS_LONG);
					ir_IF_FALSE_cold(if_op1_double_op2_long);
					ir_END_list(slow_inputs);
					ir_IF_TRUE(if_op1_double_op2_long);
				}
				if (!zend_jit_cmp_double_long(jit, opline, op1_addr, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
					return 0;
				}
				ir_refs_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
					jit->ctx.control : ir_END());
			} else if (if_op1_double_op2_double) {
				ir_END_list(slow_inputs);
			}
		} else if (if_op1_long) {
			ir_END_list(slow_inputs);
		}
	} else if ((op1_info & MAY_BE_DOUBLE) &&
	           !(op1_info & MAY_BE_LONG) &&
	           (op2_info & (MAY_BE_LONG|MAY_BE_DOUBLE))) {
		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_DOUBLE)) {
			if_op1_double = jit_if_Z_TYPE(jit, op1_addr, IS_DOUBLE);
			ir_IF_FALSE_cold(if_op1_double);
			ir_END_list(slow_inputs);
			ir_IF_TRUE(if_op1_double);
		}
		if (op2_info & MAY_BE_DOUBLE) {
			if (!same_ops && (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_DOUBLE))) {
				if_op1_double_op2_double = jit_if_Z_TYPE(jit, op2_addr, IS_DOUBLE);
				ir_IF_TRUE(if_op1_double_op2_double);
			}
			if (!zend_jit_cmp_double_double(jit, opline, op1_addr, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
				return 0;
			}
			ir_refs_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
				jit->ctx.control : ir_END());
			if (if_op1_double_op2_double) {
				ir_IF_FALSE_cold(if_op1_double_op2_double);
			}
		}
		if (!same_ops && (op2_info & MAY_BE_LONG)) {
			if (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_DOUBLE|MAY_BE_LONG))) {
				if_op1_double_op2_long = jit_if_Z_TYPE(jit, op2_addr, IS_LONG);
				ir_IF_FALSE_cold(if_op1_double_op2_long);
				ir_END_list(slow_inputs);
				ir_IF_TRUE(if_op1_double_op2_long);
			}
			if (!zend_jit_cmp_double_long(jit, opline, op1_addr, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
				return 0;
			}
			ir_refs_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
				jit->ctx.control : ir_END());
		} else if (if_op1_double_op2_double) {
			ir_END_list(slow_inputs);
		}
	} else if ((op2_info & MAY_BE_DOUBLE) &&
	           !(op2_info & MAY_BE_LONG) &&
	           (op1_info & (MAY_BE_LONG|MAY_BE_DOUBLE))) {
		if (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_DOUBLE)) {
			if_op2_double = jit_if_Z_TYPE(jit, op2_addr, IS_DOUBLE);
			ir_IF_FALSE_cold(if_op2_double);
			ir_END_list(slow_inputs);
			ir_IF_TRUE(if_op2_double);
		}
		if (op1_info & MAY_BE_DOUBLE) {
			if (!same_ops && (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_DOUBLE))) {
				if_op1_double_op2_double = jit_if_Z_TYPE(jit, op1_addr, IS_DOUBLE);
				ir_IF_TRUE(if_op1_double_op2_double);
			}
			if (!zend_jit_cmp_double_double(jit, opline, op1_addr, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
				return 0;
			}
			ir_refs_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
				jit->ctx.control : ir_END());
			if (if_op1_double_op2_double) {
				ir_IF_FALSE_cold(if_op1_double_op2_double);
			}
		}
		if (!same_ops && (op1_info & MAY_BE_LONG)) {
			if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_DOUBLE|MAY_BE_LONG))) {
				if_op1_long_op2_double = jit_if_Z_TYPE(jit, op1_addr, IS_LONG);
				ir_IF_FALSE_cold(if_op1_long_op2_double);
				ir_END_list(slow_inputs);
				ir_IF_TRUE(if_op1_long_op2_double);
			}
			if (!zend_jit_cmp_long_double(jit, opline, op1_addr, op2_addr, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
				return 0;
			}
			ir_refs_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
				jit->ctx.control : ir_END());
		} else if (if_op1_double_op2_double) {
			ir_END_list(slow_inputs);
		}
	}

	if (has_slow ||
	    (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE))) ||
	    (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_DOUBLE)))) {
	    ir_ref op1, op2, ref;

		if (slow_inputs) {
			ir_MERGE_list(slow_inputs);
		}
		jit_SET_EX_OPLINE(jit, opline);

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

		op1 = jit_ZVAL_ADDR(jit, op1_addr);
		if (opline->op1_type == IS_CV && (op1_info & MAY_BE_UNDEF)) {
			op1 = zend_jit_zval_check_undef(jit, op1, opline->op1.var, NULL, 0);
		}
		op2 = jit_ZVAL_ADDR(jit, op2_addr);
		if (opline->op2_type == IS_CV && (op2_info & MAY_BE_UNDEF)) {
			op2 = zend_jit_zval_check_undef(jit, op2, opline->op2.var, NULL, 0);
		}
		ref = ir_CALL_2(IR_I32, ir_CONST_FC_FUNC(zend_compare), op1, op2);
		if (opline->opcode != ZEND_CASE) {
			jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, NULL);
		}
		jit_FREE_OP(jit, opline->op2_type, opline->op2, op2_info, NULL);
		if (may_throw) {
			zend_jit_check_exception_undef_result(jit, opline);
		}

		if (!zend_jit_cmp_slow(jit, ref, opline, res_addr, smart_branch_opcode, target_label, target_label2, exit_addr)) {
			return 0;
		}

		ir_refs_add(end_inputs, (smart_branch_opcode && !exit_addr) ?
			jit->ctx.control : ir_END());
	}

	if (end_inputs->count) {
		uint32_t n = end_inputs->count;

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
				ir_ref merge_inputs = IR_UNUSED;

				while (n) {
					n--;
					ir_IF_TRUE(end_inputs->refs[n]);
					ir_END_list(merge_inputs);
					ir_IF_FALSE(end_inputs->refs[n]);
					ir_END_list(merge_inputs);
				}
				ir_MERGE_list(merge_inputs);
				_zend_jit_add_predecessor_ref(jit, label, jit->b, ir_END());
			} else if (n == 1) {
				ref = end_inputs->refs[0];
				_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, ref);
				_zend_jit_add_predecessor_ref(jit, bb->successors[1], jit->b, ref);
			} else {
				ir_ref true_inputs = IR_UNUSED, false_inputs = IR_UNUSED;

				while (n) {
					n--;
					ir_IF_TRUE(end_inputs->refs[n]);
					ir_END_list(true_inputs);
					ir_IF_FALSE(end_inputs->refs[n]);
					ir_END_list(false_inputs);
				}
				ir_MERGE_list(true_inputs);
				_zend_jit_add_predecessor_ref(jit, label, jit->b, ir_END());
				ir_MERGE_list(false_inputs);
				_zend_jit_add_predecessor_ref(jit, label2, jit->b, ir_END());
			}
			jit->ctx.control = IR_UNUSED;
			jit->b = -1;
		} else {
			ir_MERGE_N(n, end_inputs->refs);
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
		ir_ref op1 = jit_ZVAL_ADDR(jit, op1_addr);
		op1 = zend_jit_zval_check_undef(jit, op1, opline->op1.var, NULL, 0);
		op1_info |= MAY_BE_NULL;
		op1_addr = ZEND_ADDR_REF_ZVAL(op1);
	}
	if (opline->op2_type == IS_CV && (op2_info & MAY_BE_UNDEF)) {
		ir_ref op2 = jit_ZVAL_ADDR(jit, op2_addr);
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
			jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
		}
		jit_FREE_OP(jit, opline->op2_type, opline->op2, op2_info, opline);
		if (!smart_branch_opcode
		 || smart_branch_opcode == ZEND_JMPZ_EX
		 || smart_branch_opcode == ZEND_JMPNZ_EX) {
			jit_set_Z_TYPE_INFO(jit, res_addr, opline->opcode != ZEND_IS_NOT_IDENTICAL ? IS_TRUE : IS_FALSE);
		}
		if (may_throw) {
			zend_jit_check_exception(jit);
		}
		if (exit_addr) {
			if (smart_branch_opcode == ZEND_JMPNZ || smart_branch_opcode == ZEND_JMPNZ_EX) {
				jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
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
			_zend_jit_add_predecessor_ref(jit, label, jit->b, ir_END());
			jit->ctx.control = IR_UNUSED;
			jit->b = -1;
		}
		return 1;
	} else if (always_false) {
		if (opline->opcode != ZEND_CASE_STRICT) {
			jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
		}
		jit_FREE_OP(jit, opline->op2_type, opline->op2, op2_info, opline);
		if (!smart_branch_opcode
		 || smart_branch_opcode == ZEND_JMPZ_EX
		 || smart_branch_opcode == ZEND_JMPNZ_EX) {
			jit_set_Z_TYPE_INFO(jit, res_addr, opline->opcode != ZEND_IS_NOT_IDENTICAL ? IS_FALSE : IS_TRUE);
		}
		if (may_throw) {
			zend_jit_check_exception(jit);
		}
		if (exit_addr) {
			if (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) {
				jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
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
			_zend_jit_add_predecessor_ref(jit, label, jit->b, ir_END());
			jit->ctx.control = IR_UNUSED;
			jit->b = -1;
		}
		return 1;
	}

	if ((opline->op1_type & (IS_CV|IS_VAR)) && (op1_info & MAY_BE_REF)) {
		ref = jit_ZVAL_ADDR(jit, op1_addr);
		ref = jit_ZVAL_DEREF_ref(jit, ref);
		op1_addr = ZEND_ADDR_REF_ZVAL(ref);
	}
	if ((opline->op2_type & (IS_CV|IS_VAR)) && (op2_info & MAY_BE_REF)) {
		ref = jit_ZVAL_ADDR(jit, op2_addr);
		ref = jit_ZVAL_DEREF_ref(jit, ref);
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

			ref = ir_EQ(jit_Z_TYPE(jit, op2_addr), ir_CONST_U8(Z_TYPE_P(val)));
		} else if (Z_MODE(op2_addr) == IS_CONST_ZVAL && Z_TYPE_P(Z_ZV(op2_addr)) <= IS_TRUE) {
			zval *val = Z_ZV(op2_addr);

			ref = ir_EQ(jit_Z_TYPE(jit, op1_addr), ir_CONST_U8(Z_TYPE_P(val)));
		} else {
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

			ref = ir_CALL_2(IR_BOOL, ir_CONST_FC_FUNC(zend_is_identical),
				jit_ZVAL_ADDR(jit, op1_addr),
				jit_ZVAL_ADDR(jit, op2_addr));
		}

		if (!smart_branch_opcode || smart_branch_opcode == ZEND_JMPNZ_EX || smart_branch_opcode == ZEND_JMPZ_EX) {
			if (opline->opcode == ZEND_IS_NOT_IDENTICAL) {
				jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
					ir_SUB_U32(ir_CONST_U32(IS_TRUE), ir_ZEXT_U32(ref)));
			} else {
				jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
					ir_ADD_U32(ir_ZEXT_U32(ref), ir_CONST_U32(IS_FALSE)));
			}
		}
		if (opline->opcode != ZEND_CASE_STRICT) {
			jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, NULL);
		}
		jit_FREE_OP(jit, opline->op2_type, opline->op2, op2_info, NULL);
		if (may_throw) {
			zend_jit_check_exception_undef_result(jit, opline);
		}
		if (exit_addr) {
			if (smart_branch_opcode == ZEND_JMPZ || smart_branch_opcode == ZEND_JMPZ_EX) {
				ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
			} else {
				ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
			}
		} else if (smart_branch_opcode) {
			if (opline->opcode == ZEND_IS_NOT_IDENTICAL) {
				/* swap labels */
				uint32_t tmp = target_label;
				target_label = target_label2;
				target_label2 = tmp;
			}
			jit_IF_ex(jit, ref,
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

		ref = jit->ctx.control;
		if (bb->successors_count == 2 && bb->successors[0] == bb->successors[1]) {
			ir_IF_TRUE(ref);
			ir_MERGE_WITH_EMPTY_FALSE(ref);
			_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, ir_END());
		} else {
			ZEND_ASSERT(bb->successors_count == 2);
			_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, ref);
			_zend_jit_add_predecessor_ref(jit, bb->successors[1], jit->b, ref);
		}
		jit->ctx.control = IR_UNUSED;
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
	ir_ref ref, end_inputs = IR_UNUSED, true_inputs = IR_UNUSED, false_inputs = IR_UNUSED;
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
		ref = jit_ZVAL_ADDR(jit, op1_addr);
		ref = jit_ZVAL_DEREF_ref(jit, ref);
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
				ref = jit_ZVAL_ADDR(jit, op1_addr);
				zend_jit_zval_check_undef(jit, ref, opline->op1.var, opline, 0);
			}
			always_false = 1;
		}
	}

	if (always_true) {
		if (set_bool) {
			jit_set_Z_TYPE_INFO(jit, res_addr, set_bool_not ? IS_FALSE : IS_TRUE);
		}
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
		if (may_throw) {
			zend_jit_check_exception(jit);
		}
		if (true_label != (uint32_t)-1) {
			ZEND_ASSERT(exit_addr == NULL);
			_zend_jit_add_predecessor_ref(jit, true_label, jit->b, ir_END());
			jit->ctx.control = IR_UNUSED;
			jit->b = -1;
		}
		return 1;
	} else if (always_false) {
		if (set_bool) {
			jit_set_Z_TYPE_INFO(jit, res_addr, set_bool_not ? IS_TRUE : IS_FALSE);
		}
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
		if (may_throw) {
			zend_jit_check_exception(jit);
		}
		if (false_label != (uint32_t)-1) {
			ZEND_ASSERT(exit_addr == NULL);
			_zend_jit_add_predecessor_ref(jit, false_label, jit->b, ir_END());
			jit->ctx.control = IR_UNUSED;
			jit->b = -1;
		}
		return 1;
	}

	if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE)) {
		type = jit_Z_TYPE(jit, op1_addr);
		if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_FALSE)) {
			ir_ref if_type = ir_IF(ir_LT(type, ir_CONST_U8(IS_TRUE)));

			ir_IF_TRUE_cold(if_type);

			if (op1_info & MAY_BE_UNDEF) {
				zend_jit_type_check_undef(jit,
					type,
					opline->op1.var,
					opline, 1, 0);
			}
			if (set_bool) {
				jit_set_Z_TYPE_INFO(jit, res_addr, set_bool_not ? IS_TRUE : IS_FALSE);
			}
			if (exit_addr) {
				if (branch_opcode == ZEND_JMPNZ) {
					ir_END_list(end_inputs);
				} else {
					jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
				}
			} else if (false_label != (uint32_t)-1) {
				ir_END_list(false_inputs);
			} else {
				ir_END_list(end_inputs);
			}
			ir_IF_FALSE(if_type);
		}

		if (op1_info & MAY_BE_TRUE) {
			ir_ref if_type = IR_UNUSED;

			if (op1_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE))) {
				if_type = ir_IF(ir_EQ(type, ir_CONST_U8(IS_TRUE)));

				ir_IF_TRUE(if_type);
			}
			if (set_bool) {
				jit_set_Z_TYPE_INFO(jit, res_addr, set_bool_not ? IS_FALSE : IS_TRUE);
			}
			if (exit_addr) {
				if (branch_opcode == ZEND_JMPZ) {
					ir_END_list(end_inputs);
				} else {
					jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
				}
			} else if (true_label != (uint32_t)-1) {
				ir_END_list(true_inputs);
			} else {
				ir_END_list(end_inputs);
			}
			if (if_type) {
				ir_IF_FALSE(if_type);
			}
		}
	}

	if (op1_info & MAY_BE_LONG) {
		ir_ref if_long = IR_UNUSED;
		ir_ref ref;

		if (op1_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG))) {
			if (!type) {
				type = jit_Z_TYPE(jit, op1_addr);
			}
			if_long = ir_IF(ir_EQ(type, ir_CONST_U8(IS_LONG)));
			ir_IF_TRUE(if_long);
		}
		ref = jit_Z_LVAL(jit, op1_addr);
		if (branch_opcode == ZEND_BOOL || branch_opcode == ZEND_BOOL_NOT) {
			ref = ir_NE(ref, ir_CONST_LONG(0));
			if (set_bool_not) {
				jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
					ir_SUB_U32(ir_CONST_U32(IS_TRUE), ir_ZEXT_U32(ref)));
			} else {
				jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
					ir_ADD_U32(ir_ZEXT_U32(ref), ir_CONST_U32(IS_FALSE)));
			}
			ir_END_list(end_inputs);
		} else if (exit_addr) {
			if (set_bool) {
				jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
					ir_ADD_U32(ir_ZEXT_U32(ir_NE(ref, ir_CONST_LONG(0))), ir_CONST_U32(IS_FALSE)));
			}
			if (branch_opcode == ZEND_JMPZ || branch_opcode == ZEND_JMPZ_EX) {
				ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
			} else {
				ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
			}
			ir_END_list(end_inputs);
		} else {
			ir_ref if_val = ir_IF(ref);
			ir_IF_TRUE(if_val);
			if (set_bool) {
				jit_set_Z_TYPE_INFO(jit, res_addr, set_bool_not ? IS_FALSE : IS_TRUE);
			}
			ir_END_list(true_inputs);
			ir_IF_FALSE(if_val);
			if (set_bool) {
				jit_set_Z_TYPE_INFO(jit, res_addr, set_bool_not ? IS_TRUE : IS_FALSE);
			}
			ir_END_list(false_inputs);
		}
		if (if_long) {
			ir_IF_FALSE(if_long);
		}
	}

	if (op1_info & MAY_BE_DOUBLE) {
		ir_ref if_double = IR_UNUSED;
		ir_ref ref;

		if (op1_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE))) {
			if (!type) {
				type = jit_Z_TYPE(jit, op1_addr);
			}
			if_double = ir_IF(ir_EQ(type, ir_CONST_U8(IS_DOUBLE)));
			ir_IF_TRUE(if_double);
		}
		ref = ir_NE(jit_Z_DVAL(jit, op1_addr), ir_CONST_DOUBLE(0.0));
		if (branch_opcode == ZEND_BOOL || branch_opcode == ZEND_BOOL_NOT) {
			if (set_bool_not) {
				jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
					ir_SUB_U32(ir_CONST_U32(IS_TRUE), ir_ZEXT_U32(ref)));
			} else {
				jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
					ir_ADD_U32(ir_ZEXT_U32(ref), ir_CONST_U32(IS_FALSE)));
			}
			ir_END_list(end_inputs);
		} else if (exit_addr) {
		    if (set_bool) {
				jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
					ir_ADD_U32(ir_ZEXT_U32(ref), ir_CONST_U32(IS_FALSE)));
		    }
			if (branch_opcode == ZEND_JMPZ || branch_opcode == ZEND_JMPZ_EX) {
				ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
			} else {
				ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
			}
			ir_END_list(end_inputs);
		} else {
			ir_ref if_val = ir_IF(ref);
			ir_IF_TRUE(if_val);
			if (set_bool) {
				jit_set_Z_TYPE_INFO(jit, res_addr, set_bool_not ? IS_FALSE : IS_TRUE);
			}
			ir_END_list(true_inputs);
			ir_IF_FALSE(if_val);
			if (set_bool) {
				jit_set_Z_TYPE_INFO(jit, res_addr, set_bool_not ? IS_TRUE : IS_FALSE);
			}
			ir_END_list(false_inputs);
		}
		if (if_double) {
			ir_IF_FALSE(if_double);
		}
	}

	if (op1_info & (MAY_BE_ANY - (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE))) {
		jit_SET_EX_OPLINE(jit, opline);
		ref = ir_CALL_1(IR_BOOL, ir_CONST_FC_FUNC(zend_is_true), jit_ZVAL_ADDR(jit, op1_addr));
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, NULL);
		if (may_throw) {
			zend_jit_check_exception_undef_result(jit, opline);
		}
		if (branch_opcode == ZEND_BOOL || branch_opcode == ZEND_BOOL_NOT) {
			if (set_bool_not) {
				jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
					ir_SUB_U32(ir_CONST_U32(IS_TRUE), ir_ZEXT_U32(ref)));
			} else {
				jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
					ir_ADD_U32(ir_ZEXT_U32(ref), ir_CONST_U32(IS_FALSE)));
			}
			if (end_inputs) {
				ir_END_list(end_inputs);
			}
		} else if (exit_addr) {
			if (set_bool) {
				jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
					ir_ADD_U32(ir_ZEXT_U32(ref), ir_CONST_U32(IS_FALSE)));
			}
			if (branch_opcode == ZEND_JMPZ || branch_opcode == ZEND_JMPZ_EX) {
				ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
			} else {
				ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
			}
			if (end_inputs) {
				ir_END_list(end_inputs);
			}
		} else {
			ir_ref if_val = ir_IF(ref);
			ir_IF_TRUE(if_val);
			if (set_bool) {
				jit_set_Z_TYPE_INFO(jit, res_addr, set_bool_not ? IS_FALSE : IS_TRUE);
			}
			ir_END_list(true_inputs);
			ir_IF_FALSE(if_val);
			if (set_bool) {
				jit_set_Z_TYPE_INFO(jit, res_addr, set_bool_not ? IS_TRUE : IS_FALSE);
			}
			ir_END_list(false_inputs);
		}
	}

	if (branch_opcode == ZEND_BOOL || branch_opcode == ZEND_BOOL_NOT || exit_addr) {
		if (end_inputs) {
			ir_MERGE_list(end_inputs);
		}
	} else {
		_zend_jit_merge_smart_branch_inputs(jit, true_label, false_label, true_inputs, false_inputs);
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
	ir_ref end_inputs = IR_UNUSED, true_inputs = IR_UNUSED, false_inputs = IR_UNUSED;

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
	ref = ir_LOAD_A(ir_ADD_OFFSET(ir_LOAD_A(jit_EX(run_time_cache)), opline->extended_value));

	if_set = ir_IF(ref);

	ir_IF_FALSE_cold(if_set);
	if_zero = ir_END();

	ir_IF_TRUE(if_set);
	if_set2 = ir_IF(ir_AND_A(ref, ir_CONST_ADDR(CACHE_SPECIAL)));
	ir_IF_FALSE(if_set2);

	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPNZ) {
			jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
		} else {
			ir_END_list(end_inputs);
		}
	} else if (smart_branch_opcode) {
		ir_END_list(true_inputs);
	} else {
		jit_set_Z_TYPE_INFO(jit, res_addr, IS_TRUE);
		ir_END_list(end_inputs);
	}

	ir_IF_TRUE_cold(if_set2);

	ref2 = jit_EG(zend_constants);
	ref = ir_SHR_A(ref, ir_CONST_ADDR(1));
	if (sizeof(void*) == 8) {
		ref = ir_TRUNC_U32(ref);
	}
	ref2 = ir_EQ(ref, ir_LOAD_U32(ir_ADD_OFFSET(ir_LOAD_A(ref2), offsetof(HashTable, nNumOfElements))));
	ref2 = ir_IF(ref2);
	ir_IF_TRUE(ref2);

	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ) {
			jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
		} else {
			ir_END_list(end_inputs);
		}
	} else if (smart_branch_opcode) {
		ir_END_list(false_inputs);
	} else {
		jit_set_Z_TYPE_INFO(jit, res_addr, IS_FALSE);
		ir_END_list(end_inputs);
	}

	ir_IF_FALSE(ref2);
	ir_MERGE_2(if_zero, ir_END());

	jit_SET_EX_OPLINE(jit, opline);
	ref2 = ir_NE(ir_CALL_1(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_check_constant), ir_CONST_ADDR(zv)), IR_NULL);
	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ) {
			ir_GUARD(ref2, ir_CONST_ADDR(exit_addr));
		} else {
			ir_GUARD_NOT(ref2, ir_CONST_ADDR(exit_addr));
		}
		ir_END_list(end_inputs);
	} else if (smart_branch_opcode) {
		ref2 = ir_IF(ref2);
		ir_IF_TRUE(ref2);
		ir_END_list(true_inputs);
		ir_IF_FALSE(ref2);
		ir_END_list(false_inputs);
	} else {
		jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
			ir_ADD_U32(ir_ZEXT_U32(ref2), ir_CONST_U32(IS_FALSE)));
		ir_END_list(end_inputs);
	}

	if (!smart_branch_opcode || exit_addr) {
		if (end_inputs) {
			ir_MERGE_list(end_inputs);
		}
	} else {
		_zend_jit_merge_smart_branch_inputs(jit, defined_label, undefined_label, true_inputs, false_inputs);
	}

	return 1;
}

static int zend_jit_escape_if_undef(zend_jit_ctx *jit, int var, uint32_t flags, const zend_op *opline, int8_t reg)
{
	zend_jit_addr reg_addr = ZEND_ADDR_REF_ZVAL(zend_jit_deopt_rload(jit, IR_ADDR, reg));
	ir_ref if_def = ir_IF(jit_Z_TYPE(jit, reg_addr));

	ir_IF_FALSE_cold(if_def);

	if (flags & ZEND_JIT_EXIT_RESTORE_CALL) {
		if (!zend_jit_save_call_chain(jit, -1)) {
			return 0;
		}
	}

	if ((opline-1)->opcode != ZEND_FETCH_CONSTANT
	 && (opline-1)->opcode != ZEND_FETCH_LIST_R
	 && ((opline-1)->op1_type & (IS_VAR|IS_TMP_VAR))
	 && !(flags & ZEND_JIT_EXIT_FREE_OP1)) {
		zend_jit_addr val_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, (opline-1)->op1.var);

		zend_jit_zval_try_addref(jit, val_addr);
	}

	jit_LOAD_IP_ADDR(jit, opline - 1);
	ir_IJMP(jit_STUB_ADDR(jit, jit_stub_trace_escape));

	ir_IF_TRUE(if_def);

	return 1;
}

static int zend_jit_restore_zval(zend_jit_ctx *jit, int var, int8_t reg)
{
	zend_jit_addr var_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, var);
	zend_jit_addr reg_addr = ZEND_ADDR_REF_ZVAL(zend_jit_deopt_rload(jit, IR_ADDR, reg));

	// JIT: ZVAL_COPY_OR_DUP(EX_VAR(opline->result.var), &c->value); (no dup)
	jit_ZVAL_COPY(jit, var_addr, MAY_BE_ANY, reg_addr, MAY_BE_ANY, 1);
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
	ir_ref ref = jit_ZVAL_ADDR(jit, val_addr);
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
		ir_ref if_type = jit_if_Z_TYPE(jit, val_addr, type);

		ir_IF_TRUE(if_type);
		end1 = ir_END();
		ref1 = ref;
		ir_IF_FALSE_cold(if_type);

		SET_STACK_REF_EX(stack, EX_VAR_TO_NUM(opline->result.var), ref, ZREG_ZVAL_COPY);
		exit_point = zend_jit_trace_get_exit_point(opline+1, flags);
		res_exit_addr = zend_jit_trace_get_exit_addr(exit_point);
		if (!res_exit_addr) {
			return 0;
		}

		jit_guard_Z_TYPE(jit, val_addr, IS_REFERENCE, res_exit_addr);
		ref = ir_ADD_OFFSET(jit_Z_PTR(jit, val_addr), offsetof(zend_reference, val));
		val_addr = ZEND_ADDR_REF_ZVAL(ref);
	}

	SET_STACK_REF_EX(stack, EX_VAR_TO_NUM(opline->result.var), ref, ZREG_ZVAL_COPY);
	exit_point = zend_jit_trace_get_exit_point(opline+1, flags);
	res_exit_addr = zend_jit_trace_get_exit_addr(exit_point);
	if (!res_exit_addr) {
		return 0;
	}

	jit_guard_Z_TYPE(jit, val_addr, type, res_exit_addr);

	if (deref) {
		ir_MERGE_WITH(end1);
		ref = ir_PHI_2(ref, ref1);
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
	ref = ir_LOAD_A(ir_ADD_OFFSET(ir_LOAD_A(jit_EX(run_time_cache)), opline->extended_value));

	// JIT: if (c != NULL)
	if_set = ir_IF(ref);

	if (!zend_jit_is_persistent_constant(zv, opline->op1.num)) {
		// JIT: if (!IS_SPECIAL_CACHE_VAL(c))
		ir_IF_FALSE_cold(if_set);
		not_set_path = ir_END();
		ir_IF_TRUE(if_set);
		if_special = ir_IF(ir_AND_A(ref, ir_CONST_ADDR(CACHE_SPECIAL)));
		ir_IF_TRUE_cold(if_special);
		special_path = ir_END();
		ir_IF_FALSE(if_special);
		fast_path = ir_END();
		ir_MERGE_2(not_set_path, special_path);
	} else {
		ir_IF_TRUE(if_set);
		fast_path = ir_END();
		ir_IF_FALSE_cold(if_set);
	}

	// JIT: zend_jit_get_constant(RT_CONSTANT(opline, opline->op2) + 1, opline->op1.num);
	jit_SET_EX_OPLINE(jit, opline);
	ref2 = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_get_constant),
		ir_CONST_ADDR(zv),
		ir_CONST_U32(opline->op1.num));
	ir_GUARD(ref2, jit_STUB_ADDR(jit, jit_stub_exception_handler));

	ir_MERGE_WITH(fast_path);
	ref = ir_PHI_2(ref2, ref);

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
		jit_ZVAL_COPY(jit, res_addr, MAY_BE_ANY, const_addr, res_info, 1);
		if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, res_info)) {
			return 0;
		}
	} else {
		ir_ref const_addr = ZEND_ADDR_REF_ZVAL(ref);

		// JIT: ZVAL_COPY_OR_DUP(EX_VAR(opline->result.var), &c->value); (no dup)
		jit_ZVAL_COPY(jit, res_addr, MAY_BE_ANY, const_addr, MAY_BE_ANY, 1);
	}


	return 1;
}

static int zend_jit_type_check(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_uchar smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr)
{
	uint32_t  mask;
	zend_jit_addr op1_addr = OP1_ADDR();
	zend_jit_addr res_addr = 0;
	uint32_t true_label = -1, false_label = -1;
	ir_ref end_inputs = IR_UNUSED, true_inputs = IR_UNUSED, false_inputs = IR_UNUSED;

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
			if_def = jit_if_not_Z_TYPE(jit, op1_addr, IS_UNDEF);
			ir_IF_FALSE_cold(if_def);
		}

		jit_SET_EX_OPLINE(jit, opline);
		ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_undefined_op_helper), ir_CONST_U32(opline->op1.var));
		zend_jit_check_exception_undef_result(jit, opline);
		if (opline->extended_value & MAY_BE_NULL) {
			if (exit_addr) {
				if (smart_branch_opcode == ZEND_JMPNZ) {
					jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
				} else {
					ir_END_list(end_inputs);
				}
			} else if (smart_branch_opcode) {
				ir_END_list(true_inputs);
			} else {
				jit_set_Z_TYPE_INFO(jit, res_addr, IS_TRUE);
				ir_END_list(end_inputs);
			}
		} else {
			if (exit_addr) {
				if (smart_branch_opcode == ZEND_JMPZ) {
					jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
				} else {
					ir_END_list(end_inputs);
				}
			} else if (smart_branch_opcode) {
				ir_END_list(false_inputs);
			} else {
				jit_set_Z_TYPE_INFO(jit, res_addr, IS_FALSE);
				if (if_def) {
					ir_END_list(end_inputs);
				}
			}
		}

		if (if_def) {
			ir_IF_TRUE(if_def);
			op1_info |= MAY_BE_NULL;
		}
	}

	if (op1_info & (MAY_BE_ANY|MAY_BE_REF)) {
		mask = opline->extended_value;
		if (!(op1_info & MAY_BE_GUARD) && !(op1_info & (MAY_BE_ANY - mask))) {			
			jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
			if (exit_addr) {
				if (smart_branch_opcode == ZEND_JMPNZ) {
					jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
				} else if (end_inputs) {
					ir_END_list(end_inputs);
				}
			} else if (smart_branch_opcode) {
				ir_END_list(true_inputs);
			} else {
				jit_set_Z_TYPE_INFO(jit, res_addr, IS_TRUE);
				ir_END_list(end_inputs);
			}
	    } else if (!(op1_info & MAY_BE_GUARD) && !(op1_info & mask)) {
			jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
			if (exit_addr) {
				if (smart_branch_opcode == ZEND_JMPZ) {
					jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
				} else if (end_inputs) {
					ir_END_list(end_inputs);
				}
			} else if (smart_branch_opcode) {
				ir_END_list(false_inputs);
			} else {
				jit_set_Z_TYPE_INFO(jit, res_addr, IS_FALSE);
				ir_END_list(end_inputs);
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
				ir_ref ref = jit_ZVAL_ADDR(jit, op1_addr);
				ref = jit_ZVAL_DEREF_ref(jit, ref);
				op1_addr = ZEND_ADDR_REF_ZVAL(ref);
			}
			if (type == 0) {
				ref = ir_AND_U32(ir_SHL_U32(ir_CONST_U32(1), jit_Z_TYPE(jit, op1_addr)), ir_CONST_U32(mask));
				if (!smart_branch_opcode) {
					ref = ir_NE(ref, ir_CONST_U32(0));
				}
			} else if (invert) {
				ref = ir_NE(jit_Z_TYPE(jit, op1_addr), ir_CONST_U8(type));
			} else {
				ref = ir_EQ(jit_Z_TYPE(jit, op1_addr), ir_CONST_U8(type));
			}

			jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);

			if (exit_addr) {
				if (smart_branch_opcode == ZEND_JMPZ) {
					ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
				} else {
					ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
				}
				if (end_inputs) {
					ir_END_list(end_inputs);
				}
			} else if (smart_branch_opcode) {
				ir_ref if_val = ir_IF(ref);
				ir_IF_TRUE(if_val);
				ir_END_list(true_inputs);
				ir_IF_FALSE(if_val);
				ir_END_list(false_inputs);
			} else {
				jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
					ir_ADD_U32(ir_ZEXT_U32(ref), ir_CONST_U32(IS_FALSE)));
				ir_END_list(end_inputs);
			}
	    }
	}

	if (!smart_branch_opcode || exit_addr) {
		if (end_inputs) {
			ir_MERGE_list(end_inputs);
		}
	} else {
		_zend_jit_merge_smart_branch_inputs(jit, true_label, false_label, true_inputs, false_inputs);
	}

	return 1;
}

static int zend_jit_isset_isempty_cv(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr,  zend_uchar smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr)
{
	zend_jit_addr res_addr = RES_ADDR();
	uint32_t true_label = -1, false_label = -1;
	ir_ref end_inputs = IR_UNUSED, true_inputs = IR_UNUSED, false_inputs = IR_UNUSED;

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
		ir_ref ref = jit_ZVAL_ADDR(jit, op1_addr);
		ref = jit_ZVAL_DEREF_ref(jit, ref);
		op1_addr = ZEND_ADDR_REF_ZVAL(ref);
	}

	if (!(op1_info & (MAY_BE_UNDEF|MAY_BE_NULL))) {
		if (exit_addr) {
			ZEND_ASSERT(smart_branch_opcode == ZEND_JMPZ);
		} else if (smart_branch_opcode) {
			ir_END_list(true_inputs);
		} else {
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_TRUE);
			ir_END_list(end_inputs);
		}
	} else if (!(op1_info & (MAY_BE_ANY - MAY_BE_NULL))) {
		if (exit_addr) {
			ZEND_ASSERT(smart_branch_opcode == ZEND_JMPNZ);
		} else if (smart_branch_opcode) {
			ir_END_list(false_inputs);
		} else {
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_FALSE);
			ir_END_list(end_inputs);
		}
	} else {
		ir_ref ref = ir_GT(jit_Z_TYPE(jit, op1_addr), ir_CONST_U8(IS_NULL));
		if (exit_addr) {
			if (smart_branch_opcode == ZEND_JMPNZ) {
				ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
			} else {
				ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
			}
			if (end_inputs) {
				ir_END_list(end_inputs);
			}
		} else if (smart_branch_opcode) {
			ir_ref if_val = ir_IF(ref);
			ir_IF_TRUE(if_val);
			ir_END_list(true_inputs);
			ir_IF_FALSE(if_val);
			ir_END_list(false_inputs);
		} else {
			jit_set_Z_TYPE_INFO_ref(jit, jit_ZVAL_ADDR(jit, res_addr),
				ir_ADD_U32(ir_ZEXT_U32(ref), ir_CONST_U32(IS_FALSE)));
			ir_END_list(end_inputs);
		}
	}

	if (!smart_branch_opcode || exit_addr) {
		if (end_inputs) {
			ir_MERGE_list(end_inputs);
		}
	} else {
		_zend_jit_merge_smart_branch_inputs(jit, true_label, false_label, true_inputs, false_inputs);
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
	ir_GUARD(
		ir_UGE(
			ir_SUB_A(ir_LOAD_A(jit_EG(vm_stack_end)), ir_LOAD_A(jit_EG(vm_stack_top))),
			ir_CONST_ADDR(used_stack)),
		ir_CONST_ADDR(exit_addr));

	return 1;
}

static int zend_jit_free_trampoline(zend_jit_ctx *jit, int8_t func_reg)
{
	ZEND_ASSERT(0 && "NIY");
	return 1;
}

static int zend_jit_push_call_frame(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, zend_function *func, bool is_closure, bool delayed_fetch_this, int checked_stack, ir_ref func_ref, ir_ref this_ref)
{
	uint32_t used_stack;
	ir_ref used_stack_ref = IR_UNUSED;
	bool stack_check = 1;
	ir_ref rx, ref, top, if_enough_stack, cold_path = IR_UNUSED;

	ZEND_ASSERT(func_ref != IR_NULL);
	if (func) {
		used_stack = zend_vm_calc_used_stack(opline->extended_value, func);
		if ((int)used_stack <= checked_stack) {
			stack_check = 0;
		}
		used_stack_ref = ir_CONST_ADDR(used_stack);
	} else {
		ir_ref num_args_ref;
		ir_ref if_internal_func = IR_UNUSED;

		used_stack = (ZEND_CALL_FRAME_SLOT + opline->extended_value + ZEND_OBSERVER_ENABLED) * sizeof(zval);
		used_stack_ref = ir_CONST_ADDR(used_stack);

		if (!is_closure) {
			used_stack_ref = ir_HARD_COPY_A(used_stack_ref); /* load constant once */

			// JIT: if (EXPECTED(ZEND_USER_CODE(func->type))) {
			ir_ref tmp = ir_LOAD_U8(ir_ADD_OFFSET(func_ref, offsetof(zend_function, type)));
			if_internal_func = ir_IF(ir_AND_U8(tmp, ir_CONST_U8(1)));
			ir_IF_FALSE(if_internal_func);
		}

		// JIT: used_stack += (func->op_array.last_var + func->op_array.T - MIN(func->op_array.num_args, num_args)) * sizeof(zval);
		num_args_ref = ir_CONST_U32(opline->extended_value);
		if (!is_closure) {
			ref = ir_SUB_U32(
				ir_SUB_U32(
					ir_MIN_U32(
						num_args_ref,
						ir_LOAD_U32(ir_ADD_OFFSET(func_ref, offsetof(zend_function, op_array.num_args)))),
					ir_LOAD_U32(ir_ADD_OFFSET(func_ref, offsetof(zend_function, op_array.last_var)))),
				ir_LOAD_U32(ir_ADD_OFFSET(func_ref, offsetof(zend_function, op_array.T))));
		} else {
			ref = ir_SUB_U32(
				ir_SUB_U32(
					ir_MIN_U32(
						num_args_ref,
						ir_LOAD_U32(ir_ADD_OFFSET(func_ref, offsetof(zend_closure, func.op_array.num_args)))),
					ir_LOAD_U32(ir_ADD_OFFSET(func_ref, offsetof(zend_closure, func.op_array.last_var)))),
				ir_LOAD_U32(ir_ADD_OFFSET(func_ref, offsetof(zend_closure, func.op_array.T))));
		}
		ref = ir_MUL_U32(ref, ir_CONST_U32(sizeof(zval)));
		if (sizeof(void*) == 8) {
			ref = ir_SEXT_A(ref);
		}
		ref = ir_SUB_A(used_stack_ref, ref);

		if (is_closure) {
			used_stack_ref = ref;
		} else {
			ir_MERGE_WITH_EMPTY_TRUE(if_internal_func);
			used_stack_ref = ir_PHI_2(ref, used_stack_ref);
		}
	}

	zend_jit_start_reuse_ip(jit);

	// JIT: if (UNEXPECTED(used_stack > (size_t)(((char*)EG(vm_stack_end)) - (char*)call))) {
	jit_STORE_IP(jit, ir_LOAD_A(jit_EG(vm_stack_top)));

	if (stack_check) {
		// JIT: Check Stack Overflow
		ref = ir_UGE(
			ir_SUB_A(
				ir_LOAD_A(jit_EG(vm_stack_end)),
				jit_IP(jit)),
			used_stack_ref);

		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
			int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
			const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

			if (!exit_addr) {
				return 0;
			}

			ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
		} else {
			if_enough_stack = ir_IF(ref);
			ir_IF_FALSE_cold(if_enough_stack);

#ifdef _WIN32
			if (0) {
#else
			if (opline->opcode == ZEND_INIT_FCALL && func && func->type == ZEND_INTERNAL_FUNCTION) {
#endif
				jit_SET_EX_OPLINE(jit, opline);
				ref = ir_CALL_1(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_int_extend_stack_helper), used_stack_ref);
			} else {
				if (!is_closure) {
					ref = func_ref;
				} else {
					ref = ir_ADD_OFFSET(func_ref, offsetof(zend_closure, func));
				}
				jit_SET_EX_OPLINE(jit, opline);
				ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_extend_stack_helper),
					used_stack_ref, ref);
			}
			jit_STORE_IP(jit, ref);

			cold_path = ir_END();
			ir_IF_TRUE(if_enough_stack);
		}
	}

	ref = jit_EG(vm_stack_top);
	rx = jit_IP(jit);
#if !OPTIMIZE_FOR_SIZE
	/* JIT: EG(vm_stack_top) = (zval*)((char*)call + used_stack);
	 * This vesions is longer but faster
	 *    mov EG(vm_stack_top), %CALL
	 *    lea size(%call), %tmp
	 *    mov %tmp, EG(vm_stack_top)
	 */
	top = rx;
#else
	/* JIT: EG(vm_stack_top) += used_stack;
	 * Use ir_emit() because ir_LOAD() makes load forwarding and doesn't allow load/store fusion
	 *    mov EG(vm_stack_top), %CALL
	 *    add $size, EG(vm_stack_top)
	 */
	top = jit->ctx.control = ir_emit2(&jit->ctx, IR_OPT(IR_LOAD, IR_ADDR), jit->ctx.control, ref);
#endif
	ir_STORE(ref, ir_ADD_A(top, used_stack_ref));

	// JIT: zend_vm_init_call_frame(call, call_info, func, num_args, called_scope, object);
	if (JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE || opline->opcode != ZEND_INIT_METHOD_CALL) {
		// JIT: ZEND_SET_CALL_INFO(call, 0, call_info);
		ir_STORE(jit_CALL(rx, This.u1.type_info), ir_CONST_U32(IS_UNDEF | ZEND_CALL_NESTED_FUNCTION));
	}
#ifdef _WIN32
	if (0) {
#else
	if (opline->opcode == ZEND_INIT_FCALL && func && func->type == ZEND_INTERNAL_FUNCTION) {
#endif
		if (cold_path) {
			ir_MERGE_WITH(cold_path);
			rx = jit_IP(jit);
		}

		// JIT: call->func = func;
		ir_STORE(jit_CALL(rx, func), func_ref);
	} else {
		if (!is_closure) {
			// JIT: call->func = func;
			ir_STORE(jit_CALL(rx, func), func_ref);
		} else {
			// JIT: call->func = &closure->func;
			ir_STORE(jit_CALL(rx, func), ir_ADD_OFFSET(func_ref, offsetof(zend_closure, func)));
		}
		if (cold_path) {
			ir_MERGE_WITH(cold_path);
			rx = jit_IP(jit);
		}
	}
	if (opline->opcode == ZEND_INIT_METHOD_CALL) {
		// JIT: Z_PTR(call->This) = obj;
		ZEND_ASSERT(this_ref != IR_NULL);
		ir_STORE(jit_CALL(rx, This.value.ptr), this_ref);
	    if (opline->op1_type == IS_UNUSED || delayed_fetch_this) {
			// JIT: call->call_info |= ZEND_CALL_HAS_THIS;
			ref = jit_CALL(rx, This.u1.type_info);
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				ir_STORE(ref, ir_CONST_U32( ZEND_CALL_HAS_THIS));
			} else {
				ir_STORE(ref, ir_OR_U32(ir_LOAD_U32(ref), ir_CONST_U32(ZEND_CALL_HAS_THIS)));
			}
	    } else {
			if (opline->op1_type == IS_CV) {
				// JIT: GC_ADDREF(obj);
				jit_GC_ADDREF(jit, this_ref);
			}

			// JIT: call->call_info |= ZEND_CALL_HAS_THIS | ZEND_CALL_RELEASE_THIS;
			ref = jit_CALL(rx, This.u1.type_info);
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				ir_STORE(ref, ir_CONST_U32( ZEND_CALL_HAS_THIS | ZEND_CALL_RELEASE_THIS));
			} else {
				ir_STORE(ref,
					ir_OR_U32(ir_LOAD_U32(ref),
						ir_CONST_U32(ZEND_CALL_HAS_THIS | ZEND_CALL_RELEASE_THIS)));
			}
	    }
	} else if (!is_closure) {
		// JIT: Z_CE(call->This) = called_scope;
		ir_STORE(jit_CALL(rx, This), IR_NULL);
	} else {
		ir_ref object_or_called_scope, call_info, call_info2, object, if_cond;

		if (opline->op2_type == IS_CV) {
			// JIT: GC_ADDREF(closure);
			jit_GC_ADDREF(jit, func_ref);
		}

		// JIT: RX(object_or_called_scope) = closure->called_scope;
		object_or_called_scope = ir_LOAD_A(ir_ADD_OFFSET(func_ref, offsetof(zend_closure, called_scope)));

		// JIT: call_info = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_DYNAMIC | ZEND_CALL_CLOSURE |
		//      (closure->func->common.fn_flags & ZEND_ACC_FAKE_CLOSURE);
		call_info = ir_OR_U32(
			ir_AND_U32(
				ir_LOAD_U32(ir_ADD_OFFSET(func_ref, offsetof(zend_closure, func.common.fn_flags))),
				ir_CONST_U32(ZEND_ACC_FAKE_CLOSURE)),
			ir_CONST_U32(ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_DYNAMIC | ZEND_CALL_CLOSURE));
		// JIT: if (Z_TYPE(closure->this_ptr) != IS_UNDEF) {
		if_cond = ir_IF(ir_LOAD_U8(ir_ADD_OFFSET(func_ref, offsetof(zend_closure, this_ptr.u1.v.type))));
		ir_IF_TRUE(if_cond);

		// JIT: call_info |= ZEND_CALL_HAS_THIS;
		call_info2 = ir_OR_U32(call_info, ir_CONST_U32(ZEND_CALL_HAS_THIS));

		// JIT: object_or_called_scope = Z_OBJ(closure->this_ptr);
		object = ir_LOAD_A(ir_ADD_OFFSET(func_ref, offsetof(zend_closure, this_ptr.value.ptr)));

		ir_MERGE_WITH_EMPTY_FALSE(if_cond);
		call_info = ir_PHI_2(call_info2, call_info);
		object_or_called_scope = ir_PHI_2(object, object_or_called_scope);

		// JIT: ZEND_SET_CALL_INFO(call, 0, call_info);
		ref = jit_CALL(rx, This.u1.type_info);
		ir_STORE(ref, ir_OR_U32(ir_LOAD_U32(ref), call_info));

		// JIT: Z_PTR(call->This) = object_or_called_scope;
		ir_STORE(jit_CALL(rx, This.value.ptr), object_or_called_scope);

		// JIT: if (closure->func.op_array.run_time_cache__ptr)
		if_cond = ir_IF(ir_LOAD_A(ir_ADD_OFFSET(func_ref, offsetof(zend_closure, func.op_array.run_time_cache__ptr))));
		ir_IF_FALSE(if_cond);

		// JIT: zend_jit_init_func_run_time_cache_helper(closure->func);
		ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_init_func_run_time_cache_helper),
			ir_ADD_OFFSET(func_ref, offsetof(zend_closure, func)));

		ir_MERGE_WITH_EMPTY_TRUE(if_cond);
	}

	// JIT: ZEND_CALL_NUM_ARGS(call) = num_args;
	ir_STORE(jit_CALL(rx, This.u2.num_args), ir_CONST_U32(opline->extended_value));

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
	call = ir_LOAD_A(jit_EX(call));
	while (level > 0) {
		// call = call->prev_execute_data
		call = ir_LOAD_A(jit_CALL(call, prev_execute_data));
		level--;
	}

	if (func->type == ZEND_USER_FUNCTION &&
	    (!(func->common.fn_flags & ZEND_ACC_IMMUTABLE) ||
	     (func->common.fn_flags & ZEND_ACC_CLOSURE) ||
	     !func->common.function_name)) {
		const zend_op *opcodes = func->op_array.opcodes;

		// JIT: if (call->func.op_array.opcodes != opcodes) goto exit_addr;
		ir_GUARD(
			ir_EQ(
				ir_LOAD_A(ir_ADD_OFFSET(ir_LOAD_A(jit_CALL(call, func)), offsetof(zend_op_array, opcodes))),
				ir_CONST_ADDR(opcodes)),
			ir_CONST_ADDR(exit_addr));
	} else {
		// JIT: if (call->func != func) goto exit_addr;
		ir_GUARD(ir_EQ(ir_LOAD_A(jit_CALL(call, func)), ir_CONST_ADDR(func)), ir_CONST_ADDR(exit_addr));
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
		func_ref = ir_CONST_ADDR(func);
	} else if (func && op_array == &func->op_array) {
		/* recursive call */
		if (!(func->op_array.fn_flags & ZEND_ACC_IMMUTABLE)
		 || zend_jit_prefer_const_addr_load(jit, (uintptr_t)func)) {
			func_ref = ir_LOAD_A(jit_EX(func));
		} else {
			func_ref = ir_CONST_ADDR(func);
		}
	} else {
		ir_ref if_func, cache_slot_ref, ref;

		// JIT: if (CACHED_PTR(opline->result.num))
		cache_slot_ref = ir_ADD_OFFSET(ir_LOAD_A(jit_EX(run_time_cache)), opline->result.num);
		func_ref = ir_LOAD_A(cache_slot_ref);
		if_func = ir_IF(func_ref);
		ir_IF_FALSE_cold(if_func);
		if (opline->opcode == ZEND_INIT_FCALL
		 && func
		 && func->type == ZEND_USER_FUNCTION
		 && (func->op_array.fn_flags & ZEND_ACC_IMMUTABLE)) {
			ref = ir_HARD_COPY_A(ir_CONST_ADDR(func)); /* load constant once */
		    ir_STORE(cache_slot_ref, ref);
			ref = ir_CALL_1(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_init_func_run_time_cache_helper), ref);
		} else {
			zval *zv = RT_CONSTANT(opline, opline->op2);

			if (opline->opcode == ZEND_INIT_FCALL) {
				ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_find_func_helper),
					ir_CONST_ADDR(Z_STR_P(zv)),
					cache_slot_ref);
			} else if (opline->opcode == ZEND_INIT_FCALL_BY_NAME) {
				ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_find_func_helper),
					ir_CONST_ADDR(Z_STR_P(zv + 1)),
					cache_slot_ref);
			} else if (opline->opcode == ZEND_INIT_NS_FCALL_BY_NAME) {
				ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_find_ns_func_helper),
					ir_CONST_ADDR(zv),
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
					ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
				} else if (func->type == ZEND_USER_FUNCTION
					 && !(func->common.fn_flags & ZEND_ACC_IMMUTABLE)) {
					const zend_op *opcodes = func->op_array.opcodes;

					ir_GUARD(
						ir_EQ(
							ir_LOAD_A(ir_ADD_OFFSET(ref, offsetof(zend_op_array, opcodes))),
							ir_CONST_ADDR(opcodes)),
						ir_CONST_ADDR(exit_addr));
				} else {
					ir_GUARD(ir_EQ(ref, ir_CONST_ADDR(func)), ir_CONST_ADDR(exit_addr));
				}
			} else {
jit_SET_EX_OPLINE(jit, opline);
				ir_GUARD(ref, jit_STUB_ADDR(jit, jit_stub_undefined_function));
			}
		}
		ir_MERGE_WITH_EMPTY_TRUE(if_func);
		func_ref = ir_PHI_2(ref, func_ref);
	}

	if (!zend_jit_push_call_frame(jit, opline, op_array, func, 0, 0, checked_stack, func_ref, IR_UNUSED)) {
		return 0;
	}

	if (zend_jit_needs_call_chain(call_info, b, op_array, ssa, ssa_op, opline, call_level, trace)) {
		if (!zend_jit_save_call_chain(jit, call_level)) {
			return 0;
		}
	} else {
		ZEND_ASSERT(call_level > 0);
		jit->delayed_call_level = call_level;
		delayed_call_chain = 1;
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
	ir_ref if_static = IR_UNUSED, cold_path, this_ref = IR_NULL, func_ref = IR_NULL;

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
			this_ref = jit_Z_PTR(jit, this_addr);
		} else {
		    if (op1_info & MAY_BE_REF) {
				if (opline->op1_type == IS_CV) {
					// JIT: ZVAL_DEREF(op1)
					ir_ref ref = jit_ZVAL_ADDR(jit, op1_addr);
					ref = jit_ZVAL_DEREF_ref(jit, ref);
					op1_addr = ZEND_ADDR_REF_ZVAL(ref);
				} else {
					ir_ref if_ref;

					/* Hack: Convert reference to regular value to simplify JIT code */
					ZEND_ASSERT(Z_REG(op1_addr) == ZREG_FP);

					if_ref = jit_if_Z_TYPE(jit, op1_addr, IS_REFERENCE);
					ir_IF_TRUE(if_ref);
					ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_unref_helper), jit_ZVAL_ADDR(jit, op1_addr));

					ir_MERGE_WITH_EMPTY_FALSE(if_ref);
				}
			}
			if (op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY)- MAY_BE_OBJECT)) {
				if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
					int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
					const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

					if (!exit_addr) {
						return 0;
					}
					ir_GUARD(ir_EQ(jit_Z_TYPE(jit, op1_addr), ir_CONST_U8(IS_OBJECT)),
						ir_CONST_ADDR(exit_addr));
				} else {
					ir_ref if_object = jit_if_Z_TYPE(jit, op1_addr, IS_OBJECT);

					ir_IF_FALSE_cold(if_object);

					jit_SET_EX_OPLINE(jit, opline);
					if ((opline->op1_type & (IS_VAR|IS_TMP_VAR)) && !delayed_fetch_this) {
						ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_invalid_method_call_tmp),
							jit_ZVAL_ADDR(jit, op1_addr));
					} else {
						ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_invalid_method_call),
							jit_ZVAL_ADDR(jit, op1_addr));
					}
					ir_IJMP(jit_STUB_ADDR(jit, jit_stub_exception_handler));
					ir_IF_TRUE(if_object);
				}
			}

			this_ref = jit_Z_PTR(jit, op1_addr);
		}

		if (jit->delayed_call_level) {
			if (!zend_jit_save_call_chain(jit, jit->delayed_call_level)) {
				return 0;
			}
		}

		if (func) {
			// JIT: fbc = CACHED_PTR(opline->result.num + sizeof(void*));
			ref = ir_LOAD_A(ir_ADD_OFFSET(ir_LOAD_A(jit_EX(run_time_cache)), opline->result.num + sizeof(void*)));

			if_found = ir_IF(ref);
			ir_IF_TRUE(if_found);
			fast_path = ir_END();
		} else {
			// JIT: if (CACHED_PTR(opline->result.num) == obj->ce)) {
			run_time_cache = ir_LOAD_A(jit_EX(run_time_cache));
			ref = ir_EQ(
				ir_LOAD_A(ir_ADD_OFFSET(run_time_cache, opline->result.num)),
				ir_LOAD_A(ir_ADD_OFFSET(this_ref, offsetof(zend_object, ce))));
			if_found = ir_IF(ref);
			ir_IF_TRUE(if_found);

			// JIT: fbc = CACHED_PTR(opline->result.num + sizeof(void*));
			ref = ir_LOAD_A(ir_ADD_OFFSET(run_time_cache, opline->result.num + sizeof(void*)));
			fast_path = ir_END();

		}

		ir_IF_FALSE_cold(if_found);
		jit_SET_EX_OPLINE(jit, opline);

		if (!jit->ctx.fixed_call_stack_size) {
			// JIT: alloca(sizeof(void*));
			this_ref2 = ir_ALLOCA(ir_CONST_ADDR(0x10));
		} else {
			this_ref2 = ir_HARD_COPY_A(ir_RLOAD_A(IR_REG_SP));
		}
		ir_STORE(this_ref2, this_ref);

		if ((opline->op1_type & (IS_VAR|IS_TMP_VAR)) && !delayed_fetch_this) {
			ref2 = ir_CALL_3(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_find_method_tmp_helper),
					this_ref,
					ir_CONST_ADDR(function_name),
					this_ref2);
		} else {
			ref2 = ir_CALL_3(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_find_method_helper),
					this_ref,
					ir_CONST_ADDR(function_name),
					this_ref2);
		}

		this_ref2 = ir_LOAD_A(ir_RLOAD_A(IR_REG_SP));
		if (!jit->ctx.fixed_call_stack_size) {
			// JIT: revert alloca
			ir_AFREE(ir_CONST_ADDR(0x10));
		}

		ir_GUARD(ref2, jit_STUB_ADDR(jit, jit_stub_exception_handler));

		ir_MERGE_WITH(fast_path);
		func_ref = ir_PHI_2(ref2, ref);
		this_ref = ir_PHI_2(this_ref2, this_ref);
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

			ir_GUARD(
				ir_EQ(
					ir_LOAD_A(ir_ADD_OFFSET(func_ref, offsetof(zend_op_array, opcodes))),
					ir_CONST_ADDR(opcodes)),
				ir_CONST_ADDR(exit_addr));
		} else {
			ir_GUARD(ir_EQ(func_ref, ir_CONST_ADDR(func)), ir_CONST_ADDR(exit_addr));
		}
	}

	if (!func) {
		// JIT: if (fbc->common.fn_flags & ZEND_ACC_STATIC) {
		if_static = ir_IF(ir_AND_U32(
			ir_LOAD_U32(ir_ADD_OFFSET(func_ref, offsetof(zend_function, common.fn_flags))),
			ir_CONST_U32(ZEND_ACC_STATIC)));
		ir_IF_TRUE_cold(if_static);
	}

	if (!func || (func->common.fn_flags & ZEND_ACC_STATIC) != 0) {
		ir_ref ret;

		if ((opline->op1_type & (IS_VAR|IS_TMP_VAR)) && !delayed_fetch_this) {
			ret = ir_CALL_3(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_push_static_metod_call_frame_tmp),
					this_ref,
					func_ref,
					ir_CONST_U32(opline->extended_value));
		} else {
			ret = ir_CALL_3(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_push_static_metod_call_frame),
					this_ref,
					func_ref,
					ir_CONST_U32(opline->extended_value));
		}

		if ((opline->op1_type & (IS_VAR|IS_TMP_VAR) && !delayed_fetch_this)) {
			ir_GUARD(ret, jit_STUB_ADDR(jit, jit_stub_exception_handler));
		}
		jit_STORE_IP(jit, ret);
	}

	if (!func) {
		cold_path = ir_END();
		ir_IF_FALSE(if_static);
	}

	if (!func || (func->common.fn_flags & ZEND_ACC_STATIC) == 0) {
		if (!zend_jit_push_call_frame(jit, opline, NULL, func, 0, delayed_fetch_this, checked_stack, func_ref, this_ref)) {
			return 0;
		}
	}

	if (!func) {
		ir_MERGE_WITH(cold_path);
	}
	zend_jit_start_reuse_ip(jit);

	if (zend_jit_needs_call_chain(call_info, b, op_array, ssa, ssa_op, opline, call_level, trace)) {
		if (!zend_jit_save_call_chain(jit, call_level)) {
			return 0;
		}
	} else {
		ZEND_ASSERT(call_level > 0);
		delayed_call_chain = 1;
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

	ref = jit_Z_PTR(jit, op2_addr);

	if (ssa->var_info[ssa_op->op2_use].ce != zend_ce_closure
	 && !(ssa->var_info[ssa_op->op2_use].type & MAY_BE_CLASS_GUARD)) {
		int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
		const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

		if (!exit_addr) {
			return 0;
		}

		ir_GUARD(
			ir_EQ(
				ir_LOAD_A(ir_ADD_OFFSET(ref, offsetof(zend_object, ce))),
				ir_CONST_ADDR(zend_ce_closure)),
			ir_CONST_ADDR(exit_addr));

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

		ir_GUARD(
			ir_EQ(
				ir_LOAD_A(ir_ADD_OFFSET(ref, offsetof(zend_closure, func.op_array.opcodes))),
				ir_CONST_ADDR(opcodes)),
			ir_CONST_ADDR(exit_addr));
	}

	if (jit->delayed_call_level) {
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
		ZEND_ASSERT(call_level > 0);
		delayed_call_chain = 1;
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
			ir_ref cond = ir_AND_U32(
				ir_LOAD_U32(ir_ADD_OFFSET(ir_LOAD_A(jit_RX(func)), offsetof(zend_function, quick_arg_flags))),
				ir_CONST_U32(mask));

			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);
				if (!exit_addr) {
					return 0;
				}
				ir_GUARD_NOT(cond, ir_CONST_ADDR(exit_addr));
			} else {
				ir_ref if_pass_by_ref;

				if_pass_by_ref = ir_IF(cond);

				ir_IF_TRUE_cold(if_pass_by_ref);
				if (Z_MODE(op1_addr) == IS_REG) {
					/* set type to avoid zval_ptr_dtor() on uninitialized value */
					zend_jit_addr addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->op1.var);
					jit_set_Z_TYPE_INFO(jit, addr, IS_UNDEF);
				}
				jit_SET_EX_OPLINE(jit, opline);
				ir_IJMP(jit_STUB_ADDR(jit, jit_stub_throw_cannot_pass_by_ref));

				ir_IF_FALSE(if_pass_by_ref);
			}
		}
	}

	arg_addr = ZEND_ADDR_MEM_ZVAL(ZREG_RX, opline->result.var);

	if (opline->op1_type == IS_CONST) {
		zval *zv = RT_CONSTANT(opline, opline->op1);

		jit_ZVAL_COPY_CONST(jit,
			arg_addr,
			MAY_BE_ANY, MAY_BE_ANY,
			zv, 1);
	} else {
		jit_ZVAL_COPY(jit,
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
			op1_addr = jit_ZVAL_INDIRECT_DEREF(jit, op1_addr);
		}
	} else if (opline->op1_type == IS_CV) {
		if (op1_info & MAY_BE_UNDEF) {
			if (op1_info & (MAY_BE_ANY|MAY_BE_REF)) {
				// JIT: if (Z_TYPE_P(op1) == IS_UNDEF)
				ir_ref if_def = jit_if_not_Z_TYPE(jit, op1_addr, IS_UNDEF);
				ir_IF_FALSE(if_def);
				// JIT: ZVAL_NULL(op1)
				jit_set_Z_TYPE_INFO(jit,op1_addr, IS_NULL);
				ir_MERGE_WITH_EMPTY_TRUE(if_def);
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
			if_ref = jit_if_Z_TYPE(jit, op1_addr, IS_REFERENCE);
			ir_IF_TRUE(if_ref);
			// JIT: ref = Z_PTR_P(op1)
			ref = jit_Z_PTR(jit, op1_addr);
			// JIT: GC_ADDREF(ref)
			jit_GC_ADDREF(jit, ref);
			// JIT: ZVAL_REFERENCE(arg, ref)
			jit_set_Z_PTR(jit, arg_addr, ref);
			jit_set_Z_TYPE_INFO(jit, arg_addr, IS_REFERENCE_EX);
			ref_path = ir_END();
			ir_IF_FALSE(if_ref);
		}

		// JIT: ZVAL_NEW_REF(arg, varptr);
		// JIT: ref = emalloc(sizeof(zend_reference));
		ref = jit_EMALLOC(jit, sizeof(zend_reference), op_array, opline);
		// JIT: GC_REFCOUNT(ref) = 2
		jit_set_GC_REFCOUNT(jit, ref, 2);
		// JIT: GC_TYPE(ref) = GC_REFERENCE
		ir_STORE(ir_ADD_OFFSET(ref, offsetof(zend_reference, gc.u.type_info)), ir_CONST_U32(GC_REFERENCE));
		ir_STORE(ir_ADD_OFFSET(ref, offsetof(zend_reference, sources.ptr)), IR_NULL);
		ref2 = ir_ADD_OFFSET(ref, offsetof(zend_reference, val));
		ref_addr = ZEND_ADDR_REF_ZVAL(ref2);

        // JIT: ZVAL_COPY_VALUE(&ref->val, op1)
		jit_ZVAL_COPY(jit,
			ref_addr,
			MAY_BE_ANY,
			op1_addr, op1_info, 0);

		// JIT: ZVAL_REFERENCE(arg, ref)
		jit_set_Z_PTR(jit, op1_addr, ref);
		jit_set_Z_TYPE_INFO(jit, op1_addr, IS_REFERENCE_EX);

		// JIT: ZVAL_REFERENCE(arg, ref)
		jit_set_Z_PTR(jit, arg_addr, ref);
		jit_set_Z_TYPE_INFO(jit, arg_addr, IS_REFERENCE_EX);
	}

	if (ref_path) {
		ir_MERGE_WITH(ref_path);
	}

	jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);

	return 1;
}

static int zend_jit_send_var(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, uint32_t op1_info, zend_jit_addr op1_addr, zend_jit_addr op1_def_addr)
{
	uint32_t arg_num = opline->op2.num;
	zend_jit_addr arg_addr;
	ir_ref end_inputs = IR_UNUSED;

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

			// JIT: if (RX->func->quick_arg_flags & mask)
			ir_ref if_send_by_ref = ir_IF(ir_AND_U32(
				ir_LOAD_U32(ir_ADD_OFFSET(ir_LOAD_A(jit_RX(func)), offsetof(zend_function, quick_arg_flags))),
				ir_CONST_U32(mask)));
			ir_IF_TRUE_cold(if_send_by_ref);

			if (!zend_jit_send_ref(jit, opline, op_array, op1_info, 1)) {
				return 0;
			}

			ir_END_list(end_inputs);
			ir_IF_FALSE(if_send_by_ref);
		}
	} else if (opline->opcode == ZEND_SEND_VAR_NO_REF_EX) {
		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE
		 && JIT_G(current_frame)
		 && JIT_G(current_frame)->call
		 && JIT_G(current_frame)->call->func) {
			if (ARG_SHOULD_BE_SENT_BY_REF(JIT_G(current_frame)->call->func, arg_num)) {

		        // JIT: ZVAL_COPY_VALUE(arg, op1)
				jit_ZVAL_COPY(jit,
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
						ir_GUARD(ir_EQ(jit_Z_TYPE(jit, op1_addr), ir_CONST_U32(IS_REFERENCE)),
							ir_CONST_ADDR(exit_addr));
					}
				}
				return 1;
			}
		} else {
			uint32_t mask = (ZEND_SEND_BY_REF|ZEND_SEND_PREFER_REF) << ((arg_num + 3) * 2);
			ir_ref func, if_send_by_ref, if_prefer_ref;

			// JIT: if (RX->func->quick_arg_flags & mask)
			func = ir_LOAD_A(jit_RX(func));
			if_send_by_ref = ir_IF(ir_AND_U32(
				ir_LOAD_U32(ir_ADD_OFFSET(func, offsetof(zend_function, quick_arg_flags))),
				ir_CONST_U32(mask)));
			ir_IF_TRUE_cold(if_send_by_ref);

			mask = ZEND_SEND_PREFER_REF << ((arg_num + 3) * 2);

	        // JIT: ZVAL_COPY_VALUE(arg, op1)
			jit_ZVAL_COPY(jit,
				arg_addr,
				MAY_BE_ANY,
				op1_addr, op1_info, 0);

			if (op1_info & MAY_BE_REF) {
				ir_ref if_ref = jit_if_Z_TYPE(jit, arg_addr, IS_REFERENCE);
				ir_IF_TRUE(if_ref);
				ir_END_list(end_inputs);
				ir_IF_FALSE(if_ref);
			}

			// JIT: if (RX->func->quick_arg_flags & mask)
			if_prefer_ref = ir_IF(ir_AND_U32(
				ir_LOAD_U32(ir_ADD_OFFSET(func, offsetof(zend_function, quick_arg_flags))),
				ir_CONST_U32(mask)));
			ir_IF_TRUE(if_prefer_ref);
			ir_END_list(end_inputs);
			ir_IF_FALSE(if_prefer_ref);

			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);
				if (!exit_addr) {
					return 0;
				}
				jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
			} else {
				jit_SET_EX_OPLINE(jit, opline);
				ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_only_vars_by_reference),
					jit_ZVAL_ADDR(jit, arg_addr));
				zend_jit_check_exception(jit);
				ir_END_list(end_inputs);
			}

			ir_IF_FALSE(if_send_by_ref);
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
			// JIT: if (RX->This.u1.type_info & ZEND_CALL_SEND_ARG_BY_REF)
			ir_ref if_send_by_ref = ir_IF(ir_AND_U32(
				ir_LOAD_U32(jit_RX(This.u1.type_info)),
				ir_CONST_U32(ZEND_CALL_SEND_ARG_BY_REF)));
			ir_IF_TRUE_cold(if_send_by_ref);

			if (!zend_jit_send_ref(jit, opline, op_array, op1_info, 1)) {
				return 0;
			}

			ir_END_list(end_inputs);
			ir_IF_FALSE(if_send_by_ref);
		}
	}

	if (op1_info & MAY_BE_UNDEF) {
		ir_ref ref, if_def = IR_UNUSED;

		if (op1_info & (MAY_BE_ANY|MAY_BE_REF)) {
			if_def = jit_if_not_Z_TYPE(jit, op1_addr, IS_UNDEF);
			ir_IF_FALSE_cold(if_def);
		}

		// JIT: zend_jit_undefined_op_helper(opline->op1.var)
		jit_SET_EX_OPLINE(jit, opline);
		ref = ir_CALL_1(IR_I32, ir_CONST_FC_FUNC(zend_jit_undefined_op_helper),
			ir_CONST_U32(opline->op1.var));

		// JIT: ZVAL_NULL(arg)
		jit_set_Z_TYPE_INFO(jit, arg_addr, IS_NULL);

		// JIT: check_exception
		ir_GUARD(ref, jit_STUB_ADDR(jit, jit_stub_exception_handler));

		if (op1_info & (MAY_BE_ANY|MAY_BE_REF)) {
			ir_END_list(end_inputs);
			ir_IF_TRUE(if_def);
		} else {
			if (end_inputs) {
				ir_END_list(end_inputs);
				ir_MERGE_list(end_inputs);
			}
			return 1;
		}
	}

	if (opline->opcode == ZEND_SEND_VAR_NO_REF) {
        // JIT: ZVAL_COPY_VALUE(arg, op1)
		jit_ZVAL_COPY(jit,
			arg_addr,
			MAY_BE_ANY,
			op1_addr, op1_info, 0);
		if (op1_info & MAY_BE_REF) {
				// JIT: if (Z_TYPE_P(arg) == IS_REFERENCE)
				ir_ref if_ref = jit_if_Z_TYPE(jit, arg_addr, IS_REFERENCE);
				ir_IF_TRUE(if_ref);
				ir_END_list(end_inputs);
				ir_IF_FALSE(if_ref);
		}
		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
			int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
			const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);
			if (!exit_addr) {
				return 0;
			}
			ir_GUARD(IR_FALSE, ir_CONST_ADDR(exit_addr));
		} else {
			jit_SET_EX_OPLINE(jit, opline);
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_only_vars_by_reference),
				jit_ZVAL_ADDR(jit, arg_addr));
			zend_jit_check_exception(jit);
		}
	} else {
		if (op1_info & MAY_BE_REF) {
			if (opline->op1_type == IS_CV) {
				ir_ref ref;

				// JIT: ZVAL_DEREF(op1)
				ref = jit_ZVAL_ADDR(jit, op1_addr);
				ref = jit_ZVAL_DEREF_ref(jit, ref);
				op1_addr = ZEND_ADDR_REF_ZVAL(ref);

		        // JIT: ZVAL_COPY(arg, op1)
				jit_ZVAL_COPY(jit,
					arg_addr,
					MAY_BE_ANY,
					op1_addr, op1_info, 1);
			} else {
				ir_ref if_ref, ref, ref2, refcount, if_not_zero, if_refcounted;
				zend_jit_addr ref_addr;

				// JIT: if (Z_TYPE_P(op1) == IS_REFERENCE)
				if_ref = jit_if_Z_TYPE(jit, op1_addr, IS_REFERENCE);
				ir_IF_TRUE_cold(if_ref);

				// JIT: ref = Z_COUNTED_P(op1);
				ref = jit_Z_PTR(jit, op1_addr);
				ref2 = ir_ADD_OFFSET(ref, offsetof(zend_reference, val));
				ref_addr = ZEND_ADDR_REF_ZVAL(ref2);

				// JIT: ZVAL_COPY_VALUE(arg, op1);
				jit_ZVAL_COPY(jit,
					arg_addr,
					MAY_BE_ANY,
					ref_addr, op1_info, 0);

				// JIT: if (GC_DELREF(ref) != 0)
				refcount = jit_GC_DELREF(jit, ref);
				if_not_zero = ir_IF(refcount);
				ir_IF_TRUE(if_not_zero);

                // JIT: if (Z_REFCOUNTED_P(arg)
				if_refcounted = jit_if_REFCOUNTED(jit, arg_addr);
				ir_IF_TRUE(if_refcounted);
				// JIT: Z_ADDREF_P(arg)
				jit_GC_ADDREF(jit, jit_Z_PTR(jit, arg_addr));
				ir_END_list(end_inputs);
				ir_IF_FALSE(if_refcounted);
				ir_END_list(end_inputs);

				ir_IF_FALSE(if_not_zero);

				// JIT: efree(ref)
				jit_EFREE(jit, ref, sizeof(zend_reference), op_array, opline);
				ir_END_list(end_inputs);

				ir_IF_FALSE(if_ref);

				// JIT: ZVAL_COPY_VALUE(arg, op1);
				jit_ZVAL_COPY(jit,
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
			jit_ZVAL_COPY(jit,
				arg_addr,
				MAY_BE_ANY,
				op1_addr, op1_info, opline->op1_type == IS_CV);
		}
	}

	if (end_inputs) {
		ir_END_list(end_inputs);
		ir_MERGE_list(end_inputs);
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
					ref = jit_IP(jit);
				} else {
					ref = ir_LOAD_A(jit_EX(call));
				}
				ref = jit_CALL(ref, This.u1.type_info);
				ir_STORE(ref, ir_OR_U32(ir_LOAD_U32(ref), ir_CONST_U32(ZEND_CALL_SEND_ARG_BY_REF)));
			}
		} else {
			if (!TRACE_FRAME_IS_LAST_SEND_BY_VAL(JIT_G(current_frame)->call)) {
				TRACE_FRAME_SET_LAST_SEND_BY_VAL(JIT_G(current_frame)->call);
				// JIT: ZEND_DEL_CALL_FLAG(EX(call), ZEND_CALL_SEND_ARG_BY_REF);
				if (jit->reuse_ip) {
					ref = jit_IP(jit);
				} else {
					ref = ir_LOAD_A(jit_EX(call));
				}
				ref = jit_CALL(ref, This.u1.type_info);
				ir_STORE(ref, ir_AND_U32(ir_LOAD_U32(ref), ir_CONST_U32(~ZEND_CALL_SEND_ARG_BY_REF)));
			}
		}
	} else {
		// JIT: if (QUICK_ARG_SHOULD_BE_SENT_BY_REF(EX(call)->func, arg_num)) {
		uint32_t mask = (ZEND_SEND_BY_REF|ZEND_SEND_PREFER_REF) << ((arg_num + 3) * 2);
		ir_ref rx, if_ref, cold_path;

		if (!zend_jit_reuse_ip(jit)) {
			return 0;
		}

		rx = jit_IP(jit);

		ref = ir_AND_U32(
			ir_LOAD_U32(ir_ADD_OFFSET(ir_LOAD_A(jit_CALL(rx, func)), offsetof(zend_function, quick_arg_flags))),
			ir_CONST_U32(mask));
		if_ref = ir_IF(ref);
		ir_IF_TRUE_cold(if_ref);

		// JIT: ZEND_ADD_CALL_FLAG(EX(call), ZEND_CALL_SEND_ARG_BY_REF);
		ref = jit_CALL(rx, This.u1.type_info);
		ir_STORE(ref, ir_OR_U32(ir_LOAD_U32(ref), ir_CONST_U32(ZEND_CALL_SEND_ARG_BY_REF)));

		cold_path = ir_END();
		ir_IF_FALSE(if_ref);

		// JIT: ZEND_DEL_CALL_FLAG(EX(call), ZEND_CALL_SEND_ARG_BY_REF);
		ref = jit_CALL(rx, This.u1.type_info);
		ir_STORE(ref, ir_AND_U32(ir_LOAD_U32(ref), ir_CONST_U32(~ZEND_CALL_SEND_ARG_BY_REF)));

		ir_MERGE_WITH(cold_path);
	}

	return 1;
}

static int zend_jit_check_undef_args(zend_jit_ctx *jit, const zend_op *opline)
{
	ir_ref call, if_may_have_undef, ret;

	if (jit->reuse_ip) {
		call = jit_IP(jit);
	} else {
		call = ir_LOAD_A(jit_EX(call));
	}

	if_may_have_undef = ir_IF(ir_AND_U8(
		ir_LOAD_U8(ir_ADD_OFFSET(call, offsetof(zend_execute_data, This.u1.type_info) + 3)),
		ir_CONST_U8(ZEND_CALL_MAY_HAVE_UNDEF >> 24)));

	ir_IF_TRUE_cold(if_may_have_undef);
	jit_SET_EX_OPLINE(jit, opline);
	ret = ir_CALL_1(IR_I32, ir_CONST_FC_FUNC(zend_handle_undef_args), call);
	ir_GUARD_NOT(ret, jit_STUB_ADDR(jit, jit_stub_exception_handler));
	ir_MERGE_WITH_EMPTY_FALSE(if_may_have_undef);

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
		jit_STORE_IP(jit, ir_LOAD_A(jit_EX(call)));
	}
	rx = jit_IP(jit);
	zend_jit_stop_reuse_ip(jit);

	jit_SET_EX_OPLINE(jit, opline);

	if (opline->opcode == ZEND_DO_FCALL) {
		if (!func) {
			if (trace) {
				uint32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);

				exit_addr = zend_jit_trace_get_exit_addr(exit_point);
				if (!exit_addr) {
					return 0;
				}

				func_ref = ir_LOAD_A(jit_CALL(rx, func));
				ir_GUARD_NOT(
					ir_AND_U32(
						ir_LOAD_U32(ir_ADD_OFFSET(func_ref, offsetof(zend_op_array, fn_flags))),
						ir_CONST_U32(ZEND_ACC_DEPRECATED)),
					ir_CONST_ADDR(exit_addr));
			}
		}
	}

	if (!jit->delayed_call_level) {
		// JIT: EX(call) = call->prev_execute_data;
		ir_STORE(jit_EX(call),
			(call_level == 1) ? IR_NULL : ir_LOAD_A(jit_CALL(rx, prev_execute_data)));
	}
	delayed_call_chain = 0;
	jit->delayed_call_level = 0;

	// JIT: call->prev_execute_data = execute_data;
	ir_STORE(jit_CALL(rx, prev_execute_data), jit_FP(jit));

	if (!func) {
		if (!func_ref) {
			func_ref = ir_LOAD_A(jit_CALL(rx, func));
		}
	}

	if (opline->opcode == ZEND_DO_FCALL) {
		if (!func) {
			if (!trace) {
				ir_ref if_deprecated, ret;

				if_deprecated = ir_IF(ir_AND_U32(
						ir_LOAD_U32(ir_ADD_OFFSET(func_ref, offsetof(zend_op_array, fn_flags))),
						ir_CONST_U32(ZEND_ACC_DEPRECATED)));
				ir_IF_TRUE_cold(if_deprecated);

				if (GCC_GLOBAL_REGS) {
					ret = ir_CALL(IR_BOOL, ir_CONST_FC_FUNC(zend_jit_deprecated_helper));
				} else {
					ret = ir_CALL_1(IR_BOOL, ir_CONST_FC_FUNC(zend_jit_deprecated_helper), rx);
				}
				ir_GUARD(ret, jit_STUB_ADDR(jit, jit_stub_exception_handler));
				ir_MERGE_WITH_EMPTY_FALSE(if_deprecated);
			}
		} else if (func->common.fn_flags & ZEND_ACC_DEPRECATED) {
			ir_ref ret;

			if (GCC_GLOBAL_REGS) {
				ret = ir_CALL(IR_BOOL, ir_CONST_FC_FUNC(zend_jit_deprecated_helper));
			} else {
				ret = ir_CALL_1(IR_BOOL, ir_CONST_FC_FUNC(zend_jit_deprecated_helper), rx);
			}
			ir_GUARD(ret, jit_STUB_ADDR(jit, jit_stub_exception_handler));
		}
	}

	if (!func
	 && opline->opcode != ZEND_DO_UCALL
	 && opline->opcode != ZEND_DO_ICALL) {
		ir_ref type_ref = ir_LOAD_U8(ir_ADD_OFFSET(func_ref, offsetof(zend_function, type)));
		if_user = ir_IF(ir_EQ(type_ref, ir_CONST_U8(ZEND_USER_FUNCTION)));
		ir_IF_TRUE(if_user);
	}

	if ((!func || func->type == ZEND_USER_FUNCTION)
	 && opline->opcode != ZEND_DO_ICALL) {
		bool recursive_call_through_jmp = 0;

		// JIT: EX(call) = NULL;
		ir_STORE(jit_CALL(rx, call), IR_NULL);

		// JIT: EX(return_value) = RETURN_VALUE_USED(opline) ? EX_VAR(opline->result.var) : 0;
		ir_STORE(jit_CALL(rx, return_value),
			RETURN_VALUE_USED(opline) ?
				jit_ZVAL_ADDR(jit, ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var)) :
				IR_NULL);

		// JIT: EX_LOAD_RUN_TIME_CACHE(op_array);
		if (!func || func->op_array.cache_size) {
			ir_ref run_time_cache;

			if (func && op_array == &func->op_array) {
				/* recursive call */
				run_time_cache = ir_LOAD_A(jit_EX(run_time_cache));
			} else if (func
			 && !(func->op_array.fn_flags & ZEND_ACC_CLOSURE)
			 && ZEND_MAP_PTR_IS_OFFSET(func->op_array.run_time_cache)) {
				run_time_cache = ir_LOAD_A(ir_ADD_OFFSET(ir_LOAD_A(jit_CG(map_ptr_base)),
					(uintptr_t)ZEND_MAP_PTR(func->op_array.run_time_cache)));
			} else if ((func && (func->op_array.fn_flags & ZEND_ACC_CLOSURE)) ||
					(JIT_G(current_frame) &&
					 JIT_G(current_frame)->call &&
					 TRACE_FRAME_IS_CLOSURE_CALL(JIT_G(current_frame)->call))) {
				/* Closures always use direct pointers */
				ir_ref local_func_ref = func_ref ? func_ref : ir_LOAD_A(jit_CALL(rx, func));

				run_time_cache = ir_LOAD_A(ir_ADD_OFFSET(local_func_ref, offsetof(zend_op_array, run_time_cache__ptr)));
			} else {
				ir_ref if_odd, run_time_cache2;
				ir_ref local_func_ref = func_ref ? func_ref : ir_LOAD_A(jit_CALL(rx, func));

				run_time_cache = ir_LOAD_A(ir_ADD_OFFSET(local_func_ref, offsetof(zend_op_array, run_time_cache__ptr)));
				if_odd = ir_IF(ir_AND_A(run_time_cache, ir_CONST_ADDR(1)));
				ir_IF_TRUE(if_odd);

				run_time_cache2 = ir_LOAD_A(ir_ADD_A(run_time_cache, ir_LOAD_A(jit_CG(map_ptr_base))));

				ir_MERGE_WITH_EMPTY_FALSE(if_odd);
				run_time_cache = ir_PHI_2(run_time_cache2, run_time_cache);
			}

			ir_STORE(jit_CALL(rx, run_time_cache), run_time_cache);
		}

		// JIT: EG(current_execute_data) = execute_data = call;
		ir_STORE(jit_EG(current_execute_data), rx);
		jit_STORE_FP(jit, rx);

		// JIT: opline = op_array->opcodes;
		if (func && !unknown_num_args) {

			for (i = call_num_args; i < func->op_array.last_var; i++) {
				uint32_t n = EX_NUM_TO_VAR(i);
				zend_jit_addr var_addr = ZEND_ADDR_MEM_ZVAL(ZREG_RX, n);

				jit_set_Z_TYPE_INFO(jit, var_addr, IS_UNDEF);
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
						jit_LOAD_IP_ADDR(jit, func->op_array.opcodes + num_args);
					} else {
						if (!func_ref) {
							func_ref = ir_LOAD_A(jit_CALL(rx, func));
						}
						ir_ref ip = ir_LOAD_A(ir_ADD_OFFSET(	func_ref, offsetof(zend_op_array, opcodes)));
						if (num_args) {
							ip = ir_ADD_OFFSET(ip, num_args * sizeof(zend_op));
						}
						jit_LOAD_IP(jit, ip);
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
						ip = ir_CONST_ADDR(func->op_array.opcodes);
					} else {
						if (!func_ref) {
							func_ref = ir_LOAD_A(jit_CALL(rx, func));
						}
						ip = ir_LOAD_A(ir_ADD_OFFSET(func_ref, offsetof(zend_op_array, opcodes)));
					}
					jit_LOAD_IP(jit, ip);
				}
				if (GCC_GLOBAL_REGS) {
					ir_CALL(IR_VOID, ir_CONST_FC_FUNC(zend_jit_copy_extra_args_helper));
				} else {
					ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_copy_extra_args_helper), jit_FP(jit));
				}
			}
		} else {
			ir_ref ip;
			ir_ref merge_inputs = IR_UNUSED;

			// JIT: opline = op_array->opcodes
			if (func && zend_accel_in_shm(func->op_array.opcodes)) {
				ip = ir_CONST_ADDR(func->op_array.opcodes);
			} else {
				if (!func_ref) {
					func_ref = ir_LOAD_A(jit_CALL(rx, func));
				}
				ip = ir_LOAD_A(ir_ADD_OFFSET(func_ref, offsetof(zend_op_array, opcodes)));
			}
			jit_LOAD_IP(jit, ip);

			// JIT: num_args = EX_NUM_ARGS();
			ir_ref num_args, first_extra_arg;

			num_args = ir_LOAD_U32(jit_EX(This.u2.num_args));
			if (func) {
				first_extra_arg = ir_CONST_U32(func->op_array.num_args);
			} else {
				// JIT: first_extra_arg = op_array->num_args;
				ZEND_ASSERT(func_ref);
				first_extra_arg = ir_LOAD_U32(ir_ADD_OFFSET(func_ref, offsetof(zend_op_array, num_args)));
			}

			// JIT: if (UNEXPECTED(num_args > first_extra_arg))
			ir_ref if_extra_args = ir_IF(ir_GT(num_args, first_extra_arg));
			ir_IF_TRUE_cold(if_extra_args);
			if (GCC_GLOBAL_REGS) {
				ir_CALL(IR_VOID, ir_CONST_FC_FUNC(zend_jit_copy_extra_args_helper));
			} else {
				ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_copy_extra_args_helper), jit_FP(jit));
			}
			ir_END_list(merge_inputs);
			ir_IF_FALSE(if_extra_args);
			if (!func || (func->op_array.fn_flags & ZEND_ACC_HAS_TYPE_HINTS) == 0) {
				if (!func) {
					// JIT: if (EXPECTED((op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS) == 0))
					ir_ref if_has_type_hints = ir_IF(ir_AND_U32(
						ir_LOAD_U32(ir_ADD_OFFSET(func_ref, offsetof(zend_op_array, fn_flags))),
						ir_CONST_U32(ZEND_ACC_HAS_TYPE_HINTS)));
					ir_IF_TRUE(if_has_type_hints);
					ir_END_list(merge_inputs);
					ir_IF_FALSE(if_has_type_hints);
				}
				// JIT: opline += num_args;

				ir_ref ref = ir_MUL_U32(num_args, ir_CONST_U32(sizeof(zend_op)));

				if (sizeof(void*) == 8) {
					ref = ir_ZEXT_A(ref);
				}

				if (GCC_GLOBAL_REGS) {
					jit_STORE_IP(jit, ir_ADD_A(jit_IP(jit), ref));
				} else {
					ir_ref addr = jit_EX(opline);

					ir_STORE(addr, ir_ADD_A(ir_LOAD_A(addr), ref));
				}
			}

			ir_END_list(merge_inputs);
			ir_MERGE_list(merge_inputs);

			// JIT: if (EXPECTED((int)num_args < op_array->last_var)) {
			ir_ref last_var;

			if (func) {
				last_var = ir_CONST_U32(func->op_array.last_var);
			} else {
				ZEND_ASSERT(func_ref);
				last_var = ir_LOAD_U32(ir_ADD_OFFSET(func_ref, offsetof(zend_op_array, last_var)));
			}

			ir_ref idx = ir_SUB_U32(last_var, num_args);
			ir_ref if_need = ir_IF(ir_GT(idx, ir_CONST_U32(0)));
			ir_IF_TRUE(if_need);

			// JIT: zval *var = EX_VAR_NUM(num_args);
			if (sizeof(void*) == 8) {
				num_args = ir_ZEXT_A(num_args);
			}
			ir_ref var_ref = ir_ADD_OFFSET(
				ir_ADD_A(jit_FP(jit), ir_MUL_A(num_args, ir_CONST_ADDR(sizeof(zval)))),
				(ZEND_CALL_FRAME_SLOT * sizeof(zval)) + offsetof(zval, u1.type_info));

			ir_ref loop = ir_LOOP_BEGIN(ir_END());
			var_ref = ir_PHI_2(var_ref, IR_UNUSED);
			idx = ir_PHI_2(idx, IR_UNUSED);
			ir_STORE(var_ref, ir_CONST_I32(IS_UNDEF));
			ir_PHI_SET_OP(var_ref, 2, ir_ADD_OFFSET(var_ref, sizeof(zval)));
			ir_ref idx2 = ir_SUB_U32(idx, ir_CONST_U32(1));
			ir_PHI_SET_OP(idx, 2, idx2);
			ir_ref if_not_zero = ir_IF(idx2);
			ir_IF_TRUE(if_not_zero);
			ir_MERGE_SET_OP(loop, 2, ir_LOOP_END(loop));
			ir_IF_FALSE(if_not_zero);
			ir_MERGE_WITH_EMPTY_FALSE(if_need);
		}

		if (ZEND_OBSERVER_ENABLED) {
			if (GCC_GLOBAL_REGS) {
				// EX(opline) = opline
				ir_STORE(jit_EX(opline), jit_IP(jit));
			}
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_observer_fcall_begin), jit_FP(jit));
		}

		if (trace) {
			if (!func && (opline->opcode != ZEND_DO_UCALL)) {
				user_path = ir_END();
			}
		} else {
			if (GCC_GLOBAL_REGS) {
				if (recursive_call_through_jmp) {
					// TODO: use direct jmp ???
					ir_TAILCALL(ir_LOAD_A(jit_IP(jit)));
//???+++				|	jmp =>num_args
				} else {
					ir_TAILCALL(ir_LOAD_A(jit_IP(jit)));
				}
			} else {
				ir_RETURN(ir_CONST_I32(1));
			}
			if (func || (opline->opcode == ZEND_DO_UCALL)) {
				if (jit->b >= 0) {
					zend_basic_block *bb = &jit->ssa->cfg.blocks[jit->b];

					if (bb->successors_count > 0) {
						int succ;

						ZEND_ASSERT(bb->successors_count == 1);
						succ = bb->successors[0];
						_zend_jit_add_predecessor_ref(jit, succ, jit->b, jit->ctx.control);
					}
					jit->ctx.control = IR_UNUSED;
					jit->b = -1;
				}
			}
		}
	}

	if ((!func || func->type == ZEND_INTERNAL_FUNCTION)
	 && (opline->opcode != ZEND_DO_UCALL)) {
		if (!func && (opline->opcode != ZEND_DO_ICALL)) {
			ir_IF_FALSE(if_user);
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
					ir_GUARD_NOT(
						ir_AND_U32(
							ir_LOAD_U32(ir_ADD_OFFSET(func_ref, offsetof(zend_op_array, fn_flags))),
							ir_CONST_U32(ZEND_ACC_DEPRECATED)),
						ir_CONST_ADDR(exit_addr));
				} else {
					ir_ref if_deprecated, ret;

					if_deprecated = ir_IF(ir_AND_U32(
						ir_LOAD_U32(ir_ADD_OFFSET(func_ref, offsetof(zend_op_array, fn_flags))),
						ir_CONST_U32(ZEND_ACC_DEPRECATED)));
					ir_IF_TRUE_cold(if_deprecated);

					if (GCC_GLOBAL_REGS) {
						ret = ir_CALL(IR_BOOL, ir_CONST_FC_FUNC(zend_jit_deprecated_helper));
					} else {
						ret = ir_CALL_1(IR_BOOL, ir_CONST_FC_FUNC(zend_jit_deprecated_helper), rx);
					}
					ir_GUARD(ret, jit_STUB_ADDR(jit, jit_stub_exception_handler));
					ir_MERGE_WITH_EMPTY_FALSE(if_deprecated);
				}
			} else if (func->common.fn_flags & ZEND_ACC_DEPRECATED) {
				ir_ref ret;

				if (GCC_GLOBAL_REGS) {
					ret = ir_CALL(IR_BOOL, ir_CONST_FC_FUNC(zend_jit_deprecated_helper));
				} else {
					ret = ir_CALL_1(IR_BOOL, ir_CONST_FC_FUNC(zend_jit_deprecated_helper), rx);
				}
				ir_GUARD(ret, jit_STUB_ADDR(jit, jit_stub_exception_handler));
			}
		}

		// JIT: EG(current_execute_data) = execute_data;
		ir_STORE(jit_EG(current_execute_data), rx);

		if (ZEND_OBSERVER_ENABLED) {
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_observer_fcall_begin), rx);
		}

		// JIT: ZVAL_NULL(EX_VAR(opline->result.var));
		ir_ref res_addr = IR_UNUSED, func_ptr;

		if (RETURN_VALUE_USED(opline)) {
			res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var);
		} else {
			/* CPU stack allocated temporary zval */
			ir_ref ptr;

			if (!jit->ctx.fixed_call_stack_size) {
				// JIT: alloca(sizeof(void*));
				ptr = ir_ALLOCA(ir_CONST_ADDR(sizeof(zval)));
			} else {
				ptr = ir_HARD_COPY_A(ir_RLOAD_A(IR_REG_SP));
			}
			res_addr = ZEND_ADDR_REF_ZVAL(ptr);
		}

		jit_set_Z_TYPE_INFO(jit, res_addr, IS_NULL);

		zend_jit_reset_last_valid_opline(jit);

		// JIT: fbc->internal_function.handler(call, ret);
		if (func) {
			func_ptr = ir_CONST_FC_FUNC(func->internal_function.handler);
		} else {
			func_ptr = ir_LOAD_A(ir_ADD_OFFSET(func_ref, offsetof(zend_internal_function, handler)));
#if defined(IR_TARGET_X86)
			func_ptr = ir_CAST_FC_FUNC(func_ptr);
#endif
		}
		ir_CALL_2(IR_VOID, func_ptr, rx, jit_ZVAL_ADDR(jit, res_addr));

		if (ZEND_OBSERVER_ENABLED) {
			ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_observer_fcall_end),
				rx, jit_ZVAL_ADDR(jit, res_addr));
		}

		// JIT: EG(current_execute_data) = execute_data;
		ir_STORE(jit_EG(current_execute_data), jit_FP(jit));

		// JIT: zend_vm_stack_free_args(call);
		if (func && !unknown_num_args) {
			for (i = 0; i < call_num_args; i++ ) {
				if (zend_jit_needs_arg_dtor(func, i, call_info)) {
					uint32_t offset = EX_NUM_TO_VAR(i);
					zend_jit_addr var_addr = ZEND_ADDR_MEM_ZVAL(ZREG_RX, offset);

					jit_ZVAL_PTR_DTOR(jit, var_addr, MAY_BE_ANY|MAY_BE_RC1|MAY_BE_RCN, 0, opline);
				}
			}
		} else {
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_vm_stack_free_args_helper), rx);
		}

		if (may_have_extra_named_params) {
			// JIT: if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS))
			ir_ref if_has_named = ir_IF(ir_AND_U8(
				ir_LOAD_U8(ir_ADD_OFFSET(rx, offsetof(zend_execute_data, This.u1.type_info) + 3)),
				ir_CONST_U8(ZEND_CALL_HAS_EXTRA_NAMED_PARAMS >> 24)));
			ir_IF_TRUE_cold(if_has_named);

			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_free_extra_named_params),
				ir_LOAD_A(jit_CALL(rx, extra_named_params)));

			ir_MERGE_WITH_EMPTY_FALSE(if_has_named);
		}

		if (opline->opcode == ZEND_DO_FCALL) {
			// TODO: optimize ???
			// JIT: if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_RELEASE_THIS))
			ir_ref if_release_this = ir_IF(ir_AND_U8(
				ir_LOAD_U8(ir_ADD_OFFSET(rx, offsetof(zend_execute_data, This.u1.type_info) + 2)),
				ir_CONST_U8(ZEND_CALL_RELEASE_THIS >> 16)));
			ir_IF_TRUE_cold(if_release_this);

			// JIT: OBJ_RELEASE(Z_OBJ(RX->This));
			jit_OBJ_RELEASE(jit, ir_LOAD_A(jit_CALL(rx, This.value.obj)));

			ir_MERGE_WITH_EMPTY_FALSE(if_release_this);
		}


		ir_ref allocated_path = IR_UNUSED;

		if (JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE ||
		    !JIT_G(current_frame) ||
		    !JIT_G(current_frame)->call ||
		    !TRACE_FRAME_IS_NESTED(JIT_G(current_frame)->call) ||
		    prev_opline->opcode == ZEND_SEND_UNPACK ||
		    prev_opline->opcode == ZEND_SEND_ARRAY ||
			prev_opline->opcode == ZEND_CHECK_UNDEF_ARGS) {

			// JIT: zend_vm_stack_free_call_frame(call);
			// JIT: if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_ALLOCATED))
			ir_ref if_allocated = ir_IF(ir_AND_U8(
				ir_LOAD_U8(ir_ADD_OFFSET(rx, offsetof(zend_execute_data, This.u1.type_info) + 2)),
				ir_CONST_U8(ZEND_CALL_ALLOCATED >> 16)));
			ir_IF_TRUE_cold(if_allocated);

			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_free_call_frame), rx);

			allocated_path = ir_END();
			ir_IF_FALSE(if_allocated);
		}

		ir_STORE(jit_EG(vm_stack_top), rx);

		if (allocated_path) {
			ir_MERGE_WITH(allocated_path);
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
				ir_ref sp = ir_RLOAD_A(IR_REG_SP);
				res_addr = ZEND_ADDR_REF_ZVAL(sp);
				jit_ZVAL_PTR_DTOR(jit, res_addr, func_info, 1, opline);
			}
			if (!jit->ctx.fixed_call_stack_size) {
				// JIT: revert alloca
				ir_AFREE(ir_CONST_ADDR(sizeof(zval)));
			}
		}

		// JIT: if (UNEXPECTED(EG(exception) != NULL)) {
		ir_GUARD_NOT(ir_LOAD_A(jit_EG_exception(jit)),
			jit_STUB_ADDR(jit, jit_stub_icall_throw));

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
			jit_LOAD_IP_ADDR(jit, opline + 1);
		} else if (trace
		 && trace->op == ZEND_JIT_TRACE_END
		 && trace->stop == ZEND_JIT_TRACE_STOP_INTERPRETER) {
			jit_LOAD_IP_ADDR(jit, opline + 1);
		}
	}

	if (user_path) {
		ir_MERGE_WITH(user_path);
	}

	return 1;
}

static int zend_jit_constructor(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, zend_ssa *ssa, int call_level, int next_block)
{
	ir_ref if_skip_constructor = jit_IF_ex(jit, jit_CMP_IP(jit, IR_NE, opline), next_block);

	ir_IF_FALSE(if_skip_constructor);

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
    if (!jit->ctx.control) {
		jit->bb_edges[jit->bb_predecessors[next_block]] = if_skip_constructor;
	} else {
		/* merge current control path with the true branch of constructor skip condition */
		ir_MERGE_WITH_EMPTY_TRUE(if_skip_constructor);
		jit->bb_edges[jit->bb_predecessors[next_block]] = ir_END();

		jit->ctx.control = IR_UNUSED;
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

	ref = jit_ZVAL_ADDR(jit, res_addr);
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
			ref = jit_ZVAL_DEREF_ref(jit, ref);
		} else {
			ref = jit_Z_PTR_ref(jit, ref);
			ref = ir_ADD_OFFSET(ref, offsetof(zend_reference, val));
		}
	}

	if (type_mask != 0) {
		if (is_power_of_two(type_mask)) {
			uint32_t type_code = concrete_type(type_mask);
			ir_ref if_ok = jit_if_Z_TYPE_ref(jit, ref, ir_CONST_U8(type_code));
			ir_IF_TRUE(if_ok);
			fast_path = ir_END();
			ir_IF_FALSE_cold(if_ok);
		} else {
			ir_ref if_ok = ir_IF(ir_AND_U32(
				ir_SHL_U32(ir_CONST_U32(1), jit_Z_TYPE_ref(jit, ref)),
				ir_CONST_U32(type_mask)));
			ir_IF_TRUE(if_ok);
			fast_path = ir_END();
			ir_IF_FALSE_cold(if_ok);
		}
	}

	jit_SET_EX_OPLINE(jit, opline);
	ref = ir_CALL_2(IR_BOOL, ir_CONST_FC_FUNC(zend_jit_verify_arg_slow),
		ref, ir_CONST_ADDR(arg_info));

	if (check_exception) {
		ir_GUARD(ref, jit_STUB_ADDR(jit, jit_stub_exception_handler));
	}

	if (fast_path) {
		ir_MERGE_WITH(fast_path);
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
				ir_GUARD(ir_GE(ir_LOAD_U32(jit_EX(This.u2.num_args)), ir_CONST_U32(arg_num)),
					ir_CONST_ADDR(exit_addr));
			}
		} else {
			ir_ref if_ok =ir_IF(ir_GE(ir_LOAD_U32(jit_EX(This.u2.num_args)), ir_CONST_U32(arg_num)));
			ir_IF_FALSE_cold(if_ok);

			jit_SET_EX_OPLINE(jit, opline);
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_missing_arg_error), jit_FP(jit));
			ir_IJMP(jit_STUB_ADDR(jit, jit_stub_exception_handler));
			ir_IF_TRUE(if_ok);
		}
	}

	if (arg_info) {
		if (!zend_jit_verify_arg_type(jit, opline, arg_info, 1)) {
			return 0;
		}
	}

	if (JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE) {
		if ((opline+1)->opcode != ZEND_RECV && (opline+1)->opcode != ZEND_RECV_INIT) {
			jit_LOAD_IP_ADDR(jit, opline + 1);
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
			jit_ZVAL_COPY_CONST(jit,
				res_addr,
				-1, -1,
				zv, 1);
		}
	} else {
		if (JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE ||
		    (op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
			ir_ref if_skip = ir_IF(ir_GE(ir_LOAD_U32(jit_EX(This.u2.num_args)), ir_CONST_U32(arg_num)));
			ir_IF_TRUE(if_skip);
			skip_path = ir_END();
			ir_IF_FALSE(if_skip);
		}
		jit_ZVAL_COPY_CONST(jit,
			res_addr,
			-1, -1,
			zv, 1);
	}

	if (Z_CONSTANT_P(zv)) {
		jit_SET_EX_OPLINE(jit, opline);
		ref = ir_CALL_2(IR_I32, ir_CONST_FC_FUNC(zval_update_constant_ex),
			jit_ZVAL_ADDR(jit, res_addr),
			ir_LOAD_A(ir_ADD_OFFSET(ir_LOAD_A(jit_EX(func)), offsetof(zend_op_array, scope))));

		if_fail = ir_IF(ref);
		ir_IF_TRUE_cold(if_fail);
		jit_ZVAL_PTR_DTOR(jit, res_addr, MAY_BE_ANY|MAY_BE_RC1|MAY_BE_RCN, 1, opline);
		ir_IJMP(jit_STUB_ADDR(jit, jit_stub_exception_handler));
		ir_IF_FALSE(if_fail);
	}

	if (skip_path) {
		ir_MERGE_WITH(skip_path);
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
			jit_LOAD_IP_ADDR(jit, opline + 1);
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
			ir_ref if_ok = jit_if_Z_TYPE(jit, op1_addr, type_code);

			ir_IF_TRUE(if_ok);
			fast_path = ir_END();
			ir_IF_FALSE_cold(if_ok);
		} else {
			ir_ref if_ok = ir_IF(ir_AND_U32(
				ir_SHL_U32(ir_CONST_U32(1), jit_Z_TYPE(jit, op1_addr)),
				ir_CONST_U32(type_mask)));

			ir_IF_TRUE(if_ok);
			fast_path = ir_END();
			ir_IF_FALSE_cold(if_ok);
		}
	}
	if (needs_slow_check) {
		ir_ref ref;

		jit_SET_EX_OPLINE(jit, opline);
		ref = jit_ZVAL_ADDR(jit, op1_addr);
		if (op1_info & MAY_BE_UNDEF) {
			ref = zend_jit_zval_check_undef(jit, ref, opline->op1.var, NULL, 1);
		}

		ir_CALL_4(IR_VOID, ir_CONST_FC_FUNC(zend_jit_verify_return_slow),
			ref,
			ir_LOAD_A(jit_EX(func)),
			ir_CONST_ADDR(arg_info),
			ir_ADD_OFFSET(ir_LOAD_A(jit_EX(run_time_cache)), opline->op2.num));

		zend_jit_check_exception(jit);

		if (fast_path) {
			ir_MERGE_WITH(fast_path);
		}
	}

	return 1;
}

static int zend_jit_leave_frame(zend_jit_ctx *jit)
{
	// JIT: EG(current_execute_data) = EX(prev_execute_data);
	ir_STORE(jit_EG(current_execute_data), ir_LOAD_A(jit_EX(prev_execute_data)));
	return 1;
}

static int zend_jit_free_cvs(zend_jit_ctx *jit)
{
	// JIT: EG(current_execute_data) = EX(prev_execute_data);
	ir_STORE(jit_EG(current_execute_data), ir_LOAD_A(jit_EX(prev_execute_data)));

	// JIT: zend_free_compiled_variables(execute_data);
	ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_free_compiled_variables), jit_FP(jit));
	return 1;
}

static int zend_jit_free_cv(zend_jit_ctx *jit, uint32_t info, uint32_t var)
{
	if (info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF)) {
		zend_jit_addr var_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, EX_NUM_TO_VAR(var));

		jit_ZVAL_PTR_DTOR(jit, var_addr, info, 1, NULL);
	}
	return 1;
}

static int zend_jit_free_op(zend_jit_ctx *jit, const zend_op *opline, uint32_t info, uint32_t var_offset)
{
	if (info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF)) {
		jit_ZVAL_PTR_DTOR(jit, ZEND_ADDR_MEM_ZVAL(ZREG_FP, var_offset), info, 0, opline);
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

	call_info = ir_LOAD_U32(jit_EX(This.u1.type_info));
	if (may_need_call_helper) {
		if (!left_frame) {
			left_frame = 1;
		    if (!zend_jit_leave_frame(jit)) {
				return 0;
		    }
		}
		/* ZEND_CALL_FAKE_CLOSURE handled on slow path to eliminate check for ZEND_CALL_CLOSURE on fast path */
		ref = ir_AND_U32(call_info,
			ir_CONST_U32(ZEND_CALL_TOP|ZEND_CALL_HAS_SYMBOL_TABLE|ZEND_CALL_FREE_EXTRA_ARGS|ZEND_CALL_ALLOCATED|ZEND_CALL_HAS_EXTRA_NAMED_PARAMS|ZEND_CALL_FAKE_CLOSURE));
		if (trace && trace->op != ZEND_JIT_TRACE_END) {
			ir_ref if_slow = ir_IF(ref);

			ir_IF_TRUE_cold(if_slow);
			if (!GCC_GLOBAL_REGS) {
				ref = ir_CALL_1(IR_I32, ir_CONST_FC_FUNC(zend_jit_leave_func_helper), jit_FP(jit));
			} else {
				ir_CALL(IR_VOID, ir_CONST_FC_FUNC(zend_jit_leave_func_helper));
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
					ir_GUARD(jit_IP(jit), jit_STUB_ADDR(jit, jit_stub_trace_halt));
				} else {
					ir_GUARD(ir_GE(ref, ir_CONST_I32(0)), jit_STUB_ADDR(jit, jit_stub_trace_halt));
				}
			}

			if (!GCC_GLOBAL_REGS) {
				// execute_data = EG(current_execute_data)
				jit_STORE_FP(jit, ir_LOAD_A(jit_EG(current_execute_data)));
			}
			cold_path = ir_END();
			ir_IF_FALSE(if_slow);
		} else {
			ir_GUARD_NOT(ref, jit_STUB_ADDR(jit, jit_stub_leave_function_handler));
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
		jit_OBJ_RELEASE(jit, ir_ADD_OFFSET(ir_LOAD_A(jit_EX(func)), -sizeof(zend_object)));
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
			if_release = ir_IF(ir_AND_U32(call_info, ir_CONST_U32(ZEND_CALL_RELEASE_THIS)));
			ir_IF_FALSE(if_release);
			fast_path = ir_END();
			ir_IF_TRUE(if_release);
		}
		// JIT: OBJ_RELEASE(execute_data->This))
		jit_OBJ_RELEASE(jit, ir_LOAD_A(jit_EX(This.value.obj)));
		if (fast_path) {
			ir_MERGE_WITH(fast_path);
		}
		// TODO: avoid EG(excption) check for $this->foo() calls
		may_throw = 1;
	}

	// JIT: EG(vm_stack_top) = (zval*)execute_data
	ir_STORE(jit_EG(vm_stack_top), jit_FP(jit));

	// JITL execute_data = EX(prev_execute_data)
	jit_STORE_FP(jit, ir_LOAD_A(jit_EX(prev_execute_data)));

	if (!left_frame) {
		// JIT: EG(current_execute_data) = execute_data
		ir_STORE(jit_EG(current_execute_data), jit_FP(jit));
	}

	if (trace) {
		if (trace->op != ZEND_JIT_TRACE_END
		 && (JIT_G(current_frame) && !TRACE_FRAME_IS_UNKNOWN_RETURN(JIT_G(current_frame)))) {
			zend_jit_reset_last_valid_opline(jit);
		} else {
			if (GCC_GLOBAL_REGS) {
				/* We add extra RLOAD and RSTORE to make fusion for persistent register
				 *     mov (%FP), %IP
				 *     add $0x1c, %IP
				 * The naive (commented) code leads to extra register allocation and move.
				 *     mov (%FP), %tmp
				 *     add $0x1c, %tmp
				 *     mov %tmp, %FP
				 */
#if 0
				jit_STORE_IP(jit, ir_ADD_OFFSET(ir_LOAD_A(jit_EX(opline)), sizeof(zend_op)));
#else
				jit_STORE_IP(jit, ir_LOAD_A(jit_EX(opline)));
				jit_STORE_IP(jit, ir_ADD_OFFSET(jit_IP(jit), sizeof(zend_op)));
#endif
			} else {
				ir_ref ref = jit_EX(opline);

				ir_STORE(ref, ir_ADD_OFFSET(ir_LOAD_A(ref), sizeof(zend_op)));
			}
		}

		if (cold_path) {
			ir_MERGE_WITH(cold_path);
		}

		if (trace->op == ZEND_JIT_TRACE_BACK
		 && (!JIT_G(current_frame) || TRACE_FRAME_IS_UNKNOWN_RETURN(JIT_G(current_frame)))) {
			const zend_op *next_opline = trace->opline;

			if ((opline->op1_type & (IS_VAR|IS_TMP_VAR))
			 && (op1_info & MAY_BE_RC1)
			 && (op1_info & (MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_ARRAY))) {
				/* exception might be thrown during destruction of unused return value */
				// JIT: if (EG(exception))
				ir_GUARD_NOT(ir_LOAD_A(jit_EG(exception)), jit_STUB_ADDR(jit, jit_stub_leave_throw));
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

				ir_ref if_eq = ir_IF(jit_CMP_IP(jit, IR_EQ, next_opline));

				ir_IF_TRUE(if_eq);
				ZEND_ASSERT(jit->trace_loop_ref);
				ZEND_ASSERT(jit->ctx.ir_base[jit->trace_loop_ref].op2 == IR_UNUSED);
				ir_MERGE_SET_OP(jit->trace_loop_ref, 2, ir_END());
				ir_IF_FALSE(if_eq);

#ifdef ZEND_VM_HYBRID_JIT_RED_ZONE_SIZE
				ir_TAILCALL(ir_LOAD_A(jit_IP(jit)));
#else
				ir_IJMP(jit_STUB_ADDR(jit, jit_stub_trace_escape));
#endif
			} else {
				ir_GUARD(jit_CMP_IP(jit, IR_EQ, next_opline), jit_STUB_ADDR(jit, jit_stub_trace_escape));
			}

			zend_jit_set_last_valid_opline(jit, trace->opline);

			return 1;
		} else if (may_throw ||
				(((opline->op1_type & (IS_VAR|IS_TMP_VAR))
				  && (op1_info & MAY_BE_RC1)
				  && (op1_info & (MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_ARRAY)))
				 && (!JIT_G(current_frame) || TRACE_FRAME_IS_RETURN_VALUE_UNUSED(JIT_G(current_frame))))) {
			// JIT: if (EG(exception))
			ir_GUARD_NOT(ir_LOAD_A(jit_EG(exception)), jit_STUB_ADDR(jit, jit_stub_leave_throw));
		}

		return 1;
	} else {
		// JIT: if (EG(exception))
		ir_GUARD_NOT(ir_LOAD_A(jit_EG(exception)), jit_STUB_ADDR(jit, jit_stub_leave_throw));
		// JIT: opline = EX(opline) + 1
		if (GCC_GLOBAL_REGS) {
			jit_STORE_IP(jit, ir_LOAD_A(jit_EX(opline)));
			jit_STORE_IP(jit, ir_ADD_OFFSET(jit_IP(jit), sizeof(zend_op)));
		} else {
			ir_ref ref = jit_EX(opline);

			ir_STORE(ref, ir_ADD_OFFSET(ir_LOAD_A(ref), sizeof(zend_op)));
		}
	}

	if (GCC_GLOBAL_REGS) {
		ir_TAILCALL(ir_LOAD_A(jit_IP(jit)));
	} else {
		ir_RETURN(ir_CONST_I32(2)); // ZEND_VM_LEAVE
	}

	jit->ctx.control = IR_UNUSED;
	jit->b = -1;

	return 1;
}

static void zend_jit_common_return(zend_jit_ctx *jit)
{
	ZEND_ASSERT(jit->return_inputs);
	ir_MERGE_list(jit->return_inputs);
}

static int zend_jit_return(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, uint32_t op1_info, zend_jit_addr op1_addr)
{
	zend_jit_addr ret_addr;
	int8_t return_value_used = -1;
	ir_ref return_value = IR_UNUSED, ref, refcount, if_return_value_used = IR_UNUSED;

	ZEND_ASSERT(op_array->type != ZEND_EVAL_CODE && op_array->function_name);
	ZEND_ASSERT(!(op1_info & MAY_BE_UNDEF));

	if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
		jit->return_inputs = IR_UNUSED;
		if (JIT_G(current_frame)) {
			if (TRACE_FRAME_IS_RETURN_VALUE_USED(JIT_G(current_frame))) {
				return_value_used = 1;
			} else if (TRACE_FRAME_IS_RETURN_VALUE_UNUSED(JIT_G(current_frame))) {
				return_value_used = 0;
			} else {
				return_value_used = -1;
			}
		}
	}

	if (ZEND_OBSERVER_ENABLED) {
		if (Z_MODE(op1_addr) == IS_REG) {
			zend_jit_addr dst = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->op1.var);

			if (!zend_jit_spill_store(jit, op1_addr, dst, op1_info, 1)) {
				return 0;
			}
			op1_addr = dst;
		}
		ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_observer_fcall_end),
			jit_FP(jit),
			jit_ZVAL_ADDR(jit, op1_addr));
	}

	// JIT: if (!EX(return_value))
	return_value = ir_LOAD_A(jit_EX(return_value));
	ret_addr = ZEND_ADDR_REF_ZVAL(return_value);
	if ((opline->op1_type & (IS_VAR|IS_TMP_VAR)) &&
	    (op1_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
		if (return_value_used == -1) {
			if_return_value_used = ir_IF(return_value);
			ir_IF_FALSE_cold(if_return_value_used);
		}
		if (return_value_used != 1) {
			if (op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)-(MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
				ir_ref if_refcounted = jit_if_REFCOUNTED(jit, op1_addr);
				ir_IF_FALSE(if_refcounted);
				ir_END_list(jit->return_inputs);
				ir_IF_TRUE(if_refcounted);
			}
			ref = jit_Z_PTR(jit, op1_addr);
			refcount = jit_GC_DELREF(jit, ref);

			if (RC_MAY_BE_1(op1_info)) {
				if (RC_MAY_BE_N(op1_info)) {
					ir_ref if_non_zero = ir_IF(refcount);
					ir_IF_TRUE(if_non_zero);
					ir_END_list(jit->return_inputs);
					ir_IF_FALSE(if_non_zero);
				}
				jit_ZVAL_DTOR(jit, ref, op1_info, opline);
			}
			if (return_value_used == -1) {
				ir_END_list(jit->return_inputs);
			}
		}
	} else if (return_value_used == -1) {
		if_return_value_used = ir_IF(return_value);
		ir_IF_FALSE_cold(if_return_value_used);
		ir_END_list(jit->return_inputs);
	}

	if (if_return_value_used) {
		ir_IF_TRUE(if_return_value_used);
	}

	if (return_value_used == 0) {
		if (jit->return_inputs) {
			ZEND_ASSERT(JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE);
			ir_END_list(jit->return_inputs);
			ir_MERGE_list(jit->return_inputs);
			jit->return_inputs = IR_UNUSED;
		}
		return 1;
	}

	if (opline->op1_type == IS_CONST) {
		zval *zv = RT_CONSTANT(opline, opline->op1);

		jit_ZVAL_COPY_CONST(jit, ret_addr, MAY_BE_ANY, MAY_BE_ANY, zv, 1);
	} else if (opline->op1_type == IS_TMP_VAR) {
		jit_ZVAL_COPY(jit, ret_addr, MAY_BE_ANY, op1_addr, op1_info, 0);
	} else if (opline->op1_type == IS_CV) {
		if (op1_info & MAY_BE_REF) {
			ref = jit_ZVAL_ADDR(jit, op1_addr);
			ref = jit_ZVAL_DEREF_ref(jit, ref);
			op1_addr = ZEND_ADDR_REF_ZVAL(ref);
		}

		if (op1_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
			if (JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE ||
			    (op1_info & (MAY_BE_REF|MAY_BE_OBJECT)) ||
			    !op_array->function_name) {
				jit_ZVAL_COPY(jit, ret_addr, MAY_BE_ANY, op1_addr, op1_info, 1);
			} else if (return_value_used != 1) {
				jit_ZVAL_COPY(jit, ret_addr, MAY_BE_ANY, op1_addr, op1_info, 0);
				// JIT: if (EXPECTED(!(EX_CALL_INFO() & ZEND_CALL_CODE))) ZVAL_NULL(retval_ptr);
				jit_set_Z_TYPE_INFO(jit, op1_addr, IS_NULL);
			} else {
				jit_ZVAL_COPY(jit, ret_addr, MAY_BE_ANY, op1_addr, op1_info, 0);
			}
		} else {
			jit_ZVAL_COPY(jit, ret_addr, MAY_BE_ANY, op1_addr, op1_info, 0);
		}
	} else {
		if (op1_info & MAY_BE_REF) {
			ir_ref if_ref, ref2, if_non_zero;
			zend_jit_addr ref_addr;

			if_ref = jit_if_Z_TYPE(jit, op1_addr, IS_REFERENCE);
			ir_IF_TRUE_cold(if_ref);

			// JIT: zend_refcounted *ref = Z_COUNTED_P(retval_ptr)
			ref = jit_Z_PTR(jit, op1_addr);

			// JIT: ZVAL_COPY_VALUE(return_value, &ref->value)
			ref2 = ir_ADD_OFFSET(ref, offsetof(zend_reference, val));
			ref_addr = ZEND_ADDR_REF_ZVAL(ref2);
			jit_ZVAL_COPY(jit, ret_addr, MAY_BE_ANY, ref_addr, op1_info, 0);
			ref2 = jit_GC_DELREF(jit, ref);
			if_non_zero = ir_IF(ref2);
			ir_IF_TRUE(if_non_zero);

			// JIT: if (IS_REFCOUNTED())
			ir_ref if_refcounted = jit_if_REFCOUNTED(jit, ret_addr);
			ir_IF_FALSE(if_refcounted);
			ir_END_list(jit->return_inputs);
			ir_IF_TRUE(if_refcounted);

			// JIT: ADDREF
			ref2 = jit_Z_PTR(jit, ret_addr);
			jit_GC_ADDREF(jit, ref2);
			ir_END_list(jit->return_inputs);

			ir_IF_FALSE(if_non_zero);

			jit_EFREE(jit, ref, sizeof(zend_reference), op_array, opline);
			ir_END_list(jit->return_inputs);

			ir_IF_FALSE(if_ref);
		}
		jit_ZVAL_COPY(jit, ret_addr, MAY_BE_ANY, op1_addr, op1_info, 0);
	}

	if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
		if (jit->return_inputs) {
			ir_END_list(jit->return_inputs);
			ir_MERGE_list(jit->return_inputs);
			jit->return_inputs = IR_UNUSED;
		}
	} else {
		ir_END_list(jit->return_inputs);
		jit->ctx.control = IR_UNUSED;
		jit->b = -1;
	}

	return 1;
}

static int zend_jit_bind_global(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info)
{
	zend_jit_addr op1_addr = OP1_ADDR();
	zend_string *varname = Z_STR_P(RT_CONSTANT(opline, opline->op2));
	ir_ref cache_slot_ref, idx_ref, num_used_ref, bucket_ref, ref, ref2;
	ir_ref if_fit, if_reference, if_same_key, fast_path;
	ir_ref slow_inputs = IR_UNUSED, end_inputs = IR_UNUSED;

	// JIT: idx = (uintptr_t)CACHED_PTR(opline->extended_value) - 1;
	cache_slot_ref = ir_ADD_OFFSET(ir_LOAD_A(jit_EX(run_time_cache)), opline->extended_value);
	idx_ref = ir_SUB_A(ir_LOAD_A(cache_slot_ref), ir_CONST_ADDR(1));

	// JIT: if (EXPECTED(idx < EG(symbol_table).nNumUsed * sizeof(Bucket)))
	num_used_ref = ir_MUL_U32(ir_LOAD_U32(jit_EG(symbol_table.nNumUsed)),
		ir_CONST_U32(sizeof(Bucket)));
	if (sizeof(void*) == 8) {
		num_used_ref = ir_ZEXT_A(num_used_ref);
	}
	if_fit = ir_IF(ir_ULT(idx_ref, num_used_ref));
	ir_IF_FALSE_cold(if_fit);
	ir_END_list(slow_inputs);
	ir_IF_TRUE(if_fit);

	// JIT: Bucket *p = (Bucket*)((char*)EG(symbol_table).arData + idx);
	bucket_ref = ir_ADD_A(ir_LOAD_A(jit_EG(symbol_table.arData)), idx_ref);
	if_reference = jit_if_Z_TYPE_ref(jit, bucket_ref, ir_CONST_U8(IS_REFERENCE));
	ir_IF_FALSE_cold(if_reference);
	ir_END_list(slow_inputs);
	ir_IF_TRUE(if_reference);

	// JIT: (EXPECTED(p->key == varname))
	if_same_key = ir_IF(ir_EQ(ir_LOAD_A(ir_ADD_OFFSET(bucket_ref, offsetof(Bucket, key))), ir_CONST_ADDR(varname)));
	ir_IF_FALSE_cold(if_same_key);
	ir_END_list(slow_inputs);
	ir_IF_TRUE(if_same_key);

	// JIT: GC_ADDREF(Z_PTR(p->val))
	ref = jit_Z_PTR_ref(jit, bucket_ref);
	jit_GC_ADDREF(jit, ref);

	fast_path = ir_END();
	ir_MERGE_list(slow_inputs);

	ref2 = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_fetch_global_helper),
		ir_CONST_ADDR(varname),
		cache_slot_ref);

	ir_MERGE_WITH(fast_path);
	ref = ir_PHI_2(ref2, ref);

	if (op1_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF)) {
		ir_ref if_refcounted = IR_UNUSED, refcount, if_non_zero, if_may_not_leak;

		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
			// JIT: if (UNEXPECTED(Z_REFCOUNTED_P(variable_ptr)))
			if_refcounted = jit_if_REFCOUNTED(jit, op1_addr);
			ir_IF_TRUE_cold(if_refcounted);
		}

		// JIT:zend_refcounted *garbage = Z_COUNTED_P(variable_ptr);
		ref2 = jit_Z_PTR(jit, op1_addr);

		// JIT: ZVAL_REF(variable_ptr, ref)
		jit_set_Z_PTR(jit, op1_addr, ref);
		jit_set_Z_TYPE_INFO(jit, op1_addr, IS_REFERENCE_EX);

		// JIT: if (GC_DELREF(garbage) == 0)
		refcount = jit_GC_DELREF(jit, ref2);
		if_non_zero = ir_IF(refcount);
		if (!(op1_info & (MAY_BE_REF|MAY_BE_ARRAY|MAY_BE_OBJECT))) {
			ir_IF_TRUE(if_non_zero);
			ir_END_list(end_inputs);
		}
		ir_IF_FALSE(if_non_zero);

		jit_ZVAL_DTOR(jit, ref2, op1_info, opline);
		if (op1_info & (MAY_BE_REF|MAY_BE_ARRAY|MAY_BE_OBJECT)) {
			ir_END_list(end_inputs);
			ir_IF_TRUE(if_non_zero);

			// JIT: GC_ZVAL_CHECK_POSSIBLE_ROOT(variable_ptr)
			if_may_not_leak = jit_if_GC_MAY_NOT_LEAK(jit, ref2);
			ir_IF_TRUE(if_may_not_leak);
			ir_END_list(end_inputs);
			ir_IF_FALSE(if_may_not_leak);
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(gc_possible_root), ref2);
		}
		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
			ir_END_list(end_inputs);
			ir_IF_FALSE(if_refcounted);
		}
	}

	if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
		// JIT: ZVAL_REF(variable_ptr, ref)
		jit_set_Z_PTR(jit, op1_addr, ref);
		jit_set_Z_TYPE_INFO(jit, op1_addr, IS_REFERENCE_EX);
	}

	if (end_inputs) {
		ir_END_list(end_inputs);
		ir_MERGE_list(end_inputs);
	}

	return 1;
}

static int zend_jit_free(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, int may_throw)
{
	zend_jit_addr op1_addr = OP1_ADDR();

	if (op1_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF)) {
		if (may_throw) {
			jit_SET_EX_OPLINE(jit, opline);
		}
		if (opline->opcode == ZEND_FE_FREE && (op1_info & (MAY_BE_OBJECT|MAY_BE_REF))) {
			ir_ref ref, if_array, if_exists, end_inputs = IR_UNUSED;

			if (op1_info & MAY_BE_ARRAY) {
				if_array = jit_if_Z_TYPE(jit, op1_addr, IS_ARRAY);
				ir_IF_TRUE(if_array);
				ir_END_list(end_inputs);
				ir_IF_FALSE(if_array);
			}
			ref = ir_LOAD_U32(ir_ADD_OFFSET(jit_FP(jit), opline->op1.var + offsetof(zval, u2.fe_iter_idx)));
			if_exists = ir_IF(ir_EQ(ref, ir_CONST_U32(-1)));
			ir_IF_TRUE(if_exists);
			ir_END_list(end_inputs);
			ir_IF_FALSE(if_exists);

			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_hash_iterator_del), ref);

			ir_END_list(end_inputs);
			ir_MERGE_list(end_inputs);
		}

		jit_ZVAL_PTR_DTOR(jit, op1_addr, op1_info, 0, opline);

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

			jit_SET_EX_OPLINE(jit, opline);
			ir_CALL_2(IR_VOID, ir_CONST_FUNC(zend_write),
				ir_CONST_ADDR(str), ir_CONST_ADDR(len));

			zend_jit_check_exception(jit);
		}
	} else {
		zend_jit_addr op1_addr = OP1_ADDR();
		ir_ref ref;

		ZEND_ASSERT((op1_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) == MAY_BE_STRING);

		jit_SET_EX_OPLINE(jit, opline);

		ref = jit_Z_PTR(jit, op1_addr);
		ir_CALL_2(IR_VOID, ir_CONST_FUNC(zend_write),
			ir_ADD_OFFSET(ref, offsetof(zend_string, val)),
			ir_LOAD_A(ir_ADD_OFFSET(ref, offsetof(zend_string, len))));

		if (opline->op1_type & (IS_VAR|IS_TMP_VAR)) {
			jit_ZVAL_PTR_DTOR(jit, op1_addr, op1_info, 0, opline);
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

		jit_set_Z_LVAL(jit, res_addr, ir_CONST_LONG(len));
		if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
		} else if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, MAY_BE_LONG)) {
			return 0;
		}
	} else {
		ir_ref ref;

		ZEND_ASSERT((op1_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) == MAY_BE_STRING);

		ref = jit_Z_PTR(jit, op1_addr);
		ref = ir_LOAD_L(ir_ADD_OFFSET(ref, offsetof(zend_string, len)));
		jit_set_Z_LVAL(jit, res_addr, ref);

		if (Z_MODE(res_addr) == IS_REG) {
			if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, MAY_BE_LONG)) {
				return 0;
			}
		} else {
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
		}
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
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

		jit_set_Z_LVAL(jit, res_addr, ir_CONST_LONG(count));
		if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
		} else if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, MAY_BE_LONG)) {
			return 0;
		}
	} else {
		ir_ref ref;

		ZEND_ASSERT((op1_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) == MAY_BE_ARRAY);
		// Note: See the implementation of ZEND_COUNT in Zend/zend_vm_def.h - arrays do not contain IS_UNDEF starting in php 8.1+.

		ref = jit_Z_PTR(jit, op1_addr);
		if (sizeof(void*) == 8) {
			ref = ir_LOAD_U32(ir_ADD_OFFSET(ref, offsetof(HashTable, nNumOfElements)));
			ref = ir_ZEXT_L(ref);
		} else {
			ref = ir_LOAD_L(ir_ADD_OFFSET(ref, offsetof(HashTable, nNumOfElements)));
		}
		jit_set_Z_LVAL(jit, res_addr, ref);

		if (Z_MODE(res_addr) == IS_REG) {
			if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, MAY_BE_LONG)) {
				return 0;
			}
		} else {
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
		}
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
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
		ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_hash_find),
			ir_CONST_ADDR(ht),
			jit_Z_PTR(jit, op1_addr));
	} else {
		zend_string *str = Z_STR_P(RT_CONSTANT(opline, opline->op1));

		ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_hash_find_known_hash),
			ir_CONST_ADDR(ht), ir_CONST_ADDR(str));
	}

	if (exit_addr) {
		if (smart_branch_opcode == ZEND_JMPZ) {
			ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
		} else {
			ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
		}
	} else if (smart_branch_opcode) {
		zend_basic_block *bb;

		ZEND_ASSERT(jit->b >= 0);
		bb = &jit->ssa->cfg.blocks[jit->b];
		ZEND_ASSERT(bb->successors_count == 2);
		ref = jit_IF_ex(jit, ref,
			(smart_branch_opcode == ZEND_JMPZ) ? target_label2 : target_label);
		_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, ref);
		_zend_jit_add_predecessor_ref(jit, bb->successors[1], jit->b, ref);
		jit->ctx.control = IR_UNUSED;
		jit->b = -1;
	} else {
		jit_set_Z_TYPE_INFO_ex(jit, res_addr,
			ir_ADD_U32(ir_ZEXT_U32(ir_NE(ref, IR_NULL)), ir_CONST_U32(IS_FALSE)));
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

		ir_STORE(ir_ADD_OFFSET(jit_FP(jit), offset), ir_CONST_ADDR(str));
	} else {
		zend_jit_addr op2_addr = OP2_ADDR();
		ir_ref ref;

		ZEND_ASSERT((op2_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) == MAY_BE_STRING);

		ref = jit_Z_PTR(jit, op2_addr);
		ir_STORE(ir_ADD_OFFSET(jit_FP(jit), offset), ref);
		if (opline->op2_type == IS_CV) {
			ir_ref if_refcounted, long_path;

			if_refcounted = jit_if_REFCOUNTED(jit, op2_addr);
			ir_IF_TRUE(if_refcounted);
			jit_GC_ADDREF(jit, ref);
			long_path = ir_END();

			ir_IF_FALSE(if_refcounted);
			ir_MERGE_WITH(long_path);
		}
	}

	if (opline->opcode == ZEND_ROPE_END) {
		zend_jit_addr res_addr = RES_ADDR();
		ir_ref ref;

		ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_rope_end),
			ir_ADD_OFFSET(jit_FP(jit), opline->op1.var),
			ir_CONST_U32(opline->extended_value));

		jit_set_Z_PTR(jit, res_addr, ref);
		jit_set_Z_TYPE_INFO(jit, res_addr, IS_STRING_EX);
	}

	return 1;
}

static int zend_jit_zval_copy_deref(zend_jit_ctx *jit, zend_jit_addr res_addr, zend_jit_addr val_addr, ir_ref type)
{
	ir_ref if_refcounted, if_reference, if_refcounted2, ptr, val2, ptr2, type2;
	ir_refs *merge_inputs, *types, *ptrs;
#if SIZEOF_ZEND_LONG == 4
	ir_ref val = jit_ZVAL_ADDR(jit, val_addr);
	ir_refs *values; /* we need this only for zval.w2 copy */
#endif

	ir_refs_init(merge_inputs, 4);
	ir_refs_init(types, 4);
	ir_refs_init(ptrs, 4);
#if SIZEOF_ZEND_LONG == 4
	ir_refs_init(values, 4);
#endif

	// JIT: ptr = Z_PTR_P(val);
	ptr = jit_Z_PTR(jit, val_addr);

	// JIT: if (Z_OPT_REFCOUNTED_P(val)) {
	if_refcounted = ir_IF(ir_AND_U32(type, ir_CONST_U32(Z_TYPE_FLAGS_MASK)));
	ir_IF_FALSE_cold(if_refcounted);
	ir_refs_add(merge_inputs, ir_END());
	ir_refs_add(types, type);
	ir_refs_add(ptrs, ptr);
#if SIZEOF_ZEND_LONG == 4
	ir_refs_add(values, val);
#endif

	ir_IF_TRUE(if_refcounted);

	// JIT: if (UNEXPECTED(Z_OPT_ISREF_P(val))) {
	if_reference = ir_IF(ir_EQ(type, ir_CONST_U32(IS_REFERENCE_EX)));
//	if_reference = ir_IF(ir_EQ(ir_TRUNC_U8(type), ir_CONST_U8(IS_REFERENCE))); // TODO: fix IR to avoid need for extra register ???
	ir_IF_TRUE(if_reference);

	// JIT:	val = Z_REFVAL_P(val);
	val2 = ir_ADD_OFFSET(ptr, offsetof(zend_reference, val));
	type2 = jit_Z_TYPE_INFO_ref(jit, val2);
	ptr2 = jit_Z_PTR_ref(jit, val2);

	// JIT:	if (Z_OPT_REFCOUNTED_P(val)) {
	if_refcounted2 = ir_IF(ir_AND_U32(type2, ir_CONST_U32(Z_TYPE_FLAGS_MASK)));
	ir_IF_FALSE_cold(if_refcounted2);
	ir_refs_add(merge_inputs, ir_END());
	ir_refs_add(types, type2);
	ir_refs_add(ptrs, ptr2);
#if SIZEOF_ZEND_LONG == 4
	ir_refs_add(values, val2);
#endif

	ir_IF_TRUE(if_refcounted2);
	ir_MERGE_WITH_EMPTY_FALSE(if_reference);
	type = ir_PHI_2(type2, type);
	ptr = ir_PHI_2(ptr2, ptr);
#if SIZEOF_ZEND_LONG == 4
	val = ir_PHI_2(val2, val);
#endif

	// JIT:	Z_ADDREF_P(val);
	jit_GC_ADDREF(jit, ptr);
	ir_refs_add(merge_inputs, ir_END());
	ir_refs_add(types, type);
	ir_refs_add(ptrs, ptr);
#if SIZEOF_ZEND_LONG == 4
	ir_refs_add(values, val);
#endif

	ir_MERGE_N(merge_inputs->count, merge_inputs->refs);
	type = ir_PHI_N(types->count, types->refs);
	ptr = ir_PHI_N(ptrs->count, ptrs->refs);
#if SIZEOF_ZEND_LONG == 4
	val = ir_PHI_N(values->count, values->refs);
	val_addr = ZEND_ADDR_REF_ZVAL(val);
#endif

	// JIT: Z_PTR_P(res) = ptr;
	jit_set_Z_PTR(jit, res_addr, ptr);
#if SIZEOF_ZEND_LONG == 4
	jit_set_Z_W2(jit, res_addr, jit_Z_W2(jit, val_addr));
#endif
	jit_set_Z_TYPE_INFO_ex(jit, res_addr, type);

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
                                                  ir_refs       *found_inputs,
                                                  ir_refs       *found_vals,
                                                  ir_ref        *end_inputs,
                                                  ir_ref        *not_found_inputs)
{
	zend_jit_addr op2_addr = OP2_ADDR();
	zend_jit_addr res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var);
	ir_ref ref = IR_UNUSED, cond, if_found;
	ir_ref if_type = IS_UNUSED;
	ir_refs *test_zval_inputs, *test_zval_values;

	ir_refs_init(test_zval_inputs, 4);
	ir_refs_init(test_zval_values, 4);

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
		ir_ref idx_not_found_inputs = IR_UNUSED;

		if (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - MAY_BE_LONG)) {
			// JIT: if (EXPECTED(Z_TYPE_P(dim) == IS_LONG))
			if_type = jit_if_Z_TYPE(jit, op2_addr, IS_LONG);
			ir_IF_TRUE(if_type);
		}
		if (op1_info & MAY_BE_PACKED_GUARD) {
			int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_PACKED_GUARD);
			const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

			if (!exit_addr) {
				return 0;
			}
			cond = ir_AND_U32(
				ir_LOAD_U32(ir_ADD_OFFSET(ht_ref, offsetof(zend_array, u.flags))),
				ir_CONST_U32(HASH_FLAG_PACKED));
			if (op1_info & MAY_BE_ARRAY_PACKED) {
				ir_GUARD(cond, ir_CONST_ADDR(exit_addr));
			} else {
				ir_GUARD_NOT(cond, ir_CONST_ADDR(exit_addr));
			}
		}
		if (type == BP_VAR_W) {
			// JIT: hval = Z_LVAL_P(dim);
			h = jit_Z_LVAL(jit, op2_addr);
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
						jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
					}
				}
				h = ir_CONST_LONG(val);
			} else {
				if (!op2_loaded) {
					// JIT: hval = Z_LVAL_P(dim);
					h = jit_Z_LVAL(jit, op2_addr);
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
					if_packed = ir_IF(
						ir_AND_U32(
							ir_LOAD_U32(ir_ADD_OFFSET(ht_ref, offsetof(zend_array, u.flags))),
							ir_CONST_U32(HASH_FLAG_PACKED)));
					ir_IF_TRUE(if_packed);
				}
				// JIT: if (EXPECTED((zend_ulong)(_h) < (zend_ulong)(_ht)->nNumUsed))
				ref = ir_LOAD_U32(ir_ADD_OFFSET(ht_ref, offsetof(zend_array, nNumUsed)));
#if SIZEOF_ZEND_LONG == 8
				ref = ir_ZEXT_L(ref);
#endif
				cond = ir_ULT(h, ref);
				if (type == BP_JIT_IS) {
					if (not_found_exit_addr) {
						ir_GUARD(cond, ir_CONST_ADDR(not_found_exit_addr));
					} else {
						ir_ref if_fit = ir_IF(cond);
						ir_IF_FALSE(if_fit);
						ir_END_list(*end_inputs);
						ir_IF_TRUE(if_fit);
					}
				} else if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && type == BP_VAR_R) {
					ir_GUARD(cond, ir_CONST_ADDR(exit_addr));
				} else if (type == BP_VAR_IS && not_found_exit_addr) {
					ir_GUARD(cond, ir_CONST_ADDR(not_found_exit_addr));
				} else if (type == BP_VAR_RW && not_found_exit_addr) {
					ir_GUARD(cond, ir_CONST_ADDR(not_found_exit_addr));
				} else if (type == BP_VAR_IS && result_type_guard) {
					ir_ref if_fit = ir_IF(cond);
					ir_IF_FALSE(if_fit);
					ir_END_list(*not_found_inputs);
					ir_IF_TRUE(if_fit);
				} else {
					ir_ref if_fit = ir_IF(cond);
					ir_IF_FALSE(if_fit);
					ir_END_list(idx_not_found_inputs);
					ir_IF_TRUE(if_fit);
				}
				// JIT: _ret = &_ht->arPacked[h];
				ref = ir_MUL_L(h, ir_CONST_LONG(sizeof(zval)));
				ref = ir_BITCAST_A(ref);
				ref = ir_ADD_A(ir_LOAD_A(ir_ADD_OFFSET(ht_ref, offsetof(zend_array, arPacked))), ref);
				if (type == BP_JIT_IS) {
					ir_refs_add(test_zval_values, ref);
					ir_refs_add(test_zval_inputs, ir_END());
				}
			}
		}
		switch (type) {
			case BP_JIT_IS:
				if (op1_info & MAY_BE_ARRAY_NUMERIC_HASH) {
					if (if_packed) {
						ir_IF_FALSE(if_packed);
						if_packed = IR_UNUSED;
					}
					if (!op2_loaded) {
						// JIT: hval = Z_LVAL_P(dim);
						h = jit_Z_LVAL(jit, op2_addr);
					}
					if (packed_loaded) {
						ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(_zend_hash_index_find), ht_ref, h);
					} else {
						ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_hash_index_find), ht_ref, h);
					}
					if (not_found_exit_addr) {
						ir_GUARD(ref, ir_CONST_ADDR(not_found_exit_addr));
					} else {
						if_found = ir_IF(ref);
						ir_IF_FALSE(if_found);
						ir_END_list(*end_inputs);
						ir_IF_TRUE(if_found);
					}
					ir_refs_add(test_zval_values, ref);
					ir_refs_add(test_zval_inputs, ir_END());
				} else if (!not_found_exit_addr && !packed_loaded) {
					ir_END_list(*end_inputs);
				}
				break;
			case BP_VAR_R:
			case BP_VAR_IS:
			case BP_VAR_UNSET:
				if (packed_loaded) {
					ir_ref type_ref = jit_Z_TYPE_ref(jit, ref);

					if (op1_info & MAY_BE_ARRAY_NUMERIC_HASH) {
						ir_ref if_def = ir_IF(type_ref);
						ir_IF_TRUE(if_def);
						ir_refs_add(found_inputs, ir_END());
						ir_refs_add(found_vals, ref);
						ir_IF_FALSE(if_def);
					} else if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && type == BP_VAR_R) {
						/* perform IS_UNDEF check only after result type guard (during deoptimization) */
						if (!result_type_guard || (op1_info & MAY_BE_ARRAY_NUMERIC_HASH)) {
							ir_GUARD(type_ref, ir_CONST_ADDR(exit_addr));
						}
					} else if (type == BP_VAR_IS && not_found_exit_addr) {
						ir_GUARD(type_ref, ir_CONST_ADDR(not_found_exit_addr));
					} else if (type == BP_VAR_IS && result_type_guard) {
						ir_ref if_def = ir_IF(type_ref);
						ir_IF_FALSE(if_def);
						ir_END_list(*not_found_inputs);
						ir_IF_TRUE(if_def);
					} else {
						ir_ref if_def = ir_IF(type_ref);
						ir_IF_FALSE(if_def);
						ir_END_list(idx_not_found_inputs);
						ir_IF_TRUE(if_def);
					}
				}
				if (!(op1_info & MAY_BE_ARRAY_KEY_LONG) || (packed_loaded && (op1_info & MAY_BE_ARRAY_NUMERIC_HASH))) {
					if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && type == BP_VAR_R) {
						jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
					} else if (type == BP_VAR_IS && not_found_exit_addr) {
						jit_SIDE_EXIT(jit, ir_CONST_ADDR(not_found_exit_addr));
					} else if (type == BP_VAR_IS && result_type_guard) {
						ir_END_list(*not_found_inputs);
					} else {
						ir_END_list(idx_not_found_inputs);
					}
				}
				if (/*!packed_loaded ||*/ (op1_info & MAY_BE_ARRAY_NUMERIC_HASH)) {
					if (if_packed) {
						ir_IF_FALSE(if_packed);
						if_packed = IR_UNUSED;
					}
					if (!op2_loaded) {
						// JIT: hval = Z_LVAL_P(dim);
						h = jit_Z_LVAL(jit, op2_addr);
					}
					if (packed_loaded) {
						ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(_zend_hash_index_find), ht_ref, h);
					} else {
						ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_hash_index_find), ht_ref, h);
					}
					if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && type == BP_VAR_R) {
						ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
					} else if (type == BP_VAR_IS && not_found_exit_addr) {
						ir_GUARD(ref, ir_CONST_ADDR(not_found_exit_addr));
					} else if (type == BP_VAR_IS && result_type_guard) {
						if_found = ir_IF(ref);
						ir_IF_FALSE(if_found);
						ir_END_list(*not_found_inputs);
						ir_IF_TRUE(if_found);
					} else {
						if_found = ir_IF(ref);
						ir_IF_FALSE(if_found);
						ir_END_list(idx_not_found_inputs);
						ir_IF_TRUE(if_found);
					}
					ir_refs_add(found_inputs, ir_END());
					ir_refs_add(found_vals, ref);
				} else if (packed_loaded) {
					ir_refs_add(found_inputs, ir_END());
					ir_refs_add(found_vals, ref);
				}

				if (idx_not_found_inputs) {
					ir_MERGE_list(idx_not_found_inputs);
					switch (type) {
						case BP_VAR_R:
							ZEND_ASSERT(JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE);
							// JIT: zend_error(E_WARNING,"Undefined array key " ZEND_LONG_FMT, hval);
							// JIT: retval = &EG(uninitialized_zval);
							jit_SET_EX_OPLINE(jit, opline);
							ir_CALL(IR_VOID, jit_STUB_FUNC_ADDR(jit, jit_stub_undefined_offset, IR_CONST_FASTCALL_FUNC));
							ir_END_list(*end_inputs);
							break;
						case BP_VAR_IS:
						case BP_VAR_UNSET:
							if (!not_found_exit_addr) {
								// JIT: retval = &EG(uninitialized_zval);
								jit_set_Z_TYPE_INFO(jit, res_addr, IS_NULL);
								ir_END_list(*end_inputs);
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
						ir_refs_add(found_inputs, ir_END());
						ir_refs_add(found_vals, ref);
					} else {
						ir_ref if_def = ir_IF(jit_Z_TYPE_ref(jit, ref));
						ir_IF_TRUE(if_def);
						ir_refs_add(found_inputs, ir_END());
						ir_refs_add(found_vals, ref);
						ir_IF_FALSE_cold(if_def);
						ir_END_list(idx_not_found_inputs);
					}
				}
				if (!packed_loaded ||
						!not_found_exit_addr ||
						(op1_info & MAY_BE_ARRAY_NUMERIC_HASH)) {
					if (if_packed) {
						ir_IF_FALSE(if_packed);
						if_packed = IR_UNUSED;
						ir_END_list(idx_not_found_inputs);
					} if (!packed_loaded) {
						ir_END_list(idx_not_found_inputs);
					}

					ir_MERGE_list(idx_not_found_inputs);
					if (!op2_loaded) {
						// JIT: hval = Z_LVAL_P(dim);
						h = jit_Z_LVAL(jit, op2_addr);
					}
					if (packed_loaded) {
						ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_hash_index_lookup_rw_no_packed),
							ht_ref, h);
					} else {
						ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_hash_index_lookup_rw), ht_ref, h);
					}
					if (not_found_exit_addr) {
						if (packed_loaded) {
							ir_GUARD(ref, ir_CONST_ADDR(not_found_exit_addr));
						} else {
							ir_GUARD(ref, ir_CONST_ADDR(not_found_exit_addr));
						}
					} else {
						if_found = ir_IF(ref);
						ir_IF_FALSE(if_found);
						ir_END_list(*end_inputs);
						ir_IF_TRUE(if_found);
					}
					ir_refs_add(found_inputs, ir_END());
					ir_refs_add(found_vals, ref);
				}
				break;
			case BP_VAR_W:
				if (packed_loaded) {
					ir_ref if_def = ir_IF(jit_Z_TYPE_ref(jit, ref));
					if (!(op1_info & MAY_BE_ARRAY_KEY_LONG) || (op1_info & MAY_BE_ARRAY_NUMERIC_HASH) || packed_loaded || bad_packed_key || dim_type == IS_UNDEF) {
						ir_IF_TRUE_cold(if_def);
					} else {
						ir_IF_TRUE(if_def);
					}
					ir_refs_add(found_inputs, ir_END());
					ir_refs_add(found_vals, ref);
					ir_IF_FALSE(if_def);
					ir_END_list(idx_not_found_inputs);
				}
				if (!(op1_info & MAY_BE_ARRAY_KEY_LONG) || (op1_info & MAY_BE_ARRAY_NUMERIC_HASH) || packed_loaded || bad_packed_key || dim_type == IS_UNDEF) {
					if (if_packed) {
						ir_IF_FALSE(if_packed);
						if_packed = IR_UNUSED;
						ir_END_list(idx_not_found_inputs);
					} else if (!packed_loaded) {
						ir_END_list(idx_not_found_inputs);
					}
					ir_MERGE_list(idx_not_found_inputs);
					if (!op2_loaded) {
						// JIT: hval = Z_LVAL_P(dim);
						h = jit_Z_LVAL(jit, op2_addr);
					}
					ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_hash_index_lookup), ht_ref, h);
					ir_refs_add(found_inputs, ir_END());
					ir_refs_add(found_vals, ref);
				}
				break;
			default:
				ZEND_UNREACHABLE();
		}
	}

	if (op2_info & MAY_BE_STRING) {
		ir_ref key;

		if (if_type) {
			ir_IF_FALSE(if_type);
			if_type = IS_UNUSED;
		}

		if (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_LONG|MAY_BE_STRING))) {
			// JIT: if (EXPECTED(Z_TYPE_P(dim) == IS_STRING))
			if_type = jit_if_Z_TYPE(jit, op2_addr, IS_STRING);
			ir_IF_TRUE(if_type);
		}

		// JIT: offset_key = Z_STR_P(dim);
		key = jit_Z_PTR(jit, op2_addr);

		// JIT: retval = zend_hash_find(ht, offset_key);
		switch (type) {
			case BP_JIT_IS:
				if (opline->op2_type != IS_CONST) {
					ir_ref if_num, end1, ref2;

					if_num = ir_IF(
						ir_ULE(
							ir_LOAD_C(ir_ADD_OFFSET(key, offsetof(zend_string, val))),
							ir_CONST_CHAR('9')));
					ir_IF_TRUE_cold(if_num);
					ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_symtable_find), ht_ref, key);
					end1 = ir_END();
					ir_IF_FALSE(if_num);
					ref2 = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_hash_find), ht_ref, key);
					ir_MERGE_WITH(end1);
					ref = ir_PHI_2(ref2, ref);
				} else {
					ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_hash_find_known_hash), ht_ref, key);
				}
				if (not_found_exit_addr) {
					ir_GUARD(ref, ir_CONST_ADDR(not_found_exit_addr));
				} else {
					if_found = ir_IF(ref);
					ir_IF_FALSE(if_found);
					ir_END_list(*end_inputs);
					ir_IF_TRUE(if_found);
				}
				ir_refs_add(test_zval_values, ref);
				ir_refs_add(test_zval_inputs, ir_END());
				break;
			case BP_VAR_R:
			case BP_VAR_IS:
			case BP_VAR_UNSET:
				if (opline->op2_type != IS_CONST) {
					ir_ref if_num, end1, ref2;

					if_num = ir_IF( 
						ir_ULE(
							ir_LOAD_C(ir_ADD_OFFSET(key, offsetof(zend_string, val))),
							ir_CONST_CHAR('9')));
					ir_IF_TRUE_cold(if_num);
					ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_symtable_find), ht_ref, key);
					end1 = ir_END();
					ir_IF_FALSE(if_num);
					ref2 = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_hash_find), ht_ref, key);
					ir_MERGE_WITH(end1);
					ref = ir_PHI_2(ref2, ref);
				} else {
					ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_hash_find_known_hash), ht_ref, key);
				}
				if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && type == BP_VAR_R) {
					ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
				} else if (type == BP_VAR_IS && not_found_exit_addr) {
					ir_GUARD(ref, ir_CONST_ADDR(not_found_exit_addr));
				} else if (type == BP_VAR_IS && result_type_guard) {
					if_found = ir_IF(ref);
					ir_IF_FALSE(if_found);
					ir_END_list(*not_found_inputs);
					ir_IF_TRUE(if_found);
				} else {
					if_found = ir_IF(ref);
					switch (type) {
						case BP_VAR_R:
							ir_IF_FALSE_cold(if_found);
							// JIT: zend_error(E_WARNING, "Undefined array key \"%s\"", ZSTR_VAL(offset_key));
							jit_SET_EX_OPLINE(jit, opline);
							ir_CALL(IR_VOID, jit_STUB_FUNC_ADDR(jit, jit_stub_undefined_key, IR_CONST_FASTCALL_FUNC));
							ir_END_list(*end_inputs);
							break;
						case BP_VAR_IS:
						case BP_VAR_UNSET:
							ir_IF_FALSE(if_found);
							// JIT: retval = &EG(uninitialized_zval);
							jit_set_Z_TYPE_INFO(jit, res_addr, IS_NULL);
							ir_END_list(*end_inputs);
							break;
						default:
							ZEND_UNREACHABLE();
					}
					ir_IF_TRUE(if_found);
				}
				ir_refs_add(found_inputs, ir_END());
				ir_refs_add(found_vals, ref);
				break;
			case BP_VAR_RW:
				if (opline->op2_type != IS_CONST) {
					ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_symtable_lookup_rw), ht_ref, key);
				} else {
					ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_hash_lookup_rw), ht_ref, key);
				}
				if (not_found_exit_addr) {
					ir_GUARD(ref, ir_CONST_ADDR(not_found_exit_addr));
				} else {
					if_found = ir_IF(ref);
					ir_IF_FALSE(if_found);
					ir_END_list(*end_inputs);
					ir_IF_TRUE(if_found);
				}
				ir_refs_add(found_inputs, ir_END());
				ir_refs_add(found_vals, ref);
				break;
			case BP_VAR_W:
				if (opline->op2_type != IS_CONST) {
					ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_symtable_lookup_w), ht_ref, key);
				} else {
					ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_hash_lookup), ht_ref, key);
				}
				ir_refs_add(found_inputs, ir_END());
				ir_refs_add(found_vals, ref);
				break;
			default:
				ZEND_UNREACHABLE();
		}
	}

	if (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_LONG|MAY_BE_STRING))) {
	    if (if_type) {
			ir_IF_FALSE_cold(if_type);
			if_type = IS_UNDEF;
		}
		if (type != BP_VAR_RW) {
			jit_SET_EX_OPLINE(jit, opline);
		}
		ref = jit_ZVAL_ADDR(jit, op2_addr);
		switch (type) {
			case BP_VAR_R:
				ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_fetch_dim_r_helper),
					ht_ref,
					ref,
					jit_ZVAL_ADDR(jit, res_addr));
				ir_END_list(*end_inputs);
				break;
			case BP_JIT_IS:
				ref = ir_CALL_2(IR_I32, ir_CONST_FC_FUNC(zend_jit_fetch_dim_isset_helper), ht_ref, ref);
				if (not_found_exit_addr) {
					ir_GUARD(ref, ir_CONST_ADDR(not_found_exit_addr));
					ir_refs_add(found_inputs, ir_END());
				} else if (found_exit_addr) {
					ir_GUARD_NOT(ref, ir_CONST_ADDR(found_exit_addr));
					ir_END_list(*end_inputs);
				} else {
					if_found = ir_IF(ref);
					ir_IF_TRUE(if_found);
					ir_refs_add(found_inputs, ir_END());
					ir_IF_FALSE(if_found);
					ir_END_list(*end_inputs);
				}
				break;
			case BP_VAR_IS:
			case BP_VAR_UNSET:
				ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_fetch_dim_is_helper),
					ht_ref,
					ref,
					jit_ZVAL_ADDR(jit, res_addr));
				ir_END_list(*end_inputs);
				break;
			case BP_VAR_RW:
				ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_fetch_dim_rw_helper), ht_ref, ref);
				if_found = ir_IF(ref);
				ir_IF_TRUE(if_found);
				ir_refs_add(found_inputs, ir_END());
				ir_refs_add(found_vals, ref);
				ir_IF_FALSE(if_found);
				ir_END_list(*end_inputs);
				break;
			case BP_VAR_W:
				ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_fetch_dim_w_helper), ht_ref, ref);
				if_found = ir_IF(ref);
				ir_IF_TRUE(if_found);
				ir_refs_add(found_inputs, ir_END());
				ir_refs_add(found_vals, ref);
				ir_IF_FALSE(if_found);
				ir_END_list(*end_inputs);
				break;
			default:
				ZEND_UNREACHABLE();
		}
	}

	if (type == BP_JIT_IS && (op1_info & MAY_BE_ARRAY) && (op2_info & (MAY_BE_LONG|MAY_BE_STRING))) {
		ir_MERGE_N(test_zval_inputs->count, test_zval_inputs->refs);
		ref = ir_PHI_N(test_zval_values->count, test_zval_values->refs);

		if (op1_info & MAY_BE_ARRAY_OF_REF) {
			ref = jit_ZVAL_DEREF_ref(jit, ref);
		}
		cond = ir_GT(jit_Z_TYPE_ref(jit, ref), ir_CONST_U8(IS_NULL));
		if (not_found_exit_addr) {
			ir_GUARD(cond, ir_CONST_ADDR(not_found_exit_addr));
			ir_refs_add(found_inputs, ir_END());
		} else if (found_exit_addr) {
			ir_GUARD_NOT(cond, ir_CONST_ADDR(found_exit_addr));
			ir_END_list(*end_inputs);
		} else {
			ir_ref if_set = ir_IF(cond);
			ir_IF_FALSE(if_set);
			ir_END_list(*end_inputs);
			ir_IF_TRUE(if_set);
			ir_refs_add(found_inputs, ir_END());
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
	ir_ref end_inputs = IR_UNUSED;
	ir_ref not_found_inputs = IR_UNUSED;

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
		ir_ref ref = jit_ZVAL_ADDR(jit, op1_addr);
		ref = jit_ZVAL_DEREF_ref(jit, ref);
		op1_addr = ZEND_ADDR_REF_ZVAL(ref);
	}

	if (op1_info & MAY_BE_ARRAY) {
		ir_ref ht_ref, ref;
		zend_jit_addr val_addr;
		ir_refs *found_inputs, *found_vals;

		ir_refs_init(found_inputs, 10);
		ir_refs_init(found_vals, 10);

		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - MAY_BE_ARRAY)) {
			if (exit_addr && !(op1_info & (MAY_BE_OBJECT|may_be_string))) {
				jit_guard_Z_TYPE(jit, op1_addr, IS_ARRAY, exit_addr);
			} else {
				if_type = jit_if_Z_TYPE(jit, op1_addr, IS_ARRAY);
				ir_IF_TRUE(if_type);
			}
		}

		ht_ref = jit_Z_PTR(jit, op1_addr);

		if ((op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_LONG|MAY_BE_STRING))) ||
		    (opline->opcode != ZEND_FETCH_DIM_IS && JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE)) {
			may_throw = 1;
		}

		if (!zend_jit_fetch_dimension_address_inner(jit, opline,
				(opline->opcode != ZEND_FETCH_DIM_IS) ? BP_VAR_R : BP_VAR_IS,
				op1_info, op2_info, dim_type, NULL, not_found_exit_addr, exit_addr,
				result_type_guard, ht_ref, found_inputs, found_vals,
				&end_inputs, &not_found_inputs)) {
			return 0;
		}

		if (found_inputs->count) {
			ir_MERGE_N(found_inputs->count, found_inputs->refs);
			ref = ir_PHI_N(found_vals->count, found_vals->refs);
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

				if (not_found_inputs) {
					ir_END_list(not_found_inputs);
					ir_MERGE_list(not_found_inputs);
				}

				// ZVAL_COPY
				jit_ZVAL_COPY(jit, res_addr, -1, val_addr, res_info, !result_avoid_refcounting);
				if (Z_MODE(res_addr) == IS_MEM_ZVAL) {
				} else if (!zend_jit_store_var_if_necessary(jit, opline->result.var, res_addr, res_info)) {
					return 0;
				}
			} else if (op1_info & MAY_BE_ARRAY_OF_REF) {
				// ZVAL_COPY_DEREF
				ir_ref type_info = jit_Z_TYPE_INFO(jit, val_addr);
				if (!zend_jit_zval_copy_deref(jit, res_addr, val_addr, type_info)) {
					return 0;
				}
			} else  {
				// ZVAL_COPY
				jit_ZVAL_COPY(jit, res_addr, -1, val_addr, res_info, 1);
			}

			ir_END_list(end_inputs);
		}
	}

	if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_ARRAY)) {
		if (if_type) {
			ir_IF_FALSE_cold(if_type);
			if_type = IS_UNDEF;
		}

		if (opline->opcode != ZEND_FETCH_LIST_R && (op1_info & MAY_BE_STRING)) {
			ir_ref str_ref;

			may_throw = 1;
			if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_ARRAY|MAY_BE_STRING))) {
				if (exit_addr && !(op1_info & MAY_BE_OBJECT)) {
					jit_guard_Z_TYPE(jit, op1_addr, IS_STRING, exit_addr);
				} else {
					if_type = jit_if_Z_TYPE(jit, op1_addr, IS_STRING);
					ir_IF_TRUE(if_type);
				}
			}
			jit_SET_EX_OPLINE(jit, opline);
			str_ref = jit_Z_PTR(jit, op1_addr);
			if (opline->opcode != ZEND_FETCH_DIM_IS) {
				ir_ref ref;

				if ((op2_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_GUARD)) == MAY_BE_LONG) {
					ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_fetch_dim_str_offset_r_helper),
						str_ref, jit_Z_LVAL(jit, op2_addr));
				} else {
					ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_fetch_dim_str_r_helper),
						str_ref, jit_ZVAL_ADDR(jit, op2_addr));
				}
				jit_set_Z_PTR(jit, res_addr, ref);
				jit_set_Z_TYPE_INFO(jit, res_addr, IS_STRING);
			} else {
				ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_fetch_dim_str_is_helper),
					str_ref,
					jit_ZVAL_ADDR(jit, op2_addr),
					jit_ZVAL_ADDR(jit, res_addr));
			}
			ir_END_list(end_inputs);
		}

		if (op1_info & MAY_BE_OBJECT) {
			ir_ref arg2;

			if (if_type) {
				ir_IF_FALSE_cold(if_type);
				if_type = IS_UNDEF;
			}

			may_throw = 1;
			if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_ARRAY|MAY_BE_OBJECT|may_be_string))) {
				if (exit_addr) {
					jit_guard_Z_TYPE(jit, op1_addr, IS_OBJECT, exit_addr);
				} else {
					if_type = jit_if_Z_TYPE(jit, op1_addr, IS_OBJECT);
					ir_IF_TRUE(if_type);
				}
			}

			jit_SET_EX_OPLINE(jit, opline);
			if (opline->op2_type == IS_CONST && Z_EXTRA_P(RT_CONSTANT(opline, opline->op2)) == ZEND_EXTRA_VALUE) {
				ZEND_ASSERT(Z_MODE(op2_addr) == IS_CONST_ZVAL);
				arg2 = ir_CONST_ADDR(Z_ZV(op2_addr)+1);
			} else {
				arg2 = jit_ZVAL_ADDR(jit, op2_addr);
			}

			if (opline->opcode != ZEND_FETCH_DIM_IS) {
				ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_fetch_dim_obj_r_helper),
					jit_ZVAL_ADDR(jit, op1_addr),
					arg2,
					jit_ZVAL_ADDR(jit, res_addr));
			} else {
				ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_fetch_dim_obj_is_helper),
					jit_ZVAL_ADDR(jit, op1_addr),
					arg2,
					jit_ZVAL_ADDR(jit, res_addr));
			}

			ir_END_list(end_inputs);
		}

		if ((op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_ARRAY|MAY_BE_OBJECT|may_be_string)))
		 && (!exit_addr || !(op1_info & (MAY_BE_ARRAY|MAY_BE_OBJECT|may_be_string)))) {

			if (if_type) {
				ir_IF_FALSE_cold(if_type);
				if_type = IS_UNDEF;
			}

			if ((opline->opcode != ZEND_FETCH_DIM_IS && (op1_info & MAY_BE_UNDEF)) || (op2_info & MAY_BE_UNDEF)) {
				jit_SET_EX_OPLINE(jit, opline);
				if (opline->opcode != ZEND_FETCH_DIM_IS && (op1_info & MAY_BE_UNDEF)) {
					may_throw = 1;
					zend_jit_type_check_undef(jit, jit_Z_TYPE(jit, op1_addr), opline->op1.var, NULL, 0, 1);
				}

				if (op2_info & MAY_BE_UNDEF) {
					may_throw = 1;
					zend_jit_type_check_undef(jit, jit_Z_TYPE(jit, op2_addr), opline->op2.var, NULL, 0, 1);
				}
			}

			if (opline->opcode != ZEND_FETCH_DIM_IS && opline->opcode != ZEND_FETCH_LIST_R) {
				ir_ref ref;

				may_throw = 1;
				if ((op1_info & MAY_BE_UNDEF) || (op2_info & MAY_BE_UNDEF)) {
					ref = jit_ZVAL_ADDR(jit, orig_op1_addr);
				} else {
					jit_SET_EX_OPLINE(jit, opline);
					ref = jit_ZVAL_ADDR(jit, op1_addr);
				}
				ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_invalid_array_access), ref);
			}

			jit_set_Z_TYPE_INFO(jit, res_addr, IS_NULL);
			ir_END_list(end_inputs);
		}
	}

	if (end_inputs) {
		ir_MERGE_list(end_inputs);

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
			jit_FREE_OP(jit,  opline->op2_type, opline->op2, op2_info, opline);
		}
		if (opline->opcode != ZEND_FETCH_LIST_R && !op1_avoid_refcounting) {
			if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
				if ((op1_info & MAY_HAVE_DTOR) && (op1_info & MAY_BE_RC1)) {
					may_throw = 1;
				}
				jit_FREE_OP(jit,  opline->op1_type, opline->op1, op1_info, opline);
			}
		}

		if (may_throw) {
			zend_jit_check_exception(jit);
		}
	} else if (op1_info & (MAY_BE_ANY|MAY_BE_UNDEF)) {
		ir_BEGIN(IR_UNUSED); /* unreachable tail */
	}

	return 1;
}

static zend_jit_addr zend_jit_prepare_array_update(zend_jit_ctx   *jit,
                                                   const zend_op  *opline,
                                                   uint32_t        op1_info,
                                                   zend_jit_addr   op1_addr,
                                                   ir_ref         *if_type,
                                                   ir_ref         *ht_ref,
                                                   int            *may_throw)
{
	ir_ref ref = IR_UNUSED;
	ir_ref array_reference_end = IR_UNUSED, array_reference_ref = IR_UNUSED;
	ir_refs *array_inputs, *array_values;

	ir_refs_init(array_inputs, 4);
	ir_refs_init(array_values, 4);

	ref = jit_ZVAL_ADDR(jit, op1_addr);
	if (op1_info & MAY_BE_REF) {
		ir_ref if_reference, if_array, end1, ref2;

		*may_throw = 1;
		if_reference = jit_if_Z_TYPE(jit, op1_addr, IS_REFERENCE);
		ir_IF_FALSE(if_reference);
		end1 = ir_END();
		ir_IF_TRUE_cold(if_reference);
		array_reference_ref = ir_ADD_OFFSET(jit_Z_PTR_ref(jit, ref), offsetof(zend_reference, val));
		if_array = jit_if_Z_TYPE_ref(jit, array_reference_ref, ir_CONST_U8(IS_ARRAY));
		ir_IF_TRUE(if_array);
		array_reference_end = ir_END();
		ir_IF_FALSE_cold(if_array);
		if (opline->opcode != ZEND_FETCH_DIM_RW && opline->opcode != ZEND_ASSIGN_DIM_OP) {
			jit_SET_EX_OPLINE(jit, opline);
		}
		ref2 = ir_CALL_1(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_prepare_assign_dim_ref), ref);
		ir_GUARD(ref2, jit_STUB_ADDR(jit, jit_stub_exception_handler_undef));

		ir_MERGE_WITH(end1);
		ref = ir_PHI_2(ref2, ref);
		op1_addr = ZEND_ADDR_REF_ZVAL(ref);
	}

	if (op1_info & MAY_BE_ARRAY) {
		ir_ref op1_ref = ref;

		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - MAY_BE_ARRAY)) {
			*if_type = jit_if_Z_TYPE(jit, op1_addr, IS_ARRAY);
			ir_IF_TRUE(*if_type);
		}
		if (array_reference_end) {
			ir_MERGE_WITH(array_reference_end);
			op1_ref = ir_PHI_2(ref, array_reference_ref);
		}
		// JIT: SEPARATE_ARRAY()
		ref = jit_Z_PTR_ref(jit, op1_ref);
		if (RC_MAY_BE_N(op1_info)) {
			if (RC_MAY_BE_1(op1_info)) {
				ir_ref if_refcount_1 = ir_IF(ir_EQ(jit_GC_REFCOUNT(jit, ref), ir_CONST_U32(1)));
				ir_IF_TRUE(if_refcount_1);
				ir_refs_add(array_inputs, ir_END());
				ir_refs_add(array_values, ref);
				ir_IF_FALSE(if_refcount_1);
			}
			ref = ir_CALL_1(IR_ADDR, ir_CONST_FC_FUNC(zend_jit_zval_array_dup), op1_ref);
		}
		if (array_inputs->count || (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL))) {
			ir_refs_add(array_inputs, ir_END());
			ir_refs_add(array_values, ref);
		}
	}

	if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL)) {
		if (*if_type) {
			ir_IF_FALSE_cold(*if_type);
			*if_type = IR_UNUSED;
		}
		if (op1_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_ARRAY))) {
			*if_type = ir_IF(ir_LE(jit_Z_TYPE(jit, op1_addr), ir_CONST_U8(IS_NULL)));
			ir_IF_TRUE(*if_type);
		}
		if ((op1_info & MAY_BE_UNDEF)
		 && (opline->opcode == ZEND_FETCH_DIM_RW || opline->opcode == ZEND_ASSIGN_DIM_OP)) {
			ir_ref end1 = IR_UNUSED;

			*may_throw = 1;
			if (op1_info & MAY_BE_NULL) {
				ir_ref if_def = ir_IF(jit_Z_TYPE(jit, op1_addr));
				ir_IF_TRUE(if_def);
				end1 = ir_END();
				ir_IF_FALSE(if_def);
			}
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_undefined_op_helper), ir_CONST_U32(opline->op1.var));
			if (end1) {
				ir_MERGE_WITH(end1);
			}
		}
		// JIT: ZVAL_ARR(container, zend_new_array(8));
		ref = ir_CALL_1(IR_ADDR,
			jit_STUB_FUNC_ADDR(jit, jit_stub_new_array, IR_CONST_FASTCALL_FUNC),
			jit_ZVAL_ADDR(jit, op1_addr));
		if (array_inputs->count) {
			ir_refs_add(array_inputs, ir_END());
			ir_refs_add(array_values, ref);
		}
	}

	if (array_inputs->count) {
		ir_MERGE_N(array_inputs->count, array_inputs->refs);
		ref = ir_PHI_N(array_values->count, array_values->refs);
	}

	*ht_ref = ref;
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
	ir_ref end_inputs = IR_UNUSED;
	ir_ref ref, if_type = IR_UNUSED, ht_ref;

	op2_addr = (opline->op2_type != IS_UNUSED) ? OP2_ADDR() : 0;

	if (opline->opcode == ZEND_FETCH_DIM_RW) {
		jit_SET_EX_OPLINE(jit, opline);
	}

	op1_addr = zend_jit_prepare_array_update(jit, opline, op1_info, op1_addr, &if_type, &ht_ref, &may_throw);

	if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_ARRAY)) {
		ir_refs *found_inputs, *found_vals;

		ir_refs_init(found_inputs, 8);
		ir_refs_init(found_vals, 8);

		if (opline->op2_type == IS_UNUSED) {
			ir_ref if_ok;

			may_throw = 1;
			// JIT:var_ptr = zend_hash_next_index_insert(Z_ARRVAL_P(container), &EG(uninitialized_zval));
			ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_hash_next_index_insert),
				ht_ref, jit_EG(uninitialized_zval));

			// JIT: if (UNEXPECTED(!var_ptr)) {
			if_ok = ir_IF(ref);
			ir_IF_FALSE_cold(if_ok);
			if (opline->opcode != ZEND_FETCH_DIM_RW) {
				jit_SET_EX_OPLINE(jit, opline);
			}
			ir_CALL(IR_VOID, jit_STUB_FUNC_ADDR(jit, jit_stub_cannot_add_element, IR_CONST_FASTCALL_FUNC));
			ir_END_list(end_inputs);

			ir_IF_TRUE(if_ok);
			jit_set_Z_PTR(jit, res_addr, ref);
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_INDIRECT);

			ir_END_list(end_inputs);
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
					0, ht_ref, found_inputs, found_vals, &end_inputs, NULL)) {
				return 0;
			}

			if (type == BP_VAR_RW || (op2_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - (MAY_BE_LONG|MAY_BE_STRING)))) {
				if (end_inputs) {
					ir_MERGE_list(end_inputs);
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_NULL);
					end_inputs = ir_END();
				}
			} else {
				ZEND_ASSERT(end_inputs == IR_UNUSED);
			}

			if (found_inputs->count) {
				ir_MERGE_N(found_inputs->count, found_inputs->refs);
				ref = ir_PHI_N(found_vals->count, found_vals->refs);
				jit_set_Z_PTR(jit, res_addr, ref);
				jit_set_Z_TYPE_INFO(jit, res_addr, IS_INDIRECT);
				ir_END_list(end_inputs);
			}

		}
	}

	if (op1_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_ARRAY))) {
		ir_ref arg2;

		may_throw = 1;

		if (if_type) {
			ir_IF_FALSE(if_type);
			if_type = IR_UNUSED;
		}

		if (opline->opcode != ZEND_FETCH_DIM_RW) {
			jit_SET_EX_OPLINE(jit, opline);
		}

	    if (opline->op2_type == IS_UNUSED) {
			arg2 = IR_NULL;
		} else if (opline->op2_type == IS_CONST && Z_EXTRA_P(RT_CONSTANT(opline, opline->op2)) == ZEND_EXTRA_VALUE) {
			ZEND_ASSERT(Z_MODE(op2_addr) == IS_CONST_ZVAL);
			arg2 = ir_CONST_ADDR(Z_ZV(op2_addr) + 1);
		} else {
			arg2 = jit_ZVAL_ADDR(jit, op2_addr);
		}

		switch (opline->opcode) {
			case ZEND_FETCH_DIM_W:
			case ZEND_FETCH_LIST_W:
				ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_fetch_dim_obj_w_helper),
					jit_ZVAL_ADDR(jit, op1_addr),
					arg2,
					jit_ZVAL_ADDR(jit, res_addr));
				break;
			case ZEND_FETCH_DIM_RW:
				ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_fetch_dim_obj_rw_helper),
					jit_ZVAL_ADDR(jit, op1_addr),
					arg2,
					jit_ZVAL_ADDR(jit, res_addr));
				break;
//			case ZEND_FETCH_DIM_UNSET:
//				|	EXT_CALL zend_jit_fetch_dim_obj_unset_helper, r0
//				break;
			default:
				ZEND_UNREACHABLE();
			}

		if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_ARRAY)) {
			ir_END_list(end_inputs);
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

	if (end_inputs) {
		ir_MERGE_list(end_inputs);
	}

	jit_FREE_OP(jit, opline->op2_type, opline->op2, op2_info, opline);

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
	ir_ref false_inputs = IR_UNUSED, end_inputs = IR_UNUSED;
	ir_refs *true_inputs;

	ir_refs_init(true_inputs, 8);

	// TODO: support for empty() ???
	ZEND_ASSERT(!(opline->extended_value & ZEND_ISEMPTY));

	op2_addr = OP2_ADDR();
	res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->result.var);

	if (op1_info & MAY_BE_REF) {
		ir_ref ref = jit_ZVAL_ADDR(jit, op1_addr);
		ref = jit_ZVAL_DEREF_ref(jit, ref);
		op1_addr = ZEND_ADDR_REF_ZVAL(ref);
	}

	if (op1_info & MAY_BE_ARRAY) {
		const void *found_exit_addr = NULL;
		const void *not_found_exit_addr = NULL;
		ir_ref ht_ref;

		if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF) - MAY_BE_ARRAY)) {
			if_type = jit_if_Z_TYPE(jit, op1_addr, IS_ARRAY);
			ir_IF_TRUE(if_type);
		}

		ht_ref = jit_Z_PTR(jit, op1_addr);

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
				0, ht_ref, true_inputs, NULL, &false_inputs, NULL)) {
			return 0;
		}

		if (found_exit_addr) {
			ir_MERGE_list(false_inputs);
			return 1;
		} else if (not_found_exit_addr) {
			ir_MERGE_N(true_inputs->count, true_inputs->refs);
			return 1;
		}
	}

	if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_ARRAY)) {
		if (if_type) {
			ir_IF_FALSE(if_type);
			if_type = IR_UNUSED;
		}

		if (op1_info & (MAY_BE_STRING|MAY_BE_OBJECT)) {
			ir_ref ref, arg1, arg2, if_true;

			jit_SET_EX_OPLINE(jit, opline);
			arg1 = jit_ZVAL_ADDR(jit, op1_addr);
			if (opline->op2_type == IS_CONST && Z_EXTRA_P(RT_CONSTANT(opline, opline->op2)) == ZEND_EXTRA_VALUE) {
				ZEND_ASSERT(Z_MODE(op2_addr) == IS_CONST_ZVAL);
				arg2 = ir_CONST_ADDR(Z_ZV(op2_addr)+1);
			} else {
				arg2 = jit_ZVAL_ADDR(jit, op2_addr);
			}
			ref = ir_CALL_2(IR_I32, ir_CONST_FC_FUNC(zend_jit_isset_dim_helper), arg1, arg2);
			if_true = ir_IF(ref);
			ir_IF_TRUE(if_true);
			ir_refs_add(true_inputs, ir_END());
			ir_IF_FALSE(if_true);
			ir_END_list(false_inputs);
		} else {
			if (op2_info & MAY_BE_UNDEF) {
				ir_ref end1 = IR_UNUSED;

				if (op2_info & MAY_BE_ANY) {
					ir_ref if_def = ir_IF(jit_Z_TYPE(jit, op2_addr));
					ir_IF_TRUE(if_def);
					end1 = ir_END();
					ir_IF_FALSE(if_def);
				}
				jit_SET_EX_OPLINE(jit, opline);
				ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_undefined_op_helper), ir_CONST_U32(opline->op2.var));
				if (end1) {
					ir_MERGE_WITH(end1);
				}
			}
			ir_END_list(false_inputs);
		}
	}

#ifdef ZEND_JIT_USE_RC_INFERENCE
	if ((opline->op2_type & (IS_TMP_VAR|IS_VAR)) && (op1_info & MAY_BE_OBJECT)) {
		/* Magic offsetExists() may increase refcount of the key */
		op2_info |= MAY_BE_RCN;
	}
#endif

	if (op1_info & (MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_OBJECT)) {
		ir_MERGE_N(true_inputs->count, true_inputs->refs);

		jit_FREE_OP(jit, opline->op2_type, opline->op2, op2_info, opline);
		if (!op1_avoid_refcounting) {
			jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
		}
		if (may_throw) {
			zend_jit_check_exception_undef_result(jit, opline);
		}
		if (!(opline->extended_value & ZEND_ISEMPTY)) {
			if (exit_addr) {
				if (smart_branch_opcode == ZEND_JMPNZ) {
					jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
				} else {
					ir_END_list(end_inputs);
				}
			} else if (smart_branch_opcode) {
				if (smart_branch_opcode == ZEND_JMPZ) {
					_zend_jit_add_predecessor_ref(jit, target_label2, jit->b, ir_END());
				} else if (smart_branch_opcode == ZEND_JMPNZ) {
					_zend_jit_add_predecessor_ref(jit, target_label, jit->b, ir_END());
				} else {
					ZEND_UNREACHABLE();
				}
			} else {
				jit_set_Z_TYPE_INFO(jit, res_addr, IS_TRUE);
				ir_END_list(end_inputs);
			}
		} else {
			ZEND_ASSERT(0); // TODO: support for empty()
		}
	}

	ir_MERGE_list(false_inputs);
	jit_FREE_OP(jit, opline->op2_type, opline->op2, op2_info, opline);
	if (!op1_avoid_refcounting) {
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
	}
	if (may_throw) {
		zend_jit_check_exception_undef_result(jit, opline);
	}
	if (!(opline->extended_value & ZEND_ISEMPTY)) {
		if (exit_addr) {
			if (smart_branch_opcode == ZEND_JMPZ) {
				jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
			} else {
				ir_END_list(end_inputs);
			}
		} else if (smart_branch_opcode) {
			if (smart_branch_opcode == ZEND_JMPZ) {
				_zend_jit_add_predecessor_ref(jit, target_label, jit->b, ir_END());
			} else if (smart_branch_opcode == ZEND_JMPNZ) {
				_zend_jit_add_predecessor_ref(jit, target_label2, jit->b, ir_END());
			} else {
				ZEND_UNREACHABLE();
			}
		} else {
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_FALSE);
			ir_END_list(end_inputs);
		}
	} else {
		ZEND_ASSERT(0); // TODO: support for empty()
	}

    if (!exit_addr && smart_branch_opcode) {
		jit->ctx.control = IR_UNUSED;
		jit->b = -1;
    } else {
		ir_MERGE_list(end_inputs);
    }

	return 1;
}

static int zend_jit_assign_dim(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, uint32_t op2_info, uint32_t val_info, uint8_t dim_type, int may_throw)
{
	zend_jit_addr op2_addr, op3_addr, res_addr;
	ir_ref if_type = IR_UNUSED;
	ir_ref end_inputs = IR_UNUSED, ht_ref;

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

		jit_guard_not_Z_TYPE(jit, op3_addr, IS_UNDEF, exit_addr);

		val_info &= ~MAY_BE_UNDEF;
	}

	op1_addr = zend_jit_prepare_array_update(jit, opline, op1_info, op1_addr, &if_type, &ht_ref, &may_throw);

	if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_ARRAY)) {
		if (opline->op2_type == IS_UNUSED) {
			uint32_t var_info = MAY_BE_NULL;
			ir_ref if_ok, ref;
			zend_jit_addr var_addr;

			// JIT: var_ptr = zend_hash_next_index_insert(Z_ARRVAL_P(container), &EG(uninitialized_zval));
			ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_hash_next_index_insert),
				ht_ref, jit_EG(uninitialized_zval));

			// JIT: if (UNEXPECTED(!var_ptr)) {
			if_ok = ir_IF(ref);
			ir_IF_FALSE_cold(if_ok);

			// JIT: zend_throw_error(NULL, "Cannot add element to the array as the next element is already occupied");
			jit_SET_EX_OPLINE(jit, opline);
			ir_CALL(IR_VOID, jit_STUB_FUNC_ADDR(jit, jit_stub_cannot_add_element, IR_CONST_FASTCALL_FUNC));

			ir_END_list(end_inputs);

			ir_IF_TRUE(if_ok);
			var_addr = ZEND_ADDR_REF_ZVAL(ref);
			if (!zend_jit_simple_assign(jit, opline, var_addr, var_info, -1, (opline+1)->op1_type, op3_addr, val_info, res_addr, 0)) {
				return 0;
			}
		} else {
			uint32_t var_info = zend_array_element_type(op1_info, opline->op1_type, 0, 0);
			zend_jit_addr var_addr;
			ir_ref ref;
			ir_refs *found_inputs, *found_values;

			ir_refs_init(found_inputs, 8);
			ir_refs_init(found_values, 8);

			if (!zend_jit_fetch_dimension_address_inner(jit, opline, BP_VAR_W, op1_info, op2_info, dim_type, NULL, NULL, NULL,
					0, ht_ref, found_inputs, found_values, &end_inputs, NULL)) {
				return 0;
			}

			if (op1_info & (MAY_BE_ARRAY_OF_REF|MAY_BE_OBJECT)) {
				var_info |= MAY_BE_REF;
			}
			if (var_info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
				var_info |= MAY_BE_RC1;
			}

			ir_MERGE_N(found_inputs->count, found_inputs->refs);
			ref = ir_PHI_N(found_values->count, found_values->refs);
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

		ir_END_list(end_inputs);
	}

	if (op1_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_ARRAY))) {
		ir_ref arg2, arg4;

		if (if_type) {
			ir_IF_FALSE_cold(if_type);
			if_type = IR_UNUSED;
		}

		jit_SET_EX_OPLINE(jit, opline);

	    if (opline->op2_type == IS_UNUSED) {
			arg2 = IR_NULL;
		} else if (opline->op2_type == IS_CONST && Z_EXTRA_P(RT_CONSTANT(opline, opline->op2)) == ZEND_EXTRA_VALUE) {
				ZEND_ASSERT(Z_MODE(op2_addr) == IS_CONST_ZVAL);
			arg2 = ir_CONST_ADDR(Z_ZV(op2_addr) + 1);
		} else {
			arg2 = jit_ZVAL_ADDR(jit, op2_addr);
		}

		if (opline->result_type == IS_UNUSED) {
			arg4 = IR_NULL;
		} else {
			arg4 = jit_ZVAL_ADDR(jit, res_addr);
		}
		ir_CALL_4(IR_VOID, ir_CONST_FC_FUNC(zend_jit_assign_dim_helper),
			jit_ZVAL_ADDR(jit, op1_addr),
			arg2,
			jit_ZVAL_ADDR(jit, op3_addr),
			arg4);

#ifdef ZEND_JIT_USE_RC_INFERENCE
		if (((opline+1)->op1_type & (IS_TMP_VAR|IS_VAR)) && (val_info & MAY_BE_RC1)) {
			/* ASSIGN_DIM may increase refcount of the value */
			val_info |= MAY_BE_RCN;
		}
#endif

		jit_FREE_OP(jit, (opline+1)->op1_type, (opline+1)->op1, val_info, NULL);

		ir_END_list(end_inputs);
	}

#ifdef ZEND_JIT_USE_RC_INFERENCE
	if ((opline->op2_type & (IS_TMP_VAR|IS_VAR)) && (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_ARRAY|MAY_BE_OBJECT))) {
		/* ASSIGN_DIM may increase refcount of the key */
		op2_info |= MAY_BE_RCN;
	}
#endif

	ir_MERGE_list(end_inputs);
	jit_FREE_OP(jit, opline->op2_type, opline->op2, op2_info, opline);

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
	ir_ref end_inputs = IR_UNUSED, ht_ref;
	bool emit_fast_path = 1;

	ZEND_ASSERT(opline->result_type == IS_UNUSED);

	op2_addr = (opline->op2_type != IS_UNUSED) ? OP2_ADDR() : 0;
	op3_addr = OP1_DATA_ADDR();

	jit_SET_EX_OPLINE(jit, opline);

	op1_addr = zend_jit_prepare_array_update(jit, opline, op1_info, op1_addr, &if_type, &ht_ref, &may_throw);

	if (op1_info & (MAY_BE_UNDEF|MAY_BE_NULL|MAY_BE_ARRAY)) {
		uint32_t var_def_info = zend_array_element_type(op1_def_info, opline->op1_type, 1, 0);

		if (opline->op2_type == IS_UNUSED) {
			var_info = MAY_BE_NULL;
			ir_ref if_ok, ref;

			// JIT: var_ptr = zend_hash_next_index_insert(Z_ARRVAL_P(container), &EG(uninitialized_zval));
			ref = ir_CALL_2(IR_ADDR, ir_CONST_FC_FUNC(zend_hash_next_index_insert),
				ht_ref, jit_EG(uninitialized_zval));

			// JIT: if (UNEXPECTED(!var_ptr)) {
			if_ok = ir_IF(ref);
			ir_IF_FALSE_cold(if_ok);

			// JIT: zend_throw_error(NULL, "Cannot add element to the array as the next element is already occupied");
			ir_CALL(IR_VOID, jit_STUB_FUNC_ADDR(jit, jit_stub_cannot_add_element, IR_CONST_FASTCALL_FUNC));

			ir_END_list(end_inputs);

			ir_IF_TRUE(if_ok);
			var_addr = ZEND_ADDR_REF_ZVAL(ref);
		} else {
			ir_ref ref;
			ir_refs *found_inputs, *found_values;

			ir_refs_init(found_inputs, 8);
			ir_refs_init(found_values, 8);

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
					0, ht_ref, found_inputs, found_values, &end_inputs, NULL)) {
				return 0;
			}

			if (found_inputs->count) {
				ir_MERGE_N(found_inputs->count, found_inputs->refs);
				ref = ir_PHI_N(found_values->count, found_values->refs);
				var_addr = ZEND_ADDR_REF_ZVAL(ref);

				if (not_found_exit_addr && dim_type != IS_REFERENCE) {
					jit_guard_Z_TYPE(jit, var_addr, dim_type, not_found_exit_addr);
					var_info = (1 << dim_type) | (var_info & ~(MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF));
				}
				if (var_info & MAY_BE_REF) {
					binary_op_type binary_op = get_binary_op(opline->extended_value);
					ir_ref if_ref, if_typed, noref_path, ref_path, ref, reference, ref2, arg2;

					ref = jit_ZVAL_ADDR(jit, var_addr);
					if_ref = jit_if_Z_TYPE(jit, var_addr, IS_REFERENCE);
					ir_IF_FALSE(if_ref);
					noref_path = ir_END();
					ir_IF_TRUE(if_ref);

					reference = jit_Z_PTR_ref(jit, ref);
					ref2 = ir_ADD_OFFSET(reference, offsetof(zend_reference, val));
					if_typed = jit_if_TYPED_REF(jit, reference);
					ir_IF_FALSE(if_typed);
					ref_path = ir_END();
					ir_IF_TRUE_cold(if_typed);

					arg2 = jit_ZVAL_ADDR(jit, op3_addr);
					ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_assign_op_to_typed_ref),
						reference, arg2, ir_CONST_FC_FUNC(binary_op));

					ir_END_list(end_inputs);

					ir_MERGE_2(noref_path, ref_path);
					ref = ir_PHI_2(ref, ref2);
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

			ir_END_list(end_inputs);
		}
	}

	if (op1_info & (MAY_BE_ANY-(MAY_BE_NULL|MAY_BE_ARRAY))) {
		binary_op_type binary_op;
		ir_ref arg2;

		if (if_type) {
			ir_IF_FALSE_cold(if_type);
			if_type = IS_UNUSED;
		}

	    if (opline->op2_type == IS_UNUSED) {
			arg2 = IR_NULL;
		} else if (opline->op2_type == IS_CONST && Z_EXTRA_P(RT_CONSTANT(opline, opline->op2)) == ZEND_EXTRA_VALUE) {
			ZEND_ASSERT(Z_MODE(op2_addr) == IS_CONST_ZVAL);
			arg2 = ir_CONST_ADDR(Z_ZV(op2_addr) + 1);
		} else {
			arg2 = jit_ZVAL_ADDR(jit, op2_addr);
		}
		binary_op = get_binary_op(opline->extended_value);
		ir_CALL_4(IR_VOID, ir_CONST_FC_FUNC(zend_jit_assign_dim_op_helper),
			jit_ZVAL_ADDR(jit, op1_addr),
			arg2,
			jit_ZVAL_ADDR(jit, op3_addr),
			ir_CONST_FC_FUNC(binary_op));
		ir_END_list(end_inputs);
	}

	if (end_inputs) {
		ir_MERGE_list(end_inputs);
	}

	jit_FREE_OP(jit, (opline+1)->op1_type, (opline+1)->op1, op1_data_info, NULL);
	jit_FREE_OP(jit, opline->op2_type, opline->op2, op2_info, NULL);
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

		jit_ZVAL_COPY_CONST(jit, res_addr, MAY_BE_ANY, MAY_BE_ANY, zv, 1);
	} else {
		zend_jit_addr op1_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->op1.var);

		jit_ZVAL_COPY(jit, res_addr, -1, op1_addr, op1_info, opline->op1_type == IS_CV);
	}

	// JIT: Z_FE_POS_P(res) = 0;
	ir_STORE(ir_ADD_OFFSET(jit_FP(jit), opline->result.var + offsetof(zval, u2.fe_pos)), ir_CONST_U32(0));

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

	ref = ir_AND_U32(
		ir_LOAD_U32(ir_ADD_OFFSET(jit_Z_PTR(jit, addr), offsetof(zend_array, u.flags))),
		ir_CONST_U32(HASH_FLAG_PACKED));
	if (op_info & MAY_BE_ARRAY_PACKED) {
		ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
	} else {
		ir_GUARD_NOT(ref, ir_CONST_ADDR(exit_addr));
	}

	return 1;
}

static int zend_jit_fe_fetch(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, uint32_t op2_info, unsigned int target_label, zend_uchar exit_opcode, const void *exit_addr)
{
	zend_jit_addr op1_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, opline->op1.var);
	ir_ref ref, ht_ref, hash_pos_ref, packed_pos_ref, hash_p_ref = IR_UNUSED, packed_p_ref = IR_UNUSED, if_packed = IR_UNUSED;
	ir_ref if_def_hash = IR_UNUSED, if_def_packed = IR_UNUSED;
	ir_ref exit_inputs = IR_UNUSED;

	if (!MAY_BE_HASH(op1_info) && !MAY_BE_PACKED(op1_info)) {
		/* empty array */
		if (exit_addr) {
			if (exit_opcode == ZEND_JMP) {
				jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
			}
		} else {
			zend_basic_block *bb;

			ZEND_ASSERT(jit->b >= 0);
			bb = &jit->ssa->cfg.blocks[jit->b];
			_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, ir_END());
			jit->ctx.control = IR_UNUSED;
			jit->b = -1;
		}
		return 1;
	}

	// JIT: array = EX_VAR(opline->op1.var);
	// JIT: fe_ht = Z_ARRVAL_P(array);
	ht_ref = jit_Z_PTR(jit, op1_addr);

	if (op1_info & MAY_BE_PACKED_GUARD) {
		if (!zend_jit_packed_guard(jit, opline, opline->op1.var, op1_info)) {
			return 0;
		}
	}

	// JIT: pos = Z_FE_POS_P(array);
	hash_pos_ref = packed_pos_ref = ir_LOAD_U32(ir_ADD_OFFSET(jit_FP(jit), opline->op1.var + offsetof(zval, u2.fe_pos)));

	if (MAY_BE_HASH(op1_info)) {
		ir_ref loop_ref, pos2_ref, p2_ref;

		if (MAY_BE_PACKED(op1_info)) {
			ref = ir_AND_U32(
				ir_LOAD_U32(ir_ADD_OFFSET(ht_ref, offsetof(zend_array, u.flags))),
				ir_CONST_U32(HASH_FLAG_PACKED));
			if_packed = ir_IF(ref);
			ir_IF_FALSE(if_packed);
		}

		// JIT: p = fe_ht->arData + pos;
		if (sizeof(void*) == 8) {
			ref = ir_ZEXT_A(hash_pos_ref);
		} else {
			ref = ir_BITCAST_A(hash_pos_ref);
		}
		hash_p_ref = ir_ADD_A(
			ir_MUL_A(ref, ir_CONST_ADDR(sizeof(Bucket))),
			ir_LOAD_A(ir_ADD_OFFSET(ht_ref, offsetof(zend_array, arData))));

		loop_ref = ir_LOOP_BEGIN(ir_END());
		hash_pos_ref = ir_PHI_2(hash_pos_ref, IR_UNUSED);
		hash_p_ref = ir_PHI_2(hash_p_ref, IR_UNUSED);

		// JIT: if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
		ref = ir_ULT(hash_pos_ref,
			ir_LOAD_U32(ir_ADD_OFFSET(ht_ref, offsetof(zend_array, nNumUsed))));

		// JIT: ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
		// JIT: ZEND_VM_CONTINUE();

		if (exit_addr) {
			if (exit_opcode == ZEND_JMP) {
				ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
			} else {
				ir_ref if_fit = ir_IF(ref);
				ir_IF_FALSE(if_fit);
				ir_END_list(exit_inputs);
				ir_IF_TRUE(if_fit);
			}
		} else {
			ir_ref if_fit = ir_IF(ref);
			ir_IF_FALSE(if_fit);
			ir_END_list(exit_inputs);
			ir_IF_TRUE(if_fit);
		}

		// JIT: pos++;
		pos2_ref = ir_ADD_U32(hash_pos_ref, ir_CONST_U32(1));

		// JIT: value_type = Z_TYPE_INFO_P(value);
		// JIT: if (EXPECTED(value_type != IS_UNDEF)) {
		if (!exit_addr || exit_opcode == ZEND_JMP) {
			if_def_hash = ir_IF(jit_Z_TYPE_ref(jit, hash_p_ref));
			ir_IF_FALSE(if_def_hash);
		} else {
			ir_GUARD_NOT(jit_Z_TYPE_ref(jit, hash_p_ref), ir_CONST_ADDR(exit_addr));
		}

		// JIT: p++;
		p2_ref = ir_ADD_OFFSET(hash_p_ref, sizeof(Bucket));

		ir_MERGE_SET_OP(loop_ref, 2, ir_LOOP_END(loop_ref));
		ir_PHI_SET_OP(hash_pos_ref, 2, pos2_ref);
		ir_PHI_SET_OP(hash_p_ref, 2, p2_ref);

		if (MAY_BE_PACKED(op1_info)) {
			ir_IF_TRUE(if_packed);
		}
	}
	if (MAY_BE_PACKED(op1_info)) {
		ir_ref loop_ref, pos2_ref, p2_ref;

		// JIT: p = fe_ht->arPacked + pos;
		if (sizeof(void*) == 8) {
			ref = ir_ZEXT_A(packed_pos_ref);
		} else {
			ref = ir_BITCAST_A(packed_pos_ref);
		}
		packed_p_ref = ir_ADD_A(
			ir_MUL_A(ref, ir_CONST_ADDR(sizeof(zval))),
			ir_LOAD_A(ir_ADD_OFFSET(ht_ref, offsetof(zend_array, arPacked))));

		loop_ref = ir_LOOP_BEGIN(ir_END());
		packed_pos_ref = ir_PHI_2(packed_pos_ref, IR_UNUSED);
		packed_p_ref = ir_PHI_2(packed_p_ref, IR_UNUSED);

		// JIT: if (UNEXPECTED(pos >= fe_ht->nNumUsed)) {
		ref = ir_ULT(packed_pos_ref,
			ir_LOAD_U32(ir_ADD_OFFSET(ht_ref, offsetof(zend_array, nNumUsed))));

		// JIT: ZEND_VM_SET_RELATIVE_OPCODE(opline, opline->extended_value);
		// JIT: ZEND_VM_CONTINUE();
		if (exit_addr) {
			if (exit_opcode == ZEND_JMP) {
				ir_GUARD(ref, ir_CONST_ADDR(exit_addr));
			} else {
				ir_ref if_fit = ir_IF(ref);
				ir_IF_FALSE(if_fit);
				ir_END_list(exit_inputs);
				ir_IF_TRUE(if_fit);
			}
		} else {
			ir_ref if_fit = ir_IF(ref);
			ir_IF_FALSE(if_fit);
			ir_END_list(exit_inputs);
			ir_IF_TRUE(if_fit);
		}

		// JIT: pos++;
		pos2_ref = ir_ADD_U32(packed_pos_ref, ir_CONST_U32(1));

		// JIT: value_type = Z_TYPE_INFO_P(value);
		// JIT: if (EXPECTED(value_type != IS_UNDEF)) {
		if (!exit_addr || exit_opcode == ZEND_JMP) {
			if_def_packed = ir_IF(jit_Z_TYPE_ref(jit, packed_p_ref));
			ir_IF_FALSE(if_def_packed);
		} else {
			ir_GUARD_NOT(jit_Z_TYPE_ref(jit, packed_p_ref), ir_CONST_ADDR(exit_addr));
		}

		// JIT: p++;
		p2_ref = ir_ADD_OFFSET(packed_p_ref, sizeof(zval));

		ir_MERGE_SET_OP(loop_ref, 2, ir_LOOP_END(loop_ref));
		ir_PHI_SET_OP(packed_pos_ref, 2, pos2_ref);
		ir_PHI_SET_OP(packed_p_ref, 2, p2_ref);
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

				ZEND_ASSERT(if_def_hash);
				ir_IF_TRUE(if_def_hash);

				// JIT: Z_FE_POS_P(array) = pos + 1;
				ir_STORE(ir_ADD_OFFSET(jit_FP(jit), opline->op1.var + offsetof(zval, u2.fe_pos)),
					ir_ADD_U32(hash_pos_ref, ir_CONST_U32(1)));

				if (op1_info & MAY_BE_ARRAY_KEY_STRING) {
					key_ref = ir_LOAD_A(ir_ADD_OFFSET(hash_p_ref, offsetof(Bucket, key)));
				}
				if ((op1_info & MAY_BE_ARRAY_KEY_LONG)
				 && (op1_info & MAY_BE_ARRAY_KEY_STRING)) {
					// JIT: if (!p->key) {
					if_key = ir_IF(key_ref);
					ir_IF_TRUE(if_key);
				}
				if (op1_info & MAY_BE_ARRAY_KEY_STRING) {
					ir_ref if_interned, interned_path;

					// JIT: ZVAL_STR_COPY(EX_VAR(opline->result.var), p->key);
					jit_set_Z_PTR(jit, res_addr, key_ref);
					ref = ir_AND_U32(
						ir_LOAD_U32(ir_ADD_OFFSET(key_ref, offsetof(zend_refcounted, gc.u.type_info))),
						ir_CONST_U32(IS_STR_INTERNED));
					if_interned = ir_IF(ref);
					ir_IF_TRUE(if_interned);

					jit_set_Z_TYPE_INFO(jit, res_addr, IS_STRING);

					interned_path = ir_END();
					ir_IF_FALSE(if_interned);

					jit_GC_ADDREF(jit, key_ref);
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_STRING_EX);

					ir_MERGE_WITH(interned_path);

					if (op1_info & MAY_BE_ARRAY_KEY_LONG) {
						key_path = ir_END();
					}
				}
				if (op1_info & MAY_BE_ARRAY_KEY_LONG) {
					if (op1_info & MAY_BE_ARRAY_KEY_STRING) {
						ir_IF_FALSE(if_key);
					}
					// JIT: ZVAL_LONG(EX_VAR(opline->result.var), p->h);
					ref = ir_LOAD_L(ir_ADD_OFFSET(hash_p_ref, offsetof(Bucket, h)));
					jit_set_Z_LVAL(jit, res_addr, ref);
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);

					if (op1_info & MAY_BE_ARRAY_KEY_STRING) {
						ir_MERGE_WITH(key_path);
					}
				}
				if (MAY_BE_PACKED(op1_info)) {
					hash_path = ir_END();
				} else {
					p_ref = hash_p_ref;
				}
			}
			if (MAY_BE_PACKED(op1_info)) {
				ZEND_ASSERT(if_def_packed);
				ir_IF_TRUE(if_def_packed);

				// JIT: Z_FE_POS_P(array) = pos + 1;
				ir_STORE(ir_ADD_OFFSET(jit_FP(jit), opline->op1.var + offsetof(zval, u2.fe_pos)),
					ir_ADD_U32(packed_pos_ref, ir_CONST_U32(1)));

				// JIT: ZVAL_LONG(EX_VAR(opline->result.var), pos);
				if (sizeof(zend_long) == 8) {
					packed_pos_ref = ir_ZEXT_L(packed_pos_ref);
				} else {
					packed_pos_ref = ir_BITCAST_L(packed_pos_ref);
				}
				jit_set_Z_LVAL(jit, res_addr, packed_pos_ref);
				jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);

				if (MAY_BE_HASH(op1_info)) {
					ir_MERGE_WITH(hash_path);
					p_ref = ir_PHI_2(packed_p_ref, hash_p_ref);
				} else {
					p_ref = packed_p_ref;
				}
			}
		} else {
			ir_ref pos_ref = IR_UNUSED;

			if (if_def_hash && if_def_packed) {
				ir_IF_TRUE(if_def_hash);
				ir_MERGE_WITH_EMPTY_TRUE(if_def_packed);
				pos_ref = ir_PHI_2(hash_pos_ref, packed_pos_ref);
				p_ref = ir_PHI_2(hash_p_ref, packed_p_ref);
			} else if (if_def_hash) {
				ir_IF_TRUE(if_def_hash);
				pos_ref = hash_pos_ref;
				p_ref = hash_p_ref;
			} else if (if_def_packed) {
				ir_IF_TRUE(if_def_packed);
				pos_ref = packed_pos_ref;
				p_ref = packed_p_ref;
			} else {
				ZEND_ASSERT(0);
			}

			// JIT: Z_FE_POS_P(array) = pos + 1;
			ir_STORE(ir_ADD_OFFSET(jit_FP(jit), opline->op1.var + offsetof(zval, u2.fe_pos)),
				ir_ADD_U32(pos_ref, ir_CONST_U32(1)));
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
			jit_ZVAL_COPY(jit, var_addr, -1, val_addr, val_info, 1);
		}

		if (!exit_addr) {
			zend_basic_block *bb;

			ZEND_ASSERT(jit->b >= 0);
			bb = &jit->ssa->cfg.blocks[jit->b];
			_zend_jit_add_predecessor_ref(jit, bb->successors[1], jit->b, ir_END());
			ZEND_ASSERT(exit_inputs);
			if (!jit->ctx.ir_base[exit_inputs].op2) {
				ref = exit_inputs;
			} else {
				ir_MERGE_list(exit_inputs);
				ref = ir_END();
			}
			_zend_jit_add_predecessor_ref(jit, bb->successors[0], jit->b, ref);
			jit->ctx.control = IR_UNUSED;
			jit->b = -1;
		}
	} else {
		ZEND_ASSERT(exit_inputs);
		ir_MERGE_list(exit_inputs);
	}

	return 1;
}

static int zend_jit_load_this(zend_jit_ctx *jit, uint32_t var)
{
	zend_jit_addr this_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, offsetof(zend_execute_data, This));
	zend_jit_addr var_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, var);
	ir_ref ref = jit_Z_PTR(jit, this_addr);

	jit_set_Z_PTR(jit, var_addr, ref);
	jit_set_Z_TYPE_INFO(jit, var_addr, IS_OBJECT_EX);
	jit_GC_ADDREF(jit, ref);

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

				jit_guard_Z_TYPE(jit, this_addr, IS_OBJECT, exit_addr);

				if (JIT_G(current_frame)) {
					TRACE_FRAME_SET_THIS_CHECKED(JIT_G(current_frame));
				}
			}
		} else {
			zend_jit_addr this_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, offsetof(zend_execute_data, This));
			ir_ref if_object = jit_if_Z_TYPE(jit, this_addr, IS_OBJECT);

			ir_IF_FALSE_cold(if_object);
			jit_SET_EX_OPLINE(jit, opline);
			ir_IJMP(jit_STUB_ADDR(jit, jit_stub_invalid_this));

			ir_IF_TRUE(if_object);
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

	ir_GUARD(ir_EQ(ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_object, ce))), ir_CONST_ADDR(ce)),
		ir_CONST_ADDR(exit_addr));

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
	ir_ref end_inputs = IR_UNUSED;
	ir_ref slow_inputs = IR_UNUSED;

	ZEND_ASSERT(opline->op2_type == IS_CONST);
	ZEND_ASSERT(op1_info & MAY_BE_OBJECT);

	member = RT_CONSTANT(opline, opline->op2);
	ZEND_ASSERT(Z_TYPE_P(member) == IS_STRING && Z_STRVAL_P(member)[0] != '\0');
	prop_info = zend_get_known_property_info(op_array, ce, Z_STR_P(member), on_this, op_array->filename);

	if (on_this) {
		zend_jit_addr this_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, offsetof(zend_execute_data, This));
		obj_ref = jit_Z_PTR(jit, this_addr);
	} else {
		if (opline->op1_type == IS_VAR
		 && opline->opcode == ZEND_FETCH_OBJ_W
		 && (op1_info & MAY_BE_INDIRECT)
		 && Z_REG(op1_addr) == ZREG_FP) {
			op1_addr = jit_ZVAL_INDIRECT_DEREF(jit, op1_addr);
		}
		if (op1_info & MAY_BE_REF) {
			op1_addr = jit_ZVAL_DEREF(jit, op1_addr);
		}
		if (op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY)- MAY_BE_OBJECT)) {
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				jit_guard_Z_TYPE(jit, op1_addr, IS_OBJECT, exit_addr);
			} else {
				ir_ref if_obj = jit_if_Z_TYPE(jit, op1_addr, IS_OBJECT);

				ir_IF_FALSE_cold(if_obj);
				if (opline->opcode != ZEND_FETCH_OBJ_IS) {
					ir_ref op1_ref = IR_UNUSED;

					jit_SET_EX_OPLINE(jit, opline);
					if (opline->opcode != ZEND_FETCH_OBJ_W && (op1_info & MAY_BE_UNDEF)) {
						zend_jit_addr orig_op1_addr = OP1_ADDR();
						ir_ref fast_path = IR_UNUSED;

						if (op1_info & MAY_BE_ANY) {
							ir_ref if_def = ir_IF(jit_Z_TYPE(jit, op1_addr));
							ir_IF_TRUE(if_def);
							fast_path = ir_END();
							ir_IF_FALSE_cold(if_def);
						}
						ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_undefined_op_helper),
							ir_CONST_U32(opline->op1.var));
						if (fast_path) {
							ir_MERGE_WITH(fast_path);
						}
						op1_ref = jit_ZVAL_ADDR(jit, orig_op1_addr);
					} else {
						op1_ref = jit_ZVAL_ADDR(jit, op1_addr);
					}
					if (opline->opcode == ZEND_FETCH_OBJ_W) {
						ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_invalid_property_write),
							op1_ref, ir_CONST_ADDR(Z_STRVAL_P(member)));
						jit_set_Z_TYPE_INFO(jit, res_addr, _IS_ERROR);
					} else {
						ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_invalid_property_read),
							op1_ref, ir_CONST_ADDR(Z_STRVAL_P(member)));
						jit_set_Z_TYPE_INFO(jit, res_addr, IS_NULL);
					}
				} else {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_NULL);
				}
				ir_END_list(end_inputs);

				ir_IF_TRUE(if_obj);
			}
		}
		obj_ref = jit_Z_PTR(jit, op1_addr);
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
		ir_ref run_time_cache = ir_LOAD_A(jit_EX(run_time_cache));
		ir_ref ref = ir_LOAD_A(ir_ADD_OFFSET(run_time_cache, opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS));
		ir_ref if_same = ir_IF(ir_EQ(ref,
			ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_object, ce)))));

		ir_IF_FALSE_cold(if_same);
		ir_END_list(slow_inputs);

		ir_IF_TRUE(if_same);
		ir_ref offset_ref = ir_LOAD_A(
			ir_ADD_OFFSET(run_time_cache, (opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*)));

		may_be_dynamic = zend_may_be_dynamic_property(ce, Z_STR_P(member), opline->op1_type == IS_UNUSED, op_array->filename);
		if (may_be_dynamic) {
			ir_ref if_dynamic = ir_IF(ir_LT(offset_ref, IR_NULL));
			if (opline->opcode == ZEND_FETCH_OBJ_W) {
				ir_IF_TRUE_cold(if_dynamic);
				ir_END_list(slow_inputs);
			} else {
				ir_IF_TRUE_cold(if_dynamic);
				jit_SET_EX_OPLINE(jit, opline);

				if (opline->opcode != ZEND_FETCH_OBJ_IS) {
					ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_fetch_obj_r_dynamic),
						obj_ref, offset_ref);
				} else {
					ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_fetch_obj_is_dynamic),
						obj_ref, offset_ref);
				}
				ir_END_list(end_inputs);
			}
			ir_IF_FALSE(if_dynamic);
		}
		prop_ref = ir_ADD_A(obj_ref, offset_ref);
		prop_type_ref = jit_Z_TYPE_ref(jit, prop_ref);
		ir_ref if_def = ir_IF(prop_type_ref);
		ir_IF_FALSE_cold(if_def);
		ir_END_list(slow_inputs);
		ir_IF_TRUE(if_def);
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);
		if (opline->opcode == ZEND_FETCH_OBJ_W
		 && (!ce ||	ce_is_instanceof || (ce->ce_flags & (ZEND_ACC_HAS_TYPE_HINTS|ZEND_ACC_TRAIT)))) {
			uint32_t flags = opline->extended_value & ZEND_FETCH_OBJ_FLAGS;

			ir_ref prop_info_ref = ir_LOAD_A(
				ir_ADD_OFFSET(run_time_cache, (opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*) * 2));
			ir_ref if_has_prop_info = ir_IF(prop_info_ref);

			ir_IF_TRUE_cold(if_has_prop_info);

			ir_ref if_readonly = ir_IF(
				ir_AND_U32(ir_LOAD_U32(ir_ADD_OFFSET(prop_info_ref, offsetof(zend_property_info, flags))),
					ir_CONST_U32(ZEND_ACC_READONLY)));
			ir_IF_TRUE(if_readonly);

			ir_ref if_prop_obj = jit_if_Z_TYPE(jit, prop_addr, IS_OBJECT);
			ir_IF_TRUE(if_prop_obj);
			ref = jit_Z_PTR(jit, prop_addr);
			jit_GC_ADDREF(jit, ref);
			jit_set_Z_PTR(jit, res_addr, ref);
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_OBJECT_EX);
			ir_END_list(end_inputs);

			ir_IF_FALSE_cold(if_prop_obj);
			jit_SET_EX_OPLINE(jit, opline);
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_readonly_property_modification_error), prop_info_ref);
			jit_set_Z_TYPE_INFO(jit, res_addr, _IS_ERROR);
			ir_END_list(end_inputs);

			if (flags == ZEND_FETCH_DIM_WRITE) {
				ir_IF_FALSE_cold(if_readonly);
				jit_SET_EX_OPLINE(jit, opline);
				ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_check_array_promotion),
					prop_ref, prop_info_ref);
				ir_END_list(end_inputs);
				ir_IF_FALSE(if_has_prop_info);
			} else if (flags == ZEND_FETCH_REF) {
				ir_IF_FALSE_cold(if_readonly);
				ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_create_typed_ref),
					prop_ref,
					prop_info_ref,
					jit_ZVAL_ADDR(jit, res_addr));
				ir_END_list(end_inputs);
				ir_IF_FALSE(if_has_prop_info);
			} else {
				ZEND_ASSERT(flags == 0);
				ir_IF_FALSE(if_has_prop_info);
				ir_MERGE_WITH_EMPTY_FALSE(if_readonly);
			}
		}
	} else {
		prop_ref = ir_ADD_OFFSET(obj_ref, prop_info->offset);
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);
		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
			if (opline->opcode == ZEND_FETCH_OBJ_W || !(res_info & MAY_BE_GUARD) || !JIT_G(current_frame)) {
				/* perform IS_UNDEF check only after result type guard (during deoptimization) */
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				prop_type_ref = jit_Z_TYPE_INFO(jit, prop_addr);
				ir_GUARD(prop_type_ref, ir_CONST_ADDR(exit_addr));
			}
		} else {
			prop_type_ref = jit_Z_TYPE_INFO(jit, prop_addr);
			ir_ref if_def = ir_IF(prop_type_ref);
			ir_IF_FALSE_cold(if_def);
			ir_END_list(slow_inputs);
			ir_IF_TRUE(if_def);
		}
		if (opline->opcode == ZEND_FETCH_OBJ_W && (prop_info->flags & ZEND_ACC_READONLY)) {
			if (!prop_type_ref) {
				prop_type_ref = jit_Z_TYPE_INFO(jit, prop_addr);
			}
			ir_ref if_prop_obj = jit_if_Z_TYPE(jit, prop_addr, IS_OBJECT);
			ir_IF_TRUE(if_prop_obj);
			ir_ref ref = jit_Z_PTR(jit, prop_addr);
			jit_GC_ADDREF(jit, ref);
			jit_set_Z_PTR(jit, res_addr, ref);
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_OBJECT_EX);
			ir_END_list(end_inputs);

			ir_IF_FALSE_cold(if_prop_obj);
			jit_SET_EX_OPLINE(jit, opline);
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_readonly_property_modification_error), ir_CONST_ADDR(prop_info));
			jit_set_Z_TYPE_INFO(jit, res_addr, _IS_ERROR);
			ir_END_list(end_inputs);
		} else if (opline->opcode == ZEND_FETCH_OBJ_W
		 && (opline->extended_value & ZEND_FETCH_OBJ_FLAGS)
		 && ZEND_TYPE_IS_SET(prop_info->type)) {
			uint32_t flags = opline->extended_value & ZEND_FETCH_OBJ_FLAGS;

			if (flags == ZEND_FETCH_DIM_WRITE) {
				if ((ZEND_TYPE_FULL_MASK(prop_info->type) & MAY_BE_ARRAY) == 0) {
					if (!prop_type_ref) {
						prop_type_ref = jit_Z_TYPE_INFO(jit, prop_addr);
					}
					ir_ref if_null_or_flase = ir_IF(ir_LE(prop_type_ref, ir_CONST_U32(IR_FALSE)));
					ir_IF_TRUE_cold(if_null_or_flase);
					jit_SET_EX_OPLINE(jit, opline);
					ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_check_array_promotion),
						prop_ref, ir_CONST_ADDR(prop_info));
					ir_END_list(end_inputs);
					ir_IF_FALSE(if_null_or_flase);
				}
			} else if (flags == ZEND_FETCH_REF) {
				ir_ref ref;

				if (!prop_type_ref) {
					prop_type_ref = jit_Z_TYPE_INFO(jit, prop_addr);
				}

				ir_ref if_reference = ir_IF(ir_EQ(prop_type_ref, ir_CONST_U32(IS_REFERENCE_EX)));
				ir_IF_FALSE(if_reference);
				if (ce && ce->ce_flags & ZEND_ACC_IMMUTABLE) {
					ref = ir_CONST_ADDR(prop_info);
				} else {
					int prop_info_offset =
						(((prop_info->offset - (sizeof(zend_object) - sizeof(zval))) / sizeof(zval)) * sizeof(void*));

					ref = ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_object, ce)));
					ref = ir_LOAD_A(ir_ADD_OFFSET(ref, offsetof(zend_class_entry, properties_info_table)));
					ref = ir_LOAD_A(ir_ADD_OFFSET(ref, prop_info_offset));
				}
				ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_create_typed_ref),
					prop_ref,
					ref,
					jit_ZVAL_ADDR(jit, res_addr));
				ir_END_list(end_inputs);
				ir_IF_TRUE(if_reference);
			} else {
				ZEND_UNREACHABLE();
			}
		}
	}

	if (opline->opcode == ZEND_FETCH_OBJ_W) {
		if (!prop_info || !(prop_info->flags & ZEND_ACC_READONLY)) {
			ZEND_ASSERT(prop_ref);
			jit_set_Z_PTR(jit, res_addr, prop_ref);
			jit_set_Z_TYPE_INFO(jit, res_addr, IS_INDIRECT);
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE && prop_info) {
				ssa->var_info[ssa_op->result_def].indirect_reference = 1;
			}
			ir_END_list(end_inputs);
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
			jit_ZVAL_COPY(jit, res_addr, -1, val_addr, res_info, !result_avoid_refcounting);
		} else {
			prop_type_ref = jit_Z_TYPE_INFO(jit, prop_addr);

			if (!zend_jit_zval_copy_deref(jit, res_addr, prop_addr, prop_type_ref)) {
				return 0;
			}
		}
		ir_END_list(end_inputs);
	}

	if (JIT_G(trigger) != ZEND_JIT_ON_HOT_TRACE || !prop_info) {
		ir_MERGE_list(slow_inputs);
		jit_SET_EX_OPLINE(jit, opline);

		if (opline->opcode == ZEND_FETCH_OBJ_W) {
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_fetch_obj_w_slow), obj_ref);
		} else if (opline->opcode != ZEND_FETCH_OBJ_IS) {
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_fetch_obj_r_slow), obj_ref);
		} else {
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_fetch_obj_is_slow), obj_ref);
		}
		ir_END_list(end_inputs);
	}

	ir_MERGE_list(end_inputs);

	if (opline->op1_type != IS_UNUSED && !delayed_fetch_this && !op1_indirect) {
		if (opline->op1_type == IS_VAR
		 && opline->opcode == ZEND_FETCH_OBJ_W
		 && (op1_info & MAY_BE_RC1)) {
			zend_jit_addr orig_op1_addr = OP1_ADDR();
			ir_ref if_refcounted, ptr, refcount, if_non_zero;
			ir_ref merge_inputs = IR_UNUSED;

			if_refcounted = jit_if_REFCOUNTED(jit, orig_op1_addr);
			ir_IF_FALSE( if_refcounted);
			ir_END_list(merge_inputs);
			ir_IF_TRUE( if_refcounted);
			ptr = jit_Z_PTR(jit, orig_op1_addr);
			refcount = jit_GC_DELREF(jit, ptr);
			if_non_zero = ir_IF(refcount);
			ir_IF_TRUE( if_non_zero);
			ir_END_list(merge_inputs);
			ir_IF_FALSE( if_non_zero);
			jit_SET_EX_OPLINE(jit, opline);
			ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_extract_helper), ptr);
			ir_END_list(merge_inputs);
			ir_MERGE_list(merge_inputs);
		} else if (!op1_avoid_refcounting) {
			if (on_this) {
				op1_info &= ~MAY_BE_RC1;
			}
			jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
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
	ir_ref end_inputs = IR_UNUSED;
	ir_ref slow_inputs = IR_UNUSED;

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
		obj_ref = jit_Z_PTR(jit, this_addr);
	} else {
		if (opline->op1_type == IS_VAR
		 && (op1_info & MAY_BE_INDIRECT)
		 && Z_REG(op1_addr) == ZREG_FP) {
			op1_addr = jit_ZVAL_INDIRECT_DEREF(jit, op1_addr);
		}
		if (op1_info & MAY_BE_REF) {
			op1_addr = jit_ZVAL_DEREF(jit, op1_addr);
		}
		if (op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY)- MAY_BE_OBJECT)) {
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				jit_guard_Z_TYPE(jit, op1_addr, IS_OBJECT, exit_addr);
			} else {
				ir_ref if_obj = jit_if_Z_TYPE(jit, op1_addr, IS_OBJECT);
				ir_IF_FALSE_cold(if_obj);

				jit_SET_EX_OPLINE(jit, opline);
				ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_invalid_property_assign),
					jit_ZVAL_ADDR(jit, op1_addr),
					ir_CONST_ADDR(ZSTR_VAL(name)));

				if (RETURN_VALUE_USED(opline)) {
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_NULL);
				}

				ir_END_list(end_inputs);

				ir_IF_TRUE(if_obj);
			}
		}
		obj_ref = jit_Z_PTR(jit, op1_addr);
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
		ir_ref run_time_cache = ir_LOAD_A(jit_EX(run_time_cache));
		ir_ref ref = ir_LOAD_A(ir_ADD_OFFSET(run_time_cache, opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS));
		ir_ref if_same = ir_IF(ir_EQ(ref, ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_object, ce)))));

		ir_IF_FALSE_cold(if_same);
		ir_END_list(slow_inputs);

		ir_IF_TRUE(if_same);
		ir_ref offset_ref = ir_LOAD_A(
			ir_ADD_OFFSET(run_time_cache, (opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*)));

		ir_ref if_dynamic = ir_IF(ir_LT(offset_ref, IR_NULL));
		ir_IF_TRUE_cold(if_dynamic);
		ir_END_list(slow_inputs);

		ir_IF_FALSE(if_dynamic);
		prop_ref = ir_ADD_A(obj_ref, offset_ref);
		ir_ref if_def = ir_IF(jit_Z_TYPE_ref(jit, prop_ref));
		ir_IF_FALSE_cold(if_def);
		ir_END_list(slow_inputs);

		ir_IF_TRUE(if_def);
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);

		if (!ce || ce_is_instanceof || (ce->ce_flags & (ZEND_ACC_HAS_TYPE_HINTS|ZEND_ACC_TRAIT))) {
			ir_ref prop_info_ref = ir_LOAD_A(
				ir_ADD_OFFSET(run_time_cache, (opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*) * 2));
			ir_ref if_has_prop_info = ir_IF(prop_info_ref);
			ir_IF_TRUE_cold(if_has_prop_info);

			// JIT: value = zend_assign_to_typed_prop(prop_info, property_val, value EXECUTE_DATA_CC);
			jit_SET_EX_OPLINE(jit, opline);
			ir_CALL_4(IR_VOID, ir_CONST_FC_FUNC(zend_jit_assign_to_typed_prop),
				prop_ref,
				prop_info_ref,
				jit_ZVAL_ADDR(jit, val_addr),
				RETURN_VALUE_USED(opline) ? jit_ZVAL_ADDR(jit, res_addr) : IR_NULL);

			if ((opline+1)->op1_type == IS_CONST) {
				// TODO: ???
				// if (Z_TYPE_P(value) == orig_type) {
				// CACHE_PTR_EX(cache_slot + 2, NULL);
			}

			ir_END_list(end_inputs);
			ir_IF_FALSE(if_has_prop_info);
		}
	} else {
		prop_ref = ir_ADD_OFFSET(obj_ref, prop_info->offset);
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);
		if (!ce || ce_is_instanceof || !(ce->ce_flags & ZEND_ACC_IMMUTABLE) || ce->__get || ce->__set || (prop_info->flags & ZEND_ACC_READONLY)) {
			// Undefined property with magic __get()/__set()
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				ir_GUARD(jit_Z_TYPE_INFO(jit, prop_addr), ir_CONST_ADDR(exit_addr));
			} else {
				ir_ref if_def = ir_IF(jit_Z_TYPE_INFO(jit, prop_addr));
				ir_IF_FALSE_cold(if_def);
				ir_END_list(slow_inputs);
				ir_IF_TRUE(if_def);
			}
		}
		if (ZEND_TYPE_IS_SET(prop_info->type)) {
			ir_ref ref;

			// JIT: value = zend_assign_to_typed_prop(prop_info, property_val, value EXECUTE_DATA_CC);
			jit_SET_EX_OPLINE(jit, opline);
			if (ce && ce->ce_flags & ZEND_ACC_IMMUTABLE) {
				ref = ir_CONST_ADDR(prop_info);
			} else {
				int prop_info_offset =
					(((prop_info->offset - (sizeof(zend_object) - sizeof(zval))) / sizeof(zval)) * sizeof(void*));

				ref = ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_object, ce)));
				ref = ir_LOAD_A(ir_ADD_OFFSET(ref, offsetof(zend_class_entry, properties_info_table)));
				ref = ir_LOAD_A(ir_ADD_OFFSET(ref, prop_info_offset));
			}
			ir_CALL_4(IR_VOID, ir_CONST_FC_FUNC(zend_jit_assign_to_typed_prop),
				prop_ref,
				ref,
				jit_ZVAL_ADDR(jit, val_addr),
				RETURN_VALUE_USED(opline) ? jit_ZVAL_ADDR(jit, res_addr) : IR_NULL);

			ir_END_list(end_inputs);
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
		if (end_inputs || slow_inputs) {
			if (((opline+1)->op1_type & (IS_VAR|IS_TMP_VAR))
			 && (val_info & (MAY_BE_REF|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
				/* skip FREE_OP_DATA() */
				delayed_end_input = ir_END();
			} else {
				ir_END_list(end_inputs);
			}
		}
	}

	if (slow_inputs) {
		ir_MERGE_list(slow_inputs);
		jit_SET_EX_OPLINE(jit, opline);

		// JIT: value = zobj->handlers->write_property(zobj, name, value, CACHE_ADDR(opline->extended_value));
		ir_ref run_time_cache = ir_LOAD_A(jit_EX(run_time_cache));
		ir_CALL_5(IR_VOID, ir_CONST_FC_FUNC(zend_jit_assign_obj_helper),
			obj_ref,
			ir_CONST_ADDR(name),
			jit_ZVAL_ADDR(jit, val_addr),
			ir_ADD_OFFSET(run_time_cache, opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS),
			RETURN_VALUE_USED(opline) ? jit_ZVAL_ADDR(jit, res_addr) : IR_NULL);

		ir_END_list(end_inputs);
	}

	if (end_inputs) {
		ir_MERGE_list(end_inputs);

		if (val_info & (MAY_BE_REF|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
			val_info |= MAY_BE_RC1|MAY_BE_RCN;
		}
		jit_FREE_OP(jit, (opline+1)->op1_type, (opline+1)->op1, val_info, opline);

		if (delayed_end_input) {
			ir_MERGE_WITH(delayed_end_input);
		}
	}

	if (opline->op1_type != IS_UNUSED && !delayed_fetch_this && !op1_indirect) {
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
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
	ir_ref end_inputs = IR_UNUSED;
	ir_ref slow_inputs = IR_UNUSED;

	ZEND_ASSERT(opline->op2_type == IS_CONST);
	ZEND_ASSERT(op1_info & MAY_BE_OBJECT);
	ZEND_ASSERT(opline->result_type == IS_UNUSED);

	member = RT_CONSTANT(opline, opline->op2);
	ZEND_ASSERT(Z_TYPE_P(member) == IS_STRING && Z_STRVAL_P(member)[0] != '\0');
	name = Z_STR_P(member);
	prop_info = zend_get_known_property_info(op_array, ce, name, on_this, op_array->filename);

	if (on_this) {
		zend_jit_addr this_addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, offsetof(zend_execute_data, This));
		obj_ref = jit_Z_PTR(jit, this_addr);
	} else {
		if (opline->op1_type == IS_VAR
		 && (op1_info & MAY_BE_INDIRECT)
		 && Z_REG(op1_addr) == ZREG_FP) {
			op1_addr = jit_ZVAL_INDIRECT_DEREF(jit, op1_addr);
		}
		if (op1_info & MAY_BE_REF) {
			op1_addr = jit_ZVAL_DEREF(jit, op1_addr);
		}
		if (op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY)- MAY_BE_OBJECT)) {
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				jit_guard_Z_TYPE(jit, op1_addr, IS_OBJECT, exit_addr);
			} else {
				ir_ref if_obj = jit_if_Z_TYPE(jit, op1_addr, IS_OBJECT);
				ir_IF_FALSE_cold(if_obj);

				jit_SET_EX_OPLINE(jit, opline);
				ir_CALL_2(IR_VOID,
					(op1_info & MAY_BE_UNDEF) ?
						ir_CONST_FC_FUNC(zend_jit_invalid_property_assign_op) :
						ir_CONST_FC_FUNC(zend_jit_invalid_property_assign),
					jit_ZVAL_ADDR(jit, op1_addr),
					ir_CONST_ADDR(ZSTR_VAL(name)));

				may_throw = 1;

				ir_END_list(end_inputs);
				ir_IF_TRUE(if_obj);
			}
		}
		obj_ref = jit_Z_PTR(jit, op1_addr);
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
		ir_ref run_time_cache = ir_LOAD_A(jit_EX(run_time_cache));
		ir_ref ref = ir_LOAD_A(ir_ADD_OFFSET(run_time_cache, (opline+1)->extended_value & ~ZEND_FETCH_OBJ_FLAGS));
		ir_ref if_same = ir_IF(ir_EQ(ref, ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_object, ce)))));

		ir_IF_FALSE_cold(if_same);
		ir_END_list(slow_inputs);

		ir_IF_TRUE(if_same);
		if (!ce || ce_is_instanceof || (ce->ce_flags & (ZEND_ACC_HAS_TYPE_HINTS|ZEND_ACC_TRAIT))) {
			ir_ref prop_info_ref = ir_LOAD_A(
				ir_ADD_OFFSET(run_time_cache, ((opline+1)->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*) * 2));
			ir_ref if_has_prop_info = ir_IF(prop_info_ref);
			ir_IF_TRUE_cold(if_has_prop_info);
			ir_END_list(slow_inputs);

			ir_IF_FALSE(if_has_prop_info);
		}
		ir_ref offset_ref = ir_LOAD_A(
			ir_ADD_OFFSET(run_time_cache, ((opline+1)->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*)));

		ir_ref if_dynamic = ir_IF(ir_LT(offset_ref, IR_NULL));
		ir_IF_TRUE_cold(if_dynamic);
		ir_END_list(slow_inputs);

		ir_IF_FALSE(if_dynamic);

		prop_ref = ir_ADD_A(obj_ref, offset_ref);
		if (!use_prop_guard) {
			ir_ref if_def = ir_IF(jit_Z_TYPE_ref(jit, prop_ref));
			ir_IF_FALSE_cold(if_def);
			ir_END_list(slow_inputs);

			ir_IF_TRUE(if_def);
		}
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);
	} else {
		prop_ref = ir_ADD_OFFSET(obj_ref, prop_info->offset);
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);

		if (ZEND_TYPE_IS_SET(prop_info->type) || !use_prop_guard) {
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				ir_GUARD(jit_Z_TYPE_INFO(jit, prop_addr), ir_CONST_ADDR(exit_addr));
			} else {
				ir_ref if_def = ir_IF(jit_Z_TYPE_INFO(jit, prop_addr));
				ir_IF_FALSE_cold(if_def);
				ir_END_list(slow_inputs);
				ir_IF_TRUE(if_def);
			}
		}
		if (ZEND_TYPE_IS_SET(prop_info->type)) {
			ir_ref if_ref, if_typed, noref_path, ref_path, reference, ref;

			may_throw = 1;

			jit_SET_EX_OPLINE(jit, opline);

			if_ref = jit_if_Z_TYPE(jit, prop_addr, IS_REFERENCE);
			ir_IF_FALSE(if_ref);
			noref_path = ir_END();
			ir_IF_TRUE(if_ref);

			reference = jit_Z_PTR(jit, prop_addr);
			ref = ir_ADD_OFFSET(reference, offsetof(zend_reference, val));
			if_typed = jit_if_TYPED_REF(jit, reference);
			ir_IF_FALSE(if_typed);
			ref_path = ir_END();
			ir_IF_TRUE_cold(if_typed);

			ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_assign_op_to_typed_ref),
				reference,
				jit_ZVAL_ADDR(jit, val_addr),
				ir_CONST_FC_FUNC(binary_op));

			ir_END_list(end_inputs);

			ir_MERGE_2(noref_path, ref_path);
			prop_ref = ir_PHI_2(prop_ref, ref);
			prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);

			// JIT: value = zend_assign_to_typed_prop(prop_info, property_val, value EXECUTE_DATA_CC);
			if (ce && ce->ce_flags & ZEND_ACC_IMMUTABLE) {
				ref = ir_CONST_ADDR(prop_info);
			} else {
				int prop_info_offset =
					(((prop_info->offset - (sizeof(zend_object) - sizeof(zval))) / sizeof(zval)) * sizeof(void*));

				ref = ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_object, ce)));
				ref = ir_LOAD_A(ir_ADD_OFFSET(ref, offsetof(zend_class_entry, properties_info_table)));
				ref = ir_LOAD_A(ir_ADD_OFFSET(ref, prop_info_offset));
			}

			ir_CALL_4(IR_VOID, ir_CONST_FC_FUNC(zend_jit_assign_op_to_typed_prop),
				prop_ref,
				ref,
				jit_ZVAL_ADDR(jit, val_addr),
				ir_CONST_FC_FUNC(binary_op));

			ir_END_list(end_inputs);
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

			jit_guard_Z_TYPE(jit, prop_addr, prop_type, exit_addr);
			var_info = (1 << prop_type) | (var_info & ~(MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF));
		}

		if (var_info & MAY_BE_REF) {
			ir_ref if_ref, if_typed, noref_path, ref_path, reference, ref;

			may_throw = 1;

			if_ref = jit_if_Z_TYPE(jit, prop_addr, IS_REFERENCE);
			ir_IF_FALSE(if_ref);
			noref_path = ir_END();
			ir_IF_TRUE(if_ref);

			reference = jit_Z_PTR(jit, var_addr);
			ref = ir_ADD_OFFSET(reference, offsetof(zend_reference, val));
			if_typed = jit_if_TYPED_REF(jit, reference);
			ir_IF_FALSE(if_typed);
			ref_path = ir_END();
			ir_IF_TRUE_cold(if_typed);

			jit_SET_EX_OPLINE(jit, opline);

			ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(zend_jit_assign_op_to_typed_ref),
				reference,
				jit_ZVAL_ADDR(jit, val_addr),
				ir_CONST_FC_FUNC(binary_op));

			ir_END_list(end_inputs);

			ir_MERGE_2(noref_path, ref_path);
			prop_ref = ir_PHI_2(prop_ref, ref);
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
		if (end_inputs || slow_inputs) {
			ir_END_list(end_inputs);
		}
	}

	if (slow_inputs) {
		ir_MERGE_list(slow_inputs);

		may_throw = 1;

		jit_SET_EX_OPLINE(jit, opline);
		ir_ref run_time_cache = ir_LOAD_A(jit_EX(run_time_cache));
		ir_CALL_5(IR_VOID, ir_CONST_FC_FUNC(zend_jit_assign_obj_op_helper),
			obj_ref,
			ir_CONST_ADDR(name),
			jit_ZVAL_ADDR(jit, val_addr),
			ir_ADD_OFFSET(run_time_cache, (opline+1)->extended_value & ~ZEND_FETCH_OBJ_FLAGS),
			ir_CONST_FC_FUNC(binary_op));

		ir_END_list(end_inputs);
	}

	if (end_inputs) {
		ir_MERGE_list(end_inputs);
	}

	if (val_info & (MAY_BE_REF|MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
		val_info |= MAY_BE_RC1|MAY_BE_RCN;
	}

	// JIT: FREE_OP_DATA();
	jit_FREE_OP(jit, (opline+1)->op1_type, (opline+1)->op1, val_info, opline);

	if (opline->op1_type != IS_UNUSED && !delayed_fetch_this && !op1_indirect) {
		if ((op1_info & MAY_HAVE_DTOR) && (op1_info & MAY_BE_RC1)) {
			may_throw = 1;
		}
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
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
	ir_ref end_inputs = IR_UNUSED;
	ir_ref slow_inputs = IR_UNUSED;

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
		obj_ref = jit_Z_PTR(jit, this_addr);
	} else {
		if (opline->op1_type == IS_VAR
		 && (op1_info & MAY_BE_INDIRECT)
		 && Z_REG(op1_addr) == ZREG_FP) {
			op1_addr = jit_ZVAL_INDIRECT_DEREF(jit, op1_addr);
		}
		if (op1_info & MAY_BE_REF) {
			op1_addr = jit_ZVAL_DEREF(jit, op1_addr);
		}
		if (op1_info & ((MAY_BE_UNDEF|MAY_BE_ANY)- MAY_BE_OBJECT)) {
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				jit_guard_Z_TYPE(jit, op1_addr, IS_OBJECT, exit_addr);
			} else {
				ir_ref if_obj = jit_if_Z_TYPE(jit, op1_addr, IS_OBJECT);
				ir_IF_FALSE_cold(if_obj);

				jit_SET_EX_OPLINE(jit, opline);
				ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_invalid_property_incdec),
					jit_ZVAL_ADDR(jit, op1_addr),
					ir_CONST_ADDR(ZSTR_VAL(name)));

				may_throw = 1;

				if ((opline->op1_type & (IS_VAR|IS_TMP_VAR)) && !delayed_fetch_this && !op1_indirect) {
					ir_END_list(end_inputs);
				} else {
					ir_IJMP(jit_STUB_ADDR(jit, jit_stub_exception_handler));
				}
				ir_IF_TRUE(if_obj);
			}
		}
		obj_ref = jit_Z_PTR(jit, op1_addr);
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
		ir_ref run_time_cache = ir_LOAD_A(jit_EX(run_time_cache));
		ir_ref ref = ir_LOAD_A(ir_ADD_OFFSET(run_time_cache, opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS));
		ir_ref if_same = ir_IF(ir_EQ(ref, ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_object, ce)))));

		ir_IF_FALSE_cold(if_same);
		ir_END_list(slow_inputs);

		ir_IF_TRUE(if_same);
		if (!ce || ce_is_instanceof || (ce->ce_flags & (ZEND_ACC_HAS_TYPE_HINTS|ZEND_ACC_TRAIT))) {
			ir_ref prop_info_ref = ir_LOAD_A(
				ir_ADD_OFFSET(run_time_cache, (opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*) * 2));
			ir_ref if_has_prop_info = ir_IF(prop_info_ref);
			ir_IF_TRUE_cold(if_has_prop_info);
			ir_END_list(slow_inputs);

			ir_IF_FALSE(if_has_prop_info);
		}
		ir_ref offset_ref = ir_LOAD_A(
			ir_ADD_OFFSET(run_time_cache, (opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS) + sizeof(void*)));

		ir_ref if_dynamic = ir_IF(ir_LT(offset_ref, IR_NULL));
		ir_IF_TRUE_cold(if_dynamic);
		ir_END_list(slow_inputs);

		ir_IF_FALSE(if_dynamic);

		prop_ref = ir_ADD_A(obj_ref, offset_ref);
		if (!use_prop_guard) {
			ir_ref if_def = ir_IF(jit_Z_TYPE_ref(jit, prop_ref));
			ir_IF_FALSE_cold(if_def);
			ir_END_list(slow_inputs);

			ir_IF_TRUE(if_def);
		}
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);
	} else {
		prop_ref = ir_ADD_OFFSET(obj_ref, prop_info->offset);
		prop_addr = ZEND_ADDR_REF_ZVAL(prop_ref);

		if (ZEND_TYPE_IS_SET(prop_info->type) || !use_prop_guard) {
			if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
				int32_t exit_point = zend_jit_trace_get_exit_point(opline, ZEND_JIT_EXIT_TO_VM);
				const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);

				if (!exit_addr) {
					return 0;
				}
				ir_GUARD(jit_Z_TYPE_INFO(jit, prop_addr), ir_CONST_ADDR(exit_addr));
			} else {
				ir_ref if_def = ir_IF(jit_Z_TYPE_INFO(jit, prop_addr));
				ir_IF_FALSE_cold(if_def);
				ir_END_list(slow_inputs);
				ir_IF_TRUE(if_def);
			}
		}

		if (ZEND_TYPE_IS_SET(prop_info->type)) {
			const void *func;
			ir_ref ref;

			may_throw = 1;
			jit_SET_EX_OPLINE(jit, opline);

			if (ce && ce->ce_flags & ZEND_ACC_IMMUTABLE) {
				ref = ir_CONST_ADDR(prop_info);
			} else {
				int prop_info_offset =
					(((prop_info->offset - (sizeof(zend_object) - sizeof(zval))) / sizeof(zval)) * sizeof(void*));

				ref = ir_LOAD_A(ir_ADD_OFFSET(obj_ref, offsetof(zend_object, ce)));
				ref = ir_LOAD_A(ir_ADD_OFFSET(ref, offsetof(zend_class_entry, properties_info_table)));
				ref = ir_LOAD_A(ir_ADD_OFFSET(ref, prop_info_offset));
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

				ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(func), prop_ref, ref);
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
				ir_CALL_3(IR_VOID, ir_CONST_FC_FUNC(func),
					prop_ref,
					ref,
					jit_ZVAL_ADDR(jit, res_addr));
			}
			ir_END_list(end_inputs);
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

			jit_guard_Z_TYPE(jit, prop_addr, prop_type, exit_addr);
			var_info = (1 << prop_type) | (var_info & ~(MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF));
		}

		if (var_info & MAY_BE_REF) {
			const void *func;
			ir_ref if_ref, if_typed, noref_path, ref_path, reference, ref;

			if_ref = jit_if_Z_TYPE(jit, prop_addr, IS_REFERENCE);
			ir_IF_FALSE(if_ref);
			noref_path = ir_END();
			ir_IF_TRUE(if_ref);

			reference = jit_Z_PTR(jit, var_addr);
			ref = ir_ADD_OFFSET(reference, offsetof(zend_reference, val));
			if_typed = jit_if_TYPED_REF(jit, reference);
			ir_IF_FALSE(if_typed);
			ref_path = ir_END();
			ir_IF_TRUE_cold(if_typed);

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
			jit_SET_EX_OPLINE(jit, opline);
			ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(func),
				reference,
				(opline->result_type == IS_UNUSED) ? IR_NULL : jit_ZVAL_ADDR(jit, res_addr));

			ir_END_list(end_inputs);

			ir_MERGE_2(noref_path, ref_path);
			prop_ref = ir_PHI_2(prop_ref, ref);
			var_addr = ZEND_ADDR_REF_ZVAL(prop_ref);

			var_info &= ~MAY_BE_REF;
		}

		if (var_info & MAY_BE_LONG) {
			ir_ref addr, ref;

			if (var_info & (MAY_BE_ANY - MAY_BE_LONG)) {
				if_long = jit_if_Z_TYPE(jit, var_addr, IS_LONG);
				ir_IF_TRUE(if_long);
			}

			addr = jit_ZVAL_ADDR(jit, var_addr);
			ref = ir_LOAD_L(addr);
			if (opline->opcode == ZEND_POST_INC_OBJ || opline->opcode == ZEND_POST_DEC_OBJ) {
				if (opline->result_type != IS_UNUSED) {
					jit_set_Z_LVAL(jit, res_addr, ref);
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
				}
			}
			if (opline->opcode == ZEND_PRE_INC_OBJ || opline->opcode == ZEND_POST_INC_OBJ) {
				ref = ir_ADD_OV_L(ref, ir_CONST_LONG(1));
			} else {
				ref = ir_SUB_OV_L(ref, ir_CONST_LONG(1));
			}

			ir_STORE(addr, ref);
			if_overflow = ir_IF(ir_OVERFLOW(ref));
			ir_IF_FALSE(if_overflow);

			if (opline->opcode == ZEND_PRE_INC_OBJ || opline->opcode == ZEND_PRE_DEC_OBJ) {
				if (opline->result_type != IS_UNUSED) {
					jit_set_Z_LVAL(jit, res_addr, ref);
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_LONG);
				}
			}
			ir_END_list(end_inputs);
		}

		if (var_info & (MAY_BE_ANY - MAY_BE_LONG)) {
			if (var_info & (MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
				may_throw = 1;
			}
			if (if_long) {
				ir_IF_FALSE_cold(if_long);
			}
			if (opline->opcode == ZEND_POST_INC_OBJ || opline->opcode == ZEND_POST_DEC_OBJ) {
				jit_ZVAL_COPY(jit, res_addr, -1, var_addr, var_info, 1);
			}
			if (opline->opcode == ZEND_PRE_INC_OBJ || opline->opcode == ZEND_POST_INC_OBJ) {
				if (opline->opcode == ZEND_PRE_INC_OBJ && opline->result_type != IS_UNUSED) {
					ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_pre_inc),
						jit_ZVAL_ADDR(jit, var_addr),
						jit_ZVAL_ADDR(jit, res_addr));
				} else {
					ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(increment_function),
						jit_ZVAL_ADDR(jit, var_addr));
				}
			} else {
				if (opline->opcode == ZEND_PRE_DEC_OBJ && opline->result_type != IS_UNUSED) {
					ir_CALL_2(IR_VOID, ir_CONST_FC_FUNC(zend_jit_pre_dec),
						jit_ZVAL_ADDR(jit, var_addr),
						jit_ZVAL_ADDR(jit, res_addr));
				} else {
					ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(decrement_function),
						jit_ZVAL_ADDR(jit, var_addr));
				}
			}

			ir_END_list(end_inputs);
		}
		if (var_info & MAY_BE_LONG) {
			ir_IF_TRUE_cold(if_overflow);
			if (opline->opcode == ZEND_PRE_INC_OBJ || opline->opcode == ZEND_POST_INC_OBJ) {
#if SIZEOF_ZEND_LONG == 4
				jit_set_Z_LVAL(jit, var_addr, ir_CONST_LONG(0));
				jit_set_Z_W2(jit, var_addr, ir_CONST_U32(0x41e00000));
#else
				jit_set_Z_LVAL(jit, var_addr, ir_CONST_LONG(0x43e0000000000000));
#endif
				jit_set_Z_TYPE_INFO(jit, var_addr, IS_DOUBLE);
				if (opline->opcode == ZEND_PRE_INC_OBJ && opline->result_type != IS_UNUSED) {
#if SIZEOF_ZEND_LONG == 4
					jit_set_Z_LVAL(jit, res_addr, ir_CONST_LONG(0));
					jit_set_Z_W2(jit, res_addr, ir_CONST_U32(0x41e00000));
#else
					jit_set_Z_LVAL(jit, res_addr, ir_CONST_LONG(0x43e0000000000000));
#endif
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_DOUBLE);
				}
			} else {
#if SIZEOF_ZEND_LONG == 4
				jit_set_Z_LVAL(jit, var_addr, ir_CONST_LONG(0x00200000));
				jit_set_Z_W2(jit, var_addr, ir_CONST_U32(0xc1e00000));
#else
				jit_set_Z_LVAL(jit, var_addr, ir_CONST_LONG(0xc3e0000000000000));
#endif
				jit_set_Z_TYPE_INFO(jit, var_addr, IS_DOUBLE);
				if (opline->opcode == ZEND_PRE_DEC_OBJ && opline->result_type != IS_UNUSED) {
#if SIZEOF_ZEND_LONG == 4
					jit_set_Z_LVAL(jit, res_addr, ir_CONST_LONG(0x00200000));
					jit_set_Z_W2(jit, res_addr, ir_CONST_U32(0xc1e00000));
#else
					jit_set_Z_LVAL(jit, res_addr, ir_CONST_LONG(0xc3e0000000000000));
#endif
					jit_set_Z_TYPE_INFO(jit, res_addr, IS_DOUBLE);
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
				jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
			} else {
				ir_END_list(end_inputs);
			}
		}
	}

	if (slow_inputs) {
		const void *func;

		ir_MERGE_list(slow_inputs);

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
		jit_SET_EX_OPLINE(jit, opline);
		ir_ref run_time_cache = ir_LOAD_A(jit_EX(run_time_cache));
		ir_CALL_4(IR_VOID, ir_CONST_FC_FUNC(func),
			obj_ref,
			ir_CONST_ADDR(name),
			ir_ADD_OFFSET(run_time_cache, opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS),
			(opline->result_type == IS_UNUSED) ? IR_NULL : jit_ZVAL_ADDR(jit, res_addr));

		ir_END_list(end_inputs);
	}

	if (end_inputs) {
		ir_MERGE_list(end_inputs);
	}

	if ((opline->op1_type & (IS_VAR|IS_TMP_VAR)) && !delayed_fetch_this && !op1_indirect) {
		if ((op1_info & MAY_HAVE_DTOR) && (op1_info & MAY_BE_RC1)) {
			may_throw = 1;
		}
		jit_FREE_OP(jit, opline->op1_type, opline->op1, op1_info, opline);
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
	ir_refs *slow_inputs;

	ir_refs_init(slow_inputs, 8);

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
			_zend_jit_add_predecessor_ref(jit, b, jit->b, ir_END());
			jit->ctx.control = IR_UNUSED;
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

					ref = jit_ZVAL_ADDR(jit, op1_addr);
					if_long = jit_if_Z_TYPE(jit, op1_addr, IS_LONG);
					ir_IF_TRUE(if_long);
					fast_path = ir_END();
					ir_IF_FALSE_cold(if_long);

					// JIT: ZVAL_DEREF(op)
					if (fallback_label) {
						jit_guard_Z_TYPE(jit, op1_addr, IS_REFERENCE, fallback_label);
					} else {
						ir_ref if_ref = jit_if_Z_TYPE(jit, op1_addr, IS_REFERENCE);
						ir_IF_FALSE_cold(if_ref);
						ir_refs_add(slow_inputs, ir_END());
						ir_IF_TRUE(if_ref);
					}

					ref2 = ir_ADD_OFFSET(jit_Z_PTR(jit, op1_addr), offsetof(zend_reference, val));
					op1_addr = ZEND_ADDR_REF_ZVAL(ref2);

					if (fallback_label) {
						jit_guard_Z_TYPE(jit, op1_addr, IS_LONG, fallback_label);
					} else {
						if_long = jit_if_Z_TYPE(jit, op1_addr, IS_LONG);
						ir_IF_FALSE_cold(if_long);
						ir_refs_add(slow_inputs, ir_END());
						ir_IF_TRUE(if_long);
					}

					ir_MERGE_2(fast_path, ir_END());
					ref = ir_PHI_2(ref, ref2);
					op1_addr = ZEND_ADDR_REF_ZVAL(ref);
				} else if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG)) {
					if (fallback_label) {
						jit_guard_Z_TYPE(jit, op1_addr, IS_LONG, fallback_label);
					} else {
						ir_ref if_long = jit_if_Z_TYPE(jit, op1_addr, IS_LONG);
						ir_IF_FALSE_cold(if_long);
						ir_refs_add(slow_inputs, ir_END());
						ir_IF_TRUE(if_long);
					}
				}
				ir_ref ref = jit_Z_LVAL(jit, op1_addr);

				if (!HT_IS_PACKED(jumptable)) {
					ref = ir_CALL_2(IR_LONG, ir_CONST_FC_FUNC(zend_hash_index_find),
						ir_CONST_ADDR(jumptable), ref);
					ref = ir_SUB_L(ref, ir_CONST_LONG((uintptr_t)jumptable->arData));
					ref = ir_DIV_L(ref, ir_CONST_LONG(sizeof(Bucket)));
				}
				ref = ir_SWITCH(ref);

				if (next_opline) {
					ir_ref continue_list = IR_UNUSED;

					ZEND_HASH_FOREACH_VAL(jumptable, zv) {
						ir_ref idx;
						target = ZEND_OFFSET_TO_OPLINE(opline, Z_LVAL_P(zv));

						if (HT_IS_PACKED(jumptable)) {
							idx = ir_CONST_LONG(zv - jumptable->arPacked);
						} else {
							idx = ir_CONST_LONG((Bucket*)zv - jumptable->arData);
						}
						ir_CASE_VAL(ref, idx);
						if (target == next_opline) {
							ir_END_list(continue_list);
						} else {
							exit_point = zend_jit_trace_get_exit_point(target, 0);
							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								return 0;
							}
							jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
						}
					} ZEND_HASH_FOREACH_END();

					ir_CASE_DEFAULT(ref);
					if (next_opline == default_opline) {
						ir_END_list(continue_list);
					} else {
						jit_SIDE_EXIT(jit, ir_CONST_ADDR(default_label));
					}
					if (continue_list) {
						ir_MERGE_list(continue_list);
					} else {
						ZEND_ASSERT(slow_inputs->count);
						ir_MERGE_N(slow_inputs->count, slow_inputs->refs);
					}
				} else {
					ZEND_HASH_FOREACH_VAL(jumptable, zv) {
						target = ZEND_OFFSET_TO_OPLINE(opline, Z_LVAL_P(zv));
						b = ssa->cfg.map[target - op_array->opcodes];
						_zend_jit_add_predecessor_ref(jit, b, jit->b, ref);
					} ZEND_HASH_FOREACH_END();

					_zend_jit_add_predecessor_ref(jit, default_b, jit->b, ref);
					if (slow_inputs->count) {
						ir_MERGE_N(slow_inputs->count, slow_inputs->refs);
						_zend_jit_add_predecessor_ref(jit, jit->b + 1, jit->b, ir_END());
					}
					jit->ctx.control = IR_UNUSED;
					jit->b = -1;
				}
			} else {
				ZEND_ASSERT(!next_opline);
				_zend_jit_add_predecessor_ref(jit, jit->b + 1, jit->b, ir_END());
				jit->ctx.control = IR_UNUSED;
				jit->b = -1;
			}
		} else if (opline->opcode == ZEND_SWITCH_STRING) {
			if (op1_info & MAY_BE_STRING) {
				if (op1_info & MAY_BE_REF) {
					ir_ref ref, if_string, fast_path, ref2;

					ref = jit_ZVAL_ADDR(jit, op1_addr);
					if_string = jit_if_Z_TYPE(jit, op1_addr, IS_STRING);
					ir_IF_TRUE(if_string);
					fast_path = ir_END();
					ir_IF_FALSE_cold(if_string);

					// JIT: ZVAL_DEREF(op)
					if (fallback_label) {
						jit_guard_Z_TYPE(jit, op1_addr, IS_REFERENCE, fallback_label);
					} else {
						ir_ref if_ref = jit_if_Z_TYPE(jit, op1_addr, IS_STRING);
						ir_IF_FALSE_cold(if_ref);
						ir_refs_add(slow_inputs, ir_END());
						ir_IF_TRUE(if_ref);
					}

					ref2 = ir_ADD_OFFSET(jit_Z_PTR(jit, op1_addr), offsetof(zend_reference, val));
					op1_addr = ZEND_ADDR_REF_ZVAL(ref2);

					if (fallback_label) {
						jit_guard_Z_TYPE(jit, op1_addr, IS_LONG, fallback_label);
					} else {
						if_string = jit_if_Z_TYPE(jit, op1_addr, IS_STRING);
						ir_IF_FALSE_cold(if_string);
						ir_refs_add(slow_inputs, ir_END());
						ir_IF_TRUE(if_string);
					}

					ir_MERGE_2(fast_path, ir_END());
					ref = ir_PHI_2(ref, ref2);
					op1_addr = ZEND_ADDR_REF_ZVAL(ref);
				} else if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_STRING)) {
					if (fallback_label) {
						jit_guard_Z_TYPE(jit, op1_addr, IS_STRING, fallback_label);
					} else {
						ir_ref if_string = jit_if_Z_TYPE(jit, op1_addr, IS_STRING);
						ir_IF_FALSE_cold(if_string);
						ir_refs_add(slow_inputs, ir_END());
						ir_IF_TRUE(if_string);
					}
				}

				ir_ref ref = jit_Z_PTR(jit, op1_addr);
				ref = ir_CALL_2(IR_LONG, ir_CONST_FC_FUNC(zend_hash_find),
					ir_CONST_ADDR(jumptable), ref);
				ref = ir_SUB_L(ref, ir_CONST_LONG((uintptr_t)jumptable->arData));
				ref = ir_DIV_L(ref, ir_CONST_LONG(sizeof(Bucket)));
				ref = ir_SWITCH(ref);

				if (next_opline) {
					ir_ref continue_list = IR_UNUSED;

					ZEND_HASH_FOREACH_VAL(jumptable, zv) {
						ir_ref idx;
						target = ZEND_OFFSET_TO_OPLINE(opline, Z_LVAL_P(zv));

						if (HT_IS_PACKED(jumptable)) {
							idx = ir_CONST_LONG(zv - jumptable->arPacked);
						} else {
							idx = ir_CONST_LONG((Bucket*)zv - jumptable->arData);
						}
						ir_CASE_VAL(ref, idx);
						if (target == next_opline) {
							ir_END_list(continue_list);
						} else {
							exit_point = zend_jit_trace_get_exit_point(target, 0);
							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								return 0;
							}
							jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
						}
					} ZEND_HASH_FOREACH_END();

					ir_CASE_DEFAULT(ref);
					if (next_opline == default_opline) {
						ir_END_list(continue_list);
					} else {
						jit_SIDE_EXIT(jit, ir_CONST_ADDR(default_label));
					}
					if (continue_list) {
						ir_MERGE_list(continue_list);
					} else {
						ZEND_ASSERT(slow_inputs->count);
						ir_MERGE_N(slow_inputs->count, slow_inputs->refs);
					}
				} else {
					ZEND_HASH_FOREACH_VAL(jumptable, zv) {
						target = ZEND_OFFSET_TO_OPLINE(opline, Z_LVAL_P(zv));
						b = ssa->cfg.map[target - op_array->opcodes];
						_zend_jit_add_predecessor_ref(jit, b, jit->b, ref);
					} ZEND_HASH_FOREACH_END();
					_zend_jit_add_predecessor_ref(jit, default_b, jit->b, ref);
					if (slow_inputs->count) {
						ir_MERGE_N(slow_inputs->count, slow_inputs->refs);
						_zend_jit_add_predecessor_ref(jit, jit->b + 1, jit->b, ir_END());
					}
					jit->ctx.control = IR_UNUSED;
					jit->b = -1;
				}
			} else {
				ZEND_ASSERT(!next_opline);
				_zend_jit_add_predecessor_ref(jit, jit->b + 1, jit->b, ir_END());
				jit->ctx.control = IR_UNUSED;
				jit->b = -1;
			}
		} else if (opline->opcode == ZEND_MATCH) {
			ir_ref if_type = IR_UNUSED, default_input_list = IR_UNUSED, ref = IR_UNUSED;
			ir_ref continue_list = IR_UNUSED;

			if (op1_info & (MAY_BE_LONG|MAY_BE_STRING)) {
				ir_ref long_path = IR_UNUSED;

				if (op1_info & MAY_BE_REF) {
					op1_addr = jit_ZVAL_DEREF(jit, op1_addr);
				}
				if (op1_info & MAY_BE_LONG) {
					if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-MAY_BE_LONG)) {
						if (op1_info & (MAY_BE_STRING|MAY_BE_UNDEF)) {
							if_type = jit_if_Z_TYPE(jit, op1_addr, IS_LONG);
							ir_IF_TRUE(if_type);
						} else if (default_label) {
							jit_guard_Z_TYPE(jit, op1_addr, IS_LONG, default_label);
						} else if (next_opline) {
							ir_ref if_type = jit_if_Z_TYPE(jit, op1_addr, IS_LONG);
							ir_IF_FALSE(if_type);
							ir_END_list(continue_list);
							ir_IF_TRUE(if_type);
						} else {
							ir_ref if_type = jit_if_Z_TYPE(jit, op1_addr, IS_LONG);
							ir_IF_FALSE(if_type);
							ir_END_list(default_input_list);
							ir_IF_TRUE(if_type);
						}
					}
					ref = jit_Z_LVAL(jit, op1_addr);
					ref = ir_CALL_2(IR_LONG, ir_CONST_FC_FUNC(zend_hash_index_find),
						ir_CONST_ADDR(jumptable), ref);
					if (op1_info & MAY_BE_STRING) {
						long_path = ir_END();
					}
				}
				if (op1_info & MAY_BE_STRING) {
					if (if_type) {
						ir_IF_FALSE(if_type);
						if_type = IS_UNUSED;
					}
					if (op1_info & ((MAY_BE_ANY|MAY_BE_UNDEF)-(MAY_BE_LONG|MAY_BE_STRING))) {
						if (op1_info & MAY_BE_UNDEF) {
							if_type = jit_if_Z_TYPE(jit, op1_addr, IS_STRING);
							ir_IF_TRUE(if_type);
						} else if (default_label) {
							jit_guard_Z_TYPE(jit, op1_addr, IS_STRING, default_label);
						} else if (next_opline) {
							ir_ref if_type = jit_if_Z_TYPE(jit, op1_addr, IS_STRING);
							ir_IF_FALSE(if_type);
							ir_END_list(continue_list);
							ir_IF_TRUE(if_type);
						} else {
							ir_ref if_type = jit_if_Z_TYPE(jit, op1_addr, IS_STRING);
							ir_IF_FALSE(if_type);
							ir_END_list(default_input_list);
							ir_IF_TRUE(if_type);
						}
					}
					ir_ref ref2 = jit_Z_PTR(jit, op1_addr);
					ref2 = ir_CALL_2(IR_LONG, ir_CONST_FC_FUNC(zend_hash_find),
						ir_CONST_ADDR(jumptable), ref2);
					if (op1_info & MAY_BE_LONG) {
						ir_MERGE_WITH(long_path);
						ref = ir_PHI_2(ref2, ref);
					} else {
						ref = ref2;
					}
				}

				ref = ir_SUB_L(ref, ir_CONST_LONG((uintptr_t)jumptable->arData));
				ref = ir_DIV_L(ref, ir_CONST_LONG(HT_IS_PACKED(jumptable) ? sizeof(zval) : sizeof(Bucket)));
				ref = ir_SWITCH(ref);

				if (next_opline) {
					ZEND_HASH_FOREACH_VAL(jumptable, zv) {
						ir_ref idx;
						target = ZEND_OFFSET_TO_OPLINE(opline, Z_LVAL_P(zv));

						if (HT_IS_PACKED(jumptable)) {
							idx = ir_CONST_LONG(zv - jumptable->arPacked);
						} else {
							idx = ir_CONST_LONG((Bucket*)zv - jumptable->arData);
						}
						ir_CASE_VAL(ref, idx);
						if (target == next_opline) {
							ir_END_list(continue_list);
						} else {
							exit_point = zend_jit_trace_get_exit_point(target, 0);
							exit_addr = zend_jit_trace_get_exit_addr(exit_point);
							if (!exit_addr) {
								return 0;
							}
							jit_SIDE_EXIT(jit, ir_CONST_ADDR(exit_addr));
						}
					} ZEND_HASH_FOREACH_END();

					ir_CASE_DEFAULT(ref);
					if (next_opline == default_opline) {
						ir_END_list(continue_list);
					} else {
						jit_SIDE_EXIT(jit, ir_CONST_ADDR(default_label));
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
					ir_IF_FALSE(if_type);
					if_type = IS_UNUSED;
				}
				if (op1_info & (MAY_BE_ANY-(MAY_BE_LONG|MAY_BE_STRING))) {
					if (default_label) {
						jit_guard_Z_TYPE(jit, op1_addr, IS_UNDEF, default_label);
					} else if (next_opline) {
						ir_ref if_def = ir_IF(jit_Z_TYPE(jit, op1_addr));
						ir_IF_TRUE(if_def);
						ir_END_list(continue_list);
						ir_IF_FALSE_cold(if_def);
					} else {
						ir_ref if_def = ir_IF(jit_Z_TYPE(jit, op1_addr));
						ir_IF_TRUE(if_def);
						ir_END_list(default_input_list);
						ir_IF_FALSE_cold(if_def);
					}
				}

				jit_SET_EX_OPLINE(jit, opline);
				ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_undefined_op_helper),
					ir_CONST_U32(opline->op1.var));
				zend_jit_check_exception_undef_result(jit, opline);
				if (default_label) {
					jit_SIDE_EXIT(jit, ir_CONST_ADDR(default_label));
				} else if (next_opline) {
					ir_END_list(continue_list);
				} else {
					ir_END_list(default_input_list);
				}
			}
			if (next_opline) {
				ZEND_ASSERT(continue_list);
				ir_MERGE_list(continue_list);
			} else {
				if (default_input_list) {
					ZEND_ASSERT(jit->ctx.ir_base[ref].op == IR_SWITCH);
					ZEND_ASSERT(jit->ctx.ir_base[ref].op3 == IR_UNUSED);
					jit->ctx.ir_base[ref].op3 = default_input_list;
				}
				jit->ctx.control = IR_UNUSED;
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

#ifndef _WIN32
			if (str) {
				if (JIT_G(debug) & ZEND_JIT_DEBUG_GDB) {
					uintptr_t sp_offset = 0;

//					ir_mem_unprotect(entry, size);
					if (!(jit->ctx.flags & IR_FUNCTION)
					 && zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
#if !ZEND_WIN32 && !defined(IR_TARGET_AARCH64)
						sp_offset = zend_jit_hybrid_vm_sp_adj;
#else
						sp_offset = sizeof(void*);
#endif
					} else {
						sp_offset = sizeof(void*);
					}
					ir_gdb_register(ZSTR_VAL(str), entry, size, sp_offset, 0);
//					ir_mem_protect(entry, size);
				}

				if (JIT_G(debug) & (ZEND_JIT_DEBUG_PERF|ZEND_JIT_DEBUG_PERF_DUMP)) {
					ir_perf_map_register(ZSTR_VAL(str), entry, size);
					if (JIT_G(debug) & ZEND_JIT_DEBUG_PERF_DUMP) {
						ir_perf_jitdump_register(ZSTR_VAL(str), entry, size);
					}
				}
			}
#endif
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

	if (!exit_addr) {
		return 0;
	}
	ir_GUARD(ir_EQ(jit_Z_TYPE(jit, addr), ir_CONST_U8(type)), ir_CONST_ADDR(exit_addr));

	return 1;
}

static int zend_jit_scalar_type_guard(zend_jit_ctx *jit, const zend_op *opline, uint32_t var)
{
	int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);
	const void *exit_addr = zend_jit_trace_get_exit_addr(exit_point);
	zend_jit_addr addr = ZEND_ADDR_MEM_ZVAL(ZREG_FP, var);

	if (!exit_addr) {
		return 0;
	}
	ir_GUARD(ir_LT(jit_Z_TYPE(jit, addr), ir_CONST_U8(IS_STRING)), ir_CONST_ADDR(exit_addr));

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

	if (!exit_addr) {
		return 0;
	}

	ir_GUARD(jit_CMP_IP(jit, IR_EQ, opline), ir_CONST_ADDR(exit_addr));
	zend_jit_set_last_valid_opline(jit, opline);

	return 1;
}

static bool zend_jit_guard_reference(zend_jit_ctx  *jit,
                                     const zend_op *opline,
                                     zend_jit_addr *var_addr_ptr,
                                     zend_jit_addr *ref_addr_ptr,
                                     bool           add_ref_guard)
{
	zend_jit_addr var_addr = *var_addr_ptr;
	const void *exit_addr = NULL;
	ir_ref ref;

	if (add_ref_guard) {
		int32_t exit_point = zend_jit_trace_get_exit_point(opline, 0);

		exit_addr = zend_jit_trace_get_exit_addr(exit_point);
		if (!exit_addr) {
			return 0;
		}

		ref = jit_Z_TYPE(jit, var_addr);
		ir_GUARD(ir_EQ(ref, ir_CONST_U8(IS_REFERENCE)), ir_CONST_ADDR(exit_addr));
	}

	ref = jit_Z_PTR(jit, var_addr);
	*ref_addr_ptr = ZEND_ADDR_REF_ZVAL(ref);
	ref = ir_ADD_OFFSET(ref, offsetof(zend_reference, val));
	var_addr = ZEND_ADDR_REF_ZVAL(ref);
	*var_addr_ptr = var_addr;

	return 1;
}

static bool zend_jit_fetch_reference(zend_jit_ctx  *jit,
                                     const zend_op *opline,
                                     uint8_t        var_type,
                                     uint32_t      *var_info_ptr,
                                     zend_jit_addr *var_addr_ptr,
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
		ref = jit_Z_TYPE(jit, var_addr);
		ir_GUARD(ir_EQ(ref, ir_CONST_U8(IS_REFERENCE)), ir_CONST_ADDR(exit_addr));
	}
	if (opline->opcode == ZEND_INIT_METHOD_CALL && opline->op1_type == IS_VAR) {
		/* Hack: Convert reference to regular value to simplify JIT code for INIT_METHOD_CALL */
		ir_CALL_1(IR_VOID, ir_CONST_FC_FUNC(zend_jit_unref_helper),
			jit_ZVAL_ADDR(jit, var_addr));
		*var_addr_ptr = var_addr;
	} else {
		ref = jit_Z_PTR(jit, var_addr);
		ref = ir_ADD_OFFSET(ref, offsetof(zend_reference, val));
		var_addr = ZEND_ADDR_REF_ZVAL(ref);
		*var_addr_ptr = var_addr;
	}

	if (var_type != IS_UNKNOWN) {
		var_type &= ~(IS_TRACE_REFERENCE|IS_TRACE_INDIRECT|IS_TRACE_PACKED);
	}
	if (add_type_guard
	 && var_type != IS_UNKNOWN
	 && (var_info & (MAY_BE_ANY|MAY_BE_UNDEF)) != (1 << var_type)) {
		ref = jit_Z_TYPE(jit, var_addr);
		ir_GUARD(ir_EQ(ref, ir_CONST_U8(var_type)), ir_CONST_ADDR(exit_addr));

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
		jit_guard_Z_TYPE(jit, var_addr, IS_INDIRECT, exit_addr);
		ref = jit_Z_PTR(jit, var_addr);
	} else {
		/* This LOAD of INDIRECT VAR, stored by the previous FETCH_(DIM/OBJ)_W,
		 * is eliminated by store forwarding (S2L) */
		ref = jit_Z_PTR(jit, var_addr);
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

		jit_guard_Z_TYPE(jit, var_addr, var_type, exit_addr);

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
		ir_CALL(IR_VOID, ir_CONST_FUNC(handler));
	} else {
		ref = jit_FP(jit);
		ref = ir_CALL_1(IR_I32, ir_CONST_FC_FUNC(handler), ref);
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

			ir_ref addr = jit_EG(current_execute_data);

			jit_STORE_FP(jit, ir_LOAD_A(addr));
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
					ir_GUARD(ir_NE(jit_IP(jit), ir_CONST_ADDR(zend_jit_halt_op)),
						jit_STUB_ADDR(jit, jit_stub_trace_halt));
				}
			} else if (GCC_GLOBAL_REGS) {
				ir_GUARD(jit_IP(jit), jit_STUB_ADDR(jit, jit_stub_trace_halt));
			} else {
				ir_GUARD(ir_GE(ref, ir_CONST_I32(0)), jit_STUB_ADDR(jit, jit_stub_trace_halt));
			}
		} else if (opline->opcode == ZEND_EXIT ||
		           opline->opcode == ZEND_GENERATOR_RETURN ||
		           opline->opcode == ZEND_YIELD ||
		           opline->opcode == ZEND_YIELD_FROM) {
			ir_IJMP(jit_STUB_ADDR(jit, jit_stub_trace_halt));
			ir_BEGIN(IR_UNUSED); /* unreachable block */
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
			ir_GUARD(jit_CMP_IP(jit, IR_EQ, next_opline), ir_CONST_ADDR(exit_addr));
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
	zend_jit_init_ctx(jit, (zend_jit_vm_kind == ZEND_VM_KIND_CALL) ? 0 : IR_START_BR_TARGET);

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
	zend_jit_init_ctx(jit, (zend_jit_vm_kind == ZEND_VM_KIND_CALL) ? 0 : IR_START_BR_TARGET);

	jit->ctx.spill_base = ZREG_FP;

	jit->op_array = NULL;//op_array;
	jit->ssa = ssa;
	jit->name = zend_string_copy(name);

	if (!GCC_GLOBAL_REGS) {
		if (!parent) {
			ir_ref ref = ir_PARAM(IR_ADDR, "execute_data", 1);
			jit_STORE_FP(jit, ref);
			jit->ctx.flags |= IR_FASTCALL_FUNC;
		}
	}

	if (parent) {
		jit->ctx.flags |= IR_SKIP_PROLOGUE;
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
					type = IR_LONG;
				} else if (STACK_TYPE(parent_stack, i) == IS_DOUBLE) {
					type = IR_DOUBLE;
				} else {
					ZEND_ASSERT(0);
				}
				if (ssa && ssa->vars[i].no_val) {
					/* pass */
				} else {
					ir_ref ref = ir_RLOAD(type, reg);

					if (STACK_FLAGS(parent_stack, i) & (ZREG_LOAD|ZREG_STORE)) {
						/* op3 is used as a flag that the value is already stored in memory.
						 * In case the IR framework desides to spill the result of IR_LOAD,
						 * it doesn't have to store the value once again.
						 *
						 * See: insn->op3 check in ir_emit_rload()
						 */
						ir_set_op(&jit->ctx, ref, 3, 1);
					}
				}
			}
		}
	}

	if (parent && parent->exit_info[exit_num].flags & ZEND_JIT_EXIT_METHOD_CALL) {
		ZEND_ASSERT(parent->exit_info[exit_num].poly_func_reg >= 0 && parent->exit_info[exit_num].poly_this_reg >= 0);
		ir_RLOAD_A(parent->exit_info[exit_num].poly_func_reg);
		ir_RLOAD_A(parent->exit_info[exit_num].poly_this_reg);
	}

	ir_STORE(jit_EG(jit_trace_num), ir_CONST_U32(trace_num));

	return 1;
}

static int zend_jit_trace_begin_loop(zend_jit_ctx *jit)
{
	return ir_LOOP_BEGIN(ir_END());
}

static void zend_jit_trace_gen_phi(zend_jit_ctx *jit, zend_ssa_phi *phi)
{
	int dst_var = phi->ssa_var;
	int src_var = phi->sources[0];
	ir_ref ref;

	ZEND_ASSERT(!(jit->ra[dst_var].flags & ZREG_LOAD));
	ZEND_ASSERT(jit->ra[src_var].ref != IR_UNUSED && jit->ra[src_var].ref != IR_NULL);

	ref = ir_PHI_2(zend_jit_use_reg(jit, ZEND_ADDR_REG(src_var)), IR_UNUSED);

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
	ir_MERGE_SET_OP(loop_ref, 2, ir_LOOP_END(loop_ref));
	return 1;
}

static int zend_jit_trace_return(zend_jit_ctx *jit, bool original_handler, const zend_op *opline)
{
	if (GCC_GLOBAL_REGS) {
		if (!original_handler) {
			ir_TAILCALL(ir_LOAD_A(jit_IP(jit)));
		} else {
			ir_TAILCALL(zend_jit_orig_opline_handler(jit));
		}
	} else {
		if (original_handler) {
			ir_ref ref;
			ir_ref addr = zend_jit_orig_opline_handler(jit);

#if defined(IR_TARGET_X86)
			addr = ir_CAST_FC_FUNC(addr);
#endif
			ref = ir_CALL_1(IR_I32, addr, jit_FP(jit));
			if (opline &&
			    (opline->opcode == ZEND_RETURN
			  || opline->opcode == ZEND_RETURN_BY_REF
			  || opline->opcode == ZEND_GENERATOR_RETURN
			  || opline->opcode == ZEND_GENERATOR_CREATE
			  || opline->opcode == ZEND_YIELD
			  || opline->opcode == ZEND_YIELD_FROM)) {
				ir_RETURN(ref);
			}
		}
		ir_RETURN(ir_CONST_I32(2)); // ZEND_VM_LEAVE
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
	ir_IJMP(ir_CONST_ADDR(link_addr));

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
