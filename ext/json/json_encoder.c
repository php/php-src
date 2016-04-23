/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Omar Kilani <omar@php.net>                                   |
  |         Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/html.h"
#include "zend_smart_str.h"
#include "php_json.h"
#include <zend_exceptions.h>

/* double limits */
#include <float.h>
#if defined(DBL_MANT_DIG) && defined(DBL_MIN_EXP)
#define PHP_JSON_DOUBLE_MAX_LENGTH (3 + DBL_MANT_DIG - DBL_MIN_EXP)
#else
#define PHP_JSON_DOUBLE_MAX_LENGTH 1080
#endif

static const char digits[] = "0123456789abcdef";

static void php_json_escape_string(smart_str *buf, char *s, size_t len, int options);

static int php_json_determine_array_type(zval *val) /* {{{ */
{
	int i;
	HashTable *myht = Z_ARRVAL_P(val);

	i = myht ? zend_hash_num_elements(myht) : 0;
	if (i > 0) {
		zend_string *key;
		zend_ulong index, idx;

		idx = 0;
		ZEND_HASH_FOREACH_KEY(myht, index, key) {
			if (key) {
				return PHP_JSON_OUTPUT_OBJECT;
			} else {
				if (index != idx) {
					return PHP_JSON_OUTPUT_OBJECT;
				}
			}
			idx++;
		} ZEND_HASH_FOREACH_END();
	}

	return PHP_JSON_OUTPUT_ARRAY;
}
/* }}} */

/* {{{ Pretty printing support functions */

static inline void php_json_pretty_print_char(smart_str *buf, int options, char c) /* {{{ */
{
	if (options & PHP_JSON_PRETTY_PRINT) {
		smart_str_appendc(buf, c);
	}
}
/* }}} */

static inline void php_json_pretty_print_indent(smart_str *buf, int options) /* {{{ */
{
	int i;

	if (options & PHP_JSON_PRETTY_PRINT) {
		for (i = 0; i < JSON_G(encoder_depth); ++i) {
			smart_str_appendl(buf, "    ", 4);
		}
	}
}
/* }}} */

/* }}} */

static inline int php_json_is_valid_double(double d) /* {{{ */
{
	return !zend_isinf(d) && !zend_isnan(d);
}
/* }}} */

static inline void php_json_encode_double(smart_str *buf, double d, int options) /* {{{ */
{
	size_t len;
	char num[PHP_JSON_DOUBLE_MAX_LENGTH];
	php_gcvt(d, (int)EG(precision), '.', 'e', &num[0]);
	len = strlen(num);
	if (options & PHP_JSON_PRESERVE_ZERO_FRACTION && strchr(num, '.') == NULL && len < PHP_JSON_DOUBLE_MAX_LENGTH - 2) {
		num[len++] = '.';
		num[len++] = '0';
		num[len] = '\0';
	}
	smart_str_appendl(buf, num, len);
}
/* }}} */

