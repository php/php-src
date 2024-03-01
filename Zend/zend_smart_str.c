/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Dmitry Stogov <dmitry@php.net>                               |
   +----------------------------------------------------------------------+
 */

#include <zend.h>
#include "zend_smart_str.h"
#include "zend_smart_string.h"

#define SMART_STR_OVERHEAD   (ZEND_MM_OVERHEAD + _ZSTR_HEADER_SIZE + 1)
#define SMART_STR_START_SIZE 256
#define SMART_STR_START_LEN  (SMART_STR_START_SIZE - SMART_STR_OVERHEAD)
#define SMART_STR_PAGE       4096

#define SMART_STR_NEW_LEN(len) \
	(ZEND_MM_ALIGNED_SIZE_EX(len + SMART_STR_OVERHEAD, SMART_STR_PAGE) - SMART_STR_OVERHEAD)

ZEND_API void ZEND_FASTCALL smart_str_erealloc(smart_str *str, size_t len)
{
	if (UNEXPECTED(!str->s)) {
		str->a = len <= SMART_STR_START_LEN
				? SMART_STR_START_LEN
				: SMART_STR_NEW_LEN(len);
		str->s = zend_string_alloc(str->a, 0);
		ZSTR_LEN(str->s) = 0;
	} else {
		str->a = SMART_STR_NEW_LEN(len);
		str->s = (zend_string *) erealloc2(str->s, str->a + _ZSTR_HEADER_SIZE + 1, _ZSTR_HEADER_SIZE + ZSTR_LEN(str->s));
	}
}

ZEND_API void ZEND_FASTCALL smart_str_realloc(smart_str *str, size_t len)
{
	if (UNEXPECTED(!str->s)) {
		str->a = len <= SMART_STR_START_LEN
				? SMART_STR_START_LEN
				: SMART_STR_NEW_LEN(len);
		str->s = zend_string_alloc(str->a, 1);
		ZSTR_LEN(str->s) = 0;
	} else {
		str->a = SMART_STR_NEW_LEN(len);
		str->s = (zend_string *) perealloc(str->s, str->a + _ZSTR_HEADER_SIZE + 1, 1);
	}
}

/* Windows uses VK_ESCAPE instead of \e */
#ifndef VK_ESCAPE
#define VK_ESCAPE '\e'
#endif

static size_t zend_compute_escaped_string_len(const char *s, size_t l) {
	size_t i, len = l;
	for (i = 0; i < l; ++i) {
		char c = s[i];
		if (c == '\n' || c == '\r' || c == '\t' ||
			c == '\f' || c == '\v' || c == '\\' || c == VK_ESCAPE) {
			len += 1;
		} else if (c < 32 || c > 126) {
			len += 3;
		}
	}
	return len;
}

ZEND_API void ZEND_FASTCALL smart_str_append_escaped(smart_str *str, const char *s, size_t l) {
	char *res;
	size_t i, len = zend_compute_escaped_string_len(s, l);

	smart_str_alloc(str, len, 0);
	res = &ZSTR_VAL(str->s)[ZSTR_LEN(str->s)];
	ZSTR_LEN(str->s) += len;

	for (i = 0; i < l; ++i) {
		unsigned char c = s[i];
		if (c < 32 || c == '\\' || c > 126) {
			*res++ = '\\';
			switch (c) {
				case '\n': *res++ = 'n'; break;
				case '\r': *res++ = 'r'; break;
				case '\t': *res++ = 't'; break;
				case '\f': *res++ = 'f'; break;
				case '\v': *res++ = 'v'; break;
				case '\\': *res++ = '\\'; break;
				case VK_ESCAPE: *res++ = 'e'; break;
				default:
					*res++ = 'x';
					if ((c >> 4) < 10) {
						*res++ = (c >> 4) + '0';
					} else {
						*res++ = (c >> 4) + 'A' - 10;
					}
					if ((c & 0xf) < 10) {
						*res++ = (c & 0xf) + '0';
					} else {
						*res++ = (c & 0xf) + 'A' - 10;
					}
			}
		} else {
			*res++ = c;
		}
	}
}

ZEND_API void ZEND_FASTCALL smart_str_append_double(
		smart_str *str, double num, int precision, bool zero_fraction) {
	char buf[ZEND_DOUBLE_MAX_LENGTH];
	/* Model snprintf precision behavior. */
	zend_gcvt(num, precision ? precision : 1, '.', 'E', buf);
	smart_str_appends(str, buf);
	if (zero_fraction && zend_finite(num) && !strchr(buf, '.')) {
		smart_str_appendl(str, ".0", 2);
	}
}

