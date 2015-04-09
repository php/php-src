/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_OPCODE_H
#define PHPDBG_OPCODE_H

#include "zend_types.h"

const char *phpdbg_decode_opcode(zend_uchar);
char *phpdbg_decode_opline(zend_op_array *ops, zend_op *op, HashTable *vars);
void phpdbg_print_opline(zend_execute_data *execute_data, zend_bool ignore_flags);
void phpdbg_print_opline_ex(zend_execute_data *execute_data, HashTable *vars, zend_bool ignore_flags);

#endif /* PHPDBG_OPCODE_H */
