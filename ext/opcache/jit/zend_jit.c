/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
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
#define PREFER_MAP_32BIT
//#define ZEND_JIT_RECORD
//#define ZEND_JIT_FILTER
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

// TODO: define DASM_M_GROW and DASM_M_FREE to use CG(arena) ???

#include "dynasm/dasm_proto.h"

typedef struct _zend_jit_stub {
	const char *name;
	int (*stub)(dasm_State **Dst);
} zend_jit_stub;

#define JIT_STUB(name) \
	{JIT_STUB_PREFIX #name, zend_jit_ ## name ## _stub}

static zend_uchar zend_jit_level = 0;
static zend_uchar zend_jit_trigger = 0;

zend_ulong zend_jit_profile_counter = 0;
int zend_jit_profile_counter_rid = -1;

int16_t zend_jit_hot_counters[ZEND_HOT_COUNTERS_COUNT];

static void *dasm_buf = NULL;
static void *dasm_end = NULL;
static void **dasm_ptr = NULL;

static int zend_may_throw(const zend_op *opline, zend_op_array *op_array, zend_ssa *ssa);
static int zend_may_overflow(const zend_op *opline, zend_op_array *op_array, zend_ssa *ssa);

#include "dynasm/dasm_x86.h"
#include "jit/zend_jit_helpers.c"
#include "jit/zend_jit_x86.c"
#include "jit/zend_jit_disasm_x86.c"
#include "jit/zend_jit_gdb.c"
#include "jit/zend_jit_perf_dump.c"
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

ZEND_API void zend_jit_status(zval *ret)
{
	zval stats;
	array_init(&stats);
	add_assoc_long(&stats, "level", zend_jit_level);
	add_assoc_long(&stats, "trigger", zend_jit_trigger);
	if (dasm_buf) {
		add_assoc_long(&stats, "buffer_size", (char*)dasm_end - (char*)dasm_buf);
		add_assoc_long(&stats, "buffer_free", (char*)dasm_end - (char*)*dasm_ptr);
	} else {
		add_assoc_long(&stats, "buffer_size", 0);
		add_assoc_long(&stats, "buffer_free", 0);
	}
	add_assoc_zval(ret, "jit", &stats);
}

static zend_string *zend_jit_func_name(zend_op_array *op_array)
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

static void *dasm_link_and_encode(dasm_State    **dasm_state,
                                  zend_op_array  *op_array,
                                  zend_cfg       *cfg,
                                  const zend_op  *rt_opline,
                                  const char     *name)
{
	size_t size;
	int ret;
	void *entry;
#if defined(HAVE_DISASM) || defined(HAVE_GDB) || defined(HAVE_OPROFILE) || defined(HAVE_PERFTOOLS) || defined(HAVE_VTUNE)
	zend_string *str = NULL;
#endif

    if (rt_opline && cfg && cfg->map) {
		/* Create additional entry point, to switch from interpreter to JIT-ed
		 * code at run-time.
		 */
		int b = cfg->map[rt_opline - op_array->opcodes];

#ifdef CONTEXT_THREADED_JIT
		if (!(cfg->blocks[b].flags & (ZEND_BB_START|ZEND_BB_RECV_ENTRY))) {
#else
		if (!(cfg->blocks[b].flags & (ZEND_BB_START|ZEND_BB_ENTRY|ZEND_BB_RECV_ENTRY))) {
#endif
			zend_jit_label(dasm_state, cfg->blocks_count + b);
			zend_jit_prologue(dasm_state);
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

	if (op_array && cfg) {
		int b;

		for (b = 0; b < cfg->blocks_count; b++) {
#ifdef CONTEXT_THREADED_JIT
			if (cfg->blocks[b].flags & (ZEND_BB_START|ZEND_BB_RECV_ENTRY)) {
#else
			if (cfg->blocks[b].flags & (ZEND_BB_START|ZEND_BB_ENTRY|ZEND_BB_RECV_ENTRY)) {
#endif
				zend_op *opline = op_array->opcodes + cfg->blocks[b].start;

				opline->handler = (void*)(((char*)entry) +
					dasm_getpclabel(dasm_state, cfg->blocks_count + b));
			}
		}
	    if (rt_opline && cfg && cfg->map) {
			int b = cfg->map[rt_opline - op_array->opcodes];
			zend_op *opline = (zend_op*)rt_opline;
			opline->handler = (void*)(((char*)entry) +
				dasm_getpclabel(dasm_state, cfg->blocks_count + b));
		}
	}

#if defined(HAVE_DISASM) || defined(HAVE_GDB) || defined(HAVE_OPROFILE) || defined(HAVE_PERFTOOLS) || defined(HAVE_VTUNE)
    if (!name) {
		if (ZCG(accel_directives).jit_debug & (ZEND_JIT_DEBUG_ASM|ZEND_JIT_DEBUG_GDB|ZEND_JIT_DEBUG_OPROFILE|ZEND_JIT_DEBUG_PERF|ZEND_JIT_DEBUG_VTUNE)) {
			str = zend_jit_func_name(op_array);
			if (str) {
				name = ZSTR_VAL(str);
			}
		}
	}
#endif

#ifdef HAVE_DISASM
	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_ASM) {
		zend_jit_disasm_add_symbol(name, (uintptr_t)entry, size);
		zend_jit_disasm(
			name,
			(op_array && op_array->filename) ? ZSTR_VAL(op_array->filename) : NULL,
			op_array,
			cfg,
			entry,
			size);
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
	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_PERF) {
		if (name) {
			zend_jit_perf_dump(
				name,
				entry,
				size);
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

static size_t jit_page_size(void)
{
#ifdef _WIN32
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	return system_info.dwPageSize;
#else
	return getpagesize();
#endif
}

static void *jit_alloc(size_t size, int shared)
{
#ifdef _WIN32
	return VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#else
	void *p;
	int prot;
# ifdef MAP_HUGETLB
	size_t huge_page_size = 2 * 1024 * 1024;
# endif

# ifdef HAVE_MPROTECT
	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_GDB) {
		prot = PROT_EXEC | PROT_READ | PROT_WRITE;
	} else {
		prot = PROT_NONE;
	}
# else
	prot = PROT_EXEC | PROT_READ | PROT_WRITE;
# endif

	shared = shared? MAP_SHARED : MAP_PRIVATE;

# ifdef MAP_HUGETLB
	if (size >= huge_page_size && size % huge_page_size == 0) {
#  if defined(PREFER_MAP_32BIT) && defined(__x86_64__) && defined(MAP_32BIT)
		void *p2;
		/* to got HUGE PAGES in low 32-bit address we have to reseve address
		   space and then remap it using MAP_HUGETLB */
		p = mmap(NULL, size, prot, shared | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
		if (p != MAP_FAILED) {
			munmap(p, size);
			p = (void*)(ZEND_MM_ALIGNED_SIZE_EX((ptrdiff_t)p, huge_page_size));
			p2 = mmap(p, size, prot, shared | MAP_ANONYMOUS | MAP_HUGETLB | MAP_FIXED, -1, 0);
			if (p2 != MAP_FAILED) {
				return p2;
			} else {
				p = mmap(NULL, size, prot, shared | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
				if (p != MAP_FAILED) {
					return p;
				}
			}
		}
#  endif
		p = mmap(NULL, size, prot, shared | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
		if (p != MAP_FAILED) {
			return p;
		}
#  if defined(PREFER_MAP_32BIT) && defined(__x86_64__) && defined(MAP_32BIT)
	} else {
		p = mmap(NULL, size, prot, shared | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
		if (p != MAP_FAILED) {
			return p;
		}
#  endif
	}
# elif defined(PREFER_MAP_32BIT) && defined(__x86_64__) && defined(MAP_32BIT)
	p = mmap(NULL, size, prot, shared | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
	if (p != MAP_FAILED) {
		return p;
	}
# endif
	p = mmap(NULL, size, prot, shared | MAP_ANONYMOUS, -1, 0);
	if (p == MAP_FAILED) {
		return NULL;
	}

	return (void*)p;
#endif
}

static void jit_free(void *p, size_t size)
{
#ifdef _WIN32
    VirtualFree(p, 0, MEM_RELEASE);
#else
	munmap(p, size);
#endif
}

static int zend_may_throw(const zend_op *opline, zend_op_array *op_array, zend_ssa *ssa)
{
	uint32_t t1 = OP1_INFO();
	uint32_t t2 = OP2_INFO();

    if (opline->op1_type == IS_CV) {
		if (t1 & MAY_BE_UNDEF) {
			switch (opline->opcode) {
				case ZEND_UNSET_VAR:
				case ZEND_ISSET_ISEMPTY_VAR:
					if (opline->extended_value & ZEND_QUICK_SET) {
						break;
					}
				case ZEND_ISSET_ISEMPTY_DIM_OBJ:
				case ZEND_ISSET_ISEMPTY_PROP_OBJ:
				case ZEND_ASSIGN:
				case ZEND_ASSIGN_DIM:
				case ZEND_ASSIGN_REF:
				case ZEND_BIND_GLOBAL:
				case ZEND_FETCH_DIM_IS:
				case ZEND_FETCH_OBJ_IS:
				case ZEND_SEND_REF:
					break;
				default:
					/* undefined variable warning */
					return 1;
			}
		}
    } else if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
		if (t1 & (MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_ARRAY)) {
			switch (opline->opcode) {
				case ZEND_CASE:
				case ZEND_FE_FETCH_R:
				case ZEND_FE_FETCH_RW:
				case ZEND_FETCH_LIST:
				case ZEND_QM_ASSIGN:
				case ZEND_SEND_VAL:
				case ZEND_SEND_VAL_EX:
				case ZEND_SEND_VAR:
				case ZEND_SEND_VAR_EX:
				case ZEND_SEND_VAR_NO_REF:
				case ZEND_SEND_VAR_NO_REF_EX:
				case ZEND_SEND_REF:
				case ZEND_SEPARATE:
				case ZEND_END_SILENCE:
					break;
				default:
					/* destructor may be called */
					return 1;
			}
		}
    }

    if (opline->op2_type == IS_CV) {
		if (t2 & MAY_BE_UNDEF) {
			switch (opline->opcode) {
				case ZEND_ASSIGN_REF:
					break;
				default:
					/* undefined variable warning */
					return 1;
			}
		}
	} else if (opline->op2_type & (IS_TMP_VAR|IS_VAR)) {
		if (t2 & (MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_ARRAY)) {
			switch (opline->opcode) {
				case ZEND_ASSIGN:
					break;
				default:
					/* destructor may be called */
					return 1;
			}
		}
    }

	switch (opline->opcode) {
		case ZEND_NOP:
		case ZEND_IS_IDENTICAL:
		case ZEND_IS_NOT_IDENTICAL:
		case ZEND_QM_ASSIGN:
		case ZEND_JMP:
		case ZEND_CHECK_VAR:
		case ZEND_MAKE_REF:
		case ZEND_SEND_VAR:
		case ZEND_BEGIN_SILENCE:
		case ZEND_END_SILENCE:
		case ZEND_SEND_VAL:
		case ZEND_SEND_REF:
		case ZEND_SEND_VAR_EX:
		case ZEND_FREE:
		case ZEND_SEPARATE:
		case ZEND_TYPE_CHECK:
		case ZEND_DEFINED:
		case ZEND_ISSET_ISEMPTY_THIS:
		case ZEND_COALESCE:
			return 0;
		case ZEND_INIT_FCALL:
			/* can't throw, because call is resolved at compile time */
			return 0;
		case ZEND_BIND_GLOBAL:
			if ((opline+1)->opcode == ZEND_BIND_GLOBAL) {
				return zend_may_throw(opline + 1, op_array, ssa);
			}
			return 0;
		case ZEND_ADD:
			if ((t1 & MAY_BE_ANY) == MAY_BE_ARRAY
			 && (t2 & MAY_BE_ANY) == MAY_BE_ARRAY) {
				return 0;
			}
			return (t1 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT)) ||
				(t2 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT));
		case ZEND_DIV:
		case ZEND_MOD:
			if (opline->op2_type != IS_CONST
			 || Z_TYPE_P(RT_CONSTANT(op_array, opline->op2)) == IS_NULL
			 || Z_TYPE_P(RT_CONSTANT(op_array, opline->op2)) == IS_FALSE
			 || (Z_TYPE_P(RT_CONSTANT(op_array, opline->op2)) == IS_LONG
			  && Z_LVAL_P(RT_CONSTANT(op_array, opline->op2)) == 0)
			 || (Z_TYPE_P(RT_CONSTANT(op_array, opline->op2)) == IS_DOUBLE
			  && Z_DVAL_P(RT_CONSTANT(op_array, opline->op2)) == 0.0)) {
				return 1;
			}
			/* break missing intentionally */
		case ZEND_SUB:
		case ZEND_MUL:
		case ZEND_POW:
			return (t1 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT)) ||
				(t2 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT));
		case ZEND_SL:
		case ZEND_SR:
			if (opline->op2_type != IS_CONST
			 || (Z_TYPE_P(RT_CONSTANT(op_array, opline->op2)) == IS_LONG
			  && (zend_ulong)Z_LVAL_P(RT_CONSTANT(op_array, opline->op2)) >= SIZEOF_ZEND_LONG * 8)
			 || (Z_TYPE_P(RT_CONSTANT(op_array, opline->op2)) == IS_DOUBLE
			  && (zend_ulong)Z_DVAL_P(RT_CONSTANT(op_array, opline->op2)) >= SIZEOF_ZEND_LONG * 8)) {
				return 1;
			}
			return (t1 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT)) ||
				(t2 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT));
		case ZEND_CONCAT:
		case ZEND_FAST_CONCAT:
			return (t1 & (MAY_BE_ARRAY|MAY_BE_OBJECT)) ||
				(t2 & (MAY_BE_ARRAY|MAY_BE_OBJECT));
		case ZEND_BW_OR:
		case ZEND_BW_AND:
		case ZEND_BW_XOR:
			if ((t1 & MAY_BE_ANY) == MAY_BE_STRING
			 && (t2 & MAY_BE_ANY) == MAY_BE_STRING) {
				return 0;
			}
			return (t1 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT)) ||
				(t2 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT));
		case ZEND_BW_NOT:
			return (t1 & (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_ARRAY|MAY_BE_OBJECT));
		case ZEND_BOOL_NOT:
		case ZEND_PRE_INC:
		case ZEND_POST_INC:
		case ZEND_PRE_DEC:
		case ZEND_POST_DEC:
		case ZEND_JMPZ:
		case ZEND_JMPNZ:
		case ZEND_JMPZNZ:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
		case ZEND_BOOL:
		case ZEND_JMP_SET:
			return (t1 & MAY_BE_OBJECT);
		case ZEND_BOOL_XOR:
			return (t1 & MAY_BE_OBJECT) || (t2 & MAY_BE_OBJECT);
		case ZEND_IS_EQUAL:
		case ZEND_IS_NOT_EQUAL:
		case ZEND_IS_SMALLER:
		case ZEND_IS_SMALLER_OR_EQUAL:
		case ZEND_CASE:
		case ZEND_SPACESHIP:
			if ((t1 & MAY_BE_ANY) == MAY_BE_NULL
			 || (t2 & MAY_BE_ANY) == MAY_BE_NULL) {
				return 0;
			}
			return (t1 & MAY_BE_OBJECT) || (t2 & MAY_BE_OBJECT);
		case ZEND_ASSIGN_ADD:
			if (opline->extended_value != 0) {
				return 1;
			}
			if ((t1 & MAY_BE_ANY) == MAY_BE_ARRAY
			 && (t2 & MAY_BE_ANY) == MAY_BE_ARRAY) {
				return 0;
			}
			return (t1 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT)) ||
				(t2 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT));
		case ZEND_ASSIGN_DIV:
		case ZEND_ASSIGN_MOD:
			if (opline->extended_value != 0) {
				return 1;
			}
			if (opline->op2_type != IS_CONST
			 || Z_TYPE_P(RT_CONSTANT(op_array, opline->op2)) == IS_NULL
			 || Z_TYPE_P(RT_CONSTANT(op_array, opline->op2)) == IS_FALSE
			 || (Z_TYPE_P(RT_CONSTANT(op_array, opline->op2)) == IS_LONG
			  && Z_LVAL_P(RT_CONSTANT(op_array, opline->op2)) == 0)
			 || (Z_TYPE_P(RT_CONSTANT(op_array, opline->op2)) == IS_DOUBLE
			  && Z_DVAL_P(RT_CONSTANT(op_array, opline->op2)) == 0.0)) {
				return 1;
			}
			/* break missing intentionally */
		case ZEND_ASSIGN_SUB:
		case ZEND_ASSIGN_MUL:
		case ZEND_ASSIGN_POW:
			if (opline->extended_value != 0) {
				return 1;
			}
			return (t1 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT)) ||
				(t2 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT));
		case ZEND_ASSIGN_SL:
		case ZEND_ASSIGN_SR:
			if (opline->extended_value != 0) {
				return 1;
			}
			if (opline->op2_type != IS_CONST
			 || (Z_TYPE_P(RT_CONSTANT(op_array, opline->op2)) == IS_LONG
			  && (zend_ulong)Z_LVAL_P(RT_CONSTANT(op_array, opline->op2)) >= SIZEOF_ZEND_LONG * 8)
			 || (Z_TYPE_P(RT_CONSTANT(op_array, opline->op2)) == IS_DOUBLE
			  && (zend_ulong)Z_DVAL_P(RT_CONSTANT(op_array, opline->op2)) >= SIZEOF_ZEND_LONG * 8)) {
				return 1;
			}
			return (t1 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT)) ||
				(t2 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT));
		case ZEND_ASSIGN_CONCAT:
			if (opline->extended_value != 0) {
				return 1;
			}
			return (t1 & (MAY_BE_ARRAY|MAY_BE_OBJECT)) ||
				(t2 & (MAY_BE_ARRAY|MAY_BE_OBJECT));
		case ZEND_ASSIGN_BW_OR:
		case ZEND_ASSIGN_BW_AND:
		case ZEND_ASSIGN_BW_XOR:
			if (opline->extended_value != 0) {
				return 1;
			}
			if ((t1 & MAY_BE_ANY) == MAY_BE_STRING
			 && (t2 & MAY_BE_ANY) == MAY_BE_STRING) {
				return 0;
			}
			return (t1 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT)) ||
				(t2 & (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT));
		case ZEND_ASSIGN:
			return (t1 & (MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_ARRAY));
		case ZEND_ASSIGN_DIM:
			return (t1 & (MAY_BE_OBJECT|MAY_BE_RESOURCE|MAY_BE_TRUE|MAY_BE_STRING|MAY_BE_LONG|MAY_BE_DOUBLE)) || opline->op2_type == IS_UNUSED ||
				(t2 & (MAY_BE_UNDEF|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE));
		case ZEND_ROPE_INIT:
		case ZEND_ROPE_ADD:
		case ZEND_ROPE_END:
			return t2 & (MAY_BE_ARRAY|MAY_BE_OBJECT);
		case ZEND_INIT_ARRAY:
		case ZEND_ADD_ARRAY_ELEMENT:
			return (opline->op2_type != IS_UNUSED) && (t2 & (MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE));
		case ZEND_STRLEN:
			return (t1 & MAY_BE_ANY) != MAY_BE_STRING;
		case ZEND_RECV_INIT:
			if (Z_CONSTANT_P(RT_CONSTANT(op_array, opline->op2))) {
				return 1;
			}
			if (op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS) {
				uint32_t arg_num = opline->op1.num;
				zend_arg_info *cur_arg_info;

				if (EXPECTED(arg_num <= op_array->num_args)) {
					cur_arg_info = &op_array->arg_info[arg_num-1];
				} else if (UNEXPECTED(op_array->fn_flags & ZEND_ACC_VARIADIC)) {
					cur_arg_info = &op_array->arg_info[op_array->num_args];
				} else {
					return 0;
				}
				return (cur_arg_info->type_hint != 0);
			} else {
				return 0;
			}
		case ZEND_ISSET_ISEMPTY_DIM_OBJ:
		case ZEND_FETCH_DIM_IS:
			return (t1 & MAY_BE_OBJECT) || (t2 & (MAY_BE_ARRAY|MAY_BE_OBJECT));
		case ZEND_CAST:
			switch (opline->extended_value) {
				case IS_NULL:
					return 0;
				case _IS_BOOL:
					return (t1 & MAY_BE_OBJECT);
				case IS_LONG:
				case IS_DOUBLE:
					return (t1 & (MAY_BE_STRING|MAY_BE_OBJECT));
				case IS_STRING:
					return (t1 & (MAY_BE_ARRAY|MAY_BE_OBJECT));
				case IS_ARRAY:
					return (t1 & MAY_BE_OBJECT);
				case IS_OBJECT:
					return (t1 & MAY_BE_ARRAY);
				default:
					return 1;
			}
		default:
			return 1;
	}
}

