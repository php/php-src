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

#define FILE_PREFIX "sess_"

typedef struct {
	int fd;
	char *lastkey;
	char *basedir;
	size_t basedir_len;
	size_t dirdepth;
	size_t st_size;
} ps_files;

ps_module ps_mod_files = {
	PS_MOD(files)
};

/* If you change the logic here, please also update the error message in
 * ps_files_open() appropriately */
static int ps_files_valid_key(const char *key)
{
	size_t len;
	const char *p;
	char c;
	int ret = 1;

	for (p = key; (c = *p); p++) {
		/* valid characters are a..z,A..Z,0..9 */
		if (!((c >= 'a' && c <= 'z') ||
				(c >= 'A' && c <= 'Z') ||
				(c >= '0' && c <= '9'))) {
			ret = 0;
			break;
		}
	}

	len = p - key;
	
	if (len == 0)
		ret = 0;
	
	return ret;
}

static char *ps_files_path_create(char *buf, size_t buflen, ps_files *data, const char *key)
{
	size_t key_len;
	const char *p;
	int i;
	int n;
	
	key_len = strlen(key);
	if (key_len <= data->dirdepth || buflen < 
			(strlen(data->basedir) + 2 * data->dirdepth + key_len + 5 + sizeof(FILE_PREFIX))) 
		return NULL;
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
#define O_BINARY 0
#endif 

static void ps_files_close(ps_files *data)
{
	if (data->fd != -1) {
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
		
		if (!ps_files_valid_key(key)) {
			php_error(E_WARNING, "The session id contains illegal characters, valid characters are only a-z, A-Z and 0-9");
			return;
		}
		if (!ps_files_path_create(buf, sizeof(buf), data, key))
			return;
		
		data->lastkey = estrdup(key);
		
		data->fd = VCWD_OPEN_MODE(buf, O_CREAT | O_RDWR | O_BINARY, 0600);
		
		if (data->fd != -1) {
			flock(data->fd, LOCK_EX);

#ifdef F_SETFD
			if (fcntl(data->fd, F_SETFD, 1)) {
				php_error(E_WARNING, "fcntl(%d, F_SETFD, 1) failed: %s (%d)", data->fd, strerror(errno), errno);
			}
#endif
		} else {
			php_error(E_WARNING, "open(%s, O_RDWR) failed: %s (%d)", buf, 
					strerror(errno), errno);
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
		php_error(E_NOTICE, "ps_files_cleanup_dir: opendir(%s) failed: %s (%d)\n", dirname, strerror(errno), errno);
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
			size_t entry_len;

			entry_len = strlen(entry->d_name);
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

#define PS_FILES_DATA ps_files *data = PS_GET_MOD_DATA()

PS_OPEN_FUNC(files)
{
	ps_files *data;
	char *p;

	data = ecalloc(sizeof(*data), 1);
	PS_SET_MOD_DATA(data);

	data->fd = -1;
	if ((p = strchr(save_path, ';'))) {
		data->dirdepth = (size_t) strtol(save_path, NULL, 10);
		save_path = p + 1;
	}
	data->basedir_len = strlen(save_path);
	data->basedir = estrndup(save_path, data->basedir_len);
	
	return SUCCESS;
}

PS_CLOSE_FUNC(files)
{
	PS_FILES_DATA;

	ps_files_close(data);

	if (data->lastkey) 
		efree(data->lastkey);
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

	ps_files_open(data, key TSRMLS_CC);
	if (data->fd < 0)
		return FAILURE;
	
	if (fstat(data->fd, &sbuf))
		return FAILURE;
	
	data->st_size = *vallen = sbuf.st_size;
	*val = emalloc(sbuf.st_size);

#if defined(HAVE_WORKING_PREAD_TEST) && defined(HAVE_PREAD)
	n = pread(data->fd, *val, sbuf.st_size, 0);
#else
	lseek(data->fd, 0, SEEK_SET);
	n = read(data->fd, *val, sbuf.st_size);
#endif

	if (n != sbuf.st_size) {
		if (n == -1)
			php_error(E_WARNING, "read failed: %s (%d)", strerror(errno), errno);
		else
			php_error(E_WARNING, "read returned less bytes than requested");
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
	if (data->fd < 0)
		return FAILURE;

	/* 
	 * truncate file, if the amount of new data is smaller than
	 * the existing data set.
	 */
	
	if (vallen < (int)data->st_size)
		ftruncate(data->fd, 0);

#if defined(HAVE_WORKING_PWRITE_TEST) && defined(HAVE_PWRITE)
	n = pwrite(data->fd, val, vallen, 0);
#else
	lseek(data->fd, 0, SEEK_SET);
	n = write(data->fd, val, vallen);
#endif

	if (n != vallen) {
		if (n == -1)
			php_error(E_WARNING, "write failed: %s (%d)", strerror(errno), errno);
		else
			php_error(E_WARNING, "write wrote less bytes than requested");
		return FAILURE;
	}

	return SUCCESS;
}

PS_DESTROY_FUNC(files)
{
	char buf[MAXPATHLEN];
	PS_FILES_DATA;

	if (!ps_files_path_create(buf, sizeof(buf), data, key))
		return FAILURE;
	
	ps_files_close(data);
	
	if (VCWD_UNLINK(buf) == -1) {
		return FAILURE;
	}

	return SUCCESS;
}

PS_GC_FUNC(files) 
{
	PS_FILES_DATA;
	
	/* we don't perform any cleanup, if dirdepth is larger than 0.
	   we return SUCCESS, since all cleanup should be handled by
	   an external entity (i.e. find -ctime x | xargs rm) */
	   
	if (data->dirdepth == 0)
		*nrdels = ps_files_cleanup_dir(data->basedir, maxlifetime TSRMLS_CC);
	
	return SUCCESS;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
