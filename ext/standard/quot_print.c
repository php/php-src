/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Kirill Maximov <kir@rus.net>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <errno.h>

#include "php.h"
#include "quot_print.h"

#include <stdio.h>

/*
*  Converting HEX char to INT value
*/
static char php_hex2int(int c)
{
	if ( isdigit(c) )
	{
		return c - '0';
	}
	else if ( c >= 'A' && c <= 'F' )
	{
		return c - 'A' + 10;
	}
	else
	{
		return -1;
	}
}

/*
*
* Decoding  Quoted-printable string.
*
*/
/* {{{ proto string quoted_printable_decode(string str)
   Convert a quoted-printable string to an 8 bit string */
PHP_FUNCTION(quoted_printable_decode)
{
	pval **arg1;
	char *str;
	int i = 0, j = 0;
	
    if (ARG_COUNT(ht) != 1 || zend_get_parameters_ex(1,&arg1)==FAILURE) 
    {
    	WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(arg1);
    
    str = (*arg1)->value.str.val;
    while ( str[i] )
    {
    	if ( (str[i] == '=') && str[i+1] && str[i+2] &&
    	     ( isdigit((int)str[i+1]) || (str[i+1]<='F' && str[i+1]>='A'))
    	     &&
    	     ( isdigit((int)str[i+2]) || (str[i+2]<='F' && str[i+2]>='A'))
    	   )
    	{
    		str[j++] = (php_hex2int((int)str[i+1]) << 4 ) 
    		           + php_hex2int((int)str[i+2]);
    		i += 3;
    	}
    	else if ( str[i] == 13 )
    	{
    		i++;
    	}
    	else
    	{	
    		str[j++] = str[i++];
    	}
    }
    str[j] = '\0';
    
    RETVAL_STRINGL(str, j, 1)
}
/* }}} */
