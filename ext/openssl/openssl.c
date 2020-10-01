/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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
   |          Jakub Zelenka <bukka@php.net>                               |
   |          Eliot Lear <lear@ofcourseimright.com>                       |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_openssl.h"
#include "zend_exceptions.h"
#include "Zend/zend_interfaces.h"

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
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/dsa.h>
#include <openssl/dh.h>
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

/* Common */
#include <time.h>

#if (defined(PHP_WIN32) && defined(_MSC_VER) && _MSC_VER >= 1900)
#define timezone _timezone	/* timezone is called _timezone in LibC */
#endif

#define MIN_KEY_LENGTH		384

#define OPENSSL_ALGO_SHA1 	1
#define OPENSSL_ALGO_MD5	2
#define OPENSSL_ALGO_MD4	3
#ifdef HAVE_OPENSSL_MD2_H
#define OPENSSL_ALGO_MD2	4
#endif
#if PHP_OPENSSL_API_VERSION < 0x10100
#define OPENSSL_ALGO_DSS1	5
#endif
#define OPENSSL_ALGO_SHA224 6
#define OPENSSL_ALGO_SHA256 7
#define OPENSSL_ALGO_SHA384 8
#define OPENSSL_ALGO_SHA512 9
#define OPENSSL_ALGO_RMD160 10
#define DEBUG_SMIME	0

#if !defined(OPENSSL_NO_EC) && defined(EVP_PKEY_EC)
#define HAVE_EVP_PKEY_EC 1
#endif

#include "openssl_arginfo.h"

ZEND_DECLARE_MODULE_GLOBALS(openssl)

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

/* Add some encoding rules.  This is normally handled through filters
 * in the OpenSSL code, but we will do that part as if we were one
 * of the OpenSSL binaries along the lines of -outform {DER|CMS|PEM}
 */
enum php_openssl_encoding {
	ENCODING_DER,
	ENCODING_SMIME,
	ENCODING_PEM,
};

/* OpenSSLCertificate class */

zend_class_entry *php_openssl_certificate_ce;

static zend_object_handlers php_openssl_certificate_object_handlers;

