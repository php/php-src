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

/* A process-directed signal may be delivered to any thread that does not block
 * it, including one that has no TSRM storage and cannot run PHP's handlers. Such
 * a signal is forwarded to the thread that owns it.
 *
 * Owner: the thread that most recently registered a handler for the signal, or
 * otherwise the thread that most recently armed the execution timer delivering it.
 *
 * Signal handlers only use lock-free atomics, getpid(), time() and write(). They
 * must not call pthread_kill(): POSIX allows it, but on macOS it takes a libpthread
 * lock that the interrupted thread may already hold, which aborts the process. A
 * helper thread reads the pipe and calls pthread_kill() in normal context.
 *
 * The helper holds the mutex across pthread_kill(), and a thread removes its record
 * under that mutex before it exits, so a signal never targets an exited thread.
 *
 * A forwarded signal has the siginfo of the pthread_kill(): si_pid, si_uid, si_code
 * and any payload of the original signal are lost. Repeated signals coalesce. */

#include "zend.h"

#if defined(ZTS) && !defined(ZEND_WIN32)

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "zend_atomic.h"

#ifndef NSIG
# define NSIG 65
#endif

/* How long the signal of a timer that was just disarmed may still arrive. */
#define ZEND_SIGNAL_FORWARD_TIMER_GRACE 2

typedef struct _zend_signal_forward_thread {
	pthread_t thread;
	pid_t process;
	uint64_t handler[NSIG]; /* Registration order, 0 without a handler */
	bool ignored[NSIG];
	bool active;
	int timer_signo;
	uint64_t timer_order;
	bool has_timer_action;
	struct sigaction timer_action;
	struct _zend_signal_forward_thread *prev;
	struct _zend_signal_forward_thread *next;
} zend_signal_forward_thread;

/* What a forked child cannot safely reuse or free: the mutex may have been locked by
 * another thread, and the forking thread may still reach its record through the key. */
typedef struct _zend_signal_forward_abandoned {
	pthread_mutex_t mutex;
	int process;
	struct _zend_signal_forward_abandoned *previous;
} zend_signal_forward_abandoned;

/* Protected by the mutex, except for what the calling thread reads of its own record */
static zend_signal_forward_abandoned *zend_signal_forward_lock_state;
static pthread_mutex_t *zend_signal_forward_mutex;
static zend_signal_forward_thread *zend_signal_forward_threads;
static zend_signal_forward_thread *zend_signal_forward_stale_threads;
static uint64_t zend_signal_forward_order;
static pthread_t zend_signal_forward_helper;
static bool zend_signal_forward_helper_running;
static bool zend_signal_forward_helper_stop;

static pthread_key_t zend_signal_forward_key;
static int zend_signal_forward_pipe[2] = {-1, -1};
static pid_t zend_signal_forward_pipe_process;
static zend_atomic_bool zend_signal_forward_started;
static zend_atomic_int zend_signal_forward_process;
static zend_atomic_int zend_signal_forward_init_lock;

/* Read by signal handlers */
static zend_atomic_int zend_signal_forward_pid;
static zend_atomic_int zend_signal_forward_fd;
static zend_atomic_int zend_signal_forward_targets[NSIG];
static zend_atomic_int zend_signal_forward_handlers[NSIG];
static zend_atomic_int zend_signal_forward_ignoring[NSIG];
static zend_atomic_int zend_signal_forward_pending[NSIG];
static zend_atomic_int zend_signal_forward_generation[NSIG];
static zend_atomic_int zend_signal_forward_disarmed_at[NSIG];

static zend_always_inline bool zend_signal_forward_is_trackable(int signo)
{
	/* A fault must stay on the faulting thread: the instruction would run again as
	 * soon as the handler returned. Signals from NSIG up are not tracked. */
	return signo >= 1 && signo < NSIG
		&& signo != SIGSEGV && signo != SIGFPE && signo != SIGILL
#ifdef SIGBUS
		&& signo != SIGBUS
#endif
#ifdef SIGTRAP
		&& signo != SIGTRAP
#endif
	;
}

