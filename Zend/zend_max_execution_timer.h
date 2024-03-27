/*
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
   | Author: Kévin Dunglas <kevin@dunglas.dev>                            |
   +----------------------------------------------------------------------+
 */

#ifndef ZEND_MAX_EXECUTION_TIMER_H
#define ZEND_MAX_EXECUTION_TIMER_H

# ifdef ZEND_MAX_EXECUTION_TIMERS

#include "zend_long.h"

#  ifdef __APPLE__
#define ZEND_MAX_EXECUTION_TIMERS_SIGNAL SIGIO
#  else
#define ZEND_MAX_EXECUTION_TIMERS_SIGNAL SIGRTMIN
#  endif

/* Must be called after calls to fork() */
ZEND_API void zend_max_execution_timer_init(void);
void zend_max_execution_timer_settime(zend_long seconds);
void zend_max_execution_timer_shutdown(void);

# else

#define zend_max_execution_timer_init()
#define zend_max_execution_timer_settime(seconds)
#define zend_max_execution_timer_shutdown()

# endif
#endif
