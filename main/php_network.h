/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stig Venaas <venaas@uninett.no>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef _PHP_NETWORK_H
#define _PHP_NETWORK_H

#include <php.h>

#ifdef PHP_WIN32
# include "win32/inet.h"
#else
# undef closesocket
# define closesocket close
# include <netinet/tcp.h>
#endif

#ifndef HAVE_SHUTDOWN
#undef shutdown
#define shutdown(s,n)	/* nothing */
#endif

#ifdef PHP_WIN32
# ifdef EWOULDBLOCK
#  undef EWOULDBLOCK
# endif
# ifdef EINPROGRESS
#  undef EINPROGRESS
# endif
# define EWOULDBLOCK WSAEWOULDBLOCK
# define EINPROGRESS	WSAEWOULDBLOCK
# define fsync _commit
# define ftruncate(a, b) chsize(a, b)
#endif /* defined(PHP_WIN32) */

#ifndef EWOULDBLOCK
# define EWOULDBLOCK EAGAIN
#endif

#ifdef PHP_WIN32
#define php_socket_errno() WSAGetLastError()
#else
#define php_socket_errno() errno
#endif

/* like strerror, but caller must efree the returned string,
 * unless buf is not NULL.
 * Also works sensibly for win32 */
BEGIN_EXTERN_C()
PHPAPI char *php_socket_strerror(long err, char *buf, size_t bufsize);
PHPAPI zend_string *php_socket_error_str(long err);
END_EXTERN_C()

#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_GETHOSTBYNAME_R
#include <netdb.h>
#endif

/* These are here, rather than with the win32 counterparts above,
 * since <sys/socket.h> defines them. */
#ifndef SHUT_RD
# define SHUT_RD 0
# define SHUT_WR 1
# define SHUT_RDWR 2
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#ifdef PHP_WIN32
typedef SOCKET php_socket_t;
#else
typedef int php_socket_t;
#endif

#ifdef PHP_WIN32
# define SOCK_ERR INVALID_SOCKET
# define SOCK_CONN_ERR SOCKET_ERROR
# define SOCK_RECV_ERR SOCKET_ERROR
#else
# define SOCK_ERR -1
# define SOCK_CONN_ERR -1
# define SOCK_RECV_ERR -1
#endif

#define STREAM_SOCKOP_NONE                (1 << 0)
#define STREAM_SOCKOP_SO_REUSEPORT        (1 << 1)
#define STREAM_SOCKOP_SO_BROADCAST        (1 << 2)
#define STREAM_SOCKOP_IPV6_V6ONLY         (1 << 3)
#define STREAM_SOCKOP_IPV6_V6ONLY_ENABLED (1 << 4)
#define STREAM_SOCKOP_TCP_NODELAY         (1 << 5)


/* uncomment this to debug poll(2) emulation on systems that have poll(2) */
/* #define PHP_USE_POLL_2_EMULATION 1 */

#if defined(HAVE_POLL)
# if defined(HAVE_POLL_H)
#  include <poll.h>
# elif defined(HAVE_SYS_POLL_H)
#  include <sys/poll.h>
# endif
typedef struct pollfd php_pollfd;
#else
typedef struct _php_pollfd {
	php_socket_t fd;
	short events;
	short revents;
} php_pollfd;

PHPAPI int php_poll2(php_pollfd *ufds, unsigned int nfds, int timeout);

#ifndef POLLIN
# define POLLIN      0x0001    /* There is data to read */
# define POLLPRI     0x0002    /* There is urgent data to read */
# define POLLOUT     0x0004    /* Writing now will not block */
# define POLLERR     0x0008    /* Error condition */
# define POLLHUP     0x0010    /* Hung up */
# define POLLNVAL    0x0020    /* Invalid request: fd not open */
#endif

# ifndef PHP_USE_POLL_2_EMULATION
#  define PHP_USE_POLL_2_EMULATION 1
# endif
#endif

