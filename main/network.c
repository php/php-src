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
   | Author: Stig Venaas <venaas@uninett.no>                              |
   | Streams work by Wez Furlong <wez@thebrainroom.com>                   |
   +----------------------------------------------------------------------+
 */

/*#define DEBUG_MAIN_NETWORK 1*/

#include "php.h"

#include <stddef.h>
#include <errno.h>


#ifdef PHP_WIN32
# include <Ws2tcpip.h>
# include "win32/inet.h"
# include "win32/winutil.h"
# define O_RDONLY _O_RDONLY
# include "win32/param.h"
#else
#include <sys/param.h>
#endif

#include <sys/types.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifndef _FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#if HAVE_POLL_H
#include <poll.h>
#elif HAVE_SYS_POLL_H
#include <sys/poll.h>
#endif


#ifndef PHP_WIN32
#include <netinet/in.h>
#include <netdb.h>
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#endif

#ifndef HAVE_INET_ATON
int inet_aton(const char *, struct in_addr *);
#endif

#include "php_network.h"

#if defined(PHP_WIN32) || defined(__riscos__)
#undef AF_UNIX
#endif

#if defined(AF_UNIX)
#include <sys/un.h>
#endif

#include "ext/standard/file.h"

#ifdef PHP_WIN32
# include "win32/time.h"
# define SOCK_ERR INVALID_SOCKET
# define SOCK_CONN_ERR SOCKET_ERROR
# define PHP_TIMEOUT_ERROR_VALUE		WSAETIMEDOUT

#ifdef HAVE_IPV6
const struct in6_addr in6addr_any = {0}; /* IN6ADDR_ANY_INIT; */
#endif

#else
# define SOCK_ERR -1
# define SOCK_CONN_ERR -1
# define PHP_TIMEOUT_ERROR_VALUE		ETIMEDOUT
#endif

#if HAVE_GETADDRINFO
#ifdef HAVE_GAI_STRERROR
#  define PHP_GAI_STRERROR(x) (gai_strerror(x))
#else
#  define PHP_GAI_STRERROR(x) (php_gai_strerror(x))
/* {{{ php_gai_strerror */
static const char *php_gai_strerror(int code)
{
	static struct {
		int code;
		const char *msg;
	} values[] = {
#  ifdef EAI_ADDRFAMILY
		{EAI_ADDRFAMILY, "Address family for hostname not supported"},
#  endif
		{EAI_AGAIN, "Temporary failure in name resolution"},
		{EAI_BADFLAGS, "Bad value for ai_flags"},
		{EAI_FAIL, "Non-recoverable failure in name resolution"},
		{EAI_FAMILY, "ai_family not supported"},
		{EAI_MEMORY, "Memory allocation failure"},
#  ifdef EAI_NODATA
		{EAI_NODATA, "No address associated with hostname"},
#  endif
		{EAI_NONAME, "Name or service not known"},
		{EAI_SERVICE, "Servname not supported for ai_socktype"},
		{EAI_SOCKTYPE, "ai_socktype not supported"},
		{EAI_SYSTEM, "System error"},
		{0, NULL}
	};
	int i;

	for (i = 0; values[i].msg != NULL; i++) {
		if (values[i].code == code) {
			return (char *)values[i].msg;
		}
	}

	return "Unknown error";
}
/* }}} */
#endif
#endif

/* {{{ php_network_freeaddresses */
PHPAPI void php_network_freeaddresses(struct sockaddr **sal)
{
	struct sockaddr **sap;

	if (sal == NULL)
		return;
	for (sap = sal; *sap != NULL; sap++)
		efree(*sap);
	efree(sal);
}
/* }}} */

/* {{{ php_network_getaddresses
 * Returns number of addresses, 0 for none/error
 */
