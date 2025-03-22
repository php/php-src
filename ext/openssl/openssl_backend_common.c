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

#include "php_openssl_backend.h"

#include "zend_exceptions.h"
#include "ext/standard/md5.h" /* For make_digest_ex() */
#include "ext/standard/base64.h"
#ifdef PHP_WIN32
# include "win32/winutil.h"
#endif

/* Common */
#include <time.h>

#if (defined(PHP_WIN32) && defined(_MSC_VER))
#define timezone _timezone	/* timezone is called _timezone in LibC */
#endif


/* openssl -> PHP "bridging" */
/* true global; readonly after module startup */
static char default_ssl_conf_filename[MAXPATHLEN];

void php_openssl_add_assoc_name_entry(zval * val, char * key, X509_NAME * name, int shortname)
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

void php_openssl_add_assoc_asn1_string(zval * val, char * key, ASN1_STRING * str)
{
	add_assoc_stringl(val, key, (char *)str->data, str->length);
}

time_t php_openssl_asn1_time_to_time_t(ASN1_UTCTIME * timestr)
{
    /*
     * This is how the time string is formatted:
     *
     * snprintf(p, sizeof(p), "%02d%02d%02d%02d%02d%02dZ",ts->tm_year%100,
     *   ts->tm_mon+1,ts->tm_mday,ts->tm_hour,ts->tm_min,ts->tm_sec);
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

	if (timestr_len < 13) {
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

#ifdef HAVE_STRUCT_TM_TM_GMTOFF
	gmadjust = thetime.tm_gmtoff;
#else
	/*
	 * If correcting for daylight savings time, we set the adjustment to
	 * the value of timezone - 3600 seconds. Otherwise, we need to overcorrect and
	 * set the adjustment to the main timezone + 3600 seconds.
	 */
	gmadjust = -(thetime.tm_isdst ? (long)timezone - 3600 : (long)timezone);
#endif
	ret += gmadjust;

	efree(strbuf);

	return ret;
}

int php_openssl_config_check_syntax(const char * section_label, const char * config_filename, const char * section, CONF *config)
{
	X509V3_CTX ctx;

	X509V3_set_ctx_test(&ctx);
	X509V3_set_nconf(&ctx, config);
	if (!X509V3_EXT_add_nconf(config, &ctx, (char *)section, NULL)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error loading %s section %s of %s",
				section_label,
				section,
				config_filename);
		return FAILURE;
	}
	return SUCCESS;
}

char *php_openssl_conf_get_string(CONF *conf, const char *group, const char *name)
{
	/* OpenSSL reports an error if a configuration value is not found.
	 * However, we don't want to generate errors for optional configuration. */
	ERR_set_mark();
	char *str = NCONF_get_string(conf, group, name);
	ERR_pop_to_mark();
	return str;
}

long php_openssl_conf_get_number(CONF *conf, const char *group, const char *name)
{
	/* Same here, ignore errors. */
	long res = 0;
	ERR_set_mark();
	NCONF_get_number(conf, group, name, &res);
	ERR_pop_to_mark();
	return res;
}

