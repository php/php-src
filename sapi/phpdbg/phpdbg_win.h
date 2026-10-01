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

#ifndef PHPDBG_WIN_H
#define PHPDBG_WIN_H

#include "winbase.h"
#include "windows.h"
#include "excpt.h"

#define PROT_READ 1
#define PROT_WRITE 2

int mprotect(void *addr, size_t size, int protection);

void phpdbg_win_set_mm_heap(zend_mm_heap *heap);

int phpdbg_exception_handler_win32(EXCEPTION_POINTERS *xp);

#endif
