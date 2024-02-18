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
   | Author: Sterling Hughes <sterling@php.net>                           |
   +----------------------------------------------------------------------+
*/

#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "Zend/zend_exceptions.h"

#include <stdio.h>
#include <string.h>

#ifdef PHP_WIN32
#include <winsock2.h>
#include <sys/types.h>
#endif

#include <curl/curl.h>
#include <curl/easy.h>

/* As of curl 7.11.1 this is no longer defined inside curl.h */
#ifndef HttpPost
#define HttpPost curl_httppost
#endif

/* {{{ cruft for thread safe SSL crypto locks */
#if defined(ZTS) && defined(HAVE_CURL_OLD_OPENSSL)
# if defined(HAVE_OPENSSL_CRYPTO_H)
#  define PHP_CURL_NEED_OPENSSL_TSL
#  include <openssl/crypto.h>
# else
#  warning \
	"libcurl was compiled with OpenSSL support, but configure could not find " \
	"openssl/crypto.h; thus no SSL crypto locking callbacks will be set, which may " \
	"cause random crashes on SSL requests"
# endif
#endif /* ZTS && HAVE_CURL_OLD_OPENSSL */
/* }}} */

#define SMART_STR_PREALLOC 4096

#include "zend_smart_str.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "ext/standard/url.h"
#include "curl_private.h"

#ifdef __GNUC__
/* don't complain about deprecated CURLOPT_* we're exposing to PHP; we
   need to keep using those to avoid breaking PHP API compatibiltiy */
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "curl_arginfo.h"

#ifdef PHP_CURL_NEED_OPENSSL_TSL /* {{{ */
static MUTEX_T *php_curl_openssl_tsl = NULL;

/* Locking callbacks are no longer used since OpenSSL 1.1. Mark the functions as unused to
 * avoid warnings due to this. */
static ZEND_ATTRIBUTE_UNUSED void php_curl_ssl_lock(int mode, int n, const char * file, int line)
{
	if (mode & CRYPTO_LOCK) {
		tsrm_mutex_lock(php_curl_openssl_tsl[n]);
	} else {
		tsrm_mutex_unlock(php_curl_openssl_tsl[n]);
	}
}

static ZEND_ATTRIBUTE_UNUSED unsigned long php_curl_ssl_id(void)
{
	return (unsigned long) tsrm_thread_id();
}
#endif
/* }}} */

#define CAAL(s, v) add_assoc_long_ex(return_value, s, sizeof(s) - 1, (zend_long) v);
#define CAAD(s, v) add_assoc_double_ex(return_value, s, sizeof(s) - 1, (double) v);
#define CAAS(s, v) add_assoc_string_ex(return_value, s, sizeof(s) - 1, (char *) (v ? v : ""));
#define CAASTR(s, v) add_assoc_str_ex(return_value, s, sizeof(s) - 1, \
		v ? zend_string_copy(v) : ZSTR_EMPTY_ALLOC());
#define CAAZ(s, v) add_assoc_zval_ex(return_value, s, sizeof(s) -1 , (zval *) v);

#if defined(PHP_WIN32) || defined(__GNUC__)
# define php_curl_ret(__ret) RETVAL_FALSE; return __ret;
#else
# define php_curl_ret(__ret) RETVAL_FALSE; return;
#endif

static zend_result php_curl_option_str(php_curl *ch, zend_long option, const char *str, const size_t len)
{
	if (zend_char_has_nul_byte(str, len)) {
		zend_value_error("%s(): cURL option must not contain any null bytes", get_active_function_name());
		return FAILURE;
	}

	CURLcode error = curl_easy_setopt(ch->cp, option, str);
	SAVE_CURL_ERROR(ch, error);

	return error == CURLE_OK ? SUCCESS : FAILURE;
}

static zend_result php_curl_option_url(php_curl *ch, const zend_string *url) /* {{{ */
{
	/* Disable file:// if open_basedir are used */
	if (PG(open_basedir) && *PG(open_basedir)) {
		curl_easy_setopt(ch->cp, CURLOPT_PROTOCOLS, CURLPROTO_ALL & ~CURLPROTO_FILE);
	}

#ifdef PHP_WIN32
	if (
		zend_string_starts_with_literal_ci(url, "file://")
		&& '/' != ZSTR_VAL(url)[sizeof("file://") - 1]
		&& ZSTR_LEN(url) < MAXPATHLEN - 2
	) {
		char _tmp[MAXPATHLEN] = {0};

		memmove(_tmp, "file:///", sizeof("file:///") - 1);
		memmove(_tmp + sizeof("file:///") - 1, ZSTR_VAL(url) + sizeof("file://") - 1, ZSTR_LEN(url) - sizeof("file://") + 1);

		return php_curl_option_str(ch, CURLOPT_URL, _tmp, ZSTR_LEN(url) + 1);
	}
#endif

	return php_curl_option_str(ch, CURLOPT_URL, ZSTR_VAL(url), ZSTR_LEN(url));
}
/* }}} */

void _php_curl_verify_handlers(php_curl *ch, bool reporterror) /* {{{ */
{
	php_stream *stream;

	ZEND_ASSERT(ch);

	if (!Z_ISUNDEF(ch->handlers.std_err)) {
		stream = (php_stream *)zend_fetch_resource2_ex(&ch->handlers.std_err, NULL, php_file_le_stream(), php_file_le_pstream());
		if (stream == NULL) {
			if (reporterror) {
				php_error_docref(NULL, E_WARNING, "CURLOPT_STDERR resource has gone away, resetting to stderr");
			}
			zval_ptr_dtor(&ch->handlers.std_err);
			ZVAL_UNDEF(&ch->handlers.std_err);

			curl_easy_setopt(ch->cp, CURLOPT_STDERR, stderr);
		}
	}
	if (ch->handlers.read && !Z_ISUNDEF(ch->handlers.read->stream)) {
		stream = (php_stream *)zend_fetch_resource2_ex(&ch->handlers.read->stream, NULL, php_file_le_stream(), php_file_le_pstream());
		if (stream == NULL) {
			if (reporterror) {
				php_error_docref(NULL, E_WARNING, "CURLOPT_INFILE resource has gone away, resetting to default");
			}
			zval_ptr_dtor(&ch->handlers.read->stream);
			ZVAL_UNDEF(&ch->handlers.read->stream);
			ch->handlers.read->res = NULL;
			ch->handlers.read->fp = 0;

			curl_easy_setopt(ch->cp, CURLOPT_INFILE, (void *) ch);
		}
	}
	if (ch->handlers.write_header && !Z_ISUNDEF(ch->handlers.write_header->stream)) {
		stream = (php_stream *)zend_fetch_resource2_ex(&ch->handlers.write_header->stream, NULL, php_file_le_stream(), php_file_le_pstream());
		if (stream == NULL) {
			if (reporterror) {
				php_error_docref(NULL, E_WARNING, "CURLOPT_WRITEHEADER resource has gone away, resetting to default");
			}
			zval_ptr_dtor(&ch->handlers.write_header->stream);
			ZVAL_UNDEF(&ch->handlers.write_header->stream);
			ch->handlers.write_header->fp = 0;

			ch->handlers.write_header->method = PHP_CURL_IGNORE;
			curl_easy_setopt(ch->cp, CURLOPT_WRITEHEADER, (void *) ch);
		}
	}
	if (ch->handlers.write && !Z_ISUNDEF(ch->handlers.write->stream)) {
		stream = (php_stream *)zend_fetch_resource2_ex(&ch->handlers.write->stream, NULL, php_file_le_stream(), php_file_le_pstream());
		if (stream == NULL) {
			if (reporterror) {
				php_error_docref(NULL, E_WARNING, "CURLOPT_FILE resource has gone away, resetting to default");
			}
			zval_ptr_dtor(&ch->handlers.write->stream);
			ZVAL_UNDEF(&ch->handlers.write->stream);
			ch->handlers.write->fp = 0;

			ch->handlers.write->method = PHP_CURL_STDOUT;
			curl_easy_setopt(ch->cp, CURLOPT_FILE, (void *) ch);
		}
	}
	return;
}
/* }}} */

