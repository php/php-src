/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
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

#ifndef ZEND_TYPES_H
#define ZEND_TYPES_H

#include "zend_int.h"

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
typedef uint32_t      zend_uint;
typedef unsigned short zend_ushort;

#ifdef ZEND_ENABLE_INT64
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

#define HAVE_ZEND_LONG64
#ifdef ZEND_WIN32
typedef __int64 zend_long64;
typedef unsigned __int64 zend_ulong64;
#elif SIZEOF_LONG_LONG_INT == 8
typedef long long int zend_long64;
typedef unsigned long long int zend_ulong64;
#elif SIZEOF_LONG_LONG == 8
typedef long long zend_long64;
typedef unsigned long long zend_ulong64;
#else
# undef HAVE_ZEND_LONG64
#endif

#ifdef _WIN64
typedef __int64 zend_intptr_t;
typedef unsigned __int64 zend_uintptr_t;
#else
typedef long zend_intptr_t;
typedef unsigned long zend_uintptr_t;
#endif

typedef struct _zend_object_handlers zend_object_handlers;
typedef struct _zend_class_entry     zend_class_entry;
typedef union  _zend_function        zend_function;

typedef struct _zval_struct     zval;

typedef struct _zend_refcounted zend_refcounted;
typedef struct _zend_string     zend_string;
typedef struct _zend_array      zend_array;
typedef struct _zend_object     zend_object;
typedef struct _zend_resource   zend_resource;
typedef struct _zend_reference  zend_reference;
typedef struct _zend_ast_ref    zend_ast_ref;
typedef struct _zend_ast        zend_ast;

typedef int  (*compare_func_t)(const void *, const void * TSRMLS_DC);
typedef void (*sort_func_t)(void *, size_t, size_t, compare_func_t TSRMLS_DC);
typedef void (*dtor_func_t)(zval *pDest);
typedef void (*copy_ctor_func_t)(zval *pElement);

typedef union _zend_value {
	zend_long        lval;				/* long value */
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
} zend_value;

struct _zval_struct {
	zend_value        value;			/* value */
	union {
		struct {
			ZEND_ENDIAN_LOHI_4(
				zend_uchar    type,			/* active type */
				zend_uchar    type_flags,
				zend_uchar    const_flags,
				zend_uchar    reserved)	    /* various IS_VAR flags */
		} v;
		zend_uint type_info;
	} u1;
	union {
		zend_uint     var_flags;
		zend_uint     next;                 /* hash collision chain */
		zend_uint     str_offset;           /* string offset */
		zend_uint     cache_slot;           /* literal cache slot */
	} u2;
};

struct _zend_refcounted {
	zend_uint         refcount;			/* reference counter 32-bit */
	union {
		struct {
			ZEND_ENDIAN_LOHI_3(
				zend_uchar    type,
				zend_uchar    flags,    /* used for strings & objects */
				zend_ushort   gc_info)  /* keeps GC root number (or 0) and color */
		} v;
		zend_uint type_info;
	} u;
};

struct _zend_string {
	zend_refcounted   gc;
	zend_ulong       h;                /* hash value */
	size_t       len;
	char              val[1];
};

typedef struct _Bucket {
	zend_ulong       h;                /* hash value (or numeric index)   */
	zend_string      *key;              /* string key or NULL for numerics */
	zval              val;
} Bucket;

typedef struct _HashTable {	
	zend_uint         nTableSize;
	zend_uint         nTableMask;
	zend_uint         nNumUsed;
	zend_uint         nNumOfElements;
	zend_long        nNextFreeElement;
	Bucket           *arData;
	zend_uint        *arHash;
	dtor_func_t       pDestructor;
	zend_uint         nInternalPointer; 
	union {
		struct {
			ZEND_ENDIAN_LOHI_3(
				zend_uchar    flags,
				zend_uchar    nApplyCount,
				zend_ushort   reserve)
		} v;
		zend_uint flags;
	} u;
} HashTable;

struct _zend_array {
	zend_refcounted   gc;
	HashTable         ht;
};

struct _zend_object {
	zend_refcounted   gc;
	zend_uint         handle; // TODO: may be removed ???
	zend_class_entry *ce;
	const zend_object_handlers *handlers;
	HashTable        *properties;
	HashTable        *guards; /* protects from __get/__set ... recursion */
	zval              properties_table[1];
};

struct _zend_resource {
	zend_refcounted   gc;
	zend_long              handle; // TODO: may be removed ???
	int               type;
	void             *ptr;
};

struct _zend_reference {
	zend_refcounted   gc;
	zval              val;
};

