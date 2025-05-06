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
   | Authors: Jakub Zelenka <bukka@php.net>                               |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_OPENSSL_BACKEND_H
#define PHP_OPENSSL_BACKEND_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "php_openssl.h"

#include <openssl/bn.h>
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
#include <openssl/cms.h>

/* number conversion flags checks */
#define PHP_OPENSSL_CHECK_NUMBER_CONVERSION(_cond, _name, _arg_num) \
	do { \
		if (_cond) { \
			zend_argument_value_error((_arg_num), #_name" is too long"); \
			RETURN_THROWS(); \
		} \
	} while(0)
#define PHP_OPENSSL_CHECK_NUMBER_CONVERSION_NULL_RETURN(_cond, _name) \
	do { \
		if (_cond) { \
			zend_value_error(#_name" is too long"); \
			return NULL; \
		} \
	} while(0)
/* check if size_t can be safely casted to int */
#define PHP_OPENSSL_CHECK_SIZE_T_TO_INT(_var, _name, _arg_num) \
	PHP_OPENSSL_CHECK_NUMBER_CONVERSION(ZEND_SIZE_T_INT_OVFL(_var), _name, _arg_num)
#define PHP_OPENSSL_CHECK_SIZE_T_TO_INT_NULL_RETURN(_var, _name) \
	PHP_OPENSSL_CHECK_NUMBER_CONVERSION_NULL_RETURN(ZEND_SIZE_T_INT_OVFL(_var), _name)
/* check if size_t can be safely casted to unsigned int */
#define PHP_OPENSSL_CHECK_SIZE_T_TO_UINT(_var, _name, _arg_num) \
	PHP_OPENSSL_CHECK_NUMBER_CONVERSION(ZEND_SIZE_T_UINT_OVFL(_var), _name, _arg_num)
/* check if long can be safely casted to int */
#define PHP_OPENSSL_CHECK_LONG_TO_INT(_var, _name, _arg_num) \
	PHP_OPENSSL_CHECK_NUMBER_CONVERSION(ZEND_LONG_EXCEEDS_INT(_var), _name, _arg_num)
#define PHP_OPENSSL_CHECK_LONG_TO_INT_NULL_RETURN(_var, _name) \
	PHP_OPENSSL_CHECK_NUMBER_CONVERSION_NULL_RETURN(ZEND_LONG_EXCEEDS_INT(_var), _name)

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
	OPENSSL_KEYTYPE_EC = OPENSSL_KEYTYPE_DH +1,
	OPENSSL_KEYTYPE_X25519 = OPENSSL_KEYTYPE_DH +2,
	OPENSSL_KEYTYPE_ED25519 = OPENSSL_KEYTYPE_DH +3,
	OPENSSL_KEYTYPE_X448 = OPENSSL_KEYTYPE_DH +4,
	OPENSSL_KEYTYPE_ED448 = OPENSSL_KEYTYPE_DH +5,
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

	PHP_OPENSSL_CIPHER_DEFAULT = PHP_OPENSSL_CIPHER_AES_128_CBC
};

/* Add some encoding rules.  This is normally handled through filters
 * in the OpenSSL code, but we will do that part as if we were one
 * of the OpenSSL binaries along the lines of -outform {DER|CMS|PEM}
 */
enum php_openssl_encoding {
	ENCODING_DER,
	ENCODING_SMIME,
	ENCODING_PEM,
};


#define MIN_KEY_LENGTH		384

/* constants used in ext/phar/util.c, keep in sync */
#define OPENSSL_ALGO_SHA1 	1
#define OPENSSL_ALGO_MD5	2
#ifndef OPENSSL_NO_MD4
#define OPENSSL_ALGO_MD4	3
#endif
#ifndef OPENSSL_NO_MD2
#define OPENSSL_ALGO_MD2	4
#endif
/* Number 5 was used for OPENSSL_ALGO_DSS1 which is no longer available */
#define OPENSSL_ALGO_SHA224 6
#define OPENSSL_ALGO_SHA256 7
#define OPENSSL_ALGO_SHA384 8
#define OPENSSL_ALGO_SHA512 9
#ifndef OPENSSL_NO_RMD160
#define OPENSSL_ALGO_RMD160 10
#endif
#define DEBUG_SMIME	0

#if !defined(OPENSSL_NO_EC) && defined(EVP_PKEY_EC)
#define HAVE_EVP_PKEY_EC 1

/* the OPENSSL_EC_EXPLICIT_CURVE value was added
 * in OpenSSL 1.1.0; previous versions should
 * use 0 instead.
 */
#ifndef OPENSSL_EC_EXPLICIT_CURVE
#define OPENSSL_EC_EXPLICIT_CURVE 0x000
#endif
#endif

struct php_x509_request {
	CONF *global_config;	/* Global SSL config */
	CONF *req_config;		/* SSL config for this request */
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

#ifdef HAVE_EVP_PKEY_EC
	int curve_name;
#endif

	EVP_PKEY * priv_key;

	const EVP_CIPHER * priv_key_encrypt_cipher;
};

void php_openssl_add_assoc_name_entry(zval * val, char * key, X509_NAME * name, int shortname);
void php_openssl_add_assoc_asn1_string(zval * val, char * key, ASN1_STRING * str);
time_t php_openssl_asn1_time_to_time_t(ASN1_UTCTIME * timestr);
int php_openssl_config_check_syntax(const char * section_label, const char * config_filename, const char * section, CONF *config);
char *php_openssl_conf_get_string(CONF *conf, const char *group, const char *name);
long php_openssl_conf_get_number(CONF *conf, const char *group, const char *name);
int php_openssl_add_oid_section(struct php_x509_request * req);
int php_openssl_spki_cleanup(const char *src, char *dest);

X509 *php_openssl_x509_from_param(
		zend_object *cert_obj, zend_string *cert_str, uint32_t arg_num);
X509 *php_openssl_x509_from_zval(
		zval *val, bool *free_cert, uint32_t arg_num, bool is_from_array, const char *option_name);
X509_REQ *php_openssl_csr_from_param(
		zend_object *csr_obj, zend_string *csr_str, uint32_t arg_num);
EVP_PKEY *php_openssl_pkey_from_zval(
		zval *val, int public_key, char *passphrase, size_t passphrase_len, uint32_t arg_num);

X509_STORE * php_openssl_setup_verify(zval * calist, uint32_t arg_num);
STACK_OF(X509) * php_openssl_load_all_certs_from_file(
		char *cert_file, size_t cert_file_len, uint32_t arg_num);
EVP_PKEY * php_openssl_generate_private_key(struct php_x509_request * req);
zend_string *php_openssl_pkey_derive(EVP_PKEY *key, EVP_PKEY *peer_key, size_t key_size);

#define PHP_SSL_REQ_INIT(req)		memset(req, 0, sizeof(*req))
#define PHP_SSL_REQ_DISPOSE(req)	php_openssl_dispose_config(req)
#define PHP_SSL_REQ_PARSE(req, zval)	php_openssl_parse_config(req, zval)

#define PHP_SSL_CONFIG_SYNTAX_CHECK(var) if (req->var && php_openssl_config_check_syntax(#var, \
			req->config_filename, req->var, req->req_config) == FAILURE) return FAILURE

