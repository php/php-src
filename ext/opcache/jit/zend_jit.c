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
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "main/php.h"
#include "main/SAPI.h"
#include "php_version.h"
#include <ZendAccelerator.h>
#include "zend_shared_alloc.h"
#include "Zend/zend_execute.h"
#include "Zend/zend_vm.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_closures.h"
#include "Zend/zend_ini.h"
#include "Zend/zend_observer.h"
#include "zend_smart_str.h"
#include "jit/zend_jit.h"

#ifdef HAVE_JIT

#include "Optimizer/zend_func_info.h"
#include "Optimizer/zend_ssa.h"
#include "Optimizer/zend_inference.h"
#include "Optimizer/zend_call_graph.h"
#include "Optimizer/zend_dump.h"

#ifndef ZEND_JIT_IR
#if ZEND_JIT_TARGET_X86
# include "jit/zend_jit_x86.h"
#elif ZEND_JIT_TARGET_ARM64
# include "jit/zend_jit_arm64.h"
#endif
#else
#include "Optimizer/zend_worklist.h"
#endif

#include "jit/zend_jit_internal.h"

#ifdef HAVE_PTHREAD_JIT_WRITE_PROTECT_NP
#include <pthread.h>
#endif

#ifdef ZTS
int jit_globals_id;
#else
zend_jit_globals jit_globals;
#endif

//#define CONTEXT_THREADED_JIT
#define ZEND_JIT_USE_RC_INFERENCE

#ifdef ZEND_JIT_USE_RC_INFERENCE
# define ZEND_SSA_RC_INFERENCE_FLAG ZEND_SSA_RC_INFERENCE
# define RC_MAY_BE_1(info)          (((info) & (MAY_BE_RC1|MAY_BE_REF)) != 0)
# define RC_MAY_BE_N(info)          (((info) & (MAY_BE_RCN|MAY_BE_REF)) != 0)
#else
# define ZEND_SSA_RC_INFERENCE_FLAG 0
# define RC_MAY_BE_1(info)          1
# define RC_MAY_BE_N(info)          1
#endif

#define JIT_PREFIX      "JIT$"
#define JIT_STUB_PREFIX "JIT$$"
#define TRACE_PREFIX    "TRACE-"

#ifndef ZEND_JIT_IR
#define DASM_M_GROW(ctx, t, p, sz, need) \
  do { \
    size_t _sz = (sz), _need = (need); \
    if (_sz < _need) { \
      if (_sz < 16) _sz = 16; \
      while (_sz < _need) _sz += _sz; \
      (p) = (t *)erealloc((p), _sz); \
      (sz) = _sz; \
    } \
  } while(0)

#define DASM_M_FREE(ctx, p, sz) efree(p)

#if ZEND_DEBUG
# define DASM_CHECKS 1
#endif

#include "dynasm/dasm_proto.h"

typedef struct _zend_jit_stub {
	const char *name;
	int (*stub)(dasm_State **Dst);
	uint32_t offset;
	uint32_t adjustment;
} zend_jit_stub;

#define JIT_STUB(name, offset, adjustment) \
	{JIT_STUB_PREFIX #name, zend_jit_ ## name ## _stub, offset, adjustment}
#endif /* ZEND_JIT_IR */

zend_ulong zend_jit_profile_counter = 0;
int zend_jit_profile_counter_rid = -1;

int16_t zend_jit_hot_counters[ZEND_HOT_COUNTERS_COUNT];

const zend_op *zend_jit_halt_op = NULL;
static int zend_jit_vm_kind = 0;
#ifdef HAVE_PTHREAD_JIT_WRITE_PROTECT_NP
static int zend_write_protect = 1;
#endif

static void *dasm_buf = NULL;
static void *dasm_end = NULL;
static void **dasm_ptr = NULL;

static size_t dasm_size = 0;

static zend_long jit_bisect_pos = 0;

static const void *zend_jit_runtime_jit_handler = NULL;
static const void *zend_jit_profile_jit_handler = NULL;
static const void *zend_jit_func_hot_counter_handler = NULL;
static const void *zend_jit_loop_hot_counter_handler = NULL;
static const void *zend_jit_func_trace_counter_handler = NULL;
static const void *zend_jit_ret_trace_counter_handler = NULL;
static const void *zend_jit_loop_trace_counter_handler = NULL;

static int ZEND_FASTCALL zend_runtime_jit(void);

static int zend_jit_trace_may_exit(const zend_op_array *op_array, const zend_op *opline);
static uint32_t zend_jit_trace_get_exit_point(const zend_op *to_opline, uint32_t flags);
static const void *zend_jit_trace_get_exit_addr(uint32_t n);
static void zend_jit_trace_add_code(const void *start, uint32_t size);
static zend_string *zend_jit_func_name(const zend_op_array *op_array);

static int zend_jit_trace_op_len(const zend_op *opline);
#ifndef ZEND_JIT_IR
static bool zend_jit_needs_arg_dtor(const zend_function *func, uint32_t arg_num, zend_call_info *call_info);

#if ZEND_JIT_TARGET_ARM64
static zend_jit_trace_info *zend_jit_get_current_trace_info(void);
static uint32_t zend_jit_trace_find_exit_point(const void* addr);
#endif

#if ZEND_JIT_TARGET_X86 && defined(__linux__)
# if PHP_HAVE_BUILTIN_CPU_SUPPORTS && defined(__GNUC__) && (ZEND_GCC_VERSION >= 11000)
# define ZEND_JIT_SUPPORT_CLDEMOTE 1
# else
# define ZEND_JIT_SUPPORT_CLDEMOTE 0
# endif
#endif

#if ZEND_JIT_SUPPORT_CLDEMOTE
#include <immintrin.h>
#pragma GCC push_options
#pragma GCC target("cldemote")
// check cldemote by CPUID when JIT startup
static int cpu_support_cldemote = 0;
static inline void shared_cacheline_demote(uintptr_t start, size_t size) {
    uintptr_t cache_line_base = start & ~0x3F;
    do {
        _cldemote((void *)cache_line_base);
        // next cacheline start size
        cache_line_base += 64;
    } while (cache_line_base < start + size);
}
#pragma GCC pop_options
#endif

static int zend_jit_assign_to_variable(dasm_State    **Dst,
                                       const zend_op  *opline,
                                       zend_jit_addr   var_use_addr,
                                       zend_jit_addr   var_addr,
                                       uint32_t        var_info,
                                       uint32_t        var_def_info,
                                       uint8_t         val_type,
                                       zend_jit_addr   val_addr,
                                       uint32_t        val_info,
                                       zend_jit_addr   res_addr,
                                       bool       check_exception);
#endif /* ZEND_JIT_IR */

static bool dominates(const zend_basic_block *blocks, int a, int b) {
	while (blocks[b].level > blocks[a].level) {
		b = blocks[b].idom;
	}
	return a == b;
}

static bool zend_ssa_is_last_use(const zend_op_array *op_array, const zend_ssa *ssa, int var, int use)
{
	int next_use;

	if (ssa->vars[var].phi_use_chain) {
		zend_ssa_phi *phi = ssa->vars[var].phi_use_chain;
		do {
			if (!ssa->vars[phi->ssa_var].no_val) {
				return 0;
			}
			phi = zend_ssa_next_use_phi(ssa, var, phi);
		} while (phi);
	}

	if (ssa->cfg.blocks[ssa->cfg.map[use]].loop_header > 0
	 || (ssa->cfg.blocks[ssa->cfg.map[use]].flags & ZEND_BB_LOOP_HEADER)) {
		int b = ssa->cfg.map[use];
		int prev_use = ssa->vars[var].use_chain;

		while (prev_use >= 0 && prev_use != use) {
			if (b != ssa->cfg.map[prev_use]
			 && dominates(ssa->cfg.blocks, b, ssa->cfg.map[prev_use])
			 && !zend_ssa_is_no_val_use(op_array->opcodes + prev_use, ssa->ops + prev_use, var)) {
				return 0;
			}
			prev_use = zend_ssa_next_use(ssa->ops, var, prev_use);
		}
	}

	next_use = zend_ssa_next_use(ssa->ops, var, use);
	if (next_use < 0) {
		return 1;
	} else if (zend_ssa_is_no_val_use(op_array->opcodes + next_use, ssa->ops + next_use, var)) {
		return 1;
	}
	return 0;
}

#ifndef ZEND_JIT_IR
static bool zend_ival_is_last_use(const zend_lifetime_interval *ival, int use)
{
	if (ival->flags & ZREG_LAST_USE) {
		const zend_life_range *range = &ival->range;

		while (range->next) {
			range = range->next;
		}
		return range->end == use;
	}
	return 0;
}

static bool zend_is_commutative(uint8_t opcode)
{
	return
		opcode == ZEND_ADD ||
		opcode == ZEND_MUL ||
		opcode == ZEND_BW_OR ||
		opcode == ZEND_BW_AND ||
		opcode == ZEND_BW_XOR;
}

static int zend_jit_is_constant_cmp_long_long(const zend_op  *opline,
                                              zend_ssa_range *op1_range,
                                              zend_jit_addr   op1_addr,
                                              zend_ssa_range *op2_range,
                                              zend_jit_addr   op2_addr,
                                              bool           *result)
{
	zend_long op1_min;
	zend_long op1_max;
	zend_long op2_min;
	zend_long op2_max;

	if (op1_range) {
		op1_min = op1_range->min;
		op1_max = op1_range->max;
	} else if (Z_MODE(op1_addr) == IS_CONST_ZVAL) {
		ZEND_ASSERT(Z_TYPE_P(Z_ZV(op1_addr)) == IS_LONG);
		op1_min = op1_max = Z_LVAL_P(Z_ZV(op1_addr));
	} else {
		return 0;
	}

	if (op2_range) {
		op2_min = op2_range->min;
		op2_max = op2_range->max;
	} else if (Z_MODE(op2_addr) == IS_CONST_ZVAL) {
		ZEND_ASSERT(Z_TYPE_P(Z_ZV(op2_addr)) == IS_LONG);
		op2_min = op2_max = Z_LVAL_P(Z_ZV(op2_addr));
	} else {
		return 0;
	}

	switch (opline->opcode) {
		case ZEND_IS_EQUAL:
		case ZEND_IS_IDENTICAL:
		case ZEND_CASE:
		case ZEND_CASE_STRICT:
			if (op1_min == op1_max && op2_min == op2_max && op1_min == op2_min) {
				*result = 1;
				return 1;
			} else if (op1_max < op2_min || op1_min > op2_max) {
				*result = 0;
				return 1;
			}
			return 0;
		case ZEND_IS_NOT_EQUAL:
		case ZEND_IS_NOT_IDENTICAL:
			if (op1_min == op1_max && op2_min == op2_max && op1_min == op2_min) {
				*result = 0;
				return 1;
			} else if (op1_max < op2_min || op1_min > op2_max) {
				*result = 1;
				return 1;
			}
			return 0;
		case ZEND_IS_SMALLER:
			if (op1_max < op2_min) {
				*result = 1;
				return 1;
			} else if (op1_min >= op2_max) {
				*result = 0;
				return 1;
			}
			return 0;
		case ZEND_IS_SMALLER_OR_EQUAL:
			if (op1_max <= op2_min) {
				*result = 1;
				return 1;
			} else if (op1_min > op2_max) {
				*result = 0;
				return 1;
			}
			return 0;
		default:
			ZEND_UNREACHABLE();
	}
	return 0;
}
#endif

static int zend_jit_needs_call_chain(zend_call_info *call_info, uint32_t b, const zend_op_array *op_array, zend_ssa *ssa, const zend_ssa_op *ssa_op, const zend_op *opline, int call_level, zend_jit_trace_rec *trace)
{
	int skip;

	if (trace) {
		zend_jit_trace_rec *p = trace;

		ssa_op++;
		while (1) {
			if (p->op == ZEND_JIT_TRACE_VM) {
				switch (p->opline->opcode) {
					case ZEND_SEND_ARRAY:
					case ZEND_SEND_USER:
					case ZEND_SEND_UNPACK:
					case ZEND_INIT_FCALL:
					case ZEND_INIT_METHOD_CALL:
					case ZEND_INIT_STATIC_METHOD_CALL:
					case ZEND_INIT_FCALL_BY_NAME:
					case ZEND_INIT_NS_FCALL_BY_NAME:
					case ZEND_INIT_DYNAMIC_CALL:
					case ZEND_NEW:
					case ZEND_INIT_USER_CALL:
					case ZEND_FAST_CALL:
					case ZEND_JMP:
					case ZEND_JMPZ:
					case ZEND_JMPNZ:
					case ZEND_JMPZ_EX:
					case ZEND_JMPNZ_EX:
					case ZEND_FE_RESET_R:
					case ZEND_FE_RESET_RW:
					case ZEND_JMP_SET:
					case ZEND_COALESCE:
					case ZEND_JMP_NULL:
					case ZEND_ASSERT_CHECK:
					case ZEND_CATCH:
					case ZEND_DECLARE_ANON_CLASS:
					case ZEND_FE_FETCH_R:
					case ZEND_FE_FETCH_RW:
					case ZEND_BIND_INIT_STATIC_OR_JMP:
						return 1;
					case ZEND_DO_ICALL:
					case ZEND_DO_UCALL:
					case ZEND_DO_FCALL_BY_NAME:
					case ZEND_DO_FCALL:
					case ZEND_CALLABLE_CONVERT:
						return 0;
					case ZEND_SEND_VAL:
					case ZEND_SEND_VAR:
					case ZEND_SEND_VAL_EX:
					case ZEND_SEND_VAR_EX:
					case ZEND_SEND_FUNC_ARG:
					case ZEND_SEND_REF:
					case ZEND_SEND_VAR_NO_REF:
					case ZEND_SEND_VAR_NO_REF_EX:
						/* skip */
						break;
					default:
						if (zend_may_throw(opline, ssa_op, op_array, ssa)) {
							return 1;
						}
				}
				ssa_op += zend_jit_trace_op_len(opline);
			} else if (p->op == ZEND_JIT_TRACE_ENTER ||
			           p->op == ZEND_JIT_TRACE_BACK ||
			           p->op == ZEND_JIT_TRACE_END) {
				return 1;
			}
			p++;
		}
	}

	if (!call_info) {
		const zend_op *end = op_array->opcodes + op_array->last;

		opline++;
		ssa_op++;
		skip = (call_level == 1);
		while (opline != end) {
			if (!skip) {
				if (zend_may_throw(opline, ssa_op, op_array, ssa)) {
					return 1;
				}
			}
			switch (opline->opcode) {
				case ZEND_SEND_VAL:
				case ZEND_SEND_VAR:
				case ZEND_SEND_VAL_EX:
				case ZEND_SEND_VAR_EX:
				case ZEND_SEND_FUNC_ARG:
				case ZEND_SEND_REF:
				case ZEND_SEND_VAR_NO_REF:
				case ZEND_SEND_VAR_NO_REF_EX:
					skip = 0;
					break;
				case ZEND_SEND_ARRAY:
				case ZEND_SEND_USER:
				case ZEND_SEND_UNPACK:
				case ZEND_INIT_FCALL:
				case ZEND_INIT_METHOD_CALL:
				case ZEND_INIT_STATIC_METHOD_CALL:
				case ZEND_INIT_FCALL_BY_NAME:
				case ZEND_INIT_NS_FCALL_BY_NAME:
				case ZEND_INIT_DYNAMIC_CALL:
				case ZEND_NEW:
				case ZEND_INIT_USER_CALL:
				case ZEND_FAST_CALL:
				case ZEND_JMP:
				case ZEND_JMPZ:
				case ZEND_JMPNZ:
				case ZEND_JMPZ_EX:
				case ZEND_JMPNZ_EX:
				case ZEND_FE_RESET_R:
				case ZEND_FE_RESET_RW:
				case ZEND_JMP_SET:
				case ZEND_COALESCE:
				case ZEND_JMP_NULL:
				case ZEND_ASSERT_CHECK:
				case ZEND_CATCH:
				case ZEND_DECLARE_ANON_CLASS:
				case ZEND_FE_FETCH_R:
				case ZEND_FE_FETCH_RW:
				case ZEND_BIND_INIT_STATIC_OR_JMP:
					return 1;
				case ZEND_DO_ICALL:
				case ZEND_DO_UCALL:
				case ZEND_DO_FCALL_BY_NAME:
				case ZEND_DO_FCALL:
				case ZEND_CALLABLE_CONVERT:
					end = opline;
					if (end - op_array->opcodes >= ssa->cfg.blocks[b].start + ssa->cfg.blocks[b].len) {
						/* INIT_FCALL and DO_FCALL in different BasicBlocks */
						return 1;
					}
					return 0;
			}
			opline++;
			ssa_op++;
		}

		return 1;
	} else {
		const zend_op *end = call_info->caller_call_opline;

		/* end may be null if an opcode like EXIT is part of the argument list. */
		if (!end || end - op_array->opcodes >= ssa->cfg.blocks[b].start + ssa->cfg.blocks[b].len) {
			/* INIT_FCALL and DO_FCALL in different BasicBlocks */
			return 1;
		}

		opline++;
		ssa_op++;
		skip = (call_level == 1);
		while (opline != end) {
			if (skip) {
				switch (opline->opcode) {
					case ZEND_SEND_VAL:
					case ZEND_SEND_VAR:
					case ZEND_SEND_VAL_EX:
					case ZEND_SEND_VAR_EX:
					case ZEND_SEND_FUNC_ARG:
					case ZEND_SEND_REF:
					case ZEND_SEND_VAR_NO_REF:
					case ZEND_SEND_VAR_NO_REF_EX:
						skip = 0;
						break;
					case ZEND_SEND_ARRAY:
					case ZEND_SEND_USER:
					case ZEND_SEND_UNPACK:
						return 1;
				}
			} else {
				if (zend_may_throw(opline, ssa_op, op_array, ssa)) {
					return 1;
				}
			}
			opline++;
			ssa_op++;
		}

		return 0;
	}
}

#ifndef ZEND_JIT_IR
static uint32_t skip_valid_arguments(const zend_op_array *op_array, zend_ssa *ssa, const zend_call_info *call_info)
{
	uint32_t num_args = 0;
	zend_function *func = call_info->callee_func;

	/* It's okay to handle prototypes here, because they can only increase the accepted arguments.
	 * Anything legal for the parent method is also legal for the parent method. */
	while (num_args < call_info->num_args) {
		zend_arg_info *arg_info = func->op_array.arg_info + num_args;

		if (ZEND_TYPE_IS_SET(arg_info->type)) {
			if (ZEND_TYPE_IS_ONLY_MASK(arg_info->type)) {
				zend_op *opline = call_info->arg_info[num_args].opline;
				zend_ssa_op *ssa_op = &ssa->ops[opline - op_array->opcodes];
				uint32_t type_mask = ZEND_TYPE_PURE_MASK(arg_info->type);
				if ((OP1_INFO() & (MAY_BE_ANY|MAY_BE_UNDEF)) & ~type_mask) {
					break;
				}
			} else {
				break;
			}
		}
		num_args++;
	}
	return num_args;
}

static uint32_t zend_ssa_cv_info(const zend_op_array *op_array, zend_ssa *ssa, uint32_t var)
{
	uint32_t j, info;

	if (ssa->vars && ssa->var_info) {
		info = ssa->var_info[var].type;
		for (j = op_array->last_var; j < ssa->vars_count; j++) {
			if (ssa->vars[j].var == var) {
				info |= ssa->var_info[j].type;
			}
		}
	} else {
		info = MAY_BE_RC1 | MAY_BE_RCN | MAY_BE_REF | MAY_BE_ANY | MAY_BE_UNDEF |
			MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF;
	}

#ifdef ZEND_JIT_USE_RC_INFERENCE
	/* Refcount may be increased by RETURN opcode */
	if ((info & MAY_BE_RC1) && !(info & MAY_BE_RCN)) {
		for (j = 0; j < ssa->cfg.blocks_count; j++) {
			if ((ssa->cfg.blocks[j].flags & ZEND_BB_REACHABLE) &&
			    ssa->cfg.blocks[j].len > 0) {
				const zend_op *opline = op_array->opcodes + ssa->cfg.blocks[j].start + ssa->cfg.blocks[j].len - 1;

				if (opline->opcode == ZEND_RETURN) {
					if (opline->op1_type == IS_CV && opline->op1.var == EX_NUM_TO_VAR(var)) {
						info |= MAY_BE_RCN;
						break;
					}
				}
			}
		}
	}
#endif

	return info;
}

static bool zend_jit_may_avoid_refcounting(const zend_op *opline, uint32_t op1_info)
{
	switch (opline->opcode) {
		case ZEND_FETCH_OBJ_FUNC_ARG:
			if (!JIT_G(current_frame) ||
			    !JIT_G(current_frame)->call->func ||
			    !TRACE_FRAME_IS_LAST_SEND_BY_VAL(JIT_G(current_frame)->call)) {
				return 0;
			}
			/* break missing intentionally */
		case ZEND_FETCH_OBJ_R:
		case ZEND_FETCH_OBJ_IS:
			if ((op1_info & MAY_BE_OBJECT)
			 && opline->op2_type == IS_CONST
			 && Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) == IS_STRING
			 && Z_STRVAL_P(RT_CONSTANT(opline, opline->op2))[0] != '\0') {
				return 1;
			}
			break;
		case ZEND_FETCH_DIM_FUNC_ARG:
			if (!JIT_G(current_frame) ||
			    !JIT_G(current_frame)->call->func ||
			    !TRACE_FRAME_IS_LAST_SEND_BY_VAL(JIT_G(current_frame)->call)) {
				return 0;
			}
			/* break missing intentionally */
		case ZEND_FETCH_DIM_R:
		case ZEND_FETCH_DIM_IS:
			return 1;
		case ZEND_ISSET_ISEMPTY_DIM_OBJ:
			if (!(opline->extended_value & ZEND_ISEMPTY)) {
				return 1;
			}
			break;
	}
	return 0;
}

static bool zend_jit_is_persistent_constant(zval *key, uint32_t flags)
{
	zval *zv;
	zend_constant *c = NULL;

	/* null/true/false are resolved during compilation, so don't check for them here. */
	zv = zend_hash_find_known_hash(EG(zend_constants), Z_STR_P(key));
	if (zv) {
		c = (zend_constant*)Z_PTR_P(zv);
	} else if (flags & IS_CONSTANT_UNQUALIFIED_IN_NAMESPACE) {
		key++;
		zv = zend_hash_find_known_hash(EG(zend_constants), Z_STR_P(key));
		if (zv) {
			c = (zend_constant*)Z_PTR_P(zv);
		}
	}
	return c && (ZEND_CONSTANT_FLAGS(c) & CONST_PERSISTENT);
}

static zend_property_info* zend_get_known_property_info(const zend_op_array *op_array, zend_class_entry *ce, zend_string *member, bool on_this, zend_string *filename)
{
	zend_property_info *info = NULL;

	if ((on_this && (op_array->fn_flags & ZEND_ACC_TRAIT_CLONE)) ||
	    !ce ||
	    !(ce->ce_flags & ZEND_ACC_LINKED) ||
	    (ce->ce_flags & ZEND_ACC_TRAIT) ||
	    ce->create_object) {
		return NULL;
	}

	if (!(ce->ce_flags & ZEND_ACC_IMMUTABLE)) {
		if (ce->info.user.filename != filename) {
			/* class declaration might be changed independently */
			return NULL;
		}

		if (ce->parent) {
			zend_class_entry *parent = ce->parent;

			do {
				if (parent->type == ZEND_INTERNAL_CLASS) {
					break;
				} else if (parent->info.user.filename != filename) {
					/* some of parents class declarations might be changed independently */
					/* TODO: this check may be not enough, because even
					 * in the same it's possible to conditionally define
					 * few classes with the same name, and "parent" may
					 * change from request to request.
					 */
					return NULL;
				}
				parent = parent->parent;
			} while (parent);
		}
	}

	info = (zend_property_info*)zend_hash_find_ptr(&ce->properties_info, member);
	if (info == NULL ||
	    !IS_VALID_PROPERTY_OFFSET(info->offset) ||
	    (info->flags & ZEND_ACC_STATIC)) {
		return NULL;
	}

	if (info->flags & ZEND_ACC_PUBLIC) {
		return info;
	} else if (on_this) {
		if (ce == info->ce) {
			if (ce == op_array->scope) {
				return info;
			} else {
				return NULL;
			}
		} else if ((info->flags & ZEND_ACC_PROTECTED)
				&& instanceof_function_slow(ce, info->ce)) {
			return info;
		}
	}

	return NULL;
}

static bool zend_may_be_dynamic_property(zend_class_entry *ce, zend_string *member, bool on_this, zend_string *filename)
{
	zend_property_info *info;

	if (!ce || (ce->ce_flags & ZEND_ACC_TRAIT)) {
		return 1;
	}

	if (!(ce->ce_flags & ZEND_ACC_IMMUTABLE)) {
		if (ce->info.user.filename != filename) {
			/* class declaration might be changed independently */
			return 1;
		}
	}

	info = (zend_property_info*)zend_hash_find_ptr(&ce->properties_info, member);
	if (info == NULL ||
	    !IS_VALID_PROPERTY_OFFSET(info->offset) ||
	    (info->flags & ZEND_ACC_STATIC)) {
		return 1;
	}

	if (!(info->flags & ZEND_ACC_PUBLIC) &&
	    (!on_this || info->ce != ce)) {
		return 1;
	}

	return 0;
}
#endif /* ZEND_JIT_IR */

