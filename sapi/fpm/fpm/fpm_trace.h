	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_TRACE_H
#define FPM_TRACE_H 1

#include <stdbool.h>
#include <unistd.h>

bool fpm_trace_signal(pid_t pid);
bool fpm_trace_ready(pid_t pid);
bool fpm_trace_close(pid_t pid);
bool fpm_trace_get_long(long addr, long *data);
bool fpm_trace_get_strz(char *buf, size_t sz, long addr);

#endif