static zend_object *php_openssl_certificate_create_object(zend_class_entry *class_type) {
	php_openssl_certificate_object *intern = zend_object_alloc(sizeof(php_openssl_certificate_object), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &php_openssl_certificate_object_handlers;

	return &intern->std;
}

static zend_function *php_openssl_certificate_get_constructor(zend_object *object) {
	zend_throw_error(NULL, "Cannot directly construct OpenSSLCertificate, use openssl_x509_read() instead");
	return NULL;
}

static void php_openssl_certificate_free_obj(zend_object *object)
{
	php_openssl_certificate_object *x509_object = php_openssl_certificate_from_obj(object);

	X509_free(x509_object->x509);
	zend_object_std_dtor(&x509_object->std);
}

/* OpenSSLCertificateSigningRequest class */

typedef struct _php_openssl_x509_request_object {
	X509_REQ *csr;
	zend_object std;
} php_openssl_request_object;

zend_class_entry *php_openssl_request_ce;

static inline php_openssl_request_object *php_openssl_request_from_obj(zend_object *obj) {
	return (php_openssl_request_object *)((char *)(obj) - XtOffsetOf(php_openssl_request_object, std));
}

#define Z_OPENSSL_REQUEST_P(zv) php_openssl_request_from_obj(Z_OBJ_P(zv))

static zend_object_handlers php_openssl_request_object_handlers;

static zend_object *php_openssl_request_create_object(zend_class_entry *class_type) {
	php_openssl_request_object *intern = zend_object_alloc(sizeof(php_openssl_request_object), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &php_openssl_request_object_handlers;

	return &intern->std;
}

static zend_function *php_openssl_request_get_constructor(zend_object *object) {
	zend_throw_error(NULL, "Cannot directly construct OpenSSLCertificateSigningRequest, use openssl_csr_new() instead");
	return NULL;
}

static void php_openssl_request_free_obj(zend_object *object)
{
	php_openssl_request_object *x509_request = php_openssl_request_from_obj(object);

	X509_REQ_free(x509_request->csr);
	zend_object_std_dtor(&x509_request->std);
}

/* OpenSSLAsymmetricKey class */

typedef struct _php_openssl_pkey_object {
	EVP_PKEY *pkey;
	zend_object std;
} php_openssl_pkey_object;

zend_class_entry *php_openssl_pkey_ce;

static inline php_openssl_pkey_object *php_openssl_pkey_from_obj(zend_object *obj) {
	return (php_openssl_pkey_object *)((char *)(obj) - XtOffsetOf(php_openssl_pkey_object, std));
}

#define Z_OPENSSL_PKEY_P(zv) php_openssl_pkey_from_obj(Z_OBJ_P(zv))

static zend_object_handlers php_openssl_pkey_object_handlers;

static zend_object *php_openssl_pkey_create_object(zend_class_entry *class_type) {
	php_openssl_pkey_object *intern = zend_object_alloc(sizeof(php_openssl_pkey_object), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &php_openssl_pkey_object_handlers;

	return &intern->std;
}

static zend_function *php_openssl_pkey_get_constructor(zend_object *object) {
	zend_throw_error(NULL, "Cannot directly construct OpenSSLAsymmetricKey, use openssl_pkey_new() instead");
	return NULL;
}

static void php_openssl_pkey_free_obj(zend_object *object)
{
	php_openssl_pkey_object *key_object = php_openssl_pkey_from_obj(object);

	EVP_PKEY *pkey = key_object->pkey;
	assert(pkey != NULL);
	EVP_PKEY_free(pkey);

	zend_object_std_dtor(&key_object->std);
}

/* {{{ openssl_module_entry */
zend_module_entry openssl_module_entry = {
	STANDARD_MODULE_HEADER,
	"openssl",
	ext_functions,
	PHP_MINIT(openssl),
	PHP_MSHUTDOWN(openssl),
	NULL,
	NULL,
	PHP_MINFO(openssl),
	PHP_OPENSSL_VERSION,
	PHP_MODULE_GLOBALS(openssl),
	PHP_GINIT(openssl),
	PHP_GSHUTDOWN(openssl),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_OPENSSL
ZEND_GET_MODULE(openssl)
#endif

/* {{{ OpenSSL compatibility functions and macros */
#if PHP_OPENSSL_API_VERSION < 0x10100

#define EVP_PKEY_get0_RSA(_pkey) _pkey->pkey.rsa
#define EVP_PKEY_get0_DH(_pkey) _pkey->pkey.dh
#define EVP_PKEY_get0_DSA(_pkey) _pkey->pkey.dsa
#define EVP_PKEY_get0_EC_KEY(_pkey) _pkey->pkey.ec

static int RSA_set0_key(RSA *r, BIGNUM *n, BIGNUM *e, BIGNUM *d)
{
	r->n = n;
	r->e = e;
	r->d = d;

	return 1;
}

static int RSA_set0_factors(RSA *r, BIGNUM *p, BIGNUM *q)
{
	r->p = p;
	r->q = q;

	return 1;
}

static int RSA_set0_crt_params(RSA *r, BIGNUM *dmp1, BIGNUM *dmq1, BIGNUM *iqmp)
{
	r->dmp1 = dmp1;
	r->dmq1 = dmq1;
	r->iqmp = iqmp;

	return 1;
}

static void RSA_get0_key(const RSA *r, const BIGNUM **n, const BIGNUM **e, const BIGNUM **d)
{
	*n = r->n;
	*e = r->e;
	*d = r->d;
}

static void RSA_get0_factors(const RSA *r, const BIGNUM **p, const BIGNUM **q)
{
	*p = r->p;
	*q = r->q;
}

static void RSA_get0_crt_params(const RSA *r, const BIGNUM **dmp1, const BIGNUM **dmq1, const BIGNUM **iqmp)
{
	*dmp1 = r->dmp1;
	*dmq1 = r->dmq1;
	*iqmp = r->iqmp;
}

static void DH_get0_pqg(const DH *dh, const BIGNUM **p, const BIGNUM **q, const BIGNUM **g)
{
	*p = dh->p;
	*q = dh->q;
	*g = dh->g;
}

static int DH_set0_pqg(DH *dh, BIGNUM *p, BIGNUM *q, BIGNUM *g)
{
	dh->p = p;
	dh->q = q;
	dh->g = g;

	return 1;
}

static void DH_get0_key(const DH *dh, const BIGNUM **pub_key, const BIGNUM **priv_key)
{
	*pub_key = dh->pub_key;
	*priv_key = dh->priv_key;
}

static int DH_set0_key(DH *dh, BIGNUM *pub_key, BIGNUM *priv_key)
{
	dh->pub_key = pub_key;
	dh->priv_key = priv_key;

	return 1;
}

static void DSA_get0_pqg(const DSA *d, const BIGNUM **p, const BIGNUM **q, const BIGNUM **g)
{
	*p = d->p;
	*q = d->q;
	*g = d->g;
}

int DSA_set0_pqg(DSA *d, BIGNUM *p, BIGNUM *q, BIGNUM *g)
{
	d->p = p;
	d->q = q;
	d->g = g;

	return 1;
}

static void DSA_get0_key(const DSA *d, const BIGNUM **pub_key, const BIGNUM **priv_key)
{
	*pub_key = d->pub_key;
	*priv_key = d->priv_key;
}

int DSA_set0_key(DSA *d, BIGNUM *pub_key, BIGNUM *priv_key)
{
	d->pub_key = pub_key;
	d->priv_key = priv_key;

	return 1;
}

static const unsigned char *ASN1_STRING_get0_data(const ASN1_STRING *asn1)
{
	return M_ASN1_STRING_data(asn1);
}

static int EVP_PKEY_up_ref(EVP_PKEY *pkey)
{
	return CRYPTO_add(&pkey->references, 1, CRYPTO_LOCK_EVP_PKEY);
}

#if PHP_OPENSSL_API_VERSION < 0x10002

static int X509_get_signature_nid(const X509 *x)
{
	return OBJ_obj2nid(x->sig_alg->algorithm);
}

#endif

#define OpenSSL_version		SSLeay_version
#define OPENSSL_VERSION		SSLEAY_VERSION
#define X509_getm_notBefore	X509_get_notBefore
#define X509_getm_notAfter	X509_get_notAfter
#define EVP_CIPHER_CTX_reset	EVP_CIPHER_CTX_cleanup

#endif
/* }}} */

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

/* {{{ php_openssl_store_errors */
void php_openssl_store_errors()
{
	struct php_openssl_errors *errors;
	int error_code = ERR_get_error();

	if (!error_code) {
		return;
	}

	if (!OPENSSL_G(errors)) {
		OPENSSL_G(errors) = pecalloc(1, sizeof(struct php_openssl_errors), 1);
	}

	errors = OPENSSL_G(errors);

	do {
		errors->top = (errors->top + 1) % ERR_NUM_ERRORS;
		if (errors->top == errors->bottom) {
			errors->bottom = (errors->bottom + 1) % ERR_NUM_ERRORS;
		}
		errors->buffer[errors->top] = error_code;
	} while ((error_code = ERR_get_error()));

}
/* }}} */

static int ssl_stream_data_index;

/* }}} */

/* {{{ openssl open_basedir check */
inline static int php_openssl_open_base_dir_chk(char *filename)
{
	if (php_check_open_basedir(filename)) {
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
	LHASH_OF(CONF_VALUE) * global_config;	/* Global SSL config */
	LHASH_OF(CONF_VALUE) * req_config;		/* SSL config for this request */
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
/* }}} */

static X509 *php_openssl_x509_from_param(zend_object *cert_obj, zend_string *cert_str);
static X509 *php_openssl_x509_from_zval(zval *val, bool *free_cert);
static X509_REQ *php_openssl_csr_from_param(zend_object *csr_obj, zend_string *csr_str);
static EVP_PKEY *php_openssl_pkey_from_zval(zval *val, int public_key, char *passphrase, size_t passphrase_len);

static int php_openssl_is_private_key(EVP_PKEY* pkey);
static X509_STORE * php_openssl_setup_verify(zval * calist);
static STACK_OF(X509) * php_openssl_load_all_certs_from_file(char *certfile);
static EVP_PKEY * php_openssl_generate_private_key(struct php_x509_request * req);

static void php_openssl_add_assoc_name_entry(zval * val, char * key, X509_NAME * name, int shortname) /* {{{ */
{
	zval *data;
	zval subitem, tmp;
	int i;
	char *sname;
	int nid;
	X509_NAME_ENTRY * ne;
	ASN1_STRING * str = NULL;
	ASN1_OBJECT * obj;

	if (key != NULL) {
		array_init(&subitem);
	} else {
		ZVAL_COPY_VALUE(&subitem, val);
	}

	for (i = 0; i < X509_NAME_entry_count(name); i++) {
		const unsigned char *to_add = NULL;
		int to_add_len = 0;
		unsigned char *to_add_buf = NULL;

		ne = X509_NAME_get_entry(name, i);
		obj = X509_NAME_ENTRY_get_object(ne);
		nid = OBJ_obj2nid(obj);

		if (shortname) {
			sname = (char *) OBJ_nid2sn(nid);
		} else {
			sname = (char *) OBJ_nid2ln(nid);
		}

		str = X509_NAME_ENTRY_get_data(ne);
		if (ASN1_STRING_type(str) != V_ASN1_UTF8STRING) {
			/* ASN1_STRING_to_UTF8(3): The converted data is copied into a newly allocated buffer */
			to_add_len = ASN1_STRING_to_UTF8(&to_add_buf, str);
			to_add = to_add_buf;
		} else {
			/* ASN1_STRING_get0_data(3): Since this is an internal pointer it should not be freed or modified in any way */
			to_add = ASN1_STRING_get0_data(str);
			to_add_len = ASN1_STRING_length(str);
		}

		if (to_add_len != -1) {
			if ((data = zend_hash_str_find(Z_ARRVAL(subitem), sname, strlen(sname))) != NULL) {
				if (Z_TYPE_P(data) == IS_ARRAY) {
					add_next_index_stringl(data, (const char *)to_add, to_add_len);
				} else if (Z_TYPE_P(data) == IS_STRING) {
					array_init(&tmp);
					add_next_index_str(&tmp, zend_string_copy(Z_STR_P(data)));
					add_next_index_stringl(&tmp, (const char *)to_add, to_add_len);
					zend_hash_str_update(Z_ARRVAL(subitem), sname, strlen(sname), &tmp);
				}
			} else {
				/* it might be better to expand it and pass zval from ZVAL_STRING
				 * to zend_symtable_str_update so we do not silently drop const
				 * but we need a test to cover this part first */
				add_assoc_stringl(&subitem, sname, (char *)to_add, to_add_len);
			}
		} else {
			php_openssl_store_errors();
		}

		if (to_add_buf != NULL) {
			OPENSSL_free(to_add_buf);
		}
	}

	if (key != NULL) {
		zend_hash_str_update(Z_ARRVAL_P(val), key, strlen(key), &subitem);
	}
}
/* }}} */

static void php_openssl_add_assoc_asn1_string(zval * val, char * key, ASN1_STRING * str) /* {{{ */
{
	add_assoc_stringl(val, key, (char *)str->data, str->length);
}
/* }}} */

static time_t php_openssl_asn1_time_to_time_t(ASN1_UTCTIME * timestr) /* {{{ */
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
	size_t timestr_len;

	if (ASN1_STRING_type(timestr) != V_ASN1_UTCTIME && ASN1_STRING_type(timestr) != V_ASN1_GENERALIZEDTIME) {
		php_error_docref(NULL, E_WARNING, "Illegal ASN1 data type for timestamp");
		return (time_t)-1;
	}

	timestr_len = (size_t)ASN1_STRING_length(timestr);

	if (timestr_len != strlen((const char *)ASN1_STRING_get0_data(timestr))) {
		php_error_docref(NULL, E_WARNING, "Illegal length in timestamp");
		return (time_t)-1;
	}

	if (timestr_len < 13 && timestr_len != 11) {
		php_error_docref(NULL, E_WARNING, "Unable to parse time string %s correctly", timestr->data);
		return (time_t)-1;
	}

	if (ASN1_STRING_type(timestr) == V_ASN1_GENERALIZEDTIME && timestr_len < 15) {
		php_error_docref(NULL, E_WARNING, "Unable to parse time string %s correctly", timestr->data);
		return (time_t)-1;
	}

	strbuf = estrdup((const char *)ASN1_STRING_get0_data(timestr));

	memset(&thetime, 0, sizeof(thetime));

	/* we work backwards so that we can use atoi more easily */

	thestr = strbuf + timestr_len - 3;

	if (timestr_len == 11) {
		thetime.tm_sec = 0;
	} else {
		thetime.tm_sec = atoi(thestr);
		*thestr = '\0';
		thestr -= 2;
	}
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

#ifdef HAVE_STRUCT_TM_TM_GMTOFF
	gmadjust = thetime.tm_gmtoff;
#else
	/*
	** If correcting for daylight savings time, we set the adjustment to
	** the value of timezone - 3600 seconds. Otherwise, we need to overcorrect and
	** set the adjustment to the main timezone + 3600 seconds.
	*/
	gmadjust = -(thetime.tm_isdst ? (long)timezone - 3600 : (long)timezone);
#endif
	ret += gmadjust;

	efree(strbuf);

	return ret;
}
/* }}} */

static inline int php_openssl_config_check_syntax(const char * section_label, const char * config_filename, const char * section, LHASH_OF(CONF_VALUE) * config) /* {{{ */
{
	X509V3_CTX ctx;

	X509V3_set_ctx_test(&ctx);
	X509V3_set_conf_lhash(&ctx, config);
	if (!X509V3_EXT_add_conf(config, &ctx, (char *)section, NULL)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error loading %s section %s of %s",
				section_label,
				section,
				config_filename);
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

static int php_openssl_add_oid_section(struct php_x509_request * req) /* {{{ */
{
	char * str;
	STACK_OF(CONF_VALUE) * sktmp;
	CONF_VALUE * cnf;
	int i;

	str = CONF_get_string(req->req_config, NULL, "oid_section");
	if (str == NULL) {
		php_openssl_store_errors();
		return SUCCESS;
	}
	sktmp = CONF_get_section(req->req_config, str);
	if (sktmp == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Problem loading oid section %s", str);
		return FAILURE;
	}
	for (i = 0; i < sk_CONF_VALUE_num(sktmp); i++) {
		cnf = sk_CONF_VALUE_value(sktmp, i);
		if (OBJ_sn2nid(cnf->name) == NID_undef && OBJ_ln2nid(cnf->name) == NID_undef &&
				OBJ_create(cnf->value, cnf->name, cnf->name) == NID_undef) {
			php_openssl_store_errors();
			php_error_docref(NULL, E_WARNING, "Problem creating object %s=%s", cnf->name, cnf->value);
			return FAILURE;
		}
	}
	return SUCCESS;
}
/* }}} */

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

static const EVP_CIPHER * php_openssl_get_evp_cipher_from_algo(zend_long algo);

/* {{{ strip line endings from spkac */
static int php_openssl_spki_cleanup(const char *src, char *dest)
{
	int removed = 0;

	while (*src) {
		if (*src != '\n' && *src != '\r') {
			*dest++ = *src;
		} else {
			++removed;
		}
		++src;
	}
	*dest = 0;
	return removed;
}
/* }}} */

static int php_openssl_parse_config(struct php_x509_request * req, zval * optional_args) /* {{{ */
{
	char * str;
	zval * item;

	SET_OPTIONAL_STRING_ARG("config", req->config_filename, default_ssl_conf_filename);
	SET_OPTIONAL_STRING_ARG("config_section_name", req->section_name, "req");
	req->global_config = CONF_load(NULL, default_ssl_conf_filename, NULL);
	if (req->global_config == NULL) {
		php_openssl_store_errors();
	}
	req->req_config = CONF_load(NULL, req->config_filename, NULL);
	if (req->req_config == NULL) {
		php_openssl_store_errors();
		return FAILURE;
	}

	/* read in the oids */
	str = CONF_get_string(req->req_config, NULL, "oid_file");
	if (str == NULL) {
		php_openssl_store_errors();
	} else if (!php_openssl_open_base_dir_chk(str)) {
		BIO *oid_bio = BIO_new_file(str, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
		if (oid_bio) {
			OBJ_create_objects(oid_bio);
			BIO_free(oid_bio);
			php_openssl_store_errors();
		}
	}
	if (php_openssl_add_oid_section(req) == FAILURE) {
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

	if (optional_args && (item = zend_hash_str_find(Z_ARRVAL_P(optional_args), "encrypt_key", sizeof("encrypt_key")-1)) != NULL) {
		req->priv_key_encrypt = Z_TYPE_P(item) == IS_TRUE ? 1 : 0;
	} else {
		str = CONF_get_string(req->req_config, req->section_name, "encrypt_rsa_key");
		if (str == NULL) {
			str = CONF_get_string(req->req_config, req->section_name, "encrypt_key");
			/* it is sure that there are some errors as str was NULL for encrypt_rsa_key */
			php_openssl_store_errors();
		}
		if (str != NULL && strcmp(str, "no") == 0) {
			req->priv_key_encrypt = 0;
		} else {
			req->priv_key_encrypt = 1;
		}
	}

	if (req->priv_key_encrypt &&
		optional_args &&
		(item = zend_hash_str_find(Z_ARRVAL_P(optional_args), "encrypt_key_cipher", sizeof("encrypt_key_cipher")-1)) != NULL &&
		Z_TYPE_P(item) == IS_LONG
	) {
		zend_long cipher_algo = Z_LVAL_P(item);
		const EVP_CIPHER* cipher = php_openssl_get_evp_cipher_from_algo(cipher_algo);
		if (cipher == NULL) {
			php_error_docref(NULL, E_WARNING, "Unknown cipher algorithm for private key");
			return FAILURE;
		} else {
			req->priv_key_encrypt_cipher = cipher;
		}
	} else {
		req->priv_key_encrypt_cipher = NULL;
	}

	/* digest alg */
	if (req->digest_name == NULL) {
		req->digest_name = CONF_get_string(req->req_config, req->section_name, "default_md");
	}
	if (req->digest_name != NULL) {
		req->digest = req->md_alg = EVP_get_digestbyname(req->digest_name);
	} else {
		php_openssl_store_errors();
	}
	if (req->md_alg == NULL) {
		req->md_alg = req->digest = EVP_sha1();
		php_openssl_store_errors();
	}

	PHP_SSL_CONFIG_SYNTAX_CHECK(extensions_section);
#ifdef HAVE_EVP_PKEY_EC
	/* set the ec group curve name */
	req->curve_name = NID_undef;
	if (optional_args && (item = zend_hash_str_find(Z_ARRVAL_P(optional_args), "curve_name", sizeof("curve_name")-1)) != NULL
		&& Z_TYPE_P(item) == IS_STRING) {
		req->curve_name = OBJ_sn2nid(Z_STRVAL_P(item));
		if (req->curve_name == NID_undef) {
			php_error_docref(NULL, E_WARNING, "Unknown elliptic curve (short) name %s", Z_STRVAL_P(item));
			return FAILURE;
		}
	}
#endif

	/* set the string mask */
	str = CONF_get_string(req->req_config, req->section_name, "string_mask");
	if (str == NULL) {
		php_openssl_store_errors();
	} else if (!ASN1_STRING_set_default_mask_asc(str)) {
		php_error_docref(NULL, E_WARNING, "Invalid global string mask setting %s", str);
		return FAILURE;
	}

	PHP_SSL_CONFIG_SYNTAX_CHECK(request_extensions_section);

	return SUCCESS;
}
/* }}} */

static void php_openssl_dispose_config(struct php_x509_request * req) /* {{{ */
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

#if defined(PHP_WIN32) || PHP_OPENSSL_API_VERSION >= 0x10100
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

static int php_openssl_load_rand_file(const char * file, int *egdsocket, int *seeded) /* {{{ */
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
			php_openssl_store_errors();
			php_error_docref(NULL, E_WARNING, "Unable to load random state; not enough random data!");
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
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to write random state");
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

static EVP_MD * php_openssl_get_evp_md_from_algo(zend_long algo) { /* {{{ */
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
#if PHP_OPENSSL_API_VERSION < 0x10100
		case OPENSSL_ALGO_DSS1:
			mdtype = (EVP_MD *) EVP_dss1();
			break;
#endif
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
		default:
			return NULL;
			break;
	}
	return mdtype;
}
/* }}} */

static const EVP_CIPHER * php_openssl_get_evp_cipher_from_algo(zend_long algo) { /* {{{ */
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

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(openssl)
{
	char * config_filename;

	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "OpenSSLCertificate", class_OpenSSLCertificate_methods);
	php_openssl_certificate_ce = zend_register_internal_class(&ce);
	php_openssl_certificate_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	php_openssl_certificate_ce->create_object = php_openssl_certificate_create_object;
	php_openssl_certificate_ce->serialize = zend_class_serialize_deny;
	php_openssl_certificate_ce->unserialize = zend_class_unserialize_deny;

	memcpy(&php_openssl_certificate_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_openssl_certificate_object_handlers.offset = XtOffsetOf(php_openssl_certificate_object, std);
	php_openssl_certificate_object_handlers.free_obj = php_openssl_certificate_free_obj;
	php_openssl_certificate_object_handlers.get_constructor = php_openssl_certificate_get_constructor;
	php_openssl_certificate_object_handlers.clone_obj = NULL;

	zend_class_entry csr_ce;
	INIT_CLASS_ENTRY(csr_ce, "OpenSSLCertificateSigningRequest", class_OpenSSLCertificateSigningRequest_methods);
	php_openssl_request_ce = zend_register_internal_class(&csr_ce);
	php_openssl_request_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	php_openssl_request_ce->create_object = php_openssl_request_create_object;
	php_openssl_request_ce->serialize = zend_class_serialize_deny;
	php_openssl_request_ce->unserialize = zend_class_unserialize_deny;

	memcpy(&php_openssl_request_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_openssl_request_object_handlers.offset = XtOffsetOf(php_openssl_request_object, std);
	php_openssl_request_object_handlers.free_obj = php_openssl_request_free_obj;
	php_openssl_request_object_handlers.get_constructor = php_openssl_request_get_constructor;
	php_openssl_request_object_handlers.clone_obj = NULL;

	zend_class_entry key_ce;
	INIT_CLASS_ENTRY(key_ce, "OpenSSLAsymmetricKey", class_OpenSSLAsymmetricKey_methods);
	php_openssl_pkey_ce = zend_register_internal_class(&key_ce);
	php_openssl_pkey_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	php_openssl_pkey_ce->create_object = php_openssl_pkey_create_object;
	php_openssl_pkey_ce->serialize = zend_class_serialize_deny;
	php_openssl_pkey_ce->unserialize = zend_class_unserialize_deny;

	memcpy(&php_openssl_pkey_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_openssl_pkey_object_handlers.offset = XtOffsetOf(php_openssl_pkey_object, std);
	php_openssl_pkey_object_handlers.free_obj = php_openssl_pkey_free_obj;
	php_openssl_pkey_object_handlers.get_constructor = php_openssl_pkey_get_constructor;
	php_openssl_pkey_object_handlers.clone_obj = NULL;

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined (LIBRESSL_VERSION_NUMBER)
	OPENSSL_config(NULL);
	SSL_library_init();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
	OpenSSL_add_all_algorithms();

#if !defined(OPENSSL_NO_AES) && defined(EVP_CIPH_CCM_MODE) && OPENSSL_VERSION_NUMBER < 0x100020000
	EVP_add_cipher(EVP_aes_128_ccm());
	EVP_add_cipher(EVP_aes_192_ccm());
	EVP_add_cipher(EVP_aes_256_ccm());
#endif

	SSL_load_error_strings();
#else
	OPENSSL_init_ssl(OPENSSL_INIT_LOAD_CONFIG, NULL);
#endif

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

	/* digest algorithm constants */
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_SHA1", OPENSSL_ALGO_SHA1, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_MD5", OPENSSL_ALGO_MD5, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_MD4", OPENSSL_ALGO_MD4, CONST_CS|CONST_PERSISTENT);
#ifdef HAVE_OPENSSL_MD2_H
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_MD2", OPENSSL_ALGO_MD2, CONST_CS|CONST_PERSISTENT);
#endif
#if PHP_OPENSSL_API_VERSION < 0x10100
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_DSS1", OPENSSL_ALGO_DSS1, CONST_CS|CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_SHA224", OPENSSL_ALGO_SHA224, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_SHA256", OPENSSL_ALGO_SHA256, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_SHA384", OPENSSL_ALGO_SHA384, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_SHA512", OPENSSL_ALGO_SHA512, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ALGO_RMD160", OPENSSL_ALGO_RMD160, CONST_CS|CONST_PERSISTENT);

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

	REGISTER_LONG_CONSTANT("OPENSSL_CMS_DETACHED", CMS_DETACHED, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_TEXT", CMS_TEXT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_NOINTERN", CMS_NOINTERN, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_NOVERIFY", CMS_NOVERIFY, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_NOCERTS", CMS_NOCERTS, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_NOATTR", CMS_NOATTR, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_BINARY", CMS_BINARY, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_CMS_NOSIGS", CMS_NOSIGS, CONST_CS|CONST_PERSISTENT);

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
	REGISTER_LONG_CONSTANT("OPENSSL_DONT_ZERO_PAD_KEY", OPENSSL_DONT_ZERO_PAD_KEY, CONST_CS|CONST_PERSISTENT);

#ifndef OPENSSL_NO_TLSEXT
	/* SNI support included */
	REGISTER_LONG_CONSTANT("OPENSSL_TLSEXT_SERVER_NAME", 1, CONST_CS|CONST_PERSISTENT);
#endif

	/* Register encodings */
	REGISTER_LONG_CONSTANT("OPENSSL_ENCODING_DER",ENCODING_DER,CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ENCODING_SMIME",ENCODING_SMIME,CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("OPENSSL_ENCODING_PEM",ENCODING_PEM,CONST_CS|CONST_PERSISTENT);

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

	php_stream_xport_register("ssl", php_openssl_ssl_socket_factory);
#ifndef OPENSSL_NO_SSL3
	php_stream_xport_register("sslv3", php_openssl_ssl_socket_factory);
#endif
	php_stream_xport_register("tls", php_openssl_ssl_socket_factory);
	php_stream_xport_register("tlsv1.0", php_openssl_ssl_socket_factory);
	php_stream_xport_register("tlsv1.1", php_openssl_ssl_socket_factory);
	php_stream_xport_register("tlsv1.2", php_openssl_ssl_socket_factory);
#if OPENSSL_VERSION_NUMBER >= 0x10101000
	php_stream_xport_register("tlsv1.3", php_openssl_ssl_socket_factory);
#endif

	/* override the default tcp socket provider */
	php_stream_xport_register("tcp", php_openssl_ssl_socket_factory);

	php_register_url_stream_wrapper("https", &php_stream_http_wrapper);
	php_register_url_stream_wrapper("ftps", &php_stream_ftp_wrapper);

	REGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
PHP_GINIT_FUNCTION(openssl)
{
#if defined(COMPILE_DL_OPENSSL) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	openssl_globals->errors = NULL;
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION */
PHP_GSHUTDOWN_FUNCTION(openssl)
{
	if (openssl_globals->errors) {
		pefree(openssl_globals->errors, 1);
	}
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(openssl)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "OpenSSL support", "enabled");
	php_info_print_table_row(2, "OpenSSL Library Version", OpenSSL_version(OPENSSL_VERSION));
	php_info_print_table_row(2, "OpenSSL Header Version", OPENSSL_VERSION_TEXT);
	php_info_print_table_row(2, "Openssl default config", default_ssl_conf_filename);
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(openssl)
{
#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined (LIBRESSL_VERSION_NUMBER)
	EVP_cleanup();

	/* prevent accessing locking callback from unloaded extension */
	CRYPTO_set_locking_callback(NULL);
	/* free allocated error strings */
	ERR_free_strings();
	CONF_modules_free();
#endif

	php_unregister_url_stream_wrapper("https");
	php_unregister_url_stream_wrapper("ftps");

	php_stream_xport_unregister("ssl");
#ifndef OPENSSL_NO_SSL3
	php_stream_xport_unregister("sslv3");
#endif
	php_stream_xport_unregister("tls");
	php_stream_xport_unregister("tlsv1.0");
	php_stream_xport_unregister("tlsv1.1");
	php_stream_xport_unregister("tlsv1.2");
#if OPENSSL_VERSION_NUMBER >= 0x10101000
	php_stream_xport_unregister("tlsv1.3");
#endif

	/* reinstate the default tcp handler */
	php_stream_xport_register("tcp", php_stream_generic_socket_factory);

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ x509 cert functions */

/* {{{ Retrieve an array mapping available certificate locations */
PHP_FUNCTION(openssl_get_cert_locations)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);

	add_assoc_string(return_value, "default_cert_file", (char *) X509_get_default_cert_file());
	add_assoc_string(return_value, "default_cert_file_env", (char *) X509_get_default_cert_file_env());
	add_assoc_string(return_value, "default_cert_dir", (char *) X509_get_default_cert_dir());
	add_assoc_string(return_value, "default_cert_dir_env", (char *) X509_get_default_cert_dir_env());
	add_assoc_string(return_value, "default_private_dir", (char *) X509_get_default_private_dir());
	add_assoc_string(return_value, "default_default_cert_area", (char *) X509_get_default_cert_area());
	add_assoc_string(return_value, "ini_cafile",
		zend_ini_string("openssl.cafile", sizeof("openssl.cafile")-1, 0));
	add_assoc_string(return_value, "ini_capath",
		zend_ini_string("openssl.capath", sizeof("openssl.capath")-1, 0));
}
/* }}} */

static X509 *php_openssl_x509_from_str(zend_string *cert_str) {
	X509 *cert = NULL;
	BIO *in;

	if (ZSTR_LEN(cert_str) > 7 && memcmp(ZSTR_VAL(cert_str), "file://", sizeof("file://") - 1) == 0) {
		if (php_openssl_open_base_dir_chk(ZSTR_VAL(cert_str) + (sizeof("file://") - 1))) {
			return NULL;
		}

		in = BIO_new_file(ZSTR_VAL(cert_str) + (sizeof("file://") - 1), PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
		if (in == NULL) {
			php_openssl_store_errors();
			return NULL;
		}
		cert = PEM_read_bio_X509(in, NULL, NULL, NULL);
	} else {
		in = BIO_new_mem_buf(ZSTR_VAL(cert_str), (int) ZSTR_LEN(cert_str));
		if (in == NULL) {
			php_openssl_store_errors();
			return NULL;
		}
#ifdef TYPEDEF_D2I_OF
		cert = (X509 *) PEM_ASN1_read_bio((d2i_of_void *)d2i_X509, PEM_STRING_X509, in, NULL, NULL, NULL);
#else
		cert = (X509 *) PEM_ASN1_read_bio((char *(*)())d2i_X509, PEM_STRING_X509, in, NULL, NULL, NULL);
#endif
	}

	if (!BIO_free(in)) {
		php_openssl_store_errors();
	}

	if (cert == NULL) {
		php_openssl_store_errors();
		return NULL;
	}

	return cert;
}

/* {{{ php_openssl_x509_from_param
	Given a parameter, extract it into an X509 object.
	The parameter can be:
		. X509 object created using openssl_read_x509()
		. a path to that cert if it starts with file://
		. the cert data otherwise
*/
static X509 *php_openssl_x509_from_param(zend_object *cert_obj, zend_string *cert_str) {
	if (cert_obj) {
		return php_openssl_certificate_from_obj(cert_obj)->x509;
	}

	ZEND_ASSERT(cert_str);

	return php_openssl_x509_from_str(cert_str);
}
/* }}} */

static X509 *php_openssl_x509_from_zval(zval *val, bool *free_cert)
{
	if (Z_TYPE_P(val) == IS_OBJECT && Z_OBJCE_P(val) == php_openssl_certificate_ce) {
		*free_cert = 0;

		return php_openssl_certificate_from_obj(Z_OBJ_P(val))->x509;
	}

	*free_cert = 1;

	if (!try_convert_to_string(val)) {
		return NULL;
	}

	return php_openssl_x509_from_str(Z_STR_P(val));
}
/* }}} */

/* {{{ Exports a CERT to file or a var */
PHP_FUNCTION(openssl_x509_export_to_file)
{
	X509 *cert;
	zend_object *cert_obj;
	zend_string *cert_str;

	zend_bool notext = 1;
	BIO * bio_out;
	char * filename;
	size_t filename_len;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(notext)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	cert = php_openssl_x509_from_param(cert_obj, cert_str);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		return;
	}

	if (php_openssl_open_base_dir_chk(filename)) {
		return;
	}

	bio_out = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
	if (bio_out) {
		if (!notext && !X509_print(bio_out, cert)) {
			php_openssl_store_errors();
		}
		if (!PEM_write_bio_X509(bio_out, cert)) {
			php_openssl_store_errors();
		}

		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error opening file %s", filename);
	}

	if (cert_str) {
		X509_free(cert);
	}

	if (!BIO_free(bio_out)) {
		php_openssl_store_errors();
	}
}
/* }}} */

/* {{{ Creates new private key (or uses existing) and creates a new spki cert
   outputting results to var */
PHP_FUNCTION(openssl_spki_new)
{
	size_t challenge_len;
	char * challenge = NULL, *spkstr = NULL;
	zend_string * s = NULL;
	const char *spkac = "SPKAC=";
	zend_long algo = OPENSSL_ALGO_MD5;

	zval *zpkey = NULL;
	EVP_PKEY *pkey = NULL;
	NETSCAPE_SPKI *spki=NULL;
	const EVP_MD *mdtype;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Os|l", &zpkey, php_openssl_pkey_ce, &challenge, &challenge_len, &algo) == FAILURE) {
		RETURN_THROWS();
	}
	RETVAL_FALSE;

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(challenge_len, challenge, 2);

	pkey = php_openssl_pkey_from_zval(zpkey, 0, challenge, challenge_len);
	if (pkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Unable to use supplied private key");
		}
		goto cleanup;
	}

	mdtype = php_openssl_get_evp_md_from_algo(algo);

	if (!mdtype) {
		php_error_docref(NULL, E_WARNING, "Unknown digest algorithm");
		goto cleanup;
	}

	if ((spki = NETSCAPE_SPKI_new()) == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to create new SPKAC");
		goto cleanup;
	}

	if (challenge) {
		if (!ASN1_STRING_set(spki->spkac->challenge, challenge, (int)challenge_len)) {
			php_openssl_store_errors();
			php_error_docref(NULL, E_WARNING, "Unable to set challenge data");
			goto cleanup;
		}
	}

	if (!NETSCAPE_SPKI_set_pubkey(spki, pkey)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to embed public key");
		goto cleanup;
	}

	if (!NETSCAPE_SPKI_sign(spki, pkey, mdtype)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to sign with specified digest algorithm");
		goto cleanup;
	}

	spkstr = NETSCAPE_SPKI_b64_encode(spki);
	if (!spkstr){
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to encode SPKAC");
		goto cleanup;
	}

	s = zend_string_alloc(strlen(spkac) + strlen(spkstr), 0);
	sprintf(ZSTR_VAL(s), "%s%s", spkac, spkstr);
	ZSTR_LEN(s) = strlen(ZSTR_VAL(s));
	OPENSSL_free(spkstr);

	RETVAL_STR(s);
	goto cleanup;

cleanup:
	EVP_PKEY_free(pkey);
	if (spki != NULL) {
		NETSCAPE_SPKI_free(spki);
	}

	if (s && ZSTR_LEN(s) <= 0) {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ Verifies spki returns boolean */
PHP_FUNCTION(openssl_spki_verify)
{
	size_t spkstr_len;
	int i = 0, spkstr_cleaned_len = 0;
	char *spkstr, * spkstr_cleaned = NULL;

	EVP_PKEY *pkey = NULL;
	NETSCAPE_SPKI *spki = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &spkstr, &spkstr_len) == FAILURE) {
		RETURN_THROWS();
	}
	RETVAL_FALSE;

	spkstr_cleaned = emalloc(spkstr_len + 1);
	spkstr_cleaned_len = (int)(spkstr_len - php_openssl_spki_cleanup(spkstr, spkstr_cleaned));

	if (spkstr_cleaned_len == 0) {
		php_error_docref(NULL, E_WARNING, "Invalid SPKAC");
		goto cleanup;
	}

	spki = NETSCAPE_SPKI_b64_decode(spkstr_cleaned, spkstr_cleaned_len);
	if (spki == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to decode supplied SPKAC");
		goto cleanup;
	}

	pkey = X509_PUBKEY_get(spki->spkac->pubkey);
	if (pkey == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to acquire signed public key");
		goto cleanup;
	}

	i = NETSCAPE_SPKI_verify(spki, pkey);
	goto cleanup;

cleanup:
	if (spki != NULL) {
		NETSCAPE_SPKI_free(spki);
	}
	EVP_PKEY_free(pkey);
	if (spkstr_cleaned != NULL) {
		efree(spkstr_cleaned);
	}

	if (i > 0) {
		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
	}
}
/* }}} */

/* {{{ Exports public key from existing spki to var */
PHP_FUNCTION(openssl_spki_export)
{
	size_t spkstr_len;
	char *spkstr, * spkstr_cleaned = NULL, * s = NULL;
	int spkstr_cleaned_len;

	EVP_PKEY *pkey = NULL;
	NETSCAPE_SPKI *spki = NULL;
	BIO *out = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &spkstr, &spkstr_len) == FAILURE) {
		RETURN_THROWS();
	}
	RETVAL_FALSE;

	spkstr_cleaned = emalloc(spkstr_len + 1);
	spkstr_cleaned_len = (int)(spkstr_len - php_openssl_spki_cleanup(spkstr, spkstr_cleaned));

	if (spkstr_cleaned_len == 0) {
		php_error_docref(NULL, E_WARNING, "Invalid SPKAC");
		goto cleanup;
	}

	spki = NETSCAPE_SPKI_b64_decode(spkstr_cleaned, spkstr_cleaned_len);
	if (spki == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to decode supplied SPKAC");
		goto cleanup;
	}

	pkey = X509_PUBKEY_get(spki->spkac->pubkey);
	if (pkey == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to acquire signed public key");
		goto cleanup;
	}

	out = BIO_new(BIO_s_mem());
	if (out && PEM_write_bio_PUBKEY(out, pkey)) {
		BUF_MEM *bio_buf;

		BIO_get_mem_ptr(out, &bio_buf);
		RETVAL_STRINGL((char *)bio_buf->data, bio_buf->length);
	} else {
		php_openssl_store_errors();
	}
	goto cleanup;

cleanup:

	if (spki != NULL) {
		NETSCAPE_SPKI_free(spki);
	}
	BIO_free_all(out);
	EVP_PKEY_free(pkey);
	if (spkstr_cleaned != NULL) {
		efree(spkstr_cleaned);
	}
	if (s != NULL) {
		efree(s);
	}
}
/* }}} */

/* {{{ Exports spkac challenge from existing spki to var */
PHP_FUNCTION(openssl_spki_export_challenge)
{
	size_t spkstr_len;
	char *spkstr, * spkstr_cleaned = NULL;
	int spkstr_cleaned_len;

	NETSCAPE_SPKI *spki = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &spkstr, &spkstr_len) == FAILURE) {
		RETURN_THROWS();
	}
	RETVAL_FALSE;

	spkstr_cleaned = emalloc(spkstr_len + 1);
	spkstr_cleaned_len = (int)(spkstr_len - php_openssl_spki_cleanup(spkstr, spkstr_cleaned));

	if (spkstr_cleaned_len == 0) {
		php_error_docref(NULL, E_WARNING, "Invalid SPKAC");
		goto cleanup;
	}

	spki = NETSCAPE_SPKI_b64_decode(spkstr_cleaned, spkstr_cleaned_len);
	if (spki == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to decode SPKAC");
		goto cleanup;
	}

	RETVAL_STRING((const char *)ASN1_STRING_get0_data(spki->spkac->challenge));
	goto cleanup;

cleanup:
	if (spkstr_cleaned != NULL) {
		efree(spkstr_cleaned);
	}
	if (spki) {
		NETSCAPE_SPKI_free(spki);
	}
}
/* }}} */

/* {{{ Exports a CERT to file or a var */
PHP_FUNCTION(openssl_x509_export)
{
	X509 *cert;
	zend_object *cert_obj;
	zend_string *cert_str;
	zval *zout;
	zend_bool notext = 1;
	BIO * bio_out;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_ZVAL(zout)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(notext)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	cert = php_openssl_x509_from_param(cert_obj, cert_str);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		return;
	}

	bio_out = BIO_new(BIO_s_mem());
	if (!bio_out) {
		php_openssl_store_errors();
		goto cleanup;
	}
	if (!notext && !X509_print(bio_out, cert)) {
		php_openssl_store_errors();
	}
	if (PEM_write_bio_X509(bio_out, cert)) {
		BUF_MEM *bio_buf;

		BIO_get_mem_ptr(bio_out, &bio_buf);
		ZEND_TRY_ASSIGN_REF_STRINGL(zout, bio_buf->data, bio_buf->length);

		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
	}

	BIO_free(bio_out);

cleanup:
	if (cert_str) {
		X509_free(cert);
	}
}
/* }}} */

zend_string* php_openssl_x509_fingerprint(X509 *peer, const char *method, zend_bool raw)
{
	unsigned char md[EVP_MAX_MD_SIZE];
	const EVP_MD *mdtype;
	unsigned int n;
	zend_string *ret;

	if (!(mdtype = EVP_get_digestbyname(method))) {
		php_error_docref(NULL, E_WARNING, "Unknown digest algorithm");
		return NULL;
	} else if (!X509_digest(peer, mdtype, md, &n)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_ERROR, "Could not generate signature");
		return NULL;
	}

	if (raw) {
		ret = zend_string_init((char*)md, n, 0);
	} else {
		ret = zend_string_alloc(n * 2, 0);
		make_digest_ex(ZSTR_VAL(ret), md, n);
		ZSTR_VAL(ret)[n * 2] = '\0';
	}

	return ret;
}

PHP_FUNCTION(openssl_x509_fingerprint)
{
	X509 *cert;
	zend_object *cert_obj;
	zend_string *cert_str;
	zend_bool raw_output = 0;
	char *method = "sha1";
	size_t method_len;
	zend_string *fingerprint;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(method, method_len)
		Z_PARAM_BOOL(raw_output)
	ZEND_PARSE_PARAMETERS_END();

	cert = php_openssl_x509_from_param(cert_obj, cert_str);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		RETURN_FALSE;
	}

	fingerprint = php_openssl_x509_fingerprint(cert, method, raw_output);
	if (fingerprint) {
		RETVAL_STR(fingerprint);
	} else {
		RETVAL_FALSE;
	}

	if (cert_str) {
		X509_free(cert);
	}
}

/* {{{ Checks if a private key corresponds to a CERT */
PHP_FUNCTION(openssl_x509_check_private_key)
{
	X509 *cert;
	zend_object *cert_obj;
	zend_string *cert_str;
	zval *zkey;
	EVP_PKEY * key = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_ZVAL(zkey)
	ZEND_PARSE_PARAMETERS_END();

	cert = php_openssl_x509_from_param(cert_obj, cert_str);
	if (cert == NULL) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	key = php_openssl_pkey_from_zval(zkey, 0, "", 0);
	if (key) {
		RETVAL_BOOL(X509_check_private_key(cert, key));
		EVP_PKEY_free(key);
	}

	if (cert_str) {
		X509_free(cert);
	}
}
/* }}} */

/* {{{ Verifies the signature of certificate cert using public key key */
PHP_FUNCTION(openssl_x509_verify)
{
	X509 *cert;
	zend_object *cert_obj;
	zend_string *cert_str;
	zval *zkey;
	EVP_PKEY * key = NULL;
	int err = -1;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_ZVAL(zkey)
	ZEND_PARSE_PARAMETERS_END();

	cert = php_openssl_x509_from_param(cert_obj, cert_str);
	if (cert == NULL) {
		RETURN_LONG(err);
	}

	key = php_openssl_pkey_from_zval(zkey, 1, NULL, 0);
	if (key != NULL) {
		err = X509_verify(cert, key);
		if (err < 0) {
			php_openssl_store_errors();
		}

		EVP_PKEY_free(key);
	}

	if (cert_str) {
		X509_free(cert);
	}

	RETURN_LONG(err);
}
/* }}} */

/* Special handling of subjectAltName, see CVE-2013-4073
 * Christian Heimes
 */

static int openssl_x509v3_subjectAltName(BIO *bio, X509_EXTENSION *extension)
{
	GENERAL_NAMES *names;
	const X509V3_EXT_METHOD *method = NULL;
	ASN1_OCTET_STRING *extension_data;
	long i, length, num;
	const unsigned char *p;

	method = X509V3_EXT_get(extension);
	if (method == NULL) {
		return -1;
	}

	extension_data = X509_EXTENSION_get_data(extension);
	p = extension_data->data;
	length = extension_data->length;
	if (method->it) {
		names = (GENERAL_NAMES*) (ASN1_item_d2i(NULL, &p, length,
			ASN1_ITEM_ptr(method->it)));
	} else {
		names = (GENERAL_NAMES*) (method->d2i(NULL, &p, length));
	}
	if (names == NULL) {
		php_openssl_store_errors();
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
				BIO_write(bio, ASN1_STRING_get0_data(as),
					ASN1_STRING_length(as));
				break;
			case GEN_DNS:
				BIO_puts(bio, "DNS:");
				as = name->d.dNSName;
				BIO_write(bio, ASN1_STRING_get0_data(as),
					ASN1_STRING_length(as));
				break;
			case GEN_URI:
				BIO_puts(bio, "URI:");
				as = name->d.uniformResourceIdentifier;
				BIO_write(bio, ASN1_STRING_get0_data(as),
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

/* {{{ Returns an array of the fields/values of the CERT */
PHP_FUNCTION(openssl_x509_parse)
{
	X509 *cert;
	zend_object *cert_obj;
	zend_string *cert_str;
	int i, sig_nid;
	zend_bool useshortnames = 1;
	char * tmpstr;
	zval subitem;
	X509_EXTENSION *extension;
	X509_NAME *subject_name;
	char *cert_name;
	char *extname;
	BIO *bio_out;
	BUF_MEM *bio_buf;
	ASN1_INTEGER *asn1_serial;
	BIGNUM *bn_serial;
	char *str_serial;
	char *hex_serial;
	char buf[256];

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(useshortnames)
	ZEND_PARSE_PARAMETERS_END();

	cert = php_openssl_x509_from_param(cert_obj, cert_str);
	if (cert == NULL) {
		// TODO Add Warning?
		RETURN_FALSE;
	}
	array_init(return_value);

	subject_name = X509_get_subject_name(cert);
	cert_name = X509_NAME_oneline(subject_name, NULL, 0);
	add_assoc_string(return_value, "name", cert_name);
	OPENSSL_free(cert_name);

	php_openssl_add_assoc_name_entry(return_value, "subject", subject_name, useshortnames);
	/* hash as used in CA directories to lookup cert by subject name */
	{
		char buf[32];
		snprintf(buf, sizeof(buf), "%08lx", X509_subject_name_hash(cert));
		add_assoc_string(return_value, "hash", buf);
	}

	php_openssl_add_assoc_name_entry(return_value, "issuer", X509_get_issuer_name(cert), useshortnames);
	add_assoc_long(return_value, "version", X509_get_version(cert));

	asn1_serial = X509_get_serialNumber(cert);

	bn_serial = ASN1_INTEGER_to_BN(asn1_serial, NULL);
	/* Can return NULL on error or memory allocation failure */
	if (!bn_serial) {
		php_openssl_store_errors();
		RETURN_FALSE;
	}

	hex_serial = BN_bn2hex(bn_serial);
	BN_free(bn_serial);
	/* Can return NULL on error or memory allocation failure */
	if (!hex_serial) {
		php_openssl_store_errors();
		RETURN_FALSE;
	}

	str_serial = i2s_ASN1_INTEGER(NULL, asn1_serial);
	add_assoc_string(return_value, "serialNumber", str_serial);
	OPENSSL_free(str_serial);

	/* Return the hex representation of the serial number, as defined by OpenSSL */
	add_assoc_string(return_value, "serialNumberHex", hex_serial);
	OPENSSL_free(hex_serial);

	php_openssl_add_assoc_asn1_string(return_value, "validFrom", 	X509_getm_notBefore(cert));
	php_openssl_add_assoc_asn1_string(return_value, "validTo", 		X509_getm_notAfter(cert));

	add_assoc_long(return_value, "validFrom_time_t", php_openssl_asn1_time_to_time_t(X509_getm_notBefore(cert)));
	add_assoc_long(return_value, "validTo_time_t",  php_openssl_asn1_time_to_time_t(X509_getm_notAfter(cert)));

	tmpstr = (char *)X509_alias_get0(cert, NULL);
	if (tmpstr) {
		add_assoc_string(return_value, "alias", tmpstr);
	}

	sig_nid = X509_get_signature_nid(cert);
	add_assoc_string(return_value, "signatureTypeSN", (char*)OBJ_nid2sn(sig_nid));
	add_assoc_string(return_value, "signatureTypeLN", (char*)OBJ_nid2ln(sig_nid));
	add_assoc_long(return_value, "signatureTypeNID", sig_nid);
	array_init(&subitem);

	/* NOTE: the purposes are added as integer keys - the keys match up to the X509_PURPOSE_SSL_XXX defines
	   in x509v3.h */
	for (i = 0; i < X509_PURPOSE_get_count(); i++) {
		int id, purpset;
		char * pname;
		X509_PURPOSE * purp;
		zval subsub;

		array_init(&subsub);

		purp = X509_PURPOSE_get0(i);
		id = X509_PURPOSE_get_id(purp);

		purpset = X509_check_purpose(cert, id, 0);
		add_index_bool(&subsub, 0, purpset);

		purpset = X509_check_purpose(cert, id, 1);
		add_index_bool(&subsub, 1, purpset);

		pname = useshortnames ? X509_PURPOSE_get0_sname(purp) : X509_PURPOSE_get0_name(purp);
		add_index_string(&subsub, 2, pname);

		/* NOTE: if purpset > 1 then it's a warning - we should mention it ? */

		add_index_zval(&subitem, id, &subsub);
	}
	add_assoc_zval(return_value, "purposes", &subitem);

	array_init(&subitem);


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
		if (bio_out == NULL) {
			php_openssl_store_errors();
			RETURN_FALSE;
		}
		if (nid == NID_subject_alt_name) {
			if (openssl_x509v3_subjectAltName(bio_out, extension) == 0) {
				BIO_get_mem_ptr(bio_out, &bio_buf);
				add_assoc_stringl(&subitem, extname, bio_buf->data, bio_buf->length);
			} else {
				zend_array_destroy(Z_ARR_P(return_value));
				BIO_free(bio_out);
				if (cert_str) {
					X509_free(cert);
				}
				RETURN_FALSE;
			}
		}
		else if (X509V3_EXT_print(bio_out, extension, 0, 0)) {
			BIO_get_mem_ptr(bio_out, &bio_buf);
			add_assoc_stringl(&subitem, extname, bio_buf->data, bio_buf->length);
		} else {
			php_openssl_add_assoc_asn1_string(&subitem, extname, X509_EXTENSION_get_data(extension));
		}
		BIO_free(bio_out);
	}
	add_assoc_zval(return_value, "extensions", &subitem);
	if (cert_str) {
		X509_free(cert);
	}
}
/* }}} */

/* {{{ php_openssl_load_all_certs_from_file */
static STACK_OF(X509) *php_openssl_load_all_certs_from_file(char *certfile)
{
	STACK_OF(X509_INFO) *sk=NULL;
	STACK_OF(X509) *stack=NULL, *ret=NULL;
	BIO *in=NULL;
	X509_INFO *xi;

	if(!(stack = sk_X509_new_null())) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_ERROR, "Memory allocation failure");
		goto end;
	}

	if (php_openssl_open_base_dir_chk(certfile)) {
		sk_X509_free(stack);
		goto end;
	}

	if (!(in=BIO_new_file(certfile, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY)))) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error opening the file, %s", certfile);
		sk_X509_free(stack);
		goto end;
	}

	/* This loads from a file, a stack of x509/crl/pkey sets */
	if (!(sk=PEM_X509_INFO_read_bio(in, NULL, NULL, NULL))) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error reading the file, %s", certfile);
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
	if (!sk_X509_num(stack)) {
		php_error_docref(NULL, E_WARNING, "No certificates in file, %s", certfile);
		sk_X509_free(stack);
		goto end;
	}
	ret = stack;
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

	csc = X509_STORE_CTX_new();
	if (csc == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_ERROR, "Memory allocation failure");
		return 0;
	}
	if (!X509_STORE_CTX_init(csc, ctx, x, untrustedchain)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Certificate store initialization failed");
		return 0;
	}
	if (purpose >= 0 && !X509_STORE_CTX_set_purpose(csc, purpose)) {
		php_openssl_store_errors();
	}
	ret = X509_verify_cert(csc);
	if (ret < 0) {
		php_openssl_store_errors();
	}
	X509_STORE_CTX_free(csc);

	return ret;
}
/* }}} */

