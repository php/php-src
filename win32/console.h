/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
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

/* $Id$ */

#ifndef PHP_WIN32_CONSOLE_H
#define PHP_WIN32_CONSOLE_H

#include "php.h"
#include "php_streams.h"
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

/* Pointer type to the RtlGetVersion function of ntdll.dll */
typedef LONG (WINAPI * _php_win32_console_rtlgetversion) (PRTL_OSVERSIONINFOW);

/* Check if the current Windows version supports VT100 control codes */
BOOL _php_win32_console_os_supports_vt100();

/* Check if the standard handle (STD_OUTPUT_HANDLE, STD_ERROR_HANDLE)
   is redirected to a file */
BOOL _php_win32_console_handle_is_redirected(DWORD handle_id);

/* Check if the console attached to the specified standard handle
   (STD_OUTPUT_HANDLE, STD_ERROR_HANDLE) has the
   ENABLE_VIRTUAL_TERMINAL_PROCESSING flag set */
BOOL _php_win32_console_handle_has_vt100(DWORD handle_id);

/* Set/unset the ENABLE_VIRTUAL_TERMINAL_PROCESSING flag for the console
   attached to the specified standard handle (STD_OUTPUT_HANDLE,
   STD_ERROR_HANDLE) */
BOOL _php_win32_console_handle_set_vt100(DWORD handle_id, BOOL enable);

#endif
