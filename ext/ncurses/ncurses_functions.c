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
   | Authors: Hartmut Holzgraefe <hholzgra@php.net>                       |
   |          Georg Richter <georg.richter@php-ev.de>                     |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_ncurses.h"

#define FETCH_WINRES(r, z)  ZEND_FETCH_RESOURCE(r, WINDOW **, z, -1, "ncurses_window", le_ncurses_windows)
#if HAVE_NCURSES_PANEL
# define FETCH_PANEL(r, z)  ZEND_FETCH_RESOURCE(r, PANEL **, z, -1, "ncurses_panel", le_ncurses_panels)
#endif

#define IS_NCURSES_INITIALIZED() \
		if (!NCURSES_G(registered_constants)) { \
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "You must initialize ncruses via ncurses_init(), before calling any ncurses functions."); \
			RETURN_FALSE; \
		}

/* {{{ proto int ncurses_addch(int ch)
   Adds character at current position and advance cursor */
PHP_FUNCTION(ncurses_addch)
{
	long ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &ch) == FAILURE) {
	        return;
	}

	IS_NCURSES_INITIALIZED();
	RETURN_LONG(addch(ch));
}
/* }}} */

/* {{{ proto int ncurses_waddch(resource window, int ch)
   Adds character at current position in a window and advance cursor */
PHP_FUNCTION(ncurses_waddch)
{
	long ch;
	zval *handle;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &handle, &ch) == FAILURE) {
	        return;
	}

	FETCH_WINRES(win, &handle);

	RETURN_LONG(waddch(*win, ch));
}
/* }}} */

#ifdef HAVE_NCURSES_COLOR_SET
/* {{{ proto int ncurses_color_set(int pair)
   Sets fore- and background color */
PHP_FUNCTION(ncurses_color_set)
{
	long pair;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &pair) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(color_set(pair,NULL));
}
/* }}} */
#endif

/* {{{ proto int ncurses_delwin(resource window)
   Deletes a ncurses window */
PHP_FUNCTION(ncurses_delwin)
{
	zval *handle;
	WINDOW **w;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &handle) == FAILURE) {
		return;
	}

	FETCH_WINRES(w, &handle);

	zend_list_delete(Z_LVAL_P(handle));
}
/* }}} */

/* {{{ proto int ncurses_end(void)
   Stops using ncurses, clean up the screen */
PHP_FUNCTION(ncurses_end)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(endwin());             /* endialize the curses library */
}
/* }}} */

/* {{{ proto int ncurses_getch(void)
   Reads a character from keyboard */
PHP_FUNCTION(ncurses_getch)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(getch());
}
/* }}} */

/* {{{ proto bool ncurses_has_colors(void)
   Checks if terminal has colors */
PHP_FUNCTION(ncurses_has_colors)
{
	IS_NCURSES_INITIALIZED();
	RETURN_BOOL(has_colors());
}
/* }}} */

/* {{{ proto int ncurses_init(void)
   Initializes ncurses */
PHP_FUNCTION(ncurses_init)
{
	initscr();             /* initialize the curses library */
	keypad(stdscr, TRUE);  /* enable keyboard mapping */
	(void) nonl();         /* tell curses not to do NL->CR/NL on output */
	(void) cbreak();       /* take input chars one at a time, no wait for \n */

	if (!NCURSES_G(registered_constants)) {
		zend_constant c;
		
		WINDOW **pscr = (WINDOW**)emalloc(sizeof(WINDOW *));
		zval *zscr;

		*pscr = stdscr;
		MAKE_STD_ZVAL(zscr);
		ZEND_REGISTER_RESOURCE(zscr, pscr, le_ncurses_windows);
		c.value = *zscr;
		zval_copy_ctor(&c.value);
		c.flags = CONST_CS;
		c.name = zend_strndup("STDSCR", 7);
		c.name_len = 7;
		zend_register_constant(&c TSRMLS_CC);

		/* we need this "interesting" arrangement because the
		 * underlying values of the ACS_XXX defines are not
		 * initialized until after ncurses has been initialized */
		
#define PHP_NCURSES_DEF_CONST(x)    \
		ZVAL_LONG(zscr, x);         \
		c.value = *zscr;            \
		zval_copy_ctor(&c.value);   \
		c.flags = CONST_CS;         \
		c.name = zend_strndup("NCURSES_" #x, sizeof("NCURSES_" #x)); \
		c.name_len = sizeof("NCURSES_" #x);                           \
		zend_register_constant(&c TSRMLS_CC)
		
		PHP_NCURSES_DEF_CONST(ACS_ULCORNER);
		PHP_NCURSES_DEF_CONST(ACS_LLCORNER);
		PHP_NCURSES_DEF_CONST(ACS_URCORNER);
		PHP_NCURSES_DEF_CONST(ACS_LRCORNER);
		PHP_NCURSES_DEF_CONST(ACS_LTEE);
		PHP_NCURSES_DEF_CONST(ACS_RTEE);
		PHP_NCURSES_DEF_CONST(ACS_BTEE);
		PHP_NCURSES_DEF_CONST(ACS_TTEE);
		PHP_NCURSES_DEF_CONST(ACS_HLINE);
		PHP_NCURSES_DEF_CONST(ACS_VLINE);
		PHP_NCURSES_DEF_CONST(ACS_PLUS);
		PHP_NCURSES_DEF_CONST(ACS_S1);
		PHP_NCURSES_DEF_CONST(ACS_S9);
		PHP_NCURSES_DEF_CONST(ACS_DIAMOND);
		PHP_NCURSES_DEF_CONST(ACS_CKBOARD);
		PHP_NCURSES_DEF_CONST(ACS_DEGREE);
		PHP_NCURSES_DEF_CONST(ACS_PLMINUS);
		PHP_NCURSES_DEF_CONST(ACS_BULLET);
		PHP_NCURSES_DEF_CONST(ACS_LARROW);
		PHP_NCURSES_DEF_CONST(ACS_RARROW);
		PHP_NCURSES_DEF_CONST(ACS_DARROW);
		PHP_NCURSES_DEF_CONST(ACS_UARROW);
		PHP_NCURSES_DEF_CONST(ACS_BOARD);
		PHP_NCURSES_DEF_CONST(ACS_LANTERN);
		PHP_NCURSES_DEF_CONST(ACS_BLOCK);
		
		FREE_ZVAL(zscr);
		NCURSES_G(registered_constants) = 1;
	}
}
/* }}} */

/* {{{ proto int ncurses_init_pair(int pair, int fg, int bg)
   Allocates a color pair */
PHP_FUNCTION(ncurses_init_pair)
{
	long pair, fg, bg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &pair, &fg, &bg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(init_pair(pair,fg,bg));
}
/* }}} */

/* {{{ proto int ncurses_move(int y, int x)
   Moves output position */
PHP_FUNCTION(ncurses_move)
{
	long x, y;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &y, &x) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(move(y,x));
}
/* }}} */

/* {{{ proto resource ncurses_newpad(int rows, int cols)
   Creates a new pad (window) */
PHP_FUNCTION(ncurses_newpad)
{
	long rows,cols;
	WINDOW **pwin;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &rows, &cols) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	
	pwin = (WINDOW **)emalloc(sizeof(WINDOW *));
	*pwin = newpad(rows,cols);

	if(!*pwin) {
		efree(pwin);
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, pwin, le_ncurses_windows);

}
/* }}} */

