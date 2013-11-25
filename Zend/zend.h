/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
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

#ifndef ZEND_H
#define ZEND_H

#define ZEND_VERSION "2.6.0-dev"

#define ZEND_ENGINE_2

#ifdef __cplusplus
#define BEGIN_EXTERN_C() extern "C" {
#define END_EXTERN_C() }
#else
#define BEGIN_EXTERN_C()
#define END_EXTERN_C()
#endif

/*
 * general definitions
 */

#ifdef ZEND_WIN32
# include "zend_config.w32.h"
# define ZEND_PATHS_SEPARATOR		';'
#elif defined(NETWARE)
# include <zend_config.h>
# define ZEND_PATHS_SEPARATOR		';'
#elif defined(__riscos__)
# include <zend_config.h>
# define ZEND_PATHS_SEPARATOR		';'
#else
# include <zend_config.h>
# define ZEND_PATHS_SEPARATOR		':'
#endif

#ifdef ZEND_WIN32
/* Only use this macro if you know for sure that all of the switches values
   are covered by its case statements */
#define EMPTY_SWITCH_DEFAULT_CASE() \
			default:				\
				__assume(0);		\
				break;
#else
#define EMPTY_SWITCH_DEFAULT_CASE()
#endif

/* all HAVE_XXX test have to be after the include of zend_config above */

#include <stdio.h>

#ifdef HAVE_UNIX_H
# include <unix.h>
#endif

#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif

#ifdef HAVE_DLFCN_H
# include <dlfcn.h>
#endif

#if defined(HAVE_LIBDL) && !defined(ZEND_WIN32)

# ifndef RTLD_LAZY
#  define RTLD_LAZY 1    /* Solaris 1, FreeBSD's (2.1.7.1 and older) */
# endif

# ifndef RTLD_GLOBAL
#  define RTLD_GLOBAL 0
# endif

# if defined(RTLD_GROUP) && defined(RTLD_WORLD) && defined(RTLD_PARENT)
#  define DL_LOAD(libname)			dlopen(libname, RTLD_LAZY | RTLD_GLOBAL | RTLD_GROUP | RTLD_WORLD | RTLD_PARENT)
# elif defined(RTLD_DEEPBIND)
#  define DL_LOAD(libname)			dlopen(libname, RTLD_LAZY | RTLD_GLOBAL | RTLD_DEEPBIND)
# else
#  define DL_LOAD(libname)			dlopen(libname, RTLD_LAZY | RTLD_GLOBAL)
# endif
# define DL_UNLOAD					dlclose
# if defined(DLSYM_NEEDS_UNDERSCORE)
#  define DL_FETCH_SYMBOL(h,s)		dlsym((h), "_" s)
# else
#  define DL_FETCH_SYMBOL			dlsym
# endif
# define DL_ERROR					dlerror
# define DL_HANDLE					void *
# define ZEND_EXTENSIONS_SUPPORT	1
#elif defined(ZEND_WIN32)
# define DL_LOAD(libname)			LoadLibrary(libname)
# define DL_FETCH_SYMBOL			GetProcAddress
# define DL_UNLOAD					FreeLibrary
# define DL_HANDLE					HMODULE
# define ZEND_EXTENSIONS_SUPPORT	1
#else
# define DL_HANDLE					void *
# define ZEND_EXTENSIONS_SUPPORT	0
#endif

#if HAVE_ALLOCA_H && !defined(_ALLOCA_H)
#  include <alloca.h>
#endif

/* AIX requires this to be the first thing in the file.  */
#ifndef __GNUC__
# ifndef HAVE_ALLOCA_H
#  ifdef _AIX
#pragma alloca
#  else
#   ifndef alloca /* predefined by HP cc +Olibcalls */
char *alloca ();
#   endif
#  endif
# endif
#endif

/* Compatibility with non-clang compilers */
#ifndef __has_attribute
# define __has_attribute(x) 0
#endif

/* GCC x.y.z supplies __GNUC__ = x and __GNUC_MINOR__ = y */
#ifdef __GNUC__
# define ZEND_GCC_VERSION (__GNUC__ * 1000 + __GNUC_MINOR__)
#else
# define ZEND_GCC_VERSION 0
#endif

#if ZEND_GCC_VERSION >= 2096
# define ZEND_ATTRIBUTE_MALLOC __attribute__ ((__malloc__))
#else
# define ZEND_ATTRIBUTE_MALLOC
#endif

