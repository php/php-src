/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stig Bakken <ssb@fast.no>                                   |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_TICKS_H
#define PHP_TICKS_H

struct _php_tick_function_entry {
	void (*func)(int count);
	struct _php_tick_function_entry *next;
};

int php_startup_ticks(PLS_D);
void php_shutdown_ticks(PLS_D);
void php_run_ticks(int count);
PHPAPI int php_add_tick_function(void (*func)(int count));
PHPAPI int php_remove_tick_function(void (*func)(int count));

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