/* {{{ Checks the CERT to see if it can be used for the purpose in purpose. cainfo holds information about trusted CAs */
PHP_FUNCTION(openssl_x509_checkpurpose)
{
	X509 *cert;
	zend_object *cert_obj;
	zend_string *cert_str;
	zval *zcainfo = NULL;
	X509_STORE *cainfo = NULL;
	STACK_OF(X509) *untrustedchain = NULL;
	zend_long purpose;
	char * untrusted = NULL;
	size_t untrusted_len = 0;
	int ret;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_LONG(purpose)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY(zcainfo)
		Z_PARAM_STRING_OR_NULL(untrusted, untrusted_len)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_LONG(-1);

	if (untrusted) {
		untrustedchain = php_openssl_load_all_certs_from_file(untrusted);
		if (untrustedchain == NULL) {
			goto clean_exit;
		}
	}

	cainfo = php_openssl_setup_verify(zcainfo);
	if (cainfo == NULL) {
		goto clean_exit;
	}
	cert = php_openssl_x509_from_param(cert_obj, cert_str);
	if (cert == NULL) {
		// TODO Add Warning?
		goto clean_exit;
	}

	ret = check_cert(cainfo, cert, untrustedchain, (int)purpose);
	if (ret != 0 && ret != 1) {
		RETVAL_LONG(ret);
	} else {
		RETVAL_BOOL(ret);
	}
	if (cert_str) {
		X509_free(cert);
	}
clean_exit:
	if (cainfo) {
		X509_STORE_free(cainfo);
	}
	if (untrustedchain) {
		sk_X509_pop_free(untrustedchain, X509_free);
	}
}
/* }}} */

/* {{{ php_openssl_setup_verify
 * calist is an array containing file and directory names.  create a
 * certificate store and add those certs to it for use in verification.
*/
static X509_STORE *php_openssl_setup_verify(zval *calist)
{
	X509_STORE *store;
	X509_LOOKUP * dir_lookup, * file_lookup;
	int ndirs = 0, nfiles = 0;
	zval * item;
	zend_stat_t sb;

	store = X509_STORE_new();

	if (store == NULL) {
		php_openssl_store_errors();
		return NULL;
	}

	if (calist && (Z_TYPE_P(calist) == IS_ARRAY)) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(calist), item) {
			zend_string *str = zval_try_get_string(item);
			if (UNEXPECTED(!str)) {
				return NULL;
			}

			if (VCWD_STAT(ZSTR_VAL(str), &sb) == -1) {
				php_error_docref(NULL, E_WARNING, "Unable to stat %s", ZSTR_VAL(str));
				zend_string_release(str);
				continue;
			}

			if ((sb.st_mode & S_IFREG) == S_IFREG) {
				file_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
				if (file_lookup == NULL || !X509_LOOKUP_load_file(file_lookup, ZSTR_VAL(str), X509_FILETYPE_PEM)) {
					php_openssl_store_errors();
					php_error_docref(NULL, E_WARNING, "Error loading file %s", ZSTR_VAL(str));
				} else {
					nfiles++;
				}
				file_lookup = NULL;
			} else {
				dir_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
				if (dir_lookup == NULL || !X509_LOOKUP_add_dir(dir_lookup, ZSTR_VAL(str), X509_FILETYPE_PEM)) {
					php_openssl_store_errors();
					php_error_docref(NULL, E_WARNING, "Error loading directory %s", ZSTR_VAL(str));
				} else {
					ndirs++;
				}
				dir_lookup = NULL;
			}
			zend_string_release(str);
		} ZEND_HASH_FOREACH_END();
	}
	if (nfiles == 0) {
		file_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
		if (file_lookup == NULL || !X509_LOOKUP_load_file(file_lookup, NULL, X509_FILETYPE_DEFAULT)) {
			php_openssl_store_errors();
		}
	}
	if (ndirs == 0) {
		dir_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
		if (dir_lookup == NULL || !X509_LOOKUP_add_dir(dir_lookup, NULL, X509_FILETYPE_DEFAULT)) {
			php_openssl_store_errors();
		}
	}
	return store;
}
/* }}} */

/* {{{ Reads X.509 certificates */
PHP_FUNCTION(openssl_x509_read)
{
	X509 *cert;
	php_openssl_certificate_object *x509_cert_obj;
	zend_object *cert_obj;
	zend_string *cert_str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(cert_obj, php_openssl_certificate_ce, cert_str)
	ZEND_PARSE_PARAMETERS_END();

	cert = php_openssl_x509_from_param(cert_obj, cert_str);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		RETURN_FALSE;
	}

	object_init_ex(return_value, php_openssl_certificate_ce);
	x509_cert_obj = Z_OPENSSL_CERTIFICATE_P(return_value);
	x509_cert_obj->x509 = cert_obj ? X509_dup(cert) : cert;
}
/* }}} */

/* {{{ Frees X.509 certificates */
PHP_FUNCTION(openssl_x509_free)
{
	zval *x509;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(x509, php_openssl_certificate_ce)
	ZEND_PARSE_PARAMETERS_END();
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

static STACK_OF(X509) * php_array_to_X509_sk(zval * zcerts) /* {{{ */
{
	zval * zcertval;
	STACK_OF(X509) * sk = NULL;
	X509 * cert;
	bool free_cert;

	sk = sk_X509_new_null();

	/* get certs */
	if (Z_TYPE_P(zcerts) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zcerts), zcertval) {
			cert = php_openssl_x509_from_zval(zcertval, &free_cert);
			if (cert == NULL) {
				// TODO Add Warning?
				goto clean_exit;
			}

			if (!free_cert) {
				cert = X509_dup(cert);

				if (cert == NULL) {
					php_openssl_store_errors();
					goto clean_exit;
				}

			}
			sk_X509_push(sk, cert);
		} ZEND_HASH_FOREACH_END();
	} else {
		/* a single certificate */
		cert = php_openssl_x509_from_zval(zcerts, &free_cert);

		if (cert == NULL) {
			// TODO Add Warning?
			goto clean_exit;
		}

		if (!free_cert) {
			cert = X509_dup(cert);
			if (cert == NULL) {
				php_openssl_store_errors();
				goto clean_exit;
			}
		}
		sk_X509_push(sk, cert);
	}

clean_exit:
	return sk;
}
/* }}} */

/* {{{ Creates and exports a PKCS to file */
PHP_FUNCTION(openssl_pkcs12_export_to_file)
{
	X509 *cert;
	zend_object *cert_obj;
	zend_string *cert_str;
	BIO * bio_out = NULL;
	PKCS12 * p12 = NULL;
	char * filename;
	char * friendly_name = NULL;
	size_t filename_len;
	char * pass;
	size_t pass_len;
	zval *zpkey = NULL, *args = NULL;
	EVP_PKEY *priv_key = NULL;
	zval * item;
	STACK_OF(X509) *ca = NULL;

	ZEND_PARSE_PARAMETERS_START(4, 5)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_ZVAL(zpkey)
		Z_PARAM_STRING(pass, pass_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY(args)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	cert = php_openssl_x509_from_param(cert_obj, cert_str);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		return;
	}

	priv_key = php_openssl_pkey_from_zval(zpkey, 0, "", 0);
	if (priv_key == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Cannot get private key from parameter 3");
		}
		goto cleanup;
	}
	if (!X509_check_private_key(cert, priv_key)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Private key does not correspond to cert");
		goto cleanup;
	}
	if (php_openssl_open_base_dir_chk(filename)) {
		goto cleanup;
	}

	/* parse extra config from args array, promote this to an extra function */
	if (args &&
		(item = zend_hash_str_find(Z_ARRVAL_P(args), "friendly_name", sizeof("friendly_name")-1)) != NULL &&
		Z_TYPE_P(item) == IS_STRING
	) {
		friendly_name = Z_STRVAL_P(item);
	}
	/* certpbe (default RC2-40)
	   keypbe (default 3DES)
	   friendly_caname
	*/

	if (args && (item = zend_hash_str_find(Z_ARRVAL_P(args), "extracerts", sizeof("extracerts")-1)) != NULL) {
		ca = php_array_to_X509_sk(item);
	}
	/* end parse extra config */

	/*PKCS12 *PKCS12_create(char *pass, char *name, EVP_PKEY *pkey, X509 *cert, STACK_OF(X509) *ca,
				int nid_key, int nid_cert, int iter, int mac_iter, int keytype);*/

	p12 = PKCS12_create(pass, friendly_name, priv_key, cert, ca, 0, 0, 0, 0, 0);
	if (p12 != NULL) {
		bio_out = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
		if (bio_out != NULL) {

			i2d_PKCS12_bio(bio_out, p12);
			BIO_free(bio_out);

			RETVAL_TRUE;
		} else {
			php_openssl_store_errors();
			php_error_docref(NULL, E_WARNING, "Error opening file %s", filename);
		}

		PKCS12_free(p12);
	} else {
		php_openssl_store_errors();
	}

	php_sk_X509_free(ca);

cleanup:
	EVP_PKEY_free(priv_key);

	if (cert_str) {
		X509_free(cert);
	}
}
/* }}} */

/* {{{ Creates and exports a PKCS12 to a var */
PHP_FUNCTION(openssl_pkcs12_export)
{
	X509 *cert;
	zend_object *cert_obj;
	zend_string *cert_str;
	BIO * bio_out;
	PKCS12 * p12 = NULL;
	zval *zout = NULL, *zpkey, *args = NULL;
	EVP_PKEY *priv_key = NULL;
	char * pass;
	size_t pass_len;
	char * friendly_name = NULL;
	zval * item;
	STACK_OF(X509) *ca = NULL;

	ZEND_PARSE_PARAMETERS_START(4, 5)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_ZVAL(zout)
		Z_PARAM_ZVAL(zpkey)
		Z_PARAM_STRING(pass, pass_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY(args)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	cert = php_openssl_x509_from_param(cert_obj, cert_str);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		return;
	}

	priv_key = php_openssl_pkey_from_zval(zpkey, 0, "", 0);
	if (priv_key == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Cannot get private key from parameter 3");
		}
		goto cleanup;
	}
	if (!X509_check_private_key(cert, priv_key)) {
		php_error_docref(NULL, E_WARNING, "Private key does not correspond to cert");
		goto cleanup;
	}

	/* parse extra config from args array, promote this to an extra function */
	if (args &&
		(item = zend_hash_str_find(Z_ARRVAL_P(args), "friendly_name", sizeof("friendly_name")-1)) != NULL &&
		Z_TYPE_P(item) == IS_STRING
	) {
		friendly_name = Z_STRVAL_P(item);
	}

	if (args && (item = zend_hash_str_find(Z_ARRVAL_P(args), "extracerts", sizeof("extracerts")-1)) != NULL) {
		ca = php_array_to_X509_sk(item);
	}
	/* end parse extra config */

	p12 = PKCS12_create(pass, friendly_name, priv_key, cert, ca, 0, 0, 0, 0, 0);

	if (p12 != NULL) {
		bio_out = BIO_new(BIO_s_mem());
		if (i2d_PKCS12_bio(bio_out, p12)) {
			BUF_MEM *bio_buf;

			BIO_get_mem_ptr(bio_out, &bio_buf);
			ZEND_TRY_ASSIGN_REF_STRINGL(zout, bio_buf->data, bio_buf->length);

			RETVAL_TRUE;
		} else {
			php_openssl_store_errors();
		}

		BIO_free(bio_out);
		PKCS12_free(p12);
	} else {
		php_openssl_store_errors();
	}
	php_sk_X509_free(ca);

cleanup:
	EVP_PKEY_free(priv_key);
	if (cert_str) {
		X509_free(cert);
	}
}
/* }}} */

/* {{{ Parses a PKCS12 to an array */
PHP_FUNCTION(openssl_pkcs12_read)
{
	zval *zout = NULL, zextracerts, zcert, zpkey;
	char *pass, *zp12;
	size_t pass_len, zp12_len;
	PKCS12 * p12 = NULL;
	EVP_PKEY * pkey = NULL;
	X509 * cert = NULL;
	STACK_OF(X509) * ca = NULL;
	BIO * bio_in = NULL;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szs", &zp12, &zp12_len, &zout, &pass, &pass_len) == FAILURE) {
		RETURN_THROWS();
	}

	RETVAL_FALSE;

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(zp12_len, pkcs12, 1);

	bio_in = BIO_new(BIO_s_mem());

	if (0 >= BIO_write(bio_in, zp12, (int)zp12_len)) {
		php_openssl_store_errors();
		goto cleanup;
	}

	if (d2i_PKCS12_bio(bio_in, &p12) && PKCS12_parse(p12, pass, &pkey, &cert, &ca)) {
		BIO * bio_out;
		int cert_num;

		zout = zend_try_array_init(zout);
		if (!zout) {
			goto cleanup;
		}

		if (cert) {
			bio_out = BIO_new(BIO_s_mem());
			if (PEM_write_bio_X509(bio_out, cert)) {
				BUF_MEM *bio_buf;
				BIO_get_mem_ptr(bio_out, &bio_buf);
				ZVAL_STRINGL(&zcert, bio_buf->data, bio_buf->length);
				add_assoc_zval(zout, "cert", &zcert);
			} else {
				php_openssl_store_errors();
			}
			BIO_free(bio_out);
		}

		if (pkey) {
			bio_out = BIO_new(BIO_s_mem());
			if (PEM_write_bio_PrivateKey(bio_out, pkey, NULL, NULL, 0, 0, NULL)) {
				BUF_MEM *bio_buf;
				BIO_get_mem_ptr(bio_out, &bio_buf);
				ZVAL_STRINGL(&zpkey, bio_buf->data, bio_buf->length);
				add_assoc_zval(zout, "pkey", &zpkey);
			} else {
				php_openssl_store_errors();
			}
			BIO_free(bio_out);
		}

		cert_num = sk_X509_num(ca);
		if (ca && cert_num) {
			array_init(&zextracerts);

			for (i = 0; i < cert_num; i++) {
				zval zextracert;
				X509* aCA = sk_X509_pop(ca);
				if (!aCA) break;

				bio_out = BIO_new(BIO_s_mem());
				if (PEM_write_bio_X509(bio_out, aCA)) {
					BUF_MEM *bio_buf;
					BIO_get_mem_ptr(bio_out, &bio_buf);
					ZVAL_STRINGL(&zextracert, bio_buf->data, bio_buf->length);
					add_index_zval(&zextracerts, i, &zextracert);
				}

				X509_free(aCA);
				BIO_free(bio_out);
			}

			sk_X509_free(ca);
			add_assoc_zval(zout, "extracerts", &zextracerts);
		}

		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
	}

cleanup:
	BIO_free(bio_in);
	EVP_PKEY_free(pkey);
	if (cert) {
		X509_free(cert);
	}
	if (p12) {
		PKCS12_free(p12);
	}
}
/* }}} */

/* {{{ x509 CSR functions */

/* {{{ php_openssl_make_REQ */
static int php_openssl_make_REQ(struct php_x509_request * req, X509_REQ * csr, zval * dn, zval * attribs)
{
	STACK_OF(CONF_VALUE) * dn_sk, *attr_sk = NULL;
	char * str, *dn_sect, *attr_sect;

	dn_sect = CONF_get_string(req->req_config, req->section_name, "distinguished_name");
	if (dn_sect == NULL) {
		php_openssl_store_errors();
		return FAILURE;
	}
	dn_sk = CONF_get_section(req->req_config, dn_sect);
	if (dn_sk == NULL) {
		php_openssl_store_errors();
		return FAILURE;
	}
	attr_sect = CONF_get_string(req->req_config, req->section_name, "attributes");
	if (attr_sect == NULL) {
		php_openssl_store_errors();
		attr_sk = NULL;
	} else {
		attr_sk = CONF_get_section(req->req_config, attr_sect);
		if (attr_sk == NULL) {
			php_openssl_store_errors();
			return FAILURE;
		}
	}
	/* setup the version number: version 1 */
	if (X509_REQ_set_version(csr, 0L)) {
		int i, nid;
		char * type;
		CONF_VALUE * v;
		X509_NAME * subj;
		zval * item;
		zend_string * strindex = NULL;

		subj = X509_REQ_get_subject_name(csr);
		/* apply values from the dn hash */
		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(dn), strindex, item) {
			if (strindex) {
				int nid = OBJ_txt2nid(ZSTR_VAL(strindex));
				if (nid != NID_undef) {
					zend_string *str_item = zval_try_get_string(item);
					if (UNEXPECTED(!str_item)) {
						return FAILURE;
					}
					if (!X509_NAME_add_entry_by_NID(subj, nid, MBSTRING_UTF8,
								(unsigned char*)ZSTR_VAL(str_item), -1, -1, 0))
					{
						php_openssl_store_errors();
						php_error_docref(NULL, E_WARNING,
							"dn: add_entry_by_NID %d -> %s (failed; check error"
							" queue and value of string_mask OpenSSL option "
							"if illegal characters are reported)",
							nid, ZSTR_VAL(str_item));
						zend_string_release(str_item);
						return FAILURE;
					}
					zend_string_release(str_item);
				} else {
					php_error_docref(NULL, E_WARNING, "dn: %s is not a recognized name", ZSTR_VAL(strindex));
				}
			}
		} ZEND_HASH_FOREACH_END();

		/* Finally apply defaults from config file */
		for(i = 0; i < sk_CONF_VALUE_num(dn_sk); i++) {
			size_t len;
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
				php_openssl_store_errors();
				php_error_docref(NULL, E_WARNING, "add_entry_by_txt %s -> %s (failed)", type, v->value);
				return FAILURE;
			}
			if (!X509_NAME_entry_count(subj)) {
				php_error_docref(NULL, E_WARNING, "No objects specified in config file");
				return FAILURE;
			}
		}
		if (attribs) {
			ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(attribs), strindex, item) {
				int nid;

				if (NULL == strindex) {
					php_error_docref(NULL, E_WARNING, "dn: numeric fild names are not supported");
					continue;
				}

				nid = OBJ_txt2nid(ZSTR_VAL(strindex));
				if (nid != NID_undef) {
					zend_string *str_item = zval_try_get_string(item);
					if (UNEXPECTED(!str_item)) {
						return FAILURE;
					}
					if (!X509_NAME_add_entry_by_NID(subj, nid, MBSTRING_UTF8, (unsigned char*)ZSTR_VAL(str_item), -1, -1, 0)) {
						php_openssl_store_errors();
						php_error_docref(NULL, E_WARNING, "attribs: add_entry_by_NID %d -> %s (failed)", nid, ZSTR_VAL(str_item));
						zend_string_release(str_item);
						return FAILURE;
					}
					zend_string_release(str_item);
				} else {
					php_error_docref(NULL, E_WARNING, "dn: %s is not a recognized name", ZSTR_VAL(strindex));
				}
			} ZEND_HASH_FOREACH_END();
			for (i = 0; i < sk_CONF_VALUE_num(attr_sk); i++) {
				v = sk_CONF_VALUE_value(attr_sk, i);
				/* if it is already set, skip this */
				nid = OBJ_txt2nid(v->name);
				if (X509_REQ_get_attr_by_NID(csr, nid, -1) >= 0) {
					continue;
				}
				if (!X509_REQ_add1_attr_by_txt(csr, v->name, MBSTRING_UTF8, (unsigned char*)v->value, -1)) {
					php_openssl_store_errors();
					php_error_docref(NULL, E_WARNING,
						"add1_attr_by_txt %s -> %s (failed; check error queue "
						"and value of string_mask OpenSSL option if illegal "
						"characters are reported)",
						v->name, v->value);
					return FAILURE;
				}
			}
		}
	} else {
		php_openssl_store_errors();
	}

	if (!X509_REQ_set_pubkey(csr, req->priv_key)) {
		php_openssl_store_errors();
	}
	return SUCCESS;
}
/* }}} */


static X509_REQ *php_openssl_csr_from_str(zend_string *csr_str)
{
	X509_REQ * csr = NULL;
	char * filename = NULL;
	BIO * in;

	if (ZSTR_LEN(csr_str) > 7 && memcmp(ZSTR_VAL(csr_str), "file://", sizeof("file://") - 1) == 0) {
		filename = ZSTR_VAL(csr_str) + (sizeof("file://") - 1);
	}

	if (filename) {
		if (php_openssl_open_base_dir_chk(filename)) {
			return NULL;
		}
		in = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
	} else {
		in = BIO_new_mem_buf(ZSTR_VAL(csr_str), (int) ZSTR_LEN(csr_str));
	}

	if (in == NULL) {
		php_openssl_store_errors();
		return NULL;
	}

	csr = PEM_read_bio_X509_REQ(in, NULL,NULL,NULL);
	if (csr == NULL) {
		php_openssl_store_errors();
	}

	BIO_free(in);

	return csr;
}

static X509_REQ *php_openssl_csr_from_param(zend_object *csr_obj, zend_string *csr_str)
{
	if (csr_obj) {
		return php_openssl_request_from_obj(csr_obj)->csr;
	}

	ZEND_ASSERT(csr_str);

	return php_openssl_csr_from_str(csr_str);
}

/* {{{ Exports a CSR to file */
PHP_FUNCTION(openssl_csr_export_to_file)
{
	X509_REQ *csr;
	zend_object *csr_obj;
	zend_string *csr_str;
	zend_bool notext = 1;
	char * filename = NULL;
	size_t filename_len;
	BIO * bio_out;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(csr_obj, php_openssl_request_ce, csr_str)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(notext)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	csr = php_openssl_csr_from_param(csr_obj, csr_str);
	if (csr == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate Signing Request cannot be retrieved");
		return;
	}

	if (php_openssl_open_base_dir_chk(filename)) {
		return;
	}

	bio_out = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
	if (bio_out != NULL) {
		if (!notext && !X509_REQ_print(bio_out, csr)) {
			php_openssl_store_errors();
		}
		if (!PEM_write_bio_X509_REQ(bio_out, csr)) {
			php_error_docref(NULL, E_WARNING, "Error writing PEM to file %s", filename);
			php_openssl_store_errors();
		} else {
			RETVAL_TRUE;
		}
		BIO_free(bio_out);
	} else {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error opening file %s", filename);
	}

	if (csr_str) {
		X509_REQ_free(csr);
	}
}
/* }}} */