static void zend_signal_forward_update(int signo)
{
	int targets = 0, handlers = 0, ignoring = 0;

	for (zend_signal_forward_thread *thread = zend_signal_forward_threads; thread; thread = thread->next) {
		targets += thread->handler[signo-1] || thread->timer_signo == signo;
		handlers += thread->handler[signo-1] != 0;
		ignoring += thread->ignored[signo-1];
	}
	/* In this order, so that a handler never sees a signal as unhandled in between. */
	zend_atomic_int_store_ex(&zend_signal_forward_ignoring[signo-1], ignoring);
	zend_atomic_int_store_ex(&zend_signal_forward_handlers[signo-1], handlers);
	zend_atomic_int_store_ex(&zend_signal_forward_targets[signo-1], targets);
}

static zend_signal_forward_thread *zend_signal_forward_owner(int signo, bool *is_timer)
{
	zend_signal_forward_thread *owner = NULL, *timer = NULL;

	for (zend_signal_forward_thread *thread = zend_signal_forward_threads; thread; thread = thread->next) {
		if (thread->handler[signo-1] && (!owner || thread->handler[signo-1] > owner->handler[signo-1])) {
			owner = thread;
		}
		/* The timer is process-wide, so the most recent one is the one that runs. */
		if (thread->timer_signo == signo && (!timer || thread->timer_order > timer->timer_order)) {
			timer = thread;
		}
	}
	*is_timer = !owner && timer;
	return owner ? owner : timer;
}

static void zend_signal_forward_next_generation(int signo)
{
	int generation = zend_atomic_int_load_ex(&zend_signal_forward_generation[signo-1]);

	zend_atomic_int_store_ex(&zend_signal_forward_generation[signo-1], generation >= INT_MAX - 1 ? 0 : generation + 1);
}

static void zend_signal_forward_deliver(int signo, int pending)
{
	bool is_timer;
	zend_signal_forward_thread *owner = zend_signal_forward_owner(signo, &is_timer);

	if (owner) {
		/* A timer that was re-armed meanwhile did not expire. */
		if (!is_timer || pending == zend_atomic_int_load_ex(&zend_signal_forward_generation[signo-1]) + 1) {
			pthread_kill(owner->thread, signo);
		}
	} else if (zend_atomic_int_load_ex(&zend_signal_forward_ignoring[signo-1]) == 0
	 && time(NULL) - zend_atomic_int_load_ex(&zend_signal_forward_disarmed_at[signo-1]) > ZEND_SIGNAL_FORWARD_TIMER_GRACE) {
		/* The owner left meanwhile: the signal now takes its current action. */
		kill(getpid(), signo);
	}
}

static void *zend_signal_forward_helper_main(void *arg)
{
	sigset_t all;
	char buffer[64];

	sigfillset(&all);
	pthread_sigmask(SIG_BLOCK, &all, NULL);

	while (1) {
		ssize_t bytes = read(zend_signal_forward_pipe[0], buffer, sizeof(buffer));

		if (bytes < 0 && errno == EINTR) {
			continue;
		}
		if (bytes <= 0) {
			/* Signals take their default policy from now on. */
			zend_atomic_int_store_ex(&zend_signal_forward_pid, 0);
			break;
		}

		pthread_mutex_lock(zend_signal_forward_mutex);
		if (zend_signal_forward_helper_stop) {
			pthread_mutex_unlock(zend_signal_forward_mutex);
			break;
		}
		for (int signo = 1; signo < NSIG; signo++) {
			int pending = zend_atomic_int_exchange_ex(&zend_signal_forward_pending[signo-1], 0);

			if (pending) {
				zend_signal_forward_deliver(signo, pending);
			}
		}
		pthread_mutex_unlock(zend_signal_forward_mutex);
	}

	return NULL;
}

static bool zend_signal_forward_open_pipe(void)
{
	int fds[2];
	int flags;

	if (zend_signal_forward_pipe[0] != -1) {
		return true;
	}
	if (pipe(fds) != 0) {
		return false;
	}

	/* A full pipe already guarantees that the helper will wake up. */
	flags = fcntl(fds[1], F_GETFL);
	if (flags == -1 || fcntl(fds[1], F_SETFL, flags | O_NONBLOCK) == -1
	 || fcntl(fds[0], F_SETFD, FD_CLOEXEC) == -1 || fcntl(fds[1], F_SETFD, FD_CLOEXEC) == -1) {
		close(fds[0]);
		close(fds[1]);
		return false;
	}

	zend_signal_forward_pipe[0] = fds[0];
	zend_signal_forward_pipe[1] = fds[1];
	zend_signal_forward_pipe_process = getpid();
	return true;
}

