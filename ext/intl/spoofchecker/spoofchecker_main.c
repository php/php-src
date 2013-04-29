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
   | Authors: Scott MacVicar <scottmac@php.net>                           |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_intl.h"
#include "spoofchecker_class.h"

/* {{{ proto bool Spoofchecker::isSuspicious( string text[, int &error_code ] )
 * Checks if a given text contains any suspicious characters
 */
PHP_METHOD(Spoofchecker, isSuspicious)
{
	int ret;
	char *text;
	int text_len;
	zval *error_code = NULL;
	SPOOFCHECKER_METHOD_INIT_VARS;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z", &text, &text_len, &error_code)) {
		return;
	}
	
	SPOOFCHECKER_METHOD_FETCH_OBJECT;

	ret = uspoof_checkUTF8(co->uspoof, text, text_len, NULL, SPOOFCHECKER_ERROR_CODE_P(co));

	if (U_FAILURE(SPOOFCHECKER_ERROR_CODE(co))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "(%d) %s", SPOOFCHECKER_ERROR_CODE(co), u_errorName(SPOOFCHECKER_ERROR_CODE(co)));
		RETURN_TRUE;
	}
	
	if (error_code) {
		zval_dtor(error_code);
		ZVAL_LONG(error_code, ret);
	}
	RETVAL_BOOL(ret != 0);
}
/* }}} */

/* {{{ proto bool Spoofchecker::areConfusable( string str1, string str2[, int &error_code ] )
 * Checks if a given text contains any confusable characters
 */
PHP_METHOD(Spoofchecker, areConfusable)
{
	int ret;
	char *s1, *s2;
	int s1_len, s2_len;
	zval *error_code = NULL;
	SPOOFCHECKER_METHOD_INIT_VARS;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|z", &s1, &s1_len,
										 &s2, &s2_len, &error_code)) {
		return;
	}

	SPOOFCHECKER_METHOD_FETCH_OBJECT;

	ret = uspoof_areConfusableUTF8(co->uspoof, s1, s1_len, s2, s2_len, SPOOFCHECKER_ERROR_CODE_P(co));

	if (U_FAILURE(SPOOFCHECKER_ERROR_CODE(co))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "(%d) %s", SPOOFCHECKER_ERROR_CODE(co), u_errorName(SPOOFCHECKER_ERROR_CODE(co)));
		RETURN_TRUE;
	}
	
	if (error_code) {
		zval_dtor(error_code);
		ZVAL_LONG(error_code, ret);
	}
	RETVAL_BOOL(ret != 0);
}
/* }}} */

/* {{{ proto void Spoofchecker::setAllowedLocales( string locales )
 * Locales to use when running checks
 */
PHP_METHOD(Spoofchecker, setAllowedLocales)
{
	char *locales;
	int locales_len;
	SPOOFCHECKER_METHOD_INIT_VARS;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &locales, &locales_len)) {
		return;
	}

	SPOOFCHECKER_METHOD_FETCH_OBJECT;

	uspoof_setAllowedLocales(co->uspoof, locales, SPOOFCHECKER_ERROR_CODE_P(co));

	if (U_FAILURE(SPOOFCHECKER_ERROR_CODE(co))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "(%d) %s", SPOOFCHECKER_ERROR_CODE(co), u_errorName(SPOOFCHECKER_ERROR_CODE(co)));
		return;
	}
}
/* }}} */

/* {{{ proto void Spoofchecker::setChecks( int checks )
 * Set the checks to run
 */
PHP_METHOD(Spoofchecker, setChecks)
{
	long checks;
	SPOOFCHECKER_METHOD_INIT_VARS;
		
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &checks)) {
		return;
	}

	SPOOFCHECKER_METHOD_FETCH_OBJECT;

	uspoof_setChecks(co->uspoof, checks, SPOOFCHECKER_ERROR_CODE_P(co));

	if (U_FAILURE(SPOOFCHECKER_ERROR_CODE(co))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "(%d) %s", SPOOFCHECKER_ERROR_CODE(co), u_errorName(SPOOFCHECKER_ERROR_CODE(co)));
	}
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
