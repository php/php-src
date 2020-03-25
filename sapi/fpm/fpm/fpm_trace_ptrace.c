	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <sys/wait.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <errno.h>

#if defined(PT_ATTACH) && !defined(PTRACE_ATTACH)
#define PTRACE_ATTACH PT_ATTACH
#endif

#if defined(PT_DETACH) && !defined(PTRACE_DETACH)
#define PTRACE_DETACH PT_DETACH
#endif

#if defined(PT_READ_D) && !defined(PTRACE_PEEKDATA)
#define PTRACE_PEEKDATA PT_READ_D
#endif

#include "fpm_trace.h"
#include "zlog.h"

static pid_t traced_pid;

int fpm_trace_signal(pid_t pid) /* {{{ */
{
	if (0 > ptrace(PTRACE_ATTACH, pid, 0, 0)) {
		zlog(ZLOG_SYSERROR, "failed to ptrace(ATTACH) child %d", pid);
		return -1;
	}
	return 0;
}
/* }}} */

int fpm_trace_ready(pid_t pid) /* {{{ */
{
	traced_pid = pid;
	return 0;
}
/* }}} */

int fpm_trace_close(pid_t pid) /* {{{ */
{
	if (0 > ptrace(PTRACE_DETACH, pid, (void *) 1, 0)) {
		zlog(ZLOG_SYSERROR, "failed to ptrace(DETACH) child %d", pid);
		return -1;
	}
	traced_pid = 0;
	return 0;
}
/* }}} */

int fpm_trace_get_long(long addr, long *data) /* {{{ */
{
#ifdef PT_IO
	struct ptrace_io_desc ptio = {
		.piod_op = PIOD_READ_D,
		.piod_offs = (void *) addr,
		.piod_addr = (void *) data,
		.piod_len = sizeof(long)
	};

	if (0 > ptrace(PT_IO, traced_pid, (void *) &ptio, 0)) {
		zlog(ZLOG_SYSERROR, "failed to ptrace(PT_IO) pid %d", traced_pid);
		return -1;
	}
#else
	errno = 0;
	*data = ptrace(PTRACE_PEEKDATA, traced_pid, (void *) addr, 0);
	if (errno) {
		zlog(ZLOG_SYSERROR, "failed to ptrace(PEEKDATA) pid %d", traced_pid);
		return -1;
	}
#endif
	return 0;
}
/* }}} */
