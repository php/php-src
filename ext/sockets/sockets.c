/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
   | Authors: Chris Vandomelen <chrisv@b0rked.dhs.org>                    |
   |          Sterling Hughes  <sterling@php.net>                         |
   |          Jason Greene     <jason@php.net>                            |
   |          Gustavo Lopes    <cataphract@php.net>                       |
   | WinSock: Daniel Beulshausen <daniel@php4win.de>                      |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include "php_network.h"
#include "ext/standard/file.h"
#include "ext/standard/info.h"
#include "php_ini.h"
#ifdef PHP_WIN32
# include "windows_common.h"
# include <win32/inet.h>
# include <windows.h>
# include <Ws2tcpip.h>
# include "php_sockets.h"
# include <win32/sockets.h>
# include <win32/winutil.h>
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
# include <sys/un.h>
# include <arpa/inet.h>
# include <sys/time.h>
# include <unistd.h>
# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <sys/uio.h>
# define IS_INVALID_SOCKET(a)	(a->bsd_socket < 0)
# define set_errno(a) (errno = a)
# include "php_sockets.h"
# if HAVE_IF_NAMETOINDEX
#  include <net/if.h>
# endif
#endif

#include <stddef.h>

#include "sockaddr_conv.h"
#include "multicast.h"
#include "sendrecvmsg.h"

ZEND_DECLARE_MODULE_GLOBALS(sockets)

#ifndef MSG_WAITALL
#ifdef LINUX
#define MSG_WAITALL 0x00000100
#else
#define MSG_WAITALL 0x00000000
#endif
#endif

#ifndef MSG_EOF
#ifdef MSG_FIN
#define MSG_EOF MSG_FIN
#endif
#endif

#ifndef SUN_LEN
#define SUN_LEN(su) (sizeof(*(su)) - sizeof((su)->sun_path) + strlen((su)->sun_path))
#endif

#ifndef PF_INET
#define PF_INET AF_INET
#endif

#define PHP_NORMAL_READ 0x0001
#define PHP_BINARY_READ 0x0002

static int le_socket;
#define le_socket_name php_sockets_le_socket_name

static int le_addrinfo;
#define le_addrinfo_name php_sockets_le_addrinfo_name

/* The AI_IDN_ALLOW_UNASSIGNED deprecations are implemented as a pragma GCC warning,
 * using _Pragma() for macro support. As this warning is thrown without a warning
 * category, it's also not possible to suppress it, because it is not part of
 * -Wdeprecated-declarations or similar. We work around this by defining
 * __glibc_macro_warning() to be empty. */
#undef __glibc_macro_warning
#define __glibc_macro_warning(message)

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_select, 0, 0, 4)
	ZEND_ARG_INFO(1, read_fds)
	ZEND_ARG_INFO(1, write_fds)
	ZEND_ARG_INFO(1, except_fds)
	ZEND_ARG_INFO(0, tv_sec)
	ZEND_ARG_INFO(0, tv_usec)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_create_listen, 0, 0, 1)
	ZEND_ARG_INFO(0, port)
	ZEND_ARG_INFO(0, backlog)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_accept, 0, 0, 1)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_set_nonblock, 0, 0, 1)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_set_block, 0, 0, 1)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_listen, 0, 0, 1)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, backlog)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_close, 0, 0, 1)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_write, 0, 0, 2)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, buf)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_read, 0, 0, 2)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_getsockname, 0, 0, 2)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(1, addr)
	ZEND_ARG_INFO(1, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_getpeername, 0, 0, 2)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(1, addr)
	ZEND_ARG_INFO(1, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_create, 0, 0, 3)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, protocol)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_connect, 0, 0, 2)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, addr)
	ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_strerror, 0, 0, 1)
	ZEND_ARG_INFO(0, errno)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_bind, 0, 0, 2)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, addr)
	ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_recv, 0, 0, 4)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(1, buf)
	ZEND_ARG_INFO(0, len)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_send, 0, 0, 4)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, buf)
	ZEND_ARG_INFO(0, len)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_recvfrom, 0, 0, 5)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(1, buf)
	ZEND_ARG_INFO(0, len)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(1, name)
	ZEND_ARG_INFO(1, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_sendto, 0, 0, 5)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, buf)
	ZEND_ARG_INFO(0, len)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, addr)
	ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_get_option, 0, 0, 3)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, level)
	ZEND_ARG_INFO(0, optname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_set_option, 0, 0, 4)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, level)
	ZEND_ARG_INFO(0, optname)
	ZEND_ARG_INFO(0, optval)
ZEND_END_ARG_INFO()

#ifdef HAVE_SOCKETPAIR
ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_create_pair, 0, 0, 4)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, protocol)
	ZEND_ARG_INFO(1, fd)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_SHUTDOWN
ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_shutdown, 0, 0, 1)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, how)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_last_error, 0, 0, 0)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_clear_error, 0, 0, 0)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_import_stream, 0, 0, 1)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_export_stream, 0, 0, 1)
	ZEND_ARG_INFO(0, socket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_sendmsg, 0, 0, 3)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, msghdr)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_recvmsg, 0, 0, 3)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(1, msghdr)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_cmsg_space, 0, 0, 2)
	ZEND_ARG_INFO(0, level)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_addrinfo_lookup, 0, 0, 1)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, service)
	ZEND_ARG_INFO(0, hints)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_addrinfo_connect, 0, 0, 1)
	ZEND_ARG_INFO(0, addr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_addrinfo_bind, 0, 0, 1)
	ZEND_ARG_INFO(0, addr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_addrinfo_explain, 0, 0, 1)
	ZEND_ARG_INFO(0, addr)
ZEND_END_ARG_INFO()

#ifdef PHP_WIN32
ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_wsaprotocol_info_export, 0, 0, 2)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, target_pid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_wsaprotocol_info_import, 0, 0, 1)
	ZEND_ARG_INFO(0, info_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_socket_wsaprotocol_info_release, 0, 0, 1)
	ZEND_ARG_INFO(0, info_id)
ZEND_END_ARG_INFO()
#endif
/* }}} */

static PHP_GINIT_FUNCTION(sockets);
static PHP_GSHUTDOWN_FUNCTION(sockets);
static PHP_MINIT_FUNCTION(sockets);
static PHP_MSHUTDOWN_FUNCTION(sockets);
static PHP_MINFO_FUNCTION(sockets);
static PHP_RSHUTDOWN_FUNCTION(sockets);

PHP_FUNCTION(socket_select);
PHP_FUNCTION(socket_create_listen);
#ifdef HAVE_SOCKETPAIR
PHP_FUNCTION(socket_create_pair);
#endif
PHP_FUNCTION(socket_accept);
PHP_FUNCTION(socket_set_nonblock);
PHP_FUNCTION(socket_set_block);
PHP_FUNCTION(socket_listen);
PHP_FUNCTION(socket_close);
PHP_FUNCTION(socket_write);
PHP_FUNCTION(socket_read);
PHP_FUNCTION(socket_getsockname);
PHP_FUNCTION(socket_getpeername);
PHP_FUNCTION(socket_create);
PHP_FUNCTION(socket_connect);
PHP_FUNCTION(socket_strerror);
PHP_FUNCTION(socket_bind);
PHP_FUNCTION(socket_recv);
PHP_FUNCTION(socket_send);
PHP_FUNCTION(socket_recvfrom);
PHP_FUNCTION(socket_sendto);
PHP_FUNCTION(socket_get_option);
PHP_FUNCTION(socket_set_option);
#ifdef HAVE_SHUTDOWN
PHP_FUNCTION(socket_shutdown);
#endif
PHP_FUNCTION(socket_last_error);
PHP_FUNCTION(socket_clear_error);
PHP_FUNCTION(socket_import_stream);
PHP_FUNCTION(socket_export_stream);
PHP_FUNCTION(socket_addrinfo_lookup);
PHP_FUNCTION(socket_addrinfo_connect);
PHP_FUNCTION(socket_addrinfo_bind);
PHP_FUNCTION(socket_addrinfo_explain);
#ifdef PHP_WIN32
PHP_FUNCTION(socket_wsaprotocol_info_export);
PHP_FUNCTION(socket_wsaprotocol_info_import);
PHP_FUNCTION(socket_wsaprotocol_info_release);
#endif

/* {{{ sockets_functions[]
 */
static const zend_function_entry sockets_functions[] = {
	PHP_FE(socket_select,			arginfo_socket_select)
	PHP_FE(socket_create,			arginfo_socket_create)
	PHP_FE(socket_create_listen,	arginfo_socket_create_listen)
#ifdef HAVE_SOCKETPAIR
	PHP_FE(socket_create_pair,		arginfo_socket_create_pair)
#endif
	PHP_FE(socket_accept,			arginfo_socket_accept)
	PHP_FE(socket_set_nonblock,		arginfo_socket_set_nonblock)
	PHP_FE(socket_set_block,		arginfo_socket_set_block)
	PHP_FE(socket_listen,			arginfo_socket_listen)
	PHP_FE(socket_close,			arginfo_socket_close)
	PHP_FE(socket_write,			arginfo_socket_write)
	PHP_FE(socket_read,				arginfo_socket_read)
	PHP_FE(socket_getsockname, 		arginfo_socket_getsockname)
	PHP_FE(socket_getpeername, 		arginfo_socket_getpeername)
	PHP_FE(socket_connect,			arginfo_socket_connect)
	PHP_FE(socket_strerror,			arginfo_socket_strerror)
	PHP_FE(socket_bind,				arginfo_socket_bind)
	PHP_FE(socket_recv,				arginfo_socket_recv)
	PHP_FE(socket_send,				arginfo_socket_send)
	PHP_FE(socket_recvfrom,			arginfo_socket_recvfrom)
	PHP_FE(socket_sendto,			arginfo_socket_sendto)
	PHP_FE(socket_get_option,		arginfo_socket_get_option)
	PHP_FE(socket_set_option,		arginfo_socket_set_option)
#ifdef HAVE_SHUTDOWN
	PHP_FE(socket_shutdown,			arginfo_socket_shutdown)
#endif
	PHP_FE(socket_last_error,		arginfo_socket_last_error)
	PHP_FE(socket_clear_error,		arginfo_socket_clear_error)
	PHP_FE(socket_import_stream,	arginfo_socket_import_stream)
	PHP_FE(socket_export_stream,	arginfo_socket_export_stream)
	PHP_FE(socket_sendmsg,			arginfo_socket_sendmsg)
	PHP_FE(socket_recvmsg,			arginfo_socket_recvmsg)
	PHP_FE(socket_cmsg_space,		arginfo_socket_cmsg_space)
	PHP_FE(socket_addrinfo_lookup,	arginfo_socket_addrinfo_lookup)
	PHP_FE(socket_addrinfo_connect,	arginfo_socket_addrinfo_connect)
	PHP_FE(socket_addrinfo_bind,	arginfo_socket_addrinfo_bind)
	PHP_FE(socket_addrinfo_explain,	arginfo_socket_addrinfo_explain)

	/* for downwards compatibility */
	PHP_FALIAS(socket_getopt, socket_get_option, arginfo_socket_get_option)
	PHP_FALIAS(socket_setopt, socket_set_option, arginfo_socket_set_option)

#ifdef PHP_WIN32
	PHP_FE(socket_wsaprotocol_info_export, arginfo_socket_wsaprotocol_info_export)
	PHP_FE(socket_wsaprotocol_info_import, arginfo_socket_wsaprotocol_info_import)
	PHP_FE(socket_wsaprotocol_info_release, arginfo_socket_wsaprotocol_info_release)
#endif

	PHP_FE_END
};
/* }}} */

