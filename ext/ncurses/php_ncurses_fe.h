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

#ifndef PHP_NCURSES_FE_H
#define PHP_NCURSES_FE_H

PHP_FUNCTION(confirm_ncurses_compiled);	/* For testing, remove later. */
PHP_FUNCTION(ncurses_init);
PHP_FUNCTION(ncurses_end);
PHP_FUNCTION(ncurses_getch);
PHP_FUNCTION(ncurses_move);
PHP_FUNCTION(ncurses_addch);
PHP_FUNCTION(ncurses_refresh);
PHP_FUNCTION(ncurses_has_colors);
PHP_FUNCTION(ncurses_start_color);
PHP_FUNCTION(ncurses_color_set);
PHP_FUNCTION(ncurses_init_pair);
PHP_FUNCTION(ncurses_newwin);
PHP_FUNCTION(ncurses_delwin);

#endif
