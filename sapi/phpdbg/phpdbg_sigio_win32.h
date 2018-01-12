/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 2014-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Anatol Belski <ab@php.net>                                  |
   +----------------------------------------------------------------------+
*/


#ifndef PHPDBG_SIGIO_WIN32_H
#define PHPDBG_SIGIO_WIN32_H

#include "phpdbg.h"
#include "phpdbg_prompt.h"
#include "phpdbg_io.h"

struct win32_sigio_watcher_data {
	uint64_t *flags;
	int fd;
};

void
sigio_watcher_start(void);

void
sigio_watcher_stop(void);

#endif /* PHPDBG_SIGIO_WIN32_H */
