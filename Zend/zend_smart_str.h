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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

#ifndef ZEND_SMART_STR_H
#define ZEND_SMART_STR_H

#include <zend.h>
#include "zend_globals.h"
#include "zend_smart_str_public.h"

BEGIN_EXTERN_C()

ZEND_API void ZEND_FASTCALL smart_str_erealloc(smart_str *str, size_t len);
ZEND_API void ZEND_FASTCALL smart_str_realloc(smart_str *str, size_t len);
ZEND_API void ZEND_FASTCALL smart_str_append_escaped(smart_str *str, const char *s, size_t l);
/* If zero_fraction is true, then a ".0" will be added to numbers that would not otherwise
 * have a fractional part and look like integers. */
ZEND_API void ZEND_FASTCALL smart_str_append_double(
		smart_str *str, double num, int precision, bool zero_fraction);
ZEND_API void smart_str_append_printf(smart_str *dest, const char *format, ...)
	ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);
ZEND_API void ZEND_FASTCALL smart_str_append_escaped_truncated(smart_str *str, const zend_string *value, size_t length);
ZEND_API void ZEND_FASTCALL smart_str_append_scalar(smart_str *str, const zval *value, size_t truncate);
END_EXTERN_C()

static zend_always_inline size_t smart_str_alloc(smart_str *str, size_t len, bool persistent) {
	if (UNEXPECTED(!str->s)) {
		goto do_smart_str_realloc;
	} else {
		len += ZSTR_LEN(str->s);
		if (UNEXPECTED(len >= str->a)) {
do_smart_str_realloc:
			if (persistent) {
				smart_str_realloc(str, len);
			} else {
				smart_str_erealloc(str, len);
			}
		}
	}
	return len;
}

static zend_always_inline char* smart_str_extend_ex(smart_str *dest, size_t len, bool persistent) {
	size_t new_len = smart_str_alloc(dest, len, persistent);
	char *ret = ZSTR_VAL(dest->s) + ZSTR_LEN(dest->s);
	ZSTR_LEN(dest->s) = new_len;
	return ret;
}

static zend_always_inline char* smart_str_extend(smart_str *dest, size_t length)
{
	return smart_str_extend_ex(dest, length, false);
}

static zend_always_inline void smart_str_free_ex(smart_str *str, bool persistent) {
	if (str->s) {
		zend_string_release_ex(str->s, persistent);
		str->s = NULL;
	}
	str->a = 0;
}

static zend_always_inline void smart_str_free(smart_str *str)
{
	smart_str_free_ex(str, false);
}

static zend_always_inline void smart_str_0(smart_str *str) {
	if (str->s) {
		ZSTR_VAL(str->s)[ZSTR_LEN(str->s)] = '\0';
	}
}

static zend_always_inline size_t smart_str_get_len(smart_str *str) {
	return str->s ? ZSTR_LEN(str->s) : 0;
}

static zend_always_inline void smart_str_trim_to_size_ex(smart_str *str, bool persistent)
{
	if (str->s && str->a > ZSTR_LEN(str->s)) {
		str->s = zend_string_realloc(str->s, ZSTR_LEN(str->s), persistent);
		str->a = ZSTR_LEN(str->s);
	}
}

static zend_always_inline void smart_str_trim_to_size(smart_str *dest)
{
	smart_str_trim_to_size_ex(dest, false);
}

static zend_always_inline zend_string *smart_str_extract_ex(smart_str *str, bool persistent) {
	if (str->s) {
		zend_string *res;
		smart_str_0(str);
		smart_str_trim_to_size_ex(str, persistent);
		res = str->s;
		str->s = NULL;
		return res;
	} else {
		return ZSTR_EMPTY_ALLOC();
	}
}

static zend_always_inline zend_string *smart_str_extract(smart_str *dest)
{
	return smart_str_extract_ex(dest, false);
}

static zend_always_inline void smart_str_appendc_ex(smart_str *dest, char ch, bool persistent) {
	size_t new_len = smart_str_alloc(dest, 1, persistent);
	ZSTR_VAL(dest->s)[new_len - 1] = ch;
	ZSTR_LEN(dest->s) = new_len;
}

static zend_always_inline void smart_str_appendl_ex(smart_str *dest, const char *str, size_t len, bool persistent) {
	size_t new_len = smart_str_alloc(dest, len, persistent);
	memcpy(ZSTR_VAL(dest->s) + ZSTR_LEN(dest->s), str, len);
	ZSTR_LEN(dest->s) = new_len;
}

static zend_always_inline void smart_str_append_ex(smart_str *dest, const zend_string *src, bool persistent) {
	smart_str_appendl_ex(dest, ZSTR_VAL(src), ZSTR_LEN(src), persistent);
}

static zend_always_inline void smart_str_append_smart_str_ex(smart_str *dest, const smart_str *src, bool persistent) {
	if (src->s && ZSTR_LEN(src->s)) {
		smart_str_append_ex(dest, src->s, persistent);
	}
}

static zend_always_inline void smart_str_append_long_ex(smart_str *dest, zend_long num, bool persistent) {
	char buf[32];
	char *result = zend_print_long_to_buf(buf + sizeof(buf) - 1, num);
	smart_str_appendl_ex(dest, result, buf + sizeof(buf) - 1 - result, persistent);
}

static zend_always_inline void smart_str_append_long(smart_str *dest, zend_long num)
{
	smart_str_append_long_ex(dest, num, false);
}

static zend_always_inline void smart_str_append_unsigned_ex(smart_str *dest, zend_ulong num, bool persistent) {
	char buf[32];
	char *result = zend_print_ulong_to_buf(buf + sizeof(buf) - 1, num);
	smart_str_appendl_ex(dest, result, buf + sizeof(buf) - 1 - result, persistent);
}

static zend_always_inline void smart_str_append_unsigned(smart_str *dest, zend_ulong num)
{
	smart_str_append_unsigned_ex(dest, num, false);
}

static zend_always_inline void smart_str_appendl(smart_str *dest, const char *src, size_t length)
{
	smart_str_appendl_ex(dest, src, length, false);
}
static zend_always_inline void smart_str_appends_ex(smart_str *dest, const char *src, bool persistent)
{
	smart_str_appendl_ex(dest, src, strlen(src), persistent);
}
static zend_always_inline void smart_str_appends(smart_str *dest, const char *src)
{
	smart_str_appendl_ex(dest, src, strlen(src), false);
}
static zend_always_inline void smart_str_append(smart_str *dest, const zend_string *src)
{
	smart_str_append_ex(dest, src, false);
}
static zend_always_inline void smart_str_appendc(smart_str *dest, char ch)
{
	smart_str_appendc_ex(dest, ch, false);
}
static zend_always_inline void smart_str_append_smart_str(smart_str *dest, const smart_str *src)
{
	smart_str_append_smart_str_ex(dest, src, false);
}

static zend_always_inline void smart_str_setl(smart_str *dest, const char *src, size_t len) {
	smart_str_free(dest);
	smart_str_appendl(dest, src, len);
}

static zend_always_inline void smart_str_sets(smart_str *dest, const char *src)
{
	smart_str_setl(dest, src, strlen(src));
}
#endif
