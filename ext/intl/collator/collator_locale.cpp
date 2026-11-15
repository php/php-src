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

#include "php_intl.h"
#include "collator_class.h"
#include "intl_convert.h"

#include <zend_API.h>

/* {{{ Gets the locale name of the collator. */
U_CFUNC PHP_FUNCTION( collator_get_locale )
{
	zend_long   type        = 0;
	char*  locale_name = nullptr;

	COLLATOR_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Ol",
		&object, Collator_ce_ptr, &type ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	COLLATOR_METHOD_FETCH_OBJECT;

	if (collator_check_initialized(co) == FAILURE) {
		RETURN_THROWS();
	}

	/* Get locale by specified type. */
	locale_name = (char*) ucol_getLocaleByType(
		co->ucoll, static_cast<ULocDataLocaleType>(type), COLLATOR_ERROR_CODE_P( co ) );
	COLLATOR_CHECK_STATUS( co, "Error getting locale by type" );

	/* Return it. */
	RETVAL_STRINGL( locale_name, strlen(locale_name) );
}
/* }}} */

/* {{{ Gets the collation tailoring rules. */
U_CFUNC PHP_FUNCTION(collator_get_rules)
{
	COLLATOR_METHOD_INIT_VARS

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O",
			&object, Collator_ce_ptr) == FAILURE) {
		RETURN_THROWS();
	}

	COLLATOR_METHOD_FETCH_OBJECT;

	if (collator_check_initialized(co) == FAILURE) {
		RETURN_THROWS();
	}

	int32_t rules_len;
	const UChar *rules = ucol_getRules(co->ucoll, &rules_len);
	zend_string *result = intl_convert_utf16_to_utf8(rules, rules_len, COLLATOR_ERROR_CODE_P(co));
	COLLATOR_CHECK_STATUS(co, "Error converting collation rules to UTF-8");

	RETURN_STR(result);
}
/* }}} */
