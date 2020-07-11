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
#include "zend_portability.h"

/* buffer is used for fmt result and it should never be over 2048 */
#define MAX_BUF_LENGTH 2048

/* maximal length for wrapping prefix */
#define MAX_WRAPPING_PREFIX_LENGTH 512

#define EXTRA_SPACE_FOR_PREFIX 128

static int zlog_fd = -1;
static int zlog_level = ZLOG_NOTICE;
static int zlog_limit = ZLOG_DEFAULT_LIMIT;
static zlog_bool zlog_buffering = ZLOG_DEFAULT_BUFFERING;
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

void zlog_set_launched(void) /* {{{ */
{
	launched = 1;
}
/* }}} */

size_t zlog_print_time(struct timeval *tv, char *timebuf, size_t timebuf_len) /* {{{ */
{
	struct tm t;
	size_t len;

	len = strftime(timebuf, timebuf_len, "[%d-%b-%Y %H:%M:%S",
			localtime_r((const time_t *) &tv->tv_sec, &t));
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

int zlog_set_limit(int new_value) /* {{{ */
{
	int old_value = zlog_limit;

	zlog_limit = new_value;
	return old_value;
}
/* }}} */

int zlog_set_buffering(zlog_bool buffering) /* {{{ */
{
	int old_value = zlog_buffering;

	zlog_buffering = buffering;
	return old_value;
}
/* }}} */

static inline size_t zlog_truncate_buf(char *buf, size_t buf_size, size_t space_left) /* {{{ */
{
	memcpy(buf + buf_size - sizeof("...") + 1 - space_left, "...", sizeof("...") - 1);
	return buf_size - space_left;
}
/* }}} */

static inline void zlog_external(
		int flags, char *buf, size_t buf_size, const char *fmt, va_list args) /* {{{ */
{
	va_list ap;
	size_t len;

	va_copy(ap, args);
	len = vsnprintf(buf, buf_size, fmt, ap);
	va_end(ap);

	if (len >= buf_size) {
		len = zlog_truncate_buf(buf, buf_size, 0);
	}
	external_logger(flags & ZLOG_LEVEL_MASK, buf, len);
}
/* }}} */

static size_t zlog_buf_prefix(
		const char *function, int line, int flags,
		char *buf, size_t buf_size, int use_syslog) /* {{{ */
{
	struct timeval tv;
	size_t len = 0;

#ifdef HAVE_SYSLOG_H
	if (use_syslog /* && !fpm_globals.is_child */) {
		if (zlog_level == ZLOG_DEBUG) {
			len += snprintf(buf, buf_size, "[%s] %s(), line %d: ",
					level_names[flags & ZLOG_LEVEL_MASK], function, line);
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
				len += snprintf(buf + len, buf_size - len, "%s: pid %d, %s(), line %d: ",
						level_names[flags & ZLOG_LEVEL_MASK], getpid(), function, line);
			} else {
				len += snprintf(buf + len, buf_size - len, "%s: %s(), line %d: ",
						level_names[flags & ZLOG_LEVEL_MASK], function, line);
			}
		} else {
			len += snprintf(buf + len, buf_size - len, "%s: ",
					level_names[flags & ZLOG_LEVEL_MASK]);
		}
	}

	return len;
}
/* }}} */

void vzlog(const char *function, int line, int flags, const char *fmt, va_list args) /* {{{ */
{
	char buf[MAX_BUF_LENGTH];
	size_t buf_size = MAX_BUF_LENGTH;
	size_t len = 0;
	int truncated = 0;
	int saved_errno;

	if (external_logger) {
		zlog_external(flags, buf, buf_size, fmt, args);
	}

	if ((flags & ZLOG_LEVEL_MASK) < zlog_level) {
		return;
	}

	saved_errno = errno;
	len = zlog_buf_prefix(function, line, flags, buf, buf_size, zlog_fd == ZLOG_SYSLOG);

	if (len > buf_size - 1) {
		truncated = 1;
	} else {
		len += vsnprintf(buf + len, buf_size - len, fmt, args);
		if (len >= buf_size) {
			truncated = 1;
		}
	}

	if (!truncated) {
		if (flags & ZLOG_HAVE_ERRNO) {
			len += snprintf(buf + len, buf_size - len,
					": %s (%d)", strerror(saved_errno), saved_errno);
			if (len >= zlog_limit) {
				truncated = 1;
			}
		}
	}

	if (truncated) {
		len = zlog_truncate_buf(buf, zlog_limit < buf_size ? zlog_limit : buf_size, 1);
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
		zend_quiet_write(zlog_fd > -1 ? zlog_fd : STDERR_FILENO, buf, len);
	}

	if (zlog_fd != STDERR_FILENO && zlog_fd != -1 &&
			!launched && (flags & ZLOG_LEVEL_MASK) >= ZLOG_NOTICE) {
		zend_quiet_write(STDERR_FILENO, buf, len);
	}
}
/* }}} */

