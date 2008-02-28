/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Nikos Mavroyanopoulos <nmav@hellug.gr> (HMAC, KEYGEN)       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_LIBMHASH

#include "fcntl.h"
#include "php_mhash.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"

const zend_function_entry mhash_functions[] = {
	PHP_FE(mhash_count, NULL)
	PHP_FE(mhash_get_block_size, NULL)
	PHP_FE(mhash_get_hash_name, NULL)
	PHP_FE(mhash_keygen_count, NULL)
	PHP_FE(mhash_get_keygen_name, NULL)
	PHP_FE(mhash_keygen_uses_hash, NULL)
	PHP_FE(mhash_keygen_uses_salt, NULL)
	PHP_FE(mhash_get_keygen_salt_size, NULL)
	PHP_FE(mhash_keygen_uses_count, NULL)
	PHP_FE(mhash, NULL)
	PHP_FE(mhash_keygen, NULL)
	PHP_FE(mhash_keygen_s2k, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry mhash_module_entry = {
	STANDARD_MODULE_HEADER,
	"mhash",
	mhash_functions,
	PHP_MINIT(mhash), NULL,
	NULL, NULL,
	PHP_MINFO(mhash),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES,
};

#ifdef COMPILE_DL_MHASH
ZEND_GET_MODULE(mhash)
#endif

#define NO_ARGS() (SUCCESS == zend_parse_parameters_none())

#ifndef HAVE_MHASH_GET_HASH_NAME_STATIC_PROTO
extern const char *mhash_get_hash_name_static(hashid hash);
#endif
#ifndef HAVE_MHASH_GET_KEYGEN_NAME_STATIC_PROTO
extern const char *mhash_get_keygen_name_static(hashid type);
#endif

/* {{{ int php_mhash */
int php_mhash(hashid hash, const char *input_str, int input_len, const char *key_str, int key_len, char **enc, int *len TSRMLS_DC)
{
	size_t pbsize;
	char *result;
	MHASH mh;
	
	if (key_len) {
		if (!(pbsize = mhash_get_hash_pblock(hash))) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "HMAC incompatible hash algorithm");
			return FAILURE;
		}
		mh = mhash_hmac_init(hash, (char *) key_str, key_len, pbsize);
	} else {
		mh = mhash_init(hash);
	}

	if (mh == MHASH_FAILED) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "mhash initialization failed");
		return FAILURE;
	}

	mhash(mh, input_str, input_len);

	if (key_len) {
		result = mhash_hmac_end(mh);
	} else {
		result = mhash_end(mh);
	}
	
	if (!result) {
		return FAILURE;
	}
	
	*len = mhash_get_block_size(hash);
	*enc = estrndup(result, *len);
	
	mhash_free(result);
	
	return SUCCESS;
}
/* }}} */

/* {{{ int php_mhash_keygen */
int php_mhash_keygen(keygenid type, hashid hash1, hashid hash2, const char *pass_str, int pass_len, const char *salt_str, size_t salt_len, char **key, int *len, int max_len, int max_count TSRMLS_DC)
{
	KEYGEN keygen;
	
	if (type < 0 || type > mhash_keygen_count()) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unknown keygen type %d", type);
		return FAILURE;
	}
	
	memset(&keygen, 0, sizeof(keygen));
	
	if (mhash_keygen_uses_hash_algorithm(type)) {
		if (hash1 == -1) {
			hash1 = hash2;
		}
		if (hash2 == -1) {
			hash2 = hash1;
		}
		keygen.hash_algorithm[0] = hash1;
		keygen.hash_algorithm[1] = hash2;
	}
	
	if (mhash_keygen_uses_salt(type)) {
		if (salt_len <= 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s requires a salt", mhash_get_keygen_name_static(type));
			return FAILURE;
		}
		keygen.salt = (void *) salt_str;
		keygen.salt_size = salt_len;
	}
	
	keygen.count = max_count;
	
	if (max_len > 0) {
		*len = max_len;
	} else {
		*len = 128;
	}
	
	*key = safe_emalloc(1, *len, 1);
	
	if (mhash_keygen_ext(type, keygen, *key, *len, (void *) pass_str, pass_len) < 0) {
		efree(*key);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "key generation failed");
		return FAILURE;
	}
	
	(*key)[*len] = '\0';
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINIT */
PHP_MINIT_FUNCTION(mhash)
{
	int i, n, l;
	const char *name;
	char buf[128];

	for (i = 0, n = mhash_count() + 1; i < n; ++i) {
		if ((name = (const char *) mhash_get_hash_name_static(i))) {
			l = snprintf(buf, sizeof(buf), "MHASH_%s", name);
			zend_register_long_constant(buf, l + 1, i, CONST_PERSISTENT, module_number TSRMLS_CC);
		}
	}
	for (i = 0, n = mhash_keygen_count() + 1; i < n; ++i) {
		if ((name = (const char *) mhash_get_keygen_name_static(i))) {
			l = snprintf(buf, sizeof(buf), "MHASH_KEYGEN_%s", name);
			zend_register_long_constant(buf, l + 1, i, CONST_PERSISTENT, module_number TSRMLS_CC);
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO */
PHP_MINFO_FUNCTION(mhash)
{
	char version[32];
	
	snprintf(version, sizeof(version), "%d", MHASH_API_VERSION);
	
	php_info_print_table_start();
	php_info_print_table_row(2, "MHASH support", "Enabled");
	php_info_print_table_row(2, "MHASH API Version", version);
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto int mhash_count(void) U
   Gets the number of available hashes */
PHP_FUNCTION(mhash_count)
{
	if (!NO_ARGS()) {
		return;
	}

	RETURN_LONG(mhash_count());
}

/* }}} */

/* {{{ proto int mhash_get_block_size(int hash) U
   Gets the block size of hash */
PHP_FUNCTION(mhash_get_block_size)
{
	long hash;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &hash)) {
		return;
	}

	RETURN_LONG(mhash_get_block_size(hash));
}

/* }}} */

/* {{{ proto string mhash_get_hash_name(int hash) U
   Gets the name of hash */
PHP_FUNCTION(mhash_get_hash_name)
{
	const char *name;
	long hash;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &hash)) {
		return;
	}

	if ((name = (const char *) mhash_get_hash_name_static(hash))) {
		RETVAL_ASCII_STRING((char *) name, 1);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto int mhash_keygen_count() U
	Get the number of available keygen algorithms */
PHP_FUNCTION(mhash_keygen_count)
{
	if (!NO_ARGS()) {
		return;
	}
	
	RETURN_LONG(mhash_keygen_count());
}
/* }}} */

/* {{{ proto string mhash_get_keygen_name(int keygen) U
	Get the name of the keygen algorithm */
PHP_FUNCTION(mhash_get_keygen_name)
{
	const char *name;
	long keygen;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &keygen)) {
		return;
	}
	
	if ((name = (const char *) mhash_get_keygen_name_static(keygen))) {
		RETVAL_ASCII_STRING((char *) name, 1);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto bool mhash_keygen_uses_hash(int keygen) U
	Whether the keygen algorithm uses a hash algorithm */
PHP_FUNCTION(mhash_keygen_uses_hash)
{
	long keygen;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &keygen)) {
		return;
	}
	
	RETURN_BOOL(mhash_keygen_uses_hash_algorithm(keygen));
}
/* }}} */

