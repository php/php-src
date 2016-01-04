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
   |          Dmitry Stogov <dmitry@zend.com>                             |
   |          Xinchen Hui <xinchen.h@zend.com>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_TYPES_H
#define ZEND_TYPES_H

#include "zend_portability.h"
#include "zend_long.h"

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
			ZEND_ENDIAN_LOHI_4(
				zend_uchar    type,			/* active type */
				zend_uchar    type_flags,
				zend_uchar    const_flags,
				zend_uchar    reserved)	    /* call info for EX(This) */
		} v;
		uint32_t type_info;
	} u1;
	union {
		uint32_t     var_flags;
		uint32_t     next;                 /* hash collision chain */
		uint32_t     cache_slot;           /* literal cache slot */
		uint32_t     lineno;               /* line number (for ast nodes) */
		uint32_t     num_args;             /* arguments number for EX(This) */
		uint32_t     fe_pos;               /* foreach position */
		uint32_t     fe_iter_idx;          /* foreach iterator index */
	} u2;
};

typedef struct _zend_refcounted_h {
	uint32_t         refcount;			/* reference counter 32-bit */
	union {
		struct {
			ZEND_ENDIAN_LOHI_3(
				zend_uchar    type,
				zend_uchar    flags,    /* used for strings & objects */
				uint16_t      gc_info)  /* keeps GC root number (or 0) and color */
		} v;
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
				zend_uchar    nApplyCount,
				zend_uchar    nIteratorsCount,
				zend_uchar    reserve)
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
#define HT_HASH_RESET(ht) \
	memset(&HT_HASH(ht, (ht)->nTableMask), HT_INVALID_IDX, HT_HASH_SIZE((ht)->nTableMask))
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
	uint32_t          handle;
	zend_class_entry *ce;
	const zend_object_handlers *handlers;
	HashTable        *properties;
	zval              properties_table[1];
};

struct _zend_resource {
	zend_refcounted_h gc;
	int               handle;
	int               type;
	void             *ptr;
};

struct _zend_reference {
	zend_refcounted_h gc;
	zval              val;
};

