/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2002 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
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

#define ZEND_VERSION "2.0.0-alpha2"

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
# include "zend_config.nw.h"
# include "acconfig.h"
#elif defined(__riscos__)
# include "zend_config.h"
# define ZEND_PATHS_SEPARATOR		';'
#else
# include "zend_config.h"
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

#if defined(HAVE_LIBDL)

# ifndef RTLD_LAZY
#  define RTLD_LAZY 1    /* Solaris 1, FreeBSD's (2.1.7.1 and older) */
# endif

# ifndef RTLD_GLOBAL
#  define RTLD_GLOBAL 0
# endif

# define DL_LOAD(libname)			dlopen(libname, RTLD_LAZY | RTLD_GLOBAL)
# define DL_UNLOAD					dlclose
# if defined(DLSYM_NEEDS_UNDERSCORE)
#  define DL_FETCH_SYMBOL(h,s)		dlsym((h), "_" ## s)
# else
#  define DL_FETCH_SYMBOL			dlsym
# endif
# define DL_ERROR					dlerror
# define DL_HANDLE					void *
# define ZEND_EXTENSIONS_SUPPORT	1
#elif defined(HAVE_MACH_O_DYLD_H)
# define DL_LOAD(libname)			zend_mh_bundle_load(libname)
# define DL_UNLOAD(handle)			zend_mh_bundle_unload(handle)
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

/* AIX requires this to be the first thing in the file.  */
#ifndef __GNUC__
# if HAVE_ALLOCA_H
#  include <alloca.h>
# else
#  ifdef _AIX
 #pragma alloca
#  else
#   ifndef alloca /* predefined by HP cc +Olibcalls */
char *alloca ();
#   endif
#  endif
# endif
#endif

#if (HAVE_ALLOCA || (defined (__GNUC__) && __GNUC__ >= 2)) && !(defined(ZTS) && defined(ZEND_WIN32)) && !(defined(ZTS) && defined(NETWARE))
# define do_alloca(p) alloca(p)
# define free_alloca(p)
#else
# define do_alloca(p)		emalloc(p)
# define free_alloca(p)	efree(p)
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

#define INTERNAL_FUNCTION_PARAMETERS int ht, zval *return_value, zval *this_ptr, int return_value_used TSRMLS_DC
#define INTERNAL_FUNCTION_PARAM_PASSTHRU ht, return_value, this_ptr, return_value_used TSRMLS_CC


/*
 * zval
 */
typedef struct _zval_struct zval;
typedef struct _zend_class_entry zend_class_entry;

typedef struct _zend_object {
	zend_class_entry *ce;
	HashTable *properties;
} zend_object;

typedef unsigned int zend_object_handle;
typedef	struct _zend_object_value zend_object_value;

#include "zend_object_handlers.h"

struct _zend_object_value {
	zend_object_handle handle;
	zend_object_handlers *handlers;
};

typedef union _zvalue_value {
	long lval;					/* long value */
	double dval;				/* double value */
	struct {
		char *val;
		zend_uint len;
	} str;
	HashTable *ht;				/* hash table value */
/*	struct {
		zend_class_entry *ce;
		HashTable *properties;
	} obj;
*/
	zend_object_value obj;
} zvalue_value;


struct _zval_struct {
	/* Variable information */
	zvalue_value value;		/* value */
	zend_uint refcount;
	zend_uchar type;	/* active type */
	zend_uchar is_ref;
};



typedef struct _zend_function_entry {
	char *fname;
	void (*handler)(INTERNAL_FUNCTION_PARAMETERS);
	unsigned char *func_arg_types;
} zend_function_entry;


typedef struct _zend_property_reference {
	int type;  /* read, write or r/w */
	zval *object;
	zend_llist *elements_list;
} zend_property_reference;



typedef struct _zend_overloaded_element {
	int type;		/* array offset or object proprety */
	zval element;
} zend_overloaded_element;

/* excpt.h on Digital Unix 4.0 defines function_table */
#undef function_table

/* A lot of stuff needs shifiting around in order to include zend_compile.h here */
union _zend_function;

struct _zend_class_entry {
	char type;
	char *name;
	zend_uint name_length;
	struct _zend_class_entry *parent; 
	int refcount;
	zend_bool constants_updated;

	HashTable function_table;
	HashTable default_properties;
	HashTable private_properties; /* This is only needed at compile-time */
	HashTable protected_properties; /* This is only needed at compile-time */
	HashTable class_table;
	HashTable *static_members;
	HashTable constants_table;
	zend_function_entry *builtin_functions;

	union _zend_function *constructor;
	union _zend_function *destructor;
	union _zend_function *clone;

	/* handlers */
	zend_object_value (*create_object)(zend_class_entry *class_type TSRMLS_DC);
	void (*handle_function_call)(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference);
	zval (*handle_property_get)(zend_property_reference *property_reference);
	int (*handle_property_set)(zend_property_reference *property_reference, zval *value);
};


