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

#ifdef UCACHE_USE_SHM_OPEN

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "ext/random/php_random_csprng.h"

#define UCACHE_POSIX_SHM_NAME_PREFIX   "/php_uc."
#define UCACHE_POSIX_SHM_NAME_BYTES    10
#define UCACHE_POSIX_SHM_NAME_ATTEMPTS 5

static int ucache_alloc_posix_create_seg(size_t requested_size, ucache_shm_seg **shared_seg_p, const char **err_in);
static void ucache_alloc_posix_detach_seg(ucache_shm_seg *shared_seg);

const ucache_shm_handlers ucache_alloc_posix_handlers = {
	ucache_alloc_posix_create_seg,
	ucache_alloc_posix_detach_seg
};

static bool ucache_alloc_posix_seg_name(char *buf, size_t buf_size)
{
	static const char hexits[] = "0123456789abcdef";
	unsigned char random_bytes[UCACHE_POSIX_SHM_NAME_BYTES];
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

	snprintf(buf, buf_size, UCACHE_POSIX_SHM_NAME_PREFIX "%s", hex);

	return true;
}

static int ucache_alloc_posix_create_seg(size_t requested_size, ucache_shm_seg **shared_seg_p, const char **err_in)
{
	ucache_shm_seg *shared_seg;
	mode_t shared_seg_mode = 0600;
	uint32_t shared_seg_attempt;
	int shared_seg_flags = O_RDWR | O_CREAT | O_EXCL,
		shared_seg_fd = -1;
	char shared_seg_name[sizeof(UCACHE_POSIX_SHM_NAME_PREFIX) + (UCACHE_POSIX_SHM_NAME_BYTES * 2)];
	void *mapping;

#if defined(HAVE_SHM_CREATE_LARGEPAGE)
	size_t shared_seg_largest_idx = 0, shared_seg_sindexes[3] = {0};
	const size_t entries = sizeof(shared_seg_sindexes) / sizeof(shared_seg_sindexes[0]);
	int i, shared_seg_sizes;

	shared_seg_sizes = getpagesizes(shared_seg_sindexes, entries);

	if (shared_seg_sizes > 0) {
		for (i = shared_seg_sizes; i-- > 0; ) {
			if (shared_seg_sindexes[i] != 0 &&
				!(requested_size % shared_seg_sindexes[i])
			) {
				shared_seg_largest_idx = i;

				break;
			}
		}
	}
#endif /* HAVE_SHM_CREATE_LARGEPAGE */

	for (shared_seg_attempt = 0; shared_seg_attempt < UCACHE_POSIX_SHM_NAME_ATTEMPTS; shared_seg_attempt++) {
		if (!ucache_alloc_posix_seg_name(shared_seg_name, sizeof(shared_seg_name))) {
			*err_in = "php_random_bytes";

			return UCACHE_ALLOC_FAILURE;
		}

#if defined(HAVE_SHM_CREATE_LARGEPAGE)
		if (shared_seg_largest_idx > 0) {
			shared_seg_fd = shm_create_largepage(shared_seg_name, shared_seg_flags, shared_seg_largest_idx, SHM_LARGEPAGE_ALLOC_DEFAULT, shared_seg_mode);
			if (shared_seg_fd != -1) {
				break;
			}
		}
#endif /* HAVE_SHM_CREATE_LARGEPAGE */

		shared_seg_fd = shm_open(shared_seg_name, shared_seg_flags, shared_seg_mode);
		if (shared_seg_fd != -1) {
			break;
		}

		if (errno != EEXIST) {
			*err_in = "shm_open";

			return UCACHE_ALLOC_FAILURE;
		}
	}

	if (shared_seg_fd == -1) {
		*err_in = "shm_open";

		return UCACHE_ALLOC_FAILURE;
	}

	if (shm_unlink(shared_seg_name) != 0) {
		close(shared_seg_fd);

		*err_in = "shm_unlink";

		return UCACHE_ALLOC_FAILURE;
	}

	if (ftruncate(shared_seg_fd, requested_size) != 0) {
		close(shared_seg_fd);

		*err_in = "ftruncate";

		return UCACHE_ALLOC_FAILURE;
	}

	if (!ucache_shm_fd_reserve(shared_seg_fd, requested_size)) {
		close(shared_seg_fd);

		*err_in = "posix_fallocate";

		return UCACHE_ALLOC_FAILURE;
	}

	mapping = mmap(NULL, requested_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_seg_fd, 0);
	close(shared_seg_fd);
	if (mapping == MAP_FAILED) {
		*err_in = "mmap";

		return UCACHE_ALLOC_FAILURE;
	}

	shared_seg = (ucache_shm_seg *) calloc(1, sizeof(*shared_seg));
	if (shared_seg == NULL) {
		munmap(mapping, requested_size);

		*err_in = "calloc";

		return UCACHE_ALLOC_FAILURE;
	}

	shared_seg->p = mapping;
	shared_seg->size = requested_size;
	shared_seg->zero_filled = true;
	*shared_seg_p = shared_seg;

	return UCACHE_ALLOC_SUCCESS;
}

static void ucache_alloc_posix_detach_seg(ucache_shm_seg *shared_seg)
{
	munmap(shared_seg->p, shared_seg->size);
}

#endif /* UCACHE_USE_SHM_OPEN */
