/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
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
  | Authors: Derick Rethans <derick@php.net>                             |
  |          Pierre-A. Joye <pierre@php.net>                             |
  |          Kévin Dunglas <dunglas@gmail.com>                           |
  +----------------------------------------------------------------------+
*/

#include "php_filter.h"
#include "filter_private.h"
#include "ext/standard/url.h"
#include "ext/pcre/php_pcre.h"

#include "zend_multiply.h"

#if HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif

#ifndef INADDR_NONE
# define INADDR_NONE ((unsigned long int) -1)
#endif


/* {{{ FETCH_DOUBLE_OPTION(var_name, option_name) */
#define FETCH_DOUBLE_OPTION(var_name, option_name) \
   	var_name = 0; \
	var_name##_set = 0; \
	if (option_array) { \
		if ((option_val = zend_hash_str_find(Z_ARRVAL_P(option_array), option_name, sizeof(option_name) - 1)) != NULL) {	\
			var_name = zval_get_double(option_val); \
			var_name##_set = 1; \
		} \
	}
/* }}} */

/* {{{ FETCH_LONG_OPTION(var_name, option_name) */
#define FETCH_LONG_OPTION(var_name, option_name) \
   	var_name = 0; \
	var_name##_set = 0; \
	if (option_array) { \
		if ((option_val = zend_hash_str_find(Z_ARRVAL_P(option_array), option_name, sizeof(option_name) - 1)) != NULL) {	\
			var_name = zval_get_long(option_val); \
			var_name##_set = 1; \
		} \
	}
/* }}} */

/* {{{ FETCH_STRING_OPTION(var_name, option_name) */
#define FETCH_STRING_OPTION(var_name, option_name) \
	var_name = NULL; \
	var_name##_set = 0; \
	var_name##_len = 0; \
	if (option_array) { \
		if ((option_val = zend_hash_str_find_deref(Z_ARRVAL_P(option_array), option_name, sizeof(option_name) - 1)) != NULL) { \
			if (Z_TYPE_P(option_val) == IS_STRING) { \
				var_name = Z_STRVAL_P(option_val); \
				var_name##_len = Z_STRLEN_P(option_val); \
				var_name##_set = 1; \
			} \
		} \
	}
/* }}} */

/* {{{ FETCH_STR_OPTION(var_name, option_name) */
#define FETCH_STR_OPTION(var_name, option_name) \
	var_name = NULL; \
	var_name##_set = 0; \
	if (option_array) { \
		if ((option_val = zend_hash_str_find_deref(Z_ARRVAL_P(option_array), option_name, sizeof(option_name) - 1)) != NULL) { \
			if (Z_TYPE_P(option_val) == IS_STRING) { \
				var_name = Z_STR_P(option_val); \
				var_name##_set = 1; \
			} \
		} \
	}
/* }}} */

#define FORMAT_IPV4    4
#define FORMAT_IPV6    6

static int _php_filter_validate_ipv6(char *str, size_t str_len);

static int php_filter_parse_int(const char *str, size_t str_len, zend_long *ret) { /* {{{ */
	zend_long ctx_value;
	int sign = 0, digit = 0;
	const char *end = str + str_len;

	switch (*str) {
		case '-':
			sign = 1;
		case '+':
			str++;
		default:
			break;
	}

	if (*str == '0' && str + 1 == end) {
		/* Special cases: +0 and -0 */
		return 1;
	}

	/* must start with 1..9*/
	if (str < end && *str >= '1' && *str <= '9') {
		ctx_value = ((sign)?-1:1) * ((*(str++)) - '0');
	} else {
		return -1;
	}

	if ((end - str > MAX_LENGTH_OF_LONG - 1) /* number too long */
	 || (SIZEOF_LONG == 4 && (end - str == MAX_LENGTH_OF_LONG - 1) && *str > '2')) {
		/* overflow */
		return -1;
	}

	while (str < end) {
		if (*str >= '0' && *str <= '9') {
			digit = (*(str++) - '0');
			if ( (!sign) && ctx_value <= (ZEND_LONG_MAX-digit)/10 ) {
				ctx_value = (ctx_value * 10) + digit;
			} else if ( sign && ctx_value >= (ZEND_LONG_MIN+digit)/10) {
				ctx_value = (ctx_value * 10) - digit;
			} else {
				return -1;
			}
		} else {
			return -1;
		}
	}

	*ret = ctx_value;
	return 1;
}
/* }}} */

