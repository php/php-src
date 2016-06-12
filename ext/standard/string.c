/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
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
   |          Stig S�ther Bakken <ssb@php.net>                          |
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
#include "zend_smart_str.h"
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

int php_tag_find(char *tag, size_t len, const char *set);

#ifdef PHP_WIN32
# define SET_ALIGNED(alignment, decl) __declspec(align(alignment)) decl
#elif HAVE_ATTRIBUTE_ALIGNED
# define SET_ALIGNED(alignment, decl) decl __attribute__ ((__aligned__ (alignment)))
#else
# define SET_ALIGNED(alignment, decl) decl
#endif

/* this is read-only, so it's ok */
SET_ALIGNED(16, static char hexconvtab[]) = "0123456789abcdef";

/* localeconv mutex */
#ifdef ZTS
static MUTEX_T locale_mutex = NULL;
#endif

/* {{{ php_bin2hex
 */
static zend_string *php_bin2hex(const unsigned char *old, const size_t oldlen)
{
	zend_string *result;
	size_t i, j;

	result = zend_string_safe_alloc(oldlen, 2 * sizeof(char), 0, 0);

	for (i = j = 0; i < oldlen; i++) {
		ZSTR_VAL(result)[j++] = hexconvtab[old[i] >> 4];
		ZSTR_VAL(result)[j++] = hexconvtab[old[i] & 15];
	}
	ZSTR_VAL(result)[j] = '\0';

	return result;
}
/* }}} */

/* {{{ php_hex2bin
 */
static zend_string *php_hex2bin(const unsigned char *old, const size_t oldlen)
{
	size_t target_length = oldlen >> 1;
	zend_string *str = zend_string_alloc(target_length, 0);
	unsigned char *ret = (unsigned char *)ZSTR_VAL(str);
	size_t i, j;

	for (i = j = 0; i < target_length; i++) {
		unsigned char c = old[j++];
		unsigned char l = c & ~0x20;
		int is_letter = ((unsigned int) ((l - 'A') ^ (l - 'F' - 1))) >> (8 * sizeof(unsigned int) - 1);
		unsigned char d;

		/* basically (c >= '0' && c <= '9') || (l >= 'A' && l <= 'F') */ 
		if (EXPECTED((((c ^ '0') - 10) >> (8 * sizeof(unsigned int) - 1)) | is_letter)) {
			d = (l - 0x10 - 0x27 * is_letter) << 4;
		} else {
			zend_string_free(str);
			return NULL;
		}
		c = old[j++];
		l = c & ~0x20;
		is_letter = ((unsigned int) ((l - 'A') ^ (l - 'F' - 1))) >> (8 * sizeof(unsigned int) - 1);
		if (EXPECTED((((c ^ '0') - 10) >> (8 * sizeof(unsigned int) - 1)) | is_letter)) {
			d |= l - 0x10 - 0x27 * is_letter;
		} else {
			zend_string_free(str);
			return NULL;
		}
		ret[i] = d;
	}
	ret[i] = '\0';

	return str;
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

/*  cur->locinfo is struct __crt_locale_info which implementation is
	hidden in vc14. TODO revisit this and check if a workaround available
	and needed. */
#if defined(PHP_WIN32) && _MSC_VER < 1900 && defined(ZTS)
	{
		/* Even with the enabled per thread locale, localeconv
			won't check any locale change in the master thread. */
		_locale_t cur = _get_current_locale();

		res = cur->locinfo->lconv;
	}
#else
	/* localeconv doesn't return an error condition */
	res = localeconv();
#endif

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
	zend_string *result;
	zend_string *data;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &data) == FAILURE) {
		return;
	}

	result = php_bin2hex((unsigned char *)ZSTR_VAL(data), ZSTR_LEN(data));

	if (!result) {
		RETURN_FALSE;
	}

	RETURN_STR(result);
}
/* }}} */

/* {{{ proto string hex2bin(string data)
   Converts the hex representation of data to binary */
PHP_FUNCTION(hex2bin)
{
	zend_string *result, *data;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &data) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(data) % 2 != 0) {
		php_error_docref(NULL, E_WARNING, "Hexadecimal input string must have an even length");
		RETURN_FALSE;
	}

	result = php_hex2bin((unsigned char *)ZSTR_VAL(data), ZSTR_LEN(data));

	if (!result) {
		php_error_docref(NULL, E_WARNING, "Input string must be hexadecimal string");
		RETURN_FALSE;
	}

	RETVAL_STR(result);
}
/* }}} */

static void php_spn_common_handler(INTERNAL_FUNCTION_PARAMETERS, int behavior) /* {{{ */
{
	zend_string *s11, *s22;
	zend_long start = 0, len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|ll", &s11,
				&s22, &start, &len) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() < 4) {
		len = ZSTR_LEN(s11);
	}

	/* look at substr() function for more information */

	if (start < 0) {
		start += (zend_long)ZSTR_LEN(s11);
		if (start < 0) {
			start = 0;
		}
	} else if ((size_t)start > ZSTR_LEN(s11)) {
		RETURN_FALSE;
	}

	if (len < 0) {
		len += (ZSTR_LEN(s11) - start);
		if (len < 0) {
			len = 0;
		}
	}

	if (len > (zend_long)ZSTR_LEN(s11) - start) {
		len = ZSTR_LEN(s11) - start;
	}

	if(len == 0) {
		RETURN_LONG(0);
	}

	if (behavior == STR_STRSPN) {
		RETURN_LONG(php_strspn(ZSTR_VAL(s11) + start /*str1_start*/,
						ZSTR_VAL(s22) /*str2_start*/,
						ZSTR_VAL(s11) + start + len /*str1_end*/,
						ZSTR_VAL(s22) + ZSTR_LEN(s22) /*str2_end*/));
	} else if (behavior == STR_STRCSPN) {
		RETURN_LONG(php_strcspn(ZSTR_VAL(s11) + start /*str1_start*/,
						ZSTR_VAL(s22) /*str2_start*/,
						ZSTR_VAL(s11) + start + len /*str1_end*/,
						ZSTR_VAL(s22) + ZSTR_LEN(s22) /*str2_end*/));
	}

}
/* }}} */

/* {{{ proto int strspn(string str, string mask [, int start [, int len]])
   Finds length of initial segment consisting entirely of characters found in mask. If start or/and length is provided works like strspn(substr($s,$start,$len),$good_chars) */
PHP_FUNCTION(strspn)
{
	php_spn_common_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, STR_STRSPN);
}
/* }}} */

/* {{{ proto int strcspn(string str, string mask [, int start [, int len]])
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
	zend_long item;
	char *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &item) == FAILURE) {
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
			php_error_docref(NULL, E_WARNING, "Item '" ZEND_LONG_FMT "' is not valid", item);
			RETURN_FALSE;
	}
	/* }}} */

	value = nl_langinfo(item);
	if (value == NULL) {
		RETURN_FALSE;
	} else {
		RETURN_STRING(value);
	}
}
#endif
/* }}} */

#ifdef HAVE_STRCOLL
/* {{{ proto int strcoll(string str1, string str2)
   Compares two strings using the current locale */
PHP_FUNCTION(strcoll)
{
	zend_string *s1, *s2;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &s1, &s2) == FAILURE) {
		return;
	}

	RETURN_LONG(strcoll((const char *) ZSTR_VAL(s1),
	                    (const char *) ZSTR_VAL(s2)));
}
/* }}} */
#endif

/* {{{ php_charmask
 * Fills a 256-byte bytemask with input. You can specify a range like 'a..z',
 * it needs to be incrementing.
 * Returns: FAILURE/SUCCESS whether the input was correct (i.e. no range errors)
 */
static inline int php_charmask(unsigned char *input, size_t len, char *mask)
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
				php_error_docref(NULL, E_WARNING, "Invalid '..'-range, no character to the left of '..'");
				result = FAILURE;
				continue;
			}
			if (input+2 >= end) { /* there is no 'right' char */
				php_error_docref(NULL, E_WARNING, "Invalid '..'-range, no character to the right of '..'");
				result = FAILURE;
				continue;
			}
			if (input[-1] > input[2]) { /* wrong order */
				php_error_docref(NULL, E_WARNING, "Invalid '..'-range, '..'-range needs to be incrementing");
				result = FAILURE;
				continue;
			}
			/* FIXME: better error (a..b..c is the only left possibility?) */
			php_error_docref(NULL, E_WARNING, "Invalid '..'-range");
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
PHPAPI zend_string *php_trim(zend_string *str, char *what, size_t what_len, int mode)
{
	const char *c = ZSTR_VAL(str);
	size_t len = ZSTR_LEN(str);
	register size_t i;
	size_t trimmed = 0;
	char mask[256];

	if (what) {
		if (what_len == 1) {
			char p = *what;
			if (mode & 1) {
				for (i = 0; i < len; i++) {
					if (c[i] == p) {
						trimmed++;
					} else {
						break;
					}
				}
				len -= trimmed;
				c += trimmed;
			}
			if (mode & 2) {
				if (len > 0) {
					i = len - 1;
					do {
						if (c[i] == p) {
							len--;
						} else {
							break;
						}
					} while (i-- != 0);
				}
			}
		} else {
			php_charmask((unsigned char*)what, what_len, mask);

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
				if (len > 0) {
					i = len - 1;
					do {
						if (mask[(unsigned char)c[i]]) {
							len--;
						} else {
							break;
						}
					} while (i-- != 0);
				}
			}
		}
	} else {
		if (mode & 1) {
			for (i = 0; i < len; i++) {
				if ((unsigned char)c[i] <= ' ' &&
				    (c[i] == ' ' || c[i] == '\n' || c[i] == '\r' || c[i] == '\t' || c[i] == '\v' || c[i] == '\0')) {
					trimmed++;
				} else {
					break;
				}
			}
			len -= trimmed;
			c += trimmed;
		}
		if (mode & 2) {
			if (len > 0) {
				i = len - 1;
				do {
					if ((unsigned char)c[i] <= ' ' &&
					    (c[i] == ' ' || c[i] == '\n' || c[i] == '\r' || c[i] == '\t' || c[i] == '\v' || c[i] == '\0')) {
						len--;
					} else {
						break;
					}
				} while (i-- != 0);
			}
		}
	}

	if (ZSTR_LEN(str) == len) {
		return zend_string_copy(str);
	} else {
		return zend_string_init(c, len, 0);
	}
}
/* }}} */

/* {{{ php_do_trim
 * Base for trim(), rtrim() and ltrim() functions.
 */
static void php_do_trim(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	zend_string *str;
	zend_string *what = NULL;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &str, &what) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(what)
	ZEND_PARSE_PARAMETERS_END();
#endif

	ZVAL_STR(return_value, php_trim(str, (what ? ZSTR_VAL(what) : NULL), (what ? ZSTR_LEN(what) : 0), mode));
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
	zend_string *text;
	char *breakchar = "\n";
	size_t newtextlen, chk, breakchar_len = 1;
	size_t alloced;
	zend_long current = 0, laststart = 0, lastspace = 0;
	zend_long linelength = 75;
	zend_bool docut = 0;
	zend_string *newtext;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|lsb", &text, &linelength, &breakchar, &breakchar_len, &docut) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(text) == 0) {
		RETURN_EMPTY_STRING();
	}

	if (breakchar_len == 0) {
		php_error_docref(NULL, E_WARNING, "Break string cannot be empty");
		RETURN_FALSE;
	}

	if (linelength == 0 && docut) {
		php_error_docref(NULL, E_WARNING, "Can't force cut when width is zero");
		RETURN_FALSE;
	}

	/* Special case for a single-character break as it needs no
	   additional storage space */
	if (breakchar_len == 1 && !docut) {
		newtext = zend_string_init(ZSTR_VAL(text), ZSTR_LEN(text), 0);

		laststart = lastspace = 0;
		for (current = 0; current < ZSTR_LEN(text); current++) {
			if (ZSTR_VAL(text)[current] == breakchar[0]) {
				laststart = lastspace = current + 1;
			} else if (ZSTR_VAL(text)[current] == ' ') {
				if (current - laststart >= linelength) {
					ZSTR_VAL(newtext)[current] = breakchar[0];
					laststart = current + 1;
				}
				lastspace = current;
			} else if (current - laststart >= linelength && laststart != lastspace) {
				ZSTR_VAL(newtext)[lastspace] = breakchar[0];
				laststart = lastspace + 1;
			}
		}

		RETURN_NEW_STR(newtext);
	} else {
		/* Multiple character line break or forced cut */
		if (linelength > 0) {
			chk = (size_t)(ZSTR_LEN(text)/linelength + 1);
			newtext = zend_string_safe_alloc(chk, breakchar_len, ZSTR_LEN(text), 0);
			alloced = ZSTR_LEN(text) + chk * breakchar_len + 1;
		} else {
			chk = ZSTR_LEN(text);
			alloced = ZSTR_LEN(text) * (breakchar_len + 1) + 1;
			newtext = zend_string_safe_alloc(ZSTR_LEN(text), breakchar_len + 1, 0, 0);
		}

		/* now keep track of the actual new text length */
		newtextlen = 0;

		laststart = lastspace = 0;
		for (current = 0; current < ZSTR_LEN(text); current++) {
			if (chk <= 0) {
				alloced += (size_t) (((ZSTR_LEN(text) - current + 1)/linelength + 1) * breakchar_len) + 1;
				newtext = zend_string_extend(newtext, alloced, 0);
				chk = (size_t) ((ZSTR_LEN(text) - current)/linelength) + 1;
			}
			/* when we hit an existing break, copy to new buffer, and
			 * fix up laststart and lastspace */
			if (ZSTR_VAL(text)[current] == breakchar[0]
				&& current + breakchar_len < ZSTR_LEN(text)
				&& !strncmp(ZSTR_VAL(text) + current, breakchar, breakchar_len)) {
				memcpy(ZSTR_VAL(newtext) + newtextlen, ZSTR_VAL(text) + laststart, current - laststart + breakchar_len);
				newtextlen += current - laststart + breakchar_len;
				current += breakchar_len - 1;
				laststart = lastspace = current + 1;
				chk--;
			}
			/* if it is a space, check if it is at the line boundary,
			 * copy and insert a break, or just keep track of it */
			else if (ZSTR_VAL(text)[current] == ' ') {
				if (current - laststart >= linelength) {
					memcpy(ZSTR_VAL(newtext) + newtextlen, ZSTR_VAL(text) + laststart, current - laststart);
					newtextlen += current - laststart;
					memcpy(ZSTR_VAL(newtext) + newtextlen, breakchar, breakchar_len);
					newtextlen += breakchar_len;
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
				memcpy(ZSTR_VAL(newtext) + newtextlen, ZSTR_VAL(text) + laststart, current - laststart);
				newtextlen += current - laststart;
				memcpy(ZSTR_VAL(newtext) + newtextlen, breakchar, breakchar_len);
				newtextlen += breakchar_len;
				laststart = lastspace = current;
				chk--;
			}
			/* if the current word puts us over the linelength, copy
			 * back up until the last space, insert a break, and move
			 * up the laststart */
			else if (current - laststart >= linelength
					&& laststart < lastspace) {
				memcpy(ZSTR_VAL(newtext) + newtextlen, ZSTR_VAL(text) + laststart, lastspace - laststart);
				newtextlen += lastspace - laststart;
				memcpy(ZSTR_VAL(newtext) + newtextlen, breakchar, breakchar_len);
				newtextlen += breakchar_len;
				laststart = lastspace = lastspace + 1;
				chk--;
			}
		}

		/* copy over any stragglers */
		if (laststart != current) {
			memcpy(ZSTR_VAL(newtext) + newtextlen, ZSTR_VAL(text) + laststart, current - laststart);
			newtextlen += current - laststart;
		}

		ZSTR_VAL(newtext)[newtextlen] = '\0';
		/* free unused memory */
		newtext = zend_string_truncate(newtext, newtextlen, 0);

		RETURN_NEW_STR(newtext);
	}
}
/* }}} */