/* {{{ Exports a CSR to file or a var */
PHP_FUNCTION(openssl_csr_export)
{
	X509_REQ *csr;
	zend_object *csr_obj;
	zend_string *csr_str;
	zval *zout;
	zend_bool notext = 1;
	BIO * bio_out;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(csr_obj, php_openssl_request_ce, csr_str)
		Z_PARAM_ZVAL(zout)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(notext)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	csr = php_openssl_csr_from_param(csr_obj, csr_str);
	if (csr == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate Signing Request cannot be retrieved");
		return;
	}

	/* export to a var */

	bio_out = BIO_new(BIO_s_mem());
	if (!notext && !X509_REQ_print(bio_out, csr)) {
		php_openssl_store_errors();
	}

	if (PEM_write_bio_X509_REQ(bio_out, csr)) {
		BUF_MEM *bio_buf;

		BIO_get_mem_ptr(bio_out, &bio_buf);
		ZEND_TRY_ASSIGN_REF_STRINGL(zout, bio_buf->data, bio_buf->length);

		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
	}

	if (csr_str) {
		X509_REQ_free(csr);
	}
	BIO_free(bio_out);
}
/* }}} */

/* {{{ Signs a cert with another CERT */
PHP_FUNCTION(openssl_csr_sign)
{
	X509_REQ *csr;
	zend_object *csr_obj;
	zend_string *csr_str;

	php_openssl_certificate_object *cert_object;
	zend_object *cert_obj;
	zend_string *cert_str;
	zval *zpkey, *args = NULL;
	zend_long num_days;
	zend_long serial = Z_L(0);
	X509 *cert = NULL, *new_cert = NULL;
	EVP_PKEY * key = NULL, *priv_key = NULL;
	int i;
	struct php_x509_request req;

	ZEND_PARSE_PARAMETERS_START(4, 6)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(csr_obj, php_openssl_request_ce, csr_str)
		Z_PARAM_OBJ_OF_CLASS_OR_STR_OR_NULL(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_ZVAL(zpkey)
		Z_PARAM_LONG(num_days)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_OR_NULL(args)
		Z_PARAM_LONG(serial)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	csr = php_openssl_csr_from_param(csr_obj, csr_str);
	if (csr == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate Signing Request cannot be retrieved");
		return;
	}

	PHP_SSL_REQ_INIT(&req);

	if (cert_str || cert_obj) {
		cert = php_openssl_x509_from_param(cert_obj, cert_str);
		if (cert == NULL) {
			php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
			goto cleanup;
		}
	}

	priv_key = php_openssl_pkey_from_zval(zpkey, 0, "", 0);
	if (priv_key == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Cannot get private key from parameter 3");
		}
		goto cleanup;
	}
	if (cert && !X509_check_private_key(cert, priv_key)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Private key does not correspond to signing cert");
		goto cleanup;
	}

	if (PHP_SSL_REQ_PARSE(&req, args) == FAILURE) {
		goto cleanup;
	}
	/* Check that the request matches the signature */
	key = X509_REQ_get_pubkey(csr);
	if (key == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error unpacking public key");
		goto cleanup;
	}
	i = X509_REQ_verify(csr, key);

	if (i < 0) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Signature verification problems");
		goto cleanup;
	}
	else if (i == 0) {
		php_error_docref(NULL, E_WARNING, "Signature did not match the certificate request");
		goto cleanup;
	}

	/* Now we can get on with it */

	new_cert = X509_new();
	if (new_cert == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "No memory");
		goto cleanup;
	}
	/* Version 3 cert */
	if (!X509_set_version(new_cert, 2)) {
		goto cleanup;
	}


	ASN1_INTEGER_set(X509_get_serialNumber(new_cert), (long)serial);

	X509_set_subject_name(new_cert, X509_REQ_get_subject_name(csr));

	if (cert == NULL) {
		cert = new_cert;
	}
	if (!X509_set_issuer_name(new_cert, X509_get_subject_name(cert))) {
		php_openssl_store_errors();
		goto cleanup;
	}
	X509_gmtime_adj(X509_getm_notBefore(new_cert), 0);
	X509_gmtime_adj(X509_getm_notAfter(new_cert), 60*60*24*(long)num_days);
	i = X509_set_pubkey(new_cert, key);
	if (!i) {
		php_openssl_store_errors();
		goto cleanup;
	}
	if (req.extensions_section) {
		X509V3_CTX ctx;

		X509V3_set_ctx(&ctx, cert, new_cert, csr, NULL, 0);
		X509V3_set_conf_lhash(&ctx, req.req_config);
		if (!X509V3_EXT_add_conf(req.req_config, &ctx, req.extensions_section, new_cert)) {
			php_openssl_store_errors();
			goto cleanup;
		}
	}

	/* Now sign it */
	if (!X509_sign(new_cert, priv_key, req.digest)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Failed to sign it");
		goto cleanup;
	}

	object_init_ex(return_value, php_openssl_certificate_ce);
	cert_object = Z_OPENSSL_CERTIFICATE_P(return_value);
	cert_object->x509 = new_cert;

cleanup:

	if (cert == new_cert) {
		cert = NULL;
	}

	PHP_SSL_REQ_DISPOSE(&req);
	EVP_PKEY_free(priv_key);
	EVP_PKEY_free(key);
	if (csr_str) {
		X509_REQ_free(csr);
	}
	if (cert_str && cert) {
		X509_free(cert);
	}
}
/* }}} */

/* {{{ Generates a privkey and CSR */
PHP_FUNCTION(openssl_csr_new)
{
	struct php_x509_request req;
	php_openssl_request_object *x509_request_obj;
	zval *args = NULL, *dn, *attribs = NULL;
	zval *out_pkey;
	X509_REQ *csr = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "az|a!a!", &dn, &out_pkey, &args, &attribs) == FAILURE) {
		RETURN_THROWS();
	}
	RETVAL_FALSE;

	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS) {
		int we_made_the_key = 0;
		zval *out_pkey_val = out_pkey;
		ZVAL_DEREF(out_pkey_val);

		/* Generate or use a private key */
		if (Z_TYPE_P(out_pkey_val) != IS_NULL) {
			req.priv_key = php_openssl_pkey_from_zval(out_pkey_val, 0, NULL, 0);
		}
		if (req.priv_key == NULL) {
			php_openssl_generate_private_key(&req);
			we_made_the_key = 1;
		}
		if (req.priv_key == NULL) {
			php_error_docref(NULL, E_WARNING, "Unable to generate a private key");
		} else {
			csr = X509_REQ_new();
			if (csr) {
				if (php_openssl_make_REQ(&req, csr, dn, attribs) == SUCCESS) {
					X509V3_CTX ext_ctx;

					X509V3_set_ctx(&ext_ctx, NULL, NULL, csr, NULL, 0);
					X509V3_set_conf_lhash(&ext_ctx, req.req_config);

					/* Add extensions */
					if (req.request_extensions_section && !X509V3_EXT_REQ_add_conf(req.req_config,
								&ext_ctx, req.request_extensions_section, csr))
					{
						php_openssl_store_errors();
						php_error_docref(NULL, E_WARNING, "Error loading extension section %s", req.request_extensions_section);
					} else {
						RETVAL_TRUE;

						if (X509_REQ_sign(csr, req.priv_key, req.digest)) {
							object_init_ex(return_value, php_openssl_request_ce);
							x509_request_obj = Z_OPENSSL_REQUEST_P(return_value);
							x509_request_obj->csr = csr;
							csr = NULL;
						} else {
							php_openssl_store_errors();
							php_error_docref(NULL, E_WARNING, "Error signing request");
						}

						if (we_made_the_key) {
							/* and an object for the private key */
							zval zkey_object;
							php_openssl_pkey_object *key_object;
							object_init_ex(&zkey_object, php_openssl_pkey_ce);
							key_object = Z_OPENSSL_PKEY_P(&zkey_object);
							key_object->pkey = req.priv_key;

							ZEND_TRY_ASSIGN_REF_TMP(out_pkey, &zkey_object);
							req.priv_key = NULL; /* make sure the cleanup code doesn't zap it! */
						}
					}
				}
			} else {
				php_openssl_store_errors();
			}

		}
	}
	if (csr) {
		X509_REQ_free(csr);
	}
	PHP_SSL_REQ_DISPOSE(&req);
}
/* }}} */

/* {{{ Returns the subject of a CERT or FALSE on error */
PHP_FUNCTION(openssl_csr_get_subject)
{
	X509_REQ *csr;
	zend_object *csr_obj;
	zend_string *csr_str;
	zend_bool use_shortnames = 1;
	X509_NAME *subject;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(csr_obj, php_openssl_request_ce, csr_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(use_shortnames)
	ZEND_PARSE_PARAMETERS_END();

	csr = php_openssl_csr_from_param(csr_obj, csr_str);
	if (csr == NULL) {
		RETURN_FALSE;
	}

	subject = X509_REQ_get_subject_name(csr);

	array_init(return_value);
	php_openssl_add_assoc_name_entry(return_value, NULL, subject, use_shortnames);

	if (csr_str) {
		X509_REQ_free(csr);
	}
}
/* }}} */

/* {{{ Returns the subject of a CERT or FALSE on error */
PHP_FUNCTION(openssl_csr_get_public_key)
{
	X509_REQ *orig_csr, *csr;
	zend_object *csr_obj;
	zend_string *csr_str;
	zend_bool use_shortnames = 1;

	php_openssl_pkey_object *key_object;
	EVP_PKEY *tpubkey;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(csr_obj, php_openssl_request_ce, csr_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(use_shortnames)
	ZEND_PARSE_PARAMETERS_END();

	orig_csr = php_openssl_csr_from_param(csr_obj, csr_str);
	if (orig_csr == NULL) {
		RETURN_FALSE;
	}

#if PHP_OPENSSL_API_VERSION >= 0x10100
	/* Due to changes in OpenSSL 1.1 related to locking when decoding CSR,
	 * the pub key is not changed after assigning. It means if we pass
	 * a private key, it will be returned including the private part.
	 * If we duplicate it, then we get just the public part which is
	 * the same behavior as for OpenSSL 1.0 */
	csr = X509_REQ_dup(orig_csr);
#else
	csr = orig_csr;
#endif

	/* Retrieve the public key from the CSR */
	tpubkey = X509_REQ_get_pubkey(csr);

	if (csr != orig_csr) {
		/* We need to free the duplicated CSR */
		X509_REQ_free(csr);
	}

	if (csr_str) {
		/* We also need to free the original CSR if it was freshly created */
		X509_REQ_free(orig_csr);
	}

	if (tpubkey == NULL) {
		php_openssl_store_errors();
		RETURN_FALSE;
	}

	object_init_ex(return_value, php_openssl_pkey_ce);
	key_object = Z_OPENSSL_PKEY_P(return_value);
	key_object->pkey = tpubkey;
}
/* }}} */

/* }}} */

/* {{{ EVP Public/Private key functions */

struct php_openssl_pem_password {
	char *key;
	int len;
};

/* {{{ php_openssl_pem_password_cb */
static int php_openssl_pem_password_cb(char *buf, int size, int rwflag, void *userdata)
{
	struct php_openssl_pem_password *password = userdata;

	if (password == NULL || password->key == NULL) {
		return -1;
	}

	size = (password->len > size) ? size : password->len;
	memcpy(buf, password->key, size);

	return size;
}
/* }}} */

static EVP_PKEY *php_openssl_pkey_from_zval(zval *val, int public_key, char *passphrase, size_t passphrase_len)
{
	EVP_PKEY *key = NULL;
	X509 *cert = NULL;
	bool free_cert = 0;
	char * filename = NULL;
	zval tmp;

	ZVAL_NULL(&tmp);

#define TMP_CLEAN \
	if (Z_TYPE(tmp) == IS_STRING) {\
		zval_ptr_dtor_str(&tmp); \
	} \
	return NULL;

	if (Z_TYPE_P(val) == IS_ARRAY) {
		zval * zphrase;

		/* get passphrase */

		if ((zphrase = zend_hash_index_find(Z_ARRVAL_P(val), 1)) == NULL) {
			zend_value_error("Key array must be of the form array(0 => key, 1 => phrase)");
			return NULL;
		}

		if (Z_TYPE_P(zphrase) == IS_STRING) {
			passphrase = Z_STRVAL_P(zphrase);
			passphrase_len = Z_STRLEN_P(zphrase);
		} else {
			ZVAL_COPY(&tmp, zphrase);
			if (!try_convert_to_string(&tmp)) {
				return NULL;
			}

			passphrase = Z_STRVAL(tmp);
			passphrase_len = Z_STRLEN(tmp);
		}

		/* now set val to be the key param and continue */
		if ((val = zend_hash_index_find(Z_ARRVAL_P(val), 0)) == NULL) {
			zend_value_error("Key array must be of the form array(0 => key, 1 => phrase)");
			TMP_CLEAN;
		}
	}

	if (Z_TYPE_P(val) == IS_OBJECT && Z_OBJCE_P(val) == php_openssl_pkey_ce) {
		int is_priv;

		key = php_openssl_pkey_from_obj(Z_OBJ_P(val))->pkey;
		is_priv = php_openssl_is_private_key(key);

		/* check whether it is actually a private key if requested */
		if (!public_key && !is_priv) {
			php_error_docref(NULL, E_WARNING, "Supplied key param is a public key");
			TMP_CLEAN;
		}

		if (public_key && is_priv) {
			php_error_docref(NULL, E_WARNING, "Don't know how to get public key from this private key");
			TMP_CLEAN;
		} else {
			if (Z_TYPE(tmp) == IS_STRING) {
				zval_ptr_dtor_str(&tmp);
			}

			EVP_PKEY_up_ref(key);
			return key;
		}
	} else if (Z_TYPE_P(val) == IS_OBJECT && Z_OBJCE_P(val) == php_openssl_certificate_ce) {
		cert = php_openssl_certificate_from_obj(Z_OBJ_P(val))->x509;
	} else {
		/* force it to be a string and check if it refers to a file */
		/* passing non string values leaks, object uses toString, it returns NULL
		 * See bug38255.phpt
		 */
		if (!(Z_TYPE_P(val) == IS_STRING || Z_TYPE_P(val) == IS_OBJECT)) {
			TMP_CLEAN;
		}
		if (!try_convert_to_string(val)) {
			TMP_CLEAN;
		}

		if (Z_STRLEN_P(val) > 7 && memcmp(Z_STRVAL_P(val), "file://", sizeof("file://") - 1) == 0) {
			filename = Z_STRVAL_P(val) + (sizeof("file://") - 1);
			if (php_openssl_open_base_dir_chk(filename)) {
				TMP_CLEAN;
			}
		}
		/* it's an X509 file/cert of some kind, and we need to extract the data from that */
		if (public_key) {
			cert = php_openssl_x509_from_str(Z_STR_P(val));

			if (cert) {
				free_cert = 1;
			} else {
				/* not a X509 certificate, try to retrieve public key */
				BIO* in;
				if (filename) {
					in = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
				} else {
					in = BIO_new_mem_buf(Z_STRVAL_P(val), (int)Z_STRLEN_P(val));
				}
				if (in == NULL) {
					php_openssl_store_errors();
					TMP_CLEAN;
				}
				key = PEM_read_bio_PUBKEY(in, NULL,NULL, NULL);
				BIO_free(in);
			}
		} else {
			/* we want the private key */
			BIO *in;

			if (filename) {
				in = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
			} else {
				in = BIO_new_mem_buf(Z_STRVAL_P(val), (int)Z_STRLEN_P(val));
			}

			if (in == NULL) {
				TMP_CLEAN;
			}
			if (passphrase == NULL) {
				key = PEM_read_bio_PrivateKey(in, NULL, NULL, NULL);
			} else {
				struct php_openssl_pem_password password;
				password.key = passphrase;
				password.len = passphrase_len;
				key = PEM_read_bio_PrivateKey(in, NULL, php_openssl_pem_password_cb, &password);
			}
			BIO_free(in);
		}
	}

	if (key == NULL) {
		php_openssl_store_errors();

		if (public_key && cert) {
			/* extract public key from X509 cert */
			key = (EVP_PKEY *) X509_get_pubkey(cert);
			if (key == NULL) {
				php_openssl_store_errors();
			}
		}
	}

	if (free_cert) {
		X509_free(cert);
	}

	if (Z_TYPE(tmp) == IS_STRING) {
		zval_ptr_dtor_str(&tmp);
	}

	return key;
}

/* {{{ php_openssl_generate_private_key */
static EVP_PKEY * php_openssl_generate_private_key(struct php_x509_request * req)
{
	char * randfile = NULL;
	int egdsocket, seeded;
	EVP_PKEY * return_val = NULL;

	if (req->priv_key_bits < MIN_KEY_LENGTH) {
		php_error_docref(NULL, E_WARNING, "Private key length must be at least %d bits, configured to %d",
			MIN_KEY_LENGTH, req->priv_key_bits);
		return NULL;
	}

	randfile = CONF_get_string(req->req_config, req->section_name, "RANDFILE");
	if (randfile == NULL) {
		php_openssl_store_errors();
	}
	php_openssl_load_rand_file(randfile, &egdsocket, &seeded);

	if ((req->priv_key = EVP_PKEY_new()) != NULL) {
		switch(req->priv_key_type) {
			case OPENSSL_KEYTYPE_RSA:
				{
					RSA* rsaparam;
#if OPENSSL_VERSION_NUMBER < 0x10002000L
					/* OpenSSL 1.0.2 deprecates RSA_generate_key */
					PHP_OPENSSL_RAND_ADD_TIME();
					rsaparam = (RSA*)RSA_generate_key(req->priv_key_bits, RSA_F4, NULL, NULL);
#else
					{
						BIGNUM *bne = (BIGNUM *)BN_new();
						if (BN_set_word(bne, RSA_F4) != 1) {
							BN_free(bne);
							php_error_docref(NULL, E_WARNING, "Failed setting exponent");
							return NULL;
						}
						rsaparam = RSA_new();
						PHP_OPENSSL_RAND_ADD_TIME();
						if (rsaparam == NULL || !RSA_generate_key_ex(rsaparam, req->priv_key_bits, bne, NULL)) {
							php_openssl_store_errors();
						}
						BN_free(bne);
					}
#endif
					if (rsaparam && EVP_PKEY_assign_RSA(req->priv_key, rsaparam)) {
						return_val = req->priv_key;
					} else {
						php_openssl_store_errors();
					}
				}
				break;
#if !defined(NO_DSA)
			case OPENSSL_KEYTYPE_DSA:
				PHP_OPENSSL_RAND_ADD_TIME();
				{
					DSA *dsaparam = DSA_new();
					if (dsaparam && DSA_generate_parameters_ex(dsaparam, req->priv_key_bits, NULL, 0, NULL, NULL, NULL)) {
						DSA_set_method(dsaparam, DSA_get_default_method());
						if (DSA_generate_key(dsaparam)) {
							if (EVP_PKEY_assign_DSA(req->priv_key, dsaparam)) {
								return_val = req->priv_key;
							} else {
								php_openssl_store_errors();
							}
						} else {
							php_openssl_store_errors();
							DSA_free(dsaparam);
						}
					} else {
						php_openssl_store_errors();
					}
				}
				break;
#endif
#if !defined(NO_DH)
			case OPENSSL_KEYTYPE_DH:
				PHP_OPENSSL_RAND_ADD_TIME();
				{
					int codes = 0;
					DH *dhparam = DH_new();
					if (dhparam && DH_generate_parameters_ex(dhparam, req->priv_key_bits, 2, NULL)) {
						DH_set_method(dhparam, DH_get_default_method());
						if (DH_check(dhparam, &codes) && codes == 0 && DH_generate_key(dhparam)) {
							if (EVP_PKEY_assign_DH(req->priv_key, dhparam)) {
								return_val = req->priv_key;
							} else {
								php_openssl_store_errors();
							}
						} else {
							php_openssl_store_errors();
							DH_free(dhparam);
						}
					} else {
						php_openssl_store_errors();
					}
				}
				break;
#endif
#ifdef HAVE_EVP_PKEY_EC
			case OPENSSL_KEYTYPE_EC:
				{
					EC_KEY *eckey;
					if (req->curve_name == NID_undef) {
						php_error_docref(NULL, E_WARNING, "Missing configuration value: \"curve_name\" not set");
						return NULL;
					}
					eckey = EC_KEY_new_by_curve_name(req->curve_name);
					if (eckey) {
						EC_KEY_set_asn1_flag(eckey, OPENSSL_EC_NAMED_CURVE);
						if (EC_KEY_generate_key(eckey) &&
							EVP_PKEY_assign_EC_KEY(req->priv_key, eckey)) {
							return_val = req->priv_key;
						} else {
							EC_KEY_free(eckey);
						}
					}
				}
				break;
#endif
			default:
				php_error_docref(NULL, E_WARNING, "Unsupported private key type");
		}
	} else {
		php_openssl_store_errors();
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
static int php_openssl_is_private_key(EVP_PKEY* pkey)
{
	assert(pkey != NULL);

	switch (EVP_PKEY_id(pkey)) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			{
				RSA *rsa = EVP_PKEY_get0_RSA(pkey);
				if (rsa != NULL) {
					const BIGNUM *p, *q;

					RSA_get0_factors(rsa, &p, &q);
					 if (p == NULL || q == NULL) {
						return 0;
					 }
				}
			}
			break;
		case EVP_PKEY_DSA:
		case EVP_PKEY_DSA1:
		case EVP_PKEY_DSA2:
		case EVP_PKEY_DSA3:
		case EVP_PKEY_DSA4:
			{
				DSA *dsa = EVP_PKEY_get0_DSA(pkey);
				if (dsa != NULL) {
					const BIGNUM *p, *q, *g, *pub_key, *priv_key;

					DSA_get0_pqg(dsa, &p, &q, &g);
					if (p == NULL || q == NULL) {
						return 0;
					}

					DSA_get0_key(dsa, &pub_key, &priv_key);
					if (priv_key == NULL) {
						return 0;
					}
				}
			}
			break;
		case EVP_PKEY_DH:
			{
				DH *dh = EVP_PKEY_get0_DH(pkey);
				if (dh != NULL) {
					const BIGNUM *p, *q, *g, *pub_key, *priv_key;

					DH_get0_pqg(dh, &p, &q, &g);
					if (p == NULL) {
						return 0;
					}

					DH_get0_key(dh, &pub_key, &priv_key);
					if (priv_key == NULL) {
						return 0;
					}
				}
			}
			break;
#ifdef HAVE_EVP_PKEY_EC
		case EVP_PKEY_EC:
			{
				EC_KEY *ec = EVP_PKEY_get0_EC_KEY(pkey);
				if (ec != NULL && NULL == EC_KEY_get0_private_key(ec)) {
					return 0;
				}
			}
			break;
#endif
		default:
			php_error_docref(NULL, E_WARNING, "Key type not supported in this PHP build!");
			break;
	}
	return 1;
}
/* }}} */

#define OPENSSL_GET_BN(_array, _bn, _name) do { \
		if (_bn != NULL) { \
			int len = BN_num_bytes(_bn); \
			zend_string *str = zend_string_alloc(len, 0); \
			BN_bn2bin(_bn, (unsigned char*)ZSTR_VAL(str)); \
			ZSTR_VAL(str)[len] = 0; \
			add_assoc_str(&_array, #_name, str); \
		} \
	} while (0);

#define OPENSSL_PKEY_GET_BN(_type, _name) OPENSSL_GET_BN(_type, _name, _name)

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

/* {{{ php_openssl_pkey_init_rsa */
static zend_bool php_openssl_pkey_init_and_assign_rsa(EVP_PKEY *pkey, RSA *rsa, zval *data)
{
	BIGNUM *n, *e, *d, *p, *q, *dmp1, *dmq1, *iqmp;

	OPENSSL_PKEY_SET_BN(data, n);
	OPENSSL_PKEY_SET_BN(data, e);
	OPENSSL_PKEY_SET_BN(data, d);
	if (!n || !d || !RSA_set0_key(rsa, n, e, d)) {
		return 0;
	}

	OPENSSL_PKEY_SET_BN(data, p);
	OPENSSL_PKEY_SET_BN(data, q);
	if ((p || q) && !RSA_set0_factors(rsa, p, q)) {
		return 0;
	}

	OPENSSL_PKEY_SET_BN(data, dmp1);
	OPENSSL_PKEY_SET_BN(data, dmq1);
	OPENSSL_PKEY_SET_BN(data, iqmp);
	if ((dmp1 || dmq1 || iqmp) && !RSA_set0_crt_params(rsa, dmp1, dmq1, iqmp)) {
		return 0;
	}

	if (!EVP_PKEY_assign_RSA(pkey, rsa)) {
		php_openssl_store_errors();
		return 0;
	}

	return 1;
}

/* {{{ php_openssl_pkey_init_dsa */
static zend_bool php_openssl_pkey_init_dsa(DSA *dsa, zval *data)
{
	BIGNUM *p, *q, *g, *priv_key, *pub_key;
	const BIGNUM *priv_key_const, *pub_key_const;

	OPENSSL_PKEY_SET_BN(data, p);
	OPENSSL_PKEY_SET_BN(data, q);
	OPENSSL_PKEY_SET_BN(data, g);
	if (!p || !q || !g || !DSA_set0_pqg(dsa, p, q, g)) {
		return 0;
	}

	OPENSSL_PKEY_SET_BN(data, pub_key);
	OPENSSL_PKEY_SET_BN(data, priv_key);
	if (pub_key) {
		return DSA_set0_key(dsa, pub_key, priv_key);
	}

	/* generate key */
	PHP_OPENSSL_RAND_ADD_TIME();
	if (!DSA_generate_key(dsa)) {
		php_openssl_store_errors();
		return 0;
	}

	/* if BN_mod_exp return -1, then DSA_generate_key succeed for failed key
	 * so we need to double check that public key is created */
	DSA_get0_key(dsa, &pub_key_const, &priv_key_const);
	if (!pub_key_const || BN_is_zero(pub_key_const)) {
		return 0;
	}
	/* all good */
	return 1;
}
/* }}} */

/* {{{ php_openssl_dh_pub_from_priv */
static BIGNUM *php_openssl_dh_pub_from_priv(BIGNUM *priv_key, BIGNUM *g, BIGNUM *p)
{
	BIGNUM *pub_key, *priv_key_const_time;
	BN_CTX *ctx;

	pub_key = BN_new();
	if (pub_key == NULL) {
		php_openssl_store_errors();
		return NULL;
	}

	priv_key_const_time = BN_new();
	if (priv_key_const_time == NULL) {
		BN_free(pub_key);
		php_openssl_store_errors();
		return NULL;
	}
	ctx = BN_CTX_new();
	if (ctx == NULL) {
		BN_free(pub_key);
		BN_free(priv_key_const_time);
		php_openssl_store_errors();
		return NULL;
	}

	BN_with_flags(priv_key_const_time, priv_key, BN_FLG_CONSTTIME);

	if (!BN_mod_exp_mont(pub_key, g, priv_key_const_time, p, ctx, NULL)) {
		BN_free(pub_key);
		php_openssl_store_errors();
		pub_key = NULL;
	}

	BN_free(priv_key_const_time);
	BN_CTX_free(ctx);

	return pub_key;
}
/* }}} */

/* {{{ php_openssl_pkey_init_dh */
static zend_bool php_openssl_pkey_init_dh(DH *dh, zval *data)
{
	BIGNUM *p, *q, *g, *priv_key, *pub_key;

	OPENSSL_PKEY_SET_BN(data, p);
	OPENSSL_PKEY_SET_BN(data, q);
	OPENSSL_PKEY_SET_BN(data, g);
	if (!p || !g || !DH_set0_pqg(dh, p, q, g)) {
		return 0;
	}

	OPENSSL_PKEY_SET_BN(data, priv_key);
	OPENSSL_PKEY_SET_BN(data, pub_key);
	if (pub_key) {
		return DH_set0_key(dh, pub_key, priv_key);
	}
	if (priv_key) {
		pub_key = php_openssl_dh_pub_from_priv(priv_key, g, p);
		if (pub_key == NULL) {
			return 0;
		}
		return DH_set0_key(dh, pub_key, priv_key);
	}

	/* generate key */
	PHP_OPENSSL_RAND_ADD_TIME();
	if (!DH_generate_key(dh)) {
		php_openssl_store_errors();
		return 0;
	}
	/* all good */
	return 1;
}
/* }}} */

/* {{{ Generates a new private key */
PHP_FUNCTION(openssl_pkey_new)
{
	struct php_x509_request req;
	zval * args = NULL;
	zval *data;
	php_openssl_pkey_object *key_object;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a!", &args) == FAILURE) {
		RETURN_THROWS();
	}
	RETVAL_FALSE;

	if (args && Z_TYPE_P(args) == IS_ARRAY) {
		EVP_PKEY *pkey;

		if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "rsa", sizeof("rsa")-1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			pkey = EVP_PKEY_new();
			if (pkey) {
				RSA *rsa = RSA_new();
				if (rsa) {
					if (php_openssl_pkey_init_and_assign_rsa(pkey, rsa, data)) {
						object_init_ex(return_value, php_openssl_pkey_ce);
						key_object = Z_OPENSSL_PKEY_P(return_value);
						key_object->pkey = pkey;
						return;
					}
					RSA_free(rsa);
				} else {
					php_openssl_store_errors();
				}
				EVP_PKEY_free(pkey);
			} else {
				php_openssl_store_errors();
			}
			RETURN_FALSE;
		} else if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "dsa", sizeof("dsa") - 1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			pkey = EVP_PKEY_new();
			if (pkey) {
				DSA *dsa = DSA_new();
				if (dsa) {
					if (php_openssl_pkey_init_dsa(dsa, data)) {
						if (EVP_PKEY_assign_DSA(pkey, dsa)) {
							object_init_ex(return_value, php_openssl_pkey_ce);
							key_object = Z_OPENSSL_PKEY_P(return_value);
							key_object->pkey = pkey;
							return;
						} else {
							php_openssl_store_errors();
						}
					}
					DSA_free(dsa);
				} else {
					php_openssl_store_errors();
				}
				EVP_PKEY_free(pkey);
			} else {
				php_openssl_store_errors();
			}
			RETURN_FALSE;
		} else if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "dh", sizeof("dh") - 1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			pkey = EVP_PKEY_new();
			if (pkey) {
				DH *dh = DH_new();
				if (dh) {
					if (php_openssl_pkey_init_dh(dh, data)) {
						if (EVP_PKEY_assign_DH(pkey, dh)) {
							php_openssl_pkey_object *key_object;

							object_init_ex(return_value, php_openssl_pkey_ce);
							key_object = Z_OPENSSL_PKEY_P(return_value);
							key_object->pkey = pkey;
							return;
						} else {
							php_openssl_store_errors();
						}
					}
					DH_free(dh);
				} else {
					php_openssl_store_errors();
				}
				EVP_PKEY_free(pkey);
			} else {
				php_openssl_store_errors();
			}
			RETURN_FALSE;
#ifdef HAVE_EVP_PKEY_EC
		} else if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "ec", sizeof("ec") - 1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			EC_KEY *eckey = NULL;
			EC_GROUP *group = NULL;
			EC_POINT *pnt = NULL;
			BIGNUM *d = NULL;
			pkey = EVP_PKEY_new();
			if (pkey) {
				eckey = EC_KEY_new();
				if (eckey) {
					EC_GROUP *group = NULL;
					zval *bn;
					zval *x;
					zval *y;

					if ((bn = zend_hash_str_find(Z_ARRVAL_P(data), "curve_name", sizeof("curve_name") - 1)) != NULL &&
							Z_TYPE_P(bn) == IS_STRING) {
						int nid = OBJ_sn2nid(Z_STRVAL_P(bn));
						if (nid != NID_undef) {
							group = EC_GROUP_new_by_curve_name(nid);
							if (!group) {
								php_openssl_store_errors();
								goto clean_exit;
							}
							EC_GROUP_set_asn1_flag(group, OPENSSL_EC_NAMED_CURVE);
							EC_GROUP_set_point_conversion_form(group, POINT_CONVERSION_UNCOMPRESSED);
							if (!EC_KEY_set_group(eckey, group)) {
								php_openssl_store_errors();
								goto clean_exit;
							}
						}
					}

					if (group == NULL) {
						php_error_docref(NULL, E_WARNING, "Unknown curve name");
						goto clean_exit;
					}

					// The public key 'pnt' can be calculated from 'd' or is defined by 'x' and 'y'
					if ((bn = zend_hash_str_find(Z_ARRVAL_P(data), "d", sizeof("d") - 1)) != NULL &&
							Z_TYPE_P(bn) == IS_STRING) {
						d = BN_bin2bn((unsigned char*) Z_STRVAL_P(bn), Z_STRLEN_P(bn), NULL);
						if (!EC_KEY_set_private_key(eckey, d)) {
							php_openssl_store_errors();
							goto clean_exit;
						}
						// Calculate the public key by multiplying the Point Q with the public key
						// P = d * Q
						pnt = EC_POINT_new(group);
						if (!pnt || !EC_POINT_mul(group, pnt, d, NULL, NULL, NULL)) {
							php_openssl_store_errors();
							goto clean_exit;
						}

						BN_free(d);
					} else if ((x = zend_hash_str_find(Z_ARRVAL_P(data), "x", sizeof("x") - 1)) != NULL &&
							Z_TYPE_P(x) == IS_STRING &&
							(y = zend_hash_str_find(Z_ARRVAL_P(data), "y", sizeof("y") - 1)) != NULL &&
							Z_TYPE_P(y) == IS_STRING) {
						pnt = EC_POINT_new(group);
						if (pnt == NULL) {
							php_openssl_store_errors();
							goto clean_exit;
						}
						if (!EC_POINT_set_affine_coordinates_GFp(
								group, pnt, BN_bin2bn((unsigned char*) Z_STRVAL_P(x), Z_STRLEN_P(x), NULL),
								BN_bin2bn((unsigned char*) Z_STRVAL_P(y), Z_STRLEN_P(y), NULL), NULL)) {
							php_openssl_store_errors();
							goto clean_exit;
						}
					}

					if (pnt != NULL) {
						if (!EC_KEY_set_public_key(eckey, pnt)) {
							php_openssl_store_errors();
							goto clean_exit;
						}
						EC_POINT_free(pnt);
						pnt = NULL;
					}

					if (!EC_KEY_check_key(eckey)) {
						PHP_OPENSSL_RAND_ADD_TIME();
						EC_KEY_generate_key(eckey);
						php_openssl_store_errors();
					}
					if (EC_KEY_check_key(eckey) && EVP_PKEY_assign_EC_KEY(pkey, eckey)) {
						EC_GROUP_free(group);

						object_init_ex(return_value, php_openssl_pkey_ce);
						key_object = Z_OPENSSL_PKEY_P(return_value);
						key_object->pkey = pkey;
						return;
					} else {
						php_openssl_store_errors();
					}
				} else {
					php_openssl_store_errors();
				}
			} else {
				php_openssl_store_errors();
			}
clean_exit:
			if (d != NULL) {
				BN_free(d);
			}
			if (pnt != NULL) {
				EC_POINT_free(pnt);
			}
			if (group != NULL) {
				EC_GROUP_free(group);
			}
			if (eckey != NULL) {
				EC_KEY_free(eckey);
			}
			EVP_PKEY_free(pkey);
			RETURN_FALSE;
#endif
		}
	}

	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS) {
		if (php_openssl_generate_private_key(&req)) {
			/* pass back a key resource */
			object_init_ex(return_value, php_openssl_pkey_ce);
			key_object = Z_OPENSSL_PKEY_P(return_value);
			key_object->pkey = req.priv_key;
			/* make sure the cleanup code doesn't zap it! */
			req.priv_key = NULL;
		}
	}
	PHP_SSL_REQ_DISPOSE(&req);
}
/* }}} */

