/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2000 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.92 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_92.txt.                                |
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
#include "modules.h"
#include "zend_constants.h"
#include "zend_list.h"
#include "zend_API.h"
#include "zend_builtin_functions.h"

#ifdef ZTS
#	define GLOBAL_FUNCTION_TABLE	global_function_table
#	define GLOBAL_CLASS_TABLE		global_class_table
#	define GLOBAL_CONSTANTS_TABLE	global_constants_table
#else
#	define GLOBAL_FUNCTION_TABLE	CG(function_table)
#	define GLOBAL_CLASS_TABLE		CG(class_table)
#	define GLOBAL_CONSTANTS_TABLE	CG(zend_constants)
#endif

#if defined(ZEND_WIN32) && ZEND_DEBUG
BOOL WINAPI IsDebuggerPresent(VOID);
#endif

/* true multithread-shared globals */
ZEND_API zend_class_entry zend_standard_class_def;
ZEND_API int (*zend_printf)(const char *format, ...);
ZEND_API zend_write_func_t zend_write;
ZEND_API FILE *(*zend_fopen)(const char *filename, char **opened_path);
ZEND_API void (*zend_block_interruptions)(void);
ZEND_API void (*zend_unblock_interruptions)(void);
ZEND_API void (*zend_ticks_function)(int ticks);
ZEND_API void (*zend_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);

static void (*zend_message_dispatcher_p)(long message, void *data);
static int (*zend_get_ini_entry_p)(char *name, uint name_length, zval *contents);


#ifdef ZTS
ZEND_API int compiler_globals_id;
ZEND_API int executor_globals_id;
ZEND_API int alloc_globals_id;
HashTable *global_function_table;
HashTable *global_class_table;
HashTable *global_constants_table;
#endif

zend_utility_values zend_uv;

ZEND_API zval zval_used_for_init; /* True global variable */

/* version information */
static char *zend_version_info;
static uint zend_version_info_length;
#define ZEND_CORE_VERSION_INFO	"Zend Engine v" ZEND_VERSION ", Copyright (c) 1998-2000 Zend Technologies\n"


#define PRINT_ZVAL_INDENT 4

static void print_hash(HashTable *ht, int indent)
{
	zval **tmp;
	char *string_key;
	unsigned long num_key;
	int i;

	for (i=0; i<indent; i++) {
		ZEND_PUTS(" ");
	}
	ZEND_PUTS("(\n");
	indent += PRINT_ZVAL_INDENT;
	zend_hash_internal_pointer_reset(ht);
	while (zend_hash_get_current_data(ht, (void **) &tmp) == SUCCESS) {
		for (i=0; i<indent; i++) {
			ZEND_PUTS(" ");
		}
		ZEND_PUTS("[");
		switch (zend_hash_get_current_key(ht, &string_key, &num_key)) {
			case HASH_KEY_IS_STRING:
				ZEND_PUTS(string_key);
				efree(string_key);
				break;
			case HASH_KEY_IS_LONG:
				zend_printf("%ld",num_key);
				break;
		}
		ZEND_PUTS("] => ");
		zend_print_zval_r(*tmp, indent+PRINT_ZVAL_INDENT);
		ZEND_PUTS("\n");
		zend_hash_move_forward(ht);
	}
	indent -= PRINT_ZVAL_INDENT;
	for (i=0; i<indent; i++) {
		ZEND_PUTS(" ");
	}
	ZEND_PUTS(")\n");
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
			expr_copy->value.str.len = sizeof("Object")-1;
			expr_copy->value.str.val = estrndup("Object", expr_copy->value.str.len);
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
	write_func(expr->value.str.val,expr->value.str.len);
	if (use_copy) {
		zval_dtor(expr);
	}
	return expr->value.str.len;
}


ZEND_API void zend_print_zval_r(zval *expr, int indent)
{
	zend_print_zval_r_ex(zend_write, expr, indent);
}


