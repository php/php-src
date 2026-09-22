/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Michele Locati <mlocati@gmail.com>                           |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "SAPI.h"
#include "win32/console.h"


PHP_WINUTIL_API BOOL php_win32_console_fileno_is_console(zend_long fileno)
{/*{{{*/
	BOOL result = FALSE;
	HANDLE handle = (HANDLE) _get_osfhandle(fileno);

	if (handle != INVALID_HANDLE_VALUE) {
		DWORD mode;
		if (GetConsoleMode(handle, &mode)) {
			result = TRUE;
		}
	}
	return result;
}/*}}}*/

PHP_WINUTIL_API BOOL php_win32_console_fileno_has_vt100(zend_long fileno)
{/*{{{*/
	BOOL result = FALSE;
	HANDLE handle = (HANDLE) _get_osfhandle(fileno);

	if (handle != INVALID_HANDLE_VALUE) {
		DWORD events;

		if (fileno != 0 && !GetNumberOfConsoleInputEvents(handle, &events)) {
			// Not STDIN
			DWORD mode;

			if (GetConsoleMode(handle, &mode)) {
				if (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) {
					result = TRUE;
				}
			}
		}
	}
	return result;
}/*}}}*/

PHP_WINUTIL_API BOOL php_win32_console_fileno_set_vt100(zend_long fileno, BOOL enable)
{/*{{{*/
	BOOL result = FALSE;
	HANDLE handle = (HANDLE) _get_osfhandle(fileno);

	if (handle != INVALID_HANDLE_VALUE) {
		DWORD events;

		if (fileno != 0 && !GetNumberOfConsoleInputEvents(handle, &events)) {
			// Not STDIN
			DWORD mode;

			if (GetConsoleMode(handle, &mode)) {
				DWORD newMode;

				if (enable) {
					newMode = mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				}
				else {
					newMode = mode & ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				}
				if (newMode == mode) {
					result = TRUE;
				}
				else {
					if (SetConsoleMode(handle, newMode)) {
						result = TRUE;
					}
				}
			}
		}
	}
	return result;
}/*}}}*/

PHP_WINUTIL_API BOOL php_win32_console_is_own(void)
{/*{{{*/
	if (!IsDebuggerPresent()) {
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		DWORD pl[1];
		BOOL ret0 = FALSE, ret1 = FALSE;

		if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
			ret0 = !csbi.dwCursorPosition.X && !csbi.dwCursorPosition.Y;
		}

		ret1 = GetConsoleProcessList(pl, 1) == 1;

		return ret0 && ret1;
	}

	return FALSE;
}/*}}}*/

PHP_WINUTIL_API BOOL php_win32_console_is_cli_sapi(void)
{/*{{{*/
	return strlen(sapi_module.name) >= sizeof("cli") - 1 && !strncmp(sapi_module.name, "cli", sizeof("cli") - 1);
}/*}}}*/

