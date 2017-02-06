/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Niklas Keller <kelunik@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef HRTIME_H
#define HRTIME_H

#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0) && defined(CLOCK_MONOTONIC)
#define HRTIME_AVAILABLE 1
#elif defined(_WIN32) || defined(_WIN64)
#define HRTIME_AVAILABLE 1
#elif defined(__APPLE__)
#define HRTIME_AVAILABLE 1
#else
#define HRTIME_AVAILABLE 0
#endif

#if HRTIME_AVAILABLE
	PHP_MINIT_FUNCTION(hrtime);
	PHP_MSHUTDOWN_FUNCTION(hrtime);

	PHP_FUNCTION(hrtime);
#endif /* HRTIME_AVAILABLE */

#endif /* HRTIME_H */
