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

#ifndef _LIST_H
#define _LIST_H

#include "zend_hash.h"
#include "zend_globals.h"

extern HashTable list_destructors;

typedef struct {
	void *ptr;
	int type;
	int refcount;
} list_entry;

typedef struct {
	void (*list_destructor)(void *);
	void (*plist_destructor)(void *);
	int module_number;
	int resource_id;
} list_destructors_entry;

#define register_list_destructors(ld,pld) _register_list_destructors((void (*)(void *))ld, (void (*)(void *))pld, module_number);
ZEND_API int _register_list_destructors(void (*ld)(void *), void (*pld)(void *), int module_number);

enum list_entry_type {
	LE_DB=1000
};

void list_entry_destructor(void *ptr);
void plist_entry_destructor(void *ptr);

int clean_module_resource_destructors(list_destructors_entry *ld, int *module_number);
int init_resource_list(ELS_D);
int init_resource_plist(ELS_D);
void destroy_resource_list(void);
void destroy_resource_plist(void);

int zend_list_insert(void *ptr, int type);
int zend_plist_insert(void *ptr, int type);
int zend_list_addref(int id);
int zend_list_delete(int id);
int zend_plist_delete(int id);
void *zend_list_find(int id, int *type);
void *zend_plist_find(int id, int *type);

#endif