PHPAPI int php_network_getaddresses(const char *host, int socktype, struct sockaddr ***sal, zend_string **error_string)
{
	struct sockaddr **sap;
	int n;
#if HAVE_GETADDRINFO
#ifdef HAVE_IPV6
	static int ipv6_borked = -1; /* the way this is used *is* thread safe */
#endif
	struct addrinfo hints, *res, *sai;
#else
	struct hostent *host_info;
	struct in_addr in;
#endif

	if (host == NULL) {
		return 0;
	}
#if HAVE_GETADDRINFO
	memset(&hints, '\0', sizeof(hints));

	hints.ai_family = AF_INET; /* default to regular inet (see below) */
	hints.ai_socktype = socktype;

#ifdef HAVE_IPV6
	/* probe for a working IPv6 stack; even if detected as having v6 at compile
	 * time, at runtime some stacks are slow to resolve or have other issues
	 * if they are not correctly configured.
	 * static variable use is safe here since simple store or fetch operations
	 * are atomic and because the actual probe process is not in danger of
	 * collisions or race conditions. */
	if (ipv6_borked == -1) {
		int s;

		s = socket(PF_INET6, SOCK_DGRAM, 0);
		if (s == SOCK_ERR) {
			ipv6_borked = 1;
		} else {
			ipv6_borked = 0;
			closesocket(s);
		}
	}
	hints.ai_family = ipv6_borked ? AF_INET : AF_UNSPEC;
#endif

	if ((n = getaddrinfo(host, NULL, &hints, &res))) {
		if (error_string) {
			/* free error string received during previous iteration (if any) */
			if (*error_string) {
				zend_string_release_ex(*error_string, 0);
			}
			*error_string = strpprintf(0, "php_network_getaddresses: getaddrinfo for %s failed: %s", host, PHP_GAI_STRERROR(n));
			php_error_docref(NULL, E_WARNING, "%s", ZSTR_VAL(*error_string));
		} else {
			php_error_docref(NULL, E_WARNING, "php_network_getaddresses: getaddrinfo for %s failed: %s", host, PHP_GAI_STRERROR(n));
		}
		return 0;
	} else if (res == NULL) {
		if (error_string) {
			/* free error string received during previous iteration (if any) */
			if (*error_string) {
				zend_string_release_ex(*error_string, 0);
			}
			*error_string = strpprintf(0, "php_network_getaddresses: getaddrinfo for %s failed (null result pointer) errno=%d", host, errno);
			php_error_docref(NULL, E_WARNING, "%s", ZSTR_VAL(*error_string));
		} else {
			php_error_docref(NULL, E_WARNING, "php_network_getaddresses: getaddrinfo for %s failed (null result pointer)", host);
		}
		return 0;
	}

	sai = res;
	for (n = 1; (sai = sai->ai_next) != NULL; n++)
		;

	*sal = safe_emalloc((n + 1), sizeof(*sal), 0);
	sai = res;
	sap = *sal;

	do {
		*sap = emalloc(sai->ai_addrlen);
		memcpy(*sap, sai->ai_addr, sai->ai_addrlen);
		sap++;
	} while ((sai = sai->ai_next) != NULL);

	freeaddrinfo(res);
#else
	if (!inet_pton(AF_INET, host, &in)) {
		if(strlen(host) > MAXFQDNLEN) {
			host_info = NULL;
			errno = E2BIG;
		} else {
			host_info = php_network_gethostbyname(host);
		}
		if (host_info == NULL) {
			if (error_string) {
				/* free error string received during previous iteration (if any) */
				if (*error_string) {
					zend_string_release_ex(*error_string, 0);
				}
				*error_string = strpprintf(0, "php_network_getaddresses: gethostbyname failed. errno=%d", errno);
				php_error_docref(NULL, E_WARNING, "%s", ZSTR_VAL(*error_string));
			} else {
				php_error_docref(NULL, E_WARNING, "php_network_getaddresses: gethostbyname failed");
			}
			return 0;
		}
		in = *((struct in_addr *) host_info->h_addr);
	}

	*sal = safe_emalloc(2, sizeof(*sal), 0);
	sap = *sal;
	*sap = emalloc(sizeof(struct sockaddr_in));
	(*sap)->sa_family = AF_INET;
	((struct sockaddr_in *)*sap)->sin_addr = in;
	sap++;
	n = 1;
#endif

	*sap = NULL;
	return n;
}
/* }}} */

#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif

#ifdef PHP_WIN32
typedef u_long php_non_blocking_flags_t;
#  define SET_SOCKET_BLOCKING_MODE(sock, save) \
	save = TRUE; ioctlsocket(sock, FIONBIO, &save)
#  define RESTORE_SOCKET_BLOCKING_MODE(sock, save) \
	ioctlsocket(sock, FIONBIO, &save)
#else
typedef int php_non_blocking_flags_t;
#  define SET_SOCKET_BLOCKING_MODE(sock, save) \
	 save = fcntl(sock, F_GETFL, 0); \
	 fcntl(sock, F_SETFL, save | O_NONBLOCK)
#  define RESTORE_SOCKET_BLOCKING_MODE(sock, save) \
	 fcntl(sock, F_SETFL, save)
#endif

/* Connect to a socket using an interruptible connect with optional timeout.
 * Optionally, the connect can be made asynchronously, which will implicitly
 * enable non-blocking mode on the socket.
 * */
/* {{{ php_network_connect_socket */
PHPAPI int php_network_connect_socket(php_socket_t sockfd,
		const struct sockaddr *addr,
		socklen_t addrlen,
		int asynchronous,
		struct timeval *timeout,
		zend_string **error_string,
		int *error_code)
{
	php_non_blocking_flags_t orig_flags;
	int n;
	int error = 0;
	socklen_t len;
	int ret = 0;

	SET_SOCKET_BLOCKING_MODE(sockfd, orig_flags);

	if ((n = connect(sockfd, addr, addrlen)) != 0) {
		error = php_socket_errno();

		if (error_code) {
			*error_code = error;
		}

		if (error != EINPROGRESS) {
			if (error_string) {
				*error_string = php_socket_error_str(error);
			}

			return -1;
		}
		if (asynchronous && error == EINPROGRESS) {
			/* this is fine by us */
			return 0;
		}
	}

	if (n == 0) {
		goto ok;
	}
# ifdef PHP_WIN32
	/* The documentation for connect() says in case of non-blocking connections
	 * the select function reports success in the writefds set and failure in
	 * the exceptfds set. Indeed, using PHP_POLLREADABLE results in select
	 * failing only due to the timeout and not immediately as would be
	 * expected when a connection is actively refused. This way,
	 * php_pollfd_for will return a mask with POLLOUT if the connection
	 * is successful and with POLLPRI otherwise. */
	if ((n = php_pollfd_for(sockfd, POLLOUT|POLLPRI, timeout)) == 0) {
#else
	if ((n = php_pollfd_for(sockfd, PHP_POLLREADABLE|POLLOUT, timeout)) == 0) {
#endif
		error = PHP_TIMEOUT_ERROR_VALUE;
	}

	if (n > 0) {
		len = sizeof(error);
		/*
		   BSD-derived systems set errno correctly
		   Solaris returns -1 from getsockopt in case of error
		   */
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)&error, &len) != 0) {
			ret = -1;
		}
	} else {
		/* whoops: sockfd has disappeared */
		ret = -1;
	}

