/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Georg Richter <georg@php.net>                                |
  +----------------------------------------------------------------------+
*/

#include <string.h>
#include <sys/types.h>
#include <errno.h>
#ifndef PHP_WIN32
#include <unistd.h>
#endif

#ifdef PHP_WIN32
# include <winsock.h>
# include <process.h>
# include <direct.h>
# include "win32/time.h"
#define PATH_MAX MAX_PATH
#else
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
#endif

#include "mysqli_profiler_com.h"

#ifdef PHP_WIN32
int inet_aton(const char *cp, struct in_addr *inp)
{
	inp->s_addr = inet_addr(cp);
	if (inp->s_addr == INADDR_NONE) {
		return 0;
	}
	return 1;
}
#endif

/*
 * Converts a host name to an IP address.  */
static int lookup_hostname (const char *addr, struct in_addr *in)
{
	struct hostent *host_info;

	if (!inet_aton(addr, in)) {
		host_info = gethostbyname(addr);
		if (host_info == 0) {
			/* Error: unknown host */
			return -1;
		}
		*in = *((struct in_addr *) host_info->h_addr);
	}
	return 0;
}
/* }}} */

int php_mysqli_create_socket(const char *hostname, int dport)
{
	struct sockaddr_in address;
	int                err = -1;
	int                sockfd;

	memset(&address, 0, sizeof(address));
	lookup_hostname(hostname, &address.sin_addr);
	address.sin_family = AF_INET;
	address.sin_port = htons((unsigned short)dport);

	sockfd = socket(address.sin_family, SOCK_STREAM, 0);
	if (sockfd == SOCK_ERR) {
		return -1;
	}
	while ((err = connect(sockfd, (struct sockaddr *) &address,
						  sizeof(address))) == SOCK_ERR && errno == EAGAIN);

	if (err < 0) {
		PR_SCLOSE(sockfd);
		return -1;
	}
	return sockfd;
}

void php_mysqli_close_socket(int socket)
{
	PR_SCLOSE(socket);
}
