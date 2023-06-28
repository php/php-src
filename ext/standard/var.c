/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Jani Lehtimäki <jkl@njet.net>                               |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |          Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
*/

/* {{{ includes */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "php.h"
#include "php_string.h"
#include "php_var.h"
#include "zend_smart_str.h"
#include "basic_functions.h"
#include "php_incomplete_class.h"
#include "zend_enum.h"
#include "zend_exceptions.h"
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
		ZEND_ASSERT(ZEND_TYPE_IS_SET(prop_info->type));
		zend_string *type_str = zend_type_to_string(prop_info->type);
		php_printf("%*cuninitialized(%s)\n",
			level + 1, ' ', ZSTR_VAL(type_str));
		zend_string_release(type_str);
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
			php_printf_unchecked("%sfloat(%.*H)\n", COMMON, (int) PG(serialize_precision), Z_DVAL_P(struc));
			break;
		case IS_STRING:
			php_printf("%sstring(%zd) \"", COMMON, Z_STRLEN_P(struc));
			PHPWRITE(Z_STRVAL_P(struc), Z_STRLEN_P(struc));
			PUTS("\"\n");
			break;
		case IS_ARRAY:
			myht = Z_ARRVAL_P(struc);
			if (!(GC_FLAGS(myht) & GC_IMMUTABLE)) {
				if (GC_IS_RECURSIVE(myht)) {
					PUTS("*RECURSION*\n");
					return;
				}
				GC_ADDREF(myht);
				GC_PROTECT_RECURSION(myht);
			}
			count = zend_hash_num_elements(myht);
			php_printf("%sarray(%d) {\n", COMMON, count);
			ZEND_HASH_FOREACH_KEY_VAL(myht, num, key, val) {
				php_array_element_dump(val, num, key, level);
			} ZEND_HASH_FOREACH_END();
			if (!(GC_FLAGS(myht) & GC_IMMUTABLE)) {
				GC_UNPROTECT_RECURSION(myht);
				GC_DELREF(myht);
			}
			if (level > 1) {
				php_printf("%*c", level-1, ' ');
			}
			PUTS("}\n");
			break;
		case IS_OBJECT: {
			zend_class_entry *ce = Z_OBJCE_P(struc);
			if (ce->ce_flags & ZEND_ACC_ENUM) {
				zval *case_name_zval = zend_enum_fetch_case_name(Z_OBJ_P(struc));
				php_printf("%senum(%s::%s)\n", COMMON, ZSTR_VAL(ce->name), Z_STRVAL_P(case_name_zval));
				return;
			}

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
		}
		case IS_RESOURCE: {
			const char *type_name = zend_rsrc_list_get_rsrc_type(Z_RES_P(struc));
			php_printf("%sresource(" ZEND_LONG_FMT ") of type (%s)\n", COMMON, Z_RES_P(struc)->handle, type_name ? type_name : "Unknown");
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

/* {{{ Dumps a string representation of variable to output */
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
		zend_string *type_str = zend_type_to_string(prop_info->type);
		php_printf("%*cuninitialized(%s)\n",
			level + 1, ' ', ZSTR_VAL(type_str));
		zend_string_release(type_str);
	} else {
		php_debug_zval_dump(zv, level + 2);
	}
}
/* }}} */

