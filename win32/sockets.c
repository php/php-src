/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Chris Vandomelen <chrisv@b0rked.dhs.org>                    |
   |          Sterling Hughes  <sterling@php.net>                         |
   |                                                                      |
   | WinSock: Daniel Beulshausen <daniel@php4win.de>                      |
   +----------------------------------------------------------------------+
 */

/* Code originally from ext/sockets */

#include <stdio.h>
#include <fcntl.h>

#include "php.h"

PHPAPI int socketpair_win32(int domain, int type, int protocol, SOCKET sock[2], int overlapped)
{
	struct sockaddr_in address;
	SOCKET redirect;
	int size = sizeof(address);

	if (domain != AF_INET) {
		WSASetLastError(WSAENOPROTOOPT);
		return -1;
	}

	sock[1] = redirect = INVALID_SOCKET;

	sock[0] = socket(domain, type, protocol);
	if (INVALID_SOCKET == sock[0]) {
		goto error;
	}

	address.sin_addr.s_addr	= INADDR_ANY;
	address.sin_family = AF_INET;
	address.sin_port = 0;

	if (bind(sock[0], (struct sockaddr *) &address, sizeof(address)) != 0) {
		goto error;
	}

	if (getsockname(sock[0], (struct sockaddr *) &address, &size) != 0) {
		goto error;
	}

	if (listen(sock[0], 2) != 0) {
		goto error;
	}

	if (overlapped) {
		sock[1] = socket(domain, type, protocol);
	} else {
		sock[1] = WSASocket(domain, type, protocol, NULL, 0, 0);
	}

	if (INVALID_SOCKET == sock[1]) {
		goto error;
	}

	address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (connect(sock[1], (struct sockaddr *) &address, sizeof(address)) != 0) {
		goto error;
	}

	redirect = accept(sock[0], (struct sockaddr *) &address, &size);
	if (INVALID_SOCKET == redirect) {
		goto error;
	}

	closesocket(sock[0]);
	sock[0] = redirect;

	return 0;

error:
	closesocket(redirect);
	closesocket(sock[0]);
	closesocket(sock[1]);
	WSASetLastError(WSAECONNABORTED);
	return -1;
}

PHPAPI int socketpair(int domain, int type, int protocol, SOCKET sock[2])
{
	return socketpair_win32(domain, type, protocol, sock, 1);
}
