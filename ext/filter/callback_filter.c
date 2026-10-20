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