void zlog_ex(const char *function, int line, int flags, const char *fmt, ...) /* {{{ */
{
	va_list args;
	va_start(args, fmt);
	vzlog(function, line, flags, fmt, args);
	va_end(args);
}
/* }}} */

/* predefine stream init that is used by zlog_msg_ex */
static inline void zlog_stream_init_internal(
		struct zlog_stream *stream, int flags, size_t capacity, int fd);

void zlog_msg_ex(const char *function, int line, int flags,
		const char *prefix, const char *msg) /* {{{ */
{
	struct zlog_stream stream;
	size_t prefix_len = strlen(prefix);
	size_t msg_len = strlen(msg);

	zlog_stream_init_internal(&stream, flags, msg_len + prefix_len, 0);
	zlog_stream_prefix_ex(&stream, function, line);
	zlog_stream_str(&stream, prefix, prefix_len);
	zlog_stream_str(&stream, msg, msg_len);
	zlog_stream_finish(&stream);
	zlog_stream_destroy(&stream);
}
/* }}} */

/* STREAM OPS */

static zlog_bool zlog_stream_buf_alloc_ex(struct zlog_stream *stream, size_t needed)  /* {{{ */
{
	char *buf;
	size_t size = stream->buf.size ?: stream->buf_init_size;

	if (stream->buf.data) {
		size = MIN(zlog_limit, MAX(size * 2, needed));
		buf = realloc(stream->buf.data, size);
	} else {
		size = MIN(zlog_limit, MAX(size, needed));
		buf = malloc(size);
	}

	if (buf == NULL) {
		return 0;
	}

	stream->buf.data = buf;
	stream->buf.size = size;

	return 1;
}
/* }}} */

inline static zlog_bool zlog_stream_buf_alloc(struct zlog_stream *stream)  /* {{{ */
{
	/* if there is enough space in the buffer, we do not need to reallocate */
	if (stream->buf.data && stream->buf.size >= MIN(zlog_limit, stream->buf_init_size)) {
		return 1;
	}
	return zlog_stream_buf_alloc_ex(stream, 0);
}
/* }}} */

static inline ssize_t zlog_stream_direct_write_ex(
		struct zlog_stream *stream, const char *buf, size_t len,
		const char *append, size_t append_len) /* {{{ */
{
	if (stream->use_fd) {
		zend_quiet_write(stream->fd, buf, len);
		if (append_len > 0) {
			zend_quiet_write(stream->fd, append, append_len);
		}
	}

	if (stream->use_stderr) {
		zend_quiet_write(STDERR_FILENO, buf, len);
		if (append_len > 0) {
			zend_quiet_write(STDERR_FILENO, append, append_len);
		}
	}

	return len;
}
/* }}} */

static ssize_t zlog_stream_direct_write(
		struct zlog_stream *stream, const char *buf, size_t len) /* {{{ */
{
	return zlog_stream_direct_write_ex(stream, buf, len, NULL, 0);
}
/* }}} */

static inline ssize_t zlog_stream_unbuffered_write(
		struct zlog_stream *stream, const char *buf, size_t len) /* {{{ */
{
	const char *append;
	size_t append_len = 0, required_len, reserved_len;
	ssize_t written;

	if (stream->len == 0) {
		stream->len = zlog_stream_prefix_ex(stream, stream->function, stream->line);
	}

