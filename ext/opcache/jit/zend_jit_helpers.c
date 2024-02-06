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

#include "Zend/zend_API.h"

static ZEND_COLD void undef_result_after_exception(void) {
	const zend_op *opline = EG(opline_before_exception);
	ZEND_ASSERT(EG(exception));
	if (opline && opline->result_type & (IS_VAR | IS_TMP_VAR)) {
		zend_execute_data *execute_data = EG(current_execute_data);
		ZVAL_UNDEF(EX_VAR(opline->result.var));
	}
}

static zend_never_inline zend_function* ZEND_FASTCALL _zend_jit_init_func_run_time_cache(zend_op_array *op_array) /* {{{ */
{
	void **run_time_cache;

	run_time_cache = zend_arena_alloc(&CG(arena), op_array->cache_size);
	memset(run_time_cache, 0, op_array->cache_size);
	ZEND_MAP_PTR_SET(op_array->run_time_cache, run_time_cache);
	return (zend_function*)op_array;
}
/* }}} */

static zend_never_inline zend_op_array* ZEND_FASTCALL zend_jit_init_func_run_time_cache_helper(zend_op_array *op_array) /* {{{ */
{
	void **run_time_cache;

	if (!RUN_TIME_CACHE(op_array)) {
		run_time_cache = zend_arena_alloc(&CG(arena), op_array->cache_size);
		memset(run_time_cache, 0, op_array->cache_size);
		ZEND_MAP_PTR_SET(op_array->run_time_cache, run_time_cache);
	}
	return op_array;
}
/* }}} */

static zend_function* ZEND_FASTCALL zend_jit_find_func_helper(zend_string *name, void **cache_slot)
{
	zval *func = zend_hash_find_known_hash(EG(function_table), name);
	zend_function *fbc;

	if (UNEXPECTED(func == NULL)) {
		return NULL;
	}
	fbc = Z_FUNC_P(func);
	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
		fbc = _zend_jit_init_func_run_time_cache(&fbc->op_array);
	}
	*cache_slot = fbc;
	return fbc;
}

static uint32_t ZEND_FASTCALL zend_jit_jmp_frameless_helper(zval *func_name, void **cache_slot)
{
	zval *func = zend_hash_find_known_hash(EG(function_table), Z_STR_P(func_name));
	zend_jmp_fl_result result = (func == NULL) + 1;
	*cache_slot = (void *)(uintptr_t)result;
	return result;
}

static zend_function* ZEND_FASTCALL zend_jit_find_ns_func_helper(zval *func_name, void **cache_slot)
{
	zval *func = zend_hash_find_known_hash(EG(function_table), Z_STR_P(func_name + 1));
	zend_function *fbc;

	if (func == NULL) {
		func = zend_hash_find_known_hash(EG(function_table), Z_STR_P(func_name + 2));
		if (UNEXPECTED(func == NULL)) {
			return NULL;
		}
	}
	fbc = Z_FUNC_P(func);
	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
		fbc = _zend_jit_init_func_run_time_cache(&fbc->op_array);
	}
	*cache_slot = fbc;
	return fbc;
}

static ZEND_COLD void ZEND_FASTCALL zend_jit_invalid_method_call(zval *object)
{
	zend_execute_data *execute_data = EG(current_execute_data);
	const zend_op *opline = EX(opline);
	zval *function_name = function_name = RT_CONSTANT(opline, opline->op2);;

	if (Z_TYPE_P(object) == IS_UNDEF && opline->op1_type == IS_CV) {
		zend_string *cv = EX(func)->op_array.vars[EX_VAR_TO_NUM(opline->op1.var)];

		zend_error_unchecked(E_WARNING, "Undefined variable $%S", cv);
		if (UNEXPECTED(EG(exception) != NULL)) {
			return;
		}
		object = &EG(uninitialized_zval);
	}
	zend_throw_error(NULL, "Call to a member function %s() on %s",
		Z_STRVAL_P(function_name), zend_zval_value_name(object));
}

static ZEND_COLD void ZEND_FASTCALL zend_jit_invalid_method_call_tmp(zval *object)
{
	zend_execute_data *execute_data = EG(current_execute_data);
	const zend_op *opline = EX(opline);

	zend_jit_invalid_method_call(object);
	zval_ptr_dtor_nogc(EX_VAR(opline->op1.var));
}

static zend_never_inline ZEND_COLD void ZEND_FASTCALL zend_undefined_method(const zend_class_entry *ce, const zend_string *method)
{
	zend_throw_error(NULL, "Call to undefined method %s::%s()", ZSTR_VAL(ce->name), ZSTR_VAL(method));
}

static void ZEND_FASTCALL zend_jit_unref_helper(zval *zv)
{
	zend_reference *ref;

	ZEND_ASSERT(Z_ISREF_P(zv));
	ref = Z_REF_P(zv);
	ZVAL_COPY_VALUE(zv, &ref->val);
	if (GC_DELREF(ref) == 0) {
		efree_size(ref, sizeof(zend_reference));
	} else {
		Z_TRY_ADDREF_P(zv);
	}
}

static zend_function* ZEND_FASTCALL zend_jit_find_method_helper(zend_object *obj, zval *function_name, zend_object **obj_ptr)
{
	zend_execute_data *execute_data = EG(current_execute_data);
	const zend_op *opline = EX(opline);
	zend_class_entry *called_scope = obj->ce;
	zend_function *fbc;

	fbc = obj->handlers->get_method(obj_ptr, Z_STR_P(function_name), function_name + 1);
	if (UNEXPECTED(fbc == NULL)) {
		if (EXPECTED(!EG(exception))) {
			zend_undefined_method(called_scope, Z_STR_P(function_name));
		}
		return NULL;
	}

	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!RUN_TIME_CACHE(&fbc->op_array))) {
		zend_init_func_run_time_cache(&fbc->op_array);
	}

	if (UNEXPECTED(obj != *obj_ptr)) {
		return fbc;
	}

	if (EXPECTED(!(fbc->common.fn_flags & (ZEND_ACC_CALL_VIA_TRAMPOLINE|ZEND_ACC_NEVER_CACHE)))) {
		CACHE_POLYMORPHIC_PTR(opline->result.num, called_scope, fbc);
	}

	return fbc;
}

static zend_function* ZEND_FASTCALL zend_jit_find_method_tmp_helper(zend_object *obj, zval *function_name, zend_object **obj_ptr)
{
	zend_function *fbc;

	fbc = zend_jit_find_method_helper(obj, function_name, obj_ptr);
	if (!fbc) {
		if (GC_DELREF(obj) == 0) {
			zend_objects_store_del(obj);
		}
	} else if (obj != *obj_ptr) {
		GC_ADDREF(*obj_ptr);
		if (GC_DELREF(obj) == 0) {
			zend_objects_store_del(obj);
		}
	}
	return fbc;
}

static zend_execute_data* ZEND_FASTCALL zend_jit_push_static_metod_call_frame(zend_object *obj, zend_function *fbc, uint32_t num_args)
{
	zend_class_entry *scope = obj->ce;

	return zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_FUNCTION, fbc, num_args, scope);
}

static zend_execute_data* ZEND_FASTCALL zend_jit_push_static_metod_call_frame_tmp(zend_object *obj, zend_function *fbc, uint32_t num_args)
{
	zend_class_entry *scope = obj->ce;

	if (GC_DELREF(obj) == 0) {
		zend_objects_store_del(obj);
		if (UNEXPECTED(EG(exception))) {
			return NULL;
		}
	}

	return zend_vm_stack_push_call_frame(ZEND_CALL_NESTED_FUNCTION, fbc, num_args, scope);
}

static zend_execute_data* ZEND_FASTCALL zend_jit_extend_stack_helper(uint32_t used_stack, zend_function *fbc)
{
	zend_execute_data *call = (zend_execute_data*)zend_vm_stack_extend(used_stack);
	call->func = fbc;
	ZEND_CALL_INFO(call) = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_ALLOCATED;
	return call;
}

static zend_execute_data* ZEND_FASTCALL zend_jit_int_extend_stack_helper(uint32_t used_stack)
{
	zend_execute_data *call = (zend_execute_data*)zend_vm_stack_extend(used_stack);
	ZEND_CALL_INFO(call) = ZEND_CALL_NESTED_FUNCTION | ZEND_CALL_ALLOCATED;
	return call;
}

static zval* ZEND_FASTCALL zend_jit_symtable_find(HashTable *ht, zend_string *str)
{
	zend_ulong idx;
	register const char *tmp = str->val;

	do {
		if (*tmp > '9') {
			break;
		} else if (*tmp < '0') {
			if (*tmp != '-') {
				break;
			}
			tmp++;
			if (*tmp > '9' || *tmp < '0') {
				break;
			}
		}
		if (_zend_handle_numeric_str_ex(str->val, str->len, &idx)) {
			return zend_hash_index_find(ht, idx);
		}
	} while (0);

	return zend_hash_find(ht, str);
}

static zval* ZEND_FASTCALL zend_jit_hash_index_lookup_rw_no_packed(HashTable *ht, zend_long idx)
{
	zval *retval = NULL;

	if (!HT_IS_PACKED(ht)) {
		retval = _zend_hash_index_find(ht, idx);
	}
	if (!retval) {
		retval = zend_undefined_offset_write(ht, idx);
	}
	return retval;
}

static zval* ZEND_FASTCALL zend_jit_hash_index_lookup_rw(HashTable *ht, zend_long idx)
{
	zval *retval = zend_hash_index_find(ht, idx);

	if (!retval) {
		retval = zend_undefined_offset_write(ht, idx);
	}
	return retval;
}

static zval* ZEND_FASTCALL zend_jit_hash_lookup_rw(HashTable *ht, zend_string *str)
{
	zval *retval = zend_hash_find_known_hash(ht, str);
	if (!retval) {
		/* Key may be released while throwing the undefined index warning. */
		retval = zend_undefined_index_write(ht, str);
	}
	return retval;
}

static zval* ZEND_FASTCALL zend_jit_symtable_lookup_rw(HashTable *ht, zend_string *str)
{
	zend_ulong idx;
	register const char *tmp = str->val;
	zval *retval;

	do {
		if (*tmp > '9') {
			break;
		} else if (*tmp < '0') {
			if (*tmp != '-') {
				break;
			}
			tmp++;
			if (*tmp > '9' || *tmp < '0') {
				break;
			}
		}
		if (_zend_handle_numeric_str_ex(str->val, str->len, &idx)) {
			retval = zend_hash_index_find(ht, idx);
			if (!retval) {
				retval = zend_undefined_offset_write(ht, idx);
			}
			return retval;
		}
	} while (0);

	retval = zend_hash_find(ht, str);
	if (!retval) {
		/* Key may be released while throwing the undefined index warning. */
		retval = zend_undefined_index_write(ht, str);
	}
	return retval;
}

static zval* ZEND_FASTCALL zend_jit_symtable_lookup_w(HashTable *ht, zend_string *str)
{
	zend_ulong idx;
	register const char *tmp = str->val;

	do {
		if (*tmp > '9') {
			break;
		} else if (*tmp < '0') {
			if (*tmp != '-') {
				break;
			}
			tmp++;
			if (*tmp > '9' || *tmp < '0') {
				break;
			}
		}
		if (_zend_handle_numeric_str_ex(str->val, str->len, &idx)) {
			return zend_hash_index_lookup(ht, idx);
		}
	} while (0);

	return zend_hash_lookup(ht, str);
}

static int ZEND_FASTCALL zend_jit_undefined_op_helper(uint32_t var)
{
	const zend_execute_data *execute_data = EG(current_execute_data);
	zend_string *cv = EX(func)->op_array.vars[EX_VAR_TO_NUM(var)];

	zend_error_unchecked(E_WARNING, "Undefined variable $%S", cv);
	return EG(exception) == NULL;
}

static int ZEND_FASTCALL zend_jit_undefined_op_helper_write(HashTable *ht, uint32_t var)
{
	const zend_execute_data *execute_data = EG(current_execute_data);
	zend_string *cv = EX(func)->op_array.vars[EX_VAR_TO_NUM(var)];

	/* The array may be destroyed while throwing the notice.
	 * Temporarily increase the refcount to detect this situation. */
	if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
		GC_ADDREF(ht);
	}
	zend_error_unchecked(E_WARNING, "Undefined variable $%S", cv);
	if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && GC_DELREF(ht) != 1) {
		if (!GC_REFCOUNT(ht)) {
			zend_array_destroy(ht);
		}
		return 0;
	}
	return EG(exception) == NULL;
}

static void ZEND_FASTCALL zend_jit_fetch_dim_r_helper(zend_array *ht, zval *dim, zval *result)
{
	zend_ulong hval;
	zend_string *offset_key;
	zval *retval;
	zend_execute_data *execute_data;
	const zend_op *opline;

	if (Z_TYPE_P(dim) == IS_REFERENCE) {
		dim = Z_REFVAL_P(dim);
	}

	switch (Z_TYPE_P(dim)) {
		case IS_LONG:
			hval = Z_LVAL_P(dim);
			goto num_index;
		case IS_STRING:
			offset_key = Z_STR_P(dim);
			goto str_index;
		case IS_UNDEF:
			/* The array may be destroyed while throwing the notice.
			 * Temporarily increase the refcount to detect this situation. */
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
				GC_ADDREF(ht);
			}
			execute_data = EG(current_execute_data);
			opline = EX(opline);
			zend_jit_undefined_op_helper(EG(current_execute_data)->opline->op2.var);
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && !GC_DELREF(ht)) {
				zend_array_destroy(ht);
				if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
					if (EG(exception)) {
						ZVAL_UNDEF(EX_VAR(opline->result.var));
					} else {
						ZVAL_NULL(EX_VAR(opline->result.var));
					}
				}
				return;
			}
			if (EG(exception)) {
				if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
					ZVAL_UNDEF(EX_VAR(opline->result.var));
				}
				return;
			}
			ZEND_FALLTHROUGH;
		case IS_NULL:
			offset_key = ZSTR_EMPTY_ALLOC();
			goto str_index;
		case IS_DOUBLE:
			hval = zend_dval_to_lval(Z_DVAL_P(dim));
			if (!zend_is_long_compatible(Z_DVAL_P(dim), hval)) {
				/* The array may be destroyed while throwing the notice.
				 * Temporarily increase the refcount to detect this situation. */
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
					GC_ADDREF(ht);
				}
				execute_data = EG(current_execute_data);
				opline = EX(opline);
				zend_incompatible_double_to_long_error(Z_DVAL_P(dim));
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && !GC_DELREF(ht)) {
					zend_array_destroy(ht);
					if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
						if (EG(exception)) {
							ZVAL_UNDEF(EX_VAR(opline->result.var));
						} else {
							ZVAL_NULL(EX_VAR(opline->result.var));
						}
					}
					return;
				}
				if (EG(exception)) {
					if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
						ZVAL_UNDEF(EX_VAR(opline->result.var));
					}
					return;
				}
			}
			goto num_index;
		case IS_RESOURCE:
			/* The array may be destroyed while throwing the notice.
			 * Temporarily increase the refcount to detect this situation. */
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
				GC_ADDREF(ht);
			}
			execute_data = EG(current_execute_data);
			opline = EX(opline);
			zend_use_resource_as_offset(dim);
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && !GC_DELREF(ht)) {
				zend_array_destroy(ht);
				if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
					if (EG(exception)) {
						ZVAL_UNDEF(EX_VAR(opline->result.var));
					} else {
						ZVAL_NULL(EX_VAR(opline->result.var));
					}
				}
				return;
			}
			if (EG(exception)) {
				if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
					ZVAL_UNDEF(EX_VAR(opline->result.var));
				}
				return;
			}
			hval = Z_RES_HANDLE_P(dim);
			goto num_index;
		case IS_FALSE:
			hval = 0;
			goto num_index;
		case IS_TRUE:
			hval = 1;
			goto num_index;
		default:
			zend_illegal_container_offset(ZSTR_KNOWN(ZEND_STR_ARRAY), dim, BP_VAR_R);
			undef_result_after_exception();
			return;
	}

