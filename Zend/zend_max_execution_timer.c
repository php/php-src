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

#ifdef ZEND_MAX_EXECUTION_TIMERS

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/types.h>
# ifdef __FreeBSD__
# include <pthread_np.h>
# endif

#include "zend.h"
#include "zend_globals.h"
#include "zend_portability.h"

#if defined(ZEND_MAX_EXECUTION_TIMER_CACHE) && defined(ZTS)
# include "TSRM/TSRM_atexit.h"
#endif

#if __has_feature(memory_sanitizer)
# include <sanitizer/msan_interface.h>
#endif

// Musl Libc defines this macro, glibc does not
// According to "man 2 timer_create" this field should always be available, but it's not: https://sourceware.org/bugzilla/show_bug.cgi?id=27417
# ifndef sigev_notify_thread_id
# define sigev_notify_thread_id _sigev_un._tid
# endif

// FreeBSD doesn't support CLOCK_BOOTTIME
# ifdef __FreeBSD__
# define ZEND_MAX_EXECUTION_TIMERS_CLOCK CLOCK_MONOTONIC
# else
# define ZEND_MAX_EXECUTION_TIMERS_CLOCK CLOCK_BOOTTIME
# endif

#ifdef ZEND_MAX_EXECUTION_TIMER_CACHE
typedef struct {
	pid_t pid;
	timer_t timer;
} zend_cached_timer;

static void timer_cache_dtor(void *ptr)
{
	zend_cached_timer *cache = ptr;
	if (cache->pid == getpid()) {
		timer_delete(cache->timer);
		cache->pid = 0;
	}
}

# ifdef ZTS
static ts_rsrc_id timer_cache_id;
# define TIMER_CACHE (timer_cache_id ? TSRMG_BULK_STATIC(timer_cache_id, zend_cached_timer *) : NULL)
# define CAN_CACHE_TIMER() tsrm_atexit_is_registered()

static void timer_cache_ctor(void *ptr)
{
	((zend_cached_timer *) ptr)->pid = 0;
}

void zend_max_execution_timer_cache_startup(void)
{
	ts_allocate_id(&timer_cache_id, sizeof(zend_cached_timer), timer_cache_ctor, timer_cache_dtor);
}

# else
static zend_cached_timer process_timer_cache;
# define TIMER_CACHE (&process_timer_cache)
# define CAN_CACHE_TIMER() true
# endif

void zend_max_execution_timer_cache_shutdown(void)
{
# ifdef ZTS
	if (timer_cache_id) {
		ts_free_id(timer_cache_id);
	}
# else
	timer_cache_dtor(&process_timer_cache);
# endif
}
#endif

/* Remember the SIGRTMIN handler that zend_timeout_handler() forwards foreign
 * signals to. After a request installs zend_timeout_handler(), the current
 * handler is ours, and saving it would recursively forward forever. */
static void zend_max_execution_timer_save_oldact(void)
{
	struct sigaction act;

	if (sigaction(SIGRTMIN, NULL, &act) == 0 && act.sa_sigaction != zend_timeout_handler) {
		EG(oldact) = act;
	}
}

ZEND_API void zend_max_execution_timer_init(void) /* {{{ */
{
	pid_t pid = getpid();

	if (EG(pid) == pid) {
		return;
	}

#ifdef ZEND_MAX_EXECUTION_TIMER_CACHE
	zend_cached_timer *cache = TIMER_CACHE;
	if (cache && cache->pid == pid) {
		EG(max_execution_timer_timer) = cache->timer;
		EG(pid) = pid;
		cache->pid = 0;
# ifdef MAX_EXECUTION_TIMERS_DEBUG
		fprintf(stderr, "Timer %#jx reused on thread %d\n", (uintmax_t) EG(max_execution_timer_timer), (pid_t) syscall(SYS_gettid));
# endif
		return;
	}
#endif

	struct sigevent sev;
	sev.sigev_notify = SIGEV_THREAD_ID;
	sev.sigev_value.sival_ptr = &EG(max_execution_timer_timer);
	sev.sigev_signo = SIGRTMIN;
# ifdef __FreeBSD__
	sev.sigev_notify_thread_id = pthread_getthreadid_np();
# else
	sev.sigev_notify_thread_id = (pid_t) syscall(SYS_gettid);
# endif

#if __has_feature(memory_sanitizer)
	/* MSan does not intercept timer_create() */
	__msan_unpoison(&EG(max_execution_timer_timer),
					sizeof(EG(max_execution_timer_timer)));
#endif

	// Measure wall time instead of CPU time as originally planned now that it is possible https://github.com/php/php-src/pull/6504#issuecomment-1370303727
	if (timer_create(ZEND_MAX_EXECUTION_TIMERS_CLOCK, &sev, &EG(max_execution_timer_timer)) != 0) {
		zend_strerror_noreturn(E_ERROR, errno, "Could not create timer");
	}

	EG(pid) = pid;

# ifdef MAX_EXECUTION_TIMERS_DEBUG
		fprintf(stderr, "Timer %#jx created on thread %d\n", (uintmax_t) EG(max_execution_timer_timer), sev.sigev_notify_thread_id);
# endif

	zend_max_execution_timer_save_oldact();
}
/* }}} */

void zend_max_execution_timer_settime(zend_long seconds) /* {{{ }*/
{
	/* Timer not initialized or shutdown. */
	if (!EG(pid)) {
		return;
	}

	timer_t timer = EG(max_execution_timer_timer);

	// Prevent EINVAL error
	if (seconds < 0 || seconds > 999999999) {
		seconds = 0;
	}

	struct itimerspec its;
	its.it_value.tv_sec = seconds;
	its.it_value.tv_nsec = its.it_interval.tv_sec = its.it_interval.tv_nsec = 0;

# ifdef MAX_EXECUTION_TIMERS_DEBUG
	fprintf(stderr, "Setting timer %#jx on thread %d (%ld seconds)...\n", (uintmax_t) timer, (pid_t) syscall(SYS_gettid), seconds);
# endif

	if (timer_settime(timer, 0, &its, NULL) != 0) {
		zend_strerror_noreturn(E_ERROR, errno, "Could not set timer");
	}
}
/* }}} */

void zend_max_execution_timer_shutdown(void) /* {{{ */
{
	/* Don't try to delete a timer created before a call to fork() */
	if (EG(pid) != getpid()) {
		return;
	}

#ifdef ZEND_MAX_EXECUTION_TIMER_CACHE
	static const struct itimerspec disarmed = {0};
	zend_cached_timer *cache = TIMER_CACHE;
	if (cache && CAN_CACHE_TIMER()
			&& timer_settime(EG(max_execution_timer_timer), 0, &disarmed, NULL) == 0) {
		*cache = (zend_cached_timer) {EG(pid), EG(max_execution_timer_timer)};
		EG(pid) = 0;
# ifdef MAX_EXECUTION_TIMERS_DEBUG
		fprintf(stderr, "Timer %#jx disarmed and cached on thread %d\n", (uintmax_t) cache->timer, (pid_t) syscall(SYS_gettid));
# endif
		return;
	}
#endif

	EG(pid) = 0;

	timer_t timer = EG(max_execution_timer_timer);

# ifdef MAX_EXECUTION_TIMERS_DEBUG
	fprintf(stderr, "Deleting timer %#jx on thread %d...\n", (uintmax_t) timer, (pid_t) syscall(SYS_gettid));
# endif

	int err = timer_delete(timer);
	if (err != 0) {
		zend_strerror_noreturn(E_ERROR, errno, "Could not delete timer");
	}
}
/* }}}} */

#endif
