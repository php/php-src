/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
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
   | Wez Furlong <wez@thebrainroom.com>                                   |
   | Assymetric en/decryption code by Sascha Kettler <kettler@gmx.net>    |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_openssl.h"

/* PHP Includes */
#include "ext/standard/file.h"
#include "ext/standard/info.h"

/* OpenSSL includes */
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/crypto.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#define DEBUG_SMIME	0

static unsigned char arg2of3_force_ref[] =
                       { 3, BYREF_NONE, BYREF_FORCE, BYREF_NONE };
static unsigned char arg2of4_force_ref[] =
                       { 4, BYREF_NONE, BYREF_FORCE, BYREF_NONE, BYREF_NONE };
static unsigned char arg2and3of4_force_ref[] =
                       { 4, BYREF_NONE, BYREF_FORCE, BYREF_FORCE, BYREF_NONE };

/* {{{ openssl_functions[]
 */
function_entry openssl_functions[] = {
	PHP_FE(openssl_get_privatekey,     NULL)
	PHP_FE(openssl_get_publickey,      NULL)
	PHP_FE(openssl_free_key,           NULL)

	PHP_FE(openssl_x509_read,          		NULL)
	PHP_FE(openssl_x509_free,          		NULL)

	PHP_FE(openssl_x509_parse,			  NULL)
	PHP_FE(openssl_x509_checkpurpose,		NULL)

	PHP_FE(openssl_sign,               arg2of3_force_ref)
	PHP_FE(openssl_verify,             NULL)
	PHP_FE(openssl_seal,               arg2and3of4_force_ref)
	PHP_FE(openssl_open,               arg2of4_force_ref)
/* for S/MIME handling */
	PHP_FE(openssl_pkcs7_verify,	  	  NULL)
	PHP_FE(openssl_pkcs7_decrypt,		  NULL)
	PHP_FE(openssl_pkcs7_sign,			  NULL)
	PHP_FE(openssl_pkcs7_encrypt,		  NULL)

 	PHP_FE(openssl_private_encrypt,    arg2of3_force_ref)
 	PHP_FE(openssl_private_decrypt,    arg2of3_force_ref)
 	PHP_FE(openssl_public_encrypt,     arg2of3_force_ref)
 	PHP_FE(openssl_public_decrypt,     arg2of3_force_ref)

	PHP_FE(openssl_error_string, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ openssl_module_entry
 */
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
/* }}} */

#ifdef COMPILE_DL_OPENSSL
ZEND_GET_MODULE(openssl)
#endif

static void _php_pkey_free(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static int le_key;

static void _php_x509_free(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static int le_x509;

static X509 * php_openssl_x509_from_zval(zval ** val, int makeresource, long * resourceval TSRMLS_DC);
static EVP_PKEY * php_openssl_evp_from_zval(zval ** val, int public_key, char * passphrase, int makeresource, long * resourceval TSRMLS_DC);
static X509_STORE 	  * setup_verify(zval * calist TSRMLS_DC);
static STACK_OF(X509) * load_all_certs_from_file(char *certfile);

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(openssl)
{
	le_key = zend_register_list_destructors_ex(_php_pkey_free, NULL, "OpenSSL key", module_number);
	le_x509 = zend_register_list_destructors_ex(_php_x509_free, NULL, "OpenSSL X.509", module_number);

	OpenSSL_add_all_ciphers();

/*
	SSL_load_error_strings();
*/
	ERR_load_ERR_strings();
	ERR_load_crypto_strings();
	ERR_load_EVP_strings();

	/* purposes for cert purpose checking */
	REGISTER_LONG_CONSTANT("X509_PURPOSE_SSL_CLIENT", X509_PURPOSE_SSL_CLIENT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_SSL_SERVER", X509_PURPOSE_SSL_SERVER, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_NS_SSL_SERVER", X509_PURPOSE_NS_SSL_SERVER, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_SMIME_SIGN", X509_PURPOSE_SMIME_SIGN, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_SMIME_ENCRYPT", X509_PURPOSE_SMIME_ENCRYPT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_CRL_SIGN", X509_PURPOSE_CRL_SIGN, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_ANY", X509_PURPOSE_ANY, CONST_CS|CONST_PERSISTENT);

	/* flags for S/MIME */
	REGISTER_LONG_CONSTANT("PKCS7_DETACHED", PKCS7_DETACHED, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_TEXT", PKCS7_TEXT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_NOINTERN", PKCS7_NOINTERN, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_NOVERIFY", PKCS7_NOVERIFY, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_NOCHAIN", PKCS7_NOCHAIN, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_NOCERTS", PKCS7_NOCERTS, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_NOATTR", PKCS7_NOATTR, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_BINARY", PKCS7_BINARY, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PKCS7_NOSIGS", PKCS7_NOSIGS, CONST_CS|CONST_PERSISTENT);

 	REGISTER_LONG_CONSTANT("OPENSSL_PKCS1_PADDING", 
 			       RSA_PKCS1_PADDING,
 			       CONST_CS|CONST_PERSISTENT);
 	REGISTER_LONG_CONSTANT("OPENSSL_SSLV23_PADDING", 
 			       RSA_SSLV23_PADDING,
 			       CONST_CS|CONST_PERSISTENT);
 	REGISTER_LONG_CONSTANT("OPENSSL_NO_PADDING", 
 			       RSA_NO_PADDING,
 			       CONST_CS|CONST_PERSISTENT);
 	REGISTER_LONG_CONSTANT("OPENSSL_PKCS1_OAEP_PADDING", 
 			       RSA_PKCS1_OAEP_PADDING,
 			       CONST_CS|CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(openssl)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "OpenSSL support", "enabled");
	php_info_print_table_row(2, "OpenSSL Version", OPENSSL_VERSION_TEXT);
	php_info_print_table_end();
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(openssl)
{
	EVP_cleanup();
	return SUCCESS;
}
/* }}} */

/* {{{ php_openssl_x509_from_zval
	Given a zval, coerce it into an X509 object.
	The zval can be:
		. X509 resource created using openssl_read_x509()
		. if it starts with file:// then it will be interpreted as the path to that cert
		. it will be interpreted as the cert data
	If you supply makeresource, the result will be registered as an x509 resource and
	it's value returned in makeresource.
*/
static X509 * php_openssl_x509_from_zval(zval ** val, int makeresource, long * resourceval TSRMLS_DC)
{
	X509 *cert = NULL;

	if (resourceval)
		*resourceval = -1;

	if (Z_TYPE_PP(val) == IS_RESOURCE)	{
		/* is it an x509 resource ? */
		void * what;
		int type;

		what = zend_fetch_resource(val TSRMLS_CC, -1, "OpenSSL X.509", &type, 1, le_x509);
		if (!what)
			return NULL;

		/* this is so callers can decide if they should free the X509 */
		if (resourceval)
			*resourceval = Z_LVAL_PP(val);

		if (type == le_x509)
			return (X509*)what;

		/* other types could be used here - eg: file pointers and read in the data from them */

		return NULL;
	}
	/* force it to be a string and check if it refers to a file */
	convert_to_string_ex(val);

	if (Z_STRLEN_PP(val) > 7 && memcmp(Z_STRVAL_PP(val), "file://", 7) == 0)	{
		/* read cert from the named file */
		BIO *in;

		in = BIO_new_file(Z_STRVAL_PP(val) + 7, "r");
		if (in == NULL)
			return NULL;
		cert = PEM_read_bio_X509(in, NULL, NULL, NULL);
		BIO_free(in);

	}
	else	{
		BIO *in;

		in = BIO_new_mem_buf(Z_STRVAL_PP(val), Z_STRLEN_PP(val));
		if (in == NULL)
			return NULL;

		cert = (X509 *) PEM_ASN1_read_bio((char *(*)())d2i_X509,
				PEM_STRING_X509, in,
				NULL, NULL, NULL);
		BIO_free(in);
	}

	if (cert && makeresource && resourceval)	{
		*resourceval = zend_list_insert(cert, le_x509);
	}
	return cert;
}
/* }}} */

/* {{{ php_openssl_evp_from_zval
   Given a zval, coerce it into a EVP_PKEY object.
	It can be:
		1. private key resource from openssl_get_privatekey()
		2. X509 resource -> public key will be extracted from it
		3. if it starts with file:// interpreted as path to key file
		4. interpreted as the data from the cert/key file and interpreted in same way as openssl_get_privatekey()
		5. an array(0 => [items 2..4], 1 => passphrase)
	NOTE: If you are requesting a private key but have not specified a passphrase, you should use an
	empty string rather than NULL for the passphrase - NULL causes a passphrase prompt to be emitted in
	the Apache error log!
*/
static EVP_PKEY * php_openssl_evp_from_zval(zval ** val, int public_key, char * passphrase, int makeresource, long * resourceval TSRMLS_DC)
{
	EVP_PKEY * key = NULL;
	X509 * cert = NULL;
	int free_cert = 0;
	long cert_res = -1;
	char * filename = NULL;
	
	if (resourceval)
		*resourceval = -1;

	if (Z_TYPE_PP(val) == IS_ARRAY)	{
		zval ** zphrase;
		
		/* get passphrase */

		if (zend_hash_index_find(HASH_OF(*val), 1, (void **)&zphrase) == FAILURE)	{
			zend_error(E_WARNING, "%s(): key array must be of the form array(0 => key, 1 => phrase)", get_active_function_name(TSRMLS_C));
			return NULL;
		}
		convert_to_string_ex(zphrase);
		passphrase = Z_STRVAL_PP(zphrase);

		/* now set val to be the key param and continue */
		if (zend_hash_index_find(HASH_OF(*val), 0, (void **)&val) == FAILURE)	{
			zend_error(E_WARNING, "%s(): key array must be of the form array(0 => key, 1 => phrase)", get_active_function_name(TSRMLS_C));
			return NULL;
		}
	}

	if (Z_TYPE_PP(val) == IS_RESOURCE)	{
		void * what;
		int type;

		what = zend_fetch_resource(val TSRMLS_CC, -1, "OpenSSL X.509/key", &type, 2, le_x509, le_key);
		if (!what)
			return NULL;

		if (resourceval)
			*resourceval = Z_LVAL_PP(val);

		if (type == le_x509)	{
			/* extract key from cert, depending on public_key param */
			cert = (X509*)what;
			free_cert = 0;
		}
		else if (type == le_key)	{
			/* got the key - return it */
			return (EVP_PKEY*)what;
		}

		/* other types could be used here - eg: file pointers and read in the data from them */

		return NULL;
	}
	else	{
		/* force it to be a string and check if it refers to a file */
		convert_to_string_ex(val);

		if (Z_STRLEN_PP(val) > 7 && memcmp(Z_STRVAL_PP(val), "file://", 7) == 0)
			filename = Z_STRVAL_PP(val) + 7;

		/* it's an X509 file/cert of some kind, and we need to extract the data from that */
		if (public_key)	{
			cert = php_openssl_x509_from_zval(val, 0, &cert_res TSRMLS_CC);
			free_cert = (cert_res == -1);
			/* actual extraction done later */
		}
		else	{
			/* we want the private key */
			if (filename)	{
				BIO *in = BIO_new_file(filename, "r");
				if (in == NULL)
					return NULL;
				key = PEM_read_bio_PrivateKey(in, NULL,NULL, passphrase);
				BIO_free(in);
			}
			else	{
				BIO *	b = BIO_new_mem_buf(Z_STRVAL_PP(val), Z_STRLEN_PP(val));
				if (b == NULL)
					return NULL;

				key = (EVP_PKEY *) PEM_ASN1_read_bio((char *(*)())d2i_PrivateKey,
					      PEM_STRING_EVP_PKEY, b,
					      NULL, NULL, passphrase);
				BIO_free(b);
			}
		}
	}

	if (public_key && cert && key == NULL)	{
		/* extract public key from X509 cert */
		key = (EVP_PKEY *) X509_get_pubkey(cert);
	}

	if (free_cert && cert)
		X509_free(cert);

	if (key && makeresource && resourceval)	{
		*resourceval = zend_list_insert(key, le_key);
	}
	return key;
}
/* }}} */

/* {{{ proto bool openssl_private_encrypt(string data, string crypted, mixed key [, int padding])
   Encrypt data with private key */
PHP_FUNCTION(openssl_private_encrypt)
{
	zval *key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	unsigned char *cryptedbuf = NULL;
	int successful = 0;
	long keyresource = -1;
	char * data;
	long data_len, padding = RSA_PKCS1_PADDING;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE)
		return;

	RETVAL_FALSE;

	pkey = php_openssl_evp_from_zval(&key, 0, "", 0, &keyresource TSRMLS_CC);

	if (pkey == NULL)	{
		zend_error(E_WARNING, "%s(): key param is not a valid private key",
				get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	
	cryptedlen = EVP_PKEY_size(pkey);
	cryptedbuf = emalloc(cryptedlen + 1);

	switch (pkey->type) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			successful =  (RSA_private_encrypt(data_len, 
						data, 
						cryptedbuf, 
						pkey->pkey.rsa, 
						padding) == cryptedlen);
			break;
		default:
			zend_error(E_WARNING, "%s(): key type not supported in this PHP build!");
	}

	if (successful) {
		zval_dtor(crypted);
		cryptedbuf[cryptedlen] = '\0';
		ZVAL_STRINGL(crypted, cryptedbuf, cryptedlen, 0);
		cryptedbuf = NULL;
		RETVAL_TRUE;
	}
	if (cryptedbuf)
		efree(cryptedbuf);
	if (keyresource == -1)
		EVP_PKEY_free(pkey);
}
/* }}} */

/* {{{ proto bool openssl_private_decrypt(string data, string crypted, mixed key [, int padding])
   Decrypt data with private key */
PHP_FUNCTION(openssl_private_decrypt)
{
	zval *key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	unsigned char *cryptedbuf;
	unsigned char *crypttemp;
	int successful = 0;
	long padding = RSA_PKCS1_PADDING;
	long keyresource = -1;
	char * data;
	long data_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE)
		return;

	RETVAL_FALSE;
	
	pkey = php_openssl_evp_from_zval(&key, 0, "", 0, &keyresource TSRMLS_CC);
	if (pkey == NULL)	{
		zend_error(E_WARNING, "%s(): key parameter is not a valid private key", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	
	cryptedlen = EVP_PKEY_size(pkey);
	crypttemp = emalloc(cryptedlen + 1);

	switch (pkey->type) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			cryptedlen = RSA_private_decrypt(data_len, 
					data, 
					crypttemp, 
					pkey->pkey.rsa, 
					padding);
			if (cryptedlen != -1) {
				cryptedbuf = emalloc(cryptedlen + 1);
				memcpy(cryptedbuf, crypttemp, cryptedlen);
				successful = 1;
			}
			break;
		default:
			zend_error(E_WARNING, "%s(): key type not supported in this PHP build!");
	}

	efree(crypttemp);

	if (successful) {
		zval_dtor(crypted);
		cryptedbuf[cryptedlen] = '\0';
		ZVAL_STRINGL(crypted, cryptedbuf, cryptedlen, 0);
		cryptedbuf = NULL;
		RETVAL_TRUE;
	}

	if (keyresource == -1)
		EVP_PKEY_free(pkey);
	if (cryptedbuf)
		efree(cryptedbuf);
}
/* }}} */

/* {{{ proto bool openssl_public_encrypt(string data, string crypted, mixed key [, int padding])
   Encrypt data with public key */
PHP_FUNCTION(openssl_public_encrypt)
{
	zval *key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	unsigned char *cryptedbuf;
	int successful = 0;
	long keyresource = -1;
	long padding = RSA_PKCS1_PADDING;
	char * data;
	long data_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE)
		return;

	RETVAL_FALSE;
	
	pkey = php_openssl_evp_from_zval(&key, 1, NULL, 0, &keyresource TSRMLS_CC);
	if (pkey == NULL)	{
		zend_error(E_WARNING, "%s(): key parameter is not a valid public key", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	cryptedlen = EVP_PKEY_size(pkey);
	cryptedbuf = emalloc(cryptedlen + 1);

	switch (pkey->type) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			successful = (RSA_public_encrypt(data_len, 
						data, 
						cryptedbuf, 
						pkey->pkey.rsa, 
						padding) == cryptedlen);
			break;
		default:
			zend_error(E_WARNING, "%s(): key type not supported in this PHP build!");

	}

	if (successful) {
		zval_dtor(crypted);
		cryptedbuf[cryptedlen] = '\0';
		ZVAL_STRINGL(crypted, cryptedbuf, cryptedlen, 0);
		cryptedbuf = NULL;
		RETVAL_TRUE;
	}
	if (keyresource == -1)
		EVP_PKEY_free(pkey);
	if (cryptedbuf)
		efree(cryptedbuf);
}
/* }}} */

/* {{{ proto bool openssl_public_decrypt(string data, string crypted, resource key [, int padding])
   Decrypt data with public key */
PHP_FUNCTION(openssl_public_decrypt)
{
	zval *key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	unsigned char *cryptedbuf;
	unsigned char *crypttemp;
	int successful = 0;
	long keyresource = -1;
	long padding = RSA_PKCS1_PADDING;
	char * data;
	long data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE)
		return;

	RETVAL_FALSE;
	
	pkey = php_openssl_evp_from_zval(&key, 1, NULL, 0, &keyresource TSRMLS_CC);
	if (pkey == NULL)	{
		zend_error(E_WARNING, "%s(): key parameter is not a valid public key", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	cryptedlen = EVP_PKEY_size(pkey);
	crypttemp = emalloc(cryptedlen + 1);

	switch (pkey->type) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			cryptedlen = RSA_public_decrypt(data_len, 
					data, 
					crypttemp, 
					pkey->pkey.rsa, 
					padding);
			if (cryptedlen != -1) {
				cryptedbuf = emalloc(cryptedlen + 1);
				memcpy(cryptedbuf, crypttemp, cryptedlen);
				successful = 1;
			}
			break;
			
		default:
			zend_error(E_WARNING, "%s(): key type not supported in this PHP build!");
		 
	}

	efree(crypttemp);

	if (successful) {
		zval_dtor(crypted);
		cryptedbuf[cryptedlen] = '\0';
		ZVAL_STRINGL(crypted, cryptedbuf, cryptedlen, 0);
		cryptedbuf = NULL;
		RETVAL_TRUE;
	}

	if (cryptedbuf)
		efree(cryptedbuf);
	if (keyresource == -1)
		EVP_PKEY_free(pkey);
}
/* }}} */

/* {{{ proto int openssl_get_privatekey(string key [, string passphrase])
   Get private key */
PHP_FUNCTION(openssl_get_privatekey)
{
	EVP_PKEY *pkey;
	zval * key;
	char * passphrase = "";
	int passphrase_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|s", &key, &passphrase, &passphrase_len) == FAILURE)
		return;

	return_value->type = IS_RESOURCE;
	pkey = php_openssl_evp_from_zval(&key, 0, passphrase, 1, &Z_LVAL_P(return_value) TSRMLS_CC);

	if (pkey == NULL) {
		zend_error(E_WARNING, "%s(): unable to coerce arg to a private key", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ openssl -> PHP "bridging" */

static void add_assoc_name_entry(zval * val, char * key, X509_NAME * name, int shortname)
{
	zval * subitem;
	int i;
	char * sn, * ln;
	int nid;
	X509_NAME_ENTRY * ne;
	ASN1_STRING * str;
	ASN1_OBJECT * obj;

	MAKE_STD_ZVAL(subitem);
	array_init(subitem);

	for (i = 0; i < X509_NAME_entry_count(name); i++)	{
		ne	= X509_NAME_get_entry(name, i);
		obj = X509_NAME_ENTRY_get_object(ne);
		str = X509_NAME_ENTRY_get_data(ne);
		nid = OBJ_obj2nid(obj);
		if (shortname)	{
			sn = (char*)OBJ_nid2sn(nid);
			add_assoc_stringl(subitem, sn, str->data, str->length, 1);
		}
		else	{
			ln = (char*)OBJ_nid2ln(nid);
			add_assoc_stringl(subitem, ln, str->data, str->length, 1);
		}
	}
	zend_hash_update(HASH_OF(val), key, strlen(key) + 1, (void *)&subitem, sizeof(subitem), NULL);
}

static void add_assoc_asn1_string(zval * val, char * key, ASN1_STRING * str)
{
	add_assoc_stringl(val, key, str->data, str->length, 1);
}

static time_t asn1_time_to_time_t(ASN1_UTCTIME * timestr TSRMLS_DC)
{
/*
	This is how the time string is formatted:

   sprintf(p,"%02d%02d%02d%02d%02d%02dZ",ts->tm_year%100,
      ts->tm_mon+1,ts->tm_mday,ts->tm_hour,ts->tm_min,ts->tm_sec);
*/

	time_t ret;
	struct tm thetime;
	char * strbuf;
	char * thestr;
	long gmadjust = 0;

	if (timestr->length < 13)	{
		zend_error(E_WARNING, "%s(): extension author too lazy to parse %s correctly", get_active_function_name(TSRMLS_C), timestr->data);
		return (time_t)-1;
	}

	strbuf = estrdup(timestr->data);


	memset(&thetime, 0, sizeof(thetime));

	/* we work backwards so that we can use atoi more easily */

	thestr = strbuf + timestr->length - 3;

	thetime.tm_sec = atoi(thestr);
	*thestr = '\0';
	thestr -= 2;
	thetime.tm_min = atoi(thestr);
	*thestr = '\0';
	thestr -= 2;
	thetime.tm_hour = atoi(thestr);
	*thestr = '\0';
	thestr -= 2;
	thetime.tm_mday = atoi(thestr);
	*thestr = '\0';
	thestr -= 2;
	thetime.tm_mon = atoi(thestr)-1;
	*thestr = '\0';
	thestr -= 2;
	thetime.tm_year = atoi(thestr);

	if (thetime.tm_year < 68)
		thetime.tm_year += 100;

	thetime.tm_isdst = -1;
	ret = mktime(&thetime);

#if HAVE_TM_GMTOFF
	gmadjust = thetime.tm_gmtoff;
#else
	/*
	** If correcting for daylight savings time, we set the adjustment to
	** the value of timezone - 3600 seconds. Otherwise, we need to overcorrect and
	** set the adjustment to the main timezone + 3600 seconds.
	*/
	gmadjust = -(thetime.tm_isdst ? timezone - 3600 : timezone + 3600);
#endif
	ret += gmadjust;

	efree(strbuf);

	return ret;
}
/* }}} */

/* {{{ proto array openssl_x509_parse(mixed x509[, bool shortnames=true])
	returns an array of the fields/values of the cert */
PHP_FUNCTION(openssl_x509_parse)
{
	zval * zcert;
	X509 * cert = NULL;
	long certresource = -1;
	int i;
	zend_bool useshortnames = 1;
	char * tmpstr;
	zval * subitem;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &zcert, &useshortnames))
		return;
	
	cert = php_openssl_x509_from_zval(&zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL)
		RETURN_FALSE;

	array_init(return_value);

	if (cert->name)
		add_assoc_string(return_value, "name", cert->name, 1);
/*	add_assoc_bool(return_value, "valid", cert->valid); */

	add_assoc_name_entry(return_value, "subject", 		X509_get_subject_name(cert), useshortnames);
	add_assoc_name_entry(return_value, "issuer", 		X509_get_issuer_name(cert), useshortnames);
	add_assoc_long(return_value, "version", 			X509_get_version(cert));
	add_assoc_long(return_value, "serialNumber", 		ASN1_INTEGER_get(X509_get_serialNumber(cert)));

	add_assoc_asn1_string(return_value, "validFrom", 	X509_get_notBefore(cert));
	add_assoc_asn1_string(return_value, "validTo", 		X509_get_notAfter(cert));

	add_assoc_long(return_value, "validFrom_time_t", 	asn1_time_to_time_t(X509_get_notBefore(cert) TSRMLS_CC));
	add_assoc_long(return_value, "validTo_time_t", 		asn1_time_to_time_t(X509_get_notAfter(cert) TSRMLS_CC));

	tmpstr = X509_alias_get0(cert, NULL);
	if (tmpstr)
		add_assoc_string(return_value, "alias", tmpstr, 1);

/*
	add_assoc_long(return_value, "signaturetypeLONG", X509_get_signature_type(cert));
	add_assoc_string(return_value, "signaturetype", OBJ_nid2sn(X509_get_signature_type(cert)), 1);
	add_assoc_string(return_value, "signaturetypeLN", OBJ_nid2ln(X509_get_signature_type(cert)), 1);
*/
	MAKE_STD_ZVAL(subitem);
	array_init(subitem);

	/* NOTE: the purposes are added as integer keys - the keys match up to the X509_PURPOSE_SSL_XXX defines
	   in x509v3.h */
	for (i = 0; i < X509_PURPOSE_get_count(); i++)	{
		int id, purpset;
		char * pname;
		X509_PURPOSE * purp;
		zval * subsub;

		MAKE_STD_ZVAL(subsub);
		array_init(subsub);

		purp = X509_PURPOSE_get0(i);
		id = X509_PURPOSE_get_id(purp);

		purpset = X509_check_purpose(cert, id, 0);
		add_index_bool(subsub, 0, purpset);

		purpset = X509_check_purpose(cert, id, 1);
		add_index_bool(subsub, 1, purpset);

		pname = useshortnames ? X509_PURPOSE_get0_sname(purp) : X509_PURPOSE_get0_name(purp);
		add_index_string(subsub, 2, pname, 1);

		/* NOTE: if purpset > 1 then it's a warning - we should mention it ? */

		add_index_zval(subitem, id, subsub);
	}
	add_assoc_zval(return_value, "purposes", subitem);

	if (certresource == -1 && cert)
		X509_free(cert);

}
/* }}} */

/* {{{ load_all_certs_from_file */
static STACK_OF(X509) * load_all_certs_from_file(char *certfile)
{
   STACK_OF(X509_INFO) *sk=NULL;
   STACK_OF(X509) *stack=NULL, *ret=NULL;
   BIO *in=NULL;
   X509_INFO *xi;
   TSRMLS_FETCH();

	if(!(stack = sk_X509_new_null())) {
		zend_error(E_ERROR, "%s(): memory allocation failure", get_active_function_name(TSRMLS_C));
		goto end;
	}

	if(!(in=BIO_new_file(certfile, "r"))) {
		zend_error(E_WARNING, "%s(): error opening the file, %s", get_active_function_name(TSRMLS_C), certfile);
		goto end;
	}

	/* This loads from a file, a stack of x509/crl/pkey sets */
	if(!(sk=PEM_X509_INFO_read_bio(in, NULL, NULL, NULL))) {
		zend_error(E_WARNING, "%s(): error reading the file, %s", get_active_function_name(TSRMLS_C), certfile);
		goto end;
	}

	/* scan over it and pull out the certs */
	while (sk_X509_INFO_num(sk))
	{
		xi=sk_X509_INFO_shift(sk);
		if (xi->x509 != NULL)
		{
			sk_X509_push(stack,xi->x509);
			xi->x509=NULL;
		}
		X509_INFO_free(xi);
	}
	if(!sk_X509_num(stack)) {
		zend_error(E_WARNING, "%s(): no certificates in file, %s", get_active_function_name(TSRMLS_C), certfile);
		sk_X509_free(stack);
		goto end;
	}
	ret=stack;
end:
	BIO_free(in);
	sk_X509_INFO_free(sk);

	return ret;
}
/* }}} */

/* {{{ check_cert */
static int check_cert(X509_STORE *ctx, X509 *x, STACK_OF(X509) *untrustedchain, int purpose)
{
	int ret=0;
	X509_STORE_CTX *csc;
	TSRMLS_FETCH();

	csc = X509_STORE_CTX_new();
	if (csc == NULL)
	{
		zend_error(E_ERROR, "%s(): memory allocation failure", get_active_function_name(TSRMLS_C));
		return 0;
	}
	X509_STORE_CTX_init(csc, ctx, x, untrustedchain);

	if(purpose >= 0)
		X509_STORE_CTX_set_purpose(csc, purpose);

	ret = X509_verify_cert(csc);
	X509_STORE_CTX_free(csc);

	return ret;
}
/* }}} */

/* {{{ proto int openssl_x509_checkpurpose(mixed x509cert, int purpose, array cainfo[, string untrustedfile])
	check the cert to see if it can be used for the purpose in purpose. cainfo holds information about trusted CAs */
PHP_FUNCTION(openssl_x509_checkpurpose)
{
	zval * zcert, * zcainfo;
	X509_STORE * cainfo = NULL;
	X509 * cert = NULL;
	long certresource = -1;
	STACK_OF(X509) * untrustedchain = NULL;
	long purpose;
	char * untrusted = NULL;
	long untrusted_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zla|s", &zcert, &purpose, &zcainfo, &untrusted, &untrusted_len)
			== FAILURE)
		return;

	RETVAL_LONG(-1);

	if (untrusted)	{
		untrustedchain = load_all_certs_from_file(untrusted);
		if (untrustedchain == NULL)
			goto clean_exit;
	}

	cainfo = setup_verify(zcainfo TSRMLS_CC);
	if (cainfo == NULL)
		goto clean_exit;

	cert = php_openssl_x509_from_zval(&zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL)
		goto clean_exit;

	RETVAL_BOOL(check_cert(cainfo, cert, untrustedchain, purpose));

clean_exit:
	if (certresource == 1 && cert)
		X509_free(cert);
	if (cainfo)
		X509_STORE_free(cainfo);
	if (untrustedchain)
		sk_X509_pop_free(untrustedchain, X509_free);
}
/* }}} */

/* {{{ proto int openssl_get_publickey(mixed cert)
   Get public key from X.509 certificate */
PHP_FUNCTION(openssl_get_publickey)
{
	zval *cert;
	EVP_PKEY *pkey;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &cert) == FAILURE)
		return;

	return_value->type = IS_RESOURCE;
	pkey = php_openssl_evp_from_zval(&cert, 1, NULL, 1, &Z_LVAL_P(return_value) TSRMLS_CC);

	if (pkey == NULL) {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void openssl_free_key(int key)
   Free key */
PHP_FUNCTION(openssl_free_key)
{
	zval *key;
	EVP_PKEY *pkey;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &key) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(pkey, EVP_PKEY *, &key, -1, "OpenSSL key", le_key);
	zend_list_delete(Z_LVAL_P(key));
}
/* }}} */

/* {{{ proto resource openssl_x509_read(mixed cert)
   Read X.509 certificate */
PHP_FUNCTION(openssl_x509_read)
{
	zval *cert;
	X509 *x509;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &cert) == FAILURE)
		return;

	return_value->type = IS_RESOURCE;
	x509 = php_openssl_x509_from_zval(&cert, 1, &Z_LVAL_P(return_value) TSRMLS_CC);

	if (x509 == NULL) {
		zend_error(E_WARNING, "%s() supplied parameter cannot be coerced into an X509 certificate!", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void openssl_free_x509(resource x509)
   Free X.509 certificate */
PHP_FUNCTION(openssl_x509_free)
{
	zval *x509;
	X509 *cert;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &x509) == FAILURE)
		return;

	ZEND_FETCH_RESOURCE(cert, X509 *, &x509, -1, "OpenSSL X.509", le_x509);
	zend_list_delete(Z_LVAL_P(x509));
}
/* }}} */

/* {{{ setup_verify
 * calist is an array containing file and directory names.  create a
 * certificate store and add those certs to it for use in verification.
*/
static X509_STORE * setup_verify(zval * calist TSRMLS_DC)
{
	X509_STORE *store;
	X509_LOOKUP * dir_lookup, * file_lookup;
	HashPosition pos;
	int ndirs = 0, nfiles = 0;

	store = X509_STORE_new();

	if (store == NULL)
		return NULL;

	if (calist && (calist->type == IS_ARRAY))	{
		zend_hash_internal_pointer_reset_ex(HASH_OF(calist), &pos);
		for (;; zend_hash_move_forward_ex(HASH_OF(calist), &pos))	{
			zval ** item;
			struct stat sb;

			if (zend_hash_get_current_data_ex(HASH_OF(calist), (void**)&item, &pos) == FAILURE)
				break;

			convert_to_string_ex(item);

			if (VCWD_STAT(Z_STRVAL_PP(item), &sb) == -1)	{
				zend_error(E_WARNING, "%s() unable to stat %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(item));
				continue;
			}

			if ((sb.st_mode & S_IFREG) == S_IFREG)	{
				file_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
				if (file_lookup == NULL || !X509_LOOKUP_load_file(file_lookup, Z_STRVAL_PP(item), X509_FILETYPE_PEM))
					zend_error(E_WARNING, "%s() error loading file %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(item));
				else
					nfiles++;
				file_lookup = NULL;
			}
			else	{
				dir_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
				if (dir_lookup == NULL || !X509_LOOKUP_add_dir(dir_lookup, Z_STRVAL_PP(item), X509_FILETYPE_PEM))
					zend_error(E_WARNING, "%s() error loading directory %s", get_active_function_name(TSRMLS_C), Z_STRVAL_PP(item));
				else
					ndirs++;
				dir_lookup = NULL;
			}
		}
	}
	if (nfiles == 0)	{
		file_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
		if (file_lookup)
			X509_LOOKUP_load_file(file_lookup, NULL, X509_FILETYPE_DEFAULT);
	}
	if (ndirs == 0)	{
		dir_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
		if (dir_lookup)
			X509_LOOKUP_add_dir(dir_lookup, NULL, X509_FILETYPE_DEFAULT);
	}
	return store;
}
/* }}} */

/* {{{ proto mixed openssl_error_string()
	returns a description of the last error, and alters the index of the error messages. returns false when the are no more messages. */
PHP_FUNCTION(openssl_error_string)
{
	char buf[512];
	unsigned long val;

	if (ZEND_NUM_ARGS() != 0)	{
		WRONG_PARAM_COUNT;
	}

	val = ERR_get_error();
	if (val)
	{
		RETURN_STRING(ERR_error_string(val, buf), 1);
	}
	else
	{
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool openssl_pkcs7_verify(string filename, long flags[, string signerscerts][, array cainfo][, string extracerts])
	verify that the data block is intact, the signer is who they say they are, and return the certs of the signers
*/
PHP_FUNCTION(openssl_pkcs7_verify)
{
	X509_STORE * store;
	zval * cainfo = NULL;
	STACK_OF(X509) *signers= NULL;
	STACK_OF(X509) *others = NULL;
	PKCS7 * p7 = NULL;
	BIO * in = NULL, * datain = NULL;
	long flags = 0;
	char * filename; long filename_len;
	char * extracerts = NULL; long extracerts_len;
	char * signersfilename = NULL; long signersfilename_len;
	
	RETVAL_LONG(-1);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl|sas", &filename, &filename_len,
				&flags, &signersfilename, &signersfilename_len, &cainfo,
				&extracerts, &extracerts_len) == FAILURE)
		return;
	
	if (extracerts)	{
		others = load_all_certs_from_file(extracerts);
		if (others == NULL)
			goto clean_exit;
	}

	flags = flags & ~PKCS7_DETACHED;

	store = setup_verify(cainfo TSRMLS_CC);

	if (!store)
		goto clean_exit;

	in = BIO_new_file(filename, (flags & PKCS7_BINARY) ? "rb" : "r");
	if (in == NULL)
		goto clean_exit;
	p7 = SMIME_read_PKCS7(in, &datain);
	if (p7 == NULL)	{
#if DEBUG_SMIME
		zend_printf("SMIME_read_PKCS7 failed\n");
#endif
		goto clean_exit;
	}
#if DEBUG_SMIME
	zend_printf("Calling PKCS7 verify\n");
#endif

	if (PKCS7_verify(p7, others, store, datain, NULL, flags))	{

		RETVAL_TRUE;

		if (signersfilename)	{
			BIO * certout = BIO_new_file(signersfilename, "w");
			if (certout)	{
				int i;
				signers = PKCS7_get0_signers(p7, NULL, flags);

				for(i = 0; i < sk_X509_num(signers); i++)
					PEM_write_bio_X509(certout, sk_X509_value(signers, i));

				BIO_free(certout);
				sk_X509_free(signers);
			}
			else	{
				zend_error(E_WARNING, "%s(): signature OK, but cannot open %s for writing",
					  	get_active_function_name(TSRMLS_C), signersfilename);
				RETVAL_LONG(-1);
			}
		}
		goto clean_exit;
	}
	else
		RETVAL_FALSE;
clean_exit:
	X509_STORE_free(store);
	BIO_free(datain);
	BIO_free(in);
	PKCS7_free(p7);
	sk_X509_free(others);
}
/* }}} */

/* {{{ proto bool openssl_pkcs7_encrypt(string infile, string outfile, mixed recipcerts, array headers[, long flags])
	encrypt the message in the file named infile with the certificates in recipcerts and output the result to the file named outfile */
PHP_FUNCTION(openssl_pkcs7_encrypt)
{
	zval * zrecipcerts, * zheaders = NULL;
	STACK_OF(X509) * recipcerts = NULL;
	BIO * infile = NULL, * outfile = NULL;
	long flags = 0;
	PKCS7 * p7 = NULL;
	HashPosition hpos;
	zval ** zcertval;
	X509 * cert;
	EVP_CIPHER *cipher = NULL;
	uint strindexlen;
	ulong intindex;
	char * strindex;
	char * infilename = NULL;	long infilename_len;
	char * outfilename = NULL;	long outfilename_len;
	
	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ssza!|l", &infilename, &infilename_len,
				&outfilename, &outfilename_len, &zrecipcerts, &zheaders, &flags) == FAILURE)
		return;

	infile = BIO_new_file(infilename, "r");
	if (infile == NULL)
		goto clean_exit;

	outfile = BIO_new_file(outfilename, "w");
	if (outfile == NULL)
		goto clean_exit;

	recipcerts = sk_X509_new_null();

	/* get certs */
	if (Z_TYPE_P(zrecipcerts) == IS_ARRAY)	{
		zend_hash_internal_pointer_reset_ex(HASH_OF(zrecipcerts), &hpos);
		while(zend_hash_get_current_data_ex(HASH_OF(zrecipcerts), (void**)&zcertval, &hpos) == SUCCESS)	{
			long certresource;

			cert = php_openssl_x509_from_zval(zcertval, 0, &certresource TSRMLS_CC);
			if (cert == NULL)
				goto clean_exit;

			if (certresource != -1)	{
				/* we shouldn't free this particular cert, as it is a resource.
					make a copy and push that on the stack instead */
				cert = X509_dup(cert);
				if (cert == NULL)
					goto clean_exit;
			}
			sk_X509_push(recipcerts, cert);

			zend_hash_move_forward_ex(HASH_OF(zrecipcerts), &hpos);
		}
	}
	else	{
		/* a single certificate */
		long certresource;

		cert = php_openssl_x509_from_zval(&zrecipcerts, 0, &certresource TSRMLS_CC);
		if (cert == NULL)
			goto clean_exit;

		if (certresource != -1)	{
			/* we shouldn't free this particular cert, as it is a resource.
				make a copy and push that on the stack instead */
			cert = X509_dup(cert);
			if (cert == NULL)
				goto clean_exit;
		}
		sk_X509_push(recipcerts, cert);
	}

	/* TODO: allow user to choose a different cipher */
	cipher = EVP_rc2_40_cbc();
	if (cipher == NULL)
		goto clean_exit;

	p7 = PKCS7_encrypt(recipcerts, infile, cipher, flags);

	if (p7 == NULL)
		goto clean_exit;

	/* tack on extra headers */
	if (zheaders)	{
		zend_hash_internal_pointer_reset_ex(HASH_OF(zheaders), &hpos);
		while(zend_hash_get_current_data_ex(HASH_OF(zheaders), (void**)&zcertval, &hpos) == SUCCESS)	{
			zend_hash_get_current_key_ex(HASH_OF(zheaders), &strindex, &strindexlen, &intindex, 0, &hpos);

			convert_to_string_ex(zcertval);

			if (strindex)
				BIO_printf(outfile, "%s: %s\n", strindex, Z_STRVAL_PP(zcertval));
			else
				BIO_printf(outfile, "%s\n", Z_STRVAL_PP(zcertval));

			zend_hash_move_forward_ex(HASH_OF(zheaders), &hpos);
		}
	}

	BIO_reset(infile);

	/* write the encrypted data */
	SMIME_write_PKCS7(outfile, p7, infile, flags);

	RETVAL_TRUE;


clean_exit:
	PKCS7_free(p7);
	BIO_free(infile);
	BIO_free(outfile);
	if (recipcerts)
		sk_X509_pop_free(recipcerts, X509_free);

}
/* }}} */

/* {{{ proto bool openssl_pkcs7_sign(string infile, string outfile, mixed signcert, mixed signkey, array headers[, long flags][, string extracertsfilename])
	sign the MIME message in the file named infile with signcert/signkey and output the result to file name outfile. headers lists plain text headers to exclude from the signed portion of the message, and should include to, from and subject as a minimum */

PHP_FUNCTION(openssl_pkcs7_sign)
{
	zval * zcert, * zprivkey, * zheaders;
	zval ** hval;
	X509 * cert = NULL;
	EVP_PKEY * privkey = NULL;
	long flags = PKCS7_DETACHED;
	PKCS7 * p7 = NULL;
	BIO * infile = NULL, * outfile = NULL;
	STACK_OF(X509) *others = NULL;
	long certresource = -1, keyresource = -1;
	ulong intindex;
	uint strindexlen;
	HashPosition hpos;
	char * strindex;
	char * infilename;	long infilename_len;
	char * outfilename;	long outfilename_len;
	char * extracertsfilename = NULL; long extracertsfilename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sszza!|ls",
				&infilename, &infilename_len, *outfilename, &outfilename_len,
				&zcert, &zprivkey, &zheaders, &flags, &extracertsfilename,
				&extracertsfilename_len) == FAILURE)
		return;
			
	
	RETVAL_FALSE;

	if (extracertsfilename)	{
		others = load_all_certs_from_file(extracertsfilename);
		if (others == NULL)
			goto clean_exit;
	}

	privkey = php_openssl_evp_from_zval(&zprivkey, 0, "", 0, &keyresource TSRMLS_CC);
	if (privkey == NULL)	{
		zend_error(E_WARNING, "%s(): error getting private key", get_active_function_name(TSRMLS_C));
		goto clean_exit;
	}

	cert = php_openssl_x509_from_zval(&zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL)	{
		zend_error(E_WARNING, "%s(): error getting cert", get_active_function_name(TSRMLS_C));
		goto clean_exit;
	}

	infile = BIO_new_file(infilename, "r");
	if (infile == NULL)	{
		zend_error(E_WARNING, "%s(): error opening input file %s!", get_active_function_name(TSRMLS_C), infilename);
		goto clean_exit;
	}

	outfile = BIO_new_file(outfilename, "w");
	if (outfile == NULL)	{
		zend_error(E_WARNING, "%s(): error opening output file %s!", get_active_function_name(TSRMLS_C), outfilename);
		goto clean_exit;
	}

	p7 = PKCS7_sign(cert, privkey, others, infile, flags);
	if (p7 == NULL)	{
		zend_error(E_WARNING, "%s(): error creating PKCS7 structure!", get_active_function_name(TSRMLS_C));
		goto clean_exit;
	}

	BIO_reset(infile);

	/* tack on extra headers */
	if (zheaders)	{
		zend_hash_internal_pointer_reset_ex(HASH_OF(zheaders), &hpos);
		while(zend_hash_get_current_data_ex(HASH_OF(zheaders), (void**)&hval, &hpos) == SUCCESS)	{
			zend_hash_get_current_key_ex(HASH_OF(zheaders), &strindex, &strindexlen, &intindex, 0, &hpos);

			convert_to_string_ex(hval);

			if (strindex)
				BIO_printf(outfile, "%s: %s\n", strindex, Z_STRVAL_PP(hval));
			else
				BIO_printf(outfile, "%s\n", Z_STRVAL_PP(hval));

			zend_hash_move_forward_ex(HASH_OF(zheaders), &hpos);
		}
	}
	/* write the signed data */
	SMIME_write_PKCS7(outfile, p7, infile, flags);

	RETVAL_TRUE;

clean_exit:
	PKCS7_free(p7);
	BIO_free(infile);
	BIO_free(outfile);
	if (others)
		sk_X509_pop_free(others, X509_free);
	if (privkey && keyresource == -1)
		EVP_PKEY_free(privkey);
	if (cert && certresource == -1)
		X509_free(cert);
}
/* }}} */

/* {{{ proto bool openssl_pkcs7_decrypt(string infilename, string outfilename, mixed recipcert[, mixed recipkey])
	decrypt the S/MIME message in the file name infilename and output the results to the file name outfilename.  recipcert is a cert for one of the recipients. recipkey specifies the private key matching recipcert, if recipcert does not include the key */

PHP_FUNCTION(openssl_pkcs7_decrypt)
{
	zval * recipcert, * recipkey = NULL;
	X509 * cert = NULL;
	EVP_PKEY * key = NULL;
	long certresval, keyresval;
	BIO * in = NULL, * out = NULL, * datain = NULL;
	PKCS7 * p7 = NULL;
	char * infilename;	long infilename_len;
	char * outfilename;	long outfilename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ssz|z", &infilename, &infilename_len,
				&outfilename, &outfilename_len, &recipcert, &recipkey) == FAILURE)
		return;

	RETVAL_FALSE;

	cert = php_openssl_x509_from_zval(&recipcert, 0, &certresval TSRMLS_CC);
	if (cert == NULL)	{
		zend_error(E_WARNING, "%s(): unable to coerce parameter 3 to x509 cert", get_active_function_name(TSRMLS_C));
		goto clean_exit;
	}

	key = php_openssl_evp_from_zval(recipkey ? &recipkey : &recipcert, 0, "", 0, &keyresval TSRMLS_CC);
	if (key == NULL)	{
		zend_error(E_WARNING, "%s(): unable to get private key", get_active_function_name(TSRMLS_C));
		goto clean_exit;
	}

	in = BIO_new_file(infilename, "r");
	if (in == NULL)	{
		goto clean_exit;
	}
	out = BIO_new_file(outfilename, "w");
	if (out == NULL)	{
		goto clean_exit;
	}

	p7 = SMIME_read_PKCS7(in, &datain);

	if (p7 == NULL)
		goto clean_exit;

	if (PKCS7_decrypt(p7, key, cert, out, PKCS7_DETACHED))
		RETVAL_TRUE;

