/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Hartmut Holzgraefe <hholzgra@php.net>                        |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include "php_string.h"

#define LEVENSHTEIN_MAX_LENGTH 255

/* {{{ reference_levdist
 * reference implementation, only optimized for memory usage, not speed */
static int reference_levdist(void *s1, int l1, void *s2, int l2, zend_uchar str_type, int cost_ins, int cost_rep, int cost_del )
{
	int *p1, *p2, *tmp;
	int32_t i1, i2, j1, j2, cp1, cp2;
	int32_t c0, c1, c2;
	UChar32 ch1, ch2;

	if (str_type == IS_UNICODE) {
		cp1 = u_countChar32((UChar *)s1, l1);
		cp2 = u_countChar32((UChar *)s2, l2);

		if (cp1 == 0) {
			return cp2 * cost_ins;
		}
		if (cp2 == 0) {
			return cp1 * cost_del;
		}
		if ((cp1 > LEVENSHTEIN_MAX_LENGTH) || (cp2 > LEVENSHTEIN_MAX_LENGTH)) {
			return -1;
		}

		p1 = safe_emalloc((cp2 + 1), sizeof(int), 0);
		p2 = safe_emalloc((cp2 + 1), sizeof(int), 0);
	} else {
		if (l1 == 0) {
			return l2 * cost_ins;
		}
		if (l2 == 0) {
			return l1 * cost_del;
		}
		if ((l1 > LEVENSHTEIN_MAX_LENGTH) || (l2 > LEVENSHTEIN_MAX_LENGTH)) {
			return -1;
		}

		p1 = safe_emalloc((l2 + 1), sizeof(int), 0);
		p2 = safe_emalloc((l2 + 1), sizeof(int), 0);
	}

	for (i2 = 0; i2 <= l2; i2++) {
		p1[i2] = i2 * cost_ins;
	}
	for (i1 = 0, j1 = 0; i1 < l1; i1++) {
		p2[0] = p1[0] + cost_del;
		if (str_type == IS_UNICODE) {
			U16_NEXT((UChar *)s1, j1, l1, ch1);
		}
		for (i2 = 0, j2 = 0; i2 < l2; i2++) {
			if (str_type == IS_UNICODE) {
				U16_NEXT((UChar *)s2, j2, l2, ch2);
				c0 = p1[i2] + ((ch1 == ch2) ? 0 : cost_rep);
			} else {
				c0 = p1[i2] + ((*((char *)s1 + i1) == *((char *)s2 + i2)) ? 0 : cost_rep);
			}
			c1 = p1[i2 + 1] + cost_del;
			if (c1 < c0) {
				c0 = c1;
			}
			c2 = p2[i2] + cost_ins;
			if (c2 < c0) {
				c0 = c2;
			}
			p2[i2 + 1] = c0;
		}
		tmp = p1;
		p1 = p2;
		p2 = tmp;
	}
	c0 = p1[l2];

	efree(p1);
	efree(p2);

	return c0;
}
/* }}} */

/* {{{ custom_levdist
 */
static int custom_levdist(void *str1, void *str2, char *callback_name TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "The general Levenshtein support is not there yet");
	/* not there yet */

	return -1;
}
/* }}} */

/* {{{ proto int levenshtein(string str1, string str2[, int cost_ins, int cost_rep, int cost_del]) U
   Calculate Levenshtein distance between two strings */
PHP_FUNCTION(levenshtein)
{
	int argc = ZEND_NUM_ARGS();
	void *str1, *str2;
	char *callback_name;
	int str1_len, str2_len, callback_len;
	zend_uchar str1_type, str2_type;
	long cost_ins, cost_rep, cost_del;
	int distance = -1;

	switch (argc) {
		case 2: /* just two strings: use maximum performance version */
			if (zend_parse_parameters(2 TSRMLS_CC, "TT", &str1, &str1_len, &str1_type, &str2, &str2_len, &str2_type) == FAILURE) {
				return;
			}
			distance = reference_levdist(str1, str1_len, str2, str2_len, str1_type, 1, 1, 1);
			break;

		case 5: /* more general version: calc cost by ins/rep/del weights */
			if (zend_parse_parameters(5 TSRMLS_CC, "TTlll", &str1, &str1_len, &str1_type, &str2, &str2_len, &str2_type, &cost_ins, &cost_rep, &cost_del) == FAILURE) {
				return;
			}
			distance = reference_levdist(str1, str1_len, str2, str2_len, str1_type, cost_ins, cost_rep, cost_del);
			break;

		case 3: /* most general version: calc cost by user-supplied function */
			if (zend_parse_parameters(3 TSRMLS_CC, "TTs", &str1, &str1_len, &str1_type, &str2, &str2_len, &str2_type, &callback_name, &callback_len) == FAILURE) {
				return;
			}
			distance = custom_levdist(str1, str2, callback_name TSRMLS_CC);
			break;

		default:
			WRONG_PARAM_COUNT;
	}

	if (distance < 0 && /* TODO */ ZEND_NUM_ARGS() != 3) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument string(s) too long");
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
