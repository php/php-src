/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Author: Gina Peter Banyard <girgias@php.net>                         |
   +----------------------------------------------------------------------+
*/

#include "zend_interfaces_dimension.h"
#include "zend_API.h" /* For arginfos */
#include "zend_interfaces_dimensions_arginfo.h"
#include "zend_types.h"
#include "zend_compile.h"
#include "zend_alloc.h"
// Assume already included via zend_class_entry definition
//#include "zend_dimension_handlers.h"

ZEND_API zend_class_entry *zend_ce_arrayaccess;

ZEND_API zend_class_entry *zend_ce_dimension_read;
ZEND_API zend_class_entry *zend_ce_dimension_fetch;
ZEND_API zend_class_entry *zend_ce_dimension_write;
ZEND_API zend_class_entry *zend_ce_dimension_unset;
ZEND_API zend_class_entry *zend_ce_appendable;
ZEND_API zend_class_entry *zend_ce_dimension_fetch_append;
ZEND_API zend_class_entry *zend_ce_autovivificapable;

ZEND_API zval* zend_class_read_dimension(zend_object *object, zval *offset, zval *rv) {
	if (object->ce->type == ZEND_USER_CLASS) {
		zend_function *zf = object->ce->dimension_functions->read_dimension;
		ZEND_ASSERT(zf);
		zval tmp_offset;
		ZVAL_COPY_DEREF(&tmp_offset, offset);

		GC_ADDREF(object);
		zend_call_known_instance_method_with_1_params(
			//object->ce->dimension_functions->read_dimension,
			zf,
			object, rv, offset);
		OBJ_RELEASE(object);
		zval_ptr_dtor(&tmp_offset);

		if (UNEXPECTED(Z_TYPE_P(rv) == IS_UNDEF)) {
			ZEND_ASSERT(EG(exception));
			return NULL;
		}
		return rv;
	} else {
		ZEND_ASSERT(object->ce->type == ZEND_INTERNAL_CLASS);
		return object->ce->dimension_handlers->read_dimension(object, offset, rv);
	}
}

ZEND_API bool zend_class_has_dimension(zend_object *object, zval *offset)
{
	if (object->ce->type == ZEND_USER_CLASS) {
		zend_class_entry *ce = object->ce;
		zval tmp_offset;
		zval isset_method_rv;
		bool is_set = false;

		zend_function *zf_has = ce->dimension_functions->has_dimension;
		ZEND_ASSERT(zf_has);
		ZEND_ASSERT(offset);

		ZVAL_COPY_DEREF(&tmp_offset, offset);
		GC_ADDREF(object);
		zend_call_known_instance_method_with_1_params(zf_has, object, &isset_method_rv, &tmp_offset);

		if (UNEXPECTED(Z_TYPE(isset_method_rv) == IS_UNDEF)) {
			ZEND_ASSERT(EG(exception));
		} else {
			// Use i_zend_is_true(isset_method_rv)?
			is_set = Z_TYPE(isset_method_rv) == IS_TRUE;
		}

		zval_ptr_dtor(&isset_method_rv);
		zval_ptr_dtor(&tmp_offset);
		OBJ_RELEASE(object);
		return is_set;
	} else {
		ZEND_ASSERT(object->ce->type == ZEND_INTERNAL_CLASS);
		return object->ce->dimension_handlers->has_dimension(object, offset);
	}
}

ZEND_API bool zend_class_isset_empty_dimension(zend_object *object, zval *offset, bool is_empty)
{
	bool result = is_empty;
	bool exists = zend_class_has_dimension(object, offset);
	if (!exists) {
		return result;
	}

	zval slot;
	zval *retval = zend_class_read_dimension(object, offset, &slot);
	if (UNEXPECTED(!retval)) {
		ZEND_ASSERT(EG(exception) && "zend_class_read_dimension() returned NULL without exception");
		return result;
	}
	ZEND_ASSERT(Z_TYPE_P(retval) != IS_UNDEF);
	/* Check if value is empty, which it is when it is falsy, i.e. not truthy */
	if (is_empty) {
		result = !i_zend_is_true(retval);
	} else {
		/* Check if value is null, if it is we consider it to not be set */
		result = Z_TYPE_P(retval) != IS_NULL;
	}
	zval_ptr_dtor(retval);
	return result;
}

