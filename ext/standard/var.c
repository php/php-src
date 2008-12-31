/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Jani Lehtimäki <jkl@njet.net>                               |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |          Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* {{{ includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "php.h"
#include "php_string.h"
#include "php_var.h"
#include "php_smart_str.h"
#include "basic_functions.h"
#include "php_incomplete_class.h"

#define COMMON (Z_ISREF_PP(struc) ? "&" : "")
/* }}} */

static void php_var_dump_unicode(UChar *ustr, int length, int verbose, char *quote, int escape TSRMLS_DC) /* {{{ */
{
	UChar32 c;
	int i;
	UErrorCode status = U_ZERO_ERROR;
	int clen;
	char *out = NULL;

	if (length == 0) {
		ZEND_PUTS(quote);
		ZEND_PUTS(quote);
		return;
	}

	zend_unicode_to_string_ex(ZEND_U_CONVERTER(UG(output_encoding_conv)), &out, &clen, ustr, length, &status);
	if (U_FAILURE(status)) {
		php_printf("problem converting string from Unicode: %s\n", u_errorName(status));
		efree(out);
		return;
	}

	ZEND_PUTS(quote);
	if (escape) {
		char *str;
		int str_len;

		str = php_addcslashes(out, clen, &str_len, 0, "'\\", 2 TSRMLS_CC);
		PHPWRITE(str, str_len);
		efree(str);
	} else {
		PHPWRITE(out, clen);
	}
	ZEND_PUTS(quote);
	if (verbose) {
		ZEND_PUTS(" {");
		/* output the code points (not code units) */
		if (length>=0) {
			/* s is not NUL-terminated */
			for (i = 0; i < length; /* U16_NEXT post-increments */) {
				U16_NEXT(ustr, i, length, c);
				php_printf(" %04x", c);
			}
		} else {
			/* s is NUL-terminated */
			for (i = 0; /* condition in loop body */; /* U16_NEXT post-increments */) {
				U16_NEXT(ustr, i, length, c);
				if (c == 0) {
					break;
				}
				php_printf(" %04x", c);
			}
		}
		php_printf(" }");
	}
	efree(out);
}
/* }}} */

static int php_array_element_dump(zval **zv TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	int level;
	int verbose;

	level = va_arg(args, int);
	verbose = va_arg(args, int);

	if (hash_key->nKeyLength == 0) { /* numeric key */
		php_printf("%*c[%ld]=>\n", level + 1, ' ', hash_key->h);
	} else { /* string key */
		php_printf("%*c[", level + 1, ' ');
		if (hash_key->type == IS_STRING) {
			php_printf("\"");
			PHPWRITE(hash_key->arKey.s, hash_key->nKeyLength - 1);
			php_printf("\"");
		} else if (hash_key->type == IS_UNICODE) {
			php_printf("u");
			php_var_dump_unicode(hash_key->arKey.u, hash_key->nKeyLength-1, verbose, "\"", 0 TSRMLS_CC);
		}
		php_printf("]=>\n");
	}
	php_var_dump(zv, level + 2, verbose TSRMLS_CC);
	return 0;
}
/* }}} */

static int php_object_property_dump(zval **zv TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	int level;
	zstr prop_name, class_name;
	int verbose;

	level = va_arg(args, int);
	verbose = va_arg(args, int);

	if (hash_key->nKeyLength == 0) { /* numeric key */
		php_printf("%*c[%ld]=>\n", level + 1, ' ', hash_key->h);
	} else { /* string key */
		int is_unicode = hash_key->type == IS_UNICODE;

		int unmangle = zend_u_unmangle_property_name(hash_key->type, hash_key->arKey, hash_key->nKeyLength-1, &class_name, &prop_name);
		php_printf("%*c[", level + 1, ' ');

		if (class_name.s && unmangle == SUCCESS) {
			if (class_name.s[0] == '*') {
				php_printf("%s\"%R\":protected", is_unicode ? "u" : "", hash_key->type, prop_name);
			} else {
				php_printf("%s\"%R\":%s\"%R\":private", is_unicode ? "u" : "", hash_key->type, prop_name, is_unicode ? "u" : "", hash_key->type, class_name);
			}
		} else {
			if (is_unicode) {
				php_printf("u");
				php_var_dump_unicode(hash_key->arKey.u, hash_key->nKeyLength-1, verbose, "\"", 0 TSRMLS_CC);
			} else {
				php_printf("\"");
				PHPWRITE(hash_key->arKey.s, hash_key->nKeyLength - 1);
				php_printf("\"");
			}
		}
		ZEND_PUTS("]=>\n");
	}
	php_var_dump(zv, level + 2, verbose TSRMLS_CC);
	return 0;
}
/* }}} */

PHPAPI void php_var_dump(zval **struc, int level, int verbose TSRMLS_DC) /* {{{ */
{
	HashTable *myht;
	zstr class_name;
	zend_uint class_name_len;
	int (*php_element_dump_func)(zval** TSRMLS_DC, int, va_list, zend_hash_key*);
	int is_temp;

	if (level > 1) {
		php_printf("%*c", level - 1, ' ');
	}

	switch (Z_TYPE_PP(struc)) {
	case IS_BOOL:
		php_printf("%sbool(%s)\n", COMMON, Z_LVAL_PP(struc) ? "true" : "false");
		break;
	case IS_NULL:
		php_printf("%sNULL\n", COMMON);
		break;
	case IS_LONG:
		php_printf("%sint(%ld)\n", COMMON, Z_LVAL_PP(struc));
		break;
	case IS_DOUBLE:
		php_printf("%sfloat(%.*G)\n", COMMON, (int) EG(precision), Z_DVAL_PP(struc));
		break;
	case IS_STRING:
		php_printf("%sstring(%d) \"", COMMON, Z_STRLEN_PP(struc));
		PHPWRITE(Z_STRVAL_PP(struc), Z_STRLEN_PP(struc));
		PUTS("\"\n");
		break;
	case IS_UNICODE:
		php_printf("%sunicode(%d) ", COMMON, u_countChar32((*struc)->value.ustr.val, (*struc)->value.ustr.len));
		php_var_dump_unicode((*struc)->value.ustr.val, (*struc)->value.ustr.len, verbose, "\"", 0 TSRMLS_CC);
		PUTS("\n");
		break;
	case IS_ARRAY:
		myht = Z_ARRVAL_PP(struc);
		if (myht->nApplyCount > 1) {
			PUTS("*RECURSION*\n");
			return;
		}
		php_printf("%sarray(%d) {\n", COMMON, zend_hash_num_elements(myht));
		php_element_dump_func = php_array_element_dump;
		is_temp = 0;
		goto head_done;
	case IS_OBJECT:
		myht = Z_OBJDEBUG_PP(struc, is_temp);
		if (myht && myht->nApplyCount > 1) {
			PUTS("*RECURSION*\n");
			return;
		}

		Z_OBJ_HANDLER(**struc, get_class_name)(*struc, &class_name, &class_name_len, 0 TSRMLS_CC);
		php_printf("%sobject(%v)#%d (%d) {\n", COMMON, class_name, Z_OBJ_HANDLE_PP(struc), myht ? zend_hash_num_elements(myht) : 0);
		efree(class_name.v);
		php_element_dump_func = php_object_property_dump;
head_done:
		if (myht) {
			zend_hash_apply_with_arguments(myht TSRMLS_CC, (apply_func_args_t) php_element_dump_func, 2, level, verbose);
			if (is_temp) {
				zend_hash_destroy(myht);
				efree(myht);
			}
		}
		if (level > 1) {
			php_printf("%*c", level-1, ' ');
		}
		PUTS("}\n");
		break;
	case IS_RESOURCE: {
		char *type_name;

		type_name = zend_rsrc_list_get_rsrc_type(Z_LVAL_PP(struc) TSRMLS_CC);
		php_printf("%sresource(%ld) of type (%s)\n", COMMON, Z_LVAL_PP(struc), type_name ? type_name : "Unknown");
		break;
	}
	default:
		php_printf("%sUNKNOWN:0\n", COMMON);
		break;
	}
}
/* }}} */

/* {{{ proto void var_dump(mixed var) U
   Dumps a string representation of variable to output */
PHP_FUNCTION(var_dump)
{
	zval ***args = NULL;
	int argc;
	int	i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &argc) == FAILURE) {
		return;
	}

	for (i = 0; i < argc; i++) {
		php_var_dump(args[i], 1, 0 TSRMLS_CC);
	}
	efree(args);
}
/* }}} */

/* {{{ proto void var_inspect(mixed var) U
   Dumps a string representation of variable to output (verbose form) */
PHP_FUNCTION(var_inspect)
{
	zval ***args = NULL;
	int argc;
	int	i;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &argc) == FAILURE) {
		return;
	}

	for (i = 0; i < argc; i++) {
		php_var_dump(args[i], 1, 1 TSRMLS_CC);
	}
	efree(args);
}
/* }}} */

