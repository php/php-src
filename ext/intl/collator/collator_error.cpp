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

/* {{{ Get collator's last error code. */
U_CFUNC PHP_FUNCTION( collator_get_error_code )
{
	COLLATOR_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, Collator_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object (without resetting its last error code). */
	co = Z_INTL_COLLATOR_P(object);
	if( co == nullptr )
		RETURN_FALSE;

	/* Return collator's last error code. */
	RETURN_LONG( COLLATOR_ERROR_CODE( co ) );
}
/* }}} */

/* {{{ Get text description for collator's last error code. */
U_CFUNC PHP_FUNCTION( collator_get_error_message )
{
	zend_string* message = nullptr;

	COLLATOR_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, Collator_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object (without resetting its last error code). */
	co = Z_INTL_COLLATOR_P( object );
	if( co == nullptr )
		RETURN_FALSE;

	/* Return last error message. */
	message = intl_error_get_message( COLLATOR_ERROR_P( co ) );
	RETURN_STR(message);
}
/* }}} */
