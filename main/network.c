/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Venaas <venaas@uninett.no>                             |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"

#ifdef PHP_WIN32
#include <windows.h>
#include <winsock.h>
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#else
#include <sys/param.h>
#endif

#include <sys/types.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifndef PHP_WIN32
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#ifdef PHP_WIN32
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

static void php_network_freeaddresses(struct sockaddr **sal)
{
	struct sockaddr **sap;

	if (sal == NULL)
		return;
	for (sap = sal; *sap != NULL; sap++)
		free(*sap);
	free(sal);
}

static int php_network_getaddresses(const char *host, struct sockaddr ***sal)
{
	struct sockaddr **sap;
	
        if (host != NULL) {
#ifdef HAVE_GETADDRINFO
                struct addrinfo hints, *res, *sai;
		int n;

                memset( &hints, '\0', sizeof(hints) );
                hints.ai_family = AF_UNSPEC;
                if (getaddrinfo(host, NULL, &hints, &res))
                        return -1;
                sai = res;
		for (n=2; (sai = sai->ai_next) != NULL; n++);
		*sal = malloc(n * sizeof(*sal));
		if (*sal == NULL)
			return -1;

                sai = res;
		sap = *sal;
                do {
                        switch (sai->ai_family) {
#  ifdef AF_INET6
                        case AF_INET6: {
				*sap = malloc(sizeof(struct sockaddr_in6));
				if (*sap == NULL) {
					freeaddrinfo(res);
					goto errexit;
				}
				*(struct sockaddr_in6 *)*sap =
					*((struct sockaddr_in6 *)sai->ai_addr);
                        } break;
#  endif
                        case AF_INET: {
				*sap = malloc(sizeof(struct sockaddr_in));
				if (*sap == NULL) {
					freeaddrinfo(res);
					goto errexit;
				}
				*(struct sockaddr_in *)*sap =
					*((struct sockaddr_in *)sai->ai_addr);
                        } break;
                        }
			sap++;
                } while ((sai = sai->ai_next) != NULL);
                freeaddrinfo(res);
#else
		struct hostent *host_info;
		struct in_addr in;

		if (!inet_aton(host, &in)) {
			/* XXX NOT THREAD SAFE */
			host_info = gethostbyname(host);
			if (host_info == NULL)
				return -1;
			in = *((struct in_addr *) host_info->h_addr);
		}

		*sal = malloc(2 * sizeof(*sal));
		if (*sal == NULL)
			return -1;
		sap = *sal;
		*sap = malloc(sizeof(struct sockaddr_in));
		if (*sap == NULL)
			goto errexit;

		(*sap)->sa_family = AF_INET;
		((struct sockaddr_in *)*sap)->sin_addr = in;
		sap++;
#endif
		*sap = NULL;
		return 0;
	errexit:
		php_network_freeaddresses(*sal);
        }
	return -1;
}

/*
 * Creates a socket of type socktype and connects to the given host and
 * port, returns the created socket on success, else returns -1.
 * timeout gives timeout in seconds, 0 means blocking mode.
 */
int hostconnect(char *host, int port, int socktype, int timeout)
{	
	int s;
	struct sockaddr **sal, **psal;

	if (php_network_getaddresses(host, &sal))
		return -1;
	
	psal = sal;
	while (*sal != NULL) {
		s = socket((*sal)->sa_family, socktype, 0);
		if (s != SOCK_ERR) {
			switch ((*sal)->sa_family) {
#if defined( HAVE_GETADDRINFO ) && defined( AF_INET6 )
			case AF_INET6: {
				struct sockaddr_in6 *sa =
					(struct sockaddr_in6 *)*sal;
				
				sa->sin6_family = (*sal)->sa_family;
				sa->sin6_port = htons(port);
				if (connect(s, (struct sockaddr *) sa,
					    sizeof(*sa)) != SOCK_CONN_ERR)
					goto ok;
			} break;
#endif
			case AF_INET: {
				struct sockaddr_in *sa =
					(struct sockaddr_in *)*sal;

				sa->sin_family = (*sal)->sa_family;
				sa->sin_port = htons(port);
				if (connect(s, (struct sockaddr *) sa,
					    sizeof(*sa)) != SOCK_CONN_ERR)
					goto ok;

			} break;
			}
			close (s);
		}
		sal++;
	}
	php_network_freeaddresses(psal);
	return -1;

 ok:
	php_network_freeaddresses(psal);
	return s;
}

/*
 * Local variables:
 * tab-width: 8
 * c-basic-offset: 8
 * End:
 */