/* {{{ Gets an exportable representation of a key into a file */
PHP_FUNCTION(openssl_pkey_export_to_file)
{
	struct php_x509_request req;
	zval * zpkey, * args = NULL;
	char * passphrase = NULL;
	size_t passphrase_len = 0;
	char * filename = NULL;
	size_t filename_len = 0;
	int pem_write = 0;
	EVP_PKEY * key;
	BIO * bio_out = NULL;
	const EVP_CIPHER * cipher;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zp|s!a!", &zpkey, &filename, &filename_len, &passphrase, &passphrase_len, &args) == FAILURE) {
		RETURN_THROWS();
	}
	RETVAL_FALSE;

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(passphrase_len, passphrase, 3);

	key = php_openssl_pkey_from_zval(zpkey, 0, passphrase, passphrase_len);
	if (key == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Cannot get key from parameter 1");
		}
		RETURN_FALSE;
	}

	if (php_openssl_open_base_dir_chk(filename)) {
		RETURN_FALSE;
	}

	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS) {
		bio_out = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
		if (bio_out == NULL) {
			php_openssl_store_errors();
			goto clean_exit;
		}

		if (passphrase && req.priv_key_encrypt) {
			if (req.priv_key_encrypt_cipher) {
				cipher = req.priv_key_encrypt_cipher;
			} else {
				cipher = (EVP_CIPHER *) EVP_des_ede3_cbc();
			}
		} else {
			cipher = NULL;
		}

		switch (EVP_PKEY_base_id(key)) {
#ifdef HAVE_EVP_PKEY_EC
			case EVP_PKEY_EC:
				pem_write = PEM_write_bio_ECPrivateKey(
						bio_out, EVP_PKEY_get0_EC_KEY(key), cipher,
						(unsigned char *)passphrase, (int)passphrase_len, NULL, NULL);
				break;
#endif
			default:
				pem_write = PEM_write_bio_PrivateKey(
						bio_out, key, cipher,
						(unsigned char *)passphrase, (int)passphrase_len, NULL, NULL);
				break;
		}

		if (pem_write) {
			/* Success!
			 * If returning the output as a string, do so now */
			RETVAL_TRUE;
		} else {
			php_openssl_store_errors();
		}
	}

clean_exit:
	PHP_SSL_REQ_DISPOSE(&req);
	EVP_PKEY_free(key);
	BIO_free(bio_out);
}
/* }}} */

/* {{{ Gets an exportable representation of a key into a string or file */
PHP_FUNCTION(openssl_pkey_export)
{
	struct php_x509_request req;
	zval * zpkey, * args = NULL, *out;
	char * passphrase = NULL; size_t passphrase_len = 0;
	int pem_write = 0;
	EVP_PKEY * key;
	BIO * bio_out = NULL;
	const EVP_CIPHER * cipher;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz|s!a!", &zpkey, &out, &passphrase, &passphrase_len, &args) == FAILURE) {
		RETURN_THROWS();
	}
	RETVAL_FALSE;

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(passphrase_len, passphrase, 3);

	key = php_openssl_pkey_from_zval(zpkey, 0, passphrase, passphrase_len);
	if (key == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Cannot get key from parameter 1");
		}
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

		switch (EVP_PKEY_base_id(key)) {
#ifdef HAVE_EVP_PKEY_EC
			case EVP_PKEY_EC:
				pem_write = PEM_write_bio_ECPrivateKey(
						bio_out, EVP_PKEY_get0_EC_KEY(key), cipher,
						(unsigned char *)passphrase, (int)passphrase_len, NULL, NULL);
				break;
#endif
			default:
				pem_write = PEM_write_bio_PrivateKey(
						bio_out, key, cipher,
						(unsigned char *)passphrase, (int)passphrase_len, NULL, NULL);
				break;
		}

		if (pem_write) {
			/* Success!
			 * If returning the output as a string, do so now */

			char * bio_mem_ptr;
			long bio_mem_len;
			RETVAL_TRUE;

			bio_mem_len = BIO_get_mem_data(bio_out, &bio_mem_ptr);
			ZEND_TRY_ASSIGN_REF_STRINGL(out, bio_mem_ptr, bio_mem_len);
		} else {
			php_openssl_store_errors();
		}
	}
	PHP_SSL_REQ_DISPOSE(&req);
	EVP_PKEY_free(key);
	BIO_free(bio_out);
}
/* }}} */

/* {{{ Gets public key from X.509 certificate */
PHP_FUNCTION(openssl_pkey_get_public)
{
	zval *cert;
	EVP_PKEY *pkey;
	php_openssl_pkey_object *key_object;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &cert) == FAILURE) {
		RETURN_THROWS();
	}
	pkey = php_openssl_pkey_from_zval(cert, 1, NULL, 0);
	if (pkey == NULL) {
		RETURN_FALSE;
	}

	object_init_ex(return_value, php_openssl_pkey_ce);
	key_object = Z_OPENSSL_PKEY_P(return_value);
	key_object->pkey = pkey;
}
/* }}} */

/* {{{ Frees a key */
PHP_FUNCTION(openssl_pkey_free)
{
	zval *key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &key, php_openssl_pkey_ce) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Gets private keys */
PHP_FUNCTION(openssl_pkey_get_private)
{
	zval *cert;
	EVP_PKEY *pkey;
	char * passphrase = "";
	size_t passphrase_len = sizeof("")-1;
	php_openssl_pkey_object *key_object;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|s!", &cert, &passphrase, &passphrase_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (passphrase) {
		PHP_OPENSSL_CHECK_SIZE_T_TO_INT(passphrase_len, passphrase, 2);
	}

	pkey = php_openssl_pkey_from_zval(cert, 0, passphrase, passphrase_len);
	if (pkey == NULL) {
		RETURN_FALSE;
	}

	object_init_ex(return_value, php_openssl_pkey_ce);
	key_object = Z_OPENSSL_PKEY_P(return_value);
	key_object->pkey = pkey;
}

/* }}} */

/* {{{ returns an array with the key details (bits, pkey, type)*/
PHP_FUNCTION(openssl_pkey_get_details)
{
	zval *key;
	EVP_PKEY *pkey;
	BIO *out;
	unsigned int pbio_len;
	char *pbio;
	zend_long ktype;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &key, php_openssl_pkey_ce) == FAILURE) {
		RETURN_THROWS();
	}

	pkey = Z_OPENSSL_PKEY_P(key)->pkey;

	out = BIO_new(BIO_s_mem());
	if (!PEM_write_bio_PUBKEY(out, pkey)) {
		BIO_free(out);
		php_openssl_store_errors();
		RETURN_FALSE;
	}
	pbio_len = BIO_get_mem_data(out, &pbio);

	array_init(return_value);
	add_assoc_long(return_value, "bits", EVP_PKEY_bits(pkey));
	add_assoc_stringl(return_value, "key", pbio, pbio_len);
	/*TODO: Use the real values once the openssl constants are used
	 * See the enum at the top of this file
	 */
	switch (EVP_PKEY_base_id(pkey)) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			{
				RSA *rsa = EVP_PKEY_get0_RSA(pkey);
				ktype = OPENSSL_KEYTYPE_RSA;

				if (rsa != NULL) {
					zval z_rsa;
					const BIGNUM *n, *e, *d, *p, *q, *dmp1, *dmq1, *iqmp;

					RSA_get0_key(rsa, &n, &e, &d);
					RSA_get0_factors(rsa, &p, &q);
					RSA_get0_crt_params(rsa, &dmp1, &dmq1, &iqmp);

					array_init(&z_rsa);
					OPENSSL_PKEY_GET_BN(z_rsa, n);
					OPENSSL_PKEY_GET_BN(z_rsa, e);
					OPENSSL_PKEY_GET_BN(z_rsa, d);
					OPENSSL_PKEY_GET_BN(z_rsa, p);
					OPENSSL_PKEY_GET_BN(z_rsa, q);
					OPENSSL_PKEY_GET_BN(z_rsa, dmp1);
					OPENSSL_PKEY_GET_BN(z_rsa, dmq1);
					OPENSSL_PKEY_GET_BN(z_rsa, iqmp);
					add_assoc_zval(return_value, "rsa", &z_rsa);
				}
			}
			break;
		case EVP_PKEY_DSA:
		case EVP_PKEY_DSA2:
		case EVP_PKEY_DSA3:
		case EVP_PKEY_DSA4:
			{
				DSA *dsa = EVP_PKEY_get0_DSA(pkey);
				ktype = OPENSSL_KEYTYPE_DSA;

				if (dsa != NULL) {
					zval z_dsa;
					const BIGNUM *p, *q, *g, *priv_key, *pub_key;

					DSA_get0_pqg(dsa, &p, &q, &g);
					DSA_get0_key(dsa, &pub_key, &priv_key);

					array_init(&z_dsa);
					OPENSSL_PKEY_GET_BN(z_dsa, p);
					OPENSSL_PKEY_GET_BN(z_dsa, q);
					OPENSSL_PKEY_GET_BN(z_dsa, g);
					OPENSSL_PKEY_GET_BN(z_dsa, priv_key);
					OPENSSL_PKEY_GET_BN(z_dsa, pub_key);
					add_assoc_zval(return_value, "dsa", &z_dsa);
				}
			}
			break;
		case EVP_PKEY_DH:
			{
				DH *dh = EVP_PKEY_get0_DH(pkey);
				ktype = OPENSSL_KEYTYPE_DH;

				if (dh != NULL) {
					zval z_dh;
					const BIGNUM *p, *q, *g, *priv_key, *pub_key;

					DH_get0_pqg(dh, &p, &q, &g);
					DH_get0_key(dh, &pub_key, &priv_key);

					array_init(&z_dh);
					OPENSSL_PKEY_GET_BN(z_dh, p);
					OPENSSL_PKEY_GET_BN(z_dh, g);
					OPENSSL_PKEY_GET_BN(z_dh, priv_key);
					OPENSSL_PKEY_GET_BN(z_dh, pub_key);
					add_assoc_zval(return_value, "dh", &z_dh);
				}
			}
			break;
