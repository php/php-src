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
  | Authors: Derick Rethans <derick@php.net>                             |
  |          Pierre-A. Joye <pierre@php.net>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php_filter.h"
#include "filter_private.h"
#include "ext/standard/url.h"
#include "ext/pcre/php_pcre.h"

#include "zend_multiply.h"

#if HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif

#define LONG_SIGN_MASK (1L << (8*sizeof(long)-1))

#ifndef INADDR_NONE
# define INADDR_NONE ((unsigned long int) -1)
#endif


/* {{{ FETCH_LONG_OPTION(var_name, option_name) */
#define FETCH_LONG_OPTION(var_name, option_name)                                                                         \
	var_name = 0;                                                                                                        \
	var_name##_set = 0;                                                                                                  \
	if (option_array) {                                                                                                  \
		if (zend_hash_find(HASH_OF(option_array), option_name, sizeof(option_name), (void **) &option_val) == SUCCESS) { \
			PHP_FILTER_GET_LONG_OPT(option_val, var_name);								\
			var_name##_set = 1;                                                                                          \
		}                                                                                                                \
	}
/* }}} */

/* {{{ FETCH_STRING_OPTION(var_name, option_name) */
#define FETCH_STRING_OPTION(var_name, option_name)                                                                       \
	var_name = NULL;                                                                                                     \
	var_name##_set = 0;                                                                                                  \
	var_name##_len = 0;                                                                                                  \
	if (option_array) {                                                                                                  \
		if (zend_hash_find(HASH_OF(option_array), option_name, sizeof(option_name), (void **) &option_val) == SUCCESS) { \
			if (Z_TYPE_PP(option_val) == IS_STRING) {                                                                    \
				var_name = Z_STRVAL_PP(option_val);                                                                      \
				var_name##_len = Z_STRLEN_PP(option_val);                                                                \
				var_name##_set = 1;                                                                                      \
			}                                                                                                            \
		}                                                                                                                \
	}
/* }}} */

#define FORMAT_IPV4    4
#define FORMAT_IPV6    6

static int php_filter_parse_int(const char *str, unsigned int str_len, long *ret TSRMLS_DC) { /* {{{ */
	long ctx_value;
	long sign = 1;
	const char *end = str + str_len;
	double dval;
	long overflow;

	switch (*str) {
		case '-':
			sign = -1;
		case '+':
			str++;
		default:
			break;
	}

	/* must start with 1..9*/
	if (str < end && *str >= '1' && *str <= '9') {
		ctx_value = ((*(str++)) - '0');
	} else {
		return -1;
	}

	while (str < end) {
		if (*str >= '0' && *str <= '9') {
			ZEND_SIGNED_MULTIPLY_LONG(ctx_value, 10, ctx_value, dval, overflow);
			if (overflow) {
				return -1;
			}
			ctx_value += ((*(str++)) - '0');
			if (ctx_value & LONG_SIGN_MASK) {
				return -1;
			}
		} else {
			return -1;
		}
	}

	*ret = ctx_value * sign;
	return 1;
}
/* }}} */

static int php_filter_parse_octal(const char *str, unsigned int str_len, long *ret TSRMLS_DC) { /* {{{ */
	unsigned long ctx_value = 0;
	const char *end = str + str_len;

	while (str < end) {
		if (*str >= '0' && *str <= '7') {
			unsigned long n = ((*(str++)) - '0');

			if ((ctx_value > ((unsigned long)(~(long)0)) / 8) ||
				((ctx_value = ctx_value * 8) > ((unsigned long)(~(long)0)) - n)) {
				return -1;
			}
			ctx_value += n;
		} else {
			return -1;
		}
	}
	
	*ret = (long)ctx_value;
	return 1;
}
/* }}} */

static int php_filter_parse_hex(const char *str, unsigned int str_len, long *ret TSRMLS_DC) { /* {{{ */
	unsigned long ctx_value = 0;
	const char *end = str + str_len;
	unsigned long n;

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
		if ((ctx_value > ((unsigned long)(~(long)0)) / 16) ||
			((ctx_value = ctx_value * 16) > ((unsigned long)(~(long)0)) - n)) {
			return -1;
		}
		ctx_value += n;
	}

	*ret = (long)ctx_value;
	return 1;
}
/* }}} */