struct _zend_ast_ref {
	zend_refcounted_h gc;
	zend_ast         *ast;
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
#define IS_CONSTANT					11
#define IS_CONSTANT_AST				12

/* fake types */
#define _IS_BOOL					13
#define IS_CALLABLE					14

/* internal types */
#define IS_INDIRECT             	15
#define IS_PTR						17

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

#define Z_CONST_FLAGS(zval)			(zval).u1.v.const_flags
#define Z_CONST_FLAGS_P(zval_p)		Z_CONST_FLAGS(*(zval_p))

#define Z_VAR_FLAGS(zval)			(zval).u2.var_flags
#define Z_VAR_FLAGS_P(zval_p)		Z_VAR_FLAGS(*(zval_p))

#define Z_TYPE_INFO(zval)			(zval).u1.type_info
#define Z_TYPE_INFO_P(zval_p)		Z_TYPE_INFO(*(zval_p))

#define Z_NEXT(zval)				(zval).u2.next
#define Z_NEXT_P(zval_p)			Z_NEXT(*(zval_p))

#define Z_CACHE_SLOT(zval)			(zval).u2.cache_slot
#define Z_CACHE_SLOT_P(zval_p)		Z_CACHE_SLOT(*(zval_p))

#define Z_FE_POS(zval)				(zval).u2.fe_pos
#define Z_FE_POS_P(zval_p)			Z_FE_POS(*(zval_p))

#define Z_FE_ITER(zval)				(zval).u2.fe_iter_idx
#define Z_FE_ITER_P(zval_p)			Z_FE_ITER(*(zval_p))

#define Z_COUNTED(zval)				(zval).value.counted
#define Z_COUNTED_P(zval_p)			Z_COUNTED(*(zval_p))

#define Z_TYPE_MASK					0xff

#define Z_TYPE_FLAGS_SHIFT			8
#define Z_CONST_FLAGS_SHIFT			16

#define GC_REFCOUNT(p)				(p)->gc.refcount
#define GC_TYPE(p)					(p)->gc.u.v.type
#define GC_FLAGS(p)					(p)->gc.u.v.flags
#define GC_INFO(p)					(p)->gc.u.v.gc_info
#define GC_TYPE_INFO(p)				(p)->gc.u.type_info

#define Z_GC_TYPE(zval)				GC_TYPE(Z_COUNTED(zval))
#define Z_GC_TYPE_P(zval_p)			Z_GC_TYPE(*(zval_p))

#define Z_GC_FLAGS(zval)			GC_FLAGS(Z_COUNTED(zval))
#define Z_GC_FLAGS_P(zval_p)		Z_GC_FLAGS(*(zval_p))

#define Z_GC_INFO(zval)				GC_INFO(Z_COUNTED(zval))
#define Z_GC_INFO_P(zval_p)			Z_GC_INFO(*(zval_p))

#define Z_GC_TYPE_INFO(zval)		GC_TYPE_INFO(Z_COUNTED(zval))
#define Z_GC_TYPE_INFO_P(zval_p)	Z_GC_TYPE_INFO(*(zval_p))

/* zval.u1.v.type_flags */
#define IS_TYPE_CONSTANT			(1<<0)
#define IS_TYPE_IMMUTABLE			(1<<1)
#define IS_TYPE_REFCOUNTED			(1<<2)
#define IS_TYPE_COLLECTABLE			(1<<3)
#define IS_TYPE_COPYABLE			(1<<4)
#define IS_TYPE_SYMBOLTABLE			(1<<5)

/* extended types */
#define IS_INTERNED_STRING_EX		IS_STRING

#define IS_STRING_EX				(IS_STRING         | ((                   IS_TYPE_REFCOUNTED |                       IS_TYPE_COPYABLE) << Z_TYPE_FLAGS_SHIFT))
#define IS_ARRAY_EX					(IS_ARRAY          | ((                   IS_TYPE_REFCOUNTED | IS_TYPE_COLLECTABLE | IS_TYPE_COPYABLE) << Z_TYPE_FLAGS_SHIFT))
#define IS_OBJECT_EX				(IS_OBJECT         | ((                   IS_TYPE_REFCOUNTED | IS_TYPE_COLLECTABLE                   ) << Z_TYPE_FLAGS_SHIFT))
#define IS_RESOURCE_EX				(IS_RESOURCE       | ((                   IS_TYPE_REFCOUNTED                                         ) << Z_TYPE_FLAGS_SHIFT))
#define IS_REFERENCE_EX				(IS_REFERENCE      | ((                   IS_TYPE_REFCOUNTED                                         ) << Z_TYPE_FLAGS_SHIFT))

#define IS_CONSTANT_EX				(IS_CONSTANT       | ((IS_TYPE_CONSTANT | IS_TYPE_REFCOUNTED |                       IS_TYPE_COPYABLE) << Z_TYPE_FLAGS_SHIFT))
#define IS_CONSTANT_AST_EX			(IS_CONSTANT_AST   | ((IS_TYPE_CONSTANT | IS_TYPE_REFCOUNTED |                       IS_TYPE_COPYABLE) << Z_TYPE_FLAGS_SHIFT))

/* zval.u1.v.const_flags */
#define IS_CONSTANT_UNQUALIFIED		0x010
#define IS_LEXICAL_VAR				0x020
#define IS_LEXICAL_REF				0x040
#define IS_CONSTANT_CLASS           0x080  /* __CLASS__ in trait */
#define IS_CONSTANT_IN_NAMESPACE	0x100  /* used only in opline->extended_value */

/* zval.u2.var_flags */
#define IS_VAR_RET_REF				(1<<0) /* return by by reference */

/* string flags (zval.value->gc.u.flags) */
#define IS_STR_PERSISTENT			(1<<0) /* allocated using malloc   */
#define IS_STR_INTERNED				(1<<1) /* interned string          */
#define IS_STR_PERMANENT        	(1<<2) /* relives request boundary */

#define IS_STR_CONSTANT             (1<<3) /* constant index */
#define IS_STR_CONSTANT_UNQUALIFIED (1<<4) /* the same as IS_CONSTANT_UNQUALIFIED */

/* array flags */
#define IS_ARRAY_IMMUTABLE			(1<<1) /* the same as IS_TYPE_IMMUTABLE */

/* object flags (zval.value->gc.u.flags) */
#define IS_OBJ_APPLY_COUNT			0x07
#define IS_OBJ_DESTRUCTOR_CALLED	(1<<3)
#define IS_OBJ_FREE_CALLED			(1<<4)
#define IS_OBJ_USE_GUARDS           (1<<5)
#define IS_OBJ_HAS_GUARDS           (1<<6)

#define Z_OBJ_APPLY_COUNT(zval) \
	(Z_GC_FLAGS(zval) & IS_OBJ_APPLY_COUNT)

#define Z_OBJ_INC_APPLY_COUNT(zval) do { \
		Z_GC_FLAGS(zval) = \
			(Z_GC_FLAGS(zval) & ~IS_OBJ_APPLY_COUNT) | \
			((Z_GC_FLAGS(zval) & IS_OBJ_APPLY_COUNT) + 1); \
	} while (0)

