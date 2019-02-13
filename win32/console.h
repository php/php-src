/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Michele Locati <mlocati@gmail.com>                           |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_WIN32_CONSOLE_H
#define PHP_WIN32_CONSOLE_H

#ifndef PHP_WINUTIL_API
#ifdef PHP_EXPORTS
# define PHP_WINUTIL_API __declspec(dllexport)
#else
# define PHP_WINUTIL_API __declspec(dllimport)
#endif
#endif

#include "php.h"
#include "php_streams.h"
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif


/*
Check if a file descriptor associated to a stream is a console
(valid fileno, neither redirected nor piped)
*/
PHP_WINUTIL_API BOOL php_win32_console_fileno_is_console(zend_long fileno);

/*
Check if the console attached to a file descriptor (screen buffer, not STDIN)
has the ENABLE_VIRTUAL_TERMINAL_PROCESSING flag set
*/
PHP_WINUTIL_API BOOL php_win32_console_fileno_has_vt100(zend_long fileno);

/*
Set/unset the ENABLE_VIRTUAL_TERMINAL_PROCESSING flag for the screen buffer (STDOUT/STDERR)
associated to a file descriptor
*/
PHP_WINUTIL_API BOOL php_win32_console_fileno_set_vt100(zend_long fileno, BOOL enable);

/* Check, whether the program has its own console. If a process was launched
	through a GUI, it will have it's own console. For more info see
	http://support.microsoft.com/kb/99115 */
PHP_WINUTIL_API BOOL php_win32_console_is_own(void);

/* Check whether the current SAPI is run on console. */
PHP_WINUTIL_API BOOL php_win32_console_is_cli_sapi(void);

#endif
