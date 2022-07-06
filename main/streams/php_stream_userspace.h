/*
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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

/* for user-space streams */
PHPAPI extern const php_stream_ops php_stream_userspace_ops;
PHPAPI extern const php_stream_ops php_stream_userspace_dir_ops;
#define PHP_STREAM_IS_USERSPACE	&php_stream_userspace_ops
#define PHP_STREAM_IS_USERSPACE_DIR	&php_stream_userspace_dir_ops
