/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2006 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "fastcgi.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#ifdef _WIN32

#include <windows.h>

	typedef unsigned int size_t;

	struct sockaddr_un {
		short   sun_family;
		char    sun_path[MAXPATHLEN];
	};

	static HANDLE fcgi_accept_mutex = INVALID_HANDLE_VALUE;
	static int is_impersonate = 0;

#define FCGI_LOCK(fd) \
	if (fcgi_accept_mutex != INVALID_HANDLE_VALUE) { \
		DWORD ret; \
		while ((ret = WaitForSingleObject(fcgi_accept_mutex, 1000)) == WAIT_TIMEOUT) { \
			if (in_shutdown) return -1; \
		} \
		if (ret == WAIT_FAILED) { \
			fprintf(stderr, "WaitForSingleObject() failed\n"); \
			return -1; \
		} \
	}

#define FCGI_UNLOCK(fd) \
	if (fcgi_accept_mutex != INVALID_HANDLE_VALUE) { \
		ReleaseMutex(fcgi_accept_mutex); \
	}

#else

# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <sys/un.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <signal.h>

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long) -1)
#endif

# ifndef HAVE_SOCKLEN_T
	typedef unsigned int socklen_t;
# endif

# ifdef USE_LOCKING
#  define FCGI_LOCK(fd)								\
	do {											\
		struct flock lock;							\
		lock.l_type = F_WRLCK;						\
		lock.l_start = 0;							\
		lock.l_whence = SEEK_SET;					\
		lock.l_len = 0;								\
		if (fcntl(fd, F_SETLKW, &lock) != -1) {		\
			break;									\
		} else if (errno != EINTR || in_shutdown) {	\
			return -1;								\
		}											\
	} while (1)

#  define FCGI_UNLOCK(fd)							\
	do {											\
		int orig_errno = errno;						\
		while (1) {									\
			struct flock lock;						\
			lock.l_type = F_UNLCK;					\
			lock.l_start = 0;						\
			lock.l_whence = SEEK_SET;				\
			lock.l_len = 0;							\
			if (fcntl(fd, F_SETLK, &lock) != -1) {	\
				break;								\
			} else if (errno != EINTR) {			\
				return -1;							\
			}										\
		}											\
		errno = orig_errno;							\
	} while (0)
# else
#  define FCGI_LOCK(fd)
#  define FCGI_UNLOCK(fd)
# endif

#endif

typedef union _sa_t {
	struct sockaddr     sa;
	struct sockaddr_un  sa_unix;
	struct sockaddr_in  sa_inet;
} sa_t;

typedef struct _fcgi_mgmt_rec {
	char*  name;
	size_t name_len;
	char   val;
} fcgi_mgmt_rec;

static const fcgi_mgmt_rec fcgi_mgmt_vars[] = {
	{"FCGI_MAX_CONNS",  sizeof("FCGI_MAX_CONNS")-1,  1},
	{"FCGI_MAX_REQS",   sizeof("FCGI_MAX_REQS")-1,   1},
	{"FCGI_MPXS_CONNS", sizeof("FCGI_MPXS_CONNS")-1, 0}
};


static int is_initialized = 0;
static int is_fastcgi = 0;
static int in_shutdown = 0;

#ifdef _WIN32

static DWORD WINAPI fcgi_shutdown_thread(LPVOID arg)
{
	HANDLE shutdown_event = (HANDLE) arg;
	WaitForSingleObject(shutdown_event, INFINITE);
	in_shutdown = 1;
	return 0;
}

#else

static void fcgi_signal_handler(int signo)
{
	if (signo == SIGUSR1 || signo == SIGTERM) {
		in_shutdown = 1;
	}
}

#endif

int fcgi_init(void)
{
	if (!is_initialized) {
#ifdef _WIN32
# if 0
		/* TODO: Support for TCP sockets */
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2,0), &wsaData)) {
			fprintf(stderr, "Error starting Windows Sockets.  Error: %d", WSAGetLastError());
			return 0;
		}