zend_module_entry sockets_module_entry = {
	STANDARD_MODULE_HEADER,
	"sockets",
	sockets_functions,
	PHP_MINIT(sockets),
	PHP_MSHUTDOWN(sockets),
	NULL,
	PHP_RSHUTDOWN(sockets),
	PHP_MINFO(sockets),
	PHP_SOCKETS_VERSION,
	PHP_MODULE_GLOBALS(sockets),
	PHP_GINIT(sockets),
	PHP_GSHUTDOWN(sockets),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};


#ifdef COMPILE_DL_SOCKETS
#ifdef ZTS
	ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(sockets)
#endif

/* inet_ntop should be used instead of inet_ntoa */
int inet_ntoa_lock = 0;

PHP_SOCKETS_API int php_sockets_le_socket(void) /* {{{ */
{
	return le_socket;
}
/* }}} */

/* allocating function to make programming errors due to uninitialized fields
 * less likely */
PHP_SOCKETS_API php_socket *php_create_socket(void) /* {{{ */
{
	php_socket *php_sock = emalloc(sizeof(php_socket));

	php_sock->bsd_socket = -1; /* invalid socket */
	php_sock->type		 = PF_UNSPEC;
	php_sock->error		 = 0;
	php_sock->blocking	 = 1;
	ZVAL_UNDEF(&php_sock->zstream);

	return php_sock;
}
/* }}} */

PHP_SOCKETS_API void php_destroy_socket(zend_resource *rsrc) /* {{{ */
{
	php_socket *php_sock = rsrc->ptr;

	if (Z_ISUNDEF(php_sock->zstream)) {
		if (!IS_INVALID_SOCKET(php_sock)) {
			close(php_sock->bsd_socket);
		}
	} else {
		zval_ptr_dtor(&php_sock->zstream);
	}
	efree(php_sock);
}
/* }}} */

PHP_SOCKETS_API void php_destroy_addrinfo(zend_resource *rsrc) /* {{{ */
{
	struct addrinfo *addr = rsrc->ptr;
	efree(addr->ai_addr);
	if (addr->ai_canonname != NULL) {
		efree(addr->ai_canonname);
	}
	efree(addr);
}
/* }}} */

