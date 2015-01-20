/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
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
#include "JSON_parser.h"
#include "php_json.h"
#include <zend_exceptions.h>

#include <float.h>
#if defined(DBL_MANT_DIG) && defined(DBL_MIN_EXP)
#define NUM_BUF_SIZE (3 + DBL_MANT_DIG - DBL_MIN_EXP)
#else
#define NUM_BUF_SIZE 1080
#endif


static PHP_MINFO_FUNCTION(json);
static PHP_FUNCTION(json_encode);
static PHP_FUNCTION(json_decode);
static PHP_FUNCTION(json_last_error);
static PHP_FUNCTION(json_last_error_msg);

static const char digits[] = "0123456789abcdef";

PHP_JSON_API zend_class_entry *php_json_serializable_ce;

ZEND_DECLARE_MODULE_GLOBALS(json)

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_json_encode, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, depth)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_json_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, json)
	ZEND_ARG_INFO(0, assoc)
	ZEND_ARG_INFO(0, depth)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_json_last_error, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_json_last_error_msg, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ json_functions[] */
static const zend_function_entry json_functions[] = {
	PHP_FE(json_encode, arginfo_json_encode)
	PHP_FE(json_decode, arginfo_json_decode)
	PHP_FE(json_last_error, arginfo_json_last_error)
	PHP_FE(json_last_error_msg, arginfo_json_last_error_msg)
	PHP_FE_END
};
/* }}} */

/* {{{ JsonSerializable methods */
ZEND_BEGIN_ARG_INFO(json_serialize_arginfo, 0)
	/* No arguments */
ZEND_END_ARG_INFO();

static const zend_function_entry json_serializable_interface[] = {
	PHP_ABSTRACT_ME(JsonSerializable, jsonSerialize, json_serialize_arginfo)
	PHP_FE_END
};
/* }}} */

