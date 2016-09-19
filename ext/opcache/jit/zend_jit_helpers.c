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

static void ZEND_FASTCALL zend_jit_undefined_op_helper(uint32_t var)
{
	const zend_execute_data *execute_data = EG(current_execute_data);
	zend_string *cv = EX(func)->op_array.vars[EX_VAR_TO_NUM(var)];

	zend_error(E_NOTICE, "Undefined variable: %s", ZSTR_VAL(cv));
}

static void ZEND_FASTCALL zend_jit_fetch_dim_r_helper(zend_array *ht, zval *dim, zval *result)
{
	zend_long hval;
	zend_string *offset_key;
	zval *retval;

	if (Z_TYPE_P(dim) == IS_REFERENCE) {
		dim = Z_REFVAL_P(dim);
	}

	switch (Z_TYPE_P(dim)) {
		case IS_UNDEF:
			zend_jit_undefined_op_helper(EG(current_execute_data)->opline->op2.var);
			/* break missing intentionally */
		case IS_NULL:
			offset_key = ZSTR_EMPTY_ALLOC();
			goto str_index;
		case IS_DOUBLE:
			hval = zend_dval_to_lval(Z_DVAL_P(dim));
			goto num_index;
		case IS_RESOURCE:
			zend_error(E_NOTICE, "Resource ID#%d used as offset, casting to integer (%d)", Z_RES_HANDLE_P(dim), Z_RES_HANDLE_P(dim));
			hval = Z_RES_HANDLE_P(dim);
			goto num_index;
		case IS_FALSE:
			hval = 0;
			goto num_index;
		case IS_TRUE:
			hval = 1;
			goto num_index;
		default:
			zend_error(E_WARNING, "Illegal offset type");
			ZVAL_NULL(result);
			return;
	}

str_index:
	retval = zend_hash_find(ht, offset_key);
	if (retval) {
		/* support for $GLOBALS[...] */
		if (UNEXPECTED(Z_TYPE_P(retval) == IS_INDIRECT)) {
			retval = Z_INDIRECT_P(retval);
			if (UNEXPECTED(Z_TYPE_P(retval) == IS_UNDEF)) {
				zend_error(E_NOTICE, "Undefined index: %s", ZSTR_VAL(offset_key));
				ZVAL_NULL(result);
				return;
			}
		}
	} else {
		zend_error(E_NOTICE, "Undefined index: %s", ZSTR_VAL(offset_key));
		ZVAL_NULL(result);
		return;
	}
	ZVAL_COPY_UNREF(result, retval);
	return;

num_index:
	ZEND_HASH_INDEX_FIND(ht, hval, retval, num_undef);
	ZVAL_COPY_UNREF(result, retval);
	return;

num_undef:
	zend_error(E_NOTICE,"Undefined offset: " ZEND_LONG_FMT, hval);
	ZVAL_NULL(result);
}

static void ZEND_FASTCALL zend_jit_fetch_dim_is_helper(zend_array *ht, zval *dim, zval *result)
{
	zend_long hval;
	zend_string *offset_key;
	zval *retval;

	if (Z_TYPE_P(dim) == IS_REFERENCE) {
		dim = Z_REFVAL_P(dim);
	}

	switch (Z_TYPE_P(dim)) {
		case IS_UNDEF:
			zend_jit_undefined_op_helper(EG(current_execute_data)->opline->op2.var);
			/* break missing intentionally */
		case IS_NULL:
			offset_key = ZSTR_EMPTY_ALLOC();
			goto str_index;
		case IS_DOUBLE:
			hval = zend_dval_to_lval(Z_DVAL_P(dim));
			goto num_index;
		case IS_RESOURCE:
			zend_error(E_NOTICE, "Resource ID#%d used as offset, casting to integer (%d)", Z_RES_HANDLE_P(dim), Z_RES_HANDLE_P(dim));
			hval = Z_RES_HANDLE_P(dim);
			goto num_index;
		case IS_FALSE:
			hval = 0;
			goto num_index;
		case IS_TRUE:
			hval = 1;
			goto num_index;
		default:
			zend_error(E_WARNING, "Illegal offset type");
			ZVAL_NULL(result);
			return;
	}

str_index:
	retval = zend_hash_find(ht, offset_key);
	if (retval) {
		/* support for $GLOBALS[...] */
		if (UNEXPECTED(Z_TYPE_P(retval) == IS_INDIRECT)) {
			retval = Z_INDIRECT_P(retval);
			if (UNEXPECTED(Z_TYPE_P(retval) == IS_UNDEF)) {
				ZVAL_NULL(result);
				return;
			}
		}
	} else {
		ZVAL_NULL(result);
		return;
	}
	ZVAL_COPY_UNREF(result, retval);
	return;

num_index:
	ZEND_HASH_INDEX_FIND(ht, hval, retval, num_undef);
	ZVAL_COPY_UNREF(result, retval);
	return;

num_undef:
	ZVAL_NULL(result);
}

