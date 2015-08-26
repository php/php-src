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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Sascha Schumann <sascha@schumann.cx>                        |
   |          Pierre Joye <pierre@php.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>

#include "zend.h"
#include "zend_virtual_cwd.h"
#include "tsrm_strtok_r.h"

#ifdef ZEND_WIN32
#include <io.h>
#include "tsrm_win32.h"
# ifndef IO_REPARSE_TAG_SYMLINK
#  define IO_REPARSE_TAG_SYMLINK 0xA000000C
# endif

# ifndef IO_REPARSE_TAG_DEDUP
#  define IO_REPARSE_TAG_DEDUP   0x80000013
# endif

# ifndef VOLUME_NAME_NT
#  define VOLUME_NAME_NT 0x2
# endif

# ifndef VOLUME_NAME_DOS
#  define VOLUME_NAME_DOS 0x0
# endif
#endif

#ifdef NETWARE
#include <fsio.h>
#endif

#ifndef HAVE_REALPATH
#define realpath(x,y) strcpy(y,x)
#endif

#define VIRTUAL_CWD_DEBUG 0

#include "TSRM.h"

/* Only need mutex for popen() in Windows and NetWare because it doesn't chdir() on UNIX */
#if (defined(ZEND_WIN32) || defined(NETWARE)) && defined(ZTS)
MUTEX_T cwd_mutex;
#endif

#ifdef ZTS
ts_rsrc_id cwd_globals_id;
#else
virtual_cwd_globals cwd_globals;
#endif

cwd_state main_cwd_state; /* True global */

#ifndef ZEND_WIN32
#include <unistd.h>
#else
#include <direct.h>
#endif

#ifdef ZEND_WIN32
#include <tchar.h>
#define tsrm_strtok_r(a,b,c) _tcstok((a),(b))
#define TOKENIZER_STRING "/\\"

static int php_check_dots(const char *element, int n)
{
	while (n-- > 0) if (element[n] != '.') break;

	return (n != -1);
}

#define IS_DIRECTORY_UP(element, len) \
	(len >= 2 && !php_check_dots(element, len))

#define IS_DIRECTORY_CURRENT(element, len) \
	(len == 1 && element[0] == '.')

#elif defined(NETWARE)
/* NetWare has strtok() (in LibC) and allows both slashes in paths, like Windows --
   but rest of the stuff is like Unix */
/* strtok() call in LibC is abending when used in a different address space -- hence using
   PHP's version itself for now */
/*#define tsrm_strtok_r(a,b,c) strtok((a),(b))*/
#define TOKENIZER_STRING "/\\"

#else
#define TOKENIZER_STRING "/"
#endif

/* default macros */

#ifndef IS_DIRECTORY_UP
#define IS_DIRECTORY_UP(element, len) \
	(len == 2 && element[0] == '.' && element[1] == '.')
#endif

#ifndef IS_DIRECTORY_CURRENT
#define IS_DIRECTORY_CURRENT(element, len) \
	(len == 1 && element[0] == '.')
#endif

/* define this to check semantics */
#define IS_DIR_OK(s) (1)

#ifndef IS_DIR_OK
#define IS_DIR_OK(state) (php_is_dir_ok(state) == 0)
#endif


#define CWD_STATE_COPY(d, s)				\
	(d)->cwd_length = (s)->cwd_length;		\
	(d)->cwd = (char *) emalloc((s)->cwd_length+1);	\
	memcpy((d)->cwd, (s)->cwd, (s)->cwd_length+1);

#define CWD_STATE_FREE(s)			\
	efree((s)->cwd);

#ifdef ZEND_WIN32
# define CWD_STATE_FREE_ERR(state) do { \
		DWORD last_error = GetLastError(); \
		CWD_STATE_FREE(state); \
		SetLastError(last_error); \
	} while (0)
#else
# define CWD_STATE_FREE_ERR(state) CWD_STATE_FREE(state)
#endif

#ifdef ZEND_WIN32

#ifdef CTL_CODE
#undef CTL_CODE
#endif
#define CTL_CODE(DeviceType,Function,Method,Access) (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
#define FILE_DEVICE_FILE_SYSTEM 0x00000009
#define METHOD_BUFFERED		0
#define FILE_ANY_ACCESS 	0
#define FSCTL_GET_REPARSE_POINT CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 42, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define MAXIMUM_REPARSE_DATA_BUFFER_SIZE  ( 16 * 1024 )

