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

#define FETCH_WINRES(r, z)   ZEND_FETCH_RESOURCE(r, WINDOW *, z, -1, "ncurses_handle", le_ncurses); \
                                    if(!r) RETURN_FALSE;


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

/* {{{ proto int ncurses_delwin(resource window)
    */
PHP_FUNCTION(ncurses_delwin)
{
	zval **handle;
	WINDOW *w;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &handle) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	FETCH_WINRES(w,handle);

	zend_list_delete(_INT(handle));
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

/* {{{ proto int ncurses_newwin(int rows, int cols, int y, int x)
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

/* {{{ proto int ncurses_addstr(string text)
   */
PHP_FUNCTION(ncurses_addstr)
{
	zval **data;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &data) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(data);

	RETURN_LONG(addstr(_STRING(data)));
}
/* }}} */




/**********************  area of work *************************************/




/* {{{ proto int ncurses_putp(string text)
   */
PHP_FUNCTION(ncurses_putp)
{
	zval **data;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &data) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(data);

	RETURN_LONG(putp(_STRING(data)));
}
/* }}} */

/* {{{ proto int ncurses_scr_dump(string text)
   */
PHP_FUNCTION(ncurses_scr_dump)
{
	zval **data;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &data) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(data);

	RETURN_LONG(scr_dump(_STRING(data)));
}
/* }}} */

/* {{{ proto int ncurses_scr_init(string text)
   */
PHP_FUNCTION(ncurses_scr_init)
{
	zval **data;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &data) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(data);

	RETURN_LONG(scr_init(_STRING(data)));
}
/* }}} */

/* {{{ proto int ncurses_scr_restore(string text)
   */
PHP_FUNCTION(ncurses_scr_restore)
{
	zval **data;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &data) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(data);

	RETURN_LONG(scr_restore(_STRING(data)));
}
/* }}} */

/* {{{ proto int ncurses_scr_set(string text)
   */
PHP_FUNCTION(ncurses_scr_set)
{
	zval **data;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &data) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(data);

	RETURN_LONG(scr_set(_STRING(data)));
}
/* }}} */

/* {{{ proto int ncurses_mvaddch(int x, int y, int c)
   */
PHP_FUNCTION(ncurses_mvaddch)
{
	zval **x,**y,**c;

	if(ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &x, &y, &c) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(x);
	convert_to_long_ex(y);
	convert_to_long_ex(c);

	RETURN_LONG(mvaddch(_INT(x),_INT(y),_INT(c)));
}
/* }}} */

/* {{{ proto int ncurses_mvaddchnstr(int x, int y, string s, int n)
   */
PHP_FUNCTION(ncurses_mvaddchnstr)
{
	zval **x,**y,**s,**n;

	if(ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &x, &y, &s, &n) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(x);
	convert_to_long_ex(y);
	convert_to_string_ex(s);
	convert_to_long_ex(n);

	RETURN_LONG(mvaddchnstr(_INT(x),_INT(y),(chtype *)_STRING(s),_INT(n)));
}
/* }}} */

/* {{{ proto int ncurses_addchnstr(string s, int n)
   */
PHP_FUNCTION(ncurses_addchnstr)
{
	zval **s,**n;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &s, &n) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(s);
	convert_to_long_ex(n);

	RETURN_LONG(addchnstr((chtype *)_STRING(s),_INT(n)));
}
/* }}} */

/* {{{ proto int ncurses_mvaddchstr(int x, int y, string s)
   */
PHP_FUNCTION(ncurses_mvaddchstr)
{
	zval **x,**y,**s;

	if(ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &x, &y, &s) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(x);
	convert_to_long_ex(y);
	convert_to_string_ex(s);

	RETURN_LONG(mvaddchstr(_INT(x),_INT(y),(chtype *)_STRING(s)));
}
/* }}} */

/* {{{ proto int ncurses_addchstr(string s)
   */
PHP_FUNCTION(ncurses_addchstr)
{
	zval **s;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &s) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(s);

	RETURN_LONG(addchstr((chtype *)_STRING(s)));
}
/* }}} */

/* {{{ proto int ncurses_mvaddnstr(int x, int y, string s, int n)
   */
PHP_FUNCTION(ncurses_mvaddnstr)
{
	zval **x,**y,**s,**n;

	if(ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &x, &y, &s, &n) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(x);
	convert_to_long_ex(y);
	convert_to_string_ex(s);
	convert_to_long_ex(n);

	RETURN_LONG(mvaddnstr(_INT(x),_INT(y),_STRING(s),_INT(n)));
}
/* }}} */

/* {{{ proto int ncurses_addnstr(int x, int y, string s, int n)
   */
PHP_FUNCTION(ncurses_addnstr)
{
	zval **s,**n;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &s, &n) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(s);
	convert_to_long_ex(n);

	RETURN_LONG(addnstr(_STRING(s),_INT(n)));
}
/* }}} */

/* {{{ proto int ncurses_mvaddstr(int x, int y, string s)
   */
PHP_FUNCTION(ncurses_mvaddstr)
{
	zval **x,**y,**s;

	if(ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &x, &y, &s) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(x);
	convert_to_long_ex(y);
	convert_to_string_ex(s);

	RETURN_LONG(mvaddstr(_INT(x),_INT(y),_STRING(s)));
}
/* }}} */

/* {{{ proto int ncurses_mvdelch(int x, int y)
   */
PHP_FUNCTION(ncurses_mvdelch)
{
	zval **x,**y,**c;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(x);
	convert_to_long_ex(y);

	RETURN_LONG(mvdelch(_INT(x),_INT(y)));
}
/* }}} */


/* {{{ proto int ncurses_mvgetch(int x, int y)
   */
