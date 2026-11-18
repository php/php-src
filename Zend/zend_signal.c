/*
  +----------------------------------------------------------------------+
  | Zend Signal Handling                                                 |
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
  | Authors: Lucas Nealan <lucas@php.net>                                |
  |          Arnaud Le Blanc <lbarnaud@php.net>                          |
  +----------------------------------------------------------------------+

   This software was contributed to PHP by Facebook Inc. in 2008.

   Future revisions and derivatives of this source code must acknowledge
   Facebook Inc. as the original contributor of this module by leaving
   this note intact in the source code.

   All other licensing and usage conditions are those of the PHP Group.
*/

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <string.h>

#include "zend.h"
#include "zend_globals.h"
#include <signal.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#if defined(ZTS) && !defined(ZEND_WIN32)

#ifndef NSIG
# define NSIG 65
#endif

typedef struct _zend_signal_owner_t {
	bool      set;
	pid_t     pid;
	pthread_t thread;
} zend_signal_owner_t;

/* Protected by zend_signal_owners_lock, a spinlock because forwarding happens in
 * a signal handler. It is held across pthread_kill() to keep the target alive: a
 * thread forgets its signals before it exits, which waits for a forward in
 * progress.
 *
 * A thread cannot wait for the lock it already holds:
 * - The table is only updated by threads that run PHP, and a signal interrupting
 *   such a thread does not forward.
 * - Forwarding blocks signals first, so that no other forwarding handler can
 *   interrupt it. The mask is restored when the signal handler returns. It is
 *   deliberately not changed anywhere else: on macOS sigprocmask() applies to
 *   every thread, so a mask saved here could be the temporary one of another
 *   thread's pcntl_signal_dispatch().
 *
 * The lock holds the pid of its holder. A different pid means that the holder
 * was a thread of an ancestor process, which does not exist after a fork(), so
 * the lock can be taken over. There are deliberately no pthread_atfork()
 * handlers: holding this lock across fork() deadlocks with a forwarding thread
 * that was interrupted while holding an allocator lock. */
static zend_signal_owner_t zend_signal_owners[NSIG];
static zend_atomic_int     zend_signal_owners_lock;

static zend_always_inline void zend_signal_owners_acquire(void)
{
	int self = (int) getpid();

	while (1) {
		int holder = 0;

		if (zend_atomic_int_compare_exchange_ex(&zend_signal_owners_lock, &holder, self)) {
			return;
		}
		if (holder != self) {
			if (zend_atomic_int_compare_exchange_ex(&zend_signal_owners_lock, &holder, self)) {
				return;
			}
			continue;
		}
		while (zend_atomic_int_load_ex(&zend_signal_owners_lock) == self);
	}
}

static zend_always_inline void zend_signal_owners_release(void)
{
	zend_atomic_int_store_ex(&zend_signal_owners_lock, 0);
}

/* A fault must stay on the faulting thread: the instruction would run again as
 * soon as the handler returned. */
ZEND_API bool zend_signal_is_fault(int signo)
{
	return signo == SIGSEGV || signo == SIGFPE || signo == SIGILL
#ifdef SIGBUS
		|| signo == SIGBUS
#endif
#ifdef SIGTRAP
		|| signo == SIGTRAP
#endif
	;
}

/* Signals from NSIG up, which some systems allow, are not tracked. */
static zend_always_inline bool zend_signal_is_trackable(int signo)
{
	return signo >= 1 && signo < NSIG && !zend_signal_is_fault(signo);
}

ZEND_API void zend_signal_thread_record(int signo)
{
	if (zend_signal_is_trackable(signo)) {
		zend_signal_owners_acquire();
		zend_signal_owners[signo-1].thread = pthread_self();
		zend_signal_owners[signo-1].pid = getpid();
		zend_signal_owners[signo-1].set = true;
		zend_signal_owners_release();
	}
}

ZEND_API void zend_signal_thread_forget(int signo)
{
	if (zend_signal_is_trackable(signo)) {
		zend_signal_owners_acquire();
		if (zend_signal_owners[signo-1].set && pthread_equal(zend_signal_owners[signo-1].thread, pthread_self())) {
			zend_signal_owners[signo-1].set = false;
		}
		zend_signal_owners_release();
	}
}

ZEND_API void zend_signal_thread_forget_all(void)
{
	zend_signal_owners_acquire();
	for (int i = 0; i < NSIG; i++) {
		if (zend_signal_owners[i].set && pthread_equal(zend_signal_owners[i].thread, pthread_self())) {
			zend_signal_owners[i].set = false;
		}
	}
	zend_signal_owners_release();
}

