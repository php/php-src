/*
  +----------------------------------------------------------------------+
  | Zend Signal Forwarding                                               |
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
*/

#ifndef ZEND_SIGNAL_FORWARD_H
#define ZEND_SIGNAL_FORWARD_H

#if defined(ZTS) && !defined(ZEND_WIN32)

#include <signal.h>

#include "zend_portability.h"

BEGIN_EXTERN_C()

void zend_signal_forward_startup(void);
void zend_signal_forward_shutdown(void);

/* For signal handlers. Hands the signal over to the thread that owns it, and
 * returns false if the caller has to apply the default policy for the signal. */
ZEND_API bool zend_signal_forward(int signo);
/* For signal handlers. As above, taking the default action when there is no owner. */
ZEND_API void zend_signal_forward_or_default(int signo);

/* The calling thread handles signo. If act is given, it is installed. */
ZEND_API int zend_signal_forward_acquire(int signo, const struct sigaction *act);
/* The calling thread no longer handles signo. If act is given, it is installed
 * unless the process-wide disposition is still needed by another thread. */
ZEND_API int zend_signal_forward_release(int signo, const struct sigaction *act);

/* Whether a thread has a handler for signo, which the execution timer must not replace. */
bool zend_signal_forward_has_handler(int signo);

/* The calling thread armed the execution timer, which delivers signo. If act is
 * given, it is restored when a handler for signo is released. */
ZEND_API void zend_signal_forward_timer_arm(int signo, const struct sigaction *act);
ZEND_API void zend_signal_forward_timer_disarm(void);

/* Everything registered by the calling thread ends with the request. */
void zend_signal_forward_request_shutdown(void);
/* To call in the child of a fork(), where only the calling thread exists. shares_fds
 * tells that the child shares its file descriptor table with its parent. */
ZEND_API void zend_signal_forward_fork_child(bool shares_fds);

END_EXTERN_C()

#endif /* defined(ZTS) && !defined(ZEND_WIN32) */

#endif /* ZEND_SIGNAL_FORWARD_H */
