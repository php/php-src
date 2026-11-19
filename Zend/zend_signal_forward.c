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
 * Signal handlers must not call pthread_kill(): POSIX allows it, but on macOS it
 * takes a libpthread lock that the interrupted thread may already hold, which aborts
 * the process. A helper thread reads a pipe and sends the signal instead. It runs from
 * the first registration that needs it until shutdown.
 *
 * Invariants:
 *
 * 1. Handlers only use lock-free atomics and async-signal-safe functions, take no
 *    lock, do not allocate, and preserve errno.
 * 2. A handler publishes the pending flag before it writes to the pipe.
 * 3. A nonzero flag means that a handler is about to write, that a byte is unread,
 *    or that the helper has yet to scan that flag. Only the handler that sets a flag
 *    from zero writes, so that a flag cannot wait for a wakeup that nobody will send.
 * 4. EAGAIN is a wakeup: the pipe was full after the flag was published, so a read and
 *    a scan are still to come. Any other write error clears the flag again, and so does
 *    a handler that finds the helper gone after it published the flag: the helper clears
 *    the pid before its final scan, so either that scan or the handler sees the flag.
 * 5. The helper scans every flag when it starts, after every successful read, and before
 *    it exits because a read failed.
 * 6. A flag coalesces the signals that arrive between two scans into one delivery. It
 *    keeps neither their number nor their siginfo: a forwarded signal has the siginfo
 *    of the pthread_kill(), so si_pid, si_uid, si_code and any payload are lost.
 * 7. pthread_kill() only runs on the helper with the mutex held, and a record leaves
 *    the list under that mutex before its thread exits, so that a signal never targets
 *    an exited thread. On Linux, the signal is sent with tgkill(): pthread_kill() takes
 *    a lock in the target's descriptor, and a target that forks at that moment leaves
 *    the lock held forever in its child, where the next forward would hang.
 * 8. The pipe stays open as long as this process may write to it. After a failed read,
 *    the helper starts a new one with a new pipe. The old descriptors are left alone:
 *    their numbers may already belong to something else.
 * 9. A forked child never reads the inherited list or mutex, which another thread may
 *    have been updating. It only keeps the record of the calling thread.
 * 10. zend_signal_forward_pid is only set while this process can notify a helper.
 * 11. zend_signal_forward_shutdown() requires that no other thread runs PHP.
 * 12. The mutex is the only lock. Handlers never take it, and the helper only sends
 *     signals while it holds it, so that no lock order can deadlock.
 * 13. Registrations, and the dispositions that this component decides on, only change
 *     under the mutex. A timer records itself and settles the disposition in one critical
 *     section, so that a handler cannot be registered or released between the two.
 * 14. A thread that started to exit cannot register again, so that no record outlives it.
 *     This takes a registration from before the thread exits: one that a destructor of
 *     another key makes first, on the last pass over the destructors, is not supported.
 *
 * Memory ordering: every atomic operation is sequentially consistent, with each backend
 * of zend_atomic.h. Invariant 4 depends on it. A handler stores the flag and then loads
 * the pid, and a helper that exits stores the pid and then loads the flag: with a weaker
 * order, both could miss the other's store.
 *
 * Platform assumption: POSIX does not list pthread_getspecific() as async-signal-safe, but
 * nothing portable tells a handler whether its thread has TSRM storage, and
 * zend_signal_handler_defer() has always relied on it. In glibc, musl, the macOS libpthread
 * and the FreeBSD libthr, it reads the storage of the calling thread without taking a lock
 * or allocating. Forwarding is compiled out on other systems.
 *
 * POSIX constraints: a handler may only call async-signal-safe functions, which excludes
 * mutexes and malloc(). After fork(), only the calling thread exists and every lock that
 * another thread held stays locked, including the one pthread_kill() takes in the thread
 * descriptor. pthread_atfork() cannot help: taking the mutex there deadlocks against a
 * thread that was interrupted inside malloc(). */

#include "zend.h"

#if defined(ZTS) && !defined(ZEND_WIN32)

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#ifdef __linux__
# include <sys/syscall.h>
#endif

#include "zend_atomic.h"

#ifndef NSIG
# define NSIG 65
#endif

/* How long the signal of a timer that was just disarmed may still arrive. */
#define ZEND_SIGNAL_FORWARD_TIMER_GRACE 2