ZEND_API void zend_signal_thread_fork_child(void)
{
	pid_t pid = getpid();

	zend_signal_owners_acquire();
	for (int i = 0; i < NSIG; i++) {
		if (zend_signal_owners[i].set) {
			if (pthread_equal(zend_signal_owners[i].thread, pthread_self())) {
				zend_signal_owners[i].pid = pid;
			} else {
				zend_signal_owners[i].set = false;
			}
		}
	}
	zend_signal_owners_release();
}

ZEND_API bool zend_signal_thread_forward(int signo)
{
	int errno_save = errno;
	bool forwarded = false;
	sigset_t block;

	if (!zend_signal_is_trackable(signo)) {
		return false;
	}

	sigemptyset(&block);
	for (int i = 1; i < NSIG; i++) {
		sigaddset(&block, i);
	}
	pthread_sigmask(SIG_BLOCK, &block, NULL);

	zend_signal_owners_acquire();
	/* A record from another process names a thread that does not exist here. */
	if (zend_signal_owners[signo-1].set
	 && zend_signal_owners[signo-1].pid == getpid()
	 && !pthread_equal(zend_signal_owners[signo-1].thread, pthread_self())) {
		forwarded = pthread_kill(zend_signal_owners[signo-1].thread, signo) == 0;
	}
	zend_signal_owners_release();

	errno = errno_save;
	return forwarded;
}

#endif /* defined(ZTS) && !defined(ZEND_WIN32) */

#ifdef ZEND_SIGNALS

#include "zend_signal.h"

#ifdef ZTS
ZEND_API int zend_signal_globals_id;
ZEND_API size_t zend_signal_globals_offset;
#else
ZEND_API zend_signal_globals_t zend_signal_globals;
#endif /* not ZTS */

#define SIGNAL_BEGIN_CRITICAL() \
	sigset_t oldmask; \
	zend_sigprocmask(SIG_BLOCK, &global_sigmask, &oldmask);
#define SIGNAL_END_CRITICAL() \
	zend_sigprocmask(SIG_SETMASK, &oldmask, NULL);

#ifdef ZTS
# define zend_sigprocmask(signo, set, oldset) tsrm_sigmask((signo), (set), (oldset))
#else
# define zend_sigprocmask(signo, set, oldset) sigprocmask((signo), (set), (oldset))
#endif

static void zend_signal_handler(int signo, siginfo_t *siginfo, void *context);
static zend_result zend_signal_register(int signo, void (*handler)(int, siginfo_t*, void*));

#if defined(__CYGWIN__) || defined(__PASE__)
/* Matches zend_execute_API.c; these platforms don't support ITIMER_PROF. */
#define TIMEOUT_SIG SIGALRM
#else
#define TIMEOUT_SIG SIGPROF
#endif

static const int zend_sigs[] = { TIMEOUT_SIG, SIGHUP, SIGINT, SIGQUIT, SIGTERM, SIGUSR1, SIGUSR2 };

#define SA_FLAGS_MASK ~(SA_NODEFER | SA_RESETHAND)

/* True globals, written only at process startup */
static zend_signal_entry_t global_orig_handlers[NSIG];
static sigset_t            global_sigmask;

#ifdef ZTS
/* A handler installed before PHP started keeps receiving the signals delivered
 * to threads that never ran PHP. */
static zend_always_inline bool zend_signal_may_forward(int signo)
{
	return signo < NSIG
		&& (global_orig_handlers[signo-1].handler == (void *) SIG_DFL
		 || global_orig_handlers[signo-1].handler == (void *) SIG_IGN);
}
#endif

/* {{{ zend_signal_handler_defer
 *  Blocks signals if in critical section */
