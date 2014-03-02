/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "ext/standard/file.h"
#include "ext/standard/url.h"
#include "streams/php_streams_int.h"
#include "ext/standard/php_smart_str.h"
#include "php_openssl.h"
#include "php_openssl_structs.h"
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/err.h>

#ifdef PHP_WIN32
#include "win32/time.h"
#endif

#ifdef NETWARE
#include <sys/select.h>
#endif

#if !defined(OPENSSL_NO_ECDH) && OPENSSL_VERSION_NUMBER >= 0x0090800fL
#define HAVE_ECDH
#endif

/* Flags for determining allowed stream crypto methods */
#define STREAM_CRYPTO_IS_CLIENT            (1<<0)
#define STREAM_CRYPTO_METHOD_SSLv2         (1<<1)
#define STREAM_CRYPTO_METHOD_SSLv3         (1<<2)
#define STREAM_CRYPTO_METHOD_TLSv1_0       (1<<3)
#define STREAM_CRYPTO_METHOD_TLSv1_1       (1<<4)
#define STREAM_CRYPTO_METHOD_TLSv1_2       (1<<5)

int php_openssl_apply_verification_policy(SSL *ssl, X509 *peer, php_stream *stream TSRMLS_DC);
SSL *php_SSL_new_from_context(SSL_CTX *ctx, php_stream *stream TSRMLS_DC);
extern php_stream* php_openssl_get_stream_from_ssl_handle(const SSL *ssl);
int php_openssl_get_x509_list_id(void);

php_stream_ops php_openssl_socket_ops;

/* it doesn't matter that we do some hash traversal here, since it is done only
 * in an error condition arising from a network connection problem */
static int is_http_stream_talking_to_iis(php_stream *stream TSRMLS_DC)
{
	if (stream->wrapperdata && stream->wrapper && strcasecmp(stream->wrapper->wops->label, "HTTP") == 0) {
		/* the wrapperdata is an array zval containing the headers */
		zval **tmp;

#define SERVER_MICROSOFT_IIS	"Server: Microsoft-IIS"
#define SERVER_GOOGLE "Server: GFE/"
		
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(stream->wrapperdata));
		while (SUCCESS == zend_hash_get_current_data(Z_ARRVAL_P(stream->wrapperdata), (void**)&tmp)) {

			if (strncasecmp(Z_STRVAL_PP(tmp), SERVER_MICROSOFT_IIS, sizeof(SERVER_MICROSOFT_IIS)-1) == 0) {
				return 1;
			} else if (strncasecmp(Z_STRVAL_PP(tmp), SERVER_GOOGLE, sizeof(SERVER_GOOGLE)-1) == 0) {
				return 1;
			}
			
			zend_hash_move_forward(Z_ARRVAL_P(stream->wrapperdata));
		}
	}
	return 0;
}

static int handle_ssl_error(php_stream *stream, int nr_bytes, zend_bool is_init TSRMLS_DC)
{
	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;
	int err = SSL_get_error(sslsock->ssl_handle, nr_bytes);
	char esbuf[512];
	smart_str ebuf = {0};
	unsigned long ecode;
	int retry = 1;

	switch(err) {
		case SSL_ERROR_ZERO_RETURN:
			/* SSL terminated (but socket may still be active) */
			retry = 0;
			break;
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
			/* re-negotiation, or perhaps the SSL layer needs more
			 * packets: retry in next iteration */
			errno = EAGAIN;
			retry = is_init ? 1 : sslsock->s.is_blocked;
			break;
		case SSL_ERROR_SYSCALL:
			if (ERR_peek_error() == 0) {
				if (nr_bytes == 0) {
					if (!is_http_stream_talking_to_iis(stream TSRMLS_CC) && ERR_get_error() != 0) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING,
								"SSL: fatal protocol error");
					}
					SSL_set_shutdown(sslsock->ssl_handle, SSL_SENT_SHUTDOWN|SSL_RECEIVED_SHUTDOWN);
					stream->eof = 1;
					retry = 0;
				} else {
					char *estr = php_socket_strerror(php_socket_errno(), NULL, 0);

					php_error_docref(NULL TSRMLS_CC, E_WARNING,
							"SSL: %s", estr);

					efree(estr);
					retry = 0;
				}
				break;
			}

			
			/* fall through */
		default:
			/* some other error */
			ecode = ERR_get_error();

			switch (ERR_GET_REASON(ecode)) {
				case SSL_R_NO_SHARED_CIPHER:
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "SSL_R_NO_SHARED_CIPHER: no suitable shared cipher could be used.  This could be because the server is missing an SSL certificate (local_cert context option)");
					retry = 0;
					break;

				default:
					do {
						/* NULL is automatically added */
						ERR_error_string_n(ecode, esbuf, sizeof(esbuf));
						if (ebuf.c) {
							smart_str_appendc(&ebuf, '\n');
						}
						smart_str_appends(&ebuf, esbuf);
					} while ((ecode = ERR_get_error()) != 0);

					smart_str_0(&ebuf);

					php_error_docref(NULL TSRMLS_CC, E_WARNING,
							"SSL operation failed with code %d. %s%s",
							err,
							ebuf.c ? "OpenSSL Error messages:\n" : "",
							ebuf.c ? ebuf.c : "");
					if (ebuf.c) {
						smart_str_free(&ebuf);
					}
			}
				
			retry = 0;
			errno = 0;
	}
	return retry;
}


static size_t php_openssl_sockop_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;
	int didwrite;
	
	if (sslsock->ssl_active) {
		int retry = 1;

		do {
			didwrite = SSL_write(sslsock->ssl_handle, buf, count);

			if (didwrite <= 0) {
				retry = handle_ssl_error(stream, didwrite, 0 TSRMLS_CC);
			} else {
				break;
			}
		} while(retry);

		if (didwrite > 0) {
			php_stream_notify_progress_increment(stream->context, didwrite, 0);
		}
	} else {
		didwrite = php_stream_socket_ops.write(stream, buf, count TSRMLS_CC);
	}

	if (didwrite < 0) {
		didwrite = 0;
	}
	
	return didwrite;
}

