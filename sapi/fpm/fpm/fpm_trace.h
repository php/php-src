
	/* $Id: fpm_trace.h,v 1.3 2008/07/20 22:43:39 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_TRACE_H
#define FPM_TRACE_H 1

#include <unistd.h>

int fpm_trace_signal(pid_t pid);
int fpm_trace_ready(pid_t pid);
int fpm_trace_close(pid_t pid);
int fpm_trace_get_long(long addr, long *data);
int fpm_trace_get_strz(char *buf, size_t sz, long addr);

#endif

