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
   | Authors: Kirill Maximov (kir@rus.net				                  |
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
static char _php3_hex2int(int c)
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
	pval *arg1;
	char *str;
	int i = 0, j = 0;
	
    if (ARG_COUNT(ht) != 1 || getParameters(ht,1,&arg1)==FAILURE) 
    {
    	WRONG_PARAM_COUNT;
    }
    convert_to_string(arg1);
    
    str = arg1->value.str.val;
    while ( str[i] )
    {
    	if ( (str[i] == '=') && str[i+1] && str[i+2] &&
    	     ( isdigit((int)str[i+1]) || (str[i+1]<='F' && str[i+1]>='A'))
    	     &&
    	     ( isdigit((int)str[i+2]) || (str[i+2]<='F' && str[i+2]>='A'))
    	   )
    	{
    		str[j++] = (_php3_hex2int((int)str[i+1]) << 4 ) 
    		           + _php3_hex2int((int)str[i+2]);
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