#define Z_OBJ_DEC_APPLY_COUNT(zval) do { \
		Z_GC_FLAGS(zval) = \
			(Z_GC_FLAGS(zval) & ~IS_OBJ_APPLY_COUNT) | \
			((Z_GC_FLAGS(zval) & IS_OBJ_APPLY_COUNT) - 1); \
	} while (0)

#define Z_OBJ_APPLY_COUNT_P(zv)     Z_OBJ_APPLY_COUNT(*(zv))
#define Z_OBJ_INC_APPLY_COUNT_P(zv) Z_OBJ_INC_APPLY_COUNT(*(zv))
#define Z_OBJ_DEC_APPLY_COUNT_P(zv) Z_OBJ_DEC_APPLY_COUNT(*(zv))

/* All data types < IS_STRING have their constructor/destructors skipped */
#define Z_CONSTANT(zval)			((Z_TYPE_FLAGS(zval) & IS_TYPE_CONSTANT) != 0)
#define Z_CONSTANT_P(zval_p)		Z_CONSTANT(*(zval_p))

#define Z_REFCOUNTED(zval)			((Z_TYPE_FLAGS(zval) & IS_TYPE_REFCOUNTED) != 0)
#define Z_REFCOUNTED_P(zval_p)		Z_REFCOUNTED(*(zval_p))

#define Z_COLLECTABLE(zval)			((Z_TYPE_FLAGS(zval) & IS_TYPE_COLLECTABLE) != 0)
#define Z_COLLECTABLE_P(zval_p)		Z_COLLECTABLE(*(zval_p))

#define Z_COPYABLE(zval)			((Z_TYPE_FLAGS(zval) & IS_TYPE_COPYABLE) != 0)
#define Z_COPYABLE_P(zval_p)		Z_COPYABLE(*(zval_p))

#define Z_IMMUTABLE(zval)			((Z_TYPE_FLAGS(zval) & IS_TYPE_IMMUTABLE) != 0)
#define Z_IMMUTABLE_P(zval_p)		Z_IMMUTABLE(*(zval_p))

#define Z_SYMBOLTABLE(zval)			((Z_TYPE_FLAGS(zval) & IS_TYPE_SYMBOLTABLE) != 0)
#define Z_SYMBOLTABLE_P(zval_p)		Z_SYMBOLTABLE(*(zval_p))

/* the following Z_OPT_* macros make better code when Z_TYPE_INFO accessed before */
#define Z_OPT_TYPE(zval)			(Z_TYPE_INFO(zval) & Z_TYPE_MASK)
#define Z_OPT_TYPE_P(zval_p)		Z_OPT_TYPE(*(zval_p))

#define Z_OPT_CONSTANT(zval)		((Z_TYPE_INFO(zval) & (IS_TYPE_CONSTANT << Z_TYPE_FLAGS_SHIFT)) != 0)
#define Z_OPT_CONSTANT_P(zval_p)	Z_OPT_CONSTANT(*(zval_p))

#define Z_OPT_REFCOUNTED(zval)		((Z_TYPE_INFO(zval) & (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT)) != 0)
#define Z_OPT_REFCOUNTED_P(zval_p)	Z_OPT_REFCOUNTED(*(zval_p))

#define Z_OPT_COLLECTABLE(zval)		((Z_TYPE_INFO(zval) & (IS_TYPE_COLLECTABLE << Z_TYPE_FLAGS_SHIFT)) != 0)
#define Z_OPT_COLLECTABLE_P(zval_p)	Z_OPT_COLLECTABLE(*(zval_p))

#define Z_OPT_COPYABLE(zval)		((Z_TYPE_INFO(zval) & (IS_TYPE_COPYABLE << Z_TYPE_FLAGS_SHIFT)) != 0)
#define Z_OPT_COPYABLE_P(zval_p)	Z_OPT_COPYABLE(*(zval_p))

