/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andrei Zmievski <andrei@php.net>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "php_wddx.h"

#if HAVE_WDDX

#include "php_wddx_api.h"
#define PHP_XML_INTERNAL
#include "ext/xml/php_xml.h"
#include "ext/standard/php_incomplete_class.h"
#include "ext/standard/base64.h"
#include "ext/standard/info.h"
#include "ext/standard/php_smart_str.h"
#include "ext/standard/html.h"
#include "ext/standard/php_string.h"

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
	zval *data;
	enum {
		ST_ARRAY,
		ST_BOOLEAN,
		ST_NULL,
		ST_NUMBER,
		ST_STRING,
		ST_BINARY,
		ST_STRUCT,
		ST_RECORDSET,
		ST_FIELD
	} type;
	char *varname;
} st_entry;

typedef struct {
	int top, max;
	char *varname;
	zend_bool done;
	void **elements;
} wddx_stack;


static void php_wddx_process_data(void *user_data, const char *s, int len);

/* {{{ wddx_functions[]
 */
function_entry wddx_functions[] = {
	PHP_FE(wddx_serialize_value, NULL)
	PHP_FE(wddx_serialize_vars, NULL)
	PHP_FE(wddx_packet_start, NULL)
	PHP_FE(wddx_packet_end, NULL)
	PHP_FE(wddx_add_vars, NULL)
	PHP_FE(wddx_deserialize, NULL)
	{NULL, NULL, NULL}
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
	stack->elements = (void **) emalloc(sizeof(void **) * STACK_BLOCK_SIZE);
	if (!stack->elements) {
		return FAILURE;
	} else {
		stack->max = STACK_BLOCK_SIZE;
		stack->varname = NULL;
		stack->done = 0;
		return SUCCESS;
	}
}
/* }}} */

/* {{{ wddx_stack_push
 */
static int wddx_stack_push(wddx_stack *stack, void *element, int size)
{
	if (stack->top >= stack->max) {		/* we need to allocate more memory */
		stack->elements = (void **) erealloc(stack->elements,
				   (sizeof(void **) * (stack->max += STACK_BLOCK_SIZE)));
		if (!stack->elements) {
			return FAILURE;
		}
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
			if (((st_entry *)stack->elements[i])->data)
			{
				zval_ptr_dtor(&((st_entry *)stack->elements[i])->data);
			}
			if (((st_entry *)stack->elements[i])->varname)
				efree(((st_entry *)stack->elements[i])->varname);
			efree(stack->elements[i]);
		}		
		efree(stack->elements);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ release_wddx_packet_rsrc
 */
static void release_wddx_packet_rsrc(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	smart_str *str = (smart_str *)rsrc->ptr;
	smart_str_free(str);
	efree(str);
}
/* }}} */

#include "ext/session/php_session.h"

#if HAVE_PHP_SESSION
/* {{{ PS_SERIALIZER_ENCODE_FUNC
 */
PS_SERIALIZER_ENCODE_FUNC(wddx)
{
	wddx_packet *packet;
	PS_ENCODE_VARS;

	packet = php_wddx_constructor();
	if (!packet)
		return FAILURE;

	php_wddx_packet_start(packet, NULL, 0);
	php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);
	
	PS_ENCODE_LOOP(
		php_wddx_serialize_var(packet, *struc, key, key_length);
	);
	
	php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
	php_wddx_packet_end(packet);
	*newstr = php_wddx_gather(packet);
	php_wddx_destructor(packet);
	
	if (newlen)
		*newlen = strlen(*newstr);

	return SUCCESS;
}
/* }}} */

/* {{{ PS_SERIALIZER_DECODE_FUNC
 */
PS_SERIALIZER_DECODE_FUNC(wddx)
{
	zval *retval;
	zval **ent;
	char *key;
	uint key_length;
	char tmp[128];
	ulong idx;
	int hash_type;
	int ret;

	if (vallen == 0) 
		return SUCCESS;
	
	MAKE_STD_ZVAL(retval);

	if ((ret = php_wddx_deserialize_ex((char *)val, vallen, retval)) == SUCCESS) {

		for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(retval));
			 zend_hash_get_current_data(Z_ARRVAL_P(retval), (void **) &ent) == SUCCESS;
			 zend_hash_move_forward(Z_ARRVAL_P(retval))) {
			hash_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(retval), &key, &key_length, &idx, 0, NULL);

			switch (hash_type) {
				case HASH_KEY_IS_LONG:
					sprintf(tmp, "%ld", idx);
					key = tmp;
					/* fallthru */
				case HASH_KEY_IS_STRING:
					php_set_session_var(key, key_length-1, *ent, NULL TSRMLS_CC);
					PS_ADD_VAR(key);
			}
		}
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