# endif
		is_initialized = 1;

		if ((GetStdHandle(STD_OUTPUT_HANDLE) == INVALID_HANDLE_VALUE) &&
		    (GetStdHandle(STD_ERROR_HANDLE)  == INVALID_HANDLE_VALUE) &&
		    (GetStdHandle(STD_INPUT_HANDLE)  != INVALID_HANDLE_VALUE)) {
			char *str;
			DWORD pipe_mode = PIPE_READMODE_BYTE | PIPE_WAIT;
			HANDLE pipe = GetStdHandle(STD_INPUT_HANDLE);

			SetNamedPipeHandleState(pipe, &pipe_mode, NULL, NULL);

			str = getenv("_FCGI_SHUTDOWN_EVENT_");
			if (str != NULL) {
				HANDLE shutdown_event = (HANDLE) atoi(str);
				if (!CreateThread(NULL, 0, fcgi_shutdown_thread,
				                  shutdown_event, 0, NULL)) {
					return -1;
				}
			}
			str = getenv("_FCGI_MUTEX_");
			if (str != NULL) {
				fcgi_accept_mutex = (HANDLE) atoi(str);
			}
			return is_fastcgi = 1;
		} else {
			return is_fastcgi = 0;
		}
#else
		sa_t sa;
		socklen_t len = sizeof(sa);

		is_initialized = 1;
		errno = 0;
		if (getpeername(0, (struct sockaddr *)&sa, &len) != 0 && errno == ENOTCONN) {
			struct sigaction new_sa, old_sa;

			sigemptyset(&new_sa.sa_mask);
			new_sa.sa_flags = 0;
			new_sa.sa_handler = fcgi_signal_handler;
			sigaction(SIGUSR1, &new_sa, NULL);
			sigaction(SIGTERM, &new_sa, NULL);
			sigaction(SIGPIPE, NULL, &old_sa);
			if (old_sa.sa_handler == SIG_DFL) {
				sigaction(SIGPIPE, &new_sa, NULL);
			}

			return is_fastcgi = 1;
		} else {
			return is_fastcgi = 0;
		}
#endif
	}
	return is_fastcgi;
}


int fcgi_is_fastcgi(void)
{
	if (!is_initialized) {
		return fcgi_init();
	} else {
		return is_fastcgi;
	}
}

