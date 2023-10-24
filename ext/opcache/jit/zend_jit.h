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

#ifndef HAVE_JIT_H
#define HAVE_JIT_H

#if defined(__x86_64__) || defined(i386) || defined(ZEND_WIN32)
# define ZEND_JIT_TARGET_X86   1
# define ZEND_JIT_TARGET_ARM64 0
#elif defined (__aarch64__)
# define ZEND_JIT_TARGET_X86   0
# define ZEND_JIT_TARGET_ARM64 1
#else
# error "JIT not supported on this platform"
#endif

#define ZEND_JIT_LEVEL_NONE        0     /* no JIT */
#define ZEND_JIT_LEVEL_MINIMAL     1     /* minimal JIT (subroutine threading) */
#define ZEND_JIT_LEVEL_INLINE      2     /* selective inline threading */
#define ZEND_JIT_LEVEL_OPT_FUNC    3     /* optimized JIT based on Type-Inference */
#define ZEND_JIT_LEVEL_OPT_FUNCS   4     /* optimized JIT based on Type-Inference and call-tree */
#define ZEND_JIT_LEVEL_OPT_SCRIPT  5     /* optimized JIT based on Type-Inference and inner-procedure analysis */

#define ZEND_JIT_ON_SCRIPT_LOAD    0
#define ZEND_JIT_ON_FIRST_EXEC     1
#define ZEND_JIT_ON_PROF_REQUEST   2     /* compile the most frequently caled on first request functions */
#define ZEND_JIT_ON_HOT_COUNTERS   3     /* compile functions after N calls or loop iterations */
#define ZEND_JIT_ON_DOC_COMMENT    4     /* compile functions with "@jit" tag in doc-comments */
#define ZEND_JIT_ON_HOT_TRACE      5     /* trace functions after N calls or loop iterations */

#define ZEND_JIT_REG_ALLOC_LOCAL  (1<<0) /* local linear scan register allocation */
#define ZEND_JIT_REG_ALLOC_GLOBAL (1<<1) /* global linear scan register allocation */
#define ZEND_JIT_CPU_AVX          (1<<2) /* use AVX instructions, if available */

#define ZEND_JIT_DEFAULT_BUFFER_SIZE  "0"

#define ZEND_JIT_COUNTER_INIT         32531

#define ZEND_JIT_DEBUG_ASM       (1<<0)
#define ZEND_JIT_DEBUG_SSA       (1<<1)
#define ZEND_JIT_DEBUG_REG_ALLOC (1<<2)
#define ZEND_JIT_DEBUG_ASM_STUBS (1<<3)

#define ZEND_JIT_DEBUG_PERF      (1<<4)
#define ZEND_JIT_DEBUG_PERF_DUMP (1<<5)
#define ZEND_JIT_DEBUG_VTUNE     (1<<7)

#define ZEND_JIT_DEBUG_GDB       (1<<8)
#define ZEND_JIT_DEBUG_SIZE      (1<<9)
#define ZEND_JIT_DEBUG_ASM_ADDR  (1<<10)

#define ZEND_JIT_DEBUG_TRACE_START     (1<<12)
#define ZEND_JIT_DEBUG_TRACE_STOP      (1<<13)
#define ZEND_JIT_DEBUG_TRACE_COMPILED  (1<<14)
#define ZEND_JIT_DEBUG_TRACE_EXIT      (1<<15)
#define ZEND_JIT_DEBUG_TRACE_ABORT     (1<<16)
#define ZEND_JIT_DEBUG_TRACE_BLACKLIST (1<<17)
#define ZEND_JIT_DEBUG_TRACE_BYTECODE  (1<<18)
#define ZEND_JIT_DEBUG_TRACE_TSSA      (1<<19)
#define ZEND_JIT_DEBUG_TRACE_EXIT_INFO (1<<20)

#define ZEND_JIT_DEBUG_IR_SRC            (1<<24)
#define ZEND_JIT_DEBUG_IR_FINAL          (1<<25)
#define ZEND_JIT_DEBUG_IR_CFG            (1<<26)
#define ZEND_JIT_DEBUG_IR_REGS           (1<<27)

#define ZEND_JIT_DEBUG_IR_AFTER_SCCP     (1<<28)
#define ZEND_JIT_DEBUG_IR_AFTER_SCHEDULE (1<<29)
#define ZEND_JIT_DEBUG_IR_AFTER_REGS     (1<<30)
#define ZEND_JIT_DEBUG_IR_CODEGEN        (1U<<31)

#define ZEND_JIT_DEBUG_PERSISTENT      0x1f0 /* profile and debugger flags can't be changed at run-time */

#define ZEND_JIT_TRACE_MAX_LENGTH        1024 /* max length of single trace */
#define ZEND_JIT_TRACE_MAX_EXITS          512 /* max number of side exits per trace */

#define ZEND_JIT_TRACE_MAX_FUNCS           30 /* max number of different functions in a single trace */
#define ZEND_JIT_TRACE_MAX_CALL_DEPTH      10 /* max depth of inlined calls */
#define ZEND_JIT_TRACE_MAX_RET_DEPTH        4 /* max depth of inlined returns */
#define ZEND_JIT_TRACE_MAX_LOOPS_UNROLL    10 /* max number of unrolled loops */

