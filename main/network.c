/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stig Venaas <venaas@uninett.no>                              |
   | Streams work by Wez Furlong <wez@thebrainroom.com>                   |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/*#define DEBUG_MAIN_NETWORK 1*/

#include "php.h"

#include <stddef.h>

#ifdef PHP_WIN32
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#elif defined(NETWARE)
#ifdef NEW_LIBC
#include <sys/timeval.h>
#include <sys/param.h>
#else
#include "netware/time_nw.h"
#endif
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
#if HAVE_SYS_POLL_H
#include <sys/poll.h>
#endif

#if defined(NETWARE)
#ifdef USE_WINSOCK
/*#include <ws2nlm.h>*/
#include <novsock2.h>
#else
/* New headers for socket stuff */
#ifdef NEW_LIBC
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>
#endif
#include <sys/socket.h>
#endif
#elif !defined(PHP_WIN32)
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

#if defined(PHP_WIN32) || defined(__riscos__) || defined(NETWARE)
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
#else
# define SOCK_ERR -1
# define SOCK_CONN_ERR -1
# define PHP_TIMEOUT_ERROR_VALUE		ETIMEDOUT
#endif

#ifdef HAVE_GETADDRINFO
#ifdef HAVE_GAI_STRERROR
#  define PHP_GAI_STRERROR(x) (gai_strerror(x))
#else
#  define PHP_GAI_STRERROR(x) (php_gai_strerror(x))
/* {{{ php_gai_strerror
 */
static char *php_gai_strerror(int code)
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

/* {{{ php_network_freeaddresses
 */
static void php_network_freeaddresses(struct sockaddr **sal)
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
static int php_network_getaddresses(const char *host, struct sockaddr ***sal, char **error_string TSRMLS_DC)
{
	struct sockaddr **sap;
	int n;
#ifdef HAVE_GETADDRINFO
	struct addrinfo hints, *res, *sai;
#else
	struct hostent *host_info;
	struct in_addr in;
#endif

	if (host == NULL) {
		return 0;
	}

#ifdef HAVE_GETADDRINFO
	memset(&hints, '\0', sizeof(hints));

# ifdef HAVE_IPV6
	hints.ai_family = AF_UNSPEC;
# else
	hints.ai_family = AF_INET;
# endif
	
	if ((n = getaddrinfo(host, NULL, &hints, &res)) || res == NULL) {
		char *str = res == NULL ? "null result pointer" : PHP_GAI_STRERROR(n);

		if (error_string) {
			spprintf(error_string, 0, "getaddrinfo: %s", str);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "php_network_getaddresses: getaddrinfo failed: %s", str);
		}
		return 0;
	}

	sai = res;

	for (n = 1; (sai = sai->ai_next) != NULL; n++) {
		;
	}
		
	*sal = emalloc((n + 1) * sizeof(*sal));
	sai = res;
	sap = *sal;
	do {
		switch (sai->ai_family) {
# if HAVE_IPV6
			case AF_INET6:
				*sap = emalloc(sizeof(struct sockaddr_in6));
				*(struct sockaddr_in6 *)*sap =
					*((struct sockaddr_in6 *)sai->ai_addr);
				sap++;
				break;
# endif
			case AF_INET:
				*sap = emalloc(sizeof(struct sockaddr_in));
				*(struct sockaddr_in *)*sap =
					*((struct sockaddr_in *)sai->ai_addr);
				sap++;
				break;
		}
	} while ((sai = sai->ai_next) != NULL);
	freeaddrinfo(res);
#else

	if (!inet_aton(host, &in)) {
		/* XXX NOT THREAD SAFE
		 * (but it *is* thread safe under win32)
		 */
		host_info = gethostbyname(host);
		if (host_info == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "php_network_getaddresses: gethostbyname failed");
			return 0;
		}
		in = *((struct in_addr *) host_info->h_addr);
	}

	*sal = emalloc(2 * sizeof(*sal));
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

#if !defined(__BEOS__)
# define HAVE_NON_BLOCKING_CONNECT 1
# ifdef PHP_WIN32
typedef u_long php_non_blocking_flags_t;
#  define SET_SOCKET_BLOCKING_MODE(sock, save) \
     save = TRUE; ioctlsocket(sock, FIONBIO, &save)
#  define RESTORE_SOCKET_BLOCKING_MODE(sock, save) \
	 ioctlsocket(sock, FIONBIO, &save)
# else
typedef int php_non_blocking_flags_t;
#  define SET_SOCKET_BLOCKING_MODE(sock, save) \
	 save = fcntl(sock, F_GETFL, 0); \
	 fcntl(sock, F_SETFL, save | O_NONBLOCK)
