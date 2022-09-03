/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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

#include "idn.h"
#include "intl_error.h"
#include "intl_convert.h"
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

static void php_intl_idn_handoff(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	zend_string *domain;
	zend_long option = UIDNA_DEFAULT,
	variant = INTL_IDN_VARIANT_UTS46;
	zval *idna_info = NULL;

	intl_error_reset(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|llz",
			&domain, &option, &variant, &idna_info) == FAILURE) {
		RETURN_THROWS();
	}

	if (variant != INTL_IDN_VARIANT_UTS46) {
		php_intl_bad_args("invalid variant, must be INTL_IDNA_VARIANT_UTS46");
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

	if (idna_info != NULL) {
		idna_info = zend_try_array_init(idna_info);
		if (!idna_info) {
			RETURN_THROWS();
		}
	}

	php_intl_idn_to_46(INTERNAL_FUNCTION_PARAM_PASSTHRU, domain, (uint32_t)option, mode, idna_info);
}

/* {{{ Converts an Unicode domain to ASCII representation, as defined in the IDNA RFC */
PHP_FUNCTION(idn_to_ascii)
{
	php_intl_idn_handoff(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTL_IDN_TO_ASCII);
}
/* }}} */


/* {{{ Converts an ASCII representation of the domain to Unicode (UTF-8), as defined in the IDNA RFC */
PHP_FUNCTION(idn_to_utf8)
{
	php_intl_idn_handoff(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTL_IDN_TO_UTF8);
}
/* }}} */
