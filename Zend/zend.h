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
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_H
#define ZEND_H

#define ZEND_VERSION "4.0.0-dev"

#define ZEND_ENGINE_3

#include "zend_types.h"
#include "zend_map_ptr.h"
#include "zend_errors.h"
#include "zend_alloc.h"
#include "zend_llist.h"
#include "zend_string.h"
#include "zend_hash.h"
#include "zend_ast.h"
#include "zend_gc.h"
#include "zend_variables.h"
#include "zend_iterators.h"
#include "zend_stream.h"
#include "zend_smart_str_public.h"
#include "zend_smart_string_public.h"

#define zend_sprintf sprintf

#define INTERNAL_FUNCTION_PARAMETERS zend_execute_data *execute_data, zval *return_value
#define INTERNAL_FUNCTION_PARAM_PASSTHRU execute_data, return_value

#define USED_RET() \
	(!EX(prev_execute_data) || \
	 !ZEND_USER_CODE(EX(prev_execute_data)->func->common.type) || \
	 (EX(prev_execute_data)->opline->result_type != IS_UNUSED))

#ifdef ZEND_ENABLE_STATIC_TSRMLS_CACHE
#define ZEND_TSRMG TSRMG_STATIC
#define ZEND_TSRMG_FAST TSRMG_FAST_STATIC
#define ZEND_TSRMLS_CACHE_EXTERN() TSRMLS_CACHE_EXTERN()
#define ZEND_TSRMLS_CACHE_DEFINE() TSRMLS_CACHE_DEFINE()
#define ZEND_TSRMLS_CACHE_UPDATE() TSRMLS_CACHE_UPDATE()
#define ZEND_TSRMLS_CACHE TSRMLS_CACHE
#else
#define ZEND_TSRMG TSRMG
#define ZEND_TSRMG_FAST TSRMG_FAST
#define ZEND_TSRMLS_CACHE_EXTERN()
#define ZEND_TSRMLS_CACHE_DEFINE()
#define ZEND_TSRMLS_CACHE_UPDATE()
#define ZEND_TSRMLS_CACHE
#endif

ZEND_TSRMLS_CACHE_EXTERN()

struct _zend_serialize_data;
struct _zend_unserialize_data;

typedef struct _zend_serialize_data zend_serialize_data;
typedef struct _zend_unserialize_data zend_unserialize_data;

typedef struct _zend_class_name {
	zend_string *name;
	zend_string *lc_name;
} zend_class_name;

typedef struct _zend_trait_method_reference {
	zend_string *method_name;
	zend_string *class_name;
} zend_trait_method_reference;

typedef struct _zend_trait_precedence {
	zend_trait_method_reference trait_method;
	uint32_t num_excludes;
	zend_string *exclude_class_names[1];
} zend_trait_precedence;

typedef struct _zend_trait_alias {
	zend_trait_method_reference trait_method;

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
	/* class_entry or string depending on ZEND_ACC_LINKED */
	union {
		zend_class_entry *parent;
		zend_string *parent_name;
	};
	int refcount;
	uint32_t ce_flags;

	int default_properties_count;
	int default_static_members_count;
	zval *default_properties_table;
	zval *default_static_members_table;
	ZEND_MAP_PTR_DEF(zval *, static_members_table);
	HashTable function_table;
	HashTable properties_info;
	HashTable constants_table;

	struct _zend_property_info **properties_info_table;

	zend_function *constructor;
	zend_function *destructor;
	zend_function *clone;
	zend_function *__get;
	zend_function *__set;
	zend_function *__unset;
	zend_function *__isset;
	zend_function *__call;
	zend_function *__callstatic;
	zend_function *__tostring;
	zend_function *__debugInfo;
	zend_function *serialize_func;
	zend_function *unserialize_func;

	/* allocated only if class implements Iterator or IteratorAggregate interface */
	zend_class_iterator_funcs *iterator_funcs_ptr;

	/* handlers */
	union {
		zend_object* (*create_object)(zend_class_entry *class_type);
		int (*interface_gets_implemented)(zend_class_entry *iface, zend_class_entry *class_type); /* a class implements this interface */
	};
	zend_object_iterator *(*get_iterator)(zend_class_entry *ce, zval *object, int by_ref);
	zend_function *(*get_static_method)(zend_class_entry *ce, zend_string* method);

	/* serializer callbacks */
	int (*serialize)(zval *object, unsigned char **buffer, size_t *buf_len, zend_serialize_data *data);
	int (*unserialize)(zval *object, zend_class_entry *ce, const unsigned char *buf, size_t buf_len, zend_unserialize_data *data);

	uint32_t num_interfaces;
	uint32_t num_traits;

	/* class_entry or string(s) depending on ZEND_ACC_LINKED */
	union {
		zend_class_entry **interfaces;
		zend_class_name *interface_names;
	};

	zend_class_name *trait_names;
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
	void (*error_function)(int type, const char *error_filename, const uint32_t error_lineno, const char *format, va_list args) ZEND_ATTRIBUTE_PTR_FORMAT(printf, 4, 0);
	size_t (*printf_function)(const char *format, ...) ZEND_ATTRIBUTE_PTR_FORMAT(printf, 1, 2);
	size_t (*write_function)(const char *str, size_t str_length);
	FILE *(*fopen_function)(const char *filename, zend_string **opened_path);
	void (*message_handler)(zend_long message, const void *data);
	zval *(*get_configuration_directive)(zend_string *name);
	void (*ticks_function)(int ticks);
	void (*on_timeout)(int seconds);
	int (*stream_open_function)(const char *filename, zend_file_handle *handle);
	void (*printf_to_smart_string_function)(smart_string *buf, const char *format, va_list ap);
	void (*printf_to_smart_str_function)(smart_str *buf, const char *format, va_list ap);
	char *(*getenv_function)(char *name, size_t name_len);
	zend_string *(*resolve_path_function)(const char *filename, size_t filename_len);
} zend_utility_functions;