static int php_filter_parse_octal(const char *str, size_t str_len, zend_long *ret) { /* {{{ */
	zend_ulong ctx_value = 0;
	const char *end = str + str_len;

	while (str < end) {
		if (*str >= '0' && *str <= '7') {
			zend_ulong n = ((*(str++)) - '0');

			if ((ctx_value > ((zend_ulong)(~(zend_long)0)) / 8) ||
				((ctx_value = ctx_value * 8) > ((zend_ulong)(~(zend_long)0)) - n)) {
				return -1;
			}
			ctx_value += n;
		} else {
			return -1;
		}
	}

	*ret = (zend_long)ctx_value;
	return 1;
}
/* }}} */

static int php_filter_parse_hex(const char *str, size_t str_len, zend_long *ret) { /* {{{ */
	zend_ulong ctx_value = 0;
	const char *end = str + str_len;
	zend_ulong n;

	while (str < end) {
		if (*str >= '0' && *str <= '9') {
			n = ((*(str++)) - '0');
		} else if (*str >= 'a' && *str <= 'f') {
			n = ((*(str++)) - ('a' - 10));
		} else if (*str >= 'A' && *str <= 'F') {
			n = ((*(str++)) - ('A' - 10));
		} else {
			return -1;
		}
		if ((ctx_value > ((zend_ulong)(~(zend_long)0)) / 16) ||
			((ctx_value = ctx_value * 16) > ((zend_ulong)(~(zend_long)0)) - n)) {
			return -1;
		}
		ctx_value += n;
	}

	*ret = (zend_long)ctx_value;
	return 1;
}
/* }}} */

void php_filter_int(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	zval *option_val;
	zend_long  min_range, max_range, option_flags;
	int   min_range_set, max_range_set;
	int   allow_octal = 0, allow_hex = 0;
	size_t	  len;
	int error = 0;
	zend_long  ctx_value;
	char *p;

	/* Parse options */
	FETCH_LONG_OPTION(min_range,    "min_range");
	FETCH_LONG_OPTION(max_range,    "max_range");
	option_flags = flags;

	len = Z_STRLEN_P(value);

	if (len == 0) {
		RETURN_VALIDATION_FAILED
	}

	if (option_flags & FILTER_FLAG_ALLOW_OCTAL) {
		allow_octal = 1;
	}

	if (option_flags & FILTER_FLAG_ALLOW_HEX) {
		allow_hex = 1;
	}

	/* Start the validating loop */
	p = Z_STRVAL_P(value);
	ctx_value = 0;

	PHP_FILTER_TRIM_DEFAULT(p, len);

	if (*p == '0') {
		p++; len--;
		if (allow_hex && (*p == 'x' || *p == 'X')) {
			p++; len--;
			if (php_filter_parse_hex(p, len, &ctx_value) < 0) {
				error = 1;
			}
		} else if (allow_octal) {
			if (php_filter_parse_octal(p, len, &ctx_value) < 0) {
				error = 1;
			}
		} else if (len != 0) {
			error = 1;
		}
	} else {
		if (php_filter_parse_int(p, len, &ctx_value) < 0) {
			error = 1;
		}
	}

	if (error > 0 || (min_range_set && (ctx_value < min_range)) || (max_range_set && (ctx_value > max_range))) {
		RETURN_VALIDATION_FAILED
	} else {
		zval_ptr_dtor(value);
		ZVAL_LONG(value, ctx_value);
		return;
	}
}
/* }}} */

void php_filter_boolean(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	char *str = Z_STRVAL_P(value);
	size_t len = Z_STRLEN_P(value);
	int ret;

	PHP_FILTER_TRIM_DEFAULT_EX(str, len, 0);

	/* returns true for "1", "true", "on" and "yes"
	 * returns false for "0", "false", "off", "no", and ""
	 * null otherwise. */
	switch (len) {
		case 0:
			ret = 0;
			break;
		case 1:
			if (*str == '1') {
				ret = 1;
			} else if (*str == '0') {
				ret = 0;
			} else {
				ret = -1;
			}
			break;
		case 2:
			if (strncasecmp(str, "on", 2) == 0) {
				ret = 1;
			} else if (strncasecmp(str, "no", 2) == 0) {
				ret = 0;
			} else {
				ret = -1;
			}
			break;
		case 3:
			if (strncasecmp(str, "yes", 3) == 0) {
				ret = 1;
			} else if (strncasecmp(str, "off", 3) == 0) {
				ret = 0;
			} else {
				ret = -1;
			}
			break;
		case 4:
			if (strncasecmp(str, "true", 4) == 0) {
				ret = 1;
			} else {
				ret = -1;
			}
			break;
		case 5:
			if (strncasecmp(str, "false", 5) == 0) {
				ret = 0;
			} else {
				ret = -1;
			}
			break;
		default:
			ret = -1;
	}

	if (ret == -1) {
		RETURN_VALIDATION_FAILED
	} else {
		zval_ptr_dtor(value);
		ZVAL_BOOL(value, ret);
	}
}
/* }}} */

