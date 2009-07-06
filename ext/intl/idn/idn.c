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

/* {{{ idn_register_constants
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
	INTL_IDN_TO_UNICODE
};

static void php_intl_idn_to(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	int domain_len;
	long option = 0;
	UChar* domain = NULL;
	UParseError parse_error;
	UErrorCode status;
	UChar     converted[MAXPATHLEN];
	int32_t   converted_ret_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "u|l", (char **)&domain, &domain_len, &option, &status) == FAILURE) {
		return;
	}

	if (domain_len < 1) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "idn_to_ascii: empty domain name", 0 TSRMLS_CC );
		RETURN_FALSE;
	}

	status = U_ZERO_ERROR;
	if (mode == INTL_IDN_TO_ASCII) {
		converted_ret_len = uidna_IDNToASCII(domain, domain_len, converted, MAXPATHLEN, (int32_t)option, &parse_error, &status);
	} else {
		converted_ret_len = uidna_IDNToUnicode(domain, domain_len, converted, MAXPATHLEN, (int32_t)option, &parse_error, &status);
	}

	if (U_FAILURE(status)) {
		intl_error_set( NULL, status, "idn_to_ascii: cannot convert domain name", 0 TSRMLS_CC );
		RETURN_FALSE;
	}

	RETURN_UNICODEL(converted, converted_ret_len, 1);
}

/* {{{ proto int idn_to_ascii(string domain[, int options])
   Converts a domain name to ASCII representation, as defined in the IDNA RFC */
PHP_FUNCTION(idn_to_ascii)
{
	php_intl_idn_to(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTL_IDN_TO_ASCII);
}
/* }}} */


/* {{{ proto int idn_to_utf8(string domain[, int options])
   Converts an ASCII representation of the domain to Unicode, as defined in the IDNA RFC */
PHP_FUNCTION(idn_to_unicode)
{
	php_intl_idn_to(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTL_IDN_TO_UNICODE);
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