#if HAVE_PHP_SESSION
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
#if HAVE_PHP_SESSION
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
void php_wddx_packet_start(wddx_packet *packet, char *comment, int comment_len)
{
	php_wddx_add_chunk_static(packet, WDDX_PACKET_S);
	if (comment) {
		php_wddx_add_chunk_static(packet, WDDX_HEADER_S);
		php_wddx_add_chunk_static(packet, WDDX_COMMENT_S);
		php_wddx_add_chunk_ex(packet, comment, comment_len);
		php_wddx_add_chunk_static(packet, WDDX_COMMENT_E);
		php_wddx_add_chunk_static(packet, WDDX_HEADER_E);
	} else
		php_wddx_add_chunk_static(packet, WDDX_HEADER);
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
	char *buf,
		 *p,
		 *vend,
		 control_buf[WDDX_BUF_LEN];
	int l;

	php_wddx_add_chunk_static(packet, WDDX_STRING_S);

	if (Z_STRLEN_P(var) > 0) {
		l = 0;
		vend = Z_STRVAL_P(var) + Z_STRLEN_P(var);
		buf = (char *)emalloc(Z_STRLEN_P(var) + 1);

		for(p = Z_STRVAL_P(var); p != vend; p++) {
			switch (*p) {
				case '<':
					FLUSH_BUF();
					php_wddx_add_chunk_static(packet, "&lt;");
					break;

				case '&':
					FLUSH_BUF();
					php_wddx_add_chunk_static(packet, "&amp;");
					break;

				case '>':
					FLUSH_BUF();
					php_wddx_add_chunk_static(packet, "&gt;");
					break;

				default:
					if (iscntrl((int)*p)) {
						FLUSH_BUF();
						sprintf(control_buf, WDDX_CHAR, *p);
						php_wddx_add_chunk(packet, control_buf);
					} else
						buf[l++] = *p;
					break;
			}
		}

		FLUSH_BUF();
		efree(buf);
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
	
	tmp = *var;
	zval_copy_ctor(&tmp);
	convert_to_string(&tmp);
	sprintf(tmp_buf, WDDX_NUMBER, Z_STRVAL(tmp));
	zval_dtor(&tmp);

	php_wddx_add_chunk(packet, tmp_buf);	
}
/* }}} */

/* {{{ php_wddx_serialize_boolean
 */
static void php_wddx_serialize_boolean(wddx_packet *packet, zval *var)
{
	char tmp_buf[WDDX_BUF_LEN];

	sprintf(tmp_buf, WDDX_BOOLEAN, Z_LVAL_P(var) ? "true" : "false");
	php_wddx_add_chunk(packet, tmp_buf);
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
	zval **ent, *fname, **varname;
	zval *retval = NULL;
	char *key;
	ulong idx;
	char tmp_buf[WDDX_BUF_LEN];
	TSRMLS_FETCH();

	MAKE_STD_ZVAL(fname);
	ZVAL_STRING(fname, "__sleep", 1);

	/*
	 * We try to call __sleep() method on object. It's supposed to return an
	 * array of property names to be serialized.
	 */
	if (call_user_function_ex(CG(function_table), &obj, fname, &retval, 0, 0, 1, NULL TSRMLS_CC) == SUCCESS) {
		if (retval && HASH_OF(retval)) {
			PHP_CLASS_ATTRIBUTES;
			
			PHP_SET_CLASS_ATTRIBUTES(obj);

			php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);
			sprintf(tmp_buf, WDDX_VAR_S, PHP_CLASS_NAME_VAR);
			php_wddx_add_chunk(packet, tmp_buf);
			php_wddx_add_chunk_static(packet, WDDX_STRING_S);
			php_wddx_add_chunk_ex(packet, class_name, name_len);
			php_wddx_add_chunk_static(packet, WDDX_STRING_E);
			php_wddx_add_chunk_static(packet, WDDX_VAR_E);

			PHP_CLEANUP_CLASS_ATTRIBUTES();
			
			for (zend_hash_internal_pointer_reset(HASH_OF(retval));
				 zend_hash_get_current_data(HASH_OF(retval), (void **)&varname) == SUCCESS;
				 zend_hash_move_forward(HASH_OF(retval))) {
				if (Z_TYPE_PP(varname) != IS_STRING) {
					php_error(E_NOTICE, "__sleep should return an array only containing the names of instance-variables to serialize.");
					continue;
				}

				if (zend_hash_find(HASH_OF(obj), Z_STRVAL_PP(varname), Z_STRLEN_PP(varname)+1, (void **)&ent) == SUCCESS) {
					php_wddx_serialize_var(packet, *ent, Z_STRVAL_PP(varname), Z_STRLEN_PP(varname));
				}
			}
			
			php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
		}
	} else {
		uint key_len;

		PHP_CLASS_ATTRIBUTES;

		PHP_SET_CLASS_ATTRIBUTES(obj);

		php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);
		sprintf(tmp_buf, WDDX_VAR_S, PHP_CLASS_NAME_VAR);
		php_wddx_add_chunk(packet, tmp_buf);
		php_wddx_add_chunk_static(packet, WDDX_STRING_S);
		php_wddx_add_chunk_ex(packet, class_name, name_len);
		php_wddx_add_chunk_static(packet, WDDX_STRING_E);
		php_wddx_add_chunk_static(packet, WDDX_VAR_E);

		PHP_CLEANUP_CLASS_ATTRIBUTES();
		
		for (zend_hash_internal_pointer_reset(HASH_OF(obj));
			 zend_hash_get_current_data(HASH_OF(obj), (void**)&ent) == SUCCESS;
			 zend_hash_move_forward(HASH_OF(obj))) {
			if (*ent == obj)
				continue;

			if (zend_hash_get_current_key_ex(HASH_OF(obj), &key, &key_len, &idx, 0, NULL) == HASH_KEY_IS_STRING) {
				php_wddx_serialize_var(packet, *ent, key, key_len);
			} else {
				key_len = sprintf(tmp_buf, "%ld", idx);
				php_wddx_serialize_var(packet, *ent, tmp_buf, key_len);
			}
		}
		php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
	}

	zval_dtor(fname);
	FREE_ZVAL(fname);

	if (retval) {
		zval_ptr_dtor(&retval);
	}
}
/* }}} */