/* {{{ curl_module_entry */
zend_module_entry curl_module_entry = {
	STANDARD_MODULE_HEADER,
	"curl",
	ext_functions,
	PHP_MINIT(curl),
	PHP_MSHUTDOWN(curl),
	NULL,
	NULL,
	PHP_MINFO(curl),
	PHP_CURL_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CURL
ZEND_GET_MODULE (curl)
#endif

/* CurlHandle class */

zend_class_entry *curl_ce;
zend_class_entry *curl_exception_ce;
zend_class_entry *curl_handle_exception_ce;
zend_class_entry *curl_share_ce;
zend_class_entry *curl_share_exception_ce;
static zend_object_handlers curl_object_handlers;

static zend_object *curl_create_object(zend_class_entry *class_type);
static void curl_free_obj(zend_object *object);
static HashTable *curl_get_gc(zend_object *object, zval **table, int *n);
static zend_object *curl_clone_obj(zend_object *object);
static inline zend_result build_mime_structure_from_hash(php_curl *ch, zval *zpostfields);

/* {{{ PHP_INI_BEGIN */
PHP_INI_BEGIN()
	PHP_INI_ENTRY("curl.cainfo", "", PHP_INI_SYSTEM, NULL)
PHP_INI_END()
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(curl)
{
	curl_version_info_data *d;
	char **p;
	char str[1024];
	size_t n = 0;

	d = curl_version_info(CURLVERSION_NOW);
	php_info_print_table_start();
	php_info_print_table_row(2, "cURL support",    "enabled");
	php_info_print_table_row(2, "cURL Information", d->version);
	sprintf(str, "%d", d->age);
	php_info_print_table_row(2, "Age", str);

	/* To update on each new cURL release using src/main.c in cURL sources */
	if (d->features) {
		struct feat {
			const char *name;
			int bitmask;
		};

		unsigned int i;

		static const struct feat feats[] = {
			{"AsynchDNS", CURL_VERSION_ASYNCHDNS},
			{"CharConv", CURL_VERSION_CONV},
			{"Debug", CURL_VERSION_DEBUG},
			{"GSS-Negotiate", CURL_VERSION_GSSNEGOTIATE},
			{"IDN", CURL_VERSION_IDN},
			{"IPv6", CURL_VERSION_IPV6},
			{"krb4", CURL_VERSION_KERBEROS4},
			{"Largefile", CURL_VERSION_LARGEFILE},
			{"libz", CURL_VERSION_LIBZ},
			{"NTLM", CURL_VERSION_NTLM},
			{"NTLMWB", CURL_VERSION_NTLM_WB},
			{"SPNEGO", CURL_VERSION_SPNEGO},
			{"SSL",  CURL_VERSION_SSL},
			{"SSPI",  CURL_VERSION_SSPI},
			{"TLS-SRP", CURL_VERSION_TLSAUTH_SRP},
			{"HTTP2", CURL_VERSION_HTTP2},
			{"GSSAPI", CURL_VERSION_GSSAPI},
			{"KERBEROS5", CURL_VERSION_KERBEROS5},
			{"UNIX_SOCKETS", CURL_VERSION_UNIX_SOCKETS},
			{"PSL", CURL_VERSION_PSL},
			{"HTTPS_PROXY", CURL_VERSION_HTTPS_PROXY},
			{"MULTI_SSL", CURL_VERSION_MULTI_SSL},
			{"BROTLI", CURL_VERSION_BROTLI},
#if LIBCURL_VERSION_NUM >= 0x074001 /* Available since 7.64.1 */
			{"ALTSVC", CURL_VERSION_ALTSVC},
#endif
#if LIBCURL_VERSION_NUM >= 0x074200 /* Available since 7.66.0 */
			{"HTTP3", CURL_VERSION_HTTP3},
#endif
#if LIBCURL_VERSION_NUM >= 0x074800 /* Available since 7.72.0 */
			{"UNICODE", CURL_VERSION_UNICODE},
			{"ZSTD", CURL_VERSION_ZSTD},
#endif
#if LIBCURL_VERSION_NUM >= 0x074a00 /* Available since 7.74.0 */
			{"HSTS", CURL_VERSION_HSTS},
#endif
#if LIBCURL_VERSION_NUM >= 0x074c00 /* Available since 7.76.0 */
			{"GSASL", CURL_VERSION_GSASL},
#endif
			{NULL, 0}
		};

		php_info_print_table_row(1, "Features");
		for(i=0; i<sizeof(feats)/sizeof(feats[0]); i++) {
			if (feats[i].name) {
				php_info_print_table_row(2, feats[i].name, d->features & feats[i].bitmask ? "Yes" : "No");
			}
		}
	}

	n = 0;
	p = (char **) d->protocols;
	while (*p != NULL) {
			n += sprintf(str + n, "%s%s", *p, *(p + 1) != NULL ? ", " : "");
			p++;
	}
	php_info_print_table_row(2, "Protocols", str);

	php_info_print_table_row(2, "Host", d->host);

	if (d->ssl_version) {
		php_info_print_table_row(2, "SSL Version", d->ssl_version);
	}

	if (d->libz_version) {
		php_info_print_table_row(2, "ZLib Version", d->libz_version);
	}

#if defined(CURLVERSION_SECOND) && CURLVERSION_NOW >= CURLVERSION_SECOND
	if (d->ares) {
		php_info_print_table_row(2, "ZLib Version", d->ares);
	}
#endif

#if defined(CURLVERSION_THIRD) && CURLVERSION_NOW >= CURLVERSION_THIRD
	if (d->libidn) {
		php_info_print_table_row(2, "libIDN Version", d->libidn);
	}
#endif

	if (d->iconv_ver_num) {
		php_info_print_table_row(2, "IconV Version", d->iconv_ver_num);
	}

	if (d->libssh_version) {
		php_info_print_table_row(2, "libSSH Version", d->libssh_version);
	}

	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(curl)
{
	REGISTER_INI_ENTRIES();

	register_curl_symbols(module_number);

#ifdef PHP_CURL_NEED_OPENSSL_TSL
	if (!CRYPTO_get_id_callback()) {
		int i, c = CRYPTO_num_locks();

		php_curl_openssl_tsl = malloc(c * sizeof(MUTEX_T));
		if (!php_curl_openssl_tsl) {
			return FAILURE;
		}

		for (i = 0; i < c; ++i) {
			php_curl_openssl_tsl[i] = tsrm_mutex_alloc();
		}

		CRYPTO_set_id_callback(php_curl_ssl_id);
		CRYPTO_set_locking_callback(php_curl_ssl_lock);
	}
#endif

	if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
		return FAILURE;
	}

	curl_ce = register_class_CurlHandle();
	curl_ce->create_object = curl_create_object;
	curl_ce->default_object_handlers = &curl_object_handlers;

	memcpy(&curl_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	curl_object_handlers.offset = XtOffsetOf(php_curl, std);
	curl_object_handlers.free_obj = curl_free_obj;
	curl_object_handlers.get_gc = curl_get_gc;
	curl_object_handlers.clone_obj = curl_clone_obj;
	curl_object_handlers.cast_object = curl_cast_object;
	curl_object_handlers.compare = zend_objects_not_comparable;

	curl_multi_ce = register_class_CurlMultiHandle();
	curl_multi_register_handlers();

	curl_share_ce = register_class_CurlShareHandle();
	curl_share_register_handlers();
	curlfile_register_class();

	curl_exception_ce = register_class_CurlException(zend_ce_exception);
	curl_handle_exception_ce = register_class_CurlHandleException(curl_exception_ce);
	curl_multi_exception_ce = register_class_CurlMultiException(curl_exception_ce);
	curl_share_exception_ce = register_class_CurlShareException(curl_exception_ce);

	return SUCCESS;
}
/* }}} */

/* CurlHandle class */

static zend_object *curl_create_object(zend_class_entry *class_type) {
	php_curl *intern = zend_object_alloc(sizeof(php_curl), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	init_curl_handle(intern);

	return &intern->std;
}

static zend_object *curl_clone_obj(zend_object *object) {
	php_curl *ch;
	CURL *cp;
	zval *postfields;
	zend_object *clone_object;
	php_curl *clone_ch;

	clone_object = curl_create_object(curl_ce);
	clone_ch = curl_from_obj(clone_object);

	ch = curl_from_obj(object);
	cp = curl_easy_duphandle(ch->cp);
	if (!cp) {
		zend_throw_exception(NULL, "Failed to clone CurlHandle", 0);
		return &clone_ch->std;
	}

	clone_ch->cp = cp;
	_php_setup_easy_copy_handlers(clone_ch, ch);

	postfields = &clone_ch->postfields;
	if (Z_TYPE_P(postfields) != IS_UNDEF) {
		if (build_mime_structure_from_hash(clone_ch, postfields) == FAILURE) {
			zend_throw_exception(NULL, "Failed to clone CurlHandle", 0);
			return &clone_ch->std;
		}
	}

	return &clone_ch->std;
}

static HashTable *curl_get_gc(zend_object *object, zval **table, int *n)
{
	php_curl *curl = curl_from_obj(object);

	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();

	zend_get_gc_buffer_add_zval(gc_buffer, &curl->postfields);
	if (curl->handlers.read) {
		zend_get_gc_buffer_add_zval(gc_buffer, &curl->handlers.read->func_name);
		zend_get_gc_buffer_add_zval(gc_buffer, &curl->handlers.read->stream);
	}

	if (curl->handlers.write) {
		zend_get_gc_buffer_add_zval(gc_buffer, &curl->handlers.write->func_name);
		zend_get_gc_buffer_add_zval(gc_buffer, &curl->handlers.write->stream);
	}

	if (curl->handlers.write_header) {
		zend_get_gc_buffer_add_zval(gc_buffer, &curl->handlers.write_header->func_name);
		zend_get_gc_buffer_add_zval(gc_buffer, &curl->handlers.write_header->stream);
	}

	if (curl->handlers.progress) {
		zend_get_gc_buffer_add_zval(gc_buffer, &curl->handlers.progress->func_name);
	}

	if (curl->handlers.xferinfo) {
		zend_get_gc_buffer_add_zval(gc_buffer, &curl->handlers.xferinfo->func_name);
	}

	if (curl->handlers.fnmatch) {
		zend_get_gc_buffer_add_zval(gc_buffer, &curl->handlers.fnmatch->func_name);
	}

#if LIBCURL_VERSION_NUM >= 0x075400 /* Available since 7.84.0 */
	if (curl->handlers.sshhostkey) {
		zend_get_gc_buffer_add_zval(gc_buffer, &curl->handlers.sshhostkey->func_name);
	}
#endif

	zend_get_gc_buffer_add_zval(gc_buffer, &curl->handlers.std_err);
	zend_get_gc_buffer_add_zval(gc_buffer, &curl->private_data);

	zend_get_gc_buffer_use(gc_buffer, table, n);

	return zend_std_get_properties(object);
}

zend_result curl_cast_object(zend_object *obj, zval *result, int type)
{
	if (type == IS_LONG) {
		/* For better backward compatibility, make (int) $curl_handle return the object ID,
		 * similar to how it previously returned the resource ID. */
		ZVAL_LONG(result, obj->handle);
		return SUCCESS;
	}

	return zend_std_cast_object_tostring(obj, result, type);
}

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(curl)
{
	curl_global_cleanup();
#ifdef PHP_CURL_NEED_OPENSSL_TSL
	if (php_curl_openssl_tsl) {
		int i, c = CRYPTO_num_locks();

		CRYPTO_set_id_callback(NULL);
		CRYPTO_set_locking_callback(NULL);

		for (i = 0; i < c; ++i) {
			tsrm_mutex_free(php_curl_openssl_tsl[i]);
		}

		free(php_curl_openssl_tsl);
		php_curl_openssl_tsl = NULL;
	}
#endif
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ curl_write_nothing
 * Used as a work around. See _php_curl_close_ex
 */
static size_t curl_write_nothing(char *data, size_t size, size_t nmemb, void *ctx)
{
	return size * nmemb;
}
/* }}} */

/* {{{ curl_write */
static size_t curl_write(char *data, size_t size, size_t nmemb, void *ctx)
{
	php_curl *ch = (php_curl *) ctx;
	php_curl_write *t = ch->handlers.write;
	size_t length = size * nmemb;

#if PHP_CURL_DEBUG
	fprintf(stderr, "curl_write() called\n");
	fprintf(stderr, "data = %s, size = %d, nmemb = %d, ctx = %x\n", data, size, nmemb, ctx);
#endif

	switch (t->method) {
		case PHP_CURL_STDOUT:
			PHPWRITE(data, length);
			break;
		case PHP_CURL_FILE:
			return fwrite(data, size, nmemb, t->fp);
		case PHP_CURL_RETURN:
			if (length > 0) {
				smart_str_appendl(&t->buf, data, (int) length);
			}
			break;
		case PHP_CURL_USER: {
			zval argv[2];
			zval retval;
			int  error;
			zend_fcall_info fci;

			GC_ADDREF(&ch->std);
			ZVAL_OBJ(&argv[0], &ch->std);
			ZVAL_STRINGL(&argv[1], data, length);

			fci.size = sizeof(fci);
			fci.object = NULL;
			ZVAL_COPY_VALUE(&fci.function_name, &t->func_name);
			fci.retval = &retval;
			fci.param_count = 2;
			fci.params = argv;
			fci.named_params = NULL;

			ch->in_callback = 1;
			error = zend_call_function(&fci, &t->fci_cache);
			ch->in_callback = 0;
			if (error == FAILURE) {
				php_error_docref(NULL, E_WARNING, "Could not call the CURLOPT_WRITEFUNCTION");
				length = -1;
			} else if (!Z_ISUNDEF(retval)) {
				_php_curl_verify_handlers(ch, /* reporterror */ true);
				length = zval_get_long(&retval);
			}

			zval_ptr_dtor(&argv[0]);
			zval_ptr_dtor(&argv[1]);
			break;
		}
	}

	return length;
}
/* }}} */

/* {{{ curl_fnmatch */
static int curl_fnmatch(void *ctx, const char *pattern, const char *string)
{
	php_curl *ch = (php_curl *) ctx;
	php_curl_callback *t = ch->handlers.fnmatch;
	int rval = CURL_FNMATCHFUNC_FAIL;
	zval argv[3];
	zval retval;
	zend_result error;
	zend_fcall_info fci;

	GC_ADDREF(&ch->std);
	ZVAL_OBJ(&argv[0], &ch->std);
	ZVAL_STRING(&argv[1], pattern);
	ZVAL_STRING(&argv[2], string);

	fci.size = sizeof(fci);
	ZVAL_COPY_VALUE(&fci.function_name, &t->func_name);
	fci.object = NULL;
	fci.retval = &retval;
	fci.param_count = 3;
	fci.params = argv;
	fci.named_params = NULL;

	ch->in_callback = 1;
	error = zend_call_function(&fci, &t->fci_cache);
	ch->in_callback = 0;
	if (error == FAILURE) {
		php_error_docref(NULL, E_WARNING, "Cannot call the CURLOPT_FNMATCH_FUNCTION");
	} else if (!Z_ISUNDEF(retval)) {
		_php_curl_verify_handlers(ch, /* reporterror */ true);
		rval = zval_get_long(&retval);
	}
	zval_ptr_dtor(&argv[0]);
	zval_ptr_dtor(&argv[1]);
	zval_ptr_dtor(&argv[2]);
	return rval;
}
/* }}} */

/* {{{ curl_progress */
static size_t curl_progress(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	php_curl *ch = (php_curl *)clientp;
	php_curl_callback *t = ch->handlers.progress;
	size_t	rval = 0;

#if PHP_CURL_DEBUG
	fprintf(stderr, "curl_progress() called\n");
	fprintf(stderr, "clientp = %x, dltotal = %f, dlnow = %f, ultotal = %f, ulnow = %f\n", clientp, dltotal, dlnow, ultotal, ulnow);
#endif

	zval argv[5];
	zval retval;
	zend_result error;
	zend_fcall_info fci;

	GC_ADDREF(&ch->std);
	ZVAL_OBJ(&argv[0], &ch->std);
	ZVAL_LONG(&argv[1], (zend_long)dltotal);
	ZVAL_LONG(&argv[2], (zend_long)dlnow);
	ZVAL_LONG(&argv[3], (zend_long)ultotal);
	ZVAL_LONG(&argv[4], (zend_long)ulnow);

	fci.size = sizeof(fci);
	ZVAL_COPY_VALUE(&fci.function_name, &t->func_name);
	fci.object = NULL;
	fci.retval = &retval;
	fci.param_count = 5;
	fci.params = argv;
	fci.named_params = NULL;

	ch->in_callback = 1;
	error = zend_call_function(&fci, &t->fci_cache);
	ch->in_callback = 0;
	if (error == FAILURE) {
		php_error_docref(NULL, E_WARNING, "Cannot call the CURLOPT_PROGRESSFUNCTION");
	} else if (!Z_ISUNDEF(retval)) {
		_php_curl_verify_handlers(ch, /* reporterror */ true);
		if (0 != zval_get_long(&retval)) {
			rval = 1;
		}
	}
	zval_ptr_dtor(&argv[0]);
	return rval;
}
/* }}} */

/* {{{ curl_xferinfo */
static size_t curl_xferinfo(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
	php_curl *ch = (php_curl *)clientp;
	php_curl_callback *t = ch->handlers.xferinfo;
	size_t rval = 0;

#if PHP_CURL_DEBUG
	fprintf(stderr, "curl_xferinfo() called\n");
	fprintf(stderr, "clientp = %x, dltotal = %ld, dlnow = %ld, ultotal = %ld, ulnow = %ld\n", clientp, dltotal, dlnow, ultotal, ulnow);
#endif

	zval argv[5];
	zval retval;
	zend_result error;
	zend_fcall_info fci;

	GC_ADDREF(&ch->std);
	ZVAL_OBJ(&argv[0], &ch->std);
	ZVAL_LONG(&argv[1], dltotal);
	ZVAL_LONG(&argv[2], dlnow);
	ZVAL_LONG(&argv[3], ultotal);
	ZVAL_LONG(&argv[4], ulnow);

	fci.size = sizeof(fci);
	ZVAL_COPY_VALUE(&fci.function_name, &t->func_name);
	fci.object = NULL;
	fci.retval = &retval;
	fci.param_count = 5;
	fci.params = argv;
	fci.named_params = NULL;

	ch->in_callback = 1;
	error = zend_call_function(&fci, &t->fci_cache);
	ch->in_callback = 0;
	if (error == FAILURE) {
		php_error_docref(NULL, E_WARNING, "Cannot call the CURLOPT_XFERINFOFUNCTION");
	} else if (!Z_ISUNDEF(retval)) {
		_php_curl_verify_handlers(ch, /* reporterror */ true);
		if (0 != zval_get_long(&retval)) {
			rval = 1;
		}
	}
	zval_ptr_dtor(&argv[0]);
	return rval;
}
/* }}} */

#if LIBCURL_VERSION_NUM >= 0x075400 /* Available since 7.84.0 */
static int curl_ssh_hostkeyfunction(void *clientp, int keytype, const char *key, size_t keylen)
{
	php_curl *ch = (php_curl *)clientp;
	php_curl_callback *t = ch->handlers.sshhostkey;
	int rval = CURLKHMATCH_MISMATCH; /* cancel connection in case of an exception */

#if PHP_CURL_DEBUG
	fprintf(stderr, "curl_ssh_hostkeyfunction() called\n");
	fprintf(stderr, "clientp = %x, keytype = %d, key = %s, keylen = %zu\n", clientp, keytype, key, keylen);
#endif

	zval argv[4];
	zval retval;
	zend_result error;
	zend_fcall_info fci;

	GC_ADDREF(&ch->std);
	ZVAL_OBJ(&argv[0], &ch->std);
	ZVAL_LONG(&argv[1], keytype);
	ZVAL_STRINGL(&argv[2], key, keylen);
	ZVAL_LONG(&argv[3], keylen);

	fci.size = sizeof(fci);
	ZVAL_COPY_VALUE(&fci.function_name, &t->func_name);
	fci.object = NULL;
	fci.retval = &retval;
	fci.param_count = 4;
	fci.params = argv;
	fci.named_params = NULL;

	ch->in_callback = 1;
	error = zend_call_function(&fci, &t->fci_cache);
	ch->in_callback = 0;
	if (error == FAILURE) {
		php_error_docref(NULL, E_WARNING, "Cannot call the CURLOPT_SSH_HOSTKEYFUNCTION");
	} else if (!Z_ISUNDEF(retval)) {
		_php_curl_verify_handlers(ch, /* reporterror */ true);
		if (Z_TYPE(retval) == IS_LONG) {
			zend_long retval_long = Z_LVAL(retval);
			if (retval_long == CURLKHMATCH_OK || retval_long == CURLKHMATCH_MISMATCH) {
				rval = retval_long;
			} else {
				zend_throw_error(NULL, "The CURLOPT_SSH_HOSTKEYFUNCTION callback must return either CURLKHMATCH_OK or CURLKHMATCH_MISMATCH");
			}
		} else {
			zend_throw_error(NULL, "The CURLOPT_SSH_HOSTKEYFUNCTION callback must return either CURLKHMATCH_OK or CURLKHMATCH_MISMATCH");
		}
	}
	zval_ptr_dtor(&argv[0]);
	zval_ptr_dtor(&argv[2]);
	return rval;
}
#endif

/* {{{ curl_read */
static size_t curl_read(char *data, size_t size, size_t nmemb, void *ctx)
{
	php_curl *ch = (php_curl *)ctx;
	php_curl_read *t = ch->handlers.read;
	int length = 0;

	switch (t->method) {
		case PHP_CURL_DIRECT:
			if (t->fp) {
				length = fread(data, size, nmemb, t->fp);
			}
			break;
		case PHP_CURL_USER: {
			zval argv[3];
			zval retval;
			zend_result error;
			zend_fcall_info fci;

			GC_ADDREF(&ch->std);
			ZVAL_OBJ(&argv[0], &ch->std);
			if (t->res) {
				GC_ADDREF(t->res);
				ZVAL_RES(&argv[1], t->res);
			} else {
				ZVAL_NULL(&argv[1]);
			}
			ZVAL_LONG(&argv[2], (int)size * nmemb);

			fci.size = sizeof(fci);
			ZVAL_COPY_VALUE(&fci.function_name, &t->func_name);
			fci.object = NULL;
			fci.retval = &retval;
			fci.param_count = 3;
			fci.params = argv;
			fci.named_params = NULL;

			ch->in_callback = 1;
			error = zend_call_function(&fci, &t->fci_cache);
			ch->in_callback = 0;
			if (error == FAILURE) {
				php_error_docref(NULL, E_WARNING, "Cannot call the CURLOPT_READFUNCTION");
				length = CURL_READFUNC_ABORT;
			} else if (!Z_ISUNDEF(retval)) {
				_php_curl_verify_handlers(ch, /* reporterror */ true);
				if (Z_TYPE(retval) == IS_STRING) {
					length = MIN((int) (size * nmemb), Z_STRLEN(retval));
					memcpy(data, Z_STRVAL(retval), length);
				} else if (Z_TYPE(retval) == IS_LONG) {
					length = Z_LVAL_P(&retval);
				}
				zval_ptr_dtor(&retval);
			}

			zval_ptr_dtor(&argv[0]);
			zval_ptr_dtor(&argv[1]);
			break;
		}
	}

	return length;
}
/* }}} */

/* {{{ curl_write_header */
static size_t curl_write_header(char *data, size_t size, size_t nmemb, void *ctx)
{
	php_curl *ch = (php_curl *) ctx;
	php_curl_write *t = ch->handlers.write_header;
	size_t length = size * nmemb;

	switch (t->method) {
		case PHP_CURL_STDOUT:
			/* Handle special case write when we're returning the entire transfer
			 */
			if (ch->handlers.write->method == PHP_CURL_RETURN && length > 0) {
				smart_str_appendl(&ch->handlers.write->buf, data, (int) length);
			} else {
				PHPWRITE(data, length);
			}
			break;
		case PHP_CURL_FILE:
			return fwrite(data, size, nmemb, t->fp);
		case PHP_CURL_USER: {
			zval argv[2];
			zval retval;
			zend_result error;
			zend_fcall_info fci;

			GC_ADDREF(&ch->std);
			ZVAL_OBJ(&argv[0], &ch->std);
			ZVAL_STRINGL(&argv[1], data, length);

			fci.size = sizeof(fci);
			ZVAL_COPY_VALUE(&fci.function_name, &t->func_name);
			fci.object = NULL;
			fci.retval = &retval;
			fci.param_count = 2;
			fci.params = argv;
			fci.named_params = NULL;

			ch->in_callback = 1;
			error = zend_call_function(&fci, &t->fci_cache);
			ch->in_callback = 0;
			if (error == FAILURE) {
				php_error_docref(NULL, E_WARNING, "Could not call the CURLOPT_HEADERFUNCTION");
				length = -1;
			} else if (!Z_ISUNDEF(retval)) {
				_php_curl_verify_handlers(ch, /* reporterror */ true);
				length = zval_get_long(&retval);
			}
			zval_ptr_dtor(&argv[0]);
			zval_ptr_dtor(&argv[1]);
			break;
		}

		case PHP_CURL_IGNORE:
			return length;

		default:
			return -1;
	}

	return length;
}
/* }}} */

static int curl_debug(CURL *cp, curl_infotype type, char *buf, size_t buf_len, void *ctx) /* {{{ */
{
	php_curl *ch = (php_curl *)ctx;

	if (type == CURLINFO_HEADER_OUT) {
		if (ch->header.str) {
			zend_string_release_ex(ch->header.str, 0);
		}
		ch->header.str = zend_string_init(buf, buf_len, 0);
	}

	return 0;
}
/* }}} */

/* {{{ curl_free_post */
static void curl_free_post(void **post)
{
	curl_mime_free((curl_mime *)*post);
}
/* }}} */

struct mime_data_cb_arg {
	zend_string *filename;
	php_stream *stream;
};

/* {{{ curl_free_cb_arg */
static void curl_free_cb_arg(void **cb_arg_p)
{
	struct mime_data_cb_arg *cb_arg = (struct mime_data_cb_arg *) *cb_arg_p;

	ZEND_ASSERT(cb_arg->stream == NULL);
	zend_string_release(cb_arg->filename);
	efree(cb_arg);
}
/* }}} */

/* {{{ curl_free_slist */
static void curl_free_slist(zval *el)
{
	curl_slist_free_all(((struct curl_slist *)Z_PTR_P(el)));
}
/* }}} */

/* {{{ curl_throw_last_error */
void curl_throw_last_error(php_curl *ch, const char *unknown_error_msg) {
	if (ch->err.no) {
        ch->err.str[CURL_ERROR_SIZE] = 0;
		zend_throw_exception(curl_handle_exception_ce, ch->err.str, ch->err.no);
	} else {
		zend_throw_exception(curl_handle_exception_ce, unknown_error_msg, 0);
	}
}
/* }}} */

/* {{{ Return cURL version information. */
PHP_FUNCTION(curl_version)
{
	curl_version_info_data *d;

	ZEND_PARSE_PARAMETERS_NONE();

	d = curl_version_info(CURLVERSION_NOW);
	if (d == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);

	CAAL("version_number", d->version_num);
	CAAL("age", d->age);
	CAAL("features", d->features);
	CAAL("ssl_version_number", d->ssl_version_num);
	CAAS("version", d->version);
	CAAS("host", d->host);
	CAAS("ssl_version", d->ssl_version);
	CAAS("libz_version", d->libz_version);
	/* Add an array of protocols */
	{
		char **p = (char **) d->protocols;
		zval protocol_list;

		array_init(&protocol_list);

		while (*p != NULL) {
			add_next_index_string(&protocol_list, *p);
			p++;
		}
		CAAZ("protocols", &protocol_list);
	}
	if (d->age >= 1) {
		CAAS("ares", d->ares);
		CAAL("ares_num", d->ares_num);
	}
	if (d->age >= 2) {
		CAAS("libidn", d->libidn);
	}
	if (d->age >= 3) {
		CAAL("iconv_ver_num", d->iconv_ver_num);
		CAAS("libssh_version", d->libssh_version);
	}
	if (d->age >= 4) {
		CAAL("brotli_ver_num", d->brotli_ver_num);
		CAAS("brotli_version", d->brotli_version);
	}
}
/* }}} */

void init_curl_handle(php_curl *ch)
{
	ch->to_free = ecalloc(1, sizeof(struct _php_curl_free));
	ch->handlers.write = ecalloc(1, sizeof(php_curl_write));
	ch->handlers.write_header = ecalloc(1, sizeof(php_curl_write));
	ch->handlers.read = ecalloc(1, sizeof(php_curl_read));
	ch->handlers.progress = NULL;
	ch->handlers.xferinfo = NULL;
	ch->handlers.fnmatch = NULL;
#if LIBCURL_VERSION_NUM >= 0x075400 /* Available since 7.84.0 */
	ch->handlers.sshhostkey = NULL;
#endif
	ch->clone = emalloc(sizeof(uint32_t));
	*ch->clone = 1;

	memset(&ch->err, 0, sizeof(struct _php_curl_error));

	zend_llist_init(&ch->to_free->post,  sizeof(struct HttpPost *), (llist_dtor_func_t)curl_free_post,   0);
	zend_llist_init(&ch->to_free->stream, sizeof(struct mime_data_cb_arg *), (llist_dtor_func_t)curl_free_cb_arg, 0);

	ch->to_free->slist = emalloc(sizeof(HashTable));
	zend_hash_init(ch->to_free->slist, 4, NULL, curl_free_slist, 0);
	ZVAL_UNDEF(&ch->postfields);
}

/* }}} */

/* {{{ create_certinfo */
static void create_certinfo(struct curl_certinfo *ci, zval *listcode)
{
	int i;

	if (ci) {
		zval certhash;

		for (i=0; i<ci->num_of_certs; i++) {
			struct curl_slist *slist;

			array_init(&certhash);
			for (slist = ci->certinfo[i]; slist; slist = slist->next) {
				int len;
				char s[64];
				char *tmp;
				strncpy(s, slist->data, sizeof(s));
				s[sizeof(s)-1] = '\0';
				tmp = memchr(s, ':', sizeof(s));
				if(tmp) {
					*tmp = '\0';
					len = strlen(s);
					add_assoc_string(&certhash, s, &slist->data[len+1]);
				} else {
					php_error_docref(NULL, E_WARNING, "Could not extract hash key from certificate info");
				}
			}
			add_next_index_zval(listcode, &certhash);
		}
	}
}
/* }}} */

/* {{{ _php_curl_set_default_options()
   Set default options for a handle */
static void _php_curl_set_default_options(php_curl *ch)
{
	char *cainfo;

	curl_easy_setopt(ch->cp, CURLOPT_NOPROGRESS,        1);
	curl_easy_setopt(ch->cp, CURLOPT_VERBOSE,           0);
	curl_easy_setopt(ch->cp, CURLOPT_ERRORBUFFER,       ch->err.str);
	curl_easy_setopt(ch->cp, CURLOPT_WRITEFUNCTION,     curl_write);
	curl_easy_setopt(ch->cp, CURLOPT_FILE,              (void *) ch);
	curl_easy_setopt(ch->cp, CURLOPT_READFUNCTION,      curl_read);
	curl_easy_setopt(ch->cp, CURLOPT_INFILE,            (void *) ch);
	curl_easy_setopt(ch->cp, CURLOPT_HEADERFUNCTION,    curl_write_header);
	curl_easy_setopt(ch->cp, CURLOPT_WRITEHEADER,       (void *) ch);
#ifndef ZTS
	curl_easy_setopt(ch->cp, CURLOPT_DNS_USE_GLOBAL_CACHE, 1);
#endif
	curl_easy_setopt(ch->cp, CURLOPT_DNS_CACHE_TIMEOUT, 120);
	curl_easy_setopt(ch->cp, CURLOPT_MAXREDIRS, 20); /* prevent infinite redirects */

	cainfo = INI_STR("openssl.cafile");
	if (!(cainfo && cainfo[0] != '\0')) {
		cainfo = INI_STR("curl.cainfo");
	}
	if (cainfo && cainfo[0] != '\0') {
		curl_easy_setopt(ch->cp, CURLOPT_CAINFO, cainfo);
	}

#ifdef ZTS
	curl_easy_setopt(ch->cp, CURLOPT_NOSIGNAL, 1);
#endif
}
/* }}} */

/* {{{ Initialize a cURL session */
void php_curl_init(INTERNAL_FUNCTION_PARAMETERS, zval *dest, const char** err)
{
	php_curl *ch = Z_CURL_P(dest);
	zend_string *url = NULL;

	err = NULL;
	ZEND_PARSE_PARAMETERS_START(0,1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(url)
	ZEND_PARSE_PARAMETERS_END();

	ch->cp = curl_easy_init();
	if (!ch->cp) {
		*err = "Could not initialize a new cURL handle";
		return;
	}

	ch->handlers.write->method = PHP_CURL_STDOUT;
	ch->handlers.read->method  = PHP_CURL_DIRECT;
	ch->handlers.write_header->method = PHP_CURL_IGNORE;

	_php_curl_set_default_options(ch);

	if (url && (php_curl_option_url(ch, url) == FAILURE)) {
		*err = "Could not initialize destination URL for cURL handle";
		return;
	}
}

PHP_FUNCTION(curl_init)
{
	const char *err = NULL;

    object_init_ex(return_value, curl_ce);
	php_curl_init(INTERNAL_FUNCTION_PARAM_PASSTHRU, return_value, &err);
	if (err) {
		php_error_docref(NULL, E_WARNING, "%s", err);
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}
}

PHP_METHOD(CurlHandle, __construct)
{
	const char *err = NULL;
	php_curl_init(INTERNAL_FUNCTION_PARAM_PASSTHRU, getThis(), &err);
	if (err) {
		zend_throw_exception(curl_handle_exception_ce, err, 0);
		RETURN_THROWS();
	}

	// Unlike the instance produced by curl_init(),
	// (new CurlHandle) returns the transfer by default.
	Z_CURL_P(getThis())->handlers.write->method = PHP_CURL_RETURN;
}
/* }}} */

static void _php_copy_callback(php_curl *ch, php_curl_callback **new_callback, php_curl_callback *source_callback, CURLoption option)
{
	if (source_callback) {
		*new_callback = ecalloc(1, sizeof(php_curl_callback));
		if (!Z_ISUNDEF(source_callback->func_name)) {
			ZVAL_COPY(&(*new_callback)->func_name, &source_callback->func_name);
		}
		curl_easy_setopt(ch->cp, option, (void *) ch);
	}
}

void _php_setup_easy_copy_handlers(php_curl *ch, php_curl *source)
{
	if (!Z_ISUNDEF(source->handlers.write->stream)) {
		Z_ADDREF(source->handlers.write->stream);
	}
	ch->handlers.write->stream = source->handlers.write->stream;
	ch->handlers.write->method = source->handlers.write->method;
	if (!Z_ISUNDEF(source->handlers.read->stream)) {
		Z_ADDREF(source->handlers.read->stream);
	}
	ch->handlers.read->stream  = source->handlers.read->stream;
	ch->handlers.read->method  = source->handlers.read->method;
	ch->handlers.write_header->method = source->handlers.write_header->method;
	if (!Z_ISUNDEF(source->handlers.write_header->stream)) {
		Z_ADDREF(source->handlers.write_header->stream);
	}
	ch->handlers.write_header->stream = source->handlers.write_header->stream;

	ch->handlers.write->fp = source->handlers.write->fp;
	ch->handlers.write_header->fp = source->handlers.write_header->fp;
	ch->handlers.read->fp = source->handlers.read->fp;
	ch->handlers.read->res = source->handlers.read->res;

	if (!Z_ISUNDEF(source->handlers.write->func_name)) {
		ZVAL_COPY(&ch->handlers.write->func_name, &source->handlers.write->func_name);
	}
	if (!Z_ISUNDEF(source->handlers.read->func_name)) {
		ZVAL_COPY(&ch->handlers.read->func_name, &source->handlers.read->func_name);
	}
	if (!Z_ISUNDEF(source->handlers.write_header->func_name)) {
		ZVAL_COPY(&ch->handlers.write_header->func_name, &source->handlers.write_header->func_name);
	}

	curl_easy_setopt(ch->cp, CURLOPT_ERRORBUFFER,       ch->err.str);
	curl_easy_setopt(ch->cp, CURLOPT_FILE,              (void *) ch);
	curl_easy_setopt(ch->cp, CURLOPT_INFILE,            (void *) ch);
	curl_easy_setopt(ch->cp, CURLOPT_WRITEHEADER,       (void *) ch);
	curl_easy_setopt(ch->cp, CURLOPT_DEBUGDATA,         (void *) ch);

	_php_copy_callback(ch, &ch->handlers.progress, source->handlers.progress, CURLOPT_PROGRESSDATA);
	_php_copy_callback(ch, &ch->handlers.xferinfo, source->handlers.xferinfo, CURLOPT_XFERINFODATA);
	_php_copy_callback(ch, &ch->handlers.fnmatch, source->handlers.fnmatch, CURLOPT_FNMATCH_DATA);
#if LIBCURL_VERSION_NUM >= 0x075400 /* Available since 7.84.0 */
	_php_copy_callback(ch, &ch->handlers.sshhostkey, source->handlers.sshhostkey, CURLOPT_SSH_HOSTKEYDATA);
#endif

	ZVAL_COPY(&ch->private_data, &source->private_data);

	efree(ch->to_free->slist);
	efree(ch->to_free);
	ch->to_free = source->to_free;
	efree(ch->clone);
	ch->clone = source->clone;

	/* Keep track of cloned copies to avoid invoking curl destructors for every clone */
	(*source->clone)++;
}

static size_t read_cb(char *buffer, size_t size, size_t nitems, void *arg) /* {{{ */
{
	struct mime_data_cb_arg *cb_arg = (struct mime_data_cb_arg *) arg;
	ssize_t numread;

	if (cb_arg->stream == NULL) {
		if (!(cb_arg->stream = php_stream_open_wrapper(ZSTR_VAL(cb_arg->filename), "rb", IGNORE_PATH, NULL))) {
			return CURL_READFUNC_ABORT;
		}
	}
	numread = php_stream_read(cb_arg->stream, buffer, nitems * size);
	if (numread < 0) {
		php_stream_close(cb_arg->stream);
		cb_arg->stream = NULL;
		return CURL_READFUNC_ABORT;
	}
	return numread;
}
/* }}} */

static int seek_cb(void *arg, curl_off_t offset, int origin) /* {{{ */
{
	struct mime_data_cb_arg *cb_arg = (struct mime_data_cb_arg *) arg;
	int res;

	if (cb_arg->stream == NULL) {
		return CURL_SEEKFUNC_CANTSEEK;
	}
	res = php_stream_seek(cb_arg->stream, offset, origin);
	return res == SUCCESS ? CURL_SEEKFUNC_OK : CURL_SEEKFUNC_CANTSEEK;
}
/* }}} */

static void free_cb(void *arg) /* {{{ */
{
	struct mime_data_cb_arg *cb_arg = (struct mime_data_cb_arg *) arg;

	if (cb_arg->stream != NULL) {
		php_stream_close(cb_arg->stream);
		cb_arg->stream = NULL;
	}
}
/* }}} */

static inline CURLcode add_simple_field(curl_mime *mime, zend_string *string_key, zval *current)
{
	CURLcode error = CURLE_OK;
	curl_mimepart *part;
	CURLcode form_error;
	zend_string *postval, *tmp_postval;

	postval = zval_get_tmp_string(current, &tmp_postval);

	part = curl_mime_addpart(mime);
	if (part == NULL) {
		zend_tmp_string_release(tmp_postval);
		zend_string_release_ex(string_key, 0);
		return CURLE_OUT_OF_MEMORY;
	}
	if ((form_error = curl_mime_name(part, ZSTR_VAL(string_key))) != CURLE_OK
		|| (form_error = curl_mime_data(part, ZSTR_VAL(postval), ZSTR_LEN(postval))) != CURLE_OK) {
		error = form_error;
	}

	zend_tmp_string_release(tmp_postval);

	return error;
}

static inline zend_result build_mime_structure_from_hash(php_curl *ch, zval *zpostfields) /* {{{ */
{
	HashTable *postfields = Z_ARRVAL_P(zpostfields);
	CURLcode error = CURLE_OK;
	zval *current;
	zend_string *string_key;
	zend_ulong num_key;
	curl_mime *mime = NULL;
	curl_mimepart *part;
	CURLcode form_error;

	if (zend_hash_num_elements(postfields) > 0) {
		mime = curl_mime_init(ch->cp);
		if (mime == NULL) {
			return FAILURE;
		}
	}

	ZEND_HASH_FOREACH_KEY_VAL(postfields, num_key, string_key, current) {
		zend_string *postval;
		/* Pretend we have a string_key here */
		if (!string_key) {
			string_key = zend_long_to_str(num_key);
		} else {
			zend_string_addref(string_key);
		}

		ZVAL_DEREF(current);
		if (Z_TYPE_P(current) == IS_OBJECT &&
				instanceof_function(Z_OBJCE_P(current), curl_CURLFile_class)) {
			/* new-style file upload */
			zval *prop, rv;
			char *type = NULL, *filename = NULL;
			struct mime_data_cb_arg *cb_arg;
			php_stream *stream;
			php_stream_statbuf ssb;
			size_t filesize = -1;
			curl_seek_callback seekfunc = seek_cb;

			prop = zend_read_property_ex(curl_CURLFile_class, Z_OBJ_P(current), ZSTR_KNOWN(ZEND_STR_NAME), /* silent */ false, &rv);
			ZVAL_DEREF(prop);
			if (Z_TYPE_P(prop) != IS_STRING) {
				php_error_docref(NULL, E_WARNING, "Invalid filename for key %s", ZSTR_VAL(string_key));
			} else {
				postval = Z_STR_P(prop);

				if (php_check_open_basedir(ZSTR_VAL(postval))) {
					return FAILURE;
				}

				prop = zend_read_property(curl_CURLFile_class, Z_OBJ_P(current), "mime", sizeof("mime")-1, 0, &rv);
				ZVAL_DEREF(prop);
				if (Z_TYPE_P(prop) == IS_STRING && Z_STRLEN_P(prop) > 0) {
					type = Z_STRVAL_P(prop);
				}
				prop = zend_read_property(curl_CURLFile_class, Z_OBJ_P(current), "postname", sizeof("postname")-1, 0, &rv);
				ZVAL_DEREF(prop);
				if (Z_TYPE_P(prop) == IS_STRING && Z_STRLEN_P(prop) > 0) {
					filename = Z_STRVAL_P(prop);
				}

				zval_ptr_dtor(&ch->postfields);
				ZVAL_COPY(&ch->postfields, zpostfields);

				if ((stream = php_stream_open_wrapper(ZSTR_VAL(postval), "rb", STREAM_MUST_SEEK, NULL))) {
					if (!stream->readfilters.head && !php_stream_stat(stream, &ssb)) {
						filesize = ssb.sb.st_size;
					}
				} else {
					seekfunc = NULL;
				}

				cb_arg = emalloc(sizeof *cb_arg);
				cb_arg->filename = zend_string_copy(postval);
				cb_arg->stream = stream;

				part = curl_mime_addpart(mime);
				if (part == NULL) {
					zend_string_release_ex(string_key, 0);
					return FAILURE;
				}
				if ((form_error = curl_mime_name(part, ZSTR_VAL(string_key))) != CURLE_OK
					|| (form_error = curl_mime_data_cb(part, filesize, read_cb, seekfunc, free_cb, cb_arg)) != CURLE_OK
					|| (form_error = curl_mime_filename(part, filename ? filename : ZSTR_VAL(postval))) != CURLE_OK
					|| (form_error = curl_mime_type(part, type ? type : "application/octet-stream")) != CURLE_OK) {
					error = form_error;
				}
				zend_llist_add_element(&ch->to_free->stream, &cb_arg);
			}

			zend_string_release_ex(string_key, 0);
			continue;
		}

		if (Z_TYPE_P(current) == IS_OBJECT && instanceof_function(Z_OBJCE_P(current), curl_CURLStringFile_class)) {
			/* new-style file upload from string */
			zval *prop, rv;
			char *type = NULL, *filename = NULL;

			prop = zend_read_property(curl_CURLStringFile_class, Z_OBJ_P(current), "postname", sizeof("postname")-1, 0, &rv);
			if (EG(exception)) {
				zend_string_release_ex(string_key, 0);
				return FAILURE;
			}
			ZVAL_DEREF(prop);
			ZEND_ASSERT(Z_TYPE_P(prop) == IS_STRING);

			filename = Z_STRVAL_P(prop);

			prop = zend_read_property(curl_CURLStringFile_class, Z_OBJ_P(current), "mime", sizeof("mime")-1, 0, &rv);
			if (EG(exception)) {
				zend_string_release_ex(string_key, 0);
				return FAILURE;
			}
			ZVAL_DEREF(prop);
			ZEND_ASSERT(Z_TYPE_P(prop) == IS_STRING);

			type = Z_STRVAL_P(prop);

			prop = zend_read_property(curl_CURLStringFile_class, Z_OBJ_P(current), "data", sizeof("data")-1, 0, &rv);
			if (EG(exception)) {
				zend_string_release_ex(string_key, 0);
				return FAILURE;
			}
			ZVAL_DEREF(prop);
			ZEND_ASSERT(Z_TYPE_P(prop) == IS_STRING);

			postval = Z_STR_P(prop);

			zval_ptr_dtor(&ch->postfields);
			ZVAL_COPY(&ch->postfields, zpostfields);

			part = curl_mime_addpart(mime);
			if (part == NULL) {
				zend_string_release_ex(string_key, 0);
				return FAILURE;
			}
			if ((form_error = curl_mime_name(part, ZSTR_VAL(string_key))) != CURLE_OK
				|| (form_error = curl_mime_data(part, ZSTR_VAL(postval), ZSTR_LEN(postval))) != CURLE_OK
				|| (form_error = curl_mime_filename(part, filename)) != CURLE_OK
				|| (form_error = curl_mime_type(part, type)) != CURLE_OK) {
				error = form_error;
			}

			zend_string_release_ex(string_key, 0);
			continue;
		}

		if (Z_TYPE_P(current) == IS_ARRAY) {
			zval *current_element;

			ZEND_HASH_FOREACH_VAL(HASH_OF(current), current_element) {
				add_simple_field(mime, string_key, current_element);
			} ZEND_HASH_FOREACH_END();

			zend_string_release_ex(string_key, 0);
			continue;
		}

		add_simple_field(mime, string_key, current);

		zend_string_release_ex(string_key, 0);
	} ZEND_HASH_FOREACH_END();

	SAVE_CURL_ERROR(ch, error);
	if (error != CURLE_OK) {
		return FAILURE;
	}

	if ((*ch->clone) == 1) {
		zend_llist_clean(&ch->to_free->post);
	}
	zend_llist_add_element(&ch->to_free->post, &mime);
	error = curl_easy_setopt(ch->cp, CURLOPT_MIMEPOST, mime);

	SAVE_CURL_ERROR(ch, error);
	return error == CURLE_OK ? SUCCESS : FAILURE;
}
/* }}} */

/* {{{ Copy a cURL handle along with all of it's preferences */
PHP_FUNCTION(curl_copy_handle)
{
	php_curl	*ch;
	CURL		*cp;
	zval		*zid;
	php_curl	*dupch;
	zval		*postfields;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_OBJECT_OF_CLASS(zid, curl_ce)
	ZEND_PARSE_PARAMETERS_END();

	ch = Z_CURL_P(zid);

	cp = curl_easy_duphandle(ch->cp);
	if (!cp) {
		php_error_docref(NULL, E_WARNING, "Cannot duplicate cURL handle");
		RETURN_FALSE;
	}

	object_init_ex(return_value, curl_ce);
	dupch = Z_CURL_P(return_value);
	dupch->cp = cp;

	_php_setup_easy_copy_handlers(dupch, ch);

	postfields = &ch->postfields;
	if (Z_TYPE_P(postfields) != IS_UNDEF) {
		if (build_mime_structure_from_hash(dupch, postfields) == FAILURE) {
			zval_ptr_dtor(return_value);
			php_error_docref(NULL, E_WARNING, "Cannot rebuild mime structure");
			RETURN_FALSE;
		}
	}
}
/* }}} */

static zend_result _php_curl_setopt(php_curl *ch, zend_long option, zval *zvalue, bool is_array_config) /* {{{ */
{
	CURLcode error = CURLE_OK;
	zend_long lval;

	switch (option) {
		/* Long options */
		case CURLOPT_SSL_VERIFYHOST:
			lval = zval_get_long(zvalue);
			if (lval == 1) {
				php_error_docref(NULL, E_NOTICE, "CURLOPT_SSL_VERIFYHOST no longer accepts the value 1, value 2 will be used instead");
				error = curl_easy_setopt(ch->cp, option, 2);
				break;
			}
			ZEND_FALLTHROUGH;
		case CURLOPT_AUTOREFERER:
		case CURLOPT_BUFFERSIZE:
		case CURLOPT_CONNECTTIMEOUT:
		case CURLOPT_COOKIESESSION:
		case CURLOPT_CRLF:
		case CURLOPT_DNS_CACHE_TIMEOUT:
		case CURLOPT_DNS_USE_GLOBAL_CACHE:
		case CURLOPT_FAILONERROR:
		case CURLOPT_FILETIME:
		case CURLOPT_FORBID_REUSE:
		case CURLOPT_FRESH_CONNECT:
		case CURLOPT_FTP_USE_EPRT:
		case CURLOPT_FTP_USE_EPSV:
		case CURLOPT_HEADER:
		case CURLOPT_HTTPGET:
		case CURLOPT_HTTPPROXYTUNNEL:
		case CURLOPT_HTTP_VERSION:
		case CURLOPT_INFILESIZE:
		case CURLOPT_LOW_SPEED_LIMIT:
		case CURLOPT_LOW_SPEED_TIME:
		case CURLOPT_MAXCONNECTS:
		case CURLOPT_MAXREDIRS:
		case CURLOPT_NETRC:
		case CURLOPT_NOBODY:
		case CURLOPT_NOPROGRESS:
		case CURLOPT_NOSIGNAL:
		case CURLOPT_PORT:
		case CURLOPT_POST:
		case CURLOPT_PROXYPORT:
		case CURLOPT_PROXYTYPE:
		case CURLOPT_PUT:
		case CURLOPT_RESUME_FROM:
		case CURLOPT_SSLVERSION:
		case CURLOPT_SSL_VERIFYPEER:
		case CURLOPT_TIMECONDITION:
		case CURLOPT_TIMEOUT:
		case CURLOPT_TIMEVALUE:
		case CURLOPT_TRANSFERTEXT:
		case CURLOPT_UNRESTRICTED_AUTH:
		case CURLOPT_UPLOAD:
		case CURLOPT_VERBOSE:
		case CURLOPT_HTTPAUTH:
		case CURLOPT_FTP_CREATE_MISSING_DIRS:
		case CURLOPT_PROXYAUTH:
		case CURLOPT_FTP_RESPONSE_TIMEOUT:
		case CURLOPT_IPRESOLVE:
		case CURLOPT_MAXFILESIZE:
		case CURLOPT_TCP_NODELAY:
		case CURLOPT_FTPSSLAUTH:
		case CURLOPT_IGNORE_CONTENT_LENGTH:
		case CURLOPT_FTP_SKIP_PASV_IP:
		case CURLOPT_FTP_FILEMETHOD:
		case CURLOPT_CONNECT_ONLY:
		case CURLOPT_LOCALPORT:
		case CURLOPT_LOCALPORTRANGE:
		case CURLOPT_SSL_SESSIONID_CACHE:
		case CURLOPT_FTP_SSL_CCC:
		case CURLOPT_SSH_AUTH_TYPES:
		case CURLOPT_CONNECTTIMEOUT_MS:
		case CURLOPT_HTTP_CONTENT_DECODING:
		case CURLOPT_HTTP_TRANSFER_DECODING:
		case CURLOPT_TIMEOUT_MS:
		case CURLOPT_NEW_DIRECTORY_PERMS:
		case CURLOPT_NEW_FILE_PERMS:
		case CURLOPT_USE_SSL:
		case CURLOPT_APPEND:
		case CURLOPT_DIRLISTONLY:
		case CURLOPT_PROXY_TRANSFER_MODE:
		case CURLOPT_ADDRESS_SCOPE:
		case CURLOPT_CERTINFO:
		case CURLOPT_PROTOCOLS:
		case CURLOPT_REDIR_PROTOCOLS:
		case CURLOPT_SOCKS5_GSSAPI_NEC:
		case CURLOPT_TFTP_BLKSIZE:
		case CURLOPT_FTP_USE_PRET:
		case CURLOPT_RTSP_CLIENT_CSEQ:
		case CURLOPT_RTSP_REQUEST:
		case CURLOPT_RTSP_SERVER_CSEQ:
		case CURLOPT_WILDCARDMATCH:
		case CURLOPT_GSSAPI_DELEGATION:
		case CURLOPT_ACCEPTTIMEOUT_MS:
		case CURLOPT_SSL_OPTIONS:
		case CURLOPT_TCP_KEEPALIVE:
		case CURLOPT_TCP_KEEPIDLE:
		case CURLOPT_TCP_KEEPINTVL:
		case CURLOPT_SASL_IR:
		case CURLOPT_EXPECT_100_TIMEOUT_MS:
		case CURLOPT_SSL_ENABLE_ALPN:
		case CURLOPT_SSL_ENABLE_NPN:
		case CURLOPT_HEADEROPT:
		case CURLOPT_SSL_VERIFYSTATUS:
		case CURLOPT_PATH_AS_IS:
		case CURLOPT_SSL_FALSESTART:
		case CURLOPT_PIPEWAIT:
		case CURLOPT_STREAM_WEIGHT:
		case CURLOPT_TFTP_NO_OPTIONS:
		case CURLOPT_TCP_FASTOPEN:
		case CURLOPT_KEEP_SENDING_ON_ERROR:
		case CURLOPT_PROXY_SSL_OPTIONS:
		case CURLOPT_PROXY_SSL_VERIFYHOST:
		case CURLOPT_PROXY_SSL_VERIFYPEER:
		case CURLOPT_PROXY_SSLVERSION:
		case CURLOPT_SUPPRESS_CONNECT_HEADERS:
		case CURLOPT_SOCKS5_AUTH:
		case CURLOPT_SSH_COMPRESSION:
		case CURLOPT_HAPPY_EYEBALLS_TIMEOUT_MS:
		case CURLOPT_DNS_SHUFFLE_ADDRESSES:
		case CURLOPT_HAPROXYPROTOCOL:
		case CURLOPT_DISALLOW_USERNAME_IN_URL:
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
		case CURLOPT_UPKEEP_INTERVAL_MS:
		case CURLOPT_UPLOAD_BUFFERSIZE:
#endif
#if LIBCURL_VERSION_NUM >= 0x074000 /* Available since 7.64.0 */
		case CURLOPT_HTTP09_ALLOWED:
#endif
#if LIBCURL_VERSION_NUM >= 0x074001 /* Available since 7.64.1 */
		case CURLOPT_ALTSVC_CTRL:
#endif
#if LIBCURL_VERSION_NUM >= 0x074100 /* Available since 7.65.0 */
		case CURLOPT_MAXAGE_CONN:
#endif
#if LIBCURL_VERSION_NUM >= 0x074500 /* Available since 7.69.0 */
		case CURLOPT_MAIL_RCPT_ALLLOWFAILS:
#endif
#if LIBCURL_VERSION_NUM >= 0x074a00 /* Available since 7.74.0 */
		case CURLOPT_HSTS_CTRL:
#endif
#if LIBCURL_VERSION_NUM >= 0x074c00 /* Available since 7.76.0 */
		case CURLOPT_DOH_SSL_VERIFYHOST:
		case CURLOPT_DOH_SSL_VERIFYPEER:
		case CURLOPT_DOH_SSL_VERIFYSTATUS:
#endif
#if LIBCURL_VERSION_NUM >= 0x075000 /* Available since 7.80.0 */
		case CURLOPT_MAXLIFETIME_CONN:
#endif
#if LIBCURL_VERSION_NUM >= 0x075100 /* Available since 7.81.0 */
		case CURLOPT_MIME_OPTIONS:
#endif
#if LIBCURL_VERSION_NUM >= 0x075600 /* Available since 7.86.0 */
		case CURLOPT_WS_OPTIONS:
#endif
#if LIBCURL_VERSION_NUM >= 0x075700 /* Available since 7.87.0 */
		case CURLOPT_CA_CACHE_TIMEOUT:
		case CURLOPT_QUICK_EXIT:
#endif
			lval = zval_get_long(zvalue);
			if ((option == CURLOPT_PROTOCOLS || option == CURLOPT_REDIR_PROTOCOLS) &&
				(PG(open_basedir) && *PG(open_basedir)) && (lval & CURLPROTO_FILE)) {
					php_error_docref(NULL, E_WARNING, "CURLPROTO_FILE cannot be activated when an open_basedir is set");
					return FAILURE;
			}
# if defined(ZTS)
			if (option == CURLOPT_DNS_USE_GLOBAL_CACHE && lval) {
				php_error_docref(NULL, E_WARNING, "CURLOPT_DNS_USE_GLOBAL_CACHE cannot be activated when thread safety is enabled");
				return FAILURE;
			}
# endif
			error = curl_easy_setopt(ch->cp, option, lval);
			break;
		case CURLOPT_SAFE_UPLOAD:
			if (!zend_is_true(zvalue)) {
				zend_value_error("%s(): Disabling safe uploads is no longer supported", get_active_function_name());
				return FAILURE;
			}
			break;

		/* String options */
		case CURLOPT_CAINFO:
		case CURLOPT_CAPATH:
		case CURLOPT_COOKIE:
		case CURLOPT_EGDSOCKET:
		case CURLOPT_INTERFACE:
		case CURLOPT_PROXY:
		case CURLOPT_PROXYUSERPWD:
		case CURLOPT_REFERER:
		case CURLOPT_SSLCERTTYPE:
		case CURLOPT_SSLENGINE:
		case CURLOPT_SSLENGINE_DEFAULT:
		case CURLOPT_SSLKEY:
		case CURLOPT_SSLKEYPASSWD:
		case CURLOPT_SSLKEYTYPE:
		case CURLOPT_SSL_CIPHER_LIST:
		case CURLOPT_USERAGENT:
		case CURLOPT_USERPWD:
		case CURLOPT_COOKIELIST:
		case CURLOPT_FTP_ALTERNATIVE_TO_USER:
		case CURLOPT_SSH_HOST_PUBLIC_KEY_MD5:
		case CURLOPT_PASSWORD:
		case CURLOPT_PROXYPASSWORD:
		case CURLOPT_PROXYUSERNAME:
		case CURLOPT_USERNAME:
		case CURLOPT_NOPROXY:
		case CURLOPT_SOCKS5_GSSAPI_SERVICE:
		case CURLOPT_MAIL_FROM:
		case CURLOPT_RTSP_STREAM_URI:
		case CURLOPT_RTSP_TRANSPORT:
		case CURLOPT_TLSAUTH_TYPE:
		case CURLOPT_TLSAUTH_PASSWORD:
		case CURLOPT_TLSAUTH_USERNAME:
		case CURLOPT_TRANSFER_ENCODING:
		case CURLOPT_DNS_SERVERS:
		case CURLOPT_MAIL_AUTH:
		case CURLOPT_LOGIN_OPTIONS:
		case CURLOPT_PINNEDPUBLICKEY:
		case CURLOPT_PROXY_SERVICE_NAME:
		case CURLOPT_SERVICE_NAME:
		case CURLOPT_DEFAULT_PROTOCOL:
		case CURLOPT_PRE_PROXY:
		case CURLOPT_PROXY_CAINFO:
		case CURLOPT_PROXY_CAPATH:
		case CURLOPT_PROXY_CRLFILE:
		case CURLOPT_PROXY_KEYPASSWD:
		case CURLOPT_PROXY_PINNEDPUBLICKEY:
		case CURLOPT_PROXY_SSL_CIPHER_LIST:
		case CURLOPT_PROXY_SSLCERT:
		case CURLOPT_PROXY_SSLCERTTYPE:
		case CURLOPT_PROXY_SSLKEY:
		case CURLOPT_PROXY_SSLKEYTYPE:
		case CURLOPT_PROXY_TLSAUTH_PASSWORD:
		case CURLOPT_PROXY_TLSAUTH_TYPE:
		case CURLOPT_PROXY_TLSAUTH_USERNAME:
		case CURLOPT_ABSTRACT_UNIX_SOCKET:
		case CURLOPT_REQUEST_TARGET:
		case CURLOPT_PROXY_TLS13_CIPHERS:
		case CURLOPT_TLS13_CIPHERS:
#if LIBCURL_VERSION_NUM >= 0x074001 /* Available since 7.64.1 */
		case CURLOPT_ALTSVC:
#endif
#if LIBCURL_VERSION_NUM >= 0x074200 /* Available since 7.66.0 */
		case CURLOPT_SASL_AUTHZID:
#endif
#if LIBCURL_VERSION_NUM >= 0x074700 /* Available since 7.71.0 */
		case CURLOPT_PROXY_ISSUERCERT:
#endif
#if LIBCURL_VERSION_NUM >= 0x074900 /* Available since 7.73.0 */
		case CURLOPT_SSL_EC_CURVES:
#endif
#if LIBCURL_VERSION_NUM >= 0x074b00 /* Available since 7.75.0 */
		case CURLOPT_AWS_SIGV4:
#endif
#if LIBCURL_VERSION_NUM >= 0x075000 /* Available since 7.80.0 */
		case CURLOPT_SSH_HOST_PUBLIC_KEY_SHA256:
#endif
#if LIBCURL_VERSION_NUM >= 0x075500 /* Available since 7.85.0 */
		case CURLOPT_PROTOCOLS_STR:
		case CURLOPT_REDIR_PROTOCOLS_STR:
#endif
		{
			zend_string *tmp_str;
			zend_string *str = zval_get_tmp_string(zvalue, &tmp_str);
#if LIBCURL_VERSION_NUM >= 0x075500 /* Available since 7.85.0 */
			if ((option == CURLOPT_PROTOCOLS_STR || option == CURLOPT_REDIR_PROTOCOLS_STR) &&
				(PG(open_basedir) && *PG(open_basedir)) && php_memnistr(ZSTR_VAL(str), "file", sizeof("file") - 1, ZSTR_VAL(str) + ZSTR_LEN(str)) != NULL) {
					php_error_docref(NULL, E_WARNING, "The FILE protocol cannot be activated when an open_basedir is set");
					return FAILURE;
			}
#endif
			zend_result ret = php_curl_option_str(ch, option, ZSTR_VAL(str), ZSTR_LEN(str));
			zend_tmp_string_release(tmp_str);
			return ret;
		}

		/* Curl nullable string options */
		case CURLOPT_CUSTOMREQUEST:
		case CURLOPT_FTPPORT:
		case CURLOPT_RANGE:
		case CURLOPT_FTP_ACCOUNT:
		case CURLOPT_RTSP_SESSION_ID:
		case CURLOPT_ACCEPT_ENCODING:
		case CURLOPT_DNS_INTERFACE:
		case CURLOPT_DNS_LOCAL_IP4:
		case CURLOPT_DNS_LOCAL_IP6:
		case CURLOPT_XOAUTH2_BEARER:
		case CURLOPT_UNIX_SOCKET_PATH:
#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
		case CURLOPT_DOH_URL:
#endif
#if LIBCURL_VERSION_NUM >= 0x074a00 /* Available since 7.74.0 */
		case CURLOPT_HSTS:
#endif
		case CURLOPT_KRBLEVEL:
		{
			if (Z_ISNULL_P(zvalue)) {
				error = curl_easy_setopt(ch->cp, option, NULL);
			} else {
				zend_string *tmp_str;
				zend_string *str = zval_get_tmp_string(zvalue, &tmp_str);
				zend_result ret = php_curl_option_str(ch, option, ZSTR_VAL(str), ZSTR_LEN(str));
				zend_tmp_string_release(tmp_str);
				return ret;
			}
			break;
		}

		/* Curl private option */
		case CURLOPT_PRIVATE:
		{
			zval_ptr_dtor(&ch->private_data);
			ZVAL_COPY(&ch->private_data, zvalue);
			return SUCCESS;
		}

		/* Curl url option */
		case CURLOPT_URL:
		{
			zend_string *tmp_str;
			zend_string *str = zval_get_tmp_string(zvalue, &tmp_str);
			zend_result ret = php_curl_option_url(ch, str);
			zend_tmp_string_release(tmp_str);
			return ret;
		}

		/* Curl file handle options */
		case CURLOPT_FILE:
		case CURLOPT_INFILE:
		case CURLOPT_STDERR:
		case CURLOPT_WRITEHEADER: {
			FILE *fp = NULL;
			php_stream *what = NULL;

			if (Z_TYPE_P(zvalue) != IS_NULL) {
				what = (php_stream *)zend_fetch_resource2_ex(zvalue, "File-Handle", php_file_le_stream(), php_file_le_pstream());
				if (!what) {
					return FAILURE;
				}

				if (FAILURE == php_stream_cast(what, PHP_STREAM_AS_STDIO, (void *) &fp, REPORT_ERRORS)) {
					return FAILURE;
				}

				if (!fp) {
					return FAILURE;
				}
			}

			error = CURLE_OK;
			switch (option) {
				case CURLOPT_FILE:
					if (!what) {
						if (!Z_ISUNDEF(ch->handlers.write->stream)) {
							zval_ptr_dtor(&ch->handlers.write->stream);
							ZVAL_UNDEF(&ch->handlers.write->stream);
						}
						ch->handlers.write->fp = NULL;
						ch->handlers.write->method = PHP_CURL_STDOUT;
					} else if (what->mode[0] != 'r' || what->mode[1] == '+') {
						zval_ptr_dtor(&ch->handlers.write->stream);
						ch->handlers.write->fp = fp;
						ch->handlers.write->method = PHP_CURL_FILE;
						ZVAL_COPY(&ch->handlers.write->stream, zvalue);
					} else {
						zend_value_error("%s(): The provided file handle must be writable", get_active_function_name());
						return FAILURE;
					}
					break;
				case CURLOPT_WRITEHEADER:
					if (!what) {
						if (!Z_ISUNDEF(ch->handlers.write_header->stream)) {
							zval_ptr_dtor(&ch->handlers.write_header->stream);
							ZVAL_UNDEF(&ch->handlers.write_header->stream);
						}
						ch->handlers.write_header->fp = NULL;
						ch->handlers.write_header->method = PHP_CURL_IGNORE;
					} else if (what->mode[0] != 'r' || what->mode[1] == '+') {
						zval_ptr_dtor(&ch->handlers.write_header->stream);
						ch->handlers.write_header->fp = fp;
						ch->handlers.write_header->method = PHP_CURL_FILE;
						ZVAL_COPY(&ch->handlers.write_header->stream, zvalue);
					} else {
						zend_value_error("%s(): The provided file handle must be writable", get_active_function_name());
						return FAILURE;
					}
					break;
				case CURLOPT_INFILE:
					if (!what) {
						if (!Z_ISUNDEF(ch->handlers.read->stream)) {
							zval_ptr_dtor(&ch->handlers.read->stream);
							ZVAL_UNDEF(&ch->handlers.read->stream);
						}
						ch->handlers.read->fp = NULL;
						ch->handlers.read->res = NULL;
					} else {
						zval_ptr_dtor(&ch->handlers.read->stream);
						ch->handlers.read->fp = fp;
						ch->handlers.read->res = Z_RES_P(zvalue);
						ZVAL_COPY(&ch->handlers.read->stream, zvalue);
					}
					break;
				case CURLOPT_STDERR:
					if (!what) {
						if (!Z_ISUNDEF(ch->handlers.std_err)) {
							zval_ptr_dtor(&ch->handlers.std_err);
							ZVAL_UNDEF(&ch->handlers.std_err);
						}
					} else if (what->mode[0] != 'r' || what->mode[1] == '+') {
						zval_ptr_dtor(&ch->handlers.std_err);
						ZVAL_COPY(&ch->handlers.std_err, zvalue);
					} else {
						zend_value_error("%s(): The provided file handle must be writable", get_active_function_name());
						return FAILURE;
					}
					ZEND_FALLTHROUGH;
				default:
					error = curl_easy_setopt(ch->cp, option, fp);
					break;
			}
			break;
		}

		/* Curl linked list options */
		case CURLOPT_HTTP200ALIASES:
		case CURLOPT_HTTPHEADER:
		case CURLOPT_POSTQUOTE:
		case CURLOPT_PREQUOTE:
		case CURLOPT_QUOTE:
		case CURLOPT_TELNETOPTIONS:
		case CURLOPT_MAIL_RCPT:
		case CURLOPT_RESOLVE:
		case CURLOPT_PROXYHEADER:
		case CURLOPT_CONNECT_TO:
		{
			zval *current;
			HashTable *ph;
			zend_string *val, *tmp_val;
			struct curl_slist *slist = NULL;

			if (Z_TYPE_P(zvalue) != IS_ARRAY) {
				const char *name = NULL;
				switch (option) {
					case CURLOPT_HTTPHEADER:
						name = "CURLOPT_HTTPHEADER";
						break;
					case CURLOPT_QUOTE:
						name = "CURLOPT_QUOTE";
						break;
					case CURLOPT_HTTP200ALIASES:
						name = "CURLOPT_HTTP200ALIASES";
						break;
					case CURLOPT_POSTQUOTE:
						name = "CURLOPT_POSTQUOTE";
						break;
					case CURLOPT_PREQUOTE:
						name = "CURLOPT_PREQUOTE";
						break;
					case CURLOPT_TELNETOPTIONS:
						name = "CURLOPT_TELNETOPTIONS";
						break;
					case CURLOPT_MAIL_RCPT:
						name = "CURLOPT_MAIL_RCPT";
						break;
					case CURLOPT_RESOLVE:
						name = "CURLOPT_RESOLVE";
						break;
					case CURLOPT_PROXYHEADER:
						name = "CURLOPT_PROXYHEADER";
						break;
					case CURLOPT_CONNECT_TO:
						name = "CURLOPT_CONNECT_TO";
						break;
				}

				zend_type_error("%s(): The %s option must have an array value", get_active_function_name(), name);
				return FAILURE;
			}

			ph = Z_ARRVAL_P(zvalue);
			ZEND_HASH_FOREACH_VAL(ph, current) {
				ZVAL_DEREF(current);
				val = zval_get_tmp_string(current, &tmp_val);
				slist = curl_slist_append(slist, ZSTR_VAL(val));
				zend_tmp_string_release(tmp_val);
				if (!slist) {
					php_error_docref(NULL, E_WARNING, "Could not build curl_slist");
					return FAILURE;
				}
			} ZEND_HASH_FOREACH_END();

			if (slist) {
				if ((*ch->clone) == 1) {
					zend_hash_index_update_ptr(ch->to_free->slist, option, slist);
				} else {
					zend_hash_next_index_insert_ptr(ch->to_free->slist, slist);
				}
			}

			error = curl_easy_setopt(ch->cp, option, slist);

			break;
		}

		case CURLOPT_BINARYTRANSFER:
			/* Do nothing, just backward compatibility */
			break;

		case CURLOPT_FOLLOWLOCATION:
			lval = zend_is_true(zvalue);
			error = curl_easy_setopt(ch->cp, option, lval);
			break;

		case CURLOPT_HEADERFUNCTION:
			if (!Z_ISUNDEF(ch->handlers.write_header->func_name)) {
				zval_ptr_dtor(&ch->handlers.write_header->func_name);
				ch->handlers.write_header->fci_cache = empty_fcall_info_cache;
			}
			ZVAL_COPY(&ch->handlers.write_header->func_name, zvalue);
			ch->handlers.write_header->method = PHP_CURL_USER;
			break;

		case CURLOPT_POSTFIELDS:
			if (Z_TYPE_P(zvalue) == IS_ARRAY) {
				if (zend_hash_num_elements(HASH_OF(zvalue)) == 0) {
					/* no need to build the mime structure for empty hashtables;
					   also works around https://github.com/curl/curl/issues/6455 */
					curl_easy_setopt(ch->cp, CURLOPT_POSTFIELDS, "");
					error = curl_easy_setopt(ch->cp, CURLOPT_POSTFIELDSIZE, 0);
				} else {
					return build_mime_structure_from_hash(ch, zvalue);
				}
			} else {
				zend_string *tmp_str;
				zend_string *str = zval_get_tmp_string(zvalue, &tmp_str);
				/* with curl 7.17.0 and later, we can use COPYPOSTFIELDS, but we have to provide size before */
				error = curl_easy_setopt(ch->cp, CURLOPT_POSTFIELDSIZE, ZSTR_LEN(str));
				error = curl_easy_setopt(ch->cp, CURLOPT_COPYPOSTFIELDS, ZSTR_VAL(str));
				zend_tmp_string_release(tmp_str);
			}
			break;

		case CURLOPT_PROGRESSFUNCTION:
			curl_easy_setopt(ch->cp, CURLOPT_PROGRESSFUNCTION,	curl_progress);
			curl_easy_setopt(ch->cp, CURLOPT_PROGRESSDATA, ch);
			if (ch->handlers.progress == NULL) {
				ch->handlers.progress = ecalloc(1, sizeof(php_curl_callback));
			} else if (!Z_ISUNDEF(ch->handlers.progress->func_name)) {
				zval_ptr_dtor(&ch->handlers.progress->func_name);
				ch->handlers.progress->fci_cache = empty_fcall_info_cache;
			}
			ZVAL_COPY(&ch->handlers.progress->func_name, zvalue);
			break;

#if LIBCURL_VERSION_NUM >= 0x075400 /* Available since 7.84.0 */
		case CURLOPT_SSH_HOSTKEYFUNCTION:
			curl_easy_setopt(ch->cp, CURLOPT_SSH_HOSTKEYFUNCTION, curl_ssh_hostkeyfunction);
			curl_easy_setopt(ch->cp, CURLOPT_SSH_HOSTKEYDATA, ch);
			if (ch->handlers.sshhostkey == NULL) {
				ch->handlers.sshhostkey = ecalloc(1, sizeof(php_curl_callback));
			} else if (!Z_ISUNDEF(ch->handlers.sshhostkey->func_name)) {
				zval_ptr_dtor(&ch->handlers.sshhostkey->func_name);
				ch->handlers.sshhostkey->fci_cache = empty_fcall_info_cache;
			}
			ZVAL_COPY(&ch->handlers.sshhostkey->func_name, zvalue);
			break;
#endif

		case CURLOPT_READFUNCTION:
			if (!Z_ISUNDEF(ch->handlers.read->func_name)) {
				zval_ptr_dtor(&ch->handlers.read->func_name);
				ch->handlers.read->fci_cache = empty_fcall_info_cache;
			}
			ZVAL_COPY(&ch->handlers.read->func_name, zvalue);
			ch->handlers.read->method = PHP_CURL_USER;
			break;

		case CURLOPT_RETURNTRANSFER:
			if (zend_is_true(zvalue)) {
				ch->handlers.write->method = PHP_CURL_RETURN;
			} else {
				ch->handlers.write->method = PHP_CURL_STDOUT;
			}
			break;

		case CURLOPT_WRITEFUNCTION:
			if (!Z_ISUNDEF(ch->handlers.write->func_name)) {
				zval_ptr_dtor(&ch->handlers.write->func_name);
				ch->handlers.write->fci_cache = empty_fcall_info_cache;
			}
			ZVAL_COPY(&ch->handlers.write->func_name, zvalue);
			ch->handlers.write->method = PHP_CURL_USER;
			break;

		case CURLOPT_XFERINFOFUNCTION:
			curl_easy_setopt(ch->cp, CURLOPT_XFERINFOFUNCTION,	curl_xferinfo);
			curl_easy_setopt(ch->cp, CURLOPT_XFERINFODATA, ch);
			if (ch->handlers.xferinfo == NULL) {
				ch->handlers.xferinfo = ecalloc(1, sizeof(php_curl_callback));
			} else if (!Z_ISUNDEF(ch->handlers.xferinfo->func_name)) {
				zval_ptr_dtor(&ch->handlers.xferinfo->func_name);
				ch->handlers.xferinfo->fci_cache = empty_fcall_info_cache;
			}
			ZVAL_COPY(&ch->handlers.xferinfo->func_name, zvalue);
			break;

		/* Curl off_t options */
		case CURLOPT_MAX_RECV_SPEED_LARGE:
		case CURLOPT_MAX_SEND_SPEED_LARGE:
		case CURLOPT_MAXFILESIZE_LARGE:
		case CURLOPT_TIMEVALUE_LARGE:
			lval = zval_get_long(zvalue);
			error = curl_easy_setopt(ch->cp, option, (curl_off_t)lval);
			break;

		case CURLOPT_POSTREDIR:
			lval = zval_get_long(zvalue);
			error = curl_easy_setopt(ch->cp, CURLOPT_POSTREDIR, lval & CURL_REDIR_POST_ALL);
			break;

		/* the following options deal with files, therefore the open_basedir check
		 * is required.
		 */
		case CURLOPT_COOKIEFILE:
		case CURLOPT_COOKIEJAR:
		case CURLOPT_RANDOM_FILE:
		case CURLOPT_SSLCERT:
		case CURLOPT_NETRC_FILE:
		case CURLOPT_SSH_PRIVATE_KEYFILE:
		case CURLOPT_SSH_PUBLIC_KEYFILE:
		case CURLOPT_CRLFILE:
		case CURLOPT_ISSUERCERT:
		case CURLOPT_SSH_KNOWNHOSTS:
		{
		    zend_string *tmp_str;
			zend_string *str = zval_get_tmp_string(zvalue, &tmp_str);
			zend_result ret;

			if (ZSTR_LEN(str) && php_check_open_basedir(ZSTR_VAL(str))) {
				zend_tmp_string_release(tmp_str);
				return FAILURE;
			}

			ret = php_curl_option_str(ch, option, ZSTR_VAL(str), ZSTR_LEN(str));
			zend_tmp_string_release(tmp_str);
			return ret;
		}

		case CURLINFO_HEADER_OUT:
			if (zend_is_true(zvalue)) {
				curl_easy_setopt(ch->cp, CURLOPT_DEBUGFUNCTION, curl_debug);
				curl_easy_setopt(ch->cp, CURLOPT_DEBUGDATA, (void *)ch);
				curl_easy_setopt(ch->cp, CURLOPT_VERBOSE, 1);
			} else {
				curl_easy_setopt(ch->cp, CURLOPT_DEBUGFUNCTION, NULL);
				curl_easy_setopt(ch->cp, CURLOPT_DEBUGDATA, NULL);
				curl_easy_setopt(ch->cp, CURLOPT_VERBOSE, 0);
			}
			break;

		case CURLOPT_SHARE:
			{
				if (Z_TYPE_P(zvalue) == IS_OBJECT && Z_OBJCE_P(zvalue) == curl_share_ce) {
					php_curlsh *sh = Z_CURL_SHARE_P(zvalue);
					curl_easy_setopt(ch->cp, CURLOPT_SHARE, sh->share);

					if (ch->share) {
						OBJ_RELEASE(&ch->share->std);
					}
					GC_ADDREF(&sh->std);
					ch->share = sh;
				}
			}
			break;

		case CURLOPT_FNMATCH_FUNCTION:
			curl_easy_setopt(ch->cp, CURLOPT_FNMATCH_FUNCTION, curl_fnmatch);
			curl_easy_setopt(ch->cp, CURLOPT_FNMATCH_DATA, ch);
			if (ch->handlers.fnmatch == NULL) {
				ch->handlers.fnmatch = ecalloc(1, sizeof(php_curl_callback));
			} else if (!Z_ISUNDEF(ch->handlers.fnmatch->func_name)) {
				zval_ptr_dtor(&ch->handlers.fnmatch->func_name);
				ch->handlers.fnmatch->fci_cache = empty_fcall_info_cache;
			}
			ZVAL_COPY(&ch->handlers.fnmatch->func_name, zvalue);
			break;

		/* Curl blob options */
#if LIBCURL_VERSION_NUM >= 0x074700 /* Available since 7.71.0 */
		case CURLOPT_ISSUERCERT_BLOB:
		case CURLOPT_PROXY_ISSUERCERT_BLOB:
		case CURLOPT_PROXY_SSLCERT_BLOB:
		case CURLOPT_PROXY_SSLKEY_BLOB:
		case CURLOPT_SSLCERT_BLOB:
		case CURLOPT_SSLKEY_BLOB:
#if LIBCURL_VERSION_NUM >= 0x074d00 /* Available since 7.77.0 */
		case CURLOPT_CAINFO_BLOB:
		case CURLOPT_PROXY_CAINFO_BLOB:
#endif
			{
				zend_string *tmp_str;
				zend_string *str = zval_get_tmp_string(zvalue, &tmp_str);

				struct curl_blob stblob;
				stblob.data = ZSTR_VAL(str);
				stblob.len = ZSTR_LEN(str);
				stblob.flags = CURL_BLOB_COPY;
				error = curl_easy_setopt(ch->cp, option, &stblob);

				zend_tmp_string_release(tmp_str);
			}
			break;
#endif

		default:
			if (is_array_config) {
				zend_argument_value_error(2, "must contain only valid cURL options");
			} else {
				zend_argument_value_error(2, "is not a valid cURL option");
			}
			error = CURLE_UNKNOWN_OPTION;
			break;
	}

	SAVE_CURL_ERROR(ch, error);
	if (error != CURLE_OK) {
		return FAILURE;
	} else {
		return SUCCESS;
	}
}
/* }}} */

/* {{{ Set an option for a cURL transfer */
PHP_FUNCTION(curl_setopt)
{
	zval       *zid, *zvalue;
	zend_long        options;
	php_curl   *ch;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_OBJECT_OF_CLASS(zid, curl_ce)
		Z_PARAM_LONG(options)
		Z_PARAM_ZVAL(zvalue)
	ZEND_PARSE_PARAMETERS_END();

	ch = Z_CURL_P(zid);

	if (_php_curl_setopt(ch, options, zvalue, 0) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

PHP_METHOD(CurlHandle, setOpt)
{
	zval       *zvalue;
	zend_long   options;
	php_curl   *ch = Z_CURL_P(getThis());

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(options)
		Z_PARAM_ZVAL(zvalue)
	ZEND_PARSE_PARAMETERS_END();

	if (_php_curl_setopt(ch, options, zvalue, 0) == SUCCESS) {
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		if (!EG(exception)) {
			curl_throw_last_error(ch, "Unknown cURL setopt error");
		}
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Set an array of option for a cURL transfer */
PHP_FUNCTION(curl_setopt_array)
{
	zval		*zid, *arr, *entry;
	php_curl	*ch;
	zend_ulong	option;
	zend_string	*string_key;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_OBJECT_OF_CLASS(zid, curl_ce)
		Z_PARAM_ARRAY(arr)
	ZEND_PARSE_PARAMETERS_END();

	ch = Z_CURL_P(zid);

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(arr), option, string_key, entry) {
		if (string_key) {
			zend_argument_value_error(2, "contains an invalid cURL option");
			RETURN_THROWS();
		}

		ZVAL_DEREF(entry);
		if (_php_curl_setopt(ch, (zend_long) option, entry, 1) == FAILURE) {
			RETURN_FALSE;
		}
	} ZEND_HASH_FOREACH_END();

	RETURN_TRUE;
}

PHP_METHOD(CurlHandle, setOptArray)
{
	zval		*arr, *entry;
	php_curl	*ch = Z_CURL_P(getThis());
	zend_ulong	option;
	zend_string	*string_key;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(arr)
	ZEND_PARSE_PARAMETERS_END();

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(arr), option, string_key, entry) {
		if (string_key) {
			zend_argument_value_error(2, "contains an invalid cURL option");
			RETURN_THROWS();
		}

		ZVAL_DEREF(entry);
		if (_php_curl_setopt(ch, (zend_long) option, entry, 1) == FAILURE) {
			if (!EG(exception)) {
				curl_throw_last_error(ch, "Unknown cURL setopt error");
			}
			RETURN_THROWS();
		}
	} ZEND_HASH_FOREACH_END();

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/* {{{ _php_curl_cleanup_handle(ch)
   Cleanup an execution phase */
void _php_curl_cleanup_handle(php_curl *ch)
{
	smart_str_free(&ch->handlers.write->buf);
	if (ch->header.str) {
		zend_string_release_ex(ch->header.str, 0);
		ch->header.str = NULL;
	}

	memset(ch->err.str, 0, CURL_ERROR_SIZE + 1);
	ch->err.no = 0;
}
/* }}} */

/* {{{ Perform a cURL session */
static zend_result php_curl_exec(zval *return_value, php_curl *ch)
{
	CURLcode	error;

	_php_curl_verify_handlers(ch, /* reporterror */ true);
	_php_curl_cleanup_handle(ch);

	error = curl_easy_perform(ch->cp);
	SAVE_CURL_ERROR(ch, error);

	if (error != CURLE_OK) {
		smart_str_free(&ch->handlers.write->buf);
		return FAILURE;
	}

	if (!Z_ISUNDEF(ch->handlers.std_err)) {
		php_stream  *stream;
		stream = (php_stream*)zend_fetch_resource2_ex(&ch->handlers.std_err, NULL, php_file_le_stream(), php_file_le_pstream());
		if (stream) {
			php_stream_flush(stream);
		}
	}

	if (ch->handlers.write->method == PHP_CURL_RETURN && ch->handlers.write->buf.s) {
		smart_str_0(&ch->handlers.write->buf);
		RETVAL_STR_COPY(ch->handlers.write->buf.s);
		return SUCCESS;
	}

	/* flush the file handle, so any remaining data is synched to disk */
	if (ch->handlers.write->method == PHP_CURL_FILE && ch->handlers.write->fp) {
		fflush(ch->handlers.write->fp);
	}
	if (ch->handlers.write_header->method == PHP_CURL_FILE && ch->handlers.write_header->fp) {
		fflush(ch->handlers.write_header->fp);
	}

	if (ch->handlers.write->method == PHP_CURL_RETURN) {
		RETVAL_EMPTY_STRING();
	} else {
		RETVAL_TRUE;
	}

	return SUCCESS;
}

PHP_FUNCTION(curl_exec)
{
	zval *zid;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(zid, curl_ce)
	ZEND_PARSE_PARAMETERS_END();

	if (php_curl_exec(return_value, Z_CURL_P(zid)) == FAILURE) {
		RETURN_FALSE;
	}
}

PHP_METHOD(CurlHandle, exec)
{
	php_curl *ch = Z_CURL_P(getThis());

	ZEND_PARSE_PARAMETERS_NONE();

	if (php_curl_exec(return_value, ch) == FAILURE) {
		curl_throw_last_error(ch, "Failed executing cURL request");
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Get information regarding a specific transfer */
PHP_FUNCTION(curl_getinfo)
{
	zval		*zid;
	php_curl	*ch;
	zend_long	option;
	bool option_is_null = 1;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O|l!", &zid, curl_ce, &option, &option_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	ch = Z_CURL_P(zid);

	if (option_is_null) {
		char *s_code;
		/* libcurl expects long datatype. So far no cases are known where
		   it would be an issue. Using zend_long would truncate a 64-bit
		   var on Win64, so the exact long datatype fits everywhere, as
		   long as there's no 32-bit int overflow. */
		long l_code;
		double d_code;
		struct curl_certinfo *ci = NULL;
		zval listcode;
		curl_off_t co;

		array_init(return_value);

		if (curl_easy_getinfo(ch->cp, CURLINFO_EFFECTIVE_URL, &s_code) == CURLE_OK) {
			CAAS("url", s_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_CONTENT_TYPE, &s_code) == CURLE_OK) {
			if (s_code != NULL) {
				CAAS("content_type", s_code);
			} else {
				zval retnull;
				ZVAL_NULL(&retnull);
				CAAZ("content_type", &retnull);
			}
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_HTTP_CODE, &l_code) == CURLE_OK) {
			CAAL("http_code", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_HEADER_SIZE, &l_code) == CURLE_OK) {
			CAAL("header_size", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_REQUEST_SIZE, &l_code) == CURLE_OK) {
			CAAL("request_size", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_FILETIME, &l_code) == CURLE_OK) {
			CAAL("filetime", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_SSL_VERIFYRESULT, &l_code) == CURLE_OK) {
			CAAL("ssl_verify_result", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_REDIRECT_COUNT, &l_code) == CURLE_OK) {
			CAAL("redirect_count", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_TOTAL_TIME, &d_code) == CURLE_OK) {
			CAAD("total_time", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_NAMELOOKUP_TIME, &d_code) == CURLE_OK) {
			CAAD("namelookup_time", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_CONNECT_TIME, &d_code) == CURLE_OK) {
			CAAD("connect_time", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_PRETRANSFER_TIME, &d_code) == CURLE_OK) {
			CAAD("pretransfer_time", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_SIZE_UPLOAD, &d_code) == CURLE_OK) {
			CAAD("size_upload", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_SIZE_DOWNLOAD, &d_code) == CURLE_OK) {
			CAAD("size_download", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_SPEED_DOWNLOAD, &d_code) == CURLE_OK) {
			CAAD("speed_download", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_SPEED_UPLOAD, &d_code) == CURLE_OK) {
			CAAD("speed_upload", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &d_code) == CURLE_OK) {
			CAAD("download_content_length", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_CONTENT_LENGTH_UPLOAD, &d_code) == CURLE_OK) {
			CAAD("upload_content_length", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_STARTTRANSFER_TIME, &d_code) == CURLE_OK) {
			CAAD("starttransfer_time", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_REDIRECT_TIME, &d_code) == CURLE_OK) {
			CAAD("redirect_time", d_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_REDIRECT_URL, &s_code) == CURLE_OK) {
			CAAS("redirect_url", s_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_PRIMARY_IP, &s_code) == CURLE_OK) {
			CAAS("primary_ip", s_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_CERTINFO, &ci) == CURLE_OK) {
			array_init(&listcode);
			create_certinfo(ci, &listcode);
			CAAZ("certinfo", &listcode);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_PRIMARY_PORT, &l_code) == CURLE_OK) {
			CAAL("primary_port", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_LOCAL_IP, &s_code) == CURLE_OK) {
			CAAS("local_ip", s_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_LOCAL_PORT, &l_code) == CURLE_OK) {
			CAAL("local_port", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_HTTP_VERSION, &l_code) == CURLE_OK) {
			CAAL("http_version", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_PROTOCOL, &l_code) == CURLE_OK) {
			CAAL("protocol", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_PROXY_SSL_VERIFYRESULT, &l_code) == CURLE_OK) {
			CAAL("ssl_verifyresult", l_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_SCHEME, &s_code) == CURLE_OK) {
			CAAS("scheme", s_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_APPCONNECT_TIME_T, &co) == CURLE_OK) {
			CAAL("appconnect_time_us", co);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_CONNECT_TIME_T, &co) == CURLE_OK) {
			CAAL("connect_time_us", co);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_NAMELOOKUP_TIME_T, &co) == CURLE_OK) {
			CAAL("namelookup_time_us", co);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_PRETRANSFER_TIME_T, &co) == CURLE_OK) {
			CAAL("pretransfer_time_us", co);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_REDIRECT_TIME_T, &co) == CURLE_OK) {
			CAAL("redirect_time_us", co);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_STARTTRANSFER_TIME_T, &co) == CURLE_OK) {
			CAAL("starttransfer_time_us", co);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_TOTAL_TIME_T, &co) == CURLE_OK) {
			CAAL("total_time_us", co);
		}
		if (ch->header.str) {
			CAASTR("request_header", ch->header.str);
		}
#if LIBCURL_VERSION_NUM >= 0x074800 /* Available since 7.72.0 */
		if (curl_easy_getinfo(ch->cp, CURLINFO_EFFECTIVE_METHOD, &s_code) == CURLE_OK) {
			CAAS("effective_method", s_code);
		}
#endif
#if LIBCURL_VERSION_NUM >= 0x075400 /* Available since 7.84.0 */
		if (curl_easy_getinfo(ch->cp, CURLINFO_CAPATH, &s_code) == CURLE_OK) {
			CAAS("capath", s_code);
		}
		if (curl_easy_getinfo(ch->cp, CURLINFO_CAINFO, &s_code) == CURLE_OK) {
			CAAS("cainfo", s_code);
		}
#endif
	} else {
		switch (option) {
			case CURLINFO_HEADER_OUT:
				if (ch->header.str) {
					RETURN_STR_COPY(ch->header.str);
				} else {
					RETURN_FALSE;
				}
			case CURLINFO_CERTINFO: {
				struct curl_certinfo *ci = NULL;

				array_init(return_value);

				if (curl_easy_getinfo(ch->cp, CURLINFO_CERTINFO, &ci) == CURLE_OK) {
					create_certinfo(ci, return_value);
				} else {
					RETURN_FALSE;
				}
				break;
			}
			case CURLINFO_PRIVATE:
				if (!Z_ISUNDEF(ch->private_data)) {
					RETURN_COPY(&ch->private_data);
				} else {
					RETURN_FALSE;
				}
				break;
			default: {
				int type = CURLINFO_TYPEMASK & option;
				switch (type) {
					case CURLINFO_STRING:
					{
						char *s_code = NULL;

						if (curl_easy_getinfo(ch->cp, option, &s_code) == CURLE_OK && s_code) {
							RETURN_STRING(s_code);
						} else {
							RETURN_FALSE;
						}
						break;
					}
					case CURLINFO_LONG:
					{
						zend_long code = 0;

						if (curl_easy_getinfo(ch->cp, option, &code) == CURLE_OK) {
							RETURN_LONG(code);
						} else {
							RETURN_FALSE;
						}
						break;
					}
					case CURLINFO_DOUBLE:
					{
						double code = 0.0;

						if (curl_easy_getinfo(ch->cp, option, &code) == CURLE_OK) {
							RETURN_DOUBLE(code);
						} else {
							RETURN_FALSE;
						}
						break;
					}
					case CURLINFO_SLIST:
					{
						struct curl_slist *slist;
						if (curl_easy_getinfo(ch->cp, option, &slist) == CURLE_OK) {
							struct curl_slist *current = slist;
							array_init(return_value);
							while (current) {
								add_next_index_string(return_value, current->data);
								current = current->next;
							}
							curl_slist_free_all(slist);
						} else {
							RETURN_FALSE;
						}
						break;
					}
					case CURLINFO_OFF_T:
					{
						curl_off_t c_off;
						if (curl_easy_getinfo(ch->cp, option, &c_off) == CURLE_OK) {
							RETURN_LONG((long) c_off);
						} else {
							RETURN_FALSE;
						}
						break;
					}
					default:
						RETURN_FALSE;
				}
			}
		}
	}
}
/* }}} */

/* {{{ Return a string contain the last error for the current session */
PHP_FUNCTION(curl_error)
{
	zval		*zid;
	php_curl	*ch;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zid, curl_ce) == FAILURE) {
		RETURN_THROWS();
	}

	ch = Z_CURL_P(zid);

	if (ch->err.no) {
		ch->err.str[CURL_ERROR_SIZE] = 0;
		RETURN_STRING(ch->err.str);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ Return an integer containing the last error number */
PHP_FUNCTION(curl_errno)
{
	zval		*zid;
	php_curl	*ch;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zid, curl_ce) == FAILURE) {
		RETURN_THROWS();
	}

	ch = Z_CURL_P(zid);

	RETURN_LONG(ch->err.no);
}
/* }}} */

