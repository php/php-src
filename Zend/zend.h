/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2009 Zend Technologies Ltd. (http://www.zend.com) |
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

#define ZEND_VERSION "2.2.0"

#define ZEND_ENGINE_2

#ifdef __cplusplus
#define BEGIN_EXTERN_C() extern "C" {
#define END_EXTERN_C() }
#else
#define BEGIN_EXTERN_C()
#define END_EXTERN_C()
#endif

#include <stdio.h>

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

#ifdef HAVE_UNIX_H
# include <unix.h>
#endif

#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif

#ifdef HAVE_DLFCN_H
# include <dlfcn.h>
#endif

#if HAVE_MACH_O_DYLD_H
#include <mach-o/dyld.h>

/* MH_BUNDLE loading functions for Mac OS X / Darwin */
void *zend_mh_bundle_load (char* bundle_path);
int zend_mh_bundle_unload (void *bundle_handle);
void *zend_mh_bundle_symbol(void *bundle_handle, const char *symbol_name);
const char *zend_mh_bundle_error(void);

#endif /* HAVE_MACH_O_DYLD_H */

#if defined(HAVE_LIBDL) && !defined(HAVE_MACH_O_DYLD_H) && !defined(ZEND_WIN32)

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
#elif defined(HAVE_MACH_O_DYLD_H)
# define DL_LOAD(libname)			zend_mh_bundle_load(libname)
# define DL_UNLOAD			zend_mh_bundle_unload
# define DL_FETCH_SYMBOL(h,s)		zend_mh_bundle_symbol(h,s)
# define DL_ERROR					zend_mh_bundle_error
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


#if (HAVE_ALLOCA || (defined (__GNUC__) && __GNUC__ >= 2)) && !(defined(ZTS) && defined(ZEND_WIN32)) && !(defined(ZTS) && defined(NETWARE)) && !(defined(ZTS) && defined(HPUX)) && !defined(DARWIN)
# define do_alloca(p) alloca(p)
# define free_alloca(p)
# define ZEND_ALLOCA_MAX_SIZE (32 * 1024)
# define ALLOCA_FLAG(name) \
	zend_bool name;
# define do_alloca_with_limit_ex(size, limit, use_heap) \
	((use_heap = ((size) > (limit))) ? emalloc(size) : alloca(size))
# define do_alloca_with_limit(size, use_heap) \
	do_alloca_with_limit_ex(size, ZEND_ALLOCA_MAX_SIZE, use_heap)
# define free_alloca_with_limit(p, use_heap) \
	do { if (use_heap) efree(p); } while (0)
#else
# define do_alloca(p)		emalloc(p)
# define free_alloca(p)	efree(p)
# define ALLOCA_FLAG(name)
# define do_alloca_with_limit(p, use_heap)		emalloc(p)
# define free_alloca_with_limit(p, use_heap)	efree(p)
#endif

#if ZEND_DEBUG
#define ZEND_FILE_LINE_D				char *__zend_filename, uint __zend_lineno
#define ZEND_FILE_LINE_DC				, ZEND_FILE_LINE_D
#define ZEND_FILE_LINE_ORIG_D			char *__zend_orig_filename, uint __zend_orig_lineno
#define ZEND_FILE_LINE_ORIG_DC			, ZEND_FILE_LINE_ORIG_D
#define ZEND_FILE_LINE_RELAY_C			__zend_filename, __zend_lineno
#define ZEND_FILE_LINE_RELAY_CC			, ZEND_FILE_LINE_RELAY_C
#define ZEND_FILE_LINE_C				__FILE__, __LINE__
#define ZEND_FILE_LINE_CC				, ZEND_FILE_LINE_C
#define ZEND_FILE_LINE_EMPTY_C			NULL, 0
#define ZEND_FILE_LINE_EMPTY_CC			, ZEND_FILE_LINE_EMPTY_C
#define ZEND_FILE_LINE_ORIG_RELAY_C		__zend_orig_filename, __zend_orig_lineno
#define ZEND_FILE_LINE_ORIG_RELAY_CC	, ZEND_FILE_LINE_ORIG_RELAY_C
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
#endif	/* ZEND_DEBUG */

#ifdef ZTS
#define ZTS_V 1
#else
#define ZTS_V 0
#endif

#include "zend_errors.h"
#include "zend_alloc.h"

#include "zend_types.h"

#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif

#ifndef LONG_MAX
#define LONG_MAX 2147483647L
#endif

#ifndef LONG_MIN
#define LONG_MIN (- LONG_MAX - 1)
#endif

#undef SUCCESS
#undef FAILURE
#define SUCCESS 0
#define FAILURE -1				/* this MUST stay a negative number, or it may affect functions! */


#include "zend_hash.h"
#include "zend_ts_hash.h"
#include "zend_llist.h"

#define INTERNAL_FUNCTION_PARAMETERS int ht, zval *return_value, zval **return_value_ptr, zval *this_ptr, int return_value_used TSRMLS_DC
#define INTERNAL_FUNCTION_PARAM_PASSTHRU ht, return_value, return_value_ptr, this_ptr, return_value_used TSRMLS_CC

#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(DARWIN) && !defined(__hpux) && !defined(_AIX) && !defined(__osf__)
#  define ZEND_VM_ALWAYS_INLINE  __attribute__ ((always_inline))
void zend_error_noreturn(int type, const char *format, ...) __attribute__ ((noreturn));
#else
#  define ZEND_VM_ALWAYS_INLINE
#  define zend_error_noreturn zend_error
#endif

/*
 * zval
 */
typedef struct _zval_struct zval;
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
	zend_uint refcount;
	zend_uchar type;	/* active type */
	zend_uchar is_ref;
};


