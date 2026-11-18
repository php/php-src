/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_ACCELERATOR_API_H
#define ZEND_ACCELERATOR_API_H

#include <time.h>

#include "Zend/zend_portability.h"

BEGIN_EXTERN_C()

/* Returns true during preloading */
ZEND_API bool opcache_preloading(void);

/* Returns true if the current request holds a read lock on shared memory.
 * OPcache does not restart while the lock is held, so pointers into shared
 * memory stay valid until the end of the request. Depending on the build, a
 * request takes the lock when it starts or the first time it uses the cache. */
ZEND_API bool opcache_shm_read_locked(void);

/* Returns the time shared memory was last reset by a restart, or 0 if it
 * never was or there is no shared memory. The value changes with every
 * restart. OPcache uses it to keep its in-process caches in sync with shared
 * memory, and extensions that keep pointers into shared memory between
 * requests can do the same. */
ZEND_API time_t opcache_last_restart_time(void);

END_EXTERN_C()

#endif /* ZEND_ACCELERATOR_API_H */
