/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
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


#include "zend.h"
#include "zend_extensions.h"
#include "zend_modules.h"
#include "zend_constants.h"
#include "zend_list.h"
#include "zend_API.h"
#include "zend_builtin_functions.h"
#include "zend_ini.h"

#ifdef ZTS
#	define GLOBAL_FUNCTION_TABLE	&global_main_class.function_table
#	define GLOBAL_CLASS_TABLE		&global_main_class.class_table
#	define GLOBAL_CONSTANTS_TABLE	&global_main_class.constants_table
#	define GLOBAL_AUTO_GLOBALS_TABLE	global_auto_globals_table
#else
#	define GLOBAL_FUNCTION_TABLE	CG(function_table)
#	define GLOBAL_CLASS_TABLE		CG(class_table)
#	define GLOBAL_AUTO_GLOBALS_TABLE	CG(auto_globals)
#endif

#if defined(ZEND_WIN32) && ZEND_DEBUG
BOOL WINAPI IsDebuggerPresent(VOID);
#endif

/* true multithread-shared globals */
ZEND_API zend_class_entry *zend_standard_class_def = NULL;
ZEND_API int (*zend_printf)(const char *format, ...);
ZEND_API zend_write_func_t zend_write;
ZEND_API FILE *(*zend_fopen)(const char *filename, char **opened_path);
ZEND_API void (*zend_block_interruptions)(void);
ZEND_API void (*zend_unblock_interruptions)(void);
ZEND_API void (*zend_ticks_function)(int ticks);
ZEND_API void (*zend_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);

void (*zend_on_timeout)(int seconds TSRMLS_DC);

static void (*zend_message_dispatcher_p)(long message, void *data);
static int (*zend_get_configuration_directive_p)(char *name, uint name_length, zval *contents);


static ZEND_INI_MH(OnUpdateErrorReporting)
{
	if (!new_value) {
		EG(error_reporting) = E_ALL & ~E_NOTICE;
	} else {
		EG(error_reporting) = atoi(new_value);
	}
	return SUCCESS;
}


static ZEND_INI_MH(OnUpdateImplicitClone)
{
	if (!new_value) {
		EG(implicit_clone) = 0;
	} else {
		EG(implicit_clone) = atoi(new_value) ? 1 : 0;
	}
	return SUCCESS;
}


ZEND_INI_BEGIN()
	ZEND_INI_ENTRY("error_reporting",			NULL,		ZEND_INI_ALL,		OnUpdateErrorReporting)
	ZEND_INI_ENTRY("zend2.implicit_clone",		NULL,		ZEND_INI_ALL,		OnUpdateImplicitClone)
ZEND_INI_END()


#ifdef ZTS
ZEND_API int compiler_globals_id;
ZEND_API int executor_globals_id;
ZEND_API int alloc_globals_id;
zend_class_entry global_main_class;
HashTable *global_auto_globals_table;
#endif

ZEND_API zend_utility_values zend_uv;

ZEND_API zval zval_used_for_init; /* True global variable */

/* version information */
static char *zend_version_info;
static uint zend_version_info_length;
#define ZEND_CORE_VERSION_INFO	"Zend Engine v" ZEND_VERSION ", Copyright (c) 1998-2003 Zend Technologies\n"


#define PRINT_ZVAL_INDENT 4

static void print_hash(HashTable *ht, int indent)
{
	zval **tmp;
	char *string_key;
	HashPosition iterator;
	ulong num_key;
	uint str_len;
	int i;

	for (i=0; i<indent; i++) {
		ZEND_PUTS(" ");
	}
	ZEND_PUTS("(\n");
	indent += PRINT_ZVAL_INDENT;
	zend_hash_internal_pointer_reset_ex(ht, &iterator);
	while (zend_hash_get_current_data_ex(ht, (void **) &tmp, &iterator) == SUCCESS) {
		for (i=0; i<indent; i++) {
			ZEND_PUTS(" ");
		}
		ZEND_PUTS("[");
		switch (zend_hash_get_current_key_ex(ht, &string_key, &str_len, &num_key, 0, &iterator)) {
			case HASH_KEY_IS_STRING:
				ZEND_PUTS(string_key);
				break;
			case HASH_KEY_IS_LONG:
				zend_printf("%ld", num_key);
				break;
		}
		ZEND_PUTS("] => ");
		zend_print_zval_r(*tmp, indent+PRINT_ZVAL_INDENT);
		ZEND_PUTS("\n");
		zend_hash_move_forward_ex(ht, &iterator);
	}
	indent -= PRINT_ZVAL_INDENT;
	for (i=0; i<indent; i++) {
		ZEND_PUTS(" ");
	}
	ZEND_PUTS(")\n");
}