#define OP_RANGE(ssa_op, opN) \
	(((opline->opN##_type & (IS_TMP_VAR|IS_VAR|IS_CV)) && \
	  ssa->var_info && \
	  (ssa_op)->opN##_use >= 0 && \
	  ssa->var_info[(ssa_op)->opN##_use].has_range) ? \
	 &ssa->var_info[(ssa_op)->opN##_use].range : NULL)

#define OP1_RANGE()      OP_RANGE(ssa_op, op1)
#define OP2_RANGE()      OP_RANGE(ssa_op, op2)
#define OP1_DATA_RANGE() OP_RANGE(ssa_op + 1, op1)

#ifndef ZEND_JIT_IR
#if ZEND_JIT_TARGET_X86
# include "dynasm/dasm_x86.h"
#elif ZEND_JIT_TARGET_ARM64
static int zend_jit_add_veneer(dasm_State *Dst, void *buffer, uint32_t ins, int *b, uint32_t *cp, ptrdiff_t offset);
# define DASM_ADD_VENEER zend_jit_add_veneer
# include "dynasm/dasm_arm64.h"
#endif

#include "jit/zend_jit_helpers.c"
#include "jit/zend_jit_disasm.c"
#ifndef _WIN32
# include "jit/zend_jit_gdb.h"
# include "jit/zend_jit_perf_dump.c"
#endif

#include "Zend/zend_cpuinfo.h"

#ifdef HAVE_VALGRIND
# include <valgrind/valgrind.h>
#endif

#else /* ZEND_JIT_IR */
#include "jit/zend_jit_helpers.c"
#include "Zend/zend_cpuinfo.h"
#endif /* ZEND_JIT_IR */

#ifdef HAVE_GCC_GLOBAL_REGS
# define GCC_GLOBAL_REGS 1
#else
# define GCC_GLOBAL_REGS 0
#endif

/* By default avoid JITing inline handlers if it does not seem profitable due to lack of
 * type information. Disabling this option allows testing some JIT handlers in the
 * presence of try/catch blocks, which prevent SSA construction. */
#ifndef PROFITABILITY_CHECKS
# define PROFITABILITY_CHECKS 1
#endif

#define BP_JIT_IS 6 /* Used for ISSET_ISEMPTY_DIM_OBJ. see BP_VAR_*defines in Zend/zend_compile.h */

typedef enum _sp_adj_kind {
	SP_ADJ_NONE,
	SP_ADJ_RET,
	SP_ADJ_VM,
	SP_ADJ_JIT,
	SP_ADJ_ASSIGN,
	SP_ADJ_LAST
} sp_adj_kind;

static int sp_adj[SP_ADJ_LAST];

/* The generated code may contain tautological comparisons, ignore them. */
#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wtautological-compare"
# pragma clang diagnostic ignored "-Wstring-compare"
#endif

#ifndef ZEND_JIT_IR
#if ZEND_JIT_TARGET_X86
# include "jit/zend_jit_vtune.c"
# include "jit/zend_jit_x86.c"
#elif ZEND_JIT_TARGET_ARM64
# include "jit/zend_jit_arm64.c"
#endif
#else
#include "jit/zend_jit_ir.c"
#endif

#if defined(__clang__)
# pragma clang diagnostic pop
#endif

#if _WIN32
# include <Windows.h>
#else
# include <sys/mman.h>
# if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#   define MAP_ANONYMOUS MAP_ANON
# endif
#endif

ZEND_EXT_API void zend_jit_status(zval *ret)
{
	zval stats;
	array_init(&stats);
	add_assoc_bool(&stats, "enabled", JIT_G(enabled));
	add_assoc_bool(&stats, "on", JIT_G(on));
	add_assoc_long(&stats, "kind", JIT_G(trigger));
	add_assoc_long(&stats, "opt_level", JIT_G(opt_level));
	add_assoc_long(&stats, "opt_flags", JIT_G(opt_flags));
	if (dasm_buf) {
		add_assoc_long(&stats, "buffer_size", (char*)dasm_end - (char*)dasm_buf);
		add_assoc_long(&stats, "buffer_free", (char*)dasm_end - (char*)*dasm_ptr);
	} else {
		add_assoc_long(&stats, "buffer_size", 0);
		add_assoc_long(&stats, "buffer_free", 0);
	}
	add_assoc_zval(ret, "jit", &stats);
}

static zend_string *zend_jit_func_name(const zend_op_array *op_array)
{
	smart_str buf = {0};

	if (op_array->function_name) {
		if (op_array->scope) {
			smart_str_appends(&buf, JIT_PREFIX);
			smart_str_appendl(&buf, ZSTR_VAL(op_array->scope->name), ZSTR_LEN(op_array->scope->name));
			smart_str_appends(&buf, "::");
			smart_str_appendl(&buf, ZSTR_VAL(op_array->function_name), ZSTR_LEN(op_array->function_name));
			smart_str_0(&buf);
			return buf.s;
		} else {
			smart_str_appends(&buf, JIT_PREFIX);
			smart_str_appendl(&buf, ZSTR_VAL(op_array->function_name), ZSTR_LEN(op_array->function_name));
			smart_str_0(&buf);
			return buf.s;
		}
	} else if (op_array->filename) {
		smart_str_appends(&buf, JIT_PREFIX);
		smart_str_appendl(&buf, ZSTR_VAL(op_array->filename), ZSTR_LEN(op_array->filename));
		smart_str_0(&buf);
		return buf.s;
	} else {
		return NULL;
	}
}

#ifndef ZEND_JIT_IR
#if ZEND_DEBUG
static void handle_dasm_error(int ret) {
	switch (ret & 0xff000000u) {
		case DASM_S_NOMEM:
			fprintf(stderr, "DASM_S_NOMEM\n");
			break;
		case DASM_S_PHASE:
			fprintf(stderr, "DASM_S_PHASE\n");
			break;
		case DASM_S_MATCH_SEC:
			fprintf(stderr, "DASM_S_MATCH_SEC\n");
			break;
		case DASM_S_RANGE_I:
			fprintf(stderr, "DASM_S_RANGE_I\n");
			break;
		case DASM_S_RANGE_SEC:
			fprintf(stderr, "DASM_S_RANGE_SEC\n");
			break;
		case DASM_S_RANGE_LG:
			fprintf(stderr, "DASM_S_RANGE_LG\n");
			break;
		case DASM_S_RANGE_PC:
			fprintf(stderr, "DASM_S_RANGE_PC %d\n", ret & 0xffffffu);
			break;
#ifdef DASM_S_RANGE_VREG
		case DASM_S_RANGE_VREG:
			fprintf(stderr, "DASM_S_RANGE_VREG\n");
			break;
#endif
#ifdef DASM_S_UNDEF_L
		case DASM_S_UNDEF_L:
			fprintf(stderr, "DASM_S_UNDEF_L\n");
			break;
#endif
#ifdef DASM_S_UNDEF_LG
		case DASM_S_UNDEF_LG:
			fprintf(stderr, "DASM_S_UNDEF_LG\n");
			break;
#endif
#ifdef DASM_S_RANGE_REL
		case DASM_S_RANGE_REL:
			fprintf(stderr, "DASM_S_RANGE_REL\n");
			break;
#endif
		case DASM_S_UNDEF_PC:
			fprintf(stderr, "DASM_S_UNDEF_PC %d\n", ret & 0xffffffu);
			break;
		default:
			fprintf(stderr, "DASM_S_%0x\n", ret & 0xff000000u);
			break;
	}
	ZEND_UNREACHABLE();
}
#endif

static void *dasm_link_and_encode(dasm_State             **dasm_state,
                                  const zend_op_array     *op_array,
                                  zend_ssa                *ssa,
                                  const zend_op           *rt_opline,
                                  zend_lifetime_interval **ra,
                                  const char              *name,
                                  uint32_t                 trace_num,
                                  uint32_t                 sp_offset,
                                  uint32_t                 sp_adjustment)
{
	size_t size;
	int ret;
	void *entry;
#if defined(HAVE_DISASM) || defined(HAVE_GDB) || defined(HAVE_PERFTOOLS) || defined(HAVE_VTUNE)
	zend_string *str = NULL;
#endif

	if (rt_opline && ssa && ssa->cfg.map) {
		/* Create additional entry point, to switch from interpreter to JIT-ed
		 * code at run-time.
		 */
		int b = ssa->cfg.map[rt_opline - op_array->opcodes];

//#ifdef CONTEXT_THREADED_JIT
//		if (!(ssa->cfg.blocks[b].flags & (ZEND_BB_START|ZEND_BB_RECV_ENTRY))) {
//#else
		if (!(ssa->cfg.blocks[b].flags & (ZEND_BB_START|ZEND_BB_ENTRY|ZEND_BB_RECV_ENTRY))) {
//#endif
			zend_jit_label(dasm_state, ssa->cfg.blocks_count + b);
			zend_jit_prologue(dasm_state);
			if (ra) {
				int i;
				zend_lifetime_interval *ival;
				zend_life_range *range;
				uint32_t pos = rt_opline - op_array->opcodes;

				for (i = 0; i < ssa->vars_count; i++) {
					ival = ra[i];

					if (ival && ival->reg != ZREG_NONE) {
						range = &ival->range;

						if (pos >= range->start && pos <= range->end) {
							if (!zend_jit_load_var(dasm_state, ssa->var_info[i].type, ssa->vars[i].var, ival->reg)) {
								return NULL;
							}
							break;
						}
						range = range->next;
					}
				}
			}
			zend_jit_jmp(dasm_state, b);
		}
	}

	ret = dasm_link(dasm_state, &size);
	if (ret != DASM_S_OK) {
#if ZEND_DEBUG
		handle_dasm_error(ret);
#endif
		return NULL;
	}

	if ((void*)((char*)*dasm_ptr + size) > dasm_end) {
		*dasm_ptr = dasm_end; //prevent further try
		// TODO: jit_buffer_size overflow ???
		return NULL;
	}

#if ZEND_JIT_TARGET_ARM64
	dasm_venners_size = 0;
#endif

	ret = dasm_encode(dasm_state, *dasm_ptr);
	if (ret != DASM_S_OK) {
#if ZEND_DEBUG
		handle_dasm_error(ret);
#endif
		return NULL;
	}

#if ZEND_JIT_TARGET_ARM64
	size += dasm_venners_size;
#endif

	entry = *dasm_ptr;
	*dasm_ptr = (void*)((char*)*dasm_ptr + ZEND_MM_ALIGNED_SIZE_EX(size, DASM_ALIGNMENT));

	/* flush the hardware I-cache */
	JIT_CACHE_FLUSH(entry, entry + size);
	/* hint to the hardware to push out the cache line that contains the linear address */
#if ZEND_JIT_SUPPORT_CLDEMOTE
	if (cpu_support_cldemote && JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
		shared_cacheline_demote((uintptr_t)entry, size);
	}
#endif

	if (trace_num) {
		zend_jit_trace_add_code(entry, dasm_getpclabel(dasm_state, 1));
	}

	if (op_array && ssa) {
		int b;

		for (b = 0; b < ssa->cfg.blocks_count; b++) {
//#ifdef CONTEXT_THREADED_JIT
//			if (ssa->cfg.blocks[b].flags & (ZEND_BB_START|ZEND_BB_RECV_ENTRY)) {
//#else
			if (ssa->cfg.blocks[b].flags & (ZEND_BB_START|ZEND_BB_ENTRY|ZEND_BB_RECV_ENTRY)) {
//#endif
				zend_op *opline = op_array->opcodes + ssa->cfg.blocks[b].start;
				int offset = dasm_getpclabel(dasm_state, ssa->cfg.blocks_count + b);

				if (offset >= 0) {
					opline->handler = (void*)(((char*)entry) + offset);
				}
			}
		}
	    if (rt_opline && ssa && ssa->cfg.map) {
			int b = ssa->cfg.map[rt_opline - op_array->opcodes];
			zend_op *opline = (zend_op*)rt_opline;
			int offset = dasm_getpclabel(dasm_state, ssa->cfg.blocks_count + b);

			if (offset >= 0) {
				opline->handler = (void*)(((char*)entry) + offset);
			}
		}
	}

#if defined(HAVE_DISASM) || defined(HAVE_GDB) || defined(HAVE_PERFTOOLS) || defined(HAVE_VTUNE)
	if (!name) {
		if (JIT_G(debug) & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF|ZEND_JIT_DEBUG_VTUNE|ZEND_JIT_DEBUG_PERF_DUMP)) {
			str = zend_jit_func_name(op_array);
			if (str) {
				name = ZSTR_VAL(str);
			}
		}
#ifdef HAVE_DISASM
	    if (JIT_G(debug) & ZEND_JIT_DEBUG_ASM) {
			zend_jit_disasm_add_symbol(name, (uintptr_t)entry, size);
			zend_jit_disasm(
				name,
				(op_array && op_array->filename) ? ZSTR_VAL(op_array->filename) : NULL,
				op_array,
				&ssa->cfg,
				entry,
				size);
		}
	} else {
	    if (JIT_G(debug) & (ZEND_JIT_DEBUG_ASM_STUBS|ZEND_JIT_DEBUG_ASM)) {
			zend_jit_disasm_add_symbol(name, (uintptr_t)entry, size);
			if ((JIT_G(debug) & (trace_num ? ZEND_JIT_DEBUG_ASM : ZEND_JIT_DEBUG_ASM_STUBS)) != 0) {
				zend_jit_disasm(
					name,
					(op_array && op_array->filename) ? ZSTR_VAL(op_array->filename) : NULL,
					op_array,
					ssa ? &ssa->cfg : NULL,
					entry,
					size);
			}
		}
# endif
	}
#endif

#ifdef HAVE_GDB
	if (JIT_G(debug) & ZEND_JIT_DEBUG_GDB) {
		if (name) {
			zend_jit_gdb_register(
					name,
					op_array,
					entry,
					size,
					sp_adj[sp_offset],
					sp_adj[sp_adjustment]);
		}
	}
#endif


#ifdef HAVE_PERFTOOLS
	if (JIT_G(debug) & (ZEND_JIT_DEBUG_PERF|ZEND_JIT_DEBUG_PERF_DUMP)) {
		if (name) {
			zend_jit_perf_map_register(
				name,
				entry,
				size);
			if (JIT_G(debug) & ZEND_JIT_DEBUG_PERF_DUMP) {
				zend_jit_perf_jitdump_register(
					name,
					entry,
					size);
			}
		}
	}
#endif

#ifdef HAVE_VTUNE
	if (JIT_G(debug) & ZEND_JIT_DEBUG_VTUNE) {
		if (name) {
			zend_jit_vtune_register(
				name,
				entry,
				size);
		}
	}
#endif

#if defined(HAVE_DISASM) || defined(HAVE_GDB) || defined(HAVE_PERFTOOLS) || defined(HAVE_VTUNE)
	if (str) {
		zend_string_release(str);
	}
#endif

	return entry;
}
#endif /* ZEND_JIT_IR */

static int zend_may_overflow(const zend_op *opline, const zend_ssa_op *ssa_op, const zend_op_array *op_array, zend_ssa *ssa)
{
	int res;
	zend_long op1_min, op1_max, op2_min, op2_max;

	if (!ssa->ops || !ssa->var_info) {
		return 1;
	}
	switch (opline->opcode) {
		case ZEND_PRE_INC:
		case ZEND_POST_INC:
			res = ssa_op->op1_def;
			if (res < 0
			 || !ssa->var_info[res].has_range
			 || ssa->var_info[res].range.overflow) {
				if (!OP1_HAS_RANGE()) {
					return 1;
				}
				op1_max = OP1_MAX_RANGE();
				if (op1_max == ZEND_LONG_MAX) {
					return 1;
				}
			}
			return 0;
		case ZEND_PRE_DEC:
		case ZEND_POST_DEC:
			res = ssa_op->op1_def;
			if (res < 0
			 || !ssa->var_info[res].has_range
			 || ssa->var_info[res].range.underflow) {
				if (!OP1_HAS_RANGE()) {
					return 1;
				}
				op1_min = OP1_MIN_RANGE();
				if (op1_min == ZEND_LONG_MIN) {
					return 1;
				}
			}
			return 0;
		case ZEND_ADD:
			res = ssa_op->result_def;
			if (res < 0
			 || !ssa->var_info[res].has_range
			 || ssa->var_info[res].range.underflow) {
				if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
					return 1;
				}
				op1_min = OP1_MIN_RANGE();
				op2_min = OP2_MIN_RANGE();
				if (zend_add_will_overflow(op1_min, op2_min)) {
					return 1;
				}
			}
			if (res < 0
			 || !ssa->var_info[res].has_range
			 || ssa->var_info[res].range.overflow) {
				if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
					return 1;
				}
				op1_max = OP1_MAX_RANGE();
				op2_max = OP2_MAX_RANGE();
				if (zend_add_will_overflow(op1_max, op2_max)) {
					return 1;
				}
			}
			return 0;
		case ZEND_SUB:
			res = ssa_op->result_def;
			if (res < 0
			 || !ssa->var_info[res].has_range
			 || ssa->var_info[res].range.underflow) {
				if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
					return 1;
				}
				op1_min = OP1_MIN_RANGE();
				op2_max = OP2_MAX_RANGE();
				if (zend_sub_will_overflow(op1_min, op2_max)) {
					return 1;
				}
			}
			if (res < 0
			 || !ssa->var_info[res].has_range
			 || ssa->var_info[res].range.overflow) {
				if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
					return 1;
				}
				op1_max = OP1_MAX_RANGE();
				op2_min = OP2_MIN_RANGE();
				if (zend_sub_will_overflow(op1_max, op2_min)) {
					return 1;
				}
			}
			return 0;
		case ZEND_MUL:
			res = ssa_op->result_def;
			return (res < 0 ||
				!ssa->var_info[res].has_range ||
				ssa->var_info[res].range.underflow ||
				ssa->var_info[res].range.overflow);
		case ZEND_ASSIGN_OP:
			if (opline->extended_value == ZEND_ADD) {
				res = ssa_op->op1_def;
				if (res < 0
				 || !ssa->var_info[res].has_range
				 || ssa->var_info[res].range.underflow) {
					if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
						return 1;
					}
					op1_min = OP1_MIN_RANGE();
					op2_min = OP2_MIN_RANGE();
					if (zend_add_will_overflow(op1_min, op2_min)) {
						return 1;
					}
				}
				if (res < 0
				 || !ssa->var_info[res].has_range
				 || ssa->var_info[res].range.overflow) {
					if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
						return 1;
					}
					op1_max = OP1_MAX_RANGE();
					op2_max = OP2_MAX_RANGE();
					if (zend_add_will_overflow(op1_max, op2_max)) {
						return 1;
					}
				}
				return 0;
			} else if (opline->extended_value == ZEND_SUB) {
				res = ssa_op->op1_def;
				if (res < 0
				 || !ssa->var_info[res].has_range
				 || ssa->var_info[res].range.underflow) {
					if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
						return 1;
					}
					op1_min = OP1_MIN_RANGE();
					op2_max = OP2_MAX_RANGE();
					if (zend_sub_will_overflow(op1_min, op2_max)) {
						return 1;
					}
				}
				if (res < 0
				 || !ssa->var_info[res].has_range
				 || ssa->var_info[res].range.overflow) {
					if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
						return 1;
					}
					op1_max = OP1_MAX_RANGE();
					op2_min = OP2_MIN_RANGE();
					if (zend_sub_will_overflow(op1_max, op2_min)) {
						return 1;
					}
				}
				return 0;
			} else if (opline->extended_value == ZEND_MUL) {
				res = ssa_op->op1_def;
				return (res < 0 ||
					!ssa->var_info[res].has_range ||
					ssa->var_info[res].range.underflow ||
					ssa->var_info[res].range.overflow);
			}
			ZEND_FALLTHROUGH;
		default:
			return 1;
	}
}

static int zend_jit_build_cfg(const zend_op_array *op_array, zend_cfg *cfg)
{
	uint32_t flags;

	flags = ZEND_CFG_STACKLESS | ZEND_CFG_NO_ENTRY_PREDECESSORS | ZEND_SSA_RC_INFERENCE_FLAG | ZEND_SSA_USE_CV_RESULTS | ZEND_CFG_RECV_ENTRY;

	zend_build_cfg(&CG(arena), op_array, flags, cfg);

	/* Don't JIT huge functions. Apart from likely being detrimental due to the amount of
	 * generated code, some of our analysis is recursive and will stack overflow with many
	 * blocks. */
	if (cfg->blocks_count > 100000) {
		return FAILURE;
	}

	zend_cfg_build_predecessors(&CG(arena), cfg);

	/* Compute Dominators Tree */
	zend_cfg_compute_dominators_tree(op_array, cfg);

	/* Identify reducible and irreducible loops */
	zend_cfg_identify_loops(op_array, cfg);

	return SUCCESS;
}

static int zend_jit_op_array_analyze1(const zend_op_array *op_array, zend_script *script, zend_ssa *ssa)
{
	if (zend_jit_build_cfg(op_array, &ssa->cfg) != SUCCESS) {
		return FAILURE;
	}

#if 0
	/* TODO: debugger and profiler supports? */
	if ((ssa->cfg.flags & ZEND_FUNC_HAS_EXTENDED_INFO)) {
		return FAILURE;
	}
#endif

	/* TODO: move this to zend_cfg.c ? */
	if (!op_array->function_name) {
		ssa->cfg.flags |= ZEND_FUNC_INDIRECT_VAR_ACCESS;
	}

	if ((JIT_G(opt_level) >= ZEND_JIT_LEVEL_OPT_FUNC)
	 && ssa->cfg.blocks
	 && op_array->last_try_catch == 0
	 && !(op_array->fn_flags & ZEND_ACC_GENERATOR)
	 && !(ssa->cfg.flags & ZEND_FUNC_INDIRECT_VAR_ACCESS)) {
		if (zend_build_ssa(&CG(arena), script, op_array, ZEND_SSA_RC_INFERENCE | ZEND_SSA_USE_CV_RESULTS, ssa) != SUCCESS) {
			return FAILURE;
		}

		zend_ssa_compute_use_def_chains(&CG(arena), op_array, ssa);

		zend_ssa_find_false_dependencies(op_array, ssa);

		zend_ssa_find_sccs(op_array, ssa);
	}

	return SUCCESS;
}

static int zend_jit_op_array_analyze2(const zend_op_array *op_array, zend_script *script, zend_ssa *ssa, uint32_t optimization_level)
{
	if ((JIT_G(opt_level) >= ZEND_JIT_LEVEL_OPT_FUNC)
	 && ssa->cfg.blocks
	 && op_array->last_try_catch == 0
	 && !(op_array->fn_flags & ZEND_ACC_GENERATOR)
	 && !(ssa->cfg.flags & ZEND_FUNC_INDIRECT_VAR_ACCESS)) {
		if (zend_ssa_inference(&CG(arena), op_array, script, ssa,
				optimization_level & ~ZEND_OPTIMIZER_NARROW_TO_DOUBLE) != SUCCESS) {
			return FAILURE;
		}
	}

	return SUCCESS;
}

#ifndef ZEND_JIT_IR
static int zend_jit_add_range(zend_lifetime_interval **intervals, int var, uint32_t from, uint32_t to)
{
	zend_lifetime_interval *ival = intervals[var];

	if (!ival) {
		ival = zend_arena_alloc(&CG(arena), sizeof(zend_lifetime_interval));
		if (!ival) {
			return FAILURE;
		}
		ival->ssa_var = var;
		ival->reg = ZREG_NONE;
		ival->flags = 0;
		ival->range.start = from;
		ival->range.end = to;
		ival->range.next = NULL;
		ival->hint = NULL;
		ival->used_as_hint = NULL;
		intervals[var] = ival;
	} else if (ival->range.start > to + 1) {
		zend_life_range *range = zend_arena_alloc(&CG(arena), sizeof(zend_life_range));

		if (!range) {
			return FAILURE;
		}
		range->start = ival->range.start;
		range->end   = ival->range.end;
		range->next  = ival->range.next;
		ival->range.start = from;
		ival->range.end = to;
		ival->range.next = range;
	} else if (ival->range.start == to + 1) {
		ival->range.start = from;
	} else {
		zend_life_range *range = &ival->range;
		zend_life_range *last = NULL;

		do {
			if (range->start > to + 1) {
				break;
			} else if (range->end + 1 >= from) {
				if (range->start > from) {
					range->start = from;
				}
				last = range;
				range = range->next;
				while (range) {
					if (range->start > to + 1) {
						break;
					}
					last->end = range->end;
					range = range->next;
					last->next = range;
				}
				if (to > last->end) {
					last->end = to;
				}
				return SUCCESS;
			}
			last = range;
			range = range->next;
		} while (range);

		range = zend_arena_alloc(&CG(arena), sizeof(zend_life_range));
		if (!range) {
			return FAILURE;
		}
		range->start = from;
		range->end   = to;
		range->next  = last->next;
		last->next = range;
	}

	return SUCCESS;
}

static int zend_jit_begin_range(zend_lifetime_interval **intervals, int var, uint32_t block_start, uint32_t from)
{
	if (block_start != from && intervals[var]) {
		zend_life_range *range = &intervals[var]->range;

		do {
			if (from >= range->start && from <= range->end) {
				if (range->start == block_start) {
					range->start = from;
				} else {
					zend_life_range *r = zend_arena_alloc(&CG(arena), sizeof(zend_life_range));
					if (!r) {
						return FAILURE;
					}
					r->start = from;
					r->end = range->end;
					r->next = range->next;
					range->end = block_start - 1;
					range->next = r;
				}
				return SUCCESS;
			}
			range = range->next;
		} while (range);
	}

	// dead store
	return zend_jit_add_range(intervals, var, from, from);
}

