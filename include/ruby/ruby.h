/**********************************************************************

  ruby/ruby.h -

  $Author$
  created at: Thu Jun 10 14:26:32 JST 1993

  Copyright (C) 1993-2008 Yukihiro Matsumoto
  Copyright (C) 2000  Network Applied Communication Laboratory, Inc.
  Copyright (C) 2000  Information-technology Promotion Agency, Japan

**********************************************************************/

#ifndef RUBY_RUBY_H
#define RUBY_RUBY_H 1

#if defined(__cplusplus)
extern "C" {
#if 0
} /* satisfy cc-mode */
#endif
#endif

#include "ruby/config.h"
#ifdef RUBY_EXTCONF_H
#include RUBY_EXTCONF_H
#endif

#define NORETURN_STYLE_NEW 1
#ifndef NORETURN
# define NORETURN(x) x
#endif
#ifndef DEPRECATED
# define DEPRECATED(x) x
#endif
#ifndef NOINLINE
# define NOINLINE(x) x
#endif
#ifndef UNREACHABLE
# define UNREACHABLE		/* unreachable */
#endif

#ifdef __GNUC__
#define PRINTF_ARGS(decl, string_index, first_to_check) \
  decl __attribute__((format(printf, string_index, first_to_check)))
#else
#define PRINTF_ARGS(decl, string_index, first_to_check) decl
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#ifdef HAVE_INTRINSICS_H
# include <intrinsics.h>
#endif

#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif

#include <stdarg.h>
#include <stdio.h>

#include "defines.h"

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

#if defined(HAVE_ALLOCA_H)
#include <alloca.h>
#else
#  ifdef _AIX
#pragma alloca
#  endif
#endif

#if defined HAVE_UINTPTR_T && 0
typedef uintptr_t VALUE;
typedef uintptr_t ID;
# define SIGNED_VALUE intptr_t
# define SIZEOF_VALUE SIZEOF_UINTPTR_T
# undef PRI_VALUE_PREFIX
#elif SIZEOF_LONG == SIZEOF_VOIDP
typedef unsigned long VALUE;
typedef unsigned long ID;
# define SIGNED_VALUE long
# define SIZEOF_VALUE SIZEOF_LONG
# define PRI_VALUE_PREFIX "l"
#elif SIZEOF_LONG_LONG == SIZEOF_VOIDP
typedef unsigned LONG_LONG VALUE;
typedef unsigned LONG_LONG ID;
# define SIGNED_VALUE LONG_LONG
# define LONG_LONG_VALUE 1
# define SIZEOF_VALUE SIZEOF_LONG_LONG
# define PRI_VALUE_PREFIX PRI_LL_PREFIX
#else
# error ---->> ruby requires sizeof(void*) == sizeof(long) or sizeof(LONG_LONG) to be compiled. <<----
#endif

typedef char ruby_check_sizeof_int[SIZEOF_INT == sizeof(int) ? 1 : -1];
typedef char ruby_check_sizeof_long[SIZEOF_LONG == sizeof(long) ? 1 : -1];
#ifdef SIZEOF_LONG_LONG
typedef char ruby_check_sizeof_long_long[SIZEOF_LONG_LONG == sizeof(LONG_LONG) ? 1 : -1];
#endif
typedef char ruby_check_sizeof_voidp[SIZEOF_VOIDP == sizeof(void*) ? 1 : -1];

#ifndef PRI_INT_PREFIX
#define PRI_INT_PREFIX ""
#endif
#ifndef PRI_LONG_PREFIX
#define PRI_LONG_PREFIX "l"
#endif

#if defined PRIdPTR && !defined PRI_VALUE_PREFIX
#define PRIdVALUE PRIdPTR
#define PRIiVALUE PRIiPTR
#define PRIoVALUE PRIoPTR
#define PRIuVALUE PRIuPTR
#define PRIxVALUE PRIxPTR
#define PRIXVALUE PRIXPTR
#else
#define PRIdVALUE PRI_VALUE_PREFIX"d"
#define PRIiVALUE PRI_VALUE_PREFIX"i"
#define PRIoVALUE PRI_VALUE_PREFIX"o"
#define PRIuVALUE PRI_VALUE_PREFIX"u"
#define PRIxVALUE PRI_VALUE_PREFIX"x"
#define PRIXVALUE PRI_VALUE_PREFIX"X"
#endif
#ifndef PRI_VALUE_PREFIX
# define PRI_VALUE_PREFIX ""
#endif

#ifndef PRI_TIMET_PREFIX
# if SIZEOF_TIME_T == SIZEOF_INT
#  define PRI_TIMET_PREFIX
# elif SIZEOF_TIME_T == SIZEOF_LONG
#  define PRI_TIMET_PREFIX "l"
# elif SIZEOF_TIME_T == SIZEOF_LONG_LONG
#  define PRI_TIMET_PREFIX PRI_LL_PREFIX
# endif
#endif

#if defined PRI_PTRDIFF_PREFIX
#elif SIZEOF_PTRDIFF_T == SIZEOF_INT
# define PRI_PTRDIFF_PREFIX ""
#elif SIZEOF_PTRDIFF_T == SIZEOF_LONG
# define PRI_PTRDIFF_PREFIX "l"
#elif SIZEOF_PTRDIFF_T == SIZEOF_LONG_LONG
# define PRI_PTRDIFF_PREFIX PRI_LL_PREFIX
#endif
#define PRIdPTRDIFF PRI_PTRDIFF_PREFIX"d"
#define PRIiPTRDIFF PRI_PTRDIFF_PREFIX"i"
#define PRIoPTRDIFF PRI_PTRDIFF_PREFIX"o"
#define PRIuPTRDIFF PRI_PTRDIFF_PREFIX"u"
#define PRIxPTRDIFF PRI_PTRDIFF_PREFIX"x"
#define PRIXPTRDIFF PRI_PTRDIFF_PREFIX"X"

#if defined PRI_SIZE_PREFIX
#elif SIZEOF_SIZE_T == SIZEOF_INT
# define PRI_SIZE_PREFIX ""
#elif SIZEOF_SIZE_T == SIZEOF_LONG
# define PRI_SIZE_PREFIX "l"
#elif SIZEOF_SIZE_T == SIZEOF_LONG_LONG
# define PRI_SIZE_PREFIX PRI_LL_PREFIX
#endif
#define PRIdSIZE PRI_SIZE_PREFIX"d"
#define PRIiSIZE PRI_SIZE_PREFIX"i"
#define PRIoSIZE PRI_SIZE_PREFIX"o"
#define PRIuSIZE PRI_SIZE_PREFIX"u"
#define PRIxSIZE PRI_SIZE_PREFIX"x"
#define PRIXSIZE PRI_SIZE_PREFIX"X"

#ifdef __STDC__
# include <limits.h>
#else
# ifndef LONG_MAX
#  ifdef HAVE_LIMITS_H
#   include <limits.h>
#  else
    /* assuming 32bit(2's compliment) long */
#   define LONG_MAX 2147483647
#  endif
# endif
# ifndef LONG_MIN
#  define LONG_MIN (-LONG_MAX-1)
# endif
# ifndef CHAR_BIT
#  define CHAR_BIT 8
# endif
#endif

#ifdef HAVE_LONG_LONG
# ifndef LLONG_MAX
#  ifdef LONG_LONG_MAX
#   define LLONG_MAX  LONG_LONG_MAX
#  else
#   ifdef _I64_MAX
#    define LLONG_MAX _I64_MAX
#   else
    /* assuming 64bit(2's complement) long long */
#    define LLONG_MAX 9223372036854775807LL
#   endif
#  endif
# endif
# ifndef LLONG_MIN
#  ifdef LONG_LONG_MIN
#   define LLONG_MIN  LONG_LONG_MIN
#  else
#   ifdef _I64_MIN
#    define LLONG_MIN _I64_MIN
#   else
#    define LLONG_MIN (-LLONG_MAX-1)
#   endif
#  endif
# endif
#endif

#define FIXNUM_MAX (LONG_MAX>>1)
#define FIXNUM_MIN RSHIFT((long)LONG_MIN,1)

#define INT2FIX(i) ((VALUE)(((SIGNED_VALUE)(i))<<1 | FIXNUM_FLAG))
#define LONG2FIX(i) INT2FIX(i)
#define rb_fix_new(v) INT2FIX(v)
VALUE rb_int2inum(SIGNED_VALUE);

#define rb_int_new(v) rb_int2inum(v)
VALUE rb_uint2inum(VALUE);

