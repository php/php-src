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
   | Authors:                                                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "fopen-wrappers.h"

#include "php3_dir.h"

#ifdef HAVE_DIRENT_H
# include <dirent.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <errno.h>

#if WIN32||WINNT
#define NEEDRDH 1
#include "win32/readdir.h"
#endif

static int dirp_id = 0;
static int le_dirp;
static zend_class_entry *dir_class_entry_ptr;

static zend_function_entry php_dir_functions[] = {
	PHP_FE(opendir,				NULL)
	PHP_FE(closedir,			NULL)
	PHP_FE(chdir,				NULL)
	PHP_FE(rewinddir,			NULL)
	PHP_FE(readdir,				NULL)
	PHP_FALIAS(dir,		getdir,	NULL)
	{NULL, NULL, NULL}
};


static zend_function_entry php_dir_class_functions[] = {
	PHP_FALIAS(close,	closedir,	NULL)
	PHP_FALIAS(rewind,	rewinddir,	NULL)
	PHP_FALIAS(read,	readdir,	NULL)
	{NULL, NULL, NULL}
};


php3_module_entry php3_dir_module_entry = {
	"PHP_dir", php_dir_functions, PHP_MINIT(dir), NULL, NULL, NULL, NULL, STANDARD_MODULE_PROPERTIES
};


PHP_MINIT_FUNCTION(dir)
{
	zend_class_entry dir_class_entry;

	le_dirp = register_list_destructors(closedir,NULL);

	INIT_CLASS_ENTRY(dir_class_entry, "Directory", php_dir_class_functions);
	dir_class_entry_ptr = register_internal_class(&dir_class_entry);
	return SUCCESS;
}

/* {{{ proto int opendir(string path)
   Open a directory and return a dir_handle */
PHP_FUNCTION(opendir)
{
	pval *arg;
	DIR *dirp;
	int ret;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	/* Check open_basedir */
	if (_php3_check_open_basedir(arg->value.str.val)) RETURN_FALSE;
	
	dirp = opendir(arg->value.str.val);
	if (!dirp) {
		php_error(E_WARNING, "OpenDir: %s (errno %d)", strerror(errno),errno);
		RETURN_FALSE;
	}
	ret = php3_list_insert(dirp, le_dirp);
	dirp_id = ret;
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto void closedir([int dir_handle])
Close directory connection identified by the dir_handle */
PHP_FUNCTION(closedir)
{
	pval *id, **tmp;
	int id_to_find;
	DIR *dirp;
	int dirp_type;
	
	if (ARG_COUNT(ht) == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "handle", sizeof("handle"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
		} else {
			id_to_find = dirp_id;
		}
	} else if ((ARG_COUNT(ht) != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
		
	dirp = (DIR *)php3_list_find(id_to_find, &dirp_type);
	if (!dirp || dirp_type != le_dirp) {
		php_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
	}
	php3_list_delete(id_to_find);
}
/* }}} */

/* {{{ proto int chdir(string directory)
Change the current directory */
PHP_FUNCTION(chdir)
{
	pval *arg;
	int ret;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);
	ret = chdir(arg->value.str.val);

	if (ret < 0) {
		php_error(E_WARNING, "ChDir: %s (errno %d)", strerror(errno), errno);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void rewinddir([int dir_handle])
Rewind dir_handle back to the start */
PHP_FUNCTION(rewinddir)
{
	pval *id, **tmp;
	int id_to_find;
	DIR *dirp;
	int dirp_type;
	
	if (ARG_COUNT(ht) == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "handle", sizeof("handle"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
		} else {
			id_to_find = dirp_id;
		}
	} else if ((ARG_COUNT(ht) != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
		
	dirp = (DIR *)php3_list_find(id_to_find, &dirp_type);
	if (!dirp || dirp_type != le_dirp) {
		php_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
		RETURN_FALSE;
	}
	rewinddir(dirp);
}
/* }}} */

/* {{{ proto string readdir([int dir_handle])
Read directory entry from dir_handle */
PHP_FUNCTION(readdir)
{
	pval *id, **tmp;
	int id_to_find;
	DIR *dirp;
	int dirp_type;
	struct dirent *direntp;
	
	if (ARG_COUNT(ht) == 0) {
		id = getThis();
		if (id) {
			if (zend_hash_find(id->value.obj.properties, "handle", sizeof("handle"), (void **)&tmp) == FAILURE) {
				php_error(E_WARNING, "unable to find my handle property");
				RETURN_FALSE;
			}
			id_to_find = (*tmp)->value.lval;
		} else {
			id_to_find = dirp_id;
		}
	} else if ((ARG_COUNT(ht) != 1) || getParameters(ht, 1, &id) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(id);
		id_to_find = id->value.lval;
	}
		
	dirp = (DIR *)php3_list_find(id_to_find, &dirp_type);
	if (!dirp || dirp_type != le_dirp) {
		php_error(E_WARNING, "unable to find identifier (%d)", id_to_find);
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
PHP_FUNCTION(getdir)
{
	pval *arg;
	DIR *dirp;
	int ret;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	/* Check open_basedir */
	if (_php3_check_open_basedir(arg->value.str.val)) RETURN_FALSE;
	
	dirp = opendir(arg->value.str.val);
	if (!dirp) {
		php_error(E_WARNING, "OpenDir: %s (errno %d)", strerror(errno), errno);
		RETURN_FALSE;
	}
	ret = php3_list_insert(dirp, le_dirp);
	dirp_id = ret;

	/* construct an object with some methods */
	object_init_ex(return_value, dir_class_entry_ptr);
	add_property_long(return_value, "handle", ret);
	add_property_stringl(return_value, "path", arg->value.str.val, arg->value.str.len, 1);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
