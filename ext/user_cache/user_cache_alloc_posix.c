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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "ext/random/php_random_csprng.h"

#define PHP_USER_CACHE_POSIX_SHM_NAME_PREFIX   "/php_uc."
#define PHP_USER_CACHE_POSIX_SHM_NAME_BYTES    10
#define PHP_USER_CACHE_POSIX_SHM_NAME_ATTEMPTS 5

typedef struct {
	php_user_cache_shm_segment common;
	int shm_fd;
} php_user_cache_shm_segment_posix;

static bool user_cache_alloc_posix_segment_name(char *buf, size_t buf_size)
{
	static const char hexits[] = "0123456789abcdef";
	unsigned char random_bytes[PHP_USER_CACHE_POSIX_SHM_NAME_BYTES];
	char hex[sizeof(random_bytes) * 2 + 1];
	size_t i;

	if (php_random_bytes_silent(random_bytes, sizeof(random_bytes)) == FAILURE) {
		return false;
	}

	for (i = 0; i < sizeof(random_bytes); i++) {
		hex[i * 2] = hexits[random_bytes[i] >> 4];
		hex[(i * 2) + 1] = hexits[random_bytes[i] & 0xf];
	}
	hex[sizeof(hex) - 1] = '\0';

	snprintf(buf, buf_size, PHP_USER_CACHE_POSIX_SHM_NAME_PREFIX "%s", hex);

	return true;
}

static int user_cache_alloc_posix_create_segments(size_t requested_size, php_user_cache_shm_segment_posix ***shared_segments_p, int *shared_segments_count, const char **error_in)
{
	php_user_cache_shm_segment_posix *shared_segment;
	mode_t shared_segment_mode = 0600;
	/* O_EXCL: never adopt an object somebody else created under this name. */
	int shared_segment_flags = O_RDWR | O_CREAT | O_EXCL,
		shared_segment_fd = -1, shared_segment_attempt;
	char shared_segment_name[sizeof(PHP_USER_CACHE_POSIX_SHM_NAME_PREFIX) + (PHP_USER_CACHE_POSIX_SHM_NAME_BYTES * 2)];

#if defined(HAVE_SHM_CREATE_LARGEPAGE)
	/* Prefer the largest compatible page size. Capture the getpagesizes()
	 * result as a signed int: its -1 error return in a size_t would pass
	 * the > 0 guard and index far outside the array (see the equivalent
	 * upstream fix in ext/opcache/shared_alloc_posix.c, GH-22429). */
	size_t shared_segment_lg_index = 0, shared_segment_sindexes[3] = {0};
	const size_t entries = sizeof(shared_segment_sindexes) / sizeof(shared_segment_sindexes[0]);
	int i, shared_segment_sizes;

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

	for (shared_segment_attempt = 0; shared_segment_attempt < PHP_USER_CACHE_POSIX_SHM_NAME_ATTEMPTS; shared_segment_attempt++) {
		if (!user_cache_alloc_posix_segment_name(shared_segment_name, sizeof(shared_segment_name))) {
			*error_in = "php_random_bytes";

			return PHP_USER_CACHE_ALLOC_FAILURE;
		}

#if defined(HAVE_SHM_CREATE_LARGEPAGE)
		if (shared_segment_lg_index > 0) {
			shared_segment_fd = shm_create_largepage(shared_segment_name, shared_segment_flags, shared_segment_lg_index, SHM_LARGEPAGE_ALLOC_DEFAULT, shared_segment_mode);
			if (shared_segment_fd != -1) {
				shared_segment->shm_fd = shared_segment_fd;

				goto truncate_segment;
			}
		}
#endif /* HAVE_SHM_CREATE_LARGEPAGE */

		shared_segment_fd = shm_open(shared_segment_name, shared_segment_flags, shared_segment_mode);
		if (shared_segment_fd != -1) {
			break;
		}

		if (errno != EEXIST) {
			*error_in = "shm_open";

			return PHP_USER_CACHE_ALLOC_FAILURE;
		}
	}

	if (shared_segment_fd == -1) {
		*error_in = "shm_open";

		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	shared_segment->shm_fd = shared_segment_fd;

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

	if (shm_unlink(shared_segment_name) != 0) {
		*error_in = "shm_unlink";

		munmap(shared_segment->common.p, requested_size);

		shared_segment->common.p = NULL;

		close(shared_segment->shm_fd);

		return PHP_USER_CACHE_ALLOC_FAILURE;
	}

	shared_segment->common.size = requested_size;

	return PHP_USER_CACHE_ALLOC_SUCCESS;
}

static int user_cache_alloc_posix_detach_segment(php_user_cache_shm_segment_posix *shared_segment)
{
	munmap(shared_segment->common.p, shared_segment->common.size);
	close(shared_segment->shm_fd);

	return 0;
}

const php_user_cache_shm_handlers php_user_cache_alloc_posix_handlers = {
	(php_user_cache_create_segments_t)user_cache_alloc_posix_create_segments,
	(php_user_cache_detach_segment_t)user_cache_alloc_posix_detach_segment
};

#endif /* PHP_USER_CACHE_USE_SHM_OPEN */
