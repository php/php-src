/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Hartmut Holzgraefe <hholzgra@php.net>                        |
   +----------------------------------------------------------------------+
 */

#include "php.h"

/* {{{ reference_levdist
 * reference implementation, only optimized for memory usage, not speed */
static bool reference_levdist(const zend_string *string1, const zend_string *string2, zend_long cost_ins, zend_long cost_rep, zend_long cost_del, zend_long *result)
{
	zend_long *p1, *p2, *tmp;
	zend_long c0, c1, c2;
	size_t i1, i2;

	if (ZSTR_LEN(string1) == 0) {
		*result = ZSTR_LEN(string2) * cost_ins;
		return true;
	}
	if (ZSTR_LEN(string2) == 0) {
		*result = ZSTR_LEN(string1) * cost_del;
		return true;
	}

	/* When all costs are equal, levenshtein fulfills the requirements of a metric, which means
	 * that the distance is symmetric. If string1 is shorter than string 2 we can save memory (and CPU time)
	 * by having shorter rows (p1 & p2). */
	if (ZSTR_LEN(string1) < ZSTR_LEN(string2) && cost_ins == cost_rep && cost_rep == cost_del) {
		const zend_string *tmp = string1;
		string1 = string2;
		string2 = tmp;
	}

	/* Each row is eight times the length of string2. */
	p1 = safe_femalloc((ZSTR_LEN(string2) + 1), sizeof(zend_long), 0, /* fallible */ true);
	if (UNEXPECTED(!p1)) {
		return false;
	}
	p2 = safe_femalloc((ZSTR_LEN(string2) + 1), sizeof(zend_long), 0, /* fallible */ true);
	if (UNEXPECTED(!p2)) {
		efree(p1);
		return false;
	}

	for (i2 = 0; i2 <= ZSTR_LEN(string2); i2++) {
		p1[i2] = i2 * cost_ins;
	}
	for (i1 = 0; i1 < ZSTR_LEN(string1) ; i1++) {
		p2[0] = p1[0] + cost_del;

		for (i2 = 0; i2 < ZSTR_LEN(string2); i2++) {
			c0 = p1[i2] + ((ZSTR_VAL(string1)[i1] == ZSTR_VAL(string2)[i2]) ? 0 : cost_rep);
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
	c0 = p1[ZSTR_LEN(string2)];

	efree(p1);
	efree(p2);

	*result = c0;
	return true;
}
/* }}} */

/* {{{ Calculate Levenshtein distance between two strings */
PHP_FUNCTION(levenshtein)
{
	zend_string *string1, *string2;
	zend_long cost_ins = 1;
	zend_long cost_rep = 1;
	zend_long cost_del = 1;

	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_STR(string1)
		Z_PARAM_STR(string2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(cost_ins)
		Z_PARAM_LONG(cost_rep)
		Z_PARAM_LONG(cost_del)
	ZEND_PARSE_PARAMETERS_END();

	zend_long distance;
	if (UNEXPECTED(!reference_levdist(string1, string2, cost_ins, cost_rep, cost_del, &distance))) {
		RETURN_OOM();
	}
	RETURN_LONG(distance);
}
/* }}} */
