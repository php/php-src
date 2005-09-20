/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2005 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Derick Rethans <derick@php.net>                             |
  +----------------------------------------------------------------------+
*/

#include "php_filter.h"
#include "filter_private.h"
#include "ext/standard/php_smart_str.h"

static void php_filter_encode_html(zval *value, char* chars)
{
	register int x, y;
	smart_str str = {0};
	int len = Z_STRLEN_P(value);
	char *s = Z_STRVAL_P(value);

	for (x = 0, y = 0; len--; x++, y++) {
		if (strchr(chars, s[x])) {
			smart_str_appendl(&str, "&#", 2);
			smart_str_append_long(&str, s[x]);
			smart_str_appendc(&str, ';');
		} else {
			smart_str_appendc(&str, s[x]);
		}
	}
	smart_str_0(&str);
	efree(Z_STRVAL_P(value));
	Z_STRVAL_P(value) = str.c;
	Z_STRLEN_P(value) = str.len;
}

static unsigned char hexchars[] = "0123456789ABCDEF";

static void php_filter_encode_url(zval *value, char* chars)
{
	register int x, y;
	unsigned char *str;
	int len = Z_STRLEN_P(value);
	char *s = Z_STRVAL_P(value);

	str = (unsigned char *) safe_emalloc(3, len, 1);
	for (x = 0, y = 0; len--; x++, y++) {
		str[y] = (unsigned char) s[x];

		if (strchr(chars, str[y])) {
			str[y++] = '%';
			str[y++] = hexchars[(unsigned char) s[x] >> 4];
			str[y] = hexchars[(unsigned char) s[x] & 15];
		}
	}
	str[y] = '\0';
	efree(Z_STRVAL_P(value));
	Z_STRVAL_P(value) = str;
	Z_STRLEN_P(value) = y;
}

static void php_filter_strip(zval *value, long flags)
{
	unsigned char *buf, *str;
	int   i, c;
	
	/* Optimization for if no strip flags are set */
	if (! ((flags & FILTER_FLAG_STRIP_LOW) || (flags & FILTER_FLAG_STRIP_HIGH)) ) {
		return;
	}

	str = Z_STRVAL_P(value);
	buf = safe_emalloc(1, Z_STRLEN_P(value) + 1, 1);
	c = 0;
	for (i = 0; i < Z_STRLEN_P(value); i++) {
		if ((str[i] > 127) && (flags & FILTER_FLAG_STRIP_HIGH)) {
		} else if ((str[i] < 32) && (flags & FILTER_FLAG_STRIP_LOW)) {
		} else {
			buf[c] = str[i];
			++c;
		}
	}
	/* update zval string data */
	buf[c] = '\0';
	efree(Z_STRVAL_P(value));
	Z_STRVAL_P(value) = buf;
	Z_STRLEN_P(value) = c;
}

void php_filter_string(PHP_INPUT_FILTER_PARAM_DECL)
{
	size_t new_len;
	
	/* strip tags */
	new_len = php_strip_tags(Z_STRVAL_P(value), Z_STRLEN_P(value), NULL, NULL, 0);
	Z_STRLEN_P(value) = new_len;
	
	if (! (flags & FILTER_FLAG_NO_ENCODE_QUOTES)) {
		/* encode ' and " to numerical entity */
		php_filter_encode_html(value, "'\"");
	}
	/* strip high/strip low ( see flags )*/
	php_filter_strip(value, flags);
}

void php_filter_encoded(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* urlencode */
	/* also all the flags - & encode as %xx */
}

void php_filter_special_chars(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* encodes ' " < > & \0 to numerical entities */
	/* if strip low is not set, then we encode them as %xx */
	/* encode_low doesn. tmake sense - update specs */
}

void php_filter_unsafe_raw(PHP_INPUT_FILTER_PARAM_DECL)
{
}

void php_filter_email(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* Check section 6 of rfc 822 http://www.faqs.org/rfcs/rfc822.html */
}

#define LOWALPHA    "abcdefghijklmnopqrstuvwxyz"
#define HIALPHA     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DIGIT       "0123456789"
#define SAFE        "$-_.+"
#define EXTRA       "!*'(),"
#define NATIONAL    "{}|\\^~[]`"
#define PUNCTUATION "<>#%\""

#define RESERVED    ";/?:@&="

void php_filter_url(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* Strip all chars not part of section 5 of
	 * http://www.faqs.org/rfcs/rfc1738.html */
	char *allowed_list = LOWALPHA HIALPHA DIGIT SAFE EXTRA NATIONAL PUNCTUATION RESERVED;
}

void php_filter_number_int(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* strip everything [^0-9+-] */
	char *allowed_list = "+-" DIGIT;
}

void php_filter_number_float(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* strip everything [^0-9+-] */
	char *allowed_list = "+-" DIGIT;
	/* depending on flags, strip '.', 'e', ",", "'" */
}

void php_filter_magic_quotes(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* just call magic quotes */
}

