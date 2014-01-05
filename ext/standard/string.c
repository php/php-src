/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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
   |          Stig S�ther Bakken <ssb@php.net>                            |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* Synced with php 3.0 revision 1.193 1999-06-16 [ssb] */

#include <stdio.h>
#include "php.h"
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
/*
 * This define is here because some versions of libintl redefine setlocale
 * to point to libintl_setlocale.  That's a ridiculous thing to do as far
 * as I am concerned, but with this define and the subsequent undef we
 * limit the damage to just the actual setlocale() call in this file
 * without turning zif_setlocale into zif_libintl_setlocale.  -Rasmus
 */
#define php_my_setlocale setlocale
#ifdef HAVE_LIBINTL
# include <libintl.h> /* For LC_MESSAGES */
 #ifdef setlocale
 # undef setlocale
 #endif
#endif

#include "scanf.h"
#include "zend_API.h"
#include "zend_execute.h"
#include "php_globals.h"
#include "basic_functions.h"
#include "php_smart_str.h"
#include <Zend/zend_exceptions.h>
#ifdef ZTS
#include "TSRM.h"
#endif

/* For str_getcsv() support */
#include "ext/standard/file.h"

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

	result = (unsigned char *) safe_emalloc(oldlen, 2 * sizeof(char), 1);

	for (i = j = 0; i < oldlen; i++) {
		result[j++] = hexconvtab[old[i] >> 4];
		result[j++] = hexconvtab[old[i] & 15];
	}
	result[j] = '\0';

	if (newlen)
		*newlen = oldlen * 2 * sizeof(char);

	return (char *)result;
}
/* }}} */

/* {{{ php_hex2bin
 */
static char *php_hex2bin(const unsigned char *old, const size_t oldlen, size_t *newlen)
{
	size_t target_length = oldlen >> 1;
	register unsigned char *str = (unsigned char *)safe_emalloc(target_length, sizeof(char), 1);
	size_t i, j;
	for (i = j = 0; i < target_length; i++) {
		char c = old[j++];
		if (c >= '0' && c <= '9') {
			str[i] = (c - '0') << 4;
		} else if (c >= 'a' && c <= 'f') {
			str[i] = (c - 'a' + 10) << 4;
		} else if (c >= 'A' && c <= 'F') {
			str[i] = (c - 'A' + 10) << 4;
		} else {
			efree(str);
			return NULL;
		}
		c = old[j++];
		if (c >= '0' && c <= '9') {
			str[i] |= c - '0';
		} else if (c >= 'a' && c <= 'f') {
			str[i] |= c - 'a' + 10;
		} else if (c >= 'A' && c <= 'F') {
			str[i] |= c - 'A' + 10;
		} else {
			efree(str);
			return NULL;
		}
	}
	str[target_length] = '\0';

	if (newlen)
		*newlen = target_length;

	return (char *)str;
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

/* {{{ proto string bin2hex(string data)
   Converts the binary representation of data to hex */
PHP_FUNCTION(bin2hex)
{
	char *result, *data;
	size_t newlen;
	int datalen;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &datalen) == FAILURE) {
		return;
	}

	result = php_bin2hex((unsigned char *)data, datalen, &newlen);

	if (!result) {
		RETURN_FALSE;
	}

	RETURN_STRINGL(result, newlen, 0);
}
/* }}} */

/* {{{ proto string hex2bin(string data)
   Converts the hex representation of data to binary */
PHP_FUNCTION(hex2bin)
{
	char *result, *data;
	size_t newlen;
	int datalen;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &datalen) == FAILURE) {
		return;
	}

	if (datalen % 2 != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Hexadecimal input string must have an even length");
		RETURN_FALSE;
	}

	result = php_hex2bin((unsigned char *)data, datalen, &newlen);

	if (!result) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Input string must be hexadecimal string");
		RETURN_FALSE;
	}

	RETURN_STRINGL(result, newlen, 0);
}
/* }}} */

static void php_spn_common_handler(INTERNAL_FUNCTION_PARAMETERS, int behavior) /* {{{ */
{
	char *s11, *s22;
	int len1, len2;
	long start = 0, len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ll", &s11, &len1,
				&s22, &len2, &start, &len) == FAILURE) {
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

	if (len > len1 - start) {
		len = len1 - start;
	}

	if(len == 0) {
		RETURN_LONG(0);
	}

	if (behavior == STR_STRSPN) {
		RETURN_LONG(php_strspn(s11 + start /*str1_start*/,
						s22 /*str2_start*/,
						s11 + start + len /*str1_end*/,
						s22 + len2 /*str2_end*/));
	} else if (behavior == STR_STRCSPN) {
		RETURN_LONG(php_strcspn(s11 + start /*str1_start*/,
						s22 /*str2_start*/,
						s11 + start + len /*str1_end*/,
						s22 + len2 /*str2_end*/));
	}

}
/* }}} */

/* {{{ proto int strspn(string str, string mask [, start [, len]])
   Finds length of initial segment consisting entirely of characters found in mask. If start or/and length is provided works like strspn(substr($s,$start,$len),$good_chars) */
PHP_FUNCTION(strspn)
{
	php_spn_common_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, STR_STRSPN);
}
/* }}} */

/* {{{ proto int strcspn(string str, string mask [, start [, len]])
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
	long item;
	char *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &item) == FAILURE) {
		return;
	}

	switch(item) { /* {{{ */
#ifdef ABDAY_1
		case ABDAY_1:
		case ABDAY_2:
		case ABDAY_3:
		case ABDAY_4:
		case ABDAY_5:
		case ABDAY_6:
		case ABDAY_7:
#endif
#ifdef DAY_1
		case DAY_1:
		case DAY_2:
		case DAY_3:
		case DAY_4:
		case DAY_5:
		case DAY_6:
		case DAY_7:
#endif
#ifdef ABMON_1
		case ABMON_1:
		case ABMON_2:
		case ABMON_3:
		case ABMON_4:
		case ABMON_5:
		case ABMON_6:
		case ABMON_7:
		case ABMON_8:
		case ABMON_9:
		case ABMON_10:
		case ABMON_11:
		case ABMON_12:
#endif
#ifdef MON_1
		case MON_1:
		case MON_2:
		case MON_3:
		case MON_4:
		case MON_5:
		case MON_6:
		case MON_7:
		case MON_8:
		case MON_9:
		case MON_10:
		case MON_11:
		case MON_12:
#endif
#ifdef AM_STR
		case AM_STR:
#endif
#ifdef PM_STR
		case PM_STR:
#endif
#ifdef D_T_FMT
		case D_T_FMT:
#endif
#ifdef D_FMT
		case D_FMT:
#endif
#ifdef T_FMT
		case T_FMT:
#endif
#ifdef T_FMT_AMPM
		case T_FMT_AMPM:
#endif
#ifdef ERA
		case ERA:
#endif
#ifdef ERA_YEAR
		case ERA_YEAR:
#endif
#ifdef ERA_D_T_FMT
		case ERA_D_T_FMT:
#endif
#ifdef ERA_D_FMT
		case ERA_D_FMT:
#endif
#ifdef ERA_T_FMT
		case ERA_T_FMT:
#endif
#ifdef ALT_DIGITS
		case ALT_DIGITS:
#endif
#ifdef INT_CURR_SYMBOL
		case INT_CURR_SYMBOL:
#endif
#ifdef CURRENCY_SYMBOL
		case CURRENCY_SYMBOL:
#endif
#ifdef CRNCYSTR
		case CRNCYSTR:
#endif
#ifdef MON_DECIMAL_POINT
		case MON_DECIMAL_POINT:
#endif
#ifdef MON_THOUSANDS_SEP
		case MON_THOUSANDS_SEP:
#endif
#ifdef MON_GROUPING
		case MON_GROUPING:
#endif
#ifdef POSITIVE_SIGN
		case POSITIVE_SIGN:
#endif
#ifdef NEGATIVE_SIGN
		case NEGATIVE_SIGN:
#endif
#ifdef INT_FRAC_DIGITS
		case INT_FRAC_DIGITS:
#endif
#ifdef FRAC_DIGITS
		case FRAC_DIGITS:
#endif
#ifdef P_CS_PRECEDES
		case P_CS_PRECEDES:
#endif
#ifdef P_SEP_BY_SPACE
		case P_SEP_BY_SPACE:
#endif
#ifdef N_CS_PRECEDES
		case N_CS_PRECEDES:
#endif
#ifdef N_SEP_BY_SPACE
		case N_SEP_BY_SPACE:
#endif
#ifdef P_SIGN_POSN
		case P_SIGN_POSN:
#endif
#ifdef N_SIGN_POSN
		case N_SIGN_POSN:
#endif
#ifdef DECIMAL_POINT
		case DECIMAL_POINT:
#elif defined(RADIXCHAR)
		case RADIXCHAR:
#endif
#ifdef THOUSANDS_SEP
		case THOUSANDS_SEP:
#elif defined(THOUSEP)
		case THOUSEP:
#endif
#ifdef GROUPING
		case GROUPING:
#endif
#ifdef YESEXPR
		case YESEXPR:
#endif
#ifdef NOEXPR
		case NOEXPR:
#endif
#ifdef YESSTR
		case YESSTR:
#endif
#ifdef NOSTR
		case NOSTR:
#endif
#ifdef CODESET
		case CODESET:
#endif
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Item '%ld' is not valid", item);
			RETURN_FALSE;
	}
	/* }}} */

	value = nl_langinfo(item);
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
	char *s1, *s2;
	int s1len, s2len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &s1, &s1len, &s2, &s2len) == FAILURE) {
		return;
	}

	RETURN_LONG(strcoll((const char *) s1,
	                    (const char *) s2));
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
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid '..'-range, no character to the left of '..'");
				result = FAILURE;
				continue;
			}
			if (input+2 >= end) { /* there is no 'right' char */
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid '..'-range, no character to the right of '..'");
				result = FAILURE;
				continue;
			}
			if (input[-1] > input[2]) { /* wrong order */
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid '..'-range, '..'-range needs to be incrementing");
				result = FAILURE;
				continue;
			}
			/* FIXME: better error (a..b..c is the only left possibility?) */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid '..'-range");
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
PHPAPI char *php_trim(char *c, int len, char *what, int what_len, zval *return_value, int mode TSRMLS_DC)
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

/* {{{ php_do_trim
 * Base for trim(), rtrim() and ltrim() functions.
 */
static void php_do_trim(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	char *str;
	char *what = NULL;
	int str_len, what_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &str, &str_len, &what, &what_len) == FAILURE) {
		return;
	}

	php_trim(str, str_len, what, what_len, return_value, mode TSRMLS_CC);
}
/* }}} */

/* {{{ proto string trim(string str [, string character_mask])
   Strips whitespace from the beginning and end of a string */
PHP_FUNCTION(trim)
{
	php_do_trim(INTERNAL_FUNCTION_PARAM_PASSTHRU, 3);
}
/* }}} */

/* {{{ proto string rtrim(string str [, string character_mask])
   Removes trailing whitespace */
PHP_FUNCTION(rtrim)
{
	php_do_trim(INTERNAL_FUNCTION_PARAM_PASSTHRU, 2);
}
/* }}} */

