/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2005 Zend Technologies Ltd. (http://www.zend.com) |
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
#include "zend_unicode.h"
#include "zend_interfaces.h"
#include "zend_reflection_api.h"

#if defined(ZEND_WIN32) && ZEND_DEBUG
BOOL WINAPI IsDebuggerPresent(VOID);
#endif

/* true multithread-shared globals */
ZEND_API zend_class_entry *zend_standard_class_def = NULL;
ZEND_API int (*zend_printf)(const char *format, ...);
ZEND_API zend_write_func_t zend_write;
ZEND_API FILE *(*zend_fopen)(const char *filename, char **opened_path);
ZEND_API int (*zend_stream_open_function)(const char *filename, zend_file_handle *handle TSRMLS_DC);
ZEND_API void (*zend_block_interruptions)(void);
ZEND_API void (*zend_unblock_interruptions)(void);
ZEND_API void (*zend_ticks_function)(int ticks);
ZEND_API void (*zend_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
int (*zend_vspprintf)(char **pbuf, size_t max_len, const char *format, va_list ap);
ZEND_API char *(*zend_getenv)(char *name, size_t name_len TSRMLS_DC);

void (*zend_on_timeout)(int seconds TSRMLS_DC);

static void (*zend_message_dispatcher_p)(long message, void *data);
static int (*zend_get_configuration_directive_p)(char *name, uint name_length, zval *contents);


static ZEND_INI_MH(OnUpdateErrorReporting)
{
	if (!new_value) {
		EG(error_reporting) = E_ALL & ~E_NOTICE & ~E_STRICT;
	} else {
		EG(error_reporting) = atoi(new_value);
	}
	return SUCCESS;
}


static ZEND_INI_MH(OnUpdateEncoding)
{
	UConverter **converter;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base;

	base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	converter = (UConverter **) (base+(size_t) mh_arg1);

	if (!new_value && converter == &UG(fallback_encoding_conv)) {
		new_value = "UTF-8";
	}

	if (new_value) {
		if (zend_set_converter_encoding(converter, new_value) == FAILURE) {
			zend_error(E_CORE_ERROR, "Unrecognized encoding '%s' used for %s", new_value ?  new_value : "null", entry->name);
			return FAILURE;
		}
	} else {
		if (*converter) {
			ucnv_close(*converter);
		}
		*converter = NULL;
	}
	if (*converter) {
		zend_set_converter_error_mode(*converter, UG(from_u_error_mode));
		zend_set_converter_subst_char(*converter, UG(subst_char), UG(subst_char_len));
	}

	return SUCCESS;
}

#if 0
static ZEND_INI_MH(OnUpdateErrorMode)
{
	uint8_t *error_mode;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base;

	base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	error_mode = (uint8_t *) (base+(size_t) mh_arg1);

	if (new_value) {
		if (!strcasecmp(new_value, "stop")) {
			*error_mode = ZEND_FROM_U_ERROR_STOP;
		} else if (!strcasecmp(new_value, "skip")) {
			*error_mode = ZEND_FROM_U_ERROR_SKIP;
		} else if (!strcasecmp(new_value, "escape")) {
			*error_mode = ZEND_FROM_U_ERROR_ESCAPE;
		} else if (!strcasecmp(new_value, "substitute")) {
			*error_mode = ZEND_FROM_U_ERROR_SUBST;
		} else {
			zend_error(E_WARNING, "Illegal value for conversion error mode");
			return FAILURE;
		}
	}

	return SUCCESS;
}
#endif

static void zend_update_converters_error_behavior(TSRMLS_D)
{
	if (UG(fallback_encoding_conv)) {
		zend_set_converter_error_mode(UG(fallback_encoding_conv), UG(from_u_error_mode));
		zend_set_converter_subst_char(UG(fallback_encoding_conv), UG(subst_char), UG(subst_char_len));
	}
	if (UG(runtime_encoding_conv)) {
		zend_set_converter_error_mode(UG(runtime_encoding_conv), UG(from_u_error_mode));
		zend_set_converter_subst_char(UG(runtime_encoding_conv), UG(subst_char), UG(subst_char_len));
	}
	if (UG(output_encoding_conv)) {
		zend_set_converter_error_mode(UG(output_encoding_conv), UG(from_u_error_mode));
		zend_set_converter_subst_char(UG(output_encoding_conv), UG(subst_char), UG(subst_char_len));
	}
	if (UG(http_input_encoding_conv)) {
		zend_set_converter_error_mode(UG(http_input_encoding_conv), UG(from_u_error_mode));
	}
}


static ZEND_INI_MH(OnUpdateConversionErrorMode)
{
	if (!new_value) {
		UG(from_u_error_mode) = ZEND_FROM_U_ERROR_SUBST;
	} else {
		UG(from_u_error_mode) = atoi(new_value);
	}
	zend_update_converters_error_behavior(TSRMLS_C);
	return SUCCESS;
}


static ZEND_INI_MH(OnUpdateConversionSubstChar)
{
	uint8_t i = 0;
	UChar32 c = 0x3f; /*'?'*/
	char *end_ptr;

	if (new_value) {
		c = (int32_t)strtol(new_value, &end_ptr, 16);
		if (end_ptr < new_value + strlen(new_value)) {
			zend_error(E_WARNING, "Substitution character string should be a hexadecimal Unicode codepoint value");
			return FAILURE;
		}
		if (c < 0 || c >= 0x10FFFF) {
			zend_error(E_WARNING, "Substitution character value U+%06x is out of range 0-10FFFF", c);
			return FAILURE;
		}
	}
	U16_APPEND_UNSAFE(UG(subst_char), i, c);
	UG(subst_char)[i] = 0;
	UG(subst_char_len) = i;
	zend_update_converters_error_behavior(TSRMLS_C);

	return SUCCESS;
}


ZEND_INI_BEGIN()
	ZEND_INI_ENTRY("error_reporting",			NULL,		ZEND_INI_ALL,		OnUpdateErrorReporting)
	STD_ZEND_INI_BOOLEAN("zend.ze1_compatibility_mode",	"0",	ZEND_INI_ALL,		OnUpdateBool,	ze1_compatibility_mode,	zend_executor_globals,	executor_globals)

	/* Unicode .ini entries */
	STD_ZEND_INI_ENTRY("unicode_semantics", "off", ZEND_INI_SYSTEM|ZEND_INI_PERDIR, OnUpdateBool, unicode, zend_unicode_globals, unicode_globals)
	STD_ZEND_INI_ENTRY("unicode.fallback_encoding", NULL, ZEND_INI_ALL, OnUpdateEncoding,   fallback_encoding_conv, zend_unicode_globals, unicode_globals)
	STD_ZEND_INI_ENTRY("unicode.runtime_encoding",  NULL, ZEND_INI_ALL, OnUpdateEncoding,   runtime_encoding_conv, zend_unicode_globals, unicode_globals)
	STD_ZEND_INI_ENTRY("unicode.script_encoding",  NULL, ZEND_INI_ALL, OnUpdateEncoding,   script_encoding_conv, zend_unicode_globals, unicode_globals)
	STD_ZEND_INI_ENTRY("unicode.http_input_encoding",  NULL, ZEND_INI_ALL, OnUpdateEncoding,   http_input_encoding_conv, zend_unicode_globals, unicode_globals)
	ZEND_INI_ENTRY("unicode.from_error_mode", "2", ZEND_INI_ALL, OnUpdateConversionErrorMode)
	ZEND_INI_ENTRY("unicode.from_error_subst_char", "3f", ZEND_INI_ALL, OnUpdateConversionSubstChar)
ZEND_INI_END()


#ifdef ZTS
ZEND_API int compiler_globals_id;
ZEND_API int executor_globals_id;
ZEND_API int alloc_globals_id;
#endif
static HashTable *global_function_table = NULL;
static HashTable *global_class_table = NULL;
static HashTable *global_constants_table = NULL;
static HashTable *global_auto_globals_table = NULL;
static HashTable *global_u_function_table = NULL;
static HashTable *global_u_class_table = NULL;
static HashTable *global_u_constants_table = NULL;
static HashTable *global_u_auto_globals_table = NULL;
static HashTable *global_persistent_list = NULL;

ZEND_API zend_utility_values zend_uv;

ZEND_API zval zval_used_for_init; /* True global variable */

/* version information */
static char *zend_version_info;
static uint zend_version_info_length;
#define ZEND_CORE_VERSION_INFO	"Zend Engine v" ZEND_VERSION ", Copyright (c) 1998-2005 Zend Technologies\n"


#define PRINT_ZVAL_INDENT 4

static void print_hash(HashTable *ht, int indent, zend_bool is_object TSRMLS_DC)
{
	zval **tmp;
	char *string_key;
	HashPosition iterator;
	ulong num_key;
	uint str_len;
	int i;
	zend_uchar ztype;

	for (i=0; i<indent; i++) {
		ZEND_PUTS(" ");
	}
	ZEND_PUTS("(\n");
	indent += PRINT_ZVAL_INDENT;
	zend_hash_internal_pointer_reset_ex(ht, &iterator);
	while (zend_hash_get_current_data_ex(ht, (void **) &tmp, &iterator) == SUCCESS) {
		zend_uchar key_type;

		for (i=0; i<indent; i++) {
			ZEND_PUTS(" ");
		}
		ZEND_PUTS("[");
		switch ((key_type = zend_hash_get_current_key_ex(ht, &string_key, &str_len, &num_key, 0, &iterator))) {
			case HASH_KEY_IS_STRING:
				ztype = IS_STRING;
				goto str_type;
			case HASH_KEY_IS_UNICODE:
				ztype = IS_UNICODE;
				goto str_type;
			case HASH_KEY_IS_BINARY:
				ztype = IS_BINARY;
str_type:
				if (is_object) {
					char *prop_name, *class_name;

					zend_u_unmangle_property_name(ztype, string_key, &class_name, &prop_name);

					if (class_name) {
						if (class_name[0]=='*') {
							zend_printf("%R:protected", ztype, prop_name);
						} else {
							zend_printf("%R:%v:private", ztype, prop_name, class_name);
						}
					} else {
						zend_printf("%R", ztype, prop_name);
					}
				} else {
					zend_printf("%R", ztype, string_key);
				}
				break;
			case HASH_KEY_IS_LONG:
				zend_printf("%ld", num_key);
				break;
		}
		ZEND_PUTS("] => ");
		zend_print_zval_r(*tmp, indent+PRINT_ZVAL_INDENT TSRMLS_CC);
		ZEND_PUTS("\n");
		zend_hash_move_forward_ex(ht, &iterator);
	}
	indent -= PRINT_ZVAL_INDENT;
	for (i=0; i<indent; i++) {
		ZEND_PUTS(" ");
	}
	ZEND_PUTS(")\n");
}

static void print_flat_hash(HashTable *ht TSRMLS_DC)
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
			case HASH_KEY_IS_BINARY:
				zend_printf("b\"%s\"", string_key);
				break;
			case HASH_KEY_IS_UNICODE:
				zend_printf("%r", string_key);
				break;
			case HASH_KEY_IS_LONG:
				zend_printf("%ld", num_key);
				break;
		}
		ZEND_PUTS("] => ");
		zend_print_flat_zval_r(*tmp TSRMLS_CC);
		zend_hash_move_forward_ex(ht, &iterator);
	}
}