void php_filter_float(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	size_t len;
	char *str, *end;
	char *num, *p;
	zval *option_val;
	char *decimal;
	int decimal_set;
	size_t decimal_len;
	char dec_sep = '.';
	char *thousand;
	int thousand_set;
	size_t thousand_len;
	char *tsd_sep;

	zend_long lval;
	double dval;
	double min_range, max_range;
	int   min_range_set, max_range_set;

	int first, n;

	len = Z_STRLEN_P(value);
	str = Z_STRVAL_P(value);

	PHP_FILTER_TRIM_DEFAULT(str, len);
	end = str + len;

	FETCH_STRING_OPTION(decimal, "decimal");

	if (decimal_set) {
		if (decimal_len != 1) {
			php_error_docref(NULL, E_WARNING, "decimal separator must be one char");
			RETURN_VALIDATION_FAILED
		} else {
			dec_sep = *decimal;
		}
	}

	FETCH_STRING_OPTION(thousand, "thousand");

	if (thousand_set) {
		if (thousand_len < 1) {
			php_error_docref(NULL, E_WARNING, "thousand separator must be at least one char");
			RETURN_VALIDATION_FAILED
		} else {
			tsd_sep = thousand;
		}
	} else {
		tsd_sep = "',.";
	}

	FETCH_DOUBLE_OPTION(min_range, "min_range");
	FETCH_DOUBLE_OPTION(max_range, "max_range");

	num = p = emalloc(len+1);
	if (str < end && (*str == '+' || *str == '-')) {
		*p++ = *str++;
	}
	first = 1;
	while (1) {
		n = 0;
		while (str < end && *str >= '0' && *str <= '9') {
			++n;
			*p++ = *str++;
		}
		if (str == end || *str == dec_sep || *str == 'e' || *str == 'E') {
			if (!first && n != 3) {
				goto error;
			}
			if (*str == dec_sep) {
				*p++ = '.';
				str++;
				while (str < end && *str >= '0' && *str <= '9') {
					*p++ = *str++;
				}
			}
			if (*str == 'e' || *str == 'E') {
				*p++ = *str++;
				if (str < end && (*str == '+' || *str == '-')) {
					*p++ = *str++;
				}
				while (str < end && *str >= '0' && *str <= '9') {
					*p++ = *str++;
				}
			}
			break;
		}
		if ((flags & FILTER_FLAG_ALLOW_THOUSAND) && strchr(tsd_sep, *str)) {
			if (first?(n < 1 || n > 3):(n != 3)) {
				goto error;
			}
			first = 0;
			str++;
		} else {
			goto error;
		}
	}
	if (str != end) {
		goto error;
	}
	*p = 0;

	switch (is_numeric_string(num, p - num, &lval, &dval, 0)) {
		case IS_LONG:
			zval_ptr_dtor(value);
			if ((min_range_set && (lval < min_range)) || (max_range_set && (lval > max_range))) {
				goto error;
			}
			ZVAL_DOUBLE(value, (double)lval);
			break;
		case IS_DOUBLE:
			if ((!dval && p - num > 1 && strpbrk(num, "123456789")) || !zend_finite(dval)) {
				goto error;
			}
			if ((min_range_set && (dval < min_range)) || (max_range_set && (dval > max_range))) {
				goto error;
			}
			zval_ptr_dtor(value);
			ZVAL_DOUBLE(value, dval);
			break;
		default:
error:
			efree(num);
			RETURN_VALIDATION_FAILED
	}
	efree(num);
}
/* }}} */