typedef struct {
	unsigned long  ReparseTag;
	unsigned short ReparseDataLength;
	unsigned short Reserved;
	union {
		struct {
			unsigned short SubstituteNameOffset;
			unsigned short SubstituteNameLength;
			unsigned short PrintNameOffset;
			unsigned short PrintNameLength;
			unsigned long  Flags;
			wchar_t        ReparseTarget[1];
		} SymbolicLinkReparseBuffer;
		struct {
			unsigned short SubstituteNameOffset;
			unsigned short SubstituteNameLength;
			unsigned short PrintNameOffset;
			unsigned short PrintNameLength;
			wchar_t        ReparseTarget[1];
		} MountPointReparseBuffer;
		struct {
			unsigned char  ReparseTarget[1];
		} GenericReparseBuffer;
	};
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;

#define SECS_BETWEEN_EPOCHS (__int64)11644473600
#define SECS_TO_100NS (__int64)10000000
static inline time_t FileTimeToUnixTime(const FILETIME *FileTime)
{
	__int64 UnixTime;
	long *nsec = NULL;
	SYSTEMTIME SystemTime;
	FileTimeToSystemTime(FileTime, &SystemTime);

	UnixTime = ((__int64)FileTime->dwHighDateTime << 32) +
	FileTime->dwLowDateTime;

	UnixTime -= (SECS_BETWEEN_EPOCHS * SECS_TO_100NS);

	if (nsec) {
		*nsec = (UnixTime % SECS_TO_100NS) * (__int64)100;
	}

	UnixTime /= SECS_TO_100NS; /* now convert to seconds */

	if ((time_t)UnixTime != UnixTime) {
		UnixTime = 0;
	}
	return (time_t)UnixTime;
}

CWD_API int php_sys_readlink(const char *link, char *target, size_t target_len){ /* {{{ */
	HINSTANCE kernel32;
	HANDLE hFile;
	DWORD dwRet;

	typedef BOOL (WINAPI *gfpnh_func)(HANDLE, LPTSTR, DWORD, DWORD);
	gfpnh_func pGetFinalPathNameByHandle;

	if (!target_len) {
		return -1;
	}

	kernel32 = LoadLibrary("kernel32.dll");

	if (kernel32) {
		pGetFinalPathNameByHandle = (gfpnh_func)GetProcAddress(kernel32, "GetFinalPathNameByHandleA");
		if (pGetFinalPathNameByHandle == NULL) {
			return -1;
		}
	} else {
		return -1;
	}

	hFile = CreateFile(link,            // file to open
				 GENERIC_READ,          // open for reading
				 FILE_SHARE_READ,       // share for reading
				 NULL,                  // default security
				 OPEN_EXISTING,         // existing file only
				 FILE_FLAG_BACKUP_SEMANTICS, // normal file
				 NULL);                 // no attr. template

	if( hFile == INVALID_HANDLE_VALUE) {
			return -1;
	}

	/* Despite MSDN has documented it won't to, the length returned by
		GetFinalPathNameByHandleA includes the length of the
		null terminator. This behavior is at least reproducible
		with VS2012 and earlier, and seems not to be fixed till
		now. Thus, correcting target_len so it's suddenly don't
		overflown. */
	dwRet = pGetFinalPathNameByHandle(hFile, target, target_len - 1, VOLUME_NAME_DOS);
	if(dwRet >= target_len || dwRet >= MAXPATHLEN || dwRet == 0) {
		return -1;
	}

	CloseHandle(hFile);

	if(dwRet > 4) {
		/* Skip first 4 characters if they are "\??\" */
		if(target[0] == '\\' && target[1] == '\\' && target[2] == '?' && target[3] ==  '\\') {
			char tmp[MAXPATHLEN];
			unsigned int offset = 4;
			dwRet -= 4;

			/* \??\UNC\ */
			if (dwRet > 7 && target[4] == 'U' && target[5] == 'N' && target[6] == 'C') {
				offset += 2;
				dwRet -= 2;
				target[offset] = '\\';
			}

			memcpy(tmp, target + offset, dwRet);
			memcpy(target, tmp, dwRet);
		}
	}

	target[dwRet] = '\0';
	return dwRet;
}
/* }}} */

CWD_API int php_sys_stat_ex(const char *path, zend_stat_t *buf, int lstat) /* {{{ */
{
	WIN32_FILE_ATTRIBUTE_DATA data;
	LARGE_INTEGER t;
	const size_t path_len = strlen(path);
	ALLOCA_FLAG(use_heap_large);

	if (!GetFileAttributesEx(path, GetFileExInfoStandard, &data)) {
		return zend_stat(path, buf);
	}

	if (path_len >= 1 && path[1] == ':') {
		if (path[0] >= 'A' && path[0] <= 'Z') {
			buf->st_dev = buf->st_rdev = path[0] - 'A';
		} else {
			buf->st_dev = buf->st_rdev = path[0] - 'a';
		}
	} else if (IS_UNC_PATH(path, path_len)) {
		buf->st_dev = buf->st_rdev = 0;
	} else {
		char  cur_path[MAXPATHLEN+1];
		DWORD len = sizeof(cur_path);
		char *tmp = cur_path;

		while(1) {
			DWORD r = GetCurrentDirectory(len, tmp);
			if (r < len) {
				if (tmp[1] == ':') {
					if (path[0] >= 'A' && path[0] <= 'Z') {
						buf->st_dev = buf->st_rdev = path[0] - 'A';
					} else {
						buf->st_dev = buf->st_rdev = path[0] - 'a';
					}
				} else {
					buf->st_dev = buf->st_rdev = -1;
				}
				break;
			} else if (!r) {
				buf->st_dev = buf->st_rdev = -1;
				break;
			} else {
				len = r+1;
				tmp = (char*)malloc(len);
			}
		}
		if (tmp != cur_path) {
			free(tmp);
		}
	}

	buf->st_uid = buf->st_gid = buf->st_ino = 0;

	if (lstat && data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
		/* File is a reparse point. Get the target */
		HANDLE hLink = NULL;
		REPARSE_DATA_BUFFER * pbuffer;
		DWORD retlength = 0;

		hLink = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT|FILE_FLAG_BACKUP_SEMANTICS, NULL);
		if(hLink == INVALID_HANDLE_VALUE) {
			return -1;
		}

		pbuffer = (REPARSE_DATA_BUFFER *)do_alloca(MAXIMUM_REPARSE_DATA_BUFFER_SIZE, use_heap_large);
		if(!DeviceIoControl(hLink, FSCTL_GET_REPARSE_POINT, NULL, 0, pbuffer,  MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &retlength, NULL)) {
			free_alloca(pbuffer, use_heap_large);
			CloseHandle(hLink);
			return -1;
		}

		CloseHandle(hLink);

		if(pbuffer->ReparseTag == IO_REPARSE_TAG_SYMLINK) {
			buf->st_mode = S_IFLNK;
			buf->st_mode |= (data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? (S_IREAD|(S_IREAD>>3)|(S_IREAD>>6)) : (S_IREAD|(S_IREAD>>3)|(S_IREAD>>6)|S_IWRITE|(S_IWRITE>>3)|(S_IWRITE>>6));
		}

#if 0 /* Not used yet */
		else if(pbuffer->ReparseTag == IO_REPARSE_TAG_MOUNT_POINT) {
			buf->st_mode |=;
		}
#endif
		free_alloca(pbuffer, use_heap_large);
	} else {
		buf->st_mode = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? (S_IFDIR|S_IEXEC|(S_IEXEC>>3)|(S_IEXEC>>6)) : S_IFREG;
		buf->st_mode |= (data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? (S_IREAD|(S_IREAD>>3)|(S_IREAD>>6)) : (S_IREAD|(S_IREAD>>3)|(S_IREAD>>6)|S_IWRITE|(S_IWRITE>>3)|(S_IWRITE>>6));
	}

	if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
		size_t len = strlen(path);

		if (path[len-4] == '.') {
			if (_memicmp(path+len-3, "exe", 3) == 0 ||
				_memicmp(path+len-3, "com", 3) == 0 ||
				_memicmp(path+len-3, "bat", 3) == 0 ||
				_memicmp(path+len-3, "cmd", 3) == 0) {
				buf->st_mode  |= (S_IEXEC|(S_IEXEC>>3)|(S_IEXEC>>6));
			}
		}
	}

	buf->st_nlink = 1;
	t.HighPart = data.nFileSizeHigh;
	t.LowPart = data.nFileSizeLow;
	/* It's an overflow on 32 bit, however it won't fix as long
	as zend_long is 32 bit. */
	buf->st_size = (zend_long)t.QuadPart;
	buf->st_atime = FileTimeToUnixTime(&data.ftLastAccessTime);
	buf->st_ctime = FileTimeToUnixTime(&data.ftCreationTime);
	buf->st_mtime = FileTimeToUnixTime(&data.ftLastWriteTime);
	return 0;
}
/* }}} */
#endif

static int php_is_dir_ok(const cwd_state *state)  /* {{{ */
{
	zend_stat_t buf;

	if (php_sys_stat(state->cwd, &buf) == 0 && S_ISDIR(buf.st_mode))
		return (0);

	return (1);
}
/* }}} */

static int php_is_file_ok(const cwd_state *state)  /* {{{ */
{
	zend_stat_t buf;

	if (php_sys_stat(state->cwd, &buf) == 0 && S_ISREG(buf.st_mode))
		return (0);

	return (1);
}
/* }}} */

static void cwd_globals_ctor(virtual_cwd_globals *cwd_g) /* {{{ */
{
	CWD_STATE_COPY(&cwd_g->cwd, &main_cwd_state);
	cwd_g->realpath_cache_size = 0;
	cwd_g->realpath_cache_size_limit = REALPATH_CACHE_SIZE;
	cwd_g->realpath_cache_ttl = REALPATH_CACHE_TTL;
	memset(cwd_g->realpath_cache, 0, sizeof(cwd_g->realpath_cache));
}
/* }}} */

static void cwd_globals_dtor(virtual_cwd_globals *cwd_g) /* {{{ */
{
	realpath_cache_clean();
}
/* }}} */

CWD_API void virtual_cwd_startup(void) /* {{{ */
{
	char cwd[MAXPATHLEN];
	char *result;

#ifdef NETWARE
	result = getcwdpath(cwd, NULL, 1);
	if(result)
	{
		char *c=cwd;
		while(c = strchr(c, '\\'))
		{
			*c='/';
			++c;
		}
	}
#else
	result = getcwd(cwd, sizeof(cwd));
#endif
	if (!result) {
		cwd[0] = '\0';
	}

	main_cwd_state.cwd_length = (int)strlen(cwd);
#ifdef ZEND_WIN32
	if (main_cwd_state.cwd_length >= 2 && cwd[1] == ':') {
		cwd[0] = toupper(cwd[0]);
	}
#endif
	main_cwd_state.cwd = strdup(cwd);

#ifdef ZTS
	ts_allocate_id(&cwd_globals_id, sizeof(virtual_cwd_globals), (ts_allocate_ctor) cwd_globals_ctor, (ts_allocate_dtor) cwd_globals_dtor);
#else
	cwd_globals_ctor(&cwd_globals);
#endif

#if (defined(ZEND_WIN32) || defined(NETWARE)) && defined(ZTS)
	cwd_mutex = tsrm_mutex_alloc();
#endif
}
/* }}} */

CWD_API void virtual_cwd_shutdown(void) /* {{{ */
{
#ifndef ZTS
	cwd_globals_dtor(&cwd_globals);
#endif
#if (defined(ZEND_WIN32) || defined(NETWARE)) && defined(ZTS)
	tsrm_mutex_free(cwd_mutex);
#endif

	free(main_cwd_state.cwd); /* Don't use CWD_STATE_FREE because the non global states will probably use emalloc()/efree() */
}
/* }}} */

CWD_API int virtual_cwd_activate(void) /* {{{ */
{
	if (CWDG(cwd).cwd == NULL) {
		CWD_STATE_COPY(&CWDG(cwd), &main_cwd_state);
	}
	return 0;
}
/* }}} */

CWD_API int virtual_cwd_deactivate(void) /* {{{ */
{
	if (CWDG(cwd).cwd != NULL) {
		CWD_STATE_FREE(&CWDG(cwd));
		CWDG(cwd).cwd = NULL;
	}
	return 0;
}
/* }}} */

CWD_API char *virtual_getcwd_ex(size_t *length) /* {{{ */
{
	cwd_state *state;

	state = &CWDG(cwd);

	if (state->cwd_length == 0) {
		char *retval;

		*length = 1;
		retval = (char *) emalloc(2);
		if (retval == NULL) {
			return NULL;
		}
		retval[0] = DEFAULT_SLASH;
		retval[1] = '\0';
		return retval;
	}

#ifdef ZEND_WIN32
	/* If we have something like C: */
	if (state->cwd_length == 2 && state->cwd[state->cwd_length-1] == ':') {
		char *retval;

		*length = state->cwd_length+1;
		retval = (char *) emalloc(*length+1);
		if (retval == NULL) {
			return NULL;
		}
		memcpy(retval, state->cwd, *length);
		retval[0] = toupper(retval[0]);
		retval[*length-1] = DEFAULT_SLASH;
		retval[*length] = '\0';
		return retval;
	}
#endif
	if (!state->cwd) {
		*length = 0;
		return NULL;
	}

	*length = state->cwd_length;
	return estrdup(state->cwd);
}
/* }}} */

/* Same semantics as UNIX getcwd() */
CWD_API char *virtual_getcwd(char *buf, size_t size) /* {{{ */
{
	size_t length;
	char *cwd;

	cwd = virtual_getcwd_ex(&length);

	if (buf == NULL) {
		return cwd;
	}
	if (length > size-1) {
		efree(cwd);
		errno = ERANGE; /* Is this OK? */
		return NULL;
	}
	if (!cwd) {
		return NULL;
	}
	memcpy(buf, cwd, length+1);
	efree(cwd);
	return buf;
}
/* }}} */

#ifdef ZEND_WIN32
static inline zend_ulong realpath_cache_key(const char *path, int path_len) /* {{{ */
{
	register zend_ulong h;
	char *bucket_key_start = tsrm_win32_get_path_sid_key(path);
	char *bucket_key = (char *)bucket_key_start;
	const char *e = bucket_key + strlen(bucket_key);

	if (!bucket_key) {
		return 0;
	}

	for (h = Z_UL(2166136261); bucket_key < e;) {
		h *= Z_UL(16777619);
		h ^= *bucket_key++;
	}
	HeapFree(GetProcessHeap(), 0, (LPVOID)bucket_key_start);
	return h;
}
/* }}} */
#else
static inline zend_ulong realpath_cache_key(const char *path, int path_len) /* {{{ */
{
	register zend_ulong h;
	const char *e = path + path_len;

	for (h = Z_UL(2166136261); path < e;) {
		h *= Z_UL(16777619);
		h ^= *path++;
	}

	return h;
}
/* }}} */
#endif /* defined(ZEND_WIN32) */

CWD_API void realpath_cache_clean(void) /* {{{ */
{
	uint32_t i;

	for (i = 0; i < sizeof(CWDG(realpath_cache))/sizeof(CWDG(realpath_cache)[0]); i++) {
		realpath_cache_bucket *p = CWDG(realpath_cache)[i];
		while (p != NULL) {
			realpath_cache_bucket *r = p;
			p = p->next;
			free(r);
		}
		CWDG(realpath_cache)[i] = NULL;
	}
	CWDG(realpath_cache_size) = 0;
}
/* }}} */

CWD_API void realpath_cache_del(const char *path, int path_len) /* {{{ */
{
	zend_ulong key = realpath_cache_key(path, path_len);
	zend_ulong n = key % (sizeof(CWDG(realpath_cache)) / sizeof(CWDG(realpath_cache)[0]));
	realpath_cache_bucket **bucket = &CWDG(realpath_cache)[n];

	while (*bucket != NULL) {
		if (key == (*bucket)->key && path_len == (*bucket)->path_len &&
					memcmp(path, (*bucket)->path, path_len) == 0) {
			realpath_cache_bucket *r = *bucket;
			*bucket = (*bucket)->next;

			/* if the pointers match then only subtract the length of the path */
		   	if(r->path == r->realpath) {
				CWDG(realpath_cache_size) -= sizeof(realpath_cache_bucket) + r->path_len + 1;
			} else {
				CWDG(realpath_cache_size) -= sizeof(realpath_cache_bucket) + r->path_len + 1 + r->realpath_len + 1;
			}

			free(r);
			return;
		} else {
			bucket = &(*bucket)->next;
		}
	}
}
/* }}} */

static inline void realpath_cache_add(const char *path, int path_len, const char *realpath, int realpath_len, int is_dir, time_t t) /* {{{ */
{
	zend_long size = sizeof(realpath_cache_bucket) + path_len + 1;
	int same = 1;

	if (realpath_len != path_len ||
		memcmp(path, realpath, path_len) != 0) {
		size += realpath_len + 1;
		same = 0;
	}

	if (CWDG(realpath_cache_size) + size <= CWDG(realpath_cache_size_limit)) {
		realpath_cache_bucket *bucket = malloc(size);
		zend_ulong n;

		if (bucket == NULL) {
			return;
		}

		bucket->key = realpath_cache_key(path, path_len);
		bucket->path = (char*)bucket + sizeof(realpath_cache_bucket);
		memcpy(bucket->path, path, path_len+1);
		bucket->path_len = path_len;
		if (same) {
			bucket->realpath = bucket->path;
		} else {
			bucket->realpath = bucket->path + (path_len + 1);
			memcpy(bucket->realpath, realpath, realpath_len+1);
		}
		bucket->realpath_len = realpath_len;
		bucket->is_dir = is_dir;
#ifdef ZEND_WIN32
		bucket->is_rvalid   = 0;
		bucket->is_readable = 0;
		bucket->is_wvalid   = 0;
		bucket->is_writable = 0;
#endif
		bucket->expires = t + CWDG(realpath_cache_ttl);
		n = bucket->key % (sizeof(CWDG(realpath_cache)) / sizeof(CWDG(realpath_cache)[0]));
		bucket->next = CWDG(realpath_cache)[n];
		CWDG(realpath_cache)[n] = bucket;
		CWDG(realpath_cache_size) += size;
	}
}
/* }}} */

static inline realpath_cache_bucket* realpath_cache_find(const char *path, int path_len, time_t t) /* {{{ */
{
	zend_ulong key = realpath_cache_key(path, path_len);
	zend_ulong n = key % (sizeof(CWDG(realpath_cache)) / sizeof(CWDG(realpath_cache)[0]));
	realpath_cache_bucket **bucket = &CWDG(realpath_cache)[n];

	while (*bucket != NULL) {
		if (CWDG(realpath_cache_ttl) && (*bucket)->expires < t) {
			realpath_cache_bucket *r = *bucket;
			*bucket = (*bucket)->next;

			/* if the pointers match then only subtract the length of the path */
		   	if(r->path == r->realpath) {
				CWDG(realpath_cache_size) -= sizeof(realpath_cache_bucket) + r->path_len + 1;
			} else {
				CWDG(realpath_cache_size) -= sizeof(realpath_cache_bucket) + r->path_len + 1 + r->realpath_len + 1;
			}
			free(r);
		} else if (key == (*bucket)->key && path_len == (*bucket)->path_len &&
					memcmp(path, (*bucket)->path, path_len) == 0) {
			return *bucket;
		} else {
			bucket = &(*bucket)->next;
		}
	}
	return NULL;
}
/* }}} */

CWD_API realpath_cache_bucket* realpath_cache_lookup(const char *path, int path_len, time_t t) /* {{{ */
{
	return realpath_cache_find(path, path_len, t);
}
/* }}} */

CWD_API zend_long realpath_cache_size(void)
{
	return CWDG(realpath_cache_size);
}

CWD_API zend_long realpath_cache_max_buckets(void)
{
	return (sizeof(CWDG(realpath_cache)) / sizeof(CWDG(realpath_cache)[0]));
}

CWD_API realpath_cache_bucket** realpath_cache_get_buckets(void)
{
	return CWDG(realpath_cache);
}


#undef LINK_MAX
#define LINK_MAX 32

static int tsrm_realpath_r(char *path, int start, int len, int *ll, time_t *t, int use_realpath, int is_dir, int *link_is_dir) /* {{{ */
{
	int i, j, save;
	int directory = 0;
#ifdef ZEND_WIN32
	WIN32_FIND_DATA data;
	HANDLE hFind;
	ALLOCA_FLAG(use_heap_large)
#else
	zend_stat_t st;
#endif
	realpath_cache_bucket *bucket;
	char *tmp;
	ALLOCA_FLAG(use_heap)

	while (1) {
		if (len <= start) {
			if (link_is_dir) {
				*link_is_dir = 1;
			}
			return start;
		}

		i = len;
		while (i > start && !IS_SLASH(path[i-1])) {
			i--;
		}

		if (i == len ||
			(i == len - 1 && path[i] == '.')) {
			/* remove double slashes and '.' */
			len = i - 1;
			is_dir = 1;
			continue;
		} else if (i == len - 2 && path[i] == '.' && path[i+1] == '.') {
			/* remove '..' and previous directory */
			is_dir = 1;
			if (link_is_dir) {
				*link_is_dir = 1;
			}
			if (i - 1 <= start) {
				return start ? start : len;
			}
			j = tsrm_realpath_r(path, start, i-1, ll, t, use_realpath, 1, NULL);
			if (j > start) {
				j--;
				while (j > start && !IS_SLASH(path[j])) {
					j--;
				}
				if (!start) {
					/* leading '..' must not be removed in case of relative path */
					if (j == 0 && path[0] == '.' && path[1] == '.' &&
							IS_SLASH(path[2])) {
						path[3] = '.';
						path[4] = '.';
						path[5] = DEFAULT_SLASH;
						j = 5;
					} else if (j > 0 &&
							path[j+1] == '.' && path[j+2] == '.' &&
							IS_SLASH(path[j+3])) {
						j += 4;
						path[j++] = '.';
						path[j++] = '.';
						path[j] = DEFAULT_SLASH;
					}
				}
			} else if (!start && !j) {
				/* leading '..' must not be removed in case of relative path */
				path[0] = '.';
				path[1] = '.';
				path[2] = DEFAULT_SLASH;
				j = 2;
			}
			return j;
		}

		path[len] = 0;

		save = (use_realpath != CWD_EXPAND);

		if (start && save && CWDG(realpath_cache_size_limit)) {
			/* cache lookup for absolute path */
			if (!*t) {
				*t = time(0);
			}
			if ((bucket = realpath_cache_find(path, len, *t)) != NULL) {
				if (is_dir && !bucket->is_dir) {
					/* not a directory */
					return -1;
				} else {
					if (link_is_dir) {
						*link_is_dir = bucket->is_dir;
					}
					memcpy(path, bucket->realpath, bucket->realpath_len + 1);
					return bucket->realpath_len;
				}
			}
		}

#ifdef ZEND_WIN32
		if (save && (hFind = FindFirstFile(path, &data)) == INVALID_HANDLE_VALUE) {
			if (use_realpath == CWD_REALPATH) {
				/* file not found */
				return -1;
			}
			/* continue resolution anyway but don't save result in the cache */
			save = 0;
		}

		if (save) {
			FindClose(hFind);
		}

		tmp = do_alloca(len+1, use_heap);
		memcpy(tmp, path, len+1);

		if(save &&
				!(IS_UNC_PATH(path, len) && len >= 3 && path[2] != '?') &&
				(data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
			/* File is a reparse point. Get the target */
			HANDLE hLink = NULL;
			REPARSE_DATA_BUFFER * pbuffer;
			DWORD retlength = 0;
			int bufindex = 0, isabsolute = 0;
			wchar_t * reparsetarget;
			BOOL isVolume = FALSE;
			char printname[MAX_PATH];
			char substitutename[MAX_PATH];
			int printname_len, substitutename_len;
			int substitutename_off = 0;

			if(++(*ll) > LINK_MAX) {
				return -1;
			}

			hLink = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT|FILE_FLAG_BACKUP_SEMANTICS, NULL);
			if(hLink == INVALID_HANDLE_VALUE) {
				return -1;
			}

			pbuffer = (REPARSE_DATA_BUFFER *)do_alloca(MAXIMUM_REPARSE_DATA_BUFFER_SIZE, use_heap_large);
			if (pbuffer == NULL) {
				return -1;
			}
			if(!DeviceIoControl(hLink, FSCTL_GET_REPARSE_POINT, NULL, 0, pbuffer,  MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &retlength, NULL)) {
				free_alloca(pbuffer, use_heap_large);
				CloseHandle(hLink);
				return -1;
			}

			CloseHandle(hLink);

			if(pbuffer->ReparseTag == IO_REPARSE_TAG_SYMLINK) {
				reparsetarget = pbuffer->SymbolicLinkReparseBuffer.ReparseTarget;
				printname_len = pbuffer->MountPointReparseBuffer.PrintNameLength / sizeof(WCHAR);
				isabsolute = (pbuffer->SymbolicLinkReparseBuffer.Flags == 0) ? 1 : 0;
				if (!WideCharToMultiByte(CP_THREAD_ACP, 0,
					reparsetarget + pbuffer->MountPointReparseBuffer.PrintNameOffset  / sizeof(WCHAR),
					printname_len + 1,
					printname, MAX_PATH, NULL, NULL
				)) {
					free_alloca(pbuffer, use_heap_large);
					return -1;
				};
				printname_len = pbuffer->MountPointReparseBuffer.PrintNameLength / sizeof(WCHAR);
				printname[printname_len] = 0;

				substitutename_len = pbuffer->MountPointReparseBuffer.SubstituteNameLength / sizeof(WCHAR);
				if (!WideCharToMultiByte(CP_THREAD_ACP, 0,
					reparsetarget + pbuffer->MountPointReparseBuffer.SubstituteNameOffset / sizeof(WCHAR),
					substitutename_len + 1,
					substitutename, MAX_PATH, NULL, NULL
				)) {
					free_alloca(pbuffer, use_heap_large);
					return -1;
				};
				substitutename[substitutename_len] = 0;
			}
			else if(pbuffer->ReparseTag == IO_REPARSE_TAG_MOUNT_POINT) {
				isabsolute = 1;
				reparsetarget = pbuffer->MountPointReparseBuffer.ReparseTarget;
				printname_len = pbuffer->MountPointReparseBuffer.PrintNameLength / sizeof(WCHAR);
				if (!WideCharToMultiByte(CP_THREAD_ACP, 0,
					reparsetarget + pbuffer->MountPointReparseBuffer.PrintNameOffset  / sizeof(WCHAR),
					printname_len + 1,
					printname, MAX_PATH, NULL, NULL
				)) {
					free_alloca(pbuffer, use_heap_large);
					return -1;
				};
				printname[pbuffer->MountPointReparseBuffer.PrintNameLength / sizeof(WCHAR)] = 0;

				substitutename_len = pbuffer->MountPointReparseBuffer.SubstituteNameLength / sizeof(WCHAR);
				if (!WideCharToMultiByte(CP_THREAD_ACP, 0,
					reparsetarget + pbuffer->MountPointReparseBuffer.SubstituteNameOffset / sizeof(WCHAR),
					substitutename_len + 1,
					substitutename, MAX_PATH, NULL, NULL
				)) {
					free_alloca(pbuffer, use_heap_large);
					return -1;
				};
				substitutename[substitutename_len] = 0;
			}
			else if (pbuffer->ReparseTag == IO_REPARSE_TAG_DEDUP) {
				isabsolute = 1;
				memcpy(substitutename, path, len + 1);
				substitutename_len = len;
			} else {
				/* XXX this might be not the end, restart handling with REPARSE_GUID_DATA_BUFFER should be implemented. */
				free_alloca(pbuffer, use_heap_large);
				return -1;
			}

			if(isabsolute && substitutename_len > 4) {
				/* Do not resolve volumes (for now). A mounted point can
				   target a volume without a drive, it is not certain that
				   all IO functions we use in php and its deps support
				   path with volume GUID instead of the DOS way, like:
				   d:\test\mnt\foo
				   \\?\Volume{62d1c3f8-83b9-11de-b108-806e6f6e6963}\foo
				*/
				if (strncmp(substitutename, "\\??\\Volume{",11) == 0
					|| strncmp(substitutename, "\\\\?\\Volume{",11) == 0
					|| strncmp(substitutename, "\\??\\UNC\\", 8) == 0
					) {
					isVolume = TRUE;
					substitutename_off = 0;
				} else
					/* do not use the \??\ and \\?\ prefix*/
					if (strncmp(substitutename, "\\??\\", 4) == 0
						|| strncmp(substitutename, "\\\\?\\", 4) == 0) {
					substitutename_off = 4;
				}
			}

			if (!isVolume) {
				char * tmp2 = substitutename + substitutename_off;
				for(bufindex = 0; bufindex < (substitutename_len - substitutename_off); bufindex++) {
					*(path + bufindex) = *(tmp2 + bufindex);
				}

				*(path + bufindex) = 0;
				j = bufindex;
			} else {
				j = len;
			}


#if VIRTUAL_CWD_DEBUG
			fprintf(stderr, "reparse: print: %s ", printname);
			fprintf(stderr, "sub: %s ", substitutename);
			fprintf(stderr, "resolved: %s ", path);
#endif
			free_alloca(pbuffer, use_heap_large);

			if(isabsolute == 1) {
				if (!((j == 3) && (path[1] == ':') && (path[2] == '\\'))) {
					/* use_realpath is 0 in the call below coz path is absolute*/
					j = tsrm_realpath_r(path, 0, j, ll, t, 0, is_dir, &directory);
					if(j < 0) {
						free_alloca(tmp, use_heap);
						return -1;
					}
				}
			}
			else {
				if(i + j >= MAXPATHLEN - 1) {
					free_alloca(tmp, use_heap);
					return -1;
				}

				memmove(path+i, path, j+1);
				memcpy(path, tmp, i-1);
				path[i-1] = DEFAULT_SLASH;
				j  = tsrm_realpath_r(path, start, i + j, ll, t, use_realpath, is_dir, &directory);
				if(j < 0) {
					free_alloca(tmp, use_heap);
					return -1;
				}
			}
			directory = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

			if(link_is_dir) {
				*link_is_dir = directory;
			}
		}
		else {
			if (save) {
				directory = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
				if (is_dir && !directory) {
					/* not a directory */
					return -1;
				}
			}

#elif defined(NETWARE)
		save = 0;
		tmp = do_alloca(len+1, use_heap);
		memcpy(tmp, path, len+1);
#else
		if (save && php_sys_lstat(path, &st) < 0) {
			if (use_realpath == CWD_REALPATH) {
				/* file not found */
				return -1;
			}
			/* continue resolution anyway but don't save result in the cache */
			save = 0;
		}

		tmp = do_alloca(len+1, use_heap);
		memcpy(tmp, path, len+1);

		if (save && S_ISLNK(st.st_mode)) {
			if (++(*ll) > LINK_MAX || (j = php_sys_readlink(tmp, path, MAXPATHLEN)) < 0) {
				/* too many links or broken symlinks */
				free_alloca(tmp, use_heap);
				return -1;
			}
			path[j] = 0;
			if (IS_ABSOLUTE_PATH(path, j)) {
				j = tsrm_realpath_r(path, 1, j, ll, t, use_realpath, is_dir, &directory);
				if (j < 0) {
					free_alloca(tmp, use_heap);
					return -1;
				}
			} else {
				if (i + j >= MAXPATHLEN-1) {
					free_alloca(tmp, use_heap);
					return -1; /* buffer overflow */
				}
				memmove(path+i, path, j+1);
				memcpy(path, tmp, i-1);
				path[i-1] = DEFAULT_SLASH;
				j = tsrm_realpath_r(path, start, i + j, ll, t, use_realpath, is_dir, &directory);
				if (j < 0) {
					free_alloca(tmp, use_heap);
					return -1;
				}
			}
			if (link_is_dir) {
				*link_is_dir = directory;
			}
		} else {
			if (save) {
				directory = S_ISDIR(st.st_mode);
				if (link_is_dir) {
					*link_is_dir = directory;
				}
				if (is_dir && !directory) {
					/* not a directory */
					free_alloca(tmp, use_heap);
					return -1;
				}
			}
#endif
			if (i - 1 <= start) {
				j = start;
			} else {
				/* some leading directories may be unaccessable */
				j = tsrm_realpath_r(path, start, i-1, ll, t, save ? CWD_FILEPATH : use_realpath, 1, NULL);
				if (j > start) {
					path[j++] = DEFAULT_SLASH;
				}
			}
#ifdef ZEND_WIN32
			if (j < 0 || j + len - i >= MAXPATHLEN-1) {
				free_alloca(tmp, use_heap);
				return -1;
			}
			if (save) {
				i = (int)strlen(data.cFileName);
				memcpy(path+j, data.cFileName, i+1);
				j += i;
			} else {
				/* use the original file or directory name as it wasn't found */
				memcpy(path+j, tmp+i, len-i+1);
				j += (len-i);
			}
		}
#else
			if (j < 0 || j + len - i >= MAXPATHLEN-1) {
				free_alloca(tmp, use_heap);
				return -1;
			}
			memcpy(path+j, tmp+i, len-i+1);
			j += (len-i);
		}
#endif

		if (save && start && CWDG(realpath_cache_size_limit)) {
			/* save absolute path in the cache */
			realpath_cache_add(tmp, len, path, j, directory, *t);
		}

		free_alloca(tmp, use_heap);
		return j;
	}
}
/* }}} */

/* Resolve path relatively to state and put the real path into state */
/* returns 0 for ok, 1 for error */
CWD_API int virtual_file_ex(cwd_state *state, const char *path, verify_path_func verify_path, int use_realpath) /* {{{ */
{
	int path_length = (int)strlen(path);
	char resolved_path[MAXPATHLEN];
	int start = 1;
	int ll = 0;
	time_t t;
	int ret;
	int add_slash;
	void *tmp;

	if (path_length == 0 || path_length >= MAXPATHLEN-1) {
#ifdef ZEND_WIN32
		_set_errno(EINVAL);
#else
		errno = EINVAL;
#endif
		return 1;
	}

#if VIRTUAL_CWD_DEBUG
	fprintf(stderr,"cwd = %s path = %s\n", state->cwd, path);
#endif

	/* cwd_length can be 0 when getcwd() fails.
	 * This can happen under solaris when a dir does not have read permissions
	 * but *does* have execute permissions */
	if (!IS_ABSOLUTE_PATH(path, path_length)) {
		if (state->cwd_length == 0) {
			/* resolve relative path */
			start = 0;
			memcpy(resolved_path , path, path_length + 1);
		} else {
			int state_cwd_length = state->cwd_length;

#ifdef ZEND_WIN32
			if (IS_SLASH(path[0])) {
				if (state->cwd[1] == ':') {
					/* Copy only the drive name */
					state_cwd_length = 2;
				} else if (IS_UNC_PATH(state->cwd, state->cwd_length)) {
					/* Copy only the share name */
					state_cwd_length = 2;
					while (IS_SLASH(state->cwd[state_cwd_length])) {
						state_cwd_length++;
					}
					while (state->cwd[state_cwd_length] &&
							!IS_SLASH(state->cwd[state_cwd_length])) {
						state_cwd_length++;
					}
					while (IS_SLASH(state->cwd[state_cwd_length])) {
						state_cwd_length++;
					}
					while (state->cwd[state_cwd_length] &&
							!IS_SLASH(state->cwd[state_cwd_length])) {
						state_cwd_length++;
					}
				}
			}
#endif
			if (path_length + state_cwd_length + 1 >= MAXPATHLEN-1) {
				return 1;
			}
			memcpy(resolved_path, state->cwd, state_cwd_length);
			if (resolved_path[state_cwd_length-1] == DEFAULT_SLASH) {
				memcpy(resolved_path + state_cwd_length, path, path_length + 1);
				path_length += state_cwd_length;
			} else {
				resolved_path[state_cwd_length] = DEFAULT_SLASH;
				memcpy(resolved_path + state_cwd_length + 1, path, path_length + 1);
				path_length += state_cwd_length + 1;
			}
		}
	} else {
#ifdef ZEND_WIN32
		if (path_length > 2 && path[1] == ':' && !IS_SLASH(path[2])) {
			resolved_path[0] = path[0];
			resolved_path[1] = ':';
			resolved_path[2] = DEFAULT_SLASH;
			memcpy(resolved_path + 3, path + 2, path_length - 1);
			path_length++;
		} else
#endif
		memcpy(resolved_path, path, path_length + 1);
	}

#ifdef ZEND_WIN32
	if (memchr(resolved_path, '*', path_length) ||
		memchr(resolved_path, '?', path_length)) {
		return 1;
	}
#endif

#ifdef ZEND_WIN32
	if (IS_UNC_PATH(resolved_path, path_length)) {
		/* skip UNC name */
		resolved_path[0] = DEFAULT_SLASH;
		resolved_path[1] = DEFAULT_SLASH;
		start = 2;
		while (!IS_SLASH(resolved_path[start])) {
			if (resolved_path[start] == 0) {
				goto verify;
			}
			resolved_path[start] = toupper(resolved_path[start]);
			start++;
		}
		resolved_path[start++] = DEFAULT_SLASH;
		while (!IS_SLASH(resolved_path[start])) {
			if (resolved_path[start] == 0) {
				goto verify;
			}
			resolved_path[start] = toupper(resolved_path[start]);
			start++;
		}
		resolved_path[start++] = DEFAULT_SLASH;
	} else if (IS_ABSOLUTE_PATH(resolved_path, path_length)) {
		/* skip DRIVE name */
		resolved_path[0] = toupper(resolved_path[0]);
		resolved_path[2] = DEFAULT_SLASH;
		start = 3;
	}
#elif defined(NETWARE)
	if (IS_ABSOLUTE_PATH(resolved_path, path_length)) {
		/* skip VOLUME name */
		start = 0;
		while (start != ':') {
			if (resolved_path[start] == 0) return -1;
			start++;
		}
		start++;
		if (!IS_SLASH(resolved_path[start])) return -1;
		resolved_path[start++] = DEFAULT_SLASH;
	}
#endif

	add_slash = (use_realpath != CWD_REALPATH) && path_length > 0 && IS_SLASH(resolved_path[path_length-1]);
	t = CWDG(realpath_cache_ttl) ? 0 : -1;
	path_length = tsrm_realpath_r(resolved_path, start, path_length, &ll, &t, use_realpath, 0, NULL);

	if (path_length < 0) {
		errno = ENOENT;
		return 1;
	}

	if (!start && !path_length) {
		resolved_path[path_length++] = '.';
	}
	if (add_slash && path_length && !IS_SLASH(resolved_path[path_length-1])) {
		if (path_length >= MAXPATHLEN-1) {
			return -1;
		}
		resolved_path[path_length++] = DEFAULT_SLASH;
	}
	resolved_path[path_length] = 0;

#ifdef ZEND_WIN32
verify:
#endif
	if (verify_path) {
		cwd_state old_state;

		CWD_STATE_COPY(&old_state, state);
		state->cwd_length = path_length;

		tmp = erealloc(state->cwd, state->cwd_length+1);
		if (tmp == NULL) {
#if VIRTUAL_CWD_DEBUG
			fprintf (stderr, "Out of memory\n");
#endif
			return 1;
		}
		state->cwd = (char *) tmp;

		memcpy(state->cwd, resolved_path, state->cwd_length+1);
		if (verify_path(state)) {
			CWD_STATE_FREE(state);
			*state = old_state;
			ret = 1;
		} else {
			CWD_STATE_FREE(&old_state);
			ret = 0;
		}
	} else {
		state->cwd_length = path_length;
		tmp = erealloc(state->cwd, state->cwd_length+1);
		if (tmp == NULL) {
#if VIRTUAL_CWD_DEBUG
			fprintf (stderr, "Out of memory\n");
#endif
			return 1;
		}
		state->cwd = (char *) tmp;

		memcpy(state->cwd, resolved_path, state->cwd_length+1);
		ret = 0;
	}

#if VIRTUAL_CWD_DEBUG
	fprintf (stderr, "virtual_file_ex() = %s\n",state->cwd);
#endif
	return (ret);
}
/* }}} */

CWD_API int virtual_chdir(const char *path) /* {{{ */
{
	return virtual_file_ex(&CWDG(cwd), path, php_is_dir_ok, CWD_REALPATH)?-1:0;
}
/* }}} */

CWD_API int virtual_chdir_file(const char *path, int (*p_chdir)(const char *path)) /* {{{ */
{
	int length = (int)strlen(path);
	char *temp;
	int retval;
	ALLOCA_FLAG(use_heap)

	if (length == 0) {
		return 1; /* Can't cd to empty string */
	}
	while(--length >= 0 && !IS_SLASH(path[length])) {
	}

	if (length == -1) {
		/* No directory only file name */
		errno = ENOENT;
		return -1;
	}

	if (length == COPY_WHEN_ABSOLUTE(path) && IS_ABSOLUTE_PATH(path, length+1)) { /* Also use trailing slash if this is absolute */
		length++;
	}
	temp = (char *) do_alloca(length+1, use_heap);
	memcpy(temp, path, length);
	temp[length] = 0;
#if VIRTUAL_CWD_DEBUG
	fprintf (stderr, "Changing directory to %s\n", temp);
#endif
	retval = p_chdir(temp);
	free_alloca(temp, use_heap);
	return retval;
}
/* }}} */

CWD_API char *virtual_realpath(const char *path, char *real_path) /* {{{ */
{
	cwd_state new_state;
	char *retval;
	char cwd[MAXPATHLEN];

	/* realpath("") returns CWD */
	if (!*path) {
		new_state.cwd = (char*)emalloc(1);
		if (new_state.cwd == NULL) {
			retval = NULL;
			goto end;
		}
		new_state.cwd[0] = '\0';
		new_state.cwd_length = 0;
		if (VCWD_GETCWD(cwd, MAXPATHLEN)) {
			path = cwd;
		}
	} else if (!IS_ABSOLUTE_PATH(path, strlen(path))) {
		CWD_STATE_COPY(&new_state, &CWDG(cwd));
	} else {
		new_state.cwd = (char*)emalloc(1);
		if (new_state.cwd == NULL) {
			retval = NULL;
			goto end;
		}
		new_state.cwd[0] = '\0';
		new_state.cwd_length = 0;
	}

	if (virtual_file_ex(&new_state, path, NULL, CWD_REALPATH)==0) {
		int len = new_state.cwd_length>MAXPATHLEN-1?MAXPATHLEN-1:new_state.cwd_length;

		memcpy(real_path, new_state.cwd, len);
		real_path[len] = '\0';
		retval = real_path;
	} else {
		retval = NULL;
	}

	CWD_STATE_FREE(&new_state);
end:
	return retval;
}
/* }}} */

CWD_API int virtual_filepath_ex(const char *path, char **filepath, verify_path_func verify_path) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	retval = virtual_file_ex(&new_state, path, verify_path, CWD_FILEPATH);

	*filepath = new_state.cwd;

	return retval;

}
/* }}} */

