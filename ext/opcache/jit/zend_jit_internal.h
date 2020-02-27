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
   |          Xinchen Hui <laruence@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_JIT_INTERNAL_H
#define ZEND_JIT_INTERNAL_H

/* Profiler */
extern zend_ulong zend_jit_profile_counter;
extern int zend_jit_profile_counter_rid;

#define ZEND_COUNTER_INFO(op_array) \
	ZEND_OP_ARRAY_EXTENSION(op_array, zend_jit_profile_counter_rid)

/* Hot Counters */

#define ZEND_HOT_COUNTERS_COUNT 128

extern int16_t zend_jit_hot_counters[ZEND_HOT_COUNTERS_COUNT];

static zend_always_inline zend_long zend_jit_hash(const void *ptr)
{
	uintptr_t x;

	x = (uintptr_t)ptr >> 3;
#if SIZEOF_SIZE_T == 4
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = (x >> 16) ^ x;
#elif SIZEOF_SIZE_T == 8
	x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
	x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
	x = x ^ (x >> 31);
#endif
	return x;
}

void ZEND_FASTCALL zend_jit_hot_func(zend_execute_data *execute_data, const zend_op *opline);

typedef struct _zend_jit_op_array_hot_extension {
	int16_t    *counter;
	const void *orig_handlers[1];
} zend_jit_op_array_hot_extension;

#define zend_jit_op_array_hash(op_array) \
	zend_jit_hash((op_array)->opcodes)

extern const zend_op *zend_jit_halt_op;

#ifdef HAVE_GCC_GLOBAL_REGS
# define EXECUTE_DATA_D                       void
# define EXECUTE_DATA_C
# define EXECUTE_DATA_DC
# define EXECUTE_DATA_CC
# define OPLINE_D                             void
# define OPLINE_C
# define OPLINE_DC
# define OPLINE_CC
# define ZEND_OPCODE_HANDLER_RET              void
# define ZEND_OPCODE_HANDLER_ARGS             EXECUTE_DATA_D
# define ZEND_OPCODE_HANDLER_ARGS_PASSTHRU
# define ZEND_OPCODE_HANDLER_ARGS_DC
# define ZEND_OPCODE_HANDLER_ARGS_PASSTHRU_CC
# define ZEND_OPCODE_RETURN()                 return
# define ZEND_OPCODE_TAIL_CALL(handler)       do { \
		handler(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU); \
		return; \
	} while(0)
# define ZEND_OPCODE_TAIL_CALL_EX(handler, arg) do { \
		handler(arg ZEND_OPCODE_HANDLER_ARGS_PASSTHRU_CC); \
		return; \
	} while(0)
#else
# define EXECUTE_DATA_D                       zend_execute_data* execute_data
# define EXECUTE_DATA_C                       execute_data
# define EXECUTE_DATA_DC                      , EXECUTE_DATA_D
# define EXECUTE_DATA_CC                      , EXECUTE_DATA_C
# define OPLINE_D                             const zend_op* opline
# define OPLINE_C                             opline
# define OPLINE_DC                            , OPLINE_D
# define OPLINE_CC                            , OPLINE_C
# define ZEND_OPCODE_HANDLER_RET              int
# define ZEND_OPCODE_HANDLER_ARGS             EXECUTE_DATA_D
# define ZEND_OPCODE_HANDLER_ARGS_PASSTHRU    EXECUTE_DATA_C
# define ZEND_OPCODE_HANDLER_ARGS_DC          EXECUTE_DATA_DC
# define ZEND_OPCODE_HANDLER_ARGS_PASSTHRU_CC EXECUTE_DATA_CC
# define ZEND_OPCODE_RETURN()                 return 0
# define ZEND_OPCODE_TAIL_CALL(handler)       do { \
		return handler(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU); \
	} while(0)
# define ZEND_OPCODE_TAIL_CALL_EX(handler, arg) do { \
		return handler(arg ZEND_OPCODE_HANDLER_ARGS_PASSTHRU_CC); \
	} while(0)
#endif

/* VM handlers */
typedef ZEND_OPCODE_HANDLER_RET (ZEND_FASTCALL *zend_vm_opcode_handler_t)(ZEND_OPCODE_HANDLER_ARGS);