/* {{{ proto int ncurses_prefresh(resource pad, int pminrow, int pmincol, int sminrow, int smincol, int smaxrow, int smaxcol)
   Copys a region from a pad into the virtual screen */
PHP_FUNCTION(ncurses_prefresh)
{
	WINDOW **pwin;
	zval *phandle;
	long pminrow, pmincol, sminrow, smincol, smaxrow, smaxcol;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rllllll", &phandle, &pminrow,
				&pmincol, &sminrow, &smincol, &smaxrow, &smaxcol) == FAILURE) {
		return;
	}

	FETCH_WINRES(pwin, &phandle);

	RETURN_LONG(prefresh(*pwin, pminrow, pmincol, sminrow, smincol, smaxrow, smaxcol));
}
/* }}} */

/* {{{ proto int ncurses_pnoutrefresh(resource pad, int pminrow, int pmincol, int sminrow, int smincol, int smaxrow, int smaxcol)
   Copys a region from a pad into the virtual screen */
PHP_FUNCTION(ncurses_pnoutrefresh)
{
	WINDOW **pwin;
	zval *phandle;
	long pminrow, pmincol, sminrow, smincol, smaxrow, smaxcol;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rllllll", &phandle, &pminrow,
				&pmincol, &sminrow, &smincol, &smaxrow, &smaxcol) == FAILURE) {
		return;
	}

	FETCH_WINRES(pwin, &phandle);

	RETURN_LONG(pnoutrefresh(*pwin, pminrow, pmincol, sminrow, smincol, smaxrow, smaxcol));
}
/* }}} */



/* {{{ proto int ncurses_newwin(int rows, int cols, int y, int x)
   Creates a new window */
PHP_FUNCTION(ncurses_newwin)
{
	long rows,cols,y,x;
	WINDOW **pwin; 

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llll", &rows, &cols, &y, &x) == FAILURE) {
		return;
	}

	IS_NCURSES_INITIALIZED();
	pwin = (WINDOW **)emalloc(sizeof(WINDOW *));
	*pwin=newwin(rows,cols,y,x);

	if(!*pwin) {
		efree(pwin);
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, pwin, le_ncurses_windows);
}
/* }}} */

/* {{{ proto int ncurses_refresh(int ch)
   Refresh screen */
PHP_FUNCTION(ncurses_refresh)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(refresh());
}
/* }}} */

/* {{{ proto int ncurses_start_color(void)
   Starts using colors */
PHP_FUNCTION(ncurses_start_color)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(start_color());
}
/* }}} */

/* {{{ proto int ncurses_standout(void)
   Starts using 'standout' attribute */
PHP_FUNCTION(ncurses_standout)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(standout());
}
/* }}} */

/* {{{ proto int ncurses_standend(void)
   Stops using 'standout' attribute */
PHP_FUNCTION(ncurses_standend)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(standend());
}
/* }}} */

/* {{{ proto int ncurses_baudrate(void)
   Returns baudrate of terminal */
PHP_FUNCTION(ncurses_baudrate)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(baudrate());
}
/* }}} */

/* {{{ proto int ncurses_beep(void)
   Let the terminal beep */
PHP_FUNCTION(ncurses_beep)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(beep());
}
/* }}} */

/* {{{ proto bool ncurses_can_change_color(void)
   Checks if we can change terminals colors */
PHP_FUNCTION(ncurses_can_change_color)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(can_change_color());
}
/* }}} */

/* {{{ proto bool ncurses_cbreak(void)
   Switches of input buffering */
PHP_FUNCTION(ncurses_cbreak)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(cbreak());
}
/* }}} */

/* {{{ proto bool ncurses_clear(void)
   Clears screen */
PHP_FUNCTION(ncurses_clear)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(clear());
}
/* }}} */

/* {{{ proto bool ncurses_clrtobot(void)
   Clears screen from current position to bottom */
PHP_FUNCTION(ncurses_clrtobot)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(clrtobot());
}
/* }}} */

/* {{{ proto bool ncurses_clrtoeol(void)
   Clears screen from current position to end of line */
PHP_FUNCTION(ncurses_clrtoeol)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(clrtoeol());
}
/* }}} */

/* {{{ proto int ncurses_reset_prog_mode(void)
   Resets the prog mode saved by def_prog_mode */
PHP_FUNCTION(ncurses_reset_prog_mode)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(reset_prog_mode());
}
/* }}} */

/* {{{ proto int ncurses_reset_shell_mode(void)
   Resets the shell mode saved by def_shell_mode */
PHP_FUNCTION(ncurses_reset_shell_mode)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(reset_shell_mode());
}
/* }}} */

/* {{{ proto bool ncurses_def_prog_mode(void)
   Saves terminals (program) mode */
PHP_FUNCTION(ncurses_def_prog_mode)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(def_prog_mode());
}
/* }}} */

/* {{{ proto bool ncurses_def_shell_mode(void)
   Saves terminal (shell) mode*/
PHP_FUNCTION(ncurses_def_shell_mode)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(def_shell_mode());
}
/* }}} */

/* {{{ proto bool ncurses_delch(void)
   Deletes character at current position, move rest of line left */
PHP_FUNCTION(ncurses_delch)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(delch());
}
/* }}} */

/* {{{ proto bool ncurses_deleteln(void)
   Deletes line at current position, move rest of screen up */
PHP_FUNCTION(ncurses_deleteln)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(deleteln());
}
/* }}} */

/* {{{ proto bool ncurses_doupdate(void)
   Writes all prepared refreshes to terminal */
PHP_FUNCTION(ncurses_doupdate)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(doupdate());
}
/* }}} */

/* {{{ proto bool ncurses_echo(void)
   Activates keyboard input echo */
PHP_FUNCTION(ncurses_echo)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(echo());
}
/* }}} */

/* {{{ proto bool ncurses_erase(void)
   Erases terminal screen */
PHP_FUNCTION(ncurses_erase)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(erase());
}
/* }}} */

/* {{{ proto string ncurses_erasechar(void)
   Returns current erase character */
PHP_FUNCTION(ncurses_erasechar)
{
	char temp[2];

	IS_NCURSES_INITIALIZED();
	temp[0] = erasechar();
	temp[1] = '\0';

	RETURN_STRINGL (temp, 1, 1);
}
/* }}} */

