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
	zval **pair;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &pair) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(pair);

	RETURN_LONG(color_set(_INT(pair),NULL));
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
