/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2006 The PHP Group                                |
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

/* {{{ FETCH_LONG_OPTION(var_name, option_name) */
#define FETCH_LONG_OPTION(var_name, option_name)                                                                         \
	var_name = 0;                                                                                                        \
	var_name##_set = 0;                                                                                                  \
	if (option_array) {                                                                                                  \
		if (zend_hash_find(HASH_OF(option_array), option_name, sizeof(option_name), (void **) &option_val) == SUCCESS) { \
			convert_to_long(*option_val);                                                                                \
			var_name = Z_LVAL_PP(option_val);                                                                            \
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
			convert_to_string(*option_val);                                                                              \
			var_name = Z_STRVAL_PP(option_val);                                                                          \
			var_name##_set = 1;                                                                                          \
			var_name##_len = Z_STRLEN_PP(option_val);                                                                    \
		}                                                                                                                \
	}
/* }}} */

#define TYPE_INT   1
#define TYPE_HEX   2
#define TYPE_OCTAL 3
#define FORMAT_IPV4    4
#define FORMAT_IPV6    6

void php_filter_int(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	zval **option_val;
	long   min_range, max_range, option_flags;
	int    min_range_set, max_range_set, option_flags_set;
	int    allow_octal = 0, allow_hex = 0;
	int	   len;
	long   ctx_type, ctx_value, ctx_multiply;
	char *p, *start, *end;

	/* Parse options */
	FETCH_LONG_OPTION(min_range,    "min_range");
	FETCH_LONG_OPTION(max_range,    "max_range");
	FETCH_LONG_OPTION(option_flags, "flags");

	if (option_flags_set && (option_flags & FILTER_FLAG_ALLOW_OCTAL)) {
		allow_octal = 1;
	}

	if (option_flags_set && (option_flags & FILTER_FLAG_ALLOW_HEX)) {
		allow_hex = 1;
	}

	len = Z_STRLEN_P(value);

	if (len == 0) {
		if (min_range_set && (0 < min_range)) {
			goto stateE;
		}
		if (max_range_set && (0 > max_range)) {
			goto stateE;
		}
		zval_dtor(value);
		Z_TYPE_P(value) = IS_LONG;
		Z_LVAL_P(value) = 0;
		return;
	}

	/* Start the validating loop */
	p = Z_STRVAL_P(value);
	ctx_type = TYPE_INT;
	ctx_value = 0;
	ctx_multiply = 1;

	/* fast(er) trim */
	while (*p == ' ') {
		p++;
		len--;
	}
	start = p;

	end = p + len - 1;
	if (*end == ' ') {
		unsigned int i;
		for (i = len - 1; i >= 0; i--) {
			if (p[i] != ' ') {
				break;
			}
		}
		i++;
		p[i] = '\0';
		end = p + i - 1;
	}

	/* state 0 */
	if (*p == '-' || *p == '+') {
		ctx_multiply = *p == '-' ? -1 : 1; p++;
	}
	goto stateOH1;

stateI1: /* state "integer 1" */
	if (*p >= '1' && *p <= '9') {
		ctx_value += ((*p) - '0'); p++; goto stateI2;
	} else {
		goto stateE;
	}

stateI2: /* state "integer 2" */
	if (*p >= '0' && *p <= '9') {
		ctx_value *= 10; ctx_value += ((*p) - '0'); p++; goto stateI2;
	} else {
		goto stateT;
	}

stateOH1: /* state "octal or hex" */
	if (*p != '0') {
		goto stateI1;
	}
	p++;
	if (*p == 'x' || *p == 'X') {
		if (allow_hex) {
			p++;
			goto stateH1;
		} else {
			goto stateE;
		}
	} else if (*p >= '0' && *p <= '7') {
		if (!allow_octal) {
			goto stateE;
		}
		ctx_value = ((*p) - '0'); p++; goto stateO1;
	}
	goto stateT; /* lone 0 */

stateO1: /* state "octal 1" */
	ctx_type = TYPE_OCTAL;
	if (*p >= '0' && *p <= '7') {
		ctx_value *= 8; ctx_value += ((*p) - '0'); p++; goto stateO1;
	} else {
		goto stateT;
	}

stateH1: /* state "hex 1" */
	ctx_type = TYPE_HEX;
	if ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')) {
		ctx_value *= 16;
		if (*p >= '0' && *p <= '9') {
			ctx_value += ((*p) - '0');
		}
		if (*p >= 'a' && *p <= 'f') {
			ctx_value += 10 + ((*p) - 'a');
		}
		if (*p >= 'A' && *p <= 'F') {
			ctx_value += 10 + ((*p) - 'A');
		}
		p++;
		goto stateH1;
	} else {
		goto stateT;
	}

