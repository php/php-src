/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Venaas <venaas@php.net>                                |
   |          Wez Furlong <wez@thebrainroom.com>                          |
   |          Sascha Kettler <kettler@gmx.net>                            |
   |          Pierre-Alain Joye <pierre@php.net>                          |
   |          Marc Delling <delling@silpion.de> (PKCS12 functions)        |		
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_openssl.h"

/* PHP Includes */
#include "ext/standard/file.h"
#include "ext/standard/info.h"
#include "ext/standard/php_fopen_wrappers.h"
#include "ext/standard/md5.h"
#include "ext/standard/base64.h"
#ifdef PHP_WIN32
# include "win32/winutil.h"
#endif

/* OpenSSL includes */
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/crypto.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/conf.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/pkcs12.h>

/* Common */
#include <time.h>

#ifdef NETWARE
#define timezone _timezone	/* timezone is called _timezone in LibC */
#endif

#define DEFAULT_KEY_LENGTH	512
#define MIN_KEY_LENGTH		384

#define OPENSSL_ALGO_SHA1 	1
#define OPENSSL_ALGO_MD5	2
#define OPENSSL_ALGO_MD4	3
#ifdef HAVE_OPENSSL_MD2_H
#define OPENSSL_ALGO_MD2	4
#endif
#define OPENSSL_ALGO_DSS1	5
#if OPENSSL_VERSION_NUMBER >= 0x0090708fL
#define OPENSSL_ALGO_SHA224 6
#define OPENSSL_ALGO_SHA256 7
#define OPENSSL_ALGO_SHA384 8
#define OPENSSL_ALGO_SHA512 9
#define OPENSSL_ALGO_RMD160 10
#endif
#define DEBUG_SMIME	0

#if !defined(OPENSSL_NO_EC) && defined(EVP_PKEY_EC)
#define HAVE_EVP_PKEY_EC 1
#endif

/* FIXME: Use the openssl constants instead of
 * enum. It is now impossible to match real values
 * against php constants. Also sorry to break the
 * enum principles here, BC...
 */
enum php_openssl_key_type {
	OPENSSL_KEYTYPE_RSA,
	OPENSSL_KEYTYPE_DSA,
	OPENSSL_KEYTYPE_DH,
	OPENSSL_KEYTYPE_DEFAULT = OPENSSL_KEYTYPE_RSA,
#ifdef HAVE_EVP_PKEY_EC
	OPENSSL_KEYTYPE_EC = OPENSSL_KEYTYPE_DH +1
#endif
};

enum php_openssl_cipher_type {
	PHP_OPENSSL_CIPHER_RC2_40,
	PHP_OPENSSL_CIPHER_RC2_128,
	PHP_OPENSSL_CIPHER_RC2_64,
	PHP_OPENSSL_CIPHER_DES,
	PHP_OPENSSL_CIPHER_3DES,
	PHP_OPENSSL_CIPHER_AES_128_CBC,
	PHP_OPENSSL_CIPHER_AES_192_CBC,
	PHP_OPENSSL_CIPHER_AES_256_CBC,

	PHP_OPENSSL_CIPHER_DEFAULT = PHP_OPENSSL_CIPHER_RC2_40
};

PHP_FUNCTION(openssl_get_md_methods);
PHP_FUNCTION(openssl_get_cipher_methods);

PHP_FUNCTION(openssl_digest);
PHP_FUNCTION(openssl_encrypt);
PHP_FUNCTION(openssl_decrypt);
PHP_FUNCTION(openssl_cipher_iv_length);

PHP_FUNCTION(openssl_dh_compute_key);
PHP_FUNCTION(openssl_random_pseudo_bytes);

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_x509_export_to_file, 0, 0, 2)
	ZEND_ARG_INFO(0, x509)
	ZEND_ARG_INFO(0, outfilename)
	ZEND_ARG_INFO(0, notext)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_x509_export, 0, 0, 2)
	ZEND_ARG_INFO(0, x509)
	ZEND_ARG_INFO(1, out)
	ZEND_ARG_INFO(0, notext)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_x509_fingerprint, 0, 0, 1)
	ZEND_ARG_INFO(0, x509)
	ZEND_ARG_INFO(0, method)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_x509_check_private_key, 0)
	ZEND_ARG_INFO(0, cert)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_x509_parse, 0)
	ZEND_ARG_INFO(0, x509)
	ZEND_ARG_INFO(0, shortname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_x509_checkpurpose, 0, 0, 3)
	ZEND_ARG_INFO(0, x509cert)
	ZEND_ARG_INFO(0, purpose)
	ZEND_ARG_INFO(0, cainfo) /* array */
	ZEND_ARG_INFO(0, untrustedfile)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_x509_read, 0)
	ZEND_ARG_INFO(0, cert)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_x509_free, 0)
	ZEND_ARG_INFO(0, x509)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_pkcs12_export_to_file, 0, 0, 4)
	ZEND_ARG_INFO(0, x509)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, priv_key)
	ZEND_ARG_INFO(0, pass)
	ZEND_ARG_INFO(0, args) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_pkcs12_export, 0)
	ZEND_ARG_INFO(0, x509)
	ZEND_ARG_INFO(1, out)
	ZEND_ARG_INFO(0, priv_key)
	ZEND_ARG_INFO(0, pass)
	ZEND_ARG_INFO(0, args) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_pkcs12_read, 0)
	ZEND_ARG_INFO(0, PKCS12)
	ZEND_ARG_INFO(1, certs) /* array */
	ZEND_ARG_INFO(0, pass)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_csr_export_to_file, 0, 0, 2)
	ZEND_ARG_INFO(0, csr)
	ZEND_ARG_INFO(0, outfilename)
	ZEND_ARG_INFO(0, notext)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_csr_export, 0, 0, 2)
	ZEND_ARG_INFO(0, csr)
	ZEND_ARG_INFO(1, out)
	ZEND_ARG_INFO(0, notext)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_csr_sign, 0, 0, 4)
	ZEND_ARG_INFO(0, csr)
	ZEND_ARG_INFO(0, x509)
	ZEND_ARG_INFO(0, priv_key)
	ZEND_ARG_INFO(0, days)
	ZEND_ARG_INFO(0, config_args) /* array */
	ZEND_ARG_INFO(0, serial)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_csr_new, 0, 0, 2)
	ZEND_ARG_INFO(0, dn) /* array */
	ZEND_ARG_INFO(1, privkey)
	ZEND_ARG_INFO(0, configargs)
	ZEND_ARG_INFO(0, extraattribs)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_csr_get_subject, 0)
	ZEND_ARG_INFO(0, csr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_csr_get_public_key, 0)
	ZEND_ARG_INFO(0, csr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_pkey_new, 0, 0, 0)
	ZEND_ARG_INFO(0, configargs) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_pkey_export_to_file, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, outfilename)
	ZEND_ARG_INFO(0, passphrase)
	ZEND_ARG_INFO(0, config_args) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_pkey_export, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(1, out)
	ZEND_ARG_INFO(0, passphrase)
	ZEND_ARG_INFO(0, config_args) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_pkey_get_public, 0)
	ZEND_ARG_INFO(0, cert)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_pkey_free, 0)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_pkey_get_private, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, passphrase)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_pkey_get_details, 0)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

#if OPENSSL_VERSION_NUMBER >= 0x10000000L
ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_pbkdf2, 0, 0, 4)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, salt)
	ZEND_ARG_INFO(0, key_length)
	ZEND_ARG_INFO(0, iterations)
	ZEND_ARG_INFO(0, digest_algorithm)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_pkcs7_verify, 0, 0, 2)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, signerscerts)
	ZEND_ARG_INFO(0, cainfo) /* array */
	ZEND_ARG_INFO(0, extracerts)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_pkcs7_encrypt, 0, 0, 4)
	ZEND_ARG_INFO(0, infile)
	ZEND_ARG_INFO(0, outfile)
	ZEND_ARG_INFO(0, recipcerts)
	ZEND_ARG_INFO(0, headers) /* array */
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, cipher)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_pkcs7_sign, 0, 0, 5)
	ZEND_ARG_INFO(0, infile)
	ZEND_ARG_INFO(0, outfile)
	ZEND_ARG_INFO(0, signcert)
	ZEND_ARG_INFO(0, signkey)
	ZEND_ARG_INFO(0, headers) /* array */
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, extracertsfilename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_pkcs7_decrypt, 0, 0, 3)
	ZEND_ARG_INFO(0, infilename)
	ZEND_ARG_INFO(0, outfilename)
	ZEND_ARG_INFO(0, recipcert)
	ZEND_ARG_INFO(0, recipkey)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_private_encrypt, 0, 0, 3)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(1, crypted)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, padding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_private_decrypt, 0, 0, 3)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(1, crypted)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, padding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_public_encrypt, 0, 0, 3)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(1, crypted)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, padding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_public_decrypt, 0, 0, 3)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(1, crypted)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, padding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_error_string, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_sign, 0, 0, 3)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(1, signature)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_verify, 0, 0, 3)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, signature)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_seal, 0, 0, 4)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(1, sealdata)
	ZEND_ARG_INFO(1, ekeys) /* arary */
	ZEND_ARG_INFO(0, pubkeys) /* array */
	ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_open, 0)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(1, opendata)
	ZEND_ARG_INFO(0, ekey)
	ZEND_ARG_INFO(0, privkey)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_get_md_methods, 0, 0, 0)
	ZEND_ARG_INFO(0, aliases)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_get_cipher_methods, 0, 0, 0)
	ZEND_ARG_INFO(0, aliases)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_digest, 0, 0, 2)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, method)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_encrypt, 0, 0, 3)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, method)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, iv)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_decrypt, 0, 0, 3)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, method)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, iv)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_cipher_iv_length, 0)
	ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_dh_compute_key, 0)
	ZEND_ARG_INFO(0, pub_key)
	ZEND_ARG_INFO(0, dh_key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_random_pseudo_bytes, 0, 0, 1)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(1, result_is_strong)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_openssl_spki_new, 0, 0, 2)
	ZEND_ARG_INFO(0, privkey)
	ZEND_ARG_INFO(0, challenge)
	ZEND_ARG_INFO(0, algo)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_spki_verify, 0)
	ZEND_ARG_INFO(0, spki)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_spki_export, 0)
	ZEND_ARG_INFO(0, spki)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_spki_export_challenge, 0)
	ZEND_ARG_INFO(0, spki)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_openssl_get_cert_locations, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ openssl_functions[]
 */
const zend_function_entry openssl_functions[] = {
	PHP_FE(openssl_get_cert_locations, arginfo_openssl_get_cert_locations)

/* spki functions */
	PHP_FE(openssl_spki_new, arginfo_openssl_spki_new)
	PHP_FE(openssl_spki_verify, arginfo_openssl_spki_verify)
	PHP_FE(openssl_spki_export, arginfo_openssl_spki_export)
	PHP_FE(openssl_spki_export_challenge, arginfo_openssl_spki_export_challenge)

/* public/private key functions */
	PHP_FE(openssl_pkey_free,			arginfo_openssl_pkey_free)
	PHP_FE(openssl_pkey_new,			arginfo_openssl_pkey_new)
	PHP_FE(openssl_pkey_export,			arginfo_openssl_pkey_export)
	PHP_FE(openssl_pkey_export_to_file,	arginfo_openssl_pkey_export_to_file)
	PHP_FE(openssl_pkey_get_private,	arginfo_openssl_pkey_get_private)
	PHP_FE(openssl_pkey_get_public,		arginfo_openssl_pkey_get_public)
	PHP_FE(openssl_pkey_get_details,	arginfo_openssl_pkey_get_details)

	PHP_FALIAS(openssl_free_key,		openssl_pkey_free, 			arginfo_openssl_pkey_free)
	PHP_FALIAS(openssl_get_privatekey,	openssl_pkey_get_private,	arginfo_openssl_pkey_get_private)
	PHP_FALIAS(openssl_get_publickey,	openssl_pkey_get_public,	arginfo_openssl_pkey_get_public)

/* x.509 cert funcs */
	PHP_FE(openssl_x509_read,				arginfo_openssl_x509_read)
	PHP_FE(openssl_x509_free,          		arginfo_openssl_x509_free)
	PHP_FE(openssl_x509_parse,			 	arginfo_openssl_x509_parse)
	PHP_FE(openssl_x509_checkpurpose,		arginfo_openssl_x509_checkpurpose)
	PHP_FE(openssl_x509_check_private_key,	arginfo_openssl_x509_check_private_key)
	PHP_FE(openssl_x509_export,				arginfo_openssl_x509_export)
	PHP_FE(openssl_x509_fingerprint,			arginfo_openssl_x509_fingerprint)
	PHP_FE(openssl_x509_export_to_file,		arginfo_openssl_x509_export_to_file)

/* PKCS12 funcs */
	PHP_FE(openssl_pkcs12_export,			arginfo_openssl_pkcs12_export)
	PHP_FE(openssl_pkcs12_export_to_file,	arginfo_openssl_pkcs12_export_to_file)
	PHP_FE(openssl_pkcs12_read,				arginfo_openssl_pkcs12_read)

/* CSR funcs */
	PHP_FE(openssl_csr_new,				arginfo_openssl_csr_new)
	PHP_FE(openssl_csr_export,			arginfo_openssl_csr_export)
	PHP_FE(openssl_csr_export_to_file,	arginfo_openssl_csr_export_to_file)
	PHP_FE(openssl_csr_sign,			arginfo_openssl_csr_sign)
	PHP_FE(openssl_csr_get_subject,		arginfo_openssl_csr_get_subject)
	PHP_FE(openssl_csr_get_public_key,	arginfo_openssl_csr_get_public_key)

	PHP_FE(openssl_digest,				arginfo_openssl_digest)
	PHP_FE(openssl_encrypt,				arginfo_openssl_encrypt)
	PHP_FE(openssl_decrypt,				arginfo_openssl_decrypt)
	PHP_FE(openssl_cipher_iv_length,	arginfo_openssl_cipher_iv_length)
	PHP_FE(openssl_sign,				arginfo_openssl_sign)
	PHP_FE(openssl_verify,				arginfo_openssl_verify)
	PHP_FE(openssl_seal,				arginfo_openssl_seal)
	PHP_FE(openssl_open,				arginfo_openssl_open)

#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	PHP_FE(openssl_pbkdf2,	arginfo_openssl_pbkdf2)
#endif

/* for S/MIME handling */
	PHP_FE(openssl_pkcs7_verify,		arginfo_openssl_pkcs7_verify)
	PHP_FE(openssl_pkcs7_decrypt,		arginfo_openssl_pkcs7_decrypt)
	PHP_FE(openssl_pkcs7_sign,			arginfo_openssl_pkcs7_sign)
	PHP_FE(openssl_pkcs7_encrypt,		arginfo_openssl_pkcs7_encrypt)

	PHP_FE(openssl_private_encrypt,		arginfo_openssl_private_encrypt)
	PHP_FE(openssl_private_decrypt,		arginfo_openssl_private_decrypt)
	PHP_FE(openssl_public_encrypt,		arginfo_openssl_public_encrypt)
	PHP_FE(openssl_public_decrypt,		arginfo_openssl_public_decrypt)

	PHP_FE(openssl_get_md_methods,		arginfo_openssl_get_md_methods)
	PHP_FE(openssl_get_cipher_methods,	arginfo_openssl_get_cipher_methods)

	PHP_FE(openssl_dh_compute_key,      arginfo_openssl_dh_compute_key)

	PHP_FE(openssl_random_pseudo_bytes,    arginfo_openssl_random_pseudo_bytes)
	PHP_FE(openssl_error_string, arginfo_openssl_error_string)
	PHP_FE_END
};
/* }}} */

/* {{{ openssl_module_entry
 */
zend_module_entry openssl_module_entry = {
	STANDARD_MODULE_HEADER,
	"openssl",
	openssl_functions,
	PHP_MINIT(openssl),
	PHP_MSHUTDOWN(openssl),
	NULL,
	NULL,
	PHP_MINFO(openssl),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_OPENSSL
ZEND_GET_MODULE(openssl)
#endif

static int le_key;
static int le_x509;
static int le_csr;
static int ssl_stream_data_index;

int php_openssl_get_x509_list_id(void) /* {{{ */
{
	return le_x509;
}
/* }}} */

/* {{{ resource destructors */
static void php_pkey_free(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	EVP_PKEY *pkey = (EVP_PKEY *)rsrc->ptr;

	assert(pkey != NULL);

	EVP_PKEY_free(pkey);
}

static void php_x509_free(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	X509 *x509 = (X509 *)rsrc->ptr;
	X509_free(x509);
}

static void php_csr_free(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	X509_REQ * csr = (X509_REQ*)rsrc->ptr;
	X509_REQ_free(csr);
}
/* }}} */

/* {{{ openssl open_basedir check */
inline static int php_openssl_open_base_dir_chk(char *filename TSRMLS_DC)
{
	if (php_check_open_basedir(filename TSRMLS_CC)) {
		return -1;
	}
	
	return 0;
}
/* }}} */

php_stream* php_openssl_get_stream_from_ssl_handle(const SSL *ssl)
{
	return (php_stream*)SSL_get_ex_data(ssl, ssl_stream_data_index);
}

int php_openssl_get_ssl_stream_data_index()
{
	return ssl_stream_data_index;
}

/* openssl -> PHP "bridging" */
/* true global; readonly after module startup */
static char default_ssl_conf_filename[MAXPATHLEN];

struct php_x509_request { /* {{{ */
#if OPENSSL_VERSION_NUMBER >= 0x10000002L
	LHASH_OF(CONF_VALUE) * global_config;	/* Global SSL config */
	LHASH_OF(CONF_VALUE) * req_config;		/* SSL config for this request */
#else
	LHASH * global_config;  /* Global SSL config */
	LHASH * req_config;             /* SSL config for this request */
#endif
	const EVP_MD * md_alg;
	const EVP_MD * digest;
	char	* section_name,
			* config_filename,
			* digest_name,
			* extensions_section,
			* request_extensions_section;
	int priv_key_bits;
	int priv_key_type;

	int priv_key_encrypt;

	EVP_PKEY * priv_key;

    const EVP_CIPHER * priv_key_encrypt_cipher;
};
/* }}} */

static X509 * php_openssl_x509_from_zval(zval ** val, int makeresource, long * resourceval TSRMLS_DC);
static EVP_PKEY * php_openssl_evp_from_zval(zval ** val, int public_key, char * passphrase, int makeresource, long * resourceval TSRMLS_DC);
static int php_openssl_is_private_key(EVP_PKEY* pkey TSRMLS_DC);
static X509_STORE * setup_verify(zval * calist TSRMLS_DC);
static STACK_OF(X509) * load_all_certs_from_file(char *certfile);
static X509_REQ * php_openssl_csr_from_zval(zval ** val, int makeresource, long * resourceval TSRMLS_DC);
static EVP_PKEY * php_openssl_generate_private_key(struct php_x509_request * req TSRMLS_DC);

static void add_assoc_name_entry(zval * val, char * key, X509_NAME * name, int shortname TSRMLS_DC) /* {{{ */
{
	zval **data;
	zval *subitem, *subentries;
	int i;
	char *sname;
	int nid;
	X509_NAME_ENTRY * ne;
	ASN1_STRING * str = NULL;
	ASN1_OBJECT * obj;

	if (key != NULL) {
		MAKE_STD_ZVAL(subitem);
		array_init(subitem);
	} else {
		subitem = val;
	}
	
	for (i = 0; i < X509_NAME_entry_count(name); i++) {
		unsigned char *to_add;
		int to_add_len = 0;


		ne  = X509_NAME_get_entry(name, i);
		obj = X509_NAME_ENTRY_get_object(ne);
		nid = OBJ_obj2nid(obj);

		if (shortname) {
			sname = (char *) OBJ_nid2sn(nid);
		} else {
			sname = (char *) OBJ_nid2ln(nid);
		}

		str = X509_NAME_ENTRY_get_data(ne);
		if (ASN1_STRING_type(str) != V_ASN1_UTF8STRING) {
			to_add_len = ASN1_STRING_to_UTF8(&to_add, str);
		} else {
			to_add = ASN1_STRING_data(str);
			to_add_len = ASN1_STRING_length(str);
		}

		if (to_add_len != -1) {
			if (zend_hash_find(Z_ARRVAL_P(subitem), sname, strlen(sname)+1, (void**)&data) == SUCCESS) {
				if (Z_TYPE_PP(data) == IS_ARRAY) {
					subentries = *data;
					add_next_index_stringl(subentries, (char *)to_add, to_add_len, 1);
				} else if (Z_TYPE_PP(data) == IS_STRING) {
					MAKE_STD_ZVAL(subentries);
					array_init(subentries);
					add_next_index_stringl(subentries, Z_STRVAL_PP(data), Z_STRLEN_PP(data), 1);
					add_next_index_stringl(subentries, (char *)to_add, to_add_len, 1);
					zend_hash_update(Z_ARRVAL_P(subitem), sname, strlen(sname)+1, &subentries, sizeof(zval*), NULL);
				}
			} else {
				add_assoc_stringl(subitem, sname, (char *)to_add, to_add_len, 1);
			}
		}
	}
	if (key != NULL) {
		zend_hash_update(HASH_OF(val), key, strlen(key) + 1, (void *)&subitem, sizeof(subitem), NULL);
	}
}
/* }}} */

static void add_assoc_asn1_string(zval * val, char * key, ASN1_STRING * str) /* {{{ */
{
	add_assoc_stringl(val, key, (char *)str->data, str->length, 1);
}
/* }}} */

static time_t asn1_time_to_time_t(ASN1_UTCTIME * timestr TSRMLS_DC) /* {{{ */
{
/*
	This is how the time string is formatted:

   snprintf(p, sizeof(p), "%02d%02d%02d%02d%02d%02dZ",ts->tm_year%100,
      ts->tm_mon+1,ts->tm_mday,ts->tm_hour,ts->tm_min,ts->tm_sec);
*/

	time_t ret;
	struct tm thetime;
	char * strbuf;
	char * thestr;
	long gmadjust = 0;

	if (ASN1_STRING_type(timestr) != V_ASN1_UTCTIME && ASN1_STRING_type(timestr) != V_ASN1_GENERALIZEDTIME) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "illegal ASN1 data type for timestamp");
		return (time_t)-1;
	}

	if (ASN1_STRING_length(timestr) != strlen((const char*)ASN1_STRING_data(timestr))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "illegal length in timestamp");
		return (time_t)-1;
	}

	if (ASN1_STRING_length(timestr) < 13) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to parse time string %s correctly", timestr->data);
		return (time_t)-1;
	}

	if (ASN1_STRING_type(timestr) == V_ASN1_GENERALIZEDTIME && ASN1_STRING_length(timestr) < 15) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to parse time string %s correctly", timestr->data);
		return (time_t)-1;
	}

	strbuf = estrdup((char *)ASN1_STRING_data(timestr));

	memset(&thetime, 0, sizeof(thetime));

	/* we work backwards so that we can use atoi more easily */

	thestr = strbuf + ASN1_STRING_length(timestr) - 3;

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
	if( ASN1_STRING_type(timestr) == V_ASN1_UTCTIME ) {
		thestr -= 2;
		thetime.tm_year = atoi(thestr);

		if (thetime.tm_year < 68) {
			thetime.tm_year += 100;
		}
	} else if( ASN1_STRING_type(timestr) == V_ASN1_GENERALIZEDTIME ) {
		thestr -= 4;
		thetime.tm_year = atoi(thestr) - 1900;
	}


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
	gmadjust = -(thetime.tm_isdst ? (long)timezone - 3600 : (long)timezone + 3600);
