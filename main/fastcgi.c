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
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_network.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#ifndef MAXFQDNLEN
#define MAXFQDNLEN 255
#endif

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
# include <netinet/tcp.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <signal.h>

# if defined(HAVE_POLL_H) && defined(HAVE_POLL)
#  include <poll.h>
# elif defined(HAVE_SYS_POLL_H) && defined(HAVE_POLL)
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

#include "fastcgi.h"

typedef struct _fcgi_header {
	unsigned char version;
	unsigned char type;
	unsigned char requestIdB1;
	unsigned char requestIdB0;
	unsigned char contentLengthB1;
	unsigned char contentLengthB0;
	unsigned char paddingLength;
	unsigned char reserved;
} fcgi_header;

typedef struct _fcgi_begin_request {
	unsigned char roleB1;
	unsigned char roleB0;
	unsigned char flags;
	unsigned char reserved[5];
} fcgi_begin_request;

typedef struct _fcgi_begin_request_rec {
	fcgi_header hdr;
	fcgi_begin_request body;
} fcgi_begin_request_rec;

typedef struct _fcgi_end_request {
    unsigned char appStatusB3;
    unsigned char appStatusB2;
    unsigned char appStatusB1;
    unsigned char appStatusB0;
    unsigned char protocolStatus;
    unsigned char reserved[3];
} fcgi_end_request;

typedef struct _fcgi_end_request_rec {
	fcgi_header hdr;
	fcgi_end_request body;
} fcgi_end_request_rec;

typedef struct _fcgi_hash_bucket {
	unsigned int              hash_value;
	unsigned int              var_len;
	char                     *var;
	unsigned int              val_len;
	char                     *val;
	struct _fcgi_hash_bucket *next;
	struct _fcgi_hash_bucket *list_next;
} fcgi_hash_bucket;

typedef struct _fcgi_hash_buckets {
	unsigned int	           idx;
	struct _fcgi_hash_buckets *next;
	struct _fcgi_hash_bucket   data[FCGI_HASH_TABLE_SIZE];
} fcgi_hash_buckets;

typedef struct _fcgi_data_seg {
	char                  *pos;
	char                  *end;
	struct _fcgi_data_seg *next;
	char                   data[1];
} fcgi_data_seg;

typedef struct _fcgi_hash {
	fcgi_hash_bucket  *hash_table[FCGI_HASH_TABLE_SIZE];
	fcgi_hash_bucket  *list;
	fcgi_hash_buckets *buckets;
	fcgi_data_seg     *data;
} fcgi_hash;

typedef struct _fcgi_req_hook 	fcgi_req_hook;

struct _fcgi_req_hook {
	void(*on_accept)();
	void(*on_read)();
	void(*on_close)();
};

struct _fcgi_request {
	int            listen_socket;
	int            tcp;
	int            fd;
	int            id;
	int            keep;
#ifdef TCP_NODELAY
	int            nodelay;
#endif
	int            ended;
	int            in_len;
	int            in_pad;

	fcgi_header   *out_hdr;

	unsigned char *out_pos;
	unsigned char  out_buf[1024*8];
	unsigned char  reserved[sizeof(fcgi_end_request_rec)];

	fcgi_req_hook  hook;

	int            has_env;
	fcgi_hash      env;
};

/* maybe it's better to use weak name instead */
#ifndef HAVE_ATTRIBUTE_WEAK
static fcgi_logger fcgi_log;
#endif

typedef union _sa_t {
	struct sockaddr     sa;
	struct sockaddr_un  sa_unix;
	struct sockaddr_in  sa_inet;
	struct sockaddr_in6 sa_inet6;
} sa_t;

static HashTable fcgi_mgmt_vars;

static int is_initialized = 0;
static int is_fastcgi = 0;
static int in_shutdown = 0;
static sa_t *allowed_clients = NULL;
static sa_t client_sa;

/* hash table */
static void fcgi_hash_init(fcgi_hash *h)
{
	memset(h->hash_table, 0, sizeof(h->hash_table));
	h->list = NULL;
	h->buckets = (fcgi_hash_buckets*)malloc(sizeof(fcgi_hash_buckets));
	h->buckets->idx = 0;
	h->buckets->next = NULL;
	h->data = (fcgi_data_seg*)malloc(sizeof(fcgi_data_seg) - 1 + FCGI_HASH_SEG_SIZE);
	h->data->pos = h->data->data;
	h->data->end = h->data->pos + FCGI_HASH_SEG_SIZE;
	h->data->next = NULL;
}

static void fcgi_hash_destroy(fcgi_hash *h)
{
	fcgi_hash_buckets *b;
	fcgi_data_seg *p;

	b = h->buckets;
	while (b) {
		fcgi_hash_buckets *q = b;
		b = b->next;
		free(q);
	}
	p = h->data;
	while (p) {
		fcgi_data_seg *q = p;
		p = p->next;
		free(q);
	}
}

