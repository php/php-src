/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2000 Andi Gutmans, Zeev Suraski                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.91 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_91.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef _ZEND_LLIST_H
#define _ZEND_LLIST_H

#include <stdlib.h>

typedef struct _zend_llist_element {
	struct _zend_llist_element *next;
	struct _zend_llist_element *prev;
	char data[1]; /* Needs to always be last in the struct */
} zend_llist_element;

typedef struct _zend_llist {
	zend_llist_element *head;
	zend_llist_element *tail;
	size_t size;
	void (*dtor)(void *data);
	unsigned char persistent;
	zend_llist_element *traverse_ptr;
} zend_llist;

typedef int (*llist_compare_func_t) (const zend_llist_element *, const zend_llist_element *);

BEGIN_EXTERN_C()
ZEND_API void zend_llist_init(zend_llist *l, size_t size, void (*dtor)(void *data), unsigned char persistent);
ZEND_API void zend_llist_add_element(zend_llist *l, void *element);
ZEND_API void zend_llist_prepend_element(zend_llist *l, void *element);
ZEND_API void zend_llist_del_element(zend_llist *l, void *element, int (*compare)(void *element1, void *element2));
ZEND_API void zend_llist_destroy(zend_llist *l);
ZEND_API void zend_llist_clean(zend_llist *l);
ZEND_API void zend_llist_remove_tail(zend_llist *l);
ZEND_API void zend_llist_copy(zend_llist *dst, zend_llist *src);
ZEND_API void zend_llist_apply(zend_llist *l, void (*func)(void *data));
ZEND_API void zend_llist_apply_with_argument(zend_llist *l, void (*func)(void *data, void *arg), void *arg);
ZEND_API int zend_llist_count(zend_llist *l);
ZEND_API void zend_llist_sort(zend_llist *l, llist_compare_func_t comp_func);

/* traversal */
ZEND_API void *zend_llist_get_first(zend_llist *l);
ZEND_API void *zend_llist_get_last(zend_llist *l);
ZEND_API void *zend_llist_get_next(zend_llist *l);
ZEND_API void *zend_llist_get_prev(zend_llist *l);
END_EXTERN_C()

#endif /* _ZEND_LLIST_H */
