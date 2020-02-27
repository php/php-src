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

#include "Zend/zend_execute.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_vm.h"
#include "Zend/zend_closures.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_API.h"

#include <ZendAccelerator.h>
#include "Optimizer/zend_func_info.h"
#include "Optimizer/zend_call_graph.h"
#include "zend_jit.h"
#include "zend_jit_internal.h"

#ifdef HAVE_GCC_GLOBAL_REGS
# pragma GCC diagnostic ignored "-Wvolatile-register-var"
# if defined(__x86_64__)
register zend_execute_data* volatile execute_data __asm__("%r14");
register const zend_op* volatile opline __asm__("%r15");
# else
register zend_execute_data* volatile execute_data __asm__("%esi");
register const zend_op* volatile opline __asm__("%edi");
# endif
# pragma GCC diagnostic warning "-Wvolatile-register-var"
#endif

ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_leave_nested_func_helper(uint32_t call_info EXECUTE_DATA_DC)
{
	zend_execute_data *old_execute_data;

	if (UNEXPECTED(call_info & ZEND_CALL_HAS_SYMBOL_TABLE)) {
		zend_clean_and_cache_symbol_table(EX(symbol_table));
	}

	zend_vm_stack_free_extra_args_ex(call_info, execute_data);
	if (UNEXPECTED(call_info & ZEND_CALL_RELEASE_THIS)) {
		OBJ_RELEASE(Z_OBJ(execute_data->This));
	} else if (UNEXPECTED(call_info & ZEND_CALL_CLOSURE)) {
		OBJ_RELEASE(ZEND_CLOSURE_OBJECT(EX(func)));
	}

	old_execute_data = execute_data;
	execute_data = EX(prev_execute_data);
	zend_vm_stack_free_call_frame_ex(call_info, old_execute_data);

	if (UNEXPECTED(EG(exception) != NULL)) {
		const zend_op *old_opline = EX(opline);
		zend_throw_exception_internal(NULL);
		if (old_opline->result_type != IS_UNDEF) {
			zval_ptr_dtor(EX_VAR(old_opline->result.var));
		}
#ifndef HAVE_GCC_GLOBAL_REGS
		return 2; // ZEND_VM_LEAVE
#endif
	} else {
		EX(opline)++;
#ifdef HAVE_GCC_GLOBAL_REGS
		opline = EX(opline);
#else
		return 2; // ZEND_VM_LEAVE
#endif
	}
}

ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_leave_top_func_helper(uint32_t call_info EXECUTE_DATA_DC)
{
	if (UNEXPECTED(call_info & (ZEND_CALL_HAS_SYMBOL_TABLE|ZEND_CALL_FREE_EXTRA_ARGS))) {
		if (UNEXPECTED(call_info & ZEND_CALL_HAS_SYMBOL_TABLE)) {
			zend_clean_and_cache_symbol_table(EX(symbol_table));
		}
		zend_vm_stack_free_extra_args_ex(call_info, execute_data);
	}
	if (UNEXPECTED(call_info & ZEND_CALL_CLOSURE)) {
		OBJ_RELEASE(ZEND_CLOSURE_OBJECT(EX(func)));
	}
	execute_data = EG(current_execute_data);
#ifdef HAVE_GCC_GLOBAL_REGS
	opline = zend_jit_halt_op;
#else
	return -1; // ZEND_VM_RETURN
#endif
}

ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_leave_func_helper(uint32_t call_info EXECUTE_DATA_DC)
{
	if (call_info & ZEND_CALL_TOP) {
		ZEND_OPCODE_TAIL_CALL_EX(zend_jit_leave_top_func_helper, call_info);
	} else {
		ZEND_OPCODE_TAIL_CALL_EX(zend_jit_leave_nested_func_helper, call_info);
	}
}

void ZEND_FASTCALL zend_jit_copy_extra_args_helper(EXECUTE_DATA_D)
{
	zend_op_array *op_array = &EX(func)->op_array;

	if (EXPECTED(!(op_array->fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE))) {
		uint32_t first_extra_arg = op_array->num_args;
		uint32_t num_args = EX_NUM_ARGS();
		zval *end, *src, *dst;
		uint32_t type_flags = 0;

		if (EXPECTED((op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS) == 0)) {
			/* Skip useless ZEND_RECV and ZEND_RECV_INIT opcodes */
#ifdef HAVE_GCC_GLOBAL_REGS
			opline += first_extra_arg;
#endif
		}

		/* move extra args into separate array after all CV and TMP vars */
		end = EX_VAR_NUM(first_extra_arg - 1);
		src = end + (num_args - first_extra_arg);
		dst = src + (op_array->last_var + op_array->T - first_extra_arg);
		if (EXPECTED(src != dst)) {
			do {
				type_flags |= Z_TYPE_INFO_P(src);
				ZVAL_COPY_VALUE(dst, src);
				ZVAL_UNDEF(src);
				src--;
				dst--;
			} while (src != end);
			if (type_flags & (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT)) {
				ZEND_ADD_CALL_FLAG(execute_data, ZEND_CALL_FREE_EXTRA_ARGS);
			}
		} else {
			do {
				if (Z_REFCOUNTED_P(src)) {
					ZEND_ADD_CALL_FLAG(execute_data, ZEND_CALL_FREE_EXTRA_ARGS);
					break;
				}
				src--;
			} while (src != end);
		}
	}
}