void php_filter_validate_regexp(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	zval *option_val;
	zend_string *regexp;
	int regexp_set;
	pcre2_code *re = NULL;
	pcre2_match_data *match_data = NULL;
	uint32_t capture_count;
	int rc;

	/* Parse options */
	FETCH_STR_OPTION(regexp, "regexp");

	if (!regexp_set) {
		php_error_docref(NULL, E_WARNING, "'regexp' option missing");
		RETURN_VALIDATION_FAILED
	}

	re = pcre_get_compiled_regex(regexp, &capture_count);
	if (!re) {
		RETURN_VALIDATION_FAILED
	}
	match_data = php_pcre_create_match_data(capture_count, re);
	if (!match_data) {
		RETURN_VALIDATION_FAILED
	}
	rc = pcre2_match(re, (PCRE2_SPTR)Z_STRVAL_P(value), Z_STRLEN_P(value), 0, 0, match_data, php_pcre_mctx());
	php_pcre_free_match_data(match_data);

	/* 0 means that the vector is too small to hold all the captured substring offsets */
	if (rc < 0) {
		RETURN_VALIDATION_FAILED
	}
}

static int _php_filter_validate_domain(char * domain, int len, zend_long flags) /* {{{ */
{
	char *e, *s, *t;
	size_t l;
	int hostname = flags & FILTER_FLAG_HOSTNAME;
	unsigned char i = 1;

	s = domain;
	l = len;
	e = domain + l;
	t = e - 1;

	/* Ignore trailing dot */
	if (*t == '.') {
		e = t;
		l--;
	}

	/* The total length cannot exceed 253 characters (final dot not included) */
	if (l > 253) {
		return 0;
	}

	/* First char must be alphanumeric */
	if(*s == '.' || (hostname && !isalnum((int)*(unsigned char *)s))) {
		return 0;
	}

	while (s < e) {
		if (*s == '.') {
			/* The first and the last character of a label must be alphanumeric */
			if (*(s + 1) == '.' || (hostname && (!isalnum((int)*(unsigned char *)(s - 1)) || !isalnum((int)*(unsigned char *)(s + 1))))) {
				return 0;
			}

			/* Reset label length counter */
			i = 1;
		} else {
			if (i > 63 || (hostname && *s != '-' && !isalnum((int)*(unsigned char *)s))) {
				return 0;
			}

			i++;
		}

		s++;
	}

	return 1;
}
/* }}} */

void php_filter_validate_domain(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	if (!_php_filter_validate_domain(Z_STRVAL_P(value), Z_STRLEN_P(value), flags)) {
		RETURN_VALIDATION_FAILED
	}
}
/* }}} */

static int is_userinfo_valid(zend_string *str)
{
	const char *valid = "-._~!$&'()*+,;=:";
	const char *p = ZSTR_VAL(str);
	while (p - ZSTR_VAL(str) < ZSTR_LEN(str)) {
		if (isalpha(*p) || isdigit(*p) || strchr(valid, *p)) {
			p++;
		} else if (*p == '%' && p - ZSTR_VAL(str) <= ZSTR_LEN(str) - 3 && isdigit(*(p+1)) && isxdigit(*(p+2))) {
			p += 3;
		} else {
			return 0;
		}
	}
	return 1;
}

void php_filter_validate_url(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	php_url *url;
	size_t old_len = Z_STRLEN_P(value);

	if (flags & (FILTER_FLAG_SCHEME_REQUIRED | FILTER_FLAG_HOST_REQUIRED)) {
		php_error_docref(NULL, E_DEPRECATED,
			"explicit use of FILTER_FLAG_SCHEME_REQUIRED and FILTER_FLAG_HOST_REQUIRED is deprecated");
	}

	php_filter_url(value, flags, option_array, charset);

	if (Z_TYPE_P(value) != IS_STRING || old_len != Z_STRLEN_P(value)) {
		RETURN_VALIDATION_FAILED
	}

	/* Use parse_url - if it returns false, we return NULL */
	url = php_url_parse_ex(Z_STRVAL_P(value), Z_STRLEN_P(value));

	if (url == NULL) {
		RETURN_VALIDATION_FAILED
	}

	if (url->scheme != NULL &&
		(zend_string_equals_literal_ci(url->scheme, "http") || zend_string_equals_literal_ci(url->scheme, "https"))) {
		char *e, *s, *t;
		size_t l;

		if (url->host == NULL) {
			goto bad_url;
		}

		s = ZSTR_VAL(url->host);
		l = ZSTR_LEN(url->host);
		e = s + l;
		t = e - 1;

		/* An IPv6 enclosed by square brackets is a valid hostname */
		if (*s == '[' && *t == ']' && _php_filter_validate_ipv6((s + 1), l - 2)) {
			php_url_free(url);
			return;
		}

		// Validate domain
		if (!_php_filter_validate_domain(ZSTR_VAL(url->host), l, FILTER_FLAG_HOSTNAME)) {
			php_url_free(url);
			RETURN_VALIDATION_FAILED
		}
	}

	if (
		url->scheme == NULL ||
		/* some schemas allow the host to be empty */
		(url->host == NULL && (strcmp(ZSTR_VAL(url->scheme), "mailto") && strcmp(ZSTR_VAL(url->scheme), "news") && strcmp(ZSTR_VAL(url->scheme), "file"))) ||
		((flags & FILTER_FLAG_PATH_REQUIRED) && url->path == NULL) || ((flags & FILTER_FLAG_QUERY_REQUIRED) && url->query == NULL)
	) {
bad_url:
		php_url_free(url);
		RETURN_VALIDATION_FAILED
	}

	if (url->user != NULL && !is_userinfo_valid(url->user)) {
		php_url_free(url);
		RETURN_VALIDATION_FAILED

	}

	php_url_free(url);
}
/* }}} */

