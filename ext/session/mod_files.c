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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

/**************************************************************************
 * Files save handler should be used as reference implementations of session
 * save handlers. PS_* functions are called as follows with standard usage.
 *
 *    PS_OPEN_FUNC()  - Create module data that manages save handler.
 *    PS_CREATE_SID() and/or PS_VALIDATE_SID()
 *                    - PS_CREATE_ID() is called if session ID(key) is not
 *                      provided or invalid. PS_VALIDATE_SID() is called to
 *                      verify session ID already exists or not to mitigate
 *                      session adoption vulnerability risk.
 *    PS_READ_FUNC()  - Read data from storage.
 *    PS_GC_FUNC()    - Perform GC. Called by probability
 *                                (session.gc_probability/session.gc_divisor).
 *    PS_WRITE_FUNC() or PS_UPDATE_TIMESTAMP() 
 *                    - Write session data or update session data timestamp.
 *                      It depends on session data change.
 *    PS_CLOSE_FUNC() - Clean up module data created by PS_OPEN_FUNC().
 *
 * Session module guarantees PS_OPEN_FUNC() is called before calling other
 * PS_*_FUNC() functions. Other than this, session module may call any
 * PS_*_FUNC() at any time. You may assume non null *mod_data created by
 * PS_OPEN_FUNC() is passed to PS_*_FUNC().
 *
 * NOTE:
 *  - Save handlers _MUST_NOT_ change/refer PS() values.
 *    i.e. PS(id), PS(session_status), PS(mod) and any other PS() values.
 *    Use only function parameters passed from session module.
 *  - Save handler _MUST_ use PS_GET_MOD_DATA()/PS_SET_MOD_DATA() macro to
 *    set/get save handler module data(mod_data). mod_data contains
 *    data required by PS modules. It will not be NULL except PS_OPEN_FUNC().
 *  - Refer to PS_* macros in php_session.h for function/parameter definitions.
 *  - Returning FAILURE state from PS_* function results in raising errors.
 *    Avoid failure state as much as possible.
 *  - Use static ps_[module name]_[function name] functions for internal use.
 *************************************************************************/

#include "php.h"

#include <sys/stat.h>
#include <sys/types.h>

#if HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#if HAVE_DIRENT_H
#include <dirent.h>
#endif

#ifdef PHP_WIN32
#include "win32/readdir.h"
#endif
#include <time.h>

#include <fcntl.h>
#include <errno.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "php_session.h"
#include "mod_files.h"
#include "ext/standard/flock_compat.h"
#include "php_open_temporary_file.h"

#define FILE_PREFIX "sess_"

#ifdef PHP_WIN32
# ifndef O_NOFOLLOW
#  define O_NOFOLLOW 0
# endif
#endif

typedef struct {
	char *lastkey;
	char *basedir;
	size_t basedir_len;
	size_t dirdepth;
	size_t st_size;
	int filemode;
	int fd;
} ps_files;

ps_module ps_mod_files = {
	/* New save handlers MUST use PS_MOD_UPDATE_TIMESTAMP macro */
	PS_MOD_UPDATE_TIMESTAMP(files)
};


static char *ps_files_path_create(char *buf, size_t buflen, ps_files *data, const char *key)
{
	size_t key_len;
	const char *p;
	int i;
	int n;

	key_len = strlen(key);
	if (key_len <= data->dirdepth ||
		buflen < (strlen(data->basedir) + 2 * data->dirdepth + key_len + 5 + sizeof(FILE_PREFIX))) {
		return NULL;
	}

	p = key;
	memcpy(buf, data->basedir, data->basedir_len);
	n = data->basedir_len;
	buf[n++] = PHP_DIR_SEPARATOR;
	for (i = 0; i < (int)data->dirdepth; i++) {
		buf[n++] = *p++;
		buf[n++] = PHP_DIR_SEPARATOR;
	}
	memcpy(buf + n, FILE_PREFIX, sizeof(FILE_PREFIX) - 1);
	n += sizeof(FILE_PREFIX) - 1;
	memcpy(buf + n, key, key_len);
	n += key_len;
	buf[n] = '\0';

	return buf;
}

#ifndef O_BINARY
# define O_BINARY 0
#endif

