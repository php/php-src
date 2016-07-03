
	/* $Id: fpm_sockets.c,v 1.20.2.1 2008/12/13 03:21:18 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include <sys/types.h>
#include <sys/stat.h> /* for chmod(2) */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "zlog.h"
#include "fpm_arrays.h"
#include "fpm_sockets.h"
#include "fpm_worker_pool.h"
#include "fpm_unix.h"
#include "fpm_str.h"
#include "fpm_env.h"
#include "fpm_cleanup.h"
#include "fpm_scoreboard.h"

struct listening_socket_s {
	int refcount;
	int sock;
	int type;
	char *key;
};

static struct fpm_array_s sockets_list;

enum { FPM_GET_USE_SOCKET = 1, FPM_STORE_SOCKET = 2, FPM_STORE_USE_SOCKET = 3 };

static void fpm_sockets_cleanup(int which, void *arg) /* {{{ */
{
	unsigned i;
	char *env_value = 0;
	int p = 0;
	struct listening_socket_s *ls = sockets_list.data;

	for (i = 0; i < sockets_list.used; i++, ls++) {
		if (which != FPM_CLEANUP_PARENT_EXEC) {
			close(ls->sock);
		} else { /* on PARENT EXEC we want socket fds to be inherited through environment variable */
			char fd[32];
			sprintf(fd, "%d", ls->sock);
			env_value = realloc(env_value, p + (p ? 1 : 0) + strlen(ls->key) + 1 + strlen(fd) + 1);
			p += sprintf(env_value + p, "%s%s=%s", p ? "," : "", ls->key, fd);
		}

		if (which == FPM_CLEANUP_PARENT_EXIT_MAIN) {
			if (ls->type == FPM_AF_UNIX) {
				unlink(ls->key);
			}
		}
		free(ls->key);
	}

	if (env_value) {
		setenv("FPM_SOCKETS", env_value, 1);
		free(env_value);
	}

	fpm_array_free(&sockets_list);
}
/* }}} */

static void *fpm_get_in_addr(struct sockaddr *sa) /* {{{ */
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
/* }}} */

static int fpm_get_in_port(struct sockaddr *sa) /* {{{ */
{
    if (sa->sa_family == AF_INET) {
        return ntohs(((struct sockaddr_in*)sa)->sin_port);
    }

    return ntohs(((struct sockaddr_in6*)sa)->sin6_port);
}
/* }}} */

static int fpm_sockets_hash_op(int sock, struct sockaddr *sa, char *key, int type, int op) /* {{{ */
{
	if (key == NULL) {
		switch (type) {
			case FPM_AF_INET : {
				key = alloca(INET6_ADDRSTRLEN+10);
				inet_ntop(sa->sa_family, fpm_get_in_addr(sa), key, INET6_ADDRSTRLEN);
				sprintf(key+strlen(key), ":%d", fpm_get_in_port(sa));
				break;
			}

			case FPM_AF_UNIX : {
				struct sockaddr_un *sa_un = (struct sockaddr_un *) sa;
				key = alloca(strlen(sa_un->sun_path) + 1);
				strcpy(key, sa_un->sun_path);
				break;
			}

			default :
				return -1;
		}
	}

	switch (op) {

		case FPM_GET_USE_SOCKET :
		{
			unsigned i;
			struct listening_socket_s *ls = sockets_list.data;

			for (i = 0; i < sockets_list.used; i++, ls++) {
				if (!strcmp(ls->key, key)) {
					++ls->refcount;
					return ls->sock;
				}
			}
			break;
		}

		case FPM_STORE_SOCKET :			/* inherited socket */
		case FPM_STORE_USE_SOCKET :		/* just created */
		{
			struct listening_socket_s *ls;

			ls = fpm_array_push(&sockets_list);
			if (!ls) {
				break;
			}

			if (op == FPM_STORE_SOCKET) {
				ls->refcount = 0;
			} else {
				ls->refcount = 1;
			}
			ls->type = type;
			ls->sock = sock;
			ls->key = strdup(key);

			return 0;
		}
	}
	return -1;
}
/* }}} */