void php_filter_validate_email(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	/*
	 * The regex below is based on a regex by Michael Rushton.
	 * However, it is not identical.  I changed it to only consider routeable
	 * addresses as valid.  Michael's regex considers a@b a valid address
	 * which conflicts with section 2.3.5 of RFC 5321 which states that:
	 *
	 *   Only resolvable, fully-qualified domain names (FQDNs) are permitted
	 *   when domain names are used in SMTP.  In other words, names that can
	 *   be resolved to MX RRs or address (i.e., A or AAAA) RRs (as discussed
	 *   in Section 5) are permitted, as are CNAME RRs whose targets can be
	 *   resolved, in turn, to MX or address RRs.  Local nicknames or
	 *   unqualified names MUST NOT be used.
	 *
	 * This regex does not handle comments and folding whitespace.  While
	 * this is technically valid in an email address, these parts aren't
	 * actually part of the address itself.
	 *
	 * Michael's regex carries this copyright:
	 *
	 * Copyright © Michael Rushton 2009-10
	 * http://squiloople.com/
	 * Feel free to use and redistribute this code. But please keep this copyright notice.
	 *
	 */
	pcre2_code *re = NULL;
	pcre2_match_data *match_data = NULL;
	uint32_t capture_count;
	zend_string *sregexp;
	int rc;
	const char regexp0[] = "/^(?!(?:(?:\\x22?\\x5C[\\x00-\\x7E]\\x22?)|(?:\\x22?[^\\x5C\\x22]\\x22?)){255,})(?!(?:(?:\\x22?\\x5C[\\x00-\\x7E]\\x22?)|(?:\\x22?[^\\x5C\\x22]\\x22?)){65,}@)(?:(?:[\\x21\\x23-\\x27\\x2A\\x2B\\x2D\\x2F-\\x39\\x3D\\x3F\\x5E-\\x7E\\pL\\pN]+)|(?:\\x22(?:[\\x01-\\x08\\x0B\\x0C\\x0E-\\x1F\\x21\\x23-\\x5B\\x5D-\\x7F\\pL\\pN]|(?:\\x5C[\\x00-\\x7F]))*\\x22))(?:\\.(?:(?:[\\x21\\x23-\\x27\\x2A\\x2B\\x2D\\x2F-\\x39\\x3D\\x3F\\x5E-\\x7E\\pL\\pN]+)|(?:\\x22(?:[\\x01-\\x08\\x0B\\x0C\\x0E-\\x1F\\x21\\x23-\\x5B\\x5D-\\x7F\\pL\\pN]|(?:\\x5C[\\x00-\\x7F]))*\\x22)))*@(?:(?:(?!.*[^.]{64,})(?:(?:(?:xn--)?[a-z0-9]+(?:-+[a-z0-9]+)*\\.){1,126}){1,}(?:(?:[a-z][a-z0-9]*)|(?:(?:xn--)[a-z0-9]+))(?:-+[a-z0-9]+)*)|(?:\\[(?:(?:IPv6:(?:(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){7})|(?:(?!(?:.*[a-f0-9][:\\]]){7,})(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,5})?::(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,5})?)))|(?:(?:IPv6:(?:(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){5}:)|(?:(?!(?:.*[a-f0-9]:){5,})(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,3})?::(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,3}:)?)))?(?:(?:25[0-5])|(?:2[0-4][0-9])|(?:1[0-9]{2})|(?:[1-9]?[0-9]))(?:\\.(?:(?:25[0-5])|(?:2[0-4][0-9])|(?:1[0-9]{2})|(?:[1-9]?[0-9]))){3}))\\]))$/iDu";
	const char regexp1[] = "/^(?!(?:(?:\\x22?\\x5C[\\x00-\\x7E]\\x22?)|(?:\\x22?[^\\x5C\\x22]\\x22?)){255,})(?!(?:(?:\\x22?\\x5C[\\x00-\\x7E]\\x22?)|(?:\\x22?[^\\x5C\\x22]\\x22?)){65,}@)(?:(?:[\\x21\\x23-\\x27\\x2A\\x2B\\x2D\\x2F-\\x39\\x3D\\x3F\\x5E-\\x7E]+)|(?:\\x22(?:[\\x01-\\x08\\x0B\\x0C\\x0E-\\x1F\\x21\\x23-\\x5B\\x5D-\\x7F]|(?:\\x5C[\\x00-\\x7F]))*\\x22))(?:\\.(?:(?:[\\x21\\x23-\\x27\\x2A\\x2B\\x2D\\x2F-\\x39\\x3D\\x3F\\x5E-\\x7E]+)|(?:\\x22(?:[\\x01-\\x08\\x0B\\x0C\\x0E-\\x1F\\x21\\x23-\\x5B\\x5D-\\x7F]|(?:\\x5C[\\x00-\\x7F]))*\\x22)))*@(?:(?:(?!.*[^.]{64,})(?:(?:(?:xn--)?[a-z0-9]+(?:-+[a-z0-9]+)*\\.){1,126}){1,}(?:(?:[a-z][a-z0-9]*)|(?:(?:xn--)[a-z0-9]+))(?:-+[a-z0-9]+)*)|(?:\\[(?:(?:IPv6:(?:(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){7})|(?:(?!(?:.*[a-f0-9][:\\]]){7,})(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,5})?::(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,5})?)))|(?:(?:IPv6:(?:(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){5}:)|(?:(?!(?:.*[a-f0-9]:){5,})(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,3})?::(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,3}:)?)))?(?:(?:25[0-5])|(?:2[0-4][0-9])|(?:1[0-9]{2})|(?:[1-9]?[0-9]))(?:\\.(?:(?:25[0-5])|(?:2[0-4][0-9])|(?:1[0-9]{2})|(?:[1-9]?[0-9]))){3}))\\]))$/iD";
	const char *regexp;
	size_t regexp_len;

	if (flags & FILTER_FLAG_EMAIL_UNICODE) {
		regexp = regexp0;
		regexp_len = sizeof(regexp0) - 1;
	} else {
		regexp = regexp1;
		regexp_len = sizeof(regexp1) - 1;
	}

	/* The maximum length of an e-mail address is 320 octets, per RFC 2821. */
	if (Z_STRLEN_P(value) > 320) {
		RETURN_VALIDATION_FAILED
	}

	sregexp = zend_string_init(regexp, regexp_len, 0);
	re = pcre_get_compiled_regex(sregexp, &capture_count);
	zend_string_release_ex(sregexp, 0);
	if (!re) {
		RETURN_VALIDATION_FAILED
	}
	match_data = php_pcre_create_match_data(capture_count, re);
	if (!match_data) {
		RETURN_VALIDATION_FAILED
	}
	rc = pcre2_match(re, (PCRE2_SPTR)Z_STRVAL_P(value), Z_STRLEN_P(value), 0, 0, match_data, php_pcre_mctx());
	php_pcre_free_match_data(match_data);

	/* 0 means that the vector is too small to hold all the captured substring offsets */
	if (rc < 0) {
		RETURN_VALIDATION_FAILED
	}

}
/* }}} */

