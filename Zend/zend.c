/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2008 Zend Technologies Ltd. (http://www.zend.com) |
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
#include "zend_unicode.h"

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

#ifndef __GNUC__
ZEND_API zstr null_zstr;
ZEND_API zstr empty_zstr;
#endif

#if defined(ZEND_WIN32) && ZEND_DEBUG
BOOL WINAPI IsDebuggerPresent(VOID);
#endif

/* true multithread-shared globals */
ZEND_API zend_class_entry *zend_standard_class_def = NULL;
ZEND_API int (*zend_printf)(const char *format, ...);
ZEND_API zend_write_func_t zend_write;
ZEND_API int (*zend_path_encode)(char **encpath, int *encpath_len, const UChar *path, int path_len TSRMLS_DC);
ZEND_API int (*zend_path_decode)(UChar **decpath, int *decpath_len, const char *path, int path_len TSRMLS_DC);
ZEND_API FILE *(*zend_fopen)(const char *filename, char **opened_path);
ZEND_API int (*zend_stream_open_function)(const char *filename, zend_file_handle *handle TSRMLS_DC);
ZEND_API void (*zend_block_interruptions)(void);
ZEND_API void (*zend_unblock_interruptions)(void);
ZEND_API void (*zend_ticks_function)(int ticks);
ZEND_API void (*zend_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
int (*zend_vspprintf)(char **pbuf, size_t max_len, const char *format, va_list ap);
ZEND_API char *(*zend_getenv)(char *name, size_t name_len TSRMLS_DC);
ZEND_API char *(*zend_resolve_path)(const char *filename, int filename_len TSRMLS_DC);

void (*zend_on_timeout)(int seconds TSRMLS_DC);

static void (*zend_message_dispatcher_p)(long message, void *data);
static int (*zend_get_configuration_directive_p)(const char *name, uint name_length, zval *contents);

static ZEND_INI_MH(OnUpdateErrorReporting) /* {{{ */
{
	if (!new_value) {
		EG(error_reporting) = E_ALL & ~E_NOTICE & ~E_STRICT & ~E_DEPRECATED;
	} else {
		EG(error_reporting) = atoi(new_value);
	}
	return SUCCESS;
}
/* }}} */

static ZEND_INI_MH(OnUpdateEncoding) /* {{{ */
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
			zend_error(E_CORE_ERROR, "Unrecognized encoding '%s' used for %s", new_value ? new_value : "null", entry->name);
			return FAILURE;
		}
	} else {
		if (*converter) {
			ucnv_close(*converter);
		}
		*converter = NULL;
	}
	if (*converter) {
		zend_set_converter_error_mode(*converter, ZEND_FROM_UNICODE, UG(from_error_mode));
		zend_set_converter_error_mode(*converter, ZEND_TO_UNICODE, UG(to_error_mode));
		zend_set_converter_subst_char(*converter, UG(from_subst_char));
	}

	return SUCCESS;
}
/* }}} */

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

void zend_update_converters_error_behavior(TSRMLS_D) /* {{{ */
{
	if (UG(fallback_encoding_conv)) {
		zend_set_converter_error_mode(UG(fallback_encoding_conv), ZEND_FROM_UNICODE, UG(from_error_mode));
		zend_set_converter_error_mode(UG(fallback_encoding_conv), ZEND_TO_UNICODE, UG(to_error_mode));
		zend_set_converter_subst_char(UG(fallback_encoding_conv), UG(from_subst_char));
	}
	if (UG(runtime_encoding_conv)) {
		zend_set_converter_error_mode(UG(runtime_encoding_conv), ZEND_FROM_UNICODE, UG(from_error_mode));
		zend_set_converter_error_mode(UG(runtime_encoding_conv), ZEND_TO_UNICODE, UG(to_error_mode));
		zend_set_converter_subst_char(UG(runtime_encoding_conv), UG(from_subst_char));
	}
	if (UG(output_encoding_conv)) {
		zend_set_converter_error_mode(UG(output_encoding_conv), ZEND_FROM_UNICODE, UG(from_error_mode));
		zend_set_converter_subst_char(UG(output_encoding_conv), UG(from_subst_char));
	}
}
/* }}} */

static ZEND_INI_MH(OnUpdateGCEnabled) /* {{{ */
{
	OnUpdateBool(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);

	if (GC_G(gc_enabled)) {
		gc_init(TSRMLS_C);
	}

	return SUCCESS;
}
/* }}} */
 
ZEND_INI_BEGIN()
	ZEND_INI_ENTRY("error_reporting",			NULL,		ZEND_INI_ALL,		OnUpdateErrorReporting)

	STD_ZEND_INI_BOOLEAN("zend.enable_gc",				"1",	ZEND_INI_ALL,		OnUpdateGCEnabled,      gc_enabled,     zend_gc_globals,        gc_globals)

	/* Unicode .ini entries */
	STD_ZEND_INI_ENTRY("unicode.fallback_encoding",		NULL,	ZEND_INI_ALL, OnUpdateEncoding, fallback_encoding_conv, zend_unicode_globals, unicode_globals)
	STD_ZEND_INI_ENTRY("unicode.runtime_encoding",		NULL,	ZEND_INI_ALL, OnUpdateEncoding, runtime_encoding_conv, zend_unicode_globals, unicode_globals)
	STD_ZEND_INI_ENTRY("unicode.script_encoding",		NULL,	ZEND_INI_ALL, OnUpdateEncoding, script_encoding_conv, zend_unicode_globals, unicode_globals)
	STD_ZEND_INI_ENTRY("unicode.http_input_encoding",	NULL,	ZEND_INI_ALL, OnUpdateEncoding, http_input_encoding_conv, zend_unicode_globals, unicode_globals)
	STD_ZEND_INI_ENTRY("unicode.filesystem_encoding",	NULL,	ZEND_INI_ALL, OnUpdateEncoding, filesystem_encoding_conv, zend_unicode_globals, unicode_globals)
	/*
	 * This is used as a default for the stream contexts. It's not an actual
	 * UConverter because each stream needs its own.
	 */
	STD_ZEND_INI_ENTRY("unicode.stream_encoding", "UTF-8", ZEND_INI_ALL, OnUpdateStringUnempty, stream_encoding, zend_unicode_globals, unicode_globals)
ZEND_INI_END()


#ifdef ZTS
ZEND_API int compiler_globals_id;
ZEND_API int executor_globals_id;
static HashTable *global_function_table = NULL;
static HashTable *global_class_table = NULL;
static HashTable *global_constants_table = NULL;
static HashTable *global_auto_globals_table = NULL;
static HashTable *global_persistent_list = NULL;
#endif

ZEND_API zend_utility_values zend_uv;

ZEND_API zval zval_used_for_init; /* True global variable */

/* version information */
static char *zend_version_info;
static uint zend_version_info_length;
#define ZEND_CORE_VERSION_INFO	"Zend Engine v" ZEND_VERSION ", Copyright (c) 1998-2008 Zend Technologies\n"
#define PRINT_ZVAL_INDENT 4

