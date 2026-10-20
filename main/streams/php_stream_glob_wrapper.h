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
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

PHPAPI extern const php_stream_wrapper php_glob_stream_wrapper;
PHPAPI extern const php_stream_ops     php_glob_stream_ops;

BEGIN_EXTERN_C()

PHPAPI char* _php_glob_stream_get_path(php_stream *stream, size_t *plen STREAMS_DC);
#define php_glob_stream_get_path(stream, plen)	_php_glob_stream_get_path((stream), (plen) STREAMS_CC)

PHPAPI char* _php_glob_stream_get_pattern(php_stream *stream, size_t *plen STREAMS_DC);
#define php_glob_stream_get_pattern(stream, plen)	_php_glob_stream_get_pattern((stream), (plen) STREAMS_CC)

PHPAPI int   _php_glob_stream_get_count(php_stream *stream, int *pflags STREAMS_DC);
#define php_glob_stream_get_count(stream, pflags)	_php_glob_stream_get_count((stream), (pflags) STREAMS_CC)

END_EXTERN_C()
