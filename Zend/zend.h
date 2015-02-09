/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
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

#define ZEND_VERSION "3.0.0-dev"

#define ZEND_ENGINE_3

#define ZEND_MAX_RESERVED_RESOURCES	4

#include "zend_types.h"
#include "zend_errors.h"
#include "zend_alloc.h"
#include "zend_hash.h"
#include "zend_llist.h"
#include "zend_string.h"
#include "zend_ast.h"
#include "zend_gc.h"
#include "zend_variables.h"
#include "zend_iterators.h"
#include "zend_stream.h"

#ifdef ZEND_SIGNALS
# include "zend_signal.h"
#endif

#ifndef ZEND_SIGNALS
# define HANDLE_BLOCK_INTERRUPTIONS()		if (zend_block_interruptions) { zend_block_interruptions(); }
# define HANDLE_UNBLOCK_INTERRUPTIONS()		if (zend_unblock_interruptions) { zend_unblock_interruptions(); }
#else
# define HANDLE_BLOCK_INTERRUPTIONS()		ZEND_SIGNAL_BLOCK_INTERRUPUTIONS()
# define HANDLE_UNBLOCK_INTERRUPTIONS()		ZEND_SIGNAL_UNBLOCK_INTERRUPTIONS()
#endif

#define INTERNAL_FUNCTION_PARAMETERS zend_execute_data *execute_data, zval *return_value
#define INTERNAL_FUNCTION_PARAM_PASSTHRU execute_data, return_value

#define USED_RET() \
	(!EX(prev_execute_data) || \
	 !ZEND_USER_CODE(EX(prev_execute_data)->func->common.type) || \
	 !(EX(prev_execute_data)->opline->result_type & EXT_TYPE_UNUSED))

#ifdef ZEND_ENABLE_STATIC_TSRMLS_CACHE
#define ZEND_TSRMG TSRMG_STATIC
#define ZEND_TSRMLS_CACHE_EXTERN TSRMLS_CACHE_EXTERN
#define ZEND_TSRMLS_CACHE_DEFINE TSRMLS_CACHE_DEFINE
#define ZEND_TSRMLS_CACHE_UPDATE TSRMLS_CACHE_UPDATE
#define ZEND_TSRMLS_CACHE TSRMLS_CACHE
#else
#define ZEND_TSRMG TSRMG
#define ZEND_TSRMLS_CACHE_EXTERN
#define ZEND_TSRMLS_CACHE_DEFINE
#define ZEND_TSRMLS_CACHE_UPDATE
#define ZEND_TSRMLS_CACHE
#endif

ZEND_TSRMLS_CACHE_EXTERN;

#ifdef HAVE_NORETURN
# ifdef ZEND_NORETRUN_ALIAS
void zend_error_noreturn(int type, const char *format, ...) ZEND_NORETURN;
# else
ZEND_API ZEND_NORETURN void zend_error_noreturn(int type, const char *format, ...);
# endif
#else
# define zend_error_noreturn zend_error
#endif

/* overloaded elements data types */
#define OE_IS_ARRAY					(1<<0)
#define OE_IS_OBJECT				(1<<1)
#define OE_IS_METHOD				(1<<2)

struct _zend_serialize_data;
struct _zend_unserialize_data;

typedef struct _zend_serialize_data zend_serialize_data;
typedef struct _zend_unserialize_data zend_unserialize_data;

typedef struct _zend_trait_method_reference {
	zend_string *method_name;
	zend_class_entry *ce;
	zend_string *class_name;
} zend_trait_method_reference;

typedef struct _zend_trait_precedence {
	zend_trait_method_reference *trait_method;
	union {
		zend_class_entry  *ce;
		zend_string       *class_name;
	} *exclude_from_classes;
} zend_trait_precedence;

typedef struct _zend_trait_alias {
	zend_trait_method_reference *trait_method;

	/**
	* name for method to be added
	*/
	zend_string *alias;

	/**
	* modifiers to be set on trait method
	*/
	uint32_t modifiers;
} zend_trait_alias;

struct _zend_class_entry {
	char type;
	zend_string *name;
	struct _zend_class_entry *parent;
	int refcount;
	uint32_t ce_flags;