/* {{{ proto string ltrim(string str [, string character_mask])
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
	int textlen, breakcharlen = 1, newtextlen, chk;
	size_t alloced;
	long current = 0, laststart = 0, lastspace = 0;
	long linelength = 75;
	zend_bool docut = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lsb", &text, &textlen, &linelength, &breakchar, &breakcharlen, &docut) == FAILURE) {
		return;
	}

	if (textlen == 0) {
		RETURN_EMPTY_STRING();
	}

	if (breakcharlen == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Break string cannot be empty");
		RETURN_FALSE;
	}

	if (linelength == 0 && docut) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't force cut when width is zero");
		RETURN_FALSE;
	}

	/* Special case for a single-character break as it needs no
	   additional storage space */
	if (breakcharlen == 1 && !docut) {
		newtext = estrndup(text, textlen);

		laststart = lastspace = 0;
		for (current = 0; current < textlen; current++) {
			if (text[current] == breakchar[0]) {
				laststart = lastspace = current + 1;
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
PHPAPI void php_explode(zval *delim, zval *str, zval *return_value, long limit)
{
	char *p1, *p2, *endp;

	endp = Z_STRVAL_P(str) + Z_STRLEN_P(str);

	p1 = Z_STRVAL_P(str);
	p2 = php_memnstr(Z_STRVAL_P(str), Z_STRVAL_P(delim), Z_STRLEN_P(delim), endp);

	if (p2 == NULL) {
		add_next_index_stringl(return_value, p1, Z_STRLEN_P(str), 1);
	} else {
		do {
			add_next_index_stringl(return_value, p1, p2 - p1, 1);
			p1 = p2 + Z_STRLEN_P(delim);
		} while ((p2 = php_memnstr(p1, Z_STRVAL_P(delim), Z_STRLEN_P(delim), endp)) != NULL &&
				 --limit > 1);

		if (p1 <= endp)
			add_next_index_stringl(return_value, p1, endp-p1, 1);
	}
}
/* }}} */

/* {{{ php_explode_negative_limit
 */
PHPAPI void php_explode_negative_limit(zval *delim, zval *str, zval *return_value, long limit)
{
#define EXPLODE_ALLOC_STEP 64
	char *p1, *p2, *endp;

	endp = Z_STRVAL_P(str) + Z_STRLEN_P(str);

	p1 = Z_STRVAL_P(str);
	p2 = php_memnstr(Z_STRVAL_P(str), Z_STRVAL_P(delim), Z_STRLEN_P(delim), endp);

	if (p2 == NULL) {
		/*
		do nothing since limit <= -1, thus if only one chunk - 1 + (limit) <= 0
		by doing nothing we return empty array
		*/
	} else {
		int allocated = EXPLODE_ALLOC_STEP, found = 0;
		long i, to_return;
		char **positions = emalloc(allocated * sizeof(char *));

		positions[found++] = p1;
		do {
			if (found >= allocated) {
				allocated = found + EXPLODE_ALLOC_STEP;/* make sure we have enough memory */
				positions = erealloc(positions, allocated*sizeof(char *));
			}
			positions[found++] = p1 = p2 + Z_STRLEN_P(delim);
		} while ((p2 = php_memnstr(p1, Z_STRVAL_P(delim), Z_STRLEN_P(delim), endp)) != NULL);

		to_return = limit + found;
		/* limit is at least -1 therefore no need of bounds checking : i will be always less than found */
		for (i = 0;i < to_return;i++) { /* this checks also for to_return > 0 */
			add_next_index_stringl(return_value, positions[i],
					(positions[i+1] - Z_STRLEN_P(delim)) - positions[i],
					1
				);
		}
		efree(positions);
	}
#undef EXPLODE_ALLOC_STEP
}
/* }}} */

/* {{{ proto array explode(string separator, string str [, int limit])
   Splits a string on string separator and return array of components. If limit is positive only limit number of components is returned. If limit is negative all components except the last abs(limit) are returned. */
PHP_FUNCTION(explode)
{
	char *str, *delim;
	int str_len = 0, delim_len = 0;
	long limit = LONG_MAX; /* No limit */
	zval zdelim, zstr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", &delim, &delim_len, &str, &str_len, &limit) == FAILURE) {
		return;
	}

	if (delim_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty delimiter");
		RETURN_FALSE;
	}

	array_init(return_value);

	if (str_len == 0) {
	  	if (limit >= 0) {
			add_next_index_stringl(return_value, "", sizeof("") - 1, 1);
		}
		return;
	}

	ZVAL_STRINGL(&zstr, str, str_len, 0);
	ZVAL_STRINGL(&zdelim, delim, delim_len, 0);
	if (limit > 1) {
		php_explode(&zdelim, &zstr, return_value, limit);
	} else if (limit < 0) {
		php_explode_negative_limit(&zdelim, &zstr, return_value, limit);
	} else {
		add_index_stringl(return_value, 0, str, str_len, 1);
	}
}
/* }}} */

/* {{{ proto string join(array src, string glue)
   An alias for implode */
/* }}} */

/* {{{ php_implode
 */
PHPAPI void php_implode(zval *delim, zval *arr, zval *return_value TSRMLS_DC)
{
	zval         **tmp;
	HashPosition   pos;
	smart_str      implstr = {0};
	int            numelems, i = 0;
	zval tmp_val;
	int str_len;

	numelems = zend_hash_num_elements(Z_ARRVAL_P(arr));

	if (numelems == 0) {
		RETURN_EMPTY_STRING();
	}

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(arr), &pos);

	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(arr), (void **) &tmp, &pos) == SUCCESS) {
		switch ((*tmp)->type) {
			case IS_STRING:
				smart_str_appendl(&implstr, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
				break;

			case IS_LONG: {
				char stmp[MAX_LENGTH_OF_LONG + 1];
				str_len = slprintf(stmp, sizeof(stmp), "%ld", Z_LVAL_PP(tmp));
				smart_str_appendl(&implstr, stmp, str_len);
			}
				break;

			case IS_BOOL:
				if (Z_LVAL_PP(tmp) == 1) {
					smart_str_appendl(&implstr, "1", sizeof("1")-1);
				}
				break;

			case IS_NULL:
				break;

			case IS_DOUBLE: {
				char *stmp;
				str_len = spprintf(&stmp, 0, "%.*G", (int) EG(precision), Z_DVAL_PP(tmp));
				smart_str_appendl(&implstr, stmp, str_len);
				efree(stmp);
			}
				break;

			case IS_OBJECT: {
				int copy;
				zval expr;
				zend_make_printable_zval(*tmp, &expr, &copy);
				smart_str_appendl(&implstr, Z_STRVAL(expr), Z_STRLEN(expr));
				if (copy) {
					zval_dtor(&expr);
				}
			}
				break;

			default:
				tmp_val = **tmp;
				zval_copy_ctor(&tmp_val);
				convert_to_string(&tmp_val);
				smart_str_appendl(&implstr, Z_STRVAL(tmp_val), Z_STRLEN(tmp_val));
				zval_dtor(&tmp_val);
				break;

		}

		if (++i != numelems) {
			smart_str_appendl(&implstr, Z_STRVAL_P(delim), Z_STRLEN_P(delim));
		}
		zend_hash_move_forward_ex(Z_ARRVAL_P(arr), &pos);
	}
	smart_str_0(&implstr);

	if (implstr.len) {
		RETURN_STRINGL(implstr.c, implstr.len, 0);
	} else {
		smart_str_free(&implstr);
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto string implode([string glue,] array pieces)
   Joins array elements placing glue string between items and return one string */
PHP_FUNCTION(implode)
{
	zval **arg1 = NULL, **arg2 = NULL, *delim, *arr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|Z", &arg1, &arg2) == FAILURE) {
		return;
	}

	if (arg2 == NULL) {
		if (Z_TYPE_PP(arg1) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument must be an array");
			return;
		}

		MAKE_STD_ZVAL(delim);
#define _IMPL_EMPTY ""
		ZVAL_STRINGL(delim, _IMPL_EMPTY, sizeof(_IMPL_EMPTY) - 1, 0);

		SEPARATE_ZVAL(arg1);
		arr = *arg1;
	} else {
		if (Z_TYPE_PP(arg1) == IS_ARRAY) {
			arr = *arg1;
			convert_to_string_ex(arg2);
			delim = *arg2;
		} else if (Z_TYPE_PP(arg2) == IS_ARRAY) {
			arr = *arg2;
			convert_to_string_ex(arg1);
			delim = *arg1;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid arguments passed");
			return;
		}
	}

	php_implode(delim, arr, return_value TSRMLS_CC);

	if (arg2 == NULL) {
		FREE_ZVAL(delim);
	}
}
/* }}} */

#define STRTOK_TABLE(p) BG(strtok_table)[(unsigned char) *p]

/* {{{ proto string strtok([string str,] string token)
   Tokenize a string */
PHP_FUNCTION(strtok)
{
	char *str, *tok = NULL;
	int str_len, tok_len = 0;
	zval *zv;

	char *token;
	char *token_end;
	char *p;
	char *pe;
	int skipped = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &str, &str_len, &tok, &tok_len) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() == 1) {
		tok = str;
		tok_len = str_len;
	} else {
		if (BG(strtok_zval)) {
			zval_ptr_dtor(&BG(strtok_zval));
		}
		MAKE_STD_ZVAL(zv);
		ZVAL_STRINGL(zv, str, str_len, 1);

		BG(strtok_zval) = zv;
		BG(strtok_last) = BG(strtok_string) = Z_STRVAL_P(zv);
		BG(strtok_len) = str_len;
	}

	p = BG(strtok_last); /* Where we start to search */
	pe = BG(strtok_string) + BG(strtok_len);

	if (!p || p >= pe) {
		RETURN_FALSE;
	}

	token = tok;
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

	if (p - BG(strtok_last)) {
return_token:
		RETVAL_STRINGL(BG(strtok_last) + skipped, (p - BG(strtok_last)) - skipped, 1);
		BG(strtok_last) = p + 1;
	} else {
		RETVAL_FALSE;
		BG(strtok_last) = NULL;
	}

	/* Restore table -- usually faster then memset'ing the table on every invocation */
restore:
	token = tok;

	while (token < token_end) {
		STRTOK_TABLE(token++) = 0;
	}
}
/* }}} */

/* {{{ php_strtoupper
 */
PHPAPI char *php_strtoupper(char *s, size_t len)
{
	unsigned char *c, *e;

	c = (unsigned char *)s;
	e = (unsigned char *)c+len;

	while (c < e) {
		*c = toupper(*c);
		c++;
	}
	return s;
}
/* }}} */

/* {{{ proto string strtoupper(string str)
   Makes a string uppercase */
PHP_FUNCTION(strtoupper)
{
	char *arg;
	int arglen;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arglen) == FAILURE) {
		return;
	}

	arg = estrndup(arg, arglen);
	php_strtoupper(arg, arglen);
	RETURN_STRINGL(arg, arglen, 0);
}
/* }}} */

/* {{{ php_strtolower
 */
PHPAPI char *php_strtolower(char *s, size_t len)
{
	unsigned char *c, *e;

	c = (unsigned char *)s;
	e = c+len;

	while (c < e) {
		*c = tolower(*c);
		c++;
	}
	return s;
}
/* }}} */

/* {{{ proto string strtolower(string str)
   Makes a string lowercase */
PHP_FUNCTION(strtolower)
{
	char *str;
	int arglen;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &arglen) == FAILURE) {
		return;
	}

	str = estrndup(str, arglen);
	php_strtolower(str, arglen);
	RETURN_STRINGL(str, arglen, 0);
}
/* }}} */

/* {{{ php_basename
 */
PHPAPI void php_basename(const char *s, size_t len, char *suffix, size_t sufflen, char **p_ret, size_t *p_len TSRMLS_DC)
{
	char *ret = NULL, *c, *comp, *cend;
	size_t inc_len, cnt;
	int state;

	c = comp = cend = (char*)s;
	cnt = len;
	state = 0;
	while (cnt > 0) {
		inc_len = (*c == '\0' ? 1: php_mblen(c, cnt));

		switch (inc_len) {
			case -2:
			case -1:
				inc_len = 1;
				php_ignore_value(php_mblen(NULL, 0));
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
#if defined(PHP_WIN32) || defined(NETWARE)
				/* Catch relative paths in c:file.txt style. They're not to confuse
				   with the NTFS streams. This part ensures also, that no drive 
				   letter traversing happens. */
				} else if ((*c == ':' && (c - comp == 1))) {
					if (state == 0) {
						comp = c;
						state = 1;
					} else {
						cend = c;
						state = 0;
					}
#endif
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
	if (suffix != NULL && sufflen < (uint)(cend - comp) &&
			memcmp(cend - sufflen, suffix, sufflen) == 0) {
		cend -= sufflen;
	}

	len = cend - comp;

	if (p_ret) {
		ret = emalloc(len + 1);
		memcpy(ret, comp, len);
		ret[len] = '\0';
		*p_ret = ret;
	}
	if (p_len) {
		*p_len = len;
	}
}
/* }}} */

/* {{{ proto string basename(string path [, string suffix])
   Returns the filename component of the path */
PHP_FUNCTION(basename)
{
	char *string, *suffix = NULL, *ret;
	int   string_len, suffix_len = 0;
	size_t ret_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &string, &string_len, &suffix, &suffix_len) == FAILURE) {
		return;
	}

	php_basename(string, string_len, suffix, suffix_len, &ret, &ret_len TSRMLS_CC);
	RETURN_STRINGL(ret, (int)ret_len, 0);
}
/* }}} */