static int _php_filter_validate_ipv4(char *str, size_t str_len, int *ip) /* {{{ */
{
	const char *end = str + str_len;
	int num, m;
	int n = 0;

	while (str < end) {
		int leading_zero;
		if (*str < '0' || *str > '9') {
			return 0;
		}
		leading_zero = (*str == '0');
		m = 1;
		num = ((*(str++)) - '0');
		while (str < end && (*str >= '0' && *str <= '9')) {
			num = num * 10 + ((*(str++)) - '0');
			if (num > 255 || ++m > 3) {
				return 0;
			}
		}
		/* don't allow a leading 0; that introduces octal numbers,
		 * which we don't support */
		if (leading_zero && (num != 0 || m > 1))
			return 0;
		ip[n++] = num;
		if (n == 4) {
			return str == end;
		} else if (str >= end || *(str++) != '.') {
			return 0;
		}
	}
	return 0;
}
/* }}} */

static int _php_filter_validate_ipv6(char *str, size_t str_len) /* {{{ */
{
	int compressed = 0;
	int blocks = 0;
	int n;
	char *ipv4;
	char *end;
	int ip4elm[4];
	char *s = str;

	if (!memchr(str, ':', str_len)) {
		return 0;
	}

	/* check for bundled IPv4 */
	ipv4 = memchr(str, '.', str_len);
	if (ipv4) {
 		while (ipv4 > str && *(ipv4-1) != ':') {
			ipv4--;
		}

		if (!_php_filter_validate_ipv4(ipv4, (str_len - (ipv4 - str)), ip4elm)) {
			return 0;
		}

		str_len = ipv4 - str; /* length excluding ipv4 */
		if (str_len < 2) {
			return 0;
		}

		if (ipv4[-2] != ':') {
			/* don't include : before ipv4 unless it's a :: */
			str_len--;
		}

		blocks = 2;
	}

	end = str + str_len;

	while (str < end) {
		if (*str == ':') {
			if (++str >= end) {
				/* cannot end in : without previous : */
				return 0;
			}
			if (*str == ':') {
				if (compressed) {
					return 0;
				}
				blocks++; /* :: means 1 or more 16-bit 0 blocks */
				compressed = 1;

				if (++str == end) {
					return (blocks <= 8);
				}
			} else if ((str - 1) == s) {
				/* don't allow leading : without another : following */
				return 0;
			}
		}
		n = 0;
		while ((str < end) &&
		       ((*str >= '0' && *str <= '9') ||
		        (*str >= 'a' && *str <= 'f') ||
		        (*str >= 'A' && *str <= 'F'))) {
			n++;
			str++;
		}
		if (n < 1 || n > 4) {
			return 0;
		}
		if (++blocks > 8)
			return 0;
	}
	return ((compressed && blocks <= 8) || blocks == 8);
}
/* }}} */