static void zend_signal_forward_helper_start(void)
{
	if (zend_signal_forward_helper_running || !zend_signal_forward_open_pipe()) {
		return;
	}

	/* Published first, as the new thread may receive a signal before it blocks them. */
	zend_atomic_int_store_ex(&zend_signal_forward_fd, zend_signal_forward_pipe[1]);
	zend_atomic_int_store_ex(&zend_signal_forward_pid, (int) getpid());

	zend_signal_forward_helper_stop = false;
	if (pthread_create(&zend_signal_forward_helper, NULL, zend_signal_forward_helper_main, NULL) != 0) {
		zend_atomic_int_store_ex(&zend_signal_forward_pid, 0);
		return;
	}
	zend_signal_forward_helper_running = true;
}

/* Runs on first use in each process. In the child of a fork(), only the forking thread
 * exists, but this may run on another thread, so the records are kept reachable. */
static void zend_signal_forward_init(int process)
{
	zend_signal_forward_thread *self = pthread_getspecific(zend_signal_forward_key);
	zend_signal_forward_thread *thread = zend_signal_forward_threads;

	zend_atomic_int_store_ex(&zend_signal_forward_pid, 0);

	/* PHP may also start again in the same process. */
	if (!zend_signal_forward_lock_state || zend_signal_forward_lock_state->process != process) {
		zend_signal_forward_abandoned *state = malloc(sizeof(zend_signal_forward_abandoned));

		if (!state || pthread_mutex_init(&state->mutex, NULL) != 0) {
			free(state);
			zend_atomic_bool_store_ex(&zend_signal_forward_started, false);
			return;
		}
		state->process = process;
		state->previous = zend_signal_forward_lock_state;
		zend_signal_forward_lock_state = state;
		zend_signal_forward_mutex = &state->mutex;
	}

	while (thread) {
		zend_signal_forward_thread *next = thread->next;

		if (thread != self) {
			thread->prev = NULL;
			thread->next = zend_signal_forward_stale_threads;
			zend_signal_forward_stale_threads = thread;
		}
		thread = next;
	}
	zend_signal_forward_threads = self;
	if (self) {
		self->prev = self->next = NULL;
		self->process = process;
	}

	/* A child must not share the pipe of its parent. Otherwise the pipe stays open
	 * for the lifetime of the process, as a signal handler may be writing to it. */
	if (zend_signal_forward_pipe[0] != -1 && zend_signal_forward_pipe_process != process) {
		close(zend_signal_forward_pipe[0]);
		close(zend_signal_forward_pipe[1]);
		zend_signal_forward_pipe[0] = zend_signal_forward_pipe[1] = -1;
	}
	zend_signal_forward_helper_running = false;

	for (int signo = 1; signo < NSIG; signo++) {
		zend_atomic_int_store_ex(&zend_signal_forward_pending[signo-1], 0);
		zend_signal_forward_update(signo);
	}

	zend_atomic_int_store_ex(&zend_signal_forward_process, process);
}

static bool zend_signal_forward_lock(void)
{
	int process;

	if (!zend_atomic_bool_load_ex(&zend_signal_forward_started)) {
		return false;
	}

	process = (int) getpid();
	if (UNEXPECTED(zend_atomic_int_load_ex(&zend_signal_forward_process) != process)) {
		int holder = 0;

		/* A holder from another process does not exist in this one. */
		while (!zend_atomic_int_compare_exchange_ex(&zend_signal_forward_init_lock, &holder, process)) {
			if (holder != process && zend_atomic_int_compare_exchange_ex(&zend_signal_forward_init_lock, &holder, process)) {
				break;
			}
			holder = 0;
		}
		if (zend_atomic_int_load_ex(&zend_signal_forward_process) != process) {
			zend_signal_forward_init(process);
		}
		zend_atomic_int_store_ex(&zend_signal_forward_init_lock, 0);

		if (!zend_atomic_bool_load_ex(&zend_signal_forward_started)) {
			return false;
		}
	}

	pthread_mutex_lock(zend_signal_forward_mutex);
	if (!zend_atomic_bool_load_ex(&zend_signal_forward_started)) {
		pthread_mutex_unlock(zend_signal_forward_mutex);
		return false;
	}
	return true;
}

