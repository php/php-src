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

#include "zend.h"
#include "zend_llist.h"

ZEND_API void zend_llist_init(zend_llist *l, size_t size, void (*dtor)(void *data), unsigned char persistent)
{
	l->head = NULL;
	l->tail = NULL;
	l->size = size;
	l->dtor = dtor;
	l->persistent = persistent;
}


ZEND_API void zend_llist_add_element(zend_llist *l, void *element)
{
	zend_llist_element *tmp = pemalloc(sizeof(zend_llist_element)+l->size-1, l->persistent);

	tmp->prev = l->tail;
	tmp->next = NULL;
	if (l->tail) {
		l->tail->next = tmp;
	} else {
		l->head = tmp;
	}
	l->tail = tmp;
	memcpy(tmp->data, element, l->size);
}


ZEND_API void zend_llist_del_element(zend_llist *l, void *element)
{
	zend_llist_element *current=l->head;

	while (current) {
		if (current->data == element) {
			if (current->prev) {
				current->prev->next = current->next;
			} else {
				l->head = current->next;
			}
			if (current->next) {
				current->next->prev = current->prev;
			} else {
				l->tail = current->prev;
			}
			if (l->dtor) {
				l->dtor(current->data);
				efree(current);
			}
			break;
		}
		current = current->next;
	}
}


ZEND_API void zend_llist_destroy(zend_llist *l)
{
	zend_llist_element *current=l->head, *next;

	while (current) {
		next = current->next;
		if (l->dtor) {
			l->dtor(current->data);
		}
		pefree(current, l->persistent);
		current = next;
	}
}

ZEND_API void zend_llist_remove_tail(zend_llist *l)
{
	zend_llist_element *old_tail;

	if ((old_tail = l->tail)) {
		if (l->tail->prev) {
			l->tail->prev->next = NULL;
		}
		l->tail = l->tail->prev;
		efree(old_tail);
	}
}


ZEND_API void zend_llist_copy(zend_llist *dst, zend_llist *src)
{
	zend_llist_element *ptr;

	zend_llist_init(dst, src->size, src->dtor, src->persistent);
	ptr = src->head;
	while (ptr) {
		zend_llist_add_element(dst, ptr->data);
		ptr = ptr->next;
	}
}


ZEND_API void zend_llist_apply(zend_llist *l, void (*func)(void *data))
{
	zend_llist_element *element;

	for (element=l->head; element; element=element->next) {
		func(element->data);
	}
}


ZEND_API void zend_llist_apply_with_argument(zend_llist *l, void (*func)(void *data, void *arg), void *arg)
{
	zend_llist_element *element;

	for (element=l->head; element; element=element->next) {
		func(element->data, arg);
	}
}


ZEND_API int zend_llist_count(zend_llist *l)
{
	zend_llist_element *element;
	int element_count=0;

	for (element=l->head; element; element=element->next) {
		element_count++;
	}
	return element_count;
}


ZEND_API void *zend_llist_get_first(zend_llist *l)
{
	l->traverse_ptr = l->head;
	if (l->traverse_ptr) {
		return l->traverse_ptr->data;
	} else {
		return NULL;
	}
}


ZEND_API void *zend_llist_get_last(zend_llist *l)
{
	l->traverse_ptr = l->tail;
	if (l->traverse_ptr) {
		return l->traverse_ptr->data;
	} else {
		return NULL;
	}
}


ZEND_API void *zend_llist_get_next(zend_llist *l)
{
	if (l->traverse_ptr) {
		l->traverse_ptr = l->traverse_ptr->next;
		if (l->traverse_ptr) {
			return l->traverse_ptr->data;
		}
	}
	return NULL;
}


ZEND_API void *zend_llist_get_prev(zend_llist *l)
{
	if (l->traverse_ptr) {
		l->traverse_ptr = l->traverse_ptr->prev;
		if (l->traverse_ptr) {
			return l->traverse_ptr->data;
		}
	}
	return NULL;
}