void php_filter_int(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	zval **option_val;
	long   min_range, max_range, option_flags;
	int    min_range_set, max_range_set;
	int    allow_octal = 0, allow_hex = 0;
	int	   len, error = 0;
	long   ctx_value;
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
			if (php_filter_parse_hex(p, len, &ctx_value TSRMLS_CC) < 0) {
				error = 1;
			}
		} else if (allow_octal) {
			if (php_filter_parse_octal(p, len, &ctx_value TSRMLS_CC) < 0) {
				error = 1;
			}
		} else if (len != 0) {
			error = 1;
		}
	} else {
		if (php_filter_parse_int(p, len, &ctx_value TSRMLS_CC) < 0) {
			error = 1;
		}
	}

	if (error > 0 || (min_range_set && (ctx_value < min_range)) || (max_range_set && (ctx_value > max_range))) {
		RETURN_VALIDATION_FAILED
	} else {
		zval_dtor(value);
		Z_TYPE_P(value) = IS_LONG;
		Z_LVAL_P(value) = ctx_value;
		return;
	}
}
/* }}} */

void php_filter_boolean(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	char *str = Z_STRVAL_P(value);
	int len = Z_STRLEN_P(value);
	int ret;

	PHP_FILTER_TRIM_DEFAULT(str, len);

	/* returns true for "1", "true", "on" and "yes"
	 * returns false for "0", "false", "off", "no", and ""
	 * null otherwise. */
	switch (len) {
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
		zval_dtor(value);
		ZVAL_BOOL(value, ret);
	}
}
/* }}} */

void php_filter_float(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	int len;
	char *str, *end;
	char *num, *p;

	zval **option_val;
	char *decimal;
	int decimal_set, decimal_len;
	char dec_sep = '.';
	char tsd_sep[3] = "',.";

	long lval;
	double dval;

	int first, n;

	len = Z_STRLEN_P(value);
	str = Z_STRVAL_P(value);

	PHP_FILTER_TRIM_DEFAULT(str, len);
	end = str + len;

	FETCH_STRING_OPTION(decimal, "decimal");

	if (decimal_set) {
		if (decimal_len != 1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "decimal separator must be one char");
			RETURN_VALIDATION_FAILED
		} else {
			dec_sep = *decimal;
		}
	}

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
		if ((flags & FILTER_FLAG_ALLOW_THOUSAND) && (*str == tsd_sep[0] || *str == tsd_sep[1] || *str == tsd_sep[2])) {
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
			zval_dtor(value);
			Z_TYPE_P(value) = IS_DOUBLE;
			Z_DVAL_P(value) = lval;
			break;
		case IS_DOUBLE:
			if ((!dval && p - num > 1 && strpbrk(num, "123456789")) || !zend_finite(dval)) {
				goto error;
			}
			zval_dtor(value);
			Z_TYPE_P(value) = IS_DOUBLE;
			Z_DVAL_P(value) = dval;
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
	zval **option_val;
	char  *regexp;
	int regexp_len;
	long   option_flags;
	int    regexp_set, option_flags_set;

	pcre       *re = NULL;
	pcre_extra *pcre_extra = NULL;
	int preg_options = 0;

	int         ovector[3];
	int         matches;

	/* Parse options */
	FETCH_STRING_OPTION(regexp, "regexp");
	FETCH_LONG_OPTION(option_flags, "flags");

	if (!regexp_set) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "'regexp' option missing");
		RETURN_VALIDATION_FAILED
	}

	re = pcre_get_compiled_regex(regexp, &pcre_extra, &preg_options TSRMLS_CC);
	if (!re) {
		RETURN_VALIDATION_FAILED
	}
	matches = pcre_exec(re, NULL, Z_STRVAL_P(value), Z_STRLEN_P(value), 0, 0, ovector, 3);

	/* 0 means that the vector is too small to hold all the captured substring offsets */
	if (matches < 0) {
		RETURN_VALIDATION_FAILED
	}
}
/* }}} */

void php_filter_validate_url(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	php_url *url;
	int old_len = Z_STRLEN_P(value);
	
	php_filter_url(value, flags, option_array, charset TSRMLS_CC);

	if (Z_TYPE_P(value) != IS_STRING || old_len != Z_STRLEN_P(value)) {
		RETURN_VALIDATION_FAILED
	}

	/* Use parse_url - if it returns false, we return NULL */
	url = php_url_parse_ex(Z_STRVAL_P(value), Z_STRLEN_P(value));

	if (url == NULL) {
		RETURN_VALIDATION_FAILED
	}

	if (
		url->scheme == NULL || 
		/* some schemas allow the host to be empty */
		(url->host == NULL && (strcmp(url->scheme, "mailto") && strcmp(url->scheme, "news") && strcmp(url->scheme, "file"))) ||
		((flags & FILTER_FLAG_PATH_REQUIRED) && url->path == NULL) || ((flags & FILTER_FLAG_QUERY_REQUIRED) && url->query == NULL)
	) {
		php_url_free(url);
		RETURN_VALIDATION_FAILED
	}
	php_url_free(url);
}
/* }}} */