stateT: /* state "tail" */
	if (*p != '\0' || (p-1) != end) {
		goto stateE;
	} else {
		goto stateR;
	}

stateR: /* state 'return' */
	ctx_value *= ctx_multiply;
	if (min_range_set && (ctx_value < min_range)) {
		goto stateE;
	}
	if (max_range_set && (ctx_value > max_range)) {
		goto stateE;
	}
	zval_dtor(value);
	Z_TYPE_P(value) = IS_LONG;
	Z_LVAL_P(value) = ctx_value;
	return;

	/* state "error" */
stateE:
	zval_dtor(value);
	ZVAL_BOOL(value, 0);
}
/* }}} */

void php_filter_boolean(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	char *str = Z_STRVAL_P(value);

	if (str) {
		/* fast(er) trim */
		while (*str == ' ') {
			str++;
		}
	}

	/* returns true for "1", "true", "on" and "yes"
	 * returns false for "0", "false", "off", "no", and ""
	 * null otherwise. */
	if ((strncasecmp(str, "true", sizeof("true")) == 0) ||
		(strncasecmp(str, "yes", sizeof("yes")) == 0) ||
		(strncasecmp(str, "on", sizeof("on")) == 0) ||
		(strncmp(str, "1", sizeof("1")) == 0))
	{
		zval_dtor(value);
		ZVAL_BOOL(value, 1);
	} else if ((strncasecmp(str, "false", sizeof("false")) == 0) ||
		(strncasecmp(str, "off", sizeof("off")) == 0) ||
		(strncasecmp(str, "no", sizeof("no")) == 0) ||
		(strncmp(str, "0", sizeof("0")) == 0) ||
		Z_STRLEN_P(value) == 0)
	{
		zval_dtor(value);
		ZVAL_BOOL(value, 0);
	} else {
		zval_dtor(value);
		ZVAL_BOOL(value, 0);
	}
}
/* }}} */

void php_filter_float(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	int len;
	char *str, *start, *end;

	zval **option_val;
	char *decimal;
	char dec_sep = '\0';

	const char default_decimal[] = ".";
	int decimal_set, decimal_len;

	char tsd_sep[3] = "',.";

	long options_flag;
	int options_flag_set;

	int sign = 1;

	double ret_val = 0;
	double factor;

	int exp_value = 0, exp_multiply = 1;

	len = Z_STRLEN_P(value);

	if (len < 1) {
		zval_dtor(value);
		Z_TYPE_P(value) = IS_DOUBLE;
		Z_DVAL_P(value) = 0;
		return;
	}

	str = Z_STRVAL_P(value);
	start = str;

	if (len == 1) {
		if (*str >= '0' && *str <= '9') {
			ret_val = (double)*str - '0';
		} else if (*str == 'E' || *str == 'e') {
			ret_val = 0;
		}
		zval_dtor(value);
		Z_TYPE_P(value) = IS_DOUBLE;
		Z_DVAL_P(value) = ret_val;
		return;
	}

	FETCH_STRING_OPTION(decimal, "decimal");
	FETCH_LONG_OPTION(options_flag, "flags");

	if (decimal_set) {
		if (decimal_len > 1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "decimal separator must be one char");
		} else {
			dec_sep = *decimal;
		}
	} else {
		dec_sep = *default_decimal;
	}

	/* fast(er) trim */
	while (*str == ' ') {
		str++;
		len--;
	}
	start = str;

	end = str + len - 1;
	if (*end == ' ') {
		int i;
		for (i = len - 1; i >= 0; i--) {
			if (str[i] != ' ') {
				break;
			}
		}
		i++;
		str[i] = '\0';
		end = str + i - 1;
	}

	if (*str == '-') {
		sign = -1;
		str++;
		start = str;
	} else if (*str == '+') {
		sign = 1;
		str++;
		start = str;
	}

	ret_val = 0.0;

	if (*str == '0') {
		/* leading zeros */
		while (*(str++) == '0');
	}

	if (*str == dec_sep) {
		str++;
		goto stateDot;
	}

	ret_val = 0;

	if (str != start) {
	   	str--;
	}

	while (*str && *str != dec_sep) {
		if ((options_flag & FILTER_FLAG_ALLOW_THOUSAND) && (*str == tsd_sep[0] || *str == tsd_sep[1] || *str == tsd_sep[2])) {
			str++;
			continue;
		}

		if (*str == 'e' || *str == 'E') {
			goto stateExp;
		}

		if (*str < '0' || *str > '9') {
			goto stateError;
		}

		ret_val *=10; ret_val += (*str - '0');
		str++;
	}
	if (!(*str)) {
		goto stateT;
	}
	str++;

