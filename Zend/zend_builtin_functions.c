/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.91 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_91.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#include "zend.h"
#include "zend_API.h"
#include "zend_builtin_functions.h"
#include "zend_operators.h"
#include "zend_variables.h"
#include "zend_constants.h"

static ZEND_FUNCTION(zend_version);
static ZEND_FUNCTION(zend_num_args);
static ZEND_FUNCTION(zend_get_arg);
static ZEND_FUNCTION(strlen);
static ZEND_FUNCTION(strcmp);
static ZEND_FUNCTION(strcasecmp);
static ZEND_FUNCTION(each);
static ZEND_FUNCTION(error_reporting);
static ZEND_FUNCTION(define);
static ZEND_FUNCTION(defined);
static ZEND_FUNCTION(get_class);
static ZEND_FUNCTION(get_parent_class);
static ZEND_FUNCTION(method_exists);
static ZEND_FUNCTION(leak);

static zend_function_entry builtin_functions[] = {
	ZEND_FE(zend_version,		NULL)
	ZEND_FE(zend_num_args,		NULL)
	ZEND_FE(zend_get_arg,		NULL)
	ZEND_FE(strlen,				NULL)
	ZEND_FE(strcmp,				NULL)
	ZEND_FE(strcasecmp,			NULL)
	ZEND_FE(each,				NULL)
	ZEND_FE(error_reporting,	NULL)
	ZEND_FE(define,				NULL)
	ZEND_FE(defined,			NULL)
	ZEND_FE(get_class,			NULL)
	ZEND_FE(get_parent_class,	NULL)
	ZEND_FE(method_exists,		NULL)
	ZEND_FE(leak,				NULL)
	{ NULL, NULL, NULL }
};


int zend_startup_builtin_functions()
{
	return zend_register_functions(builtin_functions, NULL);
}


ZEND_FUNCTION(zend_version)
{
	RETURN_STRINGL(ZEND_VERSION, sizeof(ZEND_VERSION)-1, 1);
}


ZEND_FUNCTION(zend_num_args)
{
	void **p;
	int arg_count;
	ELS_FETCH();

	p = EG(argument_stack).top_element-1;
	arg_count = (ulong) *p;		/* this is the amount of arguments passed to zend_num_args(); */

	p = EG(argument_stack).top_element-1-arg_count-1;
	if (p>=EG(argument_stack).elements) {
		RETURN_LONG((ulong) *p);
	} else {
		zend_error(E_WARNING, "zend_num_args():  Called from the global scope - no function context");
		RETURN_LONG(-1);
	}
}


ZEND_FUNCTION(zend_get_arg)
{
	void **p;
	int arg_count;
	zval **z_requested_offset;
	zval *arg;
	long requested_offset;
	ELS_FETCH();

	if (ARG_COUNT(ht)!=1 || getParametersEx(1, &z_requested_offset)==FAILURE) {
		RETURN_FALSE;
	}
	convert_to_long_ex(z_requested_offset);
	requested_offset = (*z_requested_offset)->value.lval;

	p = EG(argument_stack).top_element-1;
	arg_count = (ulong) *p;		/* this is the amount of arguments passed to zend_num_args(); */

	p = EG(argument_stack).top_element-1-arg_count-1;
	if (p<EG(argument_stack).elements) {
		RETURN_FALSE;
	}
	arg_count = (ulong) *p;

	if (requested_offset>arg_count) {
		zend_error(E_WARNING, "zend_get_arg():  Only %d arguments passed to function (argument %d requested)", arg_count, requested_offset);
		RETURN_FALSE;
	}

	arg = *(p-(arg_count-requested_offset));
	*return_value = *arg;
	zval_copy_ctor(return_value);
}

/* {{{ proto int strlen(string str)
   Get string length */
ZEND_FUNCTION(strlen)
{
	zval **str;
	
	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);
	RETVAL_LONG((*str)->value.str.len);
}
/* }}} */

/* {{{ proto int strcmp(string str1, string str2)
   Binary safe string comparison */
ZEND_FUNCTION(strcmp)
{
	zval *s1,*s2;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &s1, &s2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(s1);
	convert_to_string(s2);
	RETURN_LONG(zend_binary_strcmp(s1,s2));
}
/* }}} */

/* {{{ proto int strcasecmp(string str1, string str2)
   Binary safe case-insensitive string comparison */
ZEND_FUNCTION(strcasecmp)
{
	zval *s1,*s2;
	
	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &s1, &s2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(s1);
	convert_to_string(s2);
	RETURN_LONG(zend_binary_strcasecmp(s1, s2));
}
/* }}} */

