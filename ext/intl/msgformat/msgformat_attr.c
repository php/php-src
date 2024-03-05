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

#include "php_intl.h"
#include "msgformat_class.h"
#include "msgformat_data.h"
#include "intl_convert.h"

#include <unicode/ustring.h>

/* {{{ Get formatter pattern. */
PHP_FUNCTION( msgfmt_get_pattern )
{
	MSG_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O", &object, MessageFormatter_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	MSG_FORMAT_METHOD_FETCH_OBJECT;

	if(mfo->mf_data.orig_format) {
		RETURN_STRINGL(mfo->mf_data.orig_format, mfo->mf_data.orig_format_len);
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ Set formatter pattern. */
PHP_FUNCTION( msgfmt_set_pattern )
{
	char*       value = NULL;
	size_t      value_len = 0;
	int32_t     spattern_len = 0;
	UChar*	    spattern  = NULL;
	UParseError spattern_error = {0};
	MSG_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Os",
		&object, MessageFormatter_ce_ptr, &value, &value_len ) == FAILURE )
	{
		RETURN_THROWS();
	}

	MSG_FORMAT_METHOD_FETCH_OBJECT;

	/* Convert given pattern to UTF-16. */
	intl_convert_utf8_to_utf16(&spattern, &spattern_len, value, value_len, &INTL_DATA_ERROR_CODE(mfo));
	INTL_METHOD_CHECK_STATUS(mfo, "Error converting pattern to UTF-16" );

#ifdef MSG_FORMAT_QUOTE_APOS
	if(msgformat_fix_quotes(&spattern, &spattern_len, &INTL_DATA_ERROR_CODE(mfo)) != SUCCESS) {
		intl_error_set( NULL, U_INVALID_FORMAT_ERROR,
			"msgfmt_set_pattern: error converting pattern to quote-friendly format", 0 );
		RETURN_FALSE;
	}
#endif

	umsg_applyPattern(MSG_FORMAT_OBJECT(mfo), spattern, spattern_len, &spattern_error, &INTL_DATA_ERROR_CODE(mfo));
	if (spattern) {
		efree(spattern);
	}
	if (U_FAILURE(INTL_DATA_ERROR_CODE(mfo))) {
		char *msg;
		spprintf(&msg, 0, "Error setting symbol value at line %d, offset %d", spattern_error.line, spattern_error.offset);
		intl_errors_set_custom_msg(INTL_DATA_ERROR_P(mfo), msg, 1);
		efree(msg);
		RETURN_FALSE;
	}

	if(mfo->mf_data.orig_format) {
		efree(mfo->mf_data.orig_format);
	}
	mfo->mf_data.orig_format = estrndup(value, value_len);
	mfo->mf_data.orig_format_len = value_len;
	/* invalidate cached format types */
	if (mfo->mf_data.arg_types) {
		zend_hash_destroy(mfo->mf_data.arg_types);
		efree(mfo->mf_data.arg_types);
		mfo->mf_data.arg_types = NULL;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Get formatter locale. */
PHP_FUNCTION( msgfmt_get_locale )
{
	char *loc;
	MSG_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, MessageFormatter_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	MSG_FORMAT_METHOD_FETCH_OBJECT;

	loc = (char *)umsg_getLocale(MSG_FORMAT_OBJECT(mfo));
	RETURN_STRING(loc);
}
/* }}} */