/* {{{ php_dirname
   Returns directory name component of path */
PHPAPI size_t php_dirname(char *path, size_t len)
{
	return zend_dirname(path, len);
}
/* }}} */

/* {{{ proto string dirname(string path)
   Returns the directory name component of the path */
PHP_FUNCTION(dirname)
{
	char *str;
	char *ret;
	int str_len;
	size_t ret_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	ret = estrndup(str, str_len);
	ret_len = php_dirname(ret, str_len);

	RETURN_STRINGL(ret, ret_len, 0);
}
/* }}} */

/* {{{ proto array pathinfo(string path[, int options])
   Returns information about a certain string */
PHP_FUNCTION(pathinfo)
{
	zval *tmp;
	char *path, *ret = NULL;
	int path_len, have_basename;
	size_t ret_len;
	long opt = PHP_PATHINFO_ALL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &path, &path_len, &opt) == FAILURE) {
		return;
	}

	have_basename = ((opt & PHP_PATHINFO_BASENAME) == PHP_PATHINFO_BASENAME);

	MAKE_STD_ZVAL(tmp);
	array_init(tmp);

	if ((opt & PHP_PATHINFO_DIRNAME) == PHP_PATHINFO_DIRNAME) {
		ret = estrndup(path, path_len);
		php_dirname(ret, path_len);
		if (*ret) {
			add_assoc_string(tmp, "dirname", ret, 1);
		}
		efree(ret);
		ret = NULL;
	}

	if (have_basename) {
		php_basename(path, path_len, NULL, 0, &ret, &ret_len TSRMLS_CC);
		add_assoc_stringl(tmp, "basename", ret, ret_len, 0);
	}

	if ((opt & PHP_PATHINFO_EXTENSION) == PHP_PATHINFO_EXTENSION) {
		const char *p;
		int idx;

		if (!have_basename) {
			php_basename(path, path_len, NULL, 0, &ret, &ret_len TSRMLS_CC);
		}

		p = zend_memrchr(ret, '.', ret_len);

		if (p) {
			idx = p - ret;
			add_assoc_stringl(tmp, "extension", ret + idx + 1, ret_len - idx - 1, 1);
		}
	}

	if ((opt & PHP_PATHINFO_FILENAME) == PHP_PATHINFO_FILENAME) {
		const char *p;
		int idx;

		/* Have we already looked up the basename? */
		if (!have_basename && !ret) {
			php_basename(path, path_len, NULL, 0, &ret, &ret_len TSRMLS_CC);
		}

		p = zend_memrchr(ret, '.', ret_len);

		idx = p ? (p - ret) : ret_len;
		add_assoc_stringl(tmp, "filename", ret, idx, 1);
	}

	if (!have_basename && ret) {
		efree(ret);
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

/* {{{ php_stristr
   case insensitve strstr */
PHPAPI char *php_stristr(char *s, char *t, size_t s_len, size_t t_len)
{
	php_strtolower(s, s_len);
	php_strtolower(t, t_len);
	return php_memnstr(s, t, t_len, s + s_len);
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
		} while (spanp++ < (s2_end - 1));
		c = *++p;
	}
	/* NOTREACHED */
}
/* }}} */

/* {{{ php_needle_char
 */
static int php_needle_char(zval *needle, char *target TSRMLS_DC)
{
	switch (Z_TYPE_P(needle)) {
		case IS_LONG:
		case IS_BOOL:
			*target = (char)Z_LVAL_P(needle);
			return SUCCESS;
		case IS_NULL:
			*target = '\0';
			return SUCCESS;
		case IS_DOUBLE:
			*target = (char)(int)Z_DVAL_P(needle);
			return SUCCESS;
		case IS_OBJECT:
			{
				zval holder = *needle;
				zval_copy_ctor(&(holder));
				convert_to_long(&(holder));
				if(Z_TYPE(holder) != IS_LONG) {
					return FAILURE;
				}
				*target = (char)Z_LVAL(holder);
				return SUCCESS;
			}
		default: {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "needle is not a string or an integer");
			return FAILURE;
		 }
	}
}
/* }}} */

/* {{{ proto string stristr(string haystack, string needle[, bool part])
   Finds first occurrence of a string within another, case insensitive */
PHP_FUNCTION(stristr)
{
	zval *needle;
	char *haystack;
	int haystack_len;
	char *found = NULL;
	int  found_offset;
	char *haystack_dup;
	char needle_char[2];
	zend_bool part = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|b", &haystack, &haystack_len, &needle, &part) == FAILURE) {
		return;
	}

	haystack_dup = estrndup(haystack, haystack_len);

	if (Z_TYPE_P(needle) == IS_STRING) {
		char *orig_needle;
		if (!Z_STRLEN_P(needle)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty needle");
			efree(haystack_dup);
			RETURN_FALSE;
		}
		orig_needle = estrndup(Z_STRVAL_P(needle), Z_STRLEN_P(needle));
		found = php_stristr(haystack_dup, orig_needle,	haystack_len, Z_STRLEN_P(needle));
		efree(orig_needle);
	} else {
		if (php_needle_char(needle, needle_char TSRMLS_CC) != SUCCESS) {
			efree(haystack_dup);
			RETURN_FALSE;
		}
		needle_char[1] = 0;

		found = php_stristr(haystack_dup, needle_char,	haystack_len, 1);
	}

	if (found) {
		found_offset = found - haystack_dup;
		if (part) {
			RETVAL_STRINGL(haystack, found_offset, 1);
		} else {
			RETVAL_STRINGL(haystack + found_offset, haystack_len - found_offset, 1);
		}
	} else {
		RETVAL_FALSE;
	}

	efree(haystack_dup);
}
/* }}} */

/* {{{ proto string strstr(string haystack, string needle[, bool part])
   Finds first occurrence of a string within another */