/* {{{ php_wddx_serialize_array
 */
static void php_wddx_serialize_array(wddx_packet *packet, zval *arr)
{
	zval **ent;
	char *key;
	uint key_len;
	int is_struct = 0, ent_type;
	ulong idx;
	HashTable *target_hash;
	char tmp_buf[WDDX_BUF_LEN];
	ulong ind = 0;
	int type;
	TSRMLS_FETCH();

	target_hash = HASH_OF(arr);

	for (zend_hash_internal_pointer_reset(target_hash);
		 zend_hash_get_current_data(target_hash, (void**)&ent) == SUCCESS;
		 zend_hash_move_forward(target_hash)) {

		type = zend_hash_get_current_key(target_hash, &key, &idx, 0);

		if (type == HASH_KEY_IS_STRING) {
			is_struct = 1;
			break;
		}

		if (idx != ind) {
			is_struct = 1;
			break;
		}

		ind++;
	}

	if (is_struct) {
		php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);
	} else {
		sprintf(tmp_buf, WDDX_ARRAY_S, zend_hash_num_elements(target_hash));
		php_wddx_add_chunk(packet, tmp_buf);
	}

	for (zend_hash_internal_pointer_reset(target_hash);
		 zend_hash_get_current_data(target_hash, (void**)&ent) == SUCCESS;
		 zend_hash_move_forward(target_hash)) {
		if (*ent == arr)
			continue;

		if (is_struct) {
			ent_type = zend_hash_get_current_key_ex(target_hash, &key, &key_len, &idx, 0, NULL);

			if (ent_type == HASH_KEY_IS_STRING) {
				php_wddx_serialize_var(packet, *ent, key, key_len);
			} else {
				key_len = sprintf(tmp_buf, "%ld", idx);
				php_wddx_serialize_var(packet, *ent, tmp_buf, key_len);
			}
		} else
			php_wddx_serialize_var(packet, *ent, NULL, 0);
	}
	
	if (is_struct) {
		php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
	} else {
		php_wddx_add_chunk_static(packet, WDDX_ARRAY_E);
	}
}
/* }}} */

