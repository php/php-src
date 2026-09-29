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

#ifndef UCACHE_SHM_H
#define UCACHE_SHM_H

#include "php.h"

#ifdef HAVE_POSIX_FALLOCATE
# include <errno.h>
# include <fcntl.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
# ifdef HAVE_SHM_MMAP_POSIX
#  define UCACHE_USE_SHM_OPEN  1
# endif
# ifdef HAVE_SHM_MMAP_ANON
#  define UCACHE_USE_MMAP      1
# endif
#elif defined(__linux__) || defined(_AIX)
# ifdef HAVE_SHM_MMAP_POSIX
#  define UCACHE_USE_SHM_OPEN  1
# endif
# ifdef HAVE_SHM_IPC
#  define UCACHE_USE_SHM       1
# endif
# ifdef HAVE_SHM_MMAP_ANON
#  define UCACHE_USE_MMAP      1
# endif
#elif defined(__sparc) || defined(__sun)
# ifdef HAVE_SHM_MMAP_POSIX
#  define UCACHE_USE_SHM_OPEN  1
# endif
# ifdef HAVE_SHM_IPC
#  define UCACHE_USE_SHM       1
# endif
# if defined(__i386)
#  ifdef HAVE_SHM_MMAP_ANON
#   define UCACHE_USE_MMAP     1
#  endif
# endif
#else
# ifdef HAVE_SHM_MMAP_POSIX
#  define UCACHE_USE_SHM_OPEN  1
# endif
# ifdef HAVE_SHM_MMAP_ANON
#  define UCACHE_USE_MMAP      1
# endif
# ifdef HAVE_SHM_IPC
#  define UCACHE_USE_SHM       1
# endif
#endif

#define UCACHE_ALLOC_FAILURE  0
#define UCACHE_ALLOC_SUCCESS  1

#define UCACHE_SHM_RESERVE_ATTEMPTS	8U

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
# define UCACHE_PLATFORM_ALIGNMENT (alignof(ucache_align_test) < 8 ? 8 : alignof(ucache_align_test))
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
# define UCACHE_PLATFORM_ALIGNMENT (_Alignof(ucache_align_test) < 8 ? 8 : _Alignof(ucache_align_test))
#elif ZEND_GCC_VERSION >= 2000 || defined(__clang__)
# define UCACHE_PLATFORM_ALIGNMENT (__alignof__(ucache_align_test) < 8 ? 8 : __alignof__(ucache_align_test))
#else
# define UCACHE_PLATFORM_ALIGNMENT (sizeof(ucache_align_test))
#endif

#define UCACHE_ALIGNED_SIZE(size) \
	ZEND_MM_ALIGNED_SIZE_EX(size, UCACHE_PLATFORM_ALIGNMENT)

typedef union {
	void *ptr;
	double dbl;
	zend_long lng;
} ucache_align_test;

typedef struct {
	size_t size;
	void *p;
	bool zero_filled;
} ucache_shm_seg;

typedef int (*ucache_create_seg_t)(size_t requested_size, ucache_shm_seg **shared_seg, const char **err_in);
typedef void (*ucache_detach_seg_t)(ucache_shm_seg *shared_seg);

typedef struct {
	ucache_create_seg_t create_seg;
	ucache_detach_seg_t detach_seg;
} ucache_shm_handlers;

typedef struct {
	const char *name;
	const ucache_shm_handlers *handler;
} ucache_shm_handler_entry;

#ifdef UCACHE_USE_SHM
extern const ucache_shm_handlers ucache_alloc_shm_handlers;
#endif
#ifdef UCACHE_USE_SHM_OPEN
extern const ucache_shm_handlers ucache_alloc_posix_handlers;

/* Sparse shm_open() pages raise SIGBUS on first touch when tmpfs is full. */
static zend_always_inline bool ucache_shm_fd_reserve(int fd, size_t size)
{
# ifdef HAVE_POSIX_FALLOCATE
	uint32_t attempts = 0;
	int err;

	do {
		err = posix_fallocate(fd, 0, (off_t) size);
	} while (err == EINTR && ++attempts < UCACHE_SHM_RESERVE_ATTEMPTS);

	if (err == 0 || err == EINVAL || err == EOPNOTSUPP) {
		return true;
	}

	errno = err;

	return false;
# else
	(void) fd;
	(void) size;

	return true;
# endif
}
#endif

#endif /* UCACHE_SHM_H */
