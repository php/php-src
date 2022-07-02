/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend_shared_alloc.h"

#ifdef USE_SHM

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

#ifndef MIN
# define MIN(x, y) ((x) > (y)? (y) : (x))
#endif

#define SEG_ALLOC_SIZE_MAX 32*1024*1024
#define SEG_ALLOC_SIZE_MIN 2*1024*1024

typedef struct  {
    zend_shared_segment common;
    int shm_id;
} zend_shared_segment_shm;

static int create_segments(size_t requested_size, zend_shared_segment_shm ***shared_segments_p, int *shared_segments_count, char **error_in)
{
	int i;
	size_t allocate_size = 0, remaining_bytes = requested_size, seg_allocate_size;
	int first_segment_id = -1;
	key_t first_segment_key = -1;
	struct shmid_ds sds;
	int shmget_flags;
	zend_shared_segment_shm *shared_segments;

	seg_allocate_size = SEG_ALLOC_SIZE_MAX;
	/* determine segment size we _really_ need:
	 * no more than to include requested_size
	 */
	while (requested_size * 2 <= seg_allocate_size && seg_allocate_size > SEG_ALLOC_SIZE_MIN) {
		seg_allocate_size >>= 1;
	}

	shmget_flags = IPC_CREAT|SHM_R|SHM_W|IPC_EXCL;

	/* try allocating this much, if not - try shrinking */
	while (seg_allocate_size >= SEG_ALLOC_SIZE_MIN) {
		allocate_size = MIN(requested_size, seg_allocate_size);
		first_segment_id = shmget(first_segment_key, allocate_size, shmget_flags);
		if (first_segment_id != -1) {
			break;
		}
		seg_allocate_size >>= 1; /* shrink the allocated block */
	}

	if (first_segment_id == -1) {
		*error_in = "shmget";
		return ALLOC_FAILURE;
	}

	*shared_segments_count = ((requested_size - 1) / seg_allocate_size) + 1;
	*shared_segments_p = (zend_shared_segment_shm **) calloc(1, (*shared_segments_count) * sizeof(zend_shared_segment_shm) + sizeof(void *) * (*shared_segments_count));
	if (!*shared_segments_p) {
		*error_in = "calloc";
		return ALLOC_FAILURE;
	}
	shared_segments = (zend_shared_segment_shm *)((char *)(*shared_segments_p) + sizeof(void *) * (*shared_segments_count));
	for (i = 0; i < *shared_segments_count; i++) {
		(*shared_segments_p)[i] = shared_segments + i;
	}

	remaining_bytes = requested_size;
	for (i = 0; i < *shared_segments_count; i++) {
		allocate_size = MIN(remaining_bytes, seg_allocate_size);
		if (i != 0) {
			shared_segments[i].shm_id = shmget(IPC_PRIVATE, allocate_size, shmget_flags);
		} else {
			shared_segments[i].shm_id = first_segment_id;
		}

		if (shared_segments[i].shm_id == -1) {
			return ALLOC_FAILURE;
		}

		shared_segments[i].common.p = shmat(shared_segments[i].shm_id, NULL, 0);
		if (shared_segments[i].common.p == (void *)-1) {
			*error_in = "shmat";
			shmctl(shared_segments[i].shm_id, IPC_RMID, &sds);
			return ALLOC_FAILURE;
		}
		shmctl(shared_segments[i].shm_id, IPC_RMID, &sds);

		shared_segments[i].common.pos = 0;
		shared_segments[i].common.size = allocate_size;
		remaining_bytes -= allocate_size;
	}
	return ALLOC_SUCCESS;
}

static int detach_segment(zend_shared_segment_shm *shared_segment)
{
	shmdt(shared_segment->common.p);
	return 0;
}

static size_t segment_type_size(void)
{
	return sizeof(zend_shared_segment_shm);
}

zend_shared_memory_handlers zend_alloc_shm_handlers = {
	(create_segments_t)create_segments,
	(detach_segment_t)detach_segment,
	segment_type_size
};

#endif /* USE_SHM */