/* excpt.h on Digital Unix 4.0 defines function_table */
#undef function_table

/* A lot of stuff needs shifiting around in order to include zend_compile.h here */
union _zend_function;

#include "zend_iterators.h"

struct _zend_serialize_data;
struct _zend_unserialize_data;

typedef struct _zend_serialize_data zend_serialize_data;
typedef struct _zend_unserialize_data zend_unserialize_data;

struct _zend_class_entry {
	char type;
	char *name;
	zend_uint name_length;
	struct _zend_class_entry *parent;
	int refcount;
	zend_bool constants_updated;
	zend_uint ce_flags;

	HashTable function_table;
	HashTable default_properties;
	HashTable properties_info;
	HashTable default_static_members;
	HashTable *static_members;
	HashTable constants_table;
	struct _zend_function_entry *builtin_functions;

	union _zend_function *constructor;
	union _zend_function *destructor;
	union _zend_function *clone;
	union _zend_function *__get;
	union _zend_function *__set;
	union _zend_function *__unset;
	union _zend_function *__isset;
	union _zend_function *__call;
	union _zend_function *__tostring;
	union _zend_function *serialize_func;
	union _zend_function *unserialize_func;

	zend_class_iterator_funcs iterator_funcs;

	/* handlers */
	zend_object_value (*create_object)(zend_class_entry *class_type TSRMLS_DC);
	zend_object_iterator *(*get_iterator)(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC);
	int (*interface_gets_implemented)(zend_class_entry *iface, zend_class_entry *class_type TSRMLS_DC); /* a class implements this interface */

	/* serializer callbacks */
	int (*serialize)(zval *object, unsigned char **buffer, zend_uint *buf_len, zend_serialize_data *data TSRMLS_DC);
	int (*unserialize)(zval **object, zend_class_entry *ce, const unsigned char *buf, zend_uint buf_len, zend_unserialize_data *data TSRMLS_DC);

	zend_class_entry **interfaces;
	zend_uint num_interfaces;

	char *filename;
	zend_uint line_start;
	zend_uint line_end;
	char *doc_comment;
	zend_uint doc_comment_len;

	struct _zend_module_entry *module;
};

#include "zend_stream.h"
typedef struct _zend_utility_functions {
	void (*error_function)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args) ZEND_ATTRIBUTE_PTR_FORMAT(printf, 4, 0);
	int (*printf_function)(const char *format, ...) ZEND_ATTRIBUTE_PTR_FORMAT(printf, 1, 2);
	int (*write_function)(const char *str, uint str_length);
	FILE *(*fopen_function)(const char *filename, char **opened_path);
	void (*message_handler)(long message, void *data);
	void (*block_interruptions)(void);
	void (*unblock_interruptions)(void);
	int (*get_configuration_directive)(char *name, uint name_length, zval *contents);
	void (*ticks_function)(int ticks);
	void (*on_timeout)(int seconds TSRMLS_DC);
	int (*stream_open_function)(const char *filename, zend_file_handle *handle TSRMLS_DC);
	int (*vspprintf_function)(char **pbuf, size_t max_len, const char *format, va_list ap);
	char *(*getenv_function)(char *name, size_t name_len TSRMLS_DC);
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

