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

#include "zend.h"
#include "zend_extensions.h"
#include "zend_modules.h"
#include "zend_constants.h"
#include "zend_list.h"
#include "zend_API.h"
#include "zend_exceptions.h"
#include "zend_builtin_functions.h"
#include "zend_ini.h"
#include "zend_vm.h"
#include "zend_dtrace.h"
#include "zend_virtual_cwd.h"
#include "zend_smart_str.h"
#include "zend_smart_string.h"
#include "zend_cpuinfo.h"

static size_t global_map_ptr_last = 0;

#ifdef ZTS
ZEND_API int compiler_globals_id;
ZEND_API int executor_globals_id;
ZEND_API size_t compiler_globals_offset;
ZEND_API size_t executor_globals_offset;
static HashTable *global_function_table = NULL;
static HashTable *global_class_table = NULL;
static HashTable *global_constants_table = NULL;
static HashTable *global_auto_globals_table = NULL;
static HashTable *global_persistent_list = NULL;
ZEND_TSRMLS_CACHE_DEFINE()
# define GLOBAL_FUNCTION_TABLE		global_function_table
# define GLOBAL_CLASS_TABLE			global_class_table
# define GLOBAL_CONSTANTS_TABLE		global_constants_table
# define GLOBAL_AUTO_GLOBALS_TABLE	global_auto_globals_table
#else
# define GLOBAL_FUNCTION_TABLE		CG(function_table)
# define GLOBAL_CLASS_TABLE			CG(class_table)
# define GLOBAL_AUTO_GLOBALS_TABLE	CG(auto_globals)
# define GLOBAL_CONSTANTS_TABLE		EG(zend_constants)
#endif

ZEND_API zend_utility_values zend_uv;
ZEND_API zend_bool zend_dtrace_enabled;

/* version information */
static char *zend_version_info;
static uint32_t zend_version_info_length;
#define ZEND_CORE_VERSION_INFO	"Zend Engine v" ZEND_VERSION ", Copyright (c) Zend Technologies\n"
#define PRINT_ZVAL_INDENT 4

/* true multithread-shared globals */
ZEND_API zend_class_entry *zend_standard_class_def = NULL;
ZEND_API size_t (*zend_printf)(const char *format, ...);
ZEND_API zend_write_func_t zend_write;
ZEND_API FILE *(*zend_fopen)(const char *filename, zend_string **opened_path);
ZEND_API int (*zend_stream_open_function)(const char *filename, zend_file_handle *handle);
ZEND_API void (*zend_ticks_function)(int ticks);
ZEND_API void (*zend_interrupt_function)(zend_execute_data *execute_data);
ZEND_API void (*zend_error_cb)(int type, const char *error_filename, const uint32_t error_lineno, const char *format, va_list args);
void (*zend_printf_to_smart_string)(smart_string *buf, const char *format, va_list ap);
void (*zend_printf_to_smart_str)(smart_str *buf, const char *format, va_list ap);
ZEND_API char *(*zend_getenv)(char *name, size_t name_len);
ZEND_API zend_string *(*zend_resolve_path)(const char *filename, size_t filename_len);
ZEND_API int (*zend_post_startup_cb)(void) = NULL;
ZEND_API void (*zend_post_shutdown_cb)(void) = NULL;
ZEND_API int (*zend_preload_autoload)(zend_string *filename) = NULL;

void (*zend_on_timeout)(int seconds);

static void (*zend_message_dispatcher_p)(zend_long message, const void *data);
static zval *(*zend_get_configuration_directive_p)(zend_string *name);

#if ZEND_RC_DEBUG
ZEND_API zend_bool zend_rc_debug = 0;
#endif

static ZEND_INI_MH(OnUpdateErrorReporting) /* {{{ */
{
	if (!new_value) {
		EG(error_reporting) = E_ALL & ~E_NOTICE & ~E_STRICT & ~E_DEPRECATED;
	} else {
		EG(error_reporting) = atoi(ZSTR_VAL(new_value));
	}
	return SUCCESS;
}
/* }}} */

static ZEND_INI_MH(OnUpdateGCEnabled) /* {{{ */
{
	zend_bool val;

	val = zend_ini_parse_bool(new_value);
	gc_enable(val);

	return SUCCESS;
}
/* }}} */

static ZEND_INI_DISP(zend_gc_enabled_displayer_cb) /* {{{ */
{
	if (gc_enabled()) {
		ZEND_PUTS("On");
	} else {
		ZEND_PUTS("Off");
	}
}
/* }}} */


static ZEND_INI_MH(OnUpdateScriptEncoding) /* {{{ */
{
	if (!CG(multibyte)) {
		return FAILURE;
	}
	if (!zend_multibyte_get_functions()) {
		return SUCCESS;
	}
	return zend_multibyte_set_script_encoding_by_string(new_value ? ZSTR_VAL(new_value) : NULL, new_value ? ZSTR_LEN(new_value) : 0);
}
/* }}} */

static ZEND_INI_MH(OnUpdateAssertions) /* {{{ */
{
	zend_long *p, val;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base;

	base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	p = (zend_long *) (base+(size_t) mh_arg1);

	val = zend_atol(ZSTR_VAL(new_value), ZSTR_LEN(new_value));

	if (stage != ZEND_INI_STAGE_STARTUP &&
	    stage != ZEND_INI_STAGE_SHUTDOWN &&
	    *p != val &&
	    (*p < 0 || val < 0)) {
		zend_error(E_WARNING, "zend.assertions may be completely enabled or disabled only in php.ini");
		return FAILURE;
	}

	*p = val;
	return SUCCESS;
}
/* }}} */

#if ZEND_DEBUG
# define SIGNAL_CHECK_DEFAULT "1"
#else
# define SIGNAL_CHECK_DEFAULT "0"
#endif

ZEND_INI_BEGIN()
	ZEND_INI_ENTRY("error_reporting",				NULL,		ZEND_INI_ALL,		OnUpdateErrorReporting)
	STD_ZEND_INI_ENTRY("zend.assertions",				"1",    ZEND_INI_ALL,       OnUpdateAssertions,           assertions,   zend_executor_globals,  executor_globals)
	ZEND_INI_ENTRY3_EX("zend.enable_gc",				"1",	ZEND_INI_ALL,		OnUpdateGCEnabled, NULL, NULL, NULL, zend_gc_enabled_displayer_cb)
 	STD_ZEND_INI_BOOLEAN("zend.multibyte", "0", ZEND_INI_PERDIR, OnUpdateBool, multibyte,      zend_compiler_globals, compiler_globals)
 	ZEND_INI_ENTRY("zend.script_encoding",			NULL,		ZEND_INI_ALL,		OnUpdateScriptEncoding)
 	STD_ZEND_INI_BOOLEAN("zend.detect_unicode",			"1",	ZEND_INI_ALL,		OnUpdateBool, detect_unicode, zend_compiler_globals, compiler_globals)
#ifdef ZEND_SIGNALS
	STD_ZEND_INI_BOOLEAN("zend.signal_check", SIGNAL_CHECK_DEFAULT, ZEND_INI_SYSTEM, OnUpdateBool, check, zend_signal_globals_t, zend_signal_globals)
#endif
	STD_ZEND_INI_BOOLEAN("zend.exception_ignore_args",	"0",	ZEND_INI_ALL,		OnUpdateBool, exception_ignore_args, zend_executor_globals, executor_globals)
ZEND_INI_END()

ZEND_API size_t zend_vspprintf(char **pbuf, size_t max_len, const char *format, va_list ap) /* {{{ */
{
	smart_string buf = {0};

	/* since there are places where (v)spprintf called without checking for null,
	   a bit of defensive coding here */
	if (!pbuf) {
		return 0;
	}

	zend_printf_to_smart_string(&buf, format, ap);

	if (max_len && buf.len > max_len) {
		buf.len = max_len;
	}

	smart_string_0(&buf);

	if (buf.c) {
		*pbuf = buf.c;
		return buf.len;
	} else {
		*pbuf = estrndup("", 0);
		return 0;
	}
}
/* }}} */