CWD_API int virtual_filepath(const char *path, char **filepath) /* {{{ */
{
	return virtual_filepath_ex(path, filepath, php_is_file_ok);
}
/* }}} */

CWD_API FILE *virtual_fopen(const char *path, const char *mode) /* {{{ */
{
	cwd_state new_state;
	FILE *f;

	if (path[0] == '\0') { /* Fail to open empty path */
		return NULL;
	}

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_EXPAND)) {
		CWD_STATE_FREE_ERR(&new_state);
		return NULL;
	}

	f = fopen(new_state.cwd, mode);

	CWD_STATE_FREE_ERR(&new_state);

	return f;
}
/* }}} */

CWD_API int virtual_access(const char *pathname, int mode) /* {{{ */
{
	cwd_state new_state;
	int ret;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, pathname, NULL, CWD_REALPATH)) {
		CWD_STATE_FREE_ERR(&new_state);
		return -1;
	}

#if defined(ZEND_WIN32)
	ret = tsrm_win32_access(new_state.cwd, mode);
#else
	ret = access(new_state.cwd, mode);
#endif

	CWD_STATE_FREE_ERR(&new_state);

	return ret;
}
/* }}} */

#if HAVE_UTIME
CWD_API int virtual_utime(const char *filename, struct utimbuf *buf) /* {{{ */
{
	cwd_state new_state;
	int ret;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, filename, NULL, CWD_REALPATH)) {
		CWD_STATE_FREE_ERR(&new_state);
		return -1;
	}

