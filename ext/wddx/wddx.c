/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andrei Zmievski <andrei@php.net>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_WDDX

#include "ext/xml/expat_compat.h"
#include "php_wddx.h"
#include "php_wddx_api.h"

#define PHP_XML_INTERNAL
#include "ext/xml/php_xml.h"
#include "ext/standard/php_incomplete_class.h"
#include "ext/standard/base64.h"
#include "ext/standard/info.h"
#include "zend_smart_str.h"
#include "ext/standard/html.h"
#include "ext/standard/php_string.h"
#include "ext/date/php_date.h"
#include "zend_globals.h"

#define WDDX_BUF_LEN			256
#define PHP_CLASS_NAME_VAR		"php_class_name"

#define EL_ARRAY				"array"
#define EL_BINARY				"binary"
#define EL_BOOLEAN				"boolean"
#define EL_CHAR					"char"
#define EL_CHAR_CODE			"code"
#define EL_NULL					"null"
#define EL_NUMBER				"number"
#define	EL_PACKET				"wddxPacket"
#define	EL_STRING				"string"
#define EL_STRUCT				"struct"
#define EL_VALUE				"value"
#define EL_VAR					"var"
#define EL_NAME	    			"name"
#define EL_VERSION				"version"
#define EL_RECORDSET			"recordset"
#define EL_FIELD				"field"
#define EL_DATETIME				"dateTime"

#define php_wddx_deserialize(a,b) \
	php_wddx_deserialize_ex((a)->value.str.val, (a)->value.str.len, (b))

#define SET_STACK_VARNAME							\
		if (stack->varname) {						\
			ent.varname = estrdup(stack->varname);	\
			efree(stack->varname);					\
			stack->varname = NULL;					\
		} else										\
			ent.varname = NULL;						\

static int le_wddx;

typedef struct {
	zval data;
	enum {
		ST_ARRAY,
		ST_BOOLEAN,
		ST_NULL,
		ST_NUMBER,
		ST_STRING,
		ST_BINARY,
		ST_STRUCT,
		ST_RECORDSET,
		ST_FIELD,
		ST_DATETIME
	} type;
	char *varname;
} st_entry;

typedef struct {
	int top, max;
	char *varname;
	zend_bool done;
	void **elements;
} wddx_stack;


static void php_wddx_process_data(void *user_data, const XML_Char *s, int len);

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_wddx_serialize_value, 0, 0, 1)
	ZEND_ARG_INFO(0, var)
	ZEND_ARG_INFO(0, comment)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_wddx_serialize_vars, 0, 0, 1)
	ZEND_ARG_VARIADIC_INFO(0, var_names)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_wddx_serialize_start, 0, 0, 0)
	ZEND_ARG_INFO(0, comment)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_wddx_packet_end, 0, 0, 1)
	ZEND_ARG_INFO(0, packet_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_wddx_add_vars, 0, 0, 2)
	ZEND_ARG_INFO(0, packet_id)
	ZEND_ARG_VARIADIC_INFO(0, var_names)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_wddx_deserialize, 0, 0, 1)
	ZEND_ARG_INFO(0, packet)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ wddx_functions[]
 */
const zend_function_entry wddx_functions[] = {
	PHP_FE(wddx_serialize_value, arginfo_wddx_serialize_value)
	PHP_FE(wddx_serialize_vars,	arginfo_wddx_serialize_vars)
	PHP_FE(wddx_packet_start,	arginfo_wddx_serialize_start)
	PHP_FE(wddx_packet_end,		arginfo_wddx_packet_end)
	PHP_FE(wddx_add_vars,		arginfo_wddx_add_vars)
	PHP_FE(wddx_deserialize,	arginfo_wddx_deserialize)
	PHP_FE_END
};
/* }}} */

PHP_MINIT_FUNCTION(wddx);
PHP_MINFO_FUNCTION(wddx);

/* {{{ dynamically loadable module stuff */
#ifdef COMPILE_DL_WDDX
ZEND_GET_MODULE(wddx)
#endif /* COMPILE_DL_WDDX */
/* }}} */

/* {{{ wddx_module_entry
 */
