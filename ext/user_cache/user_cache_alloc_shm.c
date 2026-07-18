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
   | Author: Go Kudo <zeriyoshi@php.net>.                                 |
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

#define PHP_USER_CACHE_MIN(x, y) ((x) > (y) ? (y) : (x))

#define PHP_USER_CACHE_SEG_ALLOC_SIZE_MIN (2 * 1024 * 1024)

typedef struct {
	php_user_cache_shm_segment common;
	int shm_id;
} php_user_cache_shm_segment_shm;

static int user_cache_alloc_shm_create_segments(size_t requested_size, php_user_cache_shm_segment_shm ***shared_segments_p, int *shared_segments_count, const char **error_in)
{
	int i;
	size_t allocate_size = 0, remaining_bytes, seg_allocate_size;
	int first_segment_id = -1;
	key_t first_segment_key = -1;
	struct shmid_ds sds;
	int shmget_flags;
	php_user_cache_shm_segment_shm *shared_segments;

	shmget_flags = IPC_CREAT | SHM_R | SHM_W | IPC_EXCL;

	seg_allocate_size = requested_size;
	first_segment_id = shmget(first_segment_key, seg_allocate_size, shmget_flags);
	if (UNEXPECTED(first_segment_id == -1)) {
		/* Fall back to decreasing power-of-two segments. */
		seg_allocate_size = PHP_USER_CACHE_SEG_ALLOC_SIZE_MIN;
		while (seg_allocate_size < requested_size / 2) {
			seg_allocate_size *= 2;
		}

		while (seg_allocate_size >= PHP_USER_CACHE_SEG_ALLOC_SIZE_MIN) {
			first_segment_id = shmget(first_segment_key, seg_allocate_size, shmget_flags);
			if (first_segment_id != -1) {
				break;
			}
			seg_allocate_size >>= 1;
		}

		if (first_segment_id == -1) {
			*error_in = "shmget";
			return PHP_USER_CACHE_ALLOC_FAILURE;
		}
	}

	*shared_segments_count = ((requested_size - 1) / seg_allocate_size) + 1;
	*shared_segments_p = (php_user_cache_shm_segment_shm **) calloc(1, (*shared_segments_count) * sizeof(php_user_cache_shm_segment_shm) + sizeof(void *) * (*shared_segments_count));
	if (!*shared_segments_p) {
		*error_in = "calloc";
		/* Not yet attached, so IPC_RMID both marks and destroys it; without
		 * this the segment would outlive the process until ipcrm/reboot. */
		shmctl(first_segment_id, IPC_RMID, &sds);
		return PHP_USER_CACHE_ALLOC_FAILURE;
	}
	shared_segments = (php_user_cache_shm_segment_shm *)((char *)(*shared_segments_p) + sizeof(void *) * (*shared_segments_count));
	for (i = 0; i < *shared_segments_count; i++) {
		(*shared_segments_p)[i] = shared_segments + i;
	}

	remaining_bytes = requested_size;
	for (i = 0; i < *shared_segments_count; i++) {
		allocate_size = PHP_USER_CACHE_MIN(remaining_bytes, seg_allocate_size);
		if (i != 0) {
			shared_segments[i].shm_id = shmget(IPC_PRIVATE, allocate_size, shmget_flags);
		} else {
			shared_segments[i].shm_id = first_segment_id;
		}

		if (shared_segments[i].shm_id == -1) {
			*error_in = "shmget";
			return PHP_USER_CACHE_ALLOC_FAILURE;
		}

		shared_segments[i].common.p = shmat(shared_segments[i].shm_id, NULL, 0);
		if (shared_segments[i].common.p == (void *)-1) {
			*error_in = "shmat";
			shmctl(shared_segments[i].shm_id, IPC_RMID, &sds);
			return PHP_USER_CACHE_ALLOC_FAILURE;
		}
		shmctl(shared_segments[i].shm_id, IPC_RMID, &sds);

		shared_segments[i].common.pos = 0;
		shared_segments[i].common.size = allocate_size;
		remaining_bytes -= allocate_size;
	}
	return PHP_USER_CACHE_ALLOC_SUCCESS;
}

static int user_cache_alloc_shm_detach_segment(php_user_cache_shm_segment_shm *shared_segment)
{
	shmdt(shared_segment->common.p);
	return 0;
}

static size_t user_cache_alloc_shm_segment_type_size(void)
{
	return sizeof(php_user_cache_shm_segment_shm);
}

const php_user_cache_shm_handlers php_user_cache_alloc_shm_handlers = {
	(php_user_cache_create_segments_t)user_cache_alloc_shm_create_segments,
	(php_user_cache_detach_segment_t)user_cache_alloc_shm_detach_segment,
	user_cache_alloc_shm_segment_type_size
};

#endif /* PHP_USER_CACHE_USE_SHM */