/* {{{ Close a cURL session */
PHP_FUNCTION(curl_close)
{
	zval		*zid;
	php_curl	*ch;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(zid, curl_ce)
	ZEND_PARSE_PARAMETERS_END();

	ch = Z_CURL_P(zid);

	if (ch->in_callback) {
		zend_throw_error(NULL, "%s(): Attempt to close cURL handle from a callback", get_active_function_name());
		RETURN_THROWS();
	}
}
/* }}} */

static void _php_curl_free_callback(php_curl_callback* callback)
{
	if (callback) {
		zval_ptr_dtor(&callback->func_name);
		efree(callback);
	}
}

static void curl_free_obj(zend_object *object)
{
	php_curl *ch = curl_from_obj(object);

#if PHP_CURL_DEBUG
	fprintf(stderr, "DTOR CALLED, ch = %x\n", ch);
#endif

	_php_curl_verify_handlers(ch, /* reporterror */ false);

	/*
	 * Libcurl is doing connection caching. When easy handle is cleaned up,
	 * if the handle was previously used by the curl_multi_api, the connection
	 * remains open un the curl multi handle is cleaned up. Some protocols are
	 * sending content like the FTP one, and libcurl try to use the
	 * WRITEFUNCTION or the HEADERFUNCTION. Since structures used in those
	 * callback are freed, we need to use an other callback to which avoid
	 * segfaults.
	 *
	 * Libcurl commit d021f2e8a00 fix this issue and should be part of 7.28.2
	 */
	if (ch->cp) {
		curl_easy_setopt(ch->cp, CURLOPT_HEADERFUNCTION, curl_write_nothing);
		curl_easy_setopt(ch->cp, CURLOPT_WRITEFUNCTION, curl_write_nothing);

		curl_easy_cleanup(ch->cp);
	}

	/* cURL destructors should be invoked only by last curl handle */
	if (--(*ch->clone) == 0) {
		zend_llist_clean(&ch->to_free->post);
		zend_llist_clean(&ch->to_free->stream);

		zend_hash_destroy(ch->to_free->slist);
		efree(ch->to_free->slist);
		efree(ch->to_free);
		efree(ch->clone);
	}

	smart_str_free(&ch->handlers.write->buf);
	zval_ptr_dtor(&ch->handlers.write->func_name);
	zval_ptr_dtor(&ch->handlers.read->func_name);
	zval_ptr_dtor(&ch->handlers.write_header->func_name);
	zval_ptr_dtor(&ch->handlers.std_err);
	if (ch->header.str) {
		zend_string_release_ex(ch->header.str, 0);
	}

	zval_ptr_dtor(&ch->handlers.write_header->stream);
	zval_ptr_dtor(&ch->handlers.write->stream);
	zval_ptr_dtor(&ch->handlers.read->stream);

	efree(ch->handlers.write);
	efree(ch->handlers.write_header);
	efree(ch->handlers.read);

	_php_curl_free_callback(ch->handlers.progress);
	_php_curl_free_callback(ch->handlers.xferinfo);
	_php_curl_free_callback(ch->handlers.fnmatch);
#if LIBCURL_VERSION_NUM >= 0x075400 /* Available since 7.84.0 */
	_php_curl_free_callback(ch->handlers.sshhostkey);
#endif

	zval_ptr_dtor(&ch->postfields);
	zval_ptr_dtor(&ch->private_data);

	if (ch->share) {
		OBJ_RELEASE(&ch->share->std);
	}

	zend_object_std_dtor(&ch->std);
}
/* }}} */

