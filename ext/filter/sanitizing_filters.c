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
  | Authors: Derick Rethans <derick@php.net>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php_filter.h"
#include "filter_private.h"
#include "zend_smart_str.h"

/* {{{ STRUCTS */
typedef unsigned long filter_map[256];
/* }}} */

/* {{{ HELPER FUNCTIONS */
static void php_filter_encode_html(zval *value, const unsigned char *chars)
{
	smart_str str = {0};
	size_t len = Z_STRLEN_P(value);
	unsigned char *s = (unsigned char *)Z_STRVAL_P(value);
	unsigned char *e = s + len;

	if (Z_STRLEN_P(value) == 0) {
		return;
	}

	while (s < e) {
		if (chars[*s]) {
			smart_str_appendl(&str, "&#", 2);
			smart_str_append_unsigned(&str, (zend_ulong)*s);
			smart_str_appendc(&str, ';');
		} else {
			/* XXX: this needs to be optimized to work with blocks of 'safe' chars */
			smart_str_appendc(&str, *s);
		}
		s++;
	}

	smart_str_0(&str);
	zval_ptr_dtor(value);
	ZVAL_NEW_STR(value, str.s);
}

static const unsigned char hexchars[] = "0123456789ABCDEF";

#define LOWALPHA    "abcdefghijklmnopqrstuvwxyz"
#define HIALPHA     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DIGIT       "0123456789"

#define DEFAULT_URL_ENCODE    LOWALPHA HIALPHA DIGIT "-._"

static void php_filter_encode_url(zval *value, const unsigned char* chars, const int char_len, int high, int low, int encode_nul)
{
	unsigned char *p;
	unsigned char tmp[256];
	unsigned char *s = (unsigned char *)chars;
	unsigned char *e = s + char_len;
	zend_string *str;

	memset(tmp, 1, sizeof(tmp)-1);

	while (s < e) {
		tmp[*s++] = '\0';
	}
/* XXX: This is not needed since these chars in the allowed list never include the high/low/null value
	if (encode_nul) {
		tmp[0] = 1;
	}
	if (high) {
		memset(tmp + 127, 1, sizeof(tmp) - 127);
	}
	if (low) {
		memset(tmp, 1, 32);
	}
*/
	str = zend_string_alloc(3 * Z_STRLEN_P(value), 0);
	p = (unsigned char *) ZSTR_VAL(str);
	s = (unsigned char *) Z_STRVAL_P(value);
	e = s + Z_STRLEN_P(value);

	while (s < e) {
		if (tmp[*s]) {
			*p++ = '%';
			*p++ = hexchars[(unsigned char) *s >> 4];
			*p++ = hexchars[(unsigned char) *s & 15];
		} else {
			*p++ = *s;
		}
		s++;
	}
	*p = '\0';
	ZSTR_LEN(str) = p - (unsigned char *)ZSTR_VAL(str);
	zval_ptr_dtor(value);
	ZVAL_NEW_STR(value, str);
}

static void php_filter_strip(zval *value, zend_long flags)
{
	unsigned char *str;
	int   i, c;
	zend_string *buf;

	/* Optimization for if no strip flags are set */
	if (!(flags & (FILTER_FLAG_STRIP_LOW | FILTER_FLAG_STRIP_HIGH | FILTER_FLAG_STRIP_BACKTICK))) {
		return;
	}

	str = (unsigned char *)Z_STRVAL_P(value);
	buf = zend_string_alloc(Z_STRLEN_P(value) + 1, 0);
	c = 0;
	for (i = 0; i < Z_STRLEN_P(value); i++) {
		if ((str[i] >= 127) && (flags & FILTER_FLAG_STRIP_HIGH)) {
		} else if ((str[i] < 32) && (flags & FILTER_FLAG_STRIP_LOW)) {
		} else if ((str[i] == '`') && (flags & FILTER_FLAG_STRIP_BACKTICK)) {
		} else {
			ZSTR_VAL(buf)[c] = str[i];
			++c;
		}
	}
	/* update zval string data */
	ZSTR_VAL(buf)[c] = '\0';
	ZSTR_LEN(buf) = c;
	zval_ptr_dtor(value);
	ZVAL_NEW_STR(value, buf);
}
/* }}} */

/* {{{ FILTER MAP HELPERS */
static void filter_map_init(filter_map *map)
{
	memset(map, 0, sizeof(filter_map));
}

static void filter_map_update(filter_map *map, int flag, const unsigned char *allowed_list)
{
	size_t l, i;

	l = strlen((const char*)allowed_list);
	for (i = 0; i < l; ++i) {
		(*map)[allowed_list[i]] = flag;
	}
}

static void filter_map_apply(zval *value, filter_map *map)
{
	unsigned char *str;
	int   i, c;
	zend_string *buf;

	str = (unsigned char *)Z_STRVAL_P(value);
	buf = zend_string_alloc(Z_STRLEN_P(value) + 1, 0);
	c = 0;
	for (i = 0; i < Z_STRLEN_P(value); i++) {
		if ((*map)[str[i]]) {
			ZSTR_VAL(buf)[c] = str[i];
			++c;
		}
	}
	/* update zval string data */
	ZSTR_VAL(buf)[c] = '\0';
	ZSTR_LEN(buf) = c;
	zval_ptr_dtor(value);
	ZVAL_NEW_STR(value, buf);
}
/* }}} */