#if ZEND_GCC_VERSION >= 4003 || __has_attribute(alloc_size)
# define ZEND_ATTRIBUTE_ALLOC_SIZE(X) __attribute__ ((alloc_size(X)))
# define ZEND_ATTRIBUTE_ALLOC_SIZE2(X,Y) __attribute__ ((alloc_size(X,Y)))
#else
# define ZEND_ATTRIBUTE_ALLOC_SIZE(X)
# define ZEND_ATTRIBUTE_ALLOC_SIZE2(X,Y)
#endif

#if ZEND_GCC_VERSION >= 2007
# define ZEND_ATTRIBUTE_FORMAT(type, idx, first) __attribute__ ((format(type, idx, first)))
#else
# define ZEND_ATTRIBUTE_FORMAT(type, idx, first)
#endif

#if ZEND_GCC_VERSION >= 3001 && !defined(__INTEL_COMPILER)
# define ZEND_ATTRIBUTE_PTR_FORMAT(type, idx, first) __attribute__ ((format(type, idx, first)))
#else
# define ZEND_ATTRIBUTE_PTR_FORMAT(type, idx, first)
#endif

#if ZEND_GCC_VERSION >= 3001
# define ZEND_ATTRIBUTE_DEPRECATED  __attribute__((deprecated))
#elif defined(ZEND_WIN32) && defined(_MSC_VER) && _MSC_VER >= 1300
# define ZEND_ATTRIBUTE_DEPRECATED  __declspec(deprecated)
#else
# define ZEND_ATTRIBUTE_DEPRECATED
#endif

#if defined(__GNUC__) && ZEND_GCC_VERSION >= 3004 && defined(__i386__)
# define ZEND_FASTCALL __attribute__((fastcall))
#elif defined(_MSC_VER) && defined(_M_IX86)
# define ZEND_FASTCALL __fastcall
#else
# define ZEND_FASTCALL
#endif

#if defined(__GNUC__) && ZEND_GCC_VERSION >= 3004
#else
# define __restrict__
#endif
#define restrict __restrict__

#if (HAVE_ALLOCA || (defined (__GNUC__) && __GNUC__ >= 2)) && !(defined(ZTS) && defined(NETWARE)) && !(defined(ZTS) && defined(HPUX)) && !defined(DARWIN)
# define ZEND_ALLOCA_MAX_SIZE (32 * 1024)
# define ALLOCA_FLAG(name) \
	zend_bool name;
# define SET_ALLOCA_FLAG(name) \
	name = 1
# define do_alloca_ex(size, limit, use_heap) \
	((use_heap = (UNEXPECTED((size) > (limit)))) ? emalloc(size) : alloca(size))
# define do_alloca(size, use_heap) \
	do_alloca_ex(size, ZEND_ALLOCA_MAX_SIZE, use_heap)
# define free_alloca(p, use_heap) \
	do { if (UNEXPECTED(use_heap)) efree(p); } while (0)
#else
# define ALLOCA_FLAG(name)
# define SET_ALLOCA_FLAG(name)
# define do_alloca(p, use_heap)		emalloc(p)
# define free_alloca(p, use_heap)	efree(p)
#endif

#if ZEND_DEBUG
#define ZEND_FILE_LINE_D				const char *__zend_filename, const uint __zend_lineno
#define ZEND_FILE_LINE_DC				, ZEND_FILE_LINE_D
#define ZEND_FILE_LINE_ORIG_D			const char *__zend_orig_filename, const uint __zend_orig_lineno
#define ZEND_FILE_LINE_ORIG_DC			, ZEND_FILE_LINE_ORIG_D
#define ZEND_FILE_LINE_RELAY_C			__zend_filename, __zend_lineno
#define ZEND_FILE_LINE_RELAY_CC			, ZEND_FILE_LINE_RELAY_C
#define ZEND_FILE_LINE_C				__FILE__, __LINE__
#define ZEND_FILE_LINE_CC				, ZEND_FILE_LINE_C
#define ZEND_FILE_LINE_EMPTY_C			NULL, 0
#define ZEND_FILE_LINE_EMPTY_CC			, ZEND_FILE_LINE_EMPTY_C
#define ZEND_FILE_LINE_ORIG_RELAY_C		__zend_orig_filename, __zend_orig_lineno
#define ZEND_FILE_LINE_ORIG_RELAY_CC	, ZEND_FILE_LINE_ORIG_RELAY_C
#define ZEND_ASSERT(c)					assert(c)
#else
#define ZEND_FILE_LINE_D
#define ZEND_FILE_LINE_DC
#define ZEND_FILE_LINE_ORIG_D
#define ZEND_FILE_LINE_ORIG_DC
#define ZEND_FILE_LINE_RELAY_C
#define ZEND_FILE_LINE_RELAY_CC
#define ZEND_FILE_LINE_C
#define ZEND_FILE_LINE_CC
#define ZEND_FILE_LINE_EMPTY_C
#define ZEND_FILE_LINE_EMPTY_CC
#define ZEND_FILE_LINE_ORIG_RELAY_C
#define ZEND_FILE_LINE_ORIG_RELAY_CC
#define ZEND_ASSERT(c)
#endif	/* ZEND_DEBUG */

