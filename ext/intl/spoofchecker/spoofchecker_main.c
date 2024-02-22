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
   | Authors: Scott MacVicar <scottmac@php.net>                           |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_intl.h"
#include "spoofchecker_class.h"

/* {{{ Checks if a given text contains any suspicious characters */
PHP_METHOD(Spoofchecker, isSuspicious)
{
	int32_t ret, errmask;
	char *text;
	size_t text_len;
	zval *error_code = NULL;
	SPOOFCHECKER_METHOD_INIT_VARS;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s|z", &text, &text_len, &error_code)) {
		RETURN_THROWS();
	}

	SPOOFCHECKER_METHOD_FETCH_OBJECT;

#if U_ICU_VERSION_MAJOR_NUM >= 58
	ret = uspoof_check2UTF8(co->uspoof, text, text_len, co->uspoofres, SPOOFCHECKER_ERROR_CODE_P(co));
#else
	ret = uspoof_checkUTF8(co->uspoof, text, text_len, NULL, SPOOFCHECKER_ERROR_CODE_P(co));
#endif

	if (U_FAILURE(SPOOFCHECKER_ERROR_CODE(co))) {
		php_error_docref(NULL, E_WARNING, "(%d) %s", SPOOFCHECKER_ERROR_CODE(co), u_errorName(SPOOFCHECKER_ERROR_CODE(co)));
#if U_ICU_VERSION_MAJOR_NUM >= 58
		errmask = uspoof_getCheckResultChecks(co->uspoofres, SPOOFCHECKER_ERROR_CODE_P(co));

		if (errmask != ret) {
			php_error_docref(NULL, E_WARNING, "unexpected error (%d), does not relate to the flags passed to setChecks (%d)", ret, errmask);
		}
#endif
		RETURN_TRUE;
	}

	if (error_code) {
		zval_ptr_dtor(error_code);
		ZVAL_LONG(Z_REFVAL_P(error_code), ret);
		Z_TRY_ADDREF_P(error_code);
	}
	RETVAL_BOOL(ret != 0);
}
/* }}} */

/* {{{ Checks if a given text contains any confusable characters */
PHP_METHOD(Spoofchecker, areConfusable)
{
	int ret;
	char *s1, *s2;
	size_t s1_len, s2_len;
	zval *error_code = NULL;
	SPOOFCHECKER_METHOD_INIT_VARS;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "ss|z", &s1, &s1_len,
										 &s2, &s2_len, &error_code)) {
		RETURN_THROWS();
	}

	SPOOFCHECKER_METHOD_FETCH_OBJECT;
	if(s1_len > INT32_MAX || s2_len > INT32_MAX) {
		SPOOFCHECKER_ERROR_CODE(co) = U_BUFFER_OVERFLOW_ERROR;
	} else {
		ret = uspoof_areConfusableUTF8(co->uspoof, s1, (int32_t)s1_len, s2, (int32_t)s2_len, SPOOFCHECKER_ERROR_CODE_P(co));
	}
	if (U_FAILURE(SPOOFCHECKER_ERROR_CODE(co))) {
		php_error_docref(NULL, E_WARNING, "(%d) %s", SPOOFCHECKER_ERROR_CODE(co), u_errorName(SPOOFCHECKER_ERROR_CODE(co)));
		RETURN_TRUE;
	}

	if (error_code) {
		zval_ptr_dtor(error_code);
		ZVAL_LONG(Z_REFVAL_P(error_code), ret);
		Z_TRY_ADDREF_P(error_code);
	}
	RETVAL_BOOL(ret != 0);
}
/* }}} */

/* {{{ Locales to use when running checks */
PHP_METHOD(Spoofchecker, setAllowedLocales)
{
	char *locales;
	size_t locales_len;
	SPOOFCHECKER_METHOD_INIT_VARS;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "s", &locales, &locales_len)) {
		RETURN_THROWS();
	}

	SPOOFCHECKER_METHOD_FETCH_OBJECT;

	uspoof_setAllowedLocales(co->uspoof, locales, SPOOFCHECKER_ERROR_CODE_P(co));

	if (U_FAILURE(SPOOFCHECKER_ERROR_CODE(co))) {
		php_error_docref(NULL, E_WARNING, "(%d) %s", SPOOFCHECKER_ERROR_CODE(co), u_errorName(SPOOFCHECKER_ERROR_CODE(co)));
		return;
	}
}
/* }}} */