zend_module_entry wddx_module_entry = {
	STANDARD_MODULE_HEADER,
	"wddx",
	wddx_functions,
	PHP_MINIT(wddx),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(wddx),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* {{{ wddx_stack_init
 */
static int wddx_stack_init(wddx_stack *stack)
{
	stack->top = 0;
	stack->elements = (void **) safe_emalloc(sizeof(void **), STACK_BLOCK_SIZE, 0);
	stack->max = STACK_BLOCK_SIZE;
	stack->varname = NULL;
	stack->done = 0;

	return SUCCESS;
}
/* }}} */

/* {{{ wddx_stack_push
 */
static int wddx_stack_push(wddx_stack *stack, void *element, int size)
{
	if (stack->top >= stack->max) {		/* we need to allocate more memory */
		stack->elements = (void **) erealloc(stack->elements,
				   (sizeof(void **) * (stack->max += STACK_BLOCK_SIZE)));
	}
	stack->elements[stack->top] = (void *) emalloc(size);
	memcpy(stack->elements[stack->top], element, size);
	return stack->top++;
}
/* }}} */

/* {{{ wddx_stack_top
 */
static int wddx_stack_top(wddx_stack *stack, void **element)
{
	if (stack->top > 0) {
		*element = stack->elements[stack->top - 1];
		return SUCCESS;
	} else {
		*element = NULL;
		return FAILURE;
	}
}
/* }}} */

/* {{{ wddx_stack_is_empty
 */
static int wddx_stack_is_empty(wddx_stack *stack)
{
	if (stack->top == 0) {
		return 1;
	} else {
		return 0;
	}
}
/* }}} */

/* {{{ wddx_stack_destroy
 */
static int wddx_stack_destroy(wddx_stack *stack)
{
	register int i;

	if (stack->elements) {
		for (i = 0; i < stack->top; i++) {
			zval_ptr_dtor(&((st_entry *)stack->elements[i])->data);
			if (((st_entry *)stack->elements[i])->varname) {
				efree(((st_entry *)stack->elements[i])->varname);
			}
			efree(stack->elements[i]);
		}
		efree(stack->elements);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ release_wddx_packet_rsrc
 */
static void release_wddx_packet_rsrc(zend_resource *rsrc)
{
	smart_str *str = (smart_str *)rsrc->ptr;
	smart_str_free(str);
	efree(str);
}
/* }}} */

#include "ext/session/php_session.h"

#if HAVE_PHP_SESSION && !defined(COMPILE_DL_SESSION)
/* {{{ PS_SERIALIZER_ENCODE_FUNC
 */
PS_SERIALIZER_ENCODE_FUNC(wddx)
{
	wddx_packet *packet;
	zend_string *str;
	PS_ENCODE_VARS;

	packet = php_wddx_constructor();

	php_wddx_packet_start(packet, NULL, 0);
	php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);

	PS_ENCODE_LOOP(
		php_wddx_serialize_var(packet, struc, key);
	);

	php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
	php_wddx_packet_end(packet);
	smart_str_0(packet);
	str = zend_string_copy(packet->s);
	php_wddx_destructor(packet);

	return str;
}
/* }}} */

/* {{{ PS_SERIALIZER_DECODE_FUNC
 */
PS_SERIALIZER_DECODE_FUNC(wddx)
{
	zval retval;
	zval *ent;
	zend_string *key;
	zend_ulong idx;
	int ret;

	if (vallen == 0) {
		return SUCCESS;
	}

	ZVAL_UNDEF(&retval);
	if ((ret = php_wddx_deserialize_ex(val, vallen, &retval)) == SUCCESS) {
		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(retval), idx, key, ent) {
			if (key == NULL) {
				key = zend_long_to_str(idx);
			} else {
				zend_string_addref(key);
			}
			if (php_set_session_var(key, ent, NULL)) {
				if (Z_REFCOUNTED_P(ent)) Z_ADDREF_P(ent);
			}
			PS_ADD_VAR(key);
			zend_string_release(key);
		} ZEND_HASH_FOREACH_END();
	}

	zval_ptr_dtor(&retval);

	return ret;
}
/* }}} */
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(wddx)
{
	le_wddx = zend_register_list_destructors_ex(release_wddx_packet_rsrc, NULL, "wddx", module_number);

#if HAVE_PHP_SESSION && !defined(COMPILE_DL_SESSION)
	php_session_register_serializer("wddx",
									PS_SERIALIZER_ENCODE_NAME(wddx),
									PS_SERIALIZER_DECODE_NAME(wddx));
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(wddx)
{
	php_info_print_table_start();
#if HAVE_PHP_SESSION && !defined(COMPILE_DL_SESSION)
	php_info_print_table_header(2, "WDDX Support", "enabled" );
	php_info_print_table_row(2, "WDDX Session Serializer", "enabled" );
#else
	php_info_print_table_row(2, "WDDX Support", "enabled" );
#endif
	php_info_print_table_end();
}
/* }}} */

/* {{{ php_wddx_packet_start
 */
void php_wddx_packet_start(wddx_packet *packet, char *comment, size_t comment_len)
{
	php_wddx_add_chunk_static(packet, WDDX_PACKET_S);
	if (comment) {
		php_wddx_add_chunk_static(packet, WDDX_HEADER_S);
		php_wddx_add_chunk_static(packet, WDDX_COMMENT_S);
		php_wddx_add_chunk_ex(packet, comment, comment_len);
		php_wddx_add_chunk_static(packet, WDDX_COMMENT_E);
		php_wddx_add_chunk_static(packet, WDDX_HEADER_E);
	} else {
		php_wddx_add_chunk_static(packet, WDDX_HEADER);
	}
	php_wddx_add_chunk_static(packet, WDDX_DATA_S);
}
/* }}} */

/* {{{ php_wddx_packet_end
 */
void php_wddx_packet_end(wddx_packet *packet)
{
	php_wddx_add_chunk_static(packet, WDDX_DATA_E);
	php_wddx_add_chunk_static(packet, WDDX_PACKET_E);
}
/* }}} */

#define FLUSH_BUF()                               \
	if (l > 0) {                                  \
		php_wddx_add_chunk_ex(packet, buf, l);    \
		l = 0;                                    \
	}

/* {{{ php_wddx_serialize_string
 */
static void php_wddx_serialize_string(wddx_packet *packet, zval *var)
{
	php_wddx_add_chunk_static(packet, WDDX_STRING_S);

	if (Z_STRLEN_P(var) > 0) {
		zend_string *buf;

		buf = php_escape_html_entities(Z_STRVAL_P(var), Z_STRLEN_P(var), 0, ENT_QUOTES, NULL);

		php_wddx_add_chunk_ex(packet, buf->val, buf->len);

		zend_string_release(buf);
	}
	php_wddx_add_chunk_static(packet, WDDX_STRING_E);
}
/* }}} */

/* {{{ php_wddx_serialize_number
 */
static void php_wddx_serialize_number(wddx_packet *packet, zval *var)
{
	char tmp_buf[WDDX_BUF_LEN];
	zval tmp;

	ZVAL_DUP(&tmp, var);
	convert_to_string(&tmp);
	snprintf(tmp_buf, sizeof(tmp_buf), WDDX_NUMBER, Z_STRVAL(tmp));
	zval_ptr_dtor(&tmp);

	php_wddx_add_chunk(packet, tmp_buf);
}
/* }}} */

/* {{{ php_wddx_serialize_boolean
 */
static void php_wddx_serialize_boolean(wddx_packet *packet, zval *var)
{
	php_wddx_add_chunk(packet, Z_TYPE_P(var) == IS_TRUE ? WDDX_BOOLEAN_TRUE : WDDX_BOOLEAN_FALSE);
}
/* }}} */

/* {{{ php_wddx_serialize_unset
 */
static void php_wddx_serialize_unset(wddx_packet *packet)
{
	php_wddx_add_chunk_static(packet, WDDX_NULL);
}
/* }}} */

/* {{{ php_wddx_serialize_object
 */
static void php_wddx_serialize_object(wddx_packet *packet, zval *obj)
{
/* OBJECTS_FIXME */
	zval *ent, fname, *varname;
	zval retval;
	zend_string *key;
	zend_ulong idx;
	char tmp_buf[WDDX_BUF_LEN];
	HashTable *objhash, *sleephash;

	ZVAL_STRING(&fname, "__sleep");
	/*
	 * We try to call __sleep() method on object. It's supposed to return an
	 * array of property names to be serialized.
	 */
	if (call_user_function_ex(CG(function_table), obj, &fname, &retval, 0, 0, 1, NULL) == SUCCESS) {
		if (!Z_ISUNDEF(retval) && (sleephash = HASH_OF(&retval))) {
			PHP_CLASS_ATTRIBUTES;

			PHP_SET_CLASS_ATTRIBUTES(obj);

			php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);
			snprintf(tmp_buf, WDDX_BUF_LEN, WDDX_VAR_S, PHP_CLASS_NAME_VAR);
			php_wddx_add_chunk(packet, tmp_buf);
			php_wddx_add_chunk_static(packet, WDDX_STRING_S);
			php_wddx_add_chunk_ex(packet, class_name->val, class_name->len);
			php_wddx_add_chunk_static(packet, WDDX_STRING_E);
			php_wddx_add_chunk_static(packet, WDDX_VAR_E);

			PHP_CLEANUP_CLASS_ATTRIBUTES();

			objhash = HASH_OF(obj);

			ZEND_HASH_FOREACH_VAL(sleephash, varname) {
				if (Z_TYPE_P(varname) != IS_STRING) {
					php_error_docref(NULL, E_NOTICE, "__sleep should return an array only containing the names of instance-variables to serialize.");
					continue;
				}

				if ((ent = zend_hash_find(objhash, Z_STR_P(varname))) != NULL) {
					php_wddx_serialize_var(packet, ent, Z_STR_P(varname));
				}
			} ZEND_HASH_FOREACH_END();

			php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
		}
	} else {
		PHP_CLASS_ATTRIBUTES;

		PHP_SET_CLASS_ATTRIBUTES(obj);

		php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);
		snprintf(tmp_buf, WDDX_BUF_LEN, WDDX_VAR_S, PHP_CLASS_NAME_VAR);
		php_wddx_add_chunk(packet, tmp_buf);
		php_wddx_add_chunk_static(packet, WDDX_STRING_S);
		php_wddx_add_chunk_ex(packet, class_name->val, class_name->len);
		php_wddx_add_chunk_static(packet, WDDX_STRING_E);
		php_wddx_add_chunk_static(packet, WDDX_VAR_E);

		PHP_CLEANUP_CLASS_ATTRIBUTES();

		objhash = HASH_OF(obj);
		ZEND_HASH_FOREACH_KEY_VAL(objhash, idx, key, ent) {
			if (ent == obj) {
				continue;
			}
			if (key) {
				const char *class_name, *prop_name;
				size_t prop_name_len;
				zend_string *tmp;

				zend_unmangle_property_name_ex(key, &class_name, &prop_name, &prop_name_len);
				tmp = zend_string_init(prop_name, prop_name_len, 0);
				php_wddx_serialize_var(packet, ent, tmp);
				zend_string_release(tmp);
			} else {
				key = zend_long_to_str(idx);
				php_wddx_serialize_var(packet, ent, key);
				zend_string_release(key);
			}
		} ZEND_HASH_FOREACH_END();
		php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
	}

	zval_ptr_dtor(&fname);
	zval_ptr_dtor(&retval);
}
/* }}} */

/* {{{ php_wddx_serialize_array
 */
static void php_wddx_serialize_array(wddx_packet *packet, zval *arr)
{
	zval *ent;
	zend_string *key;
	int is_struct = 0;
	zend_ulong idx;
	HashTable *target_hash;
	char tmp_buf[WDDX_BUF_LEN];
	zend_ulong ind = 0;

	target_hash = HASH_OF(arr);
	ZEND_HASH_FOREACH_KEY(target_hash, idx, key) {
		if (key) {
			is_struct = 1;
			break;
		}

		if (idx != ind) {
			is_struct = 1;
			break;
		}
		ind++;
	} ZEND_HASH_FOREACH_END();

	if (is_struct) {
		php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);
	} else {
		snprintf(tmp_buf, sizeof(tmp_buf), WDDX_ARRAY_S, zend_hash_num_elements(target_hash));
		php_wddx_add_chunk(packet, tmp_buf);
	}

	ZEND_HASH_FOREACH_KEY_VAL(target_hash, idx, key, ent) {
		if (ent == arr) {
			continue;
		}

		if (is_struct) {
			if (key) {
				php_wddx_serialize_var(packet, ent, key);
			} else {
				key = zend_long_to_str(idx);
				php_wddx_serialize_var(packet, ent, key);
				zend_string_release(key);
			}
		} else {
			php_wddx_serialize_var(packet, ent, NULL);
		}
	} ZEND_HASH_FOREACH_END();

	if (is_struct) {
		php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
	} else {
		php_wddx_add_chunk_static(packet, WDDX_ARRAY_E);
	}
}
/* }}} */

/* {{{ php_wddx_serialize_var
 */
void php_wddx_serialize_var(wddx_packet *packet, zval *var, zend_string *name)
{
	HashTable *ht;

	if (name) {
		char *tmp_buf;
		zend_string *name_esc;

		name_esc = php_escape_html_entities(name->val, name->len, 0, ENT_QUOTES, NULL);
		tmp_buf = emalloc(name_esc->len + sizeof(WDDX_VAR_S));
		snprintf(tmp_buf, name_esc->len + sizeof(WDDX_VAR_S), WDDX_VAR_S, name_esc->val);
		php_wddx_add_chunk(packet, tmp_buf);
		efree(tmp_buf);
		zend_string_release(name_esc);
	}

	if (Z_TYPE_P(var) == IS_INDIRECT) {
		var = Z_INDIRECT_P(var);
	}
	ZVAL_DEREF(var);
	switch (Z_TYPE_P(var)) {
		case IS_STRING:
			php_wddx_serialize_string(packet, var);
			break;

		case IS_LONG:
		case IS_DOUBLE:
			php_wddx_serialize_number(packet, var);
			break;

		case IS_TRUE:
		case IS_FALSE:
			php_wddx_serialize_boolean(packet, var);
			break;

		case IS_NULL:
			php_wddx_serialize_unset(packet);
			break;

		case IS_ARRAY:
			ht = Z_ARRVAL_P(var);
			if (ht->u.v.nApplyCount > 1) {
				php_error_docref(NULL, E_RECOVERABLE_ERROR, "WDDX doesn't support circular references");
				return;
			}
			if (ZEND_HASH_APPLY_PROTECTION(ht)) {
				ht->u.v.nApplyCount++;
			}
			php_wddx_serialize_array(packet, var);
			if (ZEND_HASH_APPLY_PROTECTION(ht)) {
				ht->u.v.nApplyCount--;
			}
			break;

		case IS_OBJECT:
			ht = Z_OBJPROP_P(var);
			if (ht->u.v.nApplyCount > 1) {
				php_error_docref(NULL, E_RECOVERABLE_ERROR, "WDDX doesn't support circular references");
				return;
			}
			ht->u.v.nApplyCount++;
 			php_wddx_serialize_object(packet, var);
			ht->u.v.nApplyCount--;
			break;
	}

	if (name) {
		php_wddx_add_chunk_static(packet, WDDX_VAR_E);
	}
}
/* }}} */

/* {{{ php_wddx_add_var
 */
static void php_wddx_add_var(wddx_packet *packet, zval *name_var)
{
	zval *val;
	HashTable *target_hash;

	if (Z_TYPE_P(name_var) == IS_STRING) {
		zend_array *symbol_table = zend_rebuild_symbol_table();
		if ((val = zend_hash_find(symbol_table, Z_STR_P(name_var))) != NULL) {
			if (Z_TYPE_P(val) == IS_INDIRECT) {
				val = Z_INDIRECT_P(val);
			}
			php_wddx_serialize_var(packet, val, Z_STR_P(name_var));
		}
	} else if (Z_TYPE_P(name_var) == IS_ARRAY || Z_TYPE_P(name_var) == IS_OBJECT)	{
		int is_array = Z_TYPE_P(name_var) == IS_ARRAY;

		target_hash = HASH_OF(name_var);

		if (is_array && target_hash->u.v.nApplyCount > 1) {
			php_error_docref(NULL, E_WARNING, "recursion detected");
			return;
		}

		if (Z_IMMUTABLE_P(name_var)) {
			ZEND_HASH_FOREACH_VAL(target_hash, val) {
				php_wddx_add_var(packet, val);
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_FOREACH_VAL(target_hash, val) {
				if (is_array) {
					target_hash->u.v.nApplyCount++;
				}

				ZVAL_DEREF(val);
				php_wddx_add_var(packet, val);

				if (is_array) {
					target_hash->u.v.nApplyCount--;
				}
			} ZEND_HASH_FOREACH_END();
		}
	}
}
/* }}} */

/* {{{ php_wddx_push_element
 */
static void php_wddx_push_element(void *user_data, const XML_Char *name, const XML_Char **atts)
{
	st_entry ent;
	wddx_stack *stack = (wddx_stack *)user_data;
	if (!strcmp((char *)name, EL_PACKET)) {
		int i;

		if (atts) for (i=0; atts[i]; i++) {
			if (!strcmp((char *)atts[i], EL_VERSION)) {
				/* nothing for now */
			}
		}
	} else if (!strcmp((char *)name, EL_STRING)) {
		ent.type = ST_STRING;
		SET_STACK_VARNAME;

		ZVAL_STR(&ent.data, STR_EMPTY_ALLOC());
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_BINARY)) {
		ent.type = ST_BINARY;
		SET_STACK_VARNAME;

		ZVAL_STR(&ent.data, STR_EMPTY_ALLOC());
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_CHAR)) {
		int i;

		if (atts) for (i = 0; atts[i]; i++) {
			if (!strcmp((char *)atts[i], EL_CHAR_CODE) && atts[++i] && atts[i][0]) {
				char tmp_buf[2];

				snprintf(tmp_buf, sizeof(tmp_buf), "%c", (char)strtol((char *)atts[i], NULL, 16));
				php_wddx_process_data(user_data, tmp_buf, strlen(tmp_buf));
				break;
			}
		}
	} else if (!strcmp((char *)name, EL_NUMBER)) {
		ent.type = ST_NUMBER;
		SET_STACK_VARNAME;

		ZVAL_LONG(&ent.data, 0);
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_BOOLEAN)) {
		int i;

		if (atts) for (i = 0; atts[i]; i++) {
			if (!strcmp((char *)atts[i], EL_VALUE) && atts[++i] && atts[i][0]) {
				ent.type = ST_BOOLEAN;
				SET_STACK_VARNAME;

				ZVAL_TRUE(&ent.data);
				wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
				php_wddx_process_data(user_data, atts[i], strlen((char *)atts[i]));
				break;
			}
		}
	} else if (!strcmp((char *)name, EL_NULL)) {
		ent.type = ST_NULL;
		SET_STACK_VARNAME;

		ZVAL_NULL(&ent.data);

		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_ARRAY)) {
		ent.type = ST_ARRAY;
		SET_STACK_VARNAME;

		array_init(&ent.data);
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_STRUCT)) {
		ent.type = ST_STRUCT;
		SET_STACK_VARNAME;
		array_init(&ent.data);
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_VAR)) {
		int i;

		if (atts) for (i = 0; atts[i]; i++) {
			if (!strcmp((char *)atts[i], EL_NAME) && atts[++i] && atts[i][0]) {
				stack->varname = estrdup(atts[i]);
				break;
			}
		}
	} else if (!strcmp((char *)name, EL_RECORDSET)) {
		int i;

		ent.type = ST_RECORDSET;
		SET_STACK_VARNAME;
		array_init(&ent.data);

		if (atts) for (i = 0; atts[i]; i++) {
			if (!strcmp((char *)atts[i], "fieldNames") && atts[++i] && atts[i][0]) {
				zval tmp;
				char *key;
				char *p1, *p2, *endp;

				endp = (char *)atts[i] + strlen(atts[i]);
				p1 = (char *)atts[i];
				while ((p2 = php_memnstr(p1, ",", sizeof(",")-1, endp)) != NULL) {
					key = estrndup(p1, p2 - p1);
					array_init(&tmp);
					add_assoc_zval_ex(&ent.data, key, p2 - p1, &tmp);
					p1 = p2 + sizeof(",")-1;
					efree(key);
				}

				if (p1 <= endp) {
					array_init(&tmp);
					add_assoc_zval_ex(&ent.data, p1, endp - p1, &tmp);
				}

				break;
			}
		}

		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_FIELD)) {
		int i;
		st_entry ent;

		ent.type = ST_FIELD;
		ent.varname = NULL;
		ZVAL_UNDEF(&ent.data);

		if (atts) for (i = 0; atts[i]; i++) {
			if (!strcmp((char *)atts[i], EL_NAME) && atts[++i] && atts[i][0]) {
				st_entry *recordset;
				zval *field;

				if (wddx_stack_top(stack, (void**)&recordset) == SUCCESS &&
					recordset->type == ST_RECORDSET &&
					(field = zend_hash_str_find(Z_ARRVAL(recordset->data), (char*)atts[i], strlen(atts[i]))) != NULL) {
					ZVAL_COPY_VALUE(&ent.data, field);
				}

				break;
			}
		}

		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp((char *)name, EL_DATETIME)) {
		ent.type = ST_DATETIME;
		SET_STACK_VARNAME;

		ZVAL_LONG(&ent.data, 0);
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	}
}
/* }}} */