static size_t php_openssl_sockop_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;
	int nr_bytes = 0;

	if (sslsock->ssl_active) {
		int retry = 1;

		do {
			nr_bytes = SSL_read(sslsock->ssl_handle, buf, count);

			if (sslsock->reneg && sslsock->reneg->should_close) {
				/* renegotiation rate limiting triggered */
				php_stream_xport_shutdown(stream, (stream_shutdown_t)SHUT_RDWR TSRMLS_CC);
				nr_bytes = 0;
				stream->eof = 1;
				break;
			} else if (nr_bytes <= 0) {
				retry = handle_ssl_error(stream, nr_bytes, 0 TSRMLS_CC);
				stream->eof = (retry == 0 && errno != EAGAIN && !SSL_pending(sslsock->ssl_handle));
				
			} else {
				/* we got the data */
				break;
			}
		} while (retry);

		if (nr_bytes > 0) {
			php_stream_notify_progress_increment(stream->context, nr_bytes, 0);
		}
	}
	else
	{
		nr_bytes = php_stream_socket_ops.read(stream, buf, count TSRMLS_CC);
	}

	if (nr_bytes < 0) {
		nr_bytes = 0;
	}

	return nr_bytes;
}

static int php_openssl_sockop_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;
#ifdef PHP_WIN32
	int n;
#endif

	if (close_handle) {
		if (sslsock->ssl_active) {
			SSL_shutdown(sslsock->ssl_handle);
			sslsock->ssl_active = 0;
		}
		if (sslsock->ssl_handle) {
			SSL_free(sslsock->ssl_handle);
			sslsock->ssl_handle = NULL;
		}
		if (sslsock->ctx) {
			SSL_CTX_free(sslsock->ctx);
			sslsock->ctx = NULL;
		}
#ifdef PHP_WIN32
		if (sslsock->s.socket == -1)
			sslsock->s.socket = SOCK_ERR;
#endif
		if (sslsock->s.socket != SOCK_ERR) {
#ifdef PHP_WIN32
			/* prevent more data from coming in */
			shutdown(sslsock->s.socket, SHUT_RD);

			/* try to make sure that the OS sends all data before we close the connection.
			 * Essentially, we are waiting for the socket to become writeable, which means
			 * that all pending data has been sent.
			 * We use a small timeout which should encourage the OS to send the data,
			 * but at the same time avoid hanging indefinitely.
			 * */
			do {
				n = php_pollfd_for_ms(sslsock->s.socket, POLLOUT, 500);
			} while (n == -1 && php_socket_errno() == EINTR);
#endif
			closesocket(sslsock->s.socket);
			sslsock->s.socket = SOCK_ERR;
		}
	}

	if (sslsock->url_name) {
		pefree(sslsock->url_name, php_stream_is_persistent(stream));
	}

	if (sslsock->reneg) {
		pefree(sslsock->reneg, php_stream_is_persistent(stream));
	}

	pefree(sslsock, php_stream_is_persistent(stream));

	return 0;
}

static int php_openssl_sockop_flush(php_stream *stream TSRMLS_DC)
{
	return php_stream_socket_ops.flush(stream TSRMLS_CC);
}

static int php_openssl_sockop_stat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC)
{
	return php_stream_socket_ops.stat(stream, ssb TSRMLS_CC);
}

static inline void limit_handshake_reneg(const SSL *ssl) /* {{{ */
{
	php_stream *stream;
	php_openssl_netstream_data_t *sslsock;
	struct timeval now;
	long elapsed_time;

	stream = php_openssl_get_stream_from_ssl_handle(ssl);
	sslsock = (php_openssl_netstream_data_t*)stream->abstract;
	gettimeofday(&now, NULL);

	/* The initial handshake is never rate-limited */
	if (sslsock->reneg->prev_handshake == 0) {
		sslsock->reneg->prev_handshake = now.tv_sec;
		return;
	}

	elapsed_time = (now.tv_sec - sslsock->reneg->prev_handshake);
	sslsock->reneg->prev_handshake = now.tv_sec;
	sslsock->reneg->tokens -= (elapsed_time * (sslsock->reneg->limit / sslsock->reneg->window));

	if (sslsock->reneg->tokens < 0) {
		sslsock->reneg->tokens = 0;
	}
	++sslsock->reneg->tokens;

	/* The token level exceeds our allowed limit */
	if (sslsock->reneg->tokens > sslsock->reneg->limit) {
		zval **val;

		TSRMLS_FETCH();

		sslsock->reneg->should_close = 1;

		if (stream->context && SUCCESS == php_stream_context_get_option(stream->context,
				"ssl", "reneg_limit_callback", &val)
		) {
			zval *param, **params[1], *retval;

			MAKE_STD_ZVAL(param);
			php_stream_to_zval(stream, param);
			params[0] = &param;

			/* Closing the stream inside this callback would segfault! */
			stream->flags |= PHP_STREAM_FLAG_NO_FCLOSE;
			if (FAILURE == call_user_function_ex(EG(function_table), NULL, *val, &retval, 1, params, 0, NULL TSRMLS_CC)) {
				php_error(E_WARNING, "SSL: failed invoking reneg limit notification callback");
			}
			stream->flags ^= PHP_STREAM_FLAG_NO_FCLOSE;

			/* If the reneg_limit_callback returned true don't auto-close */
			if (retval != NULL && Z_TYPE_P(retval) == IS_BOOL && Z_BVAL_P(retval) == 1) {
				sslsock->reneg->should_close = 0;
			}

			FREE_ZVAL(param);
			if (retval != NULL) {
				zval_ptr_dtor(&retval);
			}
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
				"SSL: client-initiated handshake rate limit exceeded by peer");
		}
	}
}
/* }}} */

