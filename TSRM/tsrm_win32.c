/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
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

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <time.h>

#include "TSRM.h"

#ifdef TSRM_WIN32
#include "tsrm_win32.h"

#ifdef ZTS
static ts_rsrc_id win32_globals_id;
#else
static tsrm_win32_globals win32_globals;
#endif

static void tsrm_win32_ctor(tsrm_win32_globals *globals TSRMLS_DC)
{
	globals->process = NULL;
	globals->shm	 = NULL;
	globals->process_size = 0;
	globals->shm_size	  = 0;
	globals->comspec = _strdup((GetVersion()<0x80000000)?"cmd.exe":"command.com");
}

static void tsrm_win32_dtor(tsrm_win32_globals *globals TSRMLS_DC)
{
	shm_pair *ptr;

	if (globals->process) {
		free(globals->process);
	}

	if (globals->shm) {
		for (ptr = globals->shm; ptr < (globals->shm + globals->shm_size); ptr++) {
			UnmapViewOfFile(ptr->addr);
			CloseHandle(ptr->segment);
			UnmapViewOfFile(ptr->descriptor);
			CloseHandle(ptr->info);
		}
		free(globals->shm);
	}

	free(globals->comspec);
}

TSRM_API void tsrm_win32_startup(void)
{
#ifdef ZTS
	ts_allocate_id(&win32_globals_id, sizeof(tsrm_win32_globals), (ts_allocate_ctor)tsrm_win32_ctor, (ts_allocate_ctor)tsrm_win32_dtor);
#else
	tsrm_win32_ctor(&win32_globals TSRMLS_CC);
#endif
}

TSRM_API void tsrm_win32_shutdown(void)
{
#ifndef ZTS
	tsrm_win32_dtor(&win32_globals TSRMLS_CC);
#endif
}

static process_pair *process_get(FILE *stream TSRMLS_DC)
{
	process_pair *ptr;
	process_pair *newptr;
	
	for (ptr = TWG(process); ptr < (TWG(process) + TWG(process_size)); ptr++) {
		if (ptr->stream == stream) {
			break;
		}
	}
	
	if (ptr < (TWG(process) + TWG(process_size))) {
		return ptr;
	}
	
	newptr = (process_pair*)realloc((void*)TWG(process), (TWG(process_size)+1)*sizeof(process_pair));
	if (newptr == NULL) {
		return NULL;
	}
	
	TWG(process) = newptr;
	ptr = newptr + TWG(process_size);
	TWG(process_size)++;
	return ptr;
}

static shm_pair *shm_get(int key, void *addr)
{
	shm_pair *ptr;
	shm_pair *newptr;
	TSRMLS_FETCH();
	
	for (ptr = TWG(shm); ptr < (TWG(shm) + TWG(shm_size)); ptr++) {
		if (!ptr->descriptor) {
			continue;
		}
		if (!addr && ptr->descriptor->shm_perm.key == key) {
			break;
		} else if (ptr->addr == addr) {
			break;
		}
	}

	if (ptr < (TWG(shm) + TWG(shm_size))) {
		return ptr;
	}
	
	newptr = (shm_pair*)realloc((void*)TWG(shm), (TWG(shm_size)+1)*sizeof(shm_pair));
	if (newptr == NULL) {
		return NULL;
	}
	
	TWG(shm) = newptr;
	ptr = newptr + TWG(shm_size);
	TWG(shm_size)++;
	return ptr;
}

static HANDLE dupHandle(HANDLE fh, BOOL inherit) {
	HANDLE copy, self = GetCurrentProcess();
	if (!DuplicateHandle(self, fh, self, &copy, 0, inherit, DUPLICATE_SAME_ACCESS|DUPLICATE_CLOSE_SOURCE)) {
		return NULL;
	}
	return copy;
}

TSRM_API FILE *popen(const char *command, const char *type)
{
	FILE *stream = NULL;
	int fno, str_len = strlen(type), read, mode;
	STARTUPINFO startup;
	PROCESS_INFORMATION process;
	SECURITY_ATTRIBUTES security;
	HANDLE in, out;
	char *cmd;
	process_pair *proc;
	TSRMLS_FETCH();

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
		in = dupHandle(in, FALSE);
		startup.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
		startup.hStdOutput = out;
	} else {
		out = dupHandle(out, FALSE);
		startup.hStdInput  = in;
		startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	cmd = (char*)malloc(strlen(command)+strlen(TWG(comspec))+sizeof(" /c "));
	sprintf(cmd, "%s /c %s", TWG(comspec), command);
	if (!CreateProcess(NULL, cmd, &security, &security, security.bInheritHandle, NORMAL_PRIORITY_CLASS, NULL, NULL, &startup, &process)) {
		return NULL;
	}
	free(cmd);

	CloseHandle(process.hThread);
	proc = process_get(NULL TSRMLS_CC);

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
	return stream;
}

