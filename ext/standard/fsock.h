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

#ifndef _FSOCK_H
#define _FSOCK_H

extern php3_module_entry fsock_module_entry;
#define fsock_module_ptr &fsock_module_entry

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

PHPAPI int connect_nonb(int sockfd, struct sockaddr *addr, int addrlen, struct timeval *timeout);

#endif /* _FSOCK_H */
