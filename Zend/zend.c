/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
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

#ifdef ZTS
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

/* true multithread-shared globals */
ZEND_API zend_class_entry *zend_standard_class_def = NULL;
ZEND_API size_t (*zend_printf)(const char *format, ...);
ZEND_API zend_write_func_t zend_write;
ZEND_API FILE *(*zend_fopen)(const char *filename, zend_string **opened_path);
ZEND_API int (*zend_stream_open_function)(const char *filename, zend_file_handle *handle);
ZEND_API void (*zend_block_interruptions)(void);
ZEND_API void (*zend_unblock_interruptions)(void);
ZEND_API void (*zend_ticks_function)(int ticks);
ZEND_API void (*zend_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
size_t (*zend_vspprintf)(char **pbuf, size_t max_len, const char *format, va_list ap);
zend_string *(*zend_vstrpprintf)(size_t max_len, const char *format, va_list ap);
ZEND_API char *(*zend_getenv)(char *name, size_t name_len);
ZEND_API zend_string *(*zend_resolve_path)(const char *filename, int filename_len);

void (*zend_on_timeout)(int seconds);

static void (*zend_message_dispatcher_p)(zend_long message, const void *data);
static zval *(*zend_get_configuration_directive_p)(zend_string *name);

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
	OnUpdateBool(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);

	if (GC_G(gc_enabled)) {
		gc_init();
	}

	return SUCCESS;
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

	val = zend_atol(ZSTR_VAL(new_value), (int)ZSTR_LEN(new_value));

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

ZEND_INI_BEGIN()
	ZEND_INI_ENTRY("error_reporting",				NULL,		ZEND_INI_ALL,		OnUpdateErrorReporting)
	STD_ZEND_INI_ENTRY("zend.assertions",				"1",    ZEND_INI_ALL,       OnUpdateAssertions,           assertions,   zend_executor_globals,  executor_globals)
	STD_ZEND_INI_BOOLEAN("zend.enable_gc",				"1",	ZEND_INI_ALL,		OnUpdateGCEnabled,      gc_enabled,     zend_gc_globals,        gc_globals)
 	STD_ZEND_INI_BOOLEAN("zend.multibyte", "0", ZEND_INI_PERDIR, OnUpdateBool, multibyte,      zend_compiler_globals, compiler_globals)
 	ZEND_INI_ENTRY("zend.script_encoding",			NULL,		ZEND_INI_ALL,		OnUpdateScriptEncoding)
 	STD_ZEND_INI_BOOLEAN("zend.detect_unicode",			"1",	ZEND_INI_ALL,		OnUpdateBool, detect_unicode, zend_compiler_globals, compiler_globals)
#ifdef ZEND_SIGNALS
	STD_ZEND_INI_BOOLEAN("zend.signal_check", "0", ZEND_INI_SYSTEM, OnUpdateBool, check, zend_signal_globals_t, zend_signal_globals)
#endif
ZEND_INI_END()


#ifdef ZTS
ZEND_API int compiler_globals_id;
ZEND_API int executor_globals_id;
static HashTable *global_function_table = NULL;
static HashTable *global_class_table = NULL;
static HashTable *global_constants_table = NULL;
static HashTable *global_auto_globals_table = NULL;
static HashTable *global_persistent_list = NULL;
ZEND_TSRMLS_CACHE_DEFINE()
#endif

ZEND_API zend_utility_values zend_uv;

/* version information */
static char *zend_version_info;
static uint zend_version_info_length;
#define ZEND_CORE_VERSION_INFO	"Zend Engine v" ZEND_VERSION ", Copyright (c) 1998-2016 Zend Technologies\n"
#define PRINT_ZVAL_INDENT 4

static void print_hash(zend_write_func_t write_func, HashTable *ht, int indent, zend_bool is_object) /* {{{ */
{
	zval *tmp;
	zend_string *string_key;
	zend_ulong num_key;
	int i;

	for (i = 0; i < indent; i++) {
		ZEND_PUTS_EX(" ");
	}
	ZEND_PUTS_EX("(\n");
	indent += PRINT_ZVAL_INDENT;
	ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, string_key, tmp) {
		if (Z_TYPE_P(tmp) == IS_INDIRECT) {
			tmp = Z_INDIRECT_P(tmp);
			if (Z_TYPE_P(tmp) == IS_UNDEF) {
				continue;
			}
		}
		for (i = 0; i < indent; i++) {
			ZEND_PUTS_EX(" ");
		}
		ZEND_PUTS_EX("[");
		if (string_key) {
			if (is_object) {
				const char *prop_name, *class_name;
				size_t prop_len;
				int mangled = zend_unmangle_property_name_ex(string_key, &class_name, &prop_name, &prop_len);

				ZEND_WRITE_EX(prop_name, prop_len);
				if (class_name && mangled == SUCCESS) {
					if (class_name[0]=='*') {
						ZEND_PUTS_EX(":protected");
					} else {
						ZEND_PUTS_EX(":");
						ZEND_PUTS_EX(class_name);
						ZEND_PUTS_EX(":private");
					}
				}
			} else {
				ZEND_WRITE_EX(ZSTR_VAL(string_key), ZSTR_LEN(string_key));
			}
		} else {
			char key[25];
			snprintf(key, sizeof(key), ZEND_LONG_FMT, num_key);
			ZEND_PUTS_EX(key);
		}
		ZEND_PUTS_EX("] => ");
		zend_print_zval_r_ex(write_func, tmp, indent+PRINT_ZVAL_INDENT);
		ZEND_PUTS_EX("\n");
	} ZEND_HASH_FOREACH_END();
	indent -= PRINT_ZVAL_INDENT;
	for (i = 0; i < indent; i++) {
		ZEND_PUTS_EX(" ");
	}
	ZEND_PUTS_EX(")\n");
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
		ZVAL_STR(expr_copy, _zval_get_string_func(expr));
		return 1;
	}
}
/* }}} */

