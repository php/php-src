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
   |          Dmitry Stogov <dmitry@php.net>                              |
   |          Xinchen Hui <laruence@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_TYPES_H
#define ZEND_TYPES_H

#include "zend_portability.h"
#include "zend_long.h"
#include <stdbool.h>

#ifdef __SSE2__
# include <mmintrin.h>
# include <emmintrin.h>
#endif

#ifdef WORDS_BIGENDIAN
# define ZEND_ENDIAN_LOHI(lo, hi)          hi; lo;
# define ZEND_ENDIAN_LOHI_3(lo, mi, hi)    hi; mi; lo;
# define ZEND_ENDIAN_LOHI_4(a, b, c, d)    d; c; b; a;
# define ZEND_ENDIAN_LOHI_C(lo, hi)        hi, lo
# define ZEND_ENDIAN_LOHI_C_3(lo, mi, hi)  hi, mi, lo,
# define ZEND_ENDIAN_LOHI_C_4(a, b, c, d)  d, c, b, a
#else
# define ZEND_ENDIAN_LOHI(lo, hi)          lo; hi;
# define ZEND_ENDIAN_LOHI_3(lo, mi, hi)    lo; mi; hi;
# define ZEND_ENDIAN_LOHI_4(a, b, c, d)    a; b; c; d;
# define ZEND_ENDIAN_LOHI_C(lo, hi)        lo, hi
# define ZEND_ENDIAN_LOHI_C_3(lo, mi, hi)  lo, mi, hi,
# define ZEND_ENDIAN_LOHI_C_4(a, b, c, d)  a, b, c, d
#endif

typedef bool zend_bool;
typedef unsigned char zend_uchar;

typedef enum {
  SUCCESS =  0,
  FAILURE = -1,		/* this MUST stay a negative number, or it may affect functions! */
} ZEND_RESULT_CODE;

typedef ZEND_RESULT_CODE zend_result;

#ifdef ZEND_ENABLE_ZVAL_LONG64
# ifdef ZEND_WIN32
#  define ZEND_SIZE_MAX  _UI64_MAX
# else
#  define ZEND_SIZE_MAX  SIZE_MAX
# endif
#else
# if defined(ZEND_WIN32)
#  define ZEND_SIZE_MAX  _UI32_MAX
# else
#  define ZEND_SIZE_MAX SIZE_MAX
# endif
#endif

typedef intptr_t zend_intptr_t;
typedef uintptr_t zend_uintptr_t;

#ifdef ZTS
#define ZEND_TLS static TSRM_TLS
#define ZEND_EXT_TLS TSRM_TLS
#else
#define ZEND_TLS static
#define ZEND_EXT_TLS
#endif

typedef struct _zend_object_handlers zend_object_handlers;
typedef struct _zend_class_entry     zend_class_entry;
typedef union  _zend_function        zend_function;
typedef struct _zend_execute_data    zend_execute_data;

typedef struct _zval_struct     zval;

typedef struct _zend_refcounted zend_refcounted;
typedef struct _zend_string     zend_string;
typedef struct _zend_array      zend_array;
typedef struct _zend_object     zend_object;
typedef struct _zend_resource   zend_resource;
typedef struct _zend_reference  zend_reference;
typedef struct _zend_ast_ref    zend_ast_ref;
typedef struct _zend_ast        zend_ast;

typedef int  (*compare_func_t)(const void *, const void *);
typedef void (*swap_func_t)(void *, void *);
typedef void (*sort_func_t)(void *, size_t, size_t, compare_func_t, swap_func_t);
typedef void (*dtor_func_t)(zval *pDest);
typedef void (*copy_ctor_func_t)(zval *pElement);

/*
 * zend_type - is an abstraction layer to represent information about type hint.
 * It shouldn't be used directly. Only through ZEND_TYPE_* macros.
 *
 * ZEND_TYPE_IS_SET()        - checks if there is a type-hint
 * ZEND_TYPE_IS_ONLY_MASK()  - checks if type-hint refer to standard type only
 * ZEND_TYPE_IS_COMPLEX()    - checks if type is a type_list, or contains a class either as a CE or as a name
 * ZEND_TYPE_HAS_NAME()      - checks if type-hint contains some class as zend_string *
 * ZEND_TYPE_IS_INTERSECTION() - checks if the type_list represents an intersection type list
 * ZEND_TYPE_IS_UNION()      - checks if the type_list represents a union type list
 *
 * ZEND_TYPE_NAME()       - returns referenced class name
 * ZEND_TYPE_PURE_MASK()  - returns MAY_BE_* type mask
 * ZEND_TYPE_FULL_MASK()  - returns MAY_BE_* type mask together with other flags
 *
 * ZEND_TYPE_ALLOW_NULL() - checks if NULL is allowed
 *
 * ZEND_TYPE_INIT_*() should be used for construction.
 */

typedef struct {
	/* Not using a union here, because there's no good way to initialize them
	 * in a way that is supported in both C and C++ (designated initializers
	 * are only supported since C++20). */
	void *ptr;
	uint32_t type_mask;
	/* TODO: We could use the extra 32-bit of padding on 64-bit systems. */
} zend_type;

typedef struct {
	uint32_t num_types;
	zend_type types[1];
} zend_type_list;

#define _ZEND_TYPE_EXTRA_FLAGS_SHIFT 25
#define _ZEND_TYPE_MASK ((1u << 25) - 1)
/* Only one of these bits may be set. */
#define _ZEND_TYPE_NAME_BIT (1u << 24)
#define _ZEND_TYPE_LIST_BIT (1u << 22)
#define _ZEND_TYPE_KIND_MASK (_ZEND_TYPE_LIST_BIT|_ZEND_TYPE_NAME_BIT)
/* For BC behaviour with iterable type */
#define _ZEND_TYPE_ITERABLE_BIT (1u << 21)
/* Whether the type list is arena allocated */
#define _ZEND_TYPE_ARENA_BIT (1u << 20)
/* Whether the type list is an intersection type */
#define _ZEND_TYPE_INTERSECTION_BIT (1u << 19)
/* Whether the type is a union type */
#define _ZEND_TYPE_UNION_BIT (1u << 18)
/* Type mask excluding the flags above. */
#define _ZEND_TYPE_MAY_BE_MASK ((1u << 18) - 1)
/* Must have same value as MAY_BE_NULL */
#define _ZEND_TYPE_NULLABLE_BIT 0x2u

#define ZEND_TYPE_IS_SET(t) \
	(((t).type_mask & _ZEND_TYPE_MASK) != 0)

/* If a type is complex it means it's either a list with a union or intersection,
 * or the void pointer is a class name */
#define ZEND_TYPE_IS_COMPLEX(t) \
	((((t).type_mask) & _ZEND_TYPE_KIND_MASK) != 0)

#define ZEND_TYPE_HAS_NAME(t) \
	((((t).type_mask) & _ZEND_TYPE_NAME_BIT) != 0)

#define ZEND_TYPE_HAS_LIST(t) \
	((((t).type_mask) & _ZEND_TYPE_LIST_BIT) != 0)

#define ZEND_TYPE_IS_ITERABLE_FALLBACK(t) \
	((((t).type_mask) & _ZEND_TYPE_ITERABLE_BIT) != 0)

#define ZEND_TYPE_IS_INTERSECTION(t) \
	((((t).type_mask) & _ZEND_TYPE_INTERSECTION_BIT) != 0)

#define ZEND_TYPE_IS_UNION(t) \
	((((t).type_mask) & _ZEND_TYPE_UNION_BIT) != 0)

#define ZEND_TYPE_USES_ARENA(t) \
	((((t).type_mask) & _ZEND_TYPE_ARENA_BIT) != 0)

#define ZEND_TYPE_IS_ONLY_MASK(t) \
	(ZEND_TYPE_IS_SET(t) && (t).ptr == NULL)

#define ZEND_TYPE_NAME(t) \
	((zend_string *) (t).ptr)

#define ZEND_TYPE_LITERAL_NAME(t) \
	((const char *) (t).ptr)

#define ZEND_TYPE_LIST(t) \
	((zend_type_list *) (t).ptr)

#define ZEND_TYPE_LIST_SIZE(num_types) \
	(sizeof(zend_type_list) + ((num_types) - 1) * sizeof(zend_type))

/* This iterates over a zend_type_list. */
#define ZEND_TYPE_LIST_FOREACH(list, type_ptr) do { \
	zend_type *_list = (list)->types; \
	zend_type *_end = _list + (list)->num_types; \
	for (; _list < _end; _list++) { \
		type_ptr = _list;

#define ZEND_TYPE_LIST_FOREACH_END() \
	} \
} while (0)