#define PHP_POLLREADABLE	(POLLIN|POLLERR|POLLHUP)

#ifndef PHP_USE_POLL_2_EMULATION
# define php_poll2(ufds, nfds, timeout)		poll(ufds, nfds, timeout)
#endif

/* timeval-to-timeout (for poll(2)) */
static inline int php_tvtoto(struct timeval *timeouttv)
{
	if (timeouttv) {
		return (timeouttv->tv_sec * 1000) + (timeouttv->tv_usec / 1000);
	}
	return -1;
}

/* hybrid select(2)/poll(2) for a single descriptor.
 * timeouttv follows same rules as select(2), but is reduced to millisecond accuracy.
 * Returns 0 on timeout, -1 on error, or the event mask (ala poll(2)).
 */
static inline int php_pollfd_for(php_socket_t fd, int events, struct timeval *timeouttv)
{
	php_pollfd p;
	int n;

	p.fd = fd;
	p.events = events;
	p.revents = 0;

	n = php_poll2(&p, 1, php_tvtoto(timeouttv));

	if (n > 0) {
		return p.revents;
	}

	return n;
}

static inline int php_pollfd_for_ms(php_socket_t fd, int events, int timeout)
{
	php_pollfd p;
	int n;

	p.fd = fd;
	p.events = events;
	p.revents = 0;

	n = php_poll2(&p, 1, timeout);

	if (n > 0) {
		return p.revents;
	}

	return n;
}

/* emit warning and suggestion for unsafe select(2) usage */
PHPAPI void _php_emit_fd_setsize_warning(int max_fd);

#ifdef PHP_WIN32
/* it is safe to FD_SET too many fd's under win32; the macro will simply ignore
 * descriptors that go beyond the default FD_SETSIZE */
# define PHP_SAFE_FD_SET(fd, set)	FD_SET(fd, set)
# define PHP_SAFE_FD_CLR(fd, set)	FD_CLR(fd, set)
# define PHP_SAFE_FD_ISSET(fd, set)	FD_ISSET(fd, set)
# define PHP_SAFE_MAX_FD(m, n)		do { if (n + 1 >= FD_SETSIZE) { _php_emit_fd_setsize_warning(n); }} while(0)
#else
# define PHP_SAFE_FD_SET(fd, set)	do { if (fd < FD_SETSIZE) FD_SET(fd, set); } while(0)
# define PHP_SAFE_FD_CLR(fd, set)	do { if (fd < FD_SETSIZE) FD_CLR(fd, set); } while(0)
# define PHP_SAFE_FD_ISSET(fd, set)	((fd < FD_SETSIZE) && FD_ISSET(fd, set))
# define PHP_SAFE_MAX_FD(m, n)		do { if (m >= FD_SETSIZE) { _php_emit_fd_setsize_warning(m); m = FD_SETSIZE - 1; }} while(0)
#endif


#define PHP_SOCK_CHUNK_SIZE	8192

#ifdef HAVE_SOCKADDR_STORAGE
typedef struct sockaddr_storage php_sockaddr_storage;
#else
typedef struct {
#ifdef HAVE_SOCKADDR_SA_LEN
		unsigned char ss_len;
		unsigned char ss_family;
#else
        unsigned short ss_family;
#endif
        char info[126];
} php_sockaddr_storage;
#endif

BEGIN_EXTERN_C()
PHPAPI int php_network_getaddresses(const char *host, int socktype, struct sockaddr ***sal, zend_string **error_string);
PHPAPI void php_network_freeaddresses(struct sockaddr **sal);

PHPAPI php_socket_t php_network_connect_socket_to_host(const char *host, unsigned short port,
		int socktype, int asynchronous, struct timeval *timeout, zend_string **error_string,
		int *error_code, char *bindto, unsigned short bindport, long sockopts
		);

