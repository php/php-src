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

/* {{{ proto int ncurses_addch(int ch)
    */
PHP_FUNCTION(ncurses_addch)
{
	zval **ch;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &ch) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(ch);

	RETURN_LONG(addch(_INT(ch)));
}
/* }}} */

/* {{{ proto int ncurses_color_set(int pair)
    */
PHP_FUNCTION(ncurses_color_set)
{
#ifdef HAVE_NCURSES_COLOR_SET
	zval **pair;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &pair) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(pair);

	RETURN_LONG(color_set(_INT(pair),NULL));
#else
	php_error(E_WARNING,"%s not supported in this build");
	RETURN_FALSE;
#endif	
}
/* }}} */

/* {{{ proto int ncurses_newwin(int ch)
    */
PHP_FUNCTION(ncurses_delwin)
{
	zval **handle;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &handle) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(handle);

	/* TODO resource type check */

	zend_list_delete((*handle)->value.lval);
}
/* }}} */

/* {{{ proto int ncurses_end()
    */
PHP_FUNCTION(ncurses_end)
{
	RETURN_LONG(endwin());             /* endialize the curses library */
}
/* }}} */

/* {{{ proto int ncurses_getch()
    */
PHP_FUNCTION(ncurses_getch)
{
	RETURN_LONG(getch());
}
/* }}} */

/* {{{ proto int ncurses_has_colors()
    */
PHP_FUNCTION(ncurses_has_colors)
{
	RETURN_LONG(has_colors());
}
/* }}} */

/* {{{ proto int ncurses_init()
    */
PHP_FUNCTION(ncurses_init)
{
	initscr();             /* initialize the curses library */
	keypad(stdscr, TRUE);  /* enable keyboard mapping */
	(void) nonl();         /* tell curses not to do NL->CR/NL on output */
	(void) cbreak();       /* take input chars one at a time, no wait for \n */}
/* }}} */

/* {{{ proto int ncurses_init_pair(int pair, int fg, int bg)
    */
PHP_FUNCTION(ncurses_init_pair)
{
	zval **pair, **fg, **bg;
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &pair, &fg, &bg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(pair);
	convert_to_long_ex(fg);
	convert_to_long_ex(bg);

	RETURN_LONG(init_pair(_INT(pair),_INT(fg),_INT(bg)));
}
/* }}} */

/* {{{ proto int ncurses_move(int x, int y)
    */
PHP_FUNCTION(ncurses_move)
{
	zval **x, **y;
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(x);
	convert_to_long_ex(y);

	RETURN_LONG(move(_INT(x),_INT(y)));
}
/* }}} */

/* {{{ proto int ncurses_newwin(int ch)
    */
PHP_FUNCTION(ncurses_newwin)
{
	zval **rows,**cols,**y,**x;
	WINDOW **pwin = (WINDOW **)emalloc(sizeof(WINDOW *));

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &rows,&cols,&y,&x) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(rows);
	convert_to_long_ex(cols);
	convert_to_long_ex(y);
	convert_to_long_ex(x);

	*pwin=newwin(_INT(rows),_INT(cols),_INT(y),_INT(x));
	
	if(!*pwin) {
		efree(pwin);
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, pwin, le_ncurses);
}
/* }}} */

/* {{{ proto int ncurses_refresh(int ch)
    */
PHP_FUNCTION(ncurses_refresh)
{
	RETURN_LONG(refresh());
}
/* }}} */

/* {{{ proto int ncurses_start_color()
    */
PHP_FUNCTION(ncurses_start_color)
{
	RETURN_LONG(start_color());
}
/* }}} */

/* ----- */

/* {{{ proto int ncurses_standout()
    */
PHP_FUNCTION(ncurses_standout)
{
	RETURN_LONG(standout());
}
/* }}} */

/* {{{ proto int ncurses_standend()
    */
PHP_FUNCTION(ncurses_standend)
{
	RETURN_LONG(standend());
}
/* }}} */

/* {{{ proto int ncurses_baudrate()
    */
PHP_FUNCTION(ncurses_baudrate)
{
	RETURN_LONG(baudrate());
}
/* }}} */

/* {{{ proto int ncurses_beep()
    */
PHP_FUNCTION(ncurses_beep)
{
	RETURN_LONG(beep());
}
/* }}} */

/* {{{ proto bool ncurses_can_change_color()
    */
PHP_FUNCTION(ncurses_can_change_color)
{
	RETURN_LONG(can_change_color());
}
/* }}} */

/* {{{ proto bool ncurses_cbreak()
    */
PHP_FUNCTION(ncurses_cbreak)
{
	RETURN_LONG(cbreak());
}
/* }}} */

/* {{{ proto bool ncurses_clear()
    */
PHP_FUNCTION(ncurses_clear)
{
	RETURN_LONG(clear());
}
/* }}} */

/* {{{ proto bool ncurses_clrtobot()
    */
PHP_FUNCTION(ncurses_clrtobot)
{
	RETURN_LONG(clrtobot());
}
/* }}} */

/* {{{ proto bool ncurses_clrtoeol()
    */