static void fcgi_hash_clean(fcgi_hash *h)
{
	memset(h->hash_table, 0, sizeof(h->hash_table));
	h->list = NULL;
	/* delete all bucket blocks except the first one */
	while (h->buckets->next) {
		fcgi_hash_buckets *q = h->buckets;

		h->buckets = h->buckets->next;
		free(q);
	}
	h->buckets->idx = 0;
	/* delete all data segments except the first one */
	while (h->data->next) {
		fcgi_data_seg *q = h->data;

		h->data = h->data->next;
		free(q);
	}
	h->data->pos = h->data->data;
}

static inline char* fcgi_hash_strndup(fcgi_hash *h, char *str, unsigned int str_len)
{
	char *ret;

	if (UNEXPECTED(h->data->pos + str_len + 1 >= h->data->end)) {
		unsigned int seg_size = (str_len + 1 > FCGI_HASH_SEG_SIZE) ? str_len + 1 : FCGI_HASH_SEG_SIZE;
		fcgi_data_seg *p = (fcgi_data_seg*)malloc(sizeof(fcgi_data_seg) - 1 + seg_size);

		p->pos = p->data;
		p->end = p->pos + seg_size;
		p->next = h->data;
		h->data = p;
	}
	ret = h->data->pos;
	memcpy(ret, str, str_len);
	ret[str_len] = 0;
	h->data->pos += str_len + 1;
	return ret;
}

static char* fcgi_hash_set(fcgi_hash *h, unsigned int hash_value, char *var, unsigned int var_len, char *val, unsigned int val_len)
{
	unsigned int      idx = hash_value & FCGI_HASH_TABLE_MASK;
	fcgi_hash_bucket *p = h->hash_table[idx];

	while (UNEXPECTED(p != NULL)) {
		if (UNEXPECTED(p->hash_value == hash_value) &&
		    p->var_len == var_len &&
		    memcmp(p->var, var, var_len) == 0) {

			p->val_len = val_len;
			p->val = fcgi_hash_strndup(h, val, val_len);
			return p->val;
		}
		p = p->next;
	}

	if (UNEXPECTED(h->buckets->idx >= FCGI_HASH_TABLE_SIZE)) {
		fcgi_hash_buckets *b = (fcgi_hash_buckets*)malloc(sizeof(fcgi_hash_buckets));
		b->idx = 0;
		b->next = h->buckets;
		h->buckets = b;
	}
	p = h->buckets->data + h->buckets->idx;
	h->buckets->idx++;
	p->next = h->hash_table[idx];
	h->hash_table[idx] = p;
	p->list_next = h->list;
	h->list = p;
	p->hash_value = hash_value;
	p->var_len = var_len;
	p->var = fcgi_hash_strndup(h, var, var_len);
	p->val_len = val_len;
	p->val = fcgi_hash_strndup(h, val, val_len);
	return p->val;
}

static void fcgi_hash_del(fcgi_hash *h, unsigned int hash_value, char *var, unsigned int var_len)
{
	unsigned int      idx = hash_value & FCGI_HASH_TABLE_MASK;
	fcgi_hash_bucket **p = &h->hash_table[idx];

	while (*p != NULL) {
		if ((*p)->hash_value == hash_value &&
		    (*p)->var_len == var_len &&
		    memcmp((*p)->var, var, var_len) == 0) {

		    (*p)->val = NULL; /* NULL value means deleted */
		    (*p)->val_len = 0;
			*p = (*p)->next;
		    return;
		}
		p = &(*p)->next;
	}
}

static char *fcgi_hash_get(fcgi_hash *h, unsigned int hash_value, char *var, unsigned int var_len, unsigned int *val_len)
{
	unsigned int      idx = hash_value & FCGI_HASH_TABLE_MASK;
	fcgi_hash_bucket *p = h->hash_table[idx];

	while (p != NULL) {
		if (p->hash_value == hash_value &&
		    p->var_len == var_len &&
		    memcmp(p->var, var, var_len) == 0) {
		    *val_len = p->val_len;
		    return p->val;
		}
		p = p->next;
	}
	return NULL;
}

static void fcgi_hash_apply(fcgi_hash *h, fcgi_apply_func func, void *arg)
{
	fcgi_hash_bucket *p	= h->list;

	while (p) {
		if (EXPECTED(p->val != NULL)) {
			func(p->var, p->var_len, p->val, p->val_len, arg);
		}
		p = p->list_next;
	}
}

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

void fcgi_set_in_shutdown(int new_value)
{
	in_shutdown = new_value;
}