#endif
	ret += gmadjust;

	efree(strbuf);

	return ret;
}
/* }}} */

#if OPENSSL_VERSION_NUMBER >= 0x10000002L
static inline int php_openssl_config_check_syntax(const char * section_label, const char * config_filename, const char * section, LHASH_OF(CONF_VALUE) * config TSRMLS_DC) /* {{{ */
#else
static inline int php_openssl_config_check_syntax(const char * section_label, const char * config_filename, const char * section, LHASH * config TSRMLS_DC)
#endif
{
	X509V3_CTX ctx;
	
	X509V3_set_ctx_test(&ctx);
	X509V3_set_conf_lhash(&ctx, config);
	if (!X509V3_EXT_add_conf(config, &ctx, (char *)section, NULL)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error loading %s section %s of %s",
				section_label,
				section,
				config_filename);
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

static int add_oid_section(struct php_x509_request * req TSRMLS_DC) /* {{{ */
{
	char * str;
	STACK_OF(CONF_VALUE) * sktmp;
	CONF_VALUE * cnf;
	int i;

	str = CONF_get_string(req->req_config, NULL, "oid_section");
	if (str == NULL) {
		return SUCCESS;
	}
	sktmp = CONF_get_section(req->req_config, str);
	if (sktmp == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "problem loading oid section %s", str);
		return FAILURE;
	}
	for (i = 0; i < sk_CONF_VALUE_num(sktmp); i++) {
		cnf = sk_CONF_VALUE_value(sktmp, i);
		if (OBJ_create(cnf->value, cnf->name, cnf->name) == NID_undef) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "problem creating object %s=%s", cnf->name, cnf->value);
			return FAILURE;
		}
	}
	return SUCCESS;
}
/* }}} */

#define PHP_SSL_REQ_INIT(req)		memset(req, 0, sizeof(*req))
#define PHP_SSL_REQ_DISPOSE(req)	php_openssl_dispose_config(req TSRMLS_CC)
#define PHP_SSL_REQ_PARSE(req, zval)	php_openssl_parse_config(req, zval TSRMLS_CC)

#define PHP_SSL_CONFIG_SYNTAX_CHECK(var) if (req->var && php_openssl_config_check_syntax(#var, \
			req->config_filename, req->var, req->req_config TSRMLS_CC) == FAILURE) return FAILURE

#define SET_OPTIONAL_STRING_ARG(key, varname, defval)	\
	if (optional_args && zend_hash_find(Z_ARRVAL_P(optional_args), key, sizeof(key), (void**)&item) == SUCCESS && Z_TYPE_PP(item) == IS_STRING) \
		varname = Z_STRVAL_PP(item); \
	else \
		varname = defval

#define SET_OPTIONAL_LONG_ARG(key, varname, defval)	\
	if (optional_args && zend_hash_find(Z_ARRVAL_P(optional_args), key, sizeof(key), (void**)&item) == SUCCESS && Z_TYPE_PP(item) == IS_LONG) \
		varname = Z_LVAL_PP(item); \
	else \
		varname = defval

static const EVP_CIPHER * php_openssl_get_evp_cipher_from_algo(long algo);

int openssl_spki_cleanup(const char *src, char *dest);

static int php_openssl_parse_config(struct php_x509_request * req, zval * optional_args TSRMLS_DC) /* {{{ */
{
	char * str;
	zval ** item;

	SET_OPTIONAL_STRING_ARG("config", req->config_filename, default_ssl_conf_filename);
	SET_OPTIONAL_STRING_ARG("config_section_name", req->section_name, "req");
	req->global_config = CONF_load(NULL, default_ssl_conf_filename, NULL);
	req->req_config = CONF_load(NULL, req->config_filename, NULL);

	if (req->req_config == NULL) {
		return FAILURE;
	}

	/* read in the oids */
	str = CONF_get_string(req->req_config, NULL, "oid_file");
	if (str && !php_openssl_open_base_dir_chk(str TSRMLS_CC)) {
		BIO *oid_bio = BIO_new_file(str, "r");
		if (oid_bio) {
			OBJ_create_objects(oid_bio);
			BIO_free(oid_bio);
		}
	}
	if (add_oid_section(req TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}
	SET_OPTIONAL_STRING_ARG("digest_alg", req->digest_name,
		CONF_get_string(req->req_config, req->section_name, "default_md"));
	SET_OPTIONAL_STRING_ARG("x509_extensions", req->extensions_section,
		CONF_get_string(req->req_config, req->section_name, "x509_extensions"));
	SET_OPTIONAL_STRING_ARG("req_extensions", req->request_extensions_section,
		CONF_get_string(req->req_config, req->section_name, "req_extensions"));
	SET_OPTIONAL_LONG_ARG("private_key_bits", req->priv_key_bits,
		CONF_get_number(req->req_config, req->section_name, "default_bits"));

	SET_OPTIONAL_LONG_ARG("private_key_type", req->priv_key_type, OPENSSL_KEYTYPE_DEFAULT);

	if (optional_args && zend_hash_find(Z_ARRVAL_P(optional_args), "encrypt_key", sizeof("encrypt_key"), (void**)&item) == SUCCESS) {
		req->priv_key_encrypt = Z_BVAL_PP(item);
	} else {
		str = CONF_get_string(req->req_config, req->section_name, "encrypt_rsa_key");
		if (str == NULL) {
			str = CONF_get_string(req->req_config, req->section_name, "encrypt_key");
		}
		if (str && strcmp(str, "no") == 0) {
			req->priv_key_encrypt = 0;
		} else {
			req->priv_key_encrypt = 1;
		}
	}

	if (req->priv_key_encrypt && optional_args && zend_hash_find(Z_ARRVAL_P(optional_args), "encrypt_key_cipher", sizeof("encrypt_key_cipher"), (void**)&item) == SUCCESS 
		&& Z_TYPE_PP(item) == IS_LONG) {
		long cipher_algo = Z_LVAL_PP(item);
		const EVP_CIPHER* cipher = php_openssl_get_evp_cipher_from_algo(cipher_algo);
		if (cipher == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown cipher algorithm for private key.");
			return FAILURE;
		} else  {
			req->priv_key_encrypt_cipher = cipher;
		}
	} else {
		req->priv_key_encrypt_cipher = NULL;
	}


	
	/* digest alg */
	if (req->digest_name == NULL) {
		req->digest_name = CONF_get_string(req->req_config, req->section_name, "default_md");
	}
	if (req->digest_name) {
		req->digest = req->md_alg = EVP_get_digestbyname(req->digest_name);
	}
	if (req->md_alg == NULL) {
		req->md_alg = req->digest = EVP_sha1();
	}

	PHP_SSL_CONFIG_SYNTAX_CHECK(extensions_section);

	/* set the string mask */
	str = CONF_get_string(req->req_config, req->section_name, "string_mask");
	if (str && !ASN1_STRING_set_default_mask_asc(str)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid global string mask setting %s", str);
		return FAILURE;
	}

	PHP_SSL_CONFIG_SYNTAX_CHECK(request_extensions_section);
	
	return SUCCESS;
}
/* }}} */

static void php_openssl_dispose_config(struct php_x509_request * req TSRMLS_DC) /* {{{ */
{
	if (req->priv_key) {
		EVP_PKEY_free(req->priv_key);
		req->priv_key = NULL;
	}
	if (req->global_config) {
		CONF_free(req->global_config);
		req->global_config = NULL;
	}
	if (req->req_config) {
		CONF_free(req->req_config);
		req->req_config = NULL;
	}
}
/* }}} */

#ifdef PHP_WIN32
#define PHP_OPENSSL_RAND_ADD_TIME() ((void) 0)
#else
#define PHP_OPENSSL_RAND_ADD_TIME() php_openssl_rand_add_timeval()

static inline void php_openssl_rand_add_timeval()  /* {{{ */
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	RAND_add(&tv, sizeof(tv), 0.0);
}
/* }}} */

#endif

static int php_openssl_load_rand_file(const char * file, int *egdsocket, int *seeded TSRMLS_DC) /* {{{ */
{
	char buffer[MAXPATHLEN];

	*egdsocket = 0;
	*seeded = 0;

	if (file == NULL) {
		file = RAND_file_name(buffer, sizeof(buffer));
#ifdef HAVE_RAND_EGD
	} else if (RAND_egd(file) > 0) {
		/* if the given filename is an EGD socket, don't
		 * write anything back to it */
		*egdsocket = 1;
		return SUCCESS;
#endif
	}
	if (file == NULL || !RAND_load_file(file, -1)) {
		if (RAND_status() == 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to load random state; not enough random data!");
			return FAILURE;
		}
		return FAILURE;
	}
	*seeded = 1;
	return SUCCESS;
}
/* }}} */