/* {{{ php_wddx_serialize_var
 */
void php_wddx_serialize_var(wddx_packet *packet, zval *var, char *name, int name_len)
{
	char tmp_buf[WDDX_BUF_LEN];
	char *name_esc;
	int name_esc_len;

	if (name) {
		name_esc = php_escape_html_entities(name, name_len, &name_esc_len, 0, ENT_QUOTES, NULL);
		sprintf(tmp_buf, WDDX_VAR_S, name_esc);
		php_wddx_add_chunk(packet, tmp_buf);
		efree(name_esc);
	}
	
	switch(Z_TYPE_P(var)) {
		case IS_STRING:
			php_wddx_serialize_string(packet, var);
			break;
			
		case IS_LONG:
		case IS_DOUBLE:
			php_wddx_serialize_number(packet, var);
			break;

		case IS_BOOL:
			php_wddx_serialize_boolean(packet, var);
			break;

		case IS_NULL:
			php_wddx_serialize_unset(packet);
			break;
		
		case IS_ARRAY:
			php_wddx_serialize_array(packet, var);
			break;

		case IS_OBJECT:
			php_wddx_serialize_object(packet, var);
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
	zval **val;
	HashTable *target_hash;
	TSRMLS_FETCH();
	
	if (Z_TYPE_P(name_var) == IS_STRING)
	{
		if (zend_hash_find(EG(active_symbol_table), Z_STRVAL_P(name_var),
							Z_STRLEN_P(name_var)+1, (void**)&val) != FAILURE) {
			php_wddx_serialize_var(packet, *val, Z_STRVAL_P(name_var), Z_STRLEN_P(name_var));
		}		
	}
	else if (Z_TYPE_P(name_var) == IS_ARRAY || Z_TYPE_P(name_var) == IS_OBJECT)
	{
		target_hash = HASH_OF(name_var);
		
		zend_hash_internal_pointer_reset(target_hash);

		while(zend_hash_get_current_data(target_hash, (void**)&val) == SUCCESS) {
			php_wddx_add_var(packet, *val);
				
			zend_hash_move_forward(target_hash);
		}
	}
}
/* }}} */

/* {{{ php_wddx_push_element
 */
static void php_wddx_push_element(void *user_data, const char *name, const char **atts)
{
	st_entry ent;
	wddx_stack *stack = (wddx_stack *)user_data;
	
	if (!strcmp(name, EL_PACKET)) {
		int i;
		
		for (i=0; atts[i]; i++) {
			if (!strcmp(atts[i], EL_VERSION)) {
				/* nothing for now */
			}
		}
	} else if (!strcmp(name, EL_STRING)) {
		ent.type = ST_STRING;
		SET_STACK_VARNAME;
		
		ALLOC_ZVAL(ent.data);
		INIT_PZVAL(ent.data);
		Z_TYPE_P(ent.data) = IS_STRING;
		Z_STRVAL_P(ent.data) = empty_string;
		Z_STRLEN_P(ent.data) = 0;
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_BINARY)) {
		ent.type = ST_BINARY;
		SET_STACK_VARNAME;
		
		ALLOC_ZVAL(ent.data);
		INIT_PZVAL(ent.data);
		Z_TYPE_P(ent.data) = IS_STRING;
		Z_STRVAL_P(ent.data) = empty_string;
		Z_STRLEN_P(ent.data) = 0;
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_CHAR)) {
		int i;
		
		for (i = 0; atts[i]; i++) {
			if (!strcmp(atts[i], EL_CHAR_CODE) && atts[++i] && atts[i][0]) {
				char tmp_buf[2];

				sprintf(tmp_buf, "%c", (char)strtol(atts[i], NULL, 16));
				php_wddx_process_data(user_data, tmp_buf, strlen(tmp_buf));
				break;
			}
		}
	} else if (!strcmp(name, EL_NUMBER)) {
		ent.type = ST_NUMBER;
		SET_STACK_VARNAME;
		
		ALLOC_ZVAL(ent.data);
		INIT_PZVAL(ent.data);
		Z_TYPE_P(ent.data) = IS_LONG;
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_BOOLEAN)) {
		int i;

		for (i = 0; atts[i]; i++) {
			if (!strcmp(atts[i], EL_VALUE) && atts[++i] && atts[i][0]) {
				ent.type = ST_BOOLEAN;
				SET_STACK_VARNAME;

				ALLOC_ZVAL(ent.data);
				INIT_PZVAL(ent.data);
				Z_TYPE_P(ent.data) = IS_BOOL;
				wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
				php_wddx_process_data(user_data, atts[i], strlen(atts[i]));
				break;
			}
		}
	} else if (!strcmp(name, EL_NULL)) {
		ent.type = ST_NULL;
		SET_STACK_VARNAME;

		ALLOC_ZVAL(ent.data);
		INIT_PZVAL(ent.data);
		ZVAL_NULL(ent.data);
		
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_ARRAY)) {
		ent.type = ST_ARRAY;
		SET_STACK_VARNAME;
		
		ALLOC_ZVAL(ent.data);
		array_init(ent.data);
		INIT_PZVAL(ent.data);
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_STRUCT)) {
		ent.type = ST_STRUCT;
		SET_STACK_VARNAME;
		
		ALLOC_ZVAL(ent.data);
		array_init(ent.data);
		INIT_PZVAL(ent.data);
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_VAR)) {
		int i;
		
		for (i = 0; atts[i]; i++) {
			if (!strcmp(atts[i], EL_NAME) && atts[++i] && atts[i][0]) {
				char *decoded;
				int decoded_len;
				decoded = xml_utf8_decode(atts[i], strlen(atts[i]), &decoded_len, "ISO-8859-1");
				stack->varname = decoded;
				break;
			}
		}
	} else if (!strcmp(name, EL_RECORDSET)) {
		int i;

		ent.type = ST_RECORDSET;
		SET_STACK_VARNAME;
		MAKE_STD_ZVAL(ent.data);
		array_init(ent.data);

		for (i = 0; atts[i]; i++) {
			if (!strcmp(atts[i], "fieldNames") && atts[++i] && atts[i][0]) {
				zval *tmp;
				char *key;
				char *p1, *p2, *endp;
				char *decoded;
				int decoded_len;

				decoded = xml_utf8_decode(atts[i], strlen(atts[i]), &decoded_len, "ISO-8859-1");
				endp = (char *)decoded + decoded_len;
				p1 = (char *)decoded;
				while ((p2 = php_memnstr(p1, ",", sizeof(",")-1, endp)) != NULL) {
					key = estrndup(p1, p2 - p1);
					MAKE_STD_ZVAL(tmp);
					array_init(tmp);
					add_assoc_zval_ex(ent.data, key, p2 - p1 + 1, tmp);
					p1 = p2 + sizeof(",")-1;
					efree(key);
				}

				if (p1 <= endp) {
					MAKE_STD_ZVAL(tmp);
					array_init(tmp);
					add_assoc_zval_ex(ent.data, p1, endp - p1 + 1, tmp);
				}

				efree(decoded);
				break;
			}
		}

		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_FIELD)) {
		int i;
		st_entry ent;

		ent.type = ST_FIELD;
		ent.varname = NULL;
		ent.data = NULL;

		for (i = 0; atts[i]; i++) {
			if (!strcmp(atts[i], EL_NAME) && atts[++i] && atts[i][0]) {
				char *decoded;
				int decoded_len;
				st_entry *recordset;
				zval **field;
 
				decoded = xml_utf8_decode(atts[i], strlen(atts[i]), &decoded_len, "ISO-8859-1");
				if (wddx_stack_top(stack, (void**)&recordset) == SUCCESS &&
					recordset->type == ST_RECORDSET &&
					zend_hash_find(Z_ARRVAL_P(recordset->data), decoded, decoded_len+1, (void**)&field) == SUCCESS) {
					ent.data = *field;
				}
				
				efree(decoded);
				break;
			}
		}

		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	}
}
/* }}} */

