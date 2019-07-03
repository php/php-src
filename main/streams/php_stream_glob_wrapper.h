/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
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
