/*
  +----------------------------------------------------------------------+
  | Zend Signal Handling                                                 |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
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

#define _GNU_SOURCE
#include <string.h>

#include "zend.h"
#include "zend_globals.h"
#include <signal.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

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
static int zend_signal_register(int signo, void (*handler)(int, siginfo_t*, void*));

#ifdef __CYGWIN__
#define TIMEOUT_SIG SIGALRM
#else
#define TIMEOUT_SIG SIGPROF
#endif

static int zend_sigs[] = { TIMEOUT_SIG, SIGHUP, SIGINT, SIGQUIT, SIGTERM, SIGUSR1, SIGUSR2 };

#define SA_FLAGS_MASK ~(SA_NODEFER | SA_RESETHAND)

/* True globals, written only at process startup */
static zend_signal_entry_t global_orig_handlers[NSIG];
static sigset_t            global_sigmask;

/* {{{ zend_signal_handler_defer
 *  Blocks signals if in critical section */
void zend_signal_handler_defer(int signo, siginfo_t *siginfo, void *context)
{
	int errno_save = errno;
	zend_signal_queue_t *queue, *qtmp;

#ifdef ZTS
	/* A signal could hit after TSRM shutdown, in this case globals are already freed. */
	if (tsrm_is_shutdown()) {
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

			if ((queue = SIGG(pavail))) { /* if none available it's simply forgotton */
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
	if (tsrm_is_shutdown()) {
		p_sig.flags = 0;
		p_sig.handler = SIG_DFL;
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
ZEND_API int zend_sigaction(int signo, const struct sigaction *act, struct sigaction *oldact)
{
	struct sigaction sa;
	sigset_t sigset;

	if (oldact != NULL) {
		oldact->sa_flags   = SIGG(handlers)[signo-1].flags;
		oldact->sa_handler = (void *) SIGG(handlers)[signo-1].handler;
		oldact->sa_mask    = global_sigmask;
	}
	if (act != NULL) {
		SIGG(handlers)[signo-1].flags = act->sa_flags;
		if (act->sa_flags & SA_SIGINFO) {
			SIGG(handlers)[signo-1].handler = (void *) act->sa_sigaction;
		} else {
			SIGG(handlers)[signo-1].handler = (void *) act->sa_handler;
		}

		memset(&sa, 0, sizeof(sa));
		if (SIGG(handlers)[signo-1].handler == (void *) SIG_IGN) {
			sa.sa_sigaction = (void *) SIG_IGN;
		} else {
			sa.sa_flags     = SA_SIGINFO | (act->sa_flags & SA_FLAGS_MASK);
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

	return SUCCESS;
}
/* }}} */

/* {{{ zend_signal
 *  Register a signal handler that will be deferred in critical sections */
ZEND_API int zend_signal(int signo, void (*handler)(int))
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_flags   = 0;
	sa.sa_handler = handler;
	sa.sa_mask    = global_sigmask;

	return zend_sigaction(signo, &sa, NULL);
}
/* }}} */

/* {{{ zend_signal_register
 *  Set a handler for a signal we want to defer.
 *  Previously set handler must have been saved before.
 */
static int zend_signal_register(int signo, void (*handler)(int, siginfo_t*, void*))
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
	SIGG(check)  = ZEND_DEBUG;
} /* }}} */

/* {{{ zend_signal_deactivate
 * */
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