static int php_openssl_write_rand_file(const char * file, int egdsocket, int seeded) /* {{{ */
{
	char buffer[MAXPATHLEN];

	TSRMLS_FETCH();

	if (egdsocket || !seeded) {
		/* if we did not manage to read the seed file, we should not write
		 * a low-entropy seed file back */
		return FAILURE;
	}
	if (file == NULL) {
		file = RAND_file_name(buffer, sizeof(buffer));
	}
	PHP_OPENSSL_RAND_ADD_TIME();
	if (file == NULL || !RAND_write_file(file)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to write random state");
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

static EVP_MD * php_openssl_get_evp_md_from_algo(long algo) { /* {{{ */
	EVP_MD *mdtype;

	switch (algo) {
		case OPENSSL_ALGO_SHA1:
			mdtype = (EVP_MD *) EVP_sha1();
			break;
		case OPENSSL_ALGO_MD5:
			mdtype = (EVP_MD *) EVP_md5();
			break;
		case OPENSSL_ALGO_MD4:
			mdtype = (EVP_MD *) EVP_md4();
			break;
#ifdef HAVE_OPENSSL_MD2_H
		case OPENSSL_ALGO_MD2:
			mdtype = (EVP_MD *) EVP_md2();
			break;
#endif
		case OPENSSL_ALGO_DSS1:
			mdtype = (EVP_MD *) EVP_dss1();
			break;
#if OPENSSL_VERSION_NUMBER >= 0x0090708fL
		case OPENSSL_ALGO_SHA224:
			mdtype = (EVP_MD *) EVP_sha224();
			break;
		case OPENSSL_ALGO_SHA256:
			mdtype = (EVP_MD *) EVP_sha256();
			break;
		case OPENSSL_ALGO_SHA384:
			mdtype = (EVP_MD *) EVP_sha384();
			break;
		case OPENSSL_ALGO_SHA512:
			mdtype = (EVP_MD *) EVP_sha512();
			break;
		case OPENSSL_ALGO_RMD160:
			mdtype = (EVP_MD *) EVP_ripemd160();
			break;
#endif
		default:
			return NULL;
			break;
	}
	return mdtype;
}
/* }}} */

static const EVP_CIPHER * php_openssl_get_evp_cipher_from_algo(long algo) { /* {{{ */
	switch (algo) {
#ifndef OPENSSL_NO_RC2
		case PHP_OPENSSL_CIPHER_RC2_40:
			return EVP_rc2_40_cbc();
			break;
		case PHP_OPENSSL_CIPHER_RC2_64:
			return EVP_rc2_64_cbc();
			break;
		case PHP_OPENSSL_CIPHER_RC2_128:
			return EVP_rc2_cbc();
			break;
#endif

#ifndef OPENSSL_NO_DES
		case PHP_OPENSSL_CIPHER_DES:
			return EVP_des_cbc();
			break;
		case PHP_OPENSSL_CIPHER_3DES:
			return EVP_des_ede3_cbc();
			break;
#endif

#ifndef OPENSSL_NO_AES
		case PHP_OPENSSL_CIPHER_AES_128_CBC:
			return EVP_aes_128_cbc();
			break;
		case PHP_OPENSSL_CIPHER_AES_192_CBC:
			return EVP_aes_192_cbc();
			break;
		case PHP_OPENSSL_CIPHER_AES_256_CBC:
			return EVP_aes_256_cbc();
			break;
#endif


		default:
			return NULL;
			break;
	}
}
/* }}} */

/* {{{ INI Settings */
PHP_INI_BEGIN()
	PHP_INI_ENTRY("openssl.cafile", NULL, PHP_INI_PERDIR, NULL)
	PHP_INI_ENTRY("openssl.capath", NULL, PHP_INI_PERDIR, NULL)
PHP_INI_END()
/* }}} */
 
/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(openssl)
{
	char * config_filename;

	le_key = zend_register_list_destructors_ex(php_pkey_free, NULL, "OpenSSL key", module_number);
	le_x509 = zend_register_list_destructors_ex(php_x509_free, NULL, "OpenSSL X.509", module_number);
	le_csr = zend_register_list_destructors_ex(php_csr_free, NULL, "OpenSSL X.509 CSR", module_number);

	SSL_library_init();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
	OpenSSL_add_all_algorithms();

	SSL_load_error_strings();

	/* register a resource id number with OpenSSL so that we can map SSL -> stream structures in
	 * OpenSSL callbacks */
	ssl_stream_data_index = SSL_get_ex_new_index(0, "PHP stream index", NULL, NULL, NULL);
	
	REGISTER_STRING_CONSTANT("OPENSSL_VERSION_TEXT", OPENSSL_VERSION_TEXT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_VERSION_NUMBER", OPENSSL_VERSION_NUMBER, CONST_CS|CONST_PERSISTENT);
	
	/* purposes for cert purpose checking */
	REGISTER_LONG_CONSTANT("X509_PURPOSE_SSL_CLIENT", X509_PURPOSE_SSL_CLIENT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_SSL_SERVER", X509_PURPOSE_SSL_SERVER, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_NS_SSL_SERVER", X509_PURPOSE_NS_SSL_SERVER, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_SMIME_SIGN", X509_PURPOSE_SMIME_SIGN, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_SMIME_ENCRYPT", X509_PURPOSE_SMIME_ENCRYPT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("X509_PURPOSE_CRL_SIGN", X509_PURPOSE_CRL_SIGN, CONST_CS|CONST_PERSISTENT);
#ifdef X509_PURPOSE_ANY
	REGISTER_LONG_CONSTANT("X509_PURPOSE_ANY", X509_PURPOSE_ANY, CONST_CS|CONST_PERSISTENT);
#endif

	/* signature algorithm constants */
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_SHA1", OPENSSL_ALGO_SHA1, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_MD5", OPENSSL_ALGO_MD5, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_MD4", OPENSSL_ALGO_MD4, CONST_CS|CONST_PERSISTENT);
#ifdef HAVE_OPENSSL_MD2_H
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_MD2", OPENSSL_ALGO_MD2, CONST_CS|CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_DSS1", OPENSSL_ALGO_DSS1, CONST_CS|CONST_PERSISTENT);
#if OPENSSL_VERSION_NUMBER >= 0x0090708fL
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_SHA224", OPENSSL_ALGO_SHA224, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_SHA256", OPENSSL_ALGO_SHA256, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_SHA384", OPENSSL_ALGO_SHA384, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_SHA512", OPENSSL_ALGO_SHA512, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_RMD160", OPENSSL_ALGO_RMD160, CONST_CS|CONST_PERSISTENT);
#endif

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

	REGISTER_LONG_CONSTANT("OPENSSL_PKCS1_PADDING", RSA_PKCS1_PADDING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_SSLV23_PADDING", RSA_SSLV23_PADDING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_NO_PADDING", RSA_NO_PADDING, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_PKCS1_OAEP_PADDING", RSA_PKCS1_OAEP_PADDING, CONST_CS|CONST_PERSISTENT);

	/* Informational stream wrapper constants */
	REGISTER_STRING_CONSTANT("OPENSSL_DEFAULT_STREAM_CIPHERS", OPENSSL_DEFAULT_STREAM_CIPHERS, CONST_CS|CONST_PERSISTENT);

	/* Ciphers */
#ifndef OPENSSL_NO_RC2
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_RC2_40", PHP_OPENSSL_CIPHER_RC2_40, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_RC2_128", PHP_OPENSSL_CIPHER_RC2_128, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_RC2_64", PHP_OPENSSL_CIPHER_RC2_64, CONST_CS|CONST_PERSISTENT);
#endif
#ifndef OPENSSL_NO_DES
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_DES", PHP_OPENSSL_CIPHER_DES, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_3DES", PHP_OPENSSL_CIPHER_3DES, CONST_CS|CONST_PERSISTENT);
#endif
#ifndef OPENSSL_NO_AES
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_AES_128_CBC", PHP_OPENSSL_CIPHER_AES_128_CBC, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_AES_192_CBC", PHP_OPENSSL_CIPHER_AES_192_CBC, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_AES_256_CBC", PHP_OPENSSL_CIPHER_AES_256_CBC, CONST_CS|CONST_PERSISTENT);
#endif
 
	/* Values for key types */
	REGISTER_LONG_CONSTANT("OPENSSL_KEYTYPE_RSA", OPENSSL_KEYTYPE_RSA, CONST_CS|CONST_PERSISTENT);
#ifndef NO_DSA
	REGISTER_LONG_CONSTANT("OPENSSL_KEYTYPE_DSA", OPENSSL_KEYTYPE_DSA, CONST_CS|CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("OPENSSL_KEYTYPE_DH", OPENSSL_KEYTYPE_DH, CONST_CS|CONST_PERSISTENT);
#ifdef HAVE_EVP_PKEY_EC
	REGISTER_LONG_CONSTANT("OPENSSL_KEYTYPE_EC", OPENSSL_KEYTYPE_EC, CONST_CS|CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("OPENSSL_RAW_DATA", OPENSSL_RAW_DATA, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ZERO_PADDING", OPENSSL_ZERO_PADDING, CONST_CS|CONST_PERSISTENT);

#if OPENSSL_VERSION_NUMBER >= 0x0090806fL && !defined(OPENSSL_NO_TLSEXT)
	/* SNI support included in OpenSSL >= 0.9.8j */
	REGISTER_LONG_CONSTANT("OPENSSL_TLSEXT_SERVER_NAME", 1, CONST_CS|CONST_PERSISTENT);
#endif

	/* Determine default SSL configuration file */
	config_filename = getenv("OPENSSL_CONF");
	if (config_filename == NULL) {
		config_filename = getenv("SSLEAY_CONF");
	}

	/* default to 'openssl.cnf' if no environment variable is set */
	if (config_filename == NULL) {
		snprintf(default_ssl_conf_filename, sizeof(default_ssl_conf_filename), "%s/%s",
				X509_get_default_cert_area(),
				"openssl.cnf");
	} else {
		strlcpy(default_ssl_conf_filename, config_filename, sizeof(default_ssl_conf_filename));
	}

	php_stream_xport_register("ssl", php_openssl_ssl_socket_factory TSRMLS_CC);
	php_stream_xport_register("sslv3", php_openssl_ssl_socket_factory TSRMLS_CC);
#ifndef OPENSSL_NO_SSL2
	php_stream_xport_register("sslv2", php_openssl_ssl_socket_factory TSRMLS_CC);
#endif
	php_stream_xport_register("tls", php_openssl_ssl_socket_factory TSRMLS_CC);
	php_stream_xport_register("tlsv1.0", php_openssl_ssl_socket_factory TSRMLS_CC);
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
	php_stream_xport_register("tlsv1.1", php_openssl_ssl_socket_factory TSRMLS_CC);
	php_stream_xport_register("tlsv1.2", php_openssl_ssl_socket_factory TSRMLS_CC);
#endif

	/* override the default tcp socket provider */
	php_stream_xport_register("tcp", php_openssl_ssl_socket_factory TSRMLS_CC);

	php_register_url_stream_wrapper("https", &php_stream_http_wrapper TSRMLS_CC);
	php_register_url_stream_wrapper("ftps", &php_stream_ftp_wrapper TSRMLS_CC);

	REGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(openssl)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "OpenSSL support", "enabled");
	php_info_print_table_row(2, "OpenSSL Library Version", SSLeay_version(SSLEAY_VERSION));
	php_info_print_table_row(2, "OpenSSL Header Version", OPENSSL_VERSION_TEXT);
	php_info_print_table_row(2, "Openssl default config", default_ssl_conf_filename);
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(openssl)
{
	EVP_cleanup();

#if OPENSSL_VERSION_NUMBER >= 0x00090805f
	/* prevent accessing locking callback from unloaded extension */
	CRYPTO_set_locking_callback(NULL);
	/* free allocated error strings */
	ERR_free_strings();
#endif

	php_unregister_url_stream_wrapper("https" TSRMLS_CC);
	php_unregister_url_stream_wrapper("ftps" TSRMLS_CC);

	php_stream_xport_unregister("ssl" TSRMLS_CC);
#ifndef OPENSSL_NO_SSL2
	php_stream_xport_unregister("sslv2" TSRMLS_CC);
#endif
	php_stream_xport_unregister("sslv3" TSRMLS_CC);
	php_stream_xport_unregister("tls" TSRMLS_CC);
	php_stream_xport_unregister("tlsv1.0" TSRMLS_CC);
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
	php_stream_xport_unregister("tlsv1.1" TSRMLS_CC);
	php_stream_xport_unregister("tlsv1.2" TSRMLS_CC);
#endif

	/* reinstate the default tcp handler */
	php_stream_xport_register("tcp", php_stream_generic_socket_factory TSRMLS_CC);

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ x509 cert functions */

/* {{{ proto array openssl_get_cert_locations(void)
   Retrieve an array mapping available certificate locations */
PHP_FUNCTION(openssl_get_cert_locations)
{
	array_init(return_value);

	add_assoc_string(return_value, "default_cert_file", (char *) X509_get_default_cert_file(), 1);
	add_assoc_string(return_value, "default_cert_file_env", (char *) X509_get_default_cert_file_env(), 1);
	add_assoc_string(return_value, "default_cert_dir", (char *) X509_get_default_cert_dir(), 1);
	add_assoc_string(return_value, "default_cert_dir_env", (char *) X509_get_default_cert_dir_env(), 1);
	add_assoc_string(return_value, "default_private_dir", (char *) X509_get_default_private_dir(), 1);
	add_assoc_string(return_value, "default_default_cert_area", (char *) X509_get_default_cert_area(), 1);
	add_assoc_string(return_value, "ini_cafile",
		zend_ini_string("openssl.cafile", sizeof("openssl.cafile"), 0), 1);
	add_assoc_string(return_value, "ini_capath",
		zend_ini_string("openssl.capath", sizeof("openssl.capath"), 0), 1);
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

	if (resourceval) {
		*resourceval = -1;
	}
	if (Z_TYPE_PP(val) == IS_RESOURCE) {
		/* is it an x509 resource ? */
		void * what;
		int type;

		what = zend_fetch_resource(val TSRMLS_CC, -1, "OpenSSL X.509", &type, 1, le_x509);
		if (!what) {
			return NULL;
		}
		/* this is so callers can decide if they should free the X509 */
		if (resourceval) {
			*resourceval = Z_LVAL_PP(val);
		}
		if (type == le_x509) {
			return (X509*)what;
		}
		/* other types could be used here - eg: file pointers and read in the data from them */

		return NULL;
	}

	if (!(Z_TYPE_PP(val) == IS_STRING || Z_TYPE_PP(val) == IS_OBJECT)) {
		return NULL;
	}

	/* force it to be a string and check if it refers to a file */
	convert_to_string_ex(val);

	if (Z_STRLEN_PP(val) > 7 && memcmp(Z_STRVAL_PP(val), "file://", sizeof("file://") - 1) == 0) {
		/* read cert from the named file */
		BIO *in;

		if (php_openssl_open_base_dir_chk(Z_STRVAL_PP(val) + (sizeof("file://") - 1) TSRMLS_CC)) {
			return NULL;
		}

		in = BIO_new_file(Z_STRVAL_PP(val) + (sizeof("file://") - 1), "r");
		if (in == NULL) {
			return NULL;
		}
		cert = PEM_read_bio_X509(in, NULL, NULL, NULL);
		BIO_free(in);
	} else {
		BIO *in;

		in = BIO_new_mem_buf(Z_STRVAL_PP(val), Z_STRLEN_PP(val));
		if (in == NULL) {
			return NULL;
		}
#ifdef TYPEDEF_D2I_OF
		cert = (X509 *) PEM_ASN1_read_bio((d2i_of_void *)d2i_X509, PEM_STRING_X509, in, NULL, NULL, NULL);
#else
		cert = (X509 *) PEM_ASN1_read_bio((char *(*)())d2i_X509, PEM_STRING_X509, in, NULL, NULL, NULL);
#endif
		BIO_free(in);
	}

	if (cert && makeresource && resourceval) {
		*resourceval = zend_list_insert(cert, le_x509 TSRMLS_CC);
	}
	return cert;
}

/* }}} */

/* {{{ proto bool openssl_x509_export_to_file(mixed x509, string outfilename [, bool notext = true])
   Exports a CERT to file or a var */
PHP_FUNCTION(openssl_x509_export_to_file)
{
	X509 * cert;
	zval ** zcert;
	zend_bool notext = 1;
	BIO * bio_out;
	long certresource;
	char * filename;
	int filename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zp|b", &zcert, &filename, &filename_len, &notext) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	cert = php_openssl_x509_from_zval(zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get cert from parameter 1");
		return;
	}

	if (php_openssl_open_base_dir_chk(filename TSRMLS_CC)) {
		return;
	}

	bio_out = BIO_new_file(filename, "w");
	if (bio_out) {
		if (!notext) {
			X509_print(bio_out, cert);
		}
		PEM_write_bio_X509(bio_out, cert);

		RETVAL_TRUE;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error opening file %s", filename);
	}
	if (certresource == -1 && cert) {
		X509_free(cert);
	}
	BIO_free(bio_out);
}
/* }}} */

/* {{{ proto string openssl_spki_new(mixed zpkey, string challenge [, mixed method])
   Creates new private key (or uses existing) and creates a new spki cert
   outputting results to var */
PHP_FUNCTION(openssl_spki_new)
{
	int challenge_len;
	char * challenge = NULL, * spkstr = NULL, * s = NULL;
	long keyresource = -1;
	const char *spkac = "SPKAC=";
	long algo = OPENSSL_ALGO_MD5;

	zval *method = NULL;
	zval * zpkey = NULL;
	EVP_PKEY * pkey = NULL;
	NETSCAPE_SPKI *spki=NULL;
	const EVP_MD *mdtype;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|z", &zpkey, &challenge, &challenge_len, &method) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	pkey = php_openssl_evp_from_zval(&zpkey, 0, challenge, 1, &keyresource TSRMLS_CC);

	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to use supplied private key");
		goto cleanup;
	}

	if (method != NULL) {
		if (Z_TYPE_P(method) == IS_LONG) {
			algo = Z_LVAL_P(method);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Algorithm must be of supported type");
			goto cleanup;
		}
	}
	mdtype = php_openssl_get_evp_md_from_algo(algo);

	if (!mdtype) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm");
		goto cleanup;
	}

	if ((spki = NETSCAPE_SPKI_new()) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create new SPKAC");
		goto cleanup;
	}

	if (challenge) {
		ASN1_STRING_set(spki->spkac->challenge, challenge, challenge_len);
	}

	if (!NETSCAPE_SPKI_set_pubkey(spki, pkey)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to embed public key");
		goto cleanup;
	}

	if (!NETSCAPE_SPKI_sign(spki, pkey, mdtype)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to sign with specified algorithm");
		goto cleanup;
	}

	spkstr = NETSCAPE_SPKI_b64_encode(spki);
	if (!spkstr){
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to encode SPKAC");
		goto cleanup;
	}

	s = emalloc(strlen(spkac) + strlen(spkstr) + 1);
	sprintf(s, "%s%s", spkac, spkstr);

	RETVAL_STRINGL(s, strlen(s), 0);
	goto cleanup;

cleanup:

	if (keyresource == -1 && spki != NULL) {
		NETSCAPE_SPKI_free(spki);
	}
	if (keyresource == -1 && pkey != NULL) {
		EVP_PKEY_free(pkey);
	}
	if (keyresource == -1 && spkstr != NULL) {
		efree(spkstr);
	}

	if (s && strlen(s) <= 0) {
		RETVAL_FALSE;
	}

	if (keyresource == -1 && s != NULL) {
		efree(s);
	}
}
/* }}} */

/* {{{ proto bool openssl_spki_verify(string spki)
   Verifies spki returns boolean */
PHP_FUNCTION(openssl_spki_verify)
{
	int spkstr_len, i = 0;
	char *spkstr = NULL, * spkstr_cleaned = NULL;

	EVP_PKEY *pkey = NULL;
	NETSCAPE_SPKI *spki = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &spkstr, &spkstr_len) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	if (spkstr == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to use supplied SPKAC");
		goto cleanup;
	}

	spkstr_cleaned = emalloc(spkstr_len + 1);
	openssl_spki_cleanup(spkstr, spkstr_cleaned);

	if (strlen(spkstr_cleaned)<=0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid SPKAC");
		goto cleanup;
	}

	spki = NETSCAPE_SPKI_b64_decode(spkstr_cleaned, strlen(spkstr_cleaned));
	if (spki == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to decode supplied SPKAC");
		goto cleanup;
	}

	pkey = X509_PUBKEY_get(spki->spkac->pubkey);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to acquire signed public key");
		goto cleanup;
	}

	i = NETSCAPE_SPKI_verify(spki, pkey);
	goto cleanup;

cleanup:
	if (spki != NULL) {
		NETSCAPE_SPKI_free(spki);
	}
	if (pkey != NULL) {
		EVP_PKEY_free(pkey);
	}
	if (spkstr_cleaned != NULL) {
		efree(spkstr_cleaned);
	}

	if (i > 0) {
		RETVAL_TRUE;
	}
}
/* }}} */

/* {{{ proto string openssl_spki_export(string spki)
   Exports public key from existing spki to var */
PHP_FUNCTION(openssl_spki_export)
{
	int spkstr_len;
	char *spkstr = NULL, * spkstr_cleaned = NULL, * s = NULL;

	EVP_PKEY *pkey = NULL;
	NETSCAPE_SPKI *spki = NULL;
	BIO *out = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &spkstr, &spkstr_len) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	if (spkstr == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to use supplied SPKAC");
		goto cleanup;
	}

	spkstr_cleaned = emalloc(spkstr_len + 1);
	openssl_spki_cleanup(spkstr, spkstr_cleaned);

	spki = NETSCAPE_SPKI_b64_decode(spkstr_cleaned, strlen(spkstr_cleaned));
	if (spki == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to decode supplied SPKAC");
		goto cleanup;
	}

	pkey = X509_PUBKEY_get(spki->spkac->pubkey);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to acquire signed public key");
		goto cleanup;
	}

	out = BIO_new(BIO_s_mem());
	if (out && PEM_write_bio_PUBKEY(out, pkey))  {
		BUF_MEM *bio_buf;

		BIO_get_mem_ptr(out, &bio_buf);
		RETVAL_STRINGL((char *)bio_buf->data, bio_buf->length, 1);
	}
	goto cleanup;

cleanup:

	if (spki != NULL) {
		NETSCAPE_SPKI_free(spki);
	}
	if (out != NULL) {
		BIO_free_all(out);
	}
	if (pkey != NULL) {
		EVP_PKEY_free(pkey);
	}
	if (spkstr_cleaned != NULL) {
		efree(spkstr_cleaned);
	}
	if (s != NULL) {
		efree(s);
	}
}
/* }}} */

/* {{{ proto string openssl_spki_export_challenge(string spki)
   Exports spkac challenge from existing spki to var */
PHP_FUNCTION(openssl_spki_export_challenge)
{
	int spkstr_len;
	char *spkstr = NULL, * spkstr_cleaned = NULL;

	NETSCAPE_SPKI *spki = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &spkstr, &spkstr_len) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	if (spkstr == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to use supplied SPKAC");
		goto cleanup;
	}

	spkstr_cleaned = emalloc(spkstr_len + 1);
	openssl_spki_cleanup(spkstr, spkstr_cleaned);

	spki = NETSCAPE_SPKI_b64_decode(spkstr_cleaned, strlen(spkstr_cleaned));
	if (spki == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to decode SPKAC");
		goto cleanup;
	}

	RETVAL_STRING((char *) ASN1_STRING_data(spki->spkac->challenge), 1);
	goto cleanup;

cleanup:
	if (spkstr_cleaned != NULL) {
		efree(spkstr_cleaned);
	}
}
/* }}} */

/* {{{ strip line endings from spkac */
int openssl_spki_cleanup(const char *src, char *dest)
{
    int removed=0;

    while (*src) {
        if (*src!='\n'&&*src!='\r') {
            *dest++=*src;
        } else {
            ++removed;
        }
        ++src;
    }
    *dest=0;
    return removed;
}
/* }}} */

/* {{{ proto bool openssl_x509_export(mixed x509, string &out [, bool notext = true])
   Exports a CERT to file or a var */
PHP_FUNCTION(openssl_x509_export)
{
	X509 * cert;
	zval ** zcert, *zout;
	zend_bool notext = 1;
	BIO * bio_out;
	long certresource;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zz|b", &zcert, &zout, &notext) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	cert = php_openssl_x509_from_zval(zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get cert from parameter 1");
		return;
	}

	bio_out = BIO_new(BIO_s_mem());
	if (!notext) {
		X509_print(bio_out, cert);
	}
	if (PEM_write_bio_X509(bio_out, cert))  {
		BUF_MEM *bio_buf;

		zval_dtor(zout);
		BIO_get_mem_ptr(bio_out, &bio_buf);
		ZVAL_STRINGL(zout, bio_buf->data, bio_buf->length, 1);

		RETVAL_TRUE;
	}

	if (certresource == -1 && cert) {
		X509_free(cert);
	}
	BIO_free(bio_out);
}
/* }}} */

int php_openssl_x509_fingerprint(X509 *peer, const char *method, zend_bool raw, char **out, int *out_len TSRMLS_DC)
{
	unsigned char md[EVP_MAX_MD_SIZE];
	const EVP_MD *mdtype;
	unsigned int n;

	if (!(mdtype = EVP_get_digestbyname(method))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm");
		return FAILURE;
	} else if (!X509_digest(peer, mdtype, md, &n)) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not generate signature");
		return FAILURE;
	}

	if (raw) {
		*out_len = n;
		*out = estrndup((char *) md, n);
	} else {
		*out_len = n * 2;
		*out = emalloc(*out_len + 1);

		make_digest_ex(*out, md, n);
	}

	return SUCCESS;
}

PHP_FUNCTION(openssl_x509_fingerprint)
{
	X509 *cert;
	zval **zcert;
	long certresource;
	zend_bool raw_output = 0;
	char *method = "sha1";
	int method_len;

	char *fingerprint;
	int fingerprint_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|sb", &zcert, &method, &method_len, &raw_output) == FAILURE) {
		return;
	}

	cert = php_openssl_x509_from_zval(zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get cert from parameter 1");
		RETURN_FALSE;
	}

	if (php_openssl_x509_fingerprint(cert, method, raw_output, &fingerprint, &fingerprint_len TSRMLS_CC) == SUCCESS) {
		RETVAL_STRINGL(fingerprint, fingerprint_len, 0);
	} else {
		RETVAL_FALSE;
	}

	if (certresource == -1 && cert) {
		X509_free(cert);
	}
}

/* {{{ proto bool openssl_x509_check_private_key(mixed cert, mixed key)
   Checks if a private key corresponds to a CERT */
PHP_FUNCTION(openssl_x509_check_private_key)
{
	zval ** zcert, **zkey;
	X509 * cert = NULL;
	EVP_PKEY * key = NULL;
	long certresource = -1, keyresource = -1;

	RETVAL_FALSE;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ", &zcert, &zkey) == FAILURE) {
		return;
	}
	cert = php_openssl_x509_from_zval(zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		RETURN_FALSE;
	}	
	key = php_openssl_evp_from_zval(zkey, 0, "", 1, &keyresource TSRMLS_CC);
	if (key) {
		RETVAL_BOOL(X509_check_private_key(cert, key));
	}

	if (keyresource == -1 && key) {
		EVP_PKEY_free(key);
	}
	if (certresource == -1 && cert) {
		X509_free(cert);
	}
}
/* }}} */

/* Special handling of subjectAltName, see CVE-2013-4073
 * Christian Heimes
 */

static int openssl_x509v3_subjectAltName(BIO *bio, X509_EXTENSION *extension)
{
	GENERAL_NAMES *names;
	const X509V3_EXT_METHOD *method = NULL;
	long i, length, num;
	const unsigned char *p;

	method = X509V3_EXT_get(extension);
	if (method == NULL) {
		return -1;
	}

	p = extension->value->data;
	length = extension->value->length;
	if (method->it) {
		names = (GENERAL_NAMES*)(ASN1_item_d2i(NULL, &p, length,
						       ASN1_ITEM_ptr(method->it)));
	} else {
		names = (GENERAL_NAMES*)(method->d2i(NULL, &p, length));
	}
	if (names == NULL) {
		return -1;
	}

	num = sk_GENERAL_NAME_num(names);
	for (i = 0; i < num; i++) {
			GENERAL_NAME *name;
			ASN1_STRING *as;
			name = sk_GENERAL_NAME_value(names, i);
			switch (name->type) {
				case GEN_EMAIL:
					BIO_puts(bio, "email:");
					as = name->d.rfc822Name;
					BIO_write(bio, ASN1_STRING_data(as),
						  ASN1_STRING_length(as));
					break;
				case GEN_DNS:
					BIO_puts(bio, "DNS:");
					as = name->d.dNSName;
					BIO_write(bio, ASN1_STRING_data(as),
						  ASN1_STRING_length(as));
					break;
				case GEN_URI:
					BIO_puts(bio, "URI:");
					as = name->d.uniformResourceIdentifier;
					BIO_write(bio, ASN1_STRING_data(as),
						  ASN1_STRING_length(as));
					break;
				default:
					/* use builtin print for GEN_OTHERNAME, GEN_X400,
					 * GEN_EDIPARTY, GEN_DIRNAME, GEN_IPADD and GEN_RID
					 */
					GENERAL_NAME_print(bio, name);
			}
			/* trailing ', ' except for last element */
			if (i < (num - 1)) {
				BIO_puts(bio, ", ");
			}
	}
	sk_GENERAL_NAME_pop_free(names, GENERAL_NAME_free);

	return 0;
}

/* {{{ proto array openssl_x509_parse(mixed x509 [, bool shortnames=true])
   Returns an array of the fields/values of the CERT */
PHP_FUNCTION(openssl_x509_parse)
{
	zval ** zcert;
	X509 * cert = NULL;
	long certresource = -1;
	int i, sig_nid;
	zend_bool useshortnames = 1;
	char * tmpstr;
	zval * subitem;
	X509_EXTENSION *extension;
	char *extname;
	BIO  *bio_out;
	BUF_MEM *bio_buf;
	char buf[256];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|b", &zcert, &useshortnames) == FAILURE) {
		return;
	}
	cert = php_openssl_x509_from_zval(zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		RETURN_FALSE;
	}
	array_init(return_value);

	if (cert->name) {
		add_assoc_string(return_value, "name", cert->name, 1);
	}
