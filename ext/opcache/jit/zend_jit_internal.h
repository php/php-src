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

void ZEND_FASTCALL zend_jit_hot_func(zend_execute_data *execute_data, const zend_op *opline);

typedef struct _zend_jit_op_array_extension {
	int16_t    *counter;
	const void *orig_handlers[1];
} zend_jit_op_array_extension;

static zend_always_inline zend_long zend_jit_op_array_hash(const zend_op_array *op_array)
{
	uintptr_t x;

	x = (uintptr_t)op_array->opcodes >> 3;
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
#endif

/* VM handlers */
typedef ZEND_OPCODE_HANDLER_RET (ZEND_FASTCALL *zend_vm_opcode_handler_t)(ZEND_OPCODE_HANDLER_ARGS);

/* VM helpers */
ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_leave_nested_func_helper(uint32_t call_info EXECUTE_DATA_DC);
ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_leave_top_func_helper(uint32_t call_info EXECUTE_DATA_DC);

ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_profile_helper(ZEND_OPCODE_HANDLER_ARGS);

ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_func_counter_helper(ZEND_OPCODE_HANDLER_ARGS);
ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_loop_counter_helper(ZEND_OPCODE_HANDLER_ARGS);

void ZEND_FASTCALL zend_jit_copy_extra_args_helper(EXECUTE_DATA_D);
void ZEND_FASTCALL zend_jit_deprecated_helper(OPLINE_D);

void ZEND_FASTCALL zend_jit_get_constant(const zval *key, uint32_t flags);
int  ZEND_FASTCALL zend_jit_check_constant(const zval *key);

#endif /* ZEND_JIT_INTERNAL_H */