stateDot:
	factor = 0.1;
	while (*str) {
		if (*str == 'e' || *str == 'E') {
			goto stateExp;
		}

		if (*str < '0' || *str > '9') {
			goto stateError;
		}

		ret_val += factor * (*str - '0');
		factor /= 10;
		str++;
	}
	if (!(*str)) {
		goto stateT;
	}

stateExp:
	str++;
	switch (*str) {
		case '-':
			exp_multiply = -1;
			str++;
			break;
		case '+':
			exp_multiply = 1;
			str++;
	}

	while (*str) {
		if (*str < '0' || *str > '9') {
			goto stateError;
		}
		exp_value *= 10;
		exp_value += ((*str) - '0');
		str++;
	}

stateT:
	if ((str -1) != end) {
		goto stateError;
	}
	if (exp_value) {
		exp_value *= exp_multiply;
		ret_val *= pow(10, exp_value);
	}

	zval_dtor(value);
	Z_TYPE_P(value) = IS_DOUBLE;
	Z_DVAL_P(value) = sign * ret_val;
	return;

stateError:
	zval_dtor(value);
	ZVAL_BOOL(value, 0);
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
		zval_dtor(value);
		ZVAL_BOOL(value, 0);
		return;
	}

	re = pcre_get_compiled_regex(regexp, &pcre_extra, &preg_options TSRMLS_CC);
	if (!re) {
		zval_dtor(value);
		ZVAL_BOOL(value, 0);
	}
	matches = pcre_exec(re, NULL, Z_STRVAL_P(value), Z_STRLEN_P(value), 0, 0, ovector, 3);

	/* 0 means that the vector is too small to hold all the captured substring offsets */
	if (matches < 0) {
		zval_dtor(value);
		ZVAL_BOOL(value, 0);
	}
}
/* }}} */

void php_filter_validate_url(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	php_url *url;

	/* Use parse_url - if it returns false, we return NULL */
	url = php_url_parse_ex(Z_STRVAL_P(value), Z_STRLEN_P(value));

	if (url == NULL) {
		zval_dtor(value);
		ZVAL_BOOL(value, 0);
		return;
	}

	if ((flags & FILTER_FLAG_SCHEME_REQUIRED) && url->scheme == NULL) {
		zval_dtor(value);
		ZVAL_BOOL(value, 0);
	}
	if ((flags & FILTER_FLAG_HOST_REQUIRED) && url->host == NULL) {
		zval_dtor(value);
		ZVAL_BOOL(value, 0);
	}
	if ((flags & FILTER_FLAG_PATH_REQUIRED) && url->path == NULL) {
		zval_dtor(value);
		ZVAL_BOOL(value, 0);
	}
	if ((flags & FILTER_FLAG_QUERY_REQUIRED) && url->query == NULL) {
		zval_dtor(value);
		ZVAL_BOOL(value, 0);
	}
	php_url_free(url);
}
/* }}} */