static void print_flat_hash(HashTable *ht)
{
    zval **tmp;
    char *string_key;
    HashPosition iterator;
    ulong num_key;
    uint str_len;
    int i = 0;

    zend_hash_internal_pointer_reset_ex(ht, &iterator);
    while (zend_hash_get_current_data_ex(ht, (void **) &tmp, &iterator) == SUCCESS) {
	if (i++ > 0) {
	    ZEND_PUTS(",");
	}
	ZEND_PUTS("[");
	switch (zend_hash_get_current_key_ex(ht, &string_key, &str_len, &num_key, 0, &iterator)) {
	    case HASH_KEY_IS_STRING:
		ZEND_PUTS(string_key);
		break;
	    case HASH_KEY_IS_LONG:
		zend_printf("%ld", num_key);
		break;
	}
	ZEND_PUTS("] => ");
	zend_print_flat_zval_r(*tmp);
	zend_hash_move_forward_ex(ht, &iterator);
    }
}

ZEND_API void zend_make_printable_zval(zval *expr, zval *expr_copy, int *use_copy)
{
	if (expr->type==IS_STRING) {
		*use_copy = 0;
		return;
	}
	switch (expr->type) {
		case IS_NULL:
			expr_copy->value.str.len = 0;
			expr_copy->value.str.val = empty_string;
			break;
		case IS_BOOL:
			if (expr->value.lval) {
				expr_copy->value.str.len = 1;
				expr_copy->value.str.val = estrndup("1", 1);
			} else {
				expr_copy->value.str.len = 0;
				expr_copy->value.str.val = empty_string;
			}
			break;
		case IS_RESOURCE:
			expr_copy->value.str.val = (char *) emalloc(sizeof("Resource id #")-1 + MAX_LENGTH_OF_LONG);
			expr_copy->value.str.len = sprintf(expr_copy->value.str.val, "Resource id #%ld", expr->value.lval);
			break;
		case IS_ARRAY:
			expr_copy->value.str.len = sizeof("Array")-1;
			expr_copy->value.str.val = estrndup("Array", expr_copy->value.str.len);
			break;
		case IS_OBJECT:
			expr_copy->value.str.val = (char *) emalloc(sizeof("Object id #")-1 + MAX_LENGTH_OF_LONG);
			expr_copy->value.str.len = sprintf(expr_copy->value.str.val, "Object id #%ld", (long)expr->value.obj.handle);
#if 0
			/* FIXME: This might break BC for some people */
			expr_copy->value.str.len = sizeof("Object")-1;
			expr_copy->value.str.val = estrndup("Object", expr_copy->value.str.len);
#endif
			break;
		case IS_DOUBLE:
			*expr_copy = *expr;
			zval_copy_ctor(expr_copy);
			zend_locale_sprintf_double(expr_copy ZEND_FILE_LINE_CC);
			break;
		default:
			*expr_copy = *expr;
			zval_copy_ctor(expr_copy);
			convert_to_string(expr_copy);
			break;
	}
	expr_copy->type = IS_STRING;
	*use_copy = 1;
}


ZEND_API int zend_print_zval(zval *expr, int indent)
{
	return zend_print_zval_ex(zend_write, expr, indent);
}


ZEND_API int zend_print_zval_ex(zend_write_func_t write_func, zval *expr, int indent)
{
	zval expr_copy;
	int use_copy;

	zend_make_printable_zval(expr, &expr_copy, &use_copy);
	if (use_copy) {
		expr = &expr_copy;
	}
	if (expr->value.str.len==0) { /* optimize away empty strings */
		if (use_copy) {
			zval_dtor(expr);
		}
		return 0;
	}
	write_func(expr->value.str.val, expr->value.str.len);
	if (use_copy) {
		zval_dtor(expr);
	}
	return expr->value.str.len;
}

ZEND_API void zend_print_flat_zval_r(zval *expr)
{
    zend_write_func_t write_func = zend_write;

    switch (expr->type) {
	case IS_ARRAY:
	    ZEND_PUTS("Array (");
	    if (++expr->value.ht->nApplyCount>1) {
		ZEND_PUTS(" *RECURSION*");
		expr->value.ht->nApplyCount--;
		return;
	    }
	    print_flat_hash(expr->value.ht);
	    ZEND_PUTS(")");
	    expr->value.ht->nApplyCount--;
	    break;
	case IS_OBJECT:
	    {
			HashTable *properties = NULL;
			char *class_name = NULL;
			zend_uint clen;
			
			if(Z_OBJ_HANDLER_P(expr, get_class_name)) {
				Z_OBJ_HANDLER_P(expr, get_class_name)(expr, &class_name, &clen, 0 TSRMLS_CC);
			}
			if(class_name == NULL) {
				class_name = "Unknown Class";
			}
			zend_printf("%s Object (", class_name);
			if(Z_OBJ_HANDLER_P(expr, get_properties)) {
				properties = Z_OBJPROP_P(expr);
			}
			if(properties) {
				if (++properties->nApplyCount>1) {
					ZEND_PUTS(" *RECURSION*");
					properties->nApplyCount--;
					return;
				}
				print_flat_hash(properties);
				properties->nApplyCount--;
			}
			ZEND_PUTS(")");
			break;
	    }
	default:
	    zend_print_variable(expr);
	    break;
    }
}

