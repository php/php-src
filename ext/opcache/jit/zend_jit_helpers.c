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

static int is_null_constant(zend_class_entry *scope, zval *default_value)
{
	if (Z_CONSTANT_P(default_value)) {
		zval constant;

		ZVAL_COPY(&constant, default_value);
		if (UNEXPECTED(zval_update_constant_ex(&constant, scope) != SUCCESS)) {
			return 0;
		}
		if (Z_TYPE(constant) == IS_NULL) {
			return 1;
		}
		zval_ptr_dtor(&constant);
	}
	return 0;
}

static zend_bool zend_verify_weak_scalar_type_hint(zend_uchar type_hint, zval *arg)
{
    switch (type_hint) {
        case _IS_BOOL: {
            zend_bool dest;

            if (!zend_parse_arg_bool_weak(arg, &dest)) {
                return 0;
            }
            zval_ptr_dtor(arg);
            ZVAL_BOOL(arg, dest);
            return 1;
        }
        case IS_LONG: {
            zend_long dest;

            if (!zend_parse_arg_long_weak(arg, &dest)) {
                return 0;
            }
            zval_ptr_dtor(arg);
            ZVAL_LONG(arg, dest);
            return 1;
        }
        case IS_DOUBLE: {
            double dest;

            if (!zend_parse_arg_double_weak(arg, &dest)) {
                return 0;
            }
            zval_ptr_dtor(arg);
            ZVAL_DOUBLE(arg, dest);
            return 1;
        }
        case IS_STRING: {
            zend_string *dest;

            /* on success "arg" is converted to IS_STRING */
            if (!zend_parse_arg_str_weak(arg, &dest)) {
                return 0;
			}
            return 1;
        }
        default:
            return 0;
    }
}

static zend_bool zend_verify_scalar_type_hint(zend_uchar type_hint, zval *arg, zend_bool strict)
{
	if (UNEXPECTED(strict)) {
		/* SSTH Exception: IS_LONG may be accepted as IS_DOUBLE (converted) */
		if (type_hint != IS_DOUBLE || Z_TYPE_P(arg) != IS_LONG) {
			return 0;
		}
	} else if (UNEXPECTED(Z_TYPE_P(arg) == IS_NULL)) {
		/* NULL may be accepted only by nullable hints (this is already checked) */
		return 0;
	}
	return zend_verify_weak_scalar_type_hint(type_hint, arg);
}

static ZEND_COLD void zend_verify_type_error_common(
		const zend_function *zf, const zend_arg_info *arg_info,
		const zend_class_entry *ce, zval *value,
		const char **fname, const char **fsep, const char **fclass,
		const char **need_msg, const char **need_kind, const char **need_or_null,
		const char **given_msg, const char **given_kind)
{
	zend_bool is_interface = 0;
	*fname = ZSTR_VAL(zf->common.function_name);

	if (zf->common.scope) {
		*fsep =  "::";
		*fclass = ZSTR_VAL(zf->common.scope->name);
	} else {
		*fsep =  "";
		*fclass = "";
	}

	switch (arg_info->type_hint) {
		case IS_OBJECT:
			if (ce) {
				if (ce->ce_flags & ZEND_ACC_INTERFACE) {
					*need_msg = "implement interface ";
					is_interface = 1;
				} else {
					*need_msg = "be an instance of ";
				}
				*need_kind = ZSTR_VAL(ce->name);
			} else {
				/* We don't know whether it's a class or interface, assume it's a class */
				*need_msg = "be an instance of ";
				*need_kind = zf->common.type == ZEND_INTERNAL_FUNCTION
					? ((zend_internal_arg_info *) arg_info)->class_name
					: ZSTR_VAL(arg_info->class_name);
			}
			break;
		case IS_CALLABLE:
			*need_msg = "be callable";
			*need_kind = "";
			break;
		case IS_ITERABLE:
			*need_msg = "be iterable";
			*need_kind = "";
			break;
		default:
			*need_msg = "be of the type ";
			*need_kind = zend_get_type_by_const(arg_info->type_hint);
			break;
	}

	if (arg_info->allow_null) {
		*need_or_null = is_interface ? " or be null" : " or null";
	} else {
		*need_or_null = "";
	}

	if (value) {
		if (arg_info->type_hint == IS_OBJECT && Z_TYPE_P(value) == IS_OBJECT) {
			*given_msg = "instance of ";
			*given_kind = ZSTR_VAL(Z_OBJCE_P(value)->name);
		} else {
			*given_msg = zend_zval_type_name(value);
			*given_kind = "";
		}
	} else {
		*given_msg = "none";
		*given_kind = "";
	}
}

