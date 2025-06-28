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
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/
#ifndef DATE_FORMATTER_H
#define DATE_FORMATTER_H

#include <php.h>

/*
These are not necessary at this point of time
#define DATEF_GREGORIAN 1
#define DATEF_CUSTOMARY 2
#define DATEF_BUDDHIST 3
#define DATEF_JAPANESE_IMPERIAL 4
*/

#define CALENDAR_SEC "tm_sec"
#define CALENDAR_MIN "tm_min"
#define CALENDAR_HOUR "tm_hour"
#define CALENDAR_MDAY "tm_mday"
#define CALENDAR_MON "tm_mon"
#define CALENDAR_YEAR "tm_year"
#define CALENDAR_WDAY "tm_wday"
#define CALENDAR_YDAY "tm_yday"
#define CALENDAR_ISDST "tm_isdst"

#endif // DATE_FORMATTER_H
