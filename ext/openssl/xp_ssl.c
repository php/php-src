/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
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
#include "php_network.h"
#include "php_openssl.h"
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/err.h>

#ifdef PHP_WIN32
#include "win32/time.h"
#endif

#ifdef NETWARE
#include <sys/select.h>
#endif

int php_openssl_apply_verification_policy(SSL *ssl, X509 *peer, php_stream *stream TSRMLS_DC);
SSL *php_SSL_new_from_context(SSL_CTX *ctx, php_stream *stream TSRMLS_DC);
int php_openssl_get_x509_list_id(void);
static struct timeval subtract_timeval( struct timeval a, struct timeval b );
static int compare_timeval( struct timeval a, struct timeval b );
static size_t php_openssl_sockop_io(int read, php_stream *stream, char *buf, size_t count TSRMLS_DC);

/* This implementation is very closely tied to the that of the native
 * sockets implemented in the core.
 * Don't try this technique in other extensions!
 * */

typedef struct _php_openssl_netstream_data_t {
	php_netstream_data_t s;
	SSL *ssl_handle;
	SSL_CTX *ctx;
	struct timeval connect_timeout;
	int enable_on_connect;
	int is_client;
	int ssl_active;
	php_stream_xport_crypt_method_t method;
	char *sni;
	unsigned state_set:1;
	unsigned _spare:31;
} php_openssl_netstream_data_t;

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

static size_t php_openssl_sockop_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	return php_openssl_sockop_io(1, stream, buf, count TSRMLS_CC);
}

static size_t php_openssl_sockop_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	return php_openssl_sockop_io(0, stream, (char*)buf, count TSRMLS_CC);
}

/**
 * Factored out common functionality (blocking, timeout, loop management) for read and write.
 * Perform IO (read or write) to an SSL socket. If we have a timeout, we switch to non-blocking mode
 * for the duration of the operation, using select to do our waits. If we time out, or we have an error
 * report that back to PHP
 *
 */
