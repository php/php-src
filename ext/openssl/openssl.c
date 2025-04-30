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
#include <config.h>
#endif

#include "php.h"
#include "php_ini.h"
#include "php_openssl.h"
#include "php_openssl_backend.h"
#include "zend_attributes.h"
#include "zend_exceptions.h"

/* PHP Includes */
#include "ext/standard/file.h"
#include "ext/standard/info.h"
#include "ext/standard/php_fopen_wrappers.h"
#include "ext/standard/md5.h" /* For make_digest_ex() */
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
#include <openssl/cms.h>
#if PHP_OPENSSL_API_VERSION >= 0x30000
#include <openssl/core_names.h>
#include <openssl/param_build.h>
#include <openssl/provider.h>
#endif

ZEND_DECLARE_MODULE_GLOBALS(openssl)

#include "openssl_arginfo.h"

/* OpenSSLCertificate class */

zend_class_entry *php_openssl_certificate_ce;

static zend_object_handlers php_openssl_certificate_object_handlers;

bool php_openssl_is_certificate_ce(zval *val)
{
	return Z_TYPE_P(val) == IS_OBJECT && Z_OBJCE_P(val) == php_openssl_certificate_ce;
}

static zend_object *php_openssl_certificate_create_object(zend_class_entry *class_type) {
	php_openssl_certificate_object *intern = zend_object_alloc(sizeof(php_openssl_certificate_object), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

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

static zend_class_entry *php_openssl_request_ce;

static zend_object_handlers php_openssl_request_object_handlers;

bool php_openssl_is_request_ce(zval *val)
{
	return Z_TYPE_P(val) == IS_OBJECT && Z_OBJCE_P(val) == php_openssl_request_ce;
}

static zend_object *php_openssl_request_create_object(zend_class_entry *class_type) {
	php_openssl_request_object *intern = zend_object_alloc(sizeof(php_openssl_request_object), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

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

static zend_class_entry *php_openssl_pkey_ce;

static zend_object_handlers php_openssl_pkey_object_handlers;

bool php_openssl_is_pkey_ce(zval *val)
{
	return Z_TYPE_P(val) == IS_OBJECT && Z_OBJCE_P(val) == php_openssl_pkey_ce;
}

void php_openssl_pkey_object_init(zval *zv, EVP_PKEY *pkey, bool is_private)
{
	object_init_ex(zv, php_openssl_pkey_ce);
	php_openssl_pkey_object *obj = Z_OPENSSL_PKEY_P(zv);
	obj->pkey = pkey;
	obj->is_private = is_private;
}

static zend_object *php_openssl_pkey_create_object(zend_class_entry *class_type)
{
	php_openssl_pkey_object *intern = zend_object_alloc(sizeof(php_openssl_pkey_object), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	return &intern->std;
}

static zend_function *php_openssl_pkey_get_constructor(zend_object *object)
{
	zend_throw_error(NULL, "Cannot directly construct OpenSSLAsymmetricKey, use openssl_pkey_new() instead");
	return NULL;
}

static void php_openssl_pkey_free_obj(zend_object *object)
{
	php_openssl_pkey_object *key_object = php_openssl_pkey_from_obj(object);

	EVP_PKEY_free(key_object->pkey);
	zend_object_std_dtor(&key_object->std);
}

#if defined(HAVE_OPENSSL_ARGON2)
static const zend_module_dep openssl_deps[] = {
	ZEND_MOD_REQUIRED("standard")
	ZEND_MOD_END
};
#endif
/* {{{ openssl_module_entry */
zend_module_entry openssl_module_entry = {
#if defined(HAVE_OPENSSL_ARGON2)
	STANDARD_MODULE_HEADER_EX, NULL,
	openssl_deps,
#else
	STANDARD_MODULE_HEADER,
#endif
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

/* {{{ php_openssl_store_errors */
void php_openssl_store_errors(void)
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

/* {{{ php_openssl_errors_set_mark */
void php_openssl_errors_set_mark(void) {
	if (!OPENSSL_G(errors)) {
		return;
	}

	if (!OPENSSL_G(errors_mark)) {
		OPENSSL_G(errors_mark) = pecalloc(1, sizeof(struct php_openssl_errors), 1);
	}

	memcpy(OPENSSL_G(errors_mark), OPENSSL_G(errors), sizeof(struct php_openssl_errors));
}
/* }}} */

/* {{{ php_openssl_errors_restore_mark */
void php_openssl_errors_restore_mark(void) {
	if (!OPENSSL_G(errors)) {
		return;
	}

	struct php_openssl_errors *errors = OPENSSL_G(errors);

	if (!OPENSSL_G(errors_mark)) {
		errors->top = 0;
		errors->bottom = 0;
	} else {
		memcpy(errors, OPENSSL_G(errors_mark), sizeof(struct php_openssl_errors));
	}
}
/* }}} */

/* openssl file path check error function */
static void php_openssl_check_path_error(uint32_t arg_num, int type, const char *format, ...)
{
	va_list va;
	const char *arg_name;

	va_start(va, format);

	if (type == E_ERROR) {
		zend_argument_error_variadic(zend_ce_value_error, arg_num, format, va);
	} else {
		arg_name = get_active_function_arg_name(arg_num);
		php_verror(NULL, arg_name, type, format, va);
	}
	va_end(va);
}

/* openssl file path check extended */
bool php_openssl_check_path_ex(
		const char *file_path, size_t file_path_len, char *real_path, uint32_t arg_num,
		bool contains_file_protocol, bool is_from_array, const char *option_name)
{
	const char *fs_file_path;
	size_t fs_file_path_len;
	const char *error_msg = NULL;
	int error_type = E_WARNING;

	if (file_path_len == 0) {
		real_path[0] = '\0';
		return true;
	}

	if (contains_file_protocol) {
		size_t path_prefix_len = sizeof("file://") - 1;
		if (file_path_len <= path_prefix_len) {
			return false;
		}
		fs_file_path = file_path + path_prefix_len;
		fs_file_path_len = file_path_len - path_prefix_len;
	} else {
		fs_file_path = file_path;
		fs_file_path_len = file_path_len;
	}

	if (CHECK_NULL_PATH(fs_file_path, fs_file_path_len)) {
		error_msg = "must not contain any null bytes";
		error_type = E_ERROR;
	} else if (expand_filepath(fs_file_path, real_path) == NULL) {
		error_msg = "must be a valid file path";
	}

	if (error_msg != NULL) {
		if (arg_num == 0) {
			const char *option_title = option_name ? option_name : "unknown";
			const char *option_label = is_from_array ? "array item" : "option";
			php_error_docref(NULL, E_WARNING, "Path for %s %s %s",
					option_title, option_label, error_msg);
		} else if (is_from_array && option_name != NULL) {
			php_openssl_check_path_error(
					arg_num, error_type, "option %s array item %s", option_name, error_msg);
		} else if (is_from_array) {
			php_openssl_check_path_error(arg_num, error_type, "array item %s", error_msg);
		} else if (option_name != NULL) {
			php_openssl_check_path_error(
					arg_num, error_type, "option %s %s", option_name, error_msg);
		} else {
			php_openssl_check_path_error(arg_num, error_type, "%s", error_msg);
		}
	} else if (!php_check_open_basedir(real_path)) {
		return true;
	}

	return false;
}

static int ssl_stream_data_index;

php_stream* php_openssl_get_stream_from_ssl_handle(const SSL *ssl)
{
	return (php_stream*)SSL_get_ex_data(ssl, ssl_stream_data_index);
}

int php_openssl_get_ssl_stream_data_index(void)
{
	return ssl_stream_data_index;
}

/* {{{ INI Settings */
PHP_INI_BEGIN()
	PHP_INI_ENTRY("openssl.cafile", NULL, PHP_INI_PERDIR, NULL)
	PHP_INI_ENTRY("openssl.capath", NULL, PHP_INI_PERDIR, NULL)
PHP_INI_END()
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(openssl)
{
	php_openssl_certificate_ce = register_class_OpenSSLCertificate();
	php_openssl_certificate_ce->create_object = php_openssl_certificate_create_object;
	php_openssl_certificate_ce->default_object_handlers = &php_openssl_certificate_object_handlers;

	memcpy(&php_openssl_certificate_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_openssl_certificate_object_handlers.offset = XtOffsetOf(php_openssl_certificate_object, std);
	php_openssl_certificate_object_handlers.free_obj = php_openssl_certificate_free_obj;
	php_openssl_certificate_object_handlers.get_constructor = php_openssl_certificate_get_constructor;
	php_openssl_certificate_object_handlers.clone_obj = NULL;
	php_openssl_certificate_object_handlers.compare = zend_objects_not_comparable;

	php_openssl_request_ce = register_class_OpenSSLCertificateSigningRequest();
	php_openssl_request_ce->create_object = php_openssl_request_create_object;
	php_openssl_request_ce->default_object_handlers = &php_openssl_request_object_handlers;

	memcpy(&php_openssl_request_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_openssl_request_object_handlers.offset = XtOffsetOf(php_openssl_request_object, std);
	php_openssl_request_object_handlers.free_obj = php_openssl_request_free_obj;
	php_openssl_request_object_handlers.get_constructor = php_openssl_request_get_constructor;
	php_openssl_request_object_handlers.clone_obj = NULL;
	php_openssl_request_object_handlers.compare = zend_objects_not_comparable;

	php_openssl_pkey_ce = register_class_OpenSSLAsymmetricKey();
	php_openssl_pkey_ce->create_object = php_openssl_pkey_create_object;
	php_openssl_pkey_ce->default_object_handlers = &php_openssl_pkey_object_handlers;

	memcpy(&php_openssl_pkey_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_openssl_pkey_object_handlers.offset = XtOffsetOf(php_openssl_pkey_object, std);
	php_openssl_pkey_object_handlers.free_obj = php_openssl_pkey_free_obj;
	php_openssl_pkey_object_handlers.get_constructor = php_openssl_pkey_get_constructor;
	php_openssl_pkey_object_handlers.clone_obj = NULL;
	php_openssl_pkey_object_handlers.compare = zend_objects_not_comparable;

	register_openssl_symbols(module_number);

	php_openssl_backend_init();

	/* register a resource id number with OpenSSL so that we can map SSL -> stream structures in
	 * OpenSSL callbacks */
	ssl_stream_data_index = SSL_get_ex_new_index(0, "PHP stream index", NULL, NULL, NULL);

	php_stream_xport_register("ssl", php_openssl_ssl_socket_factory);
#ifndef OPENSSL_NO_SSL3
	php_stream_xport_register("sslv3", php_openssl_ssl_socket_factory);
#endif
	php_stream_xport_register("tls", php_openssl_ssl_socket_factory);
	php_stream_xport_register("tlsv1.0", php_openssl_ssl_socket_factory);
	php_stream_xport_register("tlsv1.1", php_openssl_ssl_socket_factory);
	php_stream_xport_register("tlsv1.2", php_openssl_ssl_socket_factory);
	php_stream_xport_register("tlsv1.3", php_openssl_ssl_socket_factory);

	/* override the default tcp socket provider */
	php_stream_xport_register("tcp", php_openssl_ssl_socket_factory);

	php_register_url_stream_wrapper("https", &php_stream_http_wrapper);
	php_register_url_stream_wrapper("ftps", &php_stream_ftp_wrapper);

	REGISTER_INI_ENTRIES();

#if defined(HAVE_OPENSSL_ARGON2)
	if (FAILURE == PHP_MINIT(openssl_pwhash)(INIT_FUNC_ARGS_PASSTHRU)) {
		return FAILURE;
	}
#endif

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
	openssl_globals->errors_mark = NULL;
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION */
PHP_GSHUTDOWN_FUNCTION(openssl)
{
	if (openssl_globals->errors) {
		pefree(openssl_globals->errors, 1);
	}
	if (openssl_globals->errors_mark) {
		pefree(openssl_globals->errors_mark, 1);
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
	php_info_print_table_row(2, "Openssl default config", php_openssl_get_conf_filename());
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(openssl)
{
	php_openssl_backend_shutdown();

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
	php_stream_xport_unregister("tlsv1.3");

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
	php_openssl_set_cert_locations(return_value);
}
/* }}} */

/* {{{ Exports a CERT to file or a var */
PHP_FUNCTION(openssl_x509_export_to_file)
{
	X509 *cert;
	zend_object *cert_obj;
	zend_string *cert_str;

	bool notext = 1;
	BIO * bio_out;
	char * filename, file_path[MAXPATHLEN];
	size_t filename_len;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(notext)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	cert = php_openssl_x509_from_param(cert_obj, cert_str, 1);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		return;
	}

	if (!php_openssl_check_path(filename, filename_len, file_path, 2)) {
		goto exit_cleanup_cert;
	}

	bio_out = BIO_new_file(file_path, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
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
		php_error_docref(NULL, E_WARNING, "Error opening file %s", file_path);
	}

	if (!BIO_free(bio_out)) {
		php_openssl_store_errors();
	}

exit_cleanup_cert:
	if (cert_str) {
		X509_free(cert);
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

	pkey = php_openssl_pkey_from_zval(zpkey, 0, challenge, challenge_len, 1);
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

	s = zend_string_concat2(spkac, strlen(spkac), spkstr, strlen(spkstr));
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
	char *spkstr, * spkstr_cleaned = NULL;
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
	bool notext = 1;
	BIO * bio_out;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_ZVAL(zout)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(notext)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	cert = php_openssl_x509_from_param(cert_obj, cert_str, 1);
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

PHP_FUNCTION(openssl_x509_fingerprint)
{
	X509 *cert;
	zend_object *cert_obj;
	zend_string *cert_str;
	bool raw_output = 0;
	char *method = "sha1";
	size_t method_len;
	zend_string *fingerprint;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(method, method_len)
		Z_PARAM_BOOL(raw_output)
	ZEND_PARSE_PARAMETERS_END();

	cert = php_openssl_x509_from_param(cert_obj, cert_str, 1);
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

	cert = php_openssl_x509_from_param(cert_obj, cert_str, 1);
	if (cert == NULL) {
		RETURN_FALSE;
	}

	RETVAL_FALSE;

	key = php_openssl_pkey_from_zval(zkey, 0, "", 0, 2);
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

	cert = php_openssl_x509_from_param(cert_obj, cert_str, 1);
	if (cert == NULL) {
		RETURN_LONG(err);
	}

	key = php_openssl_pkey_from_zval(zkey, 1, NULL, 0, 2);
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

/* {{{ Returns an array of the fields/values of the CERT */
PHP_FUNCTION(openssl_x509_parse)
{
	X509 *cert;
	zend_object *cert_obj;
	zend_string *cert_str;
	int i, sig_nid;
	bool useshortnames = 1;
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

	cert = php_openssl_x509_from_param(cert_obj, cert_str, 1);
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
		goto err;
	}

	hex_serial = BN_bn2hex(bn_serial);
	BN_free(bn_serial);
	/* Can return NULL on error or memory allocation failure */
	if (!hex_serial) {
		php_openssl_store_errors();
		goto err;
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
			goto err_subitem;
		}
		if (nid == NID_subject_alt_name) {
			if (openssl_x509v3_subjectAltName(bio_out, extension) == 0) {
				BIO_get_mem_ptr(bio_out, &bio_buf);
				add_assoc_stringl(&subitem, extname, bio_buf->data, bio_buf->length);
			} else {
				BIO_free(bio_out);
				goto err_subitem;
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
	return;

err_subitem:
	zval_ptr_dtor(&subitem);
err:
	zend_array_destroy(Z_ARR_P(return_value));
	if (cert_str) {
		X509_free(cert);
	}
	RETURN_FALSE;
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
		untrustedchain = php_openssl_load_all_certs_from_file(untrusted, untrusted_len, 4);
		if (untrustedchain == NULL) {
			goto clean_exit;
		}
	}

	cainfo = php_openssl_setup_verify(zcainfo, 3);
	if (cainfo == NULL) {
		goto clean_exit;
	}
	cert = php_openssl_x509_from_param(cert_obj, cert_str, 1);
	if (cert == NULL) {
		// TODO Add Warning?
		goto clean_exit;
	}

	ret = php_openssl_check_cert(cainfo, cert, untrustedchain, (int)purpose);
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

	cert = php_openssl_x509_from_param(cert_obj, cert_str, 1);
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

/* {{{ Creates and exports a PKCS to file */
PHP_FUNCTION(openssl_pkcs12_export_to_file)
{
	X509 *cert;
	zend_object *cert_obj;
	zend_string *cert_str;
	BIO * bio_out = NULL;
	PKCS12 * p12 = NULL;
	char * filename, file_path[MAXPATHLEN];
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

	cert = php_openssl_x509_from_param(cert_obj, cert_str, 1);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		return;
	}

	priv_key = php_openssl_pkey_from_zval(zpkey, 0, "", 0, 3);
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
	if (!php_openssl_check_path(filename, filename_len, file_path, 2)) {
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
		ca = php_openssl_array_to_X509_sk(item, 5, "extracerts");
	}
	/* end parse extra config */

	/*PKCS12 *PKCS12_create(char *pass, char *name, EVP_PKEY *pkey, X509 *cert, STACK_OF(X509) *ca,
				int nid_key, int nid_cert, int iter, int mac_iter, int keytype);*/

	p12 = PKCS12_create(pass, friendly_name, priv_key, cert, ca, 0, 0, 0, 0, 0);
	if (p12 != NULL) {
		bio_out = BIO_new_file(file_path, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
		if (bio_out != NULL) {
			if (i2d_PKCS12_bio(bio_out, p12) == 0) {
				php_openssl_store_errors();
				php_error_docref(NULL, E_WARNING, "Error writing to file %s", file_path);
			} else {
				RETVAL_TRUE;
			}
			BIO_free(bio_out);
		} else {
			php_openssl_store_errors();
			php_error_docref(NULL, E_WARNING, "Error opening file %s", file_path);
		}

		PKCS12_free(p12);
	} else {
		php_openssl_store_errors();
	}

	php_openssl_sk_X509_free(ca);

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

	cert = php_openssl_x509_from_param(cert_obj, cert_str, 1);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		return;
	}

	priv_key = php_openssl_pkey_from_zval(zpkey, 0, "", 0, 3);
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
		ca = php_openssl_array_to_X509_sk(item, 5, "extracerts");
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
	php_openssl_sk_X509_free(ca);

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

/* {{{ Exports a CSR to file */
PHP_FUNCTION(openssl_csr_export_to_file)
{
	X509_REQ *csr;
	zend_object *csr_obj;
	zend_string *csr_str;
	bool notext = 1;
	char * filename = NULL;
	size_t filename_len;
	char file_path[MAXPATHLEN];
	BIO * bio_out;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(csr_obj, php_openssl_request_ce, csr_str)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(notext)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	csr = php_openssl_csr_from_param(csr_obj, csr_str, 1);
	if (csr == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate Signing Request cannot be retrieved");
		return;
	}

	if (!php_openssl_check_path(filename, filename_len, file_path, 2)) {
		goto exit_cleanup;
	}

	bio_out = BIO_new_file(file_path, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
	if (bio_out != NULL) {
		if (!notext && !X509_REQ_print(bio_out, csr)) {
			php_openssl_store_errors();
		}
		if (!PEM_write_bio_X509_REQ(bio_out, csr)) {
			php_error_docref(NULL, E_WARNING, "Error writing PEM to file %s", file_path);
			php_openssl_store_errors();
		} else {
			RETVAL_TRUE;
		}
		BIO_free(bio_out);
	} else {
		php_openssl_store_errors();
		php_error_docref(NULL, E_WARNING, "Error opening file %s", file_path);
	}

exit_cleanup:
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
	bool notext = 1;
	BIO * bio_out;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(csr_obj, php_openssl_request_ce, csr_str)
		Z_PARAM_ZVAL(zout)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(notext)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	csr = php_openssl_csr_from_param(csr_obj, csr_str, 1);
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
	zend_string *serial_hex = NULL;
	X509 *cert = NULL, *new_cert = NULL;
	EVP_PKEY * key = NULL, *priv_key = NULL;
	int i;
	bool new_cert_used = false;
	struct php_x509_request req;

	ZEND_PARSE_PARAMETERS_START(4, 7)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(csr_obj, php_openssl_request_ce, csr_str)
		Z_PARAM_OBJ_OF_CLASS_OR_STR_OR_NULL(cert_obj, php_openssl_certificate_ce, cert_str)
		Z_PARAM_ZVAL(zpkey)
		Z_PARAM_LONG(num_days)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_OR_NULL(args)
		Z_PARAM_LONG(serial)
		Z_PARAM_STR_OR_NULL(serial_hex)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	csr = php_openssl_csr_from_param(csr_obj, csr_str, 1);
	if (csr == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate Signing Request cannot be retrieved");
		return;
	}

	PHP_SSL_REQ_INIT(&req);

	if (cert_str || cert_obj) {
		cert = php_openssl_x509_from_param(cert_obj, cert_str, 2);
		if (cert == NULL) {
			php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
			goto cleanup;
		}
	}

	priv_key = php_openssl_pkey_from_zval(zpkey, 0, "", 0, 3);
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

	if (num_days < 0 || num_days > LONG_MAX / 86400) {
		php_error_docref(NULL, E_WARNING, "Days must be between 0 and %ld", LONG_MAX / 86400);
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

	if (serial_hex != NULL) {
		char buffer[256];
		if (ZSTR_LEN(serial_hex) > 200) {
			php_error_docref(NULL, E_WARNING, "Error parsing serial number because it is too long");
			goto cleanup;
		}
		BIO *in = BIO_new_mem_buf(ZSTR_VAL(serial_hex), ZSTR_LEN(serial_hex));
		if (in == NULL) {
			php_openssl_store_errors();
			php_error_docref(NULL, E_WARNING, "Error parsing serial number because memory allocation failed");
			goto cleanup;
		}
		int success = a2i_ASN1_INTEGER(in, X509_get_serialNumber(new_cert), buffer, sizeof(buffer));
		BIO_free(in);
		if (!success) {
			php_openssl_store_errors();
			php_error_docref(NULL, E_WARNING, "Error parsing serial number");
			goto cleanup;
		}
	} else {
		PHP_OPENSSL_ASN1_INTEGER_set(X509_get_serialNumber(new_cert), serial);
	}

	X509_set_subject_name(new_cert, X509_REQ_get_subject_name(csr));

	if (cert == NULL) {
		cert = new_cert;
	}
	if (!X509_set_issuer_name(new_cert, X509_get_subject_name(cert))) {
		php_openssl_store_errors();
		goto cleanup;
	}
	X509_gmtime_adj(X509_getm_notBefore(new_cert), 0);
	X509_gmtime_adj(X509_getm_notAfter(new_cert), 60*60*24*num_days);
	i = X509_set_pubkey(new_cert, key);
	if (!i) {
		php_openssl_store_errors();
		goto cleanup;
	}
	if (req.extensions_section) {
		X509V3_CTX ctx;

		X509V3_set_ctx(&ctx, cert, new_cert, csr, NULL, 0);
		X509V3_set_nconf(&ctx, req.req_config);
		if (!X509V3_EXT_add_nconf(req.req_config, &ctx, req.extensions_section, new_cert)) {
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
	new_cert_used = true;

cleanup:

	if (!new_cert_used && new_cert) {
		X509_free(new_cert);
	}

	PHP_SSL_REQ_DISPOSE(&req);
	EVP_PKEY_free(priv_key);
	EVP_PKEY_free(key);
	if (csr_str) {
		X509_REQ_free(csr);
	}
	if (cert_str && cert && cert != new_cert) {
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
			req.priv_key = php_openssl_pkey_from_zval(out_pkey_val, 0, NULL, 0, 2);
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
				if (php_openssl_csr_make(&req, csr, dn, attribs) == SUCCESS) {
					X509V3_CTX ext_ctx;

					X509V3_set_ctx(&ext_ctx, NULL, NULL, csr, NULL, 0);
					X509V3_set_nconf(&ext_ctx, req.req_config);

					/* Add extensions */
					if (req.request_extensions_section && !X509V3_EXT_REQ_add_nconf(req.req_config,
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
							php_openssl_pkey_object_init(
								&zkey_object, req.priv_key, /* is_private */ true);
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
	bool use_shortnames = 1;
	X509_NAME *subject;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(csr_obj, php_openssl_request_ce, csr_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(use_shortnames)
	ZEND_PARSE_PARAMETERS_END();

	csr = php_openssl_csr_from_param(csr_obj, csr_str, 1);
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
	zend_object *csr_obj;
	zend_string *csr_str;
	bool use_shortnames = 1;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(csr_obj, php_openssl_request_ce, csr_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(use_shortnames)
	ZEND_PARSE_PARAMETERS_END();

	X509_REQ *csr = php_openssl_csr_from_param(csr_obj, csr_str, 1);
	if (csr == NULL) {
		RETURN_FALSE;
	}

	/* Retrieve the public key from the CSR */
	EVP_PKEY *orig_key = X509_REQ_get_pubkey(csr);
	EVP_PKEY *tpubkey = php_openssl_extract_public_key(orig_key);
	EVP_PKEY_free(orig_key);

	if (csr_str) {
		/* We need to free the original CSR if it was freshly created */
		X509_REQ_free(csr);
	}

	if (tpubkey == NULL) {
		php_openssl_store_errors();
		RETURN_FALSE;
	}

	php_openssl_pkey_object_init(return_value, tpubkey, /* is_private */ false);
}
/* }}} */

/* }}} */

/* {{{ EVP Public/Private key functions */

/* {{{ Generates a new private key */
PHP_FUNCTION(openssl_pkey_new)
{
	struct php_x509_request req;
	zval * args = NULL;
	zval *data;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a!", &args) == FAILURE) {
		RETURN_THROWS();
	}
	RETVAL_FALSE;

	if (args && Z_TYPE_P(args) == IS_ARRAY) {
		EVP_PKEY *pkey;

		if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "rsa", sizeof("rsa")-1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			pkey = php_openssl_pkey_init_rsa(data);
			if (!pkey) {
				RETURN_FALSE;
			}
			php_openssl_pkey_object_init(return_value, pkey, /* is_private */ true);
			return;
		} else if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "dsa", sizeof("dsa") - 1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			bool is_private;
			pkey = php_openssl_pkey_init_dsa(data, &is_private);
			if (!pkey) {
				RETURN_FALSE;
			}
			php_openssl_pkey_object_init(return_value, pkey, is_private);
			return;
		} else if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "dh", sizeof("dh") - 1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			bool is_private;
			pkey = php_openssl_pkey_init_dh(data, &is_private);
			if (!pkey) {
				RETURN_FALSE;
			}
			php_openssl_pkey_object_init(return_value, pkey, is_private);
			return;
#ifdef HAVE_EVP_PKEY_EC
		} else if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "ec", sizeof("ec") - 1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			bool is_private;
			pkey = php_openssl_pkey_init_ec(data, &is_private);
			if (!pkey) {
				RETURN_FALSE;
			}
			php_openssl_pkey_object_init(return_value, pkey, is_private);
			return;
#endif
#if PHP_OPENSSL_API_VERSION >= 0x30000
		} else if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "x25519", sizeof("x25519") - 1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			php_openssl_pkey_object_curve_25519_448(return_value, EVP_PKEY_X25519, data);
			return;
		} else if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "ed25519", sizeof("ed25519") - 1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			php_openssl_pkey_object_curve_25519_448(return_value, EVP_PKEY_ED25519, data);
			return;
		} else if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "x448", sizeof("x448") - 1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			php_openssl_pkey_object_curve_25519_448(return_value, EVP_PKEY_X448, data);
			return;
		} else if ((data = zend_hash_str_find(Z_ARRVAL_P(args), "ed448", sizeof("ed448") - 1)) != NULL &&
			Z_TYPE_P(data) == IS_ARRAY) {
			php_openssl_pkey_object_curve_25519_448(return_value, EVP_PKEY_ED448, data);
			return;
#endif
		}
	}

	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS) {
		if (php_openssl_generate_private_key(&req)) {
			/* pass back a key resource */
			php_openssl_pkey_object_init(return_value, req.priv_key, /* is_private */ true);
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
	char * filename = NULL, file_path[MAXPATHLEN];
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

	key = php_openssl_pkey_from_zval(zpkey, 0, passphrase, passphrase_len, 1);
	if (key == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Cannot get key from parameter 1");
		}
		RETURN_FALSE;
	}

	if (!php_openssl_check_path(filename, filename_len, file_path, 2)) {
		goto clean_exit_key;
	}

	PHP_SSL_REQ_INIT(&req);

	if (PHP_SSL_REQ_PARSE(&req, args) == SUCCESS) {
		bio_out = BIO_new_file(file_path, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
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

		pem_write = PEM_write_bio_PrivateKey(
				bio_out, key, cipher,
				(unsigned char *)passphrase, (int)passphrase_len, NULL, NULL);
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
	BIO_free(bio_out);
clean_exit_key:
	EVP_PKEY_free(key);
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

	key = php_openssl_pkey_from_zval(zpkey, 0, passphrase, passphrase_len, 1);
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

		pem_write = PEM_write_bio_PrivateKey(
				bio_out, key, cipher,
				(unsigned char *)passphrase, (int)passphrase_len, NULL, NULL);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &cert) == FAILURE) {
		RETURN_THROWS();
	}
	pkey = php_openssl_pkey_from_zval(cert, 1, NULL, 0, 1);
	if (pkey == NULL) {
		RETURN_FALSE;
	}

	php_openssl_pkey_object_init(return_value, pkey, /* is_private */ false);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|s!", &cert, &passphrase, &passphrase_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (passphrase) {
		PHP_OPENSSL_CHECK_SIZE_T_TO_INT(passphrase_len, passphrase, 2);
	}

	pkey = php_openssl_pkey_from_zval(cert, 0, passphrase, passphrase_len, 1);
	if (pkey == NULL) {
		RETURN_FALSE;
	}

	php_openssl_pkey_object_init(return_value, pkey, /* is_private */ true);
}

/* }}} */

/* {{{ returns an array with the key details (bits, pkey, type)*/
PHP_FUNCTION(openssl_pkey_get_details)
{
	zval *key;
	unsigned int pbio_len;
	char *pbio;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &key, php_openssl_pkey_ce) == FAILURE) {
		RETURN_THROWS();
	}

	EVP_PKEY *pkey = Z_OPENSSL_PKEY_P(key)->pkey;

	BIO *out = BIO_new(BIO_s_mem());
	if (!PEM_write_bio_PUBKEY(out, pkey)) {
		BIO_free(out);
		php_openssl_store_errors();
		RETURN_FALSE;
	}
	pbio_len = BIO_get_mem_data(out, &pbio);

	array_init(return_value);
	add_assoc_long(return_value, "bits", EVP_PKEY_bits(pkey));
	add_assoc_stringl(return_value, "key", pbio, pbio_len);
	
	zend_long ktype = php_openssl_pkey_get_details(return_value, pkey);

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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sO", &pub_str, &pub_len, &key, php_openssl_pkey_ce) == FAILURE) {
		RETURN_THROWS();
	}

	PHP_OPENSSL_CHECK_SIZE_T_TO_INT(pub_len, pub_key, 1);

	EVP_PKEY *pkey = Z_OPENSSL_PKEY_P(key)->pkey;
	if (EVP_PKEY_base_id(pkey) != EVP_PKEY_DH) {
		RETURN_FALSE;
	}

	zend_string *result = php_openssl_dh_compute_key(pkey, pub_str, pub_len);
	if (result) {
		RETURN_NEW_STR(result);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Computes shared secret for public value of remote and local DH or ECDH key */
PHP_FUNCTION(openssl_pkey_derive)
{
	zval *priv_key;
	zval *peer_pub_key;
	zend_long key_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz|l", &peer_pub_key, &priv_key, &key_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (key_len < 0) {
		zend_argument_value_error(3, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	EVP_PKEY *pkey = php_openssl_pkey_from_zval(priv_key, 0, "", 0, 2);
	if (!pkey) {
		RETURN_FALSE;
	}

	EVP_PKEY *peer_key = php_openssl_pkey_from_zval(peer_pub_key, 1, NULL, 0, 1);
	if (!peer_key) {
		EVP_PKEY_free(pkey);
		RETURN_FALSE;
	}

	zend_string *result = php_openssl_pkey_derive(pkey, peer_key, key_len);
	EVP_PKEY_free(pkey);
	EVP_PKEY_free(peer_key);

	if (result) {
		RETURN_NEW_STR(result);
	} else {
		RETURN_FALSE;
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
		others = php_openssl_load_all_certs_from_file(extracerts, extracerts_len, 5);
		if (others == NULL) {
			goto clean_exit;
		}
	}

	flags = flags & ~PKCS7_DETACHED;

	store = php_openssl_setup_verify(cainfo, 4);

	if (!store) {
		goto clean_exit;
	}

	in = php_openssl_bio_new_file(filename, filename_len, 1, PHP_OPENSSL_BIO_MODE_R(flags));
	if (in == NULL) {
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
		dataout = php_openssl_bio_new_file(
				datafilename, datafilename_len, 6, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
		if (dataout == NULL) {
			goto clean_exit;
		}
	}
	if (p7bfilename) {
		p7bout = php_openssl_bio_new_file(
				p7bfilename, p7bfilename_len, 7, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
		if (p7bout == NULL) {
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

			certout = php_openssl_bio_new_file(
					signersfilename, signersfilename_len, 3, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
			if (certout) {
				int i;
				signers = PKCS7_get0_signers(p7, others, (int)flags);
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
				php_error_docref(NULL, E_WARNING, "Signature OK, but cannot open %s for writing", signersfilename);
				RETVAL_LONG(-1);
			}

			if (p7bout) {
				if (PEM_write_bio_PKCS7(p7bout, p7) == 0) {
					php_error_docref(NULL, E_WARNING, "Failed to write PKCS7 to file");
					php_openssl_store_errors();
					RETVAL_FALSE;
				}
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

	infile = php_openssl_bio_new_file(infilename, infilename_len, 1, PHP_OPENSSL_BIO_MODE_R(flags));
	if (infile == NULL) {
		goto clean_exit;
	}

	outfile = php_openssl_bio_new_file(outfilename, outfilename_len, 2, PHP_OPENSSL_BIO_MODE_W(flags));
	if (outfile == NULL) {
		goto clean_exit;
	}

	recipcerts = sk_X509_new_null();

	/* get certs */
	if (Z_TYPE_P(zrecipcerts) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zrecipcerts), zcertval) {
			bool free_cert;

			cert = php_openssl_x509_from_zval(zcertval, &free_cert, 3, true, NULL);
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

		cert = php_openssl_x509_from_zval(zrecipcerts, &free_cert, 3, false, NULL);
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
			}
			BIO_free(bio_out);
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
			}
			BIO_free(bio_out);
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
		others = php_openssl_load_all_certs_from_file(
				extracertsfilename, extracertsfilename_len, 7);
		if (others == NULL) {
			goto clean_exit;
		}
	}

	privkey = php_openssl_pkey_from_zval(zprivkey, 0, "", 0, 4);
	if (privkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Error getting private key");
		}
		goto clean_exit;
	}

	cert = php_openssl_x509_from_param(cert_obj, cert_str, 3);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		goto clean_exit;
	}

	infile = php_openssl_bio_new_file(infilename, infilename_len, 1, PHP_OPENSSL_BIO_MODE_R(flags));
	if (infile == NULL) {
		php_error_docref(NULL, E_WARNING, "Error opening input file %s!", infilename);
		goto clean_exit;
	}

	outfile = php_openssl_bio_new_file(outfilename, outfilename_len, 2, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
	if (outfile == NULL) {
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

	cert = php_openssl_x509_from_zval(recipcert, &free_recipcert, 3, false, NULL);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		goto clean_exit;
	}

	key = php_openssl_pkey_from_zval(recipkey ? recipkey : recipcert, 0, "", 0, 4);
	if (key == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Unable to get private key");
		}
		goto clean_exit;
	}

	in = php_openssl_bio_new_file(
			infilename, infilename_len, 1, PHP_OPENSSL_BIO_MODE_R(PKCS7_BINARY));
	if (in == NULL) {
		goto clean_exit;
	}

	out = php_openssl_bio_new_file(
			outfilename, outfilename_len, 2, PHP_OPENSSL_BIO_MODE_W(PKCS7_BINARY));
	if (out == NULL) {
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

	in = php_openssl_bio_new_file(filename, filename_len, 1, PHP_OPENSSL_BIO_MODE_R(flags));
	if (in == NULL) {
		goto clean_exit;
	}
	if (sigfile && (flags & CMS_DETACHED)) {
		if (encoding == ENCODING_SMIME)  {
			php_error_docref(NULL, E_WARNING,
					 "Detached signatures not possible with S/MIME encoding");
			goto clean_exit;
		}
		sigbio = php_openssl_bio_new_file(sigfile, sigfile_len, 1, PHP_OPENSSL_BIO_MODE_R(flags));
		if (sigbio == NULL) {
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
		others = php_openssl_load_all_certs_from_file(extracerts, extracerts_len, 5);
		if (others == NULL) {
			goto clean_exit;
		}
	}

	store = php_openssl_setup_verify(cainfo, 4);

	if (!store) {
		goto clean_exit;
	}

	if (datafilename) {
		dataout = php_openssl_bio_new_file(
				datafilename, datafilename_len, 6, PHP_OPENSSL_BIO_MODE_W(CMS_BINARY));
		if (dataout == NULL) {
			goto clean_exit;
		}
	}

	if (p7bfilename) {
		p7bout = php_openssl_bio_new_file(
				p7bfilename, p7bfilename_len, 7, PHP_OPENSSL_BIO_MODE_W(CMS_BINARY));
		if (p7bout == NULL) {
			goto clean_exit;
		}
	}
#if DEBUG_SMIME
	zend_printf("Calling CMS verify\n");
#endif
	if (CMS_verify(cms, others, store, datain, dataout, (unsigned int)flags)) {
		RETVAL_TRUE;

		if (signersfilename) {
			certout = php_openssl_bio_new_file(
					signersfilename, signersfilename_len, 3, PHP_OPENSSL_BIO_MODE_W(CMS_BINARY));
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
				php_error_docref(NULL, E_WARNING, "Signature OK, but cannot open %s for writing", signersfilename);
				RETVAL_FALSE;
			}

			if (p7bout) {
				if (PEM_write_bio_CMS(p7bout, cms) == 0) {
					php_error_docref(NULL, E_WARNING, "Failed to write CMS to file");
					php_openssl_store_errors();
					RETVAL_FALSE;
				}
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


	infile = php_openssl_bio_new_file(
			infilename, infilename_len, 1, PHP_OPENSSL_BIO_MODE_R(flags));
	if (infile == NULL) {
		goto clean_exit;
	}

	outfile = php_openssl_bio_new_file(
			outfilename, outfilename_len, 2, PHP_OPENSSL_BIO_MODE_W(flags));
	if (outfile == NULL) {
		goto clean_exit;
	}

	recipcerts = sk_X509_new_null();

	/* get certs */
	if (Z_TYPE_P(zrecipcerts) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zrecipcerts), zcertval) {
			bool free_cert;

			cert = php_openssl_x509_from_zval(zcertval, &free_cert, 3, true, NULL);
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

		cert = php_openssl_x509_from_zval(zrecipcerts, &free_cert, 3, false, NULL);
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
			}
			BIO_free(bio_out);
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
			}
			BIO_free(bio_out);
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
		Z_PARAM_PATH_OR_NULL(extracertsfilename, extracertsfilename_len)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_FALSE;

	if (extracertsfilename) {
		others = php_openssl_load_all_certs_from_file(
				extracertsfilename, extracertsfilename_len, 8);
		if (others == NULL) {
			goto clean_exit;
		}
	}

	privkey = php_openssl_pkey_from_zval(zprivkey, 0, "", 0, 4);
	if (privkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Error getting private key");
		}
		goto clean_exit;
	}

	cert = php_openssl_x509_from_param(cert_obj, cert_str, 3);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
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

	infile = php_openssl_bio_new_file(
			infilename, infilename_len, 1, PHP_OPENSSL_BIO_MODE_R(flags));
	if (infile == NULL) {
		php_error_docref(NULL, E_WARNING, "Error opening input file %s!", infilename);
		goto clean_exit;
	}

	outfile = php_openssl_bio_new_file(
			outfilename, outfilename_len, 2, PHP_OPENSSL_BIO_MODE_W(CMS_BINARY));
	if (outfile == NULL) {
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

	cert = php_openssl_x509_from_zval(recipcert, &free_recipcert, 3, false, NULL);
	if (cert == NULL) {
		php_error_docref(NULL, E_WARNING, "X.509 Certificate cannot be retrieved");
		goto clean_exit;
	}

	key = php_openssl_pkey_from_zval(recipkey ? recipkey : recipcert, 0, "", 0, recipkey ? 4 : 3);
	if (key == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Unable to get private key");
		}
		goto clean_exit;
	}

	in = php_openssl_bio_new_file(
			infilename, infilename_len, 1, PHP_OPENSSL_BIO_MODE_R(CMS_BINARY));
	if (in == NULL) {
		goto clean_exit;
	}

	out = php_openssl_bio_new_file(
			outfilename, outfilename_len, 2, PHP_OPENSSL_BIO_MODE_W(CMS_BINARY));
	if (out == NULL) {
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
	char * data;
	size_t data_len;
	zend_long padding = RSA_PKCS1_PADDING;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE) {
		RETURN_THROWS();
	}

	EVP_PKEY *pkey = php_openssl_pkey_from_zval(key, 0, "", 0, 3);
	if (pkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "key param is not a valid private key");
		}
		RETURN_FALSE;
	}

	size_t out_len = 0;
	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
	if (!ctx || EVP_PKEY_sign_init(ctx) <= 0 ||
			EVP_PKEY_CTX_set_rsa_padding(ctx, padding) <= 0 ||
			EVP_PKEY_sign(ctx, NULL, &out_len, (unsigned char *) data, data_len) <= 0) {
		php_openssl_store_errors();
		RETVAL_FALSE;
		goto cleanup;
	}

	zend_string *out = zend_string_alloc(out_len, 0);
	if (EVP_PKEY_sign(ctx, (unsigned char *) ZSTR_VAL(out), &out_len,
			(unsigned char *) data, data_len) <= 0) {
		zend_string_release(out);
		php_openssl_store_errors();
		RETVAL_FALSE;
		goto cleanup;
	}

	ZSTR_VAL(out)[out_len] = '\0';
	ZEND_TRY_ASSIGN_REF_NEW_STR(crypted, out);
	RETVAL_TRUE;

cleanup:
	EVP_PKEY_CTX_free(ctx);
	EVP_PKEY_free(pkey);
}
/* }}} */

/* {{{ Decrypts data with private key */
PHP_FUNCTION(openssl_private_decrypt)
{
	zval *key, *crypted;
	zend_long padding = RSA_PKCS1_PADDING;
	char * data;
	size_t data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE) {
		RETURN_THROWS();
	}

	EVP_PKEY *pkey = php_openssl_pkey_from_zval(key, 0, "", 0, 3);
	if (pkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "key parameter is not a valid private key");
		}
		RETURN_FALSE;
	}

	size_t out_len = 0;
	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
	if (!ctx || EVP_PKEY_decrypt_init(ctx) <= 0 ||
			EVP_PKEY_CTX_set_rsa_padding(ctx, padding) <= 0 ||
			EVP_PKEY_decrypt(ctx, NULL, &out_len, (unsigned char *) data, data_len) <= 0) {
		php_openssl_store_errors();
		RETVAL_FALSE;
		goto cleanup;
	}

	zend_string *out = zend_string_alloc(out_len, 0);
	if (EVP_PKEY_decrypt(ctx, (unsigned char *) ZSTR_VAL(out), &out_len,
			(unsigned char *) data, data_len) <= 0) {
		zend_string_release(out);
		php_openssl_store_errors();
		RETVAL_FALSE;
		goto cleanup;
	}

	out = zend_string_truncate(out, out_len, 0);
	ZSTR_VAL(out)[out_len] = '\0';
	ZEND_TRY_ASSIGN_REF_NEW_STR(crypted, out);
	RETVAL_TRUE;

cleanup:
	EVP_PKEY_CTX_free(ctx);
	EVP_PKEY_free(pkey);
}
/* }}} */

/* {{{ Encrypts data with public key */
PHP_FUNCTION(openssl_public_encrypt)
{
	zval *key, *crypted;
	zend_long padding = RSA_PKCS1_PADDING;
	char * data;
	size_t data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE) {
		RETURN_THROWS();
	}

	EVP_PKEY *pkey = php_openssl_pkey_from_zval(key, 1, NULL, 0, 3);
	if (pkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "key parameter is not a valid public key");
		}
		RETURN_FALSE;
	}

	size_t out_len = 0;
	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
	if (!ctx || EVP_PKEY_encrypt_init(ctx) <= 0 ||
			EVP_PKEY_CTX_set_rsa_padding(ctx, padding) <= 0 ||
			EVP_PKEY_encrypt(ctx, NULL, &out_len, (unsigned char *) data, data_len) <= 0) {
		php_openssl_store_errors();
		RETVAL_FALSE;
		goto cleanup;
	}

	zend_string *out = zend_string_alloc(out_len, 0);
	if (EVP_PKEY_encrypt(ctx, (unsigned char *) ZSTR_VAL(out), &out_len,
			(unsigned char *) data, data_len) <= 0) {
		zend_string_release(out);
		php_openssl_store_errors();
		RETVAL_FALSE;
		goto cleanup;
	}

	ZSTR_VAL(out)[out_len] = '\0';
	ZEND_TRY_ASSIGN_REF_NEW_STR(crypted, out);
	RETVAL_TRUE;

cleanup:
	EVP_PKEY_CTX_free(ctx);
	EVP_PKEY_free(pkey);
}
/* }}} */

/* {{{ Decrypts data with public key */
PHP_FUNCTION(openssl_public_decrypt)
{
	zval *key, *crypted;
	zend_long padding = RSA_PKCS1_PADDING;
	char * data;
	size_t data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "szz|l", &data, &data_len, &crypted, &key, &padding) == FAILURE) {
		RETURN_THROWS();
	}

	EVP_PKEY *pkey = php_openssl_pkey_from_zval(key, 1, NULL, 0, 3);
	if (pkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "key parameter is not a valid public key");
		}
		RETURN_FALSE;
	}

	size_t out_len = 0;
	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
	if (!ctx || EVP_PKEY_verify_recover_init(ctx) <= 0 ||
			EVP_PKEY_CTX_set_rsa_padding(ctx, padding) <= 0 ||
			EVP_PKEY_verify_recover(ctx, NULL, &out_len, (unsigned char *) data, data_len) <= 0) {
		php_openssl_store_errors();
		RETVAL_FALSE;
		goto cleanup;
	}

	zend_string *out = zend_string_alloc(out_len, 0);
	if (EVP_PKEY_verify_recover(ctx, (unsigned char *) ZSTR_VAL(out), &out_len,
			(unsigned char *) data, data_len) <= 0) {
		zend_string_release(out);
		php_openssl_store_errors();
		RETVAL_FALSE;
		goto cleanup;
	}

	out = zend_string_truncate(out, out_len, 0);
	ZSTR_VAL(out)[out_len] = '\0';
	ZEND_TRY_ASSIGN_REF_NEW_STR(crypted, out);
	RETVAL_TRUE;

cleanup:
	EVP_PKEY_CTX_free(ctx);
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
	zend_string *sigbuf = NULL;
	char * data;
	size_t data_len;
	EVP_MD_CTX *md_ctx;
	zend_string *method_str = NULL;
	zend_long method_long = OPENSSL_ALGO_SHA1;
	const EVP_MD *mdtype;
	bool can_default_digest = ZEND_THREEWAY_COMPARE(PHP_OPENSSL_API_VERSION, 0x30000) >= 0;

	ZEND_PARSE_PARAMETERS_START(3, 4)
		Z_PARAM_STRING(data, data_len)
		Z_PARAM_ZVAL(signature)
		Z_PARAM_ZVAL(key)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_LONG(method_str, method_long)
	ZEND_PARSE_PARAMETERS_END();

	pkey = php_openssl_pkey_from_zval(key, 0, "", 0, 3);
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
	if (!mdtype && (!can_default_digest || method_long != 0)) {
		EVP_PKEY_free(pkey);
		php_error_docref(NULL, E_WARNING, "Unknown digest algorithm");
		RETURN_FALSE;
	}

	md_ctx = EVP_MD_CTX_create();
	size_t siglen;
	if (md_ctx != NULL &&
			EVP_DigestSignInit(md_ctx, NULL, mdtype, NULL, pkey) &&
			EVP_DigestSign(md_ctx, NULL, &siglen, (unsigned char*)data, data_len) &&
			(sigbuf = zend_string_alloc(siglen, 0)) != NULL &&
			EVP_DigestSign(md_ctx, (unsigned char*)ZSTR_VAL(sigbuf), &siglen, (unsigned char*)data, data_len)) {
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
	bool can_default_digest = ZEND_THREEWAY_COMPARE(PHP_OPENSSL_API_VERSION, 0x30000) >= 0;

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
	if (!mdtype && (!can_default_digest || method_long != 0)) {
		php_error_docref(NULL, E_WARNING, "Unknown digest algorithm");
		RETURN_FALSE;
	}

	pkey = php_openssl_pkey_from_zval(key, 1, NULL, 0, 3);
	if (pkey == NULL) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Supplied key param cannot be coerced into a public key");
		}
		RETURN_FALSE;
	}

	md_ctx = EVP_MD_CTX_create();
	if (md_ctx == NULL ||
			!EVP_DigestVerifyInit(md_ctx, NULL, mdtype, NULL, pkey) ||
			(err = EVP_DigestVerify(md_ctx, (unsigned char *)signature, signature_len, (unsigned char*)data, data_len)) < 0) {
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
		zend_argument_must_not_be_empty_error(4);
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
		pkeys[i] = php_openssl_pkey_from_zval(pubkey, 1, NULL, 0, 4);
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
	pkey = php_openssl_pkey_from_zval(privkey, 0, "", 0, 4);

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

/* {{{ Return array of available digest algorithms */
PHP_FUNCTION(openssl_get_md_methods)
{
	bool aliases = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &aliases) == FAILURE) {
		RETURN_THROWS();
	}
	php_openssl_get_md_methods(return_value, aliases);
}
/* }}} */

/* {{{ Return array of available cipher algorithms */
PHP_FUNCTION(openssl_get_cipher_methods)
{
	bool aliases = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &aliases) == FAILURE) {
		RETURN_THROWS();
	}
	php_openssl_get_cipher_methods(return_value, aliases);
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
	bool raw_output = 0;
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


/* {{{ Takes raw or base64 encoded string and decrypts it using given method and key */
PHP_FUNCTION(openssl_decrypt)
{
	zend_long options = 0;
	char *data, *method, *password, *iv = "", *tag = NULL, *aad = "";
	size_t data_len, method_len, password_len, iv_len = 0, tag_len = 0, aad_len = 0;
	zend_string *ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss|lss!s", &data, &data_len, &method, &method_len,
					&password, &password_len, &options, &iv, &iv_len, &tag, &tag_len, &aad, &aad_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (!method_len) {
		zend_argument_must_not_be_empty_error(2);
		RETURN_THROWS();
	}

	if ((ret = php_openssl_decrypt(data, data_len, method, method_len, password, password_len, options, iv, iv_len, tag, tag_len, aad, aad_len))) {
		RETVAL_STR(ret);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

PHP_FUNCTION(openssl_cipher_iv_length)
{
	zend_string *method;
	zend_long ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &method) == FAILURE) {
		RETURN_THROWS();
	}

	if (ZSTR_LEN(method) == 0) {
		zend_argument_must_not_be_empty_error(1);
		RETURN_THROWS();
	}

	/* Warning is emitted in php_openssl_cipher_iv_length */
	if ((ret = php_openssl_cipher_iv_length(ZSTR_VAL(method))) == -1) {
		RETURN_FALSE;
	}

	RETURN_LONG(ret);
}

PHP_FUNCTION(openssl_cipher_key_length)
{
	zend_string *method;
	zend_long ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &method) == FAILURE) {
		RETURN_THROWS();
	}

	if (ZSTR_LEN(method) == 0) {
		zend_argument_must_not_be_empty_error(1);
		RETURN_THROWS();
	}

	/* Warning is emitted in php_openssl_cipher_key_length */
	if ((ret = php_openssl_cipher_key_length(ZSTR_VAL(method))) == -1) {
		RETURN_FALSE;
	}

	RETURN_LONG(ret);
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

	if ((buffer = php_openssl_random_pseudo_bytes(buffer_length))) {
		ZSTR_VAL(buffer)[buffer_length] = 0;
		RETVAL_NEW_STR(buffer);

		if (zstrong_result_returned) {
			ZEND_TRY_ASSIGN_REF_TRUE(zstrong_result_returned);
		}
	} else if (zstrong_result_returned) {
		ZEND_TRY_ASSIGN_REF_FALSE(zstrong_result_returned);
	}
}
/* }}} */
