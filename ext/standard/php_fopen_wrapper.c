/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Jim Winstead <jimw@php.net>                                 |
   |          Hartmut Holzgraefe <hholzgra@php.net>                       |
   +----------------------------------------------------------------------+
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "php.h"
#include "php_globals.h"
#include "php_standard.h"
#include "php_memory_streams.h"
#include "php_fopen_wrappers.h"
#include "SAPI.h"

static ssize_t php_stream_output_write(php_stream *stream, const char *buf, size_t count) /* {{{ */
{
	PHPWRITE(buf, count);
	return count;
}
/* }}} */

static ssize_t php_stream_output_read(php_stream *stream, char *buf, size_t count) /* {{{ */
{
	stream->eof = 1;
	return -1;
}
/* }}} */

static int php_stream_output_close(php_stream *stream, int close_handle) /* {{{ */
{
	return 0;
}
/* }}} */

const php_stream_ops php_stream_output_ops = {
	php_stream_output_write,
	php_stream_output_read,
	php_stream_output_close,
	NULL, /* flush */
	"Output",
	NULL, /* seek */
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set_option */
};

typedef struct php_stream_input { /* {{{ */
	php_stream *body;
	zend_off_t position;
} php_stream_input_t;
/* }}} */

static ssize_t php_stream_input_write(php_stream *stream, const char *buf, size_t count) /* {{{ */
{
	return -1;
}
/* }}} */

static ssize_t php_stream_input_read(php_stream *stream, char *buf, size_t count) /* {{{ */
{
	php_stream_input_t *input = stream->abstract;
	ssize_t read;

	if (!SG(post_read) && SG(read_post_bytes) < (int64_t)(input->position + count)) {
		/* read requested data from SAPI */
		size_t read_bytes = sapi_read_post_block(buf, count);

		if (read_bytes > 0) {
			php_stream_seek(input->body, 0, SEEK_END);
			php_stream_write(input->body, buf, read_bytes);
		}
	}

	if (!input->body->readfilters.head) {
		/* If the input stream contains filters, it's not really seekable. The
			input->position is likely to be wrong for unfiltered data. */
		php_stream_seek(input->body, input->position, SEEK_SET);
	}
	read = php_stream_read(input->body, buf, count);

	if (!read || read == (size_t) -1) {
		stream->eof = 1;
	} else {
		input->position += read;
	}

	return read;
}
/* }}} */

static int php_stream_input_close(php_stream *stream, int close_handle) /* {{{ */
{
	efree(stream->abstract);
	stream->abstract = NULL;

	return 0;
}
/* }}} */

static int php_stream_input_flush(php_stream *stream) /* {{{ */
{
	return -1;
}
/* }}} */

static int php_stream_input_seek(php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffset) /* {{{ */
{
	php_stream_input_t *input = stream->abstract;

	if (input->body) {
		int sought = php_stream_seek(input->body, offset, whence);
		*newoffset = input->position = (input->body)->position;
		return sought;
	}

	return -1;
}
/* }}} */

const php_stream_ops php_stream_input_ops = {
	php_stream_input_write,
	php_stream_input_read,
	php_stream_input_close,
	php_stream_input_flush,
	"Input",
	php_stream_input_seek,
	NULL, /* cast */
	NULL, /* stat */
	NULL  /* set_option */
};

static void php_stream_apply_filter_list(php_stream *stream, char *filterlist, int read_chain, int write_chain) /* {{{ */
{
	char *p, *token = NULL;
	php_stream_filter *temp_filter;

	p = php_strtok_r(filterlist, "|", &token);
	while (p) {
		php_url_decode(p, strlen(p));
		if (read_chain) {
			if ((temp_filter = php_stream_filter_create(p, NULL, php_stream_is_persistent(stream)))) {
				php_stream_filter_append(&stream->readfilters, temp_filter);
			} else {
				php_error_docref(NULL, E_WARNING, "Unable to create filter (%s)", p);
			}
		}
		if (write_chain) {
			if ((temp_filter = php_stream_filter_create(p, NULL, php_stream_is_persistent(stream)))) {
				php_stream_filter_append(&stream->writefilters, temp_filter);
			} else {
				php_error_docref(NULL, E_WARNING, "Unable to create filter (%s)", p);
			}
		}
		p = php_strtok_r(NULL, "|", &token);
	}
}
/* }}} */