static int zval_array_element_dump(zval **zv TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	int level;

	level = va_arg(args, int);

	if (hash_key->nKeyLength == 0) { /* numeric key */
		php_printf("%*c[%ld]=>\n", level + 1, ' ', hash_key->h);
	} else { /* string key */
		/* XXX: perphaps when we are inside the class we should permit access to
		 * private & protected values
		 */
		if (va_arg(args, int) &&
			((hash_key->type == IS_STRING && hash_key->arKey.s[0] == 0) ||
			(hash_key->type == IS_UNICODE && hash_key->arKey.u[0] == 0))
		) {
			return 0;
		}
		php_printf("%*c[", level + 1, ' ');
		if (hash_key->type == IS_STRING) {
			php_printf("\"");
			PHPWRITE(hash_key->arKey.s, hash_key->nKeyLength - 1);
			php_printf("\"");
		} else if (hash_key->type == IS_UNICODE) {
			php_printf("u");
			php_var_dump_unicode(hash_key->arKey.u, hash_key->nKeyLength-1, 1, "\"", 0 TSRMLS_CC);
		}
		php_printf("]=>\n");
	}
	php_debug_zval_dump(zv, level + 2, 1 TSRMLS_CC);
	return 0;
}
/* }}} */

static int zval_object_property_dump(zval **zv TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	int level;
	zstr prop_name, class_name;
	int verbose;

	level = va_arg(args, int);
	verbose = va_arg(args, int);

	if (hash_key->nKeyLength == 0) { /* numeric key */
		php_printf("%*c[%ld]=>\n", level + 1, ' ', hash_key->h);
	} else { /* string key */
		int is_unicode = hash_key->type == IS_UNICODE;

		zend_u_unmangle_property_name(hash_key->type, hash_key->arKey, hash_key->nKeyLength - 1, &class_name, &prop_name);
		php_printf("%*c[", level + 1, ' ');

		if (class_name.s) {
			if (class_name.s[0]=='*') {
				php_printf("%s\"%R\":protected", is_unicode ? "u" : "", hash_key->type, prop_name);
			} else {
				php_printf("%s\"%R\":%s\"%R\":private", is_unicode ? "u" : "", hash_key->type, prop_name, is_unicode ? "u" : "", hash_key->type, class_name);
			}
		} else {
			php_printf("%s\"%R\"", is_unicode ? "u" : "", hash_key->type, prop_name);
		}
		ZEND_PUTS("]=>\n");
	}
	php_debug_zval_dump(zv, level + 2, 1 TSRMLS_CC);
	return 0;
}
/* }}} */

