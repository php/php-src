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

#if WIN32|WINNT
#	ifndef WINNT
#	define WINNT 1
#	endif
#undef FD_SETSIZE
#include "arpa/inet.h"
#endif

#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

extern php3_module_entry fsock_module_entry;
#define fsock_module_ptr &fsock_module_entry

extern PHP_FUNCTION(fsockopen);
extern PHP_FUNCTION(pfsockopen);
extern int lookup_hostname(const char *addr, struct in_addr *in);
extern int _php3_sock_fgets(char *buf, int maxlen, int socket);
extern int _php3_sock_fread(char *buf, int maxlen, int socket);
extern int _php3_is_persistent_sock(int);

#endif /* _FSOCK_H */
