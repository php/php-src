/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Daniel Beulshausen <daniel@php4win.de>                      |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef TSRM_WIN32_H
#define TSRM_WIN32_H

#include "TSRM.h"

#ifdef TSRM_WIN32
#include <windows.h>

typedef struct {
	FILE *stream;
	HANDLE prochnd;
} ProcessPair;

typedef struct {
	ProcessPair *process;
	int process_size;
	char *comspec;
} tsrm_win32_globals;

#ifdef ZTS
# define TWG(v) (win32_globals->v)
# define TWLS_FETCH() tsrm_win32_globals *win32_globals = ts_resource(win32_globals_id)
#else
# define TWG(v) (win32_globals.v)
# define TWLS_FETCH()
#endif
#endif

TSRM_API void tsrm_win32_startup(void);
TSRM_API void tsrm_win32_shutdown(void);
TSRM_API FILE* popen(const char *command, const char *type);
TSRM_API int pclose(FILE* stream);

#endif