#define Z_OPT_IMMUTABLE(zval)		((Z_TYPE_INFO(zval) & (IS_TYPE_IMMUTABLE << Z_TYPE_FLAGS_SHIFT)) != 0)
#define Z_OPT_IMMUTABLE_P(zval_p)	Z_OPT_IMMUTABLE(*(zval_p))

#define Z_OPT_ISREF(zval)			(Z_OPT_TYPE(zval) == IS_REFERENCE)
#define Z_OPT_ISREF_P(zval_p)		Z_OPT_ISREF(*(zval_p))

#define Z_ISREF(zval)				(Z_TYPE(zval) == IS_REFERENCE)
#define Z_ISREF_P(zval_p)			Z_ISREF(*(zval_p))

#define Z_ISUNDEF(zval)				(Z_TYPE(zval) == IS_UNDEF)
#define Z_ISUNDEF_P(zval_p)			Z_ISUNDEF(*(zval_p))

#define Z_ISNULL(zval)				(Z_TYPE(zval) == IS_NULL)
#define Z_ISNULL_P(zval_p)			Z_ISNULL(*(zval_p))

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

#define Z_ASTVAL(zval)				(zval).value.ast->ast
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
		if (ZSTR_IS_INTERNED(__s)) {							\
			Z_TYPE_INFO_P(__z) = IS_INTERNED_STRING_EX;	\
		} else {										\
			GC_REFCOUNT(__s)++;							\
			Z_TYPE_INFO_P(__z) = IS_STRING_EX;			\
		}												\
	} while (0)

#define ZVAL_ARR(z, a) do {						\
		zval *__z = (z);						\
		Z_ARR_P(__z) = (a);						\
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
		GC_REFCOUNT(_res) = 1;									\
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
		GC_REFCOUNT(_res) = 1;									\
		GC_TYPE_INFO(_res) = IS_RESOURCE;						\
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
		GC_REFCOUNT(_ref) = 1;									\
		GC_TYPE_INFO(_ref) = IS_REFERENCE;						\
		Z_REF_P(z) = _ref;										\
		Z_TYPE_INFO_P(z) = IS_REFERENCE_EX;						\
	} while (0)

#define ZVAL_NEW_REF(z, r) do {									\
		zend_reference *_ref =									\
		(zend_reference *) emalloc(sizeof(zend_reference));		\
		GC_REFCOUNT(_ref) = 1;									\
		GC_TYPE_INFO(_ref) = IS_REFERENCE;						\
		ZVAL_COPY_VALUE(&_ref->val, r);							\
		Z_REF_P(z) = _ref;										\
		Z_TYPE_INFO_P(z) = IS_REFERENCE_EX;						\
	} while (0)

#define ZVAL_NEW_PERSISTENT_REF(z, r) do {						\
		zend_reference *_ref =									\
		(zend_reference *) malloc(sizeof(zend_reference));		\
		GC_REFCOUNT(_ref) = 1;									\
		GC_TYPE_INFO(_ref) = IS_REFERENCE;						\
		ZVAL_COPY_VALUE(&_ref->val, r);							\
		Z_REF_P(z) = _ref;										\
		Z_TYPE_INFO_P(z) = IS_REFERENCE_EX;						\
	} while (0)

#define ZVAL_NEW_AST(z, a) do {									\
		zval *__z = (z);										\
		zend_ast_ref *_ast =									\
		(zend_ast_ref *) emalloc(sizeof(zend_ast_ref));			\
		GC_REFCOUNT(_ast) = 1;									\
		GC_TYPE_INFO(_ast) = IS_CONSTANT_AST;					\
		_ast->ast = (a);										\
		Z_AST_P(__z) = _ast;									\
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

static zend_always_inline uint32_t zval_refcount_p(zval* pz) {
	ZEND_ASSERT(Z_REFCOUNTED_P(pz) || Z_IMMUTABLE_P(pz) || Z_SYMBOLTABLE_P(pz));
	return GC_REFCOUNT(Z_COUNTED_P(pz));
}

static zend_always_inline uint32_t zval_set_refcount_p(zval* pz, uint32_t rc) {
	ZEND_ASSERT(Z_REFCOUNTED_P(pz));
	return GC_REFCOUNT(Z_COUNTED_P(pz)) = rc;
}

static zend_always_inline uint32_t zval_addref_p(zval* pz) {
	ZEND_ASSERT(Z_REFCOUNTED_P(pz));
	return ++GC_REFCOUNT(Z_COUNTED_P(pz));
}

