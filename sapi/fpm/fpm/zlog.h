	/* (c) 2004-2007 Andrei Nigmatulin */

#ifndef ZLOG_H
#define ZLOG_H 1

#include <stdarg.h>
#include <sys/types.h>

#define zlog(flags,...) zlog_ex(__func__, __LINE__, flags, __VA_ARGS__)
#define zlog_msg(flags, prefix, msg) zlog_msg_ex(__func__, __LINE__, flags, prefix, msg)

struct timeval;

typedef unsigned char zlog_bool;

#define ZLOG_TRUE 1
#define ZLOG_FALSE 0

void zlog_set_external_logger(void (*logger)(int, char *, size_t));
int zlog_set_fd(int new_fd);
int zlog_set_level(int new_value);
int zlog_set_limit(int new_value);
int zlog_set_buffering(zlog_bool buffering);
const char *zlog_get_level_name(int log_level);
void zlog_set_launched(void);

size_t zlog_print_time(struct timeval *tv, char *timebuf, size_t timebuf_len);

void vzlog(const char *function, int line, int flags, const char *fmt, va_list args);
void zlog_ex(const char *function, int line, int flags, const char *fmt, ...)
		__attribute__ ((format(printf,4,5)));

void zlog_msg_ex(const char *function, int line, int flags,
		const char *prefix, const char *msg);

#ifdef HAVE_SYSLOG_H
extern const int syslog_priorities[];
#endif

/* keep this same as FCGI_ERROR */
enum {
	ZLOG_DEBUG			= 1,
	ZLOG_NOTICE			= 2,
	ZLOG_WARNING		= 3,
	ZLOG_ERROR			= 4,
	ZLOG_ALERT			= 5,
};

#define ZLOG_LEVEL_MASK 7

#define ZLOG_HAVE_ERRNO 0x100

#define ZLOG_SYSERROR (ZLOG_ERROR | ZLOG_HAVE_ERRNO)

#define ZLOG_SYSLOG -2

/* STREAM */

struct zlog_stream_buffer {
	char *data;
	size_t size;
};

struct zlog_stream {
	int flags;
	unsigned int use_syslog:1;
	unsigned int use_fd:1;
	unsigned int use_buffer:1;
	unsigned int use_stderr:1;
	unsigned int prefix_buffer:1;
	unsigned int finished:1;
	unsigned int full:1;
	unsigned int wrap:1;
	unsigned int msg_quote:1;
	unsigned int decorate:1;
	int fd;
	int line;
	const char *function;
	struct zlog_stream_buffer buf;
	size_t len;
	size_t buf_init_size;
	size_t prefix_len;
	char *msg_prefix;
	size_t msg_prefix_len;
	char *msg_suffix;
	size_t msg_suffix_len;
	char *msg_final_suffix;
	size_t msg_final_suffix_len;
};

void zlog_stream_init(struct zlog_stream *stream, int flags);
void zlog_stream_init_ex(struct zlog_stream *stream, int flags, int fd);
void zlog_stream_set_decorating(struct zlog_stream *stream, zlog_bool decorate);
void zlog_stream_set_wrapping(struct zlog_stream *stream, zlog_bool wrap);
void zlog_stream_set_msg_quoting(struct zlog_stream *stream, zlog_bool quote);
zlog_bool zlog_stream_set_msg_prefix(struct zlog_stream *stream, const char *fmt, ...)
		__attribute__ ((format(printf,2,3)));
zlog_bool zlog_stream_set_msg_suffix(
		struct zlog_stream *stream, const char *suffix, const char *final_suffix);
#define zlog_stream_prefix(stream) \
	zlog_stream_prefix_ex(stream, __func__, __LINE__)
ssize_t zlog_stream_prefix_ex(struct zlog_stream *stream, const char *function, int line);
ssize_t zlog_stream_format(struct zlog_stream *stream, const char *fmt, ...)
		__attribute__ ((format(printf,2,3)));
ssize_t zlog_stream_vformat(struct zlog_stream *stream, const char *fmt, va_list args);
ssize_t zlog_stream_str(struct zlog_stream *stream, const char *str, size_t str_len);
zlog_bool zlog_stream_finish(struct zlog_stream *stream);
void zlog_stream_destroy(struct zlog_stream *stream);
zlog_bool zlog_stream_close(struct zlog_stream *stream);

/* default log limit */
#define ZLOG_DEFAULT_LIMIT 1024
/* minimum log limit */
#define ZLOG_MIN_LIMIT 512
/* default log buffering */
#define ZLOG_DEFAULT_BUFFERING 1

#endif