PHPAPI void php_debug_zval_dump(zval **struc, int level, int verbose TSRMLS_DC) /* {{{ */
{
	HashTable *myht = NULL;
	zstr class_name;
	zend_uint class_name_len;
	zend_class_entry *ce;
	int (*zval_element_dump_func)(zval** TSRMLS_DC, int, va_list, zend_hash_key*);
	int is_temp = 0;

	if (level > 1) {
		php_printf("%*c", level - 1, ' ');
	}

	switch (Z_TYPE_PP(struc)) {
	case IS_BOOL:
		php_printf("%sbool(%s) refcount(%u)\n", COMMON, Z_LVAL_PP(struc)?"true":"false", Z_REFCOUNT_PP(struc));
		break;
	case IS_NULL:
		php_printf("%sNULL refcount(%u)\n", COMMON, Z_REFCOUNT_PP(struc));
		break;
	case IS_LONG:
		php_printf("%slong(%ld) refcount(%u)\n", COMMON, Z_LVAL_PP(struc), Z_REFCOUNT_PP(struc));
		break;
	case IS_DOUBLE:
		php_printf("%sdouble(%.*G) refcount(%u)\n", COMMON, (int) EG(precision), Z_DVAL_PP(struc), Z_REFCOUNT_PP(struc));
		break;
	case IS_STRING:
		php_printf("%sstring(%d) \"", COMMON, Z_STRLEN_PP(struc));
		PHPWRITE(Z_STRVAL_PP(struc), Z_STRLEN_PP(struc));
		php_printf("\" refcount(%u)\n", Z_REFCOUNT_PP(struc));
		break;
	case IS_UNICODE:
		php_printf("%sunicode(%d) ", COMMON, u_countChar32((*struc)->value.ustr.val, (*struc)->value.ustr.len));
		php_var_dump_unicode((*struc)->value.ustr.val, (*struc)->value.ustr.len, verbose, "\"", 0 TSRMLS_CC);
		php_printf(" refcount(%u)\n", Z_REFCOUNT_PP(struc));
		break;
	case IS_ARRAY:
		myht = Z_ARRVAL_PP(struc);
		if (myht->nApplyCount > 1) {
			PUTS("*RECURSION*\n");
			return;
		}
		php_printf("%sarray(%d) refcount(%u){\n", COMMON, zend_hash_num_elements(myht), Z_REFCOUNT_PP(struc));
		zval_element_dump_func = zval_array_element_dump;
		goto head_done;
	case IS_OBJECT:
		myht = Z_OBJDEBUG_PP(struc, is_temp);
		if (myht && myht->nApplyCount > 1) {
			PUTS("*RECURSION*\n");
			return;
		}
		ce = Z_OBJCE_PP(struc);
		Z_OBJ_HANDLER_PP(struc, get_class_name)(*struc, &class_name, &class_name_len, 0 TSRMLS_CC);
		php_printf("%sobject(%v)#%d (%d) refcount(%u){\n", COMMON, class_name, Z_OBJ_HANDLE_PP(struc), myht ? zend_hash_num_elements(myht) : 0, Z_REFCOUNT_PP(struc));
		efree(class_name.v);
		zval_element_dump_func = zval_object_property_dump;
head_done:
		if (myht) {
			zend_hash_apply_with_arguments(myht TSRMLS_CC, (apply_func_args_t) zval_element_dump_func, 1, level, (Z_TYPE_PP(struc) == IS_ARRAY ? 0 : 1));
			if (is_temp) {
				zend_hash_destroy(myht);
				efree(myht);
			}
		}
		if (level > 1) {
			php_printf("%*c", level - 1, ' ');
		}
		PUTS("}\n");
		break;
	case IS_RESOURCE: {
		char *type_name;

		type_name = zend_rsrc_list_get_rsrc_type(Z_LVAL_PP(struc) TSRMLS_CC);
		php_printf("%sresource(%ld) of type (%s) refcount(%u)\n", COMMON, Z_LVAL_PP(struc), type_name ? type_name : "Unknown", Z_REFCOUNT_PP(struc));
		break;
	}
	default:
		php_printf("%sUNKNOWN:0\n", COMMON);
		break;
	}
}
/* }}} */

/* {{{ proto void debug_zval_dump(mixed var) U
   Dumps a string representation of an internal zend value to output. */
PHP_FUNCTION(debug_zval_dump)
{
	zval ***args = NULL;
	int argc;
	int	i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &argc) == FAILURE) {
		return;
	}

	for (i = 0; i < argc; i++) {
		php_debug_zval_dump(args[i], 1, 1 TSRMLS_CC);
	}
	efree(args);
}
/* }}} */

