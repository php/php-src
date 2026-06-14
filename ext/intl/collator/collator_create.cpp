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
#include "intl_data.h"

/* {{{ */
static int collator_ctor(INTERNAL_FUNCTION_PARAMETERS)
{
	char*            locale;
	size_t           locale_len = 0;
	zval*            object;
	Collator_object* co;

	intl_error_reset( nullptr );
	object = return_value;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(locale, locale_len)
	ZEND_PARSE_PARAMETERS_END_EX(return FAILURE);

	INTL_CHECK_LOCALE_LEN_OR_FAILURE(locale_len);
	COLLATOR_METHOD_FETCH_OBJECT;

	if(locale_len == 0) {
		locale = (char *)intl_locale_get_default();
	}

	/* Open ICU collator. */
	co->ucoll = ucol_open( locale, COLLATOR_ERROR_CODE_P( co ) );
	INTL_CTOR_CHECK_STATUS(co, "unable to open ICU collator");
	return SUCCESS;
}
/* }}} */

/* {{{ Create collator. */
U_CFUNC PHP_FUNCTION( collator_create )
{
	object_init_ex( return_value, Collator_ce_ptr );
	if (collator_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU) == FAILURE) {
		zval_ptr_dtor(return_value);
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ Collator object constructor. */
U_CFUNC PHP_METHOD( Collator, __construct )
{
	const bool old_use_exception = INTL_G(use_exceptions);
	const zend_long old_error_level = INTL_G(error_level);
	INTL_G(use_exceptions) = true;
	INTL_G(error_level) = 0;

	return_value = ZEND_THIS;
	if (collator_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU) == FAILURE) {
		ZEND_ASSERT(EG(exception));
	}
	INTL_G(use_exceptions) = old_use_exception;
	INTL_G(error_level) = old_error_level;
}
/* }}} */
