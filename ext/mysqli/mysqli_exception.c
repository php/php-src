/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Georg Richter <georg@php.net>                                |
  +----------------------------------------------------------------------+

*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_mysqli_structs.h"
#include "mysqli_priv.h"
#include "zend_exceptions.h"

/* {{{ mysqli_exception_methods[]
 */
const zend_function_entry mysqli_exception_methods[] = {
	{NULL, NULL, NULL}
};
/* }}} */

void php_mysqli_throw_sql_exception(char *sqlstate, int errorno TSRMLS_DC, char *format, ...)
{
	zval	*sql_ex;
	va_list arg;
	char 	*message;

	va_start(arg, format);
	vspprintf(&message, 0, format, arg);
	va_end(arg);;

	if (!(MyG(report_mode) & MYSQLI_REPORT_STRICT)) {
	 	php_error_docref(NULL TSRMLS_CC, E_WARNING, "(%s/%d): %s", sqlstate, errorno, message);
		efree(message);
		return;
	}

	MAKE_STD_ZVAL(sql_ex);
	object_init_ex(sql_ex, mysqli_exception_class_entry);

	if (message) {
		zend_update_property_string(mysqli_exception_class_entry, sql_ex, "message", sizeof("message") - 1,
									message TSRMLS_CC);
	}

	if (sqlstate) {
		zend_update_property_string(mysqli_exception_class_entry, sql_ex, "sqlstate", sizeof("sqlstate") - 1,
									sqlstate TSRMLS_CC);
	} else {
		zend_update_property_string(mysqli_exception_class_entry, sql_ex, "sqlstate", sizeof("sqlstate") - 1,
									"00000" TSRMLS_CC);
	}

	efree(message);
	zend_update_property_long(mysqli_exception_class_entry, sql_ex, "code", sizeof("code") - 1, errorno TSRMLS_CC);

	zend_throw_exception_object(sql_ex TSRMLS_CC);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