#define rb_uint_new(v) rb_uint2inum(v)

#ifdef HAVE_LONG_LONG
VALUE rb_ll2inum(LONG_LONG);
#define LL2NUM(v) rb_ll2inum(v)
VALUE rb_ull2inum(unsigned LONG_LONG);
#define ULL2NUM(v) rb_ull2inum(v)
#endif

#if SIZEOF_OFF_T > SIZEOF_LONG && defined(HAVE_LONG_LONG)
# define OFFT2NUM(v) LL2NUM(v)
#elif SIZEOF_OFF_T == SIZEOF_LONG
# define OFFT2NUM(v) LONG2NUM(v)
#else
# define OFFT2NUM(v) INT2NUM(v)
#endif

#if SIZEOF_SIZE_T > SIZEOF_LONG && defined(HAVE_LONG_LONG)
# define SIZET2NUM(v) ULL2NUM(v)
# define SSIZET2NUM(v) LL2NUM(v)
#elif SIZEOF_SIZE_T == SIZEOF_LONG
# define SIZET2NUM(v) ULONG2NUM(v)
# define SSIZET2NUM(v) LONG2NUM(v)
#else
# define SIZET2NUM(v) UINT2NUM(v)
# define SSIZET2NUM(v) INT2NUM(v)
#endif

#ifndef SIZE_MAX
# if SIZEOF_SIZE_T > SIZEOF_LONG && defined(HAVE_LONG_LONG)
#   define SIZE_MAX ULLONG_MAX
#   define SIZE_MIN ULLONG_MIN
# elif SIZEOF_SIZE_T == SIZEOF_LONG
#   define SIZE_MAX ULONG_MAX
#   define SIZE_MIN ULONG_MIN
# elif SIZEOF_SIZE_T == SIZEOF_INT
#   define SIZE_MAX UINT_MAX
#   define SIZE_MIN UINT_MIN
# else
#   define SIZE_MAX USHRT_MAX
#   define SIZE_MIN USHRT_MIN
# endif
#endif

#ifndef SSIZE_MAX
# if SIZEOF_SIZE_T > SIZEOF_LONG && defined(HAVE_LONG_LONG)
#   define SSIZE_MAX LLONG_MAX
#   define SSIZE_MIN LLONG_MIN
# elif SIZEOF_SIZE_T == SIZEOF_LONG
#   define SSIZE_MAX LONG_MAX
#   define SSIZE_MIN LONG_MIN
# elif SIZEOF_SIZE_T == SIZEOF_INT
#   define SSIZE_MAX INT_MAX
#   define SSIZE_MIN INT_MIN
# else
#   define SSIZE_MAX SHRT_MAX
#   define SSIZE_MIN SHRT_MIN
# endif
#endif

#if SIZEOF_INT < SIZEOF_VALUE
NORETURN(void rb_out_of_int(SIGNED_VALUE num));
#endif

#if SIZEOF_INT < SIZEOF_LONG
static inline int
rb_long2int_inline(long n)
{
    int i = (int)n;
    if ((long)i != n)
	rb_out_of_int(n);

    return i;
}
#define rb_long2int(n) rb_long2int_inline(n)
#else
#define rb_long2int(n) ((int)(n))
#endif

#ifndef PIDT2NUM
#define PIDT2NUM(v) LONG2NUM(v)
#endif
#ifndef NUM2PIDT
#define NUM2PIDT(v) NUM2LONG(v)
#endif
#ifndef UIDT2NUM
#define UIDT2NUM(v) LONG2NUM(v)
#endif
#ifndef NUM2UIDT
#define NUM2UIDT(v) NUM2LONG(v)
#endif
#ifndef GIDT2NUM
#define GIDT2NUM(v) LONG2NUM(v)
#endif
#ifndef NUM2GIDT
#define NUM2GIDT(v) NUM2LONG(v)
#endif
#ifndef NUM2MODET
#define NUM2MODET(v) NUM2INT(v)
#endif
#ifndef MODET2NUM
#define MODET2NUM(v) INT2NUM(v)
#endif

#define FIX2LONG(x) (long)RSHIFT((SIGNED_VALUE)(x),1)
#define FIX2ULONG(x) ((((VALUE)(x))>>1)&LONG_MAX)
#define FIXNUM_P(f) (((int)(SIGNED_VALUE)(f))&FIXNUM_FLAG)
#define POSFIXABLE(f) ((f) < FIXNUM_MAX+1)
#define NEGFIXABLE(f) ((f) >= FIXNUM_MIN)
#define FIXABLE(f) (POSFIXABLE(f) && NEGFIXABLE(f))

#define IMMEDIATE_P(x) ((VALUE)(x) & IMMEDIATE_MASK)

#define SYMBOL_P(x) (((VALUE)(x)&~(~(VALUE)0<<RUBY_SPECIAL_SHIFT))==SYMBOL_FLAG)
#define ID2SYM(x) (((VALUE)(x)<<RUBY_SPECIAL_SHIFT)|SYMBOL_FLAG)
#define SYM2ID(x) RSHIFT((unsigned long)(x),RUBY_SPECIAL_SHIFT)

/* Module#methods, #singleton_methods and so on return Symbols */
#define USE_SYMBOL_AS_METHOD_NAME 1

/* special constants - i.e. non-zero and non-fixnum constants */
enum ruby_special_consts {
    RUBY_Qfalse = 0,
    RUBY_Qtrue  = 2,
    RUBY_Qnil   = 4,
    RUBY_Qundef = 6,

    RUBY_IMMEDIATE_MASK = 0x03,
    RUBY_FIXNUM_FLAG    = 0x01,
    RUBY_SYMBOL_FLAG    = 0x0e,
    RUBY_SPECIAL_SHIFT  = 8
};

#define Qfalse ((VALUE)RUBY_Qfalse)
#define Qtrue  ((VALUE)RUBY_Qtrue)
#define Qnil   ((VALUE)RUBY_Qnil)
#define Qundef ((VALUE)RUBY_Qundef)	/* undefined value for placeholder */
#define IMMEDIATE_MASK RUBY_IMMEDIATE_MASK
#define FIXNUM_FLAG RUBY_FIXNUM_FLAG
#define SYMBOL_FLAG RUBY_SYMBOL_FLAG

#define RTEST(v) (((VALUE)(v) & ~Qnil) != 0)
#define NIL_P(v) ((VALUE)(v) == Qnil)

#define CLASS_OF(v) rb_class_of((VALUE)(v))

enum ruby_value_type {
    RUBY_T_NONE   = 0x00,

    RUBY_T_OBJECT = 0x01,
    RUBY_T_CLASS  = 0x02,
    RUBY_T_MODULE = 0x03,
    RUBY_T_FLOAT  = 0x04,
    RUBY_T_STRING = 0x05,
    RUBY_T_REGEXP = 0x06,
    RUBY_T_ARRAY  = 0x07,
    RUBY_T_HASH   = 0x08,
    RUBY_T_STRUCT = 0x09,
    RUBY_T_BIGNUM = 0x0a,
    RUBY_T_FILE   = 0x0b,
    RUBY_T_DATA   = 0x0c,
    RUBY_T_MATCH  = 0x0d,
    RUBY_T_COMPLEX  = 0x0e,
    RUBY_T_RATIONAL = 0x0f,

    RUBY_T_NIL    = 0x11,
    RUBY_T_TRUE   = 0x12,
    RUBY_T_FALSE  = 0x13,
    RUBY_T_SYMBOL = 0x14,
    RUBY_T_FIXNUM = 0x15,

    RUBY_T_UNDEF  = 0x1b,
    RUBY_T_NODE   = 0x1c,
    RUBY_T_ICLASS = 0x1d,
    RUBY_T_ZOMBIE = 0x1e,

    RUBY_T_MASK   = 0x1f
};

#define T_NONE   RUBY_T_NONE
#define T_NIL    RUBY_T_NIL
#define T_OBJECT RUBY_T_OBJECT
#define T_CLASS  RUBY_T_CLASS
#define T_ICLASS RUBY_T_ICLASS
#define T_MODULE RUBY_T_MODULE
#define T_FLOAT  RUBY_T_FLOAT
#define T_STRING RUBY_T_STRING
#define T_REGEXP RUBY_T_REGEXP
#define T_ARRAY  RUBY_T_ARRAY
#define T_HASH   RUBY_T_HASH
#define T_STRUCT RUBY_T_STRUCT
#define T_BIGNUM RUBY_T_BIGNUM
#define T_FILE   RUBY_T_FILE
#define T_FIXNUM RUBY_T_FIXNUM
#define T_TRUE   RUBY_T_TRUE
#define T_FALSE  RUBY_T_FALSE
#define T_DATA   RUBY_T_DATA
#define T_MATCH  RUBY_T_MATCH
#define T_SYMBOL RUBY_T_SYMBOL
#define T_RATIONAL RUBY_T_RATIONAL
#define T_COMPLEX RUBY_T_COMPLEX
#define T_UNDEF  RUBY_T_UNDEF
#define T_NODE   RUBY_T_NODE
#define T_ZOMBIE RUBY_T_ZOMBIE
#define T_MASK   RUBY_T_MASK