#ifdef ZEND_WIN32
	ret = win32_utime(new_state.cwd, buf);
#else
	ret = utime(new_state.cwd, buf);
#endif

	CWD_STATE_FREE_ERR(&new_state);
	return ret;
}
/* }}} */
#endif

CWD_API int virtual_chmod(const char *filename, mode_t mode) /* {{{ */
{
	cwd_state new_state;
	int ret;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, filename, NULL, CWD_REALPATH)) {
		CWD_STATE_FREE_ERR(&new_state);
		return -1;
	}

	ret = chmod(new_state.cwd, mode);

	CWD_STATE_FREE_ERR(&new_state);
	return ret;
}
/* }}} */

#if !defined(ZEND_WIN32) && !defined(NETWARE)
CWD_API int virtual_chown(const char *filename, uid_t owner, gid_t group, int link) /* {{{ */
{
	cwd_state new_state;
	int ret;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, filename, NULL, CWD_REALPATH)) {
		CWD_STATE_FREE_ERR(&new_state);
		return -1;
	}

	if (link) {
#if HAVE_LCHOWN
		ret = lchown(new_state.cwd, owner, group);
#else
		ret = -1;
#endif
	} else {
		ret = chown(new_state.cwd, owner, group);
	}

	CWD_STATE_FREE_ERR(&new_state);
	return ret;
}
/* }}} */
#endif