PHPAPI void php_debug_zval_dump(zval *struc, int level) /* {{{ */
{
	HashTable *myht = NULL;
	zend_string *class_name;
	zend_ulong index;
	zend_string *key;
	zval *val;
	uint32_t count;

	if (level > 1) {
		php_printf("%*c", level - 1, ' ');
	}

	switch (Z_TYPE_P(struc)) {
	case IS_FALSE:
		PUTS("bool(false)\n");
		break;
	case IS_TRUE:
		PUTS("bool(true)\n");
		break;
	case IS_NULL:
		PUTS("NULL\n");
		break;
	case IS_LONG:
		php_printf("int(" ZEND_LONG_FMT ")\n", Z_LVAL_P(struc));
		break;
	case IS_DOUBLE:
		php_printf_unchecked("float(%.*H)\n", (int) PG(serialize_precision), Z_DVAL_P(struc));
		break;
	case IS_STRING:
		php_printf("string(%zd) \"", Z_STRLEN_P(struc));
		PHPWRITE(Z_STRVAL_P(struc), Z_STRLEN_P(struc));
		if (Z_REFCOUNTED_P(struc)) {
			php_printf("\" refcount(%u)\n", Z_REFCOUNT_P(struc));
		} else {
			PUTS("\" interned\n");
		}
		break;
	case IS_ARRAY:
		myht = Z_ARRVAL_P(struc);
		if (!(GC_FLAGS(myht) & GC_IMMUTABLE)) {
			if (GC_IS_RECURSIVE(myht)) {
				PUTS("*RECURSION*\n");
				return;
			}
			GC_ADDREF(myht);
			GC_PROTECT_RECURSION(myht);
		}
		count = zend_hash_num_elements(myht);
		if (Z_REFCOUNTED_P(struc)) {
			/* -1 because of ADDREF above. */
			php_printf("array(%d) refcount(%u){\n", count, Z_REFCOUNT_P(struc) - 1);
		} else {
			php_printf("array(%d) interned {\n", count);
		}
		ZEND_HASH_FOREACH_KEY_VAL(myht, index, key, val) {
			zval_array_element_dump(val, index, key, level);
		} ZEND_HASH_FOREACH_END();
		if (!(GC_FLAGS(myht) & GC_IMMUTABLE)) {
			GC_UNPROTECT_RECURSION(myht);
			GC_DELREF(myht);
		}
		if (level > 1) {
			php_printf("%*c", level - 1, ' ');
		}
		PUTS("}\n");
		break;
	case IS_OBJECT:
		/* Check if this is already recursing on the object before calling zend_get_properties_for,
		 * to allow infinite recursion detection to work even if classes return temporary arrays,
		 * and to avoid the need to update the properties table in place to reflect the state
		 * if the result won't be used. (https://github.com/php/php-src/issues/8044) */
		if (Z_IS_RECURSIVE_P(struc)) {
			PUTS("*RECURSION*\n");
			return;
		}
		Z_PROTECT_RECURSION_P(struc);

		myht = zend_get_properties_for(struc, ZEND_PROP_PURPOSE_DEBUG);
		class_name = Z_OBJ_HANDLER_P(struc, get_class_name)(Z_OBJ_P(struc));
		php_printf("object(%s)#%d (%d) refcount(%u){\n", ZSTR_VAL(class_name), Z_OBJ_HANDLE_P(struc), myht ? zend_array_count(myht) : 0, Z_REFCOUNT_P(struc));
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
			zend_release_properties(myht);
		}
		if (level > 1) {
			php_printf("%*c", level - 1, ' ');
		}
		PUTS("}\n");
		Z_UNPROTECT_RECURSION_P(struc);
		break;
	case IS_RESOURCE: {
		const char *type_name = zend_rsrc_list_get_rsrc_type(Z_RES_P(struc));
		php_printf("resource(" ZEND_LONG_FMT ") of type (%s) refcount(%u)\n", Z_RES_P(struc)->handle, type_name ? type_name : "Unknown", Z_REFCOUNT_P(struc));
		break;
	}
	case IS_REFERENCE:
		php_printf("reference refcount(%u) {\n", Z_REFCOUNT_P(struc));
		php_debug_zval_dump(Z_REFVAL_P(struc), level + 2);
		if (level > 1) {
			php_printf("%*c", level - 1, ' ');
		}
		PUTS("}\n");
		break;
	default:
		PUTS("UNKNOWN:0\n");
		break;
	}
}
/* }}} */

