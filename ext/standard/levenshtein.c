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
   | Author: Bjørn Borud - Guardian Networks AS <borud@guardian.no>       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include "php_string.h"

int calc_levdist(const char *s1, const char *s2) /* faster, but obfuscated */
{
	register char *p1,*p2;
	register int i,j,n;
	int l1=0,l2=0;
	char r[512];

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
		(long)s1 ^= (long)s2; (long)s2 ^= (long)s1; (long)s1 ^= (long)s2;
		l1 ^= l2; l2 ^= l1; l1 ^= l2;
	}

	
	/* fill initial row */
	n=(*s1!=*s2);
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
			if(*++p1<n) n=*p1+1; /* insert cheaper then insert ? */
			*p2++=n++; /* update field and cost for next col's delete */
			p2++;
		}	
	}

	/* return result */
	return n-1;
}


/* {{{ proto int levenshtein(string str1, string str2)
   Calculate Levenshtein distance between two strings */
PHP_FUNCTION(levenshtein){
	zval **str1, **str2;
	int l;

	if (ARG_COUNT(ht) != 2 || zend_get_parameters_ex(2, &str1, &str2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str1);
	convert_to_string_ex(str2);
	
	l = calc_levdist((*str1)->value.str.val, (*str2)->value.str.val);

	if(l<0) {
		php_error(E_WARNING,"levenshtein(): argument string(s) to long");
	}

	RETURN_LONG(l);
}
/* }}} */

