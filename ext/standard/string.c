/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2006 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Stig Sæther Bakken <ssb@php.net>                            |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* Synced with php 3.0 revision 1.193 1999-06-16 [ssb] */

#include <stdio.h>
#include "php.h"
#include "reg.h"
#include "php_rand.h"
#include "php_string.h"
#include "php_variables.h"
#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif
#ifdef HAVE_LANGINFO_H
# include <langinfo.h>
#endif
#ifdef HAVE_MONETARY_H
# include <monetary.h>
#endif
#ifdef HAVE_LIBINTL
# include <libintl.h> /* For LC_MESSAGES */
#endif

#include <math.h>

#include "scanf.h"
#include "zend_API.h"
#include "zend_execute.h"
#include "php_globals.h"
#include "basic_functions.h"
#include "php_smart_str.h"
#ifdef ZTS
#include "TSRM.h"
#endif

#include "unicode/uchar.h"
#include "unicode/ubrk.h"

#define STR_PAD_LEFT			0
#define STR_PAD_RIGHT			1
#define STR_PAD_BOTH			2
#define PHP_PATHINFO_DIRNAME 	1
#define PHP_PATHINFO_BASENAME 	2
#define PHP_PATHINFO_EXTENSION 	4
#define PHP_PATHINFO_FILENAME 	8
#define PHP_PATHINFO_ALL	(PHP_PATHINFO_DIRNAME | PHP_PATHINFO_BASENAME | PHP_PATHINFO_EXTENSION | PHP_PATHINFO_FILENAME)

#define STR_STRSPN				0
#define STR_STRCSPN				1

/* {{{ register_string_constants
 */
void register_string_constants(INIT_FUNC_ARGS)
{
	REGISTER_LONG_CONSTANT("STR_PAD_LEFT", STR_PAD_LEFT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STR_PAD_RIGHT", STR_PAD_RIGHT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("STR_PAD_BOTH", STR_PAD_BOTH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PATHINFO_DIRNAME", PHP_PATHINFO_DIRNAME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PATHINFO_BASENAME", PHP_PATHINFO_BASENAME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PATHINFO_EXTENSION", PHP_PATHINFO_EXTENSION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PATHINFO_FILENAME", PHP_PATHINFO_FILENAME, CONST_CS | CONST_PERSISTENT);

#ifdef HAVE_LOCALECONV
	/* If last members of struct lconv equal CHAR_MAX, no grouping is done */

/* This is bad, but since we are going to be hardcoding in the POSIX stuff anyway... */
# ifndef HAVE_LIMITS_H
# define CHAR_MAX 127
# endif

	REGISTER_LONG_CONSTANT("CHAR_MAX", CHAR_MAX, CONST_CS | CONST_PERSISTENT);
#endif

#ifdef HAVE_LOCALE_H
	REGISTER_LONG_CONSTANT("LC_CTYPE", LC_CTYPE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_NUMERIC", LC_NUMERIC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_TIME", LC_TIME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_COLLATE", LC_COLLATE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_MONETARY", LC_MONETARY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_ALL", LC_ALL, CONST_CS | CONST_PERSISTENT);
# ifdef LC_MESSAGES
	REGISTER_LONG_CONSTANT("LC_MESSAGES", LC_MESSAGES, CONST_CS | CONST_PERSISTENT);
# endif
#endif

}
/* }}} */

int php_tag_find(char *tag, int len, char *set);
static void php_ucwords(zval *str);
static UChar* php_u_strtr_array(UChar *str, int slen, HashTable *hash, int minlen, int maxlen, int *outlen TSRMLS_DC);

/* this is read-only, so it's ok */
static char hexconvtab[] = "0123456789abcdef";

/* localeconv mutex */
#ifdef ZTS
static MUTEX_T locale_mutex = NULL;
#endif

/* {{{ php_bin2hex
 */
static char *php_bin2hex(const unsigned char *old, const size_t oldlen, size_t *newlen)
{
	register unsigned char *result = NULL;
	size_t i, j;

	result = (unsigned char *) safe_emalloc(oldlen * 2, sizeof(char), 1);

	for (i = j = 0; i < oldlen; i++) {
		result[j++] = hexconvtab[old[i] >> 4];
		result[j++] = hexconvtab[old[i] & 15];
	}
	result[j] = '\0';

	if (newlen)
		*newlen = oldlen * 2 * sizeof(char);

	return (char*)result;
}
/* }}} */

#ifdef HAVE_LOCALECONV
/* {{{ localeconv_r
 * glibc's localeconv is not reentrant, so lets make it so ... sorta */
PHPAPI struct lconv *localeconv_r(struct lconv *out)
{
	struct lconv *res;

# ifdef ZTS
	tsrm_mutex_lock( locale_mutex );
# endif

	/* localeconv doesn't return an error condition */
	res = localeconv();

	*out = *res;

# ifdef ZTS
	tsrm_mutex_unlock( locale_mutex );
# endif

	return out;
}
/* }}} */

# ifdef ZTS
/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(localeconv)
{
	locale_mutex = tsrm_mutex_alloc();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(localeconv)
{
	tsrm_mutex_free( locale_mutex );
	locale_mutex = NULL;
	return SUCCESS;
}
/* }}} */
# endif
#endif

/* {{{ proto string bin2hex(string data) U
   Converts the binary representation of data to hex */
PHP_FUNCTION(bin2hex)
{
	unsigned char *data;
	int data_len;
	char *result;
	size_t newlen;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &data, &data_len) == FAILURE) {
		return;
	}

	result = php_bin2hex(data, data_len, &newlen);

	if (!result) {
		RETURN_FALSE;
	}

	RETVAL_ASCII_STRINGL(result, newlen, ZSTR_AUTOFREE);
}
/* }}} */

static void php_spn_common_handler(INTERNAL_FUNCTION_PARAMETERS, int behavior)
{
	void *s1, *s2;
	int len1, len2;
	zend_uchar type1, type2;
	long start, len; /* For UNICODE, these are codepoint units */

	start = 0;
	len = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "TT|ll",
							  &s1, &len1, &type1, &s2, &len2, &type2,
							  &start, &len) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() < 4) {
		len = len1;
	}

	/* look at substr() function for more information */

	if (start < 0) {
		start += len1;
		if (start < 0) {
			start = 0;
		}
	} else if (start > len1) {
		RETURN_FALSE;
	}

	if (len < 0) {
		len += (len1 - start);
		if (len < 0) {
			len = 0;
		}
	}

	if (((unsigned) start + (unsigned) len) > len1) {
		len = len1 - start;
	}

	if (type1 == IS_UNICODE) {
		UChar *u_start, *u_end;
		int32_t i = 0;

		U16_FWD_N((UChar*)s1, i, len1, start);
		u_start = (UChar *)s1 + i;
		U16_FWD_N((UChar *)s1, i, len1, len);
		u_end = (UChar *)s1 + i;

		if (behavior == STR_STRSPN) {
			RETURN_LONG(php_u_strspn(u_start /*str1_start*/,
									 (UChar *)s2 /*str2_start*/,
									 u_end /*str1_end*/,
									 (UChar *)s2 + len2 /*str2_end*/));
		} else if (behavior == STR_STRCSPN) {
			RETURN_LONG(php_u_strcspn(u_start /*str1_start*/,
									  (UChar *)s2 /*str2_start*/,
									  u_end /*str1_end*/,
									  (UChar *)s2 + len2 /*str2_end*/));
		}
	} else {
		if (behavior == STR_STRSPN) {
			RETURN_LONG(php_strspn((char *)s1 + start /*str1_start*/,
								   (char *)s2 /*str2_start*/,
								   (char *)s1 + start + len /*str1_end*/,
								   (char *)s2 + len2 /*str2_end*/));
		} else if (behavior == STR_STRCSPN) {
			RETURN_LONG(php_strcspn((char *)s1 + start /*str1_start*/,
									(char *)s2 /*str2_start*/,
									(char *)s1 + start + len /*str1_end*/,
									(char *)s2 + len2 /*str2_end*/));
		}
	}

}

/* {{{ proto int strspn(string str, string mask [, start [, len]]) U
   Finds length of initial segment consisting entirely of characters found in mask. If start or/and length is provided works like strspn(substr($s,$start,$len),$good_chars) */
PHP_FUNCTION(strspn)
{
	php_spn_common_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, STR_STRSPN);
}
/* }}} */

/* {{{ proto int strcspn(string str, string mask [, start [, len]]) U
   Finds length of initial segment consisting entirely of characters not found in mask. If start or/and length is provide works like strcspn(substr($s,$start,$len),$bad_chars) */
PHP_FUNCTION(strcspn)
{
	php_spn_common_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, STR_STRCSPN);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(nl_langinfo) */
#if HAVE_NL_LANGINFO
PHP_MINIT_FUNCTION(nl_langinfo)
{
#define REGISTER_NL_LANGINFO_CONSTANT(x)	REGISTER_LONG_CONSTANT(#x, x, CONST_CS | CONST_PERSISTENT)
#ifdef ABDAY_1
	REGISTER_NL_LANGINFO_CONSTANT(ABDAY_1);
	REGISTER_NL_LANGINFO_CONSTANT(ABDAY_2);
	REGISTER_NL_LANGINFO_CONSTANT(ABDAY_3);
	REGISTER_NL_LANGINFO_CONSTANT(ABDAY_4);
	REGISTER_NL_LANGINFO_CONSTANT(ABDAY_5);
	REGISTER_NL_LANGINFO_CONSTANT(ABDAY_6);
	REGISTER_NL_LANGINFO_CONSTANT(ABDAY_7);
#endif
#ifdef DAY_1
	REGISTER_NL_LANGINFO_CONSTANT(DAY_1);
	REGISTER_NL_LANGINFO_CONSTANT(DAY_2);
	REGISTER_NL_LANGINFO_CONSTANT(DAY_3);
	REGISTER_NL_LANGINFO_CONSTANT(DAY_4);
	REGISTER_NL_LANGINFO_CONSTANT(DAY_5);
	REGISTER_NL_LANGINFO_CONSTANT(DAY_6);
	REGISTER_NL_LANGINFO_CONSTANT(DAY_7);
#endif
#ifdef ABMON_1
	REGISTER_NL_LANGINFO_CONSTANT(ABMON_1);
	REGISTER_NL_LANGINFO_CONSTANT(ABMON_2);
	REGISTER_NL_LANGINFO_CONSTANT(ABMON_3);
	REGISTER_NL_LANGINFO_CONSTANT(ABMON_4);
	REGISTER_NL_LANGINFO_CONSTANT(ABMON_5);
	REGISTER_NL_LANGINFO_CONSTANT(ABMON_6);
	REGISTER_NL_LANGINFO_CONSTANT(ABMON_7);
	REGISTER_NL_LANGINFO_CONSTANT(ABMON_8);
	REGISTER_NL_LANGINFO_CONSTANT(ABMON_9);
	REGISTER_NL_LANGINFO_CONSTANT(ABMON_10);
	REGISTER_NL_LANGINFO_CONSTANT(ABMON_11);
	REGISTER_NL_LANGINFO_CONSTANT(ABMON_12);
#endif
#ifdef MON_1
	REGISTER_NL_LANGINFO_CONSTANT(MON_1);
	REGISTER_NL_LANGINFO_CONSTANT(MON_2);
	REGISTER_NL_LANGINFO_CONSTANT(MON_3);
	REGISTER_NL_LANGINFO_CONSTANT(MON_4);
	REGISTER_NL_LANGINFO_CONSTANT(MON_5);
	REGISTER_NL_LANGINFO_CONSTANT(MON_6);
	REGISTER_NL_LANGINFO_CONSTANT(MON_7);
	REGISTER_NL_LANGINFO_CONSTANT(MON_8);
	REGISTER_NL_LANGINFO_CONSTANT(MON_9);
	REGISTER_NL_LANGINFO_CONSTANT(MON_10);
	REGISTER_NL_LANGINFO_CONSTANT(MON_11);
	REGISTER_NL_LANGINFO_CONSTANT(MON_12);
#endif
#ifdef AM_STR
	REGISTER_NL_LANGINFO_CONSTANT(AM_STR);
#endif
#ifdef PM_STR
	REGISTER_NL_LANGINFO_CONSTANT(PM_STR);
#endif
#ifdef D_T_FMT
	REGISTER_NL_LANGINFO_CONSTANT(D_T_FMT);
#endif
#ifdef D_FMT
	REGISTER_NL_LANGINFO_CONSTANT(D_FMT);
#endif
#ifdef T_FMT
	REGISTER_NL_LANGINFO_CONSTANT(T_FMT);
#endif
#ifdef T_FMT_AMPM
	REGISTER_NL_LANGINFO_CONSTANT(T_FMT_AMPM);
#endif
#ifdef ERA
	REGISTER_NL_LANGINFO_CONSTANT(ERA);
#endif
#ifdef ERA_YEAR
	REGISTER_NL_LANGINFO_CONSTANT(ERA_YEAR);
#endif
#ifdef ERA_D_T_FMT
	REGISTER_NL_LANGINFO_CONSTANT(ERA_D_T_FMT);
#endif
#ifdef ERA_D_FMT
	REGISTER_NL_LANGINFO_CONSTANT(ERA_D_FMT);
#endif
#ifdef ERA_T_FMT
	REGISTER_NL_LANGINFO_CONSTANT(ERA_T_FMT);
#endif
#ifdef ALT_DIGITS
	REGISTER_NL_LANGINFO_CONSTANT(ALT_DIGITS);
#endif
#ifdef INT_CURR_SYMBOL
	REGISTER_NL_LANGINFO_CONSTANT(INT_CURR_SYMBOL);
#endif
#ifdef CURRENCY_SYMBOL
	REGISTER_NL_LANGINFO_CONSTANT(CURRENCY_SYMBOL);
#endif
#ifdef CRNCYSTR
	REGISTER_NL_LANGINFO_CONSTANT(CRNCYSTR);
#endif
#ifdef MON_DECIMAL_POINT
	REGISTER_NL_LANGINFO_CONSTANT(MON_DECIMAL_POINT);
#endif
#ifdef MON_THOUSANDS_SEP
	REGISTER_NL_LANGINFO_CONSTANT(MON_THOUSANDS_SEP);
#endif
#ifdef MON_GROUPING
	REGISTER_NL_LANGINFO_CONSTANT(MON_GROUPING);
#endif
#ifdef POSITIVE_SIGN
	REGISTER_NL_LANGINFO_CONSTANT(POSITIVE_SIGN);
#endif
#ifdef NEGATIVE_SIGN
	REGISTER_NL_LANGINFO_CONSTANT(NEGATIVE_SIGN);
#endif
#ifdef INT_FRAC_DIGITS
	REGISTER_NL_LANGINFO_CONSTANT(INT_FRAC_DIGITS);
#endif
#ifdef FRAC_DIGITS
	REGISTER_NL_LANGINFO_CONSTANT(FRAC_DIGITS);
#endif
#ifdef P_CS_PRECEDES
	REGISTER_NL_LANGINFO_CONSTANT(P_CS_PRECEDES);
#endif
#ifdef P_SEP_BY_SPACE
	REGISTER_NL_LANGINFO_CONSTANT(P_SEP_BY_SPACE);
#endif
#ifdef N_CS_PRECEDES
	REGISTER_NL_LANGINFO_CONSTANT(N_CS_PRECEDES);
#endif
#ifdef N_SEP_BY_SPACE
	REGISTER_NL_LANGINFO_CONSTANT(N_SEP_BY_SPACE);
#endif
#ifdef P_SIGN_POSN
	REGISTER_NL_LANGINFO_CONSTANT(P_SIGN_POSN);
#endif
#ifdef N_SIGN_POSN
	REGISTER_NL_LANGINFO_CONSTANT(N_SIGN_POSN);
#endif
#ifdef DECIMAL_POINT
	REGISTER_NL_LANGINFO_CONSTANT(DECIMAL_POINT);
#endif
#ifdef RADIXCHAR
	REGISTER_NL_LANGINFO_CONSTANT(RADIXCHAR);
#endif
#ifdef THOUSANDS_SEP
	REGISTER_NL_LANGINFO_CONSTANT(THOUSANDS_SEP);
#endif
#ifdef THOUSEP
	REGISTER_NL_LANGINFO_CONSTANT(THOUSEP);
#endif
#ifdef GROUPING
	REGISTER_NL_LANGINFO_CONSTANT(GROUPING);
#endif
#ifdef YESEXPR
	REGISTER_NL_LANGINFO_CONSTANT(YESEXPR);
#endif
#ifdef NOEXPR
	REGISTER_NL_LANGINFO_CONSTANT(NOEXPR);
#endif
#ifdef YESSTR
	REGISTER_NL_LANGINFO_CONSTANT(YESSTR);
#endif
#ifdef NOSTR
	REGISTER_NL_LANGINFO_CONSTANT(NOSTR);
#endif
#ifdef CODESET
	REGISTER_NL_LANGINFO_CONSTANT(CODESET);
#endif
#undef REGISTER_NL_LANGINFO_CONSTANT
	return SUCCESS;
}
/* }}} */

/* {{{ proto string nl_langinfo(int item)
   Query language and locale information */
PHP_FUNCTION(nl_langinfo)
{
	zval **item;
	char *value;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &item) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(item);

	value = nl_langinfo(Z_LVAL_PP(item));
	if (value == NULL) {
		RETURN_FALSE;
	} else {
		RETURN_STRING(value, 1);
	}
}
#endif
/* }}} */

#ifdef HAVE_STRCOLL
/* {{{ proto int strcoll(string str1, string str2)
   Compares two strings using the current locale */
PHP_FUNCTION(strcoll)
{
	zval **s1, **s2;

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &s1, &s2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(s1);
	convert_to_string_ex(s2);

	RETURN_LONG(strcoll((const char *) Z_STRVAL_PP(s1),
	                    (const char *) Z_STRVAL_PP(s2)));
}
/* }}} */
#endif

/* {{{ php_charmask
 * Fills a 256-byte bytemask with input. You can specify a range like 'a..z',
 * it needs to be incrementing.
 * Returns: FAILURE/SUCCESS whether the input was correct (i.e. no range errors)
 */
static inline int php_charmask(unsigned char *input, int len, char *mask TSRMLS_DC)
{
	unsigned char *end;
	unsigned char c;
	int result = SUCCESS;

	memset(mask, 0, 256);
	for (end = input+len; input < end; input++) {
		c=*input;
		if ((input+3 < end) && input[1] == '.' && input[2] == '.'
				&& input[3] >= c) {
			memset(mask+c, 1, input[3] - c + 1);
			input+=3;
		} else if ((input+1 < end) && input[0] == '.' && input[1] == '.') {
			/* Error, try to be as helpful as possible:
			   (a range ending/starting with '.' won't be captured here) */
			if (end-len >= input) { /* there was no 'left' char */
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid '..'-range, no character to the left of '..'.");
				result = FAILURE;
				continue;
			}
			if (input+2 >= end) { /* there is no 'right' char */
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid '..'-range, no character to the right of '..'.");
				result = FAILURE;
				continue;
			}
			if (input[-1] > input[2]) { /* wrong order */
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid '..'-range, '..'-range needs to be incrementing.");
				result = FAILURE;
				continue;
			}
			/* FIXME: better error (a..b..c is the only left possibility?) */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid '..'-range.");
			result = FAILURE;
			continue;
		} else {
			mask[c]=1;
		}
	}
	return result;
}
/* }}} */

/* {{{ php_trim()
 * mode 1 : trim left
 * mode 2 : trim right
 * mode 3 : trim left and right
 * what indicates which chars are to be trimmed. NULL->default (' \t\n\r\v\0')
 */
PHPAPI char *php_trim(char *c, int len, char *what, int what_len, zend_uchar str_type, zval *return_value, int mode TSRMLS_DC)
{
	register int i;
	int trimmed = 0;
	char mask[256];

	if (what) {
		php_charmask((unsigned char*)what, what_len, mask TSRMLS_CC);
	} else {
		php_charmask((unsigned char*)" \n\r\t\v\0", 6, mask TSRMLS_CC);
	}

	if (mode & 1) {
		for (i = 0; i < len; i++) {
			if (mask[(unsigned char)c[i]]) {
				trimmed++;
			} else {
				break;
			}
		}
		len -= trimmed;
		c += trimmed;
	}
	if (mode & 2) {
		for (i = len - 1; i >= 0; i--) {
			if (mask[(unsigned char)c[i]]) {
				len--;
			} else {
				break;
			}
		}
	}

	if (return_value) {
		RETVAL_STRINGL(c, len, 1);
	} else {
		return estrndup(c, len);
	}
	return "";
}
/* }}} */

/* {{{ php_expand_u_trim_range()
 * Expands possible ranges of the form 'a..b' in input charlist,
 * where a < b in code-point order
 */
static int php_expand_u_trim_range(UChar **range, int *range_len TSRMLS_DC)
{
	UChar32 *codepts, *tmp, *input, *end, c;
	int32_t len, tmp_len, idx;
	UErrorCode err;
	int expanded = 0;
	int result = SUCCESS;

	/* First, convert UTF-16 to UTF-32 */
	len = *range_len;
	codepts = (UChar32 *)emalloc((len+1)*sizeof(UChar32));
	err = U_ZERO_ERROR;
	u_strToUTF32((UChar32 *)codepts, len+1, &len, *range, len, &err);

	/* Expand ranges, if any - taken from php_charmask() */
	tmp_len = len;
	tmp = (UChar32 *)emalloc((tmp_len+1)*sizeof(UChar32));
	input = codepts;
	for ( idx = 0, end = input+len ; input < end ; input++ ) {
		c = input[0];
		if ( (input+3 < end) && input[1] == '.' && input[2] == '.' && input[3] >= c ) {
			tmp_len += (input[3] - c + 1 - 4);
			tmp = (UChar32 *)erealloc(tmp, (tmp_len+1)*sizeof(UChar32));
			for ( ; c <= input[3] ; c++ ) {
				if ( U_IS_UNICODE_CHAR(c) ) tmp[idx++] = c;
			}
			input += 3;
			expanded++;
		} else if ( (input+1 < end) && input[0] == '.' && input[1] == '.' ) {
			/* Error, try to be as helpful as possible:
			   (a range ending/starting with '.' won't be captured here) */
			if ( end-len >= input ) { /* There is no 'left' char */
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid '..'-range, no character to the left of '..'");
				result = FAILURE;
				continue;
			}
			if ( input+2 >= end ) { /* There is no 'right' char */
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid '..'-range, no character to the right of '..'");
				result = FAILURE;
				continue;
			}
			if ( input[-1] > input[2] ) { /* Wrong order */
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid '..'-range, '..'-range needs to be incrementing");
				result = FAILURE;
				continue;
			}
			/* FIXME: Better error (a..b..c is the only left possibility?) */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid '..'-range");
			result = FAILURE;
			continue;
		} else {
			tmp[idx++] = c;
		}
	}

	/* If any ranges were expanded, convert the expanded results back to UTF-16 */
	if ( expanded > 0 ) {
		len = tmp_len;
		*range = (UChar *)erealloc(*range, (len+1)*sizeof(UChar));
		err = U_ZERO_ERROR;
		u_strFromUTF32(*range, len+1, &len, tmp, tmp_len, &err);
		if ( err == U_BUFFER_OVERFLOW_ERROR ) {
			err = U_ZERO_ERROR;
			*range = (UChar *)erealloc(*range, (len+1)*sizeof(UChar));
			u_strFromUTF32(*range, len+1, NULL, tmp, tmp_len, &err);
			if ( U_FAILURE(err) ) { /* Internal ICU error */
				result = FAILURE;
			}
		}
		*range_len = len;
	}

	efree(tmp);
	efree(codepts);

	return result;
}
/* }}} */

/* {{{ php_u_trim()
 * Unicode capable version of php_trim()
 */
static UChar *php_u_trim(UChar *c, int len, UChar *what, int what_len, zval *return_value, int mode TSRMLS_DC)
{
	int32_t	i,j;
	UChar	ch,wh;
	int32_t	start = 0, end = len;

	/*
	 * UTODO
	 * fix trim("a") results in ""
	 */
	if ( what ) {
		what = eustrndup(what, what_len);
		php_expand_u_trim_range(&what, &what_len TSRMLS_CC);
	}

	if ( mode & 1 ) {
		for ( i = 0 ; i < end ; ) {
			U16_NEXT(c, i, end, ch);
			if ( what ) {
				for ( j = 0 ; j < what_len ;  ) {
					U16_NEXT(what, j, what_len, wh);
					if ( wh == ch ) break;
				}
				if ( wh != ch ) break;
			} else {
				if ( u_isWhitespace(ch) == FALSE ) {
					U16_BACK_1(c, 0, i); /* U16_NEXT() post-increments 'i' */
					break;
				}
			}
		}
		start = i;
	}
	if ( mode & 2 ) {
		for ( i = end ; i > start ; ) {
			U16_PREV(c, 0, i, ch);
			if ( what ) {
				for ( j = 0 ; j < what_len ; ) {
					U16_NEXT(what, j, what_len, wh);
					if ( wh == ch ) break;
				}
				if ( wh != ch ) break;
			} else {
				if ( u_isWhitespace(ch) == FALSE ) {
					U16_FWD_1(c, i, end); /* U16_PREV() pre-decrements 'i' */
					break;
				}
			}
		}
		end = i;
	}
	if ( what )
	{
		efree( what );
	}

	if ( start < len ) {
		if ( return_value ) {
			RETVAL_UNICODEL(c+start, end-start, 1);
			return NULL;
		} else {
			return eustrndup(c+start, end-start);
		}
	} else { /* Trimmed the whole string */
		if ( return_value ) {
			RETVAL_EMPTY_UNICODE();
			return NULL;
		} else {
			return (USTR_MAKE(""));
		}
	}
}
/* }}} */

/* {{{ php_do_trim
 * Base for trim(), rtrim() and ltrim() functions.
 */
