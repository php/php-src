/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#ifdef ZEND_WIN32
# include <process.h>
#endif
#include "ZendAccelerator.h"

void zend_accel_error(int type, const char *format, ...)
{
	va_list args;
	time_t timestamp;
	char *time_string;
	FILE * fLog = NULL;

	if (type <= ZCG(accel_directives).log_verbosity_level) {

	timestamp = time(NULL);
	time_string = asctime(localtime(&timestamp));
	time_string[24] = 0;

	if (!ZCG(accel_directives).error_log ||
		!*ZCG(accel_directives).error_log ||
		strcmp(ZCG(accel_directives).error_log, "stderr") == 0) {

		fLog = stderr;
	} else {
		fLog = fopen(ZCG(accel_directives).error_log, "a");
		if (!fLog) {
			fLog = stderr;
		}
	}

#ifdef ZTS
		fprintf(fLog, "%s (" ZEND_ULONG_FMT "): ", time_string, (zend_ulong)tsrm_thread_id());
#else
		fprintf(fLog, "%s (%d): ", time_string, getpid());
#endif

		switch (type) {
			case ACCEL_LOG_FATAL:
				fprintf(fLog, "Fatal Error ");
				break;
			case ACCEL_LOG_ERROR:
				fprintf(fLog, "Error ");
				break;
			case ACCEL_LOG_WARNING:
				fprintf(fLog, "Warning ");
				break;
			case ACCEL_LOG_INFO:
				fprintf(fLog, "Message ");
				break;
			case ACCEL_LOG_DEBUG:
				fprintf(fLog, "Debug ");
				break;
		}

		va_start(args, format);
		vfprintf(fLog, format, args);
		va_end(args);
		fprintf(fLog, "\n");

		fflush(fLog);
		if (fLog != stderr) {
			fclose(fLog);
		}
	}
	/* perform error handling even without logging the error */
	switch (type) {
		case ACCEL_LOG_ERROR:
			zend_bailout();
			break;
		case ACCEL_LOG_FATAL:
			exit(-2);
			break;
	}

}