/* {{{ Set the checks to run */
PHP_METHOD(Spoofchecker, setChecks)
{
	zend_long checks;
	SPOOFCHECKER_METHOD_INIT_VARS;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "l", &checks)) {
		RETURN_THROWS();
	}

	SPOOFCHECKER_METHOD_FETCH_OBJECT;

	uspoof_setChecks(co->uspoof, checks, SPOOFCHECKER_ERROR_CODE_P(co));

	if (U_FAILURE(SPOOFCHECKER_ERROR_CODE(co))) {
		php_error_docref(NULL, E_WARNING, "(%d) %s", SPOOFCHECKER_ERROR_CODE(co), u_errorName(SPOOFCHECKER_ERROR_CODE(co)));
	}
}
/* }}} */

#if U_ICU_VERSION_MAJOR_NUM >= 58
/* TODO Document this method on PHP.net */
/* {{{ Set the loosest restriction level allowed for strings. */
PHP_METHOD(Spoofchecker, setRestrictionLevel)
{
	zend_long level;
	SPOOFCHECKER_METHOD_INIT_VARS;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "l", &level)) {
		RETURN_THROWS();
	}

	SPOOFCHECKER_METHOD_FETCH_OBJECT;

	if (USPOOF_ASCII != level &&
			USPOOF_SINGLE_SCRIPT_RESTRICTIVE != level &&
			USPOOF_HIGHLY_RESTRICTIVE != level &&
			USPOOF_MODERATELY_RESTRICTIVE != level &&
			USPOOF_MINIMALLY_RESTRICTIVE != level &&
			USPOOF_UNRESTRICTIVE != level) {
		zend_argument_value_error(1, "must be one of Spoofchecker::ASCII, Spoofchecker::SINGLE_SCRIPT_RESTRICTIVE, "
			"Spoofchecker::SINGLE_HIGHLY_RESTRICTIVE, Spoofchecker::SINGLE_MODERATELY_RESTRICTIVE, "
			"Spoofchecker::SINGLE_MINIMALLY_RESTRICTIVE, or Spoofchecker::UNRESTRICTIVE");
		RETURN_THROWS();
	}

	uspoof_setRestrictionLevel(co->uspoof, (URestrictionLevel)level);
}
/* }}} */
#endif

#if U_ICU_VERSION_MAJOR_NUM >= 74
PHP_METHOD(Spoofchecker, areBidiConfusable)
{
	int ret;
	char *id1, *id2;
	size_t length1, length2;
	zend_long direction;
	zval *error_code = NULL;
	SPOOFCHECKER_METHOD_INIT_VARS;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "lss|z", &direction, &id1, &length1,
										 &id2, &length2, &error_code)) {
		RETURN_THROWS();
	}

	SPOOFCHECKER_METHOD_FETCH_OBJECT;
	if (direction != UBIDI_LTR && direction != UBIDI_RTL) {
		zend_argument_value_error(1, "must be either SpoofChecker::UBIDI_LTR or SpoofChecker::UBIDI_RTL");
		RETURN_THROWS();
	}
	if(length1 > INT32_MAX || length2 > INT32_MAX) {
		SPOOFCHECKER_ERROR_CODE(co) = U_BUFFER_OVERFLOW_ERROR;
	} else {
		ret = uspoof_areBidiConfusableUTF8(co->uspoof, (UBiDiDirection)direction, id1, (int32_t)length1, id2, (int32_t)length2, SPOOFCHECKER_ERROR_CODE_P(co));
	}
	if (U_FAILURE(SPOOFCHECKER_ERROR_CODE(co))) {
		php_error_docref(NULL, E_WARNING, "(%d) %s", SPOOFCHECKER_ERROR_CODE(co), u_errorName(SPOOFCHECKER_ERROR_CODE(co)));
		RETURN_TRUE;
	}

	if (error_code) {
		zval_ptr_dtor(error_code);
		ZVAL_LONG(Z_REFVAL_P(error_code), ret);
		Z_TRY_ADDREF_P(error_code);
	}
	RETVAL_BOOL(ret != 0);
}
#endif
