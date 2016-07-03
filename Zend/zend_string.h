/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id: $ */

#ifndef ZEND_STRING_H
#define ZEND_STRING_H

#include "zend.h"

BEGIN_EXTERN_C()
ZEND_API extern const char *(*zend_new_interned_string)(const char *str, int len, int free_src TSRMLS_DC);
ZEND_API extern void (*zend_interned_strings_snapshot)(TSRMLS_D);
ZEND_API extern void (*zend_interned_strings_restore)(TSRMLS_D);

void zend_interned_strings_init(TSRMLS_D);
void zend_interned_strings_dtor(TSRMLS_D);
END_EXTERN_C()

#ifndef ZTS

#define IS_INTERNED(s) \
	(((s) >= CG(interned_strings_start)) && ((s) < CG(interned_strings_end)))

#else

#define IS_INTERNED(s) \
	(0)

#endif

#define INTERNED_LEN(s) \
	(((Bucket*)(((char*)(s))-sizeof(Bucket)))->nKeyLength)

#define INTERNED_HASH(s) \
	(((Bucket*)(((char*)(s))-sizeof(Bucket)))->h)

#define str_efree(s) do { \
		if (!IS_INTERNED(s)) { \
			efree((char*)s); \
		} \
	} while (0)

#define str_efree_rel(s) do { \
		if (!IS_INTERNED(s)) { \
			efree_rel((char *)s); \
		} \
	} while (0)

#define str_free(s) do { \
		if (!IS_INTERNED(s)) { \
			free((char*)s); \
		} \
	} while (0)

#define str_erealloc(str, new_len) \
	(IS_INTERNED(str) \
	 ? _str_erealloc(str, new_len, INTERNED_LEN(str)) \
	 : erealloc(str, new_len))

static inline char *_str_erealloc(char *str, size_t new_len, size_t old_len) {
	char *buf = (char *) emalloc(new_len);
	memcpy(buf, str, old_len);
	return buf;
}

#define str_estrndup(str, len) \
	(IS_INTERNED(str) ? (str) : estrndup((str), (len)))

#define str_strndup(str, len) \
	(IS_INTERNED(str) ? (str) : zend_strndup((str), (len)));

#define str_hash(str, len) \
	(IS_INTERNED(str) ? INTERNED_HASH(str) : zend_hash_func((str), (len)+1))


#endif /* ZEND_STRING_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
