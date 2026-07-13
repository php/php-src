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
   | Author: Go Kudo <zeriyoshi@php.net>.                                 |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_USER_CACHE_STORAGE_PORTABILITY_H
#define PHP_USER_CACHE_STORAGE_PORTABILITY_H

#include "user_cache_internal.h"

#ifdef ZEND_WIN32
# include "zend_execute.h"
# include "zend_system_id.h"
# include "win32/ioutil.h"

# include <fcntl.h>
# include <io.h>
# include <winbase.h>
#else
# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <pthread.h>
# include <sys/types.h>
# include <sys/stat.h>
# ifdef HAVE_UNISTD_H
#  include <unistd.h>
# endif
# if defined(PHP_USER_CACHE_HAVE_ANON_MMAP) || \
	defined(PHP_USER_CACHE_HAVE_BOUNDARY_SHM) || \
	(defined(__linux__) && defined(HAVE_MEMFD_CREATE))
#  include <sys/mman.h>
# endif
#endif

#if defined(__APPLE__) || defined(__FreeBSD__)
# include <sys/sysctl.h>
# ifdef __FreeBSD__
#  include <sys/param.h>
#  include <sys/user.h>
# endif
#endif

#ifdef PHP_USER_CACHE_HAVE_ANON_MMAP
# if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#  define MAP_ANONYMOUS MAP_ANON
# endif
#endif

#ifdef ZEND_WIN32
# define PHP_USER_CACHE_WIN32_MAPPING_NAME "PhpUserCache.SharedMemoryArea"
# define PHP_USER_CACHE_WIN32_MAPPING_MUTEX_NAME "PhpUserCache.SharedMemoryMutex"
# define PHP_USER_CACHE_WIN32_LOCK_FILE_NAME "PhpUserCache.LockFile"
#endif

#define PHP_USER_CACHE_ENTRY_LOCK_RETRY_INTERVAL_US 10000U

#ifdef ZTS
# ifdef ZEND_WIN32
#  define PHP_USER_CACHE_STARTUP_LOCK_INITIALIZER SRWLOCK_INIT
# else
#  define PHP_USER_CACHE_STARTUP_LOCK_INITIALIZER PTHREAD_MUTEX_INITIALIZER
# endif
#else
# define PHP_USER_CACHE_STARTUP_LOCK_INITIALIZER 0
#endif

#ifdef ZEND_WIN32
typedef struct _php_user_cache_win32_segment {
	php_user_cache_shm_segment segment;
	HANDLE memfile;
	void *mapping_base;
	size_t mapping_size;
} php_user_cache_win32_segment;
#endif

#ifdef ZTS
# ifdef ZEND_WIN32
typedef SRWLOCK php_user_cache_startup_lock;
# else
typedef pthread_mutex_t php_user_cache_startup_lock;
# endif
#else
typedef char php_user_cache_startup_lock;
#endif

#endif /* PHP_USER_CACHE_STORAGE_PORTABILITY_H */
