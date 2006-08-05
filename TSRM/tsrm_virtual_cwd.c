/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2006 The PHP Group                                |
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
# if HAVE_NEWAPIS_H
#  define WANT_GETLONGPATHNAME_WRAPPER
#  define COMPILE_NEWAPIS_STUBS
#  include <NewAPIs.h>
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
	
static int php_is_dir_ok(const cwd_state *state) 
{
	struct stat buf;

	if (stat(state->cwd, &buf) == 0 && S_ISDIR(buf.st_mode))
		return (0);

	return (1);
}

static int php_is_file_ok(const cwd_state *state) 
{
	struct stat buf;

	if (stat(state->cwd, &buf) == 0 && S_ISREG(buf.st_mode))
		return (0);

	return (1);
}

static void cwd_globals_ctor(virtual_cwd_globals *cwd_globals TSRMLS_DC)
{
	CWD_STATE_COPY(&cwd_globals->cwd, &main_cwd_state);
	cwd_globals->realpath_cache_size = 0;
	cwd_globals->realpath_cache_size_limit = REALPATH_CACHE_SIZE;
	cwd_globals->realpath_cache_ttl = REALPATH_CACHE_TTL;
	memset(cwd_globals->realpath_cache, 0, sizeof(cwd_globals->realpath_cache));
}

static void cwd_globals_dtor(virtual_cwd_globals *cwd_globals TSRMLS_DC)
{
	CWD_STATE_FREE(&cwd_globals->cwd);
	{
		int i;

		for (i = 0; i < sizeof(cwd_globals->realpath_cache)/sizeof(cwd_globals->realpath_cache[0]); i++) {
			realpath_cache_bucket *p = cwd_globals->realpath_cache[i];
			while (p != NULL) {
				realpath_cache_bucket *r = p;
				p = p->next;
				free(r);
			}
		}
	}
}

static char *tsrm_strndup(const char *s, size_t length)
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

CWD_API void virtual_cwd_startup(void)
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

CWD_API void virtual_cwd_shutdown(void)
{
#ifndef ZTS
	cwd_globals_dtor(&cwd_globals TSRMLS_CC);
#endif
#if (defined(TSRM_WIN32) || defined(NETWARE)) && defined(ZTS)
	tsrm_mutex_free(cwd_mutex);
#endif

	free(main_cwd_state.cwd); /* Don't use CWD_STATE_FREE because the non global states will probably use emalloc()/efree() */
}

CWD_API char *virtual_getcwd_ex(size_t *length TSRMLS_DC)
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


/* Same semantics as UNIX getcwd() */
CWD_API char *virtual_getcwd(char *buf, size_t size TSRMLS_DC)
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

static inline unsigned long realpath_cache_key(const char *path, int path_len)
{
  register unsigned long h;

  const char *e = path + path_len;
  for (h = 2166136261U; path < e; ) {
    h *= 16777619;
    h ^= *path++;
  }
  return h;
}

static inline void realpath_cache_add(const char *path, int path_len, const char *realpath, int realpath_len, time_t t TSRMLS_DC)
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

static inline realpath_cache_bucket* realpath_cache_find(const char *path, int path_len, time_t t TSRMLS_DC)
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