static void php_do_trim(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	zval **str, **what;
	int  argc = ZEND_NUM_ARGS();

	if (argc < 1 || argc > 2 || zend_get_parameters_ex(argc, &str, &what) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_text_ex(str);

	if (argc > 1) {
		if (Z_TYPE_PP(str) != Z_TYPE_PP(what)) {
			zend_error(E_WARNING, "%v() expects parameter 2 to be string (legacy, Unicode, or binary), %s given",
					get_active_function_name(TSRMLS_C),
					zend_zval_type_name(*what));
		}
		if (Z_TYPE_PP(str) == IS_UNICODE) {
			php_u_trim(Z_USTRVAL_PP(str), Z_USTRLEN_PP(str), Z_USTRVAL_PP(what), Z_USTRLEN_PP(what), return_value, mode TSRMLS_CC);
		} else {
			php_trim(Z_STRVAL_PP(str), Z_STRLEN_PP(str), Z_STRVAL_PP(what), Z_STRLEN_PP(what), Z_TYPE_PP(str), return_value, mode TSRMLS_CC);
		}
	} else {
		if (Z_TYPE_PP(str) == IS_UNICODE) {
			php_u_trim(Z_USTRVAL_PP(str), Z_USTRLEN_PP(str), NULL, 0, return_value, mode TSRMLS_CC);
		} else {
			php_trim(Z_STRVAL_PP(str), Z_STRLEN_PP(str), NULL, 0, Z_TYPE_PP(str), return_value, mode TSRMLS_CC);
		}
	}
}
/* }}} */

/* {{{ proto string trim(string str [, string character_mask]) U
   Strips whitespace from the beginning and end of a string */
PHP_FUNCTION(trim)
{
	php_do_trim(INTERNAL_FUNCTION_PARAM_PASSTHRU, 3);
}
/* }}} */

/* {{{ proto string rtrim(string str [, string character_mask]) U
   Removes trailing whitespace */
PHP_FUNCTION(rtrim)
{
	php_do_trim(INTERNAL_FUNCTION_PARAM_PASSTHRU, 2);
}
/* }}} */

/* {{{ proto string ltrim(string str [, string character_mask]) U
   Strips whitespace from the beginning of a string */
PHP_FUNCTION(ltrim)
{
	php_do_trim(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto string wordwrap(string str [, int width [, string break [, boolean cut]]])
   Wraps buffer to selected number of characters using string break char */
PHP_FUNCTION(wordwrap)
{
	const char *text, *breakchar = "\n";
	char *newtext;
	int textlen, breakcharlen = 1, newtextlen, alloced, chk;
	long current = 0, laststart = 0, lastspace = 0;
	long linelength = 75;
	zend_bool docut = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lsb", &text, &textlen, &linelength, &breakchar, &breakcharlen, &docut) == FAILURE) {
		return;
	}

	if (textlen == 0) {
		RETURN_EMPTY_STRING();
	}

	if (linelength == 0 && docut) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't force cut when width is zero.");
		RETURN_FALSE;
	}

	/* Special case for a single-character break as it needs no
	   additional storage space */
	if (breakcharlen == 1 && !docut) {
		newtext = estrndup(text, textlen);

		laststart = lastspace = 0;
		for (current = 0; current < textlen; current++) {
			if (text[current] == breakchar[0]) {
				laststart = lastspace = current;
			} else if (text[current] == ' ') {
				if (current - laststart >= linelength) {
					newtext[current] = breakchar[0];
					laststart = current + 1;
				}
				lastspace = current;
			} else if (current - laststart >= linelength && laststart != lastspace) {
				newtext[lastspace] = breakchar[0];
				laststart = lastspace + 1;
			}
		}

		RETURN_STRINGL(newtext, textlen, 0);
	} else {
		/* Multiple character line break or forced cut */
		if (linelength > 0) {
			chk = (int)(textlen/linelength + 1);
			newtext = safe_emalloc(chk, breakcharlen, textlen + 1);
			alloced = textlen + chk * breakcharlen + 1;
		} else {
			chk = textlen;
			alloced = textlen * (breakcharlen + 1) + 1;
			newtext = safe_emalloc(textlen, (breakcharlen + 1), 1);
		}

		/* now keep track of the actual new text length */
		newtextlen = 0;

		laststart = lastspace = 0;
		for (current = 0; current < textlen; current++) {
			if (chk <= 0) {
				alloced += (int) (((textlen - current + 1)/linelength + 1) * breakcharlen) + 1;
				newtext = erealloc(newtext, alloced);
				chk = (int) ((textlen - current)/linelength) + 1;
			}
			/* when we hit an existing break, copy to new buffer, and
			 * fix up laststart and lastspace */
			if (text[current] == breakchar[0]
				&& current + breakcharlen < textlen
				&& !strncmp(text+current, breakchar, breakcharlen)) {
				memcpy(newtext+newtextlen, text+laststart, current-laststart+breakcharlen);
				newtextlen += current-laststart+breakcharlen;
				current += breakcharlen - 1;
				laststart = lastspace = current + 1;
				chk--;
			}
			/* if it is a space, check if it is at the line boundary,
			 * copy and insert a break, or just keep track of it */
			else if (text[current] == ' ') {
				if (current - laststart >= linelength) {
					memcpy(newtext+newtextlen, text+laststart, current-laststart);
					newtextlen += current - laststart;
					memcpy(newtext+newtextlen, breakchar, breakcharlen);
					newtextlen += breakcharlen;
					laststart = current + 1;
					chk--;
				}
				lastspace = current;
			}
			/* if we are cutting, and we've accumulated enough
			 * characters, and we haven't see a space for this line,
			 * copy and insert a break. */
			else if (current - laststart >= linelength
					&& docut && laststart >= lastspace) {
				memcpy(newtext+newtextlen, text+laststart, current-laststart);
				newtextlen += current - laststart;
				memcpy(newtext+newtextlen, breakchar, breakcharlen);
				newtextlen += breakcharlen;
				laststart = lastspace = current;
				chk--;
			}
			/* if the current word puts us over the linelength, copy
			 * back up until the last space, insert a break, and move
			 * up the laststart */
			else if (current - laststart >= linelength
					&& laststart < lastspace) {
				memcpy(newtext+newtextlen, text+laststart, lastspace-laststart);
				newtextlen += lastspace - laststart;
				memcpy(newtext+newtextlen, breakchar, breakcharlen);
				newtextlen += breakcharlen;
				laststart = lastspace = lastspace + 1;
				chk--;
			}
		}

		/* copy over any stragglers */
		if (laststart != current) {
			memcpy(newtext+newtextlen, text+laststart, current-laststart);
			newtextlen += current - laststart;
		}

		newtext[newtextlen] = '\0';
		/* free unused memory */
		newtext = erealloc(newtext, newtextlen+1);

		RETURN_STRINGL(newtext, newtextlen, 0);
	}
}
/* }}} */

/* {{{ php_explode
 */
PHPAPI void php_explode(char *delim, uint delim_len, char *str, uint str_len, zend_uchar str_type, zval *return_value, int limit)
{
	char *p1, *p2, *endp;

	endp = str + str_len;
	p1 = str;
	p2 = php_memnstr(str, delim, delim_len, endp);

	if ( p2 == NULL ) {
		add_next_index_stringl(return_value, p1, str_len, 1);
	} else {
		do {
			add_next_index_stringl(return_value, p1, p2-p1, 1);
			p1 = p2 + delim_len;
		} while ( (p2 = php_memnstr(p1, delim, delim_len, endp)) != NULL &&
				  (limit == -1 || --limit > 1) );

		if ( p1 <= endp ) {
			add_next_index_stringl(return_value, p1, endp-p1, 1);
		}
	}
}
/* }}} */

/* {{{ php_explode_negative_limit
 */
PHPAPI void php_explode_negative_limit(char *delim, uint delim_len, char *str, uint str_len, zend_uchar str_type, zval *return_value, int limit)
{
#define EXPLODE_ALLOC_STEP 50
	char *p1, *p2, *endp;
	int allocated = EXPLODE_ALLOC_STEP, found = 0, i = 0, to_return = 0;
	char **positions = safe_emalloc(allocated, sizeof(char *), 0);

	endp = str + str_len;
	p1 = str;
	p2 = php_memnstr(str, delim, delim_len, endp);

	if ( p2 == NULL ) {
		/*
		do nothing since limit <= -1, thus if only one chunk - 1 + (limit) <= 0
		by doing nothing we return empty array
		*/
	} else {
		positions[found++] = p1;
		do {
			if ( found >= allocated ) {
				allocated = found + EXPLODE_ALLOC_STEP;/* make sure we have enough memory */
				positions = erealloc(positions, allocated*sizeof(char *));
			}
			positions[found++] = p1 = p2 + delim_len;
		} while ( (p2 = php_memnstr(p1, delim, delim_len, endp)) != NULL );

		to_return = limit + found;
		/* limit is at least -1 therefore no need of bounds checking : i will be always less than found */
		for ( i = 0 ; i < to_return ; i++ ) { /* this checks also for to_return > 0 */
			add_next_index_stringl(return_value, positions[i],
									   (positions[i+1]-delim_len) - positions[i], 1);
		}
	}
	efree(positions);
#undef EXPLODE_ALLOC_STEP
}
/* }}} */

/* {{{ php_u_explode
 * Unicode capable version of php_explode()
 */
static void php_u_explode(UChar *delim, uint delim_len, UChar *str, uint str_len, zval *return_value, int limit)
{
	UChar *p1, *p2, *endp;

	endp = str + str_len;
	p1 = str;
	p2 = zend_u_memnstr(str, delim, delim_len, endp);

	if ( p2 == NULL ) {
		add_next_index_unicodel(return_value, p1, str_len, 1);
	} else {
		do {
			add_next_index_unicodel(return_value, p1, p2-p1, 1);
			p1 = (UChar *)p2 + delim_len;
		} while ((p2 = zend_u_memnstr(p1, delim, delim_len, endp)) != NULL &&
				  (limit == -1 || --limit > 1) );

		if ( p1 <= endp ) {
			add_next_index_unicodel(return_value, p1, endp-p1, 1);
		}
	}
}
/* }}} */

/* {{{ php_u_explode_negative_limit
 * Unicode capable version of php_explode_negative_limit()
 */
static void php_u_explode_negative_limit(UChar *delim, uint delim_len, UChar *str, uint str_len, zval *return_value, int limit)
{
#define EXPLODE_ALLOC_STEP 50
	UChar *p1, *p2, *endp;
	int allocated = EXPLODE_ALLOC_STEP, found = 0, i = 0, to_return = 0;
	UChar **positions = safe_emalloc(allocated, sizeof(UChar *), 0);

	endp = str + str_len;
	p1 = str;
	p2 = zend_u_memnstr(str, delim, delim_len, endp);

	if ( p2 == NULL ) {
		/*
		do nothing since limit <= -1, thus if only one chunk - 1 + (limit) <= 0
		by doing nothing we return empty array
		*/
	} else {
		positions[found++] = p1;
		do {
			if ( found >= allocated ) {
				allocated = found + EXPLODE_ALLOC_STEP;/* make sure we have enough memory */
				positions = erealloc(positions, allocated*sizeof(UChar *));
			}
			positions[found++] = p1 = p2 + delim_len;
		} while ( (p2 = zend_u_memnstr(p1, delim, delim_len, endp)) != NULL );

		to_return = limit + found;
		/* limit is at least -1 therefore no need of bounds checking : i will be always less than found */
		for ( i = 0 ; i < to_return ; i++ ) { /* this checks also for to_return > 0 */
			add_next_index_unicodel(return_value, positions[i],
									(positions[i+1]-delim_len) - positions[i], 1);
		}
	}
	efree(positions);
#undef EXPLODE_ALLOC_STEP
}
/* }}} */

/* {{{ proto array explode(string separator, string str [, int limit]) U
   Splits a string on string separator and return array of components. If limit is positive only limit number of components is returned. If limit is negative all components except the last abs(limit) are returned. */
PHP_FUNCTION(explode)
{
	void		*str, *delim;
	int			str_len, delim_len;
	zend_uchar	str_type, delim_type;
	long		limit = -1;
	int			argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "TT|l", &delim, &delim_len, &delim_type,
							  &str, &str_len, &str_type, &limit) == FAILURE) {
		return;
	}

	if ( delim_len == 0 ) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty delimiter.");
		RETURN_FALSE;
	}

	array_init(return_value);

	if ( str_len == 0 ) {
		if ( str_type == IS_UNICODE ) {
			add_next_index_unicodel(return_value, USTR_MAKE(""), sizeof("")-1, 0);
		} else {
			add_next_index_stringl(return_value, "", sizeof("")-1, 1);
		}
		return;
	}


	if (limit == 0 || limit == 1) {
		if ( str_type == IS_UNICODE ) {
			add_index_unicodel(return_value, 0, (UChar *)str, str_len, 1);
		} else {
			add_index_stringl(return_value, 0, (char *)str, str_len, 1);
		}
	} else if (limit < 0 && argc == 3) {
		if ( str_type == IS_UNICODE ) {
			php_u_explode_negative_limit((UChar *)delim, delim_len, (UChar *)str, str_len, return_value, limit);
		} else {
			php_explode_negative_limit((char *)delim, delim_len, (char *)str, str_len, str_type, return_value, limit);
		}
	} else {
		if ( str_type == IS_UNICODE ) {
			php_u_explode((UChar *)delim, delim_len, (UChar *)str, str_len, return_value, limit);
		} else {
			php_explode((char *)delim, delim_len, (char *)str, str_len, str_type, return_value, limit);
		}
	}
}
/* }}} */

/* {{{ proto string join([string glue,] array pieces) U
   An alias for implode */
/* }}} */

/* {{{ php_implode
 */
PHPAPI void php_implode(zval *delim, zval *arr, zval *retval)
{
	zend_uchar		return_type;
	int				numelems, i=0;
	HashPosition	pos;
	zval			**tmp;
	TSRMLS_FETCH();

	Z_TYPE_P(retval) = return_type = Z_TYPE_P(delim); /* ... to start off */

	/* Setup return value */
	if (return_type == IS_UNICODE) {
		ZVAL_EMPTY_UNICODE(retval);
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	numelems = zend_hash_num_elements(Z_ARRVAL_P(arr));
	if (numelems == 0) {
		return;
	}

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(arr), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(arr), (void **)&tmp, &pos) == SUCCESS) {
		if (Z_TYPE_PP(tmp) != return_type) {
			if (return_type == IS_UNICODE) {
				if (Z_TYPE_PP(tmp) == IS_STRING) {
					/* ERROR */
					php_error_docref(NULL TSRMLS_CC, E_WARNING,
									 "Cannot mix binary strings with other string types");
					efree(Z_USTRVAL_P(retval));
					ZVAL_FALSE(retval);
					return;
				} else {
					SEPARATE_ZVAL(tmp);
					convert_to_unicode_ex(tmp);
				}
			} else if (return_type == IS_STRING) {
				if (Z_TYPE_PP(tmp) == IS_UNICODE) {
					/* ERROR */
					php_error_docref(NULL TSRMLS_CC, E_WARNING,
									 "Cannot mix binary strings with other string types");
					efree(Z_STRVAL_P(retval));
					ZVAL_FALSE(retval);
					return;
				} else {
					SEPARATE_ZVAL(tmp);
					convert_to_string_ex(tmp);
				}
			} else {
				if (Z_TYPE_PP(tmp) == IS_UNICODE) {
					/* Convert IS_STRING up to IS_UNICODE */
					convert_to_unicode_ex(&retval);
					convert_to_unicode_ex(&delim);
					Z_TYPE_P(retval) = return_type = IS_UNICODE;
				} else {
					SEPARATE_ZVAL(tmp);
					convert_to_string_ex(tmp);
				}
			}
		}

		/* Append elem */
		if (return_type == IS_UNICODE) {
			Z_USTRVAL_P(retval) = eurealloc(Z_USTRVAL_P(retval),
											Z_USTRLEN_P(retval)+Z_USTRLEN_PP(tmp)+1);
			memcpy(Z_USTRVAL_P(retval)+Z_USTRLEN_P(retval), Z_USTRVAL_PP(tmp), UBYTES(Z_USTRLEN_PP(tmp)+1));
			Z_USTRLEN_P(retval) += Z_USTRLEN_PP(tmp);
			if (++i < numelems) { /* Append delim */
				Z_USTRVAL_P(retval) = eurealloc(Z_USTRVAL_P(retval),
												Z_USTRLEN_P(retval)+Z_USTRLEN_P(delim)+1);
				memcpy(Z_USTRVAL_P(retval)+Z_USTRLEN_P(retval), Z_USTRVAL_P(delim), UBYTES(Z_USTRLEN_P(delim)+1));
				Z_USTRLEN_P(retval) += Z_USTRLEN_P(delim);
			}
		} else {
			Z_STRVAL_P(retval) = (char *)erealloc(Z_STRVAL_P(retval),
												  Z_STRLEN_P(retval)+Z_STRLEN_PP(tmp)+1);
			memcpy(Z_STRVAL_P(retval)+Z_STRLEN_P(retval), Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp)+1);
			Z_STRLEN_P(retval) += Z_STRLEN_PP(tmp);
			if (++i < numelems) { /* Append delim */
				Z_STRVAL_P(retval) = (char *)erealloc(Z_STRVAL_P(retval),
													  Z_STRLEN_P(retval)+Z_STRLEN_P(delim)+1);
				memcpy(Z_STRVAL_P(retval)+Z_STRLEN_P(retval), Z_STRVAL_P(delim), Z_STRLEN_P(delim)+1);
				Z_STRLEN_P(retval) += Z_STRLEN_P(delim);
			}
		}

		zend_hash_move_forward_ex(Z_ARRVAL_P(arr), &pos);
	}

	return;
}
/* }}} */

/* {{{ proto string implode([string glue,] array pieces) U
   Joins array elements placing glue string between items and return one string */
PHP_FUNCTION(implode)
{
	zval	**arg1 = NULL, **arg2 = NULL;
	zval	*delim, *arr;
	int		argc = ZEND_NUM_ARGS();

	if (argc < 1 || argc > 2) {
		WRONG_PARAM_COUNT;
	}
	if (zend_get_parameters_ex(argc, &arg1, &arg2) == FAILURE) {
		return;
	}

	if (argc == 1) {
		if (Z_TYPE_PP(arg1) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument to implode must be an array.");
			RETURN_FALSE;
		} else {
			SEPARATE_ZVAL(arg1);
			arr = *arg1;
			MAKE_STD_ZVAL(delim);
			if (UG(unicode)) {
				ZVAL_UNICODEL(delim, EMPTY_STR, sizeof("")-1, 0);
			} else {
				ZVAL_STRINGL(delim, "", sizeof("")-1, 0);
			}
		}
	} else {
		if (Z_TYPE_PP(arg1) == IS_ARRAY) {
			SEPARATE_ZVAL(arg1);
			arr = *arg1;
			if (Z_TYPE_PP(arg2) != IS_UNICODE && Z_TYPE_PP(arg2) != IS_STRING) {
				convert_to_text_ex(arg2);
			}
			delim = *arg2;
		} else if (Z_TYPE_PP(arg2) == IS_ARRAY) {
			SEPARATE_ZVAL(arg2);
			arr = *arg2;
			if (Z_TYPE_PP(arg1) != IS_UNICODE && Z_TYPE_PP(arg1) != IS_STRING) {
				convert_to_text_ex(arg1);
			}
			delim = *arg1;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad arguments.");
			RETURN_FALSE;
		}
	}

	php_implode(delim, arr, return_value);

	if (argc == 1) {
		FREE_ZVAL(delim);
	}
}
/* }}} */

#define STRTOK_TABLE(p) BG(strtok_table)[(unsigned char) *p]

/* {{{ proto string strtok([string str,] string token) U
   Tokenize a string */
PHP_FUNCTION(strtok)
{
	void *tok, *str;
	int tok_len, str_len;
	zend_uchar tok_type, str_type;
	zval *zv;
	char *token, *token_end, *p, *pe;
	UChar *u_token, *u_p, *u_pe;

	UChar32 ch, th;
	int32_t start = 0, end, i, j, rem_len;
	int delim_found, token_present;
	int skipped = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "T|T",
							  &str, &str_len, &str_type,
							  &tok, &tok_len, &tok_type) == FAILURE) {
		return;
	}

	switch (ZEND_NUM_ARGS()) {
		case 1:
			tok = str;
			tok_len = str_len;
			tok_type = str_type;
			break;

		default:
		case 2:
			if (BG(strtok_zval)) {
				zval_ptr_dtor(&BG(strtok_zval));
			}
			MAKE_STD_ZVAL(zv);
			if (str_type == IS_UNICODE) {
				ZVAL_UNICODEL(zv, (UChar *)str, str_len, 1);
			} else {
				ZVAL_STRINGL(zv, (char *)str, str_len, 1);
			}
			BG(strtok_zval) = zv;
			if (str_type == IS_UNICODE) {
				BG(strtok_last) = BG(strtok_string) = Z_USTRVAL_P(zv);
			} else {
				BG(strtok_last) = BG(strtok_string) = Z_STRVAL_P(zv);
			}
			BG(strtok_len) = str_len;
			break;
	}

	if (BG(strtok_zval) && tok_type != Z_TYPE_P(BG(strtok_zval))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Delimiter type must match string type.");
		RETURN_FALSE;
	}

	if (tok_type == IS_UNICODE) {
		u_p = (UChar *)BG(strtok_last); /* Where we start to search */
		u_pe = (UChar *)BG(strtok_string) + BG(strtok_len);
		u_token = (UChar *)tok;
		if (!u_p || u_p >= u_pe) {
			RETURN_FALSE;
		}
		rem_len = u_pe - u_p;

		/* Skip leading delimiters */
		token_present = 0;
		for (i = 0 ; (u_p + i) < u_pe ; ) {
			delim_found = 0;
			U16_NEXT(u_p, i, rem_len, ch);
			for (j = 0 ; j < tok_len ; ) {
				U16_NEXT(u_token, j, tok_len, th);
				if ( ch == th ) {
					delim_found = 1;
					break;
				}
			}
			if (delim_found == 0) {
				U16_BACK_1(u_p, 0, i); /* U16_NEXT() post-incrs 'i' */
				start = i;
				token_present = 1;
				break;
			}
		}
		if (token_present == 0) {
			BG(strtok_last) = NULL;
			RETURN_FALSE;
		}

		/* Seek to next delimiter */
		delim_found = 0;
		for (i = start ; (u_p + i) < u_pe ; ) {
			U16_NEXT(u_p, i, rem_len, ch);
			for (j = 0 ; j < tok_len ; ) {
				U16_NEXT(u_token, j, tok_len, th);
				if ( ch == th ) {
					delim_found = 1;
					break;
				}
			}
			if (delim_found) {
				U16_BACK_1(u_p, 0, i); /* 'i' was beyond delimiter */
				break;
			}
		}
		end = i;

		if (end - start) {
			BG(strtok_last) = u_p + end;
			RETURN_UNICODEL(u_p + start, end - start, 1);
		} else {
			BG(strtok_last) = NULL;
			RETURN_FALSE;
		}
	} else {
		p = (char *)BG(strtok_last); /* Where we start to search */
		pe = (char *)BG(strtok_string) + BG(strtok_len);
		if (!p || p >= pe) {
			RETURN_FALSE;
		}
		token = (char *)tok;
		token_end = token + tok_len;
		while (token < token_end) {
			STRTOK_TABLE(token++) = 1;
		}

		/* Skip leading delimiters */
		while (STRTOK_TABLE(p)) {
			if (++p >= pe) {
				/* no other chars left */
				BG(strtok_last) = NULL;
				RETVAL_FALSE;
				goto restore;
			}
			skipped++;
		}
		/* We know at this place that *p is no delimiter, so skip it */
		while (++p < pe) {
			if (STRTOK_TABLE(p)) {
				goto return_token;
			}
		}

		if (p - (char *)BG(strtok_last)) {
return_token:
			RETVAL_STRINGL((char *)BG(strtok_last) + skipped, (p - (char *)BG(strtok_last)) - skipped, 1);
			BG(strtok_last) = p + 1;
		} else {
			RETVAL_FALSE;
			BG(strtok_last) = NULL;
		}

		/* Restore table -- usually faster then memset'ing the table on every invocation */
restore:
		token = (char *)tok;
		while (token < token_end) {
			STRTOK_TABLE(token++) = 0;
		}
	}
}
/* }}} */

/* {{{ php_strtoupper
 */
PHPAPI char *php_strtoupper(char *s, size_t len)
{
	unsigned char *c, *e;

	c = (unsigned char*)s;
	e = c+len;

	while (c < e) {
		*c = toupper(*c);
		c++;
	}
	return s;
}
/* }}} */

/* {{{ php_u_strtoupper
 */
PHPAPI UChar* php_u_strtoupper(UChar *s, int *len, const char* locale)
{
	UChar *dest = NULL;
	int dest_len;
	UErrorCode status;

	dest_len = *len;
	while (1) {
		status = U_ZERO_ERROR;
		dest = eurealloc(dest, dest_len+1);
		dest_len = u_strToUpper(dest, dest_len, s, *len, locale, &status);
		if (status != U_BUFFER_OVERFLOW_ERROR) {
			break;
		}
	}

	if (U_SUCCESS(status)) {
		dest[dest_len] = 0;
		*len = dest_len;
		return dest;
	} else {
		efree(dest);
		return NULL;
	}
}
/* }}} */

/* {{{ proto string strtoupper(string str) U
   Makes a string uppercase */
PHP_FUNCTION(strtoupper)
{
	zstr str;
	int str_len;
	zend_uchar str_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &str, &str_len, &str_type) == FAILURE) {
		return;
	}

	if (str_type == IS_UNICODE) {
		UChar *result;
		if ((result = php_u_strtoupper(str.u, &str_len, UG(default_locale)))) {
			RETURN_UNICODEL(result, str_len, 0);
		} else {
			RETURN_EMPTY_UNICODE();
		}
	} else {
		RETVAL_STRINGL(str.s, str_len, 1);
		php_strtoupper(Z_STRVAL_P(return_value), Z_STRLEN_P(return_value));
	}
}
/* }}} */

/* {{{ php_u_strtolower
 */
PHPAPI UChar *php_u_strtolower(UChar *s, int *len, const char* locale)
{
	UChar *dest = NULL;
	int dest_len;
	UErrorCode status = U_ZERO_ERROR;

	dest_len = *len;
	while (1) {
		status = U_ZERO_ERROR;
		dest = eurealloc(dest, dest_len+1);
		dest_len = u_strToLower(dest, dest_len, s, *len, locale, &status);
		if (status != U_BUFFER_OVERFLOW_ERROR) {
			break;
		}
	}

	if (U_SUCCESS(status)) {
		dest[dest_len] = 0;
		*len = dest_len;
		return dest;
	} else {
		efree(dest);
		return NULL;
	}
}
/* }}} */

/* {{{ php_strtolower
 */
PHPAPI char *php_strtolower(char *s, size_t len)
{
	unsigned char *c, *e;

	c = (unsigned char*)s;
	e = c+len;

	while (c < e) {
		*c = tolower(*c);
		c++;
	}
	return s;
}
/* }}} */

/* {{{ proto string strtolower(string str) U
   Makes a string lowercase */
PHP_FUNCTION(strtolower)
{
	zstr str;
	int str_len;
	zend_uchar str_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &str, &str_len, &str_type) == FAILURE) {
		return;
	}

	if (str_type == IS_UNICODE) {
		UChar *result;
		if ((result = php_u_strtolower(str.u, &str_len, UG(default_locale)))) {
			RETURN_UNICODEL(result, str_len, 0);
		} else {
			RETURN_EMPTY_UNICODE();
		}
	} else {
		RETVAL_STRINGL(str.s, str_len, 1);
		php_strtolower(Z_STRVAL_P(return_value), Z_STRLEN_P(return_value));
	}
}
/* }}} */

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
PHPAPI UChar* php_u_strtotitle(UChar *s, int32_t *len, const char* locale)
{
	UChar *dest = NULL;
	int32_t dest_len;
	UErrorCode status = U_ZERO_ERROR;
	UBreakIterator *brkiter;

	dest_len = *len;
	brkiter = ubrk_open(UBRK_WORD, locale, s, *len, &status);
	while (1) {
		status = U_ZERO_ERROR;
		dest = eurealloc(dest, dest_len+1);
		dest_len = u_strToTitle(dest, dest_len, s, *len, NULL, locale, &status);
		if (status != U_BUFFER_OVERFLOW_ERROR) {
			break;
		}
	}
	ubrk_close(brkiter);

	if (U_SUCCESS(status)) {
		dest[dest_len] = 0;
		*len = dest_len;
		return dest;
	} else {
		efree(dest);
		return NULL;
	}
}
/* }}} */