int fcgi_listen(const char *path, int backlog)
{
#ifdef _WIN32
	/* TODO: Support for manual binding on TCP sockets (php -b <port>) */
	return -1;
#else
	char     *s;
	int       tcp = 0;
	char      host[MAXPATHLEN];
	short     port = 0;
	int       listen_socket;
	sa_t      sa;
	socklen_t sa_len;

	if ((s = strchr(path, ':'))) {
		port = atoi(s+1);
		if (port != 0 && (s-path) < MAXPATHLEN) {
			strncpy(host, path, s-path);
			host[s-path] = '\0';
			tcp = 1;
		}
	}

	/* Prepare socket address */
	if (tcp) {
		memset(&sa.sa_inet, 0, sizeof(sa.sa_inet));
		sa.sa_inet.sin_family = AF_INET;
		sa.sa_inet.sin_port = htons(port);
		sa_len = sizeof(sa.sa_inet);

		if (!*host || !strncmp(host, "*", sizeof("*")-1)) {
			sa.sa_inet.sin_addr.s_addr = htonl(INADDR_ANY);
		} else {
			sa.sa_inet.sin_addr.s_addr = inet_addr(host);
			if (sa.sa_inet.sin_addr.s_addr == INADDR_NONE) {
				struct hostent *hep;

				hep = gethostbyname(host);
				if (!hep || hep->h_addrtype != AF_INET || !hep->h_addr_list[0]) {
					fprintf(stderr, "Cannot resolve host name '%s'!\n", host);
					return -1;
				} else if (hep->h_addr_list[1]) {
					fprintf(stderr, "Host '%s' has multiple addresses. You must choose one explicitly!\n", host);
					return -1;
				}
				sa.sa_inet.sin_addr.s_addr = ((struct in_addr*)hep->h_addr_list[0])->s_addr;
			}
		}
	} else {
		int path_len = strlen(path);

		if (path_len >= sizeof(sa.sa_unix.sun_path)) {
			fprintf(stderr, "Listening socket's path name is too long.\n");
			return -1;
		}

		memset(&sa.sa_unix, 0, sizeof(sa.sa_unix));
		sa.sa_unix.sun_family = AF_UNIX;
		memcpy(sa.sa_unix.sun_path, path, path_len + 1);
		sa_len = (size_t)(((struct sockaddr_un *)0)->sun_path)	+ path_len;
#ifdef HAVE_SOCKADDR_UN_SUN_LEN
		sa.sa_unix.sun_len = sa_len;
#endif
		unlink(path);
	}

	/* Create, bind socket and start listen on it */
	if ((listen_socket = socket(sa.sa.sa_family, SOCK_STREAM, 0)) < 0 ||
	    bind(listen_socket, (struct sockaddr *) &sa, sa_len) < 0 ||
	    listen(listen_socket, backlog) < 0) {

		fprintf(stderr, "Cannot bind/listen socket - [%d] %s.\n",errno, strerror(errno));
		return -1;
	}

	if (!tcp) {
		chmod(path, 0777);
	}

	if (!is_initialized) {
		fcgi_init();
	}
	is_fastcgi = 1;
	return listen_socket;
#endif
}

void fcgi_init_request(fcgi_request *req, int listen_socket)
{
	memset(req, 0, sizeof(fcgi_request));
	req->listen_socket = listen_socket;
	req->fd = -1;
	req->id = -1;

	req->in_len = 0;
	req->in_pad = 0;

	req->out_hdr  = NULL;
	req->out_pos  = req->out_buf;
}

static inline ssize_t safe_write(fcgi_request *req, const void *buf, size_t count)
{
	int    ret;
	size_t n = 0;

	do {
		ret = write(req->fd, ((char*)buf)+n, count-n);
		if (ret > 0) {
			n += ret;
		} else if (ret <= 0 && errno != 0 && errno != EINTR) {
			return ret;
		}
	} while (n != count);
	return n;
}

static inline ssize_t safe_read(fcgi_request *req, const void *buf, size_t count)
{
	int    ret;
	size_t n = 0;

	do {
		ret = read(req->fd, ((char*)buf)+n, count-n);
		if (ret > 0) {
			n += ret;
		} else if (ret == 0 && errno == 0) {
			return n;
		} else if (ret <= 0 && errno != 0 && errno != EINTR) {
			return ret;
		}
	} while (n != count);
	return n;
}

static inline int fcgi_make_header(fcgi_header *hdr, fcgi_request_type type, int req_id, int len)
{
	int pad = ((len + 7) & ~7) - len;

	hdr->contentLengthB0 = (unsigned char)(len & 0xff);
	hdr->contentLengthB1 = (unsigned char)((len >> 8) & 0xff);
	hdr->paddingLength = (unsigned char)pad;
	hdr->requestIdB0 = (unsigned char)(req_id & 0xff);
	hdr->requestIdB1 = (unsigned char)((req_id >> 8) & 0xff);
	hdr->reserved = 0;
	hdr->type = type;
	hdr->version = FCGI_VERSION_1;
	return pad;
}