/* {{{ return string describing error code */
PHP_FUNCTION(curl_strerror)
{
	zend_long code;
	const char *str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(code)
	ZEND_PARSE_PARAMETERS_END();

	str = curl_easy_strerror(code);
	if (str) {
		RETURN_STRING(str);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ _php_curl_reset_handlers()
   Reset all handlers of a given php_curl */
static void _php_curl_reset_handlers(php_curl *ch)
{
	if (!Z_ISUNDEF(ch->handlers.write->stream)) {
		zval_ptr_dtor(&ch->handlers.write->stream);
		ZVAL_UNDEF(&ch->handlers.write->stream);
	}
	ch->handlers.write->fp = NULL;
	ch->handlers.write->method = PHP_CURL_STDOUT;

	if (!Z_ISUNDEF(ch->handlers.write_header->stream)) {
		zval_ptr_dtor(&ch->handlers.write_header->stream);
		ZVAL_UNDEF(&ch->handlers.write_header->stream);
	}
	ch->handlers.write_header->fp = NULL;
	ch->handlers.write_header->method = PHP_CURL_IGNORE;

	if (!Z_ISUNDEF(ch->handlers.read->stream)) {
		zval_ptr_dtor(&ch->handlers.read->stream);
		ZVAL_UNDEF(&ch->handlers.read->stream);
	}
	ch->handlers.read->fp = NULL;
	ch->handlers.read->res = NULL;
	ch->handlers.read->method  = PHP_CURL_DIRECT;

	if (!Z_ISUNDEF(ch->handlers.std_err)) {
		zval_ptr_dtor(&ch->handlers.std_err);
		ZVAL_UNDEF(&ch->handlers.std_err);
	}

	if (ch->handlers.progress) {
		zval_ptr_dtor(&ch->handlers.progress->func_name);
		efree(ch->handlers.progress);
		ch->handlers.progress = NULL;
	}

	if (ch->handlers.xferinfo) {
		zval_ptr_dtor(&ch->handlers.xferinfo->func_name);
		efree(ch->handlers.xferinfo);
		ch->handlers.xferinfo = NULL;
	}

	if (ch->handlers.fnmatch) {
		zval_ptr_dtor(&ch->handlers.fnmatch->func_name);
		efree(ch->handlers.fnmatch);
		ch->handlers.fnmatch = NULL;
	}

#if LIBCURL_VERSION_NUM >= 0x075400 /* Available since 7.84.0 */
	if (ch->handlers.sshhostkey) {
		zval_ptr_dtor(&ch->handlers.sshhostkey->func_name);
		efree(ch->handlers.sshhostkey);
		ch->handlers.sshhostkey = NULL;
	}
#endif
}
/* }}} */

/* {{{ Reset all options of a libcurl session handle */
PHP_FUNCTION(curl_reset)
{
	zval       *zid;
	php_curl   *ch;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zid, curl_ce) == FAILURE) {
		RETURN_THROWS();
	}

	ch = Z_CURL_P(zid);

	if (ch->in_callback) {
		zend_throw_error(NULL, "%s(): Attempt to reset cURL handle from a callback", get_active_function_name());
		RETURN_THROWS();
	}

	curl_easy_reset(ch->cp);
	_php_curl_reset_handlers(ch);
	_php_curl_set_default_options(ch);
}
/* }}} */

