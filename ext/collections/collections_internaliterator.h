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
   | Author: Tyson Andre <tandre@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef COLLECTIONS_INTERNALITERATOR_H
#define COLLECTIONS_INTERNALITERATOR_H

typedef struct _collections_intrusive_dllist_node {
	struct _collections_intrusive_dllist_node *prev;
	struct _collections_intrusive_dllist_node *next;
} collections_intrusive_dllist_node;

typedef struct _collections_intrusive_dllist {
	struct _collections_intrusive_dllist_node *first;
} collections_intrusive_dllist;

static zend_always_inline void collections_intrusive_dllist_prepend(collections_intrusive_dllist *list, collections_intrusive_dllist_node *node) {
	collections_intrusive_dllist_node *first = list->first;
	ZEND_ASSERT(node != first);
	node->next = first;
	node->prev = NULL;
	list->first = node;

	if (first) {
		ZEND_ASSERT(first->prev == NULL);
		first->prev = node;
	}
}

static zend_always_inline void collections_intrusive_dllist_remove(collections_intrusive_dllist *list, const collections_intrusive_dllist_node *node) {
	collections_intrusive_dllist_node *next = node->next;
	collections_intrusive_dllist_node *prev = node->prev;
	ZEND_ASSERT(node != next);
	ZEND_ASSERT(node != prev);
	ZEND_ASSERT(next != prev || next == NULL);
	if (next) {
		next->prev = prev;
	}
	if (list->first == node) {
		list->first = next;
		ZEND_ASSERT(prev == NULL);
	} else if (prev) {
		prev->next = next;
	}
}
#endif
