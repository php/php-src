/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Stig Sæther Bakken <ssb@fast.no>                            |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* Synced with php 3.0 revision 1.193 1999-06-16 [ssb] */

#include <stdio.h>
#include "php.h"
#include "reg.h"
#include "php_string.h"
#include "php_variables.h"
#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif
#ifdef HAVE_LANGINFO_H
# include <langinfo.h>
#endif
#include "scanf.h"
#include "zend_API.h"
#include "zend_execute.h"
#include "php_globals.h"
#include "basic_functions.h"
#include "php_smart_str.h"
#ifdef ZTS
#include "TSRM.h"
#endif

#define STR_PAD_LEFT			0
#define STR_PAD_RIGHT			1
#define STR_PAD_BOTH			2
#define PHP_PATHINFO_DIRNAME 	0
#define PHP_PATHINFO_BASENAME 	1
#define PHP_PATHINFO_EXTENSION 	2

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

	result = (char *) emalloc(oldlen * 2 * sizeof(char) + 1);
	if(!result) {
		return result;
	}
	
	for(i = j = 0; i < oldlen; i++) {
		result[j++] = hexconvtab[old[i] >> 4];
		result[j++] = hexconvtab[old[i] & 15];
	}
	result[j] = '\0';

	if (newlen) 
		*newlen = oldlen * 2 * sizeof(char);

	return result;
}
/* }}} */

#ifdef HAVE_LOCALECONV
/* {{{ localeconv_r
 * glibc's localeconv is not reentrant, so lets make it so ... sorta */
struct lconv *localeconv_r(struct lconv *out)
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
	zval **data;
	char *result;
	size_t newlen;

	if (ZEND_NUM_ARGS() != 1 || 
        zend_get_parameters_ex(1, &data) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(data);

	result = php_bin2hex(Z_STRVAL_PP(data), Z_STRLEN_PP(data), &newlen);
	
	if (!result) {
		RETURN_FALSE;
	}

	RETURN_STRINGL(result, newlen, 0);
}
/* }}} */

/* {{{ proto int strspn(string str, string mask)
   Find length of initial segment consisting entirely of characters found in mask */
PHP_FUNCTION(strspn)
{
	zval **s1, **s2;
	
	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &s1, &s2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(s1);
	convert_to_string_ex(s2);

	RETURN_LONG(php_strspn(Z_STRVAL_PP(s1), Z_STRVAL_PP(s2),
                           Z_STRVAL_PP(s1) + Z_STRLEN_PP(s1),
	                       Z_STRVAL_PP(s2) + Z_STRLEN_PP(s2)));
}
/* }}} */

/* {{{ proto int strcspn(string str, string mask)
   Find length of initial segment consisting entirely of characters not found in mask */
PHP_FUNCTION(strcspn)
{
	zval **s1, **s2;
	
	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &s1, &s2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(s1);
	convert_to_string_ex(s2);

	RETURN_LONG(php_strcspn(Z_STRVAL_PP(s1), Z_STRVAL_PP(s2),
	                        Z_STRVAL_PP(s1) + Z_STRLEN_PP(s1),
	                        Z_STRVAL_PP(s2) + Z_STRLEN_PP(s2)));
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

PHP_FUNCTION(nl_langinfo)
{
	zval **item;
	char *value;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &item) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(item);

	value = nl_langinfo(Z_LVAL_PP(item));
	if (value == NULL)	{
		RETURN_FALSE;
	}
	else	{
		RETURN_STRING(value, 1);
	}
}
#endif

#ifdef HAVE_STRCOLL
/* {{{ proto int strcoll(string str1, string str2)
   Compare two strings using the current locale */
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
 * Returns: FAILURE/SUCCESS wether the input was correct (i.e. no range errors)
 */
int php_charmask(unsigned char *input, int len, char *mask TSRMLS_DC)
{
	unsigned char *end;
	unsigned char c;
	int result = SUCCESS;

	memset(mask, 0, 256);
	for (end=input+len; input<end; input++) {
		c=*input; 
		if (input+3<end && input[1] == '.' && input[2] == '.' 
				&& input[3] >= c) {
			memset(mask+c, 1, input[3] - c + 1);
			input+=3;
		} else if (input+1<end && input[0] == '.' && input[1] == '.') {
			/* Error, try to be as helpful as possible:
			   (a range ending/starting with '.' won't be captured here) */
			if (end-len>=input) { /* there was no 'left' char */
				php_error(E_WARNING, "Invalid '..'-range passed to %s(), no character to the left of '..'", get_active_function_name(TSRMLS_C));
				result = FAILURE;
				continue;
			}
			if (input+2>=end) { /* there is no 'right' char */
				php_error(E_WARNING, "Invalid '..'-range passed to %s(), no character to the right of '..'", get_active_function_name(TSRMLS_C));
				result = FAILURE;
				continue;
			}
			if (input[-1] > input[2]) { /* wrong order */
				php_error(E_WARNING, "Invalid '..'-range passed to %s(), '..'-range needs to be incrementing", get_active_function_name(TSRMLS_C));
				result = FAILURE;
				continue;
			} 
			/* FIXME: better error (a..b..c is the only left possibility?) */
			php_error(E_WARNING, "Invalid '..'-range passed to %s()", get_active_function_name(TSRMLS_C));
			result = FAILURE;
			continue;
		} else {
			mask[c]=1;
		}
	}
	return result;
}
/* }}} */

/* {{{ php_trim
       Compatibility function, ports old-API to new one. (DEPRECATED)
*/
void php_trim(zval *str, zval *return_value, int mode TSRMLS_DC)
{
	php_trim2(str, NULL, return_value, mode TSRMLS_CC);
}
/* }}} */

/* {{{ php_trim2
 */
PHPAPI void php_trim2(zval **str, zval **what, zval *return_value, int mode TSRMLS_DC)
/* mode 1 : trim left
   mode 2 : trim right
   mode 3 : trim left and right

   what indicates which chars are to be trimmed. NULL->default (' \t\n\r\v\0')
*/
{
	register int i;
	int len = Z_STRLEN_PP(str);
	int trimmed = 0;
	char *c = Z_STRVAL_PP(str);
	char mask[256];

	if (what) {
		php_charmask(Z_STRVAL_PP(what), Z_STRLEN_PP(what), mask TSRMLS_CC);
	} else {
		php_charmask(" \n\r\t\v\0", 6, mask TSRMLS_CC);
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
	RETVAL_STRINGL(c, len, 1);
}
/* }}} */

/* {{{ proto string rtrim(string str[, string character_mask])
   An alias for chop */
/* }}} */

/* {{{ proto string chop(string str[, string character_mask])
   Remove trailing whitespace */
PHP_FUNCTION(chop)
{
	zval **str;
	zval **what = NULL;
	int    argc = ZEND_NUM_ARGS();

	if (argc < 1 || argc > 2 ||
	    zend_get_parameters_ex(argc, &str, &what) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);
	if (argc > 1) {
		convert_to_string_ex(what);
	}

	php_trim2(str, what, return_value, 2 TSRMLS_CC);
}
/* }}} */

/* {{{ proto string trim(string str[, string character_mask])
   Strip whitespace from the beginning and end of a string */
PHP_FUNCTION(trim)
{
	zval **str;
	zval **what = NULL;
	int    argc = ZEND_NUM_ARGS();

	if (argc < 1 || argc > 2 ||
	    zend_get_parameters_ex(argc, &str, &what) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);
	if (argc > 1) {
		convert_to_string_ex(what);
	}

	php_trim2(str, what, return_value, 3 TSRMLS_CC);
}
/* }}} */

/* {{{ proto string ltrim(string str[, string character_mask])
   Strip whitespace from the beginning of a string */
PHP_FUNCTION(ltrim)
{
	zval **str;
	zval **what = NULL;
	int    argc = ZEND_NUM_ARGS();

	if (argc < 1 || argc > 2 ||
	    zend_get_parameters_ex(argc, &str, &what) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);
	if (argc > 1) {
		convert_to_string_ex(what);
	}
	
	php_trim2(str, what, return_value, 1 TSRMLS_CC);
}
/* }}} */

/* {{{ proto string wordwrap(string str [, int width [, string break [, int cut]]])
   Wrap buffer to selected number of characters using string break char */
PHP_FUNCTION(wordwrap)
{
	zval **ptext, **plinelength, **pbreakchar, **cut;
	long i = 0, l = 0, pgr = 0, linelength = 0, last = 0, breakcharlen, docut = 0;
	char *text, *breakchar, *newtext; 
	int argc = ZEND_NUM_ARGS();

	if (argc < 1 || argc > 4 || 
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &ptext, &plinelength, &pbreakchar, &cut) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(ptext);
	
	if (Z_STRVAL_PP(ptext) == 0)
		RETURN_FALSE;

	text = Z_STRVAL_PP(ptext);

	if (argc > 1) {
		convert_to_long_ex(plinelength);
		linelength = Z_LVAL_PP(plinelength);
	}
	else {
		linelength = 75;
	}

	if (argc > 2) {
		convert_to_string_ex(pbreakchar);
		breakchar = Z_STRVAL_PP(pbreakchar);
		breakcharlen = Z_STRLEN_PP(pbreakchar);
	}
	else {
		breakchar = "\n";
		breakcharlen = 1;
	}

	if (argc > 3) {
		convert_to_long_ex(cut);
		docut = Z_LVAL_PP(cut);
	}

	/* Special case for a single-character break as it needs no
	   additional storage space */
	if (breakcharlen == 1 && docut == 0) {
		newtext = estrndup(text, Z_STRLEN_PP(ptext));
		while (newtext[i] != '\0') {

			/* prescan line to see if it is greater than linelength */
			l = 0;
			while (newtext[i+l] != breakchar[0]) {
				if (newtext[i+l] == '\0') {
					l--;
					break;
				}
				l++;
			}

			if (l >= linelength) {
				pgr = l;
				l = linelength;

				/* needs breaking; work backwards to find previous word */
				while (l >= 0) {
					if (newtext[i+l] == ' ') {
						newtext[i+l] = breakchar[0];
						break;
					}
					l--;
				}

				if (l == -1) {
					/* couldn't break is backwards, try looking forwards */
					l = linelength;
					while (l <= pgr) {
						if(newtext[i+l] == ' ') {
							newtext[i+l] = breakchar[0];
							break;
						}
						l++;
					}
				}
			}

			i += l + 1;
		}

		RETVAL_STRINGL(newtext, strlen(newtext), 1);
		efree(newtext);
	}
	else {
		/* Multiple character line break */
		newtext = emalloc(Z_STRLEN_PP(ptext) * (breakcharlen + 1) + 1);
		newtext[0] = '\0';

		i = 0;
		while (text[i] != '\0') {

			/* prescan line to see if it is greater than linelength */
			l = 0;
			while (text[i+l] != '\0') {
				if (text[i+l] == breakchar[0]) {
					if (breakcharlen == 1 || !strncmp(text+i+l, breakchar, breakcharlen))
						break;
				}
				l++;
			}

			if (l >= linelength) {
				pgr = l;
				l = linelength;

				/* needs breaking; work backwards to find previous word */
				while (l >= 0) {
					if (text[i+l] == ' ') {
						strncat(newtext, text+last, i+l-last);
						strcat(newtext, breakchar);
						last = i + l + 1;
						break;
					}
					l--;
				}

				if (l == -1) {
					/* couldn't break it backwards, try looking forwards */
					l = linelength - 1;
					while (l <= pgr) {
						if (docut == 0) {
							if (text[i+l] == ' ') {
								strncat(newtext, text+last, i+l-last);
								strcat(newtext, breakchar);
								last = i + l + 1;
								break;
							}
						}
						if (docut == 1) {
							if (text[i+l] == ' ' || l > i-last) {
								strncat(newtext, text+last, i+l-last+1);
								strcat(newtext, breakchar);
								last = i + l + 1;
								break;
							}
						}
						l++;
					}
				}
				i += l + 1;
			}
			else {
				i += (l ? l : 1);
			}
		}

		if (i + l > last) {
			strcat(newtext, text+last);
		}

		RETVAL_STRINGL(newtext, strlen(newtext), 1);
		efree(newtext);
	}
}
/* }}} */

/* {{{ php_explode
 */
PHPAPI void php_explode(zval *delim, zval *str, zval *return_value, int limit) 
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
				 (limit == -1 || --limit > 1));

		if (p1 <= endp)
			add_next_index_stringl(return_value, p1, endp-p1, 1);
	}
}
/* }}} */