ok:
	if (!asynchronous) {
		/* back to blocking mode */
		RESTORE_SOCKET_BLOCKING_MODE(sockfd, orig_flags);
	}

	if (error_code) {
		*error_code = error;
	}

	if (error) {
		ret = -1;
		if (error_string) {
			*error_string = php_socket_error_str(error);
		}
	}
	return ret;
}
/* }}} */

/* {{{ sub_times */
static inline void sub_times(struct timeval a, struct timeval b, struct timeval *result)
{
	result->tv_usec = a.tv_usec - b.tv_usec;
	if (result->tv_usec < 0L) {
		a.tv_sec--;
		result->tv_usec += 1000000L;
	}
	result->tv_sec = a.tv_sec - b.tv_sec;
	if (result->tv_sec < 0L) {
		result->tv_sec++;
		result->tv_usec -= 1000000L;
	}
}
/* }}} */

/* Bind to a local IP address.
 * Returns the bound socket, or -1 on failure.
 * */
/* {{{ php_network_bind_socket_to_local_addr */
php_socket_t php_network_bind_socket_to_local_addr(const char *host, unsigned port,
		int socktype, long sockopts, zend_string **error_string, int *error_code
		)
{
	int num_addrs, n, err = 0;
	php_socket_t sock;
	struct sockaddr **sal, **psal, *sa;
	socklen_t socklen;
	int sockoptval = 1;

	num_addrs = php_network_getaddresses(host, socktype, &psal, error_string);

	if (num_addrs == 0) {
		/* could not resolve address(es) */
		return -1;
	}

	for (sal = psal; *sal != NULL; sal++) {
		sa = *sal;

		switch (sa->sa_family) {
#if HAVE_GETADDRINFO && HAVE_IPV6
			case AF_INET6:
				((struct sockaddr_in6 *)sa)->sin6_port = htons(port);
				socklen = sizeof(struct sockaddr_in6);
				break;
#endif
			case AF_INET:
				((struct sockaddr_in *)sa)->sin_port = htons(port);
				socklen = sizeof(struct sockaddr_in);
				break;
			default:
				/* Unsupported family, skip to the next */
				continue;
		}

		/* create a socket for this address */
		sock = socket(sa->sa_family, socktype, 0);

		if (sock == SOCK_ERR) {
			continue;
		}

		/* attempt to bind */

#ifdef SO_REUSEADDR
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&sockoptval, sizeof(sockoptval));
#endif
#ifdef IPV6_V6ONLY
		if (sockopts & STREAM_SOCKOP_IPV6_V6ONLY) {
			int ipv6_val = !!(sockopts & STREAM_SOCKOP_IPV6_V6ONLY_ENABLED);
			setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&ipv6_val, sizeof(sockoptval));
		}
#endif
#ifdef SO_REUSEPORT
		if (sockopts & STREAM_SOCKOP_SO_REUSEPORT) {
			setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (char*)&sockoptval, sizeof(sockoptval));
		}
#endif
#ifdef SO_BROADCAST
		if (sockopts & STREAM_SOCKOP_SO_BROADCAST) {
			setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&sockoptval, sizeof(sockoptval));
		}
#endif
#ifdef TCP_NODELAY
		if (sockopts & STREAM_SOCKOP_TCP_NODELAY) {
			setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&sockoptval, sizeof(sockoptval));
		}
#endif

		n = bind(sock, sa, socklen);

		if (n != SOCK_CONN_ERR) {
			goto bound;
		}

		err = php_socket_errno();

		closesocket(sock);
	}
	sock = -1;

	if (error_code) {
		*error_code = err;
	}
	if (error_string) {
		*error_string = php_socket_error_str(err);
	}

bound:

	php_network_freeaddresses(psal);

	return sock;

}
/* }}} */

