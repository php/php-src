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

#ifdef PHP_UCACHE_USE_SHM_OPEN

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "ext/random/php_random_csprng.h"

#define PHP_UCACHE_POSIX_SHM_NAME_PREFIX   "/php_uc."
#define PHP_UCACHE_POSIX_SHM_NAME_BYTES    10
#define PHP_UCACHE_POSIX_SHM_NAME_ATTEMPTS 5

typedef struct {
	php_ucache_shm_segment_t common;
	int shm_fd;
} php_ucache_shm_segment_posix_t;

static bool ucache_alloc_posix_segment_name(char *buf, size_t buf_size)
{
	static const char hexits[] = "0123456789abcdef";
	size_t i;
	unsigned char random_bytes[PHP_UCACHE_POSIX_SHM_NAME_BYTES];
	char hex[sizeof(random_bytes) * 2 + 1];

	if (php_random_bytes_silent(random_bytes, sizeof(random_bytes)) == FAILURE) {
		return false;
	}

	for (i = 0; i < sizeof(random_bytes); i++) {
		hex[i * 2] = hexits[random_bytes[i] >> 4];
		hex[(i * 2) + 1] = hexits[random_bytes[i] & 0xf];
	}

	hex[sizeof(hex) - 1] = '\0';

	snprintf(buf, buf_size, PHP_UCACHE_POSIX_SHM_NAME_PREFIX "%s", hex);

	return true;
}

static bool ucache_alloc_posix_create_segments(size_t requested_size, php_ucache_shm_segment_posix_t ***shared_segments_p, uint32_t *shared_segments_count, const char **error_in)
{
	php_ucache_shm_segment_posix_t *shared_segment;
	mode_t shared_segment_mode = 0600;
	uint32_t shared_segment_attempt;
	/* O_EXCL: never adopt an object somebody else created under this name. */
	int shared_segment_flags = O_RDWR | O_CREAT | O_EXCL, shared_segment_fd = -1;
	char shared_segment_name[sizeof(PHP_UCACHE_POSIX_SHM_NAME_PREFIX) + (PHP_UCACHE_POSIX_SHM_NAME_BYTES * 2)];

#if defined(HAVE_SHM_CREATE_LARGEPAGE)
	/* Prefer the largest compatible page size. Capture the getpagesizes()
	 * result as a signed int: its -1 error return in a size_t would pass
	 * the > 0 guard and index far outside the array (see the equivalent
	 * upstream fix in ext/opcache/shared_alloc_posix.c, GH-22429). */
	const size_t entries = sizeof(shared_segments_indexes) / sizeof(shared_segments_indexes[0]);
	size_t shared_segment_lg_index = 0, shared_segments_indexes[3] = {0};
	int i, shared_segment_sizes;

	shared_segment_sizes = getpagesizes(shared_segments_indexes, entries);

	if (shared_segment_sizes > 0) {
		for (i = shared_segment_sizes; i-- > 0; ) {
			if (shared_segments_indexes[i] != 0 &&
				!(requested_size % shared_segments_indexes[i])
			) {
				shared_segment_lg_index = i;

				break;
			}
		}
	}
#endif /* defined(HAVE_SHM_CREATE_LARGEPAGE) */

	*shared_segments_count = 1;
	*shared_segments_p = (php_ucache_shm_segment_posix_t **) pecalloc(1, sizeof(php_ucache_shm_segment_posix_t) + sizeof(void *), true);

	shared_segment = (php_ucache_shm_segment_posix_t *)((char *)(*shared_segments_p) + sizeof(void *));
	(*shared_segments_p)[0] = shared_segment;

	for (shared_segment_attempt = 0; shared_segment_attempt < PHP_UCACHE_POSIX_SHM_NAME_ATTEMPTS; shared_segment_attempt++) {
		if (!ucache_alloc_posix_segment_name(shared_segment_name, sizeof(shared_segment_name))) {
			*error_in = "php_random_bytes";

			return false;
		}

#if defined(HAVE_SHM_CREATE_LARGEPAGE)
		if (shared_segment_lg_index > 0) {
			shared_segment_fd = shm_create_largepage(shared_segment_name, shared_segment_flags, shared_segment_lg_index, SHM_LARGEPAGE_ALLOC_DEFAULT, shared_segment_mode);
			if (shared_segment_fd != -1) {
				break;
			}
		}
#endif /* defined(HAVE_SHM_CREATE_LARGEPAGE) */

		shared_segment_fd = shm_open(shared_segment_name, shared_segment_flags, shared_segment_mode);
		if (shared_segment_fd != -1) {
			break;
		}

		if (errno != EEXIST) {
			*error_in = "shm_open";

			return false;
		}
	}

	if (shared_segment_fd == -1) {
		*error_in = "shm_open";

		return false;
	}

	shared_segment->shm_fd = shared_segment_fd;

	if (ftruncate(shared_segment->shm_fd, requested_size) != 0) {
		*error_in = "ftruncate";

		close(shared_segment->shm_fd);

		shm_unlink(shared_segment_name);

		return false;
	}

#ifndef __APPLE__
	/* ftruncate() leaves the object sparse; on a tmpfs-backed /dev/shm the
	 * first touch beyond the free space raises SIGBUS, so commit it now and
	 * fail closed instead. Darwin shm objects are not files (fcntl would
	 * return EBADF) and are not size-capped by a filesystem. */
	if (!php_ucache_preallocate_fd(shared_segment->shm_fd, requested_size)) {
		*error_in = "preallocate";

		close(shared_segment->shm_fd);

		shm_unlink(shared_segment_name);

		return false;
	}
#endif /* !__APPLE__ */

	shared_segment->common.p = mmap(0, requested_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_segment->shm_fd, 0);
	if (shared_segment->common.p == MAP_FAILED) {
		*error_in = "mmap";

		close(shared_segment->shm_fd);

		shm_unlink(shared_segment_name);

		return false;
	}

	if (shm_unlink(shared_segment_name) != 0) {
		*error_in = "shm_unlink";

		munmap(shared_segment->common.p, requested_size);

		shared_segment->common.p = NULL;

		close(shared_segment->shm_fd);

		return false;
	}

	shared_segment->common.size = requested_size;

	return true;
}

static void ucache_alloc_posix_detach_segment(php_ucache_shm_segment_posix_t *shared_segment)
{
	munmap(shared_segment->common.p, shared_segment->common.size);
	close(shared_segment->shm_fd);
}

const php_ucache_shm_handlers_t php_ucache_alloc_posix_handlers = {
	(php_ucache_create_segments_t)ucache_alloc_posix_create_segments,
	(php_ucache_detach_segment_t)ucache_alloc_posix_detach_segment
};

#endif /* PHP_UCACHE_USE_SHM_OPEN */
