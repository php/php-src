/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.91 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_91.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#ifndef _ZEND_H
#define _ZEND_H

#define ZEND_VERSION "0.90"

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

#if (defined(WINNT) && WINNT) || (defined(WIN32) && WIN32)
# include "zend_config.w32.h"
#else
# include "zend_config.h"
#endif

/* all HAVE_XXX test have to be after the include of zend_config above */

#ifdef HAVE_UNIX_H
# include <unix.h>
#endif

#ifdef HAVE_STDARG_H
# include <stdarg.h>
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


#include "zend_errors.h"
#include "zend_alloc.h"

typedef unsigned char zend_bool;

#undef SUCCESS
#undef FAILURE
#define SUCCESS 0
#define FAILURE -1				/* this MUST stay a negative number, or it may affect functions! */


#include "zend_hash.h"
#include "zend_llist.h"

#define INTERNAL_FUNCTION_PARAMETERS int ht, zval *return_value, HashTable *list, HashTable *plist, zval *this_ptr, int return_value_used
#define INTERNAL_FUNCTION_PARAM_PASSTHRU ht, return_value, list, plist, this_ptr, return_value_used

/*
 * zval
 */
typedef struct _zval_struct zval;
typedef struct _zend_class_entry zend_class_entry;

typedef union _zvalue_value {
	long lval;					/* long value */
	double dval;				/* double value */
	struct {
		char *val;
		int len;
	} str;
	char chval;					/* char value */
	HashTable *ht;				/* hash table value */
	struct {
		zend_class_entry *ce;
		HashTable *properties;
	} obj;
} zvalue_value;


struct _zval_struct {
	/* Variable information */
	zvalue_value value;		/* value */
	unsigned char type;	/* active type */
	unsigned char is_ref;
	short refcount;
};



typedef struct _zend_function_entry {
	char *fname;
	void (*handler)(INTERNAL_FUNCTION_PARAMETERS);
	unsigned char *func_arg_types;
} zend_function_entry;


typedef struct _zend_property_reference {
	int type;  /* read, write or r/w */
	zval *object;
	zend_llist elements_list;
} zend_property_reference;



typedef struct _zend_overloaded_element {
	int type;		/* array offset or object proprety */
	zval element;
} zend_overloaded_element;

/* excpt.h on Digital Unix 4.0 defines function_table */
#undef function_table

struct _zend_class_entry {
	char type;
	char *name;
	uint name_length;
	struct _zend_class_entry *parent; 
	int *refcount;
	zend_bool constants_updated;

	HashTable function_table;
	HashTable default_properties;
	zend_function_entry *builtin_functions;

	/* handlers */
	void (*handle_function_call)(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference);
	zval (*handle_property_get)(zend_property_reference *property_reference);
	int (*handle_property_set)(zend_property_reference *property_reference, zval *value);
};



typedef struct _zend_utility_functions {
	void (*error_function)(int type, const char *format, ...);
	int (*printf_function)(const char *format, ...);
	int (*write_function)(const char *str, uint str_length);
	FILE *(*fopen_function)(const char *filename, char **opened_path);
	void (*message_handler)(long message, void *data);
	void (*block_interruptions)(void);
	void (*unblock_interruptions)(void);
	int (*get_ini_entry)(char *name, uint name_length, zval *contents);
} zend_utility_functions;

		
typedef struct _zend_utility_values {
	zend_bool short_tags;
	zend_bool asp_tags;
	zend_bool allow_call_time_pass_reference;
	char *import_use_extension;
	uint import_use_extension_length;
} zend_utility_values;


#undef MIN
#undef MAX
#define MAX(a,b)  (((a)>(b))?(a):(b))
#define MIN(a,b)  (((a)<(b))?(a):(b))

/* data types */
#define IS_LONG		1
#define IS_DOUBLE	2
#define IS_STRING	3
#define IS_ARRAY	4
#define IS_OBJECT	5
#define IS_BC 		6 /* for parser internal use only */
#define IS_BOOL		7
#define IS_RESOURCE 8
#define IS_CONSTANT	9
#define IS_METHOD	10 /* for overloaded function calls */

int zend_startup(zend_utility_functions *utility_functions, char **extensions);
void zend_shutdown(void);

