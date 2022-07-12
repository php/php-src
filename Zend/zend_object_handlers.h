/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_OBJECT_HANDLERS_H
#define ZEND_OBJECT_HANDLERS_H

struct _zend_property_info;

#define ZEND_WRONG_PROPERTY_INFO \
	((struct _zend_property_info*)((intptr_t)-1))

#define ZEND_DYNAMIC_PROPERTY_OFFSET               ((uintptr_t)(intptr_t)(-1))

#define IS_VALID_PROPERTY_OFFSET(offset)           ((intptr_t)(offset) > 0)
#define IS_WRONG_PROPERTY_OFFSET(offset)           ((intptr_t)(offset) == 0)
#define IS_DYNAMIC_PROPERTY_OFFSET(offset)         ((intptr_t)(offset) < 0)

#define IS_UNKNOWN_DYNAMIC_PROPERTY_OFFSET(offset) (offset == ZEND_DYNAMIC_PROPERTY_OFFSET)
#define ZEND_DECODE_DYN_PROP_OFFSET(offset)        ((uintptr_t)(-(intptr_t)(offset) - 2))
#define ZEND_ENCODE_DYN_PROP_OFFSET(offset)        ((uintptr_t)(-((intptr_t)(offset) + 2)))


/* Used to fetch property from the object, read-only */
typedef zval *(*zend_object_read_property_t)(zend_object *object, zend_string *member, int type, void **cache_slot, zval *rv);

/* Used to fetch dimension from the object, read-only */
typedef zval *(*zend_object_read_dimension_t)(zend_object *object, zval *offset, int type, zval *rv);


/* Used to set property of the object
   You must return the final value of the assigned property.
*/
typedef zval *(*zend_object_write_property_t)(zend_object *object, zend_string *member, zval *value, void **cache_slot);

/* Used to set dimension of the object */
typedef void (*zend_object_write_dimension_t)(zend_object *object, zval *offset, zval *value);


/* Used to create pointer to the property of the object, for future direct r/w access.
 * May return one of:
 *  * A zval pointer, without incrementing the reference count.
 *  * &EG(error_zval), if an exception has been thrown.
 *  * NULL, if acquiring a direct pointer is not possible.
 *    In this case, the VM will fall back to using read_property and write_property.
 */
typedef zval *(*zend_object_get_property_ptr_ptr_t)(zend_object *object, zend_string *member, int type, void **cache_slot);

/* Used to check if a property of the object exists */
/* param has_set_exists:
 * 0 (has) whether property exists and is not NULL
 * 1 (set) whether property exists and is true
 * 2 (exists) whether property exists
 */
typedef int (*zend_object_has_property_t)(zend_object *object, zend_string *member, int has_set_exists, void **cache_slot);

/* Used to check if a dimension of the object exists */
typedef int (*zend_object_has_dimension_t)(zend_object *object, zval *member, int check_empty);

/* Used to remove a property of the object */
typedef void (*zend_object_unset_property_t)(zend_object *object, zend_string *member, void **cache_slot);

/* Used to remove a dimension of the object */
typedef void (*zend_object_unset_dimension_t)(zend_object *object, zval *offset);

/* Used to get hash of the properties of the object, as hash of zval's */
typedef HashTable *(*zend_object_get_properties_t)(zend_object *object);

typedef HashTable *(*zend_object_get_debug_info_t)(zend_object *object, int *is_temp);

typedef enum _zend_prop_purpose {
	/* Used for debugging. Supersedes get_debug_info handler. */
	ZEND_PROP_PURPOSE_DEBUG,
	/* Used for (array) casts. */
	ZEND_PROP_PURPOSE_ARRAY_CAST,
	/* Used for serialization using the "O" scheme.
	 * Unserialization will use __wakeup(). */
	ZEND_PROP_PURPOSE_SERIALIZE,
	/* Used for var_export().
	 * The data will be passed to __set_state() when evaluated. */
	ZEND_PROP_PURPOSE_VAR_EXPORT,
	/* Used for json_encode(). */
	ZEND_PROP_PURPOSE_JSON,
	/* Dummy member to ensure that "default" is specified. */
	_ZEND_PROP_PURPOSE_NON_EXHAUSTIVE_ENUM
} zend_prop_purpose;

/* The return value must be released using zend_release_properties(). */
typedef zend_array *(*zend_object_get_properties_for_t)(zend_object *object, zend_prop_purpose purpose);

/* Used to call methods */
/* args on stack! */
/* Andi - EX(fbc) (function being called) needs to be initialized already in the INIT fcall opcode so that the parameters can be parsed the right way. We need to add another callback for this.
 */
