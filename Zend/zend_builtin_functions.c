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

static ZEND_FUNCTION(zend_version);
static ZEND_FUNCTION(zend_num_args);
static ZEND_FUNCTION(zend_get_arg);
static ZEND_FUNCTION(strlen);
static ZEND_FUNCTION(strcmp);
static ZEND_FUNCTION(strcasecmp);
static ZEND_FUNCTION(each);
static ZEND_FUNCTION(error_reporting);

static zend_function_entry builtin_functions[] = {
	ZEND_FE(zend_version,		NULL)
	ZEND_FE(zend_num_args,		NULL)
	ZEND_FE(zend_get_arg,		NULL)
	ZEND_FE(strlen,				NULL)
	ZEND_FE(strcmp,				NULL)
	ZEND_FE(strcasecmp,			NULL)
	ZEND_FE(each,				NULL)
	ZEND_FE(error_reporting,	NULL)
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
