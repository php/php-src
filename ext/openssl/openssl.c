/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Venaas <venaas@php.net>                                |
   +----------------------------------------------------------------------+
 */
 
/* $Id$ */

#include "php.h"
#include "php_openssl.h"

/* PHP Includes */
#include "ext/standard/file.h"
#include "ext/standard/info.h"

/* OpenSSL includes */
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/pem.h>

static unsigned char arg2of3_force_ref[] =
                       { 3, BYREF_NONE, BYREF_FORCE, BYREF_NONE };
static unsigned char arg2of4_force_ref[] =
                       { 4, BYREF_NONE, BYREF_FORCE, BYREF_NONE, BYREF_NONE };
static unsigned char arg2and3of4_force_ref[] =
                       { 4, BYREF_NONE, BYREF_FORCE, BYREF_FORCE, BYREF_NONE };


function_entry openssl_functions[] = {
	PHP_FE(openssl_get_privatekey,     NULL)
	PHP_FE(openssl_get_publickey,      NULL)
	PHP_FE(openssl_free_key,           NULL)
#if 0
	PHP_FE(openssl_read_publickey,     NULL)
	PHP_FE(openssl_read_x509,          NULL)
	PHP_FE(openssl_free_x509,          NULL)
#endif
	PHP_FE(openssl_sign,               arg2of3_force_ref)
	PHP_FE(openssl_verify,             NULL)
	PHP_FE(openssl_seal,               arg2and3of4_force_ref)
	PHP_FE(openssl_open,               arg2of4_force_ref)
	{NULL, NULL, NULL}
};

zend_module_entry openssl_module_entry = {
	"openssl",
	openssl_functions,
	PHP_MINIT(openssl),
	NULL,
	NULL,	
	NULL,
	PHP_MINFO(openssl),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_OPENSSL
ZEND_GET_MODULE(openssl)
#endif

static void _php_pkey_free(zend_rsrc_list_entry *rsrc);
static int le_key;

#if 0
static void _php_x509_free(zend_rsrc_list_entry *rsrc);
static int le_x509;
#endif

PHP_MINIT_FUNCTION(openssl)
{
	le_key = zend_register_list_destructors_ex(_php_pkey_free, NULL,
						   "OpenSSL key",
						   module_number);
#if 0
	le_x509 = zend_register_list_destructors_ex(_php_x509_free, NULL,
						    "OpenSSL X.509",
						    module_number);
#endif
	OpenSSL_add_all_ciphers();
	return SUCCESS;
}

PHP_MINFO_FUNCTION(openssl)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "OpenSSL support", "enabled");
	php_info_print_table_row(2, "OpenSSL Version", OPENSSL_VERSION_TEXT);
	php_info_print_table_end();
}

PHP_MSHUTDOWN_FUNCTION(openssl)
{
	EVP_cleanup();
	return SUCCESS;
}

/* {{{ proto int openssl_get_privatekey(string key)
    Get private key */
PHP_FUNCTION(openssl_get_privatekey)
{
	zval **key;
	BIO *b;
	EVP_PKEY *pkey;
	
	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &key) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(key);
	
	b = BIO_new_mem_buf(Z_STRVAL_PP(key), Z_STRLEN_PP(key));
        if (b == NULL) {
		RETURN_FALSE;
	}

        pkey = (EVP_PKEY *) PEM_ASN1_read_bio((char *(*)())d2i_PrivateKey,
					      PEM_STRING_EVP_PKEY, b,
					      NULL, NULL, NULL);
	BIO_free(b);

	if (pkey == NULL) { 
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, pkey, le_key);
}
/* }}} */

/* {{{ proto int openssl_get_publickey(string cert)
   Get public key from X.509 certificate */
PHP_FUNCTION(openssl_get_publickey)
{
	zval **cert;
	X509 *x509;
        BIO *b;
	EVP_PKEY *pkey;

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &cert) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(cert);

	b = BIO_new_mem_buf(Z_STRVAL_PP(cert), -1);
        if (b == NULL) {
		RETURN_FALSE;
	}

        x509 = (X509 *) PEM_ASN1_read_bio((char *(*)())d2i_X509,
					  PEM_STRING_X509, b,
					  NULL, NULL, NULL);
	BIO_free(b);

	if (x509 == NULL) { 
		RETURN_FALSE;
	}

	pkey = (EVP_PKEY *) X509_get_pubkey(x509);
	X509_free(x509);

	if (pkey == NULL) { 
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, pkey, le_key);
}
/* }}} */

