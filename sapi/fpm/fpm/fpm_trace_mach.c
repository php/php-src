	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <mach/mach.h>
#include <mach/mach_vm.h>

#include <unistd.h>

#include "fpm_trace.h"
#include "fpm_process_ctl.h"
#include "fpm_unix.h"
#include "zlog.h"


static mach_port_name_t target;
static vm_offset_t target_page_base;
static vm_offset_t local_page;
static mach_msg_type_number_t local_size;

static void fpm_mach_vm_deallocate(void)
{
	if (local_page) {
		mach_vm_deallocate(mach_task_self(), local_page, local_size);
		target_page_base = 0;
		local_page = 0;
		local_size = 0;
	}
}

static int fpm_mach_vm_read_page(vm_offset_t page) /* {{{ */
{
	kern_return_t kr;

	kr = mach_vm_read(target, page, fpm_pagesize, &local_page, &local_size);
	if (kr != KERN_SUCCESS) {
		zlog(ZLOG_ERROR, "failed to read vm page: mach_vm_read(): %s (%d)", mach_error_string(kr), kr);
		return -1;
	}
	return 0;
}
/* }}} */

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
	kern_return_t kr;

	kr = task_for_pid(mach_task_self(), pid, &target);
	if (kr != KERN_SUCCESS) {
		char *msg = "";

		if (kr == KERN_FAILURE) {
			msg = " It seems that master process does not have enough privileges to trace processes.";
		}
		zlog(ZLOG_ERROR, "task_for_pid() failed: %s (%d)%s", mach_error_string(kr), kr, msg);
		return -1;
	}
	return 0;
}
/* }}} */

int fpm_trace_close(pid_t pid) /* {{{ */
{
	fpm_mach_vm_deallocate();
	target = 0;
	return 0;
}
/* }}} */

int fpm_trace_get_long(long addr, long *data) /* {{{ */
{
	size_t offset = ((uintptr_t) (addr) % fpm_pagesize);
	vm_offset_t base = (uintptr_t) (addr) - offset;

	if (base != target_page_base) {
		fpm_mach_vm_deallocate();
		if (0 > fpm_mach_vm_read_page(base)) {
			return -1;
		}
	}
	*data = * (long *) (local_page + offset);
	return 0;
}
/* }}} */