/* {{{ proto string strtotitle(string str) U
   Makes a string titlecase */
PHP_FUNCTION(strtotitle)
{
	zstr str;
	int str_len;
	zend_uchar str_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &str, &str_len, &str_type) == FAILURE) {
		return;
	}

	if (str_type == IS_UNICODE && str_len == 0) {
		RETURN_EMPTY_UNICODE();
	} else if (str_len == 0) {
		RETURN_EMPTY_STRING();
	}

	if (str_type == IS_UNICODE) {
		UChar *result;
		if ((result = php_u_strtotitle(str.u, &str_len, UG(default_locale)))) {
			RETURN_UNICODEL(result, str_len, 0);
		} else {
			RETURN_EMPTY_UNICODE();
		}
	} else {
		RETVAL_STRINGL(str.s, str_len, 1);
		php_ucwords(return_value);
	}
}
/* }}} */

/* {{{ php_u_basename
 */
PHPAPI void php_u_basename(UChar *s, int len, UChar *suffix, int sufflen, UChar **p_ret, int *p_len TSRMLS_DC)
{
	UChar *ret = NULL, *end, *c, *comp, *cend;
	int state;

	c = comp = cend = s;
	end = s + len;
	state = 0;
	while (c < end) {
#if defined(PHP_WIN32) || defined(NETWARE)
		if (*c == (UChar) 0x2f /*'/'*/ || *c == (UChar) 0x5c /*'\\'*/) {
#else
		if (*c == (UChar) 0x2f /*'/'*/) {
#endif
			if (state == 1) {
				state = 0;
				cend = c;
			}
		} else {
			if (state == 0) {
				comp = c;
				state = 1;
			}
		}
		c++;
	}

	if (state == 1) {
		cend = c;
	}
	if (suffix != NULL && sufflen < (cend - comp) &&
			u_memcmp(cend - sufflen, suffix, sufflen) == 0) {
		cend -= sufflen;
	}

	len = cend - comp;
	ret = eustrndup(comp, len);

	if (p_ret) {
		*p_ret = ret;
	}
	if (p_len) {
		*p_len = len;
	}
}
/* }}} */

/* {{{ php_basename
 */
PHPAPI void php_basename(char *s, int len, char *suffix, int sufflen, char **p_ret, int *p_len TSRMLS_DC)
{
	char *ret = NULL, *c, *comp, *cend;
	size_t inc_len, cnt;
	int state;

	c = comp = cend = s;
	cnt = len;
	state = 0;
	while (cnt > 0) {
		inc_len = (*c == '\0' ? 1: php_mblen(c, cnt));

		switch (inc_len) {
			case -2:
			case -1:
				inc_len = 1;
				php_mblen(NULL, 0);
				break;
			case 0:
				goto quit_loop;
			case 1:
#if defined(PHP_WIN32) || defined(NETWARE)
				if (*c == '/' || *c == '\\') {
#else
				if (*c == '/') {
#endif
					if (state == 1) {
						state = 0;
						cend = c;
					}
				} else {
					if (state == 0) {
						comp = c;
						state = 1;
					}
				}
				break;
			default:
				if (state == 0) {
					comp = c;
					state = 1;
				}
				break;
		}
		c += inc_len;
		cnt -= inc_len;
	}

quit_loop:
	if (state == 1) {
		cend = c;
	}
	if (suffix != NULL && sufflen < (cend - comp) &&
			memcmp(cend - sufflen, suffix, sufflen) == 0) {
		cend -= sufflen;
	}

	len = cend - comp;
	ret = emalloc(len + 1);
	memcpy(ret, comp, len);
	ret[len] = '\0';

	if (p_ret) {
		*p_ret = ret;
	}
	if (p_len) {
		*p_len = len;
	}
}
/* }}} */

/* {{{ proto string basename(string path [, string suffix]) U
   Returns the filename component of the path */
PHP_FUNCTION(basename)
{
	zstr string, suffix = NULL_ZSTR, ret;
	int string_len, suffix_len = 0;
	zend_uchar string_type, suffix_type;
	int ret_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "T|T", &string, &string_len, &string_type, &suffix, &suffix_len, &suffix_type) == FAILURE) {
		return;
	}

	if (string_type == IS_UNICODE) {
		php_u_basename(string.u, string_len, suffix.u, suffix_len, &ret.u, &ret_len TSRMLS_CC);
		RETURN_UNICODEL(ret.u, ret_len, 0);
	} else {
		php_basename(string.s, string_len, suffix.s, suffix_len, &ret.s, &ret_len TSRMLS_CC);
		RETURN_STRINGL(ret.s, ret_len, 0);
	}
}
/* }}} */

/* {{{ php_u_dirname
   Returns directory name component of path */
PHPAPI int php_u_dirname(UChar *path, int len)
{
	register UChar *end = path + len - 1;
	unsigned int len_adjust = 0;

#ifdef PHP_WIN32
	/* Note that on Win32 CWD is per drive (heritage from CP/M).
	 * This means dirname("c:foo") maps to "c:." or "c:" - which means CWD on C: drive.
	 */
	if ((2 <= len) && u_isalpha((UChar32)path[0]) && ((UChar)0x3a /*':'*/ == path[1])) {
		/* Skip over the drive spec (if any) so as not to change */
		path += 2;
		len_adjust += 2;
		if (2 == len) {
			/* Return "c:" on Win32 for dirname("c:").
			 * It would be more consistent to return "c:."
			 * but that would require making the string *longer*.
			 */
			return len;
		}
	}
#elif defined(NETWARE)
	/*
	 * Find the first occurence of : from the left
	 * move the path pointer to the position just after :
	 * increment the len_adjust to the length of path till colon character(inclusive)
	 * If there is no character beyond : simple return len
	 */
	UChar *colonpos = NULL;
	colonpos = u_strchr(path, (UChar) 0x3a /*':'*/);
	if(colonpos != NULL) {
		len_adjust = ((colonpos - path) + 1);
		path += len_adjust;
		if(len_adjust == len) {
		return len;
		}
    	}
#endif

	if (len == 0) {
		/* Illegal use of this function */
		return 0;
	}

	/* Strip trailing slashes */
	while (end >= path && IS_U_SLASH_P(end)) {
		end--;
	}
	if (end < path) {
		/* The path only contained slashes */
		path[0] = DEFAULT_U_SLASH;
		path[1] = 0;
		return 1 + len_adjust;
	}

	/* Strip filename */
	while (end >= path && !IS_U_SLASH_P(end)) {
		end--;
	}
	if (end < path) {
		/* No slash found, therefore return '.' */
#ifdef NETWARE
		if(len_adjust == 0) {
			path[0] = (UChar) 0x2e /*'.'*/;
			path[1] = 0;
			return 1; //only one character
		}
		else {
			path[0] = 0;
			return len_adjust;
		}
#else
		path[0] = (UChar) 0x2e /*'.'*/;
		path[1] = 0;
		return 1 + len_adjust;
#endif
	}

	/* Strip slashes which came before the file name */
	while (end >= path && IS_U_SLASH_P(end)) {
		end--;
	}
	if (end < path) {
		path[0] = DEFAULT_U_SLASH;
		path[1] = 0;
		return 1 + len_adjust;
	}
	*(end+1) = 0;

	return (size_t)(end + 1 - path) + len_adjust;
}
/* }}} */

/* {{{ php_dirname
   Returns directory name component of path */
PHPAPI int php_dirname(char *path, int len)
{
	register char *end = path + len - 1;
	unsigned int len_adjust = 0;

#ifdef PHP_WIN32
	/* Note that on Win32 CWD is per drive (heritage from CP/M).
	 * This means dirname("c:foo") maps to "c:." or "c:" - which means CWD on C: drive.
	 */
	if ((2 <= len) && isalpha((int)((unsigned char *)path)[0]) && (':' == path[1])) {
		/* Skip over the drive spec (if any) so as not to change */
		path += 2;
		len_adjust += 2;
		if (2 == len) {
			/* Return "c:" on Win32 for dirname("c:").
			 * It would be more consistent to return "c:."
			 * but that would require making the string *longer*.
			 */
			return len;
		}
	}
#elif defined(NETWARE)
	/*
	 * Find the first occurence of : from the left
	 * move the path pointer to the position just after :
	 * increment the len_adjust to the length of path till colon character(inclusive)
	 * If there is no character beyond : simple return len
	 */
	char *colonpos = NULL;
	colonpos = strchr(path, ':');
	if(colonpos != NULL) {
		len_adjust = ((colonpos - path) + 1);
		path += len_adjust;
		if(len_adjust == len) {
		return len;
		}
    	}
#endif

	if (len == 0) {
		/* Illegal use of this function */
		return 0;
	}

	/* Strip trailing slashes */
	while (end >= path && IS_SLASH_P(end)) {
		end--;
	}
	if (end < path) {
		/* The path only contained slashes */
		path[0] = DEFAULT_SLASH;
		path[1] = '\0';
		return 1 + len_adjust;
	}

	/* Strip filename */
	while (end >= path && !IS_SLASH_P(end)) {
		end--;
	}
	if (end < path) {
		/* No slash found, therefore return '.' */
#ifdef NETWARE
		if(len_adjust == 0) {
			path[0] = '.';
			path[1] = '\0';
			return 1; //only one character
		}
		else {
			path[0] = '\0';
			return len_adjust;
		}
#else
		path[0] = '.';
		path[1] = '\0';
		return 1 + len_adjust;
#endif
	}

	/* Strip slashes which came before the file name */
	while (end >= path && IS_SLASH_P(end)) {
		end--;
	}
	if (end < path) {
		path[0] = DEFAULT_SLASH;
		path[1] = '\0';
		return 1 + len_adjust;
	}
	*(end+1) = '\0';

	return (size_t)(end + 1 - path) + len_adjust;
}
/* }}} */

/* {{{ proto string dirname(string path) U
   Returns the directory name component of the path */
PHP_FUNCTION(dirname)
{
	zstr str;
	int str_len;
	zend_uchar str_type;
	zstr ret;
	int ret_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &str, &str_len,
							  &str_type) == FAILURE) {
		return;
	}

	if (str_type == IS_UNICODE) {
		ret.u = eustrndup(str.u, str_len);
		ret_len = php_u_dirname(ret.u, str_len);
	} else {
		ret.s = estrndup(str.s, str_len);
		ret_len = php_dirname(ret.s, str_len);
	}

	RETURN_ZSTRL(ret, ret_len, str_type, 0);
}
/* }}} */

/* {{{ proto array pathinfo(string path[, int options]) U
   Returns information about a certain string */
PHP_FUNCTION(pathinfo)
{
	zstr path, ret = NULL_ZSTR;
	int path_len, have_basename, have_ext, have_filename;
	zend_uchar path_type;
	int ret_len;
	zval *tmp;
	long opt = PHP_PATHINFO_ALL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t|l", &path, &path_len, &path_type, &opt) == FAILURE) {
		return;
	}

	have_basename = ((opt & PHP_PATHINFO_BASENAME) == PHP_PATHINFO_BASENAME);
	have_filename = ((opt & PHP_PATHINFO_FILENAME) == PHP_PATHINFO_FILENAME);
	have_ext = ((opt & PHP_PATHINFO_EXTENSION) == PHP_PATHINFO_EXTENSION);

	MAKE_STD_ZVAL(tmp);
	array_init(tmp);

	if ((opt & PHP_PATHINFO_DIRNAME) == PHP_PATHINFO_DIRNAME) {
		if (path_type == IS_UNICODE) {
			ret.u = eustrndup(path.u, path_len);
			ret_len = php_u_dirname(ret.u, path_len);
		} else {
			ret.s = estrndup(path.s, path_len);
			ret_len = php_dirname(ret.s, path_len);
		}
		if (ret_len > 0) {
			add_assoc_zstrl(tmp, "dirname", path_type, ret, ret_len, 1);
		}
		efree(ret.v);
		ret = NULL_ZSTR;
	}

	if (have_basename || have_ext || have_filename) {
		if (path_type == IS_UNICODE) {
			php_u_basename(path.u, path_len, NULL, 0, &ret.u, &ret_len TSRMLS_CC);
		} else {
			php_basename(path.s, path_len, NULL, 0, &ret.s, &ret_len TSRMLS_CC);
		}
	}
	
	if (have_basename) {
		add_assoc_zstrl(tmp, "basename", path_type, ret, ret_len, 0);
	}

	if (have_ext) {
		zstr ext;
		int ext_len;

		if (path_type == IS_UNICODE) {
			ext.u = u_strrchr(ret.u, (UChar) 0x2e /*'.'*/);
			if (ext.u) {
				ext.u++;
				ext_len = ret_len - (ext.u - ret.u);
			}
		} else {
			ext.s = strrchr(ret.s, '.');
			if (ext.s) {
				ext.s++;
				ext_len = ret_len - (ext.s - ret.s);
			}
		}

		if (ext.v) {
			add_assoc_zstrl(tmp, "extension", path_type, ext, ext_len, 1);
		}
	}

	if (have_filename) {
		zstr p;
		int idx;

		if (path_type == IS_UNICODE) {
			p.u = u_strrchr(ret.u, (UChar) 0x2e /*'.'*/);
			idx = p.u ? (p.u - ret.u) : ret_len;
		} else {
			p.s = strrchr(ret.s, '.');
			idx = p.s ? (p.s - ret.s) : ret_len;
		}

		add_assoc_zstrl(tmp, "filename", path_type, ret, idx, 1);
	}

	if (!have_basename && ret.v) {
		efree(ret.v);
	}

	if (opt == PHP_PATHINFO_ALL) {
		RETURN_ZVAL(tmp, 0, 1);
	} else {
		zval **element;
		if (zend_hash_get_current_data(Z_ARRVAL_P(tmp), (void **) &element) == SUCCESS) {
			RETVAL_ZVAL(*element, 1, 0);
		} else {
			ZVAL_EMPTY_STRING(return_value);
		}
	}

	zval_ptr_dtor(&tmp);
}
/* }}} */

/* {{{ php_u_stristr
   Unicode version of case insensitve strstr */
PHPAPI UChar *php_u_stristr(UChar *s, UChar *t, int s_len, int t_len)
{
	int32_t i,j, last;
	UChar32 ch1, ch2;

	/* Have to do this by hand since lower-casing can change lengths
	   by changing codepoints, and an offset within the lower-case &
	   upper-case strings might be different codepoints.

	   Find an occurrence of the first codepoint of 't' in 's', and
	   starting from this point, match the rest of the codepoints of
	   't' with those in 's'. Comparisons are performed against
	   lower-case equivalents of the codepoints being matched.

	   'i' & 'j' are indices used for extracting codepoints 'ch1' &
	   'ch2'. 'last' is offset in 's' where the search for 't'
	   started, and indicates beginning of 't' in 's' for a successful
	   match.
	*/

	i = 0;
	while (i <= (s_len-t_len)) {
		last = i;
		U16_NEXT(s, i, s_len, ch1);
		j = 0;
		U16_NEXT(t, j, t_len, ch2);
		if (u_tolower(ch1) == u_tolower(ch2)) {
			while (j < t_len) {
				U16_NEXT(s, i, s_len, ch1);
				U16_NEXT(t, j, t_len, ch2);
				if (u_tolower(ch1) != u_tolower(ch2)) {
					/* U16_NEXT() incr 'i' beyond 'ch1', re-adjust to
					   restart compare
					*/
					U16_BACK_1(s, 0, i);
					break;
				}
			}
			if (u_tolower(ch1) == u_tolower(ch2)) {
				return s+last;
			}
		}
	}
	return NULL;
}
/* }}} */

/* {{{ php_stristr
   case insensitve strstr */
PHPAPI char *php_stristr(char *s, char *t, size_t s_len, size_t t_len)
{
	php_strtolower(s, s_len);
	php_strtolower(t, t_len);
	return php_memnstr(s, t, t_len, s + s_len);
}
/* }}} */

/* {{{ php_u_strspn
 */
PHPAPI int php_u_strspn(UChar *s1, UChar *s2, UChar *s1_end, UChar *s2_end)
{
	int32_t len1 = s1_end - s1;
	int32_t len2 = s2_end - s2;
	int32_t i;
	int codepts;
	UChar32 ch;

	for (i = 0, codepts = 0 ; i < len1 ; codepts++) {
		U16_NEXT(s1, i, len1, ch);
		if (u_memchr32(s2, ch, len2) == NULL) {
			break;
		}
	}
	return codepts;
}
/* }}} */

/* {{{ php_strspn
 */
PHPAPI size_t php_strspn(char *s1, char *s2, char *s1_end, char *s2_end)
{
	register const char *p = s1, *spanp;
	register char c = *p;

cont:
	for (spanp = s2; p != s1_end && spanp != s2_end;) {
		if (*spanp++ == c) {
			c = *(++p);
			goto cont;
		}
	}
	return (p - s1);
}
/* }}} */

/* {{{ php_u_strcspn
 */
PHPAPI int php_u_strcspn(UChar *s1, UChar *s2, UChar *s1_end, UChar *s2_end)
{
	int32_t len1 = s1_end - s1;
	int32_t len2 = s2_end - s2;
	int32_t i;
	int codepts;
	UChar32 ch;

	for (i = 0, codepts = 0 ; i < len1 ; codepts++) {
		U16_NEXT(s1, i, len1, ch);
		if (u_memchr32(s2, ch, len2)) {
			break;
		}
	}
	return codepts;
}
/* }}} */

/* {{{ php_strcspn
 */
PHPAPI size_t php_strcspn(char *s1, char *s2, char *s1_end, char *s2_end)
{
	register const char *p, *spanp;
	register char c = *s1;

	for (p = s1;;) {
		spanp = s2;
		do {
			if (*spanp == c || p == s1_end) {
				return p - s1;
			}
		} while (spanp++ < s2_end);
		c = *++p;
	}
	/* NOTREACHED */
}
/* }}} */

/* {{{ proto string stristr(string haystack, string needle[, bool part])
   Finds first occurrence of a string within another, case insensitive */
PHP_FUNCTION(stristr)
{
	zval *haystack, *needle;
	zend_bool part = 0;
	zend_uchar str_type;
	char needle_char[2];
	UChar u_needle_char[3];
	int needle_len;
	char *haystack_copy = NULL;
	zstr target;
	void *found = NULL;
	int found_offset;
	void *start, *end;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|b", &haystack, &needle, &part) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(haystack) != IS_UNICODE && Z_TYPE_P(haystack) != IS_STRING) {
		convert_to_text(haystack);
	}

	if (Z_TYPE_P(needle) == IS_UNICODE || Z_TYPE_P(needle) == IS_STRING) {
		if (!Z_UNILEN_P(needle)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty delimiter.");
			RETURN_FALSE;
		}
		if (Z_TYPE_P(haystack) != Z_TYPE_P(needle)) {
			str_type = zend_get_unified_string_type(2 TSRMLS_CC, Z_TYPE_P(haystack), Z_TYPE_P(needle));
			if (str_type == (zend_uchar)-1) {
				zend_error(E_WARNING, "Cannot mix binary and Unicode parameters");
				return;
			}
			convert_to_explicit_type(haystack, str_type);
			convert_to_explicit_type(needle, str_type);
		}
		target = Z_UNIVAL_P(needle);
		needle_len = Z_UNILEN_P(needle);
	} else {
		convert_to_long_ex(&needle);
		needle_len = 0;
		if (Z_TYPE_P(haystack) == IS_UNICODE) {
			if (Z_LVAL_P(needle) < 0 || Z_LVAL_P(needle) > 0x10FFFF) {
				php_error(E_WARNING, "Needle argument codepoint value out of range (0 - 0x10FFFF)");
				RETURN_FALSE;
			}
			if (U_IS_BMP(Z_LVAL_P(needle))) {
				u_needle_char[needle_len++] = (UChar)Z_LVAL_P(needle);
				u_needle_char[needle_len]   = 0;
			} else {
				u_needle_char[needle_len++] = (UChar)U16_LEAD(Z_LVAL_P(needle));
				u_needle_char[needle_len++] = (UChar)U16_TRAIL(Z_LVAL_P(needle));
				u_needle_char[needle_len]   = 0;
			}
			target.u = u_needle_char;
		} else {
			needle_char[needle_len++] = (char)Z_LVAL_P(needle);
			needle_char[needle_len] = 0;
			target.s = needle_char;
		}
	}

	if (needle_len > Z_UNILEN_P(haystack)) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(haystack) == IS_UNICODE) {
		found = php_u_stristr(Z_USTRVAL_P(haystack), target.u,
							  Z_USTRLEN_P(haystack), needle_len);
	} else {
		haystack_copy = estrndup(Z_STRVAL_P(haystack), Z_STRLEN_P(haystack));
		found = php_stristr(Z_STRVAL_P(haystack), target.s,
							Z_STRLEN_P(haystack), needle_len);
	}

	if (found) {
		if (Z_TYPE_P(haystack) == IS_UNICODE) {
			start = part ? Z_USTRVAL_P(haystack) : found;
			end = part ? found : (Z_USTRVAL_P(haystack) + Z_USTRLEN_P(haystack));
			RETVAL_UNICODEL((UChar *)start, (UChar *)end-(UChar *)start, 1);
		} else {
			found_offset = (char *)found - Z_STRVAL_P(haystack);
			start = part ? haystack_copy : haystack_copy + found_offset;
			end = part ? haystack_copy + found_offset : (haystack_copy + Z_STRLEN_P(haystack));
			RETVAL_STRINGL((char *)start, (char *)end-(char *)start, 1);
		}
	} else {
		RETVAL_FALSE;
	}

	if (Z_TYPE_P(haystack) != IS_UNICODE) {
		efree(haystack_copy);
	}
}
/* }}} */

/* {{{ proto string strstr(string haystack, string needle[, bool part]) U
   Finds first occurrence of a string within another */
