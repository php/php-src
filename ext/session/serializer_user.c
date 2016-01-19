/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
   | Author: Yasuo Ohgaki <yohgaki@ohgaki.net/php.net>                    |
   +----------------------------------------------------------------------+
 */


#include "php.h"
#include "php_session.h"

PS_SERIALIZER_ENCODE_FUNC(user);
PS_SERIALIZER_DECODE_FUNC(user);

ps_serializer ps_serializer_user = PS_SERIALIZER_ENTRY(user);


static void ps_call_handler(zval *func, int argc, zval *argv, zval *retval)
{
	int i;
	if (call_user_function(EG(function_table), NULL, func, retval, argc, argv) == FAILURE) {
		zval_ptr_dtor(retval);
		ZVAL_UNDEF(retval);
	} else if (Z_ISUNDEF_P(retval)) {
		ZVAL_NULL(retval);
	}
	for (i = 0; i < argc; i++) {
		zval_ptr_dtor(&argv[i]);
	}
}


#define PSF(a) PS(serializer_user_names).name.ps_##a


/* {{{ PS_SERIALIZER_ENCODE_FUNC
 */
PS_SERIALIZER_ENCODE_FUNC(user)
{
	zval args[1], retval;

	if (Z_ISUNDEF(PSF(encode))) {
		php_error_docref(NULL, E_RECOVERABLE_ERROR,
			"User session encode function is not defined");
		return NULL;
	}

	ZVAL_UNDEF(&retval);
	args[0] = PS(http_session_vars);
	Z_ADDREF(args[0]);
	ps_call_handler(&PSF(encode), 1, args, &retval);

	if (Z_TYPE(retval) == IS_STRING) {
		zend_string *tmp;
		tmp = zend_string_copy(Z_STR(retval));
		zval_ptr_dtor(&retval);
		return tmp;
	} else if (Z_TYPE(retval) == IS_FALSE) {
		return NULL;
	} else {
		if (!EG(exception)) {
			php_error_docref(NULL, E_RECOVERABLE_ERROR,
							 "Session decode callback expects string or FALSE return value");
		}
	}
	return NULL;
}
/* }}} */


/* {{{ PS_SERIALIZER_DECODE_FUNC
 */
PS_SERIALIZER_DECODE_FUNC(user)
{
	zval args[1], retval;
	zend_string *data;
	zend_string *var_name = zend_string_init("_SESSION", sizeof("_SESSION") - 1, 0);

	if (Z_ISUNDEF(PSF(encode))) {
		php_error_docref(NULL, E_RECOVERABLE_ERROR,
			"User session decode function is not defined");
		return FAILURE;
	}

	data = zend_string_init(val, vallen, 0);
	ZVAL_STR(&args[0], data);
	ZVAL_UNDEF(&retval);
	ps_call_handler(&PSF(decode), 1, args, &retval);

	if (Z_TYPE(retval) != IS_ARRAY) {
		if (!Z_ISUNDEF(retval)) {
			zval_ptr_dtor(&retval);
		}
		php_error_docref(NULL, E_RECOVERABLE_ERROR,
						 "User session decode function must return array");
		return FAILURE;
	}

	if (!Z_ISUNDEF(PS(http_session_vars))) {
		zval_ptr_dtor(&PS(http_session_vars));
	}
	ZVAL_NEW_REF(&PS(http_session_vars), &retval);
	Z_ADDREF_P(&PS(http_session_vars));
	zend_hash_update_ind(&EG(symbol_table), var_name, &PS(http_session_vars));
	zend_string_release(var_name);
	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