/* {{{ php_filter_string */
void php_filter_string(PHP_INPUT_FILTER_PARAM_DECL)
{
	size_t new_len;
	unsigned char enc[256] = {0};

	if (!Z_REFCOUNTED_P(value)) {
		ZVAL_STRINGL(value, Z_STRVAL_P(value), Z_STRLEN_P(value));
	}

	/* strip high/strip low ( see flags )*/
	php_filter_strip(value, flags);

	if (!(flags & FILTER_FLAG_NO_ENCODE_QUOTES)) {
		enc['\''] = enc['"'] = 1;
	}
	if (flags & FILTER_FLAG_ENCODE_AMP) {
		enc['&'] = 1;
	}
	if (flags & FILTER_FLAG_ENCODE_LOW) {
		memset(enc, 1, 32);
	}
	if (flags & FILTER_FLAG_ENCODE_HIGH) {
		memset(enc + 127, 1, sizeof(enc) - 127);
	}

	php_filter_encode_html(value, enc);

	/* strip tags, implicitly also removes \0 chars */
	new_len = php_strip_tags_ex(Z_STRVAL_P(value), Z_STRLEN_P(value), NULL, NULL, 0, 1);
	Z_STRLEN_P(value) = new_len;

	if (new_len == 0) {
		zval_dtor(value);
		if (flags & FILTER_FLAG_EMPTY_STRING_NULL) {
			ZVAL_NULL(value);
		} else {
			ZVAL_EMPTY_STRING(value);
		}
		return;
	}
}
/* }}} */

/* {{{ php_filter_encoded */
void php_filter_encoded(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* apply strip_high and strip_low filters */
	php_filter_strip(value, flags);
	/* urlencode */
	php_filter_encode_url(value, (unsigned char *)DEFAULT_URL_ENCODE, sizeof(DEFAULT_URL_ENCODE)-1, flags & FILTER_FLAG_ENCODE_HIGH, flags & FILTER_FLAG_ENCODE_LOW, 1);
}
/* }}} */

/* {{{ php_filter_special_chars */
void php_filter_special_chars(PHP_INPUT_FILTER_PARAM_DECL)
{
	unsigned char enc[256] = {0};

	php_filter_strip(value, flags);

	/* encodes ' " < > & \0 to numerical entities */
	enc['\''] = enc['"'] = enc['<'] = enc['>'] = enc['&'] = enc[0] = 1;

	/* if strip low is not set, then we encode them as &#xx; */
	memset(enc, 1, 32);

	if (flags & FILTER_FLAG_ENCODE_HIGH) {
		memset(enc + 127, 1, sizeof(enc) - 127);
	}

	php_filter_encode_html(value, enc);
}
/* }}} */

/* {{{ php_filter_full_special_chars */
void php_filter_full_special_chars(PHP_INPUT_FILTER_PARAM_DECL)
{
	zend_string *buf;
	int quotes;

	if (!(flags & FILTER_FLAG_NO_ENCODE_QUOTES)) {
		quotes = ENT_QUOTES;
	} else {
		quotes = ENT_NOQUOTES;
	}
	buf = php_escape_html_entities_ex((unsigned char *) Z_STRVAL_P(value), Z_STRLEN_P(value), 1, quotes, SG(default_charset), 0);
	zval_ptr_dtor(value);
	ZVAL_STR(value, buf);
}
/* }}} */

/* {{{ php_filter_unsafe_raw */
void php_filter_unsafe_raw(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* Only if no flags are set (optimization) */
	if (flags != 0 && Z_STRLEN_P(value) > 0) {
		unsigned char enc[256] = {0};

		php_filter_strip(value, flags);

		if (flags & FILTER_FLAG_ENCODE_AMP) {
			enc['&'] = 1;
		}
		if (flags & FILTER_FLAG_ENCODE_LOW) {
			memset(enc, 1, 32);
		}
		if (flags & FILTER_FLAG_ENCODE_HIGH) {
			memset(enc + 127, 1, sizeof(enc) - 127);
		}

		php_filter_encode_html(value, enc);
	} else if (flags & FILTER_FLAG_EMPTY_STRING_NULL && Z_STRLEN_P(value) == 0) {
		zval_dtor(value);
		ZVAL_NULL(value);
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
	const unsigned char allowed_list[] = LOWALPHA HIALPHA DIGIT "!#$%&'*+-=?^_`{|}~@.[]";
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
		filter_map_update(&map, 2, (const unsigned char *) ".");
	}
	if (flags & FILTER_FLAG_ALLOW_THOUSAND) {
		filter_map_update(&map, 3,  (const unsigned char *) ",");
	}
	if (flags & FILTER_FLAG_ALLOW_SCIENTIFIC) {
		filter_map_update(&map, 4,  (const unsigned char *) "eE");
	}
	filter_map_apply(value, &map);
}
/* }}} */

/* {{{ php_filter_magic_quotes */
void php_filter_magic_quotes(PHP_INPUT_FILTER_PARAM_DECL)
{
	zend_string *buf;

	/* just call php_addslashes quotes */
	buf = php_addslashes(Z_STR_P(value), 0);

	zval_ptr_dtor(value);
	ZVAL_STR(value, buf);
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