#  define RESTORE_SOCKET_BLOCKING_MODE(sock, save) \
	 fcntl(sock, F_SETFL, save)
# endif
#endif




/* Connect to a socket using an interruptible connect with optional timeout.
 * Optionally, the connect can be made asynchronously, which will implicitly
 * enable non-blocking mode on the socket.
 * */
/* {{{ php_network_connect_socket */
PHPAPI int php_network_connect_socket(int sockfd,
		const struct sockaddr *addr,
		socklen_t addrlen,
		int asynchronous,
		struct timeval *timeout,
		char **error_string,
		int *error_code)
{
#if HAVE_NON_BLOCKING_CONNECT
	php_non_blocking_flags_t orig_flags;
	int n;
	int error = 0;
	socklen_t len;
	int ret = 0;
	fd_set rset;
	fd_set wset;
	fd_set eset;

	SET_SOCKET_BLOCKING_MODE(sockfd, orig_flags);
	
	if ((n = connect(sockfd, addr, addrlen)) < 0) {
		error = php_socket_errno();

		if (error_code) {
			*error_code = error;
		}

		if (error != EINPROGRESS) {
			if (error_string) {
				*error_string = php_socket_strerror(error, NULL, 0);
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

	FD_ZERO(&rset);
	FD_ZERO(&eset);
	FD_SET(sockfd, &rset);
	FD_SET(sockfd, &eset);

	wset = rset;

	if ((n = select(sockfd + 1, &rset, &wset, &eset, timeout)) == 0) {
		error = PHP_TIMEOUT_ERROR_VALUE;
	}

	if(FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
		len = sizeof(error);
		/*
		   BSD-derived systems set errno correctly
		   Solaris returns -1 from getsockopt in case of error
		   */
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
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

	if (error && error_string) {
		*error_string = php_socket_strerror(error, NULL, 0);
		ret = -1;
	}
	return ret;
#else
	if (asynchronous) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Asynchronous connect() not supported on this platform");
	}
	return connect(sockfd, addr, addrlen);
#endif
}
/* }}} */

/* {{{ php_connect_nonb */
PHPAPI int php_connect_nonb(int sockfd,
						const struct sockaddr *addr,
						socklen_t addrlen,
						struct timeval *timeout)
{
#if (!defined(__BEOS__) && !defined(PHP_WIN32)) && (defined(O_NONBLOCK) || defined(O_NDELAY))


	int flags;
	int n;
	int error = 0;
	socklen_t len;
	int ret = 0;
	fd_set rset;
	fd_set wset;
	fd_set eset;

	if (timeout == NULL)	{
		/* blocking mode */
		return connect(sockfd, addr, addrlen);
	}
	
	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	if ((n = connect(sockfd, addr, addrlen)) < 0) {
		if (errno != EINPROGRESS) {
			return -1;
		}
	}

	if (n == 0) {
		goto ok;
	}

	FD_ZERO(&rset);
	FD_ZERO(&eset);
	FD_SET(sockfd, &rset);
	FD_SET(sockfd, &eset);

	wset = rset;

	if ((n = select(sockfd + 1, &rset, &wset, &eset, timeout)) == 0) {
		error = ETIMEDOUT;
	}

	if(FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
		len = sizeof(error);
		/*
		   BSD-derived systems set errno correctly
		   Solaris returns -1 from getsockopt in case of error
		   */
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
			ret = -1;
		}
	} else {
		/* whoops: sockfd has disappeared */
		ret = -1;
	}

ok:
	fcntl(sockfd, F_SETFL, flags);

	if(error) {
		errno = error;
		ret = -1;
	}
	return ret;
#else /* !defined(PHP_WIN32) && ... */
#ifdef PHP_WIN32
	return php_connect_nonb_win32((SOCKET) sockfd, addr, addrlen, timeout);
#endif
	return connect(sockfd, addr, addrlen);
#endif
}
/* }}} */

#ifdef PHP_WIN32
/* {{{ php_connect_nonb_win32 */
PHPAPI int php_connect_nonb_win32(SOCKET sockfd,
						const struct sockaddr *addr,
						socklen_t addrlen,
						struct timeval *timeout)
{
	int error = 0, error_len, ret;
	u_long non_block = TRUE, block = FALSE;

	fd_set rset, wset;

	if (timeout == NULL)	{
		/* blocking mode */
		return connect(sockfd, addr, addrlen);
	}
	
	/* Set the socket to be non-blocking */
	ioctlsocket(sockfd, FIONBIO, &non_block);

	if (connect(sockfd, addr, addrlen) == SOCKET_ERROR) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) {
			return SOCKET_ERROR;
		}
	}

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);

	FD_ZERO(&wset);
	FD_SET(sockfd, &wset);

	if ((ret = select(sockfd + 1, &rset, &wset, NULL, timeout)) == 0) {
		WSASetLastError(WSAETIMEDOUT);
		return SOCKET_ERROR;
	}

	if (ret == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}

	if(FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
		error_len = sizeof(error);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char *) &error, &error_len) == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}
	} else {
		/* whoops: sockfd has disappeared */
		return SOCKET_ERROR;
	}

	/* Set the socket back to blocking */
	ioctlsocket(sockfd, FIONBIO, &block);

	if (error) { 
		WSASetLastError(error);
		return SOCKET_ERROR;
	}

	return 0;
}
/* }}} */
#endif

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

