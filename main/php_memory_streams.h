/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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

#define php_stream_memory_create(mode) _php_stream_memory_create((mode) STREAMS_REL_CC);
#define php_stream_memory_open(mode, buf, length) _php_stream_memory_open((mode), (buf), (length) STREAMS_REL_CC);
#define php_stream_memory_get_buffer(stream, length) _php_stream_memory_get_buffer((stream), (length) STREAMS_REL_CC);

#define php_stream_temp_create(mode, max_memory_usage) _php_stream_temp_create((mode), (max_memory_usage) STREAMS_REL_CC);
#define php_stream_temp_open(mode, max_memory_usage, buf, length) _php_stream_temp_open((mode), (max_memory_usage), (buf), (length) STREAMS_REL_CC);


PHPAPI php_stream *_php_stream_memory_create(int mode STREAMS_DC);
PHPAPI php_stream *_php_stream_memory_open(int mode, char *buf, size_t length STREAMS_DC);
PHPAPI char *_php_stream_memory_get_buffer(php_stream *stream, size_t *length STREAMS_DC);

PHPAPI php_stream *_php_stream_temp_create(int mode, size_t max_memory_usage STREAMS_DC);
PHPAPI php_stream *_php_stream_temp_open(int mode, size_t max_memory_usage, char *buf, size_t length STREAMS_DC);

#define TEMP_STREAM_DEFAULT  0
#define TEMP_STREAM_READONLY 1

extern php_stream_ops php_stream_memory_ops;
extern php_stream_ops php_stream_temp_ops;

#define PHP_STREAM_IS_MEMORY &php_stream_memory_ops
#define PHP_STREAM_IS_TEMP   &php_stream_temp_ops

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
