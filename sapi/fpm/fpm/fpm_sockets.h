
	/* $Id: fpm_sockets.h,v 1.12 2008/08/26 15:09:15 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_MISC_H
#define FPM_MISC_H 1

#include <unistd.h>
#include <fcntl.h>

#include "fpm_worker_pool.h"

enum fpm_address_domain fpm_sockets_domain_from_address(char *addr);
int fpm_sockets_init_main();


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

#define IPQUAD(sin_addr) \
			(unsigned int) ((unsigned char *) &(sin_addr)->s_addr)[0], \
			(unsigned int) ((unsigned char *) &(sin_addr)->s_addr)[1], \
			(unsigned int) ((unsigned char *) &(sin_addr)->s_addr)[2], \
			(unsigned int) ((unsigned char *) &(sin_addr)->s_addr)[3]

#endif
