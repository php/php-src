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
  | Author: Georg Richter <georg@php.net>                                |
  +----------------------------------------------------------------------+

*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <signal.h>

#include "php.h"
#include "php_mysqli_structs.h"
#include "mysqli_priv.h"
#include "zend_exceptions.h"

void php_mysqli_throw_sql_exception(char *sqlstate, int errorno, char *format, ...)
{
	zval	sql_ex;
	va_list arg;
	char 	*message;

	va_start(arg, format);
	vspprintf(&message, 0, format, arg);
	va_end(arg);

	if (!(MyG(report_mode) & MYSQLI_REPORT_STRICT)) {
	 	php_error_docref(NULL, E_WARNING, "(%s/%d): %s", sqlstate, errorno, message);
		efree(message);
		return;
	}

	object_init_ex(&sql_ex, mysqli_exception_class_entry);

	if (message) {
		zend_update_property_string(
			mysqli_exception_class_entry, Z_OBJ(sql_ex), "message", sizeof("message") - 1, message);
	}

	if (sqlstate) {
		zend_update_property_string(
			mysqli_exception_class_entry, Z_OBJ(sql_ex), "sqlstate", sizeof("sqlstate") - 1, sqlstate);
	} else {
		zend_update_property_string(
			mysqli_exception_class_entry, Z_OBJ(sql_ex), "sqlstate", sizeof("sqlstate") - 1, "00000");
	}

	efree(message);
	zend_update_property_long(mysqli_exception_class_entry, Z_OBJ(sql_ex), "code", sizeof("code") - 1, errorno);

	zend_throw_exception_object(&sql_ex);
}

PHP_METHOD(mysqli_sql_exception, getSqlState)
{
	zval *prop;
	zval rv;

	ZEND_PARSE_PARAMETERS_NONE();

	prop = zend_read_property(mysqli_exception_class_entry, Z_OBJ_P(ZEND_THIS), "sqlstate", sizeof("sqlstate")-1, 1, &rv);
	ZVAL_DEREF(prop);
	zend_string *str = zval_get_string(prop);

	RETURN_STR(str);
}