void php_filter_validate_email(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	/* From http://cvs.php.net/co.php/pear/HTML_QuickForm/QuickForm/Rule/Email.php?r=1.4 */
	const char regexp[] = "/^((\\\"[^\\\"\\f\\n\\r\\t\\v\\b]+\\\")|([\\w\\!\\#\\$\\%\\&\\'\\*\\+\\-\\~\\/\\^\\`\\|\\{\\}]+(\\.[\\w\\!\\#\\$\\%\\&\\'\\*\\+\\-\\~\\/\\^\\`\\|\\{\\}]+)*))@((\\[(((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9]))\\.((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9]))\\.((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9]))\\.((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9])))\\])|(((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9]))\\.((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9]))\\.((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9]))\\.((25[0-5])|(2[0-4][0-9])|([0-1]?[0-9]?[0-9])))|((([A-Za-z0-9\\-])+\\.)+[A-Za-z\\-]+))$/";

	pcre       *re = NULL;
	pcre_extra *pcre_extra = NULL;
	int preg_options = 0;
	int         ovector[150]; /* Needs to be a multiple of 3 */
	int         matches;


	re = pcre_get_compiled_regex((char *)regexp, &pcre_extra, &preg_options TSRMLS_CC);
	if (!re) {
		zval_dtor(value);
		ZVAL_BOOL(value, 0);
	}
	matches = pcre_exec(re, NULL, Z_STRVAL_P(value), Z_STRLEN_P(value), 0, 0, ovector, 3);

	/* 0 means that the vector is too small to hold all the captured substring offsets */
	if (matches < 0) {
		zval_dtor(value);
		ZVAL_BOOL(value, 0);
	}

}
/* }}} */

static int _php_filter_validate_ipv4_count_dots(char *str) /* {{{ */
{
	char *s1, *s2, *s3, *s4;

	s1 = strchr(str, '.');
	if (!s1)
		return 0;
	s2 = strchr(s1 + 1, '.');
	if (!s2)
		return 1;
	s3 = strchr(s2 + 1, '.');
	if (!s3)
		return 2;
	s4 = strchr(s3 + 1, '.');
	if (!s4)
		return 3;
	return 4; /* too many */
}
/* }}} */

static int _php_filter_validate_ipv4_get_nr(char **str) /* {{{ */
{
	char *begin, *end, *ptr, *tmp_str;
	int   tmp_nr = -1;

	begin = ptr = *str;
	while ((*ptr >= '0') && (*ptr <= '9')) {
		++ptr;
	}
	end = ptr;
	*str = end + 1;

	if (end == begin) {
		return -1;
	}

	tmp_str = calloc(1, end - begin + 1);
	memcpy(tmp_str, begin, end - begin);
	tmp_nr = strtol(tmp_str, NULL, 10);
	free(tmp_str);

	if (tmp_nr < 0 || tmp_nr > 255) {
		tmp_nr = -1;
	}
	return tmp_nr;
}
/* }}} */

static int _php_filter_validate_ipv4(char *str, int *ip TSRMLS_DC) /* {{{ */
{
	char *p;
	int x;

	if (_php_filter_validate_ipv4_count_dots(str) != 3) {
		return 0;
	}

	p = str;
	for (x = 0; x < 4; ++x) {
		ip[x] = _php_filter_validate_ipv4_get_nr(&p);
		if (ip[x] == -1) {
			return 0;
		}
	}
	return 1;
}
/* }}} */

#define IS_HEX(s) if (!((s >= '0' && s <= '9') || (s >= 'a' && s <= 'f') ||(s >= 'A' && s <= 'F'))) { \
	return 0; \
}

#define IPV6_LOOP_IN(str) \
			if (*str == ':') { \
				if (hexcode_found > 4) { 	\
					return -134; 			\
				}							\
				hexcode_found = 0; 			\
				col_fnd++; 					\
			} else { 						\
				IS_HEX(*str); 				\
				hexcode_found++; 			\
			}

