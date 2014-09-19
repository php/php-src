/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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

/* $Id$ */

#ifndef PHP_SMART_STR_H
#define PHP_SMART_STR_H

#include "php_smart_str_public.h"

#include <stdlib.h>
#ifndef SMART_STR_USE_REALLOC
#include <zend.h>
#endif

#define smart_str_0(x) do {											\
	if ((x)->s) {													\
		(x)->s->val[(x)->s->len] = '\0';							\
	}																\
} while (0)

#ifndef SMART_STR_PREALLOC
#define SMART_STR_PREALLOC 128
#endif

#ifndef SMART_STR_START_SIZE
#define SMART_STR_START_SIZE 78
#endif


#ifdef SMART_STR_USE_REALLOC
#define SMART_STR_DO_REALLOC(b, w) do {								\
	int oldlen = (b)->s->len;										\
	(b)->s = erealloc((buf)->s, _STR_HEADER_SIZE + (b)->a + 1);		\
	(b)->s->len = oldlen;											\
} while (0)
#else
#define SMART_STR_DO_REALLOC(b, w) do {								\
	(b)->s = perealloc((b)->s, _STR_HEADER_SIZE + (b)->a + 1, (w));	\
} while (0)
#endif


#define smart_str_alloc4(d, n, what, newlen) do {					\
	if (!(d)->s) {													\
		newlen = (n);												\
		(d)->a = newlen < SMART_STR_START_SIZE 						\
				? SMART_STR_START_SIZE 								\
				: newlen + SMART_STR_PREALLOC;						\
		(d)->s = zend_string_alloc((d)->a, (what));							\
		(d)->s->len = 0;											\
	} else {														\
		newlen = (d)->s->len + (n);									\
		if (newlen >= (d)->a) {										\
			(d)->a = newlen + SMART_STR_PREALLOC;					\
			SMART_STR_DO_REALLOC((d), (what));						\
		}															\
	}																\
} while (0)

#define smart_str_alloc(d, n, what) \
	smart_str_alloc4((d), (n), (what), newlen)

/* wrapper */

#define smart_str_appends_ex(dest, src, what) \
	smart_str_appendl_ex((dest), (src), strlen(src), (what))
#define smart_str_appends(dest, src) \
	smart_str_appendl((dest), (src), strlen(src))

#define smart_str_appendc(dest, c) \
	smart_str_appendc_ex((dest), (c), 0)
#define smart_str_free(s) \
	smart_str_free_ex((s), 0)
#define smart_str_appendl(dest, src, len) \
	smart_str_appendl_ex((dest), (src), (len), 0)
#define smart_str_append(dest, src) \
	smart_str_append_ex((dest), (src), 0)
#define smart_str_append_long(dest, val) \
	smart_str_append_long_ex((dest), (val), 0)
#define smart_str_append_off_t(dest, val) \
	smart_str_append_off_t_ex((dest), (val), 0)
#define smart_str_append_unsigned(dest, val) \
	smart_str_append_unsigned_ex((dest), (val), 0)

#define smart_str_appendc_ex(dest, ch, what) do {				   \
	register size_t __nl;										   \
	smart_str_alloc4((dest), 1, (what), __nl);					   \
	(dest)->s->len = __nl;										   \
	((unsigned char *) (dest)->s->val)[(dest)->s->len - 1] = (ch); \
} while (0)

#define smart_str_free_ex(buf, what) do {							\
	smart_str *__s = (smart_str *) (buf);							\
	if (__s->s) {													\
		zend_string_release(__s->s);										\
		__s->s = NULL;												\
	}																\
	__s->a = 0;														\
} while (0)

#define smart_str_appendl_ex(dest, src, nlen, what) do {			\
	register size_t __nl;											\
	smart_str *__dest = (smart_str *) (dest);						\
																	\
	smart_str_alloc4(__dest, (nlen), (what), __nl);					\
	memcpy(__dest->s->val + __dest->s->len, (src), (nlen));			\
	__dest->s->len = __nl;											\
} while (0)

/*
 * these could be replaced using a braced-group inside an expression
 * for GCC compatible compilers, e.g.
 *
 * #define f(..) ({char *r;..;__r;})
 */  
 
static inline char *smart_str_print_long(char *buf, zend_long num) {
	char *r; 
	_zend_print_signed_to_buf(buf, num, zend_long, r); 
	return r;
}

static inline char *smart_str_print_unsigned(char *buf, zend_long num) {
	char *r; 
	_zend_print_unsigned_to_buf(buf, num, zend_ulong, r); 
	return r;
}

#define smart_str_append_generic_ex(dest, num, type, vartype, func) do {	\
	char __b[32];															\
	char *__t;																\
   	_zend_print##func##_to_buf (__b + sizeof(__b) - 1, (num), vartype, __t);	\
	smart_str_appendl_ex((dest), __t, __b + sizeof(__b) - 1 - __t, (type));	\
} while (0)
	
#define smart_str_append_unsigned_ex(dest, num, type) \
	smart_str_append_generic_ex((dest), (num), (type), zend_ulong, _unsigned)

#define smart_str_append_long_ex(dest, num, type) \
	smart_str_append_generic_ex((dest), (num), (type), zend_ulong, _signed)

#define smart_str_append_off_t_ex(dest, num, type) \
	smart_str_append_generic_ex((dest), (num), (type), zend_off_t, _signed)

#define smart_str_append_ex(dest, src, what) do {							\
	if ((src)->s && (src)->s->len) {										\
		smart_str_appendl_ex((dest), (src)->s->val, (src)->s->len, (what));	\
	}																		\
} while(0)

#define smart_str_setl(dest, src, nlen) do {						\
	smart_str_free((dest));											\
	smart_str_appendl_ex((dest), (src), (nlen), 0);					\
} while (0)

#define smart_str_sets(dest, src) \
	smart_str_setl((dest), (src), strlen(src));

#endif
