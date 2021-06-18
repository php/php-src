/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

/* {{{ includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "php.h"
#include "php_string.h"
#include "php_var.h"
#include "zend_smart_str.h"
#include "basic_functions.h"
#include "php_incomplete_class.h"
/* }}} */

struct php_serialize_data {
	HashTable ht;
	uint32_t n;
};

#define COMMON (is_ref ? "&" : "")

static void php_array_element_dump(zval *zv, zend_ulong index, zend_string *key, int level) /* {{{ */
{
	if (key == NULL) { /* numeric key */
		php_printf("%*c[" ZEND_LONG_FMT "]=>\n", level + 1, ' ', index);
	} else { /* string key */
		php_printf("%*c[\"", level + 1, ' ');
		PHPWRITE(ZSTR_VAL(key), ZSTR_LEN(key));
		php_printf("\"]=>\n");
	}
	php_var_dump(zv, level + 2);
}
/* }}} */

static void php_object_property_dump(zend_property_info *prop_info, zval *zv, zend_ulong index, zend_string *key, int level) /* {{{ */
{
	const char *prop_name, *class_name;

	if (key == NULL) { /* numeric key */
		php_printf("%*c[" ZEND_LONG_FMT "]=>\n", level + 1, ' ', index);
	} else { /* string key */
		int unmangle = zend_unmangle_property_name(key, &class_name, &prop_name);
		php_printf("%*c[", level + 1, ' ');

		if (class_name && unmangle == SUCCESS) {
			if (class_name[0] == '*') {
				php_printf("\"%s\":protected", prop_name);
			} else {
				php_printf("\"%s\":\"%s\":private", prop_name, class_name);
			}
		} else {
			php_printf("\"");
			PHPWRITE(ZSTR_VAL(key), ZSTR_LEN(key));
			php_printf("\"");
		}
		ZEND_PUTS("]=>\n");
	}

	if (Z_TYPE_P(zv) == IS_UNDEF) {
		ZEND_ASSERT(prop_info->type);
		php_printf("%*cuninitialized(%s%s)\n",
			level + 1, ' ',
			ZEND_TYPE_ALLOW_NULL(prop_info->type) ? "?" : "",
			ZEND_TYPE_IS_CLASS(prop_info->type) ?
				ZSTR_VAL(ZEND_TYPE_IS_CE(prop_info->type) ? ZEND_TYPE_CE(prop_info->type)->name : ZEND_TYPE_NAME(prop_info->type)) :
				zend_get_type_by_const(ZEND_TYPE_CODE(prop_info->type)));
	} else {
		php_var_dump(zv, level + 2);
	}
}
/* }}} */

PHPAPI void php_var_dump(zval *struc, int level) /* {{{ */
{
	HashTable *myht;
	zend_string *class_name;
	int is_ref = 0;
	zend_ulong num;
	zend_string *key;
	zval *val;
	uint32_t count;

	if (level > 1) {
		php_printf("%*c", level - 1, ' ');
	}

again:
	switch (Z_TYPE_P(struc)) {
		case IS_FALSE:
			php_printf("%sbool(false)\n", COMMON);
			break;
		case IS_TRUE:
			php_printf("%sbool(true)\n", COMMON);
			break;
		case IS_NULL:
			php_printf("%sNULL\n", COMMON);
			break;
		case IS_LONG:
			php_printf("%sint(" ZEND_LONG_FMT ")\n", COMMON, Z_LVAL_P(struc));
			break;
		case IS_DOUBLE:
			php_printf("%sfloat(%.*G)\n", COMMON, (int) EG(precision), Z_DVAL_P(struc));
			break;
		case IS_STRING:
			php_printf("%sstring(%zd) \"", COMMON, Z_STRLEN_P(struc));
			PHPWRITE(Z_STRVAL_P(struc), Z_STRLEN_P(struc));
			PUTS("\"\n");
			break;
		case IS_ARRAY:
			myht = Z_ARRVAL_P(struc);
			if (!(GC_FLAGS(myht) & GC_IMMUTABLE)) {
				if (level > 1) {
					if (GC_IS_RECURSIVE(myht)) {
						PUTS("*RECURSION*\n");
						return;
					}
					GC_PROTECT_RECURSION(myht);
				}
				GC_ADDREF(myht);
			}
			count = zend_array_count(myht);
			php_printf("%sarray(%d) {\n", COMMON, count);
			ZEND_HASH_FOREACH_KEY_VAL_IND(myht, num, key, val) {
				php_array_element_dump(val, num, key, level);
			} ZEND_HASH_FOREACH_END();
			if (!(GC_FLAGS(myht) & GC_IMMUTABLE)) {
				if (level > 1) {
					GC_UNPROTECT_RECURSION(myht);
				}
				GC_DELREF(myht);
			}
			if (level > 1) {
				php_printf("%*c", level-1, ' ');
			}
			PUTS("}\n");
			break;
		case IS_OBJECT:
			if (Z_IS_RECURSIVE_P(struc)) {
				PUTS("*RECURSION*\n");
				return;
			}
			Z_PROTECT_RECURSION_P(struc);

			myht = zend_get_properties_for(struc, ZEND_PROP_PURPOSE_DEBUG);
			class_name = Z_OBJ_HANDLER_P(struc, get_class_name)(Z_OBJ_P(struc));
			php_printf("%sobject(%s)#%d (%d) {\n", COMMON, ZSTR_VAL(class_name), Z_OBJ_HANDLE_P(struc), myht ? zend_array_count(myht) : 0);
			zend_string_release_ex(class_name, 0);

			if (myht) {
				zend_ulong num;
				zend_string *key;
				zval *val;

				ZEND_HASH_FOREACH_KEY_VAL(myht, num, key, val) {
					zend_property_info *prop_info = NULL;

					if (Z_TYPE_P(val) == IS_INDIRECT) {
						val = Z_INDIRECT_P(val);
						if (key) {
							prop_info = zend_get_typed_property_info_for_slot(Z_OBJ_P(struc), val);
						}
					}

					if (!Z_ISUNDEF_P(val) || prop_info) {
						php_object_property_dump(prop_info, val, num, key, level);
					}
				} ZEND_HASH_FOREACH_END();
				zend_release_properties(myht);
			}
			if (level > 1) {
				php_printf("%*c", level-1, ' ');
			}
			PUTS("}\n");
			Z_UNPROTECT_RECURSION_P(struc);
			break;
		case IS_RESOURCE: {
			const char *type_name = zend_rsrc_list_get_rsrc_type(Z_RES_P(struc));
			php_printf("%sresource(%d) of type (%s)\n", COMMON, Z_RES_P(struc)->handle, type_name ? type_name : "Unknown");
			break;
		}
		case IS_REFERENCE:
			//??? hide references with refcount==1 (for compatibility)
			if (Z_REFCOUNT_P(struc) > 1) {
				is_ref = 1;
			}
			struc = Z_REFVAL_P(struc);
			goto again;
			break;
		default:
			php_printf("%sUNKNOWN:0\n", COMMON);
			break;
	}
}
/* }}} */

