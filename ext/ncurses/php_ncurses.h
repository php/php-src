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

#ifndef PHP_NCURSES_H
#define PHP_NCURSES_H

#include <curses.h>

extern int le_ncurses_windows;

#if HAVE_NCURSES_PANEL
# include <panel.h>
extern int le_ncurses_panels;
#endif


extern zend_module_entry ncurses_module_entry;
#define phpext_ncurses_ptr &ncurses_module_entry

extern function_entry ncurses_functions[];
#include "php_ncurses_fe.h"

#ifdef PHP_WIN32
#define PHP_NCURSES_API __declspec(dllexport)
#else
#define PHP_NCURSES_API
#endif

PHP_MINIT_FUNCTION(ncurses);
PHP_MSHUTDOWN_FUNCTION(ncurses);
PHP_RINIT_FUNCTION(ncurses);
PHP_RSHUTDOWN_FUNCTION(ncurses);
PHP_MINFO_FUNCTION(ncurses);

ZEND_BEGIN_MODULE_GLOBALS(ncurses)
	int	  registered_constants;
ZEND_END_MODULE_GLOBALS(ncurses)

/* In every function that needs to use variables in php_ncurses_globals,
   do call NCURSES_LS_FETCH(); after declaring other variables used by
   that function, and always refer to them as NCURSES_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define NCURSES_G(v) TSRMG(ncurses_globals_id, zend_ncurses_globals *, v)
#define NCURSES_LS_FETCH() zend_ncurses_globals *ncurses_globals = ts_resource(ncurses_globals_id)
#else
#define NCURSES_G(v) (ncurses_globals.v)
#define NCURSES_LS_FETCH()
#endif

ZEND_EXTERN_MODULE_GLOBALS(ncurses);
	
#endif  /* PHP_NCURSES_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
