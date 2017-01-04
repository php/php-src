/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
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
#include "zend_smart_str.h"
#include "basic_functions.h"
#include "php_incomplete_class.h"

#define COMMON (is_ref ? "&" : "")
/* }}} */

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

static void php_object_property_dump(zval *zv, zend_ulong index, zend_string *key, int level) /* {{{ */
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
	php_var_dump(zv, level + 2);
}
/* }}} */

PHPAPI void php_var_dump(zval *struc, int level) /* {{{ */
{
	HashTable *myht;
	zend_string *class_name;
	int is_temp;
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
			if (level > 1 && ZEND_HASH_APPLY_PROTECTION(myht) && ++myht->u.v.nApplyCount > 1) {
				PUTS("*RECURSION*\n");
				--myht->u.v.nApplyCount;
				return;
			}
			count = zend_array_count(myht);
			php_printf("%sarray(%d) {\n", COMMON, count);
			is_temp = 0;

			ZEND_HASH_FOREACH_KEY_VAL_IND(myht, num, key, val) {
				php_array_element_dump(val, num, key, level);
			} ZEND_HASH_FOREACH_END();
			if (level > 1 && ZEND_HASH_APPLY_PROTECTION(myht)) {
				--myht->u.v.nApplyCount;
			}
			if (is_temp) {
				zend_hash_destroy(myht);
				efree(myht);
			}
			if (level > 1) {
				php_printf("%*c", level-1, ' ');
			}
			PUTS("}\n");
			break;
		case IS_OBJECT:
			if (Z_OBJ_APPLY_COUNT_P(struc) > 0) {
				PUTS("*RECURSION*\n");
				return;
			}
			Z_OBJ_INC_APPLY_COUNT_P(struc);

			myht = Z_OBJDEBUG_P(struc, is_temp);
			class_name = Z_OBJ_HANDLER_P(struc, get_class_name)(Z_OBJ_P(struc));
			php_printf("%sobject(%s)#%d (%d) {\n", COMMON, ZSTR_VAL(class_name), Z_OBJ_HANDLE_P(struc), myht ? zend_array_count(myht) : 0);
			zend_string_release(class_name);

			if (myht) {
				zend_ulong num;
				zend_string *key;
				zval *val;

				ZEND_HASH_FOREACH_KEY_VAL_IND(myht, num, key, val) {
					php_object_property_dump(val, num, key, level);
				} ZEND_HASH_FOREACH_END();
				if (is_temp) {
					zend_hash_destroy(myht);
					efree(myht);
				}
			}
			if (level > 1) {
				php_printf("%*c", level-1, ' ');
			}
			PUTS("}\n");
			Z_OBJ_DEC_APPLY_COUNT_P(struc);
			break;
		case IS_RESOURCE: {
			const char *type_name = zend_rsrc_list_get_rsrc_type(Z_RES_P(struc));
			php_printf("%sresource(%pd) of type (%s)\n", COMMON, Z_RES_P(struc)->handle, type_name ? type_name : "Unknown");
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "+", &args, &argc) == FAILURE) {
		return;
	}

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

static void zval_object_property_dump(zval *zv, zend_ulong index, zend_string *key, int level) /* {{{ */
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
	php_debug_zval_dump(zv, level + 2);
}
/* }}} */