#ifdef ZTS
#define ZTS_V 1
#else
#define ZTS_V 0
#endif

#include "zend_errors.h"
#include "zend_alloc.h"

#include "zend_types.h"
#include "zend_string.h"

#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif

#ifndef LONG_MAX
#define LONG_MAX 2147483647L
#endif

#ifndef LONG_MIN
#define LONG_MIN (- LONG_MAX - 1)
#endif

#if SIZEOF_LONG == 4
#define MAX_LENGTH_OF_LONG 11
static const char long_min_digits[] = "2147483648";
#elif SIZEOF_LONG == 8
#define MAX_LENGTH_OF_LONG 20
static const char long_min_digits[] = "9223372036854775808";
#else
#error "Unknown SIZEOF_LONG"
#endif

#define MAX_LENGTH_OF_DOUBLE 32

typedef enum {
  SUCCESS =  0,
  FAILURE = -1,		/* this MUST stay a negative number, or it may affect functions! */
} ZEND_RESULT_CODE;

#include "zend_hash.h"
#include "zend_ts_hash.h"
#include "zend_llist.h"

#define INTERNAL_FUNCTION_PARAMETERS int ht, zval *return_value, zval **return_value_ptr, zval *this_ptr, int return_value_used TSRMLS_DC
#define INTERNAL_FUNCTION_PARAM_PASSTHRU ht, return_value, return_value_ptr, this_ptr, return_value_used TSRMLS_CC

#if defined(__GNUC__) && __GNUC__ >= 3 && !defined(__INTEL_COMPILER) && !defined(DARWIN) && !defined(__hpux) && !defined(_AIX) && !defined(__osf__)
void zend_error_noreturn(int type, const char *format, ...) __attribute__ ((noreturn));
#else
#  define zend_error_noreturn zend_error
#endif

/*
 * zval
 */
typedef struct _zend_class_entry zend_class_entry;

typedef struct _zend_guard {
	zend_bool in_get;
	zend_bool in_set;
	zend_bool in_unset;
	zend_bool in_isset;
	zend_bool dummy; /* sizeof(zend_guard) must not be equal to sizeof(void*) */
} zend_guard;

typedef struct _zend_object {
	zend_class_entry *ce;
	HashTable *properties;
	zval **properties_table;
	HashTable *guards; /* protects from __get/__set ... recursion */
} zend_object;

#include "zend_object_handlers.h"

typedef union _zvalue_value {
	long lval;					/* long value */
	double dval;				/* double value */
	struct {
		char *val;
		int len;
	} str;
	HashTable *ht;				/* hash table value */
	zend_object_value obj;
} zvalue_value;

struct _zval_struct {
	/* Variable information */
	zvalue_value value;		/* value */
	zend_uint refcount__gc;
	zend_uchar type;	/* active type */
	zend_uchar is_ref__gc;
};

#define Z_REFCOUNT_PP(ppz)		Z_REFCOUNT_P(*(ppz))
#define Z_SET_REFCOUNT_PP(ppz, rc)	Z_SET_REFCOUNT_P(*(ppz), rc)
#define Z_ADDREF_PP(ppz)		Z_ADDREF_P(*(ppz))
#define Z_DELREF_PP(ppz)		Z_DELREF_P(*(ppz))
#define Z_ISREF_PP(ppz)			Z_ISREF_P(*(ppz))
#define Z_SET_ISREF_PP(ppz)		Z_SET_ISREF_P(*(ppz))
#define Z_UNSET_ISREF_PP(ppz)		Z_UNSET_ISREF_P(*(ppz))
#define Z_SET_ISREF_TO_PP(ppz, isref)	Z_SET_ISREF_TO_P(*(ppz), isref)