typedef struct _zend_signal_forward_thread {
	pthread_t thread;
#ifdef SYS_tgkill
	pid_t tid;
#endif
	pid_t process;
	uint64_t handler[NSIG]; /* Registration order, 0 without a handler */
	bool ignored[NSIG];
	bool installed[NSIG];   /* The handler was installed through this component */
	bool active;
	int timer_signo;
	uint64_t timer_order;
	bool has_timer_action;
	struct sigaction timer_action;
	struct _zend_signal_forward_thread *prev;
	struct _zend_signal_forward_thread *next;
} zend_signal_forward_thread;

/* The state of each process, which a forked child abandons instead of reusing. */
typedef struct _zend_signal_forward_abandoned {
	pthread_mutex_t mutex;
	int process;
	zend_signal_forward_thread *inherited_threads;
	struct _zend_signal_forward_abandoned *previous;
} zend_signal_forward_abandoned;

/* Protected by the mutex, except for what the calling thread reads of its own record */
static zend_signal_forward_abandoned *zend_signal_forward_lock_state;
static pthread_mutex_t *zend_signal_forward_mutex;
static zend_signal_forward_thread *zend_signal_forward_threads;
static uint64_t zend_signal_forward_order;
static pthread_t zend_signal_forward_helper;
static bool zend_signal_forward_helper_running;
static bool zend_signal_forward_helper_stop;
static bool zend_signal_forward_shutting_down;
static bool zend_signal_forward_shared_fds;

static pthread_key_t zend_signal_forward_key;
/* The thread has started to exit. A key that keeps a value would make the system run the
 * destructors again, and FreeBSD reports every thread that still has one afterwards. */
static TSRM_TLS bool zend_signal_forward_exiting;
static int zend_signal_forward_pipe[2] = {-1, -1};
/* In a forked child: the handler of a thread that does not exist there. */
static void (*zend_signal_forward_orphaned[NSIG])(int);
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

#if ZEND_DEBUG
static zend_atomic_bool zend_signal_forward_test_hold;
static zend_atomic_bool zend_signal_forward_test_held;
static zend_atomic_bool zend_signal_forward_test_fail_read;
static zend_atomic_bool zend_signal_forward_test_eagain;
static zend_atomic_bool zend_signal_forward_test_stall;
static zend_atomic_bool zend_signal_forward_test_stalled;
static zend_atomic_bool zend_signal_forward_test_resume;
static zend_atomic_bool zend_signal_forward_test_fail_pipe;
static zend_atomic_bool zend_signal_forward_test_fail_create;
static zend_atomic_bool zend_signal_forward_test_fail_init;
#endif

static zend_always_inline zend_signal_forward_thread *zend_signal_forward_current(void)
{
	return zend_signal_forward_exiting ? NULL : pthread_getspecific(zend_signal_forward_key);
}

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