static int _php_filter_validate_ipv6_(char *str TSRMLS_DC) /* {{{ */
{
	int hexcode_found = 0;
	int compressed_2end = 0;
	int col_fnd = 0;
	char *start = str;
	char *compressed = NULL, *t = str;
	char *s2 = NULL, *ipv4=NULL;
	int ip4elm[4];

	if (!strchr(str, ':')) {
		return 0;
	}

	/* Check for compressed expression. only one is allowed */
	compressed = strstr(str, "::");
	if (compressed) {
		s2 = strstr(compressed+1, "::");
		if (s2) {
			return 0;
		}
	}

	/* check for bundled IPv4 */
	ipv4 = strchr(str, '.');

	if (ipv4) {
		while (*ipv4 != ':' && ipv4 >= start) {
			ipv4--;
		}

		/* ::w.x.y.z */
		if (compressed && ipv4 == (compressed + 1)) {
			compressed_2end = 1;
		}
		ipv4++;

		if (!_php_filter_validate_ipv4(ipv4, ip4elm TSRMLS_CC)) {
			return 0;
		}

		if (compressed_2end) {
			return 1;
		}
	}

	if (!compressed) {
		char *end;
		if (ipv4) {
			end = ipv4 - 1;
		} else {
			end = str + strlen(start);
		}

		while (*str && str <= end) {
			IPV6_LOOP_IN(str);
			str++;
		}

		if (!ipv4) {
			if (col_fnd != 7) {
				return 0;
			} else {
				return 1;
			}
		} else {
			if (col_fnd != 6) {
				return -1230;
			} else {
				return 1;
			}
		}
	} else {
		if (!ipv4) {
			t = compressed - 1;
			while (t >= start) {
				IPV6_LOOP_IN(t);
				t--;
			}

			if (hexcode_found > 4) {
				return 0;
			}

			t = compressed + 2;
			hexcode_found = 0;
			while (*t) {
				IPV6_LOOP_IN(t);
				t++;
			}

			if (hexcode_found > 4) {
				return 0;
			}

			if (col_fnd > 6) {
				return 0;
			} else {
				return 1;
			}
		} else {
			/* ipv4 part always at the end */
			t = ipv4 - 1;
			while (t >= (compressed + 2)) {
				IPV6_LOOP_IN(t);
				t--;
			}

			if (hexcode_found > 4) {
				return 0;
			}

			hexcode_found = 0;
			t = compressed - 1;
			while (t >= start) {
				IPV6_LOOP_IN(t);
				t--;
			}
			if (hexcode_found > 4) {
				return 0;
			}

			if (col_fnd > 6) {
				return 0;
			} else {
				return 1;
			}
		}
	}
	return 0;
}
/* }}} */

void php_filter_validate_ip(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	/* validates an ipv4 or ipv6 IP, based on the flag (4, 6, or both) add a
	 * flag to throw out reserved ranges; multicast ranges... etc. If both
	 * allow_ipv4 and allow_ipv6 flags flag are used, then the first dot or
	 * colon determine the format */

	char          *str = NULL;
	int            ip[4];
	int            mode;

	str = Z_STRVAL_P(value);

	if (strchr(str, ':')) {
		mode = FORMAT_IPV6;
	} else if (strchr(str, '.')) {
		mode = FORMAT_IPV4;
	} else {
		zval_dtor(value);
		ZVAL_BOOL(value, 0);
		return;
	}

	if (flags & (FILTER_FLAG_IPV4 || FILTER_FLAG_IPV6)) {
		/* Both formats are cool */
	} else if ((flags & FILTER_FLAG_IPV4) && mode == FORMAT_IPV6) {
		zval_dtor(value);
		ZVAL_BOOL(value, 0);
		return;
	} else if ((flags & FILTER_FLAG_IPV6) && mode == FORMAT_IPV4) {
		zval_dtor(value);
		ZVAL_BOOL(value, 0);
		return;
	}

	switch (mode) {
		case FORMAT_IPV4:
			if (!_php_filter_validate_ipv4(str, ip TSRMLS_CC)) {
				zval_dtor(value);
				ZVAL_BOOL(value, 0);
				return;
			}

			/* Check flags */
			if (flags & FILTER_FLAG_NO_PRIV_RANGE) {
				if (
					(ip[0] == 10) ||
					(ip[0] == 172 && (ip[1] >= 16 && ip[1] <= 31)) ||
					(ip[0] == 192 && ip[1] == 168)
				) {
					zval_dtor(value);
					ZVAL_BOOL(value, 0);
					return;
				}
			}

			if (flags & FILTER_FLAG_NO_RES_RANGE) {
				if (
					(ip[0] == 0) ||
					(ip[0] == 169 && ip[1] == 254) ||
					(ip[0] == 192 && ip[1] == 0 && ip[2] == 2) ||
					(ip[0] >= 224 && ip[0] <= 255)
				) {
					zval_dtor(value);
					ZVAL_BOOL(value, 0);
					return;
				}
			}
			break;

		case FORMAT_IPV6:
			{
				int res = 0;
				res = _php_filter_validate_ipv6_(str TSRMLS_CC);
				if (res < 1) {
					zval_dtor(value);
					ZVAL_BOOL(value, 0);
					return;
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
