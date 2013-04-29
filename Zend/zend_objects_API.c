/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
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

#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"
#include "zend_API.h"
#include "zend_objects_API.h"

#define ZEND_DEBUG_OBJECTS 0

ZEND_API void zend_objects_store_init(zend_objects_store *objects, zend_uint init_size)
{
	objects->object_buckets = (zend_object_store_bucket *) emalloc(init_size * sizeof(zend_object_store_bucket));
	objects->top = 1; /* Skip 0 so that handles are true */
	objects->size = init_size;
	objects->free_list_head = -1;
	memset(&objects->object_buckets[0], 0, sizeof(zend_object_store_bucket));
}

ZEND_API void zend_objects_store_destroy(zend_objects_store *objects)
{
	efree(objects->object_buckets);
	objects->object_buckets = NULL;
}

ZEND_API void zend_objects_store_call_destructors(zend_objects_store *objects TSRMLS_DC)
{
	zend_uint i = 1;

	for (i = 1; i < objects->top ; i++) {
		if (objects->object_buckets[i].valid) {
			struct _store_object *obj = &objects->object_buckets[i].bucket.obj;

			if (!objects->object_buckets[i].destructor_called) {
				objects->object_buckets[i].destructor_called = 1;
				if (obj->dtor && obj->object) {
					obj->refcount++;
					obj->dtor(obj->object, i TSRMLS_CC);
					obj = &objects->object_buckets[i].bucket.obj;
					obj->refcount--;
				}
			}
		}
	}
}

ZEND_API void zend_objects_store_mark_destructed(zend_objects_store *objects TSRMLS_DC)
{
	zend_uint i;

	if (!objects->object_buckets) {
		return;
	}
	for (i = 1; i < objects->top ; i++) {
		if (objects->object_buckets[i].valid) {
			objects->object_buckets[i].destructor_called = 1;
		}
	}
}

ZEND_API void zend_objects_store_free_object_storage(zend_objects_store *objects TSRMLS_DC)
{
	zend_uint i = 1;

	for (i = 1; i < objects->top ; i++) {
		if (objects->object_buckets[i].valid) {
			struct _store_object *obj = &objects->object_buckets[i].bucket.obj;

			GC_REMOVE_ZOBJ_FROM_BUFFER(obj);

			objects->object_buckets[i].valid = 0;
			if (obj->free_storage) {
				obj->free_storage(obj->object TSRMLS_CC);
			}
			/* Not adding to free list as we are shutting down anyway */
		}
	}
}


/* Store objects API */

ZEND_API zend_object_handle zend_objects_store_put(void *object, zend_objects_store_dtor_t dtor, zend_objects_free_object_storage_t free_storage, zend_objects_store_clone_t clone TSRMLS_DC)
{
	zend_object_handle handle;
	struct _store_object *obj;

	if (EG(objects_store).free_list_head != -1) {
		handle = EG(objects_store).free_list_head;
		EG(objects_store).free_list_head = EG(objects_store).object_buckets[handle].bucket.free_list.next;
	} else {
		if (EG(objects_store).top == EG(objects_store).size) {
			EG(objects_store).size <<= 1;
			EG(objects_store).object_buckets = (zend_object_store_bucket *) erealloc(EG(objects_store).object_buckets, EG(objects_store).size * sizeof(zend_object_store_bucket));
		}
		handle = EG(objects_store).top++;
	}
	obj = &EG(objects_store).object_buckets[handle].bucket.obj;
	EG(objects_store).object_buckets[handle].destructor_called = 0;
	EG(objects_store).object_buckets[handle].valid = 1;
	EG(objects_store).object_buckets[handle].apply_count = 0;

	obj->refcount = 1;
	GC_OBJ_INIT(obj);
	obj->object = object;
	obj->dtor = dtor?dtor:(zend_objects_store_dtor_t)zend_objects_destroy_object;
	obj->free_storage = free_storage;
	obj->clone = clone;
	obj->handlers = NULL;

#if ZEND_DEBUG_OBJECTS
	fprintf(stderr, "Allocated object id #%d\n", handle);
#endif
	return handle;
}

ZEND_API zend_uint zend_objects_store_get_refcount(zval *object TSRMLS_DC)
{
	zend_object_handle handle = Z_OBJ_HANDLE_P(object);

	return EG(objects_store).object_buckets[handle].bucket.obj.refcount;
}

ZEND_API void zend_objects_store_add_ref(zval *object TSRMLS_DC)
{
	zend_object_handle handle = Z_OBJ_HANDLE_P(object);

	EG(objects_store).object_buckets[handle].bucket.obj.refcount++;
#if ZEND_DEBUG_OBJECTS
	fprintf(stderr, "Increased refcount of object id #%d\n", handle);
#endif
}

