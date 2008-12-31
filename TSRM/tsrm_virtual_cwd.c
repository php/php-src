/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
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

#include "tsrm_virtual_cwd.h"
#include "tsrm_strtok_r.h"

#ifdef TSRM_WIN32
#include <io.h>
#include "tsrm_win32.h"
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
#if (defined(TSRM_WIN32) || defined(NETWARE)) && defined(ZTS)
MUTEX_T cwd_mutex;
#endif

#ifdef ZTS
ts_rsrc_id cwd_globals_id;
#else
virtual_cwd_globals cwd_globals;
#endif

cwd_state main_cwd_state; /* True global */

#ifndef TSRM_WIN32
#include <unistd.h>
#else
#include <direct.h>
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode) ((mode) & _S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(mode) ((mode) & _S_IFREG)
#endif

#ifdef TSRM_WIN32
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
	(d)->cwd = (char *) malloc((s)->cwd_length+1);	\
	memcpy((d)->cwd, (s)->cwd, (s)->cwd_length+1);

#define CWD_STATE_FREE(s)			\
	free((s)->cwd);
	
#ifdef TSRM_WIN32
CWD_API int php_sys_stat(const char *path, struct stat *buf) /* {{{ */
{
	WIN32_FILE_ATTRIBUTE_DATA data;
	__int64 t;

	if (!GetFileAttributesEx(path, GetFileExInfoStandard, &data)) {
		return stat(path, buf);
	}

	if (path[1] == ':') {
		if (path[0] >= 'A' && path[0] <= 'Z') {
			buf->st_dev = buf->st_rdev = path[0] - 'A';
		} else {
			buf->st_dev = buf->st_rdev = path[0] - 'a';
		}
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
	buf->st_mode = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? (S_IFDIR|S_IEXEC|(S_IEXEC>>3)|(S_IEXEC>>6)) : S_IFREG;
	buf->st_mode |= (data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? (S_IREAD|(S_IREAD>>3)|(S_IREAD>>6)) : (S_IREAD|(S_IREAD>>3)|(S_IREAD>>6)|S_IWRITE|(S_IWRITE>>3)|(S_IWRITE>>6));
	if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
		int len = strlen(path);

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
	t = data.nFileSizeHigh;
	t = t << 32;
	t |= data.nFileSizeLow;
	buf->st_size = t;
	t = data.ftLastAccessTime.dwHighDateTime;
	t = t << 32;
	t |= data.ftLastAccessTime.dwLowDateTime;
	buf->st_atime = (unsigned long)((t / 10000000) - 11644473600);
	t = data.ftCreationTime.dwHighDateTime;
	t = t << 32;
	t |= data.ftCreationTime.dwLowDateTime;
	buf->st_ctime = (unsigned long)((t / 10000000) - 11644473600);
	t = data.ftLastWriteTime.dwHighDateTime;
	t = t << 32;
	t |= data.ftLastWriteTime.dwLowDateTime;
	buf->st_mtime = (unsigned long)((t / 10000000) - 11644473600);
	return 0;
}
/* }}} */
#endif

static int php_is_dir_ok(const cwd_state *state)  /* {{{ */
{
	struct stat buf;

	if (php_sys_stat(state->cwd, &buf) == 0 && S_ISDIR(buf.st_mode))
		return (0);

	return (1);
}
/* }}} */

static int php_is_file_ok(const cwd_state *state)  /* {{{ */
{
	struct stat buf;

	if (php_sys_stat(state->cwd, &buf) == 0 && S_ISREG(buf.st_mode))
		return (0);

	return (1);
}
/* }}} */

static void cwd_globals_ctor(virtual_cwd_globals *cwd_globals TSRMLS_DC) /* {{{ */
{
	CWD_STATE_COPY(&cwd_globals->cwd, &main_cwd_state);
	cwd_globals->realpath_cache_size = 0;
	cwd_globals->realpath_cache_size_limit = REALPATH_CACHE_SIZE;
	cwd_globals->realpath_cache_ttl = REALPATH_CACHE_TTL;
	memset(cwd_globals->realpath_cache, 0, sizeof(cwd_globals->realpath_cache));
}
/* }}} */

static void cwd_globals_dtor(virtual_cwd_globals *cwd_globals TSRMLS_DC) /* {{{ */
{
	CWD_STATE_FREE(&cwd_globals->cwd);
	realpath_cache_clean(TSRMLS_C);
}
/* }}} */

static char *tsrm_strndup(const char *s, size_t length) /* {{{ */
{
    char *p;

    p = (char *) malloc(length+1);
    if (!p) {
        return (char *)NULL;
    }
    if (length) {
        memcpy(p,s,length);
    }
    p[length]=0;
    return p;
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
	main_cwd_state.cwd = strdup(cwd);
	main_cwd_state.cwd_length = strlen(cwd);

#ifdef ZTS
	ts_allocate_id(&cwd_globals_id, sizeof(virtual_cwd_globals), (ts_allocate_ctor) cwd_globals_ctor, (ts_allocate_dtor) cwd_globals_dtor);
#else
	cwd_globals_ctor(&cwd_globals TSRMLS_CC);
#endif

#if (defined(TSRM_WIN32) || defined(NETWARE)) && defined(ZTS)
	cwd_mutex = tsrm_mutex_alloc();
#endif
}
/* }}} */

CWD_API void virtual_cwd_shutdown(void) /* {{{ */
{
#ifndef ZTS
	cwd_globals_dtor(&cwd_globals TSRMLS_CC);
#endif
#if (defined(TSRM_WIN32) || defined(NETWARE)) && defined(ZTS)
	tsrm_mutex_free(cwd_mutex);
#endif

	free(main_cwd_state.cwd); /* Don't use CWD_STATE_FREE because the non global states will probably use emalloc()/efree() */
}
/* }}} */

CWD_API char *virtual_getcwd_ex(size_t *length TSRMLS_DC) /* {{{ */
{
	cwd_state *state;

	state = &CWDG(cwd);

	if (state->cwd_length == 0) {
		char *retval;

		*length = 1;
		retval = (char *) malloc(2);
		retval[0] = DEFAULT_SLASH;
		retval[1] = '\0';	
		return retval;
	}

#ifdef TSRM_WIN32
	/* If we have something like C: */
	if (state->cwd_length == 2 && state->cwd[state->cwd_length-1] == ':') {
		char *retval;

		*length = state->cwd_length+1;
		retval = (char *) malloc(*length+1);
		memcpy(retval, state->cwd, *length);
		retval[*length-1] = DEFAULT_SLASH;
		retval[*length] = '\0';
		return retval;
	}
#endif
	*length = state->cwd_length;
	return strdup(state->cwd);
}
/* }}} */

/* Same semantics as UNIX getcwd() */
CWD_API char *virtual_getcwd(char *buf, size_t size TSRMLS_DC) /* {{{ */
{
	size_t length;
	char *cwd;

	cwd = virtual_getcwd_ex(&length TSRMLS_CC);

	if (buf == NULL) {
		return cwd;
	}
	if (length > size-1) {
		free(cwd);
		errno = ERANGE; /* Is this OK? */
		return NULL;
	}
	memcpy(buf, cwd, length+1);
	free(cwd);
	return buf;
}
/* }}} */

static inline unsigned long realpath_cache_key(const char *path, int path_len) /* {{{ */
{
	register unsigned long h;
	const char *e = path + path_len;

	for (h = 2166136261U; path < e;) {
		h *= 16777619;
		h ^= *path++;
	}

	return h;
}
/* }}} */

CWD_API void realpath_cache_clean(TSRMLS_D) /* {{{ */
{
	int i;

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

CWD_API void realpath_cache_del(const char *path, int path_len TSRMLS_DC) /* {{{ */
{
	unsigned long key = realpath_cache_key(path, path_len);
	unsigned long n = key % (sizeof(CWDG(realpath_cache)) / sizeof(CWDG(realpath_cache)[0]));
	realpath_cache_bucket **bucket = &CWDG(realpath_cache)[n];

	while (*bucket != NULL) {
		if (key == (*bucket)->key && path_len == (*bucket)->path_len &&
		           memcmp(path, (*bucket)->path, path_len) == 0) {
			realpath_cache_bucket *r = *bucket;
			*bucket = (*bucket)->next;
			CWDG(realpath_cache_size) -= sizeof(realpath_cache_bucket) + r->path_len + 1 + r->realpath_len + 1;
			free(r);
			return;
		} else {
			bucket = &(*bucket)->next;
		}
	}
}
/* }}} */

static inline void realpath_cache_add(const char *path, int path_len, const char *realpath, int realpath_len, time_t t TSRMLS_DC) /* {{{ */
{
	long size = sizeof(realpath_cache_bucket) + path_len + 1 + realpath_len + 1;
	if (CWDG(realpath_cache_size) + size <= CWDG(realpath_cache_size_limit)) {
		realpath_cache_bucket *bucket = malloc(size);
		unsigned long n;
	
		bucket->key = realpath_cache_key(path, path_len);
		bucket->path = (char*)bucket + sizeof(realpath_cache_bucket);
		memcpy(bucket->path, path, path_len+1);
		bucket->path_len = path_len;
		bucket->realpath = bucket->path + (path_len + 1);
		memcpy(bucket->realpath, realpath, realpath_len+1);
		bucket->realpath_len = realpath_len;
		bucket->expires = t + CWDG(realpath_cache_ttl);
		n = bucket->key % (sizeof(CWDG(realpath_cache)) / sizeof(CWDG(realpath_cache)[0]));
		bucket->next = CWDG(realpath_cache)[n];
		CWDG(realpath_cache)[n] = bucket;
		CWDG(realpath_cache_size) += size;
	}
}
/* }}} */

static inline realpath_cache_bucket* realpath_cache_find(const char *path, int path_len, time_t t TSRMLS_DC) /* {{{ */
{
	unsigned long key = realpath_cache_key(path, path_len);
	unsigned long n = key % (sizeof(CWDG(realpath_cache)) / sizeof(CWDG(realpath_cache)[0]));
	realpath_cache_bucket **bucket = &CWDG(realpath_cache)[n];

	while (*bucket != NULL) {
		if (CWDG(realpath_cache_ttl) && (*bucket)->expires < t) {
			realpath_cache_bucket *r = *bucket;
			*bucket = (*bucket)->next;
			CWDG(realpath_cache_size) -= sizeof(realpath_cache_bucket) + r->path_len + 1 + r->realpath_len + 1;
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

/* Resolve path relatively to state and put the real path into state */
/* returns 0 for ok, 1 for error */
CWD_API int virtual_file_ex(cwd_state *state, const char *path, verify_path_func verify_path, int use_realpath) /* {{{ */
{
	int path_length = strlen(path);
	cwd_state old_state;
	char orig_path[MAXPATHLEN];
	realpath_cache_bucket *bucket;
	time_t t = 0;
	int ret;
	int use_cache;
	int use_relative_path = 0;
#ifdef TSRM_WIN32
	int is_unc;
	int exists;
#endif
	TSRMLS_FETCH();

	use_cache = ((use_realpath != CWD_EXPAND) && CWDG(realpath_cache_size_limit));

	if (path_length == 0) 
		return (1);
	if (path_length >= MAXPATHLEN)
		return (1);

#if VIRTUAL_CWD_DEBUG
		fprintf(stderr,"cwd = %s path = %s\n", state->cwd, path);
#endif

	/* cwd_length can be 0 when getcwd() fails.
	 * This can happen under solaris when a dir does not have read permissions
	 * but *does* have execute permissions */
	if (!IS_ABSOLUTE_PATH(path, path_length)) {
		if (state->cwd_length == 0) {
			use_cache = 0;
			use_relative_path = 1;
		} else {
			int orig_path_len;
			int state_cwd_length = state->cwd_length;

#ifdef TSRM_WIN32
			if (IS_SLASH(path[0])) {
				state_cwd_length = 2;
			}
#endif
			orig_path_len = path_length + state_cwd_length + 1;
			if (orig_path_len >= MAXPATHLEN) {
				return 1;
			}
			memcpy(orig_path, state->cwd, state_cwd_length);
			orig_path[state_cwd_length] = DEFAULT_SLASH;
			memcpy(orig_path + state_cwd_length + 1, path, path_length + 1);
			path = orig_path;
			path_length = orig_path_len; 
		}
	}

	if (use_cache) {
		t = CWDG(realpath_cache_ttl)?time(0):0;
		if ((bucket = realpath_cache_find(path, path_length, t TSRMLS_CC)) != NULL) {		
			int len = bucket->realpath_len;

			CWD_STATE_COPY(&old_state, state);
			state->cwd = (char *) realloc(state->cwd, len+1);
			memcpy(state->cwd, bucket->realpath, len+1);
			state->cwd_length = len;
			if (verify_path && verify_path(state)) {
				CWD_STATE_FREE(state);
				*state = old_state;
				return 1;
			} else {
				CWD_STATE_FREE(&old_state);
				return 0;
			}
		}
	}

	if (use_realpath != CWD_EXPAND) {
#if !defined(TSRM_WIN32) && !defined(NETWARE)
		char resolved_path[MAXPATHLEN];

		if (!realpath(path, resolved_path)) {  /* Note: Not threadsafe on older *BSD's */
			if (use_realpath == CWD_REALPATH) {
				return 1;
			}
			goto no_realpath;
		}
		use_realpath = CWD_REALPATH;
		CWD_STATE_COPY(&old_state, state);

		state->cwd_length = strlen(resolved_path);
		state->cwd = (char *) realloc(state->cwd, state->cwd_length+1);
		memcpy(state->cwd, resolved_path, state->cwd_length+1);
#else
		goto no_realpath;
#endif
	} else {
		char *ptr, *path_copy, *free_path;
		char *tok;
		int ptr_length;
no_realpath:

#ifdef TSRM_WIN32
		if (memchr(path, '*', path_length) ||
		    memchr(path, '?', path_length)) {
			return 1;
		}
#endif

		free_path = path_copy = tsrm_strndup(path, path_length);
		CWD_STATE_COPY(&old_state, state);

#ifdef TSRM_WIN32
		exists = (use_realpath != CWD_EXPAND);
		ret = 0;
		is_unc = 0;
		if (path_length >= 2 && path[1] == ':') {			
			state->cwd = (char *) realloc(state->cwd, 2 + 1);
			state->cwd[0] = toupper(path[0]);
			state->cwd[1] = ':';
			state->cwd[2] = '\0';
			state->cwd_length = 2;
			path_copy += 2;
		} else if (IS_UNC_PATH(path, path_length)) {
			state->cwd = (char *) realloc(state->cwd, 1 + 1);
			state->cwd[0] = DEFAULT_SLASH;
			state->cwd[1] = '\0';
			state->cwd_length = 1;
			path_copy += 2;
			is_unc = 2;
		} else {
#endif
			state->cwd = (char *) realloc(state->cwd, 1);
			state->cwd[0] = '\0';
			state->cwd_length = 0;
#ifdef TSRM_WIN32
		}
#endif
		
		tok = NULL;
		ptr = tsrm_strtok_r(path_copy, TOKENIZER_STRING, &tok);
		while (ptr) {
			ptr_length = strlen(ptr);

			if (IS_DIRECTORY_UP(ptr, ptr_length)) {
				char save;

				if (use_relative_path) {
					CWD_STATE_FREE(state);
					*state = old_state;
					return 1;
				}

				save = DEFAULT_SLASH;

#define PREVIOUS state->cwd[state->cwd_length - 1]

				while (IS_ABSOLUTE_PATH(state->cwd, state->cwd_length) &&
						!IS_SLASH(PREVIOUS)) {
					save = PREVIOUS;
					PREVIOUS = '\0';
					state->cwd_length--;
				}

				if (!IS_ABSOLUTE_PATH(state->cwd, state->cwd_length)) {
					state->cwd[state->cwd_length++] = save;
					state->cwd[state->cwd_length] = '\0';
				} else {
					PREVIOUS = '\0';
					state->cwd_length--;
				}
			} else if (!IS_DIRECTORY_CURRENT(ptr, ptr_length)) {
				if (use_relative_path) {
					state->cwd = (char *) realloc(state->cwd, state->cwd_length+ptr_length+1);
					use_relative_path = 0;
				} else {
					state->cwd = (char *) realloc(state->cwd, state->cwd_length+ptr_length+1+1);
#ifdef TSRM_WIN32
					/* Windows 9x will consider C:\\Foo as a network path. Avoid it. */
					if (state->cwd_length < 2 ||
					    (state->cwd[state->cwd_length-1]!='\\' && state->cwd[state->cwd_length-1]!='/') ||
							IsDBCSLeadByte(state->cwd[state->cwd_length-2])) {
						state->cwd[state->cwd_length++] = DEFAULT_SLASH;
					}
#elif defined(NETWARE)
					/* 
					Below code keeps appending to state->cwd a File system seperator
					cases where this appending should not happen is given below,
					a) sys: should just be left as it is
					b) sys:system should just be left as it is,
						Colon is allowed only in the first token as volume names alone can have the : in their names.
						Files and Directories cannot have : in their names
						So the check goes like this,
						For second token and above simply append the DEFAULT_SLASH to the state->cwd.
						For first token check for the existence of : 
						if it exists don't append the DEFAULT_SLASH to the state->cwd.
					*/
					if(((state->cwd_length == 0) && (strchr(ptr, ':') == NULL)) || (state->cwd_length > 0)) {
						state->cwd[state->cwd_length++] = DEFAULT_SLASH;
					}
#else
					state->cwd[state->cwd_length++] = DEFAULT_SLASH;
#endif
				}
				memcpy(&state->cwd[state->cwd_length], ptr, ptr_length+1);

#ifdef TSRM_WIN32
				if (use_realpath != CWD_EXPAND) {
					WIN32_FIND_DATA data;
					HANDLE hFind;

					if ((hFind = FindFirstFile(state->cwd, &data)) != INVALID_HANDLE_VALUE) {
						int length = strlen(data.cFileName);

						if (length != ptr_length) {
							state->cwd = (char *) realloc(state->cwd, state->cwd_length+length+1);
						}
						memcpy(&state->cwd[state->cwd_length], data.cFileName, length+1);
						ptr_length = length;
						FindClose(hFind);
						ret = 0;
					} else {
						if (is_unc) {
							/* skip share name */
							is_unc--;
							ret = 0;
						} else {
							exists = 0;
							if (use_realpath == CWD_REALPATH) {
								ret = 1;
							}
						}
					}
				}
#endif

				state->cwd_length += ptr_length;
			}
			ptr = tsrm_strtok_r(NULL, TOKENIZER_STRING, &tok);
		}
		free(free_path);

		if (use_realpath == CWD_REALPATH) {
			if (ret) {
				CWD_STATE_FREE(state);
				*state = old_state;					
				return 1;
			}
		} else {
#if defined(TSRM_WIN32) || defined(NETWARE)
			if (path[path_length-1] == '\\' || path[path_length-1] == '/') {
#else 
			if (path[path_length-1] == '/') {
#endif
				state->cwd = (char*)realloc(state->cwd, state->cwd_length + 2);
				state->cwd[state->cwd_length++] = DEFAULT_SLASH;
				state->cwd[state->cwd_length] = 0;
			}
		}

		if (state->cwd_length == COPY_WHEN_ABSOLUTE(state->cwd)) {
			state->cwd = (char *) realloc(state->cwd, state->cwd_length+1+1);
			state->cwd[state->cwd_length] = DEFAULT_SLASH;
			state->cwd[state->cwd_length+1] = '\0';
			state->cwd_length++;
		}
	}

	/* Store existent file in realpath cache. */
#ifdef TSRM_WIN32
	if (use_cache && !is_unc && exists) {
#else
	if (use_cache && (use_realpath == CWD_REALPATH)) {
#endif
		realpath_cache_add(path, path_length, state->cwd, state->cwd_length, t TSRMLS_CC);
	}

	if (verify_path && verify_path(state)) {
		CWD_STATE_FREE(state);
		*state = old_state;
		ret = 1;
	} else {
		CWD_STATE_FREE(&old_state);
		ret = 0;
	}
	
#if VIRTUAL_CWD_DEBUG
	fprintf (stderr, "virtual_file_ex() = %s\n",state->cwd);
#endif
	return (ret);
}
/* }}} */

CWD_API int virtual_chdir(const char *path TSRMLS_DC) /* {{{ */
{
	return virtual_file_ex(&CWDG(cwd), path, php_is_dir_ok, CWD_REALPATH)?-1:0;
}
/* }}} */

CWD_API int virtual_chdir_file(const char *path, int (*p_chdir)(const char *path TSRMLS_DC) TSRMLS_DC) /* {{{ */
{
	int length = strlen(path);
	char *temp;
	int retval;

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
	temp = (char *) tsrm_do_alloca(length+1);
	memcpy(temp, path, length);
	temp[length] = 0;
#if VIRTUAL_CWD_DEBUG
	fprintf (stderr, "Changing directory to %s\n", temp);
#endif
	retval = p_chdir(temp TSRMLS_CC);
	tsrm_free_alloca(temp);
	return retval;
}
/* }}} */

CWD_API char *virtual_realpath(const char *path, char *real_path TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	char *retval;
	char cwd[MAXPATHLEN];

	/* realpath("") returns CWD */
	if (!*path) {
		new_state.cwd = (char*)malloc(1);
		new_state.cwd[0] = '\0';
		new_state.cwd_length = 0;		
	    if (VCWD_GETCWD(cwd, MAXPATHLEN)) {
		    path = cwd;
		}
	} else if (!IS_ABSOLUTE_PATH(path, strlen(path))) {
		CWD_STATE_COPY(&new_state, &CWDG(cwd));
	} else {
		new_state.cwd = (char*)malloc(1);
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

	return retval;
}
/* }}} */

CWD_API int virtual_filepath_ex(const char *path, char **filepath, verify_path_func verify_path TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	retval = virtual_file_ex(&new_state, path, verify_path, CWD_FILEPATH);

	*filepath = new_state.cwd;

	return retval;

}
/* }}} */

CWD_API int virtual_filepath(const char *path, char **filepath TSRMLS_DC) /* {{{ */
{
	return virtual_filepath_ex(path, filepath, php_is_file_ok TSRMLS_CC);
}
/* }}} */

CWD_API FILE *virtual_fopen(const char *path, const char *mode TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	FILE *f;

	if (path[0] == '\0') { /* Fail to open empty path */
		return NULL;
	}

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_FILEPATH)) {
		CWD_STATE_FREE(&new_state);
		return NULL;
	}

	f = fopen(new_state.cwd, mode);

	CWD_STATE_FREE(&new_state);
	return f;
}
/* }}} */

CWD_API int virtual_access(const char *pathname, int mode TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int ret;
	
	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, pathname, NULL, CWD_REALPATH)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

#if defined(TSRM_WIN32)
	ret = tsrm_win32_access(new_state.cwd, mode);
#else
	ret = access(new_state.cwd, mode);
#endif
	
	CWD_STATE_FREE(&new_state);
	
	return ret;
}
/* }}} */

#if HAVE_UTIME
CWD_API int virtual_utime(const char *filename, struct utimbuf *buf TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int ret;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, filename, NULL, CWD_REALPATH)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

	ret = utime(new_state.cwd, buf);

	CWD_STATE_FREE(&new_state);
	return ret;
}
/* }}} */
#endif

CWD_API int virtual_chmod(const char *filename, mode_t mode TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int ret;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, filename, NULL, CWD_REALPATH)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

	ret = chmod(new_state.cwd, mode);

	CWD_STATE_FREE(&new_state);
	return ret;
}
/* }}} */

#if !defined(TSRM_WIN32) && !defined(NETWARE)
CWD_API int virtual_chown(const char *filename, uid_t owner, gid_t group, int link TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int ret;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, filename, NULL, CWD_REALPATH)) {
		CWD_STATE_FREE(&new_state);
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

	CWD_STATE_FREE(&new_state);
	return ret;
}
/* }}} */
#endif

CWD_API int virtual_open(const char *path TSRMLS_DC, int flags, ...) /* {{{ */
{
	cwd_state new_state;
	int f;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_FILEPATH)) {
		CWD_STATE_FREE(&new_state);
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
	CWD_STATE_FREE(&new_state);
	return f;
}
/* }}} */

CWD_API int virtual_creat(const char *path, mode_t mode TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int f;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_FILEPATH)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

	f = creat(new_state.cwd,  mode);

	CWD_STATE_FREE(&new_state);
	return f;
}
/* }}} */

CWD_API int virtual_rename(char *oldname, char *newname TSRMLS_DC) /* {{{ */
{
	cwd_state old_state;
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&old_state, &CWDG(cwd));
	if (virtual_file_ex(&old_state, oldname, NULL, CWD_EXPAND)) {
		CWD_STATE_FREE(&old_state);
		return -1;
	}
	oldname = old_state.cwd;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, newname, NULL, CWD_EXPAND)) {
		CWD_STATE_FREE(&old_state);
		CWD_STATE_FREE(&new_state);
		return -1;
	}
	newname = new_state.cwd;
 
	retval = rename(oldname, newname);

	CWD_STATE_FREE(&old_state);
	CWD_STATE_FREE(&new_state);

	return retval;
}
/* }}} */

