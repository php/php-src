/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
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
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"

#ifdef PHP_WIN32
#include <windows.h>
#include <winsock.h>
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

/* Rearranging... */
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
int		 inet_aton(const char *, struct in_addr *);
#endif

#include "php_network.h"

#if defined(PHP_WIN32) || defined(__riscos__) || defined(NETWARE)
#undef AF_UNIX
#endif

#if defined(AF_UNIX)
#include <sys/un.h>
#endif

#ifdef PHP_WIN32
# define SOCK_ERR INVALID_SOCKET
# define SOCK_CONN_ERR SOCKET_ERROR
#else
# define SOCK_ERR -1
# define SOCK_CONN_ERR -1
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
static int php_network_getaddresses(const char *host, struct sockaddr ***sal)
{
	struct sockaddr **sap;
	int n;

	if (host == NULL) {
		return 0;
	}

	{
#ifdef HAVE_GETADDRINFO
		struct addrinfo hints, *res, *sai;

		memset(&hints, '\0', sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		if ((n = getaddrinfo(host, NULL, &hints, &res))) {
			php_error(E_WARNING, "php_network_getaddresses: getaddrinfo failed: %s", PHP_GAI_STRERROR(n));
			return 0;
		} else if (res == NULL) {
			php_error(E_WARNING, "php_network_getaddresses: getaddrinfo failed (null result pointer)");
			return 0;
		}

		sai = res;
		for (n = 1; (sai = sai->ai_next) != NULL; n++);
		*sal = emalloc((n + 1) * sizeof(*sal));
		sai = res;
		sap = *sal;
		do {
			switch (sai->ai_family) {
#  ifdef HAVE_IPV6
			case AF_INET6:
				*sap = emalloc(sizeof(struct sockaddr_in6));
				*(struct sockaddr_in6 *)*sap =
					*((struct sockaddr_in6 *)sai->ai_addr);
				sap++;
				break;
#  endif
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
		struct hostent *host_info;
		struct in_addr in;

		if (!inet_aton(host, &in)) {
			/* XXX NOT THREAD SAFE */
			host_info = gethostbyname(host);
			if (host_info == NULL) {
				php_error(E_WARNING, "php_network_getaddresses: gethostbyname failed");
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
	}
	*sap = NULL;
	return n;
}
/* }}} */

/* {{{ php_connect_nonb */
PHPAPI int php_connect_nonb(int sockfd,
						const struct sockaddr *addr,
						socklen_t addrlen,
						struct timeval *timeout)
{
	/* probably won't work on Win32, someone else might try it (read: fix it ;) */

#if (!defined(__BEOS__) && !defined(PHP_WIN32)) && (defined(O_NONBLOCK) || defined(O_NDELAY))

#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif

	int flags;
	int n;
	int error = 0;
	socklen_t len;
	int ret = 0;
	fd_set rset;
	fd_set wset;

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
	FD_SET(sockfd, &rset);

	wset = rset;

	if ((n = select(sockfd + 1, &rset, &wset, NULL, timeout)) == 0) {
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
	return connect(sockfd, addr, addrlen);
#endif
}
/* }}} */

/* {{{ php_hostconnect
 * Creates a socket of type socktype and connects to the given host and
 * port, returns the created socket on success, else returns -1.
 * timeout gives timeout in seconds, 0 means blocking mode.
 */
int php_hostconnect(const char *host, unsigned short port, int socktype, int timeout)
{	
	int n, repeatto, s;
	struct sockaddr **sal, **psal;
	struct timeval timeoutval;
	
	n = php_network_getaddresses(host, &sal);

	if (n == 0)
		return -1;
	
	/* is this a good idea? 5s? */
	repeatto = timeout / n > 5;
	if (repeatto) {
		timeout /= n;
	}
	timeoutval.tv_sec = timeout;
	timeoutval.tv_usec = 0;

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
									sizeof(*sa), timeout ? &timeoutval : NULL) != SOCK_CONN_ERR)
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
									sizeof(*sa), timeout ? &timeoutval : NULL) != SOCK_CONN_ERR)
							goto ok;

					} 
					break;
			}
			close (s);
		}
		sal++;
		if (repeatto) {
			timeoutval.tv_sec = timeout;
			timeoutval.tv_usec = 0;
		}
	}
	php_network_freeaddresses(psal);
	php_error(E_WARNING, "php_hostconnect: connect failed");
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
		sin->sin_addr.s_addr = INADDR_ANY;
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

/*
 * Local variables:
 * tab-width: 8
 * c-basic-offset: 8
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