PHP_FUNCTION(ncurses_clrtoeol)
{
	RETURN_LONG(clrtoeol());
}
/* }}} */

/* {{{ proto bool ncurses_def_prog_mode()
    */
PHP_FUNCTION(ncurses_def_prog_mode)
{
	RETURN_LONG(def_prog_mode());
}
/* }}} */

/* {{{ proto bool ncurses_def_shell_mode()
    */
PHP_FUNCTION(ncurses_def_shell_mode)
{
	RETURN_LONG(def_shell_mode());
}
/* }}} */

/* {{{ proto bool ncurses_delch()
    */
PHP_FUNCTION(ncurses_delch)
{
	RETURN_LONG(delch());
}
/* }}} */

/* {{{ proto bool ncurses_deleteln()
    */
PHP_FUNCTION(ncurses_deleteln)
{
	RETURN_LONG(deleteln());
}
/* }}} */

/* {{{ proto bool ncurses_doupdate()
    */
PHP_FUNCTION(ncurses_doupdate)
{
	RETURN_LONG(doupdate());
}
/* }}} */

/* {{{ proto bool ncurses_echo()
    */
PHP_FUNCTION(ncurses_echo)
{
	RETURN_LONG(echo());
}
/* }}} */

/* {{{ proto bool ncurses_erase()
    */
PHP_FUNCTION(ncurses_erase)
{
	RETURN_LONG(erase());
}
/* }}} */

/* {{{ proto bool ncurses_erasechar()
    */
PHP_FUNCTION(ncurses_erasechar)
{
	RETURN_LONG(erasechar());
}
/* }}} */

/* {{{ proto bool ncurses_flash()
    */
PHP_FUNCTION(ncurses_flash)
{
	RETURN_LONG(flash());
}
/* }}} */

/* {{{ proto bool ncurses_flushinp()
    */
PHP_FUNCTION(ncurses_flushinp)
{
	RETURN_LONG(flushinp());
}
/* }}} */

/* {{{ proto bool ncurses_has_ic()
    */
PHP_FUNCTION(ncurses_has_ic)
{
	RETURN_LONG(has_ic());
}
/* }}} */


/* {{{ proto bool ncurses_has_il()
    */
PHP_FUNCTION(ncurses_has_il)
{
	RETURN_LONG(has_il());
}
/* }}} */

/* {{{ proto bool ncurses_inch()
    */
PHP_FUNCTION(ncurses_inch)
{
	RETURN_LONG(inch());
}
/* }}} */

/* {{{ proto bool ncurses_insertln()
    */
PHP_FUNCTION(ncurses_insertln)
{
	RETURN_LONG(insertln());
}
/* }}} */

/* {{{ proto bool ncurses_isendwin()
    */
PHP_FUNCTION(ncurses_isendwin)
{
	RETURN_LONG(isendwin());
}
/* }}} */

/* {{{ proto bool ncurses_killchar()
    */
PHP_FUNCTION(ncurses_killchar)
{
	RETURN_LONG(killchar());
}
/* }}} */

/* {{{ proto bool ncurses_nl()
    */
PHP_FUNCTION(ncurses_nl)
{
	RETURN_LONG(nl());
}
/* }}} */

/* {{{ proto bool ncurses_nocbreak()
    */
PHP_FUNCTION(ncurses_nocbreak)
{
	RETURN_LONG(nocbreak());
}
/* }}} */

/* {{{ proto bool ncurses_noecho()
    */
PHP_FUNCTION(ncurses_noecho)
{
	RETURN_LONG(noecho());
}
/* }}} */

/* {{{ proto bool ncurses_nonl()
    */
PHP_FUNCTION(ncurses_nonl)
{
	RETURN_LONG(nonl());
}
/* }}} */

/* {{{ proto bool ncurses_noraw()
    */
PHP_FUNCTION(ncurses_noraw)
{
	RETURN_LONG(noraw());
}
/* }}} */

/* {{{ proto bool ncurses_raw()
    */
PHP_FUNCTION(ncurses_raw)
{
	RETURN_LONG(raw());
}
/* }}} */

/* {{{ proto bool ncurses_resetty()
    */
PHP_FUNCTION(ncurses_resetty)
{
	RETURN_LONG(resetty());
}
/* }}} */

/* {{{ proto bool ncurses_savetty()
    */
PHP_FUNCTION(ncurses_savetty)
{
	RETURN_LONG(savetty());
}
/* }}} */

/* {{{ proto bool ncurses_termattrs()
    */
PHP_FUNCTION(ncurses_termattrs)
{
	RETURN_LONG(termattrs());
}
/* }}} */

/* {{{ proto bool ncurses_use_default_colors()
    */
PHP_FUNCTION(ncurses_use_default_colors)
{
	RETURN_LONG(use_default_colors());
}
/* }}} */

/* {{{ proto bool ncurses_slk_attr()
    */
PHP_FUNCTION(ncurses_slk_attr)
{
	RETURN_LONG(slk_attr());
}
/* }}} */

/* {{{ proto bool ncurses_slk_clear()
    */
PHP_FUNCTION(ncurses_slk_clear)
{
	RETURN_LONG(slk_clear());
}
/* }}} */

