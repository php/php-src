#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"
#include "zend_API.h"

#define ZEND_DEBUG_OBJECTS 0

static zend_object_handlers zoh = {
	zend_objects_get_address,
	NULL,
	zend_objects_add_ref,
	zend_objects_del_ref,
	zend_objects_delete_obj,
	zend_objects_clone_obj
};

void zend_objects_init(zend_objects *objects, zend_uint init_size)
{
	objects->object_buckets = (zend_object_bucket *) emalloc(init_size * sizeof(zend_object_bucket));
	objects->top = 1; /* Skip 0 so that handles are true */
	objects->size = init_size;
	objects->free_list_head = -1;
}

void zend_objects_destroy(zend_objects *objects)
{
	efree(objects->object_buckets);
}

static inline void zend_objects_destroy_object(zend_object *object, zend_object_handle handle TSRMLS_DC)
{
	if (object->ce->destructor) {
		zval *obj;
		zval *destructor_func_name;
		zval *retval_ptr;
		HashTable symbol_table;
		
		MAKE_STD_ZVAL(obj);
		obj->type = IS_OBJECT;
		obj->value.obj.handle = handle;
		obj->value.obj.handlers = &zoh;
		zval_copy_ctor(obj);


		/* FIXME: Optimize this so that we use the old_object->ce->destructor function pointer instead of the name */
		MAKE_STD_ZVAL(destructor_func_name);
		destructor_func_name->type = IS_STRING;
		destructor_func_name->value.str.val = estrndup("_destruct", sizeof("_destruct")-1);
		destructor_func_name->value.str.len = sizeof("_destruct")-1;

		ZEND_INIT_SYMTABLE(&symbol_table);
		
		call_user_function_ex(NULL, &obj, destructor_func_name, &retval_ptr, 0, NULL, 0, &symbol_table TSRMLS_CC);

		zend_hash_destroy(&symbol_table);
		zval_ptr_dtor(&obj);
		zval_ptr_dtor(&destructor_func_name);
		zval_ptr_dtor(&retval_ptr);
	}

	/* Nuke the object */
	zend_hash_destroy(object->properties);
	efree(object->properties);
	
}

zend_object_value zend_objects_new(zend_object **object, zend_class_entry *class_type)
{
	zend_object_handle handle;
	zend_object_value retval;

	TSRMLS_FETCH();

	if (EG(objects).free_list_head != -1) {
		handle = EG(objects).free_list_head;
		EG(objects).free_list_head = EG(objects).object_buckets[handle].bucket.free_list.next;
	} else {
		if (EG(objects).top == EG(objects).size) {
			EG(objects).size <<= 1;
			EG(objects).object_buckets = (zend_object_bucket *) erealloc(EG(objects).object_buckets, EG(objects).size * sizeof(zend_object_bucket));
		}
		handle = EG(objects).top++;
	}
	EG(objects).object_buckets[handle].valid = 1;
	EG(objects).object_buckets[handle].constructor_called = 0;
	EG(objects).object_buckets[handle].bucket.obj.refcount = 1;
	
	*object = &EG(objects).object_buckets[handle].bucket.obj.object;

	(*object)->ce = class_type;

	retval.handle = handle;
	retval.handlers = &zoh;
#if ZEND_DEBUG_OBJECTS
	fprintf(stderr, "Allocated object id #%d\n", handle);
#endif
	return retval;
}

zend_object *zend_objects_get_address(zend_object_handle handle)
{
	TSRMLS_FETCH();

	if (!EG(objects).object_buckets[handle].valid) {
		zend_error(E_ERROR, "Trying to access invalid object");
	}
	return &EG(objects).object_buckets[handle].bucket.obj.object;
}

void zend_objects_add_ref(zend_object_handle handle)
{
	TSRMLS_FETCH();

	if (!EG(objects).object_buckets[handle].valid) {
		zend_error(E_ERROR, "Trying to add reference to invalid object");
	}

	EG(objects).object_buckets[handle].bucket.obj.refcount++;
#if ZEND_DEBUG_OBJECTS
	fprintf(stderr, "Increased refcount of object id #%d\n", handle);
#endif
}