/* {{{ php_wddx_pop_element
 */
static void php_wddx_pop_element(void *user_data, const char *name)
{
	st_entry 			*ent1, *ent2;
	wddx_stack 			*stack = (wddx_stack *)user_data;
	HashTable 			*target_hash;
	zend_class_entry 	*ce;
	zval				*obj;
	zval				*tmp;
	TSRMLS_FETCH();

/* OBJECTS_FIXME */
	if (stack->top == 0)
		return;

	if (!strcmp(name, EL_STRING) || !strcmp(name, EL_NUMBER) ||
		!strcmp(name, EL_BOOLEAN) || !strcmp(name, EL_NULL) ||
	  	!strcmp(name, EL_ARRAY) || !strcmp(name, EL_STRUCT) ||
		!strcmp(name, EL_RECORDSET) || !strcmp(name, EL_BINARY)) {
		wddx_stack_top(stack, (void**)&ent1);

		if (!strcmp(name, EL_BINARY)) {
			int new_len=0;
			unsigned char *new_str;

			new_str = php_base64_decode(Z_STRVAL_P(ent1->data), Z_STRLEN_P(ent1->data), &new_len);
			STR_FREE(Z_STRVAL_P(ent1->data));
			Z_STRVAL_P(ent1->data) = new_str;
			Z_STRLEN_P(ent1->data) = new_len;
		}

		/* Call __wakeup() method on the object. */
		if (Z_TYPE_P(ent1->data) == IS_OBJECT) {
			zval *fname, *retval = NULL;

			MAKE_STD_ZVAL(fname);
			ZVAL_STRING(fname, "__wakeup", 1);

			call_user_function_ex(NULL, &ent1->data, fname, &retval, 0, 0, 0, NULL TSRMLS_CC);

			zval_dtor(fname);
			FREE_ZVAL(fname);
			if (retval)
				zval_ptr_dtor(&retval);
		}

		if (stack->top > 1) {
			stack->top--;
			wddx_stack_top(stack, (void**)&ent2);
			
			/* if non-existent field */
			if (ent2->type == ST_FIELD && ent2->data == NULL) {
				zval_ptr_dtor(&ent1->data);
				efree(ent1);
				return;
			}
			
			if (Z_TYPE_P(ent2->data) == IS_ARRAY || Z_TYPE_P(ent2->data) == IS_OBJECT) {
				target_hash = HASH_OF(ent2->data);

				if (ent1->varname) {
					if (!strcmp(ent1->varname, PHP_CLASS_NAME_VAR) &&
						Z_TYPE_P(ent1->data) == IS_STRING && Z_STRLEN_P(ent1->data)) {
						zend_bool incomplete_class = 0;

						zend_str_tolower(Z_STRVAL_P(ent1->data), Z_STRLEN_P(ent1->data));
						if (zend_hash_find(EG(class_table), Z_STRVAL_P(ent1->data),
										   Z_STRLEN_P(ent1->data)+1, (void **) &ce)==FAILURE) {
							incomplete_class = 1;
							ce = PHP_IC_ENTRY;
						}

						/* Initialize target object */
						MAKE_STD_ZVAL(obj);
						INIT_PZVAL(obj);
						object_init_ex(obj, ce);
						
						/* Merge current hashtable with object's default properties */
						zend_hash_merge(Z_OBJPROP_P(obj),
										Z_ARRVAL_P(ent2->data),
										(void (*)(void *)) zval_add_ref,
										(void *) &tmp, sizeof(zval *), 0);

						if (incomplete_class) {
							php_store_class_name(obj, Z_STRVAL_P(ent1->data), Z_STRLEN_P(ent1->data));
						}

						/* Clean up old array entry */
						zval_ptr_dtor(&ent2->data);
						
						/* Set stack entry to point to the newly created object */
						ent2->data = obj;
						
						/* Clean up class name var entry */
						zval_ptr_dtor(&ent1->data);
					} else
						zend_hash_update(target_hash,
										 ent1->varname, strlen(ent1->varname)+1,
										 &ent1->data, sizeof(zval *), NULL);
					efree(ent1->varname);
				} else	{
					zend_hash_next_index_insert(target_hash,
												&ent1->data,
												sizeof(zval *), NULL);
				}
			}
			efree(ent1);
		} else
			stack->done = 1;
	} else if (!strcmp(name, EL_VAR) && stack->varname) {
		efree(stack->varname);
	} else if (!strcmp(name, EL_FIELD)) {
		st_entry *ent;
		wddx_stack_top(stack, (void **)&ent);
		efree(ent);
		stack->top--;
	}
}
/* }}} */