/* {{{ proto void openssl_free_key(int key)
   Free key */
PHP_FUNCTION(openssl_free_key)
{
	zval **key;
	EVP_PKEY *pkey;

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &key) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(pkey, EVP_PKEY *, key, -1, "OpenSSL key", le_key);
	zend_list_delete(Z_LVAL_PP(key));
}
/* }}} */

#if 0
/* {{{ proto int openssl_read_publickey(int x509)
   Read public key */
PHP_FUNCTION(openssl_read_publickey)
{
	zval **x509;
	X509 *cert;
	EVP_PKEY *pkey;

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &x509) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(cert, X509 *, x509, -1, "OpenSSL X.509", le_x509);

	pkey = (EVP_PKEY *) X509_get_pubkey(cert);
	if (pkey == NULL) { 
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, pkey, le_key);
}
/* }}} */

/* {{{ proto int openssl_read_x509(string cert)
   Read X.509 certificate */
PHP_FUNCTION(openssl_read_x509)
{
	zval **cert;
	X509 *x509;
        BIO *b;

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &cert) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(cert);

	b = BIO_new_mem_buf(Z_STRVAL_PP(cert), -1);
        if (b == NULL) {
		RETURN_FALSE;
	}

        x509 = (X509 *) PEM_ASN1_read_bio((char *(*)())d2i_X509,
					  PEM_STRING_X509, b,
					  NULL, NULL, NULL);
	BIO_free(b);

	if (x509 == NULL) { 
		RETURN_FALSE;
	}
	
	ZEND_REGISTER_RESOURCE(return_value, x509, le_x509);
}
/* }}} */

/* {{{ proto void openssl_free_x509(int x509)
   Free X.509 certificate */
