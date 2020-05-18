/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Jason Greene <jason@inetgurus.net>                           |
   +----------------------------------------------------------------------+
*/

#include "TSRM.h"
#include "php_signal.h"
#include "Zend/zend.h"

#ifdef ZTS
# define php_sigprocmask(how, set, oldset) tsrm_sigmask((how), (set), (oldset))
#else
# define php_sigprocmask(how, set, oldset) sigprocmask((how), (set), (oldset))
#endif

/* php_signal using sigaction is derived from Advanced Programming
 * in the Unix Environment by W. Richard Stevens p 298. */
int php_signal4(int signo, Sigfunc *func, int restart, int mask_all)
{
	struct sigaction act;
	sigset_t sigset;

#ifdef HAVE_STRUCT_SIGINFO_T
	act.sa_sigaction = func;
#else
	act.sa_handler = func;
#endif
	if (mask_all) {
		sigfillset(&act.sa_mask);
	} else {
		sigemptyset(&act.sa_mask);
	}
	act.sa_flags = 0;
#ifdef HAVE_STRUCT_SIGINFO_T
	act.sa_flags |= SA_SIGINFO;
#endif
	if (!restart) {
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT; /* SunOS */
#endif
	} else {
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART; /* SVR4, 4.3+BSD */
#endif
	}
	if (sigaction(signo, &act, NULL) < 0) {
		return FAILURE;
	}

	/* Ensure this signal is not blocked */
	sigemptyset(&sigset);
	sigaddset(&sigset, signo);
	php_sigprocmask(SIG_UNBLOCK, &sigset, NULL);

	return SUCCESS;
}

int php_signal(int signo, Sigfunc *func, int restart)
{
	return php_signal4(signo, func, restart, 0);
}
