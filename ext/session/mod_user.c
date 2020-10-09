/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_session.h"
#include "mod_user.h"

const ps_module ps_mod_user = {
	PS_MOD_UPDATE_TIMESTAMP(user)
};


static void ps_call_handler(zval *func, int argc, zval *argv, zval *retval)
{
	int i;
	if (PS(in_save_handler)) {
		PS(in_save_handler) = 0;
		ZVAL_UNDEF(retval);
		php_error_docref(NULL, E_WARNING, "Cannot call session save handler in a recursive manner");
		return;
	}
	PS(in_save_handler) = 1;
	if (call_user_function(NULL, NULL, func, retval, argc, argv) == FAILURE) {
		zval_ptr_dtor(retval);
		ZVAL_UNDEF(retval);
	} else if (Z_ISUNDEF_P(retval)) {
		ZVAL_NULL(retval);
	}
	PS(in_save_handler) = 0;
	for (i = 0; i < argc; i++) {
		zval_ptr_dtor(&argv[i]);
	}
}

#define STDVARS								\
	zval retval;							\
	int ret = FAILURE

#define PSF(a) PS(mod_user_names).name.ps_##a

#define FINISH \
	if (Z_TYPE(retval) != IS_UNDEF) { \
		if (Z_TYPE(retval) == IS_TRUE) { \
			ret = SUCCESS; \
		} else if (Z_TYPE(retval) == IS_FALSE) { \
			ret = FAILURE; \
        }  else if ((Z_TYPE(retval) == IS_LONG) && (Z_LVAL(retval) == -1)) { \
			if (!EG(exception)) { \
				php_error_docref(NULL, E_DEPRECATED, "Session callback must have a return value of type bool, %s returned", zend_zval_type_name(&retval)); \
			} \
			ret = FAILURE; \
		} else if ((Z_TYPE(retval) == IS_LONG) && (Z_LVAL(retval) == 0)) { \
			if (!EG(exception)) { \
				php_error_docref(NULL, E_DEPRECATED, "Session callback must have a return value of type bool, %s returned", zend_zval_type_name(&retval)); \
			} \
			ret = SUCCESS; \
		} else { \
			if (!EG(exception)) { \
				zend_type_error("Session callback must have a return value of type bool, %s returned", zend_zval_type_name(&retval)); \
			} \
			ret = FAILURE; \
			zval_ptr_dtor(&retval); \
		} \
	} \
	return ret

PS_OPEN_FUNC(user)
{
	zval args[2];
	STDVARS;

	if (Z_ISUNDEF(PSF(open))) {
		php_error_docref(NULL, E_WARNING, "User session functions are not defined");

		return FAILURE;
	}

	ZVAL_STRING(&args[0], (char*)save_path);
	ZVAL_STRING(&args[1], (char*)session_name);

	zend_try {
		ps_call_handler(&PSF(open), 2, args, &retval);
	} zend_catch {
		PS(session_status) = php_session_none;
		if (!Z_ISUNDEF(retval)) {
			zval_ptr_dtor(&retval);
		}
		zend_bailout();
	} zend_end_try();

	PS(mod_user_implemented) = 1;

	FINISH;
}

PS_CLOSE_FUNC(user)
{
	zend_bool bailout = 0;
	STDVARS;

	if (!PS(mod_user_implemented)) {
		/* already closed */
		return SUCCESS;
	}

	zend_try {
		ps_call_handler(&PSF(close), 0, NULL, &retval);
	} zend_catch {
		bailout = 1;
	} zend_end_try();

	PS(mod_user_implemented) = 0;

	if (bailout) {
		if (!Z_ISUNDEF(retval)) {
			zval_ptr_dtor(&retval);
		}
		zend_bailout();
	}

	FINISH;
}

PS_READ_FUNC(user)
{
	zval args[1];
	STDVARS;

	ZVAL_STR_COPY(&args[0], key);

	ps_call_handler(&PSF(read), 1, args, &retval);

	if (!Z_ISUNDEF(retval)) {
		if (Z_TYPE(retval) == IS_STRING) {
			*val = zend_string_copy(Z_STR(retval));
			ret = SUCCESS;
		}
		zval_ptr_dtor(&retval);
	}

	return ret;
}

PS_WRITE_FUNC(user)
{
	zval args[2];
	STDVARS;

	ZVAL_STR_COPY(&args[0], key);
	ZVAL_STR_COPY(&args[1], val);

	ps_call_handler(&PSF(write), 2, args, &retval);

	FINISH;
}

PS_DESTROY_FUNC(user)
{
	zval args[1];
	STDVARS;

	ZVAL_STR_COPY(&args[0], key);

	ps_call_handler(&PSF(destroy), 1, args, &retval);

	FINISH;
}

PS_GC_FUNC(user)
{
	zval args[1];
	zval retval;

	ZVAL_LONG(&args[0], maxlifetime);

	ps_call_handler(&PSF(gc), 1, args, &retval);

	if (Z_TYPE(retval) == IS_LONG) {
		*nrdels = Z_LVAL(retval);
	} else if (Z_TYPE(retval) == IS_TRUE) {
		/* This is for older API compatibility */
		*nrdels = 1;
	} else {
		/* Anything else is some kind of error */
		*nrdels = -1; // Error
	}
	return *nrdels;
}

PS_CREATE_SID_FUNC(user)
{
	/* maintain backwards compatibility */
	if (!Z_ISUNDEF(PSF(create_sid))) {
		zend_string *id = NULL;
		zval retval;

		ps_call_handler(&PSF(create_sid), 0, NULL, &retval);

		if (!Z_ISUNDEF(retval)) {
			if (Z_TYPE(retval) == IS_STRING) {
				id = zend_string_copy(Z_STR(retval));
			}
			zval_ptr_dtor(&retval);
		} else {
			zend_throw_error(NULL, "No session id returned by function");
			return NULL;
		}

		if (!id) {
			zend_throw_error(NULL, "Session id must be a string");
			return NULL;
		}

		return id;
	}

	/* function as defined by PS_MOD */
	return php_session_create_id(mod_data);
}

PS_VALIDATE_SID_FUNC(user)
{
	/* maintain backwards compatibility */
	if (!Z_ISUNDEF(PSF(validate_sid))) {
		zval args[1];
		STDVARS;

		ZVAL_STR_COPY(&args[0], key);

		ps_call_handler(&PSF(validate_sid), 1, args, &retval);

		FINISH;
	}

	/* dummy function defined by PS_MOD */
	return php_session_validate_sid(mod_data, key);
}

PS_UPDATE_TIMESTAMP_FUNC(user)
{
	zval args[2];
	STDVARS;

	ZVAL_STR_COPY(&args[0], key);
	ZVAL_STR_COPY(&args[1], val);

	/* maintain backwards compatibility */
	if (!Z_ISUNDEF(PSF(update_timestamp))) {
		ps_call_handler(&PSF(update_timestamp), 2, args, &retval);
	} else {
		ps_call_handler(&PSF(write), 2, args, &retval);
	}

	FINISH;
}