#define BUILTIN_TYPE(x) (int)(((struct RBasic*)(x))->flags & T_MASK)

static inline int rb_type(VALUE obj);
#define TYPE(x) rb_type((VALUE)(x))

#ifdef __GNUC__
#define RB_GC_GUARD_PTR(ptr) \
    __extension__ ({volatile VALUE *rb_gc_guarded_ptr = (ptr); rb_gc_guarded_ptr;})
#else
#ifdef _MSC_VER
#pragma optimize("", off)
#endif
static inline volatile VALUE *rb_gc_guarded_ptr(volatile VALUE *ptr) {return ptr;}
#ifdef _MSC_VER
#pragma optimize("", on)
#endif
#define RB_GC_GUARD_PTR(ptr) rb_gc_guarded_ptr(ptr)
#endif
#define RB_GC_GUARD(v) (*RB_GC_GUARD_PTR(&(v)))

void rb_check_type(VALUE,int);
#define Check_Type(v,t) rb_check_type((VALUE)(v),(t))

VALUE rb_str_to_str(VALUE);
VALUE rb_string_value(volatile VALUE*);
char *rb_string_value_ptr(volatile VALUE*);
char *rb_string_value_cstr(volatile VALUE*);

#define StringValue(v) rb_string_value(&(v))
#define StringValuePtr(v) rb_string_value_ptr(&(v))
#define StringValueCStr(v) rb_string_value_cstr(&(v))

void rb_check_safe_obj(VALUE);
DEPRECATED(void rb_check_safe_str(VALUE));
#define SafeStringValue(v) do {\
    StringValue(v);\
    rb_check_safe_obj(v);\
} while (0)
/* obsolete macro - use SafeStringValue(v) */
#define Check_SafeStr(v) rb_check_safe_str((VALUE)(v))

VALUE rb_str_export(VALUE);
#define ExportStringValue(v) do {\
    SafeStringValue(v);\
   (v) = rb_str_export(v);\
} while (0)
VALUE rb_str_export_locale(VALUE);

VALUE rb_get_path(VALUE);
#define FilePathValue(v) (RB_GC_GUARD(v) = rb_get_path(v))

VALUE rb_get_path_no_checksafe(VALUE);
#define FilePathStringValue(v) ((v) = rb_get_path_no_checksafe(v))

void rb_secure(int);
int rb_safe_level(void);
void rb_set_safe_level(int);
void rb_set_safe_level_force(int);
void rb_secure_update(VALUE);
NORETURN(void rb_insecure_operation(void));

VALUE rb_errinfo(void);
void rb_set_errinfo(VALUE);

SIGNED_VALUE rb_num2long(VALUE);
VALUE rb_num2ulong(VALUE);
static inline long
rb_num2long_inline(VALUE x)
{
    if (FIXNUM_P(x))
	return FIX2LONG(x);
    else
	return (long)rb_num2long(x);
}
#define NUM2LONG(x) rb_num2long_inline(x)
#define NUM2ULONG(x) rb_num2ulong(x)
#if SIZEOF_INT < SIZEOF_LONG
long rb_num2int(VALUE);
long rb_fix2int(VALUE);
#define FIX2INT(x) ((int)rb_fix2int((VALUE)(x)))

static inline int
rb_num2int_inline(VALUE x)
{
    if (FIXNUM_P(x))
	return FIX2INT(x);
    else
	return (int)rb_num2int(x);
}
#define NUM2INT(x) rb_num2int_inline(x)

unsigned long rb_num2uint(VALUE);
#define NUM2UINT(x) ((unsigned int)rb_num2uint(x))
unsigned long rb_fix2uint(VALUE);
#define FIX2UINT(x) ((unsigned int)rb_fix2uint(x))
#else /* SIZEOF_INT < SIZEOF_LONG */
#define NUM2INT(x) ((int)NUM2LONG(x))
#define NUM2UINT(x) ((unsigned int)NUM2ULONG(x))
#define FIX2INT(x) ((int)FIX2LONG(x))
#define FIX2UINT(x) ((unsigned int)FIX2ULONG(x))
#endif /* SIZEOF_INT < SIZEOF_LONG */

short rb_num2short(VALUE);
unsigned short rb_num2ushort(VALUE);
short rb_fix2short(VALUE);
unsigned short rb_fix2ushort(VALUE);
#define FIX2SHORT(x) (rb_fix2short((VALUE)(x)))
static inline short
rb_num2short_inline(VALUE x)
{
    if (FIXNUM_P(x))
	return FIX2SHORT(x);
    else
	return rb_num2short(x);
}

#define NUM2SHORT(x) rb_num2short_inline(x)
#define NUM2USHORT(x) rb_num2ushort(x)

#ifdef HAVE_LONG_LONG
LONG_LONG rb_num2ll(VALUE);
unsigned LONG_LONG rb_num2ull(VALUE);
static inline LONG_LONG
rb_num2ll_inline(VALUE x)
{
    if (FIXNUM_P(x))
	return FIX2LONG(x);
    else
	return rb_num2ll(x);
}
# define NUM2LL(x) rb_num2ll_inline(x)
# define NUM2ULL(x) rb_num2ull(x)
#endif

#if defined(HAVE_LONG_LONG) && SIZEOF_OFF_T > SIZEOF_LONG
# define NUM2OFFT(x) ((off_t)NUM2LL(x))
#else
# define NUM2OFFT(x) NUM2LONG(x)
#endif

#if defined(HAVE_LONG_LONG) && SIZEOF_SIZE_T > SIZEOF_LONG
# define NUM2SIZET(x) ((size_t)NUM2ULL(x))
# define NUM2SSIZET(x) ((size_t)NUM2LL(x))
#else
# define NUM2SIZET(x) NUM2ULONG(x)
# define NUM2SSIZET(x) NUM2LONG(x)
#endif

double rb_num2dbl(VALUE);
#define NUM2DBL(x) rb_num2dbl((VALUE)(x))

VALUE rb_uint2big(VALUE);
VALUE rb_int2big(SIGNED_VALUE);

VALUE rb_newobj(void);
#define NEWOBJ(obj,type) type *(obj) = (type*)rb_newobj()
#define OBJSETUP(obj,c,t) do {\
    RBASIC(obj)->flags = (t);\
    RBASIC(obj)->klass = (c);\
    if (rb_safe_level() >= 3) FL_SET((obj), FL_TAINT | FL_UNTRUSTED);\
} while (0)
#define CLONESETUP(clone,obj) do {\
    OBJSETUP((clone),rb_singleton_class_clone((VALUE)(obj)),RBASIC(obj)->flags);\
    rb_singleton_class_attached(RBASIC(clone)->klass, (VALUE)(clone));\
    if (FL_TEST((obj), FL_EXIVAR)) rb_copy_generic_ivar((VALUE)(clone),(VALUE)(obj));\
} while (0)
#define DUPSETUP(dup,obj) do {\
    OBJSETUP((dup),rb_obj_class(obj), (RBASIC(obj)->flags)&(T_MASK|FL_EXIVAR|FL_TAINT|FL_UNTRUSTED)); \
    if (FL_TEST((obj), FL_EXIVAR)) rb_copy_generic_ivar((VALUE)(dup),(VALUE)(obj));\
} while (0)

struct RBasic {
    VALUE flags;
    VALUE klass;
};

#define ROBJECT_EMBED_LEN_MAX 3
struct RObject {
    struct RBasic basic;
    union {
	struct {
	    long numiv;
	    VALUE *ivptr;
            struct st_table *iv_index_tbl; /* shortcut for RCLASS_IV_INDEX_TBL(rb_obj_class(obj)) */
	} heap;
	VALUE ary[ROBJECT_EMBED_LEN_MAX];
    } as;
};
#define ROBJECT_EMBED FL_USER1
#define ROBJECT_NUMIV(o) \
    ((RBASIC(o)->flags & ROBJECT_EMBED) ? \
     ROBJECT_EMBED_LEN_MAX : \
     ROBJECT(o)->as.heap.numiv)