static void zend_signal_handler_defer(int signo, siginfo_t *siginfo, void *context)
{
	int errno_save = errno;
	zend_signal_queue_t *queue, *qtmp;

#ifdef ZTS
	/* A signal could hit after TSRM shutdown, in this case globals are already freed.
	 * Or it could be delivered to a thread that didn't execute PHP yet.
	 * In the latter case we act as if SIGG(active) is false. */
	if (tsrm_is_shutdown() || !tsrm_is_managed_thread()) {
		if (!tsrm_is_shutdown() && zend_signal_may_forward(signo) && zend_signal_thread_forward(signo)) {
			errno = errno_save;
			return;
		}
		/* Forward to default handler handler */
		zend_signal_handler(signo, siginfo, context);
		return;
	}
#endif

	if (EXPECTED(SIGG(active))) {
		if (UNEXPECTED(SIGG(depth) == 0)) { /* try to handle signal */
			if (UNEXPECTED(SIGG(blocked))) {
				SIGG(blocked) = 0;
			}
			if (EXPECTED(SIGG(running) == 0)) {
				SIGG(running) = 1;
				zend_signal_handler(signo, siginfo, context);

				queue = SIGG(phead);
				SIGG(phead) = NULL;

				while (queue) {
					zend_signal_handler(queue->zend_signal.signo, queue->zend_signal.siginfo, queue->zend_signal.context);
					qtmp = queue->next;
					queue->next = SIGG(pavail);
					queue->zend_signal.signo = 0;
					SIGG(pavail) = queue;
					queue = qtmp;
				}
				SIGG(running) = 0;
			}
		} else { /* delay signal handling */
			SIGG(blocked) = 1; /* signal is blocked */

			if ((queue = SIGG(pavail))) { /* if none available it's simply forgotten */
				SIGG(pavail) = queue->next;
				queue->zend_signal.signo = signo;
				queue->zend_signal.siginfo = siginfo;
				queue->zend_signal.context = context;
				queue->next = NULL;

				if (SIGG(phead) && SIGG(ptail)) {
					SIGG(ptail)->next = queue;
				} else {
					SIGG(phead) = queue;
				}
				SIGG(ptail) = queue;
			}
#if ZEND_DEBUG
			else { /* this may not be safe to do, but could work and be useful */
				zend_output_debug_string(0, "zend_signal: not enough queue storage, lost signal (%d)", signo);
			}
#endif
		}
	} else {
		/* need to just run handler if we're inactive and getting a signal */
		zend_signal_handler(signo, siginfo, context);
	}

	errno = errno_save;
} /* }}} */

/* {{{ zend_signal_handler_unblock
 * Handle deferred signal from HANDLE_UNBLOCK_ALARMS */
ZEND_API void zend_signal_handler_unblock(void)
{
	zend_signal_queue_t *queue;
	zend_signal_t zend_signal;

	if (EXPECTED(SIGG(active))) {
		SIGNAL_BEGIN_CRITICAL(); /* procmask to protect handler_defer as if it were called by the kernel */
		queue = SIGG(phead);
		SIGG(phead) = queue->next;
		zend_signal = queue->zend_signal;
		queue->next = SIGG(pavail);
		queue->zend_signal.signo = 0;
		SIGG(pavail) = queue;

		zend_signal_handler_defer(zend_signal.signo, zend_signal.siginfo, zend_signal.context);
		SIGNAL_END_CRITICAL();
	}
}
/* }}} */

/* {{{ zend_signal_handler
 *  Call the previously registered handler for a signal
 */
static void zend_signal_handler(int signo, siginfo_t *siginfo, void *context)
{
	int errno_save = errno;
	struct sigaction sa;
	sigset_t sigset;
	zend_signal_entry_t p_sig;
#ifdef ZTS
	if (tsrm_is_shutdown() || !tsrm_is_managed_thread()) {
		p_sig = global_orig_handlers[signo-1];
	} else
#endif
	p_sig = SIGG(handlers)[signo-1];

	if (p_sig.handler == SIG_DFL) { /* raise default handler */
		if (sigaction(signo, NULL, &sa) == 0) {
			sa.sa_handler = SIG_DFL;
			sigemptyset(&sa.sa_mask);

			sigemptyset(&sigset);
			sigaddset(&sigset, signo);

			if (sigaction(signo, &sa, NULL) == 0) {
				/* throw away any blocked signals */
				zend_sigprocmask(SIG_UNBLOCK, &sigset, NULL);
#ifdef ZTS
# define RAISE_ERROR "raise() failed\n"
				if (raise(signo) != 0) {
					/* On some systems raise() fails with errno 3: No such process */
					kill(getpid(), signo);
				}
#else
				kill(getpid(), signo);
#endif
			}
		}
	} else if (p_sig.handler != SIG_IGN) {
		if (p_sig.flags & SA_SIGINFO) {
			if (p_sig.flags & SA_RESETHAND) {
				SIGG(handlers)[signo-1].flags   = 0;
				SIGG(handlers)[signo-1].handler = SIG_DFL;
			}
			(*(void (*)(int, siginfo_t*, void*))p_sig.handler)(signo, siginfo, context);
		} else {
			(*(void (*)(int))p_sig.handler)(signo);
		}
	}

	errno = errno_save;
} /* }}} */