/* {{{ proto void var_dump(mixed var)
   Dumps a string representation of variable to output */
PHP_FUNCTION(var_dump)
{
	zval *args;
	int argc;
	int	i;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	for (i = 0; i < argc; i++) {
		php_var_dump(&args[i], 1);
	}
}
/* }}} */

static void zval_array_element_dump(zval *zv, zend_ulong index, zend_string *key, int level) /* {{{ */
{
	if (key == NULL) { /* numeric key */
		php_printf("%*c[" ZEND_LONG_FMT "]=>\n", level + 1, ' ', index);
	} else { /* string key */
		php_printf("%*c[\"", level + 1, ' ');
		PHPWRITE(ZSTR_VAL(key), ZSTR_LEN(key));
		php_printf("\"]=>\n");
	}
	php_debug_zval_dump(zv, level + 2);
}
/* }}} */

static void zval_object_property_dump(zend_property_info *prop_info, zval *zv, zend_ulong index, zend_string *key, int level) /* {{{ */
{
	const char *prop_name, *class_name;

	if (key == NULL) { /* numeric key */
		php_printf("%*c[" ZEND_LONG_FMT "]=>\n", level + 1, ' ', index);
	} else { /* string key */
		zend_unmangle_property_name(key, &class_name, &prop_name);
		php_printf("%*c[", level + 1, ' ');

		if (class_name) {
			if (class_name[0] == '*') {
				php_printf("\"%s\":protected", prop_name);
			} else {
				php_printf("\"%s\":\"%s\":private", prop_name, class_name);
			}
		} else {
			php_printf("\"%s\"", prop_name);
		}
		ZEND_PUTS("]=>\n");
	}
	if (prop_info && Z_TYPE_P(zv) == IS_UNDEF) {
		ZEND_ASSERT(prop_info->type);
		php_printf("%*cuninitialized(%s%s)\n",
			level + 1, ' ',
			ZEND_TYPE_ALLOW_NULL(prop_info->type) ? "?" : "",
			ZEND_TYPE_IS_CLASS(prop_info->type) ?
				ZSTR_VAL(ZEND_TYPE_IS_CE(prop_info->type) ? ZEND_TYPE_CE(prop_info->type)->name : ZEND_TYPE_NAME(prop_info->type)) :
				zend_get_type_by_const(ZEND_TYPE_CODE(prop_info->type)));
	} else {
		php_debug_zval_dump(zv, level + 2);
	}
}
/* }}} */

PHPAPI void php_debug_zval_dump(zval *struc, int level) /* {{{ */
{
	HashTable *myht = NULL;
	zend_string *class_name;
	int is_ref = 0;
	zend_ulong index;
	zend_string *key;
	zval *val;
	uint32_t count;

	if (level > 1) {
		php_printf("%*c", level - 1, ' ');
	}

again:
	switch (Z_TYPE_P(struc)) {
	case IS_FALSE:
		php_printf("%sbool(false)\n", COMMON);
		break;
	case IS_TRUE:
		php_printf("%sbool(true)\n", COMMON);
		break;
	case IS_NULL:
		php_printf("%sNULL\n", COMMON);
		break;
	case IS_LONG:
		php_printf("%sint(" ZEND_LONG_FMT ")\n", COMMON, Z_LVAL_P(struc));
		break;
	case IS_DOUBLE:
		php_printf("%sfloat(%.*G)\n", COMMON, (int) EG(precision), Z_DVAL_P(struc));
		break;
	case IS_STRING:
		php_printf("%sstring(%zd) \"", COMMON, Z_STRLEN_P(struc));
		PHPWRITE(Z_STRVAL_P(struc), Z_STRLEN_P(struc));
		php_printf("\" refcount(%u)\n", Z_REFCOUNTED_P(struc) ? Z_REFCOUNT_P(struc) : 1);
		break;
	case IS_ARRAY:
		myht = Z_ARRVAL_P(struc);
		if (!(GC_FLAGS(myht) & GC_IMMUTABLE)) {
			if (level > 1) {
				if (GC_IS_RECURSIVE(myht)) {
					PUTS("*RECURSION*\n");
					return;
				}
				GC_PROTECT_RECURSION(myht);
			}
			GC_ADDREF(myht);
		}
		count = zend_array_count(myht);
		php_printf("%sarray(%d) refcount(%u){\n", COMMON, count, Z_REFCOUNTED_P(struc) ? Z_REFCOUNT_P(struc) - 1 : 1);
		ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, val) {
			zval_array_element_dump(val, index, key, level);
		} ZEND_HASH_FOREACH_END();
		if (!(GC_FLAGS(myht) & GC_IMMUTABLE)) {
			if (level > 1) {
				GC_UNPROTECT_RECURSION(myht);
			}
			GC_DELREF(myht);
		}
		if (level > 1) {
			php_printf("%*c", level - 1, ' ');
		}
		PUTS("}\n");
		break;
	case IS_OBJECT:
		myht = zend_get_properties_for(struc, ZEND_PROP_PURPOSE_DEBUG);
		if (myht) {
			if (GC_IS_RECURSIVE(myht)) {
				PUTS("*RECURSION*\n");
				zend_release_properties(myht);
				return;
			}
			GC_PROTECT_RECURSION(myht);
		}
		class_name = Z_OBJ_HANDLER_P(struc, get_class_name)(Z_OBJ_P(struc));
		php_printf("%sobject(%s)#%d (%d) refcount(%u){\n", COMMON, ZSTR_VAL(class_name), Z_OBJ_HANDLE_P(struc), myht ? zend_array_count(myht) : 0, Z_REFCOUNT_P(struc));
		zend_string_release_ex(class_name, 0);
		if (myht) {
			ZEND_HASH_FOREACH_KEY_VAL(myht, index, key, val) {
				zend_property_info *prop_info = NULL;

				if (Z_TYPE_P(val) == IS_INDIRECT) {
					val = Z_INDIRECT_P(val);
					if (key) {
						prop_info = zend_get_typed_property_info_for_slot(Z_OBJ_P(struc), val);
					}
				}

				if (!Z_ISUNDEF_P(val) || prop_info) {
					zval_object_property_dump(prop_info, val, index, key, level);
				}
			} ZEND_HASH_FOREACH_END();
			GC_UNPROTECT_RECURSION(myht);
			zend_release_properties(myht);
		}
		if (level > 1) {
			php_printf("%*c", level - 1, ' ');
		}
		PUTS("}\n");
		break;
	case IS_RESOURCE: {
		const char *type_name = zend_rsrc_list_get_rsrc_type(Z_RES_P(struc));
		php_printf("%sresource(%d) of type (%s) refcount(%u)\n", COMMON, Z_RES_P(struc)->handle, type_name ? type_name : "Unknown", Z_REFCOUNT_P(struc));
		break;
	}
	case IS_REFERENCE:
		//??? hide references with refcount==1 (for compatibility)
		if (Z_REFCOUNT_P(struc) > 1) {
			is_ref = 1;
		}
		struc = Z_REFVAL_P(struc);
		goto again;
	default:
		php_printf("%sUNKNOWN:0\n", COMMON);
		break;
	}
}
/* }}} */