static void fcgi_get_params(fcgi_request *req, unsigned char *p, unsigned char *end)
{
	char buf[128];
	char *tmp = buf;
	size_t buf_size = sizeof(buf);
	int name_len, val_len;
	char *s;

	while (p < end) {
		name_len = *p++;
		if (name_len >= 128) {
			name_len = ((name_len & 0x7f) << 24);
			name_len |= (*p++ << 16);
			name_len |= (*p++ << 8);
			name_len |= *p++;
		}
		val_len = *p++;
		if (val_len >= 128) {
			val_len = ((val_len & 0x7f) << 24);
			val_len |= (*p++ << 16);
			val_len |= (*p++ << 8);
			val_len |= *p++;
		}
		if (name_len+1 >= buf_size) {
			buf_size = name_len + 64;
			tmp = (tmp == buf ? emalloc(buf_size): erealloc(tmp, buf_size));
		}
		memcpy(tmp, p, name_len);
		tmp[name_len] = 0;
		s = zend_strndup((char*)p + name_len, val_len);
		zend_hash_update(&req->env, tmp, name_len+1, &s, sizeof(char*), NULL);
		p += name_len + val_len;
	}
	if (tmp != buf && tmp != NULL) {
		efree(tmp);
	}
}

static void fcgi_free_var(char **s)
{
	free(*s);
}

static int fcgi_read_request(fcgi_request *req)
{
	fcgi_header hdr;
	int len, padding;
	unsigned char buf[FCGI_MAX_LENGTH+8];

	req->keep = 0;
	req->in_len = 0;
	req->out_hdr = NULL;
	req->out_pos = req->out_buf;
	zend_hash_init(&req->env, 0, NULL, (void (*)(void *)) fcgi_free_var, 1);

	if (safe_read(req, &hdr, sizeof(fcgi_header)) != sizeof(fcgi_header) ||
	    hdr.version < FCGI_VERSION_1) {
		return 0;
	}

	len = (hdr.contentLengthB1 << 8) | hdr.contentLengthB0;
	padding = hdr.paddingLength;

	while (hdr.type == FCGI_STDIN && len == 0) {
		if (safe_read(req, &hdr, sizeof(fcgi_header)) != sizeof(fcgi_header) ||
		    hdr.version < FCGI_VERSION_1) {
			return 0;
		}

		len = (hdr.contentLengthB1 << 8) | hdr.contentLengthB0;
		padding = hdr.paddingLength;
	}

	req->id = (hdr.requestIdB1 << 8) + hdr.requestIdB0;

	if (hdr.type == FCGI_BEGIN_REQUEST && len == sizeof(fcgi_begin_request)) {
		char *val;

		if (safe_read(req, buf, len+padding) != len+padding) {
			return 0;
		}

		req->keep = (((fcgi_begin_request*)buf)->flags & FCGI_KEEP_CONN);
		switch ((((fcgi_begin_request*)buf)->roleB1 << 8) + ((fcgi_begin_request*)buf)->roleB0) {
			case FCGI_RESPONDER:
				val = strdup("RESPONDER");
				zend_hash_update(&req->env, "FCGI_ROLE", sizeof("FCGI_ROLE"), &val, sizeof(char*), NULL);
				break;
			case FCGI_AUTHORIZER:
				val = strdup("AUTHORIZER");
				zend_hash_update(&req->env, "FCGI_ROLE", sizeof("FCGI_ROLE"), &val, sizeof(char*), NULL);
				break;
			case FCGI_FILTER:
				val = strdup("FILTER");
				zend_hash_update(&req->env, "FCGI_ROLE", sizeof("FCGI_ROLE"), &val, sizeof(char*), NULL);
				break;
			default:
				return 0;
		}

		if (safe_read(req, &hdr, sizeof(fcgi_header)) != sizeof(fcgi_header) ||
		    hdr.version < FCGI_VERSION_1) {
			return 0;
		}

		len = (hdr.contentLengthB1 << 8) | hdr.contentLengthB0;
		padding = hdr.paddingLength;

		while (hdr.type == FCGI_PARAMS && len > 0) {
			if (safe_read(req, buf, len+padding) != len+padding) {
				req->keep = 0;
				return 0;
			}
			fcgi_get_params(req, buf, buf+len);

			if (safe_read(req, &hdr, sizeof(fcgi_header)) != sizeof(fcgi_header) ||
			    hdr.version < FCGI_VERSION_1) {
				req->keep = 0;
				return 0;
			}
			len = (hdr.contentLengthB1 << 8) | hdr.contentLengthB0;
			padding = hdr.paddingLength;
		}
	} else if (hdr.type == FCGI_GET_VALUES) {
		int j;
		unsigned char *p = buf + sizeof(fcgi_header);

		if (safe_read(req, buf, len+padding) != len+padding) {
			return 0;
		}
		fcgi_get_params(req, buf, buf+len);

		for (j = 0; j < sizeof(fcgi_mgmt_vars)/sizeof(fcgi_mgmt_vars[0]); j++) {
			if (zend_hash_exists(&req->env, fcgi_mgmt_vars[j].name, fcgi_mgmt_vars[j].name_len+1) == 0) {
                sprintf((char*)p, "%c%c%s%c", fcgi_mgmt_vars[j].name_len, 1, fcgi_mgmt_vars[j].name, fcgi_mgmt_vars[j].val);
                p += fcgi_mgmt_vars[j].name_len + 3;
			}
		}
		len = p - buf - sizeof(fcgi_header);
		len += fcgi_make_header((fcgi_header*)buf, FCGI_GET_VALUES_RESULT, 0, len);
		if (safe_write(req, buf, sizeof(fcgi_header)+len) != (int)sizeof(fcgi_header)+len) {
			return 0;
		}
		return 0;
	} else {
		return 0;
	}

	return 1;
}

