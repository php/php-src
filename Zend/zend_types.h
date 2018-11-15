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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Dmitry Stogov <dmitry@php.net>                              |
   |          Xinchen Hui <xinchen.h@zend.com>                            |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_TYPES_H
#define ZEND_TYPES_H

#include "zend_portability.h"
#include "zend_long.h"

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

typedef unsigned char zend_bool;
typedef unsigned char zend_uchar;

typedef enum {
  SUCCESS =  0,
  FAILURE = -1,		/* this MUST stay a negative number, or it may affect functions! */
} ZEND_RESULT_CODE;

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
 * ZEND_TYPE_IS_SET()     - checks if type-hint exists
 * ZEND_TYPE_IS_CODE()    - checks if type-hint refer to standard type
 * ZEND_TYPE_IS_CLASS()   - checks if type-hint refer to some class
 *
 * ZEND_TYPE_NAME()       - returns referenced class name
 * ZEND_TYPE_CE()         - returns referenced class entry
 * ZEND_TYPE_CODE()       - returns standard type code (e.g. IS_LONG, _IS_BOOL)
 *
 * ZEND_TYPE_ALLOW_NULL() - checks if NULL is allowed
 *
 * ZEND_TYPE_ENCODE() and ZEND_TYPE_ENCODE_CLASS() should be used for
 * construction.
 */

typedef uintptr_t zend_type;

#define ZEND_TYPE_IS_SET(t) \
	((t) > Z_L(1))

#define ZEND_TYPE_IS_CODE(t) \
	(((t) > Z_L(1)) && ((t) <= Z_L(0x1ff)))

#define ZEND_TYPE_IS_CLASS(t) \
	((t) > Z_L(0x1ff))

#define ZEND_TYPE_NAME(t) \
	((zend_string*)((t) & ~Z_L(0x3)))

#define ZEND_TYPE_CE(t) \
	((zend_class_entry*)((t) & ~Z_L(0x3)))

#define ZEND_TYPE_CODE(t) \
	((t) >> Z_L(1))

#define ZEND_TYPE_ALLOW_NULL(t) \
	(((t) & Z_L(0x1)) != 0)

#define ZEND_TYPE_ENCODE(code, allow_null) \
	(((code) << Z_L(1)) | ((allow_null) ? Z_L(0x1) : Z_L(0x0)))

#define ZEND_TYPE_ENCODE_CLASS(class_name, allow_null) \
	(((uintptr_t)(class_name)) | ((allow_null) ? Z_L(0x1) : Z_L(0)))

#define ZEND_TYPE_ENCODE_CLASS_CONST_0(class_name) \
	((zend_type) class_name)
#define ZEND_TYPE_ENCODE_CLASS_CONST_1(class_name) \
	((zend_type) "?" class_name)
#define ZEND_TYPE_ENCODE_CLASS_CONST_Q2(macro, class_name) \
	macro(class_name)