void ZEND_FASTCALL zend_jit_deprecated_helper(OPLINE_D)
{
	zend_execute_data *call = (zend_execute_data *) opline;
	zend_function *fbc = call->func;
	zend_error(E_DEPRECATED, "Function %s%s%s() is deprecated",
		fbc->common.scope ? ZSTR_VAL(fbc->common.scope->name) : "",
		fbc->common.scope ? "::" : "",
		ZSTR_VAL(fbc->common.function_name));
	if (EG(exception)) {
#ifndef HAVE_GCC_GLOBAL_REGS
		zend_execute_data *execute_data = EG(current_execute_data);
#endif
		const zend_op *opline = EG(opline_before_exception);
		if (RETURN_VALUE_USED(opline)) {
			ZVAL_UNDEF(EX_VAR(opline->result.var));
		}

		zend_vm_stack_free_args(call);

		if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_RELEASE_THIS)) {
			OBJ_RELEASE(Z_OBJ(call->This));
		}

		zend_vm_stack_free_call_frame(call);
	}
}

ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_profile_helper(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_op_array *op_array = (zend_op_array*)EX(func);
	zend_vm_opcode_handler_t handler = (zend_vm_opcode_handler_t)ZEND_FUNC_INFO(op_array);
	uintptr_t counter = (uintptr_t)ZEND_COUNTER_INFO(op_array);

	ZEND_COUNTER_INFO(op_array) = (void*)(counter + 1);
	++zend_jit_profile_counter;
	ZEND_OPCODE_TAIL_CALL(handler);
}

ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_func_counter_helper(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_jit_op_array_hot_extension *jit_extension =
		(zend_jit_op_array_hot_extension*)ZEND_FUNC_INFO(&EX(func)->op_array);
#ifndef HAVE_GCC_GLOBAL_REGS
	const zend_op *opline = EX(opline);
#endif

	*(jit_extension->counter) -= ZEND_JIT_HOT_FUNC_COST;

	if (UNEXPECTED(*(jit_extension->counter) <= 0)) {
		*(jit_extension->counter) = ZEND_JIT_HOT_COUNTER_INIT;
		zend_jit_hot_func(execute_data, opline);
		ZEND_OPCODE_RETURN();
	} else {
		zend_vm_opcode_handler_t handler = (zend_vm_opcode_handler_t)jit_extension->orig_handlers[opline - EX(func)->op_array.opcodes];
		ZEND_OPCODE_TAIL_CALL(handler);
	}
}

ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_loop_counter_helper(ZEND_OPCODE_HANDLER_ARGS)
{
	zend_jit_op_array_hot_extension *jit_extension =
		(zend_jit_op_array_hot_extension*)ZEND_FUNC_INFO(&EX(func)->op_array);
#ifndef HAVE_GCC_GLOBAL_REGS
	const zend_op *opline = EX(opline);
#endif

	*(jit_extension->counter) -= ZEND_JIT_HOT_LOOP_COST;

	if (UNEXPECTED(*(jit_extension->counter) <= 0)) {
		*(jit_extension->counter) = ZEND_JIT_HOT_COUNTER_INIT;
		zend_jit_hot_func(execute_data, opline);
		ZEND_OPCODE_RETURN();
	} else {
		zend_vm_opcode_handler_t handler = (zend_vm_opcode_handler_t)jit_extension->orig_handlers[opline - EX(func)->op_array.opcodes];
		ZEND_OPCODE_TAIL_CALL(handler);
	}
}