PHP_FUNCTION(strstr)
{
	zval *needle;
	char *haystack;
	int haystack_len;
	char *found = NULL;
	char needle_char[2];
	long found_offset;
	zend_bool part = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|b", &haystack, &haystack_len, &needle, &part) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(needle) == IS_STRING) {
		if (!Z_STRLEN_P(needle)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty needle");
			RETURN_FALSE;
		}

		found = php_memnstr(haystack, Z_STRVAL_P(needle), Z_STRLEN_P(needle), haystack + haystack_len);
	} else {
		if (php_needle_char(needle, needle_char TSRMLS_CC) != SUCCESS) {
			RETURN_FALSE;
		}
		needle_char[1] = 0;

		found = php_memnstr(haystack, needle_char,	1, haystack + haystack_len);
	}

	if (found) {
		found_offset = found - haystack;
		if (part) {
			RETURN_STRINGL(haystack, found_offset, 1);
		} else {
			RETURN_STRINGL(found, haystack_len - found_offset, 1);
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string strchr(string haystack, string needle)
   An alias for strstr */
/* }}} */

/* {{{ proto int strpos(string haystack, string needle [, int offset])
   Finds position of first occurrence of a string within another */
PHP_FUNCTION(strpos)
{
	zval *needle;
	char *haystack;
	char *found = NULL;
	char  needle_char[2];
	long  offset = 0;
	int   haystack_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|l", &haystack, &haystack_len, &needle, &offset) == FAILURE) {
		return;
	}

	if (offset < 0 || offset > haystack_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset not contained in string");
		RETURN_FALSE;
	}

	if (Z_TYPE_P(needle) == IS_STRING) {
		if (!Z_STRLEN_P(needle)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty needle");
			RETURN_FALSE;
		}

		found = php_memnstr(haystack + offset,
			                Z_STRVAL_P(needle),
			                Z_STRLEN_P(needle),
			                haystack + haystack_len);
	} else {
		if (php_needle_char(needle, needle_char TSRMLS_CC) != SUCCESS) {
			RETURN_FALSE;
		}
		needle_char[1] = 0;

		found = php_memnstr(haystack + offset,
							needle_char,
							1,
		                    haystack + haystack_len);
	}

	if (found) {
		RETURN_LONG(found - haystack);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int stripos(string haystack, string needle [, int offset])
   Finds position of first occurrence of a string within another, case insensitive */
PHP_FUNCTION(stripos)
{
	char *found = NULL;
	char *haystack;
	int haystack_len;
	long offset = 0;
	char *needle_dup = NULL, *haystack_dup;
	char needle_char[2];
	zval *needle;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|l", &haystack, &haystack_len, &needle, &offset) == FAILURE) {
		return;
	}

	if (offset < 0 || offset > haystack_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset not contained in string");
		RETURN_FALSE;
	}

	if (haystack_len == 0) {
		RETURN_FALSE;
	}

	haystack_dup = estrndup(haystack, haystack_len);
	php_strtolower(haystack_dup, haystack_len);

	if (Z_TYPE_P(needle) == IS_STRING) {
		if (Z_STRLEN_P(needle) == 0 || Z_STRLEN_P(needle) > haystack_len) {
			efree(haystack_dup);
			RETURN_FALSE;
		}

		needle_dup = estrndup(Z_STRVAL_P(needle), Z_STRLEN_P(needle));
		php_strtolower(needle_dup, Z_STRLEN_P(needle));
		found = php_memnstr(haystack_dup + offset, needle_dup, Z_STRLEN_P(needle), haystack_dup + haystack_len);
	} else {
		if (php_needle_char(needle, needle_char TSRMLS_CC) != SUCCESS) {
			efree(haystack_dup);
			RETURN_FALSE;
		}
		needle_char[0] = tolower(needle_char[0]);
		needle_char[1] = '\0';
		found = php_memnstr(haystack_dup + offset,
							needle_char,
							sizeof(needle_char) - 1,
							haystack_dup + haystack_len);
	}

	efree(haystack_dup);
	if (needle_dup) {
		efree(needle_dup);
	}

	if (found) {
		RETURN_LONG(found - haystack_dup);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int strrpos(string haystack, string needle [, int offset])
   Finds position of last occurrence of a string within another string */
PHP_FUNCTION(strrpos)
{
	zval *zneedle;
	char *needle, *haystack;
	int needle_len, haystack_len;
	long offset = 0;
	char *p, *e, ord_needle[2];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz|l", &haystack, &haystack_len, &zneedle, &offset) == FAILURE) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(zneedle) == IS_STRING) {
		needle = Z_STRVAL_P(zneedle);
		needle_len = Z_STRLEN_P(zneedle);
	} else {
		if (php_needle_char(zneedle, ord_needle TSRMLS_CC) != SUCCESS) {
			RETURN_FALSE;
		}
		ord_needle[1] = '\0';
		needle = ord_needle;
		needle_len = 1;
	}

	if ((haystack_len == 0) || (needle_len == 0)) {
		RETURN_FALSE;
	}

	if (offset >= 0) {
		if (offset > haystack_len) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset is greater than the length of haystack string");
			RETURN_FALSE;
		}
		p = haystack + offset;
		e = haystack + haystack_len - needle_len;
	} else {
		if (offset < -INT_MAX || -offset > haystack_len) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset is greater than the length of haystack string");
			RETURN_FALSE;
		}

		p = haystack;
		if (needle_len > -offset) {
			e = haystack + haystack_len - needle_len;
		} else {
			e = haystack + haystack_len + offset;
		}
	}

	if (needle_len == 1) {
		/* Single character search can shortcut memcmps */
		while (e >= p) {
			if (*e == *needle) {
				RETURN_LONG(e - p + (offset > 0 ? offset : 0));
			}
			e--;
		}
		RETURN_FALSE;
	}

	while (e >= p) {
		if (memcmp(e, needle, needle_len) == 0) {
			RETURN_LONG(e - p + (offset > 0 ? offset : 0));
		}
		e--;
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
		if (php_needle_char(zneedle, ord_needle TSRMLS_CC) != SUCCESS) {
			RETURN_FALSE;
		}
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
			if (offset > haystack_len) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset is greater than the length of haystack string");
				RETURN_FALSE;
			}
			p = haystack + offset;
			e = haystack + haystack_len - 1;
		} else {
			p = haystack;
			if (offset < -INT_MAX || -offset > haystack_len) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset is greater than the length of haystack string");
				RETURN_FALSE;
			}
			e = haystack + haystack_len + offset;
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
		if (offset > haystack_len) {
			efree(needle_dup);
			efree(haystack_dup);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset is greater than the length of haystack string");
			RETURN_FALSE;
		}
		p = haystack_dup + offset;
		e = haystack_dup + haystack_len - needle_len;
	} else {
		if (offset < -INT_MAX || -offset > haystack_len) {
			efree(needle_dup);
			efree(haystack_dup);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset is greater than the length of haystack string");
			RETURN_FALSE;
		}
		p = haystack_dup;
		if (needle_len > -offset) {
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

/* {{{ proto string strrchr(string haystack, string needle)
   Finds the last occurrence of a character in a string within another */
PHP_FUNCTION(strrchr)
{
	zval *needle;
	char *haystack;
	const char *found = NULL;
	long found_offset;
	int  haystack_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &haystack, &haystack_len, &needle) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(needle) == IS_STRING) {
		found = zend_memrchr(haystack, *Z_STRVAL_P(needle), haystack_len);
	} else {
		char needle_chr;
		if (php_needle_char(needle, &needle_chr TSRMLS_CC) != SUCCESS) {
			RETURN_FALSE;
		}

		found = zend_memrchr(haystack,  needle_chr, haystack_len);
	}

	if (found) {
		found_offset = found - haystack;
		RETURN_STRINGL(found, haystack_len - found_offset, 1);
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
	int out_len;

	chunks = srclen / chunklen;
	restlen = srclen - chunks * chunklen; /* srclen % chunklen */

	if(chunks > INT_MAX - 1) {
		return NULL;
	}
	out_len = chunks + 1;
	if(endlen !=0 && out_len > INT_MAX/endlen) {
		return NULL;
	}
	out_len *= endlen;
	if(out_len > INT_MAX - srclen - 1) {
		return NULL;
	}
	out_len += srclen + 1;

	dest = safe_emalloc((int)out_len, sizeof(char), 0);

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
	char *str;
	char *result;
	char *end    = "\r\n";
	int endlen   = 2;
	long chunklen = 76;
	int result_len;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls", &str, &str_len, &chunklen, &end, &endlen) == FAILURE) {
		return;
	}

	if (chunklen <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Chunk length should be greater than zero");
		RETURN_FALSE;
	}

	if (chunklen > str_len) {
		/* to maintain BC, we must return original string + ending */
		result_len = endlen + str_len;
		result = emalloc(result_len + 1);
		memcpy(result, str, str_len);
		memcpy(result + str_len, end, endlen);
		result[result_len] = '\0';
		RETURN_STRINGL(result, result_len, 0);
	}

	if (!str_len) {
		RETURN_EMPTY_STRING();
	}

	result = php_chunk_split(str, str_len, end, endlen, chunklen, &result_len);

	if (result) {
		RETURN_STRINGL(result, result_len, 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string substr(string str, int start [, int length])
   Returns part of a string */
PHP_FUNCTION(substr)
{
	char *str;
	long l = 0, f;
	int str_len;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|l", &str, &str_len, &f, &l) == FAILURE) {
		return;
	}

	if (argc > 2) {
		if ((l < 0 && -l > str_len)) {
			RETURN_FALSE;
		} else if (l > str_len) {
			l = str_len;
		}
	} else {
		l = str_len;
	}

	if (f > str_len) {
		RETURN_FALSE;
	} else if (f < 0 && -f > str_len) {
		f = 0;
	}

	if (l < 0 && (l + str_len - f) < 0) {
		RETURN_FALSE;
	}

	/* if "from" position is negative, count start position from the end
	 * of the string
	 */
	if (f < 0) {
		f = str_len + f;
		if (f < 0) {
			f = 0;
		}
	}

	/* if "length" position is negative, set it to the length
	 * needed to stop that many chars from the end of the string
	 */
	if (l < 0) {
		l = (str_len - f) + l;
		if (l < 0) {
			l = 0;
		}
	}

	if (f >= str_len) {
		RETURN_FALSE;
	}

	if ((f + l) > str_len) {
		l = str_len - f;
	}

	RETURN_STRINGL(str + f, l, 1);
}
/* }}} */

/* {{{ proto mixed substr_replace(mixed str, mixed repl, mixed start [, mixed length])
   Replaces part of a string with another string */
PHP_FUNCTION(substr_replace)
{
	zval **str;
	zval **from;
	zval **len = NULL;
	zval **repl;
	char *result;
	int result_len;
	int l = 0;
	int f;
	int argc = ZEND_NUM_ARGS();

	HashPosition pos_str, pos_from, pos_repl, pos_len;
	zval **tmp_str = NULL, **tmp_from = NULL, **tmp_repl = NULL, **tmp_len= NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZZ|Z", &str, &repl, &from, &len) == FAILURE) {
		return;
	}

	if (Z_TYPE_PP(str) != IS_ARRAY) {
		if (Z_ISREF_PP(str)) {
			SEPARATE_ZVAL(str);
		}
		convert_to_string_ex(str);
	}
	if (Z_TYPE_PP(repl) != IS_ARRAY) {
		if (Z_ISREF_PP(repl)) {
			SEPARATE_ZVAL(repl);
		}
		convert_to_string_ex(repl);
	}
	if (Z_TYPE_PP(from) != IS_ARRAY) {
		if (Z_ISREF_PP(from)) {
			SEPARATE_ZVAL(from);
		}
		convert_to_long_ex(from);
	}

	if (argc > 3) {
		SEPARATE_ZVAL(len);
		if (Z_TYPE_PP(len) != IS_ARRAY) {
			convert_to_long_ex(len);
			l = Z_LVAL_PP(len);
		}
	} else {
		if (Z_TYPE_PP(str) != IS_ARRAY) {
			l = Z_STRLEN_PP(str);
		}
	}

	if (Z_TYPE_PP(str) == IS_STRING) {
		if (
			(argc == 3 && Z_TYPE_PP(from) == IS_ARRAY) ||
			(argc == 4 && Z_TYPE_PP(from) != Z_TYPE_PP(len))
		) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "'from' and 'len' should be of same type - numerical or array ");
			RETURN_STRINGL(Z_STRVAL_PP(str), Z_STRLEN_PP(str), 1);
		}
		if (argc == 4 && Z_TYPE_PP(from) == IS_ARRAY) {
			if (zend_hash_num_elements(Z_ARRVAL_PP(from)) != zend_hash_num_elements(Z_ARRVAL_PP(len))) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "'from' and 'len' should have the same number of elements");
				RETURN_STRINGL(Z_STRVAL_PP(str), Z_STRLEN_PP(str), 1);
			}
		}
	}

	if (Z_TYPE_PP(str) != IS_ARRAY) {
		if (Z_TYPE_PP(from) != IS_ARRAY) {
			int repl_len = 0;

			f = Z_LVAL_PP(from);

			/* if "from" position is negative, count start position from the end
			 * of the string
			 */
			if (f < 0) {
				f = Z_STRLEN_PP(str) + f;
				if (f < 0) {
					f = 0;
				}
			} else if (f > Z_STRLEN_PP(str)) {
				f = Z_STRLEN_PP(str);
			}
			/* if "length" position is negative, set it to the length
			 * needed to stop that many chars from the end of the string
			 */
			if (l < 0) {
				l = (Z_STRLEN_PP(str) - f) + l;
				if (l < 0) {
					l = 0;
				}
			}

			if (f > Z_STRLEN_PP(str) || (f < 0 && -f > Z_STRLEN_PP(str))) {
				RETURN_FALSE;
			} else if (l > Z_STRLEN_PP(str) || (l < 0 && -l > Z_STRLEN_PP(str))) {
				l = Z_STRLEN_PP(str);
			}

			if ((f + l) > Z_STRLEN_PP(str)) {
				l = Z_STRLEN_PP(str) - f;
			}
			if (Z_TYPE_PP(repl) == IS_ARRAY) {
				zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(repl), &pos_repl);
				if (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_PP(repl), (void **) &tmp_repl, &pos_repl)) {
					convert_to_string_ex(tmp_repl);
					repl_len = Z_STRLEN_PP(tmp_repl);
				}
			} else {
				repl_len = Z_STRLEN_PP(repl);
			}
			result_len = Z_STRLEN_PP(str) - l + repl_len;
			result = emalloc(result_len + 1);

			memcpy(result, Z_STRVAL_PP(str), f);
			if (repl_len) {
				memcpy((result + f), (Z_TYPE_PP(repl) == IS_ARRAY ? Z_STRVAL_PP(tmp_repl) : Z_STRVAL_PP(repl)), repl_len);
			}
			memcpy((result + f + repl_len), Z_STRVAL_PP(str) + f + l, Z_STRLEN_PP(str) - f - l);
			result[result_len] = '\0';
			RETURN_STRINGL(result, result_len, 0);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Functionality of 'from' and 'len' as arrays is not implemented");
			RETURN_STRINGL(Z_STRVAL_PP(str), Z_STRLEN_PP(str), 1);
		}
	} else { /* str is array of strings */
		char *str_index = NULL;
		uint str_index_len;
		ulong num_index;

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
			zval *orig_str;
			zval dummy;
			ulong refcount;
			int was_ref;

			if(Z_TYPE_PP(tmp_str) != IS_STRING) {
				dummy = **tmp_str;
				orig_str = &dummy;
				zval_copy_ctor(orig_str);
				convert_to_string(orig_str);
			} else {
				orig_str = *tmp_str;
			}
			was_ref = Z_ISREF_P(orig_str);
			Z_UNSET_ISREF_P(orig_str);
			refcount = Z_REFCOUNT_P(orig_str);

			if (Z_TYPE_PP(from) == IS_ARRAY) {
				if (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_PP(from), (void **) &tmp_from, &pos_from)) {
					if(Z_TYPE_PP(tmp_from) != IS_LONG) {
						zval dummy = **tmp_from;
						zval_copy_ctor(&dummy);
						convert_to_long(&dummy);
						f = Z_LVAL(dummy);
					} else {
						f = Z_LVAL_PP(tmp_from);
					}

					if (f < 0) {
						f = Z_STRLEN_P(orig_str) + f;
						if (f < 0) {
							f = 0;
						}
					} else if (f > Z_STRLEN_P(orig_str)) {
						f = Z_STRLEN_P(orig_str);
					}
					zend_hash_move_forward_ex(Z_ARRVAL_PP(from), &pos_from);
				} else {
					f = 0;
				}
			} else {
				f = Z_LVAL_PP(from);
				if (f < 0) {
					f = Z_STRLEN_P(orig_str) + f;
					if (f < 0) {
						f = 0;
					}
				} else if (f > Z_STRLEN_P(orig_str)) {
					f = Z_STRLEN_P(orig_str);
				}
			}

			if (argc > 3 && Z_TYPE_PP(len) == IS_ARRAY) {
				if (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_PP(len), (void **) &tmp_len, &pos_len)) {
					if(Z_TYPE_PP(tmp_len) != IS_LONG) {
						zval dummy = **tmp_len;
						zval_copy_ctor(&dummy);
						convert_to_long(&dummy);
						l = Z_LVAL(dummy);
					} else {
						l = Z_LVAL_PP(tmp_len);
					}
					zend_hash_move_forward_ex(Z_ARRVAL_PP(len), &pos_len);
				} else {
					l = Z_STRLEN_P(orig_str);
				}
			} else if (argc > 3) {
				l = Z_LVAL_PP(len);
			} else {
				l = Z_STRLEN_P(orig_str);
			}

			if (l < 0) {
				l = (Z_STRLEN_P(orig_str) - f) + l;
				if (l < 0) {
					l = 0;
				}
			}

			if ((f + l) > Z_STRLEN_P(orig_str)) {
				l = Z_STRLEN_P(orig_str) - f;
			}

			result_len = Z_STRLEN_P(orig_str) - l;

			if (Z_TYPE_PP(repl) == IS_ARRAY) {
				if (SUCCESS == zend_hash_get_current_data_ex(Z_ARRVAL_PP(repl), (void **) &tmp_repl, &pos_repl)) {
					zval *repl_str;
					zval zrepl;
					if(Z_TYPE_PP(tmp_repl) != IS_STRING) {
						zrepl = **tmp_repl;
						repl_str = &zrepl;
						zval_copy_ctor(repl_str);
						convert_to_string(repl_str);
					} else {
						repl_str = *tmp_repl;
					}

					if(Z_REFCOUNT_P(orig_str) != refcount) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument was modified while replacing");
						if(Z_TYPE_PP(tmp_repl) != IS_STRING) {
							zval_dtor(repl_str);
						}
						break;
					}

					result_len += Z_STRLEN_P(repl_str);
					zend_hash_move_forward_ex(Z_ARRVAL_PP(repl), &pos_repl);
					result = emalloc(result_len + 1);

					memcpy(result, Z_STRVAL_P(orig_str), f);
					memcpy((result + f), Z_STRVAL_P(repl_str), Z_STRLEN_P(repl_str));
					memcpy((result + f + Z_STRLEN_P(repl_str)), Z_STRVAL_P(orig_str) + f + l, Z_STRLEN_P(orig_str) - f - l);
					if(Z_TYPE_PP(tmp_repl) != IS_STRING) {
						zval_dtor(repl_str);
					}
				} else {
					result = emalloc(result_len + 1);

					memcpy(result, Z_STRVAL_P(orig_str), f);
					memcpy((result + f), Z_STRVAL_P(orig_str) + f + l, Z_STRLEN_P(orig_str) - f - l);
				}
			} else {
				result_len += Z_STRLEN_PP(repl);

				result = emalloc(result_len + 1);

				memcpy(result, Z_STRVAL_P(orig_str), f);
				memcpy((result + f), Z_STRVAL_PP(repl), Z_STRLEN_PP(repl));
				memcpy((result + f + Z_STRLEN_PP(repl)), Z_STRVAL_P(orig_str) + f + l, Z_STRLEN_P(orig_str) - f - l);
			}

			result[result_len] = '\0';

			if (zend_hash_get_current_key_ex(Z_ARRVAL_PP(str), &str_index, &str_index_len, &num_index, 0, &pos_str) == HASH_KEY_IS_STRING) {
				add_assoc_stringl_ex(return_value, str_index, str_index_len, result, result_len, 0);
			} else {
				add_index_stringl(return_value, num_index, result, result_len, 0);
			}

			if(Z_TYPE_PP(tmp_str) != IS_STRING) {
				zval_dtor(orig_str);
			} else {
				Z_SET_ISREF_TO_P(orig_str, was_ref);
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
	char *str, *old;
	char *old_end;
	char *p, *q;
	char c;
	int  old_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &old, &old_len) == FAILURE) {
		return;
	}

	old_end = old + old_len;

	if (old == old_end) {
		RETURN_FALSE;
	}

	str = safe_emalloc(2, old_len, 1);

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