/*
 * Add a reference to an objects store entry given the object handle.
 */
ZEND_API void zend_objects_store_add_ref_by_handle(zend_object_handle handle TSRMLS_DC)
{
	EG(objects_store).object_buckets[handle].bucket.obj.refcount++;
}

#define ZEND_OBJECTS_STORE_ADD_TO_FREE_LIST()																	\
			EG(objects_store).object_buckets[handle].bucket.free_list.next = EG(objects_store).free_list_head;	\
			EG(objects_store).free_list_head = handle;															\
			EG(objects_store).object_buckets[handle].valid = 0;

ZEND_API void zend_objects_store_del_ref(zval *zobject TSRMLS_DC)
{
	zend_object_handle handle;

	handle = Z_OBJ_HANDLE_P(zobject);

	Z_ADDREF_P(zobject);
	zend_objects_store_del_ref_by_handle_ex(handle, Z_OBJ_HT_P(zobject) TSRMLS_CC);
	Z_DELREF_P(zobject);

	GC_ZOBJ_CHECK_POSSIBLE_ROOT(zobject);
}

/*
 * Delete a reference to an objects store entry given the object handle.
 */
ZEND_API void zend_objects_store_del_ref_by_handle_ex(zend_object_handle handle, const zend_object_handlers *handlers TSRMLS_DC) /* {{{ */
{
	struct _store_object *obj;
	int failure = 0;

	if (!EG(objects_store).object_buckets) {
		return;
	}

	obj = &EG(objects_store).object_buckets[handle].bucket.obj;

	/*	Make sure we hold a reference count during the destructor call
		otherwise, when the destructor ends the storage might be freed
		when the refcount reaches 0 a second time
	 */
	if (EG(objects_store).object_buckets[handle].valid) {
		if (obj->refcount == 1) {
			if (!EG(objects_store).object_buckets[handle].destructor_called) {
				EG(objects_store).object_buckets[handle].destructor_called = 1;

				if (obj->dtor) {
					if (handlers && !obj->handlers) {
						obj->handlers = handlers;
					}
					zend_try {
						obj->dtor(obj->object, handle TSRMLS_CC);
					} zend_catch {
						failure = 1;
					} zend_end_try();
				}
			}
			
			/* re-read the object from the object store as the store might have been reallocated in the dtor */
			obj = &EG(objects_store).object_buckets[handle].bucket.obj;

			if (obj->refcount == 1) {
				GC_REMOVE_ZOBJ_FROM_BUFFER(obj);
				if (obj->free_storage) {
					zend_try {
						obj->free_storage(obj->object TSRMLS_CC);
					} zend_catch {
						failure = 1;
					} zend_end_try();
				}
				ZEND_OBJECTS_STORE_ADD_TO_FREE_LIST();
			}
		}
	}

	obj->refcount--;

#if ZEND_DEBUG_OBJECTS
	if (obj->refcount == 0) {
		fprintf(stderr, "Deallocated object id #%d\n", handle);
	} else {
		fprintf(stderr, "Decreased refcount of object id #%d\n", handle);
	}
#endif
	if (failure) {
		zend_bailout();
	}
}
/* }}} */

ZEND_API zend_object_value zend_objects_store_clone_obj(zval *zobject TSRMLS_DC)
{
	zend_object_value retval;
	void *new_object;
	struct _store_object *obj;
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);

	obj = &EG(objects_store).object_buckets[handle].bucket.obj;

	if (obj->clone == NULL) {
		zend_error(E_CORE_ERROR, "Trying to clone uncloneable object of class %s", Z_OBJCE_P(zobject)->name);
	}

	obj->clone(obj->object, &new_object TSRMLS_CC);
	obj = &EG(objects_store).object_buckets[handle].bucket.obj;

	retval.handle = zend_objects_store_put(new_object, obj->dtor, obj->free_storage, obj->clone TSRMLS_CC);
	retval.handlers = Z_OBJ_HT_P(zobject);
	EG(objects_store).object_buckets[handle].bucket.obj.handlers = retval.handlers;

	return retval;
}

ZEND_API void *zend_object_store_get_object(const zval *zobject TSRMLS_DC)
{
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);

	return EG(objects_store).object_buckets[handle].bucket.obj.object;
}

/*
 * Retrieve an entry from the objects store given the object handle.
 */
ZEND_API void *zend_object_store_get_object_by_handle(zend_object_handle handle TSRMLS_DC)
{
	return EG(objects_store).object_buckets[handle].bucket.obj.object;
}

/* zend_object_store_set_object:
 * It is ONLY valid to call this function from within the constructor of an
 * overloaded object.  Its purpose is to set the object pointer for the object
 * when you can't possibly know its value until you have parsed the arguments
 * from the constructor function.  You MUST NOT use this function for any other
 * weird games, or call it at any other time after the object is constructed.
 * */