static int php_array_element_export(zval **zv TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	int level;

	level = va_arg(args, int);

	if (hash_key->nKeyLength == 0) { /* numeric key */
		php_printf("%*c%ld => ", level + 1, ' ', hash_key->h);
	} else { /* string key */
		php_printf("%*c'", level + 1, ' ');
		if (hash_key->type == IS_UNICODE) {
			php_var_dump_unicode(hash_key->arKey.u, hash_key->nKeyLength-1, 0, "", 1 TSRMLS_CC);
		} else {
			char *key, *tmp_str;
			int key_len, tmp_len;
			key = php_addcslashes(hash_key->arKey.s, hash_key->nKeyLength - 1, &key_len, 0, "'\\", 2 TSRMLS_CC);
	 		tmp_str = php_str_to_str_ex(key, key_len, "\0", 1, "' . \"\\0\" . '", 12, &tmp_len, 0, NULL);
			PHPWRITE(tmp_str, tmp_len);
			efree(key);
	 		efree(tmp_str);
		}
		php_printf("' => ");
	}
	php_var_export(zv, level + 2 TSRMLS_CC);
	PUTS (",\n");
	return 0;
}
/* }}} */

static int php_object_element_export(zval **zv TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	int level;
	zstr prop_name, class_name;

	level = va_arg(args, int);

	if (hash_key->nKeyLength != 0) {
		php_printf("%*c", level + 1, ' ');
		zend_u_unmangle_property_name(hash_key->type, hash_key->arKey, hash_key->nKeyLength - 1, &class_name, &prop_name);
		php_printf(" '%R' => ", hash_key->type, prop_name);
		php_var_export(zv, level + 2 TSRMLS_CC);
		PUTS (",\n");
	}
	return 0;
}
/* }}} */

static void php_unicode_export(UChar *ustr, int ustr_len TSRMLS_DC) /* {{{ */
{
	UChar32 cp;
	int i = 0;
	char buf[10];
	int buf_len;
	int state = 0; /* 0 = in single quotes, 1 = in double quotes */

	/*
	 * We export all codepoints > 128 in escaped form to avoid encoding issues
	 * in case the result is used in a script.
	 */
	while (i < ustr_len) {
		U16_NEXT(ustr, i, ustr_len, cp);
		switch (cp) {
			case 0x0: /* '\0' */
				if (state == 0) {
					PHPWRITE("' . \"", 5);
					state = 1;
				}
				PHPWRITE("\\0", 2);
				break;

			case 0x27: /* '\'' */
				if (state == 1) {
					PHPWRITE("\" . '", 5);
					state = 0;
				}
				PHPWRITE("\\'", 2);
				break;

			case 0x5c: /* '\\' */
				PHPWRITE("\\\\", 2);
				break;

			default:
				if ((uint32_t)cp < 128) {
					if (state == 1) {
						PHPWRITE("\" . '", 5);
						state = 0;
					}
					buf[0] = (char) (short) cp;
					buf_len = 1;
				} else if (U_IS_BMP(cp)) {
					if (state == 0) {
						PHPWRITE("' . \"", 5);
						state = 1;
					}
					buf_len = snprintf(buf, sizeof(buf), "\\u%04X", cp);
				} else {
					if (state == 0) {
						PHPWRITE("' . \"", 5);
						state = 1;
					}
					buf_len = snprintf(buf, sizeof(buf), "\\u%06X", cp);
				}
				PHPWRITE(buf, buf_len);
				break;
		}
	}
	if (state == 1) { /* if we are in double quotes, go back to single */
		PHPWRITE("\" . '", 5);
	}
}
/* }}} */

PHPAPI void php_var_export(zval **struc, int level TSRMLS_DC) /* {{{ */
{
	HashTable *myht;
 	char *tmp_str, *tmp_str2;
 	int tmp_len, tmp_len2;
	zstr class_name;
	zend_uint class_name_len;

	switch (Z_TYPE_PP(struc)) {
	case IS_BOOL:
		php_printf("%s", Z_LVAL_PP(struc) ? "true" : "false");
		break;
	case IS_NULL:
		php_printf("NULL");
		break;
	case IS_LONG:
		php_printf("%ld", Z_LVAL_PP(struc));
		break;
	case IS_DOUBLE:
		php_printf("%.*H", (int) EG(precision), Z_DVAL_PP(struc));
		break;
	case IS_STRING:
		tmp_str = php_addcslashes(Z_STRVAL_PP(struc), Z_STRLEN_PP(struc), &tmp_len, 0, "'\\", 2 TSRMLS_CC);
		tmp_str2 = php_str_to_str_ex(tmp_str, tmp_len, "\0", 1, "' . \"\\0\" . '", 12, &tmp_len2, 0, NULL);
		PUTS ("'");
		PHPWRITE(tmp_str2, tmp_len2);
		PUTS ("'");
		efree(tmp_str2);
		efree(tmp_str);
		break;
	case IS_UNICODE:
		PUTS ("'");
		php_unicode_export(Z_USTRVAL_PP(struc), Z_USTRLEN_PP(struc) TSRMLS_CC);
		PUTS ("'");
		break;
	case IS_ARRAY:
		myht = Z_ARRVAL_PP(struc);
		if (level > 1) {
			php_printf("\n%*c", level - 1, ' ');
		}
		PUTS ("array (\n");
		zend_hash_apply_with_arguments(myht TSRMLS_CC, (apply_func_args_t) php_array_element_export, 1, level, 0);
		if (level > 1) {
			php_printf("%*c", level - 1, ' ');
		}
		PUTS(")");
		break;
	case IS_OBJECT:
		myht = Z_OBJPROP_PP(struc);
		if (level > 1) {
			php_printf("\n%*c", level - 1, ' ');
		}
		Z_OBJ_HANDLER(**struc, get_class_name)(*struc, &class_name, &class_name_len, 0 TSRMLS_CC);
		php_printf ("%v::__set_state(array(\n", class_name);
		efree(class_name.v);
		if (myht) {
			zend_hash_apply_with_arguments(myht TSRMLS_CC, (apply_func_args_t) php_object_element_export, 1, level);
		}
		if (level > 1) {
			php_printf("%*c", level - 1, ' ');
		}
		php_printf ("))");
		break;
	default:
		PUTS ("NULL");
		break;
	}
}
/* }}} */