/* {{{ proto int ord(string character)
   Returns ASCII value of character */
PHP_FUNCTION(ord)
{
	char *str;
	int   str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	RETURN_LONG((unsigned char) str[0]);
}
/* }}} */

/* {{{ proto string chr(int ascii)
   Converts ASCII code to a character */
PHP_FUNCTION(chr)
{
	long c;
	char temp[2];

	if (ZEND_NUM_ARGS() != 1) {
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "l", &c) == FAILURE) {
		c = 0;
	}

	temp[0] = (char)c;
	temp[1] = '\0';

	RETURN_STRINGL(temp, 1, 1);
}
/* }}} */

/* {{{ php_ucfirst
   Uppercase the first character of the word in a native string */
static void php_ucfirst(char *str)
{
	register char *r;
	r = str;
	*r = toupper((unsigned char) *r);
}
/* }}} */

/* {{{ proto string ucfirst(string str)
   Makes a string's first character uppercase */
PHP_FUNCTION(ucfirst)
{
	char *str;
	int  str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	if (!str_len) {
		RETURN_EMPTY_STRING();
	}

	ZVAL_STRINGL(return_value, str, str_len, 1);
	php_ucfirst(Z_STRVAL_P(return_value));
}
/* }}} */

/* {{{
   Lowercase the first character of the word in a native string */
static void php_lcfirst(char *str)
{
	register char *r;
	r = str;
	*r = tolower((unsigned char) *r);
}
/* }}} */

/* {{{ proto string lcfirst(string str)
   Make a string's first character lowercase */
PHP_FUNCTION(lcfirst)
{
	char  *str;
	int   str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	if (!str_len) {
		RETURN_EMPTY_STRING();
	}

	ZVAL_STRINGL(return_value, str, str_len, 1);
	php_lcfirst(Z_STRVAL_P(return_value));
}
/* }}} */

/* {{{ proto string ucwords(string str)
   Uppercase the first character of every word in a string */
PHP_FUNCTION(ucwords)
{
	char *str;
	register char *r, *r_end;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	if (!str_len) {
		RETURN_EMPTY_STRING();
	}

	ZVAL_STRINGL(return_value, str, str_len, 1);
	r = Z_STRVAL_P(return_value);

	*r = toupper((unsigned char) *r);
	for (r_end = r + Z_STRLEN_P(return_value) - 1; r < r_end; ) {
		if (isspace((int) *(unsigned char *)r++)) {
			*r = toupper((unsigned char) *r);
		}
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

/* {{{ Definitions for php_strtr_array */
typedef size_t STRLEN;	/* STRLEN should be unsigned */
typedef uint16_t HASH;
typedef struct {
	HASH			table_mask;
	STRLEN			entries[1];
} SHIFT_TAB;
typedef struct {
	HASH			table_mask;
	int				entries[1];
} HASH_TAB;
typedef struct {
	const char	*s;
	STRLEN		l;
} STR;
typedef struct _pat_and_repl {
	STR			pat;
	STR			repl;
} PATNREPL;

#define S(a) ((a)->s)
#define L(a) ((a)->l)

#define SHIFT_TAB_BITS	13
#define HASH_TAB_BITS	10 /* should be less than sizeof(HASH) * 8 */
#define SHIFT_TAB_SIZE	(1U << SHIFT_TAB_BITS)
#define HASH_TAB_SIZE	(1U << HASH_TAB_BITS)

typedef struct {
	int				B;			/* size of suffixes */
	int				Bp;			/* size of prefixes */
	STRLEN			m;			/* minimum pattern length */
	int				patnum;		/* number of patterns */
	SHIFT_TAB		*shift;		/* table mapping hash to allowed shift */
	HASH_TAB		*hash;		/* table mapping hash to int (pair of pointers) */
	HASH			*prefix;	/* array of hashes of prefixes by pattern suffix hash order */
	PATNREPL		*patterns;	/* array of prefixes by pattern suffix hash order */
} PPRES;
/* }}} */

/* {{{ php_strtr_hash */
static inline HASH php_strtr_hash(const char *str, int len)
{
	HASH	res = 0;
	int		i;
	for (i = 0; i < len; i++) {
		res = res * 33 + (unsigned char)str[i];
	}

	return res;
}
/* }}} */
/* {{{ php_strtr_populate_shift */
static inline void php_strtr_populate_shift(PATNREPL *patterns, int patnum, int B, STRLEN m, SHIFT_TAB *shift)
{
	int		i;
	STRLEN	j,
			max_shift;

	max_shift = m - B + 1;
	for (i = 0; i < SHIFT_TAB_SIZE; i++) {
		shift->entries[i] = max_shift;
	}
	for (i = 0; i < patnum; i++) {
		for (j = 0; j < m - B + 1; j++) {
			HASH h = php_strtr_hash(&S(&patterns[i].pat)[j], B) & shift->table_mask;
			assert((long long) m - (long long) j - B >= 0);
			shift->entries[h] = MIN(shift->entries[h], m - j - B);
		}
	}
}
/* }}} */
/* {{{ php_strtr_compare_hash_suffix */
static int php_strtr_compare_hash_suffix(const void *a, const void *b TSRMLS_DC, void *ctx_g)
{
	const PPRES		*res = ctx_g;
	const PATNREPL	*pnr_a = a,
					*pnr_b = b;
	HASH			hash_a = php_strtr_hash(&S(&pnr_a->pat)[res->m - res->B], res->B)
								& res->hash->table_mask,
					hash_b = php_strtr_hash(&S(&pnr_b->pat)[res->m - res->B], res->B)
								& res->hash->table_mask;
	/* TODO: don't recalculate the hashes all the time */
	if (hash_a > hash_b) {
		return 1;
	} else if (hash_a < hash_b) {
		return -1;
	} else {
		/* longer patterns must be sorted first */
		if (L(&pnr_a->pat) > L(&pnr_b->pat)) {
			return -1;
		} else if (L(&pnr_a->pat) < L(&pnr_b->pat)) {
			return 1;
		} else {
			return 0;
		}
	}
}
/* }}} */
/* {{{ php_strtr_free_strp */
static void php_strtr_free_strp(void *strp)
{
	STR_FREE(*(char**)strp);
}
/* }}} */
/* {{{ php_strtr_array_prepare_repls */
static PATNREPL *php_strtr_array_prepare_repls(int slen, HashTable *pats, zend_llist **allocs, int *outsize)
{
	PATNREPL		*patterns;
	HashPosition	hpos;
	zval			**entry;
	int				num_pats = zend_hash_num_elements(pats),
					i;

	patterns = safe_emalloc(num_pats, sizeof(*patterns), 0);
	*allocs = emalloc(sizeof **allocs);
	zend_llist_init(*allocs, sizeof(void*), &php_strtr_free_strp, 0);

	for (i = 0, zend_hash_internal_pointer_reset_ex(pats, &hpos);
			zend_hash_get_current_data_ex(pats, (void **)&entry, &hpos) == SUCCESS;
			zend_hash_move_forward_ex(pats, &hpos)) {
		char	*string_key;
		uint  	string_key_len;
		ulong	num_key;
		zval	*tzv = NULL;

		switch (zend_hash_get_current_key_ex(pats, &string_key, &string_key_len, &num_key, 0, &hpos)) {
		case HASH_KEY_IS_LONG:
			string_key_len = 1 + zend_spprintf(&string_key, 0, "%ld", (long)num_key);
			zend_llist_add_element(*allocs, &string_key);
			/* break missing intentionally */

		case HASH_KEY_IS_STRING:
			string_key_len--; /* exclude final '\0' */
			if (string_key_len == 0) { /* empty string given as pattern */
				efree(patterns);
				zend_llist_destroy(*allocs);
				efree(*allocs);
				*allocs = NULL;
				return NULL;
			}
			if (string_key_len > slen) { /* this pattern can never match */
				continue;
			}

			if (Z_TYPE_PP(entry) != IS_STRING) {
				tzv = *entry;
				zval_addref_p(tzv);
				SEPARATE_ZVAL(&tzv);
				convert_to_string(tzv);
				entry = &tzv;
				zend_llist_add_element(*allocs, &Z_STRVAL_PP(entry));
			}

			S(&patterns[i].pat) = string_key;
			L(&patterns[i].pat) = string_key_len;
			S(&patterns[i].repl) = Z_STRVAL_PP(entry);
			L(&patterns[i].repl) = Z_STRLEN_PP(entry);
			i++;

			if (tzv) {
				efree(tzv);
			}
		}
	}

	*outsize = i;
	return patterns;
}
/* }}} */

/* {{{ PPRES *php_strtr_array_prepare(STR *text, PATNREPL *patterns, int patnum, int B, int Bp) */
static PPRES *php_strtr_array_prepare(STR *text, PATNREPL *patterns, int patnum, int B, int Bp)
{
	int		i;
	PPRES	*res = emalloc(sizeof *res);

	res->m = (STRLEN)-1;
	for (i = 0; i < patnum; i++) {
		if (L(&patterns[i].pat) < res->m) {
			res->m = L(&patterns[i].pat);
		}
	}
	assert(res->m > 0);
	res->B	= B		= MIN(B, res->m);
	res->Bp	= Bp	= MIN(Bp, res->m);

	res->shift = safe_emalloc(SHIFT_TAB_SIZE, sizeof(*res->shift->entries), sizeof(*res->shift));
	res->shift->table_mask = SHIFT_TAB_SIZE - 1;
	php_strtr_populate_shift(patterns, patnum, B, res->m, res->shift);

	res->hash = safe_emalloc(HASH_TAB_SIZE, sizeof(*res->hash->entries), sizeof(*res->hash));
	res->hash->table_mask = HASH_TAB_SIZE - 1;

	res->patterns = safe_emalloc(patnum, sizeof(*res->patterns), 0);
	memcpy(res->patterns, patterns, sizeof(*patterns) * patnum);
#ifdef ZTS
	zend_qsort_r(res->patterns, patnum, sizeof(*res->patterns),
			php_strtr_compare_hash_suffix, res, NULL); /* tsrmls not needed */
#else
	zend_qsort_r(res->patterns, patnum, sizeof(*res->patterns),
			php_strtr_compare_hash_suffix, res);
#endif

	res->prefix = safe_emalloc(patnum, sizeof(*res->prefix), 0);
	for (i = 0; i < patnum; i++) {
		res->prefix[i] = php_strtr_hash(S(&res->patterns[i].pat), Bp);
	}

	/* Initialize the rest of ->hash */
	for (i = 0; i < HASH_TAB_SIZE; i++) {
		res->hash->entries[i] = -1;
	}
	{
		HASH last_h = -1; /* assumes not all bits are used in res->hash */
		/* res->patterns is already ordered by hash.
		 * Make res->hash->entries[h] de index of the first pattern in
		 * res->patterns that has hash h */
		for (i = 0; i < patnum; i++) {
			HASH h = php_strtr_hash(&S(&res->patterns[i].pat)[res->m - res->B], res->B)
						& res->hash->table_mask;
			if (h != last_h) {
				res->hash->entries[h] = i;
				last_h = h;
			}
		}
	}
	res->hash->entries[HASH_TAB_SIZE] = patnum; /* OK, we effectively allocated SIZE+1 */
	for (i = HASH_TAB_SIZE - 1; i >= 0; i--) {
		if (res->hash->entries[i] == -1) {
			res->hash->entries[i] = res->hash->entries[i + 1];
		}
	}

	res->patnum	= patnum;

	return res;
}
/* }}} */
/* {{{ php_strtr_array_destroy_ppres(PPRES *d) */
static void php_strtr_array_destroy_ppres(PPRES *d)
{
	efree(d->shift);
	efree(d->hash);
	efree(d->prefix);
	efree(d->patterns);
	efree(d);
}
/* }}} */

/* {{{ php_strtr_array_do_repl(STR *text, PPRES *d, zval *return_value) */
static void php_strtr_array_do_repl(STR *text, PPRES *d, zval *return_value)
{
	STRLEN		pos = 0,
				nextwpos = 0,
				lastpos	= L(text) - d->m;
	smart_str	result = {0};

	while (pos <= lastpos) {
		HASH	h		= php_strtr_hash(&S(text)[pos + d->m - d->B], d->B) & d->shift->table_mask;
		STRLEN	shift	= d->shift->entries[h];

		if (shift > 0) {
			pos += shift;
		} else {
			HASH	h2				= h & d->hash->table_mask,
					prefix_h		= php_strtr_hash(&S(text)[pos], d->Bp);

			int		offset_start	= d->hash->entries[h2],
					offset_end		= d->hash->entries[h2 + 1], /* exclusive */
					i				= 0;

			for (i = offset_start; i < offset_end; i++) {
				PATNREPL *pnr;
				if (d->prefix[i] != prefix_h)
					continue;

				pnr = &d->patterns[i];
				if (L(&pnr->pat) > L(text) - pos ||
						memcmp(S(&pnr->pat), &S(text)[pos], L(&pnr->pat)) != 0)
					continue;

				smart_str_appendl(&result, &S(text)[nextwpos], pos - nextwpos);
				smart_str_appendl(&result, S(&pnr->repl), L(&pnr->repl));
				pos += L(&pnr->pat);
				nextwpos = pos;
				goto end_outer_loop;
			}

			pos++;
end_outer_loop: ;
		}
	}

	smart_str_appendl(&result, &S(text)[nextwpos], L(text) - nextwpos);

	if (result.c != NULL) {
		smart_str_0(&result);
		RETVAL_STRINGL(result.c, result.len, 0);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ php_strtr_array */
static void php_strtr_array(zval *return_value, char *str, int slen, HashTable *pats)
{
	PPRES		*data;
	STR			text;
	PATNREPL	*patterns;
	int			patterns_len;
	zend_llist	*allocs;

	S(&text) = str;
	L(&text) = slen;

	patterns = php_strtr_array_prepare_repls(slen, pats, &allocs, &patterns_len);
	if (patterns == NULL) {
		RETURN_FALSE;
	}
	data = php_strtr_array_prepare(&text, patterns, patterns_len, 2, 2);
	efree(patterns);
	php_strtr_array_do_repl(&text, data, return_value);
	php_strtr_array_destroy_ppres(data);
	zend_llist_destroy(allocs);
	efree(allocs);
}
/* }}} */

/* {{{ proto string strtr(string str, string from[, string to])
   Translates characters in str using given translation tables */
PHP_FUNCTION(strtr)
{
	zval **from;
	char *str, *to = NULL;
	int str_len, to_len = 0;
	int ac = ZEND_NUM_ARGS();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sZ|s", &str, &str_len, &from, &to, &to_len) == FAILURE) {
		return;
	}

	if (ac == 2 && Z_TYPE_PP(from) != IS_ARRAY) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The second argument is not an array");
		RETURN_FALSE;
	}

	/* shortcut for empty string */
	if (str_len == 0) {
		RETURN_EMPTY_STRING();
	}

	if (ac == 2) {
		php_strtr_array(return_value, str, str_len, HASH_OF(*from));
	} else {
		convert_to_string_ex(from);

		ZVAL_STRINGL(return_value, str, str_len, 1);

		php_strtr(Z_STRVAL_P(return_value),
				  Z_STRLEN_P(return_value),
				  Z_STRVAL_PP(from),
				  to,
				  MIN(Z_STRLEN_PP(from),
				  to_len));
	}
}
/* }}} */

/* {{{ proto string strrev(string str)
   Reverse a string */
PHP_FUNCTION(strrev)
{
	char *str;
	char *e, *n, *p;
	int  str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	n = emalloc(str_len+1);
	p = n;

	e = str + str_len;

	while (--e>=str) {
		*p++ = *e;
	}

	*p = '\0';

	RETVAL_STRINGL(n, str_len, 0);
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
	int pos1 = 0, pos2 = 0, max;

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

/* {{{ proto int similar_text(string str1, string str2 [, float percent])
   Calculates the similarity between two strings */
PHP_FUNCTION(similar_text)
{
	char *t1, *t2;
	zval **percent = NULL;
	int ac = ZEND_NUM_ARGS();
	int sim;
	int t1_len, t2_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|Z", &t1, &t1_len, &t2, &t2_len, &percent) == FAILURE) {
		return;
	}

	if (ac > 2) {
		convert_to_double_ex(percent);
	}

	if (t1_len + t2_len == 0) {
		if (ac > 2) {
			Z_DVAL_PP(percent) = 0;
		}

		RETURN_LONG(0);
	}

	sim = php_similar_char(t1, t1_len, t2, t2_len);

	if (ac > 2) {
		Z_DVAL_PP(percent) = sim * 200.0 / (t1_len + t2_len);
	}

	RETURN_LONG(sim);
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
	char *str, *what;
	int str_len, what_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &str, &str_len, &what, &what_len) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_EMPTY_STRING();
	}

	if (what_len == 0) {
		RETURN_STRINGL(str, str_len, 1);
	}

	Z_STRVAL_P(return_value) = php_addcslashes(str, str_len, &Z_STRLEN_P(return_value), 0, what, what_len TSRMLS_CC);
	RETURN_STRINGL(Z_STRVAL_P(return_value), Z_STRLEN_P(return_value), 0);
}
/* }}} */

