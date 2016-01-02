/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
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

union _zend_function;
struct _zend_property_info;
struct _zend_literal;

/* The following rule applies to read_property() and read_dimension() implementations:
   If you return a zval which is not otherwise referenced by the extension or the engine's
   symbol table, its reference count should be 0.
*/
/* Used to fetch property from the object, read-only */
typedef zval *(*zend_object_read_property_t)(zval *object, zval *member, int type, const struct _zend_literal *key TSRMLS_DC);

/* Used to fetch dimension from the object, read-only */
typedef zval *(*zend_object_read_dimension_t)(zval *object, zval *offset, int type TSRMLS_DC);


/* The following rule applies to write_property() and write_dimension() implementations:
   If you receive a value zval in write_property/write_dimension, you may only modify it if
   its reference count is 1.  Otherwise, you must create a copy of that zval before making
   any changes.  You should NOT modify the reference count of the value passed to you.
*/
/* Used to set property of the object */
typedef void (*zend_object_write_property_t)(zval *object, zval *member, zval *value, const struct _zend_literal *key TSRMLS_DC);

/* Used to set dimension of the object */
typedef void (*zend_object_write_dimension_t)(zval *object, zval *offset, zval *value TSRMLS_DC);


/* Used to create pointer to the property of the object, for future direct r/w access */
typedef zval **(*zend_object_get_property_ptr_ptr_t)(zval *object, zval *member, int type, const struct _zend_literal *key TSRMLS_DC);

/* Used to set object value. Can be used to override assignments and scalar
   write ops (like ++, +=) on the object */
typedef void (*zend_object_set_t)(zval **object, zval *value TSRMLS_DC);

/* Used to get object value. Can be used when converting object value to
 * one of the basic types and when using scalar ops (like ++, +=) on the object
 */
typedef zval* (*zend_object_get_t)(zval *object TSRMLS_DC);

/* Used to check if a property of the object exists */
/* param has_set_exists:
 * 0 (has) whether property exists and is not NULL
 * 1 (set) whether property exists and is true
 * 2 (exists) whether property exists
 */
typedef int (*zend_object_has_property_t)(zval *object, zval *member, int has_set_exists, const struct _zend_literal *key TSRMLS_DC);

/* Used to check if a dimension of the object exists */
typedef int (*zend_object_has_dimension_t)(zval *object, zval *member, int check_empty TSRMLS_DC);

/* Used to remove a property of the object */
typedef void (*zend_object_unset_property_t)(zval *object, zval *member, const struct _zend_literal *key TSRMLS_DC);

/* Used to remove a dimension of the object */
typedef void (*zend_object_unset_dimension_t)(zval *object, zval *offset TSRMLS_DC);

/* Used to get hash of the properties of the object, as hash of zval's */
typedef HashTable *(*zend_object_get_properties_t)(zval *object TSRMLS_DC);

typedef HashTable *(*zend_object_get_debug_info_t)(zval *object, int *is_temp TSRMLS_DC);

/* Used to call methods */
/* args on stack! */
/* Andi - EX(fbc) (function being called) needs to be initialized already in the INIT fcall opcode so that the parameters can be parsed the right way. We need to add another callback for this.
 */
typedef int (*zend_object_call_method_t)(const char *method, INTERNAL_FUNCTION_PARAMETERS);
typedef union _zend_function *(*zend_object_get_method_t)(zval **object_ptr, char *method, int method_len, const struct _zend_literal *key TSRMLS_DC);
typedef union _zend_function *(*zend_object_get_constructor_t)(zval *object TSRMLS_DC);

/* Object maintenance/destruction */
typedef void (*zend_object_add_ref_t)(zval *object TSRMLS_DC);
typedef void (*zend_object_del_ref_t)(zval *object TSRMLS_DC);
typedef void (*zend_object_delete_obj_t)(zval *object TSRMLS_DC);
typedef zend_object_value (*zend_object_clone_obj_t)(zval *object TSRMLS_DC);