ZEND_API size_t zend_print_zval(zval *expr, int indent) /* {{{ */
{
	return zend_print_zval_ex(zend_write, expr, indent);
}
/* }}} */

ZEND_API size_t zend_print_zval_ex(zend_write_func_t write_func, zval *expr, int indent) /* {{{ */
{
	zend_string *str = zval_get_string(expr);
	size_t len = ZSTR_LEN(str);

	if (len != 0) {
		write_func(ZSTR_VAL(str), len);
	}

	zend_string_release(str);
	return len;
}
/* }}} */

ZEND_API void zend_print_flat_zval_r(zval *expr) /* {{{ */
{
	switch (Z_TYPE_P(expr)) {
		case IS_ARRAY:
			ZEND_PUTS("Array (");
			if (ZEND_HASH_APPLY_PROTECTION(Z_ARRVAL_P(expr)) &&
			    ++Z_ARRVAL_P(expr)->u.v.nApplyCount>1) {
				ZEND_PUTS(" *RECURSION*");
				Z_ARRVAL_P(expr)->u.v.nApplyCount--;
				return;
			}
			print_flat_hash(Z_ARRVAL_P(expr));
			ZEND_PUTS(")");
			if (ZEND_HASH_APPLY_PROTECTION(Z_ARRVAL_P(expr))) {
				Z_ARRVAL_P(expr)->u.v.nApplyCount--;
			}
			break;
		case IS_OBJECT:
		{
			HashTable *properties = NULL;
			zend_string *class_name = Z_OBJ_HANDLER_P(expr, get_class_name)(Z_OBJ_P(expr));
			zend_printf("%s Object (", ZSTR_VAL(class_name));
			zend_string_release(class_name);

			if (Z_OBJ_APPLY_COUNT_P(expr) > 0) {
				ZEND_PUTS(" *RECURSION*");
				return;
			}

			if (Z_OBJ_HANDLER_P(expr, get_properties)) {
				properties = Z_OBJPROP_P(expr);
			}
			if (properties) {
				Z_OBJ_INC_APPLY_COUNT_P(expr);
				print_flat_hash(properties);
				Z_OBJ_DEC_APPLY_COUNT_P(expr);
			}
			ZEND_PUTS(")");
			break;
		}
		default:
			zend_print_variable(expr);
			break;
	}
}
/* }}} */

