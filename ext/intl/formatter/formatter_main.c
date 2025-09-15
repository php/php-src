/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unicode/ustring.h>
#include <unicode/uloc.h>

#include "php_intl.h"
#include "formatter_class.h"
#include "intl_convert.h"

/* {{{ */
static int numfmt_ctor(INTERNAL_FUNCTION_PARAMETERS)
{
	char*       locale;
	char*       pattern = NULL;
	size_t      locale_len = 0, pattern_len = 0;
	zend_long   style;
	UChar*      spattern     = NULL;
	int32_t     spattern_len = 0;
	FORMATTER_METHOD_INIT_VARS;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(locale, locale_len)
		Z_PARAM_LONG(style)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(pattern, pattern_len)
	ZEND_PARSE_PARAMETERS_END_EX(return FAILURE);

	INTL_CHECK_LOCALE_LEN_OR_FAILURE(locale_len);
	object = return_value;
	FORMATTER_METHOD_FETCH_OBJECT_NO_CHECK;
	if (FORMATTER_OBJECT(nfo)) {
		zend_throw_error(NULL, "NumberFormatter object is already constructed");
		return FAILURE;
	}

	/* Convert pattern (if specified) to UTF-16. */
	if(pattern && pattern_len) {
		intl_convert_utf8_to_utf16(&spattern, &spattern_len, pattern, pattern_len, &INTL_DATA_ERROR_CODE(nfo));
		INTL_CTOR_CHECK_STATUS(nfo, "error converting pattern to UTF-16");
	}

	if(locale_len == 0) {
		locale = (char *)intl_locale_get_default();
	}

	if (strlen(uloc_getISO3Language(locale)) == 0) {
		zend_argument_value_error(1, "\"%s\" is invalid", locale);
		return FAILURE;
	}

	char* canonicalized_locale = canonicalize_locale_string(locale);
	const char* final_locale = canonicalized_locale ? canonicalized_locale : locale;

	FORMATTER_OBJECT(nfo) = unum_open(style, spattern, spattern_len, final_locale, NULL, &INTL_DATA_ERROR_CODE(nfo));

	if (spattern) {
		efree(spattern);
	}
	
	if (canonicalized_locale) {
		efree(canonicalized_locale);
	}

	INTL_CTOR_CHECK_STATUS(nfo, "number formatter creation failed");
	return SUCCESS;
}
/* }}} */

/* {{{ Create number formatter. */
PHP_FUNCTION( numfmt_create )
{
	object_init_ex( return_value, NumberFormatter_ce_ptr );
	if (numfmt_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU) == FAILURE) {
		zval_ptr_dtor(return_value);
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ NumberFormatter object constructor. */
PHP_METHOD( NumberFormatter, __construct )
{
	const bool old_use_exception = INTL_G(use_exceptions);
	const zend_long old_error_level = INTL_G(error_level);
	INTL_G(use_exceptions) = true;
	INTL_G(error_level) = 0;

	return_value = ZEND_THIS;
	if (numfmt_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU) == FAILURE) {
		ZEND_ASSERT(EG(exception));
	}
	INTL_G(use_exceptions) = old_use_exception;
	INTL_G(error_level) = old_error_level;
}
/* }}} */

/* {{{ Get formatter's last error code. */
PHP_FUNCTION( numfmt_get_error_code )
{
	FORMATTER_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, NumberFormatter_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	nfo = Z_INTL_NUMBERFORMATTER_P(object);

	/* Return formatter's last error code. */
	RETURN_LONG( INTL_DATA_ERROR_CODE(nfo) );
}
/* }}} */

/* {{{ Get text description for formatter's last error code. */
PHP_FUNCTION( numfmt_get_error_message )
{
	zend_string *message = NULL;
	FORMATTER_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, NumberFormatter_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	nfo = Z_INTL_NUMBERFORMATTER_P(object);

	/* Return last error message. */
	message = intl_error_get_message( INTL_DATA_ERROR_P(nfo) );
	RETURN_STR(message);
}
/* }}} */