TSRM_API int pclose(FILE *stream)
{
	DWORD termstat = 0;
	process_pair *process;
	TSRMLS_FETCH();

	if ((process = process_get(stream TSRMLS_CC)) == NULL) {
		return 0;
	}

	fflush(process->stream);
    fclose(process->stream);

	WaitForSingleObject(process->prochnd, INFINITE);
	GetExitCodeProcess(process->prochnd, &termstat);
	process->stream = NULL;
	CloseHandle(process->prochnd);

	return termstat;
}

TSRM_API int shmget(int key, int size, int flags)
{
	shm_pair *shm;
	char shm_segment[26], shm_info[29];
	HANDLE shm_handle, info_handle;
	BOOL created = FALSE;

	if (size < 0) {
		return -1;
	}

	sprintf(shm_segment, "TSRM_SHM_SEGMENT:%d", key);
	sprintf(shm_info, "TSRM_SHM_DESCRIPTOR:%d", key);

	shm_handle  = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, shm_segment);
	info_handle = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, shm_info);

	if ((!shm_handle && !info_handle)) {
		if (flags & IPC_EXCL) {
			return -1;
		}
		if (flags & IPC_CREAT) {
			shm_handle	= CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, shm_segment);
			info_handle	= CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(shm->descriptor), shm_info);
			created		= TRUE;
		}
		if ((!shm_handle || !info_handle)) {
			return -1;
		}
	}

	shm = shm_get(key, NULL);
	shm->segment = shm_handle;
	shm->info	 = info_handle;
	shm->descriptor = MapViewOfFileEx(shm->info, FILE_MAP_ALL_ACCESS, 0, 0, 0, NULL);

	if (created) {
		shm->descriptor->shm_perm.key	= key;
		shm->descriptor->shm_segsz		= size;
		shm->descriptor->shm_ctime		= time(NULL);
		shm->descriptor->shm_cpid		= getpid();
		shm->descriptor->shm_perm.mode	= flags;

		shm->descriptor->shm_perm.cuid	= shm->descriptor->shm_perm.cgid= 0;
		shm->descriptor->shm_perm.gid	= shm->descriptor->shm_perm.uid = 0;
		shm->descriptor->shm_atime		= shm->descriptor->shm_dtime	= 0;
		shm->descriptor->shm_lpid		= shm->descriptor->shm_nattch	= 0;
		shm->descriptor->shm_perm.mode	= shm->descriptor->shm_perm.seq	= 0;
	}

	if (shm->descriptor->shm_perm.key != key || size > shm->descriptor->shm_segsz ) {
		CloseHandle(shm->segment);
		UnmapViewOfFile(shm->descriptor);
		CloseHandle(shm->info);
		return -1;
	}

	return key;
}

TSRM_API void *shmat(int key, const void *shmaddr, int flags)
{
	shm_pair *shm = shm_get(key, NULL);

	if (!shm->segment) {
		return (void*)-1;
	}

	shm->descriptor->shm_atime = time(NULL);
	shm->descriptor->shm_lpid  = getpid();
	shm->descriptor->shm_nattch++;

	shm->addr = MapViewOfFileEx(shm->segment, FILE_MAP_ALL_ACCESS, 0, 0, 0, NULL);

	return shm->addr;
}

TSRM_API int shmdt(const void *shmaddr)
{
	shm_pair *shm = shm_get(0, (void*)shmaddr);

	if (!shm->segment) {
		return -1;
	}

	shm->descriptor->shm_dtime = time(NULL);
	shm->descriptor->shm_lpid  = getpid();
	shm->descriptor->shm_nattch--;

	return UnmapViewOfFile(shm->addr) ? 0 : -1;
}

TSRM_API int shmctl(int key, int cmd, struct shmid_ds *buf) {
	shm_pair *shm = shm_get(key, NULL);

	if (!shm->segment) {
		return -1;
	}

	switch (cmd) {
		case IPC_STAT:
			memcpy(buf, shm->descriptor, sizeof(struct shmid_ds));
			return 0;

		case IPC_SET:
			shm->descriptor->shm_ctime		= time(NULL);
			shm->descriptor->shm_perm.uid	= buf->shm_perm.uid;
			shm->descriptor->shm_perm.gid	= buf->shm_perm.gid;
			shm->descriptor->shm_perm.mode	= buf->shm_perm.mode;
			return 0;

		case IPC_RMID:
			if (shm->descriptor->shm_nattch < 1) {
				shm->descriptor->shm_perm.key = -1;
			}
			return 0;

		default:
			return -1;
	}
}
#endif