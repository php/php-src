/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
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
   |          Wez Furlong <wez@thebrainroom.com>                          |
   |          Sascha Kettler <kettler@gmx.net>                            |
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
#include "ext/standard/php_fopen_wrappers.h"

/* OpenSSL includes */
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/crypto.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/conf.h>
#include <openssl/rand.h>

#define DEFAULT_KEY_LENGTH	512
#define MIN_KEY_LENGTH		384


#define DEBUG_SMIME	0

static unsigned char arg2_force_ref[] =
                       { 2, BYREF_NONE, BYREF_FORCE };
static unsigned char arg2and3_force_ref[] =
                       { 3, BYREF_NONE, BYREF_FORCE, BYREF_FORCE };

enum php_openssl_key_type {
	OPENSSL_KEYTYPE_RSA,
	OPENSSL_KEYTYPE_DSA,
	OPENSSL_KEYTYPE_DH,
	OPENSSL_KEYTYPE_DEFAULT = OPENSSL_KEYTYPE_RSA
};

enum php_openssl_cipher_type {
 	PHP_OPENSSL_CIPHER_RC2_40,
 	PHP_OPENSSL_CIPHER_RC2_128,
 	PHP_OPENSSL_CIPHER_RC2_64,
 	PHP_OPENSSL_CIPHER_DES,
 	PHP_OPENSSL_CIPHER_3DES,

	PHP_OPENSSL_CIPHER_DEFAULT = PHP_OPENSSL_CIPHER_RC2_40
};

/* {{{ openssl_functions[]
 */
