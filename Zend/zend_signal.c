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

#include <fcntl.h>

#ifndef NSIG
# define NSIG 65
#endif

/* A process-directed signal may be delivered to a thread that never ran PHP.
 * Such a thread has no TSRM storage, so the signal has to reach a PHP thread.
 *
 * The signal handler only sets an atomic flag and writes to a pipe. It must not
 * call pthread_kill() itself: POSIX allows it, but on macOS it takes a libpthread
 * lock that the interrupted thread may already hold, which aborts the process.
 * A helper thread reads the pipe and calls pthread_kill() in normal context.
 *
 * zend_signal_forward_mutex protects the fields that are not atomic. The helper
 * holds it across pthread_kill(), and a thread removes its record under it before
 * exiting, so a signal is never sent to a thread that has exited.
 *
 * The owner of a signal is the thread that registered it last. A forwarded signal
 * has the siginfo of the pthread_kill(), not of the original sender. */
typedef struct _zend_signal_thread_t {
	pthread_t thread;
	uint64_t registered[NSIG]; /* Registration order, 0 when not registered */
	bool timer[NSIG];          /* Registered for the execution timer, not for a handler */
	struct _zend_signal_thread_t *prev;
	struct _zend_signal_thread_t *next;
} zend_signal_thread_t;

static pthread_mutex_t       zend_signal_forward_mutex;
static pthread_key_t         zend_signal_thread_key;
static bool                  zend_signal_forward_started;
static zend_signal_thread_t *zend_signal_threads;
static zend_signal_thread_t *zend_signal_owners[NSIG];
static bool                  zend_signal_timer_left[NSIG];
static uint64_t              zend_signal_registrations;
static pthread_t             zend_signal_helper;
static bool                  zend_signal_helper_running;
static bool                  zend_signal_helper_stop;
static int                   zend_signal_pipe[2] = {-1, -1};
static pid_t                 zend_signal_pipe_process;
static zend_atomic_int       zend_signal_forward_process;
static zend_atomic_int       zend_signal_forward_init_lock;

/* Read by signal handlers */
static zend_atomic_int  zend_signal_forward_pid;
static zend_atomic_int  zend_signal_forward_fd;
static zend_atomic_bool zend_signal_has_owner[NSIG];
static zend_atomic_bool zend_signal_pending[NSIG];

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

static void zend_signal_update_owner(int signo)
{
	zend_signal_thread_t *owner = NULL;

	for (zend_signal_thread_t *thread = zend_signal_threads; thread; thread = thread->next) {
		if (!thread->registered[signo-1]) {
			continue;
		}
		/* A handler goes first: the timer does not need the signal of another thread. */
		if (!owner
		 || (owner->timer[signo-1] && !thread->timer[signo-1])
		 || (owner->timer[signo-1] == thread->timer[signo-1] && thread->registered[signo-1] > owner->registered[signo-1])) {
			owner = thread;
		}
	}
	zend_signal_owners[signo-1] = owner;
	zend_atomic_bool_store_ex(&zend_signal_has_owner[signo-1], owner != NULL);
}

static void *zend_signal_helper_main(void *arg)
{
	sigset_t all;
	char buffer[64];

	sigfillset(&all);
	pthread_sigmask(SIG_BLOCK, &all, NULL);

	while (1) {
		ssize_t bytes = read(zend_signal_pipe[0], buffer, sizeof(buffer));

		if (bytes < 0 && errno == EINTR) {
			continue;
		}
		if (bytes <= 0) {
			break;
		}

		pthread_mutex_lock(&zend_signal_forward_mutex);
		if (zend_signal_helper_stop) {
			pthread_mutex_unlock(&zend_signal_forward_mutex);
			break;
		}
		for (int signo = 1; signo < NSIG; signo++) {
			if (zend_atomic_bool_exchange_ex(&zend_signal_pending[signo-1], false)) {
				if (zend_signal_owners[signo-1]) {
					pthread_kill(zend_signal_owners[signo-1]->thread, signo);
				} else if (!zend_signal_timer_left[signo-1]) {
					/* The owner left meanwhile: the signal now takes its current action.
					 * The signal of a cancelled timer has no purpose anymore. */
					kill(getpid(), signo);
				}
			}
		}
		pthread_mutex_unlock(&zend_signal_forward_mutex);
	}

	return NULL;
}

