/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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
#include <errno.h>

#define TSRM_INCLUDE_FULL_WINDOWS_HEADERS
#include "SAPI.h"
#include "TSRM.h"

#ifdef TSRM_WIN32
#include <Sddl.h>
#include "tsrm_win32.h"
#include "zend_virtual_cwd.h"

#ifdef ZTS
static ts_rsrc_id win32_globals_id;
#else
static tsrm_win32_globals win32_globals;
#endif

static void tsrm_win32_ctor(tsrm_win32_globals *globals)
{
#ifdef ZTS
TSRMLS_CACHE_UPDATE();
#endif
	globals->process = NULL;
	globals->shm	 = NULL;
	globals->process_size = 0;
	globals->shm_size	  = 0;
	globals->comspec = _strdup("cmd.exe");

	/* Set it to INVALID_HANDLE_VALUE
	 * It will be initialized correctly in tsrm_win32_access or set to
	 * NULL if no impersonation has been done.
	 * the impersonated token can't be set here as the impersonation
	 * will happen later, in fcgi_accept_request (or whatever is the
	 * SAPI being used).
	 */
	globals->impersonation_token = INVALID_HANDLE_VALUE;
	globals->impersonation_token_sid = NULL;
}

static void tsrm_win32_dtor(tsrm_win32_globals *globals)
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

	if (globals->impersonation_token && globals->impersonation_token != INVALID_HANDLE_VALUE	) {
		CloseHandle(globals->impersonation_token);
	}
	if (globals->impersonation_token_sid) {
		free(globals->impersonation_token_sid);
	}
}

TSRM_API void tsrm_win32_startup(void)
{
#ifdef ZTS
	ts_allocate_id(&win32_globals_id, sizeof(tsrm_win32_globals), (ts_allocate_ctor)tsrm_win32_ctor, (ts_allocate_ctor)tsrm_win32_dtor);
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

char * tsrm_win32_get_path_sid_key(const char *pathname)
{
	PSID pSid = TWG(impersonation_token_sid);
	TCHAR *ptcSid = NULL;
	char *bucket_key = NULL;
	size_t ptc_sid_len, pathname_len;

	pathname_len = strlen(pathname);

	if (!pSid) {
		bucket_key = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pathname_len + 1);
		if (!bucket_key) {
			return NULL;
		}
		memcpy(bucket_key, pathname, pathname_len);
		return bucket_key;
	}

	if (!ConvertSidToStringSid(pSid, &ptcSid)) {
		return NULL;
	}


	ptc_sid_len = strlen(ptcSid);
	bucket_key = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pathname_len + ptc_sid_len + 1);
	if (!bucket_key) {
		LocalFree(ptcSid);
		return NULL;
	}

	memcpy(bucket_key, ptcSid, ptc_sid_len);
	memcpy(bucket_key + ptc_sid_len, pathname, pathname_len + 1);

	LocalFree(ptcSid);
	return bucket_key;
}


PSID tsrm_win32_get_token_sid(HANDLE hToken)
{
	DWORD dwLength = 0;
	PTOKEN_USER pTokenUser = NULL;
	DWORD sid_len;
	PSID pResultSid = NULL;

	/* Get the actual size of the TokenUser structure */
	if (!GetTokenInformation(
			hToken, TokenUser, (LPVOID) pTokenUser, 0, &dwLength))  {
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
			goto Finished;
		}

		pTokenUser = (PTOKEN_USER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwLength);
		if (pTokenUser == NULL) {
			goto Finished;
		}
	}

	/* and fetch it now */
	if (!GetTokenInformation(
		hToken, TokenUser, (LPVOID) pTokenUser, dwLength, &dwLength)) {
		goto Finished;
	}

	sid_len = GetLengthSid(pTokenUser->User.Sid);

	/* ConvertSidToStringSid(pTokenUser->User.Sid, &ptcSidOwner); */
	pResultSid = malloc(sid_len);
	if (!pResultSid) {
		goto Finished;
	}
	if (!CopySid(sid_len, pResultSid, pTokenUser->User.Sid)) {
		goto Finished;
	}
	HeapFree(GetProcessHeap(), 0, (LPVOID)pTokenUser);
	return pResultSid;

Finished:
	if (pResultSid) {
		free(pResultSid);
	}
	/* Free the buffer for the token groups. */
	if (pTokenUser != NULL) {
		HeapFree(GetProcessHeap(), 0, (LPVOID)pTokenUser);
	}
	return NULL;
}