/* {{{ php_wddx_process_data
 */
static void php_wddx_process_data(void *user_data, const char *s, int len)
{
	st_entry *ent;
	wddx_stack *stack = (wddx_stack *)user_data;
	char *decoded;
	int decoded_len;
	TSRMLS_FETCH();

	if (!wddx_stack_is_empty(stack) && !stack->done) {
		wddx_stack_top(stack, (void**)&ent);
		switch (Z_TYPE_P(ent)) {
			case ST_STRING: 
				decoded = xml_utf8_decode(s, len, &decoded_len, "ISO-8859-1");

				if (Z_STRLEN_P(ent->data) == 0) {
					Z_STRVAL_P(ent->data) = estrndup(decoded, decoded_len);
					Z_STRLEN_P(ent->data) = decoded_len;
				} else {
					Z_STRVAL_P(ent->data) = erealloc(Z_STRVAL_P(ent->data),
							Z_STRLEN_P(ent->data) + decoded_len + 1);
					strncpy(Z_STRVAL_P(ent->data)+Z_STRLEN_P(ent->data), decoded, decoded_len);
					Z_STRLEN_P(ent->data) += decoded_len;
					Z_STRVAL_P(ent->data)[Z_STRLEN_P(ent->data)] = '\0';
				}

				efree(decoded);
				break;

			case ST_BINARY:
				if (Z_STRLEN_P(ent->data) == 0) {
					Z_STRVAL_P(ent->data) = estrndup(s, len + 1);
				} else {
					Z_STRVAL_P(ent->data) = erealloc(Z_STRVAL_P(ent->data), Z_STRLEN_P(ent->data) + len + 1);
					memcpy(Z_STRVAL_P(ent->data) + Z_STRLEN_P(ent->data), s, len);
				}
				Z_STRLEN_P(ent->data) += len;
				Z_STRVAL_P(ent->data)[Z_STRLEN_P(ent->data)] = '\0';
				break;

			case ST_NUMBER:
				Z_TYPE_P(ent->data) = IS_STRING;
				Z_STRLEN_P(ent->data) = len;
				Z_STRVAL_P(ent->data) = estrndup(s, len);
				convert_scalar_to_number(ent->data TSRMLS_CC);
				break;

			case ST_BOOLEAN:
				if (!strcmp(s, "true"))
					Z_LVAL_P(ent->data) = 1;
				else if (!strcmp(s, "false"))
					Z_LVAL_P(ent->data) = 0;
				else {
					stack->top--;
					zval_ptr_dtor(&ent->data);
					if (ent->varname)
						efree(ent->varname);
					efree(ent);
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
int php_wddx_deserialize_ex(char *value, int vallen, zval *return_value)
{
	wddx_stack stack;
	XML_Parser parser;
	st_entry *ent;
	int retval;
	
	wddx_stack_init(&stack);
	parser = XML_ParserCreate("ISO-8859-1");

	XML_SetUserData(parser, &stack);
	XML_SetElementHandler(parser, php_wddx_push_element, php_wddx_pop_element);
	XML_SetCharacterDataHandler(parser, php_wddx_process_data);
	
	XML_Parse(parser, value, vallen, 1);
	
	XML_ParserFree(parser);

	if (stack.top == 1) {
		wddx_stack_top(&stack, (void**)&ent);
		*return_value = *(ent->data);
		zval_copy_ctor(return_value);
		retval = SUCCESS;
	} else
		retval = FAILURE;
		
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
	int comment_len = 0;
	wddx_packet *packet;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|s",
							  &var, &comment, &comment_len) == FAILURE)
		return;
	
	packet = php_wddx_constructor();
	if (!packet) {
		RETURN_FALSE;
	}

	php_wddx_packet_start(packet, comment, comment_len);
	php_wddx_serialize_var(packet, var, NULL, 0);
	php_wddx_packet_end(packet);
					
	ZVAL_STRINGL(return_value, packet->c, packet->len, 1);
	smart_str_free(packet);
	efree(packet);
}
/* }}} */

/* {{{ proto string wddx_serialize_vars(mixed var_name [, mixed ...])
   Creates a new packet and serializes given variables into a struct */
PHP_FUNCTION(wddx_serialize_vars)
{
	int argc, i;
	wddx_packet *packet;
	zval ***args;
		
	argc = ZEND_NUM_ARGS();
	if (argc < 1) {
		php_error(E_WARNING, "%s() requires at least 1 argument, 0 given",
				  get_active_function_name(TSRMLS_C));
		return;
	}

	/* Allocate arguments array and get the arguments, checking for errors. */
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}
		
	packet = php_wddx_constructor();
	if (!packet) {
		RETURN_FALSE;
	}

	php_wddx_packet_start(packet, NULL, 0);
	php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);
	
	for (i=0; i<argc; i++) {
		if (Z_TYPE_PP(args[i]) != IS_ARRAY && Z_TYPE_PP(args[i]) != IS_OBJECT)
			convert_to_string_ex(args[i]);
		php_wddx_add_var(packet, *args[i]);
	}	
	
	php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);
	php_wddx_packet_end(packet);

	efree(args);

	ZVAL_STRINGL(return_value, packet->c, packet->len, 1);
	smart_str_free(packet);
	efree(packet);
}
/* }}} */