#ifdef HAVE_EVP_PKEY_EC
		case EVP_PKEY_EC:
			ktype = OPENSSL_KEYTYPE_EC;
			if (EVP_PKEY_get0_EC_KEY(pkey) != NULL) {
				zval ec;
				const EC_GROUP *ec_group;
				const EC_POINT *pub;
				int nid;
				char *crv_sn;
				ASN1_OBJECT *obj;
				// openssl recommends a buffer length of 80
				char oir_buf[80];
				const EC_KEY *ec_key = EVP_PKEY_get0_EC_KEY(pkey);
				BIGNUM *x = BN_new();
				BIGNUM *y = BN_new();
				const BIGNUM *d;

				ec_group = EC_KEY_get0_group(ec_key);

				// Curve nid (numerical identifier) used for ASN1 mapping
				nid = EC_GROUP_get_curve_name(ec_group);
				if (nid == NID_undef) {
					break;
				}
				array_init(&ec);

				// Short object name
				crv_sn = (char*) OBJ_nid2sn(nid);
				if (crv_sn != NULL) {
					add_assoc_string(&ec, "curve_name", crv_sn);
				}

				obj = OBJ_nid2obj(nid);
				if (obj != NULL) {
					int oir_len = OBJ_obj2txt(oir_buf, sizeof(oir_buf), obj, 1);
					add_assoc_stringl(&ec, "curve_oid", (char*) oir_buf, oir_len);
					ASN1_OBJECT_free(obj);
				}

				pub = EC_KEY_get0_public_key(ec_key);

				if (EC_POINT_get_affine_coordinates_GFp(ec_group, pub, x, y, NULL)) {
					OPENSSL_GET_BN(ec, x, x);
					OPENSSL_GET_BN(ec, y, y);
				} else {
					php_openssl_store_errors();
				}

				if ((d = EC_KEY_get0_private_key(EVP_PKEY_get0_EC_KEY(pkey))) != NULL) {
					OPENSSL_GET_BN(ec, d, d);
				}

				add_assoc_zval(return_value, "ec", &ec);

				BN_free(x);
				BN_free(y);
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

/* {{{ Computes shared secret for public value of remote DH key and local DH key */
PHP_FUNCTION(openssl_dh_compute_key)
{
	zval *key;
	char *pub_str;
	size_t pub_len;
	DH *dh;
	EVP_PKEY *pkey;
	BIGNUM *pub;
	zend_string *data;
	int len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sO", &pub_str, &pub_len, &key, php_openssl_pkey_ce) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(pub_len, pub_key, 1);

	pkey = Z_OPENSSL_PKEY_P(key)->pkey;

	if (EVP_PKEY_base_id(pkey) != EVP_PKEY_DH) {
		RETURN_FALSE;
	}
	dh = EVP_PKEY_get0_DH(pkey);
	if (dh == NULL) {
		RETURN_FALSE;
	}

	pub = BN_bin2bn((unsigned char*)pub_str, (int)pub_len, NULL);

	data = zend_string_alloc(DH_size(dh), 0);
	len = DH_compute_key((unsigned char*)ZSTR_VAL(data), pub, dh);

	if (len >= 0) {
		ZSTR_LEN(data) = len;
		ZSTR_VAL(data)[len] = 0;
		RETVAL_NEW_STR(data);
	} else {
		php_openssl_store_errors();
		zend_string_release_ex(data, 0);
		RETVAL_FALSE;
	}

	BN_free(pub);
}
/* }}} */

/* {{{ Computes shared secret for public value of remote and local DH or ECDH key */
PHP_FUNCTION(openssl_pkey_derive)
{
	zval *priv_key;
	zval *peer_pub_key;
	EVP_PKEY *pkey = NULL;
	EVP_PKEY *peer_key = NULL;
	EVP_PKEY_CTX *ctx = NULL;
	size_t key_size;
	zend_long key_len = 0;
	zend_string *result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz|l", &peer_pub_key, &priv_key, &key_len) == FAILURE) {
		RETURN_THROWS();
	}

	RETVAL_FALSE;
	if (key_len < 0) {
		zend_argument_value_error(3, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	key_size = key_len;
	pkey = php_openssl_pkey_from_zval(priv_key, 0, "", 0);
	if (!pkey) {
		goto cleanup;
	}

	peer_key = php_openssl_pkey_from_zval(peer_pub_key, 1, NULL, 0);
	if (!peer_key) {
		goto cleanup;
	}

	ctx = EVP_PKEY_CTX_new(pkey, NULL);
	if (!ctx) {
		goto cleanup;
	}

	if (EVP_PKEY_derive_init(ctx) > 0
		&& EVP_PKEY_derive_set_peer(ctx, peer_key) > 0
		&& (key_size > 0 || EVP_PKEY_derive(ctx, NULL, &key_size) > 0)
		&& (result = zend_string_alloc(key_size, 0)) != NULL) {
		if (EVP_PKEY_derive(ctx, (unsigned char*)ZSTR_VAL(result), &key_size) > 0) {
			ZSTR_LEN(result) = key_size;
			ZSTR_VAL(result)[key_size] = 0;
			RETVAL_NEW_STR(result);
		} else {
			php_openssl_store_errors();
			zend_string_release_ex(result, 0);
			RETVAL_FALSE;
		}
	}

cleanup:
	EVP_PKEY_free(pkey);
	EVP_PKEY_free(peer_key);
	if (ctx) {
		EVP_PKEY_CTX_free(ctx);
	}
}
/* }}} */


/* {{{ Generates a PKCS5 v2 PBKDF2 string, defaults to sha1 */
PHP_FUNCTION(openssl_pbkdf2)
{
	zend_long key_length = 0, iterations = 0;
	char *password;
	size_t password_len;
	char *salt;
	size_t salt_len;
	char *method;
	size_t method_len = 0;
	zend_string *out_buffer;

	const EVP_MD *digest;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ssll|s",
				&password, &password_len,
				&salt, &salt_len,
				&key_length, &iterations,
				&method, &method_len) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(password_len, password, 1);
	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(salt_len, salt, 2);
	PHP_OPENSSL_CHECK_LONG_TO_INT(key_length, key, 3);
	PHP_OPENSSL_CHECK_LONG_TO_INT(iterations, iterations, 4);

	if (key_length <= 0) {
		zend_argument_value_error(3, "must be greater than 0");
		RETURN_THROWS();
	}

	if (method_len) {
		digest = EVP_get_digestbyname(method);
	} else {
		digest = EVP_sha1();
	}

	if (!digest) {
		php_error_docref(NULL, E_WARNING, "Unknown digest algorithm");
		RETURN_FALSE;
	}

	out_buffer = zend_string_alloc(key_length, 0);

	if (PKCS5_PBKDF2_HMAC(password, (int)password_len, (unsigned char *)salt, (int)salt_len, (int)iterations, digest, (int)key_length, (unsigned char*)ZSTR_VAL(out_buffer)) == 1) {
		ZSTR_VAL(out_buffer)[key_length] = 0;
		RETURN_NEW_STR(out_buffer);
	} else {
		php_openssl_store_errors();
		zend_string_release_ex(out_buffer, 0);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ PKCS7 S/MIME functions */

/* {{{ Verifys that the data block is intact, the signer is who they say they are, and returns the CERTs of the signers */
PHP_FUNCTION(openssl_pkcs7_verify)
{
	X509_STORE * store = NULL;
	zval * cainfo = NULL;
	STACK_OF(X509) *signers= NULL;
	STACK_OF(X509) *others = NULL;
	PKCS7 * p7 = NULL;
	BIO * in = NULL, * datain = NULL, * dataout = NULL, * p7bout  = NULL;
	zend_long flags = 0;
	char * filename;
	size_t filename_len;
	char * extracerts = NULL;
	size_t extracerts_len = 0;
	char * signersfilename = NULL;
	size_t signersfilename_len = 0;
	char * datafilename = NULL;
	size_t datafilename_len = 0;
	char * p7bfilename = NULL;
	size_t p7bfilename_len = 0;

	RETVAL_LONG(-1);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "pl|p!ap!p!p!", &filename, &filename_len,
				&flags, &signersfilename, &signersfilename_len, &cainfo,
				&extracerts, &extracerts_len, &datafilename, &datafilename_len, &p7bfilename, &p7bfilename_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (extracerts) {
		others = php_openssl_load_all_certs_from_file(extracerts);
		if (others == NULL) {
			goto clean_exit;
		}
	}

	flags = flags & ~PKCS7_DETACHED;

	store = php_openssl_setup_verify(cainfo);

	if (!store) {
		goto clean_exit;
	}
	if (php_openssl_open_base_dir_chk(filename)) {
		goto clean_exit;
	}

	in = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_R(flags));
	if (in == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}
	p7 = SMIME_read_PKCS7(in, &datain);
	if (p7 == NULL) {
#if DEBUG_SMIME
		zend_printf("SMIME_read_PKCS7 failed\n");
#endif
		php_openssl_store_errors();
		goto clean_exit;
	}
	if (datafilename) {
		if (php_openssl_open_base_dir_chk(datafilename)) {
			goto clean_exit;
		}
		dataout = BIO_new_file(datafilename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
		if (dataout == NULL) {
			php_openssl_store_errors();
			goto clean_exit;
		}
	}
	if (p7bfilename) {
		if (php_openssl_open_base_dir_chk(p7bfilename)) {
			goto clean_exit;
		}

		p7bout = BIO_new_file(p7bfilename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
		if (p7bout == NULL) {
			php_openssl_store_errors();
			goto clean_exit;
		}
	}
#if DEBUG_SMIME
	zend_printf("Calling PKCS7 verify\n");
#endif

	if (PKCS7_verify(p7, others, store, datain, dataout, (int)flags)) {

		RETVAL_TRUE;

		if (signersfilename) {
			BIO *certout;

			if (php_openssl_open_base_dir_chk(signersfilename)) {
				goto clean_exit;
			}

			certout = BIO_new_file(signersfilename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
			if (certout) {
				int i;
				signers = PKCS7_get0_signers(p7, NULL, (int)flags);
				if (signers != NULL) {

					for (i = 0; i < sk_X509_num(signers); i++) {
						if (!PEM_write_bio_X509(certout, sk_X509_value(signers, i))) {
							php_openssl_store_errors();
							RETVAL_LONG(-1);
							php_error_docref(NULL, E_WARNING, "Failed to write signer %d", i);
						}
					}

					sk_X509_free(signers);
				} else {
					RETVAL_LONG(-1);
					php_openssl_store_errors();
				}

				BIO_free(certout);
			} else {
				php_openssl_store_errors();
				php_error_docref(NULL, E_WARNING, "Signature OK, but cannot open %s for writing", signersfilename);
				RETVAL_LONG(-1);
			}

			if (p7bout) {
				PEM_write_bio_PKCS7(p7bout, p7);
			}
		}
	} else {
		php_openssl_store_errors();
		RETVAL_FALSE;
	}
clean_exit:
	if (p7bout) {
		BIO_free(p7bout);
	}
	X509_STORE_free(store);
	BIO_free(datain);
	BIO_free(in);
	BIO_free(dataout);
	PKCS7_free(p7);
	sk_X509_pop_free(others, X509_free);
}
/* }}} */

/* {{{ Encrypts the message in the file named infile with the certificates in recipcerts and output the result to the file named outfile */
PHP_FUNCTION(openssl_pkcs7_encrypt)
{
	zval * zrecipcerts, * zheaders = NULL;
	STACK_OF(X509) * recipcerts = NULL;
	BIO * infile = NULL, * outfile = NULL;
	zend_long flags = 0;
	PKCS7 * p7 = NULL;
	zval * zcertval;
	X509 * cert;
	const EVP_CIPHER *cipher = NULL;
	zend_long cipherid = PHP_OPENSSL_CIPHER_DEFAULT;
	zend_string * strindex;
	char * infilename = NULL;
	size_t infilename_len;
	char * outfilename = NULL;
	size_t outfilename_len;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ppza!|ll", &infilename, &infilename_len,
				&outfilename, &outfilename_len, &zrecipcerts, &zheaders, &flags, &cipherid) == FAILURE) {
		RETURN_THROWS();
	}

	if (php_openssl_open_base_dir_chk(infilename) || php_openssl_open_base_dir_chk(outfilename)) {
		return;
	}

	infile = BIO_new_file(infilename, PHP_OPENSSL_BIO_MODE_R(flags));
	if (infile == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	outfile = BIO_new_file(outfilename, PHP_OPENSSL_BIO_MODE_W(flags));
	if (outfile == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	recipcerts = sk_X509_new_null();

	/* get certs */
	if (Z_TYPE_P(zrecipcerts) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zrecipcerts), zcertval) {
			bool free_cert;

			cert = php_openssl_x509_from_zval(zcertval, &free_cert);
			if (cert == NULL) {
				// TODO Add warning?
				goto clean_exit;
			}

			if (!free_cert) {
				/* we shouldn't free this particular cert, as it is a resource.
					make a copy and push that on the stack instead */
				cert = X509_dup(cert);
				if (cert == NULL) {
					php_openssl_store_errors();
					goto clean_exit;
				}
			}
			sk_X509_push(recipcerts, cert);
		} ZEND_HASH_FOREACH_END();
	} else {
		/* a single certificate */
		bool free_cert;

		cert = php_openssl_x509_from_zval(zrecipcerts, &free_cert);
		if (cert == NULL) {
			// TODO Add warning?
			goto clean_exit;
		}

		if (!free_cert) {
			/* we shouldn't free this particular cert, as it is a resource.
				make a copy and push that on the stack instead */
			cert = X509_dup(cert);
			if (cert == NULL) {
				php_openssl_store_errors();
				goto clean_exit;
			}
		}
		sk_X509_push(recipcerts, cert);
	}

	/* sanity check the cipher */
	cipher = php_openssl_get_evp_cipher_from_algo(cipherid);
	if (cipher == NULL) {
		/* shouldn't happen */
		php_error_docref(NULL, E_WARNING, "Failed to get cipher");
		goto clean_exit;
	}

	p7 = PKCS7_encrypt(recipcerts, infile, (EVP_CIPHER*)cipher, (int)flags);

	if (p7 == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	/* tack on extra headers */
	if (zheaders) {
		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(zheaders), strindex, zcertval) {
			zend_string *str = zval_try_get_string(zcertval);
			if (UNEXPECTED(!str)) {
				goto clean_exit;
			}
			if (strindex) {
				BIO_printf(outfile, "%s: %s\n", ZSTR_VAL(strindex), ZSTR_VAL(str));
			} else {
				BIO_printf(outfile, "%s\n", ZSTR_VAL(str));
			}
			zend_string_release(str);
		} ZEND_HASH_FOREACH_END();
	}

	(void)BIO_reset(infile);

	/* write the encrypted data */
	if (!SMIME_write_PKCS7(outfile, p7, infile, (int)flags)) {
		php_openssl_store_errors();
		goto clean_exit;
	}

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

/* {{{ Exports the PKCS7 file to an array of PEM certificates */
PHP_FUNCTION(openssl_pkcs7_read)
{
	zval * zout = NULL, zcert;
	char *p7b;
	size_t p7b_len;
	STACK_OF(X509) *certs = NULL;
	STACK_OF(X509_CRL) *crls = NULL;
	BIO * bio_in = NULL, * bio_out = NULL;
	PKCS7 * p7 = NULL;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz", &p7b, &p7b_len,
				&zout) == FAILURE) {
		RETURN_THROWS();
	}

	RETVAL_FALSE;

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(p7b_len, p7b, 1);

	bio_in = BIO_new(BIO_s_mem());
	if (bio_in == NULL) {
		goto clean_exit;
	}

	if (0 >= BIO_write(bio_in, p7b, (int)p7b_len)) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	p7 = PEM_read_bio_PKCS7(bio_in, NULL, NULL, NULL);
	if (p7 == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	switch (OBJ_obj2nid(p7->type)) {
		case NID_pkcs7_signed:
			if (p7->d.sign != NULL) {
				certs = p7->d.sign->cert;
				crls = p7->d.sign->crl;
			}
			break;
		case NID_pkcs7_signedAndEnveloped:
			if (p7->d.signed_and_enveloped != NULL) {
				certs = p7->d.signed_and_enveloped->cert;
				crls = p7->d.signed_and_enveloped->crl;
			}
			break;
		default:
			break;
	}

	zout = zend_try_array_init(zout);
	if (!zout) {
		goto clean_exit;
	}

	if (certs != NULL) {
		for (i = 0; i < sk_X509_num(certs); i++) {
			X509* ca = sk_X509_value(certs, i);

			bio_out = BIO_new(BIO_s_mem());
			if (bio_out && PEM_write_bio_X509(bio_out, ca)) {
				BUF_MEM *bio_buf;
				BIO_get_mem_ptr(bio_out, &bio_buf);
				ZVAL_STRINGL(&zcert, bio_buf->data, bio_buf->length);
				add_index_zval(zout, i, &zcert);
				BIO_free(bio_out);
			}
		}
	}

	if (crls != NULL) {
		for (i = 0; i < sk_X509_CRL_num(crls); i++) {
			X509_CRL* crl = sk_X509_CRL_value(crls, i);

			bio_out = BIO_new(BIO_s_mem());
			if (bio_out && PEM_write_bio_X509_CRL(bio_out, crl)) {
				BUF_MEM *bio_buf;
				BIO_get_mem_ptr(bio_out, &bio_buf);
				ZVAL_STRINGL(&zcert, bio_buf->data, bio_buf->length);
				add_index_zval(zout, i, &zcert);
				BIO_free(bio_out);
			}
		}
	}

	RETVAL_TRUE;

clean_exit:
	BIO_free(bio_in);

	if (p7 != NULL) {
		PKCS7_free(p7);
	}
}
/* }}} */

/* {{{ Signs the MIME message in the file named infile with signcert/signkey and output the result to file name outfile. headers lists plain text headers to exclude from the signed portion of the message, and should include to, from and subject as a minimum */

PHP_FUNCTION(openssl_pkcs7_sign)
{
	X509 *cert = NULL;
	zend_object *cert_obj;
	zend_string *cert_str;
	zval *zprivkey, * zheaders;
	zval * hval;
	EVP_PKEY * privkey = NULL;
	zend_long flags = PKCS7_DETACHED;
	PKCS7 * p7 = NULL;
	BIO * infile = NULL, * outfile = NULL;
	STACK_OF(X509) *others = NULL;
	zend_string * strindex;
	char * infilename;
	size_t infilename_len;
	char * outfilename;
	size_t outfilename_len;
	char * extracertsfilename = NULL;
	size_t extracertsfilename_len;

	ZEND_PARSE_PARAMETERS_START(5, 7)
		Z_PARAM_PATH(infilename, infilename_len)
		Z_PARAM_PATH(outfilename, outfilename_len)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_ZVAL(zprivkey)
		Z_PARAM_ARRAY_OR_NULL(zheaders)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
		Z_PARAM_PATH_OR_NULL(extracertsfilename, extracertsfilename_len)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	if (extracertsfilename) {
		others = php_openssl_load_all_certs_from_file(extracertsfilename);
		if (others == NULL) {
			goto clean_exit;
		}
	}

	privkey = php_openssl_pkey_from_zval(zprivkey, 0, "", 0);
	if (privkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Error getting private key");
		}
		goto clean_exit;
	}

	cert = php_openssl_x509_from_param(cert_obj, cert_str);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		goto clean_exit;
	}

	if (php_openssl_open_base_dir_chk(infilename) || php_openssl_open_base_dir_chk(outfilename)) {
		goto clean_exit;
	}

	infile = BIO_new_file(infilename, PHP_OPENSSL_BIO_MODE_R(flags));
	if (infile == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error opening input file %s!", infilename);
		goto clean_exit;
	}

	outfile = BIO_new_file(outfilename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
	if (outfile == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error opening output file %s!", outfilename);
		goto clean_exit;
	}

	p7 = PKCS7_sign(cert, privkey, others, infile, (int)flags);
	if (p7 == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error creating PKCS7 structure!");
		goto clean_exit;
	}

	(void)BIO_reset(infile);

	/* tack on extra headers */
	if (zheaders) {
		int ret;

		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(zheaders), strindex, hval) {
			zend_string *str = zval_try_get_string(hval);
			if (UNEXPECTED(!str)) {
				goto clean_exit;
			}
			if (strindex) {
				ret = BIO_printf(outfile, "%s: %s\n", ZSTR_VAL(strindex), ZSTR_VAL(str));
			} else {
				ret = BIO_printf(outfile, "%s\n", ZSTR_VAL(str));
			}
			zend_string_release(str);
			if (ret < 0) {
				php_openssl_store_errors();
			}
		} ZEND_HASH_FOREACH_END();
	}
	/* write the signed data */
	if (!SMIME_write_PKCS7(outfile, p7, infile, (int)flags)) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	RETVAL_TRUE;

clean_exit:
	PKCS7_free(p7);
	BIO_free(infile);
	BIO_free(outfile);
	if (others) {
		sk_X509_pop_free(others, X509_free);
	}
	EVP_PKEY_free(privkey);
	if (cert && cert_str) {
		X509_free(cert);
	}
}
/* }}} */

/* {{{ Decrypts the S/MIME message in the file name infilename and output the results to the file name outfilename.  recipcert is a CERT for one of the recipients. recipkey specifies the private key matching recipcert, if recipcert does not include the key */

PHP_FUNCTION(openssl_pkcs7_decrypt)
{
	X509 *cert;
	zval *recipcert, *recipkey = NULL;
	bool free_recipcert;
	EVP_PKEY * key = NULL;
	BIO * in = NULL, *out = NULL, *datain = NULL;
	PKCS7 * p7 = NULL;
	char * infilename;
	size_t infilename_len;
	char * outfilename;
	size_t outfilename_len;

	ZEND_PARSE_PARAMETERS_START(3, 4)
		Z_PARAM_PATH(infilename, infilename_len)
		Z_PARAM_PATH(outfilename, outfilename_len)
		Z_PARAM_ZVAL(recipcert)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL_OR_NULL(recipkey)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	cert = php_openssl_x509_from_zval(recipcert, &free_recipcert);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		goto clean_exit;
	}

	key = php_openssl_pkey_from_zval(recipkey ? recipkey : recipcert, 0, "", 0);
	if (key == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Unable to get private key");
		}
		goto clean_exit;
	}

	if (php_openssl_open_base_dir_chk(infilename) || php_openssl_open_base_dir_chk(outfilename)) {
		goto clean_exit;
	}

	in = BIO_new_file(infilename, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
	if (in == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}
	out = BIO_new_file(outfilename, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
	if (out == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	p7 = SMIME_read_PKCS7(in, &datain);

	if (p7 == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}
	if (PKCS7_decrypt(p7, key, cert, out, PKCS7_DETACHED)) {
		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
	}
clean_exit:
	PKCS7_free(p7);
	BIO_free(datain);
	BIO_free(in);
	BIO_free(out);
	if (cert && free_recipcert) {
		X509_free(cert);
	}
	EVP_PKEY_free(key);
}
/* }}} */

/* }}} */

/* {{{ CMS S/MIME functions taken from PKCS#7 functions */

/* {{{ Verifies that the data block is intact, the signer is who they say they are, and returns the CERTs of the signers */
PHP_FUNCTION(openssl_cms_verify)
{
	X509_STORE * store = NULL;
	zval * cainfo = NULL;
	STACK_OF(X509) *signers= NULL;
	STACK_OF(X509) *others = NULL;
	CMS_ContentInfo * cms = NULL;
	BIO * in = NULL, * datain = NULL, * dataout = NULL, * p7bout  = NULL;
	BIO *certout = NULL, *sigbio = NULL;
	zend_long flags = 0;
	char * filename;
	size_t filename_len;
	char * extracerts = NULL;
	size_t extracerts_len = 0;
	char * signersfilename = NULL;
	size_t signersfilename_len = 0;
	char * datafilename = NULL;
	size_t datafilename_len = 0;
	char * p7bfilename = NULL;
	size_t p7bfilename_len = 0;
	char * sigfile = NULL;
	size_t sigfile_len = 0;
	zend_long encoding = ENCODING_SMIME;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "pl|p!ap!p!p!p!l", &filename, &filename_len,
				&flags, &signersfilename, &signersfilename_len, &cainfo,
				&extracerts, &extracerts_len, &datafilename, &datafilename_len,
				&p7bfilename, &p7bfilename_len,
				&sigfile, &sigfile_len, &encoding) == FAILURE) {
		RETURN_THROWS();
	}

	if (php_openssl_open_base_dir_chk(filename)) {
		goto clean_exit;
	}
	in = BIO_new_file(filename, PHP_OPENSSL_BIO_MODE_R(flags));
	if (in == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}
	if (sigfile && (flags & CMS_DETACHED)) {
		sigbio = BIO_new_file(sigfile, PHP_OPENSSL_BIO_MODE_R(flags));
		if (encoding == ENCODING_SMIME)  {
			php_error_docref(NULL, E_WARNING,
					 "Detached signatures not possible with S/MIME encoding");
			goto clean_exit;
		}
	} else  {
		sigbio = in;	/* non-detached signature */
	}

	switch (encoding) {
		case ENCODING_PEM:
			cms = PEM_read_bio_CMS(sigbio, NULL, 0, NULL);
				datain = in;
				break;
			case ENCODING_DER:
				cms = d2i_CMS_bio(sigbio, NULL);
				datain = in;
				break;
			case ENCODING_SMIME:
				cms = SMIME_read_CMS(sigbio, &datain);
				break;
			default:
				php_error_docref(NULL, E_WARNING, "Unknown encoding");
				goto clean_exit;
	}
	if (cms == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}
	if (encoding != ENCODING_SMIME && !(flags & CMS_DETACHED)) {
		datain = NULL; /* when not detached, don't pass a real BIO */
	}

	if (extracerts) {
		others = php_openssl_load_all_certs_from_file(extracerts);
		if (others == NULL) {
			goto clean_exit;
		}
	}

	store = php_openssl_setup_verify(cainfo);

	if (!store) {
		goto clean_exit;
	}

	if (datafilename) {

		if (php_openssl_open_base_dir_chk(datafilename)) {
			goto clean_exit;
		}

		dataout = BIO_new_file(datafilename, PHP_OPENSSL_BIO_MODE_W(CMS_BINARY));
		if (dataout == NULL) {
			php_openssl_store_errors();
			goto clean_exit;
		}
	}

	if (p7bfilename) {

		if (php_openssl_open_base_dir_chk(p7bfilename)) {
			goto clean_exit;
		}

		p7bout = BIO_new_file(p7bfilename, PHP_OPENSSL_BIO_MODE_W(CMS_BINARY));
		if (p7bout == NULL) {
			php_openssl_store_errors();
			goto clean_exit;
		}
	}
#if DEBUG_SMIME
	zend_printf("Calling CMS verify\n");
#endif
	if (CMS_verify(cms, others, store, datain, dataout, (unsigned int)flags)) {
		RETVAL_TRUE;

		if (signersfilename) {
			if (php_openssl_open_base_dir_chk(signersfilename)) {
				goto clean_exit;
			}

			certout = BIO_new_file(signersfilename, PHP_OPENSSL_BIO_MODE_W(CMS_BINARY));
			if (certout) {
				int i;
				signers = CMS_get0_signers(cms);
				if (signers != NULL) {

					for (i = 0; i < sk_X509_num(signers); i++) {
						if (!PEM_write_bio_X509(certout, sk_X509_value(signers, i))) {
							php_openssl_store_errors();
							RETVAL_FALSE;
							php_error_docref(NULL, E_WARNING, "Failed to write signer %d", i);
						}
					}

					sk_X509_free(signers);
				} else {
					RETVAL_FALSE;
					php_openssl_store_errors();
				}
			} else {
				php_openssl_store_errors();
				php_error_docref(NULL, E_WARNING, "Signature OK, but cannot open %s for writing", signersfilename);
				RETVAL_FALSE;
			}

			if (p7bout) {
				PEM_write_bio_CMS(p7bout, cms);
			}
		}
	} else {
		php_openssl_store_errors();
		RETVAL_FALSE;
	}
clean_exit:
	BIO_free(p7bout);
	if (store) {
		X509_STORE_free(store);
	}
	if (datain != in) {
		BIO_free(datain);
	}
	if (sigbio != in) {
		BIO_free(sigbio);
	}
	BIO_free(in);
	BIO_free(dataout);
	BIO_free(certout);
	if (cms) {
		CMS_ContentInfo_free(cms);
	}
	if (others) {
		sk_X509_pop_free(others, X509_free);
	}
}
/* }}} */

/* {{{ Encrypts the message in the file named infile with the certificates in recipcerts and output the result to the file named outfile */
PHP_FUNCTION(openssl_cms_encrypt)
{
	zval * zrecipcerts, * zheaders = NULL;
	STACK_OF(X509) * recipcerts = NULL;
	BIO * infile = NULL, * outfile = NULL;
	zend_long flags = 0;
	zend_long encoding = ENCODING_SMIME;
	CMS_ContentInfo * cms = NULL;
	zval * zcertval;
	X509 * cert;
	const EVP_CIPHER *cipher = NULL;
	zend_long cipherid = PHP_OPENSSL_CIPHER_DEFAULT;
	zend_string * strindex;
	char * infilename = NULL;
	size_t infilename_len;
	char * outfilename = NULL;
	size_t outfilename_len;
	int need_final = 0;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ppza!|lll", &infilename, &infilename_len,
				  &outfilename, &outfilename_len, &zrecipcerts, &zheaders, &flags, &encoding, &cipherid) == FAILURE) {
		RETURN_THROWS();
	}


	if (php_openssl_open_base_dir_chk(infilename) || php_openssl_open_base_dir_chk(outfilename)) {
		return;
	}

	infile = BIO_new_file(infilename, PHP_OPENSSL_BIO_MODE_R(flags));
	if (infile == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	outfile = BIO_new_file(outfilename, PHP_OPENSSL_BIO_MODE_W(flags));
	if (outfile == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	recipcerts = sk_X509_new_null();

	/* get certs */
	if (Z_TYPE_P(zrecipcerts) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zrecipcerts), zcertval) {
			bool free_cert;

			cert = php_openssl_x509_from_zval(zcertval, &free_cert);
			if (cert == NULL) {
				goto clean_exit;
			}

			if (!free_cert) {
				/* we shouldn't free this particular cert, as it is a resource.
					make a copy and push that on the stack instead */
				cert = X509_dup(cert);
				if (cert == NULL) {
					php_openssl_store_errors();
					goto clean_exit;
				}
			}
			sk_X509_push(recipcerts, cert);
		} ZEND_HASH_FOREACH_END();
	} else {
		/* a single certificate */
		bool free_cert;

		cert = php_openssl_x509_from_zval(zrecipcerts, &free_cert);
		if (cert == NULL) {
			goto clean_exit;
		}

		if (!free_cert) {
			/* we shouldn't free this particular cert, as it is a resource.
				make a copy and push that on the stack instead */
			cert = X509_dup(cert);
			if (cert == NULL) {
				php_openssl_store_errors();
				goto clean_exit;
			}
		}
		sk_X509_push(recipcerts, cert);
	}

	/* sanity check the cipher */
	cipher = php_openssl_get_evp_cipher_from_algo(cipherid);
	if (cipher == NULL) {
		/* shouldn't happen */
		php_error_docref(NULL, E_WARNING, "Failed to get cipher");
		goto clean_exit;
	}

	cms = CMS_encrypt(recipcerts, infile, (EVP_CIPHER*)cipher, (unsigned int)flags);

	if (cms == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	if (flags & CMS_PARTIAL && !(flags & CMS_STREAM)) {
		need_final=1;
	}

	/* tack on extra headers */
	if (zheaders && encoding == ENCODING_SMIME) {
		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(zheaders), strindex, zcertval) {
			zend_string *str = zval_try_get_string(zcertval);
			if (UNEXPECTED(!str)) {
				goto clean_exit;
			}
			if (strindex) {
				BIO_printf(outfile, "%s: %s\n", ZSTR_VAL(strindex), ZSTR_VAL(str));
			} else {
				BIO_printf(outfile, "%s\n", ZSTR_VAL(str));
			}
			zend_string_release(str);
		} ZEND_HASH_FOREACH_END();
	}

	(void)BIO_reset(infile);

	switch (encoding) {
		case ENCODING_SMIME:
			if (!SMIME_write_CMS(outfile, cms, infile, (int)flags)) {
				php_openssl_store_errors();
					goto clean_exit;
			}
			break;
		case ENCODING_DER:
			if (need_final) {
				if (CMS_final(cms, infile, NULL, (unsigned int) flags) != 1) {
					php_openssl_store_errors();
					goto clean_exit;
				}
			}
			if (i2d_CMS_bio(outfile, cms) != 1) {
				php_openssl_store_errors();
				goto clean_exit;
			}
			break;
		case ENCODING_PEM:
			if (need_final) {
				if (CMS_final(cms, infile, NULL, (unsigned int) flags) != 1) {
					php_openssl_store_errors();
					goto clean_exit;
				}
			}
			if (flags & CMS_STREAM) {
				if (PEM_write_bio_CMS_stream(outfile, cms, infile, flags) == 0) {
					php_openssl_store_errors();
					goto clean_exit;
				}
			} else {
				if (PEM_write_bio_CMS(outfile, cms) == 0) {
					php_openssl_store_errors();
					goto clean_exit;
				}
			}
			break;
		default:
			php_error_docref(NULL, E_WARNING, "Unknown OPENSSL encoding");
			goto clean_exit;
	}

	RETVAL_TRUE;