PHPAPI int php_network_connect_socket(php_socket_t sockfd,
		const struct sockaddr *addr,
		socklen_t addrlen,
		int asynchronous,
		struct timeval *timeout,
		zend_string **error_string,
		int *error_code);

#define php_connect_nonb(sock, addr, addrlen, timeout) \
	php_network_connect_socket((sock), (addr), (addrlen), 0, (timeout), NULL, NULL)

PHPAPI php_socket_t php_network_bind_socket_to_local_addr(const char *host, unsigned port,
		int socktype, long sockopts, zend_string **error_string, int *error_code
		);

PHPAPI php_socket_t php_network_accept_incoming(php_socket_t srvsock,
		zend_string **textaddr,
		struct sockaddr **addr,
		socklen_t *addrlen,
		struct timeval *timeout,
		zend_string **error_string,
		int *error_code,
		int tcp_nodelay
		);

PHPAPI int php_network_get_sock_name(php_socket_t sock,
		zend_string **textaddr,
		struct sockaddr **addr,
		socklen_t *addrlen
		);

PHPAPI int php_network_get_peer_name(php_socket_t sock,
		zend_string **textaddr,
		struct sockaddr **addr,
		socklen_t *addrlen
		);

PHPAPI void php_any_addr(int family, php_sockaddr_storage *addr, unsigned short port);
PHPAPI int php_sockaddr_size(php_sockaddr_storage *addr);
END_EXTERN_C()

struct _php_netstream_data_t	{
	php_socket_t socket;
	char is_blocked;
	struct timeval timeout;
	char timeout_event;
	size_t ownsize;
};
typedef struct _php_netstream_data_t php_netstream_data_t;
PHPAPI extern php_stream_ops php_stream_socket_ops;
extern php_stream_ops php_stream_generic_socket_ops;
#define PHP_STREAM_IS_SOCKET	(&php_stream_socket_ops)

BEGIN_EXTERN_C()
PHPAPI php_stream *_php_stream_sock_open_from_socket(php_socket_t socket, const char *persistent_id STREAMS_DC );
/* open a connection to a host using php_hostconnect and return a stream */
PHPAPI php_stream *_php_stream_sock_open_host(const char *host, unsigned short port,
		int socktype, struct timeval *timeout, const char *persistent_id STREAMS_DC);
PHPAPI void php_network_populate_name_from_sockaddr(
		/* input address */
		struct sockaddr *sa, socklen_t sl,
		/* output readable address */
		zend_string **textaddr,
		/* output address */
		struct sockaddr **addr,
		socklen_t *addrlen
		);

PHPAPI int php_network_parse_network_address_with_port(const char *addr,
		zend_long addrlen, struct sockaddr *sa, socklen_t *sl);

PHPAPI struct hostent*	php_network_gethostbyname(char *name);

PHPAPI int php_set_sock_blocking(php_socket_t socketd, int block);
END_EXTERN_C()

#define php_stream_sock_open_from_socket(socket, persistent)	_php_stream_sock_open_from_socket((socket), (persistent) STREAMS_CC)
#define php_stream_sock_open_host(host, port, socktype, timeout, persistent)	_php_stream_sock_open_host((host), (port), (socktype), (timeout), (persistent) STREAMS_CC)

/* {{{ memory debug */
#define php_stream_sock_open_from_socket_rel(socket, persistent)	_php_stream_sock_open_from_socket((socket), (persistent) STREAMS_REL_CC)
#define php_stream_sock_open_host_rel(host, port, socktype, timeout, persistent)	_php_stream_sock_open_host((host), (port), (socktype), (timeout), (persistent) STREAMS_REL_CC)
#define php_stream_sock_open_unix_rel(path, pathlen, persistent, timeval)	_php_stream_sock_open_unix((path), (pathlen), (persistent), (timeval) STREAMS_REL_CC)

/* }}} */

#ifndef MAXFQDNLEN
#define MAXFQDNLEN 255
#endif

#endif /* _PHP_NETWORK_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