/* {{{ php_wddx_pop_element
 */
static void php_wddx_pop_element(void *user_data, const XML_Char *name)
{
	st_entry 			*ent1, *ent2;
	wddx_stack 			*stack = (wddx_stack *)user_data;
	HashTable 			*target_hash;
	zend_class_entry 	*pce;
	zval				obj;

/* OBJECTS_FIXME */
	if (stack->top == 0) {
		return;
	}

	if (!strcmp((char *)name, EL_STRING) || !strcmp((char *)name, EL_NUMBER) ||
		!strcmp((char *)name, EL_BOOLEAN) || !strcmp((char *)name, EL_NULL) ||
	  	!strcmp((char *)name, EL_ARRAY) || !strcmp((char *)name, EL_STRUCT) ||
		!strcmp((char *)name, EL_RECORDSET) || !strcmp((char *)name, EL_BINARY) ||
		!strcmp((char *)name, EL_DATETIME)) {
		wddx_stack_top(stack, (void**)&ent1);

		if (!strcmp((char *)name, EL_BINARY)) {
			zend_string *new_str;

			new_str = php_base64_decode(Z_STRVAL(ent1->data), Z_STRLEN(ent1->data));
			zval_ptr_dtor(&ent1->data);
			ZVAL_STR(&ent1->data, new_str);
		}

		/* Call __wakeup() method on the object. */
		if (Z_TYPE(ent1->data) == IS_OBJECT) {
			zval fname, retval;

			ZVAL_STRING(&fname, "__wakeup");

			call_user_function_ex(NULL, &ent1->data, &fname, &retval, 0, 0, 0, NULL);

			zval_ptr_dtor(&fname);
			zval_ptr_dtor(&retval);
		}

		if (stack->top > 1) {
			stack->top--;
			wddx_stack_top(stack, (void**)&ent2);

			/* if non-existent field */
			if (ent2->type == ST_FIELD && Z_ISUNDEF(ent2->data)) {
				zval_ptr_dtor(&ent1->data);
				efree(ent1);
				return;
			}

			if (Z_TYPE(ent2->data) == IS_ARRAY || Z_TYPE(ent2->data) == IS_OBJECT) {
				target_hash = HASH_OF(&ent2->data);

				if (ent1->varname) {
					if (!strcmp(ent1->varname, PHP_CLASS_NAME_VAR) &&
						Z_TYPE(ent1->data) == IS_STRING && Z_STRLEN(ent1->data)) {
						zend_bool incomplete_class = 0;

						zend_str_tolower(Z_STRVAL(ent1->data), Z_STRLEN(ent1->data));
						zend_string_forget_hash_val(Z_STR(ent1->data));
						if ((pce = zend_hash_find_ptr(EG(class_table), Z_STR(ent1->data))) == NULL) {
							incomplete_class = 1;
							pce = PHP_IC_ENTRY;
						}

						/* Initialize target object */
						object_init_ex(&obj, pce);

						/* Merge current hashtable with object's default properties */
						zend_hash_merge(Z_OBJPROP(obj),
										Z_ARRVAL(ent2->data),
										zval_add_ref, 0);

						if (incomplete_class) {
							php_store_class_name(&obj, Z_STRVAL(ent1->data), Z_STRLEN(ent1->data));
						}

						/* Clean up old array entry */
						zval_ptr_dtor(&ent2->data);

						/* Set stack entry to point to the newly created object */
						ZVAL_COPY_VALUE(&ent2->data, &obj);

						/* Clean up class name var entry */
						zval_ptr_dtor(&ent1->data);
					} else if (Z_TYPE(ent2->data) == IS_OBJECT) {
						zend_class_entry *old_scope = EG(scope);

						EG(scope) = Z_OBJCE(ent2->data);
						add_property_zval(&ent2->data, ent1->varname, &ent1->data);
						if Z_REFCOUNTED(ent1->data) Z_DELREF(ent1->data);
						EG(scope) = old_scope;
					} else {
						zend_symtable_str_update(target_hash, ent1->varname, strlen(ent1->varname), &ent1->data);
					}
					efree(ent1->varname);
				} else	{
					zend_hash_next_index_insert(target_hash, &ent1->data);
				}
			}
			efree(ent1);
		} else {
			stack->done = 1;
		}
	} else if (!strcmp((char *)name, EL_VAR) && stack->varname) {
		efree(stack->varname);
	} else if (!strcmp((char *)name, EL_FIELD)) {
		st_entry *ent;
		wddx_stack_top(stack, (void **)&ent);
		efree(ent);
		stack->top--;
	}
}
/* }}} */

