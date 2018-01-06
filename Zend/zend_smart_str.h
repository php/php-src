/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

#define smart_str_appends_ex(dest, src, what) \
	smart_str_appendl_ex((dest), (src), strlen(src), (what))
#define smart_str_appends(dest, src) \
	smart_str_appendl((dest), (src), strlen(src))
#define smart_str_appendc(dest, c) \
	smart_str_appendc_ex((dest), (c), 0)
#define smart_str_appendl(dest, src, len) \
	smart_str_appendl_ex((dest), (src), (len), 0)
#define smart_str_append(dest, src) \
	smart_str_append_ex((dest), (src), 0)
#define smart_str_append_smart_str(dest, src) \
	smart_str_append_smart_str_ex((dest), (src), 0)
#define smart_str_sets(dest, src) \
	smart_str_setl((dest), (src), strlen(src));
#define smart_str_append_long(dest, val) \
	smart_str_append_long_ex((dest), (val), 0)
#define smart_str_append_unsigned(dest, val) \
	smart_str_append_unsigned_ex((dest), (val), 0)

BEGIN_EXTERN_C()

ZEND_API void ZEND_FASTCALL smart_str_erealloc(smart_str *str, size_t len);
ZEND_API void ZEND_FASTCALL smart_str_realloc(smart_str *str, size_t len);
ZEND_API void ZEND_FASTCALL smart_str_append_escaped(smart_str *str, const char *s, size_t l);
ZEND_API void ZEND_FASTCALL smart_str_append_printf(smart_str *dest, const char *format, ...)
	ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);

END_EXTERN_C()

static zend_always_inline size_t smart_str_alloc(smart_str *str, size_t len, zend_bool persistent) {
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

static zend_always_inline void smart_str_free(smart_str *str) {
	if (str->s) {
		zend_string_release(str->s);
		str->s = NULL;
	}
	str->a = 0;
}

static zend_always_inline void smart_str_0(smart_str *str) {
	if (str->s) {
		ZSTR_VAL(str->s)[ZSTR_LEN(str->s)] = '\0';
	}
}

static zend_always_inline size_t smart_str_get_len(smart_str *str) {
	return str->s ? ZSTR_LEN(str->s) : 0;
}

static zend_always_inline zend_string *smart_str_extract(smart_str *str) {
	if (str->s) {
		zend_string *res;
		smart_str_0(str);
		res = str->s;
		str->s = NULL;
		return res;
	} else {
		return ZSTR_EMPTY_ALLOC();
	}
}

static zend_always_inline void smart_str_appendc_ex(smart_str *dest, char ch, zend_bool persistent) {
	size_t new_len = smart_str_alloc(dest, 1, persistent);
	ZSTR_VAL(dest->s)[new_len - 1] = ch;
	ZSTR_LEN(dest->s) = new_len;
}

static zend_always_inline void smart_str_appendl_ex(smart_str *dest, const char *str, size_t len, zend_bool persistent) {
	size_t new_len = smart_str_alloc(dest, len, persistent);
	memcpy(ZSTR_VAL(dest->s) + ZSTR_LEN(dest->s), str, len);
	ZSTR_LEN(dest->s) = new_len;
}

static zend_always_inline void smart_str_append_ex(smart_str *dest, const zend_string *src, zend_bool persistent) {
	smart_str_appendl_ex(dest, ZSTR_VAL(src), ZSTR_LEN(src), persistent);
}

static zend_always_inline void smart_str_append_smart_str_ex(smart_str *dest, const smart_str *src, zend_bool persistent) {
	if (src->s && ZSTR_LEN(src->s)) {
		smart_str_append_ex(dest, src->s, persistent);
	}
}

static zend_always_inline void smart_str_append_long_ex(smart_str *dest, zend_long num, zend_bool persistent) {
	char buf[32];
	char *result = zend_print_long_to_buf(buf + sizeof(buf) - 1, num);
	smart_str_appendl_ex(dest, result, buf + sizeof(buf) - 1 - result, persistent);
}

static zend_always_inline void smart_str_append_unsigned_ex(smart_str *dest, zend_ulong num, zend_bool persistent) {
	char buf[32];
	char *result = zend_print_ulong_to_buf(buf + sizeof(buf) - 1, num);
	smart_str_appendl_ex(dest, result, buf + sizeof(buf) - 1 - result, persistent);
}

static zend_always_inline void smart_str_setl(smart_str *dest, const char *src, size_t len) {
	smart_str_free(dest);
	smart_str_appendl(dest, src, len);
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