str_index:
	if (ZEND_HANDLE_NUMERIC(offset_key, hval)) {
		goto num_index;
	}
	retval = zend_hash_find(ht, offset_key);
	if (!retval) {
		zend_error(E_WARNING, "Undefined array key \"%s\"", ZSTR_VAL(offset_key));
		ZVAL_NULL(result);
		return;
	}
	ZVAL_COPY_DEREF(result, retval);
	return;

num_index:
	ZEND_HASH_INDEX_FIND(ht, hval, retval, num_undef);
	ZVAL_COPY_DEREF(result, retval);
	return;

num_undef:
	zend_error(E_WARNING, "Undefined array key " ZEND_LONG_FMT, hval);
	ZVAL_NULL(result);
}

static void ZEND_FASTCALL zend_jit_fetch_dim_is_helper(zend_array *ht, zval *dim, zval *result)
{
	zend_ulong hval;
	zend_string *offset_key;
	zval *retval;
	zend_execute_data *execute_data;
	const zend_op *opline;

	if (Z_TYPE_P(dim) == IS_REFERENCE) {
		dim = Z_REFVAL_P(dim);
	}

	switch (Z_TYPE_P(dim)) {
		case IS_LONG:
			hval = Z_LVAL_P(dim);
			goto num_index;
		case IS_STRING:
			offset_key = Z_STR_P(dim);
			goto str_index;
		case IS_UNDEF:
			/* The array may be destroyed while throwing the notice.
			 * Temporarily increase the refcount to detect this situation. */
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
				GC_ADDREF(ht);
			}
			execute_data = EG(current_execute_data);
			opline = EX(opline);
			zend_jit_undefined_op_helper(EG(current_execute_data)->opline->op2.var);
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && !GC_DELREF(ht)) {
				zend_array_destroy(ht);
				if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
					if (EG(exception)) {
						ZVAL_UNDEF(EX_VAR(opline->result.var));
					} else {
						ZVAL_NULL(EX_VAR(opline->result.var));
					}
				}
				return;
			}
			if (EG(exception)) {
				if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
					ZVAL_UNDEF(EX_VAR(opline->result.var));
				}
				return;
			}
			ZEND_FALLTHROUGH;
		case IS_NULL:
			offset_key = ZSTR_EMPTY_ALLOC();
			goto str_index;
		case IS_DOUBLE:
			hval = zend_dval_to_lval(Z_DVAL_P(dim));
			if (!zend_is_long_compatible(Z_DVAL_P(dim), hval)) {
				/* The array may be destroyed while throwing the notice.
				 * Temporarily increase the refcount to detect this situation. */
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
					GC_ADDREF(ht);
				}
				execute_data = EG(current_execute_data);
				opline = EX(opline);
				zend_incompatible_double_to_long_error(Z_DVAL_P(dim));
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && !GC_DELREF(ht)) {
					zend_array_destroy(ht);
					if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
						if (EG(exception)) {
							ZVAL_UNDEF(EX_VAR(opline->result.var));
						} else {
							ZVAL_NULL(EX_VAR(opline->result.var));
						}
					}
					return;
				}
				if (EG(exception)) {
					if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
						ZVAL_UNDEF(EX_VAR(opline->result.var));
					}
					return;
				}
			}
			goto num_index;
		case IS_RESOURCE:
			/* The array may be destroyed while throwing the notice.
			 * Temporarily increase the refcount to detect this situation. */
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
				GC_ADDREF(ht);
			}
			execute_data = EG(current_execute_data);
			opline = EX(opline);
			zend_use_resource_as_offset(dim);
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && !GC_DELREF(ht)) {
				zend_array_destroy(ht);
				if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
					if (EG(exception)) {
						ZVAL_UNDEF(EX_VAR(opline->result.var));
					} else {
						ZVAL_NULL(EX_VAR(opline->result.var));
					}
				}
				return;
			}
			if (EG(exception)) {
				if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
					ZVAL_UNDEF(EX_VAR(opline->result.var));
				}
				return;
			}
			hval = Z_RES_HANDLE_P(dim);
			goto num_index;
		case IS_FALSE:
			hval = 0;
			goto num_index;
		case IS_TRUE:
			hval = 1;
			goto num_index;
		default:
			zend_illegal_container_offset(ZSTR_KNOWN(ZEND_STR_ARRAY), dim,
				EG(current_execute_data)->opline->opcode == ZEND_ISSET_ISEMPTY_DIM_OBJ ?
					BP_VAR_IS : BP_VAR_RW);
			undef_result_after_exception();
			return;
	}

str_index:
	if (ZEND_HANDLE_NUMERIC(offset_key, hval)) {
		goto num_index;
	}
	retval = zend_hash_find(ht, offset_key);
	if (!retval) {
		ZVAL_NULL(result);
		return;
	}
	ZVAL_COPY_DEREF(result, retval);
	return;

num_index:
	ZEND_HASH_INDEX_FIND(ht, hval, retval, num_undef);
	ZVAL_COPY_DEREF(result, retval);
	return;

num_undef:
	ZVAL_NULL(result);
}

static int ZEND_FASTCALL zend_jit_fetch_dim_isset_helper(zend_array *ht, zval *dim)
{
	zend_ulong hval;
	zend_string *offset_key;
	zval *retval;

	if (Z_TYPE_P(dim) == IS_REFERENCE) {
		dim = Z_REFVAL_P(dim);
	}

	switch (Z_TYPE_P(dim)) {
		case IS_LONG:
			hval = Z_LVAL_P(dim);
			goto num_index;
		case IS_STRING:
			offset_key = Z_STR_P(dim);
			goto str_index;
		case IS_UNDEF:
			/* The array may be destroyed while throwing the notice.
			 * Temporarily increase the refcount to detect this situation. */
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
				GC_ADDREF(ht);
			}
			zend_jit_undefined_op_helper(EG(current_execute_data)->opline->op2.var);
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && !GC_DELREF(ht)) {
				zend_array_destroy(ht);
				return 0;
			}
			if (EG(exception)) {
				return 0;
			}
			ZEND_FALLTHROUGH;
		case IS_NULL:
			offset_key = ZSTR_EMPTY_ALLOC();
			goto str_index;
		case IS_DOUBLE:
			hval = zend_dval_to_lval(Z_DVAL_P(dim));
			if (!zend_is_long_compatible(Z_DVAL_P(dim), hval)) {
				/* The array may be destroyed while throwing the notice.
				 * Temporarily increase the refcount to detect this situation. */
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
					GC_ADDREF(ht);
				}
				zend_incompatible_double_to_long_error(Z_DVAL_P(dim));
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && !GC_DELREF(ht)) {
					zend_array_destroy(ht);
					return 0;
				}
				if (EG(exception)) {
					return 0;
				}
			}
			goto num_index;
		case IS_RESOURCE:
			/* The array may be destroyed while throwing the notice.
			 * Temporarily increase the refcount to detect this situation. */
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
				GC_ADDREF(ht);
			}
			zend_use_resource_as_offset(dim);
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && !GC_DELREF(ht)) {
				zend_array_destroy(ht);
				return 0;
			}
			if (EG(exception)) {
				return 0;
			}
			hval = Z_RES_HANDLE_P(dim);
			goto num_index;
		case IS_FALSE:
			hval = 0;
			goto num_index;
		case IS_TRUE:
			hval = 1;
			goto num_index;
		default:
			zend_illegal_container_offset(ZSTR_KNOWN(ZEND_STR_ARRAY), dim, BP_VAR_IS);
			return 0;
	}

str_index:
	if (ZEND_HANDLE_NUMERIC(offset_key, hval)) {
		goto num_index;
	}
	retval = zend_hash_find(ht, offset_key);
	if (!retval) {
		return 0;
	}
	if (UNEXPECTED(Z_TYPE_P(retval) == IS_REFERENCE)) {
		retval = Z_REFVAL_P(retval);
	}
	return Z_TYPE_P(retval) > IS_NULL;

num_index:
	ZEND_HASH_INDEX_FIND(ht, hval, retval, num_undef);
	if (UNEXPECTED(Z_TYPE_P(retval) == IS_REFERENCE)) {
		retval = Z_REFVAL_P(retval);
	}
	return (Z_TYPE_P(retval) > IS_NULL);

num_undef:
	return 0;
}

static zval* ZEND_FASTCALL zend_jit_fetch_dim_rw_helper(zend_array *ht, zval *dim)
{
	zend_ulong hval;
	zend_string *offset_key;
	zval *retval;
	zend_execute_data *execute_data;
	const zend_op *opline;

	if (Z_TYPE_P(dim) == IS_REFERENCE) {
		dim = Z_REFVAL_P(dim);
	}

	switch (Z_TYPE_P(dim)) {
		case IS_LONG:
			hval = Z_LVAL_P(dim);
			goto num_index;
		case IS_STRING:
			offset_key = Z_STR_P(dim);
			goto str_index;
		case IS_UNDEF:
			execute_data = EG(current_execute_data);
			opline = EX(opline);
			if (UNEXPECTED(opline->opcode == ZEND_HANDLE_EXCEPTION)) {
				opline = EG(opline_before_exception);
			}
			if (opline && !zend_jit_undefined_op_helper_write(ht, opline->op2.var)) {
				if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
					if (EG(exception)) {
						ZVAL_UNDEF(EX_VAR(opline->result.var));
					} else {
						ZVAL_NULL(EX_VAR(opline->result.var));
					}
				}
				return NULL;
			}
			ZEND_FALLTHROUGH;
		case IS_NULL:
			offset_key = ZSTR_EMPTY_ALLOC();
			goto str_index;
		case IS_DOUBLE:
			hval = zend_dval_to_lval(Z_DVAL_P(dim));
			if (!zend_is_long_compatible(Z_DVAL_P(dim), hval)) {
				/* The array may be destroyed while throwing the notice.
				 * Temporarily increase the refcount to detect this situation. */
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
					GC_ADDREF(ht);
				}
				execute_data = EG(current_execute_data);
				opline = EX(opline);
				zend_incompatible_double_to_long_error(Z_DVAL_P(dim));
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && GC_DELREF(ht) != 1) {
					if (!GC_REFCOUNT(ht)) {
						zend_array_destroy(ht);
					}
					if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
						if (EG(exception)) {
							ZVAL_UNDEF(EX_VAR(opline->result.var));
						} else {
							ZVAL_NULL(EX_VAR(opline->result.var));
						}
					}
					return NULL;
				}
				if (EG(exception)) {
					if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
						ZVAL_UNDEF(EX_VAR(opline->result.var));
					}
					return NULL;
				}
			}
			goto num_index;
		case IS_RESOURCE:
			/* The array may be destroyed while throwing the notice.
			 * Temporarily increase the refcount to detect this situation. */
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
				GC_ADDREF(ht);
			}
			execute_data = EG(current_execute_data);
			opline = EX(opline);
			zend_use_resource_as_offset(dim);
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && GC_DELREF(ht) != 1) {
				if (!GC_REFCOUNT(ht)) {
					zend_array_destroy(ht);
				}
				if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
					if (EG(exception)) {
						ZVAL_UNDEF(EX_VAR(opline->result.var));
					} else {
						ZVAL_NULL(EX_VAR(opline->result.var));
					}
				}
				return NULL;
			}
			if (EG(exception)) {
				if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
					ZVAL_UNDEF(EX_VAR(opline->result.var));
				}
				return NULL;
			}
			hval = Z_RES_HANDLE_P(dim);
			goto num_index;
		case IS_FALSE:
			hval = 0;
			goto num_index;
		case IS_TRUE:
			hval = 1;
			goto num_index;
		default:
			zend_illegal_container_offset(ZSTR_KNOWN(ZEND_STR_ARRAY), dim, BP_VAR_RW);
			undef_result_after_exception();
			return NULL;
	}

str_index:
	if (ZEND_HANDLE_NUMERIC(offset_key, hval)) {
		goto num_index;
	}
	retval = zend_hash_find(ht, offset_key);
	if (!retval) {
		/* Key may be released while throwing the undefined index warning. */
		retval = zend_undefined_index_write(ht, offset_key);
	}
	return retval;

num_index:
	ZEND_HASH_INDEX_FIND(ht, hval, retval, num_undef);
	return retval;

num_undef:
	return zend_undefined_offset_write(ht, hval);
}