ZEND_API void smart_str_append_printf(smart_str *dest, const char *format, ...) {
	va_list arg;
	va_start(arg, format);
	zend_printf_to_smart_str(dest, format, arg);
	va_end(arg);
}

#define SMART_STRING_OVERHEAD   (ZEND_MM_OVERHEAD + 1)
#define SMART_STRING_START_SIZE 256
#define SMART_STRING_START_LEN  (SMART_STRING_START_SIZE - SMART_STRING_OVERHEAD)
#define SMART_STRING_PAGE       4096

ZEND_API void ZEND_FASTCALL _smart_string_alloc_persistent(smart_string *str, size_t len)
{
	if (!str->c) {
		str->len = 0;
		if (len <= SMART_STRING_START_LEN) {
			str->a = SMART_STRING_START_LEN;
		} else {
			str->a = ZEND_MM_ALIGNED_SIZE_EX(len + SMART_STRING_OVERHEAD, SMART_STRING_PAGE) - SMART_STRING_OVERHEAD;
		}
		str->c = pemalloc(str->a + 1, 1);
	} else {
		if (UNEXPECTED((size_t) len > SIZE_MAX - str->len)) {
			zend_error_noreturn(E_ERROR, "String size overflow");
		}
		len += str->len;
		str->a = ZEND_MM_ALIGNED_SIZE_EX(len + SMART_STRING_OVERHEAD, SMART_STRING_PAGE) - SMART_STRING_OVERHEAD;
		str->c = perealloc(str->c, str->a + 1, 1);
	}
}

ZEND_API void ZEND_FASTCALL _smart_string_alloc(smart_string *str, size_t len)
{
	if (!str->c) {
		str->len = 0;
		if (len <= SMART_STRING_START_LEN) {
			str->a = SMART_STRING_START_LEN;
			str->c = emalloc(SMART_STRING_START_LEN + 1);
		} else {
			str->a = ZEND_MM_ALIGNED_SIZE_EX(len + SMART_STRING_OVERHEAD, SMART_STRING_PAGE) - SMART_STRING_OVERHEAD;
			if (EXPECTED(str->a < (ZEND_MM_CHUNK_SIZE - SMART_STRING_OVERHEAD))) {
				str->c = emalloc_large(str->a + 1);
			} else {
				/* allocate a huge chunk */
				str->c = emalloc(str->a + 1);
			}
		}
	} else {
		if (UNEXPECTED((size_t) len > SIZE_MAX - str->len)) {
			zend_error_noreturn(E_ERROR, "String size overflow");
		}
		len += str->len;
		str->a = ZEND_MM_ALIGNED_SIZE_EX(len + SMART_STRING_OVERHEAD, SMART_STRING_PAGE) - SMART_STRING_OVERHEAD;
		str->c = erealloc2(str->c, str->a + 1, str->len);
	}
}

ZEND_API void ZEND_FASTCALL smart_str_append_escaped_truncated(smart_str *str, const zend_string *value, size_t length)
{
	smart_str_append_escaped(str, ZSTR_VAL(value), MIN(length, ZSTR_LEN(value)));

	if (ZSTR_LEN(value) > length) {
		smart_str_appendl(str, "...", sizeof("...")-1);
	}
}

ZEND_API void ZEND_FASTCALL smart_str_append_scalar(smart_str *dest, const zval *value, size_t truncate) {
	ZEND_ASSERT(Z_TYPE_P(value) <= IS_STRING);

	switch (Z_TYPE_P(value)) {
		case IS_UNDEF:
		case IS_NULL:
			smart_str_appendl(dest, "NULL", sizeof("NULL")-1);
		break;

		case IS_TRUE:
		case IS_FALSE:
			smart_str_appends(dest, Z_TYPE_P(value) == IS_TRUE ? "true" : "false");
		break;

		case IS_DOUBLE:
			smart_str_append_double(dest, Z_DVAL_P(value), (int) EG(precision), true);
		break;

		case IS_LONG:
			smart_str_append_long(dest, Z_LVAL_P(value));
		break;

		case IS_STRING:
			smart_str_appendc(dest, '\'');
			smart_str_append_escaped_truncated(dest, Z_STR_P(value), truncate);
			smart_str_appendc(dest, '\'');
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}
}