#define ROBJECT_IVPTR(o) \
    ((RBASIC(o)->flags & ROBJECT_EMBED) ? \
     ROBJECT(o)->as.ary : \
     ROBJECT(o)->as.heap.ivptr)
#define ROBJECT_IV_INDEX_TBL(o) \
    ((RBASIC(o)->flags & ROBJECT_EMBED) ? \
     RCLASS_IV_INDEX_TBL(rb_obj_class(o)) : \
     ROBJECT(o)->as.heap.iv_index_tbl)

/** @internal */
typedef struct rb_classext_struct rb_classext_t;

struct RClass {
    struct RBasic basic;
    rb_classext_t *ptr;
    struct st_table *m_tbl;
    struct st_table *iv_index_tbl;
};
#define RCLASS_SUPER(c) rb_class_get_superclass(c)
#define RMODULE_IV_TBL(m) RCLASS_IV_TBL(m)
#define RMODULE_CONST_TBL(m) RCLASS_CONST_TBL(m)
#define RMODULE_M_TBL(m) RCLASS_M_TBL(m)
#define RMODULE_SUPER(m) RCLASS_SUPER(m)

struct RFloat {
    struct RBasic basic;
    double float_value;
};
#define RFLOAT_VALUE(v) (RFLOAT(v)->float_value)
#define DBL2NUM(dbl)  rb_float_new(dbl)

#define ELTS_SHARED FL_USER2

#define RSTRING_EMBED_LEN_MAX ((int)((sizeof(VALUE)*3)/sizeof(char)-1))
struct RString {
    struct RBasic basic;
    union {
	struct {
	    long len;
	    char *ptr;
	    union {
		long capa;
		VALUE shared;
	    } aux;
	} heap;
	char ary[RSTRING_EMBED_LEN_MAX + 1];
    } as;
};
#define RSTRING_NOEMBED FL_USER1
#define RSTRING_EMBED_LEN_MASK (FL_USER2|FL_USER3|FL_USER4|FL_USER5|FL_USER6)
#define RSTRING_EMBED_LEN_SHIFT (FL_USHIFT+2)
#define RSTRING_EMBED_LEN(str) \
     (long)((RBASIC(str)->flags >> RSTRING_EMBED_LEN_SHIFT) & \
            (RSTRING_EMBED_LEN_MASK >> RSTRING_EMBED_LEN_SHIFT))
#define RSTRING_LEN(str) \
    (!(RBASIC(str)->flags & RSTRING_NOEMBED) ? \
     RSTRING_EMBED_LEN(str) : \
     RSTRING(str)->as.heap.len)
#define RSTRING_PTR(str) \
    (!(RBASIC(str)->flags & RSTRING_NOEMBED) ? \
     RSTRING(str)->as.ary : \
     RSTRING(str)->as.heap.ptr)
#define RSTRING_END(str) \
    (!(RBASIC(str)->flags & RSTRING_NOEMBED) ? \
     (RSTRING(str)->as.ary + RSTRING_EMBED_LEN(str)) : \
     (RSTRING(str)->as.heap.ptr + RSTRING(str)->as.heap.len))
#define RSTRING_LENINT(str) rb_long2int(RSTRING_LEN(str))
#define RSTRING_GETMEM(str, ptrvar, lenvar) \
    (!(RBASIC(str)->flags & RSTRING_NOEMBED) ? \
     ((ptrvar) = RSTRING(str)->as.ary, (lenvar) = RSTRING_EMBED_LEN(str)) : \
     ((ptrvar) = RSTRING(str)->as.heap.ptr, (lenvar) = RSTRING(str)->as.heap.len))

#define RARRAY_EMBED_LEN_MAX 3
struct RArray {
    struct RBasic basic;
    union {
	struct {
	    long len;
	    union {
		long capa;
		VALUE shared;
	    } aux;
	    VALUE *ptr;
	} heap;
	VALUE ary[RARRAY_EMBED_LEN_MAX];
    } as;
};
#define RARRAY_EMBED_FLAG FL_USER1
/* FL_USER2 is for ELTS_SHARED */
#define RARRAY_EMBED_LEN_MASK (FL_USER4|FL_USER3)
#define RARRAY_EMBED_LEN_SHIFT (FL_USHIFT+3)
#define RARRAY_LEN(a) \
    ((RBASIC(a)->flags & RARRAY_EMBED_FLAG) ? \
     (long)((RBASIC(a)->flags >> RARRAY_EMBED_LEN_SHIFT) & \
	 (RARRAY_EMBED_LEN_MASK >> RARRAY_EMBED_LEN_SHIFT)) : \
     RARRAY(a)->as.heap.len)
#define RARRAY_PTR(a) \
    ((RBASIC(a)->flags & RARRAY_EMBED_FLAG) ? \
     RARRAY(a)->as.ary : \
     RARRAY(a)->as.heap.ptr)
#define RARRAY_LENINT(ary) rb_long2int(RARRAY_LEN(ary))

struct RRegexp {
    struct RBasic basic;
    struct re_pattern_buffer *ptr;
    VALUE src;
    unsigned long usecnt;
};
#define RREGEXP_SRC(r) RREGEXP(r)->src
#define RREGEXP_SRC_PTR(r) RSTRING_PTR(RREGEXP(r)->src)
#define RREGEXP_SRC_LEN(r) RSTRING_LEN(RREGEXP(r)->src)
#define RREGEXP_SRC_END(r) RSTRING_END(RREGEXP(r)->src)

struct RHash {
    struct RBasic basic;
    struct st_table *ntbl;      /* possibly 0 */
    int iter_lev;
    VALUE ifnone;
};
/* RHASH_TBL allocates st_table if not available. */
#define RHASH_TBL(h) rb_hash_tbl(h)
#define RHASH_ITER_LEV(h) (RHASH(h)->iter_lev)
#define RHASH_IFNONE(h) (RHASH(h)->ifnone)
#define RHASH_SIZE(h) (RHASH(h)->ntbl ? RHASH(h)->ntbl->num_entries : 0)
#define RHASH_EMPTY_P(h) (RHASH_SIZE(h) == 0)

struct RFile {
    struct RBasic basic;
    struct rb_io_t *fptr;
};

struct RRational {
    struct RBasic basic;
    VALUE num;
    VALUE den;
};

struct RComplex {
    struct RBasic basic;
    VALUE real;
    VALUE imag;
};

struct RData {
    struct RBasic basic;
    void (*dmark)(void*);
    void (*dfree)(void*);
    void *data;
};

typedef struct rb_data_type_struct rb_data_type_t;

struct rb_data_type_struct {
    const char *wrap_struct_name;
    struct {
	void (*dmark)(void*);
	void (*dfree)(void*);
	size_t (*dsize)(const void *);
	void *reserved[2]; /* For future extension.
			      This array *must* be filled with ZERO. */
    } function;
    const rb_data_type_t *parent;
    void *data;        /* This area can be used for any purpose
                          by a programmer who define the type. */
};

#define HAVE_TYPE_RB_DATA_TYPE_T 1
#define HAVE_RB_DATA_TYPE_T_FUNCTION 1
#define HAVE_RB_DATA_TYPE_T_PARENT 1

struct RTypedData {
    struct RBasic basic;
    const rb_data_type_t *type;
    VALUE typed_flag; /* 1 or not */
    void *data;
};

#define DATA_PTR(dta) (RDATA(dta)->data)

#define RTYPEDDATA_P(v)    (RTYPEDDATA(v)->typed_flag == 1)
#define RTYPEDDATA_TYPE(v) (RTYPEDDATA(v)->type)
#define RTYPEDDATA_DATA(v) (RTYPEDDATA(v)->data)

/*
#define RUBY_DATA_FUNC(func) ((void (*)(void*))(func))
*/
typedef void (*RUBY_DATA_FUNC)(void*);

VALUE rb_data_object_alloc(VALUE,void*,RUBY_DATA_FUNC,RUBY_DATA_FUNC);
VALUE rb_data_typed_object_alloc(VALUE klass, void *datap, const rb_data_type_t *);
int rb_typeddata_inherited_p(const rb_data_type_t *child, const rb_data_type_t *parent);
int rb_typeddata_is_kind_of(VALUE, const rb_data_type_t *);
void *rb_check_typeddata(VALUE, const rb_data_type_t *);
#define Check_TypedStruct(v,t) rb_check_typeddata((VALUE)(v),(t))
#define RUBY_DEFAULT_FREE ((RUBY_DATA_FUNC)-1)
#define RUBY_NEVER_FREE   ((RUBY_DATA_FUNC)0)
#define RUBY_TYPED_DEFAULT_FREE RUBY_DEFAULT_FREE
#define RUBY_TYPED_NEVER_FREE   RUBY_NEVER_FREE

