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
#include <config.h>
#endif

#include "php_intl.h"
#include "intl_convert.h"
#include "spoofchecker_class.h"

/* {{{ Checks if a given text contains any suspicious characters */
PHP_METHOD(Spoofchecker, isSuspicious)
{
	int32_t ret, errmask;
	zend_string *text;
	zval *error_code = NULL;
	SPOOFCHECKER_METHOD_INIT_VARS;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(text)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(error_code)
	ZEND_PARSE_PARAMETERS_END();

	SPOOFCHECKER_METHOD_FETCH_OBJECT;

#if U_ICU_VERSION_MAJOR_NUM >= 58
	ret = uspoof_check2UTF8(co->uspoof, ZSTR_VAL(text), ZSTR_LEN(text), co->uspoofres, SPOOFCHECKER_ERROR_CODE_P(co));
#else
	ret = uspoof_checkUTF8(co->uspoof, ZSTR_VAL(text), ZSTR_LEN(text), NULL, SPOOFCHECKER_ERROR_CODE_P(co));
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
		ZEND_TRY_ASSIGN_REF_LONG(error_code, ret);
	}
	RETVAL_BOOL(ret != 0);
}
/* }}} */

/* {{{ Checks if a given text contains any confusable characters */
PHP_METHOD(Spoofchecker, areConfusable)
{
	int ret;
	zend_string *s1, *s2;
	zval *error_code = NULL;
	SPOOFCHECKER_METHOD_INIT_VARS;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(s1)
		Z_PARAM_STR(s2)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(error_code)
	ZEND_PARSE_PARAMETERS_END();

	SPOOFCHECKER_METHOD_FETCH_OBJECT;
	if(ZSTR_LEN(s1) > INT32_MAX || ZSTR_LEN(s2) > INT32_MAX) {
		SPOOFCHECKER_ERROR_CODE(co) = U_BUFFER_OVERFLOW_ERROR;
	} else {
		ret = uspoof_areConfusableUTF8(co->uspoof, ZSTR_VAL(s1), (int32_t)ZSTR_LEN(s1), ZSTR_VAL(s2), (int32_t)ZSTR_LEN(s2), SPOOFCHECKER_ERROR_CODE_P(co));
	}
	if (U_FAILURE(SPOOFCHECKER_ERROR_CODE(co))) {
		php_error_docref(NULL, E_WARNING, "(%d) %s", SPOOFCHECKER_ERROR_CODE(co), u_errorName(SPOOFCHECKER_ERROR_CODE(co)));
		RETURN_TRUE;
	}

	if (error_code) {
		ZEND_TRY_ASSIGN_REF_LONG(error_code, ret);
	}
	RETVAL_BOOL(ret != 0);
}
/* }}} */

/* {{{ Locales to use when running checks */
PHP_METHOD(Spoofchecker, setAllowedLocales)
{
	zend_string *locales;
	SPOOFCHECKER_METHOD_INIT_VARS;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(locales)
	ZEND_PARSE_PARAMETERS_END();

	SPOOFCHECKER_METHOD_FETCH_OBJECT;

	uspoof_setAllowedLocales(co->uspoof, ZSTR_VAL(locales), SPOOFCHECKER_ERROR_CODE_P(co));

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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(checks)
	ZEND_PARSE_PARAMETERS_END();

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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(level)
	ZEND_PARSE_PARAMETERS_END();

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

PHP_METHOD(Spoofchecker, setAllowedChars)
{
	zend_string *pattern;
	UChar *upattern = NULL;
	int32_t upattern_len = 0;
	zend_long pattern_option = 0;
	SPOOFCHECKER_METHOD_INIT_VARS;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(pattern)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(pattern_option)
	ZEND_PARSE_PARAMETERS_END();
	SPOOFCHECKER_METHOD_FETCH_OBJECT;

	if (ZSTR_LEN(pattern) > INT32_MAX) {
		zend_argument_value_error(1, "must be less than or equal to " ZEND_LONG_FMT " bytes long", INT32_MAX);
		RETURN_THROWS();
	}

	/* uset_applyPattern requires to start with a regex range char */
	if (ZSTR_VAL(pattern)[0] != '[' || ZSTR_VAL(pattern)[ZSTR_LEN(pattern) -1] != ']') {
		zend_argument_value_error(1, "must be a valid regular expression character set pattern");
		RETURN_THROWS();
	}

	intl_convert_utf8_to_utf16(&upattern, &upattern_len, ZSTR_VAL(pattern), ZSTR_LEN(pattern), SPOOFCHECKER_ERROR_CODE_P(co));
	if (U_FAILURE(SPOOFCHECKER_ERROR_CODE(co))) {
		zend_argument_value_error(1, "string conversion to unicode encoding failed (%d) %s", SPOOFCHECKER_ERROR_CODE(co), u_errorName(SPOOFCHECKER_ERROR_CODE(co)));
		RETURN_THROWS();
	}

	USet *set = uset_openEmpty();

	/* pattern is either USE_IGNORE_SPACE alone or in conjunction with the following flags (but mutually exclusive) */
	if (pattern_option &&
            pattern_option != USET_IGNORE_SPACE &&
#if U_ICU_VERSION_MAJOR_NUM >= 73
            pattern_option != (USET_IGNORE_SPACE|USET_SIMPLE_CASE_INSENSITIVE) &&
#endif
            pattern_option != (USET_IGNORE_SPACE|USET_CASE_INSENSITIVE) &&
            pattern_option != (USET_IGNORE_SPACE|USET_ADD_CASE_MAPPINGS)) {
		zend_argument_value_error(2, "must be a valid pattern option, 0 or (SpoofChecker::IGNORE_SPACE|(<none> or SpoofChecker::USET_CASE_INSENSITIVE or SpoofChecker::USET_ADD_CASE_MAPPINGS"
#if U_ICU_VERSION_MAJOR_NUM >= 73
				" or SpoofChecker::USET_SIMPLE_CASE_INSENSITIVE"
#endif
				"))"
		);
		uset_close(set);
		efree(upattern);
		RETURN_THROWS();
	}

	uset_applyPattern(set, upattern, upattern_len, (uint32_t)pattern_option, SPOOFCHECKER_ERROR_CODE_P(co));
	if (U_FAILURE(SPOOFCHECKER_ERROR_CODE(co))) {
		zend_argument_value_error(1, "must be a valid regular expression character set pattern (%d) %s", SPOOFCHECKER_ERROR_CODE(co), u_errorName(SPOOFCHECKER_ERROR_CODE(co)));
		uset_close(set);
		efree(upattern);
		RETURN_THROWS();
	}

	uset_compact(set);
	uspoof_setAllowedChars(co->uspoof, set, SPOOFCHECKER_ERROR_CODE_P(co));
	uset_close(set);
	efree(upattern);

	if (U_FAILURE(SPOOFCHECKER_ERROR_CODE(co))) {
		php_error_docref(NULL, E_WARNING, "(%d) %s", SPOOFCHECKER_ERROR_CODE(co), u_errorName(SPOOFCHECKER_ERROR_CODE(co)));
	}
}