/* This iterates over any zend_type. If it's a type list, all list elements will
 * be visited. If it's a single type, only the single type is visited. */
#define ZEND_TYPE_FOREACH(type, type_ptr) do { \
	zend_type *_cur, *_end; \
	if (ZEND_TYPE_HAS_LIST(type)) { \
		zend_type_list *_list = ZEND_TYPE_LIST(type); \
		_cur = _list->types; \
		_end = _cur + _list->num_types; \
	} else { \
		_cur = &(type); \
		_end = _cur + 1; \
	} \
	do { \
		type_ptr = _cur;

#define ZEND_TYPE_FOREACH_END() \
	} while (++_cur < _end); \
} while (0)

#define ZEND_TYPE_SET_PTR(t, _ptr) \
	((t).ptr = (_ptr))

#define ZEND_TYPE_SET_PTR_AND_KIND(t, _ptr, kind_bit) do { \
	(t).ptr = (_ptr); \
	(t).type_mask &= ~_ZEND_TYPE_KIND_MASK; \
	(t).type_mask |= (kind_bit); \
} while (0)

#define ZEND_TYPE_SET_LIST(t, list) \
	ZEND_TYPE_SET_PTR_AND_KIND(t, list, _ZEND_TYPE_LIST_BIT)

/* FULL_MASK() includes the MAY_BE_* type mask, as well as additional metadata bits.
 * The PURE_MASK() only includes the MAY_BE_* type mask. */
#define ZEND_TYPE_FULL_MASK(t) \
	((t).type_mask)

#define ZEND_TYPE_PURE_MASK(t) \
	((t).type_mask & _ZEND_TYPE_MAY_BE_MASK)

#define ZEND_TYPE_FULL_MASK_WITHOUT_NULL(t) \
	((t).type_mask & ~_ZEND_TYPE_NULLABLE_BIT)

#define ZEND_TYPE_PURE_MASK_WITHOUT_NULL(t) \
	((t).type_mask & _ZEND_TYPE_MAY_BE_MASK & ~_ZEND_TYPE_NULLABLE_BIT)

#define ZEND_TYPE_CONTAINS_CODE(t, code) \
	(((t).type_mask & (1u << (code))) != 0)

#define ZEND_TYPE_ALLOW_NULL(t) \
	(((t).type_mask & _ZEND_TYPE_NULLABLE_BIT) != 0)

#define ZEND_TYPE_INIT_NONE(extra_flags) \
	{ NULL, (extra_flags) }

#define ZEND_TYPE_INIT_MASK(_type_mask) \
	{ NULL, (_type_mask) }

#define ZEND_TYPE_INIT_CODE(code, allow_null, extra_flags) \
	ZEND_TYPE_INIT_MASK(((code) == _IS_BOOL ? MAY_BE_BOOL : ( (code) == IS_ITERABLE ? _ZEND_TYPE_ITERABLE_BIT : ((code) == IS_MIXED ? MAY_BE_ANY : (1 << (code))))) \
		| ((allow_null) ? _ZEND_TYPE_NULLABLE_BIT : 0) | (extra_flags))

#define ZEND_TYPE_INIT_PTR(ptr, type_kind, allow_null, extra_flags) \
	{ (void *) (ptr), \
		(type_kind) | ((allow_null) ? _ZEND_TYPE_NULLABLE_BIT : 0) | (extra_flags) }

#define ZEND_TYPE_INIT_PTR_MASK(ptr, type_mask) \
	{ (void *) (ptr), (type_mask) }

#define ZEND_TYPE_INIT_UNION(ptr, extra_flags) \
	{ (void *) (ptr), (_ZEND_TYPE_LIST_BIT|_ZEND_TYPE_UNION_BIT) | (extra_flags) }

#define ZEND_TYPE_INIT_INTERSECTION(ptr, extra_flags) \
	{ (void *) (ptr), (_ZEND_TYPE_LIST_BIT|_ZEND_TYPE_INTERSECTION_BIT) | (extra_flags) }

#define ZEND_TYPE_INIT_CLASS(class_name, allow_null, extra_flags) \
	ZEND_TYPE_INIT_PTR(class_name, _ZEND_TYPE_NAME_BIT, allow_null, extra_flags)

#define ZEND_TYPE_INIT_CLASS_CONST(class_name, allow_null, extra_flags) \
	ZEND_TYPE_INIT_PTR(class_name, _ZEND_TYPE_NAME_BIT, allow_null, extra_flags)

#define ZEND_TYPE_INIT_CLASS_CONST_MASK(class_name, type_mask) \
	ZEND_TYPE_INIT_PTR_MASK(class_name, _ZEND_TYPE_NAME_BIT | (type_mask))

typedef union _zend_value {
	zend_long         lval;				/* long value */
	double            dval;				/* double value */
	zend_refcounted  *counted;
	zend_string      *str;
	zend_array       *arr;
	zend_object      *obj;
	zend_resource    *res;
	zend_reference   *ref;
	zend_ast_ref     *ast;
	zval             *zv;
	void             *ptr;
	zend_class_entry *ce;
	zend_function    *func;
	struct {
		uint32_t w1;
		uint32_t w2;
	} ww;
} zend_value;

struct _zval_struct {
	zend_value        value;			/* value */
	union {
		uint32_t type_info;
		struct {
			ZEND_ENDIAN_LOHI_3(
				zend_uchar    type,			/* active type */
				zend_uchar    type_flags,
				union {
					uint16_t  extra;        /* not further specified */
				} u)
		} v;
	} u1;
	union {
		uint32_t     next;                 /* hash collision chain */
		uint32_t     cache_slot;           /* cache slot (for RECV_INIT) */
		uint32_t     opline_num;           /* opline number (for FAST_CALL) */
		uint32_t     lineno;               /* line number (for ast nodes) */
		uint32_t     num_args;             /* arguments number for EX(This) */
		uint32_t     fe_pos;               /* foreach position */
		uint32_t     fe_iter_idx;          /* foreach iterator index */
		uint32_t     property_guard;       /* single property guard */
		uint32_t     constant_flags;       /* constant flags */
		uint32_t     extra;                /* not further specified */
	} u2;
};

typedef struct _zend_refcounted_h {
	uint32_t         refcount;			/* reference counter 32-bit */
	union {
		uint32_t type_info;
	} u;
} zend_refcounted_h;

struct _zend_refcounted {
	zend_refcounted_h gc;
};

struct _zend_string {
	zend_refcounted_h gc;
	zend_ulong        h;                /* hash value */
	size_t            len;
	char              val[1];
};

typedef struct _Bucket {
	zval              val;
	zend_ulong        h;                /* hash value (or numeric index)   */
	zend_string      *key;              /* string key or NULL for numerics */
} Bucket;

typedef struct _zend_array HashTable;

struct _zend_array {
	zend_refcounted_h gc;
	union {
		struct {
			ZEND_ENDIAN_LOHI_4(
				zend_uchar    flags,
				zend_uchar    _unused,
				zend_uchar    nIteratorsCount,
				zend_uchar    _unused2)
		} v;
		uint32_t flags;
	} u;
	uint32_t          nTableMask;
	union {
		uint32_t     *arHash;   /* hash table (allocated above this pointer) */
		Bucket       *arData;   /* array of hash buckets */
		zval         *arPacked; /* packed array of zvals */
	};
	uint32_t          nNumUsed;
	uint32_t          nNumOfElements;
	uint32_t          nTableSize;
	uint32_t          nInternalPointer;
	zend_long         nNextFreeElement;
	dtor_func_t       pDestructor;
};

/*
 * HashTable Data Layout
 * =====================
 *
 *                 +=============================+
 *                 | HT_HASH(ht, ht->nTableMask) |                   +=============================+
 *                 | ...                         |                   | HT_INVALID_IDX              |
 *                 | HT_HASH(ht, -1)             |                   | HT_INVALID_IDX              |
 *                 +-----------------------------+                   +-----------------------------+
 * ht->arData ---> | Bucket[0]                   | ht->arPacked ---> | ZVAL[0]                     |
 *                 | ...                         |                   | ...                         |
 *                 | Bucket[ht->nTableSize-1]    |                   | ZVAL[ht->nTableSize-1]      |
 *                 +=============================+                   +=============================+
 */

#define HT_INVALID_IDX ((uint32_t) -1)

#define HT_MIN_MASK ((uint32_t) -2)
#define HT_MIN_SIZE 8