static void ps_files_close(ps_files *data)
{
	if (data->fd != -1) {
#ifdef PHP_WIN32
		/* On Win32 locked files that are closed without being explicitly unlocked
		   will be unlocked only when "system resources become available". */
		flock(data->fd, LOCK_UN);
#endif
		close(data->fd);
		data->fd = -1;
	}
}

static void ps_files_open(ps_files *data, const char *key)
{
	char buf[MAXPATHLEN];
#if !defined(O_NOFOLLOW) || !defined(PHP_WIN32)
    struct stat sbuf;
#endif
	int ret;

	if (data->fd < 0 || !data->lastkey || strcmp(key, data->lastkey)) {
		if (data->lastkey) {
			efree(data->lastkey);
			data->lastkey = NULL;
		}

		ps_files_close(data);

		if (php_session_valid_key(key) == FAILURE) {
			php_error_docref(NULL, E_WARNING, "The session id is too long or contains illegal characters, valid characters are a-z, A-Z, 0-9 and '-,'");
			return;
		}

		if (!ps_files_path_create(buf, sizeof(buf), data, key)) {
			return;
		}

		data->lastkey = estrdup(key);

		/* O_NOFOLLOW to prevent us from following evil symlinks */
#ifdef O_NOFOLLOW
		data->fd = VCWD_OPEN_MODE(buf, O_CREAT | O_RDWR | O_BINARY | O_NOFOLLOW, data->filemode);
#else
		/* Check to make sure that the opened file is not outside of allowable dirs.
		   This is not 100% safe but it's hard to do something better without O_NOFOLLOW */
		if(PG(open_basedir) && lstat(buf, &sbuf) == 0 && S_ISLNK(sbuf.st_mode) && php_check_open_basedir(buf)) {
			return;
		}
		data->fd = VCWD_OPEN_MODE(buf, O_CREAT | O_RDWR | O_BINARY, data->filemode);
#endif

		if (data->fd != -1) {
#ifndef PHP_WIN32
			/* check that this session file was created by us or root – we
			   don't want to end up accepting the sessions of another webapp */
			if (fstat(data->fd, &sbuf) || (sbuf.st_uid != 0 && sbuf.st_uid != getuid() && sbuf.st_uid != geteuid())) {
				close(data->fd);
				data->fd = -1;
				return;
			}
#endif
			do {
				ret = flock(data->fd, LOCK_EX);
			} while (ret == -1 && errno == EINTR);

#ifdef F_SETFD
# ifndef FD_CLOEXEC
#  define FD_CLOEXEC 1
# endif
			if (fcntl(data->fd, F_SETFD, FD_CLOEXEC)) {
				php_error_docref(NULL, E_WARNING, "fcntl(%d, F_SETFD, FD_CLOEXEC) failed: %s (%d)", data->fd, strerror(errno), errno);
			}
#endif
		} else {
			php_error_docref(NULL, E_WARNING, "open(%s, O_RDWR) failed: %s (%d)", buf, strerror(errno), errno);
		}
	}
}

static int ps_files_write(ps_files *data, zend_string *key, zend_string *val)
{
	zend_long n;

	/* PS(id) may be changed by calling session_regenerate_id().
	   Re-initialization should be tried here. ps_files_open() checks
       data->lastkey and reopen when it is needed. */
	ps_files_open(data, key->val);
	if (data->fd < 0) {
		return FAILURE;
	}

	/* Truncate file if the amount of new data is smaller than the existing data set. */
	if (val->len < (int)data->st_size) {
		php_ignore_value(ftruncate(data->fd, 0));
	}

#if defined(HAVE_PWRITE)
	n = pwrite(data->fd, val->val, val->len, 0);
#else
	lseek(data->fd, 0, SEEK_SET);
	n = write(data->fd, val->val, val->len);
#endif

	if (n != val->len) {
		if (n == -1) {
			php_error_docref(NULL, E_WARNING, "write failed: %s (%d)", strerror(errno), errno);
		} else {
			php_error_docref(NULL, E_WARNING, "write wrote less bytes than requested");
		}
		return FAILURE;
	}

	return SUCCESS;
}