struct _zend_ast_ref {
	zend_refcounted   gc;
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
#define IS_STR_OFFSET				16
#define IS_PTR						17

static inline zend_uchar zval_get_type(const zval* pz) {
	return pz->u1.v.type;
}

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

#define Z_COUNTED(zval)				(zval).value.counted
#define Z_COUNTED_P(zval_p)			Z_COUNTED(*(zval_p))

#define Z_TYPE_FLAGS_SHIFT			8
#define Z_CONST_FLAGS_SHIFT			8

#define GC_REFCOUNT(p)				((zend_refcounted*)(p))->refcount
#define GC_TYPE(p)					((zend_refcounted*)(p))->u.v.type
#define GC_FLAGS(p)					((zend_refcounted*)(p))->u.v.flags
#define GC_INFO(p)					((zend_refcounted*)(p))->u.v.gc_info
#define GC_TYPE_INFO(p)				((zend_refcounted*)(p))->u.type_info

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
#define IS_TYPE_REFCOUNTED			(1<<1)
#define IS_TYPE_COLLECTABLE			(1<<2)
#define IS_TYPE_COPYABLE			(1<<3)
#define IS_TYPE_IMMUTABLE			(1<<4)

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
#define IS_CONSTANT_IN_NAMESPACE	0x100  /* used only in opline->extended_value */

/* zval.u2.var_flags */
#define IS_VAR_RET_REF				(1<<0) /* return by by reference */

/* string flags (zval.value->gc.u.flags) */
#define IS_STR_PERSISTENT			(1<<0) /* allocated using malloc   */
#define IS_STR_INTERNED				(1<<1) /* interned string          */
#define IS_STR_PERMANENT        	(1<<2) /* relives request boundary */

#define IS_STR_CONSTANT             (1<<3) /* constant index */
#define IS_STR_CONSTANT_UNQUALIFIED (1<<4) /* the same as IS_CONSTANT_UNQUALIFIED */

/* object flags (zval.value->gc.u.flags) */
#define IS_OBJ_APPLY_COUNT			0x07
#define IS_OBJ_DESTRUCTOR_CALLED	(1<<3)
#define IS_OBJ_FREE_CALLED			(1<<4)

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

/* the following Z_OPT_* macros make better code when Z_TYPE_INFO accessed before */
#define Z_OPT_TYPE(zval)			(Z_TYPE_INFO(zval) & 0xff)
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

#define Z_STRVAL(zval)				Z_STR(zval)->val
#define Z_STRVAL_P(zval_p)			Z_STRVAL(*(zval_p))

#define Z_STRLEN(zval)				Z_STR(zval)->len
#define Z_STRLEN_P(zval_p)			Z_STRLEN(*(zval_p))

#define Z_STRHASH(zval)				Z_STR(zval)->h
#define Z_STRHASH_P(zval_p)			Z_STRHASH(*(zval_p))

#define Z_ARR(zval)					(zval).value.arr
#define Z_ARR_P(zval_p)				Z_ARR(*(zval_p))

#define Z_ARRVAL(zval)				(&Z_ARR(zval)->ht)
#define Z_ARRVAL_P(zval_p)			Z_ARRVAL(*(zval_p))

#define Z_OBJ(zval)					(zval).value.obj
#define Z_OBJ_P(zval_p)				Z_OBJ(*(zval_p))

#define Z_OBJ_HT(zval)				Z_OBJ(zval)->handlers
#define Z_OBJ_HT_P(zval_p)			Z_OBJ_HT(*(zval_p))

#define Z_OBJ_HANDLER(zval, hf)		Z_OBJ_HT((zval))->hf
#define Z_OBJ_HANDLER_P(zv_p, hf)	Z_OBJ_HANDLER(*(zv_p), hf)

#define Z_OBJ_HANDLE(zval)          (Z_OBJ((zval)))->handle
#define Z_OBJ_HANDLE_P(zval_p)      Z_OBJ_HANDLE(*(zval_p))

#define Z_OBJCE(zval)				zend_get_class_entry(Z_OBJ(zval) TSRMLS_CC)
#define Z_OBJCE_P(zval_p)			Z_OBJCE(*(zval_p))

#define Z_OBJPROP(zval)				Z_OBJ_HT((zval))->get_properties(&(zval) TSRMLS_CC)
#define Z_OBJPROP_P(zval_p)			Z_OBJPROP(*(zval_p))

#define Z_OBJDEBUG(zval,tmp)		(Z_OBJ_HANDLER((zval),get_debug_info)?Z_OBJ_HANDLER((zval),get_debug_info)(&(zval),&tmp TSRMLS_CC):(tmp=0,Z_OBJ_HANDLER((zval),get_properties)?Z_OBJPROP(zval):NULL))
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
		Z_TYPE_INFO_P(__z) = IS_INTERNED(__s) ? \
			IS_INTERNED_STRING_EX : 			\
			IS_STRING_EX;						\
	} while (0)

