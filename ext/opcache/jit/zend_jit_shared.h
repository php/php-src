/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_JIT_SHARED_H
#define ZEND_JIT_SHARED_H

#include "jit/zend_jit_internal.h"

#define ZEND_JIT_USE_RC_INFERENCE

#ifdef HAVE_GCC_GLOBAL_REGS
# define GCC_GLOBAL_REGS 1
#else
# define GCC_GLOBAL_REGS 0
#endif

#if defined(IR_TARGET_X86)
# define IR_REG_SP            4 /* IR_REG_RSP */
# define IR_REG_FP            5 /* IR_REG_RBP */
# define ZREG_FP              6 /* IR_REG_RSI */
# define ZREG_IP              7 /* IR_REG_RDI */
# define ZREG_FIRST_FPR       8
# define IR_REGSET_PRESERVED ((1<<3) | (1<<5) | (1<<6) | (1<<7)) /* all preserved registers */
# if ZEND_VM_KIND == ZEND_VM_KIND_TAILCALL
#  error
# endif
#elif defined(IR_TARGET_X64)
# define IR_REG_SP            4 /* IR_REG_RSP */
# define IR_REG_FP            5 /* IR_REG_RBP */
# if ZEND_VM_KIND == ZEND_VM_KIND_TAILCALL
/* Use the first two arg registers of the preserve_none calling convention for FP/IP
 * https://github.com/llvm/llvm-project/blob/68bfe91b5a34f80dbcc4f0a7fa5d7aa1cdf959c2/llvm/lib/Target/X86/X86CallingConv.td#L1029 */
#  define ZREG_FP             12 /* IR_REG_R12 */
#  define ZREG_IP             13 /* IR_REG_R13 */
# else
#  define ZREG_FP             14 /* IR_REG_R14 */
#  define ZREG_IP             15 /* IR_REG_R15 */
# endif
# define ZREG_FIRST_FPR      16
# if defined(_WIN64)
#  define IR_REGSET_PRESERVED ((1<<3) | (1<<5) | (1<<6) | (1<<7) | (1<<12) | (1<<13) | (1<<14) | (1<<15))
/*
#  define IR_REGSET_PRESERVED ((1<<3) | (1<<5) | (1<<6) | (1<<7) | (1<<12) | (1<<13) | (1<<14) | (1<<15) | \
                               (1<<(16+6)) | (1<<(16+7)) | (1<<(16+8)) | (1<<(16+9)) | (1<<(16+10)) | \
                               (1<<(16+11)) | (1<<(16+12)) | (1<<(16+13)) | (1<<(16+14)) | (1<<(16+15)))
*/
#  define IR_SHADOW_ARGS     32
# else
#  define IR_REGSET_PRESERVED ((1<<3) | (1<<5) | (1<<12) | (1<<13) | (1<<14) | (1<<15)) /* all preserved registers */
# endif
#elif defined(IR_TARGET_AARCH64)
# define IR_REG_SP           31 /* IR_REG_RSP */
# define IR_REG_LR           30 /* IR_REG_X30 */
# define IR_REG_FP           29 /* IR_REG_X29 */
# if ZEND_VM_KIND == ZEND_VM_KIND_TAILCALL
/* Use the first two arg registers of the preserve_none calling convention for FP/IP
 * https://github.com/llvm/llvm-project/blob/68bfe91b5a34f80dbcc4f0a7fa5d7aa1cdf959c2/llvm/lib/Target/AArch64/AArch64CallingConvention.td#L541 */
#  define ZREG_FP             20 /* IR_REG_X20 */
#  define ZREG_IP             21 /* IR_REG_X21 */
# else
#  define ZREG_FP             27 /* IR_REG_X27 */
#  define ZREG_IP             28 /* IR_REG_X28 */
# endif
# define ZREG_FIRST_FPR      32
# define IR_REGSET_PRESERVED ((1<<19) | (1<<20) | (1<<21) | (1<<22) | (1<<23) | \
                              (1<<24) | (1<<25) | (1<<26) | (1<<27) | (1<<28)) /* all preserved registers */