#define ZEND_TYPE_ENCODE_CLASS_CONST_Q1(allow_null, class_name) \
	ZEND_TYPE_ENCODE_CLASS_CONST_Q2(ZEND_TYPE_ENCODE_CLASS_CONST_ ##allow_null, class_name)
#define ZEND_TYPE_ENCODE_CLASS_CONST(class_name, allow_null) \
	ZEND_TYPE_ENCODE_CLASS_CONST_Q1(allow_null, class_name)

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
		struct {
			ZEND_ENDIAN_LOHI_3(
				zend_uchar    type,			/* active type */
				zend_uchar    type_flags,
				union {
					uint16_t  call_info;    /* call info for EX(This) */
					uint16_t  extra;        /* not further specified */
				} u)
		} v;
		uint32_t type_info;
	} u1;
	union {
		uint32_t     next;                 /* hash collision chain */
		uint32_t     cache_slot;           /* cache slot (for RECV_INIT) */
		uint32_t     opline_num;           /* opline number (for FAST_CALL) */
		uint32_t     lineno;               /* line number (for ast nodes) */
		uint32_t     num_args;             /* arguments number for EX(This) */
		uint32_t     fe_pos;               /* foreach position */
		uint32_t     fe_iter_idx;          /* foreach iterator index */
		uint32_t     access_flags;         /* class constant access flags */
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
	Bucket           *arData;
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
 *                 | HT_HASH(ht, ht->nTableMask) |
 *                 | ...                         |
 *                 | HT_HASH(ht, -1)             |
 *                 +-----------------------------+
 * ht->arData ---> | Bucket[0]                   |
 *                 | ...                         |
 *                 | Bucket[ht->nTableSize-1]    |
 *                 +=============================+
 */

#define HT_INVALID_IDX ((uint32_t) -1)

#define HT_MIN_MASK ((uint32_t) -2)
#define HT_MIN_SIZE 8

#if SIZEOF_SIZE_T == 4
# define HT_MAX_SIZE 0x04000000 /* small enough to avoid overflow checks */
# define HT_HASH_TO_BUCKET_EX(data, idx) \
	((Bucket*)((char*)(data) + (idx)))
# define HT_IDX_TO_HASH(idx) \
	((idx) * sizeof(Bucket))
# define HT_HASH_TO_IDX(idx) \
	((idx) / sizeof(Bucket))
#elif SIZEOF_SIZE_T == 8
# define HT_MAX_SIZE 0x80000000
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
	HT_HASH_EX((ht)->arData, idx)

#define HT_SIZE_TO_MASK(nTableSize) \
	((uint32_t)(-((nTableSize) + (nTableSize))))
#define HT_HASH_SIZE(nTableMask) \
	(((size_t)(uint32_t)-(int32_t)(nTableMask)) * sizeof(uint32_t))
#define HT_DATA_SIZE(nTableSize) \
	((size_t)(nTableSize) * sizeof(Bucket))
#define HT_SIZE_EX(nTableSize, nTableMask) \
	(HT_DATA_SIZE((nTableSize)) + HT_HASH_SIZE((nTableMask)))
#define HT_SIZE(ht) \
	HT_SIZE_EX((ht)->nTableSize, (ht)->nTableMask)
#define HT_USED_SIZE(ht) \
	(HT_HASH_SIZE((ht)->nTableMask) + ((size_t)(ht)->nNumUsed * sizeof(Bucket)))
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
	int               handle; // TODO: may be removed ???
	int               type;
	void             *ptr;
};

struct _zend_reference {
	zend_refcounted_h gc;
	zval              val;
};

struct _zend_ast_ref {
	zend_refcounted_h gc;
	/*zend_ast        ast; zend_ast follows the zend_ast_ref structure */
};

/* regular data types */
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

/* constant expressions */
#define IS_CONSTANT_AST				11

/* internal types */
#define IS_INDIRECT             	13
#define IS_PTR						14
#define _IS_ERROR					15

/* fake types used only for type hinting (Z_TYPE(zv) can not use them) */
#define _IS_BOOL					16
#define IS_CALLABLE					17
#define IS_ITERABLE					18
#define IS_VOID						19
#define _IS_NUMBER					20

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

#define Z_ACCESS_FLAGS(zval)		(zval).u2.access_flags
#define Z_ACCESS_FLAGS_P(zval_p)	Z_ACCESS_FLAGS(*(zval_p))

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
#define GC_COLLECTABLE				(1<<4)
#define GC_PROTECTED                (1<<5) /* used for recursion detection */
#define GC_IMMUTABLE                (1<<6) /* can't be canged in place */
#define GC_PERSISTENT               (1<<7) /* allocated using malloc */
#define GC_PERSISTENT_LOCAL         (1<<8) /* persistent, but thread-local */

#define GC_ARRAY					(IS_ARRAY          | (GC_COLLECTABLE << GC_FLAGS_SHIFT))
#define GC_OBJECT					(IS_OBJECT         | (GC_COLLECTABLE << GC_FLAGS_SHIFT))

/* zval.u1.v.type_flags */
#define IS_TYPE_REFCOUNTED			(1<<0)