static int zend_may_overflow(const zend_op *opline, zend_op_array *op_array, zend_ssa *ssa)
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
				zend_long op1_min, op2_min, res_min;

				if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
					return 1;
				}
				op1_min = OP1_MIN_RANGE();
				op2_min = OP2_MIN_RANGE();
				res_min = op1_min + op2_min;
				if (op1_min < 0 && op2_min < 0 && res_min >= 0) {
					return 1;
				}
			}
			if (ssa->var_info[res].range.overflow) {
				zend_long op1_max, op2_max, res_max;

				if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
					return 1;
				}
				op1_max = OP1_MAX_RANGE();
				op2_max = OP2_MAX_RANGE();
				res_max = op1_max + op2_max;
				if (op1_max > 0 && op2_max > 0 && res_max <= 0) {
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
				zend_long op1_min, op2_max, res_min;

				if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
					return 1;
				}
				op1_min = OP1_MIN_RANGE();
				op2_max = OP2_MAX_RANGE();
				res_min = op1_min - op2_max;
				if (op1_min < 0 && op2_max > 0 && res_min >= 0) {
					return 1;
				}
			}
			if (ssa->var_info[res].range.overflow) {
				zend_long op1_max, op2_min, res_max;

				if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
					return 1;
				}
				op1_max = OP1_MAX_RANGE();
				op2_min = OP2_MIN_RANGE();
				res_max = op1_max - op2_min;
				if (op1_max > 0 && op2_min < 0 && res_max <= 0) {
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
		case ZEND_ASSIGN_ADD:
			if (opline->extended_value != 0) {
				return 1;
			}
			num = opline - op_array->opcodes;
			res = ssa->ops[num].op1_def;
			if (res < 0 ||
			    !ssa->var_info[res].has_range) {
				return 1;
			}
			if (ssa->var_info[res].range.underflow) {
				zend_long op1_min, op2_min, res_min;

				if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
					return 1;
				}
				op1_min = OP1_MIN_RANGE();
				op2_min = OP2_MIN_RANGE();
				res_min = op1_min + op2_min;
				if (op1_min < 0 && op2_min < 0 && res_min >= 0) {
					return 1;
				}
			}
			if (ssa->var_info[res].range.overflow) {
				zend_long op1_max, op2_max, res_max;

				if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
					return 1;
				}
				op1_max = OP1_MAX_RANGE();
				op2_max = OP2_MAX_RANGE();
				res_max = op1_max + op2_max;
				if (op1_max > 0 && op2_max > 0 && res_max <= 0) {
					return 1;
				}
			}
			return 0;
		case ZEND_ASSIGN_SUB:
			if (opline->extended_value != 0) {
				return 1;
			}
			num = opline - op_array->opcodes;
			res = ssa->ops[num].op1_def;
			if (res < 0 ||
			    !ssa->var_info[res].has_range) {
				return 1;
			}
			if (ssa->var_info[res].range.underflow) {
				zend_long op1_min, op2_max, res_min;

				if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
					return 1;
				}
				op1_min = OP1_MIN_RANGE();
				op2_max = OP2_MAX_RANGE();
				res_min = op1_min - op2_max;
				if (op1_min < 0 && op2_max > 0 && res_min >= 0) {
					return 1;
				}
			}
			if (ssa->var_info[res].range.overflow) {
				zend_long op1_max, op2_min, res_max;

				if (!OP1_HAS_RANGE() || !OP2_HAS_RANGE()) {
					return 1;
				}
				op1_max = OP1_MAX_RANGE();
				op2_min = OP2_MIN_RANGE();
				res_max = op1_max - op2_min;
				if (op1_max > 0 && op2_min < 0 && res_max <= 0) {
					return 1;
				}
			}
			return 0;
		case ZEND_ASSIGN_MUL:
			if (opline->extended_value != 0) {
				return 1;
			}
			num = opline - op_array->opcodes;
			res = ssa->ops[num].op1_def;
			return (res < 0 ||
				!ssa->var_info[res].has_range ||
				ssa->var_info[res].range.underflow ||
				ssa->var_info[res].range.overflow);
		default:
			return 1;
	}
}