/* Resolve path relatively to state and put the real path into state */
/* returns 0 for ok, 1 for error */
CWD_API int virtual_file_ex(cwd_state *state, const char *path, verify_path_func verify_path, int use_realpath)
{
	int path_length = strlen(path);
	char *ptr, *path_copy;
	char *tok = NULL;
	int ptr_length;
	cwd_state old_state;
	int ret = 0;
	int copy_amount = -1;
	char *free_path;
	unsigned char is_absolute = 0;
#ifndef TSRM_WIN32
	char resolved_path[MAXPATHLEN];
#else
	char *new_path;
#endif
	char orig_path[MAXPATHLEN];
	int orig_path_len = 0;
	realpath_cache_bucket *bucket;
	time_t t = 0;
	TSRMLS_FETCH();

	if (path_length == 0) 
		return (0);
	if (path_length >= MAXPATHLEN)
		return (1);

	if (use_realpath && CWDG(realpath_cache_size_limit)) {
		if (IS_ABSOLUTE_PATH(path, path_length) || (state->cwd_length < 1)) {
			memcpy(orig_path, path, path_length+1);
			orig_path_len = path_length;
		} else {
			orig_path_len = path_length + state->cwd_length + 1;
			if (orig_path_len >= MAXPATHLEN) {
				return 1;
			}
			memcpy(orig_path, state->cwd, state->cwd_length);
			orig_path[state->cwd_length] = DEFAULT_SLASH;
			memcpy(orig_path + state->cwd_length + 1, path, path_length + 1);
		}
		t = CWDG(realpath_cache_ttl)?time(NULL):0;
		if ((bucket = realpath_cache_find(orig_path, orig_path_len, t TSRMLS_CC)) != NULL) {		
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
#if !defined(TSRM_WIN32) && !defined(NETWARE)
	/* cwd_length can be 0 when getcwd() fails.
	 * This can happen under solaris when a dir does not have read permissions
	 * but *does* have execute permissions */
	if (IS_ABSOLUTE_PATH(path, path_length) || (state->cwd_length < 1)) {
		if (use_realpath) {
			if (realpath(path, resolved_path)) {  /* Note: Not threadsafe on older *BSD's */
				path = resolved_path;
				path_length = strlen(path);
			} else {
				/* disable for now
				return 1; */
			}
		}
	} else { /* Concat current directory with relative path and then run realpath() on it */
		char *tmp;
		char *ptr;

		ptr = tmp = (char *) malloc(state->cwd_length+path_length+sizeof("/"));
		if (!tmp) {
			return 1;
		}
		memcpy(ptr, state->cwd, state->cwd_length);
		ptr += state->cwd_length;
		*ptr++ = DEFAULT_SLASH;
		memcpy(ptr, path, path_length);
		ptr += path_length;
		*ptr = '\0';
		if (strlen(tmp) >= MAXPATHLEN) {
			free(tmp);
			return 1;
		}
		if (use_realpath) {
			if (realpath(tmp, resolved_path)) {
				path = resolved_path;
				path_length = strlen(path);
			} else {
				/* disable for now 
				free(tmp);
				return 1; */
			}
		}
		free(tmp);
	}
#endif
#if defined(TSRM_WIN32)
	{
		int new_path_length;
  
		new_path_length = GetLongPathName(path, NULL, 0);
		if (new_path_length == 0) {
			goto php_failed_getlongpath;
		}

		/* GetLongPathName already counts the \0 */
		new_path = (char *) malloc(new_path_length);
		if (!new_path) {
			return 1;
		}
		
		if (GetLongPathName(path, new_path, new_path_length) != 0) {
			path = new_path;
			path_length = new_path_length;
		} else {
			free(new_path);
php_failed_getlongpath:
			new_path = NULL;
		}
	}
#endif
	free_path = path_copy = tsrm_strndup(path, path_length);

	CWD_STATE_COPY(&old_state, state);
#if VIRTUAL_CWD_DEBUG
	fprintf(stderr,"cwd = %s path = %s\n", state->cwd, path);
#endif
	if (IS_ABSOLUTE_PATH(path_copy, path_length)) {
        copy_amount = COPY_WHEN_ABSOLUTE(path_copy);
		is_absolute = 1;
#ifdef TSRM_WIN32
	} else if (IS_SLASH(path_copy[0])) {
		copy_amount = 2;
#endif
	}

	if (copy_amount != -1) {
		state->cwd = (char *) realloc(state->cwd, copy_amount + 1);
		if (copy_amount) {
			if (is_absolute) {
				memcpy(state->cwd, path_copy, copy_amount);
				path_copy += copy_amount;
			} else {
				memcpy(state->cwd, old_state.cwd, copy_amount);
			}
		}
		state->cwd[copy_amount] = '\0';
		state->cwd_length = copy_amount;
	}


	if (state->cwd_length > 0 || IS_ABSOLUTE_PATH(path, path_length)) {
		ptr = tsrm_strtok_r(path_copy, TOKENIZER_STRING, &tok);
		while (ptr) {
			ptr_length = strlen(ptr);

			if (IS_DIRECTORY_UP(ptr, ptr_length)) {
				char save;

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
				state->cwd = (char *) realloc(state->cwd, state->cwd_length+ptr_length+1+1);
#ifdef TSRM_WIN32
				/* Windows 9x will consider C:\\Foo as a network path. Avoid it. */
				if ((state->cwd[state->cwd_length-1]!='\\' && state->cwd[state->cwd_length-1]!='/') || 
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
				memcpy(&state->cwd[state->cwd_length], ptr, ptr_length+1);
				state->cwd_length += ptr_length;
			}
			ptr = tsrm_strtok_r(NULL, TOKENIZER_STRING, &tok);
		}

		if (state->cwd_length == COPY_WHEN_ABSOLUTE(state->cwd)) {
			state->cwd = (char *) realloc(state->cwd, state->cwd_length+1+1);
			state->cwd[state->cwd_length] = DEFAULT_SLASH;
			state->cwd[state->cwd_length+1] = '\0';
			state->cwd_length++;
		}
	} else {
		state->cwd = (char *) realloc(state->cwd, path_length+1);
		memcpy(state->cwd, path, path_length+1);
		state->cwd_length = path_length;
	}

#ifdef TSRM_WIN32
	if (new_path) {
		free(new_path);
	}
#endif
	free(free_path);

	if (use_realpath && CWDG(realpath_cache_size_limit)) {
		realpath_cache_add(orig_path, orig_path_len, state->cwd, state->cwd_length, t TSRMLS_CC);
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

CWD_API int virtual_chdir(const char *path TSRMLS_DC)
{
	return virtual_file_ex(&CWDG(cwd), path, php_is_dir_ok, 1)?-1:0;
}

CWD_API int virtual_chdir_file(const char *path, int (*p_chdir)(const char *path TSRMLS_DC) TSRMLS_DC)
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

CWD_API char *virtual_realpath(const char *path, char *real_path TSRMLS_DC)
{
	cwd_state new_state;
	char *retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	
	if (virtual_file_ex(&new_state, path, NULL, 1)==0) {
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

CWD_API int virtual_filepath_ex(const char *path, char **filepath, verify_path_func verify_path TSRMLS_DC)
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	retval = virtual_file_ex(&new_state, path, verify_path, 1);

	*filepath = new_state.cwd;

	return retval;

}

CWD_API int virtual_filepath(const char *path, char **filepath TSRMLS_DC)
{
	return virtual_filepath_ex(path, filepath, php_is_file_ok TSRMLS_CC);
}

CWD_API FILE *virtual_fopen(const char *path, const char *mode TSRMLS_DC)
{
	cwd_state new_state;
	FILE *f;

	if (path[0] == '\0') { /* Fail to open empty path */
		return NULL;
	}

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	virtual_file_ex(&new_state, path, NULL, 1);

	f = fopen(new_state.cwd, mode);

	CWD_STATE_FREE(&new_state);
	return f;
}

CWD_API int virtual_access(const char *pathname, int mode TSRMLS_DC)
{
	cwd_state new_state;
	int ret;
	
	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, pathname, NULL, 1)) {
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


#if HAVE_UTIME
CWD_API int virtual_utime(const char *filename, struct utimbuf *buf TSRMLS_DC)
{
	cwd_state new_state;
	int ret;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	virtual_file_ex(&new_state, filename, NULL, 0);

	ret = utime(new_state.cwd, buf);

	CWD_STATE_FREE(&new_state);
	return ret;
}
#endif

CWD_API int virtual_chmod(const char *filename, mode_t mode TSRMLS_DC)
{
	cwd_state new_state;
	int ret;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	virtual_file_ex(&new_state, filename, NULL, 1);

	ret = chmod(new_state.cwd, mode);

	CWD_STATE_FREE(&new_state);
	return ret;
}

#if !defined(TSRM_WIN32) && !defined(NETWARE)
CWD_API int virtual_chown(const char *filename, uid_t owner, gid_t group, int link TSRMLS_DC)
{
	cwd_state new_state;
	int ret;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	virtual_file_ex(&new_state, filename, NULL, 0);

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
#endif

CWD_API int virtual_open(const char *path TSRMLS_DC, int flags, ...)
{
	cwd_state new_state;
	int f;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	virtual_file_ex(&new_state, path, NULL, 1);

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

CWD_API int virtual_creat(const char *path, mode_t mode TSRMLS_DC)
{
	cwd_state new_state;
	int f;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	virtual_file_ex(&new_state, path, NULL, 1);

	f = creat(new_state.cwd,  mode);

	CWD_STATE_FREE(&new_state);
	return f;
}

CWD_API int virtual_rename(char *oldname, char *newname TSRMLS_DC)
{
	cwd_state old_state;
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&old_state, &CWDG(cwd));
	virtual_file_ex(&old_state, oldname, NULL, 0);
	oldname = old_state.cwd;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	virtual_file_ex(&new_state, newname, NULL, 0);
	newname = new_state.cwd;
 
	retval = rename(oldname, newname);

	CWD_STATE_FREE(&old_state);
	CWD_STATE_FREE(&new_state);

	return retval;
}

CWD_API int virtual_stat(const char *path, struct stat *buf TSRMLS_DC)
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	if (virtual_file_ex(&new_state, path, NULL, 1)) {
		return -1;
	}

	retval = stat(new_state.cwd, buf);

	CWD_STATE_FREE(&new_state);
	return retval;
}

#if !defined(TSRM_WIN32)
CWD_API int virtual_lstat(const char *path, struct stat *buf TSRMLS_DC)
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	virtual_file_ex(&new_state, path, NULL, 0);

	retval = lstat(new_state.cwd, buf);

	CWD_STATE_FREE(&new_state);
	return retval;
}
#endif

CWD_API int virtual_unlink(const char *path TSRMLS_DC)
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	virtual_file_ex(&new_state, path, NULL, 0);

	retval = unlink(new_state.cwd);

	CWD_STATE_FREE(&new_state);
	return retval;
}

CWD_API int virtual_mkdir(const char *pathname, mode_t mode TSRMLS_DC)
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	virtual_file_ex(&new_state, pathname, NULL, 1);

#ifdef TSRM_WIN32
	retval = mkdir(new_state.cwd);
#else
	retval = mkdir(new_state.cwd, mode);
#endif
	CWD_STATE_FREE(&new_state);
	return retval;
}

CWD_API int virtual_rmdir(const char *pathname TSRMLS_DC)
{
	cwd_state new_state;
	int retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	virtual_file_ex(&new_state, pathname, NULL, 0);

	retval = rmdir(new_state.cwd);

	CWD_STATE_FREE(&new_state);
	return retval;
}

#ifdef TSRM_WIN32
DIR *opendir(const char *name);
#endif

CWD_API DIR *virtual_opendir(const char *pathname TSRMLS_DC)
{
	cwd_state new_state;
	DIR *retval;

	CWD_STATE_COPY(&new_state, &CWDG(cwd));
	virtual_file_ex(&new_state, pathname, NULL, 1);

	retval = opendir(new_state.cwd);

	CWD_STATE_FREE(&new_state);
	return retval;
}

#ifdef TSRM_WIN32

CWD_API FILE *virtual_popen(const char *command, const char *type TSRMLS_DC)
{
	return popen_ex(command, type, CWDG(cwd).cwd, NULL);
}

#elif defined(NETWARE)

/* On NetWare, the trick of prepending "cd cwd; " doesn't work so we need to perform
   a VCWD_CHDIR() and mutex it
 */
CWD_API FILE *virtual_popen(const char *command, const char *type TSRMLS_DC)
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

#else /* Unix */

CWD_API FILE *virtual_popen(const char *command, const char *type TSRMLS_DC)
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

#endif

/* On AIX & Tru64 when a file does not exist realpath() returns
 * NULL, and sets errno to ENOENT. Unlike in other libc implementations
 * the destination is not filled and remains undefined. Therefor, we
 * must populate it manually using strcpy as done on systems with no
 * realpath() function.
 */
#if defined(__osf__) || defined(_AIX)
char *php_realpath_hack(char *src, char *dest)
{
	char *ret;

	if ((ret = realpath(src, dest)) == NULL && errno == ENOENT) {
		return strcpy(dest, src);
	} else {
		return ret;
	}
}
#endif



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
