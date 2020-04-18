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
   | Author: Kirill Maximov (kir@rus.net)                                 |
   +----------------------------------------------------------------------+
*/

#ifndef QUOT_PRINT_H
#define QUOT_PRINT_H

PHPAPI zend_string *php_quot_print_decode(const unsigned char *str, size_t length, int replace_us_by_ws);
PHPAPI zend_string *php_quot_print_encode(const unsigned char *str, size_t length);

#endif /* QUOT_PRINT_H */
