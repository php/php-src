/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2005 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Sara Golemon <pollita@php.net>                               |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_hash.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"

static int php_hash_le_hash;
HashTable php_hash_hashtable;

/* Hash Registry Access */

PHP_HASH_API php_hash_ops *php_hash_fetch_ops(const char *algo, int algo_len)
{
	php_hash_ops *ops;

	if (zend_hash_find(&php_hash_hashtable, algo, algo_len + 1, (void**)&ops) == SUCCESS) {
		return ops;
	}

	return NULL;
}

PHP_HASH_API void php_hash_register_algo(const char *algo, php_hash_ops *ops)
{
	zend_hash_add(&php_hash_hashtable, algo, strlen(algo) + 1, ops, sizeof(php_hash_ops), NULL);
}

/* Userspace */

inline void php_hash_bin2hex(char *out, unsigned char *in, int in_len)
{
	static const char hexits[16] = "0123456789abcdef";
	int i;

	for(i = 0; i < in_len; i++) {
		out[i * 2]       = hexits[in[i] >> 4];
		out[(i * 2) + 1] = hexits[in[i] &  0x0F];
	}
}

static void php_hash_do_hash(INTERNAL_FUNCTION_PARAMETERS, int isfilename)
{
	char *algo, *data, *digest;
	int algo_len, data_len;
	zend_bool raw_output = 0;
	php_hash_ops *ops;
	void *context;
	php_stream *stream = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|b", &algo, &algo_len, &data, &data_len, &raw_output) == FAILURE) {
		return;
	}

	ops = php_hash_fetch_ops(algo, algo_len);
	if (!ops) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown hashing algorithm: %s", algo);
		RETURN_FALSE;
	}
	if (isfilename) {
		stream = php_stream_open_wrapper_ex(data, "rb", REPORT_ERRORS | ENFORCE_SAFE_MODE, NULL, FG(default_context));
		if (!stream) {
			/* Stream will report errors opening file */
			RETURN_FALSE;
		}
	}

	context = emalloc(ops->context_size);
	ops->hash_init(context);

	if (isfilename) {
		char buf[1024];
		int n;

		while ((n = php_stream_read(stream, buf, sizeof(buf))) > 0) {
			ops->hash_update(context, buf, n);
		}
		php_stream_close(stream);
	} else {
		ops->hash_update(context, data, data_len);
	}

	digest = emalloc(ops->digest_size + 1);
	ops->hash_final(digest, context);
	efree(context);

	if (raw_output) {
		digest[ops->digest_size] = 0;
		RETURN_STRINGL(digest, ops->digest_size, 0);
	} else {
		char *hex_digest = emalloc((ops->digest_size * 2) + 1);

		php_hash_bin2hex(hex_digest, digest, ops->digest_size);
		hex_digest[2 * ops->digest_size] = 0;
		efree(digest);
		RETURN_STRINGL(hex_digest, 2 * ops->digest_size, 0);
	}
}

/* {{{ proto string hash(string algo, string data[, bool raw_output = false])
Generate a hash of a given input string
Returns lowercase hexits by default */
PHP_FUNCTION(hash) {
	php_hash_do_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string hash_file(string algo, string filename[, bool raw_output = false])
Generate a hash of a given file
Returns lowercase hexits by default */
PHP_FUNCTION(hash_file) {
	php_hash_do_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto resource hash_init(string algo)
Initialize a hashing context */
PHP_FUNCTION(hash_init)
{
	char *algo;
	int algo_len;
	void *context;
	php_hash_ops *ops;
	php_hash_data *hash;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &algo, &algo_len) == FAILURE) {
		return;
	}

	ops = php_hash_fetch_ops(algo, algo_len);
	if (!ops) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown hashing algorithm: %s", algo);
		RETURN_FALSE;
	}

	context = emalloc(ops->context_size);
	ops->hash_init(context);

	hash = emalloc(sizeof(php_hash_data));
	hash->ops = ops;
	hash->context = context;

	ZEND_REGISTER_RESOURCE(return_value, hash, php_hash_le_hash);
}
/* }}} */

