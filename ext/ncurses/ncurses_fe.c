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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_ncurses.h"

static unsigned char first_args_force_ref[] = {1, BYREF_FORCE};
static unsigned char firstandsecond_args_force_ref[] = {2, BYREF_FORCE, BYREF_FORCE};
static unsigned char second_args_force_ref[] = {2, BYREF_NONE, BYREF_FORCE};
static unsigned char secondandthird_args_force_ref[] = {3, BYREF_NONE, BYREF_FORCE, BYREF_FORCE};

/* ncurses_functions[]
 *
 * Every user visible function must have an entry in ncurses_functions[].
 */
function_entry ncurses_functions[] = {
  PHP_FE(ncurses_addch, NULL)
  PHP_FE(ncurses_color_set, NULL)
  PHP_FE(ncurses_delwin, NULL)
  PHP_FE(ncurses_end, NULL)
  PHP_FE(ncurses_getch, NULL)
  PHP_FE(ncurses_has_colors, NULL)
  PHP_FE(ncurses_init, NULL)
  PHP_FE(ncurses_init_pair, NULL)
  PHP_FE(ncurses_move, NULL)
  PHP_FE(ncurses_newwin, NULL)
  PHP_FE(ncurses_refresh, NULL)
  PHP_FE(ncurses_start_color, NULL)
  PHP_FE(ncurses_standout, NULL)
  PHP_FE(ncurses_standend, NULL)
  PHP_FE(ncurses_baudrate, NULL)
  PHP_FE(ncurses_beep, NULL)
  PHP_FE(ncurses_can_change_color, NULL)
  PHP_FE(ncurses_cbreak, NULL)
  PHP_FE(ncurses_clear, NULL)
  PHP_FE(ncurses_clrtobot, NULL)
  PHP_FE(ncurses_clrtoeol, NULL)
  PHP_FE(ncurses_def_prog_mode, NULL)
  PHP_FE(ncurses_def_shell_mode, NULL)
  PHP_FE(ncurses_delch, NULL)
  PHP_FE(ncurses_deleteln, NULL)
  PHP_FE(ncurses_doupdate, NULL)
  PHP_FE(ncurses_echo, NULL)
  PHP_FE(ncurses_erase, NULL)
  PHP_FE(ncurses_erasechar, NULL)
  PHP_FE(ncurses_flash, NULL)
  PHP_FE(ncurses_flushinp, NULL)
  PHP_FE(ncurses_has_ic, NULL)
  PHP_FE(ncurses_has_il, NULL)
  PHP_FE(ncurses_inch, NULL)
  PHP_FE(ncurses_insertln, NULL)
  PHP_FE(ncurses_isendwin, NULL)
  PHP_FE(ncurses_killchar, NULL)
  PHP_FE(ncurses_nl, NULL)
  PHP_FE(ncurses_nocbreak, NULL)
  PHP_FE(ncurses_noecho, NULL)
  PHP_FE(ncurses_nonl, NULL)
  PHP_FE(ncurses_noraw, NULL)
  PHP_FE(ncurses_raw, NULL)
  PHP_FE(ncurses_resetty, NULL)
  PHP_FE(ncurses_savetty, NULL)
  PHP_FE(ncurses_termattrs, NULL)
  PHP_FE(ncurses_use_default_colors, NULL)
  PHP_FE(ncurses_slk_attr, NULL)
  PHP_FE(ncurses_slk_clear, NULL)
  PHP_FE(ncurses_slk_noutrefresh, NULL)
  PHP_FE(ncurses_slk_refresh, NULL)
  PHP_FE(ncurses_slk_restore, NULL)
  PHP_FE(ncurses_slk_touch, NULL)
  PHP_FE(ncurses_attroff, NULL)
  PHP_FE(ncurses_attron, NULL)
  PHP_FE(ncurses_attrset, NULL)
  PHP_FE(ncurses_bkgd, NULL)
  PHP_FE(ncurses_curs_set, NULL)
  PHP_FE(ncurses_delay_output, NULL)
  PHP_FE(ncurses_echochar, NULL)
  PHP_FE(ncurses_halfdelay, NULL)
  PHP_FE(ncurses_has_key, NULL)
  PHP_FE(ncurses_insch, NULL)
  PHP_FE(ncurses_insdelln, NULL)
  PHP_FE(ncurses_mouseinterval, NULL)
  PHP_FE(ncurses_napms, NULL)
  PHP_FE(ncurses_scrl, NULL)
  PHP_FE(ncurses_slk_attroff, NULL)
  PHP_FE(ncurses_slk_attron, NULL)
  PHP_FE(ncurses_slk_attrset, NULL)
  PHP_FE(ncurses_slk_color, NULL)
  PHP_FE(ncurses_slk_init, NULL)
	PHP_FE(ncurses_slk_set, NULL)
  PHP_FE(ncurses_typeahead, NULL)
  PHP_FE(ncurses_ungetch, NULL)
  PHP_FE(ncurses_vidattr, NULL)
  PHP_FE(ncurses_wrefresh, NULL)
  PHP_FE(ncurses_use_extended_names, NULL)
  PHP_FE(ncurses_bkgdset, NULL)
  PHP_FE(ncurses_filter, NULL)
  PHP_FE(ncurses_noqiflush, NULL)
  PHP_FE(ncurses_qiflush, NULL)
  PHP_FE(ncurses_timeout, NULL)
  PHP_FE(ncurses_use_env, NULL)
  PHP_FE(ncurses_addstr, NULL)
  PHP_FE(ncurses_putp, NULL)
  PHP_FE(ncurses_scr_dump, NULL)
  PHP_FE(ncurses_scr_init, NULL)
  PHP_FE(ncurses_scr_restore, NULL)
  PHP_FE(ncurses_scr_set, NULL)
  PHP_FE(ncurses_mvaddch, NULL)
  PHP_FE(ncurses_mvaddchnstr, NULL)
  PHP_FE(ncurses_addchnstr, NULL)
  PHP_FE(ncurses_mvaddchstr, NULL)
  PHP_FE(ncurses_addchstr, NULL)
  PHP_FE(ncurses_mvaddnstr, NULL)
  PHP_FE(ncurses_addnstr, NULL)
  PHP_FE(ncurses_mvaddstr, NULL)
  PHP_FE(ncurses_mvdelch, NULL)
  PHP_FE(ncurses_mvgetch, NULL)
  PHP_FE(ncurses_mvinch, NULL)
  PHP_FE(ncurses_mvwaddstr, NULL)
  PHP_FE(ncurses_insstr, NULL)
  PHP_FE(ncurses_instr, first_args_force_ref)
  PHP_FE(ncurses_mvhline, NULL)
  PHP_FE(ncurses_mvcur, NULL)
  PHP_FE(ncurses_init_color, NULL)
  PHP_FE(ncurses_border, NULL)
  PHP_FE(ncurses_assume_default_colors, NULL)
  PHP_FE(ncurses_define_key, NULL)
  PHP_FE(ncurses_hline, NULL)
  PHP_FE(ncurses_vline, NULL)
  PHP_FE(ncurses_keyok, NULL)
  PHP_FE(ncurses_termname, NULL)
  PHP_FE(ncurses_longname, NULL)
  PHP_FE(ncurses_mousemask, second_args_force_ref)
  PHP_FE(ncurses_getmouse, first_args_force_ref)
  PHP_FE(ncurses_ungetmouse, NULL)
  PHP_FE(ncurses_mouse_trafo, firstandsecond_args_force_ref)
	PHP_FE(ncurses_wmouse_trafo, secondandthird_args_force_ref)
  PHP_FE(ncurses_waddstr, NULL)
  PHP_FE(ncurses_wnoutrefresh, NULL)
  PHP_FE(ncurses_wclear, NULL)
  PHP_FE(ncurses_wcolor_set, NULL)
  PHP_FE(ncurses_wgetch, NULL)
  PHP_FE(ncurses_keypad, NULL)
  PHP_FE(ncurses_wmove, NULL)

	PHP_FE(ncurses_newpad,		NULL)
	PHP_FE(ncurses_prefresh,	NULL)
	PHP_FE(ncurses_pnoutrefresh,	NULL)
	PHP_FE(ncurses_wstandout,		NULL)
	PHP_FE(ncurses_wstandend,		NULL)
	PHP_FE(ncurses_wattrset,		NULL)
	PHP_FE(ncurses_wattron,		NULL)
	PHP_FE(ncurses_wattroff,		NULL)
	PHP_FE(ncurses_waddch,		NULL)
	PHP_FE(ncurses_wborder,		NULL)
	PHP_FE(ncurses_whline,		NULL)
	PHP_FE(ncurses_wvline,		NULL)
	PHP_FE(ncurses_getyx,		secondandthird_args_force_ref)
	
#if HAVE_NCURSES_PANEL
	PHP_FE(ncurses_update_panels,	NULL)
	PHP_FE(ncurses_panel_window,	NULL)
	PHP_FE(ncurses_panel_below,	NULL)
	PHP_FE(ncurses_panel_above,	NULL)
	PHP_FE(ncurses_replace_panel,	NULL)
	PHP_FE(ncurses_move_panel,	NULL)
	PHP_FE(ncurses_bottom_panel,	NULL)
	PHP_FE(ncurses_top_panel,	NULL)
	PHP_FE(ncurses_show_panel,	NULL)
	PHP_FE(ncurses_hide_panel,	NULL)
	PHP_FE(ncurses_del_panel,	NULL)
	PHP_FE(ncurses_new_panel,	NULL)
#endif
  
  {NULL, NULL, NULL}  /* Must be the last line in ncurses_functions[] */
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