ZEND_API void zend_print_zval_r(zval *expr, int indent) /* {{{ */
{
	zend_print_zval_r_ex(zend_write, expr, indent);
}
/* }}} */

ZEND_API void zend_print_zval_r_ex(zend_write_func_t write_func, zval *expr, int indent) /* {{{ */
{
	ZVAL_DEREF(expr);
	switch (Z_TYPE_P(expr)) {
		case IS_ARRAY:
			ZEND_PUTS_EX("Array\n");
			if (ZEND_HASH_APPLY_PROTECTION(Z_ARRVAL_P(expr)) &&
			    ++Z_ARRVAL_P(expr)->u.v.nApplyCount>1) {
				ZEND_PUTS_EX(" *RECURSION*");
				Z_ARRVAL_P(expr)->u.v.nApplyCount--;
				return;
			}
			print_hash(write_func, Z_ARRVAL_P(expr), indent, 0);
			if (ZEND_HASH_APPLY_PROTECTION(Z_ARRVAL_P(expr))) {
				Z_ARRVAL_P(expr)->u.v.nApplyCount--;
			}
			break;
		case IS_OBJECT:
			{
				HashTable *properties;
				int is_temp;

				zend_string *class_name = Z_OBJ_HANDLER_P(expr, get_class_name)(Z_OBJ_P(expr));
				ZEND_PUTS_EX(ZSTR_VAL(class_name));
				zend_string_release(class_name);

				ZEND_PUTS_EX(" Object\n");
				if (Z_OBJ_APPLY_COUNT_P(expr) > 0) {
					ZEND_PUTS_EX(" *RECURSION*");
					return;
				}
				if ((properties = Z_OBJDEBUG_P(expr, is_temp)) == NULL) {
					break;
				}

				Z_OBJ_INC_APPLY_COUNT_P(expr);
				print_hash(write_func, properties, indent, 1);
				Z_OBJ_DEC_APPLY_COUNT_P(expr);

				if (is_temp) {
					zend_hash_destroy(properties);
					FREE_HASHTABLE(properties);
				}
				break;
			}
		default:
			zend_print_zval_ex(write_func, expr, indent);
			break;
	}
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
	EG(exception_op)[0].op1_type = IS_UNUSED;
	EG(exception_op)[0].op2_type = IS_UNUSED;
	EG(exception_op)[0].result_type = IS_UNUSED;
	ZEND_VM_SET_OPCODE_HANDLER(EG(exception_op));
	EG(exception_op)[1].opcode = ZEND_HANDLE_EXCEPTION;
	EG(exception_op)[1].op1_type = IS_UNUSED;
	EG(exception_op)[1].op2_type = IS_UNUSED;
	EG(exception_op)[1].result_type = IS_UNUSED;
	ZEND_VM_SET_OPCODE_HANDLER(EG(exception_op)+1);
	EG(exception_op)[2].opcode = ZEND_HANDLE_EXCEPTION;
	EG(exception_op)[2].op1_type = IS_UNUSED;
	EG(exception_op)[2].op2_type = IS_UNUSED;
	EG(exception_op)[2].result_type = IS_UNUSED;
	ZEND_VM_SET_OPCODE_HANDLER(EG(exception_op)+2);
}
/* }}} */