static void php_json_encode_array(smart_str *buf, zval *val, int options) /* {{{ */
{
	int i, r, need_comma = 0;
	HashTable *myht;

	if (Z_TYPE_P(val) == IS_ARRAY) {
		myht = Z_ARRVAL_P(val);
		r = (options & PHP_JSON_FORCE_OBJECT) ? PHP_JSON_OUTPUT_OBJECT : php_json_determine_array_type(val);
	} else {
		myht = Z_OBJPROP_P(val);
		r = PHP_JSON_OUTPUT_OBJECT;
	}

	if (myht && ZEND_HASH_GET_APPLY_COUNT(myht) > 1) {
		JSON_G(error_code) = PHP_JSON_ERROR_RECURSION;
		smart_str_appendl(buf, "null", 4);
		return;
	}

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		smart_str_appendc(buf, '[');
	} else {
		smart_str_appendc(buf, '{');
	}

	++JSON_G(encoder_depth);

	i = myht ? zend_hash_num_elements(myht) : 0;

	if (i > 0) {
		zend_string *key;
		zval *data;
		zend_ulong index;
		HashTable *tmp_ht;

		ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, data) {
			ZVAL_DEREF(data);
			tmp_ht = HASH_OF(data);
			if (tmp_ht && ZEND_HASH_APPLY_PROTECTION(tmp_ht)) {
				ZEND_HASH_INC_APPLY_COUNT(tmp_ht);
			}

			if (r == PHP_JSON_OUTPUT_ARRAY) {
				if (need_comma) {
					smart_str_appendc(buf, ',');
				} else {
					need_comma = 1;
				}

				php_json_pretty_print_char(buf, options, '\n');
				php_json_pretty_print_indent(buf, options);
				php_json_encode(buf, data, options);
			} else if (r == PHP_JSON_OUTPUT_OBJECT) {
				if (key) {
					if (ZSTR_VAL(key)[0] == '\0' && Z_TYPE_P(val) == IS_OBJECT) {
						/* Skip protected and private members. */
						if (tmp_ht && ZEND_HASH_APPLY_PROTECTION(tmp_ht)) {
							ZEND_HASH_DEC_APPLY_COUNT(tmp_ht);
						}
						continue;
					}

					if (need_comma) {
						smart_str_appendc(buf, ',');
					} else {
						need_comma = 1;
					}

					php_json_pretty_print_char(buf, options, '\n');
					php_json_pretty_print_indent(buf, options);

					php_json_escape_string(buf, ZSTR_VAL(key), ZSTR_LEN(key), options & ~PHP_JSON_NUMERIC_CHECK);
					smart_str_appendc(buf, ':');

					php_json_pretty_print_char(buf, options, ' ');

					php_json_encode(buf, data, options);
				} else {
					if (need_comma) {
						smart_str_appendc(buf, ',');
					} else {
						need_comma = 1;
					}

					php_json_pretty_print_char(buf, options, '\n');
					php_json_pretty_print_indent(buf, options);

					smart_str_appendc(buf, '"');
					smart_str_append_long(buf, (zend_long) index);
					smart_str_appendc(buf, '"');
					smart_str_appendc(buf, ':');

					php_json_pretty_print_char(buf, options, ' ');

					php_json_encode(buf, data, options);
				}
			}

			if (tmp_ht && ZEND_HASH_APPLY_PROTECTION(tmp_ht)) {
				ZEND_HASH_DEC_APPLY_COUNT(tmp_ht);
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (JSON_G(encoder_depth) > JSON_G(encode_max_depth)) {
		JSON_G(error_code) = PHP_JSON_ERROR_DEPTH;
	}
	--JSON_G(encoder_depth);

	/* Only keep closing bracket on same line for empty arrays/objects */
	if (need_comma) {
		php_json_pretty_print_char(buf, options, '\n');
		php_json_pretty_print_indent(buf, options);
	}

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		smart_str_appendc(buf, ']');
	} else {
		smart_str_appendc(buf, '}');
	}
}
/* }}} */

static int php_json_utf8_to_utf16(unsigned short *utf16, char utf8[], size_t len) /* {{{ */
{
	size_t pos = 0, us;
	int j, status;

	if (utf16) {
		/* really convert the utf8 string */
		for (j=0 ; pos < len ; j++) {
			us = php_next_utf8_char((const unsigned char *)utf8, len, &pos, &status);
			if (status != SUCCESS) {
				return -1;
			}
			/* From http://en.wikipedia.org/wiki/UTF16 */
			if (us >= 0x10000) {
				us -= 0x10000;
				utf16[j++] = (unsigned short)((us >> 10) | 0xd800);
				utf16[j] = (unsigned short)((us & 0x3ff) | 0xdc00);
			} else {
				utf16[j] = (unsigned short)us;
			}
		}
	} else {
		/* Only check if utf8 string is valid, and compute utf16 length */
		for (j=0 ; pos < len ; j++) {
			us = php_next_utf8_char((const unsigned char *)utf8, len, &pos, &status);
			if (status != SUCCESS) {
				return -1;
			}
			if (us >= 0x10000) {
				j++;
			}
		}
	}
	return j;
}
/* }}} */