/* {{{ proto string addslashes(string str)
   Escapes single quote, double quotes and backslash characters in a string with backslashes */
PHP_FUNCTION(addslashes)
{
	char *str;
	int  str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_EMPTY_STRING();
	}

	RETURN_STRING(php_addslashes(str,
	                             str_len,
	                             &Z_STRLEN_P(return_value), 0
	                             TSRMLS_CC), 0);
}
/* }}} */

/* {{{ proto string stripcslashes(string str)
   Strips backslashes from a string. Uses C-style conventions */
PHP_FUNCTION(stripcslashes)
{
	char *str;
	int  str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	ZVAL_STRINGL(return_value, str, str_len, 1);
	php_stripcslashes(Z_STRVAL_P(return_value), &Z_STRLEN_P(return_value));
}
/* }}} */

/* {{{ proto string stripslashes(string str)
   Strips backslashes from a string */
PHP_FUNCTION(stripslashes)
{
	char *str;
	int  str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	ZVAL_STRINGL(return_value, str, str_len, 1);
	php_stripslashes(Z_STRVAL_P(return_value), &Z_STRLEN_P(return_value) TSRMLS_CC);
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

	(void) snprintf(BG(str_ebuf), sizeof(php_basic_globals.str_ebuf), "Unknown error: %d", errnum);
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
PHPAPI char *php_addcslashes(const char *str, int length, int *new_length, int should_free, char *what, int wlength TSRMLS_DC)
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

	php_charmask((unsigned char *)what, wlength, flags TSRMLS_CC);

	for (source = (char*)str, end = source + length, target = new_str; source < end; source++) {
		c = *source;
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
		STR_FREE((char*)str);
	}
	return new_str;
}
/* }}} */

/* {{{ php_addslashes
 */
PHPAPI char *php_addslashes(char *str, int length, int *new_length, int should_free TSRMLS_DC)
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
	Z_STRVAL_P(result) = target = safe_emalloc(char_count, to_len, len + 1);
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
			if (replace_count) {
				*replace_count += 1;
			}
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
		if (case_sensitivity && memcmp(haystack, needle, length)) {
			goto nothing_todo;
		} else if (!case_sensitivity) {
			char *l_haystack, *l_needle;

			l_haystack = estrndup(haystack, length);
			l_needle = estrndup(needle, length);

			php_strtolower(l_haystack, length);
			php_strtolower(l_needle, length);

			if (memcmp(l_haystack, l_needle, length)) {
				efree(l_haystack);
				efree(l_needle);
				goto nothing_todo;
			}
			efree(l_haystack);
			efree(l_needle);
		}

		*_new_length = str_len;
		new_str = estrndup(str, str_len);

		if (replace_count) {
			(*replace_count)++;
		}
		return new_str;
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
/* }}} */

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
		MAKE_COPY_ZVAL(subject, result);

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

           str_efree(Z_STRVAL_P(result));
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
			MAKE_COPY_ZVAL(subject, result);
		}
	}
}
/* }}} */

/* {{{ php_str_replace_common
 */