#define Data_Wrap_Struct(klass,mark,free,sval)\
    rb_data_object_alloc((klass),(sval),(RUBY_DATA_FUNC)(mark),(RUBY_DATA_FUNC)(free))

#define Data_Make_Struct(klass,type,mark,free,sval) (\
    (sval) = ALLOC(type),\
    memset((sval), 0, sizeof(type)),\
    Data_Wrap_Struct((klass),(mark),(free),(sval))\
)

#define TypedData_Wrap_Struct(klass,data_type,sval)\
  rb_data_typed_object_alloc((klass),(sval),(data_type))

#define TypedData_Make_Struct(klass, type, data_type, sval) (\
    (sval) = ALLOC(type),\
    memset((sval), 0, sizeof(type)),\
    TypedData_Wrap_Struct((klass),(data_type),(sval))\
)

#define Data_Get_Struct(obj,type,sval) do {\
    Check_Type((obj), T_DATA); \
    (sval) = (type*)DATA_PTR(obj);\
} while (0)

#define TypedData_Get_Struct(obj,type,data_type,sval) do {\
    (sval) = (type*)rb_check_typeddata((obj), (data_type)); \
} while (0)

#define RSTRUCT_EMBED_LEN_MAX 3
struct RStruct {
    struct RBasic basic;
    union {
	struct {
	    long len;
	    VALUE *ptr;
	} heap;
	VALUE ary[RSTRUCT_EMBED_LEN_MAX];
    } as;
};
#define RSTRUCT_EMBED_LEN_MASK (FL_USER2|FL_USER1)
#define RSTRUCT_EMBED_LEN_SHIFT (FL_USHIFT+1)
#define RSTRUCT_LEN(st) \
    ((RBASIC(st)->flags & RSTRUCT_EMBED_LEN_MASK) ? \
     (long)((RBASIC(st)->flags >> RSTRUCT_EMBED_LEN_SHIFT) & \
            (RSTRUCT_EMBED_LEN_MASK >> RSTRUCT_EMBED_LEN_SHIFT)) : \
     RSTRUCT(st)->as.heap.len)
#define RSTRUCT_PTR(st) \
    ((RBASIC(st)->flags & RSTRUCT_EMBED_LEN_MASK) ? \
     RSTRUCT(st)->as.ary : \
     RSTRUCT(st)->as.heap.ptr)
#define RSTRUCT_LENINT(st) rb_long2int(RSTRUCT_LEN(st))

#define RBIGNUM_EMBED_LEN_MAX ((int)((sizeof(VALUE)*3)/sizeof(BDIGIT)))
struct RBignum {
    struct RBasic basic;
    union {
        struct {
            long len;
            BDIGIT *digits;
        } heap;
        BDIGIT ary[RBIGNUM_EMBED_LEN_MAX];
    } as;
};
#define RBIGNUM_SIGN_BIT FL_USER1
/* sign: positive:1, negative:0 */
#define RBIGNUM_SIGN(b) ((RBASIC(b)->flags & RBIGNUM_SIGN_BIT) != 0)
#define RBIGNUM_SET_SIGN(b,sign) \
  ((sign) ? (RBASIC(b)->flags |= RBIGNUM_SIGN_BIT) \
          : (RBASIC(b)->flags &= ~RBIGNUM_SIGN_BIT))
#define RBIGNUM_POSITIVE_P(b) RBIGNUM_SIGN(b)
#define RBIGNUM_NEGATIVE_P(b) (!RBIGNUM_SIGN(b))

#define RBIGNUM_EMBED_FLAG FL_USER2
#define RBIGNUM_EMBED_LEN_MASK (FL_USER5|FL_USER4|FL_USER3)
#define RBIGNUM_EMBED_LEN_SHIFT (FL_USHIFT+3)
#define RBIGNUM_LEN(b) \
    ((RBASIC(b)->flags & RBIGNUM_EMBED_FLAG) ? \
     (long)((RBASIC(b)->flags >> RBIGNUM_EMBED_LEN_SHIFT) & \
            (RBIGNUM_EMBED_LEN_MASK >> RBIGNUM_EMBED_LEN_SHIFT)) : \
     RBIGNUM(b)->as.heap.len)
/* LSB:RBIGNUM_DIGITS(b)[0], MSB:RBIGNUM_DIGITS(b)[RBIGNUM_LEN(b)-1] */
#define RBIGNUM_DIGITS(b) \
    ((RBASIC(b)->flags & RBIGNUM_EMBED_FLAG) ? \
     RBIGNUM(b)->as.ary : \
     RBIGNUM(b)->as.heap.digits)
#define RBIGNUM_LENINT(b) rb_long2int(RBIGNUM_LEN(b))

#define R_CAST(st)   (struct st*)
#define RBASIC(obj)  (R_CAST(RBasic)(obj))
#define ROBJECT(obj) (R_CAST(RObject)(obj))
#define RCLASS(obj)  (R_CAST(RClass)(obj))
#define RMODULE(obj) RCLASS(obj)
#define RFLOAT(obj)  (R_CAST(RFloat)(obj))
#define RSTRING(obj) (R_CAST(RString)(obj))
#define RREGEXP(obj) (R_CAST(RRegexp)(obj))
#define RARRAY(obj)  (R_CAST(RArray)(obj))
#define RHASH(obj)   (R_CAST(RHash)(obj))
#define RDATA(obj)   (R_CAST(RData)(obj))
#define RTYPEDDATA(obj)   (R_CAST(RTypedData)(obj))
#define RSTRUCT(obj) (R_CAST(RStruct)(obj))
#define RBIGNUM(obj) (R_CAST(RBignum)(obj))
#define RFILE(obj)   (R_CAST(RFile)(obj))
#define RRATIONAL(obj) (R_CAST(RRational)(obj))
#define RCOMPLEX(obj) (R_CAST(RComplex)(obj))

#define FL_SINGLETON FL_USER0
#define FL_RESERVED1 (((VALUE)1)<<5)
#define FL_RESERVED2 (((VALUE)1)<<6) /* will be used in the future GC */
#define FL_FINALIZE  (((VALUE)1)<<7)
#define FL_TAINT     (((VALUE)1)<<8)
#define FL_UNTRUSTED (((VALUE)1)<<9)
#define FL_EXIVAR    (((VALUE)1)<<10)
#define FL_FREEZE    (((VALUE)1)<<11)

#define FL_USHIFT    12

#define FL_USER0     (((VALUE)1)<<(FL_USHIFT+0))
#define FL_USER1     (((VALUE)1)<<(FL_USHIFT+1))
#define FL_USER2     (((VALUE)1)<<(FL_USHIFT+2))
#define FL_USER3     (((VALUE)1)<<(FL_USHIFT+3))
#define FL_USER4     (((VALUE)1)<<(FL_USHIFT+4))
#define FL_USER5     (((VALUE)1)<<(FL_USHIFT+5))
#define FL_USER6     (((VALUE)1)<<(FL_USHIFT+6))
#define FL_USER7     (((VALUE)1)<<(FL_USHIFT+7))
#define FL_USER8     (((VALUE)1)<<(FL_USHIFT+8))
#define FL_USER9     (((VALUE)1)<<(FL_USHIFT+9))
#define FL_USER10    (((VALUE)1)<<(FL_USHIFT+10))
#define FL_USER11    (((VALUE)1)<<(FL_USHIFT+11))
#define FL_USER12    (((VALUE)1)<<(FL_USHIFT+12))
#define FL_USER13    (((VALUE)1)<<(FL_USHIFT+13))
#define FL_USER14    (((VALUE)1)<<(FL_USHIFT+14))
#define FL_USER15    (((VALUE)1)<<(FL_USHIFT+15))
#define FL_USER16    (((VALUE)1)<<(FL_USHIFT+16))
#define FL_USER17    (((VALUE)1)<<(FL_USHIFT+17))
#define FL_USER18    (((VALUE)1)<<(FL_USHIFT+18))
#define FL_USER19    (((VALUE)1)<<(FL_USHIFT+19))

#define SPECIAL_CONST_P(x) (IMMEDIATE_P(x) || !RTEST(x))

