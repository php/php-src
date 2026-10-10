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
   | Authors: Jakub Zelenka <bukka@php.net>                               |
   +----------------------------------------------------------------------+
*/

/* The pipes between the process and a child, as proc_open() makes them: pipe(2) and a plain
 * stream, unless the platform has something a provider can perform on (Windows, an overlapped
 * named pipe under a provider: php_io_pipe_windows.c) */

#include "php_io_internal.h"
#include "main/streams/php_stream_plain_wrapper.h"

#ifndef PHP_WIN32
#include <unistd.h>
#endif

PHPAPI zend_result php_io_pipe_create(php_io_pipe_handle pair[2], bool parent_reads, bool *overlapped)
{
#ifdef PHP_IO_PLATFORM_PIPE_CREATE
	return PHP_IO_PLATFORM_PIPE_CREATE(pair, parent_reads, overlapped);
#else
	*overlapped = false;
	return pipe(pair) == 0 ? SUCCESS : FAILURE;
#endif
}

PHPAPI php_stream *php_io_pipe_stream(php_io_pipe_handle end, int mode_flags, const char *mode, bool overlapped)
{
#ifdef PHP_IO_PLATFORM_PIPE_STREAM
	return PHP_IO_PLATFORM_PIPE_STREAM(end, mode_flags, mode, overlapped);
#else
	return php_stream_fopen_from_fd(end, mode, NULL);
#endif
}

PHPAPI zend_result php_io_pipe_stream_release(php_stream *stream)
{
#ifdef PHP_IO_PLATFORM_PIPE_STREAM_RELEASE
	return PHP_IO_PLATFORM_PIPE_STREAM_RELEASE(stream);
#else
	return SUCCESS;
#endif
}

PHPAPI void php_io_pipe_stream_handed_out(php_stream *stream)
{
#ifdef PHP_IO_PLATFORM_PIPE_STREAM_HANDED_OUT
	PHP_IO_PLATFORM_PIPE_STREAM_HANDED_OUT(stream);
#endif
}
