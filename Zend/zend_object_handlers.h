/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 Zend Technologies Ltd. (http://www.zend.com) |
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

#ifndef ZEND_OBJECT_HANDLERS_H
#define ZEND_OBJECT_HANDLERS_H

struct _zend_property_info;

#define ZEND_WRONG_PROPERTY_INFO \
	((struct _zend_property_info*)((zend_intptr_t)-1))

#define ZEND_DYNAMIC_PROPERTY_OFFSET ((uint32_t)(-1))
#define ZEND_WRONG_PROPERTY_OFFSET   ((uint32_t)(-2))

/* The following rule applies to read_property() and read_dimension() implementations:
   If you return a zval which is not otherwise referenced by the extension or the engine's
   symbol table, its reference count should be 0.
*/
/* Used to fetch property from the object, read-only */
typedef zval *(*zend_object_read_property_t)(zval *object, zval *member, int type, void **cache_slot, zval *rv);

/* Used to fetch dimension from the object, read-only */
typedef zval *(*zend_object_read_dimension_t)(zval *object, zval *offset, int type, zval *rv);


/* The following rule applies to write_property() and write_dimension() implementations:
   If you receive a value zval in write_property/write_dimension, you may only modify it if
   its reference count is 1.  Otherwise, you must create a copy of that zval before making
   any changes.  You should NOT modify the reference count of the value passed to you.
*/
/* Used to set property of the object */
typedef void (*zend_object_write_property_t)(zval *object, zval *member, zval *value, void **cache_slot);

/* Used to set dimension of the object */
typedef void (*zend_object_write_dimension_t)(zval *object, zval *offset, zval *value);


/* Used to create pointer to the property of the object, for future direct r/w access */
typedef zval *(*zend_object_get_property_ptr_ptr_t)(zval *object, zval *member, int type, void **cache_slot);

/* Used to set object value. Can be used to override assignments and scalar
   write ops (like ++, +=) on the object */
typedef void (*zend_object_set_t)(zval *object, zval *value);

/* Used to get object value. Can be used when converting object value to
 * one of the basic types and when using scalar ops (like ++, +=) on the object
 */
typedef zval* (*zend_object_get_t)(zval *object, zval *rv);

/* Used to check if a property of the object exists */
/* param has_set_exists:
 * 0 (has) whether property exists and is not NULL
 * 1 (set) whether property exists and is true
 * 2 (exists) whether property exists
 */
typedef int (*zend_object_has_property_t)(zval *object, zval *member, int has_set_exists, void **cache_slot);

/* Used to check if a dimension of the object exists */
typedef int (*zend_object_has_dimension_t)(zval *object, zval *member, int check_empty);

/* Used to remove a property of the object */
typedef void (*zend_object_unset_property_t)(zval *object, zval *member, void **cache_slot);

/* Used to remove a dimension of the object */
typedef void (*zend_object_unset_dimension_t)(zval *object, zval *offset);

/* Used to get hash of the properties of the object, as hash of zval's */
typedef HashTable *(*zend_object_get_properties_t)(zval *object);

typedef HashTable *(*zend_object_get_debug_info_t)(zval *object, int *is_temp);

/* Used to call methods */
/* args on stack! */
/* Andi - EX(fbc) (function being called) needs to be initialized already in the INIT fcall opcode so that the parameters can be parsed the right way. We need to add another callback for this.
 */
typedef int (*zend_object_call_method_t)(zend_string *method, zend_object *object, INTERNAL_FUNCTION_PARAMETERS);
typedef union _zend_function *(*zend_object_get_method_t)(zend_object **object, zend_string *method, const zval *key);
typedef union _zend_function *(*zend_object_get_constructor_t)(zend_object *object);

/* Object maintenance/destruction */
typedef void (*zend_object_dtor_obj_t)(zend_object *object);
typedef void (*zend_object_free_obj_t)(zend_object *object);
typedef zend_object* (*zend_object_clone_obj_t)(zval *object);

/* Get class name for display in var_dump and other debugging functions.
 * Must be defined and must return a non-NULL value. */
typedef zend_string *(*zend_object_get_class_name_t)(const zend_object *object);

typedef int (*zend_object_compare_t)(zval *object1, zval *object2);
typedef int (*zend_object_compare_zvals_t)(zval *resul, zval *op1, zval *op2);

/* Cast an object to some other type
 */
typedef int (*zend_object_cast_t)(zval *readobj, zval *retval, int type);

/* updates *count to hold the number of elements present and returns SUCCESS.
 * Returns FAILURE if the object does not have any sense of overloaded dimensions */
typedef int (*zend_object_count_elements_t)(zval *object, zend_long *count);

typedef int (*zend_object_get_closure_t)(zval *obj, zend_class_entry **ce_ptr, union _zend_function **fptr_ptr, zend_object **obj_ptr);

typedef HashTable *(*zend_object_get_gc_t)(zval *object, zval **table, int *n);

typedef int (*zend_object_do_operation_t)(zend_uchar opcode, zval *result, zval *op1, zval *op2);

struct _zend_object_handlers {
	/* offset of real object header (usually zero) */
	int										offset;
	/* general object functions */
	zend_object_free_obj_t					free_obj;
	zend_object_dtor_obj_t					dtor_obj;
	zend_object_clone_obj_t					clone_obj;
	/* individual object functions */
	zend_object_read_property_t				read_property;
	zend_object_write_property_t			write_property;
	zend_object_read_dimension_t			read_dimension;
	zend_object_write_dimension_t			write_dimension;
	zend_object_get_property_ptr_ptr_t		get_property_ptr_ptr;
	zend_object_get_t						get;
	zend_object_set_t						set;
	zend_object_has_property_t				has_property;
	zend_object_unset_property_t			unset_property;
	zend_object_has_dimension_t				has_dimension;
	zend_object_unset_dimension_t			unset_dimension;
	zend_object_get_properties_t			get_properties;
	zend_object_get_method_t				get_method;
	zend_object_call_method_t				call_method;
	zend_object_get_constructor_t			get_constructor;
	zend_object_get_class_name_t			get_class_name;
	zend_object_compare_t					compare_objects;
	zend_object_cast_t						cast_object;
	zend_object_count_elements_t			count_elements;
	zend_object_get_debug_info_t			get_debug_info;
	zend_object_get_closure_t				get_closure;
	zend_object_get_gc_t					get_gc;
	zend_object_do_operation_t				do_operation;
	zend_object_compare_zvals_t				compare;
};

BEGIN_EXTERN_C()
extern ZEND_API zend_object_handlers std_object_handlers;

#define zend_get_function_root_class(fbc) \
	((fbc)->common.prototype ? (fbc)->common.prototype->common.scope : (fbc)->common.scope)

ZEND_API union _zend_function *zend_std_get_static_method(zend_class_entry *ce, zend_string *function_name_strval, const zval *key);
ZEND_API zval *zend_std_get_static_property(zend_class_entry *ce, zend_string *property_name, zend_bool silent);
ZEND_API ZEND_COLD zend_bool zend_std_unset_static_property(zend_class_entry *ce, zend_string *property_name);
ZEND_API union _zend_function *zend_std_get_constructor(zend_object *object);
ZEND_API struct _zend_property_info *zend_get_property_info(zend_class_entry *ce, zend_string *member, int silent);
ZEND_API HashTable *zend_std_get_properties(zval *object);
ZEND_API HashTable *zend_std_get_debug_info(zval *object, int *is_temp);
ZEND_API int zend_std_cast_object_tostring(zval *readobj, zval *writeobj, int type);
ZEND_API void zend_std_write_property(zval *object, zval *member, zval *value, void **cache_slot);
ZEND_API void rebuild_object_properties(zend_object *zobj);

ZEND_API int zend_check_private(union _zend_function *fbc, zend_class_entry *ce, zend_string *function_name);

ZEND_API int zend_check_protected(zend_class_entry *ce, zend_class_entry *scope);

ZEND_API int zend_check_property_access(zend_object *zobj, zend_string *prop_info_name);

ZEND_API zend_function *zend_get_call_trampoline_func(zend_class_entry *ce, zend_string *method_name, int is_static);

ZEND_API uint32_t *zend_get_property_guard(zend_object *zobj, zend_string *member);

#define zend_free_trampoline(func) do { \
		if ((func) == &EG(trampoline)) { \
			EG(trampoline).common.function_name = NULL; \
		} else { \
			efree(func); \
		} \
	} while (0)

END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