ZEND_API void zend_make_string_zval(zval *expr, zval *expr_copy, int *use_copy)
{
	if (expr->type==IS_STRING) {
		*use_copy = 0;
		return;
	}
	switch (expr->type) {
		case IS_OBJECT:
			{
				TSRMLS_FETCH();

				if(Z_OBJ_HT_P(expr)->cast_object && Z_OBJ_HANDLER_P(expr, cast_object)(expr, expr_copy, IS_STRING TSRMLS_CC) == SUCCESS) {
					break;
				}
				if (Z_OBJ_HANDLER_P(expr, get)) {
					zval *z = Z_OBJ_HANDLER_P(expr, get)(expr TSRMLS_CC);
					if(Z_TYPE_P(z) != IS_OBJECT) {
						zend_make_string_zval(z, expr_copy, use_copy);
						FREE_ZVAL(z);
						break;
					}
				}
				zend_error(EG(exception) ? E_ERROR : E_RECOVERABLE_ERROR, "Object of class %v could not be converted to string", Z_OBJCE_P(expr)->name);
				ZVAL_EMPTY_STRING(expr_copy);
				break;
			}
		default:
			*expr_copy = *expr;
			zval_copy_ctor(expr_copy);
			convert_to_string(expr_copy);
			break;
	}
	expr_copy->type = IS_STRING;
	*use_copy = 1;
}