/* VM helpers */
ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_leave_nested_func_helper(uint32_t call_info EXECUTE_DATA_DC);
ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_leave_top_func_helper(uint32_t call_info EXECUTE_DATA_DC);
ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_leave_func_helper(uint32_t call_info EXECUTE_DATA_DC);

ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_profile_helper(ZEND_OPCODE_HANDLER_ARGS);

ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_func_counter_helper(ZEND_OPCODE_HANDLER_ARGS);
ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_loop_counter_helper(ZEND_OPCODE_HANDLER_ARGS);

void ZEND_FASTCALL zend_jit_copy_extra_args_helper(EXECUTE_DATA_D);
void ZEND_FASTCALL zend_jit_deprecated_helper(OPLINE_D);

void ZEND_FASTCALL zend_jit_get_constant(const zval *key, uint32_t flags);
int  ZEND_FASTCALL zend_jit_check_constant(const zval *key);

/* Tracer */
#define zend_jit_opline_hash(opline) \
	zend_jit_hash(opline)

#define ZEND_JIT_TRACE_FUNC_COST      (1*250)
#define ZEND_JIT_TRACE_RET_COST     (1*250-1)
#define ZEND_JIT_TRACE_LOOP_COST      (2*250)
#define ZEND_JIT_TRACE_COUNTER_INIT (127*250)

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

#define ZEND_JIT_TRACE_STOP(_) \
	_(LOOP,              "loop") \
	_(RECURSIVE_CALL,    "recursive call") \
	_(RECURSIVE_RET,     "recursive return") \
	_(RETURN,            "return") \
	_(LINK,              "link to another trace") \
	/* comiplation and linking successful */ \
	_(COMPILED,          "compiled") \
	_(ALREADY_DONE,      "already prcessed") \
	/* failures */ \
	_(ERROR,             "error")                          /* not used */ \
	_(NOT_SUPPORTED,     "not supported instructions") \
	_(EXCEPTION,         "exception") \
	_(TOO_LONG,          "trace too long") \
	_(TOO_DEEP,          "trace too deep") \
	_(TOO_DEEP_RET,      "trace too deep return") \
	_(DEEP_RECURSION,    "deep recursion") \
	_(LOOP_UNROLL,       "loop unroll limit reached") \
	_(LOOP_EXIT,         "exit from loop") \
	_(BLACK_LIST,        "trace blacklisted") \
	_(INNER_LOOP,        "inner loop")                     /* trace it */ \
	_(COMPILED_LOOP,     "compiled loop") \
	_(TOPLEVEL,          "toplevel") \
	_(TRAMPOLINE,        "trampoline call") \
	_(BAD_FUNC,          "bad function call") \
	_(HALT,              "exit from interpreter") \
	_(COMPILER_ERROR,    "JIT compilation error") \
	/* no recoverable error (blacklist innediately) */ \
	_(NO_SHM,            "insufficient shared memory") \
	_(TOO_MANY_TRACES,   "too many traces") \
	_(TOO_MANY_CHILDREN, "too many side traces") \
	_(TOO_MANY_EXITS,    "too many side exits") \

#define ZEND_JIT_TRACE_STOP_NAME(name, description) \
	ZEND_JIT_TRACE_STOP_ ## name,

typedef enum _zend_jit_trace_stop {
	ZEND_JIT_TRACE_STOP(ZEND_JIT_TRACE_STOP_NAME)
} zend_jit_trace_stop;

#define ZEND_JIT_TRACE_STOP_OK(ret) \
	(ret < ZEND_JIT_TRACE_STOP_COMPILED)

#define ZEND_JIT_TRACE_STOP_DONE(ret) \
	(ret < ZEND_JIT_TRACE_STOP_ERROR)

#define ZEND_JIT_TRACE_STOP_REPEAT(ret) \
	(ret == ZEND_JIT_TRACE_STOP_INNER_LOOP)

#define ZEND_JIT_TRACE_STOP_MAY_RECOVER(ret) \
	(ret <= ZEND_JIT_TRACE_STOP_COMPILER_ERROR)