static void ZEND_FASTCALL zend_jit_fetch_dim_str_r_helper(zval *container, zval *dim, zval *result)
{
	zend_long offset;

try_string_offset:
	if (UNEXPECTED(Z_TYPE_P(dim) != IS_LONG)) {
		switch (Z_TYPE_P(dim)) {
			/* case IS_LONG: */
			case IS_STRING:
				if (IS_LONG == is_numeric_string(Z_STRVAL_P(dim), Z_STRLEN_P(dim), NULL, NULL, -1)) {
					break;
				}
				zend_error(E_WARNING, "Illegal string offset '%s'", Z_STRVAL_P(dim));
				break;
			case IS_UNDEF:
				zend_jit_undefined_op_helper(EG(current_execute_data)->opline->op2.var);
			case IS_DOUBLE:
			case IS_NULL:
			case IS_FALSE:
			case IS_TRUE:
				zend_error(E_NOTICE, "String offset cast occurred");
				break;
			case IS_REFERENCE:
				dim = Z_REFVAL_P(dim);
				goto try_string_offset;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				break;
		}

		offset = _zval_get_long_func(dim);
	} else {
		offset = Z_LVAL_P(dim);
	}

	if (UNEXPECTED(Z_STRLEN_P(container) < (size_t)((offset < 0) ? -offset : (offset + 1)))) {
		zend_error(E_NOTICE, "Uninitialized string offset: " ZEND_LONG_FMT, offset);
		ZVAL_EMPTY_STRING(result);
	} else {
		zend_uchar c;
		zend_long real_offset;

		real_offset = (UNEXPECTED(offset < 0)) /* Handle negative offset */
			? (zend_long)Z_STRLEN_P(container) + offset : offset;
		c = (zend_uchar)Z_STRVAL_P(container)[real_offset];
			if (CG(one_char_string)[c]) {
			ZVAL_INTERNED_STR(result, CG(one_char_string)[c]);
		} else {
			ZVAL_NEW_STR(result, zend_string_init(Z_STRVAL_P(container) + real_offset, 1, 0));
		}
	}
}