ZEND_API void zend_make_printable_zval(zval *expr, zval *expr_copy, int *use_copy)
{
	UErrorCode temp = U_ZERO_ERROR;
	TSRMLS_FETCH();

	if (expr->type == IS_BINARY ||
	  /* UTODO: clean this up */
	    (expr->type == IS_STRING && 
	    (!strcmp(ucnv_getName(ZEND_U_CONVERTER(UG(output_encoding_conv)), &temp),
				 ucnv_getName(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &temp))))) {
		*use_copy = 0;
		return;
	}
	switch (expr->type) {
		case IS_NULL:
			expr_copy->value.str.len = 0;
			expr_copy->value.str.val = STR_EMPTY_ALLOC();
			break;
		case IS_BOOL:
			if (expr->value.lval) {
				expr_copy->value.str.len = 1;
				expr_copy->value.str.val = estrndup("1", 1);
			} else {
				expr_copy->value.str.len = 0;
				expr_copy->value.str.val = STR_EMPTY_ALLOC();
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
			if(Z_OBJ_HT_P(expr)->cast_object && Z_OBJ_HANDLER_P(expr, cast_object)(expr, expr_copy, IS_STRING TSRMLS_CC) == SUCCESS) {
				break;
			}
			if (Z_OBJ_HANDLER_P(expr, get)) {
				zval *z = Z_OBJ_HANDLER_P(expr, get)(expr TSRMLS_CC);
				if(Z_TYPE_P(z) != IS_OBJECT) {
					zend_make_printable_zval(z, expr_copy, use_copy);
					FREE_ZVAL(z);
					break;
				}
			}
			zend_error(EG(exception) ? E_ERROR : E_RECOVERABLE_ERROR, "Object of class %v could not be converted to string", Z_OBJCE_P(expr)->name);
			ZVAL_EMPTY_STRING(expr_copy);
			break;
		case IS_DOUBLE:
			*expr_copy = *expr;
			zval_copy_ctor(expr_copy);
			zend_locale_sprintf_double(expr_copy ZEND_FILE_LINE_CC);
			break;
		default:
			*expr_copy = *expr;
			zval_copy_ctor(expr_copy);
			if (UG(unicode)) {
				convert_to_string_with_converter(expr_copy, ZEND_U_CONVERTER(UG(output_encoding_conv)));
			} else {
				convert_to_string(expr_copy);
			}
			break;
	}
	expr_copy->type = IS_STRING;
	*use_copy = 1;
}


ZEND_API void zend_make_unicode_zval(zval *expr, zval *expr_copy, int *use_copy)
{
	TSRMLS_FETCH();

	if (expr->type==IS_UNICODE) {
		*use_copy = 0;
		return;
	}
	switch (expr->type) {
		case IS_OBJECT:
			if(Z_OBJ_HT_P(expr)->cast_object && Z_OBJ_HANDLER_P(expr, cast_object)(expr, expr_copy, IS_UNICODE TSRMLS_CC) == SUCCESS) {
				break;
			}
			if (Z_OBJ_HANDLER_P(expr, get)) {
				zval *z = Z_OBJ_HANDLER_P(expr, get)(expr TSRMLS_CC);
				if(Z_TYPE_P(z) != IS_OBJECT) {
					zend_make_unicode_zval(z, expr_copy, use_copy);
					FREE_ZVAL(z);
					break;
				}
			}
			zend_error(EG(exception) ? E_ERROR : E_RECOVERABLE_ERROR, "Object of class %v could not be converted to string", Z_OBJCE_P(expr)->name);
			ZVAL_EMPTY_UNICODE(expr_copy);
			break;
		default:
			*expr_copy = *expr;
			zval_copy_ctor(expr_copy);
			convert_to_unicode(expr_copy);
			break;
	}
	expr_copy->type = IS_UNICODE;
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

ZEND_API void zend_print_flat_zval_r(zval *expr TSRMLS_DC)
{
	switch (expr->type) {
		case IS_ARRAY:
			ZEND_PUTS("Array (");
			if (++expr->value.ht->nApplyCount>1) {
				ZEND_PUTS(" *RECURSION*");
				expr->value.ht->nApplyCount--;
				return;
			}
			print_flat_hash(expr->value.ht TSRMLS_CC);
			ZEND_PUTS(")");
			expr->value.ht->nApplyCount--;
			break;
		case IS_OBJECT:
		{
			HashTable *properties = NULL;
			char *class_name = NULL;
			zend_uint clen;

			if (Z_OBJ_HANDLER_P(expr, get_class_name)) {
				Z_OBJ_HANDLER_P(expr, get_class_name)(expr, &class_name, &clen, 0 TSRMLS_CC);
			}
			if (class_name) {
				zend_printf("%v Object (", class_name);
			} else {
				zend_printf("%s Object (", "Unknown Class");
			}
			if (class_name) {
				efree(class_name);
			}
			if (Z_OBJ_HANDLER_P(expr, get_properties)) {
				properties = Z_OBJPROP_P(expr);
			}
			if (properties) {
				if (++properties->nApplyCount>1) {
					ZEND_PUTS(" *RECURSION*");
					properties->nApplyCount--;
					return;
				}
				print_flat_hash(properties TSRMLS_CC);
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

ZEND_API void zend_print_zval_r(zval *expr, int indent TSRMLS_DC)
{
	zend_print_zval_r_ex(zend_write, expr, indent TSRMLS_CC);
}


ZEND_API void zend_print_zval_r_ex(zend_write_func_t write_func, zval *expr, int indent TSRMLS_DC)
{
	switch (expr->type) {
		case IS_ARRAY:
			ZEND_PUTS("Array\n");
			if (++expr->value.ht->nApplyCount>1) {
				ZEND_PUTS(" *RECURSION*");
				expr->value.ht->nApplyCount--;
				return;
			}
			print_hash(expr->value.ht, indent, 0 TSRMLS_CC);
			expr->value.ht->nApplyCount--;
			break;
		case IS_OBJECT:
			{
				HashTable *properties = NULL;
				char *class_name = NULL;
				zend_uint clen;

				if (Z_OBJ_HANDLER_P(expr, get_class_name)) {
					Z_OBJ_HANDLER_P(expr, get_class_name)(expr, &class_name, &clen, 0 TSRMLS_CC);
				}
				if (class_name) {
					zend_printf("%v Object\n", class_name);
				} else {
					zend_printf("%s Object\n", "Unknown Class");
				}
				if (class_name) {
					efree(class_name);
				}
				if (Z_OBJ_HANDLER_P(expr, get_properties)) {
					properties = Z_OBJPROP_P(expr);
				}
				if (properties) {
					if (++properties->nApplyCount>1) {
						ZEND_PUTS(" *RECURSION*");
						properties->nApplyCount--;
						return;
					}
					print_hash(properties, indent, 1 TSRMLS_CC);
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


static void register_standard_class(TSRMLS_D)
{
	zend_standard_class_def = calloc(1, sizeof(zend_class_entry));

	zend_standard_class_def->type = ZEND_INTERNAL_CLASS;
	zend_standard_class_def->name_length = sizeof("stdClass") - 1;
	zend_standard_class_def->name = zend_strndup("stdClass", zend_standard_class_def->name_length);
	zend_initialize_class_data(zend_standard_class_def, 1 TSRMLS_CC);

	zend_hash_add(CG(class_table), "stdclass", sizeof("stdclass"), &zend_standard_class_def, sizeof(zend_class_entry *), NULL);
}

static void zend_set_default_compile_time_values(TSRMLS_D)
{
	/* default compile-time values */
	CG(asp_tags) = 0;
	CG(short_tags) = 1;
	CG(allow_call_time_pass_reference) = 1;
	CG(extended_info) = 0;
}


#define ZEND_U_FUNCTION_DTOR (void (*)(void *)) zend_u_function_dtor
#define ZEND_U_CONSTANT_DTOR (void (*)(void *)) free_u_zend_constant

static void zval_copy_persistent(zval *zv)
{
	if (Z_TYPE_P(zv) == IS_BINARY) {
		Z_BINVAL_P(zv) = zend_strndup(Z_BINVAL_P(zv), Z_BINLEN_P(zv));
	} else if (Z_TYPE_P(zv) == IS_UNICODE) {
		Z_USTRVAL_P(zv) = zend_ustrndup(Z_USTRVAL_P(zv), Z_USTRLEN_P(zv));
	} else if (Z_TYPE_P(zv) == IS_STRING || Z_TYPE_P(zv) == IS_CONSTANT) {
		UChar *ustr;

		ustr = malloc(UBYTES(Z_STRLEN_P(zv)+1));
		u_charsToUChars(Z_STRVAL_P(zv), ustr, Z_STRLEN_P(zv)+1);
		Z_USTRVAL_P(zv) = ustr;
		if (Z_TYPE_P(zv) == IS_STRING) Z_TYPE_P(zv) = IS_UNICODE;
	}
}

static void zend_u_function_dtor(zend_function *function)
{
	TSRMLS_FETCH();

	destroy_zend_function(function TSRMLS_CC);
	if (function->type == ZEND_INTERNAL_FUNCTION) {
		if (function->common.function_name) {
			free(function->common.function_name);
		}
		if (function->common.arg_info) {
			int n = function->common.num_args;

			while (n > 0) {
		  	--n;
			  if (function->common.arg_info[n].name) {
			  	free(function->common.arg_info[n].name);
			  }
			  if (function->common.arg_info[n].class_name) {
					free(function->common.arg_info[n].class_name);
				}
		  }
			free(function->common.arg_info);
		}
	}
}

static void free_u_zend_constant(zend_constant *c)
{
	if (!(c->flags & CONST_PERSISTENT)) {
		zval_dtor(&c->value);
	} else {
		zval_internal_dtor(&c->value);
	}
	free(c->name);
}

static void copy_u_zend_constant(zend_constant *c)
{
	c->name = (char*)zend_ustrndup((UChar*)c->name, c->name_len - 1);
	if (!(c->flags & CONST_PERSISTENT)) {
		zval_copy_ctor(&c->value);
	}
}

static void function_to_unicode(zend_function *func)
{
	if (func->common.function_name) {
		UChar *uname;
		int   len = strlen(func->common.function_name)+1;

		uname = malloc(UBYTES(len));
		u_charsToUChars(func->common.function_name, uname, len);
		func->common.function_name = (char*)uname;
	}
	if (func->common.arg_info) {
		zend_arg_info *args;
		int n = func->common.num_args;

		args = malloc((n + 1) * sizeof(zend_arg_info));
		memcpy(args, func->common.arg_info, (n + 1) * sizeof(zend_arg_info));
		while (n > 0) {
		  --n;
		  if (args[n].name) {
				UChar *uname = malloc(UBYTES(args[n].name_len));
				u_charsToUChars(args[n].name, uname, args[n].name_len);
				args[n].name = (char*)uname;
		  }
		  if (args[n].class_name) {
				UChar *uname = malloc(UBYTES(args[n].class_name_len));
				u_charsToUChars(args[n].class_name, uname, args[n].class_name_len);
				args[n].class_name = (char*)uname;
		  }
		}
		func->common.arg_info = args;
	}
}

static void property_info_to_unicode(zend_property_info *info)
{
	if (info->name) {
		UChar *uname;

		uname = malloc(UBYTES(info->name_length+1));
		u_charsToUChars(info->name, uname, info->name_length+1);
		info->name = (char*)uname;
		info->h = zend_u_get_hash_value(IS_UNICODE, info->name, info->name_length+1);
	}
}

static void zval_ptr_to_unicode(zval **zv)
{
	zval *new_zv = malloc(sizeof(zval));

	memcpy(new_zv, *zv, sizeof(zval));
	new_zv->refcount = 1;
	zval_copy_persistent(new_zv);
	*zv = new_zv;
}

static void const_to_unicode(zend_constant *c)
{
	UChar *uname;

	if (c->name) {
		uname = malloc(UBYTES(c->name_len));
		u_charsToUChars(c->name, uname, c->name_len);
		c->name = (char*)uname;
	}
	zval_copy_persistent(&c->value);
}

static void class_to_unicode(zend_class_entry **ce)
{
	zend_class_entry *new_ce = malloc(sizeof(zend_class_entry));
	zend_property_info tmp_info;
	zval* tmp_zval;

	memcpy(new_ce, *ce, sizeof(zend_class_entry));
	(*ce)->u_twin = new_ce;

	/* Convert name to unicode */
	if (new_ce->name) {
		UChar *uname = malloc(UBYTES(new_ce->name_length+1));

		u_charsToUChars(new_ce->name, uname, new_ce->name_length+1);
		new_ce->name = (char*)uname;
	}

	/* Copy methods */
	zend_u_hash_init_ex(&new_ce->function_table, (*ce)->function_table.nNumOfElements, NULL, ZEND_U_FUNCTION_DTOR, 1, 1, 0);
	{
		Bucket *p = (*ce)->function_table.pListHead;
		while (p != NULL) {
			zend_function *src = (zend_function*)p->pData;
			zend_function *target;
			zend_function tmp_func = *src;

			function_to_unicode(&tmp_func);
			
			zend_hash_add(&new_ce->function_table, p->key.u.string, p->nKeyLength, &tmp_func, sizeof(zend_function), (void**)&target);
			src->common.u_twin = target;

		  p = p->pListNext;
		}
	}
/*
	zend_hash_copy(&new_ce->function_table, &(*ce)->function_table, (copy_ctor_func_t) function_to_unicode, &tmp_func, sizeof(zend_function));
*/
	/* Copy constants */
	zend_u_hash_init_ex(&new_ce->constants_table, (*ce)->constants_table.nNumOfElements, NULL, (*ce)->constants_table.pDestructor, 1, 1, 0);
	zend_hash_copy(&new_ce->constants_table, &(*ce)->constants_table, (copy_ctor_func_t) zval_ptr_to_unicode, &tmp_zval, sizeof(zval*));

	/* Copy properties */
	zend_u_hash_init_ex(&new_ce->properties_info, (*ce)->properties_info.nNumOfElements, NULL, (*ce)->properties_info.pDestructor/*(dtor_func_t)zend_destroy_property_info_internal*/, 1, 1, 0);
	zend_hash_copy(&new_ce->properties_info, &(*ce)->properties_info, (copy_ctor_func_t) property_info_to_unicode, &tmp_info, sizeof(zend_property_info));

	zend_u_hash_init_ex(&new_ce->default_properties, (*ce)->default_properties.nNumOfElements, NULL, (*ce)->default_properties.pDestructor, 1, 1, 0);
	zend_hash_copy(&new_ce->default_properties, &(*ce)->default_properties, (copy_ctor_func_t) zval_ptr_to_unicode, &tmp_zval, sizeof(zval*));

	zend_u_hash_init_ex(&new_ce->default_static_members, (*ce)->default_static_members.nNumOfElements, NULL, (*ce)->default_static_members.pDestructor, 1, 1, 0);

	{
		HashPosition pos;
		zval **p;

		zend_hash_internal_pointer_reset_ex(&(*ce)->default_static_members, &pos);
		while (zend_hash_get_current_data_ex(&(*ce)->default_static_members, (void**)&p, &pos) == SUCCESS) {
			char *str_index;
			uint str_length;
			ulong num_index;
			zval **q;

			zend_hash_get_current_key_ex(&(*ce)->default_static_members, &str_index, &str_length, &num_index, 0, &pos);
			if ((*p)->is_ref &&
			    (*ce)->parent &&
			    zend_hash_find(&(*ce)->parent->default_static_members, str_index, str_length, (void**)&q) == SUCCESS &&
			    *p == *q &&
			    zend_hash_find(&new_ce->parent->u_twin->default_static_members, str_index, str_length, (void**)&q) == SUCCESS) {
				(*q)->refcount++;
				(*q)->is_ref = 1;
				zend_hash_add(&new_ce->default_static_members, str_index, str_length, (void**)q, sizeof(zval*), NULL);
			} else {
				zval *q = *p;
				zval_ptr_to_unicode(&q);
				q->is_ref = 0;
				zend_hash_add(&new_ce->default_static_members, str_index, str_length, (void**)&q, sizeof(zval*), NULL);
			}
			zend_hash_move_forward_ex(&(*ce)->default_static_members, &pos);
		}
	}

	*ce = new_ce;
}

static void fix_classes(HashTable *ht) {
	Bucket *p = ht->pListHead;
	Bucket *q;

	/* Fix parent classes */
	while (p != NULL) {
		zend_class_entry *ce = *(zend_class_entry**)p->pData;

		if (ce->parent) {
			ce->parent = ce->parent->u_twin;
		}
		if (ce->num_interfaces > 0 && ce->interfaces) {
			int i = sizeof(zend_class_entry*)*ce->num_interfaces;
			zend_class_entry **new_interfaces;

			new_interfaces = (zend_class_entry**)malloc(i);
			memcpy(new_interfaces, ce->interfaces, i);
			ce->interfaces = new_interfaces;
			for (i = 0; i < ce->num_interfaces; i++) {
				ce->interfaces[i] = ce->interfaces[i]->u_twin;
			}
		}
		
		q = ce->function_table.pListHead;
		while (q != NULL) {
			zend_function *f = (zend_function*)q->pData;

			if (f->common.scope) {
				f->common.scope = f->common.scope->u_twin;
			}			
			if (f->common.prototype) {
				f->common.prototype = f->common.prototype->common.u_twin;
			}
			q = q->pListNext;
		}

		if (ce->constructor) {
			ce->constructor = ce->constructor->common.u_twin;
		} else if (ce->destructor) {
			ce->destructor = ce->destructor->common.u_twin;
		} else if (ce->clone) {
			ce->clone = ce->clone->common.u_twin;
		} else if (ce->__get) {
			ce->__get = ce->__get->common.u_twin;
		} else if (ce->__set) {
			ce->__set = ce->__set->common.u_twin;
		} else if (ce->__unset) {
			ce->__unset = ce->__unset->common.u_twin;
		} else if (ce->__isset) {
			ce->__isset = ce->__isset->common.u_twin;
		} else if (ce->__call) {
			ce->__call = ce->__call->common.u_twin;
		} else if (ce->__tostring) {
			ce->__tostring = ce->__tostring->common.u_twin;
		} else if (ce->serialize_func) {
			ce->serialize_func = ce->serialize_func->common.u_twin;
		} else if (ce->unserialize_func) {
			ce->unserialize_func = ce->unserialize_func->common.u_twin;
		}

		p = p->pListNext;
	}

}

#ifdef ZTS
static void compiler_globals_ctor(zend_compiler_globals *compiler_globals TSRMLS_DC)
{
	zend_function tmp_func;
	zend_class_entry *tmp_class;

	compiler_globals->compiled_filename = NULL;

	compiler_globals->global_function_table = compiler_globals->function_table = (HashTable *) malloc(sizeof(HashTable));
	zend_u_hash_init_ex(compiler_globals->function_table, 100, NULL, ZEND_FUNCTION_DTOR, 1, 0, 0);
	zend_hash_copy(compiler_globals->function_table, global_function_table, NULL, &tmp_func, sizeof(zend_function));

	if (global_u_function_table) {
		compiler_globals->global_u_function_table = (HashTable *) malloc(sizeof(HashTable));
		zend_u_hash_init_ex(compiler_globals->global_u_function_table, 100, NULL, ZEND_FUNCTION_DTOR, 1, 1, 0);
		zend_hash_copy(compiler_globals->global_u_function_table, global_u_function_table, NULL, &tmp_func, sizeof(zend_function));
	} else {
		compiler_globals->global_u_function_table = 0;
	}

	compiler_globals->global_class_table = compiler_globals->class_table = (HashTable *) malloc(sizeof(HashTable));
	zend_u_hash_init_ex(compiler_globals->class_table, 10, NULL, ZEND_CLASS_DTOR, 1, 0, 0);
	zend_hash_copy(compiler_globals->class_table, global_class_table, (copy_ctor_func_t) zend_class_add_ref, &tmp_class, sizeof(zend_class_entry *));

	if (global_u_class_table) {
		compiler_globals->global_u_class_table =  (HashTable *) malloc(sizeof(HashTable));
		zend_u_hash_init_ex(compiler_globals->global_u_class_table, 10, NULL, ZEND_CLASS_DTOR, 1, 1, 0);
		zend_hash_copy(compiler_globals->global_u_class_table, global_u_class_table, (copy_ctor_func_t) zend_class_add_ref, &tmp_class, sizeof(zend_class_entry *));
	} else {
		compiler_globals->global_u_class_table = NULL;
	}
	zend_set_default_compile_time_values(TSRMLS_C);

	CG(interactive) = 0;
	CG(literal_type) = IS_STRING;

	compiler_globals->global_auto_globals_table = compiler_globals->auto_globals = (HashTable *) malloc(sizeof(HashTable));
	zend_u_hash_init_ex(compiler_globals->auto_globals, 8, NULL, NULL, 1, 0, 0);
	zend_hash_copy(compiler_globals->auto_globals, global_auto_globals_table, NULL, NULL, sizeof(zend_auto_global) /* empty element */);

	if (global_u_auto_globals_table) {
		compiler_globals->global_u_auto_globals_table = (HashTable *) malloc(sizeof(HashTable));
		zend_u_hash_init_ex(compiler_globals->global_u_auto_globals_table, 8, NULL, NULL, 1, 1, 0);
		zend_hash_copy(compiler_globals->global_u_auto_globals_table, global_u_auto_globals_table, NULL, NULL, sizeof(zend_auto_global) /* empty element */);
	} else {
		compiler_globals->global_u_auto_globals_table = NULL;
	}
}


static void compiler_globals_dtor(zend_compiler_globals *compiler_globals TSRMLS_DC)
{
	if (compiler_globals->function_table != global_function_table &&
	    compiler_globals->function_table != global_u_function_table) {
		if (compiler_globals->global_function_table) {
			zend_hash_destroy(compiler_globals->global_function_table);
			free(compiler_globals->global_function_table);
		}
		if (compiler_globals->global_u_function_table) {
			zend_hash_destroy(compiler_globals->global_u_function_table);
			free(compiler_globals->global_u_function_table);
		}
	}
	if (compiler_globals->class_table != global_class_table &&
	    compiler_globals->class_table != global_u_class_table) {
		if (compiler_globals->global_class_table) {
			zend_hash_destroy(compiler_globals->global_class_table);
			free(compiler_globals->global_class_table);
		}
		if (compiler_globals->global_u_class_table) {
			zend_hash_destroy(compiler_globals->global_u_class_table);
			free(compiler_globals->global_u_class_table);
		}
	}
	if (compiler_globals->auto_globals != global_auto_globals_table &&
	    compiler_globals->auto_globals != global_u_auto_globals_table) {
		if (compiler_globals->global_auto_globals_table) {
			zend_hash_destroy(compiler_globals->global_auto_globals_table);
			free(compiler_globals->global_auto_globals_table);
		}
		if (compiler_globals->global_u_auto_globals_table) {
			zend_hash_destroy(compiler_globals->global_u_auto_globals_table);
			free(compiler_globals->global_u_auto_globals_table);
		}
	}
}


static void executor_globals_ctor(zend_executor_globals *executor_globals TSRMLS_DC)
{
	zend_constant tmp_const;

	zend_startup_constants(TSRMLS_C);
	zend_copy_constants(EG(zend_constants), global_constants_table);
	EG(global_constants_table) = EG(zend_constants);

	if (global_u_constants_table) {
		EG(global_u_constants_table) = (HashTable *) malloc(sizeof(HashTable));
		zend_u_hash_init_ex(EG(global_u_constants_table), global_u_constants_table->nNumOfElements, NULL, ZEND_CONSTANT_DTOR, 1, 1, 0);
		zend_hash_copy(EG(global_u_constants_table), global_u_constants_table, (copy_ctor_func_t) copy_u_zend_constant, &tmp_const, sizeof(zend_constant));
	} else {
		EG(global_u_constants_table) = NULL;
	}

	zend_init_rsrc_plist(TSRMLS_C);
	EG(lambda_count)=0;
	EG(user_error_handler) = NULL;
	EG(user_exception_handler) = NULL;
	EG(in_execution) = 0;
	EG(in_autoload) = NULL;
	EG(current_execute_data) = NULL;
	EG(current_module) = NULL;
}


static void executor_globals_dtor(zend_executor_globals *executor_globals TSRMLS_DC)
{
	zend_ini_shutdown(TSRMLS_C);
	if (&executor_globals->persistent_list != global_persistent_list) {
		zend_destroy_rsrc_list(&executor_globals->persistent_list TSRMLS_CC);
	}
	if (executor_globals->global_constants_table != global_constants_table &&
	    executor_globals->global_u_constants_table != global_u_function_table) {
		if (executor_globals->global_constants_table) {
			zend_hash_destroy(executor_globals->global_constants_table);
			free(executor_globals->global_constants_table);
		}
		if (executor_globals->global_u_constants_table) {
			zend_hash_destroy(executor_globals->global_u_constants_table);
			free(executor_globals->global_u_constants_table);
		}
	}
}


static void zend_new_thread_end_handler(THREAD_T thread_id TSRMLS_DC)
{
	zend_copy_ini_directives(TSRMLS_C);
	zend_ini_refresh_caches(ZEND_INI_STAGE_STARTUP TSRMLS_CC);
}

static void alloc_globals_dtor(zend_alloc_globals *alloc_globals_p TSRMLS_DC)
{
	shutdown_memory_manager(0, 1 TSRMLS_CC);
}
#endif

static void alloc_globals_ctor(zend_alloc_globals *alloc_globals_p TSRMLS_DC)
{
	start_memory_manager(TSRMLS_C);
}

#if defined(__FreeBSD__) || defined(__DragonFly__)
/* FreeBSD and DragonFly floating point precision fix */
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

static void unicode_globals_ctor(zend_unicode_globals *unicode_globals TSRMLS_DC)
{
	unicode_globals->unicode = 0;
	unicode_globals->utf8_conv = NULL;
	unicode_globals->fallback_encoding_conv = NULL;
	unicode_globals->runtime_encoding_conv = NULL;
	unicode_globals->output_encoding_conv = NULL;
	unicode_globals->script_encoding_conv = NULL;
	unicode_globals->http_input_encoding_conv = NULL;
	unicode_globals->subst_char_len = 0;
	zend_set_converter_encoding(&unicode_globals->utf8_conv, "UTF-8");
	unicode_globals->from_u_error_mode = ZEND_FROM_U_ERROR_SUBST;

	zend_hash_init_ex(&unicode_globals->flex_compatible, 0, NULL, NULL, 1, 0);
}

static void unicode_globals_dtor(zend_unicode_globals *unicode_globals TSRMLS_DC)
{
	if (unicode_globals->fallback_encoding_conv) {
		ucnv_close(unicode_globals->fallback_encoding_conv);
	}
	if (unicode_globals->runtime_encoding_conv) {
		ucnv_close(unicode_globals->runtime_encoding_conv);
	}
	if (unicode_globals->output_encoding_conv) {
		ucnv_close(unicode_globals->output_encoding_conv);
	}
	if (unicode_globals->script_encoding_conv) {
		ucnv_close(unicode_globals->script_encoding_conv);
	}
	if (unicode_globals->http_input_encoding_conv) {
		ucnv_close(unicode_globals->http_input_encoding_conv);
	}
	if (unicode_globals->utf8_conv) {
		ucnv_close(unicode_globals->utf8_conv);
	}
	zend_hash_destroy(&unicode_globals->flex_compatible);
}

void zend_init_opcodes_handlers();

int zend_startup(zend_utility_functions *utility_functions, char **extensions, int start_builtin_functions)
{
#ifdef ZTS
	zend_compiler_globals *compiler_globals;
	zend_executor_globals *executor_globals;
	void ***tsrm_ls;
	extern ZEND_API ts_rsrc_id ini_scanner_globals_id;
	extern ZEND_API ts_rsrc_id language_scanner_globals_id;
	extern ZEND_API ts_rsrc_id unicode_globals_id;
#else
	extern zend_scanner_globals ini_scanner_globals;
	extern zend_scanner_globals language_scanner_globals;
	extern zend_unicode_globals unicode_globals;
#endif

#ifdef ZTS
	ts_allocate_id(&alloc_globals_id, sizeof(zend_alloc_globals), (ts_allocate_ctor) alloc_globals_ctor, (ts_allocate_dtor) alloc_globals_dtor);
#else
	alloc_globals_ctor(&alloc_globals TSRMLS_CC);
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
	/* FreeBSD and DragonFly floating point precision fix */
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
	zend_stream_open_function = utility_functions->stream_open_function;
	zend_message_dispatcher_p = utility_functions->message_handler;
	zend_block_interruptions = utility_functions->block_interruptions;
	zend_unblock_interruptions = utility_functions->unblock_interruptions;
	zend_get_configuration_directive_p = utility_functions->get_configuration_directive;
	zend_ticks_function = utility_functions->ticks_function;
	zend_on_timeout = utility_functions->on_timeout;
	zend_vspprintf = utility_functions->vspprintf_function;
	zend_getenv = utility_functions->getenv_function;

	zend_compile_file = compile_file;
	zend_execute = execute;
	zend_execute_internal = NULL;
	zend_throw_exception_hook = NULL;

	zend_init_opcodes_handlers();

	/* set up version */
	zend_version_info = strdup(ZEND_CORE_VERSION_INFO);
	zend_version_info_length = sizeof(ZEND_CORE_VERSION_INFO)-1;

	global_function_table = (HashTable *) malloc(sizeof(HashTable));
	global_class_table = (HashTable *) malloc(sizeof(HashTable));
	global_auto_globals_table = (HashTable *) malloc(sizeof(HashTable));
	global_constants_table = (HashTable *) malloc(sizeof(HashTable));

	zend_hash_init_ex(global_function_table, 100, NULL, ZEND_FUNCTION_DTOR, 1, 0);
	zend_hash_init_ex(global_class_table, 10, NULL, ZEND_CLASS_DTOR, 1, 0);
	zend_hash_init_ex(global_auto_globals_table, 8, NULL, (dtor_func_t) zend_auto_global_dtor, 1, 0);
	zend_hash_init_ex(global_constants_table, 20, NULL, ZEND_CONSTANT_DTOR, 1, 0);

	zend_hash_init_ex(&module_registry, 50, NULL, ZEND_MODULE_DTOR, 1, 0);
	zend_init_rsrc_list_dtors();


	/* This zval can be used to initialize allocate zval's to an uninit'ed value */
	zval_used_for_init.is_ref = 0;
	zval_used_for_init.refcount = 1;
	zval_used_for_init.type = IS_NULL;

#ifdef ZTS
	ts_allocate_id(&unicode_globals_id, sizeof(zend_unicode_globals), (ts_allocate_ctor) unicode_globals_ctor, (ts_allocate_dtor) unicode_globals_dtor);
	ts_allocate_id(&compiler_globals_id, sizeof(zend_compiler_globals), (ts_allocate_ctor) compiler_globals_ctor, (ts_allocate_dtor) compiler_globals_dtor);
	ts_allocate_id(&executor_globals_id, sizeof(zend_executor_globals), (ts_allocate_ctor) executor_globals_ctor, (ts_allocate_dtor) executor_globals_dtor);
	ts_allocate_id(&language_scanner_globals_id, sizeof(zend_scanner_globals), (ts_allocate_ctor) scanner_globals_ctor, NULL);
	ts_allocate_id(&ini_scanner_globals_id, sizeof(zend_scanner_globals), (ts_allocate_ctor) scanner_globals_ctor, NULL);
	compiler_globals = ts_resource(compiler_globals_id);
	executor_globals = ts_resource(executor_globals_id);
	tsrm_ls = ts_resource_ex(0, NULL);

	compiler_globals_dtor(compiler_globals TSRMLS_CC);
	compiler_globals->in_compilation = 0;
	compiler_globals->function_table = (HashTable *) malloc(sizeof(HashTable));
	compiler_globals->class_table = (HashTable *) malloc(sizeof(HashTable));

	*compiler_globals->function_table = *global_function_table;
	*compiler_globals->class_table = *global_class_table;
	compiler_globals->auto_globals = global_auto_globals_table;

	zend_hash_destroy(executor_globals->zend_constants);
	*executor_globals->zend_constants = *global_constants_table;
#else
	unicode_globals_ctor(&unicode_globals TSRMLS_CC);
	scanner_globals_ctor(&ini_scanner_globals TSRMLS_CC);
	scanner_globals_ctor(&language_scanner_globals TSRMLS_CC);

	CG(function_table) = global_function_table;
	CG(class_table) = global_class_table;
	CG(auto_globals) = global_auto_globals_table;
	EG(zend_constants) = global_constants_table;

	zend_set_default_compile_time_values(TSRMLS_C);
	EG(user_error_handler) = NULL;
	EG(user_exception_handler) = NULL;
#endif
	register_standard_class(TSRMLS_C);
	zend_register_standard_constants(TSRMLS_C);
 	zend_register_auto_global("GLOBALS", sizeof("GLOBALS")-1, NULL TSRMLS_CC);

#ifndef ZTS
	zend_init_rsrc_plist(TSRMLS_C);
	global_persistent_list = &EG(persistent_list);
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

/* Unlink the global (r/o) copies of the class, function and constant tables,
 * and use a fresh r/w copy for the startup thread
 */
void zend_post_startup(TSRMLS_D)
{
	UConverter *old_runtime_encoding_conv;
  UErrorCode status = U_ZERO_ERROR;
	zend_function tmp_func;
	zend_class_entry *tmp_class;
	zend_constant tmp_const;
#ifdef ZTS
	zend_compiler_globals *compiler_globals = ts_resource(compiler_globals_id);
	zend_executor_globals *executor_globals = ts_resource(executor_globals_id);

	*global_function_table = *compiler_globals->function_table;
	*global_class_table = *compiler_globals->class_table;
	*global_constants_table = *executor_globals->zend_constants;
#endif

	/* Make copies of HashTables with UNICODE */

	old_runtime_encoding_conv = UG(runtime_encoding_conv);
	UG(runtime_encoding_conv) = ucnv_open("ASCII", &status);

	global_u_function_table = (HashTable *) malloc(sizeof(HashTable));
	zend_u_hash_init_ex(global_u_function_table, global_function_table->nNumOfElements, NULL, ZEND_U_FUNCTION_DTOR, 1, 1, 0);
	zend_hash_copy(global_u_function_table, global_function_table, (copy_ctor_func_t) function_to_unicode, &tmp_func, sizeof(zend_function));

	global_u_class_table = (HashTable *) malloc(sizeof(HashTable));
	zend_u_hash_init_ex(global_u_class_table, global_class_table->nNumOfElements, NULL, ZEND_CLASS_DTOR, 1, 1, 0);
	zend_hash_copy(global_u_class_table, global_class_table, (copy_ctor_func_t) class_to_unicode, &tmp_class, sizeof(zend_class_entry *));
	fix_classes(global_u_class_table);

	global_u_auto_globals_table = (HashTable *) malloc(sizeof(HashTable));
	zend_u_hash_init_ex(global_u_auto_globals_table, global_auto_globals_table->nNumOfElements, NULL, NULL, 1, 1, 0);
	zend_hash_copy(global_u_auto_globals_table, global_auto_globals_table, NULL, NULL, sizeof(zend_auto_global));

	global_u_constants_table = (HashTable *) malloc(sizeof(HashTable));
	zend_u_hash_init_ex(global_u_constants_table, global_constants_table->nNumOfElements, NULL, ZEND_U_CONSTANT_DTOR, 1, 1, 0);
	zend_hash_copy(global_u_constants_table, global_constants_table, (copy_ctor_func_t) const_to_unicode, &tmp_const, sizeof(zend_constant));

  ucnv_close(UG(runtime_encoding_conv));
	UG(runtime_encoding_conv) = old_runtime_encoding_conv;

#ifdef ZTS
	zend_destroy_rsrc_list(&EG(persistent_list) TSRMLS_CC);
	free(compiler_globals->function_table);
	free(compiler_globals->class_table);
	compiler_globals_ctor(compiler_globals, tsrm_ls);
	free(EG(zend_constants));
	executor_globals_ctor(executor_globals, tsrm_ls);
	global_persistent_list = &EG(persistent_list);
	zend_new_thread_end_handler(tsrm_thread_id() TSRMLS_CC);
#endif
}


void zend_shutdown(TSRMLS_D)
{
#ifdef ZEND_WIN32
	zend_shutdown_timeout_thread();
#endif
	if (global_persistent_list) {
		zend_destroy_rsrc_list(global_persistent_list TSRMLS_CC);
	}
	zend_hash_graceful_reverse_destroy(&module_registry);

	zend_hash_destroy(global_function_table);
	zend_hash_destroy(global_class_table);
	if (global_u_function_table) {
		zend_hash_destroy(global_u_function_table);
	}
	if (global_u_class_table) {
		zend_hash_destroy(global_u_class_table);
	}

	zend_hash_destroy(global_auto_globals_table);
	free(global_auto_globals_table);
	if (global_u_auto_globals_table) {
		zend_hash_destroy(global_u_auto_globals_table);
		free(global_u_auto_globals_table);
	}

	zend_shutdown_extensions(TSRMLS_C);
	free(zend_version_info);

	zend_hash_destroy(global_constants_table);
	free(global_constants_table);
	if (global_u_constants_table) {
		zend_hash_destroy(global_u_constants_table);
		free(global_u_constants_table);
	}
	
	free(global_function_table);
	free(global_class_table);
	if (global_u_function_table) {
		free(global_u_function_table);
	}
	if (global_u_class_table) {
		free(global_u_class_table);
	}
#ifdef ZTS
	global_function_table = NULL;
	global_u_function_table = NULL;
	global_class_table = NULL;
	global_u_class_table = NULL;
	global_auto_globals_table = NULL;
	global_u_auto_globals_table = NULL;
	global_constants_table = NULL;
	global_u_constants_table = NULL;
#else
	unicode_globals_dtor(&unicode_globals TSRMLS_CC);
#endif
	zend_destroy_rsrc_list_dtors();
}


void zend_set_utility_values(zend_utility_values *utility_values)
{
	zend_uv = *utility_values;
	zend_uv.import_use_extension_length = strlen(zend_uv.import_use_extension);
}


/* this should be compatible with the standard zenderror */
void zenderror(char *error)
{
	zend_error(E_PARSE, "%s", error);
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

ZEND_API void zend_reset_locale_deps(TSRMLS_D)
{
	UErrorCode status = U_ZERO_ERROR;

	if (UG(default_collator)) {
		ucol_close(UG(default_collator));
	}
	UG(default_collator) = ucol_open(UG(default_locale), &status);
	if (U_FAILURE(status)) {
		zend_error(E_ERROR, "Could not open collator for locale %s", UG(default_locale));
	}
}

static void init_unicode_request_globals(TSRMLS_D)
{
	UG(default_locale) = safe_estrdup(uloc_getDefault());
	UG(default_collator) = NULL;

	zend_reset_locale_deps(TSRMLS_C);
}

static void shutdown_unicode_request_globals(TSRMLS_D)
{
	ucol_close(UG(default_collator));
	efree(UG(default_locale));
}

void zend_activate(TSRMLS_D)
{
#ifdef ZTS
	CG(function_table) = UG(unicode)?CG(global_u_function_table):CG(global_function_table);
	CG(class_table) = UG(unicode)?CG(global_u_class_table):CG(global_class_table);
	CG(auto_globals) = UG(unicode)?CG(global_u_auto_globals_table):CG(global_auto_globals_table);
	EG(zend_constants) = UG(unicode)?EG(global_u_constants_table):EG(global_constants_table);
#else
	CG(function_table) = UG(unicode)?global_u_function_table:global_function_table;
	CG(class_table) = UG(unicode)?global_u_class_table:global_class_table;
	CG(auto_globals) = UG(unicode)?global_u_auto_globals_table:global_auto_globals_table;
	EG(zend_constants) = UG(unicode)?global_u_constants_table:global_constants_table;
#endif
	init_unicode_request_globals(TSRMLS_C);

	init_unicode_strings();
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

void zend_call_destructors(TSRMLS_D)
{
	zend_try {
		shutdown_destructors(TSRMLS_C);
	} zend_end_try();
}

void zend_deactivate(TSRMLS_D)
{
	/* we're no longer executing anything */
	EG(opline_ptr) = NULL;
	EG(active_symbol_table) = NULL;

	shutdown_unicode_request_globals(TSRMLS_C);

	zend_try {
		shutdown_scanner(TSRMLS_C);
	} zend_end_try();

	/* shutdown_executor() takes care of its own bailout handling */
	shutdown_executor(TSRMLS_C);

	zend_try {
		shutdown_compiler(TSRMLS_C);
	} zend_end_try();

	zend_destroy_rsrc_list(&EG(regular_list) TSRMLS_CC);

	zend_try {
		zend_ini_deactivate(TSRMLS_C);
	} zend_end_try();
}


static int exec_done_cb(zend_module_entry *module TSRMLS_DC)
{
	if (module->post_deactivate_func) {
		module->post_deactivate_func();
	}
	return 0;
}


void zend_post_deactivate_modules(TSRMLS_D)
{
	zend_hash_apply(&module_registry, (apply_func_t) exec_done_cb TSRMLS_CC);
	zend_hash_apply(&module_registry, (apply_func_t) module_registry_unload_temp TSRMLS_CC);
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


ZEND_API void zend_error(int type, const char *format, ...)
{
	va_list args;
	va_list usr_copy;
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
		case E_STRICT:
		case E_WARNING:
		case E_USER_ERROR:
		case E_USER_WARNING:
		case E_USER_NOTICE:
		case E_RECOVERABLE_ERROR:
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
	if (!EG(user_error_handler)
		|| !(EG(user_error_handler_error_reporting) & type)) {
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
			z_error_message->value.str.len = zend_vspprintf(&z_error_message->value.str.val, 0, format, usr_copy);
#ifdef va_copy
			va_end(usr_copy);
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
			zval_copy_ctor(z_context);

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
					if (Z_TYPE_P(retval) == IS_BOOL && Z_LVAL_P(retval) == 0) {
						zend_error_cb(type, error_filename, error_lineno, format, args);
					}
					zval_ptr_dtor(&retval);
				}
			} else {
				/* The user error handler failed, use built-in error handler */
				zend_error_cb(type, error_filename, error_lineno, format, args);
			}

			if (!EG(user_error_handler)) {
				EG(user_error_handler) = orig_user_error_handler;
			} 
			else {
				zval_ptr_dtor(&orig_user_error_handler);
			}

			efree(params);
			zval_ptr_dtor(&z_error_message);
			zval_ptr_dtor(&z_error_type);
			zval_ptr_dtor(&z_error_filename);
			zval_ptr_dtor(&z_error_lineno);
			zval_ptr_dtor(&z_context);
			break;
	}

	va_end(args);

	if (type == E_PARSE) {
		EG(exit_status) = 255;
		zend_init_compiler_data_structures(TSRMLS_C);
	}
}

#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(DARWIN)
void zend_error_noreturn(int type, const char *format, ...) __attribute__ ((alias("zend_error"),noreturn));
#endif

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
		if(file_handle->opened_path) {
			int dummy=1;
			zend_hash_add(&EG(included_files), file_handle->opened_path, strlen(file_handle->opened_path)+1, (void *)&dummy, sizeof(int), NULL);
		}
		zend_destroy_file_handle(file_handle TSRMLS_CC);
		if (EG(active_op_array)) {
			EG(return_value_ptr_ptr) = retval ? retval : &local_retval;
			zend_execute(EG(active_op_array) TSRMLS_CC);
			if (EG(exception)) {
				char ex_class_name[128];
				if (Z_TYPE_P(EG(exception)) == IS_OBJECT) {
					strncpy(ex_class_name, Z_OBJ_CLASS_NAME_P(EG(exception)), 127);
					ex_class_name[127] = '\0';
				} else {
					strcpy(ex_class_name, "Unknown Exception");
				}
				if (EG(user_exception_handler)) {
					zval *orig_user_exception_handler;
					zval ***params, *retval2, *old_exception;
					params = (zval ***)emalloc(sizeof(zval **));
					old_exception = EG(exception);
					EG(exception) = NULL;
					params[0] = &old_exception;
					orig_user_exception_handler = EG(user_exception_handler);
					if (call_user_function_ex(CG(function_table), NULL, orig_user_exception_handler, &retval2, 1, params, 1, NULL TSRMLS_CC) == SUCCESS) {
						if (retval2 != NULL) {
							zval_ptr_dtor(&retval2);
						}
					} else {
						zend_exception_error(EG(exception) TSRMLS_CC);
					}
					efree(params);
					zval_ptr_dtor(&old_exception);
					if (EG(exception)) {
						zval_ptr_dtor(&EG(exception));
						EG(exception) = NULL;
					}
				} else {
					zend_exception_error(EG(exception) TSRMLS_CC);
				}
				if (retval == NULL && *EG(return_value_ptr_ptr) != NULL) {
					zval_ptr_dtor(EG(return_value_ptr_ptr));
					local_retval = NULL;
				}
			} else if (!retval && *EG(return_value_ptr_ptr)) {
				zval_ptr_dtor(EG(return_value_ptr_ptr));
				local_retval = NULL;
			}
			destroy_op_array(EG(active_op_array) TSRMLS_CC);
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
