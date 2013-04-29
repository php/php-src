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
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_intl.h"
#include "collator_class.h"
#include "collator_error.h"

/* {{{ proto int Collator::getErrorCode( Collator $coll )
 * Get collator's last error code. }}} */
/* {{{ proto int collator_get_error_code( Collator $coll )
 * Get collator's last error code.
 */
PHP_FUNCTION( collator_get_error_code )
{
	COLLATOR_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",
		&object, Collator_ce_ptr ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"collator_get_error_code: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	/* Fetch the object (without resetting its last error code). */
	co = (Collator_object *) zend_object_store_get_object(object TSRMLS_CC);
	if( co == NULL )
		RETURN_FALSE;

	/* Return collator's last error code. */
	RETURN_LONG( COLLATOR_ERROR_CODE( co ) );
}
/* }}} */

/* {{{ proto string Collator::getErrorMessage( Collator $coll )
 * Get text description for collator's last error code. }}} */
/* {{{ proto string collator_get_error_message( Collator $coll )
 * Get text description for collator's last error code.
 */
PHP_FUNCTION( collator_get_error_message )
{
	const char* message = NULL;

	COLLATOR_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",
		&object, Collator_ce_ptr ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"collator_get_error_message: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	/* Fetch the object (without resetting its last error code). */
	co = (Collator_object *) zend_object_store_get_object( object TSRMLS_CC );
	if( co == NULL )
		RETURN_FALSE;

	/* Return last error message. */
	message = intl_error_get_message( COLLATOR_ERROR_P( co ) TSRMLS_CC );
	RETURN_STRING( (char*)message, FALSE );
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