/* {{{ Dumps a string representation of an internal zend value to output. */
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
			smart_str_append_double(
				buf, Z_DVAL_P(struc), (int) PG(serialize_precision), /* zero_fraction */ true);
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
			ZEND_HASH_FOREACH_KEY_VAL(myht, index, key, val) {
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
			/* Check if this is already recursing on the object before calling zend_get_properties_for,
			 * to allow infinite recursion detection to work even if classes return temporary arrays,
			 * and to avoid the need to update the properties table in place to reflect the state
			 * if the result won't be used. (https://github.com/php/php-src/issues/8044) */
			if (Z_IS_RECURSIVE_P(struc)) {
				smart_str_appendl(buf, "NULL", 4);
				zend_error(E_WARNING, "var_export does not handle circular references");
				return;
			}
			Z_PROTECT_RECURSION_P(struc);
			myht = zend_get_properties_for(struc, ZEND_PROP_PURPOSE_VAR_EXPORT);
			if (level > 1) {
				smart_str_appendc(buf, '\n');
				buffer_append_spaces(buf, level - 1);
			}

			zend_class_entry *ce = Z_OBJCE_P(struc);
			bool is_enum = ce->ce_flags & ZEND_ACC_ENUM;

			/* stdClass has no __set_state method, but can be casted to */
			if (ce == zend_standard_class_def) {
				smart_str_appendl(buf, "(object) array(\n", 16);
			} else {
				smart_str_appendc(buf, '\\');
				smart_str_append(buf, ce->name);
				if (is_enum) {
					zend_object *zobj = Z_OBJ_P(struc);
					zval *case_name_zval = zend_enum_fetch_case_name(zobj);
					smart_str_appendl(buf, "::", 2);
					smart_str_append(buf, Z_STR_P(case_name_zval));
				} else {
					smart_str_appendl(buf, "::__set_state(array(\n", 21);
				}
			}

			if (myht) {
				if (!is_enum) {
					ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, val) {
						php_object_element_export(val, index, key, level, buf);
					} ZEND_HASH_FOREACH_END();
				}
				zend_release_properties(myht);
			}
			Z_UNPROTECT_RECURSION_P(struc);
			if (level > 1 && !is_enum) {
				buffer_append_spaces(buf, level - 1);
			}
			if (ce == zend_standard_class_def) {
				smart_str_appendc(buf, ')');
			} else if (!is_enum) {
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

/* {{{ Outputs or returns a string representation of a variable */
PHP_FUNCTION(var_export)
{
	zval *var;
	bool return_output = 0;
	smart_str buf = {0};

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(var)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(return_output)
	ZEND_PARSE_PARAMETERS_END();

	php_var_export_ex(var, 1, &buf);
	smart_str_0 (&buf);

	if (return_output) {
		RETURN_STR(smart_str_extract(&buf));
	} else {
		PHPWRITE(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
		smart_str_free(&buf);
	}
}
/* }}} */

static void php_var_serialize_intern(smart_str *buf, zval *struc, php_serialize_data_t var_hash, bool in_rcn_array, bool is_root);

/**
 * @param bool in_rcn_array Whether the element appears in a potentially nested array with RC > 1.
 */
static inline zend_long php_add_var_hash(php_serialize_data_t data, zval *var, bool in_rcn_array) /* {{{ */
{
	zval *zv;
	zend_ulong key;
	bool is_ref = Z_ISREF_P(var);

	data->n += 1;

	if (is_ref) {
		/* pass */
	} else if (Z_TYPE_P(var) != IS_OBJECT) {
		return 0;
	} else if (!in_rcn_array
	 && Z_REFCOUNT_P(var) == 1
	 && (Z_OBJ_P(var)->properties == NULL || GC_REFCOUNT(Z_OBJ_P(var)->properties) == 1)) {
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
	char b[32];
	char *s = zend_print_long_to_buf(b + sizeof(b) - 1, val);
	size_t l = b + sizeof(b) - 1 - s;
	char *res = smart_str_extend(buf, 2 + l + 1);
	memcpy(res, "i:", 2);
	res += 2;
	memcpy(res, s, l);
	res[l] = ';';
}
/* }}} */

static inline void php_var_serialize_string(smart_str *buf, char *str, size_t len) /* {{{ */
{
	char b[32];
	char *s = zend_print_long_to_buf(b + sizeof(b) - 1, len);
	size_t l = b + sizeof(b) - 1 - s;
	char *res = smart_str_extend(buf, 2 + l + 2 + len + 2);
	memcpy(res, "s:", 2);
	res += 2;
	memcpy(res, s, l);
	res += l;
	memcpy(res, ":\"", 2);
	res += 2;
	memcpy(res, str, len);
	res += len;
	memcpy(res, "\";", 2);
}
/* }}} */

static inline bool php_var_serialize_class_name(smart_str *buf, zval *struc) /* {{{ */
{
	char b[32];
	PHP_CLASS_ATTRIBUTES;

	PHP_SET_CLASS_ATTRIBUTES(struc);
	size_t class_name_len = ZSTR_LEN(class_name);
	char *s = zend_print_long_to_buf(b + sizeof(b) - 1, class_name_len);
	size_t l = b + sizeof(b) - 1 - s;
	char *res = smart_str_extend(buf, 2 + l + 2 + class_name_len + 2);
	memcpy(res, "O:", 2);
	res += 2;
	memcpy(res, s, l);
	res += l;
	memcpy(res, ":\"", 2);
	res += 2;
	memcpy(res, ZSTR_VAL(class_name), class_name_len);
	res += class_name_len;
	memcpy(res, "\":", 2);
	PHP_CLEANUP_CLASS_ATTRIBUTES();
	return incomplete_class;
}
/* }}} */

static HashTable* php_var_serialize_call_sleep(zend_object *obj, zend_function *fn) /* {{{ */
{
	zend_result res;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	zval retval;

	fci.size = sizeof(fci);
	fci.object = obj;
	fci.retval = &retval;
	fci.param_count = 0;
	fci.params = NULL;
	fci.named_params = NULL;
	ZVAL_UNDEF(&fci.function_name);

	fci_cache.function_handler = fn;
	fci_cache.object = obj;
	fci_cache.called_scope = obj->ce;

	BG(serialize_lock)++;
	res = zend_call_function(&fci, &fci_cache);
	BG(serialize_lock)--;

	if (res == FAILURE || Z_ISUNDEF(retval)) {
		zval_ptr_dtor(&retval);
		return NULL;
	}

	if (Z_TYPE(retval) != IS_ARRAY) {
		zval_ptr_dtor(&retval);
		php_error_docref(NULL, E_WARNING, "%s::__sleep() should return an array only containing the names of instance-variables to serialize", ZSTR_VAL(obj->ce->name));
		return NULL;
	}

	return Z_ARRVAL(retval);
}
/* }}} */

static int php_var_serialize_call_magic_serialize(zval *retval, zval *obj) /* {{{ */
{
	BG(serialize_lock)++;
	zend_call_known_instance_method_with_0_params(
		Z_OBJCE_P(obj)->__serialize, Z_OBJ_P(obj), retval);
	BG(serialize_lock)--;

	if (EG(exception)) {
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
			"\"%s\" is returned from __sleep() multiple times", ZSTR_VAL(error_name));
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
			php_error_docref(NULL, E_WARNING,
					"%s::__sleep() should return an array only containing the names of instance-variables to serialize",
					ZSTR_VAL(ce->name));
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

		php_error_docref(NULL, E_WARNING,
			"\"%s\" returned as member variable from __sleep() but does not exist", ZSTR_VAL(name));
		zend_tmp_string_release(tmp_name);
	} ZEND_HASH_FOREACH_END();

	zend_release_properties(props);
	return retval;
}
/* }}} */

static void php_var_serialize_nested_data(smart_str *buf, zval *struc, HashTable *ht, uint32_t count, bool incomplete_class, php_serialize_data_t var_hash, bool in_rcn_array) /* {{{ */
{
	smart_str_append_unsigned(buf, count);
	smart_str_appendl(buf, ":{", 2);
	if (count > 0) {
		zend_string *key;
		zval *data;
		zend_ulong index;

		ZEND_HASH_FOREACH_KEY_VAL_IND(ht, index, key, data) {
			if (incomplete_class && zend_string_equals_literal(key, MAGIC_MEMBER)) {
				incomplete_class = 0;
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
					php_add_var_hash(var_hash, struc, in_rcn_array);
					smart_str_appendl(buf, "N;", 2);
				} else {
					if (Z_REFCOUNTED_P(data)) {
						Z_PROTECT_RECURSION_P(data);
					}
					php_var_serialize_intern(buf, data, var_hash, in_rcn_array, false);
					if (Z_REFCOUNTED_P(data)) {
						Z_UNPROTECT_RECURSION_P(data);
					}
				}
			} else {
				php_var_serialize_intern(buf, data, var_hash, in_rcn_array, false);
			}
		} ZEND_HASH_FOREACH_END();
	}
	smart_str_appendc(buf, '}');
}
/* }}} */

static void php_var_serialize_class(smart_str *buf, zval *struc, HashTable *ht, php_serialize_data_t var_hash) /* {{{ */
{
	HashTable props;

	if (php_var_serialize_get_sleep_props(&props, struc, ht) == SUCCESS) {
		php_var_serialize_class_name(buf, struc);
		php_var_serialize_nested_data(
			buf, struc, &props, zend_hash_num_elements(&props), /* incomplete_class */ 0, var_hash, GC_REFCOUNT(&props) > 1);
	}
	zend_hash_destroy(&props);
}
/* }}} */

static void php_var_serialize_intern(smart_str *buf, zval *struc, php_serialize_data_t var_hash, bool in_rcn_array, bool is_root) /* {{{ */
{
	zend_long var_already;
	HashTable *myht;

	if (EG(exception)) {
		return;
	}

	if (var_hash && (var_already = php_add_var_hash(var_hash, struc, in_rcn_array))) {
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
			char tmp_str[ZEND_DOUBLE_MAX_LENGTH];
			zend_gcvt(Z_DVAL_P(struc), (int)PG(serialize_precision), '.', 'E', tmp_str);

			size_t len = strlen(tmp_str);
			char *res = smart_str_extend(buf, 2 + len + 1);
			memcpy(res, "d:", 2);
			res += 2;
			memcpy(res, tmp_str, len);
			res[len] = ';';
			return;
		}

		case IS_STRING:
			php_var_serialize_string(buf, Z_STRVAL_P(struc), Z_STRLEN_P(struc));
			return;

		case IS_OBJECT: {
				zend_class_entry *ce = Z_OBJCE_P(struc);
				bool incomplete_class;
				uint32_t count;

				if (ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE) {
					zend_throw_exception_ex(NULL, 0, "Serialization of '%s' is not allowed",
						ZSTR_VAL(ce->name));
					return;
				}

				if (ce->ce_flags & ZEND_ACC_ENUM) {
					PHP_CLASS_ATTRIBUTES;

					zval *case_name_zval = zend_enum_fetch_case_name(Z_OBJ_P(struc));

					PHP_SET_CLASS_ATTRIBUTES(struc);
					smart_str_appendl(buf, "E:", 2);
					smart_str_append_unsigned(buf, ZSTR_LEN(class_name) + strlen(":") + Z_STRLEN_P(case_name_zval));
					smart_str_appendl(buf, ":\"", 2);
					smart_str_append(buf, class_name);
					smart_str_appendc(buf, ':');
					smart_str_append(buf, Z_STR_P(case_name_zval));
					smart_str_appendl(buf, "\";", 2);
					PHP_CLEANUP_CLASS_ATTRIBUTES();
					return;
				}

				if (ce->__serialize) {
					zval retval, obj;
					zend_string *key;
					zval *data;
					zend_ulong index;

					ZVAL_OBJ_COPY(&obj, Z_OBJ_P(struc));
					if (php_var_serialize_call_magic_serialize(&retval, &obj) == FAILURE) {
						if (!EG(exception)) {
							smart_str_appendl(buf, "N;", 2);
						}
						zval_ptr_dtor(&obj);
						return;
					}

					php_var_serialize_class_name(buf, &obj);
					smart_str_append_unsigned(buf, zend_hash_num_elements(Z_ARRVAL(retval)));
					smart_str_appendl(buf, ":{", 2);
					ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(retval), index, key, data) {
						if (!key) {
							php_var_serialize_long(buf, index);
						} else {
							php_var_serialize_string(buf, ZSTR_VAL(key), ZSTR_LEN(key));
						}

						if (Z_ISREF_P(data) && Z_REFCOUNT_P(data) == 1) {
							data = Z_REFVAL_P(data);
						}
						php_var_serialize_intern(buf, data, var_hash, Z_REFCOUNT(retval) > 1, false);
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
						char b1[32], b2[32];
						char *s1 = zend_print_long_to_buf(b1 + sizeof(b1) - 1, ZSTR_LEN(Z_OBJCE_P(struc)->name));
						size_t l1 = b1 + sizeof(b1) - 1 - s1;
						char *s2 = zend_print_long_to_buf(b2 + sizeof(b2) - 1, serialized_length);
						size_t l2 = b2 + sizeof(b2) - 1 - s2;
						char *res = smart_str_extend(buf, 2 + l1 + 2 + ZSTR_LEN(Z_OBJCE_P(struc)->name) + 2 + l2 + 2 + serialized_length + 1);
						memcpy(res, "C:", 2);
						res += 2;
						memcpy(res, s1, l1);
						res += l1;
						memcpy(res, ":\"", 2);
						res += 2;
						memcpy(res, ZSTR_VAL(Z_OBJCE_P(struc)->name), ZSTR_LEN(Z_OBJCE_P(struc)->name));
						res += ZSTR_LEN(Z_OBJCE_P(struc)->name);
						memcpy(res, "\":", 2);
						res += 2;

						memcpy(res, s2, l2);
						res += l2;
						memcpy(res, ":{", 2);
						res += 2;
						memcpy(res, (char *) serialized_data, serialized_length);
						res[serialized_length] = '}';
					} else {
						/* Mark this value in the var_hash, to avoid creating references to it. */
						zval *var_idx = zend_hash_index_find(&var_hash->ht,
							(zend_ulong) (zend_uintptr_t) Z_COUNTED_P(struc));
						if (var_idx) {
							ZVAL_LONG(var_idx, -1);
						}
						smart_str_appendl(buf, "N;", 2);
					}
					if (serialized_data) {
						efree(serialized_data);
					}
					return;
				}

				if (ce != PHP_IC_ENTRY) {
					zval *zv = zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_SLEEP));

					if (zv) {
						HashTable *ht;
						zval tmp;

						ZVAL_OBJ_COPY(&tmp, Z_OBJ_P(struc));
						if (!(ht = php_var_serialize_call_sleep(Z_OBJ(tmp), Z_FUNC_P(zv)))) {
							if (!EG(exception)) {
								/* we should still add element even if it's not OK,
								 * since we already wrote the length of the array before */
								smart_str_appendl(buf, "N;", 2);
							}
							OBJ_RELEASE(Z_OBJ(tmp));
							return;
						}

						php_var_serialize_class(buf, &tmp, ht, var_hash);
						zend_array_release(ht);
						OBJ_RELEASE(Z_OBJ(tmp));
						return;
					}
				}

				incomplete_class = php_var_serialize_class_name(buf, struc);

				if (Z_OBJ_P(struc)->properties == NULL
				 && Z_OBJ_HT_P(struc)->get_properties_for == NULL
				 && Z_OBJ_HT_P(struc)->get_properties == zend_std_get_properties) {
					/* Optimized version without rebulding properties HashTable */
					zend_object *obj = Z_OBJ_P(struc);
					zend_class_entry *ce = obj->ce;
					zend_property_info *prop_info;
					zval *prop;
					int i;

					count = ce->default_properties_count;
					for (i = 0; i < ce->default_properties_count; i++) {
						prop_info = ce->properties_info_table[i];
						if (!prop_info) {
							count--;
							continue;
						}
						prop = OBJ_PROP(obj, prop_info->offset);
						if (Z_TYPE_P(prop) == IS_UNDEF) {
							count--;
							continue;
						}
					}
					if (count) {
						smart_str_append_unsigned(buf, count);
						smart_str_appendl(buf, ":{", 2);
						for (i = 0; i < ce->default_properties_count; i++) {
							prop_info = ce->properties_info_table[i];
							if (!prop_info) {
								continue;
							}
							prop = OBJ_PROP(obj, prop_info->offset);
							if (Z_TYPE_P(prop) == IS_UNDEF) {
								continue;
							}

							php_var_serialize_string(buf, ZSTR_VAL(prop_info->name), ZSTR_LEN(prop_info->name));

							if (Z_ISREF_P(prop) && Z_REFCOUNT_P(prop) == 1) {
								prop = Z_REFVAL_P(prop);
							}

							php_var_serialize_intern(buf, prop, var_hash, false, false);
						}
						smart_str_appendc(buf, '}');
					} else {
						smart_str_appendl(buf, "0:{}", 4);
					}
					return;
				}
				myht = zend_get_properties_for(struc, ZEND_PROP_PURPOSE_SERIALIZE);
				/* count after serializing name, since php_var_serialize_class_name
				 * changes the count if the variable is incomplete class */
				count = zend_array_count(myht);
				if (count > 0 && incomplete_class) {
					--count;
				}
				php_var_serialize_nested_data(buf, struc, myht, count, incomplete_class, var_hash, GC_REFCOUNT(myht) > 1);
				zend_release_properties(myht);
				return;
			}
		case IS_ARRAY:
			smart_str_appendl(buf, "a:", 2);
			myht = Z_ARRVAL_P(struc);
			php_var_serialize_nested_data(
				buf, struc, myht, zend_array_count(myht), /* incomplete_class */ 0, var_hash,
					!is_root && (in_rcn_array || GC_REFCOUNT(myht) > 1));
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
	php_var_serialize_intern(buf, struc, *data, false, true);
	smart_str_0(buf);
}
/* }}} */