/* Ugly hack to support constants as static array indices */
#define IS_CONSTANT_INDEX	0x80


/* overloaded elements data types */
#define OE_IS_ARRAY	(1<<0)
#define OE_IS_OBJECT	(1<<1)
#define OE_IS_METHOD	(1<<2)

int zend_startup(zend_utility_functions *utility_functions, char **extensions, int start_builtin_functions);
void zend_shutdown(TSRMLS_D);
void zend_register_standard_ini_entries(TSRMLS_D);

#ifdef ZTS
void zend_post_startup(TSRMLS_D);
#endif

void zend_set_utility_values(zend_utility_values *utility_values);

BEGIN_EXTERN_C()
ZEND_API void _zend_bailout(char *filename, uint lineno);
END_EXTERN_C()

#define zend_bailout()		_zend_bailout(__FILE__, __LINE__)

#define zend_try												\
	{															\
		jmp_buf *__orig_bailout = EG(bailout);					\
		jmp_buf __bailout;										\
																\
		EG(bailout) = &__bailout;								\
		if (setjmp(__bailout)==0) {
#define zend_catch												\
		} else {												\
			EG(bailout) = __orig_bailout;
#define zend_end_try()											\
		}														\
		EG(bailout) = __orig_bailout;							\
	}
#define zend_first_try		EG(bailout)=NULL; zend_try

BEGIN_EXTERN_C()
ZEND_API char *get_zend_version(void);
ZEND_API void zend_make_printable_zval(zval *expr, zval *expr_copy, int *use_copy);
ZEND_API int zend_print_zval(zval *expr, int indent);
ZEND_API int zend_print_zval_ex(zend_write_func_t write_func, zval *expr, int indent);
ZEND_API void zend_print_zval_r(zval *expr, int indent TSRMLS_DC);
ZEND_API void zend_print_flat_zval_r(zval *expr TSRMLS_DC);
ZEND_API void zend_print_zval_r_ex(zend_write_func_t write_func, zval *expr, int indent TSRMLS_DC);
ZEND_API void zend_output_debug_string(zend_bool trigger_break, char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);
END_EXTERN_C()

void zend_activate(TSRMLS_D);
void zend_deactivate(TSRMLS_D);
void zend_call_destructors(TSRMLS_D);
void zend_activate_modules(TSRMLS_D);
void zend_deactivate_modules(TSRMLS_D);
void zend_post_deactivate_modules(TSRMLS_D);

#if ZEND_DEBUG
#define Z_DBG(expr)		(expr)
#else
#define	Z_DBG(expr)
#endif

BEGIN_EXTERN_C()
ZEND_API void free_estring(char **str_p);
END_EXTERN_C()

/* FIXME: Check if we can save if (ptr) too */

#define STR_FREE(ptr) if (ptr) { efree(ptr); }
#define STR_FREE_REL(ptr) if (ptr) { efree_rel(ptr); }

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
extern ZEND_API FILE *(*zend_fopen)(const char *filename, char **opened_path);
extern ZEND_API void (*zend_block_interruptions)(void);
extern ZEND_API void (*zend_unblock_interruptions)(void);
extern ZEND_API void (*zend_ticks_function)(int ticks);
extern ZEND_API void (*zend_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args) ZEND_ATTRIBUTE_PTR_FORMAT(printf, 4, 0);
extern void (*zend_on_timeout)(int seconds TSRMLS_DC);
extern ZEND_API int (*zend_stream_open_function)(const char *filename, zend_file_handle *handle TSRMLS_DC);
extern int (*zend_vspprintf)(char **pbuf, size_t max_len, const char *format, va_list ap);
extern ZEND_API char *(*zend_getenv)(char *name, size_t name_len TSRMLS_DC);


ZEND_API void zend_error(int type, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);

void zenderror(char *error);

/* The following #define is used for code duality in PHP for Engine 1 & 2 */
#define ZEND_STANDARD_CLASS_DEF_PTR zend_standard_class_def
extern ZEND_API zend_class_entry *zend_standard_class_def;
extern ZEND_API zend_utility_values zend_uv;
extern ZEND_API zval zval_used_for_init;

END_EXTERN_C()

#define ZEND_UV(name) (zend_uv.name)


#define HANDLE_BLOCK_INTERRUPTIONS()		if (zend_block_interruptions) { zend_block_interruptions(); }
#define HANDLE_UNBLOCK_INTERRUPTIONS()		if (zend_unblock_interruptions) { zend_unblock_interruptions(); }