/* {{{ php_explode
 */
PHPAPI void php_explode(const zend_string *delim, zend_string *str, zval *return_value, zend_long limit)
{
	char *p1 = ZSTR_VAL(str);
	char *endp = ZSTR_VAL(str) + ZSTR_LEN(str);
	char *p2 = (char *) php_memnstr(ZSTR_VAL(str), ZSTR_VAL(delim), ZSTR_LEN(delim), endp);
	zval  tmp;

	if (p2 == NULL) {
		ZVAL_STR_COPY(&tmp, str);
		zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &tmp);
	} else {
		do {
			ZVAL_STRINGL(&tmp, p1, p2 - p1);
			zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &tmp);
			p1 = p2 + ZSTR_LEN(delim);
			p2 = (char *) php_memnstr(p1, ZSTR_VAL(delim), ZSTR_LEN(delim), endp);
		} while (p2 != NULL && --limit > 1);

		if (p1 <= endp) {
			ZVAL_STRINGL(&tmp, p1, endp - p1);
			zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &tmp);
		}
	}
}
/* }}} */

/* {{{ php_explode_negative_limit
 */
PHPAPI void php_explode_negative_limit(const zend_string *delim, zend_string *str, zval *return_value, zend_long limit)
{
#define EXPLODE_ALLOC_STEP 64
	char *p1 = ZSTR_VAL(str);
	char *endp = ZSTR_VAL(str) + ZSTR_LEN(str);
	char *p2 = (char *) php_memnstr(ZSTR_VAL(str), ZSTR_VAL(delim), ZSTR_LEN(delim), endp);
	zval  tmp;

	if (p2 == NULL) {
		/*
		do nothing since limit <= -1, thus if only one chunk - 1 + (limit) <= 0
		by doing nothing we return empty array
		*/
	} else {
		size_t allocated = EXPLODE_ALLOC_STEP, found = 0;
		zend_long i, to_return;
		char **positions = emalloc(allocated * sizeof(char *));

		positions[found++] = p1;
		do {
			if (found >= allocated) {
				allocated = found + EXPLODE_ALLOC_STEP;/* make sure we have enough memory */
				positions = erealloc(positions, allocated*sizeof(char *));
			}
			positions[found++] = p1 = p2 + ZSTR_LEN(delim);
			p2 = (char *) php_memnstr(p1, ZSTR_VAL(delim), ZSTR_LEN(delim), endp);
		} while (p2 != NULL);

		to_return = limit + found;
		/* limit is at least -1 therefore no need of bounds checking : i will be always less than found */
		for (i = 0; i < to_return; i++) { /* this checks also for to_return > 0 */
			ZVAL_STRINGL(&tmp, positions[i], (positions[i+1] - ZSTR_LEN(delim)) - positions[i]);
			zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &tmp);
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
	zend_string *str, *delim;
	zend_long limit = ZEND_LONG_MAX; /* No limit */
	zval tmp;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|l", &delim, &str, &limit) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(delim)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(limit)
	ZEND_PARSE_PARAMETERS_END();
#endif

	if (ZSTR_LEN(delim) == 0) {
		php_error_docref(NULL, E_WARNING, "Empty delimiter");
		RETURN_FALSE;
	}

	array_init(return_value);

	if (ZSTR_LEN(str) == 0) {
	  	if (limit >= 0) {
			ZVAL_EMPTY_STRING(&tmp);
			zend_hash_index_add_new(Z_ARRVAL_P(return_value), 0, &tmp);
		}
		return;
	}

	if (limit > 1) {
		php_explode(delim, str, return_value, limit);
	} else if (limit < 0) {
		php_explode_negative_limit(delim, str, return_value, limit);
	} else {
		ZVAL_STR_COPY(&tmp, str);
		zend_hash_index_add_new(Z_ARRVAL_P(return_value), 0, &tmp);
	}
}
/* }}} */

/* {{{ proto string join(array src, string glue)
   An alias for implode */
/* }}} */

/* {{{ php_implode
 */
PHPAPI void php_implode(const zend_string *delim, zval *arr, zval *return_value)
{
	zval         *tmp;
	int           numelems;
	zend_string  *str;
	char         *cptr;
	size_t        len = 0;
	zend_string **strings, **strptr;

	numelems = zend_hash_num_elements(Z_ARRVAL_P(arr));

	if (numelems == 0) {
		RETURN_EMPTY_STRING();
	} else if (numelems == 1) {
		/* loop to search the first not undefined element... */
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(arr), tmp) {
			RETURN_STR(zval_get_string(tmp));
		} ZEND_HASH_FOREACH_END();
	}

	strings = emalloc((sizeof(zend_long) + sizeof(zend_string *)) * numelems);
	strptr = strings - 1;

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(arr), tmp) {
		if (Z_TYPE_P(tmp) == IS_LONG) {
			zend_long val = Z_LVAL_P(tmp);

			*++strptr = NULL;
			((zend_long *) (strings + numelems))[strptr - strings] = Z_LVAL_P(tmp);
			if (val <= 0) {
				len++;
			}
			while (val) {
				val /= 10;
				len++;
			}
		} else {
			*++strptr = zval_get_string(tmp);
			len += ZSTR_LEN(*strptr);
		}
	} ZEND_HASH_FOREACH_END();
	/* numelems can not be 0, we checked above */
	str = zend_string_safe_alloc(numelems - 1, ZSTR_LEN(delim), len, 0);
	cptr = ZSTR_VAL(str) + ZSTR_LEN(str);
	*cptr = 0;

	do {
		if (*strptr) {
			cptr -= ZSTR_LEN(*strptr);
			memcpy(cptr, ZSTR_VAL(*strptr), ZSTR_LEN(*strptr));
			zend_string_release(*strptr);
		} else {
			char *oldPtr = cptr;
			char oldVal = *cptr;
			zend_long val = ((zend_long *) (strings + numelems))[strptr - strings];
			cptr = zend_print_long_to_buf(cptr, val);
			*oldPtr = oldVal;
		}

		cptr -= ZSTR_LEN(delim);
		memcpy(cptr, ZSTR_VAL(delim), ZSTR_LEN(delim));
	} while (--strptr > strings);

	if (*strptr) {
		memcpy(ZSTR_VAL(str), ZSTR_VAL(*strptr), ZSTR_LEN(*strptr));
		zend_string_release(*strptr);
	} else {
		char *oldPtr = cptr;
		char oldVal = *cptr;
		zend_print_long_to_buf(cptr, ((zend_long *) (strings + numelems))[strptr - strings]);
		*oldPtr = oldVal;
	}

	efree(strings);
	RETURN_NEW_STR(str);
}
/* }}} */

/* {{{ proto string implode([string glue,] array pieces)
   Joins array elements placing glue string between items and return one string */
PHP_FUNCTION(implode)
{
	zval *arg1, *arg2 = NULL, *arr;
	zend_string *delim;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &arg1, &arg2) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(arg1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(arg2)
	ZEND_PARSE_PARAMETERS_END();
#endif

	if (arg2 == NULL) {
		if (Z_TYPE_P(arg1) != IS_ARRAY) {
			php_error_docref(NULL, E_WARNING, "Argument must be an array");
			return;
		}

		delim = ZSTR_EMPTY_ALLOC();
		arr = arg1;
	} else {
		if (Z_TYPE_P(arg1) == IS_ARRAY) {
			delim = zval_get_string(arg2);
			arr = arg1;
		} else if (Z_TYPE_P(arg2) == IS_ARRAY) {
			delim = zval_get_string(arg1);
			arr = arg2;
		} else {
			php_error_docref(NULL, E_WARNING, "Invalid arguments passed");
			return;
		}
	}

	php_implode(delim, arr, return_value);
	zend_string_release(delim);
}
/* }}} */

#define STRTOK_TABLE(p) BG(strtok_table)[(unsigned char) *p]

/* {{{ proto string strtok([string str,] string token)
   Tokenize a string */
PHP_FUNCTION(strtok)
{
	zend_string *str, *tok = NULL;
	char *token;
	char *token_end;
	char *p;
	char *pe;
	size_t skipped = 0;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &str, &tok) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(tok)
	ZEND_PARSE_PARAMETERS_END();
#endif

	if (ZEND_NUM_ARGS() == 1) {
		tok = str;
	} else {
		zval_ptr_dtor(&BG(strtok_zval));
		ZVAL_STRINGL(&BG(strtok_zval), ZSTR_VAL(str), ZSTR_LEN(str));
		BG(strtok_last) = BG(strtok_string) = Z_STRVAL(BG(strtok_zval));
		BG(strtok_len) = ZSTR_LEN(str);
	}

	p = BG(strtok_last); /* Where we start to search */
	pe = BG(strtok_string) + BG(strtok_len);

	if (!p || p >= pe) {
		RETURN_FALSE;
	}

	token = ZSTR_VAL(tok);
	token_end = token + ZSTR_LEN(tok);

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
		RETVAL_STRINGL(BG(strtok_last) + skipped, (p - BG(strtok_last)) - skipped);
		BG(strtok_last) = p + 1;
	} else {
		RETVAL_FALSE;
		BG(strtok_last) = NULL;
	}

	/* Restore table -- usually faster then memset'ing the table on every invocation */
restore:
	token = ZSTR_VAL(tok);

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

/* {{{ php_string_toupper
 */
PHPAPI zend_string *php_string_toupper(zend_string *s)
{
	unsigned char *c, *e;

	c = (unsigned char *)ZSTR_VAL(s);
	e = c + ZSTR_LEN(s);

	while (c < e) {
		if (!isupper(*c)) {
			register unsigned char *r;
			zend_string *res = zend_string_alloc(ZSTR_LEN(s), 0);

			if (c != (unsigned char*)ZSTR_VAL(s)) {
				memcpy(ZSTR_VAL(res), ZSTR_VAL(s), c - (unsigned char*)ZSTR_VAL(s));
			}
			r = c + (ZSTR_VAL(res) - ZSTR_VAL(s));
			while (c < e) {
				*r = toupper(*c);
				r++;
				c++;
			}
			*r = '\0';
			return res;
		}
		c++;
	}
	return zend_string_copy(s);
}
/* }}} */

/* {{{ proto string strtoupper(string str)
   Makes a string uppercase */
PHP_FUNCTION(strtoupper)
{
	zend_string *arg;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &arg) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(arg)
	ZEND_PARSE_PARAMETERS_END();
#endif

	RETURN_STR(php_string_toupper(arg));
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

/* {{{ php_string_tolower
 */
PHPAPI zend_string *php_string_tolower(zend_string *s)
{
	unsigned char *c, *e;

	c = (unsigned char *)ZSTR_VAL(s);
	e = c + ZSTR_LEN(s);

	while (c < e) {
		if (!islower(*c)) {
			register unsigned char *r;
			zend_string *res = zend_string_alloc(ZSTR_LEN(s), 0);

			if (c != (unsigned char*)ZSTR_VAL(s)) {
				memcpy(ZSTR_VAL(res), ZSTR_VAL(s), c - (unsigned char*)ZSTR_VAL(s));
			}
			r = c + (ZSTR_VAL(res) - ZSTR_VAL(s));
			while (c < e) {
				*r = tolower(*c);
				r++;
				c++;
			}
			*r = '\0';
			return res;
		}
		c++;
	}
	return zend_string_copy(s);
}
/* }}} */

/* {{{ proto string strtolower(string str)
   Makes a string lowercase */
PHP_FUNCTION(strtolower)
{
	zend_string *str;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();
#endif

	RETURN_STR(php_string_tolower(str));
}
/* }}} */

/* {{{ php_basename
 */
PHPAPI zend_string *php_basename(const char *s, size_t len, char *suffix, size_t sufflen)
{
	char *c, *comp, *cend;
	size_t inc_len, cnt;
	int state;
	zend_string *ret;

	c = comp = cend = (char*)s;
	cnt = len;
	state = 0;
	while (cnt > 0) {
		inc_len = (*c == '\0' ? 1 : php_mblen(c, cnt));

		switch (inc_len) {
			case -2:
			case -1:
				inc_len = 1;
				php_mb_reset();
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
	if (suffix != NULL && sufflen < (size_t)(cend - comp) &&
			memcmp(cend - sufflen, suffix, sufflen) == 0) {
		cend -= sufflen;
	}

	len = cend - comp;

	ret = zend_string_init(comp, len, 0);
	return ret;
}
/* }}} */

/* {{{ proto string basename(string path [, string suffix])
   Returns the filename component of the path */
PHP_FUNCTION(basename)
{
	char *string, *suffix = NULL;
	size_t   string_len, suffix_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s", &string, &string_len, &suffix, &suffix_len) == FAILURE) {
		return;
	}

	RETURN_STR(php_basename(string, string_len, suffix, suffix_len));
}
/* }}} */

/* {{{ php_dirname
   Returns directory name component of path */
PHPAPI size_t php_dirname(char *path, size_t len)
{
	return zend_dirname(path, len);
}
/* }}} */

/* {{{ proto string dirname(string path[, int levels])
   Returns the directory name component of the path */
PHP_FUNCTION(dirname)
{
	char *str;
	size_t str_len;
	zend_string *ret;
	zend_long levels = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &str, &str_len, &levels) == FAILURE) {
		return;
	}

	ret = zend_string_init(str, str_len, 0);

	if (levels == 1) {
		/* Defaut case */
		ZSTR_LEN(ret) = zend_dirname(ZSTR_VAL(ret), str_len);
	} else if (levels < 1) {
		php_error_docref(NULL, E_WARNING, "Invalid argument, levels must be >= 1");
		zend_string_free(ret);
		return;
	} else {
		/* Some levels up */
		do {
			ZSTR_LEN(ret) = zend_dirname(ZSTR_VAL(ret), str_len = ZSTR_LEN(ret));
		} while (ZSTR_LEN(ret) < str_len && --levels);
	}

	RETURN_NEW_STR(ret);
}
/* }}} */

/* {{{ proto array pathinfo(string path[, int options])
   Returns information about a certain string */
PHP_FUNCTION(pathinfo)
{
	zval tmp;
	char *path, *dirname;
	size_t path_len;
	int have_basename;
	zend_long opt = PHP_PATHINFO_ALL;
	zend_string *ret = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &path, &path_len, &opt) == FAILURE) {
		return;
	}

	have_basename = ((opt & PHP_PATHINFO_BASENAME) == PHP_PATHINFO_BASENAME);

	array_init(&tmp);

	if ((opt & PHP_PATHINFO_DIRNAME) == PHP_PATHINFO_DIRNAME) {
		dirname = estrndup(path, path_len);
		php_dirname(dirname, path_len);
		if (*dirname) {
			add_assoc_string(&tmp, "dirname", dirname);
		}
		efree(dirname);
	}

	if (have_basename) {
		ret = php_basename(path, path_len, NULL, 0);
		add_assoc_str(&tmp, "basename", zend_string_copy(ret));
	}

	if ((opt & PHP_PATHINFO_EXTENSION) == PHP_PATHINFO_EXTENSION) {
		const char *p;
		ptrdiff_t idx;

		if (!have_basename) {
			ret = php_basename(path, path_len, NULL, 0);
		}

		p = zend_memrchr(ZSTR_VAL(ret), '.', ZSTR_LEN(ret));

		if (p) {
			idx = p - ZSTR_VAL(ret);
			add_assoc_stringl(&tmp, "extension", ZSTR_VAL(ret) + idx + 1, ZSTR_LEN(ret) - idx - 1);
		}
	}

	if ((opt & PHP_PATHINFO_FILENAME) == PHP_PATHINFO_FILENAME) {
		const char *p;
		ptrdiff_t idx;

		/* Have we already looked up the basename? */
		if (!have_basename && !ret) {
			ret = php_basename(path, path_len, NULL, 0);
		}

		p = zend_memrchr(ZSTR_VAL(ret), '.', ZSTR_LEN(ret));

		idx = p ? (p - ZSTR_VAL(ret)) : ZSTR_LEN(ret);
		add_assoc_stringl(&tmp, "filename", ZSTR_VAL(ret), idx);
	}

	if (ret) {
		zend_string_release(ret);
	}

	if (opt == PHP_PATHINFO_ALL) {
		ZVAL_COPY_VALUE(return_value, &tmp);
	} else {
		zval *element;
		if ((element = zend_hash_get_current_data(Z_ARRVAL(tmp))) != NULL) {
			ZVAL_DEREF(element);
			ZVAL_COPY(return_value, element);
		} else {
			ZVAL_EMPTY_STRING(return_value);
		}
		zval_ptr_dtor(&tmp);
	}
}
/* }}} */

