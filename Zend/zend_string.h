/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2012 Zend Technologies Ltd. (http://www.zend.com) |
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

ZEND_API extern const char *(*zend_new_interned_string)(const char *str, int len, int free_src TSRMLS_DC);
ZEND_API extern void (*zend_interned_strings_snapshot)(TSRMLS_D);
ZEND_API extern void (*zend_interned_strings_restore)(TSRMLS_D);

void zend_interned_strings_init(TSRMLS_D);
void zend_interned_strings_dtor(TSRMLS_D);

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

#define str_free(s) do { \
		if (!IS_INTERNED(s)) { \
			free((char*)s); \
		} \
	} while (0)

#endif /* ZEND_STRING_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
