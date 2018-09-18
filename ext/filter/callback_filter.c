/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Derick Rethans <derick@php.net>                             |
  +----------------------------------------------------------------------+
*/

#include "php_filter.h"

void php_filter_callback(PHP_INPUT_FILTER_PARAM_DECL)
{
	zval retval;
	zval args[1];
	int status;

	if (!option_array || !zend_is_callable(option_array, IS_CALLABLE_CHECK_NO_ACCESS, NULL)) {
		php_error_docref(NULL, E_WARNING, "First argument is expected to be a valid callback");
		zval_ptr_dtor(value);
		ZVAL_NULL(value);
		return;
	}

	ZVAL_COPY(&args[0], value);
	status = call_user_function_ex(EG(function_table), NULL, option_array, &retval, 1, args, 0, NULL);

	if (status == SUCCESS && !Z_ISUNDEF(retval)) {
		zval_ptr_dtor(value);
		ZVAL_COPY_VALUE(value, &retval);
	} else {
		zval_ptr_dtor(value);
		ZVAL_NULL(value);
	}

	zval_ptr_dtor(&args[0]);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