static void zend_signal_helper_start(void)
{
	if (zend_signal_helper_running) {
		return;
	}

	if (zend_signal_pipe[0] == -1) {
		int fds[2];

		if (pipe(fds) != 0) {
			return;
		}
		fcntl(fds[0], F_SETFD, FD_CLOEXEC);
		fcntl(fds[1], F_SETFD, FD_CLOEXEC);
		/* A full pipe already guarantees that the helper will wake up. */
		fcntl(fds[1], F_SETFL, fcntl(fds[1], F_GETFL) | O_NONBLOCK);
		zend_signal_pipe[0] = fds[0];
		zend_signal_pipe[1] = fds[1];
		zend_signal_pipe_process = getpid();
	}

	/* Published first, as the new thread may receive a signal before it blocks them. */
	zend_atomic_int_store_ex(&zend_signal_forward_fd, zend_signal_pipe[1]);
	zend_atomic_int_store_ex(&zend_signal_forward_pid, (int) getpid());

	zend_signal_helper_stop = false;
	if (pthread_create(&zend_signal_helper, NULL, zend_signal_helper_main, NULL) != 0) {
		zend_atomic_int_store_ex(&zend_signal_forward_pid, 0);
		return;
	}
	zend_signal_helper_running = true;
}

/* Called once per process, including the child of a fork(), where only the
 * calling thread exists and the mutex may have been held by another thread. */
static void zend_signal_forward_init(void)
{
	pthread_mutexattr_t attr;
	zend_signal_thread_t *self = pthread_getspecific(zend_signal_thread_key);
	zend_signal_thread_t *thread = zend_signal_threads;

	zend_atomic_int_store_ex(&zend_signal_forward_pid, 0);

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&zend_signal_forward_mutex, &attr);
	pthread_mutexattr_destroy(&attr);

	while (thread) {
		zend_signal_thread_t *next = thread->next;

		if (thread != self) {
			free(thread);
		}
		thread = next;
	}
	zend_signal_threads = self;
	if (self) {
		self->prev = self->next = NULL;
	}

	/* A forked child must not share the pipe of its parent. Otherwise the pipe
	 * stays open for the lifetime of the process, as handlers may be writing. */
	if (zend_signal_pipe[0] != -1 && zend_signal_pipe_process != getpid()) {
		close(zend_signal_pipe[0]);
		close(zend_signal_pipe[1]);
		zend_signal_pipe[0] = zend_signal_pipe[1] = -1;
	}
	zend_signal_helper_running = false;

	for (int signo = 1; signo < NSIG; signo++) {
		zend_atomic_bool_store_ex(&zend_signal_pending[signo-1], false);
		zend_signal_update_owner(signo);
	}

	zend_atomic_int_store_ex(&zend_signal_forward_process, (int) getpid());
}

static void zend_signal_forward_lock(void)
{
	int self = (int) getpid();

	if (UNEXPECTED(zend_atomic_int_load_ex(&zend_signal_forward_process) != self)) {
		int expected = 0;

		while (!zend_atomic_int_compare_exchange_ex(&zend_signal_forward_init_lock, &expected, self)) {
			if (expected != self && zend_atomic_int_compare_exchange_ex(&zend_signal_forward_init_lock, &expected, self)) {
				break;
			}
			expected = 0;
		}
		if (zend_atomic_int_load_ex(&zend_signal_forward_process) != self) {
			zend_signal_forward_init();
		}
		zend_atomic_int_store_ex(&zend_signal_forward_init_lock, 0);
	}

	pthread_mutex_lock(&zend_signal_forward_mutex);
}

static zend_always_inline void zend_signal_forward_unlock(void)
{
	pthread_mutex_unlock(&zend_signal_forward_mutex);
}

/* A signal already sent to this thread is delivered when a system call returns,
 * while the handler that it expects is still there. */
static zend_always_inline void zend_signal_deliver_forwarded(void)
{
	sigset_t pending;

	sigpending(&pending);
}

static bool zend_signal_thread_forget_self(zend_signal_thread_t *self, int signo)
{
	if (!self->registered[signo-1]) {
		return false;
	}

	zend_signal_timer_left[signo-1] = self->timer[signo-1];
	self->registered[signo-1] = 0;
	self->timer[signo-1] = false;
	if (zend_signal_owners[signo-1] == self) {
		zend_signal_update_owner(signo);
	}
	return true;
}

