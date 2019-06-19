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
   | Authors: Sara Golemon <pollita@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "php_http.h"
#include "php_ini.h"
#include "url.h"

#define URL_DEFAULT_ARG_SEP "&"

/* {{{ php_url_encode_hash */
PHPAPI int php_url_encode_hash_ex(HashTable *ht, smart_str *formstr,
				const char *num_prefix, size_t num_prefix_len,
				const char *key_prefix, size_t key_prefix_len,
				const char *key_suffix, size_t key_suffix_len,
			  zval *type, char *arg_sep, int enc_type)
{
	zend_string *key = NULL;
	char *newprefix, *p;
	const char *prop_name;
	size_t arg_sep_len, newprefix_len, prop_len;
	zend_ulong idx;
	zval *zdata = NULL;

	if (!ht) {
		return FAILURE;
	}

	if (GC_IS_RECURSIVE(ht)) {
		/* Prevent recursion */
		return SUCCESS;
	}

	if (!arg_sep) {
		arg_sep = INI_STR("arg_separator.output");
		if (!arg_sep || !strlen(arg_sep)) {
			arg_sep = URL_DEFAULT_ARG_SEP;
		}
	}
	arg_sep_len = strlen(arg_sep);

	ZEND_HASH_FOREACH_KEY_VAL(ht, idx, key, zdata) {
		zend_bool is_dynamic = 1;
		if (Z_TYPE_P(zdata) == IS_INDIRECT) {
			zdata = Z_INDIRECT_P(zdata);
			if (Z_ISUNDEF_P(zdata)) {
				continue;
			}

			is_dynamic = 0;
		}

		/* handling for private & protected object properties */
		if (key) {
			prop_name = ZSTR_VAL(key);
			prop_len = ZSTR_LEN(key);

			if (type != NULL && zend_check_property_access(Z_OBJ_P(type), key, is_dynamic) != SUCCESS) {
				/* property not visible in this scope */
				continue;
			}

			if (ZSTR_VAL(key)[0] == '\0' && type != NULL) {
				const char *tmp;
				zend_unmangle_property_name_ex(key, &tmp, &prop_name, &prop_len);
			} else {
				prop_name = ZSTR_VAL(key);
				prop_len = ZSTR_LEN(key);
			}
		} else {
			prop_name = NULL;
			prop_len = 0;
		}

		ZVAL_DEREF(zdata);
		if (Z_TYPE_P(zdata) == IS_ARRAY || Z_TYPE_P(zdata) == IS_OBJECT) {
			if (key) {
				zend_string *ekey;
				if (enc_type == PHP_QUERY_RFC3986) {
					ekey = php_raw_url_encode(prop_name, prop_len);
				} else {
					ekey = php_url_encode(prop_name, prop_len);
				}
				newprefix_len = key_suffix_len + ZSTR_LEN(ekey) + key_prefix_len + 3 /* %5B */;
				newprefix = emalloc(newprefix_len + 1);
				p = newprefix;

				if (key_prefix) {
					memcpy(p, key_prefix, key_prefix_len);
					p += key_prefix_len;
				}

				memcpy(p, ZSTR_VAL(ekey), ZSTR_LEN(ekey));
				p += ZSTR_LEN(ekey);
				zend_string_free(ekey);

				if (key_suffix) {
					memcpy(p, key_suffix, key_suffix_len);
					p += key_suffix_len;
				}
				*(p++) = '%';
				*(p++) = '5';
				*(p++) = 'B';
				*p = '\0';
			} else {
				char *ekey;
				size_t ekey_len;
				/* Is an integer key */
				ekey_len = spprintf(&ekey, 0, ZEND_LONG_FMT, idx);
				newprefix_len = key_prefix_len + num_prefix_len + ekey_len + key_suffix_len + 3 /* %5B */;
				newprefix = emalloc(newprefix_len + 1);
				p = newprefix;

				if (key_prefix) {
					memcpy(p, key_prefix, key_prefix_len);
					p += key_prefix_len;
				}

				if (num_prefix) {
					memcpy(p, num_prefix, num_prefix_len);
					p += num_prefix_len;
				}

				memcpy(p, ekey, ekey_len);
				p += ekey_len;
				efree(ekey);

				if (key_suffix) {
					memcpy(p, key_suffix, key_suffix_len);
					p += key_suffix_len;
				}
				*(p++) = '%';
				*(p++) = '5';
				*(p++) = 'B';
				*p = '\0';
			}
			if (!(GC_FLAGS(ht) & GC_IMMUTABLE)) {
				GC_PROTECT_RECURSION(ht);
			}
			php_url_encode_hash_ex(HASH_OF(zdata), formstr, NULL, 0, newprefix, newprefix_len, "%5D", 3, (Z_TYPE_P(zdata) == IS_OBJECT ? zdata : NULL), arg_sep, enc_type);
			if (!(GC_FLAGS(ht) & GC_IMMUTABLE)) {
				GC_UNPROTECT_RECURSION(ht);
			}
			efree(newprefix);
		} else if (Z_TYPE_P(zdata) == IS_NULL || Z_TYPE_P(zdata) == IS_RESOURCE) {
			/* Skip these types */
			continue;
		} else {
			if (formstr->s) {
				smart_str_appendl(formstr, arg_sep, arg_sep_len);
			}
			/* Simple key=value */
			if (key_prefix) {
				smart_str_appendl(formstr, key_prefix, key_prefix_len);
			}
			if (key) {
				zend_string *ekey;
				if (enc_type == PHP_QUERY_RFC3986) {
					ekey = php_raw_url_encode(prop_name, prop_len);
				} else {
					ekey = php_url_encode(prop_name, prop_len);
				}
				smart_str_append(formstr, ekey);
				zend_string_free(ekey);
			} else {
				/* Numeric key */
				if (num_prefix) {
					smart_str_appendl(formstr, num_prefix, num_prefix_len);
				}
				smart_str_append_long(formstr, idx);
			}
			if (key_suffix) {
				smart_str_appendl(formstr, key_suffix, key_suffix_len);
			}
			smart_str_appendl(formstr, "=", 1);
			switch (Z_TYPE_P(zdata)) {
				case IS_STRING: {
						zend_string *ekey;
						if (enc_type == PHP_QUERY_RFC3986) {
							ekey = php_raw_url_encode(Z_STRVAL_P(zdata), Z_STRLEN_P(zdata));
						} else {
							ekey = php_url_encode(Z_STRVAL_P(zdata), Z_STRLEN_P(zdata));
						}
						smart_str_append(formstr, ekey);
						zend_string_free(ekey);
					}
					break;
				case IS_LONG:
					smart_str_append_long(formstr, Z_LVAL_P(zdata));
					break;
				case IS_FALSE:
					smart_str_appendl(formstr, "0", sizeof("0")-1);
					break;
				case IS_TRUE:
					smart_str_appendl(formstr, "1", sizeof("1")-1);
					break;
				default:
					{
						zend_string *ekey;
						zend_string *tmp;
						zend_string *str= zval_get_tmp_string(zdata, &tmp);
						if (enc_type == PHP_QUERY_RFC3986) {
							ekey = php_raw_url_encode(ZSTR_VAL(str), ZSTR_LEN(str));
						} else {
							ekey = php_url_encode(ZSTR_VAL(str), ZSTR_LEN(str));
						}
						smart_str_append(formstr, ekey);
						zend_tmp_string_release(tmp);
						zend_string_free(ekey);
					}
			}
		}
	} ZEND_HASH_FOREACH_END();

	return SUCCESS;
}
/* }}} */

/* {{{ proto string http_build_query(mixed formdata [, string prefix [, string arg_separator [, int enc_type]]])
   Generates a form-encoded query string from an associative array or object. */
PHP_FUNCTION(http_build_query)
{
	zval *formdata;
	char *prefix = NULL, *arg_sep=NULL;
	size_t arg_sep_len = 0, prefix_len = 0;
	smart_str formstr = {0};
	zend_long enc_type = PHP_QUERY_RFC1738;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_ARRAY_OR_OBJECT(formdata)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(prefix, prefix_len)
		Z_PARAM_STRING(arg_sep, arg_sep_len)
		Z_PARAM_LONG(enc_type)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (php_url_encode_hash_ex(HASH_OF(formdata), &formstr, prefix, prefix_len, NULL, 0, NULL, 0, (Z_TYPE_P(formdata) == IS_OBJECT ? formdata : NULL), arg_sep, (int)enc_type) == FAILURE) {
		if (formstr.s) {
			smart_str_free(&formstr);
		}
		RETURN_FALSE;
	}

	if (!formstr.s) {
		RETURN_EMPTY_STRING();
	}

	smart_str_0(&formstr);

	RETURN_NEW_STR(formstr.s);
}
/* }}} */