PHPAPI int php_network_parse_network_address_with_port(const char *addr, zend_long addrlen, struct sockaddr *sa, socklen_t *sl)
{
	char *colon;
	char *tmp;
	int ret = FAILURE;
	short port;
	struct sockaddr_in *in4 = (struct sockaddr_in*)sa;
	struct sockaddr **psal;
	int n;
	zend_string *errstr = NULL;
#ifdef HAVE_IPV6
	struct sockaddr_in6 *in6 = (struct sockaddr_in6*)sa;

	memset(in6, 0, sizeof(struct sockaddr_in6));
#else
	memset(in4, 0, sizeof(struct sockaddr_in));
#endif

	if (*addr == '[') {
		colon = memchr(addr + 1, ']', addrlen-1);
		if (!colon || colon[1] != ':') {
			return FAILURE;
		}
		port = atoi(colon + 2);
		addr++;
	} else {
		colon = memchr(addr, ':', addrlen);
		if (!colon) {
			return FAILURE;
		}
		port = atoi(colon + 1);
	}

	tmp = estrndup(addr, colon - addr);

	/* first, try interpreting the address as a numeric address */

#ifdef HAVE_IPV6
	if (inet_pton(AF_INET6, tmp, &in6->sin6_addr) > 0) {
		in6->sin6_port = htons(port);
		in6->sin6_family = AF_INET6;
		*sl = sizeof(struct sockaddr_in6);
		ret = SUCCESS;
		goto out;
	}
#endif
	if (inet_pton(AF_INET, tmp, &in4->sin_addr) > 0) {
		in4->sin_port = htons(port);
		in4->sin_family = AF_INET;
		*sl = sizeof(struct sockaddr_in);
		ret = SUCCESS;
		goto out;
	}

	/* looks like we'll need to resolve it */
	n = php_network_getaddresses(tmp, SOCK_DGRAM, &psal, &errstr);

	if (n == 0) {
		if (errstr) {
			php_error_docref(NULL, E_WARNING, "Failed to resolve `%s': %s", tmp, ZSTR_VAL(errstr));
			zend_string_release_ex(errstr, 0);
		}
		goto out;
	}

	/* copy the details from the first item */
	switch ((*psal)->sa_family) {
#if HAVE_GETADDRINFO && HAVE_IPV6
		case AF_INET6:
			*in6 = **(struct sockaddr_in6**)psal;
			in6->sin6_port = htons(port);
			*sl = sizeof(struct sockaddr_in6);
			ret = SUCCESS;
			break;
#endif
		case AF_INET:
			*in4 = **(struct sockaddr_in**)psal;
			in4->sin_port = htons(port);
			*sl = sizeof(struct sockaddr_in);
			ret = SUCCESS;
			break;
	}

	php_network_freeaddresses(psal);

out:
	efree(tmp);
	return ret;
}


PHPAPI void php_network_populate_name_from_sockaddr(
		/* input address */
		struct sockaddr *sa, socklen_t sl,
		/* output readable address */
		zend_string **textaddr,
		/* output address */
		struct sockaddr **addr,
		socklen_t *addrlen
		)
{
	if (addr) {
		*addr = emalloc(sl);
		memcpy(*addr, sa, sl);
		*addrlen = sl;
	}

	if (textaddr) {
		char abuf[256];
		const char *buf = NULL;

		switch (sa->sa_family) {
			case AF_INET:
				/* generally not thread safe, but it *is* thread safe under win32 */
				buf = inet_ntop(AF_INET, &((struct sockaddr_in*)sa)->sin_addr, (char *)&abuf, sizeof(abuf));
				if (buf) {
					*textaddr = strpprintf(0, "%s:%d",
						buf, ntohs(((struct sockaddr_in*)sa)->sin_port));
				}

				break;

#ifdef HAVE_IPV6
			case AF_INET6:
				buf = (char*)inet_ntop(sa->sa_family, &((struct sockaddr_in6*)sa)->sin6_addr, (char *)&abuf, sizeof(abuf));
				if (buf) {
					*textaddr = strpprintf(0, "[%s]:%d",
						buf, ntohs(((struct sockaddr_in6*)sa)->sin6_port));
				}

				break;
#endif
#ifdef AF_UNIX
			case AF_UNIX:
				{
					struct sockaddr_un *ua = (struct sockaddr_un*)sa;

					if (ua->sun_path[0] == '\0') {
						/* abstract name */
						int len = sl - sizeof(sa_family_t);
						*textaddr = zend_string_init((char*)ua->sun_path, len, 0);
					} else {
						int len = strlen(ua->sun_path);
						*textaddr = zend_string_init((char*)ua->sun_path, len, 0);
					}
				}
				break;
#endif

		}

	}
}

PHPAPI int php_network_get_peer_name(php_socket_t sock,
		zend_string **textaddr,
		struct sockaddr **addr,
		socklen_t *addrlen
		)
{
	php_sockaddr_storage sa;
	socklen_t sl = sizeof(sa);
	memset(&sa, 0, sizeof(sa));

	if (getpeername(sock, (struct sockaddr*)&sa, &sl) == 0) {
		php_network_populate_name_from_sockaddr((struct sockaddr*)&sa, sl,
				textaddr,
				addr, addrlen
				);
		return 0;
	}
	return -1;
}