#define ZEND_JIT_TRACE_START_MASK      0xf

#define ZEND_JIT_TRACE_START_LOOP   (1<<0)
#define ZEND_JIT_TRACE_START_ENTER  (1<<1)
#define ZEND_JIT_TRACE_START_RETURN (1<<2)
#define ZEND_JIT_TRACE_START_SIDE   (1<<3) /* used for side traces */

#define ZEND_JIT_TRACE_JITED        (1<<4)
#define ZEND_JIT_TRACE_BLACKLISTED  (1<<5)
#define ZEND_JIT_TRACE_UNSUPPORTED  (1<<6)

#define ZEND_JIT_TRACE_SUPPORTED    0

#define ZEND_JIT_EXIT_JITED         (1<<0)
#define ZEND_JIT_EXIT_BLACKLISTED   (1<<1)

typedef union _zend_op_trace_info {
	zend_op dummy; /* the size of this structure must be the same as zend_op */
	struct {
		const void *orig_handler;
		const void *call_handler;
		int16_t    *counter;
		uint8_t     trace_flags;
	};
} zend_op_trace_info;

typedef struct _zend_jit_op_array_trace_extension {
	zend_func_info func_info;
	size_t offset; /* offset from "zend_op" to corresponding "op_info" */
	zend_op_trace_info trace_info[1];
} zend_jit_op_array_trace_extension;

#define ZEND_OP_TRACE_INFO(opline, offset) \
	((zend_op_trace_info*)(((char*)opline) + offset))

/* Recorder */
typedef enum _zend_jit_trace_op {
	ZEND_JIT_TRACE_VM,
	ZEND_JIT_TRACE_OP1_TYPE,
	ZEND_JIT_TRACE_OP2_TYPE,
	ZEND_JIT_TRACE_INIT_CALL,
	ZEND_JIT_TRACE_DO_ICALL,
	ZEND_JIT_TRACE_ENTER,
	ZEND_JIT_TRACE_BACK,
	ZEND_JIT_TRACE_START,
	ZEND_JIT_TRACE_END,
} zend_jit_trace_op;

#define IS_UNKNOWN 255 /* may be used for zend_jit_trace_rec.op?_type */
#define IS_TRACE_REFERENCE (1<<5)

typedef struct _zend_jit_trace_rec {
	uint8_t   op;    /* zend_jit_trace_op */
	union {
		struct {
			uint8_t op1_type;/* recorded zval op1_type for ZEND_JIT_TRACE_VM */
			uint8_t op2_type;/* recorded zval op2_type for ZEND_JIT_TRACE_VM */
			uint8_t op3_type;/* recorded zval for op_data.op1_type for ZEND_JIT_TRACE_VM */
		};
		uint8_t   recursive; /* part of recursive return sequence for ZEND_JIT_TRACE_BACK */
		int8_t    return_value_used; /* for ZEND_JIT_TRACE_ENTER */
		uint8_t   fake; /* for ZEND_JIT_TRACE_INIT_FCALL */
		struct {
			uint8_t  start;  /* ZEND_JIT_TRACE_START_MASK for ZEND_JIT_TRACE_START/END */
			uint8_t  stop;   /* zend_jit_trace_stop for ZEND_JIT_TRACE_START/END */
			uint8_t  level;  /* recursive return level for ZEND_JIT_TRACE_START */
		};
	};
	union {
		const void             *ptr;
		const zend_function    *func;
		const zend_op_array    *op_array;
		const zend_op          *opline;
		const zend_class_entry *ce;
	};
} zend_jit_trace_rec;

typedef struct _zend_jit_trace_start_rec {
	uint8_t  op;     /* zend_jit_trace_op */
	uint8_t  start;  /* ZEND_JIT_TRACE_START_MASK for ZEND_JIT_TRACE_START/END */
	uint8_t  stop;   /* zend_jit_trace_stop for ZEND_JIT_TRACE_START/END */
	uint8_t  level;  /* recursive return level for ZEND_JIT_TRACE_START */
	const zend_op_array *op_array;
	const zend_op *opline;
} zend_jit_trace_start_rec;

