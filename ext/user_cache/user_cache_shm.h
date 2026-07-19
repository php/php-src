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
#  define PHP_USER_CACHE_USE_SHM_OPEN  1
# endif
# ifdef HAVE_SHM_MMAP_ANON
#  define PHP_USER_CACHE_USE_MMAP      1
# endif
#elif defined(__linux__) || defined(_AIX)
# ifdef HAVE_SHM_MMAP_POSIX
#  define PHP_USER_CACHE_USE_SHM_OPEN  1
# endif
# ifdef HAVE_SHM_IPC
#  define PHP_USER_CACHE_USE_SHM       1
# endif
# ifdef HAVE_SHM_MMAP_ANON
#  define PHP_USER_CACHE_USE_MMAP      1
# endif
#elif defined(__sparc) || defined(__sun)
# ifdef HAVE_SHM_MMAP_POSIX
#  define PHP_USER_CACHE_USE_SHM_OPEN  1
# endif
# ifdef HAVE_SHM_IPC
#  define PHP_USER_CACHE_USE_SHM       1
# endif
# if defined(__i386)
#  ifdef HAVE_SHM_MMAP_ANON
#   define PHP_USER_CACHE_USE_MMAP     1
#  endif
# endif
#else
# ifdef HAVE_SHM_MMAP_POSIX
#  define PHP_USER_CACHE_USE_SHM_OPEN  1
# endif
# ifdef HAVE_SHM_MMAP_ANON
#  define PHP_USER_CACHE_USE_MMAP      1
# endif
# ifdef HAVE_SHM_IPC
#  define PHP_USER_CACHE_USE_SHM       1
# endif
#endif

#define PHP_USER_CACHE_ALLOC_FAILURE  0
#define PHP_USER_CACHE_ALLOC_SUCCESS  1

/* Part of the shared-memory layout; bump PHP_USER_CACHE_VERSION if changed. */
typedef union {
	void *ptr;
	double dbl;
	zend_long lng;
} php_user_cache_align_test;

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
# define PHP_USER_CACHE_PLATFORM_ALIGNMENT (alignof(php_user_cache_align_test) < 8 ? 8 : alignof(php_user_cache_align_test))
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
# define PHP_USER_CACHE_PLATFORM_ALIGNMENT (_Alignof(php_user_cache_align_test) < 8 ? 8 : _Alignof(php_user_cache_align_test))
#elif ZEND_GCC_VERSION >= 2000 || defined(__clang__)
# define PHP_USER_CACHE_PLATFORM_ALIGNMENT (__alignof__(php_user_cache_align_test) < 8 ? 8 : __alignof__(php_user_cache_align_test))
#else
# define PHP_USER_CACHE_PLATFORM_ALIGNMENT (sizeof(php_user_cache_align_test))
#endif

#define PHP_USER_CACHE_ALIGNED_SIZE(size) \
	ZEND_MM_ALIGNED_SIZE_EX(size, PHP_USER_CACHE_PLATFORM_ALIGNMENT)

typedef struct {
	size_t size;
	size_t end;
	size_t pos;
	void *p;
} php_user_cache_shm_segment;

typedef int (*php_user_cache_create_segments_t)(size_t requested_size, php_user_cache_shm_segment ***shared_segments, int *shared_segment_count, const char **error_in);
typedef int (*php_user_cache_detach_segment_t)(php_user_cache_shm_segment *shared_segment);

typedef struct {
	php_user_cache_create_segments_t create_segments;
	php_user_cache_detach_segment_t detach_segment;
	size_t (*segment_type_size)(void);
} php_user_cache_shm_handlers;

typedef struct {
	const char *name;
	const php_user_cache_shm_handlers *handler;
} php_user_cache_shm_handler_entry;

#ifdef PHP_USER_CACHE_USE_SHM
extern const php_user_cache_shm_handlers php_user_cache_alloc_shm_handlers;
#endif
#ifdef PHP_USER_CACHE_USE_SHM_OPEN
extern const php_user_cache_shm_handlers php_user_cache_alloc_posix_handlers;
#endif

#endif /* PHP_USER_CACHE_SHM_H */