	/* msg_suffix_len and msg_quote are used only for wrapping */
	reserved_len = stream->len + stream->msg_suffix_len + stream->msg_quote;
	required_len = reserved_len + len;
	if (required_len >= zlog_limit) {
		if (stream->wrap) {
			size_t available_len;
			if (required_len == zlog_limit) {
				append = NULL;
				append_len = 0;
			} else {
				append = "\n";
				append_len = 1;
			}
			available_len = zlog_limit - reserved_len - 1;
			zlog_stream_direct_write(stream, buf, available_len);
			if (append != NULL) {
				if (stream->msg_quote) {
					zlog_stream_direct_write(stream, "\"", 1);
				}
				if (stream->msg_suffix) {
					zlog_stream_direct_write(stream, stream->msg_suffix, stream->msg_suffix_len);
				}
				zlog_stream_direct_write(stream, append, append_len);
			}
			stream->len = 0;
			written = zlog_stream_unbuffered_write(
					stream, buf + available_len, len - available_len);
			if (written > 0) {
				return available_len + written;
			}

			return written;
		}
		/* this would be used in case of an option for disabling wrapping in direct write */
		stream->full = 1;
		if (required_len == zlog_limit) {
			append = NULL;
		} else {
			append = "...";
			append_len = sizeof("...") - 1;
			len = zlog_limit - stream->len - append_len;
		}
	}

	written = zlog_stream_direct_write_ex(stream, buf, len, append, append_len);
	if (written > 0) {
		/* currently written will be always len as the write is blocking
		 * - this should be address if we change to non-blocking write */
		stream->len += written;
	}

	return written;
}
/* }}} */

static inline ssize_t zlog_stream_buf_copy_cstr(
		struct zlog_stream *stream, const char *str, size_t str_len) /* {{{ */
{
	if (stream->buf.size - stream->len <= str_len && !zlog_stream_buf_alloc_ex(stream, str_len)) {
		return -1;
	}

	memcpy(stream->buf.data + stream->len, str, str_len);
	stream->len += str_len;

	return str_len;
}
/* }}} */

static inline ssize_t zlog_stream_buf_copy_char(struct zlog_stream *stream, char c) /* {{{ */
{
	if (stream->buf.size - stream->len < 1 && !zlog_stream_buf_alloc_ex(stream, 1)) {
		return -1;
	}

	stream->buf.data[stream->len++] = c;

	return 1;
}
/* }}} */

static ssize_t zlog_stream_buf_flush(struct zlog_stream *stream) /* {{{ */
{
	ssize_t written;

#ifdef HAVE_SYSLOG_H
	if (stream->use_syslog) {
		zlog_stream_buf_copy_char(stream, '\0');
		php_syslog(syslog_priorities[zlog_level], "%s", stream->buf.data);
		--stream->len;
	}
#endif

	if (external_logger != NULL) {
		external_logger(stream->flags & ZLOG_LEVEL_MASK,
				stream->buf.data + stream->prefix_len, stream->len - stream->prefix_len);
	}
	zlog_stream_buf_copy_char(stream, '\n');
	written = zlog_stream_direct_write(stream, stream->buf.data, stream->len);
	stream->len = 0;

	return written;
}
/* }}} */

static ssize_t zlog_stream_buf_append(
		struct zlog_stream *stream, const char *str, size_t str_len)  /* {{{ */
{
	int over_limit = 0;
	size_t available_len, required_len, reserved_len;

	if (stream->len == 0) {
		stream->len = zlog_stream_prefix_ex(stream, stream->function, stream->line);
	}

	/* msg_suffix_len and msg_quote are used only for wrapping */
	reserved_len = stream->len + stream->msg_suffix_len + stream->msg_quote;
	required_len = reserved_len + str_len;
	if (required_len >= zlog_limit) {
		over_limit = 1;
		available_len = zlog_limit - reserved_len - 1;
	} else {
		available_len = str_len;
	}

	if (zlog_stream_buf_copy_cstr(stream, str, available_len) < 0) {
		return -1;
	}

	if (!over_limit) {
		return available_len;
	}

	if (stream->wrap) {
		if (stream->msg_quote) {
			zlog_stream_buf_copy_char(stream, '"');
		}
		if (stream->msg_suffix != NULL) {
			zlog_stream_buf_copy_cstr(stream, stream->msg_suffix, stream->msg_suffix_len);
		}
		zlog_stream_buf_flush(stream);
		zlog_stream_prefix_ex(stream, stream->function, stream->line);
		return available_len + zlog_stream_buf_append(
				stream, str + available_len, str_len - available_len);
	}

