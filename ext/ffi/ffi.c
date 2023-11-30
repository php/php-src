/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Dmitry Stogov <dmitry@zend.com>                              |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ffi.h"
#include "ext/standard/info.h"
#include "php_scandir.h"
#include "zend_exceptions.h"
#include "zend_closures.h"
#include "zend_weakrefs.h"
#include "main/SAPI.h"

#include <ffi.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef HAVE_LIBDL
#ifdef PHP_WIN32
#include "win32/param.h"
#include "win32/winutil.h"
#define GET_DL_ERROR()  php_win_err()
#else
#include <sys/param.h>
#define GET_DL_ERROR()  DL_ERROR()
#endif
#endif

#ifdef HAVE_GLOB
#ifdef PHP_WIN32
#include "win32/glob.h"
#else
#include <glob.h>
#endif
#endif

#ifndef __BIGGEST_ALIGNMENT__
/* XXX need something better, perhaps with regard to SIMD, etc. */
# define __BIGGEST_ALIGNMENT__ sizeof(size_t)
#endif

//#define FFI_DEBUG
#ifdef FFI_DEBUG
#define FFI_DPRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define FFI_DPRINTF(fmt, ...) 
#endif

ZEND_DECLARE_MODULE_GLOBALS(ffi)

typedef enum _zend_ffi_tag_kind {
	ZEND_FFI_TAG_ENUM,
	ZEND_FFI_TAG_STRUCT,
	ZEND_FFI_TAG_UNION
} zend_ffi_tag_kind;

static const char *zend_ffi_tag_kind_name[3] = {"enum", "struct", "union"};


typedef struct _zend_ffi_tag {
	zend_ffi_tag_kind      kind;
	zend_ffi_type         *type;
} zend_ffi_tag;

typedef enum _zend_ffi_type_kind {
	ZEND_FFI_TYPE_VOID,
	ZEND_FFI_TYPE_FLOAT,
	ZEND_FFI_TYPE_DOUBLE,
#ifdef HAVE_LONG_DOUBLE
	ZEND_FFI_TYPE_LONGDOUBLE,
#endif
	ZEND_FFI_TYPE_UINT8,
	ZEND_FFI_TYPE_SINT8,
	ZEND_FFI_TYPE_UINT16,
	ZEND_FFI_TYPE_SINT16,
	ZEND_FFI_TYPE_UINT32,
	ZEND_FFI_TYPE_SINT32,
	ZEND_FFI_TYPE_UINT64,
	ZEND_FFI_TYPE_SINT64,
	ZEND_FFI_TYPE_ENUM,
	ZEND_FFI_TYPE_BOOL,
	ZEND_FFI_TYPE_CHAR,
	ZEND_FFI_TYPE_POINTER,
	ZEND_FFI_TYPE_FUNC,
	ZEND_FFI_TYPE_ARRAY,
	ZEND_FFI_TYPE_STRUCT,
} zend_ffi_type_kind;

#include "ffi_arginfo.h"

typedef enum _zend_ffi_flags {
	ZEND_FFI_FLAG_CONST      = (1 << 0),
	ZEND_FFI_FLAG_OWNED      = (1 << 1),
	ZEND_FFI_FLAG_PERSISTENT = (1 << 2),
} zend_ffi_flags;

struct _zend_ffi_type {
	zend_ffi_type_kind     kind;
	size_t                 size;
	uint32_t               align;
	uint32_t               attr;
	union {
		struct {
			zend_string        *tag_name;
			zend_ffi_type_kind  kind;
		} enumeration;
		struct {
			zend_ffi_type *type;
			zend_long      length;
		} array;
		struct {
			zend_ffi_type *type;
		} pointer;
		struct {
			zend_string   *tag_name;
			HashTable      fields;
		} record;
		struct {
			zend_ffi_type *ret_type;
			HashTable     *args;
			ffi_abi        abi;
		} func;
	};
};

typedef struct _zend_ffi_field {
	size_t                 offset;
	bool              is_const;
	bool              is_nested; /* part of nested anonymous struct */
	uint8_t                first_bit;
	uint8_t                bits;
	zend_ffi_type         *type;
} zend_ffi_field;

typedef enum _zend_ffi_symbol_kind {
	ZEND_FFI_SYM_TYPE,
	ZEND_FFI_SYM_CONST,
	ZEND_FFI_SYM_VAR,
	ZEND_FFI_SYM_FUNC
} zend_ffi_symbol_kind;

typedef struct _zend_ffi_symbol {
	zend_ffi_symbol_kind   kind;
	bool              is_const;
	zend_ffi_type         *type;
	union {
		void *addr;
		int64_t value;
	};
} zend_ffi_symbol;

typedef struct _zend_ffi_scope {
	HashTable             *symbols;
	HashTable             *tags;
} zend_ffi_scope;

typedef struct _zend_ffi {
	zend_object            std;
	DL_HANDLE              lib;
	HashTable             *symbols;
	HashTable             *tags;
	bool              persistent;
} zend_ffi;

#define ZEND_FFI_TYPE_OWNED        (1<<0)

#define ZEND_FFI_TYPE(t) \
	((zend_ffi_type*)(((uintptr_t)(t)) & ~ZEND_FFI_TYPE_OWNED))

#define ZEND_FFI_TYPE_IS_OWNED(t) \
	(((uintptr_t)(t)) & ZEND_FFI_TYPE_OWNED)

#define ZEND_FFI_TYPE_MAKE_OWNED(t) \
	((zend_ffi_type*)(((uintptr_t)(t)) | ZEND_FFI_TYPE_OWNED))

#define ZEND_FFI_SIZEOF_ARG \
	MAX(FFI_SIZEOF_ARG, sizeof(double))

typedef struct _zend_ffi_cdata {
	zend_object            std;
	zend_ffi_type         *type;
	void                  *ptr;
	void                  *ptr_holder;
	zend_ffi_flags         flags;
} zend_ffi_cdata;

typedef struct _zend_ffi_ctype {
	zend_object            std;
	zend_ffi_type         *type;
} zend_ffi_ctype;

static zend_class_entry *zend_ffi_exception_ce;
static zend_class_entry *zend_ffi_parser_exception_ce;
static zend_class_entry *zend_ffi_ce;
static zend_class_entry *zend_ffi_cdata_ce;
static zend_class_entry *zend_ffi_ctype_ce;

static zend_object_handlers zend_ffi_handlers;
static zend_object_handlers zend_ffi_cdata_handlers;
static zend_object_handlers zend_ffi_cdata_value_handlers;
static zend_object_handlers zend_ffi_cdata_free_handlers;
static zend_object_handlers zend_ffi_ctype_handlers;

static zend_internal_function zend_ffi_new_fn;
static zend_internal_function zend_ffi_cast_fn;
static zend_internal_function zend_ffi_type_fn;

/* forward declarations */
static void _zend_ffi_type_dtor(zend_ffi_type *type);
static void zend_ffi_finalize_type(zend_ffi_dcl *dcl);
static bool zend_ffi_is_same_type(zend_ffi_type *type1, zend_ffi_type *type2);
static zend_ffi_type *zend_ffi_remember_type(zend_ffi_type *type);
static char *zend_ffi_parse_directives(const char *filename, char *code_pos, char **scope_name, char **lib, bool preload);
static ZEND_FUNCTION(ffi_trampoline);
static ZEND_COLD void zend_ffi_return_unsupported(zend_ffi_type *type);
static ZEND_COLD void zend_ffi_pass_unsupported(zend_ffi_type *type);
static ZEND_COLD void zend_ffi_assign_incompatible(zval *arg, zend_ffi_type *type);
static bool zend_ffi_is_compatible_type(zend_ffi_type *dst_type, zend_ffi_type *src_type);
static void zend_ffi_wait_cond(
	pthread_mutex_t *mutex, pthread_cond_t *cond,
	zend_atomic_bool *flag, bool wanted_value, bool release
);

#if FFI_CLOSURES
static void *zend_ffi_create_callback(zend_ffi_type *type, zval *value);
#endif

static zend_always_inline void zend_ffi_type_dtor(zend_ffi_type *type) /* {{{ */
{
	if (UNEXPECTED(ZEND_FFI_TYPE_IS_OWNED(type))) {
		_zend_ffi_type_dtor(type);
		return;
	}
}
/* }}} */

static zend_always_inline void zend_ffi_object_init(zend_object *object, zend_class_entry *ce) /* {{{ */
{
	GC_SET_REFCOUNT(object, 1);
	GC_TYPE_INFO(object) = GC_OBJECT | (IS_OBJ_DESTRUCTOR_CALLED << GC_FLAGS_SHIFT);
	object->ce = ce;
	object->handlers = ce->default_object_handlers;
	object->properties = NULL;
	zend_objects_store_put(object);
}
/* }}} */

static zend_object *zend_ffi_cdata_new(zend_class_entry *class_type) /* {{{ */
{
	zend_ffi_cdata *cdata;

	cdata = emalloc(sizeof(zend_ffi_cdata));

	zend_ffi_object_init(&cdata->std, class_type);

	cdata->type = NULL;
	cdata->ptr = NULL;
	cdata->flags = 0;

	return &cdata->std;
}
/* }}} */

static bool zend_ffi_func_ptr_are_compatible(zend_ffi_type *dst_type, zend_ffi_type *src_type) /* {{{ */
{
	uint32_t dst_argc, src_argc, i;
	zend_ffi_type *dst_arg, *src_arg;

	ZEND_ASSERT(dst_type->kind == ZEND_FFI_TYPE_FUNC);
	ZEND_ASSERT(src_type->kind == ZEND_FFI_TYPE_FUNC);

	/* Ensure calling convention matches */
	if (dst_type->func.abi != src_type->func.abi) {
		return 0;
	}

	/* Ensure variadic attr matches */
	if ((dst_type->attr & ZEND_FFI_ATTR_VARIADIC) != (src_type->attr & ZEND_FFI_ATTR_VARIADIC)) {
		return 0;
	}

	/* Ensure same arg count */
	dst_argc = dst_type->func.args ? zend_hash_num_elements(dst_type->func.args) : 0;
	src_argc = src_type->func.args ? zend_hash_num_elements(src_type->func.args) : 0;
	if (dst_argc != src_argc) {
		return 0;
	}

	/* Ensure compatible ret_type */
	if (!zend_ffi_is_compatible_type(dst_type->func.ret_type, src_type->func.ret_type)) {
		return 0;
	}

	/* Ensure compatible args */
	for (i = 0; i < dst_argc; i++) {
		dst_arg = zend_hash_index_find_ptr(dst_type->func.args, i);
		src_arg = zend_hash_index_find_ptr(src_type->func.args, i);
		if (!zend_ffi_is_compatible_type(ZEND_FFI_TYPE(dst_arg), ZEND_FFI_TYPE(src_arg))) {
			return 0;
		}
	}

	return 1;
}
/* }}} */

static bool zend_ffi_is_compatible_type(zend_ffi_type *dst_type, zend_ffi_type *src_type) /* {{{ */
{
	while (1) {
		if (dst_type == src_type) {
			return 1;
		} else if (dst_type->kind == src_type->kind) {
			if (dst_type->kind < ZEND_FFI_TYPE_POINTER) {
				return 1;
			} else if (dst_type->kind == ZEND_FFI_TYPE_POINTER) {
				dst_type = ZEND_FFI_TYPE(dst_type->pointer.type);
				src_type = ZEND_FFI_TYPE(src_type->pointer.type);
				if (dst_type->kind == ZEND_FFI_TYPE_VOID ||
				    src_type->kind == ZEND_FFI_TYPE_VOID) {
				    return 1;
				} else if (dst_type->kind == ZEND_FFI_TYPE_FUNC &&
				           src_type->kind == ZEND_FFI_TYPE_FUNC) {
				    return zend_ffi_func_ptr_are_compatible(dst_type, src_type);
				}
			} else if (dst_type->kind == ZEND_FFI_TYPE_ARRAY &&
			           (dst_type->array.length == src_type->array.length ||
			            dst_type->array.length == 0)) {
				dst_type = ZEND_FFI_TYPE(dst_type->array.type);
				src_type = ZEND_FFI_TYPE(src_type->array.type);
			} else {
				break;
			}
		} else if (dst_type->kind == ZEND_FFI_TYPE_POINTER &&
		           src_type->kind == ZEND_FFI_TYPE_ARRAY) {
			dst_type = ZEND_FFI_TYPE(dst_type->pointer.type);
			src_type = ZEND_FFI_TYPE(src_type->array.type);
			if (dst_type->kind == ZEND_FFI_TYPE_VOID) {
			    return 1;
			}
		} else {
			break;
		}
	}
	return 0;
}
/* }}} */

static ffi_type* zend_ffi_face_struct_add_fields(ffi_type* t, zend_ffi_type *type, int *i, size_t size)
{
	zend_ffi_field *field;

	ZEND_HASH_MAP_FOREACH_PTR(&type->record.fields, field) {
		switch (ZEND_FFI_TYPE(field->type)->kind) {
			case ZEND_FFI_TYPE_FLOAT:
				t->elements[(*i)++] = &ffi_type_float;
				break;
			case ZEND_FFI_TYPE_DOUBLE:
				t->elements[(*i)++] = &ffi_type_double;
				break;
#ifdef HAVE_LONG_DOUBLE
			case ZEND_FFI_TYPE_LONGDOUBLE:
				t->elements[(*i)++] = &ffi_type_longdouble;
				break;
#endif
			case ZEND_FFI_TYPE_SINT8:
			case ZEND_FFI_TYPE_UINT8:
			case ZEND_FFI_TYPE_BOOL:
			case ZEND_FFI_TYPE_CHAR:
				t->elements[(*i)++] = &ffi_type_uint8;
				break;
			case ZEND_FFI_TYPE_SINT16:
			case ZEND_FFI_TYPE_UINT16:
				t->elements[(*i)++] = &ffi_type_uint16;
				break;
			case ZEND_FFI_TYPE_SINT32:
			case ZEND_FFI_TYPE_UINT32:
				t->elements[(*i)++] = &ffi_type_uint32;
				break;
			case ZEND_FFI_TYPE_SINT64:
			case ZEND_FFI_TYPE_UINT64:
				t->elements[(*i)++] = &ffi_type_uint64;
				break;
			case ZEND_FFI_TYPE_POINTER:
				t->elements[(*i)++] = &ffi_type_pointer;
				break;
			case ZEND_FFI_TYPE_STRUCT: {
				zend_ffi_type *field_type = ZEND_FFI_TYPE(field->type);
				/* for unions we use only the first field */
				uint32_t num_fields = !(field_type->attr & ZEND_FFI_ATTR_UNION) ?
					zend_hash_num_elements(&field_type->record.fields) : 1;

				if (num_fields > 1) {
					size += sizeof(ffi_type*) * (num_fields - 1);
					t = erealloc(t, size);
					t->elements = (ffi_type**)(t + 1);
				}
				t = zend_ffi_face_struct_add_fields(t, field_type, i, size);
				break;
			}
			default:
				t->elements[(*i)++] = &ffi_type_void;
				break;
		}
		if (type->attr & ZEND_FFI_ATTR_UNION) {
			/* for unions we use only the first field */
			break;
		}
	} ZEND_HASH_FOREACH_END();
	return t;
}

static ffi_type *zend_ffi_make_fake_struct_type(zend_ffi_type *type) /* {{{ */
{
	/* for unions we use only the first field */
	uint32_t num_fields = !(type->attr & ZEND_FFI_ATTR_UNION) ?
		zend_hash_num_elements(&type->record.fields) : 1;
	size_t size = sizeof(ffi_type) + sizeof(ffi_type*) * (num_fields + 1);
	ffi_type *t = emalloc(size);
	int i;

	t->size = type->size;
	t->alignment = type->align;
	t->type = FFI_TYPE_STRUCT;
	t->elements = (ffi_type**)(t + 1);
	i = 0;
	t = zend_ffi_face_struct_add_fields(t, type, &i, size);
	t->elements[i] = NULL;
	return t;
}
/* }}} */

static ffi_type *zend_ffi_get_type(zend_ffi_type *type) /* {{{ */
{
	zend_ffi_type_kind kind = type->kind;

again:
	switch (kind) {
		case ZEND_FFI_TYPE_FLOAT:
			return &ffi_type_float;
		case ZEND_FFI_TYPE_DOUBLE:
			return &ffi_type_double;
#ifdef HAVE_LONG_DOUBLE
		case ZEND_FFI_TYPE_LONGDOUBLE:
			return &ffi_type_longdouble;
#endif
		case ZEND_FFI_TYPE_UINT8:
			return &ffi_type_uint8;
		case ZEND_FFI_TYPE_SINT8:
			return &ffi_type_sint8;
		case ZEND_FFI_TYPE_UINT16:
			return &ffi_type_uint16;
		case ZEND_FFI_TYPE_SINT16:
			return &ffi_type_sint16;
		case ZEND_FFI_TYPE_UINT32:
			return &ffi_type_uint32;
		case ZEND_FFI_TYPE_SINT32:
			return &ffi_type_sint32;
		case ZEND_FFI_TYPE_UINT64:
			return &ffi_type_uint64;
		case ZEND_FFI_TYPE_SINT64:
			return &ffi_type_sint64;
		case ZEND_FFI_TYPE_POINTER:
			return &ffi_type_pointer;
		case ZEND_FFI_TYPE_VOID:
			return &ffi_type_void;
		case ZEND_FFI_TYPE_BOOL:
			return &ffi_type_uint8;
		case ZEND_FFI_TYPE_CHAR:
			return &ffi_type_sint8;
		case ZEND_FFI_TYPE_ENUM:
			kind = type->enumeration.kind;
			goto again;
		case ZEND_FFI_TYPE_STRUCT:
			return zend_ffi_make_fake_struct_type(type);
		default:
			break;
	}
	return NULL;
}
/* }}} */

static zend_never_inline zend_ffi_cdata *zend_ffi_cdata_to_zval_slow(void *ptr, zend_ffi_type *type, zend_ffi_flags flags) /* {{{ */
{
	zend_ffi_cdata *cdata = emalloc(sizeof(zend_ffi_cdata));

	zend_ffi_object_init(&cdata->std, zend_ffi_cdata_ce);
	cdata->std.handlers =
		(type->kind < ZEND_FFI_TYPE_POINTER) ?
		&zend_ffi_cdata_value_handlers :
		&zend_ffi_cdata_handlers;
	cdata->type = type;
	cdata->flags = flags;
	cdata->ptr = ptr;
	return cdata;
}
/* }}} */

static zend_never_inline zend_ffi_cdata *zend_ffi_cdata_to_zval_slow_ptr(void *ptr, zend_ffi_type *type, zend_ffi_flags flags) /* {{{ */
{
	zend_ffi_cdata *cdata = emalloc(sizeof(zend_ffi_cdata));

	zend_ffi_object_init(&cdata->std, zend_ffi_cdata_ce);
	cdata->type = type;
	cdata->flags = flags;
	cdata->ptr = (void*)&cdata->ptr_holder;
	*(void**)cdata->ptr = *(void**)ptr;
	return cdata;
}
/* }}} */

static zend_never_inline zend_ffi_cdata *zend_ffi_cdata_to_zval_slow_ret(void *ptr, zend_ffi_type *type, zend_ffi_flags flags) /* {{{ */
{
	zend_ffi_cdata *cdata = emalloc(sizeof(zend_ffi_cdata));

	zend_ffi_object_init(&cdata->std, zend_ffi_cdata_ce);
	cdata->std.handlers =
		(type->kind < ZEND_FFI_TYPE_POINTER) ?
		&zend_ffi_cdata_value_handlers :
		&zend_ffi_cdata_handlers;
	cdata->type = type;
	cdata->flags = flags;
	if (type->kind == ZEND_FFI_TYPE_POINTER) {
		cdata->ptr = (void*)&cdata->ptr_holder;
		*(void**)cdata->ptr = *(void**)ptr;
	} else if (type->kind == ZEND_FFI_TYPE_STRUCT) {
		cdata->ptr = emalloc(type->size);
		cdata->flags |= ZEND_FFI_FLAG_OWNED;
		memcpy(cdata->ptr, ptr, type->size);
	} else {
		cdata->ptr = ptr;
	}
	return cdata;
}
/* }}} */

static zend_always_inline void zend_ffi_cdata_to_zval(zend_ffi_cdata *cdata, void *ptr, zend_ffi_type *type, int read_type, zval *rv, zend_ffi_flags flags, bool is_ret, bool debug_union) /* {{{ */
{
	if (read_type == BP_VAR_R) {
		zend_ffi_type_kind kind = type->kind;

again:
	    switch (kind) {
			case ZEND_FFI_TYPE_FLOAT:
				ZVAL_DOUBLE(rv, *(float*)ptr);
				return;
			case ZEND_FFI_TYPE_DOUBLE:
				ZVAL_DOUBLE(rv, *(double*)ptr);
				return;
#ifdef HAVE_LONG_DOUBLE
			case ZEND_FFI_TYPE_LONGDOUBLE:
				ZVAL_DOUBLE(rv, *(long double*)ptr);
				return;
#endif
			case ZEND_FFI_TYPE_UINT8:
				ZVAL_LONG(rv, *(uint8_t*)ptr);
				return;
			case ZEND_FFI_TYPE_SINT8:
				ZVAL_LONG(rv, *(int8_t*)ptr);
				return;
			case ZEND_FFI_TYPE_UINT16:
				ZVAL_LONG(rv, *(uint16_t*)ptr);
				return;
			case ZEND_FFI_TYPE_SINT16:
				ZVAL_LONG(rv, *(int16_t*)ptr);
				return;
			case ZEND_FFI_TYPE_UINT32:
				ZVAL_LONG(rv, *(uint32_t*)ptr);
				return;
			case ZEND_FFI_TYPE_SINT32:
				ZVAL_LONG(rv, *(int32_t*)ptr);
				return;
			case ZEND_FFI_TYPE_UINT64:
				ZVAL_LONG(rv, *(uint64_t*)ptr);
				return;
			case ZEND_FFI_TYPE_SINT64:
				ZVAL_LONG(rv, *(int64_t*)ptr);
				return;
			case ZEND_FFI_TYPE_BOOL:
				ZVAL_BOOL(rv, *(uint8_t*)ptr);
				return;
			case ZEND_FFI_TYPE_CHAR:
				ZVAL_CHAR(rv, *(char*)ptr);
				return;
			case ZEND_FFI_TYPE_ENUM:
				kind = type->enumeration.kind;
				goto again;
			case ZEND_FFI_TYPE_POINTER:
				if (*(void**)ptr == NULL) {
					ZVAL_NULL(rv);
					return;
				} else if (debug_union) {
					ZVAL_STR(rv, zend_strpprintf(0, "%p", *(void**)ptr));
					return;
				} else if ((type->attr & ZEND_FFI_ATTR_CONST) && ZEND_FFI_TYPE(type->pointer.type)->kind == ZEND_FFI_TYPE_CHAR) {
					ZVAL_STRING(rv, *(char**)ptr);
					return;
				}
				if (!cdata) {
					if (is_ret) {
						cdata = zend_ffi_cdata_to_zval_slow_ret(ptr, type, flags);
					} else {
						cdata = zend_ffi_cdata_to_zval_slow_ptr(ptr, type, flags);
					}
				} else {
					GC_ADDREF(&cdata->std);
				}
				ZVAL_OBJ(rv, &cdata->std);
				return;
			default:
				break;
		}
	}

	if (!cdata) {
		if (is_ret) {
			cdata = zend_ffi_cdata_to_zval_slow_ret(ptr, type, flags);
		} else {
			cdata = zend_ffi_cdata_to_zval_slow(ptr, type, flags);
		}
	} else {
		GC_ADDREF(&cdata->std);
	}
	ZVAL_OBJ(rv, &cdata->std);
}
/* }}} */

static uint64_t zend_ffi_bit_field_read(void *ptr, zend_ffi_field *field) /* {{{ */
{
	size_t bit = field->first_bit;
	size_t last_bit = bit + field->bits - 1;
	uint8_t *p = (uint8_t *) ptr + bit / 8;
	uint8_t *last_p = (uint8_t *) ptr + last_bit / 8;
	size_t pos = bit % 8;
	size_t insert_pos = 0;
	uint8_t mask;
	uint64_t val = 0;

	/* Bitfield fits into a single byte */
	if (p == last_p) {
		mask = (1U << field->bits) - 1U;
		return (*p >> pos) & mask;
	}

	/* Read partial prefix byte */
	if (pos != 0) {
		size_t num_bits = 8 - pos;
		mask = (1U << num_bits) - 1U;
		val = (*p++ >> pos) & mask;
		insert_pos += num_bits;
	}

	/* Read full bytes */
	while (p < last_p) {
		val |= (uint64_t) *p++ << insert_pos;
		insert_pos += 8;
	}

	/* Read partial suffix byte */
	if (p == last_p) {
		size_t num_bits = last_bit % 8 + 1;
		mask = (1U << num_bits) - 1U;
		val |= (uint64_t) (*p & mask) << insert_pos;
	}

	return val;
}
/* }}} */

static void zend_ffi_bit_field_to_zval(void *ptr, zend_ffi_field *field, zval *rv) /* {{{ */
{
	uint64_t val = zend_ffi_bit_field_read(ptr, field);
	if (ZEND_FFI_TYPE(field->type)->kind == ZEND_FFI_TYPE_CHAR
	 || ZEND_FFI_TYPE(field->type)->kind == ZEND_FFI_TYPE_SINT8
	 || ZEND_FFI_TYPE(field->type)->kind == ZEND_FFI_TYPE_SINT16
	 || ZEND_FFI_TYPE(field->type)->kind == ZEND_FFI_TYPE_SINT32
	 || ZEND_FFI_TYPE(field->type)->kind == ZEND_FFI_TYPE_SINT64) {
		/* Sign extend */
		uint64_t shift = 64 - (field->bits % 64);
		if (shift != 0) {
			val = (int64_t)(val << shift) >> shift;
		}
	}
	ZVAL_LONG(rv, val);
}
/* }}} */

static void zend_ffi_zval_to_bit_field(void *ptr, zend_ffi_field *field, zval *value) /* {{{ */
{
	uint64_t val = zval_get_long(value);
	size_t bit = field->first_bit;
	size_t last_bit = bit + field->bits - 1;
	uint8_t *p = (uint8_t *) ptr + bit / 8;
	uint8_t *last_p = (uint8_t *) ptr + last_bit / 8;
	size_t pos = bit % 8;
	uint8_t mask;

	/* Bitfield fits into a single byte */
	if (p == last_p) {
		mask = ((1U << field->bits) - 1U) << pos;
		*p = (*p & ~mask) | ((val << pos) & mask);
		return;
	}

	/* Write partial prefix byte */
	if (pos != 0) {
		size_t num_bits = 8 - pos;
		mask = ((1U << num_bits) - 1U) << pos;
		*p = (*p & ~mask) | ((val << pos) & mask);
		p++;
		val >>= num_bits;
	}

	/* Write full bytes */
	while (p < last_p) {
		*p++ = val;
		val >>= 8;
	}

	/* Write partial suffix byte */
	if (p == last_p) {
		size_t num_bits = last_bit % 8 + 1;
		mask = (1U << num_bits) - 1U;
		*p = (*p & ~mask) | (val & mask);
	}
}
/* }}} */

static zend_always_inline zend_result zend_ffi_zval_to_cdata(void *ptr, zend_ffi_type *type, zval *value) /* {{{ */
{
	zend_long lval;
	double dval;
	zend_string *tmp_str;
	zend_string *str;
	zend_ffi_type_kind kind = type->kind;

	/* Pointer type has special handling of CData */
	if (kind != ZEND_FFI_TYPE_POINTER && Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == zend_ffi_cdata_ce) {
		zend_ffi_cdata *cdata = (zend_ffi_cdata*)Z_OBJ_P(value);
		if (zend_ffi_is_compatible_type(type, ZEND_FFI_TYPE(cdata->type)) &&
			type->size == ZEND_FFI_TYPE(cdata->type)->size) {
			memcpy(ptr, cdata->ptr, type->size);
			return SUCCESS;
		}
	}

again:
	switch (kind) {
		case ZEND_FFI_TYPE_FLOAT:
			dval = zval_get_double(value);
			*(float*)ptr = dval;
			break;
		case ZEND_FFI_TYPE_DOUBLE:
			dval = zval_get_double(value);
			*(double*)ptr = dval;
			break;
#ifdef HAVE_LONG_DOUBLE
		case ZEND_FFI_TYPE_LONGDOUBLE:
			dval = zval_get_double(value);
			*(long double*)ptr = dval;
			break;
#endif
		case ZEND_FFI_TYPE_UINT8:
			lval = zval_get_long(value);
			*(uint8_t*)ptr = lval;
			break;
		case ZEND_FFI_TYPE_SINT8:
			lval = zval_get_long(value);
			*(int8_t*)ptr = lval;
			break;
		case ZEND_FFI_TYPE_UINT16:
			lval = zval_get_long(value);
			*(uint16_t*)ptr = lval;
			break;
		case ZEND_FFI_TYPE_SINT16:
			lval = zval_get_long(value);
			*(int16_t*)ptr = lval;
			break;
		case ZEND_FFI_TYPE_UINT32:
			lval = zval_get_long(value);
			*(uint32_t*)ptr = lval;
			break;
		case ZEND_FFI_TYPE_SINT32:
			lval = zval_get_long(value);
			*(int32_t*)ptr = lval;
			break;
		case ZEND_FFI_TYPE_UINT64:
			lval = zval_get_long(value);
			*(uint64_t*)ptr = lval;
			break;
		case ZEND_FFI_TYPE_SINT64:
			lval = zval_get_long(value);
			*(int64_t*)ptr = lval;
			break;
		case ZEND_FFI_TYPE_BOOL:
			*(uint8_t*)ptr = zend_is_true(value);
			break;
		case ZEND_FFI_TYPE_CHAR:
			str = zval_get_tmp_string(value, &tmp_str);
			if (ZSTR_LEN(str) == 1) {
				*(char*)ptr = ZSTR_VAL(str)[0];
			} else {
				zend_ffi_assign_incompatible(value, type);
				return FAILURE;
			}
			zend_tmp_string_release(tmp_str);
			break;
		case ZEND_FFI_TYPE_ENUM:
			kind = type->enumeration.kind;
			goto again;
		case ZEND_FFI_TYPE_POINTER:
			if (Z_TYPE_P(value) == IS_NULL) {
				*(void**)ptr = NULL;
				break;
			} else if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == zend_ffi_cdata_ce) {
				zend_ffi_cdata *cdata = (zend_ffi_cdata*)Z_OBJ_P(value);

				if (zend_ffi_is_compatible_type(type, ZEND_FFI_TYPE(cdata->type))) {
					if (ZEND_FFI_TYPE(cdata->type)->kind == ZEND_FFI_TYPE_POINTER) {
						*(void**)ptr = *(void**)cdata->ptr;
					} else {
						if (cdata->flags & ZEND_FFI_FLAG_OWNED) {
							zend_throw_error(zend_ffi_exception_ce, "Attempt to perform assign of owned C pointer");
							return FAILURE;
						}
						*(void**)ptr = cdata->ptr;
					}
					return SUCCESS;
				/* Allow transparent assignment of not-owned CData to compatible pointers */
				} else if (ZEND_FFI_TYPE(cdata->type)->kind != ZEND_FFI_TYPE_POINTER
				 && zend_ffi_is_compatible_type(ZEND_FFI_TYPE(type->pointer.type), ZEND_FFI_TYPE(cdata->type))) {
					if (cdata->flags & ZEND_FFI_FLAG_OWNED) {
						zend_throw_error(zend_ffi_exception_ce, "Attempt to perform assign pointer to owned C data");
						return FAILURE;
					}
					*(void**)ptr = cdata->ptr;
					return SUCCESS;
				}
#if FFI_CLOSURES
			} else if (ZEND_FFI_TYPE(type->pointer.type)->kind == ZEND_FFI_TYPE_FUNC) {
				void *callback = zend_ffi_create_callback(ZEND_FFI_TYPE(type->pointer.type), value);

				if (callback) {
					*(void**)ptr = callback;
					break;
				} else {
					return FAILURE;
				}
#endif
			}
			zend_ffi_assign_incompatible(value, type);
			return FAILURE;
		case ZEND_FFI_TYPE_STRUCT:
		case ZEND_FFI_TYPE_ARRAY:
		default:
			/* Incompatible types, because otherwise the CData check at the entry point would've succeeded. */
			zend_ffi_assign_incompatible(value, type);
			return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

#if defined(ZEND_WIN32) && (defined(HAVE_FFI_FASTCALL) || defined(HAVE_FFI_STDCALL) || defined(HAVE_FFI_VECTORCALL_PARTIAL))
static size_t zend_ffi_arg_size(zend_ffi_type *type) /* {{{ */
{
	zend_ffi_type *arg_type;
	size_t arg_size = 0;

	ZEND_HASH_PACKED_FOREACH_PTR(type->func.args, arg_type) {
		arg_size += MAX(ZEND_FFI_TYPE(arg_type)->size, sizeof(size_t));
	} ZEND_HASH_FOREACH_END();
	return arg_size;
}
/* }}} */
#endif

static zend_always_inline zend_string *zend_ffi_mangled_func_name(zend_string *name, zend_ffi_type *type) /* {{{ */
{
#ifdef ZEND_WIN32
	switch (type->func.abi) {
# ifdef HAVE_FFI_FASTCALL
		case FFI_FASTCALL:
			return strpprintf(0, "@%s@%zu", ZSTR_VAL(name), zend_ffi_arg_size(type));
# endif
# ifdef HAVE_FFI_STDCALL
		case FFI_STDCALL:
			return strpprintf(0, "_%s@%zu", ZSTR_VAL(name), zend_ffi_arg_size(type));
# endif
# ifdef HAVE_FFI_VECTORCALL_PARTIAL
		case FFI_VECTORCALL_PARTIAL:
			return strpprintf(0, "%s@@%zu", ZSTR_VAL(name), zend_ffi_arg_size(type));
# endif
	}
#endif
	return zend_string_copy(name);
}
/* }}} */

#if FFI_CLOSURES
typedef struct _zend_ffi_callback_data {
	zend_fcall_info_cache  fcc;
	zend_ffi_type         *type;
	void                  *code;
	void                  *callback;
	ffi_cif                cif;
	uint32_t               arg_count;
	ffi_type              *ret_type;
	ffi_type              *arg_types[0] ZEND_ELEMENT_COUNT(arg_count);
} zend_ffi_callback_data;

static void zend_ffi_callback_hash_dtor(zval *zv) /* {{{ */
{
	zend_ffi_callback_data *callback_data = Z_PTR_P(zv);

	ffi_closure_free(callback_data->callback);
	if (callback_data->fcc.function_handler->common.fn_flags & ZEND_ACC_CLOSURE) {
		OBJ_RELEASE(ZEND_CLOSURE_OBJECT(callback_data->fcc.function_handler));
	}
	for (int i = 0; i < callback_data->arg_count; ++i) {
		if (callback_data->arg_types[i]->type == FFI_TYPE_STRUCT) {
			efree(callback_data->arg_types[i]);
		}
	}
	if (callback_data->ret_type->type == FFI_TYPE_STRUCT) {
		efree(callback_data->ret_type);
	}
	efree(callback_data);
}
/* }}} */

static void (*orig_interrupt_function)(zend_execute_data *execute_data);

static void zend_ffi_dispatch_callback_end(void){ /* {{{ */
	//pthread_cond_broadcast(&FFI_G(vm_ack));
}
/* }}} */

static void zend_ffi_dispatch_callback(void){ /* {{{ */
	// this function must always run on the main thread
	//ZEND_ASSERT(pthread_self() == FFI_G(main_tid));

	if (!zend_atomic_bool_load_ex(&FFI_G(callback_in_progress))) {
		return;
	}

	zend_ffi_callback_data *callback_data = FFI_G(callback_data).data;
	zend_fcall_info fci;
	zend_ffi_type *ret_type;
	zval retval;
	ALLOCA_FLAG(use_heap)

	fci.size = sizeof(zend_fcall_info);
	ZVAL_UNDEF(&fci.function_name);
	fci.retval = &retval;
	fci.params = do_alloca(sizeof(zval) *callback_data->arg_count, use_heap);
	fci.object = NULL;
	fci.param_count = callback_data->arg_count;
	fci.named_params = NULL;


	if (callback_data->type->func.args) {
		int n = 0;
		zend_ffi_type *arg_type;

		ZEND_HASH_PACKED_FOREACH_PTR(callback_data->type->func.args, arg_type) {
			arg_type = ZEND_FFI_TYPE(arg_type);
			zend_ffi_cdata_to_zval(NULL, FFI_G(callback_data).args[n], arg_type, BP_VAR_R, &fci.params[n], (zend_ffi_flags)(arg_type->attr & ZEND_FFI_ATTR_CONST), 0, 0);
			n++;
		} ZEND_HASH_FOREACH_END();
	}

	ZVAL_UNDEF(&retval);
	if (zend_call_function(&fci, &callback_data->fcc) != SUCCESS) {
		zend_throw_error(zend_ffi_exception_ce, "Cannot call callback");
	}

	if (callback_data->arg_count) {
		int n = 0;

		for (n = 0; n < callback_data->arg_count; n++) {
			zval_ptr_dtor(&fci.params[n]);
		}
	}
	free_alloca(fci.params, use_heap);

	if (EG(exception)) {
		zend_error_noreturn(E_ERROR, "Throwing from FFI callbacks is not allowed");
	}

	ret_type = ZEND_FFI_TYPE(callback_data->type->func.ret_type);
	if (ret_type->kind != ZEND_FFI_TYPE_VOID) {
		zend_ffi_zval_to_cdata(FFI_G(callback_data).ret, ret_type, &retval);
	}

	zval_ptr_dtor(&retval);
}
/* }}} */

static void zend_ffi_interrupt_function(zend_execute_data *execute_data){ /* {{{ */
	bool is_main_tid = FFI_G(callback_tid) == FFI_G(main_tid);
	if(!is_main_tid){
		pthread_mutex_lock(&FFI_G(vm_response_lock));
		
		FFI_DPRINTF("<-- ACK\n");
		// notify calling thread and release
		pthread_cond_broadcast(&FFI_G(vm_ack));
		
		// release mutex and wait for the unlock signal		
		FFI_DPRINTF("-- wait unlock --\n");
		pthread_cond_wait(&FFI_G(vm_unlock), &FFI_G(vm_response_lock));
		pthread_mutex_unlock(&FFI_G(vm_response_lock));
		FFI_DPRINTF("-- end\n");
	}
	
	if (orig_interrupt_function) {
		orig_interrupt_function(execute_data);
	}
}
/* }}} */

static void zend_ffi_wait_cond(
	pthread_mutex_t *mutex, pthread_cond_t *cond,
	zend_atomic_bool *flag, bool wanted_value, bool release
){  /* {{{ */
	// get lock, first
	pthread_mutex_lock(mutex);
	
	// if we acquired the lock before the request could be serviced
	// unlock it and wait for the flag
	if(flag == NULL){
		pthread_cond_wait(cond, mutex);
	} else {
		while(zend_atomic_bool_load_ex(flag) != wanted_value){
			pthread_cond_wait(cond, mutex);
		}
	}

	if(release){
		pthread_mutex_unlock(mutex);
	}
}
/* }}} */

static void zend_ffi_wait_request_barrier(bool release){ /* {{{ */
	zend_ffi_wait_cond(&FFI_G(vm_request_lock), &FFI_G(vm_unlock), &FFI_G(callback_in_progress), false, release);
}
/* }}} */

static void zend_ffi_callback_trampoline(ffi_cif* cif, void* ret, void** args, void* data) /* {{{ */
{
	// wait for a previously initiated request to complete
	zend_ffi_wait_request_barrier(false);
	{
		// mutex is now locked, and request is not pending.
		// start a new one
		zend_atomic_bool_store_ex(&FFI_G(callback_in_progress), true);

		zend_ffi_call_data call_data = {
			.cif = cif,
			.ret = ret,
			.args = args,
			.data = (zend_ffi_callback_data *)data
		};
		FFI_G(callback_data) = call_data;

		FFI_G(callback_tid) = pthread_self();
		bool is_main_thread = FFI_G(callback_tid) == FFI_G(main_tid);
		
		if(!is_main_thread){
			// post interrupt request to synchronize with the main thread
			zend_atomic_bool_store_ex(&EG(vm_interrupt), true);
			
			// wait for the ack, keep the lock
			//zend_ffi_wait_cond(&FFI_G(vm_response_lock), &FFI_G(vm_ack), &FFI_G(callback_in_progress), true, false);

			pthread_mutex_lock(&FFI_G(vm_response_lock));
			pthread_cond_wait(&FFI_G(vm_ack), &FFI_G(vm_response_lock));
		}

		// dispatch the callback
		FFI_DPRINTF("dispatch from %ld, is_main=%d", pthread_self(), pthread_self() == FFI_G(main_tid));
		zend_ffi_dispatch_callback();
		FFI_DPRINTF("done\n");

		if(!is_main_thread){
			// unlock interrupt handler
			zend_atomic_bool_store_ex(&FFI_G(callback_in_progress), false);
			FFI_DPRINTF("--> unlock\n");
			pthread_cond_broadcast(&FFI_G(vm_unlock));
			pthread_mutex_unlock(&FFI_G(vm_response_lock));
		}
	}
	pthread_mutex_unlock(&FFI_G(vm_request_lock));
}
/* }}} */

static void *zend_ffi_create_callback(zend_ffi_type *type, zval *value) /* {{{ */
{
	zend_fcall_info_cache fcc;
	char *error = NULL;
	uint32_t arg_count;
	void *code;
	void *callback;
	zend_ffi_callback_data *callback_data;

	if (type->attr & ZEND_FFI_ATTR_VARIADIC) {
		zend_throw_error(zend_ffi_exception_ce, "Variadic function closures are not supported");
		return NULL;
	}

	if (!zend_is_callable_ex(value, NULL, 0, NULL, &fcc, &error)) {
		zend_throw_error(zend_ffi_exception_ce, "Attempt to assign an invalid callback, %s", error);
		return NULL;
	}

	arg_count = type->func.args ? zend_hash_num_elements(type->func.args) : 0;
	if (arg_count < fcc.function_handler->common.required_num_args) {
		zend_throw_error(zend_ffi_exception_ce, "Attempt to assign an invalid callback, insufficient number of arguments");
		return NULL;
	}

	callback = ffi_closure_alloc(sizeof(ffi_closure), &code);
	if (!callback) {
		zend_throw_error(zend_ffi_exception_ce, "Cannot allocate callback");
		return NULL;
	}

	callback_data = emalloc(sizeof(zend_ffi_callback_data) + sizeof(ffi_type*) * arg_count);
	memcpy(&callback_data->fcc, &fcc, sizeof(zend_fcall_info_cache));
	callback_data->type = type;
	callback_data->callback = callback;
	callback_data->code = code;
	callback_data->arg_count = arg_count;

	if (type->func.args) {
		int n = 0;
		zend_ffi_type *arg_type;

		ZEND_HASH_PACKED_FOREACH_PTR(type->func.args, arg_type) {
			arg_type = ZEND_FFI_TYPE(arg_type);
			callback_data->arg_types[n] = zend_ffi_get_type(arg_type);
			if (!callback_data->arg_types[n]) {
				zend_ffi_pass_unsupported(arg_type);
				for (int i = 0; i < n; ++i) {
					if (callback_data->arg_types[i]->type == FFI_TYPE_STRUCT) {
						efree(callback_data->arg_types[i]);
					}
				}
				efree(callback_data);
				ffi_closure_free(callback);
				return NULL;
			}
			n++;
		} ZEND_HASH_FOREACH_END();
	}
	callback_data->ret_type = zend_ffi_get_type(ZEND_FFI_TYPE(type->func.ret_type));
	if (!callback_data->ret_type) {
		zend_ffi_return_unsupported(type->func.ret_type);
		for (int i = 0; i < callback_data->arg_count; ++i) {
			if (callback_data->arg_types[i]->type == FFI_TYPE_STRUCT) {
				efree(callback_data->arg_types[i]);
			}
		}
		efree(callback_data);
		ffi_closure_free(callback);
		return NULL;
	}

	if (ffi_prep_cif(&callback_data->cif, type->func.abi, callback_data->arg_count, callback_data->ret_type, callback_data->arg_types) != FFI_OK) {
		zend_throw_error(zend_ffi_exception_ce, "Cannot prepare callback CIF");
		goto free_on_failure;
	}

	if (ffi_prep_closure_loc(callback, &callback_data->cif, zend_ffi_callback_trampoline, callback_data, code) != FFI_OK) {
		zend_throw_error(zend_ffi_exception_ce, "Cannot prepare callback");
free_on_failure: ;
		for (int i = 0; i < callback_data->arg_count; ++i) {
			if (callback_data->arg_types[i]->type == FFI_TYPE_STRUCT) {
				efree(callback_data->arg_types[i]);
			}
		}
		if (callback_data->ret_type->type == FFI_TYPE_STRUCT) {
			efree(callback_data->ret_type);
		}
		efree(callback_data);
		ffi_closure_free(callback);
		return NULL;
	}

	if (!FFI_G(callbacks)) {
		FFI_G(callbacks) = emalloc(sizeof(HashTable));
		zend_hash_init(FFI_G(callbacks), 0, NULL, zend_ffi_callback_hash_dtor, 0);
	}
	zend_hash_next_index_insert_ptr(FFI_G(callbacks), callback_data);

	if (fcc.function_handler->common.fn_flags & ZEND_ACC_CLOSURE) {
		GC_ADDREF(ZEND_CLOSURE_OBJECT(fcc.function_handler));
	}

	return code;
}
/* }}} */
#endif

static zval *zend_ffi_cdata_get(zend_object *obj, zend_string *member, int read_type, void **cache_slot, zval *rv) /* {{{ */
{
	zend_ffi_cdata *cdata = (zend_ffi_cdata*)obj;
	zend_ffi_type  *type = ZEND_FFI_TYPE(cdata->type);

#if 0
	if (UNEXPECTED(!cdata->ptr)) {
		zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
		return &EG(uninitialized_zval);
	}
#endif

	if (UNEXPECTED(!zend_string_equals_literal(member, "cdata"))) {
		zend_throw_error(zend_ffi_exception_ce, "Only 'cdata' property may be read");
		return &EG(uninitialized_zval);
	}

	zend_ffi_cdata_to_zval(cdata, cdata->ptr, type, BP_VAR_R, rv, 0, 0, 0);
	return rv;
}
/* }}} */

static zval *zend_ffi_cdata_set(zend_object *obj, zend_string *member, zval *value, void **cache_slot) /* {{{ */
{
	zend_ffi_cdata *cdata = (zend_ffi_cdata*)obj;
	zend_ffi_type  *type = ZEND_FFI_TYPE(cdata->type);

#if 0
	if (UNEXPECTED(!cdata->ptr)) {
		zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
		return &EG(uninitialized_zval);
	}
#endif

	if (UNEXPECTED(!zend_string_equals_literal(member, "cdata"))) {
		zend_throw_error(zend_ffi_exception_ce, "Only 'cdata' property may be set");
		return &EG(uninitialized_zval);
	}

	zend_ffi_zval_to_cdata(cdata->ptr, type, value);

	return value;
}
/* }}} */

static zend_result zend_ffi_cdata_cast_object(zend_object *readobj, zval *writeobj, int type) /* {{{ */
{
	if (type == IS_STRING) {
		zend_ffi_cdata *cdata = (zend_ffi_cdata*)readobj;
		zend_ffi_type  *ctype = ZEND_FFI_TYPE(cdata->type);
		void           *ptr = cdata->ptr;
		zend_ffi_type_kind kind = ctype->kind;

again:
	    switch (kind) {
			case ZEND_FFI_TYPE_FLOAT:
				ZVAL_DOUBLE(writeobj, *(float*)ptr);
				break;
			case ZEND_FFI_TYPE_DOUBLE:
				ZVAL_DOUBLE(writeobj, *(double*)ptr);
				break;
#ifdef HAVE_LONG_DOUBLE
			case ZEND_FFI_TYPE_LONGDOUBLE:
				ZVAL_DOUBLE(writeobj, *(long double*)ptr);
				break;
#endif
			case ZEND_FFI_TYPE_UINT8:
				ZVAL_LONG(writeobj, *(uint8_t*)ptr);
				break;
			case ZEND_FFI_TYPE_SINT8:
				ZVAL_LONG(writeobj, *(int8_t*)ptr);
				break;
			case ZEND_FFI_TYPE_UINT16:
				ZVAL_LONG(writeobj, *(uint16_t*)ptr);
				break;
			case ZEND_FFI_TYPE_SINT16:
				ZVAL_LONG(writeobj, *(int16_t*)ptr);
				break;
			case ZEND_FFI_TYPE_UINT32:
				ZVAL_LONG(writeobj, *(uint32_t*)ptr);
				break;
			case ZEND_FFI_TYPE_SINT32:
				ZVAL_LONG(writeobj, *(int32_t*)ptr);
				break;
			case ZEND_FFI_TYPE_UINT64:
				ZVAL_LONG(writeobj, *(uint64_t*)ptr);
				break;
			case ZEND_FFI_TYPE_SINT64:
				ZVAL_LONG(writeobj, *(int64_t*)ptr);
				break;
			case ZEND_FFI_TYPE_BOOL:
				ZVAL_BOOL(writeobj, *(uint8_t*)ptr);
				break;
			case ZEND_FFI_TYPE_CHAR:
				ZVAL_CHAR(writeobj, *(char*)ptr);
				return SUCCESS;
			case ZEND_FFI_TYPE_ENUM:
				kind = ctype->enumeration.kind;
				goto again;
			case ZEND_FFI_TYPE_POINTER:
				if (*(void**)ptr == NULL) {
					ZVAL_NULL(writeobj);
					break;
				} else if ((ctype->attr & ZEND_FFI_ATTR_CONST) && ZEND_FFI_TYPE(ctype->pointer.type)->kind == ZEND_FFI_TYPE_CHAR) {
					ZVAL_STRING(writeobj, *(char**)ptr);
					return SUCCESS;
				}
				return FAILURE;
			default:
				return FAILURE;
		}
		convert_to_string(writeobj);
		return SUCCESS;
	} else if (type == _IS_BOOL) {
		ZVAL_TRUE(writeobj);
		return SUCCESS;
	}

	return FAILURE;
}
/* }}} */

static zval *zend_ffi_cdata_read_field(zend_object *obj, zend_string *field_name, int read_type, void **cache_slot, zval *rv) /* {{{ */
{
	zend_ffi_cdata *cdata = (zend_ffi_cdata*)obj;
	zend_ffi_type  *type = ZEND_FFI_TYPE(cdata->type);
	void           *ptr = cdata->ptr;
	zend_ffi_field *field;

	if (cache_slot && *cache_slot == type) {
		field = *(cache_slot + 1);
	} else {
		if (type->kind == ZEND_FFI_TYPE_POINTER) {
			type = ZEND_FFI_TYPE(type->pointer.type);
		}
		if (UNEXPECTED(type->kind != ZEND_FFI_TYPE_STRUCT)) {
			if (UNEXPECTED(type->kind != ZEND_FFI_TYPE_STRUCT)) {
				zend_throw_error(zend_ffi_exception_ce, "Attempt to read field '%s' of non C struct/union", ZSTR_VAL(field_name));
				return &EG(uninitialized_zval);
			}
		}

		field = zend_hash_find_ptr(&type->record.fields, field_name);
		if (UNEXPECTED(!field)) {
			zend_throw_error(zend_ffi_exception_ce, "Attempt to read undefined field '%s' of C struct/union", ZSTR_VAL(field_name));
			return &EG(uninitialized_zval);
		}

		if (cache_slot) {
			*cache_slot = type;
			*(cache_slot + 1) = field;
		}
	}

	if (ZEND_FFI_TYPE(cdata->type)->kind == ZEND_FFI_TYPE_POINTER) {
		/* transparently dereference the pointer */
		if (UNEXPECTED(!ptr)) {
			zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
			return &EG(uninitialized_zval);
		}
		ptr = (void*)(*(char**)ptr);
		if (UNEXPECTED(!ptr)) {
			zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
			return &EG(uninitialized_zval);
		}
		type = ZEND_FFI_TYPE(type->pointer.type);
	}

#if 0
	if (UNEXPECTED(!ptr)) {
		zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
		return &EG(uninitialized_zval);
	}
#endif

	if (EXPECTED(!field->bits)) {
		zend_ffi_type *field_type = field->type;

		if (ZEND_FFI_TYPE_IS_OWNED(field_type)) {
			field_type = ZEND_FFI_TYPE(field_type);
			if (!(field_type->attr & ZEND_FFI_ATTR_STORED)
			 && field_type->kind == ZEND_FFI_TYPE_POINTER) {
				field->type = field_type = zend_ffi_remember_type(field_type);
			}
		}
		ptr = (void*)(((char*)ptr) + field->offset);
		zend_ffi_cdata_to_zval(NULL, ptr, field_type, read_type, rv, (cdata->flags & ZEND_FFI_FLAG_CONST) | (zend_ffi_flags)field->is_const, 0, 0);
	} else {
		zend_ffi_bit_field_to_zval(ptr, field, rv);
	}

	return rv;
}
/* }}} */

static zval *zend_ffi_cdata_write_field(zend_object *obj, zend_string *field_name, zval *value, void **cache_slot) /* {{{ */
{
	zend_ffi_cdata *cdata = (zend_ffi_cdata*)obj;
	zend_ffi_type  *type = ZEND_FFI_TYPE(cdata->type);
	void           *ptr = cdata->ptr;
	zend_ffi_field *field;

	if (cache_slot && *cache_slot == type) {
		field = *(cache_slot + 1);
	} else {
		if (type->kind == ZEND_FFI_TYPE_POINTER) {
			type = ZEND_FFI_TYPE(type->pointer.type);
		}
		if (UNEXPECTED(type->kind != ZEND_FFI_TYPE_STRUCT)) {
			if (UNEXPECTED(type->kind != ZEND_FFI_TYPE_STRUCT)) {
				zend_throw_error(zend_ffi_exception_ce, "Attempt to assign field '%s' of non C struct/union", ZSTR_VAL(field_name));
				return value;
			}
		}

		field = zend_hash_find_ptr(&type->record.fields, field_name);
		if (UNEXPECTED(!field)) {
			zend_throw_error(zend_ffi_exception_ce, "Attempt to assign undefined field '%s' of C struct/union", ZSTR_VAL(field_name));
			return value;
		}

		if (cache_slot) {
			*cache_slot = type;
			*(cache_slot + 1) = field;
		}
	}

	if (ZEND_FFI_TYPE(cdata->type)->kind == ZEND_FFI_TYPE_POINTER) {
		/* transparently dereference the pointer */
		if (UNEXPECTED(!ptr)) {
			zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
			return value;
		}
		ptr = (void*)(*(char**)ptr);
		if (UNEXPECTED(!ptr)) {
			zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
			return value;
		}
	}

#if 0
	if (UNEXPECTED(!ptr)) {
		zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
		return value;
	}
#endif

	if (UNEXPECTED(cdata->flags & ZEND_FFI_FLAG_CONST)) {
		zend_throw_error(zend_ffi_exception_ce, "Attempt to assign read-only location");
		return value;
	} else if (UNEXPECTED(field->is_const)) {
		zend_throw_error(zend_ffi_exception_ce, "Attempt to assign read-only field '%s'", ZSTR_VAL(field_name));
		return value;
	}

	if (EXPECTED(!field->bits)) {
		ptr = (void*)(((char*)ptr) + field->offset);
		zend_ffi_zval_to_cdata(ptr, ZEND_FFI_TYPE(field->type), value);
	} else {
		zend_ffi_zval_to_bit_field(ptr, field, value);
	}
	return value;
}
/* }}} */

static zval *zend_ffi_cdata_read_dim(zend_object *obj, zval *offset, int read_type, zval *rv) /* {{{ */
{
	zend_ffi_cdata *cdata = (zend_ffi_cdata*)obj;
	zend_ffi_type  *type = ZEND_FFI_TYPE(cdata->type);
	zend_long       dim = zval_get_long(offset);
	zend_ffi_type  *dim_type;
	void           *ptr;
	zend_ffi_flags  is_const;

	if (EXPECTED(type->kind == ZEND_FFI_TYPE_ARRAY)) {
		if (UNEXPECTED((zend_ulong)(dim) >= (zend_ulong)type->array.length)
		 && (UNEXPECTED(dim < 0) || UNEXPECTED(type->array.length != 0))) {
			zend_throw_error(zend_ffi_exception_ce, "C array index out of bounds");
			return &EG(uninitialized_zval);
		}

		is_const = (cdata->flags & ZEND_FFI_FLAG_CONST) | (zend_ffi_flags)(type->attr & ZEND_FFI_ATTR_CONST);

		dim_type = type->array.type;
		if (ZEND_FFI_TYPE_IS_OWNED(dim_type)) {
			dim_type = ZEND_FFI_TYPE(dim_type);
			if (!(dim_type->attr & ZEND_FFI_ATTR_STORED)
			 && dim_type->kind == ZEND_FFI_TYPE_POINTER) {
				type->array.type = dim_type = zend_ffi_remember_type(dim_type);
			}
		}
#if 0
		if (UNEXPECTED(!cdata->ptr)) {
			zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
			return &EG(uninitialized_zval);
		}
#endif
		ptr = (void*)(((char*)cdata->ptr) + dim_type->size * dim);
	} else if (EXPECTED(type->kind == ZEND_FFI_TYPE_POINTER)) {
		is_const = (cdata->flags & ZEND_FFI_FLAG_CONST) | (zend_ffi_flags)(type->attr & ZEND_FFI_ATTR_CONST);
		dim_type = type->pointer.type;
		if (ZEND_FFI_TYPE_IS_OWNED(dim_type)) {
			dim_type = ZEND_FFI_TYPE(dim_type);
			if (!(dim_type->attr & ZEND_FFI_ATTR_STORED)
			 && dim_type->kind == ZEND_FFI_TYPE_POINTER) {
				type->pointer.type = dim_type = zend_ffi_remember_type(dim_type);
			}
		}
		if (UNEXPECTED(!cdata->ptr)) {
			zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
			return &EG(uninitialized_zval);
		}
		ptr = (void*)((*(char**)cdata->ptr) + dim_type->size * dim);
	} else {
		zend_throw_error(zend_ffi_exception_ce, "Attempt to read element of non C array");
		return &EG(uninitialized_zval);
	}

	zend_ffi_cdata_to_zval(NULL, ptr, dim_type, read_type, rv, is_const, 0, 0);
	return rv;
}
/* }}} */

static void zend_ffi_cdata_write_dim(zend_object *obj, zval *offset, zval *value) /* {{{ */
{
	zend_ffi_cdata *cdata = (zend_ffi_cdata*)obj;
	zend_ffi_type  *type = ZEND_FFI_TYPE(cdata->type);
	zend_long       dim;
	void           *ptr;
	zend_ffi_flags  is_const;

	if (offset == NULL) {
		zend_throw_error(zend_ffi_exception_ce, "Cannot add next element to object of type FFI\\CData");
		return;
	}

	dim = zval_get_long(offset);
	if (EXPECTED(type->kind == ZEND_FFI_TYPE_ARRAY)) {
		if (UNEXPECTED((zend_ulong)(dim) >= (zend_ulong)type->array.length)
		 && (UNEXPECTED(dim < 0) || UNEXPECTED(type->array.length != 0))) {
			zend_throw_error(zend_ffi_exception_ce, "C array index out of bounds");
			return;
		}

		is_const = (cdata->flags & ZEND_FFI_FLAG_CONST) | (zend_ffi_flags)(type->attr & ZEND_FFI_ATTR_CONST);
		type = ZEND_FFI_TYPE(type->array.type);
#if 0
		if (UNEXPECTED(!cdata->ptr)) {
			zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
			return;
		}
#endif
		ptr = (void*)(((char*)cdata->ptr) + type->size * dim);
	} else if (EXPECTED(type->kind == ZEND_FFI_TYPE_POINTER)) {
		is_const = (cdata->flags & ZEND_FFI_FLAG_CONST) | (zend_ffi_flags)(type->attr & ZEND_FFI_ATTR_CONST);
		type = ZEND_FFI_TYPE(type->pointer.type);
		if (UNEXPECTED(!cdata->ptr)) {
			zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
			return;
		}
		ptr = (void*)((*(char**)cdata->ptr) + type->size * dim);
	} else {
		zend_throw_error(zend_ffi_exception_ce, "Attempt to assign element of non C array");
		return;
	}

	if (UNEXPECTED(is_const)) {
		zend_throw_error(zend_ffi_exception_ce, "Attempt to assign read-only location");
		return;
	}

	zend_ffi_zval_to_cdata(ptr, type, value);
}
/* }}} */

#define MAX_TYPE_NAME_LEN 256

typedef struct _zend_ffi_ctype_name_buf {
	char *start;
	char *end;
	char buf[MAX_TYPE_NAME_LEN];
} zend_ffi_ctype_name_buf;

static bool zend_ffi_ctype_name_prepend(zend_ffi_ctype_name_buf *buf, const char *str, size_t len) /* {{{ */
{
	buf->start -= len;
	if (buf->start < buf->buf) {
		return 0;
	}
	memcpy(buf->start, str, len);
	return 1;
}
/* }}} */

static bool zend_ffi_ctype_name_append(zend_ffi_ctype_name_buf *buf, const char *str, size_t len) /* {{{ */
{
	if (buf->end + len > buf->buf + MAX_TYPE_NAME_LEN) {
		return 0;
	}
	memcpy(buf->end, str, len);
	buf->end += len;
	return 1;
}
/* }}} */

static bool zend_ffi_ctype_name(zend_ffi_ctype_name_buf *buf, const zend_ffi_type *type) /* {{{ */
{
	const char *name = NULL;
	bool is_ptr = 0;

	while (1) {
		switch (type->kind) {
			case ZEND_FFI_TYPE_VOID:
				name = "void";
				break;
			case ZEND_FFI_TYPE_FLOAT:
				name = "float";
				break;
			case ZEND_FFI_TYPE_DOUBLE:
				name = "double";
				break;
#ifdef HAVE_LONG_DOUBLE
			case ZEND_FFI_TYPE_LONGDOUBLE:
				name = "long double";
				break;
#endif
			case ZEND_FFI_TYPE_UINT8:
				name = "uint8_t";
				break;
			case ZEND_FFI_TYPE_SINT8:
				name = "int8_t";
				break;
			case ZEND_FFI_TYPE_UINT16:
				name = "uint16_t";
				break;
			case ZEND_FFI_TYPE_SINT16:
				name = "int16_t";
				break;
			case ZEND_FFI_TYPE_UINT32:
				name = "uint32_t";
				break;
			case ZEND_FFI_TYPE_SINT32:
				name = "int32_t";
				break;
			case ZEND_FFI_TYPE_UINT64:
				name = "uint64_t";
				break;
			case ZEND_FFI_TYPE_SINT64:
				name = "int64_t";
				break;
			case ZEND_FFI_TYPE_ENUM:
				if (type->enumeration.tag_name) {
					zend_ffi_ctype_name_prepend(buf, ZSTR_VAL(type->enumeration.tag_name), ZSTR_LEN(type->enumeration.tag_name));
				} else {
					zend_ffi_ctype_name_prepend(buf, "<anonymous>", sizeof("<anonymous>")-1);
				}
				name = "enum ";
				break;
			case ZEND_FFI_TYPE_BOOL:
				name = "bool";
				break;
			case ZEND_FFI_TYPE_CHAR:
				name = "char";
				break;
			case ZEND_FFI_TYPE_POINTER:
				if (!zend_ffi_ctype_name_prepend(buf, "*", 1)) {
					return 0;
				}
				is_ptr = 1;
				type = ZEND_FFI_TYPE(type->pointer.type);
				break;
			case ZEND_FFI_TYPE_FUNC:
				if (is_ptr) {
					is_ptr = 0;
					if (!zend_ffi_ctype_name_prepend(buf, "(", 1)
					 || !zend_ffi_ctype_name_append(buf, ")", 1)) {
						return 0;
					}
				}
				if (!zend_ffi_ctype_name_append(buf, "(", 1)
				 || !zend_ffi_ctype_name_append(buf, ")", 1)) {
					return 0;
				}
				type = ZEND_FFI_TYPE(type->func.ret_type);
				break;
			case ZEND_FFI_TYPE_ARRAY:
				if (is_ptr) {
					is_ptr = 0;
					if (!zend_ffi_ctype_name_prepend(buf, "(", 1)
					 || !zend_ffi_ctype_name_append(buf, ")", 1)) {
						return 0;
					}
				}
				if (!zend_ffi_ctype_name_append(buf, "[", 1)) {
					return 0;
				}
				if (type->attr & ZEND_FFI_ATTR_VLA) {
					if (!zend_ffi_ctype_name_append(buf, "*", 1)) {
						return 0;
					}
				} else if (!(type->attr & ZEND_FFI_ATTR_INCOMPLETE_ARRAY)) {
					char str[MAX_LENGTH_OF_LONG + 1];
					char *s = zend_print_long_to_buf(str + sizeof(str) - 1, type->array.length);

					if (!zend_ffi_ctype_name_append(buf, s, strlen(s))) {
						return 0;
					}
				}
				if (!zend_ffi_ctype_name_append(buf, "]", 1)) {
					return 0;
				}
				type = ZEND_FFI_TYPE(type->array.type);
				break;
			case ZEND_FFI_TYPE_STRUCT:
				if (type->attr & ZEND_FFI_ATTR_UNION) {
					if (type->record.tag_name) {
						zend_ffi_ctype_name_prepend(buf, ZSTR_VAL(type->record.tag_name), ZSTR_LEN(type->record.tag_name));
					} else {
						zend_ffi_ctype_name_prepend(buf, "<anonymous>", sizeof("<anonymous>")-1);
					}
					name = "union ";
				} else {
					if (type->record.tag_name) {
						zend_ffi_ctype_name_prepend(buf, ZSTR_VAL(type->record.tag_name), ZSTR_LEN(type->record.tag_name));
					} else {
						zend_ffi_ctype_name_prepend(buf, "<anonymous>", sizeof("<anonymous>")-1);
					}
					name = "struct ";
				}
				break;
			default:
				ZEND_UNREACHABLE();
		}
		if (name) {
			break;
		}
	}

//	if (buf->start != buf->end && *buf->start != '[') {
//		if (!zend_ffi_ctype_name_prepend(buf, " ", 1)) {
//			return 0;
//		}
//	}
	return zend_ffi_ctype_name_prepend(buf, name, strlen(name));
}
/* }}} */

static ZEND_COLD void zend_ffi_return_unsupported(zend_ffi_type *type) /* {{{ */
{
	type = ZEND_FFI_TYPE(type);
	if (type->kind == ZEND_FFI_TYPE_STRUCT) {
		zend_throw_error(zend_ffi_exception_ce, "FFI return struct/union is not implemented");
	} else if (type->kind == ZEND_FFI_TYPE_ARRAY) {
		zend_throw_error(zend_ffi_exception_ce, "FFI return array is not implemented");
	} else {
		zend_throw_error(zend_ffi_exception_ce, "FFI internal error. Unsupported return type");
	}
}
/* }}} */

static ZEND_COLD void zend_ffi_pass_unsupported(zend_ffi_type *type) /* {{{ */
{
	type = ZEND_FFI_TYPE(type);
	if (type->kind == ZEND_FFI_TYPE_STRUCT) {
		zend_throw_error(zend_ffi_exception_ce, "FFI passing struct/union is not implemented");
	} else if (type->kind == ZEND_FFI_TYPE_ARRAY) {
		zend_throw_error(zend_ffi_exception_ce, "FFI passing array is not implemented");
	} else {
		zend_throw_error(zend_ffi_exception_ce, "FFI internal error. Unsupported parameter type");
	}
}
/* }}} */

static ZEND_COLD void zend_ffi_pass_incompatible(zval *arg, zend_ffi_type *type, uint32_t n, zend_execute_data *execute_data) /* {{{ */
{
	zend_ffi_ctype_name_buf buf1, buf2;

	buf1.start = buf1.end = buf1.buf + ((MAX_TYPE_NAME_LEN * 3) / 4);
	if (!zend_ffi_ctype_name(&buf1, type)) {
		zend_throw_error(zend_ffi_exception_ce, "Passing incompatible argument %d of C function '%s'", n + 1, ZSTR_VAL(EX(func)->internal_function.function_name));
	} else {
		*buf1.end = 0;
		if (Z_TYPE_P(arg) == IS_OBJECT && Z_OBJCE_P(arg) == zend_ffi_cdata_ce) {
			zend_ffi_cdata *cdata = (zend_ffi_cdata*)Z_OBJ_P(arg);

			type = ZEND_FFI_TYPE(cdata->type);
			buf2.start = buf2.end = buf2.buf + ((MAX_TYPE_NAME_LEN * 3) / 4);
			if (!zend_ffi_ctype_name(&buf2, type)) {
				zend_throw_error(zend_ffi_exception_ce, "Passing incompatible argument %d of C function '%s', expecting '%s'", n + 1, ZSTR_VAL(EX(func)->internal_function.function_name), buf1.start);
			} else {
				*buf2.end = 0;
				zend_throw_error(zend_ffi_exception_ce, "Passing incompatible argument %d of C function '%s', expecting '%s', found '%s'", n + 1, ZSTR_VAL(EX(func)->internal_function.function_name), buf1.start, buf2.start);
			}
		} else {
			zend_throw_error(zend_ffi_exception_ce, "Passing incompatible argument %d of C function '%s', expecting '%s', found PHP '%s'", n + 1, ZSTR_VAL(EX(func)->internal_function.function_name), buf1.start, zend_zval_value_name(arg));
		}
	}
}
/* }}} */

static ZEND_COLD void zend_ffi_assign_incompatible(zval *arg, zend_ffi_type *type) /* {{{ */
{
	zend_ffi_ctype_name_buf buf1, buf2;

	buf1.start = buf1.end = buf1.buf + ((MAX_TYPE_NAME_LEN * 3) / 4);
	if (!zend_ffi_ctype_name(&buf1, type)) {
		zend_throw_error(zend_ffi_exception_ce, "Incompatible types when assigning");
	} else {
		*buf1.end = 0;
		if (Z_TYPE_P(arg) == IS_OBJECT && Z_OBJCE_P(arg) == zend_ffi_cdata_ce) {
			zend_ffi_cdata *cdata = (zend_ffi_cdata*)Z_OBJ_P(arg);

			type = ZEND_FFI_TYPE(cdata->type);
			buf2.start = buf2.end = buf2.buf + ((MAX_TYPE_NAME_LEN * 3) / 4);
			if (!zend_ffi_ctype_name(&buf2, type)) {
				zend_throw_error(zend_ffi_exception_ce, "Incompatible types when assigning to type '%s'", buf1.start);
			} else {
				*buf2.end = 0;
				zend_throw_error(zend_ffi_exception_ce, "Incompatible types when assigning to type '%s' from type '%s'", buf1.start, buf2.start);
			}
		} else {
			zend_throw_error(zend_ffi_exception_ce, "Incompatible types when assigning to type '%s' from PHP '%s'", buf1.start, zend_zval_value_name(arg));
		}
	}
}
/* }}} */

static zend_string *zend_ffi_get_class_name(zend_string *prefix, const zend_ffi_type *type) /* {{{ */
{
	zend_ffi_ctype_name_buf buf;

	buf.start = buf.end = buf.buf + ((MAX_TYPE_NAME_LEN * 3) / 4);
	if (!zend_ffi_ctype_name(&buf, type)) {
		return zend_string_copy(prefix);
	} else {
		return zend_string_concat3(
			ZSTR_VAL(prefix), ZSTR_LEN(prefix), ":", 1, buf.start, buf.end - buf.start);
	}
}
/* }}} */

static zend_string *zend_ffi_cdata_get_class_name(const zend_object *zobj) /* {{{ */
{
	zend_ffi_cdata *cdata = (zend_ffi_cdata*)zobj;

	return zend_ffi_get_class_name(zobj->ce->name, ZEND_FFI_TYPE(cdata->type));
}
/* }}} */

static int zend_ffi_cdata_compare_objects(zval *o1, zval *o2) /* {{{ */
{
	if (Z_TYPE_P(o1) == IS_OBJECT && Z_OBJCE_P(o1) == zend_ffi_cdata_ce &&
	    Z_TYPE_P(o2) == IS_OBJECT && Z_OBJCE_P(o2) == zend_ffi_cdata_ce) {
		zend_ffi_cdata *cdata1 = (zend_ffi_cdata*)Z_OBJ_P(o1);
		zend_ffi_cdata *cdata2 = (zend_ffi_cdata*)Z_OBJ_P(o2);
		zend_ffi_type *type1 = ZEND_FFI_TYPE(cdata1->type);
		zend_ffi_type *type2 = ZEND_FFI_TYPE(cdata2->type);

		if (type1->kind == ZEND_FFI_TYPE_POINTER && type2->kind == ZEND_FFI_TYPE_POINTER) {
			void *ptr1 = *(void**)cdata1->ptr;
			void *ptr2 = *(void**)cdata2->ptr;

			if (!ptr1 || !ptr2) {
				zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
				return 0;
			}
			return ptr1 == ptr2 ? 0 : (ptr1 < ptr2 ? -1 : 1);
		}
	}
	zend_throw_error(zend_ffi_exception_ce, "Comparison of incompatible C types");
	return 0;
}
/* }}} */

static zend_result zend_ffi_cdata_count_elements(zend_object *obj, zend_long *count) /* {{{ */
{
	zend_ffi_cdata *cdata = (zend_ffi_cdata*)obj;
	zend_ffi_type  *type = ZEND_FFI_TYPE(cdata->type);

	if (type->kind != ZEND_FFI_TYPE_ARRAY) {
		zend_throw_error(zend_ffi_exception_ce, "Attempt to count() on non C array");
		return FAILURE;
	} else {
		*count = type->array.length;
		return SUCCESS;
	}
}
/* }}} */

static zend_object* zend_ffi_add(zend_ffi_cdata *base_cdata, zend_ffi_type *base_type, zend_long offset) /* {{{ */
{
	char *ptr;
	zend_ffi_type *ptr_type;
	zend_ffi_cdata *cdata =
		(zend_ffi_cdata*)zend_ffi_cdata_new(zend_ffi_cdata_ce);

	if (base_type->kind == ZEND_FFI_TYPE_POINTER) {
		if (ZEND_FFI_TYPE_IS_OWNED(base_cdata->type)) {
			if (!(base_type->attr & ZEND_FFI_ATTR_STORED)) {
				if (GC_REFCOUNT(&base_cdata->std) == 1) {
					/* transfer type ownership */
					base_cdata->type = base_type;
					base_type = ZEND_FFI_TYPE_MAKE_OWNED(base_type);
				} else {
					base_cdata->type = base_type = zend_ffi_remember_type(base_type);
				}
			}
		}
		cdata->type = base_type;
		ptr = (char*)(*(void**)base_cdata->ptr);
		ptr_type = ZEND_FFI_TYPE(base_type)->pointer.type;
	} else {
		zend_ffi_type *new_type = emalloc(sizeof(zend_ffi_type));

		new_type->kind = ZEND_FFI_TYPE_POINTER;
		new_type->attr = 0;
		new_type->size = sizeof(void*);
		new_type->align = _Alignof(void*);

		ptr_type = base_type->array.type;
		if (ZEND_FFI_TYPE_IS_OWNED(ptr_type)) {
			ptr_type = ZEND_FFI_TYPE(ptr_type);
			if (!(ptr_type->attr & ZEND_FFI_ATTR_STORED)) {
				if (GC_REFCOUNT(&base_cdata->std) == 1) {
					/* transfer type ownership */
					base_type->array.type = ptr_type;
					ptr_type = ZEND_FFI_TYPE_MAKE_OWNED(ptr_type);
				} else {
					base_type->array.type = ptr_type = zend_ffi_remember_type(ptr_type);
				}
			}
		}
		new_type->pointer.type = ptr_type;

		cdata->type = ZEND_FFI_TYPE_MAKE_OWNED(new_type);
		ptr = (char*)base_cdata->ptr;
	}
	cdata->ptr = &cdata->ptr_holder;
	cdata->ptr_holder = ptr +
		(ptrdiff_t) (offset * ZEND_FFI_TYPE(ptr_type)->size);
	cdata->flags = base_cdata->flags & ZEND_FFI_FLAG_CONST;
	return &cdata->std;
}
/* }}} */

static zend_result zend_ffi_cdata_do_operation(uint8_t opcode, zval *result, zval *op1, zval *op2) /* {{{ */
{
	zend_long offset;

	ZVAL_DEREF(op1);
	ZVAL_DEREF(op2);
	if (Z_TYPE_P(op1) == IS_OBJECT && Z_OBJCE_P(op1) == zend_ffi_cdata_ce) {
		zend_ffi_cdata *cdata1 = (zend_ffi_cdata*)Z_OBJ_P(op1);
		zend_ffi_type *type1 = ZEND_FFI_TYPE(cdata1->type);

		if (type1->kind == ZEND_FFI_TYPE_POINTER || type1->kind == ZEND_FFI_TYPE_ARRAY) {
			if (opcode == ZEND_ADD) {
				offset = zval_get_long(op2);
				ZVAL_OBJ(result, zend_ffi_add(cdata1, type1, offset));
				if (result == op1) {
					OBJ_RELEASE(&cdata1->std);
				}
				return SUCCESS;
			} else if (opcode == ZEND_SUB) {
				if (Z_TYPE_P(op2) == IS_OBJECT && Z_OBJCE_P(op2) == zend_ffi_cdata_ce) {
					zend_ffi_cdata *cdata2 = (zend_ffi_cdata*)Z_OBJ_P(op2);
					zend_ffi_type *type2 = ZEND_FFI_TYPE(cdata2->type);

					if (type2->kind == ZEND_FFI_TYPE_POINTER || type2->kind == ZEND_FFI_TYPE_ARRAY) {
						zend_ffi_type *t1, *t2;
						char *p1, *p2;

						if (type1->kind == ZEND_FFI_TYPE_POINTER) {
							t1 = ZEND_FFI_TYPE(type1->pointer.type);
							p1 = (char*)(*(void**)cdata1->ptr);
						} else {
							t1 = ZEND_FFI_TYPE(type1->array.type);
							p1 = cdata1->ptr;
						}
						if (type2->kind == ZEND_FFI_TYPE_POINTER) {
							t2 = ZEND_FFI_TYPE(type2->pointer.type);
							p2 = (char*)(*(void**)cdata2->ptr);
						} else {
							t2 = ZEND_FFI_TYPE(type2->array.type);
							p2 = cdata2->ptr;
						}
						if (zend_ffi_is_same_type(t1, t2)) {
							ZVAL_LONG(result,
								(zend_long)(p1 - p2) / (zend_long)t1->size);
							return SUCCESS;
						}
					}
				}
				offset = zval_get_long(op2);
				ZVAL_OBJ(result, zend_ffi_add(cdata1, type1, -offset));
				if (result == op1) {
					OBJ_RELEASE(&cdata1->std);
				}
				return SUCCESS;
			}
		}
	} else if (Z_TYPE_P(op2) == IS_OBJECT && Z_OBJCE_P(op2) == zend_ffi_cdata_ce) {
		zend_ffi_cdata *cdata2 = (zend_ffi_cdata*)Z_OBJ_P(op2);
		zend_ffi_type *type2 = ZEND_FFI_TYPE(cdata2->type);

		if (type2->kind == ZEND_FFI_TYPE_POINTER || type2->kind == ZEND_FFI_TYPE_ARRAY) {
			if (opcode == ZEND_ADD) {
				offset = zval_get_long(op1);
				ZVAL_OBJ(result, zend_ffi_add(cdata2, type2, offset));
				return SUCCESS;
			}
		}
	}

	return FAILURE;
}
/* }}} */

typedef struct _zend_ffi_cdata_iterator {
	zend_object_iterator it;
	zend_long key;
	zval value;
	bool by_ref;
} zend_ffi_cdata_iterator;

static void zend_ffi_cdata_it_dtor(zend_object_iterator *iter) /* {{{ */
{
	zval_ptr_dtor(&((zend_ffi_cdata_iterator*)iter)->value);
	zval_ptr_dtor(&iter->data);
}
/* }}} */

static int zend_ffi_cdata_it_valid(zend_object_iterator *it) /* {{{ */
{
	zend_ffi_cdata_iterator *iter = (zend_ffi_cdata_iterator*)it;
	zend_ffi_cdata *cdata = (zend_ffi_cdata*)Z_OBJ(iter->it.data);
	zend_ffi_type  *type = ZEND_FFI_TYPE(cdata->type);

	return (iter->key >= 0 && iter->key < type->array.length) ? SUCCESS : FAILURE;
}
/* }}} */

static zval *zend_ffi_cdata_it_get_current_data(zend_object_iterator *it) /* {{{ */
{
	zend_ffi_cdata_iterator *iter = (zend_ffi_cdata_iterator*)it;
	zend_ffi_cdata *cdata = (zend_ffi_cdata*)Z_OBJ(iter->it.data);
	zend_ffi_type  *type = ZEND_FFI_TYPE(cdata->type);
	zend_ffi_type  *dim_type;
	void *ptr;

	if (!cdata->ptr) {
		zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
		return &EG(uninitialized_zval);
	}
	dim_type = type->array.type;
	if (ZEND_FFI_TYPE_IS_OWNED(dim_type)) {
		dim_type = ZEND_FFI_TYPE(dim_type);
		if (!(dim_type->attr & ZEND_FFI_ATTR_STORED)
		 && dim_type->kind == ZEND_FFI_TYPE_POINTER) {
			type->array.type = dim_type = zend_ffi_remember_type(dim_type);
		}
	}
	ptr = (void*)((char*)cdata->ptr + dim_type->size * iter->it.index);

	zval_ptr_dtor(&iter->value);
	zend_ffi_cdata_to_zval(NULL, ptr, dim_type, iter->by_ref ? BP_VAR_RW : BP_VAR_R, &iter->value, (cdata->flags & ZEND_FFI_FLAG_CONST) | (zend_ffi_flags)(type->attr & ZEND_FFI_ATTR_CONST), 0, 0);
	return &iter->value;
}
/* }}} */

static void zend_ffi_cdata_it_get_current_key(zend_object_iterator *it, zval *key) /* {{{ */
{
	zend_ffi_cdata_iterator *iter = (zend_ffi_cdata_iterator*)it;
	ZVAL_LONG(key, iter->key);
}
/* }}} */

static void zend_ffi_cdata_it_move_forward(zend_object_iterator *it) /* {{{ */
{
	zend_ffi_cdata_iterator *iter = (zend_ffi_cdata_iterator*)it;
	iter->key++;
}
/* }}} */

static void zend_ffi_cdata_it_rewind(zend_object_iterator *it) /* {{{ */
{
	zend_ffi_cdata_iterator *iter = (zend_ffi_cdata_iterator*)it;
	iter->key = 0;
}
/* }}} */

static const zend_object_iterator_funcs zend_ffi_cdata_it_funcs = {
	zend_ffi_cdata_it_dtor,
	zend_ffi_cdata_it_valid,
	zend_ffi_cdata_it_get_current_data,
	zend_ffi_cdata_it_get_current_key,
	zend_ffi_cdata_it_move_forward,
	zend_ffi_cdata_it_rewind,
	NULL,
	NULL, /* get_gc */
};

static zend_object_iterator *zend_ffi_cdata_get_iterator(zend_class_entry *ce, zval *object, int by_ref) /* {{{ */
{
	zend_ffi_cdata *cdata = (zend_ffi_cdata*)Z_OBJ_P(object);
	zend_ffi_type  *type = ZEND_FFI_TYPE(cdata->type);
	zend_ffi_cdata_iterator *iter;

	if (type->kind != ZEND_FFI_TYPE_ARRAY) {
		zend_throw_error(zend_ffi_exception_ce, "Attempt to iterate on non C array");
		return NULL;
	}

	iter = emalloc(sizeof(zend_ffi_cdata_iterator));

	zend_iterator_init(&iter->it);

	Z_ADDREF_P(object);
	ZVAL_OBJ(&iter->it.data, Z_OBJ_P(object));
	iter->it.funcs = &zend_ffi_cdata_it_funcs;
	iter->key = 0;
	iter->by_ref = by_ref;
	ZVAL_UNDEF(&iter->value);

	return &iter->it;
}
/* }}} */

static HashTable *zend_ffi_cdata_get_debug_info(zend_object *obj, int *is_temp) /* {{{ */
{
	zend_ffi_cdata *cdata = (zend_ffi_cdata*)obj;
	zend_ffi_type  *type = ZEND_FFI_TYPE(cdata->type);
	void           *ptr = cdata->ptr;
	HashTable      *ht = NULL;
	zend_string    *key;
	zend_ffi_field *f;
	zend_long       n;
	zval            tmp;

	if (!cdata->ptr) {
		zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
		return NULL;
	}

	switch (type->kind) {
		case ZEND_FFI_TYPE_VOID:
			return NULL;
		case ZEND_FFI_TYPE_BOOL:
		case ZEND_FFI_TYPE_CHAR:
		case ZEND_FFI_TYPE_ENUM:
		case ZEND_FFI_TYPE_FLOAT:
		case ZEND_FFI_TYPE_DOUBLE:
#ifdef HAVE_LONG_DOUBLE
		case ZEND_FFI_TYPE_LONGDOUBLE:
#endif
		case ZEND_FFI_TYPE_UINT8:
		case ZEND_FFI_TYPE_SINT8:
		case ZEND_FFI_TYPE_UINT16:
		case ZEND_FFI_TYPE_SINT16:
		case ZEND_FFI_TYPE_UINT32:
		case ZEND_FFI_TYPE_SINT32:
		case ZEND_FFI_TYPE_UINT64:
		case ZEND_FFI_TYPE_SINT64:
			zend_ffi_cdata_to_zval(cdata, ptr, type, BP_VAR_R, &tmp, ZEND_FFI_FLAG_CONST, 0, 0);
			ht = zend_new_array(1);
			zend_hash_str_add(ht, "cdata", sizeof("cdata")-1, &tmp);
			*is_temp = 1;
			return ht;
			break;
		case ZEND_FFI_TYPE_POINTER:
			if (*(void**)ptr == NULL) {
				ZVAL_NULL(&tmp);
				ht = zend_new_array(1);
				zend_hash_index_add_new(ht, 0, &tmp);
				*is_temp = 1;
				return ht;
			} else if (ZEND_FFI_TYPE(type->pointer.type)->kind == ZEND_FFI_TYPE_VOID) {
				ZVAL_LONG(&tmp, (uintptr_t)*(void**)ptr);
				ht = zend_new_array(1);
				zend_hash_index_add_new(ht, 0, &tmp);
				*is_temp = 1;
				return ht;
			} else {
				zend_ffi_cdata_to_zval(NULL, *(void**)ptr, ZEND_FFI_TYPE(type->pointer.type), BP_VAR_R, &tmp, ZEND_FFI_FLAG_CONST, 0, 0);
				ht = zend_new_array(1);
				zend_hash_index_add_new(ht, 0, &tmp);
				*is_temp = 1;
				return ht;
			}
			break;
		case ZEND_FFI_TYPE_STRUCT:
			ht = zend_new_array(zend_hash_num_elements(&type->record.fields));
			ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&type->record.fields, key, f) {
				if (key) {
					if (!f->bits) {
						void *f_ptr = (void*)(((char*)ptr) + f->offset);
						zend_ffi_cdata_to_zval(NULL, f_ptr, ZEND_FFI_TYPE(f->type), BP_VAR_R, &tmp, ZEND_FFI_FLAG_CONST, 0, type->attr & ZEND_FFI_ATTR_UNION);
						zend_hash_add(ht, key, &tmp);
					} else {
						zend_ffi_bit_field_to_zval(ptr, f, &tmp);
						zend_hash_add(ht, key, &tmp);
					}
				}
			} ZEND_HASH_FOREACH_END();
			*is_temp = 1;
			return ht;
		case ZEND_FFI_TYPE_ARRAY:
			ht = zend_new_array(type->array.length);
			for (n = 0; n < type->array.length; n++) {
				zend_ffi_cdata_to_zval(NULL, ptr, ZEND_FFI_TYPE(type->array.type), BP_VAR_R, &tmp, ZEND_FFI_FLAG_CONST, 0, 0);
				zend_hash_index_add(ht, n, &tmp);
				ptr = (void*)(((char*)ptr) + ZEND_FFI_TYPE(type->array.type)->size);
			}
			*is_temp = 1;
			return ht;
		case ZEND_FFI_TYPE_FUNC:
			ht = zend_new_array(0);
			// TODO: function name ???
			*is_temp = 1;
			return ht;
			break;
		default:
			ZEND_UNREACHABLE();
			break;
	}
	return NULL;
}
/* }}} */

static zend_result zend_ffi_cdata_get_closure(zend_object *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zend_object **obj_ptr, bool check_only) /* {{{ */
{
	zend_ffi_cdata *cdata = (zend_ffi_cdata*)obj;
	zend_ffi_type  *type = ZEND_FFI_TYPE(cdata->type);
	zend_function  *func;

	if (type->kind != ZEND_FFI_TYPE_POINTER) {
		if (!check_only) {
			zend_throw_error(zend_ffi_exception_ce, "Attempt to call non C function pointer");
		}
		return FAILURE;
	}
	type = ZEND_FFI_TYPE(type->pointer.type);
	if (type->kind != ZEND_FFI_TYPE_FUNC) {
		if (!check_only) {
			zend_throw_error(zend_ffi_exception_ce, "Attempt to call non C function pointer");
		}
		return FAILURE;
	}
	if (!cdata->ptr) {
		if (!check_only) {
			zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
		}
		return FAILURE;
	}

	if (EXPECTED(EG(trampoline).common.function_name == NULL)) {
		func = &EG(trampoline);
	} else {
		func = ecalloc(sizeof(zend_internal_function), 1);
	}
	func->type = ZEND_INTERNAL_FUNCTION;
	func->common.arg_flags[0] = 0;
	func->common.arg_flags[1] = 0;
	func->common.arg_flags[2] = 0;
	func->common.fn_flags = ZEND_ACC_CALL_VIA_TRAMPOLINE;
	func->common.function_name = ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE);
	/* set to 0 to avoid arg_info[] allocation, because all values are passed by value anyway */
	func->common.num_args = 0;
	func->common.required_num_args = type->func.args ? zend_hash_num_elements(type->func.args) : 0;
	func->common.scope = NULL;
	func->common.prototype = NULL;
	func->common.arg_info = NULL;
	func->internal_function.handler = ZEND_FN(ffi_trampoline);
	func->internal_function.module = NULL;

	func->internal_function.reserved[0] = type;
	func->internal_function.reserved[1] = *(void**)cdata->ptr;

	*ce_ptr = NULL;
	*fptr_ptr= func;
	*obj_ptr = NULL;

	return SUCCESS;
}
/* }}} */

static zend_object *zend_ffi_ctype_new(zend_class_entry *class_type) /* {{{ */
{
	zend_ffi_ctype *ctype;

	ctype = emalloc(sizeof(zend_ffi_ctype));

	zend_ffi_object_init(&ctype->std, class_type);

	ctype->type = NULL;

	return &ctype->std;
}
/* }}} */

static void zend_ffi_ctype_free_obj(zend_object *object) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)object;

	zend_ffi_type_dtor(ctype->type);

    if (UNEXPECTED(GC_FLAGS(object) & IS_OBJ_WEAKLY_REFERENCED)) {
        zend_weakrefs_notify(object);
    }
}
/* }}} */

static bool zend_ffi_is_same_type(zend_ffi_type *type1, zend_ffi_type *type2) /* {{{ */
{
	while (1) {
		if (type1 == type2) {
			return 1;
		} else if (type1->kind == type2->kind) {
			if (type1->kind < ZEND_FFI_TYPE_POINTER) {
				return 1;
			} else if (type1->kind == ZEND_FFI_TYPE_POINTER) {
				type1 = ZEND_FFI_TYPE(type1->pointer.type);
				type2 = ZEND_FFI_TYPE(type2->pointer.type);
				if (type1->kind == ZEND_FFI_TYPE_VOID ||
				    type2->kind == ZEND_FFI_TYPE_VOID) {
				    return 1;
				}
			} else if (type1->kind == ZEND_FFI_TYPE_ARRAY &&
			           type1->array.length == type2->array.length) {
				type1 = ZEND_FFI_TYPE(type1->array.type);
				type2 = ZEND_FFI_TYPE(type2->array.type);
			} else {
				break;
			}
		} else {
			break;
		}
	}
	return 0;
}
/* }}} */

static zend_string *zend_ffi_ctype_get_class_name(const zend_object *zobj) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)zobj;

	return zend_ffi_get_class_name(zobj->ce->name, ZEND_FFI_TYPE(ctype->type));
}
/* }}} */

static int zend_ffi_ctype_compare_objects(zval *o1, zval *o2) /* {{{ */
{
	if (Z_TYPE_P(o1) == IS_OBJECT && Z_OBJCE_P(o1) == zend_ffi_ctype_ce &&
	    Z_TYPE_P(o2) == IS_OBJECT && Z_OBJCE_P(o2) == zend_ffi_ctype_ce) {
		zend_ffi_ctype *ctype1 = (zend_ffi_ctype*)Z_OBJ_P(o1);
		zend_ffi_ctype *ctype2 = (zend_ffi_ctype*)Z_OBJ_P(o2);
		zend_ffi_type *type1 = ZEND_FFI_TYPE(ctype1->type);
		zend_ffi_type *type2 = ZEND_FFI_TYPE(ctype2->type);

		if (zend_ffi_is_same_type(type1, type2)) {
			return 0;
		} else {
			return 1;
		}
	}
	zend_throw_error(zend_ffi_exception_ce, "Comparison of incompatible C types");
	return 0;
}
/* }}} */

static HashTable *zend_ffi_ctype_get_debug_info(zend_object *obj, int *is_temp) /* {{{ */
{
	return NULL;
}
/* }}} */

static zend_object *zend_ffi_new(zend_class_entry *class_type) /* {{{ */
{
	zend_ffi *ffi;

	ffi = emalloc(sizeof(zend_ffi));

	zend_ffi_object_init(&ffi->std, class_type);

	ffi->lib = NULL;
	ffi->symbols = NULL;
	ffi->tags = NULL;
	ffi->persistent = 0;

	return &ffi->std;
}
/* }}} */

static void _zend_ffi_type_dtor(zend_ffi_type *type) /* {{{ */
{
	type = ZEND_FFI_TYPE(type);

	switch (type->kind) {
		case ZEND_FFI_TYPE_ENUM:
			if (type->enumeration.tag_name) {
				zend_string_release(type->enumeration.tag_name);
			}
			break;
		case ZEND_FFI_TYPE_STRUCT:
			if (type->record.tag_name) {
				zend_string_release(type->record.tag_name);
			}
			zend_hash_destroy(&type->record.fields);
			break;
		case ZEND_FFI_TYPE_POINTER:
			zend_ffi_type_dtor(type->pointer.type);
			break;
		case ZEND_FFI_TYPE_ARRAY:
			zend_ffi_type_dtor(type->array.type);
			break;
		case ZEND_FFI_TYPE_FUNC:
			if (type->func.args) {
				zend_hash_destroy(type->func.args);
				pefree(type->func.args, type->attr & ZEND_FFI_ATTR_PERSISTENT);
			}
			zend_ffi_type_dtor(type->func.ret_type);
			break;
		default:
			break;
	}
	pefree(type, type->attr & ZEND_FFI_ATTR_PERSISTENT);
}
/* }}} */

static void zend_ffi_type_hash_dtor(zval *zv) /* {{{ */
{
	zend_ffi_type *type = Z_PTR_P(zv);
	zend_ffi_type_dtor(type);
}
/* }}} */

static void zend_ffi_field_hash_dtor(zval *zv) /* {{{ */
{
	zend_ffi_field *field = Z_PTR_P(zv);
	zend_ffi_type_dtor(field->type);
	efree(field);
}
/* }}} */

static void zend_ffi_field_hash_persistent_dtor(zval *zv) /* {{{ */
{
	zend_ffi_field *field = Z_PTR_P(zv);
	zend_ffi_type_dtor(field->type);
	free(field);
}
/* }}} */

static void zend_ffi_symbol_hash_dtor(zval *zv) /* {{{ */
{
	zend_ffi_symbol *sym = Z_PTR_P(zv);
	zend_ffi_type_dtor(sym->type);
	efree(sym);
}
/* }}} */

static void zend_ffi_symbol_hash_persistent_dtor(zval *zv) /* {{{ */
{
	zend_ffi_symbol *sym = Z_PTR_P(zv);
	zend_ffi_type_dtor(sym->type);
	free(sym);
}
/* }}} */

static void zend_ffi_tag_hash_dtor(zval *zv) /* {{{ */
{
	zend_ffi_tag *tag = Z_PTR_P(zv);
	zend_ffi_type_dtor(tag->type);
	efree(tag);
}
/* }}} */

static void zend_ffi_tag_hash_persistent_dtor(zval *zv) /* {{{ */
{
	zend_ffi_tag *tag = Z_PTR_P(zv);
	zend_ffi_type_dtor(tag->type);
	free(tag);
}
/* }}} */

static void zend_ffi_cdata_dtor(zend_ffi_cdata *cdata) /* {{{ */
{
	zend_ffi_type_dtor(cdata->type);
	if (cdata->flags & ZEND_FFI_FLAG_OWNED) {
		if (cdata->ptr != (void*)&cdata->ptr_holder) {
			pefree(cdata->ptr, cdata->flags & ZEND_FFI_FLAG_PERSISTENT);
		} else {
			pefree(cdata->ptr_holder, cdata->flags & ZEND_FFI_FLAG_PERSISTENT);
		}
	}
}
/* }}} */

static void zend_ffi_scope_hash_dtor(zval *zv) /* {{{ */
{
	zend_ffi_scope *scope = Z_PTR_P(zv);
	if (scope->symbols) {
		zend_hash_destroy(scope->symbols);
		free(scope->symbols);
	}
	if (scope->tags) {
		zend_hash_destroy(scope->tags);
		free(scope->tags);
	}
	free(scope);
}
/* }}} */

static void zend_ffi_free_obj(zend_object *object) /* {{{ */
{
	zend_ffi *ffi = (zend_ffi*)object;

	if (ffi->persistent) {
		return;
	}

	if (ffi->lib) {
		DL_UNLOAD(ffi->lib);
		ffi->lib = NULL;
	}

	if (ffi->symbols) {
		zend_hash_destroy(ffi->symbols);
		efree(ffi->symbols);
	}

	if (ffi->tags) {
		zend_hash_destroy(ffi->tags);
		efree(ffi->tags);
	}

    if (UNEXPECTED(GC_FLAGS(object) & IS_OBJ_WEAKLY_REFERENCED)) {
        zend_weakrefs_notify(object);
    }
}
/* }}} */

static void zend_ffi_cdata_free_obj(zend_object *object) /* {{{ */
{
	zend_ffi_cdata *cdata = (zend_ffi_cdata*)object;

	zend_ffi_cdata_dtor(cdata);

    if (UNEXPECTED(GC_FLAGS(object) & IS_OBJ_WEAKLY_REFERENCED)) {
        zend_weakrefs_notify(object);
    }
}
/* }}} */

static zend_object *zend_ffi_cdata_clone_obj(zend_object *obj) /* {{{ */
{
	zend_ffi_cdata *old_cdata = (zend_ffi_cdata*)obj;
	zend_ffi_type *type = ZEND_FFI_TYPE(old_cdata->type);
	zend_ffi_cdata *new_cdata;

	new_cdata = (zend_ffi_cdata*)zend_ffi_cdata_new(zend_ffi_cdata_ce);
	if (type->kind < ZEND_FFI_TYPE_POINTER) {
		new_cdata->std.handlers = &zend_ffi_cdata_value_handlers;
	}
	new_cdata->type = type;
	new_cdata->ptr = emalloc(type->size);
	memcpy(new_cdata->ptr, old_cdata->ptr, type->size);
	new_cdata->flags |= ZEND_FFI_FLAG_OWNED;

	return &new_cdata->std;
}
/* }}} */

static zval *zend_ffi_read_var(zend_object *obj, zend_string *var_name, int read_type, void **cache_slot, zval *rv) /* {{{ */
{
	zend_ffi        *ffi = (zend_ffi*)obj;
	zend_ffi_symbol *sym = NULL;

	if (ffi->symbols) {
		sym = zend_hash_find_ptr(ffi->symbols, var_name);
		if (sym && sym->kind != ZEND_FFI_SYM_VAR && sym->kind != ZEND_FFI_SYM_CONST && sym->kind != ZEND_FFI_SYM_FUNC) {
			sym = NULL;
		}
	}
	if (!sym) {
		zend_throw_error(zend_ffi_exception_ce, "Attempt to read undefined C variable '%s'", ZSTR_VAL(var_name));
		return &EG(uninitialized_zval);
	}

	if (sym->kind == ZEND_FFI_SYM_VAR) {
		zend_ffi_cdata_to_zval(NULL, sym->addr, ZEND_FFI_TYPE(sym->type), read_type, rv, (zend_ffi_flags)sym->is_const, 0, 0);
	} else if (sym->kind == ZEND_FFI_SYM_FUNC) {
		zend_ffi_cdata *cdata;
		zend_ffi_type *new_type = emalloc(sizeof(zend_ffi_type));

		new_type->kind = ZEND_FFI_TYPE_POINTER;
		new_type->attr = 0;
		new_type->size = sizeof(void*);
		new_type->align = _Alignof(void*);
		new_type->pointer.type = ZEND_FFI_TYPE(sym->type);

		cdata = emalloc(sizeof(zend_ffi_cdata));
		zend_ffi_object_init(&cdata->std, zend_ffi_cdata_ce);
		cdata->type = ZEND_FFI_TYPE_MAKE_OWNED(new_type);
		cdata->flags = ZEND_FFI_FLAG_CONST;
		cdata->ptr_holder = sym->addr;
		cdata->ptr = &cdata->ptr_holder;
		ZVAL_OBJ(rv, &cdata->std);
	} else {
		ZVAL_LONG(rv, sym->value);
	}

	return rv;
}
/* }}} */

static zval *zend_ffi_write_var(zend_object *obj, zend_string *var_name, zval *value, void **cache_slot) /* {{{ */
{
	zend_ffi        *ffi = (zend_ffi*)obj;
	zend_ffi_symbol *sym = NULL;

	if (ffi->symbols) {
		sym = zend_hash_find_ptr(ffi->symbols, var_name);
		if (sym && sym->kind != ZEND_FFI_SYM_VAR) {
			sym = NULL;
		}
	}
	if (!sym) {
		zend_throw_error(zend_ffi_exception_ce, "Attempt to assign undefined C variable '%s'", ZSTR_VAL(var_name));
		return value;
	}

	if (sym->is_const) {
		zend_throw_error(zend_ffi_exception_ce, "Attempt to assign read-only C variable '%s'", ZSTR_VAL(var_name));
		return value;
	}

	zend_ffi_zval_to_cdata(sym->addr, ZEND_FFI_TYPE(sym->type), value);
	return value;
}
/* }}} */

static zend_result zend_ffi_pass_arg(zval *arg, zend_ffi_type *type, ffi_type **pass_type, void **arg_values, uint32_t n, zend_execute_data *execute_data) /* {{{ */
{
	zend_long lval;
	double dval;
	zend_string *str, *tmp_str;
	zend_ffi_type_kind kind = type->kind;

	ZVAL_DEREF(arg);

again:
	switch (kind) {
		case ZEND_FFI_TYPE_FLOAT:
			dval = zval_get_double(arg);
			*pass_type = &ffi_type_float;
			*(float*)arg_values[n] = (float)dval;
			break;
		case ZEND_FFI_TYPE_DOUBLE:
			dval = zval_get_double(arg);
			*pass_type = &ffi_type_double;
			*(double*)arg_values[n] = dval;
			break;
#ifdef HAVE_LONG_DOUBLE
		case ZEND_FFI_TYPE_LONGDOUBLE:
			dval = zval_get_double(arg);
			*pass_type = &ffi_type_double;
			*(long double*)arg_values[n] = (long double)dval;
			break;
#endif
		case ZEND_FFI_TYPE_UINT8:
			lval = zval_get_long(arg);
			*pass_type = &ffi_type_uint8;
			*(uint8_t*)arg_values[n] = (uint8_t)lval;
			break;
		case ZEND_FFI_TYPE_SINT8:
			lval = zval_get_long(arg);
			*pass_type = &ffi_type_sint8;
			*(int8_t*)arg_values[n] = (int8_t)lval;
			break;
		case ZEND_FFI_TYPE_UINT16:
			lval = zval_get_long(arg);
			*pass_type = &ffi_type_uint16;
			*(uint16_t*)arg_values[n] = (uint16_t)lval;
			break;
		case ZEND_FFI_TYPE_SINT16:
			lval = zval_get_long(arg);
			*pass_type = &ffi_type_sint16;
			*(int16_t*)arg_values[n] = (int16_t)lval;
			break;
		case ZEND_FFI_TYPE_UINT32:
			lval = zval_get_long(arg);
			*pass_type = &ffi_type_uint32;
			*(uint32_t*)arg_values[n] = (uint32_t)lval;
			break;
		case ZEND_FFI_TYPE_SINT32:
			lval = zval_get_long(arg);
			*pass_type = &ffi_type_sint32;
			*(int32_t*)arg_values[n] = (int32_t)lval;
			break;
		case ZEND_FFI_TYPE_UINT64:
			lval = zval_get_long(arg);
			*pass_type = &ffi_type_uint64;
			*(uint64_t*)arg_values[n] = (uint64_t)lval;
			break;
		case ZEND_FFI_TYPE_SINT64:
			lval = zval_get_long(arg);
			*pass_type = &ffi_type_sint64;
			*(int64_t*)arg_values[n] = (int64_t)lval;
			break;
		case ZEND_FFI_TYPE_POINTER:
			*pass_type = &ffi_type_pointer;
			if (Z_TYPE_P(arg) == IS_NULL) {
				*(void**)arg_values[n] = NULL;
				return SUCCESS;
			} else if (Z_TYPE_P(arg) == IS_STRING
			        && ((ZEND_FFI_TYPE(type->pointer.type)->kind == ZEND_FFI_TYPE_CHAR)
			         || (ZEND_FFI_TYPE(type->pointer.type)->kind == ZEND_FFI_TYPE_VOID))) {
				*(void**)arg_values[n] = Z_STRVAL_P(arg);
				return SUCCESS;
			} else if (Z_TYPE_P(arg) == IS_OBJECT && Z_OBJCE_P(arg) == zend_ffi_cdata_ce) {
				zend_ffi_cdata *cdata = (zend_ffi_cdata*)Z_OBJ_P(arg);

				if (zend_ffi_is_compatible_type(type, ZEND_FFI_TYPE(cdata->type))) {
					if (ZEND_FFI_TYPE(cdata->type)->kind == ZEND_FFI_TYPE_POINTER) {
						if (!cdata->ptr) {
							zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
							return FAILURE;
						}
						*(void**)arg_values[n] = *(void**)cdata->ptr;
					} else {
						*(void**)arg_values[n] = cdata->ptr;
					}
					return SUCCESS;
				}
#if FFI_CLOSURES
			} else if (ZEND_FFI_TYPE(type->pointer.type)->kind == ZEND_FFI_TYPE_FUNC) {
				void *callback = zend_ffi_create_callback(ZEND_FFI_TYPE(type->pointer.type), arg);

				if (callback) {
					*(void**)arg_values[n] = callback;
					break;
				} else {
					return FAILURE;
				}
#endif
			}
			zend_ffi_pass_incompatible(arg, type, n, execute_data);
			return FAILURE;
		case ZEND_FFI_TYPE_BOOL:
			*pass_type = &ffi_type_uint8;
			*(uint8_t*)arg_values[n] = zend_is_true(arg);
			break;
		case ZEND_FFI_TYPE_CHAR:
			str = zval_get_tmp_string(arg, &tmp_str);
			*pass_type = &ffi_type_sint8;
			*(char*)arg_values[n] = ZSTR_VAL(str)[0];
			if (ZSTR_LEN(str) != 1) {
				zend_ffi_pass_incompatible(arg, type, n, execute_data);
			}
			zend_tmp_string_release(tmp_str);
			break;
		case ZEND_FFI_TYPE_ENUM:
			kind = type->enumeration.kind;
			goto again;
		case ZEND_FFI_TYPE_STRUCT:
			if (Z_TYPE_P(arg) == IS_OBJECT && Z_OBJCE_P(arg) == zend_ffi_cdata_ce) {
				zend_ffi_cdata *cdata = (zend_ffi_cdata*)Z_OBJ_P(arg);

				if (zend_ffi_is_compatible_type(type, ZEND_FFI_TYPE(cdata->type))) {
					*pass_type = zend_ffi_make_fake_struct_type(type);
					arg_values[n] = cdata->ptr;
					break;
				}
			}
			zend_ffi_pass_incompatible(arg, type, n, execute_data);
			return FAILURE;
		default:
			zend_ffi_pass_unsupported(type);
			return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

static zend_result zend_ffi_pass_var_arg(zval *arg, ffi_type **pass_type, void **arg_values, uint32_t n, zend_execute_data *execute_data) /* {{{ */
{
	ZVAL_DEREF(arg);
	switch (Z_TYPE_P(arg)) {
		case IS_NULL:
			*pass_type = &ffi_type_pointer;
			*(void**)arg_values[n] = NULL;
			break;
		case IS_FALSE:
			*pass_type = &ffi_type_uint8;
			*(uint8_t*)arg_values[n] = 0;
			break;
		case IS_TRUE:
			*pass_type = &ffi_type_uint8;
			*(uint8_t*)arg_values[n] = 1;
			break;
		case IS_LONG:
			if (sizeof(zend_long) == 4) {
				*pass_type = &ffi_type_sint32;
				*(int32_t*)arg_values[n] = Z_LVAL_P(arg);
			} else {
				*pass_type = &ffi_type_sint64;
				*(int64_t*)arg_values[n] = Z_LVAL_P(arg);
			}
			break;
		case IS_DOUBLE:
			*pass_type = &ffi_type_double;
			*(double*)arg_values[n] = Z_DVAL_P(arg);
			break;
		case IS_STRING:
			*pass_type = &ffi_type_pointer;
			*(char**)arg_values[n] = Z_STRVAL_P(arg);
			break;
		case IS_OBJECT:
			if (Z_OBJCE_P(arg) == zend_ffi_cdata_ce) {
				zend_ffi_cdata *cdata = (zend_ffi_cdata*)Z_OBJ_P(arg);
				zend_ffi_type *type = ZEND_FFI_TYPE(cdata->type);

				return zend_ffi_pass_arg(arg, type, pass_type, arg_values, n, execute_data);
			}
			ZEND_FALLTHROUGH;
		default:
			zend_throw_error(zend_ffi_exception_ce, "Unsupported argument type");
			return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

static ZEND_FUNCTION(ffi_trampoline) /* {{{ */
{
	zend_ffi_type *type = EX(func)->internal_function.reserved[0];
	void *addr = EX(func)->internal_function.reserved[1];
	ffi_cif cif;
	ffi_type *ret_type = NULL;
	ffi_type **arg_types = NULL;
	void **arg_values = NULL;
	uint32_t n, arg_count;
	void *ret;
	zend_ffi_type *arg_type;
	ALLOCA_FLAG(arg_types_use_heap = 0)
	ALLOCA_FLAG(arg_values_use_heap = 0)
	ALLOCA_FLAG(ret_use_heap = 0)

	ZEND_ASSERT(type->kind == ZEND_FFI_TYPE_FUNC);
	arg_count = type->func.args ? zend_hash_num_elements(type->func.args) : 0;
	if (type->attr & ZEND_FFI_ATTR_VARIADIC) {
		if (arg_count > EX_NUM_ARGS()) {
			zend_throw_error(zend_ffi_exception_ce, "Incorrect number of arguments for C function '%s', expecting at least %d parameter%s", ZSTR_VAL(EX(func)->internal_function.function_name), arg_count, (arg_count != 1) ? "s" : "");
			goto exit;
		}
		if (EX_NUM_ARGS()) {
			arg_types = do_alloca(
				sizeof(ffi_type*) * EX_NUM_ARGS(), arg_types_use_heap);
			arg_values = do_alloca(
				(sizeof(void*) + ZEND_FFI_SIZEOF_ARG) * EX_NUM_ARGS(), arg_values_use_heap);
			n = 0;
			if (type->func.args) {
				ZEND_HASH_PACKED_FOREACH_PTR(type->func.args, arg_type) {
					arg_type = ZEND_FFI_TYPE(arg_type);
					arg_values[n] = ((char*)arg_values) + (sizeof(void*) * EX_NUM_ARGS()) + (ZEND_FFI_SIZEOF_ARG * n);
					if (zend_ffi_pass_arg(EX_VAR_NUM(n), arg_type, &arg_types[n], arg_values, n, execute_data) == FAILURE) {
						free_alloca(arg_types, arg_types_use_heap);
						free_alloca(arg_values, arg_values_use_heap);
						goto exit;
					}
					n++;
				} ZEND_HASH_FOREACH_END();
			}
			for (; n < EX_NUM_ARGS(); n++) {
				arg_values[n] = ((char*)arg_values) + (sizeof(void*) * EX_NUM_ARGS()) + (ZEND_FFI_SIZEOF_ARG * n);
				if (zend_ffi_pass_var_arg(EX_VAR_NUM(n), &arg_types[n], arg_values, n, execute_data) == FAILURE) {
					free_alloca(arg_types, arg_types_use_heap);
					free_alloca(arg_values, arg_values_use_heap);
					goto exit;
				}
			}
		}
		ret_type = zend_ffi_get_type(ZEND_FFI_TYPE(type->func.ret_type));
		if (!ret_type) {
			zend_ffi_return_unsupported(type->func.ret_type);
			free_alloca(arg_types, arg_types_use_heap);
			free_alloca(arg_values, arg_values_use_heap);
			goto exit;
		}
		if (ffi_prep_cif_var(&cif, type->func.abi, arg_count, EX_NUM_ARGS(), ret_type, arg_types) != FFI_OK) {
			zend_throw_error(zend_ffi_exception_ce, "Cannot prepare callback CIF");
			free_alloca(arg_types, arg_types_use_heap);
			free_alloca(arg_values, arg_values_use_heap);
			goto exit;
		}
	} else {
		if (arg_count != EX_NUM_ARGS()) {
			zend_throw_error(zend_ffi_exception_ce, "Incorrect number of arguments for C function '%s', expecting exactly %d parameter%s", ZSTR_VAL(EX(func)->internal_function.function_name), arg_count, (arg_count != 1) ? "s" : "");
			goto exit;
		}
		if (EX_NUM_ARGS()) {
			arg_types = do_alloca(
				(sizeof(ffi_type*) + sizeof(ffi_type)) * EX_NUM_ARGS(), arg_types_use_heap);
			arg_values = do_alloca(
				(sizeof(void*) + ZEND_FFI_SIZEOF_ARG) * EX_NUM_ARGS(), arg_values_use_heap);
			n = 0;
			if (type->func.args) {
				ZEND_HASH_PACKED_FOREACH_PTR(type->func.args, arg_type) {
					arg_type = ZEND_FFI_TYPE(arg_type);
					arg_values[n] = ((char*)arg_values) + (sizeof(void*) * EX_NUM_ARGS()) + (ZEND_FFI_SIZEOF_ARG * n);
					if (zend_ffi_pass_arg(EX_VAR_NUM(n), arg_type, &arg_types[n], arg_values, n, execute_data) == FAILURE) {
						free_alloca(arg_types, arg_types_use_heap);
						free_alloca(arg_values, arg_values_use_heap);
						goto exit;
					}
					n++;
				} ZEND_HASH_FOREACH_END();
			}
		}
		ret_type = zend_ffi_get_type(ZEND_FFI_TYPE(type->func.ret_type));
		if (!ret_type) {
			zend_ffi_return_unsupported(type->func.ret_type);
			free_alloca(arg_types, arg_types_use_heap);
			free_alloca(arg_values, arg_values_use_heap);
			goto exit;
		}
		if (ffi_prep_cif(&cif, type->func.abi, arg_count, ret_type, arg_types) != FFI_OK) {
			zend_throw_error(zend_ffi_exception_ce, "Cannot prepare callback CIF");
			free_alloca(arg_types, arg_types_use_heap);
			free_alloca(arg_values, arg_values_use_heap);
			goto exit;
		}
	}

	ret = do_alloca(MAX(ret_type->size, sizeof(ffi_arg)), ret_use_heap);
	ffi_call(&cif, addr, ret, arg_values);

	for (n = 0; n < arg_count; n++) {
		if (arg_types[n]->type == FFI_TYPE_STRUCT) {
			efree(arg_types[n]);
		}
	}
	if (ret_type->type == FFI_TYPE_STRUCT) {
		efree(ret_type);
	}

	if (EX_NUM_ARGS()) {
		free_alloca(arg_types, arg_types_use_heap);
		free_alloca(arg_values, arg_values_use_heap);
	}

	if (ZEND_FFI_TYPE(type->func.ret_type)->kind != ZEND_FFI_TYPE_VOID) {
		zend_ffi_cdata_to_zval(NULL, ret, ZEND_FFI_TYPE(type->func.ret_type), BP_VAR_R, return_value, 0, 1, 0);
	} else {
		ZVAL_NULL(return_value);
	}
	free_alloca(ret, ret_use_heap);

exit:
	zend_string_release(EX(func)->common.function_name);
	if (EX(func)->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
		zend_free_trampoline(EX(func));
		EX(func) = NULL;
	}
}
/* }}} */

static zend_function *zend_ffi_get_func(zend_object **obj, zend_string *name, const zval *key) /* {{{ */
{
	zend_ffi        *ffi = (zend_ffi*)*obj;
	zend_ffi_symbol *sym = NULL;
	zend_function   *func;
	zend_ffi_type   *type;

	if (ZSTR_LEN(name) == sizeof("new") -1
	 && (ZSTR_VAL(name)[0] == 'n' || ZSTR_VAL(name)[0] == 'N')
	 && (ZSTR_VAL(name)[1] == 'e' || ZSTR_VAL(name)[1] == 'E')
	 && (ZSTR_VAL(name)[2] == 'w' || ZSTR_VAL(name)[2] == 'W')) {
		return (zend_function*)&zend_ffi_new_fn;
	} else if (ZSTR_LEN(name) == sizeof("cast") -1
	 && (ZSTR_VAL(name)[0] == 'c' || ZSTR_VAL(name)[0] == 'C')
	 && (ZSTR_VAL(name)[1] == 'a' || ZSTR_VAL(name)[1] == 'A')
	 && (ZSTR_VAL(name)[2] == 's' || ZSTR_VAL(name)[2] == 'S')
	 && (ZSTR_VAL(name)[3] == 't' || ZSTR_VAL(name)[3] == 'T')) {
		return (zend_function*)&zend_ffi_cast_fn;
	} else if (ZSTR_LEN(name) == sizeof("type") -1
	 && (ZSTR_VAL(name)[0] == 't' || ZSTR_VAL(name)[0] == 'T')
	 && (ZSTR_VAL(name)[1] == 'y' || ZSTR_VAL(name)[1] == 'Y')
	 && (ZSTR_VAL(name)[2] == 'p' || ZSTR_VAL(name)[2] == 'P')
	 && (ZSTR_VAL(name)[3] == 'e' || ZSTR_VAL(name)[3] == 'E')) {
		return (zend_function*)&zend_ffi_type_fn;
	}

	if (ffi->symbols) {
		sym = zend_hash_find_ptr(ffi->symbols, name);
		if (sym && sym->kind != ZEND_FFI_SYM_FUNC) {
			sym = NULL;
		}
	}
	if (!sym) {
		zend_throw_error(zend_ffi_exception_ce, "Attempt to call undefined C function '%s'", ZSTR_VAL(name));
		return NULL;
	}

	type = ZEND_FFI_TYPE(sym->type);
	ZEND_ASSERT(type->kind == ZEND_FFI_TYPE_FUNC);

	if (EXPECTED(EG(trampoline).common.function_name == NULL)) {
		func = &EG(trampoline);
	} else {
		func = ecalloc(sizeof(zend_internal_function), 1);
	}
	func->common.type = ZEND_INTERNAL_FUNCTION;
	func->common.arg_flags[0] = 0;
	func->common.arg_flags[1] = 0;
	func->common.arg_flags[2] = 0;
	func->common.fn_flags = ZEND_ACC_CALL_VIA_TRAMPOLINE;
	func->common.function_name = zend_string_copy(name);
	/* set to 0 to avoid arg_info[] allocation, because all values are passed by value anyway */
	func->common.num_args = 0;
	func->common.required_num_args = type->func.args ? zend_hash_num_elements(type->func.args) : 0;
	func->common.scope = NULL;
	func->common.prototype = NULL;
	func->common.arg_info = NULL;
	func->internal_function.handler = ZEND_FN(ffi_trampoline);
	func->internal_function.module = NULL;

	func->internal_function.reserved[0] = type;
	func->internal_function.reserved[1] = sym->addr;

	return func;
}
/* }}} */

static zend_never_inline int zend_ffi_disabled(void) /* {{{ */
{
	zend_throw_error(zend_ffi_exception_ce, "FFI API is restricted by \"ffi.enable\" configuration directive");
	return 0;
}
/* }}} */

static zend_always_inline bool zend_ffi_validate_api_restriction(zend_execute_data *execute_data) /* {{{ */
{
	if (EXPECTED(FFI_G(restriction) > ZEND_FFI_ENABLED)) {
		ZEND_ASSERT(FFI_G(restriction) == ZEND_FFI_PRELOAD);
		if (FFI_G(is_cli)
		 || (execute_data->prev_execute_data
		  && (execute_data->prev_execute_data->func->common.fn_flags & ZEND_ACC_PRELOADED))
		 || (CG(compiler_options) & ZEND_COMPILE_PRELOAD)) {
			return 1;
		}
	} else if (EXPECTED(FFI_G(restriction) == ZEND_FFI_ENABLED)) {
		return 1;
	}
	return zend_ffi_disabled();
}
/* }}} */

#define ZEND_FFI_VALIDATE_API_RESTRICTION() do { \
		if (UNEXPECTED(!zend_ffi_validate_api_restriction(execute_data))) { \
			RETURN_THROWS(); \
		} \
	} while (0)

ZEND_METHOD(FFI, cdef) /* {{{ */
{
	zend_string *code = NULL;
	zend_string *lib = NULL;
	zend_ffi *ffi = NULL;
	DL_HANDLE handle = NULL;
	char *err;
	void *addr;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(code)
		Z_PARAM_STR_OR_NULL(lib)
	ZEND_PARSE_PARAMETERS_END();

	if (lib) {
		handle = DL_LOAD(ZSTR_VAL(lib));
		if (!handle) {
			err = GET_DL_ERROR();
#ifdef PHP_WIN32
			if (err && err[0]) {
				zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s' (%s)", ZSTR_VAL(lib), err);
				php_win32_error_msg_free(err);
			} else {
				zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s' (Unknown reason)", ZSTR_VAL(lib));
			}
#else
			zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s' (%s)", ZSTR_VAL(lib), err);
			GET_DL_ERROR(); /* free the buffer storing the error */
#endif
			RETURN_THROWS();
		}

#ifdef RTLD_DEFAULT
	} else if (1) {
		// TODO: this might need to be disabled or protected ???
		handle = RTLD_DEFAULT;
#endif
	}

	FFI_G(symbols) = NULL;
	FFI_G(tags) = NULL;

	if (code && ZSTR_LEN(code)) {
		/* Parse C definitions */
		FFI_G(default_type_attr) = ZEND_FFI_ATTR_STORED;

		if (zend_ffi_parse_decl(ZSTR_VAL(code), ZSTR_LEN(code)) == FAILURE) {
			if (FFI_G(symbols)) {
				zend_hash_destroy(FFI_G(symbols));
				efree(FFI_G(symbols));
				FFI_G(symbols) = NULL;
			}
			if (FFI_G(tags)) {
				zend_hash_destroy(FFI_G(tags));
				efree(FFI_G(tags));
				FFI_G(tags) = NULL;
			}
			RETURN_THROWS();
		}

		if (FFI_G(symbols)) {
			zend_string *name;
			zend_ffi_symbol *sym;

			ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(FFI_G(symbols), name, sym) {
				if (sym->kind == ZEND_FFI_SYM_VAR) {
					addr = DL_FETCH_SYMBOL(handle, ZSTR_VAL(name));
					if (!addr) {
						zend_throw_error(zend_ffi_exception_ce, "Failed resolving C variable '%s'", ZSTR_VAL(name));
						RETURN_THROWS();
					}
					sym->addr = addr;
				} else if (sym->kind == ZEND_FFI_SYM_FUNC) {
					zend_string *mangled_name = zend_ffi_mangled_func_name(name, ZEND_FFI_TYPE(sym->type));

					addr = DL_FETCH_SYMBOL(handle, ZSTR_VAL(mangled_name));
					zend_string_release(mangled_name);
					if (!addr) {
						zend_throw_error(zend_ffi_exception_ce, "Failed resolving C function '%s'", ZSTR_VAL(name));
						RETURN_THROWS();
					}
					sym->addr = addr;
				}
			} ZEND_HASH_FOREACH_END();
		}
	}

	ffi = (zend_ffi*)zend_ffi_new(zend_ffi_ce);
	ffi->lib = handle;
	ffi->symbols = FFI_G(symbols);
	ffi->tags = FFI_G(tags);

	FFI_G(symbols) = NULL;
	FFI_G(tags) = NULL;

	RETURN_OBJ(&ffi->std);
}
/* }}} */

static bool zend_ffi_same_types(zend_ffi_type *old, zend_ffi_type *type) /* {{{ */
{
	if (old == type) {
		return 1;
	}

	if (old->kind != type->kind
	 || old->size != type->size
	 || old->align != type->align
	 || old->attr != type->attr) {
		return 0;
	}

	switch (old->kind) {
		case ZEND_FFI_TYPE_ENUM:
			return old->enumeration.kind == type->enumeration.kind;
		case ZEND_FFI_TYPE_ARRAY:
			return old->array.length == type->array.length
			 &&	zend_ffi_same_types(ZEND_FFI_TYPE(old->array.type), ZEND_FFI_TYPE(type->array.type));
		case ZEND_FFI_TYPE_POINTER:
			return zend_ffi_same_types(ZEND_FFI_TYPE(old->pointer.type), ZEND_FFI_TYPE(type->pointer.type));
		case ZEND_FFI_TYPE_STRUCT:
			if (zend_hash_num_elements(&old->record.fields) != zend_hash_num_elements(&type->record.fields)) {
				return 0;
			} else {
				zend_ffi_field *old_field, *field;
				zend_string *key;
				Bucket *b = type->record.fields.arData;

				ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&old->record.fields, key, old_field) {
					while (Z_TYPE(b->val) == IS_UNDEF) {
						b++;
					}
					if (key) {
						if (!b->key
						 || !zend_string_equals(key, b->key)) {
							return 0;
						}
					} else if (b->key) {
						return 0;
					}
					field = Z_PTR(b->val);
					if (old_field->offset != field->offset
					 || old_field->is_const != field->is_const
					 || old_field->is_nested != field->is_nested
					 || old_field->first_bit != field->first_bit
					 || old_field->bits != field->bits
					 || !zend_ffi_same_types(ZEND_FFI_TYPE(old_field->type), ZEND_FFI_TYPE(field->type))) {
						return 0;
					}
					b++;
				} ZEND_HASH_FOREACH_END();
			}
			break;
		case ZEND_FFI_TYPE_FUNC:
			if (old->func.abi != type->func.abi
			 || ((old->func.args ? zend_hash_num_elements(old->func.args) : 0) != (type->func.args ? zend_hash_num_elements(type->func.args) : 0))
			 || !zend_ffi_same_types(ZEND_FFI_TYPE(old->func.ret_type), ZEND_FFI_TYPE(type->func.ret_type))) {
				return 0;
			} else if (old->func.args) {
				zend_ffi_type *arg_type;
				zval *zv = type->func.args->arPacked;

				ZEND_HASH_PACKED_FOREACH_PTR(old->func.args, arg_type) {
					while (Z_TYPE_P(zv) == IS_UNDEF) {
						zv++;
					}
					if (!zend_ffi_same_types(ZEND_FFI_TYPE(arg_type), ZEND_FFI_TYPE(Z_PTR_P(zv)))) {
						return 0;
					}
					zv++;
				} ZEND_HASH_FOREACH_END();
			}
			break;
		default:
			break;
	}

	return 1;
}
/* }}} */

static bool zend_ffi_same_symbols(zend_ffi_symbol *old, zend_ffi_symbol *sym) /* {{{ */
{
	if (old->kind != sym->kind || old->is_const != sym->is_const) {
		return 0;
	}

	if (old->kind == ZEND_FFI_SYM_CONST) {
		if (old->value != sym->value) {
			return 0;
		}
	}

	return zend_ffi_same_types(ZEND_FFI_TYPE(old->type), ZEND_FFI_TYPE(sym->type));
}
/* }}} */

static bool zend_ffi_same_tags(zend_ffi_tag *old, zend_ffi_tag *tag) /* {{{ */
{
	if (old->kind != tag->kind) {
		return 0;
	}

	return zend_ffi_same_types(ZEND_FFI_TYPE(old->type), ZEND_FFI_TYPE(tag->type));
}
/* }}} */

static bool zend_ffi_subst_old_type(zend_ffi_type **dcl, zend_ffi_type *old, zend_ffi_type *type) /* {{{ */
{
	zend_ffi_type *dcl_type;
	zend_ffi_field *field;

	if (ZEND_FFI_TYPE(*dcl) == type) {
		*dcl = old;
		return 1;
	}
	dcl_type = *dcl;
	switch (dcl_type->kind) {
		case ZEND_FFI_TYPE_POINTER:
			return zend_ffi_subst_old_type(&dcl_type->pointer.type, old, type);
		case ZEND_FFI_TYPE_ARRAY:
			return zend_ffi_subst_old_type(&dcl_type->array.type, old, type);
		case ZEND_FFI_TYPE_FUNC:
			if (zend_ffi_subst_old_type(&dcl_type->func.ret_type, old, type)) {
				return 1;
			}
			if (dcl_type->func.args) {
				zval *zv;

				ZEND_HASH_PACKED_FOREACH_VAL(dcl_type->func.args, zv) {
					if (zend_ffi_subst_old_type((zend_ffi_type**)&Z_PTR_P(zv), old, type)) {
						return 1;
					}
				} ZEND_HASH_FOREACH_END();
			}
			break;
		case ZEND_FFI_TYPE_STRUCT:
			ZEND_HASH_MAP_FOREACH_PTR(&dcl_type->record.fields, field) {
				if (zend_ffi_subst_old_type(&field->type, old, type)) {
					return 1;
				}
			} ZEND_HASH_FOREACH_END();
			break;
		default:
			break;
	}
	return 0;
} /* }}} */

static void zend_ffi_cleanup_type(zend_ffi_type *old, zend_ffi_type *type) /* {{{ */
{
	zend_ffi_symbol *sym;
	zend_ffi_tag *tag;

	if (FFI_G(symbols)) {
		ZEND_HASH_MAP_FOREACH_PTR(FFI_G(symbols), sym) {
			zend_ffi_subst_old_type(&sym->type, old, type);
		} ZEND_HASH_FOREACH_END();
	}
	if (FFI_G(tags)) {
		ZEND_HASH_MAP_FOREACH_PTR(FFI_G(tags), tag) {
			zend_ffi_subst_old_type(&tag->type, old, type);
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

static zend_ffi_type *zend_ffi_remember_type(zend_ffi_type *type) /* {{{ */
{
	if (!FFI_G(weak_types)) {
		FFI_G(weak_types) = emalloc(sizeof(HashTable));
		zend_hash_init(FFI_G(weak_types), 0, NULL, zend_ffi_type_hash_dtor, 0);
	}
	// TODO: avoid dups ???
	type->attr |= ZEND_FFI_ATTR_STORED;
	zend_hash_next_index_insert_ptr(FFI_G(weak_types), ZEND_FFI_TYPE_MAKE_OWNED(type));
	return type;
}
/* }}} */

static zend_ffi *zend_ffi_load(const char *filename, bool preload) /* {{{ */
{
	struct stat buf;
	int fd;
	char *code, *code_pos, *scope_name, *lib, *err;
	size_t code_size, scope_name_len;
	zend_ffi *ffi;
	DL_HANDLE handle = NULL;
	zend_ffi_scope *scope = NULL;
	zend_string *name;
	zend_ffi_symbol *sym;
	zend_ffi_tag *tag;
	void *addr;

	if (stat(filename, &buf) != 0) {
		if (preload) {
			zend_error(E_WARNING, "FFI: failed pre-loading '%s', file doesn't exist", filename);
		} else {
			zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s', file doesn't exist", filename);
		}
		return NULL;
	}

	if ((buf.st_mode & S_IFMT) != S_IFREG) {
		if (preload) {
			zend_error(E_WARNING, "FFI: failed pre-loading '%s', not a regular file", filename);
		} else {
			zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s', not a regular file", filename);
		}
		return NULL;
	}

	code_size = buf.st_size;
	code = emalloc(code_size + 1);
	fd = open(filename, O_RDONLY, 0);
	if (fd < 0 || read(fd, code, code_size) != code_size) {
		if (preload) {
			zend_error(E_WARNING, "FFI: Failed pre-loading '%s', cannot read_file", filename);
		} else {
			zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s', cannot read_file", filename);
		}
		efree(code);
		close(fd);
		return NULL;
	}
	close(fd);
	code[code_size] = 0;

	FFI_G(symbols) = NULL;
	FFI_G(tags) = NULL;
	FFI_G(persistent) = preload;
	FFI_G(default_type_attr) = preload ?
		ZEND_FFI_ATTR_STORED | ZEND_FFI_ATTR_PERSISTENT :
		ZEND_FFI_ATTR_STORED;

	scope_name = NULL;
	scope_name_len = 0;
	lib = NULL;
	code_pos = zend_ffi_parse_directives(filename, code, &scope_name, &lib, preload);
	if (!code_pos) {
		efree(code);
		FFI_G(persistent) = 0;
		return NULL;
	}
	code_size -= code_pos - code;

	if (zend_ffi_parse_decl(code_pos, code_size) == FAILURE) {
		if (preload) {
			zend_error(E_WARNING, "FFI: failed pre-loading '%s'", filename);
		} else {
			zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s'", filename);
		}
		goto cleanup;
	}

	if (lib) {
		handle = DL_LOAD(lib);
		if (!handle) {
			if (preload) {
				zend_error(E_WARNING, "FFI: Failed pre-loading '%s'", lib);
			} else {
				err = GET_DL_ERROR();
#ifdef PHP_WIN32
				if (err && err[0]) {
					zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s' (%s)", lib, err);
					php_win32_error_msg_free(err);
				} else {
					zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s' (Unknown reason)", lib);
				}
#else
				zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s' (%s)", lib, err);
				GET_DL_ERROR(); /* free the buffer storing the error */
#endif
			}
			goto cleanup;
		}
#ifdef RTLD_DEFAULT
	} else if (1) {
		// TODO: this might need to be disabled or protected ???
		handle = RTLD_DEFAULT;
#endif
	}

	if (preload) {
		if (!scope_name) {
			scope_name = "C";
		}
		scope_name_len = strlen(scope_name);
		if (FFI_G(scopes)) {
			scope = zend_hash_str_find_ptr(FFI_G(scopes), scope_name, scope_name_len);
		}
	}

	if (FFI_G(symbols)) {
		ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(FFI_G(symbols), name, sym) {
			if (sym->kind == ZEND_FFI_SYM_VAR) {
				addr = DL_FETCH_SYMBOL(handle, ZSTR_VAL(name));
				if (!addr) {
					if (preload) {
						zend_error(E_WARNING, "FFI: failed pre-loading '%s', cannot resolve C variable '%s'", filename, ZSTR_VAL(name));
					} else {
						zend_throw_error(zend_ffi_exception_ce, "Failed resolving C variable '%s'", ZSTR_VAL(name));
					}
					if (lib) {
						DL_UNLOAD(handle);
					}
					goto cleanup;
				}
				sym->addr = addr;
			} else if (sym->kind == ZEND_FFI_SYM_FUNC) {
				zend_string *mangled_name = zend_ffi_mangled_func_name(name, ZEND_FFI_TYPE(sym->type));

				addr = DL_FETCH_SYMBOL(handle, ZSTR_VAL(mangled_name));
				zend_string_release(mangled_name);
				if (!addr) {
					if (preload) {
						zend_error(E_WARNING, "failed pre-loading '%s', cannot resolve C function '%s'", filename, ZSTR_VAL(name));
					} else {
						zend_throw_error(zend_ffi_exception_ce, "Failed resolving C function '%s'", ZSTR_VAL(name));
					}
					if (lib) {
						DL_UNLOAD(handle);
					}
					goto cleanup;
				}
				sym->addr = addr;
			}
			if (scope && scope->symbols) {
				zend_ffi_symbol *old_sym = zend_hash_find_ptr(scope->symbols, name);

				if (old_sym) {
					if (zend_ffi_same_symbols(old_sym, sym)) {
						if (ZEND_FFI_TYPE_IS_OWNED(sym->type)
						 && ZEND_FFI_TYPE(old_sym->type) != ZEND_FFI_TYPE(sym->type)) {
							zend_ffi_type *type = ZEND_FFI_TYPE(sym->type);
							zend_ffi_cleanup_type(ZEND_FFI_TYPE(old_sym->type), ZEND_FFI_TYPE(type));
							zend_ffi_type_dtor(type);
						}
					} else {
						zend_error(E_WARNING, "FFI: failed pre-loading '%s', redefinition of '%s'", filename, ZSTR_VAL(name));
						if (lib) {
							DL_UNLOAD(handle);
						}
						goto cleanup;
					}
				}
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (preload) {
		if (scope && scope->tags && FFI_G(tags)) {
			ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(FFI_G(tags), name, tag) {
				zend_ffi_tag *old_tag = zend_hash_find_ptr(scope->tags, name);

				if (old_tag) {
					if (zend_ffi_same_tags(old_tag, tag)) {
						if (ZEND_FFI_TYPE_IS_OWNED(tag->type)
						 && ZEND_FFI_TYPE(old_tag->type) != ZEND_FFI_TYPE(tag->type)) {
							zend_ffi_type *type = ZEND_FFI_TYPE(tag->type);
							zend_ffi_cleanup_type(ZEND_FFI_TYPE(old_tag->type), ZEND_FFI_TYPE(type));
							zend_ffi_type_dtor(type);
						}
					} else {
						zend_error(E_WARNING, "FFI: failed pre-loading '%s', redefinition of '%s %s'", filename, zend_ffi_tag_kind_name[tag->kind], ZSTR_VAL(name));
						if (lib) {
							DL_UNLOAD(handle);
						}
						goto cleanup;
					}
				}
			} ZEND_HASH_FOREACH_END();
		}

		if (!scope) {
			scope = malloc(sizeof(zend_ffi_scope));
			scope->symbols = FFI_G(symbols);
			scope->tags = FFI_G(tags);

			if (!FFI_G(scopes)) {
				FFI_G(scopes) = malloc(sizeof(HashTable));
				zend_hash_init(FFI_G(scopes), 0, NULL, zend_ffi_scope_hash_dtor, 1);
			}

			zend_hash_str_add_ptr(FFI_G(scopes), scope_name, scope_name_len, scope);
		} else {
			if (FFI_G(symbols)) {
				if (!scope->symbols) {
					scope->symbols = FFI_G(symbols);
					FFI_G(symbols) = NULL;
				} else {
					ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(FFI_G(symbols), name, sym) {
						if (!zend_hash_add_ptr(scope->symbols, name, sym)) {
							zend_ffi_type_dtor(sym->type);
							free(sym);
						}
					} ZEND_HASH_FOREACH_END();
					FFI_G(symbols)->pDestructor = NULL;
					zend_hash_destroy(FFI_G(symbols));
				}
			}
			if (FFI_G(tags)) {
				if (!scope->tags) {
					scope->tags = FFI_G(tags);
					FFI_G(tags) = NULL;
				} else {
					ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(FFI_G(tags), name, tag) {
						if (!zend_hash_add_ptr(scope->tags, name, tag)) {
							zend_ffi_type_dtor(tag->type);
							free(tag);
						}
					} ZEND_HASH_FOREACH_END();
					FFI_G(tags)->pDestructor = NULL;
					zend_hash_destroy(FFI_G(tags));
				}
			}
		}

		if (EG(objects_store).object_buckets) {
			ffi = (zend_ffi*)zend_ffi_new(zend_ffi_ce);
		} else {
			ffi = ecalloc(1, sizeof(zend_ffi));
		}
		ffi->symbols = scope->symbols;
		ffi->tags = scope->tags;
		ffi->persistent = 1;
	} else {
		ffi = (zend_ffi*)zend_ffi_new(zend_ffi_ce);
		ffi->lib = handle;
		ffi->symbols = FFI_G(symbols);
		ffi->tags = FFI_G(tags);
	}

	efree(code);
	FFI_G(symbols) = NULL;
	FFI_G(tags) = NULL;
	FFI_G(persistent) = 0;

	return ffi;

cleanup:
	efree(code);
	if (FFI_G(symbols)) {
		zend_hash_destroy(FFI_G(symbols));
		pefree(FFI_G(symbols), preload);
		FFI_G(symbols) = NULL;
	}
	if (FFI_G(tags)) {
		zend_hash_destroy(FFI_G(tags));
		pefree(FFI_G(tags), preload);
		FFI_G(tags) = NULL;
	}
	FFI_G(persistent) = 0;
	return NULL;
}
/* }}} */

ZEND_METHOD(FFI, load) /* {{{ */
{
	zend_string *fn;
	zend_ffi *ffi;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(fn)
	ZEND_PARSE_PARAMETERS_END();

	if (CG(compiler_options) & ZEND_COMPILE_PRELOAD_IN_CHILD) {
		zend_throw_error(zend_ffi_exception_ce, "FFI::load() doesn't work in conjunction with \"opcache.preload_user\". Use \"ffi.preload\" instead.");
		RETURN_THROWS();
	}

	ffi = zend_ffi_load(ZSTR_VAL(fn), (CG(compiler_options) & ZEND_COMPILE_PRELOAD) != 0);

	if (ffi) {
		RETURN_OBJ(&ffi->std);
	}
}
/* }}} */

ZEND_METHOD(FFI, scope) /* {{{ */
{
	zend_string *scope_name;
	zend_ffi_scope *scope = NULL;
	zend_ffi *ffi;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(scope_name)
	ZEND_PARSE_PARAMETERS_END();

	if (FFI_G(scopes)) {
		scope = zend_hash_find_ptr(FFI_G(scopes), scope_name);
	}

	if (!scope) {
		zend_throw_error(zend_ffi_exception_ce, "Failed loading scope '%s'", ZSTR_VAL(scope_name));
		RETURN_THROWS();
	}

	ffi = (zend_ffi*)zend_ffi_new(zend_ffi_ce);

	ffi->symbols = scope->symbols;
	ffi->tags = scope->tags;
	ffi->persistent = 1;

	RETURN_OBJ(&ffi->std);
}
/* }}} */

static void zend_ffi_cleanup_dcl(zend_ffi_dcl *dcl) /* {{{ */
{
	if (dcl) {
		zend_ffi_type_dtor(dcl->type);
		dcl->type = NULL;
	}
}
/* }}} */

static void zend_ffi_throw_parser_error(const char *format, ...) /* {{{ */
{
	va_list va;
	char *message = NULL;

	va_start(va, format);
	zend_vspprintf(&message, 0, format, va);

	if (EG(current_execute_data)) {
		zend_throw_exception(zend_ffi_parser_exception_ce, message, 0);
	} else {
		zend_error(E_WARNING, "FFI Parser: %s", message);
	}

	efree(message);
	va_end(va);
}
/* }}} */

static zend_result zend_ffi_validate_vla(zend_ffi_type *type) /* {{{ */
{
	if (!FFI_G(allow_vla) && (type->attr & ZEND_FFI_ATTR_VLA)) {
		zend_ffi_throw_parser_error("\"[*]\" is not allowed in other than function prototype scope at line %d", FFI_G(line));
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

static zend_result zend_ffi_validate_incomplete_type(zend_ffi_type *type, bool allow_incomplete_tag, bool allow_incomplete_array) /* {{{ */
{
	if (!allow_incomplete_tag && (type->attr & ZEND_FFI_ATTR_INCOMPLETE_TAG)) {
		if (FFI_G(tags)) {
			zend_string *key;
			zend_ffi_tag *tag;

			ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(FFI_G(tags), key, tag) {
				if (ZEND_FFI_TYPE(tag->type) == type) {
					if (type->kind == ZEND_FFI_TYPE_ENUM) {
						zend_ffi_throw_parser_error("Incomplete enum \"%s\" at line %d", ZSTR_VAL(key), FFI_G(line));
					} else if (type->attr & ZEND_FFI_ATTR_UNION) {
						zend_ffi_throw_parser_error("Incomplete union \"%s\" at line %d", ZSTR_VAL(key), FFI_G(line));
					} else {
						zend_ffi_throw_parser_error("Incomplete struct \"%s\" at line %d", ZSTR_VAL(key), FFI_G(line));
					}
					return FAILURE;
				}
			} ZEND_HASH_FOREACH_END();
		}
		if (FFI_G(symbols)) {
			zend_string *key;
			zend_ffi_symbol *sym;

			ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(FFI_G(symbols), key, sym) {
				if (type == ZEND_FFI_TYPE(sym->type)) {
					zend_ffi_throw_parser_error("Incomplete C type %s at line %d", ZSTR_VAL(key), FFI_G(line));
					return FAILURE;
				}
			} ZEND_HASH_FOREACH_END();
		}
		zend_ffi_throw_parser_error("Incomplete type at line %d", FFI_G(line));
		return FAILURE;
	} else if (!allow_incomplete_array && (type->attr & ZEND_FFI_ATTR_INCOMPLETE_ARRAY)) {
		zend_ffi_throw_parser_error("\"[]\" is not allowed at line %d", FFI_G(line));
		return FAILURE;
	} else if (!FFI_G(allow_vla) && (type->attr & ZEND_FFI_ATTR_VLA)) {
		zend_ffi_throw_parser_error("\"[*]\" is not allowed in other than function prototype scope at line %d", FFI_G(line));
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

static zend_result zend_ffi_validate_type(zend_ffi_type *type, bool allow_incomplete_tag, bool allow_incomplete_array) /* {{{ */
{
	if (type->kind == ZEND_FFI_TYPE_VOID) {
		zend_ffi_throw_parser_error("void type is not allowed at line %d", FFI_G(line));
		return FAILURE;
	}
	return zend_ffi_validate_incomplete_type(type, allow_incomplete_tag, allow_incomplete_array);
}
/* }}} */

static zend_result zend_ffi_validate_var_type(zend_ffi_type *type, bool allow_incomplete_array) /* {{{ */
{
	if (type->kind == ZEND_FFI_TYPE_FUNC) {
		zend_ffi_throw_parser_error("function type is not allowed at line %d", FFI_G(line));
		return FAILURE;
	}
	return zend_ffi_validate_type(type, 0, allow_incomplete_array);
}
/* }}} */

void zend_ffi_validate_type_name(zend_ffi_dcl *dcl) /* {{{ */
{
	zend_ffi_finalize_type(dcl);
	if (zend_ffi_validate_var_type(ZEND_FFI_TYPE(dcl->type), 0) == FAILURE) {
		zend_ffi_cleanup_dcl(dcl);
		LONGJMP(FFI_G(bailout), FAILURE);
	}
}
/* }}} */

static bool zend_ffi_subst_type(zend_ffi_type **dcl, zend_ffi_type *type) /* {{{ */
{
	zend_ffi_type *dcl_type;
	zend_ffi_field *field;

	if (*dcl == type) {
		*dcl = ZEND_FFI_TYPE_MAKE_OWNED(type);
		return 1;
	}
	dcl_type = *dcl;
	switch (dcl_type->kind) {
		case ZEND_FFI_TYPE_POINTER:
			return zend_ffi_subst_type(&dcl_type->pointer.type, type);
		case ZEND_FFI_TYPE_ARRAY:
			return zend_ffi_subst_type(&dcl_type->array.type, type);
		case ZEND_FFI_TYPE_FUNC:
			if (zend_ffi_subst_type(&dcl_type->func.ret_type, type)) {
				return 1;
			}
			if (dcl_type->func.args) {
				zval *zv;

				ZEND_HASH_PACKED_FOREACH_VAL(dcl_type->func.args, zv) {
					if (zend_ffi_subst_type((zend_ffi_type**)&Z_PTR_P(zv), type)) {
						return 1;
					}
				} ZEND_HASH_FOREACH_END();
			}
			break;
		case ZEND_FFI_TYPE_STRUCT:
			ZEND_HASH_MAP_FOREACH_PTR(&dcl_type->record.fields, field) {
				if (zend_ffi_subst_type(&field->type, type)) {
					return 1;
				}
			} ZEND_HASH_FOREACH_END();
			break;
		default:
			break;
	}
	return 0;
} /* }}} */

static void zend_ffi_tags_cleanup(zend_ffi_dcl *dcl) /* {{{ */
{
	zend_ffi_tag *tag;
	ZEND_HASH_MAP_FOREACH_PTR(FFI_G(tags), tag) {
		if (ZEND_FFI_TYPE_IS_OWNED(tag->type)) {
			zend_ffi_type *type = ZEND_FFI_TYPE(tag->type);
			zend_ffi_subst_type(&dcl->type, type);
			tag->type = type;
		}
	} ZEND_HASH_FOREACH_END();
	zend_hash_destroy(FFI_G(tags));
	efree(FFI_G(tags));
}
/* }}} */

ZEND_METHOD(FFI, new) /* {{{ */
{
	zend_string *type_def = NULL;
	zend_object *type_obj = NULL;
	zend_ffi_type *type, *type_ptr;
	zend_ffi_cdata *cdata;
	void *ptr;
	bool owned = 1;
	bool persistent = 0;
	bool is_const = 0;
	bool is_static_call = Z_TYPE(EX(This)) != IS_OBJECT;
	zend_ffi_flags flags = ZEND_FFI_FLAG_OWNED;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(type_obj, zend_ffi_ctype_ce, type_def)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(owned)
		Z_PARAM_BOOL(persistent)
	ZEND_PARSE_PARAMETERS_END();

	if (is_static_call) {
		zend_error(E_DEPRECATED, "Calling FFI::new() statically is deprecated");
		if (EG(exception)) {
			RETURN_THROWS();
		}
	}

	if (!owned) {
		flags &= ~ZEND_FFI_FLAG_OWNED;
	}

	if (persistent) {
		flags |= ZEND_FFI_FLAG_PERSISTENT;
	}

	if (type_def) {
		zend_ffi_dcl dcl = ZEND_FFI_ATTR_INIT;

		if (!is_static_call) {
			zend_ffi *ffi = (zend_ffi*)Z_OBJ(EX(This));
			FFI_G(symbols) = ffi->symbols;
			FFI_G(tags) = ffi->tags;
		} else {
			FFI_G(symbols) = NULL;
			FFI_G(tags) = NULL;
		}
		bool clean_symbols = FFI_G(symbols) == NULL;
		bool clean_tags = FFI_G(tags) == NULL;

		FFI_G(default_type_attr) = 0;

		if (zend_ffi_parse_type(ZSTR_VAL(type_def), ZSTR_LEN(type_def), &dcl) == FAILURE) {
			zend_ffi_type_dtor(dcl.type);
			if (clean_tags && FFI_G(tags)) {
				zend_hash_destroy(FFI_G(tags));
				efree(FFI_G(tags));
				FFI_G(tags) = NULL;
			}
			if (clean_symbols && FFI_G(symbols)) {
				zend_hash_destroy(FFI_G(symbols));
				efree(FFI_G(symbols));
				FFI_G(symbols) = NULL;
			}
			return;
		}

		type = ZEND_FFI_TYPE(dcl.type);
		if (dcl.attr & ZEND_FFI_ATTR_CONST) {
			is_const = 1;
		}

		if (clean_tags && FFI_G(tags)) {
			zend_ffi_tags_cleanup(&dcl);
		}
		if (clean_symbols && FFI_G(symbols)) {
			zend_hash_destroy(FFI_G(symbols));
			efree(FFI_G(symbols));
			FFI_G(symbols) = NULL;
		}
		FFI_G(symbols) = NULL;
		FFI_G(tags) = NULL;

		type_ptr = dcl.type;
	} else {
		zend_ffi_ctype *ctype = (zend_ffi_ctype*) type_obj;

		type_ptr = type = ctype->type;
		if (ZEND_FFI_TYPE_IS_OWNED(type)) {
			type = ZEND_FFI_TYPE(type);
			if (!(type->attr & ZEND_FFI_ATTR_STORED)) {
				if (GC_REFCOUNT(&ctype->std) == 1) {
					/* transfer type ownership */
					ctype->type = type;
				} else {
					ctype->type = type_ptr = type = zend_ffi_remember_type(type);
				}
			}
		}
	}

	if (type->size == 0) {
		zend_throw_error(zend_ffi_exception_ce, "Cannot instantiate FFI\\CData of zero size");
		zend_ffi_type_dtor(type_ptr);
		return;
	}

	ptr = pemalloc(type->size, flags & ZEND_FFI_FLAG_PERSISTENT);
	memset(ptr, 0, type->size);

	cdata = (zend_ffi_cdata*)zend_ffi_cdata_new(zend_ffi_cdata_ce);
	if (type->kind < ZEND_FFI_TYPE_POINTER) {
		cdata->std.handlers = &zend_ffi_cdata_value_handlers;
	}
	cdata->type = type_ptr;
	cdata->ptr = ptr;
	cdata->flags = flags;
	if (is_const) {
		cdata->flags |= ZEND_FFI_FLAG_CONST;
	}

	RETURN_OBJ(&cdata->std);
}
/* }}} */

ZEND_METHOD(FFI, free) /* {{{ */
{
	zval *zv;
	zend_ffi_cdata *cdata;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS_EX(zv, zend_ffi_cdata_ce, 0, 1);
	ZEND_PARSE_PARAMETERS_END();

	cdata = (zend_ffi_cdata*)Z_OBJ_P(zv);

	if (ZEND_FFI_TYPE(cdata->type)->kind == ZEND_FFI_TYPE_POINTER) {
		if (!cdata->ptr) {
			zend_throw_error(zend_ffi_exception_ce, "NULL pointer dereference");
			RETURN_THROWS();
		}
		if (cdata->ptr != (void*)&cdata->ptr_holder) {
			pefree(*(void**)cdata->ptr, cdata->flags & ZEND_FFI_FLAG_PERSISTENT);
		} else {
			pefree(cdata->ptr_holder, (cdata->flags & ZEND_FFI_FLAG_PERSISTENT) || !is_zend_ptr(cdata->ptr_holder));
		}
		*(void**)cdata->ptr = NULL;
	} else if (!(cdata->flags & ZEND_FFI_FLAG_OWNED)) {
		pefree(cdata->ptr, cdata->flags & ZEND_FFI_FLAG_PERSISTENT);
		cdata->ptr = NULL;
		cdata->flags &= ~(ZEND_FFI_FLAG_OWNED|ZEND_FFI_FLAG_PERSISTENT);
		cdata->std.handlers = &zend_ffi_cdata_free_handlers;
	} else {
		zend_throw_error(zend_ffi_exception_ce, "free() non a C pointer");
	}
}
/* }}} */

ZEND_METHOD(FFI, cast) /* {{{ */
{
	zend_string *type_def = NULL;
	zend_object *ztype = NULL;
	zend_ffi_type *old_type, *type, *type_ptr;
	zend_ffi_cdata *old_cdata, *cdata;
	bool is_const = 0;
	bool is_static_call = Z_TYPE(EX(This)) != IS_OBJECT;
	zval *zv, *arg;
	void *ptr;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(ztype, zend_ffi_ctype_ce, type_def)
		Z_PARAM_ZVAL(zv)
	ZEND_PARSE_PARAMETERS_END();

	if (is_static_call) {
		zend_error(E_DEPRECATED, "Calling FFI::cast() statically is deprecated");
		if (EG(exception)) {
			RETURN_THROWS();
		}
	}

	arg = zv;
	ZVAL_DEREF(zv);

	if (type_def) {
		zend_ffi_dcl dcl = ZEND_FFI_ATTR_INIT;

		if (!is_static_call) {
			zend_ffi *ffi = (zend_ffi*)Z_OBJ(EX(This));
			FFI_G(symbols) = ffi->symbols;
			FFI_G(tags) = ffi->tags;
		} else {
			FFI_G(symbols) = NULL;
			FFI_G(tags) = NULL;
		}
		bool clean_symbols = FFI_G(symbols) == NULL;
		bool clean_tags = FFI_G(tags) == NULL;

		FFI_G(default_type_attr) = 0;

		if (zend_ffi_parse_type(ZSTR_VAL(type_def), ZSTR_LEN(type_def), &dcl) == FAILURE) {
			zend_ffi_type_dtor(dcl.type);
			if (clean_tags && FFI_G(tags)) {
				zend_hash_destroy(FFI_G(tags));
				efree(FFI_G(tags));
				FFI_G(tags) = NULL;
			}
			if (clean_symbols && FFI_G(symbols)) {
				zend_hash_destroy(FFI_G(symbols));
				efree(FFI_G(symbols));
				FFI_G(symbols) = NULL;
			}
			return;
		}

		type = ZEND_FFI_TYPE(dcl.type);
		if (dcl.attr & ZEND_FFI_ATTR_CONST) {
			is_const = 1;
		}

		if (clean_tags && FFI_G(tags)) {
			zend_ffi_tags_cleanup(&dcl);
		}
		if (clean_symbols && FFI_G(symbols)) {
			zend_hash_destroy(FFI_G(symbols));
			efree(FFI_G(symbols));
			FFI_G(symbols) = NULL;
		}
		FFI_G(symbols) = NULL;
		FFI_G(tags) = NULL;

		type_ptr = dcl.type;
	} else {
		zend_ffi_ctype *ctype = (zend_ffi_ctype*) ztype;

		type_ptr = type = ctype->type;
		if (ZEND_FFI_TYPE_IS_OWNED(type)) {
			type = ZEND_FFI_TYPE(type);
			if (!(type->attr & ZEND_FFI_ATTR_STORED)) {
				if (GC_REFCOUNT(&ctype->std) == 1) {
					/* transfer type ownership */
					ctype->type = type;
				} else {
					ctype->type = type_ptr = type = zend_ffi_remember_type(type);
				}
			}
		}
	}

	if (Z_TYPE_P(zv) != IS_OBJECT || Z_OBJCE_P(zv) != zend_ffi_cdata_ce) {
		if (type->kind < ZEND_FFI_TYPE_POINTER && Z_TYPE_P(zv) < IS_STRING) {
			/* numeric conversion */
			cdata = (zend_ffi_cdata*)zend_ffi_cdata_new(zend_ffi_cdata_ce);
			cdata->std.handlers = &zend_ffi_cdata_value_handlers;
			cdata->type = type_ptr;
			cdata->ptr = emalloc(type->size);
			zend_ffi_zval_to_cdata(cdata->ptr, type, zv);
			cdata->flags = ZEND_FFI_FLAG_OWNED;
			if (is_const) {
				cdata->flags |= ZEND_FFI_FLAG_CONST;
			}
			RETURN_OBJ(&cdata->std);
		} else if (type->kind == ZEND_FFI_TYPE_POINTER && Z_TYPE_P(zv) == IS_LONG) {
			/* number to pointer conversion */
			cdata = (zend_ffi_cdata*)zend_ffi_cdata_new(zend_ffi_cdata_ce);
			cdata->type = type_ptr;
			cdata->ptr = &cdata->ptr_holder;
			cdata->ptr_holder = (void*)(intptr_t)Z_LVAL_P(zv);
			if (is_const) {
				cdata->flags |= ZEND_FFI_FLAG_CONST;
			}
			RETURN_OBJ(&cdata->std);
		} else if (type->kind == ZEND_FFI_TYPE_POINTER && Z_TYPE_P(zv) == IS_NULL) {
			/* null -> pointer */
			cdata = (zend_ffi_cdata*)zend_ffi_cdata_new(zend_ffi_cdata_ce);
			cdata->type = type_ptr;
			cdata->ptr = &cdata->ptr_holder;
			cdata->ptr_holder = NULL;
			if (is_const) {
				cdata->flags |= ZEND_FFI_FLAG_CONST;
			}
			RETURN_OBJ(&cdata->std);
		} else {
			zend_wrong_parameter_class_error(2, "FFI\\CData", zv);
			RETURN_THROWS();
		}
	}

	old_cdata = (zend_ffi_cdata*)Z_OBJ_P(zv);
	old_type = ZEND_FFI_TYPE(old_cdata->type);
	ptr = old_cdata->ptr;

	cdata = (zend_ffi_cdata*)zend_ffi_cdata_new(zend_ffi_cdata_ce);
	if (type->kind < ZEND_FFI_TYPE_POINTER) {
		cdata->std.handlers = &zend_ffi_cdata_value_handlers;
	}
	cdata->type = type_ptr;

	if (old_type->kind == ZEND_FFI_TYPE_POINTER
	 && type->kind != ZEND_FFI_TYPE_POINTER
	 && ZEND_FFI_TYPE(old_type->pointer.type)->kind == ZEND_FFI_TYPE_VOID) {
		/* automatically dereference void* pointers ??? */
		cdata->ptr = *(void**)ptr;
	} else if (old_type->kind == ZEND_FFI_TYPE_ARRAY
	 && type->kind == ZEND_FFI_TYPE_POINTER
	 && zend_ffi_is_compatible_type(ZEND_FFI_TYPE(old_type->array.type), ZEND_FFI_TYPE(type->pointer.type))) {		cdata->ptr = &cdata->ptr_holder;
 		cdata->ptr = &cdata->ptr_holder;
 		cdata->ptr_holder = old_cdata->ptr;
	} else if (old_type->kind == ZEND_FFI_TYPE_POINTER
	 && type->kind == ZEND_FFI_TYPE_ARRAY
	 && zend_ffi_is_compatible_type(ZEND_FFI_TYPE(old_type->pointer.type), ZEND_FFI_TYPE(type->array.type))) {
		cdata->ptr = old_cdata->ptr_holder;
	} else if (type->size > old_type->size) {
		zend_object_release(&cdata->std);
		zend_throw_error(zend_ffi_exception_ce, "attempt to cast to larger type");
		RETURN_THROWS();
	} else if (ptr != &old_cdata->ptr_holder) {
		cdata->ptr = ptr;
	} else {
		cdata->ptr = &cdata->ptr_holder;
		cdata->ptr_holder = old_cdata->ptr_holder;
	}
	if (is_const) {
		cdata->flags |= ZEND_FFI_FLAG_CONST;
	}

	if (old_cdata->flags & ZEND_FFI_FLAG_OWNED) {
		if (GC_REFCOUNT(&old_cdata->std) == 1 && Z_REFCOUNT_P(arg) == 1) {
			/* transfer ownership */
			old_cdata->flags &= ~ZEND_FFI_FLAG_OWNED;
			cdata->flags |= ZEND_FFI_FLAG_OWNED;
		} else {
			//???zend_throw_error(zend_ffi_exception_ce, "Attempt to cast owned C pointer");
		}
	}

	RETURN_OBJ(&cdata->std);
}
/* }}} */

ZEND_METHOD(FFI, type) /* {{{ */
{
	zend_ffi_ctype *ctype;
	zend_ffi_dcl dcl = ZEND_FFI_ATTR_INIT;
	zend_string *type_def;
	bool is_static_call = Z_TYPE(EX(This)) != IS_OBJECT;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(type_def);
	ZEND_PARSE_PARAMETERS_END();

	if (is_static_call) {
		zend_error(E_DEPRECATED, "Calling FFI::type() statically is deprecated");
		if (EG(exception)) {
			RETURN_THROWS();
		}
	}

	if (!is_static_call) {
		zend_ffi *ffi = (zend_ffi*)Z_OBJ(EX(This));
		FFI_G(symbols) = ffi->symbols;
		FFI_G(tags) = ffi->tags;
	} else {
		FFI_G(symbols) = NULL;
		FFI_G(tags) = NULL;
	}
	bool clean_symbols = FFI_G(symbols) == NULL;
	bool clean_tags = FFI_G(tags) == NULL;

	FFI_G(default_type_attr) = 0;

	if (zend_ffi_parse_type(ZSTR_VAL(type_def), ZSTR_LEN(type_def), &dcl) == FAILURE) {
		zend_ffi_type_dtor(dcl.type);
		if (clean_tags && FFI_G(tags)) {
			zend_hash_destroy(FFI_G(tags));
			efree(FFI_G(tags));
			FFI_G(tags) = NULL;
		}
		if (clean_symbols && FFI_G(symbols)) {
			zend_hash_destroy(FFI_G(symbols));
			efree(FFI_G(symbols));
			FFI_G(symbols) = NULL;
		}
		return;
	}

	if (clean_tags && FFI_G(tags)) {
		zend_ffi_tags_cleanup(&dcl);
	}
	if (clean_symbols && FFI_G(symbols)) {
		zend_hash_destroy(FFI_G(symbols));
		efree(FFI_G(symbols));
		FFI_G(symbols) = NULL;
	}
	FFI_G(symbols) = NULL;
	FFI_G(tags) = NULL;

	ctype = (zend_ffi_ctype*)zend_ffi_ctype_new(zend_ffi_ctype_ce);
	ctype->type = dcl.type;

	RETURN_OBJ(&ctype->std);
}
/* }}} */

ZEND_METHOD(FFI, typeof) /* {{{ */
{
	zval *zv, *arg;
	zend_ffi_ctype *ctype;
	zend_ffi_type *type;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(zv);
	ZEND_PARSE_PARAMETERS_END();

	arg = zv;
	ZVAL_DEREF(zv);
	if (Z_TYPE_P(zv) == IS_OBJECT && Z_OBJCE_P(zv) == zend_ffi_cdata_ce) {
		zend_ffi_cdata *cdata = (zend_ffi_cdata*)Z_OBJ_P(zv);

		type = cdata->type;
		if (ZEND_FFI_TYPE_IS_OWNED(type)) {
			type = ZEND_FFI_TYPE(type);
			if (!(type->attr & ZEND_FFI_ATTR_STORED)) {
				if (GC_REFCOUNT(&cdata->std) == 1 && Z_REFCOUNT_P(arg) == 1) {
					/* transfer type ownership */
					cdata->type = type;
					type = ZEND_FFI_TYPE_MAKE_OWNED(type);
				} else {
					cdata->type = type = zend_ffi_remember_type(type);
				}
			}
		}
	} else {
		zend_wrong_parameter_class_error(1, "FFI\\CData", zv);
		RETURN_THROWS();
	}

	ctype = (zend_ffi_ctype*)zend_ffi_ctype_new(zend_ffi_ctype_ce);
	ctype->type = type;

	RETURN_OBJ(&ctype->std);
}
/* }}} */

ZEND_METHOD(FFI, arrayType) /* {{{ */
{
	zval *ztype;
	zend_ffi_ctype *ctype;
	zend_ffi_type *type;
	HashTable *dims;
	zval *val;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_OBJECT_OF_CLASS(ztype, zend_ffi_ctype_ce)
		Z_PARAM_ARRAY_HT(dims)
	ZEND_PARSE_PARAMETERS_END();

	ctype = (zend_ffi_ctype*)Z_OBJ_P(ztype);
	type = ZEND_FFI_TYPE(ctype->type);

	if (type->kind == ZEND_FFI_TYPE_FUNC) {
		zend_throw_error(zend_ffi_exception_ce, "Array of functions is not allowed");
		RETURN_THROWS();
	} else if (type->kind == ZEND_FFI_TYPE_ARRAY && (type->attr & ZEND_FFI_ATTR_INCOMPLETE_ARRAY)) {
		zend_throw_error(zend_ffi_exception_ce, "Only the leftmost array can be undimensioned");
		RETURN_THROWS();
	} else if (type->kind == ZEND_FFI_TYPE_VOID) {
		zend_throw_error(zend_ffi_exception_ce, "Array of void type is not allowed");
		RETURN_THROWS();
	} else if (type->attr & ZEND_FFI_ATTR_INCOMPLETE_TAG) {
		zend_throw_error(zend_ffi_exception_ce, "Array of incomplete type is not allowed");
		RETURN_THROWS();
	}

	if (ZEND_FFI_TYPE_IS_OWNED(ctype->type)) {
		if (!(type->attr & ZEND_FFI_ATTR_STORED)) {
			if (GC_REFCOUNT(&ctype->std) == 1) {
				/* transfer type ownership */
				ctype->type = type;
				type = ZEND_FFI_TYPE_MAKE_OWNED(type);
			} else {
				ctype->type = type = zend_ffi_remember_type(type);
			}
		}
	}

	ZEND_HASH_REVERSE_FOREACH_VAL(dims, val) {
		zend_long n = zval_get_long(val);
		zend_ffi_type *new_type;

		if (n < 0) {
			zend_throw_error(zend_ffi_exception_ce, "negative array index");
			zend_ffi_type_dtor(type);
			RETURN_THROWS();
		} else if (ZEND_FFI_TYPE(type)->kind == ZEND_FFI_TYPE_ARRAY && (ZEND_FFI_TYPE(type)->attr & ZEND_FFI_ATTR_INCOMPLETE_ARRAY)) {
			zend_throw_error(zend_ffi_exception_ce, "only the leftmost array can be undimensioned");
			zend_ffi_type_dtor(type);
			RETURN_THROWS();
		}

		new_type = emalloc(sizeof(zend_ffi_type));
		new_type->kind = ZEND_FFI_TYPE_ARRAY;
		new_type->attr = 0;
		new_type->size = n * ZEND_FFI_TYPE(type)->size;
		new_type->align = ZEND_FFI_TYPE(type)->align;
		new_type->array.type = type;
		new_type->array.length = n;

		if (n == 0) {
			new_type->attr |= ZEND_FFI_ATTR_INCOMPLETE_ARRAY;
		}

		type = ZEND_FFI_TYPE_MAKE_OWNED(new_type);
	} ZEND_HASH_FOREACH_END();

	ctype = (zend_ffi_ctype*)zend_ffi_ctype_new(zend_ffi_ctype_ce);
	ctype->type = type;

	RETURN_OBJ(&ctype->std);
}
/* }}} */

ZEND_METHOD(FFI, addr) /* {{{ */
{
	zend_ffi_type *type, *new_type;
	zend_ffi_cdata *cdata, *new_cdata;
	zval *zv, *arg;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(zv)
	ZEND_PARSE_PARAMETERS_END();

	arg = zv;
	ZVAL_DEREF(zv);
	if (Z_TYPE_P(zv) != IS_OBJECT || Z_OBJCE_P(zv) != zend_ffi_cdata_ce) {
		zend_wrong_parameter_class_error(1, "FFI\\CData", zv);
		RETURN_THROWS();
	}

	cdata = (zend_ffi_cdata*)Z_OBJ_P(zv);
	type = ZEND_FFI_TYPE(cdata->type);

	if (GC_REFCOUNT(&cdata->std) == 1 && Z_REFCOUNT_P(arg) == 1 && type->kind == ZEND_FFI_TYPE_POINTER
	 && cdata->ptr == &cdata->ptr_holder) {
		zend_throw_error(zend_ffi_exception_ce, "FFI::addr() cannot create a reference to a temporary pointer");
		RETURN_THROWS();
	}

	new_type = emalloc(sizeof(zend_ffi_type));
	new_type->kind = ZEND_FFI_TYPE_POINTER;
	new_type->attr = 0;
	new_type->size = sizeof(void*);
	new_type->align = _Alignof(void*);
	/* life-time (source must relive the resulting pointer) ??? */
	new_type->pointer.type = type;

	new_cdata = (zend_ffi_cdata*)zend_ffi_cdata_new(zend_ffi_cdata_ce);
	new_cdata->type = ZEND_FFI_TYPE_MAKE_OWNED(new_type);
	new_cdata->ptr_holder = cdata->ptr;
	new_cdata->ptr = &new_cdata->ptr_holder;

	if (GC_REFCOUNT(&cdata->std) == 1 && Z_REFCOUNT_P(arg) == 1) {
		if (ZEND_FFI_TYPE_IS_OWNED(cdata->type)) {
			/* transfer type ownership */
			cdata->type = type;
			new_type->pointer.type = ZEND_FFI_TYPE_MAKE_OWNED(type);
		}
		if (cdata->flags & ZEND_FFI_FLAG_OWNED) {
			/* transfer ownership */
			cdata->flags &= ~ZEND_FFI_FLAG_OWNED;
			new_cdata->flags |= ZEND_FFI_FLAG_OWNED;
		}
	}

	RETURN_OBJ(&new_cdata->std);
}
/* }}} */

ZEND_METHOD(FFI, sizeof) /* {{{ */
{
	zval *zv;
	zend_ffi_type *type;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(zv);
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_DEREF(zv);
	if (Z_TYPE_P(zv) == IS_OBJECT && Z_OBJCE_P(zv) == zend_ffi_cdata_ce) {
		zend_ffi_cdata *cdata = (zend_ffi_cdata*)Z_OBJ_P(zv);
		type = ZEND_FFI_TYPE(cdata->type);
	} else if (Z_TYPE_P(zv) == IS_OBJECT && Z_OBJCE_P(zv) == zend_ffi_ctype_ce) {
		zend_ffi_ctype *ctype = (zend_ffi_ctype*)Z_OBJ_P(zv);
		type = ZEND_FFI_TYPE(ctype->type);
	} else {
		zend_wrong_parameter_class_error(1, "FFI\\CData or FFI\\CType", zv);
		RETURN_THROWS();
	}

	RETURN_LONG(type->size);
}
/* }}} */

ZEND_METHOD(FFI, alignof) /* {{{ */
{
	zval *zv;
	zend_ffi_type *type;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(zv);
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_DEREF(zv);
	if (Z_TYPE_P(zv) == IS_OBJECT && Z_OBJCE_P(zv) == zend_ffi_cdata_ce) {
		zend_ffi_cdata *cdata = (zend_ffi_cdata*)Z_OBJ_P(zv);
		type = ZEND_FFI_TYPE(cdata->type);
	} else if (Z_TYPE_P(zv) == IS_OBJECT && Z_OBJCE_P(zv) == zend_ffi_ctype_ce) {
		zend_ffi_ctype *ctype = (zend_ffi_ctype*)Z_OBJ_P(zv);
		type = ZEND_FFI_TYPE(ctype->type);
	} else {
		zend_wrong_parameter_class_error(1, "FFI\\CData or FFI\\CType", zv);
		RETURN_THROWS();
	}

	RETURN_LONG(type->align);
}
/* }}} */

ZEND_METHOD(FFI, memcpy) /* {{{ */
{
	zval *zv1, *zv2;
	zend_ffi_cdata *cdata1, *cdata2;
	zend_ffi_type *type1, *type2;
	void *ptr1, *ptr2;
	zend_long size;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_OBJECT_OF_CLASS_EX(zv1, zend_ffi_cdata_ce, 0, 1);
		Z_PARAM_ZVAL(zv2)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	cdata1 = (zend_ffi_cdata*)Z_OBJ_P(zv1);
	type1 = ZEND_FFI_TYPE(cdata1->type);
	if (type1->kind == ZEND_FFI_TYPE_POINTER) {
		ptr1 = *(void**)cdata1->ptr;
	} else {
		ptr1 = cdata1->ptr;
		if (type1->kind != ZEND_FFI_TYPE_POINTER && size > type1->size) {
			zend_throw_error(zend_ffi_exception_ce, "Attempt to write over data boundary");
			RETURN_THROWS();
		}
	}

	ZVAL_DEREF(zv2);
	if (Z_TYPE_P(zv2) == IS_STRING) {
		ptr2 = Z_STRVAL_P(zv2);
		if (size > Z_STRLEN_P(zv2)) {
			zend_throw_error(zend_ffi_exception_ce, "Attempt to read over string boundary");
			RETURN_THROWS();
		}
	} else if (Z_TYPE_P(zv2) == IS_OBJECT && Z_OBJCE_P(zv2) == zend_ffi_cdata_ce) {
		cdata2 = (zend_ffi_cdata*)Z_OBJ_P(zv2);
		type2 = ZEND_FFI_TYPE(cdata2->type);
		if (type2->kind == ZEND_FFI_TYPE_POINTER) {
			ptr2 = *(void**)cdata2->ptr;
		} else {
			ptr2 = cdata2->ptr;
			if (type2->kind != ZEND_FFI_TYPE_POINTER && size > type2->size) {
				zend_throw_error(zend_ffi_exception_ce, "Attempt to read over data boundary");
				RETURN_THROWS();
			}
		}
	} else {
		zend_wrong_parameter_class_error(2, "FFI\\CData or string", zv2);
		RETURN_THROWS();
	}

	memcpy(ptr1, ptr2, size);
}
/* }}} */

ZEND_METHOD(FFI, memcmp) /* {{{ */
{
	zval *zv1, *zv2;
	zend_ffi_cdata *cdata1, *cdata2;
	zend_ffi_type *type1, *type2;
	void *ptr1, *ptr2;
	zend_long size;
	int ret;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_ZVAL(zv1);
		Z_PARAM_ZVAL(zv2);
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_DEREF(zv1);
	if (Z_TYPE_P(zv1) == IS_STRING) {
		ptr1 = Z_STRVAL_P(zv1);
		if (size > Z_STRLEN_P(zv1)) {
			zend_throw_error(zend_ffi_exception_ce, "attempt to read over string boundary");
			RETURN_THROWS();
		}
	} else if (Z_TYPE_P(zv1) == IS_OBJECT && Z_OBJCE_P(zv1) == zend_ffi_cdata_ce) {
		cdata1 = (zend_ffi_cdata*)Z_OBJ_P(zv1);
		type1 = ZEND_FFI_TYPE(cdata1->type);
		if (type1->kind == ZEND_FFI_TYPE_POINTER) {
			ptr1 = *(void**)cdata1->ptr;
		} else {
			ptr1 = cdata1->ptr;
			if (type1->kind != ZEND_FFI_TYPE_POINTER && size > type1->size) {
				zend_throw_error(zend_ffi_exception_ce, "attempt to read over data boundary");
				RETURN_THROWS();
			}
		}
	} else {
		zend_wrong_parameter_class_error(1, "FFI\\CData or string", zv1);
		RETURN_THROWS();
	}

	ZVAL_DEREF(zv2);
	if (Z_TYPE_P(zv2) == IS_STRING) {
		ptr2 = Z_STRVAL_P(zv2);
		if (size > Z_STRLEN_P(zv2)) {
			zend_throw_error(zend_ffi_exception_ce, "Attempt to read over string boundary");
			RETURN_THROWS();
		}
	} else if (Z_TYPE_P(zv2) == IS_OBJECT && Z_OBJCE_P(zv2) == zend_ffi_cdata_ce) {
		cdata2 = (zend_ffi_cdata*)Z_OBJ_P(zv2);
		type2 = ZEND_FFI_TYPE(cdata2->type);
		if (type2->kind == ZEND_FFI_TYPE_POINTER) {
			ptr2 = *(void**)cdata2->ptr;
		} else {
			ptr2 = cdata2->ptr;
			if (type2->kind != ZEND_FFI_TYPE_POINTER && size > type2->size) {
				zend_throw_error(zend_ffi_exception_ce, "Attempt to read over data boundary");
				RETURN_THROWS();
			}
		}
	} else {
		zend_wrong_parameter_class_error(2, "FFI\\CData or string", zv2);
		RETURN_THROWS();
	}

	ret = memcmp(ptr1, ptr2, size);
	if (ret == 0) {
		RETVAL_LONG(0);
	} else if (ret < 0) {
		RETVAL_LONG(-1);
	} else {
		RETVAL_LONG(1);
	}
}
/* }}} */

ZEND_METHOD(FFI, memset) /* {{{ */
{
	zval *zv;
	zend_ffi_cdata *cdata;
	zend_ffi_type *type;
	void *ptr;
	zend_long ch, size;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_OBJECT_OF_CLASS_EX(zv, zend_ffi_cdata_ce, 0, 1);
		Z_PARAM_LONG(ch)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	cdata = (zend_ffi_cdata*)Z_OBJ_P(zv);
	type = ZEND_FFI_TYPE(cdata->type);
	if (type->kind == ZEND_FFI_TYPE_POINTER) {
		ptr = *(void**)cdata->ptr;
	} else {
		ptr = cdata->ptr;
		if (type->kind != ZEND_FFI_TYPE_POINTER && size > type->size) {
			zend_throw_error(zend_ffi_exception_ce, "attempt to write over data boundary");
			RETURN_THROWS();
		}
	}

	memset(ptr, ch, size);
}
/* }}} */

ZEND_METHOD(FFI, string) /* {{{ */
{
	zval *zv;
	zend_ffi_cdata *cdata;
	zend_ffi_type *type;
	void *ptr;
	zend_long size;
	bool size_is_null = 1;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJECT_OF_CLASS_EX(zv, zend_ffi_cdata_ce, 0, 1);
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(size, size_is_null)
	ZEND_PARSE_PARAMETERS_END();

	cdata = (zend_ffi_cdata*)Z_OBJ_P(zv);
	type = ZEND_FFI_TYPE(cdata->type);
	if (!size_is_null) {
		if (type->kind == ZEND_FFI_TYPE_POINTER) {
			ptr = *(void**)cdata->ptr;
		} else {
			ptr = cdata->ptr;
			if (type->kind != ZEND_FFI_TYPE_POINTER && size > type->size) {
				zend_throw_error(zend_ffi_exception_ce, "attempt to read over data boundary");
				RETURN_THROWS();
			}
		}
		RETURN_STRINGL((char*)ptr, size);
	} else {
		if (type->kind == ZEND_FFI_TYPE_POINTER && ZEND_FFI_TYPE(type->pointer.type)->kind == ZEND_FFI_TYPE_CHAR) {
			ptr = *(void**)cdata->ptr;
		} else if (type->kind == ZEND_FFI_TYPE_ARRAY && ZEND_FFI_TYPE(type->array.type)->kind == ZEND_FFI_TYPE_CHAR) {
			ptr = cdata->ptr;
		} else {
			zend_throw_error(zend_ffi_exception_ce, "FFI\\Cdata is not a C string");
			RETURN_THROWS();
		}
		RETURN_STRING((char*)ptr);
	}
}
/* }}} */

ZEND_METHOD(FFI, isNull) /* {{{ */
{
	zval *zv;
	zend_ffi_cdata *cdata;
	zend_ffi_type *type;

	ZEND_FFI_VALIDATE_API_RESTRICTION();
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(zv);
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_DEREF(zv);
	if (Z_TYPE_P(zv) != IS_OBJECT || Z_OBJCE_P(zv) != zend_ffi_cdata_ce) {
		zend_wrong_parameter_class_error(1, "FFI\\CData", zv);
		RETURN_THROWS();
	}

	cdata = (zend_ffi_cdata*)Z_OBJ_P(zv);
	type = ZEND_FFI_TYPE(cdata->type);

	if (type->kind != ZEND_FFI_TYPE_POINTER){
		zend_throw_error(zend_ffi_exception_ce, "FFI\\Cdata is not a pointer");
		RETURN_THROWS();
	}

	RETURN_BOOL(*(void**)cdata->ptr == NULL);
}
/* }}} */


ZEND_METHOD(FFI_CType, getName) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	zend_ffi_ctype_name_buf buf;

	buf.start = buf.end = buf.buf + ((MAX_TYPE_NAME_LEN * 3) / 4);
	if (!zend_ffi_ctype_name(&buf, ZEND_FFI_TYPE(ctype->type))) {
		RETURN_STR_COPY(Z_OBJ_P(ZEND_THIS)->ce->name);
	} else {
		size_t len = buf.end - buf.start;
		zend_string *res = zend_string_init(buf.start, len, 0);
		RETURN_STR(res);
	}
}
/* }}} */

ZEND_METHOD(FFI_CType, getKind) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	zend_ffi_type *type;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	type = ZEND_FFI_TYPE(ctype->type);
	RETURN_LONG(type->kind);
}
/* }}} */

ZEND_METHOD(FFI_CType, getSize) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	zend_ffi_type *type;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	type = ZEND_FFI_TYPE(ctype->type);
	RETURN_LONG(type->size);
}
/* }}} */

ZEND_METHOD(FFI_CType, getAlignment) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	zend_ffi_type *type;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	type = ZEND_FFI_TYPE(ctype->type);
	RETURN_LONG(type->align);
}
/* }}} */

ZEND_METHOD(FFI_CType, getAttributes) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	zend_ffi_type *type;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	type = ZEND_FFI_TYPE(ctype->type);
	RETURN_LONG(type->attr);
}
/* }}} */

ZEND_METHOD(FFI_CType, getEnumKind) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	zend_ffi_type *type;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	type = ZEND_FFI_TYPE(ctype->type);
	if (type->kind != ZEND_FFI_TYPE_ENUM) {
		zend_throw_error(zend_ffi_exception_ce, "FFI\\CType is not an enumeration");
		RETURN_THROWS();
	}
	RETURN_LONG(type->enumeration.kind);
}
/* }}} */

ZEND_METHOD(FFI_CType, getArrayElementType) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	zend_ffi_type *type;
	zend_ffi_ctype *ret;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	type = ZEND_FFI_TYPE(ctype->type);
	if (type->kind != ZEND_FFI_TYPE_ARRAY) {
		zend_throw_error(zend_ffi_exception_ce, "FFI\\CType is not an array");
		RETURN_THROWS();
	}

	ret = (zend_ffi_ctype*)zend_ffi_ctype_new(zend_ffi_ctype_ce);
	ret->type = ZEND_FFI_TYPE(type->array.type);
	RETURN_OBJ(&ret->std);
}
/* }}} */

ZEND_METHOD(FFI_CType, getArrayLength) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	zend_ffi_type *type;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	type = ZEND_FFI_TYPE(ctype->type);
	if (type->kind != ZEND_FFI_TYPE_ARRAY) {
		zend_throw_error(zend_ffi_exception_ce, "FFI\\CType is not an array");
		RETURN_THROWS();
	}
	RETURN_LONG(type->array.length);
}
/* }}} */

ZEND_METHOD(FFI_CType, getPointerType) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	zend_ffi_ctype *ret;
	zend_ffi_type *type;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	type = ZEND_FFI_TYPE(ctype->type);
	if (type->kind != ZEND_FFI_TYPE_POINTER) {
		zend_throw_error(zend_ffi_exception_ce, "FFI\\CType is not a pointer");
		RETURN_THROWS();
	}

	ret = (zend_ffi_ctype*)zend_ffi_ctype_new(zend_ffi_ctype_ce);
	ret->type = ZEND_FFI_TYPE(type->pointer.type);
	RETURN_OBJ(&ret->std);
}
/* }}} */

ZEND_METHOD(FFI_CType, getStructFieldNames) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	zend_ffi_type *type;
	HashTable *ht;
	zend_string* name;
	zval zv;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	type = ZEND_FFI_TYPE(ctype->type);
	if (type->kind != ZEND_FFI_TYPE_STRUCT) {
		zend_throw_error(zend_ffi_exception_ce, "FFI\\CType is not a structure");
		RETURN_THROWS();
	}

	ht = zend_new_array(zend_hash_num_elements(&type->record.fields));
	RETVAL_ARR(ht);
	ZEND_HASH_MAP_FOREACH_STR_KEY(&type->record.fields, name) {
		ZVAL_STR_COPY(&zv, name);
		zend_hash_next_index_insert_new(ht, &zv);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

ZEND_METHOD(FFI_CType, getStructFieldOffset) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	zend_ffi_type *type;
	zend_string *name;
	zend_ffi_field *ptr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	type = ZEND_FFI_TYPE(ctype->type);
	if (type->kind != ZEND_FFI_TYPE_STRUCT) {
		zend_throw_error(zend_ffi_exception_ce, "FFI\\CType is not a structure");
		RETURN_THROWS();
	}

	ptr = zend_hash_find_ptr(&type->record.fields, name);
	if (!ptr) {
		zend_throw_error(zend_ffi_exception_ce, "Wrong field name");
		RETURN_THROWS();
	}
	RETURN_LONG(ptr->offset);
}
/* }}} */

ZEND_METHOD(FFI_CType, getStructFieldType) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	zend_ffi_type *type;
	zend_string *name;
	zend_ffi_field *ptr;
	zend_ffi_ctype *ret;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	type = ZEND_FFI_TYPE(ctype->type);
	if (type->kind != ZEND_FFI_TYPE_STRUCT) {
		zend_throw_error(zend_ffi_exception_ce, "FFI\\CType is not a structure");
		RETURN_THROWS();
	}

	ptr = zend_hash_find_ptr(&type->record.fields, name);
	if (!ptr) {
		zend_throw_error(zend_ffi_exception_ce, "Wrong field name");
		RETURN_THROWS();
	}

	ret = (zend_ffi_ctype*)zend_ffi_ctype_new(zend_ffi_ctype_ce);
	ret->type = ZEND_FFI_TYPE(ptr->type);
	RETURN_OBJ(&ret->std);
}
/* }}} */

ZEND_METHOD(FFI_CType, getFuncABI) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	zend_ffi_type *type;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	type = ZEND_FFI_TYPE(ctype->type);
	if (type->kind != ZEND_FFI_TYPE_FUNC) {
		zend_throw_error(zend_ffi_exception_ce, "FFI\\CType is not a function");
		RETURN_THROWS();
	}
	RETURN_LONG(type->func.abi);
}
/* }}} */

ZEND_METHOD(FFI_CType, getFuncReturnType) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	zend_ffi_ctype *ret;
	zend_ffi_type *type;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	type = ZEND_FFI_TYPE(ctype->type);
	if (type->kind != ZEND_FFI_TYPE_FUNC) {
		zend_throw_error(zend_ffi_exception_ce, "FFI\\CType is not a function");
		RETURN_THROWS();
	}

	ret = (zend_ffi_ctype*)zend_ffi_ctype_new(zend_ffi_ctype_ce);
	ret->type = ZEND_FFI_TYPE(type->func.ret_type);
	RETURN_OBJ(&ret->std);
}
/* }}} */

ZEND_METHOD(FFI_CType, getFuncParameterCount) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	zend_ffi_type *type;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	type = ZEND_FFI_TYPE(ctype->type);
	if (type->kind != ZEND_FFI_TYPE_FUNC) {
		zend_throw_error(zend_ffi_exception_ce, "FFI\\CType is not a function");
		RETURN_THROWS();
	}
	RETURN_LONG(type->func.args ? zend_hash_num_elements(type->func.args) : 0);
}
/* }}} */

ZEND_METHOD(FFI_CType, getFuncParameterType) /* {{{ */
{
	zend_ffi_ctype *ctype = (zend_ffi_ctype*)(Z_OBJ_P(ZEND_THIS));
	zend_ffi_type *type, *ptr;
	zend_long n;
	zend_ffi_ctype *ret;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();

	type = ZEND_FFI_TYPE(ctype->type);
	if (type->kind != ZEND_FFI_TYPE_FUNC) {
		zend_throw_error(zend_ffi_exception_ce, "FFI\\CType is not a function");
		RETURN_THROWS();
	}

	if (!type->func.args) {
		zend_throw_error(zend_ffi_exception_ce, "Wrong argument number");
		RETURN_THROWS();
	}

	ptr = zend_hash_index_find_ptr(type->func.args, n);
	if (!ptr) {
		zend_throw_error(zend_ffi_exception_ce, "Wrong argument number");
		RETURN_THROWS();
	}

	ret = (zend_ffi_ctype*)zend_ffi_ctype_new(zend_ffi_ctype_ce);
	ret->type = ZEND_FFI_TYPE(ptr);
	RETURN_OBJ(&ret->std);
}
/* }}} */

static char *zend_ffi_parse_directives(const char *filename, char *code_pos, char **scope_name, char **lib, bool preload) /* {{{ */
{
	char *p;

	*scope_name = NULL;
	*lib = NULL;
	while (*code_pos == '#') {
		if (strncmp(code_pos, "#define FFI_SCOPE", sizeof("#define FFI_SCOPE") - 1) == 0
		 && (code_pos[sizeof("#define FFI_SCOPE") - 1] == ' '
		  || code_pos[sizeof("#define FFI_SCOPE") - 1] == '\t')) {
			p = code_pos + sizeof("#define FFI_SCOPE");
			while (*p == ' ' || *p == '\t') {
				p++;
			}
			if (*p != '"') {
				if (preload) {
					zend_error(E_WARNING, "FFI: failed pre-loading '%s', bad FFI_SCOPE define", filename);
				} else {
					zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s', bad FFI_SCOPE define", filename);
				}
				return NULL;
			}
			p++;
			if (*scope_name) {
				if (preload) {
					zend_error(E_WARNING, "FFI: failed pre-loading '%s', FFI_SCOPE defined twice", filename);
				} else {
					zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s', FFI_SCOPE defined twice", filename);
				}
				return NULL;
			}
			*scope_name = p;
			while (1) {
				if (*p == '\"') {
					*p = 0;
					p++;
					break;
				} else if (*p <= ' ') {
					if (preload) {
						zend_error(E_WARNING, "FFI: failed pre-loading '%s', bad FFI_SCOPE define", filename);
					} else {
						zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s', bad FFI_SCOPE define", filename);
					}
					return NULL;
				}
				p++;
			}
			while (*p == ' ' || *p == '\t') {
				p++;
			}
			while (*p == '\r' || *p == '\n') {
				p++;
			}
			code_pos = p;
		} else if (strncmp(code_pos, "#define FFI_LIB", sizeof("#define FFI_LIB") - 1) == 0
		 && (code_pos[sizeof("#define FFI_LIB") - 1] == ' '
		  || code_pos[sizeof("#define FFI_LIB") - 1] == '\t')) {
			p = code_pos + sizeof("#define FFI_LIB");
			while (*p == ' ' || *p == '\t') {
				p++;
			}
			if (*p != '"') {
				if (preload) {
					zend_error(E_WARNING, "FFI: failed pre-loading '%s', bad FFI_LIB define", filename);
				} else {
					zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s', bad FFI_LIB define", filename);
				}
				return NULL;
			}
			p++;
			if (*lib) {
				if (preload) {
					zend_error(E_WARNING, "FFI: failed pre-loading '%s', FFI_LIB defined twice", filename);
				} else {
					zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s', FFI_LIB defined twice", filename);
				}
				return NULL;
			}
			*lib = p;
			while (1) {
				if (*p == '\"') {
					*p = 0;
					p++;
					break;
				} else if (*p <= ' ') {
					if (preload) {
						zend_error(E_WARNING, "FFI: failed pre-loading '%s', bad FFI_LIB define", filename);
					} else {
						zend_throw_error(zend_ffi_exception_ce, "Failed loading '%s', bad FFI_LIB define", filename);
					}
					return NULL;
				}
				p++;
			}
			while (*p == ' ' || *p == '\t') {
				p++;
			}
			while (*p == '\r' || *p == '\n') {
				p++;
			}
			code_pos = p;
		} else {
			break;
		}
	}
	return code_pos;
}
/* }}} */

static ZEND_COLD zend_function *zend_fake_get_constructor(zend_object *object) /* {{{ */
{
	zend_throw_error(NULL, "Instantiation of %s is not allowed", ZSTR_VAL(object->ce->name));
	return NULL;
}
/* }}} */

static ZEND_COLD zend_never_inline void zend_bad_array_access(zend_class_entry *ce) /* {{{ */
{
	zend_throw_error(NULL, "Cannot use object of type %s as array", ZSTR_VAL(ce->name));
}
/* }}} */

static ZEND_COLD zval *zend_fake_read_dimension(zend_object *obj, zval *offset, int type, zval *rv) /* {{{ */
{
	zend_bad_array_access(obj->ce);
	return NULL;
}
/* }}} */

static ZEND_COLD void zend_fake_write_dimension(zend_object *obj, zval *offset, zval *value) /* {{{ */
{
	zend_bad_array_access(obj->ce);
}
/* }}} */

static ZEND_COLD int zend_fake_has_dimension(zend_object *obj, zval *offset, int check_empty) /* {{{ */
{
	zend_bad_array_access(obj->ce);
	return 0;
}
/* }}} */

static ZEND_COLD void zend_fake_unset_dimension(zend_object *obj, zval *offset) /* {{{ */
{
	zend_bad_array_access(obj->ce);
}
/* }}} */

static ZEND_COLD zend_never_inline void zend_bad_property_access(zend_class_entry *ce) /* {{{ */
{
	zend_throw_error(NULL, "Cannot access property of object of type %s", ZSTR_VAL(ce->name));
}
/* }}} */

static ZEND_COLD zval *zend_fake_read_property(zend_object *obj, zend_string *member, int type, void **cache_slot, zval *rv) /* {{{ */
{
	zend_bad_property_access(obj->ce);
	return &EG(uninitialized_zval);
}
/* }}} */

static ZEND_COLD zval *zend_fake_write_property(zend_object *obj, zend_string *member, zval *value, void **cache_slot) /* {{{ */
{
	zend_bad_array_access(obj->ce);
	return value;
}
/* }}} */

static ZEND_COLD int zend_fake_has_property(zend_object *obj, zend_string *member, int has_set_exists, void **cache_slot) /* {{{ */
{
	zend_bad_array_access(obj->ce);
	return 0;
}
/* }}} */

static ZEND_COLD void zend_fake_unset_property(zend_object *obj, zend_string *member, void **cache_slot) /* {{{ */
{
	zend_bad_array_access(obj->ce);
}
/* }}} */

static zval *zend_fake_get_property_ptr_ptr(zend_object *obj, zend_string *member, int type, void **cache_slot) /* {{{ */
{
	return NULL;
}
/* }}} */

static ZEND_COLD zend_function *zend_fake_get_method(zend_object **obj_ptr, zend_string *method_name, const zval *key) /* {{{ */
{
	zend_class_entry *ce = (*obj_ptr)->ce;
	zend_throw_error(NULL, "Object of type %s does not support method calls", ZSTR_VAL(ce->name));
	return NULL;
}
/* }}} */

static HashTable *zend_fake_get_properties(zend_object *obj) /* {{{ */
{
	return (HashTable*)&zend_empty_array;
}
/* }}} */

static HashTable *zend_fake_get_gc(zend_object *ob, zval **table, int *n) /* {{{ */
{
	*table = NULL;
	*n = 0;
	return NULL;
}
/* }}} */

static zend_result zend_fake_cast_object(zend_object *obj, zval *result, int type)
{
	switch (type) {
		case _IS_BOOL:
			ZVAL_TRUE(result);
			return SUCCESS;
		default:
			return FAILURE;
	}
}

static ZEND_COLD zend_never_inline void zend_ffi_use_after_free(void) /* {{{ */
{
	zend_throw_error(zend_ffi_exception_ce, "Use after free()");
}
/* }}} */

static zend_object *zend_ffi_free_clone_obj(zend_object *obj) /* {{{ */
{
	zend_ffi_use_after_free();
	return NULL;
}
/* }}} */

static ZEND_COLD zval *zend_ffi_free_read_dimension(zend_object *obj, zval *offset, int type, zval *rv) /* {{{ */
{
	zend_ffi_use_after_free();
	return NULL;
}
/* }}} */

static ZEND_COLD void zend_ffi_free_write_dimension(zend_object *obj, zval *offset, zval *value) /* {{{ */
{
	zend_ffi_use_after_free();
}
/* }}} */

static ZEND_COLD int zend_ffi_free_has_dimension(zend_object *obj, zval *offset, int check_empty) /* {{{ */
{
	zend_ffi_use_after_free();
	return 0;
}
/* }}} */

static ZEND_COLD void zend_ffi_free_unset_dimension(zend_object *obj, zval *offset) /* {{{ */
{
	zend_ffi_use_after_free();
}
/* }}} */

static ZEND_COLD zval *zend_ffi_free_read_property(zend_object *obj, zend_string *member, int type, void **cache_slot, zval *rv) /* {{{ */
{
	zend_ffi_use_after_free();
	return &EG(uninitialized_zval);
}
/* }}} */

static ZEND_COLD zval *zend_ffi_free_write_property(zend_object *obj, zend_string *member, zval *value, void **cache_slot) /* {{{ */
{
	zend_ffi_use_after_free();
	return value;
}
/* }}} */

static ZEND_COLD int zend_ffi_free_has_property(zend_object *obj, zend_string *member, int has_set_exists, void **cache_slot) /* {{{ */
{
	zend_ffi_use_after_free();
	return 0;
}
/* }}} */

static ZEND_COLD void zend_ffi_free_unset_property(zend_object *obj, zend_string *member, void **cache_slot) /* {{{ */
{
	zend_ffi_use_after_free();
}
/* }}} */

static HashTable *zend_ffi_free_get_debug_info(zend_object *obj, int *is_temp) /* {{{ */
{
	zend_ffi_use_after_free();
	return NULL;
}
/* }}} */

static ZEND_INI_MH(OnUpdateFFIEnable) /* {{{ */
{
	if (zend_string_equals_literal_ci(new_value, "preload")) {
		FFI_G(restriction) = ZEND_FFI_PRELOAD;
	} else {
		FFI_G(restriction) = (zend_ffi_api_restriction)zend_ini_parse_bool(new_value);
	}
	return SUCCESS;
}
/* }}} */

static ZEND_INI_DISP(zend_ffi_enable_displayer_cb) /* {{{ */
{
	if (FFI_G(restriction) == ZEND_FFI_PRELOAD) {
		ZEND_PUTS("preload");
	} else if (FFI_G(restriction) == ZEND_FFI_ENABLED) {
		ZEND_PUTS("On");
	} else {
		ZEND_PUTS("Off");
	}
}
/* }}} */

ZEND_INI_BEGIN()
	ZEND_INI_ENTRY_EX("ffi.enable", "preload", ZEND_INI_SYSTEM, OnUpdateFFIEnable, zend_ffi_enable_displayer_cb)
	STD_ZEND_INI_ENTRY("ffi.preload", NULL, ZEND_INI_SYSTEM, OnUpdateString, preload, zend_ffi_globals, ffi_globals)
ZEND_INI_END()

static zend_result zend_ffi_preload_glob(const char *filename) /* {{{ */
{
#ifdef HAVE_GLOB
	glob_t globbuf;
	int    ret;
	unsigned int i;

	memset(&globbuf, 0, sizeof(glob_t));

	ret = glob(filename, 0, NULL, &globbuf);
#ifdef GLOB_NOMATCH
	if (ret == GLOB_NOMATCH || !globbuf.gl_pathc) {
#else
	if (!globbuf.gl_pathc) {
#endif
		/* pass */
	} else {
		for(i=0 ; i<globbuf.gl_pathc; i++) {
			zend_ffi *ffi = zend_ffi_load(globbuf.gl_pathv[i], 1);
			if (!ffi) {
				globfree(&globbuf);
				return FAILURE;
			}
			efree(ffi);
		}
		globfree(&globbuf);
	}
#else
	zend_ffi *ffi = zend_ffi_load(filename, 1);
	if (!ffi) {
		return FAILURE;
	}
	efree(ffi);
#endif

	return SUCCESS;
}
/* }}} */

static zend_result zend_ffi_preload(char *preload) /* {{{ */
{
	zend_ffi *ffi;
	char *s = NULL, *e, *filename;
	bool is_glob = 0;

	e = preload;
	while (*e) {
		switch (*e) {
			case ZEND_PATHS_SEPARATOR:
				if (s) {
					filename = estrndup(s, e-s);
					s = NULL;
					if (!is_glob) {
						ffi = zend_ffi_load(filename, 1);
						efree(filename);
						if (!ffi) {
							return FAILURE;
						}
						efree(ffi);
					} else {
						zend_result ret = zend_ffi_preload_glob(filename);

						efree(filename);
						if (ret == FAILURE) {
							return FAILURE;
						}
						is_glob = 0;
					}
				}
				break;
			case '*':
			case '?':
			case '[':
				is_glob = 1;
				break;
			default:
				if (!s) {
					s = e;
				}
				break;
		}
		e++;
	}
	if (s) {
		filename = estrndup(s, e-s);
		if (!is_glob) {
			ffi = zend_ffi_load(filename, 1);
			efree(filename);
			if (!ffi) {
				return FAILURE;
			}
			efree(ffi);
		} else {
			zend_result ret = zend_ffi_preload_glob(filename);
			efree(filename);
			if (ret == FAILURE) {
				return FAILURE;
			}
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ ZEND_MINIT_FUNCTION */
ZEND_MINIT_FUNCTION(ffi)
{
	REGISTER_INI_ENTRIES();

	FFI_G(is_cli) = strcmp(sapi_module.name, "cli") == 0;

	zend_ffi_exception_ce = register_class_FFI_Exception(zend_ce_error);

	zend_ffi_parser_exception_ce = register_class_FFI_ParserException(zend_ffi_exception_ce);

	zend_ffi_ce = register_class_FFI();
	zend_ffi_ce->create_object = zend_ffi_new;
	zend_ffi_ce->default_object_handlers = &zend_ffi_handlers;

	memcpy(&zend_ffi_new_fn, zend_hash_str_find_ptr(&zend_ffi_ce->function_table, "new", sizeof("new")-1), sizeof(zend_internal_function));
	zend_ffi_new_fn.fn_flags &= ~ZEND_ACC_STATIC;
	memcpy(&zend_ffi_cast_fn, zend_hash_str_find_ptr(&zend_ffi_ce->function_table, "cast", sizeof("cast")-1), sizeof(zend_internal_function));
	zend_ffi_cast_fn.fn_flags &= ~ZEND_ACC_STATIC;
	memcpy(&zend_ffi_type_fn, zend_hash_str_find_ptr(&zend_ffi_ce->function_table, "type", sizeof("type")-1), sizeof(zend_internal_function));
	zend_ffi_type_fn.fn_flags &= ~ZEND_ACC_STATIC;

	memcpy(&zend_ffi_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	zend_ffi_handlers.get_constructor      = zend_fake_get_constructor;
	zend_ffi_handlers.free_obj             = zend_ffi_free_obj;
	zend_ffi_handlers.clone_obj            = NULL;
	zend_ffi_handlers.read_property        = zend_ffi_read_var;
	zend_ffi_handlers.write_property       = zend_ffi_write_var;
	zend_ffi_handlers.read_dimension       = zend_fake_read_dimension;
	zend_ffi_handlers.write_dimension      = zend_fake_write_dimension;
	zend_ffi_handlers.get_property_ptr_ptr = zend_fake_get_property_ptr_ptr;
	zend_ffi_handlers.has_property         = zend_fake_has_property;
	zend_ffi_handlers.unset_property       = zend_fake_unset_property;
	zend_ffi_handlers.has_dimension        = zend_fake_has_dimension;
	zend_ffi_handlers.unset_dimension      = zend_fake_unset_dimension;
	zend_ffi_handlers.get_method           = zend_ffi_get_func;
	zend_ffi_handlers.compare              = NULL;
	zend_ffi_handlers.cast_object          = zend_fake_cast_object;
	zend_ffi_handlers.get_debug_info       = NULL;
	zend_ffi_handlers.get_closure          = NULL;
	zend_ffi_handlers.get_properties       = zend_fake_get_properties;
	zend_ffi_handlers.get_gc               = zend_fake_get_gc;

	zend_ffi_cdata_ce = register_class_FFI_CData();
	zend_ffi_cdata_ce->create_object = zend_ffi_cdata_new;
	zend_ffi_cdata_ce->default_object_handlers = &zend_ffi_cdata_handlers;
	zend_ffi_cdata_ce->get_iterator = zend_ffi_cdata_get_iterator;

	memcpy(&zend_ffi_cdata_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	zend_ffi_cdata_handlers.get_constructor      = zend_fake_get_constructor;
	zend_ffi_cdata_handlers.free_obj             = zend_ffi_cdata_free_obj;
	zend_ffi_cdata_handlers.clone_obj            = zend_ffi_cdata_clone_obj;
	zend_ffi_cdata_handlers.read_property        = zend_ffi_cdata_read_field;
	zend_ffi_cdata_handlers.write_property       = zend_ffi_cdata_write_field;
	zend_ffi_cdata_handlers.read_dimension       = zend_ffi_cdata_read_dim;
	zend_ffi_cdata_handlers.write_dimension      = zend_ffi_cdata_write_dim;
	zend_ffi_cdata_handlers.get_property_ptr_ptr = zend_fake_get_property_ptr_ptr;
	zend_ffi_cdata_handlers.has_property         = zend_fake_has_property;
	zend_ffi_cdata_handlers.unset_property       = zend_fake_unset_property;
	zend_ffi_cdata_handlers.has_dimension        = zend_fake_has_dimension;
	zend_ffi_cdata_handlers.unset_dimension      = zend_fake_unset_dimension;
	zend_ffi_cdata_handlers.get_method           = zend_fake_get_method;
	zend_ffi_cdata_handlers.get_class_name       = zend_ffi_cdata_get_class_name;
	zend_ffi_cdata_handlers.do_operation         = zend_ffi_cdata_do_operation;
	zend_ffi_cdata_handlers.compare              = zend_ffi_cdata_compare_objects;
	zend_ffi_cdata_handlers.cast_object          = zend_ffi_cdata_cast_object;
	zend_ffi_cdata_handlers.count_elements       = zend_ffi_cdata_count_elements;
	zend_ffi_cdata_handlers.get_debug_info       = zend_ffi_cdata_get_debug_info;
	zend_ffi_cdata_handlers.get_closure          = zend_ffi_cdata_get_closure;
	zend_ffi_cdata_handlers.get_properties       = zend_fake_get_properties;
	zend_ffi_cdata_handlers.get_gc               = zend_fake_get_gc;

	memcpy(&zend_ffi_cdata_value_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	zend_ffi_cdata_value_handlers.get_constructor      = zend_fake_get_constructor;
	zend_ffi_cdata_value_handlers.free_obj             = zend_ffi_cdata_free_obj;
	zend_ffi_cdata_value_handlers.clone_obj            = zend_ffi_cdata_clone_obj;
	zend_ffi_cdata_value_handlers.read_property        = zend_ffi_cdata_get;
	zend_ffi_cdata_value_handlers.write_property       = zend_ffi_cdata_set;
	zend_ffi_cdata_value_handlers.read_dimension       = zend_fake_read_dimension;
	zend_ffi_cdata_value_handlers.write_dimension      = zend_fake_write_dimension;
	zend_ffi_cdata_value_handlers.get_property_ptr_ptr = zend_fake_get_property_ptr_ptr;
	zend_ffi_cdata_value_handlers.has_property         = zend_fake_has_property;
	zend_ffi_cdata_value_handlers.unset_property       = zend_fake_unset_property;
	zend_ffi_cdata_value_handlers.has_dimension        = zend_fake_has_dimension;
	zend_ffi_cdata_value_handlers.unset_dimension      = zend_fake_unset_dimension;
	zend_ffi_cdata_value_handlers.get_method           = zend_fake_get_method;
	zend_ffi_cdata_value_handlers.get_class_name       = zend_ffi_cdata_get_class_name;
	zend_ffi_cdata_value_handlers.compare              = zend_ffi_cdata_compare_objects;
	zend_ffi_cdata_value_handlers.cast_object          = zend_ffi_cdata_cast_object;
	zend_ffi_cdata_value_handlers.count_elements       = NULL;
	zend_ffi_cdata_value_handlers.get_debug_info       = zend_ffi_cdata_get_debug_info;
	zend_ffi_cdata_value_handlers.get_closure          = NULL;
	zend_ffi_cdata_value_handlers.get_properties       = zend_fake_get_properties;
	zend_ffi_cdata_value_handlers.get_gc               = zend_fake_get_gc;

	memcpy(&zend_ffi_cdata_free_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	zend_ffi_cdata_free_handlers.get_constructor      = zend_fake_get_constructor;
	zend_ffi_cdata_free_handlers.free_obj             = zend_ffi_cdata_free_obj;
	zend_ffi_cdata_free_handlers.clone_obj            = zend_ffi_free_clone_obj;
	zend_ffi_cdata_free_handlers.read_property        = zend_ffi_free_read_property;
	zend_ffi_cdata_free_handlers.write_property       = zend_ffi_free_write_property;
	zend_ffi_cdata_free_handlers.read_dimension       = zend_ffi_free_read_dimension;
	zend_ffi_cdata_free_handlers.write_dimension      = zend_ffi_free_write_dimension;
	zend_ffi_cdata_free_handlers.get_property_ptr_ptr = zend_fake_get_property_ptr_ptr;
	zend_ffi_cdata_free_handlers.has_property         = zend_ffi_free_has_property;
	zend_ffi_cdata_free_handlers.unset_property       = zend_ffi_free_unset_property;
	zend_ffi_cdata_free_handlers.has_dimension        = zend_ffi_free_has_dimension;
	zend_ffi_cdata_free_handlers.unset_dimension      = zend_ffi_free_unset_dimension;
	zend_ffi_cdata_free_handlers.get_method           = zend_fake_get_method;
	zend_ffi_cdata_free_handlers.get_class_name       = zend_ffi_cdata_get_class_name;
	zend_ffi_cdata_free_handlers.compare              = zend_ffi_cdata_compare_objects;
	zend_ffi_cdata_free_handlers.cast_object          = zend_fake_cast_object;
	zend_ffi_cdata_free_handlers.count_elements       = NULL;
	zend_ffi_cdata_free_handlers.get_debug_info       = zend_ffi_free_get_debug_info;
	zend_ffi_cdata_free_handlers.get_closure          = NULL;
	zend_ffi_cdata_free_handlers.get_properties       = zend_fake_get_properties;
	zend_ffi_cdata_free_handlers.get_gc               = zend_fake_get_gc;

	zend_ffi_ctype_ce = register_class_FFI_CType();
	zend_ffi_ctype_ce->create_object = zend_ffi_ctype_new;
	zend_ffi_ctype_ce->default_object_handlers = &zend_ffi_ctype_handlers;

	memcpy(&zend_ffi_ctype_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	zend_ffi_ctype_handlers.get_constructor      = zend_fake_get_constructor;
	zend_ffi_ctype_handlers.free_obj             = zend_ffi_ctype_free_obj;
	zend_ffi_ctype_handlers.clone_obj            = NULL;
	zend_ffi_ctype_handlers.read_property        = zend_fake_read_property;
	zend_ffi_ctype_handlers.write_property       = zend_fake_write_property;
	zend_ffi_ctype_handlers.read_dimension       = zend_fake_read_dimension;
	zend_ffi_ctype_handlers.write_dimension      = zend_fake_write_dimension;
	zend_ffi_ctype_handlers.get_property_ptr_ptr = zend_fake_get_property_ptr_ptr;
	zend_ffi_ctype_handlers.has_property         = zend_fake_has_property;
	zend_ffi_ctype_handlers.unset_property       = zend_fake_unset_property;
	zend_ffi_ctype_handlers.has_dimension        = zend_fake_has_dimension;
	zend_ffi_ctype_handlers.unset_dimension      = zend_fake_unset_dimension;
	//zend_ffi_ctype_handlers.get_method           = zend_fake_get_method;
	zend_ffi_ctype_handlers.get_class_name       = zend_ffi_ctype_get_class_name;
	zend_ffi_ctype_handlers.compare              = zend_ffi_ctype_compare_objects;
	zend_ffi_ctype_handlers.cast_object          = zend_fake_cast_object;
	zend_ffi_ctype_handlers.count_elements       = NULL;
	zend_ffi_ctype_handlers.get_debug_info       = zend_ffi_ctype_get_debug_info;
	zend_ffi_ctype_handlers.get_closure          = NULL;
	zend_ffi_ctype_handlers.get_properties       = zend_fake_get_properties;
	zend_ffi_ctype_handlers.get_gc               = zend_fake_get_gc;

	if (FFI_G(preload)) {
		return zend_ffi_preload(FFI_G(preload));
	}

	FFI_G(main_tid) = pthread_self();

	zend_atomic_bool_store_ex(&FFI_G(callback_in_progress), false);
	orig_interrupt_function = zend_interrupt_function;
	zend_interrupt_function = zend_ffi_interrupt_function;

	pthread_mutex_init(&FFI_G(vm_request_lock), NULL);
	pthread_mutex_init(&FFI_G(vm_response_lock), NULL);
	pthread_cond_init(&FFI_G(vm_ack), NULL);
	pthread_cond_init(&FFI_G(vm_unlock), NULL);

	return SUCCESS;
}
/* }}} */

/* {{{ ZEND_RSHUTDOWN_FUNCTION */
ZEND_RSHUTDOWN_FUNCTION(ffi)
{
	zend_ffi_wait_request_barrier(true);

	if (FFI_G(callbacks)) {
		zend_hash_destroy(FFI_G(callbacks));
		efree(FFI_G(callbacks));
		FFI_G(callbacks) = NULL;
	}
	if (FFI_G(weak_types)) {
#if 0
		fprintf(stderr, "WeakTypes: %d\n", zend_hash_num_elements(FFI_G(weak_types)));
#endif
		zend_hash_destroy(FFI_G(weak_types));
		efree(FFI_G(weak_types));
		FFI_G(weak_types) = NULL;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ ZEND_MINFO_FUNCTION */
ZEND_MINFO_FUNCTION(ffi)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "FFI support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

static const zend_ffi_type zend_ffi_type_void = {.kind=ZEND_FFI_TYPE_VOID, .size=1, .align=1};
static const zend_ffi_type zend_ffi_type_char = {.kind=ZEND_FFI_TYPE_CHAR, .size=1, .align=_Alignof(char)};
static const zend_ffi_type zend_ffi_type_bool = {.kind=ZEND_FFI_TYPE_BOOL, .size=1, .align=_Alignof(uint8_t)};
static const zend_ffi_type zend_ffi_type_sint8 = {.kind=ZEND_FFI_TYPE_SINT8, .size=1, .align=_Alignof(int8_t)};
static const zend_ffi_type zend_ffi_type_uint8 = {.kind=ZEND_FFI_TYPE_UINT8, .size=1, .align=_Alignof(uint8_t)};
static const zend_ffi_type zend_ffi_type_sint16 = {.kind=ZEND_FFI_TYPE_SINT16, .size=2, .align=_Alignof(int16_t)};
static const zend_ffi_type zend_ffi_type_uint16 = {.kind=ZEND_FFI_TYPE_UINT16, .size=2, .align=_Alignof(uint16_t)};
static const zend_ffi_type zend_ffi_type_sint32 = {.kind=ZEND_FFI_TYPE_SINT32, .size=4, .align=_Alignof(int32_t)};
static const zend_ffi_type zend_ffi_type_uint32 = {.kind=ZEND_FFI_TYPE_UINT32, .size=4, .align=_Alignof(uint32_t)};
static const zend_ffi_type zend_ffi_type_sint64 = {.kind=ZEND_FFI_TYPE_SINT64, .size=8, .align=_Alignof(int64_t)};
static const zend_ffi_type zend_ffi_type_uint64 = {.kind=ZEND_FFI_TYPE_UINT64, .size=8, .align=_Alignof(uint64_t)};
static const zend_ffi_type zend_ffi_type_float = {.kind=ZEND_FFI_TYPE_FLOAT, .size=sizeof(float), .align=_Alignof(float)};
static const zend_ffi_type zend_ffi_type_double = {.kind=ZEND_FFI_TYPE_DOUBLE, .size=sizeof(double), .align=_Alignof(double)};

#ifdef HAVE_LONG_DOUBLE
static const zend_ffi_type zend_ffi_type_long_double = {.kind=ZEND_FFI_TYPE_LONGDOUBLE, .size=sizeof(long double), .align=_Alignof(long double)};
#endif

static const zend_ffi_type zend_ffi_type_ptr = {.kind=ZEND_FFI_TYPE_POINTER, .size=sizeof(void*), .align=_Alignof(void*), .pointer.type = (zend_ffi_type*)&zend_ffi_type_void};

static const struct {
	const char *name;
	const zend_ffi_type *type;
} zend_ffi_types[] = {
	{"void",        &zend_ffi_type_void},
	{"char",        &zend_ffi_type_char},
	{"bool",        &zend_ffi_type_bool},
	{"int8_t",      &zend_ffi_type_sint8},
	{"uint8_t",     &zend_ffi_type_uint8},
	{"int16_t",     &zend_ffi_type_sint16},
	{"uint16_t",    &zend_ffi_type_uint16},
	{"int32_t",     &zend_ffi_type_sint32},
	{"uint32_t",    &zend_ffi_type_uint32},
	{"int64_t",     &zend_ffi_type_sint64},
	{"uint64_t",    &zend_ffi_type_uint64},
	{"float",       &zend_ffi_type_float},
	{"double",      &zend_ffi_type_double},
#ifdef HAVE_LONG_DOUBLE
	{"long double", &zend_ffi_type_long_double},
#endif
#if SIZEOF_SIZE_T == 4
	{"uintptr_t",  &zend_ffi_type_uint32},
	{"intptr_t",   &zend_ffi_type_sint32},
	{"size_t",     &zend_ffi_type_uint32},
	{"ssize_t",    &zend_ffi_type_sint32},
	{"ptrdiff_t",  &zend_ffi_type_sint32},
#else
	{"uintptr_t",  &zend_ffi_type_uint64},
	{"intptr_t",   &zend_ffi_type_sint64},
	{"size_t",     &zend_ffi_type_uint64},
	{"ssize_t",    &zend_ffi_type_sint64},
	{"ptrdiff_t",  &zend_ffi_type_sint64},
#endif
#if SIZEOF_OFF_T == 4
	{"off_t",      &zend_ffi_type_sint32},
#else
	{"off_t",      &zend_ffi_type_sint64},
#endif

	{"va_list",           &zend_ffi_type_ptr},
	{"__builtin_va_list", &zend_ffi_type_ptr},
	{"__gnuc_va_list",    &zend_ffi_type_ptr},
};

/* {{{ ZEND_GINIT_FUNCTION */
static ZEND_GINIT_FUNCTION(ffi)
{
	size_t i;

#if defined(COMPILE_DL_FFI) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	memset(ffi_globals, 0, sizeof(*ffi_globals));
	zend_hash_init(&ffi_globals->types, 0, NULL, NULL, 1);
	for (i = 0; i < sizeof(zend_ffi_types)/sizeof(zend_ffi_types[0]); i++) {
		zend_hash_str_add_new_ptr(&ffi_globals->types, zend_ffi_types[i].name, strlen(zend_ffi_types[i].name), (void*)zend_ffi_types[i].type);
	}
}
/* }}} */

/* {{{ ZEND_GINIT_FUNCTION */
static ZEND_GSHUTDOWN_FUNCTION(ffi)
{
	zend_ffi_wait_request_barrier(true);
	if (ffi_globals->scopes) {
		zend_hash_destroy(ffi_globals->scopes);
		free(ffi_globals->scopes);
	}
	zend_hash_destroy(&ffi_globals->types);
}
/* }}} */

/* {{{ ffi_module_entry */
zend_module_entry ffi_module_entry = {
	STANDARD_MODULE_HEADER,
	"FFI",					/* Extension name */
	NULL,					/* zend_function_entry */
	ZEND_MINIT(ffi),		/* ZEND_MINIT - Module initialization */
	NULL,					/* ZEND_MSHUTDOWN - Module shutdown */
	NULL,					/* ZEND_RINIT - Request initialization */
	ZEND_RSHUTDOWN(ffi),	/* ZEND_RSHUTDOWN - Request shutdown */
	ZEND_MINFO(ffi),		/* ZEND_MINFO - Module info */
	PHP_VERSION,			/* Version */
	ZEND_MODULE_GLOBALS(ffi),
	ZEND_GINIT(ffi),
	ZEND_GSHUTDOWN(ffi),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_FFI
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(ffi)
#endif

/* parser callbacks */
void zend_ffi_parser_error(const char *format, ...) /* {{{ */
{
	va_list va;
	char *message = NULL;

	va_start(va, format);
	zend_vspprintf(&message, 0, format, va);

	if (EG(current_execute_data)) {
		zend_throw_exception(zend_ffi_parser_exception_ce, message, 0);
	} else {
		zend_error(E_WARNING, "FFI Parser: %s", message);
	}

	efree(message);
	va_end(va);

	LONGJMP(FFI_G(bailout), FAILURE);
}
/* }}} */

static void zend_ffi_finalize_type(zend_ffi_dcl *dcl) /* {{{ */
{
	if (!dcl->type) {
		switch (dcl->flags & ZEND_FFI_DCL_TYPE_SPECIFIERS) {
			case ZEND_FFI_DCL_VOID:
				dcl->type = (zend_ffi_type*)&zend_ffi_type_void;
				break;
			case ZEND_FFI_DCL_CHAR:
				dcl->type = (zend_ffi_type*)&zend_ffi_type_char;
				break;
			case ZEND_FFI_DCL_CHAR|ZEND_FFI_DCL_SIGNED:
				dcl->type = (zend_ffi_type*)&zend_ffi_type_sint8;
				break;
			case ZEND_FFI_DCL_CHAR|ZEND_FFI_DCL_UNSIGNED:
			case ZEND_FFI_DCL_BOOL:
				dcl->type = (zend_ffi_type*)&zend_ffi_type_uint8;
				break;
			case ZEND_FFI_DCL_SHORT:
			case ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_SIGNED:
			case ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_INT:
			case ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_SIGNED|ZEND_FFI_DCL_INT:
				dcl->type = (zend_ffi_type*)&zend_ffi_type_sint16;
				break;
			case ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_UNSIGNED:
			case ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_UNSIGNED|ZEND_FFI_DCL_INT:
				dcl->type = (zend_ffi_type*)&zend_ffi_type_uint16;
				break;
			case ZEND_FFI_DCL_INT:
			case ZEND_FFI_DCL_SIGNED:
			case ZEND_FFI_DCL_SIGNED|ZEND_FFI_DCL_INT:
				dcl->type = (zend_ffi_type*)&zend_ffi_type_sint32;
				break;
			case ZEND_FFI_DCL_UNSIGNED:
			case ZEND_FFI_DCL_UNSIGNED|ZEND_FFI_DCL_INT:
				dcl->type = (zend_ffi_type*)&zend_ffi_type_uint32;
				break;
			case ZEND_FFI_DCL_LONG:
			case ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_SIGNED:
			case ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_INT:
			case ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_SIGNED|ZEND_FFI_DCL_INT:
				if (sizeof(long) == 4) {
					dcl->type = (zend_ffi_type*)&zend_ffi_type_sint32;
				} else {
					dcl->type = (zend_ffi_type*)&zend_ffi_type_sint64;
				}
				break;
			case ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_UNSIGNED:
			case ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_UNSIGNED|ZEND_FFI_DCL_INT:
				if (sizeof(long) == 4) {
					dcl->type = (zend_ffi_type*)&zend_ffi_type_uint32;
				} else {
					dcl->type = (zend_ffi_type*)&zend_ffi_type_uint64;
				}
				break;
			case ZEND_FFI_DCL_LONG_LONG|ZEND_FFI_DCL_LONG:
			case ZEND_FFI_DCL_LONG_LONG|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_SIGNED:
			case ZEND_FFI_DCL_LONG_LONG|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_INT:
			case ZEND_FFI_DCL_LONG_LONG|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_SIGNED|ZEND_FFI_DCL_INT:
				dcl->type = (zend_ffi_type*)&zend_ffi_type_sint64;
				break;
			case ZEND_FFI_DCL_LONG_LONG|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_UNSIGNED:
			case ZEND_FFI_DCL_LONG_LONG|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_UNSIGNED|ZEND_FFI_DCL_INT:
				dcl->type = (zend_ffi_type*)&zend_ffi_type_uint64;
				break;
			case ZEND_FFI_DCL_FLOAT:
				dcl->type = (zend_ffi_type*)&zend_ffi_type_float;
				break;
			case ZEND_FFI_DCL_DOUBLE:
				dcl->type = (zend_ffi_type*)&zend_ffi_type_double;
				break;
			case ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_DOUBLE:
#ifdef _WIN32
				dcl->type = (zend_ffi_type*)&zend_ffi_type_double;
#else
				dcl->type = (zend_ffi_type*)&zend_ffi_type_long_double;
#endif
				break;
			case ZEND_FFI_DCL_FLOAT|ZEND_FFI_DCL_COMPLEX:
			case ZEND_FFI_DCL_DOUBLE|ZEND_FFI_DCL_COMPLEX:
			case ZEND_FFI_DCL_DOUBLE|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_COMPLEX:
				zend_ffi_parser_error("Unsupported type _Complex at line %d", FFI_G(line));
				break;
			default:
				zend_ffi_parser_error("Unsupported type specifier combination at line %d", FFI_G(line));
				break;
		}
		dcl->flags &= ~ZEND_FFI_DCL_TYPE_SPECIFIERS;
		dcl->flags |= ZEND_FFI_DCL_TYPEDEF_NAME;
	}
}
/* }}} */

bool zend_ffi_is_typedef_name(const char *name, size_t name_len) /* {{{ */
{
	zend_ffi_symbol *sym;
	zend_ffi_type *type;

	if (FFI_G(symbols)) {
		sym = zend_hash_str_find_ptr(FFI_G(symbols), name, name_len);
		if (sym) {
			return (sym->kind == ZEND_FFI_SYM_TYPE);
		}
	}
	type = zend_hash_str_find_ptr(&FFI_G(types), name, name_len);
	if (type) {
		return 1;
	}
	return 0;
}
/* }}} */

void zend_ffi_resolve_typedef(const char *name, size_t name_len, zend_ffi_dcl *dcl) /* {{{ */
{
	zend_ffi_symbol *sym;
	zend_ffi_type *type;

	if (FFI_G(symbols)) {
		sym = zend_hash_str_find_ptr(FFI_G(symbols), name, name_len);
		if (sym && sym->kind == ZEND_FFI_SYM_TYPE) {
			dcl->type = ZEND_FFI_TYPE(sym->type);
			if (sym->is_const) {
				dcl->attr |= ZEND_FFI_ATTR_CONST;
			}
			return;
		}
	}
	type = zend_hash_str_find_ptr(&FFI_G(types), name, name_len);
	if (type) {
		dcl->type = type;
		return;
	}
	zend_ffi_parser_error("Undefined C type \"%.*s\" at line %d", name_len, name, FFI_G(line));
}
/* }}} */

void zend_ffi_resolve_const(const char *name, size_t name_len, zend_ffi_val *val) /* {{{ */
{
	zend_ffi_symbol *sym;

	if (UNEXPECTED(FFI_G(attribute_parsing))) {
		val->kind = ZEND_FFI_VAL_NAME;
		val->str = name;
		val->len = name_len;
		return;
	} else if (FFI_G(symbols)) {
		sym = zend_hash_str_find_ptr(FFI_G(symbols), name, name_len);
		if (sym && sym->kind == ZEND_FFI_SYM_CONST) {
			val->i64 = sym->value;
			switch (sym->type->kind) {
				case ZEND_FFI_TYPE_SINT8:
				case ZEND_FFI_TYPE_SINT16:
				case ZEND_FFI_TYPE_SINT32:
					val->kind = ZEND_FFI_VAL_INT32;
					break;
				case ZEND_FFI_TYPE_SINT64:
					val->kind = ZEND_FFI_VAL_INT64;
					break;
				case ZEND_FFI_TYPE_UINT8:
				case ZEND_FFI_TYPE_UINT16:
				case ZEND_FFI_TYPE_UINT32:
					val->kind = ZEND_FFI_VAL_UINT32;
					break;
				case ZEND_FFI_TYPE_UINT64:
					val->kind = ZEND_FFI_VAL_UINT64;
					break;
				default:
					ZEND_UNREACHABLE();
			}
			return;
		}
	}
	val->kind = ZEND_FFI_VAL_ERROR;
}
/* }}} */

void zend_ffi_make_enum_type(zend_ffi_dcl *dcl) /* {{{ */
{
	zend_ffi_type *type = pemalloc(sizeof(zend_ffi_type), FFI_G(persistent));
	type->kind = ZEND_FFI_TYPE_ENUM;
	type->attr = FFI_G(default_type_attr) | (dcl->attr & ZEND_FFI_ENUM_ATTRS);
	type->enumeration.tag_name = NULL;
	if (type->attr & ZEND_FFI_ATTR_PACKED) {
		type->size = zend_ffi_type_uint8.size;
		type->align = zend_ffi_type_uint8.align;
		type->enumeration.kind = ZEND_FFI_TYPE_UINT8;
	} else {
		type->size = zend_ffi_type_uint32.size;
		type->align = zend_ffi_type_uint32.align;
		type->enumeration.kind = ZEND_FFI_TYPE_UINT32;
	}
	dcl->type = ZEND_FFI_TYPE_MAKE_OWNED(type);
	dcl->attr &= ~ZEND_FFI_ENUM_ATTRS;
}
/* }}} */

void zend_ffi_add_enum_val(zend_ffi_dcl *enum_dcl, const char *name, size_t name_len, zend_ffi_val *val, int64_t *min, int64_t *max, int64_t *last) /* {{{ */
{
	zend_ffi_symbol *sym;
	const zend_ffi_type *sym_type;
	int64_t value;
	zend_ffi_type *enum_type = ZEND_FFI_TYPE(enum_dcl->type);
	bool overflow = 0;
	bool is_signed =
		(enum_type->enumeration.kind == ZEND_FFI_TYPE_SINT8 ||
		 enum_type->enumeration.kind == ZEND_FFI_TYPE_SINT16 ||
		 enum_type->enumeration.kind == ZEND_FFI_TYPE_SINT32 ||
		 enum_type->enumeration.kind == ZEND_FFI_TYPE_SINT64);

	ZEND_ASSERT(enum_type && enum_type->kind == ZEND_FFI_TYPE_ENUM);
	if (val->kind == ZEND_FFI_VAL_EMPTY) {
		if (is_signed) {
			if (*last == 0x7FFFFFFFFFFFFFFFLL) {
				overflow = 1;
			}
		} else {
			if ((*min != 0 || *max != 0)
			 && (uint64_t)*last == 0xFFFFFFFFFFFFFFFFULL) {
				overflow = 1;
			}
		}
		value = *last + 1;
	} else if (val->kind == ZEND_FFI_VAL_CHAR) {
		if (!is_signed && val->ch < 0) {
			if ((uint64_t)*max > 0x7FFFFFFFFFFFFFFFULL) {
				overflow = 1;
			} else {
				is_signed = 1;
			}
		}
		value = val->ch;
	} else if (val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) {
		if (!is_signed && val->i64 < 0) {
			if ((uint64_t)*max > 0x7FFFFFFFFFFFFFFFULL) {
				overflow = 1;
			} else {
				is_signed = 1;
			}
		}
		value = val->i64;
	} else if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64) {
		if (is_signed && val->u64 > 0x7FFFFFFFFFFFFFFFULL) {
			overflow = 1;
		}
		value = val->u64;
	} else {
		zend_ffi_parser_error("Enumerator value \"%.*s\" must be an integer at line %d", name_len, name, FFI_G(line));
		return;
	}

	if (overflow) {
		zend_ffi_parser_error("Overflow in enumeration values \"%.*s\" at line %d", name_len, name, FFI_G(line));
		return;
	}

	if (is_signed) {
		*min = MIN(*min, value);
		*max = MAX(*max, value);
		if ((enum_type->attr & ZEND_FFI_ATTR_PACKED)
		 && *min >= -0x7FLL-1 && *max <= 0x7FLL) {
			sym_type = &zend_ffi_type_sint8;
		} else if ((enum_type->attr & ZEND_FFI_ATTR_PACKED)
		 && *min >= -0x7FFFLL-1 && *max <= 0x7FFFLL) {
			sym_type = &zend_ffi_type_sint16;
		} else if (*min >= -0x7FFFFFFFLL-1 && *max <= 0x7FFFFFFFLL) {
			sym_type = &zend_ffi_type_sint32;
		} else {
			sym_type = &zend_ffi_type_sint64;
		}
	} else {
		*min = MIN((uint64_t)*min, (uint64_t)value);
		*max = MAX((uint64_t)*max, (uint64_t)value);
		if ((enum_type->attr & ZEND_FFI_ATTR_PACKED)
		 && (uint64_t)*max <= 0xFFULL) {
			sym_type = &zend_ffi_type_uint8;
		} else if ((enum_type->attr & ZEND_FFI_ATTR_PACKED)
		 && (uint64_t)*max <= 0xFFFFULL) {
			sym_type = &zend_ffi_type_uint16;
		} else if ((uint64_t)*max <= 0xFFFFFFFFULL) {
			sym_type = &zend_ffi_type_uint32;
		} else {
			sym_type = &zend_ffi_type_uint64;
		}
	}
	enum_type->enumeration.kind = sym_type->kind;
	enum_type->size = sym_type->size;
	enum_type->align = sym_type->align;
	*last = value;

	if (!FFI_G(symbols)) {
		FFI_G(symbols) = pemalloc(sizeof(HashTable), FFI_G(persistent));
		zend_hash_init(FFI_G(symbols), 0, NULL, FFI_G(persistent) ? zend_ffi_symbol_hash_persistent_dtor : zend_ffi_symbol_hash_dtor, FFI_G(persistent));
	}
	sym = zend_hash_str_find_ptr(FFI_G(symbols), name, name_len);
	if (sym) {
		zend_ffi_parser_error("Redeclaration of \"%.*s\" at line %d", name_len, name, FFI_G(line));
	} else {
		sym = pemalloc(sizeof(zend_ffi_symbol), FFI_G(persistent));
		sym->kind  = ZEND_FFI_SYM_CONST;
		sym->type  = (zend_ffi_type*)sym_type;
		sym->value = value;
		zend_hash_str_add_new_ptr(FFI_G(symbols), name, name_len, sym);
	}
}
/* }}} */

void zend_ffi_make_struct_type(zend_ffi_dcl *dcl) /* {{{ */
{
	zend_ffi_type *type = pemalloc(sizeof(zend_ffi_type), FFI_G(persistent));
	type->kind = ZEND_FFI_TYPE_STRUCT;
	type->attr = FFI_G(default_type_attr) | (dcl->attr & ZEND_FFI_STRUCT_ATTRS);
	type->size = 0;
	type->align = dcl->align > 1 ? dcl->align : 1;
	if (dcl->flags & ZEND_FFI_DCL_UNION) {
		type->attr |= ZEND_FFI_ATTR_UNION;
	}
	dcl->type = ZEND_FFI_TYPE_MAKE_OWNED(type);
	type->record.tag_name = NULL;
	zend_hash_init(&type->record.fields, 0, NULL, FFI_G(persistent) ? zend_ffi_field_hash_persistent_dtor :zend_ffi_field_hash_dtor, FFI_G(persistent));
	dcl->attr &= ~ZEND_FFI_STRUCT_ATTRS;
	dcl->align = 0;
}
/* }}} */

static zend_result zend_ffi_validate_prev_field_type(zend_ffi_type *struct_type) /* {{{ */
{
	if (zend_hash_num_elements(&struct_type->record.fields) > 0) {
		zend_ffi_field *field = NULL;

		ZEND_HASH_MAP_REVERSE_FOREACH_PTR(&struct_type->record.fields, field) {
			break;
		} ZEND_HASH_FOREACH_END();
		if (ZEND_FFI_TYPE(field->type)->attr & ZEND_FFI_ATTR_INCOMPLETE_ARRAY) {
			zend_ffi_throw_parser_error("Flexible array member not at end of struct at line %d", FFI_G(line));
			return FAILURE;
		}
	}
	return SUCCESS;
}
/* }}} */

static zend_result zend_ffi_validate_field_type(zend_ffi_type *type, zend_ffi_type *struct_type) /* {{{ */
{
	if (type == struct_type) {
		zend_ffi_throw_parser_error("Struct/union can't contain an instance of itself at line %d", FFI_G(line));
		return FAILURE;
	} else if (zend_ffi_validate_var_type(type, 1) == FAILURE) {
		return FAILURE;
	} else if (struct_type->attr & ZEND_FFI_ATTR_UNION) {
		if (type->attr & ZEND_FFI_ATTR_INCOMPLETE_ARRAY) {
			zend_ffi_throw_parser_error("Flexible array member in union at line %d", FFI_G(line));
			return FAILURE;
		}
	}
	return zend_ffi_validate_prev_field_type(struct_type);
}
/* }}} */

void zend_ffi_add_field(zend_ffi_dcl *struct_dcl, const char *name, size_t name_len, zend_ffi_dcl *field_dcl) /* {{{ */
{
	zend_ffi_field *field;
	zend_ffi_type *struct_type = ZEND_FFI_TYPE(struct_dcl->type);
	zend_ffi_type *field_type;

	ZEND_ASSERT(struct_type && struct_type->kind == ZEND_FFI_TYPE_STRUCT);
	zend_ffi_finalize_type(field_dcl);
	field_type = ZEND_FFI_TYPE(field_dcl->type);
	if (zend_ffi_validate_field_type(field_type, struct_type) == FAILURE) {
		zend_ffi_cleanup_dcl(field_dcl);
		LONGJMP(FFI_G(bailout), FAILURE);
	}

	field = pemalloc(sizeof(zend_ffi_field), FFI_G(persistent));
	if (!(struct_type->attr & ZEND_FFI_ATTR_PACKED) && !(field_dcl->attr & ZEND_FFI_ATTR_PACKED)) {
		struct_type->align = MAX(struct_type->align, MAX(field_type->align, field_dcl->align));
	}
	if (struct_type->attr & ZEND_FFI_ATTR_UNION) {
		field->offset = 0;
		struct_type->size = MAX(struct_type->size, field_type->size);
	} else {
		if (!(struct_type->attr & ZEND_FFI_ATTR_PACKED) && !(field_dcl->attr & ZEND_FFI_ATTR_PACKED)) {
			uint32_t field_align = MAX(field_type->align, field_dcl->align);
			struct_type->size = ((struct_type->size + (field_align - 1)) / field_align) * field_align;
		}
		field->offset = struct_type->size;
		struct_type->size += field_type->size;
	}
	field->type = field_dcl->type;
	field->is_const = (bool)(field_dcl->attr & ZEND_FFI_ATTR_CONST);
	field->is_nested = 0;
	field->first_bit = 0;
	field->bits = 0;
	field_dcl->type = field_type; /* reset "owned" flag */

	if (!zend_hash_str_add_ptr(&struct_type->record.fields, name, name_len, field)) {
		zend_ffi_type_dtor(field->type);
		pefree(field, FFI_G(persistent));
		zend_ffi_parser_error("Duplicate field name \"%.*s\" at line %d", name_len, name, FFI_G(line));
	}
}
/* }}} */

void zend_ffi_add_anonymous_field(zend_ffi_dcl *struct_dcl, zend_ffi_dcl *field_dcl) /* {{{ */
{
	zend_ffi_type *struct_type = ZEND_FFI_TYPE(struct_dcl->type);
	zend_ffi_type *field_type;
	zend_ffi_field *field;
	zend_string *key;

	ZEND_ASSERT(struct_type && struct_type->kind == ZEND_FFI_TYPE_STRUCT);
	zend_ffi_finalize_type(field_dcl);
	field_type = ZEND_FFI_TYPE(field_dcl->type);
	if (field_type->kind != ZEND_FFI_TYPE_STRUCT) {
		zend_ffi_cleanup_dcl(field_dcl);
		zend_ffi_parser_error("Declaration does not declare anything at line %d", FFI_G(line));
		return;
	}

	if (!(struct_type->attr & ZEND_FFI_ATTR_PACKED) && !(field_dcl->attr & ZEND_FFI_ATTR_PACKED)) {
		struct_type->align = MAX(struct_type->align, MAX(field_type->align, field_dcl->align));
	}
	if (!(struct_type->attr & ZEND_FFI_ATTR_UNION)) {
		if (zend_ffi_validate_prev_field_type(struct_type) == FAILURE) {
			zend_ffi_cleanup_dcl(field_dcl);
			LONGJMP(FFI_G(bailout), FAILURE);
		}
		if (!(struct_type->attr & ZEND_FFI_ATTR_PACKED) && !(field_dcl->attr & ZEND_FFI_ATTR_PACKED)) {
			uint32_t field_align = MAX(field_type->align, field_dcl->align);
			struct_type->size = ((struct_type->size + (field_align - 1)) / field_align) * field_align;
		}
	}

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&field_type->record.fields, key, field) {
		zend_ffi_field *new_field = pemalloc(sizeof(zend_ffi_field), FFI_G(persistent));

		if (struct_type->attr & ZEND_FFI_ATTR_UNION) {
			new_field->offset = field->offset;
		} else {
			new_field->offset = struct_type->size + field->offset;
		}
		new_field->type = field->type;
		new_field->is_const = field->is_const;
		new_field->is_nested = 1;
		new_field->first_bit = field->first_bit;
		new_field->bits = field->bits;
		field->type = ZEND_FFI_TYPE(field->type); /* reset "owned" flag */

		if (key) {
			if (!zend_hash_add_ptr(&struct_type->record.fields, key, new_field)) {
				zend_ffi_type_dtor(new_field->type);
				pefree(new_field, FFI_G(persistent));
				zend_ffi_parser_error("Duplicate field name \"%s\" at line %d", ZSTR_VAL(key), FFI_G(line));
				return;
			}
		} else {
			zend_hash_next_index_insert_ptr(&struct_type->record.fields, field);
		}
	} ZEND_HASH_FOREACH_END();

	if (struct_type->attr & ZEND_FFI_ATTR_UNION) {
		struct_type->size = MAX(struct_type->size, field_type->size);
	} else {
		struct_type->size += field_type->size;
	}

	zend_ffi_type_dtor(field_dcl->type);
	field_dcl->type = NULL;
}
/* }}} */

void zend_ffi_add_bit_field(zend_ffi_dcl *struct_dcl, const char *name, size_t name_len, zend_ffi_dcl *field_dcl, zend_ffi_val *bits) /* {{{ */
{
	zend_ffi_type *struct_type = ZEND_FFI_TYPE(struct_dcl->type);
	zend_ffi_type *field_type;
	zend_ffi_field *field;

	ZEND_ASSERT(struct_type && struct_type->kind == ZEND_FFI_TYPE_STRUCT);
	zend_ffi_finalize_type(field_dcl);
	field_type = ZEND_FFI_TYPE(field_dcl->type);
	if (zend_ffi_validate_field_type(field_type, struct_type) == FAILURE) {
		zend_ffi_cleanup_dcl(field_dcl);
		LONGJMP(FFI_G(bailout), FAILURE);
	}

	if (field_type->kind < ZEND_FFI_TYPE_UINT8 || field_type->kind > ZEND_FFI_TYPE_BOOL) {
		zend_ffi_cleanup_dcl(field_dcl);
		zend_ffi_parser_error("Wrong type of bit field \"%.*s\" at line %d", name ? name_len : sizeof("<anonymous>")-1, name ? name : "<anonymous>", FFI_G(line));
	}

	if (bits->kind == ZEND_FFI_VAL_INT32 || bits->kind == ZEND_FFI_VAL_INT64) {
		if (bits->i64 < 0) {
			zend_ffi_cleanup_dcl(field_dcl);
			zend_ffi_parser_error("Negative width in bit-field \"%.*s\" at line %d", name ? name_len : sizeof("<anonymous>")-1, name ? name : "<anonymous>", FFI_G(line));
		} else if (bits->i64 == 0) {
			zend_ffi_cleanup_dcl(field_dcl);
			if (name) {
				zend_ffi_parser_error("Zero width in bit-field \"%.*s\" at line %d", name ? name_len : sizeof("<anonymous>")-1, name ? name : "<anonymous>", FFI_G(line));
			}
			return;
		} else if (bits->i64 > field_type->size * 8) {
			zend_ffi_cleanup_dcl(field_dcl);
			zend_ffi_parser_error("Width of \"%.*s\" exceeds its type at line %d", name ? name_len : sizeof("<anonymous>")-1, name ? name : "<anonymous>", FFI_G(line));
		}
	} else if (bits->kind == ZEND_FFI_VAL_UINT32 || bits->kind == ZEND_FFI_VAL_UINT64) {
		if (bits->u64 == 0) {
			zend_ffi_cleanup_dcl(field_dcl);
			if (name) {
				zend_ffi_parser_error("Zero width in bit-field \"%.*s\" at line %d", name ? name_len : sizeof("<anonymous>")-1, name ? name : "<anonymous>", FFI_G(line));
			}
			return;
		} else if (bits->u64 > field_type->size * 8) {
			zend_ffi_cleanup_dcl(field_dcl);
			zend_ffi_parser_error("Width of \"%.*s\" exceeds its type at line %d", name ? name_len : sizeof("<anonymous>")-1, name ? name : "<anonymous>", FFI_G(line));
		}
	} else {
		zend_ffi_cleanup_dcl(field_dcl);
		zend_ffi_parser_error("Bit field \"%.*s\" width not an integer constant at line %d", name ? name_len : sizeof("<anonymous>")-1, name ? name : "<anonymous>", FFI_G(line));
	}

	field = pemalloc(sizeof(zend_ffi_field), FFI_G(persistent));
	if (!(struct_type->attr & ZEND_FFI_ATTR_PACKED)) {
		struct_type->align = MAX(struct_type->align, sizeof(uint32_t));
	}
	if (struct_type->attr & ZEND_FFI_ATTR_UNION) {
		field->offset = 0;
		field->first_bit = 0;
		field->bits = bits->u64;
		if (struct_type->attr & ZEND_FFI_ATTR_PACKED) {
			struct_type->size = MAX(struct_type->size, (bits->u64 + 7) / 8);
		} else {
			struct_type->size = MAX(struct_type->size, ((bits->u64 + 31) / 32) * 4);
		}
	} else {
		zend_ffi_field *prev_field = NULL;

		if (zend_hash_num_elements(&struct_type->record.fields) > 0) {
			ZEND_HASH_MAP_REVERSE_FOREACH_PTR(&struct_type->record.fields, prev_field) {
				break;
			} ZEND_HASH_FOREACH_END();
		}
		if (prev_field && prev_field->bits) {
			field->offset = prev_field->offset;
			field->first_bit = prev_field->first_bit + prev_field->bits;
			field->bits = bits->u64;
		} else {
			field->offset = struct_type->size;
			field->first_bit = 0;
			field->bits = bits->u64;
		}
		if (struct_type->attr & ZEND_FFI_ATTR_PACKED) {
			struct_type->size = field->offset + ((field->first_bit + field->bits) + 7) / 8;
		} else {
			struct_type->size = field->offset + (((field->first_bit + field->bits) + 31) / 32) * 4;
		}
	}
	field->type = field_dcl->type;
	field->is_const = (bool)(field_dcl->attr & ZEND_FFI_ATTR_CONST);
	field->is_nested = 0;
	field_dcl->type = field_type; /* reset "owned" flag */

	if (name) {
		if (!zend_hash_str_add_ptr(&struct_type->record.fields, name, name_len, field)) {
			zend_ffi_type_dtor(field->type);
			pefree(field, FFI_G(persistent));
			zend_ffi_parser_error("Duplicate field name \"%.*s\" at line %d", name_len, name, FFI_G(line));
		}
	} else {
		zend_hash_next_index_insert_ptr(&struct_type->record.fields, field);
	}
}
/* }}} */

void zend_ffi_adjust_struct_size(zend_ffi_dcl *dcl) /* {{{ */
{
	zend_ffi_type *struct_type = ZEND_FFI_TYPE(dcl->type);

	ZEND_ASSERT(struct_type->kind == ZEND_FFI_TYPE_STRUCT);
	if (dcl->align > struct_type->align) {
		struct_type->align = dcl->align;
	}
	if (!(struct_type->attr & ZEND_FFI_ATTR_PACKED)) {
		struct_type->size = ((struct_type->size + (struct_type->align - 1)) / struct_type->align) * struct_type->align;
	}
	dcl->align = 0;
}
/* }}} */

void zend_ffi_make_pointer_type(zend_ffi_dcl *dcl) /* {{{ */
{
	zend_ffi_type *type = pemalloc(sizeof(zend_ffi_type), FFI_G(persistent));
	type->kind = ZEND_FFI_TYPE_POINTER;
	type->attr = FFI_G(default_type_attr) | (dcl->attr & ZEND_FFI_POINTER_ATTRS);
	type->size = sizeof(void*);
	type->align = _Alignof(void*);
	zend_ffi_finalize_type(dcl);
	if (zend_ffi_validate_vla(ZEND_FFI_TYPE(dcl->type)) == FAILURE) {
		zend_ffi_cleanup_dcl(dcl);
		LONGJMP(FFI_G(bailout), FAILURE);
	}
	type->pointer.type = dcl->type;
	dcl->type = ZEND_FFI_TYPE_MAKE_OWNED(type);
	dcl->flags &= ~ZEND_FFI_DCL_TYPE_QUALIFIERS;
	dcl->attr &= ~ZEND_FFI_POINTER_ATTRS;
	dcl->align = 0;
}
/* }}} */

static zend_result zend_ffi_validate_array_element_type(zend_ffi_type *type) /* {{{ */
{
	if (type->kind == ZEND_FFI_TYPE_FUNC) {
		zend_ffi_throw_parser_error("Array of functions is not allowed at line %d", FFI_G(line));
		return FAILURE;
	} else if (type->kind == ZEND_FFI_TYPE_ARRAY && (type->attr & ZEND_FFI_ATTR_INCOMPLETE_ARRAY)) {
		zend_ffi_throw_parser_error("Only the leftmost array can be undimensioned at line %d", FFI_G(line));
		return FAILURE;
	}
	return zend_ffi_validate_type(type, 0, 1);
}
/* }}} */

void zend_ffi_make_array_type(zend_ffi_dcl *dcl, zend_ffi_val *len) /* {{{ */
{
	int length = 0;
	zend_ffi_type *element_type;
	zend_ffi_type *type;

	zend_ffi_finalize_type(dcl);
	element_type = ZEND_FFI_TYPE(dcl->type);

	if (len->kind == ZEND_FFI_VAL_EMPTY) {
		length = 0;
	} else if (len->kind == ZEND_FFI_VAL_UINT32 || len->kind == ZEND_FFI_VAL_UINT64) {
		length = len->u64;
	} else if (len->kind == ZEND_FFI_VAL_INT32 || len->kind == ZEND_FFI_VAL_INT64) {
		length = len->i64;
	} else if (len->kind == ZEND_FFI_VAL_CHAR) {
		length = len->ch;
	} else {
		zend_ffi_cleanup_dcl(dcl);
		zend_ffi_parser_error("Unsupported array index type at line %d", FFI_G(line));
		return;
	}
	if (length < 0) {
		zend_ffi_cleanup_dcl(dcl);
		zend_ffi_parser_error("Negative array index at line %d", FFI_G(line));
		return;
	}

	if (zend_ffi_validate_array_element_type(element_type) == FAILURE) {
		zend_ffi_cleanup_dcl(dcl);
		LONGJMP(FFI_G(bailout), FAILURE);
	}

	type = pemalloc(sizeof(zend_ffi_type), FFI_G(persistent));
	type->kind = ZEND_FFI_TYPE_ARRAY;
	type->attr = FFI_G(default_type_attr) | (dcl->attr & ZEND_FFI_ARRAY_ATTRS);
	type->size = length * element_type->size;
	type->align = element_type->align;
	type->array.type = dcl->type;
	type->array.length = length;
	dcl->type = ZEND_FFI_TYPE_MAKE_OWNED(type);
	dcl->flags &= ~ZEND_FFI_DCL_TYPE_QUALIFIERS;
	dcl->attr &= ~ZEND_FFI_ARRAY_ATTRS;
	dcl->align = 0;
}
/* }}} */

static zend_result zend_ffi_validate_func_ret_type(zend_ffi_type *type) /* {{{ */
{
	if (type->kind == ZEND_FFI_TYPE_FUNC) {
		zend_ffi_throw_parser_error("Function returning function is not allowed at line %d", FFI_G(line));
		return FAILURE;
	 } else if (type->kind == ZEND_FFI_TYPE_ARRAY) {
		zend_ffi_throw_parser_error("Function returning array is not allowed at line %d", FFI_G(line));
		return FAILURE;
	}
	return zend_ffi_validate_incomplete_type(type, 1, 0);
}
/* }}} */

void zend_ffi_make_func_type(zend_ffi_dcl *dcl, HashTable *args, zend_ffi_dcl *nested_dcl) /* {{{ */
{
	zend_ffi_type *type;
	zend_ffi_type *ret_type;

	zend_ffi_finalize_type(dcl);
	ret_type = ZEND_FFI_TYPE(dcl->type);

	if (args) {
		int no_args = 0;
		zend_ffi_type *arg_type;

		ZEND_HASH_PACKED_FOREACH_PTR(args, arg_type) {
			arg_type = ZEND_FFI_TYPE(arg_type);
			if (arg_type->kind == ZEND_FFI_TYPE_VOID) {
				if (zend_hash_num_elements(args) != 1) {
					zend_ffi_cleanup_dcl(nested_dcl);
					zend_ffi_cleanup_dcl(dcl);
					zend_hash_destroy(args);
					pefree(args, FFI_G(persistent));
					zend_ffi_parser_error("void type is not allowed at line %d", FFI_G(line));
					return;
				} else {
					no_args = 1;
				}
			}
		} ZEND_HASH_FOREACH_END();
		if (no_args) {
			zend_hash_destroy(args);
			pefree(args, FFI_G(persistent));
			args = NULL;
		}
	}

#ifdef HAVE_FFI_VECTORCALL_PARTIAL
	if (dcl->abi == ZEND_FFI_ABI_VECTORCALL && args) {
		zend_ulong i;
		zend_ffi_type *arg_type;

		ZEND_HASH_PACKED_FOREACH_KEY_PTR(args, i, arg_type) {
			arg_type = ZEND_FFI_TYPE(arg_type);
# ifdef _WIN64
			if (i >= 4 && i <= 5 && (arg_type->kind == ZEND_FFI_TYPE_FLOAT || arg_type->kind == ZEND_FFI_TYPE_DOUBLE)) {
# else
			if (i < 6 && (arg_type->kind == ZEND_FFI_TYPE_FLOAT || arg_type->kind == ZEND_FFI_TYPE_DOUBLE)) {
# endif
				zend_ffi_cleanup_dcl(nested_dcl);
				zend_ffi_cleanup_dcl(dcl);
				zend_hash_destroy(args);
				pefree(args, FFI_G(persistent));
				zend_ffi_parser_error("Type float/double is not allowed at position " ZEND_ULONG_FMT " with __vectorcall at line %d", i+1, FFI_G(line));
				return;
			}
		} ZEND_HASH_FOREACH_END();
	}
#endif

	if (zend_ffi_validate_func_ret_type(ret_type) == FAILURE) {
		zend_ffi_cleanup_dcl(nested_dcl);
		zend_ffi_cleanup_dcl(dcl);
		if (args) {
			zend_hash_destroy(args);
			pefree(args, FFI_G(persistent));
		}
		LONGJMP(FFI_G(bailout), FAILURE);
	}

	type = pemalloc(sizeof(zend_ffi_type), FFI_G(persistent));
	type->kind = ZEND_FFI_TYPE_FUNC;
	type->attr = FFI_G(default_type_attr) | (dcl->attr & ZEND_FFI_FUNC_ATTRS);
	type->size = sizeof(void*);
	type->align = 1;
	type->func.ret_type = dcl->type;
	switch (dcl->abi) {
		case ZEND_FFI_ABI_DEFAULT:
		case ZEND_FFI_ABI_CDECL:
			type->func.abi = FFI_DEFAULT_ABI;
			break;
#ifdef HAVE_FFI_FASTCALL
		case ZEND_FFI_ABI_FASTCALL:
			type->func.abi = FFI_FASTCALL;
			break;
#endif
#ifdef HAVE_FFI_THISCALL
		case ZEND_FFI_ABI_THISCALL:
			type->func.abi = FFI_THISCALL;
			break;
#endif
#ifdef HAVE_FFI_STDCALL
		case ZEND_FFI_ABI_STDCALL:
			type->func.abi = FFI_STDCALL;
			break;
#endif
#ifdef HAVE_FFI_PASCAL
		case ZEND_FFI_ABI_PASCAL:
			type->func.abi = FFI_PASCAL;
			break;
#endif
#ifdef HAVE_FFI_REGISTER
		case ZEND_FFI_ABI_REGISTER:
			type->func.abi = FFI_REGISTER;
			break;
#endif
#ifdef HAVE_FFI_MS_CDECL
		case ZEND_FFI_ABI_MS:
			type->func.abi = FFI_MS_CDECL;
			break;
#endif
#ifdef HAVE_FFI_SYSV
		case ZEND_FFI_ABI_SYSV:
			type->func.abi = FFI_SYSV;
			break;
#endif
#ifdef HAVE_FFI_VECTORCALL_PARTIAL
		case ZEND_FFI_ABI_VECTORCALL:
			type->func.abi = FFI_VECTORCALL_PARTIAL;
			break;
#endif
		default:
			type->func.abi = FFI_DEFAULT_ABI;
			zend_ffi_cleanup_dcl(nested_dcl);
			if (args) {
				zend_hash_destroy(args);
				pefree(args, FFI_G(persistent));
			}
			type->func.args = NULL;
			_zend_ffi_type_dtor(type);
			zend_ffi_parser_error("Unsupported calling convention line %d", FFI_G(line));
			break;
	}
	type->func.args = args;
	dcl->type = ZEND_FFI_TYPE_MAKE_OWNED(type);
	dcl->attr &= ~ZEND_FFI_FUNC_ATTRS;
	dcl->align = 0;
	dcl->abi = 0;
}
/* }}} */

void zend_ffi_add_arg(HashTable **args, const char *name, size_t name_len, zend_ffi_dcl *arg_dcl) /* {{{ */
{
	zend_ffi_type *type;

	if (!*args) {
		*args = pemalloc(sizeof(HashTable), FFI_G(persistent));
		zend_hash_init(*args, 0, NULL, zend_ffi_type_hash_dtor, FFI_G(persistent));
	}
	zend_ffi_finalize_type(arg_dcl);
	type = ZEND_FFI_TYPE(arg_dcl->type);
	if (type->kind == ZEND_FFI_TYPE_ARRAY) {
		if (ZEND_FFI_TYPE_IS_OWNED(arg_dcl->type)) {
			type->kind = ZEND_FFI_TYPE_POINTER;
			type->size = sizeof(void*);
		} else {
			zend_ffi_type *new_type = pemalloc(sizeof(zend_ffi_type), FFI_G(persistent));
			new_type->kind = ZEND_FFI_TYPE_POINTER;
			new_type->attr = FFI_G(default_type_attr) | (type->attr & ZEND_FFI_POINTER_ATTRS);
			new_type->size = sizeof(void*);
			new_type->align = _Alignof(void*);
			new_type->pointer.type = ZEND_FFI_TYPE(type->array.type);
			arg_dcl->type = ZEND_FFI_TYPE_MAKE_OWNED(new_type);
		}
	} else if (type->kind == ZEND_FFI_TYPE_FUNC) {
		zend_ffi_type *new_type = pemalloc(sizeof(zend_ffi_type), FFI_G(persistent));
		new_type->kind = ZEND_FFI_TYPE_POINTER;
		new_type->attr = FFI_G(default_type_attr);
		new_type->size = sizeof(void*);
		new_type->align = _Alignof(void*);
		new_type->pointer.type = arg_dcl->type;
		arg_dcl->type = ZEND_FFI_TYPE_MAKE_OWNED(new_type);
	}
	if (zend_ffi_validate_incomplete_type(type, 1, 1) == FAILURE) {
		zend_ffi_cleanup_dcl(arg_dcl);
		zend_hash_destroy(*args);
		pefree(*args, FFI_G(persistent));
		*args = NULL;
		LONGJMP(FFI_G(bailout), FAILURE);
	}
	zend_hash_next_index_insert_ptr(*args, (void*)arg_dcl->type);
}
/* }}} */

void zend_ffi_declare(const char *name, size_t name_len, zend_ffi_dcl *dcl) /* {{{ */
{
	zend_ffi_symbol *sym;

	if (!FFI_G(symbols)) {
		FFI_G(symbols) = pemalloc(sizeof(HashTable), FFI_G(persistent));
		zend_hash_init(FFI_G(symbols), 0, NULL, FFI_G(persistent) ? zend_ffi_symbol_hash_persistent_dtor : zend_ffi_symbol_hash_dtor, FFI_G(persistent));
	}
	zend_ffi_finalize_type(dcl);
	sym = zend_hash_str_find_ptr(FFI_G(symbols), name, name_len);
	if (sym) {
		if ((dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) == ZEND_FFI_DCL_TYPEDEF
		 && sym->kind == ZEND_FFI_SYM_TYPE
		 && zend_ffi_is_same_type(ZEND_FFI_TYPE(sym->type), ZEND_FFI_TYPE(dcl->type))
		 && sym->is_const == (bool)(dcl->attr & ZEND_FFI_ATTR_CONST)) {
			/* allowed redeclaration */
			zend_ffi_type_dtor(dcl->type);
			return;
		} else if ((dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) == 0
		 || (dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) == ZEND_FFI_DCL_EXTERN) {
			zend_ffi_type *type = ZEND_FFI_TYPE(dcl->type);

			if (type->kind == ZEND_FFI_TYPE_FUNC) {
				if (sym->kind == ZEND_FFI_SYM_FUNC
				 && zend_ffi_same_types(ZEND_FFI_TYPE(sym->type), type)) {
					/* allowed redeclaration */
					zend_ffi_type_dtor(dcl->type);
					return;
				}
			} else {
				if (sym->kind == ZEND_FFI_SYM_VAR
				 && zend_ffi_is_same_type(ZEND_FFI_TYPE(sym->type), type)
				 && sym->is_const == (bool)(dcl->attr & ZEND_FFI_ATTR_CONST)) {
					/* allowed redeclaration */
					zend_ffi_type_dtor(dcl->type);
					return;
				}
			}
		}
		zend_ffi_parser_error("Redeclaration of \"%.*s\" at line %d", name_len, name, FFI_G(line));
	} else {
		if ((dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) == ZEND_FFI_DCL_TYPEDEF) {
			if (zend_ffi_validate_vla(ZEND_FFI_TYPE(dcl->type)) == FAILURE) {
				zend_ffi_cleanup_dcl(dcl);
				LONGJMP(FFI_G(bailout), FAILURE);
			}
			if (dcl->align && dcl->align > ZEND_FFI_TYPE(dcl->type)->align) {
				if (ZEND_FFI_TYPE_IS_OWNED(dcl->type)) {
					ZEND_FFI_TYPE(dcl->type)->align = dcl->align;
				} else {
					zend_ffi_type *type = pemalloc(sizeof(zend_ffi_type), FFI_G(persistent));

					memcpy(type, ZEND_FFI_TYPE(dcl->type), sizeof(zend_ffi_type));
					type->attr |= FFI_G(default_type_attr);
					type->align = dcl->align;
					dcl->type = ZEND_FFI_TYPE_MAKE_OWNED(type);
				}
			}
			sym = pemalloc(sizeof(zend_ffi_symbol), FFI_G(persistent));
			sym->kind = ZEND_FFI_SYM_TYPE;
			sym->type = dcl->type;
			sym->is_const = (bool)(dcl->attr & ZEND_FFI_ATTR_CONST);
			dcl->type = ZEND_FFI_TYPE(dcl->type); /* reset "owned" flag */
			zend_hash_str_add_new_ptr(FFI_G(symbols), name, name_len, sym);
		} else {
			zend_ffi_type *type;

			type = ZEND_FFI_TYPE(dcl->type);
			if (zend_ffi_validate_type(type, (dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) == ZEND_FFI_DCL_EXTERN, 1) == FAILURE) {
				zend_ffi_cleanup_dcl(dcl);
				LONGJMP(FFI_G(bailout), FAILURE);
			}
			if ((dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) == 0 ||
			    (dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) == ZEND_FFI_DCL_EXTERN) {
				sym = pemalloc(sizeof(zend_ffi_symbol), FFI_G(persistent));
				sym->kind = (type->kind == ZEND_FFI_TYPE_FUNC) ? ZEND_FFI_SYM_FUNC : ZEND_FFI_SYM_VAR;
				sym->type = dcl->type;
				sym->is_const = (bool)(dcl->attr & ZEND_FFI_ATTR_CONST);
				dcl->type = type; /* reset "owned" flag */
				zend_hash_str_add_new_ptr(FFI_G(symbols), name, name_len, sym);
			} else {
				/* useless declaration */
				zend_ffi_type_dtor(dcl->type);
			}
		}
	}
}
/* }}} */

void zend_ffi_declare_tag(const char *name, size_t name_len, zend_ffi_dcl *dcl, bool incomplete) /* {{{ */
{
	zend_ffi_tag *tag;
	zend_ffi_type *type;

	if (!FFI_G(tags)) {
		FFI_G(tags) = pemalloc(sizeof(HashTable), FFI_G(persistent));
		zend_hash_init(FFI_G(tags), 0, NULL, FFI_G(persistent) ? zend_ffi_tag_hash_persistent_dtor : zend_ffi_tag_hash_dtor, FFI_G(persistent));
	}
	tag = zend_hash_str_find_ptr(FFI_G(tags), name, name_len);
	if (tag) {
		zend_ffi_type *type = ZEND_FFI_TYPE(tag->type);

		if (dcl->flags & ZEND_FFI_DCL_STRUCT) {
			if (tag->kind != ZEND_FFI_TAG_STRUCT) {
				zend_ffi_parser_error("\"%.*s\" defined as wrong kind of tag at line %d", name_len, name, FFI_G(line));
				return;
			} else if (!incomplete && !(type->attr & ZEND_FFI_ATTR_INCOMPLETE_TAG)) {
				zend_ffi_parser_error("Redefinition of \"struct %.*s\" at line %d", name_len, name, FFI_G(line));
				return;
			}
		} else if (dcl->flags & ZEND_FFI_DCL_UNION) {
			if (tag->kind != ZEND_FFI_TAG_UNION) {
				zend_ffi_parser_error("\"%.*s\" defined as wrong kind of tag at line %d", name_len, name, FFI_G(line));
				return;
			} else if (!incomplete && !(type->attr & ZEND_FFI_ATTR_INCOMPLETE_TAG)) {
				zend_ffi_parser_error("Redefinition of \"union %.*s\" at line %d", name_len, name, FFI_G(line));
				return;
			}
		} else if (dcl->flags & ZEND_FFI_DCL_ENUM) {
			if (tag->kind != ZEND_FFI_TAG_ENUM) {
				zend_ffi_parser_error("\"%.*s\" defined as wrong kind of tag at line %d", name_len, name, FFI_G(line));
				return;
			} else if (!incomplete && !(type->attr & ZEND_FFI_ATTR_INCOMPLETE_TAG)) {
				zend_ffi_parser_error("Redefinition of \"enum %.*s\" at line %d", name_len, name, FFI_G(line));
				return;
			}
		} else {
			ZEND_UNREACHABLE();
			return;
		}
		dcl->type = type;
		if (!incomplete) {
			type->attr &= ~ZEND_FFI_ATTR_INCOMPLETE_TAG;
		}
	} else {
		zend_ffi_tag *tag = pemalloc(sizeof(zend_ffi_tag), FFI_G(persistent));
		zend_string *tag_name = zend_string_init(name, name_len, FFI_G(persistent));

		if (dcl->flags & ZEND_FFI_DCL_STRUCT) {
			tag->kind = ZEND_FFI_TAG_STRUCT;
			zend_ffi_make_struct_type(dcl);
			type = ZEND_FFI_TYPE(dcl->type);
			type->record.tag_name = zend_string_copy(tag_name);
		} else if (dcl->flags & ZEND_FFI_DCL_UNION) {
			tag->kind = ZEND_FFI_TAG_UNION;
			zend_ffi_make_struct_type(dcl);
			type = ZEND_FFI_TYPE(dcl->type);
			type->record.tag_name = zend_string_copy(tag_name);
		} else if (dcl->flags & ZEND_FFI_DCL_ENUM) {
			tag->kind = ZEND_FFI_TAG_ENUM;
			zend_ffi_make_enum_type(dcl);
			type = ZEND_FFI_TYPE(dcl->type);
			type->enumeration.tag_name = zend_string_copy(tag_name);
		} else {
			ZEND_UNREACHABLE();
		}
		tag->type = ZEND_FFI_TYPE_MAKE_OWNED(dcl->type);
		dcl->type = ZEND_FFI_TYPE(dcl->type);
		if (incomplete) {
			dcl->type->attr |= ZEND_FFI_ATTR_INCOMPLETE_TAG;
		}
		zend_hash_add_new_ptr(FFI_G(tags), tag_name, tag);
		zend_string_release(tag_name);
	}
}
/* }}} */

void zend_ffi_set_abi(zend_ffi_dcl *dcl, uint16_t abi) /* {{{ */
{
	if (dcl->abi != ZEND_FFI_ABI_DEFAULT) {
		zend_ffi_parser_error("Multiple calling convention specifiers at line %d", FFI_G(line));
	} else {
		dcl->abi = abi;
	}
}
/* }}} */

#define SIMPLE_ATTRIBUTES(_) \
	_(cdecl) \
	_(fastcall) \
	_(thiscall) \
	_(stdcall) \
	_(ms_abi) \
	_(sysv_abi) \
	_(vectorcall) \
	_(aligned) \
	_(packed) \
	_(ms_struct) \
	_(gcc_struct) \
	_(const) \
	_(malloc) \
	_(deprecated) \
	_(nothrow) \
	_(leaf) \
	_(pure) \
	_(noreturn) \
	_(warn_unused_result)

#define ATTR_ID(name)   attr_ ## name,
#define ATTR_NAME(name) {sizeof(#name)-1, #name},

void zend_ffi_add_attribute(zend_ffi_dcl *dcl, const char *name, size_t name_len) /* {{{ */
{
	enum {
		SIMPLE_ATTRIBUTES(ATTR_ID)
		attr_unsupported
	};
	static const struct {
		size_t len;
		const char * const name;
	} names[] = {
		SIMPLE_ATTRIBUTES(ATTR_NAME)
		{0, NULL}
	};
	int id;

	if (name_len > 4
	 && name[0] == '_'
	 && name[1] == '_'
	 && name[name_len-2] == '_'
	 && name[name_len-1] == '_') {
		name += 2;
		name_len -= 4;
	}
	for (id = 0; names[id].len != 0; id++) {
		if (name_len == names[id].len) {
			if (memcmp(name, names[id].name, name_len) == 0) {
				break;
			}
		}
	}
	switch (id) {
		case attr_cdecl:
			zend_ffi_set_abi(dcl, ZEND_FFI_ABI_CDECL);
			break;
		case attr_fastcall:
			zend_ffi_set_abi(dcl, ZEND_FFI_ABI_FASTCALL);
			break;
		case attr_thiscall:
			zend_ffi_set_abi(dcl, ZEND_FFI_ABI_THISCALL);
			break;
		case attr_stdcall:
			zend_ffi_set_abi(dcl, ZEND_FFI_ABI_STDCALL);
			break;
		case attr_ms_abi:
			zend_ffi_set_abi(dcl, ZEND_FFI_ABI_MS);
			break;
		case attr_sysv_abi:
			zend_ffi_set_abi(dcl, ZEND_FFI_ABI_SYSV);
			break;
		case attr_vectorcall:
			zend_ffi_set_abi(dcl, ZEND_FFI_ABI_VECTORCALL);
			break;
		case attr_aligned:
			dcl->align = __BIGGEST_ALIGNMENT__;
			break;
		case attr_packed:
			dcl->attr |= ZEND_FFI_ATTR_PACKED;
			break;
		case attr_ms_struct:
			dcl->attr |= ZEND_FFI_ATTR_MS_STRUCT;
			break;
		case attr_gcc_struct:
			dcl->attr |= ZEND_FFI_ATTR_GCC_STRUCT;
			break;
		case attr_unsupported:
			zend_ffi_parser_error("Unsupported attribute \"%.*s\" at line %d", name_len, name, FFI_G(line));
			break;
		default:
			/* ignore */
			break;
	}
}
/* }}} */

#define VALUE_ATTRIBUTES(_) \
	_(regparam) \
	_(aligned) \
	_(mode) \
	_(nonnull) \
	_(alloc_size) \
	_(format) \
	_(deprecated)

void zend_ffi_add_attribute_value(zend_ffi_dcl *dcl, const char *name, size_t name_len, int n, zend_ffi_val *val) /* {{{ */
{
	enum {
		VALUE_ATTRIBUTES(ATTR_ID)
		attr_unsupported
	};
	static const struct {
		size_t len;
		const char * const name;
	} names[] = {
		VALUE_ATTRIBUTES(ATTR_NAME)
		{0, NULL}
	};
	int id;

	if (name_len > 4
	 && name[0] == '_'
	 && name[1] == '_'
	 && name[name_len-2] == '_'
	 && name[name_len-1] == '_') {
		name += 2;
		name_len -= 4;
	}
	for (id = 0; names[id].len != 0; id++) {
		if (name_len == names[id].len) {
			if (memcmp(name, names[id].name, name_len) == 0) {
				break;
			}
		}
	}
	switch (id) {
		case attr_regparam:
			if (n == 0
			 && (val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_INT64 || val->kind == ZEND_FFI_VAL_UINT64)
			 && val->i64 == 3) {
				zend_ffi_set_abi(dcl, ZEND_FFI_ABI_REGISTER);
			} else {
				zend_ffi_parser_error("Incorrect \"regparam\" value at line %d", FFI_G(line));
			}
			break;
		case attr_aligned:
			if (n == 0
			 && (val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_INT64 || val->kind == ZEND_FFI_VAL_UINT64)
			 && val->i64 > 0 && val->i64 <= 0x80000000 && (val->i64 & (val->i64 - 1)) == 0) {
				dcl->align = val->i64;
			} else {
				zend_ffi_parser_error("Incorrect \"alignment\" value at line %d", FFI_G(line));
			}
			break;
		case attr_mode:
			if (n == 0
			 && (val->kind == ZEND_FFI_VAL_NAME)) {
				const char *str = val->str;
				size_t len = val->len;
				if (len > 4
				 && str[0] == '_'
				 && str[1] == '_'
				 && str[len-2] == '_'
				 && str[len-1] == '_') {
					str += 2;
					len -= 4;
				}
				// TODO: Add support for vector type 'VnXX' ???
				if (len == 2) {
					if (str[1] == 'I') {
						if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_CHAR|ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_INT|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_LONG_LONG|ZEND_FFI_DCL_SIGNED|ZEND_FFI_DCL_UNSIGNED))) {
							/* inappropriate type */
						} else if (str[0] == 'Q') {
							dcl->flags &= ~(ZEND_FFI_DCL_CHAR|ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_INT|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_LONG_LONG);
							dcl->flags |= ZEND_FFI_DCL_CHAR;
							break;
						} else if (str[0] == 'H') {
							dcl->flags &= ~(ZEND_FFI_DCL_CHAR|ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_INT|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_LONG_LONG);
							dcl->flags |= ZEND_FFI_DCL_SHORT;
							break;
						} else if (str[0] == 'S') {
							dcl->flags &= ~(ZEND_FFI_DCL_CHAR|ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_INT|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_LONG_LONG);
							dcl->flags |= ZEND_FFI_DCL_INT;
							break;
						} else if (str[0] == 'D') {
							dcl->flags &= ~(ZEND_FFI_DCL_CHAR|ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_INT|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_LONG_LONG);
							if (sizeof(long) == 8) {
								dcl->flags |= ZEND_FFI_DCL_LONG;
							} else {
								dcl->flags |= ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_LONG_LONG;
							}
							break;
						}
					} else if (str[1] == 'F') {
						if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_FLOAT|ZEND_FFI_DCL_DOUBLE))) {
							/* inappropriate type */
						} else if (str[0] == 'S') {
							dcl->flags &= ~(ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_FLOAT|ZEND_FFI_DCL_DOUBLE);
							dcl->flags |= ZEND_FFI_DCL_FLOAT;
							break;
						} else if (str[0] == 'D') {
							dcl->flags &= ~(ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_FLOAT|ZEND_FFI_DCL_DOUBLE);
							dcl->flags |= ZEND_FFI_DCL_DOUBLE;
							break;
						}
					}
				}
			}
			zend_ffi_parser_error("Unsupported \"mode\" value at line %d", FFI_G(line));
			// TODO: ???
		case attr_unsupported:
			zend_ffi_parser_error("Unsupported attribute \"%.*s\" at line %d", name_len, name, FFI_G(line));
			break;
		default:
			/* ignore */
			break;
	}
}
/* }}} */

void zend_ffi_add_msvc_attribute_value(zend_ffi_dcl *dcl, const char *name, size_t name_len, zend_ffi_val *val) /* {{{ */
{
	if (name_len == sizeof("align")-1 && memcmp(name, "align", sizeof("align")-1) == 0) {
		if ((val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_INT64 || val->kind == ZEND_FFI_VAL_UINT64)
		 && val->i64 > 0 && val->i64 <= 0x80000000 && (val->i64 & (val->i64 - 1)) == 0) {
			dcl->align = val->i64;
		} else {
			zend_ffi_parser_error("Incorrect \"alignment\" value at line %d", FFI_G(line));
		}
	} else {
		/* ignore */
	}
}
/* }}} */

static zend_result zend_ffi_nested_type(zend_ffi_type *type, zend_ffi_type *nested_type) /* {{{ */
{
	nested_type = ZEND_FFI_TYPE(nested_type);
	switch (nested_type->kind) {
		case ZEND_FFI_TYPE_POINTER:
			/* "char" is used as a terminator of nested declaration */
			if (nested_type->pointer.type == &zend_ffi_type_char) {
				nested_type->pointer.type = type;
				return zend_ffi_validate_vla(ZEND_FFI_TYPE(type));
			} else {
				return zend_ffi_nested_type(type, nested_type->pointer.type);
			}
			break;
		case ZEND_FFI_TYPE_ARRAY:
			/* "char" is used as a terminator of nested declaration */
			if (nested_type->array.type == &zend_ffi_type_char) {
				nested_type->array.type = type;
				if (zend_ffi_validate_array_element_type(ZEND_FFI_TYPE(type)) == FAILURE) {
					return FAILURE;
				}
			} else {
				if (zend_ffi_nested_type(type, nested_type->array.type) != SUCCESS) {
					return FAILURE;
				}
			}
			nested_type->size = nested_type->array.length * ZEND_FFI_TYPE(nested_type->array.type)->size;
			nested_type->align = ZEND_FFI_TYPE(nested_type->array.type)->align;
			return SUCCESS;
			break;
		case ZEND_FFI_TYPE_FUNC:
			/* "char" is used as a terminator of nested declaration */
			if (nested_type->func.ret_type == &zend_ffi_type_char) {
				nested_type->func.ret_type = type;
				return zend_ffi_validate_func_ret_type(ZEND_FFI_TYPE(type));
			} else {
				return zend_ffi_nested_type(type, nested_type->func.ret_type);
			}
			break;
		default:
			ZEND_UNREACHABLE();
	}
}
/* }}} */

void zend_ffi_nested_declaration(zend_ffi_dcl *dcl, zend_ffi_dcl *nested_dcl) /* {{{ */
{
	/* "char" is used as a terminator of nested declaration */
	zend_ffi_finalize_type(dcl);
	if (!nested_dcl->type || nested_dcl->type == &zend_ffi_type_char) {
		nested_dcl->type = dcl->type;
	} else {
		if (zend_ffi_nested_type(dcl->type, nested_dcl->type) == FAILURE) {
			zend_ffi_cleanup_dcl(nested_dcl);
			LONGJMP(FFI_G(bailout), FAILURE);
		}
	}
	dcl->type = nested_dcl->type;
}
/* }}} */

void zend_ffi_align_as_type(zend_ffi_dcl *dcl, zend_ffi_dcl *align_dcl) /* {{{ */
{
	zend_ffi_finalize_type(align_dcl);
	dcl->align = MAX(align_dcl->align, ZEND_FFI_TYPE(align_dcl->type)->align);
}
/* }}} */

void zend_ffi_align_as_val(zend_ffi_dcl *dcl, zend_ffi_val *align_val) /* {{{ */
{
	switch (align_val->kind) {
		case ZEND_FFI_VAL_INT32:
		case ZEND_FFI_VAL_UINT32:
			dcl->align = zend_ffi_type_uint32.align;
			break;
		case ZEND_FFI_VAL_INT64:
		case ZEND_FFI_VAL_UINT64:
			dcl->align = zend_ffi_type_uint64.align;
			break;
		case ZEND_FFI_VAL_FLOAT:
			dcl->align = zend_ffi_type_float.align;
			break;
		case ZEND_FFI_VAL_DOUBLE:
			dcl->align = zend_ffi_type_double.align;
			break;
#ifdef HAVE_LONG_DOUBLE
		case ZEND_FFI_VAL_LONG_DOUBLE:
			dcl->align = zend_ffi_type_long_double.align;
			break;
#endif
		case ZEND_FFI_VAL_CHAR:
		case ZEND_FFI_VAL_STRING:
			dcl->align = zend_ffi_type_char.align;
			break;
		default:
			break;
	}
}
/* }}} */

#define zend_ffi_expr_bool(val) do { \
	if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64) { \
		val->kind = ZEND_FFI_VAL_INT32; \
		val->i64 = !!val->u64; \
	} else if (val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) { \
		val->kind = ZEND_FFI_VAL_INT32; \
		val->i64 = !!val->i64; \
	} else if (val->kind == ZEND_FFI_VAL_FLOAT || val->kind == ZEND_FFI_VAL_DOUBLE || val->kind == ZEND_FFI_VAL_LONG_DOUBLE) { \
		val->kind = ZEND_FFI_VAL_INT32; \
		val->i64 = !!val->d; \
	} else if (val->kind == ZEND_FFI_VAL_CHAR) { \
		val->kind = ZEND_FFI_VAL_INT32; \
		val->i64 = !!val->ch; \
	} else { \
		val->kind = ZEND_FFI_VAL_ERROR; \
	} \
} while (0)

#define zend_ffi_expr_math(val, op2, OP) do { \
	if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64) { \
		if (op2->kind == ZEND_FFI_VAL_UINT32 || op2->kind == ZEND_FFI_VAL_UINT64) { \
			val->kind = MAX(val->kind, op2->kind); \
			val->u64 = val->u64 OP op2->u64; \
		} else if (op2->kind == ZEND_FFI_VAL_INT32) { \
			val->u64 = val->u64 OP op2->i64; \
		} else if (op2->kind == ZEND_FFI_VAL_INT64) { \
			val->u64 = val->u64 OP op2->i64; \
		} else if (op2->kind == ZEND_FFI_VAL_FLOAT || op2->kind == ZEND_FFI_VAL_DOUBLE || op2->kind == ZEND_FFI_VAL_LONG_DOUBLE) { \
			val->kind = op2->kind; \
			val->d = (zend_ffi_double)val->u64 OP op2->d; \
		} else if (op2->kind == ZEND_FFI_VAL_CHAR) { \
			val->u64 = val->u64 OP op2->ch; \
		} else { \
			val->kind = ZEND_FFI_VAL_ERROR; \
		} \
	} else if (val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) { \
		if (op2->kind == ZEND_FFI_VAL_UINT32) { \
			val->i64 = val->i64 OP op2->u64; \
		} else if (op2->kind == ZEND_FFI_VAL_UINT64) { \
			val->i64 = val->i64 OP op2->u64; \
		} else if (op2->kind == ZEND_FFI_VAL_INT32 || op2->kind == ZEND_FFI_VAL_INT64) { \
			val->kind = MAX(val->kind, op2->kind); \
			val->i64 = val->i64 OP op2->i64; \
		} else if (op2->kind == ZEND_FFI_VAL_FLOAT || op2->kind == ZEND_FFI_VAL_DOUBLE || op2->kind == ZEND_FFI_VAL_LONG_DOUBLE) { \
			val->kind = op2->kind; \
			val->d = (zend_ffi_double)val->i64 OP op2->d; \
		} else if (op2->kind == ZEND_FFI_VAL_CHAR) { \
			val->i64 = val->i64 OP op2->ch; \
		} else { \
			val->kind = ZEND_FFI_VAL_ERROR; \
		} \
	} else if (val->kind == ZEND_FFI_VAL_FLOAT || val->kind == ZEND_FFI_VAL_DOUBLE || val->kind == ZEND_FFI_VAL_LONG_DOUBLE) { \
		if (op2->kind == ZEND_FFI_VAL_UINT32 || op2->kind == ZEND_FFI_VAL_UINT64) { \
			val->d = val->d OP (zend_ffi_double)op2->u64; \
		} else if (op2->kind == ZEND_FFI_VAL_INT32 ||op2->kind == ZEND_FFI_VAL_INT64) { \
			val->d = val->d OP (zend_ffi_double)op2->i64; \
		} else if (op2->kind == ZEND_FFI_VAL_FLOAT || op2->kind == ZEND_FFI_VAL_DOUBLE || op2->kind == ZEND_FFI_VAL_LONG_DOUBLE) { \
			val->kind = MAX(val->kind, op2->kind); \
			val->d = val->d OP op2->d; \
		} else if (op2->kind == ZEND_FFI_VAL_CHAR) { \
			val->d = val->d OP (zend_ffi_double)op2->ch; \
		} else { \
			val->kind = ZEND_FFI_VAL_ERROR; \
		} \
	} else if (val->kind == ZEND_FFI_VAL_CHAR) { \
		if (op2->kind == ZEND_FFI_VAL_UINT32 || op2->kind == ZEND_FFI_VAL_UINT64) { \
			val->kind = op2->kind; \
			val->u64 = val->ch OP op2->u64; \
		} else if (op2->kind == ZEND_FFI_VAL_INT32 || op2->kind == ZEND_FFI_VAL_INT64) { \
			val->kind = ZEND_FFI_VAL_INT64; \
			val->i64 = val->ch OP op2->i64; \
		} else if (op2->kind == ZEND_FFI_VAL_FLOAT || op2->kind == ZEND_FFI_VAL_DOUBLE || op2->kind == ZEND_FFI_VAL_LONG_DOUBLE) { \
			val->kind = op2->kind; \
			val->d = (zend_ffi_double)val->ch OP op2->d; \
		} else if (op2->kind == ZEND_FFI_VAL_CHAR) { \
			val->ch = val->ch OP op2->ch; \
		} else { \
			val->kind = ZEND_FFI_VAL_ERROR; \
		} \
	} else { \
		val->kind = ZEND_FFI_VAL_ERROR; \
	} \
} while (0)

#define zend_ffi_expr_int_math(val, op2, OP) do { \
	if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64) { \
		if (op2->kind == ZEND_FFI_VAL_UINT32 || op2->kind == ZEND_FFI_VAL_UINT64) { \
			val->kind = MAX(val->kind, op2->kind); \
			val->u64 = val->u64 OP op2->u64; \
		} else if (op2->kind == ZEND_FFI_VAL_INT32) { \
			val->u64 = val->u64 OP op2->i64; \
		} else if (op2->kind == ZEND_FFI_VAL_INT64) { \
			val->u64 = val->u64 OP op2->i64; \
		} else if (op2->kind == ZEND_FFI_VAL_FLOAT || op2->kind == ZEND_FFI_VAL_DOUBLE || op2->kind == ZEND_FFI_VAL_LONG_DOUBLE) { \
			val->u64 = val->u64 OP (uint64_t)op2->d; \
		} else if (op2->kind == ZEND_FFI_VAL_CHAR) { \
			val->u64 = val->u64 OP op2->ch; \
		} else { \
			val->kind = ZEND_FFI_VAL_ERROR; \
		} \
	} else if (val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) { \
		if (op2->kind == ZEND_FFI_VAL_UINT32) { \
			val->i64 = val->i64 OP op2->u64; \
		} else if (op2->kind == ZEND_FFI_VAL_UINT64) { \
			val->i64 = val->i64 OP op2->u64; \
		} else if (op2->kind == ZEND_FFI_VAL_INT32 || op2->kind == ZEND_FFI_VAL_INT64) { \
			val->kind = MAX(val->kind, op2->kind); \
			val->i64 = val->i64 OP op2->i64; \
		} else if (op2->kind == ZEND_FFI_VAL_FLOAT || op2->kind == ZEND_FFI_VAL_DOUBLE || op2->kind == ZEND_FFI_VAL_LONG_DOUBLE) { \
			val->u64 = val->u64 OP (int64_t)op2->d; \
		} else if (op2->kind == ZEND_FFI_VAL_CHAR) { \
			val->i64 = val->i64 OP op2->ch; \
		} else { \
			val->kind = ZEND_FFI_VAL_ERROR; \
		} \
	} else if (val->kind == ZEND_FFI_VAL_FLOAT || val->kind == ZEND_FFI_VAL_DOUBLE || val->kind == ZEND_FFI_VAL_LONG_DOUBLE) { \
		if (op2->kind == ZEND_FFI_VAL_UINT32 || op2->kind == ZEND_FFI_VAL_UINT64) { \
			val->kind = op2->kind; \
			val->u64 = (uint64_t)val->d OP op2->u64; \
		} else if (op2->kind == ZEND_FFI_VAL_INT32 || op2->kind == ZEND_FFI_VAL_INT64) { \
			val->kind = op2->kind; \
			val->i64 = (int64_t)val->d OP op2->i64; \
		} else { \
			val->kind = ZEND_FFI_VAL_ERROR; \
		} \
	} else if (val->kind == ZEND_FFI_VAL_CHAR) { \
		if (op2->kind == ZEND_FFI_VAL_UINT32 || op2->kind == ZEND_FFI_VAL_UINT64) { \
			val->kind = op2->kind; \
			val->u64 = (uint64_t)val->ch OP op2->u64; \
		} else if (op2->kind == ZEND_FFI_VAL_INT32 || op2->kind == ZEND_FFI_VAL_INT64) { \
			val->kind = op2->kind; \
			val->i64 = (int64_t)val->ch OP op2->u64; \
		} else if (op2->kind == ZEND_FFI_VAL_CHAR) { \
			val->ch = val->ch OP op2->ch; \
		} else { \
			val->kind = ZEND_FFI_VAL_ERROR; \
		} \
	} else { \
		val->kind = ZEND_FFI_VAL_ERROR; \
	} \
} while (0)

#define zend_ffi_expr_cmp(val, op2, OP) do { \
	if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64) { \
		if (op2->kind == ZEND_FFI_VAL_UINT32 || op2->kind == ZEND_FFI_VAL_UINT64) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = val->u64 OP op2->u64; \
		} else if (op2->kind == ZEND_FFI_VAL_INT32 || op2->kind == ZEND_FFI_VAL_INT64) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = val->u64 OP op2->u64; /*signed/unsigned */ \
		} else if (op2->kind == ZEND_FFI_VAL_FLOAT || op2->kind == ZEND_FFI_VAL_DOUBLE || op2->kind == ZEND_FFI_VAL_LONG_DOUBLE) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = (zend_ffi_double)val->u64 OP op2->d; \
		} else if (op2->kind == ZEND_FFI_VAL_CHAR) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = val->u64 OP op2->d; \
		} else { \
			val->kind = ZEND_FFI_VAL_ERROR; \
		} \
	} else if (val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) { \
		if (op2->kind == ZEND_FFI_VAL_UINT32 || op2->kind == ZEND_FFI_VAL_UINT64) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = val->i64 OP op2->i64; /* signed/unsigned */ \
		} else if (op2->kind == ZEND_FFI_VAL_INT32 || op2->kind == ZEND_FFI_VAL_INT64) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = val->i64 OP op2->i64; \
		} else if (op2->kind == ZEND_FFI_VAL_FLOAT || op2->kind == ZEND_FFI_VAL_DOUBLE || op2->kind == ZEND_FFI_VAL_LONG_DOUBLE) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = (zend_ffi_double)val->i64 OP op2->d; \
		} else if (op2->kind == ZEND_FFI_VAL_CHAR) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = val->i64 OP op2->ch; \
		} else { \
			val->kind = ZEND_FFI_VAL_ERROR; \
		} \
	} else if (val->kind == ZEND_FFI_VAL_FLOAT || val->kind == ZEND_FFI_VAL_DOUBLE || val->kind == ZEND_FFI_VAL_LONG_DOUBLE) { \
		if (op2->kind == ZEND_FFI_VAL_UINT32 || op2->kind == ZEND_FFI_VAL_UINT64) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = val->d OP (zend_ffi_double)op2->u64; \
		} else if (op2->kind == ZEND_FFI_VAL_INT32 ||op2->kind == ZEND_FFI_VAL_INT64) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = val->d OP (zend_ffi_double)op2->i64; \
		} else if (op2->kind == ZEND_FFI_VAL_FLOAT || op2->kind == ZEND_FFI_VAL_DOUBLE || op2->kind == ZEND_FFI_VAL_LONG_DOUBLE) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = val->d OP op2->d; \
		} else if (op2->kind == ZEND_FFI_VAL_CHAR) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = val->d OP (zend_ffi_double)op2->ch; \
		} else { \
			val->kind = ZEND_FFI_VAL_ERROR; \
		} \
	} else if (val->kind == ZEND_FFI_VAL_CHAR) { \
		if (op2->kind == ZEND_FFI_VAL_UINT32 || op2->kind == ZEND_FFI_VAL_UINT64) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = val->ch OP op2->i64; /* signed/unsigned */ \
		} else if (op2->kind == ZEND_FFI_VAL_INT32 || op2->kind == ZEND_FFI_VAL_INT64) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = val->ch OP op2->i64; \
		} else if (op2->kind == ZEND_FFI_VAL_FLOAT || op2->kind == ZEND_FFI_VAL_DOUBLE || op2->kind == ZEND_FFI_VAL_LONG_DOUBLE) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = (zend_ffi_double)val->ch OP op2->d; \
		} else if (op2->kind == ZEND_FFI_VAL_CHAR) { \
			val->kind = ZEND_FFI_VAL_INT32; \
			val->i64 = val->ch OP op2->ch; \
		} else { \
			val->kind = ZEND_FFI_VAL_ERROR; \
		} \
	} else { \
		val->kind = ZEND_FFI_VAL_ERROR; \
	} \
} while (0)

void zend_ffi_expr_conditional(zend_ffi_val *val, zend_ffi_val *op2, zend_ffi_val *op3) /* {{{ */
{
	zend_ffi_expr_bool(val);
	if (val->kind == ZEND_FFI_VAL_INT32) {
		if (val->i64) {
			*val = *op2;
		} else {
			*val = *op3;
		}
	}
}
/* }}} */

void zend_ffi_expr_bool_or(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_bool(val);
	zend_ffi_expr_bool(op2);
	if (val->kind == ZEND_FFI_VAL_INT32 && op2->kind == ZEND_FFI_VAL_INT32) {
		val->i64 = val->i64 || op2->i64;
	} else {
		val->kind = ZEND_FFI_VAL_ERROR;
	}
}
/* }}} */

void zend_ffi_expr_bool_and(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_bool(val);
	zend_ffi_expr_bool(op2);
	if (val->kind == ZEND_FFI_VAL_INT32 && op2->kind == ZEND_FFI_VAL_INT32) {
		val->i64 = val->i64 && op2->i64;
	} else {
		val->kind = ZEND_FFI_VAL_ERROR;
	}
}
/* }}} */

void zend_ffi_expr_bw_or(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_int_math(val, op2, |);
}
/* }}} */

void zend_ffi_expr_bw_xor(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_int_math(val, op2, ^);
}
/* }}} */

void zend_ffi_expr_bw_and(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_int_math(val, op2, &);
}
/* }}} */

void zend_ffi_expr_is_equal(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_cmp(val, op2, ==);
}
/* }}} */

void zend_ffi_expr_is_not_equal(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_cmp(val, op2, !=);
}
/* }}} */

void zend_ffi_expr_is_less(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_cmp(val, op2, <);
}
/* }}} */

void zend_ffi_expr_is_greater(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_cmp(val, op2, >);
}
/* }}} */

void zend_ffi_expr_is_less_or_equal(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_cmp(val, op2, <=);
}
/* }}} */

void zend_ffi_expr_is_greater_or_equal(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_cmp(val, op2, >=);
}
/* }}} */

void zend_ffi_expr_shift_left(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_int_math(val, op2, <<);
}
/* }}} */

void zend_ffi_expr_shift_right(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_int_math(val, op2, >>);
}
/* }}} */

void zend_ffi_expr_add(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_math(val, op2, +);
}
/* }}} */

void zend_ffi_expr_sub(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_math(val, op2, -);
}
/* }}} */

void zend_ffi_expr_mul(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_math(val, op2, *);
}
/* }}} */

void zend_ffi_expr_div(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_math(val, op2, /);
}
/* }}} */

void zend_ffi_expr_mod(zend_ffi_val *val, zend_ffi_val *op2) /* {{{ */
{
	zend_ffi_expr_int_math(val, op2, %); // ???
}
/* }}} */

void zend_ffi_expr_cast(zend_ffi_val *val, zend_ffi_dcl *dcl) /* {{{ */
{
	zend_ffi_finalize_type(dcl);
	switch (ZEND_FFI_TYPE(dcl->type)->kind) {
		case ZEND_FFI_TYPE_FLOAT:
			if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64) {
				val->kind = ZEND_FFI_VAL_FLOAT;
				val->d = val->u64;
			} else if (val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) {
				val->kind = ZEND_FFI_VAL_FLOAT;
				val->d = val->i64;
			} else if (val->kind == ZEND_FFI_VAL_FLOAT || val->kind == ZEND_FFI_VAL_DOUBLE || val->kind == ZEND_FFI_VAL_LONG_DOUBLE) {
				val->kind = ZEND_FFI_VAL_FLOAT;
			} else if (val->kind == ZEND_FFI_VAL_CHAR) {
				val->kind = ZEND_FFI_VAL_FLOAT;
				val->d = val->ch;
			} else {
				val->kind = ZEND_FFI_VAL_ERROR;
			}
			break;
		case ZEND_FFI_TYPE_DOUBLE:
			if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64) {
				val->kind = ZEND_FFI_VAL_DOUBLE;
				val->d = val->u64;
			} else if (val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) {
				val->kind = ZEND_FFI_VAL_DOUBLE;
				val->d = val->i64;
			} else if (val->kind == ZEND_FFI_VAL_FLOAT || val->kind == ZEND_FFI_VAL_DOUBLE || val->kind == ZEND_FFI_VAL_LONG_DOUBLE) {
				val->kind = ZEND_FFI_VAL_DOUBLE;
			} else if (val->kind == ZEND_FFI_VAL_CHAR) {
				val->kind = ZEND_FFI_VAL_DOUBLE;
				val->d = val->ch;
			} else {
				val->kind = ZEND_FFI_VAL_ERROR;
			}
			break;
#ifdef HAVE_LONG_DOUBLE
		case ZEND_FFI_TYPE_LONGDOUBLE:
			if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64) {
				val->kind = ZEND_FFI_VAL_LONG_DOUBLE;
				val->d = val->u64;
			} else if (val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) {
				val->kind = ZEND_FFI_VAL_LONG_DOUBLE;
				val->d = val->i64;
			} else if (val->kind == ZEND_FFI_VAL_FLOAT || val->kind == ZEND_FFI_VAL_DOUBLE || val->kind == ZEND_FFI_VAL_LONG_DOUBLE) {
				val->kind = ZEND_FFI_VAL_LONG_DOUBLE;
			} else if (val->kind == ZEND_FFI_VAL_CHAR) {
				val->kind = ZEND_FFI_VAL_LONG_DOUBLE;
				val->d = val->ch;
			} else {
				val->kind = ZEND_FFI_VAL_ERROR;
			}
			break;
#endif
		case ZEND_FFI_TYPE_UINT8:
		case ZEND_FFI_TYPE_UINT16:
		case ZEND_FFI_TYPE_UINT32:
		case ZEND_FFI_TYPE_BOOL:
			if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64 || val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) {
				val->kind = ZEND_FFI_VAL_UINT32;
			} else if (val->kind == ZEND_FFI_VAL_FLOAT || val->kind == ZEND_FFI_VAL_DOUBLE || val->kind == ZEND_FFI_VAL_LONG_DOUBLE) {
				val->kind = ZEND_FFI_VAL_UINT32;
				val->u64 = val->d;
			} else if (val->kind == ZEND_FFI_VAL_CHAR) {
				val->kind = ZEND_FFI_VAL_UINT32;
				val->u64 = val->ch;
			} else {
				val->kind = ZEND_FFI_VAL_ERROR;
			}
			break;
		case ZEND_FFI_TYPE_SINT8:
		case ZEND_FFI_TYPE_SINT16:
		case ZEND_FFI_TYPE_SINT32:
			if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64 || val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) {
				val->kind = ZEND_FFI_VAL_INT32;
			} else if (val->kind == ZEND_FFI_VAL_FLOAT || val->kind == ZEND_FFI_VAL_DOUBLE || val->kind == ZEND_FFI_VAL_LONG_DOUBLE) {
				val->kind = ZEND_FFI_VAL_INT32;
				val->i64 = val->d;
			} else if (val->kind == ZEND_FFI_VAL_CHAR) {
				val->kind = ZEND_FFI_VAL_INT32;
				val->i64 = val->ch;
			} else {
				val->kind = ZEND_FFI_VAL_ERROR;
			}
			break;
		case ZEND_FFI_TYPE_UINT64:
			if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64 || val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) {
				val->kind = ZEND_FFI_VAL_UINT64;
			} else if (val->kind == ZEND_FFI_VAL_FLOAT || val->kind == ZEND_FFI_VAL_DOUBLE || val->kind == ZEND_FFI_VAL_LONG_DOUBLE) {
				val->kind = ZEND_FFI_VAL_UINT64;
				val->u64 = val->d;
			} else if (val->kind == ZEND_FFI_VAL_CHAR) {
				val->kind = ZEND_FFI_VAL_UINT64;
				val->u64 = val->ch;
			} else {
				val->kind = ZEND_FFI_VAL_ERROR;
			}
			break;
		case ZEND_FFI_TYPE_SINT64:
			if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64) {
				val->kind = ZEND_FFI_VAL_CHAR;
				val->ch = val->u64;
			} else if (val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) {
				val->kind = ZEND_FFI_VAL_CHAR;
				val->ch = val->i64;
			} else if (val->kind == ZEND_FFI_VAL_FLOAT || val->kind == ZEND_FFI_VAL_DOUBLE || val->kind == ZEND_FFI_VAL_LONG_DOUBLE) {
				val->kind = ZEND_FFI_VAL_CHAR;
				val->ch = val->d;
			} else if (val->kind == ZEND_FFI_VAL_CHAR) {
			} else {
				val->kind = ZEND_FFI_VAL_ERROR;
			}
			break;
		case ZEND_FFI_TYPE_CHAR:
			if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64 || val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) {
				val->kind = ZEND_FFI_VAL_UINT32;
			} else if (val->kind == ZEND_FFI_VAL_FLOAT || val->kind == ZEND_FFI_VAL_DOUBLE || val->kind == ZEND_FFI_VAL_LONG_DOUBLE) {
				val->kind = ZEND_FFI_VAL_UINT32;
				val->u64 = val->d;
			} else if (val->kind == ZEND_FFI_VAL_CHAR) {
				val->kind = ZEND_FFI_VAL_UINT32;
				val->u64 = val->ch;
			} else {
				val->kind = ZEND_FFI_VAL_ERROR;
			}
			break;
		default:
			val->kind = ZEND_FFI_VAL_ERROR;
			break;
	}
	zend_ffi_type_dtor(dcl->type);
}
/* }}} */

void zend_ffi_expr_plus(zend_ffi_val *val) /* {{{ */
{
	if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64) {
	} else if (val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) {
	} else if (val->kind == ZEND_FFI_VAL_FLOAT || val->kind == ZEND_FFI_VAL_DOUBLE || val->kind == ZEND_FFI_VAL_LONG_DOUBLE) {
	} else if (val->kind == ZEND_FFI_VAL_CHAR) {
	} else {
		val->kind = ZEND_FFI_VAL_ERROR;
	}
}
/* }}} */

void zend_ffi_expr_neg(zend_ffi_val *val) /* {{{ */
{
	if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64) {
		val->u64 = -val->u64;
	} else if (val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) {
		val->i64 = -val->i64;
	} else if (val->kind == ZEND_FFI_VAL_FLOAT || val->kind == ZEND_FFI_VAL_DOUBLE || val->kind == ZEND_FFI_VAL_LONG_DOUBLE) {
		val->d = -val->d;
	} else if (val->kind == ZEND_FFI_VAL_CHAR) {
		val->ch = -val->ch;
	} else {
		val->kind = ZEND_FFI_VAL_ERROR;
	}
}
/* }}} */

void zend_ffi_expr_bw_not(zend_ffi_val *val) /* {{{ */
{
	if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_UINT64) {
		val->u64 = ~val->u64;
	} else if (val->kind == ZEND_FFI_VAL_INT32 || val->kind == ZEND_FFI_VAL_INT64) {
		val->i64 = ~val->i64;
	} else if (val->kind == ZEND_FFI_VAL_CHAR) {
		val->ch = ~val->ch;
	} else {
		val->kind = ZEND_FFI_VAL_ERROR;
	}
}
/* }}} */

void zend_ffi_expr_bool_not(zend_ffi_val *val) /* {{{ */
{
	zend_ffi_expr_bool(val);
	if (val->kind == ZEND_FFI_VAL_INT32) {
		val->i64 = !val->i64;
	}
}
/* }}} */

void zend_ffi_expr_sizeof_val(zend_ffi_val *val) /* {{{ */
{
	if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_INT32) {
		val->kind = ZEND_FFI_VAL_UINT32;
		val->u64 = zend_ffi_type_uint32.size;
	} else if (val->kind == ZEND_FFI_VAL_UINT64 || val->kind == ZEND_FFI_VAL_INT64) {
		val->kind = ZEND_FFI_VAL_UINT32;
		val->u64 = zend_ffi_type_uint64.size;
	} else if (val->kind == ZEND_FFI_VAL_FLOAT) {
		val->kind = ZEND_FFI_VAL_UINT32;
		val->u64 = zend_ffi_type_float.size;
	} else if (val->kind == ZEND_FFI_VAL_DOUBLE) {
		val->kind = ZEND_FFI_VAL_UINT32;
		val->u64 = zend_ffi_type_double.size;
	} else if (val->kind == ZEND_FFI_VAL_LONG_DOUBLE) {
		val->kind = ZEND_FFI_VAL_UINT32;
#ifdef _WIN32
		val->u64 = zend_ffi_type_double.size;
#else
		val->u64 = zend_ffi_type_long_double.size;
#endif
	} else if (val->kind == ZEND_FFI_VAL_CHAR) {
		val->kind = ZEND_FFI_VAL_UINT32;
		val->u64 = zend_ffi_type_char.size;
	} else if (val->kind == ZEND_FFI_VAL_STRING) {
		if (memchr(val->str, '\\', val->len)) {
			// TODO: support for escape sequences ???
			val->kind = ZEND_FFI_VAL_ERROR;
		} else {
			val->kind = ZEND_FFI_VAL_UINT32;
			val->u64 = val->len + 1;
		}
	} else {
		val->kind = ZEND_FFI_VAL_ERROR;
	}
}
/* }}} */

void zend_ffi_expr_sizeof_type(zend_ffi_val *val, zend_ffi_dcl *dcl) /* {{{ */
{
	zend_ffi_type *type;

	zend_ffi_finalize_type(dcl);
	type = ZEND_FFI_TYPE(dcl->type);
	val->kind = (type->size > 0xffffffff) ? ZEND_FFI_VAL_UINT64 : ZEND_FFI_VAL_UINT32;
	val->u64 = type->size;
	zend_ffi_type_dtor(dcl->type);
}
/* }}} */

void zend_ffi_expr_alignof_val(zend_ffi_val *val) /* {{{ */
{
	if (val->kind == ZEND_FFI_VAL_UINT32 || val->kind == ZEND_FFI_VAL_INT32) {
		val->kind = ZEND_FFI_VAL_UINT32;
		val->u64 = zend_ffi_type_uint32.align;
	} else if (val->kind == ZEND_FFI_VAL_UINT64 || val->kind == ZEND_FFI_VAL_INT64) {
		val->kind = ZEND_FFI_VAL_UINT32;
		val->u64 = zend_ffi_type_uint64.align;
	} else if (val->kind == ZEND_FFI_VAL_FLOAT) {
		val->kind = ZEND_FFI_VAL_UINT32;
		val->u64 = zend_ffi_type_float.align;
	} else if (val->kind == ZEND_FFI_VAL_DOUBLE) {
		val->kind = ZEND_FFI_VAL_UINT32;
		val->u64 = zend_ffi_type_double.align;
#ifdef HAVE_LONG_DOUBLE
	} else if (val->kind == ZEND_FFI_VAL_LONG_DOUBLE) {
		val->kind = ZEND_FFI_VAL_UINT32;
		val->u64 = zend_ffi_type_long_double.align;
#endif
	} else if (val->kind == ZEND_FFI_VAL_CHAR) {
		val->kind = ZEND_FFI_VAL_UINT32;
		val->u64 = zend_ffi_type_char.size;
	} else if (val->kind == ZEND_FFI_VAL_STRING) {
		val->kind = ZEND_FFI_VAL_UINT32;
		val->u64 = _Alignof(char*);
	} else {
		val->kind = ZEND_FFI_VAL_ERROR;
	}
}
/* }}} */

void zend_ffi_expr_alignof_type(zend_ffi_val *val, zend_ffi_dcl *dcl) /* {{{ */
{
	zend_ffi_finalize_type(dcl);
	val->kind = ZEND_FFI_VAL_UINT32;
	val->u64 = ZEND_FFI_TYPE(dcl->type)->align;
	zend_ffi_type_dtor(dcl->type);
}
/* }}} */

void zend_ffi_val_number(zend_ffi_val *val, int base, const char *str, size_t str_len) /* {{{ */
{
	int u = 0;
	int l = 0;

	if (str[str_len-1] == 'u' || str[str_len-1] == 'U') {
		u = 1;
		if (str[str_len-2] == 'l' || str[str_len-2] == 'L') {
			l = 1;
			if (str[str_len-3] == 'l' || str[str_len-3] == 'L') {
				l = 2;
			}
		}
	} else if (str[str_len-1] == 'l' || str[str_len-1] == 'L') {
		l = 1;
		if (str[str_len-2] == 'l' || str[str_len-2] == 'L') {
			l = 2;
			if (str[str_len-3] == 'u' || str[str_len-3] == 'U') {
				u = 1;
			}
		} else if (str[str_len-2] == 'u' || str[str_len-2] == 'U') {
			u = 1;
		}
	}
	if (u) {
		val->u64 = strtoull(str, NULL, base);
		if (l == 0) {
			val->kind = ZEND_FFI_VAL_UINT32;
		} else if (l == 1) {
			val->kind = (sizeof(long) == 4) ? ZEND_FFI_VAL_UINT32 : ZEND_FFI_VAL_UINT64;
		} else if (l == 2) {
			val->kind = ZEND_FFI_VAL_UINT64;
		}
	} else {
		val->i64 = strtoll(str, NULL, base);
		if (l == 0) {
			val->kind = ZEND_FFI_VAL_INT32;
		} else if (l == 1) {
			val->kind = (sizeof(long) == 4) ? ZEND_FFI_VAL_INT32 : ZEND_FFI_VAL_INT64;
		} else if (l == 2) {
			val->kind = ZEND_FFI_VAL_INT64;
		}
	}
}
/* }}} */

void zend_ffi_val_float_number(zend_ffi_val *val, const char *str, size_t str_len) /* {{{ */
{
	val->d = strtold(str, NULL);
	if (str[str_len-1] == 'f' || str[str_len-1] == 'F') {
		val->kind = ZEND_FFI_VAL_FLOAT;
	} else if (str[str_len-1] == 'l' || str[str_len-1] == 'L') {
		val->kind = ZEND_FFI_VAL_LONG_DOUBLE;
	} else {
		val->kind = ZEND_FFI_VAL_DOUBLE;
	}
}
/* }}} */

void zend_ffi_val_string(zend_ffi_val *val, const char *str, size_t str_len) /* {{{ */
{
	if (str[0] != '\"') {
		val->kind = ZEND_FFI_VAL_ERROR;
	} else {
		val->kind = ZEND_FFI_VAL_STRING;
		val->str = str + 1;
		val->len = str_len - 2;
	}
}
/* }}} */

void zend_ffi_val_character(zend_ffi_val *val, const char *str, size_t str_len) /* {{{ */
{
	int n;

	if (str[0] != '\'') {
		val->kind = ZEND_FFI_VAL_ERROR;
	} else {
		val->kind = ZEND_FFI_VAL_CHAR;
		if (str_len == 3) {
			val->ch = str[1];
		} else if (str[1] == '\\') {
			if (str[2] == 'a') {
			} else if (str[2] == 'b' && str_len == 4) {
				val->ch = '\b';
			} else if (str[2] == 'f' && str_len == 4) {
				val->ch = '\f';
			} else if (str[2] == 'n' && str_len == 4) {
				val->ch = '\n';
			} else if (str[2] == 'r' && str_len == 4) {
				val->ch = '\r';
			} else if (str[2] == 't' && str_len == 4) {
				val->ch = '\t';
			} else if (str[2] == 'v' && str_len == 4) {
				val->ch = '\v';
			} else if (str[2] >= '0' && str[2] <= '7') {
				n = str[2] - '0';
				if (str[3] >= '0' && str[3] <= '7') {
					n = n * 8 + (str[3] - '0');
					if ((str[4] >= '0' && str[4] <= '7') && str_len == 6) {
						n = n * 8 + (str[4] - '0');
					} else if (str_len != 5) {
						val->kind = ZEND_FFI_VAL_ERROR;
					}
				} else if (str_len != 4) {
					val->kind = ZEND_FFI_VAL_ERROR;
				}
				if (n <= 0xff) {
					val->ch = n;
				} else {
					val->kind = ZEND_FFI_VAL_ERROR;
				}
			} else if (str[2] == 'x') {
				if (str[3] >= '0' && str[3] <= '9') {
					n = str[3] - '0';
				} else if (str[3] >= 'A' && str[3] <= 'F') {
					n = str[3] - 'A';
				} else if (str[3] >= 'a' && str[3] <= 'f') {
					n = str[3] - 'a';
				} else {
					val->kind = ZEND_FFI_VAL_ERROR;
					return;
				}
				if ((str[4] >= '0' && str[4] <= '9') && str_len == 6) {
					n = n * 16 + (str[4] - '0');
				} else if ((str[4] >= 'A' && str[4] <= 'F') && str_len == 6) {
					n = n * 16 + (str[4] - 'A');
				} else if ((str[4] >= 'a' && str[4] <= 'f') && str_len == 6) {
					n = n * 16 + (str[4] - 'a');
				} else if (str_len != 5) {
					val->kind = ZEND_FFI_VAL_ERROR;
					return;
				}
				val->ch = n;
			} else if (str_len == 4) {
				val->ch = str[2];
			} else {
				val->kind = ZEND_FFI_VAL_ERROR;
			}
		} else {
			val->kind = ZEND_FFI_VAL_ERROR;
		}
	}
}
/* }}} */
