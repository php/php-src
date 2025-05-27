/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
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

#ifdef HAVE_ARPA_INET_H
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

static bool _php_filter_validate_ipv6(const char *str, size_t str_len, int ip[8]);

static bool php_filter_parse_int(const char *str, size_t str_len, zend_long *ret) { /* {{{ */
	zend_long ctx_value;
	bool is_negative = false;
	int digit = 0;
	const char *end = str + str_len;

	switch (*str) {
		case '-':
			is_negative = true;
			ZEND_FALLTHROUGH;
		case '+':
			str++;
		default:
			break;
	}

	if (*str == '0' && str + 1 == end) {
		/* Special cases: +0 and -0 */
		return true;
	}

	/* must start with 1..9*/
	if (str < end && *str >= '1' && *str <= '9') {
		ctx_value = (is_negative?-1:1) * ((*(str++)) - '0');
	} else {
		return false;
	}

	if ((end - str > MAX_LENGTH_OF_LONG - 1) /* number too long */
	 || (SIZEOF_LONG == 4 && (end - str == MAX_LENGTH_OF_LONG - 1) && *str > '2')) {
		/* overflow */
		return false;
	}

	while (str < end) {
		if (*str >= '0' && *str <= '9') {
			digit = (*(str++) - '0');
			if ( (!is_negative) && ctx_value <= (ZEND_LONG_MAX-digit)/10 ) {
				ctx_value = (ctx_value * 10) + digit;
			} else if ( is_negative && ctx_value >= (ZEND_LONG_MIN+digit)/10) {
				ctx_value = (ctx_value * 10) - digit;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	*ret = ctx_value;
	return true;
}
/* }}} */

static bool php_filter_parse_octal(const char *str, size_t str_len, zend_long *ret) { /* {{{ */
	zend_ulong ctx_value = 0;
	const char *end = str + str_len;

	while (str < end) {
		if (*str >= '0' && *str <= '7') {
			zend_ulong n = ((*(str++)) - '0');

			if ((ctx_value > ((zend_ulong)(~(zend_long)0)) / 8) ||
				((ctx_value = ctx_value * 8) > ((zend_ulong)(~(zend_long)0)) - n)) {
				return false;
			}
			ctx_value += n;
		} else {
			return false;
		}
	}

	*ret = (zend_long)ctx_value;
	return true;
}
/* }}} */

static bool php_filter_parse_hex(const char *str, size_t str_len, zend_long *ret) { /* {{{ */
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
			return false;
		}
		if ((ctx_value > ((zend_ulong)(~(zend_long)0)) / 16) ||
			((ctx_value = ctx_value * 16) > ((zend_ulong)(~(zend_long)0)) - n)) {
			return false;
		}
		ctx_value += n;
	}

	*ret = (zend_long)ctx_value;
	return true;
}
/* }}} */

void php_filter_int(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	zval *option_val;
	zend_long  min_range, max_range, option_flags;
	int   min_range_set, max_range_set;
	bool allow_octal = false, allow_hex = false;
	size_t	  len;
	bool error = false;
	zend_long  ctx_value;
	const char *p;

	/* Parse options */
	FETCH_LONG_OPTION(min_range,    "min_range");
	FETCH_LONG_OPTION(max_range,    "max_range");
	option_flags = flags;

	len = Z_STRLEN_P(value);

	if (len == 0) {
		RETURN_VALIDATION_FAILED
	}

	if (option_flags & FILTER_FLAG_ALLOW_OCTAL) {
		allow_octal = true;
	}

	if (option_flags & FILTER_FLAG_ALLOW_HEX) {
		allow_hex = true;
	}

	/* Start the validating loop */
	p = Z_STRVAL_P(value);
	ctx_value = 0;

	PHP_FILTER_TRIM_DEFAULT(p, len);

	if (*p == '0') {
		p++; len--;
		if (allow_hex && (*p == 'x' || *p == 'X')) {
			p++; len--;
			if (len == 0) {
				RETURN_VALIDATION_FAILED
			}
			if (!php_filter_parse_hex(p, len, &ctx_value)) {
				error = true;
			}
		} else if (allow_octal) {
			/* Support explicit octal prefix notation */
			if (*p == 'o' || *p == 'O') {
				p++; len--;
				if (len == 0) {
					RETURN_VALIDATION_FAILED
				}
			}
			if (!php_filter_parse_octal(p, len, &ctx_value)) {
				error = true;
			}
		} else if (len != 0) {
			error = true;
		}
	} else {
		if (!php_filter_parse_int(p, len, &ctx_value)) {
			error = true;
		}
	}

	if (error || (min_range_set && (ctx_value < min_range)) || (max_range_set && (ctx_value > max_range))) {
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
	const char *str = Z_STRVAL_P(value);
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
	const char *str, *end;
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

	int n;

	len = Z_STRLEN_P(value);
	str = Z_STRVAL_P(value);

	PHP_FILTER_TRIM_DEFAULT(str, len);
	end = str + len;

	FETCH_STRING_OPTION(decimal, "decimal");

	if (decimal_set) {
		if (decimal_len != 1) {
			zend_value_error("%s(): \"decimal\" option must be one character long", get_active_function_name());
			RETURN_VALIDATION_FAILED
		} else {
			dec_sep = *decimal;
		}
	}

	FETCH_STRING_OPTION(thousand, "thousand");

	if (thousand_set) {
		if (thousand_len < 1) {
			zend_value_error("%s(): \"thousand\" option must not be empty", get_active_function_name());
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
	bool first = true;
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
			first = false;
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
			if ((min_range_set && (lval < min_range)) || (max_range_set && (lval > max_range))) {
				goto error;
			}
			zval_ptr_dtor(value);
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
		zend_value_error("%s(): \"regexp\" option is missing", get_active_function_name());
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

static bool php_filter_validate_domain_ex(const zend_string *domain, zend_long flags) /* {{{ */
{
	const char *e, *s, *t;
	size_t l;
	int hostname = flags & FILTER_FLAG_HOSTNAME;
	unsigned char i = 1;

	s = ZSTR_VAL(domain);
	l = ZSTR_LEN(domain);
	e = s + l;
	t = e - 1;

	/* Ignore trailing dot */
	if (l > 0 && *t == '.') {
		e = t;
		l--;
	}

	/* The total length cannot exceed 253 characters (final dot not included) */
	if (l > 253) {
		return false;
	}

	/* First char must be alphanumeric */
	if(*s == '.' || (hostname && !isalnum((int)*(unsigned char *)s))) {
		return false;
	}

	while (s < e) {
		if (*s == '.') {
			/* The first and the last character of a label must be alphanumeric */
			if (*(s + 1) == '.' || (hostname && (!isalnum((int)*(unsigned char *)(s - 1)) || !isalnum((int)*(unsigned char *)(s + 1))))) {
				return false;
			}

			/* Reset label length counter */
			i = 1;
		} else {
			if (i > 63 || (hostname && (*s != '-' || *(s + 1) == '\0') && !isalnum((int)*(unsigned char *)s))) {
				return false;
			}

			i++;
		}

		s++;
	}

	return true;
}
/* }}} */

void php_filter_validate_domain(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	if (!php_filter_validate_domain_ex(Z_STR_P(value), flags)) {
		RETURN_VALIDATION_FAILED
	}
}
/* }}} */

static bool is_userinfo_valid(const zend_string *str)
{
	const char *p = ZSTR_VAL(str);
	while (p - ZSTR_VAL(str) < ZSTR_LEN(str)) {
		static const char *valid = "-._~!$&'()*+,;=:";
		if (isalpha(*p) || isdigit(*p) || strchr(valid, *p)) {
			p++;
		} else if (*p == '%' && p - ZSTR_VAL(str) <= ZSTR_LEN(str) - 3 && isdigit(*(p+1)) && isxdigit(*(p+2))) {
			p += 3;
		} else {
			return false;
		}
	}
	return true;
}

static bool php_filter_is_valid_ipv6_hostname(const zend_string *s)
{
	const char *e = ZSTR_VAL(s) + ZSTR_LEN(s);
	const char *t = e - 1;

	return *ZSTR_VAL(s) == '[' && *t == ']' && _php_filter_validate_ipv6(ZSTR_VAL(s) + 1, ZSTR_LEN(s) - 2, NULL);
}

void php_filter_validate_url(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	php_url *url;
	size_t old_len = Z_STRLEN_P(value);

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

		if (url->host == NULL) {
			goto bad_url;
		}

		if (
			/* An IPv6 enclosed by square brackets is a valid hostname.*/
			!php_filter_is_valid_ipv6_hostname(url->host) &&
			/* Validate domain.
			 * This includes a loose check for an IPv4 address. */
			!php_filter_validate_domain_ex(url->host, FILTER_FLAG_HOSTNAME)
		) {
			php_url_free(url);
			RETURN_VALIDATION_FAILED
		}
	}

	if (
		url->scheme == NULL ||
		/* some schemas allow the host to be empty */
		(url->host == NULL && (!zend_string_equals_literal(url->scheme, "mailto") && !zend_string_equals_literal(url->scheme, "news") && !zend_string_equals_literal(url->scheme, "file"))) ||
		((flags & FILTER_FLAG_PATH_REQUIRED) && url->path == NULL) || ((flags & FILTER_FLAG_QUERY_REQUIRED) && url->query == NULL)
	) {
bad_url:
		php_url_free(url);
		RETURN_VALIDATION_FAILED
	}

	if ((url->user != NULL && !is_userinfo_valid(url->user))
		|| (url->pass != NULL && !is_userinfo_valid(url->pass))
	) {
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

static bool _php_filter_validate_ipv4(const char *str, size_t str_len, int *ip) /* {{{ */
{
	const char *end = str + str_len;
	int num, m;
	int n = 0;

	while (str < end) {
		bool leading_zero;
		if (*str < '0' || *str > '9') {
			return false;
		}
		leading_zero = (*str == '0');
		m = 1;
		num = ((*(str++)) - '0');
		while (str < end && (*str >= '0' && *str <= '9')) {
			num = num * 10 + ((*(str++)) - '0');
			if (num > 255 || ++m > 3) {
				return false;
			}
		}
		/* don't allow a leading 0; that introduces octal numbers,
		 * which we don't support */
		if (leading_zero && (num != 0 || m > 1))
			return false;
		ip[n++] = num;
		if (n == 4) {
			return str == end;
		} else if (str >= end || *(str++) != '.') {
			return false;
		}
	}
	return false;
}
/* }}} */

static bool _php_filter_validate_ipv6(const char *str, size_t str_len, int ip[8]) /* {{{ */
{
	int compressed_pos = -1;
	int blocks = 0;
	unsigned int num, n;
	int i;
	const char *ipv4;
	const char *end;
	int ip4elm[4];
	const char *s = str;

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
				return false;
			}
			if (*str == ':') {
				if (compressed_pos >= 0) {
					return false;
				}
				if (ip && blocks < 8) {
					ip[blocks] = -1;
				}
				compressed_pos = blocks++; /* :: means 1 or more 16-bit 0 blocks */
				if (++str == end) {
					if (blocks > 8) {
						return false;
				}
					goto fixup_ip;
				}
			} else if ((str - 1) == s) {
				/* don't allow leading : without another : following */
				return false;
			}
		}
		num = n = 0;
		while (str < end) {
			if (*str >= '0' && *str <= '9') {
				num = 16 * num + (*str - '0');
			} else if (*str >= 'a' && *str <= 'f') {
				num = 16 * num + (*str - 'a') + 10;
			} else if (*str >= 'A' && *str <= 'F') {
				num = 16 * num + (*str - 'A') + 10;
			} else {
				break;
			}
			n++;
			str++;
		}
		if (ip && blocks < 8) {
			ip[blocks] = num;
		}
		if (n < 1 || n > 4) {
			return false;
		}
		if (++blocks > 8)
			return false;
	}

fixup_ip:
	if (ip && ipv4) {
		for (i = 0; i < 5; i++) {
			ip[i] = 0;
}
		ip[i++] = 0xffff;
		ip[i++] = 256 * ip4elm[0] + ip4elm[1];
		ip[i++] = 256 * ip4elm[2] + ip4elm[3];
	} else if (ip && compressed_pos >= 0 && blocks <= 8) {
		int offset = 8 - blocks;
		for (i = 7; i > compressed_pos + offset; i--) {
			ip[i] = ip[i - offset];
		}
		for (i = compressed_pos + offset; i >= compressed_pos; i--) {
			ip[i] = 0;
		}
	}

	return (compressed_pos >= 0 && blocks <= 8) || blocks == 8;
}
/* }}} */

/* From the tables in RFC 6890 - Special-Purpose IP Address Registriesi
 * Including errata: https://www.rfc-editor.org/errata_search.php?rfc=6890&rec_status=1 */
static bool ipv4_get_status_flags(const int ip[8], bool *global, bool *reserved, bool *private)
{
	*global = false;
	*reserved = false;
	*private = false;

	if (ip[0] == 0) {
		/* RFC 0791 - This network */
		*reserved = true;
	} else if (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0) {
		/* RFC 1122 - This host on this network */
		*reserved = true;
	} else if (ip[0] == 10) {
		/* RFC 1918 - Private Use */
		*private = true;
	} else if (ip[0] == 100 && ip[1] >= 64 && ip[1] <= 127) {
		/* RFC 6598 - Shared Address Space */
	} else if (ip[0] == 127) {
		/* RFC 1122 - Loopback */
		*reserved = true;
	} else if (ip[0] == 169 && ip[1] == 254) {
		/* RFC 3927 - Link Local */
		*reserved = true;
	} else if (ip[0] == 172 && ip[1] >= 16 && ip[1] <= 31) {
		/* RFC 1918 - Private Use */
		*private = true;
	} else if (ip[0] == 192 && ip[1] == 0 && ip[2] == 0) {
		/* RFC 6890 - IETF Protocol Assignments */
	} else if (ip[0] == 192 && ip[1] == 0 && ip[2] == 0 && ip[3] >= 0 && ip[3] <= 7) {
		/* RFC 6333 - DS-Lite */
	} else if (ip[0] == 192 && ip[1] == 0 && ip[2] == 2) {
		/* RFC 5737 - Documentation */
	} else if (ip[0] == 192 && ip[1] == 88 && ip[2] == 99) {
		/* RFC 3068 - 6to4 Relay Anycast */
		*global = true;
	} else if (ip[0] == 192 && ip[1] == 168) {
		/* RFC 1918 - Private Use */
		*private = true;
	} else if (ip[0] == 198 && ip[1] >= 18 && ip[1] <= 19) {
		/* RFC 2544 - Benchmarking */
	} else if (ip[0] == 198 && ip[1] == 51 && ip[2] == 100) {
		/* RFC 5737 - Documentation */
	} else if (ip[0] == 203 && ip[1] == 0 && ip[2] == 113) {
		/* RFC 5737 - Documentation */
	} else if (ip[0] >= 240 && ip[1] <= 255) {
		/* RFC 1122 - Reserved */
		*reserved = true;
	} else if (ip[0] == 255 && ip[1] == 255 && ip[2] == 255 && ip[3] == 255) {
		/* RFC 0919 - Limited Broadcast, Updated by RFC 8190, 2.2. */
		*reserved = true;
	} else {
		return false;
	}

	return true;
}

/* From the tables in RFC 6890 - Special-Purpose IP Address Registries */
static bool ipv6_get_status_flags(const int ip[8], bool *global, bool *reserved, bool *private)
{
	*global = false;
	*reserved = false;
	*private = false;

	if (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0 && ip[4] == 0 && ip[5] == 0 && ip[6] == 0 && ip[7] == 0) {
		/* RFC 4291 - Unspecified Address */
		*reserved = true;
	} else if (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0 && ip[4] == 0 && ip[5] == 0 && ip[6] == 0 && ip[7] == 1) {
		/* RFC 4291 - Loopback Address */
		*reserved = true;
	} else if (ip[0] == 0x0064 && ip[1] == 0xff9b) {
		/* RFC 6052 - IPv4-IPv6 Translation */
		*global = true;
	} else if (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0 && ip[4] == 0 && ip[5] == 0xffff) {
		/* RFC 4291 - IPv4-mapped Address */
		*reserved = true;
	} else if (ip[0] == 0x0100 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0) {
		/* RFC 6666 - Discard-Only Address Block */
	} else if (ip[0] == 0x2001 && ip[1] == 0x0000) {
		/* RFC 4380 - TEREDO */
	} else if (ip[0] == 0x2001 && ip[1] <= 0x01ff) {
		/* RFC 2928 - IETF Protocol Assignments */
	} else if (ip[0] == 0x2001 && ip[1] == 0x0002 && ip[2] == 0) {
		/* RFC 5180 - Benchmarking */
	} else if (ip[0] == 0x2001 && ip[1] == 0x0db8) {
		/* RFC 3849 - Documentation */
	} else if (ip[0] == 0x2001 && ip[1] >= 0x0010 && ip[1] <= 0x001f) {
		/* RFC 4843 - ORCHID */
	} else if (ip[0] == 0x2002) {
		/* RFC 3056 - 6to4 */
	} else if (ip[0] >= 0xfc00 && ip[0] <= 0xfdff) {
		/* RFC 4193 - Unique-Local */
		*private = true;
	} else if (ip[0] >= 0xfe80 && ip[0] <= 0xfebf) {
		/* RFC 4291 - Linked-Scoped Unicast */
		*reserved = true;
	} else {
		return false;
	}

	return true;
}

/* Validates an ipv4 or ipv6 IP, based on the flag (4, 6, or both) add a flag
 * to throw out reserved ranges; multicast ranges... etc. If both allow_ipv4
 * and allow_ipv6 flags flag are used, then the first dot or colon determine
 * the format */
void php_filter_validate_ip(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	int  ip[8];
	int  mode;
	bool flag_global, flag_reserved, flag_private; /* flags for ranges as determined by RFC 6890 */

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

	if (mode == FORMAT_IPV4) {
		if (!_php_filter_validate_ipv4(Z_STRVAL_P(value), Z_STRLEN_P(value), ip)) {
			RETURN_VALIDATION_FAILED
		}

		if (!ipv4_get_status_flags(ip, &flag_global, &flag_reserved, &flag_private)) {
			return; /* no special block */
		}
	}
	else if (mode == FORMAT_IPV6) {
		if (!_php_filter_validate_ipv6(Z_STRVAL_P(value), Z_STRLEN_P(value), ip)) {
			RETURN_VALIDATION_FAILED
		}

		if (!ipv6_get_status_flags(ip, &flag_global, &flag_reserved, &flag_private)) {
			return; /* no special block */
		}
	}

	if ((flags & FILTER_FLAG_GLOBAL_RANGE) && flag_global != true) {
		RETURN_VALIDATION_FAILED
	}

	if ((flags & FILTER_FLAG_NO_PRIV_RANGE) && flag_private == true) {
		RETURN_VALIDATION_FAILED
	}

	if ((flags & FILTER_FLAG_NO_RES_RANGE) && flag_reserved == true) {
		RETURN_VALIDATION_FAILED
	}
}
/* }}} */

void php_filter_validate_mac(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	const char *input = Z_STRVAL_P(value);
	size_t input_len = Z_STRLEN_P(value);
	int tokens, length, exp_separator_set;
	size_t exp_separator_len;
	char separator;
	char *exp_separator;
	zend_long ret = 0;
	zval *option_val;

	FETCH_STRING_OPTION(exp_separator, "separator");

	if (exp_separator_set && exp_separator_len != 1) {
		zend_value_error("%s(): \"separator\" option must be one character long", get_active_function_name());
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
	for (int i = 0; i < tokens; i++) {
		int offset = i * (length + 1);

		if (i < tokens - 1 && input[offset + length] != separator) {
			/* The current token did not end with e.g. a "." */
			RETURN_VALIDATION_FAILED
		}
		if (!php_filter_parse_hex(input + offset, length, &ret)) {
			/* The current token is no valid hexadecimal digit */
			RETURN_VALIDATION_FAILED
		}
	}
}
/* }}} */