/* HT_MAX_SIZE is chosen to satisfy the following constraints:
 * - HT_SIZE_TO_MASK(HT_MAX_SIZE) != 0
 * - HT_SIZE_EX(HT_MAX_SIZE, HT_SIZE_TO_MASK(HT_MAX_SIZE)) does not overflow or
 *   wrapparound, and is <= the addressable space size
 * - HT_MAX_SIZE must be a power of two:
 *   (nTableSize<HT_MAX_SIZE ? nTableSize+nTableSize : nTableSize) <= HT_MAX_SIZE
 */
#if SIZEOF_SIZE_T == 4
# define HT_MAX_SIZE 0x02000000
# define HT_HASH_TO_BUCKET_EX(data, idx) \
	((Bucket*)((char*)(data) + (idx)))
# define HT_IDX_TO_HASH(idx) \
	((idx) * sizeof(Bucket))
# define HT_HASH_TO_IDX(idx) \
	((idx) / sizeof(Bucket))
#elif SIZEOF_SIZE_T == 8
# define HT_MAX_SIZE 0x40000000
# define HT_HASH_TO_BUCKET_EX(data, idx) \
	((data) + (idx))
# define HT_IDX_TO_HASH(idx) \
	(idx)
# define HT_HASH_TO_IDX(idx) \
	(idx)
#else
# error "Unknown SIZEOF_SIZE_T"
#endif

#define HT_HASH_EX(data, idx) \
	((uint32_t*)(data))[(int32_t)(idx)]
#define HT_HASH(ht, idx) \
	HT_HASH_EX((ht)->arHash, idx)

#define HT_SIZE_TO_MASK(nTableSize) \
	((uint32_t)(-((nTableSize) + (nTableSize))))
#define HT_HASH_SIZE(nTableMask) \
	(((size_t)-(uint32_t)(nTableMask)) * sizeof(uint32_t))
#define HT_DATA_SIZE(nTableSize) \
	((size_t)(nTableSize) * sizeof(Bucket))
#define HT_SIZE_EX(nTableSize, nTableMask) \
	(HT_DATA_SIZE((nTableSize)) + HT_HASH_SIZE((nTableMask)))
#define HT_SIZE(ht) \
	HT_SIZE_EX((ht)->nTableSize, (ht)->nTableMask)
#define HT_USED_SIZE(ht) \
	(HT_HASH_SIZE((ht)->nTableMask) + ((size_t)(ht)->nNumUsed * sizeof(Bucket)))
#define HT_PACKED_DATA_SIZE(nTableSize) \
	((size_t)(nTableSize) * sizeof(zval))
#define HT_PACKED_SIZE_EX(nTableSize, nTableMask) \
	(HT_PACKED_DATA_SIZE((nTableSize)) + HT_HASH_SIZE((nTableMask)))
#define HT_PACKED_SIZE(ht) \
	HT_PACKED_SIZE_EX((ht)->nTableSize, (ht)->nTableMask)
#define HT_PACKED_USED_SIZE(ht) \
	(HT_HASH_SIZE((ht)->nTableMask) + ((size_t)(ht)->nNumUsed * sizeof(zval)))
#ifdef __SSE2__
# define HT_HASH_RESET(ht) do { \
		char *p = (char*)&HT_HASH(ht, (ht)->nTableMask); \
		size_t size = HT_HASH_SIZE((ht)->nTableMask); \
		__m128i xmm0 = _mm_setzero_si128(); \
		xmm0 = _mm_cmpeq_epi8(xmm0, xmm0); \
		ZEND_ASSERT(size >= 64 && ((size & 0x3f) == 0)); \
		do { \
			_mm_storeu_si128((__m128i*)p, xmm0); \
			_mm_storeu_si128((__m128i*)(p+16), xmm0); \
			_mm_storeu_si128((__m128i*)(p+32), xmm0); \
			_mm_storeu_si128((__m128i*)(p+48), xmm0); \
			p += 64; \
			size -= 64; \
		} while (size != 0); \
	} while (0)
#else
# define HT_HASH_RESET(ht) \
	memset(&HT_HASH(ht, (ht)->nTableMask), HT_INVALID_IDX, HT_HASH_SIZE((ht)->nTableMask))
#endif
#define HT_HASH_RESET_PACKED(ht) do { \
		HT_HASH(ht, -2) = HT_INVALID_IDX; \
		HT_HASH(ht, -1) = HT_INVALID_IDX; \
	} while (0)
#define HT_HASH_TO_BUCKET(ht, idx) \
	HT_HASH_TO_BUCKET_EX((ht)->arData, idx)

#define HT_SET_DATA_ADDR(ht, ptr) do { \
		(ht)->arData = (Bucket*)(((char*)(ptr)) + HT_HASH_SIZE((ht)->nTableMask)); \
	} while (0)
#define HT_GET_DATA_ADDR(ht) \
	((char*)((ht)->arData) - HT_HASH_SIZE((ht)->nTableMask))

typedef uint32_t HashPosition;

typedef struct _HashTableIterator {
	HashTable    *ht;
	HashPosition  pos;
} HashTableIterator;

struct _zend_object {
	zend_refcounted_h gc;
	uint32_t          handle; // TODO: may be removed ???
	zend_class_entry *ce;
	const zend_object_handlers *handlers;
	HashTable        *properties;
	zval              properties_table[1];
};

struct _zend_resource {
	zend_refcounted_h gc;
	zend_long         handle; // TODO: may be removed ???
	int               type;
	void             *ptr;
};

typedef struct {
	size_t num;
	size_t num_allocated;
	struct _zend_property_info *ptr[1];
} zend_property_info_list;

typedef union {
	struct _zend_property_info *ptr;
	uintptr_t list;
} zend_property_info_source_list;

#define ZEND_PROPERTY_INFO_SOURCE_FROM_LIST(list) (0x1 | (uintptr_t) (list))
#define ZEND_PROPERTY_INFO_SOURCE_TO_LIST(list) ((zend_property_info_list *) ((list) & ~0x1))
#define ZEND_PROPERTY_INFO_SOURCE_IS_LIST(list) ((list) & 0x1)

struct _zend_reference {
	zend_refcounted_h              gc;
	zval                           val;
	zend_property_info_source_list sources;
};

struct _zend_ast_ref {
	zend_refcounted_h gc;
	/*zend_ast        ast; zend_ast follows the zend_ast_ref structure */
};

/* Regular data types: Must be in sync with zend_variables.c. */
#define IS_UNDEF					0
#define IS_NULL						1
#define IS_FALSE					2
#define IS_TRUE						3
#define IS_LONG						4
#define IS_DOUBLE					5
#define IS_STRING					6
#define IS_ARRAY					7
#define IS_OBJECT					8
#define IS_RESOURCE					9
#define IS_REFERENCE				10
#define IS_CONSTANT_AST				11 /* Constant expressions */

/* Fake types used only for type hinting.
 * These are allowed to overlap with the types below. */
#define IS_CALLABLE					12
#define IS_ITERABLE					13
#define IS_VOID						14
#define IS_STATIC					15
#define IS_MIXED					16
#define IS_NEVER					17

/* internal types */
#define IS_INDIRECT             	12
#define IS_PTR						13
#define IS_ALIAS_PTR				14
#define _IS_ERROR					15

/* used for casts */
#define _IS_BOOL					18
#define _IS_NUMBER					19

static zend_always_inline zend_uchar zval_get_type(const zval* pz) {
	return pz->u1.v.type;
}

#define ZEND_SAME_FAKE_TYPE(faketype, realtype) ( \
	(faketype) == (realtype) \
	|| ((faketype) == _IS_BOOL && ((realtype) == IS_TRUE || (realtype) == IS_FALSE)) \
)

/* we should never set just Z_TYPE, we should set Z_TYPE_INFO */
#define Z_TYPE(zval)				zval_get_type(&(zval))
#define Z_TYPE_P(zval_p)			Z_TYPE(*(zval_p))

#define Z_TYPE_FLAGS(zval)			(zval).u1.v.type_flags
#define Z_TYPE_FLAGS_P(zval_p)		Z_TYPE_FLAGS(*(zval_p))

#define Z_TYPE_INFO(zval)			(zval).u1.type_info
#define Z_TYPE_INFO_P(zval_p)		Z_TYPE_INFO(*(zval_p))

#define Z_NEXT(zval)				(zval).u2.next
#define Z_NEXT_P(zval_p)			Z_NEXT(*(zval_p))

#define Z_CACHE_SLOT(zval)			(zval).u2.cache_slot
#define Z_CACHE_SLOT_P(zval_p)		Z_CACHE_SLOT(*(zval_p))