PHP_FUNCTION(strstr)
{
	void *haystack;
	int haystack_len;
	zend_uchar haystack_type;
	zval **needle;
	void *found = NULL;
	char  needle_char[2];
	UChar u_needle_char[3];
	int n_len = 0;
	size_t found_offset;
	zend_bool part = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "tZ|b", &haystack, &haystack_len, &haystack_type, &needle, &part) == FAILURE) {
		return;
	}

	if (Z_TYPE_PP(needle) == IS_STRING || Z_TYPE_PP(needle) == IS_UNICODE) {
		if (!Z_STRLEN_PP(needle)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty delimiter.");
			RETURN_FALSE;
		}

		/* haystack type determines the needle type */
		if (haystack_type == IS_UNICODE) {
			convert_to_unicode_ex(needle);
			found = zend_u_memnstr((UChar*)haystack,
								   Z_USTRVAL_PP(needle),
								   Z_USTRLEN_PP(needle),
								   (UChar*)haystack + haystack_len);
		} else {
			convert_to_string_ex(needle);
			found = php_memnstr((char*)haystack,
								Z_STRVAL_PP(needle),
								Z_STRLEN_PP(needle),
								(char*)haystack + haystack_len);
		}
	} else {
		convert_to_long_ex(needle);
		if (haystack_type == IS_UNICODE) {
			if (Z_LVAL_PP(needle) < 0 || Z_LVAL_PP(needle) > 0x10FFFF) {
				php_error(E_WARNING, "Needle argument codepoint value out of range (0 - 0x10FFFF)");
				RETURN_FALSE;
			}
			/* supplementary codepoint values may require 2 UChar's */
			if (U_IS_BMP(Z_LVAL_PP(needle))) {
				u_needle_char[n_len++] = (UChar) Z_LVAL_PP(needle);
				u_needle_char[n_len]   = 0;
			} else {
				u_needle_char[n_len++] = (UChar) U16_LEAD(Z_LVAL_PP(needle));
				u_needle_char[n_len++] = (UChar) U16_TRAIL(Z_LVAL_PP(needle));
				u_needle_char[n_len]   = 0;
			}

			found = zend_u_memnstr((UChar*)haystack,
								   u_needle_char,
								   n_len,
								   (UChar*)haystack + haystack_len);
		} else {
			needle_char[0] = (char) Z_LVAL_PP(needle);
			needle_char[1] = 0;

			found = php_memnstr((char*)haystack,
								needle_char,
								1,
								(char*)haystack + haystack_len);
		}
	}

	if (found) {
		switch (haystack_type) {
			case IS_UNICODE:
				found_offset = (UChar*)found - (UChar*)haystack;
				if (part) {
					UChar *ret;
					ret = eumalloc(found_offset + 1);
					u_strncpy(ret, haystack, found_offset);
					ret[found_offset] = '\0';
					RETURN_UNICODEL(ret , found_offset, 0);
				} else {
					RETURN_UNICODEL(found, haystack_len - found_offset, 1);
				}
				break;

			case IS_STRING:
				found_offset = (char *)found - (char *)haystack;
				if (part) {
					char *ret;
					ret = emalloc(found_offset + 1);
					strncpy(ret, haystack, found_offset);
					ret[found_offset] = '\0';
					RETURN_STRINGL(ret , found_offset, 0);
				} else {
					RETURN_STRINGL(found, haystack_len - found_offset, 1);
				}
				break;
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string strchr(string haystack, string needle[, bool part]) U
   An alias for strstr */
/* }}} */

/* {{{ proto int strpos(string haystack, mixed needle [, int offset]) U
   Finds position of first occurrence of a string within another */
PHP_FUNCTION(strpos)
{
	void *haystack;
	int haystack_len;
	zend_uchar haystack_type;
	zval **needle;
	long   offset = 0;
	void *found = NULL;
	char  needle_char[2];
	UChar u_needle_char[3];
	int n_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "tZ|l", &haystack,
							  &haystack_len, &haystack_type, &needle, &offset) == FAILURE) {
		return;
	}

	/*
	 * Unicode note: it's okay to not convert offset to codepoint offset here.
	 * We'll just do a rough check that the offset does not exceed length in
	 * code units, and leave the rest to zend_u_memnstr().
	 */
	if (offset < 0 || offset > haystack_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset not contained in string.");
		RETURN_FALSE;
	}

	if (Z_TYPE_PP(needle) == IS_STRING || Z_TYPE_PP(needle) == IS_UNICODE) {
		if (!Z_STRLEN_PP(needle)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty delimiter.");
			RETURN_FALSE;
		}

		/* haystack type determines the needle type */
		if (haystack_type == IS_UNICODE) {
			int32_t cp_offset = 0;
			convert_to_unicode_ex(needle);
			/* locate the codepoint at the specified offset */
			U16_FWD_N((UChar*)haystack, cp_offset, haystack_len, offset);
			found = zend_u_memnstr((UChar*)haystack + cp_offset,
								   Z_USTRVAL_PP(needle),
								   Z_USTRLEN_PP(needle),
								   (UChar*)haystack + haystack_len);
		} else {
			convert_to_string_ex(needle);
			found = php_memnstr((char*)haystack + offset,
								Z_STRVAL_PP(needle),
								Z_STRLEN_PP(needle),
								(char*)haystack + haystack_len);
		}
	} else {
		convert_to_long_ex(needle);
		if (haystack_type == IS_UNICODE) {
			int32_t cp_offset = 0;
			if (Z_LVAL_PP(needle) < 0 || Z_LVAL_PP(needle) > 0x10FFFF) {
				php_error(E_WARNING, "Needle argument codepoint value out of range (0 - 0x10FFFF)");
				RETURN_FALSE;
			}
			n_len += zend_codepoint_to_uchar(Z_LVAL_PP(needle), u_needle_char);
			u_needle_char[n_len] = 0;

			/* locate the codepoint at the specified offset */
			U16_FWD_N((UChar*)haystack, cp_offset, haystack_len, offset);
			found = zend_u_memnstr((UChar*)haystack + cp_offset,
								   u_needle_char,
								   n_len,
								   (UChar*)haystack + haystack_len);
		} else {
			needle_char[0] = (char) Z_LVAL_PP(needle);
			needle_char[1] = 0;

			found = php_memnstr((char*)haystack + offset,
								needle_char,
								1,
								(char*)haystack + haystack_len);
		}
	}

	if (found) {
		if (haystack_type == IS_UNICODE) {
			/* simple subtraction will not suffice, since there may be
			   supplementary codepoints */
			RETURN_LONG(u_countChar32(haystack, ((char *)found - (char *)haystack)/sizeof(UChar)));
		} else {
			RETURN_LONG((char *)found - (char *)haystack);
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int stripos(string haystack, string needle [, int offset])
   Finds position of first occurrence of a string within another, case insensitive */
PHP_FUNCTION(stripos)
{
	zval *haystack, *needle;
	long offset = 0;
	int haystack_len, needle_len = 0;
	zend_uchar str_type;
	void *haystack_dup, *needle_dup = NULL;
	char needle_char[2];
	char c = 0;
	UChar u_needle_char[3];
	UChar32 ch = 0;
	void *found = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|l", &haystack, &needle, &offset) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(haystack) != IS_UNICODE && Z_TYPE_P(haystack) != IS_STRING) {
		convert_to_text(haystack);
	}
	if (offset < 0 || offset > Z_UNILEN_P(haystack)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset not contained in string.");
		RETURN_FALSE;
	}

	haystack_len = Z_UNILEN_P(haystack);

	if (haystack_len == 0) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(needle) == IS_UNICODE || Z_TYPE_P(needle) == IS_STRING) {
		if (!Z_UNILEN_P(needle) || Z_UNILEN_P(needle) > haystack_len) {
			RETURN_FALSE;
		}
		if (Z_TYPE_P(haystack) != Z_TYPE_P(needle)) {
			str_type = zend_get_unified_string_type(2 TSRMLS_CC, Z_TYPE_P(haystack), Z_TYPE_P(needle));
			if (str_type == (zend_uchar)-1) {
				zend_error(E_WARNING, "Cannot mix binary and Unicode parameters");
				return;
			}
			convert_to_explicit_type(haystack, str_type);
			convert_to_explicit_type(needle, str_type);
		}
		needle_len = Z_UNILEN_P(needle);
		if (Z_TYPE_P(haystack) == IS_UNICODE) {
			haystack_dup = php_u_strtolower(Z_USTRVAL_P(haystack), &haystack_len, UG(default_locale));
			needle_dup = php_u_strtolower(Z_USTRVAL_P(needle), &needle_len, UG(default_locale));
			found = zend_u_memnstr((UChar *)haystack_dup + offset,
								   (UChar *)needle_dup, needle_len,
								   (UChar *)haystack_dup + haystack_len);
		} else {
			haystack_dup = estrndup(Z_STRVAL_P(haystack), haystack_len);
			php_strtolower((char *)haystack_dup, haystack_len);
			needle_dup = estrndup(Z_STRVAL_P(needle), needle_len);
			php_strtolower((char *)needle_dup, Z_STRLEN_P(needle));
			found = php_memnstr((char *)haystack_dup + offset,
								(char *)needle_dup, needle_len,
								(char *)haystack_dup + haystack_len);
		}
	} else {
		switch (Z_TYPE_P(needle)) {
			case IS_LONG:
			case IS_BOOL:
				if (Z_TYPE_P(haystack) == IS_UNICODE) {
					ch = u_tolower((UChar32)Z_LVAL_P(needle));
				} else {
					c = tolower((char)Z_LVAL_P(needle));
				}
				break;
			case IS_DOUBLE:
				if (Z_TYPE_P(haystack) == IS_UNICODE) {
					ch = u_tolower((UChar32)Z_DVAL_P(needle));
				} else {
					c = tolower((char)Z_DVAL_P(needle));
				}
				break;
			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "needle is not a string or an integer.");
				RETURN_FALSE;
				break;

		}
		if (Z_TYPE_P(haystack) == IS_UNICODE) {
			if (U_IS_BMP(ch)) {
				u_needle_char[needle_len++] = ch;
				u_needle_char[needle_len]   = 0;
			} else {
				u_needle_char[needle_len++] = U16_LEAD(ch);
				u_needle_char[needle_len++] = U16_TRAIL(ch);
				u_needle_char[needle_len]   = 0;
			}
			haystack_dup = php_u_strtolower(Z_USTRVAL_P(haystack), &haystack_len, UG(default_locale));
			found = zend_u_memnstr((UChar *)haystack_dup + offset,
								   (UChar *)u_needle_char, needle_len,
								   (UChar *)haystack_dup + haystack_len);
		} else {
			needle_char[0] = c;
			needle_char[1] = '\0';
			haystack_dup = estrndup(Z_STRVAL_P(haystack), haystack_len);
			php_strtolower((char *)haystack_dup, haystack_len);
			found = php_memnstr((char *)haystack_dup + offset,
								(char *)needle_char,
								sizeof(needle_char) - 1,
								(char *)haystack_dup + haystack_len);
		}
	}

	efree(haystack_dup);
	if (needle_dup) {
		efree(needle_dup);
	}

	if (found) {
		if (Z_TYPE_P(haystack) == IS_UNICODE) {
			RETURN_LONG((UChar *)found - (UChar *)haystack_dup);
		} else {
			RETURN_LONG((char *)found - (char *)haystack_dup);
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int strrpos(string haystack, string needle [, int offset]) U
   Finds position of last occurrence of a string within another string */
PHP_FUNCTION(strrpos)
{
	zval *zhaystack, *zneedle;
	zstr haystack, needle;
	int haystack_len, needle_len = 0;
	zend_uchar str_type;
	long offset = 0;
	char *p, *e, ord_needle[2];
	UChar *pos, *u_p, *u_e, u_ord_needle[3];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|l",
							  &zhaystack, &zneedle, &offset) == FAILURE) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(zhaystack) != IS_UNICODE && Z_TYPE_P(zhaystack) != IS_STRING) {
		convert_to_text(zhaystack);
	}
	if (Z_TYPE_P(zneedle) == IS_UNICODE || Z_TYPE_P(zneedle) == IS_STRING) {
		if (Z_TYPE_P(zneedle) != Z_TYPE_P(zhaystack)) {
			str_type = zend_get_unified_string_type(2 TSRMLS_CC, Z_TYPE_P(zhaystack), Z_TYPE_P(zneedle));
			if (str_type == (zend_uchar)-1) {
				zend_error(E_WARNING, "Cannot mix binary and Unicode parameters");
				return;
			}
			convert_to_explicit_type(zhaystack, str_type);
			convert_to_explicit_type(zneedle, str_type);
		}
		needle = Z_UNIVAL_P(zneedle);
		needle_len = Z_UNILEN_P(zneedle);
	} else {
		if (Z_TYPE_P(zhaystack) == IS_UNICODE) {
			if (Z_LVAL_P(zneedle) < 0 || Z_LVAL_P(zneedle) > 0x10FFFF) {
				php_error(E_WARNING, "Needle argument codepoint value out of range (0 - 0x10FFFF)");
				RETURN_FALSE;
			}
			if (U_IS_BMP(Z_LVAL_P(zneedle))) {
				u_ord_needle[needle_len++] = (UChar)Z_LVAL_P(zneedle);
				u_ord_needle[needle_len]   = 0;
			} else {
				u_ord_needle[needle_len++] = (UChar)U16_LEAD(Z_LVAL_P(zneedle));
				u_ord_needle[needle_len++] = (UChar)U16_TRAIL(Z_LVAL_P(zneedle));
				u_ord_needle[needle_len]   = 0;
			}
			needle.u = u_ord_needle;
		} else {
			convert_to_long(zneedle);
			ord_needle[0] = (char)(Z_LVAL_P(zneedle) & 0xFF);
			ord_needle[1] = '\0';
			needle.s = ord_needle;
			needle_len = 1;
		}
	}
	haystack = Z_UNIVAL_P(zhaystack);
	haystack_len = Z_UNILEN_P(zhaystack);

	if ((haystack_len == 0) || (needle_len == 0)) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(zhaystack) == IS_UNICODE) {
		if (offset >= 0) {
			u_p = haystack.u + offset;
			u_e = haystack.u + haystack_len - needle_len;
		} else {
			u_p = haystack.u;
			if (-offset > haystack_len) {
				u_e = haystack.u - needle_len;
			} else if (needle_len > -offset) {
				u_e = haystack.u + haystack_len - needle_len;
			} else {
				u_e = haystack.u + haystack_len + offset;
			}
		}

		pos = u_strFindLast(u_p, u_e-u_p+needle_len, needle.u, needle_len);
		if (pos) {
			RETURN_LONG(pos - haystack.u);
		} else {
			RETURN_FALSE;
		}
	} else {
		if (offset >= 0) {
			p = haystack.s + offset;
			e = haystack.s + haystack_len - needle_len;
		} else {
			p = haystack.s;
			if (-offset > haystack_len) {
				e = haystack.s - needle_len;
			} else if (needle_len > -offset) {
				e = haystack.s + haystack_len - needle_len;
			} else {
				e = haystack.s + haystack_len + offset;
			}
		}

		if (needle_len == 1) {
			/* Single character search can shortcut memcmps */
			while (e >= p) {
				if (*e == *needle.s) {
					RETURN_LONG(e - p + (offset > 0 ? offset : 0));
				}
				e--;
			}
			RETURN_FALSE;
		}

		while (e >= p) {
			if (memcmp(e, needle.s, needle_len) == 0) {
				RETURN_LONG(e - p + (offset > 0 ? offset : 0));
			}
			e--;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int strripos(string haystack, string needle [, int offset])
   Finds position of last occurrence of a string within another string */
PHP_FUNCTION(strripos)
{
	zval *zneedle;
	char *needle, *haystack;
	int needle_len, haystack_len;
	long offset = 0;
	char *p, *e, ord_needle[2];
	char *needle_dup, *haystack_dup;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|l", &haystack, &haystack_len, &zneedle, &offset) == FAILURE) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(zneedle) == IS_STRING) {
		needle = Z_STRVAL_P(zneedle);
		needle_len = Z_STRLEN_P(zneedle);
	} else {
		convert_to_long(zneedle);
		ord_needle[0] = (char)(Z_LVAL_P(zneedle) & 0xFF);
		ord_needle[1] = '\0';
		needle = ord_needle;
		needle_len = 1;
	}

	if ((haystack_len == 0) || (needle_len == 0)) {
		RETURN_FALSE;
	}

	if (needle_len == 1) {
		/* Single character search can shortcut memcmps
		   Can also avoid tolower emallocs */
		if (offset >= 0) {
			p = haystack + offset;
			e = haystack + haystack_len - 1;
		} else {
			p = haystack;
			if (-offset > haystack_len) {
				e = haystack + haystack_len - 1;
			} else {
				e = haystack + haystack_len + offset;
			}
		}
		/* Borrow that ord_needle buffer to avoid repeatedly tolower()ing needle */
		*ord_needle = tolower(*needle);
		while (e >= p) {
			if (tolower(*e) == *ord_needle) {
				RETURN_LONG(e - p + (offset > 0 ? offset : 0));
			}
			e--;
		}
		RETURN_FALSE;
	}

	needle_dup = estrndup(needle, needle_len);
	php_strtolower(needle_dup, needle_len);
	haystack_dup = estrndup(haystack, haystack_len);
	php_strtolower(haystack_dup, haystack_len);

	if (offset >= 0) {
		p = haystack_dup + offset;
		e = haystack_dup + haystack_len - needle_len;
	} else {
		p = haystack_dup;
		if (-offset > haystack_len) {
			e = haystack_dup - needle_len;
		} else if (needle_len > -offset) {
			e = haystack_dup + haystack_len - needle_len;
		} else {
			e = haystack_dup + haystack_len + offset;
		}
	}

	while (e >= p) {
		if (memcmp(e, needle_dup, needle_len) == 0) {
			efree(haystack_dup);
			efree(needle_dup);
			RETURN_LONG(e - p + (offset > 0 ? offset : 0));
		}
		e--;
	}

	efree(haystack_dup);
	efree(needle_dup);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string strrchr(string haystack, string needle) U
   Finds the last occurrence of a character in a string within another */
PHP_FUNCTION(strrchr)
{
	zval *haystack, *needle;
	zend_uchar str_type;
	UChar32 ch;
	void *found = NULL;
	int found_offset;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z/z/", &haystack, &needle) == FAILURE) {
		return;
	}
	if (Z_TYPE_P(haystack) != IS_UNICODE || Z_TYPE_P(haystack) != IS_STRING) {
		convert_to_text(haystack);
	}

	if (Z_TYPE_P(needle) == IS_UNICODE || Z_TYPE_P(needle) == IS_STRING) {
		if (Z_TYPE_P(needle) != Z_TYPE_P(haystack)) {
			str_type = zend_get_unified_string_type(2 TSRMLS_CC, Z_TYPE_P(haystack), Z_TYPE_P(needle));
			convert_to_explicit_type(haystack, str_type);
			convert_to_explicit_type(needle, str_type);
		}
		if (Z_USTRLEN_P(needle) == 0) {
			RETURN_FALSE;
		}
		if (Z_TYPE_P(haystack) == IS_UNICODE) {
			ch = zend_get_codepoint_at(Z_USTRVAL_P(needle), Z_USTRLEN_P(needle), 0);
			found = u_memrchr32(Z_USTRVAL_P(haystack), ch, Z_USTRLEN_P(haystack));
		} else {
			found = strrchr(Z_STRVAL_P(haystack), *Z_STRVAL_P(needle));
		}
	} else {
		convert_to_long(needle);
		if (Z_TYPE_P(haystack) == IS_UNICODE) {
			UChar32 ch = (UChar32)Z_LVAL_P(needle);
			if (ch < 0 || ch > 0x10FFFF) {
				php_error(E_WARNING, "Needle argument codepoint value out of range (0 - 0x10FFFF)");
				RETURN_FALSE;
			}
			found = u_memrchr32(Z_USTRVAL_P(haystack), ch, Z_USTRLEN_P(haystack));
		} else {
			found = strrchr(Z_STRVAL_P(haystack), (char)Z_LVAL_P(needle));
		}
	}

	if (found) {
		if (Z_TYPE_P(haystack) == IS_UNICODE) {
			found_offset = (UChar *)found - Z_USTRVAL_P(haystack);
			RETURN_UNICODEL((UChar *)found, Z_USTRLEN_P(haystack) - found_offset, 1);
		} else {
			found_offset = (char *)found - Z_STRVAL_P(haystack);
			RETURN_STRINGL((char *)found, Z_STRLEN_P(haystack) - found_offset, 1);
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ php_chunk_split
 */
static char *php_chunk_split(char *src, int srclen, char *end, int endlen, int chunklen, int *destlen)
{
	char *dest;
	char *p, *q;
	int chunks; /* complete chunks! */
	int restlen;

	chunks = srclen / chunklen;
	restlen = srclen - chunks * chunklen; /* srclen % chunklen */

	dest = safe_emalloc((srclen + (chunks + 1) * endlen + 1), sizeof(char), 0);

	for (p = src, q = dest; p < (src + srclen - chunklen + 1); ) {
		memcpy(q, p, chunklen);
		q += chunklen;
		memcpy(q, end, endlen);
		q += endlen;
		p += chunklen;
	}

	if (restlen) {
		memcpy(q, p, restlen);
		q += restlen;
		memcpy(q, end, endlen);
		q += endlen;
	}

	*q = '\0';
	if (destlen) {
		*destlen = q - dest;
	}

	return(dest);
}
/* }}} */

/* {{{ proto string chunk_split(string str [, int chunklen [, string ending]])
   Returns split line */
PHP_FUNCTION(chunk_split)
{
	zval **p_str, **p_chunklen, **p_ending;
	char *result;
	char *end    = "\r\n";
	int endlen   = 2;
	int chunklen = 76;
	int result_len;
	int argc = ZEND_NUM_ARGS();

	if (argc < 1 || argc > 3 ||	zend_get_parameters_ex(argc, &p_str, &p_chunklen, &p_ending) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(p_str);

	if (argc > 1) {
		convert_to_long_ex(p_chunklen);
		chunklen = Z_LVAL_PP(p_chunklen);
	}

	if (argc > 2) {
		convert_to_string_ex(p_ending);
		end = Z_STRVAL_PP(p_ending);
		endlen = Z_STRLEN_PP(p_ending);
	}

	if (chunklen <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Chunk length should be greater than zero.");
		RETURN_FALSE;
	}

	if (chunklen > Z_STRLEN_PP(p_str)) {
		/* to maintain BC, we must return original string + ending */
		result_len = endlen + Z_STRLEN_PP(p_str);
		result = emalloc(result_len + 1);
		memcpy(result, Z_STRVAL_PP(p_str), Z_STRLEN_PP(p_str));
		memcpy(result + Z_STRLEN_PP(p_str), end, endlen);
		result[result_len] = '\0';
		RETURN_STRINGL(result, result_len, 0);
	}

	if (!Z_STRLEN_PP(p_str)) {
		RETURN_EMPTY_STRING();
	}

	result = php_chunk_split(Z_STRVAL_PP(p_str), Z_STRLEN_PP(p_str), end, endlen, chunklen, &result_len);

	if (result) {
		RETURN_STRINGL(result, result_len, 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string substr(string str, int start [, int length]) U
   Returns part of a string */
PHP_FUNCTION(substr)
{
	void *str;
	int str_len;
	int cp_len;
	zend_uchar str_type;
	long l = -1;
	long f;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "tl|l", &str, &str_len, &str_type, &f, &l) == FAILURE) {
		return;
	}

	if (str_type == IS_UNICODE) {
		cp_len = u_countChar32(str, str_len);
	} else {
		cp_len = str_len;
	}

	if (ZEND_NUM_ARGS() == 2) {
		l = cp_len;
	}

	/* if "from" position is negative, count start position from the end
	 * of the string
	 */
	if (f < 0) {
		f = cp_len + f;
		if (f < 0) {
			f = 0;
		}
	}

	/* if "length" position is negative, set it to the length
	 * needed to stop that many chars from the end of the string
	 */
	if (l < 0) {
		l = (cp_len - f) + l;
		if (l < 0) {
			l = 0;
		}
	}

	if (f >= cp_len) {
		RETURN_FALSE;
	}

	if (((unsigned) f + (unsigned) l) > cp_len) {
		l = cp_len - f;
	}

	if (str_type == IS_UNICODE) {
		int32_t start = 0, end = 0;
		U16_FWD_N((UChar*)str, end, str_len, f);
		start = end;
		U16_FWD_N((UChar*)str, end, str_len, l);
		RETURN_UNICODEL((UChar*)str + start, end-start, 1);
	} else {
		RETURN_STRINGL((char*)str + f, l, 1);
	}
}
/* }}} */

/* {{{ php_adjust_limits
 */
PHPAPI void php_adjust_limits(zval **str, int *f, int *l)
{
	int str_codepts;

	if (Z_TYPE_PP(str) == IS_UNICODE) {
		str_codepts = u_countChar32(Z_USTRVAL_PP(str), Z_USTRLEN_PP(str));
	} else {
		str_codepts = Z_STRLEN_PP(str);
	}

	/* If "from" position is negative, count start position from the end
	 * of the string */
	if (*f < 0) {
		*f = str_codepts + *f;
		if (*f < 0) {
			*f = 0;
		}
	} else if (*f > str_codepts) {
		*f = str_codepts;
	}
	/* If "length" position is negative, set it to the length
	 * needed to stop that many codepts/chars from the end of the string */
	if (*l < 0) {
		*l = str_codepts - *f + *l;
		if (*l < 0) {
			*l = 0;
		}
	}
	if (((unsigned)(*f) + (unsigned)(*l)) > str_codepts) {
		*l = str_codepts - *f;
	}
}
/* }}} */

/* {{{ php_do_substr_replace
 */
PHPAPI int php_do_substr_replace(void **result, zval **str, zval **repl, int f, int l TSRMLS_DC)
{
	void *buf;
	int32_t buf_len, idx;
	UChar ch;
	int repl_len;

	if (repl) {
		repl_len = Z_UNILEN_PP(repl);
	} else {
		repl_len = 0;
	}

	if (Z_TYPE_PP(str) == IS_UNICODE) {
		buf = eumalloc(Z_USTRLEN_PP(str) -l + repl_len + 1);

		/* buf_len is codept count here */
		buf_len = 0; idx = 0;
		while (f-- > 0) {
			U16_NEXT(Z_USTRVAL_PP(str), idx, Z_USTRLEN_PP(str), ch);
			buf_len += zend_codepoint_to_uchar(ch, (UChar *)buf + buf_len);
		}
		if (repl != NULL) {
			u_memcpy((UChar *)buf + buf_len, Z_USTRVAL_PP(repl), repl_len);
			buf_len += repl_len;
		}
		U16_FWD_N(Z_USTRVAL_PP(str), idx, Z_USTRLEN_PP(str), l);
		u_memcpy((UChar *)buf + buf_len, Z_USTRVAL_PP(str) + idx, Z_USTRLEN_PP(str) - idx);
		buf_len += (Z_USTRLEN_PP(str) - idx);

		*((UChar *)buf + buf_len) = 0;
		buf = eurealloc(buf, buf_len + 1);
	} else {
		/* buf_len is char count here */
		buf_len = Z_STRLEN_PP(str) - l + repl_len;
		buf = emalloc(buf_len + 1);

		memcpy(buf, Z_STRVAL_PP(str), f);
		if (repl != NULL) {
			memcpy((char *)buf + f, Z_STRVAL_PP(repl), repl_len);
		}
		memcpy((char *)buf + f + repl_len, Z_STRVAL_PP(str) + f + l, Z_STRLEN_PP(str) - f - l);

		*((char *)buf + buf_len) = '\0';
	}

	*result = buf;
	return buf_len;
}
/* }}} */

/* {{{ proto mixed substr_replace(mixed str, mixed repl, mixed start [, mixed length]) U
   Replaces part of a string with another string */
PHP_FUNCTION(substr_replace)
{
	zval **str;
	zval **from;
	zval **len = NULL;
	zval **repl;
	void *result;
	int result_len;
	int l = 0;
	int f;
	int argc = ZEND_NUM_ARGS();

	HashPosition pos_str, pos_from, pos_repl, pos_len;
	zval **tmp_str = NULL, **tmp_from = NULL, **tmp_repl = NULL, **tmp_len= NULL;
	zend_uchar str_type;

	if (argc < 3 || argc > 4 || zend_get_parameters_ex(argc, &str, &repl, &from, &len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(str) != IS_ARRAY && Z_TYPE_PP(str) != IS_UNICODE &&
	    Z_TYPE_PP(str) != IS_STRING) {
		convert_to_text_ex(str);
	}
	if (Z_TYPE_PP(repl) != IS_ARRAY && Z_TYPE_PP(repl) != IS_UNICODE &&
	    Z_TYPE_PP(repl) != IS_STRING) {
		convert_to_text_ex(repl);
	}
	if (Z_TYPE_PP(from) != IS_ARRAY) {
		convert_to_long_ex(from);
		f = Z_LVAL_PP(from);
	}
	if (argc > 3) {
		if (Z_TYPE_PP(len) != IS_ARRAY) {
			convert_to_long_ex(len);
			l = Z_LVAL_PP(len);
		}
	} else {
		if (Z_TYPE_PP(str) != IS_ARRAY) {
			l = Z_UNILEN_PP(str);
		}
	}

	if (Z_TYPE_PP(str) != IS_ARRAY) {
		if ( (argc == 3 && Z_TYPE_PP(from) == IS_ARRAY) ||
			 (argc == 4 && Z_TYPE_PP(from) != Z_TYPE_PP(len)) ) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "'from' and 'len' should be of same type - numerical or array ");
			RETURN_ZVAL(*str, 1, 0);
		}
		if (argc == 4 && Z_TYPE_PP(from) == IS_ARRAY) {
			if (zend_hash_num_elements(Z_ARRVAL_PP(from)) != zend_hash_num_elements(Z_ARRVAL_PP(len))) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "'from' and 'len' should have the same number of elements");
				RETURN_ZVAL(*str, 1, 0);
			}
		}
	}


	if (Z_TYPE_PP(str) != IS_ARRAY) {
		if (Z_TYPE_PP(from) != IS_ARRAY ) {
			if (Z_TYPE_PP(repl) == IS_ARRAY) {
				zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(repl), &pos_repl);
				if (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_PP(repl), (void **) &tmp_repl, &pos_repl)) {
					if (Z_TYPE_PP(repl) != IS_UNICODE && Z_TYPE_PP(repl) != IS_STRING) {
						convert_to_text_ex(tmp_repl);
					}
				} else {
					tmp_repl = NULL;
				}
			} else {
				tmp_repl = repl;
			}

			if (tmp_repl && Z_TYPE_PP(str) != Z_TYPE_PP(tmp_repl)) {
				str_type = zend_get_unified_string_type(2 TSRMLS_CC, Z_TYPE_PP(str), Z_TYPE_PP(tmp_repl));
				if (str_type == (zend_uchar)-1) {
					zend_error(E_WARNING, "Cannot mix binary and Unicode types");
					return;
				}
				convert_to_explicit_type_ex(str, str_type);
				convert_to_explicit_type_ex(tmp_repl, str_type);
			}
			php_adjust_limits(str, &f, &l);
			result_len = php_do_substr_replace(&result, str, tmp_repl, f, l TSRMLS_CC);

			if (Z_TYPE_PP(str) == IS_UNICODE) {
				RETURN_UNICODEL((UChar *)result, result_len, 0);
			} else {
				RETURN_STRINGL((char *)result, result_len, 0);
			}
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Functionality of 'from' and 'len' as arrays is not implemented.");
			RETURN_ZVAL(*str, 1, 0);
		}
	} else { /* str is array of strings */
		array_init(return_value);

		if (Z_TYPE_PP(from) == IS_ARRAY) {
			zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(from), &pos_from);
		}
		if (argc > 3 && Z_TYPE_PP(len) == IS_ARRAY) {
			zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(len), &pos_len);
		}
		if (Z_TYPE_PP(repl) == IS_ARRAY) {
			zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(repl), &pos_repl);
		}

		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(str), &pos_str);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_PP(str), (void **) &tmp_str, &pos_str) == SUCCESS) {
			if (Z_TYPE_PP(tmp_str) != IS_UNICODE && Z_TYPE_PP(tmp_str) != IS_STRING && Z_TYPE_PP(tmp_str) != IS_STRING) {
				convert_to_text_ex(tmp_str);
			}

			if (Z_TYPE_PP(from) == IS_ARRAY) {
				if (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_PP(from), (void **) &tmp_from, &pos_from)) {
					convert_to_long_ex(tmp_from);
					f = Z_LVAL_PP(tmp_from);
					zend_hash_move_forward_ex(Z_ARRVAL_PP(from), &pos_from);
				} else {
					f = 0;
				}
			}

			if (argc > 3 && (Z_TYPE_PP(len) == IS_ARRAY)) {
				if (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_PP(len), (void **) &tmp_len, &pos_len)) {
					convert_to_long_ex(tmp_len);
					l = Z_LVAL_PP(tmp_len);
					zend_hash_move_forward_ex(Z_ARRVAL_PP(len), &pos_len);
				} else {
					l = Z_UNILEN_PP(tmp_str);
				}
			} else if (argc > 3) {
				/* 'l' parsed & set at top of funcn */
				l = Z_LVAL_PP(len);
			} else {
				l = Z_UNILEN_PP(tmp_str);
			}

			if (Z_TYPE_PP(repl) == IS_ARRAY) {
				if (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_PP(repl), (void **) &tmp_repl, &pos_repl)) {
					if (Z_TYPE_PP(repl) != IS_UNICODE && Z_TYPE_PP(repl) != IS_STRING && Z_TYPE_PP(repl) != IS_STRING) {
						convert_to_text_ex(tmp_repl);
					}
					zend_hash_move_forward_ex(Z_ARRVAL_PP(repl), &pos_repl);
				} else {
					tmp_repl = NULL;
				}
			} else {
				tmp_repl = repl;
			}

			if (tmp_repl && Z_TYPE_PP(tmp_str) != Z_TYPE_PP(tmp_repl)) {
				str_type = zend_get_unified_string_type(2 TSRMLS_CC, Z_TYPE_PP(tmp_str), Z_TYPE_PP(tmp_repl));
				if (str_type == (zend_uchar)-1) {
					zend_error(E_WARNING, "Cannot mix binary and Unicode types");
					return;
				}
				convert_to_explicit_type_ex(tmp_str, str_type);
				convert_to_explicit_type_ex(tmp_repl, str_type);
			}
			php_adjust_limits(tmp_str, &f, &l);
			result_len = php_do_substr_replace(&result, tmp_str, tmp_repl, f, l TSRMLS_CC);

			if (Z_TYPE_PP(tmp_str) == IS_UNICODE) {
				add_next_index_unicodel(return_value, (UChar *)result, result_len, 0);
			} else {
				add_next_index_stringl(return_value, (char *)result, result_len, 0);
			}

			zend_hash_move_forward_ex(Z_ARRVAL_PP(str), &pos_str);
		} /*while*/
	} /* if */
}
/* }}} */