PHPAPI int php_network_get_sock_name(php_socket_t sock,
		zend_string **textaddr,
		struct sockaddr **addr,
		socklen_t *addrlen
		)
{
	php_sockaddr_storage sa;
	socklen_t sl = sizeof(sa);
	memset(&sa, 0, sizeof(sa));

	if (getsockname(sock, (struct sockaddr*)&sa, &sl) == 0) {
		php_network_populate_name_from_sockaddr((struct sockaddr*)&sa, sl,
				textaddr,
				addr, addrlen
				);
		return 0;
	}
	return -1;

}


/* Accept a client connection from a server socket,
 * using an optional timeout.
 * Returns the peer address in addr/addrlen (it will emalloc
 * these, so be sure to efree the result).
 * If you specify textaddr, a text-printable
 * version of the address will be emalloc'd and returned.
 * */

/* {{{ php_network_accept_incoming */
PHPAPI php_socket_t php_network_accept_incoming(php_socket_t srvsock,
		zend_string **textaddr,
		struct sockaddr **addr,
		socklen_t *addrlen,
		struct timeval *timeout,
		zend_string **error_string,
		int *error_code,
		int tcp_nodelay
		)
{
	php_socket_t clisock = -1;
	int error = 0, n;
	php_sockaddr_storage sa;
	socklen_t sl;

	n = php_pollfd_for(srvsock, PHP_POLLREADABLE, timeout);

	if (n == 0) {
		error = PHP_TIMEOUT_ERROR_VALUE;
	} else if (n == -1) {
		error = php_socket_errno();
	} else {
		sl = sizeof(sa);

		clisock = accept(srvsock, (struct sockaddr*)&sa, &sl);

		if (clisock != SOCK_ERR) {
			php_network_populate_name_from_sockaddr((struct sockaddr*)&sa, sl,
					textaddr,
					addr, addrlen
					);
			if (tcp_nodelay) {
#ifdef TCP_NODELAY
				setsockopt(clisock, IPPROTO_TCP, TCP_NODELAY, (char*)&tcp_nodelay, sizeof(tcp_nodelay));
#endif
			}
		} else {
			error = php_socket_errno();
		}
	}

	if (error_code) {
		*error_code = error;
	}
	if (error_string) {
		*error_string = php_socket_error_str(error);
	}

	return clisock;
}
/* }}} */


/* Connect to a remote host using an interruptible connect with optional timeout.
 * Optionally, the connect can be made asynchronously, which will implicitly
 * enable non-blocking mode on the socket.
 * Returns the connected (or connecting) socket, or -1 on failure.
 * */

/* {{{ php_network_connect_socket_to_host */
php_socket_t php_network_connect_socket_to_host(const char *host, unsigned short port,
		int socktype, int asynchronous, struct timeval *timeout, zend_string **error_string,
		int *error_code, const char *bindto, unsigned short bindport, long sockopts
		)
{
	int num_addrs, n, fatal = 0;
	php_socket_t sock;
	struct sockaddr **sal, **psal, *sa;
	struct timeval working_timeout;
	socklen_t socklen;
#if HAVE_GETTIMEOFDAY
	struct timeval limit_time, time_now;
#endif

	num_addrs = php_network_getaddresses(host, socktype, &psal, error_string);

	if (num_addrs == 0) {
		/* could not resolve address(es) */
		return -1;
	}

	if (timeout) {
		memcpy(&working_timeout, timeout, sizeof(working_timeout));
#if HAVE_GETTIMEOFDAY
		gettimeofday(&limit_time, NULL);
		limit_time.tv_sec += working_timeout.tv_sec;
		limit_time.tv_usec += working_timeout.tv_usec;
		if (limit_time.tv_usec >= 1000000) {
			limit_time.tv_usec -= 1000000;
			limit_time.tv_sec++;
		}
#endif
	}

	for (sal = psal; !fatal && *sal != NULL; sal++) {
		sa = *sal;

		switch (sa->sa_family) {
#if HAVE_GETADDRINFO && HAVE_IPV6
			case AF_INET6:
				if (!bindto || strchr(bindto, ':')) {
					((struct sockaddr_in6 *)sa)->sin6_port = htons(port);
					socklen = sizeof(struct sockaddr_in6);
				} else {
					/* Expect IPV4 address, skip to the next */
					continue;
				}
				break;
#endif
			case AF_INET:
				((struct sockaddr_in *)sa)->sin_port = htons(port);
				socklen = sizeof(struct sockaddr_in);
				if (bindto && (strchr(bindto, ':') || !strcmp(bindto, "0"))) {
					/* IPV4 sock can not bind to IPV6 address */
					bindto = NULL;
				}
				break;
			default:
				/* Unsupported family, skip to the next */
				continue;
		}

		/* create a socket for this address */
		sock = socket(sa->sa_family, socktype, 0);

		if (sock == SOCK_ERR) {
			continue;
		}

			/* make a connection attempt */

		if (bindto) {
			union {
				struct sockaddr common;
				struct sockaddr_in in4;
#ifdef HAVE_IPV6
				struct sockaddr_in6 in6;
#endif
			} local_address;
			int local_address_len = 0;

			if (sa->sa_family == AF_INET) {
				if (inet_pton(AF_INET, bindto, &local_address.in4.sin_addr) == 1) {
					local_address_len = sizeof(struct sockaddr_in);
					local_address.in4.sin_family = sa->sa_family;
					local_address.in4.sin_port = htons(bindport);
					memset(&(local_address.in4.sin_zero), 0, sizeof(local_address.in4.sin_zero));
				}
			}
#ifdef HAVE_IPV6
			else { /* IPV6 */
				if (inet_pton(AF_INET6, bindto, &local_address.in6.sin6_addr) == 1) {
					local_address_len = sizeof(struct sockaddr_in6);
					local_address.in6.sin6_family = sa->sa_family;
					local_address.in6.sin6_port = htons(bindport);
				}
			}
#endif
#ifdef IP_BIND_ADDRESS_NO_PORT
			{
				int val = 1;
				(void) setsockopt(sock, SOL_IP, IP_BIND_ADDRESS_NO_PORT, &val, sizeof(val));
			}
#endif
			if (local_address_len == 0) {
				php_error_docref(NULL, E_WARNING, "Invalid IP Address: %s", bindto);
			} else if (bind(sock, &local_address.common, local_address_len)) {
				php_error_docref(NULL, E_WARNING, "Failed to bind to '%s:%d', system said: %s", bindto, bindport, strerror(errno));
			}
		}
		/* free error string received during previous iteration (if any) */
		if (error_string && *error_string) {
			zend_string_release_ex(*error_string, 0);
			*error_string = NULL;
		}

#ifdef SO_BROADCAST
		{
			int val = 1;
			if (sockopts & STREAM_SOCKOP_SO_BROADCAST) {
				setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&val, sizeof(val));
			}
		}
#endif

#ifdef TCP_NODELAY
		{
			int val = 1;
			if (sockopts & STREAM_SOCKOP_TCP_NODELAY) {
				setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&val, sizeof(val));
			}
		}
