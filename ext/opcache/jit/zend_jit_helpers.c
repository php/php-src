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

#include "Zend/zend_API.h"

static zend_function* ZEND_FASTCALL zend_jit_find_func_helper(zend_string *name)
{
	zval *func = zend_hash_find(EG(function_table), name);
	zend_function *fbc;

	if (UNEXPECTED(func == NULL)) {
		zend_throw_error(NULL, "Call to undefined function %s()", ZSTR_VAL(name));
		return NULL;
	}
	fbc = Z_FUNC_P(func);
	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
		fbc->op_array.run_time_cache = zend_arena_alloc(&CG(arena), fbc->op_array.cache_size);
		memset(fbc->op_array.run_time_cache, 0, fbc->op_array.cache_size);
	}
	return fbc;
}

static zend_execute_data* ZEND_FASTCALL zend_jit_extend_stack_helper(uint32_t used_stack, zend_function *fbc)
{
	zend_execute_data *call = (zend_execute_data*)zend_vm_stack_extend(used_stack);
	call->func = fbc;
	ZEND_SET_CALL_INFO(call, 0, ZEND_CALL_NESTED_FUNCTION|ZEND_CALL_ALLOCATED);
	return call;
}

static zend_always_inline void i_free_compiled_variables(zend_execute_data *execute_data)
{
	zval *cv = EX_VAR_NUM(0);
	zval *end = cv + EX(func)->op_array.last_var;
	while (EXPECTED(cv != end)) {
		if (Z_REFCOUNTED_P(cv)) {
			if (!Z_DELREF_P(cv)) {
				zend_refcounted *r = Z_COUNTED_P(cv);
				ZVAL_NULL(cv);
				zval_dtor_func(r);
			} else {
				GC_ZVAL_CHECK_POSSIBLE_ROOT(cv);
			}
		}
		cv++;
	}
}

static int ZEND_FASTCALL zend_jit_leave_helper(zend_execute_data *execute_data) {
	zend_execute_data *old_execute_data;
	uint32_t call_info = EX_CALL_INFO();

	if (EXPECTED((call_info & (ZEND_CALL_CODE|ZEND_CALL_TOP|ZEND_CALL_HAS_SYMBOL_TABLE|ZEND_CALL_FREE_EXTRA_ARGS|ZEND_CALL_ALLOCATED)) == 0)) {
		i_free_compiled_variables(execute_data);

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
		EG(vm_stack_top) = (zval*)execute_data;
		execute_data = EX(prev_execute_data);

		if (UNEXPECTED(EG(exception) != NULL)) {
			const zend_op *old_opline = EX(opline);
			zend_throw_exception_internal(NULL);
			if (RETURN_VALUE_USED(old_opline)) {
				zval_ptr_dtor(EX_VAR(old_opline->result.var));
			}
			return 2;
		}
		EX(opline)++;
		return 0;
	} else if (EXPECTED((call_info & (ZEND_CALL_CODE|ZEND_CALL_TOP)) == 0)) {
		i_free_compiled_variables(execute_data);

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
			if (RETURN_VALUE_USED(old_opline)) {
				zval_ptr_dtor(EX_VAR(old_opline->result.var));
			}
			return 2;
		}
		EX(opline)++;
		return 0;
	} else if (EXPECTED((call_info & ZEND_CALL_TOP) == 0)) {
		zend_detach_symbol_table(execute_data);
		destroy_op_array(&EX(func)->op_array);
		efree_size(EX(func), sizeof(zend_op_array));
		old_execute_data = execute_data;
		execute_data = EG(current_execute_data) = EX(prev_execute_data);
		zend_vm_stack_free_call_frame_ex(call_info, old_execute_data);

		zend_attach_symbol_table(execute_data);
		if (UNEXPECTED(EG(exception) != NULL)) {
			zend_throw_exception_internal(NULL);
			return 2;
		}
		EX(opline)++;
		return 0;
	} else {
		if (EXPECTED((call_info & ZEND_CALL_CODE) == 0)) {
			i_free_compiled_variables(execute_data);
			i_free_compiled_variables(execute_data);
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
			return -1;
		} else /* if (call_kind == ZEND_CALL_TOP_CODE) */ {
			zend_array *symbol_table = EX(symbol_table);

			zend_detach_symbol_table(execute_data);
			old_execute_data = EX(prev_execute_data);
			while (old_execute_data) {
				if (old_execute_data->func && (ZEND_CALL_INFO(old_execute_data) & ZEND_CALL_HAS_SYMBOL_TABLE)) {
					if (old_execute_data->symbol_table == symbol_table) {
						zend_attach_symbol_table(old_execute_data);
					}
					break;
				}
				old_execute_data = old_execute_data->prev_execute_data;
			}
			EG(current_execute_data) = EX(prev_execute_data);
			return -1;
		}
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