/* {{{ proto string quotemeta(string str)
   Quotes meta characters */
PHP_FUNCTION(quotemeta)
{
	zval **arg;
	char *str, *old;
	char *old_end;
	char *p, *q;
	char c;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg);

	old = Z_STRVAL_PP(arg);
	old_end = Z_STRVAL_PP(arg) + Z_STRLEN_PP(arg);

	if (old == old_end) {
		RETURN_FALSE;
	}

	str = safe_emalloc(2, Z_STRLEN_PP(arg), 1);

	for (p = old, q = str; p != old_end; p++) {
		c = *p;
		switch (c) {
			case '.':
			case '\\':
			case '+':
			case '*':
			case '?':
			case '[':
			case '^':
			case ']':
			case '$':
			case '(':
			case ')':
				*q++ = '\\';
				/* break is missing _intentionally_ */
			default:
				*q++ = c;
		}
	}
	*q = 0;

	RETURN_STRINGL(erealloc(str, q - str + 1), q - str, 0);
}
/* }}} */

/* {{{ proto int ord(string character) U
   Returns the codepoint value of a character */
PHP_FUNCTION(ord)
{
	zval **str;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_text_ex(str);
	if (Z_TYPE_PP(str) == IS_UNICODE) {
		RETURN_LONG(zend_get_codepoint_at(Z_USTRVAL_PP(str), Z_USTRLEN_PP(str), 0));
	} else {
		RETURN_LONG((unsigned char) Z_STRVAL_PP(str)[0]);
	}
}
/* }}} */

/* {{{ proto string chr(int codepoint) U
   Converts a codepoint number to a character */
PHP_FUNCTION(chr)
{
	zval **num;
	char temp[2];

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(num);

	if (UG(unicode)) {
		UChar buf[2];
		int buf_len;

		if (Z_LVAL_PP(num) > UCHAR_MAX_VALUE) {
			php_error(E_WARNING, "Codepoint value cannot be greater than %X", UCHAR_MAX_VALUE);
			return;
		}
		buf_len = zend_codepoint_to_uchar((uint32_t)Z_LVAL_PP(num), buf);
		RETURN_UNICODEL(buf, buf_len, 1);
	} else {
		temp[0] = (char) Z_LVAL_PP(num);
		temp[1] = 0;

		RETVAL_STRINGL(temp, 1, 1);
	}
}
/* }}} */

/* {{{ php_u_ucfirst()
   Makes an Unicode string's first character uppercase */
static void php_u_ucfirst(zval *ustr, zval *return_value TSRMLS_DC)
{
	UChar tmp[3] = { 0, 0, 0 }; /* UChar32 will be converted to upto 2 UChar units */
	int tmp_len = 0;
	int pos = 0;
	UErrorCode status = U_ZERO_ERROR;

	U16_FWD_1(Z_USTRVAL_P(ustr), pos, Z_USTRLEN_P(ustr));
	tmp_len = u_strToUpper(tmp, sizeof(tmp)/sizeof(UChar), Z_USTRVAL_P(ustr), pos, UG(default_locale), &status);

	Z_USTRVAL_P(return_value) = eumalloc(tmp_len+Z_USTRLEN_P(ustr)-pos+1);
	Z_USTRVAL_P(return_value)[0] = tmp[0];
	if (tmp_len > 1) {
		Z_USTRVAL_P(return_value)[1] = tmp[1];
	}
	u_memcpy(Z_USTRVAL_P(return_value)+tmp_len, Z_USTRVAL_P(ustr)+pos, Z_USTRLEN_P(ustr)-pos+1);
	Z_USTRLEN_P(return_value) = tmp_len+Z_USTRLEN_P(ustr)-pos;
}
/* }}} */

/* {{{ proto string ucfirst(string str) U
   Makes a string's first character uppercase */
PHP_FUNCTION(ucfirst)
{
	zval **str;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(str) != IS_UNICODE && Z_TYPE_PP(str) != IS_STRING) {
		convert_to_text_ex(str);
	}

	if (Z_TYPE_PP(str) == IS_UNICODE && !Z_USTRLEN_PP(str)) {
		RETURN_EMPTY_UNICODE();
	} else if (!Z_STRLEN_PP(str)) {
		RETURN_EMPTY_STRING();
	}

	if (Z_TYPE_PP(str) == IS_UNICODE) {
		Z_TYPE_P(return_value) = IS_UNICODE;
		php_u_ucfirst(*str, return_value TSRMLS_CC);
	} else {
		ZVAL_STRINGL(return_value, Z_STRVAL_PP(str), Z_STRLEN_PP(str), 1);
		*Z_STRVAL_P(return_value) = toupper((unsigned char) *Z_STRVAL_P(return_value));
	}
}
/* }}} */

/* {{{ php_ucwords()
   Uppercase the first character of every word in a native string */
static void php_ucwords(zval *str)
{
	register char *r, *r_end;

	r = Z_STRVAL_P(str);
	*r = toupper((unsigned char) *r);
	for (r_end = r + Z_STRLEN_P(str) - 1; r < r_end; ) {
		if (isspace((int) *(unsigned char *)r++)) {
			*r = toupper((unsigned char) *r);
		}
	}
}
/* }}} */

/* {{{ php_u_ucwords() U
   Uppercase the first character of every word in an Unicode string */
static void php_u_ucwords(zval *ustr, zval *retval TSRMLS_DC)
{
	UChar32 cp = 0;
	UChar *tmp;
	int retval_len;
	int pos = 0, last_pos = 0;
	int tmp_len = 0;
	zend_bool last_was_space = TRUE;
	UErrorCode status = U_ZERO_ERROR;

	/*
	 * We can calculate maximum resulting length precisely considering that not
	 * more than half of the codepoints in the string can follow a whitespace
	 * and that maximum expansion is 2 UChar's.
	 */
	retval_len = ((3 * Z_USTRLEN_P(ustr)) >> 1) + 2;
	tmp = eumalloc(retval_len);

	while (pos < Z_USTRLEN_P(ustr)) {

		U16_NEXT(Z_USTRVAL_P(ustr), pos, Z_USTRLEN_P(ustr), cp);

		if (u_isWhitespace(cp) == TRUE) {
			tmp_len += zend_codepoint_to_uchar(cp, tmp + tmp_len);
			last_was_space = TRUE;
		} else {
			if (last_was_space) {
				tmp_len += u_strToUpper(tmp + tmp_len, retval_len - tmp_len, Z_USTRVAL_P(ustr) + last_pos, 1, UG(default_locale), &status);
				last_was_space = FALSE;
			} else {
				tmp_len += zend_codepoint_to_uchar(cp, tmp + tmp_len);
			}
		}

		last_pos = pos;
	}
	tmp[tmp_len] = 0;

	/*
	 * Try to avoid another alloc if the difference between allocated size and
	 * real length is "small".
	 */
	if (retval_len - tmp_len > 256) {
		ZVAL_UNICODEL(retval, tmp, tmp_len, 1);
		efree(tmp);
	} else {
		ZVAL_UNICODEL(retval, tmp, tmp_len, 0);
	}
}
/* }}} */

/* {{{ proto string ucwords(string str)
   Uppercase the first character of every word in a string */
PHP_FUNCTION(ucwords)
{
	zval **str;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(str) != IS_UNICODE && Z_TYPE_PP(str) != IS_STRING) {
		convert_to_text_ex(str);
	}

	if (Z_TYPE_PP(str) == IS_UNICODE && !Z_USTRLEN_PP(str)) {
		RETURN_EMPTY_UNICODE();
	} else if (!Z_STRLEN_PP(str)) {
		RETURN_EMPTY_STRING();
	}

	if (Z_TYPE_PP(str) == IS_UNICODE) {
		Z_TYPE_P(return_value) = IS_UNICODE;
		php_u_ucwords(*str, return_value TSRMLS_CC);
	} else {
		ZVAL_STRINGL(return_value, Z_STRVAL_PP(str), Z_STRLEN_PP(str), 1);
		php_ucwords(return_value);
	}
}
/* }}} */

/* {{{ php_strtr
 */
PHPAPI char *php_strtr(char *str, int len, char *str_from, char *str_to, int trlen)
{
	int i;
	unsigned char xlat[256];

	if ((trlen < 1) || (len < 1)) {
		return str;
	}

	for (i = 0; i < 256; xlat[i] = i, i++);

	for (i = 0; i < trlen; i++) {
		xlat[(unsigned char) str_from[i]] = str_to[i];
	}

	for (i = 0; i < len; i++) {
		str[i] = xlat[(unsigned char) str[i]];
	}

	return str;
}
/* }}} */

/* {{{ php_u_strtr
 */
PHPAPI UChar *php_u_strtr(UChar *str, int len, UChar *str_from, int str_from_len, UChar *str_to, int str_to_len, int trlen, int *outlen TSRMLS_DC)
{
	int i;
	int can_optimize = 1;

	if ((trlen < 1) || (len < 1)) {
		return str;
	}

	/* First loop to see if we can use the optimized version */
	for (i = 0; i < trlen; i++)	{
		if (str_from[i] > 255 || str_to[i] > 255) {
			can_optimize = 0;
			break;
		}
	}
	if (can_optimize) {
		for (i = trlen; i < str_from_len; i++) {
			if (str_from[i] > 255) {
				can_optimize = 0;
				break;
			}
		}
	}
	if (can_optimize) {
		for (i = trlen; i < str_to_len; i++) {
			if (str_from[i] > 255) {
				can_optimize = 0;
				break;
			}
		}
	}

	if (can_optimize) {
		UChar xlat[256];
		UChar *tmp_str = eustrndup(str, len);

		for (i = 0; i < 256; xlat[i] = i, i++);

		for (i = 0; i < trlen; i++) {
			xlat[str_from[i]] = str_to[i];
		}

		for (i = 0; i < len; i++) {
			tmp_str[i] = xlat[str[i]];
		}

		*outlen = len;
		return tmp_str;
	} else {
		/* We use the character break iterator here to assemble an mapping
		 * array in such a way that we can reuse the code in php_u_strtr_array
		 * to do the replacements in order to avoid duplicating code. */
		HashTable *tmp_hash;
		int minlen = 128*1024, maxlen;
		zval *tmp;

		tmp_hash = emalloc(sizeof(HashTable));
		zend_hash_init(tmp_hash, 0, NULL, NULL, 0);

		/* Loop over the two strings and prepare the hash entries */
		MAKE_STD_ZVAL(tmp);
		ZVAL_UNICODEL(tmp, "X", 1, 1);
		minlen = maxlen = 1;
		zend_u_hash_add(tmp_hash, IS_UNICODE, ZSTR("a"), 2, &tmp, sizeof(zval *), NULL);

		/* Run the replacement */
		str = php_u_strtr_array(str, len, tmp_hash, minlen, maxlen, outlen TSRMLS_CC);
		zend_hash_destroy(tmp_hash);
		efree(tmp_hash);

		return str;
	}
}
/* }}} */

static HashTable* php_u_strtr_array_prepare_hashtable(HashTable *hash, int *minlen_out, int *maxlen_out TSRMLS_DC)
{
	HashTable *tmp_hash = emalloc(sizeof(HashTable));
	HashPosition hpos;
	zval **entry;
	zstr   string_key;
	uint   string_key_len;
	int minlen = 128*1024;
	ulong num_key;
	int maxlen = 0, len;
	zval   ctmp;

	zend_hash_init(tmp_hash, 0, NULL, NULL, 0);
	zend_hash_internal_pointer_reset_ex(hash, &hpos);
	while (zend_hash_get_current_data_ex(hash, (void **)&entry, &hpos) == SUCCESS) {
		switch (zend_hash_get_current_key_ex(hash, &string_key, &string_key_len, &num_key, 0, &hpos)) {
			case HASH_KEY_IS_UNICODE:
				len = string_key_len-1;
				if (len < 1) {
					zend_hash_destroy(tmp_hash);
					return NULL;
				}
				zend_u_hash_add(tmp_hash, IS_UNICODE, string_key, string_key_len, entry, sizeof(zval*), NULL);
				if (len > maxlen) {
					maxlen = len;
				}
				if (len < minlen) {
					minlen = len;
				}
				break;

			case HASH_KEY_IS_LONG:
				Z_TYPE(ctmp) = IS_LONG;
				Z_LVAL(ctmp) = num_key;

				convert_to_unicode(&ctmp);
				len = Z_USTRLEN(ctmp);
				zend_u_hash_add(tmp_hash, IS_UNICODE, Z_UNIVAL(ctmp), len+1, entry, sizeof(zval*), NULL);
				zval_dtor(&ctmp);

				if (len > maxlen) {
					maxlen = len;
				}
				if (len < minlen) {
					minlen = len;
				}
				break;
		}
		zend_hash_move_forward_ex(hash, &hpos);
	}
	*minlen_out = minlen;
	*maxlen_out = maxlen;
	return tmp_hash;
}

/* {{{ php_u_strtr_array
 */
static UChar* php_u_strtr_array(UChar *str, int slen, HashTable *hash, int minlen, int maxlen, int *outlen TSRMLS_DC)
{
	zval **trans;
	UChar *key;
	int pos, found, len;
	smart_str result = {0};

	key = eumalloc(maxlen+1);
	pos = 0;

	while (pos < slen) {
		if ((pos + maxlen) > slen) {
			maxlen = slen - pos;
		}

		found = 0;
		memcpy(key, str+pos, UBYTES(maxlen));

		for (len = maxlen; len >= minlen; len--) {
			key[len] = 0;

			if (zend_u_hash_find(hash, IS_UNICODE, ZSTR(key), len+1, (void**)&trans) == SUCCESS) {
				UChar *tval;
				int tlen;
				zval tmp;

				if (Z_TYPE_PP(trans) != IS_UNICODE) {
					tmp = **trans;
					zval_copy_ctor(&tmp);
					convert_to_unicode(&tmp);
					tval = Z_USTRVAL(tmp);
					tlen = Z_USTRLEN(tmp);
				} else {
					tval = Z_USTRVAL_PP(trans);
					tlen = Z_USTRLEN_PP(trans);
				}

				smart_str_appendl(&result, tval, UBYTES(tlen));
				pos += len;
				found = 1;

				if (Z_TYPE_PP(trans) != IS_UNICODE) {
					zval_dtor(&tmp);
				}
				break;
			}
		}

		if (! found) {
			smart_str_append2c(&result, str[pos]);
			pos++;
		}
	}

	efree(key);
	smart_str_0(&result);
	*outlen = result.len >> 1;
	return (UChar*) result.c;
}
/* }}} */

/* {{{ php_strtr_array
 */
static void php_strtr_array(zval *return_value, char *str, int slen, HashTable *hash)
{
	zval **entry;
	zstr   string_key;
	uint   string_key_len;
	zval **trans;
	zval   ctmp;
	ulong num_key;
	int minlen = 128*1024;
	int maxlen = 0, pos, len, found;
	char *key;
	HashPosition hpos;
	smart_str result = {0};
	HashTable tmp_hash;

	zend_hash_init(&tmp_hash, 0, NULL, NULL, 0);
	zend_hash_internal_pointer_reset_ex(hash, &hpos);
	while (zend_hash_get_current_data_ex(hash, (void **)&entry, &hpos) == SUCCESS) {
		switch (zend_hash_get_current_key_ex(hash, &string_key, &string_key_len, &num_key, 0, &hpos)) {
			case HASH_KEY_IS_STRING:
				len = string_key_len-1;
				if (len < 1) {
					zend_hash_destroy(&tmp_hash);
					RETURN_FALSE;
				}
				zend_u_hash_add(&tmp_hash, IS_STRING, string_key, string_key_len, entry, sizeof(zval*), NULL);
				if (len > maxlen) {
					maxlen = len;
				}
				if (len < minlen) {
					minlen = len;
				}
				break;

			case HASH_KEY_IS_LONG:
				Z_TYPE(ctmp) = IS_LONG;
				Z_LVAL(ctmp) = num_key;

				convert_to_string(&ctmp);
				len = Z_STRLEN(ctmp);
				zend_hash_add(&tmp_hash, Z_STRVAL(ctmp), len+1, entry, sizeof(zval*), NULL);
				zval_dtor(&ctmp);

				if (len > maxlen) {
					maxlen = len;
				}
				if (len < minlen) {
					minlen = len;
				}
				break;
		}
		zend_hash_move_forward_ex(hash, &hpos);
	}

	key = emalloc(maxlen+1);
	pos = 0;

	while (pos < slen) {
		if ((pos + maxlen) > slen) {
			maxlen = slen - pos;
		}

		found = 0;
		memcpy(key, str+pos, maxlen);

		for (len = maxlen; len >= minlen; len--) {
			key[len] = 0;

			if (zend_hash_find(&tmp_hash, key, len+1, (void**)&trans) == SUCCESS) {
				char *tval;
				int tlen;
				zval tmp;

				if (Z_TYPE_PP(trans) != IS_STRING) {
					tmp = **trans;
					zval_copy_ctor(&tmp);
					convert_to_string(&tmp);
					tval = Z_STRVAL(tmp);
					tlen = Z_STRLEN(tmp);
				} else {
					tval = Z_STRVAL_PP(trans);
					tlen = Z_STRLEN_PP(trans);
				}

				smart_str_appendl(&result, tval, tlen);
				pos += len;
				found = 1;

				if (Z_TYPE_PP(trans) != IS_STRING) {
					zval_dtor(&tmp);
				}
				break;
			}
		}

		if (! found) {
			smart_str_appendc(&result, str[pos++]);
		}
	}

	efree(key);
	zend_hash_destroy(&tmp_hash);
	smart_str_0(&result);
	RETVAL_STRINGL(result.c, result.len, 0);
}
/* }}} */

/* {{{ proto string strtr(string str, string from[, string to])
   Translates characters in str using given translation tables */
PHP_FUNCTION(strtr)
{
	zval **str, **from, **to;
	int ac = ZEND_NUM_ARGS();

	if (ac < 2 || ac > 3 || zend_get_parameters_ex(ac, &str, &from, &to) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (ac == 2 && Z_TYPE_PP(from) != IS_ARRAY) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The second argument is not an array.");
		RETURN_FALSE;
	}

	if (Z_TYPE_PP(str) != IS_UNICODE && Z_TYPE_PP(str) != IS_STRING) {
		convert_to_text_ex(str);
	}

	/* shortcut for empty string */
	if (Z_TYPE_PP(str) == IS_UNICODE && !Z_USTRLEN_PP(str)) {
		RETURN_EMPTY_UNICODE();
	} else if (!Z_STRLEN_PP(str)) {
		RETURN_EMPTY_STRING();
	}

	if (Z_TYPE_PP(str) == IS_UNICODE) {
		int outlen;
		UChar *outstr;

		if (ac == 2) {
			int minlen, maxlen;
			HashTable *hash;

			hash = php_u_strtr_array_prepare_hashtable(HASH_OF(*from), &minlen, &maxlen TSRMLS_CC);
			outstr = php_u_strtr_array(Z_USTRVAL_PP(str), Z_USTRLEN_PP(str), hash, minlen, maxlen, &outlen TSRMLS_CC);
			zend_hash_destroy(hash);
			efree(hash);
			RETVAL_UNICODEL(outstr, outlen, 0);
			Z_TYPE_P(return_value) = IS_UNICODE;
		} else {
			convert_to_unicode_ex(from);
			convert_to_unicode_ex(to);

			outstr = php_u_strtr(Z_USTRVAL_PP(str),
					  Z_USTRLEN_PP(str),
					  Z_USTRVAL_PP(from),
					  Z_USTRLEN_PP(from),
					  Z_USTRVAL_PP(to),
					  Z_USTRLEN_PP(to),
					  MIN(Z_USTRLEN_PP(from), Z_USTRLEN_PP(to)),
					  &outlen TSRMLS_CC);
			ZVAL_UNICODEL(return_value, outstr, outlen, 0);

			Z_TYPE_P(return_value) = IS_UNICODE;
		}
	} else {
		if (ac == 2) {
			php_strtr_array(return_value, Z_STRVAL_PP(str), Z_STRLEN_PP(str), HASH_OF(*from));
		} else {
			convert_to_string_ex(from);
			convert_to_string_ex(to);

			ZVAL_STRINGL(return_value, Z_STRVAL_PP(str), Z_STRLEN_PP(str), 1);

			php_strtr(Z_STRVAL_P(return_value),
					  Z_STRLEN_P(return_value),
					  Z_STRVAL_PP(from),
					  Z_STRVAL_PP(to),
					  MIN(Z_STRLEN_PP(from),
					  Z_STRLEN_PP(to)));
		}
	}
}
/* }}} */

/* {{{ proto string strrev(string str) U
   Reverse a string */
PHP_FUNCTION(strrev)
{
	zstr str;
	int str_len;
	zend_uchar str_type;
	char *s, *e, *n = NULL, *p;
	int32_t i, x1, x2;
	UChar32 ch;
	UChar *u_s, *u_n = NULL, *u_p;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &str, &str_len, &str_type) == FAILURE) {
		return;
	}

	if (str_type == IS_UNICODE) {
		u_n = eumalloc(str_len+1);
		u_p = u_n;
		u_s = str.u;

		i = str_len;
		while (i > 0) {
			U16_PREV(u_s, 0, i, ch);
			if (u_getCombiningClass(ch) == 0) {
				u_p += zend_codepoint_to_uchar(ch, u_p);
			} else {
				x2 = i;
				do {
					U16_PREV(u_s, 0, i, ch);
				} while (u_getCombiningClass(ch) != 0);
				x1 = i;
				while (x1 <= x2) {
					U16_NEXT(u_s, x1, str_len, ch);
					u_p += zend_codepoint_to_uchar(ch, u_p);
				}
			}
		}
		*u_p = 0;
	} else {
		n = emalloc(str_len+1);
		p = n;
		s = str.s;
		e = s + str_len;

		while (--e >= s) {
			*(p++) = *e;
		}
		*p = '\0';
	}

	if (str_type == IS_UNICODE) {
		RETVAL_UNICODEL(u_n, str_len, 0);
	} else {
		RETVAL_STRINGL(n, str_len, 0);
	}
}
/* }}} */

/* {{{ php_u_similar_str
 */
static void php_u_similar_str(const UChar *txt1, int len1,
							  const UChar *txt2, int len2,
							  int *pos1, int *end1,
							  int *pos2, int *end2, int *max)
{
	int32_t i1, i2, j1, j2, l;
	UChar32 ch1, ch2;

	*max = 0;
	for (i1 = 0 ; i1 < len1 ; ) {
		for (i2 = 0 ; i2 < len2 ; ) {
			l = 0 ; j1 = 0 ; j2 = 0;
			while ((i1+j1 < len1) && (i2+j2 < len2)) {
				U16_NEXT(txt1+i1, j1, len1-i1, ch1);
				U16_NEXT(txt2+i2, j2, len2-i2, ch2);
				if (ch1 != ch2) {
					U16_BACK_1(txt1+i1, 0, j1);
					U16_BACK_1(txt2+i2, 0, j2);
					break;
				}
				l++;
			}
			if (l > *max) {
				*max = l;
				*pos1 = i1; *end1 = j1;
				*pos2 = i2; *end2 = j2;
			}
			U16_FWD_1(txt2, i2, len2);
		}
		U16_FWD_1(txt1, i1, len1);
	}
}
/* }}} */

/* {{{ php_similar_str
 */
static void php_similar_str(const char *txt1, int len1, const char *txt2, int len2, int *pos1, int *pos2, int *max)
{
	char *p, *q;
	char *end1 = (char *) txt1 + len1;
	char *end2 = (char *) txt2 + len2;
	int l;

	*max = 0;
	for (p = (char *) txt1; p < end1; p++) {
		for (q = (char *) txt2; q < end2; q++) {
			for (l = 0; (p + l < end1) && (q + l < end2) && (p[l] == q[l]); l++);
			if (l > *max) {
				*max = l;
				*pos1 = p - txt1;
				*pos2 = q - txt2;
			}
		}
	}
}
/* }}} */

/* {{{ php_similar_char
 */
static int php_similar_char(const char *txt1, int len1, const char *txt2, int len2)
{
	int sum;
	int pos1, pos2, max;

	php_similar_str(txt1, len1, txt2, len2, &pos1, &pos2, &max);
	if ((sum = max)) {
		if (pos1 && pos2) {
			sum += php_similar_char(txt1, pos1,
									txt2, pos2);
		}
		if ((pos1 + max < len1) && (pos2 + max < len2)) {
			sum += php_similar_char(txt1 + pos1 + max, len1 - pos1 - max,
									txt2 + pos2 + max, len2 - pos2 - max);
		}
	}

	return sum;
}
/* }}} */

