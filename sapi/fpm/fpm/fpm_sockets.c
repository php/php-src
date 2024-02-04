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

#ifdef SO_SETFIB
static int routemax = -1;
#endif

static inline void fpm_sockets_get_env_name(char *envname, unsigned idx) /* {{{ */
{
	if (!idx) {
		strcpy(envname, "FPM_SOCKETS");
	} else {
		sprintf(envname, "FPM_SOCKETS_%d", idx);
	}
}
/* }}} */

static void fpm_sockets_cleanup(int which, void *arg) /* {{{ */
{
	unsigned i;
	unsigned socket_set_count = 0;
	unsigned socket_set[FPM_ENV_SOCKET_SET_MAX];
	unsigned socket_set_buf = 0;
	char envname[32];
	char *env_value = 0;
	int p = 0;
	struct listening_socket_s *ls = sockets_list.data;

	for (i = 0; i < sockets_list.used; i++, ls++) {
		if (which != FPM_CLEANUP_PARENT_EXEC) {
			close(ls->sock);
		} else { /* on PARENT EXEC we want socket fds to be inherited through environment variable */
			char fd[32];
			char *tmpenv_value;
			sprintf(fd, "%d", ls->sock);

			socket_set_buf = (i % FPM_ENV_SOCKET_SET_SIZE == 0 && i) ? 1 : 0;
			tmpenv_value = realloc(env_value, p + (p ? 1 : 0) + strlen(ls->key) + 1 + strlen(fd) + socket_set_buf + 1);
			if (!tmpenv_value) {
				zlog(ZLOG_SYSERROR, "failure to inherit data on parent exec for socket `%s` due to memory allocation failure", ls->key);
				free(ls->key);
				break;
			}

			env_value = tmpenv_value;

			if (i % FPM_ENV_SOCKET_SET_SIZE == 0) {
				socket_set[socket_set_count] = p + socket_set_buf;
				socket_set_count++;
				if (i) {
					*(env_value + p + 1) = 0;
				}
			}

			p += sprintf(env_value + p + socket_set_buf, "%s%s=%s", (p && !socket_set_buf) ? "," : "", ls->key, fd);
			p += socket_set_buf;
		}

		if (which == FPM_CLEANUP_PARENT_EXIT_MAIN) {
			if (ls->type == FPM_AF_UNIX) {
				unlink(ls->key);
			}
		}
		free(ls->key);
	}

	if (env_value) {
		for (i = 0; i < socket_set_count; i++) {
			fpm_sockets_get_env_name(envname, i);
			setenv(envname, env_value + socket_set[i], 1);
		}
		fpm_sockets_get_env_name(envname, socket_set_count);
		unsetenv(envname);
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
			zlog(ZLOG_ERROR, "Another FPM instance seems to already listen on %s", ((struct sockaddr_un *) sa)->sun_path);
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

		if (0 > fpm_unix_set_socket_permissions(wp, path)) {
			close(sock);
			return -1;
		}
	}

	if (0 > listen(sock, wp->config->listen_backlog)) {
		zlog(ZLOG_SYSERROR, "failed to listen to address '%s'", wp->config->listen_address);
		close(sock);
		return -1;
	}

#ifdef SO_SETFIB
	if (-1 < wp->config->listen_setfib) {
		if (routemax < wp->config->listen_setfib) {
			zlog(ZLOG_ERROR, "Invalid routing table id %d, max is %d", wp->config->listen_setfib, routemax);
			close(sock);
			return -1;
		}

		if (0 > setsockopt(sock, SOL_SOCKET, SO_SETFIB, &wp->config->listen_setfib, sizeof(wp->config->listen_setfib))) {
			zlog(ZLOG_WARNING, "failed to change socket SO_SETFIB attribute");
		}
	}
#endif

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

static int fpm_socket_af_inet_socket_by_addr(struct fpm_worker_pool_s *wp, const char *addr, const char *port) /* {{{ */
{
	struct addrinfo hints, *servinfo, *p;
	char tmpbuf[INET6_ADDRSTRLEN];
	int status;
	int sock = -1;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(addr, port, &hints, &servinfo)) != 0) {
		zlog(ZLOG_ERROR, "getaddrinfo: %s", gai_strerror(status));
		return -1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		inet_ntop(p->ai_family, fpm_get_in_addr(p->ai_addr), tmpbuf, INET6_ADDRSTRLEN);
		if (sock < 0) {
			if ((sock = fpm_sockets_get_listening_socket(wp, p->ai_addr, p->ai_addrlen)) != -1) {
				zlog(ZLOG_DEBUG, "Found address for %s, socket opened on %s", addr, tmpbuf);
			}
		} else {
			zlog(ZLOG_WARNING, "Found multiple addresses for %s, %s ignored", addr, tmpbuf);
		}
	}

	freeaddrinfo(servinfo);

	return sock;
}
/* }}} */