/* {{{ zend_sigaction
 *  Register a signal handler that will be deferred in critical sections */
ZEND_API void zend_sigaction(int signo, const struct sigaction *act, struct sigaction *oldact)
{
	struct sigaction sa;
	sigset_t sigset;

	if (oldact != NULL) {
		oldact->sa_flags   = SIGG(handlers)[signo-1].flags;
		oldact->sa_handler = (void *) SIGG(handlers)[signo-1].handler;
		oldact->sa_mask    = global_sigmask;
	}
	if (act != NULL) {
		void *handler = (act->sa_flags & SA_SIGINFO) ? (void *) act->sa_sigaction : (void *) act->sa_handler;
#ifdef ZTS
		bool has_handler = handler != (void *) SIG_DFL && handler != (void *) SIG_IGN;

		/* While the handler is still there: a signal forwarded here afterwards
		 * would take the default action. */
		if (!has_handler) {
			zend_signal_thread_forget(signo);
		}
#endif

		SIGG(handlers)[signo-1].flags = act->sa_flags;
		SIGG(handlers)[signo-1].handler = handler;

#ifdef ZTS
		if (has_handler && zend_signal_may_forward(signo)) {
			zend_signal_thread_record(signo);
		}
#endif

		memset(&sa, 0, sizeof(sa));
		if (SIGG(handlers)[signo-1].handler == (void *) SIG_IGN) {
			sa.sa_sigaction = (void *) SIG_IGN;
		} else {
			sa.sa_flags     = SA_ONSTACK | SA_SIGINFO | (act->sa_flags & SA_FLAGS_MASK);
			sa.sa_sigaction = zend_signal_handler_defer;
			sa.sa_mask      = global_sigmask;
		}

		if (sigaction(signo, &sa, NULL) < 0) {
			zend_error_noreturn(E_ERROR, "Error installing signal handler for %d", signo);
		}

		/* unsure this signal is not blocked */
		sigemptyset(&sigset);
		sigaddset(&sigset, signo);
		zend_sigprocmask(SIG_UNBLOCK, &sigset, NULL);
	}
}
/* }}} */

/* {{{ zend_signal
 *  Register a signal handler that will be deferred in critical sections */
ZEND_API void zend_signal(int signo, void (*handler)(int))
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_flags   = 0;
	sa.sa_handler = handler;
	sa.sa_mask    = global_sigmask;

	zend_sigaction(signo, &sa, NULL);
}
/* }}} */

/* {{{ zend_signal_register
 *  Set a handler for a signal we want to defer.
 *  Previously set handler must have been saved before.
 */
static zend_result zend_signal_register(int signo, void (*handler)(int, siginfo_t*, void*))
{
	struct sigaction sa;

	if (sigaction(signo, NULL, &sa) == 0) {
		if ((sa.sa_flags & SA_SIGINFO) && sa.sa_sigaction == handler) {
			return FAILURE;
		}

		SIGG(handlers)[signo-1].flags = sa.sa_flags;
		if (sa.sa_flags & SA_SIGINFO) {
			SIGG(handlers)[signo-1].handler = (void *)sa.sa_sigaction;
		} else {
			SIGG(handlers)[signo-1].handler = (void *)sa.sa_handler;
		}

		sa.sa_flags     = SA_SIGINFO; /* we'll use a siginfo handler */
		sa.sa_sigaction = handler;
		sa.sa_mask      = global_sigmask;

		if (sigaction(signo, &sa, NULL) < 0) {
			zend_error_noreturn(E_ERROR, "Error installing signal handler for %d", signo);
		}

		return SUCCESS;
	}
	return FAILURE;
} /* }}} */

/* {{{ zend_signal_activate
 *  Install our signal handlers, per request */
void zend_signal_activate(void)
{
	size_t x;

	memcpy(&SIGG(handlers), &global_orig_handlers, sizeof(global_orig_handlers));

	if (SIGG(reset)) {
		for (x = 0; x < sizeof(zend_sigs) / sizeof(*zend_sigs); x++) {
			zend_signal_register(zend_sigs[x], zend_signal_handler_defer);
		}
	}

	SIGG(active) = 1;
	SIGG(depth)  = 0;
} /* }}} */