typedef zend_function *(*zend_object_get_method_t)(zend_object **object, zend_string *method, const zval *key);
typedef zend_function *(*zend_object_get_constructor_t)(zend_object *object);

/* free_obj should release any resources the object holds, without freeing the
 * object structure itself. The object does not need to be in a valid state after
 * free_obj finishes running.
 *
 * free_obj will always be invoked, even if the object leaks or a fatal error
 * occurs. However, during shutdown it may be called once the executor is no
 * longer active, in which case execution of user code may be skipped.
 */
typedef void (*zend_object_free_obj_t)(zend_object *object);

/* dtor_obj is called before free_obj. The object must remain in a valid state
 * after dtor_obj finishes running. Unlike free_obj, it is run prior to
 * deactivation of the executor during shutdown, which allows user code to run.
 *
 * This handler is not guaranteed to be called (e.g. on fatal error), and as
 * such should not be used to release resources or deallocate memory. Furthermore,
 * releasing resources in this handler can break detection of memory leaks, as
 * cycles may be broken early.
 *
 * dtor_obj should be used *only* to call user destruction hooks, such as __destruct.
 */
typedef void (*zend_object_dtor_obj_t)(zend_object *object);

typedef zend_object* (*zend_object_clone_obj_t)(zend_object *object);

/* Get class name for display in var_dump and other debugging functions.
 * Must be defined and must return a non-NULL value. */
typedef zend_string *(*zend_object_get_class_name_t)(const zend_object *object);

typedef int (*zend_object_compare_t)(zval *object1, zval *object2);

/* Cast an object to some other type.
 * readobj and retval must point to distinct zvals.
 */
typedef zend_result (*zend_object_cast_t)(zend_object *readobj, zval *retval, int type);

/* updates *count to hold the number of elements present and returns SUCCESS.
 * Returns FAILURE if the object does not have any sense of overloaded dimensions */
typedef zend_result (*zend_object_count_elements_t)(zend_object *object, zend_long *count);

typedef zend_result (*zend_object_get_closure_t)(zend_object *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zend_object **obj_ptr, bool check_only);

typedef HashTable *(*zend_object_get_gc_t)(zend_object *object, zval **table, int *n);

typedef zend_result (*zend_object_do_operation_t)(zend_uchar opcode, zval *result, zval *op1, zval *op2);

struct _zend_object_handlers {
	/* offset of real object header (usually zero) */
	int										offset;
	/* object handlers */
	zend_object_free_obj_t					free_obj;             /* required */
	zend_object_dtor_obj_t					dtor_obj;             /* required */
	zend_object_clone_obj_t					clone_obj;            /* optional */
	zend_object_read_property_t				read_property;        /* required */
	zend_object_write_property_t			write_property;       /* required */
	zend_object_read_dimension_t			read_dimension;       /* required */
	zend_object_write_dimension_t			write_dimension;      /* required */
	zend_object_get_property_ptr_ptr_t		get_property_ptr_ptr; /* required */
	zend_object_has_property_t				has_property;         /* required */
	zend_object_unset_property_t			unset_property;       /* required */
	zend_object_has_dimension_t				has_dimension;        /* required */
	zend_object_unset_dimension_t			unset_dimension;      /* required */
	zend_object_get_properties_t			get_properties;       /* required */
	zend_object_get_method_t				get_method;           /* required */
	zend_object_get_constructor_t			get_constructor;      /* required */
	zend_object_get_class_name_t			get_class_name;       /* required */
	zend_object_cast_t						cast_object;          /* required */
	zend_object_count_elements_t			count_elements;       /* optional */
	zend_object_get_debug_info_t			get_debug_info;       /* optional */
	zend_object_get_closure_t				get_closure;          /* optional */
	zend_object_get_gc_t					get_gc;               /* required */
	zend_object_do_operation_t				do_operation;         /* optional */
	zend_object_compare_t					compare;              /* required */
	zend_object_get_properties_for_t		get_properties_for;   /* optional */
};

BEGIN_EXTERN_C()
extern const ZEND_API zend_object_handlers std_object_handlers;

#define zend_get_std_object_handlers() \
	(&std_object_handlers)

#define zend_get_function_root_class(fbc) \
	((fbc)->common.prototype ? (fbc)->common.prototype->common.scope : (fbc)->common.scope)

#define ZEND_PROPERTY_ISSET     0x0          /* Property exists and is not NULL */
#define ZEND_PROPERTY_NOT_EMPTY ZEND_ISEMPTY /* Property is not empty */
#define ZEND_PROPERTY_EXISTS    0x2          /* Property exists */