static int fpm_socket_af_inet_listening_socket(struct fpm_worker_pool_s *wp) /* {{{ */
{
	char *dup_address = strdup(wp->config->listen_address);
	char *port_str = strrchr(dup_address, ':');
	char *addr = NULL;
	int addr_len;
	int port = 0;
	int sock = -1;

	if (port_str) { /* this is host:port pair */
		*port_str++ = '\0';
		port = atoi(port_str);
		addr = dup_address;

		/* strip brackets from address for getaddrinfo */
		addr_len = strlen(addr);
		if (addr[0] == '[' && addr[addr_len - 1] == ']') {
			addr[addr_len - 1] = '\0';
			addr++;
		}

	} else if (strlen(dup_address) == strspn(dup_address, "0123456789")) { /* this is port */
		port = atoi(dup_address);
		port_str = dup_address;
	}

	if (port < 1 || port > 65535) {
		zlog(ZLOG_ERROR, "invalid port value '%s'", port_str);
		free(dup_address);
		return -1;
	}

	if (addr) {
		/* Bind a specific address */
		sock = fpm_socket_af_inet_socket_by_addr(wp, addr, port_str);
	} else {
		/* Bind ANYADDR
		 *
		 * Try "::" first as that covers IPv6 ANYADDR and mapped IPv4 ANYADDR
		 * silencing warnings since failure is an option
		 *
		 * If that fails (because AF_INET6 is unsupported) retry with 0.0.0.0
		 */
		int old_level = zlog_set_level(ZLOG_ALERT);
		sock = fpm_socket_af_inet_socket_by_addr(wp, "::", port_str);
		zlog_set_level(old_level);

		if (sock < 0) {
			zlog(ZLOG_NOTICE, "Failed implicitly binding to ::, retrying with 0.0.0.0");
			sock = fpm_socket_af_inet_socket_by_addr(wp, "0.0.0.0", port_str);
		}
	}

	free(dup_address);

	return sock;
}
/* }}} */

static int fpm_socket_af_unix_listening_socket(struct fpm_worker_pool_s *wp) /* {{{ */
{
	struct sockaddr_un sa_un;
    size_t socket_length = sizeof(sa_un.sun_path);
    size_t address_length = strlen(wp->config->listen_address);

	memset(&sa_un, 0, sizeof(sa_un));
	strlcpy(sa_un.sun_path, wp->config->listen_address, socket_length);

    if (address_length >= socket_length) {
        zlog(
            ZLOG_WARNING,
            "[pool %s] cannot bind to UNIX socket '%s' as path is too long (found length: %zu, "
				"maximal length: %zu), trying cut socket path instead '%s'",
            wp->config->name,
			wp->config->listen_address,
			address_length,
			socket_length,
			sa_un.sun_path
        );
    }

	sa_un.sun_family = AF_UNIX;
	return fpm_sockets_get_listening_socket(wp, (struct sockaddr *) &sa_un, sizeof(struct sockaddr_un));
}
/* }}} */

#ifdef SO_SETFIB
static zend_result fpm_socket_setfib_init(void)
{
	/* potentially up to 65536 but needs to check the actual cap beforehand */
	size_t len = sizeof(routemax);
	if (sysctlbyname("net.fibs", &routemax, &len, NULL, 0) < 0) {
		zlog(ZLOG_ERROR, "failed to get max routing table");
		return FAILURE;
	}

	return SUCCESS;
}
#endif

int fpm_sockets_init_main(void)
{
	unsigned i, lq_len;
	struct fpm_worker_pool_s *wp;
	char envname[32];
	char sockpath[256];
	char *inherited;
	struct listening_socket_s *ls;

	if (0 == fpm_array_init(&sockets_list, sizeof(struct listening_socket_s), 10)) {
		return -1;
	}

#ifdef SO_SETFIB
	if (fpm_socket_setfib_init() == FAILURE) {
		return -1;
	}
#endif

	/* import inherited sockets */
	for (i = 0; i < FPM_ENV_SOCKET_SET_MAX; i++) {
		fpm_sockets_get_env_name(envname, i);
		inherited = getenv(envname);
		if (!inherited) {
			break;
		}

		while (inherited && *inherited) {
			char *comma = strchr(inherited, ',');
			int type, fd_no;
			char *eq;

			if (comma) {
				*comma = '\0';
			}

			eq = strchr(inherited, '=');
			if (eq) {
				int sockpath_len = eq - inherited;
				if (sockpath_len > 255) {
					/* this should never happen as UDS limit is lower */
					sockpath_len = 255;
				}
				memcpy(sockpath, inherited, sockpath_len);
				sockpath[sockpath_len] = '\0';
				fd_no = atoi(eq + 1);
				type = fpm_sockets_domain_from_address(sockpath);
				zlog(ZLOG_NOTICE, "using inherited socket fd=%d, \"%s\"", fd_no, sockpath);
				fpm_sockets_hash_op(fd_no, 0, sockpath, type, FPM_STORE_SOCKET);
			}

			if (comma) {
				inherited = comma + 1;
			} else {
				inherited = 0;
			}
		}
	}

	/* create all required sockets */
	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		switch (wp->listen_address_domain) {
			case FPM_AF_INET :
				wp->listening_socket = fpm_socket_af_inet_listening_socket(wp);
				break;

			case FPM_AF_UNIX :
				if (0 > fpm_unix_resolve_socket_permissions(wp)) {
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
#if defined(__FreeBSD__) || defined(__NetBSD__)
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

#elif defined(HAVE_LQ_TCP_CONNECTION_INFO)

#include <netinet/tcp.h>

int fpm_socket_get_listening_queue(int sock, unsigned *cur_lq, unsigned *max_lq)
{
	struct tcp_connection_info info;
	socklen_t len = sizeof(info);

	if (0 > getsockopt(sock, IPPROTO_TCP, TCP_CONNECTION_INFO, &info, &len)) {
		zlog(ZLOG_SYSERROR, "failed to retrieve TCP_CONNECTION_INFO for socket");
		return -1;
	}

	if (cur_lq) {
		*cur_lq = info.tcpi_tfo_syn_data_acked;
	}

	if (max_lq) {
		*max_lq = 0;
	}

	return 0;
}

#elif defined(HAVE_LQ_SO_LISTENQ)

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
