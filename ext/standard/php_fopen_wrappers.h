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

#ifndef PHP_FOPEN_WRAPPERS_H
#define PHP_FOPEN_WRAPPERS_H

php_stream *php_stream_url_wrap_http(php_stream_wrapper *wrapper, const char *path, const char *mode, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC);
php_stream *php_stream_url_wrap_ftp(php_stream_wrapper *wrapper, const char *path, const char *mode, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC);
extern PHPAPI const php_stream_wrapper php_stream_http_wrapper;
extern PHPAPI const php_stream_wrapper php_stream_ftp_wrapper;
extern PHPAPI const php_stream_wrapper php_stream_php_wrapper;
extern PHPAPI /*const*/ php_stream_wrapper php_plain_files_wrapper;

#endif