static void zend_jit_insert_interval(zend_lifetime_interval **list, zend_lifetime_interval *ival)
{
	while (1) {
		if (*list == NULL) {
			*list = ival;
			ival->list_next = NULL;
			return;
		} else if (ival->range.start < (*list)->range.start) {
			ival->list_next = *list;
			*list = ival;
			return;
		}
		list = &(*list)->list_next;
	}
}

static int zend_jit_split_interval(zend_lifetime_interval *current, uint32_t pos, zend_lifetime_interval **list, zend_lifetime_interval **free)
{
	zend_lifetime_interval *ival;
	zend_life_range *range = &current->range;
	zend_life_range *prev = NULL;

	if (*free) {
		ival = *free;
		*free = ival->list_next;
	} else {
		ival = zend_arena_alloc(&CG(arena), sizeof(zend_lifetime_interval));

		if (!ival) {
			return FAILURE;
		}
	}

	current->flags |= ZREG_STORE;

	ival->ssa_var = current->ssa_var;
	ival->reg     = ZREG_NONE;
	ival->flags  |= ZREG_SPLIT | ZREG_LOAD;
	ival->flags  &= ~ZREG_STORE;
	ival->hint    = NULL;

	do {
		if (pos >= range->start && pos <= range->end) {
			break;
		}
		prev = range;
		range = range->next;
	} while(range);

	ZEND_ASSERT(range != NULL);

	ival->range.start   = pos;
	ival->range.end     = range->end;
	ival->range.next    = range->next;

	if (pos == range->start) {
		ZEND_ASSERT(prev != NULL);
		prev->next = NULL;
	} else {
		range->end = pos - 1;
	}

	zend_jit_insert_interval(list, ival);

	return SUCCESS;
}

static zend_lifetime_interval *zend_jit_sort_intervals(zend_lifetime_interval **intervals, int count)
{
	zend_lifetime_interval *list, *last;
	int i;

	list = NULL;
	i = 0;
	while (i < count) {
		list = intervals[i];
		i++;
		if (list) {
			last = list;
			last->list_next = NULL;
			break;
		}
	}

	while (i < count) {
		zend_lifetime_interval *ival = intervals[i];

		i++;
		if (ival) {
			if ((ival->range.start > last->range.start) ||
			    (ival->range.start == last->range.start &&
			     ((!ival->hint && last->hint && last->hint != ival) ||
			      ival->range.end > last->range.end))) {
				last->list_next = ival;
				last = ival;
				ival->list_next = NULL;
			} else {
				zend_lifetime_interval **p = &list;

				while (1) {
					if (*p == NULL) {
						*p = last = ival;
						ival->list_next = NULL;
						break;
					} else if ((ival->range.start < (*p)->range.start) ||
					           (ival->range.start == (*p)->range.start &&
					            ((ival->hint && !(*p)->hint && ival->hint != *p) ||
					             ival->range.end < (*p)->range.end))) {
						ival->list_next = *p;
						*p = ival;
						break;
					}
					p = &(*p)->list_next;
				}
			}
		}
	}

	return list;
}

static ZEND_ATTRIBUTE_UNUSED void zend_jit_print_regset(zend_regset regset)
{
	zend_reg reg;
	int first = 1;

	ZEND_REGSET_FOREACH(regset, reg) {
		if (first) {
			first = 0;
			fprintf(stderr, "%s", zend_reg_name[reg]);
		} else {
			fprintf(stderr, ", %s", zend_reg_name[reg]);
		}
	} ZEND_REGSET_FOREACH_END();
}

static int *zend_jit_compute_block_order_int(zend_ssa *ssa, int n, int *block_order)
{
	zend_basic_block *b = ssa->cfg.blocks + n;

tail_call:
	*block_order = n;
	block_order++;

	n = b->children;
	while (n >= 0) {
		b = ssa->cfg.blocks + n;
		if (b->next_child < 0) {
			goto tail_call;
		}
		block_order = zend_jit_compute_block_order_int(ssa, n, block_order);
		n = b->next_child;
	}

	return block_order;
}

static int zend_jit_compute_block_order(zend_ssa *ssa, int *block_order)
{
	int *end = zend_jit_compute_block_order_int(ssa, 0, block_order);

	return end - block_order;
}

static bool zend_jit_in_loop(zend_ssa *ssa, int header, zend_basic_block *b)
{
	while (b->loop_header >= 0) {
		if (b->loop_header == header) {
			return 1;
		}
		b = ssa->cfg.blocks + b->loop_header;
	}
	return 0;
}

static void zend_jit_compute_loop_body(zend_ssa *ssa, int header, int n, zend_bitset loop_body)
{
	zend_basic_block *b = ssa->cfg.blocks + n;
	uint32_t i;

tail_call:
	if (b->len) {
		for (i = b->start; i < b->start + b->len; i++) {
			zend_bitset_incl(loop_body, i);
		}
	}

	n = b->children;
	while (n >= 0) {
		b = ssa->cfg.blocks + n;
		if (zend_jit_in_loop(ssa, header, b)) {
			if (b->next_child < 0) {
				goto tail_call;
			}
			zend_jit_compute_loop_body(ssa, header, n, loop_body);
		}
		n = b->next_child;
	}
}

static void zend_jit_add_hint(zend_lifetime_interval **intervals, int dst, int src)
{
	if (intervals[dst]->range.start < intervals[src]->range.start) {
		int tmp = src;
		src = dst;
		dst = tmp;
	}
	while (dst != src && intervals[dst]->hint) {
		if (intervals[dst]->hint->range.start < intervals[src]->range.start) {
			int tmp = src;
			src = intervals[dst]->hint->ssa_var;
			dst = tmp;
		} else {
			dst = intervals[dst]->hint->ssa_var;
		}
	}
	if (dst != src) {
		intervals[dst]->hint = intervals[src];
	}
}

/* See "Linear Scan Register Allocation on SSA Form", Christian Wimmer and
   Michael Franz, CGO'10 (2010), Figure 4. */
static int zend_jit_compute_liveness(const zend_op_array *op_array, zend_ssa *ssa, zend_bitset candidates, zend_lifetime_interval **list)
{
	int set_size, i, j, k, l;
	uint32_t n;
	zend_bitset live, live_in, pi_vars, loop_body;
	int *block_order;
	zend_ssa_phi *phi;
	zend_lifetime_interval **intervals;
	size_t mem_size;
	ALLOCA_FLAG(use_heap);

	set_size = zend_bitset_len(ssa->vars_count);
	mem_size =
		ZEND_MM_ALIGNED_SIZE(ssa->vars_count * sizeof(zend_lifetime_interval*)) +
		ZEND_MM_ALIGNED_SIZE((set_size * ssa->cfg.blocks_count) * ZEND_BITSET_ELM_SIZE) +
		ZEND_MM_ALIGNED_SIZE(set_size * ZEND_BITSET_ELM_SIZE) +
		ZEND_MM_ALIGNED_SIZE(set_size * ZEND_BITSET_ELM_SIZE) +
		ZEND_MM_ALIGNED_SIZE(zend_bitset_len(op_array->last) * ZEND_BITSET_ELM_SIZE) +
		ZEND_MM_ALIGNED_SIZE(ssa->cfg.blocks_count * sizeof(int));
	intervals = do_alloca(mem_size, use_heap);
	if (!intervals) {
		*list = NULL;
		return FAILURE;
	}

	live_in = (zend_bitset)((char*)intervals + ZEND_MM_ALIGNED_SIZE(ssa->vars_count * sizeof(zend_lifetime_interval*)));
	live = (zend_bitset)((char*)live_in + ZEND_MM_ALIGNED_SIZE((set_size * ssa->cfg.blocks_count) * ZEND_BITSET_ELM_SIZE));
	pi_vars = (zend_bitset)((char*)live + ZEND_MM_ALIGNED_SIZE(set_size * ZEND_BITSET_ELM_SIZE));
	loop_body = (zend_bitset)((char*)pi_vars + ZEND_MM_ALIGNED_SIZE(set_size * ZEND_BITSET_ELM_SIZE));
	block_order = (int*)((char*)loop_body + ZEND_MM_ALIGNED_SIZE(zend_bitset_len(op_array->last) * ZEND_BITSET_ELM_SIZE));

	memset(intervals, 0, ssa->vars_count * sizeof(zend_lifetime_interval*));
	zend_bitset_clear(live_in, set_size * ssa->cfg.blocks_count);

	/* TODO: Provide a linear block order where all dominators of a block
	 * are before this block, and where all blocks belonging to the same loop
	 * are contiguous ???
	 */
	for (l = zend_jit_compute_block_order(ssa, block_order) - 1; l >= 0; l--) {
		zend_basic_block *b;

		i = block_order[l];
		b = ssa->cfg.blocks + i;

		/* live = UNION of successor.liveIn for each successor of b */
		/* live.add(phi.inputOf(b)) for each phi of successors of b */
		zend_bitset_clear(live, set_size);
		for (j = 0; j < b->successors_count; j++) {
			int succ = b->successors[j];

			zend_bitset_union(live, live_in + set_size * succ, set_size);
			zend_bitset_clear(pi_vars, set_size);
			for (phi = ssa->blocks[succ].phis; phi; phi = phi->next) {
				if (ssa->vars[phi->ssa_var].no_val) {
					/* skip */
				} else if (phi->pi >= 0) {
					if (phi->pi == i && phi->sources[0] >= 0) {
						if (zend_bitset_in(candidates, phi->sources[0])) {
							zend_bitset_incl(live, phi->sources[0]);
						}
						zend_bitset_incl(pi_vars, phi->var);
					}
				} else if (!zend_bitset_in(pi_vars, phi->var)) {
					for (k = 0; k < ssa->cfg.blocks[succ].predecessors_count; k++) {
						if (ssa->cfg.predecessors[ssa->cfg.blocks[succ].predecessor_offset + k] == i) {
							if (phi->sources[k] >= 0 && zend_bitset_in(candidates, phi->sources[k])) {
								zend_bitset_incl(live, phi->sources[k]);
							}
							break;
						}
					}
				}
			}
		}

		/* addRange(var, b.from, b.to) for each var in live */
		ZEND_BITSET_FOREACH(live, set_size, j) {
			if (zend_bitset_in(candidates, j)) {
				if (zend_jit_add_range(intervals, j, b->start, b->start + b->len - 1) != SUCCESS) {
					goto failure;
				}
			}
		} ZEND_BITSET_FOREACH_END();

		/* for each operation op of b in reverse order */
		for (n = b->start + b->len; n > b->start;) {
			zend_ssa_op *op;
			const zend_op *opline;
			uint32_t num;

			n--;
			op = ssa->ops + n;
			opline = op_array->opcodes + n;

			if (UNEXPECTED(opline->opcode == ZEND_OP_DATA)) {
				num = n - 1;
			} else {
				num = n;
			}

			/* for each output operand opd of op do */
			/*   setFrom(opd, op)                   */
			/*   live.remove(opd)                   */
			if (op->op1_def >= 0 && zend_bitset_in(candidates, op->op1_def)) {
				if (zend_jit_begin_range(intervals, op->op1_def, b->start, num) != SUCCESS) {
					goto failure;
				}
				zend_bitset_excl(live, op->op1_def);
			}
			if (op->op2_def >= 0 && zend_bitset_in(candidates, op->op2_def)) {
				if (zend_jit_begin_range(intervals, op->op2_def, b->start, num) != SUCCESS) {
					goto failure;
				}
				zend_bitset_excl(live, op->op2_def);
			}
			if (op->result_def >= 0 && zend_bitset_in(candidates, op->result_def)) {
				if (zend_jit_begin_range(intervals, op->result_def, b->start, num) != SUCCESS) {
					goto failure;
				}
				zend_bitset_excl(live, op->result_def);
			}

			/* for each input operand opd of op do */
			/*   live.add(opd)                     */
			/*   addRange(opd, b.from, op)         */
			if (op->op1_use >= 0
			 && zend_bitset_in(candidates, op->op1_use)
			 && !zend_ssa_is_no_val_use(opline, op, op->op1_use)) {
				zend_bitset_incl(live, op->op1_use);
				if (zend_jit_add_range(intervals, op->op1_use, b->start, num) != SUCCESS) {
					goto failure;
				}
			}
			if (op->op2_use >= 0
			 && zend_bitset_in(candidates, op->op2_use)
			 && !zend_ssa_is_no_val_use(opline, op, op->op2_use)) {
				zend_bitset_incl(live, op->op2_use);
				if (zend_jit_add_range(intervals, op->op2_use, b->start, num) != SUCCESS) {
					goto failure;
				}
			}
			if (op->result_use >= 0
			 && zend_bitset_in(candidates, op->result_use)
			 && !zend_ssa_is_no_val_use(opline, op, op->result_use)) {
				zend_bitset_incl(live, op->result_use);
				if (zend_jit_add_range(intervals, op->result_use, b->start, num) != SUCCESS) {
					goto failure;
				}
			}
		}

		/* live.remove(phi.output) for each phi of b */
		for (phi = ssa->blocks[i].phis; phi; phi = phi->next) {
			zend_bitset_excl(live, phi->ssa_var);
		}

		/* b.liveIn = live */
		zend_bitset_copy(live_in + set_size * i, live, set_size);
	}

	for (i = ssa->cfg.blocks_count - 1; i >= 0; i--) {
		zend_basic_block *b = ssa->cfg.blocks + i;

		/* if b is loop header */
		if ((b->flags & ZEND_BB_LOOP_HEADER)) {
			live = live_in + set_size * i;

			if (!zend_bitset_empty(live, set_size)) {
				uint32_t set_size2 = zend_bitset_len(op_array->last);

				zend_bitset_clear(loop_body, set_size2);
				zend_jit_compute_loop_body(ssa, i, i, loop_body);
				while (!zend_bitset_empty(loop_body, set_size2)) {
					uint32_t from = zend_bitset_first(loop_body, set_size2);
					uint32_t to = from;

					do {
						zend_bitset_excl(loop_body, to);
						to++;
					} while (zend_bitset_in(loop_body, to));
					to--;

					ZEND_BITSET_FOREACH(live, set_size, j) {
						if (zend_jit_add_range(intervals, j, from, to) != SUCCESS) {
							goto failure;
						}
					} ZEND_BITSET_FOREACH_END();
				}
			}
		}

	}

	if (JIT_G(opt_flags) & ZEND_JIT_REG_ALLOC_GLOBAL) {
		/* Register hinting (a cheap way for register coalescing) */
		for (i = 0; i < ssa->vars_count; i++) {
			if (intervals[i]) {
				int src;

				if (ssa->vars[i].definition_phi) {
					zend_ssa_phi *phi = ssa->vars[i].definition_phi;

					if (phi->pi >= 0) {
						src = phi->sources[0];
						if (intervals[src]) {
							zend_jit_add_hint(intervals, i, src);
						}
					} else {
						for (k = 0; k < ssa->cfg.blocks[phi->block].predecessors_count; k++) {
							src = phi->sources[k];
							if (src >= 0) {
								if (ssa->vars[src].definition_phi
								 && ssa->vars[src].definition_phi->pi >= 0
								 && phi->block == ssa->vars[src].definition_phi->block) {
									/* Skip zero-length interval for Pi variable */
									src = ssa->vars[src].definition_phi->sources[0];
								}
								if (intervals[src]) {
									zend_jit_add_hint(intervals, i, src);
								}
							}
						}
					}
				}
			}
		}
		for (i = 0; i < ssa->vars_count; i++) {
			if (intervals[i] && !intervals[i]->hint) {

				if (ssa->vars[i].definition >= 0) {
					uint32_t line = ssa->vars[i].definition;
					const zend_op *opline = op_array->opcodes + line;

					switch (opline->opcode) {
						case ZEND_QM_ASSIGN:
						case ZEND_POST_INC:
						case ZEND_POST_DEC:
							if (ssa->ops[line].op1_use >= 0 &&
							    intervals[ssa->ops[line].op1_use] &&
							    (i == ssa->ops[line].op1_def ||
							     (i == ssa->ops[line].result_def &&
							      (ssa->ops[line].op1_def < 0 ||
							       !intervals[ssa->ops[line].op1_def])))) {
								zend_jit_add_hint(intervals, i, ssa->ops[line].op1_use);
							}
							break;
						case ZEND_SEND_VAR:
						case ZEND_PRE_INC:
						case ZEND_PRE_DEC:
							if (i == ssa->ops[line].op1_def &&
							    ssa->ops[line].op1_use >= 0 &&
							    intervals[ssa->ops[line].op1_use]) {
								zend_jit_add_hint(intervals, i, ssa->ops[line].op1_use);
							}
							break;
						case ZEND_ASSIGN:
							if (ssa->ops[line].op2_use >= 0 &&
							    intervals[ssa->ops[line].op2_use] &&
							    (i == ssa->ops[line].op2_def ||
								 (i == ssa->ops[line].op1_def &&
							      (ssa->ops[line].op2_def < 0 ||
							       !intervals[ssa->ops[line].op2_def])) ||
								 (i == ssa->ops[line].result_def &&
							      (ssa->ops[line].op2_def < 0 ||
							       !intervals[ssa->ops[line].op2_def]) &&
							      (ssa->ops[line].op1_def < 0 ||
							       !intervals[ssa->ops[line].op1_def])))) {
								zend_jit_add_hint(intervals, i, ssa->ops[line].op2_use);
							}
							break;
						case ZEND_SUB:
						case ZEND_ADD:
						case ZEND_MUL:
						case ZEND_BW_OR:
						case ZEND_BW_AND:
						case ZEND_BW_XOR:
							if (i == ssa->ops[line].result_def) {
								if (ssa->ops[line].op1_use >= 0 &&
								    intervals[ssa->ops[line].op1_use] &&
								    ssa->ops[line].op1_use_chain < 0 &&
								    !ssa->vars[ssa->ops[line].op1_use].phi_use_chain &&
								    (ssa->var_info[i].type & MAY_BE_ANY) ==
								        (ssa->var_info[ssa->ops[line].op1_use].type & MAY_BE_ANY)) {
									zend_jit_add_hint(intervals, i, ssa->ops[line].op1_use);
								} else if (opline->opcode != ZEND_SUB &&
								    ssa->ops[line].op2_use >= 0 &&
								    intervals[ssa->ops[line].op2_use] &&
								    ssa->ops[line].op2_use_chain < 0 &&
								    !ssa->vars[ssa->ops[line].op2_use].phi_use_chain &&
								    (ssa->var_info[i].type & MAY_BE_ANY) ==
								        (ssa->var_info[ssa->ops[line].op2_use].type & MAY_BE_ANY)) {
									zend_jit_add_hint(intervals, i, ssa->ops[line].op2_use);
								}
							}
							break;
					}
				}
			}
		}
	}

	*list = zend_jit_sort_intervals(intervals, ssa->vars_count);

	if (*list) {
		zend_lifetime_interval *ival = *list;
		while (ival) {
			if (ival->hint) {
				ival->hint->used_as_hint = ival;
			}
			ival = ival->list_next;
		}
	}

	free_alloca(intervals, use_heap);
	return SUCCESS;

failure:
	*list = NULL;
	free_alloca(intervals, use_heap);
	return FAILURE;
}

static uint32_t zend_interval_end(zend_lifetime_interval *ival)
{
	zend_life_range *range = &ival->range;

	while (range->next) {
		range = range->next;
	}
	return range->end;
}

static bool zend_interval_covers(zend_lifetime_interval *ival, uint32_t position)
{
	zend_life_range *range = &ival->range;

	do {
		if (position >= range->start && position <= range->end) {
			return 1;
		}
		range = range->next;
	} while (range);

	return 0;
}

static uint32_t zend_interval_intersection(zend_lifetime_interval *ival1, zend_lifetime_interval *ival2)
{
	zend_life_range *r1 = &ival1->range;
	zend_life_range *r2 = &ival2->range;

	do {
		if (r1->start <= r2->end) {
			if (r2->start <= r1->end) {
				return MAX(r1->start, r2->start);
			} else {
				r2 = r2->next;
			}
		} else {
			r1 = r1->next;
		}
	} while (r1 && r2);

	return 0xffffffff;
}

/* See "Optimized Interval Splitting in a Linear Scan Register Allocator",
   Christian Wimmer VEE'05 (2005), Figure 4. Allocation without spilling */
static int zend_jit_try_allocate_free_reg(const zend_op_array *op_array, const zend_op **ssa_opcodes, zend_ssa *ssa, zend_lifetime_interval *current, zend_regset available, zend_regset *hints, zend_lifetime_interval *active, zend_lifetime_interval *inactive, zend_lifetime_interval **list, zend_lifetime_interval **free)
{
	zend_lifetime_interval *it;
	uint32_t freeUntilPos[ZREG_NUM];
	uint32_t pos, pos2;
	zend_reg i, reg, reg2;
	zend_reg hint = ZREG_NONE;
	zend_regset low_priority_regs;
	zend_life_range *range;

	if ((ssa->var_info[current->ssa_var].type & MAY_BE_ANY) == MAY_BE_DOUBLE) {
		available = ZEND_REGSET_INTERSECTION(available, ZEND_REGSET_FP);
	} else {
		available = ZEND_REGSET_INTERSECTION(available, ZEND_REGSET_GP);
	}

	/* TODO: Allow usage of preserved registers ???
	 * Their values have to be stored in prologue and restored in epilogue
	 */
	available = ZEND_REGSET_DIFFERENCE(available, ZEND_REGSET_PRESERVED);

	/* Set freeUntilPos of all physical registers to maxInt */
	for (i = 0; i < ZREG_NUM; i++) {
		freeUntilPos[i] = 0xffffffff;
	}

	/* for each interval it in active do */
	/*   freeUntilPos[it.reg] = 0        */
	it = active;
	if (ssa->vars[current->ssa_var].definition == current->range.start) {
		while (it) {
			if (current->range.start != zend_interval_end(it)) {
				freeUntilPos[it->reg] = 0;
			} else if (zend_jit_may_reuse_reg(
					ssa_opcodes ? ssa_opcodes[current->range.start] : op_array->opcodes + current->range.start,
					ssa->ops + current->range.start, ssa, current->ssa_var, it->ssa_var)) {
				if (!ZEND_REGSET_IN(*hints, it->reg) &&
				    /* TODO: Avoid most often scratch registers. Find a better way ??? */
				    (!current->used_as_hint ||
				     !ZEND_REGSET_IN(ZEND_REGSET_LOW_PRIORITY, it->reg))) {
					hint = it->reg;
				}
			} else {
				freeUntilPos[it->reg] = 0;
			}
			it = it->list_next;
		}
	} else {
		while (it) {
			freeUntilPos[it->reg] = 0;
			it = it->list_next;
		}
	}
	if (current->hint) {
		hint = current->hint->reg;
		if (hint != ZREG_NONE && current->hint->used_as_hint == current) {
			ZEND_REGSET_EXCL(*hints, hint);
		}
	}

	if (hint == ZREG_NONE && ZEND_REGSET_IS_EMPTY(available)) {
		return 0;
	}

	/* See "Linear Scan Register Allocation on SSA Form", Christian Wimmer and
	   Michael Franz, CGO'10 (2010), Figure 6. */
	if (current->flags & ZREG_SPLIT) {
		/* for each interval it in inactive intersecting with current do */
		/*   freeUntilPos[it.reg] = next intersection of it with current */
		it = inactive;
		while (it) {
			uint32_t next = zend_interval_intersection(current, it);

			//ZEND_ASSERT(next != 0xffffffff && !current->split);
			if (next < freeUntilPos[it->reg]) {
				freeUntilPos[it->reg] = next;
			}
			it = it->list_next;
		}
	}

	/* Handle Scratch Registers */
	/* TODO: Optimize ??? */
	range = &current->range;
	do {
		uint32_t line = range->start;
		uint32_t last_use_line = (uint32_t)-1;
		zend_regset regset;
		zend_reg reg;

		if ((current->flags & ZREG_LAST_USE) && !range->next) {
			last_use_line = range->end;
		}
		if (ssa->ops[line].op1_def == current->ssa_var ||
		    ssa->ops[line].op2_def == current->ssa_var ||
		    ssa->ops[line].result_def == current->ssa_var) {
			regset = zend_jit_get_def_scratch_regset(
				ssa_opcodes ? ssa_opcodes[line] : op_array->opcodes + line,
				ssa->ops + line,
				op_array, ssa, current->ssa_var, line == last_use_line);
			ZEND_REGSET_FOREACH(regset, reg) {
				if (line < freeUntilPos[reg]) {
					freeUntilPos[reg] = line;
				}
			} ZEND_REGSET_FOREACH_END();
			line++;
		}
		while (line <= range->end) {
			regset = zend_jit_get_scratch_regset(
				ssa_opcodes ? ssa_opcodes[line] : op_array->opcodes + line,
				ssa->ops + line,
				op_array, ssa, current->ssa_var, line == last_use_line);
			ZEND_REGSET_FOREACH(regset, reg) {
				if (line < freeUntilPos[reg]) {
					freeUntilPos[reg] = line;
				}
			} ZEND_REGSET_FOREACH_END();
			line++;
		}
		range = range->next;
	} while (range);

#if 0
	/* Coalescing */
	if (ssa->vars[current->ssa_var].definition == current->start) {
		zend_op *opline = op_array->opcodes + current->start;
		int hint = -1;

		switch (opline->opcode) {
			case ZEND_ASSIGN:
				hint = ssa->ops[current->start].op2_use;
			case ZEND_QM_ASSIGN:
				hint = ssa->ops[current->start].op1_use;
				break;
			case ZEND_ADD:
			case ZEND_SUB:
			case ZEND_MUL:
				hint = ssa->ops[current->start].op1_use;
				break;
			case ZEND_ASSIGN_OP:
				if (opline->extended_value == ZEND_ADD
				 || opline->extended_value == ZEND_SUB
				 || opline->extended_value == ZEND_MUL) {
					hint = ssa->ops[current->start].op1_use;
				}
				break;
		}
		if (hint >= 0) {
		}
	}
#endif

	if (hint != ZREG_NONE && freeUntilPos[hint] > zend_interval_end(current)) {
		current->reg = hint;
		if (current->used_as_hint) {
			ZEND_REGSET_INCL(*hints, hint);
		}
		return 1;
	}

	if (ZEND_REGSET_IS_EMPTY(available)) {
		return 0;
	}

	pos = 0; reg = ZREG_NONE;
	pos2 = 0; reg2 = ZREG_NONE;
	low_priority_regs = *hints;
	if (current->used_as_hint) {
		/* TODO: Avoid most often scratch registers. Find a better way ??? */
		low_priority_regs = ZEND_REGSET_UNION(low_priority_regs, ZEND_REGSET_LOW_PRIORITY);
	}

	ZEND_REGSET_FOREACH(available, i) {
		if (ZEND_REGSET_IN(low_priority_regs, i)) {
			if (freeUntilPos[i] > pos2) {
				reg2 = i;
				pos2 = freeUntilPos[i];
			}
		} else if (freeUntilPos[i] > pos) {
			reg = i;
			pos = freeUntilPos[i];
		}
	} ZEND_REGSET_FOREACH_END();

	if (reg == ZREG_NONE) {
		if (reg2 != ZREG_NONE) {
			reg = reg2;
			pos = pos2;
			reg2 = ZREG_NONE;
		}
	}

	if (reg == ZREG_NONE) {
		/* no register available without spilling */
		return 0;
	} else if (zend_interval_end(current) < pos) {
		/* register available for the whole interval */
		current->reg = reg;
		if (current->used_as_hint) {
			ZEND_REGSET_INCL(*hints, reg);
		}
		return 1;
#if 0
	// TODO: allow low priority register usage
	} else if (reg2 != ZREG_NONE && zend_interval_end(current) < pos2) {
		/* register available for the whole interval */
		current->reg = reg2;
		if (current->used_as_hint) {
			ZEND_REGSET_INCL(*hints, reg2);
		}
		return 1;
#endif
	} else {
		/* TODO: enable interval splitting ??? */
		/* register available for the first part of the interval */
		if (1 || zend_jit_split_interval(current, pos, list, free) != SUCCESS) {
			return 0;
		}
		current->reg = reg;
		if (current->used_as_hint) {
			ZEND_REGSET_INCL(*hints, reg);
		}
		return 1;
	}
}

