/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Anatol Belski <ab@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "phpdbg_io.h"

#ifdef PHP_WIN32
#undef UNICODE
#include "win32/inet.h"
#include <winsock2.h>
#include <windows.h>
#include <Ws2tcpip.h>
#include "win32/sockets.h"

#else

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <netdb.h>

#include <fcntl.h>

#include <poll.h>
#endif


ZEND_EXTERN_MODULE_GLOBALS(phpdbg);


PHPDBG_API int
phpdbg_consume_bytes(int sock, char *ptr, int len, int tmo)
{/*{{{*/
	int got_now, i = len, j;
	char *p = ptr;
#ifndef PHP_WIN32
	struct pollfd pfd;

	pfd.fd = sock;
	pfd.events = POLLIN;

	j = poll(&pfd, 1, tmo);

	if (j == 0) {
#else
	struct fd_set readfds;
	struct timeval ttmo;

	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);

	ttmo.tv_sec = 0;
	ttmo.tv_usec = tmo*1000;

	j = select(0, &readfds, NULL, NULL, &ttmo);

	if (j <= 0) {
#endif
		return -1;
	}

	while(i > 0) {
#ifdef PHP_WIN32
		int can_read = recv(sock, p, i, MSG_PEEK);

		i = can_read;
#endif
		got_now = recv(sock, p, i, 0);
		if (got_now == -1) {
			return -1;
		}
		i -= got_now;
		p += got_now;
	}

	return p - ptr;
}/*}}}*/

PHPDBG_API int
phpdbg_send_bytes(int sock, char *ptr, int len)
{/*{{{*/
	int sent, i = len;
	char *p = ptr;

	while(i > 0) {
		sent = send(sock, p, i, 0);
		if (sent == -1) {
			return -1;
		}
		i -= sent;
		p += sent;
	}

	return len;
}/*}}}*/


PHPDBG_API int
phpdbg_mixed_read(int sock, char *ptr, int len, int tmo TSRMLS_CC)
{/*{{{*/
	if (PHPDBG_G(flags) & PHPDBG_IS_REMOTE) {
		return phpdbg_consume_bytes(sock, ptr, len, tmo);
	}

	return read(sock, ptr, len);
}/*}}}*/


PHPDBG_API int
phpdbg_mixed_write(int sock, char *ptr, int len TSRMLS_CC)
{/*{{{*/
	if (PHPDBG_G(flags) & PHPDBG_IS_REMOTE) {
		return phpdbg_send_bytes(sock, ptr, len);
	}

	return write(sock, ptr, len);
}/*}}}*/
