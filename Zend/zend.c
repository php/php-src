/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#include "zend.h"
#include "zend_operators.h"
#include "zend_variables.h"
#include "zend_extensions.h"
#include "modules.h"
#include "zend_constants.h"
#include "zend_list.h"
#include "zend_API.h"

#ifdef ZTS
#	define GLOBAL_FUNCTION_TABLE	global_function_table
#	define GLOBAL_CLASS_TABLE		global_class_table
#else
#	define GLOBAL_FUNCTION_TABLE	CG(function_table)
#	define GLOBAL_CLASS_TABLE		CG(class_table)
#endif

/* true multithread-shared globals */
ZEND_API zend_class_entry zend_standard_class_def;
ZEND_API int (*zend_printf)(const char *format, ...);
ZEND_API int (*zend_write)(const char *str, uint str_length);
ZEND_API void (*zend_error)(int type, const char *format, ...);
void (*zend_message_dispatcher)(long message, void *data);
FILE *(*zend_fopen)(const char *filename);
void (*zend_block_interruptions)();
void (*zend_unblock_interruptions)();
#ifdef ZTS
ZEND_API int compiler_globals_id;
ZEND_API int executor_globals_id;
int alloc_globals_id;
HashTable *global_function_table;
HashTable *global_class_table;
#endif

zend_utility_values zend_uv;

/* version information */
static char *zend_version_info;
static uint zend_version_info_length;
#define ZEND_CORE_VERSION_INFO	"Zend Engine v" ZEND_VERSION ", Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski\n"


#define PRINT_PVAL_INDENT 4

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
	indent += PRINT_PVAL_INDENT;
	zend_hash_internal_pointer_reset(ht);
	while (zend_hash_get_current_data(ht, (void **) &tmp) == SUCCESS) {
		for (i=0; i<indent; i++) {
			ZEND_PUTS(" ");
		}
		ZEND_PUTS("[");
		switch (zend_hash_get_current_key(ht, &string_key, &num_key)) {
			case HASH_KEY_IS_STRING:
				ZEND_PUTS(string_key);
				break;
			case HASH_KEY_IS_LONG:
				zend_printf("%ld",num_key);
				break;
		}
		ZEND_PUTS("] => ");
		zend_print_zval_r(*tmp, indent+PRINT_PVAL_INDENT);
		ZEND_PUTS("\n");
		zend_hash_move_forward(ht);
	}
	indent -= PRINT_PVAL_INDENT;
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
		case IS_BOOL:
			if (expr->value.lval) {
				expr_copy->value.str.len = sizeof("true")-1;
				expr_copy->value.str.val = estrndup("true", expr_copy->value.str.len);
			} else {
				expr_copy->value.str.len = sizeof("false")-1;
				expr_copy->value.str.val = estrndup("false", expr_copy->value.str.len);
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
		case IS_OBJECT: {
				zval function_name;

				function_name.value.str.len = sizeof("to_string")-1;
				function_name.value.str.val = estrndup("to_string", function_name.value.str.len);
				function_name.type = IS_STRING;

				if (call_user_function(NULL, expr, &function_name, expr_copy, 0, NULL)==FAILURE) {
					expr_copy->value.str.len = sizeof("Object")-1;
					expr_copy->value.str.val = estrndup("Object", expr_copy->value.str.len);
				}
				efree(function_name.value.str.val);
			}
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
	ZEND_WRITE(expr->value.str.val,expr->value.str.len);
	if (use_copy) {
		zval_dtor(expr);
	}
	return expr->value.str.len;
}


ZEND_API void zend_print_zval_r(zval *expr, int indent) 
{
	switch(expr->type) {
		case IS_ARRAY:
			ZEND_PUTS("Array\n");
			print_hash(expr->value.ht,indent);
			break;
		case IS_OBJECT:
			zend_printf("%s Object\n", expr->value.obj.ce->name);
			print_hash(expr->value.obj.properties, indent);
			break;
		default:
			zend_print_variable(expr);
			break;
	}
}


static FILE *zend_fopen_wrapper(const char *filename)
{
	return fopen(filename, "r");
}


static void register_standard_class()
{
	zend_standard_class_def.type = ZEND_INTERNAL_CLASS;
	zend_standard_class_def.name_length = sizeof("stdClass") - 1;
	zend_standard_class_def.name = zend_strndup("stdClass", zend_standard_class_def.name_length);
	zend_standard_class_def.parent = NULL;
	zend_hash_init(&zend_standard_class_def.default_properties, 0, NULL, PVAL_PTR_DTOR, 1);
	zend_hash_init(&zend_standard_class_def.function_table, 0, NULL, (void (*)(void *)) destroy_zend_function, 1);
	zend_standard_class_def.handle_function_call = NULL;
	zend_standard_class_def.handle_property_get = NULL;
	zend_standard_class_def.handle_property_set = NULL;
	zend_standard_class_def.refcount = (int *) malloc(sizeof(int));
	*zend_standard_class_def.refcount = 1;
	zend_hash_add(GLOBAL_CLASS_TABLE, "stdClass", sizeof("stdClass"), &zend_standard_class_def, sizeof(zend_class_entry), NULL);
}