/* See "Optimized Interval Splitting in a Linear Scan Register Allocator",
   Christian Wimmer VEE'05 (2005), Figure 5. Allocation with spilling.
   and "Linear Scan Register Allocation on SSA Form", Christian Wimmer and
   Michael Franz, CGO'10 (2010), Figure 6. */
static int zend_jit_allocate_blocked_reg(void)
{
	/* TODO: ??? */
	return 0;
}

/* See "Optimized Interval Splitting in a Linear Scan Register Allocator",
   Christian Wimmer VEE'10 (2005), Figure 2. */
static zend_lifetime_interval* zend_jit_linear_scan(const zend_op_array *op_array, const zend_op **ssa_opcodes, zend_ssa *ssa, zend_lifetime_interval *list)
{
	zend_lifetime_interval *unhandled, *active, *inactive, *handled, *free;
	zend_lifetime_interval *current, **p, *q;
	uint32_t position;
	zend_regset available = ZEND_REGSET_UNION(ZEND_REGSET_GP, ZEND_REGSET_FP);
	zend_regset hints = ZEND_REGSET_EMPTY;

	unhandled = list;
	/* active = inactive = handled = free = {} */
	active = inactive = handled = free = NULL;
	while (unhandled != NULL) {
		current = unhandled;
		unhandled = unhandled->list_next;
		position = current->range.start;

		p = &active;
		while (*p) {
			uint32_t end = zend_interval_end(*p);

			q = *p;
			if (end < position) {
				/* move ival from active to handled */
				ZEND_REGSET_INCL(available, q->reg);
				*p = q->list_next;
				q->list_next = handled;
				handled = q;
			} else if (!zend_interval_covers(q, position)) {
				/* move ival from active to inactive */
				ZEND_REGSET_INCL(available, q->reg);
				*p = q->list_next;
				q->list_next = inactive;
				inactive = q;
			} else {
				p = &q->list_next;
			}
		}

		p = &inactive;
		while (*p) {
			uint32_t end = zend_interval_end(*p);

			q = *p;
			if (end < position) {
				/* move ival from inactive to handled */
				*p = q->list_next;
				q->list_next = handled;
				handled = q;
			} else if (zend_interval_covers(q, position)) {
				/* move ival from inactive to active */
				ZEND_REGSET_EXCL(available, q->reg);
				*p = q->list_next;
				q->list_next = active;
				active = q;
			} else {
				p = &q->list_next;
			}
		}

		if (zend_jit_try_allocate_free_reg(op_array, ssa_opcodes, ssa, current, available, &hints, active, inactive, &unhandled, &free) ||
		    zend_jit_allocate_blocked_reg()) {
			ZEND_REGSET_EXCL(available, current->reg);
			current->list_next = active;
			active = current;
		} else {
			current->list_next = free;
			free = current;
		}
	}

	/* move active to handled */
	while (active) {
		current = active;
		active = active->list_next;
		current->list_next = handled;
		handled = current;
	}

	/* move inactive to handled */
	while (inactive) {
		current = inactive;
		inactive = inactive->list_next;
		current->list_next = handled;
		handled = current;
	}

	return handled;
}

static void zend_jit_dump_lifetime_interval(const zend_op_array *op_array, const zend_ssa *ssa, const zend_lifetime_interval *ival)
{
	zend_life_range *range;
	int var_num = ssa->vars[ival->ssa_var].var;

	fprintf(stderr, "#%d.", ival->ssa_var);
	zend_dump_var(op_array, (var_num < op_array->last_var ? IS_CV : 0), var_num);
	fprintf(stderr, ": %u-%u", ival->range.start, ival->range.end);
	range = ival->range.next;
	while (range) {
		fprintf(stderr, ", %u-%u", range->start, range->end);
		range = range->next;
	}
	if (ival->reg != ZREG_NONE) {
		fprintf(stderr, " (%s)", zend_reg_name[ival->reg]);
	}
	if (ival->flags & ZREG_LAST_USE) {
		fprintf(stderr, " last_use");
	}
	if (ival->flags & ZREG_LOAD) {
		fprintf(stderr, " load");
	}
	if (ival->flags & ZREG_STORE) {
		fprintf(stderr, " store");
	}
	if (ival->hint) {
		fprintf(stderr, " hint");
		if (ival->hint->ssa_var >= 0) {
			var_num = ssa->vars[ival->hint->ssa_var].var;
			fprintf(stderr, "=#%d.", ival->hint->ssa_var);
			zend_dump_var(op_array, (var_num < op_array->last_var ? IS_CV : 0), var_num);
		}
		if (ival->hint->reg != ZREG_NONE) {
			fprintf(stderr, " (%s)", zend_reg_name[ival->hint->reg]);
		}
	}
	fprintf(stderr, "\n");
}

static zend_lifetime_interval** zend_jit_allocate_registers(const zend_op_array *op_array, zend_ssa *ssa)
{
	void *checkpoint;
	int set_size, candidates_count, i;
	zend_bitset candidates = NULL;
	zend_lifetime_interval *list, *ival;
	zend_lifetime_interval **intervals;
	ALLOCA_FLAG(use_heap);

	if (!ssa->var_info) {
		return NULL;
	}

	/* Identify SSA variables suitable for register allocation */
	set_size = zend_bitset_len(ssa->vars_count);
	candidates = ZEND_BITSET_ALLOCA(set_size, use_heap);
	if (!candidates) {
		return NULL;
	}
	candidates_count = 0;
	zend_bitset_clear(candidates, set_size);
	for (i = 0; i < ssa->vars_count; i++) {
		if (zend_jit_may_be_in_reg(op_array, ssa, i)) {
			zend_bitset_incl(candidates, i);
			candidates_count++;
		}
	}
	if (!candidates_count) {
		free_alloca(candidates, use_heap);
		return NULL;
	}

	checkpoint = zend_arena_checkpoint(CG(arena));

	/* Find life-time intervals */
	if (zend_jit_compute_liveness(op_array, ssa, candidates, &list) != SUCCESS) {
		goto failure;
	}

	if (list) {
		/* Set ZREG_LAST_USE flags */
		ival = list;
		while (ival) {
			zend_life_range *range = &ival->range;

			while (range->next) {
				range = range->next;
			}
			if (zend_ssa_is_last_use(op_array, ssa, ival->ssa_var, range->end)) {
				ival->flags |= ZREG_LAST_USE;
			}
			ival = ival->list_next;
		}
	}

	if (list) {
		if (JIT_G(debug) & ZEND_JIT_DEBUG_REG_ALLOC) {
			fprintf(stderr, "Live Ranges \"%s\"\n", op_array->function_name ? ZSTR_VAL(op_array->function_name) : "[main]");
			ival = list;
			while (ival) {
				zend_jit_dump_lifetime_interval(op_array, ssa, ival);
				ival = ival->list_next;
			}
			fprintf(stderr, "\n");
		}

		/* Linear Scan Register Allocation */
		list = zend_jit_linear_scan(op_array, NULL, ssa, list);

		if (list) {
			intervals = zend_arena_calloc(&CG(arena), ssa->vars_count, sizeof(zend_lifetime_interval*));
			if (!intervals) {
				goto failure;
			}

			ival = list;
			while (ival != NULL) {
				zend_lifetime_interval *next = ival->list_next;

				ival->list_next = intervals[ival->ssa_var];
				intervals[ival->ssa_var] = ival;
				ival = next;
			}

			if (JIT_G(opt_flags) & ZEND_JIT_REG_ALLOC_GLOBAL) {
				/* Naive SSA resolution */
				for (i = 0; i < ssa->vars_count; i++) {
					if (ssa->vars[i].definition_phi && !ssa->vars[i].no_val) {
						zend_ssa_phi *phi = ssa->vars[i].definition_phi;
						int k, src;

						if (phi->pi >= 0) {
							if (!ssa->vars[i].phi_use_chain
							 || ssa->vars[i].phi_use_chain->block != phi->block) {
								src = phi->sources[0];
								if (intervals[i]) {
									if (!intervals[src]) {
										intervals[i]->flags |= ZREG_LOAD;
									} else if (intervals[i]->reg != intervals[src]->reg) {
										intervals[i]->flags |= ZREG_LOAD;
										intervals[src]->flags |= ZREG_STORE;
									}
								} else if (intervals[src]) {
									intervals[src]->flags |= ZREG_STORE;
								}
							}
						} else {
							int need_move = 0;

							for (k = 0; k < ssa->cfg.blocks[phi->block].predecessors_count; k++) {
								src = phi->sources[k];
								if (src >= 0) {
									if (ssa->vars[src].definition_phi
									 && ssa->vars[src].definition_phi->pi >= 0
									 && phi->block == ssa->vars[src].definition_phi->block) {
										/* Skip zero-length interval for Pi variable */
										src = ssa->vars[src].definition_phi->sources[0];
									}
									if (intervals[i]) {
										if (!intervals[src]) {
											need_move = 1;
										} else if (intervals[i]->reg != intervals[src]->reg) {
											need_move = 1;
										}
									} else if (intervals[src]) {
										need_move = 1;
									}
								}
							}
							if (need_move) {
								if (intervals[i]) {
									intervals[i]->flags |= ZREG_LOAD;
								}
								for (k = 0; k < ssa->cfg.blocks[phi->block].predecessors_count; k++) {
									src = phi->sources[k];
									if (src >= 0) {
										if (ssa->vars[src].definition_phi
										 && ssa->vars[src].definition_phi->pi >= 0
										 && phi->block == ssa->vars[src].definition_phi->block) {
											/* Skip zero-length interval for Pi variable */
											src = ssa->vars[src].definition_phi->sources[0];
										}
										if (intervals[src]) {
											intervals[src]->flags |= ZREG_STORE;
										}
									}
								}
							}
						}
					}
				}
				/* Remove useless register allocation */
				for (i = 0; i < ssa->vars_count; i++) {
					if (intervals[i] &&
					    ((intervals[i]->flags & ZREG_LOAD) ||
					     ((intervals[i]->flags & ZREG_STORE) && ssa->vars[i].definition >= 0)) &&
					    ssa->vars[i].use_chain < 0) {
					    bool may_remove = 1;
						zend_ssa_phi *phi = ssa->vars[i].phi_use_chain;

						while (phi) {
							if (intervals[phi->ssa_var] &&
							    !(intervals[phi->ssa_var]->flags & ZREG_LOAD)) {
								may_remove = 0;
								break;
							}
							phi = zend_ssa_next_use_phi(ssa, i, phi);
						}
						if (may_remove) {
							intervals[i] = NULL;
						}
					}
				}
				/* Remove intervals used once */
				for (i = 0; i < ssa->vars_count; i++) {
					if (intervals[i] &&
					    (intervals[i]->flags & ZREG_LOAD) &&
					    (intervals[i]->flags & ZREG_STORE) &&
					    (ssa->vars[i].use_chain < 0 ||
					     zend_ssa_next_use(ssa->ops, i, ssa->vars[i].use_chain) < 0)) {
						bool may_remove = 1;
						zend_ssa_phi *phi = ssa->vars[i].phi_use_chain;

						while (phi) {
							if (intervals[phi->ssa_var] &&
							    !(intervals[phi->ssa_var]->flags & ZREG_LOAD)) {
								may_remove = 0;
								break;
							}
							phi = zend_ssa_next_use_phi(ssa, i, phi);
						}
						if (may_remove) {
							intervals[i] = NULL;
						}
					}
				}
			}

			if (JIT_G(debug) & ZEND_JIT_DEBUG_REG_ALLOC) {
				fprintf(stderr, "Allocated Live Ranges \"%s\"\n", op_array->function_name ? ZSTR_VAL(op_array->function_name) : "[main]");
				for (i = 0; i < ssa->vars_count; i++) {
					ival = intervals[i];
					while (ival) {
						zend_jit_dump_lifetime_interval(op_array, ssa, ival);
						ival = ival->list_next;
					}
				}
				fprintf(stderr, "\n");
			}

			free_alloca(candidates, use_heap);
			return intervals;
		}
	}

failure:
	zend_arena_release(&CG(arena), checkpoint);
	free_alloca(candidates, use_heap);
	return NULL;
}

#else /* ZEND_JIT_IR */

static void zend_jit_allocate_registers(zend_jit_ctx *ctx, const zend_op_array *op_array, zend_ssa *ssa)
{
	void *checkpoint;
	int candidates_count, i;
	zend_jit_reg_var *ra;

	checkpoint = zend_arena_checkpoint(CG(arena));
	ra = zend_arena_calloc(&CG(arena), ssa->vars_count, sizeof(zend_jit_reg_var));
	candidates_count = 0;
	for (i = 0; i < ssa->vars_count; i++) {
		if (zend_jit_may_be_in_reg(op_array, ssa, i)) {
			ra[i].ref = IR_NULL;
			candidates_count++;
		}
	}
	if (!candidates_count) {
		zend_arena_release(&CG(arena), checkpoint);
		return;
	}

			if (JIT_G(opt_flags) & ZEND_JIT_REG_ALLOC_GLOBAL) {
				/* Naive SSA resolution */
				for (i = 0; i < ssa->vars_count; i++) {
					if (ssa->vars[i].definition_phi && !ssa->vars[i].no_val) {
						zend_ssa_phi *phi = ssa->vars[i].definition_phi;
						int k, src;

						if (phi->pi >= 0) {
							src = phi->sources[0];
							if (ra[i].ref) {
								if (!ra[src].ref) {
									ra[i].flags |= ZREG_LOAD;
								} else {
									ra[i].flags |= ZREG_PI;
								}
							} else if (ra[src].ref) {
								ra[src].flags |= ZREG_STORE;
							}
						} else {
							int need_move = 0;

							for (k = 0; k < ssa->cfg.blocks[phi->block].predecessors_count; k++) {
								src = phi->sources[k];
								if (src >= 0) {
									if (ssa->vars[src].definition_phi
									 && ssa->vars[src].definition_phi->pi >= 0
									 && phi->block == ssa->vars[src].definition_phi->block) {
										/* Skip zero-length interval for Pi variable */
										src = ssa->vars[src].definition_phi->sources[0];
									}
									if (ra[i].ref) {
										if (!ra[src].ref) {
											need_move = 1;
										}
									} else if (ra[src].ref) {
										need_move = 1;
									}
								}
							}
							if (need_move) {
								if (ra[i].ref) {
									ra[i].flags |= ZREG_LOAD;
								}
								for (k = 0; k < ssa->cfg.blocks[phi->block].predecessors_count; k++) {
									src = phi->sources[k];
									if (src >= 0) {
										if (ssa->vars[src].definition_phi
										 && ssa->vars[src].definition_phi->pi >= 0
										 && phi->block == ssa->vars[src].definition_phi->block) {
											/* Skip zero-length interval for Pi variable */
											src = ssa->vars[src].definition_phi->sources[0];
										}
										if (ra[src].ref) {
											ra[src].flags |= ZREG_STORE;
										}
									}
								}
							} else {
								ra[i].flags |= ZREG_PHI;
							}
						}
					}
				}
#if 1
				/* Remove useless register allocation */
				for (i = 0; i < ssa->vars_count; i++) {
					if (ra[i].ref &&
					    ((ra[i].flags & ZREG_LOAD) ||
					     ((ra[i].flags & ZREG_STORE) && ssa->vars[i].definition >= 0)) &&
					    ssa->vars[i].use_chain < 0) {
					    bool may_remove = 1;
						zend_ssa_phi *phi = ssa->vars[i].phi_use_chain;

						while (phi) {
							if (ra[phi->ssa_var].ref &&
							    !(ra[phi->ssa_var].flags & ZREG_LOAD)) {
								may_remove = 0;
								break;
							}
							phi = zend_ssa_next_use_phi(ssa, i, phi);
						}
						if (may_remove) {
							ra[i].ref = IR_UNUSED;
						}
					}
				}
				/* Remove intervals used once */
				for (i = 0; i < ssa->vars_count; i++) {
					if (ra[i].ref &&
					    (ra[i].flags & ZREG_LOAD) &&
					    (ra[i].flags & ZREG_STORE) &&
					    (ssa->vars[i].use_chain < 0 ||
					     zend_ssa_next_use(ssa->ops, i, ssa->vars[i].use_chain) < 0)) {
						bool may_remove = 1;
						zend_ssa_phi *phi = ssa->vars[i].phi_use_chain;

						while (phi) {
							if (ra[phi->ssa_var].ref &&
							    !(ra[phi->ssa_var].flags & ZREG_LOAD)) {
								may_remove = 0;
								break;
							}
							phi = zend_ssa_next_use_phi(ssa, i, phi);
						}
						if (may_remove) {
							ra[i].ref = IR_UNUSED;
						}
					}
				}
#endif
			}

			if (JIT_G(debug) & ZEND_JIT_DEBUG_REG_ALLOC) {
				fprintf(stderr, "Live Ranges \"%s\"\n", op_array->function_name ? ZSTR_VAL(op_array->function_name) : "[main]");
				for (i = 0; i < ssa->vars_count; i++) {
					if (ra[i].ref) {
						fprintf(stderr, "#%d.", i);
						uint32_t var_num = ssa->vars[i].var;
						zend_dump_var(op_array, (var_num < op_array->last_var ? IS_CV : 0), var_num);
						if (ra[i].flags & ZREG_LOAD) {
							fprintf(stderr, " load");
						}
						if (ra[i].flags & ZREG_STORE) {
							fprintf(stderr, " store");
						}
						fprintf(stderr, "\n");
					}
				}
				fprintf(stderr, "\n");
			}

	ctx->ra = ra;
}

static int zend_jit_compute_post_order(zend_cfg *cfg, int start, int *post_order)
{
	int count = 0;
	int b, n, *p;
	zend_basic_block *bb;
	zend_worklist worklist;
	ALLOCA_FLAG(use_heap)

	ZEND_WORKLIST_ALLOCA(&worklist, cfg->blocks_count, use_heap);
	zend_worklist_push(&worklist, start);

	while (zend_worklist_len(&worklist) != 0) {
next:
		b = zend_worklist_peek(&worklist);
		bb = &cfg->blocks[b];
		n = bb->successors_count;
		if (n > 0) {
			p = bb->successors;
			do {
				if (cfg->blocks[*p].flags & (ZEND_BB_CATCH|ZEND_BB_FINALLY|ZEND_BB_FINALLY_END)) {
					/* skip */
				} else if (zend_worklist_push(&worklist, *p)) {
					goto next;
				}
				p++;
				n--;
			} while (n > 0);
		}
		zend_worklist_pop(&worklist);
		post_order[count++] = b;
	}
	ZEND_WORKLIST_FREE_ALLOCA(&worklist, use_heap);
	return count;
}
#endif /* ZEND_JIT_IR */

static bool zend_jit_next_is_send_result(const zend_op *opline)
{
	if (opline->result_type == IS_TMP_VAR
	 && (opline+1)->opcode == ZEND_SEND_VAL
	 && (opline+1)->op1_type == IS_TMP_VAR
	 && (opline+1)->op2_type != IS_CONST
	 && (opline+1)->op1.var == opline->result.var) {
#ifndef ZEND_JIT_IR //???
		return 1;
#endif
	}
	return 0;
}