clean_exit:
	PKCS7_free(p7);
	BIO_free(datain);
	BIO_free(in);
	BIO_free(out);
	if (cert && certresval == -1)
		X509_free(cert);
	if (key && keyresval == -1)
		EVP_PKEY_free(key);
}
/* }}} */

/* {{{ proto bool openssl_sign(string data, &string signature, mixed key)
   Sign data */
PHP_FUNCTION(openssl_sign)
{
	zval *key, *signature;
	EVP_PKEY *pkey;
	int siglen;
	unsigned char *sigbuf;
	long keyresource = -1;
	char * data;	long data_len;
	EVP_MD_CTX md_ctx;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szz", &data, &data_len, &signature, &key) == FAILURE)
		return;

	pkey = php_openssl_evp_from_zval(&key, 0, "", 0, &keyresource TSRMLS_CC);
	if (pkey == NULL)	{
		zend_error(E_WARNING, "%s(): supplied key param cannot be coerced into a private key", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	siglen = EVP_PKEY_size(pkey);
	sigbuf = emalloc(siglen + 1);

	EVP_SignInit(&md_ctx, EVP_sha1());
	EVP_SignUpdate(&md_ctx, data, data_len);
	if (EVP_SignFinal (&md_ctx, sigbuf, &siglen, pkey)) {
		zval_dtor(signature);
		sigbuf[siglen] = '\0';
		ZVAL_STRINGL(signature, sigbuf, siglen, 0);
		RETVAL_TRUE;
	} else {
		efree(sigbuf);
		RETVAL_FALSE;
	}
	if (keyresource == -1)
		EVP_PKEY_free(pkey);
}
/* }}} */

/* {{{ proto int openssl_verify(string data, string signature, mixed key)
   Verify data */
PHP_FUNCTION(openssl_verify)
{
	zval *key;
	EVP_PKEY *pkey;
	int err;
	EVP_MD_CTX     md_ctx;
	long keyresource = -1;
	char * data;	long data_len;
	char * signature;	long signature_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ssz", &data, &data_len,
				&signature, &signature_len, &key) == FAILURE)
		return;
	
	pkey = php_openssl_evp_from_zval(&key, 1, NULL, 0, &keyresource TSRMLS_CC);
	if (pkey == NULL)	{
		zend_error(E_WARNING, "%s(): supplied key param cannot be coerced into a public key", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	EVP_VerifyInit   (&md_ctx, EVP_sha1());
	EVP_VerifyUpdate (&md_ctx, data, data_len);
	err = EVP_VerifyFinal (&md_ctx, signature, signature_len, pkey);

	if (keyresource == -1)
		EVP_PKEY_free(pkey);

	RETURN_LONG(err);
}
/* }}} */

/* {{{ proto int openssl_seal(string data, &string sealdata, &array ekeys, array pubkeys)
   Seal data */
PHP_FUNCTION(openssl_seal)
{
	zval *pubkeys, **pubkey, *sealdata, *ekeys;
	HashTable *pubkeysht;
	HashPosition pos;
	EVP_PKEY **pkeys;
	long * key_resources;	/* so we know what to cleanup */
	int i, len1, len2, *eksl, nkeys;
	unsigned char *buf = NULL, **eks;
	char * data;	long data_len;
	EVP_CIPHER_CTX ctx;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szza/",
				&data, &data_len, &sealdata, &ekeys, &pubkeys) == FAILURE)
		return;
	
	pubkeysht = HASH_OF(pubkeys);
	nkeys = pubkeysht ? zend_hash_num_elements(pubkeysht) : 0;
	if (!nkeys) {
		zend_error(E_WARNING,
				"Fourth argument to openssl_seal() must be a non-empty array");
		RETURN_FALSE;
	}

	pkeys = emalloc(nkeys * sizeof(*pkeys));
	eksl = emalloc(nkeys * sizeof(*eksl));
	eks = emalloc(nkeys * sizeof(*eks));
	key_resources = emalloc(nkeys * sizeof(long));

	/* get the public keys we are using to seal this data */
	zend_hash_internal_pointer_reset_ex(pubkeysht, &pos);
	i = 0;
	while (zend_hash_get_current_data_ex(pubkeysht, (void **) &pubkey,
				&pos) == SUCCESS) {
		pkeys[i] = php_openssl_evp_from_zval(pubkey, 1, NULL, 0, &key_resources[i] TSRMLS_CC);
		if (pkeys[i] == NULL)	{
			zend_error(E_WARNING, "%s(): not a public key (%dth member of pubkeys)", get_active_function_name(TSRMLS_C), i);
			RETVAL_FALSE;
			goto clean_exit;
		}
		eks[i] = emalloc(EVP_PKEY_size(pkeys[i]) + 1);
		zend_hash_move_forward_ex(pubkeysht, &pos);
		i++;
	}

#if OPENSSL_VERSION_NUMBER >= 0x0090600fL
	if (!EVP_EncryptInit(&ctx,EVP_rc4(),NULL,NULL)) {
		RETVAL_FALSE;
		goto clean_exit;
	}
#else
	EVP_EncryptInit(&ctx,EVP_rc4(),NULL,NULL);
#endif

#if 0
	/* Need this if allow ciphers that require initialization vector */
	ivlen = EVP_CIPHER_CTX_iv_length(&ctx);
	iv = ivlen ? emalloc(ivlen + 1) : NULL;
#endif
	/* allocate one byte extra to make room for \0 */
	buf = emalloc(data_len + EVP_CIPHER_CTX_block_size(&ctx));

	if (!EVP_SealInit(&ctx, EVP_rc4(), eks, eksl, NULL, pkeys, nkeys)
#if OPENSSL_VERSION_NUMBER >= 0x0090600fL
			|| !EVP_SealUpdate(&ctx, buf, &len1, data, data_len)
#endif
		) 
	{
		RETVAL_FALSE;
		efree(buf);
		goto clean_exit;

	}

#if OPENSSL_VERSION_NUMBER < 0x0090600fL
	EVP_SealUpdate(&ctx, buf, &len1, data, data_len);
#endif
	EVP_SealFinal(&ctx, buf + len1, &len2);

	if (len1 + len2 > 0) {
		zval_dtor(sealdata);
		buf[len1 + len2] = '\0';
		buf = erealloc(buf, len1 + len2 + 1);
		ZVAL_STRINGL(sealdata, buf, len1 + len2, 0);

		zval_dtor(ekeys);
		if (array_init(ekeys) == FAILURE) {
			zend_error(E_ERROR, "Cannot initialize return value");
			RETVAL_FALSE;
			efree(buf);
			goto clean_exit;
		}
		for (i=0; i<nkeys; i++) {
			eks[i][eksl[i]] = '\0';
			add_next_index_stringl(ekeys, erealloc(eks[i], eksl[i] + 1), eksl[i], 0);
			eks[i] = NULL;
		}
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
	}
  	else
		efree(buf);

	RETVAL_LONG(len1 + len2);

clean_exit:
	for (i=0; i<nkeys; i++) {
		if (key_resources[i] == -1)
			EVP_PKEY_free(pkeys[i]);
		if (eks[i])
			efree(eks[i]);
	}
	efree(eks);
	efree(eksl);
	efree(pkeys);
	efree(key_resources);
}
/* }}} */