static int fpm_sockets_new_listening_socket(struct fpm_worker_pool_s *wp, struct sockaddr *sa, int socklen) /* {{{ */
{
	int flags = 1;
	int sock;
	mode_t saved_umask = 0;

	sock = socket(sa->sa_family, SOCK_STREAM, 0);

	if (0 > sock) {
		zlog(ZLOG_SYSERROR, "failed to create new listening socket: socket()");
		return -1;
	}

	if (0 > setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(flags))) {
		zlog(ZLOG_WARNING, "failed to change socket attribute");
	}

	if (wp->listen_address_domain == FPM_AF_UNIX) {
		if (fpm_socket_unix_test_connect((struct sockaddr_un *)sa, socklen) == 0) {
			zlog(ZLOG_ERROR, "An another FPM instance seems to already listen on %s", ((struct sockaddr_un *) sa)->sun_path);
			close(sock);
			return -1;
		}
		unlink( ((struct sockaddr_un *) sa)->sun_path);
		saved_umask = umask(0777 ^ wp->socket_mode);
	}

	if (0 > bind(sock, sa, socklen)) {
		zlog(ZLOG_SYSERROR, "unable to bind listening socket for address '%s'", wp->config->listen_address);
		if (wp->listen_address_domain == FPM_AF_UNIX) {
			umask(saved_umask);
		}
		close(sock);
		return -1;
	}

	if (wp->listen_address_domain == FPM_AF_UNIX) {
		char *path = ((struct sockaddr_un *) sa)->sun_path;

		umask(saved_umask);

		if (0 > fpm_unix_set_socket_premissions(wp, path)) {
			close(sock);
			return -1;
		}
	}

	if (0 > listen(sock, wp->config->listen_backlog)) {
		zlog(ZLOG_SYSERROR, "failed to listen to address '%s'", wp->config->listen_address);
		close(sock);
		return -1;
	}

	return sock;
}
/* }}} */

static int fpm_sockets_get_listening_socket(struct fpm_worker_pool_s *wp, struct sockaddr *sa, int socklen) /* {{{ */
{
	int sock;

	sock = fpm_sockets_hash_op(0, sa, 0, wp->listen_address_domain, FPM_GET_USE_SOCKET);
	if (sock >= 0) {
		return sock;
	}

	sock = fpm_sockets_new_listening_socket(wp, sa, socklen);
	fpm_sockets_hash_op(sock, sa, 0, wp->listen_address_domain, FPM_STORE_USE_SOCKET);

	return sock;
}
/* }}} */

enum fpm_address_domain fpm_sockets_domain_from_address(char *address) /* {{{ */
{
	if (strchr(address, ':')) {
		return FPM_AF_INET;
	}

	if (strlen(address) == strspn(address, "0123456789")) {
		return FPM_AF_INET;
	}
	return FPM_AF_UNIX;
}
/* }}} */

static int fpm_socket_af_inet_listening_socket(struct fpm_worker_pool_s *wp) /* {{{ */
{
	struct addrinfo hints, *servinfo, *p;
	char *dup_address = strdup(wp->config->listen_address);
	char *port_str = strrchr(dup_address, ':');
	char *addr = NULL;
	char tmpbuf[INET6_ADDRSTRLEN];
	int addr_len;
	int port = 0;
	int sock = -1;
	int status;

	if (port_str) { /* this is host:port pair */
		*port_str++ = '\0';
		port = atoi(port_str);
		addr = dup_address;
	} else if (strlen(dup_address) == strspn(dup_address, "0123456789")) { /* this is port */
		port = atoi(dup_address);
		port_str = dup_address;
	}

	if (port == 0) {
		zlog(ZLOG_ERROR, "invalid port value '%s'", port_str);
		return -1;
	}

	if (!addr) {
		/* no address: default documented behavior, all IPv4 addresses */
		struct sockaddr_in sa_in;

		memset(&sa_in, 0, sizeof(sa_in));
		sa_in.sin_family = AF_INET;
		sa_in.sin_port = htons(port);
		sa_in.sin_addr.s_addr = htonl(INADDR_ANY);
		free(dup_address);
		return fpm_sockets_get_listening_socket(wp, (struct sockaddr *) &sa_in, sizeof(struct sockaddr_in));
	}

	/* strip brackets from address for getaddrinfo */
	addr_len = strlen(addr);
	if (addr[0] == '[' && addr[addr_len - 1] == ']') {
		addr[addr_len - 1] = '\0';
		addr++;
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(addr, port_str, &hints, &servinfo)) != 0) {
		zlog(ZLOG_ERROR, "getaddrinfo: %s\n", gai_strerror(status));
		free(dup_address);
		return -1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		inet_ntop(p->ai_family, fpm_get_in_addr(p->ai_addr), tmpbuf, INET6_ADDRSTRLEN);
		if (sock < 0) {
			if ((sock = fpm_sockets_get_listening_socket(wp, p->ai_addr, p->ai_addrlen)) != -1) {
				zlog(ZLOG_DEBUG, "Found address for %s, socket opened on %s", dup_address, tmpbuf);
			}
		} else {
			zlog(ZLOG_WARNING, "Found multiple addresses for %s, %s ignored", dup_address, tmpbuf);
		}
	}

	free(dup_address);
	freeaddrinfo(servinfo);

	return sock;
}
/* }}} */

static int fpm_socket_af_unix_listening_socket(struct fpm_worker_pool_s *wp) /* {{{ */
{
	struct sockaddr_un sa_un;

	memset(&sa_un, 0, sizeof(sa_un));
	strlcpy(sa_un.sun_path, wp->config->listen_address, sizeof(sa_un.sun_path));
	sa_un.sun_family = AF_UNIX;
	return fpm_sockets_get_listening_socket(wp, (struct sockaddr *) &sa_un, sizeof(struct sockaddr_un));
}
/* }}} */