#define Z_LINENO(zval)				(zval).u2.lineno
#define Z_LINENO_P(zval_p)			Z_LINENO(*(zval_p))

#define Z_OPLINE_NUM(zval)			(zval).u2.opline_num
#define Z_OPLINE_NUM_P(zval_p)		Z_OPLINE_NUM(*(zval_p))

#define Z_FE_POS(zval)				(zval).u2.fe_pos
#define Z_FE_POS_P(zval_p)			Z_FE_POS(*(zval_p))

#define Z_FE_ITER(zval)				(zval).u2.fe_iter_idx
#define Z_FE_ITER_P(zval_p)			Z_FE_ITER(*(zval_p))

#define Z_PROPERTY_GUARD(zval)		(zval).u2.property_guard
#define Z_PROPERTY_GUARD_P(zval_p)	Z_PROPERTY_GUARD(*(zval_p))

#define Z_CONSTANT_FLAGS(zval)		(zval).u2.constant_flags
#define Z_CONSTANT_FLAGS_P(zval_p)	Z_CONSTANT_FLAGS(*(zval_p))

#define Z_EXTRA(zval)				(zval).u2.extra
#define Z_EXTRA_P(zval_p)			Z_EXTRA(*(zval_p))

#define Z_COUNTED(zval)				(zval).value.counted
#define Z_COUNTED_P(zval_p)			Z_COUNTED(*(zval_p))

#define Z_TYPE_MASK					0xff
#define Z_TYPE_FLAGS_MASK			0xff00

#define Z_TYPE_FLAGS_SHIFT			8

#define GC_REFCOUNT(p)				zend_gc_refcount(&(p)->gc)
#define GC_SET_REFCOUNT(p, rc)		zend_gc_set_refcount(&(p)->gc, rc)
#define GC_ADDREF(p)				zend_gc_addref(&(p)->gc)
#define GC_DELREF(p)				zend_gc_delref(&(p)->gc)
#define GC_ADDREF_EX(p, rc)			zend_gc_addref_ex(&(p)->gc, rc)
#define GC_DELREF_EX(p, rc)			zend_gc_delref_ex(&(p)->gc, rc)
#define GC_TRY_ADDREF(p)			zend_gc_try_addref(&(p)->gc)
#define GC_TRY_DELREF(p)			zend_gc_try_delref(&(p)->gc)

#define GC_TYPE_MASK				0x0000000f
#define GC_FLAGS_MASK				0x000003f0
#define GC_INFO_MASK				0xfffffc00
#define GC_FLAGS_SHIFT				0
#define GC_INFO_SHIFT				10

static zend_always_inline zend_uchar zval_gc_type(uint32_t gc_type_info) {
	return (gc_type_info & GC_TYPE_MASK);
}

static zend_always_inline uint32_t zval_gc_flags(uint32_t gc_type_info) {
	return (gc_type_info >> GC_FLAGS_SHIFT) & (GC_FLAGS_MASK >> GC_FLAGS_SHIFT);
}

static zend_always_inline uint32_t zval_gc_info(uint32_t gc_type_info) {
	return (gc_type_info >> GC_INFO_SHIFT);
}

#define GC_TYPE_INFO(p)				(p)->gc.u.type_info
#define GC_TYPE(p)					zval_gc_type(GC_TYPE_INFO(p))
#define GC_FLAGS(p)					zval_gc_flags(GC_TYPE_INFO(p))
#define GC_INFO(p)					zval_gc_info(GC_TYPE_INFO(p))

#define GC_ADD_FLAGS(p, flags) do { \
		GC_TYPE_INFO(p) |= (flags) << GC_FLAGS_SHIFT; \
	} while (0)
#define GC_DEL_FLAGS(p, flags) do { \
		GC_TYPE_INFO(p) &= ~((flags) << GC_FLAGS_SHIFT); \
	} while (0)

#define Z_GC_TYPE(zval)				GC_TYPE(Z_COUNTED(zval))
#define Z_GC_TYPE_P(zval_p)			Z_GC_TYPE(*(zval_p))

#define Z_GC_FLAGS(zval)			GC_FLAGS(Z_COUNTED(zval))
#define Z_GC_FLAGS_P(zval_p)		Z_GC_FLAGS(*(zval_p))

#define Z_GC_INFO(zval)				GC_INFO(Z_COUNTED(zval))
#define Z_GC_INFO_P(zval_p)			Z_GC_INFO(*(zval_p))
#define Z_GC_TYPE_INFO(zval)		GC_TYPE_INFO(Z_COUNTED(zval))
#define Z_GC_TYPE_INFO_P(zval_p)	Z_GC_TYPE_INFO(*(zval_p))

/* zval_gc_flags(zval.value->gc.u.type_info) (common flags) */
#define GC_NOT_COLLECTABLE			(1<<4)
#define GC_PROTECTED                (1<<5) /* used for recursion detection */
#define GC_IMMUTABLE                (1<<6) /* can't be changed in place */
#define GC_PERSISTENT               (1<<7) /* allocated using malloc */
#define GC_PERSISTENT_LOCAL         (1<<8) /* persistent, but thread-local */

#define GC_NULL						(IS_NULL         | (GC_NOT_COLLECTABLE << GC_FLAGS_SHIFT))
#define GC_STRING					(IS_STRING       | (GC_NOT_COLLECTABLE << GC_FLAGS_SHIFT))
#define GC_ARRAY					IS_ARRAY
#define GC_OBJECT					IS_OBJECT
#define GC_RESOURCE					(IS_RESOURCE     | (GC_NOT_COLLECTABLE << GC_FLAGS_SHIFT))
#define GC_REFERENCE				(IS_REFERENCE    | (GC_NOT_COLLECTABLE << GC_FLAGS_SHIFT))
#define GC_CONSTANT_AST				(IS_CONSTANT_AST | (GC_NOT_COLLECTABLE << GC_FLAGS_SHIFT))

/* zval.u1.v.type_flags */
#define IS_TYPE_REFCOUNTED			(1<<0)
#define IS_TYPE_COLLECTABLE			(1<<1)

#if 1
/* This optimized version assumes that we have a single "type_flag" */
/* IS_TYPE_COLLECTABLE may be used only with IS_TYPE_REFCOUNTED */
# define Z_TYPE_INFO_REFCOUNTED(t)	(((t) & Z_TYPE_FLAGS_MASK) != 0)
#else
# define Z_TYPE_INFO_REFCOUNTED(t)	(((t) & (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT)) != 0)
#endif

/* extended types */
#define IS_INTERNED_STRING_EX		IS_STRING

#define IS_STRING_EX				(IS_STRING         | (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT))
#define IS_ARRAY_EX					(IS_ARRAY          | (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT) | (IS_TYPE_COLLECTABLE << Z_TYPE_FLAGS_SHIFT))
#define IS_OBJECT_EX				(IS_OBJECT         | (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT) | (IS_TYPE_COLLECTABLE << Z_TYPE_FLAGS_SHIFT))
#define IS_RESOURCE_EX				(IS_RESOURCE       | (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT))
#define IS_REFERENCE_EX				(IS_REFERENCE      | (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT))

#define IS_CONSTANT_AST_EX			(IS_CONSTANT_AST   | (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT))

/* string flags (zval.value->gc.u.flags) */
#define IS_STR_CLASS_NAME_MAP_PTR   GC_PROTECTED  /* refcount is a map_ptr offset of class_entry */
#define IS_STR_INTERNED				GC_IMMUTABLE  /* interned string */
#define IS_STR_PERSISTENT			GC_PERSISTENT /* allocated using malloc */
#define IS_STR_PERMANENT        	(1<<8)        /* relives request boundary */
#define IS_STR_VALID_UTF8           (1<<9)        /* valid UTF-8 according to PCRE */

/* array flags */
#define IS_ARRAY_IMMUTABLE			GC_IMMUTABLE
#define IS_ARRAY_PERSISTENT			GC_PERSISTENT

/* object flags (zval.value->gc.u.flags) */
#define IS_OBJ_WEAKLY_REFERENCED	GC_PERSISTENT
#define IS_OBJ_DESTRUCTOR_CALLED	(1<<8)
#define IS_OBJ_FREE_CALLED			(1<<9)

#define OBJ_FLAGS(obj)              GC_FLAGS(obj)

/* Fast class cache */
#define ZSTR_HAS_CE_CACHE(s)		(GC_FLAGS(s) & IS_STR_CLASS_NAME_MAP_PTR)
#define ZSTR_GET_CE_CACHE(s)		ZSTR_GET_CE_CACHE_EX(s, 1)
#define ZSTR_SET_CE_CACHE(s, ce)	ZSTR_SET_CE_CACHE_EX(s, ce, 1)

