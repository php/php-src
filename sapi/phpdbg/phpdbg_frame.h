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

#ifndef PHPDBG_FRAME_H
#define PHPDBG_FRAME_H

#include "TSRM.h"

zend_string *phpdbg_compile_stackframe(zend_execute_data *);
void phpdbg_restore_frame(void);
void phpdbg_switch_frame(int);
void phpdbg_dump_backtrace(size_t);
void phpdbg_open_generator_frame(zend_generator *);

#endif /* PHPDBG_FRAME_H */