static void zend_init_call_trampoline_op(void) /* {{{ */
{
	memset(&EG(call_trampoline_op), 0, sizeof(EG(call_trampoline_op)));
	EG(call_trampoline_op).opcode = ZEND_CALL_TRAMPOLINE;
	EG(call_trampoline_op).op1_type = IS_UNUSED;
	EG(call_trampoline_op).op2_type = IS_UNUSED;
	EG(call_trampoline_op).result_type = IS_UNUSED;
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
	Z_FUNC_P(zv) = pemalloc(sizeof(zend_internal_function), 1);
	memcpy(Z_FUNC_P(zv), old_func, sizeof(zend_internal_function));
	function_add_ref(Z_FUNC_P(zv));
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

	compiler_globals->last_static_member = zend_hash_num_elements(compiler_globals->class_table);
	if (compiler_globals->last_static_member) {
		compiler_globals->static_members_table = calloc(compiler_globals->last_static_member, sizeof(zval*));
	} else {
		compiler_globals->static_members_table = NULL;
	}
	compiler_globals->script_encoding_list = NULL;

	compiler_globals->empty_string = zend_zts_interned_string_init("", sizeof("")-1);

	memset(compiler_globals->one_char_string, 0, sizeof(compiler_globals->one_char_string));
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
	if (compiler_globals->static_members_table) {
		free(compiler_globals->static_members_table);
	}
	if (compiler_globals->script_encoding_list) {
		pefree((char*)compiler_globals->script_encoding_list, 1);
	}
	compiler_globals->last_static_member = 0;

	zend_zts_interned_string_free(&compiler_globals->empty_string);
}
/* }}} */

static void executor_globals_ctor(zend_executor_globals *executor_globals) /* {{{ */
{
	ZEND_TSRMLS_CACHE_UPDATE();

	zend_startup_constants();
	zend_copy_constants(EG(zend_constants), GLOBAL_CONSTANTS_TABLE);
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

void zend_init_opcodes_handlers(void);

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

	ZVAL_ARR(&globals, &EG(symbol_table));
	Z_TYPE_INFO_P(&globals) = IS_ARRAY;
	ZVAL_NEW_REF(&globals, &globals);
	zend_hash_update(&EG(symbol_table), name, &globals);
	return 0;
}
/* }}} */