#define ZSTR_VALID_CE_CACHE(s)		EXPECTED((GC_REFCOUNT(s)-1)/sizeof(void *) < CG(map_ptr_last))

#define ZSTR_GET_CE_CACHE_EX(s, validate) \
	((!(validate) || ZSTR_VALID_CE_CACHE(s)) ? GET_CE_CACHE(GC_REFCOUNT(s)) : NULL)

#define ZSTR_SET_CE_CACHE_EX(s, ce, validate) do { \
		if (!(validate) || ZSTR_VALID_CE_CACHE(s)) { \
			ZEND_ASSERT((validate) || ZSTR_VALID_CE_CACHE(s)); \
			SET_CE_CACHE(GC_REFCOUNT(s), ce); \
		} \
	} while (0)

#define GET_CE_CACHE(ce_cache) \
	(*(zend_class_entry **)ZEND_MAP_PTR_OFFSET2PTR(ce_cache))

#define SET_CE_CACHE(ce_cache, ce) do { \
		*((zend_class_entry **)ZEND_MAP_PTR_OFFSET2PTR(ce_cache)) = ce; \
	} while (0)

/* Recursion protection macros must be used only for arrays and objects */
#define GC_IS_RECURSIVE(p) \
	(GC_FLAGS(p) & GC_PROTECTED)

#define GC_PROTECT_RECURSION(p) do { \
		GC_ADD_FLAGS(p, GC_PROTECTED); \
	} while (0)

#define GC_UNPROTECT_RECURSION(p) do { \
		GC_DEL_FLAGS(p, GC_PROTECTED); \
	} while (0)

#define GC_TRY_PROTECT_RECURSION(p) do { \
		if (!(GC_FLAGS(p) & GC_IMMUTABLE)) GC_PROTECT_RECURSION(p); \
	} while (0)

#define GC_TRY_UNPROTECT_RECURSION(p) do { \
		if (!(GC_FLAGS(p) & GC_IMMUTABLE)) GC_UNPROTECT_RECURSION(p); \
	} while (0)

#define Z_IS_RECURSIVE(zval)        GC_IS_RECURSIVE(Z_COUNTED(zval))
#define Z_PROTECT_RECURSION(zval)   GC_PROTECT_RECURSION(Z_COUNTED(zval))
#define Z_UNPROTECT_RECURSION(zval) GC_UNPROTECT_RECURSION(Z_COUNTED(zval))
#define Z_IS_RECURSIVE_P(zv)        Z_IS_RECURSIVE(*(zv))
#define Z_PROTECT_RECURSION_P(zv)   Z_PROTECT_RECURSION(*(zv))
#define Z_UNPROTECT_RECURSION_P(zv) Z_UNPROTECT_RECURSION(*(zv))

/* All data types < IS_STRING have their constructor/destructors skipped */
#define Z_CONSTANT(zval)			(Z_TYPE(zval) == IS_CONSTANT_AST)
#define Z_CONSTANT_P(zval_p)		Z_CONSTANT(*(zval_p))

#if 1
/* This optimized version assumes that we have a single "type_flag" */
/* IS_TYPE_COLLECTABLE may be used only with IS_TYPE_REFCOUNTED */
#define Z_REFCOUNTED(zval)			(Z_TYPE_FLAGS(zval) != 0)
#else
#define Z_REFCOUNTED(zval)			((Z_TYPE_FLAGS(zval) & IS_TYPE_REFCOUNTED) != 0)
#endif
#define Z_REFCOUNTED_P(zval_p)		Z_REFCOUNTED(*(zval_p))

#define Z_COLLECTABLE(zval)			((Z_TYPE_FLAGS(zval) & IS_TYPE_COLLECTABLE) != 0)
#define Z_COLLECTABLE_P(zval_p)		Z_COLLECTABLE(*(zval_p))

/* deprecated: (COPYABLE is the same as IS_ARRAY) */
#define Z_COPYABLE(zval)			(Z_TYPE(zval) == IS_ARRAY)
#define Z_COPYABLE_P(zval_p)		Z_COPYABLE(*(zval_p))

/* deprecated: (IMMUTABLE is the same as IS_ARRAY && !REFCOUNTED) */
#define Z_IMMUTABLE(zval)			(Z_TYPE_INFO(zval) == IS_ARRAY)
#define Z_IMMUTABLE_P(zval_p)		Z_IMMUTABLE(*(zval_p))
#define Z_OPT_IMMUTABLE(zval)		Z_IMMUTABLE(zval_p)
#define Z_OPT_IMMUTABLE_P(zval_p)	Z_IMMUTABLE(*(zval_p))

/* the following Z_OPT_* macros make better code when Z_TYPE_INFO accessed before */
#define Z_OPT_TYPE(zval)			(Z_TYPE_INFO(zval) & Z_TYPE_MASK)
#define Z_OPT_TYPE_P(zval_p)		Z_OPT_TYPE(*(zval_p))

#define Z_OPT_CONSTANT(zval)		(Z_OPT_TYPE(zval) == IS_CONSTANT_AST)
#define Z_OPT_CONSTANT_P(zval_p)	Z_OPT_CONSTANT(*(zval_p))

#define Z_OPT_REFCOUNTED(zval)		Z_TYPE_INFO_REFCOUNTED(Z_TYPE_INFO(zval))
#define Z_OPT_REFCOUNTED_P(zval_p)	Z_OPT_REFCOUNTED(*(zval_p))

/* deprecated: (COPYABLE is the same as IS_ARRAY) */
#define Z_OPT_COPYABLE(zval)		(Z_OPT_TYPE(zval) == IS_ARRAY)
#define Z_OPT_COPYABLE_P(zval_p)	Z_OPT_COPYABLE(*(zval_p))

#define Z_OPT_ISREF(zval)			(Z_OPT_TYPE(zval) == IS_REFERENCE)
#define Z_OPT_ISREF_P(zval_p)		Z_OPT_ISREF(*(zval_p))

#define Z_ISREF(zval)				(Z_TYPE(zval) == IS_REFERENCE)
#define Z_ISREF_P(zval_p)			Z_ISREF(*(zval_p))

#define Z_ISUNDEF(zval)				(Z_TYPE(zval) == IS_UNDEF)
#define Z_ISUNDEF_P(zval_p)			Z_ISUNDEF(*(zval_p))

#define Z_ISNULL(zval)				(Z_TYPE(zval) == IS_NULL)
#define Z_ISNULL_P(zval_p)			Z_ISNULL(*(zval_p))

#define Z_ISERROR(zval)				(Z_TYPE(zval) == _IS_ERROR)
#define Z_ISERROR_P(zval_p)			Z_ISERROR(*(zval_p))

#define Z_LVAL(zval)				(zval).value.lval
#define Z_LVAL_P(zval_p)			Z_LVAL(*(zval_p))

#define Z_DVAL(zval)				(zval).value.dval
#define Z_DVAL_P(zval_p)			Z_DVAL(*(zval_p))

#define Z_STR(zval)					(zval).value.str
#define Z_STR_P(zval_p)				Z_STR(*(zval_p))

#define Z_STRVAL(zval)				ZSTR_VAL(Z_STR(zval))
#define Z_STRVAL_P(zval_p)			Z_STRVAL(*(zval_p))

#define Z_STRLEN(zval)				ZSTR_LEN(Z_STR(zval))
#define Z_STRLEN_P(zval_p)			Z_STRLEN(*(zval_p))

#define Z_STRHASH(zval)				ZSTR_HASH(Z_STR(zval))
#define Z_STRHASH_P(zval_p)			Z_STRHASH(*(zval_p))

#define Z_ARR(zval)					(zval).value.arr
#define Z_ARR_P(zval_p)				Z_ARR(*(zval_p))

#define Z_ARRVAL(zval)				Z_ARR(zval)
#define Z_ARRVAL_P(zval_p)			Z_ARRVAL(*(zval_p))

#define Z_OBJ(zval)					(zval).value.obj
#define Z_OBJ_P(zval_p)				Z_OBJ(*(zval_p))

#define Z_OBJ_HT(zval)				Z_OBJ(zval)->handlers
#define Z_OBJ_HT_P(zval_p)			Z_OBJ_HT(*(zval_p))

#define Z_OBJ_HANDLER(zval, hf)		Z_OBJ_HT((zval))->hf
#define Z_OBJ_HANDLER_P(zv_p, hf)	Z_OBJ_HANDLER(*(zv_p), hf)

