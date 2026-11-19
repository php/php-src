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
*/

#include "signal_forwarding.h"

#if defined(ZTS) && !defined(PHP_WIN32)

#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "zend_atomic.h"
#include "zend_signal.h"

static void *zend_test_raise_thread(void *arg)
{
	raise((int) (intptr_t) arg);
	return NULL;
}

bool zend_test_signal_forwarding_raise(int signo)
{
	pthread_t thread;

	if (pthread_create(&thread, NULL, zend_test_raise_thread, (void *) (intptr_t) signo) != 0) {
		return false;
	}
	pthread_join(thread, NULL);
	return true;
}

/* Drives the record of which threads handle a signal, without running PHP code
 * on the other threads. */
/* Ignored by default, so that a forward arriving after a scenario is harmless. */
#define ZEND_TEST_FORWARD_SIGNAL SIGURG
#define ZEND_TEST_FORWARD_WORKERS 4

enum {
	ZEND_TEST_FORWARD_IDLE,
	ZEND_TEST_FORWARD_RECORD,
	ZEND_TEST_FORWARD_RECORD_TIMER,
	ZEND_TEST_FORWARD_FORGET,
	ZEND_TEST_FORWARD_RAISE,
	ZEND_TEST_FORWARD_BLOCK,
	ZEND_TEST_FORWARD_WAIT_PENDING,
	ZEND_TEST_FORWARD_EXIT
};

static pthread_t zend_test_forward_main;
static pthread_t zend_test_forward_helper;
static zend_atomic_bool zend_test_forward_helper_set;
static zend_atomic_bool zend_test_forward_helper_registered;
static zend_atomic_int zend_test_forward_main_hits;
static zend_atomic_int zend_test_forward_helper_hits;
static zend_atomic_int zend_test_forward_command;
static zend_atomic_bool zend_test_forward_stop;
static zend_atomic_bool zend_test_forward_result;
static zend_atomic_int zend_test_forward_replacement_hits;

/* Polls instead of sleeping, so that a passing run does not depend on timing. */
static bool zend_test_forward_wait_pending(void)
{
	time_t start = time(NULL);

	/* Measured in time, as signals interrupt the sleep. */
	while (time(NULL) - start < 30) {
		sigset_t pending;

		sigpending(&pending);
		if (sigismember(&pending, ZEND_TEST_FORWARD_SIGNAL) == 1) {
			return true;
		}
		usleep(1000);
	}
	return false;
}

static void zend_test_forward_replacement_handler(int signo)
{
	zend_atomic_int_store(&zend_test_forward_replacement_hits, zend_atomic_int_load(&zend_test_forward_replacement_hits) + 1);
}

static void zend_test_forward_handler(int signo)
{
	pthread_t self = pthread_self();

	if (pthread_equal(self, zend_test_forward_main)) {
		zend_atomic_int_store(&zend_test_forward_main_hits, zend_atomic_int_load(&zend_test_forward_main_hits) + 1);
	} else if (zend_atomic_bool_load(&zend_test_forward_helper_set) && pthread_equal(self, zend_test_forward_helper)
	 && zend_atomic_bool_load(&zend_test_forward_helper_registered)) {
		zend_atomic_int_store(&zend_test_forward_helper_hits, zend_atomic_int_load(&zend_test_forward_helper_hits) + 1);
	} else {
		zend_atomic_bool_store(&zend_test_forward_result, zend_signal_forward(signo));
	}
}

static void *zend_test_forward_helper_main(void *arg)
{
	while (1) {
		int command = zend_atomic_int_load(&zend_test_forward_command);

		if (command == ZEND_TEST_FORWARD_IDLE) {
			usleep(100);
			continue;
		}
		if (command == ZEND_TEST_FORWARD_EXIT) {
			return NULL;
		}
		if (command == ZEND_TEST_FORWARD_RECORD) {
			zend_atomic_bool_store(&zend_test_forward_helper_registered, true);
			zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		} else if (command == ZEND_TEST_FORWARD_RECORD_TIMER) {
			zend_signal_forward_timer_arm(ZEND_TEST_FORWARD_SIGNAL, NULL);
		} else if (command == ZEND_TEST_FORWARD_FORGET) {
			zend_atomic_bool_store(&zend_test_forward_helper_registered, false);
			zend_signal_forward_release(ZEND_TEST_FORWARD_SIGNAL, NULL);
		} else if (command == ZEND_TEST_FORWARD_RAISE) {
			raise(ZEND_TEST_FORWARD_SIGNAL);
		} else if (command == ZEND_TEST_FORWARD_BLOCK) {
			sigset_t block;

			sigemptyset(&block);
			sigaddset(&block, ZEND_TEST_FORWARD_SIGNAL);
			pthread_sigmask(SIG_BLOCK, &block, NULL);
		} else if (command == ZEND_TEST_FORWARD_WAIT_PENDING) {
			zend_test_forward_wait_pending();
		}
		zend_atomic_int_store(&zend_test_forward_command, ZEND_TEST_FORWARD_IDLE);
	}
}

