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
  | Authors: Derick Rethans <derick@php.net>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php_filter.h"
#include "filter_private.h"
#include "ext/standard/php_smart_str.h"

/* {{{ STRUCTS */
typedef unsigned long filter_map[256];
/* }}} */

/* {{{ HELPER FUNCTIONS */
static void php_filter_encode_html(zval *value, const char* chars, int encode_nul)
{
	register int x, y;
	smart_str str = {0};
	int len = Z_STRLEN_P(value);
	char *s = Z_STRVAL_P(value);

	if (Z_STRLEN_P(value) == 0) {
		return;
	}

	for (x = 0, y = 0; len--; x++, y++) {
		if (strchr(chars, s[x]) || (encode_nul && s[x] == 0)) {
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

static void php_filter_encode_html_high_low(zval *value, long flags)
{
	register int x, y;
	smart_str str = {0};
	int len = Z_STRLEN_P(value);
	unsigned char *s = (unsigned char *)Z_STRVAL_P(value);

	if (Z_STRLEN_P(value) == 0) {
		return;
	}
	
	for (x = 0, y = 0; len--; x++, y++) {
		if (((flags & FILTER_FLAG_ENCODE_LOW) && (s[x] < 32)) || ((flags & FILTER_FLAG_ENCODE_HIGH) && (s[x] > 127))) {
			smart_str_appendl(&str, "&#", 2);
			smart_str_append_unsigned(&str, s[x]);
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

static const unsigned char hexchars[] = "0123456789ABCDEF";

#define LOWALPHA    "abcdefghijklmnopqrstuvwxyz"
#define HIALPHA     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DIGIT       "0123456789"

#define DEFAULT_URL_ENCODE    LOWALPHA HIALPHA DIGIT "-._"

static void php_filter_encode_url(zval *value, const char* chars, int high, int low, int encode_nul)
{
	register int x, y;
	unsigned char *str;
	int len = Z_STRLEN_P(value);
	char *s = Z_STRVAL_P(value);

	str = (unsigned char *) safe_emalloc(3, len, 1);
	for (x = 0, y = 0; len--; x++, y++) {
		str[y] = (unsigned char) s[x];

		if ((strlen(chars) && !strchr(chars, str[y])) || (high && str[y] > 127) || (low && str[y] < 32) || (encode_nul && str[y] == 0)) {
			str[y++] = '%';
			str[y++] = hexchars[(unsigned char) s[x] >> 4];
			str[y] = hexchars[(unsigned char) s[x] & 15];
		}
	}
	str[y] = '\0';
	efree(Z_STRVAL_P(value));
	Z_STRVAL_P(value) = (char *)str;
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

	str = (unsigned char *)Z_STRVAL_P(value);
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
	Z_STRVAL_P(value) = (char *)buf;
	Z_STRLEN_P(value) = c;
}
/* }}} */

/* {{{ FILTER MAP HELPERS */
static void filter_map_init(filter_map *map)
{
	memset(map, 0, sizeof(filter_map));
}

static void filter_map_update(filter_map *map, int flag, const unsigned char *allowed_list)
{
	int l, i;

	l = strlen((const char*)allowed_list);
	for (i = 0; i < l; ++i) {
		(*map)[allowed_list[i]] = flag;
	}
}

static void filter_map_apply(zval *value, filter_map *map)
{
	unsigned char *buf, *str;
	int   i, c;
	
	str = (unsigned char *)Z_STRVAL_P(value);
	buf = safe_emalloc(1, Z_STRLEN_P(value) + 1, 1);
	c = 0;
	for (i = 0; i < Z_STRLEN_P(value); i++) {
		if ((*map)[str[i]]) {
			buf[c] = str[i];
			++c;
		}
	}
	/* update zval string data */
	buf[c] = '\0';
	efree(Z_STRVAL_P(value));
	Z_STRVAL_P(value) = (char *)buf;
	Z_STRLEN_P(value) = c;
}
/* }}} */


/* {{{ php_filter_string */
void php_filter_string(PHP_INPUT_FILTER_PARAM_DECL)
{
	size_t new_len;
	
	/* strip tags, implicitly also removes \0 chars */
	new_len = php_strip_tags(Z_STRVAL_P(value), Z_STRLEN_P(value), NULL, NULL, 0);
	Z_STRLEN_P(value) = new_len;

	if (new_len == 0) {
		zval_dtor(value);
		ZVAL_EMPTY_STRING(value);
		return;
	}

	if (! (flags & FILTER_FLAG_NO_ENCODE_QUOTES)) {
		/* encode ' and " to numerical entity */
		php_filter_encode_html(value, "'\"", 0);
	}
	/* strip high/strip low ( see flags )*/
	php_filter_strip(value, flags);

	/* encode low/encode high flags */
	php_filter_encode_html_high_low(value, flags);

	/* also all the flags - & encode as %xx */
	if (flags & FILTER_FLAG_ENCODE_AMP) {
		php_filter_encode_html(value, "&", 0);
	}
}
/* }}} */

/* {{{ php_filter_encoded */
void php_filter_encoded(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* apply strip_high and strip_low filters */
	php_filter_strip(value, flags);
	/* urlencode */
	php_filter_encode_url(value, DEFAULT_URL_ENCODE, flags & FILTER_FLAG_ENCODE_HIGH, flags & FILTER_FLAG_ENCODE_LOW, 1);
}
/* }}} */

/* {{{ php_filter_special_chars */
void php_filter_special_chars(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* encodes ' " < > & \0 to numerical entities */
	php_filter_encode_html(value, "'\"<>&", 1);
	/* if strip low is not set, then we encode them as &#xx; */
	php_filter_strip(value, flags);
	php_filter_encode_html_high_low(value, FILTER_FLAG_ENCODE_LOW | flags);
}
/* }}} */

/* {{{ php_filter_unsafe_raw */
void php_filter_unsafe_raw(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* Only if no flags are set (optimization) */
	if (flags != 0 && Z_STRLEN_P(value) > 0) {
		php_filter_strip(value, flags);
		if (flags & FILTER_FLAG_ENCODE_AMP) {
			php_filter_encode_html(value, "&", 0);
		}
		php_filter_encode_html_high_low(value, flags);
	}
}
/* }}} */

/* {{{ php_filter_email */
#define SAFE        "$-_.+"
#define EXTRA       "!*'(),"
#define NATIONAL    "{}|\\^~[]`"
#define PUNCTUATION "<>#%\""
#define RESERVED    ";/?:@&="

void php_filter_email(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* Check section 6 of rfc 822 http://www.faqs.org/rfcs/rfc822.html */
	const unsigned char allowed_list[] = LOWALPHA HIALPHA DIGIT "!#$%&'*+-/=?^_`{|}~@.[]";
	filter_map     map;

	filter_map_init(&map);
	filter_map_update(&map, 1, allowed_list);
	filter_map_apply(value, &map);
}
/* }}} */

/* {{{ php_filter_url */
void php_filter_url(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* Strip all chars not part of section 5 of
	 * http://www.faqs.org/rfcs/rfc1738.html */
	const unsigned char allowed_list[] = LOWALPHA HIALPHA DIGIT SAFE EXTRA NATIONAL PUNCTUATION RESERVED;
	filter_map     map;

	filter_map_init(&map);
	filter_map_update(&map, 1, allowed_list);
	filter_map_apply(value, &map);
}
/* }}} */

/* {{{ php_filter_number_int */
void php_filter_number_int(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* strip everything [^0-9+-] */
	const unsigned char allowed_list[] = "+-" DIGIT;
	filter_map     map;

	filter_map_init(&map);
	filter_map_update(&map, 1, allowed_list);
	filter_map_apply(value, &map);
}
/* }}} */

/* {{{ php_filter_number_float */
void php_filter_number_float(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* strip everything [^0-9+-] */
	const unsigned char allowed_list[] = "+-" DIGIT;
	filter_map     map;

	filter_map_init(&map);
	filter_map_update(&map, 1, allowed_list);

	/* depending on flags, strip '.', 'e', ",", "'" */
	if (flags & FILTER_FLAG_ALLOW_FRACTION) {
		filter_map_update(&map, 2, (unsigned char *) ".");
	}
	if (flags & FILTER_FLAG_ALLOW_THOUSAND) {
		filter_map_update(&map, 3,  (unsigned char *) ",");
	}
	if (flags & FILTER_FLAG_ALLOW_SCIENTIFIC) {
		filter_map_update(&map, 4,  (unsigned char *) "eE");
	}
	filter_map_apply(value, &map);
}
/* }}} */

/* {{{ php_filter_magic_quotes */
void php_filter_magic_quotes(PHP_INPUT_FILTER_PARAM_DECL)
{
	char *buf;
	int   len;
	
	/* just call php_addslashes quotes */
	buf = php_addslashes(Z_STRVAL_P(value), Z_STRLEN_P(value), &len, 0 TSRMLS_CC);

	efree(Z_STRVAL_P(value));
	Z_STRVAL_P(value) = buf;
	Z_STRLEN_P(value) = len;
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