CWD_API int virtual_open(const char *path, int flags, ...) /* {{{ */
{
	cwd_state new_state;
	int f;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_FILEPATH)) {
		CWD_STATE_FREE_ERR(&new_state);
		return -1;
	}

	if (flags & O_CREAT) {
		mode_t mode;
		va_list arg;

		va_start(arg, flags);
		mode = (mode_t) va_arg(arg, int);
		va_end(arg);

		f = open(new_state.cwd, flags, mode);
	} else {
		f = open(new_state.cwd, flags);
	}
	CWD_STATE_FREE_ERR(&new_state);
	return f;
}
/* }}} */

CWD_API int virtual_creat(const char *path, mode_t mode) /* {{{ */
{
	cwd_state new_state;
	int f;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_FILEPATH)) {
		CWD_STATE_FREE_ERR(&new_state);
		return -1;
	}

	f = creat(new_state.cwd,  mode);

	CWD_STATE_FREE_ERR(&new_state);
	return f;
}
/* }}} */

CWD_API int virtual_rename(const char *oldname, const char *newname) /* {{{ */
{
	cwd_state old_state;
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&old_state, &CWDG(cwd));
	if (virtual_file_ex(&old_state, oldname, NULL, CWD_EXPAND)) {
		CWD_STATE_FREE_ERR(&old_state);
		return -1;
	}
	oldname = old_state.cwd;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, newname, NULL, CWD_EXPAND)) {
		CWD_STATE_FREE_ERR(&old_state);
		CWD_STATE_FREE_ERR(&new_state);
		return -1;
	}
	newname = new_state.cwd;

	/* rename on windows will fail if newname already exists.
	   MoveFileEx has to be used */
