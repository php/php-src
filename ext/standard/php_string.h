/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stig Sæther Bakken <ssb@guardian.no>                        |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

/* Synced with php 3.0 revision 1.43 1999-06-16 [ssb] */

#ifndef PHP_STRING_H
#define PHP_STRING_H

PHP_FUNCTION(strspn);
PHP_FUNCTION(strcspn);
PHP_FUNCTION(str_replace);
PHP_FUNCTION(chop);
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
PHP_FUNCTION(strstr);
PHP_FUNCTION(strpos);
PHP_FUNCTION(strrpos);
PHP_FUNCTION(strrchr);
PHP_FUNCTION(substr);
PHP_FUNCTION(quotemeta);
PHP_FUNCTION(ucfirst);
PHP_FUNCTION(ucwords);
PHP_FUNCTION(strtr);
PHP_FUNCTION(strrev);
PHP_FUNCTION(hebrev);
PHP_FUNCTION(hebrevc);
PHP_FUNCTION(user_sprintf);
PHP_FUNCTION(user_printf);
PHP_FUNCTION(addcslashes);
PHP_FUNCTION(addslashes);
PHP_FUNCTION(stripcslashes);
PHP_FUNCTION(stripslashes);
PHP_FUNCTION(chr);
PHP_FUNCTION(ord);
PHP_FUNCTION(nl2br);
PHP_FUNCTION(setlocale);
PHP_FUNCTION(stristr);
PHP_FUNCTION(chunk_split);
PHP_FUNCTION(parse_str);
PHP_FUNCTION(bin2hex);
PHP_FUNCTION(similar_text);
PHP_FUNCTION(strip_tags);
PHP_FUNCTION(str_repeat);
PHP_FUNCTION(substr_replace);
PHP_FUNCTION(strnatcmp);
PHP_FUNCTION(strnatcasecmp);
PHP_FUNCTION(substr_count);
PHP_FUNCTION(str_pad);
PHP_FUNCTION(sscanf);

#define strnatcmp(a, b) \
	strnatcmp_ex(a, strlen(a), b, strlen(b), 0)
#define strnatcasecmp(a, b) \
	strnatcmp_ex(a, strlen(a), b, strlen(b), 1)
PHPAPI int strnatcmp_ex(char const *a, size_t a_len, char const *b, size_t b_len, int fold_case);

PHPAPI char *php_strtoupper(char *s, size_t len);
PHPAPI char *php_strtolower(char *s, size_t len);
PHPAPI char *php_strtr(char *str, int len, char *str_from, char *str_to, int trlen);
PHPAPI char *php_addslashes(char *str, int length, int *new_length, int freeit);
PHPAPI char *php_addcslashes(char *str, int length, int *new_length, int freeit, char *what, int wlength);
PHPAPI void php_stripslashes(char *str, int *len);
PHPAPI void php_stripcslashes(char *str, int *len);
PHPAPI char *php_basename(char *str, size_t  len);
PHPAPI void php_dirname(char *str, int len);
PHPAPI char *php_stristr(unsigned char *s, unsigned char *t, size_t s_len, size_t t_len);
PHPAPI char *php_str_to_str(char *haystack, int length, char *needle,
		int needle_len, char *str, int str_len, int *_new_length);
PHPAPI void php_trim(pval *str, pval *return_value, int mode);
PHPAPI void php_strip_tags(char *rbuf, int len, int state, char *allow, int allow_len);

PHPAPI void php_char_to_str(char *str, uint len, char from, char *to, int to_len, pval *result);

PHPAPI void php_implode(pval *delim, pval *arr, pval *return_value);
PHPAPI void php_explode(pval *delim, pval *str, pval *return_value, int limit);
PHPAPI inline char *php_memnstr(char *haystack, char *needle, int needle_len, char *end);
PHPAPI size_t php_strspn(char *s1, char *s2, char *s1_end, char *s2_end); 
PHPAPI size_t php_strcspn(char *s1, char *s2, char *s1_end, char *s2_end); 

#ifndef HAVE_STRERROR
PHPAPI char *php_strerror(int errnum);
#define strerror php_strerror
#endif

void register_string_constants(INIT_FUNC_ARGS);

#endif /* PHP_STRING_H */
