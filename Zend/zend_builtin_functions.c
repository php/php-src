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

static zend_function_entry builtin_functions[] = {
	ZEND_FE(zend_version,		NULL)
	ZEND_FE(zend_num_args,		NULL)
	ZEND_FE(zend_get_arg,		NULL)
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