/* {{{ php_stristr
   case insensitve strstr */
PHPAPI char *php_stristr(char *s, char *t, size_t s_len, size_t t_len)
{
	php_strtolower(s, s_len);
	php_strtolower(t, t_len);
	return (char*)php_memnstr(s, t, t_len, s + s_len);
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
static int php_needle_char(zval *needle, char *target)
{
	switch (Z_TYPE_P(needle)) {
		case IS_LONG:
			*target = (char)Z_LVAL_P(needle);
			return SUCCESS;
		case IS_NULL:
		case IS_FALSE:
			*target = '\0';
			return SUCCESS;
		case IS_TRUE:
			*target = '\1';
			return SUCCESS;
		case IS_DOUBLE:
			*target = (char)(int)Z_DVAL_P(needle);
			return SUCCESS;
		case IS_OBJECT:
			*target = (char) zval_get_long(needle);
			return SUCCESS;
		default:
			php_error_docref(NULL, E_WARNING, "needle is not a string or an integer");
			return FAILURE;
	}
}
/* }}} */

/* {{{ proto string stristr(string haystack, string needle[, bool part])
   Finds first occurrence of a string within another, case insensitive */
PHP_FUNCTION(stristr)
{
	zval *needle;
	zend_string *haystack;
	char *found = NULL;
	size_t  found_offset;
	char *haystack_dup;
	char needle_char[2];
	zend_bool part = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz|b", &haystack, &needle, &part) == FAILURE) {
		return;
	}

	haystack_dup = estrndup(ZSTR_VAL(haystack), ZSTR_LEN(haystack));

	if (Z_TYPE_P(needle) == IS_STRING) {
		char *orig_needle;
		if (!Z_STRLEN_P(needle)) {
			php_error_docref(NULL, E_WARNING, "Empty needle");
			efree(haystack_dup);
			RETURN_FALSE;
		}
		orig_needle = estrndup(Z_STRVAL_P(needle), Z_STRLEN_P(needle));
		found = php_stristr(haystack_dup, orig_needle, ZSTR_LEN(haystack), Z_STRLEN_P(needle));
		efree(orig_needle);
	} else {
		if (php_needle_char(needle, needle_char) != SUCCESS) {
			efree(haystack_dup);
			RETURN_FALSE;
		}
		needle_char[1] = 0;

		found = php_stristr(haystack_dup, needle_char, ZSTR_LEN(haystack), 1);
	}

	if (found) {
		found_offset = found - haystack_dup;
		if (part) {
			RETVAL_STRINGL(ZSTR_VAL(haystack), found_offset);
		} else {
			RETVAL_STRINGL(ZSTR_VAL(haystack) + found_offset, ZSTR_LEN(haystack) - found_offset);
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
	zend_string *haystack;
	char *found = NULL;
	char needle_char[2];
	zend_long found_offset;
	zend_bool part = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz|b", &haystack, &needle, &part) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(needle) == IS_STRING) {
		if (!Z_STRLEN_P(needle)) {
			php_error_docref(NULL, E_WARNING, "Empty needle");
			RETURN_FALSE;
		}

		found = (char*)php_memnstr(ZSTR_VAL(haystack), Z_STRVAL_P(needle), Z_STRLEN_P(needle), ZSTR_VAL(haystack) + ZSTR_LEN(haystack));
	} else {
		if (php_needle_char(needle, needle_char) != SUCCESS) {
			RETURN_FALSE;
		}
		needle_char[1] = 0;

		found = (char*)php_memnstr(ZSTR_VAL(haystack), needle_char, 1, ZSTR_VAL(haystack) + ZSTR_LEN(haystack));
	}

	if (found) {
		found_offset = found - ZSTR_VAL(haystack);
		if (part) {
			RETURN_STRINGL(ZSTR_VAL(haystack), found_offset);
		} else {
			RETURN_STRINGL(found, ZSTR_LEN(haystack) - found_offset);
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
	zend_string *haystack;
	char *found = NULL;
	char  needle_char[2];
	zend_long  offset = 0;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz|l", &haystack, &needle, &offset) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(haystack)
		Z_PARAM_ZVAL(needle)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END();
#endif

	if (offset < 0 || (size_t)offset > ZSTR_LEN(haystack)) {
		php_error_docref(NULL, E_WARNING, "Offset not contained in string");
		RETURN_FALSE;
	}

	if (Z_TYPE_P(needle) == IS_STRING) {
		if (!Z_STRLEN_P(needle)) {
			php_error_docref(NULL, E_WARNING, "Empty needle");
			RETURN_FALSE;
		}

		found = (char*)php_memnstr(ZSTR_VAL(haystack) + offset,
			                Z_STRVAL_P(needle),
			                Z_STRLEN_P(needle),
			                ZSTR_VAL(haystack) + ZSTR_LEN(haystack));
	} else {
		if (php_needle_char(needle, needle_char) != SUCCESS) {
			RETURN_FALSE;
		}
		needle_char[1] = 0;

		found = (char*)php_memnstr(ZSTR_VAL(haystack) + offset,
							needle_char,
							1,
		                    ZSTR_VAL(haystack) + ZSTR_LEN(haystack));
	}

	if (found) {
		RETURN_LONG(found - ZSTR_VAL(haystack));
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
	zend_string *haystack;
	zend_long offset = 0;
	char needle_char[2];
	zval *needle;
	zend_string *needle_dup = NULL, *haystack_dup;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz|l", &haystack, &needle, &offset) == FAILURE) {
		return;
	}

	if (offset < 0 || (size_t)offset > ZSTR_LEN(haystack)) {
		php_error_docref(NULL, E_WARNING, "Offset not contained in string");
		RETURN_FALSE;
	}

	if (ZSTR_LEN(haystack) == 0) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(needle) == IS_STRING) {
		if (Z_STRLEN_P(needle) == 0 || Z_STRLEN_P(needle) > ZSTR_LEN(haystack)) {
			RETURN_FALSE;
		}

		haystack_dup = php_string_tolower(haystack);
		needle_dup = php_string_tolower(Z_STR_P(needle));
		found = (char*)php_memnstr(ZSTR_VAL(haystack_dup) + offset,
				ZSTR_VAL(needle_dup), ZSTR_LEN(needle_dup), ZSTR_VAL(haystack_dup) + ZSTR_LEN(haystack));
	} else {
		if (php_needle_char(needle, needle_char) != SUCCESS) {
			RETURN_FALSE;
		}
		haystack_dup = php_string_tolower(haystack);
		needle_char[0] = tolower(needle_char[0]);
		needle_char[1] = '\0';
		found = (char*)php_memnstr(ZSTR_VAL(haystack_dup) + offset,
							needle_char,
							sizeof(needle_char) - 1,
							ZSTR_VAL(haystack_dup) + ZSTR_LEN(haystack));
	}


	if (found) {
		RETVAL_LONG(found - ZSTR_VAL(haystack_dup));
	} else {
		RETVAL_FALSE;
	}

	zend_string_release(haystack_dup);
	if (needle_dup) {
		zend_string_release(needle_dup);
	}
}
/* }}} */

/* {{{ proto int strrpos(string haystack, string needle [, int offset])
   Finds position of last occurrence of a string within another string */
PHP_FUNCTION(strrpos)
{
	zval *zneedle;
	char *needle;
	zend_string *haystack;
	size_t needle_len;
	zend_long offset = 0;
	char *p, *e, ord_needle[2];
	char *found;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz|l", &haystack, &zneedle, &offset) == FAILURE) {
		RETURN_FALSE;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(haystack)
		Z_PARAM_ZVAL(zneedle)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(offset)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

	if (Z_TYPE_P(zneedle) == IS_STRING) {
		needle = Z_STRVAL_P(zneedle);
		needle_len = Z_STRLEN_P(zneedle);
	} else {
		if (php_needle_char(zneedle, ord_needle) != SUCCESS) {
			RETURN_FALSE;
		}
		ord_needle[1] = '\0';
		needle = ord_needle;
		needle_len = 1;
	}

	if ((ZSTR_LEN(haystack) == 0) || (needle_len == 0)) {
		RETURN_FALSE;
	}

	if (offset >= 0) {
		if ((size_t)offset > ZSTR_LEN(haystack)) {
			php_error_docref(NULL, E_WARNING, "Offset is greater than the length of haystack string");
			RETURN_FALSE;
		}
		p = ZSTR_VAL(haystack) + (size_t)offset;
		e = ZSTR_VAL(haystack) + ZSTR_LEN(haystack);
	} else {
		if (offset < -INT_MAX || (size_t)(-offset) > ZSTR_LEN(haystack)) {
			php_error_docref(NULL, E_WARNING, "Offset is greater than the length of haystack string");
			RETURN_FALSE;
		}
		p = ZSTR_VAL(haystack);
		if (-offset < needle_len) {
			e = ZSTR_VAL(haystack) + ZSTR_LEN(haystack);
		} else {
			e = ZSTR_VAL(haystack) + ZSTR_LEN(haystack) + offset + needle_len;
		}
	}

	if ((found = (char *)zend_memnrstr(p, needle, needle_len, e))) {
		RETURN_LONG(found - ZSTR_VAL(haystack));
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int strripos(string haystack, string needle [, int offset])
   Finds position of last occurrence of a string within another string */
PHP_FUNCTION(strripos)
{
	zval *zneedle;
	zend_string *needle;
	zend_string *haystack;
	zend_long offset = 0;
	char *p, *e;
	char *found;
	zend_string *needle_dup, *haystack_dup, *ord_needle = NULL;
	ALLOCA_FLAG(use_heap);


	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz|l", &haystack, &zneedle, &offset) == FAILURE) {
		RETURN_FALSE;
	}

	ZSTR_ALLOCA_ALLOC(ord_needle, 1, use_heap);
	if (Z_TYPE_P(zneedle) == IS_STRING) {
		needle = Z_STR_P(zneedle);
	} else {
		if (php_needle_char(zneedle, ZSTR_VAL(ord_needle)) != SUCCESS) {
			ZSTR_ALLOCA_FREE(ord_needle, use_heap);
			RETURN_FALSE;
		}
		ZSTR_VAL(ord_needle)[1] = '\0';
		needle = ord_needle;
	}

	if ((ZSTR_LEN(haystack) == 0) || (ZSTR_LEN(needle) == 0)) {
		ZSTR_ALLOCA_FREE(ord_needle, use_heap);
		RETURN_FALSE;
	}

	if (ZSTR_LEN(needle) == 1) {
		/* Single character search can shortcut memcmps
		   Can also avoid tolower emallocs */
		if (offset >= 0) {
			if ((size_t)offset > ZSTR_LEN(haystack)) {
				ZSTR_ALLOCA_FREE(ord_needle, use_heap);
				php_error_docref(NULL, E_WARNING, "Offset is greater than the length of haystack string");
				RETURN_FALSE;
			}
			p = ZSTR_VAL(haystack) + (size_t)offset;
			e = ZSTR_VAL(haystack) + ZSTR_LEN(haystack) - 1;
		} else {
			p = ZSTR_VAL(haystack);
			if (offset < -INT_MAX || (size_t)(-offset) > ZSTR_LEN(haystack)) {
				ZSTR_ALLOCA_FREE(ord_needle, use_heap);
				php_error_docref(NULL, E_WARNING, "Offset is greater than the length of haystack string");
				RETURN_FALSE;
			}
			e = ZSTR_VAL(haystack) + ZSTR_LEN(haystack) + (size_t)offset;
		}
		/* Borrow that ord_needle buffer to avoid repeatedly tolower()ing needle */
		*ZSTR_VAL(ord_needle) = tolower(*ZSTR_VAL(needle));
		while (e >= p) {
			if (tolower(*e) == *ZSTR_VAL(ord_needle)) {
				ZSTR_ALLOCA_FREE(ord_needle, use_heap);
				RETURN_LONG(e - p + (offset > 0 ? offset : 0));
			}
			e--;
		}
		ZSTR_ALLOCA_FREE(ord_needle, use_heap);
		RETURN_FALSE;
	}

	haystack_dup = php_string_tolower(haystack);
	if (offset >= 0) {
		if ((size_t)offset > ZSTR_LEN(haystack)) {
			zend_string_release(haystack_dup);
			ZSTR_ALLOCA_FREE(ord_needle, use_heap);
			php_error_docref(NULL, E_WARNING, "Offset is greater than the length of haystack string");
			RETURN_FALSE;
		}
		p = ZSTR_VAL(haystack_dup) + offset;
		e = ZSTR_VAL(haystack_dup) + ZSTR_LEN(haystack);
	} else {
		if (offset < -INT_MAX || (size_t)(-offset) > ZSTR_LEN(haystack)) {
			zend_string_release(haystack_dup);
			ZSTR_ALLOCA_FREE(ord_needle, use_heap);
			php_error_docref(NULL, E_WARNING, "Offset is greater than the length of haystack string");
			RETURN_FALSE;
		}
		p = ZSTR_VAL(haystack_dup);
		if (-offset < ZSTR_LEN(needle)) {
			e = ZSTR_VAL(haystack_dup) + ZSTR_LEN(haystack);
		} else {
			e = ZSTR_VAL(haystack_dup) + ZSTR_LEN(haystack) + offset + ZSTR_LEN(needle);
		}
	}

	needle_dup = php_string_tolower(needle);
	if ((found = (char *)zend_memnrstr(p, ZSTR_VAL(needle_dup), ZSTR_LEN(needle_dup), e))) {
		RETVAL_LONG(found - ZSTR_VAL(haystack_dup));
		zend_string_release(needle_dup);
		zend_string_release(haystack_dup);
		ZSTR_ALLOCA_FREE(ord_needle, use_heap);
	} else {
		zend_string_release(needle_dup);
		zend_string_release(haystack_dup);
		ZSTR_ALLOCA_FREE(ord_needle, use_heap);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string strrchr(string haystack, string needle)
   Finds the last occurrence of a character in a string within another */
PHP_FUNCTION(strrchr)
{
	zval *needle;
	zend_string *haystack;
	const char *found = NULL;
	zend_long found_offset;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &haystack, &needle) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(needle) == IS_STRING) {
		found = zend_memrchr(ZSTR_VAL(haystack), *Z_STRVAL_P(needle), ZSTR_LEN(haystack));
	} else {
		char needle_chr;
		if (php_needle_char(needle, &needle_chr) != SUCCESS) {
			RETURN_FALSE;
		}

		found = zend_memrchr(ZSTR_VAL(haystack),  needle_chr, ZSTR_LEN(haystack));
	}

	if (found) {
		found_offset = found - ZSTR_VAL(haystack);
		RETURN_STRINGL(found, ZSTR_LEN(haystack) - found_offset);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ php_chunk_split
 */
static zend_string *php_chunk_split(char *src, size_t srclen, char *end, size_t endlen, size_t chunklen)
{
	char *p, *q;
	size_t chunks; /* complete chunks! */
	size_t restlen;
	size_t out_len;
	zend_string *dest;

	chunks = srclen / chunklen;
	restlen = srclen - chunks * chunklen; /* srclen % chunklen */

	if (chunks > INT_MAX - 1) {
		return NULL;
	}
	out_len = chunks + 1;
	if (endlen !=0 && out_len > INT_MAX/endlen) {
		return NULL;
	}
	out_len *= endlen;
	if (out_len > INT_MAX - srclen - 1) {
		return NULL;
	}
	out_len += srclen + 1;

	dest = zend_string_alloc(out_len * sizeof(char), 0);

	for (p = src, q = ZSTR_VAL(dest); p < (src + srclen - chunklen + 1); ) {
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
	ZSTR_LEN(dest) = q - ZSTR_VAL(dest);

	return dest;
}
/* }}} */

/* {{{ proto string chunk_split(string str [, int chunklen [, string ending]])
   Returns split line */
PHP_FUNCTION(chunk_split)
{
	zend_string *str;
	char *end    = "\r\n";
	size_t endlen   = 2;
	zend_long chunklen = 76;
	zend_string *result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|ls", &str, &chunklen, &end, &endlen) == FAILURE) {
		return;
	}

	if (chunklen <= 0) {
		php_error_docref(NULL, E_WARNING, "Chunk length should be greater than zero");
		RETURN_FALSE;
	}

	if ((size_t)chunklen > ZSTR_LEN(str)) {
		/* to maintain BC, we must return original string + ending */
		result = zend_string_safe_alloc(ZSTR_LEN(str), 1, endlen, 0);
		memcpy(ZSTR_VAL(result), ZSTR_VAL(str), ZSTR_LEN(str));
		memcpy(ZSTR_VAL(result) + ZSTR_LEN(str), end, endlen);
		ZSTR_VAL(result)[ZSTR_LEN(result)] = '\0';
		RETURN_NEW_STR(result);
	}

	if (!ZSTR_LEN(str)) {
		RETURN_EMPTY_STRING();
	}

	result = php_chunk_split(ZSTR_VAL(str), ZSTR_LEN(str), end, endlen, (size_t)chunklen);

	if (result) {
		RETURN_STR(result);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string substr(string str, int start [, int length])
   Returns part of a string */
PHP_FUNCTION(substr)
{
	zend_string *str;
	zend_long l = 0, f;
	int argc = ZEND_NUM_ARGS();

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sl|l", &str, &f, &l) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(str)
		Z_PARAM_LONG(f)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(l)
	ZEND_PARSE_PARAMETERS_END();
#endif

	if (argc > 2) {
		if ((l < 0 && (size_t)(-l) > ZSTR_LEN(str))) {
			RETURN_FALSE;
		} else if (l > (zend_long)ZSTR_LEN(str)) {
			l = ZSTR_LEN(str);
		}
	} else {
		l = ZSTR_LEN(str);
	}

	if (f > (zend_long)ZSTR_LEN(str)) {
		RETURN_FALSE;
	} else if (f < 0 && -f > ZSTR_LEN(str)) {
		f = 0;
	}

	if (l < 0 && (l + (zend_long)ZSTR_LEN(str) - f) < 0) {
		RETURN_FALSE;
	}

	/* if "from" position is negative, count start position from the end
	 * of the string
	 */
	if (f < 0) {
		f = (zend_long)ZSTR_LEN(str) + f;
		if (f < 0) {
			f = 0;
		}
	}

	/* if "length" position is negative, set it to the length
	 * needed to stop that many chars from the end of the string
	 */
	if (l < 0) {
		l = ((zend_long)ZSTR_LEN(str) - f) + l;
		if (l < 0) {
			l = 0;
		}
	}

	if (f > (zend_long)ZSTR_LEN(str)) {
		RETURN_FALSE;
	}

	if ((f + l) > (zend_long)ZSTR_LEN(str)) {
		l = ZSTR_LEN(str) - f;
	}

	RETURN_STRINGL(ZSTR_VAL(str) + f, l);
}
/* }}} */

/* {{{ proto mixed substr_replace(mixed str, mixed repl, mixed start [, mixed length])
   Replaces part of a string with another string */
PHP_FUNCTION(substr_replace)
{
	zval *str;
	zval *from;
	zval *len = NULL;
	zval *repl;
	zend_long l = 0;
	zend_long f;
	int argc = ZEND_NUM_ARGS();
	zend_string *result;
	HashPosition from_idx, repl_idx, len_idx;
	zval *tmp_str = NULL, *tmp_from = NULL, *tmp_repl = NULL, *tmp_len= NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zzz|z/", &str, &repl, &from, &len) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(str) != IS_ARRAY) {
		convert_to_string_ex(str);
	}
	if (Z_TYPE_P(repl) != IS_ARRAY) {
		convert_to_string_ex(repl);
	}
	if (Z_TYPE_P(from) != IS_ARRAY) {
		convert_to_long_ex(from);
	}

	if (argc > 3) {
		if (Z_TYPE_P(len) != IS_ARRAY) {
			convert_to_long_ex(len);
			l = zval_get_long(len);
		}
	} else {
		if (Z_TYPE_P(str) != IS_ARRAY) {
			l = Z_STRLEN_P(str);
		}
	}

	if (Z_TYPE_P(str) == IS_STRING) {
		if (
			(argc == 3 && Z_TYPE_P(from) == IS_ARRAY) ||
			(argc == 4 && Z_TYPE_P(from) != Z_TYPE_P(len))
		) {
			php_error_docref(NULL, E_WARNING, "'start' and 'length' should be of same type - numerical or array ");
			RETURN_STR_COPY(Z_STR_P(str));
		}
		if (argc == 4 && Z_TYPE_P(from) == IS_ARRAY) {
			if (zend_hash_num_elements(Z_ARRVAL_P(from)) != zend_hash_num_elements(Z_ARRVAL_P(len))) {
				php_error_docref(NULL, E_WARNING, "'start' and 'length' should have the same number of elements");
				RETURN_STR_COPY(Z_STR_P(str));
			}
		}
	}

	if (Z_TYPE_P(str) != IS_ARRAY) {
		if (Z_TYPE_P(from) != IS_ARRAY) {
			zend_string *repl_str;
			zend_bool repl_release = 0;
			f = Z_LVAL_P(from);

			/* if "from" position is negative, count start position from the end
			 * of the string
			 */
			if (f < 0) {
				f = (zend_long)Z_STRLEN_P(str) + f;
				if (f < 0) {
					f = 0;
				}
			} else if (f > Z_STRLEN_P(str)) {
				f = Z_STRLEN_P(str);
			}
			/* if "length" position is negative, set it to the length
			 * needed to stop that many chars from the end of the string
			 */
			if (l < 0) {
				l = ((zend_long)Z_STRLEN_P(str) - f) + l;
				if (l < 0) {
					l = 0;
				}
			}

			if (l > Z_STRLEN_P(str) || (l < 0 && (size_t)(-l) > Z_STRLEN_P(str))) {
				l = Z_STRLEN_P(str);
			}

			if ((f + l) > (zend_long)Z_STRLEN_P(str)) {
				l = Z_STRLEN_P(str) - f;
			}
			if (Z_TYPE_P(repl) == IS_ARRAY) {
				repl_idx = 0;
				while (repl_idx < Z_ARRVAL_P(repl)->nNumUsed) {
					tmp_repl = &Z_ARRVAL_P(repl)->arData[repl_idx].val;
					if (Z_TYPE_P(tmp_repl) != IS_UNDEF) {
						break;
					}
					repl_idx++;
				}
				if (repl_idx < Z_ARRVAL_P(repl)->nNumUsed) {
					repl_str = zval_get_string(tmp_repl);
					repl_release = 1;
				} else {
					repl_str = STR_EMPTY_ALLOC();
				}
			} else {
				repl_str = Z_STR_P(repl);
			}

			result = zend_string_alloc(Z_STRLEN_P(str) - l + ZSTR_LEN(repl_str), 0);

			memcpy(ZSTR_VAL(result), Z_STRVAL_P(str), f);
			if (ZSTR_LEN(repl_str)) {
				memcpy((ZSTR_VAL(result) + f), ZSTR_VAL(repl_str), ZSTR_LEN(repl_str));
			}
			memcpy((ZSTR_VAL(result) + f + ZSTR_LEN(repl_str)), Z_STRVAL_P(str) + f + l, Z_STRLEN_P(str) - f - l);
			ZSTR_VAL(result)[ZSTR_LEN(result)] = '\0';
			if (repl_release) {
				zend_string_release(repl_str);
			}
			RETURN_NEW_STR(result);
		} else {
			php_error_docref(NULL, E_WARNING, "Functionality of 'start' and 'length' as arrays is not implemented");
			RETURN_STR_COPY(Z_STR_P(str));
		}
	} else { /* str is array of strings */
		zend_string *str_index = NULL;
		size_t result_len;
		zend_ulong num_index;

		array_init(return_value);

		from_idx = len_idx = repl_idx = 0;

		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(str), num_index, str_index, tmp_str) {
			zend_string *orig_str = zval_get_string(tmp_str);

			if (Z_TYPE_P(from) == IS_ARRAY) {
				while (from_idx < Z_ARRVAL_P(from)->nNumUsed) {
					tmp_from = &Z_ARRVAL_P(from)->arData[from_idx].val;
					if (Z_TYPE_P(tmp_from) != IS_UNDEF) {
						break;
					}
					from_idx++;
				}
				if (from_idx < Z_ARRVAL_P(from)->nNumUsed) {
					f = zval_get_long(tmp_from);

					if (f < 0) {
						f = (zend_long)ZSTR_LEN(orig_str) + f;
						if (f < 0) {
							f = 0;
						}
					} else if (f > (zend_long)ZSTR_LEN(orig_str)) {
						f = ZSTR_LEN(orig_str);
					}
					from_idx++;
				} else {
					f = 0;
				}
			} else {
				f = Z_LVAL_P(from);
				if (f < 0) {
					f = (zend_long)ZSTR_LEN(orig_str) + f;
					if (f < 0) {
						f = 0;
					}
				} else if (f > (zend_long)ZSTR_LEN(orig_str)) {
					f = ZSTR_LEN(orig_str);
				}
			}

			if (argc > 3 && Z_TYPE_P(len) == IS_ARRAY) {
				while (len_idx < Z_ARRVAL_P(len)->nNumUsed) {
					tmp_len = &Z_ARRVAL_P(len)->arData[len_idx].val;
					if (Z_TYPE_P(tmp_len) != IS_UNDEF) {
						break;
					}
					len_idx++;
				}
				if (len_idx < Z_ARRVAL_P(len)->nNumUsed) {
					l = zval_get_long(tmp_len);
					len_idx++;
				} else {
					l = ZSTR_LEN(orig_str);
				}
			} else if (argc > 3) {
				l = Z_LVAL_P(len);
			} else {
				l = ZSTR_LEN(orig_str);
			}

			if (l < 0) {
				l = (ZSTR_LEN(orig_str) - f) + l;
				if (l < 0) {
					l = 0;
				}
			}

			if ((f + l) > (zend_long)ZSTR_LEN(orig_str)) {
				l = ZSTR_LEN(orig_str) - f;
			}

			result_len = ZSTR_LEN(orig_str) - l;

			if (Z_TYPE_P(repl) == IS_ARRAY) {
				while (repl_idx < Z_ARRVAL_P(repl)->nNumUsed) {
					tmp_repl = &Z_ARRVAL_P(repl)->arData[repl_idx].val;
					if (Z_TYPE_P(tmp_repl) != IS_UNDEF) {
						break;
					}
					repl_idx++;
				}
				if (repl_idx < Z_ARRVAL_P(repl)->nNumUsed) {
					zend_string *repl_str = zval_get_string(tmp_repl);

					result_len += ZSTR_LEN(repl_str);
					repl_idx++;
					result = zend_string_alloc(result_len, 0);

					memcpy(ZSTR_VAL(result), ZSTR_VAL(orig_str), f);
					memcpy((ZSTR_VAL(result) + f), ZSTR_VAL(repl_str), ZSTR_LEN(repl_str));
					memcpy((ZSTR_VAL(result) + f + ZSTR_LEN(repl_str)), ZSTR_VAL(orig_str) + f + l, ZSTR_LEN(orig_str) - f - l);
					zend_string_release(repl_str);
				} else {
					result = zend_string_alloc(result_len, 0);

					memcpy(ZSTR_VAL(result), ZSTR_VAL(orig_str), f);
					memcpy((ZSTR_VAL(result) + f), ZSTR_VAL(orig_str) + f + l, ZSTR_LEN(orig_str) - f - l);
				}
			} else {
				result_len += Z_STRLEN_P(repl);

				result = zend_string_alloc(result_len, 0);

				memcpy(ZSTR_VAL(result), ZSTR_VAL(orig_str), f);
				memcpy((ZSTR_VAL(result) + f), Z_STRVAL_P(repl), Z_STRLEN_P(repl));
				memcpy((ZSTR_VAL(result) + f + Z_STRLEN_P(repl)), ZSTR_VAL(orig_str) + f + l, ZSTR_LEN(orig_str) - f - l);
			}

			ZSTR_VAL(result)[ZSTR_LEN(result)] = '\0';

			if (str_index) {
				zval tmp;

				ZVAL_NEW_STR(&tmp, result);
				zend_symtable_update(Z_ARRVAL_P(return_value), str_index, &tmp);
			} else {
				add_index_str(return_value, num_index, result);
			}

			zend_string_release(orig_str);
		} ZEND_HASH_FOREACH_END();
	} /* if */
}
/* }}} */

/* {{{ proto string quotemeta(string str)
   Quotes meta characters */
PHP_FUNCTION(quotemeta)
{
	zend_string *old;
	char *old_end;
	char *p, *q;
	char c;
	zend_string *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &old) == FAILURE) {
		return;
	}

	old_end = ZSTR_VAL(old) + ZSTR_LEN(old);

	if (ZSTR_VAL(old) == old_end) {
		RETURN_FALSE;
	}

	str = zend_string_safe_alloc(2, ZSTR_LEN(old), 0, 0);

	for (p = ZSTR_VAL(old), q = ZSTR_VAL(str); p != old_end; p++) {
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

	*q = '\0';

	RETURN_NEW_STR(zend_string_truncate(str, q - ZSTR_VAL(str), 0));
}
/* }}} */

/* {{{ proto int ord(string character)
   Returns ASCII value of character */
PHP_FUNCTION(ord)
{
	char   *str;
	size_t str_len;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &str, &str_len) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(str, str_len)
	ZEND_PARSE_PARAMETERS_END();
#endif

	RETURN_LONG((unsigned char) str[0]);
}
/* }}} */

/* {{{ proto string chr(int ascii)
   Converts ASCII code to a character */
PHP_FUNCTION(chr)
{
	zend_long c;

	if (ZEND_NUM_ARGS() != 1) {
		WRONG_PARAM_COUNT;
	}

#ifndef FAST_ZPP
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "l", &c) == FAILURE) {
		c = 0;
	}
#else
	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_QUIET, 1, 1)
		Z_PARAM_LONG(c)
	ZEND_PARSE_PARAMETERS_END_EX(c = 0);
#endif

	c &= 0xff;
	if (CG(one_char_string)[c]) {
		ZVAL_INTERNED_STR(return_value, CG(one_char_string)[c]);
	} else {
		ZVAL_NEW_STR(return_value, zend_string_alloc(1, 0));
		Z_STRVAL_P(return_value)[0] = (char)c;
		Z_STRVAL_P(return_value)[1] = '\0';
	}
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
	zend_string *str;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();
#endif

	if (!ZSTR_LEN(str)) {
		RETURN_EMPTY_STRING();
	}

	ZVAL_STRINGL(return_value, ZSTR_VAL(str), ZSTR_LEN(str));
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
	zend_string  *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		return;
	}

	if (!ZSTR_LEN(str)) {
		RETURN_EMPTY_STRING();
	}

	ZVAL_STRINGL(return_value, ZSTR_VAL(str), ZSTR_LEN(str));
	php_lcfirst(Z_STRVAL_P(return_value));
}
/* }}} */