ZEND_API zval* zend_class_fetch_dimension(zend_object *object, zval *offset, zval *rv)
{
	if (object->ce->type == ZEND_USER_CLASS) {
		zend_class_entry *ce = object->ce;
		zend_function *zf = ce->dimension_functions->fetch_dimension;

		ZEND_ASSERT(zf);
		ZEND_ASSERT(offset);

		GC_ADDREF(object);
		zend_call_known_instance_method_with_1_params(zf, object, rv, offset);
		OBJ_RELEASE(object);

		if (UNEXPECTED(Z_TYPE_P(rv) == IS_UNDEF)) {
			ZEND_ASSERT(EG(exception));
			return NULL;
		}

		/* For legacy ArrayAccess calls */
		if (zf == ce->dimension_functions->read_dimension) {
			/* Warn about not returning by-ref */
			if (!Z_ISREF_P(rv) && Z_TYPE_P(rv) != IS_OBJECT) {
				zend_class_entry *ce = object->ce;
				// TODO Make this an E_WARNING
				zend_error(E_NOTICE, "Indirect modification of overloaded element of %s has no effect", ZSTR_VAL(ce->name));
				if (UNEXPECTED(EG(exception))) {
					zval_ptr_dtor(rv);
					ZVAL_UNDEF(rv);
					return NULL;
				}
			}
			return rv;
		}
		ZEND_ASSERT(Z_ISREF_P(rv));
		return rv;
	} else {
		ZEND_ASSERT(object->ce->type == ZEND_INTERNAL_CLASS);
		return object->ce->dimension_handlers->fetch_dimension(object, offset, rv);
	}
}

ZEND_API void zend_class_write_dimension(zend_object *object, zval *offset, zval *value) {
	if (object->ce->type == ZEND_USER_CLASS) {
		zend_class_entry *ce = object->ce;
		zend_function *zf = ce->dimension_functions->write_dimension;

		ZEND_ASSERT(zf);
		ZEND_ASSERT(offset);
		ZEND_ASSERT(value);

		zval tmp_offset;
		ZVAL_COPY_DEREF(&tmp_offset, offset);
		GC_ADDREF(object);
		zend_call_known_instance_method_with_2_params(zf, object, /* retval */ NULL, &tmp_offset, value);
		OBJ_RELEASE(object);
		zval_ptr_dtor(&tmp_offset);
	} else {
		ZEND_ASSERT(object->ce->type == ZEND_INTERNAL_CLASS);
		object->ce->dimension_handlers->write_dimension(object, offset, value);
	}
}

ZEND_API void zend_class_append(zend_object *object, zval *value)
{
	if (object->ce->type == ZEND_USER_CLASS) {
		zend_class_entry *ce = object->ce;
		zend_function *zf = ce->dimension_functions->append;

		ZEND_ASSERT(zf);
		ZEND_ASSERT(value);

		GC_ADDREF(object);
		if (zf == ce->dimension_functions->write_dimension) {
			/* ArrayAccess */
			zval tmp_offset;
			ZVAL_NULL(&tmp_offset);
			zend_call_known_instance_method_with_2_params(zf, object, /* retval */ NULL, &tmp_offset, value);
		} else {
			zend_call_known_instance_method_with_1_params(zf, object, /* retval */ NULL, value);
		}
		OBJ_RELEASE(object);
	} else {
		ZEND_ASSERT(object->ce->type == ZEND_INTERNAL_CLASS);
		object->ce->dimension_handlers->append(object, value);
	}
}