/* {{{ php_u_similar_char
 */
static int php_u_similar_char(const UChar *txt1, int len1, const UChar *txt2, int len2)
{
	int sum, max;
	int pos1, pos2, end1, end2;

	php_u_similar_str(txt1, len1, txt2, len2, &pos1, &end1, &pos2, &end2, &max);
	if ((sum = max)) {
		if (pos1 && pos2) {
			sum += php_u_similar_char(txt1, pos1, txt2, pos2);
		}
		if ((pos1 + end1 < len1) && (pos2 + end2 < len2)) {
			sum += php_u_similar_char((UChar *)txt1+pos1+end1, len1-pos1-end1,
									(UChar *)txt2+pos2+end2, len2-pos2-end2);
		}
	}
	return sum;
}
/* }}} */

/* {{{ proto int similar_text(string str1, string str2 [, float percent]) U
   Calculates the similarity between two strings */
PHP_FUNCTION(similar_text)
{
	zstr t1, t2;
	int t1_len, t2_len;
	zend_uchar t1_type, t2_type;
	zval *percent = NULL;
	int sim;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "TT|z", &t1, &t1_len,
							  &t1_type, &t2, &t2_len, &t2_type, &percent) == FAILURE)  {
		return;
	}

	if (percent) {
		zval_dtor(percent);
		Z_TYPE_P(percent) = IS_DOUBLE;
	}

	if (t1_len + t2_len == 0) {
		if (percent) {
			Z_DVAL_P(percent) = 0;
		}

		RETURN_LONG(0);
	}

	/* t1_type and t2_type are guaranteed to be the same */
	if (t1_type == IS_UNICODE) {
		sim = php_u_similar_char(t1.u, t1_len, t2.u, t2_len);
	} else {
		sim = php_similar_char(t1.s, t1_len, t2.s, t2_len);
	}

	if (percent) {
		if (t1_type == IS_UNICODE) {
			Z_DVAL_P(percent) = sim * 200.0 / (u_countChar32(t1.u, t1_len) + u_countChar32(t2.u, t2_len));
		} else {
			Z_DVAL_P(percent) = sim * 200.0 / (t1_len + t2_len);
		}
	}

	RETURN_LONG(sim);
}
/* }}} */

/* {{{ php_u_stripslashes
 *
 * be careful, this edits the string in-place */
PHPAPI void php_u_stripslashes(UChar *str, int *len TSRMLS_DC)
{
	int32_t tmp_len = 0, i = 0, src_len = *len;
	UChar32 ch1, ch2;

	ch1 = -1; ch2 = -1;
	while (i < src_len) {
		U16_NEXT(str, i, src_len, ch1);
		if (ch1 == '\\') {
			if (i < src_len) {
				U16_NEXT(str, i, src_len, ch2);
				if (ch2 == '0') {
					tmp_len += zend_codepoint_to_uchar('\0', str+tmp_len);
				} else {
					tmp_len += zend_codepoint_to_uchar(ch2, str+tmp_len);
				}
			}
		} else {
			tmp_len += zend_codepoint_to_uchar(ch1, str+tmp_len);
		}
	}
	*(str+tmp_len) = 0;
	str = eurealloc(str, tmp_len+1);
	*len = tmp_len;
	return;
}
/* }}} */

/* {{{ php_stripslashes
 *
 * be careful, this edits the string in-place */
PHPAPI void php_stripslashes(char *str, int *len TSRMLS_DC)
{
	char *s, *t;
	int l;

	if (len != NULL) {
		l = *len;
	} else {
		l = strlen(str);
	}
	s = str;
	t = str;

	while (l > 0) {
		if (*t == '\\') {
			t++;				/* skip the slash */
			if (len != NULL) {
				(*len)--;
			}
			l--;
			if (l > 0) {
				if (*t == '0') {
					*s++='\0';
					t++;
				} else {
					*s++ = *t++;	/* preserve the next character */
				}
				l--;
			}
		} else {
			*s++ = *t++;
			l--;
		}
	}
	if (s != t) {
		*s = '\0';
	}
}
/* }}} */

/* {{{ proto string addcslashes(string str, string charlist)
   Escapes all chars mentioned in charlist with backslash. It creates octal representations if asked to backslash characters with 8th bit set or with ASCII<32 (except '\n', '\r', '\t' etc...) */
PHP_FUNCTION(addcslashes)
{
	zval **str, **what;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &str, &what) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);
	convert_to_string_ex(what);

	if (Z_STRLEN_PP(str) == 0) {
		RETURN_EMPTY_STRING();
	}

	if (Z_STRLEN_PP(what) == 0) {
		RETURN_STRINGL(Z_STRVAL_PP(str), Z_STRLEN_PP(str), 1);
	}

	RETURN_STRING(php_addcslashes(Z_STRVAL_PP(str),
	                              Z_STRLEN_PP(str),
	                              &Z_STRLEN_P(return_value), 0,
	                              Z_STRVAL_PP(what),
	                              Z_STRLEN_PP(what) TSRMLS_CC), 0);
}
/* }}} */

/* {{{ proto string addslashes(string str) U
   Escapes single quote, double quotes and backslash characters in a string with backslashes */
PHP_FUNCTION(addslashes)
{
	zval **str;
	void *tmp = NULL;
	int tmp_len = 0;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_text_ex(str);

	if (Z_TYPE_PP(str) == IS_UNICODE && Z_USTRLEN_PP(str) == 0) {
		RETURN_EMPTY_UNICODE();
	} else if (Z_TYPE_PP(str) == IS_STRING && Z_STRLEN_PP(str) == 0) {
		RETURN_EMPTY_STRING();
	}

	if (Z_TYPE_PP(str) == IS_UNICODE) {
		tmp = (UChar *)php_u_addslashes(Z_USTRVAL_PP(str), Z_USTRLEN_PP(str),
										&tmp_len, 0 TSRMLS_CC);
		RETURN_UNICODEL((UChar *)tmp, tmp_len, 0);
	} else {
		tmp = (char *)php_addslashes(Z_STRVAL_PP(str), Z_STRLEN_PP(str),
									 &tmp_len, 0 TSRMLS_CC);
		RETURN_STRINGL((char *)tmp, tmp_len, 0);
	}
}
/* }}} */

/* {{{ proto string stripcslashes(string str)
   Strips backslashes from a string. Uses C-style conventions */
PHP_FUNCTION(stripcslashes)
{
	zval **str;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

	ZVAL_STRINGL(return_value, Z_STRVAL_PP(str), Z_STRLEN_PP(str), 1);
	php_stripcslashes(Z_STRVAL_P(return_value), &Z_STRLEN_P(return_value));
}
/* }}} */

/* {{{ proto string stripslashes(string str) U
   Strips backslashes from a string */
PHP_FUNCTION(stripslashes)
{
	zval **str;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_text_ex(str);

	if (Z_TYPE_PP(str) == IS_UNICODE) {
		ZVAL_UNICODEL(return_value, Z_USTRVAL_PP(str), Z_USTRLEN_PP(str), 1);
		php_u_stripslashes(Z_USTRVAL_P(return_value), &Z_USTRLEN_P(return_value) TSRMLS_CC);
	} else {
		ZVAL_STRINGL(return_value, Z_STRVAL_PP(str), Z_STRLEN_PP(str), 1);
		php_stripslashes(Z_STRVAL_P(return_value), &Z_STRLEN_P(return_value) TSRMLS_CC);
	}
}
/* }}} */

#ifndef HAVE_STRERROR
/* {{{ php_strerror
 */
char *php_strerror(int errnum)
{
	extern int sys_nerr;
	extern char *sys_errlist[];
	TSRMLS_FETCH();

	if ((unsigned int) errnum < sys_nerr) {
		return(sys_errlist[errnum]);
	}

	(void) sprintf(BG(str_ebuf), "Unknown error: %d", errnum);
	return(BG(str_ebuf));
}
/* }}} */
#endif

/* {{{ php_stripcslashes
 */
PHPAPI void php_stripcslashes(char *str, int *len)
{
	char *source, *target, *end;
	int  nlen = *len, i;
	char numtmp[4];

	for (source=str, end=str+nlen, target=str; source < end; source++) {
		if (*source == '\\' && source+1 < end) {
			source++;
			switch (*source) {
				case 'n':  *target++='\n'; nlen--; break;
				case 'r':  *target++='\r'; nlen--; break;
				case 'a':  *target++='\a'; nlen--; break;
				case 't':  *target++='\t'; nlen--; break;
				case 'v':  *target++='\v'; nlen--; break;
				case 'b':  *target++='\b'; nlen--; break;
				case 'f':  *target++='\f'; nlen--; break;
				case '\\': *target++='\\'; nlen--; break;
				case 'x':
					if (source+1 < end && isxdigit((int)(*(source+1)))) {
						numtmp[0] = *++source;
						if (source+1 < end && isxdigit((int)(*(source+1)))) {
							numtmp[1] = *++source;
							numtmp[2] = '\0';
							nlen-=3;
						} else {
							numtmp[1] = '\0';
							nlen-=2;
						}
						*target++=(char)strtol(numtmp, NULL, 16);
						break;
					}
					/* break is left intentionally */
				default:
					i=0;
					while (source < end && *source >= '0' && *source <= '7' && i<3) {
						numtmp[i++] = *source++;
					}
					if (i) {
						numtmp[i]='\0';
						*target++=(char)strtol(numtmp, NULL, 8);
						nlen-=i;
						source--;
					} else {
						*target++=*source;
						nlen--;
					}
			}
		} else {
			*target++=*source;
		}
	}

	if (nlen != 0) {
		*target='\0';
	}

	*len = nlen;
}
/* }}} */

/* {{{ php_addcslashes
 */
PHPAPI char *php_addcslashes(char *str, int length, int *new_length, int should_free, char *what, int wlength TSRMLS_DC)
{
	char flags[256];
	char *new_str = safe_emalloc(4, (length?length:(length=strlen(str))), 1);
	char *source, *target;
	char *end;
	char c;
	int  newlen;

	if (!wlength) {
		wlength = strlen(what);
	}

	if (!length) {
		length = strlen(str);
	}

	php_charmask((unsigned char*)what, wlength, flags TSRMLS_CC);

	for (source = str, end = source + length, target = new_str; (c = *source) || (source < end); source++) {
		if (flags[(unsigned char)c]) {
			if ((unsigned char) c < 32 || (unsigned char) c > 126) {
				*target++ = '\\';
				switch (c) {
					case '\n': *target++ = 'n'; break;
					case '\t': *target++ = 't'; break;
					case '\r': *target++ = 'r'; break;
					case '\a': *target++ = 'a'; break;
					case '\v': *target++ = 'v'; break;
					case '\b': *target++ = 'b'; break;
					case '\f': *target++ = 'f'; break;
					default: target += sprintf(target, "%03o", (unsigned char) c);
				}
				continue;
			}
			*target++ = '\\';
		}
		*target++ = c;
	}
	*target = 0;
	newlen = target - new_str;
	if (target - new_str < length * 4) {
		new_str = erealloc(new_str, newlen + 1);
	}
	if (new_length) {
		*new_length = newlen;
	}
	if (should_free) {
		STR_FREE(str);
	}
	return new_str;
}
/* }}} */

/* {{{ php_u_addslashes
 */
