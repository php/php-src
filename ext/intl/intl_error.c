/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
#include "intl_convert.h"

ZEND_EXTERN_MODULE_GLOBALS( intl )

zend_class_entry *IntlException_ce_ptr;

/* {{{ intl_error* intl_g_error_get()
 * Return global error structure.
 */
static intl_error* intl_g_error_get( void )
{
	return &INTL_G( g_error );
}
/* }}} */

/* {{{ void intl_free_custom_error_msg( intl_error* err )
 * Free mem.
 */
static void intl_free_custom_error_msg( intl_error* err )
{
	if( !err && !( err = intl_g_error_get(  ) ) )
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
intl_error* intl_error_create( void )
{
	intl_error* err = ecalloc( 1, sizeof( intl_error ) );

	intl_error_init( err );

	return err;
}
/* }}} */

/* {{{ void intl_error_init( intl_error* coll_error )
 * Initialize internals of 'intl_error'.
 */
void intl_error_init( intl_error* err )
{
	if( !err && !( err = intl_g_error_get(  ) ) )
		return;

	err->code                      = U_ZERO_ERROR;
	err->custom_error_message      = NULL;
	err->free_custom_error_message = 0;
}
/* }}} */

/* {{{ void intl_error_reset( intl_error* err )
 * Set last error code to 0 and unset last error message
 */
void intl_error_reset( intl_error* err )
{
	if( !err && !( err = intl_g_error_get(  ) ) )
		return;

	err->code = U_ZERO_ERROR;

	intl_free_custom_error_msg( err );
}
/* }}} */

/* {{{ void intl_error_set_custom_msg( intl_error* err, char* msg, int copyMsg )
 * Set last error message to msg copying it if needed.
 */
void intl_error_set_custom_msg( intl_error* err, const char* msg, int copyMsg )
{
	if( !msg )
		return;

	if( !err ) {
		if( INTL_G( error_level ) )
			php_error_docref( NULL, INTL_G( error_level ), "%s", msg );
		if( INTL_G( use_exceptions ) )
			zend_throw_exception_ex( IntlException_ce_ptr, 0, "%s", msg );
	}
	if( !err && !( err = intl_g_error_get(  ) ) )
		return;

	/* Free previous message if any */
	intl_free_custom_error_msg( err );

	/* Mark message copied if any */
	err->free_custom_error_message = copyMsg;

	/* Set user's error text message */
	err->custom_error_message = copyMsg ? estrdup( msg ) : (char *) msg;
}
/* }}} */

/* {{{ const char* intl_error_get_message( intl_error* err )
 * Create output message in format "<intl_error_text>: <extra_user_error_text>".
 */
zend_string * intl_error_get_message( intl_error* err )
{
	const char *uErrorName = NULL;
	zend_string *errMessage = 0;

	if( !err && !( err = intl_g_error_get(  ) ) )
		return ZSTR_EMPTY_ALLOC();

	uErrorName = u_errorName( err->code );

	/* Format output string */
	if( err->custom_error_message )
	{
		errMessage = strpprintf(0, "%s: %s", err->custom_error_message, uErrorName );
	}
	else
	{
		errMessage = strpprintf(0, "%s", uErrorName );
	}

	return errMessage;
}
/* }}} */

/* {{{ void intl_error_set_code( intl_error* err, UErrorCode err_code )
 * Set last error code.
 */
void intl_error_set_code( intl_error* err, UErrorCode err_code )
{
	if( !err && !( err = intl_g_error_get(  ) ) )
		return;

	err->code = err_code;
}
/* }}} */

/* {{{ void intl_error_get_code( intl_error* err )
 * Return last error code.
 */
UErrorCode intl_error_get_code( intl_error* err )
{
	if( !err && !( err = intl_g_error_get(  ) ) )
		return U_ZERO_ERROR;

	return err->code;
}
/* }}} */

/* {{{ void intl_error_set( intl_error* err, UErrorCode code, char* msg, int copyMsg )
 * Set error code and message.
 */
void intl_error_set( intl_error* err, UErrorCode code, const char* msg, int copyMsg )
{
	intl_error_set_code( err, code );
	intl_error_set_custom_msg( err, msg, copyMsg );
}
/* }}} */

/* {{{ void intl_errors_set( intl_error* err, UErrorCode code, char* msg, int copyMsg )
 * Set error code and message.
 */
void intl_errors_set( intl_error* err, UErrorCode code, const char* msg, int copyMsg )
{
	intl_errors_set_code( err, code );
	intl_errors_set_custom_msg( err, msg, copyMsg );
}
/* }}} */

/* {{{ void intl_errors_reset( intl_error* err )
 */
void intl_errors_reset( intl_error* err )
{
	if(err) {
		intl_error_reset( err );
	}
	intl_error_reset( NULL );
}
/* }}} */

/* {{{ void intl_errors_set_custom_msg( intl_error* err, char* msg, int copyMsg )
 */
void intl_errors_set_custom_msg( intl_error* err, const char* msg, int copyMsg )
{
	if(err) {
		intl_error_set_custom_msg( err, msg, copyMsg );
	}
	intl_error_set_custom_msg( NULL, msg, copyMsg );
}
/* }}} */

/* {{{ intl_errors_set_code( intl_error* err, UErrorCode err_code )
 */
void intl_errors_set_code( intl_error* err, UErrorCode err_code )
{
	if(err) {
		intl_error_set_code( err, err_code );
	}
	intl_error_set_code( NULL, err_code );
}
/* }}} */

void intl_register_IntlException_class( void )
{
	zend_class_entry ce;
	
	/* Create and register 'IntlException' class. */
	INIT_CLASS_ENTRY_EX( ce, "IntlException", sizeof( "IntlException" ) - 1, NULL );
	IntlException_ce_ptr = zend_register_internal_class_ex( &ce,
		zend_ce_exception );
	IntlException_ce_ptr->create_object = zend_ce_exception->create_object;
}

smart_str intl_parse_error_to_string( UParseError* pe )
{
	smart_str    ret = {0};
	zend_string *u8str;
	UErrorCode   status;
	int          any = 0;

	assert( pe != NULL );

	smart_str_appends( &ret, "parse error " );
	if( pe->line > 0 )
	{
		smart_str_appends( &ret, "on line " );
		smart_str_append_long( &ret, (zend_long ) pe->line );
		any = 1;
	}
	if( pe->offset >= 0 ) {
		if( any )
			smart_str_appends( &ret, ", " );
		else
			smart_str_appends( &ret, "at " );

		smart_str_appends( &ret, "offset " );
		smart_str_append_long( &ret, (zend_long ) pe->offset );
		any = 1;
	}

	if (pe->preContext[0] != 0 ) {
		if( any )
			smart_str_appends( &ret, ", " );

		smart_str_appends( &ret, "after \"" );
		u8str = intl_convert_utf16_to_utf8(pe->preContext, -1, &status );
		if( !u8str )
		{
			smart_str_appends( &ret, "(could not convert parser error pre-context to UTF-8)" );
		}
		else {
			smart_str_append( &ret, u8str );
			zend_string_release( u8str );
		}
		smart_str_appends( &ret, "\"" );
		any = 1;
	}

	if( pe->postContext[0] != 0 )
	{
		if( any )
			smart_str_appends( &ret, ", " );

		smart_str_appends( &ret, "before or at \"" );
		u8str = intl_convert_utf16_to_utf8(pe->postContext, -1, &status );
		if( !u8str )
		{
			smart_str_appends( &ret, "(could not convert parser error post-context to UTF-8)" );
		}
		else
		{
			smart_str_append( &ret, u8str );
			zend_string_release( u8str );
		}
		smart_str_appends( &ret, "\"" );
		any = 1;
	}

	if( !any )
	{
		smart_str_free( &ret );
		smart_str_appends( &ret, "no parse error" );
	}

	smart_str_0( &ret );
	return ret;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
