/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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

/* $Id$ */

#include "php.h"
#include "php_session.h"
#include "mod_user.h"

ps_module ps_mod_user = {
	PS_MOD_SID(user)
};

#define SESS_ZVAL_LONG(val, a)						\
{													\
	ZVAL_LONG(a, val);								\
}

#define SESS_ZVAL_STRING(vl, a)						\
{													\
	char *__vl = vl;								\
	SESS_ZVAL_STRINGN(__vl, strlen(__vl), a);		\
}

#define SESS_ZVAL_STRINGN(vl, ln, a)				\
{													\
	ZVAL_STRINGL(a, vl, ln);						\
}

#define SESS_ZVAL_STR(vl, a)						\
{													\
	ZVAL_STR_COPY(a, vl);							\
}

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
			/* BC for clever users - Deprecate me */ \
			ret = FAILURE; \
		} else if ((Z_TYPE(retval) == IS_LONG) && (Z_LVAL(retval) == 0)) { \
			/* BC for clever users - Deprecate me */ \
			ret = SUCCESS; \
		} else { \
			if (!EG(exception)) { \
				php_error_docref(NULL, E_WARNING, \
				                 "Session callback expects true/false return value"); \
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
		php_error_docref(NULL, E_WARNING,
			"user session functions not defined");

		return FAILURE;
	}

	SESS_ZVAL_STRING((char*)save_path, &args[0]);
	SESS_ZVAL_STRING((char*)session_name, &args[1]);

	ps_call_handler(&PSF(open), 2, args, &retval);
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

	SESS_ZVAL_STR(key, &args[0]);

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

	SESS_ZVAL_STR(key, &args[0]);
	SESS_ZVAL_STR(val, &args[1]);

	ps_call_handler(&PSF(write), 2, args, &retval);

	FINISH;
}

PS_DESTROY_FUNC(user)
{
	zval args[1];
	STDVARS;

	SESS_ZVAL_STR(key, &args[0]);

	ps_call_handler(&PSF(destroy), 1, args, &retval);

	FINISH;
}

PS_GC_FUNC(user)
{
	zval args[1];
	STDVARS;

	SESS_ZVAL_LONG(maxlifetime, &args[0]);

	ps_call_handler(&PSF(gc), 1, args, &retval);

	FINISH;
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
			php_error_docref(NULL, E_ERROR, "No session id returned by function");
			return NULL;
		}

		if (!id) {
			php_error_docref(NULL, E_ERROR, "Session id must be a string");
			return NULL;
		}

		return id;
	}

	/* function as defined by PS_MOD */
	return php_session_create_id(mod_data);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