static zend_always_inline void zend_signal_forward_unlock(void)
{
	pthread_mutex_unlock(zend_signal_forward_mutex);
}

/* A signal already sent to this thread is delivered when a system call returns,
 * while the handler that it expects is still there. */
static zend_always_inline void zend_signal_forward_flush(void)
{
	sigset_t pending;

	sigpending(&pending);
}

static zend_signal_forward_thread *zend_signal_forward_self(void)
{
	zend_signal_forward_thread *self = pthread_getspecific(zend_signal_forward_key);
	pid_t process = getpid();

	if (self && self->process == process) {
		return self;
	}

	if (self) {
		/* Left behind by a fork() that another thread of the child noticed first. */
		memset(self, 0, sizeof(*self));
	} else {
		self = calloc(1, sizeof(zend_signal_forward_thread));
		if (!self) {
			return NULL;
		}
		/* Without the key, nothing would remove the record when this thread exits. */
		if (pthread_setspecific(zend_signal_forward_key, self) != 0) {
			free(self);
			return NULL;
		}
	}

	self->thread = pthread_self();
	self->process = process;
	self->next = zend_signal_forward_threads;
	if (self->next) {
		self->next->prev = self;
	}
	zend_signal_forward_threads = self;
	return self;
}

static void zend_signal_forward_disarm_self(zend_signal_forward_thread *self)
{
	int signo = self->timer_signo;

	if (signo) {
		self->timer_signo = 0;
		self->has_timer_action = false;
		zend_atomic_int_store_ex(&zend_signal_forward_disarmed_at[signo-1], (int) time(NULL));
		zend_signal_forward_next_generation(signo);
		zend_signal_forward_update(signo);
	}
}

static bool zend_signal_forward_clear_self(zend_signal_forward_thread *self)
{
	bool cleared = false;

	for (int signo = 1; signo < NSIG; signo++) {
		if (self->handler[signo-1] || self->ignored[signo-1]) {
			self->handler[signo-1] = 0;
			self->ignored[signo-1] = false;
			zend_signal_forward_update(signo);
			cleared = true;
		}
	}
	if (self->timer_signo) {
		zend_signal_forward_disarm_self(self);
		cleared = true;
	}
	self->active = false;
	return cleared;
}

static void zend_signal_forward_thread_exit(void *data)
{
	zend_signal_forward_thread *self = data;
	zend_signal_forward_thread *thread;
	sigset_t handled, pending, blocked;

	if (!zend_signal_forward_lock()) {
		return;
	}

	/* The record is left behind if this process was forked meanwhile. */
	for (thread = zend_signal_forward_threads; thread && thread != self; thread = thread->next);
	if (!thread) {
		zend_signal_forward_unlock();
		return;
	}

	sigemptyset(&handled);
	for (int signo = 1; signo < NSIG; signo++) {
		if (self->handler[signo-1]) {
			sigaddset(&handled, signo);
		}
	}
	zend_signal_forward_clear_self(self);
	if (self->prev) {
		self->prev->next = self->next;
	} else {
		zend_signal_forward_threads = self->next;
	}
	if (self->next) {
		self->next->prev = self->prev;
	}
	free(self);
	zend_signal_forward_unlock();

	/* A signal that this thread blocks would be lost with it. */
	sigpending(&pending);
	pthread_sigmask(SIG_BLOCK, NULL, &blocked);
	for (int signo = 1; signo < NSIG; signo++) {
		if (sigismember(&handled, signo) == 1 && sigismember(&pending, signo) == 1 && sigismember(&blocked, signo) == 1) {
			kill(getpid(), signo);
		}
	}
}

void zend_signal_forward_startup(void)
{
	if (pthread_key_create(&zend_signal_forward_key, zend_signal_forward_thread_exit) != 0) {
		return;
	}
	zend_atomic_int_store_ex(&zend_signal_forward_process, 0);
	zend_atomic_bool_store_ex(&zend_signal_forward_started, true);
}