static zend_always_inline int _zend_quick_get_constant(
		const zval *key, uint32_t flags, int check_defined_only)
{
#ifndef HAVE_GCC_GLOBAL_REGS
	zend_execute_data *execute_data = EG(current_execute_data);
#endif
	const zend_op *opline = EX(opline);
	zval *zv;
	zend_constant *c = NULL;

	/* null/true/false are resolved during compilation, so don't check for them here. */
	zv = zend_hash_find_ex(EG(zend_constants), Z_STR_P(key), 1);
	if (zv) {
		c = (zend_constant*)Z_PTR_P(zv);
	} else if (flags & IS_CONSTANT_UNQUALIFIED_IN_NAMESPACE) {
		key++;
		zv = zend_hash_find_ex(EG(zend_constants), Z_STR_P(key), 1);
		if (zv) {
			c = (zend_constant*)Z_PTR_P(zv);
		}
	}

	if (!c) {
		if (!check_defined_only) {
			zend_throw_error(NULL, "Undefined constant '%s'", Z_STRVAL_P(RT_CONSTANT(opline, opline->op2)));
			ZVAL_UNDEF(EX_VAR(opline->result.var));
		}
		CACHE_PTR(opline->extended_value, ENCODE_SPECIAL_CACHE_NUM(zend_hash_num_elements(EG(zend_constants))));
		return FAILURE;
	}

	if (!check_defined_only) {
		ZVAL_COPY_OR_DUP(EX_VAR(opline->result.var), &c->value);
	}

	CACHE_PTR(opline->extended_value, c);
	return SUCCESS;
}

void ZEND_FASTCALL zend_jit_get_constant(const zval *key, uint32_t flags)
{
	_zend_quick_get_constant(key, flags, 0);
}

int ZEND_FASTCALL zend_jit_check_constant(const zval *key)
{
	return _zend_quick_get_constant(key, 0, 1);
}

