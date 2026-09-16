/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if __cplusplus >= 201703L
#include <string_view>
#include <unicode/unistr.h>
#endif

extern "C" {
#include "php_intl.h"
}
#include "collator_class.h"
#include "collator_convert.h"

#include <unicode/ustring.h>

/* {{{ Get collation attribute value. */
U_CFUNC PHP_FUNCTION( collator_get_attribute )
{
	zend_long attribute, value;

	COLLATOR_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Ol",
		&object, Collator_ce_ptr, &attribute ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	COLLATOR_METHOD_FETCH_OBJECT;

	value = ucol_getAttribute( co->ucoll, static_cast<UColAttribute>(attribute), COLLATOR_ERROR_CODE_P( co ) );
	COLLATOR_CHECK_STATUS( co, "Error getting attribute value" );

	RETURN_LONG( value );
}
/* }}} */

/* {{{ Set collation attribute. */
U_CFUNC PHP_FUNCTION( collator_set_attribute )
{
	zend_long attribute, value;
	COLLATOR_METHOD_INIT_VARS


	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Oll",
		&object, Collator_ce_ptr, &attribute, &value ) == FAILURE)
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	COLLATOR_METHOD_FETCH_OBJECT;

	/* Set new value for the given attribute. */
	ucol_setAttribute( co->ucoll, static_cast<UColAttribute>(attribute), static_cast<UColAttributeValue>(value), COLLATOR_ERROR_CODE_P( co ) );
	COLLATOR_CHECK_STATUS( co, "Error setting attribute value" );

	RETURN_TRUE;
}
/* }}} */

/* {{{ Returns the current collation strength. */
U_CFUNC PHP_FUNCTION( collator_get_strength )
{
	COLLATOR_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, Collator_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	COLLATOR_METHOD_FETCH_OBJECT;

	/* Get current strength and return it. */
	RETURN_LONG( ucol_getStrength( co->ucoll ) );
}
/* }}} */

/* {{{ Set the collation strength. */
U_CFUNC PHP_FUNCTION( collator_set_strength )
{
	zend_long strength;

	COLLATOR_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Ol",
		&object, Collator_ce_ptr, &strength ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	COLLATOR_METHOD_FETCH_OBJECT;

	/* Set given strength. */
	ucol_setStrength( co->ucoll, static_cast<UColAttributeValue>(strength) );

	RETURN_TRUE;
}
/* }}} */
