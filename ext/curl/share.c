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
   | Author: Pierrick Charron <pierrick@php.net>                          |
   +----------------------------------------------------------------------+
*/

#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_CURL

#include "php_curl.h"

#include <curl/curl.h>

#define SAVE_CURLSH_ERROR(__handle, __err) (__handle)->err.no = (int) __err;

/* {{{ proto void curl_share_init()
   Initialize a share curl handle */
PHP_FUNCTION(curl_share_init)
{
	php_curlsh *sh;

	ZEND_PARSE_PARAMETERS_NONE();

	sh = ecalloc(1, sizeof(php_curlsh));

	sh->share = curl_share_init();

	RETURN_RES(zend_register_resource(sh, le_curl_share_handle));
}
/* }}} */

/* {{{ proto void curl_share_close(resource sh)
   Close a set of cURL handles */
PHP_FUNCTION(curl_share_close)
{
	zval *z_sh;
	php_curlsh *sh;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_RESOURCE(z_sh)
	ZEND_PARSE_PARAMETERS_END();

	if ((sh = (php_curlsh *)zend_fetch_resource(Z_RES_P(z_sh), le_curl_share_handle_name, le_curl_share_handle)) == NULL) {
		RETURN_THROWS();
	}

	zend_list_close(Z_RES_P(z_sh));
}
/* }}} */

static int _php_curl_share_setopt(php_curlsh *sh, zend_long option, zval *zvalue, zval *return_value) /* {{{ */
{
	CURLSHcode error = CURLSHE_OK;

	switch (option) {
		case CURLSHOPT_SHARE:
		case CURLSHOPT_UNSHARE:
			error = curl_share_setopt(sh->share, option, zval_get_long(zvalue));
			break;

		default:
			php_error_docref(NULL, E_WARNING, "Invalid curl share configuration option");
			error = CURLSHE_BAD_OPTION;
			break;
	}

	SAVE_CURLSH_ERROR(sh, error);

	return error != CURLSHE_OK;
}
/* }}} */

/* {{{ proto bool curl_share_setopt(resource sh, int option, mixed value)
      Set an option for a cURL transfer */
PHP_FUNCTION(curl_share_setopt)
{
	zval       *zid, *zvalue;
	zend_long        options;
	php_curlsh *sh;

	ZEND_PARSE_PARAMETERS_START(3,3)
		Z_PARAM_RESOURCE(zid)
		Z_PARAM_LONG(options)
		Z_PARAM_ZVAL(zvalue)
	ZEND_PARSE_PARAMETERS_END();

	if ((sh = (php_curlsh *)zend_fetch_resource(Z_RES_P(zid), le_curl_share_handle_name, le_curl_share_handle)) == NULL) {
		RETURN_THROWS();
	}

	if (!_php_curl_share_setopt(sh, options, zvalue, return_value)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

void _php_curl_share_close(zend_resource *rsrc) /* {{{ */
{
	php_curlsh *sh = (php_curlsh *)rsrc->ptr;
	if (sh) {
		curl_share_cleanup(sh->share);
		efree(sh);
		rsrc->ptr = NULL;
	}
}
/* }}} */

/* {{{ proto int curl_share_errno(resource mh)
         Return an integer containing the last share curl error number */
PHP_FUNCTION(curl_share_errno)
{
	zval        *z_sh;
	php_curlsh  *sh;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_RESOURCE(z_sh)
	ZEND_PARSE_PARAMETERS_END();

	if ((sh = (php_curlsh *)zend_fetch_resource(Z_RES_P(z_sh), le_curl_share_handle_name, le_curl_share_handle)) == NULL) {
		RETURN_THROWS();
	}

	RETURN_LONG(sh->err.no);
}
/* }}} */


/* {{{ proto bool curl_share_strerror(int code)
         return string describing error code */
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

#endif
