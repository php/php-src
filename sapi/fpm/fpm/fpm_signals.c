	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "fpm.h"
#include "fpm_signals.h"
#include "fpm_sockets.h"
#include "fpm_php.h"
#include "zlog.h"

static int sp[2];
static sigset_t block_sigset;
static sigset_t child_block_sigset;

const char *fpm_signal_names[NSIG + 1] = {
#ifdef SIGHUP
	[SIGHUP] 		= "SIGHUP",
#endif
#ifdef SIGINT
	[SIGINT] 		= "SIGINT",
#endif
#ifdef SIGQUIT
	[SIGQUIT] 		= "SIGQUIT",
#endif
#ifdef SIGILL
	[SIGILL] 		= "SIGILL",
#endif
#ifdef SIGTRAP
	[SIGTRAP] 		= "SIGTRAP",
#endif
#ifdef SIGABRT
	[SIGABRT] 		= "SIGABRT",
#endif
#ifdef SIGEMT
	[SIGEMT] 		= "SIGEMT",
#endif
#ifdef SIGBUS
	[SIGBUS] 		= "SIGBUS",
#endif
#ifdef SIGFPE
	[SIGFPE] 		= "SIGFPE",
#endif
#ifdef SIGKILL
	[SIGKILL] 		= "SIGKILL",
#endif
#ifdef SIGUSR1
	[SIGUSR1] 		= "SIGUSR1",
#endif
#ifdef SIGSEGV
	[SIGSEGV] 		= "SIGSEGV",
#endif
#ifdef SIGUSR2
	[SIGUSR2] 		= "SIGUSR2",
#endif
#ifdef SIGPIPE
	[SIGPIPE] 		= "SIGPIPE",
#endif
#ifdef SIGALRM
	[SIGALRM] 		= "SIGALRM",
#endif
#ifdef SIGTERM
	[SIGTERM] 		= "SIGTERM",
#endif
#ifdef SIGCHLD
	[SIGCHLD] 		= "SIGCHLD",
#endif
#ifdef SIGCONT
	[SIGCONT] 		= "SIGCONT",
#endif
#ifdef SIGSTOP
	[SIGSTOP] 		= "SIGSTOP",
#endif
#ifdef SIGTSTP
	[SIGTSTP] 		= "SIGTSTP",
#endif
#ifdef SIGTTIN
	[SIGTTIN] 		= "SIGTTIN",
#endif
#ifdef SIGTTOU
	[SIGTTOU] 		= "SIGTTOU",
#endif
#ifdef SIGURG
	[SIGURG] 		= "SIGURG",
#endif
#ifdef SIGXCPU
	[SIGXCPU] 		= "SIGXCPU",
#endif
#ifdef SIGXFSZ
	[SIGXFSZ] 		= "SIGXFSZ",
#endif
#ifdef SIGVTALRM
	[SIGVTALRM] 	= "SIGVTALRM",
#endif
#ifdef SIGPROF
	[SIGPROF] 		= "SIGPROF",
#endif
#ifdef SIGWINCH
	[SIGWINCH] 		= "SIGWINCH",
#endif
#ifdef SIGINFO
	[SIGINFO] 		= "SIGINFO",
#endif
#ifdef SIGIO
	[SIGIO] 		= "SIGIO",
#endif
#ifdef SIGPWR
	[SIGPWR] 		= "SIGPWR",
#endif
#ifdef SIGSYS
	[SIGSYS] 		= "SIGSYS",
#endif
#ifdef SIGWAITING
	[SIGWAITING] 	= "SIGWAITING",
#endif
#ifdef SIGLWP
	[SIGLWP] 		= "SIGLWP",
#endif
#ifdef SIGFREEZE
	[SIGFREEZE] 	= "SIGFREEZE",
#endif
#ifdef SIGTHAW
	[SIGTHAW] 		= "SIGTHAW",
#endif
#ifdef SIGCANCEL
	[SIGCANCEL] 	= "SIGCANCEL",
#endif
#ifdef SIGLOST
	[SIGLOST] 		= "SIGLOST",
#endif
};

static void sig_soft_quit(int signo) /* {{{ */
{
	int saved_errno = errno;

	/* closing fastcgi listening socket will force fcgi_accept() exit immediately */
	close(fpm_globals.listening_socket);
	if (0 > socket(AF_UNIX, SOCK_STREAM, 0)) {
		zlog(ZLOG_WARNING, "failed to create a new socket");
	}
	fpm_php_soft_quit();
	errno = saved_errno;
}
/* }}} */

static void sig_handler(int signo) /* {{{ */
{
	static const char sig_chars[NSIG + 1] = {
		[SIGTERM] = 'T',
		[SIGINT]  = 'I',
		[SIGUSR1] = '1',
		[SIGUSR2] = '2',
		[SIGQUIT] = 'Q',
		[SIGCHLD] = 'C'
	};
	char s;
	int saved_errno;

	if (fpm_globals.parent_pid != getpid()) {
		/* Avoid using of signal handlers from the master process in a worker
			before the child sets up its own signal handlers.
			Normally it is prevented by the sigprocmask() calls
			around fork(). This execution branch is a last resort trap
			that has no protection against #76601. */
		return;
	}

	saved_errno = errno;
	s = sig_chars[signo];
	zend_quiet_write(sp[1], &s, sizeof(s));
	errno = saved_errno;
}
/* }}} */