/* {{{ proto mixed var_export(mixed var [, bool return]) U
   Outputs or returns a string representation of a variable */
PHP_FUNCTION(var_export)
{
	zval *var;
	zend_bool return_output = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &var, &return_output) == FAILURE) {
		return;
	}

	if (return_output) {
		php_output_start_default(TSRMLS_C);
	}

	/* UTODO
	 * We need to escape non-ASCII chars with \uXXXX format. php_var_export()
	 * currently uses output_encoding to export Unicode strings. Suppose it's
	 * set to utf-8. If you use the result of var_export() in non-utf-8 context,
	 * there may be trouble.
	 */
	php_var_export(&var, 1 TSRMLS_CC);

	if (return_output) {
		php_output_get_contents(return_value TSRMLS_CC);
		php_output_discard(TSRMLS_C);
	}
}
/* }}} */

static void php_var_serialize_intern(smart_str *buf, zval *struc, HashTable *var_hash TSRMLS_DC);

static inline int php_add_var_hash(HashTable *var_hash, zval *var, void *var_old TSRMLS_DC) /* {{{ */
{
	ulong var_no;
	char id[32], *p;
	register int len;

	/* relies on "(long)" being a perfect hash function for data pointers,
	 * however the actual identity of an object has had to be determined
	 * by its object handle and the class entry since 5.0. */
	if ((Z_TYPE_P(var) == IS_OBJECT) && Z_OBJ_HT_P(var)->get_class_entry) {
		p = smart_str_print_long(id + sizeof(id) - 1,
				(((size_t)Z_OBJCE_P(var) << 5)
				| ((size_t)Z_OBJCE_P(var) >> (sizeof(long) * 8 - 5)))
				+ (long) Z_OBJ_HANDLE_P(var));
		*(--p) = 'O';
		len = id + sizeof(id) - 1 - p;
	} else {
		p = smart_str_print_long(id + sizeof(id) - 1, (long) var);
		len = id + sizeof(id) - 1 - p;
	}

	if (var_old && zend_hash_find(var_hash, p, len, var_old) == SUCCESS) {
		if (!Z_ISREF_P(var)) {
			/* we still need to bump up the counter, since non-refs will
			 * be counted separately by unserializer */
			var_no = -1;
			zend_hash_next_index_insert(var_hash, &var_no, sizeof(var_no), NULL);
		}
		return FAILURE;
	}

	/* +1 because otherwise hash will think we are trying to store NULL pointer */
	var_no = zend_hash_num_elements(var_hash) + 1;
	zend_hash_add(var_hash, p, len, &var_no, sizeof(var_no), NULL);
	return SUCCESS;
}
/* }}} */

static inline void php_var_serialize_long(smart_str *buf, long val) /* {{{ */
{
	smart_str_appendl(buf, "i:", 2);
	smart_str_append_long(buf, val);
	smart_str_appendc(buf, ';');
}
/* }}} */

static inline void php_var_serialize_string(smart_str *buf, char *str, int len) /* {{{ */
{
	static const char hex[] = "0123456789abcdef";
	unsigned char c;
	int i;

	smart_str_appendl(buf, "S:", 2);
	smart_str_append_long(buf, len);
	smart_str_appendl(buf, ":\"", 2);

	for (i = 0; i < len; i++) {
		c = (unsigned char) str[i];
		if (c < 128 && c != 0x5c /*'\\'*/) {
			smart_str_appendc(buf, c & 0xff);
		} else {
			smart_str_appendc(buf, 0x5c /*'\\'*/);
			smart_str_appendc(buf, hex[(c >> 4) & 0xf]);
			smart_str_appendc(buf, hex[(c >> 0) & 0xf]);
		}
	}

	smart_str_appendl(buf, "\";", 2);
}
/* }}} */

static inline void php_var_serialize_ustr(smart_str *buf, UChar *ustr, int len) /* {{{ */
{
	static const char hex[] = "0123456789abcdef";
	UChar c;
	int i;

	for (i = 0; i < len; i++) {
		c = ustr[i];
		if (c < 128 && c != 0x5c /*'\\'*/) {
			smart_str_appendc(buf, c & 0xff);
		} else {
			smart_str_appendc(buf, 0x5c /*'\\'*/);
			smart_str_appendc(buf, hex[(c >> 12) & 0xf]);
			smart_str_appendc(buf, hex[(c >> 8) & 0xf]);
			smart_str_appendc(buf, hex[(c >> 4) & 0xf]);
			smart_str_appendc(buf, hex[(c >> 0) & 0xf]);
		}
	}
}
/* }}} */

static inline void php_var_serialize_unicode(smart_str *buf, UChar *ustr, int len) /* {{{ */
{
	smart_str_appendl(buf, "U:", 2);
	smart_str_append_long(buf, len);
	smart_str_appendl(buf, ":\"", 2);
	php_var_serialize_ustr(buf, ustr, len);
	smart_str_appendl(buf, "\";", 2);
}
/* }}} */