#define Z_REFCOUNT_P(pz)		zval_refcount_p(pz)
#define Z_SET_REFCOUNT_P(pz, rc)	zval_set_refcount_p(pz, rc)
#define Z_ADDREF_P(pz)			zval_addref_p(pz)
#define Z_DELREF_P(pz)			zval_delref_p(pz)
#define Z_ISREF_P(pz)			zval_isref_p(pz)
#define Z_SET_ISREF_P(pz)		zval_set_isref_p(pz)
#define Z_UNSET_ISREF_P(pz)		zval_unset_isref_p(pz)
#define Z_SET_ISREF_TO_P(pz, isref)	zval_set_isref_to_p(pz, isref)

#define Z_REFCOUNT(z)			Z_REFCOUNT_P(&(z))
#define Z_SET_REFCOUNT(z, rc)		Z_SET_REFCOUNT_P(&(z), rc)
#define Z_ADDREF(z)			Z_ADDREF_P(&(z))
#define Z_DELREF(z)			Z_DELREF_P(&(z))
#define Z_ISREF(z)			Z_ISREF_P(&(z))
#define Z_SET_ISREF(z)			Z_SET_ISREF_P(&(z))
#define Z_UNSET_ISREF(z)		Z_UNSET_ISREF_P(&(z))
#define Z_SET_ISREF_TO(z, isref)	Z_SET_ISREF_TO_P(&(z), isref)

#if ZEND_DEBUG
#define zend_always_inline inline
#define zend_never_inline
#else
#if defined(__GNUC__)
#if __GNUC__ >= 3
#define zend_always_inline inline __attribute__((always_inline))
#define zend_never_inline __attribute__((noinline))
#else
#define zend_always_inline inline
#define zend_never_inline
#endif
#elif defined(_MSC_VER)
#define zend_always_inline __forceinline
#define zend_never_inline
#else
#define zend_always_inline inline
#define zend_never_inline
#endif
#endif /* ZEND_DEBUG */

#if (defined (__GNUC__) && __GNUC__ > 2 ) && !defined(DARWIN) && !defined(__hpux) && !defined(_AIX)
# define EXPECTED(condition)   __builtin_expect(condition, 1)
# define UNEXPECTED(condition) __builtin_expect(condition, 0)
#else
# define EXPECTED(condition)   (condition)
# define UNEXPECTED(condition) (condition)
#endif

static zend_always_inline zend_uint zval_refcount_p(zval* pz) {
	return pz->refcount__gc;
}

static zend_always_inline zend_uint zval_set_refcount_p(zval* pz, zend_uint rc) {
	return pz->refcount__gc = rc;
}

static zend_always_inline zend_uint zval_addref_p(zval* pz) {
	return ++pz->refcount__gc;
}

static zend_always_inline zend_uint zval_delref_p(zval* pz) {
	return --pz->refcount__gc;
}

static zend_always_inline zend_bool zval_isref_p(zval* pz) {
	return pz->is_ref__gc;
}

static zend_always_inline zend_bool zval_set_isref_p(zval* pz) {
	return pz->is_ref__gc = 1;
}

static zend_always_inline zend_bool zval_unset_isref_p(zval* pz) {
	return pz->is_ref__gc = 0;
}

static zend_always_inline zend_bool zval_set_isref_to_p(zval* pz, zend_bool isref) {
	return pz->is_ref__gc = isref;
}

/* excpt.h on Digital Unix 4.0 defines function_table */
#undef function_table

/* A lot of stuff needs shifiting around in order to include zend_compile.h here */
union _zend_function;

#include "zend_iterators.h"

struct _zend_serialize_data;
struct _zend_unserialize_data;

typedef struct _zend_serialize_data zend_serialize_data;
typedef struct _zend_unserialize_data zend_unserialize_data;

struct _zend_trait_method_reference {
	const char* method_name;
	unsigned int mname_len;
	
	zend_class_entry *ce;
	
	const char* class_name;
	unsigned int cname_len;
};
typedef struct _zend_trait_method_reference	zend_trait_method_reference;

struct _zend_trait_precedence {
	zend_trait_method_reference *trait_method;
	
	zend_class_entry** exclude_from_classes;
};
typedef struct _zend_trait_precedence zend_trait_precedence;