static void php_openssl_info_callback(const SSL *ssl, int where, int ret) /* {{{ */
{
        /* Rate-limit client-initiated handshake renegotiation to prevent DoS */
        if (where & SSL_CB_HANDSHAKE_START) {
                limit_handshake_reneg(ssl);
        }
}
/* }}} */

static inline void init_handshake_limiting(php_stream *stream, php_openssl_netstream_data_t *sslsock) /* {{{ */
{
	zval **val;
	long limit = DEFAULT_RENEG_LIMIT;
	long window = DEFAULT_RENEG_WINDOW;

	if (stream->context &&
		SUCCESS == php_stream_context_get_option(stream->context,
				"ssl", "reneg_limit", &val)
	) {
		convert_to_long(*val);
		limit = Z_LVAL_PP(val);
	}

	/* No renegotiation rate-limiting */
	if (limit < 0) {
		return;
	}

	if (stream->context &&
		SUCCESS == php_stream_context_get_option(stream->context,
				"ssl", "reneg_window", &val)
	) {
		convert_to_long(*val);
		window = Z_LVAL_PP(val);
	}

	sslsock->reneg = (void*)pemalloc(sizeof(php_openssl_handshake_bucket_t),
		php_stream_is_persistent(stream)
	);

	sslsock->reneg->limit = limit;
	sslsock->reneg->window = window;
	sslsock->reneg->prev_handshake = 0;
	sslsock->reneg->tokens = 0;
	sslsock->reneg->should_close = 0;

	SSL_CTX_set_info_callback(sslsock->ctx, php_openssl_info_callback);
}
/* }}} */

static const SSL_METHOD *php_select_crypto_method(long method_value, int is_client TSRMLS_DC)
{
	if (method_value == STREAM_CRYPTO_METHOD_SSLv2) {
#ifndef OPENSSL_NO_SSL2
		return is_client ? SSLv2_client_method() : SSLv2_server_method();
#else
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
				"SSLv2 support is not compiled into the OpenSSL library PHP is linked against");
		return NULL;
#endif
	} else if (method_value == STREAM_CRYPTO_METHOD_SSLv3) {
		return is_client ? SSLv3_client_method() : SSLv3_server_method();
	} else if (method_value == STREAM_CRYPTO_METHOD_TLSv1_0) {
		return is_client ? TLSv1_client_method() : TLSv1_server_method();
	} else if (method_value == STREAM_CRYPTO_METHOD_TLSv1_1) {
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
		return is_client ? TLSv1_1_client_method() : TLSv1_1_server_method();
#else
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
				"TLSv1.1 support is not compiled into the OpenSSL library PHP is linked against");
		return NULL;
#endif
	} else if (method_value == STREAM_CRYPTO_METHOD_TLSv1_2) {
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
		return is_client ? TLSv1_2_client_method() : TLSv1_2_server_method();
#else
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
				"TLSv1.2 support is not compiled into the OpenSSL library PHP is linked against");
		return NULL;
#endif
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
				"Invalid crypto method");
		return NULL;
	}
}

static long php_get_crypto_method_ctx_flags(long method_flags TSRMLS_DC)
{
	long ssl_ctx_options = SSL_OP_ALL;

#ifndef OPENSSL_NO_SSL2
	if (!(method_flags & STREAM_CRYPTO_METHOD_SSLv2)) {
		ssl_ctx_options |= SSL_OP_NO_SSLv2;
	}
#endif
#ifndef OPENSSL_NO_SSL3
	if (!(method_flags & STREAM_CRYPTO_METHOD_SSLv3)) {
		ssl_ctx_options |= SSL_OP_NO_SSLv3;
	}
#endif
#ifndef OPENSSL_NO_TLS1
	if (!(method_flags & STREAM_CRYPTO_METHOD_TLSv1_0)) {
		ssl_ctx_options |= SSL_OP_NO_TLSv1;
	}
#endif
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
	if (!(method_flags & STREAM_CRYPTO_METHOD_TLSv1_1)) {
		ssl_ctx_options |= SSL_OP_NO_TLSv1_1;
	}

	if (!(method_flags & STREAM_CRYPTO_METHOD_TLSv1_2)) {
		ssl_ctx_options |= SSL_OP_NO_TLSv1_2;
	}
#endif

	return ssl_ctx_options;
}

static inline int php_openssl_setup_crypto(php_stream *stream,
		php_openssl_netstream_data_t *sslsock,
		php_stream_xport_crypto_param *cparam
		TSRMLS_DC)
{
	const SSL_METHOD *method;
	long ssl_ctx_options;
	long method_flags;
	zval **val;
#ifdef SSL_MODE_RELEASE_BUFFERS
	long mode;
#endif

	if (sslsock->ssl_handle) {
		if (sslsock->s.is_blocked) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "SSL/TLS already set-up for this stream");
			return -1;
		} else {
			return 0;
		}
	}

	/* need to do slightly different things, based on client/server method,
	 * so lets remember which method was selected */
	sslsock->is_client = cparam->inputs.method & STREAM_CRYPTO_IS_CLIENT;
	method_flags = ((cparam->inputs.method >> 1) << 1);

	/* Should we use a specific crypto method or is generic SSLv23 okay? */
	if ((method_flags & (method_flags-1)) == 0) {
		ssl_ctx_options = SSL_OP_ALL;
		method = php_select_crypto_method(method_flags, sslsock->is_client TSRMLS_CC);
		if (method == NULL) {
			return -1;
		}
	} else {
		method = sslsock->is_client ? SSLv23_client_method() : SSLv23_server_method();
		ssl_ctx_options = php_get_crypto_method_ctx_flags(method_flags TSRMLS_CC);
		if (ssl_ctx_options == -1) {
			return -1;
		}
	}