clean_exit:
	if (cms) {
		CMS_ContentInfo_free(cms);
	}
	BIO_free(infile);
	BIO_free(outfile);
	if (recipcerts) {
		sk_X509_pop_free(recipcerts, X509_free);
	}
}
/* }}} */

/* {{{ Exports the CMS file to an array of PEM certificates */
PHP_FUNCTION(openssl_cms_read)
{
	zval * zout = NULL, zcert;
	char *p7b;
	size_t p7b_len;
	STACK_OF(X509) *certs = NULL;
	STACK_OF(X509_CRL) *crls = NULL;
	BIO * bio_in = NULL, * bio_out = NULL;
	CMS_ContentInfo * cms = NULL;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz", &p7b, &p7b_len,
				&zout) == FAILURE) {
		RETURN_THROWS();
	}

	RETVAL_FALSE;

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(p7b_len, p7b, 1);

	bio_in = BIO_new(BIO_s_mem());
	if (bio_in == NULL) {
		goto clean_exit;
	}

	if (0 >= BIO_write(bio_in, p7b, (int)p7b_len)) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	cms = PEM_read_bio_CMS(bio_in, NULL, NULL, NULL);
	if (cms == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	switch (OBJ_obj2nid(CMS_get0_type(cms))) {
		case NID_pkcs7_signed:
		case NID_pkcs7_signedAndEnveloped:
			certs = CMS_get1_certs(cms);
			crls = CMS_get1_crls(cms);
			break;
		default:
			break;
	}

	zout = zend_try_array_init(zout);
	if (!zout) {
		goto clean_exit;
	}

	if (certs != NULL) {
		for (i = 0; i < sk_X509_num(certs); i++) {
			X509* ca = sk_X509_value(certs, i);

			bio_out = BIO_new(BIO_s_mem());
			if (bio_out && PEM_write_bio_X509(bio_out, ca)) {
				BUF_MEM *bio_buf;
				BIO_get_mem_ptr(bio_out, &bio_buf);
				ZVAL_STRINGL(&zcert, bio_buf->data, bio_buf->length);
				add_index_zval(zout, i, &zcert);
				BIO_free(bio_out);
			}
		}
	}

	if (crls != NULL) {
		for (i = 0; i < sk_X509_CRL_num(crls); i++) {
			X509_CRL* crl = sk_X509_CRL_value(crls, i);

			bio_out = BIO_new(BIO_s_mem());
			if (bio_out && PEM_write_bio_X509_CRL(bio_out, crl)) {
				BUF_MEM *bio_buf;
				BIO_get_mem_ptr(bio_out, &bio_buf);
				ZVAL_STRINGL(&zcert, bio_buf->data, bio_buf->length);
				add_index_zval(zout, i, &zcert);
				BIO_free(bio_out);
			}
		}
	}

	RETVAL_TRUE;

clean_exit:
	BIO_free(bio_in);
	if (cms != NULL) {
		CMS_ContentInfo_free(cms);
	}
	if (certs != NULL) {
		sk_X509_pop_free(certs, X509_free);
	}
	if (crls != NULL) {
		sk_X509_CRL_pop_free(crls, X509_CRL_free);
	}
}
/* }}} */

/* {{{ Signs the MIME message in the file named infile with signcert/signkey and output the result to file name outfile. headers lists plain text headers to exclude from the signed portion of the message, and should include to, from and subject as a minimum */

PHP_FUNCTION(openssl_cms_sign)
{
	X509 *cert = NULL;
	zend_object *cert_obj;
	zend_string *cert_str;
	zval *zprivkey, *zheaders;
	zval * hval;
	EVP_PKEY * privkey = NULL;
	zend_long flags = 0;
	zend_long encoding = ENCODING_SMIME;
	CMS_ContentInfo * cms = NULL;
	BIO * infile = NULL, * outfile = NULL;
	STACK_OF(X509) *others = NULL;
	zend_string * strindex;
	char * infilename;
	size_t infilename_len;
	char * outfilename;
	size_t outfilename_len;
	char * extracertsfilename = NULL;
	size_t extracertsfilename_len;
	int need_final = 0;

	ZEND_PARSE_PARAMETERS_START(5, 8)
		Z_PARAM_PATH(infilename, infilename_len)
		Z_PARAM_PATH(outfilename, outfilename_len)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_ZVAL(zprivkey)
		Z_PARAM_ARRAY_OR_NULL(zheaders)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
		Z_PARAM_LONG(encoding)
		Z_PARAM_PATH(extracertsfilename, extracertsfilename_len)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	if (extracertsfilename) {
		others = php_openssl_load_all_certs_from_file(extracertsfilename);
		if (others == NULL) {
			goto clean_exit;
		}
	}

	privkey = php_openssl_pkey_from_zval(zprivkey, 0, "", 0);
	if (privkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Error getting private key");
		}
		goto clean_exit;
	}

	cert = php_openssl_x509_from_param(cert_obj, cert_str);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		goto clean_exit;
	}

	if (php_openssl_open_base_dir_chk(infilename) || php_openssl_open_base_dir_chk(outfilename)) {
		goto clean_exit;
	}

	if ((encoding & ENCODING_SMIME) && (flags & CMS_DETACHED)) {
		php_error_docref(NULL,
				 E_WARNING, "Detached signatures not possible with S/MIME encoding");
		goto clean_exit;
	}

	/* a CMS struct will not be complete if either CMS_PARTIAL or CMS_STREAM is set.
	 * However, CMS_PARTIAL requires a CMS_final call whereas CMS_STREAM requires
	 * a different write routine below.  There may be a more efficient way to do this
	 * with function pointers, but the readability goes down.
	 * References: CMS_sign(3SSL), CMS_final(3SSL)
	 */

	if (flags & CMS_PARTIAL && !(flags & CMS_STREAM)) {
		need_final=1;
	}

	infile = BIO_new_file(infilename, PHP_OPENSSL_BIO_MODE_R(flags));
	if (infile == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error opening input file %s!", infilename);
		goto clean_exit;
	}

	outfile = BIO_new_file(outfilename, PHP_OPENSSL_BIO_MODE_W(CMS_BINARY));
	if (outfile == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error opening output file %s!", outfilename);
		goto clean_exit;
	}

	cms = CMS_sign(cert, privkey, others, infile, (unsigned int)flags);
	if (cms == NULL) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error creating CMS structure!");
		goto clean_exit;
	}
	if (BIO_reset(infile) != 0) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	/* tack on extra headers */
	if (zheaders && encoding == ENCODING_SMIME) {
		int ret;

		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(zheaders), strindex, hval) {
			zend_string *str = zval_try_get_string(hval);
			if (UNEXPECTED(!str)) {
				goto clean_exit;
			}
			if (strindex) {
				ret = BIO_printf(outfile, "%s: %s\n", ZSTR_VAL(strindex), ZSTR_VAL(str));
			} else {
				ret = BIO_printf(outfile, "%s\n", ZSTR_VAL(str));
			}
			zend_string_release(str);
			if (ret < 0) {
				php_openssl_store_errors();
			}
		} ZEND_HASH_FOREACH_END();
	}
	/* writing the signed data depends on the encoding */
	switch (encoding) {
		case ENCODING_SMIME:
			if (!SMIME_write_CMS(outfile, cms, infile, (int)flags)) {
				php_openssl_store_errors();
					goto clean_exit;
			}
			break;
		case ENCODING_DER:
			if (need_final) {
				if (CMS_final(cms, infile, NULL, (unsigned int) flags) != 1) {
					php_openssl_store_errors();
					goto clean_exit;
				}
			}
			if (i2d_CMS_bio(outfile, cms) != 1) {
				php_openssl_store_errors();
				goto clean_exit;
			}
			break;
		case ENCODING_PEM:
			if (need_final) {
				if (CMS_final(cms, infile, NULL, (unsigned int) flags) != 1) {
					php_openssl_store_errors();
					goto clean_exit;
				}
			}
			if (flags & CMS_STREAM) {
				if (PEM_write_bio_CMS_stream(outfile, cms, infile, flags) == 0) {
					php_openssl_store_errors();
					goto clean_exit;
				}
			} else {
				if (PEM_write_bio_CMS(outfile, cms) == 0) {
					php_openssl_store_errors();
					goto clean_exit;
				}
			}
			break;
		default:
			php_error_docref(NULL, E_WARNING, "Unknown OPENSSL encoding");
			goto clean_exit;
	}
	RETVAL_TRUE;

clean_exit:
	if (cms) {
		CMS_ContentInfo_free(cms);
	}
	BIO_free(infile);
	BIO_free(outfile);
	if (others) {
		sk_X509_pop_free(others, X509_free);
	}
	EVP_PKEY_free(privkey);
	if (cert && cert_str) {
		X509_free(cert);
	}
}
/* }}} */

/* {{{ Decrypts the S/MIME message in the file name infilename and output the results to the file name outfilename.  recipcert is a CERT for one of the recipients. recipkey specifies the private key matching recipcert, if recipcert does not include the key */

PHP_FUNCTION(openssl_cms_decrypt)
{
	X509 *cert;
	zval *recipcert, *recipkey = NULL;
	bool free_recipcert;
	EVP_PKEY * key = NULL;
	zend_long encoding = ENCODING_SMIME;
	BIO * in = NULL, * out = NULL, * datain = NULL;
	CMS_ContentInfo * cms = NULL;
	char * infilename;
	size_t infilename_len;
	char * outfilename;
	size_t outfilename_len;

	ZEND_PARSE_PARAMETERS_START(3, 5)
		Z_PARAM_PATH(infilename, infilename_len)
		Z_PARAM_PATH(outfilename, outfilename_len)
		Z_PARAM_ZVAL(recipcert)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL_OR_NULL(recipkey)
		Z_PARAM_LONG(encoding)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	cert = php_openssl_x509_from_zval(recipcert, &free_recipcert);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		goto clean_exit;
	}

	key = php_openssl_pkey_from_zval(recipkey ? recipkey : recipcert, 0, "", 0);
	if (key == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Unable to get private key");
		}
		goto clean_exit;
	}

	if (php_openssl_open_base_dir_chk(infilename) || php_openssl_open_base_dir_chk(outfilename)) {
		goto clean_exit;
	}

	in = BIO_new_file(infilename, PHP_OPENSSL_BIO_MODE_R(CMS_BINARY));
	if (in == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}
	out = BIO_new_file(outfilename, PHP_OPENSSL_BIO_MODE_W(CMS_BINARY));
	if (out == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}

	switch (encoding) {
		case ENCODING_DER:
			cms = d2i_CMS_bio(in, NULL);
			break;
		case ENCODING_PEM:
                        cms = PEM_read_bio_CMS(in, NULL, 0, NULL);
			break;
		case ENCODING_SMIME:
			cms = SMIME_read_CMS(in, &datain);
			break;
		default:
			zend_argument_value_error(5, "must be an OPENSSL_ENCODING_* constant");
			goto clean_exit;
	}

	if (cms == NULL) {
		php_openssl_store_errors();
		goto clean_exit;
	}
	if (CMS_decrypt(cms, key, cert, NULL, out, 0)) {
		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
	}
clean_exit:
	if (cms) {
		CMS_ContentInfo_free(cms);
	}
	BIO_free(datain);
	BIO_free(in);
	BIO_free(out);
	if (cert && free_recipcert) {
		X509_free(cert);
	}
	EVP_PKEY_free(key);
}
/* }}} */

/* }}} */



/* {{{ Encrypts data with private key */
PHP_FUNCTION(openssl_private_encrypt)
{
	zval *key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	zend_string *cryptedbuf = NULL;
	int successful = 0;
	char * data;
	size_t data_len;
	zend_long padding = RSA_PKCS1_PADDING;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(data_len, data, 1);

	RETVAL_FALSE;

	pkey = php_openssl_pkey_from_zval(key, 0, "", 0);

	if (pkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "key param is not a valid private key");
		}
		RETURN_FALSE;
	}

	cryptedlen = EVP_PKEY_size(pkey);
	cryptedbuf = zend_string_alloc(cryptedlen, 0);

	switch (EVP_PKEY_id(pkey)) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			successful = (RSA_private_encrypt((int)data_len,
						(unsigned char *)data,
						(unsigned char *)ZSTR_VAL(cryptedbuf),
						EVP_PKEY_get0_RSA(pkey),
						(int)padding) == cryptedlen);
			break;
		default:
			php_error_docref(NULL, E_WARNING, "key type not supported in this PHP build!");
	}

	if (successful) {
		ZSTR_VAL(cryptedbuf)[cryptedlen] = '\0';
		ZEND_TRY_ASSIGN_REF_NEW_STR(crypted, cryptedbuf);
		cryptedbuf = NULL;
		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
	}
	if (cryptedbuf) {
		zend_string_release_ex(cryptedbuf, 0);
	}
	EVP_PKEY_free(pkey);
}
/* }}} */

/* {{{ Decrypts data with private key */
PHP_FUNCTION(openssl_private_decrypt)
{
	zval *key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	zend_string *cryptedbuf = NULL;
	unsigned char *crypttemp;
	int successful = 0;
	zend_long padding = RSA_PKCS1_PADDING;
	char * data;
	size_t data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(data_len, data, 1);

	RETVAL_FALSE;

	pkey = php_openssl_pkey_from_zval(key, 0, "", 0);
	if (pkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "key parameter is not a valid private key");
		}
		RETURN_FALSE;
	}

	cryptedlen = EVP_PKEY_size(pkey);
	crypttemp = emalloc(cryptedlen + 1);

	switch (EVP_PKEY_id(pkey)) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			cryptedlen = RSA_private_decrypt((int)data_len,
					(unsigned char *)data,
					crypttemp,
					EVP_PKEY_get0_RSA(pkey),
					(int)padding);
			if (cryptedlen != -1) {
				cryptedbuf = zend_string_alloc(cryptedlen, 0);
				memcpy(ZSTR_VAL(cryptedbuf), crypttemp, cryptedlen);
				successful = 1;
			}
			break;
		default:
			php_error_docref(NULL, E_WARNING, "key type not supported in this PHP build!");
	}

	efree(crypttemp);

	if (successful) {
		ZSTR_VAL(cryptedbuf)[cryptedlen] = '\0';
		ZEND_TRY_ASSIGN_REF_NEW_STR(crypted, cryptedbuf);
		cryptedbuf = NULL;
		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
	}

	EVP_PKEY_free(pkey);
	if (cryptedbuf) {
		zend_string_release_ex(cryptedbuf, 0);
	}
}
/* }}} */

/* {{{ Encrypts data with public key */
PHP_FUNCTION(openssl_public_encrypt)
{
	zval *key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	zend_string *cryptedbuf;
	int successful = 0;
	zend_long padding = RSA_PKCS1_PADDING;
	char * data;
	size_t data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(data_len, data, 1);

	RETVAL_FALSE;

	pkey = php_openssl_pkey_from_zval(key, 1, NULL, 0);
	if (pkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "key parameter is not a valid public key");
		}
		RETURN_FALSE;
	}

	cryptedlen = EVP_PKEY_size(pkey);
	cryptedbuf = zend_string_alloc(cryptedlen, 0);

	switch (EVP_PKEY_id(pkey)) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			successful = (RSA_public_encrypt((int)data_len,
						(unsigned char *)data,
						(unsigned char *)ZSTR_VAL(cryptedbuf),
						EVP_PKEY_get0_RSA(pkey),
						(int)padding) == cryptedlen);
			break;
		default:
			php_error_docref(NULL, E_WARNING, "key type not supported in this PHP build!");

	}

	if (successful) {
		ZSTR_VAL(cryptedbuf)[cryptedlen] = '\0';
		ZEND_TRY_ASSIGN_REF_NEW_STR(crypted, cryptedbuf);
		cryptedbuf = NULL;
		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
	}
	EVP_PKEY_free(pkey);
	if (cryptedbuf) {
		zend_string_release_ex(cryptedbuf, 0);
	}
}
/* }}} */

/* {{{ Decrypts data with public key */
PHP_FUNCTION(openssl_public_decrypt)
{
	zval *key, *crypted;
	EVP_PKEY *pkey;
	int cryptedlen;
	zend_string *cryptedbuf = NULL;
	unsigned char *crypttemp;
	int successful = 0;
	zend_long padding = RSA_PKCS1_PADDING;
	char * data;
	size_t data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(data_len, data, 1);

	RETVAL_FALSE;

	pkey = php_openssl_pkey_from_zval(key, 1, NULL, 0);
	if (pkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "key parameter is not a valid public key");
		}
		RETURN_FALSE;
	}

	cryptedlen = EVP_PKEY_size(pkey);
	crypttemp = emalloc(cryptedlen + 1);

	switch (EVP_PKEY_id(pkey)) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			cryptedlen = RSA_public_decrypt((int)data_len,
					(unsigned char *)data,
					crypttemp,
					EVP_PKEY_get0_RSA(pkey),
					(int)padding);
			if (cryptedlen != -1) {
				cryptedbuf = zend_string_alloc(cryptedlen, 0);
				memcpy(ZSTR_VAL(cryptedbuf), crypttemp, cryptedlen);
				successful = 1;
			}
			break;

		default:
			php_error_docref(NULL, E_WARNING, "key type not supported in this PHP build!");

	}

	efree(crypttemp);

	if (successful) {
		ZSTR_VAL(cryptedbuf)[cryptedlen] = '\0';
		ZEND_TRY_ASSIGN_REF_NEW_STR(crypted, cryptedbuf);
		cryptedbuf = NULL;
		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
	}

	if (cryptedbuf) {
		zend_string_release_ex(cryptedbuf, 0);
	}
	EVP_PKEY_free(pkey);
}
/* }}} */

/* {{{ Returns a description of the last error, and alters the index of the error messages. Returns false when the are no more messages */
PHP_FUNCTION(openssl_error_string)
{
	char buf[256];
	unsigned long val;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	php_openssl_store_errors();

	if (OPENSSL_G(errors) == NULL || OPENSSL_G(errors)->top == OPENSSL_G(errors)->bottom) {
		RETURN_FALSE;
	}

	OPENSSL_G(errors)->bottom = (OPENSSL_G(errors)->bottom + 1) % ERR_NUM_ERRORS;
	val = OPENSSL_G(errors)->buffer[OPENSSL_G(errors)->bottom];

	if (val) {
		ERR_error_string_n(val, buf, 256);
		RETURN_STRING(buf);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Signs data */
PHP_FUNCTION(openssl_sign)
{
	zval *key, *signature;
	EVP_PKEY *pkey;
	unsigned int siglen;
	zend_string *sigbuf;
	char * data;
	size_t data_len;
	EVP_MD_CTX *md_ctx;
	zend_string *method_str = NULL;
	zend_long method_long = OPENSSL_ALGO_SHA1;
	const EVP_MD *mdtype;

	ZEND_PARSE_PARAMETERS_START(3, 4)
		Z_PARAM_STRING(data, data_len)
		Z_PARAM_ZVAL(signature)
		Z_PARAM_ZVAL(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_LONG(method_str, method_long)
	ZEND_PARSE_PARAMETERS_END();

	pkey = php_openssl_pkey_from_zval(key, 0, "", 0);
	if (pkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Supplied key param cannot be coerced into a private key");
		}
		RETURN_FALSE;
	}

	if (method_str) {
		mdtype = EVP_get_digestbyname(ZSTR_VAL(method_str));
	} else {
		mdtype = php_openssl_get_evp_md_from_algo(method_long);
	}
	if (!mdtype) {
		php_error_docref(NULL, E_WARNING, "Unknown digest algorithm");
		RETURN_FALSE;
	}

	siglen = EVP_PKEY_size(pkey);
	sigbuf = zend_string_alloc(siglen, 0);

	md_ctx = EVP_MD_CTX_create();
	if (md_ctx != NULL &&
			EVP_SignInit(md_ctx, mdtype) &&
			EVP_SignUpdate(md_ctx, data, data_len) &&
			EVP_SignFinal(md_ctx, (unsigned char*)ZSTR_VAL(sigbuf), &siglen, pkey)) {
		ZSTR_VAL(sigbuf)[siglen] = '\0';
		ZSTR_LEN(sigbuf) = siglen;
		ZEND_TRY_ASSIGN_REF_NEW_STR(signature, sigbuf);
		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
		efree(sigbuf);
		RETVAL_FALSE;
	}
	EVP_MD_CTX_destroy(md_ctx);
	EVP_PKEY_free(pkey);
}
/* }}} */

/* {{{ Verifys data */
PHP_FUNCTION(openssl_verify)
{
	zval *key;
	EVP_PKEY *pkey;
	int err = 0;
	EVP_MD_CTX *md_ctx;
	const EVP_MD *mdtype;
	char * data;
	size_t data_len;
	char * signature;
	size_t signature_len;
	zend_string *method_str = NULL;
	zend_long method_long = OPENSSL_ALGO_SHA1;

	ZEND_PARSE_PARAMETERS_START(3, 4)
		Z_PARAM_STRING(data, data_len)
		Z_PARAM_STRING(signature, signature_len)
		Z_PARAM_ZVAL(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_LONG(method_str, method_long)
	ZEND_PARSE_PARAMETERS_END();

	PHP_OPENSSL_CHECK_SIZE_T_TO_UINT(signature_len, signature, 2);

	if (method_str) {
		mdtype = EVP_get_digestbyname(ZSTR_VAL(method_str));
	} else {
		mdtype = php_openssl_get_evp_md_from_algo(method_long);
	}
	if (!mdtype) {
		php_error_docref(NULL, E_WARNING, "Unknown digest algorithm");
		RETURN_FALSE;
	}

	pkey = php_openssl_pkey_from_zval(key, 1, NULL, 0);
	if (pkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Supplied key param cannot be coerced into a public key");
		}
		RETURN_FALSE;
	}

	md_ctx = EVP_MD_CTX_create();
	if (md_ctx == NULL ||
			!EVP_VerifyInit (md_ctx, mdtype) ||
			!EVP_VerifyUpdate (md_ctx, data, data_len) ||
			(err = EVP_VerifyFinal(md_ctx, (unsigned char *)signature, (unsigned int)signature_len, pkey)) < 0) {
		php_openssl_store_errors();
	}
	EVP_MD_CTX_destroy(md_ctx);
	EVP_PKEY_free(pkey);
	RETURN_LONG(err);
}
/* }}} */

/* {{{ Seals data */
PHP_FUNCTION(openssl_seal)
{
	zval *pubkeys, *pubkey, *sealdata, *ekeys, *iv = NULL;
	HashTable *pubkeysht;
	EVP_PKEY **pkeys;
	int i, len1, len2, *eksl, nkeys, iv_len;
	unsigned char iv_buf[EVP_MAX_IV_LENGTH + 1], *buf = NULL, **eks;
	char * data;
	size_t data_len;
	char *method;
	size_t method_len;
	const EVP_CIPHER *cipher;
	EVP_CIPHER_CTX *ctx;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szzas|z", &data, &data_len,
				&sealdata, &ekeys, &pubkeys, &method, &method_len, &iv) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(data_len, data, 1);

	pubkeysht = Z_ARRVAL_P(pubkeys);
	nkeys = pubkeysht ? zend_hash_num_elements(pubkeysht) : 0;
	if (!nkeys) {
		zend_argument_value_error(4, "cannot be empty");
		RETURN_THROWS();
	}

	cipher = EVP_get_cipherbyname(method);
	if (!cipher) {
		php_error_docref(NULL, E_WARNING, "Unknown cipher algorithm");
		RETURN_FALSE;
	}

	iv_len = EVP_CIPHER_iv_length(cipher);
	if (!iv && iv_len > 0) {
		zend_argument_value_error(6, "cannot be null for the chosen cipher algorithm");
		RETURN_THROWS();
	}

	pkeys = safe_emalloc(nkeys, sizeof(*pkeys), 0);
	eksl = safe_emalloc(nkeys, sizeof(*eksl), 0);
	eks = safe_emalloc(nkeys, sizeof(*eks), 0);
	memset(eks, 0, sizeof(*eks) * nkeys);
	memset(pkeys, 0, sizeof(*pkeys) * nkeys);

	/* get the public keys we are using to seal this data */
	i = 0;
	ZEND_HASH_FOREACH_VAL(pubkeysht, pubkey) {
		pkeys[i] = php_openssl_pkey_from_zval(pubkey, 1, NULL, 0);
		if (pkeys[i] == NULL) {
			if (!EG(exception)) {
				php_error_docref(NULL, E_WARNING, "Not a public key (%dth member of pubkeys)", i+1);
			}
			RETVAL_FALSE;
			goto clean_exit;
		}
		eks[i] = emalloc(EVP_PKEY_size(pkeys[i]) + 1);
		i++;
	} ZEND_HASH_FOREACH_END();

	ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL || !EVP_EncryptInit(ctx,cipher,NULL,NULL)) {
		EVP_CIPHER_CTX_free(ctx);
		php_openssl_store_errors();
		RETVAL_FALSE;
		goto clean_exit;
	}

	/* allocate one byte extra to make room for \0 */
	buf = emalloc(data_len + EVP_CIPHER_CTX_block_size(ctx));
	EVP_CIPHER_CTX_reset(ctx);

	if (EVP_SealInit(ctx, cipher, eks, eksl, &iv_buf[0], pkeys, nkeys) <= 0 ||
			!EVP_SealUpdate(ctx, buf, &len1, (unsigned char *)data, (int)data_len) ||
			!EVP_SealFinal(ctx, buf + len1, &len2)) {
		efree(buf);
		EVP_CIPHER_CTX_free(ctx);
		php_openssl_store_errors();
		RETVAL_FALSE;
		goto clean_exit;
	}

	if (len1 + len2 > 0) {
		ZEND_TRY_ASSIGN_REF_NEW_STR(sealdata, zend_string_init((char*)buf, len1 + len2, 0));
		efree(buf);

		ekeys = zend_try_array_init(ekeys);
		if (!ekeys) {
			EVP_CIPHER_CTX_free(ctx);
			goto clean_exit;
		}

		for (i=0; i<nkeys; i++) {
			eks[i][eksl[i]] = '\0';
			add_next_index_stringl(ekeys, (const char*)eks[i], eksl[i]);
			efree(eks[i]);
			eks[i] = NULL;
		}

		if (iv) {
			iv_buf[iv_len] = '\0';
			ZEND_TRY_ASSIGN_REF_NEW_STR(iv, zend_string_init((char*)iv_buf, iv_len, 0));
		}
	} else {
		efree(buf);
	}
	RETVAL_LONG(len1 + len2);
	EVP_CIPHER_CTX_free(ctx);

clean_exit:
	for (i=0; i<nkeys; i++) {
		if (pkeys[i] != NULL) {
			EVP_PKEY_free(pkeys[i]);
		}
		if (eks[i]) {
			efree(eks[i]);
		}
	}
	efree(eks);
	efree(eksl);
	efree(pkeys);
}
/* }}} */