ZEND_FUNCTION(each)
{
	zval *array,*entry,**entry_ptr, *tmp;
	char *string_key;
	ulong num_key;
	zval **inserted_pointer;
	HashTable *target_hash;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	target_hash = HASH_OF(array);
	if (!target_hash) {
		zend_error(E_WARNING,"Variable passed to each() is not an array or object");
		return;
	}
	if (zend_hash_get_current_data(target_hash, (void **) &entry_ptr)==FAILURE) {
		RETURN_FALSE;
	}
	array_init(return_value);
	entry = *entry_ptr;

	/* add value elements */
	if (entry->EA.is_ref) {
		tmp = (zval *)emalloc(sizeof(zval));
		*tmp = *entry;
		zval_copy_ctor(tmp);
		tmp->EA.is_ref=0;
		tmp->EA.locks = 0;
		tmp->refcount=0;
		entry=tmp;
	}
	zend_hash_index_update(return_value->value.ht, 1, &entry, sizeof(zval *), NULL);
	entry->refcount++;
	zend_hash_update_ptr(return_value->value.ht, "value", sizeof("value"), entry, sizeof(zval *), NULL);
	entry->refcount++;

	/* add the key elements */
	switch (zend_hash_get_current_key(target_hash, &string_key, &num_key)) {
		case HASH_KEY_IS_STRING:
			add_get_index_string(return_value,0,string_key,(void **) &inserted_pointer,0);
			break;
		case HASH_KEY_IS_LONG:
			add_get_index_long(return_value,0,num_key, (void **) &inserted_pointer);
			break;
	}
	zend_hash_update(return_value->value.ht, "key", sizeof("key"), inserted_pointer, sizeof(zval *), NULL);
	(*inserted_pointer)->refcount++;
	zend_hash_move_forward(target_hash);
}

ZEND_FUNCTION(error_reporting)
{
	zval *arg;
	int old_error_reporting;
	ELS_FETCH();

	old_error_reporting = EG(error_reporting);
	switch (ARG_COUNT(ht)) {
		case 0:
			break;
		case 1:
			if (getParameters(ht,1,&arg) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(arg);
			EG(error_reporting)=arg->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	RETVAL_LONG(old_error_reporting);
}

ZEND_FUNCTION(define)
{
	zval *var, *val, *non_cs;
	int case_sensitive;
	zend_constant c;
	ELS_FETCH();
	
	switch(ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &var, &val)==FAILURE) {
				RETURN_FALSE;
			}
			case_sensitive = CONST_CS;
			break;
		case 3:
			if (getParameters(ht, 3, &var, &val, &non_cs)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(non_cs);
			if (non_cs->value.lval) {
				case_sensitive = 0;
			} else {
				case_sensitive = CONST_CS;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	switch(val->type) {
		case IS_LONG:
		case IS_DOUBLE:
		case IS_STRING:
		case IS_BOOL:
		case IS_RESOURCE:
			break;
		default:
			zend_error(E_WARNING,"Constants may only evaluate to scalar values");
			RETURN_FALSE;
			break;
	}
	convert_to_string(var);
	
	c.value = *val;
	zval_copy_ctor(&c.value);
	c.flags = case_sensitive | ~CONST_PERSISTENT; /* non persistent */
	c.name = zend_strndup(var->value.str.val, var->value.str.len);
	c.name_len = var->value.str.len+1;
	zend_register_constant(&c ELS_CC);
	RETURN_TRUE;
}


ZEND_FUNCTION(defined)
{
	zval *var;
	zval c;
		
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &var)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(var);
	if (zend_get_constant(var->value.str.val, var->value.str.len, &c)) {
		zval_dtor(&c);
		RETURN_LONG(1);
	} else {
		RETURN_LONG(0);
	}
}

/* {{{ proto string get_class(object object)
     Retrieves the class name ...
*/
ZEND_FUNCTION(get_class)
{
	zval *arg;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &arg)==FAILURE) {
		RETURN_FALSE;
	}
	if (arg->type != IS_OBJECT) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(arg->value.obj.ce->name,	arg->value.obj.ce->name_length, 1);
}
/* }}} */

/* {{{ proto string get_parent_class(object object)
     Retrieves the parent class name ...
*/
ZEND_FUNCTION(get_parent_class)
{
	zval *arg;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &arg)==FAILURE) {
		RETURN_FALSE;
	}
	if ((arg->type != IS_OBJECT) || !arg->value.obj.ce->parent) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(arg->value.obj.ce->parent->name,	arg->value.obj.ce->parent->name_length, 1);
}
/* }}} */

/* {{{ proto bool method_exists(object object, string method)
     Checks if the class method exists ...
*/
ZEND_FUNCTION(method_exists)
{
	zval *arg1, *arg2;
	
	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &arg1, &arg2)==FAILURE) {
		RETURN_FALSE;
	}
	if (arg1->type != IS_OBJECT) {
		RETURN_FALSE;
	}
	convert_to_string(arg2);
	if(zend_hash_exists(&arg1->value.obj.ce->function_table, arg2->value.str.val, arg2->value.str.len+1)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

ZEND_FUNCTION(leak)
{
	int leakbytes=3;
	zval *leak;

	if (ARG_COUNT(ht)>=1) {
		if (getParameters(ht, 1, &leak)==SUCCESS) {
			convert_to_long(leak);
			leakbytes = leak->value.lval;
		}
	}
	
	emalloc(leakbytes);
}