ZEND_API void zend_object_store_set_object(zval *zobject, void *object TSRMLS_DC)
{
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);

	EG(objects_store).object_buckets[handle].bucket.obj.object = object;
}


/* Called when the ctor was terminated by an exception */
ZEND_API void zend_object_store_ctor_failed(zval *zobject TSRMLS_DC)
{
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);
	zend_object_store_bucket *obj_bucket = &EG(objects_store).object_buckets[handle];
	
	obj_bucket->bucket.obj.handlers = Z_OBJ_HT_P(zobject);;
	obj_bucket->destructor_called = 1;
}


/* Proxy objects workings */
typedef struct _zend_proxy_object {
	zval *object;
	zval *property;
} zend_proxy_object;

static zend_object_handlers zend_object_proxy_handlers;

ZEND_API void zend_objects_proxy_destroy(zend_object *object, zend_object_handle handle TSRMLS_DC)
{
}

ZEND_API void zend_objects_proxy_free_storage(zend_proxy_object *object TSRMLS_DC)
{
	zval_ptr_dtor(&object->object);
	zval_ptr_dtor(&object->property);
	efree(object);
}

ZEND_API void zend_objects_proxy_clone(zend_proxy_object *object, zend_proxy_object **object_clone TSRMLS_DC)
{
	*object_clone = emalloc(sizeof(zend_proxy_object));
	(*object_clone)->object = object->object;
	(*object_clone)->property = object->property;
	zval_add_ref(&(*object_clone)->property);
	zval_add_ref(&(*object_clone)->object);
}

ZEND_API zval *zend_object_create_proxy(zval *object, zval *member TSRMLS_DC)
{
	zend_proxy_object *pobj = emalloc(sizeof(zend_proxy_object));
	zval *retval;

	pobj->object = object;
	zval_add_ref(&pobj->object);
	ALLOC_ZVAL(pobj->property);
	INIT_PZVAL_COPY(pobj->property, member);
	zval_copy_ctor(pobj->property);

	MAKE_STD_ZVAL(retval);
	Z_TYPE_P(retval) = IS_OBJECT;
	Z_OBJ_HANDLE_P(retval) = zend_objects_store_put(pobj, (zend_objects_store_dtor_t)zend_objects_proxy_destroy, (zend_objects_free_object_storage_t) zend_objects_proxy_free_storage, (zend_objects_store_clone_t) zend_objects_proxy_clone TSRMLS_CC);
	Z_OBJ_HT_P(retval) = &zend_object_proxy_handlers;

	return retval;
}

ZEND_API void zend_object_proxy_set(zval **property, zval *value TSRMLS_DC)
{
	zend_proxy_object *probj = zend_object_store_get_object(*property TSRMLS_CC);

	if (Z_OBJ_HT_P(probj->object) && Z_OBJ_HT_P(probj->object)->write_property) {
		Z_OBJ_HT_P(probj->object)->write_property(probj->object, probj->property, value, 0 TSRMLS_CC);
	} else {
		zend_error(E_WARNING, "Cannot write property of object - no write handler defined");
	}
}

ZEND_API zval* zend_object_proxy_get(zval *property TSRMLS_DC)
{
	zend_proxy_object *probj = zend_object_store_get_object(property TSRMLS_CC);

	if (Z_OBJ_HT_P(probj->object) && Z_OBJ_HT_P(probj->object)->read_property) {
		return Z_OBJ_HT_P(probj->object)->read_property(probj->object, probj->property, BP_VAR_R, 0 TSRMLS_CC);
	} else {
		zend_error(E_WARNING, "Cannot read property of object - no read handler defined");
	}

	return NULL;
}

ZEND_API zend_object_handlers *zend_get_std_object_handlers(void)
{
	return &std_object_handlers;
}

static zend_object_handlers zend_object_proxy_handlers = {
	ZEND_OBJECTS_STORE_HANDLERS,

	NULL,						/* read_property */
	NULL,						/* write_property */
	NULL,						/* read dimension */
	NULL,						/* write_dimension */
	NULL,						/* get_property_ptr_ptr */
	zend_object_proxy_get,		/* get */
	zend_object_proxy_set,		/* set */
	NULL,						/* has_property */
	NULL,						/* unset_property */
	NULL,						/* has_dimension */
	NULL,						/* unset_dimension */
	NULL,						/* get_properties */
	NULL,						/* get_method */
	NULL,						/* call_method */
	NULL,						/* get_constructor */
	NULL,						/* get_class_entry */
	NULL,						/* get_class_name */
	NULL,						/* compare_objects */
	NULL,						/* cast_object */
	NULL,						/* count_elements */
};


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
