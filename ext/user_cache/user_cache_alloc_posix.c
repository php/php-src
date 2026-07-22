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

#ifdef PHP_USER_CACHE_USE_SHM_OPEN

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

typedef struct {
	php_user_cache_shm_segment common;
	int shm_fd;
} php_user_cache_shm_segment_posix;

static int user_cache_alloc_posix_create_segments(size_t requested_size, php_user_cache_shm_segment_posix ***shared_segments_p, int *shared_segments_count, const char **error_in)
{
	php_user_cache_shm_segment_posix *shared_segment;
	char shared_segment_name[sizeof("/php_user_cache.") + 20];
	int shared_segment_flags = O_RDWR | O_CREAT | O_TRUNC;
	mode_t shared_segment_mode = 0600;

#if defined(HAVE_SHM_CREATE_LARGEPAGE)
	/* Prefer the largest compatible page size. */
	size_t i, shared_segment_sizes = 0, shared_segment_lg_index = 0;
	size_t shared_segment_sindexes[3] = {0};
	const size_t entries = sizeof(shared_segment_sindexes) / sizeof(shared_segment_sindexes[0]);

	shared_segment_sizes = getpagesizes(shared_segment_sindexes, entries);

	if (shared_segment_sizes > 0) {
		for (i = shared_segment_sizes; i-- > 0; ) {
			if (shared_segment_sindexes[i] != 0 &&
			    !(requested_size % shared_segment_sindexes[i])) {
				shared_segment_lg_index = i;
				break;
			}
		}
	}
#endif /* HAVE_SHM_CREATE_LARGEPAGE */

	*shared_segments_count = 1;
	*shared_segments_p = (php_user_cache_shm_segment_posix **) calloc(1, sizeof(php_user_cache_shm_segment_posix) + sizeof(void *));
	if (!*shared_segments_p) {
		*error_in = "calloc";
		return PHP_USER_CACHE_ALLOC_FAILURE;
	}
	shared_segment = (php_user_cache_shm_segment_posix *)((char *)(*shared_segments_p) + sizeof(void *));
	(*shared_segments_p)[0] = shared_segment;

	snprintf(shared_segment_name, sizeof(shared_segment_name), "/php_user_cache.%d", getpid());
#if defined(HAVE_SHM_CREATE_LARGEPAGE)
	if (shared_segment_lg_index > 0) {
		shared_segment->shm_fd =  shm_create_largepage(shared_segment_name, shared_segment_flags, shared_segment_lg_index, SHM_LARGEPAGE_ALLOC_DEFAULT, shared_segment_mode);
		if (shared_segment->shm_fd != -1) {
			goto truncate_segment;
		}
	}
#endif /* HAVE_SHM_CREATE_LARGEPAGE */

	shared_segment->shm_fd = shm_open(shared_segment_name, shared_segment_flags, shared_segment_mode);
	if (shared_segment->shm_fd == -1) {
		*error_in = "shm_open";
		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

#if defined(HAVE_SHM_CREATE_LARGEPAGE)
truncate_segment:
#endif /* HAVE_SHM_CREATE_LARGEPAGE */
	if (ftruncate(shared_segment->shm_fd, requested_size) != 0) {
		*error_in = "ftruncate";
		close(shared_segment->shm_fd);
		shm_unlink(shared_segment_name);

		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	shared_segment->common.p = mmap(0, requested_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_segment->shm_fd, 0);
	if (shared_segment->common.p == MAP_FAILED) {
		*error_in = "mmap";
		close(shared_segment->shm_fd);
		shm_unlink(shared_segment_name);

		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	shm_unlink(shared_segment_name);

	shared_segment->common.size = requested_size;

	return PHP_USER_CACHE_ALLOC_SUCCESS;
}

static int user_cache_alloc_posix_detach_segment(php_user_cache_shm_segment_posix *shared_segment)
{
	munmap(shared_segment->common.p, shared_segment->common.size);
	close(shared_segment->shm_fd);

	return 0;
}

static size_t user_cache_alloc_posix_segment_type_size(void)
{
	return sizeof(php_user_cache_shm_segment_posix);
}

const php_user_cache_shm_handlers php_user_cache_alloc_posix_handlers = {
	(php_user_cache_create_segments_t)user_cache_alloc_posix_create_segments,
	(php_user_cache_detach_segment_t)user_cache_alloc_posix_detach_segment,
	user_cache_alloc_posix_segment_type_size
};

#endif /* PHP_USER_CACHE_USE_SHM_OPEN */