PHP_FUNCTION(ncurses_mvgetch)
{
	zval **x,**y;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(x);
	convert_to_long_ex(y);

	RETURN_LONG(mvgetch(_INT(x),_INT(y)));
}
/* }}} */

/* {{{ proto int ncurses_mvinch(int x, int y)
   */
PHP_FUNCTION(ncurses_mvinch)
{
	zval **x,**y;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(x);
	convert_to_long_ex(y);

	RETURN_LONG(mvinch(_INT(x),_INT(y)));
}
/* }}} */

/* {{{ proto int ncurses_insstr(string)
   */
PHP_FUNCTION(ncurses_insstr)
{
	zval **str;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(str);

	RETURN_LONG(insstr(_STRING(str)));
}
/* }}} */

/* {{{ proto int ncurses_instr(string)
   */
PHP_FUNCTION(ncurses_instr)
{
	zval **str;

	if(ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(str);

	RETURN_LONG(instr(_STRING(str)));
}
/* }}} */

/* {{{ proto int ncurses_mvhline(int,int,int,int)
   */
PHP_FUNCTION(ncurses_mvhline)
{
	zval **i1,**i2,**i3,**i4;

	if(ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &i1, &i2, &i3, &i4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(i1);
	convert_to_long_ex(i2);
	convert_to_long_ex(i3);
	convert_to_long_ex(i4);

	RETURN_LONG(mvhline(_INT(i1),_INT(i2),_INT(i3),_INT(i4)));
}
/* }}} */

/* {{{ proto int ncurses_mvcur(int,int,int,int)
   */
PHP_FUNCTION(ncurses_mvcur)
{
	zval **i1,**i2,**i3,**i4;

	if(ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &i1, &i2, &i3, &i4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(i1);
	convert_to_long_ex(i2);
	convert_to_long_ex(i3);
	convert_to_long_ex(i4);

	RETURN_LONG(mvcur(_INT(i1),_INT(i2),_INT(i3),_INT(i4)));
}
/* }}} */

/* {{{ proto int ncurses_init_color(int,int,int,int)
   */
PHP_FUNCTION(ncurses_init_color)
{
	zval **i1,**i2,**i3,**i4;

	if(ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &i1, &i2, &i3, &i4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(i1);
	convert_to_long_ex(i2);
	convert_to_long_ex(i3);
	convert_to_long_ex(i4);

	RETURN_LONG(init_color(_INT(i1),_INT(i2),_INT(i3),_INT(i4)));
}
/* }}} */

/* {{{ proto int ncurses_border(int,int,int,int,int,int,int,int)
   */
PHP_FUNCTION(ncurses_border)
{
	zval **i1,**i2,**i3,**i4,**i5,**i6,**i7,**i8;

	if(ZEND_NUM_ARGS() != 8 || zend_get_parameters_ex(8, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(i1);
	convert_to_long_ex(i2);
	convert_to_long_ex(i3);
	convert_to_long_ex(i4);
	convert_to_long_ex(i5);
	convert_to_long_ex(i6);
	convert_to_long_ex(i7);
	convert_to_long_ex(i8);

	RETURN_LONG(border(_INT(i1),_INT(i2),_INT(i3),_INT(i4),_INT(i5),_INT(i6),_INT(i7),_INT(i8)));
}
/* }}} */

/* {{{ proto int ncurses_assume_default_colors(int,int)
   */
PHP_FUNCTION(ncurses_assume_default_colors)
{
	zval **i1,**i2;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &i1, &i2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(i1);
	convert_to_long_ex(i2);

	RETURN_LONG(assume_default_colors(_INT(i1),_INT(i2)));
}
/* }}} */

/* {{{ proto int ncurses_define_key(string,int)
   */
PHP_FUNCTION(ncurses_define_key)
{
	zval **s1,**i2;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &s1, &i2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(s1);
	convert_to_long_ex(i2);

	RETURN_LONG(define_key(_STRING(s1),_INT(i2)));
}
/* }}} */

/* {{{ proto int ncurses_hline(int,int)
   */
PHP_FUNCTION(ncurses_hline)
{
	zval **i1,**i2;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &i1, &i2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(i1);
	convert_to_long_ex(i2);

	RETURN_LONG(hline(_INT(i1),_INT(i2)));
}
/* }}} */

/* {{{ proto int ncurses_vline(int,int)
   */
PHP_FUNCTION(ncurses_vline)
{
	zval **i1,**i2;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &i1, &i2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(i1);
	convert_to_long_ex(i2);

	RETURN_LONG(vline(_INT(i1),_INT(i2)));
}
/* }}} */

/* {{{ proto int ncurses_keyok(int,bool)
   */
PHP_FUNCTION(ncurses_keyok)
{
	zval **i1,**b2;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &i1, &b2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(i1);
	convert_to_long_ex(b2);

	RETURN_LONG(hline(_INT(i1),_INT(b2)));
}
/* }}} */

/* {{{ proto int ncurses_mvwaddstr(resource window, int y, int x, string text)
    */
PHP_FUNCTION(ncurses_mvwaddstr)
{
	zval **handle, **x, **y, **text;
	WINDOW **w;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &handle, &y, &x, &text) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	FETCH_WINRES(w,handle);

	convert_to_long_ex(y);
	convert_to_long_ex(x);
	convert_to_string_ex(text);

	RETURN_LONG(mvwaddstr(*w,_INT(y),_INT(x),_STRING(text)));
}
/* }}} */

/* {{{ proto int ncurses_wrefresh(resource window)
    */
PHP_FUNCTION(ncurses_wrefresh)
{
	zval **handle;
	WINDOW **w;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &handle) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	FETCH_WINRES(w,handle);

	RETURN_LONG(wrefresh(*w));
}
/* }}} */