int fpm_signals_init_main() /* {{{ */
{
	struct sigaction act;

	if (0 > socketpair(AF_UNIX, SOCK_STREAM, 0, sp)) {
		zlog(ZLOG_SYSERROR, "failed to init signals: socketpair()");
		return -1;
	}

	if (0 > fd_set_blocked(sp[0], 0) || 0 > fd_set_blocked(sp[1], 0)) {
		zlog(ZLOG_SYSERROR, "failed to init signals: fd_set_blocked()");
		return -1;
	}

	if (0 > fcntl(sp[0], F_SETFD, FD_CLOEXEC) || 0 > fcntl(sp[1], F_SETFD, FD_CLOEXEC)) {
		zlog(ZLOG_SYSERROR, "failed to init signals: fcntl(F_SETFD, FD_CLOEXEC)");
		return -1;
	}

	memset(&act, 0, sizeof(act));
	act.sa_handler = sig_handler;
	sigfillset(&act.sa_mask);

	if (0 > sigaction(SIGTERM,  &act, 0) ||
	    0 > sigaction(SIGINT,   &act, 0) ||
	    0 > sigaction(SIGUSR1,  &act, 0) ||
	    0 > sigaction(SIGUSR2,  &act, 0) ||
	    0 > sigaction(SIGCHLD,  &act, 0) ||
	    0 > sigaction(SIGQUIT,  &act, 0)) {

		zlog(ZLOG_SYSERROR, "failed to init signals: sigaction()");
		return -1;
	}

	zlog(ZLOG_DEBUG, "Unblocking all signals");
	if (0 > fpm_signals_unblock()) {
		return -1;
	}
	return 0;
}
/* }}} */

int fpm_signals_init_child() /* {{{ */
{
	struct sigaction act, act_dfl;

	memset(&act, 0, sizeof(act));
	memset(&act_dfl, 0, sizeof(act_dfl));

	act.sa_handler = &sig_soft_quit;
	act.sa_flags |= SA_RESTART;

	act_dfl.sa_handler = SIG_DFL;

	close(sp[0]);
	close(sp[1]);

	if (0 > sigaction(SIGTERM,  &act_dfl,  0) ||
	    0 > sigaction(SIGINT,   &act_dfl,  0) ||
	    0 > sigaction(SIGUSR1,  &act_dfl,  0) ||
	    0 > sigaction(SIGUSR2,  &act_dfl,  0) ||
	    0 > sigaction(SIGCHLD,  &act_dfl,  0) ||
	    0 > sigaction(SIGQUIT,  &act,      0)) {

		zlog(ZLOG_SYSERROR, "failed to init child signals: sigaction()");
		return -1;
	}

	if (0 > fpm_signals_unblock()) {
		return -1;
	}
	return 0;
}
/* }}} */

int fpm_signals_get_fd() /* {{{ */
{
	return sp[0];
}
/* }}} */

int fpm_signals_init_mask() /* {{{ */
{
	/* Subset of signals from fpm_signals_init_main() and fpm_got_signal()
		blocked to avoid unexpected death during early init
		or during reload just after execvp() or fork */
	int init_signal_array[] = { SIGUSR1, SIGUSR2, SIGCHLD };
	size_t size = sizeof(init_signal_array)/sizeof(init_signal_array[0]);
	size_t i = 0;
	if (0 > sigemptyset(&block_sigset) ||
	    0 > sigemptyset(&child_block_sigset)) {
		zlog(ZLOG_SYSERROR, "failed to prepare signal block mask: sigemptyset()");
		return -1;
	}
	for (i = 0; i < size; ++i) {
		int sig_i = init_signal_array[i];
		if (0 > sigaddset(&block_sigset, sig_i) ||
		    0 > sigaddset(&child_block_sigset, sig_i)) {
			if (sig_i <= NSIG && fpm_signal_names[sig_i] != NULL) {
				zlog(ZLOG_SYSERROR, "failed to prepare signal block mask: sigaddset(%s)",
						fpm_signal_names[sig_i]);
			} else {
				zlog(ZLOG_SYSERROR, "failed to prepare signal block mask: sigaddset(%d)", sig_i);
			}
			return -1;
		}
	}
	if (0 > sigaddset(&child_block_sigset, SIGTERM) ||
	    0 > sigaddset(&child_block_sigset, SIGQUIT)) {
		zlog(ZLOG_SYSERROR, "failed to prepare child signal block mask: sigaddset()");
		return -1;
	}
	return 0;
}
/* }}} */

int fpm_signals_block() /* {{{ */
{
	if (0 > sigprocmask(SIG_BLOCK, &block_sigset, NULL)) {
		zlog(ZLOG_SYSERROR, "failed to block signals");
		return -1;
	}
	return 0;
}
/* }}} */

int fpm_signals_child_block() /* {{{ */
{
	if (0 > sigprocmask(SIG_BLOCK, &child_block_sigset, NULL)) {
		zlog(ZLOG_SYSERROR, "failed to block child signals");
		return -1;
	}
	return 0;
}
/* }}} */

int fpm_signals_unblock() /* {{{ */
{
	/* Ensure that during reload after upgrade all signals are unblocked.
		block_sigset could have different value before execve() */
	sigset_t all_signals;
	sigfillset(&all_signals);
	if (0 > sigprocmask(SIG_UNBLOCK, &all_signals, NULL)) {
		zlog(ZLOG_SYSERROR, "failed to unblock signals");
		return -1;
	}
	return 0;
}
/* }}} */