ZEND_API void zend_print_zval_r(zval *expr, int indent)
{
	zend_print_zval_r_ex(zend_write, expr, indent);
}


ZEND_API void zend_print_zval_r_ex(zend_write_func_t write_func, zval *expr, int indent)
{
	switch (expr->type) {
		case IS_ARRAY:
			ZEND_PUTS("Array\n");
			if (++expr->value.ht->nApplyCount>1) {
				ZEND_PUTS(" *RECURSION*");
				expr->value.ht->nApplyCount--;
				return;
			}
			print_hash(expr->value.ht, indent);
			expr->value.ht->nApplyCount--;
			break;
		case IS_OBJECT:
			{
				HashTable *properties = NULL;
				char *class_name = NULL;
				zend_uint clen;
				
				if(Z_OBJ_HANDLER_P(expr, get_class_name)) {
					Z_OBJ_HANDLER_P(expr, get_class_name)(expr, &class_name, &clen, 0 TSRMLS_CC);
				}
				if(class_name == NULL) {
					class_name = "Unknown Class";
				}
				zend_printf("%s Object\n", class_name);
				if(Z_OBJ_HANDLER_P(expr, get_properties)) {
					properties = Z_OBJPROP_P(expr);
				}
				if(properties) {
					if (++properties->nApplyCount>1) {
						ZEND_PUTS(" *RECURSION*");
						properties->nApplyCount--;
						return;
					}
					print_hash(properties, indent);
					properties->nApplyCount--;
				}
				break;
			}
		default:
			zend_print_variable(expr);
			break;
	}
}


static FILE *zend_fopen_wrapper(const char *filename, char **opened_path)
{
	if (opened_path) {
		*opened_path = estrdup(filename);
	}
	return fopen(filename, "rb");
}


static void register_standard_class(void)
{
	zend_standard_class_def = malloc(sizeof(zend_class_entry));

	zend_standard_class_def->type = ZEND_INTERNAL_CLASS;
	zend_standard_class_def->name_length = sizeof("stdClass") - 1;
	zend_standard_class_def->name = zend_strndup("stdClass", zend_standard_class_def->name_length);
	zend_standard_class_def->parent = NULL;
	zend_hash_init_ex(&zend_standard_class_def->default_properties, 0, NULL, ZVAL_PTR_DTOR, 1, 0);
	zend_hash_init_ex(&zend_standard_class_def->private_properties, 0, NULL, ZVAL_PTR_DTOR, 1, 0);
	zend_hash_init_ex(&zend_standard_class_def->protected_properties, 0, NULL, ZVAL_PTR_DTOR, 1, 0);
	zend_standard_class_def->static_members = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init_ex(zend_standard_class_def->static_members, 0, NULL, ZVAL_PTR_DTOR, 1, 0);
	zend_hash_init_ex(&zend_standard_class_def->constants_table, 0, NULL, ZVAL_PTR_DTOR, 1, 0);
	zend_hash_init_ex(&zend_standard_class_def->class_table, 10, NULL, ZEND_CLASS_DTOR, 1, 0);
	zend_hash_init_ex(&zend_standard_class_def->function_table, 0, NULL, ZEND_FUNCTION_DTOR, 1, 0);
	zend_standard_class_def->constructor = NULL;
	zend_standard_class_def->destructor = NULL;
	zend_standard_class_def->clone = NULL;
	zend_standard_class_def->handle_function_call = NULL;
	zend_standard_class_def->handle_property_get = NULL;
	zend_standard_class_def->handle_property_set = NULL;
	zend_standard_class_def->refcount = 1;
	zend_standard_class_def->constants_updated = 0;
	zend_standard_class_def->ce_flags = 0;

	zend_hash_add(GLOBAL_CLASS_TABLE, "stdclass", sizeof("stdclass"), &zend_standard_class_def, sizeof(zend_class_entry *), NULL);
}


static void zend_set_default_compile_time_values(TSRMLS_D)
{
	/* default compile-time values */
	CG(asp_tags) = 0;
	CG(short_tags) = 1;
	CG(allow_call_time_pass_reference) = 1;
	CG(extended_info) = 0;
}


