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
*/

#ifndef ZEND_GENERICS_H
#define ZEND_GENERICS_H

#include "zend_types.h"
#include "zend_string.h"

/* Variance annotations for generic type parameters */
#define ZEND_GENERIC_VARIANCE_INVARIANT    0  /* default: no variance */
#define ZEND_GENERIC_VARIANCE_COVARIANT    1  /* "out T" — only in return positions */
#define ZEND_GENERIC_VARIANCE_CONTRAVARIANT 2 /* "in T" — only in parameter positions */

/* A single generic type parameter declaration (e.g., "T", "T: Countable", "out T") */
typedef struct _zend_generic_param {
	zend_string *name;        /* "T", "K", "V", etc. */
	zend_type constraint;     /* Bound type (e.g., Countable), or IS_UNDEF */
	zend_type default_type;   /* Default type (e.g., string in V = string), or IS_UNDEF */
	uint8_t variance;         /* ZEND_GENERIC_VARIANCE_* */
} zend_generic_param;

/* Generic parameters info attached to a generic class/function.
 * Variable-length struct: params[0..num_params-1]. */
typedef struct _zend_generic_params_info {
	uint32_t num_params;
	zend_generic_param params[1]; /* Flexible array */
} zend_generic_params_info;

/* Bound generic type arguments (e.g., <int, string>).
 * Refcounted, single contiguous allocation:
 *   [refcount | num_args | args[0..N-1] | resolved_masks[0..N-1]]
 * refcount=0 means unmanaged (SHM/persistent). */
typedef struct _zend_generic_args {
	uint32_t refcount;
	uint32_t num_args;
	zend_type args[1]; /* Flexible array: args[0..num_args-1], followed by uint32_t masks[num_args] */
} zend_generic_args;

/* Access the pre-computed MAY_BE_ANY masks stored inline after the args array */
#define ZEND_GENERIC_ARGS_MASKS(a) ((uint32_t*)(&(a)->args[(a)->num_args]))

/* Reference to a generic type parameter in a zend_type.
 * Stored in zend_type.ptr when MAY_BE_GENERIC_PARAM is set. */
typedef struct _zend_generic_type_ref {
	zend_string *name;       /* "T" — for error messages */
	uint32_t param_index;    /* Index into generic_params_info */
	uint8_t variance;        /* ZEND_GENERIC_VARIANCE_* (copied from param decl) */
} zend_generic_type_ref;

/* Wildcard bound kinds for generic type arguments */
#define ZEND_GENERIC_BOUND_NONE    0  /* Exact type: Collection<int> */
#define ZEND_GENERIC_BOUND_UPPER   1  /* ? extends Foo: Collection<? extends Countable> */
#define ZEND_GENERIC_BOUND_LOWER   2  /* ? super Foo: Collection<? super Dog> */
#define ZEND_GENERIC_BOUND_UNBOUND 3  /* ?: Collection<?> (any type) */

/* A class type reference with generic arguments (e.g., Collection<int>).
 * Stored in zend_type.ptr when _ZEND_TYPE_GENERIC_CLASS_BIT is set. */
typedef struct _zend_generic_class_ref {
	zend_string *class_name;
	zend_generic_args *type_args;
	uint8_t *wildcard_bounds;  /* Array of ZEND_GENERIC_BOUND_* (NULL if no wildcards) */
} zend_generic_class_ref;

/* Allocation size for zend_generic_args with N args + N masks */
#define ZEND_GENERIC_ARGS_SIZE(n) \
	(offsetof(zend_generic_args, args) + (n) * sizeof(zend_type) + (n) * sizeof(uint32_t))

/* Allocation size for zend_generic_params_info with N params + N arg map entries */
#define ZEND_GENERIC_PARAMS_INFO_SIZE(n) \
	(offsetof(zend_generic_params_info, params) + (n) * sizeof(zend_generic_param) + (n) * sizeof(int16_t))

/* Pre-computed mapping: param_to_arg_map[param_index] = function arg index.
 * Stored inline after the params[] array. Only meaningful for function-level
 * generic params. Eliminates O(N) arg_info scanning during return type checks. */
#define ZEND_GENERIC_PARAMS_ARG_MAP(info) \
	((int16_t*)(&(info)->params[(info)->num_params]))