static zend_always_inline ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_trace_counter_helper(uint32_t cost ZEND_OPCODE_HANDLER_ARGS_DC)
{
	zend_jit_op_array_trace_extension *jit_extension =
		(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(&EX(func)->op_array);
	size_t offset = jit_extension->offset;
#ifndef HAVE_GCC_GLOBAL_REGS
	const zend_op *opline = EX(opline);
#endif

	*(ZEND_OP_TRACE_INFO(opline, offset)->counter) -= ZEND_JIT_TRACE_FUNC_COST;

	if (UNEXPECTED(*(ZEND_OP_TRACE_INFO(opline, offset)->counter) <= 0)) {
		*(ZEND_OP_TRACE_INFO(opline, offset)->counter) = ZEND_JIT_TRACE_COUNTER_INIT;
		if (UNEXPECTED(zend_jit_trace_hot_root(execute_data, opline) < 0)) {
#ifndef HAVE_GCC_GLOBAL_REGS
			return -1;
#endif
		}
#ifdef HAVE_GCC_GLOBAL_REGS
		execute_data = EG(current_execute_data);
		opline = EX(opline);
		return;
#else
		return 1;
#endif
	} else {
		zend_vm_opcode_handler_t handler = (zend_vm_opcode_handler_t)ZEND_OP_TRACE_INFO(opline, offset)->orig_handler;
		ZEND_OPCODE_TAIL_CALL(handler);
	}
}

ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_func_trace_helper(ZEND_OPCODE_HANDLER_ARGS)
{
	ZEND_OPCODE_TAIL_CALL_EX(zend_jit_trace_counter_helper, ZEND_JIT_TRACE_FUNC_COST);
}

ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_ret_trace_helper(ZEND_OPCODE_HANDLER_ARGS)
{
	ZEND_OPCODE_TAIL_CALL_EX(zend_jit_trace_counter_helper, ZEND_JIT_TRACE_RET_COST);
}

ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_jit_loop_trace_helper(ZEND_OPCODE_HANDLER_ARGS)
{
	ZEND_OPCODE_TAIL_CALL_EX(zend_jit_trace_counter_helper, ZEND_JIT_TRACE_LOOP_COST);
}

#define TRACE_RECORD(_op, _ptr) \
	trace_buffer[idx].op = _op; \
	trace_buffer[idx].ptr = _ptr; \
	idx++; \
	if (idx >= ZEND_JIT_TRACE_MAX_LENGTH - 1) { \
		stop = ZEND_JIT_TRACE_STOP_TOO_LONG; \
		break; \
	}

#define TRACE_RECORD_VM(_op, _ptr, _op1_type, _op2_type, _op3_type) \
	trace_buffer[idx].op = _op; \
	trace_buffer[idx].op1_type = _op1_type; \
	trace_buffer[idx].op2_type = _op2_type; \
	trace_buffer[idx].op3_type = _op3_type; \
	trace_buffer[idx].ptr = _ptr; \
	idx++; \
	if (idx >= ZEND_JIT_TRACE_MAX_LENGTH - 1) { \
		stop = ZEND_JIT_TRACE_STOP_TOO_LONG; \
		break; \
	}

#define TRACE_RECORD_ENTER(_op, _return_value_used, _ptr) \
	trace_buffer[idx].op = _op; \
	trace_buffer[idx].return_value_used = _return_value_used; \
	trace_buffer[idx].ptr = _ptr; \
	idx++; \
	if (idx >= ZEND_JIT_TRACE_MAX_LENGTH - 1) { \
		stop = ZEND_JIT_TRACE_STOP_TOO_LONG; \
		break; \
	}

#define TRACE_RECORD_INIT(_op, _fake, _ptr) \
	trace_buffer[idx].op = _op; \
	trace_buffer[idx].fake = _fake; \
	trace_buffer[idx].ptr = _ptr; \
	idx++; \
	if (idx >= ZEND_JIT_TRACE_MAX_LENGTH - 1) { \
		stop = ZEND_JIT_TRACE_STOP_TOO_LONG; \
		break; \
	}

#define TRACE_RECORD_BACK(_op, _recursive, _ptr) \
	trace_buffer[idx].op = _op; \
	trace_buffer[idx].recursive = _recursive; \
	trace_buffer[idx].ptr = _ptr; \
	idx++; \
	if (idx >= ZEND_JIT_TRACE_MAX_LENGTH - 1) { \
		stop = ZEND_JIT_TRACE_STOP_TOO_LONG; \
		break; \
	}

#define TRACE_RECORD_TYPE(_op, _ptr) \
	trace_buffer[idx].op = _op; \
	trace_buffer[idx].ptr = _ptr; \
	idx++; \
	if (idx >= ZEND_JIT_TRACE_MAX_LENGTH - 1) { \
		stop = ZEND_JIT_TRACE_STOP_TOO_LONG; \
		break; \
	}

#define TRACE_START(_op, _start, _ptr) \
	trace_buffer[0].op = _op; \
	trace_buffer[0].start = _start; \
	trace_buffer[0].level = 0; \
	trace_buffer[0].ptr = _ptr; \
	idx = ZEND_JIT_TRACE_START_REC_SIZE;

#define TRACE_END(_op, _stop, _ptr) \
	trace_buffer[idx].op   = _op; \
	trace_buffer[idx].start = trace_buffer[idx].start; \
	trace_buffer[idx].stop = trace_buffer[0].stop = _stop; \
	trace_buffer[idx].level = trace_buffer[0].level = ret_level ? ret_level + 1 : 0; \
	trace_buffer[idx].ptr  = _ptr;

#ifndef ZEND_JIT_RECORD_RECURSIVE_RETURN
# define ZEND_JIT_RECORD_RECURSIVE_RETURN 1
#endif

static int zend_jit_trace_recursive_call_count(const zend_op_array *op_array, const zend_op_array **unrolled_calls, int ret_level, int level)
{
	int i;
	int count = 0;

	for (i = ret_level; i < level; i++) {
		count += (unrolled_calls[i] == op_array);
	}
	return count;
}

static int zend_jit_trace_recursive_ret_count(const zend_op_array *op_array, const zend_op_array **unrolled_calls, int ret_level)
{
	int i;
	int count = 0;

	for (i = 0; i < ret_level; i++) {
		count += (unrolled_calls[i] == op_array);
	}
	return count;
}

static int zend_jit_trace_has_recursive_ret(zend_execute_data *ex, const zend_op_array *orig_op_array, const zend_op *orig_opline, int ret_level)
{
	while (ex != NULL && ret_level < ZEND_JIT_TRACE_MAX_RET_DEPTH) {
		if (&ex->func->op_array == orig_op_array && ex->opline + 1 == orig_opline) {
			return 1;
		}
		ex = ex->prev_execute_data;
		ret_level++;
	}
	return 0;
}

static int zend_jit_trace_bad_inner_loop(const zend_op *opline)
{
	const zend_op **cache_opline = JIT_G(bad_root_cache_opline);
	uint8_t *cache_count = JIT_G(bad_root_cache_count);
	uint8_t *cache_stop = JIT_G(bad_root_cache_stop);
	uint32_t i;

	for (i = 0; i < ZEND_JIT_TRACE_BAD_ROOT_SLOTS; i++) {
		if (cache_opline[i] == opline) {
			if ((cache_stop[i] == ZEND_JIT_TRACE_STOP_INNER_LOOP
			  || cache_stop[i] == ZEND_JIT_TRACE_STOP_LOOP_EXIT)
			 && cache_count[i] > ZEND_JIT_TRACE_MAX_ROOT_FAILURES / 2) {
				return 1;
			}
			break;
		}
	}
	return 0;
}

static int zend_jit_trace_bad_compiled_loop(const zend_op *opline)
{
	const zend_op **cache_opline = JIT_G(bad_root_cache_opline);
	uint8_t *cache_count = JIT_G(bad_root_cache_count);
	uint8_t *cache_stop = JIT_G(bad_root_cache_stop);
	uint32_t i;

	for (i = 0; i < ZEND_JIT_TRACE_BAD_ROOT_SLOTS; i++) {
		if (cache_opline[i] == opline) {
			if (cache_stop[i] == ZEND_JIT_TRACE_STOP_COMPILED_LOOP
			 && cache_count[i] >= ZEND_JIT_TRACE_MAX_ROOT_FAILURES - 1) {
				return 1;
			}
			break;
		}
	}
	return 0;
}

static int zend_jit_trace_bad_loop_exit(const zend_op *opline)
{
	const zend_op **cache_opline = JIT_G(bad_root_cache_opline);
	uint8_t *cache_count = JIT_G(bad_root_cache_count);
	uint8_t *cache_stop = JIT_G(bad_root_cache_stop);
	uint32_t i;

	for (i = 0; i < ZEND_JIT_TRACE_BAD_ROOT_SLOTS; i++) {
		if (cache_opline[i] == opline) {
			if (cache_stop[i] == ZEND_JIT_TRACE_STOP_LOOP_EXIT
			 && cache_count[i] >= ZEND_JIT_TRACE_MAX_ROOT_FAILURES - 1) {
				return 1;
			}
			break;
		}
	}
	return 0;
}

static int zend_jit_trace_record_fake_init_call(zend_execute_data *call, zend_jit_trace_rec *trace_buffer, int idx)
{
	zend_jit_trace_stop stop ZEND_ATTRIBUTE_UNUSED = ZEND_JIT_TRACE_STOP_ERROR;

	do {
		if (call->prev_execute_data) {
			idx = zend_jit_trace_record_fake_init_call(call->prev_execute_data, trace_buffer, idx);
		}
		TRACE_RECORD_INIT(ZEND_JIT_TRACE_INIT_CALL, 1, call->func);
	} while (0);
	return idx;
}

/*
 *  Trace Linking Rules
 *  ===================
 *
 *                                          flags
 *          +----------+----------+----------++----------+----------+----------+
 *          |                                ||              JIT               |
 *          +----------+----------+----------++----------+----------+----------+
 *   start  |   LOOP   |  ENTER   |  RETURN  ||   LOOP   |  ENTER   |  RETURN  |
 * +========+==========+==========+==========++==========+==========+==========+
 * | LOOP   |   loop   |          | loop-ret || COMPILED |   LINK   |   LINK   |
 * +--------+----------+----------+----------++----------+----------+----------+
 * | ENTER  |INNER_LOOP| rec-call |  return  ||   LINK   |   LINK   |   LINK   |
 * +--------+----------+----------+----------++----------+----------+----------+
 * | RETURN |INNER_LOOP|          |  rec-ret ||   LINK   |          |   LINK   |
 * +--------+----------+----------+----------++----------+----------+----------+
 * | SIDE   |  unroll  |          |  return  ||   LINK   |   LINK   |   LINK   |
 * +--------+----------+----------+----------++----------+----------+----------+
 *
 * loop:       LOOP if "cycle" and level == 0, otherwise INNER_LOOP
 * INNER_LOOP: abort recording and start new one (wit for loop)
 * COMPILED:   abort recording (wait while side exit creates outer loop)
 * unroll:     continue recording while unroll limit reached
 * rec-call:   RECURSIVE_CALL if "cycle" and level > N, otherwise continue
 * loop-ret:   LOOP_EXIT if level == 0, otherwise continue (wait for loop)
 * return:     RETURN if level == 0
 * rec_ret:    RECURSIVE_RET if "cycle" and ret_level > N, otherwise continue
 *
 */

zend_jit_trace_stop ZEND_FASTCALL zend_jit_trace_execute(zend_execute_data *ex, const zend_op *op, zend_jit_trace_rec *trace_buffer, uint8_t start)
{
#ifdef HAVE_GCC_GLOBAL_REGS
	zend_execute_data *save_execute_data = execute_data;
	const zend_op *save_opline = opline;
#endif
	const zend_op *orig_opline;
	zend_jit_trace_stop stop = ZEND_JIT_TRACE_STOP_ERROR;
	int level = 0;
	int ret_level = 0;
	zend_vm_opcode_handler_t handler;
	zend_jit_op_array_trace_extension *jit_extension;
	size_t offset;
	int idx, count;
	uint8_t  trace_flags, op1_type, op2_type, op3_type;
	int backtrack_recursion = -1;
	int backtrack_ret_recursion = -1;
	int backtrack_ret_recursion_level = 0;
	int loop_unroll_limit = 0;
	const zend_op_array *unrolled_calls[ZEND_JIT_TRACE_MAX_CALL_DEPTH + ZEND_JIT_TRACE_MAX_RET_DEPTH];
#if ZEND_JIT_DETECT_UNROLLED_LOOPS
	uint32_t unrolled_loops[ZEND_JIT_TRACE_MAX_UNROLL_LOOPS];
#endif
	zend_bool is_toplevel;
#ifdef HAVE_GCC_GLOBAL_REGS
	zend_execute_data *prev_execute_data = ex;

	execute_data = ex;
	opline = EX(opline) = op;
#else
	int rc;
	zend_execute_data *execute_data = ex;
	const zend_op *opline = EX(opline);
#endif
	zend_execute_data *prev_call = EX(call);

	orig_opline = opline;

	jit_extension =
		(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(&EX(func)->op_array);
	offset = jit_extension->offset;

	TRACE_START(ZEND_JIT_TRACE_START, start, &EX(func)->op_array);
	((zend_jit_trace_start_rec*)trace_buffer)->opline = opline;
	is_toplevel = EX(func)->op_array.function_name == NULL;


	if (prev_call) {
		idx = zend_jit_trace_record_fake_init_call(prev_call, trace_buffer, idx);
	}

	while (1) {
		if (UNEXPECTED(opline->opcode == ZEND_HANDLE_EXCEPTION)) {
			/* Abort trace because of exception */
			stop = ZEND_JIT_TRACE_STOP_EXCEPTION;
			break;
		}

		op1_type = op2_type = op3_type = IS_UNKNOWN;
		if ((opline->op1_type & (IS_TMP_VAR|IS_VAR|IS_CV))
		 &&	(opline->opcode != ZEND_ROPE_ADD && opline->opcode != ZEND_ROPE_END)) {
			zval *zv = EX_VAR(opline->op1.var);
			op1_type = Z_TYPE_P(zv);
			if (op1_type == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
				op1_type = Z_TYPE_P(zv);
			}
			if (op1_type == IS_REFERENCE) {
				op1_type = Z_TYPE_P(Z_REFVAL_P(zv)) | IS_TRACE_REFERENCE;
			}
		}
		if (opline->op2_type & (IS_TMP_VAR|IS_VAR|IS_CV)) {
			zval *zv = EX_VAR(opline->op2.var);
			op2_type = Z_TYPE_P(zv);
			if (op2_type == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
				op2_type = Z_TYPE_P(zv);
			}
			if (op2_type == IS_REFERENCE) {
				op2_type = Z_TYPE_P(Z_REFVAL_P(zv)) | IS_TRACE_REFERENCE;
			}
		}
		if ((
			opline->opcode == ZEND_ASSIGN_DIM ||
			opline->opcode == ZEND_ASSIGN_OBJ ||
			opline->opcode == ZEND_ASSIGN_STATIC_PROP ||
			opline->opcode == ZEND_ASSIGN_DIM_OP ||
			opline->opcode == ZEND_ASSIGN_OBJ_OP ||
			opline->opcode == ZEND_ASSIGN_STATIC_PROP_OP ||
			opline->opcode == ZEND_ASSIGN_OBJ_REF ||
			opline->opcode == ZEND_ASSIGN_STATIC_PROP_REF) &&
				((opline+1)->op1_type & (IS_TMP_VAR|IS_VAR|IS_CV))) {
			zval *zv = EX_VAR((opline+1)->op1.var);
			op3_type = Z_TYPE_P(zv);
			if (op3_type == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
				op3_type = Z_TYPE_P(zv);
			}
			if (op3_type == IS_REFERENCE) {
				op3_type = Z_TYPE_P(Z_REFVAL_P(zv)) | IS_TRACE_REFERENCE;
			}
		}

		TRACE_RECORD_VM(ZEND_JIT_TRACE_VM, opline, op1_type, op2_type, op3_type);

		if (opline->op1_type & (IS_TMP_VAR|IS_VAR|IS_CV)) {
			zval *var = EX_VAR(opline->op1.var);
			uint8_t type = Z_TYPE_P(var);

			if (type == IS_OBJECT) {
				TRACE_RECORD_TYPE(ZEND_JIT_TRACE_OP1_TYPE, Z_OBJCE_P(var));
			}
		}

		if (opline->op2_type & (IS_TMP_VAR|IS_VAR|IS_CV)) {
			zval *var = EX_VAR(opline->op2.var);
			uint8_t type = Z_TYPE_P(var);

			if (type == IS_OBJECT) {
				TRACE_RECORD_TYPE(ZEND_JIT_TRACE_OP2_TYPE, Z_OBJCE_P(var));
			}
		}

		switch (opline->opcode) {
			case ZEND_DO_FCALL:
			case ZEND_DO_ICALL:
			case ZEND_DO_UCALL:
			case ZEND_DO_FCALL_BY_NAME:
				if (EX(call)->func->type == ZEND_INTERNAL_FUNCTION) {
					TRACE_RECORD(ZEND_JIT_TRACE_DO_ICALL, EX(call)->func);
				}
				break;
			default:
				break;
		}

		handler = (zend_vm_opcode_handler_t)ZEND_OP_TRACE_INFO(opline, offset)->call_handler;
#ifdef HAVE_GCC_GLOBAL_REGS
		handler();
		if (UNEXPECTED(opline == zend_jit_halt_op)) {
			stop = ZEND_JIT_TRACE_STOP_HALT;
			break;
		}
		if (UNEXPECTED(execute_data != prev_execute_data)) {
			if (execute_data->prev_execute_data == prev_execute_data) {
#else
		rc = handler(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);
		if (rc != 0) {
			if (rc < 0) {
				stop = ZEND_JIT_TRACE_STOP_HALT;
				break;
			}
			execute_data = EG(current_execute_data);
			opline = EX(opline);
			if (rc == 1) {
#endif
				/* Enter into function */
				if (level > ZEND_JIT_TRACE_MAX_CALL_DEPTH) {
					stop = ZEND_JIT_TRACE_STOP_TOO_DEEP;
					break;
				}

				if (EX(func)->op_array.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
					/* TODO: Can we continue recording ??? */
					stop = ZEND_JIT_TRACE_STOP_TRAMPOLINE;
					break;
				}

				TRACE_RECORD_ENTER(ZEND_JIT_TRACE_ENTER, EX(return_value) != NULL, &EX(func)->op_array);

				count = zend_jit_trace_recursive_call_count(&EX(func)->op_array, unrolled_calls, ret_level, level);

				if (opline == orig_opline) {
					if (count + 1 >= ZEND_JIT_TRACE_MAX_RECURSION) {
						stop = ZEND_JIT_TRACE_STOP_RECURSIVE_CALL;
						break;
					}
					backtrack_recursion = idx;
				} else if (count >= ZEND_JIT_TRACE_MAX_RECURSION) {
					stop = ZEND_JIT_TRACE_STOP_DEEP_RECURSION;
					break;
				}

				unrolled_calls[ret_level + level] = &EX(func)->op_array;
				level++;
			} else {
				/* Return from function */
				if (level == 0) {
					if (is_toplevel) {
						stop = ZEND_JIT_TRACE_STOP_TOPLEVEL;
						break;
#if ZEND_JIT_RECORD_RECURSIVE_RETURN
					} else if (start == ZEND_JIT_TRACE_START_RETURN
					        && execute_data->prev_execute_data
					        && execute_data->prev_execute_data->func
					        && execute_data->prev_execute_data->func->type == ZEND_USER_FUNCTION
					        && zend_jit_trace_has_recursive_ret(execute_data, trace_buffer[0].op_array, orig_opline, ret_level)) {
						if (ret_level > ZEND_JIT_TRACE_MAX_RET_DEPTH) {
							stop = ZEND_JIT_TRACE_STOP_TOO_DEEP_RET;
							break;
						}
						TRACE_RECORD_BACK(ZEND_JIT_TRACE_BACK, 1, &EX(func)->op_array);
						count = zend_jit_trace_recursive_ret_count(&EX(func)->op_array, unrolled_calls, ret_level);
						if (opline == orig_opline) {
							if (count + 1 >= ZEND_JIT_TRACE_MAX_RECURSION) {
								stop = ZEND_JIT_TRACE_STOP_RECURSIVE_RET;
								break;
							}
							backtrack_ret_recursion = idx;
							backtrack_ret_recursion_level = ret_level;
						} else if (count >= ZEND_JIT_TRACE_MAX_RECURSION) {
							stop = ZEND_JIT_TRACE_STOP_DEEP_RECURSION;
							break;
						}

						unrolled_calls[ret_level] = &EX(func)->op_array;
						ret_level++;
						is_toplevel = EX(func)->op_array.function_name == NULL;
#endif
					} else if (start & ZEND_JIT_TRACE_START_LOOP
					 && !zend_jit_trace_bad_loop_exit(orig_opline)) {
						/* Fail to try close the loop.
						   If this doesn't work terminate it. */
						stop = ZEND_JIT_TRACE_STOP_LOOP_EXIT;
						break;
					} else {
						stop = ZEND_JIT_TRACE_STOP_RETURN;
						break;
					}
				} else {
					level--;
					TRACE_RECORD_BACK(ZEND_JIT_TRACE_BACK, 0, &EX(func)->op_array);
				}
			}
#ifdef HAVE_GCC_GLOBAL_REGS
			prev_execute_data = execute_data;
#endif
			jit_extension =
				(zend_jit_op_array_trace_extension*)ZEND_FUNC_INFO(&EX(func)->op_array);
			if (UNEXPECTED(!jit_extension)) {
				stop = ZEND_JIT_TRACE_STOP_BAD_FUNC;
				break;
			}
			offset = jit_extension->offset;
		}
		if (EX(call) != prev_call) {
			if (trace_buffer[idx-1].op != ZEND_JIT_TRACE_BACK
			 && EX(call)
			 && EX(call)->prev_execute_data == prev_call) {
				if (EX(call)->func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
					/* TODO: Can we continue recording ??? */
					stop = ZEND_JIT_TRACE_STOP_TRAMPOLINE;
					break;
				}
				TRACE_RECORD_INIT(ZEND_JIT_TRACE_INIT_CALL, 0, EX(call)->func);
			}
			prev_call = EX(call);
		}

#ifndef HAVE_GCC_GLOBAL_REGS
		opline = EX(opline);
#endif

		trace_flags = ZEND_OP_TRACE_INFO(opline, offset)->trace_flags;
		if (trace_flags) {
			if (trace_flags & ZEND_JIT_TRACE_JITED) {
				if (trace_flags & ZEND_JIT_TRACE_START_LOOP) {
					if ((start & ZEND_JIT_TRACE_START_LOOP) != 0
					 && level + ret_level == 0
					 && !zend_jit_trace_bad_compiled_loop(orig_opline)) {
						/* Fail to try close outer loop throgh side exit.
						   If this doesn't work just link. */
						stop = ZEND_JIT_TRACE_STOP_COMPILED_LOOP;
						break;
					} else {
						stop = ZEND_JIT_TRACE_STOP_LINK;
						break;
					}
				} else if (trace_flags & ZEND_JIT_TRACE_START_ENTER) {
					if (start != ZEND_JIT_TRACE_START_RETURN) {
						// TODO: We may try to inline function ???
						stop = ZEND_JIT_TRACE_STOP_LINK;
						break;
					}
				} else {
					stop = ZEND_JIT_TRACE_STOP_LINK;
					break;
				}
			} else if (trace_flags & ZEND_JIT_TRACE_BLACKLISTED) {
				stop = ZEND_JIT_TRACE_STOP_BLACK_LIST;
				break;
			} else if (trace_flags & ZEND_JIT_TRACE_START_LOOP) {
				if (start != ZEND_JIT_TRACE_START_SIDE) {
					if (opline == orig_opline && level + ret_level == 0) {
						stop = ZEND_JIT_TRACE_STOP_LOOP;
						break;
					}
					/* Fail to try creating a trace for inner loop first.
					   If this doesn't work try unroling loop. */
					if (!zend_jit_trace_bad_inner_loop(opline)) {
						stop = ZEND_JIT_TRACE_STOP_INNER_LOOP;
						break;
					}
				}
				if (loop_unroll_limit < ZEND_JIT_TRACE_MAX_UNROLL_LOOPS) {
					loop_unroll_limit++;
				} else {
					stop = ZEND_JIT_TRACE_STOP_LOOP_UNROLL;
					break;
				}
			} else if (trace_flags & ZEND_JIT_TRACE_UNSUPPORTED) {
				TRACE_RECORD(ZEND_JIT_TRACE_VM, opline);
				stop = ZEND_JIT_TRACE_STOP_NOT_SUPPORTED;
				break;
			}
		}
	}

	if (!ZEND_JIT_TRACE_STOP_OK(stop)) {
		if (backtrack_recursion > 0) {
			idx = backtrack_recursion;
			stop = ZEND_JIT_TRACE_STOP_RECURSIVE_CALL;
		} else if (backtrack_ret_recursion > 0) {
			idx = backtrack_ret_recursion;
			ret_level = backtrack_ret_recursion_level;
			stop = ZEND_JIT_TRACE_STOP_RECURSIVE_RET;
		}
	}

	TRACE_END(ZEND_JIT_TRACE_END, stop, opline);
	((zend_jit_trace_start_rec*)trace_buffer)->len = idx;

#ifdef HAVE_GCC_GLOBAL_REGS
	if (stop != ZEND_JIT_TRACE_STOP_HALT) {
		EX(opline) = opline;
	}
#endif

#ifdef HAVE_GCC_GLOBAL_REGS
	execute_data = save_execute_data;
	opline = save_opline;
#endif

	return stop;
}