#ifdef ZTS
static void compiler_globals_ctor(zend_compiler_globals *compiler_globals TSRMLS_DC)
{
	zend_function tmp_func;
	zend_class_entry tmp_class;

	compiler_globals->compiled_filename = NULL;

	compiler_globals->function_table = &compiler_globals->main_class.function_table;
	zend_hash_init_ex(compiler_globals->function_table, 100, NULL, ZEND_FUNCTION_DTOR, 1, 0);
	zend_hash_copy(compiler_globals->function_table, GLOBAL_FUNCTION_TABLE, NULL, &tmp_func, sizeof(zend_function));

	compiler_globals->class_table = &compiler_globals->main_class.class_table;
	zend_hash_init_ex(compiler_globals->class_table, 10, NULL, ZEND_CLASS_DTOR, 1, 0);
	zend_hash_copy(compiler_globals->class_table, GLOBAL_CLASS_TABLE, (copy_ctor_func_t) zend_class_add_ref, &tmp_class, sizeof(zend_class_entry *));

	zend_set_default_compile_time_values(TSRMLS_C);

	CG(interactive) = 0;

	compiler_globals->auto_globals = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init_ex(compiler_globals->auto_globals, 8, NULL, NULL, 1, 0);
	zend_hash_copy(compiler_globals->auto_globals, global_auto_globals_table, NULL, NULL, sizeof(void *) /* empty element */);
}


static void compiler_globals_dtor(zend_compiler_globals *compiler_globals TSRMLS_DC)
{
	if (compiler_globals->function_table != GLOBAL_FUNCTION_TABLE) {
		zend_hash_destroy(compiler_globals->function_table);
	}
	if (compiler_globals->class_table != GLOBAL_CLASS_TABLE) {
		zend_hash_destroy(compiler_globals->class_table);
	}
	if (compiler_globals->auto_globals != global_auto_globals_table) {
		zend_hash_destroy(compiler_globals->auto_globals);
		free(compiler_globals->auto_globals);
	}
}


static void executor_globals_ctor(zend_executor_globals *executor_globals TSRMLS_DC)
{
	zend_startup_constants(TSRMLS_C);
	zend_copy_constants(EG(zend_constants), GLOBAL_CONSTANTS_TABLE);
	zend_init_rsrc_plist(TSRMLS_C);
	EG(lambda_count)=0;
	EG(user_error_handler) = NULL;
	EG(user_exception_handler) = NULL;
	EG(in_execution) = 0;
	EG(current_execute_data) = NULL;
}


static void executor_globals_dtor(zend_executor_globals *executor_globals TSRMLS_DC)
{
	zend_shutdown_constants(TSRMLS_C);
	zend_destroy_rsrc_list(&EG(persistent_list) TSRMLS_CC);
	zend_ini_shutdown(TSRMLS_C);
}


static void zend_new_thread_end_handler(THREAD_T thread_id TSRMLS_DC)
{
	zend_copy_ini_directives(TSRMLS_C);
	zend_ini_refresh_caches(ZEND_INI_STAGE_STARTUP TSRMLS_CC);
}

#endif


static void alloc_globals_ctor(zend_alloc_globals *alloc_globals_p TSRMLS_DC)
{
	start_memory_manager(TSRMLS_C);
}


static void alloc_globals_dtor(zend_alloc_globals *alloc_globals_p TSRMLS_DC)
{
	shutdown_memory_manager(0, 1 TSRMLS_CC);
}


#ifdef __FreeBSD__
/* FreeBSD floating point precision fix */
#include <floatingpoint.h>
#endif


static void scanner_globals_ctor(zend_scanner_globals *scanner_globals_p TSRMLS_DC)
{
	scanner_globals_p->c_buf_p = (char *) 0;
	scanner_globals_p->init = 1;
	scanner_globals_p->start = 0;
	scanner_globals_p->current_buffer = NULL;
	scanner_globals_p->yy_in = NULL;
	scanner_globals_p->yy_out = NULL;
	scanner_globals_p->_yy_more_flag = 0;
	scanner_globals_p->_yy_more_len = 0;
	scanner_globals_p->yy_start_stack_ptr = 0;
	scanner_globals_p->yy_start_stack_depth = 0;
	scanner_globals_p->yy_start_stack = 0;
}

void zend_init_opcodes_handlers();

