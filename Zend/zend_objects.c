#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"
#include "zend_API.h"

static inline void zend_objects_call_destructor(zend_object *object, zend_object_handle handle TSRMLS_DC)
{
	if (object->ce->destructor) {
		zval *obj;
		zval *destructor_func_name;
		zval *retval_ptr;
		HashTable symbol_table;
		
		MAKE_STD_ZVAL(obj);
		obj->type = IS_OBJECT;
		obj->value.obj.handle = handle;
		obj->value.obj.handlers = &std_object_handlers;
		zval_copy_ctor(obj);


		/* FIXME: Optimize this so that we use the old_object->ce->destructor function pointer instead of the name */
		MAKE_STD_ZVAL(destructor_func_name);
		destructor_func_name->type = IS_STRING;
		destructor_func_name->value.str.val = estrndup("__destruct", sizeof("__destruct")-1);
		destructor_func_name->value.str.len = sizeof("__destruct")-1;

		ZEND_INIT_SYMTABLE(&symbol_table);
		
		call_user_function_ex(NULL, &obj, destructor_func_name, &retval_ptr, 0, NULL, 0, &symbol_table TSRMLS_CC);

		zend_hash_destroy(&symbol_table);
		zval_ptr_dtor(&obj);
		zval_ptr_dtor(&destructor_func_name);
		if (retval_ptr) {
			zval_ptr_dtor(&retval_ptr);
		}
	}
}


static inline void zend_objects_destroy_object(zend_object *object, zend_object_handle handle TSRMLS_DC)
{
	zend_objects_call_destructor(object, handle TSRMLS_CC);
	/* Nuke the object */
	zend_hash_destroy(object->properties);
	FREE_HASHTABLE(object->properties);
	efree(object);
}

ZEND_API zend_object_value zend_objects_new(zend_object **object, zend_class_entry *class_type)
{	
	zend_object_value retval;

	TSRMLS_FETCH();

	*object = emalloc(sizeof(zend_object));
	(*object)->ce = class_type;
	retval.handle = zend_objects_store_put(*object, (zend_objects_store_dtor_t) zend_objects_destroy_object, NULL TSRMLS_CC);
	retval.handlers = &std_object_handlers;
	(*object)->in_get = 0;
	(*object)->in_set = 0;
	return retval;
}

ZEND_API zend_object *zend_objects_get_address(zval *zobject)
{
	TSRMLS_FETCH();
	return (zend_object *)zend_object_store_get_object(zobject TSRMLS_CC);
}

ZEND_API zend_object_value zend_objects_clone_obj(zval *zobject TSRMLS_DC)
{
	zend_object_value retval;
	zend_object *old_object;
	zend_object *new_object;
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);

	old_object = zend_objects_get_address(zobject);
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
		old_obj->value.obj.handlers = &std_object_handlers; /* If we reached here than the handlers are standrd */
		zval_copy_ctor(old_obj);

		/* FIXME: Optimize this so that we use the old_object->ce->clone function pointer instead of the name */
		MAKE_STD_ZVAL(clone_func_name);
		clone_func_name->type = IS_STRING;
		clone_func_name->value.str.val = estrndup("__clone", sizeof("__clone")-1);
		clone_func_name->value.str.len = sizeof("__clone")-1;

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
