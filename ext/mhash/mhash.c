/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_LIBMHASH

#include "fcntl.h"
#include "php_mhash.h"
#include "mhash.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"

function_entry mhash_functions[] = {
	PHP_FE(mhash_get_block_size, NULL)
	PHP_FE(mhash_get_hash_name, NULL)
	PHP_FE(mhash_keygen_s2k, NULL)
	PHP_FE(mhash_count, NULL)
	PHP_FE(mhash, NULL)
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

/* SALTED S2K uses a fixed salt */
#define SALT_SIZE 8

PHP_MINIT_FUNCTION(mhash)
{
	int i, n, l;
	char *name;
	char buf[128];

	n = mhash_count() + 1;

	for (i=0; i<n; i++) {
		if ((name = mhash_get_hash_name(i))) {
			l = snprintf(buf, 127, "MHASH_%s", name);
			zend_register_long_constant(buf, l + 1, i, CONST_PERSISTENT, module_number TSRMLS_CC);
			free(name);
		}
	}

	return SUCCESS;
}

PHP_MINFO_FUNCTION(mhash)
{
	char version[32];
	
	sprintf(version,"%d", MHASH_API_VERSION);
	
	php_info_print_table_start();
	php_info_print_table_row(2, "MHASH support", "Enabled");
	php_info_print_table_row(2, "MHASH API Version", version);
	php_info_print_table_end();
}

/* {{{ proto int mhash_count(void)
   Gets the number of available hashes */
PHP_FUNCTION(mhash_count)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG(mhash_count());
}

/* }}} */

/* {{{ proto int mhash_get_block_size(int hash)
   Gets the block size of hash */
PHP_FUNCTION(mhash_get_block_size)
{
	long hash;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &hash) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG(mhash_get_block_size(hash));
}

/* }}} */

/* {{{ proto string mhash_get_hash_name(int hash)
   Gets the name of hash */
PHP_FUNCTION(mhash_get_hash_name)
{
	char *name;
	long hash;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &hash) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	name = mhash_get_hash_name(hash);
	if (name) {
		RETVAL_STRING(name, 1);
		free(name);
	} else {
		RETVAL_FALSE;
	}
}

/* }}} */

/* {{{ proto string mhash(int hash, string data [, string key])
   Hash data with hash */
PHP_FUNCTION(mhash)
{
	MHASH td;
	int bsize;
	unsigned char *hash_data;
	long hash;
	int data_len, key_len=0;
	char *data, *key=NULL;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls|s", &hash, &data, &data_len, &key, &key_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	bsize = mhash_get_block_size(hash);
	
	if (key_len) {
		if (mhash_get_hash_pblock(hash) == 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "mhash initialization failed");
			RETURN_FALSE;
		}
		td = mhash_hmac_init(hash, key, key_len, mhash_get_hash_pblock(hash));
	} else {
		td = mhash_init(hash);
	}

	if (td == MHASH_FAILED) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "mhash initialization failed");
		RETURN_FALSE;
	}

	mhash(td, data, data_len);

	if (key_len) {
		hash_data = (unsigned char *) mhash_hmac_end(td);
	} else {
		hash_data = (unsigned char *) mhash_end(td);
	}
	
	if (hash_data) {
		RETVAL_STRINGL(hash_data, bsize, 1);
		mhash_free(hash_data);
	} else {
		RETURN_FALSE;
	}
}

/* }}} */

/* {{{ proto string mhash_keygen_s2k(int hash, string input_password, string salt, int bytes)
   Generates a key using hash functions */
PHP_FUNCTION(mhash_keygen_s2k)
{
	KEYGEN keystruct;
	char salt[SALT_SIZE], *ret;
	long hash, bytes;
	char *password, *in_salt;
	int password_len, salt_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lssl", &hash, &password, &password_len, &in_salt, &salt_len, &bytes) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (bytes <= 0){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "the byte parameter must be greater then 0");
		RETURN_FALSE;
	}
	
	salt_len = MIN(salt_len, SALT_SIZE);

	if (salt_len > mhash_get_keygen_salt_size(KEYGEN_S2K_SALTED)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, 
			"The specified salt [%d] is more bytes than the required by the algorithm [%d]\n", 
			salt_len, mhash_get_keygen_salt_size(KEYGEN_S2K_SALTED));
	}

	memcpy(salt, in_salt, salt_len);
	if (salt_len < SALT_SIZE) {
		memset(salt + salt_len, 0, SALT_SIZE - salt_len);
	}	
	salt_len = SALT_SIZE;
	
	keystruct.hash_algorithm[0] = hash;
	keystruct.hash_algorithm[1] = hash;
	keystruct.count = 0;
	keystruct.salt = salt;
	keystruct.salt_size = salt_len;

	ret = safe_emalloc(1, bytes, 1);

	if (mhash_keygen_ext(KEYGEN_S2K_SALTED, keystruct, ret, bytes, password, password_len) >= 0) {
		ret[bytes] = '\0';
		RETVAL_STRINGL(ret, bytes, 0);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "mhash key generation failed");
		efree(ret);
		RETURN_FALSE;
	}
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