PHPAPI php_serialize_data_t php_var_serialize_init(void) {
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

/* {{{ Returns a string representation of variable (which can later be unserialized) */
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
		RETURN_THROWS();
	}

	RETURN_STR(smart_str_extract(&buf));
}
/* }}} */

/* {{{ Takes a string representation of variable and recreates it, subject to the optional unserialize options HashTable */
PHPAPI void php_unserialize_with_options(zval *return_value, const char *buf, const size_t buf_len, HashTable *options, const char* function_name)
{
	const unsigned char *p;
	php_unserialize_data_t var_hash;
	zval *retval;
	HashTable *class_hash = NULL, *prev_class_hash;
	zend_long prev_max_depth, prev_cur_depth;

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

		classes = zend_hash_str_find_deref(options, "allowed_classes", sizeof("allowed_classes")-1);
		if (classes && Z_TYPE_P(classes) != IS_ARRAY && Z_TYPE_P(classes) != IS_TRUE && Z_TYPE_P(classes) != IS_FALSE) {
			zend_type_error("%s(): Option \"allowed_classes\" must be of type array|bool, %s given", function_name, zend_zval_type_name(classes));
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
				convert_to_string(entry);
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

		max_depth = zend_hash_str_find_deref(options, "max_depth", sizeof("max_depth") - 1);
		if (max_depth) {
			if (Z_TYPE_P(max_depth) != IS_LONG) {
				zend_type_error("%s(): Option \"max_depth\" must be of type int, %s given", function_name, zend_zval_type_name(max_depth));
				goto cleanup;
			}
			if (Z_LVAL_P(max_depth) < 0) {
				zend_value_error("%s(): Option \"max_depth\" must be greater than or equal to 0", function_name);
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

/* {{{ Takes a string representation of variable and recreates it */
PHP_FUNCTION(unserialize)
{
	char *buf = NULL;
	size_t buf_len;
	HashTable *options = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(buf, buf_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT(options)
	ZEND_PARSE_PARAMETERS_END();

	php_unserialize_with_options(return_value, buf, buf_len, options, "unserialize");
}
/* }}} */

/* {{{ Returns the allocated by PHP memory */
PHP_FUNCTION(memory_get_usage) {
	bool real_usage = 0;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(real_usage)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(zend_memory_usage(real_usage));
}
/* }}} */

/* {{{ Returns the peak allocated by PHP memory */
PHP_FUNCTION(memory_get_peak_usage) {
	bool real_usage = 0;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(real_usage)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(zend_memory_peak_usage(real_usage));
}
/* }}} */

/* {{{ Resets the peak PHP memory usage */
PHP_FUNCTION(memory_reset_peak_usage) {
	ZEND_PARSE_PARAMETERS_NONE();

	zend_memory_reset_peak_usage();
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