/* {{{ proto bool ncurses_slk_noutrefresh()
    */
PHP_FUNCTION(ncurses_slk_noutrefresh)
{
	RETURN_LONG(slk_noutrefresh());
}
/* }}} */

/* {{{ proto bool ncurses_slk_refresh()
    */
PHP_FUNCTION(ncurses_slk_refresh)
{
	RETURN_LONG(slk_refresh());
}
/* }}} */

/* {{{ proto bool ncurses_slk_restore()
    */
PHP_FUNCTION(ncurses_slk_restore)
{
	RETURN_LONG(slk_restore());
}
/* }}} */

/* {{{ proto bool ncurses_slk_touch()
    */
PHP_FUNCTION(ncurses_slk_touch)
{
	RETURN_LONG(slk_touch());
}
/* }}} */

/* {{{ proto int ncurses_attroff(int intarg)
    */
PHP_FUNCTION(ncurses_attroff)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(attroff(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_attron(int intarg)
    */
PHP_FUNCTION(ncurses_attron)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(attron(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_attrset(int intarg)
    */
PHP_FUNCTION(ncurses_attrset)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(attrset(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_bkgd(int intarg)
    */
PHP_FUNCTION(ncurses_bkgd)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(bkgd(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_curs_set(int intarg)
    */
PHP_FUNCTION(ncurses_curs_set)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(curs_set(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_delay_output(int intarg)
    */
PHP_FUNCTION(ncurses_delay_output)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(delay_output(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_echochar(int intarg)
    */
PHP_FUNCTION(ncurses_echochar)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(echochar(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_halfdelay(int intarg)
    */
PHP_FUNCTION(ncurses_halfdelay)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(halfdelay(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_has_key(int intarg)
    */
PHP_FUNCTION(ncurses_has_key)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(has_key(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_insch(int intarg)
    */
PHP_FUNCTION(ncurses_insch)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(insch(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_insdelln(int intarg)
    */
PHP_FUNCTION(ncurses_insdelln)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(insdelln(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_mouseinterval(int intarg)
    */
PHP_FUNCTION(ncurses_mouseinterval)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(mouseinterval(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_napms(int intarg)
    */
PHP_FUNCTION(ncurses_napms)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(napms(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_scrl(int intarg)
    */
PHP_FUNCTION(ncurses_scrl)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(scrl(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_slk_attroff(int intarg)
    */
PHP_FUNCTION(ncurses_slk_attroff)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(slk_attroff(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_slk_attron(int intarg)
    */
PHP_FUNCTION(ncurses_slk_attron)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(slk_attron(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_slk_attrset(int intarg)
    */
PHP_FUNCTION(ncurses_slk_attrset)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(slk_attrset(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_slk_color(int intarg)
    */
PHP_FUNCTION(ncurses_slk_color)
{
#ifdef HAVE_NCURSES_SLK_COLOR
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(slk_color(_INT(intarg)));
#else
	php_error(E_WARNING,"%s not supported in this build");
	RETURN_FALSE;
#endif	
}
/* }}} */

/* {{{ proto int ncurses_slk_init(int intarg)
    */
PHP_FUNCTION(ncurses_slk_init)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(slk_init(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_typeahead(int intarg)
    */
PHP_FUNCTION(ncurses_typeahead)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(typeahead(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_ungetch(int intarg)
    */
PHP_FUNCTION(ncurses_ungetch)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(ungetch(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_vidattr(int intarg)
    */
PHP_FUNCTION(ncurses_vidattr)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(vidattr(_INT(intarg)));
}
/* }}} */

/* {{{ proto int ncurses_use_extended_names(int intarg)
    */
PHP_FUNCTION(ncurses_use_extended_names)
{
#ifdef HAVE_NCURSES_USE_EXTENDED_NAMES
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	RETURN_LONG(use_extended_names(_INT(intarg)));
#else
	php_error(E_WARNING,"%s not supported in this build");
	RETURN_FALSE;
#endif	
}
/* }}} */

/* {{{ proto int ncurses_bkgdset(int intarg)
    */
PHP_FUNCTION(ncurses_bkgdset)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	bkgdset(_INT(intarg));
}
/* }}} */

/* {{{ proto int ncurses_filter()
    */
PHP_FUNCTION(ncurses_filter)
{
	filter();
}
/* }}} */

/* {{{ proto int ncurses_noqiflush()
    */
PHP_FUNCTION(ncurses_noqiflush)
{
	noqiflush();
}
/* }}} */

/* {{{ proto int ncurses_qiflush()
    */
PHP_FUNCTION(ncurses_qiflush)
{
	qiflush();
}
/* }}} */

/* {{{ proto int ncurses_timeout(int intarg)
    */
PHP_FUNCTION(ncurses_timeout)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	timeout(_INT(intarg));
}
/* }}} */

/* {{{ proto int ncurses_use_env(int intarg)
    */
PHP_FUNCTION(ncurses_use_env)
{
	zval **intarg;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &intarg) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(intarg);

	use_env(_INT(intarg));
}
/* }}} */
