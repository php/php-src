/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
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


/* resource lists */

#include "zend.h"
#include "zend_list.h"
#include "zend_API.h"
#include "zend_globals.h"


ZEND_API int le_index_ptr;

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
/*		printf("del(%d): %d->%d\n", id, le->refcount, le->refcount-1); */
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


ZEND_API int zend_list_insert(void *ptr, int type)
{
	ELS_FETCH();

	return zend_list_do_insert(&EG(regular_list), ptr, type);
}


ZEND_API int zend_plist_insert(void *ptr, int type)
{
	ELS_FETCH();

	return zend_list_do_insert(&EG(persistent_list), ptr, type);
}


ZEND_API int zend_list_addref(int id)
{
	list_entry *le;
	ELS_FETCH();
	
	if (zend_hash_index_find(&EG(regular_list), id, (void **) &le)==SUCCESS) {
/*		printf("add(%d): %d->%d\n", id, le->refcount, le->refcount+1); */
		le->refcount++;
		return SUCCESS;
	} else {
		return FAILURE;
	}
}


ZEND_API int zend_list_delete(int id)
{
	ELS_FETCH();

	return zend_list_do_delete(&EG(regular_list), id);
}


ZEND_API int zend_plist_delete(int id)
{
	ELS_FETCH();

	return zend_list_do_delete(&EG(persistent_list), id);
}


ZEND_API void *zend_list_find(int id, int *type)
{
	ELS_FETCH();

	return zend_list_do_find(&EG(regular_list), id, type);
}


ZEND_API void *zend_plist_find(int id, int *type)
{
	ELS_FETCH();

	return zend_list_do_find(&EG(persistent_list), id, type);
}


ZEND_API void *zend_fetch_resource(zval *passed_id, int default_id, char *resource_type_name, int resource_type)
{
	return zend_fetch_resource_ex(passed_id, default_id, resource_type_name, 1, resource_type);
}


ZEND_API void *zend_fetch_resource_ex(zval *passed_id, int default_id, char *resource_type_name, int num_resource_types, ...)
{
	int id;
	int actual_resource_type;
	void *resource;
	va_list resource_types;
	int i;

	if (default_id==-1) { /* use id */
		if (passed_id->type != IS_RESOURCE) {
			zend_error(E_WARNING, "Supplied argument is not a valid %s resource", resource_type_name);
			return NULL;
		}
		id = passed_id->value.lval;
	} else {
		id = default_id;
	}

	resource = zend_list_find(id, &actual_resource_type);
	if (!resource) {
		zend_error(E_WARNING, "%d is not a valid %s resource", resource_type_name);
		return NULL;
	}

	va_start(resource_types, num_resource_types);
	for (i=0; i<num_resource_types; i++) {
		if (actual_resource_type == va_arg(resource_types, int)) {
			va_end(resource_types);
			return resource;
		}
	}
	va_end(resource_types);
	return NULL;
}


int list_entry_destructor(void *ptr)
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
	return 1;
}


int plist_entry_destructor(void *ptr)
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
	return 1;
}


int init_resource_list(ELS_D)
{
	return zend_hash_init(&EG(regular_list), 0, NULL, list_entry_destructor, 0);
}


int init_resource_plist(ELS_D)
{
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