static zval* ZEND_FASTCALL zend_jit_fetch_dim_w_helper(zend_array *ht, zval *dim)
{
	zend_ulong hval;
	zend_string *offset_key;
	zval *retval;
	zend_execute_data *execute_data;
	const zend_op *opline;

	if (Z_TYPE_P(dim) == IS_REFERENCE) {
		dim = Z_REFVAL_P(dim);
	}

	switch (Z_TYPE_P(dim)) {
		case IS_LONG:
			hval = Z_LVAL_P(dim);
			goto num_index;
		case IS_STRING:
			offset_key = Z_STR_P(dim);
			goto str_index;
		case IS_UNDEF:
			execute_data = EG(current_execute_data);
			opline = EX(opline);
			if (!zend_jit_undefined_op_helper_write(ht, opline->op2.var)) {
				if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
					if (EG(exception)) {
						ZVAL_UNDEF(EX_VAR(opline->result.var));
					} else {
						ZVAL_NULL(EX_VAR(opline->result.var));
					}
				}
				if (opline->opcode == ZEND_ASSIGN_DIM
				 && ((opline+1)->op1_type & (IS_VAR | IS_TMP_VAR))) {
					zval_ptr_dtor_nogc(EX_VAR((opline+1)->op1.var));
				}
				return NULL;
			}
			ZEND_FALLTHROUGH;
		case IS_NULL:
			offset_key = ZSTR_EMPTY_ALLOC();
			goto str_index;
		case IS_DOUBLE:
			hval = zend_dval_to_lval(Z_DVAL_P(dim));
			if (!zend_is_long_compatible(Z_DVAL_P(dim), hval)) {
				/* The array may be destroyed while throwing the notice.
				 * Temporarily increase the refcount to detect this situation. */
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
					GC_ADDREF(ht);
				}
				execute_data = EG(current_execute_data);
				opline = EX(opline);
				zend_incompatible_double_to_long_error(Z_DVAL_P(dim));
				if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && GC_DELREF(ht) != 1) {
					if (!GC_REFCOUNT(ht)) {
						zend_array_destroy(ht);
					}
					if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
						if (EG(exception)) {
							ZVAL_UNDEF(EX_VAR(opline->result.var));
						} else {
							ZVAL_NULL(EX_VAR(opline->result.var));
						}
					}
					return NULL;
				}
				if (EG(exception)) {
					if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
						ZVAL_UNDEF(EX_VAR(opline->result.var));
					}
					return NULL;
				}
			}
			goto num_index;
		case IS_RESOURCE:
			/* The array may be destroyed while throwing the notice.
			 * Temporarily increase the refcount to detect this situation. */
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE)) {
				GC_ADDREF(ht);
			}
			execute_data = EG(current_execute_data);
			opline = EX(opline);
			zend_use_resource_as_offset(dim);
			if (!(GC_FLAGS(ht) & IS_ARRAY_IMMUTABLE) && GC_DELREF(ht) != 1) {
				if (!GC_REFCOUNT(ht)) {
					zend_array_destroy(ht);
				}
				if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
					if (EG(exception)) {
						ZVAL_UNDEF(EX_VAR(opline->result.var));
					} else {
						ZVAL_NULL(EX_VAR(opline->result.var));
					}
				}
				return NULL;
			}
			if (EG(exception)) {
				if (opline->result_type & (IS_VAR | IS_TMP_VAR)) {
					ZVAL_UNDEF(EX_VAR(opline->result.var));
				}
				return NULL;
			}
			hval = Z_RES_HANDLE_P(dim);
			goto num_index;
		case IS_FALSE:
			hval = 0;
			goto num_index;
		case IS_TRUE:
			hval = 1;
			goto num_index;
		default:
			zend_illegal_container_offset(ZSTR_KNOWN(ZEND_STR_ARRAY), dim, BP_VAR_R);
			undef_result_after_exception();
			if (EG(opline_before_exception)
			 && (EG(opline_before_exception)+1)->opcode == ZEND_OP_DATA
			 && ((EG(opline_before_exception)+1)->op1_type & (IS_VAR|IS_TMP_VAR))) {
				zend_execute_data *execute_data = EG(current_execute_data);

				zval_ptr_dtor_nogc(EX_VAR((EG(opline_before_exception)+1)->op1.var));
			}
			return NULL;
	}

str_index:
	if (ZEND_HANDLE_NUMERIC(offset_key, hval)) {
		goto num_index;
	}
	return zend_hash_lookup(ht, offset_key);

num_index:
	ZEND_HASH_INDEX_LOOKUP(ht, hval, retval);
	return retval;
}

/* type is one of the BP_VAR_* constants */
static zend_never_inline zend_long zend_check_string_offset(zval *dim, int type)
{
	zend_long offset;

try_again:
	switch(Z_TYPE_P(dim)) {
		case IS_LONG:
			return Z_LVAL_P(dim);
		case IS_STRING:
		{
			bool trailing_data = false;
			/* For BC reasons we allow errors so that we can warn on leading numeric string */
			if (IS_LONG == is_numeric_string_ex(Z_STRVAL_P(dim), Z_STRLEN_P(dim), &offset, NULL,
					/* allow errors */ true, NULL, &trailing_data)) {
				if (UNEXPECTED(trailing_data)
				 && EG(current_execute_data)->opline->opcode != ZEND_FETCH_DIM_UNSET) {
					zend_error(E_WARNING, "Illegal string offset \"%s\"", Z_STRVAL_P(dim));
				}
				return offset;
			}
			zend_illegal_container_offset(ZSTR_KNOWN(ZEND_STR_STRING), dim, BP_VAR_R);
			return 0;
		}
		case IS_UNDEF:
			zend_jit_undefined_op_helper(EG(current_execute_data)->opline->op2.var);
			ZEND_FALLTHROUGH;
		case IS_DOUBLE:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
			zend_error(E_WARNING, "String offset cast occurred");
			break;
		case IS_REFERENCE:
			dim = Z_REFVAL_P(dim);
			goto try_again;
		default:
			zend_illegal_container_offset(ZSTR_KNOWN(ZEND_STR_STRING), dim, type);
			return 0;
	}

	return zval_get_long_func(dim, /* is_strict */ false);
}

static zend_always_inline zend_string* zend_jit_fetch_dim_str_offset(zend_string *str, zend_long offset)
{
	if (UNEXPECTED((zend_ulong)offset >= (zend_ulong)ZSTR_LEN(str))) {
		if (EXPECTED(offset < 0)) {
			/* Handle negative offset */
			zend_long real_offset = (zend_long)ZSTR_LEN(str) + offset;

			if (EXPECTED(real_offset >= 0)) {
				return ZSTR_CHAR((uint8_t)ZSTR_VAL(str)[real_offset]);
			}
		}
		zend_error(E_WARNING, "Uninitialized string offset " ZEND_LONG_FMT, offset);
		return ZSTR_EMPTY_ALLOC();
	} else {
		return ZSTR_CHAR((uint8_t)ZSTR_VAL(str)[offset]);
	}
}

static zend_string* ZEND_FASTCALL zend_jit_fetch_dim_str_offset_r_helper(zend_string *str, zend_long offset)
{
	return zend_jit_fetch_dim_str_offset(str, offset);
}

static zend_string* ZEND_FASTCALL zend_jit_fetch_dim_str_r_helper(zend_string *str, zval *dim)
{
	zend_long offset;

	if (UNEXPECTED(Z_TYPE_P(dim) != IS_LONG)) {
		if (!(GC_FLAGS(str) & IS_STR_INTERNED)) {
			GC_ADDREF(str);
		}
		offset = zend_check_string_offset(dim, BP_VAR_R);
		if (!(GC_FLAGS(str) & IS_STR_INTERNED) && UNEXPECTED(GC_DELREF(str) == 0)) {
			zend_string *ret = zend_jit_fetch_dim_str_offset(str, offset);
			zend_string_efree(str);
			return ret;
		}
	} else {
		offset = Z_LVAL_P(dim);
	}
	if (UNEXPECTED(EG(exception) != NULL)) {
		return ZSTR_EMPTY_ALLOC();
	}
	return zend_jit_fetch_dim_str_offset(str, offset);
}

static void ZEND_FASTCALL zend_jit_fetch_dim_str_is_helper(zend_string *str, zval *dim, zval *result)
{
	zend_long offset;

try_string_offset:
	if (UNEXPECTED(Z_TYPE_P(dim) != IS_LONG)) {
		switch (Z_TYPE_P(dim)) {
			/* case IS_LONG: */
			case IS_STRING:
				if (IS_LONG == is_numeric_string(Z_STRVAL_P(dim), Z_STRLEN_P(dim), NULL, NULL, false)) {
					break;
				}
				ZVAL_NULL(result);
				return;
			case IS_UNDEF:
				zend_jit_undefined_op_helper(EG(current_execute_data)->opline->op2.var);
			case IS_DOUBLE:
			case IS_NULL:
			case IS_FALSE:
			case IS_TRUE:
				break;
			case IS_REFERENCE:
				dim = Z_REFVAL_P(dim);
				goto try_string_offset;
			default:
				zend_illegal_container_offset(ZSTR_KNOWN(ZEND_STR_STRING), dim,
					EG(current_execute_data)->opline->opcode == ZEND_ISSET_ISEMPTY_DIM_OBJ ?
						BP_VAR_IS : BP_VAR_RW);
				ZVAL_NULL(result);
				return;
		}

		offset = zval_get_long_func(dim, /* is_strict */ false);
	} else {
		offset = Z_LVAL_P(dim);
	}

	if ((zend_ulong)offset >= (zend_ulong)ZSTR_LEN(str)) {
		if (offset < 0) {
			/* Handle negative offset */
			zend_long real_offset = (zend_long)ZSTR_LEN(str) + offset;

			if (real_offset >= 0) {
				ZVAL_CHAR(result, (uint8_t)ZSTR_VAL(str)[real_offset]);
				return;
			}
		}
		ZVAL_NULL(result);
	} else {
		ZVAL_CHAR(result, (uint8_t)ZSTR_VAL(str)[offset]);
	}
}

static void ZEND_FASTCALL zend_jit_fetch_dim_obj_r_helper(zval *container, zval *dim, zval *result)
{
	zval *retval;
	zend_object *obj = Z_OBJ_P(container);

	GC_ADDREF(obj);
	if (UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
		zend_jit_undefined_op_helper(EG(current_execute_data)->opline->op2.var);
		dim = &EG(uninitialized_zval);
	}

	retval = obj->handlers->read_dimension(obj, dim, BP_VAR_R, result);

	if (retval) {
		if (result != retval) {
			ZVAL_COPY_DEREF(result, retval);
		} else if (UNEXPECTED(Z_ISREF_P(retval))) {
			zend_unwrap_reference(retval);
		}
	} else {
		ZVAL_NULL(result);
	}
	if (UNEXPECTED(GC_DELREF(obj) == 0)) {
		zend_objects_store_del(obj);
	}
}

static void ZEND_FASTCALL zend_jit_fetch_dim_obj_is_helper(zval *container, zval *dim, zval *result)
{
	zval *retval;
	zend_object *obj = Z_OBJ_P(container);

	GC_ADDREF(obj);
	if (UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
		zend_jit_undefined_op_helper(EG(current_execute_data)->opline->op2.var);
		dim = &EG(uninitialized_zval);
	}

	retval = obj->handlers->read_dimension(obj, dim, BP_VAR_IS, result);

	if (retval) {
		if (result != retval) {
			ZVAL_COPY_DEREF(result, retval);
		} else if (UNEXPECTED(Z_ISREF_P(retval))) {
			zend_unwrap_reference(result);
		}
	} else {
		ZVAL_NULL(result);
	}
	if (UNEXPECTED(GC_DELREF(obj) == 0)) {
		zend_objects_store_del(obj);
	}
}

static zend_never_inline void zend_assign_to_string_offset(zval *str, zval *dim, zval *value, zval *result)
{
	uint8_t c;
	size_t string_len;
	zend_long offset;
	zend_string *s;

	/* separate string */
	if (Z_REFCOUNTED_P(str) && Z_REFCOUNT_P(str) == 1) {
		s = Z_STR_P(str);
	} else {
		s = zend_string_init(Z_STRVAL_P(str), Z_STRLEN_P(str), 0);
		ZSTR_H(s) = ZSTR_H(Z_STR_P(str));
		if (Z_REFCOUNTED_P(str)) {
			GC_DELREF(Z_STR_P(str));
		}
		ZVAL_NEW_STR(str, s);
	}

	if (UNEXPECTED(Z_TYPE_P(dim) != IS_LONG)) {
		/* The string may be destroyed while throwing the notice.
		 * Temporarily increase the refcount to detect this situation. */
		GC_ADDREF(s);
		offset = zend_check_string_offset(dim, BP_VAR_W);
		if (UNEXPECTED(GC_DELREF(s) == 0)) {
			zend_string_efree(s);
			if (result) {
				ZVAL_NULL(result);
			}
			return;
		}
		if (UNEXPECTED(EG(exception) != NULL)) {
			if (UNEXPECTED(result)) {
				ZVAL_UNDEF(result);
			}
			return;
		}
	} else {
		offset = Z_LVAL_P(dim);
	}
	if (offset < -(zend_long)ZSTR_LEN(s)) {
		/* Error on negative offset */
		zend_error(E_WARNING, "Illegal string offset " ZEND_LONG_FMT, offset);
		if (result) {
			ZVAL_NULL(result);
		}
		return;
	}

	if (Z_TYPE_P(value) != IS_STRING) {
		zend_string *tmp;

		/* The string may be destroyed while throwing the notice.
		 * Temporarily increase the refcount to detect this situation. */
		GC_ADDREF(s);

		if (UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
			const zend_op *op_data = EG(current_execute_data)->opline + 1;
			ZEND_ASSERT(op_data->opcode == ZEND_OP_DATA && op_data->op1_type == IS_CV);
			zend_jit_undefined_op_helper(op_data->op1.var);
			value = &EG(uninitialized_zval);
		}

		/* Convert to string, just the time to pick the 1st byte */
		tmp = zval_try_get_string_func(value);

		if (UNEXPECTED(GC_DELREF(s) == 0)) {
			zend_string_efree(s);
			if (tmp) {
				zend_string_release_ex(tmp, 0);
			}
			if (result) {
				ZVAL_NULL(result);
			}
			return;
		}
		if (UNEXPECTED(!tmp)) {
			if (result) {
				ZVAL_UNDEF(result);
			}
			return;
		}

		if (UNEXPECTED(!tmp)) {
			if (result) {
				ZVAL_UNDEF(result);
			}
			return;
		}

		string_len = ZSTR_LEN(tmp);
		c = (uint8_t)ZSTR_VAL(tmp)[0];
		zend_string_release(tmp);
	} else {
		string_len = Z_STRLEN_P(value);
		c = (uint8_t)Z_STRVAL_P(value)[0];
	}


	if (string_len != 1) {
		if (string_len == 0) {
			/* Error on empty input string */
			zend_throw_error(NULL, "Cannot assign an empty string to a string offset");
			if (result) {
				ZVAL_NULL(result);
			}
			return;
		}

		/* The string may be destroyed while throwing the notice.
		 * Temporarily increase the refcount to detect this situation. */
		GC_ADDREF(s);
		zend_error(E_WARNING, "Only the first byte will be assigned to the string offset");
		if (UNEXPECTED(GC_DELREF(s) == 0)) {
			zend_string_efree(s);
			if (result) {
				ZVAL_NULL(result);
			}
			return;
		}
		/* Illegal offset assignment */
		if (UNEXPECTED(EG(exception) != NULL)) {
			if (result) {
				ZVAL_UNDEF(result);
			}
			return;
		}
	}

	if (offset < 0) { /* Handle negative offset */
		offset += (zend_long)ZSTR_LEN(s);
	}

	if ((size_t)offset >= ZSTR_LEN(s)) {
		/* Extend string if needed */
		zend_long old_len = ZSTR_LEN(s);
		ZVAL_NEW_STR(str, zend_string_extend(s, (size_t)offset + 1, 0));
		memset(Z_STRVAL_P(str) + old_len, ' ', offset - old_len);
		Z_STRVAL_P(str)[offset+1] = 0;
	} else {
		zend_string_forget_hash_val(Z_STR_P(str));
	}

	Z_STRVAL_P(str)[offset] = c;

	if (result) {
		/* Return the new character */
		ZVAL_CHAR(result, c);
	}
}