/* {{{ proto string ucwords(string str [, string delims])
   Uppercase the first character of every word in a string */
PHP_FUNCTION(ucwords)
{
	zend_string *str;
	char *delims = " \t\r\n\f\v";
	register char *r, *r_end;
	size_t delims_len = 6;
	char mask[256];

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|s", &str, &delims, &delims_len) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(delims, delims_len)
	ZEND_PARSE_PARAMETERS_END();
#endif

	if (!ZSTR_LEN(str)) {
		RETURN_EMPTY_STRING();
	}

	php_charmask((unsigned char *)delims, delims_len, mask);

	ZVAL_STRINGL(return_value, ZSTR_VAL(str), ZSTR_LEN(str));
	r = Z_STRVAL_P(return_value);

	*r = toupper((unsigned char) *r);
	for (r_end = r + Z_STRLEN_P(return_value) - 1; r < r_end; ) {
		if (mask[(unsigned char)*r++]) {
			*r = toupper((unsigned char) *r);
		}
	}
}
/* }}} */

/* {{{ php_strtr
 */
PHPAPI char *php_strtr(char *str, size_t len, char *str_from, char *str_to, size_t trlen)
{
	size_t i;

	if (UNEXPECTED(trlen < 1)) {
		return str;
	} else if (trlen == 1) {
		char ch_from = *str_from;
		char ch_to = *str_to;

		for (i = 0; i < len; i++) {
			if (str[i] == ch_from) {
				str[i] = ch_to;
			}
		}
	} else {
		unsigned char xlat[256], j = 0;

		do { xlat[j] = j; } while (++j != 0);

		for (i = 0; i < trlen; i++) {
			xlat[(size_t)(unsigned char) str_from[i]] = str_to[i];
		}
		
		for (i = 0; i < len; i++) {
			str[i] = xlat[(size_t)(unsigned char) str[i]];
		}
	}

	return str;
}
/* }}} */

