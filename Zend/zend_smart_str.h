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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

#ifndef ZEND_SMART_STR_H
#define ZEND_SMART_STR_H

#include <zend.h>
#include "zend_smart_str_public.h"

#ifndef SMART_STR_PREALLOC
#define SMART_STR_PREALLOC 128
#endif

#ifndef SMART_STR_START_SIZE
#define SMART_STR_START_SIZE 78
#endif

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

static zend_always_inline size_t smart_str_alloc(smart_str *str, size_t len, zend_bool persistent) {
	size_t newlen;
	if (!str->s) {
		newlen = len;
		str->a = newlen < SMART_STR_START_SIZE
				? SMART_STR_START_SIZE
				: newlen + SMART_STR_PREALLOC;
		str->s = zend_string_alloc(str->a, persistent);
		str->s->len = 0;
	} else {
		newlen = str->s->len + len;
		if (newlen >= str->a) {
			str->a = newlen + SMART_STR_PREALLOC;
			str->s = (zend_string *) perealloc(str->s, _STR_HEADER_SIZE + str->a + 1, persistent);
		}
	}
	return newlen;
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
		str->s->val[str->s->len] = '\0';
	}
}

static zend_always_inline void smart_str_appendc_ex(smart_str *dest, char ch, zend_bool persistent) {
	size_t new_len = smart_str_alloc(dest, 1, persistent);
	dest->s->val[new_len - 1] = ch;
	dest->s->len = new_len;
}

static zend_always_inline void smart_str_appendl_ex(smart_str *dest, const char *str, size_t len, zend_bool persistent) {
	size_t new_len = smart_str_alloc(dest, len, persistent);
	memcpy(dest->s->val + dest->s->len, str, len);
	dest->s->len = new_len;
}

static zend_always_inline void smart_str_append_ex(smart_str *dest, const zend_string *src, zend_bool persistent) {
	smart_str_appendl_ex(dest, src->val, src->len, persistent);
}

static zend_always_inline void smart_str_append_smart_str_ex(smart_str *dest, const smart_str *src, zend_bool persistent) {
	if (src->s && src->s->len) {
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

