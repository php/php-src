	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_MISC_H
#define FPM_MISC_H 1

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

#include "fpm_worker_pool.h"

/*
  On FreeBSD and OpenBSD, backlog negative values are truncated to SOMAXCONN
*/
#if defined(__FreeBSD__) || defined(__OpenBSD__)
#define FPM_BACKLOG_DEFAULT -1
#else
#define FPM_BACKLOG_DEFAULT 511
#endif

#define FPM_ENV_SOCKET_SET_MAX 256
#define FPM_ENV_SOCKET_SET_SIZE 128

enum fpm_address_domain fpm_sockets_domain_from_address(char *addr);
int fpm_sockets_init_main(void);
int fpm_socket_get_listening_queue(int sock, unsigned *cur_lq, unsigned *max_lq);
int fpm_socket_unix_test_connect(struct sockaddr_un *sock, size_t socklen);


static inline int fd_set_blocked(int fd, int blocked) /* {{{ */
{
	int flags = fcntl(fd, F_GETFL);

	if (flags < 0) {
		return -1;
	}

	if (blocked) {
		flags &= ~O_NONBLOCK;
	} else {
		flags |= O_NONBLOCK;
	}
	return fcntl(fd, F_SETFL, flags);
}
/* }}} */

#endif
