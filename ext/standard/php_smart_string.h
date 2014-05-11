/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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
   |         Xinchen Hui <laruence@php.net>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_SMART_STRING_H
#define PHP_SMART_STRING_H

#include "php_smart_string_public.h"

#include <stdlib.h>
#ifndef SMART_STR_USE_REALLOC
#include <zend.h>
#endif

#define smart_string_0(x) do {										\
	if ((x)->c) {													\
		(x)->c[(x)->len] = '\0';									\
	}																\
} while (0)

#ifndef SMART_STRING_PREALLOC
#define SMART_STRING_PREALLOC 128
#endif

#ifndef SMART_STRING_START_SIZE
#define SMART_STRING_START_SIZE 78
#endif

#ifdef SMART_STRING_USE_REALLOC
#define SMART_STRING_REALLOC(a,b,c) realloc((a),(b))
#else
#define SMART_STRING_REALLOC(a,b,c) perealloc((a),(b),(c))
#endif

#define SMART_STRING_DO_REALLOC(d, what) \
	(d)->c = SMART_STRING_REALLOC((d)->c, (d)->a + 1, (what))

#define smart_string_alloc4(d, n, what, newlen) do {					\
	if (!(d)->c) {													\
		(d)->len = 0;												\
		newlen = (n);												\
		(d)->a = newlen < SMART_STRING_START_SIZE 						\
				? SMART_STRING_START_SIZE 								\
				: newlen + SMART_STRING_PREALLOC;						\
		SMART_STRING_DO_REALLOC(d, what);								\
	} else {														\
		newlen = (d)->len + (n);									\
		if (newlen >= (d)->a) {										\
			(d)->a = newlen + SMART_STRING_PREALLOC;					\
			SMART_STRING_DO_REALLOC(d, what);							\
		}															\
	}																\
} while (0)

#define smart_string_alloc(d, n, what) \
	smart_string_alloc4((d), (n), (what), newlen)

/* wrapper */

#define smart_string_appends_ex(dest, src, what) \
	smart_string_appendl_ex((dest), (src), strlen(src), (what))
#define smart_string_appends(dest, src) \
	smart_string_appendl((dest), (src), strlen(src))

#define smart_string_appendc(dest, c) \
	smart_string_appendc_ex((dest), (c), 0)
#define smart_string_free(s) \
	smart_string_free_ex((s), 0)
#define smart_string_appendl(dest, src, len) \
	smart_string_appendl_ex((dest), (src), (len), 0)
#define smart_string_append(dest, src) \
	smart_string_append_ex((dest), (src), 0)
#define smart_string_append_long(dest, val) \
	smart_string_append_long_ex((dest), (val), 0)
#define smart_string_append_off_t(dest, val) \
	smart_string_append_off_t_ex((dest), (val), 0)
#define smart_string_append_unsigned(dest, val) \
	smart_string_append_unsigned_ex((dest), (val), 0)

#define smart_string_appendc_ex(dest, ch, what) do {					\
	register size_t __nl;											\
	smart_string_alloc4((dest), 1, (what), __nl);						\
	(dest)->len = __nl;												\
	((unsigned char *) (dest)->c)[(dest)->len - 1] = (ch);			\
} while (0)

#define smart_string_free_ex(s, what) do {								\
	smart_string *__s = (smart_string *) (s);								\
	if (__s->c) {													\
		pefree(__s->c, what);										\
		__s->c = NULL;												\
	}																\
	__s->a = __s->len = 0;											\
} while (0)

#define smart_string_appendl_ex(dest, src, nlen, what) do {			\
	register size_t __nl;											\
	smart_string *__dest = (smart_string *) (dest);						\
																	\
	smart_string_alloc4(__dest, (nlen), (what), __nl);					\
	memcpy(__dest->c + __dest->len, (src), (nlen));					\
	__dest->len = __nl;												\
} while (0)

/* input: buf points to the END of the buffer */
#define smart_string_print_unsigned4(buf, num, vartype, result) do {	\
	char *__p = (buf);												\
	vartype __num = (num);											\
	*__p = '\0';													\
	do {															\
		*--__p = (char) (__num % 10) + '0';							\
		__num /= 10;												\
	} while (__num > 0);											\
	result = __p;													\
} while (0)

/* buf points to the END of the buffer */
#define smart_string_print_long4(buf, num, vartype, result) do {	\
	if (num < 0) {													\
		/* this might cause problems when dealing with LONG_MIN		\
		   and machines which don't support long long.  Works		\
		   flawlessly on 32bit x86 */								\
		smart_string_print_unsigned4((buf), -(num), vartype, (result));	\
		*--(result) = '-';											\
	} else {														\
		smart_string_print_unsigned4((buf), (num), vartype, (result));	\
	}																\
} while (0)

/*
 * these could be replaced using a braced-group inside an expression
 * for GCC compatible compilers, e.g.
 *
 * #define f(..) ({char *r;..;__r;})
 */  
 
static inline char *smart_string_print_long(char *buf, long num) {
	char *r; 
	smart_string_print_long4(buf, num, unsigned long, r); 
	return r;
}

static inline char *smart_string_print_unsigned(char *buf, long num) {
	char *r; 
	smart_string_print_unsigned4(buf, num, unsigned long, r); 
	return r;
}

#define smart_string_append_generic_ex(dest, num, type, vartype, func) do {	\
	char __b[32];															\
	char *__t;																\
   	smart_string_print##func##4 (__b + sizeof(__b) - 1, (num), vartype, __t);	\
	smart_string_appendl_ex((dest), __t, __b + sizeof(__b) - 1 - __t, (type));	\
} while (0)
	
#define smart_string_append_unsigned_ex(dest, num, type) \
	smart_string_append_generic_ex((dest), (num), (type), unsigned long, _unsigned)

#define smart_string_append_long_ex(dest, num, type) \
	smart_string_append_generic_ex((dest), (num), (type), unsigned long, _long)

#define smart_string_append_off_t_ex(dest, num, type) \
	smart_string_append_generic_ex((dest), (num), (type), off_t, _long)

#define smart_string_append_ex(dest, src, what) \
	smart_string_appendl_ex((dest), ((smart_string *)(src))->c, \
		((smart_string *)(src))->len, (what));


#define smart_string_setl(dest, src, nlen) do {						\
	(dest)->len = (nlen);											\
	(dest)->a = (nlen) + 1;											\
	(dest)->c = (char *) (src);										\
} while (0)

#define smart_string_sets(dest, src) \
	smart_string_setl((dest), (src), strlen(src));

#endif
