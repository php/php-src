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
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_LIST_H
#define PHPDBG_LIST_H

#include "TSRM.h"
#include "phpdbg_cmd.h"

#define PHPDBG_LIST(name)         PHPDBG_COMMAND(list_##name)
#define PHPDBG_LIST_HANDLER(name) PHPDBG_COMMAND_HANDLER(list_##name)

PHPDBG_LIST(lines);
PHPDBG_LIST(class);
PHPDBG_LIST(method);
PHPDBG_LIST(func);

void phpdbg_list_function_byname(const char *, size_t);
void phpdbg_list_function(const zend_function *);
void phpdbg_list_file(zend_string *, uint32_t, int, uint32_t);

extern const phpdbg_command_t phpdbg_list_commands[];

void phpdbg_init_list(void);
void phpdbg_list_update(void);

typedef struct {
	char *buf;
	size_t len;
	zend_op_array op_array;
	uint32_t lines;
	uint32_t line[1];
} phpdbg_file_source;

#endif /* PHPDBG_LIST_H */
