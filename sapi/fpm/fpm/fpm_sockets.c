
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

struct listening_socket_s {
	int refcount;
	int sock;
	int type;
	char *key;
};

static struct fpm_array_s sockets_list;

static int fpm_sockets_resolve_af_inet(char *node, char *service, struct sockaddr_in *addr) /* {{{ */
{
	struct addrinfo *res;
	struct addrinfo hints;
	int ret;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	ret = getaddrinfo(node, service, &hints, &res);

	if (ret != 0) {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "can't resolve hostname '%s%s%s': getaddrinfo said: %s%s%s\n",
					node, service ? ":" : "", service ? service : "",
					gai_strerror(ret), ret == EAI_SYSTEM ? ", system error: " : "", ret == EAI_SYSTEM ? strerror(errno) : "");
		return -1;
	}

	*addr = *(struct sockaddr_in *) res->ai_addr;
	freeaddrinfo(res);
	return 0;
}
/* }}} */

enum { FPM_GET_USE_SOCKET = 1, FPM_STORE_SOCKET = 2, FPM_STORE_USE_SOCKET = 3 };

static void fpm_sockets_cleanup(int which, void *arg) /* {{{ */
{
	int i;
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

static int fpm_sockets_hash_op(int sock, struct sockaddr *sa, char *key, int type, int op) /* {{{ */
{
	if (key == NULL) {
		switch (type) {
			case FPM_AF_INET : {
				struct sockaddr_in *sa_in = (struct sockaddr_in *) sa;
				key = alloca(sizeof("xxx.xxx.xxx.xxx:ppppp"));
				sprintf(key, "%u.%u.%u.%u:%u", IPQUAD(&sa_in->sin_addr), (unsigned int) ntohs(sa_in->sin_port));
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
			int i;
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
	mode_t saved_umask;

	sock = socket(sa->sa_family, SOCK_STREAM, 0);

	if (0 > sock) {
		zlog(ZLOG_STUFF, ZLOG_SYSERROR, "socket() failed");
		return -1;
	}

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(flags));

	if (wp->listen_address_domain == FPM_AF_UNIX) {
		unlink( ((struct sockaddr_un *) sa)->sun_path);
	}

	saved_umask = umask(0777 ^ wp->socket_mode);

	if (0 > bind(sock, sa, socklen)) {
		zlog(ZLOG_STUFF, ZLOG_SYSERROR, "bind() for address '%s' failed", wp->config->listen_address);
		return -1;
	}

	if (wp->listen_address_domain == FPM_AF_UNIX) {
		char *path = ((struct sockaddr_un *) sa)->sun_path;
		if (wp->socket_uid != -1 || wp->socket_gid != -1) {
			if (0 > chown(path, wp->socket_uid, wp->socket_gid)) {
				zlog(ZLOG_STUFF, ZLOG_SYSERROR, "chown() for address '%s' failed", wp->config->listen_address);
				return -1;
			}
		}
	}
	umask(saved_umask);

	if (0 > listen(sock, wp->config->listen_backlog)) {
		zlog(ZLOG_STUFF, ZLOG_SYSERROR, "listen() for address '%s' failed", wp->config->listen_address);
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
	struct sockaddr_in sa_in;
	char *dup_address = strdup(wp->config->listen_address);
	char *port_str = strchr(dup_address, ':');
	char *addr = NULL;
	int port = 0;

	if (port_str) { /* this is host:port pair */
		*port_str++ = '\0';
		port = atoi(port_str);
		addr = dup_address;
	} else if (strlen(dup_address) == strspn(dup_address, "0123456789")) { /* this is port */
		port = atoi(dup_address);
		port_str = dup_address;
	}

	if (port == 0) {
		zlog(ZLOG_STUFF, ZLOG_ERROR, "invalid port value '%s'", port_str);
		return -1;
	}

	memset(&sa_in, 0, sizeof(sa_in));

	if (addr) {
		sa_in.sin_addr.s_addr = inet_addr(addr);
		if (sa_in.sin_addr.s_addr == INADDR_NONE) { /* do resolve */
			if (0 > fpm_sockets_resolve_af_inet(addr, NULL, &sa_in)) {
				return -1;
			}
			zlog(ZLOG_STUFF, ZLOG_NOTICE, "address '%s' resolved as %u.%u.%u.%u", addr, IPQUAD(&sa_in.sin_addr));
		}
	} else {
		sa_in.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(port);
	free(dup_address);
	return fpm_sockets_get_listening_socket(wp, (struct sockaddr *) &sa_in, sizeof(struct sockaddr_in));
}
/* }}} */

static int fpm_socket_af_unix_listening_socket(struct fpm_worker_pool_s *wp) /* {{{ */
{
	struct sockaddr_un sa_un;

	memset(&sa_un, 0, sizeof(sa_un));
	cpystrn(sa_un.sun_path, wp->config->listen_address, sizeof(sa_un.sun_path));
	sa_un.sun_family = AF_UNIX;
	return fpm_sockets_get_listening_socket(wp, (struct sockaddr *) &sa_un, sizeof(struct sockaddr_un));
}
/* }}} */

int fpm_sockets_init_main() /* {{{ */
{
	int i;
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
			zlog(ZLOG_STUFF, ZLOG_NOTICE, "using inherited socket fd=%d, \"%s\"", fd_no, inherited);
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

