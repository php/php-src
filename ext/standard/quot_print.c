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
	char *str_in, *str_out;
	int i = 0, j = 0;
	
    if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1,&arg1)==FAILURE) 
    {
    	WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(arg1);
    
	if((*arg1)->value.str.len == 0) {
		/* shortcut */
		RETURN_EMPTY_STRING();
	}

    str_in = (*arg1)->value.str.val;
	str_out = emalloc((*arg1)->value.str.len+1);
    while ( str_in[i] )
    {
    	if ( (str_in[i] == '=') && str_in[i+1] && str_in[i+2] &&
    	     ( isdigit((int)str_in[i+1]) || (str_in[i+1]<='F' && str_in[i+1]>='A'))
    	     &&
    	     ( isdigit((int)str_in[i+2]) || (str_in[i+2]<='F' && str_in[i+2]>='A'))
    	   )
    	{
    		str_out[j++] = (php_hex2int((int)str_in[i+1]) << 4 ) 
    		           + php_hex2int((int)str_in[i+2]);
    		i += 3;
    	}
    	else if ( str_in[i] == 13 )
    	{
    		i++;
    	}
    	else
    	{	
    		str_out[j++] = str_in[i++];
    	}
    }
    str_out[j] = '\0';
    
    RETVAL_STRINGL(str_out, j, 0);
}
/* }}} */
