/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 2009 The PHP Group                                     |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Pierre A. Joye <pierre@php.net>                              |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

/* {{{ includes */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>

#include <unicode/uidna.h>
#include <unicode/ustring.h>
#include "ext/standard/php_string.h"

#include "intl_error.h"
 #include "intl_convert.h"
/* }}} */

/* {{{ grapheme_register_constants
 * Register API constants
 */
void idn_register_constants( INIT_FUNC_ARGS )
{
	/* Option to prohibit processing of unassigned codepoints in the input and
	   do not check if the input conforms to STD-3 ASCII rules. */
	REGISTER_LONG_CONSTANT("IDNA_DEFAULT", UIDNA_DEFAULT, CONST_CS | CONST_PERSISTENT);

	/* Option to allow processing of unassigned codepoints in the input */
	REGISTER_LONG_CONSTANT("IDNA_ALLOW_UNASSIGNED", UIDNA_ALLOW_UNASSIGNED, CONST_CS | CONST_PERSISTENT);

	/* Option to check if input conforms to STD-3 ASCII rules */
	REGISTER_LONG_CONSTANT("IDNA_USE_STD3_RULES", UIDNA_USE_STD3_RULES, CONST_CS | CONST_PERSISTENT);
}
/* }}} */

enum {
	INTL_IDN_TO_ASCII = 0,
	INTL_IDN_TO_UTF8
};

static void php_intl_idn_to(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	unsigned char* domain;
	int domain_len;
	long option = 0;
	UChar* ustring = NULL;
	int ustring_len = 0;
	UErrorCode status;
	char     *converted_utf8;
	int32_t   converted_utf8_len;
	UChar     converted[MAXPATHLEN];
	int32_t   converted_ret_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", (char **)&domain, &domain_len, &option) == FAILURE) {
		return;
	}

	if (domain_len < 1) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "idn_to_ascii: empty domain name", 0 TSRMLS_CC );
		RETURN_FALSE;
	}

	/* convert the string to UTF-16. */
	status = U_ZERO_ERROR;
	intl_convert_utf8_to_utf16(&ustring, &ustring_len, (char*) domain, domain_len, &status );

	if (U_FAILURE(status)) {
		intl_error_set_code(NULL, status TSRMLS_CC);

		/* Set error messages. */
		intl_error_set_custom_msg( NULL, "Error converting input string to UTF-16", 0 TSRMLS_CC );
		if (ustring) {
			efree(ustring);
		}
		RETURN_FALSE;
	} else {
		UParseError parse_error;

		status = U_ZERO_ERROR;
		if (mode == INTL_IDN_TO_ASCII) {
			converted_ret_len = uidna_IDNToASCII(ustring, ustring_len, converted, MAXPATHLEN, (int32_t)option, &parse_error, &status);
		} else {
			converted_ret_len = uidna_IDNToUnicode(ustring, ustring_len, converted, MAXPATHLEN, (int32_t)option, &parse_error, &status);
		}
		efree(ustring);

		if (U_FAILURE(status)) {
			intl_error_set( NULL, status, "idn_to_ascii: cannot convert to ASCII", 0 TSRMLS_CC );
			RETURN_FALSE;
		}

		status = U_ZERO_ERROR;
		intl_convert_utf16_to_utf8(&converted_utf8, &converted_utf8_len, converted, converted_ret_len, &status);

		if (U_FAILURE(status)) {
			/* Set global error code. */
			intl_error_set_code(NULL, status TSRMLS_CC);

			/* Set error messages. */
			intl_error_set_custom_msg( NULL, "Error converting output string to UTF-8", 0 TSRMLS_CC );
			efree(converted_utf8);
			RETURN_FALSE;
		}
	}

	/* return the allocated string, not a duplicate */
	RETURN_STRINGL(((char *)converted_utf8), converted_utf8_len, 0);
}

/* {{{ proto int idn_to_ascii(string domain[, int options])
   Converts an Unicode domain to ASCII representation, as defined in the IDNA RFC */
PHP_FUNCTION(idn_to_ascii)
{
	php_intl_idn_to(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTL_IDN_TO_ASCII);
}
/* }}} */


/* {{{ proto int idn_to_utf8(string domain[, int options])
   Converts an ASCII representation of the domain to Unicode (UTF-8), as defined in the IDNA RFC */
PHP_FUNCTION(idn_to_utf8)
{
	php_intl_idn_to(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTL_IDN_TO_UTF8);
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