	stream->len = zlog_truncate_buf(stream->buf.data, stream->len, 0);
	stream->full = 1;
	return available_len;
}
/* }}} */

static inline void zlog_stream_init_internal(
		struct zlog_stream *stream, int flags, size_t capacity, int fd) /* {{{ */
{
	if (fd == 0) {
		fd = zlog_fd;
	}

	memset(stream, 0, sizeof(struct zlog_stream));
	stream->flags = flags;
	stream->use_syslog = fd == ZLOG_SYSLOG;
	stream->use_fd = fd > 0;
	stream->use_buffer = zlog_buffering || external_logger != NULL || stream->use_syslog;
	stream->buf_init_size = capacity;
	stream->use_stderr = fd < 0 ||
			(
				fd != STDERR_FILENO && fd != STDOUT_FILENO && !launched &&
				(flags & ZLOG_LEVEL_MASK) >= ZLOG_NOTICE
			);
	stream->prefix_buffer = (flags & ZLOG_LEVEL_MASK) >= zlog_level &&
			(stream->use_fd || stream->use_stderr || stream->use_syslog);
	stream->fd = fd > -1 ? fd : STDERR_FILENO;
}
/* }}} */

void zlog_stream_init(struct zlog_stream *stream, int flags) /* {{{ */
{
	zlog_stream_init_internal(stream, flags, 1024, 0);
}
/* }}} */

void zlog_stream_init_ex(struct zlog_stream *stream, int flags, int fd) /* {{{ */
{
	zlog_stream_init_internal(stream, flags, 1024, fd);
	stream->wrap = 1;
}
/* }}} */

void zlog_stream_set_decorating(struct zlog_stream *stream, zlog_bool decorate) /* {{{ */
{
	if (decorate) {
		stream->decorate = 1;
	} else {
		stream->decorate = 0;
		stream->msg_quote = 0;
		stream->prefix_buffer = 0;
	}
}
/* }}} */

void zlog_stream_set_wrapping(struct zlog_stream *stream, zlog_bool wrap) /* {{{ */
{
	stream->wrap = wrap ? 1 : 0;
}
/* }}} */

void zlog_stream_set_is_stdout(struct zlog_stream *stream, zlog_bool is_stdout) /* {{{ */
{
	stream->is_stdout = is_stdout ? 1 : 0;
}
/* }}} */

void zlog_stream_set_child_pid(struct zlog_stream *stream, int child_pid) /* {{{ */
{
	stream->child_pid = child_pid;
}
/* }}} */

void zlog_stream_set_msg_quoting(struct zlog_stream *stream, zlog_bool quote) /* {{{ */
{
	stream->msg_quote = quote && stream->decorate ? 1 : 0;
}
/* }}} */

zlog_bool zlog_stream_set_msg_prefix(struct zlog_stream *stream, const char *fmt, ...) /* {{{ */
{
	char buf[MAX_WRAPPING_PREFIX_LENGTH];
	size_t len;
	va_list args;

	if (!stream->decorate) {
		return ZLOG_TRUE;
	}

	va_start(args, fmt);
	len = vsnprintf(buf, MAX_WRAPPING_PREFIX_LENGTH - 1, fmt, args);
	va_end(args);

	if (stream->msg_prefix_len < len) {
		stream->msg_prefix = stream->msg_prefix_len ? realloc(stream->msg_prefix, len + 1) : malloc(len + 1);
		if (stream->msg_prefix == NULL) {
			return ZLOG_FALSE;
		}
	}
	memcpy(stream->msg_prefix, buf, len);
	stream->msg_prefix[len] = 0;
	stream->msg_prefix_len = len;

	return len;
}
/* }}} */

zlog_bool zlog_stream_set_msg_suffix(
		struct zlog_stream *stream, const char *suffix, const char *final_suffix)  /* {{{ */
{
	size_t len;
	if (!stream->wrap || !stream->decorate) {
		return ZLOG_TRUE;
	}