#ifdef ZEND_WIN32
	/* MoveFileEx returns 0 on failure, other way 'round for this function */
	retval = (MoveFileEx(oldname, newname, MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED) == 0) ? -1 : 0;
#else
	retval = rename(oldname, newname);
#endif

	CWD_STATE_FREE_ERR(&old_state);
	CWD_STATE_FREE_ERR(&new_state);

	return retval;
}
/* }}} */

CWD_API int virtual_stat(const char *path, zend_stat_t *buf) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_REALPATH)) {
		CWD_STATE_FREE_ERR(&new_state);
		return -1;
	}

	retval = php_sys_stat(new_state.cwd, buf);

	CWD_STATE_FREE_ERR(&new_state);
	return retval;
}
/* }}} */

CWD_API int virtual_lstat(const char *path, zend_stat_t *buf) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_EXPAND)) {
		CWD_STATE_FREE_ERR(&new_state);
		return -1;
	}

	retval = php_sys_lstat(new_state.cwd, buf);

	CWD_STATE_FREE_ERR(&new_state);
	return retval;
}
/* }}} */

CWD_API int virtual_unlink(const char *path) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_EXPAND)) {
		CWD_STATE_FREE_ERR(&new_state);
		return -1;
	}

	retval = unlink(new_state.cwd);

	CWD_STATE_FREE_ERR(&new_state);
	return retval;
}
/* }}} */