/* {{{ URL encodes the given string */
PHP_FUNCTION(curl_escape)
{
	zend_string *str;
	char        *res;
	zval        *zid;
	php_curl    *ch;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OS", &zid, curl_ce, &str) == FAILURE) {
		RETURN_THROWS();
	}

	ch = Z_CURL_P(zid);

	if (ZEND_SIZE_T_INT_OVFL(ZSTR_LEN(str))) {
		RETURN_FALSE;
	}

	if ((res = curl_easy_escape(ch->cp, ZSTR_VAL(str), ZSTR_LEN(str)))) {
		RETVAL_STRING(res);
		curl_free(res);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ URL decodes the given string */
PHP_FUNCTION(curl_unescape)
{
	char        *out = NULL;
	int          out_len;
	zval        *zid;
	zend_string *str;
	php_curl    *ch;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OS", &zid, curl_ce, &str) == FAILURE) {
		RETURN_THROWS();
	}

	ch = Z_CURL_P(zid);

	if (ZEND_SIZE_T_INT_OVFL(ZSTR_LEN(str))) {
		RETURN_FALSE;
	}

	if ((out = curl_easy_unescape(ch->cp, ZSTR_VAL(str), ZSTR_LEN(str), &out_len))) {
		RETVAL_STRINGL(out, out_len);
		curl_free(out);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ pause and unpause a connection */
PHP_FUNCTION(curl_pause)
{
	zend_long       bitmask;
	zval       *zid;
	php_curl   *ch;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Ol", &zid, curl_ce, &bitmask) == FAILURE) {
		RETURN_THROWS();
	}

	ch = Z_CURL_P(zid);

	RETURN_LONG(curl_easy_pause(ch->cp, bitmask));
}
/* }}} */

#if LIBCURL_VERSION_NUM >= 0x073E00 /* Available since 7.62.0 */
/* {{{ perform connection upkeep checks */
PHP_FUNCTION(curl_upkeep)
{
	CURLcode	error;
	zval		*zid;
	php_curl	*ch;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "O", &zid, curl_ce) == FAILURE) {
		RETURN_THROWS();
	}

	ch = Z_CURL_P(zid);

	error = curl_easy_upkeep(ch->cp);
	SAVE_CURL_ERROR(ch, error);

	RETURN_BOOL(error == CURLE_OK);
}
/*}}} */
#endif