struct _zend_trait_alias {
	zend_trait_method_reference *trait_method;
	
	/**
	* name for method to be added
	*/
	const char* alias;
	unsigned int alias_len;
	
	/**
	* modifiers to be set on trait method
	*/
	zend_uint modifiers;
};
typedef struct _zend_trait_alias zend_trait_alias;

struct _zend_class_entry {
	char type;
	const char *name;
	zend_uint name_length;
	struct _zend_class_entry *parent;
	int refcount;
	zend_uint ce_flags;

	HashTable function_table;
	HashTable properties_info;
	zval **default_properties_table;
	zval **default_static_members_table;
	zval **static_members_table;
	HashTable constants_table;
	int default_properties_count;
	int default_static_members_count;

	union _zend_function *constructor;
	union _zend_function *destructor;
	union _zend_function *clone;
	union _zend_function *__get;
	union _zend_function *__set;
	union _zend_function *__unset;
	union _zend_function *__isset;
	union _zend_function *__call;
	union _zend_function *__callstatic;
	union _zend_function *__tostring;
	union _zend_function *serialize_func;
	union _zend_function *unserialize_func;

	zend_class_iterator_funcs iterator_funcs;

	/* handlers */
	zend_object_value (*create_object)(zend_class_entry *class_type TSRMLS_DC);
	zend_object_iterator *(*get_iterator)(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC);
	int (*interface_gets_implemented)(zend_class_entry *iface, zend_class_entry *class_type TSRMLS_DC); /* a class implements this interface */
	union _zend_function *(*get_static_method)(zend_class_entry *ce, char* method, int method_len TSRMLS_DC);

	/* serializer callbacks */
	int (*serialize)(zval *object, unsigned char **buffer, zend_uint *buf_len, zend_serialize_data *data TSRMLS_DC);
	int (*unserialize)(zval **object, zend_class_entry *ce, const unsigned char *buf, zend_uint buf_len, zend_unserialize_data *data TSRMLS_DC);

	zend_class_entry **interfaces;
	zend_uint num_interfaces;
	
	zend_class_entry **traits;
	zend_uint num_traits;
	zend_trait_alias **trait_aliases;
	zend_trait_precedence **trait_precedences;

	union {
		struct {
			const char *filename;
			zend_uint line_start;
			zend_uint line_end;
			const char *doc_comment;
			zend_uint doc_comment_len;
		} user;
		struct {
			const struct _zend_function_entry *builtin_functions;
			struct _zend_module_entry *module;
		} internal;
	} info;
};

#include "zend_stream.h"
typedef struct _zend_utility_functions {
	void (*error_function)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args) ZEND_ATTRIBUTE_PTR_FORMAT(printf, 4, 0);
	int (*printf_function)(const char *format, ...) ZEND_ATTRIBUTE_PTR_FORMAT(printf, 1, 2);
	int (*write_function)(const char *str, uint str_length);
	FILE *(*fopen_function)(const char *filename, char **opened_path TSRMLS_DC);
	void (*message_handler)(long message, const void *data TSRMLS_DC);
	void (*block_interruptions)(void);
	void (*unblock_interruptions)(void);
	int (*get_configuration_directive)(const char *name, uint name_length, zval *contents);
	void (*ticks_function)(int ticks);
	void (*on_timeout)(int seconds TSRMLS_DC);
	int (*stream_open_function)(const char *filename, zend_file_handle *handle TSRMLS_DC);
	int (*vspprintf_function)(char **pbuf, size_t max_len, const char *format, va_list ap);
	char *(*getenv_function)(char *name, size_t name_len TSRMLS_DC);
	char *(*resolve_path_function)(const char *filename, int filename_len TSRMLS_DC);
} zend_utility_functions;

typedef struct _zend_utility_values {
	char *import_use_extension;
	uint import_use_extension_length;
	zend_bool html_errors;
} zend_utility_values;

typedef int (*zend_write_func_t)(const char *str, uint str_length);

#undef MIN
#undef MAX
#define MAX(a, b)  (((a)>(b))?(a):(b))
#define MIN(a, b)  (((a)<(b))?(a):(b))
#define ZEND_STRL(str)		(str), (sizeof(str)-1)
#define ZEND_STRS(str)		(str), (sizeof(str))
#define ZEND_NORMALIZE_BOOL(n)			\
	((n) ? (((n)>0) ? 1 : -1) : 0)
