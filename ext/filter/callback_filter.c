/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
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

	if (!option_array || !zend_is_callable(option_array, IS_CALLABLE_SUPPRESS_DEPRECATIONS, NULL)) {
		zend_type_error("%s(): Option must be a valid callback", get_active_function_name());
		zval_ptr_dtor(value);
		ZVAL_NULL(value);
		return;
	}

	ZVAL_COPY(&args[0], value);
	status = call_user_function(NULL, NULL, option_array, &retval, 1, args);

	if (status == SUCCESS && !Z_ISUNDEF(retval)) {
		zval_ptr_dtor(value);
		ZVAL_COPY_VALUE(value, &retval);
	} else {
		zval_ptr_dtor(value);
		ZVAL_NULL(value);
	}

	zval_ptr_dtor(&args[0]);
}
