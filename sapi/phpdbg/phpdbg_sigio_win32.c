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
   | Authors: Anatol Belski <ab@php.net>                                  |
   +----------------------------------------------------------------------+
*/


#include <signal.h>

#include "phpdbg.h"
#include "phpdbg_sigio_win32.h"


ZEND_EXTERN_MODULE_GLOBALS(phpdbg)


VOID
SigIoWatcherThread(VOID *p)
{
	zend_uchar sig;
	struct win32_sigio_watcher_data *swd = (struct win32_sigio_watcher_data *)p;

top:
	(void)phpdbg_consume_bytes(swd->fd, &sig, 1, -1);


	if (3 == sig) {
		/* XXX completely not sure it is done right here */
		if (*swd->flags & PHPDBG_IS_INTERACTIVE) {
			if (raise(sig)) {
				goto top;
			}
		}
		if (*swd->flags & PHPDBG_IS_SIGNALED) {
			phpdbg_set_sigsafe_mem(&sig);
			zend_try {
				phpdbg_force_interruption();
			} zend_end_try();
			phpdbg_clear_sigsafe_mem();
			goto end;
		}
		if (!(*swd->flags & PHPDBG_IS_INTERACTIVE)) {
			*swd->flags |= PHPDBG_IS_SIGNALED;
		}
end:
		/* XXX set signaled flag to the caller thread, question is - whether it's needed */
		ExitThread(sig);
	} else {
		goto top;
	}
}


/* Start this only for the time of the run or eval command,
for so long that the main thread is busy serving some debug
session. */
void
sigio_watcher_start(void)
{

	PHPDBG_G(swd).fd = PHPDBG_G(io)[PHPDBG_STDIN].fd;
#ifdef ZTS
	PHPDBG_G(swd).flags = &PHPDBG_G(flags);
#endif

	PHPDBG_G(sigio_watcher_thread) = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)SigIoWatcherThread,
		&PHPDBG_G(swd),
		0,
		NULL);
}

void
sigio_watcher_stop(void)
{
	DWORD waited;

	if (INVALID_HANDLE_VALUE == PHPDBG_G(sigio_watcher_thread)) {
		/* it probably did bail out already */
		return;
	}

	waited = WaitForSingleObject(PHPDBG_G(sigio_watcher_thread), 300);

	if (WAIT_OBJECT_0 != waited) {
		if (!CancelSynchronousIo(PHPDBG_G(sigio_watcher_thread))) {
			/* error out */
		}

		if (!TerminateThread(PHPDBG_G(sigio_watcher_thread), 0)) {
			/* error out */
		}
	}

	PHPDBG_G(swd).fd = -1;
	PHPDBG_G(sigio_watcher_thread) = INVALID_HANDLE_VALUE;
}