/* {{{ Opens data */
PHP_FUNCTION(openssl_open)
{
	zval *privkey, *opendata;
	EVP_PKEY *pkey;
	int len1, len2, cipher_iv_len;
	unsigned char *buf, *iv_buf;
	EVP_CIPHER_CTX *ctx;
	char * data;
	size_t data_len;
	char * ekey;
	size_t ekey_len;
	char *method, *iv = NULL;
	size_t method_len, iv_len = 0;
	const EVP_CIPHER *cipher;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szszs|s!", &data, &data_len, &opendata,
				&ekey, &ekey_len, &privkey, &method, &method_len, &iv, &iv_len) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(data_len, data, 1);
	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(ekey_len, ekey, 3);
	pkey = php_openssl_pkey_from_zval(privkey, 0, "", 0);

	if (pkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Unable to coerce parameter 4 into a private key");
		}
		RETURN_FALSE;
	}

	cipher = EVP_get_cipherbyname(method);
	if (!cipher) {
		php_error_docref(NULL, E_WARNING, "Unknown cipher algorithm");
		RETURN_FALSE;
	}

	cipher_iv_len = EVP_CIPHER_iv_length(cipher);
	if (cipher_iv_len > 0) {
		if (!iv) {
			zend_argument_value_error(6, "cannot be null for the chosen cipher algorithm");
			RETURN_THROWS();
		}
		if ((size_t)cipher_iv_len != iv_len) {
			php_error_docref(NULL, E_WARNING, "IV length is invalid");
			RETURN_FALSE;
		}
		iv_buf = (unsigned char *)iv;
	} else {
		iv_buf = NULL;
	}

	buf = emalloc(data_len + 1);

	ctx = EVP_CIPHER_CTX_new();
	if (ctx != NULL && EVP_OpenInit(ctx, cipher, (unsigned char *)ekey, (int)ekey_len, iv_buf, pkey) &&
			EVP_OpenUpdate(ctx, buf, &len1, (unsigned char *)data, (int)data_len) &&
			EVP_OpenFinal(ctx, buf + len1, &len2) && (len1 + len2 > 0)) {
		buf[len1 + len2] = '\0';
		ZEND_TRY_ASSIGN_REF_NEW_STR(opendata, zend_string_init((char*)buf, len1 + len2, 0));
		RETVAL_TRUE;
	} else {
		php_openssl_store_errors();
		RETVAL_FALSE;
	}

	efree(buf);
	EVP_PKEY_free(pkey);
	EVP_CIPHER_CTX_free(ctx);
}
/* }}} */

static void php_openssl_add_method_or_alias(const OBJ_NAME *name, void *arg) /* {{{ */
{
	add_next_index_string((zval*)arg, (char*)name->name);
}
/* }}} */

static void php_openssl_add_method(const OBJ_NAME *name, void *arg) /* {{{ */
{
	if (name->alias == 0) {
		add_next_index_string((zval*)arg, (char*)name->name);
	}
}
/* }}} */

/* {{{ Return array of available digest algorithms */
PHP_FUNCTION(openssl_get_md_methods)
{
	zend_bool aliases = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &aliases) == FAILURE) {
		RETURN_THROWS();
	}
	array_init(return_value);
	OBJ_NAME_do_all_sorted(OBJ_NAME_TYPE_MD_METH,
		aliases ? php_openssl_add_method_or_alias: php_openssl_add_method,
		return_value);
}
/* }}} */

/* {{{ Return array of available cipher algorithms */
PHP_FUNCTION(openssl_get_cipher_methods)
{
	zend_bool aliases = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &aliases) == FAILURE) {
		RETURN_THROWS();
	}
	array_init(return_value);
	OBJ_NAME_do_all_sorted(OBJ_NAME_TYPE_CIPHER_METH,
		aliases ? php_openssl_add_method_or_alias: php_openssl_add_method,
		return_value);
}
/* }}} */

/* {{{ Return array of available elliptic curves */
#ifdef HAVE_EVP_PKEY_EC
PHP_FUNCTION(openssl_get_curve_names)
{
	EC_builtin_curve *curves = NULL;
	const char *sname;
	size_t i;
	size_t len = EC_get_builtin_curves(NULL, 0);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	curves = emalloc(sizeof(EC_builtin_curve) * len);
	if (!EC_get_builtin_curves(curves, len)) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (i = 0; i < len; i++) {
		sname = OBJ_nid2sn(curves[i].nid);
		if (sname != NULL) {
			add_next_index_string(return_value, sname);
		}
	}
	efree(curves);
}
#endif
/* }}} */

/* {{{ Computes digest hash value for given data using given method, returns raw or binhex encoded string */
PHP_FUNCTION(openssl_digest)
{
	zend_bool raw_output = 0;
	char *data, *method;
	size_t data_len, method_len;
	const EVP_MD *mdtype;
	EVP_MD_CTX *md_ctx;
	unsigned int siglen;
	zend_string *sigbuf;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|b", &data, &data_len, &method, &method_len, &raw_output) == FAILURE) {
		RETURN_THROWS();
	}
	mdtype = EVP_get_digestbyname(method);
	if (!mdtype) {
		php_error_docref(NULL, E_WARNING, "Unknown digest algorithm");
		RETURN_FALSE;
	}

	siglen = EVP_MD_size(mdtype);
	sigbuf = zend_string_alloc(siglen, 0);

	md_ctx = EVP_MD_CTX_create();
	if (EVP_DigestInit(md_ctx, mdtype) &&
			EVP_DigestUpdate(md_ctx, (unsigned char *)data, data_len) &&
			EVP_DigestFinal (md_ctx, (unsigned char *)ZSTR_VAL(sigbuf), &siglen)) {
		if (raw_output) {
			ZSTR_VAL(sigbuf)[siglen] = '\0';
			ZSTR_LEN(sigbuf) = siglen;
			RETVAL_STR(sigbuf);
		} else {
			int digest_str_len = siglen * 2;
			zend_string *digest_str = zend_string_alloc(digest_str_len, 0);

			make_digest_ex(ZSTR_VAL(digest_str), (unsigned char*)ZSTR_VAL(sigbuf), siglen);
			ZSTR_VAL(digest_str)[digest_str_len] = '\0';
			zend_string_release_ex(sigbuf, 0);
			RETVAL_NEW_STR(digest_str);
		}
	} else {
		php_openssl_store_errors();
		zend_string_release_ex(sigbuf, 0);
		RETVAL_FALSE;
	}

	EVP_MD_CTX_destroy(md_ctx);
}
/* }}} */

/* Cipher mode info */
struct php_openssl_cipher_mode {
	zend_bool is_aead;
	zend_bool is_single_run_aead;
	int aead_get_tag_flag;
	int aead_set_tag_flag;
	int aead_ivlen_flag;
};

static void php_openssl_load_cipher_mode(struct php_openssl_cipher_mode *mode, const EVP_CIPHER *cipher_type) /* {{{ */
{
	switch (EVP_CIPHER_mode(cipher_type)) {
#ifdef EVP_CIPH_GCM_MODE
		case EVP_CIPH_GCM_MODE:
			mode->is_aead = 1;
			mode->is_single_run_aead = 0;
			mode->aead_get_tag_flag = EVP_CTRL_GCM_GET_TAG;
			mode->aead_set_tag_flag = EVP_CTRL_GCM_SET_TAG;
			mode->aead_ivlen_flag = EVP_CTRL_GCM_SET_IVLEN;
			break;
#endif
#ifdef EVP_CIPH_CCM_MODE
		case EVP_CIPH_CCM_MODE:
			mode->is_aead = 1;
			mode->is_single_run_aead = 1;
			mode->aead_get_tag_flag = EVP_CTRL_CCM_GET_TAG;
			mode->aead_set_tag_flag = EVP_CTRL_CCM_SET_TAG;
			mode->aead_ivlen_flag = EVP_CTRL_CCM_SET_IVLEN;
			break;
#endif
		default:
			memset(mode, 0, sizeof(struct php_openssl_cipher_mode));
	}
}
/* }}} */

static int php_openssl_validate_iv(const char **piv, size_t *piv_len, size_t iv_required_len,
		zend_bool *free_iv, EVP_CIPHER_CTX *cipher_ctx, struct php_openssl_cipher_mode *mode) /* {{{ */
{
	char *iv_new;

	if (mode->is_aead) {
		if (EVP_CIPHER_CTX_ctrl(cipher_ctx, mode->aead_ivlen_flag, *piv_len, NULL) != 1) {
			php_error_docref(NULL, E_WARNING, "Setting of IV length for AEAD mode failed");
			return FAILURE;
		}
		return SUCCESS;
	}

	/* Best case scenario, user behaved */
	if (*piv_len == iv_required_len) {
		return SUCCESS;
	}

	iv_new = ecalloc(1, iv_required_len + 1);

	if (*piv_len == 0) {
		/* BC behavior */
		*piv_len = iv_required_len;
		*piv = iv_new;
		*free_iv = 1;
		return SUCCESS;

	}

	if (*piv_len < iv_required_len) {
		php_error_docref(NULL, E_WARNING,
				"IV passed is only %zd bytes long, cipher expects an IV of precisely %zd bytes, padding with \\0",
				*piv_len, iv_required_len);
		memcpy(iv_new, *piv, *piv_len);
		*piv_len = iv_required_len;
		*piv = iv_new;
		*free_iv = 1;
		return SUCCESS;
	}

	php_error_docref(NULL, E_WARNING,
			"IV passed is %zd bytes long which is longer than the %zd expected by selected cipher, truncating",
			*piv_len, iv_required_len);
	memcpy(iv_new, *piv, iv_required_len);
	*piv_len = iv_required_len;
	*piv = iv_new;
	*free_iv = 1;
	return SUCCESS;

}
/* }}} */

static int php_openssl_cipher_init(const EVP_CIPHER *cipher_type,
		EVP_CIPHER_CTX *cipher_ctx, struct php_openssl_cipher_mode *mode,
		const char **ppassword, size_t *ppassword_len, zend_bool *free_password,
		const char **piv, size_t *piv_len, zend_bool *free_iv,
		const char *tag, int tag_len, zend_long options, int enc)  /* {{{ */
{
	unsigned char *key;
	int key_len, password_len;
	size_t max_iv_len;

	*free_password = 0;

	max_iv_len = EVP_CIPHER_iv_length(cipher_type);
	if (enc && *piv_len == 0 && max_iv_len > 0 && !mode->is_aead) {
		php_error_docref(NULL, E_WARNING,
				"Using an empty Initialization Vector (iv) is potentially insecure and not recommended");
	}

	if (!EVP_CipherInit_ex(cipher_ctx, cipher_type, NULL, NULL, NULL, enc)) {
		php_openssl_store_errors();
		return FAILURE;
	}
	if (php_openssl_validate_iv(piv, piv_len, max_iv_len, free_iv, cipher_ctx, mode) == FAILURE) {
		return FAILURE;
	}
	if (mode->is_single_run_aead && enc) {
		if (!EVP_CIPHER_CTX_ctrl(cipher_ctx, mode->aead_set_tag_flag, tag_len, NULL)) {
			php_error_docref(NULL, E_WARNING, "Setting tag length for AEAD cipher failed");
			return FAILURE;
		}
	} else if (!enc && tag && tag_len > 0) {
		if (!mode->is_aead) {
			php_error_docref(NULL, E_WARNING, "The tag cannot be used because the cipher algorithm does not support AEAD");
		} else if (!EVP_CIPHER_CTX_ctrl(cipher_ctx, mode->aead_set_tag_flag, tag_len, (unsigned char *) tag)) {
			php_error_docref(NULL, E_WARNING, "Setting tag for AEAD cipher decryption failed");
			return FAILURE;
		}
	}
	/* check and set key */
	password_len = (int) *ppassword_len;
	key_len = EVP_CIPHER_key_length(cipher_type);
	if (key_len > password_len) {
		if ((OPENSSL_DONT_ZERO_PAD_KEY & options) && !EVP_CIPHER_CTX_set_key_length(cipher_ctx, password_len)) {
			php_openssl_store_errors();
			php_error_docref(NULL, E_WARNING, "Key length cannot be set for the cipher algorithm");
			return FAILURE;
		}
		key = emalloc(key_len);
		memset(key, 0, key_len);
		memcpy(key, *ppassword, password_len);
		*ppassword = (char *) key;
		*ppassword_len = key_len;
		*free_password = 1;
	} else {
		if (password_len > key_len && !EVP_CIPHER_CTX_set_key_length(cipher_ctx, password_len)) {
			php_openssl_store_errors();
		}
		key = (unsigned char*)*ppassword;
	}

	if (!EVP_CipherInit_ex(cipher_ctx, NULL, NULL, key, (unsigned char *)*piv, enc)) {
		php_openssl_store_errors();
		return FAILURE;
	}
	if (options & OPENSSL_ZERO_PADDING) {
		EVP_CIPHER_CTX_set_padding(cipher_ctx, 0);
	}

	return SUCCESS;
}
/* }}} */

static int php_openssl_cipher_update(const EVP_CIPHER *cipher_type,
		EVP_CIPHER_CTX *cipher_ctx, struct php_openssl_cipher_mode *mode,
		zend_string **poutbuf, int *poutlen, const char *data, size_t data_len,
		const char *aad, size_t aad_len, int enc)  /* {{{ */
{
	int i = 0;

	if (mode->is_single_run_aead && !EVP_CipherUpdate(cipher_ctx, NULL, &i, NULL, (int)data_len)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Setting of data length failed");
		return FAILURE;
	}

	if (mode->is_aead && !EVP_CipherUpdate(cipher_ctx, NULL, &i, (const unsigned char *) aad, (int) aad_len)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Setting of additional application data failed");
		return FAILURE;
	}

	*poutbuf = zend_string_alloc((int)data_len + EVP_CIPHER_block_size(cipher_type), 0);

	if (!EVP_CipherUpdate(cipher_ctx, (unsigned char*)ZSTR_VAL(*poutbuf),
					&i, (const unsigned char *)data, (int)data_len)) {
		/* we don't show warning when we fail but if we ever do, then it should look like this:
		if (mode->is_single_run_aead && !enc) {
			php_error_docref(NULL, E_WARNING, "Tag verifycation failed");
		} else {
			php_error_docref(NULL, E_WARNING, enc ? "Encryption failed" : "Decryption failed");
		}
		*/
		php_openssl_store_errors();
		zend_string_release_ex(*poutbuf, 0);
		return FAILURE;
	}

	*poutlen = i;

	return SUCCESS;
}
/* }}} */


PHP_OPENSSL_API zend_string* php_openssl_encrypt(
	const char *data, size_t data_len,
	const char *method, size_t method_len,
	const char *password, size_t password_len,
	zend_long options,
	const char *iv, size_t iv_len,
	zval *tag, zend_long tag_len,
	const char *aad, size_t aad_len)
{
	const EVP_CIPHER *cipher_type;
	EVP_CIPHER_CTX *cipher_ctx;
	struct php_openssl_cipher_mode mode;
	int i = 0, outlen;
	zend_bool free_iv = 0, free_password = 0;
	zend_string *outbuf = NULL;

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT_NULL_RETURN(data_len, data);
	PHP_OPENSSL_CHECK_SIZE_T_TO_INT_NULL_RETURN(password_len, password);
	PHP_OPENSSL_CHECK_SIZE_T_TO_INT_NULL_RETURN(aad_len, aad);
	PHP_OPENSSL_CHECK_LONG_TO_INT_NULL_RETURN(tag_len, tag_len);


	cipher_type = EVP_get_cipherbyname(method);
	if (!cipher_type) {
		php_error_docref(NULL, E_WARNING, "Unknown cipher algorithm");
		return NULL;
	}

	cipher_ctx = EVP_CIPHER_CTX_new();
	if (!cipher_ctx) {
		php_error_docref(NULL, E_WARNING, "Failed to create cipher context");
		return NULL;
	}

	php_openssl_load_cipher_mode(&mode, cipher_type);

	if (php_openssl_cipher_init(cipher_type, cipher_ctx, &mode,
				&password, &password_len, &free_password,
				&iv, &iv_len, &free_iv, NULL, tag_len, options, 1) == FAILURE ||
			php_openssl_cipher_update(cipher_type, cipher_ctx, &mode, &outbuf, &outlen,
				data, data_len, aad, aad_len, 1) == FAILURE) {
		outbuf = NULL;
	} else if (EVP_EncryptFinal(cipher_ctx, (unsigned char *)ZSTR_VAL(outbuf) + outlen, &i)) {
		outlen += i;
		if (options & OPENSSL_RAW_DATA) {
			ZSTR_VAL(outbuf)[outlen] = '\0';
			ZSTR_LEN(outbuf) = outlen;
		} else {
			zend_string *base64_str;

			base64_str = php_base64_encode((unsigned char*)ZSTR_VAL(outbuf), outlen);
			zend_string_release_ex(outbuf, 0);
			outbuf = base64_str;
		}
		if (mode.is_aead && tag) {
			zend_string *tag_str = zend_string_alloc(tag_len, 0);

			if (EVP_CIPHER_CTX_ctrl(cipher_ctx, mode.aead_get_tag_flag, tag_len, ZSTR_VAL(tag_str)) == 1) {
				ZSTR_VAL(tag_str)[tag_len] = '\0';
				ZSTR_LEN(tag_str) = tag_len;
				ZEND_TRY_ASSIGN_REF_NEW_STR(tag, tag_str);
			} else {
				php_error_docref(NULL, E_WARNING, "Retrieving verification tag failed");
				zend_string_release_ex(tag_str, 0);
				zend_string_release_ex(outbuf, 0);
				outbuf = NULL;
			}
		} else if (tag) {
			ZEND_TRY_ASSIGN_REF_NULL(tag);
			php_error_docref(NULL, E_WARNING,
					"The authenticated tag cannot be provided for cipher that doesn not support AEAD");
		} else if (mode.is_aead) {
			php_error_docref(NULL, E_WARNING, "A tag should be provided when using AEAD mode");
			zend_string_release_ex(outbuf, 0);
			outbuf = NULL;
		}
	} else {
		php_openssl_store_errors();
		zend_string_release_ex(outbuf, 0);
		outbuf = NULL;
	}

	if (free_password) {
		efree((void *) password);
	}
	if (free_iv) {
		efree((void *) iv);
	}
	EVP_CIPHER_CTX_reset(cipher_ctx);
	EVP_CIPHER_CTX_free(cipher_ctx);
	return outbuf;
}

/* {{{ Encrypts given data with given method and key, returns raw or base64 encoded string */
PHP_FUNCTION(openssl_encrypt)
{
	zend_long options = 0, tag_len = 16;
	char *data, *method, *password, *iv = "", *aad = "";
	size_t data_len, method_len, password_len, iv_len = 0, aad_len = 0;
	zend_string *ret;
	zval *tag = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss|lszsl", &data, &data_len, &method, &method_len,
					&password, &password_len, &options, &iv, &iv_len, &tag, &aad, &aad_len, &tag_len) == FAILURE) {
		RETURN_THROWS();
	}

	if ((ret = php_openssl_encrypt(data, data_len, method, method_len, password, password_len, options, iv, iv_len, tag, tag_len, aad, aad_len))) {
		RETVAL_STR(ret);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

PHP_OPENSSL_API zend_string* php_openssl_decrypt(
	const char *data, size_t data_len,
	const char *method, size_t method_len,
	const char *password, size_t password_len,
	zend_long options,
	const char *iv, size_t iv_len,
	const char *tag, zend_long tag_len,
	const char *aad, size_t aad_len)
{
	const EVP_CIPHER *cipher_type;
	EVP_CIPHER_CTX *cipher_ctx;
	struct php_openssl_cipher_mode mode;
	int i = 0, outlen;
	zend_string *base64_str = NULL;
	zend_bool free_iv = 0, free_password = 0;
	zend_string *outbuf = NULL;

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT_NULL_RETURN(data_len, data);
	PHP_OPENSSL_CHECK_SIZE_T_TO_INT_NULL_RETURN(password_len, password);
	PHP_OPENSSL_CHECK_SIZE_T_TO_INT_NULL_RETURN(aad_len, aad);
	PHP_OPENSSL_CHECK_SIZE_T_TO_INT_NULL_RETURN(tag_len, tag);


	cipher_type = EVP_get_cipherbyname(method);
	if (!cipher_type) {
		php_error_docref(NULL, E_WARNING, "Unknown cipher algorithm");
		return NULL;
	}

	cipher_ctx = EVP_CIPHER_CTX_new();
	if (!cipher_ctx) {
		php_error_docref(NULL, E_WARNING, "Failed to create cipher context");
		return NULL;
	}

	php_openssl_load_cipher_mode(&mode, cipher_type);

	if (!(options & OPENSSL_RAW_DATA)) {
		base64_str = php_base64_decode((unsigned char*)data, data_len);
		if (!base64_str) {
			php_error_docref(NULL, E_WARNING, "Failed to base64 decode the input");
			EVP_CIPHER_CTX_free(cipher_ctx);
			return NULL;
		}
		data_len = ZSTR_LEN(base64_str);
		data = ZSTR_VAL(base64_str);
	}

	if (php_openssl_cipher_init(cipher_type, cipher_ctx, &mode,
				&password, &password_len, &free_password,
				&iv, &iv_len, &free_iv, tag, tag_len, options, 0) == FAILURE ||
			php_openssl_cipher_update(cipher_type, cipher_ctx, &mode, &outbuf, &outlen,
				data, data_len, aad, aad_len, 0) == FAILURE) {
		outbuf = NULL;
	} else if (mode.is_single_run_aead ||
			EVP_DecryptFinal(cipher_ctx, (unsigned char *)ZSTR_VAL(outbuf) + outlen, &i)) {
		outlen += i;
		ZSTR_VAL(outbuf)[outlen] = '\0';
		ZSTR_LEN(outbuf) = outlen;
	} else {
		php_openssl_store_errors();
		zend_string_release_ex(outbuf, 0);
		outbuf = NULL;
	}

	if (free_password) {
		efree((void *) password);
	}
	if (free_iv) {
		efree((void *) iv);
	}
	if (base64_str) {
		zend_string_release_ex(base64_str, 0);
	}
	EVP_CIPHER_CTX_reset(cipher_ctx);
	EVP_CIPHER_CTX_free(cipher_ctx);
	return outbuf;
}

/* {{{ Takes raw or base64 encoded string and decrypts it using given method and key */
PHP_FUNCTION(openssl_decrypt)
{
	zend_long options = 0;
	char *data, *method, *password, *iv = "", *tag = NULL, *aad = "";
	size_t data_len, method_len, password_len, iv_len = 0, tag_len = 0, aad_len = 0;
	zend_string *ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss|lsss", &data, &data_len, &method, &method_len,
					&password, &password_len, &options, &iv, &iv_len, &tag, &tag_len, &aad, &aad_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (!method_len) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	if ((ret = php_openssl_decrypt(data, data_len, method, method_len, password, password_len, options, iv, iv_len, tag, tag_len, aad, aad_len))) {
		RETVAL_STR(ret);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

PHP_OPENSSL_API zend_long php_openssl_cipher_iv_length(const char *method)
{
	const EVP_CIPHER *cipher_type;

	cipher_type = EVP_get_cipherbyname(method);
	if (!cipher_type) {
		php_error_docref(NULL, E_WARNING, "Unknown cipher algorithm");
		return -1;
	}

	return EVP_CIPHER_iv_length(cipher_type);
}

/* {{{ */
PHP_FUNCTION(openssl_cipher_iv_length)
{
	char *method;
	size_t method_len;
	zend_long ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &method, &method_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (!method_len) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	/* Warning is emitted in php_openssl_cipher_iv_length */
	if ((ret = php_openssl_cipher_iv_length(method)) == -1) {
		RETURN_FALSE;
	}

	RETURN_LONG(ret);
}
/* }}} */


PHP_OPENSSL_API zend_string* php_openssl_random_pseudo_bytes(zend_long buffer_length)
{
	zend_string *buffer = NULL;
	if (buffer_length <= 0
#ifndef PHP_WIN32
		|| ZEND_LONG_INT_OVFL(buffer_length)
#endif
			) {
		zend_argument_value_error(1, "must be greater than 0");
		return NULL;
	}
	buffer = zend_string_alloc(buffer_length, 0);

#ifdef PHP_WIN32
	/* random/urandom equivalent on Windows */
	if (php_win32_get_random_bytes((unsigned char*)(buffer)->val, (size_t) buffer_length) == FAILURE){
		zend_string_release_ex(buffer, 0);
		zend_throw_exception(zend_ce_exception, "Error reading from source device", 0);
		return NULL;
	}
#else

	PHP_OPENSSL_CHECK_LONG_TO_INT_NULL_RETURN(buffer_length, length);
	PHP_OPENSSL_RAND_ADD_TIME();
	/* FIXME loop if requested size > INT_MAX */
	if (RAND_bytes((unsigned char*)ZSTR_VAL(buffer), (int)buffer_length) <= 0) {
		zend_string_release_ex(buffer, 0);
		zend_throw_exception(zend_ce_exception, "Error reading from source device", 0);
		return NULL;
	} else {
		php_openssl_store_errors();
	}
#endif
	return buffer;
}

/* {{{ Returns a string of the length specified filled with random pseudo bytes */
PHP_FUNCTION(openssl_random_pseudo_bytes)
{
	zend_string *buffer = NULL;
	zend_long buffer_length;
	zval *zstrong_result_returned = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|z", &buffer_length, &zstrong_result_returned) == FAILURE) {
		RETURN_THROWS();
	}

	if (zstrong_result_returned) {
		ZEND_TRY_ASSIGN_REF_FALSE(zstrong_result_returned);
	}

	if ((buffer = php_openssl_random_pseudo_bytes(buffer_length))) {
		ZSTR_VAL(buffer)[buffer_length] = 0;
		RETVAL_NEW_STR(buffer);
	}

	if (zstrong_result_returned) {
		ZEND_TRY_ASSIGN_REF_TRUE(zstrong_result_returned);
	}
}
/* }}} */
