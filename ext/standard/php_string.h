/* 
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Synced with php 3.0 revision 1.43 1999-06-16 [ssb] */

#ifndef PHP_STRING_H
#define PHP_STRING_H

PHP_FUNCTION(strspn);
PHP_FUNCTION(strcspn);
PHP_FUNCTION(str_replace);
PHP_FUNCTION(str_ireplace);
PHP_FUNCTION(rtrim);
PHP_FUNCTION(trim);
PHP_FUNCTION(ltrim);
PHP_FUNCTION(soundex);
PHP_FUNCTION(levenshtein);

PHP_FUNCTION(count_chars);
PHP_FUNCTION(wordwrap);
PHP_FUNCTION(explode);
PHP_FUNCTION(implode);
PHP_FUNCTION(strtok);
PHP_FUNCTION(strtoupper);
PHP_FUNCTION(strtolower);
PHP_FUNCTION(basename);
PHP_FUNCTION(dirname);
PHP_FUNCTION(pathinfo);
PHP_FUNCTION(strstr);
PHP_FUNCTION(strpos);
PHP_FUNCTION(stripos);
PHP_FUNCTION(strrpos);
PHP_FUNCTION(strripos);
PHP_FUNCTION(strrchr);
PHP_FUNCTION(substr);
PHP_FUNCTION(quotemeta);
PHP_FUNCTION(ucfirst);
PHP_FUNCTION(lcfirst);
PHP_FUNCTION(ucwords);
PHP_FUNCTION(strtr);
PHP_FUNCTION(strrev);
PHP_FUNCTION(hebrev);
PHP_FUNCTION(hebrevc);
PHP_FUNCTION(user_sprintf);
PHP_FUNCTION(user_printf);
PHP_FUNCTION(vprintf);
PHP_FUNCTION(vsprintf);
PHP_FUNCTION(addcslashes);
PHP_FUNCTION(addslashes);
PHP_FUNCTION(stripcslashes);
PHP_FUNCTION(stripslashes);
PHP_FUNCTION(chr);
PHP_FUNCTION(ord);
PHP_FUNCTION(nl2br);
PHP_FUNCTION(setlocale);
PHP_FUNCTION(localeconv);
PHP_FUNCTION(nl_langinfo);
PHP_FUNCTION(stristr);
PHP_FUNCTION(chunk_split);
PHP_FUNCTION(parse_str);
PHP_FUNCTION(str_getcsv);
PHP_FUNCTION(bin2hex);
PHP_FUNCTION(hex2bin);
PHP_FUNCTION(similar_text);
PHP_FUNCTION(strip_tags);
PHP_FUNCTION(str_repeat);
PHP_FUNCTION(substr_replace);
PHP_FUNCTION(strnatcmp);
PHP_FUNCTION(strnatcasecmp);
PHP_FUNCTION(substr_count);
PHP_FUNCTION(str_pad);
PHP_FUNCTION(sscanf);
PHP_FUNCTION(str_shuffle);
PHP_FUNCTION(str_word_count);
PHP_FUNCTION(str_split);
PHP_FUNCTION(strpbrk);
PHP_FUNCTION(substr_compare);
#ifdef HAVE_STRCOLL
PHP_FUNCTION(strcoll);
#endif
#if HAVE_STRFMON
PHP_FUNCTION(money_format);
#endif

#if defined(HAVE_LOCALECONV) && defined(ZTS)
PHP_MINIT_FUNCTION(localeconv);
PHP_MSHUTDOWN_FUNCTION(localeconv);
#endif
#if HAVE_NL_LANGINFO
PHP_MINIT_FUNCTION(nl_langinfo);
#endif

#define strnatcmp(a, b) \
	strnatcmp_ex(a, strlen(a), b, strlen(b), 0)
#define strnatcasecmp(a, b) \
	strnatcmp_ex(a, strlen(a), b, strlen(b), 1)
PHPAPI int strnatcmp_ex(char const *a, size_t a_len, char const *b, size_t b_len, int fold_case);

#ifdef HAVE_LOCALECONV
PHPAPI struct lconv *localeconv_r(struct lconv *out);
#endif

PHPAPI char *php_strtoupper(char *s, size_t len);
PHPAPI char *php_strtolower(char *s, size_t len);
PHPAPI zend_string *php_string_toupper(zend_string *s);
PHPAPI zend_string *php_string_tolower(zend_string *s);
PHPAPI char *php_strtr(char *str, size_t len, char *str_from, char *str_to, size_t trlen);
PHPAPI zend_string *php_addslashes(zend_string *str, int should_free);
PHPAPI zend_string *php_addcslashes(zend_string *str, int freeit, char *what, size_t what_len);
PHPAPI void php_stripslashes(zend_string *str);
PHPAPI void php_stripcslashes(zend_string *str);
PHPAPI zend_string *php_basename(const char *s, size_t len, char *suffix, size_t sufflen);
PHPAPI size_t php_dirname(char *str, size_t len);
PHPAPI char *php_stristr(char *s, char *t, size_t s_len, size_t t_len);
PHPAPI zend_string *php_str_to_str(char *haystack, size_t length, char *needle,
		size_t needle_len, char *str, size_t str_len);
PHPAPI zend_string *php_trim(zend_string *str, char *what, size_t what_len, int mode);
PHPAPI size_t php_strip_tags(char *rbuf, size_t len, int *state, const char *allow, size_t allow_len);
PHPAPI size_t php_strip_tags_ex(char *rbuf, size_t len, int *stateptr, const char *allow, size_t allow_len, zend_bool allow_tag_spaces);
PHPAPI void php_implode(const zend_string *delim, zval *arr, zval *return_value);
PHPAPI void php_explode(const zend_string *delim, zend_string *str, zval *return_value, zend_long limit);

PHPAPI size_t php_strspn(char *s1, char *s2, char *s1_end, char *s2_end); 
PHPAPI size_t php_strcspn(char *s1, char *s2, char *s1_end, char *s2_end); 

PHPAPI int string_natural_compare_function_ex(zval *result, zval *op1, zval *op2, zend_bool case_insensitive);
PHPAPI int string_natural_compare_function(zval *result, zval *op1, zval *op2);
PHPAPI int string_natural_case_compare_function(zval *result, zval *op1, zval *op2);

#ifndef HAVE_STRERROR
PHPAPI char *php_strerror(int errnum);
#define strerror php_strerror
#endif

#ifndef HAVE_MBLEN
# define php_mblen(ptr, len) 1
# define php_mb_reset()
#elif defined(_REENTRANT) && defined(HAVE_MBRLEN) && defined(HAVE_MBSTATE_T)
# ifdef PHP_WIN32
# include <wchar.h>
# endif
# define php_mblen(ptr, len) ((int) mbrlen(ptr, len, &BG(mblen_state)))
# define php_mb_reset() memset(&BG(mblen_state), 0, sizeof(BG(mblen_state)))
#else
# define php_mblen(ptr, len) mblen(ptr, len)
# define php_mb_reset() php_ignore_value(mblen(NULL, 0))
#endif

void register_string_constants(INIT_FUNC_ARGS);

#endif /* PHP_STRING_H */