/* {{{ proto bool ncurses_flash(void)
   Flashes terminal screen (visual bell) */
PHP_FUNCTION(ncurses_flash)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(flash());
}
/* }}} */

/* {{{ proto bool ncurses_flushinp(void)
   Flushes keyboard input buffer */
PHP_FUNCTION(ncurses_flushinp)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(flushinp());
}
/* }}} */

/* {{{ proto bool ncurses_has_ic(void)
   Checks for insert- and delete-capabilities */
PHP_FUNCTION(ncurses_has_ic)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(has_ic());
}
/* }}} */


/* {{{ proto bool ncurses_has_il(void)
   Checks for line insert- and delete-capabilities */
PHP_FUNCTION(ncurses_has_il)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(has_il());
}
/* }}} */

/* {{{ proto string ncurses_inch(void)
   Gets character and attribute at current position */
PHP_FUNCTION(ncurses_inch)
{
	char temp[2];

	IS_NCURSES_INITIALIZED();
	temp[0] = inch();
	temp[1] = '\0';

	RETURN_STRINGL (temp, 1, 1);
}
/* }}} */

/* {{{ proto bool ncurses_insertln(void)
   Inserts a line, move rest of screen down */
PHP_FUNCTION(ncurses_insertln)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(insertln());
}
/* }}} */

/* {{{ proto bool ncurses_isendwin(void)
   Ncurses is in endwin mode, normal screen output may be performed */
PHP_FUNCTION(ncurses_isendwin)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(isendwin());
}
/* }}} */

/* {{{ proto string ncurses_killchar(void)
   Returns current line kill character */
PHP_FUNCTION(ncurses_killchar)
{
	char temp[2];

	IS_NCURSES_INITIALIZED();
	temp[0] = killchar();
	temp[1] = '\0';

	RETURN_STRINGL (temp, 1, 1);
}
/* }}} */

/* {{{ proto bool ncurses_nl(void)
   Translates newline and carriage return / line feed */
PHP_FUNCTION(ncurses_nl)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(nl());
}
/* }}} */

/* {{{ proto bool ncurses_nocbreak(void)
   Switches terminal to cooked mode */
PHP_FUNCTION(ncurses_nocbreak)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(nocbreak());
}
/* }}} */

/* {{{ proto bool ncurses_noecho(void)
   Switches off keyboard input echo */
PHP_FUNCTION(ncurses_noecho)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(noecho());
}
/* }}} */

/* {{{ proto bool ncurses_nonl(void)
   Do not ranslate newline and carriage return / line feed */
PHP_FUNCTION(ncurses_nonl)
{
	RETURN_LONG(nonl());
}
/* }}} */

/* {{{ proto bool ncurses_noraw(void)
   Switches terminal out of raw mode */
PHP_FUNCTION(ncurses_noraw)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(noraw());
}
/* }}} */

/* {{{ proto bool ncurses_raw(void)
   Switches terminal into raw mode */
PHP_FUNCTION(ncurses_raw)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(raw());
}
/* }}} */

/* {{{ proto long ncurses_meta(resource window, bool 8bit)
   Enables/Disable 8-bit meta key information */
PHP_FUNCTION(ncurses_meta)
{
	zend_bool enable;
	zval *handle;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rb", &handle, &enable) == FAILURE) {
        	return;
	}

	FETCH_WINRES(win, &handle);

	RETURN_LONG(meta(*win, enable));
}
/* }}} */

/* {{{ proto long ncurses_werase(resource window)
   Erase window contents */
PHP_FUNCTION(ncurses_werase)
{
	zval *handle;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &handle) == FAILURE) {
	        return;
	}

	FETCH_WINRES(win, &handle);

	RETURN_LONG(werase(*win));
}
/* }}} */


/* {{{ proto bool ncurses_resetty(void)
   Restores saved terminal state */
PHP_FUNCTION(ncurses_resetty)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(resetty());
}
/* }}} */

/* {{{ proto bool ncurses_savetty(void)
   Saves terminal state */
PHP_FUNCTION(ncurses_savetty)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(savetty());
}
/* }}} */

/* {{{ proto bool ncurses_termattrs(void)
   Returns a logical OR of all attribute flags supported by terminal */
PHP_FUNCTION(ncurses_termattrs)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(termattrs());
}
/* }}} */

/* {{{ proto bool ncurses_use_default_colors(void)
   Assigns terminal default colors to color id -1 */
PHP_FUNCTION(ncurses_use_default_colors)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(use_default_colors());
}
/* }}} */

/* {{{ proto bool ncurses_slk_attr(void)
   Returns current soft label keys attribute */
PHP_FUNCTION(ncurses_slk_attr)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(slk_attr());
}
/* }}} */

/* {{{ proto bool ncurses_slk_clear(void)
   Clears soft label keys from screen */
PHP_FUNCTION(ncurses_slk_clear)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(slk_clear());
}
/* }}} */

/* {{{ proto bool ncurses_slk_noutrefresh(void)
   Copies soft label keys to virtual screen */
PHP_FUNCTION(ncurses_slk_noutrefresh)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(slk_noutrefresh());
}
/* }}} */

/* {{{ proto bool ncurses_slk_refresh(void)
   Copies soft label keys to screen */
PHP_FUNCTION(ncurses_slk_refresh)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(slk_refresh());
}
/* }}} */

/* {{{ proto bool ncurses_slk_restore(void)
   Restores soft label keys */
PHP_FUNCTION(ncurses_slk_restore)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(slk_restore());
}
/* }}} */

/* {{{ proto bool ncurses_slk_touch(void)
   Forces output when ncurses_slk_noutrefresh is performed */
PHP_FUNCTION(ncurses_slk_touch)
{
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(slk_touch());
}
/* }}} */

/* {{{ proto bool ncurses_slk_set(int labelnr, string label, int format)
   Sets function key labels */
PHP_FUNCTION(ncurses_slk_set)
{
	char *str;
	int  len;
	long labelnr;
	long format;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lsl", &labelnr, &str, &len, &format) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_BOOL(slk_set(labelnr, str, format));
}
/* }}} */


/* {{{ proto int ncurses_attroff(int attributes)
   Turns off the given attributes */
PHP_FUNCTION(ncurses_attroff)
{
	long intarg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(attroff(intarg));
}
/* }}} */

/* {{{ proto int ncurses_attron(int attributes)
   Turns on the given attributes */
PHP_FUNCTION(ncurses_attron)
{
	long intarg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(attron(intarg));
}
/* }}} */

/* {{{ proto int ncurses_attrset(int attributes)
   Sets given attributes */
PHP_FUNCTION(ncurses_attrset)
{
	long intarg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(attrset(intarg));
}
/* }}} */

/* {{{ proto int ncurses_bkgd(int attrchar)
   Sets background property for terminal screen */
