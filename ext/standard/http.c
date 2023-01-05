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
   | Authors: Sara Golemon <pollita@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "php_http.h"
#include "php_ini.h"
#include "url.h"

static void php_url_encode_scalar(zval *scalar, smart_str *form_str,
	int encoding_type, zend_ulong index_int,
	const char *index_string, size_t index_string_len,
	const char *num_prefix, size_t num_prefix_len,
	const zend_string *key_prefix,
	const zend_string *arg_sep)
{
	if (form_str->s) {
		smart_str_append(form_str, arg_sep);
	}
	/* Simple key=value */
	if (key_prefix) {
		smart_str_append(form_str, key_prefix);
	}
	if (index_string) {
		zend_string *encoded_key;
		if (encoding_type == PHP_QUERY_RFC3986) {
			encoded_key = php_raw_url_encode(index_string, index_string_len);
		} else {
			encoded_key = php_url_encode(index_string, index_string_len);
		}
		smart_str_append(form_str, encoded_key);
		zend_string_free(encoded_key);
	} else {
		/* Numeric key */
		if (num_prefix) {
			smart_str_appendl(form_str, num_prefix, num_prefix_len);
		}
		smart_str_append_long(form_str, index_int);
	}
	if (key_prefix) {
		smart_str_appendl(form_str, "%5D", strlen("%5D"));
	}
	smart_str_appendc(form_str, '=');

	switch (Z_TYPE_P(scalar)) {
		case IS_STRING: {
			zend_string *encoded_data;
			if (encoding_type == PHP_QUERY_RFC3986) {
				encoded_data = php_raw_url_encode(Z_STRVAL_P(scalar), Z_STRLEN_P(scalar));
			} else {
				encoded_data = php_url_encode(Z_STRVAL_P(scalar), Z_STRLEN_P(scalar));
			}
			smart_str_append(form_str, encoded_data);
			zend_string_release_ex(encoded_data, false);
			break;
		}
		case IS_LONG:
			smart_str_append_long(form_str, Z_LVAL_P(scalar));
			break;
		case IS_DOUBLE: {
			zend_string *encoded_data;
			zend_string *tmp = zend_double_to_str(Z_DVAL_P(scalar));
			if (encoding_type == PHP_QUERY_RFC3986) {
				encoded_data = php_raw_url_encode(ZSTR_VAL(tmp), ZSTR_LEN(tmp));
			} else {
				encoded_data = php_url_encode(ZSTR_VAL(tmp), ZSTR_LEN(tmp));
			}
			smart_str_append(form_str, encoded_data);
			zend_string_release_ex(tmp, false);
			zend_string_release_ex(encoded_data, false);
			break;
		}
		case IS_FALSE:
			smart_str_appendc(form_str, '0');
			break;
		case IS_TRUE:
			smart_str_appendc(form_str, '1');
			break;
		/* All possible types are either handled here or previously */
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

static HashTable* php_object_get_visible_properties(/* const */ zend_object *object)
{
	/* const */ HashTable *properties = object->handlers->get_properties(object);
	//HashTable *visible_properties = zend_new_array(zend_hash_num_elements(properties));
	HashTable *visible_properties = zend_array_dup(object->handlers->get_properties(object));
	zend_string *property_name;
	zval *value;

	ZEND_HASH_FOREACH_STR_KEY_VAL(properties, property_name, value) {
		bool is_dynamic = true;
		if (Z_TYPE_P(value) == IS_INDIRECT) {
			value = Z_INDIRECT_P(value);
			if (Z_ISUNDEF_P(value)) {
				continue;
			}
			is_dynamic = false;
		}

		if (zend_check_property_access(object, property_name, is_dynamic) == FAILURE) {
			/* property not visible in this scope */
			zend_hash_del(visible_properties, property_name);
			continue;
		}

		/* handling for private & protected object properties */
		if (UNEXPECTED(ZSTR_VAL(property_name)[0] == '\0')) {
			zend_hash_del(visible_properties, property_name);
			const char *tmp;
			const char *unmangled_name;
			size_t unmangled_name_len;
			zend_unmangle_property_name_ex(property_name, &tmp, &unmangled_name, &unmangled_name_len);
			zend_hash_str_add(visible_properties, unmangled_name, unmangled_name_len, value);
		}
	} ZEND_HASH_FOREACH_END();
	return visible_properties;
}

static zend_string* php_url_encode_get_new_prefix(
	int encoding_type, zend_ulong index_int,
	const char *index_string, size_t index_string_len,
	const char *num_prefix, size_t num_prefix_len,
	const zend_string *key_prefix
) {
	zend_string *new_prefix;

	if (index_string) {
		zend_string *encoded_key;
		if (encoding_type == PHP_QUERY_RFC3986) {
			encoded_key = php_raw_url_encode(index_string, index_string_len);
		} else {
			encoded_key = php_url_encode(index_string, index_string_len);
		}

		if (key_prefix) {
			new_prefix = zend_string_concat3(ZSTR_VAL(key_prefix), ZSTR_LEN(key_prefix), ZSTR_VAL(encoded_key), ZSTR_LEN(encoded_key), "%5D%5B", strlen("%5D%5B"));
		} else {
			new_prefix = zend_string_concat2(ZSTR_VAL(encoded_key), ZSTR_LEN(encoded_key), "%5B", strlen("%5B"));
		}
		zend_string_release_ex(encoded_key, false);
	} else { /* is integer index */
		char *index_int_as_str;
		size_t index_int_as_str_len;

		index_int_as_str_len = spprintf(&index_int_as_str, 0, ZEND_LONG_FMT, index_int);

		if (key_prefix && num_prefix) {
			/* zend_string_concat4() */
			size_t len = ZSTR_LEN(key_prefix) + num_prefix_len + index_int_as_str_len + strlen("%5D%5B");
			new_prefix = zend_string_alloc(len, 0);

			memcpy(ZSTR_VAL(new_prefix), ZSTR_VAL(key_prefix), ZSTR_LEN(key_prefix));
			memcpy(ZSTR_VAL(new_prefix) + ZSTR_LEN(key_prefix), num_prefix, num_prefix_len);
			memcpy(ZSTR_VAL(new_prefix) + ZSTR_LEN(key_prefix) + num_prefix_len, index_int_as_str, index_int_as_str_len);
			memcpy(ZSTR_VAL(new_prefix) + ZSTR_LEN(key_prefix) + num_prefix_len +index_int_as_str_len, "%5D%5B", strlen("%5D%5B"));
			ZSTR_VAL(new_prefix)[len] = '\0';
		} else if (key_prefix) {
			new_prefix = zend_string_concat3(ZSTR_VAL(key_prefix), ZSTR_LEN(key_prefix), index_int_as_str, index_int_as_str_len, "%5D%5B", strlen("%5D%5B"));
		} else if (num_prefix) {
			new_prefix = zend_string_concat3(num_prefix, num_prefix_len, index_int_as_str, index_int_as_str_len, "%5B", strlen("%5B"));
		} else {
			new_prefix = zend_string_concat2(index_int_as_str, index_int_as_str_len, "%5B", strlen("%5B"));
		}
		efree(index_int_as_str);
	}

	return new_prefix;
}

static void php_url_encode_object(zend_object *object, smart_str *form_str,
	int encoding_type, zend_ulong index_int,
	const char *index_string, size_t index_string_len,
	const char *num_prefix, size_t num_prefix_len,
	const zend_string *key_prefix,
	const zend_string *arg_sep)
{
	if (GC_IS_RECURSIVE(object)) {
		/* Prevent recursion */
		return;
	}

	HashTable *properties = php_object_get_visible_properties(object);
	if (zend_hash_num_elements(properties) == 0) {
		zend_array_destroy(properties);

		zval tmp;
		/* If the data object is stringable without visible properties handle it like a string instead of empty array */
		if (object->handlers->cast_object(object, &tmp, IS_STRING) == SUCCESS) {
			php_url_encode_scalar(&tmp, form_str,
				encoding_type, index_int,
				index_string, index_string_len,
				num_prefix, num_prefix_len,
				NULL,
				arg_sep);
			zval_ptr_dtor(&tmp);
		}
		return;
	}

	GC_TRY_PROTECT_RECURSION(object);
	php_url_encode_hash_ex(properties, form_str, num_prefix, num_prefix_len, key_prefix, NULL, arg_sep, encoding_type);
	GC_TRY_UNPROTECT_RECURSION(object);
	zend_array_destroy(properties);
}

/* {{{ php_url_encode_hash */
PHPAPI void php_url_encode_hash_ex(HashTable *ht, smart_str *formstr,
				const char *num_prefix, size_t num_prefix_len,
				const zend_string *key_prefix,
				ZEND_ATTRIBUTE_UNUSED zval *type, const zend_string *arg_sep, int enc_type)
{
	zend_string *key = NULL;
	const char *prop_name;
	size_t prop_len;
	zend_ulong idx;
	zval *zdata = NULL;
	ZEND_ASSERT(ht);

	if (GC_IS_RECURSIVE(ht)) {
		/* Prevent recursion */
		return;
	}

	if (!arg_sep) {
		arg_sep = zend_ini_str("arg_separator.output", strlen("arg_separator.output"), false);
		if (ZSTR_LEN(arg_sep) == 0) {
			arg_sep = ZSTR_CHAR('&');
		}
	}

	ZEND_HASH_FOREACH_KEY_VAL(ht, idx, key, zdata) {
		if (key) {
			prop_name = ZSTR_VAL(key);
			prop_len = ZSTR_LEN(key);
		} else {
			prop_name = NULL;
			prop_len = 0;
		}

		ZVAL_DEREF(zdata);
		if (Z_TYPE_P(zdata) == IS_ARRAY) {
			zend_string *new_prefix = php_url_encode_get_new_prefix(enc_type, idx, prop_name, prop_len,
				num_prefix, num_prefix_len, key_prefix);
			GC_TRY_PROTECT_RECURSION(ht);
			php_url_encode_hash_ex(Z_ARRVAL_P(zdata), formstr, NULL, 0, new_prefix, NULL, arg_sep, enc_type);
			GC_TRY_UNPROTECT_RECURSION(ht);
			zend_string_release_ex(new_prefix, false);
		} else if (Z_TYPE_P(zdata) == IS_OBJECT) {
			zend_string *new_prefix = php_url_encode_get_new_prefix(enc_type, idx, prop_name, prop_len,
				num_prefix, num_prefix_len, key_prefix);
			php_url_encode_object(Z_OBJ_P(zdata), formstr,
				enc_type, idx,
				prop_name, prop_len,
				num_prefix, num_prefix_len,
				new_prefix,
				arg_sep);
			zend_string_release_ex(new_prefix, false);
		} else if (Z_TYPE_P(zdata) == IS_NULL || Z_TYPE_P(zdata) == IS_RESOURCE) {
			/* Skip these types */
			continue;
		} else {
			php_url_encode_scalar(zdata, formstr,
				enc_type, idx,
				prop_name, prop_len,
				num_prefix, num_prefix_len,
				key_prefix,
				arg_sep);
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

	/* If there is a prefix we need to close the key with an encoded ] ("%5D") */
/* {{{ Generates a form-encoded query string from an associative array or object. */
PHP_FUNCTION(http_build_query)
{
	zval *formdata;
	char *prefix = NULL;
	size_t prefix_len = 0;
	zend_string *arg_sep = NULL;
	smart_str formstr = {0};
	zend_long enc_type = PHP_QUERY_RFC1738;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_ARRAY_OR_OBJECT(formdata)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(prefix, prefix_len)
		Z_PARAM_STR(arg_sep)
		Z_PARAM_LONG(enc_type)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(formdata) == IS_OBJECT) {
		php_url_encode_object(Z_OBJ_P(formdata), &formstr,
			(int) enc_type, /* int_index */ 0,
			/* string_index */ NULL, 0,
			prefix, prefix_len,
			NULL,
			arg_sep);
	} else {
		php_url_encode_hash_ex(Z_ARRVAL_P(formdata), &formstr, prefix, prefix_len, /* key_prefix */ NULL, NULL, arg_sep, (int)enc_type);
	}

	RETURN_STR(smart_str_extract(&formstr));
}
/* }}} */
