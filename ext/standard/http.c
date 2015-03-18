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
   | Authors: Sara Golemon <pollita@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

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
	zval *zdata = NULL, copyzval;

	if (!ht) {
		return FAILURE;
	}

	if (ht->u.v.nApplyCount > 0) {
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

	ZEND_HASH_FOREACH_KEY_VAL_IND(ht, idx, key, zdata) {
		/* handling for private & protected object properties */
		if (key) {
			if (key->val[0] == '\0' && type != NULL) {
				const char *tmp;

				zend_object *zobj = Z_OBJ_P(type);
				if (zend_check_property_access(zobj, key) != SUCCESS) {
					/* private or protected property access outside of the class */
					continue;
				}
				zend_unmangle_property_name_ex(key, &tmp, &prop_name, &prop_len);
			} else {
				prop_name = key->val;
				prop_len = key->len;
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
				newprefix_len = key_suffix_len + ekey->len + key_prefix_len + 3 /* %5B */;
				newprefix = emalloc(newprefix_len + 1);
				p = newprefix;

				if (key_prefix) {
					memcpy(p, key_prefix, key_prefix_len);
					p += key_prefix_len;
				}

				memcpy(p, ekey->val, ekey->len);
				p += ekey->len;
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
				ekey_len = spprintf(&ekey, 0, "%pd", idx);
				newprefix_len = key_prefix_len + num_prefix_len + ekey_len + key_suffix_len + 3 /* %5B */;
				newprefix = emalloc(newprefix_len + 1);
				p = newprefix;

				if (key_prefix) {
					memcpy(p, key_prefix, key_prefix_len);
					p += key_prefix_len;
				}

				memcpy(p, num_prefix, num_prefix_len);
				p += num_prefix_len;

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
			if (ZEND_HASH_APPLY_PROTECTION(ht)) {
				ht->u.v.nApplyCount++;
			}
			php_url_encode_hash_ex(HASH_OF(zdata), formstr, NULL, 0, newprefix, newprefix_len, "%5D", 3, (Z_TYPE_P(zdata) == IS_OBJECT ? zdata : NULL), arg_sep, enc_type);
			if (ZEND_HASH_APPLY_PROTECTION(ht)) {
				ht->u.v.nApplyCount--;
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
			smart_str_appendl(formstr, key_prefix, key_prefix_len);
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
			smart_str_appendl(formstr, key_suffix, key_suffix_len);
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
				case IS_DOUBLE:
					{
						char *ekey;
					  	size_t ekey_len;
						ekey_len = spprintf(&ekey, 0, "%.*G", (int) EG(precision), Z_DVAL_P(zdata));
						smart_str_appendl(formstr, ekey, ekey_len);
						efree(ekey);
				  	}
					break;
				default:
					{
						zend_string *ekey;
						/* fall back on convert to string */
						ZVAL_DUP(&copyzval, zdata);
						convert_to_string_ex(&copyzval);
						if (enc_type == PHP_QUERY_RFC3986) {
							ekey = php_raw_url_encode(Z_STRVAL(copyzval), Z_STRLEN(copyzval));
						} else {
							ekey = php_url_encode(Z_STRVAL(copyzval), Z_STRLEN(copyzval));
						}
						smart_str_append(formstr, ekey);
						zval_ptr_dtor(&copyzval);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|ssl", &formdata, &prefix, &prefix_len, &arg_sep, &arg_sep_len, &enc_type) != SUCCESS) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(formdata) != IS_ARRAY && Z_TYPE_P(formdata) != IS_OBJECT) {
		php_error_docref(NULL, E_WARNING, "Parameter 1 expected to be Array or Object.  Incorrect value given");
		RETURN_FALSE;
	}

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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