PHP_FUNCTION(ncurses_bkgd)
{
	long intarg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(bkgd(intarg));
}
/* }}} */

/* {{{ proto int ncurses_curs_set(int visibility)
   Sets cursor state */
PHP_FUNCTION(ncurses_curs_set)
{
	long intarg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(curs_set(intarg));
}
/* }}} */

/* {{{ proto int ncurses_delay_output(int milliseconds)
   Delays output on terminal using padding characters */
PHP_FUNCTION(ncurses_delay_output)
{
	long intarg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(delay_output(intarg));
}
/* }}} */

/* {{{ proto int ncurses_echochar(int character)
   Single character output including refresh */
PHP_FUNCTION(ncurses_echochar)
{
	long intarg;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(echochar(intarg));
}
/* }}} */

/* {{{ proto int ncurses_halfdelay(int tenth)
   Puts terminal into halfdelay mode */
PHP_FUNCTION(ncurses_halfdelay)
{
	long intarg;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(halfdelay(intarg));
}
/* }}} */

/* {{{ proto int ncurses_has_key(int keycode)
   Checks for presence of a function key on terminal keyboard */
PHP_FUNCTION(ncurses_has_key)
{
	long intarg;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(has_key(intarg));
}
/* }}} */

/* {{{ proto int ncurses_insch(int character)
   Inserts character moving rest of line including character at current position */
PHP_FUNCTION(ncurses_insch)
{
	long intarg;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(insch(intarg));
}
/* }}} */

/* {{{ proto int ncurses_insdelln(int count)
   Inserts lines before current line scrolling down (negative numbers delete and scroll up) */
PHP_FUNCTION(ncurses_insdelln)
{
	long intarg;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(insdelln(intarg));
}
/* }}} */

/* {{{ proto int ncurses_mouseinterval(int milliseconds)
   Sets timeout for mouse button clicks */
PHP_FUNCTION(ncurses_mouseinterval)
{
	long intarg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(mouseinterval(intarg));
}
/* }}} */

/* {{{ proto int ncurses_napms(int milliseconds)
   Sleep */
PHP_FUNCTION(ncurses_napms)
{
	long intarg;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(napms(intarg));
}
/* }}} */

/* {{{ proto int ncurses_scrl(int count)
   Scrolls window content up or down without changing current position */
PHP_FUNCTION(ncurses_scrl)
{
	long intarg;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(scrl(intarg));
}
/* }}} */

/* {{{ proto int ncurses_slk_attroff(int intarg)
   ??? */
PHP_FUNCTION(ncurses_slk_attroff)
{
	long intarg;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(slk_attroff(intarg));
}
/* }}} */

/* {{{ proto int ncurses_slk_attron(int intarg)
   ??? */
PHP_FUNCTION(ncurses_slk_attron)
{
	long intarg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(slk_attron(intarg));
}
/* }}} */

/* {{{ proto int ncurses_slk_attrset(int intarg)
   ??? */
PHP_FUNCTION(ncurses_slk_attrset)
{
	long intarg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(slk_attrset(intarg));
}
/* }}} */

#ifdef HAVE_NCURSES_SLK_COLOR
/* {{{ proto int ncurses_slk_color(int intarg)
   Sets color for soft label keys*/
PHP_FUNCTION(ncurses_slk_color)
{
	long intarg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(slk_color(intarg));
}
/* }}} */
#endif

/* {{{ proto int ncurses_slk_init(int intarg)
   Inits soft label keys */
PHP_FUNCTION(ncurses_slk_init)
{
	long intarg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(slk_init(intarg));
}
/* }}} */

/* {{{ proto int ncurses_typeahead(int fd)
   Specifys different filedescriptor for typeahead checking */
PHP_FUNCTION(ncurses_typeahead)
{
	long intarg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(typeahead(intarg));
}
/* }}} */

/* {{{ proto int ncurses_ungetch(int keycode)
   Puts a character back into the input stream */
PHP_FUNCTION(ncurses_ungetch)
{
	long intarg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(ungetch(intarg));
}
/* }}} */

/* {{{ proto int ncurses_vidattr(int intarg)
   ??? */
PHP_FUNCTION(ncurses_vidattr)
{
	long intarg;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(vidattr(intarg));
}
/* }}} */

#ifdef HAVE_NCURSES_USE_EXTENDED_NAMES
/* {{{ proto int ncurses_use_extended_names(bool flag)
   Controls use of extended names in terminfo descriptions */
PHP_FUNCTION(ncurses_use_extended_names)
{
	long intarg;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(use_extended_names(intarg));
}
/* }}} */
#endif  

/* {{{ proto void ncurses_bkgdset(int attrchar)
   Controls screen background */
PHP_FUNCTION(ncurses_bkgdset)
{
	long intarg;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	bkgdset(intarg);
}
/* }}} */

/* {{{ proto int ncurses_filter(void)
 */
PHP_FUNCTION(ncurses_filter)
{
	IS_NCURSES_INITIALIZED();
	filter();
}
/* }}} */

/* {{{ proto int ncurses_noqiflush(void)
   Do not flush on signal characters*/
PHP_FUNCTION(ncurses_noqiflush)
{
	IS_NCURSES_INITIALIZED();
	noqiflush();
}
/* }}} */

/* {{{ proto int ncurses_qiflush(void)
   Flushes on signal characters */
PHP_FUNCTION(ncurses_qiflush)
{
	IS_NCURSES_INITIALIZED();
	qiflush();
}
/* }}} */

/* {{{ proto void ncurses_timeout(int millisec)
   Sets timeout for special key sequences */
PHP_FUNCTION(ncurses_timeout)
{
	long intarg;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	timeout(intarg);
}
/* }}} */

/* {{{ proto void ncurses_use_env(bool flag)
   Controls use of environment information about terminal size */
PHP_FUNCTION(ncurses_use_env)
{
	long intarg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intarg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	use_env(intarg);
}
/* }}} */

/* {{{ proto int ncurses_addstr(string text)
   Outputs text at current position */
PHP_FUNCTION(ncurses_addstr)
{
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(addstr(str));
}
/* }}} */

/* {{{ proto int ncurses_putp(string text)
   ??? */
PHP_FUNCTION(ncurses_putp)
{
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(putp(str));
}
/* }}} */

/* {{{ proto int ncurses_scr_dump(string filename)
   Dumps screen content to file */
PHP_FUNCTION(ncurses_scr_dump)
{
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(scr_dump(str));
}
/* }}} */

/* {{{ proto int ncurses_scr_init(string filename)
   Initializes screen from file dump */
PHP_FUNCTION(ncurses_scr_init)
{
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(scr_init(str));
}
/* }}} */

/* {{{ proto int ncurses_scr_restore(string filename)
   Restores screen from file dump */
PHP_FUNCTION(ncurses_scr_restore)
{
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(scr_restore(str));
}
/* }}} */