#define SET_OPTIONAL_STRING_ARG(key, varname, defval)	\
	do { \
		if (optional_args && (item = zend_hash_str_find(Z_ARRVAL_P(optional_args), key, sizeof(key)-1)) != NULL && Z_TYPE_P(item) == IS_STRING) { \
			varname = Z_STRVAL_P(item); \
		} else { \
			varname = defval; \
			if (varname == NULL) { \
				php_openssl_store_errors(); \
			} \
		} \
	} while(0)

#define SET_OPTIONAL_LONG_ARG(key, varname, defval)	\
	if (optional_args && (item = zend_hash_str_find(Z_ARRVAL_P(optional_args), key, sizeof(key)-1)) != NULL && Z_TYPE_P(item) == IS_LONG) \
		varname = (int)Z_LVAL_P(item); \
	else \
		varname = defval

const EVP_CIPHER * php_openssl_get_evp_cipher_from_algo(zend_long algo);

int php_openssl_parse_config(struct php_x509_request * req, zval * optional_args);
void php_openssl_dispose_config(struct php_x509_request * req);

zend_result php_openssl_load_rand_file(const char * file, int *egdsocket, int *seeded);
zend_result php_openssl_write_rand_file(const char * file, int egdsocket, int seeded);

EVP_MD * php_openssl_get_evp_md_from_algo(zend_long algo);
const EVP_CIPHER * php_openssl_get_evp_cipher_from_algo(zend_long algo);

