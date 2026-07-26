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

#ifdef PHP_USER_CACHE_USE_SHM

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
	php_user_cache_shm_segment common;
	int shm_id;
} php_user_cache_shm_segment_shm;

static int user_cache_alloc_shm_create_segments(size_t requested_size, php_user_cache_shm_segment_shm ***shared_segments_p, int *shared_segments_count, const char **error_in)
{
	php_user_cache_shm_segment_shm *shared_segments;
	struct shmid_ds sds;
	int shmget_flags, segment_id;

	shmget_flags = IPC_CREAT | SHM_R | SHM_W | IPC_EXCL;

	/* The storage layer accepts a single segment only, so a smaller
	 * multi-segment fallback would just churn SysV ids and fail later. */
	segment_id = shmget(IPC_PRIVATE, requested_size, shmget_flags);
	if (UNEXPECTED(segment_id == -1)) {
		*error_in = "shmget";

		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	*shared_segments_count = 1;
	*shared_segments_p = (php_user_cache_shm_segment_shm **) calloc(1, sizeof(php_user_cache_shm_segment_shm) + sizeof(void *));
	if (!*shared_segments_p) {
		*error_in = "calloc";
		/* Nothing is attached yet, so IPC_RMID destroys the segment
		 * immediately instead of leaking it. */
		shmctl(segment_id, IPC_RMID, &sds);

		return PHP_USER_CACHE_ALLOC_FAILURE;
	}
	shared_segments = (php_user_cache_shm_segment_shm *)((char *)(*shared_segments_p) + sizeof(void *));
	(*shared_segments_p)[0] = shared_segments;

	shared_segments->shm_id = segment_id;
	shared_segments->common.p = shmat(segment_id, NULL, 0);
	if (shared_segments->common.p == (void *)-1) {
		*error_in = "shmat";
		shmctl(segment_id, IPC_RMID, &sds);

		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	shmctl(segment_id, IPC_RMID, &sds);

	shared_segments->common.size = requested_size;

	return PHP_USER_CACHE_ALLOC_SUCCESS;
}

static int user_cache_alloc_shm_detach_segment(php_user_cache_shm_segment_shm *shared_segment)
{
	shmdt(shared_segment->common.p);

	return 0;
}

const php_user_cache_shm_handlers php_user_cache_alloc_shm_handlers = {
	(php_user_cache_create_segments_t)user_cache_alloc_shm_create_segments,
	(php_user_cache_detach_segment_t)user_cache_alloc_shm_detach_segment
};

#endif /* PHP_USER_CACHE_USE_SHM */