void zend_set_utility_values(zend_utility_values *utility_values);
BEGIN_EXTERN_C()
ZEND_API void zend_bailout(void);
END_EXTERN_C()
ZEND_API char *get_zend_version(void);

ZEND_API void zend_make_printable_zval(zval *expr, zval *expr_copy, int *use_copy);
ZEND_API int zend_print_zval(zval *expr, int indent);
ZEND_API void zend_print_zval_r(zval *expr, int indent);

ZEND_API extern char *empty_string;
ZEND_API extern char *undefined_variable_string;

#define STR_FREE(ptr) if (ptr && ptr!=empty_string && ptr!=undefined_variable_string) { efree(ptr); }
#define STR_FREE_REL(ptr) if (ptr && ptr!=empty_string && ptr!=undefined_variable_string) { efree_rel(ptr); }

#define STR_REALLOC(ptr, size)										\
	if (ptr!=empty_string && ptr!=undefined_variable_string) {		\
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
extern ZEND_API int (*zend_write)(const char *str, uint str_length);
extern ZEND_API void (*zend_error)(int type, const char *format, ...);
extern ZEND_API FILE *(*zend_fopen)(const char *filename, char **opened_path);
extern ZEND_API void (*zend_block_interruptions)(void);
extern ZEND_API void (*zend_unblock_interruptions)(void);
extern ZEND_API void (*zend_message_dispatcher)(long message, void *data);
extern ZEND_API int (*zend_get_ini_entry)(char *name, uint name_length, zval *contents);
 
void zenderror(char *error);

extern ZEND_API zend_class_entry zend_standard_class_def;
extern zend_utility_values zend_uv;
extern ZEND_API zval zval_used_for_init;

END_EXTERN_C()

#define ZEND_UV(name) (zend_uv.name)


#define HANDLE_BLOCK_INTERRUPTIONS()		if (zend_block_interruptions) { zend_block_interruptions(); }
#define HANDLE_UNBLOCK_INTERRUPTIONS()		if (zend_unblock_interruptions) { zend_unblock_interruptions(); }


/* Messages for applications of Zend */
#define ZMSG_ENABLE_TRACK_VARS			1L
#define ZMSG_FAILED_INCLUDE_FOPEN		2L
#define ZMSG_FAILED_REQUIRE_FOPEN		3L
#define ZMSG_FAILED_HIGHLIGHT_FOPEN		4L
#define ZMSG_MEMORY_LEAK_DETECTED		5L
#define ZMSG_MEMORY_LEAK_REPEATED		6L
#define ZMSG_LOG_SCRIPT_NAME		7L

#define INIT_PZVAL(z)		\
	(z)->refcount = 1;		\
	(z)->is_ref = 0;	

#define INIT_ZVAL(z) z = zval_used_for_init;

#define ALLOC_ZVAL(z) (z) = (zval *) emalloc(sizeof(zval))

#define FREE_ZVAL(z) efree(z)

#define ALLOC_INIT_ZVAL(zp)						\
	ALLOC_ZVAL(zp);		\
	INIT_ZVAL(*zp);

#define MAKE_STD_ZVAL(zv)				 \
	ALLOC_ZVAL(zv); \
	INIT_PZVAL(zv);

#define SEPARATE_ZVAL(ppzv)									\
	{														\
		zval *orig_ptr = *(ppzv);							\
															\
		if (orig_ptr->refcount>1) {							\
			orig_ptr->refcount--;							\
			ALLOC_ZVAL(*(ppzv));		\
			**(ppzv) = *orig_ptr;							\
			zval_copy_ctor(*(ppzv));						\
			(*(ppzv))->refcount=1;							\
			(*(ppzv))->is_ref = 0;							\
		}													\
	}

#define COPY_PZVAL_TO_ZVAL(zv, pzv)			\
	(zv) = *(pzv);							\
	if ((pzv)->refcount>1) {				\
		zval_copy_ctor(&(zv));				\
		(pzv)->refcount--;					\
	} else {								\
		FREE_ZVAL(pzv);							\
	}										\
	INIT_PZVAL(&(zv));

#define ZEND_MAX_RESERVED_RESOURCES	1

#endif /* _ZEND_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