static zend_always_inline uint32_t zval_delref_p(zval* pz) {
	ZEND_ASSERT(Z_REFCOUNTED_P(pz));
	return --GC_REFCOUNT(Z_COUNTED_P(pz));
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
		if ((_t & (IS_TYPE_REFCOUNTED << Z_TYPE_FLAGS_SHIFT)) != 0) { \
			GC_REFCOUNT(_gc)++;							\
		}												\
	} while (0)

#define ZVAL_DUP(z, v)									\
	do {												\
		zval *_z1 = (z);								\
		const zval *_z2 = (v);							\
		zend_refcounted *_gc = Z_COUNTED_P(_z2);		\
		uint32_t _t = Z_TYPE_INFO_P(_z2);				\
		ZVAL_COPY_VALUE_EX(_z1, _z2, _gc, _t);			\
		if ((_t & ((IS_TYPE_REFCOUNTED|IS_TYPE_IMMUTABLE) << Z_TYPE_FLAGS_SHIFT)) != 0) { \
			if ((_t & ((IS_TYPE_COPYABLE|IS_TYPE_IMMUTABLE) << Z_TYPE_FLAGS_SHIFT)) != 0) { \
				_zval_copy_ctor_func(_z1 ZEND_FILE_LINE_CC); \
			} else {									\
				GC_REFCOUNT(_gc)++;						\
			}											\
		}												\
	} while (0)

#define ZVAL_DEREF(z) do {								\
		if (UNEXPECTED(Z_ISREF_P(z))) {					\
			(z) = Z_REFVAL_P(z);						\
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

#define SEPARATE_STRING(zv) do {						\
		zval *_zv = (zv);								\
		if (Z_REFCOUNTED_P(_zv) &&						\
		    Z_REFCOUNT_P(_zv) > 1) {					\
			Z_DELREF_P(_zv);							\
			zval_copy_ctor_func(_zv);					\
		}												\
	} while (0)

#define SEPARATE_ARRAY(zv) do {							\
		zval *_zv = (zv);								\
		zend_array *_arr = Z_ARR_P(_zv);				\
		if (GC_REFCOUNT(_arr) > 1) {					\
			if (!Z_IMMUTABLE_P(_zv)) {					\
				GC_REFCOUNT(_arr)--;					\
			}											\
			ZVAL_ARR(_zv, zend_array_dup(_arr));		\
		}												\
	} while (0)

#define SEPARATE_ZVAL_NOREF(zv) do {					\
		zval *_zv = (zv);								\
		ZEND_ASSERT(Z_TYPE_P(_zv) != IS_REFERENCE);		\
		if (Z_COPYABLE_P(_zv) ||						\
		    Z_IMMUTABLE_P(_zv)) {						\
			if (Z_REFCOUNT_P(_zv) > 1) {				\
				if (!Z_IMMUTABLE_P(_zv)) {				\
					Z_DELREF_P(_zv);					\
				}										\
				zval_copy_ctor_func(_zv);				\
			}											\
		}												\
	} while (0)

#define SEPARATE_ZVAL(zv) do {							\
		zval *_zv = (zv);								\
		if (Z_REFCOUNTED_P(_zv) ||						\
		    Z_IMMUTABLE_P(_zv)) {						\
			if (Z_REFCOUNT_P(_zv) > 1) {				\
				if (Z_COPYABLE_P(_zv) ||				\
				    Z_IMMUTABLE_P(_zv)) {				\
					if (!Z_IMMUTABLE_P(_zv)) {			\
						Z_DELREF_P(_zv);				\
					}									\
					zval_copy_ctor_func(_zv);			\
				} else if (Z_ISREF_P(_zv)) {			\
					Z_DELREF_P(_zv);					\
					ZVAL_DUP(_zv, Z_REFVAL_P(_zv));		\
				}										\
			}											\
		}												\
	} while (0)

#define SEPARATE_ZVAL_IF_NOT_REF(zv) do {				\
		zval *_zv = (zv);								\
		if (Z_COPYABLE_P(_zv) ||                    	\
		    Z_IMMUTABLE_P(_zv)) {						\
			if (Z_REFCOUNT_P(_zv) > 1) {				\
				if (!Z_IMMUTABLE_P(_zv)) {				\
					Z_DELREF_P(_zv);					\
				}										\
				zval_copy_ctor_func(_zv);				\
			}											\
		}												\
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
 */