/* {{{ proto array explode(string separator, string str [, int limit])
   Split a string on string separator and return array of components */
PHP_FUNCTION(explode)
{
	zval **str, **delim, **zlimit = NULL;
	int limit = -1;
	int argc = ZEND_NUM_ARGS();

	if (argc < 2 || argc > 3 ||
	    zend_get_parameters_ex(argc, &delim, &str, &zlimit) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);
	convert_to_string_ex(delim);

	if (argc > 2) {
		convert_to_long_ex(zlimit);
		limit = Z_LVAL_PP(zlimit);
	}

	if (! Z_STRLEN_PP(delim)) {
		php_error(E_WARNING, "Empty delimiter");
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	if (limit == 0 || limit == 1) {
		add_index_stringl(return_value, 0, Z_STRVAL_PP(str), Z_STRLEN_PP(str), 1);
	} else {
		php_explode(*delim, *str, return_value, limit);
	}
}
/* }}} */

/* {{{ proto string join(array src, string glue)
   An alias for implode */
/* }}} */

/* {{{ php_implode
 */
PHPAPI void php_implode(zval *delim, zval *arr, zval *return_value) 
{
	zval **tmp;
	char *tmp_str;
	int len = 0, count = 0, target = 0;
	HashPosition pos;

	/* convert everything to strings, and calculate length */
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(arr), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(arr), (void **) &tmp, &pos) == SUCCESS) {
		convert_to_string_ex(tmp);
		len += Z_STRLEN_PP(tmp);
		if (count > 0) {
			len += Z_STRLEN_P(delim);
		}

		count++;
		zend_hash_move_forward_ex(Z_ARRVAL_P(arr), &pos);
	}

	/* do it */
	tmp_str = (char *) emalloc(len + 1);
	tmp_str[0] = 0;

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(arr), &pos);
	while (zend_hash_get_current_data_ex(arr->value.ht, (void **) &tmp, &pos) == SUCCESS) {
		count--;
		memcpy(tmp_str + target, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
		target += Z_STRLEN_PP(tmp); 
		if (count > 0) {
			memcpy(tmp_str + target, Z_STRVAL_P(delim), Z_STRLEN_P(delim));
			target += Z_STRLEN_P(delim);
		}
		zend_hash_move_forward_ex(Z_ARRVAL_P(arr), &pos);
	}
	tmp_str[len] = 0;
	
	RETURN_STRINGL(tmp_str, len, 0);
}
/* }}} */

/* {{{ proto string implode(array src, string glue)
   Join array elements placing glue string between items and return one string */
PHP_FUNCTION(implode)
{
	zval **arg1, **arg2, *delim, *arr;
	
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(arg1) == IS_ARRAY) {
		SEPARATE_ZVAL(arg1);
		arr = *arg1;
		convert_to_string_ex(arg2);
		delim = *arg2;
	} else if (Z_TYPE_PP(arg2) == IS_ARRAY) {
		SEPARATE_ZVAL(arg2)
		arr = *arg2;
		convert_to_string_ex(arg1);
		delim = *arg1;
	} else {
		php_error(E_WARNING, "Bad arguments to implode()");
		return;
	}

	php_implode(delim, arr, return_value);
}
/* }}} */

#define STRTOK_TABLE(p) BG(strtok_table)[(unsigned char) *p]	

/* {{{ proto string strtok([string str, ] string token)
   Tokenize a string */
PHP_FUNCTION(strtok)
{
	zval **args[2];
	zval **tok, **str;
	char *token;
	char *token_end;
	char *p;
	char *pe;
	
	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 2 ||
	    zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE)
		WRONG_PARAM_COUNT;
	
	switch (ZEND_NUM_ARGS()) {
	case 1:
		tok = args[0];
		break;
	case 2:
		str = args[0];
		tok = args[1];
		convert_to_string_ex(str);

		zval_add_ref(str);
		if (BG(strtok_zval))
			zval_ptr_dtor(&BG(strtok_zval));
		BG(strtok_zval) = *str;
		
		BG(strtok_last) = BG(strtok_string) = Z_STRVAL_PP(str);
		BG(strtok_len) = Z_STRLEN_PP(str);
		break;
	}
	
	p = BG(strtok_last); /* Where we start to search */
	pe = BG(strtok_string) + BG(strtok_len);

	if (!p || p >= pe)
		RETURN_FALSE;

	convert_to_string_ex(tok);
	
	token = Z_STRVAL_PP(tok);
	token_end = token + Z_STRLEN_PP(tok);

	while (token < token_end) 
		STRTOK_TABLE(token++) = 1;

	/* Skip leading delimiters */
	while (STRTOK_TABLE(p))
		if (++p >= pe) {
			/* no other chars left */
			BG(strtok_last) = NULL;
			RETVAL_FALSE;
			goto restore;
		}
	
	/* We know at this place that *p is no delimiter, so skip it */	
	while (++p < pe)
		if (STRTOK_TABLE(p))
		   goto return_token;	

	if (p - BG(strtok_last)) {
return_token:
		RETVAL_STRINGL(BG(strtok_last), p - BG(strtok_last), 1);
		BG(strtok_last) = p + 1;
	} else {
		RETVAL_FALSE;
		BG(strtok_last) = NULL;
	}

	/* Restore table -- usually faster then memset'ing the table
	   on every invocation */
restore:
	token = Z_STRVAL_PP(tok);
	
	while (token < token_end)
		STRTOK_TABLE(token++) = 0;
}
/* }}} */

/* {{{ php_strtoupper
 */
PHPAPI char *php_strtoupper(char *s, size_t len)
{
	char *c;
	int ch;
	size_t i;

	c = s;
	for (i=0; i<len; i++) {
		ch = toupper((unsigned char)*c);
		*c++ = ch;
	}
	return s;
}
/* }}} */

/* {{{ proto string strtoupper(string str)
   Make a string uppercase */
PHP_FUNCTION(strtoupper)
{
	zval **arg;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg);

	*return_value = **arg;
	zval_copy_ctor(return_value);
	php_strtoupper(Z_STRVAL_P(return_value), Z_STRLEN_P(return_value));
}
/* }}} */

/* {{{ php_strtolower
 */
PHPAPI char *php_strtolower(char *s, size_t len)
{
	register int ch;
	char *c;
	size_t i;

	c = s;
	for (i=0; i<len; i++) {
		ch = tolower((unsigned char)*c);
		*c++ = ch;
	}
	return s;
}
/* }}} */

/* {{{ proto string strtolower(string str)
   Make a string lowercase */
PHP_FUNCTION(strtolower)
{
	zval **str;
	char *ret;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

	*return_value = **str;
	zval_copy_ctor(return_value);
	ret = php_strtolower(Z_STRVAL_P(return_value), Z_STRLEN_P(return_value));
}
/* }}} */

/* {{{ php_basename
 */