/*	add_assoc_bool(return_value, "valid", cert->valid); */

	add_assoc_name_entry(return_value, "subject", 		X509_get_subject_name(cert), useshortnames TSRMLS_CC);
	/* hash as used in CA directories to lookup cert by subject name */
	{
		char buf[32];
		snprintf(buf, sizeof(buf), "%08lx", X509_subject_name_hash(cert));
		add_assoc_string(return_value, "hash", buf, 1);
	}
	
	add_assoc_name_entry(return_value, "issuer", 		X509_get_issuer_name(cert), useshortnames TSRMLS_CC);
	add_assoc_long(return_value, "version", 			X509_get_version(cert));

	add_assoc_string(return_value, "serialNumber", i2s_ASN1_INTEGER(NULL, X509_get_serialNumber(cert)), 1); 

	add_assoc_asn1_string(return_value, "validFrom", 	X509_get_notBefore(cert));
	add_assoc_asn1_string(return_value, "validTo", 		X509_get_notAfter(cert));

	add_assoc_long(return_value, "validFrom_time_t", 	asn1_time_to_time_t(X509_get_notBefore(cert) TSRMLS_CC));
	add_assoc_long(return_value, "validTo_time_t", 		asn1_time_to_time_t(X509_get_notAfter(cert) TSRMLS_CC));

	tmpstr = (char *)X509_alias_get0(cert, NULL);
	if (tmpstr) {
		add_assoc_string(return_value, "alias", tmpstr, 1);
	}

	sig_nid = OBJ_obj2nid((cert)->sig_alg->algorithm);
	add_assoc_string(return_value, "signatureTypeSN", (char*)OBJ_nid2sn(sig_nid), 1);
	add_assoc_string(return_value, "signatureTypeLN", (char*)OBJ_nid2ln(sig_nid), 1);
	add_assoc_long(return_value, "signatureTypeNID", sig_nid);

	MAKE_STD_ZVAL(subitem);
	array_init(subitem);

	/* NOTE: the purposes are added as integer keys - the keys match up to the X509_PURPOSE_SSL_XXX defines
	   in x509v3.h */
	for (i = 0; i < X509_PURPOSE_get_count(); i++) {
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

	MAKE_STD_ZVAL(subitem);
	array_init(subitem);


	for (i = 0; i < X509_get_ext_count(cert); i++) {
		int nid;
		extension = X509_get_ext(cert, i);
		nid = OBJ_obj2nid(X509_EXTENSION_get_object(extension));
		if (nid != NID_undef) {
			extname = (char *)OBJ_nid2sn(OBJ_obj2nid(X509_EXTENSION_get_object(extension)));
		} else {
			OBJ_obj2txt(buf, sizeof(buf)-1, X509_EXTENSION_get_object(extension), 1);
			extname = buf;
		}
		bio_out = BIO_new(BIO_s_mem());
		if (nid == NID_subject_alt_name) {
			if (openssl_x509v3_subjectAltName(bio_out, extension) == 0) {
				BIO_get_mem_ptr(bio_out, &bio_buf);
				add_assoc_stringl(subitem, extname, bio_buf->data, bio_buf->length, 1);
			} else {
				zval_dtor(return_value);
				if (certresource == -1 && cert) {
					X509_free(cert);
				}
				BIO_free(bio_out);
				RETURN_FALSE;
			}
		}
		else if (X509V3_EXT_print(bio_out, extension, 0, 0)) {
			BIO_get_mem_ptr(bio_out, &bio_buf);
			add_assoc_stringl(subitem, extname, bio_buf->data, bio_buf->length, 1);
		} else {
			add_assoc_asn1_string(subitem, extname, X509_EXTENSION_get_data(extension));
		}
		BIO_free(bio_out);
	}
	add_assoc_zval(return_value, "extensions", subitem);

	if (certresource == -1 && cert) {
		X509_free(cert);
	}
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
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "memory allocation failure");
		goto end;
	}

	if (php_openssl_open_base_dir_chk(certfile TSRMLS_CC)) {
		sk_X509_free(stack);
		goto end;
	}

	if(!(in=BIO_new_file(certfile, "r"))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error opening the file, %s", certfile);
		sk_X509_free(stack);
		goto end;
	}

	/* This loads from a file, a stack of x509/crl/pkey sets */
	if(!(sk=PEM_X509_INFO_read_bio(in, NULL, NULL, NULL))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error reading the file, %s", certfile);
		sk_X509_free(stack);
		goto end;
	}

	/* scan over it and pull out the certs */
	while (sk_X509_INFO_num(sk)) {
		xi=sk_X509_INFO_shift(sk);
		if (xi->x509 != NULL) {
			sk_X509_push(stack,xi->x509);
			xi->x509=NULL;
		}
		X509_INFO_free(xi);
	}
	if(!sk_X509_num(stack)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "no certificates in file, %s", certfile);
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
	if (csc == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "memory allocation failure");
		return 0;
	}
	X509_STORE_CTX_init(csc, ctx, x, untrustedchain);
	if(purpose >= 0) {
		X509_STORE_CTX_set_purpose(csc, purpose);
	}
	ret = X509_verify_cert(csc);
	X509_STORE_CTX_free(csc);

	return ret;
}
/* }}} */

/* {{{ proto int openssl_x509_checkpurpose(mixed x509cert, int purpose, array cainfo [, string untrustedfile])
   Checks the CERT to see if it can be used for the purpose in purpose. cainfo holds information about trusted CAs */
PHP_FUNCTION(openssl_x509_checkpurpose)
{
	zval ** zcert, * zcainfo = NULL;
	X509_STORE * cainfo = NULL;
	X509 * cert = NULL;
	long certresource = -1;
	STACK_OF(X509) * untrustedchain = NULL;
	long purpose;
	char * untrusted = NULL;
	int untrusted_len = 0, ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zl|a!s", &zcert, &purpose, &zcainfo, &untrusted, &untrusted_len) == FAILURE) {
		return;
	}

	RETVAL_LONG(-1);

	if (untrusted) {
		untrustedchain = load_all_certs_from_file(untrusted);
		if (untrustedchain == NULL) {
			goto clean_exit;
		}
	}

	cainfo = setup_verify(zcainfo TSRMLS_CC);
	if (cainfo == NULL) {
		goto clean_exit;
	}
	cert = php_openssl_x509_from_zval(zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		goto clean_exit;
	}

	ret = check_cert(cainfo, cert, untrustedchain, purpose);
	if (ret != 0 && ret != 1) {
		RETVAL_LONG(ret);
	} else {
		RETVAL_BOOL(ret);
	}

clean_exit:
	if (certresource == 1 && cert) {
		X509_free(cert);
	}
	if (cainfo) { 
		X509_STORE_free(cainfo); 
	}
	if (untrustedchain) {
		sk_X509_pop_free(untrustedchain, X509_free);
	}
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

	if (store == NULL) {
		return NULL;
	}

	if (calist && (Z_TYPE_P(calist) == IS_ARRAY)) {
		zend_hash_internal_pointer_reset_ex(HASH_OF(calist), &pos);
		for (;; zend_hash_move_forward_ex(HASH_OF(calist), &pos)) {
			zval ** item;
			struct stat sb;

			if (zend_hash_get_current_data_ex(HASH_OF(calist), (void**)&item, &pos) == FAILURE) {
				break;
			}
			convert_to_string_ex(item);

			if (VCWD_STAT(Z_STRVAL_PP(item), &sb) == -1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to stat %s", Z_STRVAL_PP(item));
				continue;
			}

			if ((sb.st_mode & S_IFREG) == S_IFREG) {
				file_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
				if (file_lookup == NULL || !X509_LOOKUP_load_file(file_lookup, Z_STRVAL_PP(item), X509_FILETYPE_PEM)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "error loading file %s", Z_STRVAL_PP(item));
				} else {
					nfiles++;
				}
				file_lookup = NULL;
			} else {
				dir_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
				if (dir_lookup == NULL || !X509_LOOKUP_add_dir(dir_lookup, Z_STRVAL_PP(item), X509_FILETYPE_PEM)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "error loading directory %s", Z_STRVAL_PP(item));
				} else { 
					ndirs++;
				}
				dir_lookup = NULL;
			}
		}
	}
	if (nfiles == 0) {
		file_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
		if (file_lookup) {
			X509_LOOKUP_load_file(file_lookup, NULL, X509_FILETYPE_DEFAULT);
		}
	}
	if (ndirs == 0) {
		dir_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
		if (dir_lookup) {
			X509_LOOKUP_add_dir(dir_lookup, NULL, X509_FILETYPE_DEFAULT);
		}
	}
	return store;
}
/* }}} */

/* {{{ proto resource openssl_x509_read(mixed cert)
   Reads X.509 certificates */
PHP_FUNCTION(openssl_x509_read)
{
	zval **cert;
	X509 *x509;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &cert) == FAILURE) {
		return;
	}
	Z_TYPE_P(return_value) = IS_RESOURCE;
	x509 = php_openssl_x509_from_zval(cert, 1, &Z_LVAL_P(return_value) TSRMLS_CC);

	if (x509 == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "supplied parameter cannot be coerced into an X509 certificate!");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void openssl_x509_free(resource x509)
   Frees X.509 certificates */
PHP_FUNCTION(openssl_x509_free)
{
	zval *x509;
	X509 *cert;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &x509) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(cert, X509 *, &x509, -1, "OpenSSL X.509", le_x509);
	zend_list_delete(Z_LVAL_P(x509));
}
/* }}} */

/* }}} */

/* Pop all X509 from Stack and free them, free the stack afterwards */
static void php_sk_X509_free(STACK_OF(X509) * sk) /* {{{ */
{
	for (;;) {
		X509* x = sk_X509_pop(sk);
		if (!x) break;
		X509_free(x);
	}
	sk_X509_free(sk);
}
/* }}} */

static STACK_OF(X509) * php_array_to_X509_sk(zval ** zcerts TSRMLS_DC) /* {{{ */
{
	HashPosition hpos;
	zval ** zcertval;
	STACK_OF(X509) * sk = NULL;
    X509 * cert;
    long certresource;

	sk = sk_X509_new_null();

	/* get certs */
	if (Z_TYPE_PP(zcerts) == IS_ARRAY) {
		zend_hash_internal_pointer_reset_ex(HASH_OF(*zcerts), &hpos);
		while(zend_hash_get_current_data_ex(HASH_OF(*zcerts), (void**)&zcertval, &hpos) == SUCCESS) {

			cert = php_openssl_x509_from_zval(zcertval, 0, &certresource TSRMLS_CC);
			if (cert == NULL) {
				goto clean_exit;
			}

			if (certresource != -1) {
				cert = X509_dup(cert);
				
				if (cert == NULL) {
					goto clean_exit;
				}
				
			}
			sk_X509_push(sk, cert);

			zend_hash_move_forward_ex(HASH_OF(*zcerts), &hpos);
		}
	} else {
		/* a single certificate */
		cert = php_openssl_x509_from_zval(zcerts, 0, &certresource TSRMLS_CC);
		
		if (cert == NULL) {
			goto clean_exit;
		}

		if (certresource != -1) {
			cert = X509_dup(cert);
			if (cert == NULL) {
				goto clean_exit;
			}
		}
		sk_X509_push(sk, cert);
	}

  clean_exit:
    return sk;
}
/* }}} */

/* {{{ proto bool openssl_pkcs12_export_to_file(mixed x509, string filename, mixed priv_key, string pass[, array args])
   Creates and exports a PKCS to file */
PHP_FUNCTION(openssl_pkcs12_export_to_file)
{
	X509 * cert = NULL;
	BIO * bio_out = NULL;
	PKCS12 * p12 = NULL;
	char * filename;
	char * friendly_name = NULL;
	int filename_len;
	char * pass;
	int pass_len;
	zval **zcert = NULL, *zpkey = NULL, *args = NULL;
	EVP_PKEY *priv_key = NULL;
	long certresource, keyresource;
	zval ** item;
	STACK_OF(X509) *ca = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zpzs|a", &zcert, &filename, &filename_len, &zpkey, &pass, &pass_len, &args) == FAILURE)
		return;

	RETVAL_FALSE;
	
	cert = php_openssl_x509_from_zval(zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get cert from parameter 1");
		return;
	}
	priv_key = php_openssl_evp_from_zval(&zpkey, 0, "", 1, &keyresource TSRMLS_CC);
	if (priv_key == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get private key from parameter 3");
		goto cleanup;
	}
	if (cert && !X509_check_private_key(cert, priv_key)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "private key does not correspond to cert");
		goto cleanup;
	}
	if (php_openssl_open_base_dir_chk(filename TSRMLS_CC)) {
		goto cleanup;
	}

	/* parse extra config from args array, promote this to an extra function */
	if (args && zend_hash_find(Z_ARRVAL_P(args), "friendly_name", sizeof("friendly_name"), (void**)&item) == SUCCESS && Z_TYPE_PP(item) == IS_STRING)
		friendly_name = Z_STRVAL_PP(item);
	/* certpbe (default RC2-40)
	   keypbe (default 3DES)
	   friendly_caname
	*/

	if (args && zend_hash_find(Z_ARRVAL_P(args), "extracerts", sizeof("extracerts"), (void**)&item) == SUCCESS)
		ca = php_array_to_X509_sk(item TSRMLS_CC);
	/* end parse extra config */

	/*PKCS12 *PKCS12_create(char *pass, char *name, EVP_PKEY *pkey, X509 *cert, STACK_OF(X509) *ca,
                                       int nid_key, int nid_cert, int iter, int mac_iter, int keytype);*/

	p12 = PKCS12_create(pass, friendly_name, priv_key, cert, ca, 0, 0, 0, 0, 0);

	bio_out = BIO_new_file(filename, "w"); 
	if (bio_out) {
		
		i2d_PKCS12_bio(bio_out, p12);

		RETVAL_TRUE;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error opening file %s", filename);
	}

	BIO_free(bio_out);
	PKCS12_free(p12);
	php_sk_X509_free(ca);
	
cleanup:

	if (keyresource == -1 && priv_key) {
		EVP_PKEY_free(priv_key);
	}
	if (certresource == -1 && cert) { 
		X509_free(cert);
	}
}
/* }}} */

/* {{{ proto bool openssl_pkcs12_export(mixed x509, string &out, mixed priv_key, string pass[, array args])
   Creates and exports a PKCS12 to a var */
PHP_FUNCTION(openssl_pkcs12_export)
{
	X509 * cert = NULL;
	BIO * bio_out;
	PKCS12 * p12 = NULL;
	zval * zcert = NULL, *zout = NULL, *zpkey, *args = NULL;
	EVP_PKEY *priv_key = NULL;
	long certresource, keyresource;
	char * pass;
	int pass_len;
	char * friendly_name = NULL;
	zval ** item;
	STACK_OF(X509) *ca = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzzs|a", &zcert, &zout, &zpkey, &pass, &pass_len, &args) == FAILURE)
		return;

	RETVAL_FALSE;
	
	cert = php_openssl_x509_from_zval(&zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get cert from parameter 1");
		return;
	}
	priv_key = php_openssl_evp_from_zval(&zpkey, 0, "", 1, &keyresource TSRMLS_CC);
	if (priv_key == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get private key from parameter 3");
		goto cleanup;
	}
	if (cert && !X509_check_private_key(cert, priv_key)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "private key does not correspond to cert");
		goto cleanup;
	}

	/* parse extra config from args array, promote this to an extra function */
	if (args && zend_hash_find(Z_ARRVAL_P(args), "friendly_name", sizeof("friendly_name"), (void**)&item) == SUCCESS && Z_TYPE_PP(item) == IS_STRING)
		friendly_name = Z_STRVAL_PP(item);

	if (args && zend_hash_find(Z_ARRVAL_P(args), "extracerts", sizeof("extracerts"), (void**)&item) == SUCCESS)
		ca = php_array_to_X509_sk(item TSRMLS_CC);
	/* end parse extra config */
	
	p12 = PKCS12_create(pass, friendly_name, priv_key, cert, ca, 0, 0, 0, 0, 0);

	bio_out = BIO_new(BIO_s_mem());
	if (i2d_PKCS12_bio(bio_out, p12))  {
		BUF_MEM *bio_buf;

		zval_dtor(zout);
		BIO_get_mem_ptr(bio_out, &bio_buf);
		ZVAL_STRINGL(zout, bio_buf->data, bio_buf->length, 1);

		RETVAL_TRUE;
	}

	BIO_free(bio_out);
	PKCS12_free(p12);
	php_sk_X509_free(ca);
	
cleanup:

	if (keyresource == -1 && priv_key) {
		EVP_PKEY_free(priv_key);
	}
	if (certresource == -1 && cert) { 
		X509_free(cert);
	}
}
/* }}} */

/* {{{ proto bool openssl_pkcs12_read(string PKCS12, array &certs, string pass)
   Parses a PKCS12 to an array */
PHP_FUNCTION(openssl_pkcs12_read)
{
	zval *zout = NULL, *zextracerts, *zcert, *zpkey;
	char *pass, *zp12;
	int pass_len, zp12_len;
	PKCS12 * p12 = NULL;
	EVP_PKEY * pkey = NULL;
	X509 * cert = NULL;
	STACK_OF(X509) * ca = NULL;
	BIO * bio_in = NULL;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szs", &zp12, &zp12_len, &zout, &pass, &pass_len) == FAILURE)
		return;

	RETVAL_FALSE;
	
	bio_in = BIO_new(BIO_s_mem());
	
	if(!BIO_write(bio_in, zp12, zp12_len))
		goto cleanup;
	
	if(d2i_PKCS12_bio(bio_in, &p12)) {
		if(PKCS12_parse(p12, pass, &pkey, &cert, &ca)) {
			BIO * bio_out;

			zval_dtor(zout);
			array_init(zout);

			bio_out = BIO_new(BIO_s_mem());
			if (PEM_write_bio_X509(bio_out, cert)) {
				BUF_MEM *bio_buf;
				BIO_get_mem_ptr(bio_out, &bio_buf);
				MAKE_STD_ZVAL(zcert);
				ZVAL_STRINGL(zcert, bio_buf->data, bio_buf->length, 1);
				add_assoc_zval(zout, "cert", zcert);
			}
			BIO_free(bio_out);

			bio_out = BIO_new(BIO_s_mem());
			if (PEM_write_bio_PrivateKey(bio_out, pkey, NULL, NULL, 0, 0, NULL)) {
				BUF_MEM *bio_buf;
				BIO_get_mem_ptr(bio_out, &bio_buf);
				MAKE_STD_ZVAL(zpkey);
				ZVAL_STRINGL(zpkey, bio_buf->data, bio_buf->length, 1);
				add_assoc_zval(zout, "pkey", zpkey);
			}
			BIO_free(bio_out);

			MAKE_STD_ZVAL(zextracerts);
			array_init(zextracerts);
			
			for (i=0;;i++) {
				zval * zextracert;
				X509* aCA = sk_X509_pop(ca);
				if (!aCA) break;

				/* fix for bug 69882 */
				{
					int err = ERR_peek_error();
					if (err == OPENSSL_ERROR_X509_PRIVATE_KEY_VALUES_MISMATCH) {
						ERR_get_error();
					}
				}

				bio_out = BIO_new(BIO_s_mem());
				if (PEM_write_bio_X509(bio_out, aCA)) {
					BUF_MEM *bio_buf;
					BIO_get_mem_ptr(bio_out, &bio_buf);
					MAKE_STD_ZVAL(zextracert);
					ZVAL_STRINGL(zextracert, bio_buf->data, bio_buf->length, 1);
					add_index_zval(zextracerts, i, zextracert);
					
				}
				BIO_free(bio_out);

				X509_free(aCA);
			}
			if(ca) {
				sk_X509_free(ca);
				add_assoc_zval(zout, "extracerts", zextracerts);
			} else {
				zval_dtor(zextracerts);
			}
			
			RETVAL_TRUE;
			
			PKCS12_free(p12);
		}
	}
	
  cleanup:
	if (bio_in) {
		BIO_free(bio_in);
	}
	if (pkey) {
		EVP_PKEY_free(pkey);
	}
	if (cert) { 
		X509_free(cert);
	}
}
/* }}} */

/* {{{ x509 CSR functions */

