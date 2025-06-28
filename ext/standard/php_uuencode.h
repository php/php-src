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
   | Author: Ilia Alshanetsky <ilia@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_UUENCODE_H
#define PHP_UUENCODE_H

PHPAPI zend_string *php_uudecode(const char *src, size_t src_len);
PHPAPI zend_string *php_uuencode(const char *src, size_t src_len);

#endif /* PHP_UUENCODE_H */
