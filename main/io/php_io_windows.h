/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Jakub Zelenka <bukka@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_IO_WINDOWS_H
#define PHP_IO_WINDOWS_H

zend_result php_io_windows_copy(php_io_fd *src, php_io_fd *dest, size_t maxlen, size_t *copied);

#define PHP_IO_PLATFORM_COPY php_io_windows_copy
#define PHP_IO_PLATFORM_NAME "windows"

/* proc_open()'s pipes: an overlapped named pipe under a provider, an anonymous pipe otherwise */
zend_result php_io_windows_pipe_create(HANDLE pair[2], bool parent_reads, bool *overlapped);
php_stream *php_io_windows_pipe_stream(HANDLE end, int mode_flags, const char *mode, bool overlapped);
zend_result php_io_windows_pipe_stream_release(php_stream *stream);
void php_io_windows_pipe_stream_handed_out(php_stream *stream);

#define PHP_IO_PLATFORM_PIPE_CREATE php_io_windows_pipe_create
#define PHP_IO_PLATFORM_PIPE_STREAM php_io_windows_pipe_stream
#define PHP_IO_PLATFORM_PIPE_STREAM_RELEASE php_io_windows_pipe_stream_release
#define PHP_IO_PLATFORM_PIPE_STREAM_HANDED_OUT php_io_windows_pipe_stream_handed_out

#endif /* PHP_IO_WINDOWS_H */
