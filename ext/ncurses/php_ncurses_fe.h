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
PHP_FUNCTION(ncurses_standout);
PHP_FUNCTION(ncurses_standend);
PHP_FUNCTION(ncurses_baudrate);
PHP_FUNCTION(ncurses_beep);
PHP_FUNCTION(ncurses_can_change_color);
PHP_FUNCTION(ncurses_cbreak);
PHP_FUNCTION(ncurses_clear);
PHP_FUNCTION(ncurses_clrtobot);
PHP_FUNCTION(ncurses_clrtoeol);
PHP_FUNCTION(ncurses_def_prog_mode);
PHP_FUNCTION(ncurses_def_shell_mode);
PHP_FUNCTION(ncurses_delch);
PHP_FUNCTION(ncurses_deleteln);
PHP_FUNCTION(ncurses_doupdate);
PHP_FUNCTION(ncurses_echo);
PHP_FUNCTION(ncurses_echochar);
PHP_FUNCTION(ncurses_erase);
PHP_FUNCTION(ncurses_erasechar);
PHP_FUNCTION(ncurses_flash);
PHP_FUNCTION(ncurses_flushinp);
PHP_FUNCTION(ncurses_has_ic);
PHP_FUNCTION(ncurses_has_il);
PHP_FUNCTION(ncurses_inch);
PHP_FUNCTION(ncurses_insertln);
PHP_FUNCTION(ncurses_isendwin);
PHP_FUNCTION(ncurses_killchar);
PHP_FUNCTION(ncurses_nl);
PHP_FUNCTION(ncurses_nocbreak);
PHP_FUNCTION(ncurses_noecho);
PHP_FUNCTION(ncurses_nonl);
PHP_FUNCTION(ncurses_noraw);
PHP_FUNCTION(ncurses_raw);
PHP_FUNCTION(ncurses_resetty);
PHP_FUNCTION(ncurses_savetty);
PHP_FUNCTION(ncurses_termattrs);
PHP_FUNCTION(ncurses_use_default_colors);
PHP_FUNCTION(ncurses_slk_attr);
PHP_FUNCTION(ncurses_slk_clear);
PHP_FUNCTION(ncurses_slk_noutrefresh);
PHP_FUNCTION(ncurses_slk_refresh);
PHP_FUNCTION(ncurses_slk_restore);
PHP_FUNCTION(ncurses_slk_touch);
PHP_FUNCTION(ncurses_attroff);
PHP_FUNCTION(ncurses_attron);
PHP_FUNCTION(ncurses_attrset);
PHP_FUNCTION(ncurses_bkgd);
PHP_FUNCTION(ncurses_curs_set);
PHP_FUNCTION(ncurses_delay_output);
PHP_FUNCTION(ncurses_halfdelay);
PHP_FUNCTION(ncurses_has_key);
PHP_FUNCTION(ncurses_insch);
PHP_FUNCTION(ncurses_insdelln);
PHP_FUNCTION(ncurses_mouseinterval);
PHP_FUNCTION(ncurses_napms);
PHP_FUNCTION(ncurses_scrl);
PHP_FUNCTION(ncurses_slk_attroff);
PHP_FUNCTION(ncurses_slk_attron);
PHP_FUNCTION(ncurses_slk_attrset);
PHP_FUNCTION(ncurses_slk_color);
PHP_FUNCTION(ncurses_slk_init);
PHP_FUNCTION(ncurses_typeahead);
PHP_FUNCTION(ncurses_ungetch);
PHP_FUNCTION(ncurses_vidattr);
PHP_FUNCTION(ncurses_use_extended_names);
PHP_FUNCTION(ncurses_bkgdset);
PHP_FUNCTION(ncurses_filter);
PHP_FUNCTION(ncurses_noqiflush);
PHP_FUNCTION(ncurses_qiflush);
PHP_FUNCTION(ncurses_timeout);
PHP_FUNCTION(ncurses_use_env);
#endif