/* {{{ php_strtr_ex
 */
static zend_string *php_strtr_ex(zend_string *str, char *str_from, char *str_to, size_t trlen)
{
	zend_string *new_str = NULL;
	size_t i;

	if (UNEXPECTED(trlen < 1)) {
		return zend_string_copy(str);
	} else if (trlen == 1) {
		char ch_from = *str_from;
		char ch_to = *str_to;

		for (i = 0; i < ZSTR_LEN(str); i++) {
			if (ZSTR_VAL(str)[i] == ch_from) {
				new_str = zend_string_alloc(ZSTR_LEN(str), 0);
				memcpy(ZSTR_VAL(new_str), ZSTR_VAL(str), i);
				ZSTR_VAL(new_str)[i] = ch_to;
				break;
			}
		}
		for (; i < ZSTR_LEN(str); i++) {
			ZSTR_VAL(new_str)[i] = (ZSTR_VAL(str)[i] != ch_from) ? ZSTR_VAL(str)[i] : ch_to;
		}
	} else {
		unsigned char xlat[256], j = 0;

		do { xlat[j] = j; } while (++j != 0);

		for (i = 0; i < trlen; i++) {
			xlat[(size_t)(unsigned char) str_from[i]] = str_to[i];
		}

		for (i = 0; i < ZSTR_LEN(str); i++) {
			if (ZSTR_VAL(str)[i] != xlat[(size_t)(unsigned char) ZSTR_VAL(str)[i]]) {
				new_str = zend_string_alloc(ZSTR_LEN(str), 0);
				memcpy(ZSTR_VAL(new_str), ZSTR_VAL(str), i);
				ZSTR_VAL(new_str)[i] = xlat[(size_t)(unsigned char) ZSTR_VAL(str)[i]];
				break;
			}
		}

		for (;i < ZSTR_LEN(str); i++) {
			ZSTR_VAL(new_str)[i] = xlat[(size_t)(unsigned char) ZSTR_VAL(str)[i]];
		}
	}

	if (!new_str) {
		return zend_string_copy(str);
	}

	ZSTR_VAL(new_str)[ZSTR_LEN(new_str)] = 0;
	return new_str;
}
/* }}} */

/* {{{ php_strtr_array */
static void php_strtr_array(zval *return_value, zend_string *input, HashTable *pats)
{
	char *str = ZSTR_VAL(input);
	size_t slen = ZSTR_LEN(input);
	zend_ulong num_key;
	zend_string *str_key;
	size_t len, pos, old_pos;
	int num_keys = 0;
	size_t minlen = 128*1024;
	size_t maxlen = 0;
	HashTable str_hash;
	zval *entry;
	char *key;
	smart_str result = {0};
	zend_ulong bitset[256/sizeof(zend_ulong)];
	zend_ulong *num_bitset;

	/* we will collect all possible key lengths */
	num_bitset = ecalloc((slen + sizeof(zend_ulong)) / sizeof(zend_ulong), sizeof(zend_ulong));
	memset(bitset, 0, sizeof(bitset));

	/* check if original array has numeric keys */
	ZEND_HASH_FOREACH_STR_KEY(pats, str_key) {
		if (UNEXPECTED(!str_key)) {
			num_keys = 1;
		} else {
			len = ZSTR_LEN(str_key);
			if (UNEXPECTED(len < 1)) {
				RETURN_FALSE;
			} else if (UNEXPECTED(len > slen)) {
				/* skip long patterns */
				continue;
			}
			if (len > maxlen) {
				maxlen = len;
			}
			if (len < minlen) {
				minlen = len;
			}
			/* remember possible key length */
			num_bitset[len / sizeof(zend_ulong)] |= Z_UL(1) << (len % sizeof(zend_ulong));
			bitset[((unsigned char)ZSTR_VAL(str_key)[0]) / sizeof(zend_ulong)] |= Z_UL(1) << (((unsigned char)ZSTR_VAL(str_key)[0]) % sizeof(zend_ulong));
		}
	} ZEND_HASH_FOREACH_END();

	if (UNEXPECTED(num_keys)) {
		zend_string *key_used;
		/* we have to rebuild HashTable with numeric keys */
		zend_hash_init(&str_hash, zend_hash_num_elements(pats), NULL, NULL, 0);
		ZEND_HASH_FOREACH_KEY_VAL(pats, num_key, str_key, entry) {
			if (UNEXPECTED(!str_key)) {
				key_used = zend_long_to_str(num_key);
				len = ZSTR_LEN(key_used);
				if (UNEXPECTED(len > slen)) {
					/* skip long patterns */
					continue;
				}
				if (len > maxlen) {
					maxlen = len;
				}
				if (len < minlen) {
					minlen = len;
				}
				/* remember possible key length */
				num_bitset[len / sizeof(zend_ulong)] |= Z_UL(1) << (len % sizeof(zend_ulong));
				bitset[((unsigned char)ZSTR_VAL(key_used)[0]) / sizeof(zend_ulong)] |= Z_UL(1) << (((unsigned char)ZSTR_VAL(key_used)[0]) % sizeof(zend_ulong));
			} else {
				key_used = str_key;
				len = ZSTR_LEN(key_used);
				if (UNEXPECTED(len > slen)) {
					/* skip long patterns */
					continue;
				}
			}
			zend_hash_add(&str_hash, key_used, entry);
			if (UNEXPECTED(!str_key)) {
				zend_string_release(key_used);
			}
		} ZEND_HASH_FOREACH_END();
		pats = &str_hash;
	}

	if (UNEXPECTED(minlen > maxlen)) {
		/* return the original string */
		if (pats == &str_hash) {
			zend_hash_destroy(&str_hash);
		}
		efree(num_bitset);
		RETURN_STR_COPY(input);
	}

	old_pos = pos = 0;
	while (pos <= slen - minlen) {
		key = str + pos;
		if (bitset[((unsigned char)key[0]) / sizeof(zend_ulong)] & (Z_UL(1) << (((unsigned char)key[0]) % sizeof(zend_ulong)))) {
			len = maxlen;
			if (len > slen - pos) {
				len = slen - pos;
			}
			while (len >= minlen) {
				if ((num_bitset[len / sizeof(zend_ulong)] & (Z_UL(1) << (len % sizeof(zend_ulong))))) {
					entry = zend_hash_str_find(pats, key, len);
					if (entry != NULL) {
						zend_string *s = zval_get_string(entry);
						smart_str_appendl(&result, str + old_pos, pos - old_pos);
						smart_str_append(&result, s);
						old_pos = pos + len;
						pos = old_pos - 1;
						zend_string_release(s);
						break;
					}
				}
				len--;
			}
		}
		pos++;
	}

	if (result.s) {
		smart_str_appendl(&result, str + old_pos, slen - old_pos);
		smart_str_0(&result);
		RETVAL_NEW_STR(result.s);
	} else {
		smart_str_free(&result);
		RETVAL_STR_COPY(input);
	}

	if (pats == &str_hash) {
		zend_hash_destroy(&str_hash);
	}
	efree(num_bitset);
}
/* }}} */

/* {{{ php_char_to_str_ex
 */
static zend_string* php_char_to_str_ex(zend_string *str, char from, char *to, size_t to_len, int case_sensitivity, zend_long *replace_count)
{
	zend_string *result;
	size_t char_count = 0;
	char lc_from = 0;
	char *source, *target, *source_end= ZSTR_VAL(str) + ZSTR_LEN(str);

	if (case_sensitivity) {
		char *p = ZSTR_VAL(str), *e = p + ZSTR_LEN(str);
		while ((p = memchr(p, from, (e - p)))) {
			char_count++;
			p++;
		}
	} else {
		lc_from = tolower(from);
		for (source = ZSTR_VAL(str); source < source_end; source++) {
			if (tolower(*source) == lc_from) {
				char_count++;
			}
		}
	}

	if (char_count == 0) {
		return zend_string_copy(str);
	}

	if (to_len > 0) {
		result = zend_string_safe_alloc(char_count, to_len - 1, ZSTR_LEN(str), 0);
	} else {
		result = zend_string_alloc(ZSTR_LEN(str) - char_count, 0);
	}
	target = ZSTR_VAL(result);

	if (case_sensitivity) {
		char *p = ZSTR_VAL(str), *e = p + ZSTR_LEN(str), *s = ZSTR_VAL(str);
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
		for (source = ZSTR_VAL(str); source < source_end; source++) {
			if (tolower(*source) == lc_from) {
				if (replace_count) {
					*replace_count += 1;
				}
				memcpy(target, to, to_len);
				target += to_len;
			} else {
				*target = *source;
				target++;
			}
		}
	}
	*target = 0;
	return result;
}
/* }}} */

/* {{{ php_str_to_str_ex
 */
static zend_string *php_str_to_str_ex(zend_string *haystack,
	char *needle, size_t needle_len, char *str, size_t str_len, zend_long *replace_count)
{
	zend_string *new_str;

	if (needle_len < ZSTR_LEN(haystack)) {
		char *end;
		char *e, *s, *p, *r;

		if (needle_len == str_len) {
			new_str = NULL;
			end = ZSTR_VAL(haystack) + ZSTR_LEN(haystack);
			for (p = ZSTR_VAL(haystack); (r = (char*)php_memnstr(p, needle, needle_len, end)); p = r + needle_len) {
				if (!new_str) {
					new_str = zend_string_init(ZSTR_VAL(haystack), ZSTR_LEN(haystack), 0);
				}
				memcpy(ZSTR_VAL(new_str) + (r - ZSTR_VAL(haystack)), str, str_len);
				(*replace_count)++;
			}
			if (!new_str) {
				goto nothing_todo;
			}
			return new_str;
		} else {
			size_t count = 0;
			char *o = ZSTR_VAL(haystack);
			char *n = needle;
			char *endp = o + ZSTR_LEN(haystack);

			while ((o = (char*)php_memnstr(o, n, needle_len, endp))) {
				o += needle_len;
				count++;
			}
			if (count == 0) {
				/* Needle doesn't occur, shortcircuit the actual replacement. */
				goto nothing_todo;
			}
			if (str_len > needle_len) {
				new_str = zend_string_safe_alloc(count, str_len - needle_len, ZSTR_LEN(haystack), 0);
			} else {
				new_str = zend_string_alloc(count * (str_len - needle_len) + ZSTR_LEN(haystack), 0);
			}

			e = s = ZSTR_VAL(new_str);
			end = ZSTR_VAL(haystack) + ZSTR_LEN(haystack);
			for (p = ZSTR_VAL(haystack); (r = (char*)php_memnstr(p, needle, needle_len, end)); p = r + needle_len) {
				memcpy(e, p, r - p);
				e += r - p;
				memcpy(e, str, str_len);
				e += str_len;
				(*replace_count)++;
			}

			if (p < end) {
				memcpy(e, p, end - p);
				e += end - p;
			}

			*e = '\0';
			return new_str;
		}
	} else if (needle_len > ZSTR_LEN(haystack) || memcmp(ZSTR_VAL(haystack), needle, ZSTR_LEN(haystack))) {
nothing_todo:
		return zend_string_copy(haystack);
	} else {
		new_str = zend_string_init(str, str_len, 0);
		(*replace_count)++;
		return new_str;
	}
}
/* }}} */

/* {{{ php_str_to_str_i_ex
 */
static zend_string *php_str_to_str_i_ex(zend_string *haystack, char *lc_haystack,
	zend_string *needle, char *str, size_t str_len, zend_long *replace_count)
{
	zend_string *new_str = NULL;
	zend_string *lc_needle;

	if (ZSTR_LEN(needle) < ZSTR_LEN(haystack)) {
		char *end;
		char *e, *s, *p, *r;

		if (ZSTR_LEN(needle) == str_len) {
			lc_needle = php_string_tolower(needle);
			end = lc_haystack + ZSTR_LEN(haystack);
			for (p = lc_haystack; (r = (char*)php_memnstr(p, ZSTR_VAL(lc_needle), ZSTR_LEN(lc_needle), end)); p = r + ZSTR_LEN(lc_needle)) {
				if (!new_str) {
					new_str = zend_string_init(ZSTR_VAL(haystack), ZSTR_LEN(haystack), 0);
				}
				memcpy(ZSTR_VAL(new_str) + (r - lc_haystack), str, str_len);
				(*replace_count)++;
			}
			zend_string_release(lc_needle);

			if (!new_str) {
				goto nothing_todo;
			}
			return new_str;
		} else {
			size_t count = 0;
			char *o = lc_haystack;
			char *n;
			char *endp = o + ZSTR_LEN(haystack);

			lc_needle = php_string_tolower(needle);
			n = ZSTR_VAL(lc_needle);

			while ((o = (char*)php_memnstr(o, n, ZSTR_LEN(lc_needle), endp))) {
				o += ZSTR_LEN(lc_needle);
				count++;
			}
			if (count == 0) {
				/* Needle doesn't occur, shortcircuit the actual replacement. */
				zend_string_release(lc_needle);
				goto nothing_todo;
			}
			
			if (str_len > ZSTR_LEN(lc_needle)) {
				new_str = zend_string_safe_alloc(count, str_len - ZSTR_LEN(lc_needle), ZSTR_LEN(haystack), 0);
			} else {
				new_str = zend_string_alloc(count * (str_len - ZSTR_LEN(lc_needle)) + ZSTR_LEN(haystack), 0);
			}

			e = s = ZSTR_VAL(new_str);
			end = lc_haystack + ZSTR_LEN(haystack);

			for (p = lc_haystack; (r = (char*)php_memnstr(p, ZSTR_VAL(lc_needle), ZSTR_LEN(lc_needle), end)); p = r + ZSTR_LEN(lc_needle)) {
				memcpy(e, ZSTR_VAL(haystack) + (p - lc_haystack), r - p);
				e += r - p;
				memcpy(e, str, str_len);
				e += str_len;
				(*replace_count)++;
			}

			if (p < end) {
				memcpy(e, ZSTR_VAL(haystack) + (p - lc_haystack), end - p);
				e += end - p;
			}
			*e = '\0';

			zend_string_release(lc_needle);

			return new_str;
		}
	} else if (ZSTR_LEN(needle) > ZSTR_LEN(haystack)) {
nothing_todo:
		return zend_string_copy(haystack);
	} else {
		lc_needle = php_string_tolower(needle);

		if (memcmp(lc_haystack, ZSTR_VAL(lc_needle), ZSTR_LEN(lc_needle))) {
			zend_string_release(lc_needle);
			goto nothing_todo;
		}
		zend_string_release(lc_needle);

		new_str = zend_string_init(str, str_len, 0);

		(*replace_count)++;
		return new_str;
	}
}
/* }}} */