/* {{{ php_wddx_process_data
 */
static void php_wddx_process_data(void *user_data, const XML_Char *s, int len)
{
	st_entry *ent;
	wddx_stack *stack = (wddx_stack *)user_data;

	if (!wddx_stack_is_empty(stack) && !stack->done) {
		wddx_stack_top(stack, (void**)&ent);
		switch (ent->type) {
			case ST_BINARY:
			case ST_STRING:
				if (Z_STRLEN(ent->data) == 0) {
					zval_ptr_dtor(&ent->data);
					ZVAL_STRINGL(&ent->data, (char *)s, len);
				} else {
					Z_STR(ent->data) = zend_string_realloc(Z_STR(ent->data), Z_STRLEN(ent->data) + len, 0);
					memcpy(Z_STRVAL(ent->data) + Z_STRLEN(ent->data) - len, (char *)s, len);
					Z_STRVAL(ent->data)[Z_STRLEN(ent->data)] = '\0';
				}
				break;
			case ST_NUMBER:
				ZVAL_STRINGL(&ent->data, (char *)s, len);
				convert_scalar_to_number(&ent->data);
				break;

			case ST_BOOLEAN:
				if (!strcmp((char *)s, "true")) {
					Z_LVAL(ent->data) = 1;
				} else if (!strcmp((char *)s, "false")) {
					Z_LVAL(ent->data) = 0;
				} else {
					stack->top--;
					zval_ptr_dtor(&ent->data);
					if (ent->varname)
						efree(ent->varname);
					efree(ent);
				}
				break;

			case ST_DATETIME: {
				char *tmp;

				tmp = emalloc(len + 1);
				memcpy(tmp, (char *)s, len);
				tmp[len] = '\0';

				Z_LVAL(ent->data) = php_parse_date(tmp, NULL);
				/* date out of range < 1969 or > 2038 */
				if (Z_LVAL(ent->data) == -1) {
					ZVAL_STRINGL(&ent->data, (char *)s, len);
				}
				efree(tmp);
			}
				break;

			default:
				break;
		}
	}
}
/* }}} */