int zend_startup(zend_utility_functions *utility_functions, char **extensions, int start_builtin_functions)
{
#ifdef ZTS
	zend_compiler_globals *compiler_globals;
	zend_executor_globals *executor_globals;
	void ***tsrm_ls;
#ifdef ZTS
	extern ZEND_API ts_rsrc_id ini_scanner_globals_id;
	extern ZEND_API ts_rsrc_id language_scanner_globals_id;
#else
	extern zend_scanner_globals ini_scanner_globals;
	extern zend_scanner_globals language_scanner_globals;
#endif

	ts_allocate_id(&alloc_globals_id, sizeof(zend_alloc_globals), (ts_allocate_ctor) alloc_globals_ctor, (ts_allocate_dtor) alloc_globals_dtor);
#else
	alloc_globals_ctor(&alloc_globals TSRMLS_CC);
#endif

#ifdef __FreeBSD__
	/* FreeBSD floating point precision fix */
	fpsetmask(0);
#endif

	zend_startup_extensions_mechanism();

	/* Set up utility functions and values */
	zend_error_cb = utility_functions->error_function;
	zend_printf = utility_functions->printf_function;
	zend_write = (zend_write_func_t) utility_functions->write_function;
	zend_fopen = utility_functions->fopen_function;
	if (!zend_fopen) {
		zend_fopen = zend_fopen_wrapper;
	}
	zend_message_dispatcher_p = utility_functions->message_handler;
	zend_block_interruptions = utility_functions->block_interruptions;
	zend_unblock_interruptions = utility_functions->unblock_interruptions;
	zend_get_configuration_directive_p = utility_functions->get_configuration_directive;
	zend_ticks_function = utility_functions->ticks_function;
	zend_on_timeout = utility_functions->on_timeout;

	zend_compile_file = compile_file;
	zend_execute = execute;
	zend_execute_internal = NULL;

	zend_init_opcodes_handlers();

	/* set up version */
	zend_version_info = strdup(ZEND_CORE_VERSION_INFO);
	zend_version_info_length = sizeof(ZEND_CORE_VERSION_INFO)-1;

#ifndef ZTS
	GLOBAL_FUNCTION_TABLE = &compiler_globals.main_class.function_table;
	GLOBAL_CLASS_TABLE = &compiler_globals.main_class.class_table;
#endif
	GLOBAL_AUTO_GLOBALS_TABLE = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init_ex(GLOBAL_FUNCTION_TABLE, 100, NULL, ZEND_FUNCTION_DTOR, 1, 0);
	zend_hash_init_ex(GLOBAL_CLASS_TABLE, 10, NULL, ZEND_CLASS_DTOR, 1, 0);

	register_standard_class();
	zend_hash_init_ex(&module_registry, 50, NULL, ZEND_MODULE_DTOR, 1, 0);
	zend_init_rsrc_list_dtors();


	/* This zval can be used to initialize allocate zval's to an uninit'ed value */
	zval_used_for_init.is_ref = 0;
	zval_used_for_init.refcount = 1;
	zval_used_for_init.type = IS_NULL;

#ifdef ZTS
	zend_hash_init_ex(GLOBAL_CONSTANTS_TABLE, 20, NULL, ZEND_CONSTANT_DTOR, 1, 0);
	zend_hash_init_ex(GLOBAL_AUTO_GLOBALS_TABLE, 8, NULL, NULL, 1, 0);
	ts_allocate_id(&compiler_globals_id, sizeof(zend_compiler_globals), (ts_allocate_ctor) compiler_globals_ctor, (ts_allocate_dtor) compiler_globals_dtor);
	ts_allocate_id(&executor_globals_id, sizeof(zend_executor_globals), (ts_allocate_ctor) executor_globals_ctor, (ts_allocate_dtor) executor_globals_dtor);
	ts_allocate_id(&language_scanner_globals_id, sizeof(zend_scanner_globals), (ts_allocate_ctor) scanner_globals_ctor, NULL);
	ts_allocate_id(&ini_scanner_globals_id, sizeof(zend_scanner_globals), (ts_allocate_ctor) scanner_globals_ctor, NULL);
	compiler_globals = ts_resource(compiler_globals_id);
	executor_globals = ts_resource(executor_globals_id);
	tsrm_ls = ts_resource_ex(0, NULL);

	compiler_globals_dtor(compiler_globals, tsrm_ls);
	*compiler_globals->function_table = *GLOBAL_FUNCTION_TABLE;
	*compiler_globals->class_table = *GLOBAL_CLASS_TABLE;
	compiler_globals->auto_globals = GLOBAL_AUTO_GLOBALS_TABLE;

	zend_hash_destroy(executor_globals->zend_constants);
	*executor_globals->zend_constants = *GLOBAL_CONSTANTS_TABLE;
#else
	zend_hash_init_ex(CG(auto_globals), 8, NULL, NULL, 1, 0);
	scanner_globals_ctor(&ini_scanner_globals TSRMLS_CC);
	scanner_globals_ctor(&language_scanner_globals TSRMLS_CC);
	zend_startup_constants();
	zend_set_default_compile_time_values(TSRMLS_C);
	EG(user_error_handler) = NULL;
	EG(user_exception_handler) = NULL;
#endif
	zend_register_standard_constants(TSRMLS_C);

#ifndef ZTS
	zend_init_rsrc_plist(TSRMLS_C);
#endif

	if (start_builtin_functions) {
		zend_startup_builtin_functions(TSRMLS_C);
	}

	zend_ini_startup(TSRMLS_C);

#ifdef ZTS
	tsrm_set_new_thread_end_handler(zend_new_thread_end_handler);
#endif

	return SUCCESS;
}


