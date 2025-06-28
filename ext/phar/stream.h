/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
  +----------------------------------------------------------------------+
  | Authors: Gregory Beaver <cellog@php.net>                             |
  |          Marcus Boerger <helly@php.net>                              |
  +----------------------------------------------------------------------+
*/

BEGIN_EXTERN_C()
#include "ext/standard/url.h"

php_url* phar_parse_url(php_stream_wrapper *wrapper, const char *filename, const char *mode, int options);
ZEND_ATTRIBUTE_NONNULL void phar_entry_remove(phar_entry_data *idata, char **error);

static php_stream* phar_wrapper_open_url(php_stream_wrapper *wrapper, const char *path, const char *mode, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC);
static int phar_wrapper_rename(php_stream_wrapper *wrapper, const char *url_from, const char *url_to, int options, php_stream_context *context);
static int phar_wrapper_unlink(php_stream_wrapper *wrapper, const char *url, int options, php_stream_context *context);
static int phar_wrapper_stat(php_stream_wrapper *wrapper, const char *url, int flags, php_stream_statbuf *ssb, php_stream_context *context);

/* file/stream handlers */
static ssize_t phar_stream_write(php_stream *stream, const char *buf, size_t count);
static ssize_t phar_stream_read( php_stream *stream, char *buf, size_t count);
static int    phar_stream_close(php_stream *stream, int close_handle);
static int    phar_stream_flush(php_stream *stream);
static int    phar_stream_seek( php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffset);
static int    phar_stream_stat( php_stream *stream, php_stream_statbuf *ssb);
END_EXTERN_C()