/* {{{ php_wddx_deserialize_ex
 */
int php_wddx_deserialize_ex(const char *value, size_t vallen, zval *return_value)
{
	wddx_stack stack;
	XML_Parser parser;
	st_entry *ent;
	int retval;

	wddx_stack_init(&stack);
	parser = XML_ParserCreate("UTF-8");

	XML_SetUserData(parser, &stack);
	XML_SetElementHandler(parser, php_wddx_push_element, php_wddx_pop_element);
	XML_SetCharacterDataHandler(parser, php_wddx_process_data);

	/* XXX value should be parsed in the loop to exhaust size_t */
	XML_Parse(parser, value, (int)vallen, 1);

	XML_ParserFree(parser);

	if (stack.top == 1) {
		wddx_stack_top(&stack, (void**)&ent);
		ZVAL_COPY(return_value, &ent->data);
		retval = SUCCESS;
	} else {
		retval = FAILURE;
	}

	wddx_stack_destroy(&stack);

	return retval;
}
/* }}} */

/* {{{ proto string wddx_serialize_value(mixed var [, string comment])
   Creates a new packet and serializes the given value */
PHP_FUNCTION(wddx_serialize_value)
{
	zval *var;
	char *comment = NULL;
	size_t comment_len = 0;
	wddx_packet *packet;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|s", &var, &comment, &comment_len) == FAILURE) {
		return;
	}

	packet = php_wddx_constructor();

	php_wddx_packet_start(packet, comment, comment_len);
	php_wddx_serialize_var(packet, var, NULL);
	php_wddx_packet_end(packet);
	smart_str_0(packet);

	RETVAL_STR_COPY(packet->s);
	php_wddx_destructor(packet);
}
/* }}} */