/* {{{ proto bool hash_update(resource context, string data)
Pump data into the hashing algorithm */
PHP_FUNCTION(hash_update)
{
	zval *zhash;
	php_hash_data *hash;
	char *data;
	int data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zhash, &data, &data_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(hash, php_hash_data*, &zhash, -1, PHP_HASH_RESNAME, php_hash_le_hash);

	hash->ops->hash_update(hash->context, data, data_len);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hash_final(resource context[, bool raw_output=false])
Output resulting digest */
PHP_FUNCTION(hash_final)
{
	zval *zhash;
	php_hash_data *hash;
	zend_bool raw_output = 0;
	zend_rsrc_list_entry *le;
	char *digest;
	int digest_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|b", &zhash, &raw_output) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(hash, php_hash_data*, &zhash, -1, PHP_HASH_RESNAME, php_hash_le_hash);

	digest_len = hash->ops->digest_size;
	digest = emalloc(digest_len + 1);
	hash->ops->hash_final(digest, hash->context);
	digest[digest_len] = 0;

	/* zend_list_REAL_delete() */
	if (zend_hash_index_find(&EG(regular_list), Z_RESVAL_P(zhash), (void **) &le)==SUCCESS) {
		/* This is a hack to avoid letting the resource hide elsewhere (like in separated vars)
			FETCH_RESOURCE is intelligent enough to handle dealing with any issues this causes */
		le->refcount = 1;
	} /* FAILURE is not an option */
	zend_list_delete(Z_RESVAL_P(zhash));

	if (raw_output) {
		RETURN_STRINGL(digest, digest_len, 0);
	} else {
		char *hex_digest = emalloc((digest_len * 2) + 1);

		php_hash_bin2hex(hex_digest, digest, digest_len);
		hex_digest[2 * digest_len] = 0;
		efree(digest);
		RETURN_STRINGL(hex_digest, 2 * digest_len, 0);		
	}
}
/* }}} */

/* Module Housekeeping */

static void php_hash_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_hash_data *hash = (php_hash_data*)rsrc->ptr;
	char *dummy = emalloc(hash->ops->digest_size);

	/* Just in case the algo has internally allocated resources */
	hash->ops->hash_final(dummy, hash->context);
	efree(dummy);

	efree(hash->context);
	efree(hash);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(hash)
{
	php_hash_le_hash = zend_register_list_destructors_ex(php_hash_dtor, NULL, PHP_HASH_RESNAME, module_number);

	zend_hash_init(&php_hash_hashtable, 8, NULL, NULL, 1);

	php_hash_register_algo("md5",			&php_hash_md5_ops);
	php_hash_register_algo("sha1",			&php_hash_sha1_ops);
	php_hash_register_algo("sha256",		&php_hash_sha256_ops);
	php_hash_register_algo("sha384",		&php_hash_sha384_ops);
	php_hash_register_algo("sha512",		&php_hash_sha512_ops);
	php_hash_register_algo("ripemd128",		&php_hash_ripemd128_ops);
	php_hash_register_algo("ripemd160",		&php_hash_ripemd160_ops);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(hash)
{
	zend_hash_destroy(&php_hash_hashtable);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(hash)
{
	char buffer[2048];
	char *s = buffer, *e = s + sizeof(buffer), *str;
	long idx, type;

	for(zend_hash_internal_pointer_reset(&php_hash_hashtable);
		(type = zend_hash_get_current_key(&php_hash_hashtable, &str, &idx, 0)) != HASH_KEY_NON_EXISTANT;
		zend_hash_move_forward(&php_hash_hashtable)) {
		s += snprintf(s, e - s, "%s, ", str);
	}

	while (s > buffer && *(--s) == ' ');
	*s = 0;

	php_info_print_table_start();
	php_info_print_table_header(2, "hash support", "enabled");
	php_info_print_table_header(2, "Hashing Engines", buffer);
	php_info_print_table_end();
}
/* }}} */

/* {{{ hash_functions[]
 */
function_entry hash_functions[] = {
	PHP_FE(hash,									NULL)
	PHP_FE(hash_file,								NULL)

	PHP_FE(hash_init,								NULL)
	PHP_FE(hash_update,								NULL)
	PHP_FE(hash_final,								NULL)

	/* BC Land */
#ifdef PHP_HASH_MD5_NOT_IN_CORE
	PHP_NAMED_FE(md5, php_if_md5,					NULL)
	PHP_NAMED_FE(md5_file, php_if_md5_file,			NULL)
#endif /* PHP_HASH_MD5_NOT_IN_CORE */

	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ hash_module_entry
 */
zend_module_entry hash_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	PHP_HASH_EXTNAME,
	hash_functions,
	PHP_MINIT(hash),
	PHP_MSHUTDOWN(hash),
	NULL, /* RINIT */
	NULL, /* RSHUTDOWN */
	PHP_MINFO(hash),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_HASH_EXTVER, /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_HASH
ZEND_GET_MODULE(hash)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
