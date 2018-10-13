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
   | Authors: Kalle Sommer Nielsen <kalle@php.net>                        |
   +----------------------------------------------------------------------+
 */

#include <php.h>
#include "nice.h"

/*
 * Basic Windows implementation for the nice() function.
 *
 * This implementation uses SetPriorityClass() as a backend for defining
 * a process priority.
 *
 * The following values of inc, defines the value sent to SetPriorityClass():
 *
 *  +-----------------------+-----------------------------+
 *  | Expression            | Priority type                |
 *  +-----------------------+-----------------------------+
 *  | priority < -9         | HIGH_PRIORITY_CLASS          |
 *  +-----------------------+-----------------------------+
 *  | priority < -4         | ABOVE_NORMAL_PRIORITY_CLASS  |
 *  +-----------------------+-----------------------------+
 *  | priority > 4          | BELOW_NORMAL_PRIORITY_CLASS  |
 *  +-----------------------+-----------------------------+
 *  | priority > 9          | IDLE_PRIORITY_CLASS          |
 *  +-----------------------+-----------------------------+
 *
 * If a value is between -4 and 4 (inclusive), then the priority will be set
 * to NORMAL_PRIORITY_CLASS.
 *
 * These values tries to mimic that of the UNIX version of nice().
 *
 * This is applied to the main process, not per thread, although this could
 * be implemented using SetThreadPriority() at one point.
 *
 * Note, the following priority classes are left out with intention:
 *
 *   . REALTIME_PRIORITY_CLASS
 *     Realtime priority class requires special system permissions to set, and
 *     can be dangerous in certain cases.
 *
 *   . PROCESS_MODE_BACKGROUND_BEGIN
 *   . PROCESS_MODE_BACKGROUND_END
 *     Process mode is not covered because it can easily forgotten to be changed
 *     back and can cause unforseen side effects that is hard to debug. Besides
 *     that, these do generally not really fit into making a Windows somewhat
 *     compatible nice() function.
 */

PHPAPI int nice(zend_long p)
{
	DWORD dwFlag = NORMAL_PRIORITY_CLASS;

	if (p < -9) {
		dwFlag = HIGH_PRIORITY_CLASS;
	} else if (p < -4) {
		dwFlag = ABOVE_NORMAL_PRIORITY_CLASS;
	} else if (p > 9) {
		dwFlag = IDLE_PRIORITY_CLASS;
	} else if (p > 4) {
		dwFlag = BELOW_NORMAL_PRIORITY_CLASS;
	}

	if (!SetPriorityClass(GetCurrentProcess(), dwFlag)) {
		return -1;
	}

	return 0;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