ZEND_API void zend_class_init_statics(zend_class_entry *ce);
ZEND_API zend_function *zend_std_get_static_method(zend_class_entry *ce, zend_string *function_name_strval, const zval *key);
ZEND_API zval *zend_std_get_static_property_with_info(zend_class_entry *ce, zend_string *property_name, int type, struct _zend_property_info **prop_info);
ZEND_API zval *zend_std_get_static_property(zend_class_entry *ce, zend_string *property_name, int type);
ZEND_API ZEND_COLD bool zend_std_unset_static_property(zend_class_entry *ce, zend_string *property_name);
ZEND_API zend_function *zend_std_get_constructor(zend_object *object);
ZEND_API struct _zend_property_info *zend_get_property_info(zend_class_entry *ce, zend_string *member, int silent);
ZEND_API HashTable *zend_std_get_properties(zend_object *object);
ZEND_API HashTable *zend_std_get_gc(zend_object *object, zval **table, int *n);
ZEND_API HashTable *zend_std_get_debug_info(zend_object *object, int *is_temp);
ZEND_API zend_result zend_std_cast_object_tostring(zend_object *object, zval *writeobj, int type);
ZEND_API zval *zend_std_get_property_ptr_ptr(zend_object *object, zend_string *member, int type, void **cache_slot);
ZEND_API zval *zend_std_read_property(zend_object *object, zend_string *member, int type, void **cache_slot, zval *rv);
ZEND_API zval *zend_std_write_property(zend_object *object, zend_string *member, zval *value, void **cache_slot);
ZEND_API int zend_std_has_property(zend_object *object, zend_string *member, int has_set_exists, void **cache_slot);
ZEND_API void zend_std_unset_property(zend_object *object, zend_string *member, void **cache_slot);
ZEND_API zval *zend_std_read_dimension(zend_object *object, zval *offset, int type, zval *rv);
ZEND_API void zend_std_write_dimension(zend_object *object, zval *offset, zval *value);
ZEND_API int zend_std_has_dimension(zend_object *object, zval *offset, int check_empty);
ZEND_API void zend_std_unset_dimension(zend_object *object, zval *offset);
ZEND_API zend_function *zend_std_get_method(zend_object **obj_ptr, zend_string *method_name, const zval *key);
ZEND_API zend_string *zend_std_get_class_name(const zend_object *zobj);
ZEND_API int zend_std_compare_objects(zval *o1, zval *o2);
ZEND_API zend_result zend_std_get_closure(zend_object *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zend_object **obj_ptr, bool check_only);
ZEND_API void rebuild_object_properties(zend_object *zobj);

ZEND_API HashTable *zend_std_build_object_properties_array(zend_object *zobj);

/* Handler for objects that cannot be meaningfully compared.
 * Only objects with the same identity will be considered equal. */
ZEND_API int zend_objects_not_comparable(zval *o1, zval *o2);

ZEND_API int zend_check_protected(zend_class_entry *ce, zend_class_entry *scope);

ZEND_API int zend_check_property_access(zend_object *zobj, zend_string *prop_info_name, bool is_dynamic);

ZEND_API zend_function *zend_get_call_trampoline_func(zend_class_entry *ce, zend_string *method_name, int is_static);

ZEND_API uint32_t *zend_get_property_guard(zend_object *zobj, zend_string *member);

/* Default behavior for get_properties_for. For use as a fallback in custom
 * get_properties_for implementations. */
ZEND_API HashTable *zend_std_get_properties_for(zend_object *obj, zend_prop_purpose purpose);

/* Will call get_properties_for handler or use default behavior. For use by
 * consumers of the get_properties_for API. */
ZEND_API HashTable *zend_get_properties_for(zval *obj, zend_prop_purpose purpose);

#define zend_release_properties(ht) do { \
	if ((ht) && !(GC_FLAGS(ht) & GC_IMMUTABLE) && !GC_DELREF(ht)) { \
		zend_array_destroy(ht); \
	} \
} while (0)

#define zend_free_trampoline(func) do { \
		if ((func) == &EG(trampoline)) { \
			EG(trampoline).common.function_name = NULL; \
		} else { \
			efree(func); \
		} \
	} while (0)

/* Fallback to default comparison implementation if the arguments aren't both objects
 * and have the same compare() handler. You'll likely want to use this unless you
 * explicitly wish to support comparisons between objects and non-objects. */
#define ZEND_COMPARE_OBJECTS_FALLBACK(op1, op2) \
	if (Z_TYPE_P(op1) != IS_OBJECT || \
			Z_TYPE_P(op2) != IS_OBJECT || \
			Z_OBJ_HT_P(op1)->compare != Z_OBJ_HT_P(op2)->compare) { \
		return zend_std_compare_objects(op1, op2); \
	}

END_EXTERN_C()

#endif
