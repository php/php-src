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
#include "SAPI.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"

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
			zend_string_free(encoded_data);
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
			zend_string_free(tmp);
			zend_string_free(encoded_data);
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

/* {{{ php_url_encode_hash */
PHPAPI void php_url_encode_hash_ex(HashTable *ht, smart_str *formstr,
				const char *num_prefix, size_t num_prefix_len,
				const zend_string *key_prefix,
				zval *type, const zend_string *arg_sep, int enc_type)
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
		bool is_dynamic = 1;
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
			zend_string *new_prefix;
			if (key) {
				zend_string *encoded_key;
				if (enc_type == PHP_QUERY_RFC3986) {
					encoded_key = php_raw_url_encode(prop_name, prop_len);
				} else {
					encoded_key = php_url_encode(prop_name, prop_len);
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

				index_int_as_str_len = spprintf(&index_int_as_str, 0, ZEND_LONG_FMT, idx);

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
			GC_TRY_PROTECT_RECURSION(ht);
			php_url_encode_hash_ex(HASH_OF(zdata), formstr, NULL, 0, new_prefix, (Z_TYPE_P(zdata) == IS_OBJECT ? zdata : NULL), arg_sep, enc_type);
			GC_TRY_UNPROTECT_RECURSION(ht);
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
		Z_PARAM_STR_OR_NULL(arg_sep)
		Z_PARAM_LONG(enc_type)
	ZEND_PARSE_PARAMETERS_END();

	php_url_encode_hash_ex(HASH_OF(formdata), &formstr, prefix, prefix_len, /* key_prefix */ NULL, (Z_TYPE_P(formdata) == IS_OBJECT ? formdata : NULL), arg_sep, (int)enc_type);

	RETURN_STR(smart_str_extract(&formstr));
}
/* }}} */

static zend_result cache_request_parse_body_option(HashTable *options, zval *option, int cache_offset)
{
	if (option) {
		zend_long result;
		if (Z_TYPE_P(option) == IS_STRING) {
			zend_string *errstr;
			result = zend_ini_parse_quantity(Z_STR_P(option), &errstr);
			if (errstr) {
				zend_error(E_WARNING, "%s", ZSTR_VAL(errstr));
				zend_string_release(errstr);
			}
		} else if (Z_TYPE_P(option) == IS_LONG) {
			result = Z_LVAL_P(option);
		} else {
			zend_value_error("Invalid %s value in $options argument", zend_zval_value_name(option));
			return FAILURE;
		}
		SG(request_parse_body_context).options_cache[cache_offset].set = true;
		SG(request_parse_body_context).options_cache[cache_offset].value = result;
	} else {
		SG(request_parse_body_context).options_cache[cache_offset].set = false;
	}

	return SUCCESS;
}

static zend_result cache_request_parse_body_options(HashTable *options)
{
	zend_string *key;
	zval *value;
	ZEND_HASH_FOREACH_STR_KEY_VAL(options, key, value) {
		if (!key) {
			zend_value_error("Invalid integer key in $options argument");
			return FAILURE;
		}
		if (ZSTR_LEN(key) == 0) {
			zend_value_error("Invalid empty string key in $options argument");
			return FAILURE;
		}

#define CHECK_OPTION(name) \
	if (zend_string_equals_literal_ci(key, #name)) { \
		if (cache_request_parse_body_option(options, value, REQUEST_PARSE_BODY_OPTION_ ## name) == FAILURE) { \
			return FAILURE; \
		} \
		continue; \
	}

		switch (ZSTR_VAL(key)[0]) {
			case 'm':
			case 'M':
				CHECK_OPTION(max_file_uploads);
				CHECK_OPTION(max_input_vars);
				CHECK_OPTION(max_multipart_body_parts);
				break;
			case 'p':
			case 'P':
				CHECK_OPTION(post_max_size);
				break;
			case 'u':
			case 'U':
				CHECK_OPTION(upload_max_filesize);
				break;
		}

		zend_value_error("Invalid key \"%s\" in $options argument", ZSTR_VAL(key));
		return FAILURE;
	} ZEND_HASH_FOREACH_END();

#undef CACHE_OPTION

	return SUCCESS;
}

PHP_FUNCTION(request_parse_body)
{
	HashTable *options = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_NULL(options)
	ZEND_PARSE_PARAMETERS_END();

	SG(request_parse_body_context).throw_exceptions = true;
	if (options) {
		if (cache_request_parse_body_options(options) == FAILURE) {
			goto exit;
		}
	}

	if (!SG(request_info).content_type) {
		zend_throw_error(zend_ce_request_parse_body_exception, "Request does not provide a content type");
		goto exit;
	}

	sapi_read_post_data();
	if (!SG(request_info).post_entry) {
		zend_throw_error(spl_ce_InvalidArgumentException, "Content-Type \"%s\" is not supported", SG(request_info).content_type);
		goto exit;
	}

	zval post, files, old_post, old_files;
	zval *global_post = &PG(http_globals)[TRACK_VARS_POST];
	zval *global_files = &PG(http_globals)[TRACK_VARS_FILES];

	ZVAL_COPY_VALUE(&old_post, global_post);
	ZVAL_COPY_VALUE(&old_files, global_files);
	array_init(global_post);
	array_init(global_files);
	sapi_handle_post(global_post);
	ZVAL_COPY_VALUE(&post, global_post);
	ZVAL_COPY_VALUE(&files, global_files);
	ZVAL_COPY_VALUE(global_post, &old_post);
	ZVAL_COPY_VALUE(global_files, &old_files);

	RETVAL_ARR(zend_new_pair(&post, &files));

exit:
	SG(request_parse_body_context).throw_exceptions = false;
	memset(&SG(request_parse_body_context).options_cache, 0, sizeof(SG(request_parse_body_context).options_cache));
}