static void zend_test_forward_send(int command)
{
	zend_atomic_int_store(&zend_test_forward_command, command);
	while (zend_atomic_int_load(&zend_test_forward_command) != ZEND_TEST_FORWARD_IDLE) {
		usleep(100);
	}
}

static void zend_test_forward_helper_start(void)
{
	zend_atomic_int_store(&zend_test_forward_command, ZEND_TEST_FORWARD_IDLE);
	pthread_create(&zend_test_forward_helper, NULL, zend_test_forward_helper_main, NULL);
	zend_atomic_bool_store(&zend_test_forward_helper_set, true);
}

/* Returns once the thread has exited, which removes its record. */
static void zend_test_forward_helper_exit(void)
{
	zend_atomic_int_store(&zend_test_forward_command, ZEND_TEST_FORWARD_EXIT);
	pthread_join(zend_test_forward_helper, NULL);
	zend_atomic_bool_store(&zend_test_forward_helper_set, false);
	zend_atomic_bool_store(&zend_test_forward_helper_registered, false);
}

static void *zend_test_forward_raise_main(void *arg)
{
	sigset_t unblock;

	/* A new thread inherits the signal mask of its creator. */
	sigemptyset(&unblock);
	sigaddset(&unblock, ZEND_TEST_FORWARD_SIGNAL);
	pthread_sigmask(SIG_UNBLOCK, &unblock, NULL);

	raise(ZEND_TEST_FORWARD_SIGNAL);
	return NULL;
}

static void zend_test_forward_raise_foreign(void)
{
	pthread_t thread;

	pthread_create(&thread, NULL, zend_test_forward_raise_main, NULL);
	pthread_join(thread, NULL);
}

static bool zend_test_forward_wait(zend_atomic_int *hits)
{
	time_t start = time(NULL);

	while (time(NULL) - start < 30) {
		if (zend_atomic_int_load(hits) > 0) {
			return true;
		}
		usleep(1000);
	}
	return false;
}

static void *zend_test_forward_worker_main(void *arg)
{
	for (int i = 0; i < 2000; i++) {
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_signal_forward_release(ZEND_TEST_FORWARD_SIGNAL, NULL);
	}
	return NULL;
}

static void *zend_test_forward_short_lived_main(void *arg)
{
	/* The record is removed when the thread exits. */
	zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
	return NULL;
}

static void *zend_test_forward_storm_main(void *arg)
{
	while (!zend_atomic_bool_load(&zend_test_forward_stop)) {
		raise(ZEND_TEST_FORWARD_SIGNAL);
		usleep(50);
	}
	return NULL;
}

static void *zend_test_forward_churn_main(void *arg)
{
	while (!zend_atomic_bool_load(&zend_test_forward_stop)) {
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_signal_forward_release(ZEND_TEST_FORWARD_SIGNAL, NULL);
	}
	return NULL;
}

#if ZEND_DEBUG
static void zend_test_forward_exit_marker(int signo)
{
}

static pthread_key_t zend_test_forward_exit_key;

static void zend_test_forward_exit_destructor(void *arg)
{
	zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
#ifndef PTHREAD_DESTRUCTOR_ITERATIONS
# define PTHREAD_DESTRUCTOR_ITERATIONS 4
#endif
	/* Runs again on every pass that the system makes over the destructors, and leaves
	 * nothing behind after the last one. */
	if ((intptr_t) arg < PTHREAD_DESTRUCTOR_ITERATIONS - 1) {
		pthread_setspecific(zend_test_forward_exit_key, (void *) ((intptr_t) arg + 1));
	}
}

static void *zend_test_forward_timer_install_main(void *arg)
{
	zend_signal_forward_timer_install(ZEND_TEST_FORWARD_SIGNAL, arg);
	return NULL;
}

static void *zend_test_forward_exit_main(void *arg)
{
	pthread_setspecific(zend_test_forward_exit_key, (void *) 1);
	return NULL;
}
#endif

/* A hang in a child fails the test instead of stalling it. */
static bool zend_test_forward_wait_child(pid_t pid)
{
	time_t start = time(NULL);

	while (time(NULL) - start < 60) {
		int status = 0;
		pid_t result = waitpid(pid, &status, WNOHANG);

		if (result == pid) {
			return WIFEXITED(status) && WEXITSTATUS(status) == 0;
		}
		if (result < 0) {
			return false;
		}
		usleep(1000);
	}
	kill(pid, SIGKILL);
	waitpid(pid, NULL, 0);
	return false;
}

/* Forwarding must work in the child, where only the calling thread exists. */
static bool zend_test_forward_child_delivers(void)
{
	zend_test_forward_main = pthread_self();
	zend_atomic_bool_store(&zend_test_forward_helper_set, false);
	zend_atomic_int_store(&zend_test_forward_main_hits, 0);
	zend_signal_forward_fork_child(false);
	zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
	zend_test_forward_raise_foreign();
	return zend_test_forward_wait(&zend_test_forward_main_hits);
}

static bool zend_test_forward_fork_and_check(void)
{
	pid_t pid = fork();

	if (pid == 0) {
		_exit(zend_test_forward_child_delivers() ? 0 : 1);
	}
	return pid > 0 && zend_test_forward_wait_child(pid);
}