static int php_open_listen_sock(php_socket **php_sock, int port, int backlog) /* {{{ */
{
	struct sockaddr_in  la;
	struct hostent		*hp;
	php_socket			*sock = php_create_socket();

	*php_sock = sock;

#ifndef PHP_WIN32
	if ((hp = php_network_gethostbyname("0.0.0.0")) == NULL) {
#else
	if ((hp = php_network_gethostbyname("localhost")) == NULL) {
#endif
		efree(sock);
		return 0;
	}

	memcpy((char *) &la.sin_addr, hp->h_addr, hp->h_length);
	la.sin_family = hp->h_addrtype;
	la.sin_port = htons((unsigned short) port);

	sock->bsd_socket = socket(PF_INET, SOCK_STREAM, 0);
	sock->blocking = 1;

	if (IS_INVALID_SOCKET(sock)) {
		PHP_SOCKET_ERROR(sock, "unable to create listening socket", errno);
		efree(sock);
		return 0;
	}

	sock->type = PF_INET;

	if (bind(sock->bsd_socket, (struct sockaddr *)&la, sizeof(la)) != 0) {
		PHP_SOCKET_ERROR(sock, "unable to bind to given address", errno);
		close(sock->bsd_socket);
		efree(sock);
		return 0;
	}

	if (listen(sock->bsd_socket, backlog) != 0) {
		PHP_SOCKET_ERROR(sock, "unable to listen on socket", errno);
		close(sock->bsd_socket);
		efree(sock);
		return 0;
	}

	return 1;
}
/* }}} */

static int php_accept_connect(php_socket *in_sock, php_socket **new_sock, struct sockaddr *la, socklen_t *la_len) /* {{{ */
{
	php_socket	*out_sock = php_create_socket();

	*new_sock = out_sock;

	out_sock->bsd_socket = accept(in_sock->bsd_socket, la, la_len);

	if (IS_INVALID_SOCKET(out_sock)) {
		PHP_SOCKET_ERROR(out_sock, "unable to accept incoming connection", errno);
		efree(out_sock);
		return 0;
	}

	out_sock->error = 0;
	out_sock->blocking = 1;
	out_sock->type = la->sa_family;

	return 1;
}
/* }}} */

/* {{{ php_read -- wrapper around read() so that it only reads to a \r or \n. */
static int php_read(php_socket *sock, void *buf, size_t maxlen, int flags)
{
	int m = 0;
	size_t n = 0;
	int no_read = 0;
	int nonblock = 0;
	char *t = (char *) buf;

#ifndef PHP_WIN32
	m = fcntl(sock->bsd_socket, F_GETFL);
	if (m < 0) {
		return m;
	}
	nonblock = (m & O_NONBLOCK);
	m = 0;
#else
	nonblock = !sock->blocking;
#endif
	set_errno(0);

	*t = '\0';
	while (*t != '\n' && *t != '\r' && n < maxlen) {
		if (m > 0) {
			t++;
			n++;
		} else if (m == 0) {
			no_read++;
			if (nonblock && no_read >= 2) {
				return n;
				/* The first pass, m always is 0, so no_read becomes 1
				 * in the first pass. no_read becomes 2 in the second pass,
				 * and if this is nonblocking, we should return.. */
			}

			if (no_read > 200) {
				set_errno(ECONNRESET);
				return -1;
			}
		}

		if (n < maxlen) {
			m = recv(sock->bsd_socket, (void *) t, 1, flags);
		}

		if (errno != 0 && errno != ESPIPE && errno != EAGAIN) {
			return -1;
		}

		set_errno(0);
	}

	if (n < maxlen) {
		n++;
		/* The only reasons it makes it to here is
		 * if '\n' or '\r' are encountered. So, increase
		 * the return by 1 to make up for the lack of the
		 * '\n' or '\r' in the count (since read() takes
		 * place at the end of the loop..) */
	}

	return n;
}
/* }}} */

char *sockets_strerror(int error) /* {{{ */
{
	const char *buf;

#ifndef PHP_WIN32
	if (error < -10000) {
		error = -error - 10000;

#ifdef HAVE_HSTRERROR
		buf = hstrerror(error);
#else
		{
			if (SOCKETS_G(strerror_buf)) {
				efree(SOCKETS_G(strerror_buf));
			}

			spprintf(&(SOCKETS_G(strerror_buf)), 0, "Host lookup error %d", error);
			buf = SOCKETS_G(strerror_buf);
		}
#endif
	} else {
		buf = strerror(error);
	}
#else
	{
		char *tmp = php_win32_error_to_msg(error);
		buf = NULL;

		if (tmp[0]) {
			if (SOCKETS_G(strerror_buf)) {
				efree(SOCKETS_G(strerror_buf));
			}

			SOCKETS_G(strerror_buf) = estrdup(tmp);
			free(tmp);

			buf = SOCKETS_G(strerror_buf);
		}
	}
#endif

	return (buf ? (char *) buf : "");
}
/* }}} */

#ifdef PHP_WIN32
static void sockets_destroy_wsa_info(zval *data)
{/*{{{*/
	HANDLE h = (HANDLE)Z_PTR_P(data);
	CloseHandle(h);
}/*}}}*/
#endif

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(sockets)
{
#if defined(COMPILE_DL_SOCKETS) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	sockets_globals->last_error = 0;
	sockets_globals->strerror_buf = NULL;
#ifdef PHP_WIN32
	sockets_globals->wsa_child_count = 0;
	zend_hash_init(&sockets_globals->wsa_info, 0, NULL, sockets_destroy_wsa_info, 1);
#endif
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION */
static PHP_GSHUTDOWN_FUNCTION(sockets)
{
#ifdef PHP_WIN32
	zend_hash_destroy(&sockets_globals->wsa_info);
#endif
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
static PHP_MINIT_FUNCTION(sockets)
{
#if defined(COMPILE_DL_SOCKETS) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	le_socket = zend_register_list_destructors_ex(php_destroy_socket, NULL, le_socket_name, module_number);
	le_addrinfo = zend_register_list_destructors_ex(php_destroy_addrinfo, NULL, le_addrinfo_name, module_number);

	REGISTER_LONG_CONSTANT("AF_UNIX",		AF_UNIX,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("AF_INET",		AF_INET,		CONST_CS | CONST_PERSISTENT);
#if HAVE_IPV6
	REGISTER_LONG_CONSTANT("AF_INET6",		AF_INET6,		CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SOCK_STREAM",	SOCK_STREAM,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_DGRAM",	SOCK_DGRAM,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_RAW",		SOCK_RAW,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_SEQPACKET",SOCK_SEQPACKET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOCK_RDM",		SOCK_RDM,		CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("MSG_OOB",		MSG_OOB,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_WAITALL",	MSG_WAITALL,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_CTRUNC",	MSG_CTRUNC,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_TRUNC",		MSG_TRUNC,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_PEEK",		MSG_PEEK,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MSG_DONTROUTE", MSG_DONTROUTE,	CONST_CS | CONST_PERSISTENT);
#ifdef MSG_EOR
	REGISTER_LONG_CONSTANT("MSG_EOR",		MSG_EOR,		CONST_CS | CONST_PERSISTENT);
#endif
#ifdef MSG_EOF
	REGISTER_LONG_CONSTANT("MSG_EOF",		MSG_EOF,		CONST_CS | CONST_PERSISTENT);
#endif

#ifdef MSG_CONFIRM
	REGISTER_LONG_CONSTANT("MSG_CONFIRM",	MSG_CONFIRM,	CONST_CS | CONST_PERSISTENT);
#endif
#ifdef MSG_ERRQUEUE
	REGISTER_LONG_CONSTANT("MSG_ERRQUEUE",	MSG_ERRQUEUE,	CONST_CS | CONST_PERSISTENT);
#endif
#ifdef MSG_NOSIGNAL
	REGISTER_LONG_CONSTANT("MSG_NOSIGNAL",	MSG_NOSIGNAL,	CONST_CS | CONST_PERSISTENT);
#endif
#ifdef MSG_DONTWAIT
	REGISTER_LONG_CONSTANT("MSG_DONTWAIT",	MSG_DONTWAIT,	CONST_CS | CONST_PERSISTENT);
#endif
#ifdef MSG_MORE
	REGISTER_LONG_CONSTANT("MSG_MORE",		MSG_MORE,		CONST_CS | CONST_PERSISTENT);
#endif
#ifdef MSG_WAITFORONE
	REGISTER_LONG_CONSTANT("MSG_WAITFORONE",MSG_WAITFORONE,	CONST_CS | CONST_PERSISTENT);
#endif
#ifdef MSG_CMSG_CLOEXEC
	REGISTER_LONG_CONSTANT("MSG_CMSG_CLOEXEC",MSG_CMSG_CLOEXEC,CONST_CS | CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("SO_DEBUG",		SO_DEBUG,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_REUSEADDR",	SO_REUSEADDR,	CONST_CS | CONST_PERSISTENT);
#ifdef SO_REUSEPORT
	REGISTER_LONG_CONSTANT("SO_REUSEPORT",	SO_REUSEPORT,	CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SO_KEEPALIVE",	SO_KEEPALIVE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_DONTROUTE",	SO_DONTROUTE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_LINGER",		SO_LINGER,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_BROADCAST",	SO_BROADCAST,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_OOBINLINE",	SO_OOBINLINE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_SNDBUF",		SO_SNDBUF,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_RCVBUF",		SO_RCVBUF,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_SNDLOWAT",	SO_SNDLOWAT,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_RCVLOWAT",	SO_RCVLOWAT,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_SNDTIMEO",	SO_SNDTIMEO,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_RCVTIMEO",	SO_RCVTIMEO,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_TYPE",		SO_TYPE,		CONST_CS | CONST_PERSISTENT);
#ifdef SO_FAMILY
	REGISTER_LONG_CONSTANT("SO_FAMILY",		SO_FAMILY,		CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SO_ERROR",		SO_ERROR,		CONST_CS | CONST_PERSISTENT);
#ifdef SO_BINDTODEVICE
	REGISTER_LONG_CONSTANT("SO_BINDTODEVICE",       SO_BINDTODEVICE,        CONST_CS | CONST_PERSISTENT);
#endif
#ifdef SO_USER_COOKIE
	REGISTER_LONG_CONSTANT("SO_LABEL",       SO_LABEL,        CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_PEERLABEL",       SO_PEERLABEL,        CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_LISTENQLIMIT",       SO_LISTENQLIMIT,        CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_LISTENQLEN",       SO_LISTENQLEN,        CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SO_USER_COOKIE",       SO_USER_COOKIE,        CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("SOL_SOCKET",	SOL_SOCKET,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOMAXCONN",		SOMAXCONN,		CONST_CS | CONST_PERSISTENT);
#ifdef TCP_NODELAY
	REGISTER_LONG_CONSTANT("TCP_NODELAY",   TCP_NODELAY,    CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("PHP_NORMAL_READ", PHP_NORMAL_READ, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_BINARY_READ", PHP_BINARY_READ, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("MCAST_JOIN_GROUP",			PHP_MCAST_JOIN_GROUP,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCAST_LEAVE_GROUP",			PHP_MCAST_LEAVE_GROUP,			CONST_CS | CONST_PERSISTENT);
#ifdef HAS_MCAST_EXT
	REGISTER_LONG_CONSTANT("MCAST_BLOCK_SOURCE",		PHP_MCAST_BLOCK_SOURCE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCAST_UNBLOCK_SOURCE",		PHP_MCAST_UNBLOCK_SOURCE,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCAST_JOIN_SOURCE_GROUP",	PHP_MCAST_JOIN_SOURCE_GROUP,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MCAST_LEAVE_SOURCE_GROUP",	PHP_MCAST_LEAVE_SOURCE_GROUP,	CONST_CS | CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("IP_MULTICAST_IF",			IP_MULTICAST_IF,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IP_MULTICAST_TTL",			IP_MULTICAST_TTL,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IP_MULTICAST_LOOP",			IP_MULTICAST_LOOP,		CONST_CS | CONST_PERSISTENT);
#if HAVE_IPV6
	REGISTER_LONG_CONSTANT("IPV6_MULTICAST_IF",			IPV6_MULTICAST_IF,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IPV6_MULTICAST_HOPS",		IPV6_MULTICAST_HOPS,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IPV6_MULTICAST_LOOP",		IPV6_MULTICAST_LOOP,	CONST_CS | CONST_PERSISTENT);
#endif

#ifdef IPV6_V6ONLY
	REGISTER_LONG_CONSTANT("IPV6_V6ONLY",			IPV6_V6ONLY,		CONST_CS | CONST_PERSISTENT);
#endif

#ifndef WIN32
# include "unix_socket_constants.h"
#else
# include "win32_socket_constants.h"
#endif

	REGISTER_LONG_CONSTANT("IPPROTO_IP",	IPPROTO_IP,		CONST_CS | CONST_PERSISTENT);
#if HAVE_IPV6
	REGISTER_LONG_CONSTANT("IPPROTO_IPV6",	IPPROTO_IPV6,	CONST_CS | CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("SOL_TCP",		IPPROTO_TCP,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SOL_UDP",		IPPROTO_UDP,	CONST_CS | CONST_PERSISTENT);

#if HAVE_IPV6
	REGISTER_LONG_CONSTANT("IPV6_UNICAST_HOPS",			IPV6_UNICAST_HOPS,	CONST_CS | CONST_PERSISTENT);
#endif

	REGISTER_LONG_CONSTANT("AI_PASSIVE",		AI_PASSIVE,			CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("AI_CANONNAME",		AI_CANONNAME,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("AI_NUMERICHOST",	AI_NUMERICHOST,		CONST_CS | CONST_PERSISTENT);
#if HAVE_AI_V4MAPPED
	REGISTER_LONG_CONSTANT("AI_V4MAPPED",		AI_V4MAPPED,		CONST_CS | CONST_PERSISTENT);
#endif
#if HAVE_AI_ALL
	REGISTER_LONG_CONSTANT("AI_ALL",			AI_ALL,				CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("AI_ADDRCONFIG",		AI_ADDRCONFIG,		CONST_CS | CONST_PERSISTENT);
#if HAVE_AI_IDN
	REGISTER_LONG_CONSTANT("AI_IDN",			AI_IDN,				CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("AI_CANONIDN",		AI_CANONIDN,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("AI_IDN_ALLOW_UNASSIGNED",		AI_IDN_ALLOW_UNASSIGNED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("AI_IDN_USE_STD3_ASCII_RULES",	AI_IDN_USE_STD3_ASCII_RULES, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef AI_NUMERICSERV
	REGISTER_LONG_CONSTANT("AI_NUMERICSERV",	AI_NUMERICSERV,		CONST_CS | CONST_PERSISTENT);
#endif

	php_socket_sendrecvmsg_init(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
static PHP_MSHUTDOWN_FUNCTION(sockets)
{
	php_socket_sendrecvmsg_shutdown(SHUTDOWN_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(sockets)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Sockets Support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
static PHP_RSHUTDOWN_FUNCTION(sockets)
{
	if (SOCKETS_G(strerror_buf)) {
		efree(SOCKETS_G(strerror_buf));
		SOCKETS_G(strerror_buf) = NULL;
	}

	return SUCCESS;
}
/* }}} */

static int php_sock_array_to_fd_set(zval *sock_array, fd_set *fds, PHP_SOCKET *max_fd) /* {{{ */
{
	zval		*element;
	php_socket	*php_sock;
	int			num = 0;

	if (Z_TYPE_P(sock_array) != IS_ARRAY) return 0;

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(sock_array), element) {
		ZVAL_DEREF(element);
		php_sock = (php_socket*) zend_fetch_resource_ex(element, le_socket_name, le_socket);
		if (!php_sock) continue; /* If element is not a resource, skip it */

		PHP_SAFE_FD_SET(php_sock->bsd_socket, fds);
		if (php_sock->bsd_socket > *max_fd) {
			*max_fd = php_sock->bsd_socket;
		}
		num++;
	} ZEND_HASH_FOREACH_END();

	return num ? 1 : 0;
}
/* }}} */

static int php_sock_array_from_fd_set(zval *sock_array, fd_set *fds) /* {{{ */
{
	zval		*element;
	zval		*dest_element;
	php_socket	*php_sock;
	zval		new_hash;
	int			num = 0;
	zend_ulong       num_key;
	zend_string *key;

	if (Z_TYPE_P(sock_array) != IS_ARRAY) return 0;

	array_init(&new_hash);
	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(sock_array), num_key, key, element) {
		ZVAL_DEREF(element);
		php_sock = (php_socket*) zend_fetch_resource_ex(element, le_socket_name, le_socket);
		if (!php_sock) continue; /* If element is not a resource, skip it */

		if (PHP_SAFE_FD_ISSET(php_sock->bsd_socket, fds)) {
			/* Add fd to new array */
			if (key) {
				dest_element = zend_hash_add(Z_ARRVAL(new_hash), key, element);
			} else {
				dest_element = zend_hash_index_update(Z_ARRVAL(new_hash), num_key, element);
			}
			if (dest_element) {
				Z_ADDREF_P(dest_element);
			}
		}
		num++;
	} ZEND_HASH_FOREACH_END();

	/* Destroy old array, add new one */
	zval_ptr_dtor(sock_array);

	ZVAL_COPY_VALUE(sock_array, &new_hash);

	return num ? 1 : 0;
}
/* }}} */

/* {{{ proto int socket_select(array &read_fds, array &write_fds, array &except_fds, int tv_sec[, int tv_usec])
   Runs the select() system call on the sets mentioned with a timeout specified by tv_sec and tv_usec */
PHP_FUNCTION(socket_select)
{
	zval			*r_array, *w_array, *e_array, *sec;
	struct timeval	tv;
	struct timeval *tv_p = NULL;
	fd_set			rfds, wfds, efds;
	PHP_SOCKET		max_fd = 0;
	int				retval, sets = 0;
	zend_long			usec = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a!a!a!z!|l", &r_array, &w_array, &e_array, &sec, &usec) == FAILURE) {
		return;
	}

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);

	if (r_array != NULL) sets += php_sock_array_to_fd_set(r_array, &rfds, &max_fd);
	if (w_array != NULL) sets += php_sock_array_to_fd_set(w_array, &wfds, &max_fd);
	if (e_array != NULL) sets += php_sock_array_to_fd_set(e_array, &efds, &max_fd);

	if (!sets) {
		php_error_docref(NULL, E_WARNING, "no resource arrays were passed to select");
		RETURN_FALSE;
	}

	PHP_SAFE_MAX_FD(max_fd, 0); /* someone needs to make this look more like stream_socket_select */

	/* If seconds is not set to null, build the timeval, else we wait indefinitely */
	if (sec != NULL) {
		zend_long s = zval_get_long(sec);

		/* Solaris + BSD do not like microsecond values which are >= 1 sec */
		if (usec > 999999) {
			tv.tv_sec = s + (usec / 1000000);
			tv.tv_usec = usec % 1000000;
		} else {
			tv.tv_sec = s;
			tv.tv_usec = usec;
		}

		tv_p = &tv;
	}

	retval = select(max_fd+1, &rfds, &wfds, &efds, tv_p);

	if (retval == -1) {
		SOCKETS_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "unable to select [%d]: %s", errno, sockets_strerror(errno));
		RETURN_FALSE;
	}

	if (r_array != NULL) php_sock_array_from_fd_set(r_array, &rfds);
	if (w_array != NULL) php_sock_array_from_fd_set(w_array, &wfds);
	if (e_array != NULL) php_sock_array_from_fd_set(e_array, &efds);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto resource socket_create_listen(int port[, int backlog])
   Opens a socket on port to accept connections */
PHP_FUNCTION(socket_create_listen)
{
	php_socket	*php_sock;
	zend_long		port, backlog = 128;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &port, &backlog) == FAILURE) {
		return;
	}

	if (!php_open_listen_sock(&php_sock, port, backlog)) {
		RETURN_FALSE;
	}

	php_sock->error = 0;
	php_sock->blocking = 1;

	RETURN_RES(zend_register_resource(php_sock, le_socket));
}
/* }}} */

/* {{{ proto resource socket_accept(resource socket)
   Accepts a connection on the listening socket fd */
PHP_FUNCTION(socket_accept)
{
	zval				 *arg1;
	php_socket			 *php_sock, *new_sock;
	php_sockaddr_storage sa;
	socklen_t			 php_sa_len = sizeof(sa);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &arg1) == FAILURE) {
		return;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	if (!php_accept_connect(php_sock, &new_sock, (struct sockaddr*)&sa, &php_sa_len)) {
		RETURN_FALSE;
	}

	RETURN_RES(zend_register_resource(new_sock, le_socket));
}
/* }}} */

/* {{{ proto bool socket_set_nonblock(resource socket)
   Sets nonblocking mode on a socket resource */
PHP_FUNCTION(socket_set_nonblock)
{
	zval		*arg1;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &arg1) == FAILURE) {
		return;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	if (!Z_ISUNDEF(php_sock->zstream)) {
		php_stream *stream;
		/* omit notice if resource doesn't exist anymore */
		stream = zend_fetch_resource2_ex(&php_sock->zstream, NULL, php_file_le_stream(), php_file_le_pstream());
		if (stream != NULL) {
			if (php_stream_set_option(stream, PHP_STREAM_OPTION_BLOCKING, 0,
					NULL) != -1) {
				php_sock->blocking = 0;
				RETURN_TRUE;
			}
		}
	}

	if (php_set_sock_blocking(php_sock->bsd_socket, 0) == SUCCESS) {
		php_sock->blocking = 0;
		RETURN_TRUE;
	} else {
		PHP_SOCKET_ERROR(php_sock, "unable to set nonblocking mode", errno);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool socket_set_block(resource socket)
   Sets blocking mode on a socket resource */
PHP_FUNCTION(socket_set_block)
{
	zval		*arg1;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &arg1) == FAILURE) {
		return;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	/* if socket was created from a stream, give the stream a chance to take
	 * care of the operation itself, thereby allowing it to update its internal
	 * state */
	if (!Z_ISUNDEF(php_sock->zstream)) {
		php_stream *stream;
		stream = zend_fetch_resource2_ex(&php_sock->zstream, NULL, php_file_le_stream(), php_file_le_pstream());
		if (stream != NULL) {
			if (php_stream_set_option(stream, PHP_STREAM_OPTION_BLOCKING, 1,
					NULL) != -1) {
				php_sock->blocking = 1;
				RETURN_TRUE;
			}
		}
	}

	if (php_set_sock_blocking(php_sock->bsd_socket, 1) == SUCCESS) {
		php_sock->blocking = 1;
		RETURN_TRUE;
	} else {
		PHP_SOCKET_ERROR(php_sock, "unable to set blocking mode", errno);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool socket_listen(resource socket[, int backlog])
   Sets the maximum number of connections allowed to be waited for on the socket specified by fd */
PHP_FUNCTION(socket_listen)
{
	zval		*arg1;
	php_socket	*php_sock;
	zend_long		backlog = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &arg1, &backlog) == FAILURE) {
		return;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	if (listen(php_sock->bsd_socket, backlog) != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to listen on socket", errno);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void socket_close(resource socket)
   Closes a file descriptor */
PHP_FUNCTION(socket_close)
{
	zval		*arg1;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &arg1) == FAILURE) {
		return;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	if (!Z_ISUNDEF(php_sock->zstream)) {
		php_stream *stream = NULL;
		php_stream_from_zval_no_verify(stream, &php_sock->zstream);
		if (stream != NULL) {
			/* close & destroy stream, incl. removing it from the rsrc list;
			 * resource stored in php_sock->zstream will become invalid */
			php_stream_free(stream,
					PHP_STREAM_FREE_KEEP_RSRC | PHP_STREAM_FREE_CLOSE |
					(stream->is_persistent?PHP_STREAM_FREE_CLOSE_PERSISTENT:0));
		}
	}
	zend_list_close(Z_RES_P(arg1));
}
/* }}} */

/* {{{ proto int socket_write(resource socket, string buf[, int length])
   Writes the buffer to the socket resource, length is optional */
PHP_FUNCTION(socket_write)
{
	zval		*arg1;
	php_socket	*php_sock;
	int			retval;
	size_t      str_len;
	zend_long	length = 0;
	char		*str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|l", &arg1, &str, &str_len, &length) == FAILURE) {
		return;
	}

	if (length < 0) {
		php_error_docref(NULL, E_WARNING, "Length cannot be negative");
		RETURN_FALSE;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() < 3) {
		length = str_len;
	}

#ifndef PHP_WIN32
	retval = write(php_sock->bsd_socket, str, MIN(length, str_len));
#else
	retval = send(php_sock->bsd_socket, str, min(length, str_len), 0);
#endif

	if (retval < 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to write to socket", errno);
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto string socket_read(resource socket, int length [, int type])
   Reads a maximum of length bytes from socket */
PHP_FUNCTION(socket_read)
{
	zval		*arg1;
	php_socket	*php_sock;
	zend_string	*tmpbuf;
	int			retval;
	zend_long		length, type = PHP_BINARY_READ;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl|l", &arg1, &length, &type) == FAILURE) {
		return;
	}

	/* overflow check */
	if ((length + 1) < 2) {
		RETURN_FALSE;
	}

	tmpbuf = zend_string_alloc(length, 0);

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	if (type == PHP_NORMAL_READ) {
		retval = php_read(php_sock, ZSTR_VAL(tmpbuf), length, 0);
	} else {
		retval = recv(php_sock->bsd_socket, ZSTR_VAL(tmpbuf), length, 0);
	}

	if (retval == -1) {
		/* if the socket is in non-blocking mode and there's no data to read,
		don't output any error, as this is a normal situation, and not an error */
		if (errno == EAGAIN
#ifdef EWOULDBLOCK
		|| errno == EWOULDBLOCK
#endif
		) {
			php_sock->error = errno;
			SOCKETS_G(last_error) = errno;
		} else {
			PHP_SOCKET_ERROR(php_sock, "unable to read from socket", errno);
		}

		zend_string_efree(tmpbuf);
		RETURN_FALSE;
	} else if (!retval) {
		zend_string_efree(tmpbuf);
		RETURN_EMPTY_STRING();
	}

	tmpbuf = zend_string_truncate(tmpbuf, retval, 0);
	ZSTR_LEN(tmpbuf) = retval;
	ZSTR_VAL(tmpbuf)[ZSTR_LEN(tmpbuf)] = '\0' ;

	RETURN_NEW_STR(tmpbuf);
}
/* }}} */

/* {{{ proto bool socket_getsockname(resource socket, string &addr[, int &port])
   Queries the remote side of the given socket which may either result in host/port or in a UNIX filesystem path, dependent on its type. */
PHP_FUNCTION(socket_getsockname)
{
	zval					*arg1, *addr, *port = NULL;
	php_sockaddr_storage	sa_storage;
	php_socket				*php_sock;
	struct sockaddr			*sa;
	struct sockaddr_in		*sin;
#if HAVE_IPV6
	struct sockaddr_in6		*sin6;
	char					addr6[INET6_ADDRSTRLEN+1];
#endif
	struct sockaddr_un		*s_un;
	char					*addr_string;
	socklen_t				salen = sizeof(php_sockaddr_storage);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz|z", &arg1, &addr, &port) == FAILURE) {
		return;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	sa = (struct sockaddr *) &sa_storage;

	if (getsockname(php_sock->bsd_socket, sa, &salen) != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to retrieve socket name", errno);
		RETURN_FALSE;
	}

	switch (sa->sa_family) {
#if HAVE_IPV6
		case AF_INET6:
			sin6 = (struct sockaddr_in6 *) sa;
			inet_ntop(AF_INET6, &sin6->sin6_addr, addr6, INET6_ADDRSTRLEN);
			ZEND_TRY_ASSIGN_REF_STRING(addr, addr6);

			if (port != NULL) {
				ZEND_TRY_ASSIGN_REF_LONG(port, htons(sin6->sin6_port));
			}
			RETURN_TRUE;
			break;
#endif
		case AF_INET:
			sin = (struct sockaddr_in *) sa;
			while (inet_ntoa_lock == 1);
			inet_ntoa_lock = 1;
			addr_string = inet_ntoa(sin->sin_addr);
			inet_ntoa_lock = 0;

			ZEND_TRY_ASSIGN_REF_STRING(addr, addr_string);

			if (port != NULL) {
				ZEND_TRY_ASSIGN_REF_LONG(port, htons(sin->sin_port));
			}
			RETURN_TRUE;
			break;

		case AF_UNIX:
			s_un = (struct sockaddr_un *) sa;

			ZEND_TRY_ASSIGN_REF_STRING(addr, s_un->sun_path);
			RETURN_TRUE;
			break;

		default:
			php_error_docref(NULL, E_WARNING, "Unsupported address family %d", sa->sa_family);
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool socket_getpeername(resource socket, string &addr[, int &port])
   Queries the remote side of the given socket which may either result in host/port or in a UNIX filesystem path, dependent on its type. */
PHP_FUNCTION(socket_getpeername)
{
	zval					*arg1, *arg2, *arg3 = NULL;
	php_sockaddr_storage	sa_storage;
	php_socket				*php_sock;
	struct sockaddr			*sa;
	struct sockaddr_in		*sin;
#if HAVE_IPV6
	struct sockaddr_in6		*sin6;
	char					addr6[INET6_ADDRSTRLEN+1];
#endif
	struct sockaddr_un		*s_un;
	char					*addr_string;
	socklen_t				salen = sizeof(php_sockaddr_storage);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rz|z", &arg1, &arg2, &arg3) == FAILURE) {
		return;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	sa = (struct sockaddr *) &sa_storage;

	if (getpeername(php_sock->bsd_socket, sa, &salen) < 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to retrieve peer name", errno);
		RETURN_FALSE;
	}

	switch (sa->sa_family) {
#if HAVE_IPV6
		case AF_INET6:
			sin6 = (struct sockaddr_in6 *) sa;
			inet_ntop(AF_INET6, &sin6->sin6_addr, addr6, INET6_ADDRSTRLEN);

			ZEND_TRY_ASSIGN_REF_STRING(arg2, addr6);

			if (arg3 != NULL) {
				ZEND_TRY_ASSIGN_REF_LONG(arg3, htons(sin6->sin6_port));
			}

			RETURN_TRUE;
			break;
#endif
		case AF_INET:
			sin = (struct sockaddr_in *) sa;
			while (inet_ntoa_lock == 1);
			inet_ntoa_lock = 1;
			addr_string = inet_ntoa(sin->sin_addr);
			inet_ntoa_lock = 0;

			ZEND_TRY_ASSIGN_REF_STRING(arg2, addr_string);

			if (arg3 != NULL) {
				ZEND_TRY_ASSIGN_REF_LONG(arg3, htons(sin->sin_port));
			}

			RETURN_TRUE;
			break;

		case AF_UNIX:
			s_un = (struct sockaddr_un *) sa;

			ZEND_TRY_ASSIGN_REF_STRING(arg2, s_un->sun_path);
			RETURN_TRUE;
			break;

		default:
			php_error_docref(NULL, E_WARNING, "Unsupported address family %d", sa->sa_family);
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto resource socket_create(int domain, int type, int protocol)
   Creates an endpoint for communication in the domain specified by domain, of type specified by type */
PHP_FUNCTION(socket_create)
{
	zend_long		arg1, arg2, arg3;
	php_socket	*php_sock = php_create_socket();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lll", &arg1, &arg2, &arg3) == FAILURE) {
		efree(php_sock);
		return;
	}

	if (arg1 != AF_UNIX
#if HAVE_IPV6
		&& arg1 != AF_INET6
#endif
		&& arg1 != AF_INET) {
		php_error_docref(NULL, E_WARNING, "invalid socket domain [" ZEND_LONG_FMT "] specified for argument 1, assuming AF_INET", arg1);
		arg1 = AF_INET;
	}

	if (arg2 > 10) {
		php_error_docref(NULL, E_WARNING, "invalid socket type [" ZEND_LONG_FMT "] specified for argument 2, assuming SOCK_STREAM", arg2);
		arg2 = SOCK_STREAM;
	}

	php_sock->bsd_socket = socket(arg1, arg2, arg3);
	php_sock->type = arg1;

	if (IS_INVALID_SOCKET(php_sock)) {
		SOCKETS_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "Unable to create socket [%d]: %s", errno, sockets_strerror(errno));
		efree(php_sock);
		RETURN_FALSE;
	}

	php_sock->error = 0;
	php_sock->blocking = 1;

	RETURN_RES(zend_register_resource(php_sock, le_socket));
}
/* }}} */

/* {{{ proto bool socket_connect(resource socket, string addr [, int port])
   Opens a connection to addr:port on the socket specified by socket */
PHP_FUNCTION(socket_connect)
{
	zval				*arg1;
	php_socket			*php_sock;
	char				*addr;
	int					retval;
	size_t              addr_len;
	zend_long				port = 0;
	int					argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rs|l", &arg1, &addr, &addr_len, &port) == FAILURE) {
		return;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	switch(php_sock->type) {
#if HAVE_IPV6
		case AF_INET6: {
			struct sockaddr_in6 sin6 = {0};

			if (argc != 3) {
				php_error_docref(NULL, E_WARNING, "Socket of type AF_INET6 requires 3 arguments");
				RETURN_FALSE;
			}

			memset(&sin6, 0, sizeof(struct sockaddr_in6));

			sin6.sin6_family = AF_INET6;
			sin6.sin6_port   = htons((unsigned short int)port);

			if (! php_set_inet6_addr(&sin6, addr, php_sock)) {
				RETURN_FALSE;
			}

			retval = connect(php_sock->bsd_socket, (struct sockaddr *)&sin6, sizeof(struct sockaddr_in6));
			break;
		}
#endif
		case AF_INET: {
			struct sockaddr_in sin = {0};

			if (argc != 3) {
				php_error_docref(NULL, E_WARNING, "Socket of type AF_INET requires 3 arguments");
				RETURN_FALSE;
			}

			sin.sin_family = AF_INET;
			sin.sin_port   = htons((unsigned short int)port);

			if (! php_set_inet_addr(&sin, addr, php_sock)) {
				RETURN_FALSE;
			}

			retval = connect(php_sock->bsd_socket, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
			break;
		}

		case AF_UNIX: {
			struct sockaddr_un s_un = {0};

			if (addr_len >= sizeof(s_un.sun_path)) {
				php_error_docref(NULL, E_WARNING, "Path too long");
				RETURN_FALSE;
			}

			s_un.sun_family = AF_UNIX;
			memcpy(&s_un.sun_path, addr, addr_len);
			retval = connect(php_sock->bsd_socket, (struct sockaddr *) &s_un,
				(socklen_t)(XtOffsetOf(struct sockaddr_un, sun_path) + addr_len));
			break;
		}

		default:
			php_error_docref(NULL, E_WARNING, "Unsupported socket type %d", php_sock->type);
			RETURN_FALSE;
		}

	if (retval != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to connect", errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string socket_strerror(int errno)
   Returns a string describing an error */
PHP_FUNCTION(socket_strerror)
{
	zend_long	arg1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &arg1) == FAILURE) {
		return;
	}

	RETURN_STRING(sockets_strerror(arg1));
}
/* }}} */

/* {{{ proto bool socket_bind(resource socket, string addr [, int port])
   Binds an open socket to a listening port, port is only specified in AF_INET family. */
PHP_FUNCTION(socket_bind)
{
	zval					*arg1;
	php_sockaddr_storage	sa_storage = {0};
	struct sockaddr			*sock_type = (struct sockaddr*) &sa_storage;
	php_socket				*php_sock;
	char					*addr;
	size_t						addr_len;
	zend_long					port = 0;
	zend_long					retval = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|l", &arg1, &addr, &addr_len, &port) == FAILURE) {
		return;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	switch(php_sock->type) {
		case AF_UNIX:
			{
				struct sockaddr_un *sa = (struct sockaddr_un *) sock_type;

				sa->sun_family = AF_UNIX;

				if (addr_len >= sizeof(sa->sun_path)) {
					php_error_docref(NULL, E_WARNING,
							"Invalid path: too long (maximum size is %d)",
							(int)sizeof(sa->sun_path) - 1);
					RETURN_FALSE;
				}
				memcpy(&sa->sun_path, addr, addr_len);

				retval = bind(php_sock->bsd_socket, (struct sockaddr *) sa,
						offsetof(struct sockaddr_un, sun_path) + addr_len);
				break;
			}

		case AF_INET:
			{
				struct sockaddr_in *sa = (struct sockaddr_in *) sock_type;

				sa->sin_family = AF_INET;
				sa->sin_port = htons((unsigned short) port);

				if (! php_set_inet_addr(sa, addr, php_sock)) {
					RETURN_FALSE;
				}

				retval = bind(php_sock->bsd_socket, (struct sockaddr *)sa, sizeof(struct sockaddr_in));
				break;
			}
#if HAVE_IPV6
		case AF_INET6:
			{
				struct sockaddr_in6 *sa = (struct sockaddr_in6 *) sock_type;

				sa->sin6_family = AF_INET6;
				sa->sin6_port = htons((unsigned short) port);

				if (! php_set_inet6_addr(sa, addr, php_sock)) {
					RETURN_FALSE;
				}

				retval = bind(php_sock->bsd_socket, (struct sockaddr *)sa, sizeof(struct sockaddr_in6));
				break;
			}
#endif
		default:
			php_error_docref(NULL, E_WARNING, "unsupported socket type '%d', must be AF_UNIX, AF_INET, or AF_INET6", php_sock->type);
			RETURN_FALSE;
	}

	if (retval != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to bind address", errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int socket_recv(resource socket, string &buf, int len, int flags)
   Receives data from a connected socket */
PHP_FUNCTION(socket_recv)
{
	zval		*php_sock_res, *buf;
	zend_string	*recv_buf;
	php_socket	*php_sock;
	int			retval;
	zend_long		len, flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rzll", &php_sock_res, &buf, &len, &flags) == FAILURE) {
		return;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(php_sock_res), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	/* overflow check */
	if ((len + 1) < 2) {
		RETURN_FALSE;
	}

	recv_buf = zend_string_alloc(len, 0);

	if ((retval = recv(php_sock->bsd_socket, ZSTR_VAL(recv_buf), len, flags)) < 1) {
		zend_string_efree(recv_buf);
		ZEND_TRY_ASSIGN_REF_NULL(buf);
	} else {
		ZSTR_LEN(recv_buf) = retval;
		ZSTR_VAL(recv_buf)[ZSTR_LEN(recv_buf)] = '\0';
		ZEND_TRY_ASSIGN_REF_NEW_STR(buf, recv_buf);
	}

	if (retval == -1) {
		PHP_SOCKET_ERROR(php_sock, "unable to read from socket", errno);
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int socket_send(resource socket, string buf, int len, int flags)
   Sends data to a connected socket */
PHP_FUNCTION(socket_send)
{
	zval		*arg1;
	php_socket	*php_sock;
	size_t			buf_len, retval;
	zend_long		len, flags;
	char		*buf;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rsll", &arg1, &buf, &buf_len, &len, &flags) == FAILURE) {
		return;
	}

	if (len < 0) {
		php_error_docref(NULL, E_WARNING, "Length cannot be negative");
		RETURN_FALSE;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	retval = send(php_sock->bsd_socket, buf, (buf_len < (size_t)len ? buf_len : (size_t)len), flags);

	if (retval == (size_t)-1) {
		PHP_SOCKET_ERROR(php_sock, "unable to write to socket", errno);
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int socket_recvfrom(resource socket, string &buf, int len, int flags, string &name [, int &port])
   Receives data from a socket, connected or not */
PHP_FUNCTION(socket_recvfrom)
{
	zval				*arg1, *arg2, *arg5, *arg6 = NULL;
	php_socket			*php_sock;
	struct sockaddr_un	s_un;
	struct sockaddr_in	sin;
#if HAVE_IPV6
	struct sockaddr_in6	sin6;
	char				addr6[INET6_ADDRSTRLEN];
#endif
	socklen_t			slen;
	int					retval;
	zend_long				arg3, arg4;
	char				*address;
	zend_string			*recv_buf;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rzllz|z", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		return;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	/* overflow check */
	if ((arg3 + 2) < 3) {
		RETURN_FALSE;
	}

	recv_buf = zend_string_alloc(arg3 + 1, 0);

	switch (php_sock->type) {
		case AF_UNIX:
			slen = sizeof(s_un);
			memset(&s_un, 0, slen);
			s_un.sun_family = AF_UNIX;

			retval = recvfrom(php_sock->bsd_socket, ZSTR_VAL(recv_buf), arg3, arg4, (struct sockaddr *)&s_un, (socklen_t *)&slen);

			if (retval < 0) {
				PHP_SOCKET_ERROR(php_sock, "unable to recvfrom", errno);
				zend_string_efree(recv_buf);
				RETURN_FALSE;
			}
			ZSTR_LEN(recv_buf) = retval;
			ZSTR_VAL(recv_buf)[ZSTR_LEN(recv_buf)] = '\0';

			ZEND_TRY_ASSIGN_REF_NEW_STR(arg2, recv_buf);
			ZEND_TRY_ASSIGN_REF_STRING(arg5, s_un.sun_path);
			break;

		case AF_INET:
			slen = sizeof(sin);
			memset(&sin, 0, slen);
			sin.sin_family = AF_INET;

			if (arg6 == NULL) {
				zend_string_efree(recv_buf);
				WRONG_PARAM_COUNT;
			}

			retval = recvfrom(php_sock->bsd_socket, ZSTR_VAL(recv_buf), arg3, arg4, (struct sockaddr *)&sin, (socklen_t *)&slen);

			if (retval < 0) {
				PHP_SOCKET_ERROR(php_sock, "unable to recvfrom", errno);
				zend_string_efree(recv_buf);
				RETURN_FALSE;
			}
			ZSTR_LEN(recv_buf) = retval;
			ZSTR_VAL(recv_buf)[ZSTR_LEN(recv_buf)] = '\0';

			address = inet_ntoa(sin.sin_addr);

			ZEND_TRY_ASSIGN_REF_NEW_STR(arg2, recv_buf);
			ZEND_TRY_ASSIGN_REF_STRING(arg5, address ? address : "0.0.0.0");
			ZEND_TRY_ASSIGN_REF_LONG(arg6, ntohs(sin.sin_port));
			break;
#if HAVE_IPV6
		case AF_INET6:
			slen = sizeof(sin6);
			memset(&sin6, 0, slen);
			sin6.sin6_family = AF_INET6;

			if (arg6 == NULL) {
				zend_string_efree(recv_buf);
				WRONG_PARAM_COUNT;
			}

			retval = recvfrom(php_sock->bsd_socket, ZSTR_VAL(recv_buf), arg3, arg4, (struct sockaddr *)&sin6, (socklen_t *)&slen);

			if (retval < 0) {
				PHP_SOCKET_ERROR(php_sock, "unable to recvfrom", errno);
				zend_string_efree(recv_buf);
				RETURN_FALSE;
			}
			ZSTR_LEN(recv_buf) = retval;
			ZSTR_VAL(recv_buf)[ZSTR_LEN(recv_buf)] = '\0';

			memset(addr6, 0, INET6_ADDRSTRLEN);
			inet_ntop(AF_INET6, &sin6.sin6_addr, addr6, INET6_ADDRSTRLEN);

			ZEND_TRY_ASSIGN_REF_NEW_STR(arg2, recv_buf);
			ZEND_TRY_ASSIGN_REF_STRING(arg5, addr6[0] ? addr6 : "::");
			ZEND_TRY_ASSIGN_REF_LONG(arg6, ntohs(sin6.sin6_port));
			break;
#endif
		default:
			php_error_docref(NULL, E_WARNING, "Unsupported socket type %d", php_sock->type);
			RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int socket_sendto(resource socket, string buf, int len, int flags, string addr [, int port])
   Sends a message to a socket, whether it is connected or not */
PHP_FUNCTION(socket_sendto)
{
	zval				*arg1;
	php_socket			*php_sock;
	struct sockaddr_un	s_un;
	struct sockaddr_in	sin;
#if HAVE_IPV6
	struct sockaddr_in6	sin6;
#endif
	int					retval;
	size_t              buf_len, addr_len;
	zend_long			len, flags, port = 0;
	char				*buf, *addr;
	int					argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rslls|l", &arg1, &buf, &buf_len, &len, &flags, &addr, &addr_len, &port) == FAILURE) {
		return;
	}

	if (len < 0) {
		php_error_docref(NULL, E_WARNING, "Length cannot be negative");
		RETURN_FALSE;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	switch (php_sock->type) {
		case AF_UNIX:
			memset(&s_un, 0, sizeof(s_un));
			s_un.sun_family = AF_UNIX;
			snprintf(s_un.sun_path, 108, "%s", addr);

			retval = sendto(php_sock->bsd_socket, buf, ((size_t)len > buf_len) ? buf_len : (size_t)len,	flags, (struct sockaddr *) &s_un, SUN_LEN(&s_un));
			break;

		case AF_INET:
			if (argc != 6) {
				WRONG_PARAM_COUNT;
			}

			memset(&sin, 0, sizeof(sin));
			sin.sin_family = AF_INET;
			sin.sin_port = htons((unsigned short) port);

			if (! php_set_inet_addr(&sin, addr, php_sock)) {
				RETURN_FALSE;
			}

			retval = sendto(php_sock->bsd_socket, buf, ((size_t)len > buf_len) ? buf_len : (size_t)len, flags, (struct sockaddr *) &sin, sizeof(sin));
			break;
#if HAVE_IPV6
		case AF_INET6:
			if (argc != 6) {
				WRONG_PARAM_COUNT;
			}

			memset(&sin6, 0, sizeof(sin6));
			sin6.sin6_family = AF_INET6;
			sin6.sin6_port = htons((unsigned short) port);

			if (! php_set_inet6_addr(&sin6, addr, php_sock)) {
				RETURN_FALSE;
			}

			retval = sendto(php_sock->bsd_socket, buf, ((size_t)len > buf_len) ? buf_len : (size_t)len, flags, (struct sockaddr *) &sin6, sizeof(sin6));
			break;
#endif
		default:
			php_error_docref(NULL, E_WARNING, "Unsupported socket type %d", php_sock->type);
			RETURN_FALSE;
	}

	if (retval == -1) {
		PHP_SOCKET_ERROR(php_sock, "unable to write to socket", errno);
		RETURN_FALSE;
	}

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto mixed socket_get_option(resource socket, int level, int optname)
   Gets socket options for the socket */
PHP_FUNCTION(socket_get_option)
{
	zval			*arg1;
	struct linger	linger_val;
	struct timeval	tv;
#ifdef PHP_WIN32
	int				timeout = 0;
#endif
	socklen_t		optlen;
	php_socket		*php_sock;
	int				other_val;
	zend_long			level, optname;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rll", &arg1, &level, &optname) == FAILURE) {
		return;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	if (level == IPPROTO_IP) {
		switch (optname) {
		case IP_MULTICAST_IF: {
			struct in_addr if_addr;
			unsigned int if_index;
			optlen = sizeof(if_addr);
			if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&if_addr, &optlen) != 0) {
				PHP_SOCKET_ERROR(php_sock, "unable to retrieve socket option", errno);
				RETURN_FALSE;
			}
			if (php_add4_to_if_index(&if_addr, php_sock, &if_index) == SUCCESS) {
				RETURN_LONG((zend_long) if_index);
			} else {
				RETURN_FALSE;
			}
		}
		}
	}
#if HAVE_IPV6
	else if (level == IPPROTO_IPV6) {
		int ret = php_do_getsockopt_ipv6_rfc3542(php_sock, level, optname, return_value);
		if (ret == SUCCESS) {
			return;
		} else if (ret == FAILURE) {
			RETURN_FALSE;
		} /* else continue */
	}
#endif

	if (level == SOL_SOCKET) {
		switch (optname) {
			case SO_LINGER:
				optlen = sizeof(linger_val);

				if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&linger_val, &optlen) != 0) {
					PHP_SOCKET_ERROR(php_sock, "unable to retrieve socket option", errno);
					RETURN_FALSE;
				}

				array_init(return_value);
				add_assoc_long(return_value, "l_onoff", linger_val.l_onoff);
				add_assoc_long(return_value, "l_linger", linger_val.l_linger);
				return;

			case SO_RCVTIMEO:
			case SO_SNDTIMEO:
#ifndef PHP_WIN32
				optlen = sizeof(tv);

				if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&tv, &optlen) != 0) {
					PHP_SOCKET_ERROR(php_sock, "unable to retrieve socket option", errno);
					RETURN_FALSE;
				}
#else
				optlen = sizeof(int);

				if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&timeout, &optlen) != 0) {
					PHP_SOCKET_ERROR(php_sock, "unable to retrieve socket option", errno);
					RETURN_FALSE;
				}

				tv.tv_sec = timeout ? timeout / 1000 : 0;
				tv.tv_usec = timeout ? (timeout * 1000) % 1000000 : 0;
#endif

				array_init(return_value);

				add_assoc_long(return_value, "sec", tv.tv_sec);
				add_assoc_long(return_value, "usec", tv.tv_usec);
				return;
		}
	}

	optlen = sizeof(other_val);

	if (getsockopt(php_sock->bsd_socket, level, optname, (char*)&other_val, &optlen) != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to retrieve socket option", errno);
		RETURN_FALSE;
	}

	if (optlen == 1) {
		other_val = *((unsigned char *)&other_val);
	}

	RETURN_LONG(other_val);
}
/* }}} */

/* {{{ proto bool socket_set_option(resource socket, int level, int optname, int|array optval)
   Sets socket options for the socket */
PHP_FUNCTION(socket_set_option)
{
	zval					*arg1, *arg4;
	struct linger			lv;
	php_socket				*php_sock;
	int						ov, optlen, retval;
#ifdef PHP_WIN32
	int						timeout;
#else
	struct					timeval tv;
#endif
	zend_long					level, optname;
	void 					*opt_ptr;
	HashTable		 		*opt_ht;
	zval 					*l_onoff, *l_linger;
	zval		 			*sec, *usec;


	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rllz", &arg1, &level, &optname, &arg4) == FAILURE) {
		return;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	set_errno(0);

#define HANDLE_SUBCALL(res) \
	do { \
		if (res == 1) { goto default_case; } \
		else if (res == SUCCESS) { RETURN_TRUE; } \
		else { RETURN_FALSE; } \
	} while (0)


	if (level == IPPROTO_IP) {
		int res = php_do_setsockopt_ip_mcast(php_sock, level, optname, arg4);
		HANDLE_SUBCALL(res);
	}

#if HAVE_IPV6
	else if (level == IPPROTO_IPV6) {
		int res = php_do_setsockopt_ipv6_mcast(php_sock, level, optname, arg4);
		if (res == 1) {
			res = php_do_setsockopt_ipv6_rfc3542(php_sock, level, optname, arg4);
		}
		HANDLE_SUBCALL(res);
	}
#endif

	switch (optname) {
		case SO_LINGER: {
			const char l_onoff_key[] = "l_onoff";
			const char l_linger_key[] = "l_linger";

			convert_to_array_ex(arg4);
			opt_ht = Z_ARRVAL_P(arg4);

			if ((l_onoff = zend_hash_str_find(opt_ht, l_onoff_key, sizeof(l_onoff_key) - 1)) == NULL) {
				php_error_docref(NULL, E_WARNING, "no key \"%s\" passed in optval", l_onoff_key);
				RETURN_FALSE;
			}
			if ((l_linger = zend_hash_str_find(opt_ht, l_linger_key, sizeof(l_linger_key) - 1)) == NULL) {
				php_error_docref(NULL, E_WARNING, "no key \"%s\" passed in optval", l_linger_key);
				RETURN_FALSE;
			}

			convert_to_long_ex(l_onoff);
			convert_to_long_ex(l_linger);

			lv.l_onoff = (unsigned short)Z_LVAL_P(l_onoff);
			lv.l_linger = (unsigned short)Z_LVAL_P(l_linger);

			optlen = sizeof(lv);
			opt_ptr = &lv;
			break;
		}

		case SO_RCVTIMEO:
		case SO_SNDTIMEO: {
			const char sec_key[] = "sec";
			const char usec_key[] = "usec";

			convert_to_array_ex(arg4);
			opt_ht = Z_ARRVAL_P(arg4);

			if ((sec = zend_hash_str_find(opt_ht, sec_key, sizeof(sec_key) - 1)) == NULL) {
				php_error_docref(NULL, E_WARNING, "no key \"%s\" passed in optval", sec_key);
				RETURN_FALSE;
			}
			if ((usec = zend_hash_str_find(opt_ht, usec_key, sizeof(usec_key) - 1)) == NULL) {
				php_error_docref(NULL, E_WARNING, "no key \"%s\" passed in optval", usec_key);
				RETURN_FALSE;
			}

			convert_to_long_ex(sec);
			convert_to_long_ex(usec);
#ifndef PHP_WIN32
			tv.tv_sec = Z_LVAL_P(sec);
			tv.tv_usec = Z_LVAL_P(usec);
			optlen = sizeof(tv);
			opt_ptr = &tv;
#else
			timeout = Z_LVAL_P(sec) * 1000 + Z_LVAL_P(usec) / 1000;
			optlen = sizeof(int);
			opt_ptr = &timeout;
#endif
			break;
		}
#ifdef SO_BINDTODEVICE
		case SO_BINDTODEVICE: {
			if (Z_TYPE_P(arg4) == IS_STRING) {
				opt_ptr = Z_STRVAL_P(arg4);
				optlen = Z_STRLEN_P(arg4);
			} else {
				opt_ptr = "";
				optlen = 0;
			}
			break;
		}
#endif

		default:
default_case:
			convert_to_long_ex(arg4);
			ov = Z_LVAL_P(arg4);

			optlen = sizeof(ov);
			opt_ptr = &ov;
			break;
	}

	retval = setsockopt(php_sock->bsd_socket, level, optname, opt_ptr, optlen);
	if (retval != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to set socket option", errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

#ifdef HAVE_SOCKETPAIR
/* {{{ proto bool socket_create_pair(int domain, int type, int protocol, array &fd)
   Creates a pair of indistinguishable sockets and stores them in fds. */
PHP_FUNCTION(socket_create_pair)
{
	zval		retval[2], *fds_array_zval;
	php_socket	*php_sock[2];
	PHP_SOCKET	fds_array[2];
	zend_long		domain, type, protocol;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lllz", &domain, &type, &protocol, &fds_array_zval) == FAILURE) {
		return;
	}

	php_sock[0] = php_create_socket();
	php_sock[1] = php_create_socket();

	if (domain != AF_INET
#if HAVE_IPV6
		&& domain != AF_INET6
#endif
		&& domain != AF_UNIX) {
		php_error_docref(NULL, E_WARNING, "invalid socket domain [" ZEND_LONG_FMT "] specified for argument 1, assuming AF_INET", domain);
		domain = AF_INET;
	}

	if (type > 10) {
		php_error_docref(NULL, E_WARNING, "invalid socket type [" ZEND_LONG_FMT "] specified for argument 2, assuming SOCK_STREAM", type);
		type = SOCK_STREAM;
	}

	if (socketpair(domain, type, protocol, fds_array) != 0) {
		SOCKETS_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "unable to create socket pair [%d]: %s", errno, sockets_strerror(errno));
		efree(php_sock[0]);
		efree(php_sock[1]);
		RETURN_FALSE;
	}

	fds_array_zval = zend_try_array_init(fds_array_zval);
	if (!fds_array_zval) {
		efree(php_sock[0]);
		efree(php_sock[1]);
		return;
	}

	php_sock[0]->bsd_socket = fds_array[0];
	php_sock[1]->bsd_socket = fds_array[1];
	php_sock[0]->type		= domain;
	php_sock[1]->type		= domain;
	php_sock[0]->error		= 0;
	php_sock[1]->error		= 0;
	php_sock[0]->blocking	= 1;
	php_sock[1]->blocking	= 1;

	ZVAL_RES(&retval[0], zend_register_resource(php_sock[0], le_socket));
	ZVAL_RES(&retval[1], zend_register_resource(php_sock[1], le_socket));

	add_index_zval(fds_array_zval, 0, &retval[0]);
	add_index_zval(fds_array_zval, 1, &retval[1]);

	RETURN_TRUE;
}
/* }}} */
#endif

#ifdef HAVE_SHUTDOWN
/* {{{ proto bool socket_shutdown(resource socket[, int how])
   Shuts down a socket for receiving, sending, or both. */
PHP_FUNCTION(socket_shutdown)
{
	zval		*arg1;
	zend_long		how_shutdown = 2;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &arg1, &how_shutdown) == FAILURE) {
		return;
	}

	if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	if (shutdown(php_sock->bsd_socket, how_shutdown) != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to shutdown socket", errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto int socket_last_error([resource socket])
   Returns the last socket error (either the last used or the provided socket resource) */
PHP_FUNCTION(socket_last_error)
{
	zval		*arg1 = NULL;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|r", &arg1) == FAILURE) {
		return;
	}

	if (arg1) {
		if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
			RETURN_FALSE;
		}
		RETVAL_LONG(php_sock->error);
	} else {
		RETVAL_LONG(SOCKETS_G(last_error));
	}
}
/* }}} */

/* {{{ proto void socket_clear_error([resource socket])
   Clears the error on the socket or the last error code. */
PHP_FUNCTION(socket_clear_error)
{
	zval		*arg1 = NULL;
	php_socket	*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|r", &arg1) == FAILURE) {
		return;
	}

	if (arg1) {
		if ((php_sock = (php_socket *)zend_fetch_resource(Z_RES_P(arg1), le_socket_name, le_socket)) == NULL) {
			RETURN_FALSE;
		}
		php_sock->error = 0;
	} else {
		SOCKETS_G(last_error) = 0;
	}

	return;
}
/* }}} */

php_socket *socket_import_file_descriptor(PHP_SOCKET socket)
{
#ifdef SO_DOMAIN
	int						type;
	socklen_t				type_len = sizeof(type);
#endif
	php_socket 				*retsock;
	php_sockaddr_storage	addr;
	socklen_t				addr_len = sizeof(addr);
#ifndef PHP_WIN32
	int					 t;
#endif

    retsock = php_create_socket();
    retsock->bsd_socket = socket;

    /* determine family */
#ifdef SO_DOMAIN
    if (getsockopt(socket, SOL_SOCKET, SO_DOMAIN, &type, &type_len) == 0) {
		retsock->type = type;
	} else
#endif
	if (getsockname(socket, (struct sockaddr*)&addr, &addr_len) == 0) {
		retsock->type = addr.ss_family;
	} else {
		PHP_SOCKET_ERROR(retsock, "unable to obtain socket family", errno);
		goto error;
	}

    /* determine blocking mode */
#ifndef PHP_WIN32
    t = fcntl(socket, F_GETFL);
    if (t == -1) {
		PHP_SOCKET_ERROR(retsock, "unable to obtain blocking state", errno);
		goto error;
    } else {
    	retsock->blocking = !(t & O_NONBLOCK);
    }
#endif

    return retsock;

error:
	efree(retsock);
	return NULL;
}

/* {{{ proto resource socket_import_stream(resource stream)
   Imports a stream that encapsulates a socket into a socket extension resource. */
PHP_FUNCTION(socket_import_stream)
{
	zval				 *zstream;
	php_stream			 *stream;
	php_socket			 *retsock = NULL;
	PHP_SOCKET			 socket; /* fd */

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zstream) == FAILURE) {
		return;
	}
	php_stream_from_zval(stream, zstream);

	if (php_stream_cast(stream, PHP_STREAM_AS_SOCKETD, (void**)&socket, 1)) {
		/* error supposedly already shown */
		RETURN_FALSE;
	}

	retsock = socket_import_file_descriptor(socket);
	if (retsock == NULL) {
		RETURN_FALSE;
	}

#ifdef PHP_WIN32
	/* on windows, check if the stream is a socket stream and read its
	 * private data; otherwise assume it's in non-blocking mode */
	if (php_stream_is(stream, PHP_STREAM_IS_SOCKET)) {
		retsock->blocking =
				((php_netstream_data_t *)stream->abstract)->is_blocked;
	} else {
		retsock->blocking = 1;
	}
#endif

	/* hold a zval reference to the stream (holding a php_stream* directly could
	 * also be done, but this makes socket_export_stream a bit simpler) */
	ZVAL_COPY(&retsock->zstream, zstream);

	php_stream_set_option(stream, PHP_STREAM_OPTION_READ_BUFFER,
		PHP_STREAM_BUFFER_NONE, NULL);

	RETURN_RES(zend_register_resource(retsock, le_socket));
}
/* }}} */

/* {{{ proto resource socket_export_stream(resource socket)
   Exports a socket extension resource into a stream that encapsulates a socket. */
PHP_FUNCTION(socket_export_stream)
{
	zval *zsocket;
	php_socket *socket;
	php_stream *stream = NULL;
	php_netstream_data_t *stream_data;
	char *protocol = NULL;
	size_t protocollen = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zsocket) == FAILURE) {
		return;
	}
	if ((socket = (php_socket *) zend_fetch_resource(Z_RES_P(zsocket), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	/* Either we already exported a stream or the socket came from an import,
	 * just return the existing stream */
	if (!Z_ISUNDEF(socket->zstream)) {
		RETURN_ZVAL(&socket->zstream, 1, 0);
	}

	/* Determine if socket is using a protocol with one of the default registered
	 * socket stream wrappers */
	if (socket->type == PF_INET
#if HAVE_IPV6
		 || socket->type == PF_INET6
#endif
	) {
		int protoid;
		socklen_t protoidlen = sizeof(protoid);

		getsockopt(socket->bsd_socket, SOL_SOCKET, SO_TYPE, (char *) &protoid, &protoidlen);

		if (protoid == SOCK_STREAM) {
			/* SO_PROTOCOL is not (yet?) supported on OS X, so lets assume it's TCP there */
#ifdef SO_PROTOCOL
			protoidlen = sizeof(protoid);
			getsockopt(socket->bsd_socket, SOL_SOCKET, SO_PROTOCOL, (char *) &protoid, &protoidlen);
			if (protoid == IPPROTO_TCP)
#endif
			{
				protocol = "tcp";
				protocollen = 3;
			}
		} else if (protoid == SOCK_DGRAM) {
			protocol = "udp";
			protocollen = 3;
		}
#ifdef PF_UNIX
	} else if (socket->type == PF_UNIX) {
		int type;
		socklen_t typelen = sizeof(type);

		getsockopt(socket->bsd_socket, SOL_SOCKET, SO_TYPE, (char *) &type, &typelen);

		if (type == SOCK_STREAM) {
			protocol = "unix";
			protocollen = 4;
		} else if (type == SOCK_DGRAM) {
			protocol = "udg";
			protocollen = 3;
		}
#endif
	}

	/* Try to get a stream with the registered sockops for the protocol in use
	 * We don't want streams to actually *do* anything though, so don't give it
	 * anything apart from the protocol */
	if (protocol != NULL) {
		stream = php_stream_xport_create(protocol, protocollen, 0, 0, NULL, NULL, NULL, NULL, NULL);
	}

	/* Fall back to creating a generic socket stream */
	if (stream == NULL) {
		stream = php_stream_sock_open_from_socket(socket->bsd_socket, 0);

		if (stream == NULL) {
			php_error_docref(NULL, E_WARNING, "failed to create stream");
			RETURN_FALSE;
		}
	}

	stream_data = (php_netstream_data_t *) stream->abstract;
	stream_data->socket = socket->bsd_socket;
	stream_data->is_blocked = socket->blocking;
	stream_data->timeout.tv_sec = FG(default_socket_timeout);
	stream_data->timeout.tv_usec = 0;

	php_stream_to_zval(stream, &socket->zstream);

	RETURN_ZVAL(&socket->zstream, 1, 0);
}
/* }}} */

/* {{{ proto resource addrinfo socket_addrinfo_lookup(string hostname[, mixed service, array hints])
   Gets array with contents of getaddrinfo about the given hostname. */
PHP_FUNCTION(socket_addrinfo_lookup)
{
	char *service = NULL;
	size_t service_len;
	zend_string *hostname, *key;
	zval *hint, *zhints = NULL;

	struct addrinfo hints, *result, *rp, *res;

	memset(&hints, 0, sizeof(hints));

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|sa", &hostname, &service, &service_len, &zhints) == FAILURE) {
		RETURN_NULL();
	}

	if (zhints) {
		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(zhints), key, hint) {
			if (key) {
				if (zend_string_equals_literal(key, "ai_flags")) {
					zend_long flags = zval_get_long(hint);
#if HAVE_AI_IDN
					if (flags & (AI_IDN_ALLOW_UNASSIGNED | AI_IDN_USE_STD3_ASCII_RULES)) {
						php_error_docref(NULL, E_DEPRECATED,
							"AI_IDN_ALLOW_UNASSIGNED and AI_IDN_USE_STD3_ASCII_RULES are deprecated");
					}
#endif
					hints.ai_flags = flags;
				} else if (zend_string_equals_literal(key, "ai_socktype")) {
					hints.ai_socktype = zval_get_long(hint);
				} else if (zend_string_equals_literal(key, "ai_protocol")) {
					hints.ai_protocol = zval_get_long(hint);
				} else if (zend_string_equals_literal(key, "ai_family")) {
					hints.ai_family = zval_get_long(hint);
				} else {
					php_error_docref(NULL, E_NOTICE, "Unknown hint %s", ZSTR_VAL(key));
				}
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (getaddrinfo(ZSTR_VAL(hostname), service, &hints, &result) != 0) {
		RETURN_FALSE;
	}

	array_init(return_value);

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		if (rp->ai_family != AF_UNSPEC) {
			res = emalloc(sizeof(struct addrinfo));
			memcpy(res, rp, sizeof(struct addrinfo));

			res->ai_addr = emalloc(rp->ai_addrlen);
			memcpy(res->ai_addr, rp->ai_addr, rp->ai_addrlen);

			if (rp->ai_canonname != NULL) {
				res->ai_canonname = estrdup(rp->ai_canonname);
			}

			add_next_index_resource(return_value, zend_register_resource(res, le_addrinfo));
		}
	}

	freeaddrinfo(result);
}
/* }}} */

/* {{{ proto resource socket_addrinfo_bind(resource addrinfo)
   Creates and binds to a socket from a given addrinfo resource */
PHP_FUNCTION(socket_addrinfo_bind)
{
	zval			*arg1;
	int				retval;
	struct addrinfo	*ai;
	php_socket		*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &arg1) == FAILURE) {
		return;
	}

	if ((ai = (struct addrinfo *) zend_fetch_resource(Z_RES_P(arg1), le_addrinfo_name, le_addrinfo)) == NULL) {
		RETURN_FALSE;
	}

	php_sock = php_create_socket();
	php_sock->bsd_socket = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	php_sock->type = ai->ai_family;

	if (IS_INVALID_SOCKET(php_sock)) {
		SOCKETS_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "Unable to create socket [%d]: %s", errno, sockets_strerror(errno));
		efree(php_sock);
		RETURN_FALSE;
	}

	php_sock->error = 0;
	php_sock->blocking = 1;

	switch(php_sock->type) {
		case AF_UNIX:
			{
				// AF_UNIX sockets via getaddrino are not implemented due to security problems
				close(php_sock->bsd_socket);
				efree(php_sock);
				RETURN_FALSE;
			}

		case AF_INET:
#if HAVE_IPV6
		case AF_INET6:
#endif
			{
				retval = bind(php_sock->bsd_socket, ai->ai_addr, ai->ai_addrlen);
				break;
			}
		default:
			php_error_docref(NULL, E_WARNING, "unsupported socket type '%d', must be AF_UNIX, AF_INET, or AF_INET6", php_sock->type);
			close(php_sock->bsd_socket);
			efree(php_sock);
			RETURN_FALSE;
	}

	if (retval != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to bind address", errno);
		close(php_sock->bsd_socket);
		efree(php_sock);
		RETURN_FALSE;
	}

	RETURN_RES(zend_register_resource(php_sock, le_socket));
}
/* }}} */

/* {{{ proto resource socket_addrinfo_connect(resource addrinfo)
   Creates and connects to a socket from a given addrinfo resource */
PHP_FUNCTION(socket_addrinfo_connect)
{
	zval			*arg1;
	int				retval;
	struct addrinfo	*ai;
	php_socket		*php_sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &arg1) == FAILURE) {
		return;
	}

	if ((ai = (struct addrinfo *) zend_fetch_resource(Z_RES_P(arg1), le_addrinfo_name, le_addrinfo)) == NULL) {
		RETURN_FALSE;
	}

	php_sock = php_create_socket();
	php_sock->bsd_socket = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	php_sock->type = ai->ai_family;

	if (IS_INVALID_SOCKET(php_sock)) {
		SOCKETS_G(last_error) = errno;
		php_error_docref(NULL, E_WARNING, "Unable to create socket [%d]: %s", errno, sockets_strerror(errno));
		efree(php_sock);
		RETURN_FALSE;
	}

	php_sock->error = 0;
	php_sock->blocking = 1;

	switch(php_sock->type) {
		case AF_UNIX:
			{
				// AF_UNIX sockets via getaddrino are not implemented due to security problems
				close(php_sock->bsd_socket);
				efree(php_sock);
				RETURN_FALSE;
			}

		case AF_INET:
#if HAVE_IPV6
		case AF_INET6:
#endif
			{
				retval = connect(php_sock->bsd_socket, ai->ai_addr, ai->ai_addrlen);
				break;
			}
		default:
			php_error_docref(NULL, E_WARNING, "unsupported socket type '%d', must be AF_UNIX, AF_INET, or AF_INET6", php_sock->type);
			close(php_sock->bsd_socket);
			efree(php_sock);
			RETURN_FALSE;
	}

	if (retval != 0) {
		PHP_SOCKET_ERROR(php_sock, "unable to connect address", errno);
		close(php_sock->bsd_socket);
		efree(php_sock);
		RETURN_FALSE;
	}

	RETURN_RES(zend_register_resource(php_sock, le_socket));
}
/* }}} */

/* {{{ proto resource socket_addrinfo_explain(resource addrinfo)
   Creates and connects to a socket from a given addrinfo resource */
PHP_FUNCTION(socket_addrinfo_explain)
{
	zval			*arg1, sockaddr;
	struct addrinfo	*ai;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &arg1) == FAILURE) {
		return;
	}

	if ((ai = (struct addrinfo *) zend_fetch_resource(Z_RES_P(arg1), le_addrinfo_name, le_addrinfo)) == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);

	add_assoc_long(return_value, "ai_flags", ai->ai_flags);
	add_assoc_long(return_value, "ai_family", ai->ai_family);
	add_assoc_long(return_value, "ai_socktype", ai->ai_socktype);
	add_assoc_long(return_value, "ai_protocol", ai->ai_protocol);
	if (ai->ai_canonname != NULL) {
		add_assoc_string(return_value, "ai_canonname", ai->ai_canonname);
	}

	array_init(&sockaddr);
	switch(ai->ai_family) {
		case AF_INET:
			{
				struct sockaddr_in *sa = (struct sockaddr_in *) ai->ai_addr;
				char addr[INET_ADDRSTRLEN];

				add_assoc_long(&sockaddr, "sin_port", ntohs((unsigned short) sa->sin_port));
				inet_ntop(ai->ai_family, &sa->sin_addr, addr, sizeof(addr));
				add_assoc_string(&sockaddr, "sin_addr", addr);
				break;
			}
#if HAVE_IPV6
		case AF_INET6:
			{
				struct sockaddr_in6 *sa = (struct sockaddr_in6 *) ai->ai_addr;
				char addr[INET6_ADDRSTRLEN];

				add_assoc_long(&sockaddr, "sin6_port", ntohs((unsigned short) sa->sin6_port));
				inet_ntop(ai->ai_family, &sa->sin6_addr, addr, sizeof(addr));
				add_assoc_string(&sockaddr, "sin6_addr", addr);
				break;
			}
#endif
	}

	add_assoc_zval(return_value, "ai_addr", &sockaddr);
}
/* }}} */

#ifdef PHP_WIN32

 /* {{{ proto string socket_wsaprotocol_info_export(resource stream, int target_pid)
   Exports the network socket information suitable to be used in another process and returns the info id. */
PHP_FUNCTION(socket_wsaprotocol_info_export)
{
	WSAPROTOCOL_INFO wi;
	zval *zsocket;
	php_socket *socket;
	zend_long target_pid;
	zend_string *seg_name;
	HANDLE map;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &zsocket, &target_pid) == FAILURE) {
		return;
	}
	if ((socket = (php_socket *) zend_fetch_resource(Z_RES_P(zsocket), le_socket_name, le_socket)) == NULL) {
		RETURN_FALSE;
	}

	if (SOCKET_ERROR == WSADuplicateSocket(socket->bsd_socket, (DWORD)target_pid, &wi)) {
		DWORD err = WSAGetLastError();
		char *buf = php_win32_error_to_msg(err);

		if (!buf[0]) {
			php_error_docref(NULL, E_WARNING, "Unable to export WSA protocol info [0x%08lx]", err);
		} else {
			php_error_docref(NULL, E_WARNING, "Unable to export WSA protocol info [0x%08lx]: %s", err, buf);
		}

		php_win32_error_msg_free(buf);

		RETURN_FALSE;
	}

	seg_name = zend_strpprintf(0, "php_wsa_for_%u", SOCKETS_G(wsa_child_count)++);
	map = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(WSAPROTOCOL_INFO), ZSTR_VAL(seg_name));
	if (NULL != map) {
		LPVOID view = MapViewOfFile(map, FILE_MAP_WRITE, 0, 0, 0);
		if (view) {
			memcpy(view, &wi, sizeof(wi));
			UnmapViewOfFile(view);
			zend_hash_add_ptr(&(SOCKETS_G(wsa_info)), seg_name, map);
			RETURN_STR(seg_name);
		} else {
			DWORD err = GetLastError();
			php_error_docref(NULL, E_WARNING, "Unable to map file view [0x%08lx]", err);
		}
	} else {
		DWORD err = GetLastError();
		php_error_docref(NULL, E_WARNING, "Unable to create file mapping [0x%08lx]", err);
	}
	zend_string_release_ex(seg_name, 0);

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto resource socket_wsaprotocol_info_import(string id)
   Imports the network socket information using the supplied id and creates a new socket on its base. */
PHP_FUNCTION(socket_wsaprotocol_info_import)
{
	char *id;
	size_t id_len;
	WSAPROTOCOL_INFO wi;
	PHP_SOCKET sock;
	php_socket	*php_sock;
	HANDLE map;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &id, &id_len) == FAILURE) {
		return;
	}

	map = OpenFileMapping(FILE_MAP_READ, FALSE, id);
	if (map) {
		LPVOID view = MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);
		if (view) {
			memcpy(&wi, view, sizeof(WSAPROTOCOL_INFO));
			UnmapViewOfFile(view);
		} else {
			DWORD err = GetLastError();
			php_error_docref(NULL, E_WARNING, "Unable to map file view [0x%08lx]", err);
			RETURN_FALSE;
		}
		CloseHandle(map);
	} else {
		DWORD err = GetLastError();
		php_error_docref(NULL, E_WARNING, "Unable to open file mapping [0x%08lx]", err);
		RETURN_FALSE;
	}

	sock = WSASocket(FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, &wi, 0, 0);
	if (INVALID_SOCKET == sock) {
		DWORD err = WSAGetLastError();
		char *buf = php_win32_error_to_msg(err);

		if (!buf[0]) {
			php_error_docref(NULL, E_WARNING, "Unable to import WSA protocol info [0x%08lx]", err);
		} else {
			php_error_docref(NULL, E_WARNING, "Unable to import WSA protocol info [0x%08lx]: %s", err, buf);
		}

		php_win32_error_msg_free(buf);

		RETURN_FALSE;
	}

	php_sock = php_create_socket();
	php_sock->bsd_socket = sock;
	php_sock->type = wi.iAddressFamily;
	php_sock->error = 0;
	php_sock->blocking = 1;

	RETURN_RES(zend_register_resource(php_sock, le_socket));
}
/* }}} */

/* {{{ proto bool socket_wsaprotocol_info_release(string id)
   Frees the exported info and corresponding resources using the supplied id. */
PHP_FUNCTION(socket_wsaprotocol_info_release)
{
	char *id;
	size_t id_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &id, &id_len) == FAILURE) {
		return;
	}

	RETURN_BOOL(SUCCESS == zend_hash_str_del(&(SOCKETS_G(wsa_info)), id, id_len));
}
/* }}} */
#endif
