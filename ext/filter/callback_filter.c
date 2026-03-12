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

zend_result php_filter_callback(PHP_INPUT_FILTER_PARAM_DECL)
{
	zval retval;
	zend_fcall_info_cache fcc;

	if (!option_array || !zend_is_callable_ex(option_array, NULL, IS_CALLABLE_SUPPRESS_DEPRECATIONS, NULL, &fcc, NULL)) {
		zend_type_error("%s(): Option must be a valid callback", get_active_function_name());
		zval_ptr_dtor(value);
		ZVAL_NULL(value);
		return SUCCESS;
	}

	zend_call_known_fcc(&fcc, &retval, 1, value, NULL);
	zval_ptr_dtor(value);

	if (!Z_ISUNDEF(retval)) {
		ZVAL_COPY_VALUE(value, &retval);
	} else {
		ZVAL_NULL(value);
	}
	return SUCCESS;
}
