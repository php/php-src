/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
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
				const char *num_prefix, int num_prefix_len,
				const char *key_prefix, int key_prefix_len,
				const char *key_suffix, int key_suffix_len,
			  zval *type, char *arg_sep, int enc_type TSRMLS_DC)
{
	char *key = NULL;
	char *ekey, *newprefix, *p;
	int arg_sep_len, ekey_len, key_type, newprefix_len;
	uint key_len;
	ulong idx;
	zval **zdata = NULL, *copyzval;

	if (!ht) {
		return FAILURE;
	}

	if (ht->nApplyCount > 0) {
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

	for (zend_hash_internal_pointer_reset(ht);
		(key_type = zend_hash_get_current_key_ex(ht, &key, &key_len, &idx, 0, NULL)) != HASH_KEY_NON_EXISTANT;
		zend_hash_move_forward(ht)
	) {
		if (key_type == HASH_KEY_IS_STRING && key_len && key[key_len-1] == '\0') {
			/* We don't want that trailing NULL */
			key_len -= 1;
		}

		/* handling for private & protected object properties */
		if (key && *key == '\0' && type != NULL) {
			const char *tmp;

			zend_object *zobj = zend_objects_get_address(type TSRMLS_CC);
			if (zend_check_property_access(zobj, key, key_len-1 TSRMLS_CC) != SUCCESS) {
				/* private or protected property access outside of the class */
				continue;
			}
			zend_unmangle_property_name(key, key_len-1, &tmp, (const char**)&key);
			key_len = strlen(key);		
		}

		if (zend_hash_get_current_data_ex(ht, (void **)&zdata, NULL) == FAILURE || !zdata || !(*zdata)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error traversing form data array");
			return FAILURE;
		}
		if (Z_TYPE_PP(zdata) == IS_ARRAY || Z_TYPE_PP(zdata) == IS_OBJECT) {
			if (key_type == HASH_KEY_IS_STRING) {
				if (enc_type == PHP_QUERY_RFC3986) {
					ekey = php_raw_url_encode(key, key_len, &ekey_len);
				} else {
					ekey = php_url_encode(key, key_len, &ekey_len);
				}
				newprefix_len = key_suffix_len + ekey_len + key_prefix_len + 3 /* %5B */;
				newprefix = emalloc(newprefix_len + 1);
				p = newprefix;

				if (key_prefix) {
					memcpy(p, key_prefix, key_prefix_len);
					p += key_prefix_len;
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
			} else {
				/* Is an integer key */
				ekey_len = spprintf(&ekey, 0, "%ld", idx);
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
			ht->nApplyCount++;
			php_url_encode_hash_ex(HASH_OF(*zdata), formstr, NULL, 0, newprefix, newprefix_len, "%5D", 3, (Z_TYPE_PP(zdata) == IS_OBJECT ? *zdata : NULL), arg_sep, enc_type TSRMLS_CC);
			ht->nApplyCount--;
			efree(newprefix);
		} else if (Z_TYPE_PP(zdata) == IS_NULL || Z_TYPE_PP(zdata) == IS_RESOURCE) {
			/* Skip these types */
			continue;
		} else {
			if (formstr->len) {
				smart_str_appendl(formstr, arg_sep, arg_sep_len);
			}
			/* Simple key=value */
			smart_str_appendl(formstr, key_prefix, key_prefix_len);
			if (key_type == HASH_KEY_IS_STRING) {
				if (enc_type == PHP_QUERY_RFC3986) {
					ekey = php_raw_url_encode(key, key_len, &ekey_len);
				} else {
					ekey = php_url_encode(key, key_len, &ekey_len);
				}
				smart_str_appendl(formstr, ekey, ekey_len);
				efree(ekey);
			} else {
				/* Numeric key */
				if (num_prefix) {
					smart_str_appendl(formstr, num_prefix, num_prefix_len);
				}
				ekey_len = spprintf(&ekey, 0, "%ld", idx);
				smart_str_appendl(formstr, ekey, ekey_len);
				efree(ekey);
			}
			smart_str_appendl(formstr, key_suffix, key_suffix_len);
			smart_str_appendl(formstr, "=", 1);
			switch (Z_TYPE_PP(zdata)) {
				case IS_STRING:
					if (enc_type == PHP_QUERY_RFC3986) {
						ekey = php_raw_url_encode(Z_STRVAL_PP(zdata), Z_STRLEN_PP(zdata), &ekey_len);
					} else {
						ekey = php_url_encode(Z_STRVAL_PP(zdata), Z_STRLEN_PP(zdata), &ekey_len);						
					}
					break;
				case IS_LONG:
				case IS_BOOL:
					ekey_len = spprintf(&ekey, 0, "%ld", Z_LVAL_PP(zdata));
					break;
				case IS_DOUBLE:
					ekey_len = spprintf(&ekey, 0, "%.*G", (int) EG(precision), Z_DVAL_PP(zdata));
					break;
				default:
					/* fall back on convert to string */
					MAKE_STD_ZVAL(copyzval);
					*copyzval = **zdata;
					zval_copy_ctor(copyzval);
					convert_to_string_ex(&copyzval);
					if (enc_type == PHP_QUERY_RFC3986) {
						ekey = php_raw_url_encode(Z_STRVAL_P(copyzval), Z_STRLEN_P(copyzval), &ekey_len);
					} else {
						ekey = php_url_encode(Z_STRVAL_P(copyzval), Z_STRLEN_P(copyzval), &ekey_len);
					}
					zval_ptr_dtor(&copyzval);
			}
			smart_str_appendl(formstr, ekey, ekey_len);
			efree(ekey);
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ proto string http_build_query(mixed formdata [, string prefix [, string arg_separator [, int enc_type]]])
   Generates a form-encoded query string from an associative array or object. */
PHP_FUNCTION(http_build_query)
{
	zval *formdata;
	char *prefix = NULL, *arg_sep=NULL;
	int arg_sep_len = 0, prefix_len = 0;
	smart_str formstr = {0};
	long enc_type = PHP_QUERY_RFC1738;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ssl", &formdata, &prefix, &prefix_len, &arg_sep, &arg_sep_len, &enc_type) != SUCCESS) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(formdata) != IS_ARRAY && Z_TYPE_P(formdata) != IS_OBJECT) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Parameter 1 expected to be Array or Object.  Incorrect value given");
		RETURN_FALSE;
	}

	if (php_url_encode_hash_ex(HASH_OF(formdata), &formstr, prefix, prefix_len, NULL, 0, NULL, 0, (Z_TYPE_P(formdata) == IS_OBJECT ? formdata : NULL), arg_sep, enc_type TSRMLS_CC) == FAILURE) {
		if (formstr.c) {
			efree(formstr.c);
		}
		RETURN_FALSE;
	}

	if (!formstr.c) {
		RETURN_EMPTY_STRING();
	}

	smart_str_0(&formstr);
	
	RETURN_STRINGL(formstr.c, formstr.len, 0);
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