ZEND_API zval *zend_class_fetch_append(zend_object *object, zval *rv)
{
	if (object->ce->type == ZEND_USER_CLASS) {
		zend_class_entry *ce = object->ce;
		zend_function *zf = ce->dimension_functions->fetch_append;

		ZEND_ASSERT(zf);

		if (zf == ce->dimension_functions->read_dimension) {
			/* ArrayAccess */
			zval tmp_offset;
			ZVAL_NULL(&tmp_offset);
			return zend_class_fetch_dimension(object, &tmp_offset, rv);
		}
		GC_ADDREF(object);
		zend_call_known_instance_method_with_0_params(zf, object, rv);
		OBJ_RELEASE(object);

		if (UNEXPECTED(Z_TYPE_P(rv) == IS_UNDEF)) {
			ZEND_ASSERT(EG(exception));
			return NULL;
		}
		ZEND_ASSERT(Z_ISREF_P(rv));
		return rv;
	} else {
		ZEND_ASSERT(object->ce->type == ZEND_INTERNAL_CLASS);
		return object->ce->dimension_handlers->fetch_append(object, rv);
	}
}

ZEND_API void zend_class_unset_dimension(zend_object *object, zval *offset)
{
	if (object->ce->type == ZEND_USER_CLASS) {
		zend_class_entry *ce = object->ce;
		zval tmp_offset;

		zend_function *zf = ce->dimension_functions->unset_dimension;
		ZEND_ASSERT(zf);
		ZEND_ASSERT(offset);

		ZVAL_COPY_DEREF(&tmp_offset, offset);
		GC_ADDREF(object);
		zend_call_known_instance_method_with_1_params(zf, object, /* retval */ NULL, &tmp_offset);
		OBJ_RELEASE(object);
		zval_ptr_dtor(&tmp_offset);
	} else {
		ZEND_ASSERT(object->ce->type == ZEND_INTERNAL_CLASS);
		object->ce->dimension_handlers->unset_dimension(object, offset);
	}
}

ZEND_API void zend_class_autovivify(zend_object *object, zval *reference)
{
	if (object->ce->type == ZEND_USER_CLASS) {
		zend_function *zf = object->ce->dimension_functions->autovivify;
		ZEND_ASSERT(zf);
		GC_ADDREF(object);
		zend_call_known_instance_method_with_1_params(zf, object, /* retval */ NULL, reference);
		OBJ_RELEASE(object);
	} else {
		ZEND_ASSERT(object->ce->type == ZEND_INTERNAL_CLASS);
		object->ce->dimension_handlers->autovivify(object, reference);
	}
}

/* rv is a slot provided by the callee that is returned */
static zval *zend_user_class_read_dimension(zend_object *object, zval *offset, zval *rv)
{
	zend_class_entry *ce = object->ce;
	zval tmp_offset;
	zend_function *zf = zend_hash_str_find_ptr(&ce->function_table, "offsetget", strlen("offsetget"));

	ZEND_ASSERT(zf);
	ZEND_ASSERT(offset);

	ZVAL_COPY_DEREF(&tmp_offset, offset);

	GC_ADDREF(object);
	zend_call_known_instance_method_with_1_params(zf, object, rv, &tmp_offset);
	OBJ_RELEASE(object);
	zval_ptr_dtor(&tmp_offset);

	if (UNEXPECTED(Z_TYPE_P(rv) == IS_UNDEF)) {
		ZEND_ASSERT(EG(exception));
		return NULL;
	}
	return rv;
}

static zval *zend_legacy_ArrayAccess_fetch_dimension(zend_object *object, zval *offset, zval *rv)
{
	zend_class_entry *ce = object->ce;
	zend_function *user_function = zend_hash_str_find_ptr(&ce->function_table, "offsetget", strlen("offsetget"));

	GC_ADDREF(object);
	zend_call_known_function(user_function, object, object->ce, rv, 1, offset, NULL);
	OBJ_RELEASE(object);

	if (UNEXPECTED(Z_TYPE_P(rv) == IS_UNDEF)) {
		ZEND_ASSERT(EG(exception));
		return NULL;
	}

	/* Warn about not returning by-ref */
	if (!Z_ISREF_P(rv)) {
		if (Z_TYPE_P(rv) != IS_OBJECT) {
			zend_class_entry *ce = object->ce;
			// TODO Make this an E_WARNING
			zend_error(E_NOTICE, "Indirect modification of overloaded element of %s has no effect", ZSTR_VAL(ce->name));
			if (UNEXPECTED(EG(exception))) {
				zval_ptr_dtor(rv);
				ZVAL_UNDEF(rv);
				return NULL;
			}
		}
	}

	return rv;
}

