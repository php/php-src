/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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

#define MHASH_FAILED_MSG "mhash initialization failed"
#define MHASH_KEYGEN_FAILED_MSG "mhash key generation failed"

static PHP_MINIT_FUNCTION(mhash)
{
	int i;
	char *name;
	char buf[128];

	for (i = 0; i <= mhash_count(); i++) {
		name = mhash_get_hash_name(i);
		if (name) {
			snprintf(buf, 127, "MHASH_%s", name);
			zend_register_long_constant(buf, strlen(buf) + 1,
						    i, CONST_PERSISTENT,
						    module_number TSRMLS_CC);
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
	pval **hash;

	if (ZEND_NUM_ARGS() != 1
	    || zend_get_parameters_ex(1, &hash) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(hash);

	RETURN_LONG(mhash_get_block_size(Z_LVAL_PP(hash)));
}

/* }}} */

/* {{{ proto string mhash_get_hash_name(int hash)
   Gets the name of hash */
PHP_FUNCTION(mhash_get_hash_name)
{
	pval **hash;
	char *name;

	if (ZEND_NUM_ARGS() != 1
	    || zend_get_parameters_ex(1, &hash) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(hash);

	name = mhash_get_hash_name(Z_LVAL_PP(hash));
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
	pval **hash, **data, **key;
	MHASH td;
	int bsize;
	unsigned char *hash_data;
	int num_args;

	num_args = ZEND_NUM_ARGS();

	if (num_args < 2 || num_args > 3) {
		WRONG_PARAM_COUNT;
	}
	if (num_args == 2) {	/* 2 arguments, just hash */
		if (zend_get_parameters_ex(2, &hash, &data) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else {		/* 3 arguments, do HMAC hash (keyed hash) */
		if (zend_get_parameters_ex(3, &hash, &data, &key) ==
		    FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(key);
	}

	convert_to_long_ex(hash);
	convert_to_string_ex(data);

	bsize = mhash_get_block_size(Z_LVAL_PP(hash));

	if (num_args == 3) {
		if (mhash_get_hash_pblock(Z_LVAL_PP(hash)) == 0) {
			php_error(E_WARNING, MHASH_FAILED_MSG);
			RETURN_FALSE;
		}
		td = mhash_hmac_init(Z_LVAL_PP(hash),
				    Z_STRVAL_PP(key),
				    Z_STRLEN_PP(key),
				    mhash_get_hash_pblock(Z_LVAL_PP(hash)));
	} else {
		td = mhash_init(Z_LVAL_PP(hash));
	}
	if (td == MHASH_FAILED) {
		php_error(E_WARNING, MHASH_FAILED_MSG);
		RETURN_FALSE;
	}

	mhash(td, Z_STRVAL_PP(data), Z_STRLEN_PP(data));

	if (num_args == 3) {
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
/* SALTED S2K uses a fixed salt */
#define SALT_SIZE 8
PHP_FUNCTION(mhash_keygen_s2k)
{
	pval **hash, **input_password, **bytes, **input_salt;
	int password_len, salt_len;
	int hashid, size=0, val;
	KEYGEN keystruct;
	char salt[SALT_SIZE], *ret;
	char* password, error[128];
	
	if (ZEND_NUM_ARGS() != 4) {
		WRONG_PARAM_COUNT;
	}
	if (zend_get_parameters_ex(4, &hash, &input_password, &input_salt, &bytes) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(hash);
	convert_to_string_ex(input_password);
	convert_to_string_ex(input_salt);
	convert_to_long_ex(bytes);

	password = Z_STRVAL_PP(input_password);
	password_len = Z_STRLEN_PP(input_password);

	salt_len = MIN(Z_STRLEN_PP(input_salt), SALT_SIZE);

	if (salt_len > mhash_get_keygen_salt_size(KEYGEN_S2K_SALTED)) {
		sprintf( error, "The specified salt [%d] is more bytes than the required by the algorithm [%d]\n", salt_len, mhash_get_keygen_salt_size(KEYGEN_S2K_SALTED));

		php_error(E_WARNING, error);
	}

	memcpy(salt, Z_STRVAL_PP(input_salt), salt_len);
	if (salt_len < SALT_SIZE)
		memset(salt + salt_len, 0, SALT_SIZE - salt_len);
	salt_len=SALT_SIZE;
	
/*	if (salt_len==0) {
 *		php_error(E_WARNING, "Not using salt is really not recommended);
 *	}
 */
	
	hashid = Z_LVAL_PP(hash);
	size = Z_LVAL_PP(bytes);

	keystruct.hash_algorithm[0]=hashid;
	keystruct.hash_algorithm[1]=hashid;
	keystruct.count=0;
	keystruct.salt = salt;
	keystruct.salt_size = salt_len;

	ret = emalloc(size);
	if (ret==NULL) {
		php_error(E_WARNING, MHASH_KEYGEN_FAILED_MSG);
		RETURN_FALSE;
	}

	val = mhash_keygen_ext( KEYGEN_S2K_SALTED, keystruct, ret, size, password, password_len);
	if (val >= 0) {
		RETVAL_STRINGL(ret, size, 0);
	} else {
		php_error(E_WARNING, MHASH_KEYGEN_FAILED_MSG);
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