#ifdef ZTS
static void compiler_globals_ctor(zend_compiler_globals *compiler_globals)
{
	zend_function tmp_func;
	zend_class_entry tmp_class;

	compiler_globals->function_table = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init(compiler_globals->function_table, 100, NULL, (void (*)(void *)) destroy_zend_function, 1);
	zend_hash_copy(compiler_globals->function_table, global_function_table, NULL, &tmp_func, sizeof(zend_function));

	compiler_globals->class_table = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init(compiler_globals->class_table, 10, NULL, (void (*)(void *)) destroy_zend_class, 1);
	zend_hash_copy(compiler_globals->class_table, global_class_table, zend_class_add_ref, &tmp_class, sizeof(zend_class_entry));
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
	zend_startup_constants(ELS_C);
	init_resource_plist(ELS_C);
}


static void executor_globals_dtor(zend_executor_globals *executor_globals)
{
	zend_shutdown_constants(ELS_C);
	destroy_resource_plist();
}


static void alloc_globals_ctor(zend_alloc_globals *alloc_globals)
{
	start_memory_manager();
}

#endif


int zend_startup(zend_utility_functions *utility_functions, char **extensions)
{
#ifdef ZTS
	zend_compiler_globals *compiler_globals;
	zend_executor_globals *executor_globals;

	alloc_globals_id = ts_allocate_id(sizeof(zend_alloc_globals), alloc_globals_ctor, NULL);
#endif

	start_memory_manager();

	/* Set up utility functions and values */
	zend_error = utility_functions->error_function;
	zend_printf = utility_functions->printf_function;
	zend_write = utility_functions->write_function;
	zend_fopen = utility_functions->fopen_function;
	if (!zend_fopen) {
		zend_fopen = zend_fopen_wrapper;
	}
	zend_message_dispatcher = utility_functions->message_handler;
	zend_block_interruptions = utility_functions->block_interruptions;
	zend_unblock_interruptions = utility_functions->unblock_interruptions;
	zend_compile_files = compile_files;
	zend_execute = execute;

	zend_llist_init(&zend_extensions, sizeof(zend_extension), (void (*)(void *)) zend_extension_dtor, 1);

	/* set up version */
	zend_version_info = strdup(ZEND_CORE_VERSION_INFO);
	zend_version_info_length = sizeof(ZEND_CORE_VERSION_INFO)-1;

	/* Prepare data structures */
#ifndef ZTS
	zend_startup_constants(ELS_C);
#endif
	GLOBAL_FUNCTION_TABLE = (HashTable *) malloc(sizeof(HashTable));
	GLOBAL_CLASS_TABLE = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init(GLOBAL_FUNCTION_TABLE, 100, NULL, (void (*)(void *)) destroy_zend_function, 1);
	zend_hash_init(GLOBAL_CLASS_TABLE, 10, NULL, (void (*)(void *)) destroy_zend_class, 1);
	register_standard_class();
	zend_hash_init(&module_registry, 50, NULL, (void (*)(void *)) module_destructor, 1);
	zend_hash_init(&list_destructors, 50, NULL, NULL, 1);

#ifdef ZTS
	compiler_globals_id = ts_allocate_id(sizeof(zend_compiler_globals), (void (*)(void *)) compiler_globals_ctor, (void (*)(void *)) compiler_globals_dtor);
	executor_globals_id = ts_allocate_id(sizeof(zend_executor_globals), (void (*)(void *)) executor_globals_ctor, (void (*)(void *)) executor_globals_dtor);
	compiler_globals = ts_resource(compiler_globals_id);
	executor_globals = ts_resource(executor_globals_id);
	compiler_globals_dtor(compiler_globals);
	compiler_globals->function_table = GLOBAL_FUNCTION_TABLE;
	compiler_globals->class_table = GLOBAL_CLASS_TABLE;
#endif

#ifndef ZTS
	init_resource_plist(ELS_C);
#endif

	return SUCCESS;
}


void zend_shutdown()
{
#ifndef ZTS
	destroy_resource_plist();
#endif
	zend_hash_destroy(&list_destructors);
	zend_hash_destroy(&module_registry);
	zend_hash_destroy(GLOBAL_FUNCTION_TABLE);
	free(GLOBAL_FUNCTION_TABLE);
	zend_hash_destroy(GLOBAL_CLASS_TABLE);
	free(GLOBAL_CLASS_TABLE);
	zend_shutdown_extensions();
	free(zend_version_info);
#ifndef ZTS
	zend_shutdown_constants(ELS_C);
#endif
}


void zend_set_utility_values(zend_utility_values *utility_values)
{
	zend_uv = *utility_values;
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

void zend_deactivate(CLS_D ELS_DC)
{
	zend_hash_apply(&module_registry, (int (*)(void *)) module_registry_cleanup);
	shutdown_scanner(CLS_C);
	shutdown_executor(ELS_C);
	shutdown_compiler(CLS_C);
}