int fcgi_read(fcgi_request *req, char *str, int len)
{
	int ret, n, rest;
	fcgi_header hdr;
	unsigned char buf[8];

	n = 0;
	rest = len;
	while (rest > 0) {
		if (req->in_len == 0) {
			if (safe_read(req, &hdr, sizeof(fcgi_header)) != sizeof(fcgi_header) ||
			    hdr.version < FCGI_VERSION_1 ||
			    hdr.type != FCGI_STDIN) {
				req->keep = 0;
				return 0;
			}
			req->in_len = (hdr.contentLengthB1 << 8) | hdr.contentLengthB0;
			req->in_pad = hdr.paddingLength;
			if (req->in_len == 0) {
				return n;
			}
		}

		if (req->in_len >= rest) {
			ret = safe_read(req, str, rest);
		} else {
			ret = safe_read(req, str, req->in_len);
		}
		if (ret < 0) {
			req->keep = 0;
			return ret;
		} else if (ret > 0) {
			req->in_len -= ret;
			rest -= ret;
			n += ret;
			str += ret;
			if (req->in_len == 0) {
				if (req->in_pad) {
					if (safe_read(req, buf, req->in_pad) != req->in_pad) {
						req->keep = 0;
						return ret;
					}
				}
			} else {
				return n;
			}
		} else {
			return n;
		}
	}
	return n;
}

static inline void fcgi_close(fcgi_request *req, int force, int destroy)
{
	if (destroy) {
		zend_hash_destroy(&req->env);
	}
	if ((force || !req->keep) && req->fd >= 0) {
#ifdef _WIN32
		HANDLE pipe = (HANDLE)_get_osfhandle(req->fd);

		if (!force) {
			FlushFileBuffers(pipe);
		}
		DisconnectNamedPipe(pipe);
		if (is_impersonate) {
			RevertToSelf();
		}
#else
		char buf[8];

		shutdown(req->fd, 1);
		while (recv(req->fd, buf, sizeof(buf), 0) > 0) {}
		close(req->fd);
#endif
		req->fd = -1;
	}
}