static inline zend_bool php_var_serialize_class_name(smart_str *buf, zval *struc TSRMLS_DC) /* {{{ */
{
	PHP_CLASS_ATTRIBUTES;

	PHP_SET_CLASS_ATTRIBUTES(struc);
	smart_str_appendl(buf, "O:", 2);
	smart_str_append_long(buf, name_len);
	smart_str_appendl(buf, ":\"", 2);
	if (UG(unicode)) {
		php_var_serialize_ustr(buf, class_name.u, name_len);
	} else {
		smart_str_appendl(buf, class_name.s, name_len);
	}
	smart_str_appendl(buf, "\":", 2);
	PHP_CLEANUP_CLASS_ATTRIBUTES();
	return incomplete_class;
}
/* }}} */

static void php_var_serialize_class(smart_str *buf, zval *struc, zval *retval_ptr, HashTable *var_hash TSRMLS_DC) /* {{{ */
{
	int count;
	zend_bool incomplete_class;
	zstr star;

	star.s = "*";

	incomplete_class = php_var_serialize_class_name(buf, struc TSRMLS_CC);
	/* count after serializing name, since php_var_serialize_class_name
	 * changes the count if the variable is incomplete class */
	count = zend_hash_num_elements(HASH_OF(retval_ptr));
	if (incomplete_class) {
		--count;
	}
	smart_str_append_long(buf, count);
	smart_str_appendl(buf, ":{", 2);

	if (count > 0) {
		zstr key;
		unsigned int key_len;
		zval **d, **name;
		ulong index;
		HashPosition pos;
		int i;
		zval nval, *nvalp;

		ZVAL_NULL(&nval);
		nvalp = &nval;

		zend_hash_internal_pointer_reset_ex(HASH_OF(retval_ptr), &pos);

		for (;; zend_hash_move_forward_ex(HASH_OF(retval_ptr), &pos)) {
			i = zend_hash_get_current_key_ex(HASH_OF(retval_ptr), &key, &key_len, &index, 0, &pos);

			if (i == HASH_KEY_NON_EXISTANT) {
				break;
			}

			if (incomplete_class &&
				key_len == sizeof(MAGIC_MEMBER) &&
				ZEND_U_EQUAL(i, key, key_len-1, MAGIC_MEMBER, sizeof(MAGIC_MEMBER)-1)
			) {
				continue;
			}
			zend_hash_get_current_data_ex(HASH_OF(retval_ptr), (void **) &name, &pos);

			if (Z_TYPE_PP(name) != (UG(unicode) ? IS_UNICODE : IS_STRING)) {
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "__sleep should return an array only containing the names of instance-variables to serialize");
				/* we should still add element even if it's not OK,
				 * since we already wrote the length of the array before */
				smart_str_appendl(buf,"N;", 2);
				continue;
			}
			if (zend_u_hash_find(Z_OBJPROP_P(struc), Z_TYPE_PP(name), Z_UNIVAL_PP(name), Z_UNILEN_PP(name) + 1, (void *) &d) == SUCCESS) {
				if (Z_TYPE_PP(name) == IS_UNICODE) {
					php_var_serialize_unicode(buf, Z_USTRVAL_PP(name), Z_USTRLEN_PP(name));
				} else {
					php_var_serialize_string(buf, Z_STRVAL_PP(name), Z_STRLEN_PP(name));
				}
				php_var_serialize_intern(buf, *d, var_hash TSRMLS_CC);
			} else {
				zend_class_entry *ce;
				ce = zend_get_class_entry(struc TSRMLS_CC);
				if (ce) {
					zstr prot_name, priv_name;
					int prop_name_length;

					do {
						zend_u_mangle_property_name(&priv_name, &prop_name_length, Z_TYPE_PP(name), ce->name, ce->name_length, Z_UNIVAL_PP(name), Z_UNILEN_PP(name), ce->type & ZEND_INTERNAL_CLASS);
						if (zend_u_hash_find(Z_OBJPROP_P(struc), Z_TYPE_PP(name), priv_name, prop_name_length + 1, (void *) &d) == SUCCESS) {
							if (Z_TYPE_PP(name) == IS_UNICODE) {
								php_var_serialize_unicode(buf, priv_name.u, prop_name_length);
							} else {
								php_var_serialize_string(buf, priv_name.s, prop_name_length);
							}
							efree(priv_name.v);
							php_var_serialize_intern(buf, *d, var_hash TSRMLS_CC);
							break;
						}
						efree(priv_name.v);
						zend_u_mangle_property_name(&prot_name, &prop_name_length, Z_TYPE_PP(name), star, 1, Z_UNIVAL_PP(name), Z_UNILEN_PP(name), ce->type & ZEND_INTERNAL_CLASS);
						if (zend_u_hash_find(Z_OBJPROP_P(struc), Z_TYPE_PP(name), prot_name, prop_name_length+1, (void *) &d) == SUCCESS) {
							if (Z_TYPE_PP(name) == IS_UNICODE) {
								php_var_serialize_unicode(buf, prot_name.u, prop_name_length);
							} else {
								php_var_serialize_string(buf, prot_name.s, prop_name_length);
							}
							efree(prot_name.v);
							php_var_serialize_intern(buf, *d, var_hash TSRMLS_CC);
							break;
						}
						efree(prot_name.v);
						php_error_docref(NULL TSRMLS_CC, E_NOTICE, "\"%R\" returned as member variable from __sleep() but does not exist", Z_TYPE_PP(name), Z_UNIVAL_PP(name));
						if (Z_TYPE_PP(name) == IS_UNICODE) {
							php_var_serialize_unicode(buf, Z_USTRVAL_PP(name), Z_USTRLEN_PP(name));
						} else {
							php_var_serialize_string(buf, Z_STRVAL_PP(name), Z_STRLEN_PP(name));
						}
						php_var_serialize_intern(buf, nvalp, var_hash TSRMLS_CC);
					} while (0);
				} else {
					if (Z_TYPE_PP(name) == IS_UNICODE) {
						php_var_serialize_unicode(buf, Z_USTRVAL_PP(name), Z_USTRLEN_PP(name));
					} else {
						php_var_serialize_string(buf, Z_STRVAL_PP(name), Z_STRLEN_PP(name));
					}
					php_var_serialize_intern(buf, nvalp, var_hash TSRMLS_CC);
				}
			}
		}
	}
	smart_str_appendc(buf, '}');
}
/* }}} */

