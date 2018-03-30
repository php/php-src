/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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

char *phpdbg_decode_opline(zend_op_array *ops, zend_op *op);
void phpdbg_print_opline(zend_execute_data *execute_data, zend_bool ignore_flags);
void phpdbg_print_opline_ex(zend_execute_data *execute_data, zend_bool ignore_flags);

typedef struct _phpdbg_oplog_entry phpdbg_oplog_entry;
struct _phpdbg_oplog_entry {
	phpdbg_oplog_entry *next;
	zend_string *function_name;
	zend_class_entry *scope;
	zend_string *filename;
	zend_op *opcodes;
	zend_op *op;
};

typedef struct _phpdbg_oplog_list phpdbg_oplog_list;
struct _phpdbg_oplog_list {
	phpdbg_oplog_list *prev;
	phpdbg_oplog_entry *start;
};

#endif /* PHPDBG_OPCODE_H */