void php_filter_validate_ip(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	/* validates an ipv4 or ipv6 IP, based on the flag (4, 6, or both) add a
	 * flag to throw out reserved ranges; multicast ranges... etc. If both
	 * allow_ipv4 and allow_ipv6 flags flag are used, then the first dot or
	 * colon determine the format */

	int            ip[4];
	int            mode;

	if (memchr(Z_STRVAL_P(value), ':', Z_STRLEN_P(value))) {
		mode = FORMAT_IPV6;
	} else if (memchr(Z_STRVAL_P(value), '.', Z_STRLEN_P(value))) {
		mode = FORMAT_IPV4;
	} else {
		RETURN_VALIDATION_FAILED
	}

	if ((flags & FILTER_FLAG_IPV4) && (flags & FILTER_FLAG_IPV6)) {
		/* Both formats are cool */
	} else if ((flags & FILTER_FLAG_IPV4) && mode == FORMAT_IPV6) {
		RETURN_VALIDATION_FAILED
	} else if ((flags & FILTER_FLAG_IPV6) && mode == FORMAT_IPV4) {
		RETURN_VALIDATION_FAILED
	}

	switch (mode) {
		case FORMAT_IPV4:
			if (!_php_filter_validate_ipv4(Z_STRVAL_P(value), Z_STRLEN_P(value), ip)) {
				RETURN_VALIDATION_FAILED
			}

			/* Check flags */
			if (flags & FILTER_FLAG_NO_PRIV_RANGE) {
				if (
					(ip[0] == 10) ||
					(ip[0] == 172 && ip[1] >= 16 && ip[1] <= 31) ||
					(ip[0] == 192 && ip[1] == 168)
				) {
					RETURN_VALIDATION_FAILED
				}
			}

			if (flags & FILTER_FLAG_NO_RES_RANGE) {
				if (
					(ip[0] == 0) ||
					(ip[0] >= 240) ||
					(ip[0] == 127) ||
					(ip[0] == 169 && ip[1] == 254)
				) {
					RETURN_VALIDATION_FAILED
				}
			}
			break;

		case FORMAT_IPV6:
			{
				int res = 0;
				res = _php_filter_validate_ipv6(Z_STRVAL_P(value), Z_STRLEN_P(value));
				if (res < 1) {
					RETURN_VALIDATION_FAILED
				}
				/* Check flags */
				if (flags & FILTER_FLAG_NO_PRIV_RANGE) {
					if (Z_STRLEN_P(value) >=2 && (!strncasecmp("FC", Z_STRVAL_P(value), 2) || !strncasecmp("FD", Z_STRVAL_P(value), 2))) {
						RETURN_VALIDATION_FAILED
					}
				}
				if (flags & FILTER_FLAG_NO_RES_RANGE) {
					switch (Z_STRLEN_P(value)) {
						case 1: case 0:
							break;
						case 2:
							if (!strcmp("::", Z_STRVAL_P(value))) {
								RETURN_VALIDATION_FAILED
							}
							break;
						case 3:
							if (!strcmp("::1", Z_STRVAL_P(value)) || !strcmp("5f:", Z_STRVAL_P(value))) {
								RETURN_VALIDATION_FAILED
							}
							break;
						default:
							if (Z_STRLEN_P(value) >= 5) {
								if (
									!strncasecmp("fe8", Z_STRVAL_P(value), 3) ||
									!strncasecmp("fe9", Z_STRVAL_P(value), 3) ||
									!strncasecmp("fea", Z_STRVAL_P(value), 3) ||
									!strncasecmp("feb", Z_STRVAL_P(value), 3)
								) {
									RETURN_VALIDATION_FAILED
								}
							}
							if (
								(Z_STRLEN_P(value) >= 9 &&  !strncasecmp("2001:0db8", Z_STRVAL_P(value), 9)) ||
								(Z_STRLEN_P(value) >= 2 &&  !strncasecmp("5f", Z_STRVAL_P(value), 2)) ||
								(Z_STRLEN_P(value) >= 4 &&  !strncasecmp("3ff3", Z_STRVAL_P(value), 4)) ||
								(Z_STRLEN_P(value) >= 8 &&  !strncasecmp("2001:001", Z_STRVAL_P(value), 8))
							) {
								RETURN_VALIDATION_FAILED
							}
					}
				}
			}
			break;
	}
}
/* }}} */

