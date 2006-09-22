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
   | Author: Thies C. Arntzen <thies@thieso.net>                          |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* {{{ includes/startup/misc */

#include "php.h"
#include "fopen_wrappers.h"
#include "file.h"
#include "php_dir.h"
#include "php_scandir.h"

#ifdef HAVE_DIRENT_H
#include <dirent.h>
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
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find my handle property"); \
				RETURN_FALSE; \
			} \
			ZEND_FETCH_RESOURCE(dirp, php_stream *, tmp, -1, "Directory", php_file_le_stream()); \
		} else { \
			ZEND_FETCH_RESOURCE(dirp, php_stream *, 0, DIRG(default_dir), "Directory", php_file_le_stream()); \
		} \
	} else if ((ZEND_NUM_ARGS() != 1) || zend_get_parameters_ex(1, &id) == FAILURE) { \
		WRONG_PARAM_COUNT; \
	} else { \
		dirp = (php_stream *) zend_fetch_resource(id TSRMLS_CC, -1, "Directory", NULL, 1, php_file_le_stream()); \
		if (!dirp) \
			RETURN_FALSE; \
	} 

static zend_function_entry php_dir_class_functions[] = {
	PHP_FALIAS(close,	closedir,	NULL)
	PHP_FALIAS(rewind,	rewinddir,	NULL)
	PHP_NAMED_FE(read,  php_if_readdir, NULL)
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
	static char dirsep_str[2], pathsep_str[2];
	zend_class_entry dir_class_entry;

	INIT_CLASS_ENTRY(dir_class_entry, "Directory", php_dir_class_functions);
	dir_class_entry_ptr = zend_register_internal_class(&dir_class_entry TSRMLS_CC);

#ifdef ZTS
	ts_allocate_id(&dir_globals_id, sizeof(php_dir_globals), NULL, NULL);
#endif

	dirsep_str[0] = DEFAULT_SLASH;
	dirsep_str[1] = '\0';
	REGISTER_STRING_CONSTANT("DIRECTORY_SEPARATOR", dirsep_str, CONST_CS|CONST_PERSISTENT);

	pathsep_str[0] = ZEND_PATHS_SEPARATOR;
	pathsep_str[1] = '\0';
	REGISTER_STRING_CONSTANT("PATH_SEPARATOR", pathsep_str, CONST_CS|CONST_PERSISTENT);

#ifdef HAVE_GLOB
#ifdef GLOB_BRACE
	REGISTER_LONG_CONSTANT("GLOB_BRACE", GLOB_BRACE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef GLOB_MARK
	REGISTER_LONG_CONSTANT("GLOB_MARK", GLOB_MARK, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef GLOB_NOSORT
	REGISTER_LONG_CONSTANT("GLOB_NOSORT", GLOB_NOSORT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef GLOB_NOCHECK
	REGISTER_LONG_CONSTANT("GLOB_NOCHECK", GLOB_NOCHECK, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef GLOB_NOESCAPE
	REGISTER_LONG_CONSTANT("GLOB_NOESCAPE", GLOB_NOESCAPE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef GLOB_ERR
	REGISTER_LONG_CONSTANT("GLOB_ERR", GLOB_ERR, CONST_CS | CONST_PERSISTENT);
#endif

#ifndef GLOB_ONLYDIR
#define GLOB_ONLYDIR (1<<30)
#define GLOB_EMULATE_ONLYDIR
#define GLOB_FLAGMASK (~GLOB_ONLYDIR)
#else
#define GLOB_FLAGMASK (~0)
#endif

	REGISTER_LONG_CONSTANT("GLOB_ONLYDIR", GLOB_ONLYDIR, CONST_CS | CONST_PERSISTENT);

#endif /* HAVE_GLOB */

	return SUCCESS;
}
/* }}} */

/* {{{ internal functions */
static void _php_do_opendir(INTERNAL_FUNCTION_PARAMETERS, int createobject)
{
	UChar *udir;
	char *dir;
	int dir_len, udir_len;
	zend_uchar dir_type;
	zval *zcontext = NULL;
	php_stream_context *context = NULL;
	php_stream *dirp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t|r", &dir, &dir_len, &zcontext) == FAILURE) {
		return;
	}

	RETVAL_FALSE;

	/* Save for later */
	udir = (UChar*)dir;
	udir_len = dir_len;

	if (zcontext) {
		context = php_stream_context_from_zval(zcontext, 0);
	}

	if (dir_type == IS_UNICODE) {
		if (FAILURE == php_stream_path_encode(NULL, &dir, &dir_len, udir, udir_len, REPORT_ERRORS, context)) {
			goto opendir_cleanup;
		}
	}

	dirp = php_stream_opendir(dir, REPORT_ERRORS, context);

	if (dirp == NULL) {
		goto opendir_cleanup;
	}
		
	php_set_default_dir(dirp->rsrc_id TSRMLS_CC);

	if (createobject) {
		object_init_ex(return_value, dir_class_entry_ptr);
		if (dir_type == IS_UNICODE) {
			add_property_unicodel(return_value, "path", udir, udir_len, 1);
		} else {
			add_property_stringl(return_value, "path", dir, dir_len, 1);
		}
		add_property_resource(return_value, "handle", dirp->rsrc_id);
		php_stream_auto_cleanup(dirp); /* so we don't get warnings under debug */
	} else {
		php_stream_to_zval(dirp, return_value);
	}

opendir_cleanup:
	if (dir_type == IS_UNICODE) {
		efree(dir);
	}
}
/* }}} */

/* {{{ proto mixed opendir(string path[, resource context]) U
   Open a directory and return a dir_handle */
PHP_FUNCTION(opendir)
{
	_php_do_opendir(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto object dir(string directory[, resource context]) U
   Directory class with properties, handle and class and methods read, rewind and close */
PHP_FUNCTION(getdir)
{
	_php_do_opendir(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto void closedir([resource dir_handle]) U
   Close directory connection identified by the dir_handle */
PHP_FUNCTION(closedir)
{
	zval **id, **tmp, *myself;
	php_stream *dirp;

	FETCH_DIRP();

	if (dirp->rsrc_id == DIRG(default_dir)) {
		php_set_default_dir(-1 TSRMLS_CC);
	}

	zend_list_delete(dirp->rsrc_id);
}
/* }}} */

#if defined(HAVE_CHROOT) && !defined(ZTS) && ENABLE_CHROOT_FUNC
/* {{{ proto bool chroot(string directory) U
   Change root directory */
PHP_FUNCTION(chroot)
{
	char *str;
	int ret, str_len;
	zend_uchar str_type;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &str, &str_len, &str_type) == FAILURE) {
		return;
	}

	if (str_type == IS_UNICODE) {
		if (FAILURE == php_stream_path_encode(NULL, &str, &str_len, (UChar*)str, str_len, REPORT_ERRORS, FG(default_context))) {
			RETURN_FALSE;
		}
	}
	
	ret = chroot(str);
	if (str_type == IS_UNICODE) {
		efree(str);
	}
	
	if (ret != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s (errno %d)", strerror(errno), errno);
		RETURN_FALSE;
	}

	ret = chdir("/");
	
	if (ret != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s (errno %d)", strerror(errno), errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto bool chdir(string directory) U
   Change the current directory */
PHP_FUNCTION(chdir)
{
	char *str;
	int ret, str_len;
	zend_uchar str_type;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "t", &str, &str_len, &str_type) == FAILURE) {
		return;
	}

	if (str_type == IS_UNICODE) {
		if (FAILURE == php_stream_path_encode(NULL, &str, &str_len, (UChar*)str, str_len, REPORT_ERRORS, FG(default_context))) {
			RETURN_FALSE;
		}
	}

	ret = VCWD_CHDIR(str);
	if (str_type == IS_UNICODE) {
		efree(str);
	}
	
	if (ret != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s (errno %d)", strerror(errno), errno);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed getcwd(void) U
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
#endif

	if (ret) {
		RETURN_RT_STRING(path, ZSTR_DUPLICATE);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void rewinddir([resource dir_handle]) U
   Rewind dir_handle back to the start */
PHP_FUNCTION(rewinddir)
{
	zval **id, **tmp, *myself;
	php_stream *dirp;
	
	FETCH_DIRP();

	php_stream_rewinddir(dirp);
}
/* }}} */

/* {{{ proto string readdir([resource dir_handle]) U
   Read directory entry from dir_handle */
PHP_NAMED_FUNCTION(php_if_readdir)
{
	zval **id, **tmp, *myself;
	php_stream *dirp;
	php_stream_dirent entry;

	FETCH_DIRP();

	if (php_stream_readdir(dirp, &entry)) {
		RETURN_RT_STRINGL(entry.d_name, strlen(entry.d_name), ZSTR_DUPLICATE);
	}
	RETURN_FALSE;
}
/* }}} */

#ifdef HAVE_GLOB
/* {{{ proto array glob(string pattern [, int flags])
   Find pathnames matching a pattern */
PHP_FUNCTION(glob)
{
	char cwd[MAXPATHLEN];
	int cwd_skip = 0;
#ifdef ZTS
	char work_pattern[MAXPATHLEN];
	char *result;
#endif
	char *pattern = NULL;
	int pattern_len;
	long flags = 0;
	glob_t globbuf;
	unsigned int n;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &pattern, &pattern_len, &flags) == FAILURE) 
		return;

#ifdef ZTS 
	if (!IS_ABSOLUTE_PATH(pattern, pattern_len)) {
		result = VCWD_GETCWD(cwd, MAXPATHLEN);	
		if (!result) {
			cwd[0] = '\0';
		}
#ifdef PHP_WIN32
		if (IS_SLASH(*pattern)) {
			cwd[2] = '\0';
		}
#endif
		cwd_skip = strlen(cwd)+1;

		snprintf(work_pattern, MAXPATHLEN, "%s%c%s", cwd, DEFAULT_SLASH, pattern);
		pattern = work_pattern;
	} 
#endif

	globbuf.gl_offs = 0;
	if (0 != (ret = glob(pattern, flags & GLOB_FLAGMASK, NULL, &globbuf))) {
#ifdef GLOB_NOMATCH
		if (GLOB_NOMATCH == ret) {
			/* Some glob implementation simply return no data if no matches
			   were found, others return the GLOB_NOMATCH error code.
			   We don't want to treat GLOB_NOMATCH as an error condition
			   so that PHP glob() behaves the same on both types of 
			   implementations and so that 'foreach (glob() as ...'
			   can be used for simple glob() calls without further error
			   checking.
			*/
			array_init(return_value);
			return;
		}
#endif
		RETURN_FALSE;
	}

	/* now catch the FreeBSD style of "no matches" */
	if (!globbuf.gl_pathc || !globbuf.gl_pathv) {
		array_init(return_value);
		return;
	}

	/* we assume that any glob pattern will match files from one directory only
	   so checking the dirname of the first match should be sufficient */
	strncpy(cwd, globbuf.gl_pathv[0], MAXPATHLEN);

	if (php_check_open_basedir(cwd TSRMLS_CC)) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (n = 0; n < globbuf.gl_pathc; n++) {
		/* we need to do this everytime since GLOB_ONLYDIR does not guarantee that
		 * all directories will be filtered. GNU libc documentation states the
		 * following: 
		 * If the information about the type of the file is easily available 
		 * non-directories will be rejected but no extra work will be done to 
		 * determine the information for each file. I.e., the caller must still be 
		 * able to filter directories out. 
		 */
		if (flags & GLOB_ONLYDIR) {
			struct stat s;

			if (0 != VCWD_STAT(globbuf.gl_pathv[n], &s)) {
				continue;
			}

			if (S_IFDIR != (s.st_mode & S_IFMT)) {
				continue;
			}
		}
		add_next_index_rt_string(return_value, globbuf.gl_pathv[n]+cwd_skip, 1);
	}

	globfree(&globbuf);
}
/* }}} */
#endif 

/* {{{ proto array scandir(string dir [, int sorting_order [, resource context]])
   List files & directories inside the specified path */
PHP_FUNCTION(scandir)
{
	char *dirn;
	int dirn_len;
	long flags = 0;
	char **namelist;
	int n, i;
	zval *zcontext = NULL;
	php_stream_context *context = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|lr", &dirn, &dirn_len, &flags, &zcontext) == FAILURE) {
		return;
	}

	if (zcontext) {
		context = php_stream_context_from_zval(zcontext, 0);
	}

	if (!flags) {
		n = php_stream_scandir(dirn, &namelist, context, (void *) php_stream_dirent_alphasort);
	} else {
		n = php_stream_scandir(dirn, &namelist, context, (void *) php_stream_dirent_alphasortr);
	}
	if (n < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "(errno %d): %s", errno, strerror(errno));
		RETURN_FALSE;
	}
	
	array_init(return_value);

	for (i = 0; i < n; i++) {
		add_next_index_rt_string(return_value, namelist[i], 0);
		if (UG(unicode)) {
			efree(namelist[i]);
		}
	}

	if (n) {
		efree(namelist);
	}
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
