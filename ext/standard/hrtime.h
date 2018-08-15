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
   | Author: Niklas Keller <kelunik@php.net>                              |
   | Author: Anatol Belski <ab@php.net>                                   |
   +----------------------------------------------------------------------+
*/

#ifndef HRTIME_H
#define HRTIME_H

#define PHP_HRTIME_PLATFORM_POSIX   0
#define PHP_HRTIME_PLATFORM_WINDOWS 0
#define PHP_HRTIME_PLATFORM_APPLE   0
#define PHP_HRTIME_PLATFORM_HPUX    0
#define PHP_HRTIME_PLATFORM_AIX     0

#if defined(_POSIX_TIMERS) && ((_POSIX_TIMERS > 0) || defined(__OpenBSD__)) && defined(_POSIX_MONOTONIC_CLOCK) && defined(CLOCK_MONOTONIC)
# undef  PHP_HRTIME_PLATFORM_POSIX
# define PHP_HRTIME_PLATFORM_POSIX 1
#elif defined(_WIN32) || defined(_WIN64)
# undef  PHP_HRTIME_PLATFORM_WINDOWS
# define PHP_HRTIME_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
# undef  PHP_HRTIME_PLATFORM_APPLE
# define PHP_HRTIME_PLATFORM_APPLE 1
#elif (defined(__hpux) || defined(hpux)) || ((defined(__sun__) || defined(__sun) || defined(sun)) && (defined(__SVR4) || defined(__svr4__)))
# undef  PHP_HRTIME_PLATFORM_HPUX
# define PHP_HRTIME_PLATFORM_HPUX 1
#elif defined(_AIX)
# undef  PHP_HRTIME_PLATFORM_AIX
# define PHP_HRTIME_PLATFORM_AIX 1
#endif

#define HRTIME_AVAILABLE (PHP_HRTIME_PLATFORM_POSIX || PHP_HRTIME_PLATFORM_WINDOWS || PHP_HRTIME_PLATFORM_APPLE || PHP_HRTIME_PLATFORM_HPUX || PHP_HRTIME_PLATFORM_AIX)

typedef uint64_t php_hrtime_t;

PHPAPI php_hrtime_t php_hrtime_current(void);

PHP_MINIT_FUNCTION(hrtime);

PHP_FUNCTION(hrtime);

#endif /* HRTIME_H */