#if 1
/* This optimized version assumes that we have a single "type_flag" */
# define Z_TYPE_INFO_REFCOUNTED(t)	(((t) & Z_TYPE_FLAGS_MASK) != 0)
#else
# define Z_TYPE_INFO_REFCOUNTED(t)	(((t) & (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT)) != 0)
#endif

/* extended types */
#define IS_INTERNED_STRING_EX		IS_STRING

#define IS_STRING_EX				(IS_STRING         | (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT))
#define IS_ARRAY_EX					(IS_ARRAY          | (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT))
#define IS_OBJECT_EX				(IS_OBJECT         | (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT))
#define IS_RESOURCE_EX				(IS_RESOURCE       | (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT))
#define IS_REFERENCE_EX				(IS_REFERENCE      | (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT))

#define IS_CONSTANT_AST_EX			(IS_CONSTANT_AST   | (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT))

/* string flags (zval.value->gc.u.flags) */
#define IS_STR_INTERNED				GC_IMMUTABLE  /* interned string */
#define IS_STR_PERSISTENT			GC_PERSISTENT /* allocated using malloc */
#define IS_STR_PERMANENT        	(1<<8)        /* relives request boundary */

/* array flags */
#define IS_ARRAY_IMMUTABLE			GC_IMMUTABLE
#define IS_ARRAY_PERSISTENT			GC_PERSISTENT

/* object flags (zval.value->gc.u.flags) */
#define IS_OBJ_DESTRUCTOR_CALLED	(1<<8)
#define IS_OBJ_FREE_CALLED			(1<<9)

#define OBJ_FLAGS(obj)              GC_FLAGS(obj)

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
#define Z_REFCOUNTED(zval)			(Z_TYPE_FLAGS(zval) != 0)
#else
#define Z_REFCOUNTED(zval)			((Z_TYPE_FLAGS(zval) & IS_TYPE_REFCOUNTED) != 0)
#endif
#define Z_REFCOUNTED_P(zval_p)		Z_REFCOUNTED(*(zval_p))

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

#define Z_OBJPROP(zval)				Z_OBJ_HT((zval))->get_properties(&(zval))
#define Z_OBJPROP_P(zval_p)			Z_OBJPROP(*(zval_p))

#define Z_OBJDEBUG(zval,tmp)		(Z_OBJ_HANDLER((zval),get_debug_info)?Z_OBJ_HANDLER((zval),get_debug_info)(&(zval),&tmp):(tmp=0,Z_OBJ_HANDLER((zval),get_properties)?Z_OBJPROP(zval):NULL))
#define Z_OBJDEBUG_P(zval_p,tmp)	Z_OBJDEBUG(*(zval_p), tmp)

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

#define ZVAL_LONG(z, l) {				\
		zval *__z = (z);				\
		Z_LVAL_P(__z) = l;				\
		Z_TYPE_INFO_P(__z) = IS_LONG;	\
	}

#define ZVAL_DOUBLE(z, d) {				\
		zval *__z = (z);				\
		Z_DVAL_P(__z) = d;				\
		Z_TYPE_INFO_P(__z) = IS_DOUBLE;	\
	}

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

#define ZVAL_NEW_ARR(z) do {									\
		zval *__z = (z);										\
		zend_array *_arr =										\
		(zend_array *) emalloc(sizeof(zend_array));				\
		Z_ARR_P(__z) = _arr;									\
		Z_TYPE_INFO_P(__z) = IS_ARRAY_EX;						\
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
		GC_TYPE_INFO(_res) = IS_RESOURCE;						\
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
		GC_TYPE_INFO(_res) = IS_RESOURCE |						\
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
		GC_TYPE_INFO(_ref) = IS_REFERENCE;						\
		Z_REF_P(z) = _ref;										\
		Z_TYPE_INFO_P(z) = IS_REFERENCE_EX;						\
	} while (0)

