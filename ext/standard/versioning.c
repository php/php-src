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
   | Author: Stig Sæther Bakken <ssb@php.net>                             |
   +----------------------------------------------------------------------+
 */

#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "php.h"
#include "php_versioning.h"

/* {{{ php_canonicalize_version() */

PHPAPI char *
php_canonicalize_version(const char *version)
{
    size_t len = strlen(version);
    char *buf = safe_emalloc(len, 2, 1), *q, lp, lq;
    const char *p;

    if (len == 0) {
        *buf = '\0';
        return buf;
    }

    p = version;
    q = buf;
    *q++ = lp = *p++;

    while (*p) {
/*  s/[-_+]/./g;
 *  s/([^\d\.])([^\D\.])/$1.$2/g;
 *  s/([^\D\.])([^\d\.])/$1.$2/g;
 */
#define isdig(x) (isdigit(x)&&(x)!='.')
#define isndig(x) (!isdigit(x)&&(x)!='.')
#define isspecialver(x) ((x)=='-'||(x)=='_'||(x)=='+')

		lq = *(q - 1);
		if (isspecialver(*p)) {
			if (lq != '.') {
				*q++ = '.';
			}
		} else if ((isndig(lp) && isdig(*p)) || (isdig(lp) && isndig(*p))) {
			if (lq != '.') {
				*q++ = '.';
			}
			*q++ = *p;
		} else if (!isalnum(*p)) {
			if (lq != '.') {
				*q++ = '.';
			}
		} else {
			*q++ = *p;
		}
		lp = *p++;
    }
    *q++ = '\0';
    return buf;
}

/* }}} */
/* {{{ compare_special_version_forms() */

typedef struct {
	const char *name;
	int order;
} special_forms_t;

static int
compare_special_version_forms(char *form1, char *form2)
{
	int found1 = -1, found2 = -1;
	special_forms_t special_forms[11] = {
		{"dev", 0},
		{"alpha", 1},
		{"a", 1},
		{"beta", 2},
		{"b", 2},
		{"RC", 3},
		{"rc", 3},
		{"#", 4},
		{"pl", 5},
		{"p", 5},
		{NULL, 0},
	};
	special_forms_t *pp;

	for (pp = special_forms; pp && pp->name; pp++) {
		if (strncmp(form1, pp->name, strlen(pp->name)) == 0) {
			found1 = pp->order;
			break;
		}
	}
	for (pp = special_forms; pp && pp->name; pp++) {
		if (strncmp(form2, pp->name, strlen(pp->name)) == 0) {
			found2 = pp->order;
			break;
		}
	}
	return ZEND_NORMALIZE_BOOL(found1 - found2);
}

/* }}} */
/* {{{ php_version_compare() */

PHPAPI int
php_version_compare(const char *orig_ver1, const char *orig_ver2)
{
	char *ver1;
	char *ver2;
	char *p1, *p2, *n1, *n2;
	long l1, l2;
	int compare = 0;

	if (!*orig_ver1 || !*orig_ver2) {
		if (!*orig_ver1 && !*orig_ver2) {
			return 0;
		} else {
			return *orig_ver1 ? 1 : -1;
		}
	}
	if (orig_ver1[0] == '#') {
		ver1 = estrdup(orig_ver1);
	} else {
		ver1 = php_canonicalize_version(orig_ver1);
	}
	if (orig_ver2[0] == '#') {
		ver2 = estrdup(orig_ver2);
	} else {
		ver2 = php_canonicalize_version(orig_ver2);
	}
	p1 = n1 = ver1;
	p2 = n2 = ver2;
	while (*p1 && *p2 && n1 && n2) {
		if ((n1 = strchr(p1, '.')) != NULL) {
			*n1 = '\0';
		}
		if ((n2 = strchr(p2, '.')) != NULL) {
			*n2 = '\0';
		}
		if (isdigit(*p1) && isdigit(*p2)) {
			/* compare element numerically */
			l1 = strtol(p1, NULL, 10);
			l2 = strtol(p2, NULL, 10);
			compare = ZEND_NORMALIZE_BOOL(l1 - l2);
		} else if (!isdigit(*p1) && !isdigit(*p2)) {
			/* compare element names */
			compare = compare_special_version_forms(p1, p2);
		} else {
			/* mix of names and numbers */
			if (isdigit(*p1)) {
				compare = compare_special_version_forms("#N#", p2);
			} else {
				compare = compare_special_version_forms(p1, "#N#");
			}
		}
		if (compare != 0) {
			break;
		}
		if (n1 != NULL) {
			p1 = n1 + 1;
		}
		if (n2 != NULL) {
			p2 = n2 + 1;
		}
	}
	if (compare == 0) {
		if (n1 != NULL) {
			if (isdigit(*p1)) {
				compare = 1;
			} else {
				compare = php_version_compare(p1, "#N#");
			}
		} else if (n2 != NULL) {
			if (isdigit(*p2)) {
				compare = -1;
			} else {
				compare = php_version_compare("#N#", p2);
			}
		}
	}
	efree(ver1);
	efree(ver2);
	return compare;
}

/* }}} */
/* {{{ Compares two "PHP-standardized" version number strings */

PHP_FUNCTION(version_compare)
{
	char *v1, *v2;
	zend_string *op = NULL;
	size_t v1_len, v2_len;
	int compare;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(v1, v1_len)
		Z_PARAM_STRING(v2, v2_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(op)
	ZEND_PARSE_PARAMETERS_END();

	compare = php_version_compare(v1, v2);
	if (!op) {
		RETURN_LONG(compare);
	}
	if (zend_string_equals_literal(op, "<") || zend_string_equals_literal(op, "lt")) {
		RETURN_BOOL(compare == -1);
	}
	if (zend_string_equals_literal(op, "<=") || zend_string_equals_literal(op, "le")) {
		RETURN_BOOL(compare != 1);
	}
	if (zend_string_equals_literal(op, ">") || zend_string_equals_literal(op, "gt")) {
		RETURN_BOOL(compare == 1);
	}
	if (zend_string_equals_literal(op, ">=") || zend_string_equals_literal(op, "ge")) {
		RETURN_BOOL(compare != -1);
	}
	if (zend_string_equals_literal(op, "==") || zend_string_equals_literal(op, "=") || zend_string_equals_literal(op, "eq")) {
		RETURN_BOOL(compare == 0);
	}
	if (zend_string_equals_literal(op, "!=") || zend_string_equals_literal(op, "<>") || zend_string_equals_literal(op, "ne")) {
		RETURN_BOOL(compare != 0);
	}

	zend_argument_value_error(3, "must be a valid comparison operator");
}

/* }}} */