void zend_register_standard_ini_entries(TSRMLS_D)
{
	int module_number = 0;

	REGISTER_INI_ENTRIES();
}


#ifdef ZTS
/* Unlink the global (r/o) copies of the class, function and constant tables,
 * and use a fresh r/w copy for the startup thread
 */
void zend_post_startup(TSRMLS_D)
{
	zend_compiler_globals *compiler_globals = ts_resource(compiler_globals_id);
	zend_executor_globals *executor_globals = ts_resource(executor_globals_id);

	*GLOBAL_FUNCTION_TABLE = *compiler_globals->function_table;
	*GLOBAL_CLASS_TABLE = *compiler_globals->class_table;
	*GLOBAL_CONSTANTS_TABLE = *executor_globals->zend_constants;
	zend_destroy_rsrc_list(&EG(persistent_list) TSRMLS_CC);
	compiler_globals_ctor(compiler_globals, tsrm_ls);
	executor_globals_ctor(executor_globals, tsrm_ls);
}
#endif


void zend_shutdown(TSRMLS_D)
{
#ifdef ZEND_WIN32
	zend_shutdown_timeout_thread();
#endif
#ifndef ZTS
	zend_destroy_rsrc_list(&EG(persistent_list) TSRMLS_CC);
#endif
	zend_destroy_rsrc_list_dtors();
	zend_hash_graceful_reverse_destroy(&module_registry);

#ifndef ZTS
	/* In ZTS mode these are freed by compiler_globals_dtor() */
	zend_hash_destroy(GLOBAL_FUNCTION_TABLE);
	zend_hash_destroy(GLOBAL_CLASS_TABLE);
#endif

	zend_hash_destroy(GLOBAL_AUTO_GLOBALS_TABLE);
	free(GLOBAL_AUTO_GLOBALS_TABLE);
	zend_shutdown_extensions(TSRMLS_C);
	free(zend_version_info);
#ifndef ZTS
	zend_shutdown_constants();
#endif
}


void zend_set_utility_values(zend_utility_values *utility_values)
{
	zend_uv = *utility_values;
	zend_uv.import_use_extension_length = strlen(zend_uv.import_use_extension);
}


/* this should be compatible with the standard zenderror */
void zenderror(char *error)
{
	zend_error(E_PARSE, error);
}


BEGIN_EXTERN_C()
ZEND_API void _zend_bailout(char *filename, uint lineno)
{
	TSRMLS_FETCH();

	if (!EG(bailout_set)) {
		zend_output_debug_string(1, "%s(%d) : Bailed out without a bailout address!", filename, lineno);
		exit(-1);
	}
	CG(unclean_shutdown) = 1;
	CG(in_compilation) = EG(in_execution) = 0;
	EG(current_execute_data) = NULL;
	longjmp(EG(bailout), FAILURE);
}
END_EXTERN_C()


void zend_append_version_info(zend_extension *extension)
{
	char *new_info;
	uint new_info_length;

	new_info_length = sizeof("    with  v,  by \n")
						+ strlen(extension->name)
						+ strlen(extension->version)
						+ strlen(extension->copyright)
						+ strlen(extension->author);

	new_info = (char *) malloc(new_info_length+1);

	sprintf(new_info, "    with %s v%s, %s, by %s\n", extension->name, extension->version, extension->copyright, extension->author);

	zend_version_info = (char *) realloc(zend_version_info, zend_version_info_length+new_info_length+1);
	strcat(zend_version_info, new_info);
	zend_version_info_length += new_info_length;
	free(new_info);
}


ZEND_API char *get_zend_version()
{
	return zend_version_info;
}


void zend_activate(TSRMLS_D)
{
	init_compiler(TSRMLS_C);
	init_executor(TSRMLS_C);
	startup_scanner(TSRMLS_C);
}


void zend_activate_modules(TSRMLS_D)
{
	zend_hash_apply(&module_registry, (apply_func_t) module_registry_request_startup TSRMLS_CC);
}

