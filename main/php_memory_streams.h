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
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_MEMORY_STREAM_H
#define PHP_MEMORY_STREAM_H

#include "php_streams.h"

#define PHP_STREAM_MAX_MEM	2 * 1024 * 1024

#define TEMP_STREAM_DEFAULT     0x0
#define TEMP_STREAM_READONLY    0x1
#define TEMP_STREAM_TAKE_BUFFER 0x2
#define TEMP_STREAM_APPEND      0x4

#define php_stream_memory_create(mode) _php_stream_memory_create((mode) STREAMS_CC)
#define php_stream_memory_create_rel(mode) _php_stream_memory_create((mode) STREAMS_REL_CC)
#define php_stream_memory_open(mode, str) _php_stream_memory_open((mode), (str) STREAMS_CC)
#define php_stream_memory_get_buffer(stream) _php_stream_memory_get_buffer((stream) STREAMS_CC)

#define php_stream_temp_new() php_stream_temp_create(TEMP_STREAM_DEFAULT, PHP_STREAM_MAX_MEM)
#define php_stream_temp_create(mode, max_memory_usage) _php_stream_temp_create((mode), (max_memory_usage) STREAMS_CC)
#define php_stream_temp_create_ex(mode, max_memory_usage, tmpdir) _php_stream_temp_create_ex((mode), (max_memory_usage), (tmpdir) STREAMS_CC)
#define php_stream_temp_create_rel(mode, max_memory_usage) _php_stream_temp_create((mode), (max_memory_usage) STREAMS_REL_CC)
#define php_stream_temp_open(mode, max_memory_usage, buf, length) _php_stream_temp_open((mode), (max_memory_usage), (buf), (length) STREAMS_CC)

BEGIN_EXTERN_C()

PHPAPI php_stream *_php_stream_memory_create(int mode STREAMS_DC);
PHPAPI php_stream *_php_stream_memory_open(int mode, zend_string *buf STREAMS_DC);
PHPAPI zend_string *_php_stream_memory_get_buffer(php_stream *stream STREAMS_DC);

PHPAPI php_stream *_php_stream_temp_create(int mode, size_t max_memory_usage STREAMS_DC);
PHPAPI php_stream *_php_stream_temp_create_ex(int mode, size_t max_memory_usage, const char *tmpdir STREAMS_DC);
PHPAPI php_stream *_php_stream_temp_open(int mode, size_t max_memory_usage, const char *buf, size_t length STREAMS_DC);

PHPAPI int php_stream_mode_from_str(const char *mode);
PHPAPI const char *_php_stream_mode_to_str(int mode);

END_EXTERN_C()

extern PHPAPI const php_stream_ops php_stream_memory_ops;
extern PHPAPI const php_stream_ops php_stream_temp_ops;
extern PHPAPI const php_stream_ops php_stream_rfc2397_ops;
extern PHPAPI const php_stream_wrapper php_stream_rfc2397_wrapper;

#define PHP_STREAM_IS_MEMORY &php_stream_memory_ops
#define PHP_STREAM_IS_TEMP   &php_stream_temp_ops

#endif
