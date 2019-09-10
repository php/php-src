/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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
   |         Gustavo Lopes  <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
 */

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

enum {
	INTL_IDN_VARIANT_2003 = 0,
	INTL_IDN_VARIANT_UTS46
};

/* {{{ grapheme_register_constants
 * Register API constants
 */
void idn_register_constants( INIT_FUNC_ARGS )
{
	/* OPTIONS */

	/* Option to prohibit processing of unassigned codepoints in the input and
	   do not check if the input conforms to STD-3 ASCII rules. */
	REGISTER_LONG_CONSTANT("IDNA_DEFAULT", UIDNA_DEFAULT, CONST_CS | CONST_PERSISTENT);

	/* Option to allow processing of unassigned codepoints in the input */
	REGISTER_LONG_CONSTANT("IDNA_ALLOW_UNASSIGNED", UIDNA_ALLOW_UNASSIGNED, CONST_CS | CONST_PERSISTENT);

	/* Option to check if input conforms to STD-3 ASCII rules */
	REGISTER_LONG_CONSTANT("IDNA_USE_STD3_RULES", UIDNA_USE_STD3_RULES, CONST_CS | CONST_PERSISTENT);

	/* Option to check for whether the input conforms to the BiDi rules.
	 * Ignored by the IDNA2003 implementation. (IDNA2003 always performs a BiDi check.) */
	REGISTER_LONG_CONSTANT("IDNA_CHECK_BIDI", UIDNA_CHECK_BIDI, CONST_CS | CONST_PERSISTENT);

	/* Option to check for whether the input conforms to the CONTEXTJ rules.
	 * Ignored by the IDNA2003 implementation. (The CONTEXTJ check is new in IDNA2008.) */
	REGISTER_LONG_CONSTANT("IDNA_CHECK_CONTEXTJ", UIDNA_CHECK_CONTEXTJ, CONST_CS | CONST_PERSISTENT);

	/* Option for nontransitional processing in ToASCII().
	 * By default, ToASCII() uses transitional processing.
	 * Ignored by the IDNA2003 implementation. */
	REGISTER_LONG_CONSTANT("IDNA_NONTRANSITIONAL_TO_ASCII", UIDNA_NONTRANSITIONAL_TO_ASCII, CONST_CS | CONST_PERSISTENT);

	/* Option for nontransitional processing in ToUnicode().
	 * By default, ToUnicode() uses transitional processing.
	 * Ignored by the IDNA2003 implementation. */
	REGISTER_LONG_CONSTANT("IDNA_NONTRANSITIONAL_TO_UNICODE", UIDNA_NONTRANSITIONAL_TO_UNICODE, CONST_CS | CONST_PERSISTENT);

	/* VARIANTS */
	REGISTER_LONG_CONSTANT("INTL_IDNA_VARIANT_2003", INTL_IDN_VARIANT_2003, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INTL_IDNA_VARIANT_UTS46", INTL_IDN_VARIANT_UTS46, CONST_CS | CONST_PERSISTENT);

	/* PINFO ERROR CODES */
	REGISTER_LONG_CONSTANT("IDNA_ERROR_EMPTY_LABEL", UIDNA_ERROR_EMPTY_LABEL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_LABEL_TOO_LONG", UIDNA_ERROR_LABEL_TOO_LONG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_DOMAIN_NAME_TOO_LONG", UIDNA_ERROR_DOMAIN_NAME_TOO_LONG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_LEADING_HYPHEN", UIDNA_ERROR_LEADING_HYPHEN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_TRAILING_HYPHEN", UIDNA_ERROR_TRAILING_HYPHEN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_HYPHEN_3_4", UIDNA_ERROR_HYPHEN_3_4, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_LEADING_COMBINING_MARK", UIDNA_ERROR_LEADING_COMBINING_MARK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_DISALLOWED", UIDNA_ERROR_DISALLOWED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_PUNYCODE", UIDNA_ERROR_PUNYCODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_LABEL_HAS_DOT", UIDNA_ERROR_LABEL_HAS_DOT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_INVALID_ACE_LABEL", UIDNA_ERROR_INVALID_ACE_LABEL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_BIDI", UIDNA_ERROR_BIDI, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IDNA_ERROR_CONTEXTJ", UIDNA_ERROR_CONTEXTJ, CONST_CS | CONST_PERSISTENT);
}
/* }}} */

enum {
	INTL_IDN_TO_ASCII = 0,
	INTL_IDN_TO_UTF8
};

/* like INTL_CHECK_STATUS, but as a function and varying the name of the func */
static int php_intl_idn_check_status(UErrorCode err, const char *msg)
{
	intl_error_set_code(NULL, err);
	if (U_FAILURE(err)) {
		char *buff;
		spprintf(&buff, 0, "%s: %s",
			get_active_function_name(),
			msg);
		intl_error_set_custom_msg(NULL, buff, 1);
		efree(buff);
		return FAILURE;
	}

	return SUCCESS;
}

static inline void php_intl_bad_args(const char *msg)
{
	php_intl_idn_check_status(U_ILLEGAL_ARGUMENT_ERROR, msg);
}

static void php_intl_idn_to_46(INTERNAL_FUNCTION_PARAMETERS,
		const zend_string *domain, uint32_t option, int mode, zval *idna_info)
{
	UErrorCode	  status = U_ZERO_ERROR;
	UIDNA		  *uts46;
	int32_t		  len;
	zend_string	  *buffer;
	UIDNAInfo	  info = UIDNA_INFO_INITIALIZER;

	uts46 = uidna_openUTS46(option, &status);
	if (php_intl_idn_check_status(status, "failed to open UIDNA instance") == FAILURE) {
		RETURN_FALSE;
	}

	if (mode == INTL_IDN_TO_ASCII) {
		const int32_t buffer_capac = 255;
		buffer = zend_string_alloc(buffer_capac, 0);
		len = uidna_nameToASCII_UTF8(uts46, ZSTR_VAL(domain), ZSTR_LEN(domain),
				ZSTR_VAL(buffer), buffer_capac, &info, &status);
		if (len >= buffer_capac || php_intl_idn_check_status(status, "failed to convert name") == FAILURE) {
			uidna_close(uts46);
			zend_string_efree(buffer);
			RETURN_FALSE;
		}
	} else {
		const int32_t buffer_capac = 252*4;
		buffer = zend_string_alloc(buffer_capac, 0);
		len = uidna_nameToUnicodeUTF8(uts46, ZSTR_VAL(domain), ZSTR_LEN(domain),
				ZSTR_VAL(buffer), buffer_capac, &info, &status);
		if (len >= buffer_capac || php_intl_idn_check_status(status, "failed to convert name") == FAILURE) {
			uidna_close(uts46);
			zend_string_efree(buffer);
			RETURN_FALSE;
		}
	}

	ZSTR_VAL(buffer)[len] = '\0';
	ZSTR_LEN(buffer) = len;

	if (info.errors == 0) {
		RETVAL_STR_COPY(buffer);
	} else {
		RETVAL_FALSE;
	}

	if (idna_info) {
		add_assoc_str_ex(idna_info, "result", sizeof("result")-1, zend_string_copy(buffer));
		add_assoc_bool_ex(idna_info, "isTransitionalDifferent",
				sizeof("isTransitionalDifferent")-1, info.isTransitionalDifferent);
		add_assoc_long_ex(idna_info, "errors", sizeof("errors")-1, (zend_long)info.errors);
	}

	zend_string_release(buffer);
	uidna_close(uts46);
}

static void php_intl_idn_to(INTERNAL_FUNCTION_PARAMETERS,
		const zend_string *domain, uint32_t option, int mode)
{
	UChar* ustring = NULL;
	int ustring_len = 0;
	UErrorCode status;
	zend_string *u8str;

	/* convert the string to UTF-16. */
	status = U_ZERO_ERROR;
	intl_convert_utf8_to_utf16(&ustring, &ustring_len, ZSTR_VAL(domain), ZSTR_LEN(domain), &status);

	if (U_FAILURE(status)) {
		intl_error_set_code(NULL, status);

		/* Set error messages. */
		intl_error_set_custom_msg( NULL, "Error converting input string to UTF-16", 0 );
		if (ustring) {
			efree(ustring);
		}
		RETURN_FALSE;
	} else {
		UParseError parse_error;
		UChar       converted[MAXPATHLEN];
		int32_t     converted_ret_len;

		status = U_ZERO_ERROR;
#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif ZEND_GCC_VERSION >= 4008
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
		if (mode == INTL_IDN_TO_ASCII) {
			converted_ret_len = uidna_IDNToASCII(ustring, ustring_len, converted, MAXPATHLEN, (int32_t)option, &parse_error, &status);
		} else {
			converted_ret_len = uidna_IDNToUnicode(ustring, ustring_len, converted, MAXPATHLEN, (int32_t)option, &parse_error, &status);
		}
#if defined(__clang__)
# pragma clang diagnostic pop
#elif ZEND_GCC_VERSION >= 4008
# pragma GCC diagnostic pop
#endif
		efree(ustring);

		if (U_FAILURE(status)) {
			intl_error_set( NULL, status, "idn_to_ascii: cannot convert to ASCII", 0 );
			RETURN_FALSE;
		}

		status = U_ZERO_ERROR;
		u8str = intl_convert_utf16_to_utf8(converted, converted_ret_len, &status);

		if (!u8str) {
			/* Set global error code. */
			intl_error_set_code(NULL, status);

			/* Set error messages. */
			intl_error_set_custom_msg( NULL, "Error converting output string to UTF-8", 0 );
			RETURN_FALSE;
		}
	}

	/* return the allocated string, not a duplicate */
	RETVAL_NEW_STR(u8str);
}

static void php_intl_idn_handoff(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	zend_string *domain;
	zend_long option = 0,
		 variant = INTL_IDN_VARIANT_UTS46;
	zval *idna_info = NULL;

	intl_error_reset(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|llz",
			&domain, &option, &variant, &idna_info) == FAILURE) {
		php_intl_bad_args("bad arguments");
		RETURN_NULL(); /* don't set FALSE because that's not the way it was before... */
	}

	if (variant != INTL_IDN_VARIANT_2003 && variant != INTL_IDN_VARIANT_UTS46) {
		php_intl_bad_args("invalid variant, must be one of {"
			"INTL_IDNA_VARIANT_2003, INTL_IDNA_VARIANT_UTS46}");
		RETURN_FALSE;
	}

	if (ZSTR_LEN(domain) < 1) {
		php_intl_bad_args("empty domain name");
		RETURN_FALSE;
	}
	if (ZSTR_LEN(domain) > INT32_MAX - 1) {
		php_intl_bad_args("domain name too large");
		RETURN_FALSE;
	}
	/* don't check options; it wasn't checked before */

	if (variant == INTL_IDN_VARIANT_2003) {
		php_error_docref(NULL, E_DEPRECATED, "INTL_IDNA_VARIANT_2003 is deprecated");
	}

	if (idna_info != NULL) {
		if (variant == INTL_IDN_VARIANT_2003) {
			php_error_docref(NULL, E_NOTICE,
				"4 arguments were provided, but INTL_IDNA_VARIANT_2003 only "
				"takes 3 - extra argument ignored");
		} else {
			idna_info = zend_try_array_init(idna_info);
			if (!idna_info) {
				return;
			}
		}
	}

	if (variant == INTL_IDN_VARIANT_2003) {
		php_intl_idn_to(INTERNAL_FUNCTION_PARAM_PASSTHRU, domain, (uint32_t)option, mode);
	}
	else {
		php_intl_idn_to_46(INTERNAL_FUNCTION_PARAM_PASSTHRU, domain, (uint32_t)option, mode, idna_info);
	}
}

/* {{{ proto string idn_to_ascii(string domain[, int options[, int variant[, array &idna_info]]])
   Converts an Unicode domain to ASCII representation, as defined in the IDNA RFC */
PHP_FUNCTION(idn_to_ascii)
{
	php_intl_idn_handoff(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTL_IDN_TO_ASCII);
}
/* }}} */


/* {{{ proto string idn_to_utf8(string domain[, int options[, int variant[, array &idna_info]]])
   Converts an ASCII representation of the domain to Unicode (UTF-8), as defined in the IDNA RFC */
PHP_FUNCTION(idn_to_utf8)
{
	php_intl_idn_handoff(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTL_IDN_TO_UTF8);
}
/* }}} */
