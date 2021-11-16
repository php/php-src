/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
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
  | Authors: Gregory Beaver <cellog@php.net>                             |
  |          Marcus Boerger <helly@php.net>                              |
  +----------------------------------------------------------------------+
*/

BEGIN_EXTERN_C()
int phar_wrapper_mkdir(php_stream_wrapper *wrapper, const char *url_from, int mode, int options, php_stream_context *context);
int phar_wrapper_rmdir(php_stream_wrapper *wrapper, const char *url, int options, php_stream_context *context);

#ifdef PHAR_DIRSTREAM
php_url* phar_parse_url(php_stream_wrapper *wrapper, const char *filename, const char *mode, int options);

/* directory handlers */
static ssize_t phar_dir_write(php_stream *stream, const char *buf, size_t count);
static ssize_t phar_dir_read( php_stream *stream, char *buf, size_t count);
static int    phar_dir_close(php_stream *stream, int close_handle);
static int    phar_dir_flush(php_stream *stream);
static int    phar_dir_seek( php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffset);
#else
php_stream* phar_wrapper_open_dir(php_stream_wrapper *wrapper, const char *path, const char *mode, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC);
#endif
END_EXTERN_C()