static zend_always_inline void ZEND_FASTCALL zend_jit_fetch_dim_obj_helper(zval *object_ptr, zval *dim, zval *result, int type)
{
	zval *retval;

	if (EXPECTED(Z_TYPE_P(object_ptr) == IS_OBJECT)) {
		zend_object *obj = Z_OBJ_P(object_ptr);

		GC_ADDREF(obj);
		if (dim && UNEXPECTED(Z_ISUNDEF_P(dim))) {
			const zend_op *opline = EG(current_execute_data)->opline;
			zend_jit_undefined_op_helper(opline->op2.var);
			dim = &EG(uninitialized_zval);
		}

		retval = obj->handlers->read_dimension(obj, dim, type, result);
		if (UNEXPECTED(retval == &EG(uninitialized_zval))) {
			zend_class_entry *ce = obj->ce;

			ZVAL_NULL(result);
			zend_error(E_NOTICE, "Indirect modification of overloaded element of %s has no effect", ZSTR_VAL(ce->name));
		} else if (EXPECTED(retval && Z_TYPE_P(retval) != IS_UNDEF)) {
			if (!Z_ISREF_P(retval)) {
				if (result != retval) {
					ZVAL_COPY(result, retval);
					retval = result;
				}
				if (Z_TYPE_P(retval) != IS_OBJECT) {
					zend_class_entry *ce = obj->ce;
					zend_error(E_NOTICE, "Indirect modification of overloaded element of %s has no effect", ZSTR_VAL(ce->name));
				}
			} else if (UNEXPECTED(Z_REFCOUNT_P(retval) == 1)) {
				ZVAL_UNREF(retval);
			}
			if (result != retval) {
				ZVAL_INDIRECT(result, retval);
			}
		} else {
			ZEND_ASSERT(EG(exception) && "read_dimension() returned NULL without exception");
			ZVAL_UNDEF(result);
		}
		if (UNEXPECTED(GC_DELREF(obj) == 0)) {
			zend_objects_store_del(obj);
		}
	} else if (EXPECTED(Z_TYPE_P(object_ptr) == IS_STRING)) {
		if (!dim) {
			zend_throw_error(NULL, "[] operator not supported for strings");
		} else {
			if (UNEXPECTED(Z_TYPE_P(dim) != IS_LONG)) {
				zend_check_string_offset(dim, BP_VAR_RW);
			}
			zend_wrong_string_offset_error();
		}
		ZVAL_UNDEF(result);
	} else if (Z_TYPE_P(object_ptr) == IS_FALSE) {
		zend_array *arr = zend_new_array(0);
		ZVAL_ARR(object_ptr, arr);
		GC_ADDREF(arr);
		zend_false_to_array_deprecated();
		if (UNEXPECTED(GC_DELREF(arr) == 0)) {
			zend_array_destroy(arr);
			ZVAL_NULL(result);
			return;
		}
		SEPARATE_ARRAY(object_ptr);
		arr = Z_ARRVAL_P(object_ptr);
		zval *var;
		if (dim) {
			if (type == BP_VAR_W) {
				var = zend_jit_fetch_dim_w_helper(arr, dim);
			} else {
				ZEND_ASSERT(type == BP_VAR_RW);
				var = zend_jit_fetch_dim_rw_helper(arr, dim);
			}
		} else {
			var = zend_hash_next_index_insert_new(arr, &EG(uninitialized_zval));
		}
		if (var) {
			ZVAL_INDIRECT(result, var);
		} else {
			ZVAL_UNDEF(result);
		}
	} else {
		if (type == BP_VAR_UNSET) {
			zend_throw_error(NULL, "Cannot unset offset in a non-array variable");
			ZVAL_UNDEF(result);
		} else {
			zend_throw_error(NULL, "Cannot use a scalar value as an array");
			ZVAL_UNDEF(result);
		}
	}
}

static void ZEND_FASTCALL zend_jit_fetch_dim_obj_w_helper(zval *object_ptr, zval *dim, zval *result)
{
	zend_jit_fetch_dim_obj_helper(object_ptr, dim, result, BP_VAR_W);
}

static void ZEND_FASTCALL zend_jit_fetch_dim_obj_rw_helper(zval *object_ptr, zval *dim, zval *result)
{
	zend_jit_fetch_dim_obj_helper(object_ptr, dim, result, BP_VAR_RW);
}

//static void ZEND_FASTCALL zend_jit_fetch_dim_obj_unset_helper(zval *object_ptr, zval *dim, zval *result)
//{
//	zend_jit_fetch_dim_obj_helper(object_ptr, dim, result, BP_VAR_UNSET);
//}

static void ZEND_FASTCALL zend_jit_assign_dim_helper(zval *object_ptr, zval *dim, zval *value, zval *result)
{
	if (EXPECTED(Z_TYPE_P(object_ptr) == IS_OBJECT)) {
		zend_object *obj = Z_OBJ_P(object_ptr);

		GC_ADDREF(obj);
		if (dim && UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
			const zend_op *opline = EG(current_execute_data)->opline;
			zend_jit_undefined_op_helper(opline->op2.var);
			dim = &EG(uninitialized_zval);
		}

		if (UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
			const zend_op *op_data = EG(current_execute_data)->opline + 1;
			ZEND_ASSERT(op_data->opcode == ZEND_OP_DATA && op_data->op1_type == IS_CV);
			zend_jit_undefined_op_helper(op_data->op1.var);
			value = &EG(uninitialized_zval);
		} else {
			ZVAL_DEREF(value);
		}

		obj->handlers->write_dimension(obj, dim, value);
		if (result) {
			if (EXPECTED(!EG(exception))) {
				ZVAL_COPY(result, value);
			} else {
				ZVAL_UNDEF(result);
			}
		}
		if (UNEXPECTED(GC_DELREF(obj) == 0)) {
			zend_objects_store_del(obj);
		}
		return;
	} else if (EXPECTED(Z_TYPE_P(object_ptr) == IS_STRING) && EXPECTED(dim != NULL)) {
		zend_assign_to_string_offset(object_ptr, dim, value, result);
		return;
	}

	if (UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
		const zend_op *op_data = EG(current_execute_data)->opline + 1;
		ZEND_ASSERT(op_data->opcode == ZEND_OP_DATA && op_data->op1_type == IS_CV);
		zend_jit_undefined_op_helper(op_data->op1.var);
		value = &EG(uninitialized_zval);
	}

	if (EXPECTED(Z_TYPE_P(object_ptr) == IS_STRING)) {
		zend_throw_error(NULL, "[] operator not supported for strings");
		if (result) {
			ZVAL_UNDEF(result);
		}
	} else if (Z_TYPE_P(object_ptr) == IS_FALSE) {
		zend_array *arr = zend_new_array(0);
		ZVAL_ARR(object_ptr, arr);
		GC_ADDREF(arr);
		zend_false_to_array_deprecated();
		if (UNEXPECTED(GC_DELREF(arr) == 0)) {
			zend_array_destroy(arr);
			if (result) {
				ZVAL_NULL(result);
			}
			return;
		}
		SEPARATE_ARRAY(object_ptr);
		arr = Z_ARRVAL_P(object_ptr);
		zval *var = dim
			? zend_jit_fetch_dim_w_helper(arr, dim)
			: zend_hash_next_index_insert_new(arr, &EG(uninitialized_zval));
		if (!var) {
			if (result) {
				ZVAL_UNDEF(result);
			}
			return;
		}

		ZVAL_COPY_DEREF(var, value);
		if (result) {
			ZVAL_COPY(result, var);
		}
	} else {
		if (dim && UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
			const zend_op *opline = EG(current_execute_data)->opline;
			zend_jit_undefined_op_helper(opline->op2.var);
			dim = &EG(uninitialized_zval);
		}
		zend_throw_error(NULL, "Cannot use a scalar value as an array");
		if (result) {
			ZVAL_UNDEF(result);
		}
	}
}

static void ZEND_FASTCALL zend_jit_assign_dim_op_helper(zval *container, zval *dim, zval *value, binary_op_type binary_op)
{
	if (EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
		zend_object *obj = Z_OBJ_P(container);
		zval *z;
		zval rv, res;

		GC_ADDREF(obj);
		if (dim && UNEXPECTED(Z_ISUNDEF_P(dim))) {
			const zend_op *opline = EG(current_execute_data)->opline;
			zend_jit_undefined_op_helper(opline->op2.var);
			dim = &EG(uninitialized_zval);
		}

		z = obj->handlers->read_dimension(obj, dim, BP_VAR_R, &rv);
		if (z != NULL) {

			if (binary_op(&res, Z_ISREF_P(z) ? Z_REFVAL_P(z) : z, value) == SUCCESS) {
				obj->handlers->write_dimension(obj, dim, &res);
			}
			if (z == &rv) {
				zval_ptr_dtor(&rv);
			}
			zval_ptr_dtor(&res);
		} else {
			/* Exception is thrown in this case */
			GC_DELREF(obj);
			return;
		}
		if (UNEXPECTED(GC_DELREF(obj) == 0)) {
			zend_objects_store_del(obj);
//???		if (retval) {
//???			ZVAL_NULL(retval);
//???		}
		}
	} else if (UNEXPECTED(Z_TYPE_P(container) == IS_STRING)) {
		if (!dim) {
			zend_throw_error(NULL, "[] operator not supported for strings");
		} else {
			if (UNEXPECTED(Z_TYPE_P(dim) != IS_LONG)) {
				zend_check_string_offset(dim, BP_VAR_RW);
			}
			zend_wrong_string_offset_error();
		}
	} else if (Z_TYPE_P(container) == IS_FALSE) {
		zend_array *arr = zend_new_array(0);
		ZVAL_ARR(container, arr);
		GC_ADDREF(arr);
		zend_false_to_array_deprecated();
		if (UNEXPECTED(GC_DELREF(arr) == 0)) {
			zend_array_destroy(arr);
			return;
		}
		SEPARATE_ARRAY(container);
		arr = Z_ARRVAL_P(container);
		zval *var = dim
			? zend_jit_fetch_dim_rw_helper(arr, dim)
			: zend_hash_next_index_insert_new(arr, &EG(uninitialized_zval));
		if (var) {
			binary_op(var, var, value);
		}
	} else {
		zend_throw_error(NULL, "Cannot use a scalar value as an array");
	}
}

static void ZEND_FASTCALL zend_jit_fast_assign_concat_helper(zval *op1, zval *op2)
{
	size_t op1_len = Z_STRLEN_P(op1);
	size_t op2_len = Z_STRLEN_P(op2);
	size_t result_len = op1_len + op2_len;
	zend_string *result_str;
	uint32_t flags = ZSTR_GET_COPYABLE_CONCAT_PROPERTIES_BOTH(Z_STR_P(op1), Z_STR_P(op2));

	if (UNEXPECTED(op1_len > SIZE_MAX - op2_len)) {
		zend_throw_error(NULL, "String size overflow");
		return;
	}

	do {
		if (Z_REFCOUNTED_P(op1)) {
			if (GC_REFCOUNT(Z_STR_P(op1)) == 1) {
				result_str = perealloc(Z_STR_P(op1), ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(result_len)), 0);
				ZSTR_LEN(result_str) = result_len;
				zend_string_forget_hash_val(result_str);
				if (UNEXPECTED(Z_STR_P(op1) == Z_STR_P(op2))) {
					ZVAL_NEW_STR(op2, result_str);
				}
				break;
			}
			GC_DELREF(Z_STR_P(op1));
		}
		result_str = zend_string_alloc(result_len, 0);
		memcpy(ZSTR_VAL(result_str), Z_STRVAL_P(op1), op1_len);
	} while(0);

	GC_ADD_FLAGS(result_str, flags);
	ZVAL_NEW_STR(op1, result_str);
	memcpy(ZSTR_VAL(result_str) + op1_len, Z_STRVAL_P(op2), op2_len);
	ZSTR_VAL(result_str)[result_len] = '\0';
}

static void ZEND_FASTCALL zend_jit_fast_concat_helper(zval *result, zval *op1, zval *op2)
{
	size_t op1_len = Z_STRLEN_P(op1);
	size_t op2_len = Z_STRLEN_P(op2);
	size_t result_len = op1_len + op2_len;
	zend_string *result_str;
	uint32_t flags = ZSTR_GET_COPYABLE_CONCAT_PROPERTIES_BOTH(Z_STR_P(op1), Z_STR_P(op2));

	if (UNEXPECTED(op1_len > SIZE_MAX - op2_len)) {
		zend_throw_error(NULL, "String size overflow");
		return;
	}

	result_str = zend_string_alloc(result_len, 0);
	GC_ADD_FLAGS(result_str, flags);
	memcpy(ZSTR_VAL(result_str), Z_STRVAL_P(op1), op1_len);

	ZVAL_NEW_STR(result, result_str);

	memcpy(ZSTR_VAL(result_str) + op1_len, Z_STRVAL_P(op2), op2_len);
	ZSTR_VAL(result_str)[result_len] = '\0';
}

static void ZEND_FASTCALL zend_jit_fast_concat_tmp_helper(zval *result, zval *op1, zval *op2)
{
	zend_string *op1_str = Z_STR_P(op1);
	size_t op1_len = ZSTR_LEN(op1_str);
	size_t op2_len = Z_STRLEN_P(op2);
	size_t result_len = op1_len + op2_len;
	zend_string *result_str;
	uint32_t flags = ZSTR_GET_COPYABLE_CONCAT_PROPERTIES_BOTH(Z_STR_P(op1), Z_STR_P(op2));

	if (UNEXPECTED(op1_len > SIZE_MAX - op2_len)) {
		zend_throw_error(NULL, "String size overflow");
		return;
	}

	do {
		if (!ZSTR_IS_INTERNED(op1_str)) {
			if (GC_REFCOUNT(op1_str) == 1) {
				Z_STR_P(op1) = result_str =
					perealloc(op1_str, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(result_len)), 0);
				ZSTR_LEN(result_str) = result_len;
				zend_string_forget_hash_val(result_str);
				break;
			}
			GC_DELREF(op1_str);
		}
		result_str = zend_string_alloc(result_len, 0);
		memcpy(ZSTR_VAL(result_str), ZSTR_VAL(op1_str), op1_len);
	} while (0);

	GC_ADD_FLAGS(result_str, flags);
	ZVAL_NEW_STR(result, result_str);

	memcpy(ZSTR_VAL(result_str) + op1_len, Z_STRVAL_P(op2), op2_len);
	ZSTR_VAL(result_str)[result_len] = '\0';
}