static void php_var_serialize_intern(smart_str *buf, zval *struc, HashTable *var_hash TSRMLS_DC) /* {{{ */
{
	int i;
	ulong *var_already;
	HashTable *myht;

	if (var_hash && php_add_var_hash(var_hash, struc, (void *) &var_already TSRMLS_CC) == FAILURE) {
		if (Z_ISREF_P(struc)) {
			smart_str_appendl(buf, "R:", 2);
			smart_str_append_long(buf, *var_already);
			smart_str_appendc(buf, ';');
			return;
		} else if (Z_TYPE_P(struc) == IS_OBJECT) {
			smart_str_appendl(buf, "r:", 2);
			smart_str_append_long(buf, *var_already);
			smart_str_appendc(buf, ';');
			return;
		}
	}

	switch (Z_TYPE_P(struc)) {
		case IS_BOOL:
			smart_str_appendl(buf, "b:", 2);
			smart_str_append_long(buf, Z_LVAL_P(struc));
			smart_str_appendc(buf, ';');
			return;

		case IS_NULL:
			smart_str_appendl(buf, "N;", 2);
			return;

		case IS_LONG:
			php_var_serialize_long(buf, Z_LVAL_P(struc));
			return;

		case IS_DOUBLE: {
				char *s;

				smart_str_appendl(buf, "d:", 2);
				s = (char *) safe_emalloc(PG(serialize_precision), 1, MAX_LENGTH_OF_DOUBLE + 1);
				php_gcvt(Z_DVAL_P(struc), PG(serialize_precision), '.', 'E', s);
				smart_str_appends(buf, s);
				smart_str_appendc(buf, ';');
				efree(s);
				return;
			}

		case IS_STRING:
			php_var_serialize_string(buf, Z_STRVAL_P(struc), Z_STRLEN_P(struc));
			return;

		case IS_UNICODE:
			php_var_serialize_unicode(buf, Z_USTRVAL_P(struc), Z_USTRLEN_P(struc));
			return;

		case IS_OBJECT: {
				zval *retval_ptr = NULL;
				zval fname;
				int res;
				zend_class_entry *ce = NULL;

				if (Z_OBJ_HT_P(struc)->get_class_entry) {
					ce = Z_OBJCE_P(struc);
				}

				if (ce && ce->serialize != NULL) {
					/* has custom handler */
					int serialized_type;
					zstr serialized_data;
					zend_uint serialized_length;

					serialized_data.v = NULL;
					if (ce->serialize(struc, &serialized_type, &serialized_data, &serialized_length, (zend_serialize_data *)var_hash TSRMLS_CC) == SUCCESS) {
						smart_str_appendl(buf, "C:", 2);
						smart_str_append_long(buf, Z_OBJCE_P(struc)->name_length);
						smart_str_appendl(buf, ":\"", 2);

						if (UG(unicode)) {
							php_var_serialize_ustr(buf, Z_OBJCE_P(struc)->name.u, Z_OBJCE_P(struc)->name_length);
						} else {
							smart_str_appendl(buf, Z_OBJCE_P(struc)->name.s, Z_OBJCE_P(struc)->name_length);
						}

						smart_str_appendl(buf, "\":", 2);

						smart_str_append_long(buf, serialized_length);
						if (serialized_type == IS_UNICODE) {
							smart_str_appendl(buf, ":U:{", 4);
							php_var_serialize_ustr(buf, serialized_data.u, serialized_length);
						} else if (serialized_type == IS_STRING) {
							smart_str_appendl(buf, ":{", 2);
							smart_str_appendl(buf, serialized_data.s, serialized_length);
						} else {
							smart_str_appendc(buf, 'N');
						}
						smart_str_appendc(buf, '}');
					} else {
						smart_str_appendl(buf, "N;", 2);
					}
					if (serialized_data.v) {
						efree(serialized_data.v);
					}
					return;
				}

				if (ce && ce != PHP_IC_ENTRY && zend_hash_exists(&ce->function_table, "__sleep", sizeof("__sleep"))) {
					INIT_PZVAL(&fname);
					ZVAL_ASCII_STRINGL(&fname, "__sleep", sizeof("__sleep") - 1, 1);
					res = call_user_function_ex(CG(function_table), &struc, &fname, &retval_ptr, 0, 0, 1, NULL TSRMLS_CC);
					zval_dtor(&fname);
					if (res == SUCCESS && !EG(exception)) {
						if (retval_ptr) {
							if (HASH_OF(retval_ptr)) {
								php_var_serialize_class(buf, struc, retval_ptr, var_hash TSRMLS_CC);
							} else {
								php_error_docref(NULL TSRMLS_CC, E_NOTICE, "__sleep should return an array only containing the names of instance-variables to serialize");
								/* we should still add element even if it's not OK,
								 * since we already wrote the length of the array before */
								smart_str_appendl(buf,"N;", 2);
							}
							zval_ptr_dtor(&retval_ptr);
						}
						return;
					}
				}

				if (retval_ptr) {
					zval_ptr_dtor(&retval_ptr);
				}
				/* fall-through */
			}
		case IS_ARRAY: {
			zend_bool incomplete_class = 0;
			if (Z_TYPE_P(struc) == IS_ARRAY) {
				smart_str_appendl(buf, "a:", 2);
				myht = HASH_OF(struc);
			} else {
				incomplete_class = php_var_serialize_class_name(buf, struc TSRMLS_CC);
				myht = Z_OBJPROP_P(struc);
			}
			/* count after serializing name, since php_var_serialize_class_name
			 * changes the count if the variable is incomplete class */
			i = myht ? zend_hash_num_elements(myht) : 0;
			if (i > 0 && incomplete_class) {
				--i;
			}
			smart_str_append_long(buf, i);
			smart_str_appendl(buf, ":{", 2);
			if (i > 0) {
				zstr key;
				zval **data;
				ulong index;
				uint key_len;
				HashPosition pos;

				zend_hash_internal_pointer_reset_ex(myht, &pos);
				for (;; zend_hash_move_forward_ex(myht, &pos)) {
					i = zend_hash_get_current_key_ex(myht, &key, &key_len, &index, 0, &pos);
					if (i == HASH_KEY_NON_EXISTANT) {
						break;
					}

					if (incomplete_class &&
						key_len == sizeof(MAGIC_MEMBER) &&
						ZEND_U_EQUAL(i, key, key_len - 1, MAGIC_MEMBER, sizeof(MAGIC_MEMBER) - 1)
					) {
						continue;
					}

					switch (i) {
						case HASH_KEY_IS_LONG:
							php_var_serialize_long(buf, index);
							break;
						case HASH_KEY_IS_STRING:
							php_var_serialize_string(buf, key.s, key_len - 1);
							break;
						case HASH_KEY_IS_UNICODE:
							php_var_serialize_unicode(buf, key.u, key_len - 1);
							break;
					}

					/* we should still add element even if it's not OK,
					 * since we already wrote the length of the array before */
					if (zend_hash_get_current_data_ex(myht, (void **) &data, &pos) != SUCCESS
						|| !data
						|| data == &struc
						|| (Z_TYPE_PP(data) == IS_ARRAY && Z_ARRVAL_PP(data)->nApplyCount > 1)
					) {
						smart_str_appendl(buf, "N;", 2);
					} else {
						if (Z_TYPE_PP(data) == IS_ARRAY) {
							Z_ARRVAL_PP(data)->nApplyCount++;
						}
						php_var_serialize_intern(buf, *data, var_hash TSRMLS_CC);
						if (Z_TYPE_PP(data) == IS_ARRAY) {
							Z_ARRVAL_PP(data)->nApplyCount--;
						}
					}
				}
			}
			smart_str_appendc(buf, '}');
			return;
		}
		default:
			smart_str_appendl(buf, "i:0;", 4);
			return;
	}
}
/* }}} */

