#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"
#include "zend_API.h"
#include "zend_objects_API.h"

#define ZEND_DEBUG_OBJECTS 0

void zend_objects_store_init(zend_objects_store *objects, zend_uint init_size)
{
	objects->object_buckets = (zend_object_store_bucket *) emalloc(init_size * sizeof(zend_object_store_bucket));
	objects->top = 1; /* Skip 0 so that handles are true */
	objects->size = init_size;
	objects->free_list_head = -1;
}

void zend_objects_store_destroy(zend_objects_store *objects)
{
	efree(objects->object_buckets);
}

void zend_objects_store_call_destructors(zend_objects_store *objects TSRMLS_DC)
{
	zend_uint i = 1;

	for (i = 1; i < objects->top ; i++) {
		if (objects->object_buckets[i].valid) {
			struct _store_object *obj = &objects->object_buckets[i].bucket.obj;
			if(obj->dtor) {
				objects->object_buckets[i].destructor_called = 1;
				obj->dtor(obj->object, i TSRMLS_CC);
			}
			objects->object_buckets[i].valid = 0;
		}
	}
}

/* Store objects API */

zend_object_handle zend_objects_store_put(void *object, zend_objects_store_dtor_t dtor, zend_objects_store_clone_t clone TSRMLS_DC)
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
	EG(objects_store).object_buckets[handle].valid = 1;
	EG(objects_store).object_buckets[handle].destructor_called = 0;

	obj->refcount = 1;
	obj->object = object;
	obj->dtor = dtor;
	obj->clone = clone;

#if ZEND_DEBUG_OBJECTS
	fprintf(stderr, "Allocated object id #%d\n", handle);
#endif
	return handle;
}

void zend_objects_store_add_ref(zval *object TSRMLS_DC)
{
	zend_object_handle handle = Z_OBJ_HANDLE_P(object);

	if (!EG(objects_store).object_buckets[handle].valid) {
		zend_error(E_ERROR, "Trying to add reference to invalid object");
	}

	EG(objects_store).object_buckets[handle].bucket.obj.refcount++;
#if ZEND_DEBUG_OBJECTS
	fprintf(stderr, "Increased refcount of object id #%d\n", handle);
#endif
}

void zend_objects_store_delete_obj(zval *zobject TSRMLS_DC)
{
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);
	struct _store_object *obj = &EG(objects_store).object_buckets[handle].bucket.obj;
	
	if (!EG(objects_store).object_buckets[handle].valid) {
		zend_error(E_ERROR, "Trying to delete invalid object");
	}


	if(obj->dtor && !EG(objects_store).object_buckets[handle].destructor_called) {
		EG(objects_store).object_buckets[handle].destructor_called = 1;
		obj->dtor(obj->object, handle TSRMLS_CC);
	}
	EG(objects_store).object_buckets[handle].valid = 0;
	
#if ZEND_DEBUG_OBJECTS
	fprintf(stderr, "Deleted object id #%d\n", handle);
#endif

}

#define ZEND_OBJECTS_STORE_ADD_TO_FREE_LIST()																\
			EG(objects_store).object_buckets[handle].bucket.free_list.next = EG(objects_store).free_list_head;	\
			EG(objects_store).free_list_head = handle;													\
			EG(objects_store).object_buckets[handle].valid = 0;

void zend_objects_store_del_ref(zval *zobject TSRMLS_DC)
{
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);
	struct _store_object *obj = &EG(objects_store).object_buckets[handle].bucket.obj;
	
	if (--obj->refcount == 0) {
		if (EG(objects_store).object_buckets[handle].valid) {
			if(!EG(objects_store).object_buckets[handle].destructor_called) {
				EG(objects_store).object_buckets[handle].destructor_called = 1;
				if (obj->dtor) {
					obj->dtor(obj->object, handle TSRMLS_CC);
				}
				EG(objects_store).object_buckets[handle].valid = 0;
				if (obj->refcount == 0) {
					ZEND_OBJECTS_STORE_ADD_TO_FREE_LIST();
				}
			}
		}
#if ZEND_DEBUG_OBJECTS
		fprintf(stderr, "Deallocated object id #%d\n", handle);
#endif
	}
#if ZEND_DEBUG_OBJECTS
	else {
		fprintf(stderr, "Decreased refcount of object id #%d\n", handle);
	}
#endif
}

