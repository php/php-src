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

/* Copied lookup_hostname, will be replaced */

/*
 * Converts a host name to an IP address.
 */
static int my_lookup_hostname(const char *addr, struct in_addr *in)
{
        struct hostent *host_info;

        if (!inet_aton(addr, in)) {
                /* XXX NOT THREAD SAFE */
                host_info = gethostbyname(addr);
                if (host_info == 0) {
                        /* Error: unknown host */
                        return -1;
                }
                *in = *((struct in_addr *) host_info->h_addr);
        }
        return 0;
}

/*
 * Creates a socket of type socktype and connects to the given host and
 * port, returns the created socket on success, else returns -1.
 */
int hostconnect(char *host, int port, int socktype)
{	
	int s;
	struct sockaddr_in sa;

	if (my_lookup_hostname(host, &sa.sin_addr))
		return -1;
	s = socket(PF_INET, socktype, 0);
	if (s == SOCK_ERR)
		return -1;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	if (connect(s, (struct sockaddr *) &sa, sizeof(sa)) == SOCK_CONN_ERR) {
		close (s);
		return -1;
	}
	return s;
}

/*
 * Local variables:
 * tab-width: 8
 * c-basic-offset: 8
 * End:
 */