void php_filter_validate_email(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	/* From http://cvs.php.net/co.php/pear/HTML_QuickForm/QuickForm/Rule/Email.php?r=1.4 */
	const char regexp[] = "/^((\\\"[^\\\"\\f\\n\\r\\t\\b]+\\\")|([\\w\\!\\#\\$\\%\\&\\'\\*\\+\\-\\~\\/\\^\\`\\|\\{\\}]+(\\.[\\w\\!\\#\\$\\%\\&\\'\\*\\+\\-\\~\\/\\^\\`\\|\\{\\}]+)*))@((\\[(((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9]))\\.((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9]))\\.((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9]))\\.((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9])))\\])|(((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9]))\\.((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9]))\\.((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9]))\\.((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9])))|((([A-Za-z0-9])(([A-Za-z0-9\\-])*([A-Za-z0-9]))?\\.)+[A-Za-z\\-]+))$/D";

	pcre       *re = NULL;
	pcre_extra *pcre_extra = NULL;
	int preg_options = 0;
	int         ovector[150]; /* Needs to be a multiple of 3 */
	int         matches;


	re = pcre_get_compiled_regex((char *)regexp, &pcre_extra, &preg_options TSRMLS_CC);
	if (!re) {
		RETURN_VALIDATION_FAILED
	}
	matches = pcre_exec(re, NULL, Z_STRVAL_P(value), Z_STRLEN_P(value), 0, 0, ovector, 3);

	/* 0 means that the vector is too small to hold all the captured substring offsets */
	if (matches < 0) {
		RETURN_VALIDATION_FAILED
	}

}
/* }}} */

static int _php_filter_validate_ipv4(char *str, int str_len, int *ip) /* {{{ */
{
	const char *end = str + str_len;
	int num, m;
	int n = 0;

	while (str < end) {
		if (*str < '0' || *str > '9') {
			return 0;
		}
		m = 1;
		num = ((*(str++)) - '0');
		while (str < end && (*str >= '0' && *str <= '9')) {
			num = num * 10 + ((*(str++)) - '0');
			if (num > 255 || ++m > 3) {
				return 0;
			}
		}
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

static int _php_filter_validate_ipv6(char *str, int str_len TSRMLS_DC) /* {{{ */
{
	int compressed = 0;
	int blocks = 8;
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
		str_len = (ipv4 - str) - 1;
		if (str_len == 1) {
			return *str == ':';
		}
		blocks = 6;
	}

	end = str + str_len;
	while (str < end) {
		if (*str == ':') {
			if (--blocks == 0) {
				return 0;
			}			
			if (++str >= end) {
				return 0;
			}
			if (*str == ':') {
				if (compressed || --blocks == 0) {
					return 0;
				}			
				if (++str == end) {
					return 1;
				}
				compressed = 1;
			} else if ((str - 1) == s) {
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
	}
	return (compressed || blocks == 1);
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

	if (flags & (FILTER_FLAG_IPV4 || FILTER_FLAG_IPV6)) {
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
					(ip[0] == 172 && (ip[1] >= 16 && ip[1] <= 31)) ||
					(ip[0] == 192 && ip[1] == 168)
				) {
					RETURN_VALIDATION_FAILED
				}
			}

			if (flags & FILTER_FLAG_NO_RES_RANGE) {
				if (
					(ip[0] == 0) ||
					(ip[0] == 169 && ip[1] == 254) ||
					(ip[0] == 192 && ip[1] == 0 && ip[2] == 2) ||
					(ip[0] >= 224 && ip[0] <= 255)
				) {
					RETURN_VALIDATION_FAILED
				}
			}
			break;

		case FORMAT_IPV6:
			{
				int res = 0;
				res = _php_filter_validate_ipv6(Z_STRVAL_P(value), Z_STRLEN_P(value) TSRMLS_CC);
				if (res < 1) {
					RETURN_VALIDATION_FAILED
				}
			}
			break;
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