static int ps_files_cleanup_dir(const char *dirname, int maxlifetime)
{
	DIR *dir;
	char dentry[sizeof(struct dirent) + MAXPATHLEN];
	struct dirent *entry = (struct dirent *) &dentry;
	zend_stat_t sbuf;
	char buf[MAXPATHLEN];
	time_t now;
	int nrdels = 0;
	size_t dirname_len;

	dir = opendir(dirname);
	if (!dir) {
		php_error_docref(NULL, E_NOTICE, "ps_files_cleanup_dir: opendir(%s) failed: %s (%d)", dirname, strerror(errno), errno);
		return (0);
	}

	time(&now);

	dirname_len = strlen(dirname);

	/* Prepare buffer (dirname never changes) */
	memcpy(buf, dirname, dirname_len);
	buf[dirname_len] = PHP_DIR_SEPARATOR;

	while (php_readdir_r(dir, (struct dirent *) dentry, &entry) == 0 && entry) {
		/* does the file start with our prefix? */
		if (!strncmp(entry->d_name, FILE_PREFIX, sizeof(FILE_PREFIX) - 1)) {
			size_t entry_len = strlen(entry->d_name);

			/* does it fit into our buffer? */
			if (entry_len + dirname_len + 2 < MAXPATHLEN) {
				/* create the full path.. */
				memcpy(buf + dirname_len + 1, entry->d_name, entry_len);

				/* NUL terminate it and */
				buf[dirname_len + entry_len + 1] = '\0';

				/* check whether its last access was more than maxlifetime ago */
				if (VCWD_STAT(buf, &sbuf) == 0 &&
						(now - sbuf.st_mtime) > maxlifetime) {
					VCWD_UNLINK(buf);
					nrdels++;
				}
			}
		}
	}

	closedir(dir);

	return (nrdels);
}

static int ps_files_key_exists(ps_files *data, const char *key)
{
	char buf[MAXPATHLEN];
	zend_stat_t sbuf;

	if (!key || !ps_files_path_create(buf, sizeof(buf), data, key)) {
		return FAILURE;
	}
	if (VCWD_STAT(buf, &sbuf)) {
		return FAILURE;
	}
	return SUCCESS;
}


#define PS_FILES_DATA ps_files *data = PS_GET_MOD_DATA()


/*
 * Open save handler. Setup resources that are needed by the handler.
 * PARAMETERS: PS_OPEN_ARGS in php_session.h
 * RETURN VALUE: SUCCESS or FAILURE. Must set non-NULL valid module data
 * (void **mod_data) with SUCCESS, NULL(default) for FAILUREs.
 *
 * Files save handler checks/create save_path directory and setup ps_files data.
 * Note that files save handler supports splitting session data into multiple
 * directories.
 * *mod_data, *save_path, *session_name are guaranteed to have non-NULL values.
 */
PS_OPEN_FUNC(files)
{
	ps_files *data;
	const char *p, *last;
	const char *argv[3];
	int argc = 0;
	size_t dirdepth = 0;
	int filemode = 0600;

	if (*save_path == '\0') {
		/* if save path is an empty string, determine the temporary dir */
		save_path = php_get_temporary_directory();

		if (php_check_open_basedir(save_path)) {
			return FAILURE;
		}
	}

	/* split up input parameter */
	last = save_path;
	p = strchr(save_path, ';');
	while (p) {
		argv[argc++] = last;
		last = ++p;
		p = strchr(p, ';');
		if (argc > 1) break;
	}
	argv[argc++] = last;

	if (argc > 1) {
		errno = 0;
		dirdepth = (size_t) ZEND_STRTOL(argv[0], NULL, 10);
		if (errno == ERANGE) {
			php_error(E_WARNING, "The first parameter in session.save_path is invalid");
			return FAILURE;
		}
	}

	if (argc > 2) {
		errno = 0;
		filemode = ZEND_STRTOL(argv[1], NULL, 8);
		if (errno == ERANGE || filemode < 0 || filemode > 07777) {
			php_error(E_WARNING, "The second parameter in session.save_path is invalid");
			return FAILURE;
		}
	}
	save_path = argv[argc - 1];

	data = ecalloc(1, sizeof(*data));

	data->fd = -1;
	data->dirdepth = dirdepth;
	data->filemode = filemode;
	data->basedir_len = strlen(save_path);
	data->basedir = estrndup(save_path, data->basedir_len);

	if (PS_GET_MOD_DATA()) {
		ps_close_files(mod_data);
	}
	PS_SET_MOD_DATA(data);

	return SUCCESS;
}


