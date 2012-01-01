/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
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
static int reference_levdist(const char *s1, int l1, const char *s2, int l2, int cost_ins, int cost_rep, int cost_del )
{
	int *p1, *p2, *tmp;
	int i1, i2, c0, c1, c2;

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

	for (i2 = 0; i2 <= l2; i2++) {
		p1[i2] = i2 * cost_ins;
	}
	for (i1 = 0; i1 < l1 ; i1++) {
		p2[0] = p1[0] + cost_del;

		for (i2 = 0; i2 < l2; i2++) {
			c0 = p1[i2] + ((s1[i1] == s2[i2]) ? 0 : cost_rep);
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
static int custom_levdist(char *str1, char *str2, char *callback_name TSRMLS_DC)
{
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "The general Levenshtein support is not there yet");
	/* not there yet */

	return -1;
}
/* }}} */

/* {{{ proto int levenshtein(string str1, string str2[, int cost_ins, int cost_rep, int cost_del])
   Calculate Levenshtein distance between two strings */
PHP_FUNCTION(levenshtein)
{
	int argc = ZEND_NUM_ARGS();
	char *str1, *str2;
	char *callback_name;
	int str1_len, str2_len, callback_len;
	long cost_ins, cost_rep, cost_del;
	int distance = -1;

	switch (argc) {
		case 2: /* just two strings: use maximum performance version */
			if (zend_parse_parameters(2 TSRMLS_CC, "ss", &str1, &str1_len, &str2, &str2_len) == FAILURE) {
				return;
			}
			distance = reference_levdist(str1, str1_len, str2, str2_len, 1, 1, 1);
			break;

		case 5: /* more general version: calc cost by ins/rep/del weights */
			if (zend_parse_parameters(5 TSRMLS_CC, "sslll", &str1, &str1_len, &str2, &str2_len, &cost_ins, &cost_rep, &cost_del) == FAILURE) {
				return;
			}
			distance = reference_levdist(str1, str1_len, str2, str2_len, cost_ins, cost_rep, cost_del);
			break;

		case 3: /* most general version: calc cost by user-supplied function */
			if (zend_parse_parameters(3 TSRMLS_CC, "sss", &str1, &str1_len, &str2, &str2_len, &callback_name, &callback_len) == FAILURE) {
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