#if OPENSSL_VERSION_NUMBER >= 0x10001001L
	sslsock->ctx = SSL_CTX_new(method);
#else
	sslsock->ctx = SSL_CTX_new((SSL_METHOD*)method);
#endif
	if (sslsock->ctx == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to create an SSL context");
		return -1;
	}

#if OPENSSL_VERSION_NUMBER >= 0x0090605fL
	ssl_ctx_options &= ~SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS;
#endif
	SSL_CTX_set_options(sslsock->ctx, ssl_ctx_options);

#if OPENSSL_VERSION_NUMBER >= 0x0090806fL
	{
		if (stream->context && SUCCESS == php_stream_context_get_option(
					stream->context, "ssl", "no_ticket", &val) && 
				zend_is_true(*val)
		) {
			SSL_CTX_set_options(sslsock->ctx, SSL_OP_NO_TICKET);
		}
	}
#endif

#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	{
		if (stream->context && (FAILURE == php_stream_context_get_option(
					stream->context, "ssl", "disable_compression", &val) ||
				zend_is_true(*val))
		) {
			SSL_CTX_set_options(sslsock->ctx, SSL_OP_NO_COMPRESSION);
		}
	}
#endif

	if (!sslsock->is_client && stream->context && SUCCESS == php_stream_context_get_option(
				stream->context, "ssl", "honor_cipher_order", &val) &&
			zend_is_true(*val)
	) {
		SSL_CTX_set_options(sslsock->ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);
	}

	if (!sslsock->is_client && stream->context && SUCCESS == php_stream_context_get_option(
				stream->context, "ssl", "single_dh_use", &val) &&
			zend_is_true(*val)
	) {
		SSL_CTX_set_options(sslsock->ctx, SSL_OP_SINGLE_DH_USE);
	}

	if (!sslsock->is_client && stream->context && SUCCESS == php_stream_context_get_option(
				stream->context, "ssl", "single_ecdh_use", &val) &&
			zend_is_true(*val)
	) {
		SSL_CTX_set_options(sslsock->ctx, SSL_OP_SINGLE_ECDH_USE);
	}

	sslsock->ssl_handle = php_SSL_new_from_context(sslsock->ctx, stream TSRMLS_CC);
	if (sslsock->ssl_handle == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to create an SSL handle");
		SSL_CTX_free(sslsock->ctx);
		sslsock->ctx = NULL;
		return -1;
	}

	if (!sslsock->is_client && stream->context && SUCCESS == php_stream_context_get_option(
				stream->context, "ssl", "honor_cipher_order", &val) &&
			zend_is_true(*val)
	) {
		SSL_CTX_set_options(sslsock->ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);
	}

#ifdef SSL_MODE_RELEASE_BUFFERS
	mode = SSL_get_mode(sslsock->ssl_handle);
	SSL_set_mode(sslsock->ssl_handle, mode | SSL_MODE_RELEASE_BUFFERS);
#endif

	if (!sslsock->is_client) {
		init_handshake_limiting(stream, sslsock);
	}

	if (!SSL_set_fd(sslsock->ssl_handle, sslsock->s.socket)) {
		handle_ssl_error(stream, 0, 1 TSRMLS_CC);
	}

	if (cparam->inputs.session) {
		if (cparam->inputs.session->ops != &php_openssl_socket_ops) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "supplied session stream must be an SSL enabled stream");
		} else if (((php_openssl_netstream_data_t*)cparam->inputs.session->abstract)->ssl_handle == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "supplied SSL session stream is not initialized");
		} else {
			SSL_copy_session_id(sslsock->ssl_handle, ((php_openssl_netstream_data_t*)cparam->inputs.session->abstract)->ssl_handle);
		}
	}
	return 0;
}

static zval *php_capture_ssl_session_meta(SSL *ssl_handle)
{
	zval *meta_arr;
	char *proto_str;
	long proto = SSL_version(ssl_handle);
	const SSL_CIPHER *cipher = SSL_get_current_cipher(ssl_handle);

	switch (proto) {
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
		case TLS1_2_VERSION: proto_str = "TLSv1.2"; break;
		case TLS1_1_VERSION: proto_str = "TLSv1.1"; break;
#endif
		case TLS1_VERSION: proto_str = "TLSv1"; break;
		case SSL3_VERSION: proto_str = "SSLv3"; break;
		case SSL2_VERSION: proto_str = "SSLv2"; break;
		default: proto_str = "UNKNOWN";
	}

	MAKE_STD_ZVAL(meta_arr);
	array_init(meta_arr);
	add_assoc_string(meta_arr, "protocol", proto_str, 1);
	add_assoc_string(meta_arr, "cipher_name", (char *) SSL_CIPHER_get_name(cipher), 1);
	add_assoc_long(meta_arr, "cipher_bits", SSL_CIPHER_get_bits(cipher, NULL));
	add_assoc_string(meta_arr, "cipher_version", SSL_CIPHER_get_version(cipher), 1);

	return meta_arr;
}

static int php_set_server_rsa_key(php_stream *stream,
	php_openssl_netstream_data_t *sslsock
	TSRMLS_DC)
{
	zval ** val;
	int rsa_key_size;
	RSA* rsa;
	int retval = 1;

	if (php_stream_context_get_option(stream->context, "ssl", "rsa_key_size", &val) == SUCCESS) {
		rsa_key_size = (int) Z_LVAL_PP(val);
		if ((rsa_key_size != 1) && (rsa_key_size & (rsa_key_size - 1))) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
				"RSA key size requires a power of 2: %d",
				rsa_key_size);

			rsa_key_size = 2048;
		}
	} else {
		rsa_key_size = 2048;
	}

	rsa = RSA_generate_key(rsa_key_size, RSA_F4, NULL, NULL);

	if (!SSL_set_tmp_rsa(sslsock->ssl_handle, rsa)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"Failed setting RSA key");
		retval = 0;
	}

	RSA_free(rsa);

	return retval;
}


