/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Jani Lehtimäki <jkl@njet.net>                               |
   |          Thies C. Arntzen <thies@thieso.net>                         |
   |          Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
*/

/* $Id: var.c
 1.111 2001/08/06 13:36:08 thies Exp $ */


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

#define COMMON ((*struc)->is_ref ? "&" : "")
#define Z_REFCOUNT_PP(a) ((*a)->refcount)

/* }}} */
/* {{{ php_var_dump */

static int php_array_element_dump(zval **zv, int num_args, va_list args, zend_hash_key *hash_key)
{
	int level;
	TSRMLS_FETCH();

	level = va_arg(args, int);

	if (hash_key->nKeyLength==0) { /* numeric key */
		php_printf("%*c[%ld]=>\n", level + 1, ' ', hash_key->h);
	} else { /* string key */
		php_printf("%*c[\"%s\"]=>\n", level + 1, ' ', hash_key->arKey);
	}
	php_var_dump(zv, level + 2 TSRMLS_CC);
	return 0;
}

PHPAPI void php_var_dump(zval **struc, int level TSRMLS_DC)
{
	HashTable *myht = NULL;
	zend_object *object = NULL;

	if (level > 1) {
		php_printf("%*c", level - 1, ' ');
	}

	switch (Z_TYPE_PP(struc)) {
	case IS_BOOL:
		php_printf("%sbool(%s)\n", COMMON, Z_LVAL_PP(struc)?"true":"false");
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
	case IS_ARRAY:
		myht = Z_ARRVAL_PP(struc);
		if (myht->nApplyCount > 1) {
			PUTS("*RECURSION*\n");
			return;
		}
		php_printf("%sarray(%d) {\n", COMMON, zend_hash_num_elements(myht));
		goto head_done;
	case IS_OBJECT:
		object = Z_OBJ_PP(struc);
		myht = Z_OBJPROP_PP(struc);
		if (myht->nApplyCount > 1) {
			PUTS("*RECURSION*\n");
			return;
		}
		php_printf("%sobject(%s)(%d) {\n", COMMON, Z_OBJCE_PP(struc)->name, zend_hash_num_elements(myht));
head_done:
		zend_hash_apply_with_arguments(myht, (apply_func_args_t) php_array_element_dump, 1, level);
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



/* {{{ proto void var_dump(mixed var)
   Dumps a string representation of variable to output */
PHP_FUNCTION(var_dump)
{
	zval ***args;
	int argc;
	int	i;
	
	argc = ZEND_NUM_ARGS();
	
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (ZEND_NUM_ARGS() == 0 || zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}
	
	for (i=0; i<argc; i++)
		php_var_dump(args[i], 1 TSRMLS_CC);
	
	efree(args);
}
/* }}} */

/* {{{ debug_zval_dump */

static int zval_array_element_dump(zval **zv, int num_args, va_list args, zend_hash_key *hash_key)
{
	int level;
	TSRMLS_FETCH();

	level = va_arg(args, int);

	if (hash_key->nKeyLength==0) { /* numeric key */
		php_printf("%*c[%ld]=>\n", level + 1, ' ', hash_key->h);
	} else { /* string key */
		php_printf("%*c[\"%s\"]=>\n", level + 1, ' ', hash_key->arKey);
	}
	php_debug_zval_dump(zv, level + 2 TSRMLS_CC);
	return 0;
}

PHPAPI void php_debug_zval_dump(zval **struc, int level TSRMLS_DC)
{
	HashTable *myht = NULL;

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
	case IS_ARRAY:
		myht = Z_ARRVAL_PP(struc);
		php_printf("%sarray(%d) refcount(%u){\n", COMMON, zend_hash_num_elements(myht), Z_REFCOUNT_PP(struc));
		goto head_done;
	case IS_OBJECT:
		myht = Z_OBJPROP_PP(struc);
		php_printf("%sobject(%s)(%d) refcount(%u){\n", COMMON, Z_OBJCE_PP(struc)->name, zend_hash_num_elements(myht), Z_REFCOUNT_PP(struc));
head_done:
		zend_hash_apply_with_arguments(myht, (apply_func_args_t) zval_array_element_dump, 1, level);
		if (level > 1) {
			php_printf("%*c", level-1, ' ');
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

/* {{{ proto void debug_zval_dump(mixed var)
   Dumps a string representation of an internal zend value to output. */
PHP_FUNCTION(debug_zval_dump)
{
	zval ***args;
	int argc;
	int	i;
	
	argc = ZEND_NUM_ARGS();
	
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (ZEND_NUM_ARGS() == 0 || zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}
	
	for (i=0; i<argc; i++)
		php_debug_zval_dump(args[i], 1 TSRMLS_CC);
	
	efree(args);
}
/* }}} */


/* {{{ php_var_export */

static int php_array_element_export(zval **zv, int num_args, va_list args, zend_hash_key *hash_key)
{
	int level;
	TSRMLS_FETCH();

	level = va_arg(args, int);

	if (hash_key->nKeyLength==0) { /* numeric key */
		php_printf("%*c%ld => ", level + 1, ' ', hash_key->h);
	} else { /* string key */
		php_printf("%*c'%s' => ", level + 1, ' ', hash_key->arKey);
	}
	php_var_export(zv, level + 2 TSRMLS_CC);
	PUTS (",\n");
	return 0;
}

static int php_object_element_export(zval **zv, int num_args, va_list args, zend_hash_key *hash_key)
{
	int level;
	TSRMLS_FETCH();

	level = va_arg(args, int);

	if (hash_key->nKeyLength != 0) {
		php_printf("%*cvar $%s = ", level + 1, ' ', hash_key->arKey);
		php_var_export(zv, level + 2 TSRMLS_CC);
		PUTS (";\n");
	}
	return 0;
}

PHPAPI void php_var_export(zval **struc, int level TSRMLS_DC)
{
	HashTable *myht;
	char*     tmp_str;
	int       tmp_len;

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
		php_printf("%.*G", (int) EG(precision), Z_DVAL_PP(struc));
		break;
	case IS_STRING:
		tmp_str = php_addcslashes(Z_STRVAL_PP(struc), Z_STRLEN_PP(struc), &tmp_len, 0, "'\\", 2 TSRMLS_CC);
		PUTS ("'");
		PHPWRITE(tmp_str, tmp_len);
		PUTS ("'");
		efree (tmp_str);
		break;
	case IS_ARRAY:
		myht = Z_ARRVAL_PP(struc);
		if (level > 1) {
			php_printf("\n%*c", level - 1, ' ');
		}
		PUTS ("array (\n");
		zend_hash_apply_with_arguments(myht, (apply_func_args_t) php_array_element_export, 1, level);
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
		php_printf ("class %s {\n", Z_OBJCE_PP(struc)->name);
		zend_hash_apply_with_arguments(myht, (apply_func_args_t) php_object_element_export, 1, level);
		if (level > 1) {
			php_printf("%*c", level - 1, ' ');
		}
		PUTS("}");
		break;
	default:
		PUTS ("NULL");
		break;
	}
}

/* }}} */


/* {{{ proto mixed var_export(mixed var [, bool return])
   Outputs or returns a string representation of a variable */
PHP_FUNCTION(var_export)
{
	zval *var;
	zend_bool return_output = 0;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &var, &return_output) == FAILURE) {
		return;
	}
	
	if (return_output) {
		php_start_ob_buffer (NULL, 0, 1 TSRMLS_CC);
	}
	
	php_var_export(&var, 1 TSRMLS_CC);

	if (return_output) {
		php_ob_get_buffer (return_value TSRMLS_CC);
		php_end_ob_buffer (0, 0 TSRMLS_CC);
	}
}
/* }}} */



/* {{{ php_var_serialize */

static void php_var_serialize_intern(smart_str *buf, zval **struc, HashTable *var_hash TSRMLS_DC);

static inline int php_add_var_hash(HashTable *var_hash, zval *var, void *var_old)
{
	ulong var_no;
	char id[32], *p;
	register int len;

	/* relies on "(long)" being a perfect hash function for data pointers */
	p = smart_str_print_long(id + sizeof(id) - 1, (long) var);
	len = id + sizeof(id) - 1 - p;
	
	if (var_old && zend_hash_find(var_hash, p, len, var_old) == SUCCESS) {
		if (!var->is_ref) {
			/* we still need to bump up the counter, since non-refs will
			   be counted separately by unserializer */
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

static inline void php_var_serialize_long(smart_str *buf, long val)
{
	smart_str_appendl(buf, "i:", 2);
	smart_str_append_long(buf, val);
	smart_str_appendc(buf, ';');
}

static inline void php_var_serialize_string(smart_str *buf, char *str, int len)
{
	smart_str_appendl(buf, "s:", 2);
	smart_str_append_long(buf, len);
	smart_str_appendl(buf, ":\"", 2);
	smart_str_appendl(buf, str, len);
	smart_str_appendl(buf, "\";", 2);
}

static inline void php_var_serialize_class_name(smart_str *buf, zval **struc TSRMLS_DC)
{
	PHP_CLASS_ATTRIBUTES;

	PHP_SET_CLASS_ATTRIBUTES(*struc);
	smart_str_appendl(buf, "O:", 2);
	smart_str_append_long(buf, name_len);
	smart_str_appendl(buf, ":\"", 2);
	smart_str_appendl(buf, class_name, name_len);
	smart_str_appendl(buf, "\":", 2);
	PHP_CLEANUP_CLASS_ATTRIBUTES();
}

static void php_var_serialize_class(smart_str *buf, zval **struc, zval *retval_ptr, HashTable *var_hash TSRMLS_DC)
{
	int count;

	php_var_serialize_class_name(buf, struc TSRMLS_CC);
	/* count after serializing name, since php_var_serialize_class_name
	   changes the count if the variable is incomplete class */
	count = zend_hash_num_elements(HASH_OF(retval_ptr));
	smart_str_append_long(buf, count);
	smart_str_appendl(buf, ":{", 2);

	if (count > 0) {
		char *key;
		zval **d, **name;
		ulong index;
		HashPosition pos;
		int i;
		zval nval, *nvalp;

		ZVAL_NULL(&nval);
		nvalp = &nval;

		zend_hash_internal_pointer_reset_ex(HASH_OF(retval_ptr), &pos);
	
		for (;; zend_hash_move_forward_ex(HASH_OF(retval_ptr), &pos)) {
			i = zend_hash_get_current_key_ex(HASH_OF(retval_ptr), &key, NULL, 
					&index, 0, &pos);
			
			if (i == HASH_KEY_NON_EXISTANT)
				break;

			zend_hash_get_current_data_ex(HASH_OF(retval_ptr), 
					(void **) &name, &pos);

			if (Z_TYPE_PP(name) != IS_STRING) {
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "__sleep should return an array only "
						"containing the names of instance-variables to "
						"serialize.");
				/* we should still add element even if it's not OK,
				   since we already wrote the length of the array before */
				smart_str_appendl(buf,"s:0:\"\";N;", 9);
				continue;
			}

			php_var_serialize_string(buf, Z_STRVAL_PP(name), Z_STRLEN_PP(name));

			if (zend_hash_find(Z_OBJPROP_PP(struc), Z_STRVAL_PP(name), 
						Z_STRLEN_PP(name) + 1, (void *) &d) == SUCCESS) {
				php_var_serialize_intern(buf, d, var_hash TSRMLS_CC);	
			} else {
				php_var_serialize_intern(buf, &nvalp, var_hash TSRMLS_CC);	
			}
		}
	}
	smart_str_appendc(buf, '}');
}


static void php_var_serialize_intern(smart_str *buf, zval **struc, HashTable *var_hash TSRMLS_DC)
{
	int i;
	ulong *var_already;
	HashTable *myht;

	if(var_hash 
			&& php_add_var_hash(var_hash, *struc, (void *) &var_already) == FAILURE 
			&& (*struc)->is_ref) {
		smart_str_appendl(buf, "R:", 2);
		smart_str_append_long(buf, *var_already);
		smart_str_appendc(buf, ';');
		return;
	}

	switch (Z_TYPE_PP(struc)) {
		case IS_BOOL:
			smart_str_appendl(buf, "b:", 2);
			smart_str_append_long(buf, Z_LVAL_PP(struc));
			smart_str_appendc(buf, ';');
			return;

		case IS_NULL:
			smart_str_appendl(buf, "N;", 2);
			return;

		case IS_LONG:
			php_var_serialize_long(buf, Z_LVAL_PP(struc));
			return;

		case IS_DOUBLE: {
				char s[256];
				ulong slen;

				slen = sprintf(s, "d:%.252g;", Z_DVAL_PP(struc));
				smart_str_appendl(buf, s, slen);
				return;
			}

		case IS_STRING:
			php_var_serialize_string(buf, Z_STRVAL_PP(struc), Z_STRLEN_PP(struc));
			return;

		case IS_OBJECT: {
				zval *retval_ptr = NULL;
				zval fname;
				int res;

				INIT_PZVAL(&fname);
				ZVAL_STRINGL(&fname, "__sleep", sizeof("__sleep") - 1, 0);
				res = call_user_function_ex(CG(function_table), struc, &fname, 
						&retval_ptr, 0, 0, 1, NULL TSRMLS_CC);

				if (res == SUCCESS) {
					if (retval_ptr) {
					   	if (HASH_OF(retval_ptr)) {
							php_var_serialize_class(buf, struc, retval_ptr, 
									var_hash TSRMLS_CC);
						} else {
							php_error_docref(NULL TSRMLS_CC, E_NOTICE, "__sleep should return an array only "
												"containing the names of instance-variables to "
												"serialize.");
						}

						zval_ptr_dtor(&retval_ptr);
					}
					return;	
				}
				if (retval_ptr)
					zval_ptr_dtor(&retval_ptr);
				/* fall-through */
			}
		case IS_ARRAY:
			myht = HASH_OF(*struc);
			if (Z_TYPE_PP(struc) == IS_ARRAY) {
				smart_str_appendl(buf, "a:", 2);
			} else {
				php_var_serialize_class_name(buf, struc TSRMLS_CC);
			}
			/* count after serializing name, since php_var_serialize_class_name
			   changes the count if the variable is incomplete class */
			i = zend_hash_num_elements(myht);
			smart_str_append_long(buf, i);
			smart_str_appendl(buf, ":{", 2);
			if (i > 0) {
				char *key;
				zval **data;
				ulong index;
				uint key_len;
				HashPosition pos;
				
				zend_hash_internal_pointer_reset_ex(myht, &pos);
				for (;; zend_hash_move_forward_ex(myht, &pos)) {
					i = zend_hash_get_current_key_ex(myht, &key, &key_len, 
							&index, 0, &pos);
					if (i == HASH_KEY_NON_EXISTANT)
						break;
					
					switch (i) {
					  case HASH_KEY_IS_LONG:
							php_var_serialize_long(buf, index);
							break;
						case HASH_KEY_IS_STRING:
							php_var_serialize_string(buf, key, key_len - 1);
							break;
					}

					/* we should still add element even if it's not OK,
					   since we already wrote the length of the array before */
					if (zend_hash_get_current_data_ex(myht, 
						(void **) &data, &pos) != SUCCESS 
						|| !data 
						|| data == struc) {
						smart_str_appendl(buf, "N;", 2);
					} else {
						php_var_serialize_intern(buf, data, var_hash TSRMLS_CC);
					}
				}
			}
			smart_str_appendc(buf, '}');
			return;
		default:
			smart_str_appendl(buf, "i:0;", 4);
			return;
	} 
}

PHPAPI void php_var_serialize(smart_str *buf, zval **struc, HashTable *var_hash TSRMLS_DC)
{
	php_var_serialize_intern(buf, struc, var_hash TSRMLS_CC);
	smart_str_0(buf);
}
	
/* }}} */

/* {{{ proto string serialize(mixed variable)
   Returns a string representation of variable (which can later be unserialized) */
PHP_FUNCTION(serialize)
{
	zval **struc;
	php_serialize_data_t var_hash;
	smart_str buf = {0};

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &struc) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	Z_TYPE_P(return_value) = IS_STRING;
	Z_STRVAL_P(return_value) = NULL;
	Z_STRLEN_P(return_value) = 0;

	PHP_VAR_SERIALIZE_INIT(var_hash);
	php_var_serialize(&buf, struc, &var_hash TSRMLS_CC);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	if (buf.c) {
		RETURN_STRINGL(buf.c, buf.len, 0);
	} else {
		RETURN_NULL();
	}
}

/* }}} */
/* {{{ proto mixed unserialize(string variable_representation)
   Takes a string representation of variable and recreates it */


PHP_FUNCTION(unserialize)
{
	zval **buf;
	php_unserialize_data_t var_hash;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &buf) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (Z_TYPE_PP(buf) == IS_STRING) {
		const char *p = Z_STRVAL_PP(buf);

		if (Z_STRLEN_PP(buf) == 0) {
			RETURN_FALSE;
		}

		PHP_VAR_UNSERIALIZE_INIT(var_hash);
		if (!php_var_unserialize(&return_value, &p, p + Z_STRLEN_PP(buf),  &var_hash TSRMLS_CC)) {
			PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
			zval_dtor(return_value);
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Error at offset %d of %d bytes", p - Z_STRVAL_PP(buf), Z_STRLEN_PP(buf));
			RETURN_FALSE;
		}
		PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Argument is not an string");
		RETURN_FALSE;
	}
}

/* }}} */

#if MEMORY_LIMIT
/* {{{ proto int memory_get_usage()
    Returns the allocated by PHP memory */
PHP_FUNCTION(memory_get_usage) {
	RETURN_LONG(AG(allocated_memory));
}
/* }}} */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
