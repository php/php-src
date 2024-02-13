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
   | Author: Pierrick Charron <pierrick@php.net>                          |
   +----------------------------------------------------------------------+
*/

#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"

#include "curl_private.h"

#include <curl/curl.h>

#define SAVE_CURLSH_ERROR(__handle, __err) (__handle)->err.no = (int) __err;

/* {{{ curlm_throw_last_error */
void curlsh_throw_last_error(php_curlsh *sh, const char *unknown_error_msg) {
    if (sh->err.no) {
        const char* str = curl_share_strerror(sh->err.no);
        if (str) {
            zend_throw_exception(curl_share_exception_ce, str, sh->err.no);
            return;
        }
    }

    zend_throw_exception(curl_share_exception_ce, unknown_error_msg, sh->err.no);
}
/* }}} */

/* {{{ Initialize a share curl handle */
PHP_FUNCTION(curl_share_init)
{
	ZEND_PARSE_PARAMETERS_NONE();
	object_init_ex(return_value, curl_share_ce);
}
/* }}} */

/* {{{ Close a set of cURL handles */
PHP_FUNCTION(curl_share_close)
{
	zval *z_sh;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_OBJECT_OF_CLASS(z_sh, curl_share_ce)
	ZEND_PARSE_PARAMETERS_END();
}
/* }}} */

static bool _php_curl_share_setopt(php_curlsh *sh, int argnum, zend_long option, zval *zvalue, zval *return_value) /* {{{ */
{
	CURLSHcode error = CURLSHE_OK;

	switch (option) {
		case CURLSHOPT_SHARE:
		case CURLSHOPT_UNSHARE:
			error = curl_share_setopt(sh->share, option, zval_get_long(zvalue));
			break;

		default:
			zend_argument_value_error(argnum, "is not a valid cURL share option");
			error = CURLSHE_BAD_OPTION;
			break;
	}

	SAVE_CURLSH_ERROR(sh, error);

	return error == CURLSHE_OK;
}
/* }}} */

/* {{{ Set an option for a cURL transfer */
PHP_FUNCTION(curl_share_setopt)
{
	zval       *z_sh, *zvalue;
	zend_long        options;
	php_curlsh *sh;

	ZEND_PARSE_PARAMETERS_START(3,3)
		Z_PARAM_OBJECT_OF_CLASS(z_sh, curl_share_ce)
		Z_PARAM_LONG(options)
		Z_PARAM_ZVAL(zvalue)
	ZEND_PARSE_PARAMETERS_END();

	sh = Z_CURL_SHARE_P(z_sh);

	if (_php_curl_share_setopt(sh, 2, options, zvalue, return_value)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

PHP_METHOD(CurlShareHandle, setOpt)
{
	php_curlsh *sh = Z_CURL_SHARE_P(getThis());
	zval       *zvalue;
	zend_long   options;

	ZEND_PARSE_PARAMETERS_START(2,2)
		Z_PARAM_LONG(options)
		Z_PARAM_ZVAL(zvalue)
	ZEND_PARSE_PARAMETERS_END();

	if (!_php_curl_share_setopt(sh, 1, options, zvalue, return_value)) {
		if (!EG(exception)) {
			curlsh_throw_last_error(sh, "Failed setting cURL share option");
		}
		RETURN_THROWS();
	}

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/* {{{ Return an integer containing the last share curl error number */
PHP_FUNCTION(curl_share_errno)
{
	zval        *z_sh;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &z_sh, curl_share_ce) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(Z_CURL_SHARE_P(z_sh)->err.no);
}
/* }}} */

/* {{{ Return a string containing the last share curl error message */
PHP_FUNCTION(curl_share_error)
{
	zval        *z_sh;
	const char  *str;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &z_sh, curl_share_ce) == FAILURE) {
		RETURN_THROWS();
	}

	str = curl_share_strerror(Z_CURL_SHARE_P(z_sh)->err.no);
	if (str) {
		RETURN_STRING(str);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ return string describing error code */
PHP_FUNCTION(curl_share_strerror)
{
	zend_long code;
	const char *str;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_LONG(code)
	ZEND_PARSE_PARAMETERS_END();

	str = curl_share_strerror(code);
	if (str) {
		RETURN_STRING(str);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* CurlShareHandle class */

static zend_object *curl_share_create_object(zend_class_entry *class_type) {
	php_curlsh *intern = zend_object_alloc(sizeof(php_curlsh), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->share = curl_share_init();

	return &intern->std;
}

void curl_share_free_obj(zend_object *object)
{
	php_curlsh *sh = curl_share_from_obj(object);

	curl_share_cleanup(sh->share);
	zend_object_std_dtor(&sh->std);
}

static zend_object_handlers curl_share_handlers;

void curl_share_register_handlers(void) {
	curl_share_ce->create_object = curl_share_create_object;
	curl_share_ce->default_object_handlers = &curl_share_handlers;

	memcpy(&curl_share_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	curl_share_handlers.offset = XtOffsetOf(php_curlsh, std);
	curl_share_handlers.free_obj = curl_share_free_obj;
	curl_share_handlers.clone_obj = NULL;
	curl_share_handlers.compare = zend_objects_not_comparable;
}
