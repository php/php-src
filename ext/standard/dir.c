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
#include "file.h"
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

#ifdef HAVE_GLOB
#ifndef PHP_WIN32
#include <glob.h>
#else
#include "win32/glob.h"
#endif
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

#if 0
typedef struct {
	int id;
	DIR *dir;
} php_dir;

static int le_dirp;
#endif

static zend_class_entry *dir_class_entry_ptr;

#define FETCH_DIRP() \
	if (ZEND_NUM_ARGS() == 0) { \
		myself = getThis(); \
		if (myself) { \
			if (zend_hash_find(Z_OBJPROP_P(myself), "handle", sizeof("handle"), (void **)&tmp) == FAILURE) { \
				php_error(E_WARNING, "%s(): Unable to find my handle property", get_active_function_name(TSRMLS_C)); \
				RETURN_FALSE; \
			} \
			ZEND_FETCH_RESOURCE(dirp, php_stream *, tmp, -1, "Directory", php_file_le_stream()); \
		} else { \
			ZEND_FETCH_RESOURCE(dirp, php_stream *, 0, DIRG(default_dir), "Directory", php_file_le_stream()); \
		} \
	} else if ((ZEND_NUM_ARGS() != 1) || zend_get_parameters_ex(1, &id) == FAILURE) { \
		WRONG_PARAM_COUNT; \
	} else { \
		ZEND_FETCH_RESOURCE(dirp, php_stream *, id,-1, "Directory", php_file_le_stream()); \
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

PHP_RINIT_FUNCTION(dir)
{
	DIRG(default_dir) = -1;
	return SUCCESS;
}

PHP_MINIT_FUNCTION(dir)
{
	static char tmpstr[2];
	zend_class_entry dir_class_entry;

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
	php_stream *dirp;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg);
	
	dirp = php_stream_opendir(Z_STRVAL_PP(arg), ENFORCE_SAFE_MODE|REPORT_ERRORS, NULL);

	if (dirp == NULL) {
		RETURN_FALSE;
	}
		
	php_set_default_dir(dirp->rsrc_id TSRMLS_CC);

	if (createobject) {
		object_init_ex(return_value, dir_class_entry_ptr);
		add_property_stringl(return_value, "path", Z_STRVAL_PP(arg), Z_STRLEN_PP(arg), 1);
		add_property_resource(return_value, "handle", dirp->rsrc_id);
		zend_list_addref(dirp->rsrc_id); /* might not be needed */
		php_stream_auto_cleanup(dirp); /* so we don't get warnings under debug */
	} else {
		php_stream_to_zval(dirp, return_value);
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
	php_stream *dirp;

	FETCH_DIRP();

	zend_list_delete(dirp->rsrc_id);

	if (dirp->rsrc_id == DIRG(default_dir)) {
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
		php_error(E_WARNING, "%s(): %s (errno %d)", get_active_function_name(TSRMLS_C), strerror(errno), errno);
		RETURN_FALSE;
	}

	ret = chdir("/");
	
	if (ret != 0) {
		php_error(E_WARNING, "%s(): %s (errno %d)", get_active_function_name(TSRMLS_C), strerror(errno), errno);
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
		php_error(E_WARNING, "%s(): %s (errno %d)", get_active_function_name(TSRMLS_C), strerror(errno), errno);
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
	php_stream *dirp;
	
	FETCH_DIRP();

	php_stream_rewinddir(dirp);
}
/* }}} */

/* {{{ proto string readdir([resource dir_handle])
   Read directory entry from dir_handle */

PHP_NAMED_FUNCTION(php_if_readdir)
{
	pval **id, **tmp, *myself;
	php_stream *dirp;
	php_stream_dirent entry;

	FETCH_DIRP();

	if (php_stream_readdir(dirp, &entry)) {
		RETURN_STRINGL(entry.d_name, strlen(entry.d_name), 1);
	}
	RETURN_FALSE;
}

/* }}} */

#ifdef HAVE_GLOB
/* {{{ proto array glob(string pattern [, int flags])
    */
PHP_FUNCTION(glob)
{
	char *pattern = NULL;
	int pattern_len;
	long flags = 0;
	glob_t globbuf;
	int n, ret;

	if (PG(safe_mode)) {
		php_error(E_WARNING, "%s(): Safe Mode restriction in effect, function is disabled",
				  get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &pattern, &pattern_len, &flags) == FAILURE) 
		return;

	globbuf.gl_offs = 0;
	if (0 != (ret = glob(pattern, flags, NULL, &globbuf))) {
#ifdef GLOB_NOMATCH
		if (GLOB_NOMATCH == ret) {
			/* Linux handles no matches as an error condition, but FreeBSD
			 * doesn't. This ensure that if no match is found, an empty array
			 * is always returned so it can be used with worrying in e.g.
			 * foreach() */
			array_init(return_value);
			return;
		}
#endif
		RETURN_FALSE;
	}

	array_init(return_value);
	for (n = 0; n < globbuf.gl_pathc; n++) {
		add_next_index_string(return_value, globbuf.gl_pathv[n], 1);
	}
	globfree(&globbuf);
}
/* }}} */
#endif 

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