#endif
		n = php_network_connect_socket(sock, sa, socklen, asynchronous,
				timeout ? &working_timeout : NULL,
				error_string, error_code);

		if (n != -1) {
			goto connected;
		}

		/* adjust timeout for next attempt */
#if HAVE_GETTIMEOFDAY
		if (timeout) {
			gettimeofday(&time_now, NULL);

			if (!timercmp(&time_now, &limit_time, <)) {
				/* time limit expired; don't attempt any further connections */
				fatal = 1;
			} else {
				/* work out remaining time */
				sub_times(limit_time, time_now, &working_timeout);
			}
		}
#else
		if (error_code && *error_code == PHP_TIMEOUT_ERROR_VALUE) {
			/* Don't even bother trying to connect to the next alternative;
				* we have no way to determine how long we have already taken
				* and it is quite likely that the next attempt will fail too. */
			fatal = 1;
		} else {
			/* re-use the same initial timeout.
				* Not the best thing, but in practice it should be good-enough */
			if (timeout) {
				memcpy(&working_timeout, timeout, sizeof(working_timeout));
			}
		}
#endif

		closesocket(sock);
	}
	sock = -1;

connected:

	php_network_freeaddresses(psal);

	return sock;
}
/* }}} */

/* {{{ php_any_addr
 * Fills any (wildcard) address into php_sockaddr_storage
 */
PHPAPI void php_any_addr(int family, php_sockaddr_storage *addr, unsigned short port)
{
	memset(addr, 0, sizeof(php_sockaddr_storage));
	switch (family) {
#ifdef HAVE_IPV6
	case AF_INET6: {
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) addr;
		sin6->sin6_family = AF_INET6;
		sin6->sin6_port = htons(port);
		sin6->sin6_addr = in6addr_any;
		break;
	}
#endif
	case AF_INET: {
		struct sockaddr_in *sin = (struct sockaddr_in *) addr;
		sin->sin_family = AF_INET;
		sin->sin_port = htons(port);
		sin->sin_addr.s_addr = htonl(INADDR_ANY);
		break;
	}
	}
}
/* }}} */

/* {{{ php_sockaddr_size
 * Returns the size of struct sockaddr_xx for the family
 */
PHPAPI int php_sockaddr_size(php_sockaddr_storage *addr)
{
	switch (((struct sockaddr *)addr)->sa_family) {
	case AF_INET:
		return sizeof(struct sockaddr_in);
#ifdef HAVE_IPV6
	case AF_INET6:
		return sizeof(struct sockaddr_in6);
#endif
#ifdef AF_UNIX
	case AF_UNIX:
		return sizeof(struct sockaddr_un);
#endif
	default:
		return 0;
	}
}
/* }}} */

/* Given a socket error code, if buf == NULL:
 *   emallocs storage for the error message and returns
 * else
 *   sprintf message into provided buffer and returns buf
 */