/* {{{ zend_signal_deactivate */
void zend_signal_deactivate(void)
{
	if (SIGG(check)) {
		size_t x;
		struct sigaction sa;

		if (SIGG(depth) != 0) {
			zend_error(E_CORE_WARNING, "zend_signal: shutdown with non-zero blocking depth (%d)", SIGG(depth));
		}

		/* did anyone steal our installed handler */
		for (x = 0; x < sizeof(zend_sigs) / sizeof(*zend_sigs); x++) {
			sigaction(zend_sigs[x], NULL, &sa);
			if (sa.sa_sigaction != zend_signal_handler_defer &&
					sa.sa_sigaction != (void *) SIG_IGN) {
				zend_error(E_CORE_WARNING, "zend_signal: handler was replaced for signal (%d) after startup", zend_sigs[x]);
			}
		}
	}

	/* After active=0 is set, signal handlers will be called directly and other
	 * state that is reset below will not be accessed. */
	*((volatile int *) &SIGG(active)) = 0;

	SIGG(running) = 0;
	SIGG(blocked) = 0;
	SIGG(depth) = 0;

	/* If there are any queued signals because of a missed unblock, drop them. */
	if (SIGG(phead) && SIGG(ptail)) {
		SIGG(ptail)->next = SIGG(pavail);
		SIGG(pavail) = SIGG(phead);
		SIGG(phead) = NULL;
		SIGG(ptail) = NULL;
	}
}
/* }}} */

static void zend_signal_globals_ctor(zend_signal_globals_t *zend_signal_globals) /* {{{ */
{
	size_t x;

	memset(zend_signal_globals, 0, sizeof(*zend_signal_globals));
	zend_signal_globals->reset = 1;

	for (x = 0; x < sizeof(zend_signal_globals->pstorage) / sizeof(*zend_signal_globals->pstorage); ++x) {
		zend_signal_queue_t *queue = &zend_signal_globals->pstorage[x];
		queue->zend_signal.signo = 0;
		queue->next = zend_signal_globals->pavail;
		zend_signal_globals->pavail = queue;
	}
}
/* }}} */

void zend_signal_init(void) /* {{{ */
{
	int signo;
	struct sigaction sa;

	/* Save previously registered signal handlers into orig_handlers */
	memset(&global_orig_handlers, 0, sizeof(global_orig_handlers));
	for (signo = 1; signo < NSIG; ++signo) {
		if (sigaction(signo, NULL, &sa) == 0) {
			/* Left installed by an earlier startup in this process. Keeping it
			 * as the original handler would make it call itself. */
			if ((sa.sa_flags & SA_SIGINFO) && sa.sa_sigaction == zend_signal_handler_defer) {
				continue;
			}
			global_orig_handlers[signo-1].flags = sa.sa_flags;
			if (sa.sa_flags & SA_SIGINFO) {
				global_orig_handlers[signo-1].handler = (void *) sa.sa_sigaction;
			} else {
				global_orig_handlers[signo-1].handler = (void *) sa.sa_handler;
			}
		}
	}
}
/* }}} */

/* {{{ zend_signal_startup
 * alloc zend signal globals */
ZEND_API void zend_signal_startup(void)
{

#ifdef ZTS
	ts_allocate_fast_id(&zend_signal_globals_id, &zend_signal_globals_offset, sizeof(zend_signal_globals_t), (ts_allocate_ctor) zend_signal_globals_ctor, NULL);
#else
	zend_signal_globals_ctor(&zend_signal_globals);
#endif

	/* Used to block signals during execution of signal handlers */
	sigfillset(&global_sigmask);
	sigdelset(&global_sigmask, SIGILL);
	sigdelset(&global_sigmask, SIGABRT);
	sigdelset(&global_sigmask, SIGFPE);
	sigdelset(&global_sigmask, SIGKILL);
	sigdelset(&global_sigmask, SIGSEGV);
	sigdelset(&global_sigmask, SIGCONT);
	sigdelset(&global_sigmask, SIGSTOP);
	sigdelset(&global_sigmask, SIGTSTP);
	sigdelset(&global_sigmask, SIGTTIN);
	sigdelset(&global_sigmask, SIGTTOU);
#ifdef SIGBUS
	sigdelset(&global_sigmask, SIGBUS);
#endif
#ifdef SIGSYS
	sigdelset(&global_sigmask, SIGSYS);
#endif
#ifdef SIGTRAP
	sigdelset(&global_sigmask, SIGTRAP);
#endif

	zend_signal_init();
}
/* }}} */


#endif /* ZEND_SIGNALS */
