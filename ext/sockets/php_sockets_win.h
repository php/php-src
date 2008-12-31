/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Chris Vandomelen <chrisv@b0rked.dhs.org>                    |
   |          Sterling Hughes  <sterling@php.net>                         |
   |                                                                      |
   | WinSock: Daniel Beulshausen <daniel@php4win.de>                      |
   +----------------------------------------------------------------------+
 */

/* $Id$ */


#ifdef PHP_WIN32

#define EPROTONOSUPPORT	WSAEPROTONOSUPPORT
#define ECONNRESET		WSAECONNRESET

#ifdef errno
#undef errno
#endif

#define errno WSAGetLastError()
#define h_errno WSAGetLastError()
#define set_errno(a) WSASetLastError(a)
#define close(a) closesocket(a)

struct	sockaddr_un {
	short	sun_family;
	char	sun_path[108];
};

int socketpair(int domain, int type, int protocol, SOCKET sock[2]);
int inet_aton(const char *cp, struct in_addr *inp);

#endif
