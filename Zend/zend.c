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
#include "zend_attributes.h"
#include "zend_observer.h"
#include "zend_fibers.h"
#include "zend_call_stack.h"
#include "zend_max_execution_timer.h"
#include "zend_hrtime.h"
#include "Optimizer/zend_optimizer.h"
#include "php.h"
#include "php_globals.h"

// FIXME: Breaks the declaration of the function below
#undef zenderror

static size_t global_map_ptr_last = 0;
static bool startup_done = false;

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
TSRMLS_MAIN_CACHE_DEFINE()
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
ZEND_API bool zend_dtrace_enabled;

/* version information */
static char *zend_version_info;
static uint32_t zend_version_info_length;
#define ZEND_CORE_VERSION_INFO	"Zend Engine v" ZEND_VERSION ", Copyright (c) Zend Technologies\n"
#define PRINT_ZVAL_INDENT 4

/* true multithread-shared globals */
ZEND_API zend_class_entry *zend_standard_class_def = NULL;
ZEND_API size_t (*zend_printf)(const char *format, ...);
ZEND_API zend_write_func_t zend_write;
ZEND_API FILE *(*zend_fopen)(zend_string *filename, zend_string **opened_path);
ZEND_API zend_result (*zend_stream_open_function)(zend_file_handle *handle);
ZEND_API void (*zend_ticks_function)(int ticks);
ZEND_API void (*zend_interrupt_function)(zend_execute_data *execute_data);
ZEND_API void (*zend_error_cb)(int type, zend_string *error_filename, const uint32_t error_lineno, zend_string *message);
void (*zend_printf_to_smart_string)(smart_string *buf, const char *format, va_list ap);
void (*zend_printf_to_smart_str)(smart_str *buf, const char *format, va_list ap);
ZEND_API char *(*zend_getenv)(const char *name, size_t name_len);
ZEND_API zend_string *(*zend_resolve_path)(zend_string *filename);
ZEND_API zend_result (*zend_post_startup_cb)(void) = NULL;
ZEND_API void (*zend_post_shutdown_cb)(void) = NULL;

/* This callback must be signal handler safe! */
void (*zend_on_timeout)(int seconds);

static void (*zend_message_dispatcher_p)(zend_long message, const void *data);
static zval *(*zend_get_configuration_directive_p)(zend_string *name);

#if ZEND_RC_DEBUG
ZEND_API bool zend_rc_debug = 0;
#endif

static ZEND_INI_MH(OnUpdateErrorReporting) /* {{{ */
{
	if (!new_value) {
		EG(error_reporting) = E_ALL;
	} else {
		EG(error_reporting) = atoi(ZSTR_VAL(new_value));
	}
	return SUCCESS;
}
/* }}} */

