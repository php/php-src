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
   | Authors: Stig Sæther Bakken <ssb@php.net>                            |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |                                                                      |
   | Collection support by Andy Sautins <asautins@veripost.net>           |
   | Temporary LOB support by David Benson <dbenson@mancala.com>          |
   | ZTS per process OCIPLogon by Harald Radi <harald.radi@nme.at>        |
   |                                                                      |
   | Redesigned by: Antony Dovgal <antony@zend.com>                       |
   |                Andi Gutmans <andi@zend.com>                          |
   |                Wez Furlong <wez@omniti.com>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */



#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_ini.h"

#if HAVE_OCI8

#include "php_oci8.h"
#include "php_oci8_int.h"

/*typedef struct {
	char *callback;
} php_oci_failover_context;*/

/* {{{ callback_fn() 
   OCI TAF callback function, calling userspace function */
sb4 callback_fn(OCISvcCtx *svchp, OCIEnv *envhp, php_oci_connection *fo_ctx, ub4 fo_type, ub4 fo_event)
{
	TSRMLS_FETCH();

	/* Check if userspace callback function was disabled */
	if (!fo_ctx->taf_callback || !strcmp(PHP_OCI_TAF_DISABLE_CALLBACK, fo_ctx->taf_callback)) {
		return 0;
	}

	/* Create zval */
	zval *retval, *callback, *params[3];
	MAKE_STD_ZVAL(retval);
	MAKE_STD_ZVAL(callback);
	MAKE_STD_ZVAL(params[0]);
	MAKE_STD_ZVAL(params[1]);
	MAKE_STD_ZVAL(params[2]);

	/* Initialize zval */
	ZVAL_STRING(callback, fo_ctx->taf_callback, 1);
	ZVAL_RESOURCE(params[0], fo_ctx->id);
	ZVAL_LONG(params[1], fo_event);
	ZVAL_LONG(params[2], fo_type);

	/* Call user function (if possible) */
	if (call_user_function(EG(function_table), NULL, callback, retval, 3, params TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call taf callback function, is it defined?");
	}

	/* Return value */
	int returnValue = (int) Z_LVAL_P(retval);

	/* Setting params[0] to null so ressource isn't destroyed on zval_dtor */
	ZVAL_NULL(params[0]);

	/* Cleanup */
	zval_dtor(callback);
	zval_dtor(retval);
	zval_dtor(params[0]);
	zval_dtor(params[1]);
	zval_dtor(params[2]);
	return returnValue;
}
/* }}} */

/* {{{ php_oci_disable_taf_callback()
   Disables the userspace callback function for Oracle TAF,
   while keeping the OCI callback alive */
int php_oci_disable_taf_callback(php_oci_connection *connection TSRMLS_DC)
{
	return php_oci_register_taf_callback(connection, NULL TSRMLS_CC);
}
/* }}} */

/* {{{ php_oci_register_taf_callback()
   Register a callback function for Oracle TAF */
int php_oci_register_taf_callback(php_oci_connection *connection, char *callback TSRMLS_DC)
{
	sword errstatus;
	char *oldCallback = NULL;

	if (!callback) {
		/* Disable callback */
		if (!connection->taf_callback || !strcmp(PHP_OCI_TAF_DISABLE_CALLBACK, connection->taf_callback)) {
			return 0; // Nothing to disable
		}

		oldCallback = connection->taf_callback;
		callback = PHP_OCI_TAF_DISABLE_CALLBACK;
	} else if (connection->taf_callback) {
		/* Overwriting old callback */
		oldCallback = connection->taf_callback;
	}

	/* Set userspace callback function */
	connection->taf_callback = pestrdup(callback, connection->is_persistent);

	/* OCI callback function already registered */
	if (oldCallback) {
		pefree(oldCallback, connection->is_persistent);
		return 0;
	}

	/* temporary failover callback structure */
	OCIFocbkStruct failover;
	/* set context */
	failover.fo_ctx = connection;

	/* set callback function */
	failover.callback_function = &callback_fn;

	/* do the registration */
	PHP_OCI_CALL_RETURN(errstatus, OCIAttrSet, (connection->server, (ub4) OCI_HTYPE_SERVER, (void *) &failover, (ub4) 0, (ub4) OCI_ATTR_FOCBK, connection->err));

	if (errstatus  != OCI_SUCCESS) {
		pefree(connection->taf_callback, connection->is_persistent);
		connection->errcode = php_oci_error(connection->err, errstatus TSRMLS_CC);
		return 2;
	}

	/* successful conclusion */
	return 0;
}
/* }}} */

#endif /* HAVE_OCI8 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
