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

#ifndef HAVE_JIT_H
#define HAVE_JIT_H

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

//#define ZEND_JIT_LEVEL(n)          ((n) % 10)
//#define ZEND_JIT_TRIGGER(n)        (((n) / 10) % 10)
//#define ZEND_JIT_REG_ALLOC(n)      (((n) / 100) % 10)
//#define ZEND_JIT_CPU_FLAGS(n)      (((n) / 1000) % 10)

#define ZEND_JIT_DEFAULT_OPTIONS      "1205"
#define ZEND_JIT_DEFAULT_BUFFER_SIZE  "0"


/* Makes profile based JIT (opcache.jit=2*) to generate code only for most
 * often called functions (above the threshold).
 * TODO: this setting should be configurable
 */
#define ZEND_JIT_PROF_THRESHOLD    0.005

/* Hot/Trace Counters based JIT parameters.
 * TODO: this setting should be configurable
 */
#define ZEND_JIT_COUNTER_FUNC_COST    1
#define ZEND_JIT_COUNTER_RET_COST    15
#define ZEND_JIT_COUNTER_LOOP_COST    2
#define ZEND_JIT_COUNTER_INIT       127

#define ZEND_JIT_DEBUG_ASM       (1<<0)
#define ZEND_JIT_DEBUG_SSA       (1<<1)
#define ZEND_JIT_DEBUG_REG_ALLOC (1<<2)
#define ZEND_JIT_DEBUG_ASM_STUBS (1<<3)

#define ZEND_JIT_DEBUG_PERF      (1<<4)
#define ZEND_JIT_DEBUG_PERF_DUMP (1<<5)
#define ZEND_JIT_DEBUG_OPROFILE  (1<<6)
#define ZEND_JIT_DEBUG_VTUNE     (1<<7)

#define ZEND_JIT_DEBUG_GDB       (1<<8)

#define ZEND_JIT_DEBUG_TRACE_START     (1<<12)
#define ZEND_JIT_DEBUG_TRACE_STOP      (1<<13)
#define ZEND_JIT_DEBUG_TRACE_COMPILED  (1<<14)
#define ZEND_JIT_DEBUG_TRACE_EXIT      (1<<15)
#define ZEND_JIT_DEBUG_TRACE_ABORT     (1<<16)
#define ZEND_JIT_DEBUG_TRACE_BLACKLIST (1<<17)
#define ZEND_JIT_DEBUG_TRACE_BYTECODE  (1<<18)
#define ZEND_JIT_DEBUG_TRACE_TSSA      (1<<19)
#define ZEND_JIT_DEBUG_TRACE_EXIT_INFO (1<<20)

#define ZEND_JIT_DEBUG_PERSISTENT      0x1f0 /* profile and debbuger flags can't be changed at run-time */

#define ZEND_JIT_TRACE_MAX_TRACES        1024 /* max number of traces */
#define ZEND_JIT_TRACE_MAX_LENGTH        1024 /* max length of single trace */
#define ZEND_JIT_TRACE_MAX_EXITS          512 /* max number of side exits per trace */
#define ZEND_JIT_TRACE_MAX_SIDE_TRACES    128 /* max number of side traces of a root trace */
#define ZEND_JIT_TRACE_MAX_EXIT_COUNTERS 8192 /* max number of side exits for all trace */

#define ZEND_JIT_TRACE_MAX_FUNCS           30 /* max number of different functions in a single trace */
#define ZEND_JIT_TRACE_MAX_CALL_DEPTH      10 /* max depth of inlined calls */
#define ZEND_JIT_TRACE_MAX_RET_DEPTH        4 /* max depth of inlined returns */
#define ZEND_JIT_TRACE_MAX_RECURSION        2 /* max number of recursive inlined calls */
#define ZEND_JIT_TRACE_MAX_UNROLL_LOOPS     8 /* max number of unrolled loops */

#define ZEND_JIT_TRACE_HOT_SIDE_COUNT       8 /* number of exits before taking side trace */
#define ZEND_JIT_TRACE_HOT_RETURN_COUNT     8 /* number of returns before taking continuation trace */

#define ZEND_JIT_TRACE_MAX_ROOT_FAILURES   16 /* number of attemts to record/compile a root trace */
#define ZEND_JIT_TRACE_MAX_SIDE_FAILURES    4 /* number of attemts to record/compile a side trace */

#define ZEND_JIT_TRACE_BAD_ROOT_SLOTS      64 /* number of slots in bad root trace cache */

typedef struct _zend_jit_trace_rec zend_jit_trace_rec;
typedef struct _zend_jit_trace_stack_frame zend_jit_trace_stack_frame;
typedef struct _sym_node zend_sym_node;

typedef struct _zend_jit_globals {
	zend_bool enabled;
	zend_bool on;
	uint8_t   trigger;
	uint8_t   opt_level;
	uint32_t  opt_flags;

	const char *options;
	zend_long   buffer_size;
	zend_long   debug;
	zend_long   bisect_limit;

	zend_sym_node *symbols; /* symbols for disassembler */

	zend_jit_trace_rec *current_trace;
	zend_jit_trace_stack_frame *current_frame;

	const zend_op *bad_root_cache_opline[ZEND_JIT_TRACE_BAD_ROOT_SLOTS];
	uint8_t bad_root_cache_count[ZEND_JIT_TRACE_BAD_ROOT_SLOTS];
	uint8_t bad_root_cache_stop[ZEND_JIT_TRACE_BAD_ROOT_SLOTS];
	uint32_t bad_root_slot;

	uint8_t  exit_counters[ZEND_JIT_TRACE_MAX_EXIT_COUNTERS];
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
ZEND_EXT_API int  zend_jit_startup(void *jit_buffer, size_t size, zend_bool reattached);
ZEND_EXT_API void zend_jit_shutdown(void);
ZEND_EXT_API void zend_jit_activate(void);
ZEND_EXT_API void zend_jit_deactivate(void);
ZEND_EXT_API void zend_jit_status(zval *ret);

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

#endif /* HAVE_JIT_H */