typedef struct _zend_utility_functions {
	void (*error_function)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
	int (*printf_function)(const char *format, ...);
	int (*write_function)(const char *str, uint str_length);
	FILE *(*fopen_function)(const char *filename, char **opened_path);
	void (*message_handler)(long message, void *data);
	void (*block_interruptions)(void);
	void (*unblock_interruptions)(void);
	int (*get_configuration_directive)(char *name, uint name_length, zval *contents);
	void (*ticks_function)(int ticks);
} zend_utility_functions;

		
typedef struct _zend_utility_values {
	char *import_use_extension;
	uint import_use_extension_length;
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


/* data types */
#define IS_NULL		0
#define IS_LONG		1
#define IS_DOUBLE	2
#define IS_STRING	3
#define IS_ARRAY	4
#define IS_OBJECT	5
#define IS_BOOL		6
#define IS_RESOURCE	7
#define IS_CONSTANT	8
#define IS_CONSTANT_ARRAY	9

/* Special data type to temporarily mark large numbers */
#define FLAG_IS_BC	10 /* for parser internal use only */

/* Ugly hack to support constants as static array indices */
#define IS_CONSTANT_INDEX	0x80


/* overloaded elements data types */
#define OE_IS_ARRAY	(1<<0)
#define OE_IS_OBJECT	(1<<1)
#define OE_IS_METHOD	(1<<2)


/* Argument passing types */
#define BYREF_NONE 0
#define BYREF_FORCE 1
#define BYREF_ALLOW 2
#define BYREF_FORCE_REST 3

int zend_startup(zend_utility_functions *utility_functions, char **extensions, int start_builtin_functions);
void zend_shutdown(TSRMLS_D);

void zend_set_utility_values(zend_utility_values *utility_values);

BEGIN_EXTERN_C()
ZEND_API void _zend_bailout(char *filename, uint lineno);
END_EXTERN_C()

#define zend_bailout()		_zend_bailout(__FILE__, __LINE__)

#define zend_try												\
	{															\
		jmp_buf orig_bailout;									\
		zend_bool orig_bailout_set=EG(bailout_set);				\
																\
		EG(bailout_set) = 1;									\
		memcpy(&orig_bailout, &EG(bailout), sizeof(jmp_buf));	\
		if (setjmp(EG(bailout))==0)
#define zend_catch												\
		else
#define zend_end_try()											\
		memcpy(&EG(bailout), &orig_bailout, sizeof(jmp_buf));	\
		EG(bailout_set) = orig_bailout_set;						\
	}
#define zend_first_try		EG(bailout_set)=0;	zend_try

ZEND_API char *get_zend_version(void);
ZEND_API void zend_make_printable_zval(zval *expr, zval *expr_copy, int *use_copy);
ZEND_API int zend_print_zval(zval *expr, int indent);
ZEND_API int zend_print_zval_ex(zend_write_func_t write_func, zval *expr, int indent);
ZEND_API void zend_print_zval_r(zval *expr, int indent);
ZEND_API void zend_print_zval_r_ex(zend_write_func_t write_func, zval *expr, int indent);
ZEND_API void zend_output_debug_string(zend_bool trigger_break, char *format, ...);

#if ZEND_DEBUG
#define Z_DBG(expr)		(expr)
#else
#define	Z_DBG(expr)
#endif

ZEND_API extern char *empty_string;

ZEND_API void free_estring(char **str_p);

#define STR_FREE(ptr) if (ptr && ptr!=empty_string) { efree(ptr); }
#define STR_FREE_REL(ptr) if (ptr && ptr!=empty_string) { efree_rel(ptr); }

#define STR_REALLOC(ptr, size)										\
	if (ptr!=empty_string) {										\
		ptr = (char *) erealloc(ptr, size);							\
	} else {														\
		ptr = (char *) emalloc(size);								\
		memset(ptr, 0, size);										\
	}

/* output support */
#define ZEND_WRITE(str, str_len)		zend_write((str), (str_len))
#define ZEND_PUTS(str)					zend_write((str), strlen((str)))
#define ZEND_PUTC(c)					zend_write(&(c), 1), (c)


BEGIN_EXTERN_C()
extern ZEND_API int (*zend_printf)(const char *format, ...);
extern ZEND_API zend_write_func_t zend_write;
extern ZEND_API FILE *(*zend_fopen)(const char *filename, char **opened_path);
extern ZEND_API void (*zend_block_interruptions)(void);
extern ZEND_API void (*zend_unblock_interruptions)(void);
extern ZEND_API void (*zend_ticks_function)(int ticks);
extern ZEND_API void (*zend_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);


ZEND_API void zend_error(int type, const char *format, ...);

void zenderror(char *error);

/* The following #define is used for code duality in PHP for Engine 1 & 2 */
#define ZEND_STANDARD_CLASS_DEF_PTR zend_standard_class_def
extern ZEND_API zend_class_entry *zend_standard_class_def;
extern zend_utility_values zend_uv;
extern ZEND_API zval zval_used_for_init;

END_EXTERN_C()

#define ZEND_UV(name) (zend_uv.name)


#define HANDLE_BLOCK_INTERRUPTIONS()		if (zend_block_interruptions) { zend_block_interruptions(); }
#define HANDLE_UNBLOCK_INTERRUPTIONS()		if (zend_unblock_interruptions) { zend_unblock_interruptions(); }

BEGIN_EXTERN_C()
ZEND_API void zend_message_dispatcher(long message, void *data);
END_EXTERN_C()

ZEND_API int zend_get_configuration_directive(char *name, uint name_length, zval *contents);


/* Messages for applications of Zend */
#define ZMSG_FAILED_INCLUDE_FOPEN		1L
#define ZMSG_FAILED_REQUIRE_FOPEN		2L
#define ZMSG_FAILED_HIGHLIGHT_FOPEN		3L
#define ZMSG_MEMORY_LEAK_DETECTED		4L
#define ZMSG_MEMORY_LEAK_REPEATED		5L
#define ZMSG_LOG_SCRIPT_NAME			6L


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

#define ZEND_MAX_RESERVED_RESOURCES	4


#endif /* ZEND_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
