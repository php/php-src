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
#ifdef PHP_WIN32
#include "config.w32.h"
#include <Ws2tcpip.h>
#endif

#if HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif

typedef php_stream *(php_stream_transport_factory_func)(const char *proto, size_t protolen,
		const char *resourcename, size_t resourcenamelen,
		const char *persistent_id, int options, int flags,
		struct timeval *timeout,
		php_stream_context *context STREAMS_DC TSRMLS_DC);
typedef php_stream_transport_factory_func *php_stream_transport_factory;

BEGIN_EXTERN_C()
PHPAPI int php_stream_xport_register(const char *protocol, php_stream_transport_factory factory TSRMLS_DC);
PHPAPI int php_stream_xport_unregister(const char *protocol TSRMLS_DC);

#define STREAM_XPORT_CLIENT			0
#define STREAM_XPORT_SERVER			1

#define STREAM_XPORT_CONNECT		2
#define STREAM_XPORT_BIND			4
#define STREAM_XPORT_LISTEN			8
#define STREAM_XPORT_CONNECT_ASYNC	16

/* Open a client or server socket connection */
PHPAPI php_stream *_php_stream_xport_create(const char *name, size_t namelen, int options,
		int flags, const char *persistent_id,
		struct timeval *timeout,
		php_stream_context *context,
		char **error_string,
		int *error_code
		STREAMS_DC TSRMLS_DC);

#define php_stream_xport_create(name, namelen, options, flags, persistent_id, timeout, context, estr, ecode) \
	_php_stream_xport_create(name, namelen, options, flags, persistent_id, timeout, context, estr, ecode STREAMS_CC TSRMLS_CC)

/* Bind the stream to a local address */
PHPAPI int php_stream_xport_bind(php_stream *stream,
		const char *name, size_t namelen,
		char **error_text
		TSRMLS_DC);

/* Connect to a remote address */
PHPAPI int php_stream_xport_connect(php_stream *stream,
		const char *name, size_t namelen,
		int asynchronous,
		struct timeval *timeout,
		char **error_text,
		int *error_code
		TSRMLS_DC);

/* Prepare to listen */
PHPAPI int php_stream_xport_listen(php_stream *stream,
		int backlog,
		char **error_text
		TSRMLS_DC);

/* Get the next client and their address as a string, or the underlying address
 * structure.  You must efree either of these if you request them */
PHPAPI int php_stream_xport_accept(php_stream *stream, php_stream **client,
		char **textaddr, int *textaddrlen,
		void **addr, socklen_t *addrlen,
		struct timeval *timeout,
		char **error_text
		TSRMLS_DC);

/* Get the name of either the socket or it's peer */
PHPAPI int php_stream_xport_get_name(php_stream *stream, int want_peer,
		char **textaddr, int *textaddrlen,
		void **addr, socklen_t *addrlen
		TSRMLS_DC);

enum php_stream_xport_send_recv_flags {
	STREAM_OOB = 1,
	STREAM_PEEK = 2
};

/* Similar to recv() system call; read data from the stream, optionally
 * peeking, optionally retrieving OOB data */
PHPAPI int php_stream_xport_recvfrom(php_stream *stream, char *buf, size_t buflen,
		long flags, void **addr, socklen_t *addrlen,
		char **textaddr, int *textaddrlen TSRMLS_DC);

/* Similar to send() system call; send data to the stream, optionally
 * sending it as OOB data */
PHPAPI int php_stream_xport_sendto(php_stream *stream, const char *buf, size_t buflen,
		long flags, void *addr, socklen_t addrlen TSRMLS_DC);

typedef enum {
	STREAM_SHUT_RD,
	STREAM_SHUT_WR,
	STREAM_SHUT_RDWR
} stream_shutdown_t;

/* Similar to shutdown() system call; shut down part of a full-duplex
 * connection */
PHPAPI int php_stream_xport_shutdown(php_stream *stream, stream_shutdown_t how TSRMLS_DC);
END_EXTERN_C()


/* Structure definition for the set_option interface that the above functions wrap */

typedef struct _php_stream_xport_param {
	enum {
		STREAM_XPORT_OP_BIND, STREAM_XPORT_OP_CONNECT,
		STREAM_XPORT_OP_LISTEN, STREAM_XPORT_OP_ACCEPT,
		STREAM_XPORT_OP_CONNECT_ASYNC,
		STREAM_XPORT_OP_GET_NAME,
		STREAM_XPORT_OP_GET_PEER_NAME,
		STREAM_XPORT_OP_RECV,
		STREAM_XPORT_OP_SEND,
		STREAM_XPORT_OP_SHUTDOWN
	} op;
	unsigned int want_addr:1;
	unsigned int want_textaddr:1;
	unsigned int want_errortext:1;
	unsigned int how:2;

	struct {
		char *name;
		size_t namelen;
		int backlog;
		struct timeval *timeout;
		struct sockaddr *addr;
		socklen_t addrlen;
		char *buf;
		size_t buflen;
		long flags;
	} inputs;
	struct {
		php_stream *client;
		int returncode;
		struct sockaddr *addr;
		socklen_t addrlen;
		char *textaddr;
		long textaddrlen;

		char *error_text;
		int error_code;
	} outputs;
} php_stream_xport_param;


/* These functions provide crypto support on the underlying transport */
typedef enum {
	STREAM_CRYPTO_METHOD_SSLv2_CLIENT,
	STREAM_CRYPTO_METHOD_SSLv3_CLIENT,
	STREAM_CRYPTO_METHOD_SSLv23_CLIENT,
	STREAM_CRYPTO_METHOD_TLS_CLIENT,
	STREAM_CRYPTO_METHOD_TLSv1_1_CLIENT,
	STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT,
	STREAM_CRYPTO_METHOD_SSLv2_SERVER,
	STREAM_CRYPTO_METHOD_SSLv3_SERVER,
	STREAM_CRYPTO_METHOD_SSLv23_SERVER,
	STREAM_CRYPTO_METHOD_TLS_SERVER,
	STREAM_CRYPTO_METHOD_TLSv1_1_SERVER,
	STREAM_CRYPTO_METHOD_TLSv1_2_SERVER
} php_stream_xport_crypt_method_t;

BEGIN_EXTERN_C()
PHPAPI int php_stream_xport_crypto_setup(php_stream *stream, php_stream_xport_crypt_method_t crypto_method, php_stream *session_stream TSRMLS_DC);
PHPAPI int php_stream_xport_crypto_enable(php_stream *stream, int activate TSRMLS_DC);
END_EXTERN_C()

typedef struct _php_stream_xport_crypto_param {
	enum {
		STREAM_XPORT_CRYPTO_OP_SETUP,
		STREAM_XPORT_CRYPTO_OP_ENABLE
	} op;
	struct {
		int activate;
		php_stream_xport_crypt_method_t method;
		php_stream *session;
	} inputs;
	struct {
		int returncode;
	} outputs;
} php_stream_xport_crypto_param;

BEGIN_EXTERN_C()
PHPAPI HashTable *php_stream_xport_get_hash(void);
PHPAPI php_stream_transport_factory_func php_stream_generic_socket_factory;
END_EXTERN_C()

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