static void ZEND_FASTCALL zend_jit_fetch_dim_str_is_helper(zval *container, zval *dim, zval *result)
{
	zend_long offset;

try_string_offset:
	if (UNEXPECTED(Z_TYPE_P(dim) != IS_LONG)) {
		switch (Z_TYPE_P(dim)) {
			/* case IS_LONG: */
			case IS_STRING:
				if (IS_LONG == is_numeric_string(Z_STRVAL_P(dim), Z_STRLEN_P(dim), NULL, NULL, -1)) {
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
				zend_error(E_WARNING, "Illegal offset type");
				break;
		}

		offset = _zval_get_long_func(dim);
	} else {
		offset = Z_LVAL_P(dim);
	}

	if (UNEXPECTED(Z_STRLEN_P(container) < (size_t)((offset < 0) ? -offset : (offset + 1)))) {
		ZVAL_NULL(result);
	} else {
		zend_uchar c;
		zend_long real_offset;

		real_offset = (UNEXPECTED(offset < 0)) /* Handle negative offset */
			? (zend_long)Z_STRLEN_P(container) + offset : offset;
		c = (zend_uchar)Z_STRVAL_P(container)[real_offset];
			if (CG(one_char_string)[c]) {
			ZVAL_INTERNED_STR(result, CG(one_char_string)[c]);
		} else {
			ZVAL_NEW_STR(result, zend_string_init(Z_STRVAL_P(container) + real_offset, 1, 0));
		}
	}
}

static void ZEND_FASTCALL zend_jit_fetch_dim_obj_r_helper(zval *container, zval *dim, zval *result)
{
	if (UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
		zend_jit_undefined_op_helper(EG(current_execute_data)->opline->op2.var);
		dim = &EG(uninitialized_zval);
	}
	if (!Z_OBJ_HT_P(container)->read_dimension) {
		zend_throw_error(NULL, "Cannot use object as array");
		ZVAL_NULL(result);
	} else {
		zval *retval = Z_OBJ_HT_P(container)->read_dimension(container, dim, BP_VAR_R, result);

		if (retval) {
			if (result != retval) {
				ZVAL_COPY(result, retval);
			}
		} else {
			ZVAL_NULL(result);
		}
	}
}

static void ZEND_FASTCALL zend_jit_fetch_dim_obj_is_helper(zval *container, zval *dim, zval *result)
{
	if (UNEXPECTED(Z_TYPE_P(dim) == IS_UNDEF)) {
		zend_jit_undefined_op_helper(EG(current_execute_data)->opline->op2.var);
		dim = &EG(uninitialized_zval);
	}
	if (!Z_OBJ_HT_P(container)->read_dimension) {
		zend_throw_error(NULL, "Cannot use object as array");
		ZVAL_NULL(result);
	} else {
		zval *retval = Z_OBJ_HT_P(container)->read_dimension(container, dim, BP_VAR_IS, result);

		if (retval) {
			if (result != retval) {
				ZVAL_COPY(result, retval);
			}
		} else {
			ZVAL_NULL(result);
		}
	}
}

static void ZEND_FASTCALL zend_jit_zval_copy_unref_helper(zval *dst, zval *src)
{
	ZVAL_UNREF(src);
	ZVAL_COPY(dst, src);
}

static zval* ZEND_FASTCALL zend_jit_new_ref_helper(zval *value)
{
	zend_reference *ref = (zend_reference*)emalloc(sizeof(zend_reference));
	GC_REFCOUNT(ref) = 1;
	GC_TYPE_INFO(ref) = IS_REFERENCE;
	ZVAL_COPY_VALUE(&ref->val, value);
	Z_REF_P(value) = ref;
	Z_TYPE_INFO_P(value) = IS_REFERENCE_EX;

	return value;
}

static zval* ZEND_FASTCALL zend_jit_fetch_global_helper(zend_execute_data *execute_data, zval *varname)
{
	uint32_t idx;
	zval *value = zend_hash_find(&EG(symbol_table), Z_STR_P(varname));

	if (UNEXPECTED(value == NULL)) {
		value = zend_hash_add_new(&EG(symbol_table), Z_STR_P(varname), &EG(uninitialized_zval));
		idx = ((char*)value - (char*)EG(symbol_table).arData) / sizeof(Bucket);
		/* Store "hash slot index" + 1 (NULL is a mark of uninitialized cache slot) */
		CACHE_PTR(Z_CACHE_SLOT_P(varname), (void*)(uintptr_t)(idx + 1));
	} else {
		idx = ((char*)value - (char*)EG(symbol_table).arData) / sizeof(Bucket);
		/* Store "hash slot index" + 1 (NULL is a mark of uninitialized cache slot) */
		CACHE_PTR(Z_CACHE_SLOT_P(varname), (void*)(uintptr_t)(idx + 1));
		/* GLOBAL variable may be an INDIRECT pointer to CV */
		if (UNEXPECTED(Z_TYPE_P(value) == IS_INDIRECT)) {
			value = Z_INDIRECT_P(value);
			if (UNEXPECTED(Z_TYPE_P(value) == IS_UNDEF)) {
				ZVAL_NULL(value);
			}
		}
	}

	if (UNEXPECTED(!Z_ISREF_P(value))) {
		return zend_jit_new_ref_helper(value);
	}

	return value;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