PHPAPI UChar *php_u_addslashes(UChar *str, int length, int *new_length, int should_free TSRMLS_DC)
{
	return php_u_addslashes_ex(str, length, new_length, should_free, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ php_u_addslashes_ex
 */
PHPAPI UChar *php_u_addslashes_ex(UChar *str, int length, int *new_length, int should_free, int ignore_sybase TSRMLS_DC)
{
	UChar *buf;
	int32_t buf_len = 0, i = 0;
	UChar32 ch;

	if (!new_length) {
		new_length = &buf_len;
	}
	if (!str) {
		*new_length = 0;
		return str;
	}

	buf = eumalloc(length * 2 + 1);

	while (i < length) {
		U16_NEXT(str, i, length, ch);
		switch (ch) {
			case '\0':
				*(buf+buf_len) = (UChar)0x5C; buf_len++; /* \ */
				*(buf+buf_len) = (UChar)0x30; buf_len++; /* 0 */
				break;
			case '\'':
				case '\"':
			case '\\':
					*(buf+buf_len) = (UChar)0x5C; buf_len++; /* \ */
					/* break is missing *intentionally* */
			default:
					buf_len += zend_codepoint_to_uchar(ch, buf+buf_len);
					break;
		}
	}

	*(buf+buf_len) = 0;

	if (should_free) {
		STR_FREE(str);
	}
	buf = eurealloc(buf, buf_len+1);
	*new_length = buf_len;
	return buf;
}
/* }}} */

/* {{{ php_addslashes
 */
PHPAPI char *php_addslashes(char *str, int length, int *new_length, int should_free TSRMLS_DC)
{
	return php_addslashes_ex(str, length, new_length, should_free, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ php_addslashes_ex
 */
PHPAPI char *php_addslashes_ex(char *str, int length, int *new_length, int should_free, int ignore_sybase TSRMLS_DC)
{
	/* maximum string length, worst case situation */
	char *new_str;
	char *source, *target;
	char *end;
	int local_new_length;

	if (!new_length) {
		new_length = &local_new_length;
	}
	if (!str) {
		*new_length = 0;
		return str;
	}
	new_str = (char *) safe_emalloc(2, (length ? length : (length = strlen(str))), 1);
	source = str;
	end = source + length;
	target = new_str;

	while (source < end) {
		switch (*source) {
			case '\0':
				*target++ = '\\';
				*target++ = '0';
				break;
			case '\'':
				case '\"':
			case '\\':
					*target++ = '\\';
					/* break is missing *intentionally* */
			default:
					*target++ = *source;
					break;
		}

		source++;
	}

	*target = 0;
	*new_length = target - new_str;
	if (should_free) {
		STR_FREE(str);
	}
	new_str = (char *) erealloc(new_str, *new_length + 1);
	return new_str;
}
/* }}} */

#define _HEB_BLOCK_TYPE_ENG 1
#define _HEB_BLOCK_TYPE_HEB 2
#define isheb(c)      (((((unsigned char) c) >= 224) && (((unsigned char) c) <= 250)) ? 1 : 0)
#define _isblank(c)   (((((unsigned char) c) == ' '  || ((unsigned char) c) == '\t')) ? 1 : 0)
#define _isnewline(c) (((((unsigned char) c) == '\n' || ((unsigned char) c) == '\r')) ? 1 : 0)

/* {{{ php_char_to_str_ex
 */
PHPAPI int php_char_to_str_ex(char *str, uint len, char from, char *to, int to_len, zval *result, int case_sensitivity, int *replace_count)
{
	int char_count = 0;
	int replaced = 0;
	char *source, *target, *tmp, *source_end=str+len, *tmp_end = NULL;

	if (case_sensitivity) {
		char *p = str, *e = p + len;
		while ((p = memchr(p, from, (e - p)))) {
			char_count++;
			p++;
		}
	} else {
		for (source = str; source < source_end; source++) {
			if (tolower(*source) == tolower(from)) {
				char_count++;
			}
		}
	}

	if (char_count == 0 && case_sensitivity) {
		ZVAL_STRINGL(result, str, len, 1);
		return 0;
	}

	Z_STRLEN_P(result) = len + (char_count * (to_len - 1));
	Z_STRVAL_P(result) = target = emalloc(Z_STRLEN_P(result) + 1);
	Z_TYPE_P(result) = IS_STRING;

	if (case_sensitivity) {
		char *p = str, *e = p + len, *s = str;
		while ((p = memchr(p, from, (e - p)))) {
			memcpy(target, s, (p - s));
			target += p - s;
			memcpy(target, to, to_len);
			target += to_len;
			p++;
			s = p;
		}
		if (s < e) {
			memcpy(target, s, (e - s));
			target += e - s;
		}
	} else {
		for (source = str; source < source_end; source++) {
			if (tolower(*source) == tolower(from)) {
				replaced = 1;
				if (replace_count) {
					*replace_count += 1;
				}
				for (tmp = to, tmp_end = tmp+to_len; tmp < tmp_end; tmp++) {
					*target = *tmp;
					target++;
				}
			} else {
				*target = *source;
				target++;
			}
		}
	}
	*target = 0;
	return replaced;
}
/* }}} */

/* {{{ php_char_to_str
 */
PHPAPI int php_char_to_str(char *str, uint len, char from, char *to, int to_len, zval *result)
{
	return php_char_to_str_ex(str, len, from, to, to_len, result, 1, NULL);
}
/* }}} */

/* {{{ php_str_to_str_ex
 */
PHPAPI char *php_str_to_str_ex(char *haystack, int length,
	char *needle, int needle_len, char *str, int str_len, int *_new_length, int case_sensitivity, int *replace_count)
{
	char *new_str;

	if (needle_len < length) {
		char *end, *haystack_dup = NULL, *needle_dup = NULL;
		char *e, *s, *p, *r;

		if (needle_len == str_len) {
			new_str = estrndup(haystack, length);
			*_new_length = length;

			if (case_sensitivity) {
				end = new_str + length;
				for (p = new_str; (r = php_memnstr(p, needle, needle_len, end)); p = r + needle_len) {
					memcpy(r, str, str_len);
					if (replace_count) {
						(*replace_count)++;
					}
				}
			} else {
				haystack_dup = estrndup(haystack, length);
				needle_dup = estrndup(needle, needle_len);
				php_strtolower(haystack_dup, length);
				php_strtolower(needle_dup, needle_len);
				end = haystack_dup + length;
				for (p = haystack_dup; (r = php_memnstr(p, needle_dup, needle_len, end)); p = r + needle_len) {
					memcpy(new_str + (r - haystack_dup), str, str_len);
					if (replace_count) {
						(*replace_count)++;
					}
				}
				efree(haystack_dup);
				efree(needle_dup);
			}
			return new_str;
		} else {
			if (!case_sensitivity) {
				haystack_dup = estrndup(haystack, length);
				needle_dup = estrndup(needle, needle_len);
				php_strtolower(haystack_dup, length);
				php_strtolower(needle_dup, needle_len);
			}

			if (str_len < needle_len) {
				new_str = emalloc(length + 1);
			} else {
				int count = 0;
				char *o, *n, *endp;

				if (case_sensitivity) {
					o = haystack;
					n = needle;
				} else {
					o = haystack_dup;
					n = needle_dup;
				}
				endp = o + length;

				while ((o = php_memnstr(o, n, needle_len, endp))) {
					o += needle_len;
					count++;
				}
				if (count == 0) {
					/* Needle doesn't occur, shortcircuit the actual replacement. */
					if (haystack_dup) {
						efree(haystack_dup);
					}
					if (needle_dup) {
						efree(needle_dup);
					}
					new_str = estrndup(haystack, length);
					if (_new_length) {
						*_new_length = length;
					}
					return new_str;
				} else {
					new_str = safe_emalloc(count, str_len - needle_len, length + 1);
				}
			}

			e = s = new_str;

			if (case_sensitivity) {
				end = haystack + length;
				for (p = haystack; (r = php_memnstr(p, needle, needle_len, end)); p = r + needle_len) {
					memcpy(e, p, r - p);
					e += r - p;
					memcpy(e, str, str_len);
					e += str_len;
					if (replace_count) {
						(*replace_count)++;
					}
				}

				if (p < end) {
					memcpy(e, p, end - p);
					e += end - p;
				}
			} else {
				end = haystack_dup + length;

				for (p = haystack_dup; (r = php_memnstr(p, needle_dup, needle_len, end)); p = r + needle_len) {
					memcpy(e, haystack + (p - haystack_dup), r - p);
					e += r - p;
					memcpy(e, str, str_len);
					e += str_len;
					if (replace_count) {
						(*replace_count)++;
					}
				}

				if (p < end) {
					memcpy(e, haystack + (p - haystack_dup), end - p);
					e += end - p;
				}
			}

			if (haystack_dup) {
				efree(haystack_dup);
			}
			if (needle_dup) {
				efree(needle_dup);
			}

			*e = '\0';
			*_new_length = e - s;

			new_str = erealloc(new_str, *_new_length + 1);
			return new_str;
		}
	} else if (needle_len > length) {
nothing_todo:
		*_new_length = length;
		new_str = estrndup(haystack, length);
		return new_str;
	} else {
		if (case_sensitivity ? strncmp(haystack, needle, length) : strncasecmp(haystack, needle, length)) {
			goto nothing_todo;
		} else {
			*_new_length = str_len;
			new_str = estrndup(str, str_len);
			if (replace_count) {
				(*replace_count)++;
			}
			return new_str;
		}
	}

}
/* }}} */

/* {{{ php_str_to_str
 */
PHPAPI char *php_str_to_str(char *haystack, int length,
	char *needle, int needle_len, char *str, int str_len, int *_new_length)
{
	return php_str_to_str_ex(haystack, length, needle, needle_len, str, str_len, _new_length, 1, NULL);
}
/* }}}
 */

/* {{{ php_str_replace_in_subject
 */
static void php_str_replace_in_subject(zval *search, zval *replace, zval **subject, zval *result, int case_sensitivity, int *replace_count)
{
	zval		**search_entry,
				**replace_entry = NULL,
				  temp_result;
	char		*replace_value = NULL;
	int			 replace_len = 0;

	/* Make sure we're dealing with strings. */
	convert_to_string_ex(subject);
	Z_TYPE_P(result) = IS_STRING;
	if (Z_STRLEN_PP(subject) == 0) {
		ZVAL_STRINGL(result, "", 0, 1);
		return;
	}

	/* If search is an array */
	if (Z_TYPE_P(search) == IS_ARRAY) {
		/* Duplicate subject string for repeated replacement */
		*result = **subject;
		zval_copy_ctor(result);
		INIT_PZVAL(result);

		zend_hash_internal_pointer_reset(Z_ARRVAL_P(search));

		if (Z_TYPE_P(replace) == IS_ARRAY) {
			zend_hash_internal_pointer_reset(Z_ARRVAL_P(replace));
		} else {
			/* Set replacement value to the passed one */
			replace_value = Z_STRVAL_P(replace);
			replace_len = Z_STRLEN_P(replace);
		}

		/* For each entry in the search array, get the entry */
		while (zend_hash_get_current_data(Z_ARRVAL_P(search), (void **) &search_entry) == SUCCESS) {
			/* Make sure we're dealing with strings. */
			SEPARATE_ZVAL(search_entry);
			convert_to_string(*search_entry);
			if (Z_STRLEN_PP(search_entry) == 0) {
				zend_hash_move_forward(Z_ARRVAL_P(search));
				if (Z_TYPE_P(replace) == IS_ARRAY) {
					zend_hash_move_forward(Z_ARRVAL_P(replace));
				}
				continue;
			}

			/* If replace is an array. */
			if (Z_TYPE_P(replace) == IS_ARRAY) {
				/* Get current entry */
				if (zend_hash_get_current_data(Z_ARRVAL_P(replace), (void **)&replace_entry) == SUCCESS) {
					/* Make sure we're dealing with strings. */
					convert_to_string_ex(replace_entry);

					/* Set replacement value to the one we got from array */
					replace_value = Z_STRVAL_PP(replace_entry);
					replace_len = Z_STRLEN_PP(replace_entry);

					zend_hash_move_forward(Z_ARRVAL_P(replace));
				} else {
					/* We've run out of replacement strings, so use an empty one. */
					replace_value = "";
					replace_len = 0;
				}
			}

			if (Z_STRLEN_PP(search_entry) == 1) {
				php_char_to_str_ex(Z_STRVAL_P(result),
								Z_STRLEN_P(result),
								Z_STRVAL_PP(search_entry)[0],
								replace_value,
								replace_len,
								&temp_result,
								case_sensitivity,
								replace_count);
			} else if (Z_STRLEN_PP(search_entry) > 1) {
				Z_STRVAL(temp_result) = php_str_to_str_ex(Z_STRVAL_P(result), Z_STRLEN_P(result),
														   Z_STRVAL_PP(search_entry), Z_STRLEN_PP(search_entry),
														   replace_value, replace_len, &Z_STRLEN(temp_result), case_sensitivity, replace_count);
			}

			efree(Z_STRVAL_P(result));
			Z_STRVAL_P(result) = Z_STRVAL(temp_result);
			Z_STRLEN_P(result) = Z_STRLEN(temp_result);

			if (Z_STRLEN_P(result) == 0) {
				return;
			}

			zend_hash_move_forward(Z_ARRVAL_P(search));
		}
	} else {
		if (Z_STRLEN_P(search) == 1) {
			php_char_to_str_ex(Z_STRVAL_PP(subject),
							Z_STRLEN_PP(subject),
							Z_STRVAL_P(search)[0],
							Z_STRVAL_P(replace),
							Z_STRLEN_P(replace),
							result,
							case_sensitivity,
							replace_count);
		} else if (Z_STRLEN_P(search) > 1) {
			Z_STRVAL_P(result) = php_str_to_str_ex(Z_STRVAL_PP(subject), Z_STRLEN_PP(subject),
													Z_STRVAL_P(search), Z_STRLEN_P(search),
													Z_STRVAL_P(replace), Z_STRLEN_P(replace), &Z_STRLEN_P(result), case_sensitivity, replace_count);
		} else {
			*result = **subject;
			zval_copy_ctor(result);
			INIT_PZVAL(result);
		}
	}
}
/* }}} */

/* {{{ php_str_replace_common
 */
static void php_str_replace_common(INTERNAL_FUNCTION_PARAMETERS, int case_sensitivity)
{
	zval **subject, **search, **replace, **subject_entry, **zcount;
	zval *result;
	zstr string_key;
	uint string_key_len;
	ulong num_key;
	int count = 0;
	int argc = ZEND_NUM_ARGS();

	if (argc < 3 || argc > 4 ||
	   zend_get_parameters_ex(argc, &search, &replace, &subject, &zcount) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	SEPARATE_ZVAL(search);
	SEPARATE_ZVAL(replace);
	SEPARATE_ZVAL(subject);

	/* Make sure we're dealing with strings and do the replacement. */
	if (Z_TYPE_PP(search) != IS_ARRAY) {
		convert_to_string_ex(search);
		convert_to_string_ex(replace);
	} else if (Z_TYPE_PP(replace) != IS_ARRAY) {
		convert_to_string_ex(replace);
	}

	/* if subject is an array */
	if (Z_TYPE_PP(subject) == IS_ARRAY) {
		array_init(return_value);
		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(subject));

		/* For each subject entry, convert it to string, then perform replacement
		   and add the result to the return_value array. */
		while (zend_hash_get_current_data(Z_ARRVAL_PP(subject), (void **)&subject_entry) == SUCCESS) {
			if (Z_TYPE_PP(subject_entry) != IS_ARRAY && Z_TYPE_PP(subject_entry) != IS_OBJECT) {
				MAKE_STD_ZVAL(result);
				SEPARATE_ZVAL(subject_entry);
				php_str_replace_in_subject(*search, *replace, subject_entry, result, case_sensitivity, (argc > 3) ? &count : NULL);
			} else {
				ALLOC_ZVAL(result);
				ZVAL_ADDREF(*subject_entry);
				COPY_PZVAL_TO_ZVAL(*result, *subject_entry);
			}
			/* Add to return array */
			switch (zend_hash_get_current_key_ex(Z_ARRVAL_PP(subject), &string_key,
												&string_key_len, &num_key, 0, NULL)) {
				case HASH_KEY_IS_STRING:
				case HASH_KEY_IS_UNICODE:
					add_assoc_zval_ex(return_value, string_key.s, string_key_len, result);
					break;

				case HASH_KEY_IS_LONG:
					add_index_zval(return_value, num_key, result);
					break;
			}

			zend_hash_move_forward(Z_ARRVAL_PP(subject));
		}
	} else {	/* if subject is not an array */
		php_str_replace_in_subject(*search, *replace, subject, return_value, case_sensitivity, (argc > 3) ? &count : NULL);
	}
	if (argc > 3) {
		zval_dtor(*zcount);
		ZVAL_LONG(*zcount, count);
	}
}
/* }}} */

/* {{{ proto mixed str_replace(mixed search, mixed replace, mixed subject [, int &replace_count])
   Replaces all occurrences of search in haystack with replace */
PHP_FUNCTION(str_replace)
{
	php_str_replace_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto mixed str_ireplace(mixed search, mixed replace, mixed subject [, int &replace_count])
   Replaces all occurrences of search in haystack with replace / case-insensitive */
PHP_FUNCTION(str_ireplace)
{
	php_str_replace_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ php_hebrev
 *
 * Converts Logical Hebrew text (Hebrew Windows style) to Visual text
 * Cheers/complaints/flames - Zeev Suraski <zeev@php.net>
 */
static void php_hebrev(INTERNAL_FUNCTION_PARAMETERS, int convert_newlines)
{
	zval **str, **max_chars_per_line;
	char *heb_str, *tmp, *target, *broken_str;
	int block_start, block_end, block_type, block_length, i;
	long max_chars=0;
	int begin, end, char_count, orig_begin;


	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &str) == FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &str, &max_chars_per_line) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(max_chars_per_line);
			max_chars = Z_LVAL_PP(max_chars_per_line);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	convert_to_string_ex(str);

	if (Z_STRLEN_PP(str) == 0) {
		RETURN_FALSE;
	}

	tmp = Z_STRVAL_PP(str);
	block_start=block_end=0;

	heb_str = (char *) emalloc(Z_STRLEN_PP(str)+1);
	target = heb_str+Z_STRLEN_PP(str);
	*target = 0;
	target--;

	block_length=0;

	if (isheb(*tmp)) {
		block_type = _HEB_BLOCK_TYPE_HEB;
	} else {
		block_type = _HEB_BLOCK_TYPE_ENG;
	}

	do {
		if (block_type == _HEB_BLOCK_TYPE_HEB) {
			while ((isheb((int)*(tmp+1)) || _isblank((int)*(tmp+1)) || ispunct((int)*(tmp+1)) || (int)*(tmp+1)=='\n' ) && block_end<Z_STRLEN_PP(str)-1) {
				tmp++;
				block_end++;
				block_length++;
			}
			for (i = block_start; i<= block_end; i++) {
				*target = Z_STRVAL_PP(str)[i];
				switch (*target) {
					case '(':
						*target = ')';
						break;
					case ')':
						*target = '(';
						break;
					case '[':
						*target = ']';
						break;
					case ']':
						*target = '[';
						break;
					case '{':
						*target = '}';
						break;
					case '}':
						*target = '{';
						break;
					case '<':
						*target = '>';
						break;
					case '>':
						*target = '<';
						break;
					case '\\':
						*target = '/';
						break;
					case '/':
						*target = '\\';
						break;
					default:
						break;
				}
				target--;
			}
			block_type = _HEB_BLOCK_TYPE_ENG;
		} else {
			while (!isheb(*(tmp+1)) && (int)*(tmp+1)!='\n' && block_end < Z_STRLEN_PP(str)-1) {
				tmp++;
				block_end++;
				block_length++;
			}
			while ((_isblank((int)*tmp) || ispunct((int)*tmp)) && *tmp!='/' && *tmp!='-' && block_end > block_start) {
				tmp--;
				block_end--;
			}
			for (i = block_end; i >= block_start; i--) {
				*target = Z_STRVAL_PP(str)[i];
				target--;
			}
			block_type = _HEB_BLOCK_TYPE_HEB;
		}
		block_start=block_end+1;
	} while (block_end < Z_STRLEN_PP(str)-1);


	broken_str = (char *) emalloc(Z_STRLEN_PP(str)+1);
	begin=end=Z_STRLEN_PP(str)-1;
	target = broken_str;

	while (1) {
		char_count=0;
		while ((!max_chars || char_count < max_chars) && begin > 0) {
			char_count++;
			begin--;
			if (begin <= 0 || _isnewline(heb_str[begin])) {
				while (begin > 0 && _isnewline(heb_str[begin-1])) {
					begin--;
					char_count++;
				}
				break;
			}
		}
		if (char_count == max_chars) { /* try to avoid breaking words */
			int new_char_count=char_count, new_begin=begin;

			while (new_char_count > 0) {
				if (_isblank(heb_str[new_begin]) || _isnewline(heb_str[new_begin])) {
					break;
				}
				new_begin++;
				new_char_count--;
			}
			if (new_char_count > 0) {
				char_count=new_char_count;
				begin=new_begin;
			}
		}
		orig_begin=begin;

		if (_isblank(heb_str[begin])) {
			heb_str[begin]='\n';
		}
		while (begin <= end && _isnewline(heb_str[begin])) { /* skip leading newlines */
			begin++;
		}
		for (i = begin; i <= end; i++) { /* copy content */
			*target = heb_str[i];
			target++;
		}
		for (i = orig_begin; i <= end && _isnewline(heb_str[i]); i++) {
			*target = heb_str[i];
			target++;
		}
		begin=orig_begin;

		if (begin <= 0) {
			*target = 0;
			break;
		}
		begin--;
		end=begin;
	}
	efree(heb_str);

	if (convert_newlines) {
		php_char_to_str(broken_str, Z_STRLEN_PP(str),'\n', "<br />\n", 7, return_value);
		efree(broken_str);
	} else {
		Z_STRVAL_P(return_value) = broken_str;
		Z_STRLEN_P(return_value) = Z_STRLEN_PP(str);
		Z_TYPE_P(return_value) = IS_STRING;
	}
}
/* }}} */

/* {{{ proto string hebrev(string str [, int max_chars_per_line])
   Converts logical Hebrew text to visual text */
PHP_FUNCTION(hebrev)
{
	php_hebrev(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string hebrevc(string str [, int max_chars_per_line])
   Converts logical Hebrew text to visual text with newline conversion */
PHP_FUNCTION(hebrevc)
{
	php_hebrev(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


/* {{{ proto string nl2br(string str)
   Converts newlines to HTML line breaks */
PHP_FUNCTION(nl2br)
{
	/* in brief this inserts <br /> before matched regexp \n\r?|\r\n? */
	zval	**zstr;
	char	*tmp, *str;
	int	new_length;
	char	*end, *target;
	int	repl_cnt = 0;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &zstr) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(zstr);

	str = Z_STRVAL_PP(zstr);
	end = str + Z_STRLEN_PP(zstr);

	/* it is really faster to scan twice and allocate mem once insted scanning once
	   and constantly reallocing */
	while (str < end) {
		if (*str == '\r') {
			if (*(str+1) == '\n') {
				str++;
			}
			repl_cnt++;
		} else if (*str == '\n') {
			if (*(str+1) == '\r') {
				str++;
			}
			repl_cnt++;
		}

		str++;
	}

	if (repl_cnt == 0) {
		RETURN_STRINGL(Z_STRVAL_PP(zstr), Z_STRLEN_PP(zstr), 1);
	}

	new_length = Z_STRLEN_PP(zstr) + repl_cnt * (sizeof("<br />") - 1);
	tmp = target = emalloc(new_length + 1);

	str = Z_STRVAL_PP(zstr);

	while (str < end) {
		switch (*str) {
			case '\r':
			case '\n':
				*target++ = '<';
				*target++ = 'b';
				*target++ = 'r';
				*target++ = ' ';
				*target++ = '/';
				*target++ = '>';

				if ((*str == '\r' && *(str+1) == '\n') || (*str == '\n' && *(str+1) == '\r')) {
					*target++ = *str++;
				}
				/* lack of a break; is intentional */
			default:
				*target++ = *str;
		}

		str++;
	}

	*target = '\0';

	RETURN_STRINGL(tmp, new_length, 0);
}
/* }}} */


/* {{{ proto string strip_tags(string str [, string allowable_tags]) U
   Strips HTML and PHP tags from a string */
PHP_FUNCTION(strip_tags)
{
	void *str, *allow = NULL;
	int str_len, allow_len = 0;
	zend_uchar str_type, allow_type;
	void *buf;
	int retval_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "T|T", &str, &str_len, &str_type,
							  &allow, &allow_len, &allow_type) == FAILURE) {
		return;
	}

	if (str_type == IS_UNICODE) {
		buf = eustrndup(str, str_len);
		retval_len = php_u_strip_tags((UChar *)buf, str_len, NULL, (UChar *)allow, allow_len TSRMLS_CC);
		RETURN_UNICODEL((UChar *)buf, retval_len, 0);
	} else {
		buf = estrndup(str, str_len);
		retval_len = php_strip_tags((char *)buf, str_len, NULL, (char *)allow, allow_len);
		RETURN_STRINGL((char *)buf, retval_len, 0);
	}
}
/* }}} */

/* {{{ proto string setlocale(mixed category, string locale [, string ...])
   Set locale information */
PHP_FUNCTION(setlocale)
{
	zval ***args = (zval ***) safe_emalloc(sizeof(zval **), ZEND_NUM_ARGS(), 0);
	zval **pcategory, **plocale;
	int i, cat, n_args=ZEND_NUM_ARGS();
	char *loc, *retval;

	if (zend_get_parameters_array_ex(n_args, args) == FAILURE || n_args < 2) {
		efree(args);
		WRONG_PARAM_COUNT;
	}
#ifdef HAVE_SETLOCALE
	pcategory = args[0];
	if (Z_TYPE_PP(pcategory) == IS_LONG) {
		convert_to_long_ex(pcategory);
		cat = Z_LVAL_PP(pcategory);
	} else { /* FIXME: The following behaviour should be removed. */
		char *category;
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Passing locale category name as string is deprecated. Use the LC_* -constants instead.");
		convert_to_string_ex(pcategory);
		category = Z_STRVAL_P(*pcategory);

		if (!strcasecmp ("LC_ALL", category))
			cat = LC_ALL;
		else if (!strcasecmp ("LC_COLLATE", category))
			cat = LC_COLLATE;
		else if (!strcasecmp ("LC_CTYPE", category))
			cat = LC_CTYPE;
#ifdef LC_MESSAGES
		else if (!strcasecmp ("LC_MESSAGES", category))
			cat = LC_MESSAGES;
#endif
		else if (!strcasecmp ("LC_MONETARY", category))
			cat = LC_MONETARY;
		else if (!strcasecmp ("LC_NUMERIC", category))
			cat = LC_NUMERIC;
		else if (!strcasecmp ("LC_TIME", category))
			cat = LC_TIME;
		else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid locale category name %s, must be one of LC_ALL, LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, or LC_TIME.", category);
			efree(args);
			RETURN_FALSE;
		}
	}

	if (Z_TYPE_PP(args[1]) == IS_ARRAY) {
		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(args[1]));
		i=0; /* not needed in this case: only kill a compiler warning */
	} else {
		i=1;
	}
	while (1) {
		if (Z_TYPE_PP(args[1]) == IS_ARRAY) {
			if (!zend_hash_num_elements(Z_ARRVAL_PP(args[1]))) {
				break;
			}
			zend_hash_get_current_data(Z_ARRVAL_PP(args[1]),(void **)&plocale);
		} else {
			plocale = args[i];
		}

		convert_to_string_ex(plocale);

		if (!strcmp ("0", Z_STRVAL_PP(plocale))) {
			loc = NULL;
		} else {
			loc = Z_STRVAL_PP(plocale);
		}

		retval = setlocale (cat, loc);
		if (retval) {
			/* Remember if locale was changed */
			if (loc) {
				STR_FREE(BG(locale_string));
				BG(locale_string) = estrdup(retval);
			}

			efree(args);
			RETVAL_STRING(retval, 1);

			return;
		}

		if (Z_TYPE_PP(args[1]) == IS_ARRAY) {
			if (zend_hash_move_forward(Z_ARRVAL_PP(args[1])) == FAILURE) break;
		} else {
			if (++i >= n_args) break;
		}
	}

#endif
	efree(args);

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto void parse_str(string encoded_string [, array result])
   Parses GET/POST/COOKIE data and sets global variables */
PHP_FUNCTION(parse_str)
{
	zval **arg;
	zval **arrayArg;
	zval *sarg;
	char *res = NULL;
	int argCount;

	argCount = ZEND_NUM_ARGS();
	if (argCount < 1 || argCount > 2 || zend_get_parameters_ex(argCount, &arg, &arrayArg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg);
	sarg = *arg;
	if (Z_STRVAL_P(sarg) && *Z_STRVAL_P(sarg)) {
		res = estrndup(Z_STRVAL_P(sarg), Z_STRLEN_P(sarg));
	}

	if (argCount == 1) {
		zval tmp;
		Z_ARRVAL(tmp) = EG(active_symbol_table);

		sapi_module.treat_data(PARSE_STRING, res, &tmp TSRMLS_CC);
	} else 	{
		/* Clear out the array that was passed in. */
		zval_dtor(*arrayArg);
		array_init(*arrayArg);

		sapi_module.treat_data(PARSE_STRING, res, *arrayArg TSRMLS_CC);
	}
}
/* }}} */

#define PHP_TAG_BUF_SIZE 1023

/* php_u_tag_find / php_tag_find
 *
 * Check if tag is in a set of tags
 *
 * states:
 *
 * 0 start tag
 * 1 first non-whitespace char seen
 */

/* {{{ php_u_tag_find
 */
int php_u_tag_find(UChar *tag, int len, UChar *set, int set_len)
{
	int32_t idx = 0;
	UChar32 ch;
	UChar *norm, *n;
	int state = 0, done = 0;

	if (!len) {
		return 0;
	}

	norm = eumalloc(len+1);
	n = norm;

	while (!done) {
		U16_NEXT(tag, idx, len, ch);
		switch (u_tolower(ch)) {
		case '<':
			*(n++) = ch;
			break;
		case '>':
			done = 1;
			break;
		default:
			if (u_isWhitespace(ch) == FALSE) {
				if (state == 0) {
					state = 1;
					if (ch != '/')
						*(n++) = ch;
				} else {
					*(n++) = ch;
				}
			} else {
				if (state == 1)
					done = 1;
			}
			break;
		}
	}
	*(n++) = '>';
	*n = 0;

	if (u_strFindFirst(set, set_len, norm, n-norm) != NULL) {
		done = 1;
	} else {
		done = 0;
	}

	efree(norm);
	return done;
}
/* }}} */

/* {{{ php_tag_find
 */
int php_tag_find(char *tag, int len, char *set) {
	char c, *n, *t;
	int state=0, done=0;
	char *norm = emalloc(len+1);

	n = norm;
	t = tag;
	c = tolower(*t);
	/*
	   normalize the tag removing leading and trailing whitespace
	   and turn any <a whatever...> into just <a> and any </tag>
	   into <tag>
	*/
	if (!len) {
		return 0;
	}
	while (!done) {
		switch (c) {
			case '<':
				*(n++) = c;
				break;
			case '>':
				done =1;
				break;
			default:
				if (!isspace((int)c)) {
					if (state == 0) {
						state=1;
						if (c != '/')
							*(n++) = c;
					} else {
						*(n++) = c;
					}
				} else {
					if (state == 1)
						done=1;
				}
				break;
		}
		c = tolower(*(++t));
	}
	*(n++) = '>';
	*n = '\0';
	if (strstr(set, norm)) {
		done=1;
	} else {
		done=0;
	}
	efree(norm);
	return done;
}
/* }}} */

/* php_u_strip_tags / php_strip_tags

	A simple little state-machine to strip out html and php tags

	State 0 is the output state, State 1 means we are inside a
	normal html tag and state 2 means we are inside a php tag.

	The state variable is passed in to allow a function like fgetss
	to maintain state across calls to the function.

	lc holds the last significant character read and br is a bracket
	counter.

	When an allow string is passed in we keep track of the string
	in state 1 and when the tag is closed check it against the
	allow string to see if we should allow it.

	swm: Added ability to strip <?xml tags without assuming it PHP
	code.
*/
/* {{{ php_u_strip_tags
 */
PHPAPI int php_u_strip_tags(UChar *rbuf, int len, int *stateptr, UChar *allow, int allow_len TSRMLS_DC)
{
	UChar *tbuf = NULL, *tp = NULL;
	UChar *buf, *rp;
	int32_t idx = 0, tmp, codepts;
	UChar32 ch, next, prev1, prev2, last, doctype[6];
	int br = 0, depth = 0, state = 0, i;

	if (stateptr)
		state = *stateptr;

	buf = eustrndup(rbuf, len);
	rp = rbuf;
	if (allow_len != 0) {
		allow = php_u_strtolower(allow, &allow_len, UG(default_locale));
		tbuf = eumalloc(PHP_TAG_BUF_SIZE+1);
		tp = tbuf;
	}

	last = 0x00;
	ch = prev1 = prev2 = next = -1;
	codepts = 0;
	while (idx < len) {
		prev2 = prev1; prev1 = ch;
		U16_NEXT(buf, idx, len, ch);
		codepts++;

		switch (ch) {
		case 0x00: /* '\0' */
			break;

		case 0x3C: /* '<' */
			U16_GET(buf, 0, idx, len, next);
			if (u_isWhitespace(next) == TRUE) {
				goto reg_u_char;
			}
			if (state == 0) {
				last = 0x3C;
				state = 1;
				if (allow_len) {
					tp = ((tp-tbuf) >= UBYTES(PHP_TAG_BUF_SIZE) ? tbuf: tp);
					*(tp++) = ch;
				}
			} else if (state == 1) {
				depth++;
			}
			break;

		case 0x28: /* '(' */
			if (state == 2) {
				if (last != 0x22 && last != 0x27) { /* '"' & '\'' */
					last = 0x28;
					br++;
				}
			} else if (allow_len && state == 1) {
				tp = ((tp-tbuf) >= UBYTES(PHP_TAG_BUF_SIZE) ? tbuf: tp);
				*(tp++) = ch;
			} else if (state == 0) {
				*(rp++) = ch;
			}
			break;

		case 0x29: /* ')' */
			if (state == 2) {
				if (last != 0x22 && last != 0x27) { /* '"' & '\'' */
					last = ch;
					br--;
				}
			} else if (allow_len && state == 1) {
				tp = ((tp-tbuf) >= UBYTES(PHP_TAG_BUF_SIZE) ? tbuf: tp);
				*(tp++) = ch;
			} else if (state == 0) {
				*(rp++) = ch;
			}
			break;

		case 0x3E: /* '>' */
			if (depth) {
				depth--;
				break;
			}

			switch (state) {
			case 1: /* HTML/XML */
				last = ch;
				state = 0;
				if (allow_len) {
					tp = ((tp-tbuf) >= UBYTES(PHP_TAG_BUF_SIZE) ? tbuf: tp);
					*(tp++) = ch;
					*(tp) = 0;
					if (php_u_tag_find(tbuf, tp-tbuf, allow, allow_len)) {
						u_memcpy(rp, tbuf, tp-tbuf);
						rp += tp-tbuf;
					}
					tp = tbuf;
				}
				break;

			case 2: /* PHP */
				if (!br && last != 0x22 && prev1 == 0x3F) { /* '"' & '?' */
					state = 0;
					tp = tbuf;
				}
				break;

			case 3:
				state = 0;
				tp = tbuf;
				break;

			case 4: /* JavaScript/CSS/etc... */
				if (codepts >= 2 && prev1 == 0x2D && prev2 == 0x2D) { /* '-' */
					state = 0;
					tp = tbuf;
				}
				break;

			default:
				*(rp++) = ch;
				break;
			}
			break;

		case 0x22: /* '"' */
		case 0x27: /* '\'' */
			if (state == 2 && prev1 != 0x5C) { /* '\\' */
				if (last == ch) {
					last = 0x00;
				} else if (last != 0x5C) {
					last = ch;
				}
			} else if (state == 0) {
				*(rp++) = ch;
			} else if (allow_len && state == 1) {
				tp = ((tp-tbuf) >= UBYTES(PHP_TAG_BUF_SIZE) ? tbuf: tp);
				*(tp++) = ch;
			}
			break;

		case 0x21: /* '!' */
			/* JavaScript & Other HTML scripting languages */
			if (state == 1 && prev1 == 0x3C) { /* '<' */
				state = 3;
				last = ch;
			} else {
				if (state == 0) {
					*(rp++) = ch;
				} else if (allow_len && state == 1) {
					tp = ((tp-tbuf) >= UBYTES(PHP_TAG_BUF_SIZE) ? tbuf: tp);
					*(tp++) = ch;
				}
			}
			break;

		case 0x2D: /* '-' */
			if (state == 3 && codepts >= 2 && prev1 == 0x2D && prev2 == 0x21) { /* '-' & '!' */
				state = 4;
			} else {
				goto reg_u_char;
			}
			break;

		case 0x3F: /* '?' */
			if (state == 1 && prev1 == 0x3C) { /* '<' */
				br = 0;
				state = 2;
				break;
			}

		case 'E':
		case 'e':
			/* !DOCTYPE exception */
			if (state==3 && codepts > 6) {
				tmp = idx;
				for (i = 0 ; i < 6 ; i++) {
					U16_PREV(buf, 0, tmp, doctype[i]);
				}
				if (u_tolower(doctype[0])=='p' && u_tolower(doctype[1])=='y' &&
					u_tolower(doctype[2])=='t' && u_tolower(doctype[3])=='c' &&
					u_tolower(doctype[4])=='o' && u_tolower(doctype[5])=='d') {
					state = 1;
					break;
				}
			}
			/* fall-through */

		case 'l':

			/* swm: If we encounter '<?xml' then we shouldn't be in
			 * state == 2 (PHP). Switch back to HTML.
			 */

			if (state == 2 && codepts > 2 && prev1 == 'm' && prev2 == 'x') {
				state = 1;
				break;
			}
			/* fall-through */

		default:
reg_u_char:
			if (state == 0) {
				rp += zend_codepoint_to_uchar(ch, rp);
			} else if (allow_len && state == 1) {
				tp = ((tp-tbuf) >= UBYTES(PHP_TAG_BUF_SIZE) ? tbuf: tp);
				tp += zend_codepoint_to_uchar(ch, tp);
			}
			break;
		}
	}

	*rp = 0;
	efree(buf);
	if (allow_len) {
		efree(tbuf);
		efree(allow);
	}
	if (stateptr)
		*stateptr = state;

	return (int)(rp-rbuf);
}
/* }}} */

/* {{{ php_strip_tags
 */
PHPAPI size_t php_strip_tags(char *rbuf, int len, int *stateptr, char *allow, int allow_len)
{
	char *tbuf, *buf, *p, *tp, *rp, c, lc;
	int br, i=0, depth=0;
	int state = 0;

	if (stateptr)
		state = *stateptr;

	buf = estrndup(rbuf, len);
	c = *buf;
	lc = '\0';
	p = buf;
	rp = rbuf;
	br = 0;
	if (allow) {
		php_strtolower(allow, allow_len);
		tbuf = emalloc(PHP_TAG_BUF_SIZE+1);
		tp = tbuf;
	} else {
		tbuf = tp = NULL;
	}

	while (i < len) {
		switch (c) {
			case '\0':
				break;
			case '<':
				if (isspace(*(p + 1))) {
					goto reg_char;
				}
				if (state == 0) {
					lc = '<';
					state = 1;
					if (allow) {
						tp = ((tp-tbuf) >= PHP_TAG_BUF_SIZE ? tbuf: tp);
						*(tp++) = '<';
					}
				} else if (state == 1) {
					depth++;
				}
				break;

			case '(':
				if (state == 2) {
					if (lc != '"' && lc != '\'') {
						lc = '(';
						br++;
					}
				} else if (allow && state == 1) {
					tp = ((tp-tbuf) >= PHP_TAG_BUF_SIZE ? tbuf: tp);
					*(tp++) = c;
				} else if (state == 0) {
					*(rp++) = c;
				}
				break;

			case ')':
				if (state == 2) {
					if (lc != '"' && lc != '\'') {
						lc = ')';
						br--;
					}
				} else if (allow && state == 1) {
					tp = ((tp-tbuf) >= PHP_TAG_BUF_SIZE ? tbuf: tp);
					*(tp++) = c;
				} else if (state == 0) {
					*(rp++) = c;
				}
				break;

			case '>':
				if (depth) {
					depth--;
					break;
				}

				switch (state) {
					case 1: /* HTML/XML */
						lc = '>';
						state = 0;
						if (allow) {
							tp = ((tp-tbuf) >= PHP_TAG_BUF_SIZE ? tbuf: tp);
							*(tp++) = '>';
							*tp='\0';
							if (php_tag_find(tbuf, tp-tbuf, allow)) {
								memcpy(rp, tbuf, tp-tbuf);
								rp += tp-tbuf;
							}
							tp = tbuf;
						}
						break;

					case 2: /* PHP */
						if (!br && lc != '\"' && *(p-1) == '?') {
							state = 0;
							tp = tbuf;
						}
						break;

					case 3:
						state = 0;
						tp = tbuf;
						break;

					case 4: /* JavaScript/CSS/etc... */
						if (p >= buf + 2 && *(p-1) == '-' && *(p-2) == '-') {
							state = 0;
							tp = tbuf;
						}
						break;

					default:
						*(rp++) = c;
						break;
				}
				break;

			case '"':
			case '\'':
				if (state == 2 && *(p-1) != '\\') {
					if (lc == c) {
						lc = '\0';
					} else if (lc != '\\') {
						lc = c;
					}
				} else if (state == 0) {
					*(rp++) = c;
				} else if (allow && state == 1) {
					tp = ((tp-tbuf) >= PHP_TAG_BUF_SIZE ? tbuf: tp);
					*(tp++) = c;
				}
				break;

			case '!':
				/* JavaScript & Other HTML scripting languages */
				if (state == 1 && *(p-1) == '<') {
					state = 3;
					lc = c;
				} else {
					if (state == 0) {
						*(rp++) = c;
					} else if (allow && state == 1) {
						tp = ((tp-tbuf) >= PHP_TAG_BUF_SIZE ? tbuf: tp);
						*(tp++) = c;
					}
				}
				break;

			case '-':
				if (state == 3 && p >= buf + 2 && *(p-1) == '-' && *(p-2) == '!') {
					state = 4;
				} else {
					goto reg_char;
				}
				break;

			case '?':

				if (state == 1 && *(p-1) == '<') {
					br=0;
					state=2;
					break;
				}

			case 'E':
			case 'e':
				/* !DOCTYPE exception */
				if (state==3 && p > buf+6
						     && tolower(*(p-1)) == 'p'
					         && tolower(*(p-2)) == 'y'
						     && tolower(*(p-3)) == 't'
						     && tolower(*(p-4)) == 'c'
						     && tolower(*(p-5)) == 'o'
						     && tolower(*(p-6)) == 'd') {
					state = 1;
					break;
				}
				/* fall-through */

			case 'l':

				/* swm: If we encounter '<?xml' then we shouldn't be in
				 * state == 2 (PHP). Switch back to HTML.
				 */

				if (state == 2 && p > buf+2 && *(p-1) == 'm' && *(p-2) == 'x') {
					state = 1;
					break;
				}

				/* fall-through */
			default:
reg_char:
				if (state == 0) {
					*(rp++) = c;
				} else if (allow && state == 1) {
					tp = ((tp-tbuf) >= PHP_TAG_BUF_SIZE ? tbuf: tp);
					*(tp++) = c;
				}
				break;
		}
		c = *(++p);
		i++;
	}
	if (rp < rbuf + len) {
		*rp = '\0';
	}
	efree(buf);
	if (allow)
		efree(tbuf);
	if (stateptr)
		*stateptr = state;

	return (size_t)(rp - rbuf);
}
/* }}} */

/* {{{ proto string str_repeat(string input, int mult) U
   Returns the input string repeat mult times */
PHP_FUNCTION(str_repeat)
{
	void		*input_str;		/* Input string */
	int			input_str_len;
	int			input_str_chars;
	zend_uchar	input_str_type;
	long		mult;			/* Multiplier */
	void		*result;		/* Resulting string */
	int			result_len;		/* Length of the resulting string, in bytes */
	int			result_chars;	/* Chars/UChars in resulting string */

	if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "tl", &input_str,
							   &input_str_chars, &input_str_type, &mult) == FAILURE ) {
		return;
	}

	if ( mult < 0 ) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Second argument has to be greater than or equal to 0");
		return;
	}

	/* Don't waste our time if input is empty or if the multiplier is zero */
	if ( input_str_chars == 0 || mult == 0 ) {
		if ( input_str_type == IS_UNICODE ) {
			RETURN_UNICODEL(USTR_MAKE(""), 0, 0);
		} else {
			RETURN_STRINGL("", 0, 1);
		}
	}

	/* Initialize the result string */
	result_chars = (input_str_chars * mult) + 1;
	if ( input_str_type == IS_UNICODE ) {
		input_str_len = UBYTES(input_str_chars);
		result_len = UBYTES(result_chars);
		if ( result_chars < 1 || result_chars > (2147483647/UBYTES(1)) ) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "You may not create strings longer than %ld characters", 2147483647/UBYTES(1));
			RETURN_FALSE;
		}
	} else {
		input_str_len = input_str_chars;
		result_len = result_chars;
		if ( result_chars < 1 || result_chars > 2147483647 ) {
			if ( input_str_type == IS_STRING ) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "You may not create strings longer than 2147483647 characters");
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "You may not create strings longer than 2147483647 bytes");
			}
			RETURN_FALSE;
		}
	}
	result = emalloc(result_len);

	/* Heavy optimization for situations where input string is 1 byte long */
	if ( input_str_len == 1 ) {
		memset(result, *((char *)input_str), mult);
	} else {
		char *s, *e, *ee;
		int l=0;
		memcpy(result, input_str, input_str_len);
		s = result;
		e = (char *) result + input_str_len;
		ee = (char *) result + result_len;

		while ( e < ee ) {
			l = (e-s) < (ee-e) ? (e-s) : (ee-e);
			memmove(e, s, l);
			e += l;
		}
	}

	if ( input_str_type == IS_UNICODE ) {
		*(((UChar *)result)+result_chars-1) = 0;
		RETURN_UNICODEL((UChar *)result, result_chars-1, 0);
	} else {
		*(((char *)result)+result_chars-1) = '\0';
		RETURN_STRINGL((char *)result, result_chars-1, 0);
	}
}
/* }}} */

