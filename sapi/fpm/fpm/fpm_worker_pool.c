
	/* $Id: fpm_worker_pool.c,v 1.15.2.1 2008/12/13 03:21:18 anight Exp $ */
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
#include "fpm_shm_slots.h"
#include "fpm_conf.h"

struct fpm_worker_pool_s *fpm_worker_all_pools;

static void fpm_worker_pool_cleanup(int which, void *arg) /* {{{ */
{
	struct fpm_worker_pool_s *wp, *wp_next;

	for (wp = fpm_worker_all_pools; wp; wp = wp_next) {
		wp_next = wp->next;
		fpm_worker_pool_config_free(wp->config);
		fpm_children_free(wp->children);
		fpm_array_free(&wp->slots_used);
		fpm_array_free(&wp->slots_free);
		fpm_shm_free_list(wp->shm_list, which == FPM_CLEANUP_CHILD ? fpm_shm_slots_mem() : 0);
		if (wp->shm_status && which != FPM_CLEANUP_CHILD) {
			fpm_shm_free(wp->shm_status, !fpm_globals.is_child);
		}
		free(wp->config);
		free(wp->user);
		free(wp->home);
		free(wp);
	}
	fpm_worker_all_pools = 0;
}
/* }}} */

struct fpm_worker_pool_s *fpm_worker_pool_alloc() /* {{{ */
{
	struct fpm_worker_pool_s *ret;

	ret = malloc(sizeof(struct fpm_worker_pool_s));
	if (!ret) {
		return 0;
	}

	memset(ret, 0, sizeof(struct fpm_worker_pool_s));

	fpm_array_init(&ret->slots_used, sizeof(struct fpm_shm_slot_ptr_s), 50);
	fpm_array_init(&ret->slots_free, sizeof(struct fpm_shm_slot_ptr_s), 50);
	ret->idle_spawn_rate = 1;
	return ret;
}
/* }}} */

int fpm_worker_pool_init_main() /* {{{ */
{
	if (0 > fpm_cleanup_add(FPM_CLEANUP_ALL, fpm_worker_pool_cleanup, 0)) {
		return -1;
	}
	return 0;
}
/* }}} */