#define ZEND_TRUTH(x)		((x) ? 1 : 0)
#define ZEND_LOG_XOR(a, b)		(ZEND_TRUTH(a) ^ ZEND_TRUTH(b))

/* data types */
/* All data types <= IS_BOOL have their constructor/destructors skipped */
#define IS_NULL		0
#define IS_LONG		1
#define IS_DOUBLE	2
#define IS_BOOL		3
#define IS_ARRAY	4
#define IS_OBJECT	5
#define IS_STRING	6
#define IS_RESOURCE	7
#define IS_CONSTANT	8
#define IS_CONSTANT_ARRAY	9
#define IS_CALLABLE	10

/* Ugly hack to support constants as static array indices */
#define IS_CONSTANT_TYPE_MASK		0x00f
#define IS_CONSTANT_UNQUALIFIED		0x010
#define IS_CONSTANT_INDEX			0x080
#define IS_LEXICAL_VAR				0x020
#define IS_LEXICAL_REF				0x040
#define IS_CONSTANT_IN_NAMESPACE	0x100

/* overloaded elements data types */
#define OE_IS_ARRAY		(1<<0)
#define OE_IS_OBJECT	(1<<1)
#define OE_IS_METHOD	(1<<2)

int zend_startup(zend_utility_functions *utility_functions, char **extensions TSRMLS_DC);
void zend_shutdown(TSRMLS_D);
void zend_register_standard_ini_entries(TSRMLS_D);
void zend_post_startup(TSRMLS_D);
void zend_set_utility_values(zend_utility_values *utility_values);

BEGIN_EXTERN_C()
ZEND_API void _zend_bailout(char *filename, uint lineno);
END_EXTERN_C()

#define zend_bailout()		_zend_bailout(__FILE__, __LINE__)

#ifdef HAVE_SIGSETJMP
#	define SETJMP(a) sigsetjmp(a, 0)
#	define LONGJMP(a,b) siglongjmp(a, b)
#	define JMP_BUF sigjmp_buf
#else
#	define SETJMP(a) setjmp(a)
#	define LONGJMP(a,b) longjmp(a, b)
#	define JMP_BUF jmp_buf
#endif

#define zend_try												\
	{															\
		JMP_BUF *__orig_bailout = EG(bailout);					\
		JMP_BUF __bailout;										\
																\
		EG(bailout) = &__bailout;								\
		if (SETJMP(__bailout)==0) {
#define zend_catch												\
		} else {												\
			EG(bailout) = __orig_bailout;
#define zend_end_try()											\
		}														\
		EG(bailout) = __orig_bailout;							\
	}
#define zend_first_try		EG(bailout)=NULL;	zend_try

BEGIN_EXTERN_C()
ZEND_API char *get_zend_version(void);
ZEND_API void zend_make_printable_zval(zval *expr, zval *expr_copy, int *use_copy);
ZEND_API int zend_print_zval(zval *expr, int indent);
ZEND_API int zend_print_zval_ex(zend_write_func_t write_func, zval *expr, int indent);
ZEND_API void zend_print_zval_r(zval *expr, int indent TSRMLS_DC);
ZEND_API void zend_print_flat_zval_r(zval *expr TSRMLS_DC);
ZEND_API void zend_print_zval_r_ex(zend_write_func_t write_func, zval *expr, int indent TSRMLS_DC);
ZEND_API void zend_output_debug_string(zend_bool trigger_break, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);
END_EXTERN_C()

BEGIN_EXTERN_C()
ZEND_API void zend_activate(TSRMLS_D);
ZEND_API void zend_deactivate(TSRMLS_D);
ZEND_API void zend_call_destructors(TSRMLS_D);
ZEND_API void zend_activate_modules(TSRMLS_D);
ZEND_API void zend_deactivate_modules(TSRMLS_D);
ZEND_API void zend_post_deactivate_modules(TSRMLS_D);
END_EXTERN_C()

#if ZEND_DEBUG
#define Z_DBG(expr)		(expr)
#else
#define	Z_DBG(expr)
#endif

BEGIN_EXTERN_C()
ZEND_API void free_estring(char **str_p);
END_EXTERN_C()

/* FIXME: Check if we can save if (ptr) too */

#define STR_FREE(ptr) if (ptr) { str_efree(ptr); }
#define STR_FREE_REL(ptr) if (ptr) { str_efree_rel(ptr); }

