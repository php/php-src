/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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

#ifdef PHP_WIN32

#include <stdio.h>
#include <fcntl.h>

#include "php.h"
#include "php_sockets.h"
#include "php_sockets_win.h"

ssize_t readv(SOCKET sock, const struct iovec *iov, int iovcnt) {
	size_t bytes, remain, len, pos = 0;
	ssize_t retval;
	int i;
	char *buffer = NULL;

	for(bytes=0, i=0; i<iovcnt; i++) {
		bytes += iov[i].iov_len;
	}

	buffer = (char*)emalloc(bytes);
	if (buffer == NULL) {
		return -1;
	}

	retval = recv(sock, buffer, bytes, 0);

	if(retval < 0) {
		efree(buffer);
		return retval;
	}

	remain = bytes = (size_t) retval;
	
	for(i=0; i<iovcnt; i++) {
		len = ((unsigned int)iov[i].iov_len < remain) ? iov[i].iov_len : remain;
		memcpy(iov[i].iov_base, buffer+pos, len);
		pos += len;
		remain -= len;
	}

	efree(buffer);
	return bytes;
}

ssize_t writev(SOCKET sock, const struct iovec *iov, int iovcnt) {
	size_t bytes, pos = 0;
	ssize_t retval;
	int i;
	char *buffer = NULL;

	for(bytes=0, i=0; i<iovcnt; i++) {
		bytes += iov[i].iov_len;
	}

	buffer = (char*)emalloc(bytes);
	
	if(buffer == NULL) {
		return -1;
	}

	for(i=0; i<iovcnt; i++) {
		memcpy(buffer+pos, iov[i].iov_base, iov[i].iov_len);
		pos += iov[i].iov_len;
	}

	retval = send(sock, buffer, bytes, 0);
	efree(buffer);
	
	return retval;
}

ssize_t recvmsg(SOCKET sock, struct msghdr *msg, int flags) {
	set_errno(WSAEOPNOTSUPP);
	return -1;
}

ssize_t sendmsg(SOCKET sock, struct msghdr *msg, int flags) {
	set_errno(WSAEOPNOTSUPP);
	return -1;
}

int socketpair(int domain, int type, int protocol, SOCKET sock[2]) {
	struct sockaddr_in address;
	SOCKET redirect;
	int size = sizeof(address);

	if(domain != AF_INET) {
		set_errno(WSAENOPROTOOPT);
		return -1;
	}


	sock[0] = socket(domain, type, protocol);
	address.sin_addr.s_addr	= INADDR_ANY;
	address.sin_family		= AF_INET;
	address.sin_port		= 0;

	bind(sock[0], (struct sockaddr*)&address, sizeof(address));
	if(getsockname(sock[0], (struct sockaddr *)&address, &size) != 0) {

    }

	listen(sock[0], 2);
	sock[1] = socket(domain, type, protocol);	
	address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	connect(sock[1], (struct sockaddr*)&address, sizeof(address));
	redirect = accept(sock[0],(struct sockaddr*)&address, &size);

	close(sock[0]);
	sock[0] = redirect;

	if(sock[0] == INVALID_SOCKET ) {
		close(sock[0]);
		close(sock[1]);
		set_errno(WSAECONNABORTED);
		return -1;
	}
	
	return 0;
}

int inet_aton(const char *cp, struct in_addr *inp) {
  inp->s_addr = inet_addr(cp);

  if (inp->s_addr == INADDR_NONE) {
	  return 0;
  }

  return 1;
}

int fcntl(int fd, int cmd, ...) {
	va_list va;
	int retval, io, mode;
	
	va_start(va, cmd);

	switch(cmd) {
		case F_GETFL:
		case F_SETFD:
		case F_GETFD:
		default:
			retval = -1;
			break;

		case F_SETFL:
			io = va_arg(va, int);
			mode = io == O_NONBLOCK ? 1 : 0;
			retval = ioctlsocket(fd, io, &mode);
			break;
	}

	va_end(va);
	return retval;
}
#endif
