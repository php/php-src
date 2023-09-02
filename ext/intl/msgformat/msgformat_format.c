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
#include "config.h"
#endif

#include <unicode/ustring.h>

#include "php_intl.h"
#include "msgformat_class.h"
#include "msgformat_data.h"
#include "msgformat_helpers.h"
#include "intl_convert.h"

#ifndef Z_ADDREF_P
#define Z_ADDREF_P(z) ((z)->refcount++)
#endif

/* {{{ */
static void msgfmt_do_format(MessageFormatter_object *mfo, zval *args, zval *return_value)
{
	UChar* formatted = NULL;
	int32_t formatted_len = 0;

	umsg_format_helper(mfo, Z_ARRVAL_P(args), &formatted, &formatted_len);

	if (U_FAILURE(INTL_DATA_ERROR_CODE(mfo))) {
		if (formatted) {
			efree(formatted);
		}
		RETURN_FALSE;
	} else {
		INTL_METHOD_RETVAL_UTF8(mfo, formatted, formatted_len, 1);
	}
}
/* }}} */

/* {{{ Format a message. */
PHP_FUNCTION( msgfmt_format )
{
	zval *args;
	MSG_FORMAT_METHOD_INIT_VARS;


	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Oa",
		&object, MessageFormatter_ce_ptr,  &args ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	MSG_FORMAT_METHOD_FETCH_OBJECT;

	msgfmt_do_format(mfo, args, return_value);
}
/* }}} */

/* {{{ Format a message. */
PHP_FUNCTION( msgfmt_format_message )
{
	zval       *args;
	UChar      *spattern = NULL;
	int         spattern_len = 0;
	char       *pattern = NULL;
	size_t      pattern_len = 0;
	const char *slocale = NULL;
	size_t      slocale_len = 0;
	MessageFormatter_object mf;
	MessageFormatter_object *mfo = &mf;
	UParseError parse_error;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "ssa",
		  &slocale, &slocale_len, &pattern, &pattern_len, &args ) == FAILURE )
	{
		RETURN_THROWS();
	}

	INTL_CHECK_LOCALE_LEN(slocale_len);

	memset(mfo, 0, sizeof(*mfo));
	msgformat_data_init(&mfo->mf_data);

	if(pattern && pattern_len) {
		intl_convert_utf8_to_utf16(&spattern, &spattern_len, pattern, pattern_len, &INTL_DATA_ERROR_CODE(mfo));
		if( U_FAILURE(INTL_DATA_ERROR_CODE((mfo))) )
		{
			intl_error_set(/* intl_error* */ NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"msgfmt_format_message: error converting pattern to UTF-16", 0 );
			RETURN_FALSE;
		}
	} else {
		spattern_len = 0;
		spattern = NULL;
	}

	if(slocale_len == 0) {
		slocale = intl_locale_get_default();
	}

#ifdef MSG_FORMAT_QUOTE_APOS
	if(msgformat_fix_quotes(&spattern, &spattern_len, &INTL_DATA_ERROR_CODE(mfo)) != SUCCESS) {
		intl_error_set(/* intl_error* */ NULL, U_INVALID_FORMAT_ERROR,
			"msgfmt_format_message: error converting pattern to quote-friendly format", 0 );
		RETURN_FALSE;
	}
#endif

	/* Create an ICU message formatter. */
	MSG_FORMAT_OBJECT(mfo) = umsg_open(spattern, spattern_len, slocale, &parse_error, &INTL_DATA_ERROR_CODE(mfo));
	if(spattern && spattern_len) {
		efree(spattern);
	}

	/* Cannot use INTL_METHOD_CHECK_STATUS() as we need to free the message object formatter */
	if (U_FAILURE(INTL_DATA_ERROR_CODE(mfo))) {
		if (INTL_DATA_ERROR_CODE( mfo ) == U_PATTERN_SYNTAX_ERROR) {
			char *msg = NULL;
			smart_str parse_error_str;
			parse_error_str = intl_parse_error_to_string( &parse_error );
			spprintf( &msg, 0, "pattern syntax error (%s)", parse_error_str.s? ZSTR_VAL(parse_error_str.s) : "unknown parser error" );
			smart_str_free( &parse_error_str );

			/* Pass NULL to intl_error* parameter to store message in global Intl error msg stack */
			intl_error_set_code(/* intl_error* */ NULL, INTL_DATA_ERROR_CODE( mfo ) );
			intl_errors_set_custom_msg(/* intl_error* */ NULL, msg, 1 );

			efree( msg );
		} else {
			intl_errors_set_custom_msg(/* intl_error* */ NULL, "Creating message formatter failed", 0 );
		}
		umsg_close(MSG_FORMAT_OBJECT(mfo));
		RETURN_FALSE;
    }

	msgfmt_do_format(mfo, args, return_value);

	/* drop the temporary formatter */
	msgformat_data_free(&mfo->mf_data);
}
/* }}} */