static bool zend_test_forward_fork_chain(int depth)
{
	pid_t pid = fork();

	if (pid == 0) {
		bool result = zend_test_forward_child_delivers();

		_exit(result && (depth == 0 || zend_test_forward_fork_chain(depth - 1)) ? 0 : 1);
	}
	return pid > 0 && zend_test_forward_wait_child(pid);
}

static bool zend_test_forward_scenario(const char *scenario)
{
	if (strcmp(scenario, "unmanaged") == 0) {
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_raise_foreign();
		return zend_test_forward_wait(&zend_test_forward_main_hits);
	}

	if (strcmp(scenario, "successor") == 0) {
		bool result;

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_helper_start();
		zend_test_forward_send(ZEND_TEST_FORWARD_RECORD);
		zend_test_forward_send(ZEND_TEST_FORWARD_FORGET);
		zend_test_forward_raise_foreign();
		result = zend_test_forward_wait(&zend_test_forward_main_hits);
		zend_test_forward_helper_exit();
		return result;
	}

	if (strcmp(scenario, "most_recent_owner") == 0) {
		bool result;

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_helper_start();
		zend_test_forward_send(ZEND_TEST_FORWARD_RECORD);
		zend_test_forward_raise_foreign();
		result = zend_test_forward_wait(&zend_test_forward_helper_hits)
			&& zend_atomic_int_load(&zend_test_forward_main_hits) == 0;
		zend_test_forward_helper_exit();
		return result;
	}

	if (strcmp(scenario, "owner_exit") == 0) {
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_helper_start();
		zend_test_forward_send(ZEND_TEST_FORWARD_RECORD);
		zend_test_forward_helper_exit();
		zend_test_forward_raise_foreign();
		return zend_test_forward_wait(&zend_test_forward_main_hits);
	}

	if (strcmp(scenario, "last_owner_exit") == 0) {
		zend_test_forward_helper_start();
		zend_test_forward_send(ZEND_TEST_FORWARD_RECORD);
		zend_test_forward_helper_exit();
		zend_test_forward_raise_foreign();
		usleep(50000);
		return zend_atomic_int_load(&zend_test_forward_main_hits) == 0
			&& zend_atomic_int_load(&zend_test_forward_helper_hits) == 0;
	}

	if (strcmp(scenario, "thread_without_handler") == 0) {
		bool result;

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_helper_start();
		zend_test_forward_send(ZEND_TEST_FORWARD_RAISE);
		result = zend_test_forward_wait(&zend_test_forward_main_hits);
		zend_test_forward_helper_exit();
		return result;
	}

	if (strcmp(scenario, "concurrent_registration") == 0) {
		pthread_t workers[ZEND_TEST_FORWARD_WORKERS], storm;

		zend_atomic_bool_store(&zend_test_forward_stop, false);
		pthread_create(&storm, NULL, zend_test_forward_storm_main, NULL);
		for (int i = 0; i < ZEND_TEST_FORWARD_WORKERS; i++) {
			pthread_create(&workers[i], NULL, zend_test_forward_worker_main, NULL);
		}
		for (int i = 0; i < ZEND_TEST_FORWARD_WORKERS; i++) {
			pthread_join(workers[i], NULL);
		}
		zend_atomic_bool_store(&zend_test_forward_stop, true);
		pthread_join(storm, NULL);

		zend_atomic_int_store(&zend_test_forward_main_hits, 0);
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_raise_foreign();
		return zend_test_forward_wait(&zend_test_forward_main_hits);
	}

	if (strcmp(scenario, "timer_owner") == 0) {
		bool result;

		zend_signal_forward_timer_arm(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_raise_foreign();
		result = zend_test_forward_wait(&zend_test_forward_main_hits);
		zend_signal_forward_timer_disarm();
		return result;
	}

	if (strcmp(scenario, "handler_outranks_timer") == 0) {
		bool result;

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_helper_start();
		zend_test_forward_send(ZEND_TEST_FORWARD_RECORD_TIMER);
		zend_test_forward_raise_foreign();
		result = zend_test_forward_wait(&zend_test_forward_main_hits);
		zend_test_forward_helper_exit();
		return result;
	}

	if (strcmp(scenario, "stale_timer") == 0) {
		zend_signal_forward_timer_arm(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_signal_forward_timer_disarm();
		zend_atomic_bool_store(&zend_test_forward_result, false);
		zend_test_forward_raise_foreign();
		/* The signal is consumed, and reaches nobody. */
		return zend_atomic_bool_load(&zend_test_forward_result)
			&& zend_atomic_int_load(&zend_test_forward_main_hits) == 0;
	}

	if (strcmp(scenario, "repeated_signals") == 0) {
		sigset_t block, old;
		bool result;

		sigemptyset(&block);
		sigaddset(&block, ZEND_TEST_FORWARD_SIGNAL);
		pthread_sigmask(SIG_BLOCK, &block, &old);
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		for (int i = 0; i < 100; i++) {
			zend_test_forward_raise_foreign();
		}
		result = zend_test_forward_wait_pending();
		pthread_sigmask(SIG_SETMASK, &old, NULL);
		result = zend_test_forward_wait(&zend_test_forward_main_hits) && result;
		/* Repeated signals coalesce. */
		return result && zend_atomic_int_load(&zend_test_forward_main_hits) < 100;
	}

	if (strcmp(scenario, "replaced_handler") == 0) {
		struct sigaction replacement;
		sigset_t block, old;
		bool result;

		sigemptyset(&block);
		sigaddset(&block, ZEND_TEST_FORWARD_SIGNAL);
		pthread_sigmask(SIG_BLOCK, &block, &old);
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_raise_foreign();
		result = zend_test_forward_wait_pending();

		memset(&replacement, 0, sizeof(replacement));
		replacement.sa_handler = zend_test_forward_replacement_handler;
		sigfillset(&replacement.sa_mask);
		sigaction(ZEND_TEST_FORWARD_SIGNAL, &replacement, NULL);
		pthread_sigmask(SIG_SETMASK, &old, NULL);

		return zend_test_forward_wait(&zend_test_forward_replacement_hits) && result
			&& zend_atomic_int_load(&zend_test_forward_main_hits) == 0;
	}

	if (strcmp(scenario, "owner_exit_with_pending_signal") == 0) {
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_helper_start();
		zend_test_forward_send(ZEND_TEST_FORWARD_BLOCK);
		zend_test_forward_send(ZEND_TEST_FORWARD_RECORD);
		zend_test_forward_raise_foreign();
		zend_test_forward_send(ZEND_TEST_FORWARD_WAIT_PENDING);
		zend_test_forward_helper_exit();
		return zend_test_forward_wait(&zend_test_forward_main_hits);
	}

	if (strcmp(scenario, "blocked_owner_no_failover") == 0) {
		sigset_t block, previous;
		bool result;

		/* Another thread handles the signal too, but the owner registered last. */
		zend_test_forward_helper_start();
		zend_test_forward_send(ZEND_TEST_FORWARD_RECORD);
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);

		/* The signal waits for the owner, as it would for a thread that received it directly. */
		sigemptyset(&block);
		sigaddset(&block, ZEND_TEST_FORWARD_SIGNAL);
		pthread_sigmask(SIG_BLOCK, &block, &previous);
		zend_test_forward_raise_foreign();
		result = zend_test_forward_wait_pending();
		usleep(100000);
		result = result && zend_atomic_int_load(&zend_test_forward_main_hits) == 0
			&& zend_atomic_int_load(&zend_test_forward_helper_hits) == 0;
		pthread_sigmask(SIG_SETMASK, &previous, NULL);

		result = zend_test_forward_wait(&zend_test_forward_main_hits) && result;
		zend_test_forward_helper_exit();
		return result && zend_atomic_int_load(&zend_test_forward_helper_hits) == 0;
	}

	if (strcmp(scenario, "thread_identifier_reuse") == 0) {
		pthread_t storm;

		zend_atomic_bool_store(&zend_test_forward_stop, false);
		pthread_create(&storm, NULL, zend_test_forward_storm_main, NULL);
		for (int i = 0; i < 500; i++) {
			pthread_t thread;

			pthread_create(&thread, NULL, zend_test_forward_short_lived_main, NULL);
			pthread_join(thread, NULL);
		}
		zend_atomic_bool_store(&zend_test_forward_stop, true);
		pthread_join(storm, NULL);

		zend_atomic_int_store(&zend_test_forward_main_hits, 0);
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_raise_foreign();
		return zend_test_forward_wait(&zend_test_forward_main_hits);
	}

	if (strcmp(scenario, "fork_during_registration") == 0) {
		pthread_t workers[ZEND_TEST_FORWARD_WORKERS];
		bool result = true;

		zend_atomic_bool_store(&zend_test_forward_stop, false);
		for (int i = 0; i < ZEND_TEST_FORWARD_WORKERS; i++) {
			pthread_create(&workers[i], NULL, zend_test_forward_churn_main, NULL);
		}
		for (int i = 0; i < 50 && result; i++) {
			result = zend_test_forward_fork_and_check();
		}
		zend_atomic_bool_store(&zend_test_forward_stop, true);
		for (int i = 0; i < ZEND_TEST_FORWARD_WORKERS; i++) {
			pthread_join(workers[i], NULL);
		}
		return result;
	}

	if (strcmp(scenario, "fork_during_notification") == 0) {
		pthread_t storm;
		bool result = true;

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_atomic_bool_store(&zend_test_forward_stop, false);
		pthread_create(&storm, NULL, zend_test_forward_storm_main, NULL);
		for (int i = 0; i < 50 && result; i++) {
			result = zend_test_forward_fork_and_check();
		}
		zend_atomic_bool_store(&zend_test_forward_stop, true);
		pthread_join(storm, NULL);
		return result;
	}

	if (strcmp(scenario, "repeated_fork") == 0) {
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		return zend_test_forward_fork_chain(3);
	}

	if (strcmp(scenario, "timer_not_inherited") == 0) {
		pid_t pid;
		bool result;

		zend_signal_forward_timer_arm(ZEND_TEST_FORWARD_SIGNAL, NULL);
		pid = fork();
		if (pid == 0) {
			zend_test_forward_main = pthread_self();
			zend_signal_forward_fork_child(false);
			zend_atomic_bool_store(&zend_test_forward_result, true);
			zend_test_forward_raise_foreign();
			_exit(!zend_atomic_bool_load(&zend_test_forward_result)
				&& zend_atomic_int_load(&zend_test_forward_main_hits) == 0 ? 0 : 1);
		}
		result = pid > 0 && zend_test_forward_wait_child(pid);
		zend_signal_forward_timer_disarm();
		return result;
	}

	if (strcmp(scenario, "fork") == 0) {
		pid_t pid;
		int status = 0;

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_helper_start();
		zend_test_forward_send(ZEND_TEST_FORWARD_RECORD);

		pid = fork();
		if (pid == 0) {
			zend_signal_forward_fork_child(false);
			zend_atomic_bool_store(&zend_test_forward_helper_set, false);
			zend_test_forward_raise_foreign();
			_exit(zend_test_forward_wait(&zend_test_forward_main_hits) ? 0 : 1);
		}

		waitpid(pid, &status, 0);
		zend_test_forward_helper_exit();
		return pid > 0 && WIFEXITED(status) && WEXITSTATUS(status) == 0;
	}

#if ZEND_DEBUG
	if (strcmp(scenario, "handler_stalled_across_helper_exit") == 0) {
		pthread_t raiser;
		time_t start = time(NULL);
		bool stopped = false;

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_STALL_HANDLER);
		pthread_create(&raiser, NULL, zend_test_forward_raise_main, NULL);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_WAIT_STALLED);

		/* The helper exits after the handler checked that it runs, and cannot start again. */
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_FAIL_PIPE);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_FAIL_READ);
		zend_test_forward_raise_foreign();
		while (!stopped && time(NULL) - start < 30) {
			stopped = zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RUNNING) == 0;
			usleep(1000);
		}
		stopped = zend_test_forward_wait(&zend_test_forward_main_hits) && stopped;

		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RESUME_HANDLER);
		pthread_join(raiser, NULL);

		/* Not reported as forwarded, and not delivered to the next helper either. */
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		usleep(100000);
		return stopped && !zend_atomic_bool_load(&zend_test_forward_result)
			&& zend_atomic_int_load(&zend_test_forward_main_hits) == 1;
	}

	if (strcmp(scenario, "fork_inherited_handler") == 0) {
		struct sigaction handler, timer;
		pid_t pid;

		memset(&handler, 0, sizeof(handler));
		handler.sa_handler = zend_test_forward_handler;
		sigemptyset(&handler.sa_mask);
		memset(&timer, 0, sizeof(timer));
		timer.sa_handler = zend_test_forward_replacement_handler;
		sigemptyset(&timer.sa_mask);

		/* The thread that handles the signal does not exist in the child. */
		zend_test_forward_helper_start();
		zend_test_forward_send(ZEND_TEST_FORWARD_RECORD);
		sigaction(ZEND_TEST_FORWARD_SIGNAL, &handler, NULL);
		pid = fork();
		if (pid == 0) {
			struct sigaction current;
			pid_t grandchild;
			int status = 1;

			zend_signal_forward_fork_child(false);
			/* It stays orphaned in the child of this child. */
			grandchild = fork();
			if (grandchild == 0) {
				zend_signal_forward_fork_child(false);
				zend_signal_forward_timer_arm(ZEND_TEST_FORWARD_SIGNAL, &timer);
				sigaction(ZEND_TEST_FORWARD_SIGNAL, NULL, &current);
				_exit(current.sa_handler == zend_test_forward_replacement_handler ? 0 : 1);
			}
			waitpid(grandchild, &status, 0);
			if (grandchild < 0 || !WIFEXITED(status) || WEXITSTATUS(status) != 0) {
				_exit(1);
			}
			zend_signal_forward_timer_arm(ZEND_TEST_FORWARD_SIGNAL, &timer);
			sigaction(ZEND_TEST_FORWARD_SIGNAL, NULL, &current);
			if (current.sa_handler != zend_test_forward_replacement_handler) {
				_exit(1);
			}
			zend_signal_forward_timer_disarm();

			/* A handler that something else installed afterwards is not that handler. */
			sigaction(ZEND_TEST_FORWARD_SIGNAL, &handler, NULL);
			handler.sa_handler = zend_test_forward_exit_marker;
			sigaction(ZEND_TEST_FORWARD_SIGNAL, &handler, NULL);
			zend_signal_forward_timer_arm(ZEND_TEST_FORWARD_SIGNAL, &timer);
			sigaction(ZEND_TEST_FORWARD_SIGNAL, NULL, &current);
			_exit(current.sa_handler == zend_test_forward_exit_marker ? 0 : 1);
		}
		zend_test_forward_helper_exit();
		return pid > 0 && zend_test_forward_wait_child(pid);
	}

	if (strcmp(scenario, "handler_stalled_across_rearm") == 0) {
		pthread_t raiser;
		bool result;

		zend_signal_forward_timer_arm(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_HOLD);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_STALL_HANDLER);
		pthread_create(&raiser, NULL, zend_test_forward_raise_main, NULL);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_WAIT_STALLED);

		/* The new timer expires while a handler of the old one is still running. */
		zend_signal_forward_timer_arm(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_raise_foreign();
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RESUME_HANDLER);
		pthread_join(raiser, NULL);

		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RELEASE);
		result = zend_test_forward_wait(&zend_test_forward_main_hits);
		zend_signal_forward_timer_disarm();
		return result;
	}

	if (strcmp(scenario, "fork_with_pending_flag") == 0) {
		pid_t pid;
		bool result;

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_HOLD);
		zend_test_forward_raise_foreign();
		pid = fork();
		if (pid == 0) {
			/* That signal was sent to the parent. */
			zend_test_forward_main = pthread_self();
			zend_atomic_int_store(&zend_test_forward_main_hits, 0);
			zend_signal_forward_fork_child(false);
			usleep(200000);
			_exit(zend_atomic_int_load(&zend_test_forward_main_hits) == 0 ? 0 : 1);
		}
		result = pid > 0 && zend_test_forward_wait_child(pid);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RELEASE);
		return zend_test_forward_wait(&zend_test_forward_main_hits) && result;
	}

	if (strcmp(scenario, "saturation_storm") == 0) {
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		for (int round = 0; round < 20; round++) {
			pthread_t raisers[ZEND_TEST_FORWARD_WORKERS];

			zend_atomic_int_store(&zend_test_forward_main_hits, 0);
			zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_HOLD);
			zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_SATURATE);
			for (int i = 0; i < ZEND_TEST_FORWARD_WORKERS; i++) {
				pthread_create(&raisers[i], NULL, zend_test_forward_raise_main, NULL);
			}
			for (int i = 0; i < ZEND_TEST_FORWARD_WORKERS; i++) {
				pthread_join(raisers[i], NULL);
			}
			zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RELEASE);
			if (!zend_test_forward_wait(&zend_test_forward_main_hits)) {
				return false;
			}
		}
		return true;
	}

	if (strcmp(scenario, "timer_keeps_handler") == 0) {
		struct sigaction handler, timer, current;
		pthread_t installer;
		bool result;

		memset(&handler, 0, sizeof(handler));
		handler.sa_handler = zend_test_forward_handler;
		sigemptyset(&handler.sa_mask);
		memset(&timer, 0, sizeof(timer));
		timer.sa_handler = zend_test_forward_replacement_handler;
		sigemptyset(&timer.sa_mask);

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, &handler);
		zend_signal_forward_timer_install(ZEND_TEST_FORWARD_SIGNAL, &timer);
		sigaction(ZEND_TEST_FORWARD_SIGNAL, NULL, &current);
		result = current.sa_handler == zend_test_forward_handler;

		zend_signal_forward_release(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_signal_forward_timer_install(ZEND_TEST_FORWARD_SIGNAL, &timer);
		sigaction(ZEND_TEST_FORWARD_SIGNAL, NULL, &current);
		result = result && current.sa_handler == zend_test_forward_replacement_handler;

		/* The disposition only changes under the mutex. */
		sigaction(ZEND_TEST_FORWARD_SIGNAL, &handler, NULL);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_LOCK);
		pthread_create(&installer, NULL, zend_test_forward_timer_install_main, &timer);
		usleep(100000);
		sigaction(ZEND_TEST_FORWARD_SIGNAL, NULL, &current);
		result = result && current.sa_handler == zend_test_forward_handler;
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_UNLOCK);
		pthread_join(installer, NULL);
		sigaction(ZEND_TEST_FORWARD_SIGNAL, NULL, &current);
		return result && current.sa_handler == zend_test_forward_replacement_handler;
	}

	if (strcmp(scenario, "timer_arm_disposition") == 0) {
		struct sigaction timer, other, current;
		bool result;

		memset(&timer, 0, sizeof(timer));
		timer.sa_handler = zend_test_forward_replacement_handler;
		sigemptyset(&timer.sa_mask);
		memset(&other, 0, sizeof(other));
		other.sa_handler = SIG_DFL;
		sigemptyset(&other.sa_mask);

		/* A running timer must not meet the default action. */
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_signal_forward_release(ZEND_TEST_FORWARD_SIGNAL, &other);
		zend_signal_forward_timer_arm(ZEND_TEST_FORWARD_SIGNAL, &timer);
		sigaction(ZEND_TEST_FORWARD_SIGNAL, NULL, &current);
		result = current.sa_handler == zend_test_forward_replacement_handler;
		zend_signal_forward_timer_disarm();

		/* A handler that something else installed stays. */
		other.sa_handler = zend_test_forward_handler;
		sigaction(ZEND_TEST_FORWARD_SIGNAL, &other, NULL);
		zend_signal_forward_timer_arm(ZEND_TEST_FORWARD_SIGNAL, &timer);
		sigaction(ZEND_TEST_FORWARD_SIGNAL, NULL, &current);
		result = result && current.sa_handler == zend_test_forward_handler;
		zend_signal_forward_timer_disarm();

		/* So does a signal that this thread ignores. */
		other.sa_handler = SIG_IGN;
		zend_signal_forward_release(ZEND_TEST_FORWARD_SIGNAL, &other);
		zend_signal_forward_timer_arm(ZEND_TEST_FORWARD_SIGNAL, &timer);
		sigaction(ZEND_TEST_FORWARD_SIGNAL, NULL, &current);
		zend_signal_forward_timer_disarm();
		return result && current.sa_handler == SIG_IGN;
	}

	if (strcmp(scenario, "register_during_thread_exit") == 0) {
		pthread_t thread;

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		pthread_key_create(&zend_test_forward_exit_key, zend_test_forward_exit_destructor);
		pthread_create(&thread, NULL, zend_test_forward_exit_main, NULL);
		pthread_join(thread, NULL);
		pthread_key_delete(zend_test_forward_exit_key);

		/* No record may be left of the thread that registered while it exited. */
		zend_test_forward_raise_foreign();
		return zend_test_forward_wait(&zend_test_forward_main_hits);
	}

	if (strcmp(scenario, "helper_start_failure") == 0) {
		pid_t pid;
		bool result;

		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RESTART);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_FAIL_CREATE);
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		result = zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RUNNING) == 0
			&& zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_PID) == 0;

		/* Without a helper, a signal is not reported as forwarded. */
		zend_atomic_bool_store(&zend_test_forward_result, true);
		zend_test_forward_raise_foreign();
		result = result && !zend_atomic_bool_load(&zend_test_forward_result);

		/* The next registration starts over. */
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_raise_foreign();
		result = zend_test_forward_wait(&zend_test_forward_main_hits) && result;

		pid = fork();
		if (pid == 0) {
			bool running;

			zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_FAIL_PIPE);
			zend_signal_forward_fork_child(false);
			running = zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RUNNING) == 1;
			_exit(!running && zend_test_forward_child_delivers() ? 0 : 1);
		}
		return pid > 0 && zend_test_forward_wait_child(pid) && result;
	}

	if (strcmp(scenario, "init_failure") == 0) {
		struct sigaction current;
		pid_t pid;

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		pid = fork();
		if (pid == 0) {
			bool handled;

			zend_test_forward_main = pthread_self();
			zend_atomic_int_store(&zend_test_forward_main_hits, 0);
			zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_FAIL_INIT);
			zend_signal_forward_fork_child(false);
			zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
			zend_atomic_bool_store(&zend_test_forward_result, true);
			zend_test_forward_raise_foreign();
			usleep(100000);
			/* A handler stays in place, including one registered after the failure. */
			memset(&current, 0, sizeof(current));
			current.sa_handler = zend_test_forward_handler;
			sigemptyset(&current.sa_mask);
			zend_signal_forward_acquire(SIGWINCH, &current);
			zend_signal_forward_or_default(SIGWINCH);
			zend_signal_forward_or_default(ZEND_TEST_FORWARD_SIGNAL);
			sigaction(SIGWINCH, NULL, &current);
			handled = current.sa_handler == zend_test_forward_handler;
			sigaction(ZEND_TEST_FORWARD_SIGNAL, NULL, &current);
			_exit(handled && !zend_atomic_bool_load(&zend_test_forward_result)
				&& current.sa_handler == zend_test_forward_handler
				&& zend_atomic_int_load(&zend_test_forward_main_hits) == 0 ? 0 : 1);
		}
		return pid > 0 && zend_test_forward_wait_child(pid);
	}

	if (strcmp(scenario, "pipe_saturation") == 0) {
		bool result;

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_HOLD);
		result = zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_SATURATE) == 1;
		zend_test_forward_raise_foreign();
		result = zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_HANDLER_EAGAIN) == 1 && result;
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RELEASE);
		result = zend_test_forward_wait(&zend_test_forward_main_hits) && result;
		usleep(100000);
		return result && zend_atomic_int_load(&zend_test_forward_main_hits) == 1;
	}

	if (strcmp(scenario, "read_failure_keeps_signal") == 0) {
		time_t start = time(NULL);
		bool replaced = false;
		int write_fd;

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		write_fd = zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_WRITE_FD);
		/* The read that fails comes before the notification of this signal is read. */
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_HOLD);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_FAIL_READ);
		zend_test_forward_raise_foreign();
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RELEASE);
		if (!zend_test_forward_wait(&zend_test_forward_main_hits)) {
			return false;
		}
		while (!replaced && time(NULL) - start < 30) {
			replaced = zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RUNNING) == 1
				&& zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_WRITE_FD) != write_fd;
			usleep(1000);
		}

		/* Forwarding goes on without a new registration. */
		zend_atomic_int_store(&zend_test_forward_main_hits, 0);
		zend_test_forward_raise_foreign();
		return zend_test_forward_wait(&zend_test_forward_main_hits) && replaced;
	}

	if (strcmp(scenario, "helper_suspended") == 0) {
		bool result;

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_signal_forward_suspend();
		result = zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RUNNING) == 0;

		/* A signal meanwhile waits for the helper. */
		zend_atomic_bool_store(&zend_test_forward_result, false);
		zend_test_forward_raise_foreign();
		usleep(100000);
		result = result && zend_atomic_bool_load(&zend_test_forward_result)
			&& zend_atomic_int_load(&zend_test_forward_main_hits) == 0;

		zend_signal_forward_resume();
		result = zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RUNNING) == 1 && result;
		return zend_test_forward_wait(&zend_test_forward_main_hits) && result;
	}

	if (strcmp(scenario, "stale_timer_after_grace") == 0) {
		time_t start;

		zend_signal_forward_timer_arm(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_HOLD);
		zend_test_forward_raise_foreign();
		zend_signal_forward_timer_disarm();
		for (start = time(NULL); time(NULL) - start < 4; usleep(10000));
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RELEASE);
		usleep(200000);
		return zend_atomic_int_load(&zend_test_forward_main_hits) == 0;
	}

	if (strcmp(scenario, "rearmed_timer") == 0) {
		zend_signal_forward_timer_arm(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_HOLD);
		zend_test_forward_raise_foreign();
		zend_signal_forward_timer_arm(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RELEASE);
		usleep(200000);
		zend_signal_forward_timer_disarm();
		return zend_atomic_int_load(&zend_test_forward_main_hits) == 0;
	}

	if (strcmp(scenario, "exit_during_forwarding") == 0) {
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_helper_start();
		zend_test_forward_send(ZEND_TEST_FORWARD_RECORD);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_HOLD);
		zend_test_forward_raise_foreign();
		zend_test_forward_helper_exit();
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RELEASE);
		return zend_test_forward_wait(&zend_test_forward_main_hits)
			&& zend_atomic_int_load(&zend_test_forward_helper_hits) == 0;
	}

	if (strcmp(scenario, "concurrent_unregistration") == 0) {
		pthread_t storm, workers[ZEND_TEST_FORWARD_WORKERS];

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_atomic_bool_store(&zend_test_forward_stop, false);
		pthread_create(&storm, NULL, zend_test_forward_storm_main, NULL);
		for (int i = 0; i < ZEND_TEST_FORWARD_WORKERS; i++) {
			pthread_create(&workers[i], NULL, zend_test_forward_churn_main, NULL);
		}
		for (int i = 0; i < 300; i++) {
			pthread_t thread;

			pthread_create(&thread, NULL, zend_test_forward_short_lived_main, NULL);
			pthread_join(thread, NULL);
		}
		zend_atomic_bool_store(&zend_test_forward_stop, true);
		for (int i = 0; i < ZEND_TEST_FORWARD_WORKERS; i++) {
			pthread_join(workers[i], NULL);
		}
		pthread_join(storm, NULL);

		zend_atomic_int_store(&zend_test_forward_main_hits, 0);
		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_test_forward_raise_foreign();
		return zend_test_forward_wait(&zend_test_forward_main_hits);
	}

	if (strcmp(scenario, "fork_while_held") == 0) {
		bool result;

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_HOLD);
		/* The child inherits a locked mutex and a helper that does not exist there. */
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_LOCK);
		result = zend_test_forward_fork_and_check();
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_UNLOCK);
		zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RELEASE);
		zend_test_forward_raise_foreign();
		return zend_test_forward_wait(&zend_test_forward_main_hits) && result;
	}

	if (strcmp(scenario, "helper_restart") == 0) {
		for (int i = 0; i < 4; i++) {
			zend_atomic_int_store(&zend_test_forward_main_hits, 0);
			zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
			zend_test_forward_raise_foreign();
			if (!zend_test_forward_wait(&zend_test_forward_main_hits)) {
				return false;
			}
			zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_RESTART);
		}
		return true;
	}

	if (strcmp(scenario, "closed_stdio") == 0) {
		pid_t pid;

		zend_signal_forward_acquire(ZEND_TEST_FORWARD_SIGNAL, NULL);
		pid = fork();
		if (pid == 0) {
			bool result;

			close(0);
			close(1);
			close(2);
			result = zend_test_forward_child_delivers();
			_exit(result && zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_READ_FD) > 2
				&& zend_signal_forward_test(ZEND_SIGNAL_FORWARD_TEST_WRITE_FD) > 2 ? 0 : 1);
		}
		return pid > 0 && zend_test_forward_wait_child(pid);
	}
#endif

	zend_throw_error(NULL, "Unknown scenario");
	return false;
}

bool zend_test_signal_forwarding_run(const char *scenario)
{
	struct sigaction action, previous;
	bool result;

	if (strcmp(scenario, "supported") == 0) {
#ifdef ZEND_SIGNAL_FORWARD_ENABLED
		return true;
#else
		return false;
#endif
	}

	zend_test_forward_main = pthread_self();
	zend_atomic_int_store(&zend_test_forward_main_hits, 0);
	zend_atomic_int_store(&zend_test_forward_helper_hits, 0);
	zend_atomic_int_store(&zend_test_forward_replacement_hits, 0);

	memset(&action, 0, sizeof(action));
	action.sa_handler = zend_test_forward_handler;
	action.sa_flags = SA_RESTART;
	sigfillset(&action.sa_mask);
	sigaction(ZEND_TEST_FORWARD_SIGNAL, &action, &previous);

	result = zend_test_forward_scenario(scenario);

	zend_signal_forward_release(ZEND_TEST_FORWARD_SIGNAL, NULL);
	sigaction(ZEND_TEST_FORWARD_SIGNAL, &previous, NULL);

	return result;
}

#endif