int fcgi_in_shutdown(void)
{
	return in_shutdown;
}

void fcgi_terminate(void)
{
	in_shutdown = 1;
}

void fcgi_request_set_keep(fcgi_request *req, int new_value)
{
	req->keep = new_value;
}

#ifndef HAVE_ATTRIBUTE_WEAK
void fcgi_set_logger(fcgi_logger lg) {
	fcgi_log = lg;
}
#else
void __attribute__((weak)) fcgi_log(int type, const char *format, ...) {
	va_list ap;

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
}
#endif

int fcgi_init(void)
{
	if (!is_initialized) {
#ifndef _WIN32
		sa_t sa;
		socklen_t len = sizeof(sa);
#endif
		zend_hash_init(&fcgi_mgmt_vars, 8, NULL, fcgi_free_mgmt_var_cb, 1);
		fcgi_set_mgmt_var("FCGI_MPXS_CONNS", sizeof("FCGI_MPXS_CONNS")-1, "0", sizeof("0")-1);

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
		if ((GetStdHandle(STD_OUTPUT_HANDLE) == INVALID_HANDLE_VALUE) &&
		    (GetStdHandle(STD_ERROR_HANDLE)  == INVALID_HANDLE_VALUE) &&
		    (GetStdHandle(STD_INPUT_HANDLE)  != INVALID_HANDLE_VALUE)) {
			char *str;
			DWORD pipe_mode = PIPE_READMODE_BYTE | PIPE_WAIT;
			HANDLE pipe = GetStdHandle(STD_INPUT_HANDLE);

			SetNamedPipeHandleState(pipe, &pipe_mode, NULL, NULL);

			str = getenv("_FCGI_SHUTDOWN_EVENT_");
			if (str != NULL) {
				zend_long ev;
				HANDLE shutdown_event;

				ZEND_ATOL(ev, str);
				shutdown_event = (HANDLE) ev;
				if (!CreateThread(NULL, 0, fcgi_shutdown_thread,
				                  shutdown_event, 0, NULL)) {
					return -1;
				}
			}
			str = getenv("_FCGI_MUTEX_");
			if (str != NULL) {
				zend_long mt;
				ZEND_ATOL(mt, str);
				fcgi_accept_mutex = (HANDLE) mt;
			}
			return is_fastcgi = 1;
		} else {
			return is_fastcgi = 0;
		}
#else
		errno = 0;
		if (getpeername(0, (struct sockaddr *)&sa, &len) != 0 && errno == ENOTCONN) {
			fcgi_setup_signals();
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

void fcgi_shutdown(void)
{
	if (is_initialized) {
		zend_hash_destroy(&fcgi_mgmt_vars);
	}
	is_fastcgi = 0;
	if (allowed_clients) {
		free(allowed_clients);
	}
}

#ifdef _WIN32
/* Do some black magic with the NT security API.
 * We prepare a DACL (Discretionary Access Control List) so that
 * we, the creator, are allowed all access, while "Everyone Else"
 * is only allowed to read and write to the pipe.
 * This avoids security issues on shared hosts where a luser messes
 * with the lower-level pipe settings and screws up the FastCGI service.
 */
static PACL prepare_named_pipe_acl(PSECURITY_DESCRIPTOR sd, LPSECURITY_ATTRIBUTES sa)
{
	DWORD req_acl_size;
	char everyone_buf[32], owner_buf[32];
	PSID sid_everyone, sid_owner;
	SID_IDENTIFIER_AUTHORITY
		siaWorld = SECURITY_WORLD_SID_AUTHORITY,
		siaCreator = SECURITY_CREATOR_SID_AUTHORITY;
	PACL acl;

	sid_everyone = (PSID)&everyone_buf;
	sid_owner = (PSID)&owner_buf;

	req_acl_size = sizeof(ACL) +
		(2 * ((sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD)) + GetSidLengthRequired(1)));

	acl = malloc(req_acl_size);

	if (acl == NULL) {
		return NULL;
	}

	if (!InitializeSid(sid_everyone, &siaWorld, 1)) {
		goto out_fail;
	}
	*GetSidSubAuthority(sid_everyone, 0) = SECURITY_WORLD_RID;

	if (!InitializeSid(sid_owner, &siaCreator, 1)) {
		goto out_fail;
	}
	*GetSidSubAuthority(sid_owner, 0) = SECURITY_CREATOR_OWNER_RID;

	if (!InitializeAcl(acl, req_acl_size, ACL_REVISION)) {
		goto out_fail;
	}

	if (!AddAccessAllowedAce(acl, ACL_REVISION, FILE_GENERIC_READ | FILE_GENERIC_WRITE, sid_everyone)) {
		goto out_fail;
	}

	if (!AddAccessAllowedAce(acl, ACL_REVISION, FILE_ALL_ACCESS, sid_owner)) {
		goto out_fail;
	}

	if (!InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION)) {
		goto out_fail;
	}

	if (!SetSecurityDescriptorDacl(sd, TRUE, acl, FALSE)) {
		goto out_fail;
	}

	sa->lpSecurityDescriptor = sd;

	return acl;

out_fail:
	free(acl);
	return NULL;
}
#endif

static int is_port_number(const char *bindpath)
{
	while (*bindpath) {
		if (*bindpath < '0' || *bindpath > '9') {
			return 0;
		}
		bindpath++;
	}
	return 1;
}

int fcgi_listen(const char *path, int backlog)
{
	char     *s;
	int       tcp = 0;
	char      host[MAXPATHLEN];
	short     port = 0;
	int       listen_socket;
	sa_t      sa;
	socklen_t sock_len;
#ifdef SO_REUSEADDR
# ifdef _WIN32
	BOOL reuse = 1;
# else
	int reuse = 1;
# endif
#endif

	if ((s = strchr(path, ':'))) {
		port = atoi(s+1);
		if (port != 0 && (s-path) < MAXPATHLEN) {
			strncpy(host, path, s-path);
			host[s-path] = '\0';
			tcp = 1;
		}
	} else if (is_port_number(path)) {
		port = atoi(path);
		if (port != 0) {
			host[0] = '\0';
			tcp = 1;
		}
	}

	/* Prepare socket address */
	if (tcp) {
		memset(&sa.sa_inet, 0, sizeof(sa.sa_inet));
		sa.sa_inet.sin_family = AF_INET;
		sa.sa_inet.sin_port = htons(port);
		sock_len = sizeof(sa.sa_inet);

		if (!*host || !strncmp(host, "*", sizeof("*")-1)) {
			sa.sa_inet.sin_addr.s_addr = htonl(INADDR_ANY);
		} else {
			sa.sa_inet.sin_addr.s_addr = inet_addr(host);
			if (sa.sa_inet.sin_addr.s_addr == INADDR_NONE) {
				struct hostent *hep;

				if(strlen(host) > MAXFQDNLEN) {
					hep = NULL;
				} else {
					hep = php_network_gethostbyname(host);
				}
				if (!hep || hep->h_addrtype != AF_INET || !hep->h_addr_list[0]) {
					fcgi_log(FCGI_ERROR, "Cannot resolve host name '%s'!\n", host);
					return -1;
				} else if (hep->h_addr_list[1]) {
					fcgi_log(FCGI_ERROR, "Host '%s' has multiple addresses. You must choose one explicitly!\n", host);
					return -1;
				}
				sa.sa_inet.sin_addr.s_addr = ((struct in_addr*)hep->h_addr_list[0])->s_addr;
			}
		}
	} else {
#ifdef _WIN32
		SECURITY_DESCRIPTOR  sd;
		SECURITY_ATTRIBUTES  saw;
		PACL                 acl;
		HANDLE namedPipe;

		memset(&sa, 0, sizeof(saw));
		saw.nLength = sizeof(saw);
		saw.bInheritHandle = FALSE;
		acl = prepare_named_pipe_acl(&sd, &saw);

		namedPipe = CreateNamedPipe(path,
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_BYTE | PIPE_WAIT | PIPE_READMODE_BYTE,
			PIPE_UNLIMITED_INSTANCES,
			8192, 8192, 0, &saw);
		if (namedPipe == INVALID_HANDLE_VALUE) {
			return -1;
		}
		listen_socket = _open_osfhandle((intptr_t)namedPipe, 0);
		if (!is_initialized) {
			fcgi_init();
		}
		is_fastcgi = 1;
		return listen_socket;

#else
		size_t path_len = strlen(path);

		if (path_len >= sizeof(sa.sa_unix.sun_path)) {
			fcgi_log(FCGI_ERROR, "Listening socket's path name is too long.\n");
			return -1;
		}

		memset(&sa.sa_unix, 0, sizeof(sa.sa_unix));
		sa.sa_unix.sun_family = AF_UNIX;
		memcpy(sa.sa_unix.sun_path, path, path_len + 1);
		sock_len = (size_t)(((struct sockaddr_un *)0)->sun_path)	+ path_len;
#ifdef HAVE_SOCKADDR_UN_SUN_LEN
		sa.sa_unix.sun_len = sock_len;
#endif
		unlink(path);
#endif
	}

	/* Create, bind socket and start listen on it */
	if ((listen_socket = socket(sa.sa.sa_family, SOCK_STREAM, 0)) < 0 ||
#ifdef SO_REUSEADDR
	    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0 ||
#endif
	    bind(listen_socket, (struct sockaddr *) &sa, sock_len) < 0 ||
	    listen(listen_socket, backlog) < 0) {
		close(listen_socket);
		fcgi_log(FCGI_ERROR, "Cannot bind/listen socket - [%d] %s.\n",errno, strerror(errno));
		return -1;
	}

	if (!tcp) {
		chmod(path, 0777);
	} else {
		char *ip = getenv("FCGI_WEB_SERVER_ADDRS");
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
			allowed_clients = malloc(sizeof(sa_t) * (n+2));
			n = 0;
			cur = ip;
			while (cur) {
				end = strchr(cur, ',');
				if (end) {
					*end = 0;
					end++;
				}
				if (inet_pton(AF_INET, cur, &allowed_clients[n].sa_inet.sin_addr)>0) {
					allowed_clients[n].sa.sa_family = AF_INET;
					n++;
#ifdef HAVE_IPV6
				} else if (inet_pton(AF_INET6, cur, &allowed_clients[n].sa_inet6.sin6_addr)>0) {
					allowed_clients[n].sa.sa_family = AF_INET6;
					n++;
#endif
				} else {
					fcgi_log(FCGI_ERROR, "Wrong IP address '%s' in listen.allowed_clients", cur);
				}
				cur = end;
			}
			allowed_clients[n].sa.sa_family = 0;
			free(ip);
			if (!n) {
				fcgi_log(FCGI_ERROR, "There are no allowed addresses");
				/* don't clear allowed_clients as it will create an "open for all" security issue */
			}
		}
	}

	if (!is_initialized) {
		fcgi_init();
	}
	is_fastcgi = 1;

#ifdef _WIN32
	if (tcp) {
		listen_socket = _open_osfhandle((intptr_t)listen_socket, 0);
	}
#else
	fcgi_setup_signals();
#endif
	return listen_socket;
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
		allowed_clients = malloc(sizeof(sa_t) * (n+2));
		n = 0;
		cur = ip;
		while (cur) {
			end = strchr(cur, ',');
			if (end) {
				*end = 0;
				end++;
			}
			if (inet_pton(AF_INET, cur, &allowed_clients[n].sa_inet.sin_addr)>0) {
				allowed_clients[n].sa.sa_family = AF_INET;
				n++;
#ifdef HAVE_IPV6
			} else if (inet_pton(AF_INET6, cur, &allowed_clients[n].sa_inet6.sin6_addr)>0) {
				allowed_clients[n].sa.sa_family = AF_INET6;
				n++;
#endif
			} else {
				fcgi_log(FCGI_ERROR, "Wrong IP address '%s' in listen.allowed_clients", cur);
			}
			cur = end;
		}
		allowed_clients[n].sa.sa_family = 0;
		free(ip);
		if (!n) {
			fcgi_log(FCGI_ERROR, "There are no allowed addresses");
			/* don't clear allowed_clients as it will create an "open for all" security issue */
		}
	}
}