typedef struct _zend_utility_values {
	zend_bool html_errors;
} zend_utility_values;

typedef size_t (*zend_write_func_t)(const char *str, size_t str_length);

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
int zend_startup(zend_utility_functions *utility_functions);
void zend_shutdown(void);
void zend_register_standard_ini_entries(void);
int zend_post_startup(void);
void zend_set_utility_values(zend_utility_values *utility_values);

ZEND_API ZEND_COLD ZEND_NORETURN void _zend_bailout(const char *filename, uint32_t lineno);

ZEND_API size_t zend_vspprintf(char **pbuf, size_t max_len, const char *format, va_list ap);
ZEND_API size_t zend_spprintf(char **message, size_t max_len, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 3, 4);
ZEND_API zend_string *zend_vstrpprintf(size_t max_len, const char *format, va_list ap);
ZEND_API zend_string *zend_strpprintf(size_t max_len, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);

/* Same as zend_spprintf and zend_strpprintf, without checking of format validity.
 * For use with custom printf specifiers such as %H. */
ZEND_API size_t zend_spprintf_unchecked(char **message, size_t max_len, const char *format, ...);
ZEND_API zend_string *zend_strpprintf_unchecked(size_t max_len, const char *format, ...);

ZEND_API const char *get_zend_version(void);
ZEND_API int zend_make_printable_zval(zval *expr, zval *expr_copy);
ZEND_API size_t zend_print_zval(zval *expr, int indent);
ZEND_API void zend_print_zval_r(zval *expr, int indent);
ZEND_API zend_string *zend_print_zval_r_to_str(zval *expr, int indent);
ZEND_API void zend_print_flat_zval_r(zval *expr);

#define zend_print_variable(var) \
	zend_print_zval((var), 0)

ZEND_API ZEND_COLD void zend_output_debug_string(zend_bool trigger_break, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);

ZEND_API void zend_activate(void);
ZEND_API void zend_deactivate(void);
ZEND_API void zend_call_destructors(void);
ZEND_API void zend_activate_modules(void);
ZEND_API void zend_deactivate_modules(void);
ZEND_API void zend_post_deactivate_modules(void);

ZEND_API void free_estring(char **str_p);

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
extern ZEND_API FILE *(*zend_fopen)(const char *filename, zend_string **opened_path);
extern ZEND_API void (*zend_ticks_function)(int ticks);
extern ZEND_API void (*zend_interrupt_function)(zend_execute_data *execute_data);
extern ZEND_API void (*zend_error_cb)(int type, const char *error_filename, const uint32_t error_lineno, const char *format, va_list args) ZEND_ATTRIBUTE_PTR_FORMAT(printf, 4, 0);
extern ZEND_API void (*zend_on_timeout)(int seconds);
extern ZEND_API int (*zend_stream_open_function)(const char *filename, zend_file_handle *handle);
extern void (*zend_printf_to_smart_string)(smart_string *buf, const char *format, va_list ap);
extern void (*zend_printf_to_smart_str)(smart_str *buf, const char *format, va_list ap);
extern ZEND_API char *(*zend_getenv)(char *name, size_t name_len);
extern ZEND_API zend_string *(*zend_resolve_path)(const char *filename, size_t filename_len);

/* These two callbacks are especially for opcache */
extern ZEND_API int (*zend_post_startup_cb)(void);
extern ZEND_API void (*zend_post_shutdown_cb)(void);

/* Callback for loading of not preloaded part of the script */
extern ZEND_API int (*zend_preload_autoload)(zend_string *filename);

ZEND_API ZEND_COLD void zend_error(int type, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);
ZEND_API ZEND_COLD ZEND_NORETURN void zend_error_noreturn(int type, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);
/* If filename is NULL the default filename is used. */
ZEND_API ZEND_COLD void zend_error_at(int type, const char *filename, uint32_t lineno, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 4, 5);
ZEND_API ZEND_COLD ZEND_NORETURN void zend_error_at_noreturn(int type, const char *filename, uint32_t lineno, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 4, 5);

ZEND_API ZEND_COLD void zend_throw_error(zend_class_entry *exception_ce, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);
ZEND_API ZEND_COLD void zend_type_error(const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 1, 2);
ZEND_API ZEND_COLD void zend_argument_count_error(const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 1, 2);
ZEND_API ZEND_COLD void zend_value_error(const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 1, 2);

ZEND_COLD void zenderror(const char *error);

/* The following #define is used for code duality in PHP for Engine 1 & 2 */
#define ZEND_STANDARD_CLASS_DEF_PTR zend_standard_class_def
extern ZEND_API zend_class_entry *zend_standard_class_def;
extern ZEND_API zend_utility_values zend_uv;

/* If DTrace is available and enabled */
extern ZEND_API zend_bool zend_dtrace_enabled;
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