/* {{{ proto bool mhash_keygen_uses_count(int keygen) U
	Whether the keygen algorithm uses the bytes_count parameter */
PHP_FUNCTION(mhash_keygen_uses_count)
{
	long keygen;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &keygen)) {
		return;
	}
	
	RETURN_BOOL(mhash_keygen_uses_count(keygen));
}
/* }}} */

/* {{{ proto bool mhash_keygen_uses_salt(int keygen) U
	Whether the keygen algorithm requires a salt */
PHP_FUNCTION(mhash_keygen_uses_salt)
{
	long keygen;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &keygen)) {
		return;
	}
	
	RETURN_BOOL(mhash_keygen_uses_salt(keygen));
}
/* }}} */

/* {{{ proto bool mhash_get_keygen_salt_size(int keygen) U
	Get the required size of the salt for the keygen algorithm */
PHP_FUNCTION(mhash_get_keygen_salt_size)
{
	long keygen;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &keygen)) {
		return;
	}
	
	RETURN_LONG(mhash_get_keygen_salt_size(keygen));
}
/* }}} */

/* {{{ proto binary mhash(int hash, binary data [, binary key]) U
   Hash data with hash */
PHP_FUNCTION(mhash)
{
	long hash;
	char *result, *data, *key = NULL;
	int result_len, data_len, key_len = 0;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lS|S", &hash, &data, &data_len, &key, &key_len)) {
		return;
	}
	if (SUCCESS != php_mhash(hash, data, data_len, key, key_len, &result, &result_len TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(result, result_len, 0);
}

/* }}} */

/* {{{ proto binary mhash_keygen(int type, int hash1, int hash2, binary password[, binary salt[, int max_key_size = 128[, int bytes_count = 0]]) U
	Generate a key */
PHP_FUNCTION(mhash_keygen)
{
	long hash1, hash2, type, max_len = 0, bytes_count = 0;
	char *result_str, *pass_str, *salt_str = NULL;
	int result_len, pass_len, salt_len = 0;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lllS|Sll", &type, &hash1, &hash2, &pass_str, &pass_len, &salt_str, &salt_len, &max_len, &bytes_count)) {
		return;
	}
	if (SUCCESS != php_mhash_keygen(type, hash1, hash2, pass_str, pass_len, salt_str, salt_len, &result_str, &result_len, max_len, bytes_count TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(result_str, result_len, 0);
}
/* }}} */

/* {{{ proto binary mhash_keygen_s2k(int hash, binary input_password, binary salt, int bytes)
   Generates a key using hash functions */
PHP_FUNCTION(mhash_keygen_s2k)
{
	long hash, max_len = 0;
	char *result_str, *pass_str, *salt_str = NULL;
	int result_len, pass_len, salt_len = 0;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lSSl", &hash, &pass_str, &pass_len, &salt_str, &salt_len, &max_len)) {
		return;
	}
	if (SUCCESS != php_mhash_keygen(KEYGEN_S2K_SALTED, hash, hash, pass_str, pass_len, salt_str, salt_len, &result_str, &result_len, max_len, 0 TSRMLS_CC)) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(result_str, result_len, 0);
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
