/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   +----------------------------------------------------------------------+
 */

#ifndef MSVC5
#include "config.h"
#include "build-defs.h"
#endif

#include "php.h"


#if WITH_BCMATH

#include "number.h"
#include "php3_bcmath.h"

function_entry bcmath_functions[] = {
	{"bcadd",		php3_bcmath_add,			NULL},
	{"bcsub",		php3_bcmath_sub,			NULL},
	{"bcmul",		php3_bcmath_mul,			NULL},
	{"bcdiv",		php3_bcmath_div,			NULL},
	{"bcmod",		php3_bcmath_mod,			NULL},
	{"bcpow",		php3_bcmath_pow,			NULL},
	{"bcsqrt",		php3_bcmath_sqrt,			NULL},
	{"bcscale",		php3_bcmath_set_scale,		NULL},
	{"bccomp",		php3_bcmath_comp,			NULL},
	{NULL, NULL, NULL}
};

php3_module_entry bcmath_module_entry = {
	"bcmath", bcmath_functions, NULL, NULL, php3_rinit_bcmath, php3_rend_bcmath, NULL, STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL
php3_module_entry *get_module() { return &bcmath_module_entry; };
#endif

#ifndef THREAD_SAFE
static long bc_precision;
#endif

int php3_rinit_bcmath(INIT_FUNC_ARGS)
{
	TLS_VARS;

	init_numbers();
	if (cfg_get_long("bcmath.scale",&GLOBAL(bc_precision))==FAILURE) {
		GLOBAL(bc_precision)=0;
	}
	return SUCCESS;
}

int php3_rend_bcmath(SHUTDOWN_FUNC_ARGS)
{
	destruct_numbers();
	return SUCCESS;
}

/* {{{ proto string bcadd(string left_operand, string right_operand [, int scale])
   Returns the sum of two arbitrary precision numbers */
void php3_bcmath_add(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *left, *right,*scale_param;
	bc_num first, second, result;
	int scale=GLOBAL(bc_precision);

	switch (ARG_COUNT(ht)) {
		case 2:
				if (getParameters(ht, 2, &left, &right) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		case 3:
				if (getParameters(ht, 3, &left, &right, &scale_param) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				convert_to_long(scale_param);
				scale = (int) scale_param->value.lval;
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}
	convert_to_string(left);
	convert_to_string(right);
	init_num(&first);
	init_num(&second);
	init_num(&result);
	str2num(&first,left->value.str.val,scale);
	str2num(&second,right->value.str.val,scale);
	bc_add (first,second,&result, scale);
	return_value->value.str.val = num2str(result);
	return_value->value.str.len = strlen(return_value->value.str.val);
	return_value->type = IS_STRING;
	free_num(&first);
	free_num(&second);
	free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcsub(string left_operand, string right_operand [, int scale])
   Returns the difference between two arbitrary precision numbers (subtration) */
void php3_bcmath_sub(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *left, *right,*scale_param;
	bc_num first, second, result;
	int scale=GLOBAL(bc_precision);

	switch (ARG_COUNT(ht)) {
		case 2:
				if (getParameters(ht, 2, &left, &right) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		case 3:
				if (getParameters(ht, 3, &left, &right, &scale_param) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				convert_to_long(scale_param);
				scale = (int) scale_param->value.lval;
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}
	convert_to_string(left);
	convert_to_string(right);
	init_num(&first);
	init_num(&second);
	init_num(&result);
	str2num(&first,left->value.str.val,scale);
	str2num(&second,right->value.str.val,scale);
	bc_sub (first,second,&result, scale);
	return_value->value.str.val = num2str(result);
	return_value->value.str.len = strlen(return_value->value.str.val);
	return_value->type = IS_STRING;
	free_num(&first);
	free_num(&second);
	free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcmul(string left_operand, string right_operand [, int scale])
   Returns the multiplication of two arbitrary precision numbers */
void php3_bcmath_mul(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *left, *right,*scale_param;
	bc_num first, second, result;
	int scale=GLOBAL(bc_precision);

	switch (ARG_COUNT(ht)) {
		case 2:
				if (getParameters(ht, 2, &left, &right) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		case 3:
				if (getParameters(ht, 3, &left, &right, &scale_param) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				convert_to_long(scale_param);
				scale = (int) scale_param->value.lval;
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}
	convert_to_string(left);
	convert_to_string(right);
	init_num(&first);
	init_num(&second);
	init_num(&result);
	str2num(&first,left->value.str.val,scale);
	str2num(&second,right->value.str.val,scale);
	bc_multiply (first,second,&result, scale);
	return_value->value.str.val = num2str(result);
	return_value->value.str.len = strlen(return_value->value.str.val);
	return_value->type = IS_STRING;
	free_num(&first);
	free_num(&second);
	free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcdiv(string left_operand, string right_operand [, int scale])
   Returns the quotient of two arbitrary precision numbers (division) */
void php3_bcmath_div(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *left, *right,*scale_param;
	bc_num first, second, result;
	int scale=GLOBAL(bc_precision);

	switch (ARG_COUNT(ht)) {
		case 2:
				if (getParameters(ht, 2, &left, &right) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		case 3:
				if (getParameters(ht, 3, &left, &right, &scale_param) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				convert_to_long(scale_param);
				scale = (int) scale_param->value.lval;
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}
	convert_to_string(left);
	convert_to_string(right);
	init_num(&first);
	init_num(&second);
	init_num(&result);
	str2num(&first,left->value.str.val,scale);
	str2num(&second,right->value.str.val,scale);
	switch (bc_divide (first,second,&result, scale)) {
		case 0: /* OK */
			return_value->value.str.val = num2str(result);
			return_value->value.str.len = strlen(return_value->value.str.val);
			return_value->type = IS_STRING;
			break;
		case -1: /* division by zero */
			php3_error(E_WARNING,"Division by zero");
			break;
	}
	free_num(&first);
	free_num(&second);
	free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcmod(string left_operand, string right_operand)
   Returns the modulus of the two arbitrary precision operands */
void php3_bcmath_mod(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *left, *right;
	bc_num first, second, result;

	switch (ARG_COUNT(ht)) {
		case 2:
				if (getParameters(ht, 2, &left, &right) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}
	convert_to_string(left);
	convert_to_string(right);
	init_num(&first);
	init_num(&second);
	init_num(&result);
	str2num(&first,left->value.str.val,0);
	str2num(&second,right->value.str.val,0);
	switch (bc_modulo(first,second,&result, 0)) {
		case 0:
			return_value->value.str.val = num2str(result);
			return_value->value.str.len = strlen(return_value->value.str.val);
			return_value->type = IS_STRING;
			break;
		case -1:
			php3_error(E_WARNING,"Division by zero");
			break;
	}
	free_num(&first);
	free_num(&second);
	free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcpow(string x, string y [, int scale])
   Returns the value of an arbitrary precision number raised to the power of another */
void php3_bcmath_pow(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *left, *right,*scale_param;
	bc_num first, second, result;
	int scale=GLOBAL(bc_precision);

	switch (ARG_COUNT(ht)) {
		case 2:
				if (getParameters(ht, 2, &left, &right) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		case 3:
				if (getParameters(ht, 3, &left, &right, &scale_param) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				convert_to_long(scale_param);
				scale = (int) scale_param->value.lval;
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}
	convert_to_string(left);
	convert_to_string(right);
	init_num(&first);
	init_num(&second);
	init_num(&result);
	str2num(&first,left->value.str.val,scale);
	str2num(&second,right->value.str.val,scale);
	bc_raise (first,second,&result, scale); 
	return_value->value.str.val = num2str(result);
	return_value->value.str.len = strlen(return_value->value.str.val);
	return_value->type = IS_STRING;
	free_num(&first);
	free_num(&second);
	free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bcsqrt(string operand [, int scale])
   Returns the square root of an arbitray precision number */
void php3_bcmath_sqrt(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *left,*scale_param;
	bc_num result;
	int scale=GLOBAL(bc_precision);

	switch (ARG_COUNT(ht)) {
		case 1:
				if (getParameters(ht, 1, &left) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		case 2:
				if (getParameters(ht, 2, &left, &scale_param) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				convert_to_long(scale_param);
				scale = (int) scale_param->value.lval;
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}
	convert_to_string(left);
	init_num(&result);
	str2num(&result,left->value.str.val,scale);
	if (bc_sqrt (&result, scale) != 0) {
		return_value->value.str.val = num2str(result);
		return_value->value.str.len = strlen(return_value->value.str.val);
		return_value->type = IS_STRING;
	} else {
		php3_error(E_WARNING,"Square root of negative number");
	}
	free_num(&result);
	return;
}
/* }}} */

/* {{{ proto string bccomp(string left_operand, string right_operand [, int scale])
   Compares two arbitrary precision numbers */
void php3_bcmath_comp(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *left, *right, *scale_param;
	bc_num first, second;
	int scale=GLOBAL(bc_precision);

	switch (ARG_COUNT(ht)) {
		case 2:
				if (getParameters(ht, 2, &left, &right) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				break;
		case 3:
				if (getParameters(ht, 3, &left, &right, &scale_param) == FAILURE) {
		        	WRONG_PARAM_COUNT;
    			}
				convert_to_long(scale_param);
				scale = (int) scale_param->value.lval;
				break;
		default:
				WRONG_PARAM_COUNT;
				break;
	}

	convert_to_string(left);
	convert_to_string(right);
	init_num(&first);
	init_num(&second);

	str2num(&first,left->value.str.val,scale);
	str2num(&second,right->value.str.val,scale);
	return_value->value.lval = bc_compare(first,second);
	return_value->type = IS_LONG;

	free_num(&first);
	free_num(&second);
	return;
}
/* }}} */

/* {{{ proto string bcscale(int scale)
   Sets default scale parameter for all bc math functions */
void php3_bcmath_set_scale(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *new_scale;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &new_scale)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(new_scale);
	GLOBAL(bc_precision) = new_scale->value.lval;
	RETURN_TRUE;
}
/* }}} */


#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
