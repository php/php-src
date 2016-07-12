/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,	  |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "phpdbg.h"

int mprotect(void *addr, size_t size, int protection) {
	int var;
	return (int)VirtualProtect(addr, size, protection == (PROT_READ | PROT_WRITE) ? PAGE_READWRITE : PAGE_READONLY, &var);
}

int phpdbg_exception_handler_win32(EXCEPTION_POINTERS *xp) {
	EXCEPTION_RECORD *xr = xp->ExceptionRecord;
	CONTEXT *xc = xp->ContextRecord;

	if(xr->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {

		if (phpdbg_watchpoint_segfault_handler((void *)xr->ExceptionInformation[1]) == SUCCESS) {
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}
