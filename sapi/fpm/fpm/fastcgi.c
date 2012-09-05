/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
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

/* $Id: fastcgi.c 287777 2009-08-26 19:17:32Z pajoye $ */

#include "php.h"
#include "fastcgi.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>

#include <php_config.h>
#include "fpm.h"
#include "fpm_request.h"
#include "zlog.h"

#ifdef _WIN32

#include <windows.h>

	typedef unsigned int in_addr_t;

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

# define closesocket(s) close(s)

# if defined(HAVE_SYS_POLL_H) && defined(HAVE_POLL)
#  include <sys/poll.h>
# endif
# if defined(HAVE_SYS_SELECT_H)
#  include <sys/select.h>
# endif

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

static HashTable fcgi_mgmt_vars;

static int is_initialized = 0;
static int in_shutdown = 0;
static in_addr_t *allowed_clients = NULL;

static sa_t client_sa;

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

static void fcgi_setup_signals(void)
{
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
}
#endif

int fcgi_init(void)
{
	if (!is_initialized) {
		zend_hash_init(&fcgi_mgmt_vars, 0, NULL, fcgi_free_mgmt_var_cb, 1);
		fcgi_set_mgmt_var("FCGI_MPXS_CONNS", sizeof("FCGI_MPXS_CONNS") - 1, "0", sizeof("0")-1);

		is_initialized = 1;
#ifdef _WIN32
# if 0
		/* TODO: Support for TCP sockets */
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2,0), &wsaData)) {
			fprintf(stderr, "Error starting Windows Sockets.  Error: %d", WSAGetLastError());
			return 0;
		}
# endif
		{
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
			return 1;
		}
#else
		fcgi_setup_signals();
		return 1;
#endif
	}
	return 1;
}

void fcgi_set_in_shutdown(int new_value)
{
	in_shutdown = new_value;
}

void fcgi_shutdown(void)
{
	if (is_initialized) {
		zend_hash_destroy(&fcgi_mgmt_vars);
	}
	if (allowed_clients) {
		free(allowed_clients);
	}
}

void fcgi_set_allowed_clients(char *ip)
{
	char *cur, *end;
	int n;

	if (ip) {
		ip = strdup(ip);
		cur = ip;
		n = 0;
		while (*cur) {
			if (*cur == ',') n++;
			cur++;
		}
		if (allowed_clients) free(allowed_clients);
		allowed_clients = malloc(sizeof(in_addr_t) * (n+2));
		n = 0;
		cur = ip;
		while (cur) {
			end = strchr(cur, ',');
			if (end) {
				*end = 0;
				end++;
			}
			allowed_clients[n] = inet_addr(cur);
			if (allowed_clients[n] == INADDR_NONE) {
				zlog(ZLOG_ERROR, "Wrong IP address '%s' in listen.allowed_clients", cur);
			}
			n++;
			cur = end;
		}
		allowed_clients[n] = INADDR_NONE;
		free(ip);
	}
}

void fcgi_init_request(fcgi_request *req, int listen_socket)
{
	memset(req, 0, sizeof(fcgi_request));
	req->listen_socket = listen_socket;
	req->fd = -1;
	req->id = -1;

	req->in_len = 0;
	req->in_pad = 0;

	req->out_hdr = NULL;
	req->out_pos = req->out_buf;

#ifdef _WIN32
	req->tcp = !GetNamedPipeInfo((HANDLE)_get_osfhandle(req->listen_socket), NULL, NULL, NULL, NULL);
#endif
}