/* {{{ php_openssl_make_REQ */
static int php_openssl_make_REQ(struct php_x509_request * req, X509_REQ * csr, zval * dn, zval * attribs TSRMLS_DC)
{
	STACK_OF(CONF_VALUE) * dn_sk, *attr_sk = NULL;
	char * str, *dn_sect, *attr_sect;

	dn_sect = CONF_get_string(req->req_config, req->section_name, "distinguished_name");
	if (dn_sect == NULL) {
		return FAILURE;
	}
	dn_sk = CONF_get_section(req->req_config, dn_sect);
	if (dn_sk == NULL) { 
		return FAILURE;
	}
	attr_sect = CONF_get_string(req->req_config, req->section_name, "attributes");
	if (attr_sect == NULL) {
		attr_sk = NULL;
	} else {
		attr_sk = CONF_get_section(req->req_config, attr_sect);
		if (attr_sk == NULL) {
			return FAILURE;
		}
	}
	/* setup the version number: version 1 */
	if (X509_REQ_set_version(csr, 0L)) {
		int i, nid;
		char * type;
		CONF_VALUE * v;
		X509_NAME * subj;
		HashPosition hpos;
		zval ** item;
		
		subj = X509_REQ_get_subject_name(csr);
		/* apply values from the dn hash */
		zend_hash_internal_pointer_reset_ex(HASH_OF(dn), &hpos);
		while(zend_hash_get_current_data_ex(HASH_OF(dn), (void**)&item, &hpos) == SUCCESS) {
			char * strindex = NULL; 
			uint strindexlen = 0;
			ulong intindex;
			
			zend_hash_get_current_key_ex(HASH_OF(dn), &strindex, &strindexlen, &intindex, 0, &hpos);

			convert_to_string_ex(item);

			if (strindex) {
				int nid;

				nid = OBJ_txt2nid(strindex);
				if (nid != NID_undef) {
					if (!X509_NAME_add_entry_by_NID(subj, nid, MBSTRING_UTF8, 
								(unsigned char*)Z_STRVAL_PP(item), -1, -1, 0))
					{
						php_error_docref(NULL TSRMLS_CC, E_WARNING,
							"dn: add_entry_by_NID %d -> %s (failed; check error"
							" queue and value of string_mask OpenSSL option "
							"if illegal characters are reported)",
							nid, Z_STRVAL_PP(item));
						return FAILURE;
					}
				} else {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "dn: %s is not a recognized name", strindex);
				}
			}
			zend_hash_move_forward_ex(HASH_OF(dn), &hpos);
		}

		/* Finally apply defaults from config file */
		for(i = 0; i < sk_CONF_VALUE_num(dn_sk); i++) {
			int len;
			char buffer[200 + 1]; /*200 + \0 !*/
			
			v = sk_CONF_VALUE_value(dn_sk, i);
			type = v->name;
			
			len = strlen(type);
			if (len < sizeof("_default")) {
				continue;
			}
			len -= sizeof("_default") - 1;
			if (strcmp("_default", type + len) != 0) {
				continue;
			}
			if (len > 200) {
				len = 200;
			}
			memcpy(buffer, type, len);
			buffer[len] = '\0';
			type = buffer;
		
			/* Skip past any leading X. X: X, etc to allow for multiple
			 * instances */
			for (str = type; *str; str++) {
				if (*str == ':' || *str == ',' || *str == '.') {
					str++;
					if (*str) {
						type = str;
					}
					break;
				}
			}
			/* if it is already set, skip this */
			nid = OBJ_txt2nid(type);
			if (X509_NAME_get_index_by_NID(subj, nid, -1) >= 0) {
				continue;
			}
			if (!X509_NAME_add_entry_by_txt(subj, type, MBSTRING_UTF8, (unsigned char*)v->value, -1, -1, 0)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "add_entry_by_txt %s -> %s (failed)", type, v->value);
				return FAILURE;
			}
			if (!X509_NAME_entry_count(subj)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "no objects specified in config file");
				return FAILURE;
			}
		}
		if (attribs) {
			zend_hash_internal_pointer_reset_ex(HASH_OF(attribs), &hpos);
			while(zend_hash_get_current_data_ex(HASH_OF(attribs), (void**)&item, &hpos) == SUCCESS) {
				char *strindex = NULL;
				uint strindexlen;
				ulong intindex;

				zend_hash_get_current_key_ex(HASH_OF(attribs), &strindex, &strindexlen, &intindex, 0, &hpos);
				convert_to_string_ex(item);

				if (strindex) {
					int nid;

					nid = OBJ_txt2nid(strindex);
					if (nid != NID_undef) {
						if (!X509_NAME_add_entry_by_NID(subj, nid, MBSTRING_UTF8, (unsigned char*)Z_STRVAL_PP(item), -1, -1, 0)) {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "attribs: add_entry_by_NID %d -> %s (failed)", nid, Z_STRVAL_PP(item));
							return FAILURE;
						}
					} else {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "dn: %s is not a recognized name", strindex);
					}
				}
				zend_hash_move_forward_ex(HASH_OF(attribs), &hpos);
			}
			for (i = 0; i < sk_CONF_VALUE_num(attr_sk); i++) {
				v = sk_CONF_VALUE_value(attr_sk, i);
				/* if it is already set, skip this */
				nid = OBJ_txt2nid(v->name);
				if (X509_REQ_get_attr_by_NID(csr, nid, -1) >= 0) {
					continue;
				}
				if (!X509_REQ_add1_attr_by_txt(csr, v->name, MBSTRING_UTF8, (unsigned char*)v->value, -1)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING,
						"add1_attr_by_txt %s -> %s (failed; check error queue "
						"and value of string_mask OpenSSL option if illegal "
						"characters are reported)",
						v->name, v->value);
					return FAILURE;
				}
			}
		}
	}

	X509_REQ_set_pubkey(csr, req->priv_key);
	return SUCCESS;
}
/* }}} */

/* {{{ php_openssl_csr_from_zval */
static X509_REQ * php_openssl_csr_from_zval(zval ** val, int makeresource, long * resourceval TSRMLS_DC)
{
	X509_REQ * csr = NULL;
	char * filename = NULL;
	BIO * in;
	
	if (resourceval) {
		*resourceval = -1;
	}
	if (Z_TYPE_PP(val) == IS_RESOURCE) {
		void * what;
		int type;

		what = zend_fetch_resource(val TSRMLS_CC, -1, "OpenSSL X.509 CSR", &type, 1, le_csr);
		if (what) {
			if (resourceval) {
				*resourceval = Z_LVAL_PP(val);
			}
			return (X509_REQ*)what;
		}
		return NULL;
	} else if (Z_TYPE_PP(val) != IS_STRING) {
		return NULL;
	}

	if (Z_STRLEN_PP(val) > 7 && memcmp(Z_STRVAL_PP(val), "file://", sizeof("file://") - 1) == 0) {
		filename = Z_STRVAL_PP(val) + (sizeof("file://") - 1);
	}
	if (filename) {
		if (php_openssl_open_base_dir_chk(filename TSRMLS_CC)) {
			return NULL;
		}
		in = BIO_new_file(filename, "r");
	} else {
		in = BIO_new_mem_buf(Z_STRVAL_PP(val), Z_STRLEN_PP(val));
	}
	csr = PEM_read_bio_X509_REQ(in, NULL,NULL,NULL);
	BIO_free(in);

	return csr;
}
/* }}} */

/* {{{ proto bool openssl_csr_export_to_file(resource csr, string outfilename [, bool notext=true])
   Exports a CSR to file */
PHP_FUNCTION(openssl_csr_export_to_file)
{
	X509_REQ * csr;
	zval * zcsr = NULL;
	zend_bool notext = 1;
	char * filename = NULL; int filename_len;
	BIO * bio_out;
	long csr_resource;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rp|b", &zcsr, &filename, &filename_len, &notext) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	csr = php_openssl_csr_from_zval(&zcsr, 0, &csr_resource TSRMLS_CC);
	if (csr == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get CSR from parameter 1");
		return;
	}

	if (php_openssl_open_base_dir_chk(filename TSRMLS_CC)) {
		return;
	}

	bio_out = BIO_new_file(filename, "w");
	if (bio_out) {
		if (!notext) {
			X509_REQ_print(bio_out, csr);
		}
		PEM_write_bio_X509_REQ(bio_out, csr);
		RETVAL_TRUE;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error opening file %s", filename);
	}

	if (csr_resource == -1 && csr) {
		X509_REQ_free(csr);
	}
	BIO_free(bio_out);
}
/* }}} */

/* {{{ proto bool openssl_csr_export(resource csr, string &out [, bool notext=true])
   Exports a CSR to file or a var */
PHP_FUNCTION(openssl_csr_export)
{
	X509_REQ * csr;
	zval * zcsr = NULL, *zout=NULL;
	zend_bool notext = 1;
	BIO * bio_out;

	long csr_resource;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz|b", &zcsr, &zout, &notext) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	csr = php_openssl_csr_from_zval(&zcsr, 0, &csr_resource TSRMLS_CC);
	if (csr == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get CSR from parameter 1");
		return;
	}

	/* export to a var */

	bio_out = BIO_new(BIO_s_mem());
	if (!notext) {
		X509_REQ_print(bio_out, csr);
	}

	if (PEM_write_bio_X509_REQ(bio_out, csr)) {
		BUF_MEM *bio_buf;

		BIO_get_mem_ptr(bio_out, &bio_buf);
		zval_dtor(zout);
		ZVAL_STRINGL(zout, bio_buf->data, bio_buf->length, 1);

		RETVAL_TRUE;
	}

	if (csr_resource == -1 && csr) {
		X509_REQ_free(csr);
	}
	BIO_free(bio_out);
}
/* }}} */

/* {{{ proto resource openssl_csr_sign(mixed csr, mixed x509, mixed priv_key, long days [, array config_args [, long serial]])
   Signs a cert with another CERT */
PHP_FUNCTION(openssl_csr_sign)
{
	zval ** zcert = NULL, **zcsr, **zpkey, *args = NULL;
	long num_days;
	long serial = 0L;
	X509 * cert = NULL, *new_cert = NULL;
	X509_REQ * csr;
	EVP_PKEY * key = NULL, *priv_key = NULL;
	long csr_resource, certresource = 0, keyresource = -1;
	int i;
	struct php_x509_request req;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZ!Zl|a!l", &zcsr, &zcert, &zpkey, &num_days, &args, &serial) == FAILURE)
		return;

	RETVAL_FALSE;
	PHP_SSL_REQ_INIT(&req);
	
	csr = php_openssl_csr_from_zval(zcsr, 0, &csr_resource TSRMLS_CC);
	if (csr == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get CSR from parameter 1");
		return;
	}
	if (zcert) {
		cert = php_openssl_x509_from_zval(zcert, 0, &certresource TSRMLS_CC);
		if (cert == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get cert from parameter 2");
			goto cleanup;
		}
	}
	priv_key = php_openssl_evp_from_zval(zpkey, 0, "", 1, &keyresource TSRMLS_CC);
	if (priv_key == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get private key from parameter 3");
		goto cleanup;
	}
	if (cert && !X509_check_private_key(cert, priv_key)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "private key does not correspond to signing cert");
		goto cleanup;
	}
	
	if (PHP_SSL_REQ_PARSE(&req, args) == FAILURE) {
		goto cleanup;
	}
	/* Check that the request matches the signature */
	key = X509_REQ_get_pubkey(csr);
	if (key == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error unpacking public key");
		goto cleanup;
	}
	i = X509_REQ_verify(csr, key);

	if (i < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Signature verification problems");
		goto cleanup;
	}
	else if (i == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Signature did not match the certificate request");
		goto cleanup;
	}
	
	/* Now we can get on with it */
	
	new_cert = X509_new();
	if (new_cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No memory");
		goto cleanup;
	}
	/* Version 3 cert */
	if (!X509_set_version(new_cert, 2))
		goto cleanup;

	ASN1_INTEGER_set(X509_get_serialNumber(new_cert), serial);
	
	X509_set_subject_name(new_cert, X509_REQ_get_subject_name(csr));

	if (cert == NULL) {
		cert = new_cert;
	}
	if (!X509_set_issuer_name(new_cert, X509_get_subject_name(cert))) {
		goto cleanup;
	}
	X509_gmtime_adj(X509_get_notBefore(new_cert), 0);
	X509_gmtime_adj(X509_get_notAfter(new_cert), (long)60*60*24*num_days);
	i = X509_set_pubkey(new_cert, key);
	if (!i) {
		goto cleanup;
	}
	if (req.extensions_section) {
		X509V3_CTX ctx;
		
		X509V3_set_ctx(&ctx, cert, new_cert, csr, NULL, 0);
		X509V3_set_conf_lhash(&ctx, req.req_config);
		if (!X509V3_EXT_add_conf(req.req_config, &ctx, req.extensions_section, new_cert)) {
			goto cleanup;
		}
	}

	/* Now sign it */
	if (!X509_sign(new_cert, priv_key, req.digest)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to sign it");
		goto cleanup;
	}
	
	/* Succeeded; lets return the cert */
	RETVAL_RESOURCE(zend_list_insert(new_cert, le_x509 TSRMLS_CC));
	new_cert = NULL;
	
cleanup:

	if (cert == new_cert) {
		cert = NULL;
	}
	PHP_SSL_REQ_DISPOSE(&req);

	if (keyresource == -1 && priv_key) {
		EVP_PKEY_free(priv_key);
	}
	if (key) {
		EVP_PKEY_free(key);
	}
	if (csr_resource == -1 && csr) {
		X509_REQ_free(csr);
	}
	if (certresource == -1 && cert) { 
		X509_free(cert);
	}
	if (new_cert) {
		X509_free(new_cert);
	}
}
/* }}} */

/* {{{ proto bool openssl_csr_new(array dn, resource &privkey [, array configargs [, array extraattribs]])
   Generates a privkey and CSR */
PHP_FUNCTION(openssl_csr_new)
{
	struct php_x509_request req;
	zval * args = NULL, * dn, *attribs = NULL;
	zval * out_pkey;
	X509_REQ * csr = NULL;
	int we_made_the_key = 1;
	long key_resource;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "az|a!a!", &dn, &out_pkey, &args, &attribs) == FAILURE) {
		return;
	}
	RETVAL_FALSE;
	
	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS) {
		/* Generate or use a private key */
		if (Z_TYPE_P(out_pkey) != IS_NULL) {
			req.priv_key = php_openssl_evp_from_zval(&out_pkey, 0, NULL, 0, &key_resource TSRMLS_CC);
			if (req.priv_key != NULL) {
				we_made_the_key = 0;
			}
		}
		if (req.priv_key == NULL) {
			php_openssl_generate_private_key(&req TSRMLS_CC);
		}
		if (req.priv_key == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to generate a private key");
		} else {
			csr = X509_REQ_new();
			if (csr) {
				if (php_openssl_make_REQ(&req, csr, dn, attribs TSRMLS_CC) == SUCCESS) {
					X509V3_CTX ext_ctx;

					X509V3_set_ctx(&ext_ctx, NULL, NULL, csr, NULL, 0);
					X509V3_set_conf_lhash(&ext_ctx, req.req_config);

					/* Add extensions */
					if (req.request_extensions_section && !X509V3_EXT_REQ_add_conf(req.req_config,
								&ext_ctx, req.request_extensions_section, csr))
					{
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error loading extension section %s", req.request_extensions_section);
					} else {
						RETVAL_TRUE;
						
						if (X509_REQ_sign(csr, req.priv_key, req.digest)) {
							RETVAL_RESOURCE(zend_list_insert(csr, le_csr TSRMLS_CC));
							csr = NULL;			
						} else {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error signing request");
						}

						if (we_made_the_key) {
							/* and a resource for the private key */
							zval_dtor(out_pkey);
							ZVAL_RESOURCE(out_pkey, zend_list_insert(req.priv_key, le_key TSRMLS_CC));
							req.priv_key = NULL; /* make sure the cleanup code doesn't zap it! */
						} else if (key_resource != -1) {
							req.priv_key = NULL; /* make sure the cleanup code doesn't zap it! */
						}
					}
				}
				else {
					if (!we_made_the_key) {
						/* if we have not made the key we are not supposed to zap it by calling dispose! */
						req.priv_key = NULL;
					}
				}
			}
		}
	}
	if (csr) {
		X509_REQ_free(csr);
	}
	PHP_SSL_REQ_DISPOSE(&req);
}
/* }}} */

/* {{{ proto mixed openssl_csr_get_subject(mixed csr)
   Returns the subject of a CERT or FALSE on error */
PHP_FUNCTION(openssl_csr_get_subject)
{
	zval ** zcsr;
	zend_bool use_shortnames = 1;
	long csr_resource;
	X509_NAME * subject;
	X509_REQ * csr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|b", &zcsr, &use_shortnames) == FAILURE) {
		return;
	}

	csr = php_openssl_csr_from_zval(zcsr, 0, &csr_resource TSRMLS_CC);

	if (csr == NULL) {
		RETURN_FALSE;
	}

	subject = X509_REQ_get_subject_name(csr);

	array_init(return_value);
	add_assoc_name_entry(return_value, NULL, subject, use_shortnames TSRMLS_CC);
	return;
}
/* }}} */

/* {{{ proto mixed openssl_csr_get_public_key(mixed csr)
	Returns the subject of a CERT or FALSE on error */
PHP_FUNCTION(openssl_csr_get_public_key)
{
	zval ** zcsr;
	zend_bool use_shortnames = 1;
	long csr_resource;

	X509_REQ * csr;
	EVP_PKEY *tpubkey;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|b", &zcsr, &use_shortnames) == FAILURE) {
		return;
	}

	csr = php_openssl_csr_from_zval(zcsr, 0, &csr_resource TSRMLS_CC);

	if (csr == NULL) {
		RETURN_FALSE;
	}

	tpubkey=X509_REQ_get_pubkey(csr);
	RETVAL_RESOURCE(zend_list_insert(tpubkey, le_key TSRMLS_CC));
	return;
}
/* }}} */

/* }}} */

/* {{{ EVP Public/Private key functions */

/* {{{ php_openssl_evp_from_zval
   Given a zval, coerce it into a EVP_PKEY object.
	It can be:
		1. private key resource from openssl_get_privatekey()
		2. X509 resource -> public key will be extracted from it
		3. if it starts with file:// interpreted as path to key file
		4. interpreted as the data from the cert/key file and interpreted in same way as openssl_get_privatekey()
		5. an array(0 => [items 2..4], 1 => passphrase)
		6. if val is a string (possibly starting with file:///) and it is not an X509 certificate, then interpret as public key
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
	zval tmp;

	Z_TYPE(tmp) = IS_NULL;

#define TMP_CLEAN \
	if (Z_TYPE(tmp) == IS_STRING) {\
		zval_dtor(&tmp); \
	} \
	return NULL;

	if (resourceval) {
		*resourceval = -1;
	}
	if (Z_TYPE_PP(val) == IS_ARRAY) {
		zval ** zphrase;
		
		/* get passphrase */

		if (zend_hash_index_find(HASH_OF(*val), 1, (void **)&zphrase) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key array must be of the form array(0 => key, 1 => phrase)");
			return NULL;
		}
		
		if (Z_TYPE_PP(zphrase) == IS_STRING) {
			passphrase = Z_STRVAL_PP(zphrase);
		} else {
			tmp = **zphrase;
			zval_copy_ctor(&tmp);
			convert_to_string(&tmp);
			passphrase = Z_STRVAL(tmp);
		}

		/* now set val to be the key param and continue */
		if (zend_hash_index_find(HASH_OF(*val), 0, (void **)&val) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key array must be of the form array(0 => key, 1 => phrase)");
			TMP_CLEAN;
		}
	}

	if (Z_TYPE_PP(val) == IS_RESOURCE) {
		void * what;
		int type;

		what = zend_fetch_resource(val TSRMLS_CC, -1, "OpenSSL X.509/key", &type, 2, le_x509, le_key);
		if (!what) {
			TMP_CLEAN;
		}
		if (resourceval) { 
			*resourceval = Z_LVAL_PP(val);
		}
		if (type == le_x509) {
			/* extract key from cert, depending on public_key param */
			cert = (X509*)what;
			free_cert = 0;
		} else if (type == le_key) {
			int is_priv;

			is_priv = php_openssl_is_private_key((EVP_PKEY*)what TSRMLS_CC);

			/* check whether it is actually a private key if requested */
			if (!public_key && !is_priv) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "supplied key param is a public key");
				TMP_CLEAN;
			}

			if (public_key && is_priv) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Don't know how to get public key from this private key");
				TMP_CLEAN;
			} else {
				if (Z_TYPE(tmp) == IS_STRING) {
					zval_dtor(&tmp);
				}
				/* got the key - return it */
				return (EVP_PKEY*)what;
			}
		} else {
			/* other types could be used here - eg: file pointers and read in the data from them */
			TMP_CLEAN;
		}
	} else {
		/* force it to be a string and check if it refers to a file */
		/* passing non string values leaks, object uses toString, it returns NULL 
		 * See bug38255.phpt 
		 */
		if (!(Z_TYPE_PP(val) == IS_STRING || Z_TYPE_PP(val) == IS_OBJECT)) {
			TMP_CLEAN;
		}
		convert_to_string_ex(val);

		if (Z_STRLEN_PP(val) > 7 && memcmp(Z_STRVAL_PP(val), "file://", sizeof("file://") - 1) == 0) {
			filename = Z_STRVAL_PP(val) + (sizeof("file://") - 1);
		}
		/* it's an X509 file/cert of some kind, and we need to extract the data from that */
		if (public_key) {
			cert = php_openssl_x509_from_zval(val, 0, &cert_res TSRMLS_CC);
			free_cert = (cert_res == -1);
			/* actual extraction done later */
			if (!cert) {
				/* not a X509 certificate, try to retrieve public key */
				BIO* in;
				if (filename) {
					in = BIO_new_file(filename, "r");
				} else {
					in = BIO_new_mem_buf(Z_STRVAL_PP(val), Z_STRLEN_PP(val));
				}
				if (in == NULL) {
					TMP_CLEAN;
				}
				key = PEM_read_bio_PUBKEY(in, NULL,NULL, NULL);
				BIO_free(in);
			}
		} else {
			/* we want the private key */
			BIO *in;

			if (filename) {
				if (php_openssl_open_base_dir_chk(filename TSRMLS_CC)) {
					TMP_CLEAN;
				}
				in = BIO_new_file(filename, "r");
			} else {
				in = BIO_new_mem_buf(Z_STRVAL_PP(val), Z_STRLEN_PP(val));
			}

			if (in == NULL) {
				TMP_CLEAN;
			}
			key = PEM_read_bio_PrivateKey(in, NULL,NULL, passphrase);
			BIO_free(in);
		}
	}

	if (public_key && cert && key == NULL) {
		/* extract public key from X509 cert */
		key = (EVP_PKEY *) X509_get_pubkey(cert);
	}

	if (free_cert && cert) {
		X509_free(cert);
	}
	if (key && makeresource && resourceval) {
		*resourceval = ZEND_REGISTER_RESOURCE(NULL, key, le_key);
	}
	if (Z_TYPE(tmp) == IS_STRING) {
		zval_dtor(&tmp);
	}
	return key;
}
/* }}} */