static int php_set_server_dh_param(php_openssl_netstream_data_t *sslsock,
	char *dh_path
	TSRMLS_DC)
{
	DH *dh;
	BIO* bio;

	bio = BIO_new_file(dh_path, "r");

	if (bio == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"Invalid dh_param file: %s",
			dh_path);

		return 0;
	}

	dh = PEM_read_bio_DHparams(bio, NULL, NULL, NULL);
	BIO_free(bio);

	if (dh == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"Failed reading DH params from file: %s",
			dh_path);

		return 0;
	}

	if (SSL_set_tmp_dh(sslsock->ssl_handle, dh) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"DH param assignment failed");
		DH_free(dh);
		return 0;
	}

	DH_free(dh);

	return 1;
}

static int php_enable_server_crypto_opts(php_stream *stream,
	php_openssl_netstream_data_t *sslsock
	TSRMLS_DC)
{
	zval **val;
#ifdef HAVE_ECDH
	int curve_nid;
	EC_KEY *ecdh;
#endif

	if (php_stream_context_get_option(stream->context, "ssl", "dh_param", &val) == SUCCESS) {
		convert_to_string_ex(val);
		if (!php_set_server_dh_param(sslsock,  Z_STRVAL_PP(val) TSRMLS_CC)) {
			return 0;
		}
	}

#ifdef HAVE_ECDH

	if (php_stream_context_get_option(stream->context, "ssl", "ecdh_curve", &val) == SUCCESS) {
		char *curve_str;
		convert_to_string_ex(val);
		curve_str = Z_STRVAL_PP(val);
		curve_nid = OBJ_sn2nid(curve_str);
		if (curve_nid == NID_undef) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
				"Invalid ECDH curve: %s",
				curve_str);

			return 0;
		}
	} else {
		curve_nid = NID_X9_62_prime256v1;
	}

	ecdh = EC_KEY_new_by_curve_name(curve_nid);
	if (ecdh == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"Failed generating ECDH curve");

		return 0;
	}

	SSL_set_tmp_ecdh(sslsock->ssl_handle, ecdh);
	EC_KEY_free(ecdh);

#else

	if (php_stream_context_get_option(stream->context, "ssl", "ecdh_curve", &val) == SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"ECDH curve support not compiled into the OpenSSL lib against which PHP is linked");
	}

#endif

	if (!php_set_server_rsa_key(stream, sslsock TSRMLS_CC)) {
		return 0;
	}

	return 1;
}

#if OPENSSL_VERSION_NUMBER >= 0x00908070L && !defined(OPENSSL_NO_TLSEXT)
static inline void enable_client_sni(php_stream *stream, php_openssl_netstream_data_t *sslsock) /* {{{ */
{
	zval **val;

	if (stream->context &&
            (php_stream_context_get_option(stream->context, "ssl", "SNI_enabled", &val) == FAILURE
                || zend_is_true(*val))
	) {
		if (php_stream_context_get_option(stream->context, "ssl", "SNI_server_name", &val) == SUCCESS) {
			convert_to_string_ex(val);
			SSL_set_tlsext_host_name(sslsock->ssl_handle, Z_STRVAL_PP(val));
		} else if (sslsock->url_name) {
			SSL_set_tlsext_host_name(sslsock->ssl_handle, sslsock->url_name);
		}
	} else if (!stream->context && sslsock->url_name) {
		SSL_set_tlsext_host_name(sslsock->ssl_handle, sslsock->url_name);
	}
}
/* }}} */
#endif

static int capture_peer_certs(php_stream *stream,
	php_openssl_netstream_data_t *sslsock,
	X509 *peer_cert
	TSRMLS_DC)
{
	zval **val, *zcert;
	int cert_captured = 0;

	if (SUCCESS == php_stream_context_get_option(stream->context,
			"ssl", "capture_peer_cert", &val) &&
		zend_is_true(*val)
	) {
		MAKE_STD_ZVAL(zcert);
		ZVAL_RESOURCE(zcert, zend_list_insert(peer_cert, php_openssl_get_x509_list_id() TSRMLS_CC));
		php_stream_context_set_option(stream->context, "ssl", "peer_certificate", zcert);
		cert_captured = 1;
		FREE_ZVAL(zcert);
	}

	if (SUCCESS == php_stream_context_get_option(stream->context,
			"ssl", "capture_peer_cert_chain", &val) &&
		zend_is_true(*val)
	) {
		zval *arr;
		STACK_OF(X509) *chain;

		MAKE_STD_ZVAL(arr);
		chain = SSL_get_peer_cert_chain(sslsock->ssl_handle);

		if (chain && sk_X509_num(chain) > 0) {
			int i;
			array_init(arr);

			for (i = 0; i < sk_X509_num(chain); i++) {
				X509 *mycert = X509_dup(sk_X509_value(chain, i));
				MAKE_STD_ZVAL(zcert);
				ZVAL_RESOURCE(zcert, zend_list_insert(mycert, php_openssl_get_x509_list_id() TSRMLS_CC));
				add_next_index_zval(arr, zcert);
			}

		} else {
			ZVAL_NULL(arr);
		}

		php_stream_context_set_option(stream->context, "ssl", "peer_certificate_chain", arr);
		zval_dtor(arr);
		efree(arr);
	}

	return cert_captured;
}

