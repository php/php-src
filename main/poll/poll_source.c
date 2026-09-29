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
   | Authors: Jakub Zelenka <bukka@php.net>                               |
   +----------------------------------------------------------------------+
*/

/* Native sources for process and signal handles: a descriptor readable when the child exited or a
 * signal of the set is pending, so every descriptor backend can watch it.
 *
 * Linux: a pidfd and a signalfd, which is readable while a signal of its set is pending.
 *
 * kqueue: a private kqueue per source with EVFILT_PROC or one EVFILT_SIGNAL per signal. Both
 * filters record events, not state, so a child already waitable and a signal already pending are
 * announced by a triggered EVFILT_USER note. Neither consumes: the fired handle reaps the child,
 * and a pending signal is taken with sigwait() on it alone. */

#include "php_poll_internal.h"

#ifndef PHP_WIN32

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#ifdef __linux__
# include <sys/syscall.h>
#endif
#ifdef HAVE_SYS_SIGNALFD_H
# include <sys/signalfd.h>
#endif
#ifdef HAVE_SYS_PIDFD_H
# include <sys/pidfd.h>
#endif
#ifdef HAVE_KQUEUE
# include <sys/event.h>
# include <sys/time.h>
#endif

#if defined(__linux__) && (defined(HAVE_PIDFD_OPEN) || defined(SYS_pidfd_open))
# define PHP_POLL_PROCESS_SOURCE_PIDFD 1
#elif defined(HAVE_KQUEUE)
# define PHP_POLL_PROCESS_SOURCE_KQUEUE 1
#endif

#if defined(__linux__) && defined(HAVE_SYS_SIGNALFD_H)
# define PHP_POLL_SIGNAL_SOURCE_SIGNALFD 1
#elif defined(HAVE_KQUEUE)
# define PHP_POLL_SIGNAL_SOURCE_KQUEUE 1
#endif

PHPAPI bool php_poll_has_process_source(void)
{
#if defined(PHP_POLL_PROCESS_SOURCE_PIDFD) || defined(PHP_POLL_PROCESS_SOURCE_KQUEUE)
	return true;
#else
	return false;
#endif
}

PHPAPI bool php_poll_has_signal_source(void)
{
#if defined(PHP_POLL_SIGNAL_SOURCE_SIGNALFD) || defined(PHP_POLL_SIGNAL_SOURCE_KQUEUE)
	return true;
#else
	return false;
#endif
}

#ifdef HAVE_KQUEUE
/* Make the kqueue readable at once, for what the filters would not report */
static zend_result php_poll_kqueue_kick(int kq)
{
	struct kevent kev;
#ifdef EVFILT_USER
	EV_SET(&kev, 0, EVFILT_USER, EV_ADD | EV_ONESHOT, NOTE_TRIGGER, 0, NULL);
#else
	EV_SET(&kev, 0, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, 0, NULL);
#endif
	return kevent(kq, &kev, 1, NULL, 0, NULL) == 0 ? SUCCESS : FAILURE;
}

static int php_poll_kqueue_open(void)
{
	int kq = kqueue();
	if (kq >= 0) {
		fcntl(kq, F_SETFD, FD_CLOEXEC);
	}
	return kq;
}
#endif

PHPAPI int php_poll_process_source_open(pid_t pid)
{
#if defined(PHP_POLL_PROCESS_SOURCE_PIDFD)
# ifdef HAVE_PIDFD_OPEN
	int fd = pidfd_open(pid, 0);
# else
	int fd = (int) syscall(SYS_pidfd_open, pid, 0);
# endif
	if (fd >= 0) {
		fcntl(fd, F_SETFD, FD_CLOEXEC);
	}
	return fd;
#elif defined(PHP_POLL_PROCESS_SOURCE_KQUEUE)
	int kq = php_poll_kqueue_open();
	if (kq < 0) {
		return -1;
	}
	struct kevent kev;
	EV_SET(&kev, pid, EVFILT_PROC, EV_ADD, NOTE_EXIT, 0, NULL);
	if (kevent(kq, &kev, 1, NULL, 0, NULL) == 0) {
		return kq;
	}
	int err = errno;
	if (err == ESRCH) {
		/* Exited already: a child not reaped yet is still waitable, and
		 * the source reports it at the first wait */
		siginfo_t si;
		memset(&si, 0, sizeof(si));
		if (waitid(P_PID, pid, &si, WEXITED | WNOHANG | WNOWAIT) == 0 && si.si_pid == pid
				&& php_poll_kqueue_kick(kq) == SUCCESS) {
			return kq;
		}
		err = ESRCH;
	}
	close(kq);
	errno = err;
	return -1;
#else
	(void) pid;
	errno = ENOSYS;
	return -1;
#endif
}

