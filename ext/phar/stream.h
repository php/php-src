/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Gregory Beaver <cellog@php.net>                             |
  |          Marcus Boerger <helly@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

BEGIN_EXTERN_C()

php_url* phar_parse_url(php_stream_wrapper *wrapper, const char *filename, const char *mode, int options);
void phar_entry_remove(phar_entry_data *idata, char **error);

static php_stream* phar_wrapper_open_url(php_stream_wrapper *wrapper, const char *path, const char *mode, int options, char **opened_path, php_stream_context *context STREAMS_DC);
static int phar_wrapper_rename(php_stream_wrapper *wrapper, const char *url_from, const char *url_to, int options, php_stream_context *context);
static int phar_wrapper_unlink(php_stream_wrapper *wrapper, const char *url, int options, php_stream_context *context);
static int phar_wrapper_stat(php_stream_wrapper *wrapper, const char *url, int flags, php_stream_statbuf *ssb, php_stream_context *context);

/* file/stream handlers */
static size_t phar_stream_write(php_stream *stream, const char *buf, size_t count);
static size_t phar_stream_read( php_stream *stream, char *buf, size_t count);
static int    phar_stream_close(php_stream *stream, int close_handle);
static int    phar_stream_flush(php_stream *stream);
static int    phar_stream_seek( php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffset);
static int    phar_stream_stat( php_stream *stream, php_stream_statbuf *ssb);
END_EXTERN_C()

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