	if (suffix != NULL && final_suffix != NULL) {
		stream->msg_suffix_len = strlen(suffix);
		stream->msg_final_suffix_len = strlen(final_suffix);
		len = stream->msg_suffix_len + stream->msg_final_suffix_len + 2;
		if (stream->msg_suffix != NULL) {
			free(stream->msg_suffix);
		}
		stream->msg_suffix = malloc(len);
		if (stream->msg_suffix == NULL) {
			return ZLOG_FALSE;
		}
		stream->msg_final_suffix = stream->msg_suffix + stream->msg_suffix_len + 1;
		memcpy(stream->msg_suffix, suffix, stream->msg_suffix_len + 1);
		memcpy(stream->msg_final_suffix, final_suffix, stream->msg_final_suffix_len + 1);
		return ZLOG_TRUE;
	}
	if (suffix != NULL) {
		stream->msg_suffix_len = strlen(suffix);
		len = stream->msg_suffix_len + 1;
		stream->msg_suffix = malloc(len);
		if (stream->msg_suffix != NULL) {
			free(stream->msg_suffix);
		}
		if (stream->msg_suffix == NULL) {
			return ZLOG_FALSE;
		}
		memcpy(stream->msg_suffix, suffix, len);
		return ZLOG_TRUE;
	}
	if (final_suffix != NULL) {
		stream->msg_final_suffix_len = strlen(final_suffix);
		len = stream->msg_final_suffix_len + 1;
		stream->msg_final_suffix = malloc(len);
		if (stream->msg_final_suffix != NULL) {
			free(stream->msg_suffix);
		}
		if (stream->msg_final_suffix == NULL) {
			return ZLOG_FALSE;
		}
		memcpy(stream->msg_final_suffix, final_suffix, len);
		return ZLOG_TRUE;
	}

	return ZLOG_TRUE;
}
/* }}} */

ssize_t zlog_stream_prefix_ex(struct zlog_stream *stream, const char *function, int line) /* {{{ */
{
	size_t len;

	if (!stream->prefix_buffer) {
		return 0;
	}
	if (stream->wrap && stream->function == NULL) {
		stream->function = function;
		stream->line = line;
	}

	if (stream->use_buffer) {
		if (!zlog_stream_buf_alloc(stream)) {
			return -1;
		}
		len = zlog_buf_prefix(
				function, line, stream->flags,
				stream->buf.data, stream->buf.size, stream->use_syslog);
		stream->len = stream->prefix_len = len;
		if (stream->msg_prefix != NULL) {
			zlog_stream_buf_copy_cstr(stream, stream->msg_prefix, stream->msg_prefix_len);
		}
		if (stream->msg_quote) {
			zlog_stream_buf_copy_char(stream, '"');
		}
		return stream->len;
	} else {
		char sbuf[1024];
		ssize_t written;
		len = zlog_buf_prefix(function, line, stream->flags, sbuf, 1024, stream->use_syslog);
		written = zlog_stream_direct_write(stream, sbuf, len);
		if (stream->msg_prefix != NULL) {
			written += zlog_stream_direct_write(
					stream, stream->msg_prefix, stream->msg_prefix_len);
		}
		if (stream->msg_quote) {
			written += zlog_stream_direct_write(stream, "\"", 1);
		}
		return written;
	}
}
/* }}} */

ssize_t zlog_stream_vformat(struct zlog_stream *stream, const char *fmt, va_list args) /* {{{ */
{
	char sbuf[1024];
	size_t len;

	len = vsnprintf(sbuf, 1024, fmt, args);

	return zlog_stream_str(stream, sbuf, len);
}
/* }}} */

ssize_t zlog_stream_format(struct zlog_stream *stream, const char *fmt, ...) /* {{{ */
{
	ssize_t len;

	va_list args;
	va_start(args, fmt);
	len = zlog_stream_vformat(stream, fmt, args);
	va_end(args);

	return len;
}
/* }}} */

ssize_t zlog_stream_str(struct zlog_stream *stream, const char *str, size_t str_len) /* {{{ */
{
	/* do not write anything if the stream is full or str is empty */
	if (str_len == 0 || stream->full) {
		return 0;
	}

	/* reset stream if it is finished */
	if (stream->finished) {
		stream->finished = 0;
		stream->len = 0;
		stream->full = 0;
	}

	if (stream->use_buffer) {
		return zlog_stream_buf_append(stream, str, str_len);
	}

	return zlog_stream_unbuffered_write(stream, str, str_len);
}
/* }}} */