function_entry openssl_functions[] = {
/* public/private key functions */
	PHP_FE(openssl_pkey_free,			NULL)
	PHP_FE(openssl_pkey_new,			NULL)
	PHP_FE(openssl_pkey_export,			arg2_force_ref)
	PHP_FE(openssl_pkey_export_to_file,	NULL)
	PHP_FE(openssl_pkey_get_private,	NULL)
	PHP_FE(openssl_pkey_get_public,		NULL)

	PHP_FALIAS(openssl_free_key,		openssl_pkey_free, 			NULL)
	PHP_FALIAS(openssl_get_privatekey,	openssl_pkey_get_private,	NULL)
	PHP_FALIAS(openssl_get_publickey,	openssl_pkey_get_public,	NULL)

/* x.509 cert funcs */
	PHP_FE(openssl_x509_read,				NULL)
	PHP_FE(openssl_x509_free,          		NULL)
	PHP_FE(openssl_x509_parse,			 	NULL)
	PHP_FE(openssl_x509_checkpurpose,		NULL)
	PHP_FE(openssl_x509_check_private_key,	NULL)
	PHP_FE(openssl_x509_export,				arg2_force_ref)
	PHP_FE(openssl_x509_export_to_file,		NULL)

/* CSR funcs */
	PHP_FE(openssl_csr_new,				arg2_force_ref)
	PHP_FE(openssl_csr_export,			arg2_force_ref)
	PHP_FE(openssl_csr_export_to_file,	NULL)
	PHP_FE(openssl_csr_sign,			NULL)

	
	PHP_FE(openssl_sign,               arg2_force_ref)
	PHP_FE(openssl_verify,             NULL)
	PHP_FE(openssl_seal,               arg2and3_force_ref)
	PHP_FE(openssl_open,               arg2_force_ref)

/* for S/MIME handling */
	PHP_FE(openssl_pkcs7_verify,	  	  NULL)
	PHP_FE(openssl_pkcs7_decrypt,		  NULL)
	PHP_FE(openssl_pkcs7_sign,			  NULL)
	PHP_FE(openssl_pkcs7_encrypt,		  NULL)

 	PHP_FE(openssl_private_encrypt,    arg2_force_ref)
 	PHP_FE(openssl_private_decrypt,    arg2_force_ref)
 	PHP_FE(openssl_public_encrypt,     arg2_force_ref)
 	PHP_FE(openssl_public_decrypt,     arg2_force_ref)

	PHP_FE(openssl_error_string, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ openssl_module_entry
 */
zend_module_entry openssl_module_entry = {
	STANDARD_MODULE_HEADER,
	"openssl",
	openssl_functions,
	PHP_MINIT(openssl),
	NULL,
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

/* {{{ openssl safe_mode & open_basedir checks */
inline static int php_openssl_safe_mode_chk(char *filename TSRMLS_DC)
{
	if (PG(safe_mode) && (!php_checkuid(filename, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
		return -1;
	}
	if (php_check_open_basedir(filename TSRMLS_CC)) {
		return -1;
	}
	
	return 0;
}
/* }}} */

/* {{{ openssl -> PHP "bridging" */
/* true global; readonly after module startup */
static char default_ssl_conf_filename[MAXPATHLEN];

struct php_x509_request {
	LHASH * global_config;	/* Global SSL config */
	LHASH * req_config;		/* SSL config for this request */
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
};


static X509 * php_openssl_x509_from_zval(zval ** val, int makeresource, long * resourceval TSRMLS_DC);
static EVP_PKEY * php_openssl_evp_from_zval(zval ** val, int public_key, char * passphrase, int makeresource, long * resourceval TSRMLS_DC);
static int php_openssl_is_private_key(EVP_PKEY* pkey TSRMLS_DC);
static X509_STORE 	  * setup_verify(zval * calist TSRMLS_DC);
static STACK_OF(X509) * load_all_certs_from_file(char *certfile);
static X509_REQ * php_openssl_csr_from_zval(zval ** val, int makeresource, long * resourceval TSRMLS_DC);
static EVP_PKEY * php_openssl_generate_private_key(struct php_x509_request * req TSRMLS_DC);


static void add_assoc_name_entry(zval * val, char * key, X509_NAME * name, int shortname TSRMLS_DC)
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

	for (i = 0; i < X509_NAME_entry_count(name); i++) {
		ne  = X509_NAME_get_entry(name, i);
		obj = X509_NAME_ENTRY_get_object(ne);
		str = X509_NAME_ENTRY_get_data(ne);
		nid = OBJ_obj2nid(obj);
		if (shortname) {
			sn = (char*)OBJ_nid2sn(nid);
			add_assoc_stringl(subitem, sn, str->data, str->length, 1);
		} else {
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

	if (timestr->length < 13) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "extension author too lazy to parse %s correctly", timestr->data);
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
	gmadjust = -(thetime.tm_isdst ? (long)timezone - 3600 : (long)timezone + 3600);
#endif
	ret += gmadjust;

	efree(strbuf);

	return ret;
}

static inline int php_openssl_config_check_syntax(
		const char * section_label,
		const char * config_filename,
		const char * section,
		LHASH * config TSRMLS_DC)
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

static int add_oid_section(struct php_x509_request * req TSRMLS_DC)
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

#define PHP_SSL_REQ_INIT(req)		memset(req, 0, sizeof(*req))
#define PHP_SSL_REQ_DISPOSE(req)	php_openssl_dispose_config(req TSRMLS_CC)
#define PHP_SSL_REQ_PARSE(req, zval)	php_openssl_parse_config(req, zval TSRMLS_CC)

#define PHP_SSL_CONFIG_SYNTAX_CHECK(var) if (req->var && php_openssl_config_check_syntax(#var, \
			req->config_filename, req->var, req->req_config TSRMLS_CC) == FAILURE) return FAILURE

#define SET_OPTIONAL_STRING_ARG(key, varname, defval)	\
	if (optional_args && zend_hash_find(Z_ARRVAL_P(optional_args), key, sizeof(key), (void**)&item) == SUCCESS) \
		varname = Z_STRVAL_PP(item); \
	else \
		varname = defval

#define SET_OPTIONAL_LONG_ARG(key, varname, defval)	\
	if (optional_args && zend_hash_find(Z_ARRVAL_P(optional_args), key, sizeof(key), (void**)&item) == SUCCESS) \
		varname = Z_LVAL_PP(item); \
	else \
		varname = defval



static int php_openssl_parse_config(
		struct php_x509_request * req,
		zval * optional_args
		TSRMLS_DC
		)
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
	if (str && !php_openssl_safe_mode_chk(str TSRMLS_CC)) {
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
	SET_OPTIONAL_STRING_ARG("req_extensions", req->extensions_section,
			CONF_get_string(req->req_config, req->request_extensions_section, "req_extensions"));
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
	
	/* digest alg */
	if (req->digest_name == NULL) {
		req->digest_name = CONF_get_string(req->req_config, req->section_name, "default_md");
	}
	if (req->digest_name) {
		req->digest = req->md_alg = EVP_get_digestbyname(req->digest_name);
	}
	if (req->md_alg == NULL) {
		req->md_alg = req->digest = EVP_md5();
	}

	PHP_SSL_CONFIG_SYNTAX_CHECK(extensions_section);

	/* set the string mask */
	str = CONF_get_string(req->req_config, req->section_name, "string_mask");
	if (str && !ASN1_STRING_set_default_mask_asc(str)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid global string mask setting %s", str);
		return FAILURE;
	}

	if (req->request_extensions_section == NULL) {
		req->request_extensions_section = CONF_get_string(req->req_config, req->section_name, "req_extensions");
	}
	PHP_SSL_CONFIG_SYNTAX_CHECK(request_extensions_section);
	
	return SUCCESS;
}

static void php_openssl_dispose_config(struct php_x509_request * req TSRMLS_DC)
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

static int php_openssl_load_rand_file(const char * file, int *egdsocket, int *seeded)
{
	char buffer[MAXPATHLEN];

	TSRMLS_FETCH();

	*egdsocket = 0;
	*seeded = 0;
	
#ifdef WINDOWS
	RAND_screen();
#endif
	if (file == NULL) {
		file = RAND_file_name(buffer, sizeof(buffer));
	} else if (RAND_egd(file) > 0) {
		/* if the given filename is an EGD socket, don't
		 * write anything back to it */
		*egdsocket = 1;
		return SUCCESS;
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

static int php_openssl_write_rand_file(const char * file, int egdsocket, int seeded)
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
	if (file == NULL || !RAND_write_file(file)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to write random state");
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(openssl)
{
	char * config_filename;
	
	le_key = zend_register_list_destructors_ex(php_pkey_free, NULL, "OpenSSL key", module_number);
	le_x509 = zend_register_list_destructors_ex(php_x509_free, NULL, "OpenSSL X.509", module_number);
	le_csr = zend_register_list_destructors_ex(php_csr_free, NULL, "OpenSSL X.509 CSR", module_number);

	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
	OpenSSL_add_all_algorithms();

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
#ifdef X509_PURPOSE_ANY
	REGISTER_LONG_CONSTANT("X509_PURPOSE_ANY", X509_PURPOSE_ANY, CONST_CS|CONST_PERSISTENT);
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

	/* Ciphers */
 	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_RC2_40", PHP_OPENSSL_CIPHER_RC2_40, CONST_CS|CONST_PERSISTENT);
 	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_RC2_128", PHP_OPENSSL_CIPHER_RC2_128, CONST_CS|CONST_PERSISTENT);
 	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_RC2_64", PHP_OPENSSL_CIPHER_RC2_64, CONST_CS|CONST_PERSISTENT);
 	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_DES", PHP_OPENSSL_CIPHER_DES, CONST_CS|CONST_PERSISTENT);
 	REGISTER_LONG_CONSTANT("OPENSSL_CIPHER_3DES", PHP_OPENSSL_CIPHER_3DES, CONST_CS|CONST_PERSISTENT);
	
	/* Values for key types */
	REGISTER_LONG_CONSTANT("OPENSSL_KEYTYPE_RSA", OPENSSL_KEYTYPE_RSA, CONST_CS|CONST_PERSISTENT);
#ifndef NO_DSA
	REGISTER_LONG_CONSTANT("OPENSSL_KEYTYPE_DSA", OPENSSL_KEYTYPE_DSA, CONST_CS|CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("OPENSSL_KEYTYPE_DH", OPENSSL_KEYTYPE_DH, CONST_CS|CONST_PERSISTENT);
	
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
	php_stream_xport_register("tls", php_openssl_ssl_socket_factory TSRMLS_CC);

	/* override the default tcp socket provider */
	php_stream_xport_register("tcp", php_openssl_ssl_socket_factory TSRMLS_CC);

	php_register_url_stream_wrapper("https", &php_stream_http_wrapper TSRMLS_CC);
	php_register_url_stream_wrapper("ftps", &php_stream_ftp_wrapper TSRMLS_CC);
	
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

	php_unregister_url_stream_wrapper("https" TSRMLS_CC);
	php_unregister_url_stream_wrapper("ftps" TSRMLS_CC);

	php_stream_xport_unregister("ssl" TSRMLS_CC);
	php_stream_xport_unregister("tls" TSRMLS_CC);

	/* reinstate the default tcp handler */
	php_stream_xport_register("tcp", php_stream_generic_socket_factory TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/* {{{ x509 cert functions */

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
	/* force it to be a string and check if it refers to a file */
	convert_to_string_ex(val);

	if (Z_STRLEN_PP(val) > 7 && memcmp(Z_STRVAL_PP(val), "file://", sizeof("file://") - 1) == 0) {
		/* read cert from the named file */
		BIO *in;

		if (php_openssl_safe_mode_chk(Z_STRVAL_PP(val) + (sizeof("file://") - 1) TSRMLS_CC)) {
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
		cert = (X509 *) PEM_ASN1_read_bio((char *(*)())d2i_X509, PEM_STRING_X509, in, NULL, NULL, NULL);
		BIO_free(in);
	}

	if (cert && makeresource && resourceval) {
		*resourceval = zend_list_insert(cert, le_x509);
	}
	return cert;
}

/* }}} */

/* {{{ proto bool openssl_x509_export_to_file(mixed x509, string outfilename [, bool notext = true])
   Exports a CERT to file or a var */
PHP_FUNCTION(openssl_x509_export_to_file)
{
	X509 * cert;
	zval * zcert = NULL;
	zend_bool notext = 1;
	BIO * bio_out;
	long certresource;
	char * filename;
	long filename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|b", &zcert, &filename, &filename_len, &notext) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	cert = php_openssl_x509_from_zval(&zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get cert from parameter 1");
		return;
	}

	if (php_openssl_safe_mode_chk(filename TSRMLS_CC)) {
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

/* {{{ proto bool openssl_x509_export(mixed x509, string &out [, bool notext = true])
   Exports a CERT to file or a var */
PHP_FUNCTION(openssl_x509_export)
{
	X509 * cert;
	zval * zcert = NULL, *zout=NULL;
	zend_bool notext = 1;
	BIO * bio_out;
	long certresource;
	char * bio_mem_ptr;
	long bio_mem_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz|b", &zcert, &zout, &notext) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	cert = php_openssl_x509_from_zval(&zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get cert from parameter 1");
		return;
	}

	bio_out = BIO_new(BIO_s_mem());
	if (!notext) {
		X509_print(bio_out, cert);
	}
	PEM_write_bio_X509(bio_out, cert);

	bio_mem_len = BIO_get_mem_data(bio_out, &bio_mem_ptr);
	ZVAL_STRINGL(zout, bio_mem_ptr, bio_mem_len, 1);

	RETVAL_TRUE;

	if (certresource == -1 && cert) {
		X509_free(cert);
	}
	BIO_free(bio_out);
}
/* }}} */

/* {{{ proto bool openssl_x509_check_private_key(mixed cert, mixed key)
   Checks if a private key corresponds to a CERT */
PHP_FUNCTION(openssl_x509_check_private_key)
{
	zval * zcert, *zkey;
	X509 * cert = NULL;
	EVP_PKEY * key = NULL;
	long certresource = -1, keyresource = -1;

	RETVAL_FALSE;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &zcert, &zkey) == FAILURE) {
		return;
	}
	cert = php_openssl_x509_from_zval(&zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		RETURN_FALSE;
	}	
	key = php_openssl_evp_from_zval(&zkey, 0, "", 1, &keyresource TSRMLS_CC);
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

/* {{{ proto array openssl_x509_parse(mixed x509 [, bool shortnames=true])
   Returns an array of the fields/values of the CERT */
PHP_FUNCTION(openssl_x509_parse)
{
	zval * zcert;
	X509 * cert = NULL;
	long certresource = -1;
	int i;
	zend_bool useshortnames = 1;
	char * tmpstr;
	zval * subitem;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &zcert, &useshortnames) == FAILURE) {
		return;
	}
	cert = php_openssl_x509_from_zval(&zcert, 0, &certresource TSRMLS_CC);
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
	add_assoc_long(return_value, "serialNumber", 		ASN1_INTEGER_get(X509_get_serialNumber(cert)));

	add_assoc_asn1_string(return_value, "validFrom", 	X509_get_notBefore(cert));
	add_assoc_asn1_string(return_value, "validTo", 		X509_get_notAfter(cert));

	add_assoc_long(return_value, "validFrom_time_t", 	asn1_time_to_time_t(X509_get_notBefore(cert) TSRMLS_CC));
	add_assoc_long(return_value, "validTo_time_t", 		asn1_time_to_time_t(X509_get_notAfter(cert) TSRMLS_CC));

	tmpstr = X509_alias_get0(cert, NULL);
	if (tmpstr) {
		add_assoc_string(return_value, "alias", tmpstr, 1);
	}
/*
	add_assoc_long(return_value, "signaturetypeLONG", X509_get_signature_type(cert));
	add_assoc_string(return_value, "signaturetype", OBJ_nid2sn(X509_get_signature_type(cert)), 1);
	add_assoc_string(return_value, "signaturetypeLN", OBJ_nid2ln(X509_get_signature_type(cert)), 1);
*/
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

	if (php_openssl_safe_mode_chk(certfile TSRMLS_CC)) {
		goto end;
	}

	if(!(in=BIO_new_file(certfile, "r"))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error opening the file, %s", certfile);
		goto end;
	}

	/* This loads from a file, a stack of x509/crl/pkey sets */
	if(!(sk=PEM_X509_INFO_read_bio(in, NULL, NULL, NULL))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error reading the file, %s", certfile);
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
	zval * zcert, * zcainfo = NULL;
	X509_STORE * cainfo = NULL;
	X509 * cert = NULL;
	long certresource = -1;
	STACK_OF(X509) * untrustedchain = NULL;
	long purpose;
	char * untrusted = NULL;
	long untrusted_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl|a!s", &zcert, &purpose, &zcainfo, &untrusted, &untrusted_len)
			== FAILURE) {
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
	cert = php_openssl_x509_from_zval(&zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		goto clean_exit;
	}
	RETVAL_LONG(check_cert(cainfo, cert, untrustedchain, purpose));

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
	zval *cert;
	X509 *x509;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &cert) == FAILURE) {
		return;
	}
	Z_TYPE_P(return_value) = IS_RESOURCE;
	x509 = php_openssl_x509_from_zval(&cert, 1, &Z_LVAL_P(return_value) TSRMLS_CC);

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
			char * strindex; int strindexlen;
			long intindex;
			
			zend_hash_get_current_key_ex(HASH_OF(dn), &strindex, &strindexlen, &intindex, 0, &hpos);

			convert_to_string_ex(item);

			if (strindex) {
				int nid;

				nid = OBJ_txt2nid(strindex);
				if (nid != NID_undef) {
					if (!X509_NAME_add_entry_by_NID(subj, nid, MBSTRING_ASC, 
								(unsigned char*)Z_STRVAL_PP(item), -1, -1, 0))
					{
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "dn: add_entry_by_NID %d -> %s (failed)", nid, Z_STRVAL_PP(item));
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
			char buffer[200];
			
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
			if (!X509_NAME_add_entry_by_txt(subj, type, MBSTRING_ASC, (unsigned char*)v->value, -1, -1, 0)) {
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
				char * strindex; int strindexlen;
				long intindex;

				zend_hash_get_current_key_ex(HASH_OF(attribs), &strindex, &strindexlen, &intindex, 0, &hpos);
				convert_to_string_ex(item);

				if (strindex) {
					int nid;

					nid = OBJ_txt2nid(strindex);
					if (nid != NID_undef) {
						if (!X509_NAME_add_entry_by_NID(subj, nid, MBSTRING_ASC, (unsigned char*)Z_STRVAL_PP(item), -1, -1, 0)) {
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
				if (!X509_REQ_add1_attr_by_txt(csr, v->name, MBSTRING_ASC, (unsigned char*)v->value, -1)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "add1_attr_by_txt %s -> %s (failed)", v->name, v->value);
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
		if (php_openssl_safe_mode_chk(filename TSRMLS_CC)) {
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
	char * filename = NULL; long filename_len;
	BIO * bio_out;
	long csr_resource;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|b", &zcsr, &filename, &filename_len, &notext) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	csr = php_openssl_csr_from_zval(&zcsr, 0, &csr_resource TSRMLS_CC);
	if (csr == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get CSR from parameter 1");
		return;
	}

	if (php_openssl_safe_mode_chk(filename TSRMLS_CC)) {
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
	char * bio_mem_ptr;
	long bio_mem_len;

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
	PEM_write_bio_X509_REQ(bio_out, csr);

	bio_mem_len = BIO_get_mem_data(bio_out, &bio_mem_ptr);
	ZVAL_STRINGL(zout, bio_mem_ptr, bio_mem_len, 1);

	RETVAL_TRUE;

	if (csr_resource == -1 && csr) {
		X509_REQ_free(csr);
	}
	BIO_free(bio_out);
}
/* }}} */

/* {{{ proto resource openssl_csr_sign(mixed csr, mixed x509, mixed priv_key, long days)
   Signs a cert with another CERT */
PHP_FUNCTION(openssl_csr_sign)
{
	zval * zcert = NULL, *zcsr, *zpkey, *args = NULL;
	long num_days;
	X509 * cert = NULL, *new_cert = NULL;
	X509_REQ * csr;
	EVP_PKEY * key = NULL, *priv_key;
	long csr_resource, certresource, keyresource;
	int i;
	struct php_x509_request req;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz!zl|a!", &zcsr, &zcert, &zpkey, &num_days, &args) == FAILURE)
		return;

	RETVAL_FALSE;
	PHP_SSL_REQ_INIT(&req);
	
	csr = php_openssl_csr_from_zval(&zcsr, 0, &csr_resource TSRMLS_CC);
	if (csr == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get CSR from parameter 1");
		return;
	}
	if (zcert) {
		cert = php_openssl_x509_from_zval(&zcert, 0, &certresource TSRMLS_CC);
		if (cert == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get cert from parameter 2");
			goto cleanup;
		}
	}
	priv_key = php_openssl_evp_from_zval(&zpkey, 0, "", 1, &keyresource TSRMLS_CC);
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
	if (!X509_set_version(new_cert, 3))
		goto cleanup;

	/* TODO: Allow specifying */
	ASN1_INTEGER_set(X509_get_serialNumber(new_cert), 0L);
	
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
	if (req.request_extensions_section) {
		X509V3_CTX ctx;
		
		X509V3_set_ctx(&ctx, cert, new_cert, csr, NULL, 0);
		X509V3_set_conf_lhash(&ctx, req.req_config);
		if (!X509V3_EXT_add_conf(req.req_config, &ctx, req.request_extensions_section, new_cert)) {
			goto cleanup;
		}
	}

	/* Now sign it */
	if (!X509_sign(new_cert, priv_key, req.digest)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to sign it");
		goto cleanup;
	}
	
	/* Succeeded; lets return the cert */
	RETVAL_RESOURCE(zend_list_insert(new_cert, le_x509));
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

/* {{{ proto bool openssl_csr_new(array dn, resource &privkey [, array extraattribs, array configargs])
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
							RETVAL_RESOURCE(zend_list_insert(csr, le_csr));
							csr = NULL;			
						} else {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error signing request");
						}

						if (we_made_the_key) {
							/* and a resource for the private key */
							ZVAL_RESOURCE(out_pkey, zend_list_insert(req.priv_key, le_key));
						} else if (key_resource != -1) {
							req.priv_key = NULL; /* make sure the cleanup code doesn't zap it! */
						}
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
		convert_to_string_ex(zphrase);
		passphrase = Z_STRVAL_PP(zphrase);

		/* now set val to be the key param and continue */
		if (zend_hash_index_find(HASH_OF(*val), 0, (void **)&val) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key array must be of the form array(0 => key, 1 => phrase)");
			return NULL;
		}
	}

	if (Z_TYPE_PP(val) == IS_RESOURCE) {
		void * what;
		int type;

		what = zend_fetch_resource(val TSRMLS_CC, -1, "OpenSSL X.509/key", &type, 2, le_x509, le_key);
		if (!what) {
			return NULL;
		}
		if (resourceval) { 
			*resourceval = Z_LVAL_PP(val);
		}
		if (type == le_x509) {
			/* extract key from cert, depending on public_key param */
			cert = (X509*)what;
			free_cert = 0;
		} else if (type == le_key) {
			/* check whether it is actually a private key if requested */
			if (!public_key && !php_openssl_is_private_key((EVP_PKEY*)what TSRMLS_CC)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "supplied key param is a public key");
				return NULL;
			}
			
			/* got the key - return it */
			return (EVP_PKEY*)what;
		}

		/* other types could be used here - eg: file pointers and read in the data from them */

		return NULL;
	} else {
		/* force it to be a string and check if it refers to a file */
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
					return NULL;
				}
				key = PEM_read_bio_PUBKEY(in, NULL,NULL, NULL);
				BIO_free(in);
			}
		} else {
			/* we want the private key */
			BIO *in;

			if (filename) {
				if (php_openssl_safe_mode_chk(filename TSRMLS_CC)) {
					return NULL;
				}
				in = BIO_new_file(filename, "r");
			} else {
				in = BIO_new_mem_buf(Z_STRVAL_PP(val), Z_STRLEN_PP(val));
			}

			if (in == NULL) {
				return NULL;
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
	php_openssl_load_rand_file(randfile, &egdsocket, &seeded);
	
	if ((req->priv_key = EVP_PKEY_new()) != NULL) {
		switch(req->priv_key_type) {
			case OPENSSL_KEYTYPE_RSA:
				if (EVP_PKEY_assign_RSA(req->priv_key, RSA_generate_key(req->priv_key_bits, 0x10001, NULL, NULL)))
					return_val = req->priv_key;
				break;
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

			if (NULL == pkey->pkey.rsa->p || NULL == pkey->pkey.rsa->q) {
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
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key type not supported in this PHP build!");
			break;
	}
	return 1;
}
/* }}} */

/* {{{ proto resource openssl_pkey_new([array configargs])
   Generates a new private key */
PHP_FUNCTION(openssl_pkey_new)
{
	struct php_x509_request req;
	zval * args = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|a!", &args) == FAILURE) {
		return;
	}
	RETVAL_FALSE;
	
	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS)
	{
		if (php_openssl_generate_private_key(&req TSRMLS_CC)) {
			/* pass back a key resource */
			RETVAL_RESOURCE(zend_list_insert(req.priv_key, le_key));
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
	zval * zpkey, * args = NULL;
	char * passphrase = NULL; long passphrase_len = 0;
	char * filename = NULL; long filename_len = 0;
	long key_resource = -1;
	EVP_PKEY * key;
	BIO * bio_out = NULL;
	EVP_CIPHER * cipher;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs|s!a!", &zpkey, &filename, &filename_len, &passphrase, &passphrase_len, &args) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	key = php_openssl_evp_from_zval(&zpkey, 0, passphrase, 0, &key_resource TSRMLS_CC);

	if (key == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get key from parameter 1");
		RETURN_FALSE;
	}
	
	if (php_openssl_safe_mode_chk(filename TSRMLS_CC)) {
		RETURN_FALSE;
	}
	
	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS) {
		bio_out = BIO_new_file(filename, "w");

		if (passphrase && req.priv_key_encrypt) {
			cipher = EVP_des_ede3_cbc();
		} else {
			cipher = NULL;
		}
		if (PEM_write_bio_PrivateKey(bio_out, key, cipher, passphrase, passphrase_len, NULL, NULL)) {
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
	zval * zpkey, * args = NULL, *out;
	char * passphrase = NULL; long passphrase_len = 0;
	long key_resource = -1;
	EVP_PKEY * key;
	BIO * bio_out = NULL;
	EVP_CIPHER * cipher;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|s!a!", &zpkey, &out, &passphrase, &passphrase_len, &args) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	key = php_openssl_evp_from_zval(&zpkey, 0, passphrase, 0, &key_resource TSRMLS_CC);

	if (key == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "cannot get key from parameter 1");
		RETURN_FALSE;
	}
	
	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS) {
		bio_out = BIO_new(BIO_s_mem());

		if (passphrase && req.priv_key_encrypt) {
			cipher = EVP_des_ede3_cbc();
		} else {
			cipher = NULL;
		}
		if (PEM_write_bio_PrivateKey(bio_out, key, cipher, passphrase, passphrase_len, NULL, NULL)) {
			/* Success!
			 * If returning the output as a string, do so now */

			char * bio_mem_ptr;
			long bio_mem_len;
			RETVAL_TRUE;

			bio_mem_len = BIO_get_mem_data(bio_out, &bio_mem_ptr);
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
	zval *cert;
	EVP_PKEY *pkey;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &cert) == FAILURE) {
		return;
	}
	Z_TYPE_P(return_value) = IS_RESOURCE;
	pkey = php_openssl_evp_from_zval(&cert, 1, NULL, 1, &Z_LVAL_P(return_value) TSRMLS_CC);

	if (pkey == NULL) {
		RETURN_FALSE;
	}
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
	zval *cert;
	EVP_PKEY *pkey;
	char * passphrase = "";
	long passphrase_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|s", &cert, &passphrase, &passphrase_len) == FAILURE) {
		return;
	}
	Z_TYPE_P(return_value) = IS_RESOURCE;
	pkey = php_openssl_evp_from_zval(&cert, 0, passphrase, 1, &Z_LVAL_P(return_value) TSRMLS_CC);

	if (pkey == NULL) {
		RETURN_FALSE;
	}
}

/* }}} */

/* }}} */

/* {{{ PKCS7 S/MIME functions */

/* {{{ proto bool openssl_pkcs7_verify(string filename, long flags [, string signerscerts [, array cainfo [, string extracerts]]])
   Verifys that the data block is intact, the signer is who they say they are, and returns the CERTs of the signers */
PHP_FUNCTION(openssl_pkcs7_verify)
{
	X509_STORE * store = NULL;
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|sas", &filename, &filename_len,
				&flags, &signersfilename, &signersfilename_len, &cainfo,
				&extracerts, &extracerts_len) == FAILURE) {
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
	if (php_openssl_safe_mode_chk(filename TSRMLS_CC)) {
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
#if DEBUG_SMIME
	zend_printf("Calling PKCS7 verify\n");
#endif

	if (PKCS7_verify(p7, others, store, datain, NULL, flags)) {

		RETVAL_TRUE;

		if (signersfilename) {
			BIO *certout;
		
			if (php_openssl_safe_mode_chk(filename TSRMLS_CC)) {
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
	PKCS7_free(p7);
	sk_X509_free(others);
}
/* }}} */

/* {{{ proto bool openssl_pkcs7_encrypt(string infile, string outfile, mixed recipcerts, array headers [, long flags [, long cipher]])
   Encrypts the message in the file named infile with the certificates in recipcerts and output the result to the file named outfile */
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
	long cipherid = PHP_OPENSSL_CIPHER_DEFAULT;
	uint strindexlen;
	ulong intindex;
	char * strindex;
	char * infilename = NULL;	long infilename_len;
	char * outfilename = NULL;	long outfilename_len;
	
	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssza!|ll", &infilename, &infilename_len,
				&outfilename, &outfilename_len, &zrecipcerts, &zheaders, &flags, &cipherid) == FAILURE)
		return;

	
	if (php_openssl_safe_mode_chk(infilename TSRMLS_CC) || php_openssl_safe_mode_chk(outfilename TSRMLS_CC)) {
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
	if (Z_TYPE_P(zrecipcerts) == IS_ARRAY) {
		zend_hash_internal_pointer_reset_ex(HASH_OF(zrecipcerts), &hpos);
		while(zend_hash_get_current_data_ex(HASH_OF(zrecipcerts), (void**)&zcertval, &hpos) == SUCCESS) {
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

			zend_hash_move_forward_ex(HASH_OF(zrecipcerts), &hpos);
		}
	} else {
		/* a single certificate */
		long certresource;

		cert = php_openssl_x509_from_zval(&zrecipcerts, 0, &certresource TSRMLS_CC);
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
	switch (cipherid) {
		case PHP_OPENSSL_CIPHER_RC2_40:
			cipher = EVP_rc2_40_cbc();
			break;
		case PHP_OPENSSL_CIPHER_RC2_64:
			cipher = EVP_rc2_64_cbc();
			break;
		case PHP_OPENSSL_CIPHER_RC2_128:
			cipher = EVP_rc2_cbc();
			break;
		case PHP_OPENSSL_CIPHER_DES:
			cipher = EVP_des_cbc();
			break;
		case PHP_OPENSSL_CIPHER_3DES:
			cipher = EVP_des_ede3_cbc();
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid cipher type `%d'", cipherid);
			goto clean_exit;
	}
	if (cipher == NULL) {
		/* shouldn't happen */
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to get cipher");
		goto clean_exit;
	}

	p7 = PKCS7_encrypt(recipcerts, infile, cipher, flags);

	if (p7 == NULL) {
		goto clean_exit;
	}

	/* tack on extra headers */
	if (zheaders) {
		zend_hash_internal_pointer_reset_ex(HASH_OF(zheaders), &hpos);
		while(zend_hash_get_current_data_ex(HASH_OF(zheaders), (void**)&zcertval, &hpos) == SUCCESS) {
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

	BIO_reset(infile);

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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sszza!|ls",
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

	privkey = php_openssl_evp_from_zval(&zprivkey, 0, "", 0, &keyresource TSRMLS_CC);
	if (privkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error getting private key");
		goto clean_exit;
	}

	cert = php_openssl_x509_from_zval(&zcert, 0, &certresource TSRMLS_CC);
	if (cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error getting cert");
		goto clean_exit;
	}

	if (php_openssl_safe_mode_chk(infilename TSRMLS_CC) || php_openssl_safe_mode_chk(outfilename TSRMLS_CC)) {
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

	BIO_reset(infile);

	/* tack on extra headers */
	if (zheaders) {
		zend_hash_internal_pointer_reset_ex(HASH_OF(zheaders), &hpos);
		while(zend_hash_get_current_data_ex(HASH_OF(zheaders), (void**)&hval, &hpos) == SUCCESS) {
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
	zval * recipcert, * recipkey = NULL;
	X509 * cert = NULL;
	EVP_PKEY * key = NULL;
	long certresval, keyresval;
	BIO * in = NULL, * out = NULL, * datain = NULL;
	PKCS7 * p7 = NULL;
	char * infilename;	long infilename_len;
	char * outfilename;	long outfilename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssz|z", &infilename, &infilename_len,
				&outfilename, &outfilename_len, &recipcert, &recipkey) == FAILURE) {
		return;
	}

	RETVAL_FALSE;

	cert = php_openssl_x509_from_zval(&recipcert, 0, &certresval TSRMLS_CC);
	if (cert == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to coerce parameter 3 to x509 cert");
		goto clean_exit;
	}

	key = php_openssl_evp_from_zval(recipkey ? &recipkey : &recipcert, 0, "", 0, &keyresval TSRMLS_CC);
	if (key == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to get private key");
		goto clean_exit;
	}
	
	if (php_openssl_safe_mode_chk(infilename TSRMLS_CC) || php_openssl_safe_mode_chk(outfilename TSRMLS_CC)) {
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

/* {{{ proto bool openssl_private_encrypt(string data, string crypted, mixed key [, int padding])
   Encrypts data with private key */
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE) { 
		return;
	}
	RETVAL_FALSE;

	pkey = php_openssl_evp_from_zval(&key, 0, "", 0, &keyresource TSRMLS_CC);

	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "key param is not a valid private key");
		RETURN_FALSE;
	}
	
	cryptedlen = EVP_PKEY_size(pkey);
	cryptedbuf = emalloc(cryptedlen + 1);

	switch (Z_TYPE_P(pkey)) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			successful =  (RSA_private_encrypt(data_len, 
						data, 
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
		ZVAL_STRINGL(crypted, cryptedbuf, cryptedlen, 0);
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

/* {{{ proto bool openssl_private_decrypt(string data, string decrypted, mixed key [, int padding])
   Decrypts data with private key */
PHP_FUNCTION(openssl_private_decrypt)
{
	zval *key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	unsigned char *cryptedbuf = NULL;
	unsigned char *crypttemp;
	int successful = 0;
	long padding = RSA_PKCS1_PADDING;
	long keyresource = -1;
	char * data;
	long data_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE) {
		return;
	}
	RETVAL_FALSE;
	
	pkey = php_openssl_evp_from_zval(&key, 0, "", 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "key parameter is not a valid private key");
		RETURN_FALSE;
	}
	
	cryptedlen = EVP_PKEY_size(pkey);
	crypttemp = emalloc(cryptedlen + 1);

	switch (Z_TYPE_P(pkey)) {
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key type not supported in this PHP build!");
	}

	efree(crypttemp);

	if (successful) {
		zval_dtor(crypted);
		cryptedbuf[cryptedlen] = '\0';
		ZVAL_STRINGL(crypted, cryptedbuf, cryptedlen, 0);
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

/* {{{ proto bool openssl_public_encrypt(string data, string crypted, mixed key [, int padding])
   Encrypts data with public key */
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
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE)
		return;

	RETVAL_FALSE;
	
	pkey = php_openssl_evp_from_zval(&key, 1, NULL, 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "key parameter is not a valid public key");
		RETURN_FALSE;
	}

	cryptedlen = EVP_PKEY_size(pkey);
	cryptedbuf = emalloc(cryptedlen + 1);

	switch (Z_TYPE_P(pkey)) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			successful = (RSA_public_encrypt(data_len, 
						data, 
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
		ZVAL_STRINGL(crypted, cryptedbuf, cryptedlen, 0);
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

/* {{{ proto bool openssl_public_decrypt(string data, string crypted, resource key [, int padding])
   Decrypts data with public key */
PHP_FUNCTION(openssl_public_decrypt)
{
	zval *key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	unsigned char *cryptedbuf = NULL;
	unsigned char *crypttemp;
	int successful = 0;
	long keyresource = -1;
	long padding = RSA_PKCS1_PADDING;
	char * data;
	long data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE) {
		return;
	}
	RETVAL_FALSE;
	
	pkey = php_openssl_evp_from_zval(&key, 1, NULL, 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "key parameter is not a valid public key");
		RETURN_FALSE;
	}

	cryptedlen = EVP_PKEY_size(pkey);
	crypttemp = emalloc(cryptedlen + 1);

	switch (Z_TYPE_P(pkey)) {
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "key type not supported in this PHP build!");
		 
	}

	efree(crypttemp);

	if (successful) {
		zval_dtor(crypted);
		cryptedbuf[cryptedlen] = '\0';
		ZVAL_STRINGL(crypted, cryptedbuf, cryptedlen, 0);
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

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	val = ERR_get_error();
	if (val) {
		RETURN_STRING(ERR_error_string(val, buf), 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool openssl_sign(string data, &string signature, mixed key)
   Signs data */
PHP_FUNCTION(openssl_sign)
{
	zval *key, *signature;
	EVP_PKEY *pkey;
	int siglen;
	unsigned char *sigbuf;
	long keyresource = -1;
	char * data;	long data_len;
	EVP_MD_CTX md_ctx;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szz", &data, &data_len, &signature, &key) == FAILURE) {
		return;
	}
	pkey = php_openssl_evp_from_zval(&key, 0, "", 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "supplied key param cannot be coerced into a private key");
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
	if (keyresource == -1) {
		EVP_PKEY_free(pkey);
	}
}
/* }}} */

/* {{{ proto int openssl_verify(string data, string signature, mixed key)
   Verifys data */
PHP_FUNCTION(openssl_verify)
{
	zval *key;
	EVP_PKEY *pkey;
	int err;
	EVP_MD_CTX     md_ctx;
	long keyresource = -1;
	char * data;	long data_len;
	char * signature;	long signature_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssz", &data, &data_len, &signature, &signature_len, &key) == FAILURE) {
		return;
	}
	
	pkey = php_openssl_evp_from_zval(&key, 1, NULL, 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "supplied key param cannot be coerced into a public key");
		RETURN_FALSE;
	}

	EVP_VerifyInit   (&md_ctx, EVP_sha1());
	EVP_VerifyUpdate (&md_ctx, data, data_len);
	err = EVP_VerifyFinal (&md_ctx, signature, signature_len, pkey);

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
	char * data;	long data_len;
	EVP_CIPHER_CTX ctx;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szza/", &data, &data_len, &sealdata, &ekeys, &pubkeys) == FAILURE) {
		return;
	}
	
	pubkeysht = HASH_OF(pubkeys);
	nkeys = pubkeysht ? zend_hash_num_elements(pubkeysht) : 0;
	if (!nkeys) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Fourth argument to openssl_seal() must be a non-empty array");
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
		if (pkeys[i] == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "not a public key (%dth member of pubkeys)", i);
			RETVAL_FALSE;
			goto clean_exit;
		}
		eks[i] = emalloc(EVP_PKEY_size(pkeys[i]) + 1);
		zend_hash_move_forward_ex(pubkeysht, &pos);
		i++;
	}

	if (!EVP_EncryptInit(&ctx,EVP_rc4(),NULL,NULL)) {
		RETVAL_FALSE;
		goto clean_exit;
	}

#if 0
	/* Need this if allow ciphers that require initialization vector */
	ivlen = EVP_CIPHER_CTX_iv_length(&ctx);
	iv = ivlen ? emalloc(ivlen + 1) : NULL;
#endif
	/* allocate one byte extra to make room for \0 */
	buf = emalloc(data_len + EVP_CIPHER_CTX_block_size(&ctx));

	if (!EVP_SealInit(&ctx, EVP_rc4(), eks, eksl, NULL, pkeys, nkeys) || !EVP_SealUpdate(&ctx, buf, &len1, data, data_len)) {
		RETVAL_FALSE;
		efree(buf);
		goto clean_exit;
	}

	EVP_SealFinal(&ctx, buf + len1, &len2);

	if (len1 + len2 > 0) {
		zval_dtor(sealdata);
		buf[len1 + len2] = '\0';
		buf = erealloc(buf, len1 + len2 + 1);
		ZVAL_STRINGL(sealdata, buf, len1 + len2, 0);

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
	zval *privkey, *opendata;
	EVP_PKEY *pkey;
	int len1, len2;
	unsigned char *buf;
	long keyresource = -1;
	EVP_CIPHER_CTX ctx;
	char * data;	long data_len;
	char * ekey;	long ekey_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szsz", &data, &data_len, &opendata, &ekey, &ekey_len, &privkey) == FAILURE) {
		return;
	}

	pkey = php_openssl_evp_from_zval(&privkey, 0, "", 0, &keyresource TSRMLS_CC);
	if (pkey == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to coerce parameter 4 into a private key");
		RETURN_FALSE;
	}
	buf = emalloc(data_len + 1);

	if (EVP_OpenInit(&ctx, EVP_rc4(), ekey, ekey_len, NULL, pkey) && EVP_OpenUpdate(&ctx, buf, &len1, data, data_len)) {
		if (!EVP_OpenFinal(&ctx, buf + len1, &len2) || (len1 + len2 == 0)) {
			efree(buf);
			if (keyresource == -1) { 
				EVP_PKEY_free(pkey);
			}
			RETURN_FALSE;
		}
	} else {
		efree(buf);
		if (keyresource == -1) {
			EVP_PKEY_free(pkey);
		}
		RETURN_FALSE;
	}
	if (keyresource == -1) {
		EVP_PKEY_free(pkey);
	}
	zval_dtor(opendata);
	buf[len1 + len2] = '\0';
	ZVAL_STRINGL(opendata, erealloc(buf, len1 + len2 + 1), len1 + len2, 0);
	RETURN_TRUE;
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