static void zend_signal_thread_exit(void *data)
{
	zend_signal_thread_t *self = data;
	zend_signal_thread_t *thread;
	sigset_t forgotten, pending, blocked;

	sigemptyset(&forgotten);
	zend_signal_forward_lock();
	/* The record is gone if the process forked or PHP shut down meanwhile. */
	for (thread = zend_signal_threads; thread && thread != self; thread = thread->next);
	if (thread) {
		for (int signo = 1; signo < NSIG; signo++) {
			if (zend_signal_thread_forget_self(self, signo)) {
				sigaddset(&forgotten, signo);
			}
		}
		if (self->prev) {
			self->prev->next = self->next;
		} else {
			zend_signal_threads = self->next;
		}
		if (self->next) {
			self->next->prev = self->prev;
		}
		free(self);
	}
	zend_signal_forward_unlock();

	/* A signal that this thread blocks would be lost with it. */
	sigpending(&pending);
	pthread_sigmask(SIG_BLOCK, NULL, &blocked);
	for (int signo = 1; signo < NSIG; signo++) {
		if (sigismember(&forgotten, signo) == 1 && sigismember(&pending, signo) == 1 && sigismember(&blocked, signo) == 1) {
			kill(getpid(), signo);
		}
	}
}

void zend_signal_thread_startup(void)
{
	pthread_key_create(&zend_signal_thread_key, zend_signal_thread_exit);
	zend_atomic_int_store_ex(&zend_signal_forward_process, 0);
	zend_signal_forward_started = true;
}

void zend_signal_thread_shutdown(void)
{
	zend_signal_thread_t *thread;
	bool initialized = zend_atomic_int_load_ex(&zend_signal_forward_process) == (int) getpid();

	zend_atomic_int_store_ex(&zend_signal_forward_pid, 0);
	zend_signal_forward_started = false;

	if (initialized && zend_signal_helper_running) {
		pthread_mutex_lock(&zend_signal_forward_mutex);
		zend_signal_helper_stop = true;
		pthread_mutex_unlock(&zend_signal_forward_mutex);
		/* Signal handlers may still be writing, so the pipe stays open. */
		while (write(zend_signal_pipe[1], "", 1) < 0 && errno == EINTR);
		pthread_join(zend_signal_helper, NULL);
		zend_signal_helper_running = false;
	}

	if (initialized) {
		pthread_mutex_lock(&zend_signal_forward_mutex);
	}
	thread = zend_signal_threads;
	while (thread) {
		zend_signal_thread_t *next = thread->next;
		free(thread);
		thread = next;
	}
	zend_signal_threads = NULL;
	for (int signo = 1; signo < NSIG; signo++) {
		zend_signal_owners[signo-1] = NULL;
		zend_atomic_bool_store_ex(&zend_signal_has_owner[signo-1], false);
	}
	if (initialized) {
		pthread_mutex_unlock(&zend_signal_forward_mutex);
	}
	pthread_setspecific(zend_signal_thread_key, NULL);
	pthread_key_delete(zend_signal_thread_key);
}

ZEND_API void zend_signal_thread_lock(void)
{
	if (zend_signal_forward_started) {
		zend_signal_forward_lock();
	}
}

ZEND_API void zend_signal_thread_unlock(void)
{
	if (zend_signal_forward_started) {
		zend_signal_forward_unlock();
	}
}

static void zend_signal_thread_register(int signo, bool timer)
{
	zend_signal_thread_t *self;

	if (!zend_signal_forward_started || !zend_signal_is_trackable(signo)) {
		return;
	}

	zend_signal_forward_lock();
	self = pthread_getspecific(zend_signal_thread_key);
	if (!self) {
		self = calloc(1, sizeof(zend_signal_thread_t));
		if (!self) {
			zend_signal_forward_unlock();
			return;
		}
		self->thread = pthread_self();
		self->next = zend_signal_threads;
		if (self->next) {
			self->next->prev = self;
		}
		zend_signal_threads = self;
		pthread_setspecific(zend_signal_thread_key, self);
	}
	self->registered[signo-1] = ++zend_signal_registrations;
	self->timer[signo-1] = timer;
	zend_signal_update_owner(signo);
	zend_signal_helper_start();
	zend_signal_forward_unlock();
}

static void zend_signal_thread_unregister(int signo, bool only_timer)
{
	zend_signal_thread_t *self;
	bool forgotten = false;

	if (!zend_signal_forward_started || !zend_signal_is_trackable(signo)) {
		return;
	}

	zend_signal_forward_lock();
	self = pthread_getspecific(zend_signal_thread_key);
	if (self && (!only_timer || self->timer[signo-1])) {
		forgotten = zend_signal_thread_forget_self(self, signo);
	}
	zend_signal_forward_unlock();

	if (forgotten) {
		zend_signal_deliver_forwarded();
	}
}

ZEND_API void zend_signal_thread_record(int signo)
{
	zend_signal_thread_register(signo, false);
}

ZEND_API void zend_signal_thread_forget(int signo)
{
	zend_signal_thread_unregister(signo, false);
}

ZEND_API void zend_signal_thread_record_timer(int signo)
{
	zend_signal_thread_register(signo, true);
}

ZEND_API void zend_signal_thread_forget_timer(int signo)
{
	zend_signal_thread_unregister(signo, true);
}