/* {{{ proto int ncurses_scr_set(string filename)
   Inherits screen from file dump */
PHP_FUNCTION(ncurses_scr_set)
{
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(scr_set(str));
}
/* }}} */

/* {{{ proto int ncurses_mvaddch(int y, int x, int c)
   Moves current position and add character */
PHP_FUNCTION(ncurses_mvaddch)
{
	long y,x,c;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &y, &x, &c) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(mvaddch(y,x,c));
}
/* }}} */

/* {{{ proto int ncurses_mvaddchnstr(int y, int x, string s, int n)
   Moves position and add attrributed string with specified length */
PHP_FUNCTION(ncurses_mvaddchnstr)
{
	long y,x,n;
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llsl", &y, &x, &str, &str_len, &n) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(mvaddchnstr(y,x,(chtype *)str,n));
}
/* }}} */

/* {{{ proto int ncurses_addchnstr(string s, int n)
   Adds attributed string with specified length at current position */
PHP_FUNCTION(ncurses_addchnstr)
{
	long n;
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &str, &str_len, &n) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(addchnstr((chtype *)str,n));
}
/* }}} */

/* {{{ proto int ncurses_mvaddchstr(int y, int x, string s)
   Moves position and add attributed string */
PHP_FUNCTION(ncurses_mvaddchstr)
{
	long y,x;
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lls", &y, &x, &str, &str_len) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(mvaddchstr(y,x,(chtype *)str));
}
/* }}} */

/* {{{ proto int ncurses_addchstr(string s)
   Adds attributed string at current position */
PHP_FUNCTION(ncurses_addchstr)
{
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(addchstr((chtype *)str));
}
/* }}} */

/* {{{ proto int ncurses_mvaddnstr(int y, int x, string s, int n)
   Moves position and add string with specified length */
PHP_FUNCTION(ncurses_mvaddnstr)
{
	long y,x,n;
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llsl", &y, &x, &str, &str_len, &n) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(mvaddnstr(y,x,str,n));
}
/* }}} */

/* {{{ proto int ncurses_addnstr(string s, int n)
   Adds string with specified length at current position */
PHP_FUNCTION(ncurses_addnstr)
{
	long n;
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &str, &str_len, &n) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(addnstr(str,n));
}
/* }}} */

/* {{{ proto int ncurses_mvaddstr(int y, int x, string s)
   Moves position and add string */
PHP_FUNCTION(ncurses_mvaddstr)
{
	long y,x;
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lls", &y, &x, &str, &str_len) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(mvaddstr(y,x,str));
}
/* }}} */

/* {{{ proto int ncurses_mvdelch(int y, int x)
   Moves position and delete character, shift rest of line left */
PHP_FUNCTION(ncurses_mvdelch)
{
	long y,x;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lls", &y, &x) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(mvdelch(y,x));
}
/* }}} */


/* {{{ proto int ncurses_mvgetch(int y, int x)
   Moves position and get character at new position */
PHP_FUNCTION(ncurses_mvgetch)
{
	long y,x;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lls", &y, &x) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(mvgetch(y,x));
}
/* }}} */

/* {{{ proto int ncurses_mvinch(int y, int x)
   Moves position and get attributed character at new position */
PHP_FUNCTION(ncurses_mvinch)
{
	long y,x;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lls", &y, &x) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(mvinch(y,x));
}
/* }}} */

/* {{{ proto int ncurses_insstr(string text)
   Inserts string at current position, moving rest of line right */
PHP_FUNCTION(ncurses_insstr)
{
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(insstr(str));
}
/* }}} */

/* {{{ proto int ncurses_instr(string &buffer)
   Reads string from terminal screen */
PHP_FUNCTION(ncurses_instr)
{
	ulong retval;
	zval *param;
	char *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &param) == FAILURE ) {
		return;
	}
	IS_NCURSES_INITIALIZED();

	str = (char *)emalloc(COLS + 1);
	retval = instr(str);

	ZVAL_STRING(param, str, strlen(str));
	efree(str);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int ncurses_mvhline(int y, int x, int attrchar, int n)
   Sets new position and draw a horizontal line using an attributed character and max. n characters long */
PHP_FUNCTION(ncurses_mvhline)
{
	long i1,i2,i3,i4;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llll", &i1, &i2, &i3, &i4) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(mvhline(i1,i2,i3,i4));
}
/* }}} */

/* {{{ proto int ncurses_mvvline(int y, int x, int attrchar, int n)
   Sets new position and draw a vertical line using an attributed character and max. n characters long */
PHP_FUNCTION(ncurses_mvvline)
{
	long i1,i2,i3,i4;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llll", &i1, &i2, &i3, &i4) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(mvvline(i1,i2,i3,i4));
}
/* }}} */

/* {{{ proto int ncurses_mvcur(int old_y,int old_x, int new_y, int new_x)
   Moves cursor immediately */
PHP_FUNCTION(ncurses_mvcur)
{
	long i1,i2,i3,i4;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llll", &i1, &i2, &i3, &i4) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(mvcur(i1,i2,i3,i4));
}
/* }}} */

/* {{{ proto int ncurses_init_color(int color, int r, int g, int b)
   Sets new RGB value for color */
PHP_FUNCTION(ncurses_init_color)
{
	long i1,i2,i3,i4;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llll", &i1, &i2, &i3, &i4) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(init_color(i1,i2,i3,i4));
}
/* }}} */

/* {{{ proto int ncurses_color_content(int color, int &r, int &g, int &b)
   Gets the RGB value for color */
PHP_FUNCTION(ncurses_color_content)
{
	zval *r, *g, *b;
	short rv, gv, bv;
	int retval;
	long c;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lzzz", &c, &r, &g, &b) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();
	
	retval = color_content(c, &rv, &gv, &bv);

	ZVAL_LONG(r, rv);
	ZVAL_LONG(g, gv);
	ZVAL_LONG(b, bv);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int ncurses_pair_content(int pair, int &f, int &b)
   Gets the RGB value for color */
PHP_FUNCTION(ncurses_pair_content)
{
	zval *f, *b;
	short fv, bv;
	int retval;
	long p;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lzz", &p, &f, &b) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();

	retval = pair_content(p, &fv, &bv);

	ZVAL_LONG(f, fv);
	ZVAL_LONG(b, bv);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto int ncurses_border(int left, int right, int top, int bottom, int tl_corner, int tr_corner, int bl_corner, int br_corner)
   Draws a border around the screen using attributed characters */
PHP_FUNCTION(ncurses_border)
{
	long i1,i2,i3,i4,i5,i6,i7,i8;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llllllll", &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(border(i1,i2,i3,i4,i5,i6,i7,i8));
}
/* }}} */

/* {{{ proto int ncurses_wborder(resource window, int left, int right, int top, int bottom, int tl_corner, int tr_corner, int bl_corner, int br_corner)
   Draws a border around the window using attributed characters */
PHP_FUNCTION(ncurses_wborder)
{
	long i1,i2,i3,i4,i5,i6,i7,i8;
	zval *handle;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rllllllll", &handle, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8) == FAILURE) {
	        return;
	}

	FETCH_WINRES(win, &handle);
	
	RETURN_LONG(wborder(*win,i1,i2,i3,i4,i5,i6,i7,i8));
}
/* }}} */

#ifdef HAVE_NCURSES_ASSUME_DEFAULT_COLORS
/* {{{ proto int ncurses_assume_default_colors(int fg, int bg)
   Defines default colors for color 0 */
PHP_FUNCTION(ncurses_assume_default_colors)
{
	long i1,i2;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &i1, &i2) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(assume_default_colors(i1,i2));
}
/* }}} */
#endif  

