/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Georg Richter <georg@php.net>                                |
  +----------------------------------------------------------------------+

  $Id$
*/

#ifndef __HAVE_MYSQLI_PROFILER_H__
#define __HAVE_MYSQLI_PROFILER_H__

#ifdef PHP_WIN32
#include <process.h>
#include <direct.h>
#include "win32/time.h"
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#endif

typedef struct {
	struct timeval	starttime, 
					endtime;		/* execution time */
} PR_TIME_INFO;


#define MYSQLI_PR_REPORT_STDERR		1
#define MYSQLI_PR_REPORT_PORT		2



/*** PROFILER MACROS ***/
#define MYSQLI_PROFILER_STARTTIME(ptr) if (MyG(profiler.mode)) gettimeofday(&ptr.starttime, NULL)
#define MYSQLI_PROFILER_ENDTIME(ptr) if (MyG(profiler.mode)) gettimeofday(&ptr.endtime, NULL)
#define MYSQLI_PROFILER_REPORT(_type, _time, ptr) if (MyG(profiler.mode)) php_mysqli_profiler_report(_type, _time, (void *)ptr)



#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