#define FL_ABLE(x) (!SPECIAL_CONST_P(x) && BUILTIN_TYPE(x) != T_NODE)
#define FL_TEST(x,f) (FL_ABLE(x)?(RBASIC(x)->flags&(f)):0)
#define FL_ANY(x,f) FL_TEST((x),(f))
#define FL_ALL(x,f) (FL_TEST((x),(f)) == (f))
#define FL_SET(x,f) do {if (FL_ABLE(x)) RBASIC(x)->flags |= (f);} while (0)
#define FL_UNSET(x,f) do {if (FL_ABLE(x)) RBASIC(x)->flags &= ~(f);} while (0)
#define FL_REVERSE(x,f) do {if (FL_ABLE(x)) RBASIC(x)->flags ^= (f);} while (0)

#define OBJ_TAINTED(x) (!!FL_TEST((x), FL_TAINT))
#define OBJ_TAINT(x) FL_SET((x), FL_TAINT)
#define OBJ_UNTRUSTED(x) (!!FL_TEST((x), FL_UNTRUSTED))
#define OBJ_UNTRUST(x) FL_SET((x), FL_UNTRUSTED)
#define OBJ_INFECT(x,s) do {if (FL_ABLE(x) && FL_ABLE(s)) RBASIC(x)->flags |= RBASIC(s)->flags & (FL_TAINT | FL_UNTRUSTED);} while (0)

#define OBJ_FROZEN(x) (!!FL_TEST((x), FL_FREEZE))
#define OBJ_FREEZE(x) FL_SET((x), FL_FREEZE)

#if SIZEOF_INT < SIZEOF_LONG
# define INT2NUM(v) INT2FIX((int)(v))
# define UINT2NUM(v) LONG2FIX((unsigned int)(v))
#else
static inline VALUE
rb_int2num_inline(int v)
{
    if (FIXABLE(v))
	return INT2FIX(v);
    else
	return rb_int2big(v);
}
#define INT2NUM(x) rb_int2num_inline(x)

static inline VALUE
rb_uint2num_inline(unsigned int v)
{
    if (POSFIXABLE(v))
	return LONG2FIX(v);
    else
	return rb_uint2big(v);
}
#define UINT2NUM(x) rb_uint2num_inline(x)
#endif

static inline VALUE
rb_long2num_inline(long v)
{
    if (FIXABLE(v))
	return LONG2FIX(v);
    else
	return rb_int2big(v);
}
#define LONG2NUM(x) rb_long2num_inline(x)

static inline VALUE
rb_ulong2num_inline(unsigned long v)
{
    if (POSFIXABLE(v))
	return LONG2FIX(v);
    else
	return rb_uint2big(v);
}
#define ULONG2NUM(x) rb_ulong2num_inline(x)

static inline char
rb_num2char_inline(VALUE x)
{
    if ((TYPE(x) == T_STRING) && (RSTRING_LEN(x)>=1))
	return RSTRING_PTR(x)[0];
    else
	return (char)(NUM2INT(x) & 0xff);
}
#define NUM2CHR(x) rb_num2char_inline(x)

#define CHR2FIX(x) INT2FIX((long)((x)&0xff))

#define ALLOC_N(type,n) ((type*)xmalloc2((n),sizeof(type)))
#define ALLOC(type) ((type*)xmalloc(sizeof(type)))
#define REALLOC_N(var,type,n) ((var)=(type*)xrealloc2((char*)(var),(n),sizeof(type)))

#define ALLOCA_N(type,n) ((type*)alloca(sizeof(type)*(n)))

void *rb_alloc_tmp_buffer(volatile VALUE *store, long len);
void rb_free_tmp_buffer(volatile VALUE *store);
/* allocates _n_ bytes temporary buffer and stores VALUE including it
 * in _v_.  _n_ may be evaluated twice. */
#ifdef C_ALLOCA
# define ALLOCV(v, n) rb_alloc_tmp_buffer(&(v), (n))
#else
# define ALLOCV(v, n) ((n) < 1024 ? (RB_GC_GUARD(v) = 0, alloca(n)) : rb_alloc_tmp_buffer(&(v), (n)))
#endif
#define ALLOCV_N(type, v, n) ((type*)ALLOCV((v), sizeof(type)*(n)))
#define ALLOCV_END(v) rb_free_tmp_buffer(&(v))

#define MEMZERO(p,type,n) memset((p), 0, sizeof(type)*(n))
#define MEMCPY(p1,p2,type,n) memcpy((p1), (p2), sizeof(type)*(n))
#define MEMMOVE(p1,p2,type,n) memmove((p1), (p2), sizeof(type)*(n))
#define MEMCMP(p1,p2,type,n) memcmp((p1), (p2), sizeof(type)*(n))

void rb_obj_infect(VALUE,VALUE);

typedef int ruby_glob_func(const char*,VALUE, void*);
void rb_glob(const char*,void(*)(const char*,VALUE,void*),VALUE);
int ruby_glob(const char*,int,ruby_glob_func*,VALUE);
int ruby_brace_glob(const char*,int,ruby_glob_func*,VALUE);

VALUE rb_define_class(const char*,VALUE);
VALUE rb_define_module(const char*);
VALUE rb_define_class_under(VALUE, const char*, VALUE);
VALUE rb_define_module_under(VALUE, const char*);

void rb_include_module(VALUE,VALUE);
void rb_extend_object(VALUE,VALUE);

struct rb_global_variable;

typedef VALUE rb_gvar_getter_t(ID id, void *data, struct rb_global_variable *gvar);
typedef void  rb_gvar_setter_t(VALUE val, ID id, void *data, struct rb_global_variable *gvar);
typedef void  rb_gvar_marker_t(VALUE *var);

VALUE rb_gvar_undef_getter(ID id, void *data, struct rb_global_variable *gvar);
void  rb_gvar_undef_setter(VALUE val, ID id, void *data, struct rb_global_variable *gvar);
void  rb_gvar_undef_marker(VALUE *var);

VALUE rb_gvar_val_getter(ID id, void *data, struct rb_global_variable *gvar);
void  rb_gvar_val_setter(VALUE val, ID id, void *data, struct rb_global_variable *gvar);
void  rb_gvar_val_marker(VALUE *var);

VALUE rb_gvar_var_getter(ID id, void *data, struct rb_global_variable *gvar);
void  rb_gvar_var_setter(VALUE val, ID id, void *data, struct rb_global_variable *gvar);
void  rb_gvar_var_marker(VALUE *var);

void  rb_gvar_readonly_setter(VALUE val, ID id, void *data, struct rb_global_variable *gvar);

void rb_define_variable(const char*,VALUE*);
void rb_define_virtual_variable(const char*,VALUE(*)(ANYARGS),void(*)(ANYARGS));
void rb_define_hooked_variable(const char*,VALUE*,VALUE(*)(ANYARGS),void(*)(ANYARGS));
void rb_define_readonly_variable(const char*,VALUE*);
void rb_define_const(VALUE,const char*,VALUE);
void rb_define_global_const(const char*,VALUE);

#define RUBY_METHOD_FUNC(func) ((VALUE (*)(ANYARGS))(func))
void rb_define_method(VALUE,const char*,VALUE(*)(ANYARGS),int);
void rb_define_module_function(VALUE,const char*,VALUE(*)(ANYARGS),int);
void rb_define_global_function(const char*,VALUE(*)(ANYARGS),int);

void rb_undef_method(VALUE,const char*);
void rb_define_alias(VALUE,const char*,const char*);
void rb_define_attr(VALUE,const char*,int,int);

void rb_global_variable(VALUE*);
void rb_gc_register_mark_object(VALUE);
void rb_gc_register_address(VALUE*);
void rb_gc_unregister_address(VALUE*);

ID rb_intern(const char*);
ID rb_intern2(const char*, long);
ID rb_intern_str(VALUE str);
const char *rb_id2name(ID);
ID rb_check_id(volatile VALUE *);
ID rb_to_id(VALUE);
VALUE rb_id2str(ID);

#define CONST_ID_CACHE(result, str)			\
    {							\
	static ID rb_intern_id_cache;			\
	if (!rb_intern_id_cache)			\
	    rb_intern_id_cache = rb_intern2((str), (long)strlen(str)); \
	result rb_intern_id_cache;			\
    }
#define CONST_ID(var, str) \
    do CONST_ID_CACHE((var) =, (str)) while (0)
#ifdef __GNUC__
/* __builtin_constant_p and statement expression is available
 * since gcc-2.7.2.3 at least. */