void zend_deactivate_modules(TSRMLS_D)
{
	EG(opline_ptr) = NULL; /* we're no longer executing anything */

	zend_try {
		zend_hash_apply(&module_registry, (apply_func_t) module_registry_cleanup TSRMLS_CC);
	} zend_end_try();
}

void zend_deactivate(TSRMLS_D)
{
	/* we're no longer executing anything */
	EG(opline_ptr) = NULL;
	EG(active_symbol_table) = NULL;

	zend_try {
		shutdown_scanner(TSRMLS_C);
	} zend_end_try();

	/* shutdown_executor() takes care of its own bailout handling */
	shutdown_executor(TSRMLS_C);

	zend_try {
		shutdown_compiler(TSRMLS_C);
	} zend_end_try();

	zend_try {
		zend_ini_deactivate(TSRMLS_C);
	} zend_end_try();
}


BEGIN_EXTERN_C()
ZEND_API void zend_message_dispatcher(long message, void *data)
{
	if (zend_message_dispatcher_p) {
		zend_message_dispatcher_p(message, data);
	}
}
END_EXTERN_C()


ZEND_API int zend_get_configuration_directive(char *name, uint name_length, zval *contents)
{
	if (zend_get_configuration_directive_p) {
		return zend_get_configuration_directive_p(name, name_length, contents);
	} else {
		return FAILURE;
	}
}


#define ZEND_ERROR_BUFFER_SIZE 1024