static inline ssize_t safe_write(fcgi_request *req, const void *buf, size_t count)
{
	int    ret;
	size_t n = 0;

	do {
		errno = 0;
#ifdef _WIN32
		if (!req->tcp) {
			ret = write(req->fd, ((char*)buf)+n, count-n);
		} else {
			ret = send(req->fd, ((char*)buf)+n, count-n, 0);
			if (ret <= 0) {
				errno = WSAGetLastError();
			}
		}
#else
		ret = write(req->fd, ((char*)buf)+n, count-n);
#endif
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
		errno = 0;
#ifdef _WIN32
		if (!req->tcp) {
			ret = read(req->fd, ((char*)buf)+n, count-n);
		} else {
			ret = recv(req->fd, ((char*)buf)+n, count-n, 0);
			if (ret <= 0) {
				errno = WSAGetLastError();
			}
		}
#else
		ret = read(req->fd, ((char*)buf)+n, count-n);
#endif
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
	if (pad) {
		memset(((unsigned char*)hdr) + sizeof(fcgi_header) + len, 0, pad);
	}
	return pad;
}

static inline size_t fcgi_get_params_len( int *result, unsigned char *p, unsigned char *end)
{
	size_t ret = 0;

	if (p < end) {
		*result = p[0];
		if (*result < 128) {
			ret = 1;
		}
		else if (p + 3 < end) {
			*result = ((*result & 0x7f) << 24);
			*result |= (p[1] << 16);
			*result |= (p[2] << 8);
			*result |= p[3];
			ret = 4;
		}
	}
	if (*result < 0) {
		ret = 0;
	}
	return ret;
}

static inline int fcgi_param_get_eff_len( unsigned char *p, unsigned char *end, uint *eff_len)
{
	int ret = 1;
	int zero_found = 0;
	*eff_len = 0;
	for (; p != end; ++p) {
		if (*p == '\0') {
			zero_found = 1;
		}
		else {
			if (zero_found) {
				ret = 0;
				break;
			}
			if (*eff_len < ((uint)-1)) {
				++*eff_len;
			}
			else {
				ret = 0;
				break;
			}
		}
	}
	return ret;
}

static int fcgi_get_params(fcgi_request *req, unsigned char *p, unsigned char *end)
{
	char buf[128];
	char *tmp = buf;
	size_t buf_size = sizeof(buf);
	int name_len, val_len;
	uint eff_name_len;
	char *s;
	int ret = 1;
	size_t bytes_consumed;

	while (p < end) {
		bytes_consumed = fcgi_get_params_len(&name_len, p, end);
		if (!bytes_consumed) {
			/* Malformated request */
			ret = 0;
			break;
		}
		p += bytes_consumed;
		bytes_consumed = fcgi_get_params_len(&val_len, p, end);
		if (!bytes_consumed) {
			/* Malformated request */
			ret = 0;
			break;
		}
		p += bytes_consumed;
		if (name_len > (INT_MAX - val_len) || /* would the addition overflow? */
		    name_len + val_len > end - p) {   /* would we exceed the buffer? */
			/* Malformated request */
			ret = 0;
			break;
		}

		/*
		 * get the effective length of the name in case it's not a valid string
		 * don't do this on the value because it can be binary data
		 */
		if (!fcgi_param_get_eff_len(p, p+name_len, &eff_name_len)){
			/* Malicious request */
			ret = 0;
			break;
		}
		if (eff_name_len >= buf_size-1) {
			if (eff_name_len > ((uint)-1)-64) { 
				ret = 0;
				break;
			}
			buf_size = eff_name_len + 64;
			tmp = (tmp == buf ? emalloc(buf_size): erealloc(tmp, buf_size));
			if (tmp == NULL) {
				ret = 0;
				break;
			}
		}
		memcpy(tmp, p, eff_name_len);
		tmp[eff_name_len] = 0;
		s = estrndup((char*)p + name_len, val_len);
		if (s == NULL) {
			ret = 0;
			break;
		}
		zend_hash_update(req->env, tmp, eff_name_len+1, &s, sizeof(char*), NULL);
		p += name_len + val_len;
	}
	if (tmp != buf && tmp != NULL) {
		efree(tmp);
	}
	return ret;
}

static void fcgi_free_var(char **s)
{
	efree(*s);
}

static int fcgi_read_request(fcgi_request *req)
{
	fcgi_header hdr;
	int len, padding;
	unsigned char buf[FCGI_MAX_LENGTH+8];

	req->keep = 0;
	req->closed = 0;
	req->in_len = 0;
	req->out_hdr = NULL;
	req->out_pos = req->out_buf;
	ALLOC_HASHTABLE(req->env);
	zend_hash_init(req->env, 0, NULL, (void (*)(void *)) fcgi_free_var, 0);

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

	if (len + padding > FCGI_MAX_LENGTH) {
		return 0;
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
				val = estrdup("RESPONDER");
				zend_hash_update(req->env, "FCGI_ROLE", sizeof("FCGI_ROLE"), &val, sizeof(char*), NULL);
				break;
			case FCGI_AUTHORIZER:
				val = estrdup("AUTHORIZER");
				zend_hash_update(req->env, "FCGI_ROLE", sizeof("FCGI_ROLE"), &val, sizeof(char*), NULL);
				break;
			case FCGI_FILTER:
				val = estrdup("FILTER");
				zend_hash_update(req->env, "FCGI_ROLE", sizeof("FCGI_ROLE"), &val, sizeof(char*), NULL);
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
			if (len + padding > FCGI_MAX_LENGTH) {
				return 0;
			}

			if (safe_read(req, buf, len+padding) != len+padding) {
				req->keep = 0;
				return 0;
			}

			if (!fcgi_get_params(req, buf, buf+len)) {
				req->keep = 0;
				return 0;
			}

			if (safe_read(req, &hdr, sizeof(fcgi_header)) != sizeof(fcgi_header) ||
			    hdr.version < FCGI_VERSION_1) {
				req->keep = 0;
				return 0;
			}
			len = (hdr.contentLengthB1 << 8) | hdr.contentLengthB0;
			padding = hdr.paddingLength;
		}
	} else if (hdr.type == FCGI_GET_VALUES) {
		unsigned char *p = buf + sizeof(fcgi_header);
		HashPosition pos;
		char * str_index;
		uint str_length;
		ulong num_index;
		int key_type;
		zval ** value;

		if (safe_read(req, buf, len+padding) != len+padding) {
			req->keep = 0;
			return 0;
		}

		if (!fcgi_get_params(req, buf, buf+len)) {
			req->keep = 0;
			return 0;
		}

		zend_hash_internal_pointer_reset_ex(req->env, &pos);
		while ((key_type = zend_hash_get_current_key_ex(req->env, &str_index, &str_length, &num_index, 0, &pos)) != HASH_KEY_NON_EXISTANT) {
			int zlen;
			zend_hash_move_forward_ex(req->env, &pos);
			if (key_type != HASH_KEY_IS_STRING) {
				continue;
			}
			if (zend_hash_find(&fcgi_mgmt_vars, str_index, str_length, (void**) &value) != SUCCESS) {
				continue;
			}
			--str_length;
			zlen = Z_STRLEN_PP(value);
			if ((p + 4 + 4 + str_length + zlen) >= (buf + sizeof(buf))) {
				break;
			}
			if (str_length < 0x80) {
				*p++ = str_length;
			} else {
				*p++ = ((str_length >> 24) & 0xff) | 0x80;
				*p++ = (str_length >> 16) & 0xff;
				*p++ = (str_length >> 8) & 0xff;
				*p++ = str_length & 0xff;
			}
			if (zlen < 0x80) {
				*p++ = zlen;
			} else {
				*p++ = ((zlen >> 24) & 0xff) | 0x80;
				*p++ = (zlen >> 16) & 0xff;
				*p++ = (zlen >> 8) & 0xff;
				*p++ = zlen & 0xff;
			}
			memcpy(p, str_index, str_length);
			p += str_length;
			memcpy(p, Z_STRVAL_PP(value), zlen);
			p += zlen;
		}
		len = p - buf - sizeof(fcgi_header);
		len += fcgi_make_header((fcgi_header*)buf, FCGI_GET_VALUES_RESULT, 0, len);
		if (safe_write(req, buf, sizeof(fcgi_header)+len) != (int)sizeof(fcgi_header)+len) {
			req->keep = 0;
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
	unsigned char buf[255];

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

void fcgi_close(fcgi_request *req, int force, int destroy)
{
	if (destroy && req->env) {
		zend_hash_destroy(req->env);
		FREE_HASHTABLE(req->env);
		req->env = NULL;
	}

#ifdef _WIN32
	if (is_impersonate && !req->tcp) {
		RevertToSelf();
	}
#endif

	if ((force || !req->keep) && req->fd >= 0) {
#ifdef _WIN32
		if (!req->tcp) {
			HANDLE pipe = (HANDLE)_get_osfhandle(req->fd);

			if (!force) {
				FlushFileBuffers(pipe);
			}
			DisconnectNamedPipe(pipe);
		} else {
			if (!force) {
				char buf[8];

				shutdown(req->fd, 1);
				while (recv(req->fd, buf, sizeof(buf), 0) > 0) {}
			}
			closesocket(req->fd);
		}
#else
		if (!force) {
			char buf[8];

			shutdown(req->fd, 1);
			while (recv(req->fd, buf, sizeof(buf), 0) > 0) {}
		}
		close(req->fd);
#endif
		req->fd = -1;
		fpm_request_finished();
	}
}

int fcgi_accept_request(fcgi_request *req)
{
#ifdef _WIN32
	HANDLE pipe;
	OVERLAPPED ov;
#endif

	while (1) {
		if (req->fd < 0) {
			while (1) {
				if (in_shutdown) {
					return -1;
				}
#ifdef _WIN32
				if (!req->tcp) {
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
					req->fd = req->listen_socket;
					FCGI_UNLOCK(req->listen_socket);
				} else {
					SOCKET listen_socket = (SOCKET)_get_osfhandle(req->listen_socket);
#else
				{
					int listen_socket = req->listen_socket;
#endif
					sa_t sa;
					socklen_t len = sizeof(sa);

					fpm_request_accepting();

					FCGI_LOCK(req->listen_socket);
					req->fd = accept(listen_socket, (struct sockaddr *)&sa, &len);
					FCGI_UNLOCK(req->listen_socket);

					client_sa = sa;
					if (sa.sa.sa_family == AF_INET && req->fd >= 0 && allowed_clients) {
						int n = 0;
						int allowed = 0;

						while (allowed_clients[n] != INADDR_NONE) {
							if (allowed_clients[n] == sa.sa_inet.sin_addr.s_addr) {
								allowed = 1;
								break;
							}
							n++;
						}
						if (!allowed) {
							zlog(ZLOG_ERROR, "Connection disallowed: IP address '%s' has been dropped.", inet_ntoa(sa.sa_inet.sin_addr));
							closesocket(req->fd);
							req->fd = -1;
							continue;
						}
					}
				}

#ifdef _WIN32
				if (req->fd < 0 && (in_shutdown || errno != EINTR)) {
#else
				if (req->fd < 0 && (in_shutdown || (errno != EINTR && errno != ECONNABORTED))) {
#endif
					return -1;
				}

#ifdef _WIN32
				break;
#else
				if (req->fd >= 0) {
#if defined(HAVE_SYS_POLL_H) && defined(HAVE_POLL)
					struct pollfd fds;
					int ret;

					fpm_request_reading_headers();

					fds.fd = req->fd;
					fds.events = POLLIN;
					fds.revents = 0;
					do {
						errno = 0;
						ret = poll(&fds, 1, 5000);
					} while (ret < 0 && errno == EINTR);
					if (ret > 0 && (fds.revents & POLLIN)) {
						break;
					}
					fcgi_close(req, 1, 0);
#else
					fpm_request_reading_headers();

					if (req->fd < FD_SETSIZE) {
						struct timeval tv = {5,0};
						fd_set set;
						int ret;

						FD_ZERO(&set);
						FD_SET(req->fd, &set);
						do {
							errno = 0;
							ret = select(req->fd + 1, &set, NULL, NULL, &tv) >= 0;
						} while (ret < 0 && errno == EINTR);
						if (ret > 0 && FD_ISSET(req->fd, &set)) {
							break;
						}
						fcgi_close(req, 1, 0);
					} else {
						zlog(ZLOG_ERROR, "Too many open file descriptors. FD_SETSIZE limit exceeded.");
						fcgi_close(req, 1, 0);
					}
#endif
				}
#endif
			}
		} else if (in_shutdown) {
			return -1;
		}
		if (fcgi_read_request(req)) {
#ifdef _WIN32
			if (is_impersonate && !req->tcp) {
				pipe = (HANDLE)_get_osfhandle(req->fd);
				if (!ImpersonateNamedPipeClient(pipe)) {
					fcgi_close(req, 1, 1);
					continue;
				}
			}
#endif
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

		req->out_pos += fcgi_make_header(req->out_hdr, (fcgi_request_type)req->out_hdr->type, req->id, len);
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

ssize_t fcgi_write(fcgi_request *req, fcgi_request_type type, const char *str, int len)
{
	int limit, rest;

	if (len <= 0) {
		return 0;
	}

	if (req->out_hdr && req->out_hdr->type != type) {
		close_packet(req);
	}

	/* Optimized version */
	limit = sizeof(req->out_buf) - (req->out_pos - req->out_buf);
	if (!req->out_hdr) {
		limit -= sizeof(fcgi_header);
		if (limit < 0) limit = 0;
	}

	if (len < limit) {
		if (!req->out_hdr) {
			open_packet(req, type);
		}
		memcpy(req->out_pos, str, len);
		req->out_pos += len;
	} else if (len - limit < sizeof(req->out_buf) - sizeof(fcgi_header)) {
		if (!req->out_hdr) {
			open_packet(req, type);
		}
		if (limit > 0) {
			memcpy(req->out_pos, str, limit);
			req->out_pos += limit;
		}
		if (!fcgi_flush(req, 0)) {
			return -1;
		}
		if (len > limit) {
			open_packet(req, type);
			memcpy(req->out_pos, str + limit, len - limit);
			req->out_pos += len - limit;
		}
	} else {
		int pos = 0;
		int pad;

		close_packet(req);
		while ((len - pos) > 0xffff) {
			open_packet(req, type);
			fcgi_make_header(req->out_hdr, type, req->id, 0xfff8);
			req->out_hdr = NULL;
			if (!fcgi_flush(req, 0)) {
				return -1;
			}
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
		if (!fcgi_flush(req, 0)) {
			return -1;
		}
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

	return len;
}

int fcgi_finish_request(fcgi_request *req, int force_close)
{
	int ret = 1;

	if (req->fd >= 0) {
		if (!req->closed) {
			ret = fcgi_flush(req, 1);
			req->closed = 1;
		}
		fcgi_close(req, force_close, 1);
	}
	return ret;
}

char* fcgi_getenv(fcgi_request *req, const char* var, int var_len)
{
	char **val;

	if (!req) return NULL;

	if (zend_hash_find(req->env, (char*)var, var_len+1, (void**)&val) == SUCCESS) {
		return *val;
	}
	return NULL;
}

char* fcgi_putenv(fcgi_request *req, char* var, int var_len, char* val)
{
	if (var && req) {
		if (val == NULL) {
			zend_hash_del(req->env, var, var_len+1);
		} else {
			char **ret;

			val = estrdup(val);
			if (zend_hash_update(req->env, var, var_len+1, &val, sizeof(char*), (void**)&ret) == SUCCESS) {
				return *ret;
			}
		}
	}
	return NULL;
}

void fcgi_set_mgmt_var(const char * name, size_t name_len, const char * value, size_t value_len)
{
	zval * zvalue;
	zvalue = pemalloc(sizeof(*zvalue), 1);
	Z_TYPE_P(zvalue) = IS_STRING;
	Z_STRVAL_P(zvalue) = pestrndup(value, value_len, 1);
	Z_STRLEN_P(zvalue) = value_len;
	zend_hash_add(&fcgi_mgmt_vars, name, name_len + 1, &zvalue, sizeof(zvalue), NULL);
}

void fcgi_free_mgmt_var_cb(void * ptr)
{
	zval ** var = (zval **)ptr;
	pefree(Z_STRVAL_PP(var), 1);
	pefree(*var, 1);
}

char *fcgi_get_last_client_ip() /* {{{ */
{
	if (client_sa.sa.sa_family == AF_UNIX) {
		return NULL;
	}
	return inet_ntoa(client_sa.sa_inet.sin_addr);
}
/* }}} */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