#define STR_EMPTY_ALLOC() estrndup("", sizeof("")-1)

#define STR_REALLOC(ptr, size) \
			ptr = (char *) erealloc(ptr, size);

/* output support */
#define ZEND_WRITE(str, str_len)		zend_write((str), (str_len))
#define ZEND_WRITE_EX(str, str_len)		write_func((str), (str_len))
#define ZEND_PUTS(str)					zend_write((str), strlen((str)))
#define ZEND_PUTS_EX(str)				write_func((str), strlen((str)))
#define ZEND_PUTC(c)					zend_write(&(c), 1), (c)

BEGIN_EXTERN_C()
extern ZEND_API int (*zend_printf)(const char *format, ...) ZEND_ATTRIBUTE_PTR_FORMAT(printf, 1, 2);
extern ZEND_API zend_write_func_t zend_write;
extern ZEND_API FILE *(*zend_fopen)(const char *filename, char **opened_path TSRMLS_DC);
extern ZEND_API void (*zend_block_interruptions)(void);
extern ZEND_API void (*zend_unblock_interruptions)(void);
extern ZEND_API void (*zend_ticks_function)(int ticks);
extern ZEND_API void (*zend_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args) ZEND_ATTRIBUTE_PTR_FORMAT(printf, 4, 0);
extern ZEND_API void (*zend_on_timeout)(int seconds TSRMLS_DC);
extern ZEND_API int (*zend_stream_open_function)(const char *filename, zend_file_handle *handle TSRMLS_DC);
extern int (*zend_vspprintf)(char **pbuf, size_t max_len, const char *format, va_list ap);
extern ZEND_API char *(*zend_getenv)(char *name, size_t name_len TSRMLS_DC);
extern ZEND_API char *(*zend_resolve_path)(const char *filename, int filename_len TSRMLS_DC);

ZEND_API void zend_error(int type, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);

void zenderror(const char *error);

/* The following #define is used for code duality in PHP for Engine 1 & 2 */
#define ZEND_STANDARD_CLASS_DEF_PTR zend_standard_class_def
extern ZEND_API zend_class_entry *zend_standard_class_def;
extern ZEND_API zend_utility_values zend_uv;
extern ZEND_API zval zval_used_for_init;

END_EXTERN_C()

#define ZEND_UV(name) (zend_uv.name)

#ifndef ZEND_SIGNALS
#define HANDLE_BLOCK_INTERRUPTIONS()		if (zend_block_interruptions) { zend_block_interruptions(); }
#define HANDLE_UNBLOCK_INTERRUPTIONS()		if (zend_unblock_interruptions) { zend_unblock_interruptions(); }
#else
#include "zend_signal.h"

#define HANDLE_BLOCK_INTERRUPTIONS()		ZEND_SIGNAL_BLOCK_INTERRUPUTIONS()
#define HANDLE_UNBLOCK_INTERRUPTIONS()		ZEND_SIGNAL_UNBLOCK_INTERRUPTIONS()
#endif

BEGIN_EXTERN_C()
ZEND_API void zend_message_dispatcher(long message, const void *data TSRMLS_DC);

ZEND_API int zend_get_configuration_directive(const char *name, uint name_length, zval *contents);
END_EXTERN_C()

/* Messages for applications of Zend */
#define ZMSG_FAILED_INCLUDE_FOPEN		1L
#define ZMSG_FAILED_REQUIRE_FOPEN		2L
#define ZMSG_FAILED_HIGHLIGHT_FOPEN		3L
#define ZMSG_MEMORY_LEAK_DETECTED		4L
#define ZMSG_MEMORY_LEAK_REPEATED		5L
#define ZMSG_LOG_SCRIPT_NAME			6L
#define ZMSG_MEMORY_LEAKS_GRAND_TOTAL	7L

#define INIT_PZVAL(z)		\
	(z)->refcount__gc = 1;	\
	(z)->is_ref__gc = 0;

#define INIT_ZVAL(z) z = zval_used_for_init;

#define ALLOC_INIT_ZVAL(zp)						\
	ALLOC_ZVAL(zp);		\
	INIT_ZVAL(*zp);

#define MAKE_STD_ZVAL(zv)				 \
	ALLOC_ZVAL(zv); \
	INIT_PZVAL(zv);

#define PZVAL_IS_REF(z)		Z_ISREF_P(z)