int fcgi_accept_request(fcgi_request *req)
{
#ifdef _WIN32
	HANDLE pipe;
	OVERLAPPED ov;
#endif
	fcgi_finish_request(req);

	while (1) {
		if (req->fd < 0) {
			while (1) {
				if (in_shutdown) {
					return -1;
				}
#ifdef _WIN32
				pipe = (HANDLE)_get_osfhandle(req->listen_socket);

				FCGI_LOCK(req->listen_socket);
				ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
				if (!ConnectNamedPipe(pipe, &ov)) {
					errno = GetLastError();
					if (errno == ERROR_IO_PENDING) {
						while (WaitForSingleObject(ov.hEvent, 1000) == WAIT_TIMEOUT) {
							if (in_shutdown) {
								CloseHandle(ov.hEvent);
								FCGI_UNLOCK(req->listen_socket);
								return -1;
							}
						}
					} else if (errno != ERROR_PIPE_CONNECTED) {
					}
				}
				CloseHandle(ov.hEvent);
				if (is_impersonate && !ImpersonateNamedPipeClient(pipe)) {
					DisconnectNamedPipe(pipe);
					req->fd = -1;
				} else {
					req->fd = req->listen_socket;
				}
				FCGI_UNLOCK(req->listen_socket);
#else
				{
					sa_t sa;
					socklen_t len = sizeof(sa);

					FCGI_LOCK(req->listen_socket);
					req->fd = accept(req->listen_socket, (struct sockaddr *)&sa, &len);
					FCGI_UNLOCK(req->listen_socket);
				}
#endif

				if (req->fd < 0 && (in_shutdown || errno != EINTR)) {
					return -1;
				}

#ifdef _WIN32
				break;
#else
				if (req->fd >= 0) {
					struct timeval tv = {1,0};
					fd_set set;

					FD_ZERO(&set);
					FD_SET(req->fd, &set);
try_again:
					errno = 0;
					if (select(req->fd + 1, &set, NULL, NULL, &tv) >= 0 && FD_ISSET(req->fd, &set)) {
						break;
					}
					if (errno == EINTR) goto try_again;
					fcgi_close(req, 1, 0);
				}
#endif
			}
		} else if (in_shutdown) {
			return -1;
		}
		if (fcgi_read_request(req)) {
			return req->fd;
		} else {
			fcgi_close(req, 1, 1);
		}
	}
}

static inline fcgi_header* open_packet(fcgi_request *req, fcgi_request_type type)
{
	req->out_hdr = (fcgi_header*) req->out_pos;
	req->out_hdr->type = type;
	req->out_pos += sizeof(fcgi_header);
	return req->out_hdr;
}

static inline void close_packet(fcgi_request *req)
{
	if (req->out_hdr) {
		int len = req->out_pos - ((unsigned char*)req->out_hdr + sizeof(fcgi_header));

		req->out_pos += fcgi_make_header(req->out_hdr, req->out_hdr->type, req->id, len);
		req->out_hdr = NULL;
	}
}

int fcgi_flush(fcgi_request *req, int close)
{
	int len;

	close_packet(req);

	len = req->out_pos - req->out_buf;

	if (close) {
		fcgi_end_request_rec *rec = (fcgi_end_request_rec*)(req->out_pos);

		fcgi_make_header(&rec->hdr, FCGI_END_REQUEST, req->id, sizeof(fcgi_end_request));
		rec->body.appStatusB3 = 0;
		rec->body.appStatusB2 = 0;
		rec->body.appStatusB1 = 0;
		rec->body.appStatusB0 = 0;
		rec->body.protocolStatus = FCGI_REQUEST_COMPLETE;
		len += sizeof(fcgi_end_request_rec);
	}

	if (safe_write(req, req->out_buf, len) != len) {
		req->keep = 0;
		return 0;
	}

	req->out_pos = req->out_buf;
	return 1;
}

