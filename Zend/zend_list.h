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


#ifndef _LIST_H
#define _LIST_H

#include "zend_hash.h"
#include "zend_globals.h"

extern HashTable list_destructors;

typedef struct _list_entry {
	void *ptr;
	int type;
	int refcount;
} list_entry;

typedef struct _list_destructors_entry {
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
void destroy_resource_list(ELS_D);
void destroy_resource_plist(ELS_D);

ZEND_API int zend_list_insert(void *ptr, int type);
ZEND_API int zend_plist_insert(void *ptr, int type);
ZEND_API int zend_list_addref(int id);
ZEND_API int zend_list_delete(int id);
ZEND_API int zend_plist_delete(int id);
ZEND_API void *zend_list_find(int id, int *type);
ZEND_API void *zend_plist_find(int id, int *type);

ZEND_API int zend_register_resource(zval *rsrc_result, void *rsrc_pointer, int rsrc_type);
ZEND_API void *zend_fetch_resource(zval **passed_id, int default_id, char *resource_type_name, int *found_resource_type, int num_resource_types, ...);

extern ZEND_API int le_index_ptr;  /* list entry type for index pointers */

#define ZEND_VERIFY_RESOURCE(rsrc)		\
	if (!rsrc) {						\
		RETURN_FALSE;					\
	}

#define ZEND_FETCH_RESOURCE(rsrc, rsrc_type, passed_id, default_id, resource_type_name, resource_type)	\
	rsrc = (rsrc_type) zend_fetch_resource(passed_id, default_id, resource_type_name, NULL, 1, resource_type);	\
	ZEND_VERIFY_RESOURCE(rsrc);

#define ZEND_FETCH_RESOURCE2(rsrc, rsrc_type, passed_id, default_id, resource_type_name, resource_type1,resource_type2)	\
	rsrc = (rsrc_type) zend_fetch_resource(passed_id, default_id, resource_type_name, NULL, 2, resource_type1, resource_type2);	\
	ZEND_VERIFY_RESOURCE(rsrc);

#define ZEND_REGISTER_RESOURCE(rsrc_result, rsrc_pointer, rsrc_type)  \
    zend_register_resource(rsrc_result, rsrc_pointer, rsrc_type);

#endif