void php_filter_validate_mac(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	char *input = Z_STRVAL_P(value);
	size_t input_len = Z_STRLEN_P(value);
	int tokens, length, i, offset, exp_separator_set;
	size_t exp_separator_len;
	char separator;
	char *exp_separator;
	zend_long ret = 0;
	zval *option_val;

	FETCH_STRING_OPTION(exp_separator, "separator");

	if (exp_separator_set && exp_separator_len != 1) {
		php_error_docref(NULL, E_WARNING, "Separator must be exactly one character long");
		RETURN_VALIDATION_FAILED;
	}

	if (14 == input_len) {
		/* EUI-64 format: Four hexadecimal digits separated by dots. Less
		 * commonly used but valid nonetheless.
		 */
		tokens = 3;
		length = 4;
		separator = '.';
	} else if (17 == input_len && input[2] == '-') {
		/* IEEE 802 format: Six hexadecimal digits separated by hyphens. */
		tokens = 6;
		length = 2;
		separator = '-';
	} else if (17 == input_len && input[2] == ':') {
		/* IEEE 802 format: Six hexadecimal digits separated by colons. */
		tokens = 6;
		length = 2;
		separator = ':';
	} else {
		RETURN_VALIDATION_FAILED;
	}

	if (exp_separator_set && separator != exp_separator[0]) {
		RETURN_VALIDATION_FAILED;
	}

	/* Essentially what we now have is a set of tokens each consisting of
	 * a hexadecimal number followed by a separator character. (With the
	 * exception of the last token which does not have the separator.)
	 */
	for (i = 0; i < tokens; i++) {
		offset = i * (length + 1);

		if (i < tokens - 1 && input[offset + length] != separator) {
			/* The current token did not end with e.g. a "." */
			RETURN_VALIDATION_FAILED
		}
		if (php_filter_parse_hex(input + offset, length, &ret) < 0) {
			/* The current token is no valid hexadecimal digit */
			RETURN_VALIDATION_FAILED
		}
	}
}
/* }}} */
