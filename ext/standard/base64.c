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
   | Author: Jim Winstead <jimw@php.net>                                  |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <string.h>

#include "php.h"
#include "base64.h"

/* {{{ */
static const char base64_table[] =
	{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
	};

static const char base64_pad = '=';

static const short base64_reverse_table[256] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
/* }}} */

/* {{{ */
unsigned char *php_base64_encode(const unsigned char *str, int length, int *ret_length)
{
	const unsigned char *current = str;
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
/* }}} */

/* {{{ */
/* generate reverse table (do not set index 0 to 64)
static unsigned short base64_reverse_table[256];
#define rt base64_reverse_table
void php_base64_init()
{
	char *s = emalloc(10240), *sp;
	char *chp;
	short idx;

	for(ch = 0; ch < 256; ch++) {
		chp = strchr(base64_table, ch);
		if(ch && chp) {
			idx = chp - base64_table;
			if (idx >= 64) idx = -1;
			rt[ch] = idx;
		} else {
			rt[ch] = -1;
		}
	}
	sp = s;
	sprintf(sp, "static const short base64_reverse_table[256] = {\n");
	for(ch =0; ch < 256;) {
		sp = s+strlen(s);
		sprintf(sp, "\t% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,% 3d,\n", rt[ch+0], rt[ch+1], rt[ch+2], rt[ch+3], rt[ch+4], rt[ch+5], rt[ch+6], rt[ch+7], rt[ch+8], rt[ch+9], rt[ch+10], rt[ch+11], rt[ch+12], rt[ch+13], rt[ch+14], rt[ch+15]);
		ch += 16;
	}
	sprintf(sp, "};");
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Reverse_table:\n%s", s);
	efree(s);
}
*/
/* }}} */

/* {{{ */
/* as above, but backwards. :) */
unsigned char *php_base64_decode(const unsigned char *str, int length, int *ret_length)
{
	const unsigned char *current = str;
	int ch, i = 0, j = 0, k;
	/* this sucks for threaded environments */
	unsigned char *result;
	
	result = (unsigned char *)emalloc(length + 1);
	if (result == NULL) {
		return NULL;
	}

	/* run through the whole string, converting as we go */
	while ((ch = *current++) != '\0' && length-- > 0) {
		if (ch == base64_pad) break;

	    /* When Base64 gets POSTed, all pluses are interpreted as spaces.
		   This line changes them back.  It's not exactly the Base64 spec,
		   but it is completely compatible with it (the spec says that
		   spaces are invalid).  This will also save many people considerable
		   headache.  - Turadg Aleahmad <turadg@wise.berkeley.edu>
	    */

		if (ch == ' ') ch = '+'; 

		ch = base64_reverse_table[ch];
		if (ch < 0) continue;

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
	result[j] = '\0';
	return result;
}
/* }}} */

/* {{{ proto string base64_encode(string str)
   Encodes string using MIME base64 algorithm */
PHP_FUNCTION(base64_encode)
{
	char *str;
	unsigned char *result;
	int str_len, ret_length;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}
	result = php_base64_encode(str, str_len, &ret_length);
	if (result != NULL) {
		RETVAL_STRINGL(result, ret_length, 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto string base64_decode(string str)
   Decodes string using MIME base64 algorithm */
PHP_FUNCTION(base64_decode)
{
	char *str;
	unsigned char *result;
	int str_len, ret_length;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}
	result = php_base64_decode(str, str_len, &ret_length);
	if (result != NULL) {
		RETVAL_STRINGL(result, ret_length, 0);
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
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
