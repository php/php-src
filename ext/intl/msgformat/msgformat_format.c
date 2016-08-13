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
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unicode/ustring.h>

#include "php_intl.h"
#include "msgformat_class.h"
#include "msgformat_format.h"
#include "msgformat_data.h"
#include "msgformat_helpers.h"
#include "intl_convert.h"

#ifndef Z_ADDREF_P
#define Z_ADDREF_P(z) ((z)->refcount++)
#endif

/* {{{ */
static void msgfmt_do_format(MessageFormatter_object *mfo, zval *args, zval *return_value)
{
	int count;
	UChar* formatted = NULL;
	int32_t formatted_len = 0;
	HashTable *args_copy;

	count = zend_hash_num_elements(Z_ARRVAL_P(args));

	ALLOC_HASHTABLE(args_copy);
	zend_hash_init(args_copy, count, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(args_copy, Z_ARRVAL_P(args), (copy_ctor_func_t)zval_add_ref);

	umsg_format_helper(mfo, args_copy, &formatted, &formatted_len);

	zend_hash_destroy(args_copy);
	efree(args_copy);

	if (formatted && U_FAILURE(INTL_DATA_ERROR_CODE(mfo))) {
			efree(formatted);
	}

	if (U_FAILURE(INTL_DATA_ERROR_CODE(mfo))) {
		RETURN_FALSE;
	} else {
		INTL_METHOD_RETVAL_UTF8(mfo, formatted, formatted_len, 1);
	}
}
/* }}} */

/* {{{ proto mixed MessageFormatter::format( array $args )
 * Format a message. }}} */
/* {{{ proto mixed msgfmt_format( MessageFormatter $nf, array $args )
 * Format a message.
 */
PHP_FUNCTION( msgfmt_format )
{
	zval *args;
	MSG_FORMAT_METHOD_INIT_VARS;


	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Oa",
		&object, MessageFormatter_ce_ptr,  &args ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"msgfmt_format: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	MSG_FORMAT_METHOD_FETCH_OBJECT;

	msgfmt_do_format(mfo, args, return_value);
}
/* }}} */

/* {{{ proto mixed MessageFormatter::formatMessage( string $locale, string $pattern, array $args )
 * Format a message. }}} */
/* {{{ proto mixed msgfmt_format_message( string $locale, string $pattern, array $args )
 * Format a message.
 */
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

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "ssa",
		  &slocale, &slocale_len, &pattern, &pattern_len, &args ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"msgfmt_format_message: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	memset(mfo, 0, sizeof(*mfo));
	msgformat_data_init(&mfo->mf_data);

	if(pattern && pattern_len) {
		intl_convert_utf8_to_utf16(&spattern, &spattern_len, pattern, pattern_len, &INTL_DATA_ERROR_CODE(mfo));
		if( U_FAILURE(INTL_DATA_ERROR_CODE((mfo))) )
		{
			intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
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
		intl_error_set( NULL, U_INVALID_FORMAT_ERROR,
			"msgfmt_format_message: error converting pattern to quote-friendly format", 0 );
		RETURN_FALSE;
	}
#endif

	/* Create an ICU message formatter. */
	MSG_FORMAT_OBJECT(mfo) = umsg_open(spattern, spattern_len, slocale, NULL, &INTL_DATA_ERROR_CODE(mfo));
	if(spattern && spattern_len) {
		efree(spattern);
	}
	INTL_METHOD_CHECK_STATUS(mfo, "Creating message formatter failed");

	msgfmt_do_format(mfo, args, return_value);

	/* drop the temporary formatter */
	msgformat_data_free(&mfo->mf_data);
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