#define Z_OBJ_HANDLE(zval)          (Z_OBJ((zval)))->handle
#define Z_OBJ_HANDLE_P(zval_p)      Z_OBJ_HANDLE(*(zval_p))

#define Z_OBJCE(zval)				(Z_OBJ(zval)->ce)
#define Z_OBJCE_P(zval_p)			Z_OBJCE(*(zval_p))

#define Z_OBJPROP(zval)				Z_OBJ_HT((zval))->get_properties(Z_OBJ(zval))
#define Z_OBJPROP_P(zval_p)			Z_OBJPROP(*(zval_p))

#define Z_RES(zval)					(zval).value.res
#define Z_RES_P(zval_p)				Z_RES(*zval_p)

#define Z_RES_HANDLE(zval)			Z_RES(zval)->handle
#define Z_RES_HANDLE_P(zval_p)		Z_RES_HANDLE(*zval_p)

#define Z_RES_TYPE(zval)			Z_RES(zval)->type
#define Z_RES_TYPE_P(zval_p)		Z_RES_TYPE(*zval_p)

#define Z_RES_VAL(zval)				Z_RES(zval)->ptr
#define Z_RES_VAL_P(zval_p)			Z_RES_VAL(*zval_p)

#define Z_REF(zval)					(zval).value.ref
#define Z_REF_P(zval_p)				Z_REF(*(zval_p))

#define Z_REFVAL(zval)				&Z_REF(zval)->val
#define Z_REFVAL_P(zval_p)			Z_REFVAL(*(zval_p))

#define Z_AST(zval)					(zval).value.ast
#define Z_AST_P(zval_p)				Z_AST(*(zval_p))

#define GC_AST(p)					((zend_ast*)(((char*)p) + sizeof(zend_ast_ref)))

#define Z_ASTVAL(zval)				GC_AST(Z_AST(zval))
#define Z_ASTVAL_P(zval_p)			Z_ASTVAL(*(zval_p))

#define Z_INDIRECT(zval)			(zval).value.zv
#define Z_INDIRECT_P(zval_p)		Z_INDIRECT(*(zval_p))

#define Z_CE(zval)					(zval).value.ce
#define Z_CE_P(zval_p)				Z_CE(*(zval_p))

#define Z_FUNC(zval)				(zval).value.func
#define Z_FUNC_P(zval_p)			Z_FUNC(*(zval_p))

#define Z_PTR(zval)					(zval).value.ptr
#define Z_PTR_P(zval_p)				Z_PTR(*(zval_p))

#define ZVAL_UNDEF(z) do {				\
		Z_TYPE_INFO_P(z) = IS_UNDEF;	\
	} while (0)

#define ZVAL_NULL(z) do {				\
		Z_TYPE_INFO_P(z) = IS_NULL;		\
	} while (0)

#define ZVAL_FALSE(z) do {				\
		Z_TYPE_INFO_P(z) = IS_FALSE;	\
	} while (0)

#define ZVAL_TRUE(z) do {				\
		Z_TYPE_INFO_P(z) = IS_TRUE;		\
	} while (0)

#define ZVAL_BOOL(z, b) do {			\
		Z_TYPE_INFO_P(z) =				\
			(b) ? IS_TRUE : IS_FALSE;	\
	} while (0)

#define ZVAL_LONG(z, l) do {			\
		zval *__z = (z);				\
		Z_LVAL_P(__z) = l;				\
		Z_TYPE_INFO_P(__z) = IS_LONG;	\
	} while (0)

#define ZVAL_DOUBLE(z, d) do {			\
		zval *__z = (z);				\
		Z_DVAL_P(__z) = d;				\
		Z_TYPE_INFO_P(__z) = IS_DOUBLE;	\
	} while (0)

#define ZVAL_STR(z, s) do {						\
		zval *__z = (z);						\
		zend_string *__s = (s);					\
		Z_STR_P(__z) = __s;						\
		/* interned strings support */			\
		Z_TYPE_INFO_P(__z) = ZSTR_IS_INTERNED(__s) ? \
			IS_INTERNED_STRING_EX : 			\
			IS_STRING_EX;						\
	} while (0)

#define ZVAL_INTERNED_STR(z, s) do {				\
		zval *__z = (z);							\
		zend_string *__s = (s);						\
		Z_STR_P(__z) = __s;							\
		Z_TYPE_INFO_P(__z) = IS_INTERNED_STRING_EX;	\
	} while (0)

#define ZVAL_NEW_STR(z, s) do {					\
		zval *__z = (z);						\
		zend_string *__s = (s);					\
		Z_STR_P(__z) = __s;						\
		Z_TYPE_INFO_P(__z) = IS_STRING_EX;		\
	} while (0)

#define ZVAL_STR_COPY(z, s) do {						\
		zval *__z = (z);								\
		zend_string *__s = (s);							\
		Z_STR_P(__z) = __s;								\
		/* interned strings support */					\
		if (ZSTR_IS_INTERNED(__s)) {					\
			Z_TYPE_INFO_P(__z) = IS_INTERNED_STRING_EX;	\
		} else {										\
			GC_ADDREF(__s);								\
			Z_TYPE_INFO_P(__z) = IS_STRING_EX;			\
		}												\
	} while (0)

#define ZVAL_ARR(z, a) do {						\
		zend_array *__arr = (a);				\
		zval *__z = (z);						\
		Z_ARR_P(__z) = __arr;					\
		Z_TYPE_INFO_P(__z) = IS_ARRAY_EX;		\
	} while (0)

#define ZVAL_NEW_PERSISTENT_ARR(z) do {							\
		zval *__z = (z);										\
		zend_array *_arr =										\
		(zend_array *) malloc(sizeof(zend_array));				\
		Z_ARR_P(__z) = _arr;									\
		Z_TYPE_INFO_P(__z) = IS_ARRAY_EX;						\
	} while (0)

#define ZVAL_OBJ(z, o) do {						\
		zval *__z = (z);						\
		Z_OBJ_P(__z) = (o);						\
		Z_TYPE_INFO_P(__z) = IS_OBJECT_EX;		\
	} while (0)

#define ZVAL_OBJ_COPY(z, o) do {				\
		zval *__z = (z);						\
		zend_object *__o = (o);					\
		GC_ADDREF(__o);							\
		Z_OBJ_P(__z) = __o;						\
		Z_TYPE_INFO_P(__z) = IS_OBJECT_EX;		\
	} while (0)

#define ZVAL_RES(z, r) do {						\
		zval *__z = (z);						\
		Z_RES_P(__z) = (r);						\
		Z_TYPE_INFO_P(__z) = IS_RESOURCE_EX;	\
	} while (0)

#define ZVAL_NEW_RES(z, h, p, t) do {							\
		zend_resource *_res =									\
		(zend_resource *) emalloc(sizeof(zend_resource));		\
		zval *__z;												\
		GC_SET_REFCOUNT(_res, 1);								\
		GC_TYPE_INFO(_res) = GC_RESOURCE;						\
		_res->handle = (h);										\
		_res->type = (t);										\
		_res->ptr = (p);										\
		__z = (z);												\
		Z_RES_P(__z) = _res;									\
		Z_TYPE_INFO_P(__z) = IS_RESOURCE_EX;					\
	} while (0)

#define ZVAL_NEW_PERSISTENT_RES(z, h, p, t) do {				\
		zend_resource *_res =									\
		(zend_resource *) malloc(sizeof(zend_resource));		\
		zval *__z;												\
		GC_SET_REFCOUNT(_res, 1);								\
		GC_TYPE_INFO(_res) = GC_RESOURCE |						\
			(GC_PERSISTENT << GC_FLAGS_SHIFT);					\
		_res->handle = (h);										\
		_res->type = (t);										\
		_res->ptr = (p);										\
		__z = (z);												\
		Z_RES_P(__z) = _res;									\
		Z_TYPE_INFO_P(__z) = IS_RESOURCE_EX;					\
	} while (0)

#define ZVAL_REF(z, r) do {										\
		zval *__z = (z);										\
		Z_REF_P(__z) = (r);										\
		Z_TYPE_INFO_P(__z) = IS_REFERENCE_EX;					\
	} while (0)

#define ZVAL_NEW_EMPTY_REF(z) do {								\
		zend_reference *_ref =									\
		(zend_reference *) emalloc(sizeof(zend_reference));		\
		GC_SET_REFCOUNT(_ref, 1);								\
		GC_TYPE_INFO(_ref) = GC_REFERENCE;						\
		_ref->sources.ptr = NULL;									\
		Z_REF_P(z) = _ref;										\
		Z_TYPE_INFO_P(z) = IS_REFERENCE_EX;						\
	} while (0)

