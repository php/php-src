
	/* $Id: zlog.c,v 1.7 2008/05/22 21:08:32 anight Exp $ */
	/* (c) 2004-2007 Andrei Nigmatulin */

#include "fpm_config.h"

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include <errno.h>

#include "php_syslog.h"

#include "zlog.h"
#include "fpm.h"

#define MAX_LINE_LENGTH 1024

static int zlog_fd = -1;
static int zlog_level = ZLOG_NOTICE;
static int launched = 0;
static void (*external_logger)(int, char *, size_t) = NULL;

static const char *level_names[] = {
	[ZLOG_DEBUG]   = "DEBUG",
	[ZLOG_NOTICE]  = "NOTICE",
	[ZLOG_WARNING] = "WARNING",
	[ZLOG_ERROR]   = "ERROR",
	[ZLOG_ALERT]   = "ALERT",
};

#ifdef HAVE_SYSLOG_H
const int syslog_priorities[] = {
	[ZLOG_DEBUG]   = LOG_DEBUG,
	[ZLOG_NOTICE]  = LOG_NOTICE,
	[ZLOG_WARNING] = LOG_WARNING,
	[ZLOG_ERROR]   = LOG_ERR,
	[ZLOG_ALERT]   = LOG_ALERT,
};
#endif

void zlog_set_external_logger(void (*logger)(int, char *, size_t)) /* {{{ */
{
	external_logger = logger;
}
/* }}} */

const char *zlog_get_level_name(int log_level) /* {{{ */
{
	if (log_level < 0) {
		log_level = zlog_level;
	} else if (log_level < ZLOG_DEBUG || log_level > ZLOG_ALERT) {
		return "unknown value";
	}

	return level_names[log_level];
}
/* }}} */

void zlog_set_launched(void) {
	launched = 1;
}

size_t zlog_print_time(struct timeval *tv, char *timebuf, size_t timebuf_len) /* {{{ */
{
	struct tm t;
	size_t len;

	len = strftime(timebuf, timebuf_len, "[%d-%b-%Y %H:%M:%S", localtime_r((const time_t *) &tv->tv_sec, &t));
	if (zlog_level == ZLOG_DEBUG) {
		len += snprintf(timebuf + len, timebuf_len - len, ".%06d", (int) tv->tv_usec);
	}
	len += snprintf(timebuf + len, timebuf_len - len, "] ");
	return len;
}
/* }}} */

int zlog_set_fd(int new_fd) /* {{{ */
{
	int old_fd = zlog_fd;

	zlog_fd = new_fd;
	return old_fd;
}
/* }}} */

int zlog_set_level(int new_value) /* {{{ */
{
	int old_value = zlog_level;

	if (new_value < ZLOG_DEBUG || new_value > ZLOG_ALERT) return old_value;

	zlog_level = new_value;
	return old_value;
}
/* }}} */

void zlog_ex(const char *function, int line, int flags, const char *fmt, ...) /* {{{ */
{
	struct timeval tv;
	char buf[MAX_LINE_LENGTH];
	const size_t buf_size = MAX_LINE_LENGTH;
	va_list args;
	size_t len = 0;
	int truncated = 0;
	int saved_errno;

	if (external_logger) {
		va_start(args, fmt);
		len = vsnprintf(buf, buf_size, fmt, args);
		va_end(args);
		if (len >= buf_size) {
			memcpy(buf + buf_size - sizeof("..."), "...", sizeof("...") - 1);
			len = buf_size - 1;
		}
		external_logger(flags & ZLOG_LEVEL_MASK, buf, len);
		len = 0;
		memset(buf, '\0', buf_size);
	}

	if ((flags & ZLOG_LEVEL_MASK) < zlog_level) {
		return;
	}

	saved_errno = errno;
#ifdef HAVE_SYSLOG_H
	if (zlog_fd == ZLOG_SYSLOG /* && !fpm_globals.is_child */) {
		len = 0;
		if (zlog_level == ZLOG_DEBUG) {
			len += snprintf(buf, buf_size, "[%s] %s(), line %d: ", level_names[flags & ZLOG_LEVEL_MASK], function, line);
		} else {
			len += snprintf(buf, buf_size, "[%s] ", level_names[flags & ZLOG_LEVEL_MASK]);
		}
	} else
#endif
	{
		if (!fpm_globals.is_child) {
			gettimeofday(&tv, 0);
			len = zlog_print_time(&tv, buf, buf_size);
		}
		if (zlog_level == ZLOG_DEBUG) {
			if (!fpm_globals.is_child) {
				len += snprintf(buf + len, buf_size - len, "%s: pid %d, %s(), line %d: ", level_names[flags & ZLOG_LEVEL_MASK], getpid(), function, line);
			} else {
				len += snprintf(buf + len, buf_size - len, "%s: %s(), line %d: ", level_names[flags & ZLOG_LEVEL_MASK], function, line);
			}
		} else {
			len += snprintf(buf + len, buf_size - len, "%s: ", level_names[flags & ZLOG_LEVEL_MASK]);
		}
	}

	if (len > buf_size - 1) {
		truncated = 1;
	}

	if (!truncated) {
		va_start(args, fmt);
		len += vsnprintf(buf + len, buf_size - len, fmt, args);
		va_end(args);
		if (len >= buf_size) {
			truncated = 1;
		}
	}

	if (!truncated) {
		if (flags & ZLOG_HAVE_ERRNO) {
			len += snprintf(buf + len, buf_size - len, ": %s (%d)", strerror(saved_errno), saved_errno);
			if (len >= buf_size) {
				truncated = 1;
			}
		}
	}

	if (truncated) {
		memcpy(buf + buf_size - sizeof("..."), "...", sizeof("...") - 1);
		len = buf_size - 1;
	}

#ifdef HAVE_SYSLOG_H
	if (zlog_fd == ZLOG_SYSLOG) {
		buf[len] = '\0';
		php_syslog(syslog_priorities[zlog_level], "%s", buf);
		buf[len++] = '\n';
	} else
#endif
	{
		buf[len++] = '\n';
		write(zlog_fd > -1 ? zlog_fd : STDERR_FILENO, buf, len);
	}

	if (zlog_fd != STDERR_FILENO && zlog_fd != -1 && !launched && (flags & ZLOG_LEVEL_MASK) >= ZLOG_NOTICE) {
		write(STDERR_FILENO, buf, len);
	}
}
/* }}} */

