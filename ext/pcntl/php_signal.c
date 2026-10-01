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

#include "TSRM.h"
#include "php_signal.h"
#include "Zend/zend.h"
#include "Zend/zend_signal.h"

/* php_signal using sigaction is derived from Advanced Programming
 * in the Unix Environment by W. Richard Stevens p 298. */
Sigfunc *php_signal4(int signo, Sigfunc *func, bool restart, bool mask_all)
{
	struct sigaction act,oact;

	act.sa_sigaction = func;
	if (mask_all) {
		sigfillset(&act.sa_mask);
	} else {
		sigemptyset(&act.sa_mask);
	}
	act.sa_flags = SA_ONSTACK;
	act.sa_flags |= SA_SIGINFO;
	if (!restart) {
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT; /* SunOS */
#endif
	} else {
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART; /* SVR4, 4.3+BSD */
#endif
	}
	zend_sigaction(signo, &act, &oact);

	return oact.sa_sigaction;
}

Sigfunc *php_signal(int signo, Sigfunc *func, bool restart)
{
	return php_signal4(signo, func, restart, false);
}