static size_t php_openssl_sockop_io(int read, php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	php_openssl_netstream_data_t *sslsock = (php_openssl_netstream_data_t*)stream->abstract;
	int nr_bytes = 0;

	/* Only do this if SSL is active. */
	if (sslsock->ssl_active) {
		int retry = 1;
		struct timeval start_time;
		struct timeval *timeout = NULL;
		int began_blocked = sslsock->s.is_blocked;
		int has_timeout = 0;

		/* never use a timeout with non-blocking sockets */
		if (began_blocked && &sslsock->s.timeout) {
			timeout = &sslsock->s.timeout;
		}

		if (timeout && php_set_sock_blocking(sslsock->s.socket, 0 TSRMLS_CC) == SUCCESS) {
			sslsock->s.is_blocked = 0;
		}

		if (!sslsock->s.is_blocked && timeout && (timeout->tv_sec || timeout->tv_usec)) {
			has_timeout = 1;
			/* gettimeofday is not monotonic; using it here is not strictly correct */
			gettimeofday(&start_time, NULL);
		}

		/* Main IO loop. */
		do {
			struct timeval cur_time, elapsed_time, left_time;
	
			/* If we have a timeout to check, figure out how much time has elapsed since we started. */
			if (has_timeout) {
				gettimeofday(&cur_time, NULL);

				/* Determine how much time we've taken so far. */
				elapsed_time = subtract_timeval(cur_time, start_time);

				/* and return an error if we've taken too long. */
				if (compare_timeval(elapsed_time, *timeout) > 0 ) {
					/* If the socket was originally blocking, set it back. */
					if (began_blocked) {
						php_set_sock_blocking(sslsock->s.socket, 1 TSRMLS_CC);
						sslsock->s.is_blocked = 1;
					}
					sslsock->s.timeout_event = 1;
					return -1;
				}
			}

			/* Now, do the IO operation. Don't block if we can't complete... */
			if (read) {
				nr_bytes = SSL_read(sslsock->ssl_handle, buf, count);
			} else {
				nr_bytes = SSL_write(sslsock->ssl_handle, buf, count);
			}

			/* Now, how much time until we time out? */
			if (has_timeout) {
				left_time = subtract_timeval( *timeout, elapsed_time );
			}

			/* If we didn't do anything on the last loop (or an error) check to see if we should retry or exit. */
			if (nr_bytes <= 0) {

				/* Get the error code from SSL, and check to see if it's an error or not. */
				int err = SSL_get_error(sslsock->ssl_handle, nr_bytes );
				retry = handle_ssl_error(stream, nr_bytes, 0 TSRMLS_CC);

				/* If we get this (the above doesn't check) then we'll retry as well. */
				if (errno == EAGAIN && err == SSL_ERROR_WANT_READ && read) {
					retry = 1;
				}
				if (errno == EAGAIN && err == SSL_ERROR_WANT_WRITE && read == 0) {
					retry = 1;
				}

				/* Also, on reads, we may get this condition on an EOF. We should check properly. */
				if (read) {
					stream->eof = (retry == 0 && errno != EAGAIN && !SSL_pending(sslsock->ssl_handle));
				}

				/* Don't loop indefinitely in non-blocking mode if no data is available */
				if (began_blocked == 0) {
					break;
				}

				/* Now, if we have to wait some time, and we're supposed to be blocking, wait for the socket to become
				 * available. Now, php_pollfd_for uses select to wait up to our time_left value only...
				 */
				if (retry) {
					if (read) {
						php_pollfd_for(sslsock->s.socket, (err == SSL_ERROR_WANT_WRITE) ?
							(POLLOUT|POLLPRI) : (POLLIN|POLLPRI), has_timeout ? &left_time : NULL);
					} else {
						php_pollfd_for(sslsock->s.socket, (err == SSL_ERROR_WANT_READ) ?
							(POLLIN|POLLPRI) : (POLLOUT|POLLPRI), has_timeout ? &left_time : NULL);
					}
				}
			} else {
				/* Else, if we got bytes back, check for possible errors. */
				int err = SSL_get_error(sslsock->ssl_handle, nr_bytes);

				/* If we didn't get any error, then let's return it to PHP. */
				if (err == SSL_ERROR_NONE) {
					break;
				}

				/* Otherwise, we need to wait again (up to time_left or we get an error) */
				if (began_blocked) {
					if (read) {
						php_pollfd_for(sslsock->s.socket, (err == SSL_ERROR_WANT_WRITE) ?
							(POLLOUT|POLLPRI) : (POLLIN|POLLPRI), has_timeout ? &left_time : NULL);
					} else {
						php_pollfd_for(sslsock->s.socket, (err == SSL_ERROR_WANT_READ) ?
							(POLLIN|POLLPRI) : (POLLOUT|POLLPRI), has_timeout ? &left_time : NULL);
					}
				}
			}
		/* Finally, we keep going until we got data, and an SSL_ERROR_NONE, unless we had an error. */
		} while (retry);

		/* Tell PHP if we read / wrote bytes. */
		if (nr_bytes > 0) {
			php_stream_notify_progress_increment(stream->context, nr_bytes, 0);
		}

		/* And if we were originally supposed to be blocking, let's reset the socket to that. */
		if (began_blocked && php_set_sock_blocking(sslsock->s.socket, 1 TSRMLS_CC) == SUCCESS) {
			sslsock->s.is_blocked = 1;
		}
	} else {
		/* This block is if we had no timeout... We will just sit and wait forever on the IO operation. */
		if (read) {
			nr_bytes = php_stream_socket_ops.read(stream, buf, count TSRMLS_CC);
		} else {
			nr_bytes = php_stream_socket_ops.write(stream, buf, count TSRMLS_CC);
		}
	}

	/* PHP doesn't expect a negative return. */
	if (nr_bytes < 0) {
		nr_bytes = 0;
	}

	return nr_bytes;
}

struct timeval subtract_timeval( struct timeval a, struct timeval b )
{
	struct timeval difference;

	difference.tv_sec  = a.tv_sec  - b.tv_sec;
	difference.tv_usec = a.tv_usec - b.tv_usec;

	if (a.tv_usec < b.tv_usec) {
	  	b.tv_sec  -= 1L;
	   	b.tv_usec += 1000000L;
	}

	return difference;
}