CWD_API int virtual_mkdir(const char *pathname, mode_t mode) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, pathname, NULL, CWD_FILEPATH)) {
		CWD_STATE_FREE_ERR(&new_state);
		return -1;
	}

#ifdef ZEND_WIN32
	retval = mkdir(new_state.cwd);
#else
	retval = mkdir(new_state.cwd, mode);
#endif
	CWD_STATE_FREE_ERR(&new_state);
	return retval;
}
/* }}} */

CWD_API int virtual_rmdir(const char *pathname) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, pathname, NULL, CWD_EXPAND)) {
		CWD_STATE_FREE_ERR(&new_state);
		return -1;
	}

	retval = rmdir(new_state.cwd);

	CWD_STATE_FREE_ERR(&new_state);
	return retval;
}
/* }}} */

#ifdef ZEND_WIN32
DIR *opendir(const char *name);
#endif

CWD_API DIR *virtual_opendir(const char *pathname) /* {{{ */
{
	cwd_state new_state;
	DIR *retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, pathname, NULL, CWD_REALPATH)) {
		CWD_STATE_FREE_ERR(&new_state);
		return NULL;
	}

	retval = opendir(new_state.cwd);

	CWD_STATE_FREE_ERR(&new_state);
	return retval;
}
/* }}} */

