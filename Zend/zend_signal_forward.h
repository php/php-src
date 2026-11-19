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

#include "zend_atomic.h"
#include "zend_portability.h"

/* See the assumptions at the top of zend_signal_forward.c. Elsewhere, a signal on a thread
 * without PHP context is not forwarded. */
#if defined(ZEND_ATOMIC_SIGNAL_SAFE) && !defined(ZEND_SIGNAL_FORWARD_DISABLED) \
 && ((defined(__linux__) && !defined(__BIONIC__) && !defined(__UCLIBC__)) \
  || defined(__APPLE__) || defined(__FreeBSD__))
# define ZEND_SIGNAL_FORWARD_ENABLED 1
#endif

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
/* The handler that the calling thread registered for signo was installed for it, and is
 * removed with the thread. */
ZEND_API void zend_signal_forward_mark_installed(int signo);
/* The helper thread stops and starts again, for a system call that requires a process
 * with a single thread. Signals meanwhile are forwarded when it has started again.
 * Returns whether a helper was stopped. The system may count the thread for a moment
 * after it was joined. */
ZEND_API bool zend_signal_forward_suspend(void);
ZEND_API void zend_signal_forward_resume(void);
/* The calling thread no longer handles signo. If act is given, it is installed
 * unless the process-wide disposition is still needed by another thread. */
ZEND_API int zend_signal_forward_release(int signo, const struct sigaction *act);

/* Whether a thread has a handler for signo, which the execution timer must not replace. */
bool zend_signal_forward_has_handler(int signo);

/* The calling thread armed the execution timer, which delivers signo. If act is given,
 * it becomes the disposition unless a thread handles signo, this thread ignores it, or
 * something else was installed for it, and it is restored when a handler is released. */
ZEND_API void zend_signal_forward_timer_arm(int signo, const struct sigaction *act);
ZEND_API void zend_signal_forward_timer_disarm(void);
/* Installs the handler of the execution timer, unless a thread has a handler for signo. */
ZEND_API int zend_signal_forward_timer_install(int signo, const struct sigaction *act);

/* Everything registered by the calling thread ends with the request. */
void zend_signal_forward_request_shutdown(void);
/* To call in the child of a fork(), where only the calling thread exists. shares_fds
 * tells that the child shares its file descriptor table with its parent. */
ZEND_API void zend_signal_forward_fork_child(bool shares_fds);

#if ZEND_DEBUG
/* Lets ext/zend_test reach states that depend on the timing of the helper thread. */
typedef enum {
	ZEND_SIGNAL_FORWARD_TEST_HOLD,
	ZEND_SIGNAL_FORWARD_TEST_RELEASE,
	ZEND_SIGNAL_FORWARD_TEST_SATURATE,
	ZEND_SIGNAL_FORWARD_TEST_FAIL_READ,
	ZEND_SIGNAL_FORWARD_TEST_HANDLER_EAGAIN,
	ZEND_SIGNAL_FORWARD_TEST_RUNNING,
	ZEND_SIGNAL_FORWARD_TEST_RESTART,
	ZEND_SIGNAL_FORWARD_TEST_READ_FD,
	ZEND_SIGNAL_FORWARD_TEST_WRITE_FD,
	ZEND_SIGNAL_FORWARD_TEST_LOCK,
	ZEND_SIGNAL_FORWARD_TEST_UNLOCK,
	ZEND_SIGNAL_FORWARD_TEST_PID,
	ZEND_SIGNAL_FORWARD_TEST_STALL_HANDLER,
	ZEND_SIGNAL_FORWARD_TEST_WAIT_STALLED,
	ZEND_SIGNAL_FORWARD_TEST_RESUME_HANDLER,
	ZEND_SIGNAL_FORWARD_TEST_FAIL_PIPE,
	ZEND_SIGNAL_FORWARD_TEST_FAIL_CREATE,
	ZEND_SIGNAL_FORWARD_TEST_FAIL_INIT
} zend_signal_forward_test_op;

ZEND_API int zend_signal_forward_test(zend_signal_forward_test_op op);
#endif

END_EXTERN_C()

#endif /* defined(ZTS) && !defined(ZEND_WIN32) */

#endif /* ZEND_SIGNAL_FORWARD_H */
