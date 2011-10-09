
	/* $Id: fpm.h,v 1.13 2008/05/24 17:38:47 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_H
#define FPM_H 1

#include <unistd.h>

int fpm_run(int *max_requests);
int fpm_init(int argc, char **argv, char *config, char *prefix, char *pid, int test_conf);

struct fpm_globals_s {
	pid_t parent_pid;
	int argc;
	char **argv;
	char *config;
	char *prefix;
	char *pid;
	int running_children;
	int error_log_fd;
	int log_level;
	int listening_socket; /* for this child */
	int max_requests; /* for this child */
	int is_child;
	int test_successful;
	int heartbeat;
};

extern struct fpm_globals_s fpm_globals;

#endif