static ZEND_COLD void zend_verify_arg_error(
		const zend_function *zf, const zend_arg_info *arg_info,
		int arg_num, const zend_class_entry *ce, zval *value)
{
	zend_execute_data *ptr = EG(current_execute_data)->prev_execute_data;
	const char *fname, *fsep, *fclass;
	const char *need_msg, *need_kind, *need_or_null, *given_msg, *given_kind;

	if (value) {
		zend_verify_type_error_common(
				zf, arg_info, ce, value,
				&fname, &fsep, &fclass, &need_msg, &need_kind, &need_or_null, &given_msg, &given_kind);

		if (zf->common.type == ZEND_USER_FUNCTION) {
			if (ptr && ptr->func && ZEND_USER_CODE(ptr->func->common.type)) {
				zend_type_error("Argument %d passed to %s%s%s() must %s%s%s, %s%s given, called in %s on line %d",
						arg_num, fclass, fsep, fname, need_msg, need_kind, need_or_null, given_msg, given_kind,
						ZSTR_VAL(ptr->func->op_array.filename), ptr->opline->lineno);
			} else {
				zend_type_error("Argument %d passed to %s%s%s() must %s%s%s, %s%s given", arg_num, fclass, fsep, fname, need_msg, need_kind, need_or_null, given_msg, given_kind);
			}
		} else {
			zend_type_error("Argument %d passed to %s%s%s() must %s%s%s, %s%s given", arg_num, fclass, fsep, fname, need_msg, need_kind, need_or_null, given_msg, given_kind);
		}
	} else {
		zend_missing_arg_error(ptr);
	}
}

static void ZEND_FASTCALL zend_jit_verify_arg_object(zval *arg, zend_op_array *op_array, uint32_t arg_num, zend_arg_info *arg_info, void **cache_slot)
{
	zend_class_entry *ce;
	if (EXPECTED(*cache_slot)) {
		ce = (zend_class_entry *)*cache_slot;
	} else {
		ce = zend_fetch_class(arg_info->class_name, (ZEND_FETCH_CLASS_AUTO | ZEND_FETCH_CLASS_NO_AUTOLOAD));
		if (UNEXPECTED(!ce)) {
			zend_verify_arg_error((zend_function*)op_array, arg_info, arg_num, NULL, arg);
			return;
		}
		*cache_slot = (void *)ce;
	}
	if (UNEXPECTED(!instanceof_function(Z_OBJCE_P(arg), ce))) {
		zend_verify_arg_error((zend_function*)op_array, arg_info, arg_num, ce, arg);
	}
}

static void ZEND_FASTCALL zend_jit_verify_arg_slow(zval *arg, zend_op_array *op_array, uint32_t arg_num, zend_arg_info *arg_info, void **cache_slot, zval *default_value)
{
	zend_class_entry *ce = NULL;

	if (Z_TYPE_P(arg) == IS_NULL &&
		(arg_info->allow_null || (default_value && is_null_constant(op_array->scope, default_value)))) {
		/* Null passed to nullable type */
		return;
	}

	if (UNEXPECTED(arg_info->class_name)) {
		/* This is always an error - we fetch the class name for the error message here */
		if (EXPECTED(*cache_slot)) {
			ce = (zend_class_entry *) *cache_slot;
		} else {
			ce = zend_fetch_class(arg_info->class_name, (ZEND_FETCH_CLASS_AUTO | ZEND_FETCH_CLASS_NO_AUTOLOAD));
			if (ce) {
				*cache_slot = (void *)ce;
			}
		}
		goto err;
	} else if (arg_info->type_hint == IS_CALLABLE) {
		if (zend_is_callable(arg, IS_CALLABLE_CHECK_SILENT, NULL) == 0) {
			goto err;
		}
	} else if (arg_info->type_hint == IS_ITERABLE) {
		if (zend_is_iterable(arg) == 0) {
			goto err;
		}
	} else if (arg_info->type_hint == _IS_BOOL &&
			EXPECTED(Z_TYPE_P(arg) == IS_FALSE || Z_TYPE_P(arg) == IS_TRUE)) {
		return;
	} else {
		if (zend_verify_scalar_type_hint(arg_info->type_hint, arg, ZEND_RET_USES_STRICT_TYPES()) == 0) {
			goto err;
		}
	}
	return;
err:
	zend_verify_arg_error((zend_function*)op_array, arg_info, arg_num, ce, arg);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