PHPAPI void php_debug_zval_dump(zval *struc, int level) /* {{{ */
{
	HashTable *myht = NULL;
	zend_string *class_name;
	int is_temp = 0;
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
		if (level > 1 && ZEND_HASH_APPLY_PROTECTION(myht) && myht->u.v.nApplyCount++ > 1) {
			myht->u.v.nApplyCount--;
			PUTS("*RECURSION*\n");
			return;
		}
		count = zend_array_count(myht);
		php_printf("%sarray(%d) refcount(%u){\n", COMMON, count, Z_REFCOUNTED_P(struc) ? Z_REFCOUNT_P(struc) : 1);
		ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, val) {
			zval_array_element_dump(val, index, key, level);
		} ZEND_HASH_FOREACH_END();
		if (level > 1 && ZEND_HASH_APPLY_PROTECTION(myht)) {
			myht->u.v.nApplyCount--;
		}
		if (is_temp) {
			zend_hash_destroy(myht);
			efree(myht);
		}
		if (level > 1) {
			php_printf("%*c", level - 1, ' ');
		}
		PUTS("}\n");
		break;
	case IS_OBJECT:
		myht = Z_OBJDEBUG_P(struc, is_temp);
		if (myht) {
			if (myht->u.v.nApplyCount > 1) {
				PUTS("*RECURSION*\n");
				return;
			} else {
				myht->u.v.nApplyCount++;
			}
		}
		class_name = Z_OBJ_HANDLER_P(struc, get_class_name)(Z_OBJ_P(struc));
		php_printf("%sobject(%s)#%d (%d) refcount(%u){\n", COMMON, ZSTR_VAL(class_name), Z_OBJ_HANDLE_P(struc), myht ? zend_array_count(myht) : 0, Z_REFCOUNT_P(struc));
		zend_string_release(class_name);
		if (myht) {
			ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, val) {
				zval_object_property_dump(val, index, key, level);
			} ZEND_HASH_FOREACH_END();
			myht->u.v.nApplyCount--;
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "+", &args, &argc) == FAILURE) {
		return;
	}

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
		zend_string *ckey = php_addcslashes(key, 0, "'\\", 2);
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
		pname_esc = php_addcslashes(zend_string_init(prop_name, prop_name_len, 0), 1, "'\\", 2);

		smart_str_appendc(buf, '\'');
		smart_str_append(buf, pname_esc);
		smart_str_appendc(buf, '\'');
		zend_string_release(pname_esc);
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
	char *tmp_str;
	size_t tmp_len;
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
			smart_str_append_long(buf, Z_LVAL_P(struc));
			break;
		case IS_DOUBLE:
			tmp_len = spprintf(&tmp_str, 0,"%.*H", PG(serialize_precision), Z_DVAL_P(struc));
			smart_str_appendl(buf, tmp_str, tmp_len);
			/* Without a decimal point, PHP treats a number literal as an int.
			 * This check even works for scientific notation, because the
			 * mantissa always contains a decimal point.
			 * We need to check for finiteness, because INF, -INF and NAN
			 * must not have a decimal point added.
			 */
			if (zend_finite(Z_DVAL_P(struc)) && NULL == strchr(tmp_str, '.')) {
				smart_str_appendl(buf, ".0", 2);
			}
			efree(tmp_str);
			break;
		case IS_STRING:
			ztmp = php_addcslashes(Z_STR_P(struc), 0, "'\\", 2);
			ztmp2 = php_str_to_str(ZSTR_VAL(ztmp), ZSTR_LEN(ztmp), "\0", 1, "' . \"\\0\" . '", 12);

			smart_str_appendc(buf, '\'');
			smart_str_append(buf, ztmp2);
			smart_str_appendc(buf, '\'');

			zend_string_free(ztmp);
			zend_string_free(ztmp2);
			break;
		case IS_ARRAY:
			myht = Z_ARRVAL_P(struc);
			if (ZEND_HASH_APPLY_PROTECTION(myht) && myht->u.v.nApplyCount++ > 0) {
				myht->u.v.nApplyCount--;
				smart_str_appendl(buf, "NULL", 4);
				zend_error(E_WARNING, "var_export does not handle circular references");
				return;
			}
			if (level > 1) {
				smart_str_appendc(buf, '\n');
				buffer_append_spaces(buf, level - 1);
			}
			smart_str_appendl(buf, "array (\n", 8);
			ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, val) {
				php_array_element_export(val, index, key, level, buf);
			} ZEND_HASH_FOREACH_END();
			if (ZEND_HASH_APPLY_PROTECTION(myht)) {
				myht->u.v.nApplyCount--;
			}
			if (level > 1) {
				buffer_append_spaces(buf, level - 1);
			}
			smart_str_appendc(buf, ')');

			break;

		case IS_OBJECT:
			myht = Z_OBJPROP_P(struc);
			if (myht) {
				if (myht->u.v.nApplyCount > 0) {
					smart_str_appendl(buf, "NULL", 4);
					zend_error(E_WARNING, "var_export does not handle circular references");
					return;
				} else {
					myht->u.v.nApplyCount++;
				}
			}
			if (level > 1) {
				smart_str_appendc(buf, '\n');
				buffer_append_spaces(buf, level - 1);
			}

			smart_str_append(buf, Z_OBJCE_P(struc)->name);
			smart_str_appendl(buf, "::__set_state(array(\n", 21);

			if (myht) {
				ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, val) {
					php_object_element_export(val, index, key, level, buf);
				} ZEND_HASH_FOREACH_END();
				myht->u.v.nApplyCount--;
			}
			if (level > 1) {
				buffer_append_spaces(buf, level - 1);
			}
			smart_str_appendl(buf, "))", 2);

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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &var, &return_output) == FAILURE) {
		return;
	}

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
		if (is_ref) {
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

static HashTable *php_var_serialize_collect_names(HashTable *src, uint32_t count, zend_bool incomplete) /* {{{ */ {
	zval *val;
	HashTable *ht;
	zend_string *key, *name;

	ALLOC_HASHTABLE(ht);
	zend_hash_init(ht, count, NULL, NULL, 0);
	ZEND_HASH_FOREACH_STR_KEY_VAL(src, key, val) {
		if (incomplete && strcmp(ZSTR_VAL(key), MAGIC_MEMBER) == 0) {
			continue;
		}
		if (Z_TYPE_P(val) != IS_STRING) {
			php_error_docref(NULL, E_NOTICE,
					"__sleep should return an array only containing the names of instance-variables to serialize.");
		}
		name = zval_get_string(val);
		if (zend_hash_exists(ht, name)) {
			php_error_docref(NULL, E_NOTICE,
					"\"%s\" is returned from __sleep multiple times", ZSTR_VAL(name));
			zend_string_release(name);
			continue;
		}
		zend_hash_add_empty_element(ht, name);
		zend_string_release(name);
	} ZEND_HASH_FOREACH_END();

	return ht;
}
/* }}} */

static void php_var_serialize_class(smart_str *buf, zval *struc, zval *retval_ptr, php_serialize_data_t var_hash) /* {{{ */
{
	uint32_t count;
	zend_bool incomplete_class;
	HashTable *ht;

	incomplete_class = php_var_serialize_class_name(buf, struc);
	/* count after serializing name, since php_var_serialize_class_name
	 * changes the count if the variable is incomplete class */
	if (Z_TYPE_P(retval_ptr) == IS_ARRAY) {
		ht = Z_ARRVAL_P(retval_ptr);
		count = zend_array_count(ht);
	} else if (Z_TYPE_P(retval_ptr) == IS_OBJECT) {
		ht = Z_OBJPROP_P(retval_ptr);
		count = zend_array_count(ht);
		if (incomplete_class) {
			--count;
		}
	} else {
		count = 0;
		ht = NULL;
	}

	if (count > 0) {
		zval *d;
		zval nval, *nvalp;
		zend_string *name;
		HashTable *names, *propers;

		names = php_var_serialize_collect_names(ht, count, incomplete_class);

		smart_str_append_unsigned(buf, zend_hash_num_elements(names));
		smart_str_appendl(buf, ":{", 2);

		ZVAL_NULL(&nval);
		nvalp = &nval;
		propers = Z_OBJPROP_P(struc);

		ZEND_HASH_FOREACH_STR_KEY(names, name) {
			if ((d = zend_hash_find(propers, name)) != NULL) {
				if (Z_TYPE_P(d) == IS_INDIRECT) {
					d = Z_INDIRECT_P(d);
					if (Z_TYPE_P(d) == IS_UNDEF) {
						continue;
					}
				}
				php_var_serialize_string(buf, ZSTR_VAL(name), ZSTR_LEN(name));
				php_var_serialize_intern(buf, d, var_hash);
			} else {
				zend_class_entry *ce = Z_OBJ_P(struc)->ce;
				if (ce) {
					zend_string *prot_name, *priv_name;

					do {
						priv_name = zend_mangle_property_name(
								ZSTR_VAL(ce->name), ZSTR_LEN(ce->name), ZSTR_VAL(name), ZSTR_LEN(name), ce->type & ZEND_INTERNAL_CLASS);
						if ((d = zend_hash_find(propers, priv_name)) != NULL) {
							if (Z_TYPE_P(d) == IS_INDIRECT) {
								d = Z_INDIRECT_P(d);
								if (Z_ISUNDEF_P(d)) {
									break;
								}
							}
							php_var_serialize_string(buf, ZSTR_VAL(priv_name), ZSTR_LEN(priv_name));
							zend_string_free(priv_name);
							php_var_serialize_intern(buf, d, var_hash);
							break;
						}
						zend_string_free(priv_name);
						prot_name = zend_mangle_property_name(
								"*", 1, ZSTR_VAL(name), ZSTR_LEN(name), ce->type & ZEND_INTERNAL_CLASS);
						if ((d = zend_hash_find(propers, prot_name)) != NULL) {
							if (Z_TYPE_P(d) == IS_INDIRECT) {
								d = Z_INDIRECT_P(d);
								if (Z_TYPE_P(d) == IS_UNDEF) {
									zend_string_free(prot_name);
									break;
								}
							}
							php_var_serialize_string(buf, ZSTR_VAL(prot_name), ZSTR_LEN(prot_name));
							zend_string_free(prot_name);
							php_var_serialize_intern(buf, d, var_hash);
							break;
						}
						zend_string_free(prot_name);
						php_var_serialize_string(buf, ZSTR_VAL(name), ZSTR_LEN(name));
						php_var_serialize_intern(buf, nvalp, var_hash);
						php_error_docref(NULL, E_NOTICE,
								"\"%s\" returned as member variable from __sleep() but does not exist", ZSTR_VAL(name));
					} while (0);
				} else {
					php_var_serialize_string(buf, ZSTR_VAL(name), ZSTR_LEN(name));
					php_var_serialize_intern(buf, nvalp, var_hash);
				}
			}
		} ZEND_HASH_FOREACH_END();
		smart_str_appendc(buf, '}');

		zend_hash_destroy(names);
		FREE_HASHTABLE(names);
	} else {
		smart_str_appendl(buf, "0:{}", 4);
	}
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
		if (Z_ISREF_P(struc)) {
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
				char *s;

				smart_str_appendl(buf, "d:", 2);
				s = (char *) safe_emalloc(PG(serialize_precision), 1, MAX_LENGTH_OF_DOUBLE + 1);
				php_gcvt(Z_DVAL_P(struc), (int)PG(serialize_precision), '.', 'E', s);
				smart_str_appends(buf, s);
				smart_str_appendc(buf, ';');
				efree(s);
				return;
			}

		case IS_STRING:
			php_var_serialize_string(buf, Z_STRVAL_P(struc), Z_STRLEN_P(struc));
			return;

		case IS_OBJECT: {
				zend_class_entry *ce = Z_OBJCE_P(struc);

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
						smart_str_appendl(buf, "N;", 2);
					}
					if (serialized_data) {
						efree(serialized_data);
					}
					return;
				}

				if (ce != PHP_IC_ENTRY && zend_hash_str_exists(&ce->function_table, "__sleep", sizeof("__sleep")-1)) {
					zval fname, tmp, retval;
					int res;

					ZVAL_COPY(&tmp, struc);
					ZVAL_STRINGL(&fname, "__sleep", sizeof("__sleep") - 1);
					BG(serialize_lock)++;
					res = call_user_function_ex(CG(function_table), &tmp, &fname, &retval, 0, 0, 1, NULL);
					BG(serialize_lock)--;
					zval_dtor(&fname);

					if (EG(exception)) {
						zval_ptr_dtor(&retval);
						zval_ptr_dtor(&tmp);
						return;
					}

					if (res == SUCCESS) {
						if (Z_TYPE(retval) != IS_UNDEF) {
							if (HASH_OF(&retval)) {
								php_var_serialize_class(buf, &tmp, &retval, var_hash);
							} else {
								php_error_docref(NULL, E_NOTICE, "__sleep should return an array only containing the names of instance-variables to serialize");
								/* we should still add element even if it's not OK,
								 * since we already wrote the length of the array before */
								smart_str_appendl(buf,"N;", 2);
							}
						}
						zval_ptr_dtor(&retval);
						zval_ptr_dtor(&tmp);
						return;
					}
					zval_ptr_dtor(&retval);
					zval_ptr_dtor(&tmp);
				}

				/* fall-through */
			}
		case IS_ARRAY: {
			uint32_t i;
			zend_bool incomplete_class = 0;
			if (Z_TYPE_P(struc) == IS_ARRAY) {
				smart_str_appendl(buf, "a:", 2);
				myht = Z_ARRVAL_P(struc);
				i = zend_array_count(myht);
			} else {
				incomplete_class = php_var_serialize_class_name(buf, struc);
				myht = Z_OBJPROP_P(struc);
				/* count after serializing name, since php_var_serialize_class_name
				 * changes the count if the variable is incomplete class */
				i = zend_array_count(myht);
				if (i > 0 && incomplete_class) {
					--i;
				}
			}
			smart_str_append_unsigned(buf, i);
			smart_str_appendl(buf, ":{", 2);
			if (i > 0) {
				zend_string *key;
				zval *data;
				zend_ulong index;

				ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, data) {

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
					if ((Z_TYPE_P(data) == IS_ARRAY && Z_TYPE_P(struc) == IS_ARRAY && Z_ARR_P(data) == Z_ARR_P(struc))
						|| (Z_TYPE_P(data) == IS_ARRAY && Z_ARRVAL_P(data)->u.v.nApplyCount > 1)
					) {
						smart_str_appendl(buf, "N;", 2);
					} else {
						if (Z_TYPE_P(data) == IS_ARRAY && ZEND_HASH_APPLY_PROTECTION(Z_ARRVAL_P(data))) {
							Z_ARRVAL_P(data)->u.v.nApplyCount++;
						}
						php_var_serialize_intern(buf, data, var_hash);
						if (Z_TYPE_P(data) == IS_ARRAY && ZEND_HASH_APPLY_PROTECTION(Z_ARRVAL_P(data))) {
							Z_ARRVAL_P(data)->u.v.nApplyCount--;
						}
					}
				} ZEND_HASH_FOREACH_END();
			}
			smart_str_appendc(buf, '}');
			return;
		}
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