/* Connect to a remote host using an interruptible connect with optional timeout.
 * Optionally, the connect can be made asynchronously, which will implicitly
 * enable non-blocking mode on the socket.
 * Returns the connected (or connecting) socket, or -1 on failure.
 * */

/* {{{ php_network_connect_socket_to_host */
int php_network_connect_socket_to_host(const char *host, unsigned short port,
		int socktype, int asynchronous, struct timeval *timeout, char **error_string,
		int *error_code
		TSRMLS_DC)
{
	int num_addrs, sock, n, fatal = 0;
	struct sockaddr **sal, **psal, *sa;
	struct timeval working_timeout;
	socklen_t socklen;
#if HAVE_GETTIMEOFDAY
	struct timeval limit_time, time_now;
#endif

	num_addrs = php_network_getaddresses(host, &psal, error_string TSRMLS_CC);

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

		/* create a socket for this address */
		sock = socket(sa->sa_family, socktype, 0);

		if (sock == SOCK_ERR) {
			continue;
		}

		switch (sa->sa_family) {
#if HAVE_GETADDRINFO && HAVE_IPV6
			case AF_INET6:
				((struct sockaddr_in6 *)sa)->sin6_family = sa->sa_family;
				((struct sockaddr_in6 *)sa)->sin6_port = htons(port);
				socklen = sizeof(struct sockaddr_in6);
				break;
#endif
			case AF_INET:
				((struct sockaddr_in *)sa)->sin_family = sa->sa_family;
				((struct sockaddr_in *)sa)->sin_port = htons(port);
				socklen = sizeof(struct sockaddr_in);
				break;
			default:
				/* Unknown family */
				sa = NULL;
		}

		if (sa) {
			/* make a connection attempt */
			
			n = php_network_connect_socket(sock, sa, socklen, asynchronous,
					timeout ? &working_timeout : NULL,
					error_string, error_code);

			if (n != SOCK_CONN_ERR) {
				goto connected;
			}

			/* adjust timeout for next attempt */
#if HAVE_GETTIMEOFDAY
			if (timeout) {
				gettimeofday(&time_now, NULL);

				if (timercmp(&time_now, &limit_time, >=)) {
					/* time limit expired; don't attempt any further connections */
					fatal = 1;
				} else {
					/* work out remaining time */
					sub_times(limit_time, time_now, &working_timeout);
				}
			}
#else
			if (err == PHP_TIMEOUT_ERROR_VALUE) {
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
		}

		close(sock);
	}
	sock = -1;

connected:

	php_network_freeaddresses(psal);
	
	return sock;
}
/* }}} */



/* {{{ php_hostconnect
 * Creates a socket of type socktype and connects to the given host and
 * port, returns the created socket on success, else returns -1.
 * timeout gives timeout in seconds, 0 means blocking mode.
 */