#define ZVAL_NEW_REF(z, r) do {									\
		zend_reference *_ref =									\
		(zend_reference *) emalloc(sizeof(zend_reference));		\
		GC_SET_REFCOUNT(_ref, 1);								\
		GC_TYPE_INFO(_ref) = IS_REFERENCE;						\
		ZVAL_COPY_VALUE(&_ref->val, r);							\
		Z_REF_P(z) = _ref;										\
		Z_TYPE_INFO_P(z) = IS_REFERENCE_EX;						\
	} while (0)

#define ZVAL_MAKE_REF_EX(z, refcount) do {						\
		zval *_z = (z);											\
		zend_reference *_ref =									\
			(zend_reference *) emalloc(sizeof(zend_reference));	\
		GC_SET_REFCOUNT(_ref, (refcount));						\
		GC_TYPE_INFO(_ref) = IS_REFERENCE;						\
		ZVAL_COPY_VALUE(&_ref->val, _z);						\
		Z_REF_P(_z) = _ref;										\
		Z_TYPE_INFO_P(_z) = IS_REFERENCE_EX;					\
	} while (0)

#define ZVAL_NEW_PERSISTENT_REF(z, r) do {						\
		zend_reference *_ref =									\
		(zend_reference *) malloc(sizeof(zend_reference));		\
		GC_SET_REFCOUNT(_ref, 1);								\
		GC_TYPE_INFO(_ref) = IS_REFERENCE |						\
			(GC_PERSISTENT << GC_FLAGS_SHIFT);					\
		ZVAL_COPY_VALUE(&_ref->val, r);							\
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
extern ZEND_API zend_bool zend_rc_debug;
# define ZEND_RC_MOD_CHECK(p) do { \
		if (zend_rc_debug) { \
			ZEND_ASSERT(!(zval_gc_flags((p)->u.type_info) & GC_IMMUTABLE)); \
			ZEND_ASSERT((zval_gc_flags((p)->u.type_info) & (GC_PERSISTENT|GC_PERSISTENT_LOCAL)) != GC_PERSISTENT); \
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

static zend_always_inline uint32_t zend_gc_delref(zend_refcounted_h *p) {
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
			if (EXPECTED(!(GC_FLAGS(_gc) & GC_PERSISTENT))) {			\
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
			Z_DELREF_P(_zv);							\
			ZVAL_NEW_STR(_zv, zend_string_init(			\
				ZSTR_VAL(_str),	ZSTR_LEN(_str), 0));	\
		}												\
	} while (0)

#define SEPARATE_ARRAY(zv) do {							\
		zval *_zv = (zv);								\
		zend_array *_arr = Z_ARR_P(_zv);				\
		if (UNEXPECTED(GC_REFCOUNT(_arr) > 1)) {		\
			if (Z_REFCOUNTED_P(_zv)) {					\
				GC_DELREF(_arr);						\
			}											\
			ZVAL_ARR(_zv, zend_array_dup(_arr));		\
		}												\
	} while (0)

#define SEPARATE_ZVAL_IF_NOT_REF(zv) do {				\
		zval *__zv = (zv);								\
		if (Z_TYPE_P(__zv) == IS_ARRAY) {				\
			if (Z_REFCOUNT_P(__zv) > 1) {				\
				if (Z_REFCOUNTED_P(__zv)) {				\
					Z_DELREF_P(__zv);					\
				}										\
				ZVAL_ARR(__zv, zend_array_dup(Z_ARR_P(__zv)));\
			}											\
		}												\
	} while (0)

#define SEPARATE_ZVAL_NOREF(zv) do {					\
		zval *_zv = (zv);								\
		ZEND_ASSERT(Z_TYPE_P(_zv) != IS_REFERENCE);		\
		SEPARATE_ZVAL_IF_NOT_REF(_zv);					\
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
		SEPARATE_ZVAL_IF_NOT_REF(_zv);					\
	} while (0)

#define SEPARATE_ARG_IF_REF(varptr) do { 				\
		ZVAL_DEREF(varptr);								\
		if (Z_REFCOUNTED_P(varptr)) { 					\
			Z_ADDREF_P(varptr); 						\
		}												\
	} while (0)

#endif /* ZEND_TYPES_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
