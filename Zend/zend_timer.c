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

#ifdef ZEND_TIMERS

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include "zend.h"
#include "zend_globals.h"

// Musl Libc defines this macro, glibc does not
// According to "man 2 timer_create" this field should always be available, but it's not: https://sourceware.org/bugzilla/show_bug.cgi?id=27417
# ifndef sigev_notify_thread_id
# define sigev_notify_thread_id _sigev_un._tid
# endif

ZEND_API void zend_timer_init(void) /* {{{ */
{
	struct sigevent sev;
	sev.sigev_notify = SIGEV_THREAD_ID;
	sev.sigev_value.sival_ptr = &EG(timer);
	// The chosen signal must:
	// 1. not be used internally by libc
	// 2. be allowed to happen spuriously without consequences
	// 3. not be commonly used by applications, this excludes SIGALRM, SIGUSR1 and SIGUSR2
	// 4. not be used by profilers, this excludes SIGPROF
	// 5. not be used internally by runtimes of programs that can embed PHP, this excludes SIGURG, which is used by Go
	sev.sigev_signo = SIGIO;
	sev.sigev_notify_thread_id = (pid_t) syscall(SYS_gettid);

	EG(pid) = getpid();
	// Measure wall time instead of CPU time as originally planned now that it is possible https://github.com/php/php-src/pull/6504#issuecomment-1370303727
	if (timer_create(CLOCK_REALTIME, &sev, &EG(timer)) != 0) {
		zend_strerror_noreturn(E_ERROR, errno, "Could not create timer");
	}

#  ifdef TIMER_DEBUG
		fprintf(stderr, "Timer %#jx created on thread %d\n", (uintmax_t) EG(timer), sev.sigev_notify_thread_id);
#  endif

	sigaction(sev.sigev_signo, NULL, &EG(oldact));
}
/* }}} */

void zend_timer_settime(zend_long seconds) /* {{{ }*/
{
	timer_t timer = EG(timer);

	/* Timer doesn't anymore on request shutdown. */
	if (timer == (timer_t){0}) {
		return;
	}

	struct itimerspec its;
	its.it_value.tv_sec = seconds;
	its.it_value.tv_nsec = its.it_interval.tv_sec = its.it_interval.tv_nsec = 0;

# ifdef TIMER_DEBUG
	fprintf(stderr, "Setting timer %#jx on thread %d (%ld seconds)...\n", (uintmax_t) timer, (pid_t) syscall(SYS_gettid), seconds);
# endif

	if (timer_settime(timer, 0, &its, NULL) != 0) {
		zend_strerror_noreturn(E_ERROR, errno, "Could not set timer");
	}
}
/* }}} */

void zend_timer_shutdown(void) /* {{{ */
{
	/* Don't try to delete a timer created before a call to fork() */
	if (EG(pid) != getpid()) {
		return;
	}

	timer_t timer = EG(timer);
	if (timer == (timer_t){0}) {
		/* Don't trigger an error here because the timer may not be initialized when PHP fail early, and on threads created by PHP but not managed by it. */
# ifdef TIMER_DEBUG
		fprintf(stderr, "Could not delete timer that has not been created on thread %d\n", (uintmax_t) timer, (pid_t) syscall(SYS_gettid));
# endif

		return;
	}

# ifdef TIMER_DEBUG
	fprintf(stderr, "Deleting timer %#jx on thread %d...\n", (uintmax_t) timer, (pid_t) syscall(SYS_gettid));
# endif

	int err = timer_delete(timer);
	EG(timer) = (timer_t){0};
	if (err != 0) {
		zend_strerror_noreturn(E_ERROR, errno, "Could not delete timer");
	}
}
/* }}}} */

#endif