ZEND_API size_t zend_spprintf(char **message, size_t max_len, const char *format, ...) /* {{{ */
{
	va_list arg;
	size_t len;

	va_start(arg, format);
	len = zend_vspprintf(message, max_len, format, arg);
	va_end(arg);
	return len;
}
/* }}} */

ZEND_API size_t zend_spprintf_unchecked(char **message, size_t max_len, const char *format, ...) /* {{{ */
{
	va_list arg;
	size_t len;

	va_start(arg, format);
	len = zend_vspprintf(message, max_len, format, arg);
	va_end(arg);
	return len;
}
/* }}} */

ZEND_API zend_string *zend_vstrpprintf(size_t max_len, const char *format, va_list ap) /* {{{ */
{
	smart_str buf = {0};

	zend_printf_to_smart_str(&buf, format, ap);

	if (!buf.s) {
		return ZSTR_EMPTY_ALLOC();
	}

	if (max_len && ZSTR_LEN(buf.s) > max_len) {
		ZSTR_LEN(buf.s) = max_len;
	}

	smart_str_0(&buf);
	return buf.s;
}
/* }}} */

ZEND_API zend_string *zend_strpprintf(size_t max_len, const char *format, ...) /* {{{ */
{
	va_list arg;
	zend_string *str;

	va_start(arg, format);
	str = zend_vstrpprintf(max_len, format, arg);
	va_end(arg);
	return str;
}
/* }}} */

ZEND_API zend_string *zend_strpprintf_unchecked(size_t max_len, const char *format, ...) /* {{{ */
{
	va_list arg;
	zend_string *str;

	va_start(arg, format);
	str = zend_vstrpprintf(max_len, format, arg);
	va_end(arg);
	return str;
}
/* }}} */

static void zend_print_zval_r_to_buf(smart_str *buf, zval *expr, int indent);

static void print_hash(smart_str *buf, HashTable *ht, int indent, zend_bool is_object) /* {{{ */
{
	zval *tmp;
	zend_string *string_key;
	zend_ulong num_key;
	int i;

	for (i = 0; i < indent; i++) {
		smart_str_appendc(buf, ' ');
	}
	smart_str_appends(buf, "(\n");
	indent += PRINT_ZVAL_INDENT;
	ZEND_HASH_FOREACH_KEY_VAL_IND(ht, num_key, string_key, tmp) {
		for (i = 0; i < indent; i++) {
			smart_str_appendc(buf, ' ');
		}
		smart_str_appendc(buf, '[');
		if (string_key) {
			if (is_object) {
				const char *prop_name, *class_name;
				size_t prop_len;
				int mangled = zend_unmangle_property_name_ex(string_key, &class_name, &prop_name, &prop_len);

				smart_str_appendl(buf, prop_name, prop_len);
				if (class_name && mangled == SUCCESS) {
					if (class_name[0] == '*') {
						smart_str_appends(buf, ":protected");
					} else {
						smart_str_appends(buf, ":");
						smart_str_appends(buf, class_name);
						smart_str_appends(buf, ":private");
					}
				}
			} else {
				smart_str_append(buf, string_key);
			}
		} else {
			smart_str_append_long(buf, num_key);
		}
		smart_str_appends(buf, "] => ");
		zend_print_zval_r_to_buf(buf, tmp, indent+PRINT_ZVAL_INDENT);
		smart_str_appends(buf, "\n");
	} ZEND_HASH_FOREACH_END();
	indent -= PRINT_ZVAL_INDENT;
	for (i = 0; i < indent; i++) {
		smart_str_appendc(buf, ' ');
	}
	smart_str_appends(buf, ")\n");
}
/* }}} */

