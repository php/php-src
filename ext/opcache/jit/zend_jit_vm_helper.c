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
   |          Xinchen Hui <xinchen.h@zend.com>                            |
   +----------------------------------------------------------------------+
*/

#include "Zend/zend_execute.h"
#include "Zend/zend_exceptions.h"

#include <ZendAccelerator.h>
#include "Optimizer/zend_func_info.h"
#include "zend_jit.h"

#include "zend_jit_vm_helper.h"

#pragma GCC diagnostic ignored "-Wvolatile-register-var"
#if defined(__x86_64__)
register zend_execute_data* volatile execute_data __asm__("%r14");
register const zend_op* volatile opline __asm__("%r15");
#else
register zend_execute_data* volatile execute_data __asm__("%esi");
register const zend_op* volatile opline __asm__("%edi");
#endif
#pragma GCC diagnostic warning "-Wvolatile-register-var"

void ZEND_FASTCALL zend_jit_leave_nested_func_helper(uint32_t call_info)
{
	zend_execute_data *old_execute_data;

	if (UNEXPECTED(call_info & ZEND_CALL_HAS_SYMBOL_TABLE)) {
		zend_clean_and_cache_symbol_table(EX(symbol_table));
	}
	EG(current_execute_data) = EX(prev_execute_data);
	if (UNEXPECTED(call_info & ZEND_CALL_RELEASE_THIS)) {
		zend_object *object = Z_OBJ(execute_data->This);
		if (UNEXPECTED(EG(exception) != NULL) && (call_info & ZEND_CALL_CTOR)) {
			GC_REFCOUNT(object)--;
			zend_object_store_ctor_failed(object);
		}
		OBJ_RELEASE(object);
	} else if (UNEXPECTED(call_info & ZEND_CALL_CLOSURE)) {
		OBJ_RELEASE((zend_object*)execute_data->func->op_array.prototype);
	}

	zend_vm_stack_free_extra_args_ex(call_info, execute_data);
	old_execute_data = execute_data;
	execute_data = EX(prev_execute_data);
	zend_vm_stack_free_call_frame_ex(call_info, old_execute_data);

	if (UNEXPECTED(EG(exception) != NULL)) {
		const zend_op *old_opline = EX(opline);
		zend_throw_exception_internal(NULL);
		if (old_opline->result_type != IS_UNDEF) {
			zval_ptr_dtor(EX_VAR(old_opline->result.var));
		}
	} else {
		EX(opline)++;
		opline = EX(opline);
	}
}

void ZEND_FASTCALL zend_jit_leave_top_func_helper(uint32_t call_info)
{
	if (UNEXPECTED(call_info & (ZEND_CALL_HAS_SYMBOL_TABLE|ZEND_CALL_FREE_EXTRA_ARGS))) {
		if (UNEXPECTED(call_info & ZEND_CALL_HAS_SYMBOL_TABLE)) {
			zend_clean_and_cache_symbol_table(EX(symbol_table));
		}
		zend_vm_stack_free_extra_args_ex(call_info, execute_data);
	}
	EG(current_execute_data) = EX(prev_execute_data);
	if (UNEXPECTED(call_info & ZEND_CALL_CLOSURE)) {
		OBJ_RELEASE((zend_object*)EX(func)->op_array.prototype);
	}
	execute_data = EG(current_execute_data);
	opline = NULL;
}

/* The recorded log may be postprocessed to identify the hot functions and
 * loops.
 *
 * To get the top functions:
 *     sed 's/^\(.*\), (.:\(.*\):.*$/\1,\2/' | sort | uniq -c | sort -nr | \
 *     head -n25 | sed 's/^\s*[0-9]*\s*\(.*\),.*$/"\1",/' > zend_jit_filter.c
 *
 */

void ZEND_FASTCALL zend_jit_func_header_helper(void)
{
	fprintf(stderr, "%s%s%s, (F:%s:%d)\n",
		EX(func)->op_array.scope ? ZSTR_VAL(EX(func)->op_array.scope->name) : "",
		EX(func)->op_array.scope ? "::" : "",
		EX(func)->op_array.function_name ? ZSTR_VAL(EX(func)->op_array.function_name) : "",
		ZSTR_VAL(EX(func)->op_array.filename),
		EX(func)->op_array.line_start);
}

void ZEND_FASTCALL zend_jit_loop_header_helper(void)
{
	fprintf(stderr, "%s%s%s, (L:%s:%d)\n",
		EX(func)->op_array.scope ? ZSTR_VAL(EX(func)->op_array.scope->name) : "",
		EX(func)->op_array.scope ? "::" : "",
		EX(func)->op_array.function_name ? ZSTR_VAL(EX(func)->op_array.function_name) : "",
		ZSTR_VAL(EX(func)->op_array.filename),
		opline->lineno);
}

void ZEND_FASTCALL zend_jit_profile_helper(void)
{
	zend_op_array *op_array = (zend_op_array*)EX(func);
	const void *handler = (const void*)ZEND_FUNC_INFO(op_array);
	++(ZEND_COUNTER_INFO(op_array));
	++zend_jit_count;
	return ((zend_vm_opcode_handler_t)handler)();
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