static zval *zend_legacy_ArrayAccess_fetch_append(zend_object *object, zval *rv)
{
	zval tmp_offset;

	ZVAL_NULL(&tmp_offset);
	zval *retval = zend_legacy_ArrayAccess_fetch_dimension(object, &tmp_offset, rv);
	zval_ptr_dtor(&tmp_offset);

	return retval;
}

static bool zend_user_class_has_dimension(zend_object *object, zval *offset)
{
	zend_class_entry *ce = object->ce;
	zval tmp_offset;
	zval isset_method_rv;
	bool is_set = false;

	zend_function *zf_has = zend_hash_str_find_ptr(&ce->function_table, "offsetexists", strlen("offsetexists"));
	ZEND_ASSERT(zf_has);
	ZEND_ASSERT(offset);

	ZVAL_COPY_DEREF(&tmp_offset, offset);
	GC_ADDREF(object);
	zend_call_known_instance_method_with_1_params(zf_has, object, &isset_method_rv, &tmp_offset);

	if (UNEXPECTED(Z_TYPE(isset_method_rv) == IS_UNDEF)) {
		ZEND_ASSERT(EG(exception));
	} else {
		// Use i_zend_is_true(isset_method_rv)?
		is_set = Z_TYPE(isset_method_rv) == IS_TRUE;
	}

	zval_ptr_dtor(&isset_method_rv);
	zval_ptr_dtor(&tmp_offset);
	OBJ_RELEASE(object);
	return is_set;
}

static void zend_user_class_write_dimension(zend_object *object, zval *offset, zval *value)
{
	zend_class_entry *ce = object->ce;
	zend_function *zf = zend_hash_str_find_ptr(&ce->function_table, "offsetset", strlen("offsetset"));

	ZEND_ASSERT(zf);
	ZEND_ASSERT(offset);
	ZEND_ASSERT(value);

	zval tmp_offset;
	//Z_TRY_ADDREF_P(value);
	ZVAL_COPY_DEREF(&tmp_offset, offset);
	GC_ADDREF(object);
	zend_call_known_instance_method_with_2_params(zf, object, /* retval */ NULL, &tmp_offset, value);
	OBJ_RELEASE(object);
	zval_ptr_dtor(&tmp_offset);
}

static void zend_legacy_ArrayAccess_append(zend_object *object, zval *value)
{
	zval tmp_offset;
	zend_class_entry *ce = object->ce;
	zend_function *zf = zend_hash_str_find_ptr(&ce->function_table, "offsetset", strlen("offsetset"));

	ZEND_ASSERT(zf);
	ZEND_ASSERT(value);

	ZVAL_NULL(&tmp_offset);
	GC_ADDREF(object);
	zend_call_known_instance_method_with_2_params(zf, object, /* retval */ NULL, &tmp_offset, value);
	OBJ_RELEASE(object);
	zval_ptr_dtor(&tmp_offset);
}

static void zend_user_class_unset_dimension(zend_object *object, zval *offset)
{
	zend_class_entry *ce = object->ce;
	zval tmp_offset;

	zend_function *zf = zend_hash_str_find_ptr(&ce->function_table, "offsetunset", strlen("offsetunset"));
	ZEND_ASSERT(zf);
	ZEND_ASSERT(offset);

	ZVAL_COPY_DEREF(&tmp_offset, offset);
	GC_ADDREF(object);
	zend_call_known_instance_method_with_1_params(zf, object, /* retval */ NULL, &tmp_offset);
	OBJ_RELEASE(object);
	zval_ptr_dtor(&tmp_offset);
}

