/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* resource lists */

#include "zend.h"
#include "zend_list.h"
#include "zend_API.h"
#include "zend_globals.h"

ZEND_API int le_index_ptr;

/* true global */
static HashTable list_destructors;


ZEND_API int zend_list_insert(void *ptr, int type TSRMLS_DC)
{
	int index;
	zend_rsrc_list_entry le;

	le.ptr=ptr;
	le.type=type;
	le.refcount=1;

	index = zend_hash_next_free_element(&EG(regular_list));

	zend_hash_index_update(&EG(regular_list), index, (void *) &le, sizeof(zend_rsrc_list_entry), NULL);
	return index;
}

ZEND_API int _zend_list_delete(int id TSRMLS_DC)
{
	zend_rsrc_list_entry *le;
	
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


ZEND_API void *_zend_list_find(int id, int *type TSRMLS_DC)
{
	zend_rsrc_list_entry *le;

	if (zend_hash_index_find(&EG(regular_list), id, (void **) &le)==SUCCESS) {
		*type = le->type;
		return le->ptr;
	} else {
		*type = -1;
		return NULL;
	}
}

ZEND_API int _zend_list_addref(int id TSRMLS_DC)
{
	zend_rsrc_list_entry *le;
	
	if (zend_hash_index_find(&EG(regular_list), id, (void **) &le)==SUCCESS) {
/*		printf("add(%d): %d->%d\n", id, le->refcount, le->refcount+1); */
		le->refcount++;
		return SUCCESS;
	} else {
		return FAILURE;
	}
}


ZEND_API int zend_register_resource(zval *rsrc_result, void *rsrc_pointer, int rsrc_type TSRMLS_DC)
{
	int rsrc_id;

	rsrc_id = zend_list_insert(rsrc_pointer, rsrc_type TSRMLS_CC);
	
	if (rsrc_result) {
		rsrc_result->value.lval = rsrc_id;
		rsrc_result->type = IS_RESOURCE;
	}

	return rsrc_id;
}


ZEND_API void *zend_fetch_resource(zval **passed_id TSRMLS_DC, int default_id, const char *resource_type_name, int *found_resource_type, int num_resource_types, ...)
{
	int id;
	int actual_resource_type;
	void *resource;
	va_list resource_types;
	int i;
	const char *space;
	const char *class_name;

	if (default_id==-1) { /* use id */
		if (!passed_id) {
			if (resource_type_name) {
				class_name = get_active_class_name(&space TSRMLS_CC);
				zend_error(E_WARNING, "%s%s%s(): no %s resource supplied", class_name, space, get_active_function_name(TSRMLS_C), resource_type_name);
			}
			return NULL;
		} else if ((*passed_id)->type != IS_RESOURCE) {
			if (resource_type_name) {
				class_name = get_active_class_name(&space TSRMLS_CC);
				zend_error(E_WARNING, "%s%s%s(): supplied argument is not a valid %s resource", class_name, space, get_active_function_name(TSRMLS_C), resource_type_name);
			}
			return NULL;
		}
		id = (*passed_id)->value.lval;
	} else {
		id = default_id;
	}

	resource = zend_list_find(id, &actual_resource_type);
	if (!resource) {
		if (resource_type_name) {
			class_name = get_active_class_name(&space TSRMLS_CC);
			zend_error(E_WARNING, "%s%s%s(): %d is not a valid %s resource", class_name, space, get_active_function_name(TSRMLS_C), id, resource_type_name);
		}
		return NULL;
	}

	va_start(resource_types, num_resource_types);
	for (i=0; i<num_resource_types; i++) {
		if (actual_resource_type == va_arg(resource_types, int)) {
			va_end(resource_types);
			if (found_resource_type) {
				*found_resource_type = actual_resource_type;
			}
			return resource;
		}
	}
	va_end(resource_types);

	if (resource_type_name) {
		class_name = get_active_class_name(&space TSRMLS_CC);
		zend_error(E_WARNING, "%s%s%s(): supplied resource is not a valid %s resource", class_name, space, get_active_function_name(TSRMLS_C), resource_type_name);
	}

	return NULL;
}


void list_entry_destructor(void *ptr)
{
	zend_rsrc_list_entry *le = (zend_rsrc_list_entry *) ptr;
	zend_rsrc_list_dtors_entry *ld;
	TSRMLS_FETCH();
	
	if (zend_hash_index_find(&list_destructors, le->type, (void **) &ld)==SUCCESS) {
		switch (ld->type) {
			case ZEND_RESOURCE_LIST_TYPE_STD:
				if (ld->list_dtor) {
					(ld->list_dtor)(le->ptr);
				}
				break;
			case ZEND_RESOURCE_LIST_TYPE_EX:
				if (ld->list_dtor_ex) {
					ld->list_dtor_ex(le TSRMLS_CC);
				}
				break;
			EMPTY_SWITCH_DEFAULT_CASE()
		}
	} else {
		zend_error(E_WARNING,"Unknown list entry type in request shutdown (%d)", le->type);
	}
}


void plist_entry_destructor(void *ptr)
{
	zend_rsrc_list_entry *le = (zend_rsrc_list_entry *) ptr;
	zend_rsrc_list_dtors_entry *ld;
	TSRMLS_FETCH();

	if (zend_hash_index_find(&list_destructors, le->type, (void **) &ld)==SUCCESS) {
		switch (ld->type) {
			case ZEND_RESOURCE_LIST_TYPE_STD:
				if (ld->plist_dtor) {
					(ld->plist_dtor)(le->ptr);
				}
				break;
			case ZEND_RESOURCE_LIST_TYPE_EX:
				if (ld->plist_dtor_ex) {
					ld->plist_dtor_ex(le TSRMLS_CC);
				}
				break;
				EMPTY_SWITCH_DEFAULT_CASE()
		}
	} else {
		zend_error(E_WARNING,"Unknown persistent list entry type in module shutdown (%d)", le->type);
	}
}


int zend_init_rsrc_list(TSRMLS_D)
{
	if (zend_hash_init(&EG(regular_list), 0, NULL, list_entry_destructor, 0)==SUCCESS) {
		EG(regular_list).nNextFreeElement=1;	/* we don't want resource id 0 */
		return SUCCESS;
	} else {
		return FAILURE;
	}
}


int zend_init_rsrc_plist(TSRMLS_D)
{
	return zend_hash_init_ex(&EG(persistent_list), 0, NULL, plist_entry_destructor, 1, 0);
}


void zend_destroy_rsrc_list(HashTable *ht TSRMLS_DC)
{
	zend_hash_graceful_reverse_destroy(ht);
}

static int clean_module_resource(zend_rsrc_list_entry *le, int *resource_id TSRMLS_DC)
{
	if (le->type == *resource_id) {
		return 1;
	} else {
		return 0;
	}
}


static int zend_clean_module_rsrc_dtors_cb(zend_rsrc_list_dtors_entry *ld, int *module_number TSRMLS_DC)
{
	if (ld->module_number == *module_number) {
		zend_hash_apply_with_argument(&EG(persistent_list), (apply_func_arg_t) clean_module_resource, (void *) &(ld->resource_id) TSRMLS_CC);
		return 1;
	} else {
		return 0;
	}
}


void zend_clean_module_rsrc_dtors(int module_number TSRMLS_DC)
{
	zend_hash_apply_with_argument(&list_destructors, (apply_func_arg_t) zend_clean_module_rsrc_dtors_cb, (void *) &module_number TSRMLS_CC);
}


ZEND_API int zend_register_list_destructors(void (*ld)(void *), void (*pld)(void *), int module_number)
{
	zend_rsrc_list_dtors_entry lde;
	
#if 0
	printf("Registering destructors %d for module %d\n", list_destructors.nNextFreeElement, module_number);
#endif
	
	lde.list_dtor=(void (*)(void *)) ld;
	lde.plist_dtor=(void (*)(void *)) pld;
	lde.list_dtor_ex = lde.plist_dtor_ex = NULL;
	lde.module_number = module_number;
	lde.resource_id = list_destructors.nNextFreeElement;
	lde.type = ZEND_RESOURCE_LIST_TYPE_STD;
	lde.type_name = NULL;
	
	if (zend_hash_next_index_insert(&list_destructors, (void *) &lde, sizeof(zend_rsrc_list_dtors_entry), NULL)==FAILURE) {
		return FAILURE;
	}
	return list_destructors.nNextFreeElement-1;
}


ZEND_API int zend_register_list_destructors_ex(rsrc_dtor_func_t ld, rsrc_dtor_func_t pld, const char *type_name, int module_number)
{
	zend_rsrc_list_dtors_entry lde;
	
#if 0
	printf("Registering destructors %d for module %d\n", list_destructors.nNextFreeElement, module_number);
#endif

	lde.list_dtor = NULL;
	lde.plist_dtor = NULL;
	lde.list_dtor_ex = ld;
	lde.plist_dtor_ex = pld;
	lde.module_number = module_number;
	lde.resource_id = list_destructors.nNextFreeElement;
	lde.type = ZEND_RESOURCE_LIST_TYPE_EX;
	lde.type_name = type_name;
	
	if (zend_hash_next_index_insert(&list_destructors, (void *) &lde, sizeof(zend_rsrc_list_dtors_entry), NULL)==FAILURE) {
		return FAILURE;
	}
	return list_destructors.nNextFreeElement-1;
}

ZEND_API int zend_fetch_list_dtor_id(char *type_name)
{
	zend_rsrc_list_dtors_entry *lde;
	HashPosition pos;

	zend_hash_internal_pointer_reset_ex(&list_destructors, &pos);
	while (zend_hash_get_current_data_ex(&list_destructors, (void **)&lde, &pos) == SUCCESS) {
		if (lde->type_name && (strcmp(type_name, lde->type_name) == 0)) {
#if 0
			printf("Found resource id %d for resource type %s\n", (*lde).resource_id, type_name);
#endif
			return lde->resource_id;
		}
		zend_hash_move_forward_ex(&list_destructors, &pos);
	}

	return 0;
}

int zend_init_rsrc_list_dtors(void)
{
	int retval;

	retval = zend_hash_init(&list_destructors, 50, NULL, NULL, 1);
	list_destructors.nNextFreeElement=1;	/* we don't want resource type 0 */

	return retval;
}


void zend_destroy_rsrc_list_dtors(void)
{
	zend_hash_destroy(&list_destructors);
}


const char *zend_rsrc_list_get_rsrc_type(int resource TSRMLS_DC)
{
	zend_rsrc_list_dtors_entry *lde;
	int rsrc_type;

	if (!zend_list_find(resource, &rsrc_type))
		return NULL;

	if (zend_hash_index_find(&list_destructors, rsrc_type, (void **) &lde)==SUCCESS) {
		return lde->type_name;
	} else {
		return NULL;
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