zend_object_value zend_objects_store_clone_obj(zval *zobject TSRMLS_DC)
{
	zend_object_value retval;
	void *new_object;
	struct _store_object *obj;
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);

	if (!EG(objects_store).object_buckets[handle].valid) {
		zend_error(E_ERROR, "Trying to clone invalid object");
	}
	obj = &EG(objects_store).object_buckets[handle].bucket.obj;
	
	if(obj->clone == NULL) {
		zend_error(E_ERROR, "Trying to clone uncloneable object");
	}		

	obj->clone(&obj->object, &new_object TSRMLS_CC);

	retval.handle = zend_objects_store_put(new_object, obj->dtor, obj->clone TSRMLS_CC);
	retval.handlers = Z_OBJ_HT_P(zobject);
	
	return retval;
}

void *zend_object_store_get_object(zval *zobject TSRMLS_DC)
{
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);

	if (!EG(objects_store).object_buckets[handle].valid) {
		zend_error(E_ERROR, "Trying to access invalid object");
		return NULL;
	}

	return EG(objects_store).object_buckets[handle].bucket.obj.object;
}


/* Proxy objects workings */
typedef struct _zend_proxy_object {
	zval *object;
	zval *property;
} zend_proxy_object;

static zend_object_handlers zend_object_proxy_handlers;

void zend_objects_proxy_dtor(zend_proxy_object *object, zend_object_handle handle TSRMLS_DC)
{
	zval_ptr_dtor(&object->object);
	zval_ptr_dtor(&object->property);
	efree(object);
}

void zend_objects_proxy_clone(zend_proxy_object *object, zend_proxy_object **object_clone TSRMLS_DC)
{
	*object_clone = emalloc(sizeof(zend_proxy_object));
	(*object_clone)->object = object->object;
	(*object_clone)->property = object->property;
	zval_add_ref(&(*object_clone)->property);
	zval_add_ref(&(*object_clone)->object);
}

zval **zend_object_create_proxy(zval *object, zval *member TSRMLS_DC)
{
	zend_proxy_object *pobj = emalloc(sizeof(zend_proxy_object));
	zval *retval, **pretval;

	pobj->object = object;
	pobj->property = member;
	zval_add_ref(&pobj->property);
	zval_add_ref(&pobj->object);

	MAKE_STD_ZVAL(retval);
	retval->type = IS_OBJECT;
	Z_OBJ_HANDLE_P(retval) = zend_objects_store_put(pobj, (zend_objects_store_dtor_t)zend_objects_proxy_dtor, (zend_objects_store_clone_t)zend_objects_proxy_clone TSRMLS_CC);
	Z_OBJ_HT_P(retval) = &zend_object_proxy_handlers;
	pretval = emalloc(sizeof(zval *));
	*pretval = retval;
	
	return pretval;
}

void zend_object_proxy_set(zval **property, zval *value TSRMLS_DC)
{
	zend_proxy_object *probj = zend_object_store_get_object(*property TSRMLS_CC);

	if(Z_OBJ_HT_P(probj->object) && Z_OBJ_HT_P(probj->object)->write_property) {
		Z_OBJ_HT_P(probj->object)->write_property(probj->object, probj->property, value TSRMLS_CC);
	} else {
		zend_error(E_WARNING, "Cannot write property of object - no write handler defined");
	}
}

zval* zend_object_proxy_get(zval *property TSRMLS_DC)
{
	zend_proxy_object *probj = zend_object_store_get_object(property TSRMLS_CC);

	if(Z_OBJ_HT_P(probj->object) && Z_OBJ_HT_P(probj->object)->read_property) {
		return Z_OBJ_HT_P(probj->object)->read_property(probj->object, probj->property, BP_VAR_R TSRMLS_CC);
	} else {
		zend_error(E_WARNING, "Cannot read property of object - no read handler defined");
	}

	return NULL;
}

static zend_object_handlers zend_object_proxy_handlers = {
	ZEND_OBJECTS_STORE_HANDLERS,
	
	NULL,                  /* read_property */
	NULL,                 /* write_property */
	NULL,               /* get_property_ptr */
	NULL,               /* get_property_zval_ptr */
	zend_object_proxy_get,                   /* get */
	zend_object_proxy_set,                   /* set */
	NULL,                   /* has_property */
	NULL,                   /* unset_property */
	NULL,                   /* get_properties */
	NULL,                   /* get_method */
	NULL,                   /* call_method */
	NULL,                /* get_constructor */
	NULL,                /* get_class_entry */
	NULL,                /* get_class_name */
	NULL                 /* compare_objects */
};
