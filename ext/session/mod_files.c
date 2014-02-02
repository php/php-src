/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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

/* $Id$ */

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

typedef struct {
	int fd;
	char *lastkey;
	char *basedir;
	size_t basedir_len;
	size_t dirdepth;
	size_t st_size;
	int filemode;
} ps_files;

ps_module ps_mod_files = {
	PS_MOD_SID(files)
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

static void ps_files_open(ps_files *data, const char *key TSRMLS_DC)
{
	char buf[MAXPATHLEN];

	if (data->fd < 0 || !data->lastkey || strcmp(key, data->lastkey)) {
		if (data->lastkey) {
			efree(data->lastkey);
			data->lastkey = NULL;
		}

		ps_files_close(data);

		if (php_session_valid_key(key) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The session id is too long or contains illegal characters, valid characters are a-z, A-Z, 0-9 and '-,'");
			return;
		}

		if (!ps_files_path_create(buf, sizeof(buf), data, key)) {
			return;
		}

		data->lastkey = estrdup(key);

		data->fd = VCWD_OPEN_MODE(buf, O_CREAT | O_RDWR | O_BINARY, data->filemode);

		if (data->fd != -1) {
#ifndef PHP_WIN32
			/* check to make sure that the opened file is not a symlink, linking to data outside of allowable dirs */
			if (PG(open_basedir)) {
				struct stat sbuf;

				if (fstat(data->fd, &sbuf)) {
					close(data->fd);
					data->fd = -1;
					return;
				}
				if (S_ISLNK(sbuf.st_mode) && php_check_open_basedir(buf TSRMLS_CC)) {
					close(data->fd);
					data->fd = -1;
					return;
				}
			}
#endif
			flock(data->fd, LOCK_EX);

#ifdef F_SETFD
# ifndef FD_CLOEXEC
#  define FD_CLOEXEC 1
# endif
			if (fcntl(data->fd, F_SETFD, FD_CLOEXEC)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "fcntl(%d, F_SETFD, FD_CLOEXEC) failed: %s (%d)", data->fd, strerror(errno), errno);
			}
#endif
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "open(%s, O_RDWR) failed: %s (%d)", buf, strerror(errno), errno);
		}
	}
}

static int ps_files_cleanup_dir(const char *dirname, int maxlifetime TSRMLS_DC)
{
	DIR *dir;
	char dentry[sizeof(struct dirent) + MAXPATHLEN];
	struct dirent *entry = (struct dirent *) &dentry;
	struct stat sbuf;
	char buf[MAXPATHLEN];
	time_t now;
	int nrdels = 0;
	size_t dirname_len;

	dir = opendir(dirname);
	if (!dir) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ps_files_cleanup_dir: opendir(%s) failed: %s (%d)", dirname, strerror(errno), errno);
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

				/* check whether its last access was more than maxlifet ago */
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

static int ps_files_key_exists(ps_files *data, const char *key TSRMLS_DC)
{
	char buf[MAXPATHLEN];
	struct stat sbuf;

	if (!key || !ps_files_path_create(buf, sizeof(buf), data, key)) {
		return FAILURE;
	}
	if (VCWD_STAT(buf, &sbuf)) {
		return FAILURE;
	}
	return SUCCESS;
}


#define PS_FILES_DATA ps_files *data = PS_GET_MOD_DATA()

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
		save_path = php_get_temporary_directory(TSRMLS_C);

		if (php_check_open_basedir(save_path TSRMLS_CC)) {
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
		dirdepth = (size_t) strtol(argv[0], NULL, 10);
		if (errno == ERANGE) {
			php_error(E_WARNING, "The first parameter in session.save_path is invalid");
			return FAILURE;
		}
	}

	if (argc > 2) {
		errno = 0;
		filemode = strtol(argv[1], NULL, 8);
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
		ps_close_files(mod_data TSRMLS_CC);
	}
	PS_SET_MOD_DATA(data);

	return SUCCESS;
}

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

PS_READ_FUNC(files)
{
	long n;
	struct stat sbuf;
	PS_FILES_DATA;

	/* If strict mode, check session id existence */
	if (PS(use_strict_mode) &&
		ps_files_key_exists(data, key TSRMLS_CC) == FAILURE) {
		/* key points to PS(id), but cannot change here. */
		if (key) {
			efree(PS(id));
			PS(id) = NULL;
		}
		PS(id) = PS(mod)->s_create_sid((void **)&data, NULL TSRMLS_CC);
		if (!PS(id)) {
			return FAILURE;
		}
		if (PS(use_cookies)) {
			PS(send_cookie) = 1;
		}
		php_session_reset_id(TSRMLS_C);
		PS(session_status) = php_session_active;
	}

	ps_files_open(data, PS(id) TSRMLS_CC);
	if (data->fd < 0) {
		return FAILURE;
	}

	if (fstat(data->fd, &sbuf)) {
		return FAILURE;
	}

	data->st_size = *vallen = sbuf.st_size;

	if (sbuf.st_size == 0) {
		*val = STR_EMPTY_ALLOC();
		return SUCCESS;
	}

	*val = emalloc(sbuf.st_size);

#if defined(HAVE_PREAD)
	n = pread(data->fd, *val, sbuf.st_size, 0);
#else
	lseek(data->fd, 0, SEEK_SET);
	n = read(data->fd, *val, sbuf.st_size);
#endif

	if (n != sbuf.st_size) {
		if (n == -1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "read failed: %s (%d)", strerror(errno), errno);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "read returned less bytes than requested");
		}
		efree(*val);
		return FAILURE;
	}

	return SUCCESS;
}

PS_WRITE_FUNC(files)
{
	long n;
	PS_FILES_DATA;

	ps_files_open(data, key TSRMLS_CC);
	if (data->fd < 0) {
		return FAILURE;
	}

	/* Truncate file if the amount of new data is smaller than the existing data set. */

	if (vallen < (int)data->st_size) {
		php_ignore_value(ftruncate(data->fd, 0));
	}

#if defined(HAVE_PWRITE)
	n = pwrite(data->fd, val, vallen, 0);
#else
	lseek(data->fd, 0, SEEK_SET);
	n = write(data->fd, val, vallen);
#endif

	if (n != vallen) {
		if (n == -1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "write failed: %s (%d)", strerror(errno), errno);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "write wrote less bytes than requested");
		}
		return FAILURE;
	}

	return SUCCESS;
}

PS_DESTROY_FUNC(files)
{
	char buf[MAXPATHLEN];
	PS_FILES_DATA;

	if (!ps_files_path_create(buf, sizeof(buf), data, key)) {
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

PS_GC_FUNC(files)
{
	PS_FILES_DATA;

	/* we don't perform any cleanup, if dirdepth is larger than 0.
	   we return SUCCESS, since all cleanup should be handled by
	   an external entity (i.e. find -ctime x | xargs rm) */

	if (data->dirdepth == 0) {
		*nrdels = ps_files_cleanup_dir(data->basedir, maxlifetime TSRMLS_CC);
	}

	return SUCCESS;
}

PS_CREATE_SID_FUNC(files)
{
	char *sid;
	int maxfail = 3;
	PS_FILES_DATA;

	do {
		sid = php_session_create_id((void **)&data, newlen TSRMLS_CC);
		/* Check collision */
		if (data && ps_files_key_exists(data, sid TSRMLS_CC) == SUCCESS) {
			if (sid) {
				efree(sid);
				sid = NULL;
			}
			if (!(maxfail--)) {
				return NULL;
			}
		}
	} while(!sid);

	return sid;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