ZEND_API void zend_error(int type, const char *format, ...)
{
	va_list args;
	zval ***params;
	zval *retval;
	zval *z_error_type, *z_error_message, *z_error_filename, *z_error_lineno, *z_context;
	char *error_filename;
	uint error_lineno;
	zval *orig_user_error_handler;
	TSRMLS_FETCH();

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
		case E_WARNING:
		case E_USER_ERROR:
		case E_USER_WARNING:
		case E_USER_NOTICE:
			if (zend_is_compiling(TSRMLS_C)) {
				error_filename = zend_get_compiled_filename(TSRMLS_C);
				error_lineno = zend_get_compiled_lineno(TSRMLS_C);
			} else if (zend_is_executing(TSRMLS_C)) {
				error_filename = zend_get_executed_filename(TSRMLS_C);
				error_lineno = zend_get_executed_lineno(TSRMLS_C);
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


	va_start(args, format);

	/* if we don't have a user defined error handler */
	if (!EG(user_error_handler)) {
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
			ALLOC_INIT_ZVAL(z_error_message);
			ALLOC_INIT_ZVAL(z_error_type);
			ALLOC_INIT_ZVAL(z_error_filename);
			ALLOC_INIT_ZVAL(z_error_lineno);
			ALLOC_INIT_ZVAL(z_context);
			z_error_message->value.str.val = (char *) emalloc(ZEND_ERROR_BUFFER_SIZE);

#ifdef HAVE_VSNPRINTF
			z_error_message->value.str.len = vsnprintf(z_error_message->value.str.val, ZEND_ERROR_BUFFER_SIZE, format, args);
			if (z_error_message->value.str.len > ZEND_ERROR_BUFFER_SIZE-1) {
				z_error_message->value.str.len = ZEND_ERROR_BUFFER_SIZE-1;
			}
#else
			strncpy(z_error_message->value.str.val, format, ZEND_ERROR_BUFFER_SIZE);
			/* This is risky... */
			/* z_error_message->value.str.len = vsprintf(z_error_message->value.str.val, format, args); */
#endif
			z_error_message->type = IS_STRING;

			z_error_type->value.lval = type;
			z_error_type->type = IS_LONG;

			if (error_filename) {
				z_error_filename->value.str.len = strlen(error_filename);
				z_error_filename->value.str.val = estrndup(error_filename, z_error_filename->value.str.len);
				z_error_filename->type = IS_STRING;
			}

			z_error_lineno->value.lval = error_lineno;
			z_error_lineno->type = IS_LONG;

			z_context->value.ht = EG(active_symbol_table);
			z_context->type = IS_ARRAY;
			ZVAL_ADDREF(z_context);  /* we don't want this one to be freed */

			params = (zval ***) emalloc(sizeof(zval **)*5);
			params[0] = &z_error_type;
			params[1] = &z_error_message;
			params[2] = &z_error_filename;
			params[3] = &z_error_lineno;
			params[4] = &z_context;

			orig_user_error_handler = EG(user_error_handler);
			EG(user_error_handler) = NULL;
			if (call_user_function_ex(CG(function_table), NULL, orig_user_error_handler, &retval, 5, params, 1, NULL TSRMLS_CC)==SUCCESS) {
				if (retval) {
					zval_ptr_dtor(&retval);
				}
			} else {
				/* The user error handler failed, use built-in error handler */
				zend_error_cb(type, error_filename, error_lineno, format, args);
			}
			EG(user_error_handler) = orig_user_error_handler;

			efree(params);
			zval_ptr_dtor(&z_error_message);
			zval_ptr_dtor(&z_error_type);
			zval_ptr_dtor(&z_error_filename);
			zval_ptr_dtor(&z_error_lineno);
			if (ZVAL_REFCOUNT(z_context) == 2) {
				FREE_ZVAL(z_context);
			}
			break;
	}

	va_end(args);

	if (type == E_PARSE) {
		EG(exit_status) = 255;
		zend_init_compiler_data_structures(TSRMLS_C);
	}
}


ZEND_API void zend_output_debug_string(zend_bool trigger_break, char *format, ...)
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


ZEND_API int zend_execute_scripts(int type TSRMLS_DC, zval **retval, int file_count, ...)
{
	va_list files;
	int i;
	zend_file_handle *file_handle;
	zend_op_array *orig_op_array = EG(active_op_array);
	zval *local_retval=NULL;

	va_start(files, file_count);
	for (i=0; i<file_count; i++) {
		file_handle = va_arg(files, zend_file_handle *);
		if (!file_handle) {
			continue;
		}
		EG(active_op_array) = zend_compile_file(file_handle, ZEND_INCLUDE TSRMLS_CC);
		zend_destroy_file_handle(file_handle TSRMLS_CC);
		if (EG(active_op_array)) {
			EG(return_value_ptr_ptr) = retval ? retval : &local_retval;
			zend_execute(EG(active_op_array) TSRMLS_CC);
			if (EG(exception)) {
#if 1 /* support set_exception_handler() */
				if (EG(user_exception_handler)) {
					zval *orig_user_exception_handler;
					zval ***params, *retval2;
					params = (zval ***)emalloc(sizeof(zval **));
					params[0] = &EG(exception);
					orig_user_exception_handler = EG(user_exception_handler);
					if (call_user_function_ex(CG(function_table), NULL, orig_user_exception_handler, &retval2, 1, params, 1, NULL TSRMLS_CC) == SUCCESS) {
						if (retval2 != NULL) {
							zval_ptr_dtor(&retval2);
						}
					} else {
						zval_ptr_dtor(&EG(exception));
						EG(exception) = NULL;
						zend_error(E_ERROR, "Uncaught exception!");
					}
					efree(params);
					zval_ptr_dtor(&EG(exception));
					EG(exception) = NULL;
				} else {
					zval_ptr_dtor(&EG(exception));
					EG(exception) = NULL;
					zend_error(E_ERROR, "Uncaught exception!");
				}
				if (retval == NULL && *EG(return_value_ptr_ptr) != NULL) {
					zval_ptr_dtor(EG(return_value_ptr_ptr));
					local_retval = NULL;
				}
#else
				zval_ptr_dtor(&EG(exception));
				zend_error(E_ERROR, "Uncaught exception!");
#endif
			} else if (!retval) {
				zval_ptr_dtor(EG(return_value_ptr_ptr));
				local_retval = NULL;
			}
			destroy_op_array(EG(active_op_array));
			efree(EG(active_op_array));
		} else if (type==ZEND_REQUIRE) {
			va_end(files);
			EG(active_op_array) = orig_op_array;
			return FAILURE;
		}
	}
	va_end(files);
	EG(active_op_array) = orig_op_array;

	return SUCCESS;
}

#define COMPILED_STRING_DESCRIPTION_FORMAT "%s(%d) : %s"

ZEND_API char *zend_make_compiled_string_description(char *name TSRMLS_DC)
{
	char *cur_filename;
	int cur_lineno;
	char *compiled_string_description;

	if (zend_is_compiling(TSRMLS_C)) {
		cur_filename = zend_get_compiled_filename(TSRMLS_C);
		cur_lineno = zend_get_compiled_lineno(TSRMLS_C);
	} else if (zend_is_executing(TSRMLS_C)) {
		cur_filename = zend_get_executed_filename(TSRMLS_C);
		cur_lineno = zend_get_executed_lineno(TSRMLS_C);
	} else {
		cur_filename = "Unknown";
		cur_lineno = 0;
	}

	compiled_string_description = emalloc(sizeof(COMPILED_STRING_DESCRIPTION_FORMAT)+strlen(name)+strlen(cur_filename)+MAX_LENGTH_OF_LONG);
	sprintf(compiled_string_description, COMPILED_STRING_DESCRIPTION_FORMAT, cur_filename, cur_lineno, name);
	return compiled_string_description;
}


void free_estring(char **str_p)
{
	efree(*str_p);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