#if defined(PHP_POLL_SIGNAL_SOURCE_KQUEUE)
static bool php_poll_signal_any_pending(const sigset_t *set, int after)
{
	sigset_t pending;
	if (sigpending(&pending) != 0) {
		return false;
	}
	for (int signo = after + 1; signo < NSIG; signo++) {
		if (sigismember(set, signo) == 1 && sigismember(&pending, signo) == 1) {
			return true;
		}
	}
	return false;
}
#endif

PHPAPI int php_poll_signal_take_pending(const sigset_t *set, siginfo_t *info)
{
#ifdef HAVE_SIGTIMEDWAIT
	struct timespec zero = { 0, 0 };
	siginfo_t local;
	int signo;
	do {
		signo = sigtimedwait(set, info ? info : &local, &zero);
	} while (signo == -1 && errno == EINTR);
	return signo > 0 ? signo : 0;
#else
	/* Without sigtimedwait(): what sigpending() shows is taken with
	 * sigwait() on that one signal, which returns at once for a pending
	 * signal and yields no siginfo beyond the number */
	sigset_t pending;
	if (sigpending(&pending) != 0) {
		return 0;
	}
	for (int signo = 1; signo < NSIG; signo++) {
		if (sigismember(set, signo) == 1 && sigismember(&pending, signo) == 1) {
			sigset_t one;
			int got;
			sigemptyset(&one);
			sigaddset(&one, signo);
			if (sigwait(&one, &got) != 0) {
				continue;
			}
			if (info) {
				memset(info, 0, sizeof(*info));
				info->si_signo = got;
			}
			return got;
		}
	}
	return 0;
#endif
}

PHPAPI int php_poll_signal_source_open(const sigset_t *set)
{
#if defined(PHP_POLL_SIGNAL_SOURCE_SIGNALFD)
	return signalfd(-1, set, SFD_NONBLOCK | SFD_CLOEXEC);
#elif defined(PHP_POLL_SIGNAL_SOURCE_KQUEUE)
	int kq = php_poll_kqueue_open();
	if (kq < 0) {
		return -1;
	}
	struct kevent kevs[NSIG];
	int n = 0;
	for (int signo = 1; signo < NSIG; signo++) {
		if (sigismember(set, signo) == 1) {
			EV_SET(&kevs[n++], signo, EVFILT_SIGNAL, EV_ADD, 0, 0, NULL);
		}
	}
	if (n == 0 || kevent(kq, kevs, n, NULL, 0, NULL) != 0
			/* Pending already: the filter would not report it */
			|| (php_poll_signal_any_pending(set, 0) && php_poll_kqueue_kick(kq) != SUCCESS)) {
		int err = n == 0 ? EINVAL : errno;
		close(kq);
		errno = err;
		return -1;
	}
	return kq;
#else
	(void) set;
	errno = ENOSYS;
	return -1;
#endif
}

PHPAPI int php_poll_signal_source_take(int fd, const sigset_t *set, siginfo_t *info)
{
#if defined(PHP_POLL_SIGNAL_SOURCE_SIGNALFD)
	if (fd >= 0) {
		struct signalfd_siginfo fdsi;
		ssize_t n;
		do {
			n = read(fd, &fdsi, sizeof(fdsi));
		} while (n == -1 && errno == EINTR);
		if (n != (ssize_t) sizeof(fdsi)) {
			return 0;
		}
		if (info) {
			memset(info, 0, sizeof(*info));
			info->si_signo = (int) fdsi.ssi_signo;
			info->si_errno = (int) fdsi.ssi_errno;
			info->si_code = (int) fdsi.ssi_code;
			info->si_pid = (pid_t) fdsi.ssi_pid;
			info->si_uid = (uid_t) fdsi.ssi_uid;
			info->si_status = (int) fdsi.ssi_status;
		}
		return (int) fdsi.ssi_signo;
	}
	return php_poll_signal_take_pending(set, info);
#elif defined(PHP_POLL_SIGNAL_SOURCE_KQUEUE)
	if (fd >= 0) {
		/* Retrieve the notes so the kqueue stops reading as readable; what
		 * is pending comes from the kernel, not from the notes */
		struct kevent kevs[8];
		const struct timespec zero = { 0, 0 };
		while (kevent(fd, NULL, 0, kevs, 8, &zero) == 8) {
		}
	}
	int signo = php_poll_signal_take_pending(set, info);
	if (signo > 0 && fd >= 0 && php_poll_signal_any_pending(set, 0)) {
		/* Still level: more of the set is pending */
		php_poll_kqueue_kick(fd);
	}
	return signo;
#else
	(void) fd;
	return php_poll_signal_take_pending(set, info);
#endif
}

#endif /* PHP_WIN32 */