typedef zend_class_entry *(*zend_object_get_class_entry_t)(const zval *object TSRMLS_DC);
typedef int (*zend_object_get_class_name_t)(const zval *object, const char **class_name, zend_uint *class_name_len, int parent TSRMLS_DC);
typedef int (*zend_object_compare_t)(zval *object1, zval *object2 TSRMLS_DC);
typedef int (*zend_object_compare_zvals_t)(zval *resul, zval *op1, zval *op2 TSRMLS_DC);

/* Cast an object to some other type
 */
typedef int (*zend_object_cast_t)(zval *readobj, zval *retval, int type TSRMLS_DC);

/* updates *count to hold the number of elements present and returns SUCCESS.
 * Returns FAILURE if the object does not have any sense of overloaded dimensions */
typedef int (*zend_object_count_elements_t)(zval *object, long *count TSRMLS_DC);

typedef int (*zend_object_get_closure_t)(zval *obj, zend_class_entry **ce_ptr, union _zend_function **fptr_ptr, zval **zobj_ptr TSRMLS_DC);

typedef HashTable *(*zend_object_get_gc_t)(zval *object, zval ***table, int *n TSRMLS_DC);

typedef int (*zend_object_do_operation_t)(zend_uchar opcode, zval *result, zval *op1, zval *op2 TSRMLS_DC);

struct _zend_object_handlers {
	/* general object functions */
	zend_object_add_ref_t					add_ref;
	zend_object_del_ref_t					del_ref;
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
	zend_object_get_class_entry_t			get_class_entry;
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

extern ZEND_API zend_object_handlers std_object_handlers;

#define zend_get_function_root_class(fbc) \
	((fbc)->common.prototype ? (fbc)->common.prototype->common.scope : (fbc)->common.scope)

BEGIN_EXTERN_C()
ZEND_API union _zend_function *zend_std_get_static_method(zend_class_entry *ce, const char *function_name_strval, int function_name_strlen, const struct _zend_literal *key TSRMLS_DC);
ZEND_API zval **zend_std_get_static_property(zend_class_entry *ce, const char *property_name, int property_name_len, zend_bool silent, const struct _zend_literal *key TSRMLS_DC);
ZEND_API zend_bool zend_std_unset_static_property(zend_class_entry *ce, const char *property_name, int property_name_len, const struct _zend_literal *key TSRMLS_DC);
ZEND_API union _zend_function *zend_std_get_constructor(zval *object TSRMLS_DC);
ZEND_API struct _zend_property_info *zend_get_property_info(zend_class_entry *ce, zval *member, int silent TSRMLS_DC);
ZEND_API HashTable *zend_std_get_properties(zval *object TSRMLS_DC);
ZEND_API HashTable *zend_std_get_debug_info(zval *object, int *is_temp TSRMLS_DC);
ZEND_API int zend_std_cast_object_tostring(zval *readobj, zval *writeobj, int type TSRMLS_DC);
ZEND_API void zend_std_write_property(zval *object, zval *member, zval *value, const struct _zend_literal *key TSRMLS_DC);
ZEND_API void rebuild_object_properties(zend_object *zobj);


#define IS_ZEND_STD_OBJECT(z)  (Z_TYPE(z) == IS_OBJECT && (Z_OBJ_HT((z))->get_class_entry != NULL))
#define HAS_CLASS_ENTRY(z) (Z_OBJ_HT(z)->get_class_entry != NULL)

ZEND_API int zend_check_private(union _zend_function *fbc, zend_class_entry *ce, char *function_name_strval, int function_name_strlen TSRMLS_DC);

ZEND_API int zend_check_protected(zend_class_entry *ce, zend_class_entry *scope);

ZEND_API int zend_check_property_access(zend_object *zobj, const char *prop_info_name, int prop_info_name_len TSRMLS_DC);

ZEND_API void zend_std_call_user_call(INTERNAL_FUNCTION_PARAMETERS);
END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
