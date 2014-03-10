/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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
#include "msgformat_parse.h"
#include "msgformat_data.h"
#include "msgformat_helpers.h"
#include "intl_convert.h"

/* {{{ */
static void msgfmt_do_parse(MessageFormatter_object *mfo, char *source, int src_len, zval *return_value TSRMLS_DC) 
{
	zval **fargs;
	int count = 0;
	int i;
	UChar *usource = NULL;
	int usrc_len = 0;

	intl_convert_utf8_to_utf16(&usource, &usrc_len, source, src_len, &INTL_DATA_ERROR_CODE(mfo));
	INTL_METHOD_CHECK_STATUS(mfo, "Converting parse string failed");

	umsg_parse_helper(MSG_FORMAT_OBJECT(mfo), &count, &fargs, usource, usrc_len, &INTL_DATA_ERROR_CODE(mfo));
	if (usource) {
		efree(usource);
	}
	INTL_METHOD_CHECK_STATUS(mfo, "Parsing failed");

	array_init(return_value);
	for(i=0;i<count;i++) {
		add_next_index_zval(return_value, fargs[i]);
	}
	efree(fargs);
}
/* }}} */

/* {{{ proto array MessageFormatter::parse( string $source )
 * Parse a message }}} */
/* {{{ proto array msgfmt_parse( MessageFormatter $nf, string $source )
 * Parse a message.
 */
PHP_FUNCTION( msgfmt_parse )
{
	char *source;
	int source_len;
	MSG_FORMAT_METHOD_INIT_VARS;


	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os",
		&object, MessageFormatter_ce_ptr,  &source, &source_len ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"msgfmt_parse: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	MSG_FORMAT_METHOD_FETCH_OBJECT;

	msgfmt_do_parse(mfo, source, source_len, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto array MessageFormatter::formatMessage( string $locale, string $pattern, string $source )
 * Parse a message. }}} */
/* {{{ proto array numfmt_parse_message( string $locale, string $pattern, string $source )
 * Parse a message.
 */
PHP_FUNCTION( msgfmt_parse_message )
{
	UChar      *spattern = NULL;
	int         spattern_len = 0;
	char       *pattern = NULL;
	int         pattern_len = 0;
	const char *slocale = NULL;
	int         slocale_len = 0;
	char       *source = NULL;
	int         src_len = 0;
	MessageFormatter_object mf = {0};
	MessageFormatter_object *mfo = &mf;

	/* Parse parameters. */
	if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "sss",
		  &slocale, &slocale_len, &pattern, &pattern_len, &source, &src_len ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"msgfmt_parse_message: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	msgformat_data_init(&mfo->mf_data TSRMLS_CC);

	if(pattern && pattern_len) {
		intl_convert_utf8_to_utf16(&spattern, &spattern_len, pattern, pattern_len, &INTL_DATA_ERROR_CODE(mfo));
		if( U_FAILURE(INTL_DATA_ERROR_CODE((mfo))) )
		{
			intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"msgfmt_parse_message: error converting pattern to UTF-16", 0 TSRMLS_CC );
			RETURN_FALSE;
		}
	} else {
		spattern_len = 0;
		spattern = NULL;
	}

	if(slocale_len == 0) {
		slocale = intl_locale_get_default(TSRMLS_C);
	}

#ifdef MSG_FORMAT_QUOTE_APOS
	if(msgformat_fix_quotes(&spattern, &spattern_len, &INTL_DATA_ERROR_CODE(mfo)) != SUCCESS) {
		intl_error_set( NULL, U_INVALID_FORMAT_ERROR,
			"msgfmt_parse_message: error converting pattern to quote-friendly format", 0 TSRMLS_CC );
		RETURN_FALSE;
	}
#endif

	/* Create an ICU message formatter. */
	MSG_FORMAT_OBJECT(mfo) = umsg_open(spattern, spattern_len, slocale, NULL, &INTL_DATA_ERROR_CODE(mfo));
	if(spattern && spattern_len) {
		efree(spattern);
	}
	INTL_METHOD_CHECK_STATUS(mfo, "Creating message formatter failed");

	msgfmt_do_parse(mfo, source, src_len, return_value TSRMLS_CC);

	/* drop the temporary formatter */
	msgformat_data_free(&mfo->mf_data TSRMLS_CC);
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
