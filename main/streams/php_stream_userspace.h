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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

/* for user-space streams */
PHPAPI extern const php_stream_ops php_stream_userspace_ops;
PHPAPI extern const php_stream_ops php_stream_userspace_dir_ops;
#define PHP_STREAM_IS_USERSPACE	&php_stream_userspace_ops
#define PHP_STREAM_IS_USERSPACE_DIR	&php_stream_userspace_dir_ops