/* Wraps around with the clock, which does not fit an int after 2038. */
static zend_always_inline bool zend_signal_forward_recently_disarmed(int signo)
{
	int disarmed_at = zend_atomic_int_load_ex(&zend_signal_forward_disarmed_at[signo-1]);

	/* Signed, as the clock may have been set back since. */
	return disarmed_at != 0 && (int) ((unsigned int) time(NULL) - (unsigned int) disarmed_at) <= ZEND_SIGNAL_FORWARD_TIMER_GRACE;
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

/* The timer is process-wide, so the most recent one is the one that runs. */
static zend_signal_forward_thread *zend_signal_forward_timer_owner(int signo)
{
	zend_signal_forward_thread *timer = NULL;

	for (zend_signal_forward_thread *thread = zend_signal_forward_threads; thread; thread = thread->next) {
		if (thread->timer_signo == signo && (!timer || thread->timer_order > timer->timer_order)) {
			timer = thread;
		}
	}
	return timer;
}

static zend_signal_forward_thread *zend_signal_forward_owner(int signo, bool *is_timer)
{
	zend_signal_forward_thread *owner = NULL;

	for (zend_signal_forward_thread *thread = zend_signal_forward_threads; thread; thread = thread->next) {
		if (thread->handler[signo-1] && (!owner || thread->handler[signo-1] > owner->handler[signo-1])) {
			owner = thread;
		}
	}
	*is_timer = !owner;
	return owner ? owner : zend_signal_forward_timer_owner(signo);
}

static void zend_signal_forward_next_generation(int signo)
{
	int generation = zend_atomic_int_load_ex(&zend_signal_forward_generation[signo-1]);

	zend_atomic_int_store_ex(&zend_signal_forward_generation[signo-1], generation >= INT_MAX - 1 ? 0 : generation + 1);
}

static zend_always_inline void zend_signal_forward_identify(zend_signal_forward_thread *thread)
{
	thread->thread = pthread_self();
#ifdef SYS_tgkill
	thread->tid = (pid_t) syscall(SYS_gettid);
#endif
}

static zend_always_inline void zend_signal_forward_kill(const zend_signal_forward_thread *thread, int signo)
{
#ifdef SYS_tgkill
	syscall(SYS_tgkill, getpid(), thread->tid, signo);
#else
	pthread_kill(thread->thread, signo);
#endif
}

static void zend_signal_forward_deliver(int signo, int pending)
{
	bool is_timer;
	zend_signal_forward_thread *owner = zend_signal_forward_owner(signo, &is_timer);

	if (owner) {
		/* A timer that was re-armed meanwhile did not expire. */
		if (!is_timer || pending == zend_atomic_int_load_ex(&zend_signal_forward_generation[signo-1]) + 1) {
			zend_signal_forward_kill(owner, signo);
		}
	} else if (zend_atomic_int_load_ex(&zend_signal_forward_ignoring[signo-1]) == 0
	 && pending == zend_atomic_int_load_ex(&zend_signal_forward_generation[signo-1]) + 1
	 && !zend_signal_forward_recently_disarmed(signo)) {
		/* The owner left meanwhile: the signal now takes its current action. A timer
		 * that was disarmed since the signal was raised has a new generation. */
		kill(getpid(), signo);
	}
}

static void zend_signal_forward_scan(void)
{
	for (int signo = 1; signo < NSIG; signo++) {
		int pending = zend_atomic_int_exchange_ex(&zend_signal_forward_pending[signo-1], 0);

		if (pending) {
			zend_signal_forward_deliver(signo, pending);
		}
	}
}

static void zend_signal_forward_helper_start(void);

static void *zend_signal_forward_helper_main(void *arg)
{
	char buffer[64];

	while (1) {
		int fd;
		ssize_t bytes;

		pthread_mutex_lock(zend_signal_forward_mutex);
		if (zend_signal_forward_helper_stop) {
			pthread_mutex_unlock(zend_signal_forward_mutex);
			return NULL;
		}
		zend_signal_forward_scan();
		fd = zend_signal_forward_pipe[0];
		pthread_mutex_unlock(zend_signal_forward_mutex);

		bytes = read(fd, buffer, sizeof(buffer));
#if ZEND_DEBUG
		if (zend_atomic_bool_load_ex(&zend_signal_forward_test_hold)) {
			zend_atomic_bool_store_ex(&zend_signal_forward_test_held, true);
			while (zend_atomic_bool_load_ex(&zend_signal_forward_test_hold)) {
				usleep(1000);
			}
			zend_atomic_bool_store_ex(&zend_signal_forward_test_held, false);
		}
		if (zend_atomic_bool_exchange_ex(&zend_signal_forward_test_fail_read, false)) {
			bytes = 0;
		}
#endif
		if (bytes > 0 || (bytes < 0 && errno == EINTR)) {
			continue;
		}

		/* The pipe is unusable, and the read may have consumed a notification. */
		pthread_mutex_lock(zend_signal_forward_mutex);
		if (!zend_signal_forward_helper_stop) {
			zend_atomic_int_store_ex(&zend_signal_forward_pid, 0);
			zend_signal_forward_scan();
			zend_signal_forward_pipe[0] = zend_signal_forward_pipe[1] = -1;
			zend_signal_forward_helper_running = false;
			pthread_detach(pthread_self());
			/* The registrations that exist still need a helper. */
			zend_signal_forward_helper_start();
		}
		pthread_mutex_unlock(zend_signal_forward_mutex);
		return NULL;
	}
}

static bool zend_signal_forward_open_pipe(void)
{
	int fds[2];
	int flags;

	if (zend_signal_forward_pipe[0] != -1) {
		return true;
	}
#if ZEND_DEBUG
	if (zend_atomic_bool_exchange_ex(&zend_signal_forward_test_fail_pipe, false)) {
		return false;
	}
#endif
	if (pipe(fds) != 0) {
		return false;
	}

	/* With a standard stream closed, its number must not end up on the pipe. */
	for (int i = 0; i < 2; i++) {
		if (fds[i] < 3) {
			int fd = fcntl(fds[i], F_DUPFD, 3);

			close(fds[i]);
			fds[i] = fd;
		}
	}

	/* A full pipe already guarantees that the helper will wake up. */
	flags = fds[1] == -1 ? -1 : fcntl(fds[1], F_GETFL);
	if (fds[0] == -1 || flags == -1 || fcntl(fds[1], F_SETFL, flags | O_NONBLOCK) == -1
	 || fcntl(fds[0], F_SETFD, FD_CLOEXEC) == -1 || fcntl(fds[1], F_SETFD, FD_CLOEXEC) == -1) {
		if (fds[0] != -1) {
			close(fds[0]);
		}
		if (fds[1] != -1) {
			close(fds[1]);
		}
		return false;
	}

	zend_signal_forward_pipe[0] = fds[0];
	zend_signal_forward_pipe[1] = fds[1];
	zend_signal_forward_pipe_process = getpid();
	return true;
}

static void zend_signal_forward_helper_start(void)
{
	sigset_t all, old;
	bool created;

	/* A pipe opened in a table shared with the parent would outlive this process. */
	if (zend_signal_forward_helper_running || zend_signal_forward_shared_fds || zend_signal_forward_shutting_down
	 || !zend_signal_forward_open_pipe()) {
		return;
	}

	zend_atomic_int_store_ex(&zend_signal_forward_fd, zend_signal_forward_pipe[1]);
	zend_atomic_int_store_ex(&zend_signal_forward_pid, (int) getpid());

	/* The helper inherits this mask, so that it never takes a signal for another thread:
	 * it would reach its handler late, and after signals that were sent later. */
	sigfillset(&all);
	pthread_sigmask(SIG_BLOCK, &all, &old);
	zend_signal_forward_helper_stop = false;
#if ZEND_DEBUG
	if (zend_atomic_bool_exchange_ex(&zend_signal_forward_test_fail_create, false)) {
		created = false;
	} else
#endif
	created = pthread_create(&zend_signal_forward_helper, NULL, zend_signal_forward_helper_main, NULL) == 0;
	pthread_sigmask(SIG_SETMASK, &old, NULL);

	if (!created) {
		zend_atomic_int_store_ex(&zend_signal_forward_pid, 0);
		zend_signal_forward_scan();
		return;
	}
	zend_signal_forward_helper_running = true;
}

static void zend_signal_forward_helper_start_if_needed(void)
{
	for (int signo = 1; signo < NSIG; signo++) {
		if (zend_atomic_int_load_ex(&zend_signal_forward_targets[signo-1]) > 0) {
			zend_signal_forward_helper_start();
			break;
		}
	}
}

/* A helper that cannot be woken up is left behind: it never touches anything again. */
static void zend_signal_forward_helper_join(void)
{
	ssize_t written;

	do {
		written = write(zend_signal_forward_pipe[1], "", 1);
	} while (written < 0 && errno == EINTR);

	if (written < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
		pthread_detach(zend_signal_forward_helper);
		zend_signal_forward_pipe[0] = zend_signal_forward_pipe[1] = -1;
	} else {
		pthread_join(zend_signal_forward_helper, NULL);
	}
}

/* Runs on first use in each process. In the child of a fork(), only the forking thread
 * exists, but this may run on another thread, so the records are kept reachable. */
static void zend_signal_forward_init(int process)
{
	zend_signal_forward_thread *self = zend_signal_forward_current();

	zend_atomic_int_store_ex(&zend_signal_forward_pid, 0);

#if ZEND_DEBUG
	zend_atomic_bool_store_ex(&zend_signal_forward_test_hold, false);
	zend_atomic_bool_store_ex(&zend_signal_forward_test_held, false);
#endif

	/* PHP may also start again in the same process. */
	if (!zend_signal_forward_lock_state || zend_signal_forward_lock_state->process != process) {
		zend_signal_forward_abandoned *state = malloc(sizeof(zend_signal_forward_abandoned));
		bool failed = !state;

#if ZEND_DEBUG
		failed = zend_atomic_bool_exchange_ex(&zend_signal_forward_test_fail_init, false) || failed;
#endif
		if (failed || pthread_mutex_init(&state->mutex, NULL) != 0) {
			free(state);
			zend_atomic_bool_store_ex(&zend_signal_forward_started, false);
			return;
		}
		state->process = process;
		state->inherited_threads = NULL;
		state->previous = zend_signal_forward_lock_state;
		zend_signal_forward_lock_state = state;
		zend_signal_forward_mutex = &state->mutex;
	}

	/* Kept reachable, but never read: another thread may have been updating it. */
	zend_signal_forward_lock_state->inherited_threads = zend_signal_forward_threads;
	if (self) {
		/* The inherited record stays as it is, in the list that was kept reachable. */
		zend_signal_forward_thread *copy = malloc(sizeof(zend_signal_forward_thread));

		if (copy && pthread_setspecific(zend_signal_forward_key, copy) == 0) {
			*copy = *self;
			self = copy;
		} else {
			free(copy);
		}
	}
	zend_signal_forward_threads = self;
	if (self) {
		self->prev = self->next = NULL;
		self->process = process;
		zend_signal_forward_identify(self);
		/* Timers are not inherited by a child. */
		self->timer_signo = 0;
		self->has_timer_action = false;
	}

	/* A child must not share the pipe of its parent. Otherwise the pipe stays open
	 * for the lifetime of the process, as a signal handler may be writing to it. */
	if (zend_signal_forward_pipe[0] != -1 && zend_signal_forward_pipe_process != process) {
		close(zend_signal_forward_pipe[0]);
		close(zend_signal_forward_pipe[1]);
		zend_signal_forward_pipe[0] = zend_signal_forward_pipe[1] = -1;
	}
	zend_signal_forward_helper_running = false;
	zend_signal_forward_shutting_down = false;

	for (int signo = 1; signo < NSIG; signo++) {
		bool handled = zend_atomic_int_load_ex(&zend_signal_forward_handlers[signo-1]) > 0;

		zend_atomic_int_store_ex(&zend_signal_forward_pending[signo-1], 0);
		zend_atomic_int_store_ex(&zend_signal_forward_disarmed_at[signo-1], 0);
		zend_signal_forward_update(signo);
		if (handled && zend_atomic_int_load_ex(&zend_signal_forward_handlers[signo-1]) == 0) {
			struct sigaction inherited;

			if (sigaction(signo, NULL, &inherited) == 0) {
				zend_signal_forward_orphaned[signo-1] = inherited.sa_handler;
			}
		}
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

	if (zend_signal_forward_exiting) {
		return NULL;
	}

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

	zend_signal_forward_identify(self);
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
			self->installed[signo-1] = false;
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

static void zend_signal_forward_thread_exit(void *data)
{
	zend_signal_forward_thread *self = data;
	zend_signal_forward_thread *thread;
	sigset_t handled, installed, pending, blocked;
#ifdef HAVE_SETITIMER
	int timer_signo;
#endif

	/* Destructors of other keys still run, and must not register this thread again. */
	zend_signal_forward_exiting = true;

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
	sigemptyset(&installed);
	for (int signo = 1; signo < NSIG; signo++) {
		if (self->handler[signo-1]) {
			sigaddset(&handled, signo);
			if (self->installed[signo-1]) {
				sigaddset(&installed, signo);
			}
		}
	}

#ifdef HAVE_SETITIMER
	/* Nothing else would stop the timer of a thread that exits during a request. */
	timer_signo = self->timer_signo;
	if ((timer_signo == SIGALRM || timer_signo == SIGPROF) && zend_signal_forward_timer_owner(timer_signo) == self) {
		bool timer_in_use = false;

		for (thread = zend_signal_forward_threads; thread; thread = thread->next) {
			timer_in_use = timer_in_use || (thread != self && thread->timer_signo == timer_signo);
		}
		if (!timer_in_use) {
			struct itimerval no_timeout;

			memset(&no_timeout, 0, sizeof(no_timeout));
			setitimer(timer_signo == SIGALRM ? ITIMER_REAL : ITIMER_PROF, &no_timeout, NULL);
		}
	}
#endif

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

	/* While the current disposition is still in place. */
	zend_signal_forward_flush();

	/* A handler installed for this thread must not outlive it. */
	for (int signo = 1; signo < NSIG; signo++) {
		if (sigismember(&installed, signo) == 1) {
			struct sigaction default_action;

			memset(&default_action, 0, sizeof(default_action));
			default_action.sa_handler = SIG_DFL;
			sigemptyset(&default_action.sa_mask);
			zend_signal_forward_apply(signo, &default_action);
		}
	}

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
#ifndef ZEND_SIGNAL_FORWARD_ENABLED
	return;
#endif
	if (pthread_key_create(&zend_signal_forward_key, zend_signal_forward_thread_exit) != 0) {
		return;
	}
	zend_atomic_int_store_ex(&zend_signal_forward_process, 0);
	zend_signal_forward_shutting_down = false;
	zend_atomic_bool_store_ex(&zend_signal_forward_started, true);
}

void zend_signal_forward_shutdown(void)
{
	zend_signal_forward_thread *thread;

	if (!zend_atomic_bool_load_ex(&zend_signal_forward_started)) {
		return;
	}

	zend_atomic_int_store_ex(&zend_signal_forward_pid, 0);

	/* The helper goes first: a thread that exits from now on may leave its record behind. */
	if (zend_atomic_int_load_ex(&zend_signal_forward_process) == (int) getpid()) {
		bool helper_running;

		pthread_mutex_lock(zend_signal_forward_mutex);
		zend_signal_forward_helper_stop = true;
		zend_signal_forward_shutting_down = true;
		zend_atomic_int_store_ex(&zend_signal_forward_pid, 0);
		helper_running = zend_signal_forward_helper_running;
		pthread_mutex_unlock(zend_signal_forward_mutex);

		if (helper_running) {
			zend_signal_forward_helper_join();
			zend_signal_forward_helper_running = false;
		}
	}

	zend_atomic_bool_store_ex(&zend_signal_forward_started, false);

	if (zend_atomic_int_load_ex(&zend_signal_forward_process) == (int) getpid()) {
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
#ifndef ZEND_SIGNAL_FORWARD_ENABLED
	return false;
#else
	int errno_save = errno;
	bool forwarded = false;

	if (zend_signal_forward_is_trackable(signo)
	 && zend_atomic_int_load_ex(&zend_signal_forward_pid) == (int) getpid()) {
		if (zend_atomic_int_load_ex(&zend_signal_forward_targets[signo-1]) > 0) {
			int value = zend_atomic_int_load_ex(&zend_signal_forward_generation[signo-1]) + 1;
			int previous;
			bool failed = false;

#if ZEND_DEBUG
			if (zend_atomic_bool_exchange_ex(&zend_signal_forward_test_stall, false)) {
				zend_atomic_bool_store_ex(&zend_signal_forward_test_stalled, true);
				while (!zend_atomic_bool_load_ex(&zend_signal_forward_test_resume)) {
					usleep(1000);
				}
				zend_atomic_bool_store_ex(&zend_signal_forward_test_resume, false);
				zend_atomic_bool_store_ex(&zend_signal_forward_test_stalled, false);
			}
#endif
			previous = zend_atomic_int_exchange_ex(&zend_signal_forward_pending[signo-1], value);

			/* A handler that was interrupted since it read the generation must not replace
			 * the signal of the timer that runs now. */
			if (previous != 0 && previous != value
			 && previous == zend_atomic_int_load_ex(&zend_signal_forward_generation[signo-1]) + 1) {
				previous = zend_atomic_int_exchange_ex(&zend_signal_forward_pending[signo-1], previous);
			}

			if (previous == 0) {
				ssize_t written;

				do {
					written = write(zend_atomic_int_load_ex(&zend_signal_forward_fd), "", 1);
				} while (written < 0 && errno == EINTR);

				failed = written < 0 && errno != EAGAIN && errno != EWOULDBLOCK;
#if ZEND_DEBUG
				if (written < 0 && !failed) {
					zend_atomic_bool_store_ex(&zend_signal_forward_test_eagain, true);
				}
#endif
			}

			/* If the helper took the flag meanwhile, the signal is on its way. */
			forwarded = true;
			if (failed || zend_atomic_int_load_ex(&zend_signal_forward_pid) != (int) getpid()) {
				forwarded = zend_atomic_int_exchange_ex(&zend_signal_forward_pending[signo-1], 0) == 0;
			}
		} else {
			/* Ignored by a PHP thread, or the signal of a timer that was just disarmed. */
			forwarded = zend_atomic_int_load_ex(&zend_signal_forward_ignoring[signo-1]) > 0
				|| zend_signal_forward_recently_disarmed(signo);
		}
	}

	errno = errno_save;
	return forwarded;
#endif
}

ZEND_API void zend_signal_forward_or_default(int signo)
{
	/* The instruction of a fault would run again as soon as the handler returned. */
	if (!zend_signal_forward_is_trackable(signo)) {
		signal(signo, SIG_DFL);
		kill(getpid(), signo);
		return;
	}

#ifdef ZEND_SIGNAL_FORWARD_ENABLED
	if (!zend_signal_forward(signo)) {
		int errno_save = errno;

		/* A handler is registered in this process, but forwarding could not be set up.
		 * Without forwarding, the registrations of this process are not recorded at all. */
		if (zend_atomic_bool_load_ex(&zend_signal_forward_started)
		 && (zend_atomic_int_load_ex(&zend_signal_forward_process) != (int) getpid()
		  || zend_atomic_int_load_ex(&zend_signal_forward_handlers[signo-1]) == 0)) {
			signal(signo, SIG_DFL);
			kill(getpid(), signo);
		}
		errno = errno_save;
	}
#endif
}

bool zend_signal_forward_has_handler(int signo)
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
		zend_signal_forward_orphaned[signo-1] = NULL;
		self->ignored[signo-1] = false;
		self->installed[signo-1] = act != NULL;
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

ZEND_API void zend_signal_forward_mark_installed(int signo)
{
	zend_signal_forward_thread *self;

	if (!zend_signal_forward_is_trackable(signo) || !zend_signal_forward_lock()) {
		return;
	}

	self = zend_signal_forward_current();
	if (self && self->handler[signo-1]) {
		self->installed[signo-1] = true;
	}

	zend_signal_forward_unlock();
}

ZEND_API bool zend_signal_forward_suspend(void)
{
	bool running;

	if (!zend_signal_forward_lock()) {
		return false;
	}

	/* The pid stays: a signal meanwhile waits for the helper that resuming starts. */
	running = zend_signal_forward_helper_running;
	if (running) {
		zend_signal_forward_helper_stop = true;
		zend_signal_forward_helper_running = false;
	}
	zend_signal_forward_unlock();

	if (running) {
		zend_signal_forward_helper_join();
	}
	return running;
}

ZEND_API void zend_signal_forward_resume(void)
{
	if (!zend_signal_forward_lock()) {
		return;
	}

	zend_signal_forward_helper_start_if_needed();
	if (!zend_signal_forward_helper_running) {
		zend_atomic_int_store_ex(&zend_signal_forward_pid, 0);
		zend_signal_forward_scan();
	}

	zend_signal_forward_unlock();
}

ZEND_API int zend_signal_forward_release(int signo, const struct sigaction *act)
{
	zend_signal_forward_thread *self;
	bool ignore = act && act->sa_handler == SIG_IGN;
	int result = 0;

	if (!zend_signal_forward_is_trackable(signo) || !zend_signal_forward_lock()) {
		return act ? sigaction(signo, act, NULL) : 0;
	}

	self = ignore ? zend_signal_forward_self() : zend_signal_forward_current();
	if (self) {
		self->handler[signo-1] = 0;
		self->installed[signo-1] = false;
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
		struct sigaction current;
		bool runs = !self->ignored[signo-1];

		/* With the default action, the timer would end the process when it expires. */
		if (runs && act && zend_atomic_int_load_ex(&zend_signal_forward_handlers[signo-1]) == 0
		 && sigaction(signo, NULL, &current) == 0 && current.sa_handler != act->sa_handler) {
			if (current.sa_handler == SIG_DFL || current.sa_handler == SIG_IGN
			 || current.sa_handler == zend_signal_forward_orphaned[signo-1]) {
				sigaction(signo, act, NULL);
			} else {
				runs = false;
			}
		}

		zend_signal_forward_disarm_self(self);
		if (runs) {
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
	}

	zend_signal_forward_unlock();
}

ZEND_API int zend_signal_forward_timer_install(int signo, const struct sigaction *act)
{
	int result = 0;

	bool locked;

	if (!zend_signal_forward_is_trackable(signo)) {
		return sigaction(signo, act, NULL);
	}

	locked = zend_signal_forward_lock();
	if (zend_atomic_int_load_ex(&zend_signal_forward_handlers[signo-1]) == 0) {
		result = sigaction(signo, act, NULL);
	}

	if (locked) {
		zend_signal_forward_unlock();
	}
	return result;
}

ZEND_API void zend_signal_forward_timer_disarm(void)
{
	zend_signal_forward_thread *self;

	if (!zend_atomic_bool_load_ex(&zend_signal_forward_started)) {
		return;
	}

	/* A request without a running timer must not contend on the mutex. */
	self = zend_signal_forward_current();
	if (!self || !self->timer_signo || !zend_signal_forward_lock()) {
		return;
	}

	/* After a fork(), taking the lock replaced the record. */
	self = zend_signal_forward_current();
	if (self) {
		zend_signal_forward_disarm_self(self);
	}
	zend_signal_forward_unlock();
}

void zend_signal_forward_request_shutdown(void)
{
	zend_signal_forward_thread *self;
	bool cleared;

	if (!zend_atomic_bool_load_ex(&zend_signal_forward_started)) {
		return;
	}

	/* Most requests register nothing, and must not contend on the mutex. */
	self = zend_signal_forward_current();
	if (!self || !self->active || !zend_signal_forward_lock()) {
		return;
	}

	/* After a fork(), taking the lock replaced the record. */
	self = zend_signal_forward_current();
	cleared = self && zend_signal_forward_clear_self(self);
	zend_signal_forward_unlock();

	if (cleared) {
		zend_signal_forward_flush();
	}
}

ZEND_API void zend_signal_forward_fork_child(bool shares_fds)
{
	/* Closing the pipe would close it for the parent too. */
	zend_signal_forward_shared_fds = shares_fds;
	if (shares_fds) {
		zend_signal_forward_pipe[0] = zend_signal_forward_pipe[1] = -1;
	}

	if (!zend_signal_forward_lock()) {
		return;
	}

	zend_signal_forward_helper_start_if_needed();

	zend_signal_forward_unlock();
}

#if ZEND_DEBUG
ZEND_API int zend_signal_forward_test(zend_signal_forward_test_op op)
{
	int result = 1;
	int write_fd;

	if (op == ZEND_SIGNAL_FORWARD_TEST_RESTART) {
		zend_signal_forward_shutdown();
		zend_signal_forward_startup();
		return 1;
	}

	switch (op) {
		case ZEND_SIGNAL_FORWARD_TEST_UNLOCK:
			zend_signal_forward_unlock();
			return 1;
		case ZEND_SIGNAL_FORWARD_TEST_STALL_HANDLER:
			zend_atomic_bool_store_ex(&zend_signal_forward_test_stall, true);
			return 1;
		case ZEND_SIGNAL_FORWARD_TEST_WAIT_STALLED:
			for (time_t start = time(NULL); !zend_atomic_bool_load_ex(&zend_signal_forward_test_stalled); usleep(1000)) {
				if (time(NULL) - start > 30) {
					return 0;
				}
			}
			return 1;
		case ZEND_SIGNAL_FORWARD_TEST_RESUME_HANDLER:
			zend_atomic_bool_store_ex(&zend_signal_forward_test_resume, true);
			for (time_t start = time(NULL); zend_atomic_bool_load_ex(&zend_signal_forward_test_stalled); usleep(1000)) {
				if (time(NULL) - start > 30) {
					return 0;
				}
			}
			return 1;
		case ZEND_SIGNAL_FORWARD_TEST_FAIL_PIPE:
			zend_atomic_bool_store_ex(&zend_signal_forward_test_fail_pipe, true);
			return 1;
		case ZEND_SIGNAL_FORWARD_TEST_FAIL_CREATE:
			zend_atomic_bool_store_ex(&zend_signal_forward_test_fail_create, true);
			return 1;
		case ZEND_SIGNAL_FORWARD_TEST_FAIL_INIT:
			zend_atomic_bool_store_ex(&zend_signal_forward_test_fail_init, true);
			return 1;
		default:
			break;
	}
	if (op == ZEND_SIGNAL_FORWARD_TEST_PID) {
		return zend_atomic_int_load_ex(&zend_signal_forward_pid);
	}

	if (!zend_signal_forward_lock()) {
		return 0;
	}
	if (op == ZEND_SIGNAL_FORWARD_TEST_LOCK) {
		return 1;
	}
	write_fd = zend_signal_forward_pipe[1];
	if (op == ZEND_SIGNAL_FORWARD_TEST_RUNNING) {
		result = zend_signal_forward_helper_running;
	} else if (op == ZEND_SIGNAL_FORWARD_TEST_READ_FD) {
		result = zend_signal_forward_pipe[0];
	} else if (op == ZEND_SIGNAL_FORWARD_TEST_WRITE_FD) {
		result = write_fd;
	}
	zend_signal_forward_unlock();

	switch (op) {
		case ZEND_SIGNAL_FORWARD_TEST_HOLD:
			zend_atomic_bool_store_ex(&zend_signal_forward_test_hold, true);
			while (write(write_fd, "", 1) < 0 && errno == EINTR);
			while (!zend_atomic_bool_load_ex(&zend_signal_forward_test_held)) {
				usleep(1000);
			}
			break;
		case ZEND_SIGNAL_FORWARD_TEST_RELEASE:
			zend_atomic_bool_store_ex(&zend_signal_forward_test_hold, false);
			while (zend_atomic_bool_load_ex(&zend_signal_forward_test_held)) {
				usleep(1000);
			}
			break;
		case ZEND_SIGNAL_FORWARD_TEST_SATURATE:
			while (write(write_fd, "", 1) == 1);
			result = errno == EAGAIN;
			break;
		case ZEND_SIGNAL_FORWARD_TEST_FAIL_READ:
			zend_atomic_bool_store_ex(&zend_signal_forward_test_fail_read, true);
			break;
		case ZEND_SIGNAL_FORWARD_TEST_HANDLER_EAGAIN:
			result = zend_atomic_bool_exchange_ex(&zend_signal_forward_test_eagain, false);
			break;
		default:
			break;
	}
	return result;
}
#endif

#endif /* defined(ZTS) && !defined(ZEND_WIN32) */