CWD_API int virtual_stat(const char *path, struct stat *buf TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_REALPATH)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

	retval = php_sys_stat(new_state.cwd, buf);

	CWD_STATE_FREE(&new_state);
	return retval;
}
/* }}} */

#if !defined(TSRM_WIN32)
CWD_API int virtual_lstat(const char *path, struct stat *buf TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_EXPAND)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

	retval = lstat(new_state.cwd, buf);

	CWD_STATE_FREE(&new_state);
	return retval;
}
/* }}} */
#endif

CWD_API int virtual_unlink(const char *path TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, CWD_EXPAND)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

	retval = unlink(new_state.cwd);

	CWD_STATE_FREE(&new_state);
	return retval;
}
/* }}} */

CWD_API int virtual_mkdir(const char *pathname, mode_t mode TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, pathname, NULL, CWD_FILEPATH)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

#ifdef TSRM_WIN32
	retval = mkdir(new_state.cwd);
#else
	retval = mkdir(new_state.cwd, mode);
#endif
	CWD_STATE_FREE(&new_state);
	return retval;
}
/* }}} */

CWD_API int virtual_rmdir(const char *pathname TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, pathname, NULL, CWD_EXPAND)) {
		CWD_STATE_FREE(&new_state);
		return -1;
	}

	retval = rmdir(new_state.cwd);

	CWD_STATE_FREE(&new_state);
	return retval;
}
/* }}} */

