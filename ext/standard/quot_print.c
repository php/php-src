/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Kirill Maximov <kir@actimind.com>                            |
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
	if (isdigit(c)) {
		return c - '0';
	}
	else if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}
	else if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	}
	else {
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
	int i = 0, j = 0, k;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
    
	if (Z_STRLEN_PP(arg1) == 0) {
		/* shortcut */
		RETURN_EMPTY_STRING();
	}

	str_in = Z_STRVAL_PP(arg1);
	str_out = emalloc(Z_STRLEN_PP(arg1) + 1);
	while (str_in[i]) {
		switch (str_in[i]) {
		case '=':
			if (str_in[i + 1] && str_in[i + 2] && 
				isxdigit((int) str_in[i + 1]) && 
				isxdigit((int) str_in[i + 2]))
			{
				str_out[j++] = (php_hex2int((int) str_in[i + 1]) << 4) 
						+ php_hex2int((int) str_in[i + 2]);
				i += 3;
			} else  /* check for soft line break according to RFC 2045*/ {
				k = 1;
				while (str_in[i + k] && ((str_in[i + k] == 32) || (str_in[i + k] == 9))) {
					/* Possibly, skip spaces/tabs at the end of line */
					k++;
				}
				if (!str_in[i + k]) {
					/* End of line reached */
					i += k;
				}
				else if ((str_in[i + k] == 13) && (str_in[i + k + 1] == 10)) {
					/* CRLF */
					i += k + 2;
				}
				else if ((str_in[i + k] == 13) || (str_in[i + k] == 10)) {
					/* CR or LF */
					i += k + 1;
				}
				else {
					str_out[j++] = str_in[i++];
				}
			}
			break;
		default:
			str_out[j++] = str_in[i++];
		}
	}
	str_out[j] = '\0';
    
	RETVAL_STRINGL(str_out, j, 0);
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