/* {{{ MINIT */
static PHP_MINIT_FUNCTION(json)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "JsonSerializable", json_serializable_interface);
	php_json_serializable_ce = zend_register_internal_interface(&ce);

	REGISTER_LONG_CONSTANT("JSON_HEX_TAG",  PHP_JSON_HEX_TAG,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_HEX_AMP",  PHP_JSON_HEX_AMP,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_HEX_APOS", PHP_JSON_HEX_APOS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_HEX_QUOT", PHP_JSON_HEX_QUOT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_FORCE_OBJECT", PHP_JSON_FORCE_OBJECT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_NUMERIC_CHECK", PHP_JSON_NUMERIC_CHECK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_UNESCAPED_SLASHES", PHP_JSON_UNESCAPED_SLASHES, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_PRETTY_PRINT", PHP_JSON_PRETTY_PRINT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_UNESCAPED_UNICODE", PHP_JSON_UNESCAPED_UNICODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_PARTIAL_OUTPUT_ON_ERROR", PHP_JSON_PARTIAL_OUTPUT_ON_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_PRESERVE_ZERO_FRACTION", PHP_JSON_PRESERVE_ZERO_FRACTION, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("JSON_ERROR_NONE", PHP_JSON_ERROR_NONE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_DEPTH", PHP_JSON_ERROR_DEPTH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_STATE_MISMATCH", PHP_JSON_ERROR_STATE_MISMATCH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_CTRL_CHAR", PHP_JSON_ERROR_CTRL_CHAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_SYNTAX", PHP_JSON_ERROR_SYNTAX, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_UTF8", PHP_JSON_ERROR_UTF8, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_RECURSION", PHP_JSON_ERROR_RECURSION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_INF_OR_NAN", PHP_JSON_ERROR_INF_OR_NAN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_ERROR_UNSUPPORTED_TYPE", PHP_JSON_ERROR_UNSUPPORTED_TYPE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("JSON_OBJECT_AS_ARRAY",		PHP_JSON_OBJECT_AS_ARRAY,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("JSON_BIGINT_AS_STRING",		PHP_JSON_BIGINT_AS_STRING,		CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION
*/
static PHP_GINIT_FUNCTION(json)
{
#if defined(COMPILE_DL_JSON) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE;
#endif
	json_globals->encoder_depth = 0;
	json_globals->error_code = 0;
	json_globals->encode_max_depth = 0;
}
/* }}} */


/* {{{ json_module_entry
 */
zend_module_entry json_module_entry = {
	STANDARD_MODULE_HEADER,
	"json",
	json_functions,
	PHP_MINIT(json),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(json),
	PHP_JSON_VERSION,
	PHP_MODULE_GLOBALS(json),
	PHP_GINIT(json),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_JSON
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE;
#endif
ZEND_GET_MODULE(json)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(json)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "json support", "enabled");
	php_info_print_table_row(2, "json version", PHP_JSON_VERSION);
	php_info_print_table_end();
}
/* }}} */

static void json_escape_string(smart_str *buf, char *s, size_t len, int options);

static int json_determine_array_type(zval *val) /* {{{ */
{
	int i;
	HashTable *myht = HASH_OF(val);

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

static inline void json_pretty_print_char(smart_str *buf, int options, char c) /* {{{ */
{
	if (options & PHP_JSON_PRETTY_PRINT) {
		smart_str_appendc(buf, c);
	}
}
/* }}} */

static inline void json_pretty_print_indent(smart_str *buf, int options) /* {{{ */
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

static void json_encode_array(smart_str *buf, zval *val, int options) /* {{{ */
{
	int i, r, need_comma = 0;
	HashTable *myht;

	if (Z_TYPE_P(val) == IS_ARRAY) {
		myht = HASH_OF(val);
		r = (options & PHP_JSON_FORCE_OBJECT) ? PHP_JSON_OUTPUT_OBJECT : json_determine_array_type(val);
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

				json_pretty_print_char(buf, options, '\n');
				json_pretty_print_indent(buf, options);
				php_json_encode(buf, data, options);
			} else if (r == PHP_JSON_OUTPUT_OBJECT) {
				if (key) {
					if (key->val[0] == '\0' && Z_TYPE_P(val) == IS_OBJECT) {
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

					json_pretty_print_char(buf, options, '\n');
					json_pretty_print_indent(buf, options);

					json_escape_string(buf, key->val, key->len, options & ~PHP_JSON_NUMERIC_CHECK);
					smart_str_appendc(buf, ':');

					json_pretty_print_char(buf, options, ' ');

					php_json_encode(buf, data, options);
				} else {
					if (need_comma) {
						smart_str_appendc(buf, ',');
					} else {
						need_comma = 1;
					}

					json_pretty_print_char(buf, options, '\n');
					json_pretty_print_indent(buf, options);

					smart_str_appendc(buf, '"');
					smart_str_append_long(buf, (zend_long) index);
					smart_str_appendc(buf, '"');
					smart_str_appendc(buf, ':');

					json_pretty_print_char(buf, options, ' ');

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
		json_pretty_print_char(buf, options, '\n');
		json_pretty_print_indent(buf, options);
	}

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		smart_str_appendc(buf, ']');
	} else {
		smart_str_appendc(buf, '}');
	}
}
/* }}} */

static int json_utf8_to_utf16(unsigned short *utf16, char utf8[], int len) /* {{{ */
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

static void json_escape_string(smart_str *buf, char *s, size_t len, int options) /* {{{ */
{
	int status;
	unsigned int us, next_us = 0;
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
			} else if (type == IS_DOUBLE) {
				if (!zend_isinf(d) && !zend_isnan(d)) {
					char num[NUM_BUF_SIZE];
					int l;

					php_gcvt(d, EG(precision), '.', 'e', (char *)num);
					l = strlen(num);
					if (options & PHP_JSON_PRESERVE_ZERO_FRACTION && strchr(num, '.') == NULL && l < NUM_BUF_SIZE - 2) {
						num[l++] = '.';
						num[l++] = '0';
						num[l] = '\0';
					}
					smart_str_appendl(buf, num, l);
				} else {
					JSON_G(error_code) = PHP_JSON_ERROR_INF_OR_NAN;
					smart_str_appendc(buf, '0');
				}
			}
			return;
		}

	}

	if (options & PHP_JSON_UNESCAPED_UNICODE) {
		/* validate UTF-8 string first */
		if (json_utf8_to_utf16(NULL, s, len) < 0) {
			JSON_G(error_code) = PHP_JSON_ERROR_UTF8;
			smart_str_appendl(buf, "null", 4);
			return;
		}
	}

	pos = 0;
	checkpoint = buf->s ? buf->s->len : 0;

	/* pre-allocate for string length plus 2 quotes */
	smart_str_alloc(buf, len+2, 0);
	smart_str_appendc(buf, '"');

	do {
		if (UNEXPECTED(next_us)) {
			us = next_us;
			next_us = 0;
		} else {
			us = (unsigned char)s[pos];
			if (!(options & PHP_JSON_UNESCAPED_UNICODE) && us >= 0x80) {
				/* UTF-8 character */
				us = php_next_utf8_char((const unsigned char *)s, len, &pos, &status);
				if (status != SUCCESS) {
					if (buf->s) {
						buf->s->len = checkpoint;
					}
					JSON_G(error_code) = PHP_JSON_ERROR_UTF8;
					smart_str_appendl(buf, "null", 4);
					return;
				}
				/* From http://en.wikipedia.org/wiki/UTF16 */
				if (us >= 0x10000) {
					us -= 0x10000;
					next_us = (unsigned short)((us & 0x3ff) | 0xdc00);
					us = (unsigned short)((us >> 10) | 0xd800);
				}
			} else {
				pos++;
			}
		}

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
				if (us >= ' ' && ((options & PHP_JSON_UNESCAPED_UNICODE) || (us & 127) == us)) {
					smart_str_appendc(buf, (unsigned char) us);
				} else {
					smart_str_appendl(buf, "\\u", 2);
					smart_str_appendc(buf, digits[(us & 0xf000) >> 12]);
					smart_str_appendc(buf, digits[(us & 0xf00)  >> 8]);
					smart_str_appendc(buf, digits[(us & 0xf0)   >> 4]);
					smart_str_appendc(buf, digits[(us & 0xf)]);
				}
				break;
		}
	} while (pos < len || next_us);

	smart_str_appendc(buf, '"');
}
/* }}} */

static void json_encode_serializable_object(smart_str *buf, zval *val, int options) /* {{{ */
{
	zend_class_entry *ce = Z_OBJCE_P(val);
	zval retval, fname;
	HashTable* myht;

	if (Z_TYPE_P(val) == IS_ARRAY) {
		myht = HASH_OF(val);
	} else {
		myht = Z_OBJPROP_P(val);
	}

	if (myht && ZEND_HASH_GET_APPLY_COUNT(myht) > 1) {
		JSON_G(error_code) = PHP_JSON_ERROR_RECURSION;
		smart_str_appendl(buf, "null", 4);
		return;
	}

	ZVAL_STRING(&fname, "jsonSerialize");

	if (FAILURE == call_user_function_ex(EG(function_table), val, &fname, &retval, 0, NULL, 1, NULL) || Z_TYPE(retval) == IS_UNDEF) {
		zend_throw_exception_ex(NULL, 0, "Failed calling %s::jsonSerialize()", ce->name->val);
		smart_str_appendl(buf, "null", sizeof("null") - 1);
		zval_ptr_dtor(&fname);
		return;
    }

	if (EG(exception)) {
		/* Error already raised */
		zval_ptr_dtor(&retval);
		zval_ptr_dtor(&fname);
		smart_str_appendl(buf, "null", sizeof("null") - 1);
		return;
	}

	if ((Z_TYPE(retval) == IS_OBJECT) &&
		(Z_OBJ_HANDLE(retval) == Z_OBJ_HANDLE_P(val))) {
		/* Handle the case where jsonSerialize does: return $this; by going straight to encode array */
		json_encode_array(buf, &retval, options);
	} else {
		/* All other types, encode as normal */
		php_json_encode(buf, &retval, options);
	}

	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&fname);
}
/* }}} */

