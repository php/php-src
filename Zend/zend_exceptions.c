/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Sterling Hughes <sterling@php.net>                          |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_API.h"
#include "zend_reflection_api.h"

zend_class_entry *default_exception_ptr;

static zend_object_value zend_default_exception_new(zend_class_entry *class_type TSRMLS_DC)
{
	zval tmp, obj;
	zend_object *object;

	obj.value.obj = zend_objects_new(&object, class_type TSRMLS_CC);

	ALLOC_HASHTABLE(object->properties);
	zend_hash_init(object->properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(object->properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	zend_update_property_string(class_type, &obj, "file", sizeof("file")-1, zend_get_executed_filename(TSRMLS_C) TSRMLS_CC);
	zend_update_property_long(class_type, &obj, "line", sizeof("line")-1, zend_get_executed_lineno(TSRMLS_C) TSRMLS_CC);

	return obj.value.obj;
}

ZEND_METHOD(exception, __construct)
{
	char  *message = NULL;
	long   code = 0;
	zval  *object;
	int    argc = ZEND_NUM_ARGS(), message_len;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC, "|sl", &message, &message_len, &code) == FAILURE) {
		zend_error(E_CORE_ERROR, "Wrong parameter count for exception([string $exception [, long $code ]])");
	}

	object = getThis();

	if (message) {
		zend_update_property_string(Z_OBJCE_P(object), object, "message", sizeof("message")-1, message TSRMLS_CC);
	}

	if (code) {
		zend_update_property_long(Z_OBJCE_P(object), object, "code", sizeof("code")-1, code TSRMLS_CC);
	}
}

#define DEFAULT_0_PARAMS \
	if (ZEND_NUM_ARGS() > 0) { \
		ZEND_WRONG_PARAM_COUNT(); \
	}

static void _default_exception_get_entry(zval *object, char *name, int name_len, zval *return_value TSRMLS_DC)
{
	zval *value;

	value = zend_read_property(Z_OBJCE_P(object), object, name, name_len, 0 TSRMLS_CC);

	*return_value = *value;
	zval_copy_ctor(return_value);
}

ZEND_METHOD(exception, getfile)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "file", sizeof("file")-1, return_value TSRMLS_CC);
}

ZEND_METHOD(exception, getline)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "line", sizeof("line")-1, return_value TSRMLS_CC);
}

ZEND_METHOD(exception, getmessage)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "message", sizeof("message")-1, return_value TSRMLS_CC);
}

ZEND_METHOD(exception, getcode)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "code", sizeof("code")-1, return_value TSRMLS_CC);
}

static zend_function_entry default_exception_functions[] = {
	ZEND_ME(exception, __construct, NULL, 0)
	ZEND_ME(exception, getmessage, NULL, 0)
	ZEND_ME(exception, getcode, NULL, 0)
	ZEND_ME(exception, getfile, NULL, 0)
	ZEND_ME(exception, getline, NULL, 0)
	{NULL, NULL, NULL}
};

static void zend_register_default_exception(TSRMLS_D)
{
	zend_class_entry default_exception;

	INIT_CLASS_ENTRY(default_exception, "exception", default_exception_functions);
	default_exception_ptr = zend_register_internal_class(&default_exception TSRMLS_CC);
	default_exception_ptr->create_object = zend_default_exception_new;

	zend_declare_property_string(default_exception_ptr, "message", sizeof("message")-1, "Unknown exception", ZEND_ACC_PROTECTED);
	zend_declare_property_long(default_exception_ptr, "code", sizeof("code")-1, 0, ZEND_ACC_PROTECTED);
	zend_declare_property_null(default_exception_ptr, "file", sizeof("file")-1, ZEND_ACC_PROTECTED);
	zend_declare_property_null(default_exception_ptr, "line", sizeof("line")-1, ZEND_ACC_PROTECTED);
}

ZEND_API zend_class_entry *zend_exception_get_default(void)
{
	return default_exception_ptr;
}

ZEND_API void zend_throw_exception(zend_class_entry *exception_ce, char *message, long code TSRMLS_DC)
{
	zval *ex;

	MAKE_STD_ZVAL(ex);
	if (exception_ce) {
		if (!instanceof_function(exception_ce, default_exception_ptr TSRMLS_CC)) {
			zend_error(E_NOTICE, "Exceptions must be derived from exception");
			exception_ce = default_exception_ptr;
		}
	} else {
		exception_ce = default_exception_ptr;
	}
	object_init_ex(ex, exception_ce);
	

	if (message) {
		zend_update_property_string(exception_ce, ex, "message", sizeof("message")-1, message TSRMLS_CC);
	}
	if (code) {
		zend_update_property_long(exception_ce, ex, "code", sizeof("code")-1, code TSRMLS_CC);
	}

	EG(exception) = ex;
}

static void zend_error_va(int type, const char *file, uint lineno, const char *format, ...)
{
	va_list args;
	
	va_start(args, format);
	zend_error_cb(E_ERROR, file, lineno, format, args);
	va_end(args);
}

ZEND_API void zend_exception_error(zval *exception TSRMLS_DC)
{
	if (instanceof_function(Z_OBJCE_P(exception), default_exception_ptr TSRMLS_CC)) {
		zval *message = zend_read_property(default_exception_ptr, exception, "message", sizeof("message")-1, 1 TSRMLS_CC);
		zval *file = zend_read_property(default_exception_ptr, exception, "file", sizeof("file")-1, 1 TSRMLS_CC);
		zval *lineno = zend_read_property(default_exception_ptr, exception, "line", sizeof("line")-1, 1 TSRMLS_CC);
		zend_error_va(E_ERROR, Z_STRVAL_P(file), Z_LVAL_P(lineno), "Uncaught exception '%s' with message '%s'", Z_OBJCE_P(exception)->name, Z_STRVAL_P(message));
	} else {
		zend_error(E_ERROR, "Uncaught exception '%s'!", Z_OBJCE_P(exception)->name);
	}
}

ZEND_API void zend_register_default_classes(TSRMLS_D)
{
	zend_register_default_exception(TSRMLS_C);
	zend_register_reflection_api(TSRMLS_C);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
