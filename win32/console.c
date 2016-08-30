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

#include "win32/console.h"

BOOL php_win32_console_os_supports_vt100()
{
	const DWORD MINV_MAJOR = 10, MINV_MINOR = 0, MINV_BUILD = 10586;
	OSVERSIONINFOEX osvi = EG(windows_version_info);
	// Check operating system platform
	if (osvi.dwPlatformId != VER_PLATFORM_WIN32_NT) {
		return FALSE;
	}
	// Check major version number
	if (osvi.dwMajorVersion > MINV_MAJOR) {
		return TRUE;
	}
	if (osvi.dwMajorVersion < MINV_MAJOR) {
		return FALSE;
	}
	// Check minor version number
	if (osvi.dwMinorVersion > MINV_MINOR) {
		return TRUE;
	}
	if (osvi.dwMinorVersion < MINV_MINOR) {
		return FALSE;
	}
	// Check build number
	if (osvi.dwBuildNumber > MINV_BUILD) {
		return TRUE;
	}
	if (osvi.dwBuildNumber < MINV_BUILD) {
		return FALSE;
	}
	return TRUE;

}

BOOL php_win32_console_handle_is_redirected(DWORD handle_id)
{
	BOOL result = FALSE;
	HANDLE handle = handle_id ? GetStdHandle(handle_id) : INVALID_HANDLE_VALUE;

	if (handle != INVALID_HANDLE_VALUE) {
		if (GetFinalPathNameByHandleW(handle, NULL, 0, 0) != 0 || GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			result = TRUE;
		}
	}
	return result;
}

BOOL php_win32_console_handle_has_vt100(DWORD handle_id)
{
	BOOL result = FALSE;
	HANDLE handle = handle_id ? GetStdHandle(handle_id) : INVALID_HANDLE_VALUE;

	if (handle != INVALID_HANDLE_VALUE) {
		DWORD mode;

		if (GetConsoleMode(handle, &mode)) {
			if (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) {
				result = TRUE;
			}
		}
	}
	return result;
}

BOOL php_win32_console_handle_set_vt100(DWORD handle_id, BOOL enable)
{
	BOOL result = FALSE;
	HANDLE handle = handle_id ? GetStdHandle(handle_id) : INVALID_HANDLE_VALUE;

	if (handle != INVALID_HANDLE_VALUE) {
		DWORD mode;

		if (GetConsoleMode(handle, &mode)) {
			if (((mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) ? 1 : 0) == (enable ? 1 : 0)) {
				result = TRUE;
			}
			else {
				if (enable) {
					mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				}
				else {
					mode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				}
				if (SetConsoleMode(handle, mode)) {
					result = TRUE;
				}
			}
		}
	}
	return result;
}