static int ZEND_FASTCALL zend_jit_isset_dim_helper(zval *container, zval *offset)
{
	if (UNEXPECTED(Z_TYPE_P(offset) == IS_UNDEF)) {
		zend_jit_undefined_op_helper(EG(current_execute_data)->opline->op2.var);
		offset = &EG(uninitialized_zval);
	}

	if (EXPECTED(Z_TYPE_P(container) == IS_OBJECT)) {
		return Z_OBJ_HT_P(container)->has_dimension(Z_OBJ_P(container), offset, 0);
	} else if (EXPECTED(Z_TYPE_P(container) == IS_STRING)) { /* string offsets */
		zend_long lval;

		if (EXPECTED(Z_TYPE_P(offset) == IS_LONG)) {
			lval = Z_LVAL_P(offset);
isset_str_offset:
			if (UNEXPECTED(lval < 0)) { /* Handle negative offset */
				lval += (zend_long)Z_STRLEN_P(container);
			}
			if (EXPECTED(lval >= 0) && (size_t)lval < Z_STRLEN_P(container)) {
				return 1;
			}
		} else {
			ZVAL_DEREF(offset);
			if (Z_TYPE_P(offset) < IS_STRING /* simple scalar types */
					|| (Z_TYPE_P(offset) == IS_STRING /* or numeric string */
						&& IS_LONG == is_numeric_string(Z_STRVAL_P(offset), Z_STRLEN_P(offset), NULL, NULL, false))) {
				lval = zval_get_long_ex(offset, /* is_strict */ true);
				goto isset_str_offset;
			}
		}
	}
	return 0;
}

static void ZEND_FASTCALL zend_jit_free_call_frame(zend_execute_data *call)
{
	zend_vm_stack_free_call_frame(call);
}

static zend_reference* ZEND_FASTCALL zend_jit_fetch_global_helper(zend_string *varname, void **cache_slot)
{
	zval *value;
	uintptr_t idx;
	zend_reference *ref;

	/* We store "hash slot index" + 1 (NULL is a mark of uninitialized cache slot) */
	idx = (uintptr_t)CACHED_PTR_EX(cache_slot) - 1;
	if (EXPECTED(idx < EG(symbol_table).nNumUsed * sizeof(Bucket))) {
		Bucket *p = (Bucket*)((char*)EG(symbol_table).arData + idx);

		if (EXPECTED(p->key == varname) ||
	        (EXPECTED(p->h == ZSTR_H(varname)) &&
	         EXPECTED(p->key != NULL) &&
	         EXPECTED(zend_string_equal_content(p->key, varname)))) {

			value = (zval*)p; /* value = &p->val; */
			goto check_indirect;
		}
	}

	value = zend_hash_find_known_hash(&EG(symbol_table), varname);
	if (UNEXPECTED(value == NULL)) {
		value = zend_hash_add_new(&EG(symbol_table), varname, &EG(uninitialized_zval));
		idx = (char*)value - (char*)EG(symbol_table).arData;
		/* Store "hash slot index" + 1 (NULL is a mark of uninitialized cache slot) */
		CACHE_PTR_EX(cache_slot, (void*)(idx + 1));
	} else {
		idx = (char*)value - (char*)EG(symbol_table).arData;
		/* Store "hash slot index" + 1 (NULL is a mark of uninitialized cache slot) */
		CACHE_PTR_EX(cache_slot, (void*)(idx + 1));
check_indirect:
		/* GLOBAL variable may be an INDIRECT pointer to CV */
		if (UNEXPECTED(Z_TYPE_P(value) == IS_INDIRECT)) {
			value = Z_INDIRECT_P(value);
			if (UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
				ZVAL_NULL(value);
			}
		}
	}

	if (UNEXPECTED(!Z_ISREF_P(value))) {
		ZVAL_MAKE_REF_EX(value, 2);
		ref = Z_REF_P(value);
	} else {
		ref = Z_REF_P(value);
		GC_ADDREF(ref);
	}

	return ref;
}

static bool ZEND_FASTCALL zend_jit_verify_arg_slow(zval *arg, zend_arg_info *arg_info)
{
	zend_execute_data *execute_data = EG(current_execute_data);
	const zend_op *opline = EX(opline);
	void **cache_slot = CACHE_ADDR(opline->extended_value);
	bool ret = zend_check_user_type_slow(
		&arg_info->type, arg, /* ref */ NULL, cache_slot, /* is_return_type */ false);
	if (UNEXPECTED(!ret)) {
		zend_verify_arg_error(EX(func), arg_info, opline->op1.num, arg);
		return 0;
	}
	return ret;
}

static void ZEND_FASTCALL zend_jit_verify_return_slow(zval *arg, const zend_op_array *op_array, zend_arg_info *arg_info, void **cache_slot)
{
	if (UNEXPECTED(!zend_check_user_type_slow(
			&arg_info->type, arg, /* ref */ NULL, cache_slot, /* is_return_type */ true))) {
		zend_verify_return_error((zend_function*)op_array, arg);
	}
}

static void ZEND_FASTCALL zend_jit_fetch_obj_r_slow(zend_object *zobj)
{
	zval *retval;
	zend_execute_data *execute_data = EG(current_execute_data);
	const zend_op *opline = EX(opline);
	zend_string *name = Z_STR_P(RT_CONSTANT(opline, opline->op2));
	zval *result = EX_VAR(opline->result.var);
	void **cache_slot = CACHE_ADDR(opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS);

	retval = zobj->handlers->read_property(zobj, name, BP_VAR_R, cache_slot, result);
	if (retval != result) {
		ZVAL_COPY_DEREF(result, retval);
	} else if (UNEXPECTED(Z_ISREF_P(retval))) {
		zend_unwrap_reference(retval);
	}
}

static void ZEND_FASTCALL zend_jit_fetch_obj_r_dynamic(zend_object *zobj, intptr_t prop_offset)
{
	if (zobj->properties) {
		zval *retval;
		zend_execute_data *execute_data = EG(current_execute_data);
		const zend_op *opline = EX(opline);
		zend_string *name = Z_STR_P(RT_CONSTANT(opline, opline->op2));
		zval *result = EX_VAR(opline->result.var);
		void **cache_slot = CACHE_ADDR(opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS);

		if (!IS_UNKNOWN_DYNAMIC_PROPERTY_OFFSET(prop_offset)) {
			intptr_t idx = ZEND_DECODE_DYN_PROP_OFFSET(prop_offset);

			if (EXPECTED(idx < zobj->properties->nNumUsed * sizeof(Bucket))) {
				Bucket *p = (Bucket*)((char*)zobj->properties->arData + idx);

				if (EXPECTED(p->key == name) ||
			        (EXPECTED(p->h == ZSTR_H(name)) &&
			         EXPECTED(p->key != NULL) &&
			         EXPECTED(zend_string_equal_content(p->key, name)))) {
					ZVAL_COPY_DEREF(result, &p->val);
					return;
				}
			}
			CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_DYNAMIC_PROPERTY_OFFSET);
		}

		retval = zend_hash_find_known_hash(zobj->properties, name);

		if (EXPECTED(retval)) {
			intptr_t idx = (char*)retval - (char*)zobj->properties->arData;
			CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_ENCODE_DYN_PROP_OFFSET(idx));
			ZVAL_COPY_DEREF(result, retval);
			return;
		}
	}
	zend_jit_fetch_obj_r_slow(zobj);
}

static void ZEND_FASTCALL zend_jit_fetch_obj_is_slow(zend_object *zobj)
{
	zval *retval;
	zend_execute_data *execute_data = EG(current_execute_data);
	const zend_op *opline = EX(opline);
	zend_string *name = Z_STR_P(RT_CONSTANT(opline, opline->op2));
	zval *result = EX_VAR(opline->result.var);
	void **cache_slot = CACHE_ADDR(opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS);

	retval = zobj->handlers->read_property(zobj, name, BP_VAR_IS, cache_slot, result);
	if (retval != result) {
		ZVAL_COPY_DEREF(result, retval);
	} else if (UNEXPECTED(Z_ISREF_P(retval))) {
		zend_unwrap_reference(retval);
	}
}

static void ZEND_FASTCALL zend_jit_fetch_obj_is_dynamic(zend_object *zobj, intptr_t prop_offset)
{
	if (zobj->properties) {
		zval *retval;
		zend_execute_data *execute_data = EG(current_execute_data);
		const zend_op *opline = EX(opline);
		zend_string *name = Z_STR_P(RT_CONSTANT(opline, opline->op2));
		zval *result = EX_VAR(opline->result.var);
		void **cache_slot = CACHE_ADDR(opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS);

		if (!IS_UNKNOWN_DYNAMIC_PROPERTY_OFFSET(prop_offset)) {
			intptr_t idx = ZEND_DECODE_DYN_PROP_OFFSET(prop_offset);

			if (EXPECTED(idx < zobj->properties->nNumUsed * sizeof(Bucket))) {
				Bucket *p = (Bucket*)((char*)zobj->properties->arData + idx);

				if (EXPECTED(p->key == name) ||
			        (EXPECTED(p->h == ZSTR_H(name)) &&
			         EXPECTED(p->key != NULL) &&
			         EXPECTED(zend_string_equal_content(p->key, name)))) {
					ZVAL_COPY_DEREF(result, &p->val);
					return;
				}
			}
			CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_DYNAMIC_PROPERTY_OFFSET);
		}

		retval = zend_hash_find_known_hash(zobj->properties, name);

		if (EXPECTED(retval)) {
			intptr_t idx = (char*)retval - (char*)zobj->properties->arData;
			CACHE_PTR_EX(cache_slot + 1, (void*)ZEND_ENCODE_DYN_PROP_OFFSET(idx));
			ZVAL_COPY_DEREF(result, retval);
			return;
		}
	}
	zend_jit_fetch_obj_is_slow(zobj);
}

static zend_always_inline bool promotes_to_array(zval *val) {
	return Z_TYPE_P(val) <= IS_FALSE
		|| (Z_ISREF_P(val) && Z_TYPE_P(Z_REFVAL_P(val)) <= IS_FALSE);
}

static zend_always_inline bool check_type_array_assignable(zend_type type) {
	if (!ZEND_TYPE_IS_SET(type)) {
		return 1;
	}
	return (ZEND_TYPE_FULL_MASK(type) & MAY_BE_ARRAY) != 0;
}

static zend_property_info *zend_object_fetch_property_type_info(
		zend_object *obj, zval *slot)
{
	if (EXPECTED(!ZEND_CLASS_HAS_TYPE_HINTS(obj->ce))) {
		return NULL;
	}

	/* Not a declared property */
	if (UNEXPECTED(slot < obj->properties_table ||
			slot >= obj->properties_table + obj->ce->default_properties_count)) {
		return NULL;
	}

	return zend_get_typed_property_info_for_slot(obj, slot);
}

static zend_never_inline ZEND_COLD void zend_throw_auto_init_in_prop_error(zend_property_info *prop, const char *type) {
	zend_string *type_str = zend_type_to_string(prop->type);
	zend_type_error(
		"Cannot auto-initialize an %s inside property %s::$%s of type %s",
		type,
		ZSTR_VAL(prop->ce->name), zend_get_unmangled_property_name(prop->name),
		ZSTR_VAL(type_str)
	);
	zend_string_release(type_str);
}

static zend_never_inline ZEND_COLD void zend_throw_access_uninit_prop_by_ref_error(
		zend_property_info *prop) {
	zend_throw_error(NULL,
		"Cannot access uninitialized non-nullable property %s::$%s by reference",
		ZSTR_VAL(prop->ce->name),
		zend_get_unmangled_property_name(prop->name));
}

static zend_never_inline bool zend_handle_fetch_obj_flags(
		zval *result, zval *ptr, zend_object *obj, zend_property_info *prop_info, uint32_t flags)
{
	switch (flags) {
		case ZEND_FETCH_DIM_WRITE:
			if (promotes_to_array(ptr)) {
				if (!prop_info) {
					prop_info = zend_object_fetch_property_type_info(obj, ptr);
					if (!prop_info) {
						break;
					}
				}
				if (!check_type_array_assignable(prop_info->type)) {
					zend_throw_auto_init_in_prop_error(prop_info, "array");
					if (result) ZVAL_ERROR(result);
					return 0;
				}
			}
			break;
		case ZEND_FETCH_REF:
			if (Z_TYPE_P(ptr) != IS_REFERENCE) {
				if (!prop_info) {
					prop_info = zend_object_fetch_property_type_info(obj, ptr);
					if (!prop_info) {
						break;
					}
				}
				if (Z_TYPE_P(ptr) == IS_UNDEF) {
					if (!ZEND_TYPE_ALLOW_NULL(prop_info->type)) {
						zend_throw_access_uninit_prop_by_ref_error(prop_info);
						if (result) ZVAL_ERROR(result);
						return 0;
					}
					ZVAL_NULL(ptr);
				}

				ZVAL_NEW_REF(ptr, ptr);
				ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(ptr), prop_info);
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return 1;
}

static void ZEND_FASTCALL zend_jit_fetch_obj_w_slow(zend_object *zobj)
{
	zval *retval;
	zend_execute_data *execute_data = EG(current_execute_data);
	const zend_op *opline = EX(opline);
	zend_string *name = Z_STR_P(RT_CONSTANT(opline, opline->op2));
	zval *result = EX_VAR(opline->result.var);
	void **cache_slot = CACHE_ADDR(opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS);

	retval = zobj->handlers->get_property_ptr_ptr(zobj, name, BP_VAR_W, cache_slot);
	if (NULL == retval) {
		retval = zobj->handlers->read_property(zobj, name, BP_VAR_W, cache_slot, result);
		if (retval == result) {
			if (UNEXPECTED(Z_ISREF_P(retval) && Z_REFCOUNT_P(retval) == 1)) {
				ZVAL_UNREF(retval);
			}
			return;
		}
		if (UNEXPECTED(EG(exception))) {
			ZVAL_ERROR(result);
			return;
		}
	} else if (UNEXPECTED(Z_ISERROR_P(retval))) {
		ZVAL_ERROR(result);
		return;
	}

	ZVAL_INDIRECT(result, retval);

	/* Support for typed properties */
	do {
		uint32_t flags = opline->extended_value & ZEND_FETCH_OBJ_FLAGS;

		if (flags) {
			zend_property_info *prop_info = NULL;

			if (opline->op2_type == IS_CONST) {
				prop_info = CACHED_PTR_EX(cache_slot + 2);
				if (!prop_info) {
					break;
				}
			}
			if (UNEXPECTED(!zend_handle_fetch_obj_flags(result, retval, zobj, prop_info, flags))) {
				return;
			}
		}
	} while (0);

	if (UNEXPECTED(Z_TYPE_P(retval) == IS_UNDEF)) {
		ZVAL_NULL(retval);
	}
}

static void ZEND_FASTCALL zend_jit_check_array_promotion(zval *val, zend_property_info *prop)
{
	zend_execute_data *execute_data = EG(current_execute_data);
	const zend_op *opline = execute_data->opline;
	zval *result = EX_VAR(opline->result.var);

	if ((Z_TYPE_P(val) <= IS_FALSE
		|| (Z_ISREF_P(val) && Z_TYPE_P(Z_REFVAL_P(val)) <= IS_FALSE))
		&& ZEND_TYPE_IS_SET(prop->type)
		&& (ZEND_TYPE_FULL_MASK(prop->type) & MAY_BE_ARRAY) == 0) {
		zend_string *type_str = zend_type_to_string(prop->type);
		zend_type_error(
			"Cannot auto-initialize an array inside property %s::$%s of type %s",
			ZSTR_VAL(prop->ce->name), zend_get_unmangled_property_name(prop->name),
			ZSTR_VAL(type_str)
		);
		zend_string_release(type_str);
		ZVAL_ERROR(result);
	} else {
		ZVAL_INDIRECT(result, val);
	}
}

static void ZEND_FASTCALL zend_jit_create_typed_ref(zval *val, zend_property_info *prop, zval *result)
{
	if (!Z_ISREF_P(val)) {
		ZVAL_NEW_REF(val, val);
		ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(val), prop);
	}
	ZVAL_INDIRECT(result, val);
}