static void print_flat_hash(HashTable *ht) /* {{{ */
{
	zval *tmp;
	zend_string *string_key;
	zend_ulong num_key;
	int i = 0;

	ZEND_HASH_FOREACH_KEY_VAL_IND(ht, num_key, string_key, tmp) {
		if (i++ > 0) {
			ZEND_PUTS(",");
		}
		ZEND_PUTS("[");
		if (string_key) {
			ZEND_WRITE(ZSTR_VAL(string_key), ZSTR_LEN(string_key));
		} else {
			zend_printf(ZEND_ULONG_FMT, num_key);
		}
		ZEND_PUTS("] => ");
		zend_print_flat_zval_r(tmp);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

ZEND_API int zend_make_printable_zval(zval *expr, zval *expr_copy) /* {{{ */
{
	if (Z_TYPE_P(expr) == IS_STRING) {
		return 0;
	} else {
		ZVAL_STR(expr_copy, zval_get_string_func(expr));
		return 1;
	}
}
/* }}} */

ZEND_API size_t zend_print_zval(zval *expr, int indent) /* {{{ */
{
	zend_string *tmp_str;
	zend_string *str = zval_get_tmp_string(expr, &tmp_str);
	size_t len = ZSTR_LEN(str);

	if (len != 0) {
		zend_write(ZSTR_VAL(str), len);
	}

	zend_tmp_string_release(tmp_str);
	return len;
}
/* }}} */

ZEND_API void zend_print_flat_zval_r(zval *expr) /* {{{ */
{
	switch (Z_TYPE_P(expr)) {
		case IS_ARRAY:
			ZEND_PUTS("Array (");
			if (!(GC_FLAGS(Z_ARRVAL_P(expr)) & GC_IMMUTABLE)) {
				if (GC_IS_RECURSIVE(Z_ARRVAL_P(expr))) {
					ZEND_PUTS(" *RECURSION*");
					return;
				}
				GC_PROTECT_RECURSION(Z_ARRVAL_P(expr));
			}
			print_flat_hash(Z_ARRVAL_P(expr));
			ZEND_PUTS(")");
			if (!(GC_FLAGS(Z_ARRVAL_P(expr)) & GC_IMMUTABLE)) {
				GC_UNPROTECT_RECURSION(Z_ARRVAL_P(expr));
			}
			break;
		case IS_OBJECT:
		{
			HashTable *properties;
			zend_string *class_name = Z_OBJ_HANDLER_P(expr, get_class_name)(Z_OBJ_P(expr));
			zend_printf("%s Object (", ZSTR_VAL(class_name));
			zend_string_release_ex(class_name, 0);

			if (GC_IS_RECURSIVE(Z_COUNTED_P(expr))) {
				ZEND_PUTS(" *RECURSION*");
				return;
			}

			properties = Z_OBJPROP_P(expr);
			if (properties) {
				GC_PROTECT_RECURSION(Z_OBJ_P(expr));
				print_flat_hash(properties);
				GC_UNPROTECT_RECURSION(Z_OBJ_P(expr));
			}
			ZEND_PUTS(")");
			break;
		}
		case IS_REFERENCE:
			zend_print_flat_zval_r(Z_REFVAL_P(expr));
			break;
		default:
			zend_print_zval(expr, 0);
			break;
	}
}
/* }}} */

static void zend_print_zval_r_to_buf(smart_str *buf, zval *expr, int indent) /* {{{ */
{
	switch (Z_TYPE_P(expr)) {
		case IS_ARRAY:
			smart_str_appends(buf, "Array\n");
			if (!(GC_FLAGS(Z_ARRVAL_P(expr)) & GC_IMMUTABLE)) {
				if (GC_IS_RECURSIVE(Z_ARRVAL_P(expr))) {
					smart_str_appends(buf, " *RECURSION*");
					return;
				}
				GC_PROTECT_RECURSION(Z_ARRVAL_P(expr));
			}
			print_hash(buf, Z_ARRVAL_P(expr), indent, 0);
			if (!(GC_FLAGS(Z_ARRVAL_P(expr)) & GC_IMMUTABLE)) {
				GC_UNPROTECT_RECURSION(Z_ARRVAL_P(expr));
			}
			break;
		case IS_OBJECT:
			{
				HashTable *properties;

				zend_string *class_name = Z_OBJ_HANDLER_P(expr, get_class_name)(Z_OBJ_P(expr));
				smart_str_appends(buf, ZSTR_VAL(class_name));
				zend_string_release_ex(class_name, 0);

				smart_str_appends(buf, " Object\n");
				if (GC_IS_RECURSIVE(Z_OBJ_P(expr))) {
					smart_str_appends(buf, " *RECURSION*");
					return;
				}

				if ((properties = zend_get_properties_for(expr, ZEND_PROP_PURPOSE_DEBUG)) == NULL) {
					break;
				}

				GC_PROTECT_RECURSION(Z_OBJ_P(expr));
				print_hash(buf, properties, indent, 1);
				GC_UNPROTECT_RECURSION(Z_OBJ_P(expr));

				zend_release_properties(properties);
				break;
			}
		case IS_LONG:
			smart_str_append_long(buf, Z_LVAL_P(expr));
			break;
		case IS_REFERENCE:
			zend_print_zval_r_to_buf(buf, Z_REFVAL_P(expr), indent);
			break;
		case IS_STRING:
			smart_str_append(buf, Z_STR_P(expr));
			break;
		default:
			{
				zend_string *str = zval_get_string_func(expr);
				smart_str_append(buf, str);
				zend_string_release_ex(str, 0);
			}
			break;
	}
}
/* }}} */

ZEND_API zend_string *zend_print_zval_r_to_str(zval *expr, int indent) /* {{{ */
{
	smart_str buf = {0};
	zend_print_zval_r_to_buf(&buf, expr, indent);
	smart_str_0(&buf);
	return buf.s;
}
/* }}} */

ZEND_API void zend_print_zval_r(zval *expr, int indent) /* {{{ */
{
	zend_string *str = zend_print_zval_r_to_str(expr, indent);
	zend_write(ZSTR_VAL(str), ZSTR_LEN(str));
	zend_string_release_ex(str, 0);
}
/* }}} */

static FILE *zend_fopen_wrapper(const char *filename, zend_string **opened_path) /* {{{ */
{
	if (opened_path) {
		*opened_path = zend_string_init(filename, strlen(filename), 0);
	}
	return fopen(filename, "rb");
}
/* }}} */

#ifdef ZTS
static zend_bool short_tags_default      = 1;
static uint32_t compiler_options_default = ZEND_COMPILE_DEFAULT;
#else
# define short_tags_default			1
# define compiler_options_default	ZEND_COMPILE_DEFAULT
#endif

static void zend_set_default_compile_time_values(void) /* {{{ */
{
	/* default compile-time values */
	CG(short_tags) = short_tags_default;
	CG(compiler_options) = compiler_options_default;

	CG(rtd_key_counter) = 0;
}
/* }}} */

#ifdef ZEND_WIN32
static void zend_get_windows_version_info(OSVERSIONINFOEX *osvi) /* {{{ */
{
	ZeroMemory(osvi, sizeof(OSVERSIONINFOEX));
	osvi->dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!GetVersionEx((OSVERSIONINFO *) osvi)) {
		ZEND_ASSERT(0); /* Should not happen as sizeof is used. */
	}
}
/* }}} */
#endif

static void zend_init_exception_op(void) /* {{{ */
{
	memset(EG(exception_op), 0, sizeof(EG(exception_op)));
	EG(exception_op)[0].opcode = ZEND_HANDLE_EXCEPTION;
	ZEND_VM_SET_OPCODE_HANDLER(EG(exception_op));
	EG(exception_op)[1].opcode = ZEND_HANDLE_EXCEPTION;
	ZEND_VM_SET_OPCODE_HANDLER(EG(exception_op)+1);
	EG(exception_op)[2].opcode = ZEND_HANDLE_EXCEPTION;
	ZEND_VM_SET_OPCODE_HANDLER(EG(exception_op)+2);
}
/* }}} */

static void zend_init_call_trampoline_op(void) /* {{{ */
{
	memset(&EG(call_trampoline_op), 0, sizeof(EG(call_trampoline_op)));
	EG(call_trampoline_op).opcode = ZEND_CALL_TRAMPOLINE;
	ZEND_VM_SET_OPCODE_HANDLER(&EG(call_trampoline_op));
}
/* }}} */

static void auto_global_dtor(zval *zv) /* {{{ */
{
	free(Z_PTR_P(zv));
}
/* }}} */

#ifdef ZTS
static void function_copy_ctor(zval *zv) /* {{{ */
{
	zend_function *old_func = Z_FUNC_P(zv);
	zend_function *func;

	if (old_func->type == ZEND_USER_FUNCTION) {
		ZEND_ASSERT(old_func->op_array.fn_flags & ZEND_ACC_IMMUTABLE);
		return;
	}
	func = pemalloc(sizeof(zend_internal_function), 1);
	Z_FUNC_P(zv) = func;
	memcpy(func, old_func, sizeof(zend_internal_function));
	function_add_ref(func);
	if ((old_func->common.fn_flags & (ZEND_ACC_HAS_RETURN_TYPE|ZEND_ACC_HAS_TYPE_HINTS))
	 && old_func->common.arg_info) {
		uint32_t i;
		uint32_t num_args = old_func->common.num_args + 1;
		zend_arg_info *arg_info = old_func->common.arg_info - 1;
		zend_arg_info *new_arg_info;

		if (old_func->common.fn_flags & ZEND_ACC_VARIADIC) {
			num_args++;
		}
		new_arg_info = pemalloc(sizeof(zend_arg_info) * num_args, 1);
		memcpy(new_arg_info, arg_info, sizeof(zend_arg_info) * num_args);
		for (i = 0 ; i < num_args; i++) {
			if (ZEND_TYPE_IS_CLASS(arg_info[i].type)) {
				zend_string *name = zend_string_dup(ZEND_TYPE_NAME(arg_info[i].type), 1);

				new_arg_info[i].type =
					ZEND_TYPE_ENCODE_CLASS(
						name, ZEND_TYPE_ALLOW_NULL(arg_info[i].type));
			}
		}
		func->common.arg_info = new_arg_info + 1;
	}
}
/* }}} */

static void auto_global_copy_ctor(zval *zv) /* {{{ */
{
	zend_auto_global *old_ag = (zend_auto_global *) Z_PTR_P(zv);
	zend_auto_global *new_ag = pemalloc(sizeof(zend_auto_global), 1);

	new_ag->name = old_ag->name;
	new_ag->auto_global_callback = old_ag->auto_global_callback;
	new_ag->jit = old_ag->jit;

	Z_PTR_P(zv) = new_ag;
}
/* }}} */

static void compiler_globals_ctor(zend_compiler_globals *compiler_globals) /* {{{ */
{
	compiler_globals->compiled_filename = NULL;

	compiler_globals->function_table = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init_ex(compiler_globals->function_table, 1024, NULL, ZEND_FUNCTION_DTOR, 1, 0);
	zend_hash_copy(compiler_globals->function_table, global_function_table, function_copy_ctor);

	compiler_globals->class_table = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init_ex(compiler_globals->class_table, 64, NULL, ZEND_CLASS_DTOR, 1, 0);
	zend_hash_copy(compiler_globals->class_table, global_class_table, zend_class_add_ref);

	zend_set_default_compile_time_values();

	compiler_globals->auto_globals = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init_ex(compiler_globals->auto_globals, 8, NULL, auto_global_dtor, 1, 0);
	zend_hash_copy(compiler_globals->auto_globals, global_auto_globals_table, auto_global_copy_ctor);

	compiler_globals->script_encoding_list = NULL;

#if ZEND_MAP_PTR_KIND == ZEND_MAP_PTR_KIND_PTR_OR_OFFSET
	/* Map region is going to be created and resized at run-time. */
	compiler_globals->map_ptr_base = NULL;
	compiler_globals->map_ptr_size = 0;
	compiler_globals->map_ptr_last = global_map_ptr_last;
	if (compiler_globals->map_ptr_last) {
		/* Allocate map_ptr table */
		compiler_globals->map_ptr_size = ZEND_MM_ALIGNED_SIZE_EX(compiler_globals->map_ptr_last, 4096);
		compiler_globals->map_ptr_base = pemalloc(compiler_globals->map_ptr_size * sizeof(void*), 1);
		memset(compiler_globals->map_ptr_base, 0, compiler_globals->map_ptr_last * sizeof(void*));
	}
#else
# error "Unknown ZEND_MAP_PTR_KIND"
#endif
}
/* }}} */

static void compiler_globals_dtor(zend_compiler_globals *compiler_globals) /* {{{ */
{
	if (compiler_globals->function_table != GLOBAL_FUNCTION_TABLE) {
		zend_hash_destroy(compiler_globals->function_table);
		free(compiler_globals->function_table);
	}
	if (compiler_globals->class_table != GLOBAL_CLASS_TABLE) {
		zend_hash_destroy(compiler_globals->class_table);
		free(compiler_globals->class_table);
	}
	if (compiler_globals->auto_globals != GLOBAL_AUTO_GLOBALS_TABLE) {
		zend_hash_destroy(compiler_globals->auto_globals);
		free(compiler_globals->auto_globals);
	}
	if (compiler_globals->script_encoding_list) {
		pefree((char*)compiler_globals->script_encoding_list, 1);
	}
	if (compiler_globals->map_ptr_base) {
		free(compiler_globals->map_ptr_base);
		compiler_globals->map_ptr_base = NULL;
		compiler_globals->map_ptr_size = 0;
	}
}
/* }}} */

static void executor_globals_ctor(zend_executor_globals *executor_globals) /* {{{ */
{
	zend_startup_constants();
	zend_copy_constants(executor_globals->zend_constants, GLOBAL_CONSTANTS_TABLE);
	zend_init_rsrc_plist();
	zend_init_exception_op();
	zend_init_call_trampoline_op();
	memset(&executor_globals->trampoline, 0, sizeof(zend_op_array));
	executor_globals->lambda_count = 0;
	ZVAL_UNDEF(&executor_globals->user_error_handler);
	ZVAL_UNDEF(&executor_globals->user_exception_handler);
	executor_globals->in_autoload = NULL;
	executor_globals->current_execute_data = NULL;
	executor_globals->current_module = NULL;
	executor_globals->exit_status = 0;
#if XPFPA_HAVE_CW
	executor_globals->saved_fpu_cw = 0;
#endif
	executor_globals->saved_fpu_cw_ptr = NULL;
	executor_globals->active = 0;
	executor_globals->bailout = NULL;
	executor_globals->error_handling  = EH_NORMAL;
	executor_globals->exception_class = NULL;
	executor_globals->exception = NULL;
	executor_globals->objects_store.object_buckets = NULL;
#ifdef ZEND_WIN32
	zend_get_windows_version_info(&executor_globals->windows_version_info);
#endif
	executor_globals->flags = EG_FLAGS_INITIAL;
}
/* }}} */

static void executor_globals_dtor(zend_executor_globals *executor_globals) /* {{{ */
{
	zend_ini_dtor(executor_globals->ini_directives);

	if (&executor_globals->persistent_list != global_persistent_list) {
		zend_destroy_rsrc_list(&executor_globals->persistent_list);
	}
	if (executor_globals->zend_constants != GLOBAL_CONSTANTS_TABLE) {
		zend_hash_destroy(executor_globals->zend_constants);
		free(executor_globals->zend_constants);
	}
}
/* }}} */

static void zend_new_thread_end_handler(THREAD_T thread_id) /* {{{ */
{
	if (zend_copy_ini_directives() == SUCCESS) {
		zend_ini_refresh_caches(ZEND_INI_STAGE_STARTUP);
	}
}
/* }}} */
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
/* FreeBSD and DragonFly floating point precision fix */
#include <floatingpoint.h>
#endif

static void ini_scanner_globals_ctor(zend_ini_scanner_globals *scanner_globals_p) /* {{{ */
{
	memset(scanner_globals_p, 0, sizeof(*scanner_globals_p));
}
/* }}} */

static void php_scanner_globals_ctor(zend_php_scanner_globals *scanner_globals_p) /* {{{ */
{
	memset(scanner_globals_p, 0, sizeof(*scanner_globals_p));
}
/* }}} */

static void module_destructor_zval(zval *zv) /* {{{ */
{
	zend_module_entry *module = (zend_module_entry*)Z_PTR_P(zv);

	module_destructor(module);
	free(module);
}
/* }}} */

static zend_bool php_auto_globals_create_globals(zend_string *name) /* {{{ */
{
	zval globals;

	/* IS_ARRAY, but with ref-counter 1 and not IS_TYPE_REFCOUNTED */
	ZVAL_ARR(&globals, &EG(symbol_table));
	Z_TYPE_FLAGS_P(&globals) = 0;
	ZVAL_NEW_REF(&globals, &globals);
	zend_hash_update(&EG(symbol_table), name, &globals);
	return 0;
}
/* }}} */

int zend_startup(zend_utility_functions *utility_functions) /* {{{ */
{
#ifdef ZTS
	zend_compiler_globals *compiler_globals;
	zend_executor_globals *executor_globals;
	extern ZEND_API ts_rsrc_id ini_scanner_globals_id;
	extern ZEND_API ts_rsrc_id language_scanner_globals_id;
#else
	extern zend_ini_scanner_globals ini_scanner_globals;
	extern zend_php_scanner_globals language_scanner_globals;
#endif

	zend_cpu_startup();

#ifdef ZEND_WIN32
	php_win32_cp_set_by_id(65001);
#endif

	start_memory_manager();

	virtual_cwd_startup(); /* Could use shutdown to free the main cwd but it would just slow it down for CGI */

#if defined(__FreeBSD__) || defined(__DragonFly__)
	/* FreeBSD and DragonFly floating point precision fix */
	fpsetmask(0);
#endif

	zend_startup_strtod();
	zend_startup_extensions_mechanism();

	/* Set up utility functions and values */
	zend_error_cb = utility_functions->error_function;
	zend_printf = utility_functions->printf_function;
	zend_write = (zend_write_func_t) utility_functions->write_function;
	zend_fopen = utility_functions->fopen_function;
	if (!zend_fopen) {
		zend_fopen = zend_fopen_wrapper;
	}
	zend_stream_open_function = utility_functions->stream_open_function;
	zend_message_dispatcher_p = utility_functions->message_handler;
	zend_get_configuration_directive_p = utility_functions->get_configuration_directive;
	zend_ticks_function = utility_functions->ticks_function;
	zend_on_timeout = utility_functions->on_timeout;
	zend_printf_to_smart_string = utility_functions->printf_to_smart_string_function;
	zend_printf_to_smart_str = utility_functions->printf_to_smart_str_function;
	zend_getenv = utility_functions->getenv_function;
	zend_resolve_path = utility_functions->resolve_path_function;

	zend_interrupt_function = NULL;

#if HAVE_DTRACE
/* build with dtrace support */
	{
		char *tmp = getenv("USE_ZEND_DTRACE");

		if (tmp && zend_atoi(tmp, 0)) {
			zend_dtrace_enabled = 1;
			zend_compile_file = dtrace_compile_file;
			zend_execute_ex = dtrace_execute_ex;
			zend_execute_internal = dtrace_execute_internal;
		} else {
			zend_compile_file = compile_file;
			zend_execute_ex = execute_ex;
			zend_execute_internal = NULL;
		}
	}
#else
	zend_compile_file = compile_file;
	zend_execute_ex = execute_ex;
	zend_execute_internal = NULL;
#endif /* HAVE_DTRACE */
	zend_compile_string = compile_string;
	zend_throw_exception_hook = NULL;

	/* Set up the default garbage collection implementation. */
	gc_collect_cycles = zend_gc_collect_cycles;

	zend_vm_init();

	/* set up version */
	zend_version_info = strdup(ZEND_CORE_VERSION_INFO);
	zend_version_info_length = sizeof(ZEND_CORE_VERSION_INFO) - 1;

	GLOBAL_FUNCTION_TABLE = (HashTable *) malloc(sizeof(HashTable));
	GLOBAL_CLASS_TABLE = (HashTable *) malloc(sizeof(HashTable));
	GLOBAL_AUTO_GLOBALS_TABLE = (HashTable *) malloc(sizeof(HashTable));
	GLOBAL_CONSTANTS_TABLE = (HashTable *) malloc(sizeof(HashTable));

	zend_hash_init_ex(GLOBAL_FUNCTION_TABLE, 1024, NULL, ZEND_FUNCTION_DTOR, 1, 0);
	zend_hash_init_ex(GLOBAL_CLASS_TABLE, 64, NULL, ZEND_CLASS_DTOR, 1, 0);
	zend_hash_init_ex(GLOBAL_AUTO_GLOBALS_TABLE, 8, NULL, auto_global_dtor, 1, 0);
	zend_hash_init_ex(GLOBAL_CONSTANTS_TABLE, 128, NULL, ZEND_CONSTANT_DTOR, 1, 0);

	zend_hash_init_ex(&module_registry, 32, NULL, module_destructor_zval, 1, 0);
	zend_init_rsrc_list_dtors();

#ifdef ZTS
	ts_allocate_fast_id(&compiler_globals_id, &compiler_globals_offset, sizeof(zend_compiler_globals), (ts_allocate_ctor) compiler_globals_ctor, (ts_allocate_dtor) compiler_globals_dtor);
	ts_allocate_fast_id(&executor_globals_id, &executor_globals_offset, sizeof(zend_executor_globals), (ts_allocate_ctor) executor_globals_ctor, (ts_allocate_dtor) executor_globals_dtor);
	ts_allocate_fast_id(&language_scanner_globals_id, &language_scanner_globals_offset, sizeof(zend_php_scanner_globals), (ts_allocate_ctor) php_scanner_globals_ctor, NULL);
	ts_allocate_fast_id(&ini_scanner_globals_id, &ini_scanner_globals_offset, sizeof(zend_ini_scanner_globals), (ts_allocate_ctor) ini_scanner_globals_ctor, NULL);
	compiler_globals = ts_resource(compiler_globals_id);
	executor_globals = ts_resource(executor_globals_id);

	compiler_globals_dtor(compiler_globals);
	compiler_globals->in_compilation = 0;
	compiler_globals->function_table = (HashTable *) malloc(sizeof(HashTable));
	compiler_globals->class_table = (HashTable *) malloc(sizeof(HashTable));

	*compiler_globals->function_table = *GLOBAL_FUNCTION_TABLE;
	*compiler_globals->class_table = *GLOBAL_CLASS_TABLE;
	compiler_globals->auto_globals = GLOBAL_AUTO_GLOBALS_TABLE;

	zend_hash_destroy(executor_globals->zend_constants);
	*executor_globals->zend_constants = *GLOBAL_CONSTANTS_TABLE;
#else
	ini_scanner_globals_ctor(&ini_scanner_globals);
	php_scanner_globals_ctor(&language_scanner_globals);
	zend_set_default_compile_time_values();
#ifdef ZEND_WIN32
	zend_get_windows_version_info(&EG(windows_version_info));
#endif
# if ZEND_MAP_PTR_KIND == ZEND_MAP_PTR_KIND_PTR
		/* Create a map region, used for indirect pointers from shared to
		 * process memory. It's allocatred once and never resized.
		 * All processes must map it into the same address space.
		 */
		CG(map_ptr_size) = 1024 * 1024; // TODO: initial size ???
		CG(map_ptr_last) = 0;
		CG(map_ptr_base) = pemalloc(CG(map_ptr_size) * sizeof(void*), 1);
# elif ZEND_MAP_PTR_KIND == ZEND_MAP_PTR_KIND_PTR_OR_OFFSET
		/* Map region is going to be created and resized at run-time. */
		CG(map_ptr_base) = NULL;
		CG(map_ptr_size) = 0;
		CG(map_ptr_last) = 0;
# else
#  error "Unknown ZEND_MAP_PTR_KIND"
# endif
#endif
	EG(error_reporting) = E_ALL & ~E_NOTICE;

	zend_interned_strings_init();
	zend_startup_builtin_functions();
	zend_register_standard_constants();
	zend_register_auto_global(zend_string_init_interned("GLOBALS", sizeof("GLOBALS") - 1, 1), 1, php_auto_globals_create_globals);

#ifndef ZTS
	zend_init_rsrc_plist();
	zend_init_exception_op();
	zend_init_call_trampoline_op();
#endif

	zend_ini_startup();

#ifdef ZEND_WIN32
	/* Uses INI settings, so needs to be run after it. */
	php_win32_cp_setup();
#endif

#ifdef ZTS
	tsrm_set_new_thread_end_handler(zend_new_thread_end_handler);
	tsrm_set_shutdown_handler(zend_interned_strings_dtor);
#endif

	return SUCCESS;
}
/* }}} */

void zend_register_standard_ini_entries(void) /* {{{ */
{
	int module_number = 0;

	REGISTER_INI_ENTRIES();
}
/* }}} */

static void zend_resolve_property_types(void) /* {{{ */
{
	zend_class_entry *ce;
	zend_property_info *prop_info;

	ZEND_HASH_FOREACH_PTR(CG(class_table), ce) {
		if (ce->type != ZEND_INTERNAL_CLASS) {
			continue;
		}

		if (UNEXPECTED(ZEND_CLASS_HAS_TYPE_HINTS(ce))) {
			ZEND_HASH_FOREACH_PTR(&ce->properties_info, prop_info) {
				if (ZEND_TYPE_IS_NAME(prop_info->type)) {
					zend_string *type_name = ZEND_TYPE_NAME(prop_info->type);
					zend_string *lc_type_name = zend_string_tolower(type_name);
					zend_class_entry *prop_ce = zend_hash_find_ptr(CG(class_table), lc_type_name);

					ZEND_ASSERT(prop_ce && prop_ce->type == ZEND_INTERNAL_CLASS);
					prop_info->type = ZEND_TYPE_ENCODE_CE(prop_ce, ZEND_TYPE_ALLOW_NULL(prop_info->type));
					zend_string_release(lc_type_name);
					zend_string_release(type_name);
				}
			} ZEND_HASH_FOREACH_END();
		}
		ce->ce_flags |= ZEND_ACC_PROPERTY_TYPES_RESOLVED;
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* Unlink the global (r/o) copies of the class, function and constant tables,
 * and use a fresh r/w copy for the startup thread
 */
int zend_post_startup(void) /* {{{ */
{
#ifdef ZTS
	zend_encoding **script_encoding_list;

	zend_compiler_globals *compiler_globals = ts_resource(compiler_globals_id);
	zend_executor_globals *executor_globals = ts_resource(executor_globals_id);
#endif

	zend_resolve_property_types();

	if (zend_post_startup_cb) {
		int (*cb)(void) = zend_post_startup_cb;

		zend_post_startup_cb = NULL;
		if (cb() != SUCCESS) {
			return FAILURE;
		}
	}

#ifdef ZTS
	*GLOBAL_FUNCTION_TABLE = *compiler_globals->function_table;
	*GLOBAL_CLASS_TABLE = *compiler_globals->class_table;
	*GLOBAL_CONSTANTS_TABLE = *executor_globals->zend_constants;
	global_map_ptr_last = compiler_globals->map_ptr_last;

	short_tags_default = CG(short_tags);
	compiler_options_default = CG(compiler_options);

	zend_destroy_rsrc_list(&EG(persistent_list));
	free(compiler_globals->function_table);
	compiler_globals->function_table = NULL;
	free(compiler_globals->class_table);
	compiler_globals->class_table = NULL;
	free(compiler_globals->map_ptr_base);
	compiler_globals->map_ptr_base = NULL;
	if ((script_encoding_list = (zend_encoding **)compiler_globals->script_encoding_list)) {
		compiler_globals_ctor(compiler_globals);
		compiler_globals->script_encoding_list = (const zend_encoding **)script_encoding_list;
	} else {
		compiler_globals_ctor(compiler_globals);
	}
	free(EG(zend_constants));
	EG(zend_constants) = NULL;

	executor_globals_ctor(executor_globals);
	global_persistent_list = &EG(persistent_list);
	zend_copy_ini_directives();
#else
	global_map_ptr_last = CG(map_ptr_last);
#endif

	return SUCCESS;
}
/* }}} */

void zend_shutdown(void) /* {{{ */
{
	zend_vm_dtor();

	zend_destroy_rsrc_list(&EG(persistent_list));
	zend_destroy_modules();

	virtual_cwd_deactivate();
	virtual_cwd_shutdown();

	zend_hash_destroy(GLOBAL_FUNCTION_TABLE);
	zend_hash_destroy(GLOBAL_CLASS_TABLE);

	zend_hash_destroy(GLOBAL_AUTO_GLOBALS_TABLE);
	free(GLOBAL_AUTO_GLOBALS_TABLE);

	zend_shutdown_extensions();
	free(zend_version_info);

	free(GLOBAL_FUNCTION_TABLE);
	free(GLOBAL_CLASS_TABLE);

	zend_hash_destroy(GLOBAL_CONSTANTS_TABLE);
	free(GLOBAL_CONSTANTS_TABLE);
	zend_shutdown_strtod();

#ifdef ZTS
	GLOBAL_FUNCTION_TABLE = NULL;
	GLOBAL_CLASS_TABLE = NULL;
	GLOBAL_AUTO_GLOBALS_TABLE = NULL;
	GLOBAL_CONSTANTS_TABLE = NULL;
	ts_free_id(executor_globals_id);
	ts_free_id(compiler_globals_id);
#else
	if (CG(map_ptr_base)) {
		free(CG(map_ptr_base));
		CG(map_ptr_base) = NULL;
		CG(map_ptr_size) = 0;
	}
	if (CG(script_encoding_list)) {
		free(CG(script_encoding_list));
		CG(script_encoding_list) = NULL;
		CG(script_encoding_list_size) = 0;
	}
#endif
	zend_destroy_rsrc_list_dtors();
}
/* }}} */

void zend_set_utility_values(zend_utility_values *utility_values) /* {{{ */
{
	zend_uv = *utility_values;
}
/* }}} */

/* this should be compatible with the standard zenderror */
ZEND_COLD void zenderror(const char *error) /* {{{ */
{
	CG(parse_error) = 0;

	if (EG(exception)) {
		/* An exception was thrown in the lexer, don't throw another in the parser. */
		return;
	}

	zend_throw_exception(zend_ce_parse_error, error, 0);
}
/* }}} */

BEGIN_EXTERN_C()
ZEND_API ZEND_COLD ZEND_NORETURN void _zend_bailout(const char *filename, uint32_t lineno) /* {{{ */
{

	if (!EG(bailout)) {
		zend_output_debug_string(1, "%s(%d) : Bailed out without a bailout address!", filename, lineno);
		exit(-1);
	}
	gc_protect(1);
	CG(unclean_shutdown) = 1;
	CG(active_class_entry) = NULL;
	CG(in_compilation) = 0;
	EG(current_execute_data) = NULL;
	LONGJMP(*EG(bailout), FAILURE);
}
/* }}} */
END_EXTERN_C()

ZEND_API void zend_append_version_info(const zend_extension *extension) /* {{{ */
{
	char *new_info;
	uint32_t new_info_length;

	new_info_length = (uint32_t)(sizeof("    with  v, , by \n")
						+ strlen(extension->name)
						+ strlen(extension->version)
						+ strlen(extension->copyright)
						+ strlen(extension->author));

	new_info = (char *) malloc(new_info_length + 1);

	snprintf(new_info, new_info_length, "    with %s v%s, %s, by %s\n", extension->name, extension->version, extension->copyright, extension->author);

	zend_version_info = (char *) realloc(zend_version_info, zend_version_info_length+new_info_length + 1);
	strncat(zend_version_info, new_info, new_info_length);
	zend_version_info_length += new_info_length;
	free(new_info);
}
/* }}} */

ZEND_API char *get_zend_version(void) /* {{{ */
{
	return zend_version_info;
}
/* }}} */

ZEND_API void zend_activate(void) /* {{{ */
{
#ifdef ZTS
	virtual_cwd_activate();
#endif
	gc_reset();
	init_compiler();
	init_executor();
	startup_scanner();
	if (CG(map_ptr_last)) {
		memset(CG(map_ptr_base), 0, CG(map_ptr_last) * sizeof(void*));
	}
}
/* }}} */

void zend_call_destructors(void) /* {{{ */
{
	zend_try {
		shutdown_destructors();
	} zend_end_try();
}
/* }}} */

ZEND_API void zend_deactivate(void) /* {{{ */
{
	/* we're no longer executing anything */
	EG(current_execute_data) = NULL;

	zend_try {
		shutdown_scanner();
	} zend_end_try();

	/* shutdown_executor() takes care of its own bailout handling */
	shutdown_executor();

	zend_try {
		zend_ini_deactivate();
	} zend_end_try();

	zend_try {
		shutdown_compiler();
	} zend_end_try();

	zend_destroy_rsrc_list(&EG(regular_list));

#if GC_BENCH
	fprintf(stderr, "GC Statistics\n");
	fprintf(stderr, "-------------\n");
	fprintf(stderr, "Runs:               %d\n", GC_G(gc_runs));
	fprintf(stderr, "Collected:          %d\n", GC_G(collected));
	fprintf(stderr, "Root buffer length: %d\n", GC_G(root_buf_length));
	fprintf(stderr, "Root buffer peak:   %d\n\n", GC_G(root_buf_peak));
	fprintf(stderr, "      Possible            Remove from  Marked\n");
	fprintf(stderr, "        Root    Buffered     buffer     grey\n");
	fprintf(stderr, "      --------  --------  -----------  ------\n");
	fprintf(stderr, "ZVAL  %8d  %8d  %9d  %8d\n", GC_G(zval_possible_root), GC_G(zval_buffered), GC_G(zval_remove_from_buffer), GC_G(zval_marked_grey));
#endif
}
/* }}} */

BEGIN_EXTERN_C()
ZEND_API void zend_message_dispatcher(zend_long message, const void *data) /* {{{ */
{
	if (zend_message_dispatcher_p) {
		zend_message_dispatcher_p(message, data);
	}
}
/* }}} */
END_EXTERN_C()

ZEND_API zval *zend_get_configuration_directive(zend_string *name) /* {{{ */
{
	if (zend_get_configuration_directive_p) {
		return zend_get_configuration_directive_p(name);
	} else {
		return NULL;
	}
}
/* }}} */

#define SAVE_STACK(stack) do { \
		if (CG(stack).top) { \
			memcpy(&stack, &CG(stack), sizeof(zend_stack)); \
			CG(stack).top = CG(stack).max = 0; \
			CG(stack).elements = NULL; \
		} else { \
			stack.top = 0; \
		} \
	} while (0)

#define RESTORE_STACK(stack) do { \
		if (stack.top) { \
			zend_stack_destroy(&CG(stack)); \
			memcpy(&CG(stack), &stack, sizeof(zend_stack)); \
		} \
	} while (0)

static ZEND_COLD void zend_error_va_list(
		int type, const char *error_filename, uint32_t error_lineno,
		const char *format, va_list args)
{
	va_list usr_copy;
	zval params[5];
	zval retval;
	zval orig_user_error_handler;
	zend_bool in_compilation;
	zend_class_entry *saved_class_entry;
	zend_stack loop_var_stack;
	zend_stack delayed_oplines_stack;
	zend_array *symbol_table;
	zend_class_entry *orig_fake_scope;

	/* Report about uncaught exception in case of fatal errors */
	if (EG(exception)) {
		zend_execute_data *ex;
		const zend_op *opline;

		switch (type) {
			case E_CORE_ERROR:
			case E_ERROR:
			case E_RECOVERABLE_ERROR:
			case E_PARSE:
			case E_COMPILE_ERROR:
			case E_USER_ERROR:
				ex = EG(current_execute_data);
				opline = NULL;
				while (ex && (!ex->func || !ZEND_USER_CODE(ex->func->type))) {
					ex = ex->prev_execute_data;
				}
				if (ex && ex->opline->opcode == ZEND_HANDLE_EXCEPTION &&
				    EG(opline_before_exception)) {
					opline = EG(opline_before_exception);
				}
				zend_exception_error(EG(exception), E_WARNING);
				EG(exception) = NULL;
				if (opline) {
					ex->opline = opline;
				}
				break;
			default:
				break;
		}
	}

#ifdef HAVE_DTRACE
	if (DTRACE_ERROR_ENABLED()) {
		char *dtrace_error_buffer;
		va_copy(usr_copy, args);
		zend_vspprintf(&dtrace_error_buffer, 0, format, usr_copy);
		va_end(usr_copy);
		DTRACE_ERROR(dtrace_error_buffer, (char *)error_filename, error_lineno);
		efree(dtrace_error_buffer);
	}
#endif /* HAVE_DTRACE */

	/* if we don't have a user defined error handler */
	if (Z_TYPE(EG(user_error_handler)) == IS_UNDEF
		|| !(EG(user_error_handler_error_reporting) & type)
		|| EG(error_handling) != EH_NORMAL) {
		zend_error_cb(type, error_filename, error_lineno, format, args);
	} else switch (type) {
		case E_ERROR:
		case E_PARSE:
		case E_CORE_ERROR:
		case E_CORE_WARNING:
		case E_COMPILE_ERROR:
		case E_COMPILE_WARNING:
			/* The error may not be safe to handle in user-space */
			zend_error_cb(type, error_filename, error_lineno, format, args);
			break;
		default:
			/* Handle the error in user space */
			va_copy(usr_copy, args);
			ZVAL_STR(&params[1], zend_vstrpprintf(0, format, usr_copy));
			va_end(usr_copy);

			ZVAL_LONG(&params[0], type);

			if (error_filename) {
				ZVAL_STRING(&params[2], error_filename);
			} else {
				ZVAL_NULL(&params[2]);
			}

			ZVAL_LONG(&params[3], error_lineno);

			symbol_table = zend_rebuild_symbol_table();

			/* during shutdown the symbol table table can be still null */
			if (!symbol_table) {
				ZVAL_NULL(&params[4]);
			} else {
				ZVAL_ARR(&params[4], zend_array_dup(symbol_table));
			}

			ZVAL_COPY_VALUE(&orig_user_error_handler, &EG(user_error_handler));
			ZVAL_UNDEF(&EG(user_error_handler));

			/* User error handler may include() additinal PHP files.
			 * If an error was generated during comilation PHP will compile
			 * such scripts recursively, but some CG() variables may be
			 * inconsistent. */

			in_compilation = CG(in_compilation);
			if (in_compilation) {
				saved_class_entry = CG(active_class_entry);
				CG(active_class_entry) = NULL;
				SAVE_STACK(loop_var_stack);
				SAVE_STACK(delayed_oplines_stack);
				CG(in_compilation) = 0;
			}

			orig_fake_scope = EG(fake_scope);
			EG(fake_scope) = NULL;

			if (call_user_function(CG(function_table), NULL, &orig_user_error_handler, &retval, 5, params) == SUCCESS) {
				if (Z_TYPE(retval) != IS_UNDEF) {
					if (Z_TYPE(retval) == IS_FALSE) {
						zend_error_cb(type, error_filename, error_lineno, format, args);
					}
					zval_ptr_dtor(&retval);
				}
			} else if (!EG(exception)) {
				/* The user error handler failed, use built-in error handler */
				zend_error_cb(type, error_filename, error_lineno, format, args);
			}

			EG(fake_scope) = orig_fake_scope;

			if (in_compilation) {
				CG(active_class_entry) = saved_class_entry;
				RESTORE_STACK(loop_var_stack);
				RESTORE_STACK(delayed_oplines_stack);
				CG(in_compilation) = 1;
			}

			zval_ptr_dtor(&params[4]);
			zval_ptr_dtor(&params[2]);
			zval_ptr_dtor(&params[1]);

			if (Z_TYPE(EG(user_error_handler)) == IS_UNDEF) {
				ZVAL_COPY_VALUE(&EG(user_error_handler), &orig_user_error_handler);
			} else {
				zval_ptr_dtor(&orig_user_error_handler);
			}
			break;
	}

	if (type == E_PARSE) {
		/* eval() errors do not affect exit_status */
		if (!(EG(current_execute_data) &&
			EG(current_execute_data)->func &&
			ZEND_USER_CODE(EG(current_execute_data)->func->type) &&
			EG(current_execute_data)->opline->opcode == ZEND_INCLUDE_OR_EVAL &&
			EG(current_execute_data)->opline->extended_value == ZEND_EVAL)) {
			EG(exit_status) = 255;
		}
	}
}
/* }}} */

static ZEND_COLD void get_filename_lineno(int type, const char **filename, uint32_t *lineno) {
	/* Obtain relevant filename and lineno */
	switch (type) {
		case E_CORE_ERROR:
		case E_CORE_WARNING:
			*filename = NULL;
			*lineno = 0;
			break;
		case E_PARSE:
		case E_COMPILE_ERROR:
		case E_COMPILE_WARNING:
		case E_ERROR:
		case E_NOTICE:
		case E_STRICT:
		case E_DEPRECATED:
		case E_WARNING:
		case E_USER_ERROR:
		case E_USER_WARNING:
		case E_USER_NOTICE:
		case E_USER_DEPRECATED:
		case E_RECOVERABLE_ERROR:
			if (zend_is_compiling()) {
				*filename = ZSTR_VAL(zend_get_compiled_filename());
				*lineno = zend_get_compiled_lineno();
			} else if (zend_is_executing()) {
				*filename = zend_get_executed_filename();
				if ((*filename)[0] == '[') { /* [no active file] */
					*filename = NULL;
					*lineno = 0;
				} else {
					*lineno = zend_get_executed_lineno();
				}
			} else {
				*filename = NULL;
				*lineno = 0;
			}
			break;
		default:
			*filename = NULL;
			*lineno = 0;
			break;
	}
	if (!*filename) {
		*filename = "Unknown";
	}
}

ZEND_API ZEND_COLD void zend_error_at(
		int type, const char *filename, uint32_t lineno, const char *format, ...) {
	va_list args;

	if (!filename) {
		uint32_t dummy_lineno;
		get_filename_lineno(type, &filename, &dummy_lineno);
	}

	va_start(args, format);
	zend_error_va_list(type, filename, lineno, format, args);
	va_end(args);
}

ZEND_API ZEND_COLD void zend_error(int type, const char *format, ...) {
	const char *filename;
	uint32_t lineno;
	va_list args;

	get_filename_lineno(type, &filename, &lineno);
	va_start(args, format);
	zend_error_va_list(type, filename, lineno, format, args);
	va_end(args);
}

ZEND_API ZEND_COLD ZEND_NORETURN void zend_error_at_noreturn(
		int type, const char *filename, uint32_t lineno, const char *format, ...)
{
	va_list args;

	if (!filename) {
		uint32_t dummy_lineno;
		get_filename_lineno(type, &filename, &dummy_lineno);
	}

	va_start(args, format);
	zend_error_va_list(type, filename, lineno, format, args);
	va_end(args);
	/* Should never reach this. */
	abort();
}
/* }}} */

ZEND_API ZEND_COLD ZEND_NORETURN void zend_error_noreturn(int type, const char *format, ...)
{
	const char *filename;
	uint32_t lineno;
	va_list args;

	get_filename_lineno(type, &filename, &lineno);
	va_start(args, format);
	zend_error_va_list(type, filename, lineno, format, args);
	va_end(args);
	/* Should never reach this. */
	abort();
}
/* }}} */

ZEND_API ZEND_COLD void zend_throw_error(zend_class_entry *exception_ce, const char *format, ...) /* {{{ */
{
	va_list va;
	char *message = NULL;

	if (exception_ce) {
		if (!instanceof_function(exception_ce, zend_ce_error)) {
			zend_error(E_NOTICE, "Error exceptions must be derived from Error");
			exception_ce = zend_ce_error;
		}
	} else {
		exception_ce = zend_ce_error;
	}

	/* Marker used to disable exception generation during preloading. */
	if (EG(exception) == (void*)(uintptr_t)-1) {
		return;
	}

	va_start(va, format);
	zend_vspprintf(&message, 0, format, va);

	//TODO: we can't convert compile-time errors to exceptions yet???
	if (EG(current_execute_data) && !CG(in_compilation)) {
		zend_throw_exception(exception_ce, message, 0);
	} else {
		zend_error(E_ERROR, "%s", message);
	}

	efree(message);
	va_end(va);
}
/* }}} */

ZEND_API ZEND_COLD void zend_type_error(const char *format, ...) /* {{{ */
{
	va_list va;
	char *message = NULL;

	va_start(va, format);
	zend_vspprintf(&message, 0, format, va);
	zend_throw_exception(zend_ce_type_error, message, 0);
	efree(message);
	va_end(va);
} /* }}} */

ZEND_API ZEND_COLD void zend_internal_type_error(zend_bool throw_exception, const char *format, ...) /* {{{ */
{
	va_list va;
	char *message = NULL;

	va_start(va, format);
	zend_vspprintf(&message, 0, format, va);
	if (throw_exception) {
		zend_throw_exception(zend_ce_type_error, message, 0);
	} else {
		zend_error(E_WARNING, "%s", message);
	}
	efree(message);

	va_end(va);
} /* }}} */

ZEND_API ZEND_COLD void zend_internal_argument_count_error(zend_bool throw_exception, const char *format, ...) /* {{{ */
{
	va_list va;
	char *message = NULL;

	va_start(va, format);
	zend_vspprintf(&message, 0, format, va);
	if (throw_exception) {
		zend_throw_exception(zend_ce_argument_count_error, message, 0);
	} else {
		zend_error(E_WARNING, "%s", message);
	}
	efree(message);

	va_end(va);
} /* }}} */

ZEND_API ZEND_COLD void zend_output_debug_string(zend_bool trigger_break, const char *format, ...) /* {{{ */
{
#if ZEND_DEBUG
	va_list args;

	va_start(args, format);
#	ifdef ZEND_WIN32
	{
		char output_buf[1024];

		vsnprintf(output_buf, 1024, format, args);
		OutputDebugString(output_buf);
		OutputDebugString("\n");
		if (trigger_break && IsDebuggerPresent()) {
			DebugBreak();
		}
	}
#	else
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n");
#	endif
	va_end(args);
#endif
}
/* }}} */

ZEND_API ZEND_COLD void zend_user_exception_handler(void) /* {{{ */
{
	zval orig_user_exception_handler;
	zval params[1], retval2;
	zend_object *old_exception;
	old_exception = EG(exception);
	EG(exception) = NULL;
	ZVAL_OBJ(&params[0], old_exception);
	ZVAL_COPY_VALUE(&orig_user_exception_handler, &EG(user_exception_handler));

	if (call_user_function(CG(function_table), NULL, &orig_user_exception_handler, &retval2, 1, params) == SUCCESS) {
		zval_ptr_dtor(&retval2);
		if (EG(exception)) {
			OBJ_RELEASE(EG(exception));
			EG(exception) = NULL;
		}
		OBJ_RELEASE(old_exception);
	} else {
		EG(exception) = old_exception;
	}
} /* }}} */

ZEND_API int zend_execute_scripts(int type, zval *retval, int file_count, ...) /* {{{ */
{
	va_list files;
	int i;
	zend_file_handle *file_handle;
	zend_op_array *op_array;

	va_start(files, file_count);
	for (i = 0; i < file_count; i++) {
		file_handle = va_arg(files, zend_file_handle *);
		if (!file_handle) {
			continue;
		}

		op_array = zend_compile_file(file_handle, type);
		if (file_handle->opened_path) {
			zend_hash_add_empty_element(&EG(included_files), file_handle->opened_path);
		}
		zend_destroy_file_handle(file_handle);
		if (op_array) {
			zend_execute(op_array, retval);
			zend_exception_restore();
			if (UNEXPECTED(EG(exception))) {
				if (Z_TYPE(EG(user_exception_handler)) != IS_UNDEF) {
					zend_user_exception_handler();
				}
				if (EG(exception)) {
					zend_exception_error(EG(exception), E_ERROR);
				}
			}
			destroy_op_array(op_array);
			efree_size(op_array, sizeof(zend_op_array));
		} else if (type==ZEND_REQUIRE) {
			va_end(files);
			return FAILURE;
		}
	}
	va_end(files);

	return SUCCESS;
}
/* }}} */

#define COMPILED_STRING_DESCRIPTION_FORMAT "%s(%d) : %s"

ZEND_API char *zend_make_compiled_string_description(const char *name) /* {{{ */
{
	const char *cur_filename;
	int cur_lineno;
	char *compiled_string_description;

	if (zend_is_compiling()) {
		cur_filename = ZSTR_VAL(zend_get_compiled_filename());
		cur_lineno = zend_get_compiled_lineno();
	} else if (zend_is_executing()) {
		cur_filename = zend_get_executed_filename();
		cur_lineno = zend_get_executed_lineno();
	} else {
		cur_filename = "Unknown";
		cur_lineno = 0;
	}

	zend_spprintf(&compiled_string_description, 0, COMPILED_STRING_DESCRIPTION_FORMAT, cur_filename, cur_lineno, name);
	return compiled_string_description;
}
/* }}} */

void free_estring(char **str_p) /* {{{ */
{
	efree(*str_p);
}
/* }}} */

ZEND_API void zend_map_ptr_reset(void)
{
	CG(map_ptr_last) = global_map_ptr_last;
}

ZEND_API void *zend_map_ptr_new(void)
{
	void **ptr;

	if (CG(map_ptr_last) >= CG(map_ptr_size)) {
#if ZEND_MAP_PTR_KIND == ZEND_MAP_PTR_KIND_PTR
		// TODO: error ???
		ZEND_ASSERT(0);
#elif ZEND_MAP_PTR_KIND == ZEND_MAP_PTR_KIND_PTR_OR_OFFSET
		/* Grow map_ptr table */
		CG(map_ptr_size) = ZEND_MM_ALIGNED_SIZE_EX(CG(map_ptr_last) + 1, 4096);
		CG(map_ptr_base) = perealloc(CG(map_ptr_base), CG(map_ptr_size) * sizeof(void*), 1);
#else
# error "Unknown ZEND_MAP_PTR_KIND"
#endif
	}
	ptr = (void**)CG(map_ptr_base) + CG(map_ptr_last);
	*ptr = NULL;
	CG(map_ptr_last)++;
#if ZEND_MAP_PTR_KIND == ZEND_MAP_PTR_KIND_PTR
	return ptr;
#elif ZEND_MAP_PTR_KIND == ZEND_MAP_PTR_KIND_PTR_OR_OFFSET
	return ZEND_MAP_PTR_PTR2OFFSET(ptr);
#else
# error "Unknown ZEND_MAP_PTR_KIND"
#endif
}

ZEND_API void zend_map_ptr_extend(size_t last)
{
	if (last > CG(map_ptr_last)) {
		void **ptr;

		if (last >= CG(map_ptr_size)) {
#if ZEND_MAP_PTR_KIND == ZEND_MAP_PTR_KIND_PTR
			/* This may never happen */
			ZEND_ASSERT(0);
#elif ZEND_MAP_PTR_KIND == ZEND_MAP_PTR_KIND_PTR_OR_OFFSET
			/* Grow map_ptr table */
			CG(map_ptr_size) = ZEND_MM_ALIGNED_SIZE_EX(last, 4096);
			CG(map_ptr_base) = perealloc(CG(map_ptr_base), CG(map_ptr_size) * sizeof(void*), 1);
#else
# error "Unknown ZEND_MAP_PTR_KIND"
#endif
		}
		ptr = (void**)CG(map_ptr_base) + CG(map_ptr_last);
		memset(ptr, 0, (last - CG(map_ptr_last)) * sizeof(void*));
		CG(map_ptr_last) = last;
	}
}
