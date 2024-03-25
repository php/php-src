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
  | Author: Omar Kilani <omar@php.net>                                   |
  |         Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/html.h"
#include "zend_smart_str.h"
#include "php_json.h"
#include "php_json_encoder.h"
#include <zend_exceptions.h>
#include "zend_enum.h"

static const char digits[] = "0123456789abcdef";

static int php_json_determine_array_type(zval *val) /* {{{ */
{
	zend_array *myht = Z_ARRVAL_P(val);

	if (myht) {
		return zend_array_is_list(myht) ? PHP_JSON_OUTPUT_ARRAY : PHP_JSON_OUTPUT_OBJECT;
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

static inline void php_json_pretty_print_indent(smart_str *buf, int options, php_json_encoder *encoder) /* {{{ */
{
	int i;

	if (options & PHP_JSON_PRETTY_PRINT) {
		for (i = 0; i < encoder->depth; ++i) {
			smart_str_appendl(buf, "    ", 4);
		}
	}
}
/* }}} */

/* }}} */

static
#if defined(_MSC_VER) && defined(_M_ARM64)
// MSVC bug: https://developercommunity.visualstudio.com/t/corrupt-optimization-on-arm64-with-Ox-/10102551
zend_never_inline
#else
inline
#endif
bool php_json_is_valid_double(double d) /* {{{ */
{
	return !zend_isinf(d) && !zend_isnan(d);
}
/* }}} */

static inline void php_json_encode_double(smart_str *buf, double d, int options) /* {{{ */
{
	size_t len;
	char num[ZEND_DOUBLE_MAX_LENGTH];

	zend_gcvt(d, (int)PG(serialize_precision), '.', 'e', num);
	len = strlen(num);
	if (options & PHP_JSON_PRESERVE_ZERO_FRACTION && strchr(num, '.') == NULL && len < ZEND_DOUBLE_MAX_LENGTH - 2) {
		num[len++] = '.';
		num[len++] = '0';
		num[len] = '\0';
	}
	smart_str_appendl(buf, num, len);
}
/* }}} */

#define PHP_JSON_HASH_PROTECT_RECURSION(_tmp_ht) \
	do { \
		if (_tmp_ht) { \
			GC_TRY_PROTECT_RECURSION(_tmp_ht); \
		} \
	} while (0)

#define PHP_JSON_HASH_UNPROTECT_RECURSION(_tmp_ht) \
	do { \
		if (_tmp_ht) { \
			GC_TRY_UNPROTECT_RECURSION(_tmp_ht); \
		} \
	} while (0)

static zend_result php_json_encode_array(smart_str *buf, zval *val, int options, php_json_encoder *encoder) /* {{{ */
{
	int i, r, need_comma = 0;
	HashTable *myht, *prop_ht;

	if (Z_TYPE_P(val) == IS_ARRAY) {
		myht = Z_ARRVAL_P(val);
		prop_ht = NULL;
		r = (options & PHP_JSON_FORCE_OBJECT) ? PHP_JSON_OUTPUT_OBJECT : php_json_determine_array_type(val);
	} else if (Z_OBJ_P(val)->properties == NULL
	 && Z_OBJ_HT_P(val)->get_properties_for == NULL
	 && Z_OBJ_HT_P(val)->get_properties == zend_std_get_properties) {
		/* Optimized version without rebuilding properties HashTable */
		zend_object *obj = Z_OBJ_P(val);
		zend_class_entry *ce = obj->ce;
		zend_property_info *prop_info;
		zval *prop;
		int i;

		if (GC_IS_RECURSIVE(obj)) {
			encoder->error_code = PHP_JSON_ERROR_RECURSION;
			smart_str_appendl(buf, "null", 4);
			return FAILURE;
		}

		PHP_JSON_HASH_PROTECT_RECURSION(obj);

		smart_str_appendc(buf, '{');

		++encoder->depth;

		for (i = 0; i < ce->default_properties_count; i++) {
			prop_info = ce->properties_info_table[i];
			if (!prop_info) {
				continue;
			}
			if (ZSTR_VAL(prop_info->name)[0] == '\0' && ZSTR_LEN(prop_info->name) > 0) {
				/* Skip protected and private members. */
				continue;
			}
			prop = OBJ_PROP(obj, prop_info->offset);
			if (Z_TYPE_P(prop) == IS_UNDEF) {
				continue;
			}

			if (need_comma) {
				smart_str_appendc(buf, ',');
			} else {
				need_comma = 1;
			}

			php_json_pretty_print_char(buf, options, '\n');
			php_json_pretty_print_indent(buf, options, encoder);

			if (php_json_escape_string(buf, ZSTR_VAL(prop_info->name), ZSTR_LEN(prop_info->name),
					options & ~PHP_JSON_NUMERIC_CHECK, encoder) == FAILURE &&
					(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) &&
					buf->s) {
				ZSTR_LEN(buf->s) -= 4;
				smart_str_appendl(buf, "\"\"", 2);
			}

			smart_str_appendc(buf, ':');
			php_json_pretty_print_char(buf, options, ' ');

			if (php_json_encode_zval(buf, prop, options, encoder) == FAILURE &&
					!(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
				PHP_JSON_HASH_UNPROTECT_RECURSION(obj);
				return FAILURE;
			}
		}

		PHP_JSON_HASH_UNPROTECT_RECURSION(obj);
		if (encoder->depth > encoder->max_depth) {
			encoder->error_code = PHP_JSON_ERROR_DEPTH;
			if (!(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
				return FAILURE;
			}
		}
		--encoder->depth;

		if (need_comma) {
			php_json_pretty_print_char(buf, options, '\n');
			php_json_pretty_print_indent(buf, options, encoder);
		}
		smart_str_appendc(buf, '}');
		return SUCCESS;
	} else {
		prop_ht = myht = zend_get_properties_for(val, ZEND_PROP_PURPOSE_JSON);
		r = PHP_JSON_OUTPUT_OBJECT;
	}

	if (myht && GC_IS_RECURSIVE(myht)) {
		encoder->error_code = PHP_JSON_ERROR_RECURSION;
		smart_str_appendl(buf, "null", 4);
		zend_release_properties(prop_ht);
		return FAILURE;
	}

	PHP_JSON_HASH_PROTECT_RECURSION(myht);

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		smart_str_appendc(buf, '[');
	} else {
		smart_str_appendc(buf, '{');
	}

	++encoder->depth;

	i = myht ? zend_hash_num_elements(myht) : 0;

	if (i > 0) {
		zend_string *key;
		zval *data;
		zend_ulong index;

		ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, data) {
			if (r == PHP_JSON_OUTPUT_ARRAY) {
				if (need_comma) {
					smart_str_appendc(buf, ',');
				} else {
					need_comma = 1;
				}

				php_json_pretty_print_char(buf, options, '\n');
				php_json_pretty_print_indent(buf, options, encoder);
			} else if (r == PHP_JSON_OUTPUT_OBJECT) {
				if (key) {
					if (ZSTR_VAL(key)[0] == '\0' && ZSTR_LEN(key) > 0 && Z_TYPE_P(val) == IS_OBJECT) {
						/* Skip protected and private members. */
						continue;
					}

					if (need_comma) {
						smart_str_appendc(buf, ',');
					} else {
						need_comma = 1;
					}

					php_json_pretty_print_char(buf, options, '\n');
					php_json_pretty_print_indent(buf, options, encoder);

					if (php_json_escape_string(buf, ZSTR_VAL(key), ZSTR_LEN(key),
								options & ~PHP_JSON_NUMERIC_CHECK, encoder) == FAILURE &&
							(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) &&
							buf->s) {
						ZSTR_LEN(buf->s) -= 4;
						smart_str_appendl(buf, "\"\"", 2);
					}
				} else {
					if (need_comma) {
						smart_str_appendc(buf, ',');
					} else {
						need_comma = 1;
					}

					php_json_pretty_print_char(buf, options, '\n');
					php_json_pretty_print_indent(buf, options, encoder);

					smart_str_appendc(buf, '"');
					smart_str_append_long(buf, (zend_long) index);
					smart_str_appendc(buf, '"');
				}

				smart_str_appendc(buf, ':');
				php_json_pretty_print_char(buf, options, ' ');
			}

			if (php_json_encode_zval(buf, data, options, encoder) == FAILURE &&
					!(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
				PHP_JSON_HASH_UNPROTECT_RECURSION(myht);
				zend_release_properties(prop_ht);
				return FAILURE;
			}
		} ZEND_HASH_FOREACH_END();
	}

	PHP_JSON_HASH_UNPROTECT_RECURSION(myht);

	if (encoder->depth > encoder->max_depth) {
		encoder->error_code = PHP_JSON_ERROR_DEPTH;
		if (!(options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
			zend_release_properties(prop_ht);
			return FAILURE;
		}
	}
	--encoder->depth;

	/* Only keep closing bracket on same line for empty arrays/objects */
	if (need_comma) {
		php_json_pretty_print_char(buf, options, '\n');
		php_json_pretty_print_indent(buf, options, encoder);
	}

	if (r == PHP_JSON_OUTPUT_ARRAY) {
		smart_str_appendc(buf, ']');
	} else {
		smart_str_appendc(buf, '}');
	}

	zend_release_properties(prop_ht);
	return SUCCESS;
}
/* }}} */

zend_result php_json_escape_string(
		smart_str *buf, const char *s, size_t len,
		int options, php_json_encoder *encoder) /* {{{ */
{
	unsigned int us;
	size_t pos, checkpoint;
	char *dst;

	if (len == 0) {
		smart_str_appendl(buf, "\"\"", 2);
		return SUCCESS;
	}

	if (options & PHP_JSON_NUMERIC_CHECK) {
		double d;
		int type;
		zend_long p;

		if ((type = is_numeric_string(s, len, &p, &d, 0)) != 0) {
			if (type == IS_LONG) {
				smart_str_append_long(buf, p);
				return SUCCESS;
			} else if (type == IS_DOUBLE && php_json_is_valid_double(d)) {
				php_json_encode_double(buf, d, options);
				return SUCCESS;
			}
		}

	}
	checkpoint = buf->s ? ZSTR_LEN(buf->s) : 0;

	/* pre-allocate for string length plus 2 quotes */
	smart_str_alloc(buf, len+2, 0);
	smart_str_appendc(buf, '"');

	pos = 0;

	do {
		static const uint32_t charmap[8] = {
			0xffffffff, 0x500080c4, 0x10000000, 0x00000000,
			0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};

		us = (unsigned char)s[pos];
		if (EXPECTED(!ZEND_BIT_TEST(charmap, us))) {
			pos++;
			len--;
			if (len == 0) {
				smart_str_appendl(buf, s, pos);
				break;
			}
		} else {
			if (pos) {
				smart_str_appendl(buf, s, pos);
				s += pos;
				pos = 0;
			}
			us = (unsigned char)s[0];
			if (UNEXPECTED(us >= 0x80)) {
				zend_result status;
				us = php_next_utf8_char((unsigned char *)s, len, &pos, &status);

				/* check whether UTF8 character is correct */
				if (UNEXPECTED(status != SUCCESS)) {
					if (options & PHP_JSON_INVALID_UTF8_IGNORE) {
						/* ignore invalid UTF8 character */
					} else if (options & PHP_JSON_INVALID_UTF8_SUBSTITUTE) {
						/* Use Unicode character 'REPLACEMENT CHARACTER' (U+FFFD) */
						if (options & PHP_JSON_UNESCAPED_UNICODE) {
							smart_str_appendl(buf, "\xef\xbf\xbd", 3);
						} else {
							smart_str_appendl(buf, "\\ufffd", 6);
						}
					} else {
						ZSTR_LEN(buf->s) = checkpoint;
						encoder->error_code = PHP_JSON_ERROR_UTF8;
						if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
							smart_str_appendl(buf, "null", 4);
						}
						return FAILURE;
					}

				/* Escape U+2028/U+2029 line terminators, UNLESS both
				   JSON_UNESCAPED_UNICODE and
				   JSON_UNESCAPED_LINE_TERMINATORS were provided */
				} else if ((options & PHP_JSON_UNESCAPED_UNICODE)
				    && ((options & PHP_JSON_UNESCAPED_LINE_TERMINATORS)
						|| us < 0x2028 || us > 0x2029)) {
					smart_str_appendl(buf, s, pos);
				} else {
					/* From http://en.wikipedia.org/wiki/UTF16 */
					if (us >= 0x10000) {
						unsigned int next_us;

						us -= 0x10000;
						next_us = (unsigned short)((us & 0x3ff) | 0xdc00);
						us = (unsigned short)((us >> 10) | 0xd800);
						dst = smart_str_extend(buf, 6);
						dst[0] = '\\';
						dst[1] = 'u';
						dst[2] = digits[(us >> 12) & 0xf];
						dst[3] = digits[(us >> 8) & 0xf];
						dst[4] = digits[(us >> 4) & 0xf];
						dst[5] = digits[us & 0xf];
						us = next_us;
					}
					dst = smart_str_extend(buf, 6);
					dst[0] = '\\';
					dst[1] = 'u';
					dst[2] = digits[(us >> 12) & 0xf];
					dst[3] = digits[(us >> 8) & 0xf];
					dst[4] = digits[(us >> 4) & 0xf];
					dst[5] = digits[us & 0xf];
				}
				s += pos;
				len -= pos;
				pos = 0;
			} else {
				s++;
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
						ZEND_ASSERT(us < ' ');
						dst = smart_str_extend(buf, 6);
						dst[0] = '\\';
						dst[1] = 'u';
						dst[2] = '0';
						dst[3] = '0';
						dst[4] = digits[(us >> 4) & 0xf];
						dst[5] = digits[us & 0xf];
						break;
				}
				len--;
			}
		}
	} while (len);

	smart_str_appendc(buf, '"');

	return SUCCESS;
}
/* }}} */

static zend_result php_json_encode_serializable_object(smart_str *buf, zval *val, int options, php_json_encoder *encoder) /* {{{ */
{
	zend_class_entry *ce = Z_OBJCE_P(val);
	zend_object *obj = Z_OBJ_P(val);
	uint32_t *guard = zend_get_recursion_guard(obj);
	zval retval, fname;
	zend_result return_code;

	ZEND_ASSERT(guard != NULL);

	if (ZEND_GUARD_IS_RECURSIVE(guard, JSON)) {
		encoder->error_code = PHP_JSON_ERROR_RECURSION;
		if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
			smart_str_appendl(buf, "null", 4);
		}
		return FAILURE;
	}

	ZEND_GUARD_PROTECT_RECURSION(guard, JSON);

	ZVAL_STRING(&fname, "jsonSerialize");

	if (FAILURE == call_user_function(NULL, val, &fname, &retval, 0, NULL) || Z_TYPE(retval) == IS_UNDEF) {
		if (!EG(exception)) {
			zend_throw_exception_ex(NULL, 0, "Failed calling %s::jsonSerialize()", ZSTR_VAL(ce->name));
		}
		zval_ptr_dtor(&fname);

		if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
			smart_str_appendl(buf, "null", 4);
		}
		ZEND_GUARD_UNPROTECT_RECURSION(guard, JSON);
		return FAILURE;
	}

	if (EG(exception)) {
		/* Error already raised */
		zval_ptr_dtor(&retval);
		zval_ptr_dtor(&fname);

		if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
			smart_str_appendl(buf, "null", 4);
		}
		ZEND_GUARD_UNPROTECT_RECURSION(guard, JSON);
		return FAILURE;
	}

	if ((Z_TYPE(retval) == IS_OBJECT) &&
		(Z_OBJ(retval) == Z_OBJ_P(val))) {
		/* Handle the case where jsonSerialize does: return $this; by going straight to encode array */
		ZEND_GUARD_UNPROTECT_RECURSION(guard, JSON);
		return_code = php_json_encode_array(buf, &retval, options, encoder);
	} else {
		/* All other types, encode as normal */
		return_code = php_json_encode_zval(buf, &retval, options, encoder);
		ZEND_GUARD_UNPROTECT_RECURSION(guard, JSON);
	}

	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&fname);

	return return_code;
}
/* }}} */

