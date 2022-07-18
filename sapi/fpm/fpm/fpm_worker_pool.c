	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "fpm.h"
#include "fpm_worker_pool.h"
#include "fpm_cleanup.h"
#include "fpm_children.h"
#include "fpm_shm.h"
#include "fpm_scoreboard.h"
#include "fpm_conf.h"
#include "fpm_unix.h"

struct fpm_worker_pool_s *fpm_worker_all_pools;

void fpm_worker_pool_free_limit_extensions(char **limit_extensions) {
	char **ext = limit_extensions;
	while (*ext) {
		free(*ext);
		ext++;
	}
	free(limit_extensions);
}

void fpm_worker_pool_free(struct fpm_worker_pool_s *wp) /* {{{ */
{
	if (wp->config) {
		free(wp->config);
	}
	if (wp->user) {
		free(wp->user);
	}
	if (wp->home) {
		free(wp->home);
	}
	if (wp->limit_extensions) {
		fpm_worker_pool_free_limit_extensions(wp->limit_extensions);
	}
	fpm_unix_free_socket_premissions(wp);
	free(wp);
}
/* }}} */

static void fpm_worker_pool_cleanup(int which, void *arg) /* {{{ */
{
	struct fpm_worker_pool_s *wp, *wp_next;

	for (wp = fpm_worker_all_pools; wp; wp = wp_next) {
		wp_next = wp->next;
		fpm_worker_pool_config_free(wp->config);
		fpm_children_free(wp->children);
		if ((which & FPM_CLEANUP_CHILD) == 0 && fpm_globals.parent_pid == getpid()) {
			fpm_scoreboard_free(wp);
		}
		fpm_worker_pool_free(wp);
	}
	fpm_worker_all_pools = NULL;
}
/* }}} */

struct fpm_worker_pool_s *fpm_worker_pool_alloc(void)
{
	struct fpm_worker_pool_s *ret;

	ret = malloc(sizeof(struct fpm_worker_pool_s));
	if (!ret) {
		return 0;
	}

	memset(ret, 0, sizeof(struct fpm_worker_pool_s));

	ret->idle_spawn_rate = 1;
	ret->log_fd = -1;
	return ret;
}

int fpm_worker_pool_init_main(void)
{
	if (0 > fpm_cleanup_add(FPM_CLEANUP_ALL, fpm_worker_pool_cleanup, 0)) {
		return -1;
	}
	return 0;
}
