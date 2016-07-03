/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
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

/* $Id$ */

#include "php_filter.h"

void php_filter_callback(PHP_INPUT_FILTER_PARAM_DECL)
{
	zval *retval_ptr;
	zval ***args;
	int status;

	if (!option_array || !zend_is_callable(option_array, IS_CALLABLE_CHECK_NO_ACCESS, NULL TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "First argument is expected to be a valid callback");
		zval_dtor(value);
		Z_TYPE_P(value) = IS_NULL;
		return;
	}

	args = safe_emalloc(sizeof(zval **), 1, 0);
	args[0] = &value;
	
	status = call_user_function_ex(EG(function_table), NULL, option_array, &retval_ptr, 1, args, 0, NULL TSRMLS_CC);

	if (status == SUCCESS && retval_ptr != NULL) {
		if (retval_ptr != value) {
			zval_dtor(value);
			COPY_PZVAL_TO_ZVAL(*value, retval_ptr);
		} else {
			zval_ptr_dtor(&retval_ptr);
		}
	} else {
		zval_dtor(value);
		Z_TYPE_P(value) = IS_NULL;
	}

	efree(args);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