PHP_JSON_API void php_json_encode(smart_str *buf, zval *val, int options) /* {{{ */
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
			{
				char num[NUM_BUF_SIZE];
				int len;
				double dbl = Z_DVAL_P(val);

				if (!zend_isinf(dbl) && !zend_isnan(dbl)) {
					php_gcvt(dbl, EG(precision), '.', 'e', (char *)num);
					len = strlen(num);
					if (options & PHP_JSON_PRESERVE_ZERO_FRACTION && strchr(num, '.') == NULL && len < NUM_BUF_SIZE - 2) {
						num[len++] = '.';
						num[len++] = '0';
						num[len] = '\0';
					}
					smart_str_appendl(buf, num, len);
				} else {
					JSON_G(error_code) = PHP_JSON_ERROR_INF_OR_NAN;
					smart_str_appendc(buf, '0');
				}
			}
			break;

		case IS_STRING:
			json_escape_string(buf, Z_STRVAL_P(val), Z_STRLEN_P(val), options);
			break;

		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(val), php_json_serializable_ce)) {
				json_encode_serializable_object(buf, val, options);
				break;
			}
			/* fallthrough -- Non-serializable object */
		case IS_ARRAY:
			json_encode_array(buf, val, options);
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

PHP_JSON_API void php_json_decode_ex(zval *return_value, char *str, size_t str_len, zend_long options, zend_long depth) /* {{{ */
{
	size_t utf16_len;
	unsigned short *utf16;
	JSON_parser jp;

	utf16 = (unsigned short *) safe_emalloc((str_len+1), sizeof(unsigned short), 1);

	utf16_len = json_utf8_to_utf16(utf16, str, str_len);
	if (utf16_len <= 0) {
		if (utf16) {
			efree(utf16);
		}
		JSON_G(error_code) = PHP_JSON_ERROR_UTF8;
		RETURN_NULL();
	}

	if (depth <= 0) {
		php_error_docref(NULL, E_WARNING, "Depth must be greater than zero");
		efree(utf16);
		RETURN_NULL();
	}

	jp = new_JSON_parser(depth);
	if (!parse_JSON_ex(jp, return_value, utf16, utf16_len, options)) {
		double d;
		int type, overflow_info;
		zend_long p;
		char *trim = str;
		int trim_len = str_len;

		zval_dtor(return_value);

		/* Increment trimmed string pointer to strip leading whitespace */
		/* JSON RFC says to consider as whitespace: space, tab, LF or CR */
		while (trim_len && (*trim == ' ' || *trim == '\t' || *trim == '\n' || *trim == '\r')) {
			trim++;
			trim_len--;
		}

		/* Decrement trimmed string length to strip trailing whitespace */
		while (trim_len && (trim[trim_len - 1] == ' ' || trim[trim_len - 1] == '\t' || trim[trim_len - 1] == '\n' || trim[trim_len - 1] == '\r')) {
			trim_len--;
		}

		RETVAL_NULL();
		if (trim_len == 4) {
			if (!strncmp(trim, "null", trim_len)) {
				/* We need to explicitly clear the error because its an actual NULL and not an error */
				jp->error_code = PHP_JSON_ERROR_NONE;
				RETVAL_NULL();
			} else if (!strncmp(trim, "true", trim_len)) {
				RETVAL_BOOL(1);
			}
		} else if (trim_len == 5 && !strncmp(trim, "false", trim_len)) {
			RETVAL_BOOL(0);
		}

		if ((type = is_numeric_string_ex(trim, trim_len, &p, &d, 0, &overflow_info)) != 0) {
			if (type == IS_LONG) {
				RETVAL_LONG(p);
			} else if (type == IS_DOUBLE) {
				if (options & PHP_JSON_BIGINT_AS_STRING && overflow_info) {
					/* Within an object or array, a numeric literal is assumed
					 * to be an integer if and only if it's entirely made up of
					 * digits (exponent notation will result in the number
					 * being treated as a double). We'll match that behaviour
					 * here. */
					int i;
					zend_bool is_float = 0;

					for (i = (trim[0] == '-' ? 1 : 0); i < trim_len; i++) {
						/* Not using isdigit() because it's locale specific,
						 * but we expect JSON input to always be UTF-8. */
						if (trim[i] < '0' || trim[i] > '9') {
							is_float = 1;
							break;
						}
					}

					if (is_float) {
						RETVAL_DOUBLE(d);
					} else {
						RETVAL_STRINGL(trim, trim_len);
					}
				} else {
					RETVAL_DOUBLE(d);
				}
			}
		}

		if (Z_TYPE_P(return_value) != IS_NULL) {
			jp->error_code = PHP_JSON_ERROR_NONE;
		}
	}
	efree(utf16);
	JSON_G(error_code) = jp->error_code;
	free_JSON_parser(jp);
}
/* }}} */

