/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
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

typedef struct {
	zend_llist_element *head;
	zend_llist_element *tail;
	size_t size;
	void (*dtor)(void *data);
	unsigned char persistent;
	zend_llist_element *traverse_ptr;
} zend_llist;

ZEND_API void zend_llist_init(zend_llist *l, size_t size, void (*dtor)(void *data), unsigned char persistent);
ZEND_API void zend_llist_add_element(zend_llist *l, void *element);
ZEND_API void zend_llist_del_element(zend_llist *l, void *element);
ZEND_API void zend_llist_destroy(zend_llist *l);
ZEND_API void zend_llist_clean(zend_llist *l);
ZEND_API void zend_llist_remove_tail(zend_llist *l);
ZEND_API void zend_llist_copy(zend_llist *dst, zend_llist *src);
ZEND_API void zend_llist_apply(zend_llist *l, void (*func)(void *data));
ZEND_API void zend_llist_apply_with_argument(zend_llist *l, void (*func)(void *data, void *arg), void *arg);
ZEND_API int zend_llist_count(zend_llist *l);

/* traversal */
ZEND_API void *zend_llist_get_first(zend_llist *l);
ZEND_API void *zend_llist_get_last(zend_llist *l);
ZEND_API void *zend_llist_get_next(zend_llist *l);
ZEND_API void *zend_llist_get_prev(zend_llist *l);

#endif /* _ZEND_LLIST_H */
