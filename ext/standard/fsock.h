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
   | Authors: Paul Panotzki - Bunyip Information Systems                  |
   |          Jim Winstead (jimw@php.net)                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Synced with php 3.0 revision 1.24 1999-06-18 [ssb] */

#ifndef FSOCK_H
#define FSOCK_H

#include "file.h"

#ifdef PHP_WIN32
# ifndef WINNT
#  define WINNT 1
# endif
# undef FD_SETSIZE
# include "arpa/inet.h"
# define socklen_t unsigned int
#endif

#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#define PHP_FSOCK_CHUNK_SIZE 8192

struct php_sockbuf {
	int socket;
	unsigned char *readbuf;
	size_t readbuflen;
	size_t readpos;
	size_t writepos;
	struct php_sockbuf *next;
	struct php_sockbuf *prev;
	char eof;
	char persistent;
	char is_blocked;
	size_t chunk_size;
	struct timeval timeout;
	char timeout_event;
};

typedef struct php_sockbuf php_sockbuf;

PHP_FUNCTION(fsockopen);
PHP_FUNCTION(pfsockopen);

int lookup_hostname(const char *addr, struct in_addr *in);
char *php_sock_fgets(char *buf, size_t maxlen, int socket);
size_t php_sock_fread(char *buf, size_t maxlen, int socket);
int php_sock_feof(int socket);
int php_sock_fgetc(int socket);
int php_is_persistent_sock(int);
int php_sockset_blocking(int socket, int mode);
void php_sockset_timeout(int socket, struct timeval *timeout);
int php_sockdestroy(int socket);
int php_sock_close(int socket);
size_t php_sock_set_def_chunk_size(size_t size);
void php_msock_destroy(int *data);
void php_cleanup_sockbuf(int persistent FLS_DC);

PHPAPI int connect_nonb(int sockfd, struct sockaddr *addr, socklen_t addrlen, struct timeval *timeout);
PHPAPI struct php_sockbuf *php_get_socket(int socket);

PHP_MINIT_FUNCTION(fsock);
PHP_MSHUTDOWN_FUNCTION(fsock);
PHP_RSHUTDOWN_FUNCTION(fsock);

#endif /* FSOCK_H */