#define rb_intern(str) \
    (__builtin_constant_p(str) ? \
        __extension__ (CONST_ID_CACHE((ID), (str))) : \
        rb_intern(str))
#define rb_intern_const(str) \
    (__builtin_constant_p(str) ? \
     __extension__ (rb_intern2((str), (long)strlen(str))) : \
     (rb_intern)(str))
#else
#define rb_intern_const(str) rb_intern2((str), (long)strlen(str))
#endif

const char *rb_class2name(VALUE);
const char *rb_obj_classname(VALUE);

void rb_p(VALUE);

VALUE rb_eval_string(const char*);
VALUE rb_eval_string_protect(const char*, int*);
VALUE rb_eval_string_wrap(const char*, int*);
VALUE rb_funcall(VALUE, ID, int, ...);
VALUE rb_funcall2(VALUE, ID, int, const VALUE*);
VALUE rb_funcall3(VALUE, ID, int, const VALUE*);
VALUE rb_funcall_passing_block(VALUE, ID, int, const VALUE*);
int rb_scan_args(int, const VALUE*, const char*, ...);
VALUE rb_call_super(int, const VALUE*);

VALUE rb_gv_set(const char*, VALUE);
VALUE rb_gv_get(const char*);
VALUE rb_iv_get(VALUE, const char*);
VALUE rb_iv_set(VALUE, const char*, VALUE);

VALUE rb_equal(VALUE,VALUE);

VALUE *rb_ruby_verbose_ptr(void);
VALUE *rb_ruby_debug_ptr(void);
#define ruby_verbose (*rb_ruby_verbose_ptr())
#define ruby_debug   (*rb_ruby_debug_ptr())

PRINTF_ARGS(NORETURN(void rb_raise(VALUE, const char*, ...)), 2, 3);
PRINTF_ARGS(NORETURN(void rb_fatal(const char*, ...)), 1, 2);
PRINTF_ARGS(NORETURN(void rb_bug(const char*, ...)), 1, 2);
NORETURN(void rb_bug_errno(const char*, int));
NORETURN(void rb_sys_fail(const char*));
NORETURN(void rb_sys_fail_str(VALUE));
NORETURN(void rb_mod_sys_fail(VALUE, const char*));
NORETURN(void rb_mod_sys_fail_str(VALUE, VALUE));
NORETURN(void rb_iter_break(void));
NORETURN(void rb_iter_break_value(VALUE));
NORETURN(void rb_exit(int));
NORETURN(void rb_notimplement(void));
VALUE rb_syserr_new(int, const char *);
VALUE rb_syserr_new_str(int n, VALUE arg);
NORETURN(void rb_syserr_fail(int, const char*));
NORETURN(void rb_syserr_fail_str(int, VALUE));
NORETURN(void rb_mod_syserr_fail(VALUE, int, const char*));
NORETURN(void rb_mod_syserr_fail_str(VALUE, int, VALUE));

/* reports if `-W' specified */
PRINTF_ARGS(void rb_warning(const char*, ...), 1, 2);
PRINTF_ARGS(void rb_compile_warning(const char *, int, const char*, ...), 3, 4);
PRINTF_ARGS(void rb_sys_warning(const char*, ...), 1, 2);
/* reports always */
PRINTF_ARGS(void rb_warn(const char*, ...), 1, 2);
PRINTF_ARGS(void rb_compile_warn(const char *, int, const char*, ...), 3, 4);

typedef VALUE rb_block_call_func(VALUE, VALUE, int, VALUE*);

VALUE rb_each(VALUE);
VALUE rb_yield(VALUE);
VALUE rb_yield_values(int n, ...);
VALUE rb_yield_values2(int n, const VALUE *argv);
VALUE rb_yield_splat(VALUE);
int rb_block_given_p(void);
void rb_need_block(void);
VALUE rb_iterate(VALUE(*)(VALUE),VALUE,VALUE(*)(ANYARGS),VALUE);
VALUE rb_block_call(VALUE,ID,int,VALUE*,VALUE(*)(ANYARGS),VALUE);
VALUE rb_rescue(VALUE(*)(ANYARGS),VALUE,VALUE(*)(ANYARGS),VALUE);
VALUE rb_rescue2(VALUE(*)(ANYARGS),VALUE,VALUE(*)(ANYARGS),VALUE,...);
VALUE rb_ensure(VALUE(*)(ANYARGS),VALUE,VALUE(*)(ANYARGS),VALUE);
VALUE rb_catch(const char*,VALUE(*)(ANYARGS),VALUE);
VALUE rb_catch_obj(VALUE,VALUE(*)(ANYARGS),VALUE);
NORETURN(void rb_throw(const char*,VALUE));
NORETURN(void rb_throw_obj(VALUE,VALUE));

VALUE rb_require(const char*);

#ifdef __ia64
void ruby_init_stack(volatile VALUE*, void*);
#define ruby_init_stack(addr) ruby_init_stack((addr), rb_ia64_bsp())
#else
void ruby_init_stack(volatile VALUE*);
#endif
#define RUBY_INIT_STACK \
    VALUE variable_in_this_stack_frame; \
    ruby_init_stack(&variable_in_this_stack_frame);
void ruby_init(void);
void *ruby_options(int, char**);
int ruby_run_node(void *);
int ruby_exec_node(void *);
int ruby_executable_node(void *n, int *status);

RUBY_EXTERN VALUE rb_mKernel;
RUBY_EXTERN VALUE rb_mComparable;
RUBY_EXTERN VALUE rb_mEnumerable;
RUBY_EXTERN VALUE rb_mErrno;
RUBY_EXTERN VALUE rb_mFileTest;
RUBY_EXTERN VALUE rb_mGC;
RUBY_EXTERN VALUE rb_mMath;
RUBY_EXTERN VALUE rb_mProcess;
RUBY_EXTERN VALUE rb_mWaitReadable;
RUBY_EXTERN VALUE rb_mWaitWritable;

RUBY_EXTERN VALUE rb_cBasicObject;
RUBY_EXTERN VALUE rb_cObject;
RUBY_EXTERN VALUE rb_cArray;
RUBY_EXTERN VALUE rb_cBignum;
RUBY_EXTERN VALUE rb_cBinding;
RUBY_EXTERN VALUE rb_cClass;
RUBY_EXTERN VALUE rb_cCont;
RUBY_EXTERN VALUE rb_cDir;
RUBY_EXTERN VALUE rb_cData;
RUBY_EXTERN VALUE rb_cFalseClass;
RUBY_EXTERN VALUE rb_cEncoding;
RUBY_EXTERN VALUE rb_cEnumerator;
RUBY_EXTERN VALUE rb_cFile;
RUBY_EXTERN VALUE rb_cFixnum;
RUBY_EXTERN VALUE rb_cFloat;
RUBY_EXTERN VALUE rb_cHash;
RUBY_EXTERN VALUE rb_cInteger;
RUBY_EXTERN VALUE rb_cIO;
RUBY_EXTERN VALUE rb_cMatch;
RUBY_EXTERN VALUE rb_cMethod;
RUBY_EXTERN VALUE rb_cModule;
RUBY_EXTERN VALUE rb_cNameErrorMesg;
RUBY_EXTERN VALUE rb_cNilClass;
RUBY_EXTERN VALUE rb_cNumeric;
RUBY_EXTERN VALUE rb_cProc;
RUBY_EXTERN VALUE rb_cRandom;
RUBY_EXTERN VALUE rb_cRange;
RUBY_EXTERN VALUE rb_cRational;
RUBY_EXTERN VALUE rb_cComplex;
RUBY_EXTERN VALUE rb_cRegexp;
RUBY_EXTERN VALUE rb_cStat;
RUBY_EXTERN VALUE rb_cString;
RUBY_EXTERN VALUE rb_cStruct;
RUBY_EXTERN VALUE rb_cSymbol;
RUBY_EXTERN VALUE rb_cThread;
RUBY_EXTERN VALUE rb_cTime;
RUBY_EXTERN VALUE rb_cTrueClass;
RUBY_EXTERN VALUE rb_cUnboundMethod;

