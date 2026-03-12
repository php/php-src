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
#include <unicode/umsg.h>

extern "C" {
#include "php_intl.h"
#include "msgformat_class.h"
#include "msgformat_data.h"
#include "intl_convert.h"
}

/* {{{ */
static int msgfmt_ctor(INTERNAL_FUNCTION_PARAMETERS)
{
	char*       locale;
	char*       pattern;
	size_t      locale_len = 0, pattern_len = 0;
	UChar*      spattern     = NULL;
	int         spattern_len = 0;
	zval*       object;
	MessageFormatter_object* mfo;
	UParseError parse_error;
	intl_error_reset( NULL );

	object = return_value;
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(locale, locale_len)
		Z_PARAM_STRING(pattern, pattern_len)
	ZEND_PARSE_PARAMETERS_END_EX(return FAILURE);

	INTL_CHECK_LOCALE_LEN_OR_FAILURE(locale_len);
	MSG_FORMAT_METHOD_FETCH_OBJECT_NO_CHECK;

	/* Convert pattern (if specified) to UTF-16. */
	if(pattern && pattern_len) {
		intl_convert_utf8_to_utf16(&spattern, &spattern_len, pattern, pattern_len, &INTL_DATA_ERROR_CODE(mfo));
		INTL_CTOR_CHECK_STATUS(mfo, "error converting pattern to UTF-16");
	} else {
		spattern_len = 0;
		spattern = NULL;
	}

	if(locale_len == 0) {
		locale = (char *)intl_locale_get_default();
	}

#ifdef MSG_FORMAT_QUOTE_APOS
	if(msgformat_fix_quotes(&spattern, &spattern_len, &INTL_DATA_ERROR_CODE(mfo)) != SUCCESS) {
		INTL_CTOR_CHECK_STATUS(mfo, "msgfmt_create: error converting pattern to quote-friendly format");
	}
#endif

	if ((mfo)->mf_data.orig_format) {
		msgformat_data_free(&mfo->mf_data);
	}

	(mfo)->mf_data.orig_format = estrndup(pattern, pattern_len);
	(mfo)->mf_data.orig_format_len = pattern_len;

	/* Create an ICU message formatter. */
	MSG_FORMAT_OBJECT(mfo) = umsg_open(spattern, spattern_len, locale, &parse_error, &INTL_DATA_ERROR_CODE(mfo));

	if(spattern) {
		efree(spattern);
	}

	if (INTL_DATA_ERROR_CODE( mfo ) == U_PATTERN_SYNTAX_ERROR) {
		char *msg = NULL;
		smart_str parse_error_str;
		parse_error_str = intl_parse_error_to_string( &parse_error );
		spprintf( &msg, 0, "pattern syntax error (%s)", parse_error_str.s? ZSTR_VAL(parse_error_str.s) : "unknown parser error" );
		smart_str_free( &parse_error_str );

		intl_error_set_code( NULL, INTL_DATA_ERROR_CODE( mfo ) );
		intl_errors_set_custom_msg( INTL_DATA_ERROR_P( mfo ), msg);

		efree( msg );
		return FAILURE;
	}

	INTL_CTOR_CHECK_STATUS(mfo, "message formatter creation failed");
	return SUCCESS;
}
/* }}} */

/* {{{ Create formatter. */
U_CFUNC PHP_FUNCTION( msgfmt_create )
{
	object_init_ex( return_value, MessageFormatter_ce_ptr );
	if (msgfmt_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU) == FAILURE) {
		zval_ptr_dtor(return_value);
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ MessageFormatter object constructor. */
U_CFUNC PHP_METHOD( MessageFormatter, __construct )
{
	const bool old_use_exception = INTL_G(use_exceptions);
	const zend_long old_error_level = INTL_G(error_level);
	INTL_G(use_exceptions) = true;
	INTL_G(error_level) = 0;

	return_value = ZEND_THIS;
	if (msgfmt_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU) == FAILURE) {
		ZEND_ASSERT(EG(exception));
	}
	INTL_G(use_exceptions) = old_use_exception;
	INTL_G(error_level) = old_error_level;
}
/* }}} */

/* {{{ Get formatter's last error code. */
U_CFUNC PHP_FUNCTION( msgfmt_get_error_code )
{
	zval*                    object  = NULL;
	MessageFormatter_object*  mfo     = NULL;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, MessageFormatter_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	mfo = Z_INTL_MESSAGEFORMATTER_P( object );

	/* Return formatter's last error code. */
	RETURN_LONG( INTL_DATA_ERROR_CODE(mfo) );
}
/* }}} */

/* {{{ Get text description for formatter's last error code. */
U_CFUNC PHP_FUNCTION( msgfmt_get_error_message )
{
	zend_string*             message = NULL;
	zval*                    object  = NULL;
	MessageFormatter_object*  mfo     = NULL;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, MessageFormatter_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	mfo = Z_INTL_MESSAGEFORMATTER_P( object );

	/* Return last error message. */
	message = intl_error_get_message( &mfo->mf_data.error );
	RETURN_STR(message);
}
/* }}} */