/* {{{ proto void debug_zval_dump(mixed var)
   Dumps a string representation of an internal zend value to output. */
PHP_FUNCTION(debug_zval_dump)
{
	zval *args;
	int argc;
	int	i;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	for (i = 0; i < argc; i++) {
		php_debug_zval_dump(&args[i], 1);
	}
}
/* }}} */

#define buffer_append_spaces(buf, num_spaces) \
	do { \
		char *tmp_spaces; \
		size_t tmp_spaces_len; \
		tmp_spaces_len = spprintf(&tmp_spaces, 0,"%*c", num_spaces, ' '); \
		smart_str_appendl(buf, tmp_spaces, tmp_spaces_len); \
		efree(tmp_spaces); \
	} while(0);

static void php_array_element_export(zval *zv, zend_ulong index, zend_string *key, int level, smart_str *buf) /* {{{ */
{
	if (key == NULL) { /* numeric key */
		buffer_append_spaces(buf, level+1);
		smart_str_append_long(buf, (zend_long) index);
		smart_str_appendl(buf, " => ", 4);

	} else { /* string key */
		zend_string *tmp_str;
		zend_string *ckey = php_addcslashes(key, "'\\", 2);
		tmp_str = php_str_to_str(ZSTR_VAL(ckey), ZSTR_LEN(ckey), "\0", 1, "' . \"\\0\" . '", 12);

		buffer_append_spaces(buf, level + 1);

		smart_str_appendc(buf, '\'');
		smart_str_append(buf, tmp_str);
		smart_str_appendl(buf, "' => ", 5);

		zend_string_free(ckey);
		zend_string_free(tmp_str);
	}
	php_var_export_ex(zv, level + 2, buf);

	smart_str_appendc(buf, ',');
	smart_str_appendc(buf, '\n');
}
/* }}} */

static void php_object_element_export(zval *zv, zend_ulong index, zend_string *key, int level, smart_str *buf) /* {{{ */
{
	buffer_append_spaces(buf, level + 2);
	if (key != NULL) {
		const char *class_name, *prop_name;
		size_t prop_name_len;
		zend_string *pname_esc;

		zend_unmangle_property_name_ex(key, &class_name, &prop_name, &prop_name_len);
		pname_esc = php_addcslashes_str(prop_name, prop_name_len, "'\\", 2);

		smart_str_appendc(buf, '\'');
		smart_str_append(buf, pname_esc);
		smart_str_appendc(buf, '\'');
		zend_string_release_ex(pname_esc, 0);
	} else {
		smart_str_append_long(buf, (zend_long) index);
	}
	smart_str_appendl(buf, " => ", 4);
	php_var_export_ex(zv, level + 2, buf);
	smart_str_appendc(buf, ',');
	smart_str_appendc(buf, '\n');
}
/* }}} */

