/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

/* is easy to generalize ... but not needed for now */
PHPDBG_API int phpdbg_consume_stdin_line(char *buf) {
	int bytes = PHPDBG_G(input_buflen), len = 0;

	if (PHPDBG_G(input_buflen)) {
		memcpy(buf, PHPDBG_G(input_buffer), bytes);
	}

	PHPDBG_G(last_was_newline) = 1;

	do {
		int i;
		if (bytes <= 0) {
			continue;
		}

		for (i = len; i < len + bytes; i++) {
			if (buf[i] == '\x03') {
				if (i != len + bytes - 1) {
					memmove(buf + i, buf + i + 1, len + bytes - i - 1);
				}
				len--;
				i--;
				continue;
			}
			if (buf[i] == '\n') {
				PHPDBG_G(input_buflen) = len + bytes - 1 - i;
				if (PHPDBG_G(input_buflen)) {
					memcpy(PHPDBG_G(input_buffer), buf + i + 1, PHPDBG_G(input_buflen));
				}
				if (i != PHPDBG_MAX_CMD - 1) {
					buf[i + 1] = 0;
				}
				return i;
			}
		}

		len += bytes;
	} while ((bytes = phpdbg_mixed_read(PHPDBG_G(io)[PHPDBG_STDIN].fd, buf + len, PHPDBG_MAX_CMD - len, -1)) > 0);

	if (bytes <= 0) {
		PHPDBG_G(flags) |= PHPDBG_IS_QUITTING | PHPDBG_IS_DISCONNECTED;
		zend_bailout();
	}

	return bytes;
}

PHPDBG_API int phpdbg_consume_bytes(int sock, char *ptr, int len, int tmo) {
	int got_now, i = len, j;
	char *p = ptr;
#ifndef PHP_WIN32
	struct pollfd pfd;

	if (tmo < 0) goto recv_once;
	pfd.fd = sock;
	pfd.events = POLLIN;

	j = poll(&pfd, 1, tmo);

	if (j == 0) {
#else
	struct fd_set readfds;
	struct timeval ttmo;

	if (tmo < 0) goto recv_once;
	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);

	ttmo.tv_sec = 0;
	ttmo.tv_usec = tmo*1000;

	j = select(0, &readfds, NULL, NULL, &ttmo);

	if (j <= 0) {
#endif
		return -1;
	}

recv_once:
	while(i > 0) {
		if (tmo < 0) {
			/* There's something to read. Read what's available and proceed
			disregarding whether len could be exhausted or not.*/
			int can_read = recv(sock, p, i, MSG_PEEK);
#ifndef _WIN32
			if (can_read == -1 && errno == EINTR) {
				continue;
			}
#endif
			i = can_read;
		}

#ifdef _WIN32
		got_now = recv(sock, p, i, 0);
#else
		do {
			got_now = recv(sock, p, i, 0);
		} while (got_now == -1 && errno == EINTR);
#endif

		if (got_now == -1) {
			zend_quiet_write(PHPDBG_G(io)[PHPDBG_STDERR].fd, ZEND_STRL("Read operation timed out!\n"));
			return -1;
		}
		i -= got_now;
		p += got_now;
	}

	return p - ptr;
}

PHPDBG_API int phpdbg_send_bytes(int sock, const char *ptr, int len) {
	int sent, i = len;
	const char *p = ptr;
/* XXX poll/select needed here? */
	while(i > 0) {
		sent = send(sock, p, i, 0);
		if (sent == -1) {
			return -1;
		}
		i -= sent;
		p += sent;
	}

	return len;
}


PHPDBG_API int phpdbg_mixed_read(int sock, char *ptr, int len, int tmo) {
	int ret;

	if (PHPDBG_G(flags) & PHPDBG_IS_REMOTE) {
		return phpdbg_consume_bytes(sock, ptr, len, tmo);
	}

	do {
		ret = read(sock, ptr, len);
	} while (ret == -1 && errno == EINTR);

	return ret;
}