int fcgi_write(fcgi_request *req, fcgi_request_type type, const char *str, int len)
{
	int limit, rest;

	if (len <= 0) {
		return 0;
	}

	if (req->out_hdr && req->out_hdr->type != type) {
		close_packet(req);
	}
	rest = len;
#if 0
	/* Unoptinmzed, but clear version */
	while (rest > 0) {
		limit = sizeof(req->out_buf) - (req->out_pos - req->out_buf);

		if (!req->out_hdr) {
			if (limit < sizeof(fcgi_header)) {
				fcgi_flush(req, 0);
			}
			open_packet(req, type);
		}
		limit = sizeof(req->out_buf) - (req->out_pos - req->out_buf);
		if (rest < limit) {
			memcpy(req->out_pos, str, rest);
			req->out_pos += rest;
			return len;
		} else {
			memcpy(req->out_pos, str, limit);
			req->out_pos += limit;
			rest -= limit;
			str += limit;
			fcgi_flush(req, 0);
		}
	}
#else
	/* Optinmzed version */
	if (!req->out_hdr) {
		rest += sizeof(fcgi_header);
	}
	limit = sizeof(req->out_buf) - (req->out_pos - req->out_buf);

	if (rest < limit) {
		if (!req->out_hdr) {
			open_packet(req, type);
		}
		memcpy(req->out_pos, str, len);
		req->out_pos += len;
	} else if (rest - limit < sizeof(req->out_buf) - sizeof(fcgi_header)) {
		if (!req->out_hdr) {
			open_packet(req, type);
		}
		memcpy(req->out_pos, str, limit);
		req->out_pos += limit;
		fcgi_flush(req, 0);
		open_packet(req, type);
		memcpy(req->out_pos, str + limit, len - limit);
		req->out_pos += len - limit;
	} else {
		int pos = 0;
		int pad;

		close_packet(req);
		while ((len - pos) > 0xffff) {
			open_packet(req, type);
			fcgi_make_header(req->out_hdr, type, req->id, 0xfff8);
			req->out_hdr = NULL;
			fcgi_flush(req, 0);
			if (safe_write(req, str + pos, 0xfff8) != 0xfff8) {
				req->keep = 0;
				return -1;
			}
			pos += 0xfff8;
		}		
		
		pad = (((len - pos) + 7) & ~7) - (len - pos);
		rest = pad ? 8 - pad : 0;

		open_packet(req, type);
		fcgi_make_header(req->out_hdr, type, req->id, (len - pos) - rest);
		req->out_hdr = NULL;
		fcgi_flush(req, 0);
		if (safe_write(req, str + pos, (len - pos) - rest) != (len - pos) - rest) {
			req->keep = 0;
			return -1;
		}
		if (pad) {
			open_packet(req, type);
			memcpy(req->out_pos, str + len - rest,  rest);
			req->out_pos += rest;
		}
	}
#endif
	return len;
}

int fcgi_finish_request(fcgi_request *req)
{
	if (req->fd >= 0) {
		fcgi_flush(req, 1);
		fcgi_close(req, 0, 1);
	}
	return 1;
}

char* fcgi_getenv(fcgi_request *req, const char* var, int var_len)
{
	char **val;

	if (!req) return NULL;

	if (zend_hash_find(&req->env, (char*)var, var_len+1, (void**)&val) == SUCCESS) {
		return *val;
	}
	return NULL;
}

char* fcgi_putenv(fcgi_request *req, char* var, int var_len, char* val)
{
	if (var && req) {
		char **ret;

		if (val == NULL) {
			val = "";
		}
		val = strdup(val);
		if (zend_hash_update(&req->env, var, var_len+1, &val, sizeof(char*), (void**)&ret) == SUCCESS) {
			return *ret;
		}
	}
	return NULL;
}

#ifdef _WIN32
void fcgi_impersonate(void)
{
	char *os_name;

	os_name = getenv("OS");
	if (os_name && stricmp(os_name, "Windows_NT") == 0) {
		is_impersonate = 1;
	}
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
