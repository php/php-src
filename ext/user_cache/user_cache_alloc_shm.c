/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Go Kudo <zeriyoshi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#include "user_cache_shm.h"

#ifdef PHP_UCACHE_USE_SHM

#if defined(__FreeBSD__)
# include <machine/param.h>
#endif
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <sys/stat.h>
#include <fcntl.h>

typedef struct {
	php_ucache_shm_segment_t common;
	int shm_id;
} php_ucache_shm_segment_shm_t;

static bool ucache_alloc_shm_create_segments(size_t requested_size, php_ucache_shm_segment_shm_t ***shared_segments_p, uint32_t *shared_segments_count, const char **error_in)
{
	struct shmid_ds sds;
	php_ucache_shm_segment_shm_t *shared_segments;
	int shmget_flags, segment_id;

	shmget_flags = IPC_CREAT | SHM_R | SHM_W | IPC_EXCL;

	/* The storage layer accepts a single segment only, so a smaller
	 * multi-segment fallback would just churn SysV ids and fail later. */
	segment_id = shmget(IPC_PRIVATE, requested_size, shmget_flags);
	if (UNEXPECTED(segment_id == -1)) {
		*error_in = "shmget";

		return false;
	}

	*shared_segments_count = 1;
	*shared_segments_p = (php_ucache_shm_segment_shm_t **) pecalloc(1, sizeof(php_ucache_shm_segment_shm_t) + sizeof(void *), true);

	shared_segments = (php_ucache_shm_segment_shm_t *)((char *)(*shared_segments_p) + sizeof(void *));
	(*shared_segments_p)[0] = shared_segments;

	shared_segments->shm_id = segment_id;
	shared_segments->common.p = shmat(segment_id, NULL, 0);
	if (shared_segments->common.p == (void *)-1) {
		*error_in = "shmat";
		shmctl(segment_id, IPC_RMID, &sds);

		return false;
	}

	shmctl(segment_id, IPC_RMID, &sds);

	shared_segments->common.size = requested_size;

	return true;
}

static void ucache_alloc_shm_detach_segment(php_ucache_shm_segment_shm_t *shared_segment)
{
	shmdt(shared_segment->common.p);
}

const php_ucache_shm_handlers_t php_ucache_alloc_shm_handlers = {
	(php_ucache_create_segments_t)ucache_alloc_shm_create_segments,
	(php_ucache_detach_segment_t)ucache_alloc_shm_detach_segment
};

#endif /* PHP_UCACHE_USE_SHM */
