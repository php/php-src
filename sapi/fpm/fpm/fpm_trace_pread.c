	/* (c) 2007,2008 Andrei Nigmatulin */

#define _GNU_SOURCE
#define _FILE_OFFSET_BITS 64

#include "fpm_config.h"

#include <unistd.h>

#include <fcntl.h>
#include <stdio.h>
#include <inttypes.h>

#include "fpm_trace.h"
#include "fpm_process_ctl.h"
#include "zlog.h"

static int mem_file = -1;

int fpm_trace_signal(pid_t pid) /* {{{ */
{
	if (0 > fpm_pctl_kill(pid, FPM_PCTL_STOP)) {
		zlog(ZLOG_SYSERROR, "failed to send SIGSTOP to %d", pid);
		return -1;
	}
	return 0;
}
/* }}} */

int fpm_trace_ready(pid_t pid) /* {{{ */
{
	char buf[128];

	sprintf(buf, "/proc/%d/" PROC_MEM_FILE, (int) pid);
	mem_file = open(buf, O_RDONLY);
	if (0 > mem_file) {
		zlog(ZLOG_SYSERROR, "failed to open %s", buf);
		return -1;
	}
	return 0;
}
/* }}} */

int fpm_trace_close(pid_t pid) /* {{{ */
{
	close(mem_file);
	mem_file = -1;
	return 0;
}
/* }}} */

int fpm_trace_get_long(long addr, long *data) /* {{{ */
{
	if (sizeof(*data) != pread(mem_file, (void *) data, sizeof(*data), (uintptr_t) addr)) {
		zlog(ZLOG_SYSERROR, "pread() failed");
		return -1;
	}
	return 0;
}
/* }}} */