static void fcgi_hook_dummy() {
	return;
}

fcgi_request *fcgi_init_request(int listen_socket, void(*on_accept)(), void(*on_read)(), void(*on_close)())
{
	fcgi_request *req = calloc(1, sizeof(fcgi_request));
	req->listen_socket = listen_socket;
	req->fd = -1;
	req->id = -1;

	/*
	req->in_len = 0;
	req->in_pad = 0;

	req->out_hdr = NULL;

#ifdef TCP_NODELAY
	req->nodelay = 0;
#endif

	req->env = NULL;
	req->has_env = 0;

	*/
	req->out_pos = req->out_buf;
	req->hook.on_accept = on_accept ? on_accept : fcgi_hook_dummy;
	req->hook.on_read = on_read ? on_read : fcgi_hook_dummy;
	req->hook.on_close = on_close ? on_close : fcgi_hook_dummy;

#ifdef _WIN32
	req->tcp = !GetNamedPipeInfo((HANDLE)_get_osfhandle(req->listen_socket), NULL, NULL, NULL, NULL);
#endif

	fcgi_hash_init(&req->env);

	return req;
}

void fcgi_destroy_request(fcgi_request *req) {
	fcgi_hash_destroy(&req->env);
	free(req);
}

static inline ssize_t safe_write(fcgi_request *req, const void *buf, size_t count)
{
	int    ret;
	size_t n = 0;

	do {
#ifdef _WIN32
		size_t tmp;
#endif
		errno = 0;
#ifdef _WIN32
		tmp = count - n;

		if (!req->tcp) {
			unsigned int out_len = tmp > UINT_MAX ? UINT_MAX : (unsigned int)tmp;

			ret = write(req->fd, ((char*)buf)+n, out_len);
		} else {
			int out_len = tmp > INT_MAX ? INT_MAX : (int)tmp;

			ret = send(req->fd, ((char*)buf)+n, out_len, 0);
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
#ifdef _WIN32
		size_t tmp;
#endif
		errno = 0;
#ifdef _WIN32
		tmp = count - n;

		if (!req->tcp) {
			unsigned int in_len = tmp > UINT_MAX ? UINT_MAX : (unsigned int)tmp;

			ret = read(req->fd, ((char*)buf)+n, in_len);
		} else {
			int in_len = tmp > INT_MAX ? INT_MAX : (int)tmp;

			ret = recv(req->fd, ((char*)buf)+n, in_len, 0);
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

static int fcgi_get_params(fcgi_request *req, unsigned char *p, unsigned char *end)
{
	unsigned int name_len, val_len;

	while (p < end) {
		name_len = *p++;
		if (UNEXPECTED(name_len >= 128)) {
			if (UNEXPECTED(p + 3 >= end)) return 0;
			name_len = ((name_len & 0x7f) << 24);
			name_len |= (*p++ << 16);
			name_len |= (*p++ << 8);
			name_len |= *p++;
		}
		if (UNEXPECTED(p >= end)) return 0;
		val_len = *p++;
		if (UNEXPECTED(val_len >= 128)) {
			if (UNEXPECTED(p + 3 >= end)) return 0;
			val_len = ((val_len & 0x7f) << 24);
			val_len |= (*p++ << 16);
			val_len |= (*p++ << 8);
			val_len |= *p++;
		}
		if (UNEXPECTED(name_len + val_len > (unsigned int) (end - p))) {
			/* Malformated request */
			return 0;
		}
		fcgi_hash_set(&req->env, FCGI_HASH_FUNC(p, name_len), (char*)p, name_len, (char*)p + name_len, val_len);
		p += name_len + val_len;
	}
	return 1;
}

static int fcgi_read_request(fcgi_request *req)
{
	fcgi_header hdr;
	int len, padding;
	unsigned char buf[FCGI_MAX_LENGTH+8];

	req->keep = 0;
	req->ended = 0;
	req->in_len = 0;
	req->out_hdr = NULL;
	req->out_pos = req->out_buf;

	if (req->has_env) {
		fcgi_hash_clean(&req->env);
	} else {
		req->has_env = 1;
	}

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
		fcgi_begin_request *b;

		if (safe_read(req, buf, len+padding) != len+padding) {
			return 0;
		}

		b = (fcgi_begin_request*)buf;
		req->keep = (b->flags & FCGI_KEEP_CONN);
#ifdef TCP_NODELAY
		if (req->keep && req->tcp && !req->nodelay) {
# ifdef _WIN32
			BOOL on = 1;
# else
			int on = 1;
# endif

			setsockopt(req->fd, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on));
			req->nodelay = 1;
		}
#endif
		switch ((b->roleB1 << 8) + b->roleB0) {
			case FCGI_RESPONDER:
				fcgi_hash_set(&req->env, FCGI_HASH_FUNC("FCGI_ROLE", sizeof("FCGI_ROLE")-1), "FCGI_ROLE", sizeof("FCGI_ROLE")-1, "RESPONDER", sizeof("RESPONDER")-1);
				break;
			case FCGI_AUTHORIZER:
				fcgi_hash_set(&req->env, FCGI_HASH_FUNC("FCGI_ROLE", sizeof("FCGI_ROLE")-1), "FCGI_ROLE", sizeof("FCGI_ROLE")-1, "AUTHORIZER", sizeof("AUTHORIZER")-1);
				break;
			case FCGI_FILTER:
				fcgi_hash_set(&req->env, FCGI_HASH_FUNC("FCGI_ROLE", sizeof("FCGI_ROLE")-1), "FCGI_ROLE", sizeof("FCGI_ROLE")-1, "FILTER", sizeof("FILTER")-1);
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
		zval *value;
		unsigned int zlen;
		fcgi_hash_bucket *q;

		if (safe_read(req, buf, len+padding) != len+padding) {
			req->keep = 0;
			return 0;
		}

		if (!fcgi_get_params(req, buf, buf+len)) {
			req->keep = 0;
			return 0;
		}

		q = req->env.list;
		while (q != NULL) {
			if ((value = zend_hash_str_find(&fcgi_mgmt_vars, q->var, q->var_len)) == NULL) {
				q = q->list_next;
				continue;
			}
			zlen = (unsigned int)Z_STRLEN_P(value);
			if ((p + 4 + 4 + q->var_len + zlen) >= (buf + sizeof(buf))) {
				break;
			}
			if (q->var_len < 0x80) {
				*p++ = q->var_len;
			} else {
				*p++ = ((q->var_len >> 24) & 0xff) | 0x80;
				*p++ = (q->var_len >> 16) & 0xff;
				*p++ = (q->var_len >> 8) & 0xff;
				*p++ = q->var_len & 0xff;
			}
			if (zlen < 0x80) {
				*p++ = zlen;
			} else {
				*p++ = ((zlen >> 24) & 0xff) | 0x80;
				*p++ = (zlen >> 16) & 0xff;
				*p++ = (zlen >> 8) & 0xff;
				*p++ = zlen & 0xff;
			}
			memcpy(p, q->var, q->var_len);
			p += q->var_len;
			memcpy(p, Z_STRVAL_P(value), zlen);
			p += zlen;
			q = q->list_next;
		}
		len = (int)(p - buf - sizeof(fcgi_header));
		len += fcgi_make_header((fcgi_header*)buf, FCGI_GET_VALUES_RESULT, 0, len);
		if (safe_write(req, buf, sizeof(fcgi_header) + len) != (ssize_t)sizeof(fcgi_header)+len) {
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
			ret = (int)safe_read(req, str, rest);
		} else {
			ret = (int)safe_read(req, str, req->in_len);
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
	if (destroy && req->has_env) {
		fcgi_hash_clean(&req->env);
		req->has_env = 0;
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
				/* read any remaining data, it may be omitted */
				while (recv(req->fd, buf, sizeof(buf), 0) > 0) {}
			}
			closesocket(req->fd);
		}
#else
		if (!force) {
			char buf[8];

			shutdown(req->fd, 1);
			/* read any remaining data, it may be omitted */
			while (recv(req->fd, buf, sizeof(buf), 0) > 0) {}
		}
		close(req->fd);
#endif
#ifdef TCP_NODELAY
		req->nodelay = 0;
#endif
		req->fd = -1;

		req->hook.on_close();
	}
}

int fcgi_is_closed(fcgi_request *req)
{
	return (req->fd < 0);
}

static int fcgi_is_allowed() {
	int i;

	if (client_sa.sa.sa_family == AF_UNIX) {
		return 1;
	}
	if (!allowed_clients) {
		return 1;
	}
	if (client_sa.sa.sa_family == AF_INET) {
		for (i = 0; allowed_clients[i].sa.sa_family ; i++) {
			if (allowed_clients[i].sa.sa_family == AF_INET
				&& !memcmp(&client_sa.sa_inet.sin_addr, &allowed_clients[i].sa_inet.sin_addr, 4)) {
				return 1;
			}
		}
	}
#ifdef HAVE_IPV6
	if (client_sa.sa.sa_family == AF_INET6) {
		for (i = 0; allowed_clients[i].sa.sa_family ; i++) {
			if (allowed_clients[i].sa.sa_family == AF_INET6
				&& !memcmp(&client_sa.sa_inet6.sin6_addr, &allowed_clients[i].sa_inet6.sin6_addr, 12)) {
				return 1;
			}
#ifdef IN6_IS_ADDR_V4MAPPED
			if (allowed_clients[i].sa.sa_family == AF_INET
			    && IN6_IS_ADDR_V4MAPPED(&client_sa.sa_inet6.sin6_addr)
				&& !memcmp(((char *)&client_sa.sa_inet6.sin6_addr)+12, &allowed_clients[i].sa_inet.sin_addr, 4)) {
				return 1;
			}
#endif
		}
	}
#endif

	return 0;
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

				req->hook.on_accept();
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

					FCGI_LOCK(req->listen_socket);
					req->fd = accept(listen_socket, (struct sockaddr *)&sa, &len);
					FCGI_UNLOCK(req->listen_socket);

					client_sa = sa;
					if (req->fd >= 0 && !fcgi_is_allowed()) {
						fcgi_log(FCGI_ERROR, "Connection disallowed: IP address '%s' has been dropped.", fcgi_get_last_client_ip());
						closesocket(req->fd);
						req->fd = -1;
						continue;
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
#if defined(HAVE_POLL)
					struct pollfd fds;
					int ret;

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
						fcgi_log(FCGI_ERROR, "Too many open file descriptors. FD_SETSIZE limit exceeded.");
						fcgi_close(req, 1, 0);
					}
#endif
				}
#endif
			}
		} else if (in_shutdown) {
			return -1;
		}
		req->hook.on_read();
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
		int len = (int)(req->out_pos - ((unsigned char*)req->out_hdr + sizeof(fcgi_header)));

		req->out_pos += fcgi_make_header(req->out_hdr, (fcgi_request_type)req->out_hdr->type, req->id, len);
		req->out_hdr = NULL;
	}
}