/* {{{ php_str_to_str
 */
PHPAPI zend_string *php_str_to_str(char *haystack, size_t length, char *needle, size_t needle_len, char *str, size_t str_len)
{
	zend_string *new_str;

	if (needle_len < length) {
		char *end;
		char *e, *s, *p, *r;

		if (needle_len == str_len) {
			new_str = zend_string_init(haystack, length, 0);
			end = ZSTR_VAL(new_str) + length;
			for (p = ZSTR_VAL(new_str); (r = (char*)php_memnstr(p, needle, needle_len, end)); p = r + needle_len) {
				memcpy(r, str, str_len);
			}
			return new_str;
		} else {
			if (str_len < needle_len) {
				new_str = zend_string_alloc(length, 0);
			} else {
				size_t count = 0;
				char *o = haystack;
				char *n = needle;
				char *endp = o + length;

				while ((o = (char*)php_memnstr(o, n, needle_len, endp))) {
					o += needle_len;
					count++;
				}
				if (count == 0) {
					/* Needle doesn't occur, shortcircuit the actual replacement. */
					new_str = zend_string_init(haystack, length, 0);
					return new_str;
				} else {
					if (str_len > needle_len) {
						new_str = zend_string_safe_alloc(count, str_len - needle_len, length, 0);
					} else {
						new_str = zend_string_alloc(count * (str_len - needle_len) + length, 0);
					}
				}
			}

			e = s = ZSTR_VAL(new_str);
			end = haystack + length;
			for (p = haystack; (r = (char*)php_memnstr(p, needle, needle_len, end)); p = r + needle_len) {
				memcpy(e, p, r - p);
				e += r - p;
				memcpy(e, str, str_len);
				e += str_len;
			}

			if (p < end) {
				memcpy(e, p, end - p);
				e += end - p;
			}

			*e = '\0';
			new_str = zend_string_truncate(new_str, e - s, 0);
			return new_str;
		}
	} else if (needle_len > length || memcmp(haystack, needle, length)) {
		new_str = zend_string_init(haystack, length, 0);
		return new_str;
	} else {
		new_str = zend_string_init(str, str_len, 0);

		return new_str;
	}
}
/* }}} */

/* {{{ proto string strtr(string str, string from[, string to])
   Translates characters in str using given translation tables */
PHP_FUNCTION(strtr)
{
	zval *from;
	zend_string *str;
	char *to = NULL;
	size_t to_len = 0;
	int ac = ZEND_NUM_ARGS();

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz|s", &str, &from, &to, &to_len) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(str)
		Z_PARAM_ZVAL(from)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(to, to_len)
	ZEND_PARSE_PARAMETERS_END();
#endif

	if (ac == 2 && Z_TYPE_P(from) != IS_ARRAY) {
		php_error_docref(NULL, E_WARNING, "The second argument is not an array");
		RETURN_FALSE;
	}

	/* shortcut for empty string */
	if (ZSTR_LEN(str) == 0) {
		RETURN_EMPTY_STRING();
	}

	if (ac == 2) {
		HashTable *pats = Z_ARRVAL_P(from);

		if (zend_hash_num_elements(pats) < 1) {
			RETURN_STR_COPY(str);
		} else if (zend_hash_num_elements(pats) == 1) {
			zend_long num_key;
			zend_string *str_key, *replace;
			zval *entry, tmp;

			ZEND_HASH_FOREACH_KEY_VAL(pats, num_key, str_key, entry) {
				ZVAL_UNDEF(&tmp);
				if (UNEXPECTED(!str_key)) {
					ZVAL_LONG(&tmp, num_key);
					convert_to_string(&tmp);
					str_key = Z_STR(tmp);
				}		
				replace = zval_get_string(entry);
				if (ZSTR_LEN(str_key) < 1) {
					RETVAL_STR_COPY(str);
				} else if (ZSTR_LEN(str_key) == 1) {
					RETVAL_STR(php_char_to_str_ex(str,
								ZSTR_VAL(str_key)[0],
								ZSTR_VAL(replace),
								ZSTR_LEN(replace),
								1,
								NULL));
				} else {
					zend_long dummy;
					RETVAL_STR(php_str_to_str_ex(str,
								ZSTR_VAL(str_key), ZSTR_LEN(str_key),
								ZSTR_VAL(replace), ZSTR_LEN(replace), &dummy));
				}
				zend_string_release(replace);
				zval_dtor(&tmp);
				return;
			} ZEND_HASH_FOREACH_END();
		} else {
			php_strtr_array(return_value, str, pats);
		}
	} else {
		convert_to_string_ex(from);

		RETURN_STR(php_strtr_ex(str,
				  Z_STRVAL_P(from),
				  to,
				  MIN(Z_STRLEN_P(from), to_len)));
	}
}
/* }}} */

/* {{{ proto string strrev(string str)
   Reverse a string */
PHP_FUNCTION(strrev)
{
	zend_string *str;
	char *e, *p;
	zend_string *n;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		return;
	}

	n = zend_string_alloc(ZSTR_LEN(str), 0);
	p = ZSTR_VAL(n);

	e = ZSTR_VAL(str) + ZSTR_LEN(str);

	while (--e >= ZSTR_VAL(str)) {
		*p++ = *e;
	}

	*p = '\0';

	RETVAL_NEW_STR(n);
}
/* }}} */

/* {{{ php_similar_str
 */
static void php_similar_str(const char *txt1, size_t len1, const char *txt2, size_t len2, size_t *pos1, size_t *pos2, size_t *max)
{
	char *p, *q;
	char *end1 = (char *) txt1 + len1;
	char *end2 = (char *) txt2 + len2;
	size_t l;

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
static size_t php_similar_char(const char *txt1, size_t len1, const char *txt2, size_t len2)
{
	size_t sum;
	size_t pos1 = 0, pos2 = 0, max;

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
	zend_string *t1, *t2;
	zval *percent = NULL;
	int ac = ZEND_NUM_ARGS();
	size_t sim;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|z/", &t1, &t2, &percent) == FAILURE) {
		return;
	}

	if (ac > 2) {
		convert_to_double_ex(percent);
	}

	if (ZSTR_LEN(t1) + ZSTR_LEN(t2) == 0) {
		if (ac > 2) {
			Z_DVAL_P(percent) = 0;
		}

		RETURN_LONG(0);
	}

	sim = php_similar_char(ZSTR_VAL(t1), ZSTR_LEN(t1), ZSTR_VAL(t2), ZSTR_LEN(t2));

	if (ac > 2) {
		Z_DVAL_P(percent) = sim * 200.0 / (ZSTR_LEN(t1) + ZSTR_LEN(t2));
	}

	RETURN_LONG(sim);
}
/* }}} */

/* {{{ php_stripslashes
 *
 * be careful, this edits the string in-place */
PHPAPI void php_stripslashes(zend_string *str)
{
	char *s, *t;
	size_t l;

	s = ZSTR_VAL(str);
	t = ZSTR_VAL(str);
	l = ZSTR_LEN(str);

	while (l > 0) {
		if (*t == '\\') {
			t++;				/* skip the slash */
			ZSTR_LEN(str)--;
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
	zend_string *str, *what;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &str, &what) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(str) == 0) {
		RETURN_EMPTY_STRING();
	}

	if (ZSTR_LEN(what) == 0) {
		RETURN_STRINGL(ZSTR_VAL(str), ZSTR_LEN(str));
	}

	RETURN_STR(php_addcslashes(str, 0, ZSTR_VAL(what), ZSTR_LEN(what)));
}
/* }}} */

/* {{{ proto string addslashes(string str)
   Escapes single quote, double quotes and backslash characters in a string with backslashes */
PHP_FUNCTION(addslashes)
{
	zend_string *str;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();
#endif

	if (ZSTR_LEN(str) == 0) {
		RETURN_EMPTY_STRING();
	}

	RETURN_STR(php_addslashes(str, 0));
}
/* }}} */

/* {{{ proto string stripcslashes(string str)
   Strips backslashes from a string. Uses C-style conventions */
PHP_FUNCTION(stripcslashes)
{
	zend_string *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		return;
	}

	ZVAL_STRINGL(return_value, ZSTR_VAL(str), ZSTR_LEN(str));
	php_stripcslashes(Z_STR_P(return_value));
}
/* }}} */

/* {{{ proto string stripslashes(string str)
   Strips backslashes from a string */
PHP_FUNCTION(stripslashes)
{
	zend_string *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		return;
	}

	ZVAL_STRINGL(return_value, ZSTR_VAL(str), ZSTR_LEN(str));
	php_stripslashes(Z_STR_P(return_value));
}
/* }}} */

#ifndef HAVE_STRERROR
/* {{{ php_strerror
 */
