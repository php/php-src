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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_API.h"
#include "zend_reflection_api.h"

zend_class_entry *default_exception_ptr;

ZEND_FUNCTION(exception)
{
	char  *message = NULL;
	long   code = 0;
	zval  *tmp;
	zval  *object;
	int    argc = ZEND_NUM_ARGS(), message_len;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC, "|sl", &message, &message_len, &code) == FAILURE) {
		zend_error(E_CORE_ERROR, "Wrong parameter count for exception([string $exception [, long $code ]])");
	}

	object = getThis();

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, message ? message : "Unknown exception", 1);
	zend_hash_update(Z_OBJPROP_P(object), "message", sizeof("message"), (void **) &tmp, sizeof(zval *), NULL);

	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, code);
	zend_hash_update(Z_OBJPROP_P(object), "code", sizeof("code"), (void **) &tmp, sizeof(zval *), NULL);

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, zend_get_executed_filename(TSRMLS_C), 1);
	zend_hash_update(Z_OBJPROP_P(object), "file", sizeof("file"), (void **) &tmp, sizeof(zval *), NULL);

	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, zend_get_executed_lineno(TSRMLS_C));
	zend_hash_update(Z_OBJPROP_P(object), "line", sizeof("line"), (void **) &tmp, sizeof(zval *), NULL);
}

#define DEFAULT_0_PARAMS \
	if (ZEND_NUM_ARGS() > 0) { \
		ZEND_WRONG_PARAM_COUNT(); \
	}

static void _default_exception_get_entry(zval *object, char *name, int name_len, zval *return_value TSRMLS_DC)
{
	zval **value;

	if (zend_hash_find(Z_OBJPROP_P(object), name, name_len, (void **) &value) == FAILURE) {
		RETURN_FALSE;
	}

	*return_value = **value;
	zval_copy_ctor(return_value);
}

ZEND_FUNCTION(getfile)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "file", sizeof("file"), return_value TSRMLS_CC);
}

ZEND_FUNCTION(getline)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "line", sizeof("line"), return_value TSRMLS_CC);
}

ZEND_FUNCTION(getmessage)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "message", sizeof("message"), return_value TSRMLS_CC);
}

ZEND_FUNCTION(getcode)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "code", sizeof("code"), return_value TSRMLS_CC);
}

static zend_function_entry default_exception_functions[] = {
	ZEND_FE(exception, NULL)
	ZEND_FE(getmessage, NULL)
	ZEND_FE(getcode, NULL)
	ZEND_FE(getfile, NULL)
	ZEND_FE(getline, NULL)
	{NULL, NULL, NULL}
};

static void zend_register_default_exception(TSRMLS_D)
{
	zend_class_entry default_exception;

	INIT_CLASS_ENTRY(default_exception, "exception", default_exception_functions);
	default_exception_ptr = zend_register_internal_class(&default_exception TSRMLS_CC);
}

ZEND_API zend_class_entry *zend_exception_get_default(void)
{
	return default_exception_ptr;
}

ZEND_API void zend_throw_exception(char *message, int duplicate TSRMLS_DC)
{
	zval *ex;
	zval *tmp;
	HashTable *properties;

	MAKE_STD_ZVAL(ex);
	object_init_ex(ex, default_exception_ptr);
	properties = Z_OBJPROP_P(ex);

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, message, duplicate);
	zend_hash_update(properties, "message", sizeof("message"), (void **) &tmp, sizeof(zval *), NULL);

	MAKE_STD_ZVAL(tmp);
	ZVAL_STRING(tmp, zend_get_executed_filename(TSRMLS_C), 1);
	zend_hash_update(properties, "file", sizeof("file"), (void **) &tmp, sizeof(zval *), NULL);

	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, zend_get_executed_lineno(TSRMLS_C));
	zend_hash_update(properties, "line", sizeof("line"), (void **) &tmp, sizeof(zval *), NULL);

	EG(exception) = ex;
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