static inline int php_openssl_enable_crypto(php_stream *stream,
		php_openssl_netstream_data_t *sslsock,
		php_stream_xport_crypto_param *cparam
		TSRMLS_DC)
{
	int n;
	int retry = 1;
	int cert_captured;
	X509 *peer_cert;

	if (cparam->inputs.activate && !sslsock->ssl_active) {
		struct timeval	start_time,
						*timeout;
		int				blocked		= sslsock->s.is_blocked,
						has_timeout = 0;

#if OPENSSL_VERSION_NUMBER >= 0x00908070L && !defined(OPENSSL_NO_TLSEXT)
{
		if (sslsock->is_client) {
			enable_client_sni(stream, sslsock);
		}
}
#endif

		if (!sslsock->is_client && !php_enable_server_crypto_opts(stream, sslsock TSRMLS_CC)) {
			return -1;
		}

		if (!sslsock->state_set) {
			if (sslsock->is_client) {
				SSL_set_connect_state(sslsock->ssl_handle);
			} else {
				SSL_set_accept_state(sslsock->ssl_handle);
			}
			sslsock->state_set = 1;
		}
	
		if (SUCCESS == php_set_sock_blocking(sslsock->s.socket, 0 TSRMLS_CC)) {
			sslsock->s.is_blocked = 0;
		}
		
		timeout = sslsock->is_client ? &sslsock->connect_timeout : &sslsock->s.timeout;
		has_timeout = !sslsock->s.is_blocked && (timeout->tv_sec || timeout->tv_usec);
		/* gettimeofday is not monotonic; using it here is not strictly correct */
		if (has_timeout) {
			gettimeofday(&start_time, NULL);
		}
		
		do {
			struct timeval	cur_time,
							elapsed_time = {0};
			
			if (sslsock->is_client) {
				n = SSL_connect(sslsock->ssl_handle);
			} else {
				n = SSL_accept(sslsock->ssl_handle);
			}

			if (has_timeout) {
				gettimeofday(&cur_time, NULL);
				elapsed_time.tv_sec  = cur_time.tv_sec  - start_time.tv_sec;
				elapsed_time.tv_usec = cur_time.tv_usec - start_time.tv_usec;
				if (cur_time.tv_usec < start_time.tv_usec) {
					elapsed_time.tv_sec  -= 1L;
					elapsed_time.tv_usec += 1000000L;
				}
			
				if (elapsed_time.tv_sec > timeout->tv_sec ||
						(elapsed_time.tv_sec == timeout->tv_sec &&
						elapsed_time.tv_usec > timeout->tv_usec)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "SSL: crypto enabling timeout");
					return -1;
				}
			}

			if (n <= 0) {
				/* in case of SSL_ERROR_WANT_READ/WRITE, do not retry in non-blocking mode */
				retry = handle_ssl_error(stream, n, blocked TSRMLS_CC);
				if (retry) {
					/* wait until something interesting happens in the socket. It may be a
					 * timeout. Also consider the unlikely of possibility of a write block  */
					int err = SSL_get_error(sslsock->ssl_handle, n);
					struct timeval left_time;
					
					if (has_timeout) {
						left_time.tv_sec  = timeout->tv_sec  - elapsed_time.tv_sec;
						left_time.tv_usec =	timeout->tv_usec - elapsed_time.tv_usec;
						if (timeout->tv_usec < elapsed_time.tv_usec) {
							left_time.tv_sec  -= 1L;
							left_time.tv_usec += 1000000L;
						}
					}
					php_pollfd_for(sslsock->s.socket, (err == SSL_ERROR_WANT_READ) ?
						(POLLIN|POLLPRI) : POLLOUT, has_timeout ? &left_time : NULL);
				}
			} else {
				retry = 0;
			}
		} while (retry);

		if (sslsock->s.is_blocked != blocked && SUCCESS == php_set_sock_blocking(sslsock->s.socket, blocked TSRMLS_CC)) {
			sslsock->s.is_blocked = blocked;
		}

		if (n == 1) {
			peer_cert = SSL_get_peer_certificate(sslsock->ssl_handle);
			if (peer_cert && stream->context) {
				cert_captured = capture_peer_certs(stream, sslsock, peer_cert TSRMLS_CC);
			}

			if (FAILURE == php_openssl_apply_verification_policy(sslsock->ssl_handle, peer_cert, stream TSRMLS_CC)) {
				SSL_shutdown(sslsock->ssl_handle);
				n = -1;
			} else {	
				sslsock->ssl_active = 1;

				if (stream->context) {
					zval **val;

					if (SUCCESS == php_stream_context_get_option(stream->context,
							"ssl", "capture_session_meta", &val) &&
						zend_is_true(*val)
					) {
						zval *meta_arr = php_capture_ssl_session_meta(sslsock->ssl_handle);
						php_stream_context_set_option(stream->context, "ssl", "session_meta", meta_arr);
						zval_dtor(meta_arr);
						efree(meta_arr);
					}
				}
			}
		} else if (errno == EAGAIN) {
			n = 0;
		} else {
			n = -1;
			peer_cert = SSL_get_peer_certificate(sslsock->ssl_handle);
			if (peer_cert && stream->context) {
				cert_captured = capture_peer_certs(stream, sslsock, peer_cert TSRMLS_CC);
			}
		}

		if (n && peer_cert && cert_captured == 0) {
			X509_free(peer_cert);
		}

		return n;

	} else if (!cparam->inputs.activate && sslsock->ssl_active) {
		/* deactivate - common for server/client */
		SSL_shutdown(sslsock->ssl_handle);
		sslsock->ssl_active = 0;
	}

	return -1;
}

