/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 4.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <ss@2ns.de>                                 |
   +----------------------------------------------------------------------+
 */


#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "php.h"
#include "php_session.h"
#include "../standard/flock_compat.h"

typedef struct {
	int fd;
	char *lastkey;
	char *basedir;
} ps_files;

ps_module ps_mod_files = {
	PS_MOD(files)
};

#define PS_FILES_DATA ps_files *data = *mod_data

PS_OPEN_FUNC(files)
{
	ps_files *data;

	data = ecalloc(sizeof(*data), 1);
	*mod_data = data;

	data->fd = -1;
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

static void _ps_files_open(ps_files *data, const char *key)
{
	char buf[PATH_MAX];
	
	if(data->fd < 0 || !data->lastkey || strcmp(key, data->lastkey)) {
		if(data->lastkey) efree(data->lastkey);
		if(data->fd > -1) close(data->fd);
		
		data->lastkey = estrdup(key);
		
		snprintf(buf, PATH_MAX, "%s/%s", data->basedir, key);
		data->fd = open(buf, O_CREAT | O_RDWR, 0600);
		if(data->fd > -1) {
			flock(data->fd, LOCK_EX);
		}
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

PS_DELETE_FUNC(files)
{
	char buf[PATH_MAX];
	PS_FILES_DATA;

	snprintf(buf, PATH_MAX, "%s/%s", data->basedir, key);
	unlink(buf);

	return SUCCESS;
}

PS_GC_FUNC(files) 
{
	return SUCCESS;
}