/* {{{ proto string wddx_serialize_vars(mixed var_name [, mixed ...])
   Creates a new packet and serializes given variables into a struct */
PHP_FUNCTION(wddx_serialize_vars)
{
	int num_args, i;
	wddx_packet *packet;
	zval *args = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "+", &args, &num_args) == FAILURE) {
		return;
	}

	packet = php_wddx_constructor();

	php_wddx_packet_start(packet, NULL, 0);
	php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);

	for (i=0; i<num_args; i++) {
		zval *arg;
		if (!Z_ISREF(args[i])) {
			arg = &args[i];
		} else {
			arg = Z_REFVAL(args[i]);
		}
		if (Z_TYPE_P(arg) != IS_ARRAY && Z_TYPE_P(arg) != IS_OBJECT) {
			convert_to_string_ex(arg);
		}
		php_wddx_add_var(packet, arg);
	}

	php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
	php_wddx_packet_end(packet);
	smart_str_0(packet);

	RETVAL_STR_COPY(packet->s);
	php_wddx_destructor(packet);
}
/* }}} */

/* {{{ php_wddx_constructor
 */
wddx_packet *php_wddx_constructor(void)
{
	smart_str *packet;

	packet = ecalloc(1, sizeof(smart_str));

	return packet;
}
/* }}} */

