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

/* rv is a slot provided by the callee that is returned */
static zval *zend_user_class_fetch_dimension(zend_object *object, zval *offset, zval *rv)
{
	zend_class_entry *ce = object->ce;
	zend_function *zf = zend_hash_str_find_ptr(&ce->function_table, "offsetfetch", strlen("offsetfetch"));

	ZEND_ASSERT(zf);
	ZEND_ASSERT(offset);

	GC_ADDREF(object);
	zend_call_known_instance_method_with_1_params(zf, object, rv, offset);
	OBJ_RELEASE(object);

	if (UNEXPECTED(Z_TYPE_P(rv) == IS_UNDEF)) {
		ZEND_ASSERT(EG(exception));
		return NULL;
	} else {
		if (!Z_ISREF_P(rv)) {
			if (Z_TYPE_P(rv) != IS_OBJECT) {
				zend_class_entry *ce = object->ce;
				zend_error(E_NOTICE, "Indirect modification of overloaded element of %s has no effect", ZSTR_VAL(ce->name));
			}
		}
		//else if (UNEXPECTED(Z_REFCOUNT_P(rv) == 1)) {
		//	ZVAL_UNREF(rv); // Why is this wanted???
		//}
	}
	return rv;
}

static zval *zend_legacy_ArrayAccess_fetch_dimension(zend_object *object, zval *offset, zval *rv)
{
	zend_class_entry *ce = object->ce;
	zend_function *zf = zend_hash_str_find_ptr(&ce->function_table, "offsetget", strlen("offsetget"));

	GC_ADDREF(object);
	zend_call_known_instance_method_with_1_params(zf, object, rv, offset);
	OBJ_RELEASE(object);

	if (UNEXPECTED(Z_TYPE_P(rv) == IS_UNDEF)) {
		ZEND_ASSERT(EG(exception));
		return NULL;
	}
	if (!Z_ISREF_P(rv) && Z_TYPE_P(rv) != IS_OBJECT) {
		zend_class_entry *ce = object->ce;
		zend_error(E_NOTICE, "Indirect modification of overloaded element of %s has no effect", ZSTR_VAL(ce->name));

		/* "Create" a ref to the value even if it is not to have BC behaviour */
		ZVAL_NEW_REF(rv, rv);
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

static void zend_user_class_append(zend_object *object, zval *value)
{
	zend_class_entry *ce = object->ce;
	zend_function *zf = zend_hash_str_find_ptr(&ce->function_table, "append", strlen("append"));

	ZEND_ASSERT(zf);
	ZEND_ASSERT(value);

	//Z_TRY_ADDREF_P(value);
	GC_ADDREF(object);
	zend_call_known_instance_method_with_1_params(zf, object, /* retval */ NULL, value);
	OBJ_RELEASE(object);
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

static zval *zend_user_class_fetch_append(zend_object *object, zval *rv)
{
	zend_class_entry *ce = object->ce;
	zend_function *zf = zend_hash_str_find_ptr(&ce->function_table, "fetchappend", strlen("fetchappend"));

	ZEND_ASSERT(zf);

	GC_ADDREF(object);
	zend_call_known_instance_method_with_0_params(zf, object, rv);
	OBJ_RELEASE(object);

	if (UNEXPECTED(Z_TYPE_P(rv) == IS_UNDEF)) {
		ZEND_ASSERT(EG(exception));
		return NULL;
	}
	return rv;
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

// TODO Internal classes must define this?
#define ALLOC_HANDLERS_IF_MISSING(dimension_handlers_funcs_ptr, class_ce) \
	if (!class_ce->dimension_handlers) { \
		dimension_handlers_funcs_ptr = class_ce->type == ZEND_INTERNAL_CLASS \
			? pecalloc(1, sizeof(zend_class_dimensions_functions), true) \
			: zend_arena_calloc(&CG(arena), 1, sizeof(zend_class_dimensions_functions)); \
		class_type->dimension_handlers = dimension_handlers_funcs_ptr; \
	} else { \
		dimension_handlers_funcs_ptr = class_ce->dimension_handlers; \
	}

// TODO Have a nested/fetch handler? To handle nested/fetch dimension reads
static int zend_implement_arrayaccess(zend_class_entry *interface, zend_class_entry *class_type)
{
	ZEND_ASSERT(!class_type->arrayaccess_funcs_ptr && "ArrayAccess funcs already set?");
	zend_class_arrayaccess_funcs *funcs_ptr = class_type->type == ZEND_INTERNAL_CLASS
		? pemalloc(sizeof(zend_class_arrayaccess_funcs), 1)
		: zend_arena_alloc(&CG(arena), sizeof(zend_class_arrayaccess_funcs));
	class_type->arrayaccess_funcs_ptr = funcs_ptr;

	funcs_ptr->zf_offsetget = zend_hash_str_find_ptr(
		&class_type->function_table, "offsetget", sizeof("offsetget") - 1);
	funcs_ptr->zf_offsetexists = zend_hash_str_find_ptr(
		&class_type->function_table, "offsetexists", sizeof("offsetexists") - 1);
	funcs_ptr->zf_offsetset = zend_hash_str_find_ptr(
		&class_type->function_table, "offsetset", sizeof("offsetset") - 1);
	funcs_ptr->zf_offsetunset = zend_hash_str_find_ptr(
		&class_type->function_table, "offsetunset", sizeof("offsetunset") - 1);

	zend_class_dimensions_functions *funcs = NULL;
	ALLOC_HANDLERS_IF_MISSING(funcs, class_type);

	funcs->read_dimension = zend_user_class_read_dimension;
	funcs->has_dimension = zend_user_class_has_dimension;
	funcs->fetch_dimension = zend_legacy_ArrayAccess_fetch_dimension;
	funcs->write_dimension = zend_user_class_write_dimension;
	funcs->unset_dimension = zend_user_class_unset_dimension;
	funcs->append = zend_legacy_ArrayAccess_append;
	funcs->fetch_append = zend_legacy_ArrayAccess_fetch_append;

	return SUCCESS;
}

static int zend_implement_dimension_read(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->type == ZEND_INTERNAL_CLASS) {
		return SUCCESS;
	}

	zend_class_dimensions_functions *funcs = NULL;
	ALLOC_HANDLERS_IF_MISSING(funcs, class_type);

	// TODO: Check if the class that interface implements this relies on the parent handler or not?
	if (!funcs->read_dimension) {
		funcs->read_dimension = zend_user_class_read_dimension;
		funcs->has_dimension = zend_user_class_has_dimension;
	}
	return SUCCESS;
}

static int zend_implement_dimension_write(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->type == ZEND_INTERNAL_CLASS) {
		return SUCCESS;
	}

	zend_class_dimensions_functions *funcs = NULL;
	ALLOC_HANDLERS_IF_MISSING(funcs, class_type);

	if (!funcs->write_dimension) {
		funcs->write_dimension = zend_user_class_write_dimension;
	}
	return SUCCESS;
}

static int zend_implement_dimension_unset(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->type == ZEND_INTERNAL_CLASS) {
		return SUCCESS;
	}

	zend_class_dimensions_functions *funcs = NULL;
	ALLOC_HANDLERS_IF_MISSING(funcs, class_type);

	if (!funcs->unset_dimension) {
		funcs->unset_dimension = zend_user_class_unset_dimension;
	}
	return SUCCESS;
}

static int zend_implement_appendable(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->type == ZEND_INTERNAL_CLASS) {
		return SUCCESS;
	}

	zend_class_dimensions_functions *funcs = NULL;
	ALLOC_HANDLERS_IF_MISSING(funcs, class_type);

	if (!funcs->append) {
		funcs->append = zend_user_class_append;
	}
	return SUCCESS;
}

static int zend_implement_dimension_fetch(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->type == ZEND_INTERNAL_CLASS) {
		return SUCCESS;
	}

	zend_class_dimensions_functions *funcs = NULL;
	ALLOC_HANDLERS_IF_MISSING(funcs, class_type);

	if (!funcs->fetch_dimension) {
		funcs->fetch_dimension = zend_user_class_fetch_dimension;
	}
	return SUCCESS;
}

static int zend_implement_dimension_fetch_append(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->type == ZEND_INTERNAL_CLASS) {
		return SUCCESS;
	}

	zend_class_dimensions_functions *funcs = NULL;
	ALLOC_HANDLERS_IF_MISSING(funcs, class_type);

	if (!funcs->fetch_append) {
		funcs->fetch_append = zend_user_class_fetch_append;
	}
	return SUCCESS;
}

ZEND_API void zend_register_dimension_interfaces(void)
{
	zend_ce_arrayaccess = register_class_ArrayAccess();
	zend_ce_arrayaccess->interface_gets_implemented = zend_implement_arrayaccess;

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
}