void zend_objects_delete_obj(zend_object_handle handle)
{
	zend_object *object;
	TSRMLS_FETCH();

	if (!EG(objects).object_buckets[handle].valid) {
		zend_error(E_ERROR, "Trying to delete invalid object");
	}

	object = &EG(objects).object_buckets[handle].bucket.obj.object;

	if (!EG(objects).object_buckets[handle].constructor_called) {
		EG(objects).object_buckets[handle].constructor_called = 1;
		zend_objects_destroy_object(object, handle TSRMLS_CC);
	}
	
	EG(objects).object_buckets[handle].valid = 0;
	
#if ZEND_DEBUG_OBJECTS
	fprintf(stderr, "Deleted object id #%d\n", handle);
#endif

}

#define ZEND_OBJECTS_ADD_TO_FREE_LIST()																\
			EG(objects).object_buckets[handle].bucket.free_list.next = EG(objects).free_list_head;	\
			EG(objects).free_list_head = handle;													\
			EG(objects).object_buckets[handle].valid = 0;

void zend_objects_del_ref(zend_object_handle handle)
{
	TSRMLS_FETCH();

	if (--EG(objects).object_buckets[handle].bucket.obj.refcount == 0) {
		zend_object *object;
		do {
			if (EG(objects).object_buckets[handle].valid) {
				if (!EG(objects).object_buckets[handle].constructor_called) {
					object = &EG(objects).object_buckets[handle].bucket.obj.object;
					EG(objects).object_buckets[handle].constructor_called = 1;
					zend_objects_destroy_object(object, handle TSRMLS_CC);
					if (EG(objects).object_buckets[handle].bucket.obj.refcount == 0) {
						ZEND_OBJECTS_ADD_TO_FREE_LIST();
					}
					break;
				}			
			}
			ZEND_OBJECTS_ADD_TO_FREE_LIST();
		} while (0);
		
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

zend_object_value zend_objects_clone_obj(zend_object_handle handle)
{
	zend_object_value retval;
	zend_object *old_object;
	zend_object *new_object;

	TSRMLS_FETCH();

	if (!EG(objects).object_buckets[handle].valid) {
		zend_error(E_ERROR, "Trying to clone invalid object");
	}

	old_object = &EG(objects).object_buckets[handle].bucket.obj.object;
	retval = zend_objects_new(&new_object, old_object->ce);

	if (old_object->ce->clone) {
		zval *old_obj;
		zval *new_obj;
		zval *clone_func_name;
		zval *retval_ptr;
		HashTable symbol_table;

		MAKE_STD_ZVAL(new_obj);
		new_obj->type = IS_OBJECT;
		new_obj->value.obj = retval;
		zval_copy_ctor(new_obj);

		MAKE_STD_ZVAL(old_obj);
		old_obj->type = IS_OBJECT;
		old_obj->value.obj.handle = handle;
		old_obj->value.obj.handlers = &zoh; /* If we reached here than the handlers are zoh */
		zval_copy_ctor(old_obj);

		/* FIXME: Optimize this so that we use the old_object->ce->clone function pointer instead of the name */
		MAKE_STD_ZVAL(clone_func_name);
		clone_func_name->type = IS_STRING;
		clone_func_name->value.str.val = estrndup("_clone", sizeof("_clone")-1);
		clone_func_name->value.str.len = sizeof("_clone")-1;

		ALLOC_HASHTABLE(new_object->properties);
		zend_hash_init(new_object->properties, 0, NULL, ZVAL_PTR_DTOR, 0);

		ZEND_INIT_SYMTABLE(&symbol_table);
		ZEND_SET_SYMBOL(&symbol_table, "clone", old_obj);
		
		call_user_function_ex(NULL, &new_obj, clone_func_name, &retval_ptr, 0, NULL, 0, &symbol_table TSRMLS_CC);

		zend_hash_destroy(&symbol_table);
		zval_ptr_dtor(&new_obj);
		zval_ptr_dtor(&clone_func_name);
		zval_ptr_dtor(&retval_ptr);
	} else {
		ALLOC_HASHTABLE(new_object->properties);
		zend_hash_init(new_object->properties, 0, NULL, ZVAL_PTR_DTOR, 0);
		zend_hash_copy(new_object->properties, old_object->properties, (copy_ctor_func_t) zval_add_ref, (void *) NULL /* Not used anymore */, sizeof(zval *));
	}

	return retval;
}
