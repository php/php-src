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
   | Author: Thies C. Arntzen <thies@thieso.net>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* {{{ includes/startup/misc */

#include "php.h"
#include "fopen_wrappers.h"

#include "php_dir.h"

#ifdef HAVE_DIRENT_H
# include <dirent.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <errno.h>

#ifdef PHP_WIN32
#include "win32/readdir.h"
#endif

typedef struct {
	int default_dir;
} php_dir_globals;

#ifdef ZTS
#define DIRG(v) TSRMG(dir_globals_id, php_dir_globals *, v)
int dir_globals_id;
#else
#define DIRG(v) (dir_globals.v)
php_dir_globals dir_globals;
#endif

typedef struct {
	int id;
	DIR *dir;
} php_dir;

static int le_dirp;

static zend_class_entry *dir_class_entry_ptr;

#define FETCH_DIRP() \
	if (ZEND_NUM_ARGS() == 0) { \
		myself = getThis(); \
		if (myself) { \
			if (zend_hash_find(Z_OBJPROP_P(myself), "handle", sizeof("handle"), (void **)&tmp) == FAILURE) { \
				php_error(E_WARNING, "unable to find my handle property"); \
				RETURN_FALSE; \
			} \
			ZEND_FETCH_RESOURCE(dirp, php_dir *, tmp, -1, "Directory", le_dirp); \
		} else { \
			ZEND_FETCH_RESOURCE(dirp, php_dir *, 0, DIRG(default_dir), "Directory", le_dirp); \
		} \
	} else if ((ZEND_NUM_ARGS() != 1) || zend_get_parameters_ex(1, &id) == FAILURE) { \
		WRONG_PARAM_COUNT; \
	} else { \
		ZEND_FETCH_RESOURCE(dirp, php_dir *, id,-1, "Directory", le_dirp); \
	} 

static zend_function_entry php_dir_class_functions[] = {
	PHP_FALIAS(close,	closedir,	NULL)
	PHP_FALIAS(rewind,	rewinddir,	NULL)
	PHP_STATIC_FE("read", php_if_readdir, NULL)
	{NULL, NULL, NULL}
};


static void php_set_default_dir(int id TSRMLS_DC)
{
    if (DIRG(default_dir)!=-1) {
        zend_list_delete(DIRG(default_dir));
    }

	if (id != -1) {
		zend_list_addref(id);
	}
	
	DIRG(default_dir) = id;
}


static void _dir_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_dir *dirp = (php_dir *)rsrc->ptr;

	closedir(dirp->dir);
	efree(dirp);
}

PHP_RINIT_FUNCTION(dir)
{
	DIRG(default_dir) = -1;
	return SUCCESS;
}

PHP_MINIT_FUNCTION(dir)
{
	static char tmpstr[2];
	zend_class_entry dir_class_entry;

	le_dirp = zend_register_list_destructors_ex(_dir_dtor, NULL, "dir", module_number);

	INIT_CLASS_ENTRY(dir_class_entry, "Directory", php_dir_class_functions);
	dir_class_entry_ptr = zend_register_internal_class(&dir_class_entry TSRMLS_CC);

#ifdef ZTS
	ts_allocate_id(&dir_globals_id, sizeof(php_dir_globals), NULL, NULL);
#endif
	tmpstr[0] = DEFAULT_SLASH;
	tmpstr[1] = '\0';
	REGISTER_STRING_CONSTANT("DIRECTORY_SEPARATOR", tmpstr, CONST_CS|CONST_PERSISTENT);

	return SUCCESS;
}

/* }}} */
/* {{{ internal functions */

static void _php_do_opendir(INTERNAL_FUNCTION_PARAMETERS, int createobject)
{
	pval **arg;
	php_dir *dirp;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg);

	if (php_check_open_basedir(Z_STRVAL_PP(arg) TSRMLS_CC)) {
		RETURN_FALSE;
	}
	
	if (PG(safe_mode) &&(!php_checkuid(Z_STRVAL_PP(arg), NULL, CHECKUID_ALLOW_ONLY_FILE))) {
		RETURN_FALSE;
	}
	
	dirp = emalloc(sizeof(php_dir));

	dirp->dir = VCWD_OPENDIR(Z_STRVAL_PP(arg));

#ifdef PHP_WIN32
	if (!dirp->dir || dirp->dir->finished) {
		if (dirp->dir) {
			closedir(dirp->dir);
		}
#else
	if (!dirp->dir) {
#endif
		efree(dirp);
		php_error(E_WARNING, "OpenDir: %s (errno %d)", strerror(errno), errno);
		RETURN_FALSE;
	}

	dirp->id = zend_list_insert(dirp, le_dirp);

	php_set_default_dir(dirp->id TSRMLS_CC);

	if (createobject) {
		object_init_ex(return_value, dir_class_entry_ptr);
		add_property_stringl(return_value, "path", Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), 1);
		add_property_resource(return_value, "handle", dirp->id);
	} else {
		RETURN_RESOURCE(dirp->id);
	}
}

/* }}} */
/* {{{ proto mixed opendir(string path)
   Open a directory and return a dir_handle */

PHP_FUNCTION(opendir)
{
	_php_do_opendir(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

/* }}} */
/* {{{ proto class dir(string directory)
   Directory class with properties, handle and class and methods read, rewind and close */

PHP_FUNCTION(getdir)
{
	_php_do_opendir(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

/* }}} */
/* {{{ proto void closedir([resource dir_handle])
   Close directory connection identified by the dir_handle */

PHP_FUNCTION(closedir)
{
	pval **id, **tmp, *myself;
	php_dir *dirp;

	FETCH_DIRP();

	zend_list_delete(dirp->id);

	if (dirp->id == DIRG(default_dir)) {
		php_set_default_dir(-1 TSRMLS_CC);
	}
}

/* }}} */

#if defined(HAVE_CHROOT) && !defined(ZTS) && ENABLE_CHROOT_FUNC
/* {{{ proto bool chroot(string directory)
   Change root directory */

PHP_FUNCTION(chroot)
{
	char *str;
	int ret, str_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}
    ret = chroot(str);
	
	if (ret != 0) {
		php_error(E_WARNING, "chroot: %s (errno %d)", strerror(errno), errno);
		RETURN_FALSE;
	}

	ret = chdir("/");
	
	if (ret != 0) {
		php_error(E_WARNING, "chdir: %s (errno %d)", strerror(errno), errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

/* }}} */
#endif

/* {{{ proto bool chdir(string directory)
   Change the current directory */

PHP_FUNCTION(chdir)
{
	char *str;
	int ret, str_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (PG(safe_mode) && !php_checkuid(str, NULL, CHECKUID_ALLOW_ONLY_FILE)) {
		RETURN_FALSE;
	}
	ret = VCWD_CHDIR(str);
	
	if (ret != 0) {
		php_error(E_WARNING, "ChDir: %s (errno %d)", strerror(errno), errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

/* }}} */
/* {{{ proto mixed getcwd(void)
   Gets the current directory */

PHP_FUNCTION(getcwd)
{
	char path[MAXPATHLEN];
	char *ret=NULL;
	
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

#if HAVE_GETCWD
	ret = VCWD_GETCWD(path, MAXPATHLEN);
#elif HAVE_GETWD
	ret = VCWD_GETWD(path);
/*
 * #warning is not ANSI C
 * #else
 * #warning no proper getcwd support for your site
 */
#endif

	if (ret) {
		RETURN_STRING(path, 1);
	} else {
		RETURN_FALSE;
	}
}

/* }}} */
/* {{{ proto void rewinddir([resource dir_handle])
   Rewind dir_handle back to the start */

PHP_FUNCTION(rewinddir)
{
	pval **id, **tmp, *myself;
	php_dir *dirp;
	
	FETCH_DIRP();

	rewinddir(dirp->dir);
}
/* }}} */
/* {{{ proto string readdir([resource dir_handle])
   Read directory entry from dir_handle */

PHP_NAMED_FUNCTION(php_if_readdir)
{
	pval **id, **tmp, *myself;
	php_dir *dirp;
	char entry[sizeof(struct dirent)+MAXPATHLEN];
	struct dirent *result = (struct dirent *)&entry; /* patch for libc5 readdir problems */

	FETCH_DIRP();

	if (php_readdir_r(dirp->dir, (struct dirent *) entry, &result) == 0 && result) {
		RETURN_STRINGL(result->d_name, strlen(result->d_name), 1);
	}
	RETURN_FALSE;
}

/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