#ifdef TSRM_WIN32
DIR *opendir(const char *name);
#endif

CWD_API DIR *virtual_opendir(const char *pathname TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	DIR *retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, pathname, NULL, CWD_REALPATH)) {
		CWD_STATE_FREE(&new_state);
		return NULL;
	}

	retval = opendir(new_state.cwd);

	CWD_STATE_FREE(&new_state);
	return retval;
}
/* }}} */

#ifdef TSRM_WIN32
CWD_API FILE *virtual_popen(const char *command, const char *type TSRMLS_DC) /* {{{ */
{
	return popen_ex(command, type, CWDG(cwd).cwd, NULL);
}
/* }}} */
#elif defined(NETWARE)
/* On NetWare, the trick of prepending "cd cwd; " doesn't work so we need to perform
   a VCWD_CHDIR() and mutex it
 */
CWD_API FILE *virtual_popen(const char *command, const char *type TSRMLS_DC) /* {{{ */
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
CWD_API FILE *virtual_popen(const char *command, const char *type TSRMLS_DC) /* {{{ */
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

	ptr = command_line = (char *) malloc(command_length + sizeof("cd '' ; ") + dir_length + extra+1+1);
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

	free(command_line);
	return retval;
}
/* }}} */
#endif

CWD_API char *tsrm_realpath(const char *path, char *real_path TSRMLS_DC) /* {{{ */
{
	cwd_state new_state;
	char cwd[MAXPATHLEN];

	/* realpath("") returns CWD */
	if (!*path) {
		new_state.cwd = (char*)malloc(1);
		new_state.cwd[0] = '\0';
		new_state.cwd_length = 0;		
	    if (VCWD_GETCWD(cwd, MAXPATHLEN)) {
		    path = cwd;
		}
	} else if (!IS_ABSOLUTE_PATH(path, strlen(path)) &&
	    VCWD_GETCWD(cwd, MAXPATHLEN)) {
		new_state.cwd = strdup(cwd);
		new_state.cwd_length = strlen(cwd);
	} else {
		new_state.cwd = (char*)malloc(1);
		new_state.cwd[0] = '\0';
		new_state.cwd_length = 0;		
	}

	if (virtual_file_ex(&new_state, path, NULL, CWD_REALPATH)) {
		free(new_state.cwd);
		return NULL;
	}

	if (real_path) {
		int copy_len = new_state.cwd_length>MAXPATHLEN-1 ? MAXPATHLEN-1 : new_state.cwd_length;
		memcpy(real_path, new_state.cwd, copy_len);
		real_path[copy_len] = '\0';
		free(new_state.cwd);
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
