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
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "phpdbg.h"

int mprotect(void *addr, size_t size, int protection) {
	DWORD var;
	return (int)VirtualProtect(addr, size, protection == (PROT_READ | PROT_WRITE) ? PAGE_READWRITE : PAGE_READONLY, &var);
}

int phpdbg_exception_handler_win32(EXCEPTION_POINTERS *xp) {
	EXCEPTION_RECORD *xr = xp->ExceptionRecord;

	switch (xr->ExceptionCode) {
		case EXCEPTION_ACCESS_VIOLATION:
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		case EXCEPTION_STACK_OVERFLOW:
			if (phpdbg_watchpoint_segfault_handler((void *)xr->ExceptionInformation[1]) == SUCCESS) {
				return EXCEPTION_CONTINUE_EXECUTION;
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	return EXCEPTION_CONTINUE_SEARCH;
}
