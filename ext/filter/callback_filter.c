/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2005 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
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
	char *name;
	zval *args[1];
	
	if (!zend_is_callable(option_array, IS_CALLABLE_CHECK_NO_ACCESS, &name)) {
		php_error_docref1(NULL TSRMLS_CC, name, E_WARNING, "First argument is expected to be a valid callback");
		efree(name);
		Z_TYPE_P(value) = IS_NULL;
		return;
	}

	args[0] = value;

	call_user_function(EG(function_table), NULL, option_array, value, 1, args TSRMLS_CC);
}