/* {{{ php_openssl_generate_private_key */
static EVP_PKEY * php_openssl_generate_private_key(struct php_x509_request * req TSRMLS_DC)
{
	char * randfile = NULL;
	int egdsocket, seeded;
	EVP_PKEY * return_val = NULL;
	
	if (req->priv_key_bits < MIN_KEY_LENGTH) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "private key length is too short; it needs to be at least %d bits, not %d",
				MIN_KEY_LENGTH, req->priv_key_bits);
		return NULL;
	}

	randfile = CONF_get_string(req->req_config, req->section_name, "RANDFILE");
	php_openssl_load_rand_file(randfile, &egdsocket, &seeded TSRMLS_CC);
	
	if ((req->priv_key = EVP_PKEY_new()) != NULL) {
		switch(req->priv_key_type) {
			case OPENSSL_KEYTYPE_RSA:
				PHP_OPENSSL_RAND_ADD_TIME();
				if (EVP_PKEY_assign_RSA(req->priv_key, RSA_generate_key(req->priv_key_bits, 0x10001, NULL, NULL))) {
					return_val = req->priv_key;
				}
				break;
#if !defined(NO_DSA) && defined(HAVE_DSA_DEFAULT_METHOD)
			case OPENSSL_KEYTYPE_DSA:
				PHP_OPENSSL_RAND_ADD_TIME();
				{
					DSA *dsapar = DSA_generate_parameters(req->priv_key_bits, NULL, 0, NULL, NULL, NULL, NULL);
					if (dsapar) {
						DSA_set_method(dsapar, DSA_get_default_method());
						if (DSA_generate_key(dsapar)) {
							if (EVP_PKEY_assign_DSA(req->priv_key, dsapar)) {
								return_val = req->priv_key;
							}
						} else {
							DSA_free(dsapar);
						}
					}
				}
				break;
#endif
#if !defined(NO_DH)
			case OPENSSL_KEYTYPE_DH:
				PHP_OPENSSL_RAND_ADD_TIME();
				{
					DH *dhpar = DH_generate_parameters(req->priv_key_bits, 2, NULL, NULL);
					int codes = 0;

					if (dhpar) {
						DH_set_method(dhpar, DH_get_default_method());
						if (DH_check(dhpar, &codes) && codes == 0 && DH_generate_key(dhpar)) {
							if (EVP_PKEY_assign_DH(req->priv_key, dhpar)) {
								return_val = req->priv_key;
							}
						} else {
							DH_free(dhpar);
						}
					}
				}
				break;
#endif
			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unsupported private key type");
		}
	}

	php_openssl_write_rand_file(randfile, egdsocket, seeded);
	
	if (return_val == NULL) {
		EVP_PKEY_free(req->priv_key);
		req->priv_key = NULL;
		return NULL;
	}
	
	return return_val;
}
/* }}} */

/* {{{ php_openssl_is_private_key
	Check whether the supplied key is a private key by checking if the secret prime factors are set */
static int php_openssl_is_private_key(EVP_PKEY* pkey TSRMLS_DC)
{
	assert(pkey != NULL);

	switch (pkey->type) {
#ifndef NO_RSA
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			assert(pkey->pkey.rsa != NULL);
			if (pkey->pkey.rsa != NULL && (NULL == pkey->pkey.rsa->p || NULL == pkey->pkey.rsa->q)) {
				return 0;
			}
			break;
#endif
#ifndef NO_DSA
		case EVP_PKEY_DSA:
		case EVP_PKEY_DSA1:
		case EVP_PKEY_DSA2:
		case EVP_PKEY_DSA3:
		case EVP_PKEY_DSA4:
			assert(pkey->pkey.dsa != NULL);

			if (NULL == pkey->pkey.dsa->p || NULL == pkey->pkey.dsa->q || NULL == pkey->pkey.dsa->priv_key){ 
				return 0;
			}
			break;
#endif
#ifndef NO_DH
		case EVP_PKEY_DH:
			assert(pkey->pkey.dh != NULL);

			if (NULL == pkey->pkey.dh->p || NULL == pkey->pkey.dh->priv_key) {
				return 0;
			}
			break;
#endif
#ifdef HAVE_EVP_PKEY_EC
		case EVP_PKEY_EC:
			assert(pkey->pkey.ec != NULL);

			if ( NULL == EC_KEY_get0_private_key(pkey->pkey.ec)) {
				return 0;
			}
			break;
#endif
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key type not supported in this PHP build!");
			break;
	}
	return 1;
}
/* }}} */

#define OPENSSL_PKEY_GET_BN(_type, _name) do {							\
		if (pkey->pkey._type->_name != NULL) {							\
			int len = BN_num_bytes(pkey->pkey._type->_name);			\
			char *str = emalloc(len + 1);								\
			BN_bn2bin(pkey->pkey._type->_name, (unsigned char*)str);	\
			str[len] = 0;                                           	\
			add_assoc_stringl(_type, #_name, str, len, 0);				\
		}																\
	} while (0)

#define OPENSSL_PKEY_SET_BN(_ht, _type, _name) do {						\
		zval **bn;														\
		if (zend_hash_find(_ht, #_name, sizeof(#_name),	(void**)&bn) == SUCCESS && \
				Z_TYPE_PP(bn) == IS_STRING) {							\
			_type->_name = BN_bin2bn(									\
				(unsigned char*)Z_STRVAL_PP(bn),						\
	 			Z_STRLEN_PP(bn), NULL);									\
	    }                                                               \
	} while (0);

/* {{{ php_openssl_pkey_init_dsa */
zend_bool php_openssl_pkey_init_dsa(DSA *dsa)
{
	if (!dsa->p || !dsa->q || !dsa->g) {
		return 0;
	}
	if (dsa->priv_key || dsa->pub_key) {
		return 1;
	}
	PHP_OPENSSL_RAND_ADD_TIME();
	if (!DSA_generate_key(dsa)) {
		return 0;
	}
	/* if BN_mod_exp return -1, then DSA_generate_key succeed for failed key
	 * so we need to double check that public key is created */
	if (!dsa->pub_key || BN_is_zero(dsa->pub_key)) {
		return 0;
	}
	/* all good */
	return 1;
}
/* }}} */

/* {{{ php_openssl_pkey_init_dh */
zend_bool php_openssl_pkey_init_dh(DH *dh)
{
	if (!dh->p || !dh->g) {
		return 0;
	}
	if (dh->pub_key) {
		return 1;
	}
	PHP_OPENSSL_RAND_ADD_TIME();
	if (!DH_generate_key(dh)) {
		return 0;
	}
	/* all good */
	return 1;
}
/* }}} */

/* {{{ proto resource openssl_pkey_new([array configargs])
   Generates a new private key */
PHP_FUNCTION(openssl_pkey_new)
{
	struct php_x509_request req;
	zval * args = NULL;
	zval **data;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|a!", &args) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	if (args && Z_TYPE_P(args) == IS_ARRAY) {
		EVP_PKEY *pkey;

		if (zend_hash_find(Z_ARRVAL_P(args), "rsa", sizeof("rsa"), (void**)&data) == SUCCESS &&
		    Z_TYPE_PP(data) == IS_ARRAY) {
		    pkey = EVP_PKEY_new();
		    if (pkey) {
				RSA *rsa = RSA_new();
				if (rsa) {
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, n);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, e);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, d);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, p);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, q);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, dmp1);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, dmq1);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), rsa, iqmp);
					if (rsa->n && rsa->d) {
						if (EVP_PKEY_assign_RSA(pkey, rsa)) {
							RETURN_RESOURCE(zend_list_insert(pkey, le_key TSRMLS_CC));
						}
					}
					RSA_free(rsa);
				}
				EVP_PKEY_free(pkey);
			}
			RETURN_FALSE;
		} else if (zend_hash_find(Z_ARRVAL_P(args), "dsa", sizeof("dsa"), (void**)&data) == SUCCESS &&
		           Z_TYPE_PP(data) == IS_ARRAY) {
		    pkey = EVP_PKEY_new();
		    if (pkey) {
				DSA *dsa = DSA_new();
				if (dsa) {
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dsa, p);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dsa, q);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dsa, g);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dsa, priv_key);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dsa, pub_key);
					if (php_openssl_pkey_init_dsa(dsa)) {
						if (EVP_PKEY_assign_DSA(pkey, dsa)) {
							RETURN_RESOURCE(zend_list_insert(pkey, le_key TSRMLS_CC));
						}
					}
					DSA_free(dsa);
				}
				EVP_PKEY_free(pkey);
			}
			RETURN_FALSE;
		} else if (zend_hash_find(Z_ARRVAL_P(args), "dh", sizeof("dh"), (void**)&data) == SUCCESS &&
		           Z_TYPE_PP(data) == IS_ARRAY) {
		    pkey = EVP_PKEY_new();
		    if (pkey) {
				DH *dh = DH_new();
				if (dh) {
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dh, p);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dh, g);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dh, priv_key);
					OPENSSL_PKEY_SET_BN(Z_ARRVAL_PP(data), dh, pub_key);
					if (php_openssl_pkey_init_dh(dh)) {
						if (EVP_PKEY_assign_DH(pkey, dh)) {
							RETURN_RESOURCE(zend_list_insert(pkey, le_key TSRMLS_CC));
						}
					}
					DH_free(dh);
				}
				EVP_PKEY_free(pkey);
			}
			RETURN_FALSE;
		}
	} 

	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS)
	{
		if (php_openssl_generate_private_key(&req TSRMLS_CC)) {
			/* pass back a key resource */
			RETVAL_RESOURCE(zend_list_insert(req.priv_key, le_key TSRMLS_CC));
			/* make sure the cleanup code doesn't zap it! */
			req.priv_key = NULL;
		}
	}
	PHP_SSL_REQ_DISPOSE(&req);
}
/* }}} */

/* {{{ proto bool openssl_pkey_export_to_file(mixed key, string outfilename [, string passphrase, array config_args)
   Gets an exportable representation of a key into a file */
PHP_FUNCTION(openssl_pkey_export_to_file)
{
	struct php_x509_request req;
	zval ** zpkey, * args = NULL;
	char * passphrase = NULL; 
	int passphrase_len = 0;
	char * filename = NULL; 
	int filename_len = 0;
	long key_resource = -1;
	int pem_write = 0;
	EVP_PKEY * key;
	BIO * bio_out = NULL;
	const EVP_CIPHER * cipher;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zp|s!a!", &zpkey, &filename, &filename_len, &passphrase, &passphrase_len, &args) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	key = php_openssl_evp_from_zval(zpkey, 0, passphrase, 0, &key_resource TSRMLS_CC);

	if (key == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get key from parameter 1");
		RETURN_FALSE;
	}
	
	if (php_openssl_open_base_dir_chk(filename TSRMLS_CC)) {
		RETURN_FALSE;
	}
	
	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS) {
		bio_out = BIO_new_file(filename, "w");

		if (passphrase && req.priv_key_encrypt) {
			if (req.priv_key_encrypt_cipher) {
				cipher = req.priv_key_encrypt_cipher;
			} else {
				cipher = (EVP_CIPHER *) EVP_des_ede3_cbc();
			}
		} else {
			cipher = NULL;
		}

		switch (EVP_PKEY_type(key->type)) {
#ifdef HAVE_EVP_PKEY_EC
			case EVP_PKEY_EC:
				pem_write = PEM_write_bio_ECPrivateKey(bio_out, EVP_PKEY_get1_EC_KEY(key), cipher, (unsigned char *)passphrase, passphrase_len, NULL, NULL);
				break;
#endif
			default:
				pem_write = PEM_write_bio_PrivateKey(bio_out, key, cipher, (unsigned char *)passphrase, passphrase_len, NULL, NULL);
				break;
		}

		if (pem_write) {
			/* Success!
			 * If returning the output as a string, do so now */
			RETVAL_TRUE;
		}
	}
	PHP_SSL_REQ_DISPOSE(&req);

	if (key_resource == -1 && key) {
		EVP_PKEY_free(key);
	}
	if (bio_out) {
		BIO_free(bio_out);
	}
}
/* }}} */

/* {{{ proto bool openssl_pkey_export(mixed key, &mixed out [, string passphrase [, array config_args]])
   Gets an exportable representation of a key into a string or file */
PHP_FUNCTION(openssl_pkey_export)
{
	struct php_x509_request req;
	zval ** zpkey, * args = NULL, *out;
	char * passphrase = NULL; 
	int passphrase_len = 0;
	long key_resource = -1;
	int pem_write = 0;
	EVP_PKEY * key;
	BIO * bio_out = NULL;
	const EVP_CIPHER * cipher;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zz|s!a!", &zpkey, &out, &passphrase, &passphrase_len, &args) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	key = php_openssl_evp_from_zval(zpkey, 0, passphrase, 0, &key_resource TSRMLS_CC);

	if (key == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get key from parameter 1");
		RETURN_FALSE;
	}
	
	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS) {
		bio_out = BIO_new(BIO_s_mem());

		if (passphrase && req.priv_key_encrypt) {
			if (req.priv_key_encrypt_cipher) {
				cipher = req.priv_key_encrypt_cipher;
			} else {
				cipher = (EVP_CIPHER *) EVP_des_ede3_cbc();
			}
		} else {
			cipher = NULL;
		}

		switch (EVP_PKEY_type(key->type)) {
#ifdef HAVE_EVP_PKEY_EC
			case EVP_PKEY_EC:
				pem_write = PEM_write_bio_ECPrivateKey(bio_out, EVP_PKEY_get1_EC_KEY(key), cipher, (unsigned char *)passphrase, passphrase_len, NULL, NULL);
				break;
#endif
			default:
				pem_write = PEM_write_bio_PrivateKey(bio_out, key, cipher, (unsigned char *)passphrase, passphrase_len, NULL, NULL);
				break;
		}

		if (pem_write) {
			/* Success!
			 * If returning the output as a string, do so now */

			char * bio_mem_ptr;
			long bio_mem_len;
			RETVAL_TRUE;

			bio_mem_len = BIO_get_mem_data(bio_out, &bio_mem_ptr);
			zval_dtor(out);
			ZVAL_STRINGL(out, bio_mem_ptr, bio_mem_len, 1);
		}
	}
	PHP_SSL_REQ_DISPOSE(&req);

	if (key_resource == -1 && key) {
		EVP_PKEY_free(key);
	}
	if (bio_out) {
		BIO_free(bio_out);
	}
}
/* }}} */

/* {{{ proto int openssl_pkey_get_public(mixed cert)
   Gets public key from X.509 certificate */
PHP_FUNCTION(openssl_pkey_get_public)
{
	zval **cert;
	EVP_PKEY *pkey;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &cert) == FAILURE) {
		return;
	}
	Z_TYPE_P(return_value) = IS_RESOURCE;
	pkey = php_openssl_evp_from_zval(cert, 1, NULL, 1, &Z_LVAL_P(return_value) TSRMLS_CC);

	if (pkey == NULL) {
		RETURN_FALSE;
	}
	zend_list_addref(Z_LVAL_P(return_value));
}
/* }}} */

/* {{{ proto void openssl_pkey_free(int key)
   Frees a key */
PHP_FUNCTION(openssl_pkey_free)
{
	zval *key;
	EVP_PKEY *pkey;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &key) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(pkey, EVP_PKEY *, &key, -1, "OpenSSL key", le_key);
	zend_list_delete(Z_LVAL_P(key));
}
/* }}} */

/* {{{ proto int openssl_pkey_get_private(string key [, string passphrase])
   Gets private keys */
PHP_FUNCTION(openssl_pkey_get_private)
{
	zval **cert;
	EVP_PKEY *pkey;
	char * passphrase = "";
	int passphrase_len = sizeof("")-1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|s", &cert, &passphrase, &passphrase_len) == FAILURE) {
		return;
	}
	Z_TYPE_P(return_value) = IS_RESOURCE;
	pkey = php_openssl_evp_from_zval(cert, 0, passphrase, 1, &Z_LVAL_P(return_value) TSRMLS_CC);

	if (pkey == NULL) {
		RETURN_FALSE;
	}
	zend_list_addref(Z_LVAL_P(return_value));
}

/* }}} */

/* {{{ proto resource openssl_pkey_get_details(resource key)
	returns an array with the key details (bits, pkey, type)*/
PHP_FUNCTION(openssl_pkey_get_details)
{
	zval *key;
	EVP_PKEY *pkey;
	BIO *out;
	unsigned int pbio_len;
	char *pbio;
	long ktype;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &key) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(pkey, EVP_PKEY *, &key, -1, "OpenSSL key", le_key);
	if (!pkey) {
		RETURN_FALSE;
	}
	out = BIO_new(BIO_s_mem());
	PEM_write_bio_PUBKEY(out, pkey);
	pbio_len = BIO_get_mem_data(out, &pbio);

	array_init(return_value);
	add_assoc_long(return_value, "bits", EVP_PKEY_bits(pkey));
	add_assoc_stringl(return_value, "key", pbio, pbio_len, 1);
	/*TODO: Use the real values once the openssl constants are used 
	 * See the enum at the top of this file
	 */
	switch (EVP_PKEY_type(pkey->type)) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			ktype = OPENSSL_KEYTYPE_RSA;

			if (pkey->pkey.rsa != NULL) {
				zval *rsa;

				ALLOC_INIT_ZVAL(rsa);
				array_init(rsa);
				OPENSSL_PKEY_GET_BN(rsa, n);
				OPENSSL_PKEY_GET_BN(rsa, e);
				OPENSSL_PKEY_GET_BN(rsa, d);
				OPENSSL_PKEY_GET_BN(rsa, p);
				OPENSSL_PKEY_GET_BN(rsa, q);
				OPENSSL_PKEY_GET_BN(rsa, dmp1);
				OPENSSL_PKEY_GET_BN(rsa, dmq1);
				OPENSSL_PKEY_GET_BN(rsa, iqmp);
				add_assoc_zval(return_value, "rsa", rsa);
			}

			break;	
		case EVP_PKEY_DSA:
		case EVP_PKEY_DSA2:
		case EVP_PKEY_DSA3:
		case EVP_PKEY_DSA4:
			ktype = OPENSSL_KEYTYPE_DSA;

			if (pkey->pkey.dsa != NULL) {
				zval *dsa;

				ALLOC_INIT_ZVAL(dsa);
				array_init(dsa);
				OPENSSL_PKEY_GET_BN(dsa, p);
				OPENSSL_PKEY_GET_BN(dsa, q);
				OPENSSL_PKEY_GET_BN(dsa, g);
				OPENSSL_PKEY_GET_BN(dsa, priv_key);
				OPENSSL_PKEY_GET_BN(dsa, pub_key);
				add_assoc_zval(return_value, "dsa", dsa);
			}
			break;
		case EVP_PKEY_DH:
			
			ktype = OPENSSL_KEYTYPE_DH;

			if (pkey->pkey.dh != NULL) {
				zval *dh;

				ALLOC_INIT_ZVAL(dh);
				array_init(dh);
				OPENSSL_PKEY_GET_BN(dh, p);
				OPENSSL_PKEY_GET_BN(dh, g);
				OPENSSL_PKEY_GET_BN(dh, priv_key);
				OPENSSL_PKEY_GET_BN(dh, pub_key);
				add_assoc_zval(return_value, "dh", dh);
			}

			break;
#ifdef HAVE_EVP_PKEY_EC
		case EVP_PKEY_EC:
			ktype = OPENSSL_KEYTYPE_EC;
			if (pkey->pkey.ec != NULL) {
				zval *ec;
				const EC_GROUP *ec_group;
				int nid;
				char *crv_sn;
				ASN1_OBJECT *obj;
				// openssl recommends a buffer length of 80
				char oir_buf[80];

				ec_group = EC_KEY_get0_group(EVP_PKEY_get1_EC_KEY(pkey));

				// Curve nid (numerical identifier) used for ASN1 mapping
				nid = EC_GROUP_get_curve_name(ec_group);
				if (nid == NID_undef) {
					break;
				}
				ALLOC_INIT_ZVAL(ec);
				array_init(ec);

				// Short object name
				crv_sn = (char*) OBJ_nid2sn(nid);
				if (crv_sn != NULL) {
					add_assoc_string(ec, "curve_name", crv_sn, 1);
				}

				obj = OBJ_nid2obj(nid);
				if (obj != NULL) {
					int oir_len = OBJ_obj2txt(oir_buf, sizeof(oir_buf), obj, 1);
					add_assoc_stringl(ec, "curve_oid", (char*)oir_buf, oir_len, 1);
					ASN1_OBJECT_free(obj);
				}

				add_assoc_zval(return_value, "ec", ec);
			}
			break;
#endif
		default:
			ktype = -1;
			break;
	}
	add_assoc_long(return_value, "type", ktype);

	BIO_free(out);
}
/* }}} */

/* }}} */

#if OPENSSL_VERSION_NUMBER >= 0x10000000L

/* {{{ proto string openssl_pbkdf2(string password, string salt, long key_length, long iterations [, string digest_method = "sha1"])
   Generates a PKCS5 v2 PBKDF2 string, defaults to sha1 */
PHP_FUNCTION(openssl_pbkdf2)
{
	long key_length = 0, iterations = 0;
	char *password; int password_len;
	char *salt; int salt_len;
	char *method; int method_len = 0;
	unsigned char *out_buffer;

	const EVP_MD *digest;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssll|s",
				&password, &password_len,
				&salt, &salt_len,
				&key_length, &iterations,
				&method, &method_len) == FAILURE) {
		return;
	}

	if (key_length <= 0 || key_length > INT_MAX) {
		RETURN_FALSE;
	}

	if (method_len) {
		digest = EVP_get_digestbyname(method);
	} else {
		digest = EVP_sha1();
	}

	if (!digest) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm");
		RETURN_FALSE;
	}

	out_buffer = emalloc(key_length + 1);
	out_buffer[key_length] = '\0';

	if (PKCS5_PBKDF2_HMAC(password, password_len, (unsigned char *)salt, salt_len, iterations, digest, key_length, out_buffer) == 1) {
		RETVAL_STRINGL((char *)out_buffer, key_length, 0);
	} else {
		efree(out_buffer);
		RETURN_FALSE;
	}
}
/* }}} */

#endif

