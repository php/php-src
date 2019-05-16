	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "php_syslog.h"

#include "fpm.h"
#include "fpm_children.h"
#include "fpm_cleanup.h"
#include "fpm_events.h"
#include "fpm_sockets.h"
#include "fpm_stdio.h"
#include "zlog.h"

static int fd_stdout[2];
static int fd_stderr[2];

int fpm_stdio_init_main() /* {{{ */
{
	int fd = open("/dev/null", O_RDWR);

	if (0 > fd) {
		zlog(ZLOG_SYSERROR, "failed to init stdio: open(\"/dev/null\")");
		return -1;
	}

	if (0 > dup2(fd, STDIN_FILENO) || 0 > dup2(fd, STDOUT_FILENO)) {
		zlog(ZLOG_SYSERROR, "failed to init stdio: dup2()");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}
/* }}} */

static inline int fpm_use_error_log() {  /* {{{ */
	/*
	 * the error_log is NOT used when running in foreground
	 * and from a tty (user looking at output).
	 * So, error_log is used by
	 * - SysV init launch php-fpm as a daemon
	 * - Systemd launch php-fpm in foreground
	 */
#if HAVE_UNISTD_H
	if (fpm_global_config.daemonize || (!isatty(STDERR_FILENO) && !fpm_globals.force_stderr)) {
#else
	if (fpm_global_config.daemonize) {
#endif
		return 1;
	}
	return 0;
}

/* }}} */
int fpm_stdio_init_final() /* {{{ */
{
	if (fpm_use_error_log()) {
		/* prevent duping if logging to syslog */
		if (fpm_globals.error_log_fd > 0 && fpm_globals.error_log_fd != STDERR_FILENO) {

			/* there might be messages to stderr from other parts of the code, we need to log them all */
			if (0 > dup2(fpm_globals.error_log_fd, STDERR_FILENO)) {
				zlog(ZLOG_SYSERROR, "failed to init stdio: dup2()");
				return -1;
			}
		}
#ifdef HAVE_SYSLOG_H
		else if (fpm_globals.error_log_fd == ZLOG_SYSLOG) {
			/* dup to /dev/null when using syslog */
			dup2(STDOUT_FILENO, STDERR_FILENO);
		}
#endif
	}
	zlog_set_launched();
	return 0;
}
/* }}} */

int fpm_stdio_init_child(struct fpm_worker_pool_s *wp) /* {{{ */
{
#ifdef HAVE_SYSLOG_H
	if (fpm_globals.error_log_fd == ZLOG_SYSLOG) {
		closelog(); /* ensure to close syslog not to interrupt with PHP syslog code */
	} else
#endif

	/* Notice: child cannot use master error_log
	 * because not aware when being reopen
	 * else, should use if (!fpm_use_error_log())
	 */
	if (fpm_globals.error_log_fd > 0) {
		close(fpm_globals.error_log_fd);
	}
	fpm_globals.error_log_fd = -1;
	zlog_set_fd(-1);

	return 0;
}
/* }}} */

int fpm_stdio_flush_child() /* {{{ */
{
	return write(STDERR_FILENO, "\0", 1);
}
/* }}} */

static void fpm_stdio_child_said(struct fpm_event_s *ev, short which, void *arg) /* {{{ */
{
	static const int max_buf_size = 1024;
	int fd = ev->fd;
	char buf[max_buf_size];
	struct fpm_child_s *child;
	int is_stdout;
	struct fpm_event_s *event;
	int fifo_in = 1, fifo_out = 1;
	int in_buf = 0;
	int read_fail = 0, finish_log_stream = 0, create_log_stream;
	int res;
	struct zlog_stream *log_stream;

	if (!arg) {
		return;
	}
	child = (struct fpm_child_s *)arg;

	is_stdout = (fd == child->fd_stdout);
	if (is_stdout) {
		event = &child->ev_stdout;
	} else {
		event = &child->ev_stderr;
	}

	create_log_stream = !child->log_stream;
	if (create_log_stream) {
		log_stream = child->log_stream = malloc(sizeof(struct zlog_stream));
		zlog_stream_init_ex(log_stream, ZLOG_WARNING, STDERR_FILENO);
		zlog_stream_set_decorating(log_stream, child->wp->config->decorate_workers_output);
		zlog_stream_set_wrapping(log_stream, ZLOG_TRUE);
		zlog_stream_set_msg_prefix(log_stream, "[pool %s] child %d said into %s: ",
				child->wp->config->name, (int) child->pid, is_stdout ? "stdout" : "stderr");
		zlog_stream_set_msg_quoting(log_stream, ZLOG_TRUE);
	} else {
		log_stream = child->log_stream;
	}

	while (fifo_in || fifo_out) {
		if (fifo_in) {
			res = read(fd, buf + in_buf, max_buf_size - 1 - in_buf);
			if (res <= 0) { /* no data */
				fifo_in = 0;
				if (res == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
					/* pipe is closed or error */
					read_fail = (res < 0) ? res : 1;
				}
			} else {
				in_buf += res;
				/* if buffer ends with \0, then the stream will be finished */
				if (!buf[in_buf - 1]) {
					finish_log_stream = 1;
					in_buf--;
				}
			}
		}

		if (fifo_out) {
			if (in_buf == 0) {
				fifo_out = 0;
			} else {
				char *nl;

				nl = memchr(buf, '\n', in_buf);
				if (nl) {
					/* we should print each new line int the new message */
					int out_len = nl - buf;
					zlog_stream_str(log_stream, buf, out_len);
					zlog_stream_finish(log_stream);
					/* skip new line */
					out_len++;
					/* move data in the buffer */
					memmove(buf, buf + out_len, in_buf - out_len);
					in_buf -= out_len;
				} else if (in_buf == max_buf_size - 1 || !fifo_in) {
					/* we should print if no more space in the buffer or no more data to come */
					zlog_stream_str(log_stream, buf, in_buf);
					in_buf = 0;
				}
			}
		}
	}

	if (read_fail) {
		if (create_log_stream) {
			zlog_stream_set_msg_suffix(log_stream, NULL, ", pipe is closed");
			zlog_stream_finish(log_stream);
		}
		if (read_fail < 0) {
			zlog(ZLOG_SYSERROR, "unable to read what child say");
		}

		fpm_event_del(event);

		if (is_stdout) {
			close(child->fd_stdout);
			child->fd_stdout = -1;
		} else {
			close(child->fd_stderr);
			child->fd_stderr = -1;
		}
	} else if (finish_log_stream) {
		zlog_stream_finish(log_stream);
	}
}
/* }}} */

int fpm_stdio_prepare_pipes(struct fpm_child_s *child) /* {{{ */
{
	if (0 == child->wp->config->catch_workers_output) { /* not required */
		return 0;
	}

	if (0 > pipe(fd_stdout)) {
		zlog(ZLOG_SYSERROR, "failed to prepare the stdout pipe");
		return -1;
	}

	if (0 > pipe(fd_stderr)) {
		zlog(ZLOG_SYSERROR, "failed to prepare the stderr pipe");
		close(fd_stdout[0]);
		close(fd_stdout[1]);
		return -1;
	}

	if (0 > fd_set_blocked(fd_stdout[0], 0) || 0 > fd_set_blocked(fd_stderr[0], 0)) {
		zlog(ZLOG_SYSERROR, "failed to unblock pipes");
		close(fd_stdout[0]);
		close(fd_stdout[1]);
		close(fd_stderr[0]);
		close(fd_stderr[1]);
		return -1;
	}
	return 0;
}
/* }}} */

int fpm_stdio_parent_use_pipes(struct fpm_child_s *child) /* {{{ */
{
	if (0 == child->wp->config->catch_workers_output) { /* not required */
		return 0;
	}

	close(fd_stdout[1]);
	close(fd_stderr[1]);

	child->fd_stdout = fd_stdout[0];
	child->fd_stderr = fd_stderr[0];

	fpm_event_set(&child->ev_stdout, child->fd_stdout, FPM_EV_READ, fpm_stdio_child_said, child);
	fpm_event_add(&child->ev_stdout, 0);

	fpm_event_set(&child->ev_stderr, child->fd_stderr, FPM_EV_READ, fpm_stdio_child_said, child);
	fpm_event_add(&child->ev_stderr, 0);
	return 0;
}
/* }}} */

int fpm_stdio_discard_pipes(struct fpm_child_s *child) /* {{{ */
{
	if (0 == child->wp->config->catch_workers_output) { /* not required */
		return 0;
	}

	close(fd_stdout[1]);
	close(fd_stderr[1]);

	close(fd_stdout[0]);
	close(fd_stderr[0]);
	return 0;
}
/* }}} */

void fpm_stdio_child_use_pipes(struct fpm_child_s *child) /* {{{ */
{
	if (child->wp->config->catch_workers_output) {
		dup2(fd_stdout[1], STDOUT_FILENO);
		dup2(fd_stderr[1], STDERR_FILENO);
		close(fd_stdout[0]); close(fd_stdout[1]);
		close(fd_stderr[0]); close(fd_stderr[1]);
	} else {
		/* stdout of parent is always /dev/null */
		dup2(STDOUT_FILENO, STDERR_FILENO);
	}
}
/* }}} */

int fpm_stdio_open_error_log(int reopen) /* {{{ */
{
	int fd;

#ifdef HAVE_SYSLOG_H
	if (!strcasecmp(fpm_global_config.error_log, "syslog")) {
		php_openlog(fpm_global_config.syslog_ident, LOG_PID | LOG_CONS, fpm_global_config.syslog_facility);
		fpm_globals.error_log_fd = ZLOG_SYSLOG;
		if (fpm_use_error_log()) {
			zlog_set_fd(fpm_globals.error_log_fd);
		}
		return 0;
	}
#endif

	fd = open(fpm_global_config.error_log, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
	if (0 > fd) {
		zlog(ZLOG_SYSERROR, "failed to open error_log (%s)", fpm_global_config.error_log);
		return -1;
	}

	if (reopen) {
		if (fpm_use_error_log()) {
			dup2(fd, STDERR_FILENO);
		}

		dup2(fd, fpm_globals.error_log_fd);
		close(fd);
		fd = fpm_globals.error_log_fd; /* for FD_CLOSEXEC to work */
	} else {
		fpm_globals.error_log_fd = fd;
		if (fpm_use_error_log()) {
			zlog_set_fd(fpm_globals.error_log_fd);
		}
	}
	if (0 > fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC)) {
		zlog(ZLOG_WARNING, "failed to change attribute of error_log");
	}
	return 0;
}
/* }}} */