/*
 * Clean up opened resources.
 * PARAMETERS: PS_CLOSE_ARGS in php_session.h
 * RETURN VALUE: SUCCESS. Must set PS module data(void **mod_data) to NULL.
 *
 * Files save handler closes open files and it's memory.
 * *mod_data is guaranteed to have non-NULL value.
 * PS_CLOSE_FUNC() must set *mod_data to NULL. PS_CLOSE_FUNC() should not
 * fail.
 */
PS_CLOSE_FUNC(files)
{
	PS_FILES_DATA;

	ps_files_close(data);

	if (data->lastkey) {
		efree(data->lastkey);
	}

	efree(data->basedir);
	efree(data);
	*mod_data = NULL;

	return SUCCESS;
}


/*
 * Read session data from opened resource.
 * PARAMETERS: PS_READ_ARGS in php_session.h
 * RETURN VALUE: SUCCESS or FAILURE. Must set non-NULL session data to (zend_string **val)
 * for SUCCESS. NULL(default) for FAILUREs.
 *
 * Files save handler supports splitting session data into multiple
 * directories.
 * *mod_data, *key are guaranteed to have non-NULL values.
 */
PS_READ_FUNC(files)
{
	zend_long n;
	zend_stat_t sbuf;
	PS_FILES_DATA;

	ps_files_open(data, key->val);
	if (data->fd < 0) {
		return FAILURE;
	}

	if (zend_fstat(data->fd, &sbuf)) {
		return FAILURE;
	}

	data->st_size = sbuf.st_size;

	if (sbuf.st_size == 0) {
		*val = STR_EMPTY_ALLOC();
		return SUCCESS;
	}

	*val = zend_string_alloc(sbuf.st_size, 0);

#if defined(HAVE_PREAD)
	n = pread(data->fd, (*val)->val, (*val)->len, 0);
#else
	lseek(data->fd, 0, SEEK_SET);
	n = read(data->fd, (*val)->val, (*val)->len);
#endif

	if (n != sbuf.st_size) {
		if (n == -1) {
			php_error_docref(NULL, E_WARNING, "read failed: %s (%d)", strerror(errno), errno);
		} else {
			php_error_docref(NULL, E_WARNING, "read returned less bytes than requested");
		}
		zend_string_release(*val);
		*val =  STR_EMPTY_ALLOC();
		return FAILURE;
	}

	return SUCCESS;
}


/*
 * Write session data.
 * PARAMETERS: PS_WRITE_ARGS in php_session.h
 * RETURN VALUE: SUCCESS or FAILURE.
 *
 * PS_WRITE_FUNC() must write session data(zend_string *val) unconditionally.
 * *mod_data, *key, *val are guaranteed to have non-NULL values.
 */
PS_WRITE_FUNC(files)
{
	PS_FILES_DATA;

	return ps_files_write(data, key, val);
}


/*
 * Update session data modification/access time stamp.
 * PARAMETERS: PS_UPDATE_TIMESTAMP_ARGS in php_session.h
 * RETURN VALUE: SUCCESS or FAILURE.
 *
 * PS_UPDATE_TIMESTAMP_FUNC() updates time stamp(mtime) so that active session
 * data files will not be purged by GC. If session data storage does not need to
 * update timestamp, it should return SUCCESS simply. (e.g. Memcache)
 * *mod_data, *key, *val are guaranteed to have non-NULL values.
 *
 * NOTE: Updating access timestamp at PS_READ_FUNC() may extend life of obsolete
 * session data. Use of PS_UPDATE_TIMESTAMP_FUNC() is preferred whenever it is
 * possible.
 */
PS_UPDATE_TIMESTAMP_FUNC(files)
{
	char buf[MAXPATHLEN];
	struct utimbuf newtimebuf;
	struct utimbuf *newtime = &newtimebuf;
	int ret;
	PS_FILES_DATA;

	if (!ps_files_path_create(buf, sizeof(buf), data, key->val)) {
		return FAILURE;
	}

	/* Update mtime */
#ifdef HAVE_UTIME_NULL
	newtime = NULL;
#else
	newtime->modtime = newtime->actime = time(NULL);
#endif
	ret = VCWD_UTIME(buf, newtime);
	if (ret == -1) {
		/* New session ID, create data file */
		return ps_files_write(data, key, val);
	}

	return SUCCESS;
}