/* {{{ proto int ncurses_define_key(string definition, int keycode)
   Defines a keycode */
PHP_FUNCTION(ncurses_define_key)
{
	long n;
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &str, &str_len, &n) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(define_key(str,n));
}
/* }}} */

/* {{{ proto int ncurses_hline(int charattr, int n)
   Draws a horizontal line at current position using an attributed character and max. n characters long */
PHP_FUNCTION(ncurses_hline)
{
	long i1,i2;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &i1, &i2) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(hline(i1,i2));
}
/* }}} */

/* {{{ proto int ncurses_vline(int charattr, int n)
   Draws a vertical line at current position using an attributed character and max. n characters long */
PHP_FUNCTION(ncurses_vline)
{
	long i1,i2;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &i1, &i2) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();
	RETURN_LONG(vline(i1,i2));
}
/* }}} */

/* {{{ proto int ncurses_whline(resource window, int charattr, int n)
   Draws a horizontal line in a window at current position using an attributed character and max. n characters long */
PHP_FUNCTION(ncurses_whline)
{
	long i1,i2;
	zval *handle;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &handle, &i1, &i2) == FAILURE) {
	        return;
	}

	FETCH_WINRES(win, &handle);
	
	RETURN_LONG(whline(*win,i1,i2));
}
/* }}} */

/* {{{ proto int ncurses_wvline(resource window, int charattr, int n)
   Draws a vertical line in a window at current position using an attributed character and max. n characters long */
PHP_FUNCTION(ncurses_wvline)
{
	long i1,i2;
	zval *handle;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &handle, &i1, &i2) == FAILURE) {
	        return;
	}
	FETCH_WINRES(win, &handle);

	RETURN_LONG(wvline(*win,i1,i2));
}
/* }}} */

/* {{{ proto int ncurses_keyok(int keycode, bool enable)
   Enables or disable a keycode */
PHP_FUNCTION(ncurses_keyok)
{
	long i,b;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &i, &b) == FAILURE) {
	        return;
	}
	IS_NCURSES_INITIALIZED();	
	RETURN_LONG(hline(i,b));
}
/* }}} */

/* {{{ proto int ncurses_mvwaddstr(resource window, int y, int x, string text)
   Adds string at new position in window */
PHP_FUNCTION(ncurses_mvwaddstr)
{
	zval *handle;
	long y, x;
	int text_len;
	char *text;
	WINDOW **w;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlls", &handle, &y, &x, &text, &text_len) == FAILURE) {
		return;
	}
	
	FETCH_WINRES(w, &handle);

	RETURN_LONG(mvwaddstr(*w,y,x,text));
}
/* }}} */

/* {{{ proto int ncurses_wrefresh(resource window)
   Refreshes window on terminal screen */
PHP_FUNCTION(ncurses_wrefresh)
{
	zval *handle;
	WINDOW **w;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &handle) == FAILURE) {
		return;
	}

	FETCH_WINRES(w, &handle);

	RETURN_LONG(wrefresh(*w));
}
/* }}} */

/* {{{ proto string ncurses_termname(void)
   Returns terminal name */
PHP_FUNCTION(ncurses_termname)
{
	char temp[15];
	
	IS_NCURSES_INITIALIZED();

	strlcpy(temp, termname(), sizeof(temp));

	RETURN_STRINGL (temp, strlen(temp), 1);
}
/* }}} */

/* {{{ proto string ncurses_longname(void)
   Returns terminal description */
PHP_FUNCTION(ncurses_longname)
{
	char temp[128];

	IS_NCURSES_INITIALIZED();

	strlcpy(temp, longname(), sizeof(temp));

	RETURN_STRINGL (temp, strlen(temp), 1);
}
/* }}} */

/* {{{ proto int ncurses_mousemask(int newmask, int &oldmask)
   Returns and sets mouse options */
PHP_FUNCTION(ncurses_mousemask)
{
	ulong oldmask;
	ulong retval;
	zval *param;
	long newmask;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz", &newmask, &param) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();

	retval = mousemask(newmask, &oldmask);

	ZVAL_LONG(param, oldmask);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto bool ncurses_getmouse(array &mevent)
   Reads mouse event from queue */
PHP_FUNCTION(ncurses_getmouse)
{
	zval *arg;
	MEVENT mevent;
	ulong retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &arg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();

	pval_destructor(arg);
	array_init(arg);

	retval = getmouse(&mevent);

	add_assoc_long(arg, "id", mevent.id);
	add_assoc_long(arg, "x", mevent.x);
	add_assoc_long(arg, "y", mevent.y);
	add_assoc_long(arg, "z", mevent.z);
	add_assoc_long(arg, "mmask", mevent.bstate);

	RETURN_BOOL(retval == 0);
}
/* }}} */

/* {{{ proto int ncurses_ungetmouse(array mevent)
   Pushes mouse event to queue */
PHP_FUNCTION(ncurses_ungetmouse)
{
	zval *arg, **pvalue;
	MEVENT mevent;
	ulong retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &arg) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();

	if (zend_hash_find(Z_ARRVAL_P(arg), "id", sizeof("id"), (void **) &pvalue) == SUCCESS) {
		convert_to_long_ex(pvalue);
		mevent.id = Z_LVAL_PP(pvalue);
	}

	if (zend_hash_find(Z_ARRVAL_P(arg), "x", sizeof("x"), (void **) &pvalue) == SUCCESS) {
		convert_to_long_ex(pvalue);
		mevent.x = Z_LVAL_PP(pvalue);
	}

	if (zend_hash_find(Z_ARRVAL_P(arg), "y", sizeof("y"), (void **) &pvalue) == SUCCESS) {
		convert_to_long_ex(pvalue);
		mevent.y = Z_LVAL_PP(pvalue);
	}

	if (zend_hash_find(Z_ARRVAL_P(arg), "z", sizeof("z"), (void **) &pvalue) == SUCCESS) {
		convert_to_long_ex(pvalue);
		mevent.z = Z_LVAL_PP(pvalue);
	}

	if (zend_hash_find(Z_ARRVAL_P(arg), "mmask", sizeof("mmask"), (void **) &pvalue) == SUCCESS) {
		convert_to_long_ex(pvalue);
		mevent.bstate = Z_LVAL_PP(pvalue);
	}

	retval = ungetmouse(&mevent);

	RETURN_LONG(retval);
}
/* }}} */