#define ZVAL_NEW_REF(z, r) do {									\
		zend_reference *_ref =									\
		(zend_reference *) emalloc(sizeof(zend_reference));		\
		GC_SET_REFCOUNT(_ref, 1);								\
		GC_TYPE_INFO(_ref) = GC_REFERENCE;						\
		ZVAL_COPY_VALUE(&_ref->val, r);							\
		_ref->sources.ptr = NULL;									\
		Z_REF_P(z) = _ref;										\
		Z_TYPE_INFO_P(z) = IS_REFERENCE_EX;						\
	} while (0)

#define ZVAL_MAKE_REF_EX(z, refcount) do {						\
		zval *_z = (z);											\
		zend_reference *_ref =									\
			(zend_reference *) emalloc(sizeof(zend_reference));	\
		GC_SET_REFCOUNT(_ref, (refcount));						\
		GC_TYPE_INFO(_ref) = GC_REFERENCE;						\
		ZVAL_COPY_VALUE(&_ref->val, _z);						\
		_ref->sources.ptr = NULL;									\
		Z_REF_P(_z) = _ref;										\
		Z_TYPE_INFO_P(_z) = IS_REFERENCE_EX;					\
	} while (0)

#define ZVAL_NEW_PERSISTENT_REF(z, r) do {						\
		zend_reference *_ref =									\
		(zend_reference *) malloc(sizeof(zend_reference));		\
		GC_SET_REFCOUNT(_ref, 1);								\
		GC_TYPE_INFO(_ref) = GC_REFERENCE |						\
			(GC_PERSISTENT << GC_FLAGS_SHIFT);					\
		ZVAL_COPY_VALUE(&_ref->val, r);							\
		_ref->sources.ptr = NULL;									\
		Z_REF_P(z) = _ref;										\
		Z_TYPE_INFO_P(z) = IS_REFERENCE_EX;						\
	} while (0)

#define ZVAL_AST(z, ast) do {									\
		zval *__z = (z);										\
		Z_AST_P(__z) = ast;										\
		Z_TYPE_INFO_P(__z) = IS_CONSTANT_AST_EX;				\
	} while (0)

#define ZVAL_INDIRECT(z, v) do {								\
		Z_INDIRECT_P(z) = (v);									\
		Z_TYPE_INFO_P(z) = IS_INDIRECT;							\
	} while (0)

#define ZVAL_PTR(z, p) do {										\
		Z_PTR_P(z) = (p);										\
		Z_TYPE_INFO_P(z) = IS_PTR;								\
	} while (0)

#define ZVAL_FUNC(z, f) do {									\
		Z_FUNC_P(z) = (f);										\
		Z_TYPE_INFO_P(z) = IS_PTR;								\
	} while (0)

#define ZVAL_CE(z, c) do {										\
		Z_CE_P(z) = (c);										\
		Z_TYPE_INFO_P(z) = IS_PTR;								\
	} while (0)

#define ZVAL_ALIAS_PTR(z, p) do {								\
		Z_PTR_P(z) = (p);										\
		Z_TYPE_INFO_P(z) = IS_ALIAS_PTR;						\
	} while (0)

#define ZVAL_ERROR(z) do {				\
		Z_TYPE_INFO_P(z) = _IS_ERROR;	\
	} while (0)

#define Z_REFCOUNT_P(pz)			zval_refcount_p(pz)
#define Z_SET_REFCOUNT_P(pz, rc)	zval_set_refcount_p(pz, rc)
#define Z_ADDREF_P(pz)				zval_addref_p(pz)
#define Z_DELREF_P(pz)				zval_delref_p(pz)

#define Z_REFCOUNT(z)				Z_REFCOUNT_P(&(z))
#define Z_SET_REFCOUNT(z, rc)		Z_SET_REFCOUNT_P(&(z), rc)
#define Z_ADDREF(z)					Z_ADDREF_P(&(z))
#define Z_DELREF(z)					Z_DELREF_P(&(z))

#define Z_TRY_ADDREF_P(pz) do {		\
	if (Z_REFCOUNTED_P((pz))) {		\
		Z_ADDREF_P((pz));			\
	}								\
} while (0)

#define Z_TRY_DELREF_P(pz) do {		\
	if (Z_REFCOUNTED_P((pz))) {		\
		Z_DELREF_P((pz));			\
	}								\
} while (0)

#define Z_TRY_ADDREF(z)				Z_TRY_ADDREF_P(&(z))
#define Z_TRY_DELREF(z)				Z_TRY_DELREF_P(&(z))

#ifndef ZEND_RC_DEBUG
# define ZEND_RC_DEBUG 0
#endif

#if ZEND_RC_DEBUG
extern ZEND_API bool zend_rc_debug;
/* The GC_PERSISTENT flag is reused for IS_OBJ_WEAKLY_REFERENCED on objects.
 * Skip checks for OBJECT/NULL type to avoid interpreting the flag incorrectly. */
# define ZEND_RC_MOD_CHECK(p) do { \
		if (zend_rc_debug) { \
			zend_uchar type = zval_gc_type((p)->u.type_info); \
			if (type != IS_OBJECT && type != IS_NULL) { \
				ZEND_ASSERT(!(zval_gc_flags((p)->u.type_info) & GC_IMMUTABLE)); \
				ZEND_ASSERT((zval_gc_flags((p)->u.type_info) & (GC_PERSISTENT|GC_PERSISTENT_LOCAL)) != GC_PERSISTENT); \
			} \
		} \
	} while (0)
# define GC_MAKE_PERSISTENT_LOCAL(p) do { \
		GC_ADD_FLAGS(p, GC_PERSISTENT_LOCAL); \
	} while (0)
#else
# define ZEND_RC_MOD_CHECK(p) \
	do { } while (0)
# define GC_MAKE_PERSISTENT_LOCAL(p) \
	do { } while (0)
#endif

static zend_always_inline uint32_t zend_gc_refcount(const zend_refcounted_h *p) {
	return p->refcount;
}

static zend_always_inline uint32_t zend_gc_set_refcount(zend_refcounted_h *p, uint32_t rc) {
	p->refcount = rc;
	return p->refcount;
}

static zend_always_inline uint32_t zend_gc_addref(zend_refcounted_h *p) {
	ZEND_RC_MOD_CHECK(p);
	return ++(p->refcount);
}

static zend_always_inline void zend_gc_try_addref(zend_refcounted_h *p) {
	if (!(p->u.type_info & GC_IMMUTABLE)) {
		ZEND_RC_MOD_CHECK(p);
		++p->refcount;
	}
}

static zend_always_inline void zend_gc_try_delref(zend_refcounted_h *p) {
	if (!(p->u.type_info & GC_IMMUTABLE)) {
		ZEND_RC_MOD_CHECK(p);
		--p->refcount;
	}
}

static zend_always_inline uint32_t zend_gc_delref(zend_refcounted_h *p) {
	ZEND_ASSERT(p->refcount > 0);
	ZEND_RC_MOD_CHECK(p);
	return --(p->refcount);
}

static zend_always_inline uint32_t zend_gc_addref_ex(zend_refcounted_h *p, uint32_t rc) {
	ZEND_RC_MOD_CHECK(p);
	p->refcount += rc;
	return p->refcount;
}

static zend_always_inline uint32_t zend_gc_delref_ex(zend_refcounted_h *p, uint32_t rc) {
	ZEND_RC_MOD_CHECK(p);
	p->refcount -= rc;
	return p->refcount;
}

static zend_always_inline uint32_t zval_refcount_p(const zval* pz) {
#if ZEND_DEBUG
	ZEND_ASSERT(Z_REFCOUNTED_P(pz) || Z_TYPE_P(pz) == IS_ARRAY);
#endif
	return GC_REFCOUNT(Z_COUNTED_P(pz));
}

static zend_always_inline uint32_t zval_set_refcount_p(zval* pz, uint32_t rc) {
	ZEND_ASSERT(Z_REFCOUNTED_P(pz));
	return GC_SET_REFCOUNT(Z_COUNTED_P(pz), rc);
}

static zend_always_inline uint32_t zval_addref_p(zval* pz) {
	ZEND_ASSERT(Z_REFCOUNTED_P(pz));
	return GC_ADDREF(Z_COUNTED_P(pz));
}

static zend_always_inline uint32_t zval_delref_p(zval* pz) {
	ZEND_ASSERT(Z_REFCOUNTED_P(pz));
	return GC_DELREF(Z_COUNTED_P(pz));
}

