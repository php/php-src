
	/* $Id: fpm_children.h,v 1.9 2008/05/24 17:38:47 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_CHILDREN_H
#define FPM_CHILDREN_H 1

#include <sys/time.h>
#include <sys/types.h>

#include "fpm_worker_pool.h"
#include "fpm_events.h"

int fpm_children_create_initial(struct fpm_worker_pool_s *wp);
int fpm_children_free(struct fpm_child_s *child);
void fpm_children_bury();
int fpm_children_init_main();
int fpm_children_make(struct fpm_worker_pool_s *wp, int in_event_loop, int nb_to_spawn, int is_debug);

struct fpm_child_s;

struct fpm_child_s {
	struct fpm_child_s *prev, *next;
	struct timeval started;
	struct fpm_worker_pool_s *wp;
	struct fpm_event_s ev_stdout, ev_stderr;
	int shm_slot_i;
	int fd_stdout, fd_stderr;
	void (*tracer)(struct fpm_child_s *);
	struct timeval slow_logged;
	int idle_kill;
	pid_t pid;
	int scoreboard_i;
};

#endif