int zend_startup(zend_utility_functions *utility_functions, char **extensions) /* {{{ */
{
#ifdef ZTS
	zend_compiler_globals *compiler_globals;
	zend_executor_globals *executor_globals;
	extern ZEND_API ts_rsrc_id ini_scanner_globals_id;
	extern ZEND_API ts_rsrc_id language_scanner_globals_id;
	ZEND_TSRMLS_CACHE_UPDATE();
#else
	extern zend_ini_scanner_globals ini_scanner_globals;
	extern zend_php_scanner_globals language_scanner_globals;
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
#ifndef ZEND_SIGNALS
	zend_block_interruptions = utility_functions->block_interruptions;
	zend_unblock_interruptions = utility_functions->unblock_interruptions;
#endif
	zend_get_configuration_directive_p = utility_functions->get_configuration_directive;
	zend_ticks_function = utility_functions->ticks_function;
	zend_on_timeout = utility_functions->on_timeout;
	zend_vspprintf = utility_functions->vspprintf_function;
	zend_vstrpprintf = utility_functions->vstrpprintf_function;
	zend_getenv = utility_functions->getenv_function;
	zend_resolve_path = utility_functions->resolve_path_function;

#if HAVE_DTRACE
/* build with dtrace support */
	zend_compile_file = dtrace_compile_file;
	zend_execute_ex = dtrace_execute_ex;
	zend_execute_internal = dtrace_execute_internal;
#else
	zend_compile_file = compile_file;
	zend_execute_ex = execute_ex;
	zend_execute_internal = NULL;
#endif /* HAVE_SYS_SDT_H */
	zend_compile_string = compile_string;
	zend_throw_exception_hook = NULL;

	/* Set up the default garbage collection implementation. */
	gc_collect_cycles = zend_gc_collect_cycles;

	zend_init_opcodes_handlers();

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
	ts_allocate_id(&compiler_globals_id, sizeof(zend_compiler_globals), (ts_allocate_ctor) compiler_globals_ctor, (ts_allocate_dtor) compiler_globals_dtor);
	ts_allocate_id(&executor_globals_id, sizeof(zend_executor_globals), (ts_allocate_ctor) executor_globals_ctor, (ts_allocate_dtor) executor_globals_dtor);
	ts_allocate_id(&language_scanner_globals_id, sizeof(zend_php_scanner_globals), (ts_allocate_ctor) php_scanner_globals_ctor, NULL);
	ts_allocate_id(&ini_scanner_globals_id, sizeof(zend_ini_scanner_globals), (ts_allocate_ctor) ini_scanner_globals_ctor, NULL);
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
#endif
	EG(error_reporting) = E_ALL & ~E_NOTICE;

	zend_interned_strings_init();
	zend_startup_builtin_functions();
	zend_register_standard_constants();
	zend_register_auto_global(zend_string_init("GLOBALS", sizeof("GLOBALS") - 1, 1), 1, php_auto_globals_create_globals);

#ifndef ZTS
	zend_init_rsrc_plist();
	zend_init_exception_op();
	zend_init_call_trampoline_op();
#endif

	zend_ini_startup();

#ifdef ZTS
	tsrm_set_new_thread_end_handler(zend_new_thread_end_handler);
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

/* Unlink the global (r/o) copies of the class, function and constant tables,
 * and use a fresh r/w copy for the startup thread
 */
void zend_post_startup(void) /* {{{ */
{
#ifdef ZTS
	zend_encoding **script_encoding_list;

	zend_compiler_globals *compiler_globals = ts_resource(compiler_globals_id);
	zend_executor_globals *executor_globals = ts_resource(executor_globals_id);

	*GLOBAL_FUNCTION_TABLE = *compiler_globals->function_table;
	*GLOBAL_CLASS_TABLE = *compiler_globals->class_table;
	*GLOBAL_CONSTANTS_TABLE = *executor_globals->zend_constants;

	short_tags_default = CG(short_tags);
	compiler_options_default = CG(compiler_options);

	zend_destroy_rsrc_list(&EG(persistent_list));
	free(compiler_globals->function_table);
	free(compiler_globals->class_table);
	if ((script_encoding_list = (zend_encoding **)compiler_globals->script_encoding_list)) {
		compiler_globals_ctor(compiler_globals);
		compiler_globals->script_encoding_list = (const zend_encoding **)script_encoding_list;
	} else {
		compiler_globals_ctor(compiler_globals);
	}
	free(EG(zend_constants));

	virtual_cwd_deactivate();

	executor_globals_ctor(executor_globals);
	global_persistent_list = &EG(persistent_list);
	zend_copy_ini_directives();
#else
	virtual_cwd_deactivate();
#endif
}
/* }}} */

void zend_shutdown(void) /* {{{ */
{
	zend_destroy_rsrc_list(&EG(persistent_list));
	if (EG(active))
	{
		/*
		 * The order of destruction is important here.
		 * See bugs #65463 and 66036.
		 */
		zend_function *func;
		zend_class_entry *ce;

		ZEND_HASH_REVERSE_FOREACH_PTR(GLOBAL_FUNCTION_TABLE, func) {
			if (func->type == ZEND_USER_FUNCTION) {
				zend_cleanup_op_array_data((zend_op_array *) func);
			}
		} ZEND_HASH_FOREACH_END();
		ZEND_HASH_REVERSE_FOREACH_PTR(GLOBAL_CLASS_TABLE, ce) {
			if (ce->type == ZEND_USER_CLASS) {
				zend_cleanup_user_class_data(ce);
			} else {
				break;
			}
		} ZEND_HASH_FOREACH_END();
		zend_cleanup_internal_classes();
		zend_hash_reverse_apply(GLOBAL_FUNCTION_TABLE, (apply_func_t) clean_non_persistent_function_full);
		zend_hash_reverse_apply(GLOBAL_CLASS_TABLE, (apply_func_t) clean_non_persistent_class_full);
	}
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
#endif
	zend_destroy_rsrc_list_dtors();

	zend_interned_strings_dtor();
}
/* }}} */

void zend_set_utility_values(zend_utility_values *utility_values) /* {{{ */
{
	zend_uv = *utility_values;
	zend_uv.import_use_extension_length = (uint)strlen(zend_uv.import_use_extension);
}
/* }}} */

/* this should be compatible with the standard zenderror */
ZEND_COLD void zenderror(const char *error) /* {{{ */
{
	if (EG(exception)) {
		/* An exception was thrown in the lexer, don't throw another in the parser. */
		return;
	}

	zend_throw_exception(zend_ce_parse_error, error, 0);
}
/* }}} */

BEGIN_EXTERN_C()
ZEND_API ZEND_COLD void _zend_bailout(char *filename, uint lineno) /* {{{ */
{

	if (!EG(bailout)) {
		zend_output_debug_string(1, "%s(%d) : Bailed out without a bailout address!", filename, lineno);
		exit(-1);
	}
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
	uint new_info_length;

	new_info_length = (uint)(sizeof("    with  v, , by \n")
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

#if !defined(HAVE_NORETURN) || defined(HAVE_NORETURN_ALIAS)
ZEND_API ZEND_COLD void zend_error(int type, const char *format, ...) /* {{{ */
#else
static ZEND_COLD void zend_error_va_list(int type, const char *format, va_list args)
#endif
{
	char *str;
	int len;
#if !defined(HAVE_NORETURN) || defined(HAVE_NORETURN_ALIAS)
	va_list args;
#endif
	va_list usr_copy;
	zval params[5];
	zval retval;
	const char *error_filename;
	uint error_lineno = 0;
	zval orig_user_error_handler;
	zend_bool in_compilation;
	zend_class_entry *saved_class_entry;
	zend_stack loop_var_stack;
	zend_stack delayed_oplines_stack;
	zend_array *symbol_table;

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

	/* Obtain relevant filename and lineno */
	switch (type) {
		case E_CORE_ERROR:
		case E_CORE_WARNING:
			error_filename = NULL;
			error_lineno = 0;
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
				error_filename = ZSTR_VAL(zend_get_compiled_filename());
				error_lineno = zend_get_compiled_lineno();
			} else if (zend_is_executing()) {
				error_filename = zend_get_executed_filename();
				if (error_filename[0] == '[') { /* [no active file] */
					error_filename = NULL;
					error_lineno = 0;
				} else {
					error_lineno = zend_get_executed_lineno();
				}
			} else {
				error_filename = NULL;
				error_lineno = 0;
			}
			break;
		default:
			error_filename = NULL;
			error_lineno = 0;
			break;
	}
	if (!error_filename) {
		error_filename = "Unknown";
	}

#ifdef HAVE_DTRACE
	if (DTRACE_ERROR_ENABLED()) {
		char *dtrace_error_buffer;
#if !defined(HAVE_NORETURN) || defined(HAVE_NORETURN_ALIAS)
		va_start(args, format);
#endif
		zend_vspprintf(&dtrace_error_buffer, 0, format, args);
		DTRACE_ERROR(dtrace_error_buffer, (char *)error_filename, error_lineno);
		efree(dtrace_error_buffer);
#if !defined(HAVE_NORETURN) || defined(HAVE_NORETURN_ALIAS)
		va_end(args);
#endif
	}
#endif /* HAVE_DTRACE */

#if !defined(HAVE_NORETURN) || defined(HAVE_NORETURN_ALIAS)
	va_start(args, format);
#endif

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
/* va_copy() is __va_copy() in old gcc versions.
 * According to the autoconf manual, using
 * memcpy(&dst, &src, sizeof(va_list))
 * gives maximum portability. */
#ifndef va_copy
# ifdef __va_copy
#  define va_copy(dest, src)	__va_copy((dest), (src))
# else
#  define va_copy(dest, src)	memcpy(&(dest), &(src), sizeof(va_list))
# endif
#endif
			va_copy(usr_copy, args);
			len = (int)zend_vspprintf(&str, 0, format, usr_copy);
			ZVAL_NEW_STR(&params[1], zend_string_init(str, len, 0));
			efree(str);
#ifdef va_copy
			va_end(usr_copy);
#endif

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
			 * such scripts recursivly, but some CG() variables may be
			 * inconsistent. */

			in_compilation = CG(in_compilation);
			if (in_compilation) {
				saved_class_entry = CG(active_class_entry);
				CG(active_class_entry) = NULL;
				SAVE_STACK(loop_var_stack);
				SAVE_STACK(delayed_oplines_stack);
				CG(in_compilation) = 0;
			}

			if (call_user_function_ex(CG(function_table), NULL, &orig_user_error_handler, &retval, 5, params, 1, NULL) == SUCCESS) {
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

			if (in_compilation) {
				CG(active_class_entry) = saved_class_entry;
				RESTORE_STACK(loop_var_stack);
				RESTORE_STACK(delayed_oplines_stack);
				CG(in_compilation) = 1;
			}

			zval_ptr_dtor(&params[4]);
			zval_ptr_dtor(&params[3]);
			zval_ptr_dtor(&params[2]);
			zval_ptr_dtor(&params[1]);
			zval_ptr_dtor(&params[0]);

			if (Z_TYPE(EG(user_error_handler)) == IS_UNDEF) {
				ZVAL_COPY_VALUE(&EG(user_error_handler), &orig_user_error_handler);
			} else {
				zval_ptr_dtor(&orig_user_error_handler);
			}
			break;
	}

#if !defined(HAVE_NORETURN) || defined(HAVE_NORETURN_ALIAS)
	va_end(args);
#endif

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

#ifdef HAVE_NORETURN
# ifdef HAVE_NORETURN_ALIAS
ZEND_COLD void zend_error_noreturn(int type, const char *format, ...) __attribute__ ((alias("zend_error"),noreturn));
# else
ZEND_API ZEND_COLD void zend_error(int type, const char *format, ...) /* {{{ */
{
	va_list va;

	va_start(va, format);
	zend_error_va_list(type, format, va);
	va_end(va);
}

ZEND_API ZEND_COLD ZEND_NORETURN void zend_error_noreturn(int type, const char *format, ...)
{
	va_list va;

	va_start(va, format);
	zend_error_va_list(type, format, va);
	va_end(va);
}
/* }}} */
# endif
#endif

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

ZEND_API void zend_try_exception_handler() /* {{{ */
{
	if (EG(exception)) {
		if (Z_TYPE(EG(user_exception_handler)) != IS_UNDEF) {
			zval orig_user_exception_handler;
			zval params[1], retval2;
			zend_object *old_exception;
			old_exception = EG(exception);
			EG(exception) = NULL;
			ZVAL_OBJ(&params[0], old_exception);
			ZVAL_COPY_VALUE(&orig_user_exception_handler, &EG(user_exception_handler));

			if (call_user_function_ex(CG(function_table), NULL, &orig_user_exception_handler, &retval2, 1, params, 1, NULL) == SUCCESS) {
				zval_ptr_dtor(&retval2);
				if (EG(exception)) {
					OBJ_RELEASE(EG(exception));
					EG(exception) = NULL;
				}
				OBJ_RELEASE(old_exception);
			} else {
				EG(exception) = old_exception;
			}
		}
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
			zend_try_exception_handler();
			if (EG(exception)) {
				zend_exception_error(EG(exception), E_ERROR);
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
