/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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
   |                Andi Gutmans <andi@php.net>                           |
   |                Wez Furlong <wez@omniti.com>                          |
   +----------------------------------------------------------------------+
*/



#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_ini.h"

#if HAVE_OCI8

#include "php_oci8.h"
#include "php_oci8_int.h"

/* {{{ callback_fn()
   OCI TAF callback function, calling userspace function */
sb4 callback_fn(void *svchp, void *envhp, void *fo_ctx, ub4 fo_type, ub4 fo_event)
{
	/* Create zval */
	zval retval, params[3];
    php_oci_connection *connection = (php_oci_connection*)fo_ctx;

	/* Default return value */
	sb4 returnValue = 0;

	/* Check if userspace callback function was unregistered */
	if (Z_ISUNDEF(connection->taf_callback) || Z_ISNULL(connection->taf_callback)) {
		return 0;
	}

	/* Initialize zval */
	ZVAL_RES(&params[0], connection->id);
	ZVAL_LONG(&params[1], fo_event);
	ZVAL_LONG(&params[2], fo_type);

	/* Call user function (if possible) */
	if (call_user_function(EG(function_table), NULL, &connection->taf_callback, &retval, 3, params) == FAILURE) {
		php_error_docref(NULL, E_WARNING, "Unable to call Oracle TAF callback function");
	}

	/* Set return value */
	if (Z_TYPE(retval) == IS_LONG) {
		returnValue = (sb4) Z_LVAL(retval);
	}

	/* Setting params[0] to null so resource isn't destroyed on zval_dtor */
	ZVAL_NULL(&params[0]);

	/* Cleanup */
	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&params[0]);
	zval_ptr_dtor(&params[1]);
	zval_ptr_dtor(&params[2]);

	return returnValue;
}
/* }}} */

/* {{{ php_oci_unregister_taf_callback()
   Unregister the userspace callback function for Oracle TAF,
   while keeping the OCI callback alive */
int php_oci_unregister_taf_callback(php_oci_connection *connection)
{
	return php_oci_register_taf_callback(connection, NULL);
}
/* }}} */

/* {{{ php_oci_register_taf_callback()
   Register a callback function for Oracle TAF */
int php_oci_register_taf_callback(php_oci_connection *connection, zval *callback)
{
	sword errstatus;
	int registered = 0;

	/* temporary failover callback structure */
	OCIFocbkStruct failover;

	if (!callback) {
		/* Unregister callback */
		if (Z_ISUNDEF(connection->taf_callback) || Z_ISNULL(connection->taf_callback)) {
			return 0; // Nothing to unregister
		}

		registered = 1;
		zval_ptr_dtor(&connection->taf_callback);
		ZVAL_NULL(&connection->taf_callback);
	} else {
		if (!Z_ISUNDEF(connection->taf_callback)) {
			registered = 1;
			if (!Z_ISNULL(connection->taf_callback)) {
				zval_ptr_dtor(&connection->taf_callback);
				ZVAL_NULL(&connection->taf_callback);
			}
		}

		/* Set userspace callback function */
		ZVAL_COPY(&connection->taf_callback, callback);
	}

	/* OCI callback function already registered */
	if (registered) {
		return 0;
	}

	/* set context */
	failover.fo_ctx = connection;

	/* set callback function */
	failover.callback_function = &callback_fn;

	/* do the registration */
	PHP_OCI_CALL_RETURN(errstatus, OCIAttrSet, (connection->server, (ub4) OCI_HTYPE_SERVER, (void *) &failover, (ub4) 0, (ub4) OCI_ATTR_FOCBK, connection->err));

	if (errstatus  != OCI_SUCCESS) {
		zval_ptr_dtor(&connection->taf_callback);
		ZVAL_UNDEF(&connection->taf_callback);
		connection->errcode = php_oci_error(connection->err, errstatus);
		return 1;
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