static void ZEND_FASTCALL zend_jit_extract_helper(zend_refcounted *garbage)
{
	zend_execute_data *execute_data = EG(current_execute_data);
	const zend_op *opline = execute_data->opline;
	zval *zv = EX_VAR(opline->result.var);

	if (EXPECTED(Z_TYPE_P(zv) == IS_INDIRECT)) {
		ZVAL_COPY(zv, Z_INDIRECT_P(zv));
	}
	rc_dtor_func(garbage);
}

static void ZEND_FASTCALL zend_jit_vm_stack_free_args_helper(zend_execute_data *call)
{
	zend_vm_stack_free_args(call);
}

static zend_always_inline zval* zend_jit_assign_to_typed_ref_helper(zend_reference *ref, zval *value, uint8_t value_type)
{
	zval variable;

	ZVAL_REF(&variable, ref);
	return zend_assign_to_variable(&variable, value, value_type, ZEND_CALL_USES_STRICT_TYPES(EG(current_execute_data)));
}

static zval* ZEND_FASTCALL zend_jit_assign_const_to_typed_ref(zend_reference *ref, zval *value)
{
	return zend_jit_assign_to_typed_ref_helper(ref, value, IS_CONST);
}

static zval* ZEND_FASTCALL zend_jit_assign_tmp_to_typed_ref(zend_reference *ref, zval *value)
{
	return zend_jit_assign_to_typed_ref_helper(ref, value, IS_TMP_VAR);
}

static zval* ZEND_FASTCALL zend_jit_assign_var_to_typed_ref(zend_reference *ref, zval *value)
{
	return zend_jit_assign_to_typed_ref_helper(ref, value, IS_VAR);
}

static zval* ZEND_FASTCALL zend_jit_assign_cv_to_typed_ref(zend_reference *ref, zval *value)
{
	if (UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
		const zend_op *opline = EG(current_execute_data)->opline;
		uint32_t var;
		if (opline->opcode == ZEND_ASSIGN) {
			var = opline->op2.var;
		} else {
			ZEND_ASSERT((opline + 1)->opcode == ZEND_OP_DATA);
			var = (opline + 1)->op1.var;
		}
		zend_jit_undefined_op_helper(var);
		value = &EG(uninitialized_zval);
	}
	return zend_jit_assign_to_typed_ref_helper(ref, value, IS_CV);
}

static zend_always_inline zval* zend_jit_assign_to_typed_ref2_helper(zend_reference *ref, zval *value, zval *result, uint8_t value_type)
{
	zval variable, *ret;
	zend_refcounted *garbage = NULL;

	ZVAL_REF(&variable, ref);
	ret = zend_assign_to_variable_ex(&variable, value, value_type, ZEND_CALL_USES_STRICT_TYPES(EG(current_execute_data)), &garbage);
	ZVAL_COPY(result, ret);
	if (garbage) {
		GC_DTOR(garbage);
	}
	return ret;
}

static zval* ZEND_FASTCALL zend_jit_assign_const_to_typed_ref2(zend_reference *ref, zval *value, zval *result)
{
	return zend_jit_assign_to_typed_ref2_helper(ref, value, result, IS_CONST);
}

static zval* ZEND_FASTCALL zend_jit_assign_tmp_to_typed_ref2(zend_reference *ref, zval *value, zval *result)
{
	return zend_jit_assign_to_typed_ref2_helper(ref, value, result, IS_TMP_VAR);
}

static zval* ZEND_FASTCALL zend_jit_assign_var_to_typed_ref2(zend_reference *ref, zval *value, zval *result)
{
	return zend_jit_assign_to_typed_ref2_helper(ref, value, result, IS_VAR);
}

static zval* ZEND_FASTCALL zend_jit_assign_cv_to_typed_ref2(zend_reference *ref, zval *value, zval *result)
{
	if (UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
		const zend_op *opline = EG(current_execute_data)->opline;
		uint32_t var;
		if (opline->opcode == ZEND_ASSIGN) {
			var = opline->op2.var;
		} else {
			ZEND_ASSERT((opline + 1)->opcode == ZEND_OP_DATA);
			var = (opline + 1)->op1.var;
		}
		zend_jit_undefined_op_helper(var);
		value = &EG(uninitialized_zval);
	}
	return zend_jit_assign_to_typed_ref2_helper(ref, value, result, IS_CV);
}

static zend_property_info *zend_jit_get_prop_not_accepting_double(zend_reference *ref)
{
	zend_property_info *prop;
	ZEND_REF_FOREACH_TYPE_SOURCES(ref, prop) {
		if (!(ZEND_TYPE_FULL_MASK(prop->type) & MAY_BE_DOUBLE)) {
			return prop;
		}
	} ZEND_REF_FOREACH_TYPE_SOURCES_END();
	return NULL;
}

static ZEND_COLD void zend_jit_throw_inc_ref_error(zend_reference *ref, zend_property_info *error_prop)
{
	zend_string *type_str = zend_type_to_string(error_prop->type);

	zend_type_error(
		"Cannot increment a reference held by property %s::$%s of type %s past its maximal value",
		ZSTR_VAL(error_prop->ce->name),
		zend_get_unmangled_property_name(error_prop->name),
		ZSTR_VAL(type_str));
	zend_string_release(type_str);
}

static ZEND_COLD void zend_jit_throw_dec_ref_error(zend_reference *ref, zend_property_info *error_prop)
{
	zend_string *type_str = zend_type_to_string(error_prop->type);

	zend_type_error(
		"Cannot decrement a reference held by property %s::$%s of type %s past its minimal value",
		ZSTR_VAL(error_prop->ce->name),
		zend_get_unmangled_property_name(error_prop->name),
		ZSTR_VAL(type_str));
	zend_string_release(type_str);
}

static void ZEND_FASTCALL zend_jit_pre_inc_typed_ref(zend_reference *ref, zval *ret)
{
	zval *var_ptr = &ref->val;
	zval tmp;

	ZVAL_COPY(&tmp, var_ptr);

	increment_function(var_ptr);

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_DOUBLE) && Z_TYPE(tmp) == IS_LONG) {
		zend_property_info *error_prop = zend_jit_get_prop_not_accepting_double(ref);
		if (UNEXPECTED(error_prop)) {
			zend_jit_throw_inc_ref_error(ref, error_prop);
			ZVAL_LONG(var_ptr, ZEND_LONG_MAX);
		}
	} else if (UNEXPECTED(!zend_verify_ref_assignable_zval(ref, var_ptr, ZEND_CALL_USES_STRICT_TYPES(EG(current_execute_data))))) {
		zval_ptr_dtor(var_ptr);
		ZVAL_COPY_VALUE(var_ptr, &tmp);
	} else {
		zval_ptr_dtor(&tmp);
	}
	if (ret) {
		ZVAL_COPY(ret, var_ptr);
	}
}

static void ZEND_FASTCALL zend_jit_pre_dec_typed_ref(zend_reference *ref, zval *ret)
{
	zval *var_ptr = &ref->val;
	zval tmp;

	ZVAL_COPY(&tmp, var_ptr);

	decrement_function(var_ptr);

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_DOUBLE) && Z_TYPE(tmp) == IS_LONG) {
		zend_property_info *error_prop = zend_jit_get_prop_not_accepting_double(ref);
		if (UNEXPECTED(error_prop)) {
			zend_jit_throw_dec_ref_error(ref, error_prop);
			ZVAL_LONG(var_ptr, ZEND_LONG_MIN);
		}
	} else if (UNEXPECTED(!zend_verify_ref_assignable_zval(ref, var_ptr, ZEND_CALL_USES_STRICT_TYPES(EG(current_execute_data))))) {
		zval_ptr_dtor(var_ptr);
		ZVAL_COPY_VALUE(var_ptr, &tmp);
	} else {
		zval_ptr_dtor(&tmp);
	}
	if (ret) {
		ZVAL_COPY(ret, var_ptr);
	}
}

static void ZEND_FASTCALL zend_jit_post_inc_typed_ref(zend_reference *ref, zval *ret)
{
	zval *var_ptr = &ref->val;
	ZVAL_COPY(ret, var_ptr);

	increment_function(var_ptr);

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_DOUBLE) && Z_TYPE_P(ret) == IS_LONG) {
		zend_property_info *error_prop = zend_jit_get_prop_not_accepting_double(ref);
		if (UNEXPECTED(error_prop)) {
			zend_jit_throw_inc_ref_error(ref, error_prop);
			ZVAL_LONG(var_ptr, ZEND_LONG_MAX);
		}
	} else if (UNEXPECTED(!zend_verify_ref_assignable_zval(ref, var_ptr, ZEND_CALL_USES_STRICT_TYPES(EG(current_execute_data))))) {
		zval_ptr_dtor(var_ptr);
		ZVAL_COPY_VALUE(var_ptr, ret);
	}
}

static void ZEND_FASTCALL zend_jit_post_dec_typed_ref(zend_reference *ref, zval *ret)
{
	zval *var_ptr = &ref->val;
	ZVAL_COPY(ret, var_ptr);

	decrement_function(var_ptr);

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_DOUBLE) && Z_TYPE_P(ret) == IS_LONG) {
		zend_property_info *error_prop = zend_jit_get_prop_not_accepting_double(ref);
		if (UNEXPECTED(error_prop)) {
			zend_jit_throw_dec_ref_error(ref, error_prop);
			ZVAL_LONG(var_ptr, ZEND_LONG_MIN);
		}
	} else if (UNEXPECTED(!zend_verify_ref_assignable_zval(ref, var_ptr, ZEND_CALL_USES_STRICT_TYPES(EG(current_execute_data))))) {
		zval_ptr_dtor(var_ptr);
		ZVAL_COPY_VALUE(var_ptr, ret);
	}
}

static void ZEND_FASTCALL zend_jit_assign_op_to_typed_ref(zend_reference *ref, zval *val, binary_op_type binary_op)
{
	zval z_copy;

	/* Make sure that in-place concatenation is used if the LHS is a string. */
	if (binary_op == concat_function && Z_TYPE(ref->val) == IS_STRING) {
		concat_function(&ref->val, &ref->val, val);
		ZEND_ASSERT(Z_TYPE(ref->val) == IS_STRING && "Concat should return string");
		return;
	}

	binary_op(&z_copy, &ref->val, val);
	if (EXPECTED(zend_verify_ref_assignable_zval(ref, &z_copy, ZEND_CALL_USES_STRICT_TYPES(EG(current_execute_data))))) {
		zval_ptr_dtor(&ref->val);
		ZVAL_COPY_VALUE(&ref->val, &z_copy);
	} else {
		zval_ptr_dtor(&z_copy);
	}
}

static void ZEND_FASTCALL zend_jit_assign_op_to_typed_ref_tmp(zend_reference *ref, zval *val, binary_op_type binary_op)
{
	zval z_copy;

	binary_op(&z_copy, &ref->val, val);
	if (EXPECTED(zend_verify_ref_assignable_zval(ref, &z_copy, ZEND_CALL_USES_STRICT_TYPES(EG(current_execute_data))))) {
		zval_ptr_dtor(&ref->val);
		ZVAL_COPY_VALUE(&ref->val, &z_copy);
	} else {
		zval_ptr_dtor(&z_copy);
	}
	zval_ptr_dtor_nogc(val);
}

static void ZEND_FASTCALL zend_jit_only_vars_by_reference(zval *arg)
{
	ZVAL_NEW_REF(arg, arg);
	zend_error(E_NOTICE, "Only variables should be passed by reference");
}

static void ZEND_FASTCALL zend_jit_invalid_array_access(zval *container)
{
	zend_error(E_WARNING, "Trying to access array offset on %s", zend_zval_value_name(container));
}

static void ZEND_FASTCALL zend_jit_invalid_property_read(zval *container, const char *property_name)
{
	zend_error(E_WARNING, "Attempt to read property \"%s\" on %s", property_name, zend_zval_value_name(container));
}

static void ZEND_FASTCALL zend_jit_invalid_property_write(zval *container, const char *property_name)
{
	zend_throw_error(NULL,
		"Attempt to modify property \"%s\" on %s",
		property_name, zend_zval_value_name(container));
}

static void ZEND_FASTCALL zend_jit_invalid_property_incdec(zval *container, const char *property_name)
{
	zend_execute_data *execute_data = EG(current_execute_data);
	const zend_op *opline = EX(opline);

	if (Z_TYPE_P(container) == IS_UNDEF && opline->op1_type == IS_CV) {
		zend_string *cv = EX(func)->op_array.vars[EX_VAR_TO_NUM(opline->op1.var)];

		zend_error_unchecked(E_WARNING, "Undefined variable $%S", cv);
	}
	if (opline->result_type & (IS_VAR|IS_TMP_VAR)) {
		ZVAL_UNDEF(EX_VAR(opline->result.var));
	}
	zend_throw_error(NULL,
		"Attempt to increment/decrement property \"%s\" on %s",
		property_name, zend_zval_value_name(container));
	if (opline->op1_type == IS_VAR) {
		zval_ptr_dtor_nogc(EX_VAR(opline->op1.var));
	}
}

