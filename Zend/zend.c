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

/* true multithread-shared globals */
zend_class_entry standard_class;
ZEND_API int (*zend_printf)(const char *format, ...);
ZEND_API int (*zend_write)(const char *str, uint str_length);
ZEND_API void (*zend_error)(int type, const char *format, ...);
void (*zend_message_dispatcher)(long message, void *data);
FILE *(*zend_fopen)(const char *filename);
void (*zend_block_interruptions)();
void (*zend_unblock_interruptions)();

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

ZEND_API int zend_print_zval(zval *expr, int indent)
{
	zval expr_copy;
	int destroy=0;

	if (expr->type != IS_STRING) {
		if (expr->type == IS_BOOL) {
			if (expr->value.lval) {
				expr_copy.value.str.val = estrndup("true",4);
				expr_copy.value.str.len = 4;
			} else {
				expr_copy.value.str.val = estrndup("false",5);
				expr_copy.value.str.len = 5;
			}
			expr_copy.type = IS_STRING;
		} else if (expr->type == IS_RESOURCE) {
			expr_copy.value.str.val = (char *) emalloc(sizeof("Resource id #")-1 + MAX_LENGTH_OF_LONG);
			expr_copy.value.str.len = sprintf(expr_copy.value.str.val, "Resource id #%ld", expr->value.lval);
			expr_copy.type = IS_STRING;
		} else {
			expr_copy = *expr;
			zval_copy_ctor(&expr_copy);
			convert_to_string(&expr_copy);
		}
		expr = &expr_copy;
		destroy=1;
	}
	if (expr->value.str.len==0) { /* optimize away empty strings */
		return 0;
	}
	ZEND_WRITE(expr->value.str.val,expr->value.str.len);
	if (destroy) {
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
	standard_class.type = ZEND_INTERNAL_CLASS;
	standard_class.name_length = sizeof("stdClass") - 1;
	standard_class.name = zend_strndup("stdClass", standard_class.name_length);
	standard_class.parent = NULL;
	zend_hash_init(&standard_class.default_properties, 0, NULL, PVAL_PTR_DTOR, 1);
	zend_hash_init(&standard_class.function_table, 0, NULL, (void (*)(void *)) destroy_zend_function, 1);
	standard_class.handle_function_call = NULL;
	standard_class.handle_property_get = NULL;
	standard_class.handle_property_set = NULL;
	zend_hash_add(CG(class_table), "stdClass", sizeof("stdClass"), &standard_class, sizeof(zend_class_entry), NULL);
}


int zend_startup(zend_utility_functions *utility_functions, zend_utility_values *utility_values, char **extensions)
{
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

	zend_uv = *utility_values;
	zend_llist_init(&zend_extensions, sizeof(zend_extension), (void (*)(void *)) zend_extension_dtor, 1);

	/* set up version */
	zend_version_info = strdup(ZEND_CORE_VERSION_INFO);
	zend_version_info_length = sizeof(ZEND_CORE_VERSION_INFO)-1;

	/* Prepare data structures */
	zend_startup_constants();
	CG(function_table) = (HashTable *) malloc(sizeof(HashTable));
	CG(class_table) = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init(CG(function_table), 100, NULL, (void (*)(void *)) destroy_zend_function, 1);
	zend_hash_init(CG(class_table), 10, NULL, (void (*)(void *)) destroy_zend_class, 1);
	register_standard_class();
	zend_hash_init(&module_registry, 50, NULL, (void (*)(void *)) module_destructor, 1);
	init_resource_plist();
	zend_hash_init(&list_destructors, 50, NULL, NULL, 1);

	return SUCCESS;
}


void zend_shutdown()
{
	destroy_resource_plist();
	zend_hash_destroy(&list_destructors);
	zend_hash_destroy(&module_registry);
	zend_hash_destroy(CG(function_table));
	free(CG(function_table));
	zend_hash_destroy(CG(class_table));
	free(CG(class_table));
	zend_llist_destroy(&zend_extensions);
	free(zend_version_info);
	zend_shutdown_constants();
}


/* this should be compatible with the standard zenderror */
void zenderror(char *error)
{
	zend_error(E_PARSE, error);
}


ZEND_API void zend_bailout()
{
	ELS_FETCH();

	longjmp(EG(bailout), FAILURE);
}


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