static inline void zlog_stream_finish_buffer_suffix(struct zlog_stream *stream) /* {{{ */
{
	if (stream->msg_quote) {
		zlog_stream_buf_copy_char(stream, '"');
	}
	if (stream->msg_suffix != NULL) {
		zlog_stream_buf_copy_cstr(stream, stream->msg_suffix, stream->msg_suffix_len);
	}
	if (stream->msg_final_suffix != NULL) {
		if (stream->len + stream->msg_final_suffix_len >= zlog_limit) {
			zlog_bool quoting = stream->msg_quote;
			size_t final_suffix_wrap = stream->len + stream->msg_final_suffix_len + 1 - zlog_limit;
			zlog_stream_buf_copy_cstr(
					stream, stream->msg_final_suffix,
					stream->msg_final_suffix_len - final_suffix_wrap);
			zlog_stream_buf_copy_char(stream, '\n');
			zlog_stream_buf_flush(stream);
			stream->msg_quote = 0;
			zlog_stream_prefix_ex(stream, stream->function, stream->line);
			stream->msg_quote = quoting;
			zlog_stream_buf_copy_cstr(
					stream,
					stream->msg_final_suffix + (stream->msg_final_suffix_len - final_suffix_wrap),
					final_suffix_wrap);
			zlog_stream_buf_copy_char(stream, '\n');
		} else {
			zlog_stream_buf_copy_cstr(
					stream, stream->msg_final_suffix, stream->msg_final_suffix_len);
		}
	}
}
/* }}} */

static inline void zlog_stream_finish_direct_suffix(struct zlog_stream *stream) /* {{{ */
{
	if (stream->msg_quote) {
		zlog_stream_direct_write(stream, "\"", 1);
		++stream->len;
	}
	if (stream->msg_suffix != NULL) {
		/* we should always have space for wrap suffix so we don't have to check it */
		zlog_stream_direct_write(stream, stream->msg_suffix, stream->msg_suffix_len);
		stream->len += stream->msg_suffix_len;
	}
	if (stream->msg_final_suffix != NULL) {
		if (stream->len + stream->msg_final_suffix_len >= zlog_limit) {
			zlog_bool quoting = stream->msg_quote;
			size_t final_suffix_wrap = stream->len + stream->msg_final_suffix_len + 1 - zlog_limit;
			zlog_stream_direct_write_ex(
					stream, stream->msg_final_suffix,
					stream->msg_final_suffix_len - final_suffix_wrap, "\n", 1);
			stream->msg_quote = 0;
			zlog_stream_prefix_ex(stream, stream->function, stream->line);
			stream->msg_quote = quoting;
			zlog_stream_direct_write_ex(
					stream,
					stream->msg_final_suffix + (stream->msg_final_suffix_len - final_suffix_wrap),
					final_suffix_wrap, "\n", 1);
		} else {
			zlog_stream_direct_write_ex(
					stream, stream->msg_final_suffix, stream->msg_final_suffix_len, "\n", 1);
		}
	} else {
		zlog_stream_direct_write(stream, "\n", 1);
	}
}
/* }}} */

zlog_bool zlog_stream_finish(struct zlog_stream *stream) /* {{{ */
{
	if (stream->finished || stream->len == 0) {
		return ZLOG_TRUE;
	}

	if (stream->use_buffer) {
		if (stream->decorate) {
			zlog_stream_finish_buffer_suffix(stream);
		}
		zlog_stream_buf_flush(stream);
	} else {
		if (stream->decorate) {
			zlog_stream_finish_direct_suffix(stream);
		} else {
			zlog_stream_direct_write(stream, "\n", 1);
		}
	}
	stream->finished = 1;

	return ZLOG_TRUE;
}
/* }}} */

void zlog_stream_destroy(struct zlog_stream *stream) /* {{{ */
{
	if (stream->buf.data != NULL) {
		free(stream->buf.data);
	}
	if (stream->msg_prefix != NULL) {
		free(stream->msg_prefix);
	}
	if (stream->msg_suffix != NULL) {
		free(stream->msg_suffix);
	} else if (stream->msg_final_suffix != NULL) {
		free(stream->msg_final_suffix);
	}
}
/* }}} */

zlog_bool zlog_stream_close(struct zlog_stream *stream) /* {{{ */
{
	zlog_bool finished = zlog_stream_finish(stream);
	zlog_stream_destroy(stream);

	return finished;
}
/* }}} */