char *php_strerror(int errnum)
{
	extern int sys_nerr;
	extern char *sys_errlist[];

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
PHPAPI void php_stripcslashes(zend_string *str)
{
	char *source, *target, *end;
	size_t  nlen = ZSTR_LEN(str), i;
	char numtmp[4];

	for (source = (char*)ZSTR_VAL(str), end = source + ZSTR_LEN(str), target = ZSTR_VAL(str); source < end; source++) {
		if (*source == '\\' && source + 1 < end) {
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

	ZSTR_LEN(str) = nlen;
}
/* }}} */

/* {{{ php_addcslashes
 */
PHPAPI zend_string *php_addcslashes(zend_string *str, int should_free, char *what, size_t wlength)
{
	char flags[256];
	char *source, *target;
	char *end;
	char c;
	size_t  newlen;
	zend_string *new_str = zend_string_safe_alloc(4, ZSTR_LEN(str), 0, 0);

	php_charmask((unsigned char *)what, wlength, flags);

	for (source = (char*)ZSTR_VAL(str), end = source + ZSTR_LEN(str), target = ZSTR_VAL(new_str); source < end; source++) {
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
	newlen = target - ZSTR_VAL(new_str);
	if (newlen < ZSTR_LEN(str) * 4) {
		new_str = zend_string_truncate(new_str, newlen, 0);
	}
	if (should_free) {
		zend_string_release(str);
	}
	return new_str;
}
/* }}} */

/* {{{ php_addslashes
 */
PHPAPI zend_string *php_addslashes(zend_string *str, int should_free)
{
	/* maximum string length, worst case situation */
	char *source, *target;
	char *end;
	size_t offset;
	zend_string *new_str;

	if (!str) {
		return ZSTR_EMPTY_ALLOC();
	}

	source = ZSTR_VAL(str);
	end = source + ZSTR_LEN(str);

	while (source < end) {
		switch (*source) {
			case '\0':
			case '\'':
			case '\"':
			case '\\':
				goto do_escape;
			default:
				source++;
				break;
		}
	}

	if (!should_free) {
		return zend_string_copy(str);
	}

	return str;

do_escape:
	offset = source - (char *)ZSTR_VAL(str);
	new_str = zend_string_safe_alloc(2, ZSTR_LEN(str) - offset, offset, 0);
	memcpy(ZSTR_VAL(new_str), ZSTR_VAL(str), offset);
	target = ZSTR_VAL(new_str) + offset;

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
	if (should_free) {
		zend_string_release(str);
	}

	if (ZSTR_LEN(new_str) - (target - ZSTR_VAL(new_str)) > 16) {
		new_str = zend_string_truncate(new_str, target - ZSTR_VAL(new_str), 0);
	} else {
		ZSTR_LEN(new_str) = target - ZSTR_VAL(new_str);
	}

	return new_str;
}
/* }}} */

#define _HEB_BLOCK_TYPE_ENG 1
#define _HEB_BLOCK_TYPE_HEB 2
#define isheb(c)      (((((unsigned char) c) >= 224) && (((unsigned char) c) <= 250)) ? 1 : 0)
#define _isblank(c)   (((((unsigned char) c) == ' '  || ((unsigned char) c) == '\t')) ? 1 : 0)
#define _isnewline(c) (((((unsigned char) c) == '\n' || ((unsigned char) c) == '\r')) ? 1 : 0)

/* {{{ php_str_replace_in_subject
 */
static zend_long php_str_replace_in_subject(zval *search, zval *replace, zval *subject, zval *result, int case_sensitivity)
{
	zval		*search_entry,
				*replace_entry = NULL;
	zend_string	*tmp_result,
				*replace_entry_str = NULL;
	char		*replace_value = NULL;
	size_t		 replace_len = 0;
	zend_long	 replace_count = 0;
	zend_string	*subject_str;
	zend_string *lc_subject_str = NULL;
	uint32_t     replace_idx;

	/* Make sure we're dealing with strings. */
	subject_str = zval_get_string(subject);
	if (ZSTR_LEN(subject_str) == 0) {
		zend_string_release(subject_str);
		ZVAL_EMPTY_STRING(result);
		return 0;
	}

	/* If search is an array */
	if (Z_TYPE_P(search) == IS_ARRAY) {
		/* Duplicate subject string for repeated replacement */
		ZVAL_STR_COPY(result, subject_str);

		if (Z_TYPE_P(replace) == IS_ARRAY) {
			replace_idx = 0;
		} else {
			/* Set replacement value to the passed one */
			replace_value = Z_STRVAL_P(replace);
			replace_len = Z_STRLEN_P(replace);
		}

		/* For each entry in the search array, get the entry */
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(search), search_entry) {
			/* Make sure we're dealing with strings. */
			zend_string *search_str = zval_get_string(search_entry);
			if (ZSTR_LEN(search_str) == 0) {
				if (Z_TYPE_P(replace) == IS_ARRAY) {
					replace_idx++;
				}
				zend_string_release(search_str);
				continue;
			}

			/* If replace is an array. */
			if (Z_TYPE_P(replace) == IS_ARRAY) {
				/* Get current entry */
				while (replace_idx < Z_ARRVAL_P(replace)->nNumUsed) {
					replace_entry = &Z_ARRVAL_P(replace)->arData[replace_idx].val;
					if (Z_TYPE_P(replace_entry) != IS_UNDEF) {
						break;
					}
					replace_idx++;
				}
				if (replace_idx < Z_ARRVAL_P(replace)->nNumUsed) {
					/* Make sure we're dealing with strings. */
					replace_entry_str = zval_get_string(replace_entry);

					/* Set replacement value to the one we got from array */
					replace_value = ZSTR_VAL(replace_entry_str);
					replace_len = ZSTR_LEN(replace_entry_str);

					replace_idx++;
				} else {
					/* We've run out of replacement strings, so use an empty one. */
					replace_value = "";
					replace_len = 0;
				}
			}

			if (ZSTR_LEN(search_str) == 1) {
				zend_long old_replace_count = replace_count;

				tmp_result = php_char_to_str_ex(Z_STR_P(result),
								ZSTR_VAL(search_str)[0],
								replace_value,
								replace_len,
								case_sensitivity,
								&replace_count);
				if (lc_subject_str && replace_count != old_replace_count) {
					zend_string_release(lc_subject_str);
					lc_subject_str = NULL;
				}
			} else if (ZSTR_LEN(search_str) > 1) {
				if (case_sensitivity) {
					tmp_result = php_str_to_str_ex(Z_STR_P(result),
							ZSTR_VAL(search_str), ZSTR_LEN(search_str),
							replace_value, replace_len, &replace_count);
				} else {
					zend_long old_replace_count = replace_count;

					if (!lc_subject_str) {
						lc_subject_str = php_string_tolower(Z_STR_P(result));
					}
					tmp_result = php_str_to_str_i_ex(Z_STR_P(result), ZSTR_VAL(lc_subject_str),
							search_str, replace_value, replace_len, &replace_count);
					if (replace_count != old_replace_count) {
						zend_string_release(lc_subject_str);
						lc_subject_str = NULL;
					}
				}				
			}

			zend_string_release(search_str);

			if (replace_entry_str) {
				zend_string_release(replace_entry_str);
				replace_entry_str = NULL;
			}
			zend_string_release(Z_STR_P(result));
			ZVAL_STR(result, tmp_result);

			if (Z_STRLEN_P(result) == 0) {
				if (lc_subject_str) {
					zend_string_release(lc_subject_str);
				}
				zend_string_release(subject_str);
				return replace_count;
			}
		} ZEND_HASH_FOREACH_END();
		if (lc_subject_str) {
			zend_string_release(lc_subject_str);
		}
	} else {
		ZEND_ASSERT(Z_TYPE_P(search) == IS_STRING);
		if (Z_STRLEN_P(search) == 1) {
			ZVAL_STR(result,
				php_char_to_str_ex(subject_str,
							Z_STRVAL_P(search)[0],
							Z_STRVAL_P(replace),
							Z_STRLEN_P(replace),
							case_sensitivity,
							&replace_count));
		} else if (Z_STRLEN_P(search) > 1) {
			if (case_sensitivity) {
				ZVAL_STR(result, php_str_to_str_ex(subject_str,
						Z_STRVAL_P(search), Z_STRLEN_P(search),
						Z_STRVAL_P(replace), Z_STRLEN_P(replace), &replace_count));
			} else {
				lc_subject_str = php_string_tolower(subject_str);
				ZVAL_STR(result, php_str_to_str_i_ex(subject_str, ZSTR_VAL(lc_subject_str),
						Z_STR_P(search),
						Z_STRVAL_P(replace), Z_STRLEN_P(replace), &replace_count));
				zend_string_release(lc_subject_str);
			}
		} else {
			ZVAL_STR_COPY(result, subject_str);
		}
	}
	zend_string_release(subject_str);
	return replace_count;
}
/* }}} */

/* {{{ php_str_replace_common
 */
static void php_str_replace_common(INTERNAL_FUNCTION_PARAMETERS, int case_sensitivity)
{
	zval *subject, *search, *replace, *subject_entry, *zcount = NULL;
	zval result;
	zend_string *string_key;
	zend_ulong num_key;
	zend_long count = 0;
	int argc = ZEND_NUM_ARGS();

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zzz|z/", &search, &replace, &subject, &zcount) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(3, 4)
		Z_PARAM_ZVAL(search)
		Z_PARAM_ZVAL(replace)
		Z_PARAM_ZVAL(subject)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL_EX(zcount, 0, 1)
	ZEND_PARSE_PARAMETERS_END();
#endif

	/* Make sure we're dealing with strings and do the replacement. */
	if (Z_TYPE_P(search) != IS_ARRAY) {
		convert_to_string_ex(search);
		if (Z_TYPE_P(replace) != IS_STRING) {
			convert_to_string_ex(replace);
		}
	} else if (Z_TYPE_P(replace) != IS_ARRAY) {
		convert_to_string_ex(replace);
	}

	/* if subject is an array */
	if (Z_TYPE_P(subject) == IS_ARRAY) {
		array_init(return_value);

		/* For each subject entry, convert it to string, then perform replacement
		   and add the result to the return_value array. */
		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(subject), num_key, string_key, subject_entry) {
			ZVAL_DEREF(subject_entry);
			if (Z_TYPE_P(subject_entry) != IS_ARRAY && Z_TYPE_P(subject_entry) != IS_OBJECT) {
				count += php_str_replace_in_subject(search, replace, subject_entry, &result, case_sensitivity);
			} else {
				ZVAL_COPY(&result, subject_entry);
			}
			/* Add to return array */
			if (string_key) {
				zend_hash_add_new(Z_ARRVAL_P(return_value), string_key, &result);
			} else {
				zend_hash_index_add_new(Z_ARRVAL_P(return_value), num_key, &result);
			}
		} ZEND_HASH_FOREACH_END();
	} else {	/* if subject is not an array */
		count = php_str_replace_in_subject(search, replace, subject, return_value, case_sensitivity);
	}
	if (argc > 3) {
		zval_ptr_dtor(zcount);
		ZVAL_LONG(zcount, count);
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
	char *heb_str, *tmp, *target;
	size_t block_start, block_end, block_type, block_length, i;
	zend_long max_chars=0;
	size_t begin, end, char_count, orig_begin;
	size_t str_len;
	zend_string *broken_str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &str, &str_len, &max_chars) == FAILURE) {
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
			for (i = block_start+1; i<= block_end+1; i++) {
				*target = str[i-1];
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
			for (i = block_end+1; i >= block_start+1; i--) {
				*target = str[i-1];
				target--;
			}
			block_type = _HEB_BLOCK_TYPE_HEB;
		}
		block_start=block_end+1;
	} while (block_end < str_len-1);


	broken_str = zend_string_alloc(str_len, 0);
	begin = end = str_len-1;
	target = ZSTR_VAL(broken_str);

	while (1) {
		char_count=0;
		while ((!max_chars || (max_chars > 0 && char_count < max_chars)) && begin > 0) {
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
		if (max_chars >= 0 && char_count == max_chars) { /* try to avoid breaking words */
			size_t new_char_count=char_count, new_begin=begin;

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
		RETVAL_STR(php_char_to_str_ex(broken_str, '\n', "<br />\n", 7, 1, NULL));
		zend_string_release(broken_str);
	} else {
		RETURN_NEW_STR(broken_str);
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
	char	*tmp;
	zend_string *str;
	char	*end, *target;
	size_t	repl_cnt = 0;
	zend_bool	is_xhtml = 1;
	zend_string *result;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|b", &str, &is_xhtml) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(is_xhtml)
	ZEND_PARSE_PARAMETERS_END();
#endif

	tmp = ZSTR_VAL(str);
	end = ZSTR_VAL(str) + ZSTR_LEN(str);

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
		RETURN_STR_COPY(str);
	}

	{
		size_t repl_len = is_xhtml ? (sizeof("<br />") - 1) : (sizeof("<br>") - 1);

		result = zend_string_safe_alloc(repl_cnt, repl_len, ZSTR_LEN(str), 0);
		target = ZSTR_VAL(result);
	}

	tmp = ZSTR_VAL(str);
	while (tmp < end) {
		switch (*tmp) {
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

				if ((*tmp == '\r' && *(tmp+1) == '\n') || (*tmp == '\n' && *(tmp+1) == '\r')) {
					*target++ = *tmp++;
				}
				/* lack of a break; is intentional */
			default:
				*target++ = *tmp;
		}

		tmp++;
	}

	*target = '\0';

	RETURN_NEW_STR(result);
}
/* }}} */

/* {{{ proto string strip_tags(string str [, string allowable_tags])
   Strips HTML and PHP tags from a string */
PHP_FUNCTION(strip_tags)
{
	zend_string *buf;
	zend_string *str;
	zval *allow=NULL;
	char *allowed_tags=NULL;
	size_t allowed_tags_len=0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &str, &allow) == FAILURE) {
		return;
	}

	/* To maintain a certain BC, we allow anything for the second parameter and return original string */
	if (allow) {
		convert_to_string(allow);
		allowed_tags = Z_STRVAL_P(allow);
		allowed_tags_len = Z_STRLEN_P(allow);
	}

	buf = zend_string_init(ZSTR_VAL(str), ZSTR_LEN(str), 0);
	ZSTR_LEN(buf) = php_strip_tags_ex(ZSTR_VAL(buf), ZSTR_LEN(str), NULL, allowed_tags, allowed_tags_len, 0);
	RETURN_NEW_STR(buf);
}
/* }}} */

/* {{{ proto string setlocale(mixed category, string locale [, string ...])
   Set locale information */
PHP_FUNCTION(setlocale)
{
	zval *args = NULL;
	zval *plocale;
	zend_string *loc;
	char *retval;
	zend_long cat;
	int num_args, i = 0;
	uint32_t idx;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l+", &cat, &args, &num_args) == FAILURE) {
		return;
	}

#ifdef HAVE_SETLOCALE
	idx = 0;
	while (1) {
		if (Z_TYPE(args[0]) == IS_ARRAY) {
			while (idx < Z_ARRVAL(args[0])->nNumUsed) {
				plocale = &Z_ARRVAL(args[0])->arData[idx].val;
				if (Z_TYPE_P(plocale) != IS_UNDEF) {
					break;
				}
				idx++;
			}
			if (idx >= Z_ARRVAL(args[0])->nNumUsed) {
				break;
			}
		} else {
			plocale = &args[i];
		}

		loc = zval_get_string(plocale);

		if (!strcmp("0", ZSTR_VAL(loc))) {
			zend_string_release(loc);
			loc = NULL;
		} else {
			if (ZSTR_LEN(loc) >= 255) {
				php_error_docref(NULL, E_WARNING, "Specified locale name is too long");
				zend_string_release(loc);
				break;
			}
		}

		retval = php_my_setlocale(cat, loc ? ZSTR_VAL(loc) : NULL);
		zend_update_current_locale();
		if (retval) {
			if (loc) {
				/* Remember if locale was changed */
				size_t len = strlen(retval);

				BG(locale_changed) = 1;
				if (cat == LC_CTYPE || cat == LC_ALL) {
					if (BG(locale_string)) {
						zend_string_release(BG(locale_string));
					}
					if (len == ZSTR_LEN(loc) && !memcmp(ZSTR_VAL(loc), retval, len)) {
						BG(locale_string) = zend_string_copy(loc);
						RETURN_STR(BG(locale_string));
					} else {
						BG(locale_string) = zend_string_init(retval, len, 0);
						zend_string_release(loc);
						RETURN_STR_COPY(BG(locale_string));
					}
				} else if (len == ZSTR_LEN(loc) && !memcmp(ZSTR_VAL(loc), retval, len)) {
					RETURN_STR(loc);
				}
				zend_string_release(loc);
			}
			RETURN_STRING(retval);
		}
		if (loc) {
			zend_string_release(loc);
		}

		if (Z_TYPE(args[0]) == IS_ARRAY) {
			idx++;
		} else {
			if (++i >= num_args) break;
		}
	}

#endif
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
	size_t arglen;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|z/", &arg, &arglen, &arrayArg) == FAILURE) {
		return;
	}

	res = estrndup(arg, arglen);

	if (arrayArg == NULL) {
		zval tmp;
		zend_array *symbol_table = zend_rebuild_symbol_table();

		ZVAL_ARR(&tmp, symbol_table);
		sapi_module.treat_data(PARSE_STRING, res, &tmp);
	} else 	{
		zval ret;

		/* Clear out the array that was passed in. */
		zval_dtor(arrayArg);
		array_init(&ret);
		sapi_module.treat_data(PARSE_STRING, res, &ret);
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
int php_tag_find(char *tag, size_t len, const char *set) {
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

PHPAPI size_t php_strip_tags(char *rbuf, size_t len, int *stateptr, const char *allow, size_t allow_len) /* {{{ */
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
PHPAPI size_t php_strip_tags_ex(char *rbuf, size_t len, int *stateptr, const char *allow, size_t allow_len, zend_bool allow_tag_spaces)
{
	char *tbuf, *buf, *p, *tp, *rp, c, lc;
	int br, depth=0, in_q = 0;
	int state = 0;
	size_t pos, i = 0;
	char *allow_free = NULL;
	const char *allow_actual;
	char is_xml = 0;

	if (stateptr)
		state = *stateptr;

	buf = estrndup(rbuf, len);
	c = *buf;
	lc = '\0';
	p = buf;
	rp = rbuf;
	br = 0;
	if (allow) {
		allow_free = zend_str_tolower_dup_ex(allow, allow_len);
		allow_actual = allow_free ? allow_free : allow;
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
						if (is_xml && *(p -1) == '-') {
							break;
						}
						in_q = state = is_xml = 0;
						if (allow) {
							if (tp - tbuf >= PHP_TAG_BUF_SIZE) {
								pos = tp - tbuf;
								tbuf = erealloc(tbuf, (tp - tbuf) + PHP_TAG_BUF_SIZE + 1);
								tp = tbuf + pos;
							}
							*(tp++) = '>';
							*tp='\0';
							if (php_tag_find(tbuf, tp-tbuf, allow_actual)) {
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

				if (state == 2 && p > buf+4 && strncasecmp(p-4, "<?xm", 4) == 0) {
					state = 1; is_xml=1;
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
	zend_string *str;
	char delim = ',', enc = '"', esc = '\\';
	char *delim_str = NULL, *enc_str = NULL, *esc_str = NULL;
	size_t delim_len = 0, enc_len = 0, esc_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|sss", &str, &delim_str, &delim_len,
		&enc_str, &enc_len, &esc_str, &esc_len) == FAILURE) {
		return;
	}

	delim = delim_len ? delim_str[0] : delim;
	enc = enc_len ? enc_str[0] : enc;
	esc = esc_len ? esc_str[0] : esc;

	php_fgetcsv(NULL, delim, enc, esc, ZSTR_LEN(str), ZSTR_VAL(str), return_value);
}
/* }}} */

/* {{{ proto string str_repeat(string input, int mult)
   Returns the input string repeat mult times */
PHP_FUNCTION(str_repeat)
{
	zend_string		*input_str;		/* Input string */
	zend_long 		mult;			/* Multiplier */
	zend_string	*result;		/* Resulting string */
	size_t		result_len;		/* Length of the resulting string */

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sl", &input_str, &mult) == FAILURE) {
		return;
	}

	if (mult < 0) {
		php_error_docref(NULL, E_WARNING, "Second argument has to be greater than or equal to 0");
		return;
	}

	/* Don't waste our time if it's empty */
	/* ... or if the multiplier is zero */
	if (ZSTR_LEN(input_str) == 0 || mult == 0)
		RETURN_EMPTY_STRING();

	/* Initialize the result string */
	result = zend_string_safe_alloc(ZSTR_LEN(input_str), mult, 0, 0);
	result_len = ZSTR_LEN(input_str) * mult;

	/* Heavy optimization for situations where input string is 1 byte long */
	if (ZSTR_LEN(input_str) == 1) {
		memset(ZSTR_VAL(result), *ZSTR_VAL(input_str), mult);
	} else {
		char *s, *e, *ee;
		ptrdiff_t l=0;
		memcpy(ZSTR_VAL(result), ZSTR_VAL(input_str), ZSTR_LEN(input_str));
		s = ZSTR_VAL(result);
		e = ZSTR_VAL(result) + ZSTR_LEN(input_str);
		ee = ZSTR_VAL(result) + result_len;

		while (e<ee) {
			l = (e-s) < (ee-e) ? (e-s) : (ee-e);
			memmove(e, s, l);
			e += l;
		}
	}

	ZSTR_VAL(result)[result_len] = '\0';

	RETURN_NEW_STR(result);
}
/* }}} */

/* {{{ proto mixed count_chars(string input [, int mode])
   Returns info about what characters are used in input */
PHP_FUNCTION(count_chars)
{
	zend_string *input;
	int chars[256];
	zend_long mymode=0;
	unsigned char *buf;
	int inx;
	char retstr[256];
	size_t retlen=0;
	size_t tmp = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|l", &input, &mymode) == FAILURE) {
		return;
	}

	if (mymode < 0 || mymode > 4) {
		php_error_docref(NULL, E_WARNING, "Unknown mode");
		RETURN_FALSE;
	}

	buf = (unsigned char *) ZSTR_VAL(input);
	memset((void*) chars, 0, sizeof(chars));

	while (tmp < ZSTR_LEN(input)) {
		chars[*buf]++;
		buf++;
		tmp++;
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
		RETURN_STRINGL(retstr, retlen);
	}
}
/* }}} */

/* {{{ php_strnatcmp
 */
static void php_strnatcmp(INTERNAL_FUNCTION_PARAMETERS, int fold_case)
{
	zend_string *s1, *s2;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &s1, &s2) == FAILURE) {
		return;
	}

	RETURN_LONG(strnatcmp_ex(ZSTR_VAL(s1), ZSTR_LEN(s1),
							 ZSTR_VAL(s2), ZSTR_LEN(s2),
							 fold_case));
}
/* }}} */

