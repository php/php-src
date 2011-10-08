
	/* $Id: fpm_worker_pool.h,v 1.13 2008/08/26 15:09:15 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_WORKER_POOL_H
#define FPM_WORKER_POOL_H 1

#include "fpm_conf.h"
#include "fpm_shm.h"

struct fpm_worker_pool_s;
struct fpm_child_s;
struct fpm_child_stat_s;
struct fpm_shm_s;

enum fpm_address_domain {
	FPM_AF_UNIX = 1,
	FPM_AF_INET = 2
};

struct fpm_worker_pool_s {
	struct fpm_worker_pool_s *next;
	struct fpm_worker_pool_config_s *config;
	char *user, *home;									/* for setting env USER and HOME */
	enum fpm_address_domain listen_address_domain;
	int listening_socket;
	int set_uid, set_gid;								/* config uid and gid */
	int socket_uid, socket_gid, socket_mode;

	/* runtime */
	struct fpm_child_s *children;
	int running_children;
	int idle_spawn_rate;
	int warn_max_children;
#if 0
	int warn_lq;
#endif
	struct fpm_scoreboard_s *scoreboard;
	int log_fd;
	char **limit_extensions;

	/* for ondemand PM */
	struct fpm_event_s *ondemand_event;
	int socket_event_set;
};

struct fpm_worker_pool_s *fpm_worker_pool_alloc();
int fpm_worker_pool_init_main();

extern struct fpm_worker_pool_s *fpm_worker_all_pools;

#endif