/* {{{ proto bool ncurses_mouse_trafo(int &y, int &x, bool toscreen)
   Transforms coordinates */
PHP_FUNCTION(ncurses_mouse_trafo)
{
	zval *x, *y;
	zend_bool toscreen;
	int nx, ny, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzb", &y, &x, &toscreen) == FAILURE) {
		return;
	}
	IS_NCURSES_INITIALIZED();

	convert_to_long(y);
	convert_to_long(x);

	nx = Z_LVAL_P(x);
	ny = Z_LVAL_P(y);

	retval = mouse_trafo(&ny, &nx, toscreen);

	ZVAL_LONG(x, nx);
	ZVAL_LONG(y, ny);

	RETURN_BOOL(retval);
}
/* }}} */

/* {{{ proto bool ncurses_wmouse_trafo(resource window, int &y, int &x, bool toscreen)
   Transforms window/stdscr coordinates */
PHP_FUNCTION(ncurses_wmouse_trafo)
{
	zval *handle, *x, *y;
	int nx, ny, retval;
	WINDOW **win;
	zend_bool toscreen;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzzb", &handle, &y, &x, &toscreen) == FAILURE) {
		return;
	}

	FETCH_WINRES(win, &handle);

	convert_to_long(x);
	convert_to_long(y);

	nx = Z_LVAL_P(x);
	ny = Z_LVAL_P(y);

	retval = wmouse_trafo (*win, &ny, &nx, toscreen);

	ZVAL_LONG(x, nx);
	ZVAL_LONG(y, ny);

	RETURN_BOOL(retval);
}
/* }}} */

/* {{{ proto void ncurses_getyx(resource window, int &y, int &x)
   Returns the current cursor position for a window */
PHP_FUNCTION(ncurses_getyx)
{
	zval *handle, *x, *y;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzz", &handle, &y, &x) == FAILURE) {
		return;
	}

	FETCH_WINRES(win, &handle);

	convert_to_long(x);
	convert_to_long(y);

	getyx(*win, Z_LVAL_P(y), Z_LVAL_P(x));
}
/* }}} */

/* {{{ proto void ncurses_getmaxyx(resource window, int &y, int &x)
   Returns the size of a window */
PHP_FUNCTION(ncurses_getmaxyx)
{
	zval *handle, *x, *y;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzz", &handle, &y, &x) == FAILURE) {
		return;
	}

	FETCH_WINRES(win, &handle);

	convert_to_long(x);
	convert_to_long(y);

	getmaxyx(*win, Z_LVAL_P(y), Z_LVAL_P(x));
}
/* }}} */

/* {{{ proto int ncurses_wmove(resource window, int y, int x)
   Moves windows output position */
PHP_FUNCTION(ncurses_wmove)
{
	zval *handle, *x, *y;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzz", &handle, &y, &x) == FAILURE) {
		return;
	}

	FETCH_WINRES(win, &handle);

	convert_to_long(x);
	convert_to_long(y);

	RETURN_LONG(wmove(*win, Z_LVAL_P(y), Z_LVAL_P(x)));
}
/* }}} */

/* {{{ proto int ncurses_keypad(resource window, bool bf)
   Turns keypad on or off */
PHP_FUNCTION(ncurses_keypad)
{
	zval *handle;
	zend_bool bf;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rb", &handle, &bf) == FAILURE) {
		return;
	}

	FETCH_WINRES(win, &handle);

	RETURN_LONG(keypad(*win, bf));

}
/* }}} */

#ifdef HAVE_NCURSES_COLOR_SET
/* {{{ proto int ncurses_wcolor_set(resource window, int color_pair)
   Sets windows color pairings */
PHP_FUNCTION(ncurses_wcolor_set)
{
	zval *handle;
	long color_pair;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &handle, &color_pair) == FAILURE) {
		return;
	}

  	FETCH_WINRES(win, &handle);

	RETURN_LONG(wcolor_set(*win, color_pair, 0));
}
/* }}} */
#endif

/* {{{ proto int ncurses_wclear(resource window)
   Clears window */
PHP_FUNCTION(ncurses_wclear)
{
	zval *handle;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &handle) == FAILURE) {
		return;
	}	

	FETCH_WINRES(win, &handle);

	RETURN_LONG(wclear(*win));
}
/* }}} */

/* {{{ proto int ncurses_wnoutrefresh(resource window)
   Copies window to virtual screen */
PHP_FUNCTION(ncurses_wnoutrefresh)
{
	zval *handle;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &handle) == FAILURE) {
		return;
	}

	FETCH_WINRES(win, &handle);

	RETURN_LONG(wnoutrefresh(*win));
}
/* }}} */

/* {{{ proto int ncurses_waddstr(resource window, string str [, int n])
   Outputs text at current postion in window */
PHP_FUNCTION(ncurses_waddstr)
{
	zval *handle;
	char *str;
	int str_len;
	long n = 0;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l", &handle, &str, &str_len, &n) == FAILURE) {
		return;
	}

	FETCH_WINRES(win, &handle);
	if (!n) {
		RETURN_LONG(waddstr(*win, str));
	} else {
		RETURN_LONG(waddnstr(*win, str, n));
	}
}
/* }}} */

/* {{{ proto int ncurses_wgetch(resource window)
   Reads a character from keyboard (window) */
PHP_FUNCTION(ncurses_wgetch)
{
	zval *handle;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &handle) == FAILURE) {
		return;
	}

	FETCH_WINRES(win, &handle);

	RETURN_LONG(wgetch(*win));
}
/* }}} */

/* {{{ proto int wattroff(resource window, int attrs)
   Turns off attributes for a window */
PHP_FUNCTION(ncurses_wattroff)
{
	zval *handle;
	WINDOW **win;
	long attrs;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &handle, &attrs) == FAILURE) {
		return;
	}

	FETCH_WINRES(win, &handle);

	RETURN_LONG(wattroff(*win, attrs));
}
/* }}} */

/* {{{ proto int wattron(resource window, int attrs)
   Turns on attributes for a window */
PHP_FUNCTION(ncurses_wattron)
{
	zval *handle;
	WINDOW **win;
	long attrs;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &handle, &attrs) == FAILURE) {
		return;
	}

	FETCH_WINRES(win, &handle);

	RETURN_LONG(wattron(*win, attrs));
}
/* }}} */

/* {{{ proto int wattrset(resource window, int attrs)
   Set the attributes for a window */