/* {{{ proto string json_encode(mixed data [, int options[, int depth]])
   Returns the JSON representation of a value */
static PHP_FUNCTION(json_encode)
{
	zval *parameter;
	smart_str buf = {0};
	zend_long options = 0;
    zend_long depth = JSON_PARSER_DEFAULT_DEPTH;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|ll", &parameter, &options, &depth) == FAILURE) {
		return;
	}

	JSON_G(error_code) = PHP_JSON_ERROR_NONE;

	JSON_G(encode_max_depth) = depth;

	php_json_encode(&buf, parameter, options);

	if (JSON_G(error_code) != PHP_JSON_ERROR_NONE && !(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
		smart_str_free(&buf);
		ZVAL_FALSE(return_value);
	} else {
		smart_str_0(&buf); /* copy? */
		ZVAL_NEW_STR(return_value, buf.s);
	}
}
/* }}} */

/* {{{ proto mixed json_decode(string json [, bool assoc [, long depth]])
   Decodes the JSON representation into a PHP value */
static PHP_FUNCTION(json_decode)
{
	char *str;
	size_t str_len;
	zend_bool assoc = 0; /* return JS objects as PHP objects by default */
	zend_long depth = JSON_PARSER_DEFAULT_DEPTH;
	zend_long options = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|bll", &str, &str_len, &assoc, &depth, &options) == FAILURE) {
		return;
	}

	JSON_G(error_code) = 0;

	if (!str_len) {
		RETURN_NULL();
	}

	/* For BC reasons, the bool $assoc overrides the long $options bit for PHP_JSON_OBJECT_AS_ARRAY */
	if (assoc) {
		options |=  PHP_JSON_OBJECT_AS_ARRAY;
	} else {
		options &= ~PHP_JSON_OBJECT_AS_ARRAY;
	}

	php_json_decode_ex(return_value, str, str_len, options, depth);
}
/* }}} */