#define ZEND_JIT_TRACE_BAD_ROOT_SLOTS      64 /* number of slots in bad root trace cache */

typedef struct _zend_jit_trace_rec zend_jit_trace_rec;
typedef struct _zend_jit_trace_stack_frame zend_jit_trace_stack_frame;
typedef struct _sym_node zend_sym_node;

typedef struct _zend_jit_globals {
	bool enabled;
	bool on;
	uint8_t   trigger;
	uint8_t   opt_level;
	uint32_t  opt_flags;

	const char *options;
	zend_long   buffer_size;
	zend_long   debug;
	zend_long   bisect_limit;
	double      prof_threshold;
	zend_long   max_root_traces;       /* max number of root traces */
	zend_long   max_side_traces;       /* max number of side traces (per root trace) */
	zend_long   max_exit_counters;     /* max total number of side exits for all traces */
	zend_long   hot_loop;
	zend_long   hot_func;
	zend_long   hot_return;
	zend_long   hot_side_exit;         /* number of exits before taking side trace */
	zend_long   blacklist_root_trace;  /* number of attempts to JIT a root trace before blacklist it */
	zend_long   blacklist_side_trace;  /* number of attempts to JIT a side trace before blacklist it */
	zend_long   max_loop_unrolls;      /* max number of unrolled loops */
	zend_long   max_recursive_calls;   /* max number of recursive inlined call unrolls */
	zend_long   max_recursive_returns; /* max number of recursive inlined return unrolls */
	zend_long   max_polymorphic_calls; /* max number of inlined polymorphic calls */
	zend_long   max_trace_length; 	   /* max length of a single trace */

	zend_sym_node *symbols;            /* symbols for disassembler */

	bool tracing;

	zend_jit_trace_rec *current_trace;
	zend_jit_trace_stack_frame *current_frame;

	const zend_op *bad_root_cache_opline[ZEND_JIT_TRACE_BAD_ROOT_SLOTS];
	uint8_t bad_root_cache_count[ZEND_JIT_TRACE_BAD_ROOT_SLOTS];
	uint8_t bad_root_cache_stop[ZEND_JIT_TRACE_BAD_ROOT_SLOTS];
	uint32_t bad_root_slot;

	uint8_t  *exit_counters;
} zend_jit_globals;

#ifdef ZTS
# define JIT_G(v) ZEND_TSRMG(jit_globals_id, zend_jit_globals *, v)
extern int jit_globals_id;
#else
# define JIT_G(v) (jit_globals.v)
extern zend_jit_globals jit_globals;
#endif

ZEND_EXT_API int  zend_jit_op_array(zend_op_array *op_array, zend_script *script);
ZEND_EXT_API int  zend_jit_script(zend_script *script);
ZEND_EXT_API void zend_jit_unprotect(void);
ZEND_EXT_API void zend_jit_protect(void);
ZEND_EXT_API void zend_jit_init(void);
ZEND_EXT_API int  zend_jit_config(zend_string *jit_options, int stage);
ZEND_EXT_API int  zend_jit_debug_config(zend_long old_val, zend_long new_val, int stage);
ZEND_EXT_API int  zend_jit_check_support(void);
ZEND_EXT_API int  zend_jit_startup(void *jit_buffer, size_t size, bool reattached);
ZEND_EXT_API void zend_jit_shutdown(void);
ZEND_EXT_API void zend_jit_activate(void);
ZEND_EXT_API void zend_jit_deactivate(void);
ZEND_EXT_API void zend_jit_status(zval *ret);
ZEND_EXT_API void zend_jit_restart(void);

#ifdef ZEND_JIT_IR

#define ZREG_LOAD           (1<<0)
#define ZREG_STORE          (1<<1)
#define ZREG_LAST_USE       (1<<2)

#define ZREG_PI             (1<<3)
#define ZREG_PHI            (1<<4)
#define ZREG_FORWARD        (1<<5)

#define ZREG_SPILL_SLOT     (1<<3)

#define ZREG_CONST          (1<<4)
#define ZREG_ZVAL_COPY      (2<<4)
#define ZREG_TYPE_ONLY      (3<<4)
#define ZREG_ZVAL_ADDREF    (4<<4)
#define ZREG_THIS           (5<<4)

#define ZREG_NONE           -1

#else
typedef struct _zend_lifetime_interval zend_lifetime_interval;
typedef struct _zend_life_range zend_life_range;

struct _zend_life_range {
	uint32_t         start;
	uint32_t         end;
	zend_life_range *next;
};

#define ZREG_FLAGS_SHIFT    8

#define ZREG_STORE          (1<<0)
#define ZREG_LOAD           (1<<1)
#define ZREG_LAST_USE       (1<<2)
#define ZREG_SPLIT          (1<<3)

struct _zend_lifetime_interval {
	int                     ssa_var;
	union {
		struct {
		ZEND_ENDIAN_LOHI_3(
			int8_t          reg,
			uint8_t         flags,
			uint16_t        reserved
		)};
		uint32_t            reg_flags;
	};
	zend_life_range         range;
	zend_lifetime_interval *hint;
	zend_lifetime_interval *used_as_hint;
	zend_lifetime_interval *list_next;
};
#endif

#endif /* HAVE_JIT_H */
