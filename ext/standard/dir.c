/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
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
   | Authors:                                                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "fopen-wrappers.h"

#include "php3_dir.h"

#if HAVE_DIRENT_H
#include <dirent.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <errno.h>

#if MSVC5
#if !(APACHE)
#define NEEDRDH 1
#endif
#include "win32/readdir.h"
#endif

#ifndef THREAD_SAFE
static int dirp_id = 0;
static int le_dirp;
#endif

function_entry php3_dir_functions[] = {
	{"opendir",		php3_opendir,	NULL},
	{"closedir",	php3_closedir,	NULL},
	{"chdir",		php3_chdir,		NULL},
	{"rewinddir",	php3_rewinddir,	NULL},
	{"readdir",		php3_readdir,	NULL},
	{"dir",			php3_getdir,	NULL},
	{NULL, NULL, NULL}
};

php3_module_entry php3_dir_module_entry = {
	"PHP_dir", php3_dir_functions, php3_minit_dir, NULL, NULL, NULL, NULL, STANDARD_MODULE_PROPERTIES
};


int php3_minit_dir(INIT_FUNC_ARGS)
{
	TLS_VARS;
	
	GLOBAL(le_dirp) = register_list_destructors(closedir,NULL);
	return SUCCESS;
}

/* {{{ proto int opendir(string path)
   Open a directory and return a dir_handle */
void php3_opendir(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg;
	DIR *dirp;
	int ret;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	/* Check open_basedir */
	if (_php3_check_open_basedir(arg->value.str.val)) RETURN_FALSE;
	
	dirp = opendir(arg->value.str.val);
	if (!dirp) {
		php3_error(E_WARNING, "OpenDir: %s (errno %d)", strerror(errno),errno);
		RETURN_FALSE;
	}
	ret = php3_list_insert(dirp, GLOBAL(le_dirp));
	GLOBAL(dirp_id) = ret;
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto void closedir([int dir_handle])
Close directory connection identified by the dir_handle */
void php3_closedir(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *id, *tmp;
	int id_to_find;
	DIR *dirp;
	int dirp_type;
	TLS_VARS;
	
	if (ARG_COUNT(ht) == 0) {
		if (getThis(&id) == SUCCESS) {
			if (_php3_hash_find(id->value.ht, "handle", sizeof("handle"), (void **)&tmp) == FAILURE) {
				php3_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = tmp->value.lval;
		} else {
			id_to_find = GLOBAL(dirp_id);
		}
	} else if ((ARG_COUNT(ht) != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
		
	dirp = (DIR *)php3_list_find(id_to_find, &dirp_type);
	if (!dirp || dirp_type != GLOBAL(le_dirp)) {
		php3_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
	}
	php3_list_delete(id_to_find);
}
/* }}} */

/* {{{ proto int chdir(string directory)
Change the current directory */
void php3_chdir(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg;
	int ret;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);
	ret = chdir(arg->value.str.val);

	if (ret < 0) {
		php3_error(E_WARNING, "ChDir: %s (errno %d)", strerror(errno), errno);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void rewinddir([int dir_handle])
Rewind dir_handle back to the start */
void php3_rewinddir(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *id, *tmp;
	int id_to_find;
	DIR *dirp;
	int dirp_type;
	TLS_VARS;
	
	if (ARG_COUNT(ht) == 0) {
		if (getThis(&id) == SUCCESS) {
			if (_php3_hash_find(id->value.ht, "handle", sizeof("handle"), (void **)&tmp) == FAILURE) {
				php3_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = tmp->value.lval;
		} else {
			id_to_find = GLOBAL(dirp_id);
		}
	} else if ((ARG_COUNT(ht) != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
		
	dirp = (DIR *)php3_list_find(id_to_find, &dirp_type);
	if (!dirp || dirp_type != GLOBAL(le_dirp)) {
		php3_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
	}
	rewinddir(dirp);
}
/* }}} */

/* {{{ proto string readdir([int dir_handle])
Read directory entry from dir_handle */
void php3_readdir(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *id, *tmp;
	int id_to_find;
	DIR *dirp;
	int dirp_type;
	struct dirent *direntp;
	TLS_VARS;
	
	if (ARG_COUNT(ht) == 0) {
		if (getThis(&id) == SUCCESS) {
			if (_php3_hash_find(id->value.ht, "handle", sizeof("handle"), (void **)&tmp) == FAILURE) {
				php3_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = tmp->value.lval;
		} else {
			id_to_find = GLOBAL(dirp_id);
		}
	} else if ((ARG_COUNT(ht) != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
		
	dirp = (DIR *)php3_list_find(id_to_find, &dirp_type);
	if (!dirp || dirp_type != GLOBAL(le_dirp)) {
		php3_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
	}
	direntp = readdir(dirp);
	if (direntp) {
		RETURN_STRINGL(direntp->d_name, strlen(direntp->d_name), 1);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto class dir(string directory)
Directory class with properties, handle and class and methods read, rewind and close */
void php3_getdir(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg;
	DIR *dirp;
	int ret;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	/* Check open_basedir */
	if (_php3_check_open_basedir(arg->value.str.val)) RETURN_FALSE;
	
	dirp = opendir(arg->value.str.val);
	if (!dirp) {
		php3_error(E_WARNING, "OpenDir: %s (errno %d)", strerror(errno), errno);
		RETURN_FALSE;
	}
	ret = php3_list_insert(dirp, GLOBAL(le_dirp));
	GLOBAL(dirp_id) = ret;

	/* construct an object with some methods */
	object_init(return_value);
	add_property_long(return_value, "handle", ret);
	add_property_stringl(return_value, "path", arg->value.str.val, arg->value.str.len, 1);
	add_method(return_value, "read", php3_readdir);
	add_method(return_value, "rewind", php3_rewinddir);
	add_method(return_value, "close", php3_closedir);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
