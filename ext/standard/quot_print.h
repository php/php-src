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
   | Author: Kirill Maximov (kir@rus.net)                                 |
   +----------------------------------------------------------------------+
*/

#ifndef QUOT_PRINT_H
#define QUOT_PRINT_H

PHPAPI zend_string *php_quot_print_decode(const unsigned char *str, size_t length, int replace_us_by_ws);
PHPAPI zend_string *php_quot_print_encode(const unsigned char *str, size_t length);

#endif /* QUOT_PRINT_H */
