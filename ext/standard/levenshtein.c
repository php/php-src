/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
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

/* faster, but obfuscated, all operations have a cost of 1 */
static int fastest_levdist(const char *s1, const char *s2) 
{
	register char *p1,*p2; 
	register int i,j,n;
	int l1=0,l2=0;
	char r[512];
	const char *tmp;

	/* skip equal start sequence, if any */
	while(*s1==*s2) {
		if(!*s1) break;
		s1++; s2++;
	}
	
	/* if we already used up one string, then
      the result is the length of the other */
	if(*s1=='\0') return strlen(s2);
	if(*s2=='\0') return strlen(s1);

	/* length count */
	while(*s1++) l1++;
	while(*s2++) l2++;
	
	/* cut of equal tail sequence, if any */
	while(*--s1 == *--s2) {
		l1--; l2--;		
	}
	

	/* reset pointers, adjust length */
	s1-=l1++;
	s2-=l2++;
	

	/* possible dist to great? */
 	if(abs(l1-l2)>=255) return -1;

	/* swap if l2 longer than l1 */
	if(l1<l2) {
		tmp=s1; s1=s2; s2=tmp;
		l1 ^= l2; l2 ^= l1; l1 ^= l2;
	}

	
	/* fill initial row */
	n=1;
	for(i=0,p1=r;i<l1;i++,*p1++=n++,p1++) {/*empty*/}
	
	/* calc. rowwise */
	for(j=1;j<l2;j++) {
		/* init pointers and col#0 */
		p1 = r + !(j&1);
		p2 = r + (j&1);
		n=*p1+1;
		*p2++=n;p2++;
		s2++;
		
		/* foreach column */
		for(i=1;i<l1;i++) {
			if(*p1<n) n=*p1+(*(s1+i)!=*(s2)); /* replace cheaper than delete? */
			p1++;
			if(*++p1<n) n=*p1+1; /* insert cheaper then replace ? */
			*p2++=n++; /* update field and cost for next col's delete */
			p2++;
		}	
	}

	/* return result */
	return n-1;
}


static int weighted_levdist( const char *s1
														 , const char *s2
														 , const int cost_ins
														 , const int cost_rep
														 , const int cost_del
														 ) 
{
	register int *p1,*p2; 
	register int i,j,n,c;
	int l1=0,l2=0;
	int r[512];
	const char *tmp;

	/* skip equal start sequence, if any */
	while(*s1==*s2) {
		if(!*s1) break;
		s1++; s2++;
	}
	
	/* if we already used up one string, then
      the result is the length of the other */
	if(*s1=='\0') return strlen(s2);
	if(*s2=='\0') return strlen(s1);

	/* length count */
	while(*s1++) l1++;
	while(*s2++) l2++;
	
	/* cut of equal tail sequence, if any */
	while(*--s1 == *--s2) {
		l1--; l2--;		
	}
	

	/* reset pointers, adjust length */
	s1-=l1++;
	s2-=l2++;
	

	/* possible dist to great? */
 	if(abs(l1-l2)>=255) return -1;

	/* swap if l2 longer than l1 */
	if(l1<l2) {
		tmp=s1; s1=s2; s2=tmp;
		l1 ^= l2; l2 ^= l1; l1 ^= l2;
	}

	if((l1==1)&&(l2==1)) {
		n= cost_del+cost_ins;
		return n<cost_rep?n:cost_rep;
	}

	/* fill initial row */
	n=cost_ins;
	for(i=0,p1=r;i<l1;i++,*p1++=n,p1++) {n+=cost_ins;}
	
	/* calc. rowwise */
	for(j=1;j<l2;j++) {
		/* init pointers and col#0 */
		p1 = r + !(j&1);
		p2 = r + (j&1);
		n=*p1+cost_del;
		*p2++=n;p2++;
		s2++;
		
		/* foreach column */
		for(i=1;i<l1;i++) {
			c = *p1; if(*(s1+i)!=*(s2)) c+=cost_rep;
			if(c<n) n=c; /* replace cheaper than delete? */
			p1++;
			c= *++p1+cost_ins;
			if(c<n) n=c; /* insert cheaper then replace ? */
			*p2++=n; /* update field and cost for next col's delete */
			n+=cost_del; /* update field and cost for next col's delete */
			p2++;
		}	
	}

	/* return result */
	return n-=cost_del;
}

int custom_levdist(char *str1,char *str2,char *callback_name) 
{
		php_error(E_WARNING,"the general Levenshtein support is not there yet");
		/* not there yet */

		return -1;
}


/* {{{ proto int levenshtein(string str1, string str2)
   Calculate Levenshtein distance between two strings */
PHP_FUNCTION(levenshtein)
{
	zval **str1, **str2, **cost_ins, **cost_rep, **cost_del,**callback_name;
	int distance=-1;

	switch(ZEND_NUM_ARGS()) {
	case 2: /* just two string: use maximum performance version  */
		if (zend_get_parameters_ex(2, &str1, &str2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(str1);
		convert_to_string_ex(str2);
		
		distance = fastest_levdist((*str1)->value.str.val, (*str2)->value.str.val);
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
		
		distance = weighted_levdist((*str1)->value.str.val
																, (*str2)->value.str.val
																, (*cost_ins)->value.lval
																, (*cost_rep)->value.lval
																, (*cost_del)->value.lval
																);
		
		break;

	case 3: /* most general version: calc cost by user-supplied function */
		if (zend_get_parameters_ex(3, &str1, &str2, &callback_name) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(str1);
		convert_to_string_ex(str2);
		convert_to_string_ex(callback_name);

		distance = custom_levdist((*str1)->value.str.val
																, (*str2)->value.str.val
																, (*callback_name)->value.str.val
																);
		break;

	default: 
		WRONG_PARAM_COUNT;
	}	

	if(distance<0) {
		php_error(E_WARNING,"levenshtein(): argument string(s) too long");
	}
	
	RETURN_LONG(distance);
}
/* }}} */