static void php_json_escape_string(smart_str *buf, char *s, size_t len, int options) /* {{{ */
{
	int status;
	unsigned int us;
	size_t pos, checkpoint;

	if (len == 0) {
		smart_str_appendl(buf, "\"\"", 2);
		return;
	}

	if (options & PHP_JSON_NUMERIC_CHECK) {
		double d;
		int type;
		zend_long p;

		if ((type = is_numeric_string(s, len, &p, &d, 0)) != 0) {
			if (type == IS_LONG) {
				smart_str_append_long(buf, p);
				return;
			} else if (type == IS_DOUBLE && php_json_is_valid_double(d)) {
				php_json_encode_double(buf, d, options);
				return;
			}
		}

	}

	if (options & PHP_JSON_UNESCAPED_UNICODE) {
		/* validate UTF-8 string first */
		if (php_json_utf8_to_utf16(NULL, s, len) < 0) {
			JSON_G(error_code) = PHP_JSON_ERROR_UTF8;
			smart_str_appendl(buf, "null", 4);
			return;
		}
	}

	pos = 0;
	checkpoint = buf->s ? ZSTR_LEN(buf->s) : 0;

	/* pre-allocate for string length plus 2 quotes */
	smart_str_alloc(buf, len+2, 0);
	smart_str_appendc(buf, '"');

	do {
		us = (unsigned char)s[pos];
		if (us >= 0x80 && !(options & PHP_JSON_UNESCAPED_UNICODE)) {
			/* UTF-8 character */
			us = php_next_utf8_char((const unsigned char *)s, len, &pos, &status);
			if (status != SUCCESS) {
				if (buf->s) {
					ZSTR_LEN(buf->s) = checkpoint;
				}
				JSON_G(error_code) = PHP_JSON_ERROR_UTF8;
				smart_str_appendl(buf, "null", 4);
				return;
			}
			/* From http://en.wikipedia.org/wiki/UTF16 */
			if (us >= 0x10000) {
				unsigned int next_us;
				us -= 0x10000;
				next_us = (unsigned short)((us & 0x3ff) | 0xdc00);
				us = (unsigned short)((us >> 10) | 0xd800);
				smart_str_appendl(buf, "\\u", 2);
				smart_str_appendc(buf, digits[(us & 0xf000) >> 12]);
				smart_str_appendc(buf, digits[(us & 0xf00)  >> 8]);
				smart_str_appendc(buf, digits[(us & 0xf0)   >> 4]);
				smart_str_appendc(buf, digits[(us & 0xf)]);
				us = next_us;
			}
			smart_str_appendl(buf, "\\u", 2);
			smart_str_appendc(buf, digits[(us & 0xf000) >> 12]);
			smart_str_appendc(buf, digits[(us & 0xf00)  >> 8]);
			smart_str_appendc(buf, digits[(us & 0xf0)   >> 4]);
			smart_str_appendc(buf, digits[(us & 0xf)]);
		} else {
			pos++;

			switch (us) {
				case '"':
					if (options & PHP_JSON_HEX_QUOT) {
						smart_str_appendl(buf, "\\u0022", 6);
					} else {
						smart_str_appendl(buf, "\\\"", 2);
					}
					break;

				case '\\':
					smart_str_appendl(buf, "\\\\", 2);
					break;

				case '/':
					if (options & PHP_JSON_UNESCAPED_SLASHES) {
						smart_str_appendc(buf, '/');
					} else {
						smart_str_appendl(buf, "\\/", 2);
					}
					break;

				case '\b':
					smart_str_appendl(buf, "\\b", 2);
					break;

				case '\f':
					smart_str_appendl(buf, "\\f", 2);
					break;

				case '\n':
					smart_str_appendl(buf, "\\n", 2);
					break;

				case '\r':
					smart_str_appendl(buf, "\\r", 2);
					break;

				case '\t':
					smart_str_appendl(buf, "\\t", 2);
					break;

				case '<':
					if (options & PHP_JSON_HEX_TAG) {
						smart_str_appendl(buf, "\\u003C", 6);
					} else {
						smart_str_appendc(buf, '<');
					}
					break;

				case '>':
					if (options & PHP_JSON_HEX_TAG) {
						smart_str_appendl(buf, "\\u003E", 6);
					} else {
						smart_str_appendc(buf, '>');
					}
					break;

				case '&':
					if (options & PHP_JSON_HEX_AMP) {
						smart_str_appendl(buf, "\\u0026", 6);
					} else {
						smart_str_appendc(buf, '&');
					}
					break;

				case '\'':
					if (options & PHP_JSON_HEX_APOS) {
						smart_str_appendl(buf, "\\u0027", 6);
					} else {
						smart_str_appendc(buf, '\'');
					}
					break;

				default:
					if (us >= ' ') {
						smart_str_appendc(buf, (unsigned char) us);
					} else {
						smart_str_appendl(buf, "\\u00", sizeof("\\u00")-1);
						smart_str_appendc(buf, digits[(us & 0xf0)   >> 4]);
						smart_str_appendc(buf, digits[(us & 0xf)]);
					}
					break;
			}
		}
	} while (pos < len);

	smart_str_appendc(buf, '"');
}
/* }}} */

