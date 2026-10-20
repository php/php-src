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
 *     back and can cause unforeseen side effects that is hard to debug. Besides
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