/* {{{ proto string serialize(mixed variable)
   Returns a string representation of variable (which can later be unserialized) */
PHP_FUNCTION(serialize)
{
	zval *struc;
	php_serialize_data_t var_hash;
	smart_str buf = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &struc) == FAILURE) {
		return;
	}

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

/* {{{ proto mixed unserialize(string variable_representation[, array allowed_classes])
   Takes a string representation of variable and recreates it */
PHP_FUNCTION(unserialize)
{
	char *buf = NULL;
	size_t buf_len;
	const unsigned char *p;
	php_unserialize_data_t var_hash;
	zval *options = NULL, *classes = NULL;
	zval *retval;
	HashTable *class_hash = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|a", &buf, &buf_len, &options) == FAILURE) {
		RETURN_FALSE;
	}

	if (buf_len == 0) {
		RETURN_FALSE;
	}

	p = (const unsigned char*) buf;
	PHP_VAR_UNSERIALIZE_INIT(var_hash);
	if(options != NULL) {
		classes = zend_hash_str_find(Z_ARRVAL_P(options), "allowed_classes", sizeof("allowed_classes")-1);
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
		        zend_string_release(lcname);
			} ZEND_HASH_FOREACH_END();
		}
	}

	retval = var_tmp_var(&var_hash);
	if (!php_var_unserialize_ex(retval, &p, p + buf_len, &var_hash, class_hash)) {
		PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
		if (class_hash) {
			zend_hash_destroy(class_hash);
			FREE_HASHTABLE(class_hash);
		}
		if (!EG(exception)) {
			php_error_docref(NULL, E_NOTICE, "Error at offset " ZEND_LONG_FMT " of %zd bytes",
				(zend_long)((char*)p - buf), buf_len);
		}
		RETURN_FALSE;
	}

	ZVAL_COPY(return_value, retval);

	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	if (class_hash) {
		zend_hash_destroy(class_hash);
		FREE_HASHTABLE(class_hash);
	}
}
/* }}} */

/* {{{ proto int memory_get_usage([bool real_usage])
   Returns the allocated by PHP memory */
PHP_FUNCTION(memory_get_usage) {
	zend_bool real_usage = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &real_usage) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_LONG(zend_memory_usage(real_usage));
}
/* }}} */

/* {{{ proto int memory_get_peak_usage([bool real_usage])
   Returns the peak allocated by PHP memory */
PHP_FUNCTION(memory_get_peak_usage) {
	zend_bool real_usage = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &real_usage) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_LONG(zend_memory_peak_usage(real_usage));
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