/* {{{ php_wddx_destructor
 */
void php_wddx_destructor(wddx_packet *packet)
{
	smart_str_free(packet);
	efree(packet);
}
/* }}} */

/* {{{ proto resource wddx_packet_start([string comment])
   Starts a WDDX packet with optional comment and returns the packet id */
PHP_FUNCTION(wddx_packet_start)
{
	char *comment = NULL;
	size_t comment_len = 0;
	wddx_packet *packet;

	comment = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &comment, &comment_len) == FAILURE) {
		return;
	}

	packet = php_wddx_constructor();

	php_wddx_packet_start(packet, comment, comment_len);
	php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);

	RETURN_RES(zend_register_resource(packet, le_wddx));
}
/* }}} */

/* {{{ proto string wddx_packet_end(resource packet_id)
   Ends specified WDDX packet and returns the string containing the packet */
PHP_FUNCTION(wddx_packet_end)
{
	zval *packet_id;
	wddx_packet *packet = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &packet_id) == FAILURE) {
		return;
	}

	if ((packet = (wddx_packet *)zend_fetch_resource(Z_RES_P(packet_id), "WDDX packet ID", le_wddx)) == NULL) {
		RETURN_FALSE;
	}

	php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);

	php_wddx_packet_end(packet);
	smart_str_0(packet);

	RETVAL_STR_COPY(packet->s);

	zend_list_close(Z_RES_P(packet_id));
}
/* }}} */