static int phpdbg_output_pager(int sock, const char *ptr, int len) {
	int count = 0, bytes = 0;
	const char *p = ptr, *endp = ptr + len;

	while ((p = memchr(p, '\n', endp - p))) {
		count++;
		p++;

		if (count % PHPDBG_G(lines) == 0) {
			bytes += write(sock, ptr + bytes, (p - ptr) - bytes);

			if (memchr(p, '\n', endp - p)) {
				char buf[PHPDBG_MAX_CMD];
				zend_quiet_write(sock, ZEND_STRL("\r---Type <return> to continue or q <return> to quit---"));
				phpdbg_consume_stdin_line(buf);
				if (*buf == 'q') {
					break;
				}
				zend_quiet_write(sock, "\r", 1);
			} else break;
		}
	}
	if (bytes && count % PHPDBG_G(lines) != 0) {
		bytes += write(sock, ptr + bytes, len - bytes);
	} else if (!bytes) {
		bytes += write(sock, ptr, len);
	}
	return bytes;
}

PHPDBG_API int phpdbg_mixed_write(int sock, const char *ptr, int len) {
	if (PHPDBG_G(flags) & PHPDBG_IS_REMOTE) {
		return phpdbg_send_bytes(sock, ptr, len);
	}

	if ((PHPDBG_G(flags) & PHPDBG_HAS_PAGINATION)
	 && !(PHPDBG_G(flags) & PHPDBG_WRITE_XML)
	 && PHPDBG_G(io)[PHPDBG_STDOUT].fd == sock
	 && PHPDBG_G(lines) > 0) {
		return phpdbg_output_pager(sock, ptr, len);
	}

	return write(sock, ptr, len);
}

PHPDBG_API int phpdbg_open_socket(const char *interface, unsigned short port) {
	struct addrinfo res;
	int fd = phpdbg_create_listenable_socket(interface, port, &res);

	if (fd == -1) {
		return -1;
	}

	if (bind(fd, res.ai_addr, res.ai_addrlen) == -1) {
		phpdbg_close_socket(fd);
		return -4;
	}

	listen(fd, 5);

	return fd;
}


PHPDBG_API int phpdbg_create_listenable_socket(const char *addr, unsigned short port, struct addrinfo *addr_res) {
	int sock = -1, rc;
	int reuse = 1;
	struct in6_addr serveraddr;
	struct addrinfo hints, *res = NULL;
	char port_buf[8];
	int8_t any_addr = *addr == '*';

	do {
		memset(&hints, 0, sizeof hints);
		if (any_addr) {
			hints.ai_flags = AI_PASSIVE;
		} else {
			hints.ai_flags = AI_NUMERICSERV;
		}
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		rc = inet_pton(AF_INET, addr, &serveraddr);
		if (1 == rc) {
			hints.ai_family = AF_INET;
			if (!any_addr) {
				hints.ai_flags |= AI_NUMERICHOST;
			}
		} else {
			rc = inet_pton(AF_INET6, addr, &serveraddr);
			if (1 == rc) {
				hints.ai_family = AF_INET6;
				if (!any_addr) {
					hints.ai_flags |= AI_NUMERICHOST;
				}
			} else {
				/* XXX get host by name ??? */
			}
		}

		snprintf(port_buf, sizeof(port_buf), "%u", port);
		if (!any_addr) {
			rc = getaddrinfo(addr, port_buf, &hints, &res);
		} else {
			rc = getaddrinfo(NULL, port_buf, &hints, &res);
		}

		if (0 != rc) {
#ifndef PHP_WIN32
			if (rc == EAI_SYSTEM) {
				char buf[128];

				snprintf(buf, sizeof(buf), "Could not translate address '%s'", addr);

				zend_quiet_write(PHPDBG_G(io)[PHPDBG_STDERR].fd, buf, strlen(buf));

				return sock;
			} else {
#endif
				char buf[256];

				snprintf(buf, sizeof(buf), "Host '%s' not found. %s", addr, estrdup(gai_strerror(rc)));

				zend_quiet_write(PHPDBG_G(io)[PHPDBG_STDERR].fd, buf, strlen(buf));

				return sock;
#ifndef PHP_WIN32
			}
#endif
			return sock;
		}

		if ((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
			const char *msg = "Unable to create socket";

			zend_quiet_write(PHPDBG_G(io)[PHPDBG_STDERR].fd, msg, strlen(msg));

			return sock;
		}

		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse)) == -1) {
			phpdbg_close_socket(sock);
			return sock;
		}


	} while (0);

	*addr_res = *res;

	return sock;
}

PHPDBG_API void phpdbg_close_socket(int sock) {
	if (sock >= 0) {
#ifdef _WIN32
		closesocket(sock);
#else
		shutdown(sock, SHUT_RDWR);
		close(sock);
#endif
	}
}
