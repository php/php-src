#ifndef PHP_WIN32_SIGNAL_H
#define PHP_WIN32_SIGNAL_H

#include <signal.h>

#include "win32/winutil.h"

#define SIGALRM 13
#define	SIGVTALRM 26			/* virtual time alarm */
#define	SIGPROF	27				/* profiling time alarm */

PHP_WINUTIL_API void php_win32_signal_ctrl_handler_init(void);
PHP_WINUTIL_API void php_win32_signal_ctrl_handler_shutdown(void);

#endif /* PHP_WIN32_SIGNAL_H */
