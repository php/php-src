/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Hartmut Holzgraefe <hartmut@six.de>                         |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_ncurses.h"

/* ncurses_functions[]
 *
 * Every user visible function must have an entry in ncurses_functions[].
 */
function_entry ncurses_functions[] = {
	PHP_FE(confirm_ncurses_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(ncurses_init,    NULL)
	PHP_FE(ncurses_end,    NULL)
	PHP_FE(ncurses_getch,    NULL)
	PHP_FE(ncurses_move,    NULL)
	PHP_FE(ncurses_addch,    NULL)
	PHP_FE(ncurses_refresh,    NULL)
	PHP_FE(ncurses_has_colors,    NULL)
	PHP_FE(ncurses_start_color,    NULL)
	PHP_FE(ncurses_color_set,    NULL)
	PHP_FE(ncurses_init_pair,    NULL)
	PHP_FE(ncurses_newwin,    NULL)
	PHP_FE(ncurses_delwin,    NULL)
	{NULL, NULL, NULL}	/* Must be the last line in ncurses_functions[] */
};