RUBY_EXTERN VALUE rb_eException;
RUBY_EXTERN VALUE rb_eStandardError;
RUBY_EXTERN VALUE rb_eSystemExit;
RUBY_EXTERN VALUE rb_eInterrupt;
RUBY_EXTERN VALUE rb_eSignal;
RUBY_EXTERN VALUE rb_eFatal;
RUBY_EXTERN VALUE rb_eArgError;
RUBY_EXTERN VALUE rb_eEOFError;
RUBY_EXTERN VALUE rb_eIndexError;
RUBY_EXTERN VALUE rb_eStopIteration;
RUBY_EXTERN VALUE rb_eKeyError;
RUBY_EXTERN VALUE rb_eRangeError;
RUBY_EXTERN VALUE rb_eIOError;
RUBY_EXTERN VALUE rb_eRuntimeError;
RUBY_EXTERN VALUE rb_eSecurityError;
RUBY_EXTERN VALUE rb_eSystemCallError;
RUBY_EXTERN VALUE rb_eThreadError;
RUBY_EXTERN VALUE rb_eTypeError;
RUBY_EXTERN VALUE rb_eZeroDivError;
RUBY_EXTERN VALUE rb_eNotImpError;
RUBY_EXTERN VALUE rb_eNoMemError;
RUBY_EXTERN VALUE rb_eNoMethodError;
RUBY_EXTERN VALUE rb_eFloatDomainError;
RUBY_EXTERN VALUE rb_eLocalJumpError;
RUBY_EXTERN VALUE rb_eSysStackError;
RUBY_EXTERN VALUE rb_eRegexpError;
RUBY_EXTERN VALUE rb_eEncodingError;
RUBY_EXTERN VALUE rb_eEncCompatError;

RUBY_EXTERN VALUE rb_eScriptError;
RUBY_EXTERN VALUE rb_eNameError;
RUBY_EXTERN VALUE rb_eSyntaxError;
RUBY_EXTERN VALUE rb_eLoadError;

RUBY_EXTERN VALUE rb_eMathDomainError;

RUBY_EXTERN VALUE rb_stdin, rb_stdout, rb_stderr;

static inline VALUE
rb_class_of(VALUE obj)
{
    if (IMMEDIATE_P(obj)) {
	if (FIXNUM_P(obj)) return rb_cFixnum;
	if (obj == Qtrue)  return rb_cTrueClass;
	if (SYMBOL_P(obj)) return rb_cSymbol;
    }
    else if (!RTEST(obj)) {
	if (obj == Qnil)   return rb_cNilClass;
	if (obj == Qfalse) return rb_cFalseClass;
    }
    return RBASIC(obj)->klass;
}

static inline int
rb_type(VALUE obj)
{
    if (IMMEDIATE_P(obj)) {
	if (FIXNUM_P(obj)) return T_FIXNUM;
	if (obj == Qtrue) return T_TRUE;
	if (SYMBOL_P(obj)) return T_SYMBOL;
	if (obj == Qundef) return T_UNDEF;
    }
    else if (!RTEST(obj)) {
	if (obj == Qnil) return T_NIL;
	if (obj == Qfalse) return T_FALSE;
    }
    return BUILTIN_TYPE(obj);
}

#define RB_TYPE_P(obj, type) ( \
	((type) == T_FIXNUM) ? FIXNUM_P(obj) : \
	((type) == T_TRUE) ? ((obj) == Qtrue) : \
	((type) == T_FALSE) ? ((obj) == Qfalse) : \
	((type) == T_NIL) ? ((obj) == Qnil) : \
	((type) == T_UNDEF) ? ((obj) == Qundef) : \
	((type) == T_SYMBOL) ? SYMBOL_P(obj) : \
	(!SPECIAL_CONST_P(obj) && BUILTIN_TYPE(obj) == (type)))

#ifdef __GNUC__
#define rb_type_p(obj, type) \
    __extension__ (__builtin_constant_p(type) ? RB_TYPE_P((obj), (type)) : \
		   rb_type(obj) == (type))
#else
#define rb_type_p(obj, type) (rb_type(obj) == (type))
#endif

#ifdef __GNUC__
#define rb_special_const_p(obj) \
    __extension__ ({VALUE special_const_obj = (obj); (int)(SPECIAL_CONST_P(special_const_obj) ? Qtrue : Qfalse);})
#else
static inline int
rb_special_const_p(VALUE obj)
{
    if (SPECIAL_CONST_P(obj)) return (int)Qtrue;
    return (int)Qfalse;
}
#endif

#include "ruby/missing.h"
#include "ruby/intern.h"

#if defined(EXTLIB) && defined(USE_DLN_A_OUT)
/* hook for external modules */
static char *dln_libs_to_be_linked[] = { EXTLIB, 0 };
#endif

#if (defined(__APPLE__) || defined(__NeXT__)) && defined(__MACH__)
#define RUBY_GLOBAL_SETUP /* use linker option to link startup code with ObjC support */
#else
#define RUBY_GLOBAL_SETUP
#endif

void ruby_sysinit(int *, char ***);

#define RUBY_VM 1 /* YARV */
#define HAVE_NATIVETHREAD
int ruby_native_thread_p(void);

#define RUBY_EVENT_NONE      0x0000
#define RUBY_EVENT_LINE      0x0001
#define RUBY_EVENT_CLASS     0x0002
#define RUBY_EVENT_END       0x0004
#define RUBY_EVENT_CALL      0x0008
#define RUBY_EVENT_RETURN    0x0010
#define RUBY_EVENT_C_CALL    0x0020
#define RUBY_EVENT_C_RETURN  0x0040
#define RUBY_EVENT_RAISE     0x0080
#define RUBY_EVENT_ALL       0xffff
#define RUBY_EVENT_VM       0x10000
#define RUBY_EVENT_SWITCH   0x20000
#define RUBY_EVENT_COVERAGE 0x40000

typedef unsigned int rb_event_flag_t;
typedef void (*rb_event_hook_func_t)(rb_event_flag_t evflag, VALUE data, VALUE self, ID mid, VALUE klass);

typedef struct rb_event_hook_struct {
    rb_event_flag_t flag;
    rb_event_hook_func_t func;
    VALUE data;
    struct rb_event_hook_struct *next;
} rb_event_hook_t;

#define RB_EVENT_HOOKS_HAVE_CALLBACK_DATA 1
void rb_add_event_hook(rb_event_hook_func_t func, rb_event_flag_t events,
		       VALUE data);
int rb_remove_event_hook(rb_event_hook_func_t func);

/* locale insensitive functions */

#define rb_isascii(c) ((unsigned long)(c) < 128)
int rb_isalnum(int c);
int rb_isalpha(int c);
int rb_isblank(int c);
int rb_iscntrl(int c);
int rb_isdigit(int c);
int rb_isgraph(int c);
int rb_islower(int c);
int rb_isprint(int c);
int rb_ispunct(int c);
int rb_isspace(int c);
int rb_isupper(int c);
int rb_isxdigit(int c);
int rb_tolower(int c);
int rb_toupper(int c);

#ifndef ISPRINT
#define ISASCII(c) rb_isascii((unsigned char)(c))
#undef ISPRINT
#define ISPRINT(c) rb_isprint((unsigned char)(c))
#define ISSPACE(c) rb_isspace((unsigned char)(c))
#define ISUPPER(c) rb_isupper((unsigned char)(c))
#define ISLOWER(c) rb_islower((unsigned char)(c))
#define ISALNUM(c) rb_isalnum((unsigned char)(c))
#define ISALPHA(c) rb_isalpha((unsigned char)(c))
#define ISDIGIT(c) rb_isdigit((unsigned char)(c))
#define ISXDIGIT(c) rb_isxdigit((unsigned char)(c))
#endif
#define TOUPPER(c) rb_toupper((unsigned char)(c))
#define TOLOWER(c) rb_tolower((unsigned char)(c))

int st_strcasecmp(const char *s1, const char *s2);
int st_strncasecmp(const char *s1, const char *s2, size_t n);
#define STRCASECMP(s1, s2) (st_strcasecmp((s1), (s2)))
#define STRNCASECMP(s1, s2, n) (st_strncasecmp((s1), (s2), (n)))

unsigned long ruby_strtoul(const char *str, char **endptr, int base);
#define STRTOUL(str, endptr, base) (ruby_strtoul((str), (endptr), (base)))

#define InitVM(ext) {void InitVM_##ext(void);InitVM_##ext();}

PRINTF_ARGS(int ruby_snprintf(char *str, size_t n, char const *fmt, ...), 3, 4);
int ruby_vsnprintf(char *str, size_t n, char const *fmt, va_list ap);

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#ifndef RUBY_DONT_SUBST
#include "ruby/subst.h"
#endif

#if defined(__cplusplus)
#if 0
{ /* satisfy cc-mode */
#endif
}  /* extern "C" { */
#endif
#endif /* RUBY_RUBY_H */