static void php_str_replace_common(INTERNAL_FUNCTION_PARAMETERS, int case_sensitivity)
{
	zval **subject, **search, **replace, **subject_entry, **zcount = NULL;
	zval *result;
	char *string_key;
	uint string_key_len;
	ulong num_key;
	int count = 0;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZZ|Z", &search, &replace, &subject, &zcount) == FAILURE) {
		return;
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
				Z_ADDREF_P(*subject_entry);
				COPY_PZVAL_TO_ZVAL(*result, *subject_entry);
			}
			/* Add to return array */
			switch (zend_hash_get_current_key_ex(Z_ARRVAL_PP(subject), &string_key,
												&string_key_len, &num_key, 0, NULL)) {
				case HASH_KEY_IS_STRING:
					add_assoc_zval_ex(return_value, string_key, string_key_len, result);
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
	char *str;
	char *heb_str, *tmp, *target, *broken_str;
	int block_start, block_end, block_type, block_length, i;
	long max_chars=0;
	int begin, end, char_count, orig_begin;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &str, &str_len, &max_chars) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_FALSE;
	}

	tmp = str;
	block_start=block_end=0;

	heb_str = (char *) emalloc(str_len+1);
	target = heb_str+str_len;
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
			while ((isheb((int)*(tmp+1)) || _isblank((int)*(tmp+1)) || ispunct((int)*(tmp+1)) || (int)*(tmp+1)=='\n' ) && block_end<str_len-1) {
				tmp++;
				block_end++;
				block_length++;
			}
			for (i = block_start; i<= block_end; i++) {
				*target = str[i];
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
			while (!isheb(*(tmp+1)) && (int)*(tmp+1)!='\n' && block_end < str_len-1) {
				tmp++;
				block_end++;
				block_length++;
			}
			while ((_isblank((int)*tmp) || ispunct((int)*tmp)) && *tmp!='/' && *tmp!='-' && block_end > block_start) {
				tmp--;
				block_end--;
			}
			for (i = block_end; i >= block_start; i--) {
				*target = str[i];
				target--;
			}
			block_type = _HEB_BLOCK_TYPE_HEB;
		}
		block_start=block_end+1;
	} while (block_end < str_len-1);


	broken_str = (char *) emalloc(str_len+1);
	begin=end=str_len-1;
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
		php_char_to_str(broken_str, str_len,'\n', "<br />\n", 7, return_value);
		efree(broken_str);
	} else {
		Z_STRVAL_P(return_value) = broken_str;
		Z_STRLEN_P(return_value) = str_len;
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

/* {{{ proto string nl2br(string str [, bool is_xhtml])
   Converts newlines to HTML line breaks */
PHP_FUNCTION(nl2br)
{
	/* in brief this inserts <br /> or <br> before matched regexp \n\r?|\r\n? */
	char		*tmp, *str;
	int		new_length;
	char		*end, *target;
	int		repl_cnt = 0;
	int		str_len;
	zend_bool	is_xhtml = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &str, &str_len, &is_xhtml) == FAILURE) {
		return;
	}

	tmp = str;
	end = str + str_len;

	/* it is really faster to scan twice and allocate mem once instead of scanning once
	   and constantly reallocing */
	while (tmp < end) {
		if (*tmp == '\r') {
			if (*(tmp+1) == '\n') {
				tmp++;
			}
			repl_cnt++;
		} else if (*tmp == '\n') {
			if (*(tmp+1) == '\r') {
				tmp++;
			}
			repl_cnt++;
		}

		tmp++;
	}

	if (repl_cnt == 0) {
		RETURN_STRINGL(str, str_len, 1);
	}

	{
		size_t repl_len = is_xhtml ? (sizeof("<br />") - 1) : (sizeof("<br>") - 1);

		new_length = str_len + repl_cnt * repl_len;
		tmp = target = safe_emalloc(repl_cnt, repl_len, str_len + 1);
	}

	while (str < end) {
		switch (*str) {
			case '\r':
			case '\n':
				*target++ = '<';
				*target++ = 'b';
				*target++ = 'r';

				if (is_xhtml) {
					*target++ = ' ';
					*target++ = '/';
				}

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

/* {{{ proto string strip_tags(string str [, string allowable_tags])
   Strips HTML and PHP tags from a string */
PHP_FUNCTION(strip_tags)
{
	char *buf;
	char *str;
	zval **allow=NULL;
	char *allowed_tags=NULL;
	int allowed_tags_len=0;
	int str_len;
	size_t retval_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|Z", &str, &str_len, &allow) == FAILURE) {
		return;
	}

	/* To maintain a certain BC, we allow anything for the second parameter and return original string */
	if (allow != NULL) {
		convert_to_string_ex(allow);
		allowed_tags = Z_STRVAL_PP(allow);
		allowed_tags_len = Z_STRLEN_PP(allow);
	}

	buf = estrndup(str, str_len);
	retval_len = php_strip_tags_ex(buf, str_len, NULL, allowed_tags, allowed_tags_len, 0);
	RETURN_STRINGL(buf, retval_len, 0);
}
/* }}} */

/* {{{ proto string setlocale(mixed category, string locale [, string ...])
   Set locale information */
PHP_FUNCTION(setlocale)
{
	zval ***args = NULL;
	zval **pcategory, **plocale;
	int num_args, cat, i = 0;
	char *loc, *retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z+", &pcategory, &args, &num_args) == FAILURE) {
		return;
	}

#ifdef HAVE_SETLOCALE
	if (Z_TYPE_PP(pcategory) == IS_LONG) {
		convert_to_long_ex(pcategory);
		cat = Z_LVAL_PP(pcategory);
	} else {
		/* FIXME: The following behaviour should be removed. */
		char *category;

		php_error_docref(NULL TSRMLS_CC, E_DEPRECATED, "Passing locale category name as string is deprecated. Use the LC_* -constants instead");

		convert_to_string_ex(pcategory);
		category = Z_STRVAL_PP(pcategory);

		if (!strcasecmp("LC_ALL", category)) {
			cat = LC_ALL;
		} else if (!strcasecmp("LC_COLLATE", category)) {
			cat = LC_COLLATE;
		} else if (!strcasecmp("LC_CTYPE", category)) {
			cat = LC_CTYPE;
#ifdef LC_MESSAGES
		} else if (!strcasecmp("LC_MESSAGES", category)) {
			cat = LC_MESSAGES;
#endif
		} else if (!strcasecmp("LC_MONETARY", category)) {
			cat = LC_MONETARY;
		} else if (!strcasecmp("LC_NUMERIC", category)) {
			cat = LC_NUMERIC;
		} else if (!strcasecmp("LC_TIME", category)) {
			cat = LC_TIME;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid locale category name %s, must be one of LC_ALL, LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, or LC_TIME", category);

			if (args) {
				efree(args);
			}
			RETURN_FALSE;
		}
	}

	if (Z_TYPE_PP(args[0]) == IS_ARRAY) {
		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(args[0]));
	}

	while (1) {
		if (Z_TYPE_PP(args[0]) == IS_ARRAY) {
			if (!zend_hash_num_elements(Z_ARRVAL_PP(args[0]))) {
				break;
			}
			zend_hash_get_current_data(Z_ARRVAL_PP(args[0]), (void **)&plocale);
		} else {
			plocale = args[i];
		}

		convert_to_string_ex(plocale);

		if (!strcmp ("0", Z_STRVAL_PP(plocale))) {
			loc = NULL;
		} else {
			loc = Z_STRVAL_PP(plocale);
			if (Z_STRLEN_PP(plocale) >= 255) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Specified locale name is too long");
				break;
			}
		}

		retval = php_my_setlocale(cat, loc);
		zend_update_current_locale();
		if (retval) {
			/* Remember if locale was changed */
			if (loc) {
				STR_FREE(BG(locale_string));
				BG(locale_string) = estrdup(retval);
			}

			if (args) {
				efree(args);
			}
			RETURN_STRING(retval, 1);
		}

		if (Z_TYPE_PP(args[0]) == IS_ARRAY) {
			if (zend_hash_move_forward(Z_ARRVAL_PP(args[0])) == FAILURE) break;
		} else {
			if (++i >= num_args) break;
		}
	}

#endif
	if (args) {
		efree(args);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto void parse_str(string encoded_string [, array result])
   Parses GET/POST/COOKIE data and sets global variables */
PHP_FUNCTION(parse_str)
{
	char *arg;
	zval *arrayArg = NULL;
	char *res = NULL;
	int arglen;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z", &arg, &arglen, &arrayArg) == FAILURE) {
		return;
	}

	res = estrndup(arg, arglen);

	if (arrayArg == NULL) {
		zval tmp;

		if (!EG(active_symbol_table)) {
			zend_rebuild_symbol_table(TSRMLS_C);
		}
		Z_ARRVAL(tmp) = EG(active_symbol_table);
		sapi_module.treat_data(PARSE_STRING, res, &tmp TSRMLS_CC);
	} else 	{
		zval ret;

		array_init(&ret);
		sapi_module.treat_data(PARSE_STRING, res, &ret TSRMLS_CC);
		/* Clear out the array that was passed in. */
		zval_dtor(arrayArg);
		ZVAL_COPY_VALUE(arrayArg, &ret);
	}
}
/* }}} */

#define PHP_TAG_BUF_SIZE 1023

/* {{{ php_tag_find
 *
 * Check if tag is in a set of tags
 *
 * states:
 *
 * 0 start tag
 * 1 first non-whitespace char seen
 */
int php_tag_find(char *tag, int len, char *set) {
	char c, *n, *t;
	int state=0, done=0;
	char *norm;

	if (len <= 0) {
		return 0;
	}

	norm = emalloc(len+1);

	n = norm;
	t = tag;
	c = tolower(*t);
	/*
	   normalize the tag removing leading and trailing whitespace
	   and turn any <a whatever...> into just <a> and any </tag>
	   into <tag>
	*/
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
					}
					if (c != '/') {
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

PHPAPI size_t php_strip_tags(char *rbuf, int len, int *stateptr, char *allow, int allow_len) /* {{{ */
{
	return php_strip_tags_ex(rbuf, len, stateptr, allow, allow_len, 0);
}
/* }}} */

/* {{{ php_strip_tags

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
PHPAPI size_t php_strip_tags_ex(char *rbuf, int len, int *stateptr, char *allow, int allow_len, zend_bool allow_tag_spaces)
{
	char *tbuf, *buf, *p, *tp, *rp, c, lc;
	int br, i=0, depth=0, in_q = 0;
	int state = 0, pos;
	char *allow_free = NULL;

	if (stateptr)
		state = *stateptr;

	buf = estrndup(rbuf, len);
	c = *buf;
	lc = '\0';
	p = buf;
	rp = rbuf;
	br = 0;
	if (allow) {
		if (IS_INTERNED(allow)) {
			allow_free = allow = zend_str_tolower_dup(allow, allow_len);
		} else {
			allow_free = NULL;
			php_strtolower(allow, allow_len);
		}
		tbuf = emalloc(PHP_TAG_BUF_SIZE + 1);
		tp = tbuf;
	} else {
		tbuf = tp = NULL;
	}

	while (i < len) {
		switch (c) {
			case '\0':
				break;
			case '<':
				if (in_q) {
					break;
				}
				if (isspace(*(p + 1)) && !allow_tag_spaces) {
					goto reg_char;
				}
				if (state == 0) {
					lc = '<';
					state = 1;
					if (allow) {
						if (tp - tbuf >= PHP_TAG_BUF_SIZE) {
							pos = tp - tbuf;
							tbuf = erealloc(tbuf, (tp - tbuf) + PHP_TAG_BUF_SIZE + 1);
							tp = tbuf + pos;
						}
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
					if (tp - tbuf >= PHP_TAG_BUF_SIZE) {
						pos = tp - tbuf;
						tbuf = erealloc(tbuf, (tp - tbuf) + PHP_TAG_BUF_SIZE + 1);
						tp = tbuf + pos;
					}
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
					if (tp - tbuf >= PHP_TAG_BUF_SIZE) {
						pos = tp - tbuf;
						tbuf = erealloc(tbuf, (tp - tbuf) + PHP_TAG_BUF_SIZE + 1);
						tp = tbuf + pos;
					}
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

				if (in_q) {
					break;
				}

				switch (state) {
					case 1: /* HTML/XML */
						lc = '>';
						in_q = state = 0;
						if (allow) {
							if (tp - tbuf >= PHP_TAG_BUF_SIZE) {
								pos = tp - tbuf;
								tbuf = erealloc(tbuf, (tp - tbuf) + PHP_TAG_BUF_SIZE + 1);
								tp = tbuf + pos;
							}
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
							in_q = state = 0;
							tp = tbuf;
						}
						break;

					case 3:
						in_q = state = 0;
						tp = tbuf;
						break;

					case 4: /* JavaScript/CSS/etc... */
						if (p >= buf + 2 && *(p-1) == '-' && *(p-2) == '-') {
							in_q = state = 0;
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
				if (state == 4) {
					/* Inside <!-- comment --> */
					break;
				} else if (state == 2 && *(p-1) != '\\') {
					if (lc == c) {
						lc = '\0';
					} else if (lc != '\\') {
						lc = c;
					}
				} else if (state == 0) {
					*(rp++) = c;
				} else if (allow && state == 1) {
					if (tp - tbuf >= PHP_TAG_BUF_SIZE) {
						pos = tp - tbuf;
						tbuf = erealloc(tbuf, (tp - tbuf) + PHP_TAG_BUF_SIZE + 1);
						tp = tbuf + pos;
					}
					*(tp++) = c;
				}
				if (state && p != buf && (state == 1 || *(p-1) != '\\') && (!in_q || *p == in_q)) {
					if (in_q) {
						in_q = 0;
					} else {
						in_q = *p;
					}
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
						if (tp - tbuf >= PHP_TAG_BUF_SIZE) {
							pos = tp - tbuf;
							tbuf = erealloc(tbuf, (tp - tbuf) + PHP_TAG_BUF_SIZE + 1);
							tp = tbuf + pos;
						}
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
			case 'L':

				/* swm: If we encounter '<?xml' then we shouldn't be in
				 * state == 2 (PHP). Switch back to HTML.
				 */

				if (state == 2 && p > buf+2 && strncasecmp(p-2, "xm", 2) == 0) {
					state = 1;
					break;
				}

				/* fall-through */
			default:
reg_char:
				if (state == 0) {
					*(rp++) = c;
				} else if (allow && state == 1) {
					if (tp - tbuf >= PHP_TAG_BUF_SIZE) {
						pos = tp - tbuf;
						tbuf = erealloc(tbuf, (tp - tbuf) + PHP_TAG_BUF_SIZE + 1);
						tp = tbuf + pos;
					}
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
	if (allow) {
		efree(tbuf);
		if (allow_free) {
			efree(allow_free);
		}
	}
	if (stateptr)
		*stateptr = state;

	return (size_t)(rp - rbuf);
}
/* }}} */

/* {{{ proto array str_getcsv(string input[, string delimiter[, string enclosure[, string escape]]])
Parse a CSV string into an array */
PHP_FUNCTION(str_getcsv)
{
	char *str, delim = ',', enc = '"', esc = '\\';
	char *delim_str = NULL, *enc_str = NULL, *esc_str = NULL;
	int str_len = 0, delim_len = 0, enc_len = 0, esc_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sss", &str, &str_len, &delim_str, &delim_len,
		&enc_str, &enc_len, &esc_str, &esc_len) == FAILURE) {
		return;
	}

	delim = delim_len ? delim_str[0] : delim;
	enc = enc_len ? enc_str[0] : enc;
	esc = esc_len ? esc_str[0] : esc;

	php_fgetcsv(NULL, delim, enc, esc, str_len, str, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto string str_repeat(string input, int mult)
   Returns the input string repeat mult times */
PHP_FUNCTION(str_repeat)
{
	char		*input_str;		/* Input string */
	int 		input_len;
	long 		mult;			/* Multiplier */
	char		*result;		/* Resulting string */
	size_t		result_len;		/* Length of the resulting string */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &input_str, &input_len, &mult) == FAILURE) {
		return;
	}

	if (mult < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Second argument has to be greater than or equal to 0");
		return;
	}

	/* Don't waste our time if it's empty */
	/* ... or if the multiplier is zero */
	if (input_len == 0 || mult == 0)
		RETURN_EMPTY_STRING();

	/* Initialize the result string */
	result_len = input_len * mult;
	result = (char *)safe_emalloc(input_len, mult, 1);

	/* Heavy optimization for situations where input string is 1 byte long */
	if (input_len == 1) {
		memset(result, *(input_str), mult);
	} else {
		char *s, *e, *ee;
		int l=0;
		memcpy(result, input_str, input_len);
		s = result;
		e = result + input_len;
		ee = result + result_len;

		while (e<ee) {
			l = (e-s) < (ee-e) ? (e-s) : (ee-e);
			memmove(e, s, l);
			e += l;
		}
	}

	result[result_len] = '\0';

	RETURN_STRINGL(result, result_len, 0);
}
/* }}} */

/* {{{ proto mixed count_chars(string input [, int mode])
   Returns info about what characters are used in input */
PHP_FUNCTION(count_chars)
{
	char *input;
	int chars[256];
	long mymode=0;
	unsigned char *buf;
	int len, inx;
	char retstr[256];
	int retlen=0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &input, &len, &mymode) == FAILURE) {
		return;
	}

	if (mymode < 0 || mymode > 4) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown mode");
		RETURN_FALSE;
	}

	buf = (unsigned char *) input;
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
	char *s1, *s2;
	int s1_len, s2_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &s1, &s1_len, &s2, &s2_len) == FAILURE) {
		return;
	}

	RETURN_LONG(strnatcmp_ex(s1, s1_len,
							 s2, s2_len,
							 fold_case));
}
/* }}} */

