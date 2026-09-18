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

#ifndef PHP_USER_CACHE_SHM_H
#define PHP_USER_CACHE_SHM_H

#include "php.h"

#if defined(__APPLE__) && defined(__MACH__) /* Darwin */
# ifdef HAVE_SHM_MMAP_POSIX
#  define PHP_UCACHE_USE_SHM_OPEN  1
# endif
# ifdef HAVE_SHM_MMAP_ANON
#  define PHP_UCACHE_USE_MMAP      1
# endif
#elif defined(__linux__) || defined(_AIX)
# ifdef HAVE_SHM_MMAP_POSIX
#  define PHP_UCACHE_USE_SHM_OPEN  1
# endif
# ifdef HAVE_SHM_IPC
#  define PHP_UCACHE_USE_SHM       1
# endif
# ifdef HAVE_SHM_MMAP_ANON
#  define PHP_UCACHE_USE_MMAP      1
# endif
#elif defined(__sparc) || defined(__sun)
# ifdef HAVE_SHM_MMAP_POSIX
#  define PHP_UCACHE_USE_SHM_OPEN  1
# endif
# ifdef HAVE_SHM_IPC
#  define PHP_UCACHE_USE_SHM       1
# endif
# if defined(__i386)
#  ifdef HAVE_SHM_MMAP_ANON
#   define PHP_UCACHE_USE_MMAP     1
#  endif
# endif
#else
# ifdef HAVE_SHM_MMAP_POSIX
#  define PHP_UCACHE_USE_SHM_OPEN  1
# endif
# ifdef HAVE_SHM_MMAP_ANON
#  define PHP_UCACHE_USE_MMAP      1
# endif
# ifdef HAVE_SHM_IPC
#  define PHP_UCACHE_USE_SHM       1
# endif
#endif /* defined(__APPLE__) && defined(__MACH__) */

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
# define PHP_UCACHE_PLATFORM_ALIGNMENT (alignof(php_ucache_align_test_t) < 8 ? 8 : alignof(php_ucache_align_test_t))
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
# define PHP_UCACHE_PLATFORM_ALIGNMENT (_Alignof(php_ucache_align_test_t) < 8 ? 8 : _Alignof(php_ucache_align_test_t))
#elif ZEND_GCC_VERSION >= 2000 || defined(__clang__)
# define PHP_UCACHE_PLATFORM_ALIGNMENT (__alignof__(php_ucache_align_test_t) < 8 ? 8 : __alignof__(php_ucache_align_test_t))
#else
# define PHP_UCACHE_PLATFORM_ALIGNMENT (sizeof(php_ucache_align_test_t))
#endif

#define PHP_UCACHE_ALIGNED_SIZE(size) \
	ZEND_MM_ALIGNED_SIZE_EX(size, PHP_UCACHE_PLATFORM_ALIGNMENT)

/* Segment offsets and block sizes are 32-bit counts of this many bytes, so a
 * segment can span 16 GiB without widening any stored field. */
#define PHP_UCACHE_SHM_UNIT 4
#define PHP_UCACHE_SHM_UNIT_ALIGNED_SIZE(size) \
	ZEND_MM_ALIGNED_SIZE_EX(size, PHP_UCACHE_SHM_UNIT)

/* Part of the shared-memory layout; bump PHP_UCACHE_VERSION if changed. */
typedef union {
	void *ptr;
	double dbl;
	zend_long lng;
} php_ucache_align_test_t;

typedef struct {
	size_t size;
	void *p;
} php_ucache_shm_segment_t;

typedef bool (*php_ucache_create_segments_t)(
		size_t requested_size,
		php_ucache_shm_segment_t ***shared_segments,
		uint32_t *shared_segment_count,
		const char **error_in);
typedef void (*php_ucache_detach_segment_t)(php_ucache_shm_segment_t *shared_segment);

typedef struct {
	php_ucache_create_segments_t create_segments;
	php_ucache_detach_segment_t detach_segment;
} php_ucache_shm_handlers_t;

typedef struct {
	const char *name;
	const php_ucache_shm_handlers_t *handler;
} php_ucache_shm_handler_entry_t;

#ifdef PHP_UCACHE_USE_SHM
extern const php_ucache_shm_handlers_t php_ucache_alloc_shm_handlers;
#endif
#ifdef PHP_UCACHE_USE_SHM_OPEN
extern const php_ucache_shm_handlers_t php_ucache_alloc_posix_handlers;
#endif

#ifndef ZEND_WIN32
bool php_ucache_preallocate_fd(int fd, size_t size);
#endif

#endif /* PHP_USER_CACHE_SHM_H */
