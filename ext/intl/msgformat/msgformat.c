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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unicode/ustring.h>
#include <unicode/umsg.h>

#include "php_intl.h"
#include "msgformat_class.h"
#include "intl_data.h"

/* {{{ proto MessageFormatter MesssageFormatter::create( string $locale, string $pattern )
 * Create formatter. }}} */
/* {{{ proto MessageFormatter msgfmt_create( string $locale, string $pattern )
 * Create formatter.
 */
PHP_FUNCTION( msgfmt_create )
{
	char*       locale;
	int         locale_len = 0;
	UChar*      spattern     = NULL;
	int         spattern_len = 0;
	zval*       object;
	int free_pattern = 0;
	MessageFormatter_object* mfo;

	intl_error_reset( NULL TSRMLS_CC );

	// Parse parameters.
	if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "su",
		&locale, &locale_len, &spattern, &spattern_len ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"msgfmt_create: unable to parse input parameters", 0 TSRMLS_CC );

		RETURN_NULL();
	}

	INTL_CHECK_LOCALE_LEN(locale_len);
	// Create a MessageFormatter object and save the ICU formatter into it.
	if( ( object = getThis() ) == NULL )
		object = return_value;

	if( Z_TYPE_P( object ) != IS_OBJECT )
		object_init_ex( object, MessageFormatter_ce_ptr );

	MSG_FORMAT_METHOD_FETCH_OBJECT;

	// Convert pattern (if specified) to UTF-16.
	if(locale_len == 0) {
		locale = UG(default_locale);
	}

	(mfo)->mf_data.orig_format = eustrndup(spattern, spattern_len);
	(mfo)->mf_data.orig_format_len = spattern_len;
	
	if(msfgotmat_fix_quotes(&spattern, &spattern_len, &INTL_DATA_ERROR_CODE(mfo), &free_pattern) != SUCCESS) {
		intl_error_set( NULL, U_INVALID_FORMAT_ERROR,
			"msgfmt_create: error converting pattern to quote-friendly format", 0 TSRMLS_CC );
		zval_dtor(return_value);
		RETURN_NULL();
	}

	// Create an ICU message formatter.
	MSG_FORMAT_OBJECT(mfo) = umsg_open(spattern, spattern_len, locale, NULL, &INTL_DATA_ERROR_CODE(mfo));
	if(free_pattern) {
		efree(spattern);
	}

	if( U_FAILURE( INTL_DATA_ERROR_CODE((mfo)) ) )
	{
		intl_error_set( NULL, INTL_DATA_ERROR_CODE( mfo ),
			"msgfmt_create: message formatter creation failed", 0 TSRMLS_CC );
		zval_dtor(return_value);
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto void MessageFormatter::__construct( string $locale, string $pattern )
 * MessageFormatter object constructor.
 */
PHP_METHOD( MessageFormatter, __construct )
{
	char*       locale;
	int         locale_len;
	UChar*      spattern     = NULL;
	int         spattern_len = 0;
	zval*       object;
	int free_pattern = 0;
	MessageFormatter_object* mfo;

	intl_error_reset( NULL TSRMLS_CC );

	object = getThis();

	// Parse parameters.
	if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "su",
		&locale, &locale_len, &spattern, &spattern_len ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"__construct: unable to parse input params", 0 TSRMLS_CC );
		zval_dtor(object);
		ZVAL_NULL(object);
		RETURN_NULL();
	}

	INTL_CHECK_LOCALE_LEN_OBJ(locale_len, object);
	mfo = (MessageFormatter_object *) zend_object_store_get_object( object TSRMLS_CC );

	intl_error_reset( &mfo->mf_data.error TSRMLS_CC );

	if(locale_len == 0) {
		locale = UG(default_locale);
	}

	(mfo)->mf_data.orig_format = eustrndup(spattern, spattern_len);
	(mfo)->mf_data.orig_format_len = spattern_len;
	
	if(msfgotmat_fix_quotes(&spattern, &spattern_len, &INTL_DATA_ERROR_CODE(mfo), &free_pattern) != SUCCESS) {
		intl_error_set( NULL, U_INVALID_FORMAT_ERROR,
			"__construct: error converting pattern to quote-friendly format", 0 TSRMLS_CC );
		zval_dtor(object);
		ZVAL_NULL(object);
		RETURN_NULL();
	}

	// Create an ICU message formatter.
	MSG_FORMAT_OBJECT(mfo) = umsg_open(spattern, spattern_len, locale, NULL, &INTL_DATA_ERROR_CODE(mfo));
	if(free_pattern) {
		efree(spattern);
	}

	if( U_FAILURE( INTL_DATA_ERROR_CODE((mfo)) ) )
	{
		intl_error_set( NULL, INTL_DATA_ERROR_CODE(mfo),
			"__construct: message formatter creation failed", 0 TSRMLS_CC );
		zval_dtor(object);
		ZVAL_NULL(object);
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto int MessageFormatter::getErrorCode()
 * Get formatter's last error code. }}} */
/* {{{ proto int msgfmt_get_error_code( MessageFormatter $nf )
 * Get formatter's last error code.
 */
PHP_FUNCTION( msgfmt_get_error_code )
{
	zval*                    object  = NULL;
	MessageFormatter_object*  mfo     = NULL;

	// Parse parameters.
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",
		&object, MessageFormatter_ce_ptr ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"msgfmt_get_error_code: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	mfo = (MessageFormatter_object *) zend_object_store_get_object( object TSRMLS_CC );

	// Return formatter's last error code.
	RETURN_LONG( INTL_DATA_ERROR_CODE(mfo) );
}
/* }}} */

/* {{{ proto string MessageFormatter::getErrorMessage( )
 * Get text description for formatter's last error code. }}} */
/* {{{ proto string msgfmt_get_error_message( MessageFormatter $coll )
 * Get text description for formatter's last error code.
 */
PHP_FUNCTION( msgfmt_get_error_message )
{
	char*                    message = NULL;
	zval*                    object  = NULL;
	MessageFormatter_object*  mfo     = NULL;

	// Parse parameters.
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",
		&object, MessageFormatter_ce_ptr ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"msgfmt_get_error_message: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	mfo = (MessageFormatter_object *) zend_object_store_get_object( object TSRMLS_CC );

	// Return last error message.
	message = intl_error_get_message( &mfo->mf_data.error TSRMLS_CC );
	RETURN_STRING( message, 0);
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
