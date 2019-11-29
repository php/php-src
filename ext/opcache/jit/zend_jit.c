/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include <ZendAccelerator.h>
#include "zend_shared_alloc.h"
#include "Zend/zend_execute.h"
#include "Zend/zend_vm.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_constants.h"
#include "zend_smart_str.h"
#include "jit/zend_jit.h"
#include "jit/zend_jit_internal.h"

#ifdef HAVE_JIT

#include "Optimizer/zend_func_info.h"
#include "Optimizer/zend_ssa.h"
#include "Optimizer/zend_inference.h"
#include "Optimizer/zend_call_graph.h"
#include "Optimizer/zend_dump.h"

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

#define DASM_M_FREE(ctx, p, sz)	efree(p)

#include "dynasm/dasm_proto.h"

typedef struct _zend_jit_stub {
	const char *name;
	int (*stub)(dasm_State **Dst);
} zend_jit_stub;

#define JIT_STUB(name) \
	{JIT_STUB_PREFIX #name, zend_jit_ ## name ## _stub}

static zend_uchar zend_jit_level = 0;
static zend_uchar zend_jit_trigger = 0;
static zend_uchar zend_jit_reg_alloc = 0;
static zend_uchar zend_jit_cpu_flags = 0;

zend_ulong zend_jit_profile_counter = 0;
int zend_jit_profile_counter_rid = -1;

int16_t zend_jit_hot_counters[ZEND_HOT_COUNTERS_COUNT];

const zend_op *zend_jit_halt_op = NULL;
static int zend_jit_vm_kind = 0;

static void *dasm_buf = NULL;
static void *dasm_end = NULL;
static void **dasm_ptr = NULL;

static size_t dasm_size = 0;

static zend_long jit_bisect_pos = 0;

static const void *zend_jit_runtime_jit_handler = NULL;
static const void *zend_jit_profile_jit_handler = NULL;
static const void *zend_jit_func_counter_handler = NULL;
static const void *zend_jit_loop_counter_handler = NULL;

static int zend_may_overflow(const zend_op *opline, const zend_op_array *op_array, zend_ssa *ssa);
static void ZEND_FASTCALL zend_runtime_jit(void);

static zend_bool zend_ssa_is_last_use(const zend_op_array *op_array, const zend_ssa *ssa, int var, int use)
{
	if (ssa->vars[var].phi_use_chain) {
		zend_ssa_phi *phi = ssa->vars[var].phi_use_chain;
		do {
			if (!ssa->vars[phi->ssa_var].no_val) {
				return 0;
			}
			phi = zend_ssa_next_use_phi(ssa, var, phi);
		} while (phi);
	}

	use = zend_ssa_next_use(ssa->ops, var, use);
	return use < 0 || zend_ssa_is_no_val_use(op_array->opcodes + use, ssa->ops + use, var);
}

static zend_bool zend_is_commutative(zend_uchar opcode)
{
	return
		opcode == ZEND_ADD ||
		opcode == ZEND_MUL ||
		opcode == ZEND_BW_OR ||
		opcode == ZEND_BW_AND ||
		opcode == ZEND_BW_XOR;
}

static zend_bool zend_long_is_power_of_two(zend_long x)
{
	return (x > 0) && !(x & (x - 1));
}

#define OP_RANGE(line, opN) \
	(((opline->opN##_type & (IS_TMP_VAR|IS_VAR|IS_CV)) && \
	  ssa->ops && \
	  ssa->var_info && \
	  ssa->ops[line].opN##_use >= 0 && \
	  ssa->var_info[ssa->ops[line].opN##_use].has_range) ? \
	 &ssa->var_info[ssa->ops[line].opN##_use].range : NULL)

#define OP1_RANGE()      OP_RANGE(opline - op_array->opcodes, op1)
#define OP2_RANGE()      OP_RANGE(opline - op_array->opcodes, op2)
#define OP1_DATA_RANGE() OP_RANGE(opline - op_array->opcodes + 1, op1)

#include "dynasm/dasm_x86.h"
#include "jit/zend_jit_x86.h"
#include "jit/zend_jit_helpers.c"
#include "jit/zend_jit_x86.c"
#include "jit/zend_jit_disasm_x86.c"
#ifndef _WIN32
#include "jit/zend_jit_gdb.c"
#include "jit/zend_jit_perf_dump.c"
#endif
#ifdef HAVE_OPROFILE
# include "jit/zend_jit_oprofile.c"
#endif
#include "jit/zend_jit_vtune.c"

#if _WIN32
# include <Windows.h>
#else
# include <sys/mman.h>
# if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#   define MAP_ANONYMOUS MAP_ANON
# endif
#endif

#define DASM_ALIGNMENT 16

ZEND_EXT_API void zend_jit_status(zval *ret)
{
	zval stats;
	array_init(&stats);
	add_assoc_long(&stats, "level", zend_jit_level);
	add_assoc_long(&stats, "trigger", zend_jit_trigger);
	add_assoc_long(&stats, "reg-alloc", zend_jit_reg_alloc);
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

static void *dasm_link_and_encode(dasm_State             **dasm_state,
                                  const zend_op_array     *op_array,
                                  zend_ssa                *ssa,
                                  const zend_op           *rt_opline,
                                  zend_lifetime_interval **ra,
                                  const char              *name)
{
	size_t size;
	int ret;
	void *entry;
#if defined(HAVE_DISASM) || defined(HAVE_GDB) || defined(HAVE_OPROFILE) || defined(HAVE_PERFTOOLS) || defined(HAVE_VTUNE)
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

	if (dasm_link(dasm_state, &size) != DASM_S_OK) {
		// TODO: dasm_link() failed ???
		return NULL;
	}

	if ((void*)((char*)*dasm_ptr + size) > dasm_end) {
		*dasm_ptr = dasm_end; //prevent further try
		// TODO: jit_buffer_size overflow ???
		return NULL;
	}

	ret = dasm_encode(dasm_state, *dasm_ptr);

	if (ret != DASM_S_OK) {
		// TODO: dasm_encode() failed ???
		return NULL;
	}

	entry = *dasm_ptr;
	*dasm_ptr = (void*)((char*)*dasm_ptr + ZEND_MM_ALIGNED_SIZE_EX(size, DASM_ALIGNMENT));

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

#if defined(HAVE_DISASM) || defined(HAVE_GDB) || defined(HAVE_OPROFILE) || defined(HAVE_PERFTOOLS) || defined(HAVE_VTUNE)
    if (!name) {
		if (ZCG(accel_directives).jit_debug & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_OPROFILE|ZEND_JIT_DEBUG_PERF|ZEND_JIT_DEBUG_VTUNE|ZEND_JIT_DEBUG_PERF_DUMP)) {
			str = zend_jit_func_name(op_array);
			if (str) {
				name = ZSTR_VAL(str);
			}
		}
#ifdef HAVE_DISASM
	    if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_ASM) {
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
	    if (ZCG(accel_directives).jit_debug & (ZEND_JIT_DEBUG_ASM_STUBS|ZEND_JIT_DEBUG_ASM)) {
			zend_jit_disasm_add_symbol(name, (uintptr_t)entry, size);
		    if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_ASM_STUBS) {
				zend_jit_disasm(
					name,
					(op_array && op_array->filename) ? ZSTR_VAL(op_array->filename) : NULL,
					op_array,
					&ssa->cfg,
					entry,
					size);
			}
		}
# endif
	}
#endif

#ifdef HAVE_GDB
	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_GDB) {
		if (name) {
			zend_jit_gdb_register(
					name,
					op_array,
					entry,
					size);
		}
	}
#endif

#ifdef HAVE_OPROFILE
	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_OPROFILE) {
		zend_jit_oprofile_register(
			name,
			entry,
			size);
	}
#endif

#ifdef HAVE_PERFTOOLS
	if (ZCG(accel_directives).jit_debug & (ZEND_JIT_DEBUG_PERF|ZEND_JIT_DEBUG_PERF_DUMP)) {
		if (name) {
			zend_jit_perf_map_register(
				name,
				entry,
				size);
			if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_PERF_DUMP) {
				zend_jit_perf_jitdump_register(
					name,
					entry,
					size);
			}
		}
	}
#endif

#ifdef HAVE_VTUNE
	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_VTUNE) {
		if (name) {
			zend_jit_vtune_register(
				name,
				entry,
				size);
		}
	}
#endif

#if defined(HAVE_DISASM) || defined(HAVE_GDB) || defined(HAVE_OPROFILE) || defined(HAVE_PERFTOOLS) || defined(HAVE_VTUNE)
	if (str) {
		zend_string_release(str);
	}
#endif

	return entry;
}

static int zend_may_overflow(const zend_op *opline, const zend_op_array *op_array, zend_ssa *ssa)
{
	uint32_t num;
	int res;

	if (!ssa->ops || !ssa->var_info) {
		return 1;
	}
	switch (opline->opcode) {
		case ZEND_PRE_INC:
		case ZEND_POST_INC:
			num = opline - op_array->opcodes;
			res = ssa->ops[num].op1_def;
			return (res < 0 ||
				!ssa->var_info[res].has_range ||
				ssa->var_info[res].range.overflow);
		case ZEND_PRE_DEC:
		case ZEND_POST_DEC:
			num = opline - op_array->opcodes;
			res = ssa->ops[num].op1_def;
			return (res < 0 ||
				!ssa->var_info[res].has_range ||
				ssa->var_info[res].range.underflow);
		case ZEND_ADD:
			num = opline - op_array->opcodes;
			res = ssa->ops[num].result_def;
			if (res < 0 ||
			    !ssa->var_info[res].has_range) {
				return 1;
			}
			if (ssa->var_info[res].range.underflow) {
				zend_long op1_min, op2_min;

				if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
					return 1;
				}
				op1_min = OP1_MIN_RANGE();
				op2_min = OP2_MIN_RANGE();
				if (zend_add_will_overflow(op1_min, op2_min)) {
					return 1;
				}
			}
			if (ssa->var_info[res].range.overflow) {
				zend_long op1_max, op2_max;

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
			num = opline - op_array->opcodes;
			res = ssa->ops[num].result_def;
			if (res < 0 ||
			    !ssa->var_info[res].has_range) {
				return 1;
			}
			if (ssa->var_info[res].range.underflow) {
				zend_long op1_min, op2_max;

				if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
					return 1;
				}
				op1_min = OP1_MIN_RANGE();
				op2_max = OP2_MAX_RANGE();
				if (zend_sub_will_overflow(op1_min, op2_max)) {
					return 1;
				}
			}
			if (ssa->var_info[res].range.overflow) {
				zend_long op1_max, op2_min;

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
			num = opline - op_array->opcodes;
			res = ssa->ops[num].result_def;
			return (res < 0 ||
				!ssa->var_info[res].has_range ||
				ssa->var_info[res].range.underflow ||
				ssa->var_info[res].range.overflow);
		case ZEND_ASSIGN_OP:
			if (opline->extended_value == ZEND_ADD) {
				num = opline - op_array->opcodes;
				res = ssa->ops[num].op1_def;
				if (res < 0 ||
				    !ssa->var_info[res].has_range) {
					return 1;
				}
				if (ssa->var_info[res].range.underflow) {
					zend_long op1_min, op2_min;

					if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
						return 1;
					}
					op1_min = OP1_MIN_RANGE();
					op2_min = OP2_MIN_RANGE();
					if (zend_add_will_overflow(op1_min, op2_min)) {
						return 1;
					}
				}
				if (ssa->var_info[res].range.overflow) {
					zend_long op1_max, op2_max;

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
				num = opline - op_array->opcodes;
				res = ssa->ops[num].op1_def;
				if (res < 0 ||
				    !ssa->var_info[res].has_range) {
					return 1;
				}
				if (ssa->var_info[res].range.underflow) {
					zend_long op1_min, op2_max;

					if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
						return 1;
					}
					op1_min = OP1_MIN_RANGE();
					op2_max = OP2_MAX_RANGE();
					if (zend_sub_will_overflow(op1_min, op2_max)) {
						return 1;
					}
				}
				if (ssa->var_info[res].range.overflow) {
					zend_long op1_max, op2_min;

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
				num = opline - op_array->opcodes;
				res = ssa->ops[num].op1_def;
				return (res < 0 ||
					!ssa->var_info[res].has_range ||
					ssa->var_info[res].range.underflow ||
					ssa->var_info[res].range.overflow);
			}
		default:
			return 1;
	}
}

static int zend_jit_build_cfg(const zend_op_array *op_array, zend_cfg *cfg)
{
	uint32_t flags;

	flags = ZEND_CFG_STACKLESS | ZEND_CFG_NO_ENTRY_PREDECESSORS | ZEND_SSA_RC_INFERENCE_FLAG | ZEND_SSA_USE_CV_RESULTS | ZEND_CFG_RECV_ENTRY;

	if (zend_build_cfg(&CG(arena), op_array, flags, cfg) != SUCCESS) {
		return FAILURE;
	}

	/* Don't JIT huge functions. Apart from likely being detrimental due to the amount of
	 * generated code, some of our analysis is recursive and will stack overflow with many
	 * blocks. */
	if (cfg->blocks_count > 100000) {
		return FAILURE;
	}

	if (zend_cfg_build_predecessors(&CG(arena), cfg) != SUCCESS) {
		return FAILURE;
	}

	/* Compute Dominators Tree */
	if (zend_cfg_compute_dominators_tree(op_array, cfg) != SUCCESS) {
		return FAILURE;
	}

	/* Identify reducible and irreducible loops */
	if (zend_cfg_identify_loops(op_array, cfg) != SUCCESS) {
		return FAILURE;
	}

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

	if ((zend_jit_level >= ZEND_JIT_LEVEL_OPT_FUNC)
	 && ssa->cfg.blocks
	 && op_array->last_try_catch == 0
	 && !(op_array->fn_flags & ZEND_ACC_GENERATOR)
	 && !(ssa->cfg.flags & ZEND_FUNC_INDIRECT_VAR_ACCESS)) {
		if (zend_build_ssa(&CG(arena), script, op_array, ZEND_SSA_RC_INFERENCE | ZEND_SSA_USE_CV_RESULTS, ssa) != SUCCESS) {
			return FAILURE;
		}

		if (zend_ssa_compute_use_def_chains(&CG(arena), op_array, ssa) != SUCCESS) {
			return FAILURE;
		}

		if (zend_ssa_find_false_dependencies(op_array, ssa) != SUCCESS) {
			return FAILURE;
		}

		if (zend_ssa_find_sccs(op_array, ssa) != SUCCESS){
			return FAILURE;
		}
	}

	return SUCCESS;
}

static int zend_jit_op_array_analyze2(const zend_op_array *op_array, zend_script *script, zend_ssa *ssa)
{
	if ((zend_jit_level >= ZEND_JIT_LEVEL_OPT_FUNC)
	 && ssa->cfg.blocks
	 && op_array->last_try_catch == 0
	 && !(op_array->fn_flags & ZEND_ACC_GENERATOR)
	 && !(ssa->cfg.flags & ZEND_FUNC_INDIRECT_VAR_ACCESS)) {

		uint32_t optimization_level = ZCG(accel_directives).optimization_level;
		if (zend_ssa_inference(&CG(arena), op_array, script, ssa, optimization_level) != SUCCESS) {
			return FAILURE;
		}
	}

	return SUCCESS;
}

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
		ival->split = 0;
		ival->store = 0;
		ival->load = 0;
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

	current->store = 1;

	ival->ssa_var = current->ssa_var;
	ival->reg     = ZREG_NONE;
	ival->split   = 1;
	ival->store   = 0;
	ival->load    = 1;
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
			     ival->range.end > last->range.end)) {
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
					            ival->range.end < (*p)->range.end)) {
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

static zend_bool zend_jit_in_loop(zend_ssa *ssa, int header, zend_basic_block *b)
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
	while (1) {
		if (intervals[dst]->hint) {
			if (intervals[dst]->hint->range.start < intervals[src]->range.start) {
				int tmp = src;
				src = intervals[dst]->hint->ssa_var;
				dst = tmp;
			} else {
				dst = intervals[dst]->hint->ssa_var;
			}
		} else {
			if (dst != src) {
				intervals[dst]->hint = intervals[src];
			}
			return;
		}
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

	if (zend_jit_reg_alloc >= ZEND_JIT_REG_ALLOC_GLOBAL) {
		/* Register hinting (a cheap way for register coalescing) */
		for (i = 0; i < ssa->vars_count; i++) {
			if (intervals[i]) {
				int var = intervals[i]->ssa_var;
				int src;

				if (ssa->vars[var].definition_phi) {
					zend_ssa_phi *phi = ssa->vars[var].definition_phi;

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
									/* Skip zero-lenght interval for Pi variable */
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
				int var = intervals[i]->ssa_var;

				if (ssa->vars[var].definition >= 0) {
					uint32_t line = ssa->vars[var].definition;
					const zend_op *opline = op_array->opcodes + line;

					switch (opline->opcode) {
						case ZEND_QM_ASSIGN:
						case ZEND_POST_INC:
						case ZEND_POST_DEC:
							if (ssa->ops[line].op1_use >= 0 &&
							    intervals[ssa->ops[line].op1_use] &&
							    (var == ssa->ops[line].op1_def ||
							     (var == ssa->ops[line].result_def &&
							      (ssa->ops[line].op1_def < 0 ||
							       !intervals[ssa->ops[line].op1_def])))) {
								zend_jit_add_hint(intervals, i, ssa->ops[line].op1_use);
							}
							break;
						case ZEND_SEND_VAR:
						case ZEND_PRE_INC:
						case ZEND_PRE_DEC:
							if (var == ssa->ops[line].op1_def &&
							    ssa->ops[line].op1_use >= 0 &&
							    intervals[ssa->ops[line].op1_use]) {
								zend_jit_add_hint(intervals, i, ssa->ops[line].op1_use);
							}
							break;
						case ZEND_ASSIGN:
							if (ssa->ops[line].op2_use >= 0 &&
							    intervals[ssa->ops[line].op2_use] &&
							    (var == ssa->ops[line].op2_def ||
								 (var == ssa->ops[line].op1_def &&
							      (ssa->ops[line].op2_def < 0 ||
							       !intervals[ssa->ops[line].op2_def])) ||
								 (var == ssa->ops[line].result_def &&
							      (ssa->ops[line].op2_def < 0 ||
							       !intervals[ssa->ops[line].op2_def]) &&
							      (ssa->ops[line].op1_def < 0 ||
							       !intervals[ssa->ops[line].op1_def])))) {
								zend_jit_add_hint(intervals, i, ssa->ops[line].op2_use);
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

static zend_bool zend_interval_covers(zend_lifetime_interval *ival, uint32_t position)
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
static int zend_jit_try_allocate_free_reg(const zend_op_array *op_array, zend_ssa *ssa, zend_lifetime_interval *current, zend_regset available, zend_regset *hints, zend_lifetime_interval *active, zend_lifetime_interval *inactive, zend_lifetime_interval **list, zend_lifetime_interval **free)
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
	 * Their values have to be stored in prologuee and restored in epilogue
	 */
	available = ZEND_REGSET_DIFFERENCE(available, ZEND_REGSET_PRESERVED);

	if (ZEND_REGSET_IS_EMPTY(available)) {
		return 0;
	}

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
			} else if (zend_jit_may_reuse_reg(op_array, ssa, current->range.start, current->ssa_var, it->ssa_var)) {
				if (!ZEND_REGSET_IN(*hints, it->reg) &&
				    /* TODO: Avoid most often scratch registers. Find a better way ??? */
				    (!current->used_as_hint ||
				     (it->reg != ZREG_R0 && it->reg != ZREG_R1 && it->reg != ZREG_XMM0 && it->reg != ZREG_XMM1))) {
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

	/* See "Linear Scan Register Allocation on SSA Form", Christian Wimmer and
	   Michael Franz, CGO'10 (2010), Figure 6. */
	if (current->split) {
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
		zend_regset regset;
		zend_reg reg;

		if (ssa->ops[line].op1_def == current->ssa_var ||
		    ssa->ops[line].op2_def == current->ssa_var ||
		    ssa->ops[line].result_def == current->ssa_var) {
			line++;
		}
		while (line <= range->end) {
			regset = zend_jit_get_scratch_regset(op_array, ssa, line, current->ssa_var);
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
	/* Coalesing */
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

	pos = 0; reg = ZREG_NONE;
	pos2 = 0; reg2 = ZREG_NONE;
	low_priority_regs = *hints;
	if (current->used_as_hint) {
		/* TODO: Avoid most often scratch registers. Find a better way ??? */
		ZEND_REGSET_INCL(low_priority_regs, ZREG_R0);
		ZEND_REGSET_INCL(low_priority_regs, ZREG_R1);
		ZEND_REGSET_INCL(low_priority_regs, ZREG_XMM0);
		ZEND_REGSET_INCL(low_priority_regs, ZREG_XMM1);
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
	// TODO: allow low prioirity register usage
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
static zend_lifetime_interval* zend_jit_linear_scan(const zend_op_array *op_array, zend_ssa *ssa, zend_lifetime_interval *list)
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

		if (zend_jit_try_allocate_free_reg(op_array, ssa, current, available, &hints, active, inactive, &unhandled, &free) ||
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
		if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_REG_ALLOC) {
			fprintf(stderr, "Live Ranges \"%s\"\n", op_array->function_name ? ZSTR_VAL(op_array->function_name) : "[main]");
			ival = list;
			while (ival) {
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
				if (ival->load) {
					fprintf(stderr, " load");
				}
				if (ival->store) {
					fprintf(stderr, " store");
				}
				if (ival->hint) {
					var_num = ssa->vars[ival->hint->ssa_var].var;
					fprintf(stderr, " hint=#%d.", ival->hint->ssa_var);
					zend_dump_var(op_array, (var_num < op_array->last_var ? IS_CV : 0), var_num);
				}
				fprintf(stderr, "\n");
				ival = ival->list_next;
			}
			fprintf(stderr, "\n");
		}

		/* Linear Scan Register Allocation */
		list = zend_jit_linear_scan(op_array, ssa, list);

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

			if (zend_jit_reg_alloc >= ZEND_JIT_REG_ALLOC_GLOBAL) {
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
										intervals[i]->load = 1;
									} else if (intervals[i]->reg != intervals[src]->reg) {
										intervals[i]->load = 1;
										intervals[src]->store = 1;
									}
								} else if (intervals[src]) {
									intervals[src]->store = 1;
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
										/* Skip zero-lenght interval for Pi variable */
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
									intervals[i]->load = 1;
								}
								for (k = 0; k < ssa->cfg.blocks[phi->block].predecessors_count; k++) {
									src = phi->sources[k];
									if (src >= 0) {
										if (ssa->vars[src].definition_phi
										 && ssa->vars[src].definition_phi->pi >= 0
										 && phi->block == ssa->vars[src].definition_phi->block) {
											/* Skip zero-lenght interval for Pi variable */
											src = ssa->vars[src].definition_phi->sources[0];
										}
										if (intervals[src]) {
											intervals[src]->store = 1;
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
					    (intervals[i]->load ||
					     (intervals[i]->store && ssa->vars[i].definition >= 0)) &&
					    ssa->vars[i].use_chain < 0) {
					    zend_bool may_remove = 1;
						zend_ssa_phi *phi = ssa->vars[i].phi_use_chain;

						while (phi) {
							if (intervals[phi->ssa_var] &&
							    !intervals[phi->ssa_var]->load) {
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
					    intervals[i]->load &&
					    intervals[i]->store &&
					    (ssa->vars[i].use_chain < 0 ||
					     zend_ssa_next_use(ssa->ops, i, ssa->vars[i].use_chain) < 0)) {
						zend_bool may_remove = 1;
						zend_ssa_phi *phi = ssa->vars[i].phi_use_chain;

						while (phi) {
							if (intervals[phi->ssa_var] &&
							    !intervals[phi->ssa_var]->load) {
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

			if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_REG_ALLOC) {
				fprintf(stderr, "Allocated Live Ranges \"%s\"\n", op_array->function_name ? ZSTR_VAL(op_array->function_name) : "[main]");
				for (i = 0; i < ssa->vars_count; i++) {
					ival = intervals[i];
					while (ival) {
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
						fprintf(stderr, " (%s)", zend_reg_name[ival->reg]);
						if (ival->load) {
							fprintf(stderr, " load");
						}
						if (ival->store) {
							fprintf(stderr, " store");
						}
						if (ival->hint) {
							var_num = ssa->vars[ival->hint->ssa_var].var;
							fprintf(stderr, " hint=#%d.", ival->hint->ssa_var);
							zend_dump_var(op_array, (var_num < op_array->last_var ? IS_CV : 0), var_num);
							if (ival->hint->reg != ZREG_NONE) {
								fprintf(stderr, " (%s)", zend_reg_name[ival->hint->reg]);
							}
						}
						fprintf(stderr, "\n");
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

static int zend_jit(const zend_op_array *op_array, zend_ssa *ssa, const zend_op *rt_opline)
{
	int b, i, end;
	zend_op *opline;
	dasm_State* dasm_state = NULL;
	void *handler;
	int call_level = 0;
	void *checkpoint = NULL;
	zend_lifetime_interval **ra = NULL;
	zend_bool is_terminated = 1; /* previous basic block is terminated by jump */
	zend_bool recv_emitted = 0;   /* emitted at least one RECV opcode */
	zend_uchar smart_branch_opcode;
	uint32_t target_label, target_label2;
	uint32_t op1_info, op1_def_info, op2_info, res_info, res_use_info;
	zend_bool send_result;
	zend_jit_addr op1_addr, op1_def_addr, op2_addr, op2_def_addr, res_addr;
	zend_class_entry *ce;

	if (ZCG(accel_directives).jit_bisect_limit) {
		jit_bisect_pos++;
		if (jit_bisect_pos >= ZCG(accel_directives).jit_bisect_limit) {
			if (jit_bisect_pos == ZCG(accel_directives).jit_bisect_limit) {
				fprintf(stderr, "Not JITing %s%s%s in %s:%d and after due to jit_bisect_limit\n",
					op_array->scope ? ZSTR_VAL(op_array->scope->name) : "",
					op_array->scope ? "::" : "",
					op_array->function_name ? ZSTR_VAL(op_array->function_name) : "{main}",
					ZSTR_VAL(op_array->filename), op_array->line_start);
			}
			return FAILURE;
		}
	}

	if (zend_jit_reg_alloc) {
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

	dasm_init(&dasm_state, DASM_MAXSECTION);
	dasm_setupglobal(&dasm_state, dasm_labels, zend_lb_MAX);
	dasm_setup(&dasm_state, dasm_actions);

	dasm_growpc(&dasm_state, ssa->cfg.blocks_count * 2 + 1);

	zend_jit_align_func(&dasm_state);
	for (b = 0; b < ssa->cfg.blocks_count; b++) {
		if ((ssa->cfg.blocks[b].flags & ZEND_BB_REACHABLE) == 0) {
			continue;
		}
//#ifndef CONTEXT_THREADED_JIT
		if (ssa->cfg.blocks[b].flags & ZEND_BB_ENTRY) {
			if (ssa->cfg.blocks[b].flags & ZEND_BB_TARGET) {
				/* pass */
			} else if (zend_jit_level < ZEND_JIT_LEVEL_INLINE &&
			           ssa->cfg.blocks[b].len == 1 &&
			           (ssa->cfg.blocks[b].flags & ZEND_BB_EXIT) &&
			           op_array->opcodes[ssa->cfg.blocks[b].start].opcode != ZEND_JMP) {
				/* don't generate code for BB with single opcode */
				continue;
			}
			if (ssa->cfg.blocks[b].flags & ZEND_BB_FOLLOW) {
				if (!is_terminated) {
					zend_jit_jmp(&dasm_state, b);
				}
			}
			zend_jit_label(&dasm_state, ssa->cfg.blocks_count + b);
			zend_jit_prologue(&dasm_state);
		} else
//#endif
		if (ssa->cfg.blocks[b].flags & (ZEND_BB_START|ZEND_BB_RECV_ENTRY)) {
			opline = op_array->opcodes + ssa->cfg.blocks[b].start;
			if (ssa->cfg.flags & ZEND_CFG_RECV_ENTRY) {
				if (opline->opcode == ZEND_RECV_INIT) {
					if (opline == op_array->opcodes ||
					    (opline-1)->opcode != ZEND_RECV_INIT) {
						if (recv_emitted) {
							zend_jit_jmp(&dasm_state, b);
						}
						zend_jit_label(&dasm_state, ssa->cfg.blocks_count + b);
						for (i = 1; (opline+i)->opcode == ZEND_RECV_INIT; i++) {
							zend_jit_label(&dasm_state, ssa->cfg.blocks_count + b + i);
						}
						zend_jit_prologue(&dasm_state);
					}
					recv_emitted = 1;
				} else if (opline->opcode == ZEND_RECV) {
					if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
						/* skip */
						continue;
					} else if (recv_emitted) {
						zend_jit_jmp(&dasm_state, b);
						zend_jit_label(&dasm_state, ssa->cfg.blocks_count + b);
						zend_jit_prologue(&dasm_state);
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
						zend_jit_label(&dasm_state, ssa->cfg.blocks_count + b);
						zend_jit_prologue(&dasm_state);
						recv_emitted = 1;
					}
				} else {
					if (recv_emitted) {
						zend_jit_jmp(&dasm_state, b);
					} else if (zend_jit_level < ZEND_JIT_LEVEL_INLINE &&
					           ssa->cfg.blocks[b].len == 1 &&
					           (ssa->cfg.blocks[b].flags & ZEND_BB_EXIT)) {
						/* don't generate code for BB with single opcode */
						dasm_free(&dasm_state);

						if (zend_jit_reg_alloc) {
							zend_arena_release(&CG(arena), checkpoint);
						}
						return SUCCESS;
					}
					zend_jit_label(&dasm_state, ssa->cfg.blocks_count + b);
					zend_jit_prologue(&dasm_state);
					recv_emitted = 1;
				}
			} else if (zend_jit_level < ZEND_JIT_LEVEL_INLINE &&
			           ssa->cfg.blocks[b].len == 1 &&
			           (ssa->cfg.blocks[b].flags & ZEND_BB_EXIT)) {
				/* don't generate code for BB with single opcode */
				dasm_free(&dasm_state);

				if (zend_jit_reg_alloc) {
					zend_arena_release(&CG(arena), checkpoint);
				}
				return SUCCESS;
			} else {
				zend_jit_label(&dasm_state, ssa->cfg.blocks_count + b);
				zend_jit_prologue(&dasm_state);
			}
		}

		is_terminated = 0;

		zend_jit_label(&dasm_state, b);
		if (zend_jit_level < ZEND_JIT_LEVEL_INLINE) {
			if ((ssa->cfg.blocks[b].flags & ZEND_BB_FOLLOW)
			  && ssa->cfg.blocks[b].start != 0
			  && (op_array->opcodes[ssa->cfg.blocks[b].start - 1].opcode == ZEND_NOP
			   || op_array->opcodes[ssa->cfg.blocks[b].start - 1].opcode == ZEND_SWITCH_LONG
			   || op_array->opcodes[ssa->cfg.blocks[b].start - 1].opcode == ZEND_SWITCH_STRING)) {
				if (!zend_jit_reset_opline(&dasm_state, op_array->opcodes + ssa->cfg.blocks[b].start)
				 || !zend_jit_set_valid_ip(&dasm_state, op_array->opcodes + ssa->cfg.blocks[b].start)) {
					goto jit_failure;
				}
			} else {
				if (!zend_jit_set_opline(&dasm_state, op_array->opcodes + ssa->cfg.blocks[b].start)) {
					goto jit_failure;
				}
			}
		} else if (ssa->cfg.blocks[b].flags & ZEND_BB_TARGET) {
			if (!zend_jit_reset_opline(&dasm_state, op_array->opcodes + ssa->cfg.blocks[b].start)) {
				goto jit_failure;
			}
		} else if (ssa->cfg.blocks[b].flags & (ZEND_BB_START|ZEND_BB_RECV_ENTRY|ZEND_BB_ENTRY)) {
			if (!zend_jit_set_opline(&dasm_state, op_array->opcodes + ssa->cfg.blocks[b].start)) {
				goto jit_failure;
			}
		}
		if (ssa->cfg.blocks[b].flags & ZEND_BB_LOOP_HEADER) {
			if (!zend_jit_check_timeout(&dasm_state, op_array->opcodes + ssa->cfg.blocks[b].start)) {
				goto jit_failure;
			}
		}
		if (!ssa->cfg.blocks[b].len) {
			continue;
		}
		if ((zend_jit_reg_alloc >= ZEND_JIT_REG_ALLOC_GLOBAL) && ra) {
			zend_ssa_phi *phi = ssa->blocks[b].phis;

			while (phi) {
				zend_lifetime_interval *ival = ra[phi->ssa_var];

				if (ival) {
					if (ival->load) {
						ZEND_ASSERT(ival->reg != ZREG_NONE);

						if (!zend_jit_load_var(&dasm_state, ssa->var_info[phi->ssa_var].type, ssa->vars[phi->ssa_var].var, ival->reg)) {
							goto jit_failure;
						}
					} else if (ival->store) {
						ZEND_ASSERT(ival->reg != ZREG_NONE);

						if (!zend_jit_store_var(&dasm_state, ssa->var_info[phi->ssa_var].type, ssa->vars[phi->ssa_var].var, ival->reg)) {
							goto jit_failure;
						}
					}
				}
				phi = phi->next;
			}
		}
		end = ssa->cfg.blocks[b].start + ssa->cfg.blocks[b].len - 1;
		for (i = ssa->cfg.blocks[b].start; i <= end; i++) {
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

			if (zend_jit_level >= ZEND_JIT_LEVEL_INLINE) {
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
							res_use_info = RES_USE_INFO();
							res_info = RES_INFO();
							res_addr = RES_REG_ADDR();
						} else {
							res_use_info = -1;
							res_info = -1;
							res_addr = 0;
						}
						op1_def_info = OP1_DEF_INFO();
						if (!zend_jit_inc_dec(&dasm_state, opline, op_array,
								op1_info, OP1_REG_ADDR(),
								op1_def_info, OP1_DEF_REG_ADDR(),
								res_use_info, res_info,
								res_addr,
								(op1_def_info & MAY_BE_LONG) && (op1_def_info & MAY_BE_DOUBLE) && zend_may_overflow(opline, op_array, ssa))) {
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
						if ((op1_info & MAY_BE_UNDEF) || (op2_info & MAY_BE_UNDEF)) {
							break;
						}
						if (!(op1_info & MAY_BE_LONG)
						 || !(op2_info & MAY_BE_LONG)) {
							break;
						}
						if (opline->result_type == IS_TMP_VAR
						 && (i + 1) <= end
						 && (opline+1)->opcode == ZEND_SEND_VAL
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							i++;
							send_result = 1;
							res_use_info = -1;
							res_addr = 0; /* set inside backend */
						} else {
							send_result = 0;
							res_use_info = RES_USE_INFO();
							res_addr = RES_REG_ADDR();
						}
						if (!zend_jit_long_math(&dasm_state, opline, op_array,
								op1_info, OP1_RANGE(), OP1_REG_ADDR(),
								op2_info, OP2_RANGE(), OP2_REG_ADDR(),
								res_use_info, RES_INFO(), res_addr,
								send_result,
								zend_may_throw(opline, op_array, ssa))) {
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
						if (!(op1_info & (MAY_BE_LONG|MAY_BE_DOUBLE)) ||
						    !(op2_info & (MAY_BE_LONG|MAY_BE_DOUBLE))) {
							break;
						}
						if (opline->result_type == IS_TMP_VAR
						 && (i + 1) <= end
						 && (opline+1)->opcode == ZEND_SEND_VAL
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							i++;
							send_result = 1;
							res_use_info = -1;
							res_addr = 0; /* set inside backend */
						} else {
							send_result = 0;
							res_use_info = RES_USE_INFO();
							res_addr = RES_REG_ADDR();
						}
						res_info = RES_INFO();
						if (!zend_jit_math(&dasm_state, opline, op_array,
								op1_info, OP1_REG_ADDR(),
								op2_info, OP2_REG_ADDR(),
								res_use_info, res_info, res_addr,
								send_result,
								(res_info & MAY_BE_LONG) && (res_info & MAY_BE_DOUBLE) && zend_may_overflow(opline, op_array, ssa),
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
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
						if (opline->result_type == IS_TMP_VAR
						 && (i + 1) <= end
						 && (opline+1)->opcode == ZEND_SEND_VAL
						 && (opline+1)->op1_type == IS_TMP_VAR
						 && (opline+1)->op1.var == opline->result.var) {
							i++;
							send_result = 1;
						} else {
							send_result = 0;
						}
						if (!zend_jit_concat(&dasm_state, opline, op_array,
								op1_info, op2_info, RES_INFO(), send_result,
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN_OP:
						if (opline->extended_value == ZEND_POW
						 || opline->extended_value == ZEND_DIV) {
							// TODO: check for division by zero ???
							break;
						}
						if (opline->op1_type != IS_CV || opline->result_type != IS_UNUSED) {
							break;
						}
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						op1_info = OP1_INFO();
						op2_info = OP2_INFO();
						if ((op1_info & MAY_BE_UNDEF) || (op2_info & MAY_BE_UNDEF)) {
							break;
						}
						if (opline->extended_value == ZEND_ADD
						 || opline->extended_value == ZEND_SUB
						 || opline->extended_value == ZEND_MUL
						 || opline->extended_value == ZEND_DIV) {
							if (!(op1_info & (MAY_BE_LONG|MAY_BE_DOUBLE))
							 || !(op2_info & (MAY_BE_LONG|MAY_BE_DOUBLE))) {
								break;
							}
						} else if (opline->extended_value == ZEND_BW_OR
						 || opline->extended_value == ZEND_BW_AND
						 || opline->extended_value == ZEND_BW_XOR
						 || opline->extended_value == ZEND_SL
						 || opline->extended_value == ZEND_SR
						 || opline->extended_value == ZEND_MOD) {
							if (!(op1_info & MAY_BE_LONG)
							 || !(op2_info & MAY_BE_LONG)) {
								break;
							}
						} else if (opline->extended_value == ZEND_CONCAT) {
							if (!(op1_info & MAY_BE_STRING)
							 || !(op2_info & MAY_BE_STRING)) {
								break;
							}
						}
						op1_def_info = OP1_DEF_INFO();
						if (!zend_jit_assign_op(&dasm_state, opline, op_array,
								op1_info, op1_def_info, OP1_RANGE(),
								op2_info, OP2_RANGE(),
								(op1_def_info & MAY_BE_LONG) && (op1_def_info & MAY_BE_DOUBLE) && zend_may_overflow(opline, op_array, ssa),
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN_DIM_OP:
						if (opline->extended_value == ZEND_POW
						 || opline->extended_value == ZEND_DIV) {
							// TODO: check for division by zero ???
							break;
						}
						if (opline->op1_type != IS_CV || opline->result_type != IS_UNUSED) {
							break;
						}
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						if (!zend_jit_assign_dim_op(&dasm_state, opline, op_array,
								OP1_INFO(), OP1_DEF_INFO(), OP2_INFO(),
								OP1_DATA_INFO(), OP1_DATA_RANGE(),
								zend_may_throw(opline, op_array, ssa))) {
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
						if (!zend_jit_assign_dim(&dasm_state, opline, op_array,
								OP1_INFO(), OP2_INFO(), OP1_DATA_INFO(),
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN:
						if (opline->op1_type != IS_CV) {
							break;
						}
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						if (opline->result_type == IS_UNUSED) {
							res_addr = 0;
							res_info = -1;
						} else {
							res_addr = RES_REG_ADDR();
							res_info = RES_INFO();
						}
						op2_addr = OP2_REG_ADDR();
						if (ra
						 && ssa->ops[opline - op_array->opcodes].op2_def >= 0
						 && !ssa->vars[ssa->ops[opline - op_array->opcodes].op2_def].no_val) {
							op2_def_addr = OP2_DEF_REG_ADDR();
						} else {
							op2_def_addr = op2_addr;
						}
						if (!zend_jit_assign(&dasm_state, opline, op_array,
								OP1_INFO(), OP1_REG_ADDR(),
								OP1_DEF_INFO(), OP1_DEF_REG_ADDR(),
								OP2_INFO(), op2_addr, op2_def_addr,
								res_info, res_addr,
								zend_may_throw(opline, op_array, ssa))) {
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
						if (!zend_jit_qm_assign(&dasm_state, opline, op_array,
								OP1_INFO(), op1_addr, op1_def_addr,
								RES_INFO(), RES_REG_ADDR())) {
							goto jit_failure;
						}
						goto done;
					case ZEND_INIT_FCALL:
					case ZEND_INIT_FCALL_BY_NAME:
						if (!zend_jit_init_fcall(&dasm_state, opline, b, op_array, ssa, call_level)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SEND_VAL:
					case ZEND_SEND_VAL_EX:
						if (opline->opcode == ZEND_SEND_VAL_EX
						 && opline->op2.num > MAX_ARG_FLAG_NUM) {
							break;
						}
						if (!zend_jit_send_val(&dasm_state, opline, op_array,
								OP1_INFO(), OP1_REG_ADDR())) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SEND_REF:
						if (!zend_jit_send_ref(&dasm_state, opline, op_array,
								OP1_INFO(), 0)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SEND_VAR:
					case ZEND_SEND_VAR_EX:
					case ZEND_SEND_VAR_NO_REF:
					case ZEND_SEND_VAR_NO_REF_EX:
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
						if (!zend_jit_send_var(&dasm_state, opline, op_array,
								OP1_INFO(), op1_addr, op1_def_addr)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_DO_UCALL:
						is_terminated = 1;
						/* break missing intentionally */
					case ZEND_DO_ICALL:
					case ZEND_DO_FCALL_BY_NAME:
					case ZEND_DO_FCALL:
						if (!zend_jit_do_fcall(&dasm_state, opline, op_array, ssa, call_level, b + 1)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_IS_EQUAL:
					case ZEND_IS_NOT_EQUAL:
					case ZEND_IS_SMALLER:
					case ZEND_IS_SMALLER_OR_EQUAL:
					case ZEND_CASE:
						if ((opline->result_type & IS_TMP_VAR)
						 && (i + 1) <= end
						 && ((opline+1)->opcode == ZEND_JMPZ
						  || (opline+1)->opcode == ZEND_JMPNZ
						  || (opline+1)->opcode == ZEND_JMPZ_EX
						  || (opline+1)->opcode == ZEND_JMPNZ_EX
						  || (opline+1)->opcode == ZEND_JMPZNZ)
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
						if (!zend_jit_cmp(&dasm_state, opline, op_array,
								OP1_INFO(), OP1_REG_ADDR(),
								OP2_INFO(), OP2_REG_ADDR(),
								RES_REG_ADDR(),
								zend_may_throw(opline, op_array, ssa),
								smart_branch_opcode, target_label, target_label2)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_IS_IDENTICAL:
					case ZEND_IS_NOT_IDENTICAL:
						if ((opline->result_type & IS_TMP_VAR)
						 && (i + 1) <= end
						 && ((opline+1)->opcode == ZEND_JMPZ
						  || (opline+1)->opcode == ZEND_JMPNZ
						  || (opline+1)->opcode == ZEND_JMPZNZ)
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
						if (!zend_jit_identical(&dasm_state, opline, op_array,
								OP1_INFO(), OP1_REG_ADDR(),
								OP2_INFO(), OP2_REG_ADDR(),
								RES_REG_ADDR(),
								zend_may_throw(opline, op_array, ssa),
								smart_branch_opcode, target_label, target_label2)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_DEFINED:
						if ((opline->result_type & IS_TMP_VAR)
						 && (i + 1) <= end
						 && ((opline+1)->opcode == ZEND_JMPZ
						  || (opline+1)->opcode == ZEND_JMPNZ
						  || (opline+1)->opcode == ZEND_JMPZNZ)
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
						if (!zend_jit_defined(&dasm_state, opline, op_array, smart_branch_opcode, target_label, target_label2)) {
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
						  || (opline+1)->opcode == ZEND_JMPNZ
						  || (opline+1)->opcode == ZEND_JMPZNZ)
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
						if (!zend_jit_type_check(&dasm_state, opline, op_array, OP1_INFO(), smart_branch_opcode, target_label, target_label2)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_RETURN:
						op1_info = OP1_INFO();
						if ((PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info))
						 || op_array->type == ZEND_EVAL_CODE
						 // TODO: support for top-level code
						 || !op_array->function_name
						 // TODO: support for IS_UNDEF ???
						 || (op1_info & MAY_BE_UNDEF)) {
							if (!zend_jit_tail_handler(&dasm_state, opline)) {
								goto jit_failure;
							}
						} else if (!zend_jit_return(&dasm_state, opline, op_array, ssa,
								op1_info, OP1_REG_ADDR())) {
							goto jit_failure;
						}
						goto done;
					case ZEND_BOOL:
					case ZEND_BOOL_NOT:
						if (!zend_jit_bool_jmpznz(&dasm_state, opline, op_array,
								OP1_INFO(), OP1_REG_ADDR(), RES_REG_ADDR(),
								-1, -1,
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_JMPZ:
					case ZEND_JMPNZ:
						if (opline > op_array->opcodes + ssa->cfg.blocks[b].start &&
						    ((opline-1)->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
							/* smart branch */
							if (!zend_jit_cond_jmp(&dasm_state, opline + 1, ssa->cfg.blocks[b].successors[0])) {
								goto jit_failure;
							}
							goto done;
						}
						/* break missing intentionally */
					case ZEND_JMPZNZ:
					case ZEND_JMPZ_EX:
					case ZEND_JMPNZ_EX:
						if (opline->result_type == IS_UNDEF) {
							res_addr = 0;
						} else {
							res_addr = RES_REG_ADDR();
						}
						if (!zend_jit_bool_jmpznz(&dasm_state, opline, op_array,
								OP1_INFO(), OP1_REG_ADDR(), res_addr,
								ssa->cfg.blocks[b].successors[0], ssa->cfg.blocks[b].successors[1],
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FETCH_DIM_R:
					case ZEND_FETCH_DIM_IS:
						if (PROFITABILITY_CHECKS && (!ssa->ops || !ssa->var_info)) {
							break;
						}
						if (!zend_jit_fetch_dim_read(&dasm_state, opline, op_array,
								OP1_INFO(), OP2_INFO(), RES_INFO(),
								zend_may_throw(opline, op_array, ssa))) {
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
						  || (opline+1)->opcode == ZEND_JMPNZ
						  || (opline+1)->opcode == ZEND_JMPZNZ)
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
						if (!zend_jit_isset_isempty_dim(&dasm_state, opline, op_array,
								OP1_INFO(), OP2_INFO(),
								zend_may_throw(opline, op_array, ssa),
								smart_branch_opcode, target_label, target_label2)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FETCH_OBJ_R:
					case ZEND_FETCH_OBJ_IS:
						if (opline->op2_type != IS_CONST
						 || Z_TYPE_P(RT_CONSTANT(opline, opline->op2)) != IS_STRING
						 || Z_STRVAL_P(RT_CONSTANT(opline, opline->op2))[0] == '\0') {
							break;
						}
						ce = NULL;
						if (opline->op1_type == IS_UNUSED) {
							op1_info = MAY_BE_OBJECT|MAY_BE_RC1|MAY_BE_RCN;
							ce = op_array->scope;
						} else {
							op1_info = OP1_INFO();
							if (ssa->var_info && ssa->ops) {
								zend_ssa_op *ssa_op = &ssa->ops[opline - op_array->opcodes];
								if (ssa_op->op1_use >= 0) {
									zend_ssa_var_info *op1_ssa = ssa->var_info + ssa_op->op1_use;
									if (op1_ssa->ce && !op1_ssa->is_instanceof && !op1_ssa->ce->create_object) {
										ce = op1_ssa->ce;
									}
								}
							}
						}
						if (!(op1_info & MAY_BE_OBJECT)) {
							break;
						}
						if (!zend_jit_fetch_obj_read(&dasm_state, opline, op_array,
								op1_info, ce,
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_BIND_GLOBAL:
						if (!ssa->ops || !ssa->var_info) {
							op1_info = MAY_BE_ANY|MAY_BE_REF;
						} else {
							op1_info = OP1_INFO();
						}
						if (!zend_jit_bind_global(&dasm_state, opline, op_array, op1_info)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_RECV:
						if (!zend_jit_recv(&dasm_state, opline, op_array)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_RECV_INIT:
						if (!zend_jit_recv_init(&dasm_state, opline, op_array,
								(opline + 1)->opcode != ZEND_RECV_INIT,
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FREE:
					case ZEND_FE_FREE:
						if (!zend_jit_free(&dasm_state, opline, op_array, OP1_INFO(),
								zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ECHO:
						if (opline->op1_type != IS_CONST
						 || Z_TYPE_P(RT_CONSTANT(opline, opline->op1)) != IS_STRING) {
							break;
						}
						if (!zend_jit_echo(&dasm_state, opline, op_array)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SWITCH_LONG:
					case ZEND_SWITCH_STRING:
						if (!zend_jit_switch(&dasm_state, opline, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
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
						if (!zend_jit_handler(&dasm_state, opline, zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
					}
					zend_jit_set_opline(&dasm_state, opline+1);
					break;
				case ZEND_NOP:
				case ZEND_OP_DATA:
				case ZEND_SWITCH_LONG:
				case ZEND_SWITCH_STRING:
					break;
				case ZEND_JMP:
					if (zend_jit_level < ZEND_JIT_LEVEL_INLINE) {
						const zend_op *target = OP_JMP_ADDR(opline, opline->op1);

						if (!zend_jit_set_ip(&dasm_state, target)) {
							goto jit_failure;
						}
					}
					if (!zend_jit_jmp(&dasm_state, ssa->cfg.blocks[b].successors[0])) {
						goto jit_failure;
					}
					is_terminated = 1;
					break;
				case ZEND_CATCH:
				case ZEND_FAST_CALL:
				case ZEND_FAST_RET:
				case ZEND_GENERATOR_CREATE:
				case ZEND_GENERATOR_RETURN:
				case ZEND_RETURN_BY_REF:
				case ZEND_RETURN:
				case ZEND_EXIT:
				/* switch through trampoline */
				case ZEND_YIELD:
				case ZEND_YIELD_FROM:
					if (!zend_jit_tail_handler(&dasm_state, opline)) {
						goto jit_failure;
					}
					is_terminated = 1;
					break;
				/* stackless execution */
				case ZEND_INCLUDE_OR_EVAL:
				case ZEND_DO_FCALL:
				case ZEND_DO_UCALL:
				case ZEND_DO_FCALL_BY_NAME:
					if (!zend_jit_call(&dasm_state, opline, b + 1)) {
						goto jit_failure;
					}
					is_terminated = 1;
					break;
				case ZEND_JMPZNZ:
					if (!zend_jit_handler(&dasm_state, opline, zend_may_throw(opline, op_array, ssa)) ||
					    !zend_jit_cond_jmp(&dasm_state, OP_JMP_ADDR(opline, opline->op2), ssa->cfg.blocks[b].successors[1]) ||
					    !zend_jit_jmp(&dasm_state, ssa->cfg.blocks[b].successors[0])) {
						goto jit_failure;
					}
					is_terminated = 1;
					break;
				case ZEND_JMPZ:
				case ZEND_JMPNZ:
					if (opline > op_array->opcodes &&
					    ((opline-1)->result_type & (IS_SMART_BRANCH_JMPZ|IS_SMART_BRANCH_JMPNZ)) != 0) {
						/* smart branch */
						if (!zend_jit_cond_jmp(&dasm_state, opline + 1, ssa->cfg.blocks[b].successors[0])) {
							goto jit_failure;
						}
						goto done;
					}
					/* break missing intentionally */
				case ZEND_JMPZ_EX:
				case ZEND_JMPNZ_EX:
				case ZEND_JMP_SET:
				case ZEND_COALESCE:
				case ZEND_FE_RESET_R:
				case ZEND_FE_RESET_RW:
				case ZEND_ASSERT_CHECK:
				case ZEND_FE_FETCH_R:
				case ZEND_FE_FETCH_RW:
					if (!zend_jit_handler(&dasm_state, opline, zend_may_throw(opline, op_array, ssa)) ||
					    !zend_jit_cond_jmp(&dasm_state, opline + 1, ssa->cfg.blocks[b].successors[0])) {
						goto jit_failure;
					}
					break;
				case ZEND_NEW:
					if (!zend_jit_handler(&dasm_state, opline, 1)) {
						return 0;
					}
					if (opline->extended_value == 0 && (opline+1)->opcode == ZEND_DO_FCALL) {
						zend_class_entry *ce = NULL;

						if (zend_jit_level >= ZEND_JIT_LEVEL_OPT_FUNC) {
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

							zend_jit_cond_jmp(&dasm_state, next_opline, ssa->cfg.blocks[b].successors[0]);
							if (zend_jit_level < ZEND_JIT_LEVEL_INLINE) {
								zend_jit_call(&dasm_state, next_opline, b + 1);
								is_terminated = 1;
							} else {
								zend_jit_do_fcall(&dasm_state, next_opline, op_array, ssa, call_level, b + 1);
							}
						}
					}
					break;
				default:
					if (!zend_jit_handler(&dasm_state, opline, zend_may_throw(opline, op_array, ssa))) {
						goto jit_failure;
					}
			}
done:
			switch (opline->opcode) {
				case ZEND_DO_FCALL:
				case ZEND_DO_ICALL:
				case ZEND_DO_UCALL:
				case ZEND_DO_FCALL_BY_NAME:
					call_level--;
			}
		}
	}

	handler = dasm_link_and_encode(&dasm_state, op_array, ssa, rt_opline, ra, NULL);
	if (!handler) {
		goto jit_failure;
	}
	dasm_free(&dasm_state);

	if (zend_jit_reg_alloc) {
		zend_arena_release(&CG(arena), checkpoint);
	}
	return SUCCESS;

jit_failure:
    if (dasm_state) {
		dasm_free(&dasm_state);
    }
	if (zend_jit_reg_alloc) {
		zend_arena_release(&CG(arena), checkpoint);
	}
	return FAILURE;
}

static int zend_jit_collect_calls(zend_op_array *op_array, zend_script *script)
{
	zend_func_info *func_info =
		zend_arena_calloc(&CG(arena), 1, sizeof(zend_func_info));

	ZEND_SET_FUNC_INFO(op_array, func_info);
	func_info->num_args = -1;
	func_info->return_value_used = -1;
	return zend_analyze_calls(&CG(arena), script, ZEND_CALL_TREE, op_array, func_info);
}

static int zend_real_jit_func(zend_op_array *op_array, zend_script *script, const zend_op *rt_opline)
{
	zend_ssa ssa;
	void *checkpoint;

	if (*dasm_ptr == dasm_end) {
		return FAILURE;
	}

	checkpoint = zend_arena_checkpoint(CG(arena));

    /* Build SSA */
	memset(&ssa, 0, sizeof(zend_ssa));

	if (zend_jit_op_array_analyze1(op_array, script, &ssa) != SUCCESS) {
		goto jit_failure;
	}

	if (zend_jit_op_array_analyze2(op_array, script, &ssa) != SUCCESS) {
		goto jit_failure;
	}

	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_SSA) {
		zend_dump_op_array(op_array, ZEND_DUMP_HIDE_UNREACHABLE|ZEND_DUMP_RC_INFERENCE|ZEND_DUMP_SSA, "JIT", &ssa);
	}

	if (zend_jit_level >= ZEND_JIT_LEVEL_OPT_FUNCS) {
		if (zend_jit_collect_calls(op_array, script) != SUCCESS) {
			ZEND_SET_FUNC_INFO(op_array, NULL);
			goto jit_failure;
		}
	}

	if (zend_jit(op_array, &ssa, rt_opline) != SUCCESS) {
		goto jit_failure;
	}

	if (zend_jit_level >= ZEND_JIT_LEVEL_OPT_FUNCS) {
		ZEND_SET_FUNC_INFO(op_array, NULL);
	}

	zend_arena_release(&CG(arena), checkpoint);
	return SUCCESS;

jit_failure:
	zend_arena_release(&CG(arena), checkpoint);
	return FAILURE;
}

/* Run-time JIT handler */
static void ZEND_FASTCALL zend_runtime_jit(void)
{
	zend_execute_data *execute_data = EG(current_execute_data);
	zend_op_array *op_array = &EX(func)->op_array;
	zend_op *opline = op_array->opcodes;

	zend_shared_alloc_lock();

	if (ZEND_FUNC_INFO(op_array)) {
		SHM_UNPROTECT();
		zend_jit_unprotect();

		/* restore original opcode handlers */
		if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
			while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
				opline++;
			}
		}
		opline->handler = ZEND_FUNC_INFO(op_array);
		ZEND_SET_FUNC_INFO(op_array, NULL);

		/* perform real JIT for this function */
		zend_real_jit_func(op_array, NULL, NULL);

		zend_jit_protect();
		SHM_PROTECT();
	}

	zend_shared_alloc_unlock();

	/* JIT-ed code is going to be called by VM */
}

void zend_jit_check_funcs(HashTable *function_table, zend_bool is_method) {
	zend_op *opline;
	zend_function *func;
	zend_op_array *op_array;
	uintptr_t counter;

	ZEND_HASH_REVERSE_FOREACH_PTR(function_table, func) {
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
			opline->handler = ZEND_FUNC_INFO(op_array);
			ZEND_SET_FUNC_INFO(op_array, NULL);
			if (((double)counter / (double)zend_jit_profile_counter) > ZEND_JIT_PROF_THRESHOLD) {
				zend_real_jit_func(op_array, NULL, NULL);
			}
		}
	} ZEND_HASH_FOREACH_END();
}

void ZEND_FASTCALL zend_jit_hot_func(zend_execute_data *execute_data, const zend_op *opline)
{
	zend_op_array *op_array = &EX(func)->op_array;
	zend_jit_op_array_extension *jit_extension;
	uint32_t i;

	zend_shared_alloc_lock();
	jit_extension = (zend_jit_op_array_extension*)ZEND_FUNC_INFO(op_array);

	if (jit_extension) {
		SHM_UNPROTECT();
		zend_jit_unprotect();

		*(jit_extension->counter) = ZEND_JIT_HOT_COUNTER_INIT;
		for (i = 0; i < op_array->last; i++) {
			op_array->opcodes[i].handler = jit_extension->orig_handlers[i];
		}
		ZEND_SET_FUNC_INFO(op_array, NULL);

		/* perform real JIT for this function */
		zend_real_jit_func(op_array, NULL, opline);

		zend_jit_protect();
		SHM_PROTECT();
	}

	zend_shared_alloc_unlock();

	/* JIT-ed code is going to be called by VM */
}

static int zend_jit_setup_hot_counters(zend_op_array *op_array)
{
	zend_op *opline = op_array->opcodes;
	zend_jit_op_array_extension *jit_extension;
	zend_cfg cfg;
	uint32_t i;

	ZEND_ASSERT(zend_jit_func_counter_handler != NULL);
	ZEND_ASSERT(zend_jit_loop_counter_handler != NULL);

	if (zend_jit_build_cfg(op_array, &cfg) != SUCCESS) {
		return FAILURE;
	}

	jit_extension = (zend_jit_op_array_extension*)zend_shared_alloc(sizeof(zend_jit_op_array_extension) + (op_array->last - 1) * sizeof(void*));
	jit_extension->counter = &zend_jit_hot_counters[zend_jit_op_array_hash(op_array) & (ZEND_HOT_COUNTERS_COUNT - 1)];
	for (i = 0; i < op_array->last; i++) {
		jit_extension->orig_handlers[i] = op_array->opcodes[i].handler;
	}
	ZEND_SET_FUNC_INFO(op_array, (void*)jit_extension);

	if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
		while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
			opline++;
		}
	}

	opline->handler = (const void*)zend_jit_func_counter_handler;

	for (i = 0; i < cfg.blocks_count; i++) {
		if ((cfg.blocks[i].flags & ZEND_BB_REACHABLE) &&
		    (cfg.blocks[i].flags & ZEND_BB_LOOP_HEADER)) {
		    op_array->opcodes[cfg.blocks[i].start].handler =
				(const void*)zend_jit_loop_counter_handler;
		}
	}

	return SUCCESS;
}

static int zend_needs_manual_jit(const zend_op_array *op_array)
{
	if (op_array->doc_comment) {
		const char *s = ZSTR_VAL(op_array->doc_comment);
		const char *p = strstr(s, "@jit");

		if (p) {
			size_t l = ZSTR_LEN(op_array->doc_comment);

			if ((p == s + 3 || *(p-1) <= ' ') &&
			    (p + 6 == s + l || *(p+4) <= ' ')) {
				return 1;
			}
		}
	}
	return 0;
}

ZEND_EXT_API int zend_jit_op_array(zend_op_array *op_array, zend_script *script)
{
	if (dasm_ptr == NULL) {
		return FAILURE;
	}

	if (zend_jit_trigger == ZEND_JIT_ON_FIRST_EXEC) {
		zend_op *opline = op_array->opcodes;

		/* Set run-time JIT handler */
		ZEND_ASSERT(zend_jit_runtime_jit_handler != NULL);
		if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
			while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
				opline++;
			}
		}
		ZEND_SET_FUNC_INFO(op_array, (void*)opline->handler);
		opline->handler = (const void*)zend_jit_runtime_jit_handler;

		return SUCCESS;
	} else if (zend_jit_trigger == ZEND_JIT_ON_PROF_REQUEST) {
		zend_op *opline = op_array->opcodes;

		ZEND_ASSERT(zend_jit_profile_jit_handler != NULL);
		if (op_array->function_name) {
			if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
				while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
					opline++;
				}
			}
			ZEND_SET_FUNC_INFO(op_array, (void*)opline->handler);
			opline->handler = (const void*)zend_jit_profile_jit_handler;
		}

		return SUCCESS;
	} else if (zend_jit_trigger == ZEND_JIT_ON_HOT_COUNTERS) {
		return zend_jit_setup_hot_counters(op_array);
	} else if (zend_jit_trigger == ZEND_JIT_ON_SCRIPT_LOAD) {
		return zend_real_jit_func(op_array, script, NULL);
	} else if (zend_jit_trigger == ZEND_JIT_ON_DOC_COMMENT) {
		if (zend_needs_manual_jit(op_array)) {
			return zend_real_jit_func(op_array, script, NULL);
		} else {
			return SUCCESS;
		}
	} else {
		ZEND_ASSERT(0);
	}
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
	if (zend_build_call_graph(&CG(arena), script, &call_graph) != SUCCESS) {
		goto jit_failure;
	}

	zend_analyze_call_graph(&CG(arena), script, &call_graph);

	if (zend_jit_trigger == ZEND_JIT_ON_FIRST_EXEC ||
	    zend_jit_trigger == ZEND_JIT_ON_PROF_REQUEST ||
	    zend_jit_trigger == ZEND_JIT_ON_HOT_COUNTERS) {
		for (i = 0; i < call_graph.op_arrays_count; i++) {
			ZEND_SET_FUNC_INFO(call_graph.op_arrays[i], NULL);
			if (zend_jit_op_array(call_graph.op_arrays[i], script) != SUCCESS) {
				goto jit_failure;
			}
		}
	} else if (zend_jit_trigger == ZEND_JIT_ON_SCRIPT_LOAD ||
	           zend_jit_trigger == ZEND_JIT_ON_DOC_COMMENT) {

		if (zend_jit_trigger == ZEND_JIT_ON_DOC_COMMENT) {
			int do_jit = 0;
			for (i = 0; i < call_graph.op_arrays_count; i++) {
				if (zend_needs_manual_jit(call_graph.op_arrays[i])) {
					do_jit = 1;
					break;
				}
			}
			if (!do_jit) {
				goto jit_failure;
			}
		}
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
			if (zend_jit_trigger == ZEND_JIT_ON_DOC_COMMENT &&
			    !zend_needs_manual_jit(call_graph.op_arrays[i])) {
				continue;
			}
			info = ZEND_FUNC_INFO(call_graph.op_arrays[i]);
			if (info) {
				if (zend_jit_op_array_analyze2(call_graph.op_arrays[i], script, &info->ssa) != SUCCESS) {
					goto jit_failure;
				}
				info->flags = info->ssa.cfg.flags;
			}
		}

		if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_SSA) {
			for (i = 0; i < call_graph.op_arrays_count; i++) {
				if (zend_jit_trigger == ZEND_JIT_ON_DOC_COMMENT &&
				    !zend_needs_manual_jit(call_graph.op_arrays[i])) {
					continue;
				}
				info = ZEND_FUNC_INFO(call_graph.op_arrays[i]);
				if (info) {
					zend_dump_op_array(call_graph.op_arrays[i], ZEND_DUMP_HIDE_UNREACHABLE|ZEND_DUMP_RC_INFERENCE|ZEND_DUMP_SSA, "JIT", &info->ssa);
				}
			}
		}

		for (i = 0; i < call_graph.op_arrays_count; i++) {
			if (zend_jit_trigger == ZEND_JIT_ON_DOC_COMMENT &&
			    !zend_needs_manual_jit(call_graph.op_arrays[i])) {
				continue;
			}
			info = ZEND_FUNC_INFO(call_graph.op_arrays[i]);
			if (info) {
				if (zend_jit(call_graph.op_arrays[i], &info->ssa, NULL) != SUCCESS) {
					goto jit_failure;
				}
			}
		}

		for (i = 0; i < call_graph.op_arrays_count; i++) {
			ZEND_SET_FUNC_INFO(call_graph.op_arrays[i], NULL);
		}
	} else {
		ZEND_ASSERT(0);
	}

	zend_arena_release(&CG(arena), checkpoint);
	return SUCCESS;

jit_failure:
	for (i = 0; i < call_graph.op_arrays_count; i++) {
		ZEND_SET_FUNC_INFO(call_graph.op_arrays[i], NULL);
	}
	zend_arena_release(&CG(arena), checkpoint);
	return FAILURE;
}

ZEND_EXT_API void zend_jit_unprotect(void)
{
#ifdef HAVE_MPROTECT
	if (!(ZCG(accel_directives).jit_debug & (ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF_DUMP))) {
		if (mprotect(dasm_buf, dasm_size, PROT_READ | PROT_WRITE) != 0) {
			fprintf(stderr, "mprotect() failed [%d] %s\n", errno, strerror(errno));
		}
	}
#elif _WIN32
	if (!(ZCG(accel_directives).jit_debug & (ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF_DUMP))) {
		DWORD old;

		if (!VirtualProtect(dasm_buf, dasm_size, PAGE_READWRITE, &old)) {
			fprintf(stderr, "VirtualProtect() failed\n");
		}
	}
#endif
}

ZEND_EXT_API void zend_jit_protect(void)
{
#ifdef HAVE_MPROTECT
	if (!(ZCG(accel_directives).jit_debug & (ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF_DUMP))) {
		if (mprotect(dasm_buf, dasm_size, PROT_READ | PROT_EXEC) != 0) {
			fprintf(stderr, "mprotect() failed [%d] %s\n", errno, strerror(errno));
		}
	}
#elif _WIN32
	if (!(ZCG(accel_directives).jit_debug & (ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF_DUMP))) {
		DWORD old;

		if (!VirtualProtect(dasm_buf, dasm_size, PAGE_EXECUTE_READ, &old)) {
			fprintf(stderr, "VirtualProtect() failed\n");
		}
	}
#endif
}

static int zend_jit_make_stubs(void)
{
	dasm_State* dasm_state = NULL;
	uint32_t i;

	dasm_init(&dasm_state, DASM_MAXSECTION);
	dasm_setupglobal(&dasm_state, dasm_labels, zend_lb_MAX);

	for (i = 0; i < sizeof(zend_jit_stubs)/sizeof(zend_jit_stubs[0]); i++) {
		dasm_setup(&dasm_state, dasm_actions);
		if (!zend_jit_stubs[i].stub(&dasm_state)) {
			return 0;
		}
		if (!dasm_link_and_encode(&dasm_state, NULL, NULL, NULL, NULL, zend_jit_stubs[i].name)) {
			return 0;
		}
	}

	if (zend_jit_vm_kind == ZEND_VM_KIND_HYBRID) {
		if (zend_jit_trigger == ZEND_JIT_ON_FIRST_EXEC) {
			dasm_setup(&dasm_state, dasm_actions);
			if (!zend_jit_hybrid_runtime_jit_stub(&dasm_state)) {
				return 0;
			}
			zend_jit_runtime_jit_handler = dasm_link_and_encode(&dasm_state, NULL, NULL, NULL, NULL, "JIT$$hybrid_runtime_jit");
			if (!zend_jit_runtime_jit_handler) {
				return 0;
			}
		} else if (zend_jit_trigger == ZEND_JIT_ON_PROF_REQUEST) {
			dasm_setup(&dasm_state, dasm_actions);
			if (!zend_jit_hybrid_profile_jit_stub(&dasm_state)) {
				return 0;
			}
			zend_jit_profile_jit_handler = dasm_link_and_encode(&dasm_state, NULL, NULL, NULL, NULL, "JIT$$hybrid_profile_jit");
			if (!zend_jit_profile_jit_handler) {
				return 0;
			}
		} else if (zend_jit_trigger == ZEND_JIT_ON_HOT_COUNTERS) {
			dasm_setup(&dasm_state, dasm_actions);
			if (!zend_jit_hybrid_func_counter_stub(&dasm_state)) {
				return 0;
			}
			zend_jit_func_counter_handler = dasm_link_and_encode(&dasm_state, NULL, NULL, NULL, NULL, "JIT$$hybrid_func_counter");
			if (!zend_jit_func_counter_handler) {
				return 0;
			}

			dasm_setup(&dasm_state, dasm_actions);
			if (!zend_jit_hybrid_loop_counter_stub(&dasm_state)) {
				return 0;
			}
			zend_jit_loop_counter_handler = dasm_link_and_encode(&dasm_state, NULL, NULL, NULL, NULL, "JIT$$hybrid_loop_counter");
			if (!zend_jit_loop_counter_handler) {
				return 0;
			}
		}
	} else {
		zend_jit_runtime_jit_handler = (const void*)zend_runtime_jit;
		zend_jit_profile_jit_handler = (const void*)zend_jit_profile_helper;
		zend_jit_func_counter_handler = (const void*)zend_jit_func_counter_helper;
		zend_jit_loop_counter_handler = (const void*)zend_jit_loop_counter_helper;
	}

	dasm_free(&dasm_state);
	return 1;
}

ZEND_EXT_API int zend_jit_startup(zend_long jit, void *buf, size_t size, zend_bool reattached)
{
	int ret;

	zend_jit_level = ZEND_JIT_LEVEL(jit);
	zend_jit_trigger = ZEND_JIT_TRIGGER(jit);
	zend_jit_reg_alloc = ZEND_JIT_REG_ALLOC(jit);
	zend_jit_cpu_flags = ZEND_JIT_CPU_FLAGS(jit);

	zend_jit_vm_kind = zend_vm_kind();
	if (zend_jit_vm_kind != ZEND_VM_KIND_CALL &&
	    zend_jit_vm_kind != ZEND_VM_KIND_HYBRID) {
		// TODO: error reporting and cleanup ???
		return FAILURE;
	}

	zend_jit_halt_op = zend_get_halt_op();

	if (zend_jit_setup() != SUCCESS) {
		// TODO: error reporting and cleanup ???
		return FAILURE;
	}

	if (zend_jit_trigger == ZEND_JIT_ON_PROF_REQUEST) {
		zend_jit_profile_counter_rid = zend_get_op_array_extension_handle();
	}

#ifdef HAVE_GDB
	zend_jit_gdb_init();
#endif

#ifdef HAVE_OPROFILE
	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_OPROFILE) {
		if (!zend_jit_oprofile_startup()) {
			// TODO: error reporting and cleanup ???
			return FAILURE;
		}
	}
#endif

	dasm_buf = buf;
	dasm_size = size;

#ifdef HAVE_MPROTECT
	if (ZCG(accel_directives).jit_debug & (ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF_DUMP)) {
		if (mprotect(dasm_buf, dasm_size, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
			fprintf(stderr, "mprotect() failed [%d] %s\n", errno, strerror(errno));
		}
	} else {
		if (mprotect(dasm_buf, dasm_size, PROT_READ | PROT_EXEC) != 0) {
			fprintf(stderr, "mprotect() failed [%d] %s\n", errno, strerror(errno));
		}
	}
#elif _WIN32
	if (ZCG(accel_directives).jit_debug & (ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_PERF_DUMP)) {
		DWORD old;

		if (!VirtualProtect(dasm_buf, dasm_size, PAGE_EXECUTE_READWRITE, &old)) {
			fprintf(stderr, "VirtualProtect() failed\n");
		}
	} else {
		DWORD old;

		if (!VirtualProtect(dasm_buf, dasm_size, PAGE_EXECUTE_READ, &old)) {
			fprintf(stderr, "VirtualProtect() failed\n");
		}
	}
#endif

	dasm_ptr = dasm_end = (void*)(((char*)dasm_buf) + size - sizeof(*dasm_ptr));
	if (!reattached) {
		zend_jit_unprotect();
		*dasm_ptr = dasm_buf;
#if _WIN32
		/* reserve space for global labels */
		*dasm_ptr = (void**)*dasm_ptr + zend_lb_MAX;
#endif
		zend_jit_protect();
	}

#ifdef HAVE_DISASM
	if (ZCG(accel_directives).jit_debug & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_ASM_STUBS)) {
		if (!zend_jit_disasm_init()) {
			// TODO: error reporting and cleanup ???
			return FAILURE;
		}
	}
#endif

#ifdef HAVE_PERFTOOLS
	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_PERF_DUMP) {
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
#endif
	}

	return SUCCESS;
}

ZEND_EXT_API void zend_jit_shutdown(void)
{
#ifdef HAVE_OPROFILE
	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_OPROFILE) {
		zend_jit_oprofile_shutdown();
	}
#endif

#ifdef HAVE_GDB
	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_GDB) {
		zend_jit_gdb_unregister();
	}
#endif

#ifdef HAVE_DISASM
	if (ZCG(accel_directives).jit_debug & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_ASM_STUBS)) {
		zend_jit_disasm_shutdown();
	}
#endif

#ifdef HAVE_PERFTOOLS
	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_PERF_DUMP) {
		zend_jit_perf_jitdump_close();
	}
#endif
}

ZEND_EXT_API void zend_jit_activate(void)
{
	if (zend_jit_trigger == ZEND_JIT_ON_HOT_COUNTERS) {
		int i;

		for (i = 0; i < ZEND_HOT_COUNTERS_COUNT; i++) {
			zend_jit_hot_counters[i] = ZEND_JIT_HOT_COUNTER_INIT;
		}
	}
}

ZEND_EXT_API void zend_jit_deactivate(void)
{
	if (zend_jit_trigger == ZEND_JIT_ON_PROF_REQUEST) {
		if (!zend_jit_profile_counter) {
			return;
		} else {
			zend_class_entry *ce;

			zend_shared_alloc_lock();
			SHM_UNPROTECT();
			zend_jit_unprotect();

			zend_jit_check_funcs(EG(function_table), 0);
			ZEND_HASH_REVERSE_FOREACH_PTR(EG(class_table), ce) {
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
}

#else /* HAVE_JIT */

ZEND_EXT_API int zend_jit_op_array(const zend_op_array *op_array, zend_script *script)
{
	return FAILURE;
}

ZEND_EXT_API int zend_jit_script(zend_script *script)
{
	return FAILURE;
}

ZEND_EXT_API void zend_jit_unprotect(void)
{
}

ZEND_EXT_API void zend_jit_protect(void)
{
}

ZEND_EXT_API int zend_jit_startup(zend_long jit, size_t size)
{
	return FAILURE;
}

ZEND_EXT_API void zend_jit_shutdown(void)
{
}

ZEND_EXT_API void zend_jit_activate(void)
{
}

ZEND_EXT_API void zend_jit_deactivate(void)
{
}

#endif /* HAVE_JIT */