PHPAPI void php_var_export_ex(zval *struc, int level, smart_str *buf) /* {{{ */
{
	HashTable *myht;
	char tmp_str[PHP_DOUBLE_MAX_LENGTH];
	zend_string *ztmp, *ztmp2;
	zend_ulong index;
	zend_string *key;
	zval *val;

again:
	switch (Z_TYPE_P(struc)) {
		case IS_FALSE:
			smart_str_appendl(buf, "false", 5);
			break;
		case IS_TRUE:
			smart_str_appendl(buf, "true", 4);
			break;
		case IS_NULL:
			smart_str_appendl(buf, "NULL", 4);
			break;
		case IS_LONG:
			/* INT_MIN as a literal will be parsed as a float. Emit something like
			 * -9223372036854775807-1 to avoid this. */
			if (Z_LVAL_P(struc) == ZEND_LONG_MIN) {
				smart_str_append_long(buf, ZEND_LONG_MIN+1);
				smart_str_appends(buf, "-1");
				break;
			}
			smart_str_append_long(buf, Z_LVAL_P(struc));
			break;
		case IS_DOUBLE:
			php_gcvt(Z_DVAL_P(struc), (int)PG(serialize_precision), '.', 'E', tmp_str);
			smart_str_appends(buf, tmp_str);
			/* Without a decimal point, PHP treats a number literal as an int.
			 * This check even works for scientific notation, because the
			 * mantissa always contains a decimal point.
			 * We need to check for finiteness, because INF, -INF and NAN
			 * must not have a decimal point added.
			 */
			if (zend_finite(Z_DVAL_P(struc)) && NULL == strchr(tmp_str, '.')) {
				smart_str_appendl(buf, ".0", 2);
			}
			break;
		case IS_STRING:
			ztmp = php_addcslashes(Z_STR_P(struc), "'\\", 2);
			ztmp2 = php_str_to_str(ZSTR_VAL(ztmp), ZSTR_LEN(ztmp), "\0", 1, "' . \"\\0\" . '", 12);

			smart_str_appendc(buf, '\'');
			smart_str_append(buf, ztmp2);
			smart_str_appendc(buf, '\'');

			zend_string_free(ztmp);
			zend_string_free(ztmp2);
			break;
		case IS_ARRAY:
			myht = Z_ARRVAL_P(struc);
			if (!(GC_FLAGS(myht) & GC_IMMUTABLE)) {
				if (GC_IS_RECURSIVE(myht)) {
					smart_str_appendl(buf, "NULL", 4);
					zend_error(E_WARNING, "var_export does not handle circular references");
					return;
				}
				GC_ADDREF(myht);
				GC_PROTECT_RECURSION(myht);
			}
			if (level > 1) {
				smart_str_appendc(buf, '\n');
				buffer_append_spaces(buf, level - 1);
			}
			smart_str_appendl(buf, "array (\n", 8);
			ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, val) {
				php_array_element_export(val, index, key, level, buf);
			} ZEND_HASH_FOREACH_END();
			if (!(GC_FLAGS(myht) & GC_IMMUTABLE)) {
				GC_UNPROTECT_RECURSION(myht);
				GC_DELREF(myht);
			}
			if (level > 1) {
				buffer_append_spaces(buf, level - 1);
			}
			smart_str_appendc(buf, ')');

			break;

		case IS_OBJECT:
			myht = zend_get_properties_for(struc, ZEND_PROP_PURPOSE_VAR_EXPORT);
			if (myht) {
				if (GC_IS_RECURSIVE(myht)) {
					smart_str_appendl(buf, "NULL", 4);
					zend_error(E_WARNING, "var_export does not handle circular references");
					zend_release_properties(myht);
					return;
				} else {
					GC_TRY_PROTECT_RECURSION(myht);
				}
			}
			if (level > 1) {
				smart_str_appendc(buf, '\n');
				buffer_append_spaces(buf, level - 1);
			}

			/* stdClass has no __set_state method, but can be casted to */
			if (Z_OBJCE_P(struc) == zend_standard_class_def) {
				smart_str_appendl(buf, "(object) array(\n", 16);
			} else {
				smart_str_append(buf, Z_OBJCE_P(struc)->name);
				smart_str_appendl(buf, "::__set_state(array(\n", 21);
			}

			if (myht) {
				ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, val) {
					php_object_element_export(val, index, key, level, buf);
				} ZEND_HASH_FOREACH_END();
				GC_TRY_UNPROTECT_RECURSION(myht);
				zend_release_properties(myht);
			}
			if (level > 1) {
				buffer_append_spaces(buf, level - 1);
			}
			if (Z_OBJCE_P(struc) == zend_standard_class_def) {
				smart_str_appendc(buf, ')');
			} else {
				smart_str_appendl(buf, "))", 2);
			}

			break;
		case IS_REFERENCE:
			struc = Z_REFVAL_P(struc);
			goto again;
			break;
		default:
			smart_str_appendl(buf, "NULL", 4);
			break;
	}
}
/* }}} */

