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
   | Author: Jim Winstead (jimw@php.net)                                  |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include <string.h>

#include "php.h"
#include "base64.h"

static char base64_table[] =
	{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
	};
static char base64_pad = '=';

unsigned char *_php3_base64_encode(const unsigned char *string, int length, int *ret_length) {
	const unsigned char *current = string;
	int i = 0;
	unsigned char *result = (unsigned char *)emalloc(((length + 3 - length % 3) * 4 / 3 + 1) * sizeof(char));

	while (length > 2) { /* keep going until we have less than 24 bits */
		result[i++] = base64_table[current[0] >> 2];
		result[i++] = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
		result[i++] = base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
		result[i++] = base64_table[current[2] & 0x3f];

		current += 3;
		length -= 3; /* we just handle 3 octets of data */
	}

	/* now deal with the tail end of things */
	if (length != 0) {
		result[i++] = base64_table[current[0] >> 2];
		if (length > 1) {
			result[i++] = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
			result[i++] = base64_table[(current[1] & 0x0f) << 2];
			result[i++] = base64_pad;
		}
		else {
			result[i++] = base64_table[(current[0] & 0x03) << 4];
			result[i++] = base64_pad;
			result[i++] = base64_pad;
		}
	}
	if(ret_length) {
		*ret_length = i;
	}
	result[i] = '\0';
	return result;
}

/* as above, but backwards. :) */
unsigned char *_php3_base64_decode(const unsigned char *string, int length, int *ret_length) {
	const unsigned char *current = string;
	int ch, i = 0, j = 0, k;

	unsigned char *result = (unsigned char *)emalloc((length / 4 * 3 + 1) * sizeof(char));
	if (result == NULL) {
		return NULL;
	}

	/* run through the whole string, converting as we go */
	while ((ch = *current++) != '\0') {
		if (ch == base64_pad) break;
		ch = (int)strchr(base64_table, ch);
		if (ch == 0) {
			efree(result);
			return NULL;
		}
		ch -= (int)base64_table;

		switch(i % 4) {
		case 0:
			result[j] = ch << 2;
			break;
		case 1:
			result[j++] |= ch >> 4;
			result[j] = (ch & 0x0f) << 4;
			break;
		case 2:
			result[j++] |= ch >>2;
			result[j] = (ch & 0x03) << 6;
			break;
		case 3:
			result[j++] |= ch;
			break;
		}
		i++;
	}

	k = j;
	/* mop things up if we ended on a boundary */
	if (ch == base64_pad) {
		switch(i % 4) {
		case 0:
		case 1:
			efree(result);
			return NULL;
		case 2:
			k++;
		case 3:
			result[k++] = 0;
		}
	}
	if(ret_length) {
		*ret_length = j;
	}
	result[k] = '\0';
	return result;
}

/* {{{ proto string base64_encode(string str)
   Encodes string using MIME base64 algorithm */
void php3_base64_encode(INTERNAL_FUNCTION_PARAMETERS) {
	pval *string;
	unsigned char *result;
	int ret_length;
	TLS_VARS;

	if (ARG_COUNT(ht)!=1 || getParameters(ht,1,&string) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(string);
	result = _php3_base64_encode(string->value.str.val, string->value.str.len, &ret_length);
	if (result != NULL) {
		return_value->value.str.val = result;
		return_value->value.str.len = ret_length;
		return_value->type = IS_STRING;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto string base64_decode(string str)
   Decodes string using MIME base64 algorithm */
void php3_base64_decode(INTERNAL_FUNCTION_PARAMETERS) {
	pval *string;
	unsigned char *result;
	int ret_length;
	TLS_VARS;

	if (ARG_COUNT(ht)!=1 || getParameters(ht,1,&string) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(string);
	result = _php3_base64_decode(string->value.str.val, string->value.str.len, &ret_length);
	if (result != NULL) {
		return_value->value.str.val = result;
		return_value->value.str.len = ret_length;
		return_value->type = IS_STRING;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