static int zend_jit_build_cfg(zend_op_array *op_array, zend_cfg *cfg, uint32_t *flags)
{
	if (zend_build_cfg(&CG(arena), op_array, ZEND_CFG_STACKLESS | ZEND_CFG_RECV_ENTRY | ZEND_RT_CONSTANTS | ZEND_CFG_NO_ENTRY_PREDECESSORS | ZEND_SSA_RC_INFERENCE_FLAG, cfg, flags) != SUCCESS) {
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
	if (zend_cfg_identify_loops(op_array, cfg, flags) != SUCCESS) {
		return FAILURE;
	}

	return SUCCESS;
}

static int zend_jit_op_array_analyze1(zend_op_array *op_array, zend_script *script, zend_ssa *ssa, uint32_t *flags)
{
	if (zend_jit_build_cfg(op_array, &ssa->cfg, flags) != SUCCESS) {
		return FAILURE;
	}

	if ((zend_jit_level >= ZEND_JIT_LEVEL_OPT_FUNC)
	 && ssa->cfg.blocks
	 && op_array->last_try_catch == 0
	 && !(op_array->fn_flags & ZEND_ACC_GENERATOR)
	 && !(*flags & ZEND_FUNC_INDIRECT_VAR_ACCESS)) {
		if (zend_build_ssa(&CG(arena), script, op_array, ZEND_RT_CONSTANTS | ZEND_SSA_RC_INFERENCE, ssa, flags) != SUCCESS) {
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
	} else {
		ssa->rt_constants = 1;
	}

	return SUCCESS;
}

static int zend_jit_op_array_analyze2(zend_op_array *op_array, zend_script *script, zend_ssa *ssa, uint32_t *flags)
{
	if ((zend_jit_level >= ZEND_JIT_LEVEL_OPT_FUNC)
	 && ssa->cfg.blocks
	 && op_array->last_try_catch == 0
	 && !(op_array->fn_flags & ZEND_ACC_GENERATOR)
	 && !(*flags & ZEND_FUNC_INDIRECT_VAR_ACCESS)) {

		if (zend_ssa_inference(&CG(arena), op_array, script, ssa) != SUCCESS) {
			return FAILURE;
		}
	}

	return SUCCESS;
}

static int zend_jit(zend_op_array *op_array, zend_ssa *ssa, const zend_op *rt_opline)
{
	int b, i, end;
	zend_op *opline;
	dasm_State* dasm_state = NULL;
	void *handler;
	int call_level = 0;

#ifdef ZEND_JIT_FILTER
	const char *names[] = {
#include "zend_jit_filter.c"
	};

	for (i = 0; i < sizeof(names)/sizeof(names[0]); i++) {
		const char *name = names[i];
		const char *sep = strstr(name, "::");

		if (sep) {
			if (op_array->scope &&
			    op_array->scope->name &&
			    op_array->function_name &&
			    strncmp(ZSTR_VAL(op_array->scope->name), name, sep-name) == 0 &&
			    strcmp(ZSTR_VAL(op_array->function_name), sep+2) == 0) {
				goto pass;
			}
		} else {
			if (op_array->scope == NULL &&
			    op_array->function_name &&
			    strcmp(ZSTR_VAL(op_array->function_name), name) == 0) {
				goto pass;
			}
		}
	}
	return SUCCESS;
pass:
#endif

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
#ifndef CONTEXT_THREADED_JIT
		if (ssa->cfg.blocks[b].flags & ZEND_BB_ENTRY) {
			if (ssa->cfg.blocks[b].flags & ZEND_BB_TARGET) {
				zend_jit_jmp(&dasm_state, b);
			}
			zend_jit_label(&dasm_state, ssa->cfg.blocks_count + b);
			zend_jit_prologue(&dasm_state);
		} else
#endif
		if (ssa->cfg.blocks[b].flags & (ZEND_BB_START|ZEND_BB_RECV_ENTRY)) {
			opline = op_array->opcodes + ssa->cfg.blocks[b].start;
			if (ssa->cfg.split_at_recv && opline->opcode == ZEND_RECV_INIT) {
				if (opline > op_array->opcodes &&
				    (opline-1)->opcode == ZEND_RECV_INIT) {
					if (zend_jit_level < ZEND_JIT_LEVEL_INLINE) {
						/* repeatable opcode */
						zend_jit_label(&dasm_state, b);
						continue;
					}
				} else {
					if (opline != op_array->opcodes) {
						zend_jit_jmp(&dasm_state, 1);
					}
					zend_jit_label(&dasm_state, ssa->cfg.blocks_count + b);
					for (i = 1; (opline+i)->opcode == ZEND_RECV_INIT; i++) {
						zend_jit_label(&dasm_state, ssa->cfg.blocks_count + b + i);
					}
					zend_jit_prologue(&dasm_state);
				}
			} else {
				if (ssa->cfg.blocks[b].flags & (ZEND_BB_TARGET|ZEND_BB_RECV_ENTRY)) {
					zend_jit_jmp(&dasm_state, b);
				}
				zend_jit_label(&dasm_state, ssa->cfg.blocks_count + b);
				zend_jit_prologue(&dasm_state);
			}
#ifdef ZEND_JIT_RECORD
			if (opline->opcode != ZEND_RECV && opline->opcode != ZEND_RECV_INIT) {
				zend_jit_func_header(&dasm_state, op_array);
			}
#endif
		}

		zend_jit_label(&dasm_state, b);
		if (ssa->cfg.blocks[b].flags & ZEND_BB_TARGET) {
			if (!zend_jit_reset_opline(&dasm_state, op_array->opcodes + ssa->cfg.blocks[b].start)) {
				goto jit_failure;
			}
		} else if (ssa->cfg.blocks[b].flags & (ZEND_BB_START|ZEND_BB_RECV_ENTRY|ZEND_BB_ENTRY)) {
			if (!zend_jit_set_opline(&dasm_state, op_array->opcodes + ssa->cfg.blocks[b].start)) {
				goto jit_failure;
			}
		}
		if (ssa->cfg.blocks[b].flags & ZEND_BB_LOOP_HEADER) {
#ifdef ZEND_JIT_RECORD
			zend_jit_loop_header(&dasm_state, op_array, op_array->opcodes + ssa->cfg.blocks[b].start);
#endif
			if (!zend_jit_check_timeout(&dasm_state, op_array->opcodes + ssa->cfg.blocks[b].start)) {
				goto jit_failure;
			}
		}
		if (!ssa->cfg.blocks[b].len) {
			continue;
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
						if (!zend_jit_inc_dec(&dasm_state, opline, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SR:
					case ZEND_SL:
						if (!zend_jit_shift(&dasm_state, opline, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ADD:
					case ZEND_SUB:
					case ZEND_MUL:
//					case ZEND_DIV: // TODO: check for division by zero ???
						if (!zend_jit_math(&dasm_state, opline, &i, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_CONCAT:
					case ZEND_FAST_CONCAT:
						if (!zend_jit_concat(&dasm_state, opline, &i, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN_ADD:
					case ZEND_ASSIGN_SUB:
					case ZEND_ASSIGN_MUL:
//					case ZEND_ASSIGN_DIV: // TODO: check for division by zero ???
					case ZEND_ASSIGN_CONCAT:
						if (!zend_jit_assign_op(&dasm_state, opline, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN_DIM:
						if (!zend_jit_assign_dim(&dasm_state, opline, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ASSIGN:
						if (!zend_jit_assign(&dasm_state, opline, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_QM_ASSIGN:
						if (!zend_jit_qm_assign(&dasm_state, opline, op_array, ssa)) {
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
						if (!zend_jit_send_val(&dasm_state, opline, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SEND_REF:
						if (!zend_jit_send_ref(&dasm_state, opline, op_array, ssa, 0)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_SEND_VAR:
					case ZEND_SEND_VAR_EX:
						if (!zend_jit_send_var(&dasm_state, opline, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_DO_UCALL:
					case ZEND_DO_ICALL:
					case ZEND_DO_FCALL_BY_NAME:
					case ZEND_DO_FCALL:
						if (!zend_jit_do_fcall(&dasm_state, opline, op_array, ssa, call_level)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_IS_EQUAL:
					case ZEND_IS_NOT_EQUAL:
					case ZEND_IS_SMALLER:
					case ZEND_IS_SMALLER_OR_EQUAL:
					case ZEND_CASE:
						if (!zend_jit_cmp(&dasm_state, opline, b, &i, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_DEFINED:
						if (!zend_jit_defined(&dasm_state, opline, b, &i, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_TYPE_CHECK:
						if (!zend_jit_type_check(&dasm_state, opline, b, &i, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_RETURN:
						if (!zend_jit_return(&dasm_state, opline, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_BOOL:
					case ZEND_BOOL_NOT:
						if (!zend_jit_bool_jmpznz(&dasm_state, opline, b, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_JMPZ:
					case ZEND_JMPNZ:
					case ZEND_JMPZNZ:
					case ZEND_JMPZ_EX:
					case ZEND_JMPNZ_EX:
						if ((opline-1)->opcode == ZEND_IS_EQUAL ||
						    (opline-1)->opcode == ZEND_IS_NOT_EQUAL ||
						    (opline-1)->opcode == ZEND_IS_SMALLER ||
						    (opline-1)->opcode == ZEND_IS_SMALLER_OR_EQUAL ||
						    (opline-1)->opcode == ZEND_CASE) {
							/* skip */
						} else if ((opline->opcode == ZEND_JMPZ ||
						           (opline->opcode == ZEND_JMPNZ)) &&
						    ((opline-1)->opcode == ZEND_IS_IDENTICAL ||
						     (opline-1)->opcode == ZEND_IS_NOT_IDENTICAL ||
						     (opline-1)->opcode == ZEND_ISSET_ISEMPTY_VAR ||
						     (opline-1)->opcode == ZEND_ISSET_ISEMPTY_STATIC_PROP ||
						     (opline-1)->opcode == ZEND_ISSET_ISEMPTY_DIM_OBJ ||
						     (opline-1)->opcode == ZEND_ISSET_ISEMPTY_PROP_OBJ ||
						     (opline-1)->opcode == ZEND_INSTANCEOF ||
						     (opline-1)->opcode == ZEND_TYPE_CHECK ||
						     (opline-1)->opcode == ZEND_DEFINED)) {
						    /* smart branch */
							if (!zend_jit_cond_jmp(&dasm_state, opline + 1, ssa->cfg.blocks[b].successors[0])) {
								goto jit_failure;
							}
						} else {
							if (!zend_jit_bool_jmpznz(&dasm_state, opline, b, op_array, ssa)) {
								goto jit_failure;
							}
						}
						goto done;
					case ZEND_FETCH_DIM_R:
					case ZEND_FETCH_DIM_IS:
						if (!zend_jit_fetch_dim_read(&dasm_state, opline, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_ISSET_ISEMPTY_DIM_OBJ:
						if (!zend_jit_isset_isempty_dim(&dasm_state, opline, b, &i, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FETCH_OBJ_R:
					case ZEND_FETCH_OBJ_IS:
						if (!zend_jit_fetch_obj_read(&dasm_state, opline, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_BIND_GLOBAL:
						if (!zend_jit_bind_global(&dasm_state, opline, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_RECV_INIT:
						if (!zend_jit_recv_init(&dasm_state, opline, op_array, (opline + 1)->opcode != ZEND_RECV_INIT, ssa)) {
							goto jit_failure;
						}
						goto done;
					case ZEND_FREE:
					case ZEND_FE_FREE:
						if (!zend_jit_free(&dasm_state, opline, op_array, ssa)) {
							goto jit_failure;
						}
						goto done;
					default:
						break;
				}
			}

			switch (opline->opcode) {
				case ZEND_RECV_INIT:
					if (ssa->cfg.split_at_recv) {
						if (!zend_jit_handler(&dasm_state, opline, zend_may_throw(opline, op_array, ssa))) {
							goto jit_failure;
						}
						break;
					}
					/* break missing intentionally */
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
					break;
				case ZEND_JMP:
					if (!zend_jit_jmp(&dasm_state, ssa->cfg.blocks[b].successors[0])) {
						goto jit_failure;
					}
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
					break;
				/* stackless execution */
				case ZEND_INCLUDE_OR_EVAL:
				case ZEND_DO_FCALL:
				case ZEND_DO_UCALL:
				case ZEND_DO_FCALL_BY_NAME:
					if (!zend_jit_call(&dasm_state, opline)) {
						goto jit_failure;
					}
					break;
				case ZEND_JMPZNZ:
					if (!zend_jit_handler(&dasm_state, opline, zend_may_throw(opline, op_array, ssa)) ||
					    !zend_jit_cond_jmp(&dasm_state, OP_JMP_ADDR(opline, opline->op2), ssa->cfg.blocks[b].successors[1]) ||
					    !zend_jit_jmp(&dasm_state, ssa->cfg.blocks[b].successors[0])) {
						goto jit_failure;
					}
					break;
				case ZEND_JMPZ:
				case ZEND_JMPNZ:
					if (i != 0) {
						if ((opline-1)->opcode == ZEND_IS_EQUAL ||
						    (opline-1)->opcode == ZEND_IS_NOT_EQUAL ||
						    (opline-1)->opcode == ZEND_IS_SMALLER ||
						    (opline-1)->opcode == ZEND_IS_SMALLER_OR_EQUAL ||
						    (opline-1)->opcode == ZEND_CASE) {

							uint32_t t1 = _ssa_op1_info(op_array, ssa, (opline-1));
							uint32_t t2 = _ssa_op2_info(op_array, ssa, (opline-1));

							if ((t1 & (MAY_BE_ANY-(MAY_BE_LONG|MAY_BE_DOUBLE))) ||
							    (t2 & (MAY_BE_ANY-(MAY_BE_LONG|MAY_BE_DOUBLE)))) {
								/* might be smart branch */
								if (!zend_jit_smart_branch(&dasm_state, opline, (b + 1), ssa->cfg.blocks[b].successors[0])) {
									goto jit_failure;
								}
								/* break missing intentionally */
							} else {
								/* smart branch */
								if (!zend_jit_cond_jmp(&dasm_state, opline + 1, ssa->cfg.blocks[b].successors[0])) {
									goto jit_failure;
								}
								break;
							}
						} else if ((opline-1)->opcode == ZEND_IS_IDENTICAL ||
						           (opline-1)->opcode == ZEND_IS_NOT_IDENTICAL ||
						           (opline-1)->opcode == ZEND_ISSET_ISEMPTY_VAR ||
						           (opline-1)->opcode == ZEND_ISSET_ISEMPTY_STATIC_PROP ||
						           (opline-1)->opcode == ZEND_ISSET_ISEMPTY_DIM_OBJ ||
						           (opline-1)->opcode == ZEND_ISSET_ISEMPTY_PROP_OBJ ||
						           (opline-1)->opcode == ZEND_INSTANCEOF ||
						           (opline-1)->opcode == ZEND_TYPE_CHECK ||
						           (opline-1)->opcode == ZEND_DEFINED) {
						    /* smart branch */
							if (!zend_jit_cond_jmp(&dasm_state, opline + 1, ssa->cfg.blocks[b].successors[0])) {
								goto jit_failure;
							}
							break;
						}
					}
					/* break missing intentionally */
				case ZEND_JMPZ_EX:
				case ZEND_JMPNZ_EX:
				case ZEND_JMP_SET:
				case ZEND_COALESCE:
				case ZEND_FE_RESET_R:
				case ZEND_FE_RESET_RW:
				case ZEND_ASSERT_CHECK:
					if (!zend_jit_handler(&dasm_state, opline, zend_may_throw(opline, op_array, ssa)) ||
					    !zend_jit_cond_jmp(&dasm_state, opline + 1, ssa->cfg.blocks[b].successors[0])) {
						goto jit_failure;
					}
					break;
				case ZEND_NEW:
					if (!zend_jit_new(&dasm_state, opline, &i, op_array, ssa)) {
						goto jit_failure;
					}
					break;
				case ZEND_FE_FETCH_R:
				case ZEND_FE_FETCH_RW:
				case ZEND_DECLARE_ANON_CLASS:
				case ZEND_DECLARE_ANON_INHERITED_CLASS:
					if (!zend_jit_handler(&dasm_state, opline, zend_may_throw(opline, op_array, ssa)) ||
					    !zend_jit_cond_jmp(&dasm_state, opline + 1, ssa->cfg.blocks[b].successors[0])) {
						goto jit_failure;
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

	handler = dasm_link_and_encode(&dasm_state, op_array, &ssa->cfg, rt_opline, NULL);
	if (!handler) {
		goto jit_failure;
	}
	dasm_free(&dasm_state);

	return SUCCESS;

jit_failure:
    if (dasm_state) {
		dasm_free(&dasm_state);
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
	return zend_analyze_calls(&CG(arena), script, ZEND_RT_CONSTANTS | ZEND_CALL_TREE, op_array, func_info);
}

static int zend_real_jit_func(zend_op_array *op_array, zend_script *script, const zend_op *rt_opline)
{
	uint32_t flags = 0;
	zend_ssa ssa;
	void *checkpoint;

	if (*dasm_ptr == dasm_end) {
		return FAILURE;
	}

	checkpoint = zend_arena_checkpoint(CG(arena));

    /* Build SSA */
	memset(&ssa, 0, sizeof(zend_ssa));

	if (zend_jit_op_array_analyze1(op_array, script, &ssa, &flags) != SUCCESS) {
		goto jit_failure;
	}

	if (zend_jit_op_array_analyze2(op_array, script, &ssa, &flags) != SUCCESS) {
		goto jit_failure;
	}

	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_SSA) {
		zend_dump_op_array(op_array, ZEND_DUMP_HIDE_UNREACHABLE|ZEND_DUMP_RC_INFERENCE|ZEND_DUMP_SSA|ZEND_DUMP_RT_CONSTANTS, "JIT", &ssa);
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

/* memory write protection */
#define SHM_PROTECT() \
	do { \
		if (ZCG(accel_directives).protect_memory) { \
			zend_accel_shared_protect(1); \
		} \
	} while (0)

#define SHM_UNPROTECT() \
	do { \
		if (ZCG(accel_directives).protect_memory) { \
			zend_accel_shared_protect(0); \
		} \
	} while (0)

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
		while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
			zend_vm_set_opcode_handler(opline);
			opline++;
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

	ZEND_HASH_REVERSE_FOREACH_PTR(function_table, func) {
		if (func->type == ZEND_INTERNAL_FUNCTION) {
			break;
		}
		op_array = &func->op_array;
		opline = op_array->opcodes;
		while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
			opline++;
		}
		if (opline->handler == zend_jit_profile_helper) {
			zend_ulong counter = (zend_ulong)ZEND_COUNTER_INFO(op_array);
			ZEND_COUNTER_INFO(op_array) = 0;
			opline->handler = ZEND_FUNC_INFO(op_array);
			ZEND_SET_FUNC_INFO(op_array, NULL);
			if (((double)counter / (double)zend_jit_profile_counter) > ZEND_JIT_PROF_THRESHOLD) {
				zend_real_jit_func(op_array, NULL, NULL);
			}
		}
	} ZEND_HASH_FOREACH_END();
}

void zend_jit_hot_func(zend_execute_data *execute_data, const zend_op *opline)
{
	zend_op_array *op_array = &EX(func)->op_array;
	const void **orig_handlers;
	uint32_t i;

	zend_shared_alloc_lock();
	orig_handlers = (const void**)ZEND_FUNC_INFO(op_array);

	if (orig_handlers) {
		SHM_UNPROTECT();
		zend_jit_unprotect();

		for (i = 0; i < op_array->last; i++) {
			op_array->opcodes[i].handler = orig_handlers[i];
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
	const void **orig_handlers;
	zend_cfg cfg;
	uint32_t flags = 0;
	uint32_t i;

	if (zend_jit_build_cfg(op_array, &cfg, &flags) != SUCCESS) {
		return FAILURE;
	}

	orig_handlers = (const void**)zend_shared_alloc(op_array->last * sizeof(void*));
	for (i = 0; i < op_array->last; i++) {
		orig_handlers[i] = op_array->opcodes[i].handler;
	}
	ZEND_SET_FUNC_INFO(op_array, (void*)orig_handlers);

	while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
		opline++;
	}

	opline->handler = (const void*)zend_jit_func_counter_helper;

	for (i = 0; i < cfg.blocks_count; i++) {
		if ((cfg.blocks[i].flags & ZEND_BB_REACHABLE) &&
		    (cfg.blocks[i].flags & ZEND_BB_LOOP_HEADER)) {
		    op_array->opcodes[cfg.blocks[i].start].handler =
				(const void*)zend_jit_loop_counter_helper;
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

ZEND_API int zend_jit_op_array(zend_op_array *op_array, zend_script *script)
{
	if (dasm_ptr == NULL) {
		return FAILURE;
	}

	if (zend_jit_trigger == ZEND_JIT_ON_FIRST_EXEC) {
		zend_op *opline = op_array->opcodes;

		/* Set run-time JIT handler */
		while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
			opline->handler = (const void*)zend_runtime_jit;
			opline++;
		}
		ZEND_SET_FUNC_INFO(op_array, (void*)opline->handler);
		opline->handler = (const void*)zend_runtime_jit;

		return SUCCESS;
	} else if (zend_jit_trigger == ZEND_JIT_ON_PROF_REQUEST) {
		zend_op *opline = op_array->opcodes;

		if (op_array->function_name) {
			while (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT) {
				opline++;
			}
			ZEND_SET_FUNC_INFO(op_array, (void*)opline->handler);
			opline->handler = zend_jit_profile_helper;
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

ZEND_API int zend_jit_script(zend_script *script)
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
	if (zend_build_call_graph(&CG(arena), script, ZEND_RT_CONSTANTS, &call_graph) != SUCCESS) {
		goto jit_failure;
	}

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
				if (zend_jit_op_array_analyze1(call_graph.op_arrays[i], script, &info->ssa, &info->flags) != SUCCESS) {
					goto jit_failure;
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
				if (zend_jit_op_array_analyze2(call_graph.op_arrays[i], script, &info->ssa, &info->flags) != SUCCESS) {
					goto jit_failure;
				}
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
					zend_dump_op_array(call_graph.op_arrays[i], ZEND_DUMP_HIDE_UNREACHABLE|ZEND_DUMP_RC_INFERENCE|ZEND_DUMP_SSA|ZEND_DUMP_RT_CONSTANTS, "JIT", &info->ssa);
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

ZEND_API void zend_jit_unprotect(void)
{
#ifdef HAVE_MPROTECT
	if (!(ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_GDB)) {
		if (mprotect(dasm_buf, ((char*)dasm_end) - ((char*)dasm_buf), PROT_READ | PROT_WRITE) != 0) {
			fprintf(stderr, "mprotect() failed [%d] %s\n", errno, strerror(errno));
		}
	}
#endif
}

ZEND_API void zend_jit_protect(void)
{
#ifdef HAVE_MPROTECT
	if (!(ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_GDB)) {
		if (mprotect(dasm_buf, ((char*)dasm_end) - ((char*)dasm_buf), PROT_READ | PROT_EXEC) != 0) {
			fprintf(stderr, "mprotect() failed [%d] %s\n", errno, strerror(errno));
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
		if (!dasm_link_and_encode(&dasm_state, NULL, NULL, NULL, zend_jit_stubs[i].name)) {
			return 0;
		}
	}
	dasm_free(&dasm_state);
	return 1;
}

ZEND_API int zend_jit_startup(zend_long jit, size_t size)
{
	size_t page_size = jit_page_size();
	int shared = 1;
	int ret;

	zend_jit_level = ZEND_JIT_LEVEL(jit);
	zend_jit_trigger = ZEND_JIT_TRIGGER(jit);

	if (zend_jit_trigger == ZEND_JIT_ON_PROF_REQUEST) {
		zend_extension dummy;
		zend_jit_profile_counter_rid = zend_get_resource_handle(&dummy);
		ZEND_ASSERT(zend_jit_profile_counter_rid != -1);
	}

#ifdef HAVE_GDB
	zend_jit_gdb_init();
#endif

#ifdef HAVE_OPROFILE
	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_OPROFILE) {
		shared = 0;
		if (!zend_jit_oprofile_startup()) {
			// TODO: error reporting and cleanup ???
			return FAILURE;
		}
	}
#endif

	/* Round up to the page size, which should be a power of two.  */
	page_size = jit_page_size();

	if (!page_size || (page_size & (page_size - 1))) {
		abort();
	}

	size = ZEND_MM_ALIGNED_SIZE_EX(size, page_size);

	dasm_buf = jit_alloc(size, shared);

	if (!dasm_buf) {
		return FAILURE;
	}

	dasm_ptr = dasm_end = (void*)(((char*)dasm_buf) + size - sizeof(*dasm_ptr));
	zend_jit_unprotect();
	*dasm_ptr = dasm_buf;
	zend_jit_protect();

#ifdef HAVE_DISASM
	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_ASM) {
		if (!zend_jit_disasm_init()) {
			// TODO: error reporting and cleanup ???
			return FAILURE;
		}
	}
#endif

	zend_jit_unprotect();
	ret = zend_jit_make_stubs();
	zend_jit_protect();

	if (!ret) {
		// TODO: error reporting and cleanup ???
		return FAILURE;
	}

	return SUCCESS;
}

ZEND_API void zend_jit_shutdown(void)
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
	if (ZCG(accel_directives).jit_debug & ZEND_JIT_DEBUG_ASM) {
		zend_jit_disasm_shutdown();
	}
#endif

	if (dasm_buf) {
		jit_free(dasm_buf, ((char*)dasm_end) - ((char*)dasm_buf));
	}
}

ZEND_API void zend_jit_activate(void)
{
	if (zend_jit_trigger == ZEND_JIT_ON_HOT_COUNTERS) {
		int i;

		for (i = 0; i < ZEND_HOT_COUNTERS_COUNT; i++) {
			zend_jit_hot_counters[i] = ZEND_JIT_HOT_COUNTER_INIT;
		}
	}
}

ZEND_API void zend_jit_deactivate(void)
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

ZEND_API int zend_jit_op_array(zend_op_array *op_array, zend_script *script)
{
	return FAILURE;
}

ZEND_API int zend_jit_script(zend_script *script)
{
	return FAILURE;
}

ZEND_API void zend_jit_unprotect(void)
{
}

ZEND_API void zend_jit_protect(void)
{
}

ZEND_API int zend_jit_startup(zend_long jit, size_t size)
{
	return FAILURE;
}

ZEND_API void zend_jit_shutdown(void)
{
}

ZEND_API void zend_jit_activate(void)
{
}

ZEND_API void zend_jit_deactivate(void)
{
}

#endif /* HAVE_JIT */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
