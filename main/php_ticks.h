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
   | Author: Stig Bakken <ssb@php.net>                                    |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_TICKS_H
#define PHP_TICKS_H

void php_startup_ticks(void);
void php_deactivate_ticks(void);
void php_shutdown_ticks(php_core_globals *core_globals);
void php_run_ticks(int count);

BEGIN_EXTERN_C()
PHPAPI void php_add_tick_function(void (*func)(int, void *), void *arg);
PHPAPI void php_remove_tick_function(void (*func)(int, void *), void * arg);
END_EXTERN_C()

#endif