PHPAPI int string_natural_compare_function_ex(zval *result, zval *op1, zval *op2, zend_bool case_insensitive) /* {{{ */
{
	zend_string *str1 = zval_get_string(op1);
	zend_string *str2 = zval_get_string(op2);

	ZVAL_LONG(result, strnatcmp_ex(ZSTR_VAL(str1), ZSTR_LEN(str1), ZSTR_VAL(str2), ZSTR_LEN(str2), case_insensitive));

	zend_string_release(str1);
	zend_string_release(str2);
	return SUCCESS;
}
/* }}} */

PHPAPI int string_natural_case_compare_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	return string_natural_compare_function_ex(result, op1, op2, 1);
}
/* }}} */

PHPAPI int string_natural_compare_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	return string_natural_compare_function_ex(result, op1, op2, 0);
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
	zval grouping, mon_grouping;
	int len, i;

	/* We don't need no stinkin' parameters... */
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);
	array_init(&grouping);
	array_init(&mon_grouping);

#ifdef HAVE_LOCALECONV
	{
		struct lconv currlocdata;

		localeconv_r( &currlocdata );

		/* Grab the grouping data out of the array */
		len = (int)strlen(currlocdata.grouping);

		for (i = 0; i < len; i++) {
			add_index_long(&grouping, i, currlocdata.grouping[i]);
		}

		/* Grab the monetary grouping data out of the array */
		len = (int)strlen(currlocdata.mon_grouping);

		for (i = 0; i < len; i++) {
			add_index_long(&mon_grouping, i, currlocdata.mon_grouping[i]);
		}

		add_assoc_string(return_value, "decimal_point",     currlocdata.decimal_point);
		add_assoc_string(return_value, "thousands_sep",     currlocdata.thousands_sep);
		add_assoc_string(return_value, "int_curr_symbol",   currlocdata.int_curr_symbol);
		add_assoc_string(return_value, "currency_symbol",   currlocdata.currency_symbol);
		add_assoc_string(return_value, "mon_decimal_point", currlocdata.mon_decimal_point);
		add_assoc_string(return_value, "mon_thousands_sep", currlocdata.mon_thousands_sep);
		add_assoc_string(return_value, "positive_sign",     currlocdata.positive_sign);
		add_assoc_string(return_value, "negative_sign",     currlocdata.negative_sign);
		add_assoc_long(  return_value, "int_frac_digits",   currlocdata.int_frac_digits);
		add_assoc_long(  return_value, "frac_digits",       currlocdata.frac_digits);
		add_assoc_long(  return_value, "p_cs_precedes",     currlocdata.p_cs_precedes);
		add_assoc_long(  return_value, "p_sep_by_space",    currlocdata.p_sep_by_space);
		add_assoc_long(  return_value, "n_cs_precedes",     currlocdata.n_cs_precedes);
		add_assoc_long(  return_value, "n_sep_by_space",    currlocdata.n_sep_by_space);
		add_assoc_long(  return_value, "p_sign_posn",       currlocdata.p_sign_posn);
		add_assoc_long(  return_value, "n_sign_posn",       currlocdata.n_sign_posn);
	}
#else
	/* Ok, it doesn't look like we have locale info floating around, so I guess it
	   wouldn't hurt to just go ahead and return the POSIX locale information?  */

	add_index_long(&grouping, 0, -1);
	add_index_long(&mon_grouping, 0, -1);

	add_assoc_string(return_value, "decimal_point",     "\x2E");
	add_assoc_string(return_value, "thousands_sep",     "");
	add_assoc_string(return_value, "int_curr_symbol",   "");
	add_assoc_string(return_value, "currency_symbol",   "");
	add_assoc_string(return_value, "mon_decimal_point", "\x2E");
	add_assoc_string(return_value, "mon_thousands_sep", "");
	add_assoc_string(return_value, "positive_sign",     "");
	add_assoc_string(return_value, "negative_sign",     "");
	add_assoc_long(  return_value, "int_frac_digits",   CHAR_MAX);
	add_assoc_long(  return_value, "frac_digits",       CHAR_MAX);
	add_assoc_long(  return_value, "p_cs_precedes",     CHAR_MAX);
	add_assoc_long(  return_value, "p_sep_by_space",    CHAR_MAX);
	add_assoc_long(  return_value, "n_cs_precedes",     CHAR_MAX);
	add_assoc_long(  return_value, "n_sep_by_space",    CHAR_MAX);
	add_assoc_long(  return_value, "p_sign_posn",       CHAR_MAX);
	add_assoc_long(  return_value, "n_sign_posn",       CHAR_MAX);
#endif

	zend_hash_str_update(Z_ARRVAL_P(return_value), "grouping", sizeof("grouping")-1, &grouping);
	zend_hash_str_update(Z_ARRVAL_P(return_value), "mon_grouping", sizeof("mon_grouping")-1, &mon_grouping);
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
	zend_long offset = 0, length = 0;
	int ac = ZEND_NUM_ARGS();
	int count = 0;
	size_t haystack_len, needle_len;
	char *p, *endp, cmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|ll", &haystack, &haystack_len, &needle, &needle_len, &offset, &length) == FAILURE) {
		return;
	}

	if (needle_len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty substring");
		RETURN_FALSE;
	}

	p = haystack;
	endp = p + haystack_len;

	if (offset < 0) {
		php_error_docref(NULL, E_WARNING, "Offset should be greater than or equal to 0");
		RETURN_FALSE;
	}

	if ((size_t)offset > haystack_len) {
		php_error_docref(NULL, E_WARNING, "Offset value " ZEND_LONG_FMT " exceeds string length", offset);
		RETURN_FALSE;
	}
	p += offset;

	if (ac == 4) {

		if (length <= 0) {
			php_error_docref(NULL, E_WARNING, "Length should be greater than 0");
			RETURN_FALSE;
		}
		if (length > (haystack_len - offset)) {
			php_error_docref(NULL, E_WARNING, "Length value " ZEND_LONG_FMT " exceeds string length", length);
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
		while ((p = (char*)php_memnstr(p, needle, needle_len, endp))) {
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
	zend_string *input;				/* Input string */
	zend_long pad_length;			/* Length to pad to */

	/* Helper variables */
	size_t num_pad_chars;		/* Number of padding characters (total - input size) */
	char *pad_str = " "; /* Pointer to padding string */
	size_t pad_str_len = 1;
	zend_long   pad_type_val = STR_PAD_RIGHT; /* The padding type value */
	size_t	   i, left_pad=0, right_pad=0;
	zend_string *result = NULL;	/* Resulting string */

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sl|sl", &input, &pad_length, &pad_str, &pad_str_len, &pad_type_val) == FAILURE) {
		return;
	}

	/* If resulting string turns out to be shorter than input string,
	   we simply copy the input and return. */
	if (pad_length < 0  || (size_t)pad_length <= ZSTR_LEN(input)) {
		RETURN_STRINGL(ZSTR_VAL(input), ZSTR_LEN(input));
	}

	if (pad_str_len == 0) {
		php_error_docref(NULL, E_WARNING, "Padding string cannot be empty");
		return;
	}

	if (pad_type_val < STR_PAD_LEFT || pad_type_val > STR_PAD_BOTH) {
		php_error_docref(NULL, E_WARNING, "Padding type has to be STR_PAD_LEFT, STR_PAD_RIGHT, or STR_PAD_BOTH");
		return;
	}

	num_pad_chars = pad_length - ZSTR_LEN(input);
	if (num_pad_chars >= INT_MAX) {
		php_error_docref(NULL, E_WARNING, "Padding length is too long");
		return;
	}

	result = zend_string_safe_alloc(1, ZSTR_LEN(input), num_pad_chars, 0);
	ZSTR_LEN(result) = 0;

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
		ZSTR_VAL(result)[ZSTR_LEN(result)++] = pad_str[i % pad_str_len];

	/* Then we copy the input string. */
	memcpy(ZSTR_VAL(result) + ZSTR_LEN(result), ZSTR_VAL(input), ZSTR_LEN(input));
	ZSTR_LEN(result) += ZSTR_LEN(input);

	/* Finally, we pad on the right. */
	for (i = 0; i < right_pad; i++)
		ZSTR_VAL(result)[ZSTR_LEN(result)++] = pad_str[i % pad_str_len];

	ZSTR_VAL(result)[ZSTR_LEN(result)] = '\0';

	RETURN_NEW_STR(result);
}
/* }}} */

/* {{{ proto mixed sscanf(string str, string format [, string ...])
   Implements an ANSI C compatible sscanf */
PHP_FUNCTION(sscanf)
{
	zval *args = NULL;
	char *str, *format;
	size_t str_len, format_len;
	int result, num_args = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss*", &str, &str_len, &format, &format_len,
		&args, &num_args) == FAILURE) {
		return;
	}

	result = php_sscanf_internal(str, format, num_args, args, 0, return_value);

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
	zend_string *arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &arg) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(arg) == 0) {
		RETURN_EMPTY_STRING();
	} else {
		RETURN_STR(php_strtr_ex(arg, rot13_from, rot13_to, 52));
	}
}
/* }}} */

static void php_string_shuffle(char *str, zend_long len) /* {{{ */
{
	zend_long n_elems, rnd_idx, n_left;
	char temp;
	/* The implementation is stolen from array_data_shuffle       */
	/* Thus the characteristics of the randomization are the same */
	n_elems = len;

	if (n_elems <= 1) {
		return;
	}

	n_left = n_elems;

	while (--n_left) {
		rnd_idx = php_rand();
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
	zend_string *arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &arg) == FAILURE) {
		return;
	}

	RETVAL_STRINGL(ZSTR_VAL(arg), ZSTR_LEN(arg));
	if (Z_STRLEN_P(return_value) > 1) {
		php_string_shuffle(Z_STRVAL_P(return_value), (zend_long) Z_STRLEN_P(return_value));
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
	zend_string *str;
	char *char_list = NULL, *p, *e, *s, ch[256];
	size_t char_list_len = 0, word_count = 0;
	zend_long type = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|ls", &str, &type, &char_list, &char_list_len) == FAILURE) {
		return;
	}

	switch(type) {
		case 1:
		case 2:
			array_init(return_value);
			if (!ZSTR_LEN(str)) {
				return;
			}
			break;
		case 0:
			if (!ZSTR_LEN(str)) {
				RETURN_LONG(0);
			}
			/* nothing to be done */
			break;
		default:
			php_error_docref(NULL, E_WARNING, "Invalid format value " ZEND_LONG_FMT, type);
			RETURN_FALSE;
	}

	if (char_list) {
		php_charmask((unsigned char *)char_list, char_list_len, ch);
	}

	p = ZSTR_VAL(str);
	e = ZSTR_VAL(str) + ZSTR_LEN(str);

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
					add_next_index_stringl(return_value, s, p - s);
					break;
				case 2:
					add_index_stringl(return_value, (s - ZSTR_VAL(str)), s, p - s);
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
	size_t format_len = 0;
	char *format, *p, *e;
	double value;
	zend_bool check = 0;
	zend_string *str;
	ssize_t res_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sd", &format, &format_len, &value) == FAILURE) {
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
			php_error_docref(NULL, E_WARNING, "Only a single %%i or %%n token can be used");
			RETURN_FALSE;
		}
	}

	str = zend_string_safe_alloc(format_len, 1, 1024, 0);
	if ((res_len = strfmon(ZSTR_VAL(str), ZSTR_LEN(str), format, value)) < 0) {
		zend_string_free(str);
		RETURN_FALSE;
	}
	ZSTR_LEN(str) = (size_t)res_len;
	ZSTR_VAL(str)[ZSTR_LEN(str)] = '\0';

	RETURN_NEW_STR(zend_string_truncate(str, ZSTR_LEN(str), 0));
}
/* }}} */
#endif

/* {{{ proto array str_split(string str [, int split_length])
   Convert a string to an array. If split_length is specified, break the string down into chunks each split_length characters long. */
PHP_FUNCTION(str_split)
{
	zend_string *str;
	zend_long split_length = 1;
	char *p;
	size_t n_reg_segments;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|l", &str, &split_length) == FAILURE) {
		return;
	}

	if (split_length <= 0) {
		php_error_docref(NULL, E_WARNING, "The length of each segment must be greater than zero");
		RETURN_FALSE;
	}


	if (0 == ZSTR_LEN(str) || (size_t)split_length >= ZSTR_LEN(str)) {
		array_init_size(return_value, 1);
		add_next_index_stringl(return_value, ZSTR_VAL(str), ZSTR_LEN(str));
		return;
	}

	array_init_size(return_value, (uint32_t)(((ZSTR_LEN(str) - 1) / split_length) + 1));

	n_reg_segments = ZSTR_LEN(str) / split_length;
	p = ZSTR_VAL(str);

	while (n_reg_segments-- > 0) {
		add_next_index_stringl(return_value, p, split_length);
		p += split_length;
	}

	if (p != (ZSTR_VAL(str) + ZSTR_LEN(str))) {
		add_next_index_stringl(return_value, p, (ZSTR_VAL(str) + ZSTR_LEN(str) - p));
	}
}
/* }}} */

/* {{{ proto array strpbrk(string haystack, string char_list)
   Search a string for any of a set of characters */
PHP_FUNCTION(strpbrk)
{
	zend_string *haystack, *char_list;
	char *haystack_ptr, *cl_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &haystack, &char_list) == FAILURE) {
		RETURN_FALSE;
	}

	if (!ZSTR_LEN(char_list)) {
		php_error_docref(NULL, E_WARNING, "The character list cannot be empty");
		RETURN_FALSE;
	}

	for (haystack_ptr = ZSTR_VAL(haystack); haystack_ptr < (ZSTR_VAL(haystack) + ZSTR_LEN(haystack)); ++haystack_ptr) {
		for (cl_ptr = ZSTR_VAL(char_list); cl_ptr < (ZSTR_VAL(char_list) + ZSTR_LEN(char_list)); ++cl_ptr) {
			if (*cl_ptr == *haystack_ptr) {
				RETURN_STRINGL(haystack_ptr, (ZSTR_VAL(haystack) + ZSTR_LEN(haystack) - haystack_ptr));
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
	zend_string *s1, *s2;
	zend_long offset, len=0;
	zend_bool cs=0;
	size_t cmp_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SSl|lb", &s1, &s2, &offset, &len, &cs) == FAILURE) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() >= 4 && len <= 0) {
		if (len == 0) {
			RETURN_LONG(0L);
		} else {
			php_error_docref(NULL, E_WARNING, "The length must be greater than or equal to zero");
			RETURN_FALSE;
		}
	}

	if (offset < 0) {
		offset = ZSTR_LEN(s1) + offset;
		offset = (offset < 0) ? 0 : offset;
	}

	if ((size_t)offset >= ZSTR_LEN(s1)) {
		php_error_docref(NULL, E_WARNING, "The start position cannot exceed initial string length");
		RETURN_FALSE;
	}

	cmp_len = (size_t) (len ? len : MAX(ZSTR_LEN(s2), (ZSTR_LEN(s1) - offset)));

	if (!cs) {
		RETURN_LONG(zend_binary_strncmp(ZSTR_VAL(s1) + offset, (ZSTR_LEN(s1) - offset), ZSTR_VAL(s2), ZSTR_LEN(s2), cmp_len));
	} else {
		RETURN_LONG(zend_binary_strncasecmp_l(ZSTR_VAL(s1) + offset, (ZSTR_LEN(s1) - offset), ZSTR_VAL(s2), ZSTR_LEN(s2), cmp_len));
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