ZEND_API void zend_print_zval_r_ex(zend_write_func_t write_func, zval *expr, int indent) 
{
	switch(expr->type) {
		case IS_ARRAY:
			ZEND_PUTS("Array\n");
			if (++expr->value.ht->nApplyCount>1) {
				ZEND_PUTS(" *RECURSION*");
				expr->value.ht->nApplyCount=0;
				return;
			}
			print_hash(expr->value.ht,indent);
			expr->value.ht->nApplyCount--;
			break;
		case IS_OBJECT:
			if (++expr->value.obj.properties->nApplyCount>1) {
				ZEND_PUTS(" *RECURSION*");
				expr->value.obj.properties->nApplyCount=0;
				return;
			}
			zend_printf("%s Object\n", expr->value.obj.ce->name);
			print_hash(expr->value.obj.properties, indent);
			expr->value.obj.properties->nApplyCount--;
			break;
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
	zend_standard_class_def.type = ZEND_INTERNAL_CLASS;
	zend_standard_class_def.name_length = sizeof("stdClass") - 1;
	zend_standard_class_def.name = zend_strndup("stdClass", zend_standard_class_def.name_length);
	zend_standard_class_def.parent = NULL;
	zend_hash_init_ex(&zend_standard_class_def.default_properties, 0, NULL, ZVAL_PTR_DTOR, 1, 0);
	zend_hash_init_ex(&zend_standard_class_def.function_table, 0, NULL, ZEND_FUNCTION_DTOR, 1, 0);
	zend_standard_class_def.handle_function_call = NULL;
	zend_standard_class_def.handle_property_get = NULL;
	zend_standard_class_def.handle_property_set = NULL;
	zend_standard_class_def.refcount = (int *) malloc(sizeof(int));
	*zend_standard_class_def.refcount = 1;
	zend_hash_add(GLOBAL_CLASS_TABLE, "stdclass", sizeof("stdclass"), &zend_standard_class_def, sizeof(zend_class_entry), NULL);
}


static void zend_set_default_compile_time_values(CLS_D)
{
	/* default compile-time values */
	CG(asp_tags) = 0;
	CG(short_tags) = 1;
	CG(allow_call_time_pass_reference) = 1;
	CG(extended_info) = 0;
}


#ifdef ZTS
static void compiler_globals_ctor(zend_compiler_globals *compiler_globals)
{
	zend_function tmp_func;
	zend_class_entry tmp_class;

	compiler_globals->compiled_filename = NULL;

	compiler_globals->function_table = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init_ex(compiler_globals->function_table, 100, NULL, ZEND_FUNCTION_DTOR, 1, 0);
	zend_hash_copy(compiler_globals->function_table, global_function_table, NULL, &tmp_func, sizeof(zend_function));

	compiler_globals->class_table = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init_ex(compiler_globals->class_table, 10, NULL, ZEND_CLASS_DTOR, 1, 0);
	zend_hash_copy(compiler_globals->class_table, global_class_table, (copy_ctor_func_t) zend_class_add_ref, &tmp_class, sizeof(zend_class_entry));

	zend_set_default_compile_time_values(CLS_C);
}


static void compiler_globals_dtor(zend_compiler_globals *compiler_globals)
{
	if (compiler_globals->function_table != global_function_table) {
		zend_hash_destroy(compiler_globals->function_table);
		free(compiler_globals->function_table);
	}
	if (compiler_globals->class_table != global_class_table) {
		zend_hash_destroy(compiler_globals->class_table);
		free(compiler_globals->class_table);
	}
}


static void executor_globals_ctor(zend_executor_globals *executor_globals)
{
	if (global_constants_table) {
		zend_startup_constants(ELS_C);
		zend_copy_constants(executor_globals->zend_constants, global_constants_table);
	}
	zend_init_rsrc_plist(ELS_C);
	EG(lambda_count)=0;
}


static void executor_globals_dtor(zend_executor_globals *executor_globals)
{
	zend_shutdown_constants(ELS_C);
	zend_destroy_rsrc_plist(ELS_C);
}


static void alloc_globals_ctor(zend_alloc_globals *alloc_globals)
{
	start_memory_manager(ALS_C);
}

#endif

#ifdef __FreeBSD__
/* FreeBSD floating point precision fix */
#include <floatingpoint.h>
#endif

int zend_startup(zend_utility_functions *utility_functions, char **extensions, int start_builtin_functions)
{
#ifdef ZTS
	zend_compiler_globals *compiler_globals;
	zend_executor_globals *executor_globals;

	alloc_globals_id = ts_allocate_id(sizeof(zend_alloc_globals), (ts_allocate_ctor) alloc_globals_ctor, NULL);
#else
	start_memory_manager(ALS_C);
#endif

#ifdef __FreeBSD__
	{
		/* FreeBSD floating point precision fix */
#ifdef HAVE_FP_EXCEPT
		fp_except
#else
		fp_except_t
#endif
		mask;

		mask = fpgetmask();
		fpsetmask(mask & ~FP_X_IMP);
	} 
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
	zend_get_ini_entry_p = utility_functions->get_ini_entry;
	zend_ticks_function = utility_functions->ticks_function;

	zend_compile_file = compile_file;
	zend_execute = execute;

	/* set up version */
	zend_version_info = strdup(ZEND_CORE_VERSION_INFO);
	zend_version_info_length = sizeof(ZEND_CORE_VERSION_INFO)-1;

	GLOBAL_FUNCTION_TABLE = (HashTable *) malloc(sizeof(HashTable));
	GLOBAL_CLASS_TABLE = (HashTable *) malloc(sizeof(HashTable));
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
	global_constants_table = NULL;
	compiler_globals_id = ts_allocate_id(sizeof(zend_compiler_globals), (void (*)(void *)) compiler_globals_ctor, (void (*)(void *)) compiler_globals_dtor);
	executor_globals_id = ts_allocate_id(sizeof(zend_executor_globals), (void (*)(void *)) executor_globals_ctor, (void (*)(void *)) executor_globals_dtor);
	compiler_globals = ts_resource(compiler_globals_id);
	executor_globals = ts_resource(executor_globals_id);
	compiler_globals_dtor(compiler_globals);
	compiler_globals->function_table = GLOBAL_FUNCTION_TABLE;
	compiler_globals->class_table = GLOBAL_CLASS_TABLE;
	zend_startup_constants(executor_globals);
	GLOBAL_CONSTANTS_TABLE = EG(zend_constants);
#else
	zend_startup_constants();
	zend_set_default_compile_time_values(CLS_C);
#endif
	zend_register_standard_constants(ELS_C);

#ifndef ZTS
	zend_init_rsrc_plist(ELS_C);
#endif

	if (start_builtin_functions) {
		zend_startup_builtin_functions();
	}

	return SUCCESS;
}


void zend_shutdown()
{
#ifdef ZEND_WIN32
	zend_shutdown_timeout_thread();
#endif
#ifndef ZTS
	zend_destroy_rsrc_plist();
#endif
	zend_destroy_rsrc_list_dtors();
	zend_hash_destroy(&module_registry);
	zend_hash_destroy(GLOBAL_FUNCTION_TABLE);
	free(GLOBAL_FUNCTION_TABLE);
	zend_hash_destroy(GLOBAL_CLASS_TABLE);
	free(GLOBAL_CLASS_TABLE);
	zend_shutdown_extensions();
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
ZEND_API void zend_bailout()
{
	CLS_FETCH();
	ELS_FETCH();

	CG(unclean_shutdown) = 1;
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


void zend_activate(CLS_D ELS_DC)
{
	init_compiler(CLS_C ELS_CC);
	init_executor(CLS_C ELS_CC);
	startup_scanner(CLS_C);
}


void zend_activate_modules()
{
	zend_hash_apply(&module_registry, (int (*)(void *)) module_registry_request_startup);
}

void zend_deactivate_modules()
{
	ELS_FETCH();
	EG(opline_ptr) = NULL; /* we're no longer executing anything */

	if (setjmp(EG(bailout))==0) {
		zend_hash_apply(&module_registry, (int (*)(void *)) module_registry_cleanup);
	}
}

void zend_deactivate(CLS_D ELS_DC)
{
	/* we're no longer executing anything */
	EG(opline_ptr) = NULL; 
	EG(active_symbol_table) = NULL;

	if (setjmp(EG(bailout))==0) {
		shutdown_scanner(CLS_C);
	}
	if (setjmp(EG(bailout))==0) {
		shutdown_executor(ELS_C);
	}
	if (setjmp(EG(bailout))==0) {
		shutdown_compiler(CLS_C);
	}
}


BEGIN_EXTERN_C()
ZEND_API void zend_message_dispatcher(long message, void *data)
{
	if (zend_message_dispatcher_p) {
		zend_message_dispatcher_p(message, data);
	}
}
END_EXTERN_C()


ZEND_API int zend_get_ini_entry(char *name, uint name_length, zval *contents)
{
	if (zend_get_ini_entry_p) {
		return zend_get_ini_entry_p(name, name_length, contents);
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
	ELS_FETCH();
	CLS_FETCH();

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
			if (zend_is_compiling()) {
				error_filename = zend_get_compiled_filename(CLS_C);
				error_lineno = zend_get_compiled_lineno(CLS_C);
			} else if (zend_is_executing()) {
				error_filename = zend_get_executed_filename(ELS_C);
				error_lineno = zend_get_executed_lineno(ELS_C);
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
			if (call_user_function_ex(CG(function_table), NULL, orig_user_error_handler, &retval, 5, params, 1, NULL)==SUCCESS) {
				zval_ptr_dtor(&retval);
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
			if (ZVAL_REFCOUNT(z_context)==2) {
				FREE_ZVAL(z_context);
			}
			break;
	}

	va_end(args);
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


ZEND_API int zend_execute_scripts(int type CLS_DC ELS_DC, int file_count, ...)
{
	va_list files;
	int i;
	zend_file_handle *file_handle;

	va_start(files, file_count);
	for (i=0; i<file_count; i++) {
		file_handle = va_arg(files, zend_file_handle *);
		if (!file_handle) {
			continue;
		}
		EG(active_op_array) = zend_compile_file(file_handle, ZEND_INCLUDE CLS_CC);
		zend_destroy_file_handle(file_handle CLS_CC);
		if (EG(active_op_array)) {
			zend_execute(EG(active_op_array) ELS_CC);
			zval_ptr_dtor(EG(return_value_ptr_ptr));
			EG(return_value_ptr_ptr) = &EG(global_return_value_ptr);
			EG(global_return_value_ptr) = NULL;
			destroy_op_array(EG(active_op_array));
			efree(EG(active_op_array));
		} else if (type==ZEND_REQUIRE) {
			va_end(files);
			return FAILURE;
		}
	}
	va_end(files);

	return SUCCESS;
}

#define COMPILED_STRING_DESCRIPTION_FORMAT "%s(%d) : %s"

ZEND_API char *zend_make_compiled_string_description(char *name)
{
	char *cur_filename;
	int cur_lineno;
	char *compiled_string_description;
	CLS_FETCH();
	ELS_FETCH();

	if (zend_is_compiling()) {
		cur_filename = zend_get_compiled_filename(CLS_C);
		cur_lineno = zend_get_compiled_lineno(CLS_C);
	} else if (zend_is_executing()) {
		cur_filename = zend_get_executed_filename(ELS_C);
		cur_lineno = zend_get_executed_lineno(ELS_C);
	} else {
		cur_filename = "Unknown";
		cur_lineno = 0;
	}

	compiled_string_description = emalloc(sizeof(COMPILED_STRING_DESCRIPTION_FORMAT)+strlen(name)+strlen(cur_filename)+MAX_LENGTH_OF_LONG);
	sprintf(compiled_string_description, COMPILED_STRING_DESCRIPTION_FORMAT, cur_filename, cur_lineno, name);
	return compiled_string_description;
}

