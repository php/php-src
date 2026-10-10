/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Sara Golemon <pollita@php.net>                              |
   +----------------------------------------------------------------------+
 */

#include "converter_class.h"
#include "converter.h"
#include "../intl_icu_compat.h"
#include "zend_exceptions.h"
#include "converter_arginfo.h"

zend_class_entry *php_converter_ce;
static zend_object_handlers php_converter_object_handlers;

static void php_converter_resolve_callback(
	zend_fcall_info_cache *fcc,
	zend_object *this_ptr,
	const char *callback_name,
	size_t callback_name_len
) {
	zend_function *fn = reinterpret_cast<zend_function *>(zend_hash_str_find_ptr_lc(&this_ptr->ce->function_table, callback_name, callback_name_len));
	ZEND_ASSERT(fn != nullptr);

	fcc->function_handler = fn;
	fcc->object = this_ptr;
	fcc->called_scope = this_ptr->ce;
	fcc->closure = nullptr;
}

/* {{{ Converter create/clone/destroy */
static void php_converter_free_object(zend_object *obj) {
	php_converter_object *objval = php_converter_fetch_object(obj);

	if (objval->src) {
		ucnv_close(objval->src);
	}

	if (objval->dest) {
		ucnv_close(objval->dest);
	}

	intl_error_reset(&objval->error);
	zend_object_std_dtor(obj);
}

static zend_object *php_converter_object_ctor(zend_class_entry *ce, php_converter_object **pobjval) {
	php_converter_object *objval;

	objval = reinterpret_cast<php_converter_object *>(zend_object_alloc(sizeof(php_converter_object), ce));

	zend_object_std_init(&objval->obj, ce);
	object_properties_init(&objval->obj, ce);
	intl_error_init(&(objval->error));
	php_converter_resolve_callback(&objval->to_cache, &objval->obj, ZEND_STRL("toUCallback"));
	php_converter_resolve_callback(&objval->from_cache, &objval->obj, ZEND_STRL("fromUCallback"));

	*pobjval = objval;

	return &objval->obj;
}

static zend_object *php_converter_create_object(zend_class_entry *ce) {
	php_converter_object *objval = nullptr;
	zend_object *retval = php_converter_object_ctor(ce, &objval);

	object_properties_init(&(objval->obj), ce);

	return retval;
}

static zend_object *php_converter_clone_object(zend_object *object) {
	const php_converter_object *oldobj = php_converter_fetch_object(object);
	php_converter_object *objval;
	zend_object *retval = php_converter_object_ctor(object->ce, &objval);
	UErrorCode error = U_ZERO_ERROR;

	objval->src = intl_icu_compat_ucnv_clone(oldobj->src, &error);
	if (U_SUCCESS(error)) {
		error = U_ZERO_ERROR;
		objval->dest = intl_icu_compat_ucnv_clone(oldobj->dest, &error);
	}

	if (U_FAILURE(error)) {
		zend_throw_error(NULL, "Failed to clone UConverter");
		return retval;
	}

	/* Update contexts for converter error handlers */
	php_converter_set_callbacks(objval, objval->src );
	php_converter_set_callbacks(objval, objval->dest);

	zend_objects_clone_members(&(objval->obj), &(oldobj->obj));

	/* Newly cloned object deliberately does not inherit error state from original object */

	return retval;
}
/* }}} */

/* {{{ php_converter_minit */
U_CFUNC int php_converter_minit(INIT_FUNC_ARGS) {
	php_converter_ce = register_class_UConverter();
	php_converter_ce->create_object = php_converter_create_object;
	php_converter_ce->default_object_handlers = &php_converter_object_handlers;
	memcpy(&php_converter_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_converter_object_handlers.offset = offsetof(php_converter_object, obj);
	php_converter_object_handlers.clone_obj = php_converter_clone_object;
	php_converter_object_handlers.free_obj = php_converter_free_object;

	return SUCCESS;
}
/* }}} */