/* Internal classes must define their own handlers if they overload the dimension access */
#define ALLOC_HANDLERS_IF_MISSING(dimension_handlers_funcs_ptr, class_ce) \
	if (!class_ce->dimension_functions) { \
		ZEND_ASSERT(class_ce->type == ZEND_USER_CLASS); \
		dimension_handlers_funcs_ptr = zend_arena_calloc(&CG(arena), 1, sizeof(zend_user_class_dimensions_functions)); \
		class_type->dimension_functions = dimension_handlers_funcs_ptr; \
	} else { \
		dimension_handlers_funcs_ptr = class_ce->dimension_functions; \
	}

static /* const */ zend_internal_class_dimensions_functions zend_default_array_access_dimensions_functions = {
	.read_dimension  = zend_user_class_read_dimension,
	.has_dimension   = zend_user_class_has_dimension,
	.fetch_dimension = zend_legacy_ArrayAccess_fetch_dimension,
	.write_dimension = zend_user_class_write_dimension,
	.unset_dimension = zend_user_class_unset_dimension,
	.append          = zend_legacy_ArrayAccess_append,
	.fetch_append    = zend_legacy_ArrayAccess_fetch_append,
};

// TODO Have a nested/fetch handler? To handle nested/fetch dimension reads
static int zend_implement_arrayaccess(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->type == ZEND_INTERNAL_CLASS) {
		class_type->dimension_handlers = &zend_default_array_access_dimensions_functions;
		return SUCCESS;
	}

	zend_user_class_dimensions_functions *funcs = NULL;
	ALLOC_HANDLERS_IF_MISSING(funcs, class_type);

	zend_function *zf_read = zend_hash_str_find_ptr(&class_type->function_table, "offsetget", strlen("offsetget"));
	zend_function *zf_write = zend_hash_str_find_ptr(&class_type->function_table, "offsetset", strlen("offsetset"));
	zend_function *zf_has = zend_hash_str_find_ptr(&class_type->function_table, "offsetexists", strlen("offsetexists"));
	zend_function *zf_unset = zend_hash_str_find_ptr(&class_type->function_table, "offsetunset", strlen("offsetunset"));

	funcs->read_dimension = zf_read;
	funcs->has_dimension = zf_has;
	funcs->write_dimension = zf_write;
	funcs->unset_dimension = zf_unset;
	if (funcs->fetch_dimension == NULL) {
		funcs->fetch_dimension = zf_read;
	}
	if (funcs->append == NULL) {
		funcs->append = zf_write;
	}
	if (funcs->fetch_append == NULL) {
		funcs->fetch_append = zf_read;
	}

	return SUCCESS;
}

static int zend_implement_dimension_read(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->type == ZEND_INTERNAL_CLASS) {
		return SUCCESS;
	}

	zend_function *read_fn = zend_hash_str_find_ptr(&class_type->function_table, "offsetget", strlen("offsetget"));
	ZEND_ASSERT(read_fn);
	if (read_fn->common.fn_flags & ZEND_ACC_RETURN_REFERENCE && !instanceof_function(class_type, zend_ce_arrayaccess)) {
		zend_error_noreturn(E_ERROR, "DimensionReadable::offsetGet method must not return by reference, implement DimensionFetchable::offsetFetch instead");
		//zend_throw_error(NULL, "DimensionReadable::offsetGet method must not return by reference, implement DimensionFetchable::offsetFetch instead");
		return FAILURE;
	}

	zend_user_class_dimensions_functions *funcs = NULL;
	ALLOC_HANDLERS_IF_MISSING(funcs, class_type);

	funcs->read_dimension = read_fn;
	funcs->has_dimension = zend_hash_str_find_ptr(&class_type->function_table, "offsetexists", strlen("offsetexists"));;

	ZEND_ASSERT(funcs->has_dimension);

	return SUCCESS;
}