static inline int php_openssl_tcp_sockop_accept(php_stream *stream, php_openssl_netstream_data_t *sock,
		php_stream_xport_param *xparam STREAMS_DC TSRMLS_DC)
{
	int clisock;

	xparam->outputs.client = NULL;

	clisock = php_network_accept_incoming(sock->s.socket,
			xparam->want_textaddr ? &xparam->outputs.textaddr : NULL,
			xparam->want_textaddr ? &xparam->outputs.textaddrlen : NULL,
			xparam->want_addr ? &xparam->outputs.addr : NULL,
			xparam->want_addr ? &xparam->outputs.addrlen : NULL,
			xparam->inputs.timeout,
			xparam->want_errortext ? &xparam->outputs.error_text : NULL,
			&xparam->outputs.error_code
			TSRMLS_CC);

	if (clisock >= 0) {
		php_openssl_netstream_data_t *clisockdata;

		clisockdata = emalloc(sizeof(*clisockdata));

		if (clisockdata == NULL) {
			closesocket(clisock);
			/* technically a fatal error */
		} else {
			/* copy underlying tcp fields */
			memset(clisockdata, 0, sizeof(*clisockdata));
			memcpy(clisockdata, sock, sizeof(clisockdata->s));

			clisockdata->s.socket = clisock;
			
			xparam->outputs.client = php_stream_alloc_rel(stream->ops, clisockdata, NULL, "r+");
			if (xparam->outputs.client) {
				xparam->outputs.client->context = stream->context;
				if (stream->context) {
					zend_list_addref(stream->context->rsrc_id);
				}
			}
		}

		if (xparam->outputs.client && sock->enable_on_connect) {
			/* remove the client bit */
			if (sock->method & STREAM_CRYPTO_IS_CLIENT) {
				sock->method = ((sock->method >> 1) << 1);
			}

			clisockdata->method = sock->method;

			if (php_stream_xport_crypto_setup(xparam->outputs.client, clisockdata->method,
					NULL TSRMLS_CC) < 0 || php_stream_xport_crypto_enable(
					xparam->outputs.client, 1 TSRMLS_CC) < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to enable crypto");

				php_stream_close(xparam->outputs.client);
				xparam->outputs.client = NULL;
				xparam->outputs.returncode = -1;
			}
		}
	}
	
	return xparam->outputs.client == NULL ? -1 : 0;
}
static int php_openssl_sockop_set_option(php_stream *stream, int option, int value, void *ptrparam TSRMLS_DC)
{
	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;
	php_stream_xport_crypto_param *cparam = (php_stream_xport_crypto_param *)ptrparam;
	php_stream_xport_param *xparam = (php_stream_xport_param *)ptrparam;

	switch (option) {
		case PHP_STREAM_OPTION_CHECK_LIVENESS:
			{
				struct timeval tv;
				char buf;
				int alive = 1;

				if (value == -1) {
					if (sslsock->s.timeout.tv_sec == -1) {
						tv.tv_sec = FG(default_socket_timeout);
						tv.tv_usec = 0;
					} else {
						tv = sslsock->connect_timeout;
					}
				} else {
					tv.tv_sec = value;
					tv.tv_usec = 0;
				}

				if (sslsock->s.socket == -1) {
					alive = 0;
				} else if (php_pollfd_for(sslsock->s.socket, PHP_POLLREADABLE|POLLPRI, &tv) > 0) {
					if (sslsock->ssl_active) {
						int n;

						do {
							n = SSL_peek(sslsock->ssl_handle, &buf, sizeof(buf));
							if (n <= 0) {
								int err = SSL_get_error(sslsock->ssl_handle, n);

								if (err == SSL_ERROR_SYSCALL) {
									alive = php_socket_errno() == EAGAIN;
									break;
								}

								if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
									/* re-negotiate */
									continue;
								}

								/* any other problem is a fatal error */
								alive = 0;
							}
							/* either peek succeeded or there was an error; we
							 * have set the alive flag appropriately */
							break;
						} while (1);
					} else if (0 == recv(sslsock->s.socket, &buf, sizeof(buf), MSG_PEEK) && php_socket_errno() != EAGAIN) {
						alive = 0;
					}
				}
				return alive ? PHP_STREAM_OPTION_RETURN_OK : PHP_STREAM_OPTION_RETURN_ERR;
			}
			
		case PHP_STREAM_OPTION_CRYPTO_API:

			switch(cparam->op) {

				case STREAM_XPORT_CRYPTO_OP_SETUP:
					cparam->outputs.returncode = php_openssl_setup_crypto(stream, sslsock, cparam TSRMLS_CC);
					return PHP_STREAM_OPTION_RETURN_OK;
					break;
				case STREAM_XPORT_CRYPTO_OP_ENABLE:
					cparam->outputs.returncode = php_openssl_enable_crypto(stream, sslsock, cparam TSRMLS_CC);
					return PHP_STREAM_OPTION_RETURN_OK;
					break;
				default:
					/* fall through */
					break;
			}

			break;

		case PHP_STREAM_OPTION_XPORT_API:
			switch(xparam->op) {

				case STREAM_XPORT_OP_CONNECT:
				case STREAM_XPORT_OP_CONNECT_ASYNC:
					/* TODO: Async connects need to check the enable_on_connect option when
					 * we notice that the connect has actually been established */
					php_stream_socket_ops.set_option(stream, option, value, ptrparam TSRMLS_CC);

					if ((sslsock->enable_on_connect) &&
						((xparam->outputs.returncode == 0) ||
						(xparam->op == STREAM_XPORT_OP_CONNECT_ASYNC && 
						xparam->outputs.returncode == 1 && xparam->outputs.error_code == EINPROGRESS)))
					{
						if (php_stream_xport_crypto_setup(stream, sslsock->method, NULL TSRMLS_CC) < 0 ||
								php_stream_xport_crypto_enable(stream, 1 TSRMLS_CC) < 0) {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to enable crypto");
							xparam->outputs.returncode = -1;
						}
					}
					return PHP_STREAM_OPTION_RETURN_OK;

				case STREAM_XPORT_OP_ACCEPT:
					/* we need to copy the additional fields that the underlying tcp transport
					 * doesn't know about */
					xparam->outputs.returncode = php_openssl_tcp_sockop_accept(stream, sslsock, xparam STREAMS_CC TSRMLS_CC);

					
					return PHP_STREAM_OPTION_RETURN_OK;

				default:
					/* fall through */
					break;
			}
	}

	return php_stream_socket_ops.set_option(stream, option, value, ptrparam TSRMLS_CC);
}