/* {{{ proto bool openssl_open(string data, &string opendata, string ekey, mixed privkey)
   Open data */
PHP_FUNCTION(openssl_open)
{
	zval *privkey, *opendata;
	EVP_PKEY *pkey;
	int len1, len2;
	unsigned char *buf;
	long keyresource = -1;
	EVP_CIPHER_CTX ctx;
	char * data;	long data_len;
	char * ekey;	long ekey_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szsz", &data, &data_len,
				&opendata, &ekey, &ekey_len, &privkey) == FAILURE)
		return;

	pkey = php_openssl_evp_from_zval(&privkey, 0, "", 0, &keyresource TSRMLS_CC);
	if (pkey == NULL)	{
		zend_error(E_WARNING, "%s(): unable to coerce parameter 4 into a private key",
				get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}
	buf = emalloc(data_len + 1);

	if (EVP_OpenInit(&ctx, EVP_rc4(), ekey, ekey_len, NULL, pkey)
#if OPENSSL_VERSION_NUMBER >= 0x0090600fL
			&& EVP_OpenUpdate(&ctx, buf, &len1, data, data_len)
#endif
		) {
#if OPENSSL_VERSION_NUMBER < 0x0090600fL
		EVP_OpenUpdate(&ctx, buf, &len1, data, data_len);
#endif
		if (!EVP_OpenFinal(&ctx, buf + len1, &len2) ||
				(len1 + len2 == 0)) {
			efree(buf);
			if (keyresource == -1)
				EVP_PKEY_free(pkey);
			RETURN_FALSE;
		}
	} else {
		efree(buf);
		if (keyresource == -1)
			EVP_PKEY_free(pkey);

		RETURN_FALSE;
	}
	if (keyresource == -1)
		EVP_PKEY_free(pkey);

	zval_dtor(opendata);
	buf[len1 + len2] = '\0';
	ZVAL_STRINGL(opendata, erealloc(buf, len1 + len2 + 1), len1 + len2, 0);
	RETURN_TRUE;
}
/* }}} */

/* {{{ _php_pkey_free() */
static void _php_pkey_free(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	EVP_PKEY *pkey = (EVP_PKEY *)rsrc->ptr;
	EVP_PKEY_free(pkey);
}
/* }}} */

/* {{{ _php_x509_free() */
static void _php_x509_free(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	X509 *x509 = (X509 *)rsrc->ptr;
	X509_free(x509);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 8
 * c-basic-offset: 8
 * End:
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim<600: sw=4 ts=4 tw=78
 */