void zend_signal_forward_shutdown(void)
{
	zend_signal_forward_thread *thread;

	if (!zend_atomic_bool_load_ex(&zend_signal_forward_started)) {
		return;
	}

	zend_atomic_int_store_ex(&zend_signal_forward_pid, 0);
	zend_atomic_bool_store_ex(&zend_signal_forward_started, false);

	if (zend_atomic_int_load_ex(&zend_signal_forward_process) == (int) getpid()) {
		pthread_mutex_lock(zend_signal_forward_mutex);
		zend_signal_forward_helper_stop = true;
		pthread_mutex_unlock(zend_signal_forward_mutex);

		if (zend_signal_forward_helper_running) {
			while (write(zend_signal_forward_pipe[1], "", 1) < 0 && errno == EINTR);
			pthread_join(zend_signal_forward_helper, NULL);
			zend_signal_forward_helper_running = false;
		}

		pthread_mutex_lock(zend_signal_forward_mutex);
		thread = zend_signal_forward_threads;
		while (thread) {
			zend_signal_forward_thread *next = thread->next;

			free(thread);
			thread = next;
		}
		zend_signal_forward_threads = NULL;
		for (int signo = 1; signo < NSIG; signo++) {
			zend_signal_forward_update(signo);
		}
		pthread_mutex_unlock(zend_signal_forward_mutex);
	}

	pthread_setspecific(zend_signal_forward_key, NULL);
	pthread_key_delete(zend_signal_forward_key);
}

ZEND_API bool zend_signal_forward(int signo)
{
#ifdef HAVE_NO_ATOMICS
	return false;
#else
	int errno_save = errno;
	bool forwarded = false;

	if (zend_signal_forward_is_trackable(signo)
	 && zend_atomic_int_load_ex(&zend_signal_forward_pid) == (int) getpid()) {
		if (zend_atomic_int_load_ex(&zend_signal_forward_targets[signo-1]) > 0) {
			zend_atomic_int_store_ex(&zend_signal_forward_pending[signo-1],
				zend_atomic_int_load_ex(&zend_signal_forward_generation[signo-1]) + 1);
			/* If the pipe is full, the helper is about to read the flag anyway. */
			(void) !write(zend_atomic_int_load_ex(&zend_signal_forward_fd), "", 1);
			forwarded = true;
		} else {
			/* Ignored by a PHP thread, or the signal of a timer that was just disarmed. */
			forwarded = zend_atomic_int_load_ex(&zend_signal_forward_ignoring[signo-1]) > 0
				|| time(NULL) - zend_atomic_int_load_ex(&zend_signal_forward_disarmed_at[signo-1]) <= ZEND_SIGNAL_FORWARD_TIMER_GRACE;
		}
	}

	errno = errno_save;
	return forwarded;
#endif
}

ZEND_API void zend_signal_forward_or_default(int signo)
{
	if (!zend_signal_forward(signo)) {
		/* A handler is registered in this process, but forwarding could not be set up. */
		if (zend_signal_forward_is_trackable(signo)
		 && zend_atomic_int_load_ex(&zend_signal_forward_process) == (int) getpid()
		 && zend_atomic_int_load_ex(&zend_signal_forward_handlers[signo-1]) > 0) {
			return;
		}
		signal(signo, SIG_DFL);
		kill(getpid(), signo);
	}
}

ZEND_API bool zend_signal_forward_has_handler(int signo)
{
	return zend_signal_forward_is_trackable(signo)
		&& zend_atomic_int_load_ex(&zend_signal_forward_handlers[signo-1]) > 0;
}

ZEND_API int zend_signal_forward_acquire(int signo, const struct sigaction *act)
{
	zend_signal_forward_thread *self;
	int result = 0;

	if (!zend_signal_forward_is_trackable(signo) || !zend_signal_forward_lock()) {
		return act ? sigaction(signo, act, NULL) : 0;
	}

	self = zend_signal_forward_self();
	if (self) {
		self->handler[signo-1] = ++zend_signal_forward_order;
		self->ignored[signo-1] = false;
		self->active = true;
		zend_signal_forward_update(signo);
		zend_signal_forward_helper_start();
	}

	if (act) {
		result = sigaction(signo, act, NULL);
		if (result != 0 && self) {
			self->handler[signo-1] = 0;
			zend_signal_forward_update(signo);
		}
	}

	zend_signal_forward_unlock();
	return result;
}