#define ZVAL_COPY_VALUE(z, v)					\
	do {										\
		(z)->value = (v)->value;				\
		Z_TYPE_P(z) = Z_TYPE_P(v);				\
	} while (0)

#define INIT_PZVAL_COPY(z, v)					\
	do {										\
		ZVAL_COPY_VALUE(z, v);					\
		Z_SET_REFCOUNT_P(z, 1);					\
		Z_UNSET_ISREF_P(z);						\
	} while (0)

#define SEPARATE_ZVAL(ppzv)						\
	do {										\
		if (Z_REFCOUNT_PP((ppzv)) > 1) {		\
			zval *new_zv;						\
			Z_DELREF_PP(ppzv);					\
			ALLOC_ZVAL(new_zv);					\
			INIT_PZVAL_COPY(new_zv, *(ppzv));	\
			*(ppzv) = new_zv;					\
			zval_copy_ctor(new_zv);				\
		}										\
	} while (0)

#define SEPARATE_ZVAL_IF_NOT_REF(ppzv)		\
	if (!PZVAL_IS_REF(*ppzv)) {				\
		SEPARATE_ZVAL(ppzv);				\
	}

#define SEPARATE_ZVAL_TO_MAKE_IS_REF(ppzv)	\
	if (!PZVAL_IS_REF(*ppzv)) {				\
		SEPARATE_ZVAL(ppzv);				\
		Z_SET_ISREF_PP((ppzv));				\
	}

#define COPY_PZVAL_TO_ZVAL(zv, pzv)			\
	(zv) = *(pzv);							\
	if (Z_REFCOUNT_P(pzv)>1) {				\
		zval_copy_ctor(&(zv));				\
		Z_DELREF_P((pzv));					\
	} else {								\
		FREE_ZVAL(pzv);						\
	}										\
	INIT_PZVAL(&(zv));
	
#define MAKE_COPY_ZVAL(ppzv, pzv) 	\
	INIT_PZVAL_COPY(pzv, *(ppzv));	\
	zval_copy_ctor((pzv));

#define REPLACE_ZVAL_VALUE(ppzv_dest, pzv_src, copy) {	\
	int is_ref, refcount;						\
												\
	SEPARATE_ZVAL_IF_NOT_REF(ppzv_dest);		\
	is_ref = Z_ISREF_PP(ppzv_dest);				\
	refcount = Z_REFCOUNT_PP(ppzv_dest);		\
	zval_dtor(*ppzv_dest);						\
	ZVAL_COPY_VALUE(*ppzv_dest, pzv_src);		\
	if (copy) {                                 \
		zval_copy_ctor(*ppzv_dest);				\
    }		                                    \
	Z_SET_ISREF_TO_PP(ppzv_dest, is_ref);		\
	Z_SET_REFCOUNT_PP(ppzv_dest, refcount);		\
}

#define SEPARATE_ARG_IF_REF(varptr) \
	if (PZVAL_IS_REF(varptr)) { \
		zval *original_var = varptr; \
		ALLOC_ZVAL(varptr); \
		INIT_PZVAL_COPY(varptr, original_var); \
		zval_copy_ctor(varptr); \
	} else { \
		Z_ADDREF_P(varptr); \
	}

#define READY_TO_DESTROY(zv) \
	(Z_REFCOUNT_P(zv) == 1 && \
	 (Z_TYPE_P(zv) != IS_OBJECT || \
	  zend_objects_store_get_refcount(zv TSRMLS_CC) == 1))

#define ZEND_MAX_RESERVED_RESOURCES	4

#include "zend_gc.h"
#include "zend_operators.h"
#include "zend_variables.h"

typedef enum {
	EH_NORMAL = 0,
	EH_SUPPRESS,
	EH_THROW
} zend_error_handling_t;

typedef struct {
	zend_error_handling_t  handling;
	zend_class_entry       *exception;
	zval                   *user_handler;
} zend_error_handling;

ZEND_API void zend_save_error_handling(zend_error_handling *current TSRMLS_DC);
ZEND_API void zend_replace_error_handling(zend_error_handling_t error_handling, zend_class_entry *exception_class, zend_error_handling *current TSRMLS_DC);
ZEND_API void zend_restore_error_handling(zend_error_handling *saved TSRMLS_DC);

#define DEBUG_BACKTRACE_PROVIDE_OBJECT (1<<0)
#define DEBUG_BACKTRACE_IGNORE_ARGS    (1<<1)

#endif /* ZEND_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