void php_openssl_backend_init(void);
void php_openssl_backend_shutdown(void);

const char *php_openssl_get_conf_filename(void);

void php_openssl_set_cert_locations(zval *return_value);

X509 *php_openssl_x509_from_str(
		zend_string *cert_str, uint32_t arg_num, bool is_from_array, const char *option_name);

X509 *php_openssl_x509_from_param(
		zend_object *cert_obj, zend_string *cert_str, uint32_t arg_num);

X509 *php_openssl_x509_from_zval(
		zval *val, bool *free_cert, uint32_t arg_num, bool is_from_array, const char *option_name);

zend_string* php_openssl_x509_fingerprint(X509 *peer, const char *method, bool raw);

int openssl_x509v3_subjectAltName(BIO *bio, X509_EXTENSION *extension);

STACK_OF(X509) *php_openssl_load_all_certs_from_file(
		char *cert_file, size_t cert_file_len, uint32_t arg_num);

int php_openssl_check_cert(X509_STORE *ctx, X509 *x, STACK_OF(X509) *untrustedchain, int purpose);

X509_STORE *php_openssl_setup_verify(zval *calist, uint32_t arg_num);

void php_openssl_sk_X509_free(STACK_OF(X509) * sk);
STACK_OF(X509) *php_openssl_array_to_X509_sk(zval * zcerts, uint32_t arg_num, const char *option_name);

zend_result php_openssl_csr_add_subj_entry(zval *item, X509_NAME *subj, int nid);
zend_result php_openssl_csr_make(struct php_x509_request * req, X509_REQ * csr, zval * dn, zval * attribs);
X509_REQ *php_openssl_csr_from_str(zend_string *csr_str, uint32_t arg_num);
X509_REQ *php_openssl_csr_from_param(
		zend_object *csr_obj, zend_string *csr_str, uint32_t arg_num);

#if !defined (LIBRESSL_VERSION_NUMBER)
#define PHP_OPENSSL_ASN1_INTEGER_set ASN1_INTEGER_set_int64
#else
#define PHP_OPENSSL_ASN1_INTEGER_set ASN1_INTEGER_set
#endif

EVP_PKEY *php_openssl_extract_public_key(EVP_PKEY *priv_key);

struct php_openssl_pem_password {
	char *key;
	int len;
};

int php_openssl_pem_password_cb(char *buf, int size, int rwflag, void *userdata);
EVP_PKEY *php_openssl_pkey_from_zval(
		zval *val, int public_key, char *passphrase, size_t passphrase_len, uint32_t arg_num);
int php_openssl_get_evp_pkey_type(int key_type);
EVP_PKEY *php_openssl_generate_private_key(struct php_x509_request * req);
void php_openssl_add_bn_to_array(zval *ary, const BIGNUM *bn, const char *name);

void php_openssl_add_bn_to_array(zval *ary, const BIGNUM *bn, const char *name);

#define OPENSSL_PKEY_GET_BN(_type, _name) php_openssl_add_bn_to_array(&_type, _name, #_name)

