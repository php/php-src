/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrei Zmievski <andrei@php.net>                            |
  +----------------------------------------------------------------------+
*/

/* $Id$ */ 

#include "php_unicode.h"

#if HAVE_UNICODE
#include "unicode/ubrk.h"

static void php_canonicalize_locale_id(char **target, int32_t *target_len, char *locale, UErrorCode *status)
{
	char *canonicalized = NULL;
	int32_t canonicalized_len = 128;

	while (1) {
		*status = U_ZERO_ERROR;
		canonicalized = erealloc(canonicalized, canonicalized_len + 1);
		canonicalized_len = uloc_canonicalize(locale, canonicalized, canonicalized_len, status);
		if (*status != U_BUFFER_OVERFLOW_ERROR) {
			break;
		}
	}

	canonicalized[canonicalized_len] = 0;
	*target = canonicalized;
	*target_len = canonicalized_len;
}

PHP_FUNCTION(i18n_loc_get_default)
{
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	RETURN_STRING(UG(default_locale), 1);
}

PHP_FUNCTION(i18n_loc_set_default)
{
	char *locale;
	int locale_len;
	char *canonicalized = NULL;
	UErrorCode status = U_ZERO_ERROR;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &locale, &locale_len) == FAILURE) {
		return;
	}

	php_canonicalize_locale_id(&canonicalized, &locale_len, locale, &status);
	/*
	 * UTODO: is this right? canonicalization does not seem to perform locale
	 * validation.
	 */
	if (U_FAILURE(status)) {
		php_error(E_WARNING, "Invalid locale: %s", locale);
		RETURN_FALSE;
	}
	/* don't bother if locales are identical */
	if (!strcmp(UG(default_locale), canonicalized)) {
		efree(canonicalized);
		RETURN_FALSE;
	}
	efree(UG(default_locale));
	UG(default_locale) = canonicalized;
	zend_reset_locale_deps(TSRMLS_C);
	RETURN_TRUE;
}

/* {{{ php_strtotitle
 */
PHPAPI char *php_strtotitle(char *s, size_t len)
{
	s[0] = toupper(s[0]);
	return s;
}
/* }}} */

/* {{{ php_u_strtotitle
 */
PHPAPI UChar* php_u_strtotitle(UChar **s, int32_t *len, const char* locale)
{
	UChar *dest = NULL;
	int32_t dest_len;
	UErrorCode status = U_ZERO_ERROR;
	UBreakIterator *brkiter;
	
	dest_len = *len;
	brkiter = ubrk_open(UBRK_TITLE, locale, *s, *len, &status);
	while (1) {
		status = U_ZERO_ERROR;
		dest = eurealloc(dest, dest_len+1);
		dest_len = u_strToTitle(dest, dest_len, *s, *len, NULL, locale, &status);
		if (status != U_BUFFER_OVERFLOW_ERROR) {
			break;
		}
	}
	ubrk_close(brkiter);

	if (U_SUCCESS(status)) {
		efree(*s);
		dest[dest_len] = 0;
		*s = dest;
		*len = dest_len;
	} else {
		efree(dest);
	}

	return *s;
}
/* }}} */


/* {{{ proto string strtoupper(string str)
   Makes a string uppercase */
PHP_FUNCTION(i18n_strtotitle)
{
	zval **arg;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg)) {
		WRONG_PARAM_COUNT;
	}
	if (Z_TYPE_PP(arg) != IS_STRING && Z_TYPE_PP(arg) != IS_UNICODE) {
		convert_to_text_ex(arg);
	}

	RETVAL_ZVAL(*arg, 1, 0);
	php_u_strtotitle(&Z_USTRVAL_P(return_value), &Z_USTRLEN_P(return_value), UG(default_locale));
}
/* }}} */
#endif /* HAVE_UNICODE */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
