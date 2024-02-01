	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "php_syslog.h"
#include "php_network.h"

#include "fpm.h"
#include "fpm_children.h"
#include "fpm_cleanup.h"
#include "fpm_events.h"
#include "fpm_sockets.h"
#include "fpm_stdio.h"
#include "zlog.h"

static int fd_stderr_original = -1;
static int fd_stdout[2];
static int fd_stderr[2];

int fpm_stdio_init_main(void)
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

static inline int fpm_use_error_log(void) {
	/*
	 * the error_log is NOT used when running in foreground
	 * and from a tty (user looking at output).
	 * So, error_log is used by
	 * - SysV init launch php-fpm as a daemon
	 * - Systemd launch php-fpm in foreground
	 */
#ifdef HAVE_UNISTD_H
	if (fpm_global_config.daemonize || (!isatty(STDERR_FILENO) && !fpm_globals.force_stderr)) {
#else
	if (fpm_global_config.daemonize) {
#endif
		return 1;
	}
	return 0;
}

int fpm_stdio_init_final(void)
{
	if (0 > fpm_stdio_redirect_stderr_to_error_log() ||
	    0 > fpm_stdio_redirect_stderr_to_dev_null_for_syslog()) {

		return -1;
	}

	zlog_set_launched();
	return 0;
}
/* }}} */

int fpm_stdio_save_original_stderr(void)
{
	/* STDERR fd gets lost after calling fpm_stdio_init_final() (check GH-8555) so it can be saved.
	 * It should be used only when PHP-FPM is not daemonized otherwise it might break some
	 * applications (e.g. GH-9754). */
	zlog(ZLOG_DEBUG, "saving original STDERR fd: dup()");
	fd_stderr_original = dup(STDERR_FILENO);
	if (0 > fd_stderr_original) {
		zlog(ZLOG_SYSERROR, "failed to save original STDERR fd, access.log records may appear in error_log: dup()");
		return -1;
	}

	return 0;
}

int fpm_stdio_restore_original_stderr(int close_after_restore)
{
	/* Restore original STDERR fd if it was previously saved. */
	if (-1 != fd_stderr_original) {
		zlog(ZLOG_DEBUG, "restoring original STDERR fd: dup2()");
		if (0 > dup2(fd_stderr_original, STDERR_FILENO)) {
			zlog(ZLOG_SYSERROR, "failed to restore original STDERR fd, access.log records may appear in error_log: dup2()");
			return -1;
		} else {
			if (close_after_restore) {
				close(fd_stderr_original);
			}
		}
	}

	return 0;
}

int fpm_stdio_redirect_stderr_to_error_log(void)
{
	if (fpm_use_error_log()) {
		/* prevent duping if logging to syslog */
		if (fpm_globals.error_log_fd > 0 && fpm_globals.error_log_fd != STDERR_FILENO) {

			/* there might be messages to stderr from other parts of the code, we need to log them all */
			if (0 > dup2(fpm_globals.error_log_fd, STDERR_FILENO)) {
				zlog(ZLOG_SYSERROR, "failed to tie stderr fd with error_log fd: dup2()");
				return -1;
			}
		}
	}

	return 0;
}

int fpm_stdio_redirect_stderr_to_dev_null_for_syslog(void)
{
	if (fpm_use_error_log()) {
#ifdef HAVE_SYSLOG_H
		if (fpm_globals.error_log_fd == ZLOG_SYSLOG) {
			/* dup to /dev/null when using syslog */
			dup2(STDOUT_FILENO, STDERR_FILENO);
		}
#endif
	}

	return 0;
}

int fpm_stdio_init_child(struct fpm_worker_pool_s *wp) /* {{{ */
{
#ifdef HAVE_SYSLOG_H
	if (fpm_globals.error_log_fd == ZLOG_SYSLOG) {
		php_closelog(); /* ensure to close syslog not to interrupt with PHP syslog code */
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

#define FPM_STDIO_CMD_FLUSH "\0fscf"

int fpm_stdio_flush_child(void)
{
	return write(STDERR_FILENO, FPM_STDIO_CMD_FLUSH, sizeof(FPM_STDIO_CMD_FLUSH));
}

static void fpm_stdio_child_said(struct fpm_event_s *ev, short which, void *arg) /* {{{ */
{
	static const int max_buf_size = 1024;
	int fd = ev->fd;
	char buf[max_buf_size];
	struct fpm_child_s *child;
	int is_stdout;
	struct fpm_event_s *event;
	int in_buf = 0, cmd_pos = 0, pos, start;
	int read_fail = 0, create_log_stream;
	struct zlog_stream *log_stream;

	if (!arg) {
		return;
	}
	child = (struct fpm_child_s *) arg;

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
		zlog_stream_set_msg_prefix(log_stream, STREAM_SET_MSG_PREFIX_FMT,
				child->wp->config->name, (int) child->pid, is_stdout ? "stdout" : "stderr");
		zlog_stream_set_msg_quoting(log_stream, ZLOG_TRUE);
		zlog_stream_set_is_stdout(log_stream, is_stdout);
		zlog_stream_set_child_pid(log_stream, (int)child->pid);
	} else {
		log_stream = child->log_stream;
		// if fd type (stdout/stderr) or child's pid is changed,
		// then the stream will be finished and msg's prefix will be reinitialized
		if (log_stream->is_stdout != (unsigned int)is_stdout || log_stream->child_pid != (int)child->pid) {
			zlog_stream_finish(log_stream);
			zlog_stream_set_msg_prefix(log_stream, STREAM_SET_MSG_PREFIX_FMT,
					child->wp->config->name, (int) child->pid, is_stdout ? "stdout" : "stderr");
			zlog_stream_set_is_stdout(log_stream, is_stdout);
			zlog_stream_set_child_pid(log_stream, (int)child->pid);
		}
	}

	while (1) {
stdio_read:
		in_buf = read(fd, buf, max_buf_size - 1);
		if (in_buf <= 0) { /* no data */
			if (in_buf == 0 || !PHP_IS_TRANSIENT_ERROR(errno)) {
				/* pipe is closed or error */
				read_fail = (in_buf < 0) ? in_buf : 1;
			}
			break;
		}
		start = 0;
		if (cmd_pos > 0) {
			if 	((sizeof(FPM_STDIO_CMD_FLUSH) - cmd_pos) <= in_buf &&
					!memcmp(buf, &FPM_STDIO_CMD_FLUSH[cmd_pos], sizeof(FPM_STDIO_CMD_FLUSH) - cmd_pos)) {
				zlog_stream_finish(log_stream);
				start = cmd_pos;
			} else {
				zlog_stream_str(log_stream, &FPM_STDIO_CMD_FLUSH[0], cmd_pos);
			}
			cmd_pos = 0;
		}
		for (pos = start; pos < in_buf; pos++) {
			switch (buf[pos]) {
				case '\n':
					zlog_stream_str(log_stream, buf + start, pos - start);
					zlog_stream_finish(log_stream);
					start = pos + 1;
					break;
				case '\0':
					if (pos + sizeof(FPM_STDIO_CMD_FLUSH) <= in_buf) {
						if (!memcmp(buf + pos, FPM_STDIO_CMD_FLUSH, sizeof(FPM_STDIO_CMD_FLUSH))) {
							zlog_stream_str(log_stream, buf + start, pos - start);
							zlog_stream_finish(log_stream);
							start = pos + sizeof(FPM_STDIO_CMD_FLUSH);
							pos = start - 1;
						}
					} else if (!memcmp(buf + pos, FPM_STDIO_CMD_FLUSH, in_buf - pos)) {
						cmd_pos = in_buf - pos;
						zlog_stream_str(log_stream, buf + start, pos - start);
						goto stdio_read;
					}
					break;
			}
		}
		if (start < pos) {
			zlog_stream_str(log_stream, buf + start, pos - start);
		}
	}

	if (read_fail) {
		if (create_log_stream) {
			zlog_stream_set_msg_suffix(log_stream, NULL, ", pipe is closed");
			zlog_stream_finish(log_stream);
		}
		if (read_fail < 0) {
			zlog(ZLOG_SYSERROR, "unable to read what child %d said into %s", (int) child->pid, is_stdout ? "stdout" : "stderr");
		}

		fpm_event_del(event);

		child->postponed_free = true;
		if (is_stdout) {
			close(child->fd_stdout);
			child->fd_stdout = -1;
		} else {
			close(child->fd_stderr);
			child->fd_stderr = -1;
		}
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
