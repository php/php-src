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
extern int lookup_hostname(const char *addr, struct in_addr *in);
extern char *_php3_sock_fgets(char *buf, size_t maxlen, int socket);
extern size_t _php3_sock_fread(char *buf, size_t maxlen, int socket);
extern int _php3_sock_feof(int socket);
extern int _php3_sock_fgetc(int socket);
extern int _php3_is_persistent_sock(int);
extern int _php3_sock_set_blocking(int socket, int mode);
extern int _php3_sock_destroy(int socket);
extern int _php3_sock_close(int socket);
size_t _php3_sock_set_def_chunk_size(size_t size);

PHPAPI int connect_nonb(int sockfd, struct sockaddr *addr, int addrlen, struct timeval *timeout);

PHP_MINIT_FUNCTION(fsock);
PHP_MSHUTDOWN_FUNCTION(fsock);
PHP_RSHUTDOWN_FUNCTION(fsock);

#endif /* _FSOCK_H */
