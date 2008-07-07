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

/* {{{ */
static void msgfmt_do_parse(MessageFormatter_object *mfo, UChar *source, int src_len, zval *return_value TSRMLS_DC) 
{
	zval **fargs;
	int count = 0;
	int i;

	umsg_parse_helper(MSG_FORMAT_OBJECT(mfo), &count, &fargs, source, src_len, &INTL_DATA_ERROR_CODE(mfo));
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
	UChar *source;
	int source_len;
	MSG_FORMAT_METHOD_INIT_VARS;


	// Parse parameters.
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ou",
		&object, MessageFormatter_ce_ptr,  &source, &source_len ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"msgfmt_parse: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	// Fetch the object.
	MSG_FORMAT_METHOD_FETCH_OBJECT;

	msgfmt_do_parse(mfo, source, source_len, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto array MessageFormatter::parse( string $locale, string $pattern, string $source )
 * Parse a message. }}} */
/* {{{ proto array numfmt_parse_message( string $locale, string $pattern, string $source )
 * Parse a message.
 */
PHP_FUNCTION( msgfmt_parse_message )
{
	UChar      *spattern = NULL;
	int         spattern_len = 0;
	char       *slocale = NULL;
	int         slocale_len = 0;
	UChar      *source = NULL;
	int         source_len = 0;
	int free_pattern = 0;
	MessageFormatter_object mf={0};
	MessageFormatter_object *mfo = &mf;

	// Parse parameters.
	if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "suu",
		  &slocale, &slocale_len, &spattern, &spattern_len, &source, &source_len ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"msgfmt_parse_message: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	msgformat_data_init(&mfo->mf_data TSRMLS_CC);

	if(slocale_len == 0) {
		slocale = UG(default_locale);
	}

	if(msfgotmat_fix_quotes(&spattern, &spattern_len, &INTL_DATA_ERROR_CODE(mfo), &free_pattern) != SUCCESS) {
		intl_error_set( NULL, U_INVALID_FORMAT_ERROR,
			"msgfmt_parse_message: error converting pattern to quote-friendly format", 0 TSRMLS_CC );
		RETURN_FALSE;
	}

	// Create an ICU message formatter.
	MSG_FORMAT_OBJECT(mfo) = umsg_open(spattern, spattern_len, slocale, NULL, &INTL_DATA_ERROR_CODE(mfo));
	if(free_pattern) {
		efree(spattern);
	}

	INTL_METHOD_CHECK_STATUS(mfo, "Creating message formatter failed");
	msgfmt_do_parse(mfo, source, source_len, return_value TSRMLS_CC);

	// drop the temporary formatter
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
