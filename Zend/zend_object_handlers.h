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
   You must return the final value of the assigned property.
*/
/* Used to set property of the object */
typedef zval *(*zend_object_write_property_t)(zval *object, zval *member, zval *value, void **cache_slot);

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
	/* array_key_exists(). Not intended for general use! */
	_ZEND_PROP_PURPOSE_ARRAY_KEY_EXISTS,
	/* Dummy member to ensure that "default" is specified. */
	_ZEND_PROP_PURPOSE_NON_EXHAUSTIVE_ENUM
} zend_prop_purpose;

/* The return value must be released using zend_release_properties(). */
typedef zend_array *(*zend_object_get_properties_for_t)(zval *object, zend_prop_purpose purpose);

/* Used to call methods */
/* args on stack! */
/* Andi - EX(fbc) (function being called) needs to be initialized already in the INIT fcall opcode so that the parameters can be parsed the right way. We need to add another callback for this.
 */
typedef int (*zend_object_call_method_t)(zend_string *method, zend_object *object, INTERNAL_FUNCTION_PARAMETERS);
typedef zend_function *(*zend_object_get_method_t)(zend_object **object, zend_string *method, const zval *key);
typedef zend_function *(*zend_object_get_constructor_t)(zend_object *object);

/* Object maintenance/destruction */
typedef void (*zend_object_dtor_obj_t)(zend_object *object);
typedef void (*zend_object_free_obj_t)(zend_object *object);
typedef zend_object* (*zend_object_clone_obj_t)(zval *object);

/* Get class name for display in var_dump and other debugging functions.
 * Must be defined and must return a non-NULL value. */
typedef zend_string *(*zend_object_get_class_name_t)(const zend_object *object);

typedef int (*zend_object_compare_t)(zval *object1, zval *object2);
typedef int (*zend_object_compare_zvals_t)(zval *result, zval *op1, zval *op2);

/* Cast an object to some other type.
 * readobj and retval must point to distinct zvals.
 */
typedef int (*zend_object_cast_t)(zval *readobj, zval *retval, int type);

/* updates *count to hold the number of elements present and returns SUCCESS.
 * Returns FAILURE if the object does not have any sense of overloaded dimensions */
typedef int (*zend_object_count_elements_t)(zval *object, zend_long *count);

typedef int (*zend_object_get_closure_t)(zval *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zend_object **obj_ptr);

typedef HashTable *(*zend_object_get_gc_t)(zval *object, zval **table, int *n);

typedef int (*zend_object_do_operation_t)(zend_uchar opcode, zval *result, zval *op1, zval *op2);

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
	zend_object_get_t						get;                  /* optional */
	zend_object_set_t						set;                  /* optional */
	zend_object_has_property_t				has_property;         /* required */
	zend_object_unset_property_t			unset_property;       /* required */
	zend_object_has_dimension_t				has_dimension;        /* required */
	zend_object_unset_dimension_t			unset_dimension;      /* required */
	zend_object_get_properties_t			get_properties;       /* required */
	zend_object_get_method_t				get_method;           /* required */
	zend_object_call_method_t				call_method;          /* optional */
	zend_object_get_constructor_t			get_constructor;      /* required */
	zend_object_get_class_name_t			get_class_name;       /* required */
	zend_object_compare_t					compare_objects;      /* optional */
	zend_object_cast_t						cast_object;          /* optional */
	zend_object_count_elements_t			count_elements;       /* optional */
	zend_object_get_debug_info_t			get_debug_info;       /* optional */
	zend_object_get_closure_t				get_closure;          /* optional */
	zend_object_get_gc_t					get_gc;               /* required */
	zend_object_do_operation_t				do_operation;         /* optional */
	zend_object_compare_zvals_t				compare;              /* optional */
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
ZEND_API ZEND_COLD zend_bool zend_std_unset_static_property(zend_class_entry *ce, zend_string *property_name);
ZEND_API zend_function *zend_std_get_constructor(zend_object *object);
ZEND_API struct _zend_property_info *zend_get_property_info(zend_class_entry *ce, zend_string *member, int silent);
ZEND_API HashTable *zend_std_get_properties(zval *object);
ZEND_API HashTable *zend_std_get_gc(zval *object, zval **table, int *n);
ZEND_API HashTable *zend_std_get_debug_info(zval *object, int *is_temp);
ZEND_API int zend_std_cast_object_tostring(zval *readobj, zval *writeobj, int type);
ZEND_API zval *zend_std_get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot);
ZEND_API zval *zend_std_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv);
ZEND_API zval *zend_std_write_property(zval *object, zval *member, zval *value, void **cache_slot);
ZEND_API int zend_std_has_property(zval *object, zval *member, int has_set_exists, void **cache_slot);
ZEND_API void zend_std_unset_property(zval *object, zval *member, void **cache_slot);
ZEND_API zval *zend_std_read_dimension(zval *object, zval *offset, int type, zval *rv);
ZEND_API void zend_std_write_dimension(zval *object, zval *offset, zval *value);
ZEND_API int zend_std_has_dimension(zval *object, zval *offset, int check_empty);
ZEND_API void zend_std_unset_dimension(zval *object, zval *offset);
ZEND_API zend_function *zend_std_get_method(zend_object **obj_ptr, zend_string *method_name, const zval *key);
ZEND_API zend_string *zend_std_get_class_name(const zend_object *zobj);
ZEND_API int zend_std_compare_objects(zval *o1, zval *o2);
ZEND_API int zend_std_get_closure(zval *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zend_object **obj_ptr);
ZEND_API void rebuild_object_properties(zend_object *zobj);

ZEND_API int zend_check_protected(zend_class_entry *ce, zend_class_entry *scope);

ZEND_API int zend_check_property_access(zend_object *zobj, zend_string *prop_info_name, zend_bool is_dynamic);

ZEND_API zend_function *zend_get_call_trampoline_func(zend_class_entry *ce, zend_string *method_name, int is_static);

ZEND_API uint32_t *zend_get_property_guard(zend_object *zobj, zend_string *member);

/* Default behavior for get_properties_for. For use as a fallback in custom
 * get_properties_for implementations. */
ZEND_API HashTable *zend_std_get_properties_for(zval *obj, zend_prop_purpose purpose);

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

END_EXTERN_C()

#endif
