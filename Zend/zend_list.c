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
#include "zend_list.h"
#include "zend_API.h"
#include "zend_globals.h"

#if 0
#if HAVE_DIRENT_H
#include <dirent.h>
#endif

#if MSVC5
#if !(APACHE)
#define NEEDRDH 1
#endif
#include "win32/readdir.h"
#endif
#endif


int le_index_ptr;

static inline int zend_list_do_insert(HashTable *list,void *ptr, int type)
{
	int index;
	list_entry le;

	index = zend_hash_next_free_element(list);

	if (index==0) index++;

	le.ptr=ptr;
	le.type=type;
	le.refcount=1;
	zend_hash_index_update(list, index, (void *) &le, sizeof(list_entry), NULL);
	return index;
}

static inline int zend_list_do_delete(HashTable *list,int id)
{
	list_entry *le;
	ELS_FETCH();
	
	if (zend_hash_index_find(&EG(regular_list), id, (void **) &le)==SUCCESS) {
//		printf("del(%d): %d->%d\n", id, le->refcount, le->refcount-1);
		if (--le->refcount<=0) {
			return zend_hash_index_del(&EG(regular_list), id);
		} else {
			return SUCCESS;
		}
	} else {
		return FAILURE;
	}
}


static inline void *zend_list_do_find(HashTable *list,int id, int *type)
{
	list_entry *le;

	if (zend_hash_index_find(list, id, (void **) &le)==SUCCESS) {
		*type = le->type;
		return le->ptr;
	} else {
		*type = -1;
		return NULL;
	}
}


int zend_list_insert(void *ptr, int type)
{
	ELS_FETCH();

	return zend_list_do_insert(&EG(regular_list), ptr, type);
}


int zend_plist_insert(void *ptr, int type)
{
	ELS_FETCH();

	return zend_list_do_insert(&EG(persistent_list), ptr, type);
}


int zend_list_delete(int id)
{
	ELS_FETCH();

	return zend_list_do_delete(&EG(regular_list), id);
}


int zend_list_addref(int id)
{
	list_entry *le;
	ELS_FETCH();
	
	if (zend_hash_index_find(&EG(regular_list), id, (void **) &le)==SUCCESS) {
//		printf("add(%d): %d->%d\n", id, le->refcount, le->refcount+1);
		le->refcount++;
		return SUCCESS;
	} else {
		return FAILURE;
	}
}

int zend_plist_delete(int id)
{
	ELS_FETCH();

	return zend_list_do_delete(&EG(persistent_list), id);
}


void *zend_list_find(int id, int *type)
{
	ELS_FETCH();

	return zend_list_do_find(&EG(regular_list), id, type);
}


void *zend_plist_find(int id, int *type)
{
	ELS_FETCH();

	return zend_list_do_find(&EG(persistent_list), id, type);
}


void list_entry_destructor(void *ptr)
{
	list_entry *le = (list_entry *) ptr;
	list_destructors_entry *ld;
	
	if (zend_hash_index_find(&list_destructors,le->type,(void **) &ld)==SUCCESS) {
		if (ld->list_destructor) {
			(ld->list_destructor)(le->ptr);
		}
	} else {
		zend_error(E_WARNING,"Unknown list entry type in request shutdown (%d)",le->type);
	}
}


void plist_entry_destructor(void *ptr)
{
	list_entry *le = (list_entry *) ptr;
	list_destructors_entry *ld;

	if (zend_hash_index_find(&list_destructors,le->type,(void **) &ld)==SUCCESS) {
		if (ld->plist_destructor) {
			(ld->plist_destructor)(le->ptr);
		}
	} else {
		zend_error(E_WARNING,"Unknown persistent list entry type in module shutdown (%d)",le->type);
	}
}


int init_resource_list(void)
{
	ELS_FETCH();

	return zend_hash_init(&EG(regular_list), 0, NULL, list_entry_destructor, 0);
}


int init_resource_plist(void)
{
	ELS_FETCH();

	return zend_hash_init(&EG(persistent_list), 0, NULL, plist_entry_destructor, 1);
}


void destroy_resource_list(void)
{
	ELS_FETCH();

	zend_hash_destroy(&EG(regular_list));
}


void destroy_resource_plist(void)
{
	ELS_FETCH();

	zend_hash_destroy(&EG(persistent_list));
}


static int clean_module_resource(list_entry *le, int *resource_id)
{
	if (le->type == *resource_id) {
		return 1;
	} else {
		return 0;
	}
}


int clean_module_resource_destructors(list_destructors_entry *ld, int *module_number)
{
	if (ld->module_number == *module_number) {
		ELS_FETCH();

		zend_hash_apply_with_argument(&EG(persistent_list), (int (*)(void *,void *)) clean_module_resource, (void *) &(ld->resource_id));
		return 1;
	} else {
		return 0;
	}
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