#define OPENSSL_PKEY_SET_BN(_data, _name) do { \
		zval *bn; \
		if ((bn = zend_hash_str_find(Z_ARRVAL_P(_data), #_name, sizeof(#_name)-1)) != NULL && \
				Z_TYPE_P(bn) == IS_STRING) { \
			_name = BN_bin2bn( \
				(unsigned char*)Z_STRVAL_P(bn), \
				(int)Z_STRLEN_P(bn), NULL); \
		} else { \
			_name = NULL; \
		} \
	} while (0);


EVP_PKEY *php_openssl_pkey_init_rsa(zval *data);
EVP_PKEY *php_openssl_pkey_init_dsa(zval *data, bool *is_private);
BIGNUM *php_openssl_dh_pub_from_priv(BIGNUM *priv_key, BIGNUM *g, BIGNUM *p);
EVP_PKEY *php_openssl_pkey_init_dh(zval *data, bool *is_private);
EVP_PKEY *php_openssl_pkey_init_ec(zval *data, bool *is_private);
void php_openssl_pkey_object_curve_25519_448(zval *return_value, int key_type, zval *data);
#if PHP_OPENSSL_API_VERSION >= 0x30000
void php_openssl_pkey_object_curve_25519_448(zval *return_value, int key_type, zval *data);
#endif
zend_long php_openssl_pkey_get_details(zval *return_value, EVP_PKEY *pkey);

zend_string *php_openssl_dh_compute_key(EVP_PKEY *pkey, char *pub_str, size_t pub_len);

BIO *php_openssl_bio_new_file(
		const char *filename, size_t filename_len, uint32_t arg_num, const char *mode);

void php_openssl_add_method_or_alias(const OBJ_NAME *name, void *arg);
void php_openssl_add_method(const OBJ_NAME *name, void *arg);

void php_openssl_get_md_methods(zval *return_value, bool aliases);
void php_openssl_get_cipher_methods(zval *return_value, bool aliases);

/* Cipher mode info */
struct php_openssl_cipher_mode {
	bool is_aead;
	bool is_single_run_aead;
	bool set_tag_length_always;
	bool set_tag_length_when_encrypting;
	int aead_get_tag_flag;
	int aead_set_tag_flag;
	int aead_ivlen_flag;
};

static inline void php_openssl_set_aead_flags(struct php_openssl_cipher_mode *mode) {
	mode->is_aead = true;
	mode->aead_get_tag_flag = EVP_CTRL_AEAD_GET_TAG;
	mode->aead_set_tag_flag = EVP_CTRL_AEAD_SET_TAG;
	mode->aead_ivlen_flag = EVP_CTRL_AEAD_SET_IVLEN;
}

void php_openssl_load_cipher_mode(struct php_openssl_cipher_mode *mode, const EVP_CIPHER *cipher_type);
zend_result php_openssl_validate_iv(const char **piv, size_t *piv_len, size_t iv_required_len,
		bool *free_iv, EVP_CIPHER_CTX *cipher_ctx, struct php_openssl_cipher_mode *mode);

zend_result php_openssl_cipher_init(const EVP_CIPHER *cipher_type,
		EVP_CIPHER_CTX *cipher_ctx, struct php_openssl_cipher_mode *mode,
		const char **ppassword, size_t *ppassword_len, bool *free_password,
		const char **piv, size_t *piv_len, bool *free_iv,
		const char *tag, int tag_len, zend_long options, int enc);

zend_result php_openssl_cipher_update(const EVP_CIPHER *cipher_type,
		EVP_CIPHER_CTX *cipher_ctx, struct php_openssl_cipher_mode *mode,
		zend_string **poutbuf, int *poutlen, const char *data, size_t data_len,
		const char *aad, size_t aad_len, int enc);

const EVP_CIPHER *php_openssl_get_evp_cipher_by_name(const char *method);

#endif