PHPAPI void php_var_serialize(smart_str *buf, zval **struc, HashTable *var_hash TSRMLS_DC) /* {{{ */
{
	php_var_serialize_intern(buf, *struc, var_hash TSRMLS_CC);
	smart_str_0(buf);
}
/* }}} */

/* {{{ proto string serialize(mixed variable) U
   Returns a string representation of variable (which can later be unserialized) */
PHP_FUNCTION(serialize)
{
	zval **struc;
	php_serialize_data_t var_hash;
	smart_str buf = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &struc) == FAILURE) {
		return;
	}

	Z_TYPE_P(return_value) = IS_STRING;
	Z_STRVAL_P(return_value) = NULL;
	Z_STRLEN_P(return_value) = 0;

	PHP_VAR_SERIALIZE_INIT(var_hash);
	php_var_serialize(&buf, struc, &var_hash TSRMLS_CC);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	if (buf.c) {
		RETVAL_ASCII_STRINGL(buf.c, buf.len, 0);
		if (UG(unicode)) {
			smart_str_free(&buf);
		}
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto mixed unserialize(string variable_representation) U
   Takes a string representation of variable and recreates it */
PHP_FUNCTION(unserialize)
{
	char *buf = NULL;
	int buf_len;
	const unsigned char *p;
	php_unserialize_data_t var_hash;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&", &buf, &buf_len, UG(ascii_conv)) == FAILURE) {
		RETURN_FALSE;
	}

	if (buf_len == 0) {
		RETURN_FALSE;
	}

	p = (const unsigned char*) buf;
	PHP_VAR_UNSERIALIZE_INIT(var_hash);
	if (!php_var_unserialize(&return_value, &p, p + buf_len, &var_hash TSRMLS_CC)) {
		PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
		zval_dtor(return_value);
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Error at offset %ld of %d bytes", (long)((char*)p - buf), buf_len);
		RETURN_FALSE;
	}
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
}
/* }}} */

/* {{{ proto int memory_get_usage([real_usage]) U
   Returns the allocated by PHP memory */
PHP_FUNCTION(memory_get_usage) {
	zend_bool real_usage = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &real_usage) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_LONG(zend_memory_usage(real_usage TSRMLS_CC));
}
/* }}} */

/* {{{ proto int memory_get_peak_usage([real_usage]) U
   Returns the peak allocated by PHP memory */
PHP_FUNCTION(memory_get_peak_usage) {
	zend_bool real_usage = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &real_usage) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_LONG(zend_memory_peak_usage(real_usage TSRMLS_CC));
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