int php_openssl_add_oid_section(struct php_x509_request * req)
{
	char * str;
	STACK_OF(CONF_VALUE) * sktmp;
	CONF_VALUE * cnf;
	int i;

	str = php_openssl_conf_get_string(req->req_config, NULL, "oid_section");
	if (str == NULL) {
		return SUCCESS;
	}
	sktmp = NCONF_get_section(req->req_config, str);
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

int php_openssl_spki_cleanup(const char *src, char *dest)
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


int php_openssl_parse_config(struct php_x509_request * req, zval * optional_args)
{
	char * str, path[MAXPATHLEN];
	zval * item;

	SET_OPTIONAL_STRING_ARG("config", req->config_filename, default_ssl_conf_filename);
	SET_OPTIONAL_STRING_ARG("config_section_name", req->section_name, "req");
	req->global_config = NCONF_new(NULL);
	if (!NCONF_load(req->global_config, default_ssl_conf_filename, NULL)) {
		php_openssl_store_errors();
	}

	req->req_config = NCONF_new(NULL);
	if (!NCONF_load(req->req_config, req->config_filename, NULL)) {
		return FAILURE;
	}

	/* read in the oids */
	str = php_openssl_conf_get_string(req->req_config, NULL, "oid_file");
	if (str != NULL && php_openssl_check_path_ex(str, strlen(str), path, 0, false, false, "oid_file")) {
		BIO *oid_bio = BIO_new_file(path, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
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
		php_openssl_conf_get_string(req->req_config, req->section_name, "default_md"));
	SET_OPTIONAL_STRING_ARG("x509_extensions", req->extensions_section,
		php_openssl_conf_get_string(req->req_config, req->section_name, "x509_extensions"));
	SET_OPTIONAL_STRING_ARG("req_extensions", req->request_extensions_section,
		php_openssl_conf_get_string(req->req_config, req->section_name, "req_extensions"));
	SET_OPTIONAL_LONG_ARG("private_key_bits", req->priv_key_bits,
		php_openssl_conf_get_number(req->req_config, req->section_name, "default_bits"));
	SET_OPTIONAL_LONG_ARG("private_key_type", req->priv_key_type, OPENSSL_KEYTYPE_DEFAULT);

	if (optional_args && (item = zend_hash_str_find(Z_ARRVAL_P(optional_args), "encrypt_key", sizeof("encrypt_key")-1)) != NULL) {
		req->priv_key_encrypt = Z_TYPE_P(item) == IS_TRUE ? 1 : 0;
	} else {
		str = php_openssl_conf_get_string(req->req_config, req->section_name, "encrypt_rsa_key");
		if (str == NULL) {
			str = php_openssl_conf_get_string(req->req_config, req->section_name, "encrypt_key");
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
		req->digest_name = php_openssl_conf_get_string(req->req_config, req->section_name, "default_md");
	}
	if (req->digest_name != NULL) {
		if (strcmp(req->digest_name, "null") == 0) {
			req->digest = req->md_alg = EVP_md_null();
		} else {
			req->digest = req->md_alg = EVP_get_digestbyname(req->digest_name);
		}
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
	str = php_openssl_conf_get_string(req->req_config, req->section_name, "string_mask");
	if (str != NULL && !ASN1_STRING_set_default_mask_asc(str)) {
		php_error_docref(NULL, E_WARNING, "Invalid global string mask setting %s", str);
		return FAILURE;
	}

	PHP_SSL_CONFIG_SYNTAX_CHECK(request_extensions_section);

	return SUCCESS;
}

void php_openssl_dispose_config(struct php_x509_request * req)
{
	if (req->priv_key) {
		EVP_PKEY_free(req->priv_key);
		req->priv_key = NULL;
	}
	if (req->global_config) {
		NCONF_free(req->global_config);
		req->global_config = NULL;
	}
	if (req->req_config) {
		NCONF_free(req->req_config);
		req->req_config = NULL;
	}
}

zend_result php_openssl_load_rand_file(const char * file, int *egdsocket, int *seeded)
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

zend_result php_openssl_write_rand_file(const char * file, int egdsocket, int seeded)
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
	if (file == NULL || !RAND_write_file(file)) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Unable to write random state");
		return FAILURE;
	}
	return SUCCESS;
}

EVP_MD * php_openssl_get_evp_md_from_algo(zend_long algo) {
	EVP_MD *mdtype;

	switch (algo) {
		case OPENSSL_ALGO_SHA1:
			mdtype = (EVP_MD *) EVP_sha1();
			break;
		case OPENSSL_ALGO_MD5:
			mdtype = (EVP_MD *) EVP_md5();
			break;
#ifndef OPENSSL_NO_MD4
		case OPENSSL_ALGO_MD4:
			mdtype = (EVP_MD *) EVP_md4();
			break;
#endif
#ifndef OPENSSL_NO_MD2
		case OPENSSL_ALGO_MD2:
			mdtype = (EVP_MD *) EVP_md2();
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
#ifndef OPENSSL_NO_RMD160
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

const EVP_CIPHER * php_openssl_get_evp_cipher_from_algo(zend_long algo) {
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

void php_openssl_backend_init(void)
{
#ifdef LIBRESSL_VERSION_NUMBER
	OPENSSL_config(NULL);
	SSL_library_init();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
#else
#if PHP_OPENSSL_API_VERSION >= 0x30000 && defined(LOAD_OPENSSL_LEGACY_PROVIDER)
	OSSL_PROVIDER_load(NULL, "legacy");
	OSSL_PROVIDER_load(NULL, "default");
#endif
	OPENSSL_init_ssl(OPENSSL_INIT_LOAD_CONFIG, NULL);
#endif

	/* Determine default SSL configuration file */
	char *config_filename = getenv("OPENSSL_CONF");
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
}

const char *php_openssl_get_conf_filename(void)
{
    return default_ssl_conf_filename;
}

void php_openssl_set_cert_locations(zval *return_value)
{
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

X509 *php_openssl_x509_from_str(
		zend_string *cert_str, uint32_t arg_num, bool is_from_array, const char *option_name) {
	X509 *cert = NULL;
	char cert_path[MAXPATHLEN];
	BIO *in;

	if (ZSTR_LEN(cert_str) > 7 && memcmp(ZSTR_VAL(cert_str), "file://", sizeof("file://") - 1) == 0) {
		if (!php_openssl_check_path_str_ex(cert_str, cert_path, arg_num, true, is_from_array, option_name)) {
			return NULL;
		}

		in = BIO_new_file(cert_path, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
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
X509 *php_openssl_x509_from_param(
		zend_object *cert_obj, zend_string *cert_str, uint32_t arg_num) {
	if (cert_obj) {
		return php_openssl_certificate_from_obj(cert_obj)->x509;
	}

	ZEND_ASSERT(cert_str);

	return php_openssl_x509_from_str(cert_str, arg_num, false, NULL);
}

X509 *php_openssl_x509_from_zval(
		zval *val, bool *free_cert, uint32_t arg_num, bool is_from_array, const char *option_name)
{
	if (php_openssl_is_certificate_ce(val)) {
		*free_cert = 0;

		return php_openssl_certificate_from_obj(Z_OBJ_P(val))->x509;
	}

	*free_cert = 1;

	zend_string *str = zval_try_get_string(val);
	if (str == NULL) {
		return NULL;
	}
	X509 *cert = php_openssl_x509_from_str(str, arg_num, is_from_array, option_name);
	zend_string_release(str);
	return cert;
}

zend_string* php_openssl_x509_fingerprint(X509 *peer, const char *method, bool raw)
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

/* Special handling of subjectAltName, see CVE-2013-4073
 * Christian Heimes
 */
int openssl_x509v3_subjectAltName(BIO *bio, X509_EXTENSION *extension)
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

STACK_OF(X509) *php_openssl_load_all_certs_from_file(
		char *cert_file, size_t cert_file_len, uint32_t arg_num)
{
	STACK_OF(X509_INFO) *sk=NULL;
	STACK_OF(X509) *stack=NULL, *ret=NULL;
	BIO *in=NULL;
	X509_INFO *xi;
	char cert_path[MAXPATHLEN];

	if(!(stack = sk_X509_new_null())) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_ERROR, "Memory allocation failure");
		goto end;
	}

	if (!php_openssl_check_path(cert_file, cert_file_len, cert_path, arg_num)) {
		sk_X509_free(stack);
		goto end;
	}

	if (!(in = BIO_new_file(cert_path, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY)))) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error opening the file, %s", cert_path);
		sk_X509_free(stack);
		goto end;
	}

	/* This loads from a file, a stack of x509/crl/pkey sets */
	if (!(sk = PEM_X509_INFO_read_bio(in, NULL, NULL, NULL))) {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error reading the file, %s", cert_path);
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
		php_error_docref(NULL, E_WARNING, "No certificates in file, %s", cert_path);
		sk_X509_free(stack);
		goto end;
	}
	ret = stack;
end:
	BIO_free(in);
	sk_X509_INFO_free(sk);

	return ret;
}

int php_openssl_check_cert(X509_STORE *ctx, X509 *x, STACK_OF(X509) *untrustedchain, int purpose)
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


/* {{{ php_openssl_setup_verify
 * calist is an array containing file and directory names.  create a
 * certificate store and add those certs to it for use in verification.
*/
X509_STORE *php_openssl_setup_verify(zval *calist, uint32_t arg_num)
{
	X509_STORE *store;
	X509_LOOKUP * dir_lookup, * file_lookup;
	int ndirs = 0, nfiles = 0;
	zval * item;
	zend_stat_t sb = {0};
	char file_path[MAXPATHLEN];

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

			if (!php_openssl_check_path_str_ex(str, file_path, arg_num, false, true, NULL)) {
				zend_string_release(str);
				continue;
			}
			zend_string_release(str);

			if (VCWD_STAT(file_path, &sb) == -1) {
				php_error_docref(NULL, E_WARNING, "Unable to stat %s", file_path);
				continue;
			}

			if ((sb.st_mode & S_IFREG) == S_IFREG) {
				file_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
				if (file_lookup == NULL || !X509_LOOKUP_load_file(file_lookup, file_path, X509_FILETYPE_PEM)) {
					php_openssl_store_errors();
					php_error_docref(NULL, E_WARNING, "Error loading file %s", file_path);
				} else {
					nfiles++;
				}
				file_lookup = NULL;
			} else {
				dir_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
				if (dir_lookup == NULL || !X509_LOOKUP_add_dir(dir_lookup, file_path, X509_FILETYPE_PEM)) {
					php_openssl_store_errors();
					php_error_docref(NULL, E_WARNING, "Error loading directory %s", file_path);
				} else {
					ndirs++;
				}
				dir_lookup = NULL;
			}
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


/* Pop all X509 from Stack and free them, free the stack afterwards */
void php_openssl_sk_X509_free(STACK_OF(X509) * sk)
{
	for (;;) {
		X509* x = sk_X509_pop(sk);
		if (!x) break;
		X509_free(x);
	}
	sk_X509_free(sk);
}

STACK_OF(X509) *php_openssl_array_to_X509_sk(zval * zcerts, uint32_t arg_num, const char *option_name)
{
	zval * zcertval;
	STACK_OF(X509) * sk = NULL;
	X509 * cert;
	bool free_cert;

	sk = sk_X509_new_null();

	/* get certs */
	if (Z_TYPE_P(zcerts) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zcerts), zcertval) {
			cert = php_openssl_x509_from_zval(zcertval, &free_cert, arg_num, true, option_name);
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
		cert = php_openssl_x509_from_zval(zcerts, &free_cert, arg_num, false, option_name);

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

zend_result php_openssl_csr_add_subj_entry(zval *item, X509_NAME *subj, int nid)
{
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
	return SUCCESS;
}

zend_result php_openssl_csr_make(struct php_x509_request * req, X509_REQ * csr, zval * dn, zval * attribs)
{
	STACK_OF(CONF_VALUE) * dn_sk, *attr_sk = NULL;
	char * str, *dn_sect, *attr_sect;

	dn_sect = NCONF_get_string(req->req_config, req->section_name, "distinguished_name");
	if (dn_sect == NULL) {
		php_openssl_store_errors();
		return FAILURE;
	}
	dn_sk = NCONF_get_section(req->req_config, dn_sect);
	if (dn_sk == NULL) {
		php_openssl_store_errors();
		return FAILURE;
	}
	attr_sect = php_openssl_conf_get_string(req->req_config, req->section_name, "attributes");
	if (attr_sect == NULL) {
		attr_sk = NULL;
	} else {
		attr_sk = NCONF_get_section(req->req_config, attr_sect);
		if (attr_sk == NULL) {
			php_openssl_store_errors();
			return FAILURE;
		}
	}
	/* setup the version number: version 1 */
	if (X509_REQ_set_version(csr, 0L)) {
		int i, nid;
		char *type;
		CONF_VALUE *v;
		X509_NAME *subj;
		zval *item, *subitem;
		zend_string *strindex = NULL;

		subj = X509_REQ_get_subject_name(csr);
		/* apply values from the dn hash */
		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(dn), strindex, item) {
			if (strindex) {
				int nid = OBJ_txt2nid(ZSTR_VAL(strindex));
				if (nid != NID_undef) {
					if (Z_TYPE_P(item) == IS_ARRAY) {
						ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(item), i, subitem) {
							if (php_openssl_csr_add_subj_entry(subitem, subj, nid) == FAILURE) {
								return FAILURE;
							}
						} ZEND_HASH_FOREACH_END();
					} else if (php_openssl_csr_add_subj_entry(item, subj, nid) == FAILURE) {
						return FAILURE;
					}
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
					php_error_docref(NULL, E_WARNING, "attributes: numeric fild names are not supported");
					continue;
				}

				nid = OBJ_txt2nid(ZSTR_VAL(strindex));
				if (nid != NID_undef) {
					zend_string *str_item = zval_try_get_string(item);
					if (UNEXPECTED(!str_item)) {
						return FAILURE;
					}
					if (!X509_REQ_add1_attr_by_NID(csr, nid, MBSTRING_UTF8, (unsigned char*)ZSTR_VAL(str_item), (int)ZSTR_LEN(str_item))) {
						php_openssl_store_errors();
						php_error_docref(NULL, E_WARNING, "attributes: add_attr_by_NID %d -> %s (failed)", nid, ZSTR_VAL(str_item));
						zend_string_release(str_item);
						return FAILURE;
					}
					zend_string_release(str_item);
				} else {
					php_error_docref(NULL, E_WARNING, "attributes: %s is not a recognized attribute name", ZSTR_VAL(strindex));
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

X509_REQ *php_openssl_csr_from_str(zend_string *csr_str, uint32_t arg_num)
{
	X509_REQ * csr = NULL;
	char file_path[MAXPATHLEN];
	BIO * in;

	if (ZSTR_LEN(csr_str) > 7 && memcmp(ZSTR_VAL(csr_str), "file://", sizeof("file://") - 1) == 0) {
		if (!php_openssl_check_path_str(csr_str, file_path, arg_num)) {
			return NULL;
		}
		in = BIO_new_file(file_path, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
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

X509_REQ *php_openssl_csr_from_param(
		zend_object *csr_obj, zend_string *csr_str, uint32_t arg_num)
{
	if (csr_obj) {
		return php_openssl_request_from_obj(csr_obj)->csr;
	}

	ZEND_ASSERT(csr_str);

	return php_openssl_csr_from_str(csr_str, arg_num);
}

EVP_PKEY *php_openssl_extract_public_key(EVP_PKEY *priv_key)
{
	/* Extract public key portion by round-tripping through PEM. */
	BIO *bio = BIO_new(BIO_s_mem());
	if (!bio || !PEM_write_bio_PUBKEY(bio, priv_key)) {
		BIO_free(bio);
		return NULL;
	}

	EVP_PKEY *pub_key = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
	BIO_free(bio);
	return pub_key;
}


/* {{{ php_openssl_pem_password_cb */
int php_openssl_pem_password_cb(char *buf, int size, int rwflag, void *userdata)
{
	struct php_openssl_pem_password *password = userdata;

	if (password == NULL || password->key == NULL) {
		return -1;
	}

	size = (password->len > size) ? size : password->len;
	memcpy(buf, password->key, size);

	return size;
}

EVP_PKEY *php_openssl_pkey_from_zval(
		zval *val, int public_key, char *passphrase, size_t passphrase_len, uint32_t arg_num)
{
	EVP_PKEY *key = NULL;
	X509 *cert = NULL;
	bool free_cert = false, is_file = false;
	char file_path[MAXPATHLEN];
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
				zval_ptr_dtor(&tmp);
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

	if (php_openssl_is_pkey_ce(val)) {
		php_openssl_pkey_object *obj = php_openssl_pkey_from_obj(Z_OBJ_P(val));
		key = obj->pkey;
		bool is_priv = obj->is_private;

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
	} else if (php_openssl_is_certificate_ce(val)) {
		cert = php_openssl_certificate_from_obj(Z_OBJ_P(val))->x509;
	} else {
		/* force it to be a string and check if it refers to a file */
		/* passing non string values leaks, object uses toString, it returns NULL
		 * See bug38255.phpt
		 */
		if (!(Z_TYPE_P(val) == IS_STRING || Z_TYPE_P(val) == IS_OBJECT)) {
			TMP_CLEAN;
		}
		zend_string *val_str = zval_try_get_string(val);
		if (!val_str) {
			TMP_CLEAN;
		}

		if (ZSTR_LEN(val_str) > 7 && memcmp(ZSTR_VAL(val_str), "file://", sizeof("file://") - 1) == 0) {
			if (!php_openssl_check_path_str(val_str, file_path, arg_num)) {
				zend_string_release_ex(val_str, false);
				TMP_CLEAN;
			}
			is_file = true;
		}
		/* it's an X509 file/cert of some kind, and we need to extract the data from that */
		if (public_key) {
            php_openssl_errors_set_mark();
			cert = php_openssl_x509_from_str(val_str, arg_num, false, NULL);

			if (cert) {
				free_cert = 1;
			} else {
				/* not a X509 certificate, try to retrieve public key */
				php_openssl_errors_restore_mark();
				BIO* in;
				if (is_file) {
					in = BIO_new_file(file_path, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
				} else {
					in = BIO_new_mem_buf(ZSTR_VAL(val_str), (int)ZSTR_LEN(val_str));
				}
				if (in == NULL) {
					php_openssl_store_errors();
					zend_string_release_ex(val_str, false);
					TMP_CLEAN;
				}
				key = PEM_read_bio_PUBKEY(in, NULL,NULL, NULL);
				BIO_free(in);
			}
		} else {
			/* we want the private key */
			BIO *in;

			if (is_file) {
				in = BIO_new_file(file_path, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
			} else {
				in = BIO_new_mem_buf(ZSTR_VAL(val_str), (int)ZSTR_LEN(val_str));
			}

			if (in == NULL) {
				zend_string_release_ex(val_str, false);
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

		zend_string_release_ex(val_str, false);
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

zend_string *php_openssl_pkey_derive(EVP_PKEY *key, EVP_PKEY *peer_key, size_t key_size)
{
	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(key, NULL);
	if (!ctx) {
		return NULL;
	}

	if (EVP_PKEY_derive_init(ctx) <= 0 ||
			EVP_PKEY_derive_set_peer(ctx, peer_key) <= 0 ||
			(key_size == 0 && EVP_PKEY_derive(ctx, NULL, &key_size) <= 0)) {
		php_openssl_store_errors();
		EVP_PKEY_CTX_free(ctx);
		return NULL;
	}

	zend_string *result = zend_string_alloc(key_size, 0);
	if (EVP_PKEY_derive(ctx, (unsigned char *)ZSTR_VAL(result), &key_size) <= 0) {
		php_openssl_store_errors();
		zend_string_release_ex(result, 0);
		EVP_PKEY_CTX_free(ctx);
		return NULL;
	}

	ZSTR_LEN(result) = key_size;
	ZSTR_VAL(result)[key_size] = 0;
	EVP_PKEY_CTX_free(ctx);
	return result;
}

int php_openssl_get_evp_pkey_type(int key_type) {
	switch (key_type) {
	case OPENSSL_KEYTYPE_RSA:
		return EVP_PKEY_RSA;
#if !defined(OPENSSL_NO_DSA)
	case OPENSSL_KEYTYPE_DSA:
		return EVP_PKEY_DSA;
#endif
#if !defined(NO_DH)
	case OPENSSL_KEYTYPE_DH:
		return EVP_PKEY_DH;
#endif
#ifdef HAVE_EVP_PKEY_EC
	case OPENSSL_KEYTYPE_EC:
		return EVP_PKEY_EC;
#endif
#if PHP_OPENSSL_API_VERSION >= 0x30000
	case OPENSSL_KEYTYPE_X25519:
		return EVP_PKEY_X25519;
	case OPENSSL_KEYTYPE_ED25519:
		return EVP_PKEY_ED25519;
	case OPENSSL_KEYTYPE_X448:
		return EVP_PKEY_X448;
	case OPENSSL_KEYTYPE_ED448:
		return EVP_PKEY_ED448;
#endif
	default:
		return -1;
	}
}

EVP_PKEY * php_openssl_generate_private_key(struct php_x509_request * req)
{
	if (req->priv_key_bits < MIN_KEY_LENGTH) {
		php_error_docref(NULL, E_WARNING, "Private key length must be at least %d bits, configured to %d",
			MIN_KEY_LENGTH, req->priv_key_bits);
		return NULL;
	}

	int type = php_openssl_get_evp_pkey_type(req->priv_key_type);
	if (type < 0) {
		php_error_docref(NULL, E_WARNING, "Unsupported private key type");
		return NULL;
	}

	int egdsocket, seeded;
	char *randfile = php_openssl_conf_get_string(req->req_config, req->section_name, "RANDFILE");
	php_openssl_load_rand_file(randfile, &egdsocket, &seeded);

	EVP_PKEY *key = NULL;
	EVP_PKEY *params = NULL;
	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(type, NULL);
	if (!ctx) {
		php_openssl_store_errors();
		goto cleanup;
	}

	if (type != EVP_PKEY_RSA) {
		if (EVP_PKEY_paramgen_init(ctx) <= 0) {
			php_openssl_store_errors();
			goto cleanup;
		}

		switch (type) {
#if !defined(OPENSSL_NO_DSA)
		case EVP_PKEY_DSA:
			if (EVP_PKEY_CTX_set_dsa_paramgen_bits(ctx, req->priv_key_bits) <= 0) {
				php_openssl_store_errors();
				goto cleanup;
			}
			break;
#endif
#if !defined(NO_DH)
		case EVP_PKEY_DH:
			if (EVP_PKEY_CTX_set_dh_paramgen_prime_len(ctx, req->priv_key_bits) <= 0) {
				php_openssl_store_errors();
				goto cleanup;
			}
			break;
#endif
#ifdef HAVE_EVP_PKEY_EC
		case EVP_PKEY_EC:
			if (req->curve_name == NID_undef) {
				php_error_docref(NULL, E_WARNING, "Missing configuration value: \"curve_name\" not set");
				goto cleanup;
			}

			if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, req->curve_name) <= 0 ||
					EVP_PKEY_CTX_set_ec_param_enc(ctx, OPENSSL_EC_NAMED_CURVE) <= 0) {
				php_openssl_store_errors();
				goto cleanup;
			}
			break;
#endif
#if PHP_OPENSSL_API_VERSION >= 0x30000
		case EVP_PKEY_X25519:
			break;
		case EVP_PKEY_ED25519:
			break;
		case EVP_PKEY_X448:
			break;
		case EVP_PKEY_ED448:
			break;
#endif
		EMPTY_SWITCH_DEFAULT_CASE()
		}

		if (EVP_PKEY_paramgen(ctx, &params) <= 0) {
			php_openssl_store_errors();
			goto cleanup;
		}

		EVP_PKEY_CTX_free(ctx);
		ctx = EVP_PKEY_CTX_new(params, NULL);
		if (!ctx) {
			php_openssl_store_errors();
			goto cleanup;
		}
	}

	if (EVP_PKEY_keygen_init(ctx) <= 0) {
		php_openssl_store_errors();
		goto cleanup;
	}

	if (type == EVP_PKEY_RSA && EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, req->priv_key_bits) <= 0) {
		php_openssl_store_errors();
		goto cleanup;
	}

	if (EVP_PKEY_keygen(ctx, &key) <= 0) {
		php_openssl_store_errors();
		goto cleanup;
	}

	req->priv_key = key;

cleanup:
	php_openssl_write_rand_file(randfile, egdsocket, seeded);
	EVP_PKEY_free(params);
	EVP_PKEY_CTX_free(ctx);
	return key;
}

void php_openssl_add_bn_to_array(zval *ary, const BIGNUM *bn, const char *name) {
	if (bn != NULL) {
		int len = BN_num_bytes(bn);
		zend_string *str = zend_string_alloc(len, 0);
		BN_bn2bin(bn, (unsigned char *)ZSTR_VAL(str));
		ZSTR_VAL(str)[len] = 0;
		add_assoc_str(ary, name, str);
	}
}

BIGNUM *php_openssl_dh_pub_from_priv(BIGNUM *priv_key, BIGNUM *g, BIGNUM *p)
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

BIO *php_openssl_bio_new_file(
		const char *filename, size_t filename_len, uint32_t arg_num, const char *mode)
{
	char file_path[MAXPATHLEN];
	BIO *bio;

	if (!php_openssl_check_path(filename, filename_len, file_path, arg_num)) {
		return NULL;
	}

	bio = BIO_new_file(file_path, mode);
	if (bio == NULL) {
		php_openssl_store_errors();
		return NULL;
	}

	return bio;
}

void php_openssl_add_method_or_alias(const OBJ_NAME *name, void *arg)
{
	add_next_index_string((zval*)arg, (char*)name->name);
}

void php_openssl_add_method(const OBJ_NAME *name, void *arg)
{
	if (name->alias == 0) {
		add_next_index_string((zval*)arg, (char*)name->name);
	}
}

void php_openssl_get_md_methods(zval *return_value, bool aliases)
{
    array_init(return_value);
	OBJ_NAME_do_all_sorted(OBJ_NAME_TYPE_MD_METH,
		aliases ? php_openssl_add_method_or_alias: php_openssl_add_method,
		return_value);
}

void php_openssl_load_cipher_mode(struct php_openssl_cipher_mode *mode, const EVP_CIPHER *cipher_type)
{
	int cipher_mode = EVP_CIPHER_mode(cipher_type);
	memset(mode, 0, sizeof(struct php_openssl_cipher_mode));
	switch (cipher_mode) {
		case EVP_CIPH_GCM_MODE:
		case EVP_CIPH_CCM_MODE:
		/* We check for EVP_CIPH_OCB_MODE, because LibreSSL does not support it. */
#ifdef EVP_CIPH_OCB_MODE
		case EVP_CIPH_OCB_MODE:
			/* For OCB mode, explicitly set the tag length even when decrypting,
			 * see https://github.com/openssl/openssl/issues/8331. */
			mode->set_tag_length_always = cipher_mode == EVP_CIPH_OCB_MODE;
#endif
			php_openssl_set_aead_flags(mode);
			mode->set_tag_length_when_encrypting = cipher_mode == EVP_CIPH_CCM_MODE;
			mode->is_single_run_aead = cipher_mode == EVP_CIPH_CCM_MODE;
			break;
#ifdef NID_chacha20_poly1305
		default:
			if (EVP_CIPHER_nid(cipher_type) == NID_chacha20_poly1305) {
				php_openssl_set_aead_flags(mode);
			}
			break;
#endif
	}
}

zend_result php_openssl_validate_iv(const char **piv, size_t *piv_len, size_t iv_required_len,
		bool *free_iv, EVP_CIPHER_CTX *cipher_ctx, struct php_openssl_cipher_mode *mode)
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

zend_result php_openssl_cipher_init(const EVP_CIPHER *cipher_type,
		EVP_CIPHER_CTX *cipher_ctx, struct php_openssl_cipher_mode *mode,
		const char **ppassword, size_t *ppassword_len, bool *free_password,
		const char **piv, size_t *piv_len, bool *free_iv,
		const char *tag, int tag_len, zend_long options, int enc)
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
	if (mode->set_tag_length_always || (enc && mode->set_tag_length_when_encrypting)) {
		if (!EVP_CIPHER_CTX_ctrl(cipher_ctx, mode->aead_set_tag_flag, tag_len, NULL)) {
			php_error_docref(NULL, E_WARNING, "Setting tag length for AEAD cipher failed");
			return FAILURE;
		}
	}
	if (!enc && tag && tag_len > 0) {
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

zend_result php_openssl_cipher_update(const EVP_CIPHER *cipher_type,
		EVP_CIPHER_CTX *cipher_ctx, struct php_openssl_cipher_mode *mode,
		zend_string **poutbuf, int *poutlen, const char *data, size_t data_len,
		const char *aad, size_t aad_len, int enc)
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
	bool free_iv = 0, free_password = 0;
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
	bool free_iv = 0, free_password = 0;
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

const EVP_CIPHER *php_openssl_get_evp_cipher_by_name(const char *method)
{
	const EVP_CIPHER *cipher_type;

	cipher_type = EVP_get_cipherbyname(method);
	if (!cipher_type) {
		php_error_docref(NULL, E_WARNING, "Unknown cipher algorithm");
		return NULL;
	}

	return cipher_type;
}


PHP_OPENSSL_API zend_long php_openssl_cipher_iv_length(const char *method)
{
	const EVP_CIPHER *cipher_type = php_openssl_get_evp_cipher_by_name(method);

	return cipher_type == NULL ? -1 : EVP_CIPHER_iv_length(cipher_type);
}

PHP_OPENSSL_API zend_long php_openssl_cipher_key_length(const char *method)
{
	const EVP_CIPHER *cipher_type = php_openssl_get_evp_cipher_by_name(method);

	return cipher_type == NULL ? -1 : EVP_CIPHER_key_length(cipher_type);
}

PHP_OPENSSL_API zend_string* php_openssl_random_pseudo_bytes(zend_long buffer_length)
{
	zend_string *buffer = NULL;
	if (buffer_length <= 0) {
		zend_argument_value_error(1, "must be greater than 0");
		return NULL;
	}
	if (ZEND_LONG_INT_OVFL(buffer_length)) {
		zend_argument_value_error(1, "must be less than or equal to %d", INT_MAX);
		return NULL;
	}
	buffer = zend_string_alloc(buffer_length, 0);

	PHP_OPENSSL_CHECK_LONG_TO_INT_NULL_RETURN(buffer_length, length);
	if (RAND_bytes((unsigned char*)ZSTR_VAL(buffer), (int)buffer_length) <= 0) {
		php_openssl_store_errors();
		zend_string_release_ex(buffer, 0);
		zend_throw_exception(zend_ce_exception, "Error reading from source device", 0);
		return NULL;
	}

	return buffer;
}