/* {{{ proto mixed count_chars(string input [, int mode])
   Returns info about what characters are used in input */
PHP_FUNCTION(count_chars)
{
	zval **input, **mode;
	int chars[256];
	int ac=ZEND_NUM_ARGS();
	int mymode=0;
	unsigned char *buf;
	int len, inx;
	char retstr[256];
	int retlen=0;

	if (ac < 1 || ac > 2 || zend_get_parameters_ex(ac, &input, &mode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(input);

	if (ac == 2) {
		convert_to_long_ex(mode);
		mymode = Z_LVAL_PP(mode);

		if (mymode < 0 || mymode > 4) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown mode.");
			RETURN_FALSE;
		}
	}

	len = Z_STRLEN_PP(input);
	buf = (unsigned char *) Z_STRVAL_PP(input);
	memset((void*) chars, 0, sizeof(chars));

	while (len > 0) {
		chars[*buf]++;
		buf++;
		len--;
	}

	if (mymode < 3) {
		array_init(return_value);
	}

	for (inx = 0; inx < 256; inx++) {
		switch (mymode) {
	 		case 0:
				add_index_long(return_value, inx, chars[inx]);
				break;
	 		case 1:
				if (chars[inx] != 0) {
					add_index_long(return_value, inx, chars[inx]);
				}
				break;
  			case 2:
				if (chars[inx] == 0) {
					add_index_long(return_value, inx, chars[inx]);
				}
				break;
	  		case 3:
				if (chars[inx] != 0) {
					retstr[retlen++] = inx;
				}
				break;
  			case 4:
				if (chars[inx] == 0) {
					retstr[retlen++] = inx;
				}
				break;
		}
	}

	if (mymode >= 3 && mymode <= 4) {
		RETURN_STRINGL(retstr, retlen, 1);
	}
}
/* }}} */

/* {{{ php_strnatcmp
 */
static void php_strnatcmp(INTERNAL_FUNCTION_PARAMETERS, int fold_case)
{
	zval **s1, **s2;

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &s1, &s2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(s1);
	convert_to_string_ex(s2);

	RETURN_LONG(strnatcmp_ex(Z_STRVAL_PP(s1), Z_STRLEN_PP(s1),
							 Z_STRVAL_PP(s2), Z_STRLEN_PP(s2),
							 fold_case));
}
/* }}} */

/* {{{ proto int strnatcmp(string s1, string s2)
   Returns the result of string comparison using 'natural' algorithm */
PHP_FUNCTION(strnatcmp)
{
	php_strnatcmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto array localeconv(void)
   Returns numeric formatting information based on the current locale */
PHP_FUNCTION(localeconv)
{
	zval *grouping, *mon_grouping;
	int len, i;

	/* We don't need no stinkin' parameters... */
	if (ZEND_NUM_ARGS() > 0) {
		WRONG_PARAM_COUNT;
	}

	MAKE_STD_ZVAL(grouping);
	MAKE_STD_ZVAL(mon_grouping);

	array_init(return_value);
	array_init(grouping);
	array_init(mon_grouping);

#ifdef HAVE_LOCALECONV
	{
		struct lconv currlocdata;

		localeconv_r( &currlocdata );

		/* Grab the grouping data out of the array */
		len = strlen(currlocdata.grouping);

		for (i = 0; i < len; i++) {
			add_index_long(grouping, i, currlocdata.grouping[i]);
		}

		/* Grab the monetary grouping data out of the array */
		len = strlen(currlocdata.mon_grouping);

		for (i = 0; i < len; i++) {
			add_index_long(mon_grouping, i, currlocdata.mon_grouping[i]);
		}

		add_assoc_string(return_value, "decimal_point",     currlocdata.decimal_point,     1);
		add_assoc_string(return_value, "thousands_sep",     currlocdata.thousands_sep,     1);
		add_assoc_string(return_value, "int_curr_symbol",   currlocdata.int_curr_symbol,   1);
		add_assoc_string(return_value, "currency_symbol",   currlocdata.currency_symbol,   1);
		add_assoc_string(return_value, "mon_decimal_point", currlocdata.mon_decimal_point, 1);
		add_assoc_string(return_value, "mon_thousands_sep", currlocdata.mon_thousands_sep, 1);
		add_assoc_string(return_value, "positive_sign",     currlocdata.positive_sign,     1);
		add_assoc_string(return_value, "negative_sign",     currlocdata.negative_sign,     1);
		add_assoc_long(  return_value, "int_frac_digits",   currlocdata.int_frac_digits     );
		add_assoc_long(  return_value, "frac_digits",       currlocdata.frac_digits         );
		add_assoc_long(  return_value, "p_cs_precedes",     currlocdata.p_cs_precedes       );
		add_assoc_long(  return_value, "p_sep_by_space",    currlocdata.p_sep_by_space      );
		add_assoc_long(  return_value, "n_cs_precedes",     currlocdata.n_cs_precedes       );
		add_assoc_long(  return_value, "n_sep_by_space",    currlocdata.n_sep_by_space      );
		add_assoc_long(  return_value, "p_sign_posn",       currlocdata.p_sign_posn         );
		add_assoc_long(  return_value, "n_sign_posn",       currlocdata.n_sign_posn         );
	}
#else
	/* Ok, it doesn't look like we have locale info floating around, so I guess it
	   wouldn't hurt to just go ahead and return the POSIX locale information?  */

	add_index_long(grouping, 0, -1);
	add_index_long(mon_grouping, 0, -1);

	add_assoc_string(return_value, "decimal_point",     "\x2E", 1);
	add_assoc_string(return_value, "thousands_sep",     "",     1);
	add_assoc_string(return_value, "int_curr_symbol",   "",     1);
	add_assoc_string(return_value, "currency_symbol",   "",     1);
	add_assoc_string(return_value, "mon_decimal_point", "\x2E", 1);
	add_assoc_string(return_value, "mon_thousands_sep", "",     1);
	add_assoc_string(return_value, "positive_sign",     "",     1);
	add_assoc_string(return_value, "negative_sign",     "",     1);
	add_assoc_long(  return_value, "int_frac_digits",   CHAR_MAX );
	add_assoc_long(  return_value, "frac_digits",       CHAR_MAX );
	add_assoc_long(  return_value, "p_cs_precedes",     CHAR_MAX );
	add_assoc_long(  return_value, "p_sep_by_space",    CHAR_MAX );
	add_assoc_long(  return_value, "n_cs_precedes",     CHAR_MAX );
	add_assoc_long(  return_value, "n_sep_by_space",    CHAR_MAX );
	add_assoc_long(  return_value, "p_sign_posn",       CHAR_MAX );
	add_assoc_long(  return_value, "n_sign_posn",       CHAR_MAX );
#endif

	zend_hash_update(Z_ARRVAL_P(return_value), "grouping", 9, &grouping, sizeof(zval *), NULL);
	zend_hash_update(Z_ARRVAL_P(return_value), "mon_grouping", 13, &mon_grouping, sizeof(zval *), NULL);
}
/* }}} */

/* {{{ proto int strnatcasecmp(string s1, string s2)
   Returns the result of case-insensitive string comparison using 'natural' algorithm */
PHP_FUNCTION(strnatcasecmp)
{
	php_strnatcmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int substr_count(string haystack, string needle [, int offset [, int length]]) U
   Returns the number of times a substring occurs in the string */
PHP_FUNCTION(substr_count)
{
	void *haystack, *needle;
	int haystack_len, needle_len;
	zend_uchar haystack_type, needle_type;
	long offset = 0, length = 0;
	int ac = ZEND_NUM_ARGS();
	int count = 0;
	void *p, *endp, *tmp;
	int32_t i = 0, j;
	char cmp;

	if (zend_parse_parameters(ac TSRMLS_CC, "TT|ll",
							  &haystack, &haystack_len, &haystack_type,
							  &needle, &needle_len, &needle_type,
							  &offset, &length) == FAILURE) {
		return;
	}

	if (needle_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty substring.");
		RETURN_FALSE;
	}

	if (haystack_type == IS_UNICODE) {
		p = (UChar *)haystack;
		endp = (UChar *)haystack + haystack_len;
	} else {
		p = (char *)haystack;
		endp = (char *)haystack + haystack_len;
	}

	if (ac > 2) {
		if (offset < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset should be greater than or equal to 0.");
			RETURN_FALSE;
		}
		if (haystack_type == IS_UNICODE) {
			i = 0;
			U16_FWD_N((UChar *)haystack, i, haystack_len, offset);
			p = (UChar *)haystack + i;
		} else {
			p = (char *)haystack + offset;
		}
		if (p > endp) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset value %ld exceeds string length.", offset);
			RETURN_FALSE;
		}
		if (ac == 4) {
			if (length < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length should be greater than 0.");
				RETURN_FALSE;
			}
			if (haystack_type == IS_UNICODE) {
				j = i;
				i = 0;
				U16_FWD_N((UChar *)p, i, haystack_len-j, length);
				tmp = (UChar *)p + i;
			} else {
				tmp = (char *)p + length;
			}
			if (tmp > endp) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset value %ld exceeds string length.", offset);
				RETURN_FALSE;
			} else {
				endp = tmp;
			}
		}
	}

	if (haystack_type == IS_UNICODE) {
		while ((p = zend_u_memnstr((UChar *)p, (UChar *)needle, needle_len, (UChar *)endp)) != NULL) {
			/*(UChar *)p += needle_len; // GCC 4.0.0 cannot compile this */
			p = (UChar *)p + needle_len;
			count++;
		}
	} else {
		if (needle_len == 1) {
			cmp = ((char *)needle)[0];
			while ((p = memchr(p, cmp, (char *)endp - (char *)p))) {
				count++;
				p = (char *)p + 1;
			}
		} else {
			while ((p = php_memnstr((char *)p, (char *)needle, needle_len, (char *)endp))) {
				/*(char *)p += needle_len; // GCC 4.0.0 cannot compile this */
				p = (char *)p + needle_len;
				count++;
			}
		}
	}

	RETURN_LONG(count);
}
/* }}} */

/* {{{ proto string str_pad(string input, int pad_length [, string pad_string [, int pad_type]]) U
   Returns input string padded on the left or right to specified length with pad_string */
PHP_FUNCTION(str_pad)
{
	/* Input arguments */
	void   *input;		/* Input string */
	long    pad_length;	/* Length to pad to, in codepoints for Unicode */
	void   *padstr;		/* Padding string */
	long    pad_type;	/* Padding type (left/right/both) */
	int input_len, padstr_len; /* Lengths in code units for Unicode */
	zend_uchar input_type, padstr_type;

	/* Helper variables */
	int input_codepts;	/* Number of codepts in Unicode input */
	int	num_pad_chars;	/* Number of padding characters (total - input size) */
	void   *result = NULL;	/* Resulting string */
	int32_t	result_len = 0;	/* Length of the resulting string */
	int32_t	i, j, left_pad=0, right_pad=0;
	UChar32 ch;


	if (ZEND_NUM_ARGS() < 2 || ZEND_NUM_ARGS() > 4) {
		WRONG_PARAM_COUNT;
	}
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Tl|Tl",
							  &input, &input_len, &input_type, &pad_length,
							  &padstr, &padstr_len, &padstr_type, &pad_type) == FAILURE) {
		return;
	}

	if (input_type == IS_UNICODE) {
		/* For Unicode, num_pad_chars/pad_length is number of codepoints */
		i = 0; input_codepts = 0;
		input_codepts = u_countChar32((UChar *)input, input_len);
		num_pad_chars = pad_length - input_codepts;
	} else {
		num_pad_chars = pad_length - input_len;
	}
	/* If resulting string turns out to be shorter than input string,
	   we simply copy the input and return. */
	if (num_pad_chars < 0) {
		if (input_type == IS_UNICODE) {
			RETURN_UNICODEL((UChar *)input, input_len, 1);
		} else {
			RETURN_STRINGL((char *)input, input_len, 1);
		}
	}

	/* Setup the padding string values if NOT specified. */
	if (ZEND_NUM_ARGS() > 2) {
		if (padstr_len == 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Padding string cannot be empty.");
			return;
		}
		if (ZEND_NUM_ARGS() > 3) {
			if (pad_type < STR_PAD_LEFT || pad_type > STR_PAD_BOTH) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Padding type has to be STR_PAD_LEFT, STR_PAD_RIGHT, or STR_PAD_BOTH.");
				return;
			}
		} else {
			pad_type = STR_PAD_RIGHT;
		}
	} else {
		if (input_type == IS_UNICODE) {
			padstr = USTR_MAKE(" ");
		} else {
			padstr = " ";
		}
		padstr_len = 1;
		pad_type = STR_PAD_RIGHT;
	}

	if (input_type == IS_UNICODE) {
		result = eumalloc(input_len + num_pad_chars*2 + 1);
	} else {
		result = emalloc(input_len + num_pad_chars + 1);
	}

	/* We need to figure out the left/right padding lengths. */
	switch (pad_type) {
		case STR_PAD_RIGHT:
			left_pad = 0;
			right_pad = num_pad_chars;
			break;
		case STR_PAD_LEFT:
			left_pad = num_pad_chars;
			right_pad = 0;
			break;
		case STR_PAD_BOTH:
			left_pad = num_pad_chars / 2;
			right_pad = num_pad_chars - left_pad;
			break;
	}

	/* Pad left, copy input, pad right, terminate */
	if (input_type == IS_UNICODE) {
		j = 0;
		for (i = 0; i < left_pad; i++) {
			if (j >= padstr_len) {
				j = 0;
			}
			U16_NEXT((UChar *)padstr, j, padstr_len, ch);
			result_len += zend_codepoint_to_uchar(ch, (UChar *)result + result_len);
		}
		memcpy((UChar *)result + result_len, input, UBYTES(input_len));
		result_len += input_len;
		j = 0;
		for (i = 0; i < right_pad; i++) {
			if (j >= padstr_len) {
				j = 0;
			}
			U16_NEXT((UChar *)padstr, j, padstr_len, ch);
			result_len += zend_codepoint_to_uchar(ch, (UChar *)result + result_len);
		}
		*((UChar *)result + result_len) = 0;
		result = eurealloc(result, result_len+1);
	} else {
		for (i = 0; i < left_pad; i++)
			*((char *)result + result_len++) = *((char *)padstr + (i % padstr_len));
		memcpy((char *)result + result_len, input, input_len);
		result_len += input_len;
		for (i = 0; i < right_pad; i++)
			*((char *)result + result_len++) = *((char *)padstr + (i % padstr_len));
		*((char *)result + result_len) = '\0';
	}

	if (input_type == IS_UNICODE) {
		if (ZEND_NUM_ARGS() < 3) {
			efree(padstr);
		}
		RETURN_UNICODEL((UChar *)result, result_len, 0);
	} else {
		RETURN_STRINGL((char *)result, result_len, 0);
	}
}
/* }}} */

/* {{{ proto mixed sscanf(string str, string format [, string ...])
   Implements an ANSI C compatible sscanf */
PHP_FUNCTION(sscanf)
{
	zval ***args;
	int     result;
	int	    argc = ZEND_NUM_ARGS();

	if (argc < 2) {
		WRONG_PARAM_COUNT;
	}

	args = (zval ***) safe_emalloc(argc, sizeof(zval **), 0);
	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(args[0]);
	convert_to_string_ex(args[1]);

	result = php_sscanf_internal(Z_STRVAL_PP(args[0]),
	                             Z_STRVAL_PP(args[1]),
	                             argc, args,
	                             2, &return_value TSRMLS_CC);
	efree(args);

	if (SCAN_ERROR_WRONG_PARAM_COUNT == result) {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

static char rot13_from[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char rot13_to[] = "nopqrstuvwxyzabcdefghijklmNOPQRSTUVWXYZABCDEFGHIJKLM";

/* {{{ proto string str_rot13(string str)
   Perform the rot13 transform on a string */
PHP_FUNCTION(str_rot13)
{
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg)) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg);
	RETVAL_ZVAL(*arg, 1, 0);

	php_strtr(Z_STRVAL_P(return_value), Z_STRLEN_P(return_value), rot13_from, rot13_to, 52);
}
/* }}} */


static void php_string_shuffle(char *str, long len TSRMLS_DC)
{
	long n_elems, rnd_idx, n_left;
	char temp;
	/* The implementation is stolen from array_data_shuffle       */
	/* Thus the characteristics of the randomization are the same */
	n_elems = len;

	if (n_elems <= 1) {
		return;
	}

	n_left = n_elems;

	while (--n_left) {
		rnd_idx = php_rand(TSRMLS_C);
		RAND_RANGE(rnd_idx, 0, n_left, PHP_RAND_MAX);
		if (rnd_idx != n_left) {
			temp = str[n_left];
			str[n_left] = str[rnd_idx];
			str[rnd_idx] = temp;
		}
	}
}


/* {{{ proto void str_shuffle(string str)
   Shuffles string. One permutation of all possible is created */
PHP_FUNCTION(str_shuffle)
{
	zval **arg;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg)) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg);
	RETVAL_ZVAL(*arg, 1, 0);
	if (Z_STRLEN_P(return_value) > 1) {
		php_string_shuffle(Z_STRVAL_P(return_value), (long) Z_STRLEN_P(return_value) TSRMLS_CC);
	}
}
/* }}} */

/* {{{ proto mixed str_word_count(string str, [int format [, string charlist]])
   	Counts the number of words inside a string. If format of 1 is specified,
   	then the function will return an array containing all the words
   	found inside the string. If format of 2 is specified, then the function
   	will return an associated array where the position of the word is the key
   	and the word itself is the value.

   	For the purpose of this function, 'word' is defined as a locale dependent
   	string containing alphabetic characters, which also may contain, but not start
   	with "'" and "-" characters.
*/
PHP_FUNCTION(str_word_count)
{
	char *buf, *str, *char_list = NULL, *p, *e, *s, ch[256];
	int str_len, char_list_len, word_count = 0;
	long type = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls", &str, &str_len, &type, &char_list, &char_list_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (char_list) {
		php_charmask((unsigned char*)char_list, char_list_len, ch TSRMLS_CC);
	}

	p = str;
	e = str + str_len;

	if (type == 1 || type == 2) {
		array_init(return_value);
	}

	/* first character cannot be ' or -, unless explicitly allowed by the user */
	if ((*p == '\'' && (!char_list || !ch['\''])) || (*p == '-' && (!char_list || !ch['-']))) {
		p++;
	}
	/* last character cannot be -, unless explicitly allowed by the user */
	if (*(e - 1) == '-' && (!char_list || !ch['-'])) {
		e--;
	}

	while (p < e) {
		s = p;
		while (p < e && (isalpha(*p) || (char_list && ch[(unsigned char)*p]) || *p == '\'' || *p == '-')) {
			p++;
		}
		if (p > s) {
			switch (type)
			{
				case 1:
					buf = estrndup(s, (p-s));
					add_next_index_stringl(return_value, buf, (p-s), 0);
					break;
				case 2:
					buf = estrndup(s, (p-s));
					add_index_stringl(return_value, (s - str), buf, p-s, 0);
					break;
				default:
					word_count++;
					break;
			}
		}
		p++;
	}

	if (!type) {
		RETURN_LONG(word_count);
	}
}

/* }}} */

#if HAVE_STRFMON
/* {{{ proto string money_format(string format , float value)
   Convert monetary value(s) to string */
PHP_FUNCTION(money_format)
{
	int format_len = 0, str_len;
	char *format, *str;
	double value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sd", &format, &format_len, &value) == FAILURE) {
		return;
	}

	str_len = format_len + 1024;
	str = emalloc(str_len);
	if ((str_len = strfmon(str, str_len, format, value)) < 0) {
		efree(str);
		RETURN_FALSE;
	}
	str[str_len] = 0;

	RETURN_STRINGL(erealloc(str, str_len + 1), str_len, 0);
}
/* }}} */
#endif

/* {{{ proto array str_split(string str [, int split_length])
   Convert a string to an array. If split_length is specified, break the string down into chunks each split_length characters long. */
PHP_FUNCTION(str_split)
{
	char *str;
	int str_len;
	long split_length = 1;
	char *p;
	int n_reg_segments;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &str, &str_len, &split_length) == FAILURE) {
		return;
	}

	if (split_length <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The length of each segment must be greater than zero.");
		RETURN_FALSE;
	}

	array_init(return_value);

	if (split_length >= str_len) {
		add_next_index_stringl(return_value, str, str_len, 1);
		return;
	}

	n_reg_segments = floor(str_len / split_length);
	p = str;

	while (n_reg_segments-- > 0) {
		add_next_index_stringl(return_value, p, split_length, 1);
		p += split_length;
	}

	if (p != (str + str_len)) {
		add_next_index_stringl(return_value, p, (str + str_len - p), 1);
	}
}
/* }}} */

/* {{{ proto array strpbrk(string haystack, string char_list) U
   Search a string for any of a set of characters */
PHP_FUNCTION(strpbrk)
{
	void *haystack, *char_list;
	int haystack_len, char_list_len;
	zend_uchar haystack_type, char_list_type;
	void *p = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "TT",
							  &haystack, &haystack_len, &haystack_type,
							  &char_list, &char_list_len, &char_list_type) == FAILURE) {
		RETURN_FALSE;
	}

	if (!char_list_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The character list cannot be empty.");
		RETURN_FALSE;
	}

	if (haystack_type == IS_UNICODE) {
		int32_t i, j;
		UChar32 ch1, ch2 = 0;

		for (i = 0 ; i < haystack_len ; ) {
			U16_NEXT((UChar *)haystack, i, haystack_len, ch1);
			for (j = 0 ; j < char_list_len ; ) {
				U16_NEXT((UChar *)char_list, j, char_list_len, ch2);
				if (ch1 == ch2) {
					U16_BACK_1((UChar *)haystack, 0, i);
					p = (UChar *)haystack + i;
					break;
				}
			}
			if (ch1 == ch2) {
				break;
			}
		}
	} else {
		p = strpbrk((char *)haystack, (char *)char_list);
	}

	if (p) {
		if (haystack_type == IS_UNICODE) {
			RETURN_UNICODEL((UChar *)p, ((UChar *)haystack + haystack_len - (UChar *)p), 1);
		} else {
			RETURN_STRINGL((char *)p, ((char *)haystack + haystack_len - (char *)p), 1);
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int substr_compare(string main_str, string str, int offset [, int length [, bool case_sensitivity]])
   Binary safe optionally case insensitive comparison of 2 strings from an offset, up to length characters */
PHP_FUNCTION(substr_compare)
{
	char *s1, *s2;
	int s1_len, s2_len;
	long offset, len=0;
	zend_bool cs=0;
	uint cmp_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssl|lb", &s1, &s1_len, &s2, &s2_len, &offset, &len, &cs) == FAILURE) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() >= 4 && len <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The length must be greater than zero");
		RETURN_FALSE;
	}

	if (offset < 0) {
		offset = s1_len + offset;
		offset = (offset < 0) ? 0 : offset;
	}

	if ((offset + len) > s1_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The start position cannot exceed initial string length");
		RETURN_FALSE;
	}

	cmp_len = (uint) (len ? len : MAX(s2_len, (s1_len - offset)));

	if (!cs) {
		RETURN_LONG(zend_binary_strncmp(s1 + offset, (s1_len - offset), s2, s2_len, cmp_len));
	} else {
		RETURN_LONG(zend_binary_strncasecmp(s1 + offset, (s1_len - offset), s2, s2_len, cmp_len));
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