/* {{{ proto int json_last_error()
   Returns the error code of the last json_encode() or json_decode() call. */
static PHP_FUNCTION(json_last_error)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(JSON_G(error_code));
}
/* }}} */

/* {{{ proto string json_last_error_msg()
   Returns the error string of the last json_encode() or json_decode() call. */
static PHP_FUNCTION(json_last_error_msg)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	switch(JSON_G(error_code)) {
		case PHP_JSON_ERROR_NONE:
			RETURN_STRING("No error");
		case PHP_JSON_ERROR_DEPTH:
			RETURN_STRING("Maximum stack depth exceeded");
		case PHP_JSON_ERROR_STATE_MISMATCH:
			RETURN_STRING("State mismatch (invalid or malformed JSON)");
		case PHP_JSON_ERROR_CTRL_CHAR:
			RETURN_STRING("Control character error, possibly incorrectly encoded");
		case PHP_JSON_ERROR_SYNTAX:
			RETURN_STRING("Syntax error");
		case PHP_JSON_ERROR_UTF8:
			RETURN_STRING("Malformed UTF-8 characters, possibly incorrectly encoded");
		case PHP_JSON_ERROR_RECURSION:
			RETURN_STRING("Recursion detected");
		case PHP_JSON_ERROR_INF_OR_NAN:
			RETURN_STRING("Inf and NaN cannot be JSON encoded");
		case PHP_JSON_ERROR_UNSUPPORTED_TYPE:
			RETURN_STRING("Type is not supported");
		default:
			RETURN_STRING("Unknown error");
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