#define ZVAL_LONG_STR(z, s) do {					\
		zval *__z = (z);						\
		zend_string *__s = (s);					\
		Z_STR_P(__z) = __s;						\
		Z_TYPE_INFO_P(__z) = IS_INTERNED_STRING_EX;	\
	} while (0)

#define ZVAL_NEW_STR(z, s) do {					\
		zval *__z = (z);						\
		zend_string *__s = (s);					\
		Z_STR_P(__z) = __s;						\
		/* interned strings support */			\
		Z_TYPE_INFO_P(__z) = IS_STRING_EX;		\
	} while (0)

#define ZVAL_ARR(z, a) do {						\
		zval *__z = (z);						\
		Z_ARR_P(__z) = (a);						\
		Z_TYPE_INFO_P(__z) = IS_ARRAY_EX;		\
	} while (0)

#define ZVAL_NEW_ARR(z) do {									\
		zval *__z = (z);										\
		zend_array *_arr = emalloc(sizeof(zend_array));			\
		GC_REFCOUNT(_arr) = 1;									\
		GC_TYPE_INFO(_arr) = IS_ARRAY;							\
		Z_ARR_P(__z) = _arr;									\
		Z_TYPE_INFO_P(__z) = IS_ARRAY_EX;						\
	} while (0)

#define ZVAL_NEW_PERSISTENT_ARR(z) do {							\
		zval *__z = (z);										\
		zend_array *_arr = malloc(sizeof(zend_array));			\
		GC_REFCOUNT(_arr) = 1;									\
		GC_TYPE_INFO(_arr) = IS_ARRAY;							\
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
		zend_resource *_res = emalloc(sizeof(zend_resource));	\
		zval *__z;										\
		GC_REFCOUNT(_res) = 1;									\
		GC_TYPE_INFO(_res) = IS_RESOURCE;						\
		_res->handle = (h);										\
		_res->type = (t);										\
		_res->ptr = (p);										\
		__z = (z);										\
		Z_RES_P(__z) = _res;									\
		Z_TYPE_INFO_P(__z) = IS_RESOURCE_EX;					\
	} while (0)

#define ZVAL_NEW_PERSISTENT_RES(z, h, p, t) do {				\
		zend_resource *_res = malloc(sizeof(zend_resource));	\
		zval *__z;										\
		GC_REFCOUNT(_res) = 1;									\
		GC_TYPE_INFO(_res) = IS_RESOURCE;						\
		_res->handle = (h);										\
		_res->type = (t);										\
		_res->ptr = (p);										\
		__z = (z);										\
		Z_RES_P(__z) = _res;									\
		Z_TYPE_INFO_P(__z) = IS_RESOURCE_EX;					\
	} while (0)

#define ZVAL_REF(z, r) do {										\
		zval *__z = (z);										\
		Z_REF_P(__z) = (r);										\
		Z_TYPE_INFO_P(__z) = IS_REFERENCE_EX;					\
	} while (0)

#define ZVAL_NEW_REF(z, r) do {									\
		zend_reference *_ref = emalloc(sizeof(zend_reference));	\
		GC_REFCOUNT(_ref) = 1;									\
		GC_TYPE_INFO(_ref) = IS_REFERENCE;						\
		ZVAL_COPY_VALUE(&_ref->val, r);							\
		Z_REF_P(z) = _ref;										\
		Z_TYPE_INFO_P(z) = IS_REFERENCE_EX;						\
	} while (0)

#define ZVAL_NEW_PERSISTENT_REF(z, r) do {						\
		zend_reference *_ref = malloc(sizeof(zend_reference));	\
		GC_REFCOUNT(_ref) = 1;									\
		GC_TYPE_INFO(_ref) = IS_REFERENCE;						\
		ZVAL_COPY_VALUE(&_ref->val, r);							\
		Z_REF_P(z) = _ref;										\
		Z_TYPE_INFO_P(z) = IS_REFERENCE_EX;						\
	} while (0)

#define ZVAL_NEW_AST(z, a) do {									\
		zval *__z = (z);										\
		zend_ast_ref *_ast = emalloc(sizeof(zend_ast_ref));		\
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


#define Z_STR_OFFSET_STR(zval)		Z_INDIRECT(zval)
#define Z_STR_OFFSET_STR_P(zval_p)	Z_STR_OFFSET_STR(*(zval_p))

#define Z_STR_OFFSET_IDX(zval)		(zval).u2.str_offset
#define Z_STR_OFFSET_IDX_P(zval_p)	Z_STR_OFFSET_IDX(*(zval_p))

#define ZVAL_STR_OFFSET(z, s, i) do {							\
		Z_STR_OFFSET_STR_P(z) = (s);							\
		Z_STR_OFFSET_IDX_P(z) = (i);							\
		Z_TYPE_INFO_P(z) = IS_STR_OFFSET;						\
	} while (0)

#endif /* ZEND_TYPES_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