PHPAPI int string_natural_compare_function_ex(zval *result, zval *op1, zval *op2, zend_bool case_insensitive TSRMLS_DC) /* {{{ */
{
	zval op1_copy, op2_copy;
	int use_copy1 = 0, use_copy2 = 0;

	if (Z_TYPE_P(op1) != IS_STRING) {
		zend_make_printable_zval(op1, &op1_copy, &use_copy1);
	}
	if (Z_TYPE_P(op2) != IS_STRING) {
		zend_make_printable_zval(op2, &op2_copy, &use_copy2);
	}

	if (use_copy1) {
		op1 = &op1_copy;
	}
	if (use_copy2) {
		op2 = &op2_copy;
	}

	ZVAL_LONG(result, strnatcmp_ex(Z_STRVAL_P(op1), Z_STRLEN_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op2), case_insensitive));

	if (use_copy1) {
		zval_dtor(op1);
	}
	if (use_copy2) {
		zval_dtor(op2);
	}
	return SUCCESS;
}
/* }}} */

PHPAPI int string_natural_case_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	return string_natural_compare_function_ex(result, op1, op2, 1 TSRMLS_CC);
}
/* }}} */

PHPAPI int string_natural_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC) /* {{{ */
{
	return string_natural_compare_function_ex(result, op1, op2, 0 TSRMLS_CC);
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
	if (zend_parse_parameters_none() == FAILURE) {
		return;
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

/* {{{ proto int substr_count(string haystack, string needle [, int offset [, int length]])
   Returns the number of times a substring occurs in the string */
PHP_FUNCTION(substr_count)
{
	char *haystack, *needle;
	long offset = 0, length = 0;
	int ac = ZEND_NUM_ARGS();
	int count = 0;
	int haystack_len, needle_len;
	char *p, *endp, cmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ll", &haystack, &haystack_len, &needle, &needle_len, &offset, &length) == FAILURE) {
		return;
	}

	if (needle_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty substring");
		RETURN_FALSE;
	}

	p = haystack;
	endp = p + haystack_len;

	if (offset < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset should be greater than or equal to 0");
		RETURN_FALSE;
	}

	if (offset > haystack_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset value %ld exceeds string length", offset);
		RETURN_FALSE;
	}
	p += offset;

	if (ac == 4) {

		if (length <= 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length should be greater than 0");
			RETURN_FALSE;
		}
		if (length > (haystack_len - offset)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Length value %ld exceeds string length", length);
			RETURN_FALSE;
		}
		endp = p + length;
	}

	if (needle_len == 1) {
		cmp = needle[0];

		while ((p = memchr(p, cmp, endp - p))) {
			count++;
			p++;
		}
	} else {
		while ((p = php_memnstr(p, needle, needle_len, endp))) {
			p += needle_len;
			count++;
		}
	}

	RETURN_LONG(count);
}
/* }}} */

/* {{{ proto string str_pad(string input, int pad_length [, string pad_string [, int pad_type]])
   Returns input string padded on the left or right to specified length with pad_string */
PHP_FUNCTION(str_pad)
{
	/* Input arguments */
	char *input;				/* Input string */
	int  input_len;
	long pad_length;			/* Length to pad to */

	/* Helper variables */
	size_t	   num_pad_chars;		/* Number of padding characters (total - input size) */
	char  *result = NULL;		/* Resulting string */
	int	   result_len = 0;		/* Length of the resulting string */
	char  *pad_str_val = " ";	/* Pointer to padding string */
	int    pad_str_len = 1;		/* Length of the padding string */
	long   pad_type_val = STR_PAD_RIGHT; /* The padding type value */
	int	   i, left_pad=0, right_pad=0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|sl", &input, &input_len, &pad_length,
																  &pad_str_val, &pad_str_len, &pad_type_val) == FAILURE) {
		return;
	}

	/* If resulting string turns out to be shorter than input string,
	   we simply copy the input and return. */
	if (pad_length <= 0 || (pad_length - input_len) <= 0) {
		RETURN_STRINGL(input, input_len, 1);
	}

	if (pad_str_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Padding string cannot be empty");
		return;
	}

	if (pad_type_val < STR_PAD_LEFT || pad_type_val > STR_PAD_BOTH) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Padding type has to be STR_PAD_LEFT, STR_PAD_RIGHT, or STR_PAD_BOTH");
		return;
	}

	num_pad_chars = pad_length - input_len;
	if (num_pad_chars >= INT_MAX) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Padding length is too long");
		return;
	}
	result = (char *)emalloc(input_len + num_pad_chars + 1);

	/* We need to figure out the left/right padding lengths. */
	switch (pad_type_val) {
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

	/* First we pad on the left. */
	for (i = 0; i < left_pad; i++)
		result[result_len++] = pad_str_val[i % pad_str_len];

	/* Then we copy the input string. */
	memcpy(result + result_len, input, input_len);
	result_len += input_len;

	/* Finally, we pad on the right. */
	for (i = 0; i < right_pad; i++)
		result[result_len++] = pad_str_val[i % pad_str_len];

	result[result_len] = '\0';

	RETURN_STRINGL(result, result_len, 0);
}
/* }}} */

/* {{{ proto mixed sscanf(string str, string format [, string ...])
   Implements an ANSI C compatible sscanf */
PHP_FUNCTION(sscanf)
{
	zval ***args = NULL;
	char *str, *format;
	int str_len, format_len, result, num_args = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss*", &str, &str_len, &format, &format_len,
		&args, &num_args) == FAILURE) {
		return;
	}

	result = php_sscanf_internal(str, format, num_args, args, 0, &return_value TSRMLS_CC);

	if (args) {
		efree(args);
	}

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
	char *arg;
	int arglen;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arglen) == FAILURE) {
		return;
	}

	RETVAL_STRINGL(arg, arglen, 1);

	php_strtr(Z_STRVAL_P(return_value), Z_STRLEN_P(return_value), rot13_from, rot13_to, 52);
}
/* }}} */

static void php_string_shuffle(char *str, long len TSRMLS_DC) /* {{{ */
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
/* }}} */

/* {{{ proto void str_shuffle(string str)
   Shuffles string. One permutation of all possible is created */
PHP_FUNCTION(str_shuffle)
{
	char *arg;
	int arglen;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arglen) == FAILURE) {
		return;
	}

	RETVAL_STRINGL(arg, arglen, 1);
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
	int str_len, char_list_len = 0, word_count = 0;
	long type = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls", &str, &str_len, &type, &char_list, &char_list_len) == FAILURE) {
		return;
	}

	switch(type) {
		case 1:
		case 2:
			array_init(return_value);
			if (!str_len) {
				return;
			}
			break;
		case 0:
			if (!str_len) {
				RETURN_LONG(0);
			}
			/* nothing to be done */
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid format value %ld", type);
			RETURN_FALSE;
	}

	if (char_list) {
		php_charmask((unsigned char *)char_list, char_list_len, ch TSRMLS_CC);
	}

	p = str;
	e = str + str_len;

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
		while (p < e && (isalpha((unsigned char)*p) || (char_list && ch[(unsigned char)*p]) || *p == '\'' || *p == '-')) {
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
	char *format, *str, *p, *e;
	double value;
	zend_bool check = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sd", &format, &format_len, &value) == FAILURE) {
		return;
	}

	p = format;
	e = p + format_len;
	while ((p = memchr(p, '%', (e - p)))) {
		if (*(p + 1) == '%') {
			p += 2;
		} else if (!check) {
			check = 1;
			p++;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Only a single %%i or %%n token can be used");
			RETURN_FALSE;
		}
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The length of each segment must be greater than zero");
		RETURN_FALSE;
	}

	array_init_size(return_value, ((str_len - 1) / split_length) + 1);

	if (split_length >= str_len) {
		add_next_index_stringl(return_value, str, str_len, 1);
		return;
	}

	n_reg_segments = str_len / split_length;
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

/* {{{ proto array strpbrk(string haystack, string char_list)
   Search a string for any of a set of characters */
PHP_FUNCTION(strpbrk)
{
	char *haystack, *char_list;
	int haystack_len, char_list_len;
	char *haystack_ptr, *cl_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &haystack, &haystack_len, &char_list, &char_list_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (!char_list_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The character list cannot be empty");
		RETURN_FALSE;
	}

	for (haystack_ptr = haystack; haystack_ptr < (haystack + haystack_len); ++haystack_ptr) {
		for (cl_ptr = char_list; cl_ptr < (char_list + char_list_len); ++cl_ptr) {
			if (*cl_ptr == *haystack_ptr) {
				RETURN_STRINGL(haystack_ptr, (haystack + haystack_len - haystack_ptr), 1);
			}
		}
	}

	RETURN_FALSE;
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

	if (offset >= s1_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The start position cannot exceed initial string length");
		RETURN_FALSE;
	}

	cmp_len = (uint) (len ? len : MAX(s2_len, (s1_len - offset)));

	if (!cs) {
		RETURN_LONG(zend_binary_strncmp(s1 + offset, (s1_len - offset), s2, s2_len, cmp_len));
	} else {
		RETURN_LONG(zend_binary_strncasecmp_l(s1 + offset, (s1_len - offset), s2, s2_len, cmp_len));
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