static void print_hash(HashTable *ht, int indent, zend_bool is_object TSRMLS_DC) /* {{{ */
{
	zval **tmp;
	zstr string_key;
	HashPosition iterator;
	ulong num_key;
	uint str_len;
	int i;
	zend_uchar ztype;

	for (i = 0; i < indent; i++) {
		ZEND_PUTS(" ");
	}
	ZEND_PUTS("(\n");
	indent += PRINT_ZVAL_INDENT;
	zend_hash_internal_pointer_reset_ex(ht, &iterator);
	while (zend_hash_get_current_data_ex(ht, (void **) &tmp, &iterator) == SUCCESS) {
		zend_uchar key_type;

		for (i = 0; i < indent; i++) {
			ZEND_PUTS(" ");
		}
		ZEND_PUTS("[");
		switch ((key_type = zend_hash_get_current_key_ex(ht, &string_key, &str_len, &num_key, 0, &iterator))) {
			case HASH_KEY_IS_STRING:
				ztype = IS_STRING;
				goto str_type;
			case HASH_KEY_IS_UNICODE:
				ztype = IS_UNICODE;
str_type:
				if (is_object) {
					zstr prop_name, class_name;

					int mangled = zend_u_unmangle_property_name(ztype, string_key, str_len - 1, &class_name, &prop_name);

					if (class_name.v && mangled == SUCCESS) {
						if (class_name.s[0]=='*') {
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
	for (i = 0; i < indent; i++) {
		ZEND_PUTS(" ");
	}
	ZEND_PUTS(")\n");
}
/* }}} */

static void print_flat_hash(HashTable *ht TSRMLS_DC) /* {{{ */
{
	zval **tmp;
	zstr string_key;
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
				ZEND_PUTS(string_key.s);
				break;
			case HASH_KEY_IS_UNICODE:
				zend_printf("%r", string_key.u);
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
/* }}} */

ZEND_API void zend_make_string_zval(zval *expr, zval *expr_copy, int *use_copy) /* {{{ */
{
	if (Z_TYPE_P(expr)==IS_STRING) {
		*use_copy = 0;
		return;
	}
	switch (Z_TYPE_P(expr)) {
		case IS_OBJECT:
			{
				TSRMLS_FETCH();

				if (Z_OBJ_HT_P(expr)->cast_object && Z_OBJ_HANDLER_P(expr, cast_object)(expr, expr_copy, IS_STRING, NULL TSRMLS_CC) == SUCCESS) {
					break;
				}
				if (Z_OBJ_HANDLER_P(expr, get)) {
					zval *z = Z_OBJ_HANDLER_P(expr, get)(expr TSRMLS_CC);

					Z_ADDREF_P(z);
					if (Z_TYPE_P(z) != IS_OBJECT) {
						zend_make_string_zval(z, expr_copy, use_copy);
						if (*use_copy) {
							zval_ptr_dtor(&z);
						} else {
							ZVAL_ZVAL(expr_copy, z, 0, 1);
							*use_copy = 1;
						}
						return;
					}
					zval_ptr_dtor(&z);
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
	Z_TYPE_P(expr_copy) = IS_STRING;
	*use_copy = 1;
}
/* }}} */

ZEND_API void zend_make_printable_zval(zval *expr, zval *expr_copy, int *use_copy) /* {{{ */
{
	UErrorCode temp = U_ZERO_ERROR;
	TSRMLS_FETCH();

	if (
		/* UTODO: clean this up */
		(Z_TYPE_P(expr) == IS_STRING &&
		(!strcmp(ucnv_getName(ZEND_U_CONVERTER(UG(output_encoding_conv)), &temp), ucnv_getName(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &temp))))
	) {
		*use_copy = 0;
		return;
	}
	switch (Z_TYPE_P(expr)) {
		case IS_NULL:
			Z_STRLEN_P(expr_copy) = 0;
			Z_STRVAL_P(expr_copy) = STR_EMPTY_ALLOC();
			break;
		case IS_BOOL:
			if (Z_LVAL_P(expr)) {
				Z_STRLEN_P(expr_copy) = 1;
				Z_STRVAL_P(expr_copy) = estrndup("1", 1);
			} else {
				Z_STRLEN_P(expr_copy) = 0;
				Z_STRVAL_P(expr_copy) = STR_EMPTY_ALLOC();
			}
			break;
		case IS_RESOURCE:
			Z_STRVAL_P(expr_copy) = (char *) emalloc(sizeof("Resource id #") - 1 + MAX_LENGTH_OF_LONG);
			Z_STRLEN_P(expr_copy) = sprintf(Z_STRVAL_P(expr_copy), "Resource id #%ld", Z_LVAL_P(expr));
			break;
		case IS_ARRAY:
			Z_STRLEN_P(expr_copy) = sizeof("Array") - 1;
			Z_STRVAL_P(expr_copy) = estrndup("Array", Z_STRLEN_P(expr_copy));
			break;
		case IS_OBJECT:
			if (Z_OBJ_HT_P(expr)->cast_object && Z_OBJ_HANDLER_P(expr, cast_object)(expr, expr_copy, IS_STRING, ZEND_U_CONVERTER(UG(output_encoding_conv)) TSRMLS_CC) == SUCCESS) {
				break;
			}
			if (Z_OBJ_HANDLER_P(expr, get)) {
				zval *z = Z_OBJ_HANDLER_P(expr, get)(expr TSRMLS_CC);

				Z_ADDREF_P(z);
				if (Z_TYPE_P(z) != IS_OBJECT) {
					zend_make_printable_zval(z, expr_copy, use_copy);
					if (*use_copy) {
						zval_ptr_dtor(&z);
					} else {
						ZVAL_ZVAL(expr_copy, z, 0, 1);
						*use_copy = 1;
					}
					return;
				}
				zval_ptr_dtor(&z);
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
	Z_TYPE_P(expr_copy) = IS_STRING;
	*use_copy = 1;
}
/* }}} */

ZEND_API void zend_make_unicode_zval(zval *expr, zval *expr_copy, int *use_copy) /* {{{ */
{
	TSRMLS_FETCH();

	if (Z_TYPE_P(expr)==IS_UNICODE) {
		*use_copy = 0;
		return;
	}
	switch (Z_TYPE_P(expr)) {
		case IS_OBJECT:
			if (Z_OBJ_HT_P(expr)->cast_object && Z_OBJ_HANDLER_P(expr, cast_object)(expr, expr_copy, IS_UNICODE, NULL TSRMLS_CC) == SUCCESS) {
				break;
			}
			if (Z_OBJ_HANDLER_P(expr, get)) {
				zval *z = Z_OBJ_HANDLER_P(expr, get)(expr TSRMLS_CC);

				Z_ADDREF_P(z);
				if (Z_TYPE_P(z) != IS_OBJECT) {
					zend_make_unicode_zval(z, expr_copy, use_copy);
					if (*use_copy) {
						zval_ptr_dtor(&z);
					} else {
						ZVAL_ZVAL(expr_copy, z, 0, 1);
						*use_copy = 1;
					}
					return;
				}
				zval_ptr_dtor(&z);
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
	Z_TYPE_P(expr_copy) = IS_UNICODE;
	*use_copy = 1;
}
/* }}} */

ZEND_API int zend_print_zval(zval *expr, int indent) /* {{{ */
{
	return zend_print_zval_ex(zend_write, expr, indent);
}
/* }}} */

ZEND_API int zend_print_zval_ex(zend_write_func_t write_func, zval *expr, int indent) /* {{{ */
{
	zval expr_copy;
	int use_copy;

	zend_make_printable_zval(expr, &expr_copy, &use_copy);
	if (use_copy) {
		expr = &expr_copy;
	}
	if (Z_STRLEN_P(expr) == 0) { /* optimize away empty strings */
		if (use_copy) {
			zval_dtor(expr);
		}
		return 0;
	}
	write_func(Z_STRVAL_P(expr), Z_STRLEN_P(expr));
	if (use_copy) {
		zval_dtor(expr);
	}
	return Z_STRLEN_P(expr);
}
/* }}} */

ZEND_API void zend_print_flat_zval_r(zval *expr TSRMLS_DC) /* {{{ */
{
	switch (Z_TYPE_P(expr)) {
		case IS_ARRAY:
			ZEND_PUTS("Array (");
			if (++Z_ARRVAL_P(expr)->nApplyCount>1) {
				ZEND_PUTS(" *RECURSION*");
				Z_ARRVAL_P(expr)->nApplyCount--;
				return;
			}
			print_flat_hash(Z_ARRVAL_P(expr) TSRMLS_CC);
			ZEND_PUTS(")");
			Z_ARRVAL_P(expr)->nApplyCount--;
			break;
		case IS_OBJECT:
		{
			HashTable *properties = NULL;
			zstr class_name = NULL_ZSTR;
			zend_uint clen;

			if (Z_OBJ_HANDLER_P(expr, get_class_name)) {
				Z_OBJ_HANDLER_P(expr, get_class_name)(expr, &class_name, &clen, 0 TSRMLS_CC);
			}
			if (class_name.v) {
				zend_printf("%v Object (", class_name.v);
			} else {
				zend_printf("%s Object (", "Unknown Class");
			}
			if (class_name.v) {
				efree(class_name.v);
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
/* }}} */

ZEND_API void zend_print_zval_r(zval *expr, int indent TSRMLS_DC) /* {{{ */
{
	zend_print_zval_r_ex(zend_write, expr, indent TSRMLS_CC);
}
/* }}} */

ZEND_API void zend_print_zval_r_ex(zend_write_func_t write_func, zval *expr, int indent TSRMLS_DC) /* {{{ */
{
	switch (Z_TYPE_P(expr)) {
		case IS_ARRAY:
			ZEND_PUTS("Array\n");
			if (++Z_ARRVAL_P(expr)->nApplyCount>1) {
				ZEND_PUTS(" *RECURSION*");
				Z_ARRVAL_P(expr)->nApplyCount--;
				return;
			}
			print_hash(Z_ARRVAL_P(expr), indent, 0 TSRMLS_CC);
			Z_ARRVAL_P(expr)->nApplyCount--;
			break;
		case IS_OBJECT:
			{
				HashTable *properties;
				zstr class_name = NULL_ZSTR;
				zend_uint clen;
				int is_temp;

				if (Z_OBJ_HANDLER_P(expr, get_class_name)) {
					Z_OBJ_HANDLER_P(expr, get_class_name)(expr, &class_name, &clen, 0 TSRMLS_CC);
				}
				if (class_name.v) {
					zend_printf("%v Object\n", class_name.v);
				} else {
					zend_printf("%s Object\n", "Unknown Class");
				}
				if (class_name.v) {
					efree(class_name.v);
				}
				if ((properties = Z_OBJDEBUG_P(expr, is_temp)) == NULL) {
					break;
				}
				if (++properties->nApplyCount>1) {
					ZEND_PUTS(" *RECURSION*");
					properties->nApplyCount--;
					return;
				}
				print_hash(properties, indent, 1 TSRMLS_CC);
				properties->nApplyCount--;
				if (is_temp) {
					zend_hash_destroy(properties);
					efree(properties);
				}
				break;
			}
		default:
			zend_print_variable(expr);
			break;
	}
}
/* }}} */

static int zend_path_encode_wrapper(char **encpath, int *encpath_len, const UChar *path, int path_len TSRMLS_DC) /* {{{ */
{
	UErrorCode status = U_ZERO_ERROR;

	zend_unicode_to_string_ex(ZEND_U_CONVERTER(UG(filesystem_encoding_conv)), encpath, encpath_len, path, path_len, &status);

	if (U_FAILURE(status)) {
		efree(*encpath);
		*encpath = NULL;
		*encpath_len = 0;
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

static int zend_path_decode_wrapper(UChar **decpath, int *decpath_len, const char *path, int path_len TSRMLS_DC) /* {{{ */
{
	UErrorCode status = U_ZERO_ERROR;

	zend_string_to_unicode_ex(ZEND_U_CONVERTER(UG(filesystem_encoding_conv)), decpath, decpath_len, path, path_len, &status);

	if (U_FAILURE(status)) {
		efree(*decpath);
		*decpath = NULL;
		*decpath_len = 0;
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

static FILE *zend_fopen_wrapper(const char *filename, char **opened_path) /* {{{ */
{
	if (opened_path) {
		*opened_path = estrdup(filename);
	}
	return fopen(filename, "rb");
}
/* }}} */

static void register_standard_class(TSRMLS_D) /* {{{ */
{
	zend_standard_class_def = calloc(1, sizeof(zend_class_entry));

	zend_standard_class_def->type = ZEND_INTERNAL_CLASS;
	zend_standard_class_def->name_length = sizeof("stdClass") - 1;
	zend_standard_class_def->name.s = zend_strndup("stdClass", zend_standard_class_def->name_length);
	zend_initialize_class_data(zend_standard_class_def, 1 TSRMLS_CC);

	zend_hash_add(CG(class_table), "stdclass", sizeof("stdclass"), &zend_standard_class_def, sizeof(zend_class_entry *), NULL);
}
/* }}} */

#ifdef ZTS
static zend_bool asp_tags_default			= 0;
static zend_bool short_tags_default			= 1;
static zend_uint compiler_options_default	= ZEND_COMPILE_DEFAULT;
#else
# define asp_tags_default			0
# define short_tags_default			1
# define compiler_options_default	ZEND_COMPILE_DEFAULT
#endif

static void zend_set_default_compile_time_values(TSRMLS_D) /* {{{ */
{
	/* default compile-time values */
	CG(asp_tags) = asp_tags_default;
	CG(short_tags) = short_tags_default;
	CG(compiler_options) = compiler_options_default;
	CG(literal_type) = ZEND_STR_TYPE;
}
/* }}} */

#define ZEND_U_CONSTANT_DTOR (void (*)(void *)) free_u_zend_constant

static void zval_copy_persistent(zval *zv) /* {{{ */
{
	if (Z_TYPE_P(zv) == IS_STRING || (Z_TYPE_P(zv) & IS_CONSTANT_TYPE_MASK) == IS_CONSTANT) {
		UChar *ustr;

		ustr = malloc(UBYTES(Z_STRLEN_P(zv) + 1));
		u_charsToUChars(Z_STRVAL_P(zv), ustr, Z_STRLEN_P(zv) + 1);
		free(Z_STRVAL_P(zv));
		Z_USTRVAL_P(zv) = ustr;
		if (Z_TYPE_P(zv) == IS_STRING) Z_TYPE_P(zv) = IS_UNICODE;
	}
}
/* }}} */

static void free_u_zend_constant(zend_constant *c) /* {{{ */
{
	if (!(c->flags & CONST_PERSISTENT)) {
		zval_dtor(&c->value);
	} else {
		zval_internal_dtor(&c->value);
	}
	free(c->name.v);
}
/* }}} */

static void function_to_unicode(zend_function *func TSRMLS_DC) /* {{{ */
{
	if (func->common.function_name.s) {
		if (UG(unicode)) {
			func->common.function_name.u = zend_ustrdup(func->common.function_name.u);
		} else {
			UChar *uname;
			int len = strlen(func->common.function_name.s) + 1;
	
			uname = malloc(UBYTES(len));
			u_charsToUChars(func->common.function_name.s, uname, len);
			func->common.function_name.u = uname;
		}
	}
	if (func->common.arg_info) {
		zend_arg_info *args;
		int n = func->common.num_args;

		args = malloc((n + 1) * sizeof(zend_arg_info));
		memcpy(args, func->common.arg_info, (n + 1) * sizeof(zend_arg_info));
		while (n > 0) {
			--n;
			if (args[n].name.s) {
				UChar *uname = malloc(UBYTES(args[n].name_len + 1));
				u_charsToUChars(args[n].name.s, uname, args[n].name_len + 1);
				args[n].name.u = uname;
			}
			if (args[n].class_name.s) {
				UChar *uname = malloc(UBYTES(args[n].class_name_len + 1));
				u_charsToUChars(args[n].class_name.s, uname, args[n].class_name_len + 1);
				args[n].class_name.u = uname;
			}
		}
		func->common.arg_info = args;
	}
}
/* }}} */

static void property_info_to_unicode(zend_property_info *info TSRMLS_DC) /* {{{ */
{
	if (info->name.s) {
		UChar *uname;

		uname = malloc(UBYTES(info->name_length + 1));
		u_charsToUChars(info->name.s, uname, info->name_length + 1);
		free(info->name.s);
		info->name.u = uname;
		info->h = zend_u_get_hash_value(IS_UNICODE, info->name, info->name_length + 1);
	}
}
/* }}} */

static void zval_ptr_to_unicode(zval **zv TSRMLS_DC) /* {{{ */
{
	zval_copy_persistent(*zv);
}
/* }}} */

static void const_to_unicode(zend_constant *c) /* {{{ */
{
	UChar *uname;

	if (c->name.s) {
		uname = malloc(UBYTES(c->name_len));
		u_charsToUChars(c->name.s, uname, c->name_len);
		free(c->name.s);
		c->name.u = uname;
	}
	if (Z_TYPE(c->value) == IS_STRING || (Z_TYPE(c->value) & IS_CONSTANT_TYPE_MASK) == IS_CONSTANT) {
		UChar *ustr;

		ustr = malloc(UBYTES(Z_STRLEN(c->value) + 1));
		u_charsToUChars(Z_STRVAL(c->value), ustr, Z_STRLEN(c->value) + 1);
		Z_USTRVAL(c->value) = ustr;
		if (Z_TYPE(c->value) == IS_STRING) Z_TYPE(c->value) = IS_UNICODE;
	}
}
/* }}} */

static void class_to_unicode(zend_class_entry **ce TSRMLS_DC) /* {{{ */
{
	/* Convert name to unicode */
	if ((*ce)->name.s) {
		UChar *uname = malloc(UBYTES((*ce)->name_length + 1));

		u_charsToUChars((*ce)->name.s, uname, (*ce)->name_length + 1);
		free((*ce)->name.s);
		(*ce)->name.u = uname;
	}

	zend_hash_to_unicode(&(*ce)->function_table, (apply_func_t)function_to_unicode TSRMLS_CC);
	(*ce)->function_table.pDestructor = ZEND_U_FUNCTION_DTOR;
	zend_hash_to_unicode(&(*ce)->constants_table, (apply_func_t)zval_ptr_to_unicode TSRMLS_CC);
	zend_hash_to_unicode(&(*ce)->properties_info, (apply_func_t)property_info_to_unicode TSRMLS_CC);
	zend_hash_to_unicode(&(*ce)->default_properties, (apply_func_t)zval_ptr_to_unicode TSRMLS_CC);
	zend_hash_to_unicode(&(*ce)->default_static_members, (apply_func_t)zval_ptr_to_unicode TSRMLS_CC);
}
/* }}} */

static void zend_init_exception_op(TSRMLS_D) /* {{{ */
{
	memset(EG(exception_op), 0, sizeof(EG(exception_op)));
	EG(exception_op)[0].opcode = ZEND_HANDLE_EXCEPTION;
	EG(exception_op)[0].op1.op_type = IS_UNUSED;
	EG(exception_op)[0].op2.op_type = IS_UNUSED;
	EG(exception_op)[0].result.op_type = IS_UNUSED;
	ZEND_VM_SET_OPCODE_HANDLER(EG(exception_op));
	EG(exception_op)[1].opcode = ZEND_HANDLE_EXCEPTION;
	EG(exception_op)[1].op1.op_type = IS_UNUSED;
	EG(exception_op)[1].op2.op_type = IS_UNUSED;
	EG(exception_op)[1].result.op_type = IS_UNUSED;
	ZEND_VM_SET_OPCODE_HANDLER(EG(exception_op)+1);
	EG(exception_op)[2].opcode = ZEND_HANDLE_EXCEPTION;
	EG(exception_op)[2].op1.op_type = IS_UNUSED;
	EG(exception_op)[2].op2.op_type = IS_UNUSED;
	EG(exception_op)[2].result.op_type = IS_UNUSED;
	ZEND_VM_SET_OPCODE_HANDLER(EG(exception_op)+2);
}
/* }}} */

#ifdef ZTS
static void compiler_globals_ctor(zend_compiler_globals *compiler_globals TSRMLS_DC) /* {{{ */
{
	zend_function tmp_func;
	zend_class_entry *tmp_class;

	compiler_globals->compiled_filename = NULL;

	compiler_globals->function_table = (HashTable *) malloc(sizeof(HashTable));
	zend_u_hash_init_ex(compiler_globals->function_table, global_function_table->nNumOfElements, NULL, ZEND_FUNCTION_DTOR, 1, UG(unicode), 0);
	zend_hash_copy(compiler_globals->function_table, global_function_table, NULL, &tmp_func, sizeof(zend_function));

	compiler_globals->class_table = (HashTable *) malloc(sizeof(HashTable));
	zend_u_hash_init_ex(compiler_globals->class_table, global_class_table->nNumOfElements, NULL, ZEND_CLASS_DTOR, 1, UG(unicode), 0);
	zend_hash_copy(compiler_globals->class_table, global_class_table, (copy_ctor_func_t) zend_class_add_ref, &tmp_class, sizeof(zend_class_entry *));

	zend_set_default_compile_time_values(TSRMLS_C);

	CG(interactive) = 0;
	CG(literal_type) = ZEND_STR_TYPE;

	compiler_globals->auto_globals = (HashTable *) malloc(sizeof(HashTable));
	zend_u_hash_init_ex(compiler_globals->auto_globals, global_auto_globals_table->nNumOfElements, NULL, NULL, 1, UG(unicode), 0);
	zend_hash_copy(compiler_globals->auto_globals, global_auto_globals_table, NULL, NULL, sizeof(zend_auto_global) /* empty element */);

	compiler_globals->last_static_member = zend_hash_num_elements(compiler_globals->class_table);
	if (compiler_globals->last_static_member) {
		compiler_globals->static_members = (HashTable**)calloc(compiler_globals->last_static_member, sizeof(HashTable*));
	} else {
		compiler_globals->static_members = NULL;
	}
}
/* }}} */

static void compiler_globals_dtor(zend_compiler_globals *compiler_globals TSRMLS_DC) /* {{{ */
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
	if (compiler_globals->static_members) {
		free(compiler_globals->static_members);
	}
	compiler_globals->last_static_member = 0;
}
/* }}} */

static void executor_globals_ctor(zend_executor_globals *executor_globals TSRMLS_DC) /* {{{ */
{
	zend_startup_constants(TSRMLS_C);
	zend_copy_constants(EG(zend_constants), GLOBAL_CONSTANTS_TABLE);
	zend_init_rsrc_plist(TSRMLS_C);
	zend_init_exception_op(TSRMLS_C);
	EG(lambda_count) = 0;
	EG(user_error_handler) = NULL;
	EG(user_exception_handler) = NULL;
	EG(in_execution) = 0;
	EG(in_autoload) = NULL;
	EG(current_execute_data) = NULL;
	EG(current_module) = NULL;
	EG(exit_status) = 0;
	EG(active) = 0;
}
/* }}} */

static void executor_globals_dtor(zend_executor_globals *executor_globals TSRMLS_DC) /* {{{ */
{
	zend_ini_shutdown(TSRMLS_C);
	if (&executor_globals->persistent_list != global_persistent_list) {
		zend_destroy_rsrc_list(&executor_globals->persistent_list TSRMLS_CC);
	}
	if (executor_globals->zend_constants != GLOBAL_CONSTANTS_TABLE) {
		zend_hash_destroy(executor_globals->zend_constants);
		free(executor_globals->zend_constants);
	}
}
/* }}} */

static void zend_new_thread_end_handler(THREAD_T thread_id TSRMLS_DC) /* {{{ */
{
	if (zend_copy_ini_directives(TSRMLS_C) == SUCCESS) {
		zend_ini_refresh_caches(ZEND_INI_STAGE_STARTUP TSRMLS_CC);
	}
}
/* }}} */
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
/* FreeBSD and DragonFly floating point precision fix */
#include <floatingpoint.h>
#endif

static void ini_scanner_globals_ctor(zend_ini_scanner_globals *scanner_globals_p TSRMLS_DC) /* {{{ */
{
	memset(scanner_globals_p, 0, sizeof(*scanner_globals_p));
}
/* }}} */

static void php_scanner_globals_ctor(zend_php_scanner_globals *scanner_globals_p TSRMLS_DC) /* {{{ */
{
	memset(scanner_globals_p, 0, sizeof(*scanner_globals_p));
}
/* }}} */

static void unicode_globals_ctor(zend_unicode_globals *unicode_globals TSRMLS_DC) /* {{{ */
{
	unicode_globals->unicode = 1;
	unicode_globals->utf8_conv = NULL;
	unicode_globals->ascii_conv = NULL;
	unicode_globals->fallback_encoding_conv = NULL;
	unicode_globals->runtime_encoding_conv = NULL;
	unicode_globals->output_encoding_conv = NULL;
	unicode_globals->script_encoding_conv = NULL;
	unicode_globals->http_input_encoding_conv = NULL;
	unicode_globals->filesystem_encoding_conv = NULL;
	zend_set_converter_encoding(&unicode_globals->utf8_conv, "UTF-8");
	zend_set_converter_error_mode(unicode_globals->utf8_conv, ZEND_TO_UNICODE, ZEND_CONV_ERROR_STOP);
	zend_set_converter_encoding(&unicode_globals->ascii_conv, "US-ASCII");
	zend_set_converter_error_mode(unicode_globals->ascii_conv, ZEND_FROM_UNICODE, ZEND_CONV_ERROR_STOP);
	unicode_globals->from_error_mode = ZEND_CONV_ERROR_SUBST;
	memset(unicode_globals->from_subst_char, 0, 3 * sizeof(UChar));
	zend_codepoint_to_uchar(0x3f, unicode_globals->from_subst_char);
	unicode_globals->to_error_mode = ZEND_CONV_ERROR_STOP;
	unicode_globals->conv_error_handler = NULL;

	{
		UErrorCode status = U_ZERO_ERROR;

		unicode_globals->root_collator = ucol_open("en_US", &status);
		ucol_setStrength(unicode_globals->root_collator, UCOL_PRIMARY);
		unicode_globals->root_search = usearch_openFromCollator(EMPTY_STR, 1, EMPTY_STR, 1,
																unicode_globals->root_collator, NULL, &status);
	}

	zend_hash_init_ex(&unicode_globals->flex_compatible, 0, NULL, NULL, 1, 0);
}
/* }}} */

static void unicode_globals_dtor(zend_unicode_globals *unicode_globals TSRMLS_DC) /* {{{ */
{
	if (unicode_globals->root_collator) {
		ucol_close(unicode_globals->root_collator);
	}
	if (unicode_globals->root_search) {
		usearch_close(unicode_globals->root_search);
	}
	if (unicode_globals->fallback_encoding_conv &&
		unicode_globals->fallback_encoding_conv != unicode_globals->utf8_conv &&
		unicode_globals->fallback_encoding_conv != unicode_globals->ascii_conv) {
		ucnv_close(unicode_globals->fallback_encoding_conv);
	}
	if (unicode_globals->runtime_encoding_conv &&
		unicode_globals->runtime_encoding_conv != unicode_globals->utf8_conv &&
		unicode_globals->runtime_encoding_conv != unicode_globals->ascii_conv) {
		ucnv_close(unicode_globals->runtime_encoding_conv);
	}
	if (unicode_globals->output_encoding_conv &&
		unicode_globals->output_encoding_conv != unicode_globals->utf8_conv &&
		unicode_globals->output_encoding_conv != unicode_globals->ascii_conv) {
		ucnv_close(unicode_globals->output_encoding_conv);
	}
	if (unicode_globals->script_encoding_conv &&
		unicode_globals->script_encoding_conv != unicode_globals->utf8_conv &&
		unicode_globals->script_encoding_conv != unicode_globals->ascii_conv) {
		ucnv_close(unicode_globals->script_encoding_conv);
	}
	if (unicode_globals->http_input_encoding_conv &&
		unicode_globals->http_input_encoding_conv != unicode_globals->utf8_conv &&
		unicode_globals->http_input_encoding_conv != unicode_globals->ascii_conv) {
		ucnv_close(unicode_globals->http_input_encoding_conv);
	}
	if (unicode_globals->utf8_conv) {
		ucnv_close(unicode_globals->utf8_conv);
	}
	if (unicode_globals->ascii_conv) {
		ucnv_close(unicode_globals->ascii_conv);
	}
	zend_hash_destroy(&unicode_globals->flex_compatible);
}
/* }}} */

void zend_init_opcodes_handlers(void);

int zend_startup(zend_utility_functions *utility_functions, char **extensions) /* {{{ */
{
#ifdef ZTS
	zend_compiler_globals *compiler_globals;
	zend_executor_globals *executor_globals;
	extern ZEND_API ts_rsrc_id ini_scanner_globals_id;
	extern ZEND_API ts_rsrc_id language_scanner_globals_id;
	extern ZEND_API ts_rsrc_id unicode_globals_id;
#else
	extern zend_ini_scanner_globals ini_scanner_globals;
	extern zend_php_scanner_globals language_scanner_globals;
	extern zend_unicode_globals unicode_globals;
#endif
	TSRMLS_FETCH();

#ifndef __GNUC__
	null_zstr.v = NULL;
	empty_zstr.u = EMPTY_STR;
#endif

	start_memory_manager(TSRMLS_C);

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
	zend_path_encode = utility_functions->path_encode_function;
	if (!zend_path_encode) {
		zend_path_encode = zend_path_encode_wrapper;
	}
	zend_path_decode = utility_functions->path_decode_function;
	if (!zend_path_decode) {
		zend_path_decode = zend_path_decode_wrapper;
	}
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
	zend_resolve_path = utility_functions->resolve_path_function;

	zend_compile_file = compile_file;
	zend_compile_string = compile_string;
	zend_execute = execute;
	zend_execute_internal = NULL;
	zend_throw_exception_hook = NULL;

	zend_init_opcodes_handlers();

	/* set up version */
	zend_version_info = strdup(ZEND_CORE_VERSION_INFO);
	zend_version_info_length = sizeof(ZEND_CORE_VERSION_INFO) - 1;

	GLOBAL_FUNCTION_TABLE = (HashTable *) malloc(sizeof(HashTable));
	GLOBAL_CLASS_TABLE = (HashTable *) malloc(sizeof(HashTable));
	GLOBAL_AUTO_GLOBALS_TABLE = (HashTable *) malloc(sizeof(HashTable));
	GLOBAL_CONSTANTS_TABLE = (HashTable *) malloc(sizeof(HashTable));

	zend_hash_init_ex(GLOBAL_FUNCTION_TABLE, 100, NULL, ZEND_FUNCTION_DTOR, 1, 0);
	zend_hash_init_ex(GLOBAL_CLASS_TABLE, 10, NULL, ZEND_CLASS_DTOR, 1, 0);
	zend_hash_init_ex(GLOBAL_AUTO_GLOBALS_TABLE, 8, NULL, (dtor_func_t) zend_auto_global_dtor, 1, 0);
	zend_hash_init_ex(GLOBAL_CONSTANTS_TABLE, 20, NULL, ZEND_CONSTANT_DTOR, 1, 0);

	zend_hash_init_ex(&module_registry, 50, NULL, ZEND_MODULE_DTOR, 1, 0);
	zend_init_rsrc_list_dtors();

	/* This zval can be used to initialize allocate zval's to an uninit'ed value */
	Z_UNSET_ISREF(zval_used_for_init);
	Z_SET_REFCOUNT(zval_used_for_init, 1);
	Z_TYPE(zval_used_for_init) = IS_NULL;

#ifdef ZTS
	ts_allocate_id(&unicode_globals_id, sizeof(zend_unicode_globals), (ts_allocate_ctor) unicode_globals_ctor, (ts_allocate_dtor) unicode_globals_dtor);
	ts_allocate_id(&compiler_globals_id, sizeof(zend_compiler_globals), (ts_allocate_ctor) compiler_globals_ctor, (ts_allocate_dtor) compiler_globals_dtor);
	ts_allocate_id(&executor_globals_id, sizeof(zend_executor_globals), (ts_allocate_ctor) executor_globals_ctor, (ts_allocate_dtor) executor_globals_dtor);
	ts_allocate_id(&language_scanner_globals_id, sizeof(zend_php_scanner_globals), (ts_allocate_ctor) php_scanner_globals_ctor, NULL);
	ts_allocate_id(&ini_scanner_globals_id, sizeof(zend_ini_scanner_globals), (ts_allocate_ctor) ini_scanner_globals_ctor, NULL);
	compiler_globals = ts_resource(compiler_globals_id);
	executor_globals = ts_resource(executor_globals_id);
	tsrm_ls = ts_resource_ex(0, NULL);

	compiler_globals_dtor(compiler_globals TSRMLS_CC);
	compiler_globals->in_compilation = 0;
	compiler_globals->function_table = (HashTable *) malloc(sizeof(HashTable));
	compiler_globals->class_table = (HashTable *) malloc(sizeof(HashTable));

	*compiler_globals->function_table = *GLOBAL_FUNCTION_TABLE;
	*compiler_globals->class_table = *GLOBAL_CLASS_TABLE;
	compiler_globals->auto_globals = GLOBAL_AUTO_GLOBALS_TABLE;

	zend_hash_destroy(executor_globals->zend_constants);
	*executor_globals->zend_constants = *GLOBAL_CONSTANTS_TABLE;
#else
	unicode_globals_ctor(&unicode_globals TSRMLS_CC);
	ini_scanner_globals_ctor(&ini_scanner_globals TSRMLS_CC);
	php_scanner_globals_ctor(&language_scanner_globals TSRMLS_CC);
	zend_set_default_compile_time_values(TSRMLS_C);
	EG(user_error_handler) = NULL;
	EG(user_exception_handler) = NULL;
#endif
	register_standard_class(TSRMLS_C);
	zend_register_standard_constants(TSRMLS_C);
	zend_register_auto_global("GLOBALS", sizeof("GLOBALS") - 1, NULL TSRMLS_CC);

#ifndef ZTS
	zend_init_rsrc_plist(TSRMLS_C);
	zend_init_exception_op(TSRMLS_C);
#endif

	zend_ini_startup(TSRMLS_C);

#ifdef ZTS
	tsrm_set_new_thread_end_handler(zend_new_thread_end_handler);
#endif

	return SUCCESS;
}
/* }}} */

void zend_register_standard_ini_entries(TSRMLS_D) /* {{{ */
{
	int module_number = 0;

	REGISTER_INI_ENTRIES();

	/* Make copies of HashTables with UNICODE */

	if (UG(unicode)) {
		UConverter *old_runtime_encoding_conv;
		UErrorCode status = U_ZERO_ERROR;

		old_runtime_encoding_conv = UG(runtime_encoding_conv);
		UG(runtime_encoding_conv) = ucnv_open("ASCII", &status);

		zend_hash_to_unicode(CG(function_table), (apply_func_t)function_to_unicode TSRMLS_CC);
		CG(function_table)->pDestructor = ZEND_U_FUNCTION_DTOR;
		zend_hash_to_unicode(CG(class_table), (apply_func_t)class_to_unicode TSRMLS_CC);
		zend_hash_to_unicode(CG(auto_globals), NULL TSRMLS_CC);
		zend_hash_to_unicode(EG(zend_constants), (apply_func_t)const_to_unicode TSRMLS_CC);
		EG(zend_constants)->pDestructor = ZEND_U_CONSTANT_DTOR;

		ucnv_close(UG(runtime_encoding_conv));
		UG(runtime_encoding_conv) = old_runtime_encoding_conv;
	}
	zend_startup_builtin_functions(TSRMLS_C);
}
/* }}} */

/* Unlink the global (r/o) copies of the class, function and constant tables,
 * and use a fresh r/w copy for the startup thread
 */
void zend_post_startup(TSRMLS_D) /* {{{ */
{
#ifdef ZTS
	zend_compiler_globals *compiler_globals = ts_resource(compiler_globals_id);
	zend_executor_globals *executor_globals = ts_resource(executor_globals_id);

	*GLOBAL_FUNCTION_TABLE = *compiler_globals->function_table;
	*GLOBAL_CLASS_TABLE = *compiler_globals->class_table;
	*GLOBAL_CONSTANTS_TABLE = *executor_globals->zend_constants;

	asp_tags_default = CG(asp_tags);
	short_tags_default = CG(short_tags);
	compiler_options_default = CG(compiler_options);

	zend_destroy_rsrc_list(&EG(persistent_list) TSRMLS_CC);
	free(compiler_globals->function_table);
	free(compiler_globals->class_table);
	compiler_globals_ctor(compiler_globals, tsrm_ls);
	free(EG(zend_constants));
	executor_globals_ctor(executor_globals, tsrm_ls);
	global_persistent_list = &EG(persistent_list);
	zend_copy_ini_directives(TSRMLS_C);
#endif
}
/* }}} */

void zend_shutdown(TSRMLS_D) /* {{{ */
{
#ifdef ZEND_WIN32
	zend_shutdown_timeout_thread();
#endif
	zend_destroy_rsrc_list(&EG(persistent_list) TSRMLS_CC);
	zend_hash_graceful_reverse_destroy(&module_registry);
	zend_shutdown_builtin_functions(TSRMLS_C);

	zend_hash_destroy(GLOBAL_FUNCTION_TABLE);
	zend_hash_destroy(GLOBAL_CLASS_TABLE);

	zend_hash_destroy(GLOBAL_AUTO_GLOBALS_TABLE);
	free(GLOBAL_AUTO_GLOBALS_TABLE);

	zend_shutdown_extensions(TSRMLS_C);
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
#else
	unicode_globals_dtor(&unicode_globals TSRMLS_CC);
#endif
	zend_destroy_rsrc_list_dtors();

	/* free ICU cache and any open ICU item (collators, converters, ...) */
	/* u_cleanup(); */
}
/* }}} */

void zend_set_utility_values(zend_utility_values *utility_values) /* {{{ */
{
	zend_uv = *utility_values;
	zend_uv.import_use_extension_length = strlen(zend_uv.import_use_extension);
}
/* }}} */

/* this should be compatible with the standard zenderror */
void zenderror(const char *error) /* {{{ */
{
	zend_error(E_PARSE, "%s", error);
}
/* }}} */

BEGIN_EXTERN_C()
ZEND_API void _zend_bailout(char *filename, uint lineno) /* {{{ */
{
	TSRMLS_FETCH();

	if (!EG(bailout)) {
		zend_output_debug_string(1, "%s(%d) : Bailed out without a bailout address!", filename, lineno);
		exit(-1);
	}
	CG(unclean_shutdown) = 1;
	CG(in_compilation) = EG(in_execution) = 0;
	EG(current_execute_data) = NULL;
	LONGJMP(*EG(bailout), FAILURE);
}
/* }}} */
END_EXTERN_C()

void zend_append_version_info(const zend_extension *extension) /* {{{ */
{
	char *new_info;
	uint new_info_length;

	new_info_length = sizeof("    with  v,  by \n")
						+ strlen(extension->name)
						+ strlen(extension->version)
						+ strlen(extension->copyright)
						+ strlen(extension->author);

	new_info = (char *) malloc(new_info_length + 1);

	sprintf(new_info, "    with %s v%s, %s, by %s\n", extension->name, extension->version, extension->copyright, extension->author);

	zend_version_info = (char *) realloc(zend_version_info, zend_version_info_length+new_info_length + 1);
	strcat(zend_version_info, new_info);
	zend_version_info_length += new_info_length;
	free(new_info);
}
/* }}} */

ZEND_API char *get_zend_version(void) /* {{{ */
{
	return zend_version_info;
}
/* }}} */

ZEND_API void zend_reset_locale_deps(TSRMLS_D) /* {{{ */
{
	UCollator *coll;
	UErrorCode status = U_ZERO_ERROR;

	if (UG(default_collator)) {
		zend_collator_destroy(UG(default_collator));
	}
	coll = ucol_open(UG(default_locale), &status);
	if (U_FAILURE(status)) {
		zend_error(E_ERROR, "Could not open collator for locale %s", UG(default_locale));
	}
	UG(default_collator) = zend_collator_create(coll);
}
/* }}} */

static void init_unicode_request_globals(TSRMLS_D) /* {{{ */
{
	UG(default_locale) = safe_estrdup(uloc_getDefault());
	UG(default_collator) = NULL;

	zend_reset_locale_deps(TSRMLS_C);
}
/* }}} */

static void shutdown_unicode_request_globals(TSRMLS_D) /* {{{ */
{
	zend_collator_destroy(UG(default_collator));
	efree(UG(default_locale));
}
/* }}} */

void zend_activate(TSRMLS_D) /* {{{ */
{
	gc_reset(TSRMLS_C);
	init_unicode_request_globals(TSRMLS_C);
	init_unicode_strings();
	init_compiler(TSRMLS_C);
	init_executor(TSRMLS_C);
	startup_scanner(TSRMLS_C);
}
/* }}} */

void zend_activate_modules(TSRMLS_D) /* {{{ */
{
	zend_hash_apply(&module_registry, (apply_func_t) module_registry_request_startup TSRMLS_CC);
}
/* }}} */

void zend_deactivate_modules(TSRMLS_D) /* {{{ */
{
	EG(opline_ptr) = NULL; /* we're no longer executing anything */

	zend_try {
		zend_hash_apply(&module_registry, (apply_func_t) module_registry_cleanup TSRMLS_CC);
	} zend_end_try();
}
/* }}} */

void zend_call_destructors(TSRMLS_D) /* {{{ */
{
	zend_try {
		shutdown_destructors(TSRMLS_C);
	} zend_end_try();
}
/* }}} */

void zend_deactivate(TSRMLS_D) /* {{{ */
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

#ifdef ZEND_DEBUG
	if (GC_G(gc_enabled) && !CG(unclean_shutdown)) {
		gc_collect_cycles(TSRMLS_C);
	}
#endif

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
	fprintf(stderr, "ZOBJ  %8d  %8d  %9d  %8d\n", GC_G(zobj_possible_root), GC_G(zobj_buffered), GC_G(zobj_remove_from_buffer), GC_G(zobj_marked_grey));
#endif

	zend_try {
		zend_ini_deactivate(TSRMLS_C);
	} zend_end_try();
}
/* }}} */

static int exec_done_cb(zend_module_entry *module TSRMLS_DC) /* {{{ */
{
	if (module->post_deactivate_func) {
		module->post_deactivate_func();
	}
	return 0;
}
/* }}} */

void zend_post_deactivate_modules(TSRMLS_D) /* {{{ */
{
	zend_hash_apply(&module_registry, (apply_func_t) exec_done_cb TSRMLS_CC);
	zend_hash_reverse_apply(&module_registry, (apply_func_t) module_registry_unload_temp TSRMLS_CC);
}
/* }}} */

BEGIN_EXTERN_C()
ZEND_API void zend_message_dispatcher(long message, void *data) /* {{{ */
{
	if (zend_message_dispatcher_p) {
		zend_message_dispatcher_p(message, data);
	}
}
/* }}} */
END_EXTERN_C()

ZEND_API int zend_get_configuration_directive(const char *name, uint name_length, zval *contents) /* {{{ */
{
	if (zend_get_configuration_directive_p) {
		return zend_get_configuration_directive_p(name, name_length, contents);
	} else {
		return FAILURE;
	}
}
/* }}} */

ZEND_API void zend_error(int type, const char *format, ...) /* {{{ */
{
	va_list args;
	va_list usr_copy;
	zval ***params;
	zval *retval;
	zval *z_error_type, *z_error_message, *z_error_filename, *z_error_lineno, *z_context;
	char *error_filename;
	uint error_lineno;
	zval *orig_user_error_handler;
	zend_bool in_compilation;
	zend_class_entry *saved_class_entry;
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
		case E_DEPRECATED:
		case E_WARNING:
		case E_USER_ERROR:
		case E_USER_WARNING:
		case E_USER_NOTICE:
		case E_USER_DEPRECATED:
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
			Z_STRLEN_P(z_error_message) = zend_vspprintf(&Z_STRVAL_P(z_error_message), 0, format, usr_copy);
			Z_TYPE_P(z_error_message) = IS_STRING;
			if (UG(unicode)) {
				char *str = Z_STRVAL_P(z_error_message);
				int len  = Z_STRLEN_P(z_error_message);

				ZVAL_RT_STRINGL(z_error_message, str, len, 1);
				efree(str);
			}
#ifdef va_copy
			va_end(usr_copy);
#endif

			Z_LVAL_P(z_error_type) = type;
			Z_TYPE_P(z_error_type) = IS_LONG;

			if (error_filename) {
				ZVAL_RT_STRING(z_error_filename, error_filename, 1);
			}

			Z_LVAL_P(z_error_lineno) = error_lineno;
			Z_TYPE_P(z_error_lineno) = IS_LONG;

			if (!EG(active_symbol_table)) {
				zend_rebuild_symbol_table(TSRMLS_C);
			}
			Z_ARRVAL_P(z_context) = EG(active_symbol_table);
			Z_TYPE_P(z_context) = IS_ARRAY;
			zval_copy_ctor(z_context);

			params = (zval ***) emalloc(sizeof(zval **)*5);
			params[0] = &z_error_type;
			params[1] = &z_error_message;
			params[2] = &z_error_filename;
			params[3] = &z_error_lineno;
			params[4] = &z_context;

			orig_user_error_handler = EG(user_error_handler);
			EG(user_error_handler) = NULL;

			/* User error handler may include() additinal PHP files.
			 * If an error was generated during comilation PHP will compile
			 * such scripts recursivly, but some CG() variables may be
			 * inconsistent. */

			in_compilation = zend_is_compiling(TSRMLS_C);
			if (in_compilation) {
				saved_class_entry = CG(active_class_entry);
				CG(active_class_entry) = NULL;
			}

			if (call_user_function_ex(CG(function_table), NULL, orig_user_error_handler, &retval, 5, params, 1, NULL TSRMLS_CC) == SUCCESS) {
				if (retval) {
					if (Z_TYPE_P(retval) == IS_BOOL && Z_LVAL_P(retval) == 0) {
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
/* }}} */

#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(DARWIN) && !defined(__hpux) && !defined(_AIX) && !defined(__osf__)
void zend_error_noreturn(int type, const char *format, ...) __attribute__ ((alias("zend_error"),noreturn));
#endif

ZEND_API void zend_output_debug_string(zend_bool trigger_break, const char *format, ...) /* {{{ */
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

ZEND_API int zend_execute_scripts(int type TSRMLS_DC, zval **retval, int file_count, ...) /* {{{ */
{
	va_list files;
	int i;
	zend_file_handle *file_handle;
	zend_op_array *orig_op_array = EG(active_op_array);
	zval **orig_retval_ptr_ptr = EG(return_value_ptr_ptr);

	va_start(files, file_count);
	for (i = 0; i < file_count; i++) {
		file_handle = va_arg(files, zend_file_handle *);
		if (!file_handle) {
			continue;
		}
		EG(active_op_array) = zend_compile_file(file_handle, type TSRMLS_CC);
		if (file_handle->opened_path) {
			int dummy = 1;
			zend_hash_add(&EG(included_files), file_handle->opened_path, strlen(file_handle->opened_path) + 1, (void *)&dummy, sizeof(int), NULL);
		}
		zend_destroy_file_handle(file_handle TSRMLS_CC);
		if (EG(active_op_array)) {
			EG(return_value_ptr_ptr) = retval ? retval : NULL;
			zend_execute(EG(active_op_array) TSRMLS_CC);
			if (EG(exception)) {
				EG(opline_ptr) = NULL;
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
						if (!EG(exception)) {
							EG(exception) = old_exception;
						}
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
			}
			destroy_op_array(EG(active_op_array) TSRMLS_CC);
			efree(EG(active_op_array));
		} else if (type==ZEND_REQUIRE) {
			va_end(files);
			EG(active_op_array) = orig_op_array;
			EG(return_value_ptr_ptr) = orig_retval_ptr_ptr;
			return FAILURE;
		}
	}
	va_end(files);
	EG(active_op_array) = orig_op_array;
	EG(return_value_ptr_ptr) = orig_retval_ptr_ptr;

	return SUCCESS;
}
/* }}} */

#define COMPILED_STRING_DESCRIPTION_FORMAT "%s(%d) : %s"

ZEND_API char *zend_make_compiled_string_description(const char *name TSRMLS_DC) /* {{{ */
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