static int php_openssl_sockop_cast(php_stream *stream, int castas, void **ret TSRMLS_DC)
{
	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;

	switch(castas)	{
		case PHP_STREAM_AS_STDIO:
			if (sslsock->ssl_active) {
				return FAILURE;
			}
			if (ret)	{
				*ret = fdopen(sslsock->s.socket, stream->mode);
				if (*ret) {
					return SUCCESS;
				}
				return FAILURE;
			}
			return SUCCESS;

		case PHP_STREAM_AS_FD_FOR_SELECT:
			if (ret) {
				*(php_socket_t *)ret = sslsock->s.socket;
			}
			return SUCCESS;

		case PHP_STREAM_AS_FD:
		case PHP_STREAM_AS_SOCKETD:
			if (sslsock->ssl_active) {
				return FAILURE;
			}
			if (ret) {
				*(php_socket_t *)ret = sslsock->s.socket;
			}
			return SUCCESS;
		default:
			return FAILURE;
	}
}

php_stream_ops php_openssl_socket_ops = {
	php_openssl_sockop_write, php_openssl_sockop_read,
	php_openssl_sockop_close, php_openssl_sockop_flush,
	"tcp_socket/ssl",
	NULL, /* seek */
	php_openssl_sockop_cast,
	php_openssl_sockop_stat,
	php_openssl_sockop_set_option,
};

static long get_crypto_method(php_stream_context *ctx, long crypto_method)
{
	zval **val;

	if (ctx && php_stream_context_get_option(ctx, "ssl", "crypto_method", &val) == SUCCESS) {
		convert_to_long_ex(val);
		crypto_method = (long)Z_LVAL_PP(val);
	        crypto_method |= STREAM_CRYPTO_IS_CLIENT;
	}

	return crypto_method;
}

static char *get_url_name(const char *resourcename, size_t resourcenamelen, int is_persistent TSRMLS_DC)
{
	php_url *url;

	if (!resourcename) {
		return NULL;
	}

	url = php_url_parse_ex(resourcename, resourcenamelen);
	if (!url) {
		return NULL;
	}

	if (url->host) {
		const char * host = url->host;
		char * url_name = NULL;
		size_t len = strlen(host);

		/* skip trailing dots */
		while (len && host[len-1] == '.') {
			--len;
		}

		if (len) {
			url_name = pestrndup(host, len, is_persistent);
		}

		php_url_free(url);
		return url_name;
	}

	php_url_free(url);
	return NULL;
}

php_stream *php_openssl_ssl_socket_factory(const char *proto, size_t protolen,
		const char *resourcename, size_t resourcenamelen,
		const char *persistent_id, int options, int flags,
		struct timeval *timeout,
		php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	php_stream *stream = NULL;
	php_openssl_netstream_data_t *sslsock = NULL;

	sslsock = pemalloc(sizeof(php_openssl_netstream_data_t), persistent_id ? 1 : 0);
	memset(sslsock, 0, sizeof(*sslsock));

	sslsock->s.is_blocked = 1;
	/* this timeout is used by standard stream funcs, therefor it should use the default value */
	sslsock->s.timeout.tv_sec = FG(default_socket_timeout);
	sslsock->s.timeout.tv_usec = 0;

	/* use separate timeout for our private funcs */
	sslsock->connect_timeout.tv_sec = timeout->tv_sec;
	sslsock->connect_timeout.tv_usec = timeout->tv_usec;

	/* we don't know the socket until we have determined if we are binding or
	 * connecting */
	sslsock->s.socket = -1;

	/* Initialize context as NULL */
	sslsock->ctx = NULL;	

	stream = php_stream_alloc_rel(&php_openssl_socket_ops, sslsock, persistent_id, "r+");

	if (stream == NULL)	{
		pefree(sslsock, persistent_id ? 1 : 0);
		return NULL;
	}

	if (strncmp(proto, "ssl", protolen) == 0) {
		sslsock->enable_on_connect = 1;
		sslsock->method = get_crypto_method(context, STREAM_CRYPTO_METHOD_ANY_CLIENT);
	} else if (strncmp(proto, "sslv2", protolen) == 0) {
#ifdef OPENSSL_NO_SSL2
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "SSLv2 support is not compiled into the OpenSSL library PHP is linked against");
		return NULL;
#else
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_SSLv2_CLIENT;
#endif
	} else if (strncmp(proto, "sslv3", protolen) == 0) {
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_SSLv3_CLIENT;
	} else if (strncmp(proto, "tls", protolen) == 0) {
		sslsock->enable_on_connect = 1;
		sslsock->method = get_crypto_method(context, STREAM_CRYPTO_METHOD_TLS_CLIENT);
	} else if (strncmp(proto, "tlsv1.0", protolen) == 0) {
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_TLSv1_0_CLIENT;
	} else if (strncmp(proto, "tlsv1.1", protolen) == 0) {
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_TLSv1_1_CLIENT;
#else
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "TLSv1.1 support is not compiled into the OpenSSL library PHP is linked against");
		return NULL;
#endif
	} else if (strncmp(proto, "tlsv1.2", protolen) == 0) {
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT;
#else
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "TLSv1.2 support is not compiled into the OpenSSL library PHP is linked against");
		return NULL;
#endif
	}

	sslsock->url_name = get_url_name(resourcename, resourcenamelen, !!persistent_id TSRMLS_CC);

	return stream;
}



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