#ifdef ZEND_WIN32
CWD_API FILE *virtual_popen(const char *command, const char *type) /* {{{ */
{
	return popen_ex(command, type, CWDG(cwd).cwd, NULL);
}
/* }}} */
#elif defined(NETWARE)
/* On NetWare, the trick of prepending "cd cwd; " doesn't work so we need to perform
   a VCWD_CHDIR() and mutex it
 */
CWD_API FILE *virtual_popen(const char *command, const char *type) /* {{{ */
{
	char prev_cwd[MAXPATHLEN];
	char *getcwd_result;
	FILE *retval;

	getcwd_result = VCWD_GETCWD(prev_cwd, MAXPATHLEN);
	if (!getcwd_result) {
		return NULL;
	}

#ifdef ZTS
	tsrm_mutex_lock(cwd_mutex);
#endif

	VCWD_CHDIR(CWDG(cwd).cwd);
	retval = popen(command, type);
	VCWD_CHDIR(prev_cwd);

#ifdef ZTS
	tsrm_mutex_unlock(cwd_mutex);
#endif

	return retval;
}
/* }}} */
#else /* Unix */
CWD_API FILE *virtual_popen(const char *command, const char *type) /* {{{ */
{
	int command_length;
	int dir_length, extra = 0;
	char *command_line;
	char *ptr, *dir;
	FILE *retval;

	command_length = strlen(command);

	dir_length = CWDG(cwd).cwd_length;
	dir = CWDG(cwd).cwd;
	while (dir_length > 0) {
		if (*dir == '\'') extra+=3;
		dir++;
		dir_length--;
	}
	dir_length = CWDG(cwd).cwd_length;
	dir = CWDG(cwd).cwd;

	ptr = command_line = (char *) emalloc(command_length + sizeof("cd '' ; ") + dir_length + extra+1+1);
	if (!command_line) {
		return NULL;
	}
	memcpy(ptr, "cd ", sizeof("cd ")-1);
	ptr += sizeof("cd ")-1;

	if (CWDG(cwd).cwd_length == 0) {
		*ptr++ = DEFAULT_SLASH;
	} else {
		*ptr++ = '\'';
		while (dir_length > 0) {
			switch (*dir) {
			case '\'':
				*ptr++ = '\'';
				*ptr++ = '\\';
				*ptr++ = '\'';
				/* fall-through */
			default:
				*ptr++ = *dir;
			}
			dir++;
			dir_length--;
		}
		*ptr++ = '\'';
	}

	*ptr++ = ' ';
	*ptr++ = ';';
	*ptr++ = ' ';

	memcpy(ptr, command, command_length+1);
	retval = popen(command_line, type);

	efree(command_line);
	return retval;
}
/* }}} */
#endif

CWD_API char *tsrm_realpath(const char *path, char *real_path) /* {{{ */
{
	cwd_state new_state;
	char cwd[MAXPATHLEN];

	/* realpath("") returns CWD */
	if (!*path) {
		new_state.cwd = (char*)emalloc(1);
		if (new_state.cwd == NULL) {
			return NULL;
		}
		new_state.cwd[0] = '\0';
		new_state.cwd_length = 0;
		if (VCWD_GETCWD(cwd, MAXPATHLEN)) {
			path = cwd;
		}
	} else if (!IS_ABSOLUTE_PATH(path, strlen(path)) &&
					VCWD_GETCWD(cwd, MAXPATHLEN)) {
		new_state.cwd = estrdup(cwd);
		new_state.cwd_length = (int)strlen(cwd);
	} else {
		new_state.cwd = (char*)emalloc(1);
		if (new_state.cwd == NULL) {
			return NULL;
		}
		new_state.cwd[0] = '\0';
		new_state.cwd_length = 0;
	}

	if (virtual_file_ex(&new_state, path, NULL, CWD_REALPATH)) {
		efree(new_state.cwd);
		return NULL;
	}

	if (real_path) {
		int copy_len = new_state.cwd_length>MAXPATHLEN-1 ? MAXPATHLEN-1 : new_state.cwd_length;
		memcpy(real_path, new_state.cwd, copy_len);
		real_path[copy_len] = '\0';
		efree(new_state.cwd);
		return real_path;
	} else {
		return new_state.cwd;
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
