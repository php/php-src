/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrei Zmievski <andrei@php.net>                            |
  +----------------------------------------------------------------------+
*/

/* $Id$ */ 

#include "php_unicode.h"
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

/* {{{ proto string locale_get_default(void) U
   Returns default locale */
PHP_FUNCTION(locale_get_default)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_STRING(UG(default_locale), 1);
}
/* }}} */

/* {{{ proto bool locale_set_default(string locale) U
	Sets default locale */
PHP_FUNCTION(locale_set_default)
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
	 * validation. See this for possible solution:
	 * http://sourceforge.net/mailarchive/message.php?msg_id=11953411
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
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