static void ZEND_FASTCALL zend_jit_invalid_property_assign(zval *container, const char *property_name)
{
	zend_throw_error(NULL,
		"Attempt to assign property \"%s\" on %s",
		property_name, zend_zval_value_name(container));
}

static void ZEND_FASTCALL zend_jit_invalid_property_assign_op(zval *container, const char *property_name)
{
	if (Z_TYPE_P(container) == IS_UNDEF) {
		const zend_execute_data *execute_data = EG(current_execute_data);

		zend_jit_undefined_op_helper(EX(opline)->op1.var);
	}
	zend_jit_invalid_property_assign(container, property_name);
}

static zval * ZEND_FASTCALL zend_jit_prepare_assign_dim_ref(zval *ref) {
	zval *val = Z_REFVAL_P(ref);
	if (Z_TYPE_P(val) <= IS_FALSE) {
		if (ZEND_REF_HAS_TYPE_SOURCES(Z_REF_P(ref))
				&& !zend_verify_ref_array_assignable(Z_REF_P(ref))) {
			return NULL;
		}
		if (Z_TYPE_P(val) == IS_FALSE) {
			ZVAL_ARR(val, zend_new_array(8));
			zend_false_to_array_deprecated();
			if (EG(exception)) {
				return NULL;
			}
		} else {
			ZVAL_ARR(val, zend_new_array(8));
		}
	}
	return val;
}

static void ZEND_FASTCALL zend_jit_pre_inc(zval *var_ptr, zval *ret)
{
	increment_function(var_ptr);
	ZVAL_COPY(ret, var_ptr);
}

static void ZEND_FASTCALL zend_jit_pre_dec(zval *var_ptr, zval *ret)
{
	decrement_function(var_ptr);
	ZVAL_COPY(ret, var_ptr);
}

#define HT_POISONED_PTR ((HashTable *) (intptr_t) -1)

static zend_never_inline void ZEND_FASTCALL _zend_hash_iterators_remove(HashTable *ht)
{
	HashTableIterator *iter = EG(ht_iterators);
	HashTableIterator *end  = iter + EG(ht_iterators_used);

	while (iter != end) {
		if (iter->ht == ht) {
			iter->ht = HT_POISONED_PTR;
		}
		iter++;
	}
}

static void ZEND_FASTCALL zend_jit_array_free(HashTable *ht)
{
	GC_REMOVE_FROM_BUFFER(ht);
	if (UNEXPECTED(HT_HAS_ITERATORS(ht))) {
		_zend_hash_iterators_remove(ht);
	}
	if (!(EXPECTED(HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED))) {
		efree(HT_GET_DATA_ADDR(ht));
	}
	FREE_HASHTABLE(ht);
}

static HashTable *ZEND_FASTCALL zend_jit_zval_array_dup(zval *arr)
{
	HashTable *ht;

	Z_TRY_DELREF_P(arr);
	ht = Z_ARRVAL_P(arr);
	ht = zend_array_dup(ht);
	ZVAL_ARR(arr, ht);
	return ht;
}

static zend_array *ZEND_FASTCALL zend_jit_add_arrays_helper(zend_array *op1, zend_array *op2)
{
	zend_array *res;
	res = zend_array_dup(op1);
	zend_hash_merge(res, op2, zval_add_ref, 0);
	return res;
}

static void ZEND_FASTCALL zend_jit_assign_obj_helper(zend_object *zobj, zend_string *name, zval *value, void **cache_slot, zval *result)
{
	if (UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
		const zend_op *op_data = EG(current_execute_data)->opline + 1;
		ZEND_ASSERT(op_data->opcode == ZEND_OP_DATA && op_data->op1_type == IS_CV);
		zend_jit_undefined_op_helper(op_data->op1.var);
		value = &EG(uninitialized_zval);
	}

	ZVAL_DEREF(value);
	value = zobj->handlers->write_property(zobj, name, value, cache_slot);
	if (result && value) {
		ZVAL_COPY_DEREF(result, value);
	}
}

static void ZEND_FASTCALL zend_jit_assign_to_typed_prop(zval *property_val, zend_property_info *info, zval *value, zval *result)
{
	zend_execute_data *execute_data = EG(current_execute_data);
	zend_refcounted *garbage = NULL;
	zval tmp;

	if (UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
		const zend_op *op_data = execute_data->opline + 1;
		ZEND_ASSERT(op_data->opcode == ZEND_OP_DATA && op_data->op1_type == IS_CV);
		zend_jit_undefined_op_helper(op_data->op1.var);
		value = &EG(uninitialized_zval);
	}

	if (UNEXPECTED((info->flags & ZEND_ACC_READONLY) && !(Z_PROP_FLAG_P(property_val) & IS_PROP_REINITABLE))) {
		zend_readonly_property_modification_error(info);
		if (result) {
			ZVAL_UNDEF(result);
		}
		return;
	}

	ZVAL_DEREF(value);
	ZVAL_COPY(&tmp, value);

	if (UNEXPECTED(!zend_verify_property_type(info, &tmp, EX_USES_STRICT_TYPES()))) {
		zval_ptr_dtor(&tmp);
		if (result) {
			ZVAL_NULL(result);
		}
		return;
	}

	Z_PROP_FLAG_P(property_val) &= ~IS_PROP_REINITABLE;

	value = zend_assign_to_variable_ex(property_val, &tmp, IS_TMP_VAR, EX_USES_STRICT_TYPES(), &garbage);
	if (result) {
		ZVAL_COPY_DEREF(result, value);
	}
	if (garbage) {
		GC_DTOR(garbage);
	}
}

static zend_never_inline void _zend_jit_assign_op_overloaded_property(zend_object *object, zend_string *name, void **cache_slot, zval *value, binary_op_type binary_op)
{
	zval *z;
	zval rv, res;

	GC_ADDREF(object);
	z = object->handlers->read_property(object, name, BP_VAR_R, cache_slot, &rv);
	if (UNEXPECTED(EG(exception))) {
		OBJ_RELEASE(object);
//???		if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
//???			ZVAL_UNDEF(EX_VAR(opline->result.var));
//???		}
		return;
	}
	if (binary_op(&res, z, value) == SUCCESS) {
		object->handlers->write_property(object, name, &res, cache_slot);
	}
//???	if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
//???		ZVAL_COPY(EX_VAR(opline->result.var), &res);
//???	}
	if (z == &rv) {
		zval_ptr_dtor(z);
	}
	zval_ptr_dtor(&res);
	OBJ_RELEASE(object);
}

static void ZEND_FASTCALL zend_jit_assign_op_to_typed_prop(zval *zptr, zend_property_info *prop_info, zval *value, binary_op_type binary_op)
{
	zend_execute_data *execute_data = EG(current_execute_data);
	zval z_copy;

	if (UNEXPECTED((prop_info->flags & ZEND_ACC_READONLY) && !(Z_PROP_FLAG_P(zptr) & IS_PROP_REINITABLE))) {
		zend_readonly_property_modification_error(prop_info);
		return;
	}

	ZVAL_DEREF(zptr);
	/* Make sure that in-place concatenation is used if the LHS is a string. */
	if (binary_op == concat_function && Z_TYPE_P(zptr) == IS_STRING) {
		concat_function(zptr, zptr, value);
		ZEND_ASSERT(Z_TYPE_P(zptr) == IS_STRING && "Concat should return string");
		return;
	}

	binary_op(&z_copy, zptr, value);
	if (EXPECTED(zend_verify_property_type(prop_info, &z_copy, EX_USES_STRICT_TYPES()))) {
		Z_PROP_FLAG_P(zptr) &= ~IS_PROP_REINITABLE;
		zval_ptr_dtor(zptr);
		ZVAL_COPY_VALUE(zptr, &z_copy);
	} else {
		zval_ptr_dtor(&z_copy);
	}
}

static void ZEND_FASTCALL zend_jit_assign_obj_op_helper(zend_object *zobj, zend_string *name, zval *value, void **cache_slot, binary_op_type binary_op)
{
	zval *zptr;
	zend_property_info *prop_info;

	if (EXPECTED((zptr = zobj->handlers->get_property_ptr_ptr(zobj, name, BP_VAR_RW, cache_slot)) != NULL)) {
		if (UNEXPECTED(Z_ISERROR_P(zptr))) {
//???			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
//???				ZVAL_NULL(EX_VAR(opline->result.var));
//???			}
		} else {
//???			zval *orig_zptr = zptr;
			zend_reference *ref;

			do {
				if (UNEXPECTED(Z_ISREF_P(zptr))) {
					ref = Z_REF_P(zptr);
					zptr = Z_REFVAL_P(zptr);
					if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) {
						zend_jit_assign_op_to_typed_ref(ref, value, binary_op);
						break;
					}
				}

//???				if (OP2_TYPE == IS_CONST) {
				prop_info = (zend_property_info*)CACHED_PTR_EX(cache_slot + 2);
//???				} else {
//???					prop_info = zend_object_fetch_property_type_info(Z_OBJ_P(object), orig_zptr);
//???				}
				if (UNEXPECTED(prop_info)) {
					/* special case for typed properties */
					zend_jit_assign_op_to_typed_prop(zptr, prop_info, value, binary_op);
				} else {
					binary_op(zptr, zptr, value);
				}
			} while (0);

//???			if (UNEXPECTED(RETURN_VALUE_USED(opline))) {
//???				ZVAL_COPY(EX_VAR(opline->result.var), zptr);
//???			}
		}
	} else {
		_zend_jit_assign_op_overloaded_property(zobj, name, cache_slot, value, binary_op);
	}
}

static ZEND_COLD zend_long _zend_jit_throw_inc_prop_error(zend_property_info *prop)
{
	zend_string *type_str = zend_type_to_string(prop->type);
	zend_type_error("Cannot increment property %s::$%s of type %s past its maximal value",
		ZSTR_VAL(prop->ce->name),
		zend_get_unmangled_property_name(prop->name),
		ZSTR_VAL(type_str));
	zend_string_release(type_str);
	return ZEND_LONG_MAX;
}

static ZEND_COLD zend_long _zend_jit_throw_dec_prop_error(zend_property_info *prop)
{
	zend_string *type_str = zend_type_to_string(prop->type);
	zend_type_error("Cannot decrement property %s::$%s of type %s past its minimal value",
		ZSTR_VAL(prop->ce->name),
		zend_get_unmangled_property_name(prop->name),
		ZSTR_VAL(type_str));
	zend_string_release(type_str);
	return ZEND_LONG_MIN;
}

static void ZEND_FASTCALL zend_jit_inc_typed_prop(zval *var_ptr, zend_property_info *prop_info)
{
	ZEND_ASSERT(Z_TYPE_P(var_ptr) != IS_UNDEF);

	if (UNEXPECTED((prop_info->flags & ZEND_ACC_READONLY) && !(Z_PROP_FLAG_P(var_ptr) & IS_PROP_REINITABLE))) {
		zend_readonly_property_modification_error(prop_info);
		return;
	}

	zend_execute_data *execute_data = EG(current_execute_data);
	zval tmp;

	ZVAL_DEREF(var_ptr);
	ZVAL_COPY(&tmp, var_ptr);

	increment_function(var_ptr);

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_DOUBLE) && Z_TYPE(tmp) == IS_LONG) {
		if (!(ZEND_TYPE_FULL_MASK(prop_info->type) & MAY_BE_DOUBLE)) {
			zend_long val = _zend_jit_throw_inc_prop_error(prop_info);
			ZVAL_LONG(var_ptr, val);
		} else {
			Z_PROP_FLAG_P(var_ptr) &= ~IS_PROP_REINITABLE;
		}
	} else if (UNEXPECTED(!zend_verify_property_type(prop_info, var_ptr, EX_USES_STRICT_TYPES()))) {
		zval_ptr_dtor(var_ptr);
		ZVAL_COPY_VALUE(var_ptr, &tmp);
	} else {
		Z_PROP_FLAG_P(var_ptr) &= ~IS_PROP_REINITABLE;
		zval_ptr_dtor(&tmp);
	}
}

static void ZEND_FASTCALL zend_jit_dec_typed_prop(zval *var_ptr, zend_property_info *prop_info)
{
	ZEND_ASSERT(Z_TYPE_P(var_ptr) != IS_UNDEF);

	if (UNEXPECTED((prop_info->flags & ZEND_ACC_READONLY) && !(Z_PROP_FLAG_P(var_ptr) & IS_PROP_REINITABLE))) {
		zend_readonly_property_modification_error(prop_info);
		return;
	}

	zend_execute_data *execute_data = EG(current_execute_data);
	zval tmp;

	ZVAL_DEREF(var_ptr);
	ZVAL_COPY(&tmp, var_ptr);

	decrement_function(var_ptr);

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_DOUBLE) && Z_TYPE(tmp) == IS_LONG) {
		if (!(ZEND_TYPE_FULL_MASK(prop_info->type) & MAY_BE_DOUBLE)) {
			zend_long val = _zend_jit_throw_dec_prop_error(prop_info);
			ZVAL_LONG(var_ptr, val);
		} else {
			Z_PROP_FLAG_P(var_ptr) &= ~IS_PROP_REINITABLE;
		}
	} else if (UNEXPECTED(!zend_verify_property_type(prop_info, var_ptr, EX_USES_STRICT_TYPES()))) {
		zval_ptr_dtor(var_ptr);
		ZVAL_COPY_VALUE(var_ptr, &tmp);
	} else {
		Z_PROP_FLAG_P(var_ptr) &= ~IS_PROP_REINITABLE;
		zval_ptr_dtor(&tmp);
	}
}

static void ZEND_FASTCALL zend_jit_pre_inc_typed_prop(zval *var_ptr, zend_property_info *prop_info, zval *result)
{
	ZVAL_DEREF(var_ptr);
	zend_jit_inc_typed_prop(var_ptr, prop_info);
	ZVAL_COPY(result, var_ptr);
}

static void ZEND_FASTCALL zend_jit_pre_dec_typed_prop(zval *var_ptr, zend_property_info *prop_info, zval *result)
{
	ZVAL_DEREF(var_ptr);
	zend_jit_dec_typed_prop(var_ptr, prop_info);
	ZVAL_COPY(result, var_ptr);
}