int fpm_sockets_init_main() /* {{{ */
{
	unsigned i, lq_len;
	struct fpm_worker_pool_s *wp;
	char *inherited = getenv("FPM_SOCKETS");
	struct listening_socket_s *ls;

	if (0 == fpm_array_init(&sockets_list, sizeof(struct listening_socket_s), 10)) {
		return -1;
	}

	/* import inherited sockets */
	while (inherited && *inherited) {
		char *comma = strchr(inherited, ',');
		int type, fd_no;
		char *eq;

		if (comma) {
			*comma = '\0';
		}

		eq = strchr(inherited, '=');
		if (eq) {
			*eq = '\0';
			fd_no = atoi(eq + 1);
			type = fpm_sockets_domain_from_address(inherited);
			zlog(ZLOG_NOTICE, "using inherited socket fd=%d, \"%s\"", fd_no, inherited);
			fpm_sockets_hash_op(fd_no, 0, inherited, type, FPM_STORE_SOCKET);
		}

		if (comma) {
			inherited = comma + 1;
		} else {
			inherited = 0;
		}
	}

	/* create all required sockets */
	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		switch (wp->listen_address_domain) {
			case FPM_AF_INET :
				wp->listening_socket = fpm_socket_af_inet_listening_socket(wp);
				break;

			case FPM_AF_UNIX :
				if (0 > fpm_unix_resolve_socket_premissions(wp)) {
					return -1;
				}
				wp->listening_socket = fpm_socket_af_unix_listening_socket(wp);
				break;
		}

		if (wp->listening_socket == -1) {
			return -1;
		}

	if (wp->listen_address_domain == FPM_AF_INET && fpm_socket_get_listening_queue(wp->listening_socket, NULL, &lq_len) >= 0) {
			fpm_scoreboard_update(-1, -1, -1, (int)lq_len, -1, -1, 0, FPM_SCOREBOARD_ACTION_SET, wp->scoreboard);
		}
	}

	/* close unused sockets that was inherited */
	ls = sockets_list.data;

	for (i = 0; i < sockets_list.used; ) {
		if (ls->refcount == 0) {
			close(ls->sock);
			if (ls->type == FPM_AF_UNIX) {
				unlink(ls->key);
			}
			free(ls->key);
			fpm_array_item_remove(&sockets_list, i);
		} else {
			++i;
			++ls;
		}
	}

	if (0 > fpm_cleanup_add(FPM_CLEANUP_ALL, fpm_sockets_cleanup, 0)) {
		return -1;
	}
	return 0;
}
/* }}} */

#if HAVE_FPM_LQ

#ifdef HAVE_LQ_TCP_INFO

#include <netinet/tcp.h>

int fpm_socket_get_listening_queue(int sock, unsigned *cur_lq, unsigned *max_lq)
{
	struct tcp_info info;
	socklen_t len = sizeof(info);

	if (0 > getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, &len)) {
		zlog(ZLOG_SYSERROR, "failed to retrieve TCP_INFO for socket");
		return -1;
	}
#if defined(__FreeBSD__)
	if (info.__tcpi_sacked == 0) {
		return -1;
	}

	if (cur_lq) {
		*cur_lq = info.__tcpi_unacked;
	}

	if (max_lq) {
		*max_lq = info.__tcpi_sacked;
	}
#else
	/* kernel >= 2.6.24 return non-zero here, that means operation is supported */
	if (info.tcpi_sacked == 0) {
		return -1;
	}

	if (cur_lq) {
		*cur_lq = info.tcpi_unacked;
	}

	if (max_lq) {
		*max_lq = info.tcpi_sacked;
	}
#endif

	return 0;
}

#endif

#ifdef HAVE_LQ_SO_LISTENQ

int fpm_socket_get_listening_queue(int sock, unsigned *cur_lq, unsigned *max_lq)
{
	int val;
	socklen_t len = sizeof(val);

	if (cur_lq) {
		if (0 > getsockopt(sock, SOL_SOCKET, SO_LISTENQLEN, &val, &len)) {
			return -1;
		}

		*cur_lq = val;
	}

	if (max_lq) {
		if (0 > getsockopt(sock, SOL_SOCKET, SO_LISTENQLIMIT, &val, &len)) {
			return -1;
		}

		*max_lq = val;
	}

	return 0;
}

#endif

#else

int fpm_socket_get_listening_queue(int sock, unsigned *cur_lq, unsigned *max_lq)
{
	return -1;
}

#endif

int fpm_socket_unix_test_connect(struct sockaddr_un *sock, size_t socklen) /* {{{ */
{
	int fd;

	if (!sock || sock->sun_family != AF_UNIX) {
		return -1;
	}

	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		return -1;
	}

	if (connect(fd, (struct sockaddr *)sock, socklen) == -1) {
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}
/* }}} */