PHP_FUNCTION(ncurses_wattrset)
{
	zval *handle;
	WINDOW **win;
	long attrs;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &handle, &attrs) == FAILURE) {
		return;
	}

	FETCH_WINRES(win, &handle);

	RETURN_LONG(wattrset(*win, attrs));
}
/* }}} */

/* {{{ proto int wstandend(resource window)
   End standout mode for a window */
PHP_FUNCTION(ncurses_wstandend)
{
	zval *handle;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &handle) == FAILURE) {
		return;
	}

	FETCH_WINRES(win, &handle);

	RETURN_LONG(wstandend(*win));
}
/* }}} */

/* {{{ proto int wstandout(resource window)
   Enter standout mode for a window */
PHP_FUNCTION(ncurses_wstandout)
{
	zval *handle;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &handle) == FAILURE) {
		return;
	}

	FETCH_WINRES(win, &handle);

	RETURN_LONG(wstandout(*win));
}
/* }}} */

#if HAVE_NCURSES_PANEL
/* {{{ proto resource ncurses_new_panel(resource window)
   Create a new panel and associate it with window */
PHP_FUNCTION(ncurses_new_panel)
{
	zval *handle;
	WINDOW **win;
	PANEL **panel;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &handle) == FAILURE) {
		return;
	}

	FETCH_WINRES(win, &handle);

	panel = (PANEL **)emalloc(sizeof(PANEL *));
	*panel = new_panel(*win);

	if (*panel == NULL) {
		efree(panel);
		RETURN_FALSE;
	} else {
		long id = ZEND_REGISTER_RESOURCE(return_value, panel, le_ncurses_panels);
		set_panel_userptr(*panel, (void*)id);
	}

}
/* }}} */

/* {{{ proto int ncurses_del_panel(resource panel)
   Remove panel from the stack and delete it (but not the associated window) */
PHP_FUNCTION(ncurses_del_panel)
{
	zval *handle;
	PANEL **panel;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &handle) == FAILURE) {
		return;
	}

	FETCH_PANEL(panel, &handle);

	RETURN_LONG(del_panel(*panel));
}
/* }}} */

/* {{{ proto int ncurses_hide_panel(resource panel)
   Remove panel from the stack, making it invisible */
PHP_FUNCTION(ncurses_hide_panel)
{
	zval *handle;
	PANEL **panel;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &handle) == FAILURE) {
		return;
	}
	
	FETCH_PANEL(panel, &handle);

	RETURN_LONG(hide_panel(*panel));

}
/* }}} */

/* {{{ proto int ncurses_show_panel(resource panel)
   Places an invisible panel on top of the stack, making it visible */
PHP_FUNCTION(ncurses_show_panel)
{
	zval *handle;
	PANEL **panel;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &handle) == FAILURE) {
		return;
	}

	FETCH_PANEL(panel, &handle);

	RETURN_LONG(show_panel(*panel));

}
/* }}} */

/* {{{ proto int ncurses_top_panel(resource panel)
   Moves a visible panel to the top of the stack */
PHP_FUNCTION(ncurses_top_panel)
{
	zval *handle;
	PANEL **panel;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &handle) == FAILURE) {
		return;
	}

	FETCH_PANEL(panel, &handle);

	RETURN_LONG(top_panel(*panel));

}
/* }}} */

/* {{{ proto int ncurses_bottom_panel(resource panel)
   Moves a visible panel to the bottom of the stack */
PHP_FUNCTION(ncurses_bottom_panel)
{
	zval *handle;
	PANEL **panel;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &handle) == FAILURE) {
		return;
	}

	FETCH_PANEL(panel, &handle);

	RETURN_LONG(bottom_panel(*panel));

}
/* }}} */

/* {{{ proto int ncurses_move_panel(resource panel, int startx, int starty)
   Moves a panel so that it's upper-left corner is at [startx, starty] */
PHP_FUNCTION(ncurses_move_panel)
{
	zval *handle;
	PANEL **panel;
	long startx, starty;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &handle, &startx, &starty) == FAILURE) {
		return;
	}

	FETCH_PANEL(panel, &handle);

	RETURN_LONG(move_panel(*panel, startx, starty));

}
/* }}} */

/* {{{ proto int ncurses_replace_panel(resource panel, resource window)
   Replaces the window associated with panel */
PHP_FUNCTION(ncurses_replace_panel)
{
	zval *phandle, *whandle;
	PANEL **panel;
	WINDOW **window;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &phandle, &whandle) == FAILURE) {
		return;
	}

	FETCH_PANEL(panel, &phandle);
	FETCH_WINRES(window, &whandle);

	RETURN_LONG(replace_panel(*panel, *window));

}
/* }}} */

/* {{{ proto int ncurses_panel_above(resource panel)
   Returns the panel above panel. If panel is null, returns the bottom panel in the stack */
PHP_FUNCTION(ncurses_panel_above)
{
	zval *phandle = NULL;
	PANEL **panel;
	PANEL *above;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r!", &phandle) == FAILURE) {
		return;
	}

	if (phandle) {
		FETCH_PANEL(panel, &phandle);
		above = panel_above(*panel);
	} else {
		above = panel_above((PANEL *)0);
	}

	if (above) {
		long id = (long)panel_userptr(above);
		zend_list_addref(id);
		RETURN_RESOURCE(id);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int ncurses_panel_below(resource panel)
   Returns the panel below panel. If panel is null, returns the top panel in the stack */
PHP_FUNCTION(ncurses_panel_below)
{
	zval *phandle = NULL;
	PANEL **panel;
	PANEL *below;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r!", &phandle) == FAILURE) {
		return;
	}

	if (phandle) {
		FETCH_PANEL(panel, &phandle);
		below = panel_below(*panel);
	} else {
		below = panel_below((PANEL *)0);
	}
	if (below) {
		long id = (long)panel_userptr(below);
		zend_list_addref(id);
		RETURN_RESOURCE(id);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int ncurses_panel_window(resource panel)
   Returns the window associated with panel */
PHP_FUNCTION(ncurses_panel_window)
{
	zval *phandle = NULL;
	PANEL **panel;
	WINDOW **win;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &phandle) == FAILURE) {
		return;
	}

	FETCH_PANEL(panel, &phandle);

	win = (WINDOW **)emalloc(sizeof(WINDOW *));
	*win = panel_window(*panel);

	if (*win == NULL) {
		efree(win);
		RETURN_FALSE;
	}
	ZEND_REGISTER_RESOURCE(return_value, win, le_ncurses_windows);
}
/* }}} */

/* {{{ proto void ncurses_update_panels(void)
   Refreshes the virtual screen to reflect the relations between panels in the stack. */
PHP_FUNCTION(ncurses_update_panels)
{
	IS_NCURSES_INITIALIZED();
	update_panels();
}
/* }}} */
#endif /* HAVE_NCURSES_PANEL */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
