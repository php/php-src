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
   | Author: Jason Greene <jason@inetgurus.net>                           |
   +----------------------------------------------------------------------+
*/

#include <signal.h>
#ifndef PHP_SIGNAL_H
#define PHP_SIGNAL_H

typedef void Sigfunc(int, siginfo_t*, void*);

Sigfunc *php_signal(int signo, Sigfunc *func, bool restart);
Sigfunc *php_signal4(int signo, Sigfunc *func, bool restart, bool mask_all);

#endif