/*
 * Delete session data.
 * PARAMETERS: PS_DESTROY_ARGS in php_session.h
 * RETURN VALUE: SUCCESS or FAILURE.
 *
 * PS_DESTROY_FUNC() must remove the session data specified by *key from
 * session data storage unconditionally. It must not return FAILURE for
 * non-existent session data.
 * *mod_data, *key are guaranteed to have non-NULL values.
 */
PS_DESTROY_FUNC(files)
{
	char buf[MAXPATHLEN];
	PS_FILES_DATA;

	if (!ps_files_path_create(buf, sizeof(buf), data, key->val)) {
		return FAILURE;
	}

	if (data->fd != -1) {
		ps_files_close(data);

		if (VCWD_UNLINK(buf) == -1) {
			/* This is a little safety check for instances when we are dealing with a regenerated session
			 * that was not yet written to disk. */
			if (!VCWD_ACCESS(buf, F_OK)) {
				return FAILURE;
			}
		}
	}

	return SUCCESS;
}


/*
 * Cleanup expired session data.
 * PARAMETERS: PS_GC_ARGS in php_session.h
 * RETURN VALUE: SUCCESS or FAILURE. Number of deleted records(int *nrdels(default=-1)).
 *
 * PS_GC_FUNC() must remove session data that are not accessed
 * 'session.maxlifetime'(seconds). If storage does not need manual GC, it
 * may return SUCCESS simply. (e.g. Memcache) It must set number of records
 * deleted(nrdels).
 * *mod_data is guaranteed to have non-NULL value.
 */
PS_GC_FUNC(files)
{
	PS_FILES_DATA;

	/* We don't perform any cleanup, if dirdepth is larger than 0.
	   we return SUCCESS, since all cleanup should be handled by
	   an external entity (i.e. find -ctime x | xargs rm) */

	if (data->dirdepth == 0) {
		*nrdels = ps_files_cleanup_dir(data->basedir, maxlifetime);
	}

	return SUCCESS;
}


/*
 * Create session ID.
 * PARAMETERS: PS_CREATE_SID_ARGS in php_session.h
 * RETURN VALUE: Valid session ID(zend_string *) or NULL for FAILURE.
 *
 * PS_CREATE_SID_FUNC() must check collision. i.e. Check session data if
 * new sid exists already.
 * *mod_data is guaranteed to have non-NULL value.
 * NOTE: Default php_session_create_id() does not check collision. If
 * NULL is returned, session module create new ID by using php_session_create_id().
 * If php_session_create_id() fails due to invalid configuration, it raises E_ERROR.
 * NULL return value checks from php_session_create_id() is not required generally.
 */
PS_CREATE_SID_FUNC(files)
{
	zend_string *sid;
	int maxfail = 3;
	PS_FILES_DATA;

	do {
		sid = php_session_create_id((void**)&data);
		if (!sid) {
			if (--maxfail < 0) {
				return NULL;
			} else {
				continue;
			}
		}
		/* Check collision */
		/* FIXME: mod_data(data) should not be NULL (User handler could be NULL) */
		if (data && ps_files_key_exists(data, sid->val) == SUCCESS) {
			if (sid) {
				zend_string_release(sid);
				sid = NULL;
			}
			if (--maxfail < 0) {
				return NULL;
			}
		}
	} while(!sid);

	return sid;
}


/*
 * Check session ID existence for use_strict_mode support.
 * PARAMETERS: PS_VALIDATE_SID_ARGS in php_session.h
 * RETURN VALUE: SUCCESS or FAILURE.
 *
 * Return SUCCESS for valid key(already existing session).
 * Return FAILURE for invalid key(non-existing session).
 * *mod_data, *key are guaranteed to have non-NULL values.
 */
PS_VALIDATE_SID_FUNC(files)
{
	PS_FILES_DATA;

	return ps_files_key_exists(data, key->val);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