int compare_timeval( struct timeval a, struct timeval b )
{
	if (a.tv_sec > b.tv_sec || (a.tv_sec == b.tv_sec && a.tv_usec > b.tv_usec) ) {
		return 1;
	} else if( a.tv_sec == b.tv_sec && a.tv_usec == b.tv_usec ) {
		return 0;
	} else {
		return -1;
	}
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

	if (sslsock->sni) {
		pefree(sslsock->sni, php_stream_is_persistent(stream));
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


static inline int php_openssl_setup_crypto(php_stream *stream,
		php_openssl_netstream_data_t *sslsock,
		php_stream_xport_crypto_param *cparam
		TSRMLS_DC)
{
	SSL_METHOD *method;
	long ssl_ctx_options = SSL_OP_ALL;
	
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

	switch (cparam->inputs.method) {
		case STREAM_CRYPTO_METHOD_SSLv23_CLIENT:
			sslsock->is_client = 1;
			method = SSLv23_client_method();
			break;
		case STREAM_CRYPTO_METHOD_SSLv2_CLIENT:
#ifdef OPENSSL_NO_SSL2
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "SSLv2 support is not compiled into the OpenSSL library PHP is linked against");
			return -1;
#else
			sslsock->is_client = 1;
			method = SSLv2_client_method();
			break;
#endif
		case STREAM_CRYPTO_METHOD_SSLv3_CLIENT:
#ifdef OPENSSL_NO_SSL3
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "SSLv3 support is not compiled into the OpenSSL library PHP is linked against");
			return -1;
#else
			sslsock->is_client = 1;
			method = SSLv3_client_method();
			break;
#endif
		case STREAM_CRYPTO_METHOD_TLS_CLIENT:
			sslsock->is_client = 1;
			method = TLSv1_client_method();
			break;
		case STREAM_CRYPTO_METHOD_SSLv23_SERVER:
			sslsock->is_client = 0;
			method = SSLv23_server_method();
			break;
		case STREAM_CRYPTO_METHOD_SSLv3_SERVER:
#ifdef OPENSSL_NO_SSL3
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "SSLv3 support is not compiled into the OpenSSL library PHP is linked against");
			return -1;
#else
			sslsock->is_client = 0;
			method = SSLv3_server_method();
			break;
#endif
		case STREAM_CRYPTO_METHOD_SSLv2_SERVER:
#ifdef OPENSSL_NO_SSL2
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "SSLv2 support is not compiled into the OpenSSL library PHP is linked against");
			return -1;
#else
			sslsock->is_client = 0;
			method = SSLv2_server_method();
			break;
#endif
		case STREAM_CRYPTO_METHOD_TLS_SERVER:
			sslsock->is_client = 0;
			method = TLSv1_server_method();
			break;
		default:
			return -1;

	}

	sslsock->ctx = SSL_CTX_new(method);
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
		zval **val;

		if (stream->context && SUCCESS == php_stream_context_get_option(
					stream->context, "ssl", "no_ticket", &val) && 
				zval_is_true(*val)) {
			SSL_CTX_set_options(sslsock->ctx, SSL_OP_NO_TICKET);
		}
	}
#endif

#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	{
		zval **val;

		if (stream->context && SUCCESS == php_stream_context_get_option(
					stream->context, "ssl", "disable_compression", &val) &&
				zval_is_true(*val)) {
			SSL_CTX_set_options(sslsock->ctx, SSL_OP_NO_COMPRESSION);
		}
	}