/* {{{ php_wddx_constructor
 */
wddx_packet *php_wddx_constructor(void)
{
	smart_str *packet;

	packet = (smart_str *)emalloc(sizeof(smart_str));
	packet->c = NULL;

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

/* {{{ proto int wddx_packet_start([string comment])
   Starts a WDDX packet with optional comment and returns the packet id */
PHP_FUNCTION(wddx_packet_start)
{
	char *comment = NULL;
	int comment_len = 0;
	wddx_packet *packet;

	comment = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &comment, &comment_len) == FAILURE)
		return;

	packet = php_wddx_constructor();
	if (!packet) {
		RETURN_FALSE;
	}
	
	php_wddx_packet_start(packet, comment, comment_len);
	php_wddx_add_chunk_static(packet, WDDX_STRUCT_S);

	ZEND_REGISTER_RESOURCE(return_value, packet, le_wddx);
}
/* }}} */

/* {{{ proto string wddx_packet_end(int packet_id)
   Ends specified WDDX packet and returns the string containing the packet */
PHP_FUNCTION(wddx_packet_end)
{
	zval *packet_id;
	wddx_packet *packet = NULL;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &packet_id) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(packet, wddx_packet *, &packet_id, -1, "WDDX packet ID", le_wddx);
			
	php_wddx_add_chunk_static(packet, WDDX_STRUCT_E);	
	
	php_wddx_packet_end(packet);

	ZVAL_STRINGL(return_value, packet->c, packet->len, 1);

	zend_list_delete(Z_LVAL_P(packet_id));
}
/* }}} */