/* FOR BC reasons, this will always perform and then print */
PHPAPI void php_var_export(zval *struc, int level) /* {{{ */
{
	smart_str buf = {0};
	php_var_export_ex(struc, level, &buf);
	smart_str_0(&buf);
	PHPWRITE(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
	smart_str_free(&buf);
}
/* }}} */

/* {{{ proto mixed var_export(mixed var [, bool return])
   Outputs or returns a string representation of a variable */
PHP_FUNCTION(var_export)
{
	zval *var;
	zend_bool return_output = 0;
	smart_str buf = {0};

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(var)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(return_output)
	ZEND_PARSE_PARAMETERS_END();

	php_var_export_ex(var, 1, &buf);
	smart_str_0 (&buf);

	if (return_output) {
		RETURN_NEW_STR(buf.s);
	} else {
		PHPWRITE(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
		smart_str_free(&buf);
	}
}
/* }}} */

static void php_var_serialize_intern(smart_str *buf, zval *struc, php_serialize_data_t var_hash);

static inline zend_long php_add_var_hash(php_serialize_data_t data, zval *var) /* {{{ */
{
	zval *zv;
	zend_ulong key;
	zend_bool is_ref = Z_ISREF_P(var);

	data->n += 1;

	if (!is_ref && Z_TYPE_P(var) != IS_OBJECT) {
		return 0;
	}

	/* References to objects are treated as if the reference didn't exist */
	if (is_ref && Z_TYPE_P(Z_REFVAL_P(var)) == IS_OBJECT) {
		var = Z_REFVAL_P(var);
	}

	/* Index for the variable is stored using the numeric value of the pointer to
	 * the zend_refcounted struct */
	key = (zend_ulong) (zend_uintptr_t) Z_COUNTED_P(var);
	zv = zend_hash_index_find(&data->ht, key);

	if (zv) {
		/* References are only counted once, undo the data->n increment above */
		if (is_ref && Z_LVAL_P(zv) != -1) {
			data->n -= 1;
		}

		return Z_LVAL_P(zv);
	} else {
		zval zv_n;
		ZVAL_LONG(&zv_n, data->n);
		zend_hash_index_add_new(&data->ht, key, &zv_n);

		/* Additionally to the index, we also store the variable, to ensure that it is
		 * not destroyed during serialization and its pointer reused. The variable is
		 * stored at the numeric value of the pointer + 1, which cannot be the location
		 * of another zend_refcounted structure. */
		zend_hash_index_add_new(&data->ht, key + 1, var);
		Z_ADDREF_P(var);

		return 0;
	}
}
/* }}} */

static inline void php_var_serialize_long(smart_str *buf, zend_long val) /* {{{ */
{
	smart_str_appendl(buf, "i:", 2);
	smart_str_append_long(buf, val);
	smart_str_appendc(buf, ';');
}
/* }}} */

static inline void php_var_serialize_string(smart_str *buf, char *str, size_t len) /* {{{ */
{
	smart_str_appendl(buf, "s:", 2);
	smart_str_append_unsigned(buf, len);
	smart_str_appendl(buf, ":\"", 2);
	smart_str_appendl(buf, str, len);
	smart_str_appendl(buf, "\";", 2);
}
/* }}} */

static inline zend_bool php_var_serialize_class_name(smart_str *buf, zval *struc) /* {{{ */
{
	PHP_CLASS_ATTRIBUTES;

	PHP_SET_CLASS_ATTRIBUTES(struc);
	smart_str_appendl(buf, "O:", 2);
	smart_str_append_unsigned(buf, ZSTR_LEN(class_name));
	smart_str_appendl(buf, ":\"", 2);
	smart_str_append(buf, class_name);
	smart_str_appendl(buf, "\":", 2);
	PHP_CLEANUP_CLASS_ATTRIBUTES();
	return incomplete_class;
}
/* }}} */

static int php_var_serialize_call_sleep(zval *retval, zval *struc) /* {{{ */
{
	zval fname;
	int res;

	ZVAL_STRINGL(&fname, "__sleep", sizeof("__sleep") - 1);
	BG(serialize_lock)++;
	res = call_user_function(NULL, struc, &fname, retval, 0, 0);
	BG(serialize_lock)--;
	zval_ptr_dtor_str(&fname);

	if (res == FAILURE || Z_ISUNDEF_P(retval)) {
		zval_ptr_dtor(retval);
		return FAILURE;
	}

	if (!HASH_OF(retval)) {
		zval_ptr_dtor(retval);
		php_error_docref(NULL, E_NOTICE, "__sleep should return an array only containing the names of instance-variables to serialize");
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

static int php_var_serialize_call_magic_serialize(zval *retval, zval *obj) /* {{{ */
{
	zval fname;
	int res;

	ZVAL_STRINGL(&fname, "__serialize", sizeof("__serialize") - 1);
	BG(serialize_lock)++;
	res = call_user_function(CG(function_table), obj, &fname, retval, 0, 0);
	BG(serialize_lock)--;
	zval_ptr_dtor_str(&fname);

	if (res == FAILURE || Z_ISUNDEF_P(retval)) {
		zval_ptr_dtor(retval);
		return FAILURE;
	}

	if (Z_TYPE_P(retval) != IS_ARRAY) {
		zval_ptr_dtor(retval);
		zend_type_error("%s::__serialize() must return an array", ZSTR_VAL(Z_OBJCE_P(obj)->name));
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

static int php_var_serialize_try_add_sleep_prop(
		HashTable *ht, HashTable *props, zend_string *name, zend_string *error_name, zval *struc) /* {{{ */
{
	zval *val = zend_hash_find(props, name);
	if (val == NULL) {
		return FAILURE;
	}

	if (Z_TYPE_P(val) == IS_INDIRECT) {
		val = Z_INDIRECT_P(val);
		if (Z_TYPE_P(val) == IS_UNDEF) {
			zend_property_info *info = zend_get_typed_property_info_for_slot(Z_OBJ_P(struc), val);
			if (info) {
				return SUCCESS;
			}
			return FAILURE;
		}
	}

	if (!zend_hash_add(ht, name, val)) {
		php_error_docref(NULL, E_NOTICE,
			"\"%s\" is returned from __sleep multiple times", ZSTR_VAL(error_name));
		return SUCCESS;
	}

	Z_TRY_ADDREF_P(val);
	return SUCCESS;
}
/* }}} */

static int php_var_serialize_get_sleep_props(
		HashTable *ht, zval *struc, HashTable *sleep_retval) /* {{{ */
{
	zend_class_entry *ce = Z_OBJCE_P(struc);
	HashTable *props = zend_get_properties_for(struc, ZEND_PROP_PURPOSE_SERIALIZE);
	zval *name_val;
	int retval = SUCCESS;

	zend_hash_init(ht, zend_hash_num_elements(sleep_retval), NULL, ZVAL_PTR_DTOR, 0);
	/* TODO: Rewrite this by fetching the property info instead of trying out different
	 * name manglings? */
	ZEND_HASH_FOREACH_VAL_IND(sleep_retval, name_val) {
		zend_string *name, *tmp_name, *priv_name, *prot_name;

		ZVAL_DEREF(name_val);
		if (Z_TYPE_P(name_val) != IS_STRING) {
			php_error_docref(NULL, E_NOTICE,
					"__sleep should return an array only containing the names of instance-variables to serialize.");
		}

		name = zval_get_tmp_string(name_val, &tmp_name);
		if (php_var_serialize_try_add_sleep_prop(ht, props, name, name, struc) == SUCCESS) {
			zend_tmp_string_release(tmp_name);
			continue;
		}

		if (EG(exception)) {
			zend_tmp_string_release(tmp_name);
			retval = FAILURE;
			break;
		}

		priv_name = zend_mangle_property_name(
			ZSTR_VAL(ce->name), ZSTR_LEN(ce->name),
			ZSTR_VAL(name), ZSTR_LEN(name), ce->type & ZEND_INTERNAL_CLASS);
		if (php_var_serialize_try_add_sleep_prop(ht, props, priv_name, name, struc) == SUCCESS) {
			zend_tmp_string_release(tmp_name);
			zend_string_release(priv_name);
			continue;
		}
		zend_string_release(priv_name);

		if (EG(exception)) {
			zend_tmp_string_release(tmp_name);
			retval = FAILURE;
			break;
		}

		prot_name = zend_mangle_property_name(
			"*", 1, ZSTR_VAL(name), ZSTR_LEN(name), ce->type & ZEND_INTERNAL_CLASS);
		if (php_var_serialize_try_add_sleep_prop(ht, props, prot_name, name, struc) == SUCCESS) {
			zend_tmp_string_release(tmp_name);
			zend_string_release(prot_name);
			continue;
		}
		zend_string_release(prot_name);

		if (EG(exception)) {
			zend_tmp_string_release(tmp_name);
			retval = FAILURE;
			break;
		}

		php_error_docref(NULL, E_NOTICE,
			"\"%s\" returned as member variable from __sleep() but does not exist", ZSTR_VAL(name));
		zend_hash_add(ht, name, &EG(uninitialized_zval));
		zend_tmp_string_release(tmp_name);
	} ZEND_HASH_FOREACH_END();

	zend_release_properties(props);
	return retval;
}
/* }}} */

static void php_var_serialize_nested_data(smart_str *buf, zval *struc, HashTable *ht, uint32_t count, zend_bool incomplete_class, php_serialize_data_t var_hash) /* {{{ */
{
	smart_str_append_unsigned(buf, count);
	smart_str_appendl(buf, ":{", 2);
	if (count > 0) {
		zend_string *key;
		zval *data;
		zend_ulong index;

		ZEND_HASH_FOREACH_KEY_VAL_IND(ht, index, key, data) {
			if (incomplete_class && strcmp(ZSTR_VAL(key), MAGIC_MEMBER) == 0) {
				continue;
			}

			if (!key) {
				php_var_serialize_long(buf, index);
			} else {
				php_var_serialize_string(buf, ZSTR_VAL(key), ZSTR_LEN(key));
			}

			if (Z_ISREF_P(data) && Z_REFCOUNT_P(data) == 1) {
				data = Z_REFVAL_P(data);
			}

			/* we should still add element even if it's not OK,
			 * since we already wrote the length of the array before */
			if (Z_TYPE_P(data) == IS_ARRAY) {
				if (UNEXPECTED(Z_IS_RECURSIVE_P(data))
					|| UNEXPECTED(Z_TYPE_P(struc) == IS_ARRAY && Z_ARR_P(data) == Z_ARR_P(struc))) {
					php_add_var_hash(var_hash, struc);
					smart_str_appendl(buf, "N;", 2);
				} else {
					if (Z_REFCOUNTED_P(data)) {
						Z_PROTECT_RECURSION_P(data);
					}
					php_var_serialize_intern(buf, data, var_hash);
					if (Z_REFCOUNTED_P(data)) {
						Z_UNPROTECT_RECURSION_P(data);
					}
				}
			} else {
				php_var_serialize_intern(buf, data, var_hash);
			}
		} ZEND_HASH_FOREACH_END();
	}
	smart_str_appendc(buf, '}');
}
/* }}} */

static void php_var_serialize_class(smart_str *buf, zval *struc, zval *retval_ptr, php_serialize_data_t var_hash) /* {{{ */
{
	HashTable props;
	if (php_var_serialize_get_sleep_props(&props, struc, HASH_OF(retval_ptr)) == SUCCESS) {
		php_var_serialize_class_name(buf, struc);
		php_var_serialize_nested_data(
			buf, struc, &props, zend_hash_num_elements(&props), /* incomplete_class */ 0, var_hash);
	}
	zend_hash_destroy(&props);
}
/* }}} */

static void php_var_serialize_intern(smart_str *buf, zval *struc, php_serialize_data_t var_hash) /* {{{ */
{
	zend_long var_already;
	HashTable *myht;

	if (EG(exception)) {
		return;
	}

	if (var_hash && (var_already = php_add_var_hash(var_hash, struc))) {
		if (var_already == -1) {
			/* Reference to an object that failed to serialize, replace with null. */
			smart_str_appendl(buf, "N;", 2);
			return;
		} else if (Z_ISREF_P(struc)) {
			smart_str_appendl(buf, "R:", 2);
			smart_str_append_long(buf, var_already);
			smart_str_appendc(buf, ';');
			return;
		} else if (Z_TYPE_P(struc) == IS_OBJECT) {
			smart_str_appendl(buf, "r:", 2);
			smart_str_append_long(buf, var_already);
			smart_str_appendc(buf, ';');
			return;
		}
	}

again:
	switch (Z_TYPE_P(struc)) {
		case IS_FALSE:
			smart_str_appendl(buf, "b:0;", 4);
			return;

		case IS_TRUE:
			smart_str_appendl(buf, "b:1;", 4);
			return;

		case IS_NULL:
			smart_str_appendl(buf, "N;", 2);
			return;

		case IS_LONG:
			php_var_serialize_long(buf, Z_LVAL_P(struc));
			return;

		case IS_DOUBLE: {
			char tmp_str[PHP_DOUBLE_MAX_LENGTH];
			smart_str_appendl(buf, "d:", 2);
			php_gcvt(Z_DVAL_P(struc), (int)PG(serialize_precision), '.', 'E', tmp_str);
			smart_str_appends(buf, tmp_str);
			smart_str_appendc(buf, ';');
			return;
		}

		case IS_STRING:
			php_var_serialize_string(buf, Z_STRVAL_P(struc), Z_STRLEN_P(struc));
			return;

		case IS_OBJECT: {
				zend_class_entry *ce = Z_OBJCE_P(struc);
				zend_bool incomplete_class;
				uint32_t count;

				if (zend_hash_str_exists(&ce->function_table, "__serialize", sizeof("__serialize")-1)) {
					zval retval, obj;
					zend_string *key;
					zval *data;
					zend_ulong index;

					Z_ADDREF_P(struc);
					ZVAL_OBJ(&obj, Z_OBJ_P(struc));
					if (php_var_serialize_call_magic_serialize(&retval, &obj) == FAILURE) {
						if (!EG(exception)) {
							smart_str_appendl(buf, "N;", 2);
						}
						zval_ptr_dtor(&obj);
						return;
					}

					php_var_serialize_class_name(buf, &obj);
					smart_str_append_unsigned(buf, zend_array_count(Z_ARRVAL(retval)));
					smart_str_appendl(buf, ":{", 2);
					ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL(retval), index, key, data) {
						if (!key) {
							php_var_serialize_long(buf, index);
						} else {
							php_var_serialize_string(buf, ZSTR_VAL(key), ZSTR_LEN(key));
						}

						if (Z_ISREF_P(data) && Z_REFCOUNT_P(data) == 1) {
							data = Z_REFVAL_P(data);
						}
						php_var_serialize_intern(buf, data, var_hash);
					} ZEND_HASH_FOREACH_END();
					smart_str_appendc(buf, '}');

					zval_ptr_dtor(&obj);
					zval_ptr_dtor(&retval);
					return;
				}

				if (ce->serialize != NULL) {
					/* has custom handler */
					unsigned char *serialized_data = NULL;
					size_t serialized_length;

					if (ce->serialize(struc, &serialized_data, &serialized_length, (zend_serialize_data *)var_hash) == SUCCESS) {
						smart_str_appendl(buf, "C:", 2);
						smart_str_append_unsigned(buf, ZSTR_LEN(Z_OBJCE_P(struc)->name));
						smart_str_appendl(buf, ":\"", 2);
						smart_str_append(buf, Z_OBJCE_P(struc)->name);
						smart_str_appendl(buf, "\":", 2);

						smart_str_append_unsigned(buf, serialized_length);
						smart_str_appendl(buf, ":{", 2);
						smart_str_appendl(buf, (char *) serialized_data, serialized_length);
						smart_str_appendc(buf, '}');
					} else {
						/* Mark this value in the var_hash, to avoid creating references to it. */
						zval *var_idx = zend_hash_index_find(&var_hash->ht,
							(zend_ulong) (zend_uintptr_t) Z_COUNTED_P(struc));
						ZVAL_LONG(var_idx, -1);
						smart_str_appendl(buf, "N;", 2);
					}
					if (serialized_data) {
						efree(serialized_data);
					}
					return;
				}

				if (ce != PHP_IC_ENTRY && zend_hash_str_exists(&ce->function_table, "__sleep", sizeof("__sleep")-1)) {
					zval retval, tmp;

					Z_ADDREF_P(struc);
					ZVAL_OBJ(&tmp, Z_OBJ_P(struc));

					if (php_var_serialize_call_sleep(&retval, &tmp) == FAILURE) {
						if (!EG(exception)) {
							/* we should still add element even if it's not OK,
							 * since we already wrote the length of the array before */
							smart_str_appendl(buf, "N;", 2);
						}
						zval_ptr_dtor(&tmp);
						return;
					}

					php_var_serialize_class(buf, &tmp, &retval, var_hash);
					zval_ptr_dtor(&retval);
					zval_ptr_dtor(&tmp);
					return;
				}

				incomplete_class = php_var_serialize_class_name(buf, struc);
				myht = zend_get_properties_for(struc, ZEND_PROP_PURPOSE_SERIALIZE);
				/* count after serializing name, since php_var_serialize_class_name
				 * changes the count if the variable is incomplete class */
				count = zend_array_count(myht);
				if (count > 0 && incomplete_class) {
					--count;
				}
				php_var_serialize_nested_data(buf, struc, myht, count, incomplete_class, var_hash);
				zend_release_properties(myht);
				return;
			}
		case IS_ARRAY:
			smart_str_appendl(buf, "a:", 2);
			myht = Z_ARRVAL_P(struc);
			php_var_serialize_nested_data(
				buf, struc, myht, zend_array_count(myht), /* incomplete_class */ 0, var_hash);
			return;
		case IS_REFERENCE:
			struc = Z_REFVAL_P(struc);
			goto again;
		default:
			smart_str_appendl(buf, "i:0;", 4);
			return;
	}
}
/* }}} */

PHPAPI void php_var_serialize(smart_str *buf, zval *struc, php_serialize_data_t *data) /* {{{ */
{
	php_var_serialize_intern(buf, struc, *data);
	smart_str_0(buf);
}
/* }}} */

PHPAPI php_serialize_data_t php_var_serialize_init() {
	struct php_serialize_data *d;
	/* fprintf(stderr, "SERIALIZE_INIT      == lock: %u, level: %u\n", BG(serialize_lock), BG(serialize).level); */
	if (BG(serialize_lock) || !BG(serialize).level) {
		d = emalloc(sizeof(struct php_serialize_data));
		zend_hash_init(&d->ht, 16, NULL, ZVAL_PTR_DTOR, 0);
		d->n = 0;
		if (!BG(serialize_lock)) {
			BG(serialize).data = d;
			BG(serialize).level = 1;
		}
	} else {
		d = BG(serialize).data;
		++BG(serialize).level;
	}
	return d;
}

PHPAPI void php_var_serialize_destroy(php_serialize_data_t d) {
	/* fprintf(stderr, "SERIALIZE_DESTROY   == lock: %u, level: %u\n", BG(serialize_lock), BG(serialize).level); */
	if (BG(serialize_lock) || BG(serialize).level == 1) {
		zend_hash_destroy(&d->ht);
		efree(d);
	}
	if (!BG(serialize_lock) && !--BG(serialize).level) {
		BG(serialize).data = NULL;
	}
}

/* {{{ proto string serialize(mixed variable)
   Returns a string representation of variable (which can later be unserialized) */
PHP_FUNCTION(serialize)
{
	zval *struc;
	php_serialize_data_t var_hash;
	smart_str buf = {0};

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(struc)
	ZEND_PARSE_PARAMETERS_END();

	PHP_VAR_SERIALIZE_INIT(var_hash);
	php_var_serialize(&buf, struc, &var_hash);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	if (EG(exception)) {
		smart_str_free(&buf);
		RETURN_FALSE;
	}

	if (buf.s) {
		RETURN_NEW_STR(buf.s);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto mixed unserialize(string variable_representation[, array options])
   Takes a string representation of variable and recreates it */
PHP_FUNCTION(unserialize)
{
	char *buf = NULL;
	size_t buf_len;
	const unsigned char *p;
	php_unserialize_data_t var_hash;
	zval *options = NULL;
	zval *retval;
	HashTable *class_hash = NULL, *prev_class_hash;
	zend_long prev_max_depth, prev_cur_depth;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(buf, buf_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY(options)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (buf_len == 0) {
		RETURN_FALSE;
	}

	p = (const unsigned char*) buf;
	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	prev_class_hash = php_var_unserialize_get_allowed_classes(var_hash);
	prev_max_depth = php_var_unserialize_get_max_depth(var_hash);
	prev_cur_depth = php_var_unserialize_get_cur_depth(var_hash);
	if (options != NULL) {
		zval *classes, *max_depth;

		classes = zend_hash_str_find_deref(Z_ARRVAL_P(options), "allowed_classes", sizeof("allowed_classes")-1);
		if (classes && Z_TYPE_P(classes) != IS_ARRAY && Z_TYPE_P(classes) != IS_TRUE && Z_TYPE_P(classes) != IS_FALSE) {
			php_error_docref(NULL, E_WARNING, "allowed_classes option should be array or boolean");
			RETVAL_FALSE;
			goto cleanup;
		}

		if(classes && (Z_TYPE_P(classes) == IS_ARRAY || !zend_is_true(classes))) {
			ALLOC_HASHTABLE(class_hash);
			zend_hash_init(class_hash, (Z_TYPE_P(classes) == IS_ARRAY)?zend_hash_num_elements(Z_ARRVAL_P(classes)):0, NULL, NULL, 0);
		}
		if(class_hash && Z_TYPE_P(classes) == IS_ARRAY) {
			zval *entry;
			zend_string *lcname;

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(classes), entry) {
				convert_to_string_ex(entry);
				lcname = zend_string_tolower(Z_STR_P(entry));
				zend_hash_add_empty_element(class_hash, lcname);
		        zend_string_release_ex(lcname, 0);
			} ZEND_HASH_FOREACH_END();

			/* Exception during string conversion. */
			if (EG(exception)) {
				goto cleanup;
			}
		}
		php_var_unserialize_set_allowed_classes(var_hash, class_hash);

		max_depth = zend_hash_str_find_deref(Z_ARRVAL_P(options), "max_depth", sizeof("max_depth") - 1);
		if (max_depth) {
			if (Z_TYPE_P(max_depth) != IS_LONG) {
				php_error_docref(NULL, E_WARNING, "max_depth should be int");
				RETVAL_FALSE;
				goto cleanup;
			}
			if (Z_LVAL_P(max_depth) < 0) {
				php_error_docref(NULL, E_WARNING, "max_depth cannot be negative");
				RETVAL_FALSE;
				goto cleanup;
			}

			php_var_unserialize_set_max_depth(var_hash, Z_LVAL_P(max_depth));
			/* If the max_depth for a nested unserialize() call has been overridden,
			 * start counting from zero again (for the nested call only). */
			php_var_unserialize_set_cur_depth(var_hash, 0);
		}
	}

	if (BG(unserialize).level > 1) {
		retval = var_tmp_var(&var_hash);
	} else {
		retval = return_value;
	}
	if (!php_var_unserialize(retval, &p, p + buf_len, &var_hash)) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_NOTICE, "Error at offset " ZEND_LONG_FMT " of %zd bytes",
				(zend_long)((char*)p - buf), buf_len);
		}
		if (BG(unserialize).level <= 1) {
			zval_ptr_dtor(return_value);
		}
		RETVAL_FALSE;
	} else if (BG(unserialize).level > 1) {
		ZVAL_COPY(return_value, retval);
	} else if (Z_REFCOUNTED_P(return_value)) {
		zend_refcounted *ref = Z_COUNTED_P(return_value);
		gc_check_possible_root(ref);
	}

cleanup:
	if (class_hash) {
		zend_hash_destroy(class_hash);
		FREE_HASHTABLE(class_hash);
	}

	/* Reset to previous options in case this is a nested call */
	php_var_unserialize_set_allowed_classes(var_hash, prev_class_hash);
	php_var_unserialize_set_max_depth(var_hash, prev_max_depth);
	php_var_unserialize_set_cur_depth(var_hash, prev_cur_depth);
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);

	/* Per calling convention we must not return a reference here, so unwrap. We're doing this at
	 * the very end, because __wakeup() calls performed during UNSERIALIZE_DESTROY might affect
	 * the value we unwrap here. This is compatible with behavior in PHP <=7.0. */
	if (Z_ISREF_P(return_value)) {
		zend_unwrap_reference(return_value);
	}
}
/* }}} */

/* {{{ proto int memory_get_usage([bool real_usage])
   Returns the allocated by PHP memory */
PHP_FUNCTION(memory_get_usage) {
	zend_bool real_usage = 0;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(real_usage)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	RETURN_LONG(zend_memory_usage(real_usage));
}
/* }}} */

/* {{{ proto int memory_get_peak_usage([bool real_usage])
   Returns the peak allocated by PHP memory */
PHP_FUNCTION(memory_get_peak_usage) {
	zend_bool real_usage = 0;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(real_usage)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	RETURN_LONG(zend_memory_peak_usage(real_usage));
}
/* }}} */

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("unserialize_max_depth", "4096", PHP_INI_ALL, OnUpdateLong, unserialize_max_depth, php_basic_globals, basic_globals)
PHP_INI_END()

PHP_MINIT_FUNCTION(var)
{
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}
