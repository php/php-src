
	/* $Id: fpm.h,v 1.13 2008/05/24 17:38:47 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_H
#define FPM_H 1

#include <unistd.h>
#include <sys/types.h> /* for event.h below */
#include <event.h>

int fpm_run(int *max_requests, struct event_base *base);
int fpm_init(int argc, char **argv, char *config, struct event_base **base);

struct fpm_globals_s {
	pid_t parent_pid;
	int argc;
	char **argv;
	char *config;
	int running_children;
	int error_log_fd;
	int log_level;
	int listening_socket; /* for this child */
	int max_requests; /* for this child */
	int is_child;
};

extern struct fpm_globals_s fpm_globals;

#endif
