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

/* Synced with php3 revision 1.24 1999-06-18 [ssb] */

#ifndef _FSOCK_H
#define _FSOCK_H

#if WIN32|WINNT
# ifndef WINNT
#  define WINNT 1
# endif
# undef FD_SETSIZE
# include "arpa/inet.h"
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

extern php3_module_entry fsock_module_entry;
#define phpext_fsock_ptr &fsock_module_entry

PHP_FUNCTION(fsockopen);
PHP_FUNCTION(pfsockopen);
int lookup_hostname(const char *addr, struct in_addr *in);
char *_php3_sock_fgets(char *buf, size_t maxlen, int socket);
size_t _php3_sock_fread(char *buf, size_t maxlen, int socket);
int _php3_sock_feof(int socket);
int _php3_sock_fgetc(int socket);
int _php3_is_persistent_sock(int);
int _php3_sock_set_blocking(int socket, int mode);
int _php3_sock_destroy(int socket);
int _php3_sock_close(int socket);
size_t _php3_sock_set_def_chunk_size(size_t size);
int php_msock_destroy(int *data);

PHPAPI int connect_nonb(int sockfd, struct sockaddr *addr, int addrlen, struct timeval *timeout);

PHP_MINIT_FUNCTION(fsock);
PHP_MSHUTDOWN_FUNCTION(fsock);
PHP_RSHUTDOWN_FUNCTION(fsock);

typedef struct {
	HashTable ht_fsock_keys;
	HashTable ht_fsock_socks;
	struct php3i_sockbuf *phpsockbuf;
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
#define FLS_D
#define FLS_DC
#define FLS_C
#define FLS_CC
#define FG(v) (fsock_globals.v)
#define FLS_FETCH()
#endif

#endif /* _FSOCK_H */