static void php_json_encode_serializable_object(smart_str *buf, zval *val, int options) /* {{{ */
{
	zend_class_entry *ce = Z_OBJCE_P(val);
	zval retval, fname;
	HashTable* myht;
	int origin_error_code;

	if (Z_TYPE_P(val) == IS_ARRAY) {
		myht = Z_ARRVAL_P(val);
	} else {
		myht = Z_OBJPROP_P(val);
	}

	if (myht && ZEND_HASH_GET_APPLY_COUNT(myht) > 1) {
		JSON_G(error_code) = PHP_JSON_ERROR_RECURSION;
		smart_str_appendl(buf, "null", 4);
		return;
	}


	ZVAL_STRING(&fname, "jsonSerialize");

	origin_error_code = JSON_G(error_code);
	if (FAILURE == call_user_function_ex(EG(function_table), val, &fname, &retval, 0, NULL, 1, NULL) || Z_TYPE(retval) == IS_UNDEF) {
		zend_throw_exception_ex(NULL, 0, "Failed calling %s::jsonSerialize()", ZSTR_VAL(ce->name));
		smart_str_appendl(buf, "null", sizeof("null") - 1);
		zval_ptr_dtor(&fname);
		return;
	}

	JSON_G(error_code) = origin_error_code;
	if (EG(exception)) {
		/* Error already raised */
		zval_ptr_dtor(&retval);
		zval_ptr_dtor(&fname);
		smart_str_appendl(buf, "null", sizeof("null") - 1);
		return;
	}

	if ((Z_TYPE(retval) == IS_OBJECT) &&
		(Z_OBJ(retval) == Z_OBJ_P(val))) {
		/* Handle the case where jsonSerialize does: return $this; by going straight to encode array */
		php_json_encode_array(buf, &retval, options);
	} else {
		/* All other types, encode as normal */
		php_json_encode(buf, &retval, options);
	}

	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&fname);
}
/* }}} */

void php_json_encode_zval(smart_str *buf, zval *val, int options) /* {{{ */
{
again:
	switch (Z_TYPE_P(val))
	{
		case IS_NULL:
			smart_str_appendl(buf, "null", 4);
			break;

		case IS_TRUE:
			smart_str_appendl(buf, "true", 4);
			break;
		case IS_FALSE:
			smart_str_appendl(buf, "false", 5);
			break;

		case IS_LONG:
			smart_str_append_long(buf, Z_LVAL_P(val));
			break;

		case IS_DOUBLE:
			if (php_json_is_valid_double(Z_DVAL_P(val))) {
				php_json_encode_double(buf, Z_DVAL_P(val), options);
			} else {
				JSON_G(error_code) = PHP_JSON_ERROR_INF_OR_NAN;
				smart_str_appendc(buf, '0');
			}
			break;

		case IS_STRING:
			php_json_escape_string(buf, Z_STRVAL_P(val), Z_STRLEN_P(val), options);
			break;

		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(val), php_json_serializable_ce)) {
				php_json_encode_serializable_object(buf, val, options);
				break;
			}
			/* fallthrough -- Non-serializable object */
		case IS_ARRAY:
			php_json_encode_array(buf, val, options);
			break;

		case IS_REFERENCE:
			val = Z_REFVAL_P(val);
			goto again;

		default:
			JSON_G(error_code) = PHP_JSON_ERROR_UNSUPPORTED_TYPE;
			smart_str_appendl(buf, "null", 4);
			break;
	}

	return;
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
