/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Michele Locati <mlocati@gmail.com>                           |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "SAPI.h"
#include "win32/console.h"

PHP_WINUTIL_API BOOL php_win32_console_fileno_is_console(zend_long fileno)
{
    HANDLE handle = (HANDLE)_get_osfhandle(fileno);
    
    if (handle == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    DWORD mode;
    return GetConsoleMode(handle, &mode);
}

PHP_WINUTIL_API BOOL php_win32_console_fileno_has_vt100(zend_long fileno)
{
    HANDLE handle = (HANDLE)_get_osfhandle(fileno);
    
    if (handle == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    // Skip STDIN check for non-input handles
    if (fileno != 0) {
        DWORD mode;
        if (GetConsoleMode(handle, &mode)) {
            return (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
        }
    }

    return FALSE;
}

PHP_WINUTIL_API BOOL php_win32_console_fileno_set_vt100(zend_long fileno, BOOL enable)
{
    HANDLE handle = (HANDLE)_get_osfhandle(fileno);
    
    if (handle == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    // Skip STDIN check for non-input handles
    if (fileno != 0) {
        DWORD mode;
        if (!GetConsoleMode(handle, &mode)) {
            return FALSE;
        }

        const DWORD newMode = enable 
            ? (mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)
            : (mode & ~ENABLE_VIRTUAL_TERMINAL_PROCESSING);

        if (newMode == mode) {
            return TRUE;
        }

        return SetConsoleMode(handle, newMode);
    }

    return FALSE;
}

PHP_WINUTIL_API BOOL php_win32_console_is_own(void)
{
    if (IsDebuggerPresent()) {
        return FALSE;
    }

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    const HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    
    // Check cursor position
    BOOL atOrigin = FALSE;
    if (GetConsoleScreenBufferInfo(stdoutHandle, &csbi)) {
        atOrigin = (csbi.dwCursorPosition.X == 0 && csbi.dwCursorPosition.Y == 0);
    }

    // Check process count
    DWORD processList[1];
    const DWORD processCount = GetConsoleProcessList(processList, 1);

    return atOrigin && (processCount == 1);
}

PHP_WINUTIL_API BOOL php_win32_console_is_cli_sapi(void)
{
    static const char CLI_SAPI_NAME[] = "cli";
    static const size_t CLI_SAPI_NAME_LEN = sizeof(CLI_SAPI_NAME) - 1;
    
    return strncasecmp(sapi_module.name, CLI_SAPI_NAME, CLI_SAPI_NAME_LEN) == 0;
}
