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

#ifndef UCACHE_STORAGE_PORTABILITY_H
#define UCACHE_STORAGE_PORTABILITY_H

#include "user_cache_internal.h"

#ifdef ZEND_WIN32
# include "zend_execute.h"
# include "zend_system_id.h"
# include "win32/ioutil.h"

# include <aclapi.h>
# include <fcntl.h>
# include <io.h>
# include <lmcons.h>
# include <sddl.h>
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
# if defined(UCACHE_USE_MMAP) || \
	defined(UCACHE_HAVE_BOUNDARY_SHM) || \
	(defined(__linux__) && defined(HAVE_MEMFD_CREATE))
#  include <sys/mman.h>
# endif
#endif

#if defined(__APPLE__) || defined(__FreeBSD__)
# include <sys/sysctl.h>
# ifdef __FreeBSD__
#  include <sys/param.h>
#  include <sys/proc.h>
#  include <sys/user.h>
# endif
#elif defined(__NetBSD__) || defined(__OpenBSD__)
# include <sys/param.h>
# include <sys/proc.h>
# include <sys/sysctl.h>
#elif defined(__DragonFly__)
# include <sys/param.h>
# include <sys/kinfo.h>
# include <sys/sysctl.h>
# include <sys/user.h>
#elif defined(__sun)
# include <procfs.h>
#elif defined(_AIX)
# include <procinfo.h>
# include <sys/proc.h>
#endif

#ifdef UCACHE_USE_MMAP
# if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#  define MAP_ANONYMOUS MAP_ANON
# endif
#endif

#ifdef ZEND_WIN32
# define UCACHE_WIN32_MAPPING_NAME "PhpUserCache.SharedMemoryArea"
# define UCACHE_WIN32_MAPPING_MUTEX_NAME "PhpUserCache.SharedMemoryMutex"
# define UCACHE_WIN32_LOCK_FILE_NAME "PhpUserCache.LockFile"
# define UCACHE_WIN32_COMMIT_CHUNK ((size_t) 1024 * 1024)
#endif

#ifdef UCACHE_HAVE_BOUNDARY_SHM
# define UCACHE_BOUNDARY_ID_LEN 24U
#endif

#define UCACHE_ENTRY_LOCK_RETRY_INTERVAL_US 10000U
#define UCACHE_ENTRY_LOCK_RETRY_INITIAL_US 250U

#ifdef ZTS
# ifdef ZEND_WIN32
#  define UCACHE_STARTUP_LOCK_INITIALIZER SRWLOCK_INIT
# else
#  define UCACHE_STARTUP_LOCK_INITIALIZER PTHREAD_MUTEX_INITIALIZER
# endif
#endif

#ifdef ZEND_WIN32
typedef struct _ucache_win32_seg {
	ucache_shm_seg seg;
	HANDLE memfile;
	size_t committed_bytes;
} ucache_win32_seg;
#endif

#ifdef UCACHE_HAVE_BOUNDARY_SHM
typedef struct _ucache_boundary_seg {
	ucache_shm_seg seg;
	struct _ucache_boundary_seg *next;
	uint64_t shm_dev;
	uint64_t shm_ino;
	uint64_t lock_dev;
	uint64_t lock_ino;
	int dir_fd;
	int lock_fd;
	char id[UCACHE_BOUNDARY_ID_LEN + 1];
} ucache_boundary_seg;
#endif

#ifdef ZTS
# ifdef ZEND_WIN32
typedef SRWLOCK ucache_startup_lock;
# else
typedef pthread_mutex_t ucache_startup_lock;
# endif
#endif

#endif /* UCACHE_STORAGE_PORTABILITY_H */