TSRM_API int tsrm_win32_access(const char *pathname, int mode)
{
	time_t t;
	HANDLE thread_token = NULL;
	PSID token_sid;
	SECURITY_INFORMATION sec_info = OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;
	GENERIC_MAPPING gen_map = { FILE_GENERIC_READ, FILE_GENERIC_WRITE, FILE_GENERIC_EXECUTE, FILE_ALL_ACCESS };
	DWORD priv_set_length = sizeof(PRIVILEGE_SET);

	PRIVILEGE_SET privilege_set = {0};
	DWORD sec_desc_length = 0, desired_access = 0, granted_access = 0;
	BYTE * psec_desc = NULL;
	BOOL fAccess = FALSE;

	realpath_cache_bucket * bucket = NULL;
	char * real_path = NULL;

	if (mode == 1 /*X_OK*/) {
		DWORD type;
		return GetBinaryType(pathname, &type) ? 0 : -1;
	} else {
		if(!IS_ABSOLUTE_PATH(pathname, strlen(pathname)+1)) {
			real_path = (char *)malloc(MAX_PATH);
			if(tsrm_realpath(pathname, real_path) == NULL) {
				goto Finished;
			}
			pathname = real_path;
 		}

		if(access(pathname, mode)) {
			free(real_path);
			return errno;
		}

 		/* If only existence check is made, return now */
 		if (mode == 0) {
			free(real_path);
			return 0;
		}

/* Only in NTS when impersonate==1 (aka FastCGI) */

		/*
		 AccessCheck() requires an impersonation token.  We first get a primary
		 token and then create a duplicate impersonation token.  The
		 impersonation token is not actually assigned to the thread, but is
		 used in the call to AccessCheck.  Thus, this function itself never
		 impersonates, but does use the identity of the thread.  If the thread
		 was impersonating already, this function uses that impersonation context.
		*/
		if(!OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, TRUE, &thread_token)) {
			if (GetLastError() == ERROR_NO_TOKEN) {
				if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &thread_token)) {
					 TWG(impersonation_token) = NULL;
					 goto Finished;
				 }
			}
		}

		/* token_sid will be freed in tsrmwin32_dtor */
		token_sid = tsrm_win32_get_token_sid(thread_token);
		if (!token_sid) {
			if (TWG(impersonation_token_sid)) {
				free(TWG(impersonation_token_sid));
			}
			TWG(impersonation_token_sid) = NULL;
			goto Finished;
		}

		/* Different identity, we need a new impersontated token as well */
		if (!TWG(impersonation_token_sid) || !EqualSid(token_sid, TWG(impersonation_token_sid))) {
			if (TWG(impersonation_token_sid)) {
				free(TWG(impersonation_token_sid));
			}
			TWG(impersonation_token_sid) = token_sid;

			/* Duplicate the token as impersonated token */
			if (!DuplicateToken(thread_token, SecurityImpersonation, &TWG(impersonation_token))) {
				goto Finished;
			}
		} else {
			/* we already have it, free it then */
			free(token_sid);
		}

		if (CWDG(realpath_cache_size_limit)) {
			t = time(0);
			bucket = realpath_cache_lookup(pathname, (int)strlen(pathname), t);
			if(bucket == NULL && real_path == NULL) {
				/* We used the pathname directly. Call tsrm_realpath */
				/* so that entry is created in realpath cache */
				real_path = (char *)malloc(MAX_PATH);
				if(tsrm_realpath(pathname, real_path) != NULL) {
					pathname = real_path;
					bucket = realpath_cache_lookup(pathname, (int)strlen(pathname), t);
				}
			}
 		}

 		/* Do a full access check because access() will only check read-only attribute */
 		if(mode == 0 || mode > 6) {
			if(bucket != NULL && bucket->is_rvalid) {
				fAccess = bucket->is_readable;
				goto Finished;
			}
 			desired_access = FILE_GENERIC_READ;
 		} else if(mode <= 2) {
			if(bucket != NULL && bucket->is_wvalid) {
				fAccess = bucket->is_writable;
				goto Finished;
			}
			desired_access = FILE_GENERIC_WRITE;
 		} else if(mode <= 4) {
			if(bucket != NULL && bucket->is_rvalid) {
				fAccess = bucket->is_readable;
				goto Finished;
			}
			desired_access = FILE_GENERIC_READ|FILE_FLAG_BACKUP_SEMANTICS;
 		} else { // if(mode <= 6)
			if(bucket != NULL && bucket->is_rvalid && bucket->is_wvalid) {
				fAccess = bucket->is_readable & bucket->is_writable;
				goto Finished;
			}
			desired_access = FILE_GENERIC_READ | FILE_GENERIC_WRITE;
 		}

		if(TWG(impersonation_token) == NULL) {
			goto Finished;
		}

		/* Get size of security buffer. Call is expected to fail */
		if(GetFileSecurity(pathname, sec_info, NULL, 0, &sec_desc_length)) {
			goto Finished;
		}

		psec_desc = (BYTE *)malloc(sec_desc_length);
		if(psec_desc == NULL ||
			 !GetFileSecurity(pathname, sec_info, (PSECURITY_DESCRIPTOR)psec_desc, sec_desc_length, &sec_desc_length)) {
			goto Finished;
		}

		MapGenericMask(&desired_access, &gen_map);

		if(!AccessCheck((PSECURITY_DESCRIPTOR)psec_desc, TWG(impersonation_token), desired_access, &gen_map, &privilege_set, &priv_set_length, &granted_access, &fAccess)) {
			goto Finished_Impersonate;
		}

		/* Keep the result in realpath_cache */
		if(bucket != NULL) {
			if(desired_access == (FILE_GENERIC_READ|FILE_FLAG_BACKUP_SEMANTICS)) {
				bucket->is_rvalid = 1;
				bucket->is_readable = fAccess;
			}
			else if(desired_access == FILE_GENERIC_WRITE) {
				bucket->is_wvalid = 1;
				bucket->is_writable = fAccess;
			} else if (desired_access == (FILE_GENERIC_READ | FILE_GENERIC_WRITE)) {
				bucket->is_rvalid = 1;
				bucket->is_readable = fAccess;
				bucket->is_wvalid = 1;
				bucket->is_writable = fAccess;
			}
		}