static ZEND_INI_MH(OnUpdateGCEnabled) /* {{{ */
{
	bool val;

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
	zend_long *p = (zend_long *) ZEND_INI_GET_ADDR();

	zend_long val = zend_ini_parse_quantity_warn(new_value, entry->name);

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

static ZEND_INI_MH(OnSetExceptionStringParamMaxLen) /* {{{ */
{
	zend_long i = ZEND_ATOL(ZSTR_VAL(new_value));
	if (i >= 0 && i <= 1000000) {
		EG(exception_string_param_max_len) = i;
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

#ifdef ZEND_CHECK_STACK_LIMIT
static ZEND_INI_MH(OnUpdateMaxAllowedStackSize) /* {{{ */
{
	zend_long size = zend_ini_parse_quantity_warn(new_value, entry->name);

	if (size < ZEND_MAX_ALLOWED_STACK_SIZE_UNCHECKED) {
		zend_error(E_WARNING, "Invalid \"%s\" setting. Value must be >= %d, but got " ZEND_LONG_FMT,
			ZSTR_VAL(entry->name), ZEND_MAX_ALLOWED_STACK_SIZE_UNCHECKED, size);
		return FAILURE;
	}

	EG(max_allowed_stack_size) = size;

	return SUCCESS;
}
/* }}} */

static ZEND_INI_MH(OnUpdateReservedStackSize) /* {{{ */
{
	zend_ulong size = zend_ini_parse_uquantity_warn(new_value, entry->name);

	/* Min value accounts for alloca, PCRE2 START_FRAMES_SIZE, and some buffer
	 * for normal function calls.
	 * We could reduce this on systems without alloca if we also add stack size
	 * checks before pcre2_match(). */
#ifdef ZEND_ALLOCA_MAX_SIZE
	zend_ulong min = ZEND_ALLOCA_MAX_SIZE + 16*1024;
#else
	zend_ulong min = 32*1024;
#endif

	if (size == 0) {
		size = min;
	} else if (size < min) {
		zend_error(E_WARNING, "Invalid \"%s\" setting. Value must be >= " ZEND_ULONG_FMT ", but got " ZEND_ULONG_FMT "\n",
			ZSTR_VAL(entry->name), min, size);
		return FAILURE;
	}

	EG(reserved_stack_size) = size;

	return SUCCESS;
}
/* }}} */
#endif /* ZEND_CHECK_STACK_LIMIT */

static ZEND_INI_MH(OnUpdateFiberStackSize) /* {{{ */
{
	if (new_value) {
		EG(fiber_stack_size) = zend_ini_parse_uquantity_warn(new_value, entry->name);
	} else {
		EG(fiber_stack_size) = ZEND_FIBER_DEFAULT_C_STACK_SIZE;
	}
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
	STD_ZEND_INI_ENTRY("zend.exception_string_param_max_len",	"15",	ZEND_INI_ALL,	OnSetExceptionStringParamMaxLen,	exception_string_param_max_len,		zend_executor_globals,	executor_globals)
	STD_ZEND_INI_ENTRY("fiber.stack_size",		NULL,			ZEND_INI_ALL,		OnUpdateFiberStackSize,		fiber_stack_size,	zend_executor_globals, 		executor_globals)
#ifdef ZEND_CHECK_STACK_LIMIT
	/* The maximum allowed call stack size. 0: auto detect, -1: no limit. For fibers, this is fiber.stack_size. */
	STD_ZEND_INI_ENTRY("zend.max_allowed_stack_size",	"0",	ZEND_INI_SYSTEM,	OnUpdateMaxAllowedStackSize,	max_allowed_stack_size,		zend_executor_globals,	executor_globals)
	/* Substracted from the max allowed stack size, as a buffer, when checking for overflow. 0: auto detect. */
	STD_ZEND_INI_ENTRY("zend.reserved_stack_size",	"0",	ZEND_INI_SYSTEM,	OnUpdateReservedStackSize,	reserved_stack_size,		zend_executor_globals,	executor_globals)
#endif

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

	return smart_str_extract(&buf);
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

static void print_hash(smart_str *buf, HashTable *ht, int indent, bool is_object) /* {{{ */
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

static void print_flat_hash(smart_str *buf, HashTable *ht) /* {{{ */
{
	zval *tmp;
	zend_string *string_key;
	zend_ulong num_key;
	int i = 0;

	ZEND_HASH_FOREACH_KEY_VAL_IND(ht, num_key, string_key, tmp) {
		if (i++ > 0) {
			smart_str_appendc(buf, ',');
		}
		smart_str_appendc(buf, '[');
		if (string_key) {
			smart_str_append(buf, string_key);
		} else {
			smart_str_append_unsigned(buf, num_key);
		}
		smart_str_appends(buf, "] => ");
		zend_print_flat_zval_r_to_buf(buf, tmp);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

ZEND_API bool zend_make_printable_zval(zval *expr, zval *expr_copy) /* {{{ */
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

void zend_print_flat_zval_r_to_buf(smart_str *buf, zval *expr) /* {{{ */
{
	switch (Z_TYPE_P(expr)) {
		case IS_ARRAY:
			smart_str_appends(buf, "Array (");
			if (!(GC_FLAGS(Z_ARRVAL_P(expr)) & GC_IMMUTABLE)) {
				if (GC_IS_RECURSIVE(Z_ARRVAL_P(expr))) {
					smart_str_appends(buf, " *RECURSION*");
					return;
				}
				GC_PROTECT_RECURSION(Z_ARRVAL_P(expr));
			}
			print_flat_hash(buf, Z_ARRVAL_P(expr));
			smart_str_appendc(buf, ')');
			GC_TRY_UNPROTECT_RECURSION(Z_ARRVAL_P(expr));
			break;
		case IS_OBJECT:
		{
			HashTable *properties;
			zend_string *class_name = Z_OBJ_HANDLER_P(expr, get_class_name)(Z_OBJ_P(expr));
			smart_str_append(buf, class_name);
			smart_str_appends(buf, " Object (");
			zend_string_release_ex(class_name, 0);

			if (GC_IS_RECURSIVE(Z_COUNTED_P(expr))) {
				smart_str_appends(buf, " *RECURSION*");
				return;
			}

			properties = Z_OBJPROP_P(expr);
			if (properties) {
				GC_PROTECT_RECURSION(Z_OBJ_P(expr));
				print_flat_hash(buf, properties);
				GC_UNPROTECT_RECURSION(Z_OBJ_P(expr));
			}
			smart_str_appendc(buf, ')');
			break;
		}
		case IS_REFERENCE:
			zend_print_flat_zval_r_to_buf(buf, Z_REFVAL_P(expr));
			break;
		case IS_STRING:
			smart_str_append(buf, Z_STR_P(expr));
			break;
		default:
		{
			zend_string *str = zval_get_string_func(expr);
			smart_str_append(buf, str);
			zend_string_release_ex(str, 0);
			break;
		}
	}
}
/* }}} */

ZEND_API void zend_print_flat_zval_r(zval *expr)
{
	smart_str buf = {0};
	zend_print_flat_zval_r_to_buf(&buf, expr);
	smart_str_0(&buf);
	zend_write(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
	smart_str_free(&buf);
}

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
			GC_TRY_UNPROTECT_RECURSION(Z_ARRVAL_P(expr));
			break;
		case IS_OBJECT:
			{
				HashTable *properties;

				zend_object *zobj = Z_OBJ_P(expr);
				uint32_t *guard = zend_get_recursion_guard(zobj);
				zend_string *class_name = Z_OBJ_HANDLER_P(expr, get_class_name)(zobj);
				smart_str_appends(buf, ZSTR_VAL(class_name));
				zend_string_release_ex(class_name, 0);

				if (!(zobj->ce->ce_flags & ZEND_ACC_ENUM)) {
					smart_str_appends(buf, " Object\n");
				} else {
					smart_str_appends(buf, " Enum");
					if (zobj->ce->enum_backing_type != IS_UNDEF) {
						smart_str_appendc(buf, ':');
						smart_str_appends(buf, zend_get_type_by_const(zobj->ce->enum_backing_type));
					}
					smart_str_appendc(buf, '\n');
				}

				if (ZEND_GUARD_OR_GC_IS_RECURSIVE(guard, DEBUG, zobj)) {
					smart_str_appends(buf, " *RECURSION*");
					return;
				}

				if ((properties = zend_get_properties_for(expr, ZEND_PROP_PURPOSE_DEBUG)) == NULL) {
					print_hash(buf, (HashTable*) &zend_empty_array, indent, 1);
					break;
				}

				ZEND_GUARD_OR_GC_PROTECT_RECURSION(guard, DEBUG, zobj);
				print_hash(buf, properties, indent, 1);
				ZEND_GUARD_OR_GC_UNPROTECT_RECURSION(guard, DEBUG, zobj);

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

static FILE *zend_fopen_wrapper(zend_string *filename, zend_string **opened_path) /* {{{ */
{
	if (opened_path) {
		*opened_path = zend_string_copy(filename);
	}
	return fopen(ZSTR_VAL(filename), "rb");
}
/* }}} */

#ifdef ZTS
static bool short_tags_default      = 1;
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
		ZEND_UNREACHABLE(); /* Should not happen as sizeof is used. */
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
	zend_hash_init(compiler_globals->function_table, 1024, NULL, ZEND_FUNCTION_DTOR, 1);
	zend_hash_copy(compiler_globals->function_table, global_function_table, NULL);
	compiler_globals->copied_functions_count = zend_hash_num_elements(compiler_globals->function_table);

	compiler_globals->class_table = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init(compiler_globals->class_table, 64, NULL, ZEND_CLASS_DTOR, 1);
	zend_hash_copy(compiler_globals->class_table, global_class_table, zend_class_add_ref);

	zend_set_default_compile_time_values();

	compiler_globals->auto_globals = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init(compiler_globals->auto_globals, 8, NULL, auto_global_dtor, 1);
	zend_hash_copy(compiler_globals->auto_globals, global_auto_globals_table, auto_global_copy_ctor);

	compiler_globals->script_encoding_list = NULL;
	compiler_globals->current_linking_class = NULL;

	/* Map region is going to be created and resized at run-time. */
	compiler_globals->map_ptr_real_base = NULL;
	compiler_globals->map_ptr_base = ZEND_MAP_PTR_BIASED_BASE(NULL);
	compiler_globals->map_ptr_size = 0;
	compiler_globals->map_ptr_last = global_map_ptr_last;
	if (compiler_globals->map_ptr_last) {
		/* Allocate map_ptr table */
		compiler_globals->map_ptr_size = ZEND_MM_ALIGNED_SIZE_EX(compiler_globals->map_ptr_last, 4096);
		void *base = pemalloc(compiler_globals->map_ptr_size * sizeof(void*), 1);
		compiler_globals->map_ptr_real_base = base;
		compiler_globals->map_ptr_base = ZEND_MAP_PTR_BIASED_BASE(base);
		memset(base, 0, compiler_globals->map_ptr_last * sizeof(void*));
	}
}
/* }}} */

static void compiler_globals_dtor(zend_compiler_globals *compiler_globals) /* {{{ */
{
	if (compiler_globals->function_table != GLOBAL_FUNCTION_TABLE) {
		uint32_t n = compiler_globals->copied_functions_count;

	    /* Prevent destruction of functions copied from the main process context */
		if (zend_hash_num_elements(compiler_globals->function_table) <= n) {
			compiler_globals->function_table->nNumUsed = 0;
		} else {
			Bucket *p = compiler_globals->function_table->arData;

			compiler_globals->function_table->nNumOfElements -= n;
			while (n != 0) {
				ZVAL_UNDEF(&p->val);
				p++;
				n--;
			}
		}
		zend_hash_destroy(compiler_globals->function_table);
		free(compiler_globals->function_table);
	}
	if (compiler_globals->class_table != GLOBAL_CLASS_TABLE) {
		/* Child classes may reuse structures from parent classes, so destroy in reverse order. */
		zend_hash_graceful_reverse_destroy(compiler_globals->class_table);
		free(compiler_globals->class_table);
	}
	if (compiler_globals->auto_globals != GLOBAL_AUTO_GLOBALS_TABLE) {
		zend_hash_destroy(compiler_globals->auto_globals);
		free(compiler_globals->auto_globals);
	}
	if (compiler_globals->script_encoding_list) {
		pefree((char*)compiler_globals->script_encoding_list, 1);
	}
	if (compiler_globals->map_ptr_real_base) {
		free(compiler_globals->map_ptr_real_base);
		compiler_globals->map_ptr_real_base = NULL;
		compiler_globals->map_ptr_base = ZEND_MAP_PTR_BIASED_BASE(NULL);
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
	executor_globals->capture_warnings_during_sccp = 0;
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
	executor_globals->current_fiber_context = NULL;
	executor_globals->main_fiber_context = NULL;
	executor_globals->active_fiber = NULL;
#ifdef ZEND_WIN32
	zend_get_windows_version_info(&executor_globals->windows_version_info);
#endif
	executor_globals->flags = EG_FLAGS_INITIAL;
	executor_globals->record_errors = false;
	executor_globals->num_errors = 0;
	executor_globals->errors = NULL;
#ifdef ZEND_CHECK_STACK_LIMIT
	executor_globals->stack_limit = (void*)0;
	executor_globals->stack_base = (void*)0;
#endif
#ifdef ZEND_MAX_EXECUTION_TIMERS
	executor_globals->pid = 0;
	executor_globals->oldact = (struct sigaction){0};
#endif
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
	zend_copy_ini_directives();
	zend_ini_refresh_caches(ZEND_INI_STAGE_STARTUP);
#ifdef ZEND_CHECK_STACK_LIMIT
	zend_call_stack_init();
#endif
	zend_max_execution_timer_init();
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
}
/* }}} */

static bool php_auto_globals_create_globals(zend_string *name) /* {{{ */
{
	/* While we keep registering $GLOBALS as an auto-global, we do not create an
	 * actual variable for it. Access to it handled specially by the compiler. */
	return 0;
}
/* }}} */

void zend_startup(zend_utility_functions *utility_functions) /* {{{ */
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

	zend_startup_hrtime();
	zend_startup_strtod();
	zend_startup_extensions_mechanism();

	/* Set up utility functions and values */
	zend_error_cb = utility_functions->error_function;
	zend_printf = utility_functions->printf_function;
	zend_write = utility_functions->write_function;
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

#ifdef HAVE_DTRACE
/* build with dtrace support */
	{
		char *tmp = getenv("USE_ZEND_DTRACE");

		if (tmp && ZEND_ATOL(tmp)) {
			zend_dtrace_enabled = 1;
			zend_compile_file = dtrace_compile_file;
			zend_execute_ex = dtrace_execute_ex;
			zend_execute_internal = dtrace_execute_internal;

			zend_observer_error_register(dtrace_error_notify_cb);
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

	zend_hash_init(GLOBAL_FUNCTION_TABLE, 1024, NULL, ZEND_FUNCTION_DTOR, 1);
	zend_hash_init(GLOBAL_CLASS_TABLE, 64, NULL, ZEND_CLASS_DTOR, 1);
	zend_hash_init(GLOBAL_AUTO_GLOBALS_TABLE, 8, NULL, auto_global_dtor, 1);
	zend_hash_init(GLOBAL_CONSTANTS_TABLE, 128, NULL, ZEND_CONSTANT_DTOR, 1);

	zend_hash_init(&module_registry, 32, NULL, module_destructor_zval, 1);
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
	/* Map region is going to be created and resized at run-time. */
	CG(map_ptr_real_base) = NULL;
	CG(map_ptr_base) = ZEND_MAP_PTR_BIASED_BASE(NULL);
	CG(map_ptr_size) = 0;
	CG(map_ptr_last) = 0;
#endif /* ZTS */
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

	zend_optimizer_startup();

#ifdef ZTS
	tsrm_set_new_thread_end_handler(zend_new_thread_end_handler);
	tsrm_set_shutdown_handler(zend_interned_strings_dtor);
#endif
}
/* }}} */

void zend_register_standard_ini_entries(void) /* {{{ */
{
	zend_register_ini_entries_ex(ini_entries, 0, MODULE_PERSISTENT);
}
/* }}} */


/* Unlink the global (r/o) copies of the class, function and constant tables,
 * and use a fresh r/w copy for the startup thread
 */
zend_result zend_post_startup(void) /* {{{ */
{
#ifdef ZTS
	zend_encoding **script_encoding_list;

	zend_compiler_globals *compiler_globals = ts_resource(compiler_globals_id);
	zend_executor_globals *executor_globals = ts_resource(executor_globals_id);
#endif

	startup_done = true;

	if (zend_post_startup_cb) {
		zend_result (*cb)(void) = zend_post_startup_cb;

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
	if (compiler_globals->map_ptr_real_base) {
		free(compiler_globals->map_ptr_real_base);
	}
	compiler_globals->map_ptr_real_base = NULL;
	compiler_globals->map_ptr_base = ZEND_MAP_PTR_BIASED_BASE(NULL);
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

#ifdef ZEND_CHECK_STACK_LIMIT
	zend_call_stack_init();
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
	/* Child classes may reuse structures from parent classes, so destroy in reverse order. */
	zend_hash_graceful_reverse_destroy(GLOBAL_CLASS_TABLE);

	zend_hash_destroy(GLOBAL_AUTO_GLOBALS_TABLE);
	free(GLOBAL_AUTO_GLOBALS_TABLE);

	zend_shutdown_extensions();
	free(zend_version_info);

	free(GLOBAL_FUNCTION_TABLE);
	free(GLOBAL_CLASS_TABLE);

	zend_hash_destroy(GLOBAL_CONSTANTS_TABLE);
	free(GLOBAL_CONSTANTS_TABLE);
	zend_shutdown_strtod();
	zend_attributes_shutdown();

#ifdef ZTS
	GLOBAL_FUNCTION_TABLE = NULL;
	GLOBAL_CLASS_TABLE = NULL;
	GLOBAL_AUTO_GLOBALS_TABLE = NULL;
	GLOBAL_CONSTANTS_TABLE = NULL;
	ts_free_id(executor_globals_id);
	ts_free_id(compiler_globals_id);
#else
	if (CG(map_ptr_real_base)) {
		free(CG(map_ptr_real_base));
		CG(map_ptr_real_base) = NULL;
		CG(map_ptr_base) = ZEND_MAP_PTR_BIASED_BASE(NULL);
		CG(map_ptr_size) = 0;
	}
	if (CG(script_encoding_list)) {
		free(ZEND_VOIDP(CG(script_encoding_list)));
		CG(script_encoding_list) = NULL;
		CG(script_encoding_list_size) = 0;
	}
#endif
	zend_destroy_rsrc_list_dtors();

	zend_optimizer_shutdown();
	startup_done = false;
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
	CG(memoize_mode) = 0;
	EG(current_execute_data) = NULL;
	LONGJMP(*EG(bailout), FAILURE);
}
/* }}} */

ZEND_API size_t zend_get_page_size(void)
{
#ifdef _WIN32
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	return system_info.dwPageSize;
#elif defined(__FreeBSD__)
	/* This returns the value obtained from
	 * the auxv vector, avoiding a syscall. */
	return getpagesize();
#else
	return (size_t) sysconf(_SC_PAGESIZE);
#endif
}

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

ZEND_API const char *get_zend_version(void) /* {{{ */
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
		memset(CG(map_ptr_real_base), 0, CG(map_ptr_last) * sizeof(void*));
	}
	zend_init_internal_run_time_cache();
	zend_observer_activate();
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

	/* See GH-8646: https://github.com/php/php-src/issues/8646
	 *
	 * Interned strings that hold class entries can get a corresponding slot in map_ptr for the CE cache.
	 * map_ptr works like a bump allocator: there is a counter which increases to allocate the next slot in the map.
	 *
	 * For class name strings in non-opcache we have:
	 *   - on startup: permanent + interned
	 *   - on request: interned
	 * For class name strings in opcache we have:
	 *   - on startup: permanent + interned
	 *   - on request: either not interned at all, which we can ignore because they won't get a CE cache entry
	 *                 or they were already permanent + interned
	 *                 or we get a new permanent + interned string in the opcache persistence code
	 *
	 * Notice that the map_ptr layout always has the permanent strings first, and the request strings after.
	 * In non-opcache, a request string may get a slot in map_ptr, and that interned request string
	 * gets destroyed at the end of the request. The corresponding map_ptr slot can thereafter never be used again.
	 * This causes map_ptr to keep reallocating to larger and larger sizes.
	 *
	 * We solve it as follows:
	 * We can check whether we had any interned request strings, which only happens in non-opcache.
	 * If we have any, we reset map_ptr to the last permanent string.
	 * We can't lose any permanent strings because of map_ptr's layout.
	 */
	if (zend_hash_num_elements(&CG(interned_strings)) > 0) {
		zend_map_ptr_reset();
	}

#if GC_BENCH
	gc_bench_print();
#endif
}
/* }}} */

ZEND_API void zend_message_dispatcher(zend_long message, const void *data) /* {{{ */
{
	if (zend_message_dispatcher_p) {
		zend_message_dispatcher_p(message, data);
	}
}
/* }}} */

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

ZEND_API ZEND_COLD void zend_error_zstr_at(
		int orig_type, zend_string *error_filename, uint32_t error_lineno, zend_string *message)
{
	zval params[4];
	zval retval;
	zval orig_user_error_handler;
	bool in_compilation;
	zend_class_entry *saved_class_entry;
	zend_stack loop_var_stack;
	zend_stack delayed_oplines_stack;
	int type = orig_type & E_ALL;
	bool orig_record_errors;
	uint32_t orig_num_errors;
	zend_error_info **orig_errors;
	zend_result res;

	/* If we're executing a function during SCCP, count any warnings that may be emitted,
	 * but don't perform any other error handling. */
	if (EG(capture_warnings_during_sccp)) {
		ZEND_ASSERT(!(type & E_FATAL_ERRORS) && "Fatal error during SCCP");
		EG(capture_warnings_during_sccp)++;
		return;
	}

	if (EG(record_errors)) {
		zend_error_info *info = emalloc(sizeof(zend_error_info));
		info->type = type;
		info->lineno = error_lineno;
		info->filename = zend_string_copy(error_filename);
		info->message = zend_string_copy(message);

		/* This is very inefficient for a large number of errors.
		 * Use pow2 realloc if it becomes a problem. */
		EG(num_errors)++;
		EG(errors) = erealloc(EG(errors), sizeof(zend_error_info*) * EG(num_errors));
		EG(errors)[EG(num_errors)-1] = info;
	}

	/* Report about uncaught exception in case of fatal errors */
	if (EG(exception)) {
		zend_execute_data *ex;
		const zend_op *opline;

		if (type & E_FATAL_ERRORS) {
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
		}
	}

	zend_observer_error_notify(type, error_filename, error_lineno, message);

	/* if we don't have a user defined error handler */
	if (Z_TYPE(EG(user_error_handler)) == IS_UNDEF
		|| !(EG(user_error_handler_error_reporting) & type)
		|| EG(error_handling) != EH_NORMAL) {
		zend_error_cb(orig_type, error_filename, error_lineno, message);
	} else switch (type) {
		case E_ERROR:
		case E_PARSE:
		case E_CORE_ERROR:
		case E_CORE_WARNING:
		case E_COMPILE_ERROR:
		case E_COMPILE_WARNING:
			/* The error may not be safe to handle in user-space */
			zend_error_cb(orig_type, error_filename, error_lineno, message);
			break;
		default:
			/* Handle the error in user space */
			ZVAL_STR_COPY(&params[1], message);
			ZVAL_LONG(&params[0], type);

			if (error_filename) {
				ZVAL_STR_COPY(&params[2], error_filename);
			} else {
				ZVAL_NULL(&params[2]);
			}

			ZVAL_LONG(&params[3], error_lineno);

			ZVAL_COPY_VALUE(&orig_user_error_handler, &EG(user_error_handler));
			ZVAL_UNDEF(&EG(user_error_handler));

			/* User error handler may include() additional PHP files.
			 * If an error was generated during compilation PHP will compile
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

			orig_record_errors = EG(record_errors);
			orig_num_errors = EG(num_errors);
			orig_errors = EG(errors);
			EG(record_errors) = false;
			EG(num_errors) = 0;
			EG(errors) = NULL;

			res = call_user_function(CG(function_table), NULL, &orig_user_error_handler, &retval, 4, params);

			EG(record_errors) = orig_record_errors;
			EG(num_errors) = orig_num_errors;
			EG(errors) = orig_errors;

			if (res == SUCCESS) {
				if (Z_TYPE(retval) != IS_UNDEF) {
					if (Z_TYPE(retval) == IS_FALSE) {
						zend_error_cb(orig_type, error_filename, error_lineno, message);
					}
					zval_ptr_dtor(&retval);
				}
			} else if (!EG(exception)) {
				/* The user error handler failed, use built-in error handler */
				zend_error_cb(orig_type, error_filename, error_lineno, message);
			}

			if (in_compilation) {
				CG(active_class_entry) = saved_class_entry;
				RESTORE_STACK(loop_var_stack);
				RESTORE_STACK(delayed_oplines_stack);
				CG(in_compilation) = 1;
			}

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

static ZEND_COLD void zend_error_va_list(
		int orig_type, zend_string *error_filename, uint32_t error_lineno,
		const char *format, va_list args)
{
	zend_string *message = zend_vstrpprintf(0, format, args);
	zend_error_zstr_at(orig_type, error_filename, error_lineno, message);
	zend_string_release(message);
}

static ZEND_COLD void get_filename_lineno(int type, zend_string **filename, uint32_t *lineno) {
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
				*filename = zend_get_compiled_filename();
				*lineno = zend_get_compiled_lineno();
			} else if (zend_is_executing()) {
				*filename = zend_get_executed_filename_ex();
				*lineno = zend_get_executed_lineno();
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
		*filename = ZSTR_KNOWN(ZEND_STR_UNKNOWN_CAPITALIZED);
	}
}

ZEND_API ZEND_COLD void zend_error_at(
		int type, zend_string *filename, uint32_t lineno, const char *format, ...) {
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
	zend_string *filename;
	uint32_t lineno;
	va_list args;

	get_filename_lineno(type, &filename, &lineno);
	va_start(args, format);
	zend_error_va_list(type, filename, lineno, format, args);
	va_end(args);
}

ZEND_API ZEND_COLD void zend_error_unchecked(int type, const char *format, ...) {
	zend_string *filename;
	uint32_t lineno;
	va_list args;

	get_filename_lineno(type, &filename, &lineno);
	va_start(args, format);
	zend_error_va_list(type, filename, lineno, format, args);
	va_end(args);
}

ZEND_API ZEND_COLD ZEND_NORETURN void zend_error_at_noreturn(
		int type, zend_string *filename, uint32_t lineno, const char *format, ...)
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

ZEND_API ZEND_COLD ZEND_NORETURN void zend_error_noreturn(int type, const char *format, ...)
{
	zend_string *filename;
	uint32_t lineno;
	va_list args;

	get_filename_lineno(type, &filename, &lineno);
	va_start(args, format);
	zend_error_va_list(type, filename, lineno, format, args);
	va_end(args);
	/* Should never reach this. */
	abort();
}

ZEND_API ZEND_COLD ZEND_NORETURN void zend_strerror_noreturn(int type, int errn, const char *message)
{
#ifdef HAVE_STRERROR_R
	char b[1024];

# ifdef STRERROR_R_CHAR_P
	char *buf = strerror_r(errn, b, sizeof(b));
# else
	strerror_r(errn, b, sizeof(b));
	char *buf = b;
# endif
#else
	char *buf = strerror(errn);
#endif

	zend_error_noreturn(type, "%s: %s (%d)", message, buf, errn);
}

ZEND_API ZEND_COLD void zend_error_zstr(int type, zend_string *message) {
	zend_string *filename;
	uint32_t lineno;
	get_filename_lineno(type, &filename, &lineno);
	zend_error_zstr_at(type, filename, lineno, message);
}

ZEND_API void zend_begin_record_errors(void)
{
	ZEND_ASSERT(!EG(record_errors) && "Error recording already enabled");
	EG(record_errors) = true;
	EG(num_errors) = 0;
	EG(errors) = NULL;
}

ZEND_API void zend_emit_recorded_errors(void)
{
	EG(record_errors) = false;
	for (uint32_t i = 0; i < EG(num_errors); i++) {
		zend_error_info *error = EG(errors)[i];
		zend_error_zstr_at(error->type, error->filename, error->lineno, error->message);
	}
}

ZEND_API void zend_free_recorded_errors(void)
{
	if (!EG(num_errors)) {
		return;
	}

	for (uint32_t i = 0; i < EG(num_errors); i++) {
		zend_error_info *info = EG(errors)[i];
		zend_string_release(info->filename);
		zend_string_release(info->message);
		efree(info);
	}
	efree(EG(errors));
	EG(errors) = NULL;
	EG(num_errors) = 0;
}

ZEND_API ZEND_COLD void zend_throw_error(zend_class_entry *exception_ce, const char *format, ...) /* {{{ */
{
	va_list va;
	char *message = NULL;

	if (!exception_ce) {
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
		zend_error_noreturn(E_ERROR, "%s", message);
	}

	efree(message);
	va_end(va);
}
/* }}} */

/* type should be one of the BP_VAR_* constants, only special messages happen for isset/empty and unset */
ZEND_API ZEND_COLD void zend_illegal_container_offset(const zend_string *container, const zval *offset, int type)
{
	switch (type) {
		case BP_VAR_IS:
			zend_type_error("Cannot access offset of type %s in isset or empty",
				zend_zval_type_name(offset));
			return;
		case BP_VAR_UNSET:
			/* Consistent error for when trying to unset a string offset */
			if (zend_string_equals(container, ZSTR_KNOWN(ZEND_STR_STRING))) {
				zend_throw_error(NULL, "Cannot unset string offsets");
			} else {
				zend_type_error("Cannot unset offset of type %s on %s", zend_zval_type_name(offset), ZSTR_VAL(container));
			}
			return;
		default:
			zend_type_error("Cannot access offset of type %s on %s",
				zend_zval_type_name(offset), ZSTR_VAL(container));
			return;
	}
}

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

ZEND_API ZEND_COLD void zend_argument_count_error(const char *format, ...) /* {{{ */
{
	va_list va;
	char *message = NULL;

	va_start(va, format);
	zend_vspprintf(&message, 0, format, va);
	zend_throw_exception(zend_ce_argument_count_error, message, 0);
	efree(message);

	va_end(va);
} /* }}} */

ZEND_API ZEND_COLD void zend_value_error(const char *format, ...) /* {{{ */
{
	va_list va;
	char *message = NULL;

	va_start(va, format);
	zend_vspprintf(&message, 0, format, va);
	zend_throw_exception(zend_ce_value_error, message, 0);
	efree(message);
	va_end(va);
} /* }}} */

ZEND_API ZEND_COLD void zend_output_debug_string(bool trigger_break, const char *format, ...) /* {{{ */
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

	if (zend_is_unwind_exit(EG(exception))) {
		return;
	}

	old_exception = EG(exception);
	EG(exception) = NULL;
	ZVAL_OBJ(&params[0], old_exception);
	ZVAL_COPY_VALUE(&orig_user_exception_handler, &EG(user_exception_handler));
	ZVAL_UNDEF(&EG(user_exception_handler));

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

	zval_ptr_dtor(&orig_user_exception_handler);
} /* }}} */

ZEND_API zend_result zend_execute_scripts(int type, zval *retval, int file_count, ...) /* {{{ */
{
	va_list files;
	int i;
	zend_file_handle *file_handle;
	zend_op_array *op_array;
	zend_result ret = SUCCESS;

	va_start(files, file_count);
	for (i = 0; i < file_count; i++) {
		file_handle = va_arg(files, zend_file_handle *);
		if (!file_handle) {
			continue;
		}

		if (ret == FAILURE) {
			continue;
		}

		op_array = zend_compile_file(file_handle, type);
		if (file_handle->opened_path) {
			zend_hash_add_empty_element(&EG(included_files), file_handle->opened_path);
		}
		if (op_array) {
			zend_execute(op_array, retval);
			zend_exception_restore();
			if (UNEXPECTED(EG(exception))) {
				if (Z_TYPE(EG(user_exception_handler)) != IS_UNDEF) {
					zend_user_exception_handler();
				}
				if (EG(exception)) {
					ret = zend_exception_error(EG(exception), E_ERROR);
				}
			}
			zend_destroy_static_vars(op_array);
			destroy_op_array(op_array);
			efree_size(op_array, sizeof(zend_op_array));
		} else if (type==ZEND_REQUIRE) {
			ret = FAILURE;
		}
	}
	va_end(files);

	return ret;
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
		/* Grow map_ptr table */
		CG(map_ptr_size) = ZEND_MM_ALIGNED_SIZE_EX(CG(map_ptr_last) + 1, 4096);
		CG(map_ptr_real_base) = perealloc(CG(map_ptr_real_base), CG(map_ptr_size) * sizeof(void*), 1);
		CG(map_ptr_base) = ZEND_MAP_PTR_BIASED_BASE(CG(map_ptr_real_base));
	}
	ptr = (void**)CG(map_ptr_real_base) + CG(map_ptr_last);
	*ptr = NULL;
	CG(map_ptr_last)++;
	return ZEND_MAP_PTR_PTR2OFFSET(ptr);
}

ZEND_API void zend_map_ptr_extend(size_t last)
{
	if (last > CG(map_ptr_last)) {
		void **ptr;

		if (last >= CG(map_ptr_size)) {
			/* Grow map_ptr table */
			CG(map_ptr_size) = ZEND_MM_ALIGNED_SIZE_EX(last, 4096);
			CG(map_ptr_real_base) = perealloc(CG(map_ptr_real_base), CG(map_ptr_size) * sizeof(void*), 1);
			CG(map_ptr_base) = ZEND_MAP_PTR_BIASED_BASE(CG(map_ptr_real_base));
		}
		ptr = (void**)CG(map_ptr_real_base) + CG(map_ptr_last);
		memset(ptr, 0, (last - CG(map_ptr_last)) * sizeof(void*));
		CG(map_ptr_last) = last;
	}
}

ZEND_API void zend_alloc_ce_cache(zend_string *type_name)
{
	if (ZSTR_HAS_CE_CACHE(type_name) || !ZSTR_IS_INTERNED(type_name)) {
		return;
	}

	if ((GC_FLAGS(type_name) & IS_STR_PERMANENT) && startup_done) {
		/* Don't allocate slot on permanent interned string outside module startup.
		 * The cache slot would no longer be valid on the next request. */
		return;
	}

	if (zend_string_equals_literal_ci(type_name, "self")
			|| zend_string_equals_literal_ci(type_name, "parent")) {
		return;
	}

	/* We use the refcount to keep map_ptr of corresponding type */
	uint32_t ret;
	do {
		ret = ZEND_MAP_PTR_NEW_OFFSET();
	} while (ret <= 2);
	GC_ADD_FLAGS(type_name, IS_STR_CLASS_NAME_MAP_PTR);
	GC_SET_REFCOUNT(type_name, ret);
}