#if SIZEOF_SIZE_T == 4
# define ZVAL_COPY_VALUE_EX(z, v, gc, t)				\
	do {												\
		uint32_t _w2 = v->value.ww.w2;					\
		Z_COUNTED_P(z) = gc;							\
		z->value.ww.w2 = _w2;							\
		Z_TYPE_INFO_P(z) = t;							\
	} while (0)
#elif SIZEOF_SIZE_T == 8
# define ZVAL_COPY_VALUE_EX(z, v, gc, t)				\
	do {												\
		Z_COUNTED_P(z) = gc;							\
		Z_TYPE_INFO_P(z) = t;							\
	} while (0)
#else
# error "Unknown SIZEOF_SIZE_T"
#endif

#define ZVAL_COPY_VALUE(z, v)							\
	do {												\
		zval *_z1 = (z);								\
		const zval *_z2 = (v);							\
		zend_refcounted *_gc = Z_COUNTED_P(_z2);		\
		uint32_t _t = Z_TYPE_INFO_P(_z2);				\
		ZVAL_COPY_VALUE_EX(_z1, _z2, _gc, _t);			\
	} while (0)

#define ZVAL_COPY(z, v)									\
	do {												\
		zval *_z1 = (z);								\
		const zval *_z2 = (v);							\
		zend_refcounted *_gc = Z_COUNTED_P(_z2);		\
		uint32_t _t = Z_TYPE_INFO_P(_z2);				\
		ZVAL_COPY_VALUE_EX(_z1, _z2, _gc, _t);			\
		if (Z_TYPE_INFO_REFCOUNTED(_t)) {				\
			GC_ADDREF(_gc);								\
		}												\
	} while (0)

#define ZVAL_DUP(z, v)									\
	do {												\
		zval *_z1 = (z);								\
		const zval *_z2 = (v);							\
		zend_refcounted *_gc = Z_COUNTED_P(_z2);		\
		uint32_t _t = Z_TYPE_INFO_P(_z2);				\
		if ((_t & Z_TYPE_MASK) == IS_ARRAY) {			\
			ZVAL_ARR(_z1, zend_array_dup((zend_array*)_gc));\
		} else {										\
			if (Z_TYPE_INFO_REFCOUNTED(_t)) {			\
				GC_ADDREF(_gc);							\
			}											\
			ZVAL_COPY_VALUE_EX(_z1, _z2, _gc, _t);		\
		}												\
	} while (0)


/* ZVAL_COPY_OR_DUP() should be used instead of ZVAL_COPY() and ZVAL_DUP()
 * in all places where the source may be a persistent zval.
 */
#define ZVAL_COPY_OR_DUP(z, v)											\
	do {																\
		zval *_z1 = (z);												\
		const zval *_z2 = (v);											\
		zend_refcounted *_gc = Z_COUNTED_P(_z2);						\
		uint32_t _t = Z_TYPE_INFO_P(_z2);								\
		ZVAL_COPY_VALUE_EX(_z1, _z2, _gc, _t);							\
		if (Z_TYPE_INFO_REFCOUNTED(_t)) {								\
			/* Objects reuse PERSISTENT as WEAKLY_REFERENCED */			\
			if (EXPECTED(!(GC_FLAGS(_gc) & GC_PERSISTENT)				\
					|| GC_TYPE(_gc) == IS_OBJECT)) {					\
				GC_ADDREF(_gc);											\
			} else {													\
				zval_copy_ctor_func(_z1);								\
			}															\
		}																\
	} while (0)

#define ZVAL_DEREF(z) do {								\
		if (UNEXPECTED(Z_ISREF_P(z))) {					\
			(z) = Z_REFVAL_P(z);						\
		}												\
	} while (0)

#define ZVAL_DEINDIRECT(z) do {							\
		if (Z_TYPE_P(z) == IS_INDIRECT) {				\
			(z) = Z_INDIRECT_P(z);						\
		}												\
	} while (0)

#define ZVAL_OPT_DEREF(z) do {							\
		if (UNEXPECTED(Z_OPT_ISREF_P(z))) {				\
			(z) = Z_REFVAL_P(z);						\
		}												\
	} while (0)

#define ZVAL_MAKE_REF(zv) do {							\
		zval *__zv = (zv);								\
		if (!Z_ISREF_P(__zv)) {							\
			ZVAL_NEW_REF(__zv, __zv);					\
		}												\
	} while (0)

#define ZVAL_UNREF(z) do {								\
		zval *_z = (z);									\
		zend_reference *ref;							\
		ZEND_ASSERT(Z_ISREF_P(_z));						\
		ref = Z_REF_P(_z);								\
		ZVAL_COPY_VALUE(_z, &ref->val);					\
		efree_size(ref, sizeof(zend_reference));		\
	} while (0)

#define ZVAL_COPY_DEREF(z, v) do {						\
		zval *_z3 = (v);								\
		if (Z_OPT_REFCOUNTED_P(_z3)) {					\
			if (UNEXPECTED(Z_OPT_ISREF_P(_z3))) {		\
				_z3 = Z_REFVAL_P(_z3);					\
				if (Z_OPT_REFCOUNTED_P(_z3)) {			\
					Z_ADDREF_P(_z3);					\
				}										\
			} else {									\
				Z_ADDREF_P(_z3);						\
			}											\
		}												\
		ZVAL_COPY_VALUE(z, _z3);						\
	} while (0)


#define SEPARATE_STRING(zv) do {						\
		zval *_zv = (zv);								\
		if (Z_REFCOUNT_P(_zv) > 1) {					\
			zend_string *_str = Z_STR_P(_zv);			\
			ZEND_ASSERT(Z_REFCOUNTED_P(_zv));			\
			ZEND_ASSERT(!ZSTR_IS_INTERNED(_str));		\
			ZVAL_NEW_STR(_zv, zend_string_init(			\
				ZSTR_VAL(_str),	ZSTR_LEN(_str), 0));	\
			GC_DELREF(_str);							\
		}												\
	} while (0)

#define SEPARATE_ARRAY(zv) do {							\
		zval *__zv = (zv);								\
		zend_array *_arr = Z_ARR_P(__zv);				\
		if (UNEXPECTED(GC_REFCOUNT(_arr) > 1)) {		\
			ZVAL_ARR(__zv, zend_array_dup(_arr));		\
			GC_TRY_DELREF(_arr);						\
		}												\
	} while (0)

#define SEPARATE_ZVAL_NOREF(zv) do {					\
		zval *_zv = (zv);								\
		ZEND_ASSERT(Z_TYPE_P(_zv) != IS_REFERENCE);		\
		if (Z_TYPE_P(_zv) == IS_ARRAY) {				\
			SEPARATE_ARRAY(_zv);						\
		}												\
	} while (0)

#define SEPARATE_ZVAL(zv) do {							\
		zval *_zv = (zv);								\
		if (Z_ISREF_P(_zv)) {							\
			zend_reference *_r = Z_REF_P(_zv);			\
			ZVAL_COPY_VALUE(_zv, &_r->val);				\
			if (GC_DELREF(_r) == 0) {					\
				efree_size(_r, sizeof(zend_reference));	\
			} else if (Z_OPT_TYPE_P(_zv) == IS_ARRAY) {	\
				ZVAL_ARR(_zv, zend_array_dup(Z_ARR_P(_zv)));\
				break;									\
			} else if (Z_OPT_REFCOUNTED_P(_zv)) {		\
				Z_ADDREF_P(_zv);						\
				break;									\
			}											\
		}												\
		if (Z_TYPE_P(_zv) == IS_ARRAY) {				\
			SEPARATE_ARRAY(_zv);						\
		}												\
	} while (0)

/* Properties store a flag distinguishing unset and uninitialized properties
 * (both use IS_UNDEF type) in the Z_EXTRA space. As such we also need to copy
 * the Z_EXTRA space when copying property default values etc. We define separate
 * macros for this purpose, so this workaround is easier to remove in the future. */
#define IS_PROP_UNINIT 1
#define Z_PROP_FLAG_P(z) Z_EXTRA_P(z)
#define ZVAL_COPY_VALUE_PROP(z, v) \
	do { *(z) = *(v); } while (0)
#define ZVAL_COPY_PROP(z, v) \
	do { ZVAL_COPY(z, v); Z_PROP_FLAG_P(z) = Z_PROP_FLAG_P(v); } while (0)
#define ZVAL_COPY_OR_DUP_PROP(z, v) \
	do { ZVAL_COPY_OR_DUP(z, v); Z_PROP_FLAG_P(z) = Z_PROP_FLAG_P(v); } while (0)


#endif /* ZEND_TYPES_H */
