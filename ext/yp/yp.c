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
   | Authors: Stephanie Wehner <_@r4k.net>                                |
   |          Fredrik Ohrn                                                |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"

#if HAVE_YP

#include "php_yp.h"

#include <rpcsvc/ypclnt.h>

/* {{{ thread safety stuff */

#ifdef ZTS
int yp_globals_id;
#else
PHP_YP_API php_yp_globals yp_globals;
#endif

/* }}} */

function_entry yp_functions[] = {
	PHP_FE(yp_get_default_domain, NULL)
	PHP_FE(yp_order, NULL)
	PHP_FE(yp_master, NULL)
	PHP_FE(yp_match, NULL)
	PHP_FE(yp_first, NULL)
	PHP_FE(yp_next, NULL)
	PHP_FE(yp_all, NULL)
	PHP_FE(yp_cat, NULL)
	PHP_FE(yp_errno, NULL)
	PHP_FE(yp_err_string, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry yp_module_entry = {
    STANDARD_MODULE_HEADER,
	"yp",
	yp_functions,
	PHP_MINIT(yp),
	NULL,
	PHP_RINIT(yp),
	NULL,
	PHP_MINFO(yp),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_YP
ZEND_GET_MODULE(yp)
#endif

/* {{{ proto string yp_get_default_domain(void)
   Returns the domain or false */
PHP_FUNCTION(yp_get_default_domain)
{
	char *outdomain;

	if((YP(error) = yp_get_default_domain(&outdomain))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", yperr_string (YP(error)));
		RETURN_FALSE;
	}
	RETVAL_STRING(outdomain,1);
}
/* }}} */

/* {{{ proto int yp_order(string domain, string map)            
   Returns the order number or false */
PHP_FUNCTION(yp_order)
{
	pval **domain, **map;

#if SOLARIS_YP
	unsigned long outval;
#else
	int outval;
#endif

	if((ZEND_NUM_ARGS() != 2) || zend_get_parameters_ex(2,&domain,&map) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(domain);
	convert_to_string_ex(map);

	if((YP(error) = yp_order(Z_STRVAL_PP (domain), Z_STRVAL_PP (map), &outval))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", yperr_string (YP(error)));
		RETURN_FALSE;
	}

	RETVAL_LONG(outval);
}
/* }}} */

/* {{{ proto string yp_master(string domain, string map)
   Returns the machine name of the master */
PHP_FUNCTION(yp_master)
{
	pval **domain, **map;
	char *outname;

	if((ZEND_NUM_ARGS() != 2) || zend_get_parameters_ex(2,&domain,&map) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(domain);
	convert_to_string_ex(map);

	if((YP(error) = yp_master(Z_STRVAL_PP (domain), Z_STRVAL_PP (map), &outname))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", yperr_string (YP(error)));
		RETURN_FALSE;
	}

	RETVAL_STRING(outname,1);
}
/* }}} */

/* {{{ proto string yp_match(string domain, string map, string key)
   Returns the matched line or false */
PHP_FUNCTION(yp_match)
{
	pval **domain, **map, **key;
	char *outval;
	int outvallen;

	if((ZEND_NUM_ARGS() != 3) || zend_get_parameters_ex(3,&domain,&map,&key) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(domain);
	convert_to_string_ex(map);
	convert_to_string_ex(key);

	if((YP(error) = yp_match(Z_STRVAL_PP (domain), Z_STRVAL_PP (map), Z_STRVAL_PP (key), Z_STRLEN_PP (key), &outval, &outvallen))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", yperr_string (YP(error)));
		RETURN_FALSE;
	}

	RETVAL_STRINGL(outval,outvallen,1);
}
/* }}} */

/* {{{ proto array yp_first(string domain, string map)
   Returns the first key as array with $var[$key] and the the line as the value */
PHP_FUNCTION(yp_first)
{
	pval **domain, **map;
	char *outval, *outkey;
	int outvallen, outkeylen;

	if((ZEND_NUM_ARGS() != 2) || zend_get_parameters_ex(2,&domain,&map) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(domain);
	convert_to_string_ex(map);

	if((YP(error) = yp_first(Z_STRVAL_PP (domain), Z_STRVAL_PP (map), &outkey, &outkeylen, &outval, &outvallen))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", yperr_string (YP(error)));
		RETURN_FALSE;
	}
	array_init(return_value);
	add_assoc_stringl_ex(return_value,outkey,outkeylen,outval,outvallen,1);

	/* Deprecated */
	add_assoc_stringl(return_value,"key",outkey,outkeylen,1);
	add_assoc_stringl(return_value,"value",outval,outvallen,1);
}
/* }}} */

/* {{{ proto array yp_next(string domain, string map, string key)
   Returns an array with $var[$key] and the the line as the value */
PHP_FUNCTION(yp_next)
{
	pval **domain, **map, **key;
	char *outval, *outkey;
	int outvallen, outkeylen;

	if((ZEND_NUM_ARGS() != 3) || zend_get_parameters_ex(3,&domain,&map,&key) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(domain);
	convert_to_string_ex(map);
	convert_to_string_ex(key);

	if((YP(error) = yp_next(Z_STRVAL_PP (domain), Z_STRVAL_PP (map), Z_STRVAL_PP (key), Z_STRLEN_PP (key), &outkey, &outkeylen, &outval, &outvallen))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", yperr_string (YP(error)));
		RETURN_FALSE;
	}
	
	array_init(return_value);
	add_assoc_stringl_ex(return_value,outkey,outkeylen,outval,outvallen,1);
}
/* }}} */

/* {{{ php_foreach_all
 */
static int php_foreach_all (int instatus, char *inkey, int inkeylen, char *inval, int invallen, char *indata)
{
	int r;
	zval *status, *key, *value;
	zval **args [3];
	zval *retval;
	TSRMLS_FETCH();

	args[0] = &status;
	args[1] = &key;
	args[2] = &value;

	MAKE_STD_ZVAL (status);
	ZVAL_LONG (status, ypprot_err (instatus));

	MAKE_STD_ZVAL (key);
	ZVAL_STRINGL (key, inkey, inkeylen, 1);

	MAKE_STD_ZVAL (value);
	ZVAL_STRINGL (value, inval, invallen, 1);

	if(call_user_function_ex(CG(function_table), NULL, *((zval **)indata), &retval, 3, args, 0, NULL TSRMLS_CC) != SUCCESS)
	{
		zval_ptr_dtor(&status);
		zval_ptr_dtor(&key);
		zval_ptr_dtor(&value);

		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Function call failed");
		return 1;
	}

	convert_to_long_ex(&retval);
	r = Z_LVAL_P (retval);

	zval_ptr_dtor(&retval);

	zval_ptr_dtor(&status);
	zval_ptr_dtor(&key);
	zval_ptr_dtor(&value);

	return r;
}
/* }}} */

/* {{{ proto void yp_all(string domain, string map, string callback)
   Traverse the map and call a function on each entry */
PHP_FUNCTION(yp_all)
{
	pval **domain, **map, **php_callback;
	struct ypall_callback callback;

	if((ZEND_NUM_ARGS() != 3) || zend_get_parameters_ex(3,&domain,&map,&php_callback) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(domain);
	convert_to_string_ex(map);

	callback.foreach = php_foreach_all;
	callback.data = (char *) php_callback;

	yp_all(Z_STRVAL_PP(domain),Z_STRVAL_PP(map),&callback);

	RETURN_FALSE;
}
/* }}} */

/* {{{ php_foreach_cat
 */
static int php_foreach_cat (int instatus, char *inkey, int inkeylen, char *inval, int invallen, char *indata)
{
	int err;

	err = ypprot_err (instatus);

	if (!err)
	{
		if (inkeylen) {
			char *key = emalloc(inkeylen+1);
			if(key) {
				strncpy(key, inkey, inkeylen);
				key[inkeylen] = '\0';
				add_assoc_stringl_ex((zval *) indata, key, inkeylen+1, inval, invallen, 1);
				efree(key);
			} else {
				php_error(E_WARNING, "Can't allocate %d bytes for key buffer in yp_cat()");
			}
		}

		return 0;
	}

	if (err != YPERR_NOMORE)
	{
		TSRMLS_FETCH();

		YP(error) = err;
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", yperr_string (err));
	}

	return 0;
}
/* }}} */

/* {{{ proto array yp_cat(string domain, string map)
   Return an array containing the entire map */
PHP_FUNCTION(yp_cat)
{
	pval **domain, **map;
	struct ypall_callback callback;

	if((ZEND_NUM_ARGS() != 2) || zend_get_parameters_ex(2,&domain,&map) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(domain);
	convert_to_string_ex(map);

	array_init(return_value);

	callback.foreach = php_foreach_cat;
	callback.data = (char *) return_value;

	yp_all(Z_STRVAL_PP(domain),Z_STRVAL_PP(map),&callback);
}
/* }}} */

/* {{{ proto int yp_errno()
   Returns the error code from the last call or 0 if no error occured */
PHP_FUNCTION(yp_errno)
{
	if((ZEND_NUM_ARGS() != 0)) {
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG (YP(error));
}
/* }}} */

/* {{{ proto string yp_err_string(int errorcode)
   Returns the corresponding error string for the given error code */
PHP_FUNCTION(yp_err_string)
{
	pval **error;
	char *string;

	if((ZEND_NUM_ARGS() != 1) || zend_get_parameters_ex(1,&error) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(error);

	if((string = yperr_string(Z_LVAL_PP(error))) == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRING(string,1);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(yp)
{
#ifdef ZTS
	ts_allocate_id(&yp_globals_id, sizeof(php_yp_globals), NULL, NULL);
#endif

	REGISTER_LONG_CONSTANT("YPERR_BADARGS", YPERR_BADARGS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_BADDB", YPERR_BADDB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_BUSY", YPERR_BUSY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_DOMAIN", YPERR_DOMAIN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_KEY", YPERR_KEY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_MAP", YPERR_MAP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_NODOM", YPERR_NODOM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_NOMORE", YPERR_NOMORE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_PMAP", YPERR_PMAP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_RESRC", YPERR_RESRC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_RPC", YPERR_RPC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_YPBIND", YPERR_YPBIND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_YPERR", YPERR_YPERR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_YPSERV", YPERR_YPSERV, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("YPERR_VERS", YPERR_VERS, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

PHP_RINIT_FUNCTION(yp)
{
	YP(error) = 0;
	
	return SUCCESS;
}

PHP_MINFO_FUNCTION(yp)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "YP Support", "enabled");
	php_info_print_table_end();
}
#endif /* HAVE_YP */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