PHPAPI char *php_basename(char *s, size_t len, char *suffix, size_t sufflen)
{
	char *ret=NULL, *c, *p=NULL, buf='\0';
	c = s + len - 1;	

	/* do suffix removal as the unix command does */
	if(suffix && (len > sufflen)) {
		if(!strncmp(suffix, c-sufflen+1, sufflen)) {
			c -= sufflen; 
			*(c + 1) = '\0';
		}
	}


	/* strip trailing slashes */
	while (*c == '/'
#ifdef PHP_WIN32
		   || *c == '\\'
#endif
		)
		c--;
	if(c < s+len-1) {
		buf = *(c + 1);  /* Save overwritten char */
		*(c + 1) = '\0'; /* overwrite char */
		p = c + 1;       /* Save pointer to overwritten char */
	}

	if ((c = strrchr(s, '/'))
#ifdef PHP_WIN32
		|| (c = strrchr(s, '\\'))
#endif
		) {
		ret = estrdup(c + 1);
	} else {
		ret = estrdup(s);
	}
	if(buf) *p = buf;
	return (ret);
}
/* }}} */

/* {{{ proto string basename(string path [, string suffix])
   Return the filename component of the path */
PHP_FUNCTION(basename)
{
	zval **str, **suffix;
	char *ret;
	char *suffixp = NULL;
	int   suffix_len = 0;
	int   argc = ZEND_NUM_ARGS();

	if (argc < 1 || argc > 2 ||
	    zend_get_parameters_ex(argc, &str, &suffix) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (argc > 1) {
		convert_to_string_ex(suffix);
		suffixp = Z_STRVAL_PP(suffix);
		suffix_len = Z_STRLEN_PP(suffix);
	}

	ret = php_basename(Z_STRVAL_PP(str), Z_STRLEN_PP(str), suffixp, suffix_len);	
	RETURN_STRING(ret, 0);
}
/* }}} */

/* {{{ php_dirname
 *
 * This function doesn't work with absolute paths in Win32 such as C:\foo
 *  (and it didn't before either). This needs to be fixed
 */
PHPAPI void php_dirname(char *path, int len)
{
	register char *end = path + len - 1;

	if (len <= 0) {
		/* Illegal use of this function */
		return;
	}

	/* Strip trailing slashes */
	while (end >= path && IS_SLASH(*end)) {
		end--;
	}
	if (end < path) {
		/* The path only contained slashes */
		path[0] = DEFAULT_SLASH;
		path[1] = '\0';
		return;
	}

	/* Strip filename */
	while (end >= path && !IS_SLASH(*end)) {
		end--;
	}
	if (end < path) {
		/* No slash found, therefore return '.' */
		path[0] = '.';
		path[1] = '\0';
		return;
	}

	/* Strip slashes which came before the file name */
	while (end >= path && IS_SLASH(*end)) {
		end--;
	}
	if (end < path) {
		path[0] = DEFAULT_SLASH;
		path[1] = '\0';
		return;
	}
	*(end+1) = '\0';
}
/* }}} */

/* {{{ proto string dirname(string path)
   Return the directory name component of the path */
PHP_FUNCTION(dirname)
{
	zval **str;
	char *ret;
	
	if (ZEND_NUM_ARGS() != 1 || 
	    zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

	ret = estrndup(Z_STRVAL_PP(str), Z_STRLEN_PP(str));
	php_dirname(ret, Z_STRLEN_PP(str));

	RETURN_STRING(ret, 0);
}
/* }}} */

/* {{{ proto array pathinfo(string path)
   Return information about a certain string */
PHP_FUNCTION(pathinfo)
{
	zval **path, **uopt, *tmp;
	char *ret;
	int argc = ZEND_NUM_ARGS(), opt, len;
	
	if (argc < 1 || argc > 2 ||
	    zend_get_parameters_ex(argc, &path, &uopt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(path);
	len = Z_STRLEN_PP(path);
	
	if (argc > 1) {
		convert_to_long_ex(uopt);
		opt = Z_LVAL_PP(uopt);
		if (opt < PHP_PATHINFO_DIRNAME || opt > PHP_PATHINFO_EXTENSION) {
			php_error(E_WARNING, "Invalid option in call to pathinfo()");
			RETURN_FALSE;
		}
	}

	MAKE_STD_ZVAL(tmp);
	array_init(tmp);
	
	if (argc < 2 || opt == PHP_PATHINFO_DIRNAME) {
		ret = estrndup(Z_STRVAL_PP(path), len);
		php_dirname(ret, len);
		if (*ret)
			add_assoc_string(tmp, "dirname", ret, 1);
		efree(ret);
	}
	
	if (argc < 2 || opt == PHP_PATHINFO_BASENAME) {
		ret = php_basename(Z_STRVAL_PP(path), len, NULL, 0);
		add_assoc_string(tmp, "basename", ret, 0);
	}			
	
	if (argc < 2 || opt == PHP_PATHINFO_EXTENSION) {
		char *p;
		int idx;

		p = strrchr(Z_STRVAL_PP(path), '.');
		if (p) {
			idx = p - Z_STRVAL_PP(path);
			add_assoc_stringl(tmp, "extension", Z_STRVAL_PP(path) + idx + 1, len - idx - 1, 1);
		}
	}

	if (argc == 2) {
		zval **element;
		zend_hash_get_current_data(Z_ARRVAL_P(tmp), (void **) &element);
		*return_value = **element;
	} else {
		*return_value = *tmp;
	}

	zval_copy_ctor(return_value);
	zval_dtor(tmp);
	efree(tmp);
}
/* }}} */

/* {{{ php_stristr
 * case insensitve strstr */
PHPAPI char *php_stristr(unsigned char *s, unsigned char *t,
                         size_t s_len, size_t t_len)
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
	for (spanp = s2; p != s1_end && spanp != s2_end;)
		if (*spanp++ == c) {
			c = *(++p);
			goto cont;
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
			if (*spanp == c || p == s1_end)
				return p - s1;
		} while (spanp++ < s2_end);
		c = *++p;
	}
	/* NOTREACHED */
}
/* }}} */

/* {{{ proto string stristr(string haystack, string needle)
   Find first occurrence of a string within another, case insensitive */
PHP_FUNCTION(stristr)
{
	zval **haystack, **needle;
	char *found = NULL;
	int  found_offset;
	char *haystack_orig;
	char needle_char[2];
	
	if (ZEND_NUM_ARGS() != 2 || 
	    zend_get_parameters_ex(2, &haystack, &needle) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(haystack);

	if (!Z_STRLEN_PP(needle)) {
		php_error(E_WARNING, "Empty Delimiter");
		RETURN_FALSE;
	}

	haystack_orig = estrndup(Z_STRVAL_PP(haystack), Z_STRLEN_PP(haystack));

	if (Z_TYPE_PP(needle) == IS_STRING) {
		if (!Z_STRLEN_PP(needle)) {
			php_error(E_WARNING, "Empty Delimiter");
			RETURN_FALSE;
		}

		found = php_stristr(Z_STRVAL_PP(haystack), Z_STRVAL_PP(needle),
		                    Z_STRLEN_PP(haystack), Z_STRLEN_PP(needle));
	}
	else {
		convert_to_long_ex(needle);
		needle_char[0] = (char) Z_LVAL_PP(needle);
		needle_char[1] = 0;

		found = php_stristr(Z_STRVAL_PP(haystack), needle_char, 
		                    Z_STRLEN_PP(haystack), 1);
	}
		
	if (found) {
		found_offset = found - Z_STRVAL_PP(haystack);
		RETURN_STRINGL(haystack_orig + found_offset,
		               Z_STRLEN_PP(haystack) - found_offset, 
					   1);
	} else {
		RETURN_FALSE;
	}

	efree(haystack_orig);
}
/* }}} */

/* {{{ proto string strstr(string haystack, string needle)
   Find first occurrence of a string within another */
PHP_FUNCTION(strstr)
{
	zval **haystack, **needle;
	char *found = NULL;
	char needle_char[2];
	
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2, &haystack, &needle) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(haystack);

	if (Z_TYPE_PP(needle) == IS_STRING) {
		if (!Z_STRLEN_PP(needle)) {
			php_error(E_WARNING, "Empty Delimiter");
			RETURN_FALSE;
		}

		found = php_memnstr(Z_STRVAL_PP(haystack), 
	                        Z_STRVAL_PP(needle),
	                        Z_STRLEN_PP(needle), 
	                        Z_STRVAL_PP(haystack) + Z_STRLEN_PP(haystack));
	}
	else {
		convert_to_long_ex(needle);
		needle_char[0] = (char) Z_LVAL_PP(needle);
		needle_char[1] = 0;

		found = php_memnstr(Z_STRVAL_PP(haystack),
		                    needle_char, 1,
		                    Z_STRVAL_PP(haystack) + Z_STRLEN_PP(haystack));
	}

	if (found) {
		RETURN_STRING(found, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string strchr(string haystack, string needle)
   An alias for strstr */
/* }}} */

/* {{{ proto int strpos(string haystack, string needle [, int offset])
   Find position of first occurrence of a string within another */
PHP_FUNCTION(strpos)
{
	zval **haystack, **needle, **z_offset;
	char *found = NULL;
	char  needle_char[2];
	int   offset = 0;
	int   argc = ZEND_NUM_ARGS();

	if (argc < 2 || argc > 3 ||
		zend_get_parameters_ex(argc, &haystack, &needle, &z_offset) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(haystack);

	if (argc > 2) {
		convert_to_long_ex(z_offset);
		offset = Z_LVAL_PP(z_offset);
	}

	if (offset < 0 || offset > Z_STRLEN_PP(haystack)) {
		php_error(E_WARNING, "Offset not contained in string");
		RETURN_FALSE;
	}

	if (Z_TYPE_PP(needle) == IS_STRING) {
		if (!Z_STRLEN_PP(needle)) {
			php_error(E_WARNING, "Empty Delimiter");
			RETURN_FALSE;
		}

		found = php_memnstr(Z_STRVAL_PP(haystack) + offset,
		                    Z_STRVAL_PP(needle),
		                    Z_STRLEN_PP(needle),
		                    Z_STRVAL_PP(haystack) + Z_STRLEN_PP(haystack));
	}
	else {
		convert_to_long_ex(needle);
		needle_char[0] = (char) Z_LVAL_PP(needle);
		needle_char[1] = 0;

		found = php_memnstr(Z_STRVAL_PP(haystack) + offset,
		                    needle_char, 1,
		                    Z_STRVAL_PP(haystack) + Z_STRLEN_PP(haystack));
	}

	if (found) {
		RETURN_LONG(found - Z_STRVAL_PP(haystack));
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int strrpos(string haystack, string needle)
   Find position of last occurrence of a character in a string within another */
PHP_FUNCTION(strrpos)
{
	zval **haystack, **needle;
	char *found = NULL;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &haystack, &needle) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(haystack);

	if (Z_TYPE_PP(needle) == IS_STRING) {
		found = strrchr(Z_STRVAL_PP(haystack), *Z_STRVAL_PP(needle));
	}
	else {
		convert_to_long_ex(needle);
		found = strrchr(Z_STRVAL_PP(haystack), (char) Z_LVAL_PP(needle));
	}

	if (found) {
		RETURN_LONG(Z_STRLEN_PP(haystack) - strlen(found));
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string strrchr(string haystack, string needle)
   Find the last occurrence of a character in a string within another */
PHP_FUNCTION(strrchr)
{
	zval **haystack, **needle;
	char *found = NULL;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &haystack, &needle) ==
		FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(haystack);

	if (Z_TYPE_PP(needle) == IS_STRING) {
		found = strrchr(Z_STRVAL_PP(haystack), *Z_STRVAL_PP(needle));
	}
	else {
		convert_to_long_ex(needle);
		found = strrchr(Z_STRVAL_PP(haystack), (char) Z_LVAL_PP(needle));
	}

	if (found) {
		RETURN_STRING(found, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ php_chunk_split
 */
static char *php_chunk_split(char *src, int srclen, char *end, int endlen,
							 int chunklen, int *destlen)
{
	char *dest;
	char *p, *q;
	int chunks; /* complete chunks! */
	int restlen;

	chunks = srclen / chunklen;
	restlen = srclen - chunks * chunklen; /* srclen % chunklen */

	dest = emalloc((srclen + (chunks + 1) * endlen + 1) * sizeof(char));

	for(p = src, q = dest; p < (src + srclen - chunklen + 1); ) {
		memcpy(q, p, chunklen);
		q += chunklen;
		memcpy(q, end, endlen);
		q += endlen;
		p += chunklen;
	}

	if(restlen) {
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
   Return split line */
PHP_FUNCTION(chunk_split) 
{
	zval **p_str, **p_chunklen, **p_ending;
	char *result;
	char *end    = "\r\n";
	int endlen   = 2;
	int chunklen = 76;
	int result_len;
	int argc = ZEND_NUM_ARGS();

	if (argc < 1 || argc > 3 ||
		zend_get_parameters_ex(argc, &p_str, &p_chunklen, &p_ending) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(p_str);

	if (argc > 1) {
		convert_to_string_ex(p_chunklen);
		chunklen = Z_LVAL_PP(p_chunklen);
	}

	if (argc > 2) {
		convert_to_string_ex(p_ending);
		end = Z_STRVAL_PP(p_ending);
		endlen = Z_STRLEN_PP(p_ending);
	}

	if (chunklen <= 0) {
		php_error(E_WARNING, "Chunk length should be greater than zero");
		RETURN_FALSE;
	}

	if (!Z_STRLEN_PP(p_str)) {
		RETURN_EMPTY_STRING();
	}

	result = php_chunk_split(Z_STRVAL_PP(p_str), Z_STRLEN_PP(p_str),
	                         end, endlen, chunklen, &result_len);
	if (result) {
		RETURN_STRINGL(result, result_len, 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string substr(string str, int start [, int length])
   Return part of a string */
PHP_FUNCTION(substr)
{
	zval **str, **from, **len;
	int l;
	int f;
	int argc = ZEND_NUM_ARGS();

	if (argc < 2 || argc > 3 ||
	    zend_get_parameters_ex(argc, &str, &from, &len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);
	convert_to_string_ex(from);

	if (argc > 2) {
		convert_to_long_ex(len);
		l = Z_LVAL_PP(len);
	}
	else {
		l = Z_STRLEN_PP(str);
	}
	
	f = Z_LVAL_PP(from);

	/* if "from" position is negative, count start position from the end
	 * of the string
	 */
	if (f < 0) {
		f = Z_STRLEN_PP(str) + f;
		if (f < 0) {
			f = 0;
		}
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

	if (f >= Z_STRLEN_PP(str)) {
		RETURN_FALSE;
	}

	if((f + l) > Z_STRLEN_PP(str)) {
		l = Z_STRLEN_PP(str) - f;
	}

	RETURN_STRINGL(Z_STRVAL_PP(str) + f, l, 1);
}
/* }}} */

/* {{{ proto string substr_replace(string str, string repl, int start [, int length])
   Replace part of a string with another string */
PHP_FUNCTION(substr_replace)
{
    zval      **str;
    zval      **from;
    zval      **len;
    zval      **repl;
    char       *result;
	int         result_len;
    int         l;
    int         f;
	int         argc = ZEND_NUM_ARGS();

	if (argc < 3 || argc > 4 ||
	    zend_get_parameters_ex(argc, &str, &repl, &from, &len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);
	convert_to_string_ex(repl);
	convert_to_long_ex(from);
	
	if (argc > 3) {
		convert_to_long_ex(len);
		l = Z_LVAL_PP(len);
	}
	else {
		l = Z_STRLEN_PP(str);
	}
	
	f = Z_LVAL_PP(from);

	/* if "from" position is negative, count start position from the end
	 * of the string
	 */
	if (f < 0) {
		f = Z_STRLEN_PP(str) + f;
		if (f < 0) {
			f = 0;
		}
	} else if (f > Z_STRLEN_PP(str))
		f = Z_STRLEN_PP(str);


	/* if "length" position is negative, set it to the length
	 * needed to stop that many chars from the end of the string
	 */
	if (l < 0) {
		l = (Z_STRLEN_PP(str) - f) + l;
		if (l < 0) {
			l = 0;
		}
	}

	if((f + l) > Z_STRLEN_PP(str)) {
		l = Z_STRLEN_PP(str) - f;
	}

	result_len = Z_STRLEN_PP(str) - l + Z_STRLEN_PP(repl);
	result = ecalloc(result_len + 1, sizeof(char *));

	memcpy(result, Z_STRVAL_PP(str), f);
	memcpy(&result[f], Z_STRVAL_PP(repl), Z_STRLEN_PP(repl));
	memcpy(&result[f + Z_STRLEN_PP(repl)], Z_STRVAL_PP(str) + f + l,
          Z_STRLEN_PP(str) - f - l);

	RETURN_STRINGL(result, result_len, 0);
}
/* }}} */

/* {{{ proto string quotemeta(string str)
   Quote meta characters */
PHP_FUNCTION(quotemeta)
{
	zval **arg;
	char *str, *old;
	char *old_end;
	char *p, *q;
	char c;
	
	if (ZEND_NUM_ARGS() != 1 || 
	    zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg);

	old = Z_STRVAL_PP(arg);
	old_end = Z_STRVAL_PP(arg) + Z_STRLEN_PP(arg);

	if (old == old_end) {
		RETURN_FALSE;
	}
	
	str = emalloc(2 * (*arg)->value.str.len + 1);
	
	for(p = old, q = str; p != old_end; p++) {
		c = *p;
		switch(c) {
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
   Return ASCII value of character */
PHP_FUNCTION(ord)
{
	zval **str;
	
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

	RETURN_LONG((unsigned char) Z_STRVAL_PP(str)[0]);
}
/* }}} */

/* {{{ proto string chr(int ascii)
   Convert ASCII code to a character */
PHP_FUNCTION(chr)
{
	zval **num;
	char temp[2];
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(num);
	
	temp[0] = (char) Z_LVAL_PP(num);
	temp[1] = 0;

	RETVAL_STRINGL(temp, 1, 1);
}
/* }}} */

/* {{{ proto string ucfirst(string str)
   Make a string's first character uppercase */
PHP_FUNCTION(ucfirst)
{
	zval **str;
	
	if (ZEND_NUM_ARGS() != 1 || 
	    zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

	if (!Z_STRLEN_PP(str)) {
		RETURN_FALSE;
	}

	ZVAL_STRINGL(return_value, Z_STRVAL_PP(str), Z_STRLEN_PP(str), 1);
	*Z_STRVAL_P(return_value) = toupper((unsigned char) *Z_STRVAL_P(return_value));
}
/* }}} */

/* {{{ proto string ucwords(string str)
   Uppercase the first character of every word in a string */
PHP_FUNCTION(ucwords)
{
	zval **str;
	register char *r, *r_end;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

	if (!Z_STRLEN_PP(str)) {
		RETURN_FALSE;
	}

	ZVAL_STRINGL(return_value, Z_STRVAL_PP(str), Z_STRLEN_PP(str), 1);
	r = Z_STRVAL_P(return_value);

	*r = toupper((unsigned char) *r);
	for (r_end = r + Z_STRLEN_P(return_value) - 1; r < r_end; ) {
		if (isspace((int) *r++)) {
			*r = toupper((unsigned char) *r);
		}
	}
}
/* }}} */

/* {{{ php_strtr
 */
PHPAPI char *php_strtr(char *str, int len, char *str_from,
					   char *str_to, int trlen)
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

/* {{{ php_strtr_array
 */
static void php_strtr_array(zval *return_value, char *str, int slen, HashTable *hash)
{
	zval **entry;
	char  *string_key;
	uint   string_key_len;
	zval **trans;
	zval   ctmp;
	ulong num_key;
	int minlen = 128*1024;
	int maxlen = 0, pos, len, found;
	char *key;
	HashPosition hpos;
	smart_str result = {0};
	
	zend_hash_internal_pointer_reset_ex(hash, &hpos);
	while (zend_hash_get_current_data_ex(hash, (void **)&entry, &hpos) == SUCCESS) {
		switch (zend_hash_get_current_key_ex(hash, &string_key, &string_key_len, &num_key, 0, &hpos)) {
		case HASH_KEY_IS_STRING:
			len = string_key_len;
			if (len > maxlen) maxlen = len;
			if (len < minlen) minlen = len;
			break; 
			
		case HASH_KEY_IS_LONG:
			ctmp.type = IS_LONG;
			ctmp.value.lval = num_key;
			
			convert_to_string(&ctmp);
			len = Z_STRLEN(ctmp);
			zval_dtor(&ctmp);

			if (len > maxlen) maxlen = len;
			if (len < minlen) minlen = len;
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
			
			if (zend_hash_find(hash, key, len+1, (void**)&trans) == SUCCESS) {
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
	smart_str_0(&result);
	RETVAL_STRINGL(result.c, result.len, 0);
}
/* }}} */

/* {{{ proto string strtr(string str, string from, string to)
   Translate characters in str using given translation tables */
PHP_FUNCTION(strtr)
{								/* strtr(STRING, FROM, TO) */
	zval **str, **from, **to;
	int ac = ZEND_NUM_ARGS();

	if (ac < 2 || ac > 3 || zend_get_parameters_ex(ac, &str, &from, &to) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if (ac == 2 && (*from)->type != IS_ARRAY) {
		php_error(E_WARNING, "arg2 must be passed an array");
		RETURN_FALSE;
	}

	convert_to_string_ex(str);

	/* shortcut for empty string */
	if(Z_STRLEN_PP(str) == 0) {
		RETURN_EMPTY_STRING();
	}

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
				  MIN(Z_STRLEN_PP(from), Z_STRLEN_PP(to)));
	}
}
/* }}} */

/* {{{ proto string strrev(string str)
   Reverse a string */
PHP_FUNCTION(strrev)
{
	zval **str;
	int i, len;
	char c;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &str)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);
	
	ZVAL_STRINGL(return_value, Z_STRVAL_PP(str), Z_STRLEN_PP(str), 1);
	len = Z_STRLEN_P(return_value);
	
	for (i = 0; i < len - 1 - i; i++) {
		c = Z_STRVAL_P(return_value)[i];
		Z_STRVAL_P(return_value)[i] = Z_STRVAL_P(return_value)[len - 1 - i];
		Z_STRVAL_P(return_value)[len - 1 - i] = c;
	}
}
/* }}} */

/* {{{ php_similar_str
 */
static void php_similar_str(const char *txt1, int len1, const char *txt2, 
							int len2, int *pos1, int *pos2, int *max)
{
	char *p, *q;
	char *end1 = (char *) txt1 + len1;
	char *end2 = (char *) txt2 + len2;
	int l;
	
	*max = 0;
	for (p = (char *) txt1; p < end1; p++) {
		for (q = (char *) txt2; q < end2; q++) {
			for (l = 0; (p + l < end1) && (q + l < end2) && (p[l] == q[l]); 
				 l++);
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
static int php_similar_char(const char *txt1, int len1, 
							const char *txt2, int len2)
{
	int sum;
	int pos1, pos2, max;

	php_similar_str(txt1, len1, txt2, len2, &pos1, &pos2, &max);
	if ((sum = max)) {
		if (pos1 && pos2)
			sum += php_similar_char(txt1, pos1, txt2, pos2);
		if ((pos1 + max < len1) && (pos2 + max < len2))
			sum += php_similar_char(txt1 + pos1 + max, len1 - pos1 - max, 
									txt2 + pos2 + max, len2 - pos2 - max);
	}

	return sum;
}
/* }}} */

/* {{{ proto int similar_text(string str1, string str2 [, double percent])
   Calculates the similarity between two strings */
PHP_FUNCTION(similar_text)
{
	zval **t1, **t2, **percent;
	int ac = ZEND_NUM_ARGS();
	int sim;
	
	if (ac < 2 || ac > 3 ||
		zend_get_parameters_ex(ac, &t1, &t2, &percent) == FAILURE) {
		WRONG_PARAM_COUNT;
	}	
	convert_to_string_ex(t1);
	convert_to_string_ex(t2);
	if (ac > 2) {
		convert_to_double_ex(percent);
	}
	
	if (Z_STRLEN_PP(t1) + Z_STRLEN_PP(t2) == 0) {
		if(ac > 2) {
			Z_DVAL_PP(percent) = 0;
		}

		RETURN_LONG(0);
	}
	
	sim = php_similar_char(Z_STRVAL_PP(t1), Z_STRLEN_PP(t1), 
	                       Z_STRVAL_PP(t2), Z_STRLEN_PP(t2));	
	if (ac > 2) {
		Z_DVAL_PP(percent) = sim * 200.0 / (Z_STRLEN_PP(t1) + Z_STRLEN_PP(t2));
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
	char escape_char='\\';

	if (PG(magic_quotes_sybase)) {
		escape_char='\'';
	}

	if (len != NULL) {
		l = *len;
	} else {
		l = strlen(str);
	}
	s = str;
	t = str;
	while (l > 0) {
		if (*t == escape_char) {
			t++;				/* skip the slash */
			if (len != NULL)
				(*len)--;
			l--;
			if (l > 0) {
				if(*t=='0') {
					*s++='\0';
					t++;
				} else {
					*s++ = *t++;	/* preserve the next character */
				}
				l--;
			}
		} else {
			if (s != t)
				*s++ = *t++;
			else {
				s++;
				t++;
			}
			l--;
		}
	}
	if (s != t) {
		*s = '\0';
	}
}
/* }}} */

/* {{{ proto string addcslashes(string str, string charlist)
   Escape all chars mentioned in charlist with backslash. It creates octal representations if asked to backslash characters with 8th bit set or with ASCII<32 (except '\n', '\r', '\t' etc...) */
PHP_FUNCTION(addcslashes)
{
	zval **str, **what;

	if (ZEND_NUM_ARGS() != 2 || 
	    zend_get_parameters_ex(2, &str, &what) == FAILURE) {
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
	                              Z_STRLEN_PP(what) TSRMLS_CC), 
	              0);
}
/* }}} */

/* {{{ proto string addslashes(string str)
   Escape single quote, double quotes and backslash characters in a string with backslashes */
PHP_FUNCTION(addslashes)
{
	zval **str;

	if (ZEND_NUM_ARGS() != 1 || 
	    zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

	if (Z_STRLEN_PP(str) == 0) {
		RETURN_EMPTY_STRING();
	}

	RETURN_STRING(php_addslashes(Z_STRVAL_PP(str),
	                             Z_STRLEN_PP(str), 
	                             &Z_STRLEN_P(return_value), 0 
	                             TSRMLS_CC), 
	              0);
}
/* }}} */

/* {{{ proto string stripcslashes(string str)
   Strip backslashes from a string. Uses C-style conventions */
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

/* {{{ proto string stripslashes(string str)
   Strip backslashes from a string */
PHP_FUNCTION(stripslashes)
{
	zval **str;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

	ZVAL_STRINGL(return_value, Z_STRVAL_PP(str), Z_STRLEN_PP(str), 1);
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

	if ((unsigned int)errnum < sys_nerr) return(sys_errlist[errnum]);
	(void)sprintf(BG(str_ebuf), "Unknown error: %d", errnum);
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

	for (source=str, end=str+nlen, target=str; source<end; source++) {
		if (*source == '\\' && source+1<end) {
			source++;
			switch (*source) {
				case 'n': *target++='\n'; nlen--; break;
				case 'r': *target++='\r'; nlen--; break;
				case 'a': *target++='\a'; nlen--; break;
				case 't': *target++='\t'; nlen--; break;
				case 'v': *target++='\v'; nlen--; break;
				case 'b': *target++='\b'; nlen--; break;
				case 'f': *target++='\f'; nlen--; break;
				case '\\': *target++='\\'; nlen--; break;
				case 'x': if (source+1<end && isxdigit((int)(*(source+1)))) {
						numtmp[0] = *++source;
						if (source+1<end && isxdigit((int)(*(source+1)))) {
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
				default: i=0; 
					while (source<end && *source>='0' && *source<='7' && i<3) {
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

	if(nlen != 0) {
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
	char *new_str = emalloc((length?length:(length=strlen(str)))*4+1); 
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

	php_charmask(what, wlength, flags TSRMLS_CC);

	for (source=str, end=source+length, target=new_str; (c=*source) || source<end; source++) {
		if (flags[(unsigned char)c]) {
			if ((unsigned char)c<32 || (unsigned char)c>126) {
				*target++ = '\\';
				switch (c) {
					case '\n': *target++ = 'n'; break;
					case '\t': *target++ = 't'; break;
					case '\r': *target++ = 'r'; break;
					case '\a': *target++ = 'a'; break;
					case '\v': *target++ = 'v'; break;
					case '\b': *target++ = 'b'; break;
					case '\f': *target++ = 'f'; break;
					default: target += sprintf(target, "%03o", (unsigned char)c);
				}
				continue;
			} 
			*target++ = '\\';
		}
		*target++ = c;
	}
	*target = 0;
	newlen = target-new_str;
	if (target-new_str<length*4) {
		new_str = erealloc(new_str, newlen+1);
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

/* {{{ php_addslashes
 */
PHPAPI char *php_addslashes(char *str, int length, int *new_length, int should_free TSRMLS_DC)
{
	/* maximum string length, worst case situation */
	char *new_str;
	char *source, *target;
	char *end;
	char c;
 	
	if (!str) {
		*new_length = 0;
		return str;
	}
	new_str = (char *) emalloc((length?length:(length=strlen(str)))*2+1);
	for (source=str, end=source+length, target=new_str; source<end; source++) {
		c = *source;
		switch(c) {
			case '\0':
				*target++ = '\\';
				*target++ = '0';
				break;
			case '\'':
				if (PG(magic_quotes_sybase)) {
					*target++ = '\'';
					*target++ = '\'';
					break;
				}
				/* break is missing *intentionally* */
			case '\"':
			case '\\':
				if (!PG(magic_quotes_sybase)) {
					*target++ = '\\';
				}
				/* break is missing *intentionally* */
			default:
				*target++ = c;
				break;
		}
	}
	*target = 0;
	if (new_length) {
		*new_length = target - new_str;
	}
	if (should_free) {
		STR_FREE(str);
	}
	return new_str;
}
/* }}} */

#define _HEB_BLOCK_TYPE_ENG 1
#define _HEB_BLOCK_TYPE_HEB 2
#define isheb(c) (((((unsigned char) c)>=224) && (((unsigned char) c)<=250)) ? 1 : 0)
#define _isblank(c) (((((unsigned char) c)==' ' || ((unsigned char) c)=='\t')) ? 1 : 0)
#define _isnewline(c) (((((unsigned char) c)=='\n' || ((unsigned char) c)=='\r')) ? 1 : 0)

/* {{{ php_char_to_str
 */
PHPAPI void php_char_to_str(char *str, uint len, char from, char *to, int to_len, zval *result)
{
	int char_count=0;
	char *source, *target, *tmp, *source_end=str+len, *tmp_end=NULL;
	
	for (source=str; source<source_end; source++) {
		if (*source==from) {
			char_count++;
		}
	}

	if (char_count==0) {
		ZVAL_STRINGL(result, str, len, 1);
		return;
	}
	
	Z_STRLEN_P(result) = len + (char_count * (to_len - 1));
	Z_STRVAL_P(result) = target = emalloc(Z_STRLEN_P(result) + 1);
	Z_TYPE_P(result) = IS_STRING;
	
	for (source = str; source < source_end; source++) {
		if (*source == from) {
			for (tmp = to, tmp_end = tmp+to_len; tmp < tmp_end; tmp++) {
				*target = *tmp;
				target++;
			}
		} else {
			*target = *source;
			target++;
		}
	}
	*target = 0;
}
/* }}} */

/* {{{ boyer_str_to_str */
static char *boyer_str_to_str(char *haystack, int length,
		char *needle, int needle_len, char *str, 
		int str_len, int *new_length)
{
	char *p, *pe, *cursor, *end, *r;
	int off;
	char jump_table[256];
	smart_str result = {0};

	/*
	 * We implement only the first half of the Boyer-Moore algorithm,
	 * because the second half is too expensive to compute during run-time.
	 * TODO: Split matching into compile-/match-stage.
	 */
	
	/* Prepare the jump_table which contains the skip offsets */
	memset(jump_table, needle_len, 256);
	
	off = needle_len - 1;
	
	/* Calculate the default start where each comparison starts */
	pe = needle + off;

	/* Assign skip offsets based on the pattern */
	for (p = needle; p <= pe; p++)
		jump_table[(unsigned char) *p] = off--;
	
	/* Start to look at the first possible position for the pattern */
	cursor = haystack + needle_len - 1;
	
	/* The cursor must not cross this limit */
	end = haystack + length;

	/* Start to copy at haystack */
	r = haystack;
	
nextiter:
	while (cursor < end) {
		p = pe;						/* Compare from right to left */
		while (*p == *cursor) {
			if (--p < needle) {		/* Found the pattern */
									
				/* Append whatever was not matched */
				smart_str_appendl(&result, r, cursor - r);
									
				/* Append replacement string */
				smart_str_appendl(&result, str, str_len);
				
				/* Update copy pointer */
				r = cursor + needle_len;
				
				/* needle_len was substracted from cursor for 
				 * this comparison, add it back.  Also add 
				 * needle_len - 1 which is the default search 
				 * offset.
				 */
				cursor += (needle_len << 1) - 1;
				
				/* Next iteration */
				goto nextiter;
			}
			cursor--;
		}

		cursor += jump_table[(unsigned char) *cursor];
	}

	if (r < end)		/* Copy the remaining data */
		smart_str_appendl(&result, r, end - r);

	smart_str_0(&result); /* NUL-ify result */

	if (new_length)
		*new_length = result.len;

	return result.c;
}
/* }}} */

/* {{{ php_str_to_str
 */
PHPAPI char *php_str_to_str(char *haystack, int length, 
	char *needle, int needle_len, char *str, int str_len, int *_new_length)
{
	char *p;
	char *r;
	char *end = haystack + length;
	smart_str result = {0};

	for (p = haystack;
			(r = php_memnstr(p, needle, needle_len, end));
			p = r + needle_len) {
		smart_str_appendl(&result, p, r - p);
		smart_str_appendl(&result, str, str_len);
	}

	if (p < end) 
		smart_str_appendl(&result, p, end - p);

	smart_str_0(&result);

	if (_new_length) 
		*_new_length = result.len;

	return result.c;
}
/* }}} */

/* {{{ php_str_replace_in_subject
 */
static void php_str_replace_in_subject(zval *search, zval *replace, zval **subject, zval *result, int boyer)
{
	zval		**search_entry,
				**replace_entry = NULL,
				  temp_result;
	char		*replace_value = NULL;
	int			 replace_len = 0;
	char *(*str_to_str)(char *, int, char *, int, char *, int, int *);

	str_to_str = boyer ? boyer_str_to_str : php_str_to_str;
	
	/* Make sure we're dealing with strings. */	
	convert_to_string_ex(subject);
	Z_TYPE_P(result) = IS_STRING;
	if (Z_STRLEN_PP(subject) == 0) {
		ZVAL_STRINGL(result, empty_string, 0, 1);
		return;
	}
	
	/* If search is an array */
	if (Z_TYPE_P(search) == IS_ARRAY) {
		/* Duplicate subject string for repeated replacement */
		*result = **subject;
		zval_copy_ctor(result);
		
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
			convert_to_string_ex(search_entry);
			if(Z_STRLEN_PP(search_entry) == 0) {
				zend_hash_move_forward(Z_ARRVAL_P(search));
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
					replace_value = empty_string;
					replace_len = 0;
				}
			}
			
			if(Z_STRLEN_PP(search_entry) == 1) {
				php_char_to_str(Z_STRVAL_P(result),
								Z_STRLEN_P(result),
								Z_STRVAL_PP(search_entry)[0],
								replace_value,
								replace_len,
								&temp_result);
			} else if (Z_STRLEN_PP(search_entry) > 1) {
				Z_STRVAL(temp_result) = str_to_str(Z_STRVAL_P(result), Z_STRLEN_P(result),
													   Z_STRVAL_PP(search_entry), Z_STRLEN_PP(search_entry),
													   replace_value, replace_len, &Z_STRLEN(temp_result));
			}

			efree(Z_STRVAL_P(result));
			Z_STRVAL_P(result) = Z_STRVAL(temp_result);
			Z_STRLEN_P(result) = Z_STRLEN(temp_result);

			zend_hash_move_forward(Z_ARRVAL_P(search));
		}
	} else {
		if (Z_STRLEN_P(search) == 1) {
			php_char_to_str(Z_STRVAL_PP(subject),
							Z_STRLEN_PP(subject),
							Z_STRVAL_P(search)[0],
							Z_STRVAL_P(replace),
							Z_STRLEN_P(replace),
							result);
		} else if (Z_STRLEN_P(search) > 1) {
			Z_STRVAL_P(result) = str_to_str(Z_STRVAL_PP(subject), Z_STRLEN_PP(subject),
												Z_STRVAL_P(search), Z_STRLEN_P(search),
												Z_STRVAL_P(replace), Z_STRLEN_P(replace), &Z_STRLEN_P(result));
		} else {
			*result = **subject;
			zval_copy_ctor(result);
		}
	}
}
/* }}} */

/* {{{ proto mixed str_replace(mixed search, mixed replace, mixed subject[, bool boyer])
   Replace all occurrences of search in haystack with replace */
PHP_FUNCTION(str_replace)
{
	zval **subject, **search, **replace, **subject_entry, **pboyer;
	zval *result;
	char *string_key;
	uint string_key_len;
	ulong num_key;
	int boyer = 0;

	if(ZEND_NUM_ARGS() < 3 ||
			ZEND_NUM_ARGS() > 4 ||
			zend_get_parameters_ex(ZEND_NUM_ARGS(), &search, 
				&replace, &subject, &pboyer) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	switch (ZEND_NUM_ARGS()) {
		case 4:
			convert_to_boolean_ex(pboyer);
			if (Z_BVAL_PP(pboyer))
				boyer = 1;
			break;
	}
	
	SEPARATE_ZVAL(search);
	SEPARATE_ZVAL(replace);
	SEPARATE_ZVAL(subject);

	/* Make sure we're dealing with strings and do the replacement. */
	if (Z_TYPE_PP(search) != IS_ARRAY) {
		convert_to_string_ex(search);
		convert_to_string_ex(replace);
	} else if (Z_TYPE_PP(replace) != IS_ARRAY)
		convert_to_string_ex(replace);

	/* if subject is an array */
	if (Z_TYPE_PP(subject) == IS_ARRAY) {
		array_init(return_value);
		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(subject));

		/* For each subject entry, convert it to string, then perform replacement
		   and add the result to the return_value array. */
		while (zend_hash_get_current_data(Z_ARRVAL_PP(subject), (void **)&subject_entry) == SUCCESS) {
			MAKE_STD_ZVAL(result);
			php_str_replace_in_subject(*search, *replace, subject_entry, result, boyer);
			/* Add to return array */
			switch(zend_hash_get_current_key_ex(Z_ARRVAL_PP(subject), &string_key,
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
		php_str_replace_in_subject(*search, *replace, subject, return_value, boyer);
	}	
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
	char *heb_str, *tmp, *target, *opposite_target, *broken_str;
	int block_start, block_end, block_type, block_length, i;
	int block_ended;
	long max_chars=0;
	int begin, end, char_count, orig_begin;

	
	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &str)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &str, &max_chars_per_line)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(max_chars_per_line);
			max_chars = (*max_chars_per_line)->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_string_ex(str);
	
	if ((*str)->value.str.len==0) {
		RETURN_FALSE;
	}

	tmp = (*str)->value.str.val;
	block_start=block_end=0;
	block_ended=0;

	heb_str = (char *) emalloc((*str)->value.str.len+1);
	target = heb_str+(*str)->value.str.len;
	opposite_target = heb_str;
	*target = 0;
	target--;

	block_length=0;

	if (isheb(*tmp)) {
		block_type = _HEB_BLOCK_TYPE_HEB;
	} else {
		block_type = _HEB_BLOCK_TYPE_ENG;
	}
	
	do {
		if (block_type==_HEB_BLOCK_TYPE_HEB) {
			while((isheb((int)*(tmp+1)) || _isblank((int)*(tmp+1)) || ispunct((int)*(tmp+1)) || (int)*(tmp+1)=='\n' ) && block_end<(*str)->value.str.len-1) {
				tmp++;
				block_end++;
				block_length++;
			}
			for (i=block_start; i<=block_end; i++) {
				*target = (*str)->value.str.val[i];
				switch (*target) {
					case '(':
						*target = ')';
						break;
					case ')':
						*target = '(';
						break;
					default:
						break;
				}
				target--;
			}
			block_type = _HEB_BLOCK_TYPE_ENG;
		} else {
			while(!isheb(*(tmp+1)) && (int)*(tmp+1)!='\n' && block_end<(*str)->value.str.len-1) {
				tmp++;
				block_end++;
				block_length++;
			}
			while ((_isblank((int)*tmp) || ispunct((int)*tmp)) && *tmp!='/' && *tmp!='-' && block_end>block_start) {
				tmp--;
				block_end--;
			}
			for (i=block_end; i>=block_start; i--) {
				*target = (*str)->value.str.val[i];
				target--;
			}
			block_type = _HEB_BLOCK_TYPE_HEB;
		}
		block_start=block_end+1;
	} while(block_end<(*str)->value.str.len-1);


	broken_str = (char *) emalloc((*str)->value.str.len+1);
	begin=end=(*str)->value.str.len-1;
	target = broken_str;
		
	while (1) {
		char_count=0;
		while ((!max_chars || char_count<max_chars) && begin>0) {
			char_count++;
			begin--;
			if (begin<=0 || _isnewline(heb_str[begin])) {
				while(begin>0 && _isnewline(heb_str[begin-1])) {
					begin--;
					char_count++;
				}
				break;
			}
		}
		if (char_count==max_chars) { /* try to avoid breaking words */
			int new_char_count=char_count, new_begin=begin;
			
			while (new_char_count>0) {
				if (_isblank(heb_str[new_begin]) || _isnewline(heb_str[new_begin])) {
					break;
				}
				new_begin++;
				new_char_count--;
			}
			if (new_char_count>0) {
				char_count=new_char_count;
				begin=new_begin;
			}
		}
		orig_begin=begin;
		
		if (_isblank(heb_str[begin])) {
			heb_str[begin]='\n';
		}
		while (begin<=end && _isnewline(heb_str[begin])) { /* skip leading newlines */
			begin++;
		}
		for (i=begin; i<=end; i++) { /* copy content */
			*target = heb_str[i];
			target++;
		}
		for (i=orig_begin; i<=end && _isnewline(heb_str[i]); i++) {
			*target = heb_str[i];
			target++;
		}
		begin=orig_begin;

		if (begin<=0) {
			*target = 0;
			break;
		}
		begin--;
		end=begin;
	}
	efree(heb_str);

	if (convert_newlines) {
		php_char_to_str(broken_str, (*str)->value.str.len,'\n', "<br>\n", 5, return_value);
		efree(broken_str);
	} else {
		return_value->value.str.val = broken_str;
		return_value->value.str.len = (*str)->value.str.len;
		return_value->type = IS_STRING;
	}
}
/* }}} */

/* {{{ proto string hebrev(string str [, int max_chars_per_line])
   Convert logical Hebrew text to visual text */
PHP_FUNCTION(hebrev)
{
	php_hebrev(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string hebrevc(string str [, int max_chars_per_line])
   Convert logical Hebrew text to visual text with newline conversion */
PHP_FUNCTION(hebrevc)
{
	php_hebrev(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto string nl2br(string str)
   Converts newlines to HTML line breaks */
PHP_FUNCTION(nl2br)
{
	zval **str;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &str)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(str);
	
	php_char_to_str((*str)->value.str.val, (*str)->value.str.len,'\n', "<br />\n", 7, return_value);
}
/* }}} */

/* {{{ proto string strip_tags(string str [, string allowable_tags])
   Strips HTML and PHP tags from a string */
PHP_FUNCTION(strip_tags)
{
	char *buf;
	zval **str, **allow=NULL;
	char *allowed_tags=NULL;
	int allowed_tags_len=0;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if(zend_get_parameters_ex(1, &str)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if(zend_get_parameters_ex(2, &str, &allow)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string_ex(allow);
			allowed_tags = (*allow)->value.str.val;
			allowed_tags_len = (*allow)->value.str.len;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	convert_to_string_ex(str);
	buf = estrndup((*str)->value.str.val, (*str)->value.str.len);
	php_strip_tags(buf, (*str)->value.str.len, 0, allowed_tags, allowed_tags_len);
	RETURN_STRING(buf, 0);
}
/* }}} */

/* {{{ proto string setlocale(mixed category, string locale)
   Set locale information */
PHP_FUNCTION(setlocale)
{
	zval **pcategory, **plocale;
	zval *category, *locale;
	int cat;
	char *loc, *retval;

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &pcategory, &plocale)==FAILURE)
		WRONG_PARAM_COUNT;
#ifdef HAVE_SETLOCALE
	convert_to_string_ex(plocale);
	locale = *plocale;

	if (Z_TYPE_PP(pcategory) == IS_LONG) {
		convert_to_long_ex(pcategory);	
		cat = Z_LVAL_PP(pcategory);
	} else { /* FIXME: The following behaviour should be removed. */
		php_error(E_NOTICE, "Passing locale category name as string is deprecated. Use the LC_* -constants instead.");
		convert_to_string_ex(pcategory);
		category = *pcategory;

		if (!strcasecmp ("LC_ALL", category->value.str.val))
			cat = LC_ALL;
		else if (!strcasecmp ("LC_COLLATE", category->value.str.val))
			cat = LC_COLLATE;
		else if (!strcasecmp ("LC_CTYPE", category->value.str.val))
			cat = LC_CTYPE;
#ifdef LC_MESSAGES
		else if (!strcasecmp ("LC_MESSAGES", category->value.str.val))
			cat = LC_MESSAGES;
#endif
		else if (!strcasecmp ("LC_MONETARY", category->value.str.val))
			cat = LC_MONETARY;
		else if (!strcasecmp ("LC_NUMERIC", category->value.str.val))
			cat = LC_NUMERIC;
		else if (!strcasecmp ("LC_TIME", category->value.str.val))
			cat = LC_TIME;
		else {
			php_error(E_WARNING, "Invalid locale category name %s, must be one of LC_ALL, LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC or LC_TIME", category->value.str.val);
			RETURN_FALSE;
		}
	}
	if (!strcmp ("0", locale->value.str.val)) {
		loc = NULL;
	} else {
		loc = locale->value.str.val;
	}
	
	retval = setlocale (cat, loc);
	if (retval) {
		/* Remember if locale was changed */
		if (loc) {
			STR_FREE(BG(locale_string));
			BG(locale_string) = estrdup(retval);
		}

		RETVAL_STRING(retval, 1);
		return;
	}
#endif
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto void parse_str(string encoded_string, [array result])
   Parses GET/POST/COOKIE data and sets global variables */
PHP_FUNCTION(parse_str)
{
	zval **arg;
	zval **arrayArg;
	zval *sarg;
	char *res = NULL;
	int argCount;
	int old_rg;

	argCount = ARG_COUNT(ht);
	if(argCount < 1 || argCount > 2 || zend_get_parameters_ex(argCount, &arg, &arrayArg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg);
	sarg = *arg;
	if (sarg->value.str.val && *sarg->value.str.val) {
		res = estrndup(sarg->value.str.val, sarg->value.str.len);
	}

	old_rg = PG(register_globals);
	if(argCount == 1) {
		PG(register_globals) = 1;
		php_treat_data(PARSE_STRING, res, NULL TSRMLS_CC);
	} else 	{
		PG(register_globals) = 0;
		/* Clear out the array that was passed in. */
		zval_dtor(*arrayArg);
		array_init(*arrayArg);
		
		php_treat_data(PARSE_STRING, res, *arrayArg TSRMLS_CC);
	}
	PG(register_globals) = old_rg;
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
	while(!done) {
		switch(c) {
		case '<':
			*(n++) = c;
			break;
		case '>':
			done =1;
			break;
		default:
			if(!isspace((int)c)) {
				if(state==0) {
					state=1;
					if(c!='/') *(n++) = c;
				} else {
					*(n++) = c;
				}
			} else {
				if(state==1) done=1;
			}
			break;
		}
		c = tolower(*(++t));
	}  
	*(n++) = '>';
	*n = '\0'; 
	if(strstr(set, norm)) {
		done=1;
	} else {
		done=0;
	}
	efree(norm);
	return done;
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
PHPAPI void php_strip_tags(char *rbuf, int len, int state, char *allow, int allow_len)
{
	char *tbuf, *buf, *p, *tp, *rp, c, lc;
	int br, i=0;

	buf = estrndup(rbuf, len);
	c = *buf;
	lc = '\0';
	p = buf;
	rp = rbuf;
	br = 0;
	if(allow) {
		php_strtolower(allow, allow_len);
		tbuf = emalloc(PHP_TAG_BUF_SIZE+1);
		tp = tbuf;
	} else {
		tbuf = tp = NULL;
	}

	while(i<len) {
		switch (c) {
			case '<':
				if (state == 0) {
					lc = '<';
					state = 1;
					if(allow) {
						*(tp++) = '<';
					}
				}
				break;

			case '(':
				if (state == 2) {
					if (lc != '\"') {
						lc = '(';
						br++;
					}
				} else if (allow && state == 1) {
					*(tp++) = c;
				} else if (state == 0) {
					*(rp++) = c;
				}
				break;	

			case ')':
				if (state == 2) {
					if (lc != '\"') {
						lc = ')';
						br--;
					}
				} else if (allow && state == 1) {
					*(tp++) = c;
				} else if (state == 0) {
					*(rp++) = c;
				}
				break;	

			case '>':
				if (state == 1) {
					lc = '>';
					state = 0;
					if(allow) {
						*(tp++) = '>';
						*tp='\0';
						if(php_tag_find(tbuf, tp-tbuf, allow)) {
							memcpy(rp, tbuf, tp-tbuf);
							rp += tp-tbuf;
						}
						tp = tbuf;
					}
				} else if (state == 2) {
					if (!br && lc != '\"' && *(p-1)=='?') {
						state = 0;
						tp = tbuf;
					}
				} else {
					*(rp++) = c;
				}
				break;

			case '\"':
				if (state == 2) {
					if (lc == '\"') {
						lc = '\0';
					} else if (lc != '\\') {
						lc = '\"';
					}
				} else if (state == 0) {
					*(rp++) = c;
				} else if (allow && state == 1) {
					*(tp++) = c;
				}
				break;

			case '?':

				if (state==1 && *(p-1)=='<') { 
					br=0;
					state=2;
					break;
				}

			case 'l':

				/* swm: If we encounter '<?xml' then we shouldn't be in
				 * state == 2 (PHP). Switch back to HTML.
				 */

				if(state == 2 && *(p-1) == 'm' && *(p-2) == 'x') {
					state = 1;
					break;
				}

				/* fall-through */
			default:
				if (state == 0) {
					*(rp++) = c;
				} else if(allow && state == 1) {
					*(tp++) = c;
					if( (tp-tbuf)>=PHP_TAG_BUF_SIZE ) { /* no buffer overflows */
						tp = tbuf;
					}
				} 
				break;
		}
		c = *(++p);
		i++;
	}	
	*rp = '\0';
	efree(buf);
	if(allow) efree(tbuf);
}
/* }}} */

/* {{{ proto string str_repeat(string input, int mult)
   Returns the input string repeat mult times */
PHP_FUNCTION(str_repeat)
{
	zval		**input_str;		/* Input string */
	zval		**mult;				/* Multiplier */
	char		 *result;			/* Resulting string */
	int			  result_len;		/* Length of the resulting string */
	int			  i;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &input_str, &mult) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	/* Make sure we're dealing with proper types */
	convert_to_string_ex(input_str);
	convert_to_long_ex(mult);
	
	if ((*mult)->value.lval < 0) {
		php_error(E_WARNING, "Second argument to %s() has to be greater than or equal to 0",
				  get_active_function_name(TSRMLS_C));
		return;
	}

	/* Don't waste our time if it's empty */
	if ((*input_str)->value.str.len == 0)
		RETURN_STRINGL(empty_string, 0, 1);
	
	/* ... or if the multiplier is zero */
	if ((*mult)->value.lval == 0)
		RETURN_STRINGL(empty_string, 0, 1);
	
	/* Initialize the result string */	
	result_len = (*input_str)->value.str.len * (*mult)->value.lval;
	result = (char *)emalloc(result_len + 1);
	
	/* Copy the input string into the result as many times as necessary */
	for (i=0; i<(*mult)->value.lval; i++) {
		memcpy(result + (*input_str)->value.str.len * i,
			   (*input_str)->value.str.val,
			   (*input_str)->value.str.len);
	}
	result[result_len] = '\0';
	
	RETURN_STRINGL(result, result_len, 0);
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
		mymode = (*mode)->value.lval;
		
		if (mymode < 0 || mymode > 4) {
			php_error(E_WARNING, "unknown mode");
			RETURN_FALSE;
		}
	}
	
	len = (*input)->value.str.len;
	buf = (unsigned char *) (*input)->value.str.val;
	memset((void*) chars, 0, sizeof(chars));

	while (len > 0) {
		chars[*buf]++;
		buf++;
		len--;
	}

	if (mymode < 3) {
		array_init(return_value);
	}

	for (inx=0; inx < 256; inx++) {
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

	RETURN_LONG(strnatcmp_ex((*s1)->value.str.val, (*s1)->value.str.len,
							 (*s2)->value.str.val, (*s2)->value.str.len,
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

/* {{{ proto array localeconv( void )
   Returns numeric formatting information based on the current locale */
PHP_FUNCTION(localeconv)
{
	zval *grouping, *mon_grouping;
	int len, i;

	MAKE_STD_ZVAL(grouping);
	MAKE_STD_ZVAL(mon_grouping);

	/* We don't need no stinkin' parameters... */
	if (ZEND_NUM_ARGS() > 0) {
		WRONG_PARAM_COUNT;
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	if (array_init(grouping) == FAILURE || array_init(mon_grouping) == FAILURE) {
		RETURN_FALSE;
	}	

#ifdef HAVE_LOCALECONV
	{
		struct lconv currlocdata;

		localeconv_r( &currlocdata );
   
		/* Grab the grouping data out of the array */
		len = strlen(currlocdata.grouping);

		for (i=0;i<len;i++) {
			add_index_long(grouping, i, currlocdata.grouping[i]);
		}

		/* Grab the monetary grouping data out of the array */
		len = strlen(currlocdata.mon_grouping);

		for (i=0;i<len;i++) {
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

	zend_hash_update(return_value->value.ht, "grouping", 9, &grouping, sizeof(zval *), NULL);
	zend_hash_update(return_value->value.ht, "mon_grouping", 13, &mon_grouping, sizeof(zval *), NULL);
}
/* }}} */

/* {{{ proto int strnatcasecmp(string s1, string s2)
   Returns the result of case-insensitive string comparison using 'natural' algorithm */
PHP_FUNCTION(strnatcasecmp)
{
	php_strnatcmp(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int substr_count(string haystack, string needle)
   Returns the number of times a substring occurs in the string */
PHP_FUNCTION(substr_count)
{
	zval **haystack, **needle;	
	int i, length, count = 0;
	char *p, *endp, cmp;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &haystack, &needle) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(haystack);
	convert_to_string_ex(needle);

	if ((*needle)->value.str.len == 0) {
		php_error(E_WARNING, "Empty substring");
		RETURN_FALSE;
	} else if ((*needle)->value.str.len == 1) {
		/* Special optimized case to avoid calls to php_memnstr(). */
		for (i = 0, p = (*haystack)->value.str.val, 
		     length = (*haystack)->value.str.len, cmp = (*needle)->value.str.val[0]; 
		     i < length; i++) {
			if (p[i] == cmp) {
				count++;
			}
		}
	} else {
 		p = (*haystack)->value.str.val;
		endp = p + (*haystack)->value.str.len;
		while (p <= endp) {
			if( (p = php_memnstr(p, (*needle)->value.str.val, (*needle)->value.str.len, endp)) != NULL ) {
				p += (*needle)->value.str.len;
				count++;
			} else {
				break;
			}
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
	zval **input,				/* Input string */
		 **pad_length,			/* Length to pad to */
		 **pad_string,			/* Padding string */
		 **pad_type;			/* Padding type (left/right/both) */
	
	/* Helper variables */
	int	   num_pad_chars;		/* Number of padding characters (total - input size) */
	char  *result = NULL;		/* Resulting string */
	int	   result_len = 0;		/* Length of the resulting string */
	char  *pad_str_val = " ";	/* Pointer to padding string */
	int    pad_str_len = 1;		/* Length of the padding string */
	int	   pad_type_val = STR_PAD_RIGHT; /* The padding type value */
	int	   i, left_pad=0, right_pad=0;


	if (ZEND_NUM_ARGS() < 2 || ZEND_NUM_ARGS() > 4 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &input, &pad_length, &pad_string, &pad_type) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* Perform initial conversion to expected data types. */
	convert_to_string_ex(input);
	convert_to_long_ex(pad_length);

	num_pad_chars = Z_LVAL_PP(pad_length) - Z_STRLEN_PP(input);

	/* If resulting string turns out to be shorter than input string,
	   we simply copy the input and return. */
	if (num_pad_chars < 0) {
		*return_value = **input;
		zval_copy_ctor(return_value);
		return;
	}

	/* Setup the padding string values if specified. */
	if (ZEND_NUM_ARGS() > 2) {
		convert_to_string_ex(pad_string);
		if (Z_STRLEN_PP(pad_string) == 0) {
			php_error(E_WARNING, "Padding string cannot be empty in %s()",
					  get_active_function_name(TSRMLS_C));
			return;
		}
		pad_str_val = Z_STRVAL_PP(pad_string);
		pad_str_len = Z_STRLEN_PP(pad_string);

		if (ZEND_NUM_ARGS() > 3) {
			convert_to_long_ex(pad_type);
			pad_type_val = Z_LVAL_PP(pad_type);
			if (pad_type_val < STR_PAD_LEFT || pad_type_val > STR_PAD_BOTH) {
				php_error(E_WARNING, "Padding type has to be STR_PAD_LEFT, STR_PAD_RIGHT, or STR_PAD_BOTH in %s()", get_active_function_name(TSRMLS_C));
				return;
			}
		}
	}

	result = (char *)emalloc(Z_STRLEN_PP(input) + num_pad_chars + 1);

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
	memcpy(result + result_len, Z_STRVAL_PP(input), Z_STRLEN_PP(input));
	result_len += Z_STRLEN_PP(input);

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
    zval ***args;
    int     result;
	int	    argc = ZEND_NUM_ARGS();	

	if (argc < 2) {
		WRONG_PARAM_COUNT;
	}

	args = (zval ***) emalloc(argc * sizeof(zval **));
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 tw=78 fdm=marker
 * vim<600: noet sw=4 ts=4 tw=78
 */
