/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
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

void php_var_dump(zval **struc, int level TSRMLS_DC)
{
	HashTable *myht;

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
		php_printf("%sarray(%d) {\n", COMMON, zend_hash_num_elements(myht));
		goto head_done;
	case IS_OBJECT:
		myht = Z_OBJPROP_PP(struc);
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


/* {{{ php_var_serialize */

static void php_var_serialize_intern(smart_str *buf, zval **struc, HashTable *var_hash TSRMLS_DC);

static inline int php_add_var_hash(HashTable *var_hash, zval *var, void *var_old)
{
	ulong var_no;
	char id[32], *p;

	p = smart_str_print_long(id, (long) var);
	*p = '\0';

	if (var_old && zend_hash_find(var_hash, id, p - id, var_old) == SUCCESS) {
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
	zend_hash_add(var_hash, id, p - id, &var_no, sizeof(var_no), NULL);
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
	int count = zend_hash_num_elements(HASH_OF(retval_ptr));

	php_var_serialize_class_name(buf, struc TSRMLS_CC);

	smart_str_append_long(buf, count);
	smart_str_appendl(buf, ":{", 2);

	if (count > 0) {
		char *key;
		zval **d, **name;
		ulong index;
		HashPosition pos;
		int i;

		zend_hash_internal_pointer_reset_ex(HASH_OF(retval_ptr), &pos);
	
		for (;; zend_hash_move_forward_ex(HASH_OF(retval_ptr), &pos)) {
			i = zend_hash_get_current_key_ex(HASH_OF(retval_ptr), &key, NULL, 
					&index, 0, &pos);
			
			if (i == HASH_KEY_NON_EXISTANT)
				break;

			zend_hash_get_current_data_ex(HASH_OF(retval_ptr), 
					(void **) &name, &pos);

			if (Z_TYPE_PP(name) != IS_STRING) {
				php_error(E_NOTICE, "__sleep should return an array only "
						"containing the names of instance-variables to "
						"serialize.");
				continue;
			}

			if (zend_hash_find(Z_OBJPROP_PP(struc), Z_STRVAL_PP(name), 
						Z_STRLEN_PP(name) + 1, (void *) &d) == SUCCESS) {
				php_var_serialize_string(buf, Z_STRVAL_PP(name), 
						Z_STRLEN_PP(name));
				php_var_serialize_intern(buf, d, var_hash TSRMLS_CC);	
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

				slen = sprintf(s, "d:%.*G;", (int) EG(precision), Z_DVAL_PP(struc));
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
					   	if (HASH_OF(retval_ptr))
							php_var_serialize_class(buf, struc, retval_ptr, 
									var_hash TSRMLS_CC);
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
			i = zend_hash_num_elements(myht);
			if (Z_TYPE_PP(struc) == IS_ARRAY) {
				smart_str_appendl(buf, "a:", 2);
			} else {
				php_var_serialize_class_name(buf, struc TSRMLS_CC);
			}
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
					
					if (zend_hash_get_current_data_ex(myht, 
								(void **) &data, &pos) != SUCCESS 
							|| !data 
							|| data == struc)
						continue;

					switch (i) {
					  case HASH_KEY_IS_LONG:
							php_var_serialize_long(buf, index);
							break;
						case HASH_KEY_IS_STRING:
							php_var_serialize_string(buf, key, key_len - 1);
							break;
					}
					php_var_serialize_intern(buf, data, var_hash TSRMLS_CC);
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
/* {{{ php_var_unserialize */

PHPAPI int php_var_unserialize(zval **rval, const char **p, const char *max, HashTable *var_hash TSRMLS_DC)
{
	const char *q;
	char *str;
	int i;
	char cur;
	ulong id;
	HashTable *myht;
	zval **rval_ref;

	if (var_hash && **p != 'R') {  /* references aren't counted by serializer! */
		zend_hash_next_index_insert(var_hash, rval, sizeof(*rval), NULL);
	}

	switch (cur = **p) {
	    case 'R':
			if (*((*p) + 1) != ':') {
				return 0;
			}
			q = *p;
			while (**p && **p != ';') {
				(*p)++;
			}
			if (**p != ';') {
				return 0;
			}
			(*p)++;
			id = atol(q + 2)-1; /* count starts with 1 */
			if (!var_hash) {
				return 0;
			}
			if (zend_hash_index_find(var_hash, id, (void *)&rval_ref) != SUCCESS) {
				return 0;
			}
			zval_ptr_dtor(rval);
			*rval = *rval_ref;
			(*rval)->refcount++;
			(*rval)->is_ref = 1;
			return 1;

		case 'N':
			if (*((*p) + 1) != ';') {
				return 0;
			}
			(*p)++;
			INIT_PZVAL(*rval);
			ZVAL_NULL(*rval);
			(*p)++;
			return 1;

		case 'b': /* bool */
		case 'i':
			if (*((*p) + 1) != ':') {
				return 0;
			}
			q = *p;
			while (**p && **p != ';') {
				(*p)++;
			}
			if (**p != ';') {
				return 0;
			}
			(*p)++;
			INIT_PZVAL(*rval);
			if (cur == 'b') {
				ZVAL_BOOL(*rval, atol(q + 2));
			} else {
				ZVAL_LONG(*rval, atol(q + 2));
			}
			return 1;

		case 'd':
			if (*((*p) + 1) != ':') {
				return 0;
			}
			q = *p;
			while (**p && **p != ';') {
				(*p)++;
			}
			if (**p != ';') {
				return 0;
			}
			(*p)++;
			INIT_PZVAL(*rval);
			ZVAL_DOUBLE(*rval, atof(q + 2));
			return 1;

		case 's':
			if (*((*p) + 1) != ':') {
				return 0;
			}
			(*p) += 2;
			q = *p;
			while (**p && **p != ':') {
				(*p)++;
			}
			if (**p != ':') {
				return 0;
			}
			i = atoi(q);
			if (i < 0 || (*p + 3 + i) > max || *((*p) + 1) != '\"' ||
				*((*p) + 2 + i) != '\"' || *((*p) + 3 + i) != ';') {
				return 0;
			}
			(*p) += 2;

			if (i == 0) {
			  	str = empty_string;
			} else  {
			  	str = estrndup(*p, i);
			}
			(*p) += i + 2;
			INIT_PZVAL(*rval);
			ZVAL_STRINGL(*rval, str, i, 0);
			return 1;

		case 'a':
		case 'o':
		case 'O': {
			zend_bool incomplete_class = 0;
			char *class_name = NULL;
			size_t name_len = 0;
			int pi;
			
			INIT_PZVAL(*rval);

			if (cur == 'a') {
				Z_TYPE_PP(rval) = IS_ARRAY;
				ALLOC_HASHTABLE(Z_ARRVAL_PP(rval));
				myht = Z_ARRVAL_PP(rval);
			} else {
				zend_class_entry *ce;

				if (cur == 'O') { /* php4 serialized - we get the class-name */
					if (*((*p) + 1) != ':') {
						return 0;
					}
					(*p) += 2;
					q = *p;
					while (**p && **p != ':') {
						(*p)++;
					}
					if (**p != ':') {
						return 0;
					}
					name_len = i = atoi(q);
					if (i < 0 || (*p + 3 + i) > max || *((*p) + 1) != '\"' ||
						*((*p) + 2 + i) != '\"' || *((*p) + 3 + i) != ':') {
						return 0;
					}
					(*p) += 2;
					class_name = emalloc(i + 1);
					for(pi=0;pi<i;pi++) {
						class_name[pi] = tolower((*p)[pi]);
					}
					class_name[i] = 0;
					(*p) += i;
					
					if (zend_hash_find(EG(class_table), class_name, i+1, (void **) &ce)==FAILURE) {
						incomplete_class = 1;
						ce = PHP_IC_ENTRY;
					}
				} else { /* old php 3.0 data 'o' */
					ce = &zend_standard_class_def;
				}

				object_init_ex(*rval, ce);
				myht = Z_OBJPROP_PP(rval);

				if (incomplete_class)
					php_store_class_name(*rval, class_name, name_len);

				if (class_name)
					efree(class_name);
			}

			(*p) += 2;
			i = atoi(*p);

			if (cur == 'a') { /* object_init_ex will init the HashTable for objects! */
				zend_hash_init(myht, i + 1, NULL, ZVAL_PTR_DTOR, 0);
			}

			while (**p && **p != ':') {
				(*p)++;
			}
			if (**p != ':' || *((*p) + 1) != '{') {
				return 0;
			}
			for ((*p) += 2; **p && **p != '}' && i > 0; i--) {
				zval *key;
				zval *data;
				
				ALLOC_INIT_ZVAL(key);
				ALLOC_INIT_ZVAL(data);

				if (!php_var_unserialize(&key, p, max, NULL TSRMLS_CC)) {
					zval_dtor(key);
					FREE_ZVAL(key);
					FREE_ZVAL(data);
					return 0;
				}
				if (!php_var_unserialize(&data, p, max, var_hash TSRMLS_CC)) {
					zval_dtor(key);
					FREE_ZVAL(key);
					zval_dtor(data);
					FREE_ZVAL(data);
					return 0;
				}
				switch (Z_TYPE_P(key)) {
					case IS_LONG:
						zend_hash_index_update(myht, Z_LVAL_P(key), &data, sizeof(data), NULL);
						break;
					case IS_STRING:
						zend_hash_update(myht, Z_STRVAL_P(key), Z_STRLEN_P(key) + 1, &data, sizeof(data), NULL);
						break;
				}
				zval_dtor(key);
				FREE_ZVAL(key);
			}

			if (Z_TYPE_PP(rval) == IS_OBJECT) {
				zval *retval_ptr = NULL;
				zval fname;

				INIT_PZVAL(&fname);
				ZVAL_STRINGL(&fname, "__wakeup", sizeof("__wakeup") - 1, 0);
				call_user_function_ex(CG(function_table), rval, &fname, &retval_ptr, 0, 0, 1, NULL TSRMLS_CC);

				if (retval_ptr)
					zval_ptr_dtor(&retval_ptr);
			}

			return *((*p)++) == '}';
		  }
	}

	return 0;
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
	RETVAL_STRINGL(buf.c, buf.len, 0);
}

/* }}} */
/* {{{ proto mixed unserialize(string variable_representation)
   Takes a string representation of variable and recreates it */


PHP_FUNCTION(unserialize)
{
	zval **buf;
	php_serialize_data_t var_hash;
	
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
			php_error(E_NOTICE, "unserialize() failed at offset %d of %d bytes", p - Z_STRVAL_PP(buf), Z_STRLEN_PP(buf));
			RETURN_FALSE;
		}
		PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	} else {
		php_error(E_NOTICE, "argument passed to unserialize() is not an string");
		RETURN_FALSE;
	}
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