/* {{{ php_socket_strerror */
PHPAPI char *php_socket_strerror(long err, char *buf, size_t bufsize)
{
#ifndef PHP_WIN32
	char *errstr;

	errstr = strerror(err);
	if (buf == NULL) {
		buf = estrdup(errstr);
	} else {
		strncpy(buf, errstr, bufsize);
		buf[bufsize?(bufsize-1):0] = 0;
	}
	return buf;
#else
	char *sysbuf = php_win32_error_to_msg(err);
	if (!sysbuf[0]) {
		sysbuf = "Unknown Error";
	}

	if (buf == NULL) {
		buf = estrdup(sysbuf);
	} else {
		strncpy(buf, sysbuf, bufsize);
		buf[bufsize?(bufsize-1):0] = 0;
	}

	php_win32_error_msg_free(sysbuf);

	return buf;
#endif
}
/* }}} */

/* {{{ php_socket_error_str */
PHPAPI zend_string *php_socket_error_str(long err)
{
#ifndef PHP_WIN32
	char *errstr;

	errstr = strerror(err);
	return zend_string_init(errstr, strlen(errstr), 0);
#else
	zend_string *ret;

	char *sysbuf = php_win32_error_to_msg(err);
	if (!sysbuf[0]) {
		sysbuf = "Unknown Error";
	}

	ret = zend_string_init(sysbuf, strlen(sysbuf), 0);

	php_win32_error_msg_free(sysbuf);

	return ret;
#endif
}
/* }}} */

/* deprecated */
PHPAPI php_stream *_php_stream_sock_open_from_socket(php_socket_t socket, const char *persistent_id STREAMS_DC)
{
	php_stream *stream;
	php_netstream_data_t *sock;

	sock = pemalloc(sizeof(php_netstream_data_t), persistent_id ? 1 : 0);
	memset(sock, 0, sizeof(php_netstream_data_t));

	sock->is_blocked = 1;
	sock->timeout.tv_sec = FG(default_socket_timeout);
	sock->timeout.tv_usec = 0;
	sock->socket = socket;

	stream = php_stream_alloc_rel(&php_stream_generic_socket_ops, sock, persistent_id, "r+");

	if (stream == NULL) {
		pefree(sock, persistent_id ? 1 : 0);
	} else {
		stream->flags |= PHP_STREAM_FLAG_AVOID_BLOCKING;
	}

	return stream;
}

PHPAPI php_stream *_php_stream_sock_open_host(const char *host, unsigned short port,
		int socktype, struct timeval *timeout, const char *persistent_id STREAMS_DC)
{
	char *res;
	zend_long reslen;
	php_stream *stream;

	reslen = spprintf(&res, 0, "tcp://%s:%d", host, port);

	stream = php_stream_xport_create(res, reslen, REPORT_ERRORS,
			STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT, persistent_id, timeout, NULL, NULL, NULL);

	efree(res);

	return stream;
}

PHPAPI int php_set_sock_blocking(php_socket_t socketd, int block)
{
	int ret = SUCCESS;

#ifdef PHP_WIN32
	u_long flags;

	/* with ioctlsocket, a non-zero sets nonblocking, a zero sets blocking */
	flags = !block;
	if (ioctlsocket(socketd, FIONBIO, &flags) == SOCKET_ERROR) {
		ret = FAILURE;
	}
#else
	int myflag = 0;
	int flags = fcntl(socketd, F_GETFL);

#ifdef O_NONBLOCK
	myflag = O_NONBLOCK; /* POSIX version */
#elif defined(O_NDELAY)
	myflag = O_NDELAY;   /* old non-POSIX version */
#endif
	if (!block) {
		flags |= myflag;
	} else {
		flags &= ~myflag;
	}
	if (fcntl(socketd, F_SETFL, flags) == -1) {
		ret = FAILURE;
	}
#endif
	return ret;
}

PHPAPI void _php_emit_fd_setsize_warning(int max_fd)
{

#ifdef PHP_WIN32
	php_error_docref(NULL, E_WARNING,
		"PHP needs to be recompiled with a larger value of FD_SETSIZE.\n"
		"If this binary is from an official www.php.net package, file a bug report\n"
		"at https://github.com/php/php-src/issues, including the following information:\n"
		"FD_SETSIZE=%d, but you are using %d.\n"
		" --enable-fd-setsize=%d is recommended, but you may want to set it\n"
		"to match to maximum number of sockets each script will work with at\n"
		"one time, in order to avoid seeing this error again at a later date.",
		FD_SETSIZE, max_fd, (max_fd + 128) & ~127);
#else
	php_error_docref(NULL, E_WARNING,
		"You MUST recompile PHP with a larger value of FD_SETSIZE.\n"
		"It is set to %d, but you have descriptors numbered at least as high as %d.\n"
		" --enable-fd-setsize=%d is recommended, but you may want to set it\n"
		"to equal the maximum number of open files supported by your system,\n"
		"in order to avoid seeing this error again at a later date.",
		FD_SETSIZE, max_fd, (max_fd + 1024) & ~1023);
#endif
}

#if defined(PHP_USE_POLL_2_EMULATION)

/* emulate poll(2) using select(2), safely. */