/* {{{ PKCS7 S/MIME functions */

/* {{{ proto bool openssl_pkcs7_verify(string filename, long flags [, string signerscerts [, array cainfo [, string extracerts [, string content]]]])
   Verifys that the data block is intact, the signer is who they say they are, and returns the CERTs of the signers */
PHP_FUNCTION(openssl_pkcs7_verify)
{
	X509_STORE * store = NULL;
	zval * cainfo = NULL;
	STACK_OF(X509) *signers= NULL;
	STACK_OF(X509) *others = NULL;
	PKCS7 * p7 = NULL;
	BIO * in = NULL, * datain = NULL, * dataout = NULL;
	long flags = 0;
	char * filename; int filename_len;
	char * extracerts = NULL; int extracerts_len = 0;
	char * signersfilename = NULL; int signersfilename_len = 0;
	char * datafilename = NULL; int datafilename_len = 0;
	
	RETVAL_LONG(-1);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "pl|papp", &filename, &filename_len,
				&flags, &signersfilename, &signersfilename_len, &cainfo,
				&extracerts, &extracerts_len, &datafilename, &datafilename_len) == FAILURE) {
		return;
	}
	
	if (extracerts) {
		others = load_all_certs_from_file(extracerts);
		if (others == NULL) {
			goto clean_exit;
		}
	}

	flags = flags & ~PKCS7_DETACHED;

	store = setup_verify(cainfo TSRMLS_CC);

	if (!store) {
		goto clean_exit;
	}
	if (php_openssl_open_base_dir_chk(filename TSRMLS_CC)) {
		goto clean_exit;
	}

	in = BIO_new_file(filename, (flags & PKCS7_BINARY) ? "rb" : "r");
	if (in == NULL) {
		goto clean_exit;
	}
	p7 = SMIME_read_PKCS7(in, &datain);
	if (p7 == NULL) {
#if DEBUG_SMIME
		zend_printf("SMIME_read_PKCS7 failed\n");
#endif
		goto clean_exit;
	}

	if (datafilename) {

		if (php_openssl_open_base_dir_chk(datafilename TSRMLS_CC)) {
			goto clean_exit;
		}

		dataout = BIO_new_file(datafilename, "w");
		if (dataout == NULL) {
			goto clean_exit;
		}
	}
#if DEBUG_SMIME
	zend_printf("Calling PKCS7 verify\n");
#endif

	if (PKCS7_verify(p7, others, store, datain, dataout, flags)) {

		RETVAL_TRUE;

		if (signersfilename) {
			BIO *certout;
		
			if (php_openssl_open_base_dir_chk(signersfilename TSRMLS_CC)) {
				goto clean_exit;
			}
		
			certout = BIO_new_file(signersfilename, "w");
			if (certout) {
				int i;
				signers = PKCS7_get0_signers(p7, NULL, flags);

				for(i = 0; i < sk_X509_num(signers); i++) {
					PEM_write_bio_X509(certout, sk_X509_value(signers, i));
				}
				BIO_free(certout);
				sk_X509_free(signers);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "signature OK, but cannot open %s for writing", signersfilename);
				RETVAL_LONG(-1);
			}
		}
		goto clean_exit;
	} else {
		RETVAL_FALSE;
	}
clean_exit:
	X509_STORE_free(store);
	BIO_free(datain);
	BIO_free(in);
	BIO_free(dataout);
	PKCS7_free(p7);
	sk_X509_free(others);
}
/* }}} */

/* {{{ proto bool openssl_pkcs7_encrypt(string infile, string outfile, mixed recipcerts, array headers [, long flags [, long cipher]])
   Encrypts the message in the file named infile with the certificates in recipcerts and output the result to the file named outfile */
PHP_FUNCTION(openssl_pkcs7_encrypt)
{
	zval ** zrecipcerts, * zheaders = NULL;
	STACK_OF(X509) * recipcerts = NULL;
	BIO * infile = NULL, * outfile = NULL;
	long flags = 0;
	PKCS7 * p7 = NULL;
	HashPosition hpos;
	zval ** zcertval;
	X509 * cert;
	const EVP_CIPHER *cipher = NULL;
	long cipherid = PHP_OPENSSL_CIPHER_DEFAULT;
	uint strindexlen;
	ulong intindex;
	char * strindex;
	char * infilename = NULL;	int infilename_len;
	char * outfilename = NULL;	int outfilename_len;
	
	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ppZa!|ll", &infilename, &infilename_len,
				&outfilename, &outfilename_len, &zrecipcerts, &zheaders, &flags, &cipherid) == FAILURE)
		return;

	
	if (php_openssl_open_base_dir_chk(infilename TSRMLS_CC) || php_openssl_open_base_dir_chk(outfilename TSRMLS_CC)) {
		return;
	}

	infile = BIO_new_file(infilename, "r");
	if (infile == NULL) {
		goto clean_exit;
	}

	outfile = BIO_new_file(outfilename, "w");
	if (outfile == NULL) { 
		goto clean_exit;
	}

	recipcerts = sk_X509_new_null();

	/* get certs */
	if (Z_TYPE_PP(zrecipcerts) == IS_ARRAY) {
		zend_hash_internal_pointer_reset_ex(HASH_OF(*zrecipcerts), &hpos);
		while(zend_hash_get_current_data_ex(HASH_OF(*zrecipcerts), (void**)&zcertval, &hpos) == SUCCESS) {
			long certresource;

			cert = php_openssl_x509_from_zval(zcertval, 0, &certresource TSRMLS_CC);
			if (cert == NULL) {
				goto clean_exit;
			}

			if (certresource != -1) {
				/* we shouldn't free this particular cert, as it is a resource.
					make a copy and push that on the stack instead */
				cert = X509_dup(cert);
				if (cert == NULL) {
					goto clean_exit;
				}
			}
			sk_X509_push(recipcerts, cert);

			zend_hash_move_forward_ex(HASH_OF(*zrecipcerts), &hpos);
		}
	} else {
		/* a single certificate */
		long certresource;

		cert = php_openssl_x509_from_zval(zrecipcerts, 0, &certresource TSRMLS_CC);
		if (cert == NULL) {
			goto clean_exit;
		}

		if (certresource != -1) {
			/* we shouldn't free this particular cert, as it is a resource.
				make a copy and push that on the stack instead */
			cert = X509_dup(cert);
			if (cert == NULL) {
				goto clean_exit;
			}
		}
		sk_X509_push(recipcerts, cert);
	}

	/* sanity check the cipher */
	cipher = php_openssl_get_evp_cipher_from_algo(cipherid);
	if (cipher == NULL) {
		/* shouldn't happen */
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to get cipher");
		goto clean_exit;
	}

	p7 = PKCS7_encrypt(recipcerts, infile, (EVP_CIPHER*)cipher, flags);

	if (p7 == NULL) {
		goto clean_exit;
	}

	/* tack on extra headers */
	if (zheaders) {
		zend_hash_internal_pointer_reset_ex(HASH_OF(zheaders), &hpos);
		while(zend_hash_get_current_data_ex(HASH_OF(zheaders), (void**)&zcertval, &hpos) == SUCCESS) {
			strindex = NULL;
			zend_hash_get_current_key_ex(HASH_OF(zheaders), &strindex, &strindexlen, &intindex, 0, &hpos);

			convert_to_string_ex(zcertval);

			if (strindex) {
				BIO_printf(outfile, "%s: %s\n", strindex, Z_STRVAL_PP(zcertval));
			} else {
				BIO_printf(outfile, "%s\n", Z_STRVAL_PP(zcertval));
			}

			zend_hash_move_forward_ex(HASH_OF(zheaders), &hpos);
		}
	}

	(void)BIO_reset(infile);

	/* write the encrypted data */
	SMIME_write_PKCS7(outfile, p7, infile, flags);

	RETVAL_TRUE;

clean_exit:
	PKCS7_free(p7);
	BIO_free(infile);
	BIO_free(outfile);
	if (recipcerts) {
		sk_X509_pop_free(recipcerts, X509_free);
	}
}
/* }}} */

/* {{{ proto bool openssl_pkcs7_sign(string infile, string outfile, mixed signcert, mixed signkey, array headers [, long flags [, string extracertsfilename]])
   Signs the MIME message in the file named infile with signcert/signkey and output the result to file name outfile. headers lists plain text headers to exclude from the signed portion of the message, and should include to, from and subject as a minimum */

PHP_FUNCTION(openssl_pkcs7_sign)
{
	zval ** zcert, ** zprivkey, * zheaders;
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
	char * infilename;	int infilename_len;
	char * outfilename;	int outfilename_len;
	char * extracertsfilename = NULL; int extracertsfilename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ppZZa!|lp!",
				&infilename, &infilename_len, &outfilename, &outfilename_len,
				&zcert, &zprivkey, &zheaders, &flags, &extracertsfilename,
				&extracertsfilename_len) == FAILURE) {
		return;
	}
	
	RETVAL_FALSE;

	if (extracertsfilename) {
		others = load_all_certs_from_file(extracertsfilename);
		if (others == NULL) { 
			goto clean_exit;
		}
	}

	privkey = php_openssl_evp_from_zval(zprivkey, 0, "", 0, &keyresource TSRMLS_CC);
	if (privkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error getting private key");
		goto clean_exit;
	}

	cert = php_openssl_x509_from_zval(zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error getting cert");
		goto clean_exit;
	}

	if (php_openssl_open_base_dir_chk(infilename TSRMLS_CC) || php_openssl_open_base_dir_chk(outfilename TSRMLS_CC)) {
		goto clean_exit;
	}

	infile = BIO_new_file(infilename, "r");
	if (infile == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error opening input file %s!", infilename);
		goto clean_exit;
	}

	outfile = BIO_new_file(outfilename, "w");
	if (outfile == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error opening output file %s!", outfilename);
		goto clean_exit;
	}

	p7 = PKCS7_sign(cert, privkey, others, infile, flags);
	if (p7 == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error creating PKCS7 structure!");
		goto clean_exit;
	}

	(void)BIO_reset(infile);

	/* tack on extra headers */
	if (zheaders) {
		zend_hash_internal_pointer_reset_ex(HASH_OF(zheaders), &hpos);
		while(zend_hash_get_current_data_ex(HASH_OF(zheaders), (void**)&hval, &hpos) == SUCCESS) {
			strindex = NULL;
			zend_hash_get_current_key_ex(HASH_OF(zheaders), &strindex, &strindexlen, &intindex, 0, &hpos);

			convert_to_string_ex(hval);

			if (strindex) {
				BIO_printf(outfile, "%s: %s\n", strindex, Z_STRVAL_PP(hval));
			} else {
				BIO_printf(outfile, "%s\n", Z_STRVAL_PP(hval));
			}
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
	if (others) {
		sk_X509_pop_free(others, X509_free);
	}
	if (privkey && keyresource == -1) {
		EVP_PKEY_free(privkey);
	}
	if (cert && certresource == -1) {
		X509_free(cert);
	}
}
/* }}} */

/* {{{ proto bool openssl_pkcs7_decrypt(string infilename, string outfilename, mixed recipcert [, mixed recipkey])
   Decrypts the S/MIME message in the file name infilename and output the results to the file name outfilename.  recipcert is a CERT for one of the recipients. recipkey specifies the private key matching recipcert, if recipcert does not include the key */

PHP_FUNCTION(openssl_pkcs7_decrypt)
{
	zval ** recipcert, ** recipkey = NULL;
	X509 * cert = NULL;
	EVP_PKEY * key = NULL;
	long certresval, keyresval;
	BIO * in = NULL, * out = NULL, * datain = NULL;
	PKCS7 * p7 = NULL;
	char * infilename;	int infilename_len;
	char * outfilename;	int outfilename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ppZ|Z", &infilename, &infilename_len,
				&outfilename, &outfilename_len, &recipcert, &recipkey) == FAILURE) {
		return;
	}

	RETVAL_FALSE;

	cert = php_openssl_x509_from_zval(recipcert, 0, &certresval TSRMLS_CC);
	if (cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to coerce parameter 3 to x509 cert");
		goto clean_exit;
	}

	key = php_openssl_evp_from_zval(recipkey ? recipkey : recipcert, 0, "", 0, &keyresval TSRMLS_CC);
	if (key == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to get private key");
		goto clean_exit;
	}
	
	if (php_openssl_open_base_dir_chk(infilename TSRMLS_CC) || php_openssl_open_base_dir_chk(outfilename TSRMLS_CC)) {
		goto clean_exit;
	}

	in = BIO_new_file(infilename, "r");
	if (in == NULL) {
		goto clean_exit;
	}
	out = BIO_new_file(outfilename, "w");
	if (out == NULL) {
		goto clean_exit;
	}

	p7 = SMIME_read_PKCS7(in, &datain);

	if (p7 == NULL) {
		goto clean_exit;
	}
	if (PKCS7_decrypt(p7, key, cert, out, PKCS7_DETACHED)) { 
		RETVAL_TRUE;
	}
clean_exit:
	PKCS7_free(p7);
	BIO_free(datain);
	BIO_free(in);
	BIO_free(out);
	if (cert && certresval == -1) {
		X509_free(cert);
	}
	if (key && keyresval == -1) {
		EVP_PKEY_free(key);
	}
}
/* }}} */

/* }}} */

/* {{{ proto bool openssl_private_encrypt(string data, string &crypted, mixed key [, int padding])
   Encrypts data with private key */
PHP_FUNCTION(openssl_private_encrypt)
{
	zval **key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	unsigned char *cryptedbuf = NULL;
	int successful = 0;
	long keyresource = -1;
	char * data;
	int data_len;
	long padding = RSA_PKCS1_PADDING;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szZ|l", &data, &data_len, &crypted, &key, &padding) == FAILURE) { 
		return;
	}
	RETVAL_FALSE;

	pkey = php_openssl_evp_from_zval(key, 0, "", 0, &keyresource TSRMLS_CC);

	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "key param is not a valid private key");
		RETURN_FALSE;
	}

	cryptedlen = EVP_PKEY_size(pkey);
	cryptedbuf = emalloc(cryptedlen + 1);

	switch (pkey->type) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			successful =  (RSA_private_encrypt(data_len, 
						(unsigned char *)data, 
						cryptedbuf, 
						pkey->pkey.rsa, 
						padding) == cryptedlen);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key type not supported in this PHP build!");
	}

	if (successful) {
		zval_dtor(crypted);
		cryptedbuf[cryptedlen] = '\0';
		ZVAL_STRINGL(crypted, (char *)cryptedbuf, cryptedlen, 0);
		cryptedbuf = NULL;
		RETVAL_TRUE;
	}
	if (cryptedbuf) {
		efree(cryptedbuf);
	}
	if (keyresource == -1) { 
		EVP_PKEY_free(pkey);
	}
}
/* }}} */

/* {{{ proto bool openssl_private_decrypt(string data, string &decrypted, mixed key [, int padding])
   Decrypts data with private key */
PHP_FUNCTION(openssl_private_decrypt)
{
	zval **key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	unsigned char *cryptedbuf = NULL;
	unsigned char *crypttemp;
	int successful = 0;
	long padding = RSA_PKCS1_PADDING;
	long keyresource = -1;
	char * data;
	int data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szZ|l", &data, &data_len, &crypted, &key, &padding) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	pkey = php_openssl_evp_from_zval(key, 0, "", 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "key parameter is not a valid private key");
		RETURN_FALSE;
	}

	cryptedlen = EVP_PKEY_size(pkey);
	crypttemp = emalloc(cryptedlen + 1);

	switch (pkey->type) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			cryptedlen = RSA_private_decrypt(data_len, 
					(unsigned char *)data, 
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key type not supported in this PHP build!");
	}

	efree(crypttemp);

	if (successful) {
		zval_dtor(crypted);
		cryptedbuf[cryptedlen] = '\0';
		ZVAL_STRINGL(crypted, (char *)cryptedbuf, cryptedlen, 0);
		cryptedbuf = NULL;
		RETVAL_TRUE;
	}

	if (keyresource == -1) {
		EVP_PKEY_free(pkey);
	}
	if (cryptedbuf) { 
		efree(cryptedbuf);
	}
}
/* }}} */

/* {{{ proto bool openssl_public_encrypt(string data, string &crypted, mixed key [, int padding])
   Encrypts data with public key */
PHP_FUNCTION(openssl_public_encrypt)
{
	zval **key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	unsigned char *cryptedbuf;
	int successful = 0;
	long keyresource = -1;
	long padding = RSA_PKCS1_PADDING;
	char * data;
	int data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szZ|l", &data, &data_len, &crypted, &key, &padding) == FAILURE)
		return;

	RETVAL_FALSE;
	
	pkey = php_openssl_evp_from_zval(key, 1, NULL, 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "key parameter is not a valid public key");
		RETURN_FALSE;
	}

	cryptedlen = EVP_PKEY_size(pkey);
	cryptedbuf = emalloc(cryptedlen + 1);

	switch (pkey->type) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			successful = (RSA_public_encrypt(data_len, 
						(unsigned char *)data, 
						cryptedbuf, 
						pkey->pkey.rsa, 
						padding) == cryptedlen);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key type not supported in this PHP build!");

	}

	if (successful) {
		zval_dtor(crypted);
		cryptedbuf[cryptedlen] = '\0';
		ZVAL_STRINGL(crypted, (char *)cryptedbuf, cryptedlen, 0);
		cryptedbuf = NULL;
		RETVAL_TRUE;
	}
	if (keyresource == -1) {
		EVP_PKEY_free(pkey);
	}
	if (cryptedbuf) {
		efree(cryptedbuf);
	}
}
/* }}} */

/* {{{ proto bool openssl_public_decrypt(string data, string &crypted, resource key [, int padding])
   Decrypts data with public key */
PHP_FUNCTION(openssl_public_decrypt)
{
	zval **key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	unsigned char *cryptedbuf = NULL;
	unsigned char *crypttemp;
	int successful = 0;
	long keyresource = -1;
	long padding = RSA_PKCS1_PADDING;
	char * data;
	int data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szZ|l", &data, &data_len, &crypted, &key, &padding) == FAILURE) {
		return;
	}
	RETVAL_FALSE;
	
	pkey = php_openssl_evp_from_zval(key, 1, NULL, 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "key parameter is not a valid public key");
		RETURN_FALSE;
	}

	cryptedlen = EVP_PKEY_size(pkey);
	crypttemp = emalloc(cryptedlen + 1);

	switch (pkey->type) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			cryptedlen = RSA_public_decrypt(data_len, 
					(unsigned char *)data, 
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key type not supported in this PHP build!");
		 
	}

	efree(crypttemp);

	if (successful) {
		zval_dtor(crypted);
		cryptedbuf[cryptedlen] = '\0';
		ZVAL_STRINGL(crypted, (char *)cryptedbuf, cryptedlen, 0);
		cryptedbuf = NULL;
		RETVAL_TRUE;
	}

	if (cryptedbuf) {
		efree(cryptedbuf);
	}
	if (keyresource == -1) {
		EVP_PKEY_free(pkey);
	}
}
/* }}} */

/* {{{ proto mixed openssl_error_string(void)
   Returns a description of the last error, and alters the index of the error messages. Returns false when the are no more messages */
PHP_FUNCTION(openssl_error_string)
{
	char buf[512];
	unsigned long val;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	val = ERR_get_error();
	if (val) {
		RETURN_STRING(ERR_error_string(val, buf), 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool openssl_sign(string data, &string signature, mixed key[, mixed method])
   Signs data */
PHP_FUNCTION(openssl_sign)
{
	zval **key, *signature;
	EVP_PKEY *pkey;
	int siglen;
	unsigned char *sigbuf;
	long keyresource = -1;
	char * data;
	int data_len;
	EVP_MD_CTX md_ctx;
	zval *method = NULL;
	long signature_algo = OPENSSL_ALGO_SHA1;
	const EVP_MD *mdtype;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szZ|z", &data, &data_len, &signature, &key, &method) == FAILURE) {
		return;
	}
	pkey = php_openssl_evp_from_zval(key, 0, "", 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "supplied key param cannot be coerced into a private key");
		RETURN_FALSE;
	}

	if (method == NULL || Z_TYPE_P(method) == IS_LONG) {
		if (method != NULL) {
			signature_algo = Z_LVAL_P(method);
		}
		mdtype = php_openssl_get_evp_md_from_algo(signature_algo);
	} else if (Z_TYPE_P(method) == IS_STRING) {
		mdtype = EVP_get_digestbyname(Z_STRVAL_P(method));
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm.");
		RETURN_FALSE;
	}
	if (!mdtype) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm.");
		RETURN_FALSE;
	}

	siglen = EVP_PKEY_size(pkey);
	sigbuf = emalloc(siglen + 1);

	EVP_SignInit(&md_ctx, mdtype);
	EVP_SignUpdate(&md_ctx, data, data_len);
	if (EVP_SignFinal (&md_ctx, sigbuf,(unsigned int *)&siglen, pkey)) {
		zval_dtor(signature);
		sigbuf[siglen] = '\0';
		ZVAL_STRINGL(signature, (char *)sigbuf, siglen, 0);
		RETVAL_TRUE;
	} else {
		efree(sigbuf);
		RETVAL_FALSE;
	}
	EVP_MD_CTX_cleanup(&md_ctx);
	if (keyresource == -1) {
		EVP_PKEY_free(pkey);
	}
}
/* }}} */

/* {{{ proto int openssl_verify(string data, string signature, mixed key[, mixed method])
   Verifys data */