static zend_result php_json_encode_serializable_enum(smart_str *buf, zval *val, int options, php_json_encoder *encoder)
{
	zend_class_entry *ce = Z_OBJCE_P(val);
	if (ce->enum_backing_type == IS_UNDEF) {
		encoder->error_code = PHP_JSON_ERROR_NON_BACKED_ENUM;
		smart_str_appendc(buf, '0');
		return FAILURE;
	}

	zval *value_zv = zend_enum_fetch_case_value(Z_OBJ_P(val));
	return php_json_encode_zval(buf, value_zv, options, encoder);
}

zend_result php_json_encode_zval(smart_str *buf, zval *val, int options, php_json_encoder *encoder) /* {{{ */
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
				encoder->error_code = PHP_JSON_ERROR_INF_OR_NAN;
				smart_str_appendc(buf, '0');
			}
			break;

		case IS_STRING:
			return php_json_escape_string(buf, Z_STRVAL_P(val), Z_STRLEN_P(val), options, encoder);

		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(val), php_json_serializable_ce)) {
				return php_json_encode_serializable_object(buf, val, options, encoder);
			}
			if (Z_OBJCE_P(val)->ce_flags & ZEND_ACC_ENUM) {
				return php_json_encode_serializable_enum(buf, val, options, encoder);
			}
			/* fallthrough -- Non-serializable object */
			ZEND_FALLTHROUGH;
		case IS_ARRAY: {
			/* Avoid modifications (and potential freeing) of the array through a reference when a
			 * jsonSerialize() method is invoked. */
			zval zv;
			int res;
			ZVAL_COPY(&zv, val);
			res = php_json_encode_array(buf, &zv, options, encoder);
			zval_ptr_dtor_nogc(&zv);
			return res;
		}

		case IS_REFERENCE:
			val = Z_REFVAL_P(val);
			goto again;

		default:
			encoder->error_code = PHP_JSON_ERROR_UNSUPPORTED_TYPE;
			if (options & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR) {
				smart_str_appendl(buf, "null", 4);
			}
			return FAILURE;
	}

	return SUCCESS;
}
/* }}} */