php_stream * php_stream_url_wrap_php(php_stream_wrapper *wrapper, const char *path, const char *mode, int options,
									 zend_string **opened_path, php_stream_context *context STREAMS_DC) /* {{{ */
{
	int fd = -1;
	int mode_rw = 0;
	php_stream * stream = NULL;
	char *p, *token = NULL, *pathdup;
	zend_long max_memory;
	FILE *file = NULL;
#ifdef PHP_WIN32
	int pipe_requested = 0;
#endif

	if (!strncasecmp(path, "php://", 6)) {
		path += 6;
	}

	if (!strncasecmp(path, "temp", 4)) {
		path += 4;
		max_memory = PHP_STREAM_MAX_MEM;
		if (!strncasecmp(path, "/maxmemory:", 11)) {
			path += 11;
			max_memory = ZEND_STRTOL(path, NULL, 10);
			if (max_memory < 0) {
				zend_argument_value_error(2, "must be greater than or equal to 0");
				return NULL;
			}
		}
		mode_rw = php_stream_mode_from_str(mode);
		return php_stream_temp_create(mode_rw, max_memory);
	}

	if (!strcasecmp(path, "memory")) {
		mode_rw = php_stream_mode_from_str(mode);
		return php_stream_memory_create(mode_rw);
	}

	if (!strcasecmp(path, "output")) {
		return php_stream_alloc(&php_stream_output_ops, NULL, 0, "wb");
	}

	if (!strcasecmp(path, "input")) {
		php_stream_input_t *input;

		if ((options & STREAM_OPEN_FOR_INCLUDE) && !PG(allow_url_include) ) {
			if (options & REPORT_ERRORS) {
				php_error_docref(NULL, E_WARNING, "URL file-access is disabled in the server configuration");
			}
			return NULL;
		}

		input = ecalloc(1, sizeof(*input));
		if ((input->body = SG(request_info).request_body)) {
			php_stream_rewind(input->body);
		} else {
			input->body = php_stream_temp_create_ex(TEMP_STREAM_DEFAULT, SAPI_POST_BLOCK_SIZE, PG(upload_tmp_dir));
			SG(request_info).request_body = input->body;
		}

		return php_stream_alloc(&php_stream_input_ops, input, 0, "rb");
	}

	if (!strcasecmp(path, "stdin")) {
		if ((options & STREAM_OPEN_FOR_INCLUDE) && !PG(allow_url_include) ) {
			if (options & REPORT_ERRORS) {
				php_error_docref(NULL, E_WARNING, "URL file-access is disabled in the server configuration");
			}
			return NULL;
		}
		if (!strcmp(sapi_module.name, "cli")) {
			static int cli_in = 0;
			fd = STDIN_FILENO;
			if (cli_in) {
				fd = dup(fd);
			} else {
				cli_in = 1;
				file = stdin;
			}
		} else {
			fd = dup(STDIN_FILENO);
		}
#ifdef PHP_WIN32
		pipe_requested = 1;
#endif
	} else if (!strcasecmp(path, "stdout")) {
		if (!strcmp(sapi_module.name, "cli")) {
			static int cli_out = 0;
			fd = STDOUT_FILENO;
			if (cli_out++) {
				fd = dup(fd);
			} else {
				cli_out = 1;
				file = stdout;
			}
		} else {
			fd = dup(STDOUT_FILENO);
		}
#ifdef PHP_WIN32
		pipe_requested = 1;
#endif
	} else if (!strcasecmp(path, "stderr")) {
		if (!strcmp(sapi_module.name, "cli")) {
			static int cli_err = 0;
			fd = STDERR_FILENO;
			if (cli_err++) {
				fd = dup(fd);
			} else {
				cli_err = 1;
				file = stderr;
			}
		} else {
			fd = dup(STDERR_FILENO);
		}
#ifdef PHP_WIN32
		pipe_requested = 1;
#endif
	} else if (!strncasecmp(path, "fd/", 3)) {
		const char *start;
		char       *end;
		zend_long  fildes_ori;
		int		   dtablesize;

		if (strcmp(sapi_module.name, "cli")) {
			if (options & REPORT_ERRORS) {
				php_error_docref(NULL, E_WARNING, "Direct access to file descriptors is only available from command-line PHP");
			}
			return NULL;
		}

		if ((options & STREAM_OPEN_FOR_INCLUDE) && !PG(allow_url_include) ) {
			if (options & REPORT_ERRORS) {
				php_error_docref(NULL, E_WARNING, "URL file-access is disabled in the server configuration");
			}
			return NULL;
		}

		start = &path[3];
		fildes_ori = ZEND_STRTOL(start, &end, 10);
		if (end == start || *end != '\0') {
			php_stream_wrapper_log_error(wrapper, options,
				"php://fd/ stream must be specified in the form php://fd/<orig fd>");
			return NULL;
		}

#ifdef HAVE_UNISTD_H
		dtablesize = getdtablesize();
#else
		dtablesize = INT_MAX;
#endif

		if (fildes_ori < 0 || fildes_ori >= dtablesize) {
			php_stream_wrapper_log_error(wrapper, options,
				"The file descriptors must be non-negative numbers smaller than %d", dtablesize);
			return NULL;
		}

		fd = dup((int)fildes_ori);
		if (fd == -1) {
			php_stream_wrapper_log_error(wrapper, options,
				"Error duping file descriptor " ZEND_LONG_FMT "; possibly it doesn't exist: "
				"[%d]: %s", fildes_ori, errno, strerror(errno));
			return NULL;
		}
	} else if (!strncasecmp(path, "filter/", 7)) {
		/* Save time/memory when chain isn't specified */
		if (strchr(mode, 'r') || strchr(mode, '+')) {
			mode_rw |= PHP_STREAM_FILTER_READ;
		}
		if (strchr(mode, 'w') || strchr(mode, '+') || strchr(mode, 'a')) {
			mode_rw |= PHP_STREAM_FILTER_WRITE;
		}
		pathdup = estrndup(path + 6, strlen(path + 6));
		p = strstr(pathdup, "/resource=");
		if (!p) {
			zend_throw_error(NULL, "No URL resource specified");
			efree(pathdup);
			return NULL;
		}

		if (!(stream = php_stream_open_wrapper(p + 10, mode, options, opened_path))) {
			efree(pathdup);
			return NULL;
		}

		*p = '\0';

		p = php_strtok_r(pathdup + 1, "/", &token);
		while (p) {
			if (!strncasecmp(p, "read=", 5)) {
				php_stream_apply_filter_list(stream, p + 5, 1, 0);
			} else if (!strncasecmp(p, "write=", 6)) {
				php_stream_apply_filter_list(stream, p + 6, 0, 1);
			} else {
				php_stream_apply_filter_list(stream, p, mode_rw & PHP_STREAM_FILTER_READ, mode_rw & PHP_STREAM_FILTER_WRITE);
			}
			p = php_strtok_r(NULL, "/", &token);
		}
		efree(pathdup);

		if (EG(exception)) {
			php_stream_close(stream);
			return NULL;
		}

		return stream;
	} else {
		/* invalid php://thingy */
		php_error_docref(NULL, E_WARNING, "Invalid php:// URL specified");
		return NULL;
	}

	/* must be stdin, stderr or stdout */
	if (fd == -1)	{
		/* failed to dup */
		return NULL;
	}

#if defined(S_IFSOCK) && !defined(PHP_WIN32)
	do {
		zend_stat_t st = {0};
		memset(&st, 0, sizeof(st));
		if (zend_fstat(fd, &st) == 0 && (st.st_mode & S_IFMT) == S_IFSOCK) {
			stream = php_stream_sock_open_from_socket(fd, NULL);
			if (stream) {
				stream->ops = &php_stream_socket_ops;
				return stream;
			}
		}
	} while (0);
#endif

	if (file) {
		stream = php_stream_fopen_from_file(file, mode);
	} else {
		stream = php_stream_fopen_from_fd(fd, mode, NULL);
		if (stream == NULL) {
			close(fd);
		}
	}

#ifdef PHP_WIN32
	if (pipe_requested && stream && context) {
		zval *blocking_pipes = php_stream_context_get_option(context, "pipe", "blocking");
		if (blocking_pipes) {
			php_stream_set_option(stream, PHP_STREAM_OPTION_PIPE_BLOCKING, zval_get_long(blocking_pipes), NULL);
		}
	}
#endif
	return stream;
}
/* }}} */

static const php_stream_wrapper_ops php_stdio_wops = {
	php_stream_url_wrap_php,
	NULL, /* close */
	NULL, /* fstat */
	NULL, /* stat */
	NULL, /* opendir */
	"PHP",
	NULL, /* unlink */
	NULL, /* rename */
	NULL, /* mkdir */
	NULL, /* rmdir */
	NULL
};

PHPAPI const php_stream_wrapper php_stream_php_wrapper =	{
	&php_stdio_wops,
	NULL,
	0, /* is_url */
};
