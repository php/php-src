/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#include "php.h"
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include "php_string.h"

#define LEVENSHTEIN_MAX_LENGTH 255

/* {{{ reference_levdist
 * reference implementation, only optimized for memory usage, not speed */
static zend_long reference_levdist(const char *s1, size_t l1, const char *s2, size_t l2, zend_long cost_ins, zend_long cost_rep, zend_long cost_del )
{
	zend_long *p1, *p2, *tmp;
	zend_long c0, c1, c2;
	size_t i1, i2;

	if (l1 == 0) {
		return l2 * cost_ins;
	}
	if (l2 == 0) {
		return l1 * cost_del;
	}

	p1 = safe_emalloc((l2 + 1), sizeof(zend_long), 0);
	p2 = safe_emalloc((l2 + 1), sizeof(zend_long), 0);

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

/* {{{ proto int levenshtein(string str1, string str2[, int cost_ins, int cost_rep, int cost_del])
   Calculate Levenshtein distance between two strings */
PHP_FUNCTION(levenshtein)
{
	zend_string *string1, *string2;
	zend_long cost_ins = 1;
	zend_long cost_rep = 1;
	zend_long cost_del = 1;
	zend_long distance = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|lll", &string1, &string2, &cost_ins, &cost_rep, &cost_del) == FAILURE) {
		RETURN_THROWS();
	}

	if (ZSTR_LEN(string1) > LEVENSHTEIN_MAX_LENGTH) {
		zend_argument_value_error(1, "must be less than %d characters", LEVENSHTEIN_MAX_LENGTH + 1);
		RETURN_THROWS();
	}

	if (ZSTR_LEN(string2) > LEVENSHTEIN_MAX_LENGTH) {
		zend_argument_value_error(2, "must be less than %d characters", LEVENSHTEIN_MAX_LENGTH + 1);
		RETURN_THROWS();
	}

	distance = reference_levdist(ZSTR_VAL(string1), ZSTR_LEN(string1), ZSTR_VAL(string2), ZSTR_LEN(string2), cost_ins, cost_rep, cost_del);

	RETURN_LONG(distance);
}
/* }}} */