PHP_FUNCTION(openssl_verify)
{
	zval **key;
	EVP_PKEY *pkey;
	int err;
	EVP_MD_CTX     md_ctx;
	const EVP_MD *mdtype;
	long keyresource = -1;
	char * data;	int data_len;
	char * signature;	int signature_len;
	zval *method = NULL;
	long signature_algo = OPENSSL_ALGO_SHA1;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssZ|z", &data, &data_len, &signature, &signature_len, &key, &method) == FAILURE) {
		return;
	}

	if (method == NULL || Z_TYPE_P(method) == IS_LONG) {
		if (method != NULL) {
			signature_algo = Z_LVAL_P(method);
		}
		mdtype = php_openssl_get_evp_md_from_algo(signature_algo);
	} else if (Z_TYPE_P(method) == IS_STRING) {
		mdtype = EVP_get_digestbyname(Z_STRVAL_P(method));
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm.");
		RETURN_FALSE;
	}
	if (!mdtype) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm.");
		RETURN_FALSE;
	}

	pkey = php_openssl_evp_from_zval(key, 1, NULL, 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "supplied key param cannot be coerced into a public key");
		RETURN_FALSE;
	}

	EVP_VerifyInit   (&md_ctx, mdtype);
	EVP_VerifyUpdate (&md_ctx, data, data_len);
	err = EVP_VerifyFinal (&md_ctx, (unsigned char *)signature, signature_len, pkey);
	EVP_MD_CTX_cleanup(&md_ctx);

	if (keyresource == -1) {
		EVP_PKEY_free(pkey);
	}
	RETURN_LONG(err);
}
/* }}} */

/* {{{ proto int openssl_seal(string data, &string sealdata, &array ekeys, array pubkeys)
   Seals data */
PHP_FUNCTION(openssl_seal)
{
	zval *pubkeys, **pubkey, *sealdata, *ekeys;
	HashTable *pubkeysht;
	HashPosition pos;
	EVP_PKEY **pkeys;
	long * key_resources;	/* so we know what to cleanup */
	int i, len1, len2, *eksl, nkeys;
	unsigned char *buf = NULL, **eks;
	char * data; int data_len;
	char *method =NULL;
	int method_len = 0;
	const EVP_CIPHER *cipher;
	EVP_CIPHER_CTX ctx;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szza/|s", &data, &data_len, &sealdata, &ekeys, &pubkeys, &method, &method_len) == FAILURE) {
		return;
	}
	
	pubkeysht = HASH_OF(pubkeys);
	nkeys = pubkeysht ? zend_hash_num_elements(pubkeysht) : 0;
	if (!nkeys) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Fourth argument to openssl_seal() must be a non-empty array");
		RETURN_FALSE;
	}

	if (method) {
		cipher = EVP_get_cipherbyname(method);
		if (!cipher) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm.");
			RETURN_FALSE;
		}
		if (EVP_CIPHER_iv_length(cipher) > 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Ciphers with modes requiring IV are not supported");
			RETURN_FALSE;
		}
	} else {
		cipher = EVP_rc4();
	}

	pkeys = safe_emalloc(nkeys, sizeof(*pkeys), 0);
	eksl = safe_emalloc(nkeys, sizeof(*eksl), 0);
	eks = safe_emalloc(nkeys, sizeof(*eks), 0);
	memset(eks, 0, sizeof(*eks) * nkeys);
	key_resources = safe_emalloc(nkeys, sizeof(long), 0);
	memset(key_resources, 0, sizeof(*key_resources) * nkeys);

	/* get the public keys we are using to seal this data */
	zend_hash_internal_pointer_reset_ex(pubkeysht, &pos);
	i = 0;
	while (zend_hash_get_current_data_ex(pubkeysht, (void **) &pubkey,
				&pos) == SUCCESS) {
		pkeys[i] = php_openssl_evp_from_zval(pubkey, 1, NULL, 0, &key_resources[i] TSRMLS_CC);
		if (pkeys[i] == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "not a public key (%dth member of pubkeys)", i+1);
			RETVAL_FALSE;
			goto clean_exit;
		}
		eks[i] = emalloc(EVP_PKEY_size(pkeys[i]) + 1);
		zend_hash_move_forward_ex(pubkeysht, &pos);
		i++;
	}

	if (!EVP_EncryptInit(&ctx,cipher,NULL,NULL)) {
		RETVAL_FALSE;
		EVP_CIPHER_CTX_cleanup(&ctx);
		goto clean_exit;
	}

#if 0
	/* Need this if allow ciphers that require initialization vector */
	ivlen = EVP_CIPHER_CTX_iv_length(&ctx);
	iv = ivlen ? emalloc(ivlen + 1) : NULL;
#endif
	/* allocate one byte extra to make room for \0 */
	buf = emalloc(data_len + EVP_CIPHER_CTX_block_size(&ctx));
	EVP_CIPHER_CTX_cleanup(&ctx);

	if (EVP_SealInit(&ctx, cipher, eks, eksl, NULL, pkeys, nkeys) <= 0 ||
			!EVP_SealUpdate(&ctx, buf, &len1, (unsigned char *)data, data_len) ||
			!EVP_SealFinal(&ctx, buf + len1, &len2)) {
		RETVAL_FALSE;
		efree(buf);
		EVP_CIPHER_CTX_cleanup(&ctx);
		goto clean_exit;
	}

	if (len1 + len2 > 0) {
		zval_dtor(sealdata);
		buf[len1 + len2] = '\0';
		buf = erealloc(buf, len1 + len2 + 1);
		ZVAL_STRINGL(sealdata, (char *)buf, len1 + len2, 0);

		zval_dtor(ekeys);
		array_init(ekeys);
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
	} else {
		efree(buf);
	}
	RETVAL_LONG(len1 + len2);
	EVP_CIPHER_CTX_cleanup(&ctx);

clean_exit:
	for (i=0; i<nkeys; i++) {
		if (key_resources[i] == -1) {
			EVP_PKEY_free(pkeys[i]);
		}
		if (eks[i]) { 
			efree(eks[i]);
		}
	}
	efree(eks);
	efree(eksl);
	efree(pkeys);
	efree(key_resources);
}
/* }}} */

/* {{{ proto bool openssl_open(string data, &string opendata, string ekey, mixed privkey)
   Opens data */
PHP_FUNCTION(openssl_open)
{
	zval **privkey, *opendata;
	EVP_PKEY *pkey;
	int len1, len2;
	unsigned char *buf;
	long keyresource = -1;
	EVP_CIPHER_CTX ctx;
	char * data;	int data_len;
	char * ekey;	int ekey_len;
	char *method =NULL;
	int method_len = 0;
	const EVP_CIPHER *cipher;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szsZ|s", &data, &data_len, &opendata, &ekey, &ekey_len, &privkey, &method, &method_len) == FAILURE) {
		return;
	}

	pkey = php_openssl_evp_from_zval(privkey, 0, "", 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to coerce parameter 4 into a private key");
		RETURN_FALSE;
	}

	if (method) {
		cipher = EVP_get_cipherbyname(method);
		if (!cipher) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm.");
			RETURN_FALSE;
		}
	} else {
		cipher = EVP_rc4();
	}
	
	buf = emalloc(data_len + 1);

	if (EVP_OpenInit(&ctx, cipher, (unsigned char *)ekey, ekey_len, NULL, pkey) && EVP_OpenUpdate(&ctx, buf, &len1, (unsigned char *)data, data_len)) {
		if (!EVP_OpenFinal(&ctx, buf + len1, &len2) || (len1 + len2 == 0)) {
			efree(buf);
			RETVAL_FALSE;
		} else {
			zval_dtor(opendata);
			buf[len1 + len2] = '\0';
			ZVAL_STRINGL(opendata, erealloc(buf, len1 + len2 + 1), len1 + len2, 0);
			RETVAL_TRUE;
		}
	} else {
		efree(buf);
		RETVAL_FALSE;
	}
	if (keyresource == -1) {
		EVP_PKEY_free(pkey);
	}
	EVP_CIPHER_CTX_cleanup(&ctx);
}
/* }}} */



static void openssl_add_method_or_alias(const OBJ_NAME *name, void *arg) /* {{{ */
{
	add_next_index_string((zval*)arg, (char*)name->name, 1);
}
/* }}} */

static void openssl_add_method(const OBJ_NAME *name, void *arg) /* {{{ */
{
	if (name->alias == 0) {
		add_next_index_string((zval*)arg, (char*)name->name, 1);
	}
}
/* }}} */

/* {{{ proto array openssl_get_md_methods([bool aliases = false])
   Return array of available digest methods */
PHP_FUNCTION(openssl_get_md_methods)
{
	zend_bool aliases = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &aliases) == FAILURE) {
		return;
	}
	array_init(return_value);
	OBJ_NAME_do_all_sorted(OBJ_NAME_TYPE_MD_METH,
		aliases ? openssl_add_method_or_alias: openssl_add_method, 
		return_value);
}
/* }}} */

/* {{{ proto array openssl_get_cipher_methods([bool aliases = false])
   Return array of available cipher methods */
PHP_FUNCTION(openssl_get_cipher_methods)
{
	zend_bool aliases = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &aliases) == FAILURE) {
		return;
	}
	array_init(return_value);
	OBJ_NAME_do_all_sorted(OBJ_NAME_TYPE_CIPHER_METH,
		aliases ? openssl_add_method_or_alias: openssl_add_method, 
		return_value);
}
/* }}} */

/* {{{ proto string openssl_digest(string data, string method [, bool raw_output=false])
   Computes digest hash value for given data using given method, returns raw or binhex encoded string */
PHP_FUNCTION(openssl_digest)
{
	zend_bool raw_output = 0;
	char *data, *method;
	int data_len, method_len;
	const EVP_MD *mdtype;
	EVP_MD_CTX md_ctx;
	int siglen;
	unsigned char *sigbuf;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|b", &data, &data_len, &method, &method_len, &raw_output) == FAILURE) {
		return;
	}
	mdtype = EVP_get_digestbyname(method);
	if (!mdtype) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signature algorithm");
		RETURN_FALSE;
	}

	siglen = EVP_MD_size(mdtype);
	sigbuf = emalloc(siglen + 1);

	EVP_DigestInit(&md_ctx, mdtype);
	EVP_DigestUpdate(&md_ctx, (unsigned char *)data, data_len);
	if (EVP_DigestFinal (&md_ctx, (unsigned char *)sigbuf, (unsigned int *)&siglen)) {
		if (raw_output) {
			sigbuf[siglen] = '\0';
			RETVAL_STRINGL((char *)sigbuf, siglen, 0);
		} else {
			int digest_str_len = siglen * 2;
			char *digest_str = emalloc(digest_str_len + 1);

			make_digest_ex(digest_str, sigbuf, siglen);
			efree(sigbuf);
			RETVAL_STRINGL(digest_str, digest_str_len, 0);
		}
	} else {
		efree(sigbuf);
		RETVAL_FALSE;
	}
}
/* }}} */

static zend_bool php_openssl_validate_iv(char **piv, int *piv_len, int iv_required_len TSRMLS_DC)
{
	char *iv_new;

	/* Best case scenario, user behaved */
	if (*piv_len == iv_required_len) {
		return 0;
	}

	iv_new = ecalloc(1, iv_required_len + 1);

	if (*piv_len <= 0) {
		/* BC behavior */
		*piv_len = iv_required_len;
		*piv     = iv_new;
		return 1;
	}

	if (*piv_len < iv_required_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "IV passed is only %d bytes long, cipher expects an IV of precisely %d bytes, padding with \\0", *piv_len, iv_required_len);
		memcpy(iv_new, *piv, *piv_len);
		*piv_len = iv_required_len;
		*piv     = iv_new;
		return 1;
	}

	php_error_docref(NULL TSRMLS_CC, E_WARNING, "IV passed is %d bytes long which is longer than the %d expected by selected cipher, truncating", *piv_len, iv_required_len);
	memcpy(iv_new, *piv, iv_required_len);
	*piv_len = iv_required_len;
	*piv     = iv_new;
	return 1;

}

/* {{{ proto string openssl_encrypt(string data, string method, string password [, long options=0 [, string $iv='']])
   Encrypts given data with given method and key, returns raw or base64 encoded string */
PHP_FUNCTION(openssl_encrypt)
{
	long options = 0;
	char *data, *method, *password, *iv = "";
	int data_len, method_len, password_len, iv_len = 0, max_iv_len;
	const EVP_CIPHER *cipher_type;
	EVP_CIPHER_CTX cipher_ctx;
	int i=0, outlen, keylen;
	unsigned char *outbuf, *key;
	zend_bool free_iv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|ls", &data, &data_len, &method, &method_len, &password, &password_len, &options, &iv, &iv_len) == FAILURE) {
		return;
	}
	cipher_type = EVP_get_cipherbyname(method);
	if (!cipher_type) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown cipher algorithm");
		RETURN_FALSE;
	}

	keylen = EVP_CIPHER_key_length(cipher_type);
	if (keylen > password_len) {
		key = emalloc(keylen);
		memset(key, 0, keylen);
		memcpy(key, password, password_len);
	} else {
		key = (unsigned char*)password;
	}

	max_iv_len = EVP_CIPHER_iv_length(cipher_type);
	if (iv_len <= 0 && max_iv_len > 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Using an empty Initialization Vector (iv) is potentially insecure and not recommended");
	}
	free_iv = php_openssl_validate_iv(&iv, &iv_len, max_iv_len TSRMLS_CC);

	outlen = data_len + EVP_CIPHER_block_size(cipher_type);
	outbuf = safe_emalloc(outlen, 1, 1);

	EVP_EncryptInit(&cipher_ctx, cipher_type, NULL, NULL);
	if (password_len > keylen) {
		EVP_CIPHER_CTX_set_key_length(&cipher_ctx, password_len);
	}
	EVP_EncryptInit_ex(&cipher_ctx, NULL, NULL, key, (unsigned char *)iv);
	if (options & OPENSSL_ZERO_PADDING) {
		EVP_CIPHER_CTX_set_padding(&cipher_ctx, 0);
	}
	if (data_len > 0) {
		EVP_EncryptUpdate(&cipher_ctx, outbuf, &i, (unsigned char *)data, data_len);
	}
	outlen = i;
	if (EVP_EncryptFinal(&cipher_ctx, (unsigned char *)outbuf + i, &i)) {
		outlen += i;
		if (options & OPENSSL_RAW_DATA) {
			outbuf[outlen] = '\0';
			RETVAL_STRINGL_CHECK((char *)outbuf, outlen, 0);
		} else {
			int base64_str_len;
			char *base64_str;

			base64_str = (char*)php_base64_encode(outbuf, outlen, &base64_str_len);
			efree(outbuf);
			if (!base64_str) {
				RETVAL_FALSE;
			} else {
				RETVAL_STRINGL(base64_str, base64_str_len, 0);
			}
		}
	} else {
		efree(outbuf);
		RETVAL_FALSE;
	}
	if (key != (unsigned char*)password) {
		efree(key);
	}
	if (free_iv) {
		efree(iv);
	}
	EVP_CIPHER_CTX_cleanup(&cipher_ctx);
}
/* }}} */

/* {{{ proto string openssl_decrypt(string data, string method, string password [, long options=0 [, string $iv = '']])
   Takes raw or base64 encoded string and dectupt it using given method and key */
PHP_FUNCTION(openssl_decrypt)
{
	long options = 0;
	char *data, *method, *password, *iv = "";
	int data_len, method_len, password_len, iv_len = 0;
	const EVP_CIPHER *cipher_type;
	EVP_CIPHER_CTX cipher_ctx;
	int i, outlen, keylen;
	unsigned char *outbuf, *key;
	int base64_str_len;
	char *base64_str = NULL;
	zend_bool free_iv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|ls", &data, &data_len, &method, &method_len, &password, &password_len, &options, &iv, &iv_len) == FAILURE) {
		return;
	}

	if (!method_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown cipher algorithm");
		RETURN_FALSE;
	}

	cipher_type = EVP_get_cipherbyname(method);
	if (!cipher_type) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown cipher algorithm");
		RETURN_FALSE;
	}

	if (!(options & OPENSSL_RAW_DATA)) {
		base64_str = (char*)php_base64_decode((unsigned char*)data, data_len, &base64_str_len);
		if (!base64_str) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to base64 decode the input");
			RETURN_FALSE;
		}
		data_len = base64_str_len;
		data = base64_str;
	}

	keylen = EVP_CIPHER_key_length(cipher_type);
	if (keylen > password_len) {
		key = emalloc(keylen);
		memset(key, 0, keylen);
		memcpy(key, password, password_len);
	} else {
		key = (unsigned char*)password;
	}

	free_iv = php_openssl_validate_iv(&iv, &iv_len, EVP_CIPHER_iv_length(cipher_type) TSRMLS_CC);

	outlen = data_len + EVP_CIPHER_block_size(cipher_type);
	outbuf = emalloc(outlen + 1);

	EVP_DecryptInit(&cipher_ctx, cipher_type, NULL, NULL);
	if (password_len > keylen) {
		EVP_CIPHER_CTX_set_key_length(&cipher_ctx, password_len);
	}
	EVP_DecryptInit_ex(&cipher_ctx, NULL, NULL, key, (unsigned char *)iv);
	if (options & OPENSSL_ZERO_PADDING) {
		EVP_CIPHER_CTX_set_padding(&cipher_ctx, 0);
	}
	EVP_DecryptUpdate(&cipher_ctx, outbuf, &i, (unsigned char *)data, data_len);
	outlen = i;
	if (EVP_DecryptFinal(&cipher_ctx, (unsigned char *)outbuf + i, &i)) {
		outlen += i;
		outbuf[outlen] = '\0';
		RETVAL_STRINGL((char *)outbuf, outlen, 0);
	} else {
		efree(outbuf);
		RETVAL_FALSE;
	}
	if (key != (unsigned char*)password) {
		efree(key);
	}
	if (free_iv) {
		efree(iv);
	}
	if (base64_str) {
		efree(base64_str);
	}
 	EVP_CIPHER_CTX_cleanup(&cipher_ctx);
}
/* }}} */

/* {{{ proto int openssl_cipher_iv_length(string $method) */
PHP_FUNCTION(openssl_cipher_iv_length)
{
	char *method;
	int method_len;
	const EVP_CIPHER *cipher_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &method, &method_len) == FAILURE) {
		return;
	}

	if (!method_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown cipher algorithm");
		RETURN_FALSE;
	}

	cipher_type = EVP_get_cipherbyname(method);
	if (!cipher_type) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown cipher algorithm");
		RETURN_FALSE;
	}

	RETURN_LONG(EVP_CIPHER_iv_length(cipher_type));
}
/* }}} */


/* {{{ proto string openssl_dh_compute_key(string pub_key, resource dh_key)
   Computes shared secret for public value of remote DH key and local DH key */
PHP_FUNCTION(openssl_dh_compute_key)
{
	zval *key;
	char *pub_str;
	int pub_len;
	EVP_PKEY *pkey;
	BIGNUM *pub;
	char *data;
	int len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sr", &pub_str, &pub_len, &key) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(pkey, EVP_PKEY *, &key, -1, "OpenSSL key", le_key);
	if (!pkey || EVP_PKEY_type(pkey->type) != EVP_PKEY_DH || !pkey->pkey.dh) {
		RETURN_FALSE;
	}

	pub = BN_bin2bn((unsigned char*)pub_str, pub_len, NULL);

	data = emalloc(DH_size(pkey->pkey.dh) + 1);
	len = DH_compute_key((unsigned char*)data, pub, pkey->pkey.dh);

	if (len >= 0) {
		data[len] = 0;
		RETVAL_STRINGL(data, len, 0);
	} else {
		efree(data);
		RETVAL_FALSE;
	}

	BN_free(pub);
}
/* }}} */

/* {{{ proto string openssl_random_pseudo_bytes(integer length [, &bool returned_strong_result])
   Returns a string of the length specified filled with random pseudo bytes */
PHP_FUNCTION(openssl_random_pseudo_bytes)
{
	long buffer_length;
	unsigned char *buffer = NULL;
	zval *zstrong_result_returned = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|z", &buffer_length, &zstrong_result_returned) == FAILURE) {
		return;
	}

	if (zstrong_result_returned) {
		zval_dtor(zstrong_result_returned);
		ZVAL_BOOL(zstrong_result_returned, 0);
	}

	if (buffer_length <= 0 || buffer_length > INT_MAX) {
		RETURN_FALSE;
	}

	buffer = safe_emalloc(buffer_length, 1, 1);

#ifdef PHP_WIN32
	/* random/urandom equivalent on Windows */
	if (php_win32_get_random_bytes(buffer, (size_t) buffer_length) == FAILURE){
		efree(buffer);
		if (zstrong_result_returned) {
			ZVAL_BOOL(zstrong_result_returned, 0);
		}
		RETURN_FALSE;
	}
#else
	PHP_OPENSSL_RAND_ADD_TIME();
	if (RAND_bytes(buffer, buffer_length) <= 0) {
		efree(buffer);
		if (zstrong_result_returned) {
			ZVAL_BOOL(zstrong_result_returned, 0);
		}
		RETURN_FALSE;
	}
#endif

	buffer[buffer_length] = 0;
	RETVAL_STRINGL((char *)buffer, buffer_length, 0);

	if (zstrong_result_returned) {
		ZVAL_BOOL(zstrong_result_returned, 1);
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 8
 * c-basic-offset: 8
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