#else
# error "Unknown IR target"
#endif

#define ZREG_RX ZREG_IP

#define OP_RANGE(ssa_op, opN) \
	(((opline->opN##_type & (IS_TMP_VAR|IS_VAR|IS_CV)) && \
	  ssa->var_info && \
	  (ssa_op)->opN##_use >= 0 && \
	  ssa->var_info[(ssa_op)->opN##_use].has_range) ? \
	 &ssa->var_info[(ssa_op)->opN##_use].range : NULL)

#define OP1_RANGE()      OP_RANGE(ssa_op, op1)
#define OP2_RANGE()      OP_RANGE(ssa_op, op2)
#define OP1_DATA_RANGE() OP_RANGE(ssa_op + 1, op1)

/* Keep 32 exit points in a single code block */
#define ZEND_JIT_EXIT_POINTS_SPACING   4  // push byte + short jmp = bytes
#define ZEND_JIT_EXIT_POINTS_PER_GROUP 32 // number of continuous exit points

#define TRACE_PREFIX    "TRACE-"

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

typedef enum _jit_stub_id {
		JIT_STUBS(JIT_STUB_ID)
		jit_last_stub
} jit_stub_id;

typedef struct _zend_jit_ctx zend_jit_ctx;

typedef struct _zend_jit_reg_var {
	ir_ref   ref;
	uint32_t flags;
} zend_jit_reg_var;

/* A helper structure to collect IT rers for the following use in (MERGE/PHI)_N */
typedef struct _ir_refs {
  uint32_t count;
  uint32_t limit;
  ir_ref   refs[] ZEND_ELEMENT_COUNT(count);
} ir_refs;

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
	ir_ref               poly_func_ref; /* restored from parent trace snapshot */
	ir_ref               poly_this_ref; /* restored from parent trace snapshot */
	ir_ref               trace_loop_ref;
	ir_ref               return_inputs;
	const zend_op_array *op_array;
	const zend_op_array *current_op_array;
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

static const void *zend_jit_trace_allocate_exit_group(uint32_t n);
static const void *zend_jit_trace_get_exit_addr(uint32_t n);
static int zend_jit_is_constant_cmp_long_long(const zend_op *opline, zend_ssa_range *op1_range, zend_jit_addr op1_addr, zend_ssa_range *op2_range, zend_jit_addr op2_addr, bool *result);
static bool zend_jit_is_persistent_constant(zval *key, uint32_t flags);
static int zend_jit_needs_call_chain(zend_call_info *call_info, uint32_t b, const zend_op_array *op_array, zend_ssa *ssa, const zend_ssa_op *ssa_op, const zend_op *opline, int call_level, zend_jit_trace_rec *trace);
static bool zend_jit_may_be_modified(const zend_function *func, const zend_op_array *called_from);
static zend_class_entry* zend_get_known_class(const zend_op_array *op_array, const zend_op *opline, uint8_t op_type, znode_op op);
static uint32_t skip_valid_arguments(const zend_op_array *op_array, zend_ssa *ssa, const zend_call_info *call_info);
static bool zend_jit_needs_arg_dtor(const zend_function *func, uint32_t arg_num, zend_call_info *call_info);
static bool zend_jit_may_avoid_refcounting(const zend_op *opline, uint32_t op1_info);
static zend_property_info* zend_get_known_property_info(const zend_op_array *op_array, zend_class_entry *ce, zend_string *member, bool on_this, zend_string *filename);
static bool zend_may_be_dynamic_property(zend_class_entry *ce, zend_string *member, bool on_this, const zend_op_array *op_array);
static zend_string *zend_jit_func_name(const zend_op_array *op_array);
static void zend_jit_trace_add_code(const void *start, uint32_t size);
static int zend_jit_trace_may_exit(const zend_op_array *op_array, const zend_op *opline);
static bool zend_jit_supported_binary_op(uint8_t op, uint32_t op1_info, uint32_t op2_info);
static int zend_jit_op_array_analyze1(const zend_op_array *op_array, zend_script *script, zend_ssa *ssa);
static int zend_jit_op_array_analyze2(const zend_op_array *op_array, zend_script *script, zend_ssa *ssa, uint32_t optimization_level);
static int zend_jit_build_cfg(const zend_op_array *op_array, zend_cfg *cfg);
static bool zend_jit_inc_call_level(uint8_t opcode);
static bool zend_jit_dec_call_level(uint8_t opcode);
static bool zend_ssa_is_last_use(const zend_op_array *op_array, const zend_ssa *ssa, int var, int use);
static bool zend_jit_var_supports_reg(zend_ssa *ssa, int var);
static bool zend_jit_opline_supports_reg(const zend_op_array *op_array, zend_ssa *ssa, const zend_op *opline, const zend_ssa_op *ssa_op, zend_jit_trace_rec *trace);
static int zend_jit_store_const_long(zend_jit_ctx *jit, int var, zend_long val);
static int zend_jit_store_const_double(zend_jit_ctx *jit, int var, double val);
static int zend_jit_store_type(zend_jit_ctx *jit, int var, uint8_t type);
static int zend_jit_load_this(zend_jit_ctx *jit, uint32_t var);
static int zend_jit_zval_try_addref(zend_jit_ctx *jit, zend_jit_addr var_addr);
static int zend_jit_store_spill_slot(zend_jit_ctx *jit, uint32_t info, int var, int8_t reg, int32_t offset, bool set_type);
static int zend_jit_store_reg(zend_jit_ctx *jit, uint32_t info, int var, int8_t reg, bool in_mem, bool set_type);
static int zend_jit_escape_if_undef(zend_jit_ctx *jit, int var, uint32_t flags, const zend_op *opline, int8_t reg);
static int zend_jit_restore_zval(zend_jit_ctx *jit, int var, int8_t reg);
static int zend_jit_save_call_chain(zend_jit_ctx *jit, uint32_t call_level);
static int zend_jit_free_op(zend_jit_ctx *jit, const zend_op *opline, uint32_t info, uint32_t var_offset);
static void zend_jit_check_exception(zend_jit_ctx *jit);
static ir_ref zend_jit_deopt_rload_spilled(zend_jit_ctx *jit, ir_type type, int8_t reg, int32_t offset);
static int zend_jit_free_trampoline(zend_jit_ctx *jit, ir_ref func);
static int zend_jit_trace_start(zend_jit_ctx *jit, const zend_op_array *op_array, zend_ssa *ssa, zend_string *name, uint32_t trace_num, zend_jit_trace_info *parent, uint32_t exit_num);
static void zend_jit_set_last_valid_opline(zend_jit_ctx *jit, const zend_op *opline);
static void zend_jit_track_last_valid_opline(zend_jit_ctx *jit);
static int zend_jit_trace_opline_guard(zend_jit_ctx *jit, const zend_op *opline);
static void zend_jit_reset_last_valid_opline(zend_jit_ctx *jit);
static int zend_jit_stack_check(zend_jit_ctx *jit, const zend_op *opline, uint32_t used_stack);
static int zend_jit_type_guard(zend_jit_ctx *jit, const zend_op *opline, uint32_t var, uint8_t type);
static int zend_jit_packed_guard(zend_jit_ctx *jit, const zend_op *opline, uint32_t var, uint32_t op_info);
static int zend_jit_load_var(zend_jit_ctx *jit, uint32_t info, int var, int ssa_var);
static int zend_jit_trace_begin_loop(zend_jit_ctx *jit);
static void zend_jit_trace_gen_phi(zend_jit_ctx *jit, zend_ssa_phi *phi);
static int zend_jit_store_var(zend_jit_ctx *jit, uint32_t info, int var, int ssa_var, bool set_type);
static int zend_jit_inc_dec(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, uint32_t op1_def_info, zend_jit_addr op1_def_addr, uint32_t res_use_info, uint32_t res_info, zend_jit_addr res_addr, int may_overflow, int may_throw);
static int zend_may_overflow(const zend_op *opline, const zend_ssa_op *ssa_op, const zend_op_array *op_array, zend_ssa *ssa);
static int zend_jit_reuse_ip(zend_jit_ctx *jit);
static int zend_jit_long_math(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_ssa_range *op1_range, zend_jit_addr op1_addr, uint32_t op2_info, zend_ssa_range *op2_range, zend_jit_addr op2_addr, uint32_t res_use_info, uint32_t res_info, zend_jit_addr res_addr, int may_throw);
static bool zend_jit_fetch_reference(zend_jit_ctx *jit, const zend_op *opline, uint8_t var_type, uint32_t *var_info_ptr, zend_jit_addr *var_addr_ptr, bool add_ref_guard, bool add_type_guard);
static int zend_jit_add_arrays(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, uint32_t op2_info, zend_jit_addr op2_addr, zend_jit_addr res_addr);
static int zend_jit_math(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, uint32_t op2_info, zend_jit_addr op2_addr, uint32_t res_use_info, uint32_t res_info, zend_jit_addr res_addr, int may_overflow, int may_throw);
static int zend_jit_concat(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, uint32_t op2_info, zend_jit_addr res_addr, int may_throw);
static int zend_jit_assign_op(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, zend_ssa_range *op1_range, uint32_t op1_def_info, zend_jit_addr op1_def_addr, uint32_t op1_mem_info, uint32_t op2_info, zend_jit_addr op2_addr, zend_ssa_range *op2_range, int may_overflow, int may_throw);
static bool zend_jit_fetch_indirect_var(zend_jit_ctx *jit, const zend_op *opline, uint8_t var_type, uint32_t *var_info_ptr, zend_jit_addr *var_addr_ptr, bool add_indirect_guard);
static int zend_jit_assign_dim_op(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, uint32_t op1_def_info, zend_jit_addr op1_addr, bool op1_indirect, uint32_t op2_info, zend_jit_addr op2_addr, zend_ssa_range *op2_range, uint32_t op1_data_info, zend_jit_addr op3_addr, zend_ssa_range *op1_data_range, uint8_t dim_type, int may_throw);
static int zend_jit_incdec_obj(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, zend_ssa *ssa, const zend_ssa_op *ssa_op, uint32_t op1_info, zend_jit_addr op1_addr, bool op1_indirect, zend_class_entry *ce, bool ce_is_instanceof, bool on_this, bool delayed_fetch_this, zend_class_entry *trace_ce, uint8_t prop_type);
static int zend_jit_assign_obj_op(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, zend_ssa *ssa, const zend_ssa_op *ssa_op, uint32_t op1_info, zend_jit_addr op1_addr, uint32_t val_info, zend_jit_addr val_addr, zend_ssa_range *val_range, bool op1_indirect, zend_class_entry *ce, bool ce_is_instanceof, bool on_this, bool delayed_fetch_this, zend_class_entry *trace_ce, uint8_t prop_type);
static int zend_jit_assign_obj(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, zend_ssa *ssa, const zend_ssa_op *ssa_op, uint32_t op1_info, zend_jit_addr op1_addr, uint32_t val_info, zend_jit_addr val_addr, zend_jit_addr val_def_addr, zend_jit_addr res_addr, bool op1_indirect, zend_class_entry *ce, bool ce_is_instanceof, bool on_this, bool delayed_fetch_this, zend_class_entry *trace_ce, uint8_t prop_type, int may_throw);
static int zend_jit_assign_dim(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, bool op1_indirect, uint32_t op2_info, zend_jit_addr op2_addr, zend_ssa_range *op2_range, uint32_t val_info, zend_jit_addr op3_addr, zend_jit_addr op3_def_addr, zend_jit_addr res_addr, uint8_t dim_type, int may_throw);
static int zend_jit_scalar_type_guard(zend_jit_ctx *jit, const zend_op *opline, uint32_t var);
static bool zend_jit_guard_reference(zend_jit_ctx *jit, const zend_op *opline, zend_jit_addr *var_addr_ptr, zend_jit_addr *ref_addr_ptr, bool add_ref_guard);
static bool zend_jit_noref_guard(zend_jit_ctx *jit, const zend_op *opline, zend_jit_addr var_addr);
static int zend_jit_assign(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_use_addr, uint32_t op1_def_info, zend_jit_addr op1_addr, uint32_t op2_info, zend_jit_addr op2_addr, zend_jit_addr op2_def_addr, uint32_t res_info, zend_jit_addr res_addr, zend_jit_addr ref_addr, int may_throw);
static int zend_jit_qm_assign(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, zend_jit_addr op1_def_addr, uint32_t res_use_info, uint32_t res_info, zend_jit_addr res_addr);
static int zend_jit_init_fcall(zend_jit_ctx *jit, const zend_op *opline, uint32_t b, const zend_op_array *op_array, zend_ssa *ssa, const zend_ssa_op *ssa_op, int call_level, zend_jit_trace_rec *trace, int checked_stack);
static int zend_jit_send_val(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr);
static int zend_jit_send_ref(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, uint32_t op1_info, int cold);
static int zend_jit_send_var(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, uint32_t op1_info, zend_jit_addr op1_addr, zend_jit_addr op1_def_addr);
static int zend_jit_check_func_arg(zend_jit_ctx *jit, const zend_op *opline);
static int zend_jit_check_undef_args(zend_jit_ctx *jit, const zend_op *opline);
static int zend_jit_do_fcall(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, zend_ssa *ssa, int call_level, unsigned int next_block, zend_jit_trace_rec *trace);
static int zend_jit_cmp(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_ssa_range *op1_range, zend_jit_addr op1_addr, uint32_t op2_info, zend_ssa_range *op2_range, zend_jit_addr op2_addr, zend_jit_addr res_addr, int may_throw, uint8_t smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr, bool skip_comparison);
static int zend_jit_identical(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_ssa_range *op1_range, zend_jit_addr op1_addr, uint32_t op2_info, zend_ssa_range *op2_range, zend_jit_addr op2_addr, zend_jit_addr res_addr, int may_throw, uint8_t smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr, bool skip_comparison);
static int zend_jit_defined(zend_jit_ctx *jit, const zend_op *opline, uint8_t smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr);
static int zend_jit_type_check(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, uint8_t smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr);
static int zend_jit_trace_handler(zend_jit_ctx *jit, const zend_op_array *op_array, const zend_op *opline, int may_throw, zend_jit_trace_rec *trace);
static int zend_jit_return(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, uint32_t op1_info, zend_jit_addr op1_addr);
static int zend_jit_free_cvs(zend_jit_ctx *jit);
static uint32_t zend_ssa_cv_info(const zend_op_array *op_array, zend_ssa *ssa, uint32_t var);
static int zend_jit_leave_frame(zend_jit_ctx *jit);
static int zend_jit_free_cv(zend_jit_ctx *jit, uint32_t info, uint32_t var);
static int zend_jit_leave_func(zend_jit_ctx *jit, const zend_op_array *op_array, const zend_op *opline, uint32_t op1_info, bool left_frame, zend_jit_trace_rec *trace, zend_jit_trace_info *trace_info, int indirect_var_access, int may_throw);
static int zend_jit_bool_jmpznz(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, zend_jit_addr res_addr, uint32_t target_label, uint32_t target_label2, int may_throw, uint8_t branch_opcode, const void *exit_addr);
static int zend_jit_jmp_frameless(zend_jit_ctx *jit, const zend_op *opline, const void *exit_addr, zend_jmp_fl_result guard);
static int zend_jit_isset_isempty_cv(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, uint8_t smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr);
static int zend_jit_in_array(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, uint8_t smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr);
static int zend_jit_fetch_dim_read(zend_jit_ctx *jit, const zend_op *opline, zend_ssa *ssa, const zend_ssa_op *ssa_op, uint32_t op1_info, zend_jit_addr op1_addr, bool op1_avoid_refcounting, uint32_t op2_info, zend_jit_addr op2_addr, zend_ssa_range *op2_range, uint32_t res_info, zend_jit_addr res_addr, uint8_t dim_type);
static int zend_jit_fetch_dim(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, uint32_t op2_info, zend_jit_addr op2_addr, zend_ssa_range *op2_range, zend_jit_addr res_addr, uint8_t dim_type);
static int zend_jit_isset_isempty_dim(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, bool op1_avoid_refcounting, uint32_t op2_info, zend_jit_addr op2_addr, zend_ssa_range *op2_range, uint8_t dim_type, int may_throw, uint8_t smart_branch_opcode, uint32_t target_label, uint32_t target_label2, const void *exit_addr);
static int zend_jit_fetch_obj(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, zend_ssa *ssa, const zend_ssa_op *ssa_op, uint32_t op1_info, zend_jit_addr op1_addr, bool op1_indirect, zend_class_entry *ce, bool ce_is_instanceof, bool on_this, bool delayed_fetch_this, bool op1_avoid_refcounting, zend_class_entry *trace_ce, zend_jit_addr res_addr, uint8_t prop_type, int may_throw);
static int zend_jit_fetch_static_prop(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array);
static int zend_jit_bind_global(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info);
static int zend_jit_recv(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array);
static int zend_jit_recv_init(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, bool is_last, int may_throw);
static int zend_jit_free(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, int may_throw);
static int zend_jit_echo(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info);
static int zend_jit_strlen(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, zend_jit_addr res_addr);
static int zend_jit_count(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, zend_jit_addr op1_addr, zend_jit_addr res_addr, int may_throw);
static int zend_jit_fetch_this(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, bool check_only);
static int zend_jit_switch(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, zend_ssa *ssa, zend_jit_trace_rec *trace, zend_jit_trace_info *trace_info);
static bool zend_jit_verify_return_type(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, uint32_t op1_info);
static int zend_jit_fe_reset(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info);
static int zend_jit_fe_fetch(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, uint32_t op2_info, unsigned int target_label, uint8_t exit_opcode, const void *exit_addr);
static int zend_jit_fetch_constant(zend_jit_ctx *jit, const zend_op *opline, const zend_op_array *op_array, zend_ssa *ssa, const zend_ssa_op *ssa_op, zend_jit_addr res_addr);
static int zend_jit_init_method_call(zend_jit_ctx *jit, const zend_op *opline, uint32_t b, const zend_op_array *op_array, zend_ssa *ssa, const zend_ssa_op *ssa_op, int call_level, uint32_t op1_info, zend_jit_addr op1_addr, zend_class_entry *ce, bool ce_is_instanceof, bool on_this, bool delayed_fetch_this, zend_class_entry *trace_ce, zend_jit_trace_rec *trace, int checked_stack, ir_ref func_ref, ir_ref this_ref, bool polymorphic_side_trace);
static int zend_jit_init_static_method_call(zend_jit_ctx *jit, const zend_op *opline, uint32_t b, const zend_op_array *op_array, zend_ssa *ssa, const zend_ssa_op *ssa_op, int call_level, zend_jit_trace_rec *trace, int checked_stack);
static int zend_jit_init_closure_call(zend_jit_ctx *jit, const zend_op *opline, uint32_t b, const zend_op_array *op_array, zend_ssa *ssa, const zend_ssa_op *ssa_op, int call_level, zend_jit_trace_rec *trace, int checked_stack);
static int zend_jit_rope(zend_jit_ctx *jit, const zend_op *opline, uint32_t op2_info);
static void jit_frameless_icall0(zend_jit_ctx *jit, const zend_op *opline);
static void jit_frameless_icall1(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info);
static void jit_frameless_icall2(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, uint32_t op2_info);
static void jit_frameless_icall3(zend_jit_ctx *jit, const zend_op *opline, uint32_t op1_info, uint32_t op2_info, uint32_t op1_data_info);
static int zend_jit_init_fcall_guard(zend_jit_ctx *jit, uint32_t level, const zend_function *func, const zend_op *to_opline);
static bool zend_jit_trace_uses_initial_ip(zend_jit_ctx *jit);
static int zend_jit_store_var_type(zend_jit_ctx *jit, int var, uint32_t type);
static int zend_jit_set_ip(zend_jit_ctx *jit, const zend_op *target);
static int zend_jit_trace_end_loop(zend_jit_ctx *jit, int loop_ref, const void *timeout_exit_addr);
static int zend_jit_store_ref(zend_jit_ctx *jit, uint32_t info, int var, int32_t src, bool set_type);
static int zend_jit_trace_link_to_root(zend_jit_ctx *jit, zend_jit_trace_info *t, const void *timeout_exit_addr);
static int zend_jit_trace_return(zend_jit_ctx *jit, bool original_handler, const zend_op *opline);
static zend_vm_opcode_handler_t zend_jit_finish(zend_jit_ctx *jit);
static int zend_jit_free_ctx(zend_jit_ctx *jit);
static int zend_jit_deoptimizer_start(zend_jit_ctx *jit, zend_string *name, uint32_t trace_num, uint32_t exit_num);
static const char* zend_reg_name(int8_t reg);
static int zend_jit_link_side_trace(const void *code, size_t size, uint32_t jmp_table_size, uint32_t exit_num, const void *addr);

#if ZEND_VM_KIND == ZEND_VM_KIND_CALL || ZEND_VM_KIND == ZEND_VM_KIND_TAILCALL
static ZEND_OPCODE_HANDLER_RET ZEND_OPCODE_HANDLER_CCONV zend_runtime_jit(ZEND_OPCODE_HANDLER_ARGS);
#else
static ZEND_OPCODE_HANDLER_RET ZEND_OPCODE_HANDLER_FUNC_CCONV zend_runtime_jit(ZEND_OPCODE_HANDLER_ARGS);
#endif

static uint32_t _zend_jit_trace_get_exit_point(const zend_op *to_opline, uint32_t flags ZEND_FILE_LINE_DC);
#define zend_jit_trace_get_exit_point(to_opline, flags) _zend_jit_trace_get_exit_point(to_opline, flags ZEND_FILE_LINE_CC)

static void *dasm_buf = NULL;
static void *dasm_end = NULL;
static void **dasm_ptr = NULL;

static bool delayed_call_chain = false; // TODO: remove this var (use jit->delayed_call_level) ???

static zend_vm_opcode_handler_t zend_jit_func_trace_counter_handler = NULL;
static zend_vm_opcode_handler_t zend_jit_ret_trace_counter_handler = NULL;
static zend_vm_opcode_handler_t zend_jit_loop_trace_counter_handler = NULL;

#if defined(_WIN32) || defined(IR_TARGET_AARCH64)
/* We keep addresses in SHM to share them between sepaeate processes (on Windows) or to support veneers (on AArch64) */
static void** zend_jit_stub_handlers = NULL;
#else
static void* zend_jit_stub_handlers[jit_last_stub];
#endif

#endif /* ZEND_JIT_SHARED_H */
