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
   | Author: Jason Greene <jason@inetgurus.net>                           |
   +----------------------------------------------------------------------+
*/

#include <signal.h>
#ifndef PHP_SIGNAL_H
#define PHP_SIGNAL_H

#ifdef HAVE_STRUCT_SIGINFO_T
typedef void Sigfunc(int, siginfo_t*, void*);
#else
typedef void Sigfunc(int);
#endif
Sigfunc *php_signal(int signo, Sigfunc *func, int restart);
Sigfunc *php_signal4(int signo, Sigfunc *func, int restart, int mask_all);

#endif
