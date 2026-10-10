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

#ifdef UCACHE_USE_SHM

#if defined(__FreeBSD__)
# include <machine/param.h>
#endif
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdlib.h>

static int ucache_alloc_shm_create_seg(size_t requested_size, ucache_shm_seg **shared_seg_p, const char **err_in);
static void ucache_alloc_shm_detach_seg(ucache_shm_seg *shared_seg);

const ucache_shm_handlers ucache_alloc_shm_handlers = {
	ucache_alloc_shm_create_seg,
	ucache_alloc_shm_detach_seg
};

static int ucache_alloc_shm_create_seg(size_t requested_size, ucache_shm_seg **shared_seg_p, const char **err_in)
{
	struct shmid_ds sds;
	ucache_shm_seg *shared_seg;
	int shmget_flags, seg_id;

	shmget_flags = IPC_CREAT | SHM_R | SHM_W | IPC_EXCL;

	seg_id = shmget(IPC_PRIVATE, requested_size, shmget_flags);
	if (seg_id == -1) {
		*err_in = "shmget";

		return UCACHE_ALLOC_FAILURE;
	}

	shared_seg = (ucache_shm_seg *) calloc(1, sizeof(*shared_seg));
	if (shared_seg == NULL) {
		shmctl(seg_id, IPC_RMID, &sds);

		*err_in = "calloc";

		return UCACHE_ALLOC_FAILURE;
	}

	shared_seg->p = shmat(seg_id, NULL, 0);
	if (shared_seg->p == (void *) -1) {
		shmctl(seg_id, IPC_RMID, &sds);
		free(shared_seg);

		*err_in = "shmat";

		return UCACHE_ALLOC_FAILURE;
	}

	shmctl(seg_id, IPC_RMID, &sds);

	shared_seg->size = requested_size;
	shared_seg->zero_filled = true;
	*shared_seg_p = shared_seg;

	return UCACHE_ALLOC_SUCCESS;
}

static void ucache_alloc_shm_detach_seg(ucache_shm_seg *shared_seg)
{
	shmdt(shared_seg->p);
}

#endif /* UCACHE_USE_SHM */