Finished_Impersonate:
		if(psec_desc != NULL) {
			free(psec_desc);
			psec_desc = NULL;
		}

Finished:
		if(thread_token != NULL) {
			CloseHandle(thread_token);
		}
		if(real_path != NULL) {
			free(real_path);
			real_path = NULL;
		}

		if(fAccess == FALSE) {
			errno = EACCES;
			return errno;
		} else {
			return 0;
		}
	}
}


static process_pair *process_get(FILE *stream)
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
	memset(ptr, 0, sizeof(*ptr));
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

	return popen_ex(command, type, NULL, NULL);
}

TSRM_API FILE *popen_ex(const char *command, const char *type, const char *cwd, char *env)
{
	FILE *stream = NULL;
	int fno, type_len, read, mode;
	STARTUPINFO startup;
	PROCESS_INFORMATION process;
	SECURITY_ATTRIBUTES security;
	HANDLE in, out;
	DWORD dwCreateFlags = 0;
	BOOL res;
	process_pair *proc;
	char *cmd;
	int i;
	char *ptype = (char *)type;
	HANDLE thread_token = NULL;
	HANDLE token_user = NULL;
	BOOL asuser = TRUE;

	if (!type) {
		return NULL;
	}

	/*The following two checks can be removed once we drop XP support */
	type_len = (int)strlen(type);
	if (type_len <1 || type_len > 2) {
		return NULL;
	}

	for (i=0; i < type_len; i++) {
		if (!(*ptype == 'r' || *ptype == 'w' || *ptype == 'b' || *ptype == 't')) {
			return NULL;
		}
		ptype++;
	}

	security.nLength				= sizeof(SECURITY_ATTRIBUTES);
	security.bInheritHandle			= TRUE;
	security.lpSecurityDescriptor	= NULL;

	if (!type_len || !CreatePipe(&in, &out, &security, 2048L)) {
		return NULL;
	}

	memset(&startup, 0, sizeof(STARTUPINFO));
	memset(&process, 0, sizeof(PROCESS_INFORMATION));

	startup.cb			= sizeof(STARTUPINFO);
	startup.dwFlags		= STARTF_USESTDHANDLES;
	startup.hStdError	= GetStdHandle(STD_ERROR_HANDLE);

	read = (type[0] == 'r') ? TRUE : FALSE;
	mode = ((type_len == 2) && (type[1] == 'b')) ? O_BINARY : O_TEXT;

	if (read) {
		in = dupHandle(in, FALSE);
		startup.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
		startup.hStdOutput = out;
	} else {
		out = dupHandle(out, FALSE);
		startup.hStdInput  = in;
		startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	dwCreateFlags = NORMAL_PRIORITY_CLASS;
	if (strcmp(sapi_module.name, "cli") != 0) {
		dwCreateFlags |= CREATE_NO_WINDOW;
	}

	/* Get a token with the impersonated user. */
	if(OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, TRUE, &thread_token)) {
		DuplicateTokenEx(thread_token, MAXIMUM_ALLOWED, &security, SecurityImpersonation, TokenPrimary, &token_user);
	} else {
		DWORD err = GetLastError();
		if (err == ERROR_NO_TOKEN) {
			asuser = FALSE;
		}
	}

	cmd = (char*)malloc(strlen(command)+strlen(TWG(comspec))+sizeof(" /c ")+2);
	if (!cmd) {
		return NULL;
	}

	sprintf(cmd, "%s /c \"%s\"", TWG(comspec), command);
	if (asuser) {
		res = CreateProcessAsUser(token_user, NULL, cmd, &security, &security, security.bInheritHandle, dwCreateFlags, env, cwd, &startup, &process);
		CloseHandle(token_user);
	} else {
		res = CreateProcess(NULL, cmd, &security, &security, security.bInheritHandle, dwCreateFlags, env, cwd, &startup, &process);
	}
	free(cmd);

	if (!res) {
		return NULL;
	}

	CloseHandle(process.hThread);
	proc = process_get(NULL);

	if (read) {
		fno = _open_osfhandle((tsrm_intptr_t)in, _O_RDONLY | mode);
		CloseHandle(out);
	} else {
		fno = _open_osfhandle((tsrm_intptr_t)out, _O_WRONLY | mode);
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

	if ((process = process_get(stream)) == NULL) {
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

	snprintf(shm_segment, sizeof(shm_segment)-1, "TSRM_SHM_SEGMENT:%d", key);
	snprintf(shm_info, sizeof(shm_info)-1, "TSRM_SHM_DESCRIPTOR:%d", key);
	shm_segment[sizeof(shm_segment)-1] = '\0';
	shm_info[sizeof(shm_info)-1] = '\0';

	shm_handle  = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, shm_segment);
	info_handle = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, shm_info);

	if (!shm_handle && !info_handle) {
		if (flags & IPC_CREAT) {
			shm_handle	= CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, shm_segment);
			info_handle	= CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(shm->descriptor), shm_info);
			created		= TRUE;
		}
		if (!shm_handle || !info_handle) {
			if (shm_handle) {
				CloseHandle(shm_handle);
			}
			if (info_handle) {
				CloseHandle(info_handle);
			}
			return -1;
		}
	} else {
		if (flags & IPC_EXCL) {
			return -1;
		}
	}

	shm = shm_get(key, NULL);
	if (!shm) {
		CloseHandle(shm_handle);
		CloseHandle(info_handle);
		return -1;
	}
	shm->segment = shm_handle;
	shm->info	 = info_handle;
	shm->descriptor = MapViewOfFileEx(shm->info, FILE_MAP_ALL_ACCESS, 0, 0, 0, NULL);

	if (NULL != shm->descriptor && created) {
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

	if (NULL != shm->descriptor && (shm->descriptor->shm_perm.key != key || size > shm->descriptor->shm_segsz)) {
		if (NULL != shm->segment) {
			CloseHandle(shm->segment);
		}
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

TSRM_API char *realpath(char *orig_path, char *buffer)
{
	int ret = GetFullPathName(orig_path, _MAX_PATH, buffer, NULL);
	if(!ret || ret > _MAX_PATH) {
		return NULL;
	}
	return buffer;
}

#if HAVE_UTIME
static zend_always_inline void UnixTimeToFileTime(time_t t, LPFILETIME pft) /* {{{ */
{
	// Note that LONGLONG is a 64-bit value
	LONGLONG ll;

	ll = Int32x32To64(t, 10000000) + 116444736000000000;
	pft->dwLowDateTime = (DWORD)ll;
	pft->dwHighDateTime = ll >> 32;
}
/* }}} */

TSRM_API int win32_utime(const char *filename, struct utimbuf *buf) /* {{{ */
{
	FILETIME mtime, atime;
	HANDLE hFile;

	hFile = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL,
				 OPEN_ALWAYS, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	/* OPEN_ALWAYS mode sets the last error to ERROR_ALREADY_EXISTS but
	   the CreateFile operation succeeds */
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		SetLastError(0);
	}

	if ( hFile == INVALID_HANDLE_VALUE ) {
		return -1;
	}

	if (!buf) {
		SYSTEMTIME st;
		GetSystemTime(&st);
		SystemTimeToFileTime(&st, &mtime);
		atime = mtime;
	} else {
		UnixTimeToFileTime(buf->modtime, &mtime);
		UnixTimeToFileTime(buf->actime, &atime);
	}
	if (!SetFileTime(hFile, NULL, &atime, &mtime)) {
		CloseHandle(hFile);
		return -1;
	}
	CloseHandle(hFile);
	return 1;
}
/* }}} */
#endif
#endif
