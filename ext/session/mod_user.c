/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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
	PS_MOD(user)
};

#define SESS_ZVAL_LONG(val, a) 					\
{											\
	MAKE_STD_ZVAL(a); 						\
	Z_TYPE_P(a) = IS_LONG; 						\
	Z_LVAL_P(a) = val; 					\
}

#define SESS_ZVAL_STRING(vl, a) 					\
{											\
	int len = strlen(vl); 					\
	MAKE_STD_ZVAL(a); 						\
	Z_TYPE_P(a) = IS_STRING; 					\
	Z_STRLEN_P(a) = len; 				\
	Z_STRVAL_P(a) = estrndup(vl, len); 	\
}

#define SESS_ZVAL_STRINGN(vl, ln, a) 			\
{											\
	MAKE_STD_ZVAL(a); 						\
	Z_TYPE_P(a) = IS_STRING; 					\
	Z_STRLEN_P(a) = ln; 					\
	Z_STRVAL_P(a) = estrndup(vl, ln); 	\
}


static zval *ps_call_handler(zval *func, int argc, zval **argv TSRMLS_DC)
{
	int i;
	zval *retval = NULL;
	
	MAKE_STD_ZVAL(retval);
	if (call_user_function(EG(function_table), NULL, func, retval, 
				argc, argv TSRMLS_CC) == FAILURE) {
		zval_ptr_dtor(&retval);
		retval = NULL;
	}

	for (i = 0; i < argc; i++) {
		zval_ptr_dtor(&argv[i]);
	}

	return retval;
}

#define STDVARS 							\
	zval *retval; 							\
	int ret = FAILURE; 						\
	ps_user *mdata = PS_GET_MOD_DATA();		\
	if (!mdata) 							\
		return FAILURE

#define PSF(a) mdata->name.ps_##a

#define FINISH 								\
	if (retval) {							\
		convert_to_long(retval);			\
		ret = Z_LVAL_P(retval);			\
		zval_ptr_dtor(&retval);				\
	} 										\
	return ret

PS_OPEN_FUNC(user)
{
	zval *args[2];
	STDVARS;
	
	SESS_ZVAL_STRING(save_path, args[0]);
	SESS_ZVAL_STRING(session_name, args[1]);
	
	retval = ps_call_handler(PSF(open), 2, args TSRMLS_CC);
	
	FINISH;
}

PS_CLOSE_FUNC(user)
{
	int i;
	STDVARS;

	retval = ps_call_handler(PSF(close), 0, NULL TSRMLS_CC);

	for (i = 0; i < 6; i++)
		zval_ptr_dtor(&mdata->names[i]);
	efree(mdata);

	PS_SET_MOD_DATA(NULL);

	FINISH;
}

PS_READ_FUNC(user)
{
	zval *args[1];
	STDVARS;

	SESS_ZVAL_STRING(key, args[0]);

	retval = ps_call_handler(PSF(read), 1, args TSRMLS_CC);
	
	if (retval) {
		if (Z_TYPE_P(retval) == IS_STRING) {
			*val = estrndup(Z_STRVAL_P(retval), Z_STRLEN_P(retval));
			*vallen = Z_STRLEN_P(retval);
			ret = SUCCESS;
		}
		zval_ptr_dtor(&retval);
	}

	return ret;
}

PS_WRITE_FUNC(user)
{
	zval *args[2];
	STDVARS;
	
	SESS_ZVAL_STRING(key, args[0]);
	SESS_ZVAL_STRINGN(val, vallen, args[1]);

	retval = ps_call_handler(PSF(write), 2, args TSRMLS_CC);

	FINISH;
}

PS_DESTROY_FUNC(user)
{
	zval *args[1];
	STDVARS;

	SESS_ZVAL_STRING(key, args[0]);

	retval = ps_call_handler(PSF(destroy), 1, args TSRMLS_CC);

	FINISH;
}

PS_GC_FUNC(user)
{
	zval *args[1];
	STDVARS;

	SESS_ZVAL_LONG(maxlifetime, args[0]);

	retval = ps_call_handler(PSF(gc), 1, args TSRMLS_CC);

	FINISH;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