PHP_FUNCTION(openssl_free_x509)
{
	zval **x509;
	X509 *cert;

	if (ZEND_NUM_ARGS() != 1 ||
	    zend_get_parameters_ex(1, &x509) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	ZEND_FETCH_RESOURCE(cert, X509 *, x509, -1, "OpenSSL X.509", le_x509);
	zend_list_delete(Z_LVAL_PP(x509));
}
/* }}} */
#endif

/* {{{ proto bool openssl_sign(string data, string signature, int key)
   Sign data */
PHP_FUNCTION(openssl_sign)
{
	zval **key, **data, **signature;
	EVP_PKEY *pkey;
	int siglen;
	unsigned char *sigbuf;
	EVP_MD_CTX md_ctx;

	if (ZEND_NUM_ARGS() != 3 ||
	    zend_get_parameters_ex(3, &data, &signature, &key) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(data);
	
	ZEND_FETCH_RESOURCE(pkey, EVP_PKEY *, key, -1, "OpenSSL key", le_key);
	siglen = EVP_PKEY_size(pkey);
	sigbuf = emalloc(siglen + 1);
	if (sigbuf == NULL) {
		RETURN_FALSE;
	}

	EVP_SignInit(&md_ctx, EVP_sha1());
	EVP_SignUpdate(&md_ctx, Z_STRVAL_PP(data), Z_STRLEN_PP(data));
	if (EVP_SignFinal (&md_ctx, sigbuf, &siglen, pkey)) {
		zval_dtor(*signature);
		sigbuf[siglen] = '\0';
		ZVAL_STRINGL(*signature, sigbuf, siglen, 0);
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
		efree(sigbuf);
	}
}
/* }}} */

/* {{{ proto int openssl_verify(string data, string signature, int key)
   Verify data */
PHP_FUNCTION(openssl_verify)
{
	zval **key, **data, **signature;
	EVP_PKEY *pkey;
	int err;
	EVP_MD_CTX     md_ctx;

	if (ZEND_NUM_ARGS() != 3 ||
        zend_get_parameters_ex(3, &data, &signature, &key) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(data);
	convert_to_string_ex(signature);

	ZEND_FETCH_RESOURCE(pkey, EVP_PKEY *, key, -1, "OpenSSL key", le_key);

	EVP_VerifyInit   (&md_ctx, EVP_sha1());
	EVP_VerifyUpdate (&md_ctx, Z_STRVAL_PP(data), Z_STRLEN_PP(data));
	err = EVP_VerifyFinal (&md_ctx, Z_STRVAL_PP(signature),
			       Z_STRLEN_PP(signature), pkey);
	RETURN_LONG(err);
}
/* }}} */

/* {{{ proto int openssl_seal(string data, string sealdata, array ekeys,
                              array pubkeys)
   Seal data */
PHP_FUNCTION(openssl_seal)
{
	zval **pubkeys, **pubkey, **data, **sealdata, **ekeys;
	HashTable *pubkeysht;
        HashPosition pos;
	EVP_PKEY **pkeys;
	int i, len1, len2, *eksl, nkeys;
	unsigned char *buf, **eks;

	EVP_CIPHER_CTX ctx;

	if (ZEND_NUM_ARGS() != 4 ||
		zend_get_parameters_ex(4, &data, &sealdata, &ekeys,
				       &pubkeys) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	SEPARATE_ZVAL(pubkeys);
        pubkeysht = HASH_OF(*pubkeys);
	nkeys = pubkeysht ? zend_hash_num_elements(pubkeysht) : 0;
	if (!nkeys) {
                php_error(E_WARNING,
		 "Fifth argument to openssl_seal() must be a non-empty array");
                RETURN_FALSE;
	}

	pkeys = emalloc(nkeys * sizeof(*pkeys));
	if (pkeys == NULL) {
		RETURN_FALSE;
	}
	eksl = emalloc(nkeys * sizeof(*eksl));
	if (eksl == NULL) {
		efree(pkeys);
		RETURN_FALSE;
	}
	eks = emalloc(nkeys * sizeof(*eks));
	if (eks == NULL) {
		efree(eksl);
		efree(pkeys);
		RETURN_FALSE;
	}
	
	convert_to_string_ex(data);

        zend_hash_internal_pointer_reset_ex(pubkeysht, &pos);
	i = 0;
        while (zend_hash_get_current_data_ex(pubkeysht, (void **) &pubkey,
					     &pos) == SUCCESS) {
		ZEND_FETCH_RESOURCE(pkeys[i], EVP_PKEY *, pubkey, -1,
				    "OpenSSL key", le_key);
		eks[i] = emalloc(EVP_PKEY_size(pkeys[i]) + 1);
		if (eks[i] == NULL) {
			while (i--) {
				efree(eks[i]);
			}
			efree(eks);
			efree(eksl);
			efree(pkeys);
			RETURN_FALSE;
		}
                zend_hash_move_forward_ex(pubkeysht, &pos);
		i++;
	}

	if (!EVP_EncryptInit(&ctx,EVP_rc4(),NULL,NULL)) {
		for (i=0; i<nkeys; i++) {
			efree(eks[i]);
		}
		efree(eks);
		efree(eksl);
		efree(pkeys);
		RETURN_FALSE;
	}

#if 0
	/* Need this if allow ciphers that require initialization vector */
	ivlen = EVP_CIPHER_CTX_iv_length(&ctx);
	if (ivlen) {
		iv = emalloc(ivlen + 1);
		if (iv == NULL) {
			for (i=0; i<nkeys; i++) {
				efree(eks[i]);
			}
			efree(eks);
			efree(eksl);
			efree(pkeys);
			RETURN_FALSE;
		}
	} else {
		iv = NULL;
	}
#endif
	/* allocate one byte extra to make room for \0 */
	buf = emalloc(Z_STRLEN_PP(data) + EVP_CIPHER_CTX_block_size(&ctx));
	if (buf == NULL) {
		for (i=0; i<nkeys; i++) {
			efree(eks[i]);
		}
		efree(eks);
		efree(eksl);
		efree(pkeys);
		RETURN_FALSE;
	}

	if (!EVP_SealInit(&ctx, EVP_rc4(), eks, eksl, NULL, pkeys, nkeys) ||
	    !EVP_SealUpdate(&ctx, buf, &len1, Z_STRVAL_PP(data),
			    Z_STRLEN_PP(data))) {
		efree(buf);
		for (i=0; i<nkeys; i++) {
			efree(eks[i]);
		}
		efree(eks);
		efree(eksl);
		efree(pkeys);
		RETURN_FALSE;
	}

	EVP_SealFinal(&ctx, buf + len1, &len2);

	efree(pkeys);

	if (len1 + len2 > 0) {
		zval_dtor(*sealdata);
		buf[len1 + len2] = '\0';
		ZVAL_STRINGL(*sealdata, erealloc(buf, len1 + len2 + 1),
			     len1 + len2, 0);

		zval_dtor(*ekeys);
		if (array_init(*ekeys) == FAILURE) {
			php_error(E_ERROR, "Cannot initialize return value");
			for (i=0; i<nkeys; i++) {
				efree(eks[i]);
			}
			efree(eks);
			efree(eksl);
			RETURN_FALSE;
		}
		for (i=0; i<nkeys; i++) {
			eks[i][eksl[i]] = '\0';
			add_next_index_stringl(*ekeys,
					       erealloc(eks[i], eksl[i] + 1),
					       eksl[i], 0);
		}
		efree(eks);
		efree(eksl);

#if 0
		/* If allow ciphers that need IV, we need this */
		zval_dtor(*ivec);
		if (ivlen) {
			iv[ivlen] = '\0';
			ZVAL_STRINGL(*ivec, erealloc(iv, ivlen + 1), ivlen, 0);
		} else {
			ZVAL_EMPTY_STRING(*ivec);
		}
#endif
	} else {
		efree(buf);
		for (i=0; i<nkeys; i++) {
			efree(eks[i]);
		}
		efree(eks);
		efree(eksl);
	}
	
	RETURN_LONG(len1 + len2);
}
/* }}} */

/* {{{ proto bool openssl_open(string data, string opendata, string ekey,
                               int privkey)
   Open data */
PHP_FUNCTION(openssl_open)
{
	zval **privkey, **data, **opendata, **ekey;
	EVP_PKEY *pkey;
	int len1, len2, ekl;
	unsigned char *buf, *ek;

	EVP_CIPHER_CTX ctx;

	if (ZEND_NUM_ARGS() != 4 ||
	    zend_get_parameters_ex(4, &data, &opendata, &ekey,
				   &privkey) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(data);
	convert_to_string_ex(ekey);
	
	ZEND_FETCH_RESOURCE(pkey, EVP_PKEY *, privkey, -1, "OpenSSL key",
			    le_key);

	buf = emalloc(Z_STRLEN_PP(data) + 1);
	if (buf == NULL) {
		RETURN_FALSE;
	}

	if (!EVP_OpenInit(&ctx, EVP_rc4(), Z_STRVAL_PP(ekey),
			  Z_STRLEN_PP(ekey), NULL, pkey) ||
	    !EVP_OpenUpdate(&ctx, buf, &len1, Z_STRVAL_PP(data),
			    Z_STRLEN_PP(data)) ||
	    !EVP_OpenFinal(&ctx, buf + len1, &len2) ||
	    (len1 + len2 == 0)) {
		efree(buf);
		RETURN_FALSE;
	}
	
	zval_dtor(*opendata);
	buf[len1 + len2] = '\0';
	ZVAL_STRINGL(*opendata, erealloc(buf, len1 + len2 + 1), len1 + len2, 0);
	RETURN_TRUE;
}
/* }}} */

/* {{{ _php_pkey_free() */
static void _php_pkey_free(zend_rsrc_list_entry *rsrc)
{
	EVP_PKEY *pkey = (EVP_PKEY *)rsrc->ptr;
	EVP_PKEY_free(pkey);
}
/* }}} */

#if 0
/* {{{ _php_x509_free() */
static void _php_x509_free(zend_rsrc_list_entry *rsrc)
{
	X509 *x509 = (X509 *)rsrc->ptr;
	X509_free(x509);
}
/* }}} */
#endif

/*
 * Local variables:
 * tab-width: 8
 * c-basic-offset: 8
 * End:
 */