ZEND_API void zend_signal_thread_forget_all(void)
{
	zend_signal_thread_t *self;
	bool forgotten = false;

	if (!zend_signal_forward_started) {
		return;
	}

	zend_signal_forward_lock();
	self = pthread_getspecific(zend_signal_thread_key);
	if (self) {
		for (int signo = 1; signo < NSIG; signo++) {
			forgotten |= zend_signal_thread_forget_self(self, signo);
		}
	}
	zend_signal_forward_unlock();

	if (forgotten) {
		zend_signal_deliver_forwarded();
	}
}

ZEND_API bool zend_signal_thread_is_registered(int signo)
{
	bool registered = false;

	if (!zend_signal_forward_started || !zend_signal_is_trackable(signo)) {
		return false;
	}

	zend_signal_forward_lock();
	for (zend_signal_thread_t *thread = zend_signal_threads; thread; thread = thread->next) {
		if (thread->registered[signo-1] && !thread->timer[signo-1]) {
			registered = true;
			break;
		}
	}
	zend_signal_forward_unlock();

	return registered;
}

ZEND_API void zend_signal_thread_fork_child(void)
{
	if (!zend_signal_forward_started) {
		return;
	}

	zend_signal_forward_lock();
	for (int signo = 1; signo < NSIG; signo++) {
		if (zend_signal_owners[signo-1]) {
			zend_signal_helper_start();
			break;
		}
	}
	zend_signal_forward_unlock();
}

ZEND_API bool zend_signal_thread_forward(int signo)
{
#ifdef HAVE_NO_ATOMICS
	return false;
#else
	int errno_save = errno;
	bool forwarded = false;

	if (zend_signal_is_trackable(signo)
	 && zend_atomic_bool_load_ex(&zend_signal_has_owner[signo-1])
	 && zend_atomic_int_load_ex(&zend_signal_forward_pid) == (int) getpid()) {
		zend_atomic_bool_store_ex(&zend_signal_pending[signo-1], true);
		/* If the pipe is full, the helper is about to read the flag anyway. */
		(void) !write(zend_atomic_int_load_ex(&zend_signal_forward_fd), "", 1);
		forwarded = true;
	}

	errno = errno_save;
	return forwarded;
#endif
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
	if (!tsrm_is_managed_thread() || tsrm_is_shutdown()) {
		if (zend_signal_may_forward(signo) && zend_signal_thread_forward(signo)) {
			errno = errno_save;
			return;
		}
		/* Forward to default handler handler */
		zend_signal_handler(signo, siginfo, context);
		return;
	}

	/* Another PHP thread may handle a signal that this one does not. */
	if (zend_signal_may_forward(signo)
	 && (SIGG(handlers)[signo-1].handler == (void *) SIG_DFL || SIGG(handlers)[signo-1].handler == (void *) SIG_IGN)
	 && zend_signal_thread_forward(signo)) {
		errno = errno_save;
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
	if (!tsrm_is_managed_thread() || tsrm_is_shutdown()) {
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
		int sigaction_result;
#ifdef ZTS
		/* While the handler is still there: a signal forwarded here afterwards
		 * would take the default action. */
		if (handler == (void *) SIG_DFL || handler == (void *) SIG_IGN) {
			zend_signal_thread_forget(signo);
		}
#endif

		SIGG(handlers)[signo-1].flags = act->sa_flags;
		SIGG(handlers)[signo-1].handler = handler;

#ifdef ZTS
		zend_signal_thread_lock();
#endif

		memset(&sa, 0, sizeof(sa));
		if (SIGG(handlers)[signo-1].handler == (void *) SIG_IGN
#ifdef ZTS
		 /* The disposition is process-wide, and another thread handles this signal. */
		 && !zend_signal_thread_is_registered(signo)
#endif
		) {
			sa.sa_sigaction = (void *) SIG_IGN;
		} else {
			sa.sa_flags     = SA_ONSTACK | SA_SIGINFO | (act->sa_flags & SA_FLAGS_MASK);
			sa.sa_sigaction = zend_signal_handler_defer;
			sa.sa_mask      = global_sigmask;
		}

		sigaction_result = sigaction(signo, &sa, NULL);
#ifdef ZTS
		zend_signal_thread_unlock();
#endif
		if (sigaction_result < 0) {
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

#ifdef ZTS
	/* The handlers below replace any that this thread had registered. */
	zend_signal_thread_forget_all();
#endif

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
	for (signo = 1; signo < NSIG; ++signo) {
		if (sigaction(signo, NULL, &sa) == 0) {
			/* Left installed by an earlier startup in this process, which already
			 * saved the original handler. */
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
