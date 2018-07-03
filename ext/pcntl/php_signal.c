/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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
#include "Zend/zend_signal.h"

/* php_signal using sigaction is derived from Advanced Programing
 * in the Unix Environment by W. Richard Stevens p 298. */
Sigfunc *php_signal4(int signo, Sigfunc *func, int restart, int mask_all)
{
	struct sigaction act,oact;

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
	if (signo == SIGALRM || (! restart)) {
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT; /* SunOS */
#endif
	} else {
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART; /* SVR4, 4.3+BSD */
#endif
	}
	if (zend_sigaction(signo, &act, &oact) < 0) {
		return (Sigfunc*)SIG_ERR;
	}

#ifdef HAVE_STRUCT_SIGINFO_T
	return oact.sa_sigaction;
#else
	return oact.sa_handler;
#endif
}

Sigfunc *php_signal(int signo, Sigfunc *func, int restart)
{
	return php_signal4(signo, func, restart, 0);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