static int zend_implement_dimension_write(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->type == ZEND_INTERNAL_CLASS) {
		return SUCCESS;
	}

	zend_user_class_dimensions_functions *funcs = NULL;
	ALLOC_HANDLERS_IF_MISSING(funcs, class_type);

	funcs->write_dimension = zend_hash_str_find_ptr(&class_type->function_table, "offsetset", strlen("offsetset"));

	return SUCCESS;
}

static int zend_implement_dimension_unset(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->type == ZEND_INTERNAL_CLASS) {
		return SUCCESS;
	}

	zend_user_class_dimensions_functions *funcs = NULL;
	ALLOC_HANDLERS_IF_MISSING(funcs, class_type);

	funcs->unset_dimension = zend_hash_str_find_ptr(&class_type->function_table, "offsetunset", strlen("offsetunset"));

	return SUCCESS;
}

static int zend_implement_appendable(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->type == ZEND_INTERNAL_CLASS) {
		return SUCCESS;
	}

	zend_user_class_dimensions_functions *funcs = NULL;
	ALLOC_HANDLERS_IF_MISSING(funcs, class_type);

	funcs->append = zend_hash_str_find_ptr(&class_type->function_table, "append", strlen("append"));

	return SUCCESS;
}

static int zend_implement_dimension_fetch(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->type == ZEND_INTERNAL_CLASS) {
		return SUCCESS;
	}

	zend_user_class_dimensions_functions *funcs = NULL;
	ALLOC_HANDLERS_IF_MISSING(funcs, class_type);

	funcs->fetch_dimension = zend_hash_str_find_ptr(&class_type->function_table, "offsetfetch", strlen("offsetfetch"));
	return SUCCESS;
}

static int zend_implement_dimension_fetch_append(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->type == ZEND_INTERNAL_CLASS) {
		return SUCCESS;
	}

	zend_user_class_dimensions_functions *funcs = NULL;
	ALLOC_HANDLERS_IF_MISSING(funcs, class_type);

	funcs->fetch_append = zend_hash_str_find_ptr(&class_type->function_table, "fetchappend", strlen("fetchappend"));
	return SUCCESS;
}

static int zend_implement_autovivificapable(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->type == ZEND_INTERNAL_CLASS) {
		return SUCCESS;
	}

	zend_user_class_dimensions_functions *funcs = NULL;
	ALLOC_HANDLERS_IF_MISSING(funcs, class_type);

	funcs->autovivify = zend_hash_str_find_ptr(&class_type->function_table, "autovivify", strlen("autovivify"));
	return SUCCESS;
}

ZEND_API void zend_register_dimension_interfaces(void)
{
	zend_ce_dimension_read = register_class_DimensionReadable();
	zend_ce_dimension_read->interface_gets_implemented = zend_implement_dimension_read;

	zend_ce_dimension_write = register_class_DimensionWritable();
	zend_ce_dimension_write->interface_gets_implemented = zend_implement_dimension_write;

	zend_ce_dimension_unset = register_class_DimensionUnsetable();
	zend_ce_dimension_unset->interface_gets_implemented = zend_implement_dimension_unset;

	zend_ce_appendable = register_class_Appendable();
	zend_ce_appendable->interface_gets_implemented = zend_implement_appendable;

	zend_ce_dimension_fetch = register_class_DimensionFetchable(zend_ce_dimension_read);
	zend_ce_dimension_fetch->interface_gets_implemented = zend_implement_dimension_fetch;

	zend_ce_dimension_fetch_append = register_class_FetchAppendable(zend_ce_appendable);
	zend_ce_dimension_fetch_append->interface_gets_implemented = zend_implement_dimension_fetch_append;

	zend_ce_arrayaccess = register_class_ArrayAccess(zend_ce_dimension_read, zend_ce_dimension_write, zend_ce_dimension_unset);
	zend_ce_arrayaccess->interface_gets_implemented = zend_implement_arrayaccess;

	zend_ce_autovivificapable = register_class_Autovivificapable();
	zend_ce_autovivificapable->interface_gets_implemented = zend_implement_autovivificapable;
}