static void ZEND_FASTCALL zend_jit_post_inc_typed_prop(zval *var_ptr, zend_property_info *prop_info, zval *result)
{
	ZEND_ASSERT(Z_TYPE_P(var_ptr) != IS_UNDEF);

	if (UNEXPECTED((prop_info->flags & ZEND_ACC_READONLY) && !(Z_PROP_FLAG_P(var_ptr) & IS_PROP_REINITABLE))) {
		zend_readonly_property_modification_error(prop_info);
		if (result) {
			ZVAL_UNDEF(result);
		}
		return;
	}

	zend_execute_data *execute_data = EG(current_execute_data);

	ZVAL_DEREF(var_ptr);
	ZVAL_COPY(result, var_ptr);

	increment_function(var_ptr);

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_DOUBLE) && Z_TYPE_P(result) == IS_LONG) {
		if (!(ZEND_TYPE_FULL_MASK(prop_info->type) & MAY_BE_DOUBLE)) {
			zend_long val = _zend_jit_throw_inc_prop_error(prop_info);
			ZVAL_LONG(var_ptr, val);
		} else {
			Z_PROP_FLAG_P(var_ptr) &= ~IS_PROP_REINITABLE;
		}
	} else if (UNEXPECTED(!zend_verify_property_type(prop_info, var_ptr, EX_USES_STRICT_TYPES()))) {
		zval_ptr_dtor(var_ptr);
		ZVAL_COPY_VALUE(var_ptr, result);
		ZVAL_UNDEF(result);
	} else {
		Z_PROP_FLAG_P(var_ptr) &= ~IS_PROP_REINITABLE;
	}
}

static void ZEND_FASTCALL zend_jit_post_dec_typed_prop(zval *var_ptr, zend_property_info *prop_info, zval *result)
{
	ZEND_ASSERT(Z_TYPE_P(var_ptr) != IS_UNDEF);

	if (UNEXPECTED((prop_info->flags & ZEND_ACC_READONLY) && !(Z_PROP_FLAG_P(var_ptr) & IS_PROP_REINITABLE))) {
		zend_readonly_property_modification_error(prop_info);
		if (result) {
			ZVAL_UNDEF(result);
		}
		return;
	}

	zend_execute_data *execute_data = EG(current_execute_data);

	ZVAL_DEREF(var_ptr);
	ZVAL_COPY(result, var_ptr);

	decrement_function(var_ptr);

	if (UNEXPECTED(Z_TYPE_P(var_ptr) == IS_DOUBLE) && Z_TYPE_P(result) == IS_LONG) {
		if (!(ZEND_TYPE_FULL_MASK(prop_info->type) & MAY_BE_DOUBLE)) {
			zend_long val = _zend_jit_throw_dec_prop_error(prop_info);
			ZVAL_LONG(var_ptr, val);
		} else {
			Z_PROP_FLAG_P(var_ptr) &= ~IS_PROP_REINITABLE;
		}
	} else if (UNEXPECTED(!zend_verify_property_type(prop_info, var_ptr, EX_USES_STRICT_TYPES()))) {
		zval_ptr_dtor(var_ptr);
		ZVAL_COPY_VALUE(var_ptr, result);
		ZVAL_UNDEF(result);
	} else {
		Z_PROP_FLAG_P(var_ptr) &= ~IS_PROP_REINITABLE;
	}
}

static void ZEND_FASTCALL zend_jit_pre_inc_obj_helper(zend_object *zobj, zend_string *name, void **cache_slot, zval *result)
{
	zval *prop;

	if (EXPECTED((prop = zobj->handlers->get_property_ptr_ptr(zobj, name, BP_VAR_RW, cache_slot)) != NULL)) {
		if (UNEXPECTED(Z_ISERROR_P(prop))) {
			if (UNEXPECTED(result)) {
				ZVAL_NULL(result);
			}
		} else {
			zend_property_info *prop_info = (zend_property_info *) CACHED_PTR_EX(cache_slot + 2);

			if (EXPECTED(Z_TYPE_P(prop) == IS_LONG)) {
				fast_long_increment_function(prop);
				if (UNEXPECTED(Z_TYPE_P(prop) != IS_LONG) && UNEXPECTED(prop_info)
						&& !(ZEND_TYPE_FULL_MASK(prop_info->type) & MAY_BE_DOUBLE)) {
					zend_long val = _zend_jit_throw_inc_prop_error(prop_info);
					ZVAL_LONG(prop, val);
				}
			} else {
				do {
					if (Z_ISREF_P(prop)) {
						zend_reference *ref = Z_REF_P(prop);
						prop = Z_REFVAL_P(prop);
						if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) {
							zend_jit_pre_inc_typed_ref(ref, result);
							break;
						}
					}

					if (UNEXPECTED(prop_info)) {
						zend_jit_inc_typed_prop(prop, prop_info);
					} else {
						increment_function(prop);
					}
				} while (0);
			}
			if (UNEXPECTED(result)) {
				ZVAL_COPY(result, prop);
			}
		}
	} else {
		zval rv;
		zval *z;
		zval z_copy;

		GC_ADDREF(zobj);
		z = zobj->handlers->read_property(zobj, name, BP_VAR_R, cache_slot, &rv);
		if (UNEXPECTED(EG(exception))) {
			OBJ_RELEASE(zobj);
			if (UNEXPECTED(result)) {
				ZVAL_NULL(result);
			}
			return;
		}

		ZVAL_COPY_DEREF(&z_copy, z);
		increment_function(&z_copy);
		if (UNEXPECTED(result)) {
			ZVAL_COPY(result, &z_copy);
		}
		zobj->handlers->write_property(zobj, name, &z_copy, cache_slot);
		OBJ_RELEASE(zobj);
		zval_ptr_dtor(&z_copy);
		if (z == &rv) {
			zval_ptr_dtor(z);
		}
	}
}

static void ZEND_FASTCALL zend_jit_pre_dec_obj_helper(zend_object *zobj, zend_string *name, void **cache_slot, zval *result)
{
	zval *prop;

	if (EXPECTED((prop = zobj->handlers->get_property_ptr_ptr(zobj, name, BP_VAR_RW, cache_slot)) != NULL)) {
		if (UNEXPECTED(Z_ISERROR_P(prop))) {
			if (UNEXPECTED(result)) {
				ZVAL_NULL(result);
			}
		} else {
			zend_property_info *prop_info = (zend_property_info *) CACHED_PTR_EX(cache_slot + 2);

			if (EXPECTED(Z_TYPE_P(prop) == IS_LONG)) {
				fast_long_decrement_function(prop);
				if (UNEXPECTED(Z_TYPE_P(prop) != IS_LONG) && UNEXPECTED(prop_info)
						&& !(ZEND_TYPE_FULL_MASK(prop_info->type) & MAY_BE_DOUBLE)) {
					zend_long val = _zend_jit_throw_dec_prop_error(prop_info);
					ZVAL_LONG(prop, val);
				}
			} else {
				do {
					if (Z_ISREF_P(prop)) {
						zend_reference *ref = Z_REF_P(prop);
						prop = Z_REFVAL_P(prop);
						if (UNEXPECTED(ZEND_REF_HAS_TYPE_SOURCES(ref))) {
							zend_jit_pre_dec_typed_ref(ref, result);
							break;
						}
					}

					if (UNEXPECTED(prop_info)) {
						zend_jit_dec_typed_prop(prop, prop_info);
					} else {
						decrement_function(prop);
					}
				} while (0);
			}
			if (UNEXPECTED(result)) {
				ZVAL_COPY(result, prop);
			}
		}
	} else {
		zval rv;
		zval *z;
		zval z_copy;

		GC_ADDREF(zobj);
		z = zobj->handlers->read_property(zobj, name, BP_VAR_R, cache_slot, &rv);
		if (UNEXPECTED(EG(exception))) {
			OBJ_RELEASE(zobj);
			if (UNEXPECTED(result)) {
				ZVAL_NULL(result);
			}
			return;
		}

		ZVAL_COPY_DEREF(&z_copy, z);
		decrement_function(&z_copy);
		if (UNEXPECTED(result)) {
			ZVAL_COPY(result, &z_copy);
		}
		zobj->handlers->write_property(zobj, name, &z_copy, cache_slot);
		OBJ_RELEASE(zobj);
		zval_ptr_dtor(&z_copy);
		if (z == &rv) {
			zval_ptr_dtor(z);
		}
	}
}

static void ZEND_FASTCALL zend_jit_post_inc_obj_helper(zend_object *zobj, zend_string *name, void **cache_slot, zval *result)
{
	zval *prop;

	if (EXPECTED((prop = zobj->handlers->get_property_ptr_ptr(zobj, name, BP_VAR_RW, cache_slot)) != NULL)) {
		if (UNEXPECTED(Z_ISERROR_P(prop))) {
			ZVAL_NULL(result);
		} else {
			zend_property_info *prop_info = (zend_property_info*)CACHED_PTR_EX(cache_slot + 2);

			if (EXPECTED(Z_TYPE_P(prop) == IS_LONG)) {
				ZVAL_LONG(result, Z_LVAL_P(prop));
				fast_long_increment_function(prop);
				if (UNEXPECTED(Z_TYPE_P(prop) != IS_LONG) && UNEXPECTED(prop_info)
						&& !(ZEND_TYPE_FULL_MASK(prop_info->type) & MAY_BE_DOUBLE)) {
					zend_long val = _zend_jit_throw_inc_prop_error(prop_info);
					ZVAL_LONG(prop, val);
				}
			} else {
				if (Z_ISREF_P(prop)) {
					zend_reference *ref = Z_REF_P(prop);
					prop = Z_REFVAL_P(prop);
					if (ZEND_REF_HAS_TYPE_SOURCES(ref)) {
						zend_jit_post_inc_typed_ref(ref, result);
						return;
					}
				}

				if (UNEXPECTED(prop_info)) {
					zend_jit_post_inc_typed_prop(prop, prop_info, result);
				} else {
					ZVAL_COPY(result, prop);
					increment_function(prop);
				}
			}
		}
	} else {
		zval rv;
		zval *z;
		zval z_copy;

		GC_ADDREF(zobj);
		z = zobj->handlers->read_property(zobj, name, BP_VAR_R, cache_slot, &rv);
		if (UNEXPECTED(EG(exception))) {
			OBJ_RELEASE(zobj);
			ZVAL_UNDEF(result);
			return;
		}

		ZVAL_COPY_DEREF(&z_copy, z);
		ZVAL_COPY(result, &z_copy);
		increment_function(&z_copy);
		zobj->handlers->write_property(zobj, name, &z_copy, cache_slot);
		OBJ_RELEASE(zobj);
		zval_ptr_dtor(&z_copy);
		if (z == &rv) {
			zval_ptr_dtor(z);
		}
	}
}

static void ZEND_FASTCALL zend_jit_post_dec_obj_helper(zend_object *zobj, zend_string *name, void **cache_slot, zval *result)
{
	zval *prop;

	if (EXPECTED((prop = zobj->handlers->get_property_ptr_ptr(zobj, name, BP_VAR_RW, cache_slot)) != NULL)) {
		if (UNEXPECTED(Z_ISERROR_P(prop))) {
			ZVAL_NULL(result);
		} else {
			zend_property_info *prop_info = (zend_property_info*)CACHED_PTR_EX(cache_slot + 2);

			if (EXPECTED(Z_TYPE_P(prop) == IS_LONG)) {
				ZVAL_LONG(result, Z_LVAL_P(prop));
				fast_long_decrement_function(prop);
				if (UNEXPECTED(Z_TYPE_P(prop) != IS_LONG) && UNEXPECTED(prop_info)
						&& !(ZEND_TYPE_FULL_MASK(prop_info->type) & MAY_BE_DOUBLE)) {
					zend_long val = _zend_jit_throw_dec_prop_error(prop_info);
					ZVAL_LONG(prop, val);
				}
			} else {
				if (Z_ISREF_P(prop)) {
					zend_reference *ref = Z_REF_P(prop);
					prop = Z_REFVAL_P(prop);
					if (ZEND_REF_HAS_TYPE_SOURCES(ref)) {
						zend_jit_post_dec_typed_ref(ref, result);
						return;
					}
				}

				if (UNEXPECTED(prop_info)) {
					zend_jit_post_dec_typed_prop(prop, prop_info, result);
				} else {
					ZVAL_COPY(result, prop);
					decrement_function(prop);
				}
			}
		}
	} else {
		zval rv;
		zval *z;
		zval z_copy;

		GC_ADDREF(zobj);
		z = zobj->handlers->read_property(zobj, name, BP_VAR_R, cache_slot, &rv);
		if (UNEXPECTED(EG(exception))) {
			OBJ_RELEASE(zobj);
			ZVAL_UNDEF(result);
			return;
		}

		ZVAL_COPY_DEREF(&z_copy, z);
		ZVAL_COPY(result, &z_copy);
		decrement_function(&z_copy);
		zobj->handlers->write_property(zobj, name, &z_copy, cache_slot);
		OBJ_RELEASE(zobj);
		zval_ptr_dtor(&z_copy);
		if (z == &rv) {
			zval_ptr_dtor(z);
		}
	}
}

static void ZEND_FASTCALL zend_jit_free_trampoline_helper(zend_function *func)
{
	ZEND_ASSERT(func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE);
	zend_string_release_ex(func->common.function_name, 0);
	zend_free_trampoline(func);
}

static void ZEND_FASTCALL zend_jit_exception_in_interrupt_handler_helper(void)
{
	if (EG(exception)) {
		/* We have to UNDEF result, because ZEND_HANDLE_EXCEPTION is going to free it */
		const zend_op *throw_op = EG(opline_before_exception);

		if (throw_op
		 && throw_op->result_type & (IS_TMP_VAR|IS_VAR)
		 && throw_op->opcode != ZEND_ADD_ARRAY_ELEMENT
		 && throw_op->opcode != ZEND_ADD_ARRAY_UNPACK
		 && throw_op->opcode != ZEND_ROPE_INIT
		 && throw_op->opcode != ZEND_ROPE_ADD) {
			ZVAL_UNDEF(ZEND_CALL_VAR(EG(current_execute_data), throw_op->result.var));
		}
	}
}

static zend_string* ZEND_FASTCALL zend_jit_rope_end(zend_string **rope, uint32_t count)
{
	zend_string *ret;
	uint32_t i;
	size_t len = 0;

	uint32_t flags = ZSTR_COPYABLE_CONCAT_PROPERTIES;
	for (i = 0; i <= count; i++) {
		flags &= ZSTR_GET_COPYABLE_CONCAT_PROPERTIES(rope[i]);
		len += ZSTR_LEN(rope[i]);
	}
	ret = zend_string_alloc(len, 0);
	GC_ADD_FLAGS(ret, flags);

	char *target = ZSTR_VAL(ret);
	for (i = 0; i <= count; i++) {
		target = zend_mempcpy(target, ZSTR_VAL(rope[i]), ZSTR_LEN(rope[i]));
		zend_string_release_ex(rope[i], 0);
	}
	*target = '\0';
	return ret;
}
