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

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <process.h>

#include "TSRM.h"

#ifdef TSRM_WIN32
#include <windows.h>
#include "tsrm_win32.h"

#ifdef ZTS
static ts_rsrc_id win32_globals_id;
#else
static tsrm_win32_globals win32_globals;
#endif

static void tsrm_win32_ctor(tsrm_win32_globals *globals)
{
	globals->process = NULL;
	globals->process_size = 0;
}

static void tsrm_win32_dtor(tsrm_win32_globals *globals)
{
	if (globals->process != NULL) {
		free(globals->process);
	}
}

TSRM_API void tsrm_win32_startup(void)
{
#ifdef ZTS
	win32_globals_id = ts_allocate_id(sizeof(tsrm_win32_globals), (ts_allocate_ctor)tsrm_win32_ctor, (ts_allocate_ctor)tsrm_win32_dtor);
#else
	tsrm_win32_ctor(&win32_globals);
#endif
}

TSRM_API void tsrm_win32_shutdown(void)
{
#ifndef ZTS
	tsrm_win32_dtor(&win32_globals);
#endif
}

static ProcessPair* process_get(FILE *stream)
{
	ProcessPair* ptr;
	ProcessPair* newptr;
	TWLS_FETCH();
	
	for (ptr = TWG(process); ptr < (TWG(process) + TWG(process_size)); ptr++) {
		if (stream != NULL && ptr->stream == stream) {
			break;
		} else if (stream == NULL && !ptr->inuse) {
			break;
		}
	}
	
	if (ptr < (TWG(process) + TWG(process_size))) {
		return ptr;
	}
	
	newptr = (ProcessPair*)realloc((void*)TWG(process), (TWG(process_size)+1)*sizeof(ProcessPair));
	if (newptr == NULL) {
		return NULL;
	}
	
	TWG(process) = newptr;
	ptr = newptr + TWG(process_size);
	TWG(process_size)++;
	return ptr;
}

TSRM_API FILE* popen(const char *command, const char *type)
{
	FILE *stream = NULL;
	int fno, str_len = strlen(type), read, mode;
	STARTUPINFO startup;
	PROCESS_INFORMATION process;
	SECURITY_ATTRIBUTES security;
	HANDLE in, out;
	ProcessPair *proc;

	security.nLength				= sizeof(SECURITY_ATTRIBUTES);
	security.bInheritHandle			= TRUE;
	security.lpSecurityDescriptor	= NULL;

	if (!str_len || !CreatePipe(&in, &out, &security, 2048L)) {
		return NULL;
	}
	
	memset(&startup, 0, sizeof(STARTUPINFO));
	memset(&process, 0, sizeof(PROCESS_INFORMATION));

	startup.cb			= sizeof(STARTUPINFO);
	startup.dwFlags		= STARTF_USESTDHANDLES;
	startup.hStdError	= GetStdHandle(STD_ERROR_HANDLE);

	read = (type[0] == 'r') ? TRUE : FALSE;
	mode = ((str_len == 2) && (type[1] == 'b')) ? O_BINARY : O_TEXT;


	if (read) {
		startup.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
		startup.hStdOutput = out;
	} else {
		startup.hStdInput  = in;
		startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	}


	if (!CreateProcess(NULL, (LPTSTR)command, &security, &security, security.bInheritHandle, NORMAL_PRIORITY_CLASS, NULL, NULL, &startup, &process)) {
		return NULL;
	}

	CloseHandle(process.hThread);
	proc = process_get(NULL);

	if (read) {
		fno = _open_osfhandle((long)in, _O_RDONLY | mode);
		CloseHandle(out);
	} else {
		fno = _open_osfhandle((long)out, _O_WRONLY | mode);
		CloseHandle(in);
	}

	stream = _fdopen(fno, type);
	proc->prochnd = process.hProcess;
	proc->stream = stream;
	proc->inuse = 1;
	return stream;
}

TSRM_API int pclose(FILE* stream)
{
	DWORD termstat = 0;
	ProcessPair* process;

	if ((process = process_get(stream)) == NULL) {
		return 0;
	}

	fflush(process->stream);
    fclose(process->stream);

	GetExitCodeProcess(process->prochnd, &termstat);
	if (termstat == STILL_ACTIVE) {
		TerminateProcess(process->prochnd, termstat);
	}

	process->stream = NULL;
	process->inuse = 0;
	CloseHandle(process->prochnd);

	return termstat;
}

#endif