/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Paul Panotzki - Bunyip Information Systems                  |
   |          Jim Winstead (jimw@php.net)                                 |
   +----------------------------------------------------------------------+
*/
/* $Id$ */

/* Synced with php 3.0 revision 1.24 1999-06-18 [ssb] */

#ifndef FSOCK_H
#define FSOCK_H

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

PHPAPI int connect_nonb(int sockfd, struct sockaddr *addr, socklen_t addrlen, struct timeval *timeout);
PHPAPI struct php_sockbuf *php_get_socket(int socket);

PHP_MINIT_FUNCTION(fsock);
PHP_MSHUTDOWN_FUNCTION(fsock);
PHP_RSHUTDOWN_FUNCTION(fsock);

typedef struct {
	HashTable ht_fsock_keys;
	HashTable ht_fsock_socks;
	struct php_sockbuf *phpsockbuf;
	size_t def_chunk_size;
} php_fsock_globals;

#ifdef ZTS
#define FLS_D php_fsock_globals *fsock_globals
#define FLS_DC , FLS_D
#define FLS_C fsock_globals
#define FLS_CC , FLS_C
#define FG(v) (fsock_globals->v)
#define FLS_FETCH() php_fsock_globals *fsock_globals = ts_resource(fsock_globals_id)
#else
#define FLS_D	void
#define FLS_DC
#define FLS_C
#define FLS_CC
#define FG(v) (fsock_globals.v)
#define FLS_FETCH()
#endif

#endif /* FSOCK_H */
