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
   | Author: Hartmut Holzgraefe <hartmut@six.de>                          |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include "php_string.h"

#define LEVENSHTEIN_MAX_LENTH 255

/* {{{ reference_levdist
 * reference implementation, only optimized for memory usage, not speed */
static int reference_levdist(const char *s1, int l1, 
														 const char *s2, int l2, 
														 int cost_ins, int cost_rep, int cost_del )
{
	int *p1, *p2, *tmp;
	int i1, i2, c0, c1, c2;
	
	if(l1==0) return l2*cost_ins;
	if(l2==0) return l1*cost_del;

	if((l1>LEVENSHTEIN_MAX_LENTH)||(l2>LEVENSHTEIN_MAX_LENTH))
		return -1;

	if(!(p1=emalloc((l2+1)*sizeof(int)))) {
		return -2;
	}
	if(!(p2=emalloc((l2+1)*sizeof(int)))) {
		free(p1);
		return -2;
	}

	for(i2=0;i2<=l2;i2++)
		p1[i2] = i2*cost_ins;

	for(i1=0;i1<l1;i1++)
		{
			p2[0]=p1[0]+cost_del;
			for(i2=0;i2<l2;i2++)
				{
					c0=p1[i2]+((s1[i1]==s2[i2])?0:cost_rep);
					c1=p1[i2+1]+cost_del; if(c1<c0) c0=c1;
					c2=p2[i2]+cost_ins; if(c2<c0) c0=c2;				
					p2[i2+1]=c0;
				}
			tmp=p1; p1=p2; p2=tmp;
		}

	c0=p1[l2];

	efree(p1);
	efree(p2);

	return c0;
}
/* }}} */

/* {{{ custom_levdist
 */
static int custom_levdist(char *str1, char *str2, char *callback_name) 
{
		php_error(E_WARNING, "the general Levenshtein support is not there yet");
		/* not there yet */

		return -1;
}
/* }}} */

/* {{{ proto int levenshtein(string str1, string str2)
   Calculate Levenshtein distance between two strings */
PHP_FUNCTION(levenshtein)
{
	zval **str1, **str2, **cost_ins, **cost_rep, **cost_del, **callback_name;
	int distance=-1;

	switch(ZEND_NUM_ARGS()) {
	case 2: /* just two string: use maximum performance version  */
		if (zend_get_parameters_ex(2, &str1, &str2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(str1);
		convert_to_string_ex(str2);

		distance = reference_levdist(Z_STRVAL_PP(str1), Z_STRLEN_PP(str1), 
									 Z_STRVAL_PP(str2), Z_STRLEN_PP(str2), 1, 1, 1);

		break;

	case 5: /* more general version: calc cost by ins/rep/del weights */
		if (zend_get_parameters_ex(5, &str1, &str2, &cost_ins, &cost_rep, &cost_del) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(str1);
		convert_to_string_ex(str2);
		convert_to_long_ex(cost_ins);
		convert_to_long_ex(cost_rep);
		convert_to_long_ex(cost_del);
		
		distance = reference_levdist(Z_STRVAL_PP(str1), Z_STRLEN_PP(str1),
									 Z_STRVAL_PP(str2), Z_STRLEN_PP(str2),
									 Z_LVAL_PP(cost_ins), Z_LVAL_PP(cost_rep),
									 Z_LVAL_PP(cost_del));
		
		break;

	case 3: /* most general version: calc cost by user-supplied function */
		if (zend_get_parameters_ex(3, &str1, &str2, &callback_name) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(str1);
		convert_to_string_ex(str2);
		convert_to_string_ex(callback_name);

		distance = custom_levdist(Z_STRVAL_PP(str1), Z_STRVAL_PP(str2),
								  Z_STRVAL_PP(callback_name));
		break;

	default: 
		WRONG_PARAM_COUNT;
	}	

	if(distance<0) {
		php_error(E_WARNING, "%s(): Argument string(s) too long", get_active_function_name(TSRMLS_C));
	}
	
	RETURN_LONG(distance);
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