BEGIN_EXTERN_C()
ZEND_API void zend_message_dispatcher(long message, void *data);

ZEND_API int zend_get_configuration_directive(char *name, uint name_length, zval *contents);
END_EXTERN_C()


/* Messages for applications of Zend */
#define ZMSG_FAILED_INCLUDE_FOPEN		1L
#define ZMSG_FAILED_REQUIRE_FOPEN		2L
#define ZMSG_FAILED_HIGHLIGHT_FOPEN		3L
#define ZMSG_MEMORY_LEAK_DETECTED		4L
#define ZMSG_MEMORY_LEAK_REPEATED		5L
#define ZMSG_LOG_SCRIPT_NAME			6L
#define ZMSG_MEMORY_LEAKS_GRAND_TOTAL	7L


#define ZVAL_ADDREF(pz)		(++(pz)->refcount)
#define ZVAL_DELREF(pz)		(--(pz)->refcount)
#define ZVAL_REFCOUNT(pz)	((pz)->refcount)

#define INIT_PZVAL(z)		\
	(z)->refcount = 1;		\
	(z)->is_ref = 0;

#define INIT_ZVAL(z) z = zval_used_for_init;

#define ALLOC_INIT_ZVAL(zp)						\
	ALLOC_ZVAL(zp);		\
	INIT_ZVAL(*zp);

#define MAKE_STD_ZVAL(zv)				 \
	ALLOC_ZVAL(zv); \
	INIT_PZVAL(zv);

#define PZVAL_IS_REF(z)		((z)->is_ref)

#define SEPARATE_ZVAL(ppzv)									\
	{														\
		zval *orig_ptr = *(ppzv);							\
															\
		if (orig_ptr->refcount>1) {							\
			orig_ptr->refcount--;							\
			ALLOC_ZVAL(*(ppzv));							\
			**(ppzv) = *orig_ptr;							\
			zval_copy_ctor(*(ppzv));						\
			(*(ppzv))->refcount=1;							\
			(*(ppzv))->is_ref = 0;							\
		}													\
	}

#define SEPARATE_ZVAL_IF_NOT_REF(ppzv)		\
	if (!PZVAL_IS_REF(*ppzv)) {				\
		SEPARATE_ZVAL(ppzv);				\
	}

#define SEPARATE_ZVAL_TO_MAKE_IS_REF(ppzv)	\
	if (!PZVAL_IS_REF(*ppzv)) {				\
		SEPARATE_ZVAL(ppzv);				\
		(*(ppzv))->is_ref = 1;				\
	}

#define COPY_PZVAL_TO_ZVAL(zv, pzv)			\
	(zv) = *(pzv);							\
	if ((pzv)->refcount>1) {				\
		zval_copy_ctor(&(zv));				\
		(pzv)->refcount--;					\
	} else {								\
		FREE_ZVAL(pzv);						\
	}										\
	INIT_PZVAL(&(zv));

#define REPLACE_ZVAL_VALUE(ppzv_dest, pzv_src, copy) {	\
	int is_ref, refcount;						\
												\
	SEPARATE_ZVAL_IF_NOT_REF(ppzv_dest);		\
	is_ref = (*ppzv_dest)->is_ref;				\
	refcount = (*ppzv_dest)->refcount;			\
	zval_dtor(*ppzv_dest);						\
	**ppzv_dest = *pzv_src;						\
	if (copy) {                                 \
		zval_copy_ctor(*ppzv_dest);				\
    }		                                    \
	(*ppzv_dest)->is_ref = is_ref;				\
	(*ppzv_dest)->refcount = refcount;			\
}

#define SEPARATE_ARG_IF_REF(varptr) \
	if (PZVAL_IS_REF(varptr)) { \
		zval *original_var = varptr; \
		ALLOC_ZVAL(varptr); \
		varptr->value = original_var->value; \
		varptr->type = original_var->type; \
		varptr->is_ref = 0; \
		varptr->refcount = 1; \
		zval_copy_ctor(varptr); \
	} else { \
		varptr->refcount++; \
	}

#define READY_TO_DESTROY(zv) \
	((zv)->refcount == 1 && \
	 (Z_TYPE_P(zv) != IS_OBJECT || \
	  zend_objects_store_get_refcount(zv TSRMLS_CC) == 1))


#define ZEND_MAX_RESERVED_RESOURCES	4

#include "zend_operators.h"
#include "zend_variables.h"

#endif /* ZEND_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
