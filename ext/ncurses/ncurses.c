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
   | Author: Hartmut Holzgraefe <hartmut@six.de>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_ncurses.h"
#include "ext/standard/info.h"

/* If you declare any globals in php_ncurses.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(ncurses)
*/

/* True global resources - no need for thread safety here */
int le_ncurses_windows;
#if HAVE_NCURSES_PANEL
int le_ncurses_panels;
#endif

static void ncurses_destruct_window(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    WINDOW **pwin = (WINDOW **)rsrc->ptr;

    delwin(*pwin);
    efree(pwin);
}

#if HAVE_NCURSES_PANEL
static void ncurses_destruct_panel(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	PANEL **ppanel = (PANEL **)rsrc->ptr;

	del_panel(*ppanel);
	efree(ppanel);
}
#endif

/* {{{ ncurses_module_entry
 */
zend_module_entry ncurses_module_entry = {
    STANDARD_MODULE_HEADER,
    "ncurses",
    ncurses_functions,
    PHP_MINIT(ncurses),
    PHP_MSHUTDOWN(ncurses),
    PHP_RINIT(ncurses),     /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(ncurses), /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(ncurses),
    NO_VERSION_YET,
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_NCURSES
ZEND_GET_MODULE(ncurses)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("ncurses.value",      "42", PHP_INI_ALL, OnUpdateInt, global_value, zend_ncurses_globals, ncurses_globals)
    STD_PHP_INI_ENTRY("ncurses.string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_ncurses_globals, ncurses_globals)
PHP_INI_END()
*/
/* }}} */

#define PHP_NCURSES_CONST(x)  REGISTER_LONG_CONSTANT("NCURSES_"#x, x, CONST_CS | CONST_PERSISTENT)
#define PHP_NCURSES_FKEY_CONST(x)  REGISTER_LONG_CONSTANT("NCURSES_KEY_F"#x, KEY_F0 + x, CONST_CS | CONST_PERSISTENT)


/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ncurses)
{
    /* color constants */
    PHP_NCURSES_CONST(COLOR_BLACK);
    PHP_NCURSES_CONST(COLOR_RED);
    PHP_NCURSES_CONST(COLOR_GREEN);
    PHP_NCURSES_CONST(COLOR_YELLOW);
    PHP_NCURSES_CONST(COLOR_BLUE);
    PHP_NCURSES_CONST(COLOR_MAGENTA);
    PHP_NCURSES_CONST(COLOR_CYAN);
    PHP_NCURSES_CONST(COLOR_WHITE);

    /* keyboard constants */
    PHP_NCURSES_CONST(KEY_DOWN);
    PHP_NCURSES_CONST(KEY_UP);
    PHP_NCURSES_CONST(KEY_LEFT);
    PHP_NCURSES_CONST(KEY_RIGHT);
    PHP_NCURSES_CONST(KEY_BACKSPACE);
    PHP_NCURSES_CONST(KEY_MOUSE);
    PHP_NCURSES_CONST(KEY_F0);


    /* TODO:this macro sux, we have 65 function key,
       so we need a little loop */
    PHP_NCURSES_FKEY_CONST(1);
    PHP_NCURSES_FKEY_CONST(2);
    PHP_NCURSES_FKEY_CONST(3);
    PHP_NCURSES_FKEY_CONST(4);
    PHP_NCURSES_FKEY_CONST(5);
    PHP_NCURSES_FKEY_CONST(6);
    PHP_NCURSES_FKEY_CONST(7);
    PHP_NCURSES_FKEY_CONST(8);
    PHP_NCURSES_FKEY_CONST(9);
    PHP_NCURSES_FKEY_CONST(10);
    PHP_NCURSES_FKEY_CONST(11);
    PHP_NCURSES_FKEY_CONST(12);

    PHP_NCURSES_CONST(KEY_DL);
    PHP_NCURSES_CONST(KEY_IL);
    PHP_NCURSES_CONST(KEY_DC);
    PHP_NCURSES_CONST(KEY_IC);
    PHP_NCURSES_CONST(KEY_EIC);
    PHP_NCURSES_CONST(KEY_CLEAR);
    PHP_NCURSES_CONST(KEY_EOS);
    PHP_NCURSES_CONST(KEY_EOL);
    PHP_NCURSES_CONST(KEY_SF);
    PHP_NCURSES_CONST(KEY_SR);
    PHP_NCURSES_CONST(KEY_NPAGE);
    PHP_NCURSES_CONST(KEY_PPAGE);
    PHP_NCURSES_CONST(KEY_STAB);
    PHP_NCURSES_CONST(KEY_CTAB);
    PHP_NCURSES_CONST(KEY_CATAB);
    PHP_NCURSES_CONST(KEY_ENTER);
    PHP_NCURSES_CONST(KEY_SRESET);
    PHP_NCURSES_CONST(KEY_RESET);
    PHP_NCURSES_CONST(KEY_PRINT);
    PHP_NCURSES_CONST(KEY_LL);
    PHP_NCURSES_CONST(KEY_A1);
    PHP_NCURSES_CONST(KEY_A3);
    PHP_NCURSES_CONST(KEY_B2);
    PHP_NCURSES_CONST(KEY_C1);
    PHP_NCURSES_CONST(KEY_C3);
    PHP_NCURSES_CONST(KEY_BTAB);
    PHP_NCURSES_CONST(KEY_BEG);
    PHP_NCURSES_CONST(KEY_CANCEL);
    PHP_NCURSES_CONST(KEY_CLOSE);
    PHP_NCURSES_CONST(KEY_COMMAND);
    PHP_NCURSES_CONST(KEY_COPY);
    PHP_NCURSES_CONST(KEY_CREATE);
    PHP_NCURSES_CONST(KEY_END);
    PHP_NCURSES_CONST(KEY_EXIT);
    PHP_NCURSES_CONST(KEY_FIND);
    PHP_NCURSES_CONST(KEY_HELP);
    PHP_NCURSES_CONST(KEY_MARK);
    PHP_NCURSES_CONST(KEY_MESSAGE);
    PHP_NCURSES_CONST(KEY_MOVE);
    PHP_NCURSES_CONST(KEY_NEXT);
    PHP_NCURSES_CONST(KEY_OPEN);
    PHP_NCURSES_CONST(KEY_OPTIONS);
    PHP_NCURSES_CONST(KEY_PREVIOUS);
    PHP_NCURSES_CONST(KEY_REDO);
    PHP_NCURSES_CONST(KEY_REFERENCE);
    PHP_NCURSES_CONST(KEY_REFRESH);
    PHP_NCURSES_CONST(KEY_REPLACE);
    PHP_NCURSES_CONST(KEY_RESTART);
    PHP_NCURSES_CONST(KEY_RESUME);
    PHP_NCURSES_CONST(KEY_SAVE);
    PHP_NCURSES_CONST(KEY_SBEG);
    PHP_NCURSES_CONST(KEY_SCANCEL);
    PHP_NCURSES_CONST(KEY_SCOMMAND);
    PHP_NCURSES_CONST(KEY_SCOPY);
    PHP_NCURSES_CONST(KEY_SCREATE);
    PHP_NCURSES_CONST(KEY_SDC);
    PHP_NCURSES_CONST(KEY_SDL);
    PHP_NCURSES_CONST(KEY_SELECT);
    PHP_NCURSES_CONST(KEY_SEND);
    PHP_NCURSES_CONST(KEY_SEOL);
    PHP_NCURSES_CONST(KEY_SEXIT);
    PHP_NCURSES_CONST(KEY_SFIND);
    PHP_NCURSES_CONST(KEY_SHELP);
    PHP_NCURSES_CONST(KEY_SHOME);
    PHP_NCURSES_CONST(KEY_SIC);
    PHP_NCURSES_CONST(KEY_SLEFT);
    PHP_NCURSES_CONST(KEY_SMESSAGE);
    PHP_NCURSES_CONST(KEY_SMOVE);
    PHP_NCURSES_CONST(KEY_SNEXT);
    PHP_NCURSES_CONST(KEY_SOPTIONS);
    PHP_NCURSES_CONST(KEY_SPREVIOUS);
    PHP_NCURSES_CONST(KEY_SPRINT);
    PHP_NCURSES_CONST(KEY_SREDO);
    PHP_NCURSES_CONST(KEY_SREPLACE);
    PHP_NCURSES_CONST(KEY_SRIGHT);
    PHP_NCURSES_CONST(KEY_SRSUME);
    PHP_NCURSES_CONST(KEY_SSAVE);
    PHP_NCURSES_CONST(KEY_SSUSPEND);
    PHP_NCURSES_CONST(KEY_SUNDO);
    PHP_NCURSES_CONST(KEY_SUSPEND);
    PHP_NCURSES_CONST(KEY_UNDO);
    PHP_NCURSES_CONST(KEY_RESIZE);

    /* screen attribute constants */
    PHP_NCURSES_CONST(A_NORMAL);
    PHP_NCURSES_CONST(A_STANDOUT);
    PHP_NCURSES_CONST(A_UNDERLINE);
    PHP_NCURSES_CONST(A_REVERSE);
    PHP_NCURSES_CONST(A_BLINK);
    PHP_NCURSES_CONST(A_DIM);
    PHP_NCURSES_CONST(A_BOLD);
    PHP_NCURSES_CONST(A_PROTECT);
    PHP_NCURSES_CONST(A_INVIS);
    PHP_NCURSES_CONST(A_ALTCHARSET);
    PHP_NCURSES_CONST(A_CHARTEXT);

    /* mouse constants */
    PHP_NCURSES_CONST(BUTTON1_PRESSED);
    PHP_NCURSES_CONST(BUTTON1_RELEASED);
    PHP_NCURSES_CONST(BUTTON1_CLICKED);
    PHP_NCURSES_CONST(BUTTON1_DOUBLE_CLICKED);
    PHP_NCURSES_CONST(BUTTON1_TRIPLE_CLICKED);
    PHP_NCURSES_CONST(BUTTON2_PRESSED);
    PHP_NCURSES_CONST(BUTTON2_RELEASED);
    PHP_NCURSES_CONST(BUTTON2_CLICKED);
    PHP_NCURSES_CONST(BUTTON2_DOUBLE_CLICKED);
    PHP_NCURSES_CONST(BUTTON2_TRIPLE_CLICKED);
    PHP_NCURSES_CONST(BUTTON3_PRESSED);
    PHP_NCURSES_CONST(BUTTON3_RELEASED);
    PHP_NCURSES_CONST(BUTTON3_CLICKED);
    PHP_NCURSES_CONST(BUTTON3_DOUBLE_CLICKED);
    PHP_NCURSES_CONST(BUTTON3_TRIPLE_CLICKED);
    PHP_NCURSES_CONST(BUTTON4_PRESSED);
    PHP_NCURSES_CONST(BUTTON4_RELEASED);
    PHP_NCURSES_CONST(BUTTON4_CLICKED);
    PHP_NCURSES_CONST(BUTTON4_DOUBLE_CLICKED);
    PHP_NCURSES_CONST(BUTTON4_TRIPLE_CLICKED);
    PHP_NCURSES_CONST(BUTTON_SHIFT);
    PHP_NCURSES_CONST(BUTTON_CTRL);
    PHP_NCURSES_CONST(BUTTON_ALT);
    PHP_NCURSES_CONST(ALL_MOUSE_EVENTS);
    PHP_NCURSES_CONST(REPORT_MOUSE_POSITION);

    le_ncurses_windows = zend_register_list_destructors_ex(ncurses_destruct_window, NULL, "ncurses_window", module_number);
#if HAVE_NCURSES_PANEL
    le_ncurses_panels = zend_register_list_destructors_ex(ncurses_destruct_panel, NULL, "ncurses_panel", module_number);
#endif

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(ncurses)
{
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(ncurses)
{
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(ncurses)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(ncurses)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "ncurses support", "enabled");
    php_info_print_table_row(2, "ncurses library version", NCURSES_VERSION);
#ifdef HAVE_NCURSES_COLOR_SET
		php_info_print_table_row(2, "color support", "yes");
#else
		php_info_print_table_row(2, "color support", "no");
#endif
    php_info_print_table_end();

    /* Remove comments if you have entries in php.ini
    DISPLAY_INI_ENTRIES();
    */
}
/* }}} */

/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