static bool zend_jit_supported_binary_op(uint8_t op, uint32_t op1_info, uint32_t op2_info)
{
	if ((op1_info & MAY_BE_UNDEF) || (op2_info & MAY_BE_UNDEF)) {
		return false;
	}
	switch (op) {
		case ZEND_POW:
		case ZEND_DIV:
			// TODO: check for division by zero ???
			return false;
		case ZEND_ADD:
		case ZEND_SUB:
		case ZEND_MUL:
			return (op1_info & (MAY_BE_LONG|MAY_BE_DOUBLE))
				&& (op2_info & (MAY_BE_LONG|MAY_BE_DOUBLE));
		case ZEND_BW_OR:
		case ZEND_BW_AND:
		case ZEND_BW_XOR:
		case ZEND_SL:
		case ZEND_SR:
		case ZEND_MOD:
			return (op1_info & MAY_BE_LONG) && (op2_info & MAY_BE_LONG);
		case ZEND_CONCAT:
			return (op1_info & MAY_BE_STRING) && (op2_info & MAY_BE_STRING);
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}

static int zend_jit(const zend_op_array *op_array, zend_ssa *ssa, const zend_op *rt_opline)
{
	int b, i, end;
	zend_op *opline;
#ifndef ZEND_JIT_IR
	dasm_State* ctx = NULL;
	zend_lifetime_interval **ra = NULL;
#else
	zend_jit_ctx ctx;
	zend_jit_ctx *jit = &ctx;
	zend_jit_reg_var *ra = NULL;
#endif
	void *handler;
	int call_level = 0;
	void *checkpoint = NULL;
#ifndef ZEND_JIT_IR //???
	bool is_terminated = 1; /* previous basic block is terminated by jump */
#endif
	bool recv_emitted = 0;   /* emitted at least one RECV opcode */
	uint8_t smart_branch_opcode;
	uint32_t target_label, target_label2;
	uint32_t op1_info, op1_def_info, op2_info, res_info, res_use_info;
	zend_jit_addr op1_addr, op1_def_addr, op2_addr, op2_def_addr, res_addr;
#ifndef ZEND_JIT_IR //???
	zend_class_entry *ce;
	bool ce_is_instanceof;
	bool on_this;
#endif

	if (JIT_G(bisect_limit)) {
		jit_bisect_pos++;
		if (jit_bisect_pos >= JIT_G(bisect_limit)) {
			if (jit_bisect_pos == JIT_G(bisect_limit)) {
				fprintf(stderr, "Not JITing %s%s%s in %s:%d and after due to jit_bisect_limit\n",
					op_array->scope ? ZSTR_VAL(op_array->scope->name) : "",
					op_array->scope ? "::" : "",
					op_array->function_name ? ZSTR_VAL(op_array->function_name) : "{main}",
					ZSTR_VAL(op_array->filename), op_array->line_start);
			}
			return FAILURE;
		}
	}

#ifndef ZEND_JIT_IR
	if (JIT_G(opt_flags) & (ZEND_JIT_REG_ALLOC_LOCAL|ZEND_JIT_REG_ALLOC_GLOBAL)) {
		checkpoint = zend_arena_checkpoint(CG(arena));
		ra = zend_jit_allocate_registers(op_array, ssa);
	}

	/* mark hidden branch targets */
	for (b = 0; b < ssa->cfg.blocks_count; b++) {
		if (ssa->cfg.blocks[b].flags & ZEND_BB_REACHABLE &&
		    ssa->cfg.blocks[b].len > 1) {

			opline = op_array->opcodes + ssa->cfg.blocks[b].start + ssa->cfg.blocks[b].len - 1;
			if (opline->opcode == ZEND_DO_FCALL &&
			    (opline-1)->opcode == ZEND_NEW) {
				ssa->cfg.blocks[ssa->cfg.blocks[b].successors[0]].flags |= ZEND_BB_TARGET;
			}
		}
	}

	dasm_init(&ctx, DASM_MAXSECTION);
	dasm_setupglobal(&ctx, dasm_labels, zend_lb_MAX);
	dasm_setup(&ctx, dasm_actions);

	dasm_growpc(&ctx, ssa->cfg.blocks_count * 2 + 1);

	zend_jit_align_func(&ctx);
	for (b = 0; b < ssa->cfg.blocks_count; b++) {
		if ((ssa->cfg.blocks[b].flags & ZEND_BB_REACHABLE) == 0) {
			continue;
		}
//#ifndef CONTEXT_THREADED_JIT
		if (ssa->cfg.blocks[b].flags & ZEND_BB_ENTRY) {
			if (ssa->cfg.blocks[b].flags & ZEND_BB_TARGET) {
				/* pass */
			} else if (JIT_G(opt_level) < ZEND_JIT_LEVEL_INLINE &&
			           ssa->cfg.blocks[b].len == 1 &&
			           (ssa->cfg.blocks[b].flags & ZEND_BB_EXIT) &&
			           op_array->opcodes[ssa->cfg.blocks[b].start].opcode != ZEND_JMP) {
				/* don't generate code for BB with single opcode */
				continue;
			}
			if (ssa->cfg.blocks[b].flags & ZEND_BB_FOLLOW) {
				if (!is_terminated) {
					zend_jit_jmp(&ctx, b);
				}
			}
			zend_jit_label(&ctx, ssa->cfg.blocks_count + b);
			zend_jit_prologue(&ctx);
		} else
//#endif
		if (ssa->cfg.blocks[b].flags & (ZEND_BB_START|ZEND_BB_RECV_ENTRY)) {
			opline = op_array->opcodes + ssa->cfg.blocks[b].start;
			if (ssa->cfg.flags & ZEND_CFG_RECV_ENTRY) {
				if (opline->opcode == ZEND_RECV_INIT) {
					if (opline == op_array->opcodes ||
					    (opline-1)->opcode != ZEND_RECV_INIT) {
						if (recv_emitted) {
							zend_jit_jmp(&ctx, b);
						}
						zend_jit_label(&ctx, ssa->cfg.blocks_count + b);
						for (i = 1; (opline+i)->opcode == ZEND_RECV_INIT; i++) {
							zend_jit_label(&ctx, ssa->cfg.blocks_count + b + i);
						}
						zend_jit_prologue(&ctx);
					}
					recv_emitted = 1;
				} else if (opline->opcode == ZEND_RECV) {
					if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
						/* skip */
						continue;
					} else if (recv_emitted) {
						zend_jit_jmp(&ctx, b);
						zend_jit_label(&ctx, ssa->cfg.blocks_count + b);
						zend_jit_prologue(&ctx);
					} else {
						zend_arg_info *arg_info;

						if (opline->op1.num <= op_array->num_args) {
							arg_info = &op_array->arg_info[opline->op1.num - 1];
						} else if (op_array->fn_flags & ZEND_ACC_VARIADIC) {
							arg_info = &op_array->arg_info[op_array->num_args];
						} else {
							/* skip */
							continue;
						}
						if (!ZEND_TYPE_IS_SET(arg_info->type)) {
							/* skip */
							continue;
						}
						zend_jit_label(&ctx, ssa->cfg.blocks_count + b);
						zend_jit_prologue(&ctx);
						recv_emitted = 1;
					}
				} else {
					if (recv_emitted) {
						zend_jit_jmp(&ctx, b);
					} else if (JIT_G(opt_level) < ZEND_JIT_LEVEL_INLINE &&
					           ssa->cfg.blocks[b].len == 1 &&
					           (ssa->cfg.blocks[b].flags & ZEND_BB_EXIT)) {
						/* don't generate code for BB with single opcode */
						dasm_free(&ctx);

						if (JIT_G(opt_flags) & (ZEND_JIT_REG_ALLOC_LOCAL|ZEND_JIT_REG_ALLOC_GLOBAL)) {
							zend_arena_release(&CG(arena), checkpoint);
						}
						return SUCCESS;
					}
					zend_jit_label(&ctx, ssa->cfg.blocks_count + b);
					zend_jit_prologue(&ctx);
					recv_emitted = 1;
				}
			} else if (JIT_G(opt_level) < ZEND_JIT_LEVEL_INLINE &&
			           ssa->cfg.blocks[b].len == 1 &&
			           (ssa->cfg.blocks[b].flags & ZEND_BB_EXIT)) {
				/* don't generate code for BB with single opcode */
				dasm_free(&ctx);

				if (JIT_G(opt_flags) & (ZEND_JIT_REG_ALLOC_LOCAL|ZEND_JIT_REG_ALLOC_GLOBAL)) {
					zend_arena_release(&CG(arena), checkpoint);
				}
				return SUCCESS;
			} else {
				zend_jit_label(&ctx, ssa->cfg.blocks_count + b);
				zend_jit_prologue(&ctx);
			}
		}

		is_terminated = 0;

		zend_jit_label(&ctx, b);
#else /* ZEND_JIT_IR */

	zend_jit_start(&ctx, op_array, ssa);
	if (JIT_G(opt_flags) & (ZEND_JIT_REG_ALLOC_LOCAL|ZEND_JIT_REG_ALLOC_GLOBAL)) {
		checkpoint = zend_arena_checkpoint(CG(arena));
		zend_jit_allocate_registers(&ctx, op_array, ssa);
		ra = ctx.ra;
	}

	/* Process blocks in Reverse Post Order */
	int *sorted_blocks = alloca(sizeof(int) * ssa->cfg.blocks_count);
	int n = zend_jit_compute_post_order(&ssa->cfg, 0, sorted_blocks);

	while (n > 0) {
		b = sorted_blocks[--n];
		if ((ssa->cfg.blocks[b].flags & ZEND_BB_REACHABLE) == 0) {
			continue;
		}

		if (rt_opline
		 && (ssa->cfg.blocks[b].flags & (ZEND_BB_START|ZEND_BB_RECV_ENTRY)) == 0
		 && rt_opline == op_array->opcodes + ssa->cfg.blocks[b].start) {
			ssa->cfg.blocks[b].flags |= ZEND_BB_ENTRY;
		}

		if (ssa->cfg.blocks[b].flags & ZEND_BB_ENTRY) {
			if (ssa->cfg.blocks[b].flags & ZEND_BB_TARGET) {
				/* pass */
			} else if (JIT_G(opt_level) < ZEND_JIT_LEVEL_INLINE &&
			           ssa->cfg.blocks[b].len == 1 &&
			           (ssa->cfg.blocks[b].flags & ZEND_BB_EXIT) &&
			           op_array->opcodes[ssa->cfg.blocks[b].start].opcode != ZEND_JMP) {
				/* don't generate code for BB with single opcode */
				continue;
			}
			zend_jit_entry(&ctx, ssa->cfg.blocks[b].start);
		} else if (ssa->cfg.blocks[b].flags & (ZEND_BB_START|ZEND_BB_RECV_ENTRY)) {
			opline = op_array->opcodes + ssa->cfg.blocks[b].start;
			if (ssa->cfg.flags & ZEND_CFG_RECV_ENTRY) {
				if (opline->opcode == ZEND_RECV_INIT) {
					if (opline != op_array->opcodes &&
					    (opline-1)->opcode != ZEND_RECV_INIT) {
						zend_jit_entry(&ctx, ssa->cfg.blocks[b].start);
					}
					recv_emitted = 1;
				} else if (opline->opcode == ZEND_RECV) {
					if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
						/* skip */
						zend_jit_bb_start(&ctx, b);
						zend_jit_bb_end(&ctx, b);
						continue;
					} else if (recv_emitted) {
						zend_jit_entry(&ctx, ssa->cfg.blocks[b].start);
					} else {
						zend_arg_info *arg_info;

						if (opline->op1.num <= op_array->num_args) {
							arg_info = &op_array->arg_info[opline->op1.num - 1];
						} else if (op_array->fn_flags & ZEND_ACC_VARIADIC) {
							arg_info = &op_array->arg_info[op_array->num_args];
						} else {
							/* skip */
							zend_jit_bb_start(&ctx, b);
							zend_jit_bb_end(&ctx, b);
							continue;
						}
						if (!ZEND_TYPE_IS_SET(arg_info->type)) {
							/* skip */
							zend_jit_bb_start(&ctx, b);
							zend_jit_bb_end(&ctx, b);
							continue;
						}
						recv_emitted = 1;
					}
				} else {
					if (recv_emitted) {
						zend_jit_entry(&ctx, ssa->cfg.blocks[b].start);
					} else if (JIT_G(opt_level) < ZEND_JIT_LEVEL_INLINE &&
					           ssa->cfg.blocks[b].len == 1 &&
					           (ssa->cfg.blocks[b].flags & ZEND_BB_EXIT)) {
						/* don't generate code for BB with single opcode */
						zend_jit_free(&ctx);

						if (JIT_G(opt_flags) & (ZEND_JIT_REG_ALLOC_LOCAL|ZEND_JIT_REG_ALLOC_GLOBAL)) {
							zend_arena_release(&CG(arena), checkpoint);
						}
						return SUCCESS;
					}
				}

			} else if (JIT_G(opt_level) < ZEND_JIT_LEVEL_INLINE &&
			           ssa->cfg.blocks[b].len == 1 &&
			           (ssa->cfg.blocks[b].flags & ZEND_BB_EXIT)) {
				/* don't generate code for BB with single opcode */
				zend_jit_free(&ctx);

				if (JIT_G(opt_flags) & (ZEND_JIT_REG_ALLOC_LOCAL|ZEND_JIT_REG_ALLOC_GLOBAL)) {
					zend_arena_release(&CG(arena), checkpoint);
				}
				return SUCCESS;
			}
		}

		zend_jit_bb_start(&ctx, b);

		if ((JIT_G(opt_flags) & ZEND_JIT_REG_ALLOC_GLOBAL) && ctx.ra) {
			zend_ssa_phi *phi = ssa->blocks[b].phis;

			/* First try to insert IR Phi */
			while (phi) {
				zend_jit_reg_var *ival = &ctx.ra[phi->ssa_var];

				if (ival->ref) {
					if (ival->flags & ZREG_PI) {
						zend_jit_gen_pi(jit, phi);
					} else if (ival->flags & ZREG_PHI) {
						zend_jit_gen_phi(jit, phi);
					}
				}
				phi = phi->next;
			}
		}
#endif /* ZEND_JIT_IR */

		if (JIT_G(opt_level) < ZEND_JIT_LEVEL_INLINE) {
			if ((ssa->cfg.blocks[b].flags & ZEND_BB_FOLLOW)
			  && ssa->cfg.blocks[b].start != 0
			  && (op_array->opcodes[ssa->cfg.blocks[b].start - 1].opcode == ZEND_NOP
			   || op_array->opcodes[ssa->cfg.blocks[b].start - 1].opcode == ZEND_SWITCH_LONG
			   || op_array->opcodes[ssa->cfg.blocks[b].start - 1].opcode == ZEND_SWITCH_STRING
			   || op_array->opcodes[ssa->cfg.blocks[b].start - 1].opcode == ZEND_MATCH)) {
#ifndef ZEND_JIT_IR
				zend_jit_reset_last_valid_opline();
				if (!zend_jit_set_ip(&ctx, op_array->opcodes + ssa->cfg.blocks[b].start)) {
					goto jit_failure;
				}
#else
				zend_jit_reset_last_valid_opline(&ctx);
#endif
			} else {
#ifndef ZEND_JIT_IR
				zend_jit_set_last_valid_opline(op_array->opcodes + ssa->cfg.blocks[b].start);
#else
				zend_jit_set_last_valid_opline(&ctx, op_array->opcodes + ssa->cfg.blocks[b].start);
#endif
			}
		} else if (ssa->cfg.blocks[b].flags & ZEND_BB_TARGET) {
#ifndef ZEND_JIT_IR
			zend_jit_reset_last_valid_opline();
#else
			zend_jit_reset_last_valid_opline(&ctx);
#endif
		} else if (ssa->cfg.blocks[b].flags & (ZEND_BB_START|ZEND_BB_RECV_ENTRY|ZEND_BB_ENTRY)) {
#ifndef ZEND_JIT_IR
			zend_jit_set_last_valid_opline(op_array->opcodes + ssa->cfg.blocks[b].start);
#else
			zend_jit_set_last_valid_opline(&ctx, op_array->opcodes + ssa->cfg.blocks[b].start);
#endif
		}
		if (ssa->cfg.blocks[b].flags & ZEND_BB_LOOP_HEADER) {
			if (!zend_jit_check_timeout(&ctx, op_array->opcodes + ssa->cfg.blocks[b].start, NULL)) {
				goto jit_failure;
			}
		}
		if (!ssa->cfg.blocks[b].len) {
#ifdef ZEND_JIT_IR
			zend_jit_bb_end(&ctx, b);
#endif
			continue;
		}
		if ((JIT_G(opt_flags) & ZEND_JIT_REG_ALLOC_GLOBAL) && ra) {
			zend_ssa_phi *phi = ssa->blocks[b].phis;

			while (phi) {
#ifndef ZEND_JIT_IR
				zend_lifetime_interval *ival = ra[phi->ssa_var];

				if (ival) {
					if (ival->flags & ZREG_LOAD) {
						ZEND_ASSERT(ival->reg != ZREG_NONE);

						if (!zend_jit_load_var(&ctx, ssa->var_info[phi->ssa_var].type, ssa->vars[phi->ssa_var].var, ival->reg)) {
							goto jit_failure;
						}
					} else if (ival->flags & ZREG_STORE) {
						ZEND_ASSERT(ival->reg != ZREG_NONE);

						if (!zend_jit_store_var(&ctx, ssa->var_info[phi->ssa_var].type, ssa->vars[phi->ssa_var].var, ival->reg, 1)) {
							goto jit_failure;
						}
					}
				}
#else
				zend_jit_reg_var *ival = &ra[phi->ssa_var];

				if (ival->ref) {
					if (ival->flags & ZREG_LOAD) {
						ZEND_ASSERT(ival->ref == IR_NULL);

						if (!zend_jit_load_var(&ctx, ssa->var_info[phi->ssa_var].type, ssa->vars[phi->ssa_var].var, phi->ssa_var)) {
							goto jit_failure;
						}
					} else if (ival->flags & ZREG_STORE) {
						ZEND_ASSERT(ival->ref != IR_NULL);

						if (!zend_jit_store_var(&ctx, ssa->var_info[phi->ssa_var].type, ssa->vars[phi->ssa_var].var, phi->ssa_var, 1)) {
							goto jit_failure;
						}
					}
				}
#endif

				phi = phi->next;
			}
		}
		end = ssa->cfg.blocks[b].start + ssa->cfg.blocks[b].len - 1;
		for (i = ssa->cfg.blocks[b].start; i <= end; i++) {
			zend_ssa_op *ssa_op = ssa->ops ? &ssa->ops[i] : NULL;
			opline = op_array->opcodes + i;
			switch (opline->opcode) {
				case ZEND_INIT_FCALL:
				case ZEND_INIT_FCALL_BY_NAME:
				case ZEND_INIT_NS_FCALL_BY_NAME:
				case ZEND_INIT_METHOD_CALL:
				case ZEND_INIT_DYNAMIC_CALL:
				case ZEND_INIT_STATIC_METHOD_CALL:
				case ZEND_INIT_USER_CALL:
				case ZEND_NEW:
					call_level++;
			}

			if (JIT_G(opt_level) >= ZEND_JIT_LEVEL_INLINE) {
				switch (opline->opcode) {
					case ZEND_PRE_INC:
					case ZEND_PRE_DEC:
					case ZEND_POST_INC:
					case ZEND_POST_DEC:
						if (opline->op1_type != IS_CV) {
							break;
						}
						op1_info = OP1_INFO();
						if (!(op1_info & MAY_BE_LONG)) {
							break;
						}
						if (opline->result_type != IS_UNUSED) {
							res_use_info = -1;

							if (opline->result_type == IS_CV
							 && ssa->vars
							 && ssa_op->result_use >= 0
							 && !ssa->vars[ssa_op->result_use].no_val) {
								zend_jit_addr res_use_addr = RES_USE_REG_ADDR();

								if (Z_MODE(res_use_addr) != IS_REG
								 || Z_LOAD(res_use_addr)
								 || Z_STORE(res_use_addr)) {
									res_use_info = RES_USE_INFO();
								}
							}
							res_info = RES_INFO();
							res_addr = RES_REG_ADDR();
						} else {
							res_use_info = -1;
							res_info = -1;
							res_addr = 0;
						}
						op1_def_info = OP1_DEF_INFO();
						if (!zend_jit_inc_dec(&ctx, opline,
								op1_info, OP1_REG_ADDR(),
								op1_def_info, OP1_DEF_REG_ADDR(),
								res_use_info, res_info,
								res_addr,
								(op1_info & MAY_BE_LONG) && (op1_def_info & MAY_BE_DOUBLE) && zend_may_overflow(opline, ssa_op, op_array, ssa),
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_BW_OR:
					case ZEND_BW_AND:
					case ZEND_BW_XOR:
					case ZEND_SL:
					case ZEND_SR:
					case ZEND_MOD:
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
#ifndef ZEND_JIT_IR
						if ((op1_info & MAY_BE_UNDEF) || (op2_info & MAY_BE_UNDEF)) {
							break;
						}
#endif
						if (!(op1_info & MAY_BE_LONG)
						 || !(op2_info & MAY_BE_LONG)) {
							break;
						}
						res_addr = RES_REG_ADDR();
						if (Z_MODE(res_addr) != IS_REG
						 && (i + 1) <= end
						 && zend_jit_next_is_send_result(opline)) {
#ifndef ZEND_JIT_IR //???
							i++;
							res_use_info = -1;
							res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_RX, (opline+1)->result.var);
							if (!zend_jit_reuse_ip(&ctx)) {
								goto jit_failure;
							}
#endif
						} else {
							res_use_info = -1;

							if (opline->result_type == IS_CV
							 && ssa->vars
							 && ssa_op->result_use >= 0
							 && !ssa->vars[ssa_op->result_use].no_val) {
								zend_jit_addr res_use_addr = RES_USE_REG_ADDR();

								if (Z_MODE(res_use_addr) != IS_REG
								 || Z_LOAD(res_use_addr)
								 || Z_STORE(res_use_addr)) {
									res_use_info = RES_USE_INFO();
								}
							}
						}
						if (!zend_jit_long_math(&ctx, opline,
								op1_info, OP1_RANGE(), OP1_REG_ADDR(),
								op2_info, OP2_RANGE(), OP2_REG_ADDR(),
								res_use_info, RES_INFO(), res_addr,
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ADD:
					case ZEND_SUB:
					case ZEND_MUL:
//					case ZEND_DIV: // TODO: check for division by zero ???
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
						if ((op1_info & MAY_BE_UNDEF) || (op2_info & MAY_BE_UNDEF)) {
							break;
						}
						if (opline->opcode == ZEND_ADD &&
						    (op1_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_ARRAY &&
						    (op2_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_ARRAY) {
							/* pass */
						} else if (!(op1_info & (MAY_BE_LONG|MAY_BE_DOUBLE)) ||
						    !(op2_info & (MAY_BE_LONG|MAY_BE_DOUBLE))) {
							break;
						}
						res_addr = RES_REG_ADDR();
						if (Z_MODE(res_addr) != IS_REG
						 && (i + 1) <= end
						 && zend_jit_next_is_send_result(opline)) {
#ifndef ZEND_JIT_IR //???
							i++;
							res_use_info = -1;
							res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_RX, (opline+1)->result.var);
							if (!zend_jit_reuse_ip(&ctx)) {
								goto jit_failure;
							}
#endif
						} else {
							res_use_info = -1;

							if (opline->result_type == IS_CV
							 && ssa->vars
							 && ssa_op->result_use >= 0
							 && !ssa->vars[ssa_op->result_use].no_val) {
								zend_jit_addr res_use_addr = RES_USE_REG_ADDR();

								if (Z_MODE(res_use_addr) != IS_REG
								 || Z_LOAD(res_use_addr)
								 || Z_STORE(res_use_addr)) {
									res_use_info = RES_USE_INFO();
								}
							}
						}
						res_info = RES_INFO();
						if (opline->opcode == ZEND_ADD &&
						    (op1_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_ARRAY &&
						    (op2_info & (MAY_BE_ANY|MAY_BE_UNDEF)) == MAY_BE_ARRAY) {
							if (!zend_jit_add_arrays(&ctx, opline, op1_info, OP1_REG_ADDR(), op2_info, OP2_REG_ADDR(), res_addr)) {
								goto jit_failure;
							}
						} else {
							if (!zend_jit_math(&ctx, opline,
									op1_info, OP1_REG_ADDR(),
									op2_info, OP2_REG_ADDR(),
									res_use_info, res_info, res_addr,
									(op1_info & MAY_BE_LONG) && (op2_info & MAY_BE_LONG) && (res_info & MAY_BE_DOUBLE) && zend_may_overflow(opline, ssa_op, op_array, ssa),
									zend_may_throw(opline, ssa_op, op_array, ssa))) {
								goto jit_failure;
							}
						}
						goto done;
					case ZEND_CONCAT:
					case ZEND_FAST_CONCAT:
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
						if ((op1_info & MAY_BE_UNDEF) || (op2_info & MAY_BE_UNDEF)) {
							break;
						}
						if (!(op1_info & MAY_BE_STRING) ||
						    !(op2_info & MAY_BE_STRING)) {
							break;
						}
						res_addr = RES_REG_ADDR();
						if ((i + 1) <= end
						 && zend_jit_next_is_send_result(opline)) {
#ifndef ZEND_JIT_IR //???
							i++;
							res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_RX, (opline+1)->result.var);
							if (!zend_jit_reuse_ip(&ctx)) {
								goto jit_failure;
							}
#endif
						}
						if (!zend_jit_concat(&ctx, opline,
								op1_info, op2_info, res_addr,
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN_OP:
						if (opline->op1_type != IS_CV || opline->result_type != IS_UNUSED) {
							break;
						}
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
						if (!zend_jit_supported_binary_op(
								opline->extended_value, op1_info, op2_info)) {
							break;
						}
						op1_def_info = OP1_DEF_INFO();
						if (!zend_jit_assign_op(&ctx, opline,
								op1_info, op1_def_info, OP1_RANGE(),
								op2_info, OP2_RANGE(),
								(op1_info & MAY_BE_LONG) && (op2_info & MAY_BE_LONG) && (op1_def_info & MAY_BE_DOUBLE) && zend_may_overflow(opline, ssa_op, op_array, ssa),
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
#ifndef ZEND_JIT_IR //???
					case ZEND_ASSIGN_DIM_OP:
						if (opline->op1_type != IS_CV || opline->result_type != IS_UNUSED) {
							break;
						}
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						if (!zend_jit_supported_binary_op(
								opline->extended_value, MAY_BE_ANY, OP1_DATA_INFO())) {
							break;
						}
						if (!zend_jit_assign_dim_op(&ctx, opline,
								OP1_INFO(), OP1_DEF_INFO(), OP1_REG_ADDR(), OP2_INFO(),
								OP1_DATA_INFO(), OP1_DATA_RANGE(), IS_UNKNOWN,
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN_DIM:
						if (opline->op1_type != IS_CV) {
							break;
						}
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						if (!zend_jit_assign_dim(&ctx, opline,
								OP1_INFO(), OP1_REG_ADDR(), OP2_INFO(), OP1_DATA_INFO(), IS_UNKNOWN,
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_PRE_INC_OBJ:
					case ZEND_PRE_DEC_OBJ:
					case ZEND_POST_INC_OBJ:
					case ZEND_POST_DEC_OBJ:
						if (opline->op2_type != IS_CONST
						 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING
						 || Z_STRVAL_P(RT_CONSTANT(opline, opline->op2))[0] == '\0') {
							break;
						}
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						ce = NULL;
						ce_is_instanceof = 0;
						on_this = 0;
						if (opline->op1_type == IS_UNUSED) {
							op1_info = MAY_BE_OBJECT|MAY_BE_RC1|MAY_BE_RCN;
							ce = op_array->scope;
							ce_is_instanceof = (ce->ce_flags & ZEND_ACC_FINAL) != 0;
							op1_addr = 0;
							on_this = 1;
						} else {
							op1_info = OP1_INFO();
							if (!(op1_info & MAY_BE_OBJECT)) {
								break;
							}
							op1_addr = OP1_REG_ADDR();
							if (ssa->var_info && ssa->ops) {
								zend_ssa_op *ssa_op = &ssa->ops[opline - op_array->opcodes];
								if (ssa_op->op1_use >= 0) {
									zend_ssa_var_info *op1_ssa = ssa->var_info + ssa_op->op1_use;
									if (op1_ssa->ce && !op1_ssa->ce->create_object) {
										ce = op1_ssa->ce;
										ce_is_instanceof = op1_ssa->is_instanceof;
									}
								}
							}
						}
						if (!zend_jit_incdec_obj(&ctx, opline, op_array, ssa, ssa_op,
								op1_info, op1_addr,
								0, ce, ce_is_instanceof, on_this, 0, NULL, IS_UNKNOWN)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN_OBJ_OP:
						if (opline->result_type != IS_UNUSED) {
							break;
						}
						if (opline->op2_type != IS_CONST
						 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING
						 || Z_STRVAL_P(RT_CONSTANT(opline, opline->op2))[0] == '\0') {
							break;
						}
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						if (!zend_jit_supported_binary_op(
								opline->extended_value, MAY_BE_ANY, OP1_DATA_INFO())) {
							break;
						}
						ce = NULL;
						ce_is_instanceof = 0;
						on_this = 0;
						if (opline->op1_type == IS_UNUSED) {
							op1_info = MAY_BE_OBJECT|MAY_BE_RC1|MAY_BE_RCN;
							ce = op_array->scope;
							ce_is_instanceof = (ce->ce_flags & ZEND_ACC_FINAL) != 0;
							op1_addr = 0;
							on_this = 1;
						} else {
							op1_info = OP1_INFO();
							if (!(op1_info & MAY_BE_OBJECT)) {
								break;
							}
							op1_addr = OP1_REG_ADDR();
							if (ssa->var_info && ssa->ops) {
								zend_ssa_op *ssa_op = &ssa->ops[opline - op_array->opcodes];
								if (ssa_op->op1_use >= 0) {
									zend_ssa_var_info *op1_ssa = ssa->var_info + ssa_op->op1_use;
									if (op1_ssa->ce && !op1_ssa->ce->create_object) {
										ce = op1_ssa->ce;
										ce_is_instanceof = op1_ssa->is_instanceof;
									}
								}
							}
						}
						if (!zend_jit_assign_obj_op(&ctx, opline, op_array, ssa, ssa_op,
								op1_info, op1_addr, OP1_DATA_INFO(), OP1_DATA_RANGE(),
								0, ce, ce_is_instanceof, on_this, 0, NULL, IS_UNKNOWN)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN_OBJ:
						if (opline->op2_type != IS_CONST
						 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING
						 || Z_STRVAL_P(RT_CONSTANT(opline, opline->op2))[0] == '\0') {
							break;
						}
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						ce = NULL;
						ce_is_instanceof = 0;
						on_this = 0;
						if (opline->op1_type == IS_UNUSED) {
							op1_info = MAY_BE_OBJECT|MAY_BE_RC1|MAY_BE_RCN;
							ce = op_array->scope;
							ce_is_instanceof = (ce->ce_flags & ZEND_ACC_FINAL) != 0;
							op1_addr = 0;
							on_this = 1;
						} else {
							op1_info = OP1_INFO();
							if (!(op1_info & MAY_BE_OBJECT)) {
								break;
							}
							op1_addr = OP1_REG_ADDR();
							if (ssa->var_info && ssa->ops) {
								zend_ssa_op *ssa_op = &ssa->ops[opline - op_array->opcodes];
								if (ssa_op->op1_use >= 0) {
									zend_ssa_var_info *op1_ssa = ssa->var_info + ssa_op->op1_use;
									if (op1_ssa->ce && !op1_ssa->ce->create_object) {
										ce = op1_ssa->ce;
										ce_is_instanceof = op1_ssa->is_instanceof;
									}
								}
							}
						}
						if (!zend_jit_assign_obj(&ctx, opline, op_array, ssa, ssa_op,
								op1_info, op1_addr, OP1_DATA_INFO(),
								0, ce, ce_is_instanceof, on_this, 0, NULL, IS_UNKNOWN,
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
#endif
					case ZEND_ASSIGN:
						if (opline->op1_type != IS_CV) {
							break;
						}
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						op2_addr = OP2_REG_ADDR();
						if (ra
						 && ssa->ops[opline - op_array->opcodes].op2_def >= 0
						 && !ssa->vars[ssa->ops[opline - op_array->opcodes].op2_def].no_val) {
							op2_def_addr = OP2_DEF_REG_ADDR();
						} else {
							op2_def_addr = op2_addr;
						}
						op1_info = OP1_INFO();
						if (ra && ssa->vars[ssa_op->op1_use].no_val) {
							op1_info |= MAY_BE_UNDEF; // requres type assignment
						}
						if (opline->result_type == IS_UNUSED) {
							res_addr = 0;
							res_info = -1;
						} else {
							res_addr = RES_REG_ADDR();
							res_info = RES_INFO();
							if (Z_MODE(res_addr) != IS_REG
							 && (i + 1) <= end
							 && zend_jit_next_is_send_result(opline)
							 && (!(op1_info & MAY_HAVE_DTOR) || !(op1_info & MAY_BE_RC1))) {
#ifndef ZEND_JIT_IR //???
								i++;
								res_addr = ZEND_ADDR_MEM_ZVAL(ZREG_RX, (opline+1)->result.var);
								if (!zend_jit_reuse_ip(&ctx)) {
									goto jit_failure;
								}
#endif
							}
						}
						if (!zend_jit_assign(&ctx, opline,
								op1_info, OP1_REG_ADDR(),
								OP1_DEF_INFO(), OP1_DEF_REG_ADDR(),
								OP2_INFO(), op2_addr, op2_def_addr,
								res_info, res_addr,
#ifdef ZEND_JIT_IR
								0,
#endif
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_QM_ASSIGN:
						op1_addr = OP1_REG_ADDR();
						if (ra
						 && ssa->ops[opline - op_array->opcodes].op1_def >= 0
						 && !ssa->vars[ssa->ops[opline - op_array->opcodes].op1_def].no_val) {
							op1_def_addr = OP1_DEF_REG_ADDR();
						} else {
							op1_def_addr = op1_addr;
						}
						if (!zend_jit_qm_assign(&ctx, opline,
								OP1_INFO(), op1_addr, op1_def_addr,
								-1, RES_INFO(), RES_REG_ADDR())) {
							goto jit_failure;
						}
						goto done;
					case ZEND_INIT_FCALL:
					case ZEND_INIT_FCALL_BY_NAME:
					case ZEND_INIT_NS_FCALL_BY_NAME:
						if (!zend_jit_init_fcall(&ctx, opline, b, op_array, ssa, ssa_op, call_level, NULL, 0)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SEND_VAL:
					case ZEND_SEND_VAL_EX:
						if (opline->op2_type == IS_CONST) {
							/* Named parameters not supported in JIT (yet) */
							break;
						}
						if (opline->opcode == ZEND_SEND_VAL_EX
						 && opline->op2.num > MAX_ARG_FLAG_NUM) {
							break;
						}
						if (!zend_jit_send_val(&ctx, opline,
								OP1_INFO(), OP1_REG_ADDR())) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SEND_REF:
						if (opline->op2_type == IS_CONST) {
							/* Named parameters not supported in JIT (yet) */
							break;
						}
						if (!zend_jit_send_ref(&ctx, opline, op_array,
								OP1_INFO(), 0)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SEND_VAR:
					case ZEND_SEND_VAR_EX:
					case ZEND_SEND_VAR_NO_REF:
					case ZEND_SEND_VAR_NO_REF_EX:
					case ZEND_SEND_FUNC_ARG:
						if (opline->op2_type == IS_CONST) {
							/* Named parameters not supported in JIT (yet) */
							break;
						}
						if ((opline->opcode == ZEND_SEND_VAR_EX
						  || opline->opcode == ZEND_SEND_VAR_NO_REF_EX)
						 && opline->op2.num > MAX_ARG_FLAG_NUM) {
							break;
						}
						op1_addr = OP1_REG_ADDR();
						if (ra
						 && ssa->ops[opline - op_array->opcodes].op1_def >= 0
						 && !ssa->vars[ssa->ops[opline - op_array->opcodes].op1_def].no_val) {
							op1_def_addr = OP1_DEF_REG_ADDR();
						} else {
							op1_def_addr = op1_addr;
						}
						if (!zend_jit_send_var(&ctx, opline, op_array,
								OP1_INFO(), op1_addr, op1_def_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_CHECK_FUNC_ARG:
						if (opline->op2_type == IS_CONST) {
							/* Named parameters not supported in JIT (yet) */
							break;
						}
						if (opline->op2.num > MAX_ARG_FLAG_NUM) {
							break;
						}
						if (!zend_jit_check_func_arg(&ctx, opline)) {
							goto jit_failure;
						}
						goto done;
#ifndef ZEND_JIT_IR //???
					case ZEND_CHECK_UNDEF_ARGS:
						if (!zend_jit_check_undef_args(&ctx, opline)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_DO_UCALL:
						is_terminated = 1;
						ZEND_FALLTHROUGH;
					case ZEND_DO_ICALL:
					case ZEND_DO_FCALL_BY_NAME:
					case ZEND_DO_FCALL:
						if (!zend_jit_do_fcall(&ctx, opline, op_array, ssa, call_level, b + 1, NULL)) {
							goto jit_failure;
						}
						goto done;
#endif
					case ZEND_IS_EQUAL:
					case ZEND_IS_NOT_EQUAL:
					case ZEND_IS_SMALLER:
					case ZEND_IS_SMALLER_OR_EQUAL:
					case ZEND_CASE: {
						res_addr = RES_REG_ADDR();
						if ((opline->result_type & IS_TMP_VAR)
						 && (i + 1) <= end
						 && ((opline+1)->opcode == ZEND_JMPZ
						  || (opline+1)->opcode == ZEND_JMPNZ
						  || (opline+1)->opcode == ZEND_JMPZ_EX
						  || (opline+1)->opcode == ZEND_JMPNZ_EX)
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							i++;
							smart_branch_opcode = (opline+1)->opcode;
							target_label = ssa->cfg.blocks[b].successors[0];
							target_label2 = ssa->cfg.blocks[b].successors[1];
							/* For EX variant write into the result of EX opcode. */
							if ((opline+1)->opcode == ZEND_JMPZ_EX
									|| (opline+1)->opcode == ZEND_JMPNZ_EX) {
								res_addr = OP_REG_ADDR(opline + 1, result_type, result, result_def);
							}
						} else {
							smart_branch_opcode = 0;
							target_label = target_label2 = (uint32_t)-1;
						}
						if (!zend_jit_cmp(&ctx, opline,
								OP1_INFO(), OP1_RANGE(), OP1_REG_ADDR(),
								OP2_INFO(), OP2_RANGE(), OP2_REG_ADDR(),
								res_addr,
								zend_may_throw(opline, ssa_op, op_array, ssa),
								smart_branch_opcode, target_label, target_label2,
								NULL, 0)) {
							goto jit_failure;
						}
						goto done;
					}
					case ZEND_IS_IDENTICAL:
					case ZEND_IS_NOT_IDENTICAL:
					case ZEND_CASE_STRICT:
						if ((opline->result_type & IS_TMP_VAR)
						 && (i + 1) <= end
						 && ((opline+1)->opcode == ZEND_JMPZ
#ifdef ZEND_JIT_IR
						  || (opline+1)->opcode == ZEND_JMPZ_EX
						  || (opline+1)->opcode == ZEND_JMPNZ_EX
#endif
						  || (opline+1)->opcode == ZEND_JMPNZ)
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							i++;
							smart_branch_opcode = (opline+1)->opcode;
							target_label = ssa->cfg.blocks[b].successors[0];
							target_label2 = ssa->cfg.blocks[b].successors[1];
						} else {
							smart_branch_opcode = 0;
							target_label = target_label2 = (uint32_t)-1;
						}
						if (!zend_jit_identical(&ctx, opline,
								OP1_INFO(), OP1_RANGE(), OP1_REG_ADDR(),
								OP2_INFO(), OP2_RANGE(), OP2_REG_ADDR(),
								RES_REG_ADDR(),
								zend_may_throw(opline, ssa_op, op_array, ssa),
								smart_branch_opcode, target_label, target_label2,
								NULL, 0)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_DEFINED:
						if ((opline->result_type & IS_TMP_VAR)
						 && (i + 1) <= end
						 && ((opline+1)->opcode == ZEND_JMPZ
						  || (opline+1)->opcode == ZEND_JMPNZ)
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							i++;
							smart_branch_opcode = (opline+1)->opcode;
							target_label = ssa->cfg.blocks[b].successors[0];
							target_label2 = ssa->cfg.blocks[b].successors[1];
						} else {
							smart_branch_opcode = 0;
							target_label = target_label2 = (uint32_t)-1;
						}
						if (!zend_jit_defined(&ctx, opline, smart_branch_opcode, target_label, target_label2, NULL)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_TYPE_CHECK:
						if (opline->extended_value == MAY_BE_RESOURCE) {
							// TODO: support for is_resource() ???
							break;
						}
						if ((opline->result_type & IS_TMP_VAR)
						 && (i + 1) <= end
						 && ((opline+1)->opcode == ZEND_JMPZ
						  || (opline+1)->opcode == ZEND_JMPNZ)
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							i++;
							smart_branch_opcode = (opline+1)->opcode;
							target_label = ssa->cfg.blocks[b].successors[0];
							target_label2 = ssa->cfg.blocks[b].successors[1];
						} else {
							smart_branch_opcode = 0;
							target_label = target_label2 = (uint32_t)-1;
						}
						if (!zend_jit_type_check(&ctx, opline, OP1_INFO(), smart_branch_opcode, target_label, target_label2, NULL)) {
							goto jit_failure;
						}
						goto done;
#ifndef ZEND_JIT_IR //???
					case ZEND_RETURN:
						op1_info = OP1_INFO();
						if ((PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info))
						 || op_array->type == ZEND_EVAL_CODE
						 // TODO: support for top-level code
						 || !op_array->function_name
						 // TODO: support for IS_UNDEF ???
						 || (op1_info & MAY_BE_UNDEF)) {
							if (!zend_jit_tail_handler(&ctx, opline)) {
								goto jit_failure;
							}
						} else {
							int j;
							bool left_frame = 0;

							if (!zend_jit_return(&ctx, opline, op_array,
									op1_info, OP1_REG_ADDR())) {
								goto jit_failure;
							}
							if (jit_return_label >= 0) {
								if (!zend_jit_jmp(&ctx, jit_return_label)) {
									goto jit_failure;
								}
								goto done;
							}
							jit_return_label = ssa->cfg.blocks_count * 2;
							if (!zend_jit_label(&ctx, jit_return_label)) {
								goto jit_failure;
							}
							if (op_array->last_var > 100) {
								/* To many CVs to unroll */
								if (!zend_jit_free_cvs(&ctx)) {
									goto jit_failure;
								}
								left_frame = 1;
							}
							if (!left_frame) {
								for (j = 0 ; j < op_array->last_var; j++) {
									uint32_t info = zend_ssa_cv_info(op_array, ssa, j);

									if (info & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_REF)) {
										if (!left_frame) {
											left_frame = 1;
										    if (!zend_jit_leave_frame(&ctx)) {
												goto jit_failure;
										    }
										}
										if (!zend_jit_free_cv(&ctx, info, j)) {
											goto jit_failure;
										}
									}
								}
							}
							if (!zend_jit_leave_func(&ctx, op_array, opline, op1_info, left_frame,
									NULL, NULL, (ssa->cfg.flags & ZEND_FUNC_INDIRECT_VAR_ACCESS) != 0, 1)) {
								goto jit_failure;
							}
						}
						goto done;
#endif
					case ZEND_BOOL:
					case ZEND_BOOL_NOT:
						if (!zend_jit_bool_jmpznz(&ctx, opline,
								OP1_INFO(), OP1_REG_ADDR(), RES_REG_ADDR(),
								-1, -1,
								zend_may_throw(opline, ssa_op, op_array, ssa),
								opline->opcode, NULL)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_JMPZ:
					case ZEND_JMPNZ:
						if (opline > op_array->opcodes + ssa->cfg.blocks[b].start &&
						    ((opline-1)->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
							/* smart branch */
							if (!zend_jit_cond_jmp(&ctx, opline + 1, ssa->cfg.blocks[b].successors[0])) {
								goto jit_failure;
							}
							goto done;
						}
						ZEND_FALLTHROUGH;
					case ZEND_JMPZ_EX:
					case ZEND_JMPNZ_EX:
						if (opline->result_type == IS_UNDEF) {
							res_addr = 0;
						} else {
							res_addr = RES_REG_ADDR();
						}
						if (!zend_jit_bool_jmpznz(&ctx, opline,
								OP1_INFO(), OP1_REG_ADDR(), res_addr,
								ssa->cfg.blocks[b].successors[0], ssa->cfg.blocks[b].successors[1],
								zend_may_throw(opline, ssa_op, op_array, ssa),
								opline->opcode, NULL)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ISSET_ISEMPTY_CV:
						if ((opline->extended_value & ZEND_ISEMPTY)) {
							// TODO: support for empty() ???
							break;
						}
						if ((opline->result_type & IS_TMP_VAR)
						 && (i + 1) <= end
						 && ((opline+1)->opcode == ZEND_JMPZ
						  || (opline+1)->opcode == ZEND_JMPNZ)
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							i++;
							smart_branch_opcode = (opline+1)->opcode;
							target_label = ssa->cfg.blocks[b].successors[0];
							target_label2 = ssa->cfg.blocks[b].successors[1];
						} else {
							smart_branch_opcode = 0;
							target_label = target_label2 = (uint32_t)-1;
						}
						if (!zend_jit_isset_isempty_cv(&ctx, opline,
								OP1_INFO(), OP1_REG_ADDR(),
								smart_branch_opcode, target_label, target_label2,
								NULL)) {
							goto jit_failure;
						}
						goto done;
#ifndef ZEND_JIT_IR //???
					case ZEND_IN_ARRAY:
						if (opline->op1_type == IS_VAR || opline->op1_type == IS_TMP_VAR) {
							break;
						}
						op1_info = OP1_INFO();
						if ((op1_info & (MAY_BE_ANY|MAY_BE_UNDEF|MAY_BE_REF)) != MAY_BE_STRING) {
							break;
						}
						if ((opline->result_type & IS_TMP_VAR)
						 && (i + 1) <= end
						 && ((opline+1)->opcode == ZEND_JMPZ
						  || (opline+1)->opcode == ZEND_JMPNZ)
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							i++;
							smart_branch_opcode = (opline+1)->opcode;
							target_label = ssa->cfg.blocks[b].successors[0];
							target_label2 = ssa->cfg.blocks[b].successors[1];
						} else {
							smart_branch_opcode = 0;
							target_label = target_label2 = (uint32_t)-1;
						}
						if (!zend_jit_in_array(&ctx, opline,
								op1_info, OP1_REG_ADDR(),
								smart_branch_opcode, target_label, target_label2,
								NULL)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FETCH_DIM_R:
					case ZEND_FETCH_DIM_IS:
					case ZEND_FETCH_LIST_R:
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						if (!zend_jit_fetch_dim_read(&ctx, opline, ssa, ssa_op,
								OP1_INFO(), OP1_REG_ADDR(), 0,
								OP2_INFO(), RES_INFO(), RES_REG_ADDR(), IS_UNKNOWN)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FETCH_DIM_W:
					case ZEND_FETCH_DIM_RW:
//					case ZEND_FETCH_DIM_UNSET:
					case ZEND_FETCH_LIST_W:
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						if (opline->op1_type != IS_CV) {
							break;
						}
						if (!zend_jit_fetch_dim(&ctx, opline,
								OP1_INFO(), OP1_REG_ADDR(), OP2_INFO(), RES_REG_ADDR(), IS_UNKNOWN)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ISSET_ISEMPTY_DIM_OBJ:
						if ((opline->extended_value & ZEND_ISEMPTY)) {
							// TODO: support for empty() ???
							break;
						}
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						if ((opline->result_type & IS_TMP_VAR)
						 && (i + 1) <= end
						 && ((opline+1)->opcode == ZEND_JMPZ
						  || (opline+1)->opcode == ZEND_JMPNZ)
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							i++;
							smart_branch_opcode = (opline+1)->opcode;
							target_label = ssa->cfg.blocks[b].successors[0];
							target_label2 = ssa->cfg.blocks[b].successors[1];
						} else {
							smart_branch_opcode = 0;
							target_label = target_label2 = (uint32_t)-1;
						}
						if (!zend_jit_isset_isempty_dim(&ctx, opline,
								OP1_INFO(), OP1_REG_ADDR(), 0,
								OP2_INFO(), IS_UNKNOWN,
								zend_may_throw(opline, ssa_op, op_array, ssa),
								smart_branch_opcode, target_label, target_label2,
								NULL)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FETCH_OBJ_R:
					case ZEND_FETCH_OBJ_IS:
					case ZEND_FETCH_OBJ_W:
						if (opline->op2_type != IS_CONST
						 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING
						 || Z_STRVAL_P(RT_CONSTANT(opline, opline->op2))[0] == '\0') {
							break;
						}
						ce = NULL;
						ce_is_instanceof = 0;
						on_this = 0;
						if (opline->op1_type == IS_UNUSED) {
							op1_info = MAY_BE_OBJECT|MAY_BE_RC1|MAY_BE_RCN;
							op1_addr = 0;
							ce = op_array->scope;
							ce_is_instanceof = (ce->ce_flags & ZEND_ACC_FINAL) != 0;
							on_this = 1;
						} else {
							op1_info = OP1_INFO();
							if (!(op1_info & MAY_BE_OBJECT)) {
								break;
							}
							op1_addr = OP1_REG_ADDR();
							if (ssa->var_info && ssa->ops) {
								zend_ssa_op *ssa_op = &ssa->ops[opline - op_array->opcodes];
								if (ssa_op->op1_use >= 0) {
									zend_ssa_var_info *op1_ssa = ssa->var_info + ssa_op->op1_use;
									if (op1_ssa->ce && !op1_ssa->ce->create_object) {
										ce = op1_ssa->ce;
										ce_is_instanceof = op1_ssa->is_instanceof;
									}
								}
							}
						}
						if (!zend_jit_fetch_obj(&ctx, opline, op_array, ssa, ssa_op,
								op1_info, op1_addr, 0, ce, ce_is_instanceof, on_this, 0, 0, NULL,
								IS_UNKNOWN,
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_BIND_GLOBAL:
						if (!ssa->ops || !ssa->var_info) {
							op1_info = MAY_BE_ANY|MAY_BE_REF;
						} else {
							op1_info = OP1_INFO();
						}
						if (!zend_jit_bind_global(&ctx, opline, op1_info)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_RECV:
						if (!zend_jit_recv(&ctx, opline, op_array)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_RECV_INIT:
						if (!zend_jit_recv_init(&ctx, opline, op_array,
								(opline + 1)->opcode != ZEND_RECV_INIT,
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FREE:
					case ZEND_FE_FREE:
						if (!zend_jit_free(&ctx, opline, OP1_INFO(),
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ECHO:
						op1_info = OP1_INFO();
						if ((op1_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) != MAY_BE_STRING) {
							break;
						}
						if (!zend_jit_echo(&ctx, opline, op1_info)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_STRLEN:
						op1_info = OP1_INFO();
						if ((op1_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) != MAY_BE_STRING) {
							break;
						}
						if (!zend_jit_strlen(&ctx, opline, op1_info, OP1_REG_ADDR(), RES_REG_ADDR())) {
							goto jit_failure;
						}
						goto done;
					case ZEND_COUNT:
						op1_info = OP1_INFO();
						if ((op1_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) != MAY_BE_ARRAY) {
							break;
						}
						if (!zend_jit_count(&ctx, opline, op1_info, OP1_REG_ADDR(), RES_REG_ADDR(), zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FETCH_THIS:
						if (!zend_jit_fetch_this(&ctx, opline, op_array, 0)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SWITCH_LONG:
					case ZEND_SWITCH_STRING:
					case ZEND_MATCH:
						if (!zend_jit_switch(&ctx, opline, op_array, ssa, NULL, NULL)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_VERIFY_RETURN_TYPE:
						if (opline->op1_type == IS_UNUSED) {
							/* Always throws */
							break;
						}
						if (opline->op1_type == IS_CONST) {
							/* TODO Different instruction format, has return value */
							break;
						}
						if (op_array->fn_flags & ZEND_ACC_RETURN_REFERENCE) {
							/* Not worth bothering with */
							break;
						}
						if (OP1_INFO() & MAY_BE_REF) {
							/* TODO May need reference unwrapping. */
							break;
						}
						if (!zend_jit_verify_return_type(&ctx, opline, op_array, OP1_INFO())) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FE_RESET_R:
						op1_info = OP1_INFO();
						if ((op1_info & (MAY_BE_ANY|MAY_BE_REF|MAY_BE_UNDEF)) != MAY_BE_ARRAY) {
							break;
						}
						if (!zend_jit_fe_reset(&ctx, opline, op1_info)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FE_FETCH_R:
						op1_info = OP1_INFO();
						if ((op1_info & MAY_BE_ANY) != MAY_BE_ARRAY) {
							break;
						}
						if (!zend_jit_fe_fetch(&ctx, opline, op1_info, OP2_INFO(),
								ssa->cfg.blocks[b].successors[0], opline->opcode, NULL)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FETCH_CONSTANT:
						if (!zend_jit_fetch_constant(&ctx, opline, op_array, ssa, ssa_op, RES_REG_ADDR())) {
							goto jit_failure;
						}
						goto done;
					case ZEND_INIT_METHOD_CALL:
						if (opline->op2_type != IS_CONST
						 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING) {
							break;
						}
						ce = NULL;
						ce_is_instanceof = 0;
						on_this = 0;
						if (opline->op1_type == IS_UNUSED) {
							op1_info = MAY_BE_OBJECT|MAY_BE_RC1|MAY_BE_RCN;
							op1_addr = 0;
							ce = op_array->scope;
							ce_is_instanceof = (ce->ce_flags & ZEND_ACC_FINAL) != 0;
							on_this = 1;
						} else {
							op1_info = OP1_INFO();
							if (!(op1_info & MAY_BE_OBJECT)) {
								break;
							}
							op1_addr = OP1_REG_ADDR();
							if (ssa->var_info && ssa->ops) {
								zend_ssa_op *ssa_op = &ssa->ops[opline - op_array->opcodes];
								if (ssa_op->op1_use >= 0) {
									zend_ssa_var_info *op1_ssa = ssa->var_info + ssa_op->op1_use;
									if (op1_ssa->ce && !op1_ssa->ce->create_object) {
										ce = op1_ssa->ce;
										ce_is_instanceof = op1_ssa->is_instanceof;
									}
								}
							}
						}
						if (!zend_jit_init_method_call(&ctx, opline, b, op_array, ssa, ssa_op, call_level,
								op1_info, op1_addr, ce, ce_is_instanceof, on_this, 0, NULL,
								NULL, 0, 0)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ROPE_INIT:
					case ZEND_ROPE_ADD:
					case ZEND_ROPE_END:
						op2_info = OP2_INFO();
						if ((op2_info & (MAY_BE_UNDEF|MAY_BE_ANY|MAY_BE_REF)) != MAY_BE_STRING) {
							break;
						}
						if (!zend_jit_rope(&ctx, opline, op2_info)) {
							goto jit_failure;
						}
						goto done;
#endif
					default:
						break;
				}
			}

			switch (opline->opcode) {
				case ZEND_RECV_INIT:
				case ZEND_BIND_GLOBAL:
					if (opline == op_array->opcodes ||
					    opline->opcode != op_array->opcodes[i-1].opcode) {
						/* repeatable opcodes */
						if (!zend_jit_handler(&ctx, opline,
								zend_may_throw(opline, ssa_op, op_array, ssa))) {
							goto jit_failure;
						}
					}
#ifndef ZEND_JIT_IR
					zend_jit_set_last_valid_opline(opline+1);
#else
					zend_jit_set_last_valid_opline(&ctx, opline+1);
#endif
					break;
				case ZEND_NOP:
				case ZEND_OP_DATA:
				case ZEND_SWITCH_LONG:
				case ZEND_SWITCH_STRING:
				case ZEND_MATCH:
					break;
				case ZEND_JMP:
					if (JIT_G(opt_level) < ZEND_JIT_LEVEL_INLINE) {
						const zend_op *target = OP_JMP_ADDR(opline, opline->op1);

						if (!zend_jit_set_ip(&ctx, target)) {
							goto jit_failure;
						}
					}
					if (!zend_jit_jmp(&ctx, ssa->cfg.blocks[b].successors[0])) {
						goto jit_failure;
					}
#ifndef ZEND_JIT_IR
					is_terminated = 1;
#endif
					break;
				case ZEND_CATCH:
				case ZEND_FAST_CALL:
				case ZEND_FAST_RET:
				case ZEND_GENERATOR_CREATE:
				case ZEND_GENERATOR_RETURN:
				case ZEND_RETURN_BY_REF:
				case ZEND_RETURN:
				case ZEND_EXIT:
				case ZEND_MATCH_ERROR:
				/* switch through trampoline */
				case ZEND_YIELD:
				case ZEND_YIELD_FROM:
#ifdef ZEND_JIT_IR
				case ZEND_THROW:
				case ZEND_VERIFY_NEVER_TYPE:
#endif
					if (!zend_jit_tail_handler(&ctx, opline)) {
						goto jit_failure;
					}
#ifndef ZEND_JIT_IR
					is_terminated = 1;
#else
					/* THROW and EXIT may be used in the middle of BB */
					/* don't generate code for the rest of BB */
					i = end;
#endif
					break;
				/* stackless execution */
				case ZEND_INCLUDE_OR_EVAL:
				case ZEND_DO_FCALL:
				case ZEND_DO_UCALL:
				case ZEND_DO_FCALL_BY_NAME:
					if (!zend_jit_call(&ctx, opline, b + 1)) {
						goto jit_failure;
					}
#ifndef ZEND_JIT_IR
					is_terminated = 1;
#endif
					break;
				case ZEND_JMPZ:
				case ZEND_JMPNZ:
					if (opline > op_array->opcodes + ssa->cfg.blocks[b].start &&
					    ((opline-1)->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
						/* smart branch */
						if (!zend_jit_cond_jmp(&ctx, opline + 1, ssa->cfg.blocks[b].successors[0])) {
							goto jit_failure;
						}
						goto done;
					}
					ZEND_FALLTHROUGH;
				case ZEND_JMPZ_EX:
				case ZEND_JMPNZ_EX:
				case ZEND_JMP_SET:
				case ZEND_COALESCE:
				case ZEND_JMP_NULL:
				case ZEND_FE_RESET_R:
				case ZEND_FE_RESET_RW:
				case ZEND_ASSERT_CHECK:
				case ZEND_FE_FETCH_R:
				case ZEND_FE_FETCH_RW:
				case ZEND_BIND_INIT_STATIC_OR_JMP:
					if (!zend_jit_handler(&ctx, opline,
							zend_may_throw(opline, ssa_op, op_array, ssa)) ||
					    !zend_jit_cond_jmp(&ctx, opline + 1, ssa->cfg.blocks[b].successors[0])) {
						goto jit_failure;
					}
					break;
				case ZEND_NEW:
					if (!zend_jit_handler(&ctx, opline, 1)) {
						return 0;
					}
					if (opline->extended_value == 0 && (opline+1)->opcode == ZEND_DO_FCALL) {
						zend_class_entry *ce = NULL;

						if (JIT_G(opt_level) >= ZEND_JIT_LEVEL_OPT_FUNC) {
							if (ssa->ops && ssa->var_info) {
								zend_ssa_var_info *res_ssa = &ssa->var_info[ssa->ops[opline - op_array->opcodes].result_def];
								if (res_ssa->ce && !res_ssa->is_instanceof) {
									ce = res_ssa->ce;
								}
							}
						} else {
							if (opline->op1_type == IS_CONST) {
								zval *zv = RT_CONSTANT(opline, opline->op1);
								if (Z_TYPE_P(zv) == IS_STRING) {
									zval *lc = zv + 1;
									ce = (zend_class_entry*)zend_hash_find_ptr(EG(class_table), Z_STR_P(lc));
								}
							}
						}

						i++;

						if (!ce || !(ce->ce_flags & ZEND_ACC_LINKED) || ce->constructor) {
							const zend_op *next_opline = opline + 1;

#ifndef ZEND_JIT_IR
							zend_jit_cond_jmp(&ctx, next_opline, ssa->cfg.blocks[b].successors[0]);
							if (JIT_G(opt_level) < ZEND_JIT_LEVEL_INLINE) {
								zend_jit_call(&ctx, next_opline, b + 1);
								is_terminated = 1;
							} else {
								zend_jit_do_fcall(&ctx, next_opline, op_array, ssa, call_level, b + 1, NULL);
							}
#else
							zend_jit_constructor(&ctx, next_opline, ssa->cfg.blocks[b].successors[0]);
#endif
						}

						/* We skip over the DO_FCALL, so decrement call_level ourselves. */
						call_level--;
					}
					break;
				default:
					if (!zend_jit_handler(&ctx, opline,
							zend_may_throw(opline, ssa_op, op_array, ssa))) {
						goto jit_failure;
					}
					if (i == end
					 && (opline->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
						/* smart branch split across basic blocks */
#ifndef ZEND_JIT_IR
						if (!zend_jit_cond_jmp(&ctx, opline + 2, ssa->cfg.blocks[b+1].successors[0])) {
							goto jit_failure;
						}
						if (!zend_jit_jmp(&ctx, ssa->cfg.blocks[b+1].successors[1])) {
							goto jit_failure;
						}
						is_terminated = 1;
#else
						if (!zend_jit_set_cond(&ctx, opline + 2, opline->result.var)) {
							goto jit_failure;
						}
#endif
					}
			}
done:
			switch (opline->opcode) {
				case ZEND_DO_FCALL:
				case ZEND_DO_ICALL:
				case ZEND_DO_UCALL:
				case ZEND_DO_FCALL_BY_NAME:
				case ZEND_CALLABLE_CONVERT:
					call_level--;
			}
		}
#ifdef ZEND_JIT_IR
		zend_jit_bb_end(&ctx, b);
#endif
	}

#ifndef ZEND_JIT_IR
	handler = dasm_link_and_encode(&ctx, op_array, ssa, rt_opline, ra, NULL, 0,
		(zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) ? SP_ADJ_VM : SP_ADJ_RET, SP_ADJ_JIT);
	if (!handler) {
		goto jit_failure;
	}
	dasm_free(&ctx);
#else
	handler = zend_jit_finish(&ctx);
	if (!handler) {
		goto jit_failure;
	}
	zend_jit_free(&ctx);
#endif

	if (JIT_G(opt_flags) & (ZEND_JIT_REG_ALLOC_LOCAL|ZEND_JIT_REG_ALLOC_GLOBAL)) {
		zend_arena_release(&CG(arena), checkpoint);
	}
	return SUCCESS;

jit_failure:
#ifndef ZEND_JIT_IR
	if (ctx) {
		dasm_free(&ctx);
	}
#else
	zend_jit_free(&ctx);
#endif
	if (JIT_G(opt_flags) & (ZEND_JIT_REG_ALLOC_LOCAL|ZEND_JIT_REG_ALLOC_GLOBAL)) {
		zend_arena_release(&CG(arena), checkpoint);
	}
	return FAILURE;
}

static void zend_jit_collect_calls(zend_op_array *op_array, zend_script *script)
{
	zend_func_info *func_info;

	if (JIT_G(trigger) == ZEND_JIT_ON_FIRST_EXEC ||
	    JIT_G(trigger) == ZEND_JIT_ON_PROF_REQUEST ||
	    JIT_G(trigger) == ZEND_JIT_ON_HOT_COUNTERS) {
	    func_info = ZEND_FUNC_INFO(op_array);
	} else {
		func_info = zend_arena_calloc(&CG(arena), 1, sizeof(zend_func_info));
		ZEND_SET_FUNC_INFO(op_array, func_info);
	}
	zend_analyze_calls(&CG(arena), script, ZEND_CALL_TREE, op_array, func_info);
}

static void zend_jit_cleanup_func_info(zend_op_array *op_array)
{
	zend_func_info *func_info = ZEND_FUNC_INFO(op_array);
	zend_call_info *caller_info, *callee_info;

	if (func_info) {
		caller_info = func_info->caller_info;
		callee_info = func_info->callee_info;

		if (JIT_G(trigger) == ZEND_JIT_ON_FIRST_EXEC ||
		    JIT_G(trigger) == ZEND_JIT_ON_PROF_REQUEST ||
		    JIT_G(trigger) == ZEND_JIT_ON_HOT_COUNTERS) {
			func_info->num = 0;
			func_info->flags &= ZEND_FUNC_JIT_ON_FIRST_EXEC
				| ZEND_FUNC_JIT_ON_PROF_REQUEST
				| ZEND_FUNC_JIT_ON_HOT_COUNTERS
				| ZEND_FUNC_JIT_ON_HOT_TRACE;
			memset(&func_info->ssa, 0, sizeof(zend_func_info) - offsetof(zend_func_info, ssa));
		} else {
			ZEND_SET_FUNC_INFO(op_array, NULL);
		}

		while (caller_info) {
			if (caller_info->caller_op_array) {
				zend_jit_cleanup_func_info(caller_info->caller_op_array);
			}
			caller_info = caller_info->next_caller;
		}
		while (callee_info) {
			if (callee_info->callee_func && callee_info->callee_func->type == ZEND_USER_FUNCTION) {
				zend_jit_cleanup_func_info(&callee_info->callee_func->op_array);
			}
			callee_info = callee_info->next_callee;
		}
	}
}

static int zend_real_jit_func(zend_op_array *op_array, zend_script *script, const zend_op *rt_opline)
{
	zend_ssa ssa;
	void *checkpoint;
	zend_func_info *func_info;

	if (*dasm_ptr == dasm_end) {
		return FAILURE;
	}

	checkpoint = zend_arena_checkpoint(CG(arena));

	/* Build SSA */
	memset(&ssa, 0, sizeof(zend_ssa));

	if (zend_jit_op_array_analyze1(op_array, script, &ssa) != SUCCESS) {
		goto jit_failure;
	}

	if (JIT_G(opt_level) >= ZEND_JIT_LEVEL_OPT_FUNCS) {
		zend_jit_collect_calls(op_array, script);
		func_info = ZEND_FUNC_INFO(op_array);
		func_info->call_map = zend_build_call_map(&CG(arena), func_info, op_array);
		if (op_array->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
			zend_init_func_return_info(op_array, script, &func_info->return_info);
		}
	}

	if (zend_jit_op_array_analyze2(op_array, script, &ssa, ZCG(accel_directives).optimization_level) != SUCCESS) {
		goto jit_failure;
	}

	if (JIT_G(debug) & ZEND_JIT_DEBUG_SSA) {
		zend_dump_op_array(op_array, ZEND_DUMP_HIDE_UNREACHABLE|ZEND_DUMP_RC_INFERENCE|ZEND_DUMP_SSA, "JIT", &ssa);
	}

	if (zend_jit(op_array, &ssa, rt_opline) != SUCCESS) {
		goto jit_failure;
	}

	zend_jit_cleanup_func_info(op_array);
	zend_arena_release(&CG(arena), checkpoint);
	return SUCCESS;

jit_failure:
	zend_jit_cleanup_func_info(op_array);
	zend_arena_release(&CG(arena), checkpoint);
	return FAILURE;
}

/* Run-time JIT handler */
static int ZEND_FASTCALL zend_runtime_jit(void)
{
	zend_execute_data *execute_data = EG(current_execute_data);
	zend_op_array *op_array = &EX(func)->op_array;
	zend_op *opline = op_array->opcodes;
	zend_jit_op_array_extension *jit_extension;
	bool do_bailout = 0;

	zend_shared_alloc_lock();

	if (ZEND_FUNC_INFO(op_array)) {

		SHM_UNPROTECT();
		zend_jit_unprotect();

		zend_try {
			/* restore original opcode handlers */
			if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
				while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
					opline++;
				}
			}
			jit_extension = (zend_jit_op_array_extension*)ZEND_FUNC_INFO(op_array);
			opline->handler = jit_extension->orig_handler;

			/* perform real JIT for this function */
			zend_real_jit_func(op_array, NULL, NULL);
		} zend_catch {
			do_bailout = true;
		} zend_end_try();

		zend_jit_protect();
		SHM_PROTECT();
	}

	zend_shared_alloc_unlock();

	if (do_bailout) {
		zend_bailout();
	}

	/* JIT-ed code is going to be called by VM */
	return 0;
}

void zend_jit_check_funcs(HashTable *function_table, bool is_method) {
	zend_op *opline;
	zend_function *func;
	zend_op_array *op_array;
	uintptr_t counter;
	zend_jit_op_array_extension *jit_extension;

	ZEND_HASH_MAP_REVERSE_FOREACH_PTR(function_table, func) {
		if (func->type == ZEND_INTERNAL_FUNCTION) {
			break;
		}
		op_array = &func->op_array;
		opline = op_array->opcodes;
		if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
			while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
				opline++;
			}
		}
		if (opline->handler == zend_jit_profile_jit_handler) {
			if (!RUN_TIME_CACHE(op_array)) {
				continue;
			}
			counter = (uintptr_t)ZEND_COUNTER_INFO(op_array);
			ZEND_COUNTER_INFO(op_array) = 0;
			jit_extension = (zend_jit_op_array_extension*)ZEND_FUNC_INFO(op_array);
			opline->handler = jit_extension->orig_handler;
			if (((double)counter / (double)zend_jit_profile_counter) > JIT_G(prof_threshold)) {
				zend_real_jit_func(op_array, NULL, NULL);
			}
		}
	} ZEND_HASH_FOREACH_END();
}

void ZEND_FASTCALL zend_jit_hot_func(zend_execute_data *execute_data, const zend_op *opline)
{
	zend_op_array *op_array = &EX(func)->op_array;
	zend_jit_op_array_hot_extension *jit_extension;
	uint32_t i;
	bool do_bailout = 0;

	zend_shared_alloc_lock();
	jit_extension = (zend_jit_op_array_hot_extension*)ZEND_FUNC_INFO(op_array);

	if (jit_extension) {
		SHM_UNPROTECT();
		zend_jit_unprotect();

		zend_try {
			for (i = 0; i < op_array->last; i++) {
				op_array->opcodes[i].handler = jit_extension->orig_handlers[i];
			}

			/* perform real JIT for this function */
			zend_real_jit_func(op_array, NULL, opline);
		} zend_catch {
			do_bailout = 1;
		} zend_end_try();

		zend_jit_protect();
		SHM_PROTECT();
	}

	zend_shared_alloc_unlock();

	if (do_bailout) {
		zend_bailout();
	}
	/* JIT-ed code is going to be called by VM */
}

static void zend_jit_setup_hot_counters_ex(zend_op_array *op_array, zend_cfg *cfg)
{
	if (JIT_G(hot_func)) {
		zend_op *opline = op_array->opcodes;

		if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
			while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
				opline++;
			}
		}

		opline->handler = (const void*)zend_jit_func_hot_counter_handler;
	}

	if (JIT_G(hot_loop)) {
		uint32_t i;

		for (i = 0; i < cfg->blocks_count; i++) {
			if ((cfg->blocks[i].flags & ZEND_BB_REACHABLE) &&
			    (cfg->blocks[i].flags & ZEND_BB_LOOP_HEADER)) {
			    op_array->opcodes[cfg->blocks[i].start].handler =
					(const void*)zend_jit_loop_hot_counter_handler;
			}
		}
	}
}

static int zend_jit_restart_hot_counters(zend_op_array *op_array)
{
	zend_jit_op_array_hot_extension *jit_extension;
	zend_cfg cfg;
	uint32_t i;

	jit_extension = (zend_jit_op_array_hot_extension*)ZEND_FUNC_INFO(op_array);
	for (i = 0; i < op_array->last; i++) {
		op_array->opcodes[i].handler = jit_extension->orig_handlers[i];
	}

	if (zend_jit_build_cfg(op_array, &cfg) != SUCCESS) {
		return FAILURE;
	}

	zend_jit_setup_hot_counters_ex(op_array, &cfg);

	return SUCCESS;
}

static int zend_jit_setup_hot_counters(zend_op_array *op_array)
{
	zend_jit_op_array_hot_extension *jit_extension;
	zend_cfg cfg;
	uint32_t i;

	ZEND_ASSERT(!JIT_G(hot_func) || zend_jit_func_hot_counter_handler != NULL);
	ZEND_ASSERT(!JIT_G(hot_loop) || zend_jit_loop_hot_counter_handler != NULL);

	if (zend_jit_build_cfg(op_array, &cfg) != SUCCESS) {
		return FAILURE;
	}

	jit_extension = (zend_jit_op_array_hot_extension*)zend_shared_alloc(sizeof(zend_jit_op_array_hot_extension) + (op_array->last - 1) * sizeof(void*));
	if (!jit_extension) {
		return FAILURE;
	}
	memset(&jit_extension->func_info, 0, sizeof(zend_func_info));
	jit_extension->func_info.flags = ZEND_FUNC_JIT_ON_HOT_COUNTERS;
	jit_extension->counter = &zend_jit_hot_counters[zend_jit_op_array_hash(op_array) & (ZEND_HOT_COUNTERS_COUNT - 1)];
	for (i = 0; i < op_array->last; i++) {
		jit_extension->orig_handlers[i] = op_array->opcodes[i].handler;
	}
	ZEND_SET_FUNC_INFO(op_array, (void*)jit_extension);

	zend_jit_setup_hot_counters_ex(op_array, &cfg);

	zend_shared_alloc_register_xlat_entry(op_array->opcodes, jit_extension);

	return SUCCESS;
}

#include "jit/zend_jit_trace.c"

ZEND_EXT_API int zend_jit_op_array(zend_op_array *op_array, zend_script *script)
{
	if (dasm_ptr == NULL) {
		return FAILURE;
	}

	if (JIT_G(trigger) == ZEND_JIT_ON_FIRST_EXEC) {
		zend_jit_op_array_extension *jit_extension;
		zend_op *opline = op_array->opcodes;

		if (CG(compiler_options) & ZEND_COMPILE_PRELOAD) {
			ZEND_SET_FUNC_INFO(op_array, NULL);
			zend_error(E_WARNING, "Preloading is incompatible with first-exec and profile triggered JIT");
			return SUCCESS;
		}

		/* Set run-time JIT handler */
		ZEND_ASSERT(zend_jit_runtime_jit_handler != NULL);
		if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
			while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
				opline++;
			}
		}
		jit_extension = (zend_jit_op_array_extension*)zend_shared_alloc(sizeof(zend_jit_op_array_extension));
		if (!jit_extension) {
			return FAILURE;
		}
		memset(&jit_extension->func_info, 0, sizeof(zend_func_info));
		jit_extension->func_info.flags = ZEND_FUNC_JIT_ON_FIRST_EXEC;
		jit_extension->orig_handler = (void*)opline->handler;
		ZEND_SET_FUNC_INFO(op_array, (void*)jit_extension);
		opline->handler = (const void*)zend_jit_runtime_jit_handler;
		zend_shared_alloc_register_xlat_entry(op_array->opcodes, jit_extension);

		return SUCCESS;
	} else if (JIT_G(trigger) == ZEND_JIT_ON_PROF_REQUEST) {
		zend_jit_op_array_extension *jit_extension;
		zend_op *opline = op_array->opcodes;

		if (CG(compiler_options) & ZEND_COMPILE_PRELOAD) {
			ZEND_SET_FUNC_INFO(op_array, NULL);
			zend_error(E_WARNING, "Preloading is incompatible with first-exec and profile triggered JIT");
			return SUCCESS;
		}

		ZEND_ASSERT(zend_jit_profile_jit_handler != NULL);
		if (op_array->function_name) {
			if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
				while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
					opline++;
				}
			}
			jit_extension = (zend_jit_op_array_extension*)zend_shared_alloc(sizeof(zend_jit_op_array_extension));
			if (!jit_extension) {
				return FAILURE;
			}
			memset(&jit_extension->func_info, 0, sizeof(zend_func_info));
			jit_extension->func_info.flags = ZEND_FUNC_JIT_ON_PROF_REQUEST;
			jit_extension->orig_handler = (void*)opline->handler;
			ZEND_SET_FUNC_INFO(op_array, (void*)jit_extension);
			opline->handler = (const void*)zend_jit_profile_jit_handler;
			zend_shared_alloc_register_xlat_entry(op_array->opcodes, jit_extension);
		}

		return SUCCESS;
	} else if (JIT_G(trigger) == ZEND_JIT_ON_HOT_COUNTERS) {
		return zend_jit_setup_hot_counters(op_array);
	} else if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
		return zend_jit_setup_hot_trace_counters(op_array);
	} else if (JIT_G(trigger) == ZEND_JIT_ON_SCRIPT_LOAD) {
		return zend_real_jit_func(op_array, script, NULL);
	} else {
		ZEND_UNREACHABLE();
	}
	return FAILURE;
}

ZEND_EXT_API int zend_jit_script(zend_script *script)
{
	void *checkpoint;
	zend_call_graph call_graph;
	zend_func_info *info;
	int i;

	if (dasm_ptr == NULL || *dasm_ptr == dasm_end) {
		return FAILURE;
	}

	checkpoint = zend_arena_checkpoint(CG(arena));

	call_graph.op_arrays_count = 0;
	zend_build_call_graph(&CG(arena), script, &call_graph);

	zend_analyze_call_graph(&CG(arena), script, &call_graph);

	if (JIT_G(trigger) == ZEND_JIT_ON_FIRST_EXEC ||
	    JIT_G(trigger) == ZEND_JIT_ON_PROF_REQUEST ||
	    JIT_G(trigger) == ZEND_JIT_ON_HOT_COUNTERS ||
	    JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
		for (i = 0; i < call_graph.op_arrays_count; i++) {
			if (zend_jit_op_array(call_graph.op_arrays[i], script) != SUCCESS) {
				goto jit_failure;
			}
		}
	} else if (JIT_G(trigger) == ZEND_JIT_ON_SCRIPT_LOAD) {
		for (i = 0; i < call_graph.op_arrays_count; i++) {
			info = ZEND_FUNC_INFO(call_graph.op_arrays[i]);
			if (info) {
				if (zend_jit_op_array_analyze1(call_graph.op_arrays[i], script, &info->ssa) != SUCCESS) {
					goto jit_failure;
				}
				info->flags = info->ssa.cfg.flags;
			}
		}

		for (i = 0; i < call_graph.op_arrays_count; i++) {
			info = ZEND_FUNC_INFO(call_graph.op_arrays[i]);
			if (info) {
				info->call_map = zend_build_call_map(&CG(arena), info, call_graph.op_arrays[i]);
				if (call_graph.op_arrays[i]->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
					zend_init_func_return_info(call_graph.op_arrays[i], script, &info->return_info);
				}
			}
		}

		for (i = 0; i < call_graph.op_arrays_count; i++) {
			info = ZEND_FUNC_INFO(call_graph.op_arrays[i]);
			if (info) {
				if (zend_jit_op_array_analyze2(call_graph.op_arrays[i], script, &info->ssa, ZCG(accel_directives).optimization_level) != SUCCESS) {
					goto jit_failure;
				}
				info->flags = info->ssa.cfg.flags;
			}
		}

		for (i = 0; i < call_graph.op_arrays_count; i++) {
			info = ZEND_FUNC_INFO(call_graph.op_arrays[i]);
			if (info) {
				if (JIT_G(debug) & ZEND_JIT_DEBUG_SSA) {
					zend_dump_op_array(call_graph.op_arrays[i], ZEND_DUMP_HIDE_UNREACHABLE|ZEND_DUMP_RC_INFERENCE|ZEND_DUMP_SSA, "JIT", &info->ssa);
				}
				if (zend_jit(call_graph.op_arrays[i], &info->ssa, NULL) != SUCCESS) {
					goto jit_failure;
				}
			}
		}

		for (i = 0; i < call_graph.op_arrays_count; i++) {
			ZEND_SET_FUNC_INFO(call_graph.op_arrays[i], NULL);
		}
	} else {
		ZEND_UNREACHABLE();
	}

	zend_arena_release(&CG(arena), checkpoint);

	if (JIT_G(trigger) == ZEND_JIT_ON_FIRST_EXEC
	 || JIT_G(trigger) == ZEND_JIT_ON_PROF_REQUEST
	 || JIT_G(trigger) == ZEND_JIT_ON_HOT_COUNTERS
	 || JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
		zend_class_entry *ce;
		zend_op_array *op_array;

		ZEND_HASH_MAP_FOREACH_PTR(&script->class_table, ce) {
			ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, op_array) {
				if (!ZEND_FUNC_INFO(op_array)) {
					void *jit_extension = zend_shared_alloc_get_xlat_entry(op_array->opcodes);

					if (jit_extension) {
						ZEND_SET_FUNC_INFO(op_array, jit_extension);
					}
				}
			} ZEND_HASH_FOREACH_END();
		} ZEND_HASH_FOREACH_END();
	}

	return SUCCESS;

jit_failure:
	if (JIT_G(trigger) == ZEND_JIT_ON_SCRIPT_LOAD) {
		for (i = 0; i < call_graph.op_arrays_count; i++) {
			ZEND_SET_FUNC_INFO(call_graph.op_arrays[i], NULL);
		}
	}
	zend_arena_release(&CG(arena), checkpoint);
	return FAILURE;
}

ZEND_EXT_API void zend_jit_unprotect(void)
{
#ifdef HAVE_MPROTECT
	if (!(JIT_G(debug) & (ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF_DUMP))) {
		int opts = PROT_READ | PROT_WRITE;
#ifdef ZTS
#ifdef HAVE_PTHREAD_JIT_WRITE_PROTECT_NP
		if (zend_write_protect) {
			pthread_jit_write_protect_np(0);
		}
#endif
		opts |= PROT_EXEC;
#endif
		if (mprotect(dasm_buf, dasm_size, opts) != 0) {
			fprintf(stderr, "mprotect() failed [%d] %s\n", errno, strerror(errno));
		}
	}
#elif _WIN32
	if (!(JIT_G(debug) & (ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF_DUMP))) {
		DWORD old, new;
#ifdef ZTS
		new = PAGE_EXECUTE_READWRITE;
#else
		new = PAGE_READWRITE;
#endif
		if (!VirtualProtect(dasm_buf, dasm_size, new, &old)) {
			DWORD err = GetLastError();
			char *msg = php_win32_error_to_msg(err);
			fprintf(stderr, "VirtualProtect() failed [%u] %s\n", err, msg);
			php_win32_error_msg_free(msg);
		}
	}
#endif
}

ZEND_EXT_API void zend_jit_protect(void)
{
#ifdef HAVE_MPROTECT
	if (!(JIT_G(debug) & (ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF_DUMP))) {
#ifdef HAVE_PTHREAD_JIT_WRITE_PROTECT_NP
		if (zend_write_protect) {
			pthread_jit_write_protect_np(1);
		}
#endif
		if (mprotect(dasm_buf, dasm_size, PROT_READ | PROT_EXEC) != 0) {
			fprintf(stderr, "mprotect() failed [%d] %s\n", errno, strerror(errno));
		}
	}
#elif _WIN32
	if (!(JIT_G(debug) & (ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF_DUMP))) {
		DWORD old;

		if (!VirtualProtect(dasm_buf, dasm_size, PAGE_EXECUTE_READ, &old)) {
			DWORD err = GetLastError();
			char *msg = php_win32_error_to_msg(err);
			fprintf(stderr, "VirtualProtect() failed [%u] %s\n", err, msg);
			php_win32_error_msg_free(msg);
		}
	}
#endif
}

static void zend_jit_init_handlers(void)
{
	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
#ifndef ZEND_JIT_IR
		zend_jit_runtime_jit_handler = dasm_labels[zend_lbhybrid_runtime_jit];
		zend_jit_profile_jit_handler = dasm_labels[zend_lbhybrid_profile_jit];
		zend_jit_func_hot_counter_handler = dasm_labels[zend_lbhybrid_func_hot_counter];
		zend_jit_loop_hot_counter_handler = dasm_labels[zend_lbhybrid_loop_hot_counter];
		zend_jit_func_trace_counter_handler = dasm_labels[zend_lbhybrid_func_trace_counter];
		zend_jit_ret_trace_counter_handler = dasm_labels[zend_lbhybrid_ret_trace_counter];
		zend_jit_loop_trace_counter_handler = dasm_labels[zend_lbhybrid_loop_trace_counter];
#else
		zend_jit_runtime_jit_handler = zend_jit_stub_handlers[jit_stub_hybrid_runtime_jit];
		zend_jit_profile_jit_handler = zend_jit_stub_handlers[jit_stub_hybrid_profile_jit];
		zend_jit_func_hot_counter_handler = zend_jit_stub_handlers[jit_stub_hybrid_func_hot_counter];
		zend_jit_loop_hot_counter_handler = zend_jit_stub_handlers[jit_stub_hybrid_loop_hot_counter];
		zend_jit_func_trace_counter_handler = zend_jit_stub_handlers[jit_stub_hybrid_func_trace_counter];
		zend_jit_ret_trace_counter_handler = zend_jit_stub_handlers[jit_stub_hybrid_ret_trace_counter];
		zend_jit_loop_trace_counter_handler = zend_jit_stub_handlers[jit_stub_hybrid_loop_trace_counter];
#endif
	} else {
		zend_jit_runtime_jit_handler = (const void*)zend_runtime_jit;
		zend_jit_profile_jit_handler = (const void*)zend_jit_profile_helper;
		zend_jit_func_hot_counter_handler = (const void*)zend_jit_func_counter_helper;
		zend_jit_loop_hot_counter_handler = (const void*)zend_jit_loop_counter_helper;
		zend_jit_func_trace_counter_handler = (const void*)zend_jit_func_trace_helper;
		zend_jit_ret_trace_counter_handler = (const void*)zend_jit_ret_trace_helper;
		zend_jit_loop_trace_counter_handler = (const void*)zend_jit_loop_trace_helper;
	}
}

#ifndef ZEND_JIT_IR
static int zend_jit_make_stubs(void)
{
	dasm_State* dasm_state = NULL;
	uint32_t i;

	dasm_init(&dasm_state, DASM_MAXSECTION);
	dasm_setupglobal(&dasm_state, dasm_labels, zend_lb_MAX);

	for (i = 0; i < sizeof(zend_jit_stubs)/sizeof(zend_jit_stubs[0]); i++) {
		dasm_setup(&dasm_state, dasm_actions);
		zend_jit_align_stub(&dasm_state);
		if (!zend_jit_stubs[i].stub(&dasm_state)) {
			return 0;
		}
		if (!dasm_link_and_encode(&dasm_state, NULL, NULL, NULL, NULL, zend_jit_stubs[i].name, 0,
				zend_jit_stubs[i].offset, zend_jit_stubs[i].adjustment)) {
			return 0;
		}
	}

	zend_jit_init_handlers();

	dasm_free(&dasm_state);
	return 1;
}
#endif

static void zend_jit_globals_ctor(zend_jit_globals *jit_globals)
{
	memset(jit_globals, 0, sizeof(zend_jit_globals));
	zend_jit_trace_init_caches();
}

#ifdef ZTS
static void zend_jit_globals_dtor(zend_jit_globals *jit_globals)
{
	zend_jit_trace_free_caches(jit_globals);
}
#endif

static int zend_jit_parse_config_num(zend_long jit)
{
	if (jit == 0) {
		JIT_G(on) = 0;
		return SUCCESS;
	}

	if (jit < 0) return FAILURE;

	if (jit % 10 == 0 || jit % 10 > 5) return FAILURE;
	JIT_G(opt_level) = jit % 10;

	jit /= 10;
	if (jit % 10 > 5) return FAILURE;
	JIT_G(trigger) = jit % 10;

	jit /= 10;
	if (jit % 10 > 2) return FAILURE;
	JIT_G(opt_flags) = jit % 10;

	jit /= 10;
	if (jit % 10 > 1) return FAILURE;
	JIT_G(opt_flags) |= ((jit % 10) ? ZEND_JIT_CPU_AVX : 0);

	if (jit / 10 != 0) return FAILURE;

	JIT_G(on) = 1;

	return SUCCESS;
}

ZEND_EXT_API int zend_jit_config(zend_string *jit, int stage)
{
	if (stage != ZEND_INI_STAGE_STARTUP && !JIT_G(enabled)) {
		if (stage == ZEND_INI_STAGE_RUNTIME) {
			zend_error(E_WARNING, "Cannot change opcache.jit setting at run-time (JIT is disabled)");
		}
		return FAILURE;
	}

	if (ZSTR_LEN(jit) == 0
	 || zend_string_equals_literal_ci(jit, "disable")) {
		JIT_G(enabled) = 0;
		JIT_G(on) = 0;
		return SUCCESS;
	} else if (zend_string_equals_literal_ci(jit, "0")
			|| zend_string_equals_literal_ci(jit, "off")
			|| zend_string_equals_literal_ci(jit, "no")
			|| zend_string_equals_literal_ci(jit, "false")) {
		JIT_G(enabled) = 1;
		JIT_G(on) = 0;
		return SUCCESS;
	} else if (zend_string_equals_literal_ci(jit, "1")
			|| zend_string_equals_literal_ci(jit, "on")
			|| zend_string_equals_literal_ci(jit, "yes")
			|| zend_string_equals_literal_ci(jit, "true")
			|| zend_string_equals_literal_ci(jit, "tracing")) {
		JIT_G(enabled) = 1;
		JIT_G(on) = 1;
		JIT_G(opt_level) = ZEND_JIT_LEVEL_OPT_FUNCS;
		JIT_G(trigger) = ZEND_JIT_ON_HOT_TRACE;
		JIT_G(opt_flags) = ZEND_JIT_REG_ALLOC_GLOBAL | ZEND_JIT_CPU_AVX;
		return SUCCESS;
	} else if (zend_string_equals_ci(jit, ZSTR_KNOWN(ZEND_STR_FUNCTION))) {
		JIT_G(enabled) = 1;
		JIT_G(on) = 1;
		JIT_G(opt_level) = ZEND_JIT_LEVEL_OPT_SCRIPT;
		JIT_G(trigger) = ZEND_JIT_ON_SCRIPT_LOAD;
		JIT_G(opt_flags) = ZEND_JIT_REG_ALLOC_GLOBAL | ZEND_JIT_CPU_AVX;
		return SUCCESS;
	} else  {
		char *end;
		zend_long num = ZEND_STRTOL(ZSTR_VAL(jit), &end, 10);
		if (end != ZSTR_VAL(jit) + ZSTR_LEN(jit) || zend_jit_parse_config_num(num) != SUCCESS) {
			goto failure;
		}
		JIT_G(enabled) = 1;
		return SUCCESS;
	}

failure:
	zend_error(E_WARNING, "Invalid \"opcache.jit\" setting. Should be \"disable\", \"on\", \"off\", \"tracing\", \"function\" or 4-digit number");
	JIT_G(enabled) = 0;
	JIT_G(on) = 0;
	return FAILURE;
}

ZEND_EXT_API int zend_jit_debug_config(zend_long old_val, zend_long new_val, int stage)
{
	if (stage != ZEND_INI_STAGE_STARTUP) {
		if (((old_val ^ new_val) & ZEND_JIT_DEBUG_PERSISTENT) != 0) {
			if (stage == ZEND_INI_STAGE_RUNTIME) {
				zend_error(E_WARNING, "Some opcache.jit_debug bits cannot be changed after startup");
			}
			return FAILURE;
		}
#ifdef HAVE_DISASM
		if (new_val & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_ASM_STUBS)) {
			if (JIT_G(enabled) && !JIT_G(symbols) && !zend_jit_disasm_init()) {
				// TODO: error reporting and cleanup ???
				return FAILURE;
			}
			// TODO: symbols for JIT-ed code compiled before are missing ???
		}
#endif
	}
	return SUCCESS;
}

ZEND_EXT_API void zend_jit_init(void)
{
#ifdef ZTS
	jit_globals_id = ts_allocate_id(&jit_globals_id, sizeof(zend_jit_globals), (ts_allocate_ctor) zend_jit_globals_ctor, (ts_allocate_dtor) zend_jit_globals_dtor);
#else
	zend_jit_globals_ctor(&jit_globals);
#endif
}

ZEND_EXT_API int zend_jit_check_support(void)
{
	int i;

	zend_jit_vm_kind = zend_vm_kind();
	if (zend_jit_vm_kind != ZEND_VM_KIND_CALL &&
	    zend_jit_vm_kind != ZEND_VM_KIND_HYBRID) {
		zend_error(E_WARNING, "JIT is compatible only with CALL and HYBRID VM. JIT disabled.");
		JIT_G(enabled) = 0;
		JIT_G(on) = 0;
		return FAILURE;
	}

	if (zend_execute_ex != execute_ex) {
		if (strcmp(sapi_module.name, "phpdbg") != 0) {
			zend_error(E_WARNING, "JIT is incompatible with third party extensions that override zend_execute_ex(). JIT disabled.");
		}
		JIT_G(enabled) = 0;
		JIT_G(on) = 0;
		return FAILURE;
	}

	for (i = 0; i <= 256; i++) {
		switch (i) {
			/* JIT has no effect on these opcodes */
			case ZEND_BEGIN_SILENCE:
			case ZEND_END_SILENCE:
			case ZEND_EXIT:
				break;
			default:
				if (zend_get_user_opcode_handler(i) != NULL) {
					zend_error(E_WARNING, "JIT is incompatible with third party extensions that setup user opcode handlers. JIT disabled.");
					JIT_G(enabled) = 0;
					JIT_G(on) = 0;
					return FAILURE;
				}
		}
	}

	return SUCCESS;
}

ZEND_EXT_API int zend_jit_startup(void *buf, size_t size, bool reattached)
{
	int ret;

	zend_jit_halt_op = zend_get_halt_op();
	zend_jit_profile_counter_rid = zend_get_op_array_extension_handle(ACCELERATOR_PRODUCT_NAME);

#if ZEND_JIT_SUPPORT_CLDEMOTE
	cpu_support_cldemote = zend_cpu_supports_cldemote();
#endif

#ifdef HAVE_PTHREAD_JIT_WRITE_PROTECT_NP
	zend_write_protect = pthread_jit_write_protect_supported_np();
#endif

	dasm_buf = buf;
	dasm_size = size;
	dasm_ptr = dasm_end = (void*)(((char*)dasm_buf) + size - sizeof(*dasm_ptr) * 2);

#ifdef HAVE_MPROTECT
#ifdef HAVE_PTHREAD_JIT_WRITE_PROTECT_NP
	if (zend_write_protect) {
		pthread_jit_write_protect_np(1);
	}
#endif
	if (JIT_G(debug) & (ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF_DUMP)) {
		if (mprotect(dasm_buf, dasm_size, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
			fprintf(stderr, "mprotect() failed [%d] %s\n", errno, strerror(errno));
		}
	} else {
		if (mprotect(dasm_buf, dasm_size, PROT_READ | PROT_EXEC) != 0) {
			fprintf(stderr, "mprotect() failed [%d] %s\n", errno, strerror(errno));
		}
	}
#elif _WIN32
	if (JIT_G(debug) & (ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF_DUMP)) {
		DWORD old;

		if (!VirtualProtect(dasm_buf, dasm_size, PAGE_EXECUTE_READWRITE, &old)) {
			DWORD err = GetLastError();
			char *msg = php_win32_error_to_msg(err);
			fprintf(stderr, "VirtualProtect() failed [%u] %s\n", err, msg);
			php_win32_error_msg_free(msg);
		}
	} else {
		DWORD old;

		if (!VirtualProtect(dasm_buf, dasm_size, PAGE_EXECUTE_READ, &old)) {
			DWORD err = GetLastError();
			char *msg = php_win32_error_to_msg(err);
			fprintf(stderr, "VirtualProtect() failed [%u] %s\n", err, msg);
			php_win32_error_msg_free(msg);
		}
	}
#endif

	if (!reattached) {
		zend_jit_unprotect();
		*dasm_ptr = dasm_buf;
#if _WIN32
		/* reserve space for global labels */
		*dasm_ptr = (void**)*dasm_ptr + zend_lb_MAX;
#endif
		zend_jit_protect();
	}

#ifndef ZEND_JIT_IR
	if (zend_jit_setup() != SUCCESS) {
		// TODO: error reporting and cleanup ???
		return FAILURE;
	}

#ifdef HAVE_GDB
	zend_jit_gdb_init();
#endif

#ifdef HAVE_OPROFILE
	if (JIT_G(debug) & ZEND_JIT_DEBUG_OPROFILE) {
		if (!zend_jit_oprofile_startup()) {
			// TODO: error reporting and cleanup ???
			return FAILURE;
		}
	}
#endif

#ifdef HAVE_DISASM
	if (JIT_G(debug) & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_ASM_STUBS)) {
		if (!zend_jit_disasm_init()) {
			// TODO: error reporting and cleanup ???
			return FAILURE;
		}
	}
#endif

#ifdef HAVE_PERFTOOLS
	if (JIT_G(debug) & ZEND_JIT_DEBUG_PERF_DUMP) {
		zend_jit_perf_jitdump_open();
	}
#endif

	if (!reattached) {
		zend_jit_unprotect();
		ret = zend_jit_make_stubs();
#if _WIN32
		/* save global labels */
		memcpy(dasm_buf, dasm_labels, sizeof(void*) * zend_lb_MAX);
#endif
		zend_jit_protect();
		if (!ret) {
			// TODO: error reporting and cleanup ???
			return FAILURE;
		}
	} else {
#if _WIN32
		/* restore global labels */
		memcpy(dasm_labels, dasm_buf, sizeof(void*) * zend_lb_MAX);
		zend_jit_init_handlers();
#endif
	}

#else /* ZEND_JIT_IR */
	zend_jit_unprotect();
	if (zend_jit_setup() != SUCCESS) {
		zend_jit_protect();
		// TODO: error reporting and cleanup ???
		return FAILURE;
	}
	zend_jit_protect();
	zend_jit_init_handlers();
#endif /* ZEND_JIT_IR */

	if (zend_jit_trace_startup(reattached) != SUCCESS) {
		return FAILURE;
	}

	zend_jit_unprotect();
#ifndef ZEND_JIT_IR
#if ZEND_JIT_TARGET_ARM64
	/* reserve space for global labels veneers */
	dasm_labels_veneers = *dasm_ptr;
	*dasm_ptr = (void**)*dasm_ptr + ZEND_MM_ALIGNED_SIZE_EX(zend_lb_MAX, DASM_ALIGNMENT);
	memset(dasm_labels_veneers, 0, sizeof(void*) * ZEND_MM_ALIGNED_SIZE_EX(zend_lb_MAX, DASM_ALIGNMENT));
#endif
#endif
	/* save JIT buffer pos */
	dasm_ptr[1] = dasm_ptr[0];
	zend_jit_protect();

	return SUCCESS;
}

ZEND_EXT_API void zend_jit_shutdown(void)
{
	if (JIT_G(debug) & ZEND_JIT_DEBUG_SIZE) {
		fprintf(stderr, "\nJIT memory usage: %td\n", (ptrdiff_t)((char*)*dasm_ptr - (char*)dasm_buf));
	}

#ifndef ZEND_JIT_IR
#ifdef HAVE_GDB
	if (JIT_G(debug) & ZEND_JIT_DEBUG_GDB) {
		zend_jit_gdb_unregister();
	}
#endif

#ifdef HAVE_DISASM
	zend_jit_disasm_shutdown();
#endif

#ifdef HAVE_PERFTOOLS
	if (JIT_G(debug) & ZEND_JIT_DEBUG_PERF_DUMP) {
		zend_jit_perf_jitdump_close();
	}
#endif
#else
	zend_jit_end();
#endif

#ifdef ZTS
	ts_free_id(jit_globals_id);
#else
	zend_jit_trace_free_caches(&jit_globals);
#endif
}

static void zend_jit_reset_counters(void)
{
	int i;

	for (i = 0; i < ZEND_HOT_COUNTERS_COUNT; i++) {
		zend_jit_hot_counters[i] = ZEND_JIT_COUNTER_INIT;
	}
}

ZEND_EXT_API void zend_jit_activate(void)
{
	zend_jit_profile_counter = 0;
	if (JIT_G(on)) {
		if (JIT_G(trigger) == ZEND_JIT_ON_HOT_COUNTERS) {
			zend_jit_reset_counters();
		} else if (JIT_G(trigger) == ZEND_JIT_ON_HOT_TRACE) {
			zend_jit_reset_counters();
			zend_jit_trace_reset_caches();
		}
	}
}

ZEND_EXT_API void zend_jit_deactivate(void)
{
	if (zend_jit_profile_counter) {
		zend_class_entry *ce;

		zend_shared_alloc_lock();
		SHM_UNPROTECT();
		zend_jit_unprotect();

		zend_jit_check_funcs(EG(function_table), 0);
		ZEND_HASH_MAP_REVERSE_FOREACH_PTR(EG(class_table), ce) {
			if (ce->type == ZEND_INTERNAL_CLASS) {
				break;
			}
			zend_jit_check_funcs(&ce->function_table, 1);
		} ZEND_HASH_FOREACH_END();

		zend_jit_protect();
		SHM_PROTECT();
		zend_shared_alloc_unlock();

		zend_jit_profile_counter = 0;
	}
}

static void zend_jit_restart_preloaded_op_array(zend_op_array *op_array)
{
	zend_func_info *func_info = ZEND_FUNC_INFO(op_array);

	if (!func_info) {
		return;
	}

	if (func_info->flags & ZEND_FUNC_JIT_ON_HOT_TRACE) {
		zend_jit_restart_hot_trace_counters(op_array);
	} else if (func_info->flags & ZEND_FUNC_JIT_ON_HOT_COUNTERS) {
		zend_jit_restart_hot_counters(op_array);
#if 0
	// TODO: We have to restore handlers for some inner basic-blocks, but we didn't store them ???
	} else if (func_info->flags & (ZEND_FUNC_JIT_ON_FIRST_EXEC|ZEND_FUNC_JIT_ON_PROF_REQUEST)) {
		zend_op *opline = op_array->opcodes;
		zend_jit_op_array_extension *jit_extension =
			(zend_jit_op_array_extension*)func_info;

		if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
			while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
				opline++;
			}
		}
		if (func_info->flags & ZEND_FUNC_JIT_ON_FIRST_EXEC) {
			opline->handler = (const void*)zend_jit_runtime_jit_handler;
		} else {
			opline->handler = (const void*)zend_jit_profile_jit_handler;
		}
#endif
	}
	if (op_array->num_dynamic_func_defs) {
		for (uint32_t i = 0; i < op_array->num_dynamic_func_defs; i++) {
			zend_jit_restart_preloaded_op_array(op_array->dynamic_func_defs[i]);
		}
	}
}

static void zend_jit_restart_preloaded_script(zend_persistent_script *script)
{
	zend_class_entry *ce;
	zend_op_array *op_array;

	zend_jit_restart_preloaded_op_array(&script->script.main_op_array);

	ZEND_HASH_MAP_FOREACH_PTR(&script->script.function_table, op_array) {
		zend_jit_restart_preloaded_op_array(op_array);
	} ZEND_HASH_FOREACH_END();

	ZEND_HASH_MAP_FOREACH_PTR(&script->script.class_table, ce) {
		ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, op_array) {
			if (op_array->type == ZEND_USER_FUNCTION) {
				zend_jit_restart_preloaded_op_array(op_array);
			}
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FOREACH_END();
}

ZEND_EXT_API void zend_jit_restart(void)
{
	if (dasm_buf) {
		zend_jit_unprotect();

#ifndef ZEND_JIT_IR //???
#if ZEND_JIT_TARGET_ARM64
		memset(dasm_labels_veneers, 0, sizeof(void*) * ZEND_MM_ALIGNED_SIZE_EX(zend_lb_MAX, DASM_ALIGNMENT));
#endif
#endif

		/* restore JIT buffer pos */
		dasm_ptr[0] = dasm_ptr[1];

		zend_jit_trace_restart();

		if (ZCSG(preload_script)) {
			zend_jit_restart_preloaded_script(ZCSG(preload_script));
			if (ZCSG(saved_scripts)) {
				zend_persistent_script **p = ZCSG(saved_scripts);

				while (*p) {
					zend_jit_restart_preloaded_script(*p);
					p++;
				}
			}
		}

		zend_jit_protect();

#ifdef HAVE_DISASM
		if (JIT_G(debug) & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_ASM_STUBS)) {
			zend_jit_disasm_shutdown();
			zend_jit_disasm_init();
		}
#endif
	}
}

#endif /* HAVE_JIT */