PHPAPI int php_poll2(php_pollfd *ufds, unsigned int nfds, int timeout)
{
	fd_set rset, wset, eset;
	php_socket_t max_fd = SOCK_ERR; /* effectively unused on Windows */
	unsigned int i;
	int n;
	struct timeval tv;

#ifndef PHP_WIN32
	/* check the highest numbered descriptor */
	for (i = 0; i < nfds; i++) {
		if (ufds[i].fd > max_fd)
			max_fd = ufds[i].fd;
	}
#endif

	if (!PHP_SAFE_MAX_FD(max_fd, nfds + 1)) {
#ifdef PHP_WIN32
		WSASetLastError(WSAEINVAL);
#else
		errno = ERANGE;
#endif
		return -1;
	}

	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_ZERO(&eset);

	for (i = 0; i < nfds; i++) {
		if (ufds[i].events & PHP_POLLREADABLE) {
			PHP_SAFE_FD_SET(ufds[i].fd, &rset);
		}
		if (ufds[i].events & POLLOUT) {
			PHP_SAFE_FD_SET(ufds[i].fd, &wset);
		}
		if (ufds[i].events & POLLPRI) {
			PHP_SAFE_FD_SET(ufds[i].fd, &eset);
		}
	}

	if (timeout >= 0) {
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout - (tv.tv_sec * 1000)) * 1000;
	}
/* Resetting/initializing */
#ifdef PHP_WIN32
	WSASetLastError(0);
#else
	errno = 0;
#endif
	n = select(max_fd + 1, &rset, &wset, &eset, timeout >= 0 ? &tv : NULL);

	if (n >= 0) {
		for (i = 0; i < nfds; i++) {
			ufds[i].revents = 0;

			if (PHP_SAFE_FD_ISSET(ufds[i].fd, &rset)) {
				/* could be POLLERR or POLLHUP but can't tell without probing */
				ufds[i].revents |= POLLIN;
			}
			if (PHP_SAFE_FD_ISSET(ufds[i].fd, &wset)) {
				ufds[i].revents |= POLLOUT;
			}
			if (PHP_SAFE_FD_ISSET(ufds[i].fd, &eset)) {
				ufds[i].revents |= POLLPRI;
			}
		}
	}
	return n;
}
#endif

#if defined(HAVE_GETHOSTBYNAME_R)
#ifdef HAVE_FUNC_GETHOSTBYNAME_R_6
struct hostent * gethostname_re (const char *host,struct hostent *hostbuf,char **tmphstbuf,size_t *hstbuflen)
{
	struct hostent *hp;
	int herr,res;

	if (*hstbuflen == 0) {
		*hstbuflen = 1024;
		*tmphstbuf = (char *)malloc (*hstbuflen);
	}

	while (( res =
		gethostbyname_r(host,hostbuf,*tmphstbuf,*hstbuflen,&hp,&herr))
		&& (errno == ERANGE)) {
		/* Enlarge the buffer. */
		*hstbuflen *= 2;
		*tmphstbuf = (char *)realloc (*tmphstbuf,*hstbuflen);
	}

	if (res != SUCCESS) {
		return NULL;
	}

	return hp;
}
#endif
#ifdef HAVE_FUNC_GETHOSTBYNAME_R_5
struct hostent * gethostname_re (const char *host,struct hostent *hostbuf,char **tmphstbuf,size_t *hstbuflen)
{
	struct hostent *hp;
	int herr;

	if (*hstbuflen == 0) {
		*hstbuflen = 1024;
		*tmphstbuf = (char *)malloc (*hstbuflen);
	}

	while ((NULL == ( hp =
		gethostbyname_r(host,hostbuf,*tmphstbuf,*hstbuflen,&herr)))
		&& (errno == ERANGE)) {
		/* Enlarge the buffer. */
		*hstbuflen *= 2;
		*tmphstbuf = (char *)realloc (*tmphstbuf,*hstbuflen);
	}
	return hp;
}
#endif
#ifdef HAVE_FUNC_GETHOSTBYNAME_R_3
struct hostent * gethostname_re (const char *host,struct hostent *hostbuf,char **tmphstbuf,size_t *hstbuflen)
{
	if (*hstbuflen == 0) {
		*hstbuflen = sizeof(struct hostent_data);
		*tmphstbuf = (char *)malloc (*hstbuflen);
	} else {
		if (*hstbuflen < sizeof(struct hostent_data)) {
			*hstbuflen = sizeof(struct hostent_data);
			*tmphstbuf = (char *)realloc(*tmphstbuf, *hstbuflen);
		}
	}
	memset((void *)(*tmphstbuf),0,*hstbuflen);

	if (SUCCESS != gethostbyname_r(host,hostbuf,(struct hostent_data *)*tmphstbuf)) {
		return NULL;
	}

	return hostbuf;
}
#endif
#endif

PHPAPI struct hostent*	php_network_gethostbyname(const char *name) {
#if !defined(HAVE_GETHOSTBYNAME_R)
	return gethostbyname(name);
#else
	if (FG(tmp_host_buf)) {
		free(FG(tmp_host_buf));
	}

	FG(tmp_host_buf) = NULL;
	FG(tmp_host_buf_len) = 0;

	memset(&FG(tmp_host_info), 0, sizeof(struct hostent));

	return gethostname_re(name, &FG(tmp_host_info), &FG(tmp_host_buf), &FG(tmp_host_buf_len));
#endif
}