int fcgi_flush(fcgi_request *req, int end)
{
	int len;

	close_packet(req);

	len = (int)(req->out_pos - req->out_buf);

	if (end) {
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
		req->out_pos = req->out_buf;
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
#if 0
	/* Unoptimized, but clear version */
	rest = len;
	while (rest > 0) {
		limit = sizeof(req->out_buf) - (req->out_pos - req->out_buf);

		if (!req->out_hdr) {
			if (limit < sizeof(fcgi_header)) {
				if (!fcgi_flush(req, 0)) {
					return -1;
				}
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
			if (!fcgi_flush(req, 0)) {
				return -1;
			}
		}
	}
#else
	/* Optimized version */
	limit = (int)(sizeof(req->out_buf) - (req->out_pos - req->out_buf));
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
	} else if (len - limit < (int)(sizeof(req->out_buf) - sizeof(fcgi_header))) {
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
#endif
	return len;
}

int fcgi_end(fcgi_request *req) {
	int ret = 1;
	if (!req->ended) {
		ret = fcgi_flush(req, 1);
		req->ended = 1;
	}
	return ret;
}

int fcgi_finish_request(fcgi_request *req, int force_close)
{
	int ret = 1;

	if (req->fd >= 0) {
		ret = fcgi_end(req);
		fcgi_close(req, force_close, 1);
	}
	return ret;
}

int fcgi_has_env(fcgi_request *req)
{
	return req && req->has_env;
}

char* fcgi_getenv(fcgi_request *req, const char* var, int var_len)
{
	unsigned int val_len;

	if (!req) return NULL;

	return fcgi_hash_get(&req->env, FCGI_HASH_FUNC(var, var_len), (char*)var, var_len, &val_len);
}

char* fcgi_quick_getenv(fcgi_request *req, const char* var, int var_len, unsigned int hash_value)
{
	unsigned int val_len;

	return fcgi_hash_get(&req->env, hash_value, (char*)var, var_len, &val_len);
}

char* fcgi_putenv(fcgi_request *req, char* var, int var_len, char* val)
{
	if (!req) return NULL;
	if (val == NULL) {
		fcgi_hash_del(&req->env, FCGI_HASH_FUNC(var, var_len), var, var_len);
		return NULL;
	} else {
		return fcgi_hash_set(&req->env, FCGI_HASH_FUNC(var, var_len), var, var_len, val, (unsigned int)strlen(val));
	}
}

char* fcgi_quick_putenv(fcgi_request *req, char* var, int var_len, unsigned int hash_value, char* val)
{
	if (val == NULL) {
		fcgi_hash_del(&req->env, hash_value, var, var_len);
		return NULL;
	} else {
		return fcgi_hash_set(&req->env, hash_value, var, var_len, val, (unsigned int)strlen(val));
	}
}

void fcgi_loadenv(fcgi_request *req, fcgi_apply_func func, zval *array)
{
	fcgi_hash_apply(&req->env, func, array);
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

void fcgi_set_mgmt_var(const char * name, size_t name_len, const char * value, size_t value_len)
{
	zval zvalue;
	zend_string *key = zend_string_init(name, name_len, 1);
	ZVAL_NEW_STR(&zvalue, zend_string_init(value, value_len, 1));
	GC_MAKE_PERSISTENT_LOCAL(key);
	GC_MAKE_PERSISTENT_LOCAL(Z_STR(zvalue));
	zend_hash_add(&fcgi_mgmt_vars, key, &zvalue);
	zend_string_release_ex(key, 1);
}

void fcgi_free_mgmt_var_cb(zval *zv)
{
	pefree(Z_STR_P(zv), 1);
}

const char *fcgi_get_last_client_ip()
{
	static char str[INET6_ADDRSTRLEN];

	/* Ipv4 */
	if (client_sa.sa.sa_family == AF_INET) {
		return inet_ntop(client_sa.sa.sa_family, &client_sa.sa_inet.sin_addr, str, INET6_ADDRSTRLEN);
	}
#ifdef HAVE_IPV6
#ifdef IN6_IS_ADDR_V4MAPPED
	/* Ipv4-Mapped-Ipv6 */
	if (client_sa.sa.sa_family == AF_INET6
		&& IN6_IS_ADDR_V4MAPPED(&client_sa.sa_inet6.sin6_addr)) {
		return inet_ntop(AF_INET, ((char *)&client_sa.sa_inet6.sin6_addr)+12, str, INET6_ADDRSTRLEN);
	}
#endif
	/* Ipv6 */
	if (client_sa.sa.sa_family == AF_INET6) {
		return inet_ntop(client_sa.sa.sa_family, &client_sa.sa_inet6.sin6_addr, str, INET6_ADDRSTRLEN);
	}
#endif
	/* Unix socket */
	return NULL;
}