/* The disposition is process-wide, so it follows what the remaining threads need. */
static int zend_signal_forward_apply(int signo, const struct sigaction *act)
{
	struct sigaction ignore;
	const struct sigaction *timer_action = NULL;
	bool timer = false;

	for (zend_signal_forward_thread *thread = zend_signal_forward_threads; thread; thread = thread->next) {
		if (thread->handler[signo-1]) {
			return 0;
		}
		if (thread->timer_signo == signo) {
			timer = true;
			if (thread->has_timer_action) {
				timer_action = &thread->timer_action;
			}
		}
	}

	if (timer_action) {
		return sigaction(signo, timer_action, NULL);
	}
	if (timer && act->sa_handler == SIG_IGN) {
		/* With zend signals, the armed timer needs signals to keep being delivered. */
		return 0;
	}

	if (!timer && zend_atomic_int_load_ex(&zend_signal_forward_ignoring[signo-1]) > 0) {
		memset(&ignore, 0, sizeof(ignore));
		ignore.sa_handler = SIG_IGN;
		sigemptyset(&ignore.sa_mask);
		return sigaction(signo, &ignore, NULL);
	}

	return sigaction(signo, act, NULL);
}

ZEND_API int zend_signal_forward_release(int signo, const struct sigaction *act)
{
	zend_signal_forward_thread *self;
	bool ignore = act && act->sa_handler == SIG_IGN;
	int result = 0;

	if (!zend_signal_forward_is_trackable(signo) || !zend_signal_forward_lock()) {
		return act ? sigaction(signo, act, NULL) : 0;
	}

	self = ignore ? zend_signal_forward_self() : pthread_getspecific(zend_signal_forward_key);
	if (self) {
		self->handler[signo-1] = 0;
		self->ignored[signo-1] = ignore;
		self->active = self->active || ignore;
		/* This thread replaced the handler of its own timer. */
		if (self->timer_signo == signo) {
			zend_signal_forward_disarm_self(self);
		}
		zend_signal_forward_update(signo);
	}

	/* While the current disposition is still in place. */
	zend_signal_forward_flush();
	if (act) {
		result = zend_signal_forward_apply(signo, act);
	}

	zend_signal_forward_unlock();
	return result;
}

ZEND_API void zend_signal_forward_timer_arm(int signo, const struct sigaction *act)
{
	zend_signal_forward_thread *self;

	if (!zend_signal_forward_is_trackable(signo) || !zend_signal_forward_lock()) {
		return;
	}

	self = zend_signal_forward_self();
	if (self) {
		zend_signal_forward_disarm_self(self);
		self->timer_signo = signo;
		self->timer_order = ++zend_signal_forward_order;
		self->has_timer_action = act != NULL;
		if (act) {
			self->timer_action = *act;
		}
		self->active = true;
		zend_atomic_int_store_ex(&zend_signal_forward_disarmed_at[signo-1], 0);
		zend_signal_forward_next_generation(signo);
		zend_signal_forward_update(signo);
		zend_signal_forward_helper_start();
	}

	zend_signal_forward_unlock();
}

ZEND_API void zend_signal_forward_timer_disarm(void)
{
	zend_signal_forward_thread *self;

	if (!zend_atomic_bool_load_ex(&zend_signal_forward_started)) {
		return;
	}

	/* Most requests have no timer, and must not contend on the mutex. */
	self = pthread_getspecific(zend_signal_forward_key);
	if (!self || !self->timer_signo || !zend_signal_forward_lock()) {
		return;
	}

	zend_signal_forward_disarm_self(self);
	zend_signal_forward_unlock();
}

ZEND_API void zend_signal_forward_request_shutdown(void)
{
	zend_signal_forward_thread *self;
	bool cleared;

	if (!zend_atomic_bool_load_ex(&zend_signal_forward_started)) {
		return;
	}

	/* Most requests register nothing, and must not contend on the mutex. */
	self = pthread_getspecific(zend_signal_forward_key);
	if (!self || !self->active || !zend_signal_forward_lock()) {
		return;
	}

	cleared = zend_signal_forward_clear_self(self);
	zend_signal_forward_unlock();

	if (cleared) {
		zend_signal_forward_flush();
	}
}

ZEND_API void zend_signal_forward_fork_child(void)
{
	if (!zend_signal_forward_lock()) {
		return;
	}

	for (int signo = 1; signo < NSIG; signo++) {
		if (zend_atomic_int_load_ex(&zend_signal_forward_targets[signo-1]) > 0) {
			zend_signal_forward_helper_start();
			break;
		}
	}

	zend_signal_forward_unlock();
}

#endif /* defined(ZTS) && !defined(ZEND_WIN32) */