/* {{{ proto int wddx_add_vars(int packet_id,  mixed var_names [, mixed ...])
   Serializes given variables and adds them to packet given by packet_id */
PHP_FUNCTION(wddx_add_vars)
{
	int argc, i;
	zval ***args;
	zval **packet_id;
	wddx_packet *packet = NULL;
	
	argc = ZEND_NUM_ARGS();
	if (argc < 2) {
		php_error(E_WARNING, "%s() requires at least 2 arguments, %d given",
				  get_active_function_name(TSRMLS_C), ZEND_NUM_ARGS());
		return;	
	}
	
	/* Allocate arguments array and get the arguments, checking for errors. */
	args = (zval ***)emalloc(argc * sizeof(zval **));
	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}
	
	packet_id = args[0];

	packet = (wddx_packet *)zend_fetch_resource(packet_id TSRMLS_CC, -1, "WDDX packet ID", NULL, 1, le_wddx);
	if (!packet)
	{
		efree(args);
		RETURN_FALSE;
	}
		
	for (i=1; i<argc; i++) {
		if (Z_TYPE_PP(args[i]) != IS_ARRAY && Z_TYPE_PP(args[i]) != IS_OBJECT)
			convert_to_string_ex(args[i]);
		php_wddx_add_var(packet, (*args[i]));
	}

	efree(args);	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed wddx_deserialize(string packet) 
   Deserializes given packet and returns a PHP value */
PHP_FUNCTION(wddx_deserialize)
{
	char *packet;
	int packet_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &packet, &packet_len) == FAILURE)
		return;

	if (packet_len == 0)
		return;
		
	php_wddx_deserialize_ex(packet, packet_len, return_value);
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