#define ZEND_GENERIC_ARG_UNMAPPED  ((int16_t)-1)  /* Param doesn't appear in any argument */
#define ZEND_GENERIC_ARG_VARIADIC  ((int16_t)-2)  /* Param appears in the variadic argument */

/* Allocation */
ZEND_API zend_generic_params_info *zend_alloc_generic_params_info(uint32_t num_params);
ZEND_API zend_generic_args *zend_alloc_generic_args(uint32_t num_args);

/* Pre-compute MAY_BE_ANY bitmasks for O(1) scalar type checking */
ZEND_API void zend_generic_args_compute_masks(zend_generic_args *args);
ZEND_API zend_type zend_copy_generic_type(zend_type src);
ZEND_API zend_generic_args *zend_copy_generic_args(const zend_generic_args *src);

/* Destruction — frees types and the struct itself */
ZEND_API void zend_generic_params_info_dtor(zend_generic_params_info *info);
ZEND_API void zend_generic_args_dtor(zend_generic_args *args);
ZEND_API void zend_generic_type_ref_dtor(zend_generic_type_ref *ref);
ZEND_API void zend_generic_class_ref_dtor(zend_generic_class_ref *ref);

/* Refcount management for generic args.
 * refcount=0 means unmanaged (SHM/persistent) — addref/release are no-ops. */
static zend_always_inline void zend_generic_args_addref(zend_generic_args *args) {
	if (args->refcount > 0) {
		args->refcount++;
	}
}

static zend_always_inline void zend_generic_args_release(zend_generic_args *args) {
	if (args->refcount > 0 && --args->refcount == 0) {
		zend_generic_args_dtor(args);
	}
}

/* Resolution: resolves a generic type param to its concrete type */
ZEND_API zend_type zend_resolve_generic_type(zend_type type, const zend_generic_args *args);

/* Resolve generic param refs in args using a context (e.g., new Box<T> inside Factory<int>).
 * Returns a new resolved args struct if any param refs were resolved, or NULL if none found. */
ZEND_API zend_generic_args *zend_resolve_generic_args_with_context(
	const zend_generic_args *args, const zend_generic_args *context);

/* Expand generic args with defaults from params_info.
 * Returns a new expanded args struct, or NULL if no expansion needed. */
ZEND_API zend_generic_args *zend_expand_generic_args_with_defaults(
	const zend_generic_params_info *params, const zend_generic_args *args);

/* Validation: checks that generic args satisfy constraints */
ZEND_API bool zend_verify_generic_args(
	const zend_generic_params_info *params, const zend_generic_args *args);

/* Comparison: checks if two sets of generic args are compatible.
 * params_info is optional (may be NULL) — when provided, variance annotations are respected.
 * wildcard_bounds is optional (may be NULL) — array of ZEND_GENERIC_BOUND_* per expected arg. */
ZEND_API bool zend_generic_args_compatible(
	const zend_generic_args *expected, const zend_generic_args *actual,
	const zend_generic_params_info *params_info,
	const uint8_t *wildcard_bounds);

/* Infer a zend_type from a runtime zval value */
ZEND_API zend_type zend_infer_type_from_zval(const zval *value);

/* Infer generic type args from constructor call arguments.
 * Returns true if inference succeeded and args were bound to the object. */
ZEND_API bool zend_infer_generic_args_from_constructor(zend_object *obj, zend_execute_data *call);

/* Infer generic type args from a generic function's call arguments.
 * Returns a new zend_generic_args (refcount=1) or NULL if inference fails.
 * Caller must call zend_generic_args_release() on the result. */
ZEND_API zend_generic_args *zend_infer_generic_args_from_call(zend_execute_data *ex);

/* Get current generic args from execution context */
ZEND_API zend_generic_args *zend_get_current_generic_args(void);

/* Format generic args as a string, e.g. "<int, string>" */
ZEND_API zend_string *zend_generic_args_to_string(const zend_generic_args *args);

/* Parse generic args from a string, e.g. "<int, string>" → zend_generic_args */
ZEND_API zend_generic_args *zend_generic_args_from_string(const char *str, size_t len);

/* Get class name with generic args for display, e.g. "Box<int>" */
ZEND_API zend_string *zend_object_get_class_name_with_generics(const zend_object *obj);

#endif /* ZEND_GENERICS_H */
