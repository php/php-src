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
   | Author: Kévin Dunglas <kevin@dunglas.dev>                            |
   +----------------------------------------------------------------------+
 */

#ifndef ZEND_MAX_EXECUTION_TIMER_H
#define ZEND_MAX_EXECUTION_TIMER_H

# ifdef ZEND_MAX_EXECUTION_TIMERS

#include "zend_long.h"

/* Linux reuses each thread's timer across requests instead of recreating it. */
#  ifdef __linux__
#   define ZEND_MAX_EXECUTION_TIMER_CACHE
#  endif

BEGIN_EXTERN_C()
/* Must be called after calls to fork() */
ZEND_API void zend_max_execution_timer_init(void);
END_EXTERN_C()
void zend_max_execution_timer_settime(zend_long seconds);
void zend_max_execution_timer_shutdown(void);

# else

#define zend_max_execution_timer_init()
#define zend_max_execution_timer_settime(seconds)
#define zend_max_execution_timer_shutdown()

# endif

# ifdef ZEND_MAX_EXECUTION_TIMER_CACHE
#  ifdef ZTS
void zend_max_execution_timer_cache_startup(void);
#  endif
void zend_max_execution_timer_cache_shutdown(void);
# else
#  define zend_max_execution_timer_cache_startup()
#  define zend_max_execution_timer_cache_shutdown()
# endif

#endif
