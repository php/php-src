/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   |          Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <zend_exceptions.h>

#include "php_intl.h"
#include "intl_error.h"

ZEND_EXTERN_MODULE_GLOBALS( intl )

static zend_class_entry *IntlException_ce_ptr;

/* {{{ intl_error* intl_g_error_get()
 * Return global error structure.
 */
static intl_error* intl_g_error_get( TSRMLS_D )
{
	return &INTL_G( g_error );
}
/* }}} */

/* {{{ void intl_free_custom_error_msg( intl_error* err )
 * Free mem.
 */
static void intl_free_custom_error_msg( intl_error* err TSRMLS_DC )
{
	if( !err && !( err = intl_g_error_get( TSRMLS_C ) ) )
		return;

	if(err->free_custom_error_message ) {
		efree( err->custom_error_message );
	}

	err->custom_error_message      = NULL;
	err->free_custom_error_message = 0;
}
/* }}} */

/* {{{ intl_error* intl_error_create()
 * Create and initialize  internals of 'intl_error'.
 */
intl_error* intl_error_create( TSRMLS_D )
{
	intl_error* err = ecalloc( 1, sizeof( intl_error ) );

	intl_error_init( err TSRMLS_CC );

	return err;
}
/* }}} */

/* {{{ void intl_error_init( intl_error* coll_error )
 * Initialize internals of 'intl_error'.
 */
void intl_error_init( intl_error* err TSRMLS_DC )
{
	if( !err && !( err = intl_g_error_get( TSRMLS_C ) ) )
		return;

	err->code                      = U_ZERO_ERROR;
	err->custom_error_message      = NULL;
	err->free_custom_error_message = 0;
}
/* }}} */

/* {{{ void intl_error_reset( intl_error* err )
 * Set last error code to 0 and unset last error message
 */
void intl_error_reset( intl_error* err TSRMLS_DC )
{
	if( !err && !( err = intl_g_error_get( TSRMLS_C ) ) )
		return;

	err->code = U_ZERO_ERROR;

	intl_free_custom_error_msg( err TSRMLS_CC );
}
/* }}} */

/* {{{ void intl_error_set_custom_msg( intl_error* err, char* msg, int copyMsg )
 * Set last error message to msg copying it if needed.
 */
void intl_error_set_custom_msg( intl_error* err, char* msg, int copyMsg TSRMLS_DC )
{
	if( !msg )
		return;

	if( !err ) {
		if( INTL_G( error_level ) )
			php_error_docref( NULL TSRMLS_CC, INTL_G( error_level ), "%s", msg );
		if( INTL_G( use_exceptions ) )
			zend_throw_exception_ex( IntlException_ce_ptr, 0 TSRMLS_CC, "%s", msg );
	}
	if( !err && !( err = intl_g_error_get( TSRMLS_C ) ) )
		return;

	/* Free previous message if any */
	intl_free_custom_error_msg( err TSRMLS_CC );

	/* Mark message copied if any */
	err->free_custom_error_message = copyMsg;

	/* Set user's error text message */
	err->custom_error_message = copyMsg ? estrdup( msg ) : msg;
}
/* }}} */

/* {{{ const char* intl_error_get_message( intl_error* err )
 * Create output message in format "<intl_error_text>: <extra_user_error_text>".
 */
char* intl_error_get_message( intl_error* err TSRMLS_DC )
{
	const char* uErrorName = NULL;
	char*       errMessage = 0;

	if( !err && !( err = intl_g_error_get( TSRMLS_C ) ) )
		return estrdup( "" );

	uErrorName = u_errorName( err->code );

	/* Format output string */
	if( err->custom_error_message )
	{
		spprintf( &errMessage, 0, "%s: %s", err->custom_error_message, uErrorName );
	}
	else
	{
		spprintf( &errMessage, 0, "%s", uErrorName );
	}

	return errMessage;
}
/* }}} */

/* {{{ void intl_error_set_code( intl_error* err, UErrorCode err_code )
 * Set last error code.
 */
void intl_error_set_code( intl_error* err, UErrorCode err_code TSRMLS_DC )
{
	if( !err && !( err = intl_g_error_get( TSRMLS_C ) ) )
		return;

	err->code = err_code;
}
/* }}} */

/* {{{ void intl_error_get_code( intl_error* err )
 * Return last error code.
 */
UErrorCode intl_error_get_code( intl_error* err TSRMLS_DC )
{
	if( !err && !( err = intl_g_error_get( TSRMLS_C ) ) )
		return U_ZERO_ERROR;

	return err->code;
}
/* }}} */

/* {{{ void intl_error_set( intl_error* err, UErrorCode code, char* msg, int copyMsg )
 * Set error code and message.
 */
void intl_error_set( intl_error* err, UErrorCode code, char* msg, int copyMsg TSRMLS_DC )
{
	intl_error_set_code( err, code TSRMLS_CC );
	intl_error_set_custom_msg( err, msg, copyMsg TSRMLS_CC );
}
/* }}} */

/* {{{ void intl_errors_set( intl_error* err, UErrorCode code, char* msg, int copyMsg )
 * Set error code and message.
 */
void intl_errors_set( intl_error* err, UErrorCode code, char* msg, int copyMsg TSRMLS_DC )
{
	intl_errors_set_code( err, code TSRMLS_CC );
	intl_errors_set_custom_msg( err, msg, copyMsg TSRMLS_CC );
}
/* }}} */

/* {{{ void intl_errors_reset( intl_error* err )
 */
void intl_errors_reset( intl_error* err TSRMLS_DC )
{
	if(err) {
		intl_error_reset( err TSRMLS_CC );
	}
	intl_error_reset( NULL TSRMLS_CC );
}
/* }}} */

/* {{{ void intl_errors_set_custom_msg( intl_error* err, char* msg, int copyMsg )
 */
void intl_errors_set_custom_msg( intl_error* err, char* msg, int copyMsg TSRMLS_DC )
{
	if(err) {
		intl_error_set_custom_msg( err, msg, copyMsg TSRMLS_CC );
	}
	intl_error_set_custom_msg( NULL, msg, copyMsg TSRMLS_CC );
}
/* }}} */

/* {{{ intl_errors_set_code( intl_error* err, UErrorCode err_code )
 */
void intl_errors_set_code( intl_error* err, UErrorCode err_code TSRMLS_DC )
{
	if(err) {
		intl_error_set_code( err, err_code TSRMLS_CC );
	}
	intl_error_set_code( NULL, err_code TSRMLS_CC );
}
/* }}} */

void intl_register_IntlException_class( TSRMLS_D )
{
	zend_class_entry ce,
					 *default_exception_ce;

	default_exception_ce = zend_exception_get_default( TSRMLS_C );

	/* Create and register 'IntlException' class. */
	INIT_CLASS_ENTRY_EX( ce, "IntlException", sizeof( "IntlException" ) - 1, NULL );
	IntlException_ce_ptr = zend_register_internal_class_ex( &ce,
		default_exception_ce, NULL TSRMLS_CC );
	IntlException_ce_ptr->create_object = default_exception_ce->create_object;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