#endif

	sslsock->ssl_handle = php_SSL_new_from_context(sslsock->ctx, stream TSRMLS_CC);
	if (sslsock->ssl_handle == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to create an SSL handle");
		SSL_CTX_free(sslsock->ctx);
		sslsock->ctx = NULL;
		return -1;
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

static inline int php_openssl_enable_crypto(php_stream *stream,
		php_openssl_netstream_data_t *sslsock,
		php_stream_xport_crypto_param *cparam
		TSRMLS_DC)
{
	int n, retry = 1;

	if (cparam->inputs.activate && !sslsock->ssl_active) {
		struct timeval	start_time,
						*timeout;
		int				blocked		= sslsock->s.is_blocked,
						has_timeout = 0;

#if OPENSSL_VERSION_NUMBER >= 0x0090806fL && !defined(OPENSSL_NO_TLSEXT)
		if (sslsock->is_client && sslsock->sni) {
			SSL_set_tlsext_host_name(sslsock->ssl_handle, sslsock->sni);
		}
#endif

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
				elapsed_time = subtract_timeval( cur_time, start_time );
			
				if (compare_timeval( elapsed_time, *timeout) > 0) {
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
						left_time = subtract_timeval( *timeout, elapsed_time );
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
			X509 *peer_cert;

			peer_cert = SSL_get_peer_certificate(sslsock->ssl_handle);

			if (FAILURE == php_openssl_apply_verification_policy(sslsock->ssl_handle, peer_cert, stream TSRMLS_CC)) {
				SSL_shutdown(sslsock->ssl_handle);
				n = -1;
			} else {	
				sslsock->ssl_active = 1;

				/* allow the script to capture the peer cert
				 * and/or the certificate chain */
				if (stream->context) {
					zval **val, *zcert;

					if (SUCCESS == php_stream_context_get_option(
								stream->context, "ssl",
								"capture_peer_cert", &val) &&
							zval_is_true(*val)) {
						MAKE_STD_ZVAL(zcert);
						ZVAL_RESOURCE(zcert, zend_list_insert(peer_cert, 
									php_openssl_get_x509_list_id() TSRMLS_CC));
						php_stream_context_set_option(stream->context,
								"ssl", "peer_certificate",
								zcert);
						peer_cert = NULL;
						FREE_ZVAL(zcert);
					}

					if (SUCCESS == php_stream_context_get_option(
								stream->context, "ssl",
								"capture_peer_cert_chain", &val) &&
							zval_is_true(*val)) {
						zval *arr;
						STACK_OF(X509) *chain;

						MAKE_STD_ZVAL(arr);
						chain = SSL_get_peer_cert_chain(
									sslsock->ssl_handle);

						if (chain && sk_X509_num(chain) > 0) {
							int i;
							array_init(arr);

							for (i = 0; i < sk_X509_num(chain); i++) {
								X509 *mycert = X509_dup(
										sk_X509_value(chain, i));
								MAKE_STD_ZVAL(zcert);
								ZVAL_RESOURCE(zcert,
										zend_list_insert(mycert,
											php_openssl_get_x509_list_id() TSRMLS_CC));
								add_next_index_zval(arr, zcert);
							}

						} else {
							ZVAL_NULL(arr);
						}

						php_stream_context_set_option(stream->context,
								"ssl", "peer_certificate_chain",
								arr);
						zval_dtor(arr);
						efree(arr);
					}
				}
			}

			if (peer_cert) {
				X509_free(peer_cert);
			}
		} else  {
			n = errno == EAGAIN ? 0 : -1;
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
			/* apply crypto */
			switch (sock->method) {
				case STREAM_CRYPTO_METHOD_SSLv23_CLIENT:
					sock->method = STREAM_CRYPTO_METHOD_SSLv23_SERVER;
					break;
				case STREAM_CRYPTO_METHOD_SSLv2_CLIENT:
					sock->method = STREAM_CRYPTO_METHOD_SSLv2_SERVER;
					break;
				case STREAM_CRYPTO_METHOD_SSLv3_CLIENT:
					sock->method = STREAM_CRYPTO_METHOD_SSLv3_SERVER;
					break;
				case STREAM_CRYPTO_METHOD_TLS_CLIENT:
					sock->method = STREAM_CRYPTO_METHOD_TLS_SERVER;
					break;
				default:
					break;
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
				/* OpenSSL has an internal buffer which select() cannot see. If we don't
				 * fetch it into the stream's buffer, no activity will be reported on the
				 * stream even though there is data waiting to be read - but we only fetch
				 * the lower of bytes OpenSSL has ready to give us or chunk_size since we
				 * weren't asked for any data at this stage. This is only likely to cause
				 * issues with non-blocking streams, but it's harmless to always do it. */
				size_t pending;
				if (stream->writepos == stream->readpos
					&& sslsock->ssl_active
					&& (pending = (size_t)SSL_pending(sslsock->ssl_handle)) > 0) {
						php_stream_fill_read_buffer(stream, pending < stream->chunk_size
							? pending
							: stream->chunk_size);
				}

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

static char * get_sni(php_stream_context *ctx, char *resourcename, long resourcenamelen, int is_persistent TSRMLS_DC) {

	php_url *url;

	if (ctx) {
		zval **val = NULL;

		if (php_stream_context_get_option(ctx, "ssl", "SNI_enabled", &val) == SUCCESS && !zend_is_true(*val)) {
			return NULL;
		}
		if (php_stream_context_get_option(ctx, "ssl", "SNI_server_name", &val) == SUCCESS) {
			convert_to_string_ex(val);
			return pestrdup(Z_STRVAL_PP(val), is_persistent);
		}
	}

	if (!resourcename) {
		return NULL;
	}

	url = php_url_parse_ex(resourcename, resourcenamelen);
	if (!url) {
		return NULL;
	}

	if (url->host) {
		const char * host = url->host;
		char * sni = NULL;
		size_t len = strlen(host);

		/* skip trailing dots */
		while (len && host[len-1] == '.') {
			--len;
		}

		if (len) {
			sni = pestrndup(host, len, is_persistent);
		}

		php_url_free(url);
		return sni;
	}

	php_url_free(url);
	return NULL;
}

php_stream *php_openssl_ssl_socket_factory(const char *proto, long protolen,
		char *resourcename, long resourcenamelen,
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

	sslsock->sni = get_sni(context, resourcename, resourcenamelen, !!persistent_id TSRMLS_CC);
	
	if (strncmp(proto, "ssl", protolen) == 0) {
		sslsock->enable_on_connect = 1;
		sslsock->method = STREAM_CRYPTO_METHOD_SSLv23_CLIENT;
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
		sslsock->method = STREAM_CRYPTO_METHOD_TLS_CLIENT;
	}

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
