/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <ss@2ns.de>                                 |
   +----------------------------------------------------------------------+
 */


#include "php.h"

#include <sys/stat.h>
#include <sys/types.h>

#if HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#include <fcntl.h>

#include "php_session.h"
#include "mod_files.h"
#include "ext/standard/flock_compat.h"

typedef struct {
	int fd;
	char *lastkey;
	char *basedir;
	int dirdepth;
} ps_files;


ps_module ps_mod_files = {
	PS_MOD(files)
};


#define PS_FILES_DATA ps_files *data = *mod_data

PS_OPEN_FUNC(files)
{
	ps_files *data;
	char *p;

	data = ecalloc(sizeof(*data), 1);
	*mod_data = data;

	data->fd = -1;
	if((p = strchr(save_path, ':'))) {
		data->dirdepth = strtol(save_path, NULL, 10);
		save_path = p + 1;
	}
	data->basedir = estrdup(save_path);
	
	return SUCCESS;
}

PS_CLOSE_FUNC(files)
{
	PS_FILES_DATA;

	if(data->fd > -1) close(data->fd);
	if(data->lastkey) efree(data->lastkey);
	efree(data->basedir);
	efree(data);
	*mod_data = NULL;

	return SUCCESS;
}

#if WIN32|WINNT
#define DIR_DELIMITER '\\'
#else
#define DIR_DELIMITER '/'
#endif

static void _ps_files_open(ps_files *data, const char *key)
{
	char buf[MAXPATHLEN];
	const char *p;
	int i;
	int n;
	int keylen;

	if(data->fd < 0 || !data->lastkey || strcmp(key, data->lastkey)) {
		if(data->lastkey) efree(data->lastkey);
		if(data->fd > -1) close(data->fd);

		keylen = strlen(key);
		if(keylen <= data->dirdepth || MAXPATHLEN < 
				(strlen(data->basedir) + 2 * data->dirdepth + keylen + 5)) 
			return;
		p = key;
		n = sprintf(buf, "%s/", data->basedir);
		for(i = 0; i < data->dirdepth; i++) {
			buf[n++] = *p++;
			buf[n++] = DIR_DELIMITER;
		}
		strcat(buf, p);
		
		data->lastkey = estrdup(key);
		
#ifdef O_EXCL
		data->fd = open(buf, O_EXCL | O_RDWR | O_CREAT, 0600);
		/* -1, if file exists and access failed due to O_EXCL|O_CREAT */
		if(data->fd == -1) {
			data->fd = open(buf, O_EXCL | O_RDWR);
		}
#else
		data->fd = open(buf, O_CREAT | O_RDWR, 0600);
		if(data->fd != -1) {
			flock(data->fd, LOCK_EX);
		}
#endif
	}
}

PS_READ_FUNC(files)
{
	int n;
	struct stat sbuf;
	PS_FILES_DATA;

	_ps_files_open(data, key);
	if(data->fd < 0)
		return FAILURE;
	
	if(fstat(data->fd, &sbuf)) {
		return FAILURE;
	}
	
	lseek(data->fd, 0, SEEK_SET);

	*vallen = sbuf.st_size;
	*val = emalloc(sbuf.st_size);

	n = read(data->fd, *val, sbuf.st_size);
	if(n != sbuf.st_size) {
		efree(*val);
		return FAILURE;
	}
	
	return SUCCESS;
}

PS_WRITE_FUNC(files)
{
	PS_FILES_DATA;

	_ps_files_open(data, key);
	if(data->fd < 0) 
		return FAILURE;

	ftruncate(data->fd, 0);
	lseek(data->fd, 0, SEEK_SET);
	write(data->fd, val, vallen);

	return SUCCESS;
}

PS_DESTROY_FUNC(files)
{
	char buf[MAXPATHLEN];
	PS_FILES_DATA;

	snprintf(buf, MAXPATHLEN, "%s/%s", data->basedir, key);
	unlink(buf);

	return SUCCESS;
}

PS_GC_FUNC(files) 
{
	
	return SUCCESS;
}