/* {{{ proto int wddx_add_vars(resource packet_id,  mixed var_names [, mixed ...])
   Serializes given variables and adds them to packet given by packet_id */
PHP_FUNCTION(wddx_add_vars)
{
	int num_args, i;
	zval *args = NULL;
	zval *packet_id;
	wddx_packet *packet = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r+", &packet_id, &args, &num_args) == FAILURE) {
		return;
	}

	if ((packet = (wddx_packet *)zend_fetch_resource(Z_RES_P(packet_id), "WDDX packet ID", le_wddx)) == NULL) {
		RETURN_FALSE;
	}

	for (i=0; i<num_args; i++) {
		zval *arg;
		if (!Z_ISREF(args[i])) {
			arg = &args[i];
		} else {
			arg = Z_REFVAL(args[i]);
		}
		if (Z_TYPE_P(arg) != IS_ARRAY && Z_TYPE_P(arg) != IS_OBJECT) {
			convert_to_string_ex(arg);
		}
		php_wddx_add_var(packet, arg);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed wddx_deserialize(mixed packet)
   Deserializes given packet and returns a PHP value */
PHP_FUNCTION(wddx_deserialize)
{
	zval *packet;
	php_stream *stream = NULL;
	zend_string *payload = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &packet) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(packet) == IS_STRING) {
		payload = Z_STR_P(packet);
	} else if (Z_TYPE_P(packet) == IS_RESOURCE) {
		php_stream_from_zval(stream, packet);
		if (stream) {
			payload = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0);
		}
	} else {
		php_error_docref(NULL, E_WARNING, "Expecting parameter 1 to be a string or a stream");
		return;
	}

	if (payload == NULL) {
		return;
	}

	php_wddx_deserialize_ex(payload->val, payload->len, return_value);

	if (stream) {
		efree(payload);
	}
}
/* }}} */

#endif /* HAVE_LIBEXPAT */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
