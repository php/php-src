/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
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
   |          Georg Richter <georg.richter@php-ev.de>                     |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_NCURSES_FE_H
#define PHP_NCURSES_FE_H

PHP_FUNCTION(ncurses_addch);
PHP_FUNCTION(ncurses_color_set);
PHP_FUNCTION(ncurses_delwin);
PHP_FUNCTION(ncurses_end);
PHP_FUNCTION(ncurses_getch);
PHP_FUNCTION(ncurses_has_colors);
PHP_FUNCTION(ncurses_init);
PHP_FUNCTION(ncurses_init_pair);
PHP_FUNCTION(ncurses_move);
PHP_FUNCTION(ncurses_newwin);
PHP_FUNCTION(ncurses_refresh);
PHP_FUNCTION(ncurses_start_color);
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
PHP_FUNCTION(ncurses_echochar);
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
PHP_FUNCTION(ncurses_slk_set);
PHP_FUNCTION(ncurses_typeahead);
PHP_FUNCTION(ncurses_ungetch);
PHP_FUNCTION(ncurses_vidattr);
PHP_FUNCTION(ncurses_wrefresh);
PHP_FUNCTION(ncurses_use_extended_names);
PHP_FUNCTION(ncurses_bkgdset);
PHP_FUNCTION(ncurses_filter);
PHP_FUNCTION(ncurses_noqiflush);
PHP_FUNCTION(ncurses_qiflush);
PHP_FUNCTION(ncurses_timeout);
PHP_FUNCTION(ncurses_use_env);
PHP_FUNCTION(ncurses_addstr);
PHP_FUNCTION(ncurses_putp);
PHP_FUNCTION(ncurses_scr_dump);
PHP_FUNCTION(ncurses_scr_init);
PHP_FUNCTION(ncurses_scr_restore);
PHP_FUNCTION(ncurses_scr_set);
PHP_FUNCTION(ncurses_mvaddch);
PHP_FUNCTION(ncurses_mvaddchnstr);
PHP_FUNCTION(ncurses_addchnstr);
PHP_FUNCTION(ncurses_mvaddchstr);
PHP_FUNCTION(ncurses_addchstr);
PHP_FUNCTION(ncurses_mvaddnstr);
PHP_FUNCTION(ncurses_addnstr);
PHP_FUNCTION(ncurses_mvaddstr);
PHP_FUNCTION(ncurses_mvdelch);
PHP_FUNCTION(ncurses_mvgetch);
PHP_FUNCTION(ncurses_mvinch);
PHP_FUNCTION(ncurses_mvwaddstr);
PHP_FUNCTION(ncurses_insstr);
PHP_FUNCTION(ncurses_instr);
PHP_FUNCTION(ncurses_mvhline);
PHP_FUNCTION(ncurses_mvcur);
PHP_FUNCTION(ncurses_init_color);
PHP_FUNCTION(ncurses_border);
PHP_FUNCTION(ncurses_assume_default_colors);
PHP_FUNCTION(ncurses_define_key);
PHP_FUNCTION(ncurses_hline);
PHP_FUNCTION(ncurses_vline);
PHP_FUNCTION(ncurses_keyok);
PHP_FUNCTION(ncurses_termname);
PHP_FUNCTION(ncurses_longname);
PHP_FUNCTION(ncurses_mousemask);
PHP_FUNCTION(ncurses_getmouse);
PHP_FUNCTION(ncurses_ungetmouse);
PHP_FUNCTION(ncurses_mouse_trafo);
PHP_FUNCTION(ncurses_wmouse_trafo);
PHP_FUNCTION(ncurses_waddstr);
PHP_FUNCTION(ncurses_wnoutrefresh);
PHP_FUNCTION(ncurses_wclear);
PHP_FUNCTION(ncurses_wcolor_set);
PHP_FUNCTION(ncurses_wgetch);
PHP_FUNCTION(ncurses_keypad);
PHP_FUNCTION(ncurses_wmove);

PHP_FUNCTION(ncurses_newpad);
PHP_FUNCTION(ncurses_prefresh);
PHP_FUNCTION(ncurses_pnoutrefresh);

PHP_FUNCTION(ncurses_wstandout);
PHP_FUNCTION(ncurses_wstandend);
PHP_FUNCTION(ncurses_wattrset);
PHP_FUNCTION(ncurses_wattron);
PHP_FUNCTION(ncurses_wattroff);
#if HAVE_NCURSES_PANEL
PHP_FUNCTION(ncurses_update_panels);
PHP_FUNCTION(ncurses_panel_window);
PHP_FUNCTION(ncurses_panel_below);
PHP_FUNCTION(ncurses_panel_above);
PHP_FUNCTION(ncurses_replace_panel);
PHP_FUNCTION(ncurses_move_panel);
PHP_FUNCTION(ncurses_bottom_panel);
PHP_FUNCTION(ncurses_top_panel);
PHP_FUNCTION(ncurses_show_panel);
PHP_FUNCTION(ncurses_hide_panel);
PHP_FUNCTION(ncurses_del_panel);
PHP_FUNCTION(ncurses_new_panel);
#endif


#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