	int default_properties_count;
	int default_static_members_count;
	zval *default_properties_table;
	zval *default_static_members_table;
	zval *static_members_table;
	HashTable function_table;
	HashTable properties_info;
	HashTable constants_table;

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
	union _zend_function *__debugInfo;
	union _zend_function *serialize_func;
	union _zend_function *unserialize_func;

	zend_class_iterator_funcs iterator_funcs;

	/* handlers */
	zend_object* (*create_object)(zend_class_entry *class_type);
	zend_object_iterator *(*get_iterator)(zend_class_entry *ce, zval *object, int by_ref);
	int (*interface_gets_implemented)(zend_class_entry *iface, zend_class_entry *class_type); /* a class implements this interface */
	union _zend_function *(*get_static_method)(zend_class_entry *ce, zend_string* method);

	/* serializer callbacks */
	int (*serialize)(zval *object, unsigned char **buffer, size_t *buf_len, zend_serialize_data *data);
	int (*unserialize)(zval *object, zend_class_entry *ce, const unsigned char *buf, size_t buf_len, zend_unserialize_data *data);

	uint32_t num_interfaces;
	uint32_t num_traits;
	zend_class_entry **interfaces;

	zend_class_entry **traits;
	zend_trait_alias **trait_aliases;
	zend_trait_precedence **trait_precedences;

	union {
		struct {
			zend_string *filename;
			uint32_t line_start;
			uint32_t line_end;
			zend_string *doc_comment;
		} user;
		struct {
			const struct _zend_function_entry *builtin_functions;
			struct _zend_module_entry *module;
		} internal;
	} info;
};

typedef struct _zend_utility_functions {
	void (*error_function)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args) ZEND_ATTRIBUTE_PTR_FORMAT(printf, 4, 0);
	size_t (*printf_function)(const char *format, ...) ZEND_ATTRIBUTE_PTR_FORMAT(printf, 1, 2);
	size_t (*write_function)(const char *str, size_t str_length);
	FILE *(*fopen_function)(const char *filename, char **opened_path);
	void (*message_handler)(zend_long message, const void *data);
	void (*block_interruptions)(void);
	void (*unblock_interruptions)(void);
	zval *(*get_configuration_directive)(zend_string *name);
	void (*ticks_function)(int ticks);
	void (*on_timeout)(int seconds);
	int (*stream_open_function)(const char *filename, zend_file_handle *handle);
	size_t (*vspprintf_function)(char **pbuf, size_t max_len, const char *format, va_list ap);
	zend_string *(*vstrpprintf_function)(size_t max_len, const char *format, va_list ap);
	char *(*getenv_function)(char *name, size_t name_len);
	char *(*resolve_path_function)(const char *filename, int filename_len);
} zend_utility_functions;

typedef struct _zend_utility_values {
	char *import_use_extension;
	uint import_use_extension_length;
	zend_bool html_errors;
} zend_utility_values;

typedef int (*zend_write_func_t)(const char *str, size_t str_length);

#define zend_bailout()		_zend_bailout(__FILE__, __LINE__)

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
int zend_startup(zend_utility_functions *utility_functions, char **extensions);
void zend_shutdown(void);
void zend_register_standard_ini_entries(void);
void zend_post_startup(void);
void zend_set_utility_values(zend_utility_values *utility_values);

ZEND_API void _zend_bailout(char *filename, uint lineno);

ZEND_API char *get_zend_version(void);
ZEND_API int zend_make_printable_zval(zval *expr, zval *expr_copy);
ZEND_API size_t zend_print_zval(zval *expr, int indent);
ZEND_API size_t zend_print_zval_ex(zend_write_func_t write_func, zval *expr, int indent);
ZEND_API void zend_print_zval_r(zval *expr, int indent);
ZEND_API void zend_print_flat_zval_r(zval *expr);
ZEND_API void zend_print_zval_r_ex(zend_write_func_t write_func, zval *expr, int indent);
ZEND_API void zend_output_debug_string(zend_bool trigger_break, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);