int php_hostconnect(const char *host, unsigned short port, int socktype, struct timeval *timeout TSRMLS_DC)
{	
	int n, repeatto, s;
	struct sockaddr **sal, **psal;
	struct timeval individual_timeout;
	int set_timeout = 0;
	int err;
	
	n = php_network_getaddresses(host, &sal, NULL TSRMLS_CC);

	if (n == 0)
		return -1;
	
	if (timeout != NULL) {
		/* is this a good idea? 5s? */
		repeatto = timeout->tv_sec / n > 5;
		if (repeatto) {
			individual_timeout.tv_sec = timeout->tv_sec / n;
		} else {
			individual_timeout.tv_sec = timeout->tv_sec;
		}

		individual_timeout.tv_usec = timeout->tv_usec;
	} else {
		individual_timeout.tv_sec = 0;
		individual_timeout.tv_usec = 0;
	}
	
	/* Boolean indicating whether to pass a timeout */
	set_timeout = individual_timeout.tv_sec + individual_timeout.tv_usec;
	
	psal = sal;
	while (*sal != NULL) {
		s = socket((*sal)->sa_family, socktype, 0);
		if (s != SOCK_ERR) {
			switch ((*sal)->sa_family) {
#if defined( HAVE_GETADDRINFO ) && defined( HAVE_IPV6 )
				case AF_INET6:
					{
						struct sockaddr_in6 *sa =
							(struct sockaddr_in6 *)*sal;

						sa->sin6_family = (*sal)->sa_family;
						sa->sin6_port = htons(port);
						if (php_connect_nonb(s, (struct sockaddr *) sa,
									sizeof(*sa), (set_timeout) ? &individual_timeout : NULL) != SOCK_CONN_ERR)
							goto ok;
					} 
					break;
#endif
				case AF_INET:
					{
						struct sockaddr_in *sa =
							(struct sockaddr_in *)*sal;

						sa->sin_family = (*sal)->sa_family;
						sa->sin_port = htons(port);
						if (php_connect_nonb(s, (struct sockaddr *) sa,
									sizeof(*sa), (set_timeout) ? &individual_timeout : NULL) != SOCK_CONN_ERR)
							goto ok;

					} 
					break;
			}
#ifdef PHP_WIN32
			/* Preserve the last error */
			err = WSAGetLastError();
#else
			err = errno;
#endif
			close (s);
		}
		sal++;

		if (err == PHP_TIMEOUT_ERROR_VALUE) {
			/* if the first attempt timed out, it's highly likely
			 * that any subsequent attempts will do so also */
			break;
		}
		
	}
	php_network_freeaddresses(psal);
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "php_hostconnect: connect failed");

#ifdef PHP_WIN32
	/* Restore the last error */
	WSASetLastError(err);
#endif 

	return -1;

 ok:
	php_network_freeaddresses(psal);
	return s;
}
/* }}} */

/* {{{ php_any_addr
 * Fills the any (wildcard) address into php_sockaddr_storage
 */
void php_any_addr(int family, php_sockaddr_storage *addr, unsigned short port)
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
int php_sockaddr_size(php_sockaddr_storage *addr)
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
	}
	return buf;
#else
	char *sysbuf;
	int free_it = 1;

	if (!FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				err,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&sysbuf,
				0,
				NULL)) {
		free_it = 0;
		sysbuf = "Unknown Error";
	}

	if (buf == NULL) {
		buf = estrdup(sysbuf);
	} else {
		strncpy(buf, sysbuf, bufsize);
	}

	if (free_it) {
		LocalFree(sysbuf);
	}

	return buf;
#endif
}
/* }}} */

/* deprecated */
PHPAPI php_stream *_php_stream_sock_open_from_socket(int socket, const char *persistent_id STREAMS_DC TSRMLS_DC)
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
	stream->flags |= PHP_STREAM_FLAG_AVOID_BLOCKING;

	if (stream == NULL)	
		pefree(sock, persistent_id ? 1 : 0);

	return stream;
}

PHPAPI php_stream *_php_stream_sock_open_host(const char *host, unsigned short port,
		int socktype, struct timeval *timeout, const char *persistent_id STREAMS_DC TSRMLS_DC)
{
	char *res;
	long reslen;
	php_stream *stream;

	reslen = spprintf(&res, 0, "tcp://%s:%d", host, port);

	stream = php_stream_xport_create(res, reslen, ENFORCE_SAFE_MODE | REPORT_ERRORS,
			STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT, persistent_id, timeout, NULL, NULL, NULL);

	efree(res);

	return stream;
}

PHPAPI int php_set_sock_blocking(int socketd, int block TSRMLS_DC)
{
      int ret = SUCCESS;
      int flags;
      int myflag = 0;

#ifdef PHP_WIN32
      /* with ioctlsocket, a non-zero sets nonblocking, a zero sets blocking */
	  flags = !block;
	  if (ioctlsocket(socketd, FIONBIO, &flags)==SOCKET_ERROR){
		  php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", WSAGetLastError());
		  ret = FALSE;
      }
#else
      flags = fcntl(socketd, F_GETFL);
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
      fcntl(socketd, F_SETFL, flags);
#endif
      return ret;
}


/*
 * Local variables:
 * tab-width: 8
 * c-basic-offset: 8
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
