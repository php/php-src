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

#ifdef USE_SHM_OPEN

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct  {
    zend_shared_segment common;
    int shm_fd;
} zend_shared_segment_posix;

static int create_segments(size_t requested_size, zend_shared_segment_posix ***shared_segments_p, int *shared_segments_count, char **error_in)
{
	zend_shared_segment_posix *shared_segment;
	char shared_segment_name[sizeof("/ZendAccelerator.") + 20];

	*shared_segments_count = 1;
	*shared_segments_p = (zend_shared_segment_posix **) calloc(1, sizeof(zend_shared_segment_posix) + sizeof(void *));
	if (!*shared_segments_p) {
		*error_in = "calloc";
		return ALLOC_FAILURE;
	}
	shared_segment = (zend_shared_segment_posix *)((char *)(*shared_segments_p) + sizeof(void *));
	(*shared_segments_p)[0] = shared_segment;

	sprintf(shared_segment_name, "/ZendAccelerator.%d", getpid());
	shared_segment->shm_fd = shm_open(shared_segment_name, O_RDWR|O_CREAT|O_TRUNC, 0600);
	if (shared_segment->shm_fd == -1) {
		*error_in = "shm_open";
		return ALLOC_FAILURE;
	}

	if (ftruncate(shared_segment->shm_fd, requested_size) != 0) {
		*error_in = "ftruncate";
		shm_unlink(shared_segment_name);
		return ALLOC_FAILURE;
	}

	shared_segment->common.p = mmap(0, requested_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_segment->shm_fd, 0);
	if (shared_segment->common.p == MAP_FAILED) {
		*error_in = "mmap";
		shm_unlink(shared_segment_name);
		return ALLOC_FAILURE;
	}
	shm_unlink(shared_segment_name);

	shared_segment->common.pos = 0;
	shared_segment->common.size = requested_size;

	return ALLOC_SUCCESS;
}

static int detach_segment(zend_shared_segment_posix *shared_segment)
{
	munmap(shared_segment->common.p, shared_segment->common.size);
	close(shared_segment->shm_fd);
	return 0;
}

static size_t segment_type_size(void)
{
	return sizeof(zend_shared_segment_posix);
}

zend_shared_memory_handlers zend_alloc_posix_handlers = {
	(create_segments_t)create_segments,
	(detach_segment_t)detach_segment,
	segment_type_size
};

#endif /* USE_SHM_OPEN */