ZEND_API void zend_activate(void);
ZEND_API void zend_deactivate(void);
ZEND_API void zend_call_destructors(void);
ZEND_API void zend_activate_modules(void);
ZEND_API void zend_deactivate_modules(void);
ZEND_API void zend_post_deactivate_modules(void);

ZEND_API void free_estring(char **str_p);
ZEND_API void free_string_zval(zval *zv);
END_EXTERN_C()

/* output support */
#define ZEND_WRITE(str, str_len)		zend_write((str), (str_len))
#define ZEND_WRITE_EX(str, str_len)		write_func((str), (str_len))
#define ZEND_PUTS(str)					zend_write((str), strlen((str)))
#define ZEND_PUTS_EX(str)				write_func((str), strlen((str)))
#define ZEND_PUTC(c)					zend_write(&(c), 1)

BEGIN_EXTERN_C()
extern ZEND_API size_t (*zend_printf)(const char *format, ...) ZEND_ATTRIBUTE_PTR_FORMAT(printf, 1, 2);
extern ZEND_API zend_write_func_t zend_write;
extern ZEND_API FILE *(*zend_fopen)(const char *filename, char **opened_path);
extern ZEND_API void (*zend_block_interruptions)(void);
extern ZEND_API void (*zend_unblock_interruptions)(void);
extern ZEND_API void (*zend_ticks_function)(int ticks);
extern ZEND_API void (*zend_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args) ZEND_ATTRIBUTE_PTR_FORMAT(printf, 4, 0);
extern ZEND_API void (*zend_on_timeout)(int seconds);
extern ZEND_API int (*zend_stream_open_function)(const char *filename, zend_file_handle *handle);
extern size_t (*zend_vspprintf)(char **pbuf, size_t max_len, const char *format, va_list ap);
extern zend_string *(*zend_vstrpprintf)(size_t max_len, const char *format, va_list ap);
extern ZEND_API char *(*zend_getenv)(char *name, size_t name_len);
extern ZEND_API char *(*zend_resolve_path)(const char *filename, int filename_len);

ZEND_API void zend_error(int type, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);

void zenderror(const char *error);

/* The following #define is used for code duality in PHP for Engine 1 & 2 */
#define ZEND_STANDARD_CLASS_DEF_PTR zend_standard_class_def
extern ZEND_API zend_class_entry *zend_standard_class_def;
extern ZEND_API zend_utility_values zend_uv;
extern ZEND_API zval zval_used_for_init;

END_EXTERN_C()

#define ZEND_UV(name) (zend_uv.name)

BEGIN_EXTERN_C()
ZEND_API void zend_message_dispatcher(zend_long message, const void *data);

ZEND_API zval *zend_get_configuration_directive(zend_string *name);
END_EXTERN_C()

/* Messages for applications of Zend */
#define ZMSG_FAILED_INCLUDE_FOPEN		1L
#define ZMSG_FAILED_REQUIRE_FOPEN		2L
#define ZMSG_FAILED_HIGHLIGHT_FOPEN		3L
#define ZMSG_MEMORY_LEAK_DETECTED		4L
#define ZMSG_MEMORY_LEAK_REPEATED		5L
#define ZMSG_LOG_SCRIPT_NAME			6L
#define ZMSG_MEMORY_LEAKS_GRAND_TOTAL	7L

typedef enum {
	EH_NORMAL = 0,
	EH_SUPPRESS,
	EH_THROW
} zend_error_handling_t;

typedef struct {
	zend_error_handling_t  handling;
	zend_class_entry       *exception;
	zval                   user_handler;
} zend_error_handling;

ZEND_API void zend_save_error_handling(zend_error_handling *current);
ZEND_API void zend_replace_error_handling(zend_error_handling_t error_handling, zend_class_entry *exception_class, zend_error_handling *current);
ZEND_API void zend_restore_error_handling(zend_error_handling *saved);

#define DEBUG_BACKTRACE_PROVIDE_OBJECT (1<<0)
#define DEBUG_BACKTRACE_IGNORE_ARGS    (1<<1)

#include "zend_object_handlers.h"
#include "zend_operators.h"

#endif /* ZEND_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