#define ZEND_JIT_TRACE_START_REC_SIZE 2

typedef struct _zend_jit_trace_exit_info {
	const zend_op *opline;     /* opline where VM should continue execution */
	uint32_t       stack_size;
	uint32_t       stack_offset;
} zend_jit_trace_exit_info;

typedef int32_t zend_jit_trace_stack;

typedef struct _zend_jit_trace_info {
	uint32_t                  id;            /* trace id */
	uint32_t                  root;          /* root trace id or sekf id for root traces */
	uint32_t                  parent;        /* parent trace id or 0 for root traces */
	uint32_t                  link;          /* link trace id or self id for loop) */
	uint32_t                  exit_count;    /* number of side exits */
	uint32_t                  child_count;   /* number of side traces for root traces */
	uint32_t                  code_size;     /* size of native code */
	uint32_t                  exit_counters; /* offset in exit counters array */
	uint32_t                  stack_map_size;
	const void               *code_start;    /* address of native code */
	zend_jit_trace_exit_info *exit_info;     /* address of native code */
	zend_jit_trace_stack     *stack_map;
	//uint32_t    loop_offset;
} zend_jit_trace_info;

typedef struct _zend_jit_trace_stack_frame zend_jit_trace_stack_frame;

struct _zend_jit_trace_stack_frame {
	zend_jit_trace_stack_frame *call;
	zend_jit_trace_stack_frame *prev;
	const zend_function        *func;
	union {
		int8_t                  return_value_used;
		int                     return_ssa_var;
	};
	zend_jit_trace_stack        stack[1];
};

typedef struct _zend_jit_globals {
	zend_jit_trace_stack_frame *current_frame;

	const zend_op *bad_root_cache_opline[ZEND_JIT_TRACE_BAD_ROOT_SLOTS];
	uint8_t bad_root_cache_count[ZEND_JIT_TRACE_BAD_ROOT_SLOTS];
	uint8_t bad_root_cache_stop[ZEND_JIT_TRACE_BAD_ROOT_SLOTS];
	uint32_t bad_root_slot;

	uint8_t  exit_counters[ZEND_JIT_TRACE_MAX_EXIT_COUNTERS];
} zend_jit_globals;

#ifdef ZTS
# define JIT_G(v) ZEND_TSRMG(zend_jit_globals_id, zend_jit_globals *, v)
extern int zend_jit_globals_id;
#else
# define JIT_G(v) (jit_globals.v)
extern zend_jit_globals jit_globals;
#endif

ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_func_trace_helper(ZEND_OPCODE_HANDLER_ARGS);
ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_ret_trace_helper(ZEND_OPCODE_HANDLER_ARGS);
ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_loop_trace_helper(ZEND_OPCODE_HANDLER_ARGS);

int ZEND_FASTCALL zend_jit_trace_hot_root(zend_execute_data *execute_data, const zend_op *opline);
int ZEND_FASTCALL zend_jit_trace_exit(uint32_t trace_num, uint32_t exit_num);
zend_jit_trace_stop ZEND_FASTCALL zend_jit_trace_execute(zend_execute_data *execute_data, const zend_op *opline, zend_jit_trace_rec *trace_buffer, uint8_t start);

static zend_always_inline const zend_op* zend_jit_trace_get_exit_opline(zend_jit_trace_rec *trace, const zend_op *opline, zend_bool *exit_if_true)
{
	if (trace->op == ZEND_JIT_TRACE_VM || trace->op == ZEND_JIT_TRACE_END) {
		if (trace->opline == opline + 1) {
			/* not taken branch */
			*exit_if_true = opline->opcode == ZEND_JMPNZ;
			return OP_JMP_ADDR(opline, opline->op2);
		} else if (trace->opline == OP_JMP_ADDR(opline, opline->op2)) {
			/* taken branch */
			*exit_if_true = opline->opcode == ZEND_JMPZ;
			return opline + 1;
		} else {
			ZEND_ASSERT(0);
		}
	} else  {
		ZEND_ASSERT(0);
	}
	*exit_if_true = 0;
	return NULL;
}

#endif /* ZEND_JIT_INTERNAL_H */
