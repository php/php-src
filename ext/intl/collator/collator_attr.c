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
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_intl.h"
#include "collator_class.h"
#include "collator_convert.h"
#include "collator_attr.h"

#include <unicode/ustring.h>

/* {{{ proto int Collator::getAttribute( int $attr )
 * Get collation attribute value. }}} */
/* {{{ proto int collator_get_attribute( Collator $coll, int $attr )
 * Get collation attribute value.
 */
PHP_FUNCTION( collator_get_attribute )
{
	zend_long attribute, value;

	COLLATOR_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Ol",
		&object, Collator_ce_ptr, &attribute ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"collator_get_attribute: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	COLLATOR_METHOD_FETCH_OBJECT;

	value = ucol_getAttribute( co->ucoll, attribute, COLLATOR_ERROR_CODE_P( co ) );
	COLLATOR_CHECK_STATUS( co, "Error getting attribute value" );

	RETURN_LONG( value );
}
/* }}} */

/* {{{ proto bool Collator::getAttribute( int $attr )
 * Get collation attribute value. }}} */
/* {{{ proto bool collator_set_attribute( Collator $coll, int $attr, int $val )
 * Set collation attribute.
 */
PHP_FUNCTION( collator_set_attribute )
{
	zend_long attribute, value;
	COLLATOR_METHOD_INIT_VARS


	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Oll",
		&object, Collator_ce_ptr, &attribute, &value ) == FAILURE)
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "collator_set_attribute: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	COLLATOR_METHOD_FETCH_OBJECT;

	/* Set new value for the given attribute. */
	ucol_setAttribute( co->ucoll, attribute, value, COLLATOR_ERROR_CODE_P( co ) );
	COLLATOR_CHECK_STATUS( co, "Error setting attribute value" );

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int Collator::getStrength()
 * Returns the current collation strength. }}} */
/* {{{ proto int collator_get_strength(Collator coll)
 * Returns the current collation strength.
 */
PHP_FUNCTION( collator_get_strength )
{
	COLLATOR_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, Collator_ce_ptr ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "collator_get_strength: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	COLLATOR_METHOD_FETCH_OBJECT;

	/* Get current strength and return it. */
	RETURN_LONG( ucol_getStrength( co->ucoll ) );
}
/* }}} */

/* {{{ proto bool Collator::setStrength(int strength)
 * Set the collation strength. }}} */
/* {{{ proto bool collator_set_strength(Collator coll, int strength)
 * Set the collation strength.
 */
PHP_FUNCTION( collator_set_strength )
{
	zend_long strength;

	COLLATOR_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Ol",
		&object, Collator_ce_ptr, &strength ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "collator_set_strength: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	COLLATOR_METHOD_FETCH_OBJECT;

	/* Set given strength. */
	ucol_setStrength( co->ucoll, strength );

	RETURN_TRUE;
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